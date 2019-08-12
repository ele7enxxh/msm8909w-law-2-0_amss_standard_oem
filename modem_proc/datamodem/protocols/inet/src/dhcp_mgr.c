/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           D H C P _ M G R . C

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol manager.  This file
  contains all of the external interfaces, task serialization code and
  initialization logic for the different modules.   The internal
  functions run only from the PS task. 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  - The DCHP server is started by calling dhcp_start().  The DHCP
    server handle will be returned in the callback that is registered. 
  - The DHCP server is stoped by calling dhcp_stop(). 
  - The DHCP server must be restarted if the IP address from the PSDN
    changes. 
  - The DHCP server is restarted by calling dhcp_stop(), then
    dhcp_start().

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcp_mgr.c#1 $ 

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/10/14    vl     Optimization of F3 messages
01/03/14    mp     Added the support for SIP server option.
12/12/12    ds     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
10/12/11    am     dhcp_calc_gw_addr would now fetch IP from stored lease.
                   Added server iface to AM init struct.
08/29/11    ag     Added the support for NBNS option.
01/11/11    cp     Reverted back the netmask calculation to be based on 
                   MIN_MASK.
12/22/10    cp     Modified the netmask calculation to be based on IP address
                   class.
11/25/10    sa     Removed the inclusion of rex.h as it is not required.
02/16/10    cp     Changes to propagate the UM iface mtu value to TE.
05/14/09    pp     Moved Local Heap allocations to Modem Heap.
10/23/08    am     Cleanly return NULL if handle alloc fails.
09/26/08    am     Changed malloc/free names for off target support.
10/15/07    ks     Export function to calculate gateway address and mask.
11/27/06    jd     Added DHCP message callbacks
01/18/05    jd     Removed dhcpi_mgr_input_dsm_extract and dhcp_input_dsm
                   as these are now unused as DHCP server uses server socket 
04/15/04    clp    Initial development work done.  Added comments.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "dhcp.h"
#include "dhcpi.h"
#include "ps_iface.h"
#include "err.h"
#include "ps_svc.h"
#include "msg.h"
#include "ps_system_heap.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Handle for the DHCP mgr.  */
typedef struct {
  void     * core;
  void     * am;
  void     * server;
  /* Cookied to verify that a handle passed in came from here.  The
   * cookie_val will contain the value defined below.  The cookie_ptr
   * will contain a pointer to be beginning of the structure.  These
   * are set when the handle is valid.
   */
#define DHCPI_HANDLE_COOKIE ((uint32)0xD4C00C1E)
  uint32     cookie_val;
  void     * cookie_ptr; 
} dhcp_mgr_info;

/* Data structure for passing start data to PS context */
typedef struct {
  ps_iface_type * client;
  ps_iface_type * am;
  dhcp_msg_cback_type  msg_cb;
  dhcp_arp_cache_updt_type arp_cache_update_cb;
  void (*done_cb)(void * userdata, void * handle);
  void * userdata;
} dhcp_mgr_start;

/* Data structure for passing stop data to PS context */
typedef struct {
  void * handle;
  void (*done_cb)(void * userdata, boolean ok);
  void * userdata;
} dhcp_mgr_stop;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
LOCAL FUNCTION DHCPI_MEM_ALLOC()
  Uses Modem Heap.

DESCRIPTION
  This is a wrapper function for mem_malloc() to be passed to each of
  the subsystems in the DHCP server to use.

DEPENDENCIES
  The memory heap must have been initialized previously. 

PARAMETERS
  uint32 count - how many bytes to allocate.

RETURN VALUE
  void * - pointer to newly allocated memory.  NULL if failed.

SIDE EFFECTS
  None.
===========================================================================*/
static void * 
dhcpi_mem_alloc
(
  uint32 count
)
{

  return ps_system_heap_mem_alloc( count );

} /* dhcpi_mem_alloc */

/*===========================================================================
LOCAL FUNCTION DHCPI_MEM_FREE()

DESCRIPTION
  This is a wrapper function for mem_free() to be passed to each of
  the subsystems in the DHCP server to use.

DEPENDENCIES
  The memory heap must have been initialized previously.
  The memory to be freed must have been allocated out of the
  dhcp_mem_heap pool.

PARAMETERS
  void * - The memory to return to the pool.

RETURN VALUE
  None.

SIDE EFFECTS
  Returns the memory to the dhcp_mem_buffer via mem_heap.
===========================================================================*/
static void 
dhcpi_mem_free
(
  void * buf
)
{
  ASSERT(NULL != buf);
  PS_SYSTEM_HEAP_MEM_FREE(buf);

} /* dhcpi_mem_free */

