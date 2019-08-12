/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D H C P _ C L I E N T _ M G R . C

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol manager.  This file
  contains all of the external interfaces, task serialization code and
  initialization logic for the different modules.   The internal
  functions run only from the PS task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  - The DHCP client is started by calling dhcp_client_start().
  - The DHCP client is stoped by calling dhcp_client_stop().

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcp_client_mgr.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/25/14    ds     Removed ASSERT for graceful exit incase DHCP handle is 
                   invalid or already being freed.
04/09/14    ash    Added Prefix Delegation support.
01/10/14    vl     Optimization of F3 messages
09/12/13    sd     Cryptographic vulnerability - replacing ran_next APIs.
07/08/13    cx     Replace memcpy and memmove with memscpy and memsmove
12/12/12    ds     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
11/25/10    sa     Removed the inclusion of rex.h as it is not required.
10/28/09    am     Return TRUE for ongoing det if DHCP start is pending.
08/07/09    kk     Added support for client identifier option (opt type=61).
05/22/09    am     Aligning alloc/free order for Client modules.
05/14/09    pp     Moved Local Heap allocations to Modem Heap.
11/28/08    pp     Removed TASKLOCK() usage from dhcp_client_init.
09/26/08    am     Changed malloc/free names for off target support.
08/25/08    am     Added DHCP ongoing determination.
01/29/08    am     Added Rapid Commit Option handling.
05/02/07    es     Moved set_handle to config layer.
                   Removed ref to dhcp_client_params to fix layering issue.
04/17/07    es     Made changes to support IWLAN - ps_iface configuration
                   according to request flags in params struct, set dhcp
                   handle in ps_iface, unicast addr in dhcp_params struct.
                   Removed changes to support earlier IOCTL.
02/01/07    hm     Added IOCTL to get fresh DHCP configuration
09/25/06    es     Added hook to ps_dnsi for expanding domain names in
                   SIP option.
02/16/05    clp    Updated handle verification for higher priority tasks
11/05/04    clp    Initial development work done.  Added comments.
12/30/04    clp    Code review: static functions now have dhcpi_ prefix
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "dhcp_client.h"
#include "dhcp_clienti.h"
#include "dssocket.h"
#include "err.h"
#include "ps_svc.h"
#include "msg.h"
#include "ps_system_heap.h"
#include "ran.h"
#include "ps_dnsi.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_utils.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* The broadcast address to send the packet to per RFC 2131 */
#define DHCP_CLIENT_BROADCAST_ADDR ((uint32)0xFFFFFFFF)

/* Cookie value for the params struct */
#define DHCPI_CLIENT_PARAMS_HANDLE_COOKIE ((uint32)0xD4C30C5E)

/* Handle for the DHCP mgr.  */
typedef struct dhcp_client_mgr_info {
  void     * core;
  void     * client;
  void     * configuration;
  void     * storage;
  void     * timer;
  /* Cookied to verify that a handle passed in came from here.  The
   * cookie_val will contain the value defined below.  The cookie_ptr
   * will contain a pointer to be beginning of the structure.  These
   * are set when the handle is valid.
   */
#define DHCPI_CLIENT_HANDLE_COOKIE ((uint32)0xDCC00C1E)
#define DHCPI_CLIENT_HANDLE_UNINIT_COOKIE ((uint32)0xBADC00CE)
  uint32     cookie_val;
  struct dhcp_client_mgr_info * cookie_ptr;
} dhcp_client_mgr_info;

/* This matches the RFC length for the hardware address length */
#define DHCPI_CLIENT_HW_ADDR_LEN 16
/* Data structure for passing start data to PS context */
typedef struct {
  dhcp_client_mgr_info * info;
  dss_iface_id_type iface;
  dhcp_client_params params;
  void (*status_cb)(void * userdata, dhcp_client_status_enum status);
  void * userdata;
  /* This is a local copy of the hw addr */
  uint8 hw_addr[DHCPI_CLIENT_HW_ADDR_LEN];
} dhcp_client_mgr_start;

/* Data structure for passing stop, renew, and release data to PS context */
typedef struct {
  dhcp_client_mgr_info * info;
} dhcp_client_mgr_cmd;

/* Data structure for passing inform data to the PS context */
typedef struct {
  dhcp_client_mgr_info * info;
  uint32 addr;
} dhcp_client_mgr_inform;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* Dummy function for linking purposes. dshdr_dhcp_mgr.c calls this function */
void
dhcp_client_init
(
  void
)
{
  //NO-OP
  return;
} /* dhcp_client_init */


/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_MEM_ALLOC()

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
  Allocates memory out of dhcp_client_mem_buffer via mem_heap.
===========================================================================*/
static void *
dhcpi_client_mem_alloc
(
  uint32 count
)
{

  return ps_system_heap_mem_alloc( count );

} /* dhcpi_client_mem_alloc */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_MEM_FREE()

DESCRIPTION
  This is a wrapper function for mem_free() to be passed to each of
  the subsystems in the DHCP server to use.

DEPENDENCIES
  The memory heap must have been initialized previously.
  The memory to be freed must have been allocated out of the
  dhcp_client_mem_heap pool.

PARAMETERS
  void * - The memory to return to the pool.

RETURN VALUE
  None.

SIDE EFFECTS
  Returns the memory to the dhcp_client_mem_buffer via mem_heap.
