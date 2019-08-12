/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         D H C P _ A M _ P S _ I F A C E . C

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol PS Interface Address
  Manager.  This file implements the single address address
  manager(AM).  This AM uses the IP address assigned to the phone on
  the specificed iface, and passes that on to the client.  This module
  also uses the same address to caluculate the advertised router
  address and netmask.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Initializations for single IP address, PS address manager:
  - dhcp_am_ps_single_new must be called first to initialize a handle
    with the internal configuration.
  - dhcp_am_ps_single_start must be called second to configure the
    handle with external interdependancies.

  Cleanup for any mp address manager:
  - dhcp_am_ps_single_stop must be called to free the handle when the
    session is finished.  This will clean up the lower layer AM.

  All external functions of this file must be called from the same
  context.

Copyright (c) 2004-2014 QUALCOMM Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcp_am_ps_iface.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/10/14    vl     Optimization of F3 messages
01/03/14    mp     Added the support for SIP server option.
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
11/12/12    ash    Fixed netmask and server IP calculation logic.
09/21/12    ss     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
10/12/11    am     Added get_ip_cfg to fetch IP from stored lease.
09/29/11    am     Added DHCP querying Um iface for IP config first through
                   DHCP_ADDR_CFG IOCTL before getting it from iface's net info.
08/29/11    ag     Added the support for NBNS option.
05/03/10    cp     DHCP Soft AP changes.
02/16/10    cp     Changes to propagate the UM iface mtu value to TE.
09/09/09    ss     Removed ASSERT in case when zero IP addr for iface received.
10/23/08    am     Removed ASSERTs in case getting IP addr for invalid/down
                   iface fails.
09/26/08    am     Changed malloc/free names for off target support.
08/14/05    mct    Updated for ps_iface_ioctl restructuring.
05/11/05    clp    Fixed lint issues.
04/13/04    clp    Initial development work done.  Added comments.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ps_iface.h"
#include "ps_in.h" /* For hton* macros */
#include "dhcpi.h"
#include "dhcpi_am_ps_iface.h"
#include "dhcp.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "time_svc.h"
#include "ds_Utils_DebugMsg.h"

#include "ps_iface_ioctl.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* All over the air info stored in network byte order */
/* Lease times in seconds */
#define DHCP_AM_DEFAULT_LEASE_TIME (dss_htonl((120*60)))
#define DHCP_AM_LOCAL_LEASE_TIME(x) \
  ((uint32) (1.05 * dss_ntohl(DHCP_AM_LEASE_TIME((x))))) /* allow up to a 5% drift */
#define DHCP_AM_LEASE_TIME(x) ((0==(x))?DHCP_AM_DEFAULT_LEASE_TIME:(x))
#define DHCP_AM_RENEW_TIME(x) \
  (dss_htonl((0.5 * dss_ntohl(DHCP_AM_LEASE_TIME((x))))))
#define DHCP_AM_REBIND_TIME(x) \
  (dss_htonl((0.875 * dss_ntohl(DHCP_AM_LEASE_TIME((x))))))

#if defined(T_WINNT)
#error code not present
#endif /* T_WINNT */

/* Lease structre */
typedef struct dhcp_am_lease
{
  /* IP address pool for our clients */
  uint32  client_ip;
  /* IP address ok? */
  boolean client_bad;
  /* IP address in use */
  boolean client_in_use;
  /* HW address of client */
  uint8 client_hw[DHCP_CHADDR_LEN];
  /* HW address length */
  uint32 client_hw_len;
  /* timeout, absolute, seconds */
  uint32 timeout;
} dhcp_am_lease;

/* Handle structure */
typedef struct dhcp_am_ps_iface_info
{
  /* How we id ourselfs.  It also happens to be our IPV4 address */
  uint32   server_id;
  /* Subnet mask */
  uint32   netmask;
  /* IP address pool for our clients */
  dhcp_am_lease  lease;
  /* internal config */
  dhcp_am_init   init;
  /* linkage config */
  dhcp_am_config config;
  /* cookies: The cookie_ptr takes a value of a pointer to the
   * beginning of this structure. This happens when the core is
   * started.
   */
  struct dhcp_am_ps_iface_info   * cookie_ptr;
  /* The cookie takes the value below when allocated in _new */
#define DHCP_AM_COOKIE ((uint32)(0xA3C00C1E))
  uint32                  cookie;
} dhcp_am_ps_iface_info;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
LOCAL FUNCTION DHCPI_AM_PS_GET_SECS

