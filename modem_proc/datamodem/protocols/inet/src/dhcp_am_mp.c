/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         D H C P _ A M _ M P . C

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol Address Manager
  multiplexer.  This file multiplexs multiple connections to the same
  address manager.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Initializations for single IP address, PS address manager:
  - dhcp_am_mp_ps_single_new/dhcp_server_am_mp_new must be called 
     first to initialize a handle with the internal configuration.
  - dhcp_am_mp_start must be called second to configure the handle
    with external interdependancies.

  Cleanup for any mp address manager:
  - dhcp_am_mp_stop must be called to free the handle when the session
    is finished.  This will clean up the lower layer AM.

  All external functions of this file must be called from the same
  context.

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


 $Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcp_am_mp.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/10/14    vl     Optimization of F3 messages
01/03/14    mp     Added the support for SIP server option.
11/01/13    rk     Removed feature mobileap.
09/21/12    ss     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
10/12/11    am     Added get_ip_cfg to fetch IP from stored lease.
08/29/11    ag     Added the support for NBNS option.
01/10/11    cp     Added support for the MTU option in SoftAP mode.
05/03/10    cp     DHCP Soft AP changes.
03/31/10    cp     Fixed a bug which came up in the previous check in.
02/16/10    cp     Changes to propagate the UM iface mtu value to TE.
10/23/08    am     Dont ASSERT if dhcp_am_ps_single_new() returns NULL.
09/26/08    am     Changed malloc/free names for off target support.
01/19/07    tp/jd  Fixed dequeuing of am_mp from list on free
11/05/05    rt     Fixed coverity errors.
05/11/05    clp    Fixed lint errors.
04/12/04    clp    Initial development work done.  Added comments.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <string.h>
#include "ps_in.h"
#include "ps_iface.h"
#include "dhcpi.h"
#include "dhcpi_am_ps_iface.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Data structures have the following architecture:
 *   For each AM iface, we chain the clients together for the callouts
 *     from the AM config (which are per server).
 *   Each AM iface is also chained together to be able to detect when
 *     one is being used again.
 */

/* Forward declaration */
struct dhcp_am_mp_info;

/* This data structure is entirely internal to this file (although a
 * pointer goes into the dhcp_am_mp_info below, but that is always opaque
 * outside this file
 */
typedef struct dhcp_am_backend_info
{
  /* list pointers */
  struct dhcp_am_backend_info * next;
  struct dhcp_am_backend_info ** prev; /* ptr to previous ptr to us */
  struct dhcp_am_mp_info * head; /* list of clients using the iface */
  /* Reference count */
  uint32 ref;
  /* internal config */
  dhcp_am_init   init;
  /* handle for next layer down */
  void         * am_handle;
  /* Pass thru functions */
  boolean      (*start)(void *, dhcp_am_config *);
  void         (*stop)(void **);
  boolean      (*get_lease)(void *, dhcp_core_msg_info *);
  boolean      (*free_lease)(void *, dhcp_core_msg_info *);
  boolean      (*err_lease)(void *, dhcp_core_msg_info *);
  boolean      (*dns_first)(void *, void **, uint32 *);
  boolean      (*dns_next)(void *, void **, uint32 *);
  boolean      (*nbns_first)(void *, void **, uint32 *);
  boolean      (*nbns_next)(void *, void **, uint32 *);
  uint32       (*addr_info_server_id)(void *);
  uint32       (*addr_info_netmask)(void *);
  uint32       (*interface_mtu) (void *);
  void         (*get_ip_config) (void *, uint32 *, uint32*, uint32*);
  void         (*sip_addr_list)(void *, ip_addr_type *, uint32 *);
  void         (*sip_domain_name_list)(void *, ps_iface_domain_name_type *,
                                       uint32 *);

  /* cookies: The cookie_ptr takes a value of a pointer to the
   * beginning of this structure. This happens when the core is
   * started.
   */
  struct dhcp_am_backend_info   * cookie_ptr;
#define DHCP_AM_MP_BACKEND_COOKIE ((uint32)(0xBEC00C1E))
  uint32         cookie;
} dhcp_am_backend_info;

typedef struct dhcp_am_mp_info
{
  /* list pointers for callouts from config */
  struct dhcp_am_mp_info  * next, ** prev;
  /* The backend for this interface */
  dhcp_am_backend_info * backend;
  /* The configuration for this interface */
  dhcp_am_config         config;
  /* cookies: The cookie_ptr takes a value of a pointer to the
   * beginning of this structure. This happens when the core is
   * started.
   */
  struct dhcp_am_mp_info   * cookie_ptr;
  /* The cookie takes the value below when allocated in _new */
#define DHCP_AM_COOKIE ((uint32)(0xA4C00C1E))
  uint32                  cookie;
} dhcp_am_mp_info;