===========================================================================*/
static void
dhcpi_client_mem_free
(
  void * buf
)
{
  if (NULL == buf)
  {
    DATA_ERR_FATAL("Freeing NULL buffer");
    return;
  }

  PS_SYSTEM_HEAP_MEM_FREE(buf);

} /* dhcpi_client_mem_free */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_MGR_INFO_VERIFY()

DESCRIPTION
  Validate a handle by verifying for non-null, and the cookies are
  good.

DEPENDENCIES
  The handle should be valid!

PARAMETERS
  dhcp_client_mgr_info * server - The handle to verify
  boolean                external - Is this called from an external
                                    interface or internal?

RETURN VALUE
  boolean - TRUE if good.  FALSE (or error fatal/reboot) if failed.

SIDE EFFECTS
  None
===========================================================================*/
static boolean
dhcpi_client_mgr_info_verify
(
  dhcp_client_mgr_info * handle,
  boolean                external
)
{
  if ( NULL == handle )
  {
    DATA_ERR_FATAL("FAIL: verify handle got a null handle");
    return FALSE;
  }
  /* If called from an external interface, the handle may not have
   * had a chance to initialize yet.  So, in this case, allow either
   * the (COOKIE and cookie_ptr) or (UNINIT_COOKIE).  If called from
   * an internal interface, then the handle must have been
   * initialized, so don't allow UNINIT_COOKIE.
   */
  if ( !(
         ( DHCPI_CLIENT_HANDLE_COOKIE == handle->cookie_val &&
           handle->cookie_ptr == handle ) ||
         ( TRUE == external &&
           DHCPI_CLIENT_HANDLE_UNINIT_COOKIE == handle->cookie_val ) ) )
  {
    LOG_MSG_ERROR_3( "FAIL: COOKIE check failed 0x%lx %p %p",
                     handle->cookie_val, handle->cookie_ptr, handle );
    return FALSE;
  }
  return TRUE;
} /* dhcpi_client_mgr_info_verify */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_STOP()

DESCRIPTION
  This function will choose the approprate Server Shim, DHCP Address
  Manager and DHCP core.  It will then free these components, ungluing
  them.

DEPENDENCIES
  handle must be valid.

PARAMETERS
  void * handle - a dhcp_client_mgr_info ptr to free the components of.

RETURN VALUE
  None.

SIDE EFFECTS
  Frees each of the components of the handle, and then the handle
  itself.
===========================================================================*/
static void
dhcpi_client_stop
(
  void * handle
)
{
  dhcp_client_mgr_info * info = handle;

  LOG_MSG_INFO1_1( "dhcpi_client_stop: called with %p", handle );
  if ( FALSE != dhcpi_client_mgr_info_verify(info, FALSE) )
  {
    /* Ok, free what there is to free */
    if ( NULL != info->timer )
    {
      dhcp_clienti_timer_free(&info->timer);
    }

    if ( NULL != info->storage )
    {
      dhcp_clienti_storage_stop(&info->storage);
    }

    if ( NULL != info->configuration )
    {
      dhcp_clienti_configuration_stop(&info->configuration);
    }

    if ( NULL != info->core )
    {
      dhcp_clienti_core_stop(&info->core);
    }

    if ( NULL != info->client )
    {
      dhcp_clienti_client_stop(&info->client);
    }

    dhcpi_client_mem_free(info);
  }
  else
  {
    LOG_MSG_ERROR_0("DHCPI_CLIENT_STOP: handle failed sanity check");
    return;
  }
} /* dhcpi_client_stop */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_START()

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
  void

SIDE EFFECTS
  allocates and glues together a new DHCP server from the compentent
  parts.