DESCRIPTION
  This function returns the number of seconds from epoch, we hope.
  This function actually figures out the logic for which timer
  services routine to call based on architecture.

DEPENDENCIES
  There must be some function that returns the number of seconds from
  epoch for the current architecture.

PARAMETERS
  Void - None.

RETURN VALUE
  uint32 - Number of seconds from epoch, however that might be
  defined.

SIDE EFFECTS
  None
===========================================================================*/
static uint32
dhcpi_am_ps_get_secs( void )
{
/* Added for QTF testing. time_get_secs() API doesn't work in case of QTF */
#ifdef T_WINNT
#error code not present
#else /* T_WINNT */

  return time_get_secs();

#endif /* T_WINNT */
} /* dhcpi_am_ps_get_secs */
/*===========================================================================
LOCAL FUNCTION DHCPI_AM_PS_SINGLE_IP_CALCS()

DESCRIPTION
  This function calculates the netmask and server/router IP address
  from the client IP address. This function tries to figure out the
  smallest netmask without handing out the broadcast address.

DEPENDENCIES
  Client IP address must be valid
  Currently only supports IPV4 addrs.  Of course, with IPV6, this
  whole thing is obsolete.

PARAMETERS
  dhcp_am_ps_iface_info * - Handle to the glue needed to get at the connection
                   details.

RETURN VALUE
  None.
  Calculations put into handle structure.

SIDE EFFECTS
  None
===========================================================================*/
static void
dhcpi_am_ps_single_ip_calcs
(
  dhcp_am_ps_iface_info * am
)
{
  uint32 bits = 1;
  uint32 mask = ~bits;
  uint32 ip;
  uint32 host;
  uint32 network;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);

  /* Do the calculation in host byte oder */
  ip = dss_ntohl(am->lease.client_ip);

  /*------------------------------------------------------------------------ 
    If LSB is 1, then skip all 1's till 0 to generate netmask.
    For Example:
    IP      = 10.0.0.3        ( 0000 1010 0000 0000 0000 0000 0000 0011 )
    netmask = 255.255.255.248 ( 1111 1111 1111 1111 1111 1111 1111 1000 )
   
    If LSB is 0, then skip all 0's till 1 to genrate netmask.
    For Example:
    IP      = 10.0.0.8        ( 0000 1010 0000 0000 0000 0000 0000 1000 )
    netmask = 255.255.255.240 ( 1111 1111 1111 1111 1111 1111 1111 0000 )
  ------------------------------------------------------------------------*/
  if (bits & ip) 
  {
    while( (bits & ip) )
    {
      bits <<= 1;
      mask <<= 1;
    }
  }
  else
  {
    while( !(bits & ip) )
    {
      bits <<= 1;
      mask <<= 1;
    }
  }

#define DHCP_AM_MIN_MASK ((uint32)(0xfffffffc))
  mask &= DHCP_AM_MIN_MASK;

#if defined(PS_DHCP_IP_ADDR_CLASS_BASED_NETMASK_CALC)
  /*
   * Netmask is calculated on the basis of IP address class.
   * This is because if we choose MIN_MASK the chances of broadcast address
   * coinciding with other addresses, the n/w assigns like - DNS, SIP etc.
   * will increase.
   * Gateway address calculation will be still based on MIN_MASK.
   * This is because if we calculate it based on the calculated netmask value,
   * the value will always be netmask+1 or netmask+2, which increases the
   * chances of this address coinciding with the ip address of another
   * rmnet instance. So we choose MIN_MASK to calculate gateway address.
   * TBD: Not a fool proof solution re-visit in future if needed.
   */

  /* class C address */
  if ( ( ip & 0xE0000000 ) == 0xC0000000 )
  {
    am->netmask  = dss_htonl(0xFFFFFF00);
  }
  else if( ( ip & 0xC0000000 ) == 0x80000000 ) /* class B address */
  {
    am->netmask  = dss_htonl (0xFFFF0000);
  }
  else if( ( ip & 0x80000000 ) == 0x00000000 )   /* class A address */
  {
    am->netmask  = dss_htonl (0xFF000000);
  } 