/*===========================================================================
LOCAL FUNCTION DHCPI_MGR_INFO_VERIFY()

DESCRIPTION
  Validate a handle by verifying for non-null, and the cookies are
  good.

DEPENDENCIES
  The handle should be valid!

PARAMETERS
  dhcp_mgr_info * server - The handle to verify

RETURN VALUE
  boolean - TRUE if good.  FALSE (or error fatal/reboot) if failed.

SIDE EFFECTS
  None
===========================================================================*/
static boolean
dhcpi_mgr_info_verify
(
  dhcp_mgr_info * handle 
)
{
  if ( NULL == handle )
  {
    DATA_ERR_FATAL("FAIL: verify handle got a null handle");
    return FALSE;
  }
  if ( handle->cookie_val != DHCPI_HANDLE_COOKIE || 
       handle->cookie_ptr != handle )
  {
    LOG_MSG_ERROR_3( "FAIL: COOKIE check failed 0x%lx %p %p",
                     handle->cookie_val, handle->cookie_ptr, handle );
    ASSERT(0);
    return FALSE;
  }
  return TRUE;
} /* dhcpi_mgr_info_verify */

/*===========================================================================
LOCAL FUNCTION DHCPI_STOP()

DESCRIPTION
  This function will choose the approprate Server Shim, DHCP Address
  Manager and DHCP core.  It will then free these components, ungluing
  them.

DEPENDENCIES
  handle_ptr must be valid.

PARAMETERS
  void * handle - a dhcp_mgr_info ptr to free the components of.

RETURN VALUE
  None.

SIDE EFFECTS
  Frees each of the components of the handle_ptr, and then the handle
  itself.
===========================================================================*/
static void 
dhcpi_stop
( 
  void * handle
)
{
  dhcp_mgr_info * info = handle;

  LOG_MSG_INFO1_1( "dhcpi_stop: called with %p", handle );
  if ( FALSE != dhcpi_mgr_info_verify(info) ) 
  {
    /* Ok, free what there is to free */
    if ( NULL != info->core )
    {
      dhcp_core_stop(&info->core);
    }
    if ( NULL != info->am )
    {
      dhcp_am_mp_stop(&info->am);
    }
    if ( NULL != info->server )
    {
      dhcp_server_stop(&info->server);
    }

    /* Cleaning up cookie values, so dhcpi_mgr_info_verify() fails for sure
     * if dhcp_stop() gets called multiple times for the same handle.
     */
    info->cookie_val = 0;
    info->cookie_ptr = NULL;
    dhcpi_mem_free(info);
  }
  else
  {
    LOG_MSG_ERROR_0("DHCPI_STOP: handle failed sanity check");
  }
} /* dhcpi_stop */