===========================================================================*/
static void
dhcpi_client_start
(
  dhcp_client_mgr_info * info,
  dss_iface_id_type iface,
  dhcp_client_params * params,
  void (*status_cb)( void * userdata, dhcp_client_status_enum status),
  void * userdata
)
{
  void                            * ret = NULL;
  dhcp_client_core_init             core_init;
  dhcp_client_core_config           core_config;
  dhcp_client_timer_init            timr_init; /* spelt to avoid conflict */
  dhcp_client_timer_config          timr_config;
  dhcp_client_configuration_init    configuration_init = {0};
  dhcp_client_configuration_config  configuration_config;
  dhcp_client_storage_init          storage_init;
  dhcp_client_client_init           client_init;
  dhcp_client_client_config         client_config;

  /* parameter sanity */
  if (NULL == info )
  {
    DATA_ERR_FATAL("info NULL?");
    return;
  }
  if (NULL == params )
  {
    DATA_ERR_FATAL("params NULL?");
    return;
  }
  if (NULL == status_cb)
  {
    DATA_ERR_FATAL("status callback NULL?");
    return;
  }

  if (( 0 < params->hw_addr_len && NULL == params->hw_addr ) ||
      ( 0 == params->hw_addr_len && NULL != params->hw_addr ))
  {
    DATA_ERR_FATAL("params->hw_addr does not match hw_addr_len");
    return;
  }
  
  /* Set up init structures */
  core_init.d_malloc = dhcpi_client_mem_alloc;
  core_init.d_free = dhcpi_client_mem_free;
  (void) ps_utils_generate_rand_num( (void*)&(core_init.xid), 
                                     sizeof(core_init.xid) );
  core_init.status_cb = status_cb;
  core_init.userdata = userdata;
  /* Note: ps_dnsi_domain_name_expand handles compressed domain messages
   * and message pointers assuming DNS message format defined in RFC 1035,
   * Section 4.1. */
  core_init.domain_name_expand_cb = ps_dnsi_domain_name_expand;

  /* Copy core params */
  core_init.core_params.hw_type = params->hw_type;
  core_init.core_params.hw_addr = params->hw_addr;
  core_init.core_params.hw_addr_len = params->hw_addr_len;
  core_init.core_params.domain_name_server_option =
    params->domain_name_server_option;
  core_init.core_params.subnet_mask_option = params->subnet_mask_option;
  core_init.core_params.router_option = params->router_option;
  core_init.core_params.interface_mtu_option = params->interface_mtu_option;
  core_init.core_params.domain_name_option = params->domain_name_option;
  core_init.core_params.sip_server_option = params->sip_server_option;
  core_init.core_params.rapid_commit_option = params->rapid_commit_option;
  core_init.core_params.reboot_tries = params->reboot_tries;
  core_init.core_params.selecting_tries = params->selecting_tries;
  core_init.core_params.requesting_tries = params->requesting_tries;
  core_init.core_params.init_tries = params->init_tries;
  core_init.core_params.inform_tries = params->inform_tries;
  core_init.core_params.decline_restart_time = params->decline_restart_time;
  core_init.core_params.init_restart_time = params->init_restart_time;
  core_init.core_params.verify_time = params->verify_time;
  core_init.core_params.reboot_base_time = params->reboot_base_time;
  core_init.core_params.select_base_time = params->select_base_time;
  core_init.core_params.request_base_time = params->request_base_time;
  core_init.core_params.inform_base_time = params->inform_base_time;
  core_init.core_params.server_addr = params->server_addr;
  /* Copy the client_id-related parameters */
  core_init.core_params.client_id_type = params->client_id_type;
  core_init.core_params.client_id_len = params->client_id_len;
  /* Copy the client_id pointer only if a custom client_id is set. */
  if (params->client_id_type == DHCP_CLIENT_ID_OPTTYPE_CUSTOM)
  {
    memscpy ( (uint8 *)core_init.core_params.client_id,
              DHCP_CLIENT_CLIENT_ID_MAX_LEN,
             (uint8 *)params->client_id,
             core_init.core_params.client_id_len );
  }

  timr_init.d_malloc = dhcpi_client_mem_alloc;
  timr_init.d_free = dhcpi_client_mem_free;

  configuration_init.d_malloc = dhcpi_client_mem_alloc;
  configuration_init.d_free = dhcpi_client_mem_free;
  configuration_init.iface = iface;

  /* Copy config params */
  configuration_init.config_params.htype = params->hw_type;
  configuration_init.config_params.domain_name_server_option =
    params->domain_name_server_option;
  configuration_init.config_params.subnet_mask_option =
    params->subnet_mask_option;
  configuration_init.config_params.router_option = params->router_option;
  configuration_init.config_params.interface_mtu_option =
    params->interface_mtu_option;
  configuration_init.config_params.domain_name_option =
    params->domain_name_option;
  configuration_init.config_params.sip_server_option =
    params->sip_server_option;
  configuration_init.status_cb = NULL;
  
  storage_init.d_malloc = dhcpi_client_mem_alloc;
  storage_init.d_free = dhcpi_client_mem_free;
  storage_init.iface = iface;

  client_init.d_malloc = dhcpi_client_mem_alloc;
  client_init.d_free = dhcpi_client_mem_free;
  client_init.iface = iface;

  /* Init the core and shims, free them in order */
  info->client = dhcp_clienti_client_new(&client_init);
  info->core = dhcp_clienti_core_new(&core_init);
  info->configuration = dhcp_clienti_configuration_new(&configuration_init);
  info->storage = dhcp_clienti_storage_new(&storage_init);
  info->timer = dhcp_clienti_timer_new(&timr_init);

  /* Check everything went ok */
  if ( NULL != info->core &&
       NULL != info->timer &&
       NULL != info->configuration &&
       NULL != info->storage &&
       NULL != info->client )
  {
    /* Set up the configs */
    core_config.client_request_new = dhcp_clienti_client_request_new;
    core_config.client_request_pushdown_tail =
      dhcp_clienti_client_request_pushdown_tail;
    core_config.client_request_free = dhcp_clienti_client_request_free;
    core_config.client_request_send = dhcp_clienti_client_request_send;
    core_config.client_request_handle = info->client;

    core_config.configuration_test = dhcp_clienti_configuration_test;
    core_config.configuration_set = dhcp_clienti_configuration_set;
    core_config.configuration_clear = dhcp_clienti_configuration_clear;
    core_config.configuration_event_ind = dhcp_clienti_configuration_event_ind;
    core_config.configuration_handle = info->configuration;

    core_config.storage_get_lease = dhcp_clienti_storage_get_lease;
    core_config.storage_set_lease = dhcp_clienti_storage_set_lease;
    core_config.storage_handle = info->storage;

    core_config.timer_start = dhcp_clienti_timer_set;
    core_config.timer_stop = dhcp_clienti_timer_clear;
    core_config.timer_handle = info->timer;

    timr_config.timer_expire = dhcp_clienti_core_timer_expire;
    timr_config.timer_expire_handle = info->core;

    /* Storage doesn't need config right now */

    configuration_config.decline = dhcp_clienti_core_decline;
    configuration_config.decline_handle = info->core;

    client_config.reply = dhcp_clienti_core_reply;
    client_config.reply_handle = info->core;

    info->cookie_val = DHCPI_CLIENT_HANDLE_COOKIE;
    info->cookie_ptr = info;

    if ( dhcp_clienti_client_start( info->client, &client_config ) &&
         dhcp_clienti_configuration_start( info->configuration,
                                           &configuration_config,
                                           info ) &&
         dhcp_clienti_storage_start(info->storage) &&
         dhcp_clienti_timer_start(info->timer, &timr_config) &&
         dhcp_clienti_core_start( info->core, &core_config ) )
    {
      ret = info;
    }
  }
  if ( NULL == ret )
  {
    LOG_MSG_ERROR_0("Failed to start DHCP, informing the caller");
    status_cb(userdata, DHCP_CLIENT_INIT_FAIL);
  }

  LOG_MSG_INFO1_1( "dhcpi_client_start: returning with %p", ret );
  return;
} /* dhcpi_client_start */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_STOP_CMD()