/* Linked list of backends we have allocated. */
static dhcp_am_backend_info * dhcp_am_mp_head = NULL;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DHCPI_AM_MP_ADDR_FREE()

DESCRIPTION
  This function passes a call from the AM to each of the registered AM
  handles that exist for the backend passed in.

DEPENDENCIES
  The handle must be the backend pointer for the AM backend.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  ip_addr_type - ip address to associate with the hardware address.
  char * hwaddr - hardware address.
  uint32 len - The length of the hardware address.

RETURN VALUE
  None.

SIDE EFFECTS
  Passes the info on to each of the registered AM's.
===========================================================================*/
static void
dhcpi_am_mp_addr_free
(
  void * handle,
  uint32 ip_addr,
  uint8 * hwaddr,
  uint32 len
)
{
  dhcp_am_backend_info * backend = handle;
  dhcp_am_mp_info * am;

  ASSERT(NULL != backend);
  ASSERT(DHCP_AM_MP_BACKEND_COOKIE == backend->cookie);
  ASSERT(backend == backend->cookie_ptr);
  am = backend->head;
  while ( NULL != am )
  {
    if ( NULL != am->config.addr_free )
    {
      am->config.addr_free( am->config.addr_free_handle,
                            ip_addr,
                            hwaddr,
                            len );
    }
    am = am->next;
  }

} /* dhcpi_am_mp_addr_free */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DHCP_AM_MP_PS_SINGLE_NEW()

DESCRIPTION
  This function initializes a dhcp_am handle with multiplexing support
  for the PS single IP address Address Manager.  This works by keeping
  track of all existing PS single address manager handles that have
  been allocated.  If we are reusing one, then pass that handle back.
  If not, then set up a new MP address manager that points at the
  PS single address manager.

  The underlying address manager calculates the default route ip
  address and netmask that should be provided to clients.

DEPENDENCIES
  The handle must be "started" after the other subsystems have been
  created.  The handle should be freed when it is no longer needed.

PARAMETERS
  dhcp_am_init * - Internal configuration information.  This includes
                   the interface to connect to.

RETURN VALUE
  handle for calling am functions. NULL on failure.

SIDE EFFECTS
  None
===========================================================================*/
void *
dhcp_am_mp_ps_single_new
(
  dhcp_am_init * init
)
{
  dhcp_am_mp_info * am;
  dhcp_am_backend_info * backend = dhcp_am_mp_head;

  LOG_MSG_INFO3_0( "INFO: --- DHCP AM MP NEW ---" );
  /* sanity check info */
  ASSERT(NULL != init);
  ASSERT(NULL != init->d_malloc);
  ASSERT(NULL != init->d_free);

  am = init->d_malloc( sizeof( dhcp_am_mp_info ) ) ;
  if ( NULL == am )
  {
    DATA_ERR_FATAL("FAIL: Insufficent memory to init DHCP am");
    return NULL;
  }
  memset( am, 0, sizeof(dhcp_am_mp_info) );
  while ( NULL != backend )
  {
    backend = backend->next;
  }
  if ( NULL == backend )
  {
    backend = init->d_malloc(sizeof(dhcp_am_backend_info));
    if ( NULL == backend )
    {
      init->d_free(am);
      DATA_ERR_FATAL("FAIL: Insufficent memory to init DHCP am");
      return NULL;
    }
    memset( backend, 0, sizeof(dhcp_am_backend_info) );
    backend->init = *init;
    backend->start = dhcp_am_ps_single_start;
    backend->stop = dhcp_am_ps_single_stop;
    backend->get_lease = dhcp_am_ps_single_get_lease;
    backend->free_lease = dhcp_am_ps_single_free_lease;
    backend->err_lease = dhcp_am_ps_single_err_lease;
    backend->dns_first = dhcp_am_ps_single_dns_first;
    backend->dns_next = dhcp_am_ps_single_dns_next;
    backend->nbns_first = dhcp_am_ps_single_nbns_first;
    backend->nbns_next = dhcp_am_ps_single_nbns_next;
    backend->addr_info_server_id = dhcp_am_ps_single_addr_info_server_id;
    backend->addr_info_netmask = dhcp_am_ps_single_addr_info_netmask;
    backend->interface_mtu = dhcp_am_ps_single_mtu;
    backend->get_ip_config = dhcp_am_ps_iface_get_ip_config;
    backend->sip_addr_list = dhcp_am_ps_iface_sip_addr_list;
    backend->sip_domain_name_list = dhcp_am_ps_iface_sip_domain_name_list;
    if ( NULL != (backend->am_handle = dhcp_am_ps_single_new(init) ) )
    {
      backend->ref = 1;
      backend->next = dhcp_am_mp_head;
      if ( NULL != backend->next )
      {
        backend->next->prev = &(backend->next);
      }
      backend->prev = &dhcp_am_mp_head;
      backend->cookie = DHCP_AM_MP_BACKEND_COOKIE;
      dhcp_am_mp_head = backend;
    }
    else
    {
      init->d_free(am);
      init->d_free(backend);
      backend = NULL;
      LOG_MSG_ERROR_0("FAIL: dhcp_am_ps_single_new returned NULL");
      return NULL;
    }
  }
  else
  {
    backend->ref++;
  }
  /* Ok, we know the backend, and we have the AM we will return, now
   * link all the info together.
   */
  am->next = backend->head;
  if ( NULL != am->next )
  {
    am->next->prev = &(am->next);
  }
  am->prev = &(backend->head);
  backend->head = am;
  am->backend = backend;
  am->cookie = DHCP_AM_COOKIE;

  return am;
} /* dhcp_am_mp_ps_single_new */