#elif defined(PS_DHCP_FIXED_NETMASK_255_255_255_255)
  /*
   * IP address class based netmask calculation has few limitations.
   * In Qualnet all the IP addresses are Class A addresses.
   * When a QMI call is made in lab, the netmask will be 255.0.0.0,
   * in that case Windows routes all the packets through QMI interface.
   * In case of Multi-Rmnet calls, all the QMI ip addresses will fall into a
   * single subnet. Then in that case we cannot test the data transfers
   * individually on different PDN's. To overcome these limitations
   * we are fixing the netmask to 255.255.255.255
   * But this may have other limitations like the ones encountered in 
   * field testing.
   * TBD: Not a fool proof solution, more testing is needed.
   */
  am->netmask  = dss_htonl(0xFFFFFFFF);
#else
  am->netmask = dss_htonl (mask);
#endif /* PS_DHCP_IP_ADDR_CLASS_BASED_NETMASK_CALC */

  /*------------------------------------------------------------------------ 
    Generate Server IP by adding 1 to IP address.
    If by adding 1 the host address becomes broadcast address
    then subtract 1 to generate server address.
   
    For Example (Subtract):
    IP      =  10.0.0.10       (0000 1010 0000 0000 0000 0000 0000 1010)
    netmask =  255.255.255.252 (1111 1111 1111 1111 1111 1111 1111 1100)
    Server  =  10.0.0.9        (0000 1010 0000 0000 0000 0000 0000 1001)
   
    For Example (Addition):
    IP      =  10.0.0.11       (0000 1010 0000 0000 0000 0000 0000 1011)
    netmask =  255.255.255.248 (1111 1111 1111 1111 1111 1111 1111 1000)
    Server  =  10.0.0.12       (0000 1010 0000 0000 0000 0000 0000 1100)
  ------------------------------------------------------------------------*/
  if ( (~mask & (ip + 1)) == ~mask )
  {
    host = ~mask & (ip - 1);
  }
  else
  {
    host = ~mask & (ip + 1);
  }

  network = mask & ip;

  /*------------------------------------------------------------------------ 
    Combine Network & Host address to form server address.
  ------------------------------------------------------------------------*/
  ip = network | host;

  am->server_id = dss_htonl(ip);
} /* dhcpi_am_ps_single_ip_calcs */

/*===========================================================================
LOCAL FUNCTION DHCPI_AM_PS_SINGLE_CHECK_LEASE_TIME()

DESCRIPTION
  This function checks the time on the lease associated with the
  handle passed in.  If the lease has expired, do the paperwork to
  free the lease.

DEPENDENCIES
  Address manager must be valid.

PARAMETERS
  dhcp_am_ps_iface_info * am - Address Manager handle to check the lease for.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
static void
dhcpi_am_ps_single_check_lease_time
(
  dhcp_am_ps_iface_info * am
)
{
  ASSERT( NULL != am );
  ASSERT( DHCP_AM_COOKIE == am->cookie );
  ASSERT( am->cookie_ptr == am );

  if ( 0 != am->lease.timeout && dhcpi_am_ps_get_secs() > am->lease.timeout )
  {
    LOG_MSG_INFO3_0( "INFO: Previous lease expired" );
    /* Ok, free the address.  This includes marking it as good */
    am->lease.client_bad = FALSE;
    am->lease.client_in_use = FALSE;
    am->lease.timeout = 0;
    if ( NULL != am->config.addr_free )
    {
      am->config.addr_free( am->config.addr_free_handle,
                            am->lease.client_ip,
                            am->lease.client_hw,
                            am->lease.client_hw_len );
    }
  }
} /* dhcpi_am_ps_single_check_lease_time */

/*===========================================================================
LOCAL FUNCTION DHCPI_AM_PS_SINGLE_ALLOC_LEASE()

DESCRIPTION
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
  dhcp_am_ps_single_start must have been called with the handle.

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
static void
dhcpi_am_ps_single_alloc_lease
(
  dhcp_am_ps_iface_info * am,
  dhcp_core_msg_info * core
)
{
  ASSERT(NULL != am);
  ASSERT(NULL != core);
  /* Mark in use and by whom */
  memscpy( am->lease.client_hw,DHCP_CHADDR_LEN, core->chaddr, DHCP_CHADDR_LEN );
  am->lease.client_hw_len = core->hlen;
  am->lease.client_in_use = TRUE;
  /* Fill out the info in the core */
  core->siaddr = core->server_id = am->server_id;
  core->yiaddr = am->lease.client_ip;
  core->netmask = am->netmask;
  core->ip_lease = DHCP_AM_LEASE_TIME(core->ip_lease); /**/
  core->renew_time = DHCP_AM_RENEW_TIME(core->ip_lease); /**/
  core->rebind_time = DHCP_AM_REBIND_TIME(core->ip_lease);
  am->lease.timeout = DHCP_AM_LOCAL_LEASE_TIME(core->ip_lease) +
    dhcpi_am_ps_get_secs();
} /* dhcpi_am_ps_single_alloc_lease */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_NEW()