DESCRIPTION
  This function is used to serialize events into the PS task context,
  and will be called when the PS_DHCP_CLIENT_STOP_CMD is recieved.  This
  function will choose the approprate Server Shim, DHCP Address
  Manager and DHCP core.  It will then free these components, ungluing
  them.  This function will also NULL the handle pointer passed in.

DEPENDENCIES
  The user_data must be valid.
  The cmd must be a PS_DHCP_CLIENT_STOP_CMD.

PARAMETERS
  ps_cmd_enum_type - The type of command that this is.
  void * user_data - The data used to process this command.  In this
                     case, this consists of the dhcp_client_mgr_cmd
                     information.

RETURN VALUE
  None.

SIDE EFFECTS
  The stop data (user_data_ptr) will be freed.
===========================================================================*/
static void
dhcpi_client_stop_cmd
(
  ps_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  dhcp_client_mgr_cmd * stop = user_data_ptr;

  if ( NULL == user_data_ptr )
  {
    DATA_ERR_FATAL("user data NULL?");
    return;
  }
  if ( PS_DHCP_CLIENT_STOP_CMD != cmd )
  {
    LOG_MSG_ERROR_2("cmd not stop %d %d", cmd, PS_DHCP_CLIENT_STOP_CMD);
    ASSERT(0);
    return;
  }

  if ( TRUE == dhcpi_client_mgr_info_verify(stop->info, FALSE) )
  {
    dhcpi_client_stop( stop->info );
  }

  dhcpi_client_mem_free( stop );
} /* dhcpi_client_stop_cmd */

/*===========================================================================
FUNCTION DHCPI_CLIENT_START_CMD()

DESCRIPTION
  This function is used to serialize events into the PS task context,
  and will be called when the PS_DHCP_CLIENT_START_CMD is recieved.  This
  function will choose the approprate Server Shim, DHCP Address
  Manager and DHCP core for IPV4, ps_iface, ppp (single IP address to
  hand out).  It will then initialize these components gluing them
  together.

DEPENDENCIES
  The am interface must be up and configured.
  The user_data_ptr must point at a valid dhcp_client_mgr_start object.
  The cmd must be PS_DHCP_CLIENT_START_CMD.

PARAMETERS
  ps_cmd_enum_type - The type of command that this is.
  void * user_data - The data used to process this command.  In this
                     case, this consists of the dhcp_client_mgr_start
                     information.

RETURN VALUE
  None.

SIDE EFFECTS
  The dhcp_client_mgr_start object will be freed.
===========================================================================*/
static void
dhcpi_client_start_cmd
(
  ps_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  dhcp_client_mgr_start * start = user_data_ptr;

  if ( NULL == user_data_ptr )
  {
    DATA_ERR_FATAL("user data NULL?");
    return;
  }
  if ( PS_DHCP_CLIENT_START_CMD != cmd )
  {
    LOG_MSG_ERROR_2("cmd not start %d %d", cmd, PS_DHCP_CLIENT_START_CMD);
    ASSERT(0);
    return;
  }

  dhcpi_client_start( start->info,
                      start->iface,
                      &start->params,
                      start->status_cb,
                      start->userdata );

  dhcpi_client_mem_free( start );
} /* dhcpi_client_start_cmd */