/*===========================================================================
LOCAL FUNCTION DHCPI_START_IFACE()

DESCRIPTION
  This function will choose the approprate Server Shim, DHCP Address
  Manager and DHCP core for IPV4, ps_iface, ppp (single IP address to
  hand out).  It will then initialize these components gluing them
  together.

DEPENDENCIES
  The am interface must be up and configured. 

PARAMETERS
  ps_iface_type * server - The interface for which the DHCP is serving 
                           addresses.
  ps_iface_type * am - The interface to which the server interface
                       will be routed.

RETURN VALUE
  Handle to the new dhcp_mgr, or NULL if failed.

SIDE EFFECTS
  allocates and glues together a new DHCP server from the compentent
  parts. 
===========================================================================*/
static void *
dhcpi_start_iface
( 
  ps_iface_type *      server, 
  ps_iface_type *      am,
  dhcp_msg_cback_type  msg_cb,
  dhcp_arp_cache_updt_type arp_cache_update_cb,
  void *               user_data
)
{
  dhcp_mgr_info    * handle = dhcpi_mem_alloc(sizeof(dhcp_mgr_info));
  void             * ret = NULL;
  dhcp_core_init     core_init;
  dhcp_core_config   core_config;
  dhcp_am_init       am_init;
  dhcp_am_config     am_config;
  dhcp_server_init   server_init;
  dhcp_server_config server_config;

  ASSERT(NULL != server);
  ASSERT(NULL != am);

  core_init.d_malloc = dhcpi_mem_alloc;
  core_init.d_free = dhcpi_mem_free;

  am_init.d_malloc = dhcpi_mem_alloc;
  am_init.d_free = dhcpi_mem_free;
  am_init.iface = am;
  am_init.gw_iface = server;

  server_init.d_malloc = dhcpi_mem_alloc;
  server_init.d_free = dhcpi_mem_free;
  server_init.iface = server;

  if ( NULL == handle )
  {
    DATA_ERR_FATAL("dhcp_start unable to get memory for handle");
    return NULL;
  }
  handle->core = dhcp_core_new(&core_init);
  handle->am = dhcp_am_mp_ps_single_new(&am_init);
  handle->server = dhcp_server_new(&server_init);
  if ( NULL != handle->core && NULL != handle->am && NULL != handle->server )
  {
    core_config.server_reply_new = dhcp_server_reply_new;
    core_config.server_reply_pushdown_tail = dhcp_server_reply_pushdown_tail;
    core_config.server_reply_free = dhcp_server_reply_free;
    core_config.server_reply_send = dhcp_server_reply_send;
    core_config.server_reply_handle = handle->server;

    core_config.lease_get = dhcp_am_mp_get_lease;
    core_config.lease_free = dhcp_am_mp_free_lease;
    core_config.lease_err = dhcp_am_mp_err_lease;
    core_config.lease_handle = handle->am;

    core_config.addr_info_server_id = dhcp_am_mp_addr_info_server_id;
    core_config.addr_info_netmask = dhcp_am_mp_addr_info_netmask;
    core_config.addr_info_handle = handle->am;

    core_config.dns_first = dhcp_am_mp_dns_first;
    core_config.dns_next = dhcp_am_mp_dns_next;
    core_config.dns_handle = handle->am;

    core_config.nbns_first  = dhcp_am_mp_nbns_first;
    core_config.nbns_next   = dhcp_am_mp_nbns_next;
    core_config.nbns_handle = handle->am;
   
    core_config.interface_mtu        = dhcp_am_mp_mtu;
    core_config.interface_mtu_handle = handle->am;
   
    core_config.sip_addr_list = dhcp_am_mp_sip_addr_list;
    core_config.sip_addr_handle = handle->am;
    
    core_config.sip_domain_name_list = dhcp_am_mp_sip_domain_name_list;
    core_config.sip_domain_name_handle = handle->am;
   
    core_config.msg_cback           = msg_cb;
    core_config.msg_cback_user_data = user_data;

    server_config.recv_msg = dhcp_core_input;
    server_config.recv_msg_handle = handle->core;
    server_config.dhcp_mgr_handle = handle; /* for filter */

    server_config.addr_info_server_id = dhcp_am_mp_addr_info_server_id;
    server_config.addr_info_handle = handle->am;

    /* save the arp_cache_updt_cb in the server config, since server needs to 
       update the cache when sending dhcp reply */
    server_config.arp_cache_update_cb = arp_cache_update_cb;

    am_config.addr_free = dhcp_server_addr_free;
    am_config.addr_free_handle = handle->server;

    handle->cookie_val = DHCPI_HANDLE_COOKIE;
    handle->cookie_ptr = handle;
    if ( dhcp_am_mp_start( handle->am, &am_config ) && 
         dhcp_server_start( handle->server, &server_config ) &&
         dhcp_core_start( handle->core, &core_config ) ) 
    {
      ret = handle;
    }
  }
  if ( NULL == ret )
  {
    LOG_MSG_ERROR_0("Failed to start DHCP, cleaning up");
    /* Assiging cookie values so handle gets freed easily */
    handle->cookie_val = DHCPI_HANDLE_COOKIE;
    handle->cookie_ptr = handle;
    dhcpi_stop((void*)handle);
  }
  
  LOG_MSG_INFO1_1( "dhcpi_start_iface: returning with %p", ret );
  return ret;
} /* dhcpi_start_iface */