DESCRIPTION
  This function initializes a dhcp_am handle.

DEPENDENCIES
  The handle must be "started" after the other subsystems have been
  created.  The handle should be freed when it is no longer needed.
  This function also calculates the default route ip address and
  netmask that should be provided to clients.


PARAMETERS
  dhcp_am_init * - Internal configuration information.  This includes
                   the interface to connect to.

RETURN VALUE
  handle for calling am functions. NULL on failure.

SIDE EFFECTS
  None
===========================================================================*/
void *
dhcp_am_ps_single_new
(
  dhcp_am_init * init
)
{
  dhcp_am_ps_iface_info * am;
  ps_ip_addr_type         ip_addr;
  ps_iface_ioctl_dhcp_addr_config_type addr_cfg;
  int                     ret_val;
  int16                   errno_val;

  LOG_MSG_INFO3_0( "INFO: --- DHCP AM NEW ---" );
  /* sanity check info */
  ASSERT(NULL != init);
  ASSERT(NULL != init->d_malloc);
  ASSERT(NULL != init->d_free);
  /* Get some memory for our handle */
  am = init->d_malloc(sizeof(dhcp_am_ps_iface_info));
  if ( NULL == am )
  {
    DATA_ERR_FATAL("FAIL: Insufficent memory to init DHCP am");
    return NULL;
  }
  memset( am, 0, sizeof(dhcp_am_ps_iface_info) );
  memset( &addr_cfg, 0, sizeof(ps_iface_ioctl_dhcp_addr_config_type));
  am->init = *init;
  am->cookie = DHCP_AM_COOKIE;

  addr_cfg.gw_iface_ptr = am->init.gw_iface;
  ret_val = ps_iface_ioctl( am->init.iface,
                            PS_IFACE_IOCTL_GET_DHCP_ADDR_CONFIG,
                            &addr_cfg, &errno_val);
  if (ret_val < 0)
  {
    /* get IP address from Um iface */
    ip_addr.type = IPV4_ADDR;
    ps_iface_get_addr(am->init.iface, &ip_addr);

    if ( IPV4_ADDR != ip_addr.type )
    {
      LOG_MSG_ERROR_3("FAIL: Didn't get IPV4_ADDR %d for iface 0x%p in state %d",
                      ip_addr.type, am->init.iface, 
                      ps_iface_state(am->init.iface));
      init->d_free(am);
      return NULL;
    }
  
    if ( 0 == ip_addr.addr.v4.ps_s_addr || 
         0xFFFFFFFF == ip_addr.addr.v4.ps_s_addr )
    {
      LOG_MSG_ERROR_1("FAIL: Unable to obtain ip address %lx",
                      ip_addr.addr.v4.ps_s_addr);
      init->d_free(am);
      return NULL;
    }

    am->lease.client_ip = ip_addr.addr.v4.ps_s_addr;
     
    /* Figure out the mask and server */
    dhcpi_am_ps_single_ip_calcs(am);
  }
  else
  {
    /* get IP info from IOCTL config */
    am->lease.client_ip = addr_cfg.client_ip_addr;
    am->server_id = addr_cfg.gw_ip_addr;
    am->netmask = addr_cfg.subnet_mask;
  }

  return am;
} /* dhcp_am_ps_single_new */

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_START()

DESCRIPTION
  This function registers the functions that interface the DHCP Address
  Manager with other DHCP systems.
DEPENDENCIES
  Handle must have been allocated by dhcp_am_ps_single_new.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  dhcp_am_config * - pointer to functions used to connect to other
                     systems.

RETURN VALUE
  boolean - Success/TRUE or failure/FALSE.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_ps_single_start