/*===========================================================================
FUNCTION DHCPI_CLIENT_RELEASE_CMD()

DESCRIPTION
  This function is used to serialize events into the PS task context,
  and will be called when the PS_DHCP_CLIENT_RELEASE_CMD is recieved.
  This function will call the core release function to effect a
  release event.

DEPENDENCIES
  The cmd must be PS_DHCP_CLIENT_START_CMD.
  The handle must be a valid dhcp_client_mgr_info

PARAMETERS
  ps_cmd_enum_type - The type of command that this is.
  void * user_data - The data used to process this command.  In this
                     case, this consists of the dhcp_client_mgr_cmd
                     information.

RETURN VALUE
  None.

SIDE EFFECTS
  The dhcp_client_mgr_cmd object will be freed.
===========================================================================*/
static void
dhcpi_client_release_cmd
(
  ps_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  dhcp_client_mgr_cmd * release = user_data_ptr;

  if ( NULL == user_data_ptr )
  {
    DATA_ERR_FATAL("user data NULL?");
    return;
  }
  if ( PS_DHCP_CLIENT_RELEASE_CMD != cmd )
  {
    LOG_MSG_ERROR_2("cmd not start %d %d", cmd, PS_DHCP_CLIENT_RELEASE_CMD);
    ASSERT(0);
    return;
  }

  if ( TRUE == dhcpi_client_mgr_info_verify(release->info, FALSE) )
  {
    dhcp_clienti_core_release( release->info->core );
  }

  dhcpi_client_mem_free( release );
} /* dhcpi_client_release_cmd */

/*===========================================================================
FUNCTION DHCPI_CLIENT_INFORM_CMD()

DESCRIPTION
  This function is used to serialize events into the PS task context,
  and will be called when the PS_DHCP_CLIENT_INFORM_CMD is recieved.
  This function will call the core inform function to effect an
  inform event.

DEPENDENCIES
  The cmd must be PS_DHCP_CLIENT_START_CMD.
  The handle must be a valid dhcp_client_mgr_info

PARAMETERS
  ps_cmd_enum_type - The type of command that this is.
  void * user_data - The data used to process this command.  In this
                     case, this consists of the dhcp_client_mgr_cmd
                     information.

RETURN VALUE
  None.

SIDE EFFECTS
  The dhcp_client_mgr_inform object will be freed.
===========================================================================*/
static void
dhcpi_client_inform_cmd
(
  ps_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  dhcp_client_mgr_inform * inform = user_data_ptr;

  if ( NULL == user_data_ptr )
  {
    DATA_ERR_FATAL("user data NULL?");
    return;
  }
  if ( PS_DHCP_CLIENT_INFORM_CMD != cmd )
  {
    LOG_MSG_ERROR_2("cmd not start %d %d", cmd, PS_DHCP_CLIENT_INFORM_CMD);
    ASSERT(0);
    return;
  }

  if ( TRUE == dhcpi_client_mgr_info_verify(inform->info, FALSE) )
  {
    dhcp_clienti_core_inform( inform->info->core, inform->addr );
  }

  dhcpi_client_mem_free( inform );
} /* dhcpi_client_inform_cmd */

/*===========================================================================
FUNCTION DHCPI_CLIENT_RENEW_CMD()

DESCRIPTION
  This function is used to serialize events into the PS task context,
  and will be called when the PS_DHCP_CLIENT_RENEW_CMD is recieved.
  This function will call the core release function to effect a
  release event.

DEPENDENCIES
  The cmd must be PS_DHCP_CLIENT_START_CMD.
  The handle must be a valid dhcp_client_mgr_info

PARAMETERS
  ps_cmd_enum_type - The type of command that this is.
  void * user_data - The data used to process this command.  In this
                     case, this consists of the dhcp_client_mgr_cmd
                     information.

RETURN VALUE
  None.

SIDE EFFECTS
  The dhcp_client_mgr_cmd object will be freed.
===========================================================================*/
static void
dhcpi_client_renew_cmd
(
  ps_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  dhcp_client_mgr_cmd * renew = user_data_ptr;

  if ( NULL == user_data_ptr )
  {
    DATA_ERR_FATAL("user data NULL?");
    return;
  }
  if ( PS_DHCP_CLIENT_RENEW_CMD != cmd )
  {
    LOG_MSG_ERROR_2("cmd not start %d %d", cmd, PS_DHCP_CLIENT_RENEW_CMD);
    ASSERT(0);
    return;
  }

  if ( TRUE == dhcpi_client_mgr_info_verify(renew->info, FALSE) )
  {
    dhcp_clienti_core_renew( renew->info->core );
  }

  dhcpi_client_mem_free( renew );
} /* dhcpi_client_renew_cmd */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*==========================================================================
FUNCTION DHCP_CLIENT_PARAMS_INIT()

Description:
  This function will initialize a dhcp_client_params struct with
  suggested default values.

Arguments:
  params          - The handle to the params struct to fill with default
                    values.
  hw_addr         - The hw_addr identifying the DHCP client.
  hw_addr         - The length of the hw_addr passed in 'hw_addr'.

Return value:
  Returns -1 if failed, 0 if OK.

Dependencies:
  The 'params' handle should not be NULL and space already allocated
  for the struct.