/*===========================================================================
LOCAL FUNCTION DHCPI_STOP_CMD()

DESCRIPTION
  This function is used to serialize events into the PS task context,
  and will be called when the PS_DHCP_STOP_CMD is recieved.  This
  function will choose the approprate Server Shim, DHCP Address
  Manager and DHCP core.  It will then free these components, ungluing
  them.  This function will also NULL the handle pointer passed in. 

DEPENDENCIES
  The user_data must be valid. 
  The cmd must be a PS_DHCP_STOP_CMD.

PARAMETERS
  ps_cmd_enum_type - The type of command that this is.  
  void * user_data - The data used to process this command.  In this
                     case, this consists of the dhcp_mgr_stop
                     information. 

RETURN VALUE
  None.

SIDE EFFECTS
  The stop data (user_data_ptr) will be freed.
===========================================================================*/
static void 
dhcpi_stop_cmd
( 
  ps_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  dhcp_mgr_stop * stop = user_data_ptr;

  /* we are likely to have hit a data abort above if this is foobar */
  ASSERT( NULL != user_data_ptr );
  ASSERT( PS_DHCP_STOP_CMD == cmd );

  dhcpi_stop( stop->handle );

  if ( NULL != stop->done_cb )
  {
    stop->done_cb(stop->userdata, TRUE);
  }
  dhcpi_mem_free( stop );
} /* dhcpi_stop_cmd */

/*===========================================================================
FUNCTION DHCPI_START_CMD()

DESCRIPTION
  This function is used to serialize events into the PS task context,
  and will be called when the PS_DHCP_START_CMD is recieved.  This
  function will choose the approprate Server Shim, DHCP Address
  Manager and DHCP core for IPV4, ps_iface, ppp (single IP address to
  hand out).  It will then initialize these components gluing them
  together.

DEPENDENCIES
  The am interface must be up and configured. 
  The user_data_ptr must point at a valid dhcp_mgr_start object.
  The cmd must be PS_DHCP_START_CMD.

PARAMETERS
  ps_cmd_enum_type - The type of command that this is.  
  void * user_data - The data used to process this command.  In this
                     case, this consists of the dhcp_mgr_start
                     information. 

RETURN VALUE
  None.

SIDE EFFECTS
  The dhcp_mgr_start object will be freed. 
===========================================================================*/
static void
dhcpi_start_cmd
( 
  ps_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  dhcp_mgr_start * start = user_data_ptr;

  /* we are likely to have hit a data abort above if this is foobar */
  ASSERT( NULL != user_data_ptr );
  ASSERT( PS_DHCP_START_CMD == cmd );
  ASSERT( NULL != start->done_cb );

  /* In the future, somehow intuit which start to call */
  start->done_cb( start->userdata,
                  dhcpi_start_iface( start->client,
                                     start->am,
                                     start->msg_cb,
                                     start->arp_cache_update_cb,
                                     start->userdata ) ); 
                  
  dhcpi_mem_free( start );
} /* dhcpi_start_cmd */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DHCP_START()

DESCRIPTION
  This function serializes the start of a DHCP server through the PS
  task.  This will cause the DHCP server to be configured for the
  client and AM interfaces specified and the resulting handle to be
  returned in a callback also specified with user data.

DEPENDENCIES
  The am interface must be up and configured. 

PARAMETERS
  ps_iface_type * client - The interface for which the DHCP is serving 
                           addresses.
  ps_iface_type * am - The interface to which the Client interface
                       will be routed.
  done_cb - callback function that will be called when the DHCP has started.
            This may be called before this function has returned. 
            The handle will be NULL on failure. Non-null on success. 
            The userdata will also be passed to this function.
  void * userdata - This is user data that will be passed to done_cb.

RETURN VALUE
  boolean - TRUE if message successfully sent to PS task: Expect a
            callback.  FALSE if message not set to PS task: Do not
            expect a callback.

SIDE EFFECTS
  Causes a message to be sent to the PS task to start a DHCP server
  configured to the specified client and AM interfaces. 