/*===========================================================================
FUNCTION DHCP_AM_MP_START()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function registers the functions that interface the DHCP Address
  Manager with other DHCP systems.

DEPENDENCIES
  The handle must have been previously allocated using
  dhcp_am_mp_ps_single_new().
  Calls to this function must be serialized with the new function
  through ps task to protect against ref getting out of sync.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  dhcp_am_config * - pointer to functions used to connect to other
                           systems.

RETURN VALUE
  TRUE - if successful.
  FALSE - if there are problems.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_start
(
  void * handle,
  dhcp_am_config * config
)
{
  dhcp_am_mp_info * am = handle;
  boolean ret = TRUE;
  dhcp_am_config mp_config;

  LOG_MSG_INFO3_0( "INFO: --- DHCP AM MP START ---" );
  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(NULL != am->backend);

  am->config = *config;
  mp_config.addr_free = dhcpi_am_mp_addr_free;
  mp_config.addr_free_handle = am->backend;

  if ( 1 == am->backend->ref )
  {
    if ( FALSE !=
         ( ret = am->backend->start( am->backend->am_handle, &mp_config) ) )
    {
      am->backend->cookie_ptr = am->backend;
    }
    else
    {
      LOG_MSG_INFO2_0("We are setting the am cookie pointer even though am start failed");
    }
  }
  am->cookie_ptr = am;
  ASSERT( am->backend->cookie_ptr == am->backend );
  return ret;
} /* dhcp_am_mp_start */

/*===========================================================================
FUNCTION DHCP_AM_MP_STOP()

DESCRIPTION
  This function unregisters the functions that interface the DHCP Address
  Manager.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
dhcp_am_mp_stop
(
  void ** handle
)
{
  dhcp_am_mp_info * am;
  dhcp_am_backend_info * backend;

  ASSERT(NULL != handle);
  am = *handle;
  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  if(am != am->cookie_ptr)
  {
     LOG_MSG_INFO1_0( "am_mp_stop() called before am_mp_start()" );
  }

  /* First, unlink the current am from the list of am's associated
   * with backend.
   */
  ASSERT( NULL != am->prev );
  *(am->prev) = am->next;
  if(am->next)
  {
    am->next->prev = am->prev;
  }

  /* Save out the backend and free am */
  backend = am->backend;
  am->cookie_ptr = NULL;
  am->cookie = 0;
  backend->init.d_free(am);
  /* Now check if this is the last am on the backend.  If it is,
   * unlink and free the backend.
   */
  if ( 0 == --backend->ref )
  {
    ASSERT( NULL != backend->prev );
    *(backend->prev) = backend->next;
    if (NULL != backend->next)
    {
      backend->next->prev = backend->prev;
    }
    backend->stop(&backend->am_handle);
    backend->init.d_free(backend);
  }
  LOG_MSG_INFO3_0( "INFO: --- DHCP AM STOP ---" );
  *handle = NULL;
} /* dhcp_am_mp_stop */