(
  void * handle,
  dhcp_am_config * config
)
{
  dhcp_am_ps_iface_info * am = handle;

  LOG_MSG_INFO3_0( "INFO: --- DHCP AM START ---" );
  ASSERT(NULL != am);
  ASSERT(am->cookie == DHCP_AM_COOKIE);
  ASSERT(NULL != config);

  am->config = *config;
  am->cookie_ptr = am;
  return TRUE;
} /* dhcp_am_ps_single_start */

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_STOP()

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
dhcp_am_ps_single_stop
(
  void ** handle
)
{
  dhcp_am_ps_iface_info * am;

  ASSERT(NULL != handle);
  am = *handle;
  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  if (am != am->cookie_ptr)
  {
    LOG_MSG_INFO1_0("am_ps_single_stop() called before am_ps_single_start");
  }

  am->init.d_free(am);
  LOG_MSG_INFO3_0( "INFO: --- DHCP AM STOP ---" );
  *handle = NULL;
} /* dhcp_am_ps_single_stop */

/*===========================================================================
LOCAL FUNCTION DHCP_AM_PS_SINGLE_GET_LEASE()

DESCRIPTION
  This function allocates a lease based on the information that is
  passed in and the state of the lease.  Calls local function
  _alloc_lease() if lease needs to be allocated.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

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
dhcp_am_ps_single_get_lease
(
  void * handle,
  dhcp_core_msg_info * core
)
{
  dhcp_am_ps_iface_info * am = handle;
  boolean ret = FALSE;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != core);

  dhcpi_am_ps_single_check_lease_time(am);

/* NOTES:
 * For the rules on this, see RFC2131, 4.3.2, paricularly the paragraph on
 * DHCPREQEST generated during INIT-REBOOT state.
 */
  /* If we have been marked bad, do nothing! */
  if ( FALSE == am->lease.client_bad )
  {
    /* If we have an address to hand out, or we match the current ... */
    if ( FALSE == am->lease.client_in_use ||
         ( am->lease.client_hw_len == core->hlen &&
           0 == memcmp(am->lease.client_hw, core->chaddr, core->hlen)))
    {
      switch ( core->dhcp_msg_request )
      {
      case DHCP_DHCPREQUEST:
        if ( am->lease.client_ip != core->requested_ip &&
             am->lease.client_ip != core->ciaddr )
        {
          core->dhcp_msg_reply = DHCP_DHCPNACK;
          core->siaddr = core->server_id = am->server_id;
        }
        else
        {
          dhcpi_am_ps_single_alloc_lease(am, core);
          ret = TRUE;
        }
        break;

      case DHCP_DHCPDISCOVER:
        /* For DISCOVER, ignore the requested IP */
        dhcpi_am_ps_single_alloc_lease(am, core);
        ret = TRUE;
        break;

      default:
        LOG_MSG_ERROR_1("Invalid msg_type in get_lease %d",
                        core->dhcp_msg_request );
        ASSERT(0);
        break;
      }
    }
  }
  return ret;
} /* dhcp_am_ps_single_get_lease */

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_FREE_LEASE()

DESCRIPTION
  This function frees a previously allocated lease.  This function can be
  called either on a release from a client, or is a previously allocated lease
  is not actually sent out due to other errors.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

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
dhcp_am_ps_single_free_lease
(
  void * handle,
  dhcp_core_msg_info * core
)
{
  dhcp_am_ps_iface_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != core);

  /* First, is this for us? */
  if ( am->lease.client_ip == core->ciaddr )
  {
    /* Ok, free the address.  This includes marking it as good */
    am->lease.client_bad = FALSE;
    am->lease.client_in_use = FALSE;
    am->lease.timeout = 0;
    if ( NULL != am->config.addr_free )
    {
      am->config.addr_free( am->config.addr_free_handle,
                            am->lease.client_ip,
                            am->lease.client_hw,
                            am->lease.client_hw_len );
    }
    return TRUE;
  }
  return FALSE;
} /* dhcp_am_ps_single_free_lease */

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_ERR_LEASE()