===========================================================================*/
int
dhcp_client_params_init
(
  dhcp_client_params    * params,
  void                  * hw_addr,
  uint32                  hw_addr_len
)
{
  /* error check */
  if ( NULL == params )
  {
    LOG_MSG_ERROR_0("params NULL");
    return -1;
  }
  if (( 0 < hw_addr_len && NULL == hw_addr ) ||
      ( 0 == hw_addr_len && NULL != hw_addr ))
  {
    LOG_MSG_ERROR_0("hw_addr does not match hw_addr_len");
    return -1;
  }
  if ( DHCPI_CLIENT_HW_ADDR_LEN < hw_addr_len )
  {
    LOG_MSG_ERROR_2("hw_addr_len %d, expected < %d",
                    hw_addr_len, DHCPI_CLIENT_HW_ADDR_LEN);
    return -1;
  }

  /* NULL out params struct first */
  memset(params, 0, sizeof(dhcp_client_params));

  params->cookie = DHCPI_CLIENT_PARAMS_HANDLE_COOKIE;
  params->hw_addr_len = hw_addr_len;
  params->hw_addr = (uint8*) hw_addr; /* local deep copy made in _start */

  params->hw_type = DHCP_CLIENT_HWTYPE_ETHERNET;
  params->domain_name_server_option = TRUE;
  params->subnet_mask_option = TRUE;
  params->router_option = TRUE;
  params->interface_mtu_option = TRUE;
  params->domain_name_option = TRUE;
  params->sip_server_option = TRUE;
  params->client_id_type = DHCP_CLIENT_ID_OPTTYPE_NONE;
  params->rapid_commit_option = TRUE;

  /* The following time values are in seconds */
  params->reboot_tries = 2;
  params->selecting_tries = 4;
  params->requesting_tries = 4;
  params->init_tries = 3;
  params->inform_tries = 4;

  params->decline_restart_time = 10;
  params->init_restart_time = 10;
  params->verify_time = 2;
  params->reboot_base_time = 4;
  params->select_base_time = 4;
  params->request_base_time = 4;
  params->inform_base_time = 4;

  /* server address, default to broadcast addr */
  params->server_addr = DHCP_CLIENT_BROADCAST_ADDR;

  return 0;
} /* dhcp_client_params_init */

/*===========================================================================
FUNCTION DHCP_CLIENT_START()

Description:
  This function will configure the DHCP Client for the given
  interface, and returns a handle to the DHCP client object.

  This function will be serialized through the PS task.

Arguements:
  dss_iface_id_type - The interface descriptor of the interface to configure.
  dhcp_client_params params - The parameters that are to be used.
  void (*status_cb)(
    void * userdata,
    dhcp_client_status_enum status ) -
    This function will be called to indicate the obtaining or loss of an IP
    address.  See below for details on the enum.
  void * userdata - A cookie to be used when the status_cb() is called.


Return value:
  void * - A handle to the new DHCP client. NULL on error.

Dependencies:
  This must be called before stop, renew or release.
===========================================================================*/
void *
dhcp_client_start
(
  dss_iface_id_type iface,
  dhcp_client_params * params,
  dhcp_client_status_cb_type status_cb,
  void * userdata
)
{
  dhcp_client_mgr_start * start;
  dhcp_client_mgr_info * info;

  if ( NULL == status_cb )
  {
    DATA_ERR_FATAL("dhcp_start with NULL status_cb");
    return NULL;
  }

  if ( NULL == params )
  {
    DATA_ERR_FATAL("params NULL");
    return NULL;
  }

  if ( DHCPI_CLIENT_PARAMS_HANDLE_COOKIE != params->cookie )
  {
    LOG_MSG_ERROR_2("params cookie check failed %x %x",
                    params->cookie, DHCPI_CLIENT_PARAMS_HANDLE_COOKIE);
    ASSERT(0);
    return NULL;
  }
  
  if (( 0 < params->hw_addr_len && NULL == params->hw_addr ) ||
      ( 0 == params->hw_addr_len && NULL != params->hw_addr ))
  {
    DATA_ERR_FATAL("params->hw_addr NULL for non-zero hw_addr_len");
    return NULL;
  }
  
  start = (dhcp_client_mgr_start*)
    dhcpi_client_mem_alloc(sizeof(dhcp_client_mgr_start));
  if ( NULL == start )
  {
    DATA_ERR_FATAL("dhcp_start unable to get parameter memory");
    return NULL;
  }

  info = (dhcp_client_mgr_info*)
    dhcpi_client_mem_alloc(sizeof(dhcp_client_mgr_info));
  if ( NULL == info )
  {
    dhcpi_client_mem_free(start);
    DATA_ERR_FATAL( "dhcp_start unable to get handle memory" );
    return NULL;
  }
  info->cookie_val = DHCPI_CLIENT_HANDLE_UNINIT_COOKIE;
  info->core = NULL;
  info->client = NULL;
  info->configuration = NULL;
  info->storage = NULL;
  info->timer = NULL;
  start->info = info;
  start->iface = iface;
  start->params = *params;
  /* Copy the hw_addr into our space */
  if ( NULL != params->hw_addr )
  { 
    memscpy( start->hw_addr,
            DHCPI_CLIENT_HW_ADDR_LEN, params->hw_addr,
            MIN(params->hw_addr_len, 
            DHCPI_CLIENT_HW_ADDR_LEN) );
    start->params.hw_addr = start->hw_addr; /* Point at our copy of this */
  }
  /* Zero out the values picked up from the struct-copy. 
   * Copy the variables again only if they pass the sanity checks. */
  start->params.client_id_type = DHCP_CLIENT_ID_OPTTYPE_NONE;
  start->params.client_id_len = 0;
  start->params.client_id = NULL;
  switch (params->client_id_type)
  {
    case DHCP_CLIENT_ID_OPTTYPE_HWADDR:
      /* Ignore the client identifier, if
       * 1) the hardware type is invalid,
       * 2) the hardware address in unset, or
       * 3) the hardware address is larger than the maximum allowed
       *    client identifier size */
      if ( (params->hw_type < DHCP_CLIENT_HWTYPE_ETHERNET) ||
           (params->hw_type > DHCP_CLIENT_HWTYPE_CAI) ||
           (params->hw_addr == NULL) ||
           (params->hw_addr_len < DHCP_CLIENT_CLIENT_ID_MIN_LEN) ||
           (params->hw_addr_len > DHCP_CLIENT_CLIENT_ID_MAX_LEN) )
      {
        LOG_MSG_ERROR_3( "Invalid config - hw_type:%d hw_addr:0x%x hw_addr_len:%d",
                         (int32)(params->hw_type),
                         (int32)(params->hw_addr),
                         (int32)(params->hw_addr_len) );
      }
      /* Set the appropriate client_id_type otherwise */
      else
      {
        start->params.client_id_type = DHCP_CLIENT_ID_OPTTYPE_HWADDR;
      }
      break;

    case DHCP_CLIENT_ID_OPTTYPE_CUSTOM:
      /* Treat the custom client identifier as an opaque value.
       * Only if it is of an unacceptable size, ignore it. */
      if ( (params->client_id_len < DHCP_CLIENT_CLIENT_ID_MIN_LEN) ||
           (params->client_id_len > DHCP_CLIENT_CLIENT_ID_MAX_LEN) ||
           (params->client_id == NULL) )
      {
        LOG_MSG_ERROR_2( "Invalid custom client identifier with len=%d ptr=0x%x",
                         params->client_id_len, params->client_id );
      }
      /* Otherwise, do a blind copy. */
      else
      {
        start->params.client_id_type = DHCP_CLIENT_ID_OPTTYPE_CUSTOM;
        start->params.client_id_len = params->client_id_len;
        start->params.client_id = params->client_id;
      }
      break;

    default:
      break;
  }
  start->status_cb = status_cb;
  start->userdata = userdata;

  (void) ps_set_cmd_handler( PS_DHCP_CLIENT_START_CMD,
                             dhcpi_client_start_cmd );
  ps_send_cmd( PS_DHCP_CLIENT_START_CMD, start );

  /* We return info here because if the PS task runs first, then start
   * will be freed already
   */
  return info;
} /* dhcp_client_start */