/*===========================================================================
FUNCTION DHCP_AM_MP_GET_LEASE()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fills out the following IP lease information in the
  dhcp_core_msg_info structure, if a lease can be filled out:
    - Assigned IP.
    - Server IP.
    - Router IP.
    - Subnet Mask.
    - Lease time.
    - Renew time (T1).
    - Rebind time (T2).
  This function is called in the event of a DISCOVER, REQUEST or INFORM
  message.
  If a NAK is desired, set reply_type to DHCP_DHCPNAK and return FALSE.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  dhcp_core_msg_info * - This provides information about the client for
                         calculating the values to return.  Also used to return
                         the results.

RETURN VALUE
  uint32 - non-zero if lease filled out ok.  zero if there is a problem.
  dhcp_core_msg_info * - Results filled out in this structure.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_get_lease
(
  void * handle,
  dhcp_core_msg_info * core
)
{
  dhcp_am_mp_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != am->backend);
  return am->backend->get_lease(am->backend->am_handle, core);
} /* dhcp_am_mp_get_lease */

/*===========================================================================
FUNCTION DHCP_AM_MP_FREE_LEASE()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function frees a previously allocated lease.  This function can be
  called either on a release from a client, or is a previously allocated lease
  is not actually sent out due to other errors.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  dhcp_core_msg_info * - This provides information about the client for
                         calculating the lease to free.

RETURN VALUE
  boolean - TRUE if SUCCESS.
          - FALSE if FAILS.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_free_lease
(
  void * handle,
  dhcp_core_msg_info * core
)
{
  dhcp_am_mp_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != core);
  ASSERT(NULL != am->backend);
  return am->backend->free_lease(am->backend->am_handle, core);
} /* dhcp_am_mp_free_lease */

/*===========================================================================
FUNCTION DHCP_AM_MP_ERR_LEASE()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function informs the DHCP Address Manager that a lease that was
  previously given out had a problem.  This function is called on a
  DHCPDECLINE message from the client.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  dhcp_core_msg_info * - This provides information about the client for
                         calculating the lease.

RETURN VALUE
  boolean - TRUE if SUCCESS.
          - FALSE if FAILS.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_err_lease
(
  void * handle,
  dhcp_core_msg_info * core
)
{
  dhcp_am_mp_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != core);
  ASSERT(NULL != am->backend);
  return am->backend->err_lease(am->backend->am_handle, core);
} /* dhcp_am_mp_err_lease */

/*===========================================================================
FUNCTION DHCP_AM_MP_DNS_FIRST()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the DNS server information (if available) and fills
  out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  void ** - This is a cookie that will be returned to get to the next
            DNS address.
  ip_add_type * - The first dns address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  void ** - cookie to get to next address.


SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_dns_first
(
  void * handle,
  void **cookie,
  uint32 * ip_addr
)
{
  dhcp_am_mp_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != am->backend);
  ASSERT(NULL != cookie);
  ASSERT(NULL != ip_addr);
  return am->backend->dns_first(am->backend->am_handle, cookie, ip_addr);
} /* dhcp_am_mp_dns_first */

/*===========================================================================
FUNCTION DHCP_AM_MP_DNS_NEXT()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the DNS server information (if available) and fills
  out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  void ** - This is a cookie that will be returned to get to the next
            DNS address.
  ip_add_type * - The next dns address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  void ** - cookie to get to next address.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_dns_next
(
  void * handle,
  void ** cookie,
  uint32 * ip_addr
)
{
  dhcp_am_mp_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != am->backend);
  ASSERT(NULL != cookie);
  ASSERT(NULL != ip_addr);
  return am->backend->dns_next(am->backend->am_handle, cookie, ip_addr);
} /* dhcp_am_mp_dns_next */


/*===========================================================================
FUNCTION DHCP_AM_MP_NBNS_FIRST()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the NBNS server information (if available) and fills
  out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  void ** - This is a cookie that will be returned to get to the next
            NBNS address.
  ip_add_type * - The first nbns address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  void ** - cookie to get to next address.


SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_nbns_first
(
  void * handle,
  void **cookie,
  uint32 * ip_addr
)
{
  dhcp_am_mp_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != am->backend);
  ASSERT(NULL != cookie);
  ASSERT(NULL != ip_addr);
  return am->backend->nbns_first(am->backend->am_handle, cookie, ip_addr);
} /* dhcp_am_mp_netbios_first */