DESCRIPTION
  This function informs the DHCP Address Manager that a lease that was
  previously given out had a problem.  This function is called on a
  DHCPDECLINE message from the client.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

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
dhcp_am_ps_single_err_lease
(
  void * handle,
  dhcp_core_msg_info * core
)
{
  dhcp_am_ps_iface_info * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != core);

  /* Ok. RFC-2131 has the following to say:
   *   If the server receives a DHCPDECLINE message, the client has
   *   discovered through some other means that the suggested network
   *   address is already in use.  The server MUST mark the network
   *   address as not available and SHOULD notify the local system
   *   administrator of a possible configuration problem.
   * First, figure out if we have any reason to be concerned:
   */
  if ( core->server_id == am->server_id )
  {
    /* Now, in the current incarnation, we only have 1 IP address to
     * hand out.  If the client sees another host with the same IP
     * address, this is confused.  Squeek, to the extent that we can:
     */
    LOG_MSG_INFO1_1("WARN: Client detected dup IP address 0x%lx",
                    core->requested_ip );
    /* Did we give out this address? */
    if ( core->requested_ip == am->lease.client_ip )
    {
      /* Indeed.  Mark it bad */
      am->lease.client_bad = TRUE;
      return TRUE;
    }
  }
  return FALSE;
} /* dhcp_am_ps_single_err_lease */

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_DNS_FIRST()

DESCRIPTION
  This function fetches the DNS server information (if available) and
  fills out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  void ** - This is user data that will be returned to get to the next
            DNS address.
  ip_add_type * - The first dns address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  void ** - user data to get to next address.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_ps_single_dns_first
(
  void * handle,
  void **user_data,
  uint32 * ip_addr
)
{
  dhcp_am_ps_iface_info   * am = handle;
  ip_addr_type     ipa, ipb;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != user_data);
  ASSERT(NULL != ip_addr);

  ipa.type = IPV4_ADDR;
  ipb.type = IPV4_ADDR;
  ps_iface_get_v4_dns_addrs(am->init.iface, &ipa, &ipb);

  if ( IPV4_ADDR == ipa.type && 0 != ipa.addr.v4 )
  {
    *ip_addr = ipa.addr.v4;
    *user_data = 0;
    return TRUE;
  }

  return FALSE;
} /* dhcp_am_ps_single_dns_first */

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_DNS_NEXT()

DESCRIPTION
  This function fetches the DNS server information (if available) and fills
  out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

PARAMETERS
  void * - Handle to the glue needed to get at the connection
                       details.
  void ** - This is user data that will be returned to get to the next
            DNS address.
  ip_add_type * - The next dns address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  void ** - user data to get to next address.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_ps_single_dns_next
(
  void * handle,
  void ** user_data,
  uint32 * ip_addr
)
{
  dhcp_am_ps_iface_info   * am = handle;
  ip_addr_type     ipa, ipb;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != user_data);

  if ( 0 == *user_data )
  {
    *user_data = (void *)1;
    ipa.type = IPV4_ADDR;
    ipb.type = IPV4_ADDR;
    ps_iface_get_v4_dns_addrs(am->init.iface, &ipa, &ipb);
    if ( IPV4_ADDR == ipb.type && 0 != ipb.addr.v4 )
    {
      *ip_addr = ipb.addr.v4;
      return TRUE;
    }
  }

  *user_data = NULL;
  return FALSE;
} /* dhcp_am_ps_single_dns_next */
/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_NBNS_FIRST()

DESCRIPTION
  This function fetches the NBNS server information (if available) and
  fills out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

PARAMETERS
  handle     - Handle to the glue needed to get at the connection
               details.
  user_data  - This is a cookie that will be returned to get to the next
               NBNS address.
  ip_addr    - The first NBNS address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  cookie  - cookie to get to next address.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_ps_single_nbns_first
( 
  void * handle,
  void ** user_data,
  uint32 * ip_addr
)
{

  dhcp_am_ps_iface_info   * am = handle;
  ip_addr_type     ipa, ipb;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != user_data);
  ASSERT(NULL != ip_addr);

  ipa.type = IPV4_ADDR;
  ipb.type = IPV4_ADDR;
  ps_iface_get_v4_nbns_addrs(am->init.iface, &ipa, &ipb);

  if ( IPV4_ADDR == ipa.type && 0 != ipa.addr.v4 )
  {
    *ip_addr = ipa.addr.v4;
    *user_data = 0;
    return TRUE;
  }

  return FALSE;

}
/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_NBNS_NEXT()

DESCRIPTION
  This function fetches the NBNS server information (if available) and
  fills out the dhcp_core_msg_info with these addresses.

DEPENDENCIES
  dhcp_am_ps_single_start must have been called with the handle.

PARAMETERS
  handle     - Handle to the glue needed to get at the connection
               details.
  user_data  - This is a cookie that will be returned to get to the next
               NBNS address.
  ip_addr    - The next NBNS address.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - no more addresses.
  cookie  - cookie to get to next address.