/*===========================================================================
FUNCTION DHCP_CLIENT_RENEW()

Description:
  This function starts the DHCP client.  In particular, this function
  will configure an instance of the DHCP client for provisioning the
  PS network stacks for operation.

  The actual provisioning will occur at some point in the future once
  the DHCP client has found and negotiated with a server.

  This function will be serialized through the PS task.

Arguements:
  void * dhcp_client_handle - The handle from dhcp_client_start() of the
    client to release.

Return value:
  none.

Dependencies:
  The dhcp_client_init() function must have been called previously.

  The interface passed in must in such a state that it is able to
  recieve UDP packets and pass them up to the application (DHCP
  client) before an IP address is provisioned based apon the hardware
  address.
===========================================================================*/
void
dhcp_client_renew
(
  void * dhcp_client_handle
)
{
  dhcp_client_mgr_cmd * renew;

  if ( NULL == dhcp_client_handle )
  {
    DATA_ERR_FATAL("dhcp_client_renew called with NULL handle");
    return;
  }

  if ( FALSE == dhcpi_client_mgr_info_verify(dhcp_client_handle, TRUE) )
  {
    LOG_MSG_ERROR_1("dhcp_client_renew called with invalid handle %p",
                    dhcp_client_handle);
    return;
  }

  renew = (dhcp_client_mgr_cmd*)
    dhcpi_client_mem_alloc(sizeof(dhcp_client_mgr_cmd));
  if ( NULL == renew )
  {
    DATA_ERR_FATAL("dhcp_client_renew unable to get parameter memory");
    return;
  }

  renew->info = (dhcp_client_mgr_info*)dhcp_client_handle;

  (void) ps_set_cmd_handler( PS_DHCP_CLIENT_RENEW_CMD,
                             dhcpi_client_renew_cmd );
  ps_send_cmd( PS_DHCP_CLIENT_RENEW_CMD, renew );

  return;
} /* dhcp_client_renew */

/*===========================================================================
FUNCTION DHCP_CLIENT_RELEASE()

Description:
  This function will cause the DCHP client to release a lease.

  This function may not be needed and may or may not be
  implemented.

  This function will be serialized through the PS task.

Arguements:
  void * dhcp_client_handle - The handle from dhcp_client_start() of the
    client to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated from a dhcp_client_start()
    function.
===========================================================================*/
void
dhcp_client_release
(
  void * dhcp_client_handle
)
{
  dhcp_client_mgr_cmd * release;

  if ( NULL == dhcp_client_handle )
  {
    DATA_ERR_FATAL("dhcp_client_release called with NULL handle");
    return;
  }

  if ( FALSE == dhcpi_client_mgr_info_verify(dhcp_client_handle, TRUE) )
  {
    LOG_MSG_ERROR_1("dhcp_client_release called with invalid handle %p",
                    dhcp_client_handle);
    return;
  }

  release = (dhcp_client_mgr_cmd*)
    dhcpi_client_mem_alloc(sizeof(dhcp_client_mgr_cmd));
  if ( NULL == release )
  {
    DATA_ERR_FATAL("dhcp_client_release unable to get parameter memory");
    return;
  }

  release->info = (dhcp_client_mgr_info*)dhcp_client_handle;

  (void) ps_set_cmd_handler( PS_DHCP_CLIENT_RELEASE_CMD,
                             dhcpi_client_release_cmd );
  ps_send_cmd( PS_DHCP_CLIENT_RELEASE_CMD, release );

  return;
} /* dhcp_client_release */