/*===========================================================================
FUNCTION DHCP_AM_MP_NBNS_NEXT()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the NBNS server information (if available) and fills
  out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  void ** - This is a cookie that will be returned to get to the next
            NBNS address.
  ip_add_type * - The first nbns address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  void ** - cookie to get to next address.


SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_mp_nbns_next
(
  void * handle,
  void **cookie,
  uint32 * ip_addr
)
{
  dhcp_am_mp_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != am->backend);
  ASSERT(NULL != cookie);
  ASSERT(NULL != ip_addr);
  return am->backend->nbns_next(am->backend->am_handle, cookie, ip_addr);
} /* dhcp_am_mp_nbns_first */


/*===========================================================================
FUNCTION DHCP_AM_MP_ADDR_INFO_SERVER_ID()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the IP address of the server (us).

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.

RETURN VALUE
  uint32 - The IP address of the server.

SIDE EFFECTS
  None
===========================================================================*/
uint32
dhcp_am_mp_addr_info_server_id
(
  void * handle
)
{
  dhcp_am_mp_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != am->backend);

  return am->backend->addr_info_server_id(am->backend->am_handle);
} /* dhcp_am_mp_addr_info_server_id */

/*===========================================================================
FUNCTION DHCP_AM_MP_ADDR_INFO_NETMASK()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the netmask of the server (us).

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.

RETURN VALUE
  uint32 - The netmask of the subnet we are on.

SIDE EFFECTS
  None
===========================================================================*/
uint32
dhcp_am_mp_addr_info_netmask
(
  void * handle
)
{
  dhcp_am_mp_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != am->backend);

  return am->backend->addr_info_netmask(am->backend->am_handle);
} /* dhcp_am_mp_addr_info_netmask */

/*===========================================================================
FUNCTION DHCP_AM_MP_MTU()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the MTU of the server (us).

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
           details.

RETURN VALUE
  uint32 - The MTU of the server.

SIDE EFFECTS
  None
===========================================================================*/
uint32
dhcp_am_mp_mtu
(
  void * handle
)
{
  dhcp_am_mp_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != am->backend);

  return am->backend->interface_mtu(am->backend->am_handle);
} /* dhcp_am_mp_mtu */

/*===========================================================================
FUNCTION DHCP_AM_MP_GET_IP_CONFIG()

DESCRIPTION
  This function returns the IP config for the address
    manager associated with the DHCP handle.

  This is invoked only in single client DHCP case.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
           details.
  3*uint32* - IP params

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dhcp_am_mp_get_ip_config
(
  void   * handle,
  uint32 * client_ip,
  uint32 * server_id,
  uint32 * netmask
)
{
  dhcp_am_mp_info * am = handle;
  
  ASSERT(NULL != am);
  
  am->backend->get_ip_config(
    am->backend->am_handle,
	client_ip, server_id, netmask );
} /* dhcp_am_mp_get_ip_config */

/*===========================================================================
FUNCTION DHCP_AM_MP_SIP_ADDR_LIST()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the SIP server information (if available) for this AM.
DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  ip_addr_type * - pointer to the SIP server addresses.
  uint32 * : as input value it contains the count of the SIP server
          addresses asked for and as a return value it contains the
          actual number of addresses returned.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
dhcp_am_mp_sip_addr_list
(
  void * handle,
  ip_addr_type * sip_addr_list,
  uint32 * count
)
{
  dhcp_am_mp_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != am->backend);
  ASSERT(NULL != sip_addr_list);
  ASSERT(NULL != count);

  am->backend->sip_addr_list(am->backend->am_handle, sip_addr_list, count);

}

/*===========================================================================
FUNCTION DHCP_AM_MP_SIP_DOMAIN_NAME_LIST()

DESCRIPTION
  This function passes through to the underlying address manager.
  This function fetches the SIP server domain name information for this AM.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  dns_domain_name_list: pointer to an array of SIP domain names.
  uint32 * : as input value it contains the count of the SIP domain
          names asked for and as a return value it contains the
          actual number of domains names returned.


RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
dhcp_am_mp_sip_domain_name_list
(
  void * handle,
  ps_iface_domain_name_type  *sip_domain_name_list,
  uint32 * count
)
{
  dhcp_am_mp_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != am->backend);
  ASSERT(NULL != sip_domain_name_list);
  ASSERT(NULL != count);

  am->backend->sip_domain_name_list(am->backend->am_handle,
                                         sip_domain_name_list, count);

}