===========================================================================*/
boolean
dhcp_start
( 
  ps_iface_type * client, 
  ps_iface_type * am,
  void (*done_cb)(void * userdata, void * handle),
  dhcp_msg_cback_type msg_cb,
  dhcp_arp_cache_updt_type arp_cache_update_cb,
  void * userdata
)
{
  dhcp_mgr_start * start;
  
  if ( NULL == done_cb )
  {
    DATA_ERR_FATAL("dhcp_start with NULL done_cb");
    return FALSE;
  }

  if ( NULL == client || NULL == am )
  {
    LOG_MSG_ERROR_2( "dhcp_start got invalid ps_iface_handle %p %p",
                     client, am);
    ASSERT(0);
    return FALSE;
  }
  
  start = (dhcp_mgr_start*) dhcpi_mem_alloc( sizeof( dhcp_mgr_start ) );
  if ( NULL == start ) 
  {
    DATA_ERR_FATAL("dhcp_start unable to get parameter memory");
    return FALSE;
  }

  start->client = client;
  start->am = am;
  start->done_cb = done_cb;
  start->msg_cb = msg_cb;
  start->arp_cache_update_cb = arp_cache_update_cb;
  start->userdata = userdata;

  (void) ps_set_cmd_handler( PS_DHCP_START_CMD, dhcpi_start_cmd );
  ps_send_cmd( PS_DHCP_START_CMD, start );
  return TRUE;
} /* dhcp_start */

/*===========================================================================
FUNCTION DHCP_STOP()

DESCRIPTION
  This function serializes the start of a DHCP server through the PS
  task.  This will cause the DHCP server specified to be freed.  The
  callback specifed will be called back with the userdata when this is
  done. The clients handle_ptr will be set to NULL before this
  function returns.

DEPENDENCIES
  The handle must have being allocated by a call to dhcp_start.

PARAMETERS
  void ** handle_ptr - The handle to free. 
  done_cb() - The function to call when done freeing this handle.
              Function will not be called if NULL.
  void * userdata - User specifed data to be passed back when the
                    callback is called.

RETURN VALUE
  boolean - TRUE if message successfully sent to PS task: Expect a
            callback.  FALSE if message not set to PS task: Do not
            expect a callback.

SIDE EFFECTS
  Causes a message to be sent to the PS task to stop the DHCP server
  specifed by the handle passed in.
===========================================================================*/
boolean
dhcp_stop
( 
  void ** handle_ptr,
  void (*done_cb)(void * userdata, boolean ok),
  void * userdata
)
{
  dhcp_mgr_stop * stop;

  ASSERT( NULL != handle_ptr );
  
  if ( NULL == *handle_ptr )
  {
    LOG_MSG_ERROR_0("dhcp_stop called with NULL handle");
    return FALSE;
  }

  if ( FALSE == dhcpi_mgr_info_verify(*handle_ptr) ) 
  {
    LOG_MSG_ERROR_1("dhcp_stop called with invalid handle %p", *handle_ptr);
    ASSERT(0);
    return FALSE;
  }

  stop = (dhcp_mgr_stop*) dhcpi_mem_alloc( sizeof( dhcp_mgr_stop ) );
  if ( NULL == stop )
  {
    DATA_ERR_FATAL("dhcp_stop unable to get parameter memory");
    return FALSE;
  }

  stop->handle = *handle_ptr;
  stop->done_cb = done_cb;
  stop->userdata = userdata;

  (void) ps_set_cmd_handler( PS_DHCP_STOP_CMD, dhcpi_stop_cmd );
  ps_send_cmd( PS_DHCP_STOP_CMD, stop );

  *handle_ptr = NULL;
  return TRUE;
} /* dhcp_stop */

/*===========================================================================
FUNCTION DHCP_CALC_GATEWAY_INFO()

DESCRIPTION
  This function calculates the netmask and server/router IP address
  from the client IP address. This function tries to figure out the
  smallest netmask without handing out the broadcast address.

DEPENDENCIES
  Client IP address must be valid
  Currently only supports IPV4 addrs.  Of course, with IPV6, this
  whole thing is obsolete.

PARAMETERS
  client_ip: Client IP address
  server_id: Gateway IP address (OUT)
  netmask:   Subnet mask (OUT) 

RETURN VALUE
  Server_id and netmask  

SIDE EFFECTS
  None
===========================================================================*/
void dhcp_calc_gateway_info
(
  void  *  handle,
  uint32*  client_ip,
  uint32*  server_id,
  uint32*  netmask
)
{
  dhcp_mgr_info    *info = (dhcp_mgr_info *)handle;
  
  ASSERT( NULL != info );
  ASSERT( NULL != client_ip );
  ASSERT( NULL != server_id );
  ASSERT( NULL != netmask );

  dhcp_am_mp_get_ip_config(info->am, client_ip, server_id, netmask);
} /* dhcp_calculate_gateway_info */