/*===========================================================================
FUNCTION DHCP_CLIENT_INFORM()

Description:
  This function will cause the DCHP client to try to get provisions
  via an INFORM message without getting a least.

  This function will be serialized through the PS task.

Arguements:
  void * dhcp_client_handle - The handle from dhcp_client_start() of the
    client to release.
  uint32 addr - The IP address we have been provisioned with, in
    network byte order.

Return value:
  None.

Dependencies:
  The handle must have been allocated from a dhcp_client_start()
    function.
===========================================================================*/
void
dhcp_client_inform
(
  void * dhcp_client_handle,
  uint32 addr
)
{
  dhcp_client_mgr_inform * inform;

  if ( NULL == dhcp_client_handle )
  {
    DATA_ERR_FATAL("dhcp_client_inform called with NULL handle");
    return;
  }

  if ( FALSE == dhcpi_client_mgr_info_verify(dhcp_client_handle, TRUE) )
  {
    LOG_MSG_ERROR_1("dhcp_client_inform called with invalid handle %p",
                    dhcp_client_handle);
    return;
  }

  inform = (dhcp_client_mgr_inform*)
    dhcpi_client_mem_alloc(sizeof(dhcp_client_mgr_inform));
  if ( NULL == inform )
  {
    DATA_ERR_FATAL("dhcp_client_inform unable to get parameter memory");
    return;
  }

  inform->info = (dhcp_client_mgr_info*)dhcp_client_handle;
  inform->addr = addr;

  (void) ps_set_cmd_handler( PS_DHCP_CLIENT_INFORM_CMD,
                             dhcpi_client_inform_cmd );
  ps_send_cmd( PS_DHCP_CLIENT_INFORM_CMD, inform );

  return;
} /* dhcp_client_inform */

/*===========================================================================
FUNCTION DHCP_CLIENT_STOP()

Description:
  This function will stop a DHCP client.  This will cause the dhcp to
  close the DHCP client handle and free the associated memory.

  This function will be serialized through the PS task.

Arguements:
  dhcp_client_handle - The handle from dhcp_client_start() of the
    client to close.

Return value:
  None.

Dependencies:
  The handle must have been allocated from a dhcp_client_start()
    function.
===========================================================================*/
void
dhcp_client_stop
(
  void ** dhcp_client_handle
)
{
  dhcp_client_mgr_cmd * stop;

  ASSERT( NULL != dhcp_client_handle );

  if ( NULL == *dhcp_client_handle )
  {
    LOG_MSG_ERROR_0("dhcp_stop called with NULL handle");
    return;
  }

  if ( FALSE == dhcpi_client_mgr_info_verify(*dhcp_client_handle, TRUE) )
  {
    LOG_MSG_ERROR_1("dhcp_stop called with invalid handle %p",
                    *dhcp_client_handle);
    return;
  }

  stop = (dhcp_client_mgr_cmd*)
    dhcpi_client_mem_alloc(sizeof(dhcp_client_mgr_cmd));
  if ( NULL == stop )
  {
    DATA_ERR_FATAL("dhcp_stop unable to get parameter memory");
    return;
  }

  stop->info = (dhcp_client_mgr_info*)*dhcp_client_handle;

  (void) ps_set_cmd_handler( PS_DHCP_CLIENT_STOP_CMD,
                             dhcpi_client_stop_cmd );
  ps_send_cmd( PS_DHCP_CLIENT_STOP_CMD, stop );

  *dhcp_client_handle = NULL;
  return;
} /* dhcp_client_stop */

/*===========================================================================
FUNCTION DHCP_CLIENT_IS_DHCP_IN_PROGRESS

Description:
  This function returns whether DHCP client is currently running.

Arguements:
  void * dhcp_client_handle - handle to DHCP Client.

Return value:
  TRUE if client is running, FALSE otherwise.

Dependencies:
  The handle must have been allocated by dhcp_client_start() function.
===========================================================================*/
boolean
dhcp_client_is_dhcp_in_progress
(
  void * dhcp_client_handle
)
{
  dhcp_client_mgr_info * info;

  if ( NULL == dhcp_client_handle )
  {
    DATA_ERR_FATAL("dhcp_client_is_dhcp_in_progress called with NULL handle");
    return FALSE;
  }

  if ( FALSE == dhcpi_client_mgr_info_verify(dhcp_client_handle, TRUE) )
  {
    LOG_MSG_ERROR_1("dhcp_client_is_dhcp_in_progress called with invalid handle %p",
                    dhcp_client_handle);
    return FALSE;
  }

  info = (dhcp_client_mgr_info *)dhcp_client_handle;

  if (info->cookie_val == DHCPI_CLIENT_HANDLE_UNINIT_COOKIE)
  {
    /* If we hit this, a start cmd is pending in PS. Return TRUE.
     */
    return TRUE;
  }
  else
  {
    return dhcp_clienti_core_is_dhcp_core_in_progress( info->core );
  }

} /* dhcp_client_is_dhcp_in_progress */