SIDE EFFECTS
  None
===========================================================================*/
boolean
dhcp_am_ps_single_nbns_next
( 
  void * handle,
  void ** user_data,
  uint32 * ip_addr
)
{
  dhcp_am_ps_iface_info   * am = handle;
  ip_addr_type     ipa, ipb;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != user_data);

  if ( 0 == *user_data )
  {
    *user_data = (void *)1;
    ipa.type = IPV4_ADDR;
    ipb.type = IPV4_ADDR;
    ps_iface_get_v4_nbns_addrs(am->init.iface, &ipa, &ipb);
    if ( IPV4_ADDR == ipb.type && 0 != ipb.addr.v4 )
    {
      *ip_addr = ipb.addr.v4;
      return TRUE;
    }
  }

  *user_data = NULL;
  return FALSE;
}

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_SERVER_ADDR_INFO()

DESCRIPTION
  This function fetches the IP address of the server (us)

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.

RETURN VALUE
  boolean - TRUE is address is valid. FALSE - a problem.

SIDE EFFECTS
  None
===========================================================================*/
uint32
dhcp_am_ps_single_addr_info_server_id
(
  void * handle
)
{
  dhcp_am_ps_iface_info   * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);

  return am->server_id;
} /* dhcp_am_ps_single_addr_info_server_id */

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_ADDR_INFO_NETMASK()

DESCRIPTION
  This function fetches the netmask of the subnet the server is on.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.

RETURN VALUE
  uint32 - The IPV4 netmask of the subnet we are on.

SIDE EFFECTS
  None
===========================================================================*/
uint32
dhcp_am_ps_single_addr_info_netmask
(
  void * handle
)
{
  dhcp_am_ps_iface_info   * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);

  return am->netmask;
} /* dhcp_am_ps_single_addr_info_netmask */

/*===========================================================================
FUNCTION DHCP_AM_PS_SINGLE_MTU()

DESCRIPTION
  This function fetches the MTU of the server (us)

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.

RETURN VALUE
  uint32 - The MTU of the server.

SIDE EFFECTS
  None
===========================================================================*/
uint32
dhcp_am_ps_single_mtu
(
  void * handle
)
{
  dhcp_am_ps_iface_info   * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);

  return PS_IFACE_GET_MTU(am->init.iface);
} /* dhcp_am_ps_single_mtu */


/*===========================================================================
FUNCTION DHCP_AM_PS_IFACE_GET_IP_CONFIG()

DESCRIPTION
  This function fetches the IP config for this AM.

DEPENDENCIES
  Handle must have been previously allocated and started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  3*uint32 * - IP params

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dhcp_am_ps_iface_get_ip_config
(
  void   * handle,
  uint32 * client_ip,
  uint32 * server_id,
  uint32 * netmask
)
{
  dhcp_am_ps_iface_info   * am = handle;

  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);

  *client_ip = am->lease.client_ip;
  *server_id = am->server_id;
  *netmask = am->netmask;
} /* dhcp_am_ps_iface_get_ip_config */ 

/*===========================================================================
FUNCTION DHCP_AM_PS_IFACE_SIP_ADDR_LIST()

DESCRIPTION
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
dhcp_am_ps_iface_sip_addr_list
(
  void * handle,
  ip_addr_type * sip_addr_list,
  uint32 * count
)
{
  dhcp_am_ps_iface_info   * am = handle;
  
  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != sip_addr_list);
  ASSERT(NULL != count);

  ps_iface_get_sip_serv_addr(am->init.iface, sip_addr_list, count);

}

/*===========================================================================
FUNCTION DHCP_AM_PS_IFACE_SIP_DOMAIN_NAME_LIST()

DESCRIPTION
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
dhcp_am_ps_iface_sip_domain_name_list
(
  void * handle,
  ps_iface_domain_name_type  *sip_domain_name_list,
  uint32 * count
)
{
  dhcp_am_ps_iface_info   * am = handle;
  
  ASSERT(NULL != am);
  ASSERT(DHCP_AM_COOKIE == am->cookie);
  ASSERT(am == am->cookie_ptr);
  ASSERT(NULL != sip_domain_name_list);
  ASSERT(NULL != count);

  ps_iface_get_sip_domain_names(am->init.iface,
                                sip_domain_name_list, count);
}
