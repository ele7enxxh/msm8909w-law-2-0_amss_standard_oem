/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  D H C P _ C L I E N T _ C O N F I G . C

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol client configuration code.
  This file provides the shim layer between the core and the stacks
  that get parameters from DHCP.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004-2014 QUALCOMM Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcp_client_config.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/04/14    ss     Added support for generating 
                   IFACE_EXTENDED_IP_CONFIG_EX_EV
01/10/14    vl     Optimization of F3 messages
08/22/13    ds     Fix to ignore the incoming socket read event when the 
                   socket is closed.
06/21/13    ash    Support MTU upto 2000
06/10/13    ss     Replacing memcpy with strlcpy for domain name option.
12/14/12    ssb    Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR
01/29/09    pp     Added support for SLIP interface.
09/26/08    am     Changed malloc/free names for off target support.
05/10/07    es     Fixed bug where ip_address is cleared unconditionally in
                   configuration_start().
05/02/07    es     Moved setting of handle from mgr to config layer.
                   Removed use of dhcp_client_params struct to correct
                   layering problem.
04/17/07    es     Added function to trigger EXTENDED_IP_CONFIG event,
                   use params struct flags to determine whether to set config
                   to ps_iface.
10/31/06    es     Fixed lint issues.
08/14/06    es     Added SIP address and domain name provisioning.
04/25/06    msr    L4/Tasklock code review changes
11/05/05    rt     Fixed coverity errors.
08/15/05    clp    Added a clear of the IP address in the start.
05/12/05    clp    Fixed lint issues
12/30/04    clp    Code review: static functions now have dhcpi_ prefix
10/28/04    clp    Initial development work done.
===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "dhcp_clienti.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "ps_iface.h"
#include "ps_arp.h"
#include "ds_Utils_DebugMsg.h"

#include <stringl/stringl.h>
#include "ps_in.h"

typedef struct dhcp_client_configuration_info {
  /* module init info */
  dhcp_client_configuration_init init;
  /* module config info */
  dhcp_client_configuration_config config;

  /* ps_iface_type is cached at start to avoid having to do it over
   * and over again.
   */
  ps_iface_type * ps_iface;
  /* cookies: The cookie_ptr takes a value of a pointer to the
   * beginning of this structure. This happens when the shim is
   * started.
   */
  struct dhcp_client_configuration_info * cookie_ptr;
  /* The cookie takes the value below when allocated in _new */
#define DHCP_CLIENT_CONFIGURATION_COOKIE ((uint32)0XC04F1614)
  uint32 cookie;
} dhcp_client_configuration_info;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_IS_SIP_SERVER_ADDR_UPDATED

DESCRIPTION
  This helper function checks if the sip server address in the iface is same
  as the new address specified.

PARAMETERS
  info                : DHCP client configuration info
  new_addr_array      : New address array.
  new_addr_array_size : Size of the new address array.

RETURN VALUE
  boolean - TRUE if the new address is different than what is stored in the 
            iface, FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean
dhcpi_client_is_sip_server_addr_updated
(
  dhcp_client_configuration_info  *info,
  ps_ip_addr_type                 *new_addr_array,
  uint32                           new_addr_array_size
)
{
#define SIP_SERVER_ADDR_ARRAY_MAX_SIZE 10
  ip_addr_type   *old_addr_array = NULL;
  uint32          old_addr_array_size = SIP_SERVER_ADDR_ARRAY_MAX_SIZE;
  uint32          counter = 0;
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO2_0("dhcpi_client_is_sip_server_addr_updated()");

  /* Validate */
  if( info == NULL || new_addr_array == NULL )
  {
    LOG_MSG_ERROR_2( "NULL args, info: %d, new_addr_array: %d", 
                     info, new_addr_array );
    return FALSE;
  }

  /* Fetch the existing address list from iface structure */
  old_addr_array = info->init.d_malloc( old_addr_array_size *
                                        sizeof(ps_ip_addr_type) );
  if ( NULL == old_addr_array )
  {
    LOG_MSG_ERROR_0("dhcpi_client_is_sip_server_addr_updated(): "
                    "Memory allocation failed.");
    return FALSE;
  }
  else
  {
    memset( old_addr_array, 0, 
            old_addr_array_size * sizeof(ps_ip_addr_type));
  }

  ps_iface_get_sip_serv_addr( info->ps_iface,
                              old_addr_array, &old_addr_array_size );

  /* Compare array sizes as a quick check */
  if( old_addr_array_size != new_addr_array_size )
  {  
    /* Free the allocated memory and return */
    info->init.d_free(old_addr_array);
    return TRUE;
  }

  /* Compare against new address array values */
  for( counter = 0; counter < old_addr_array_size; counter++ )
  {
    if( old_addr_array[counter].type != new_addr_array[counter].type ||
        0 != memcmp( &old_addr_array[counter].addr.v4,
                     &new_addr_array[counter].addr.v4,
                     sizeof(struct ps_in_addr)) )
    {
      /* Free the allocated memory and return */
      info->init.d_free(old_addr_array);
      return TRUE;
    }
  }

  /* Free the allocated memory and return */
  info->init.d_free(old_addr_array);
  return FALSE;
}

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_IS_SIP_DOMAIN_NAME_UPDATED

DESCRIPTION
  This helper function checks if the sip domain names in the iface are same
  as the new ones specified.

PARAMETERS
  info                       : DHCP client configuration info
  new_domain_name_array      : New address array.
  new_domain_name_array_size : Size of the new address array.

RETURN VALUE
  boolean - TRUE if the new address is different than what is stored in the 
            iface, FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean
dhcpi_client_is_sip_domain_name_updated
(
  dhcp_client_configuration_info  *info,
  ps_iface_domain_name_type       *new_domain_name_array,
  uint32                           new_domain_name_array_size
)
{
#define SIP_DOMAIN_NAME_ARRAY_MAX_SIZE 10
  ps_iface_domain_name_type *old_domain_name_array = NULL;
  uint32                     old_domain_name_array_size = 
                               SIP_DOMAIN_NAME_ARRAY_MAX_SIZE;
  uint32                     counter = 0;
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO2_0("dhcpi_client_is_sip_domain_name_updated()");

  /* Validate */
  if( info == NULL || new_domain_name_array == NULL )
  {
    LOG_MSG_ERROR_2( "NULL args, info: %d, new_domain_name_array: %d", 
                     info, new_domain_name_array );
    return FALSE;
  }

  /* Fetch the existing domain name array from iface structure */
  old_domain_name_array = info->init.d_malloc( 
                            old_domain_name_array_size *
                            sizeof(ps_iface_domain_name_type) );
  if ( NULL == old_domain_name_array )
  {
    LOG_MSG_ERROR_0("dhcpi_client_is_sip_domain_name_updated(): "
                    "Memory allocation failed.");
    return FALSE;
  }
  else
  {
    memset( old_domain_name_array, 0, 
            old_domain_name_array_size * sizeof(ps_iface_domain_name_type));
  }

  ps_iface_get_sip_domain_names( info->ps_iface,
                                 old_domain_name_array,
                                 &old_domain_name_array_size );

  /* Compare array sizes as a quick check */
  if( old_domain_name_array_size != new_domain_name_array_size )
  {  
    /* Free the allocated memory and return */
    info->init.d_free(old_domain_name_array);
    return TRUE;
  }

  /* Compare against new address array values */
  for( counter = 0; counter < old_domain_name_array_size; counter++ )
  {
    if( 0 != strncasecmp( old_domain_name_array[counter].domain_name,
                          new_domain_name_array[counter].domain_name,
                          PS_IFACE_MAX_DOMAIN_NAME_SIZE) )
    {
      /* Free the allocated memory and return */
      info->init.d_free(old_domain_name_array);
      return TRUE;
    }
  }

  /* Free the allocated memory and return */
  info->init.d_free(old_domain_name_array);
  return FALSE;
}

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_CONFIGURATION_ARP_CB

Description:
  This function is call from the ARP layer to indicate a successful
  arp probe (bad for us) or failure.  Right now we ignore the
  failure case and impliment our own timer.

Arguements:
  void * handle - a pointer to our info.
  arp_probe_status - Success or failure.

Return value:
  void

Dependencies:
===========================================================================*/
static void
dhcpi_client_configuration_arp_cb
(
  void * handle,
  arp_probe_status status
)
{
  dhcp_client_configuration_info * info = handle;

  /* Sanity check parameters */
  if (NULL == info)
  {
    LOG_MSG_ERROR_0("Got NULL info");
    return;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("Cookie pointer doesn't match %x %x",
                    info->cookie_ptr, info);
    /*free the memory in the info before returning*/
    info->init.d_free(info);
    return;
  }
  if (DHCP_CLIENT_CONFIGURATION_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie doesn't match %x %x",
                    info->cookie, DHCP_CLIENT_CONFIGURATION_COOKIE);
    /*free the memory in the info before returning*/
    info->init.d_free(info);
    return;
  }

  if ( ARP_PROBE_SUCCESS == status )
  {
    info->config.decline( info->config.decline_handle );
  }
} /* dhcpi_client_configuration_arp_cb */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_CONFIGURATION_SET_SIP_ADDR

DESCRIPTION
  This helper function does the appropriate set up to call the ps_iface API
  setting the SIP addresses stored in the provision struct.

DEPENDENCIES
  The input address list should not be empty.

PARAMETERS
  dhcp_client_configuration_info * info     - Handle to the config shim
  dhcp_client_addr               * addr_ptr - Pointer to the input address
                                               list
  ps_iface_ioctl_extended_ip_config_ex_type *config_change_mask_ptr
                                            - Mask to be updated 

RETURN VALUE
  boolean - Indicates success/failure.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean
dhcpi_client_configuration_set_sip_addr
(
  dhcp_client_configuration_info            *info,
  dhcp_client_sip_addr                      *addr_ptr,
  ps_iface_ioctl_extended_ip_config_ex_type *config_change_mask_ptr
)
{

  ps_ip_addr_type      * ip_addr_array = NULL;
  dhcp_client_sip_addr * addr_head = addr_ptr; /* save the front of the list */
  errno_enum_type        ps_errno;
  uint32                 listcount = 0;
  int32                  i;
  boolean                ret = TRUE;

  /* Sanity check parameters */
  if ( NULL == info )
  {
    DATA_ERR_FATAL("Got NULL info");
    return FALSE;
  }
  if ( info->cookie_ptr != info )
  {
    LOG_MSG_ERROR_2("Cookie pointer doesn't match %x %x",
                    info->cookie_ptr, info);
    ASSERT(0);
    return FALSE;
  }
  if ( DHCP_CLIENT_CONFIGURATION_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2("Cookie doesn't match %x %x",
                    info->cookie, DHCP_CLIENT_CONFIGURATION_COOKIE);
    ASSERT(0);
    return FALSE;
  }

  LOG_MSG_INFO2_0( "Setting SIP Addr List in PS");

  /* Count list length */
  while ( NULL != addr_ptr )
  {
    listcount++;
    addr_ptr = addr_ptr->next;
  }
  if ( 0 < listcount)
  {
    /* Allocate array of ps_ip_addr used in ps_iface call */
    ip_addr_array = info->init.d_malloc(listcount *
                                          sizeof(ps_ip_addr_type));

    if ( NULL == ip_addr_array )
    {
      LOG_MSG_ERROR_0("malloc for ip_addr_array failed");
      ret = FALSE;
    }
    else
    {
      for (i = listcount - 1, addr_ptr = addr_head;
          ( i >= 0 ) && ( NULL != addr_ptr );
          i--, addr_ptr = addr_ptr->next)
      {
        /* initialize array element */
        ip_addr_array[i].type = IPV4_ADDR;
        ip_addr_array[i].addr.v4.ps_s_addr = addr_ptr->addr;
      }

      /* Check if the sip server address is updated or not */
      if( dhcpi_client_is_sip_server_addr_updated(
            info, ip_addr_array, listcount) )
      {
        LOG_MSG_INFO2_0("dhcpi_client_configuration_set_sip_addr(): "
                        "SIP server updated, setting in cfg chg msk");
        *config_change_mask_ptr |= PS_IFACE_EXT_IP_CFG_MASK_PCSCF_ADDR;
      }
      else
      {
        LOG_MSG_INFO2_0("dhcpi_client_configuration_set_sip_addr(): "
                        "SIP server NOT updated, clearing in cfg chg msk");
        *config_change_mask_ptr &= ~PS_IFACE_EXT_IP_CFG_MASK_PCSCF_ADDR;
      }

      /* call ps_iface API function to set SIP server address */
      if ( -1 == ps_iface_set_sip_serv_addr(info->ps_iface,
                 ip_addr_array, listcount, &ps_errno))
      {
        LOG_MSG_ERROR_1( "ps_iface_set_sip_serv_addr failed with errno %d",
                         ps_errno);
        ret = FALSE;
      }
    } /* if( NULL != ip_addr_arr ) */

    info->init.d_free(ip_addr_array);
    ip_addr_array = NULL;
  } /* if( 0 < listcount ) */

  if ( FALSE == ret )
  {
    LOG_MSG_ERROR_0( "Failed to provision SIP Addr List successfully");
  }

  return ret;

} /* dhcpi_client_configuration_set_sip_addr */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_CONFIGURATION_SET_SIP_DOMAIN_NAMES

DESCRIPTION
  This helper function does the appropriate set up to call the ps_iface API
  setting the SIP domain names stored in the provision struct.

DEPENDENCIES
  The input domain name list should not be empty.

PARAMETERS
  dhcp_client_configuration_info * info   - Handle to the config shim
  dhcp_client_sip_domain_name    * dn_ptr - Pointer to the head of the
                                             input domain name list
  ps_iface_ioctl_extended_ip_config_ex_type *config_change_mask_ptr
                                          - Mask to be updated 

RETURN VALUE
  boolean - Indicates success/failure.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean
dhcpi_client_configuration_set_sip_domain_names
(
  dhcp_client_configuration_info            *info,
  dhcp_client_sip_domain_name               *dn_ptr,
  ps_iface_ioctl_extended_ip_config_ex_type *config_change_mask_ptr
)
{

  ps_iface_domain_name_type   * domain_name_array = NULL;
  dhcp_client_sip_domain_name * dn_head = dn_ptr; /* save head of list */
  errno_enum_type               ps_errno;
  uint32                        listcount = 0;
  int32                         i;
  boolean                       ret = TRUE;

  /* Sanity check parameters */
  if ( NULL == info )
  {
    DATA_ERR_FATAL("Got NULL info");
    return FALSE;
  }
  if ( info->cookie_ptr != info )
  {
    LOG_MSG_ERROR_2("Cookie pointer doesn't match %x %x",
                    info->cookie_ptr, info);
    ASSERT(0);
    return FALSE;
  }
  if ( DHCP_CLIENT_CONFIGURATION_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2("Cookie doesn't match %x %x",
                    info->cookie, DHCP_CLIENT_CONFIGURATION_COOKIE);
    ASSERT(0);
    return FALSE;
  }

  LOG_MSG_INFO2_0( "Setting SIP Domain Name List in PS");
  /* Count list length */
  while ( NULL != dn_ptr )
  {
    listcount++;
    dn_ptr = dn_ptr->next;
  }

  if ( 0 < listcount )
  {
    /* Allocate array of ps_iface_domain_name_type ptrs
     * used in ps_iface call */
    domain_name_array = info->init.d_malloc(listcount *
                                    sizeof(ps_iface_domain_name_type));

    if ( NULL == domain_name_array )
    {
      LOG_MSG_ERROR_0("malloc for domain_name_array failed");
      ret = FALSE;
    }
    else
    {
      for (i = listcount - 1, dn_ptr = dn_head;
           ( i >=0 ) && ( NULL != dn_ptr );
           i--, dn_ptr = dn_ptr->next)
      {
        /* Add 1 for NULL terminating char */
        if ( PS_IFACE_MAX_DOMAIN_NAME_SIZE < dn_ptr->domain_name_len + 1 )
        {

          LOG_MSG_ERROR_2("(domain_name_len + 1) too large: %d, max %d",
                          dn_ptr->domain_name_len+1,
                          PS_IFACE_MAX_DOMAIN_NAME_SIZE);
          ret = FALSE;
          break;
        }
        else
        {
          /* copy the null-terminated string to the array entry */
          strlcpy( domain_name_array[i].domain_name,
                   (const char*)dn_ptr->domain_name,
                   dn_ptr->domain_name_len + 1);
        }
      } /* for loop */

      /* call ps_iface API function to set SIP server address */
      if ( TRUE == ret )
      {
        /* Check if the sip domain names are updated or not */
        if( dhcpi_client_is_sip_domain_name_updated(
              info, domain_name_array, listcount) )
        {
          LOG_MSG_INFO2_0("dhcpi_client_configuration_set_sip_domain_names(): "
                          "SIP domain names updated, setting in cfg chg msk");
          *config_change_mask_ptr |= PS_IFACE_EXT_IP_CFG_MASK_PCSCF_NAME;
        }
        else
        {
          LOG_MSG_INFO2_0("dhcpi_client_configuration_set_sip_domain_names(): "
                          "SIP domain names NOT updated, clearing cfg chg msk");
          *config_change_mask_ptr &= ~PS_IFACE_EXT_IP_CFG_MASK_PCSCF_NAME;
        }

        if ( -1 == ps_iface_set_sip_domain_names(info->ps_iface,
                 domain_name_array, listcount, &ps_errno))
        {
          LOG_MSG_ERROR_1("ps_iface_set_sip_domain_names failed with errno %d",
                          ps_errno);
          ret = FALSE;
        }
      }
    } /* if( NULL != domain_name_arr ) */

    info->init.d_free(domain_name_array);
    domain_name_array = NULL;

  } /* if( 0 < listcount ) */

  if ( FALSE == ret )
  {
     LOG_MSG_ERROR_0( "Failed to provision SIP Domain Name List successfully");
  }

  return ret;
} /* dhcpi_client_configuration_set_sip_domain_names */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_CONFIGURATION_SET_HANDLE_IN_PS_IFACE()

DESCRIPTION
  This function will set the dhcp client handle in the ps_iface struct.

DEPENDENCIES
  handle must be valid.

PARAMETERS
  void              * handle - handle to the dhcp client.
  dss_iface_id_type   iface  - interface id

RETURN VALUE
  None.
===========================================================================*/
static boolean
dhcpi_client_configuration_set_handle_in_ps_iface
(
  dhcp_client_configuration_info * info,
  void * handle
)
{
  boolean ret = TRUE;

  /* Sanity check parameters */
  if ( NULL == info )
  {
    DATA_ERR_FATAL("Got NULL info");
    return FALSE;
  }
  if ( DHCP_CLIENT_CONFIGURATION_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2("Cookie doesn't match %x %x",
                    info->cookie, DHCP_CLIENT_CONFIGURATION_COOKIE);
    ASSERT(0);
    return FALSE;
  }

  LOG_MSG_INFO2_0( "Setting DHCP client handle in PS");

  if (-1 == ps_iface_set_dhcp_stateless_client_handle(info->ps_iface, handle))
  {
    LOG_MSG_ERROR_0("ps_iface_set_dhcp_handle failed");
    ret = FALSE;
  }

  return ret;
} /* dhcpi_client_configuration_set_handle_in_ps_iface */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_NEW

Description:
  This function initializes the configuration layer with internal data and
  returns a handle to the core object.

Arguements:
  dhcp_client_configuration_init * init - The information needed to
    initialize the configuration layer.

Return value:
  void * - A handle to the new configuration object, or NULL on failure.

Dependencies:
  This function must be called to get a handle before any other configuration
  functions are called.  The handle must be started (see
  dhcp_clienti_configuration_start() below) before the configuration layer is
  fully functional.
===========================================================================*/
void *
dhcp_clienti_configuration_new
(
   dhcp_client_configuration_init * init
)
{
  dhcp_client_configuration_info * handle;
  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT CONFIGURATION INIT ---" );

  /* Sanity check parameters */
  if (NULL == init)
  {
    DATA_ERR_FATAL("init NULL");
    return NULL;
  }
  if (NULL == init->d_malloc ||
      NULL == init->d_free)
  {
    LOG_MSG_ERROR_2("malloc or free NULL %x %x", init->d_malloc, init->d_free);
    ASSERT(0);
    return NULL;
  }

  handle = init->d_malloc(sizeof(dhcp_client_configuration_info));
  if ( NULL == handle )
  {
    DATA_ERR_FATAL("Insufficent memory to init DHCP configuration layer");
    return NULL;
  }
  memset( handle, 0, sizeof(dhcp_client_configuration_info) );
  handle->init = *init; /* struct copy */
  handle->ps_iface = ps_iface_get_handle( init->iface );
  handle->cookie = DHCP_CLIENT_CONFIGURATION_COOKIE;

  return handle;
} /* dhcp_clienti_configuration_new */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_START

Description:
  This function configures the configuration layer with the information on how
  to interact with the other client modules.

Arguements:
  void * handle - Handle to the configuration object to start.
  dhcp_client_configuration_config * config - The configuration information
  needed to interface with other modules in the DHCP client architecture.
  void * dhcp_handle - Handle for DHCP client to set in ps_iface.

Return value:
  Boolean - TRUE if success, otherwise FALSE.

Dependencies:
  The handle must be have been allocated by dhcp_clienti_configuration_new()
  above.
===========================================================================*/
boolean
dhcp_clienti_configuration_start
(
  void * handle,
  dhcp_client_configuration_config * config,
  void * dhcp_handle
)
{
  boolean ret = TRUE;
  dhcp_client_configuration_info * info = handle;

  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT CONFIGURATION START ---" );
  /* Sanity check parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("Got NULL info");
    return FALSE;
  }
  if (DHCP_CLIENT_CONFIGURATION_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie doesn't match %x %x",
                    info->cookie, DHCP_CLIENT_CONFIGURATION_COOKIE);
    ASSERT(0);
    return FALSE;
  }
  if (NULL == config)
  {
    DATA_ERR_FATAL("config NULL");
    return FALSE;
  }
  /* Verify htype for flavors supported later */
  switch ( info->init.config_params.htype )
  {
  case DHCP_CLIENT_HWTYPE_ETHERNET:
  case DHCP_CLIENT_HWTYPE_EXP_ETHERNET:
  case DHCP_CLIENT_HWTYPE_IEEE_802:
  case DHCP_CLIENT_HWTYPE_LOOPBACK:
  case DHCP_CLIENT_HWTYPE_SERIAL_LINE: /* For SLIP */

    break;

  default:
    LOG_MSG_ERROR_1( "Unknown hardware type=%d",
                     info->init.config_params.htype );
    ASSERT(0);
    return FALSE;
  }

  /* set handle in ps_iface */
  if (TRUE !=
        dhcpi_client_configuration_set_handle_in_ps_iface(info, dhcp_handle))
  {
    LOG_MSG_ERROR_0("Failed to set dhcp handle in ps_iface!");
    ret = FALSE;
  }

  /* Initialize "config" info */
  info->config = *config; /* struct copy */
  info->cookie_ptr = info;

  return ret;
} /* dhcp_clienti_configuration_start */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_STOP

Description:
  This function stops a DHCP client configuration layer object and frees the
  associated memory.  Additionally, the handle pointer will be NULL'd.

Arguements:
  void ** handle - The handle to the configuration layer object to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_configuration_new().
===========================================================================*/
void
dhcp_clienti_configuration_stop
(
  void ** handle
)
{
  dhcp_client_configuration_info * info;

  /* Sanity check parameters */
  if (NULL == handle)
  {
    DATA_ERR_FATAL("Got NULL handle pointer");
    return;
  }
  info = *handle;

  if (NULL == info)
  {
    DATA_ERR_FATAL("Got NULL info");
    return;
  }
  if (DHCP_CLIENT_CONFIGURATION_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie doesn't match %x %x",
                    info->cookie, DHCP_CLIENT_CONFIGURATION_COOKIE);
    ASSERT(0);
    return;
  }

  /* NULL handle in ps_iface */
  if (TRUE !=
        dhcpi_client_configuration_set_handle_in_ps_iface(info, NULL))
  {
    DATA_ERR_FATAL("Failed to clear dhcp handle in ps_iface!");
    return;
  }

  info->init.d_free(info);
  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT CONFIGURATION STOP ---" );
  *handle = NULL;
} /* dhcp_client_configuration_stop */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_TEST

Description:
  This function configures the ARP (or other) layer with the proposed lease to
  verify that the lease is valid.

  If the ARP (or other) layer finds that the lease is not valid, then it
  should promptly call the registered callback which will call back into this
  layer.  This layer will then inform the core about the bad lease.

  If there is nothing wrong with the lease, then no action need be taken.

Arguements:
  void * handle - Handle to the configuration layer object.
  dhcp_client_provision * provision - The lease information needed to
    configure the interface

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_configuration_new().
  The handle must have been started by dhcp_clienti_configuration_start().
===========================================================================*/
void
dhcp_clienti_configuration_test
(
  void * handle,
  dhcp_client_provision * provision
)
{
  dhcp_client_configuration_info * info = handle;

  /* Sanity check parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("Got NULL info");
    return;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("Cookie pointer doesn't match %x %x",
                    info->cookie_ptr, info);
    ASSERT(0);
    return;
  }
  if (DHCP_CLIENT_CONFIGURATION_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie doesn't match %x %x",
                    info->cookie, DHCP_CLIENT_CONFIGURATION_COOKIE);
    ASSERT(0);
    return;
  }
  if ( NULL == provision )
  {
    DATA_ERR_FATAL("NULL Provision");
    return;
  }

  /* Call the approprate link layer duplicate IP detection scheme */
  switch ( info->init.config_params.htype )
  {
  case DHCP_CLIENT_HWTYPE_ETHERNET:
  case DHCP_CLIENT_HWTYPE_EXP_ETHERNET:
  case DHCP_CLIENT_HWTYPE_IEEE_802:
    LOG_MSG_INFO2_0( "Starting arp probe for dup ip addr detection" );
    if ( 0 != arp_probe( info->ps_iface,
                         ARP_BROADCAST_PROBE,
                         provision->ip_addr,
                         dhcpi_client_configuration_arp_cb,
                         handle ) )
    {
      if ( NULL != info->init.status_cb )
      {
        info->init.status_cb( info->init.userdata, DHCP_CLIENT_IP_FAIL );
      }
    }
    break;

  /* For SLIP:: Since SLIP doesn't have ARP, we need to use the serial 
     line type and not the ethernet type */
  case DHCP_CLIENT_HWTYPE_SERIAL_LINE:
    LOG_MSG_INFO2_0( "Serial Line DHCP option, no duplicate IP test." );
    break;

  case DHCP_CLIENT_HWTYPE_LOOPBACK:
    LOG_MSG_INFO2_0( "Loopback IP config test" );
    break;
  default:
    LOG_MSG_ERROR_1( "Unknown hardware type %x",
                     info->init.config_params.htype);
    ASSERT(0);
    break;
  }
} /* dhcp_clienti_configuration_test */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_SET

Description:
  This function configures the network interface associated with the
  configuration layer handle.

Arguements:
  void * handle - Handle to the configuration layer object.
  dhcp_client_provision * provision - The lease information needed to
    configure the interface

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_configuration_new().
  The handle must have been started by dhcp_clienti_configuration_start().
===========================================================================*/
ps_iface_ioctl_extended_ip_config_ex_type
dhcp_clienti_configuration_set
(
  void * handle,
  dhcp_client_provision * provision
)
{
  dhcp_client_configuration_info * info = handle;
  ps_ip_addr_type ip_addr;
  ps_iface_ioctl_extended_ip_config_ex_type config_change_mask = 0;
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO2_0( "Configuration Set called");
  /* Sanity check parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("Got NULL info");
    return 0;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("Cookie pointer doesn't match %x %x",
                    info->cookie_ptr, info);
    ASSERT(0);
    return 0;
  }
  if (DHCP_CLIENT_CONFIGURATION_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie doesn't match %x %x",
                    info->cookie, DHCP_CLIENT_CONFIGURATION_COOKIE);
    ASSERT(0);
    return 0;
  }
  if ( NULL == provision )
  {
    DATA_ERR_FATAL("NULL Provision");
    return 0;
  }

  /* We only set ip_addr to ps_iface if it has been set
   * in the core. Inform mechanism SHOULD not set the ip_addr */
  if ( 0 != provision->ip_addr )
  {
    ip_addr.type = IPV4_ADDR;
    ip_addr.addr.v4.ps_s_addr = provision->ip_addr;

    if ( 0 != ps_iface_set_v4_addr( info->ps_iface, &ip_addr ) )
    {
      if ( NULL != info->init.status_cb )
      {
        info->init.status_cb( info->init.userdata, DHCP_CLIENT_IP_FAIL );
      }
    }
  }

  if ( TRUE == info->init.config_params.router_option &&
       0 != provision->router )
  {
    LOG_MSG_INFO2_1("Setting Router Addr 0x%x", provision->router);
    info->ps_iface->v4_net_info.gateway.ps_s_addr = provision->router;
  }

  if ( TRUE == info->init.config_params.domain_name_server_option &&
       NULL != provision->dns_servers )
  {
    LOG_MSG_INFO2_1("Setting primary DNS Addr 0x%x",
                    provision->dns_servers->dns_addr);

    /* Check if the dns server addresses are updated or not */
    if( info->ps_iface->v4_net_info.primary_dns.ps_s_addr !=
        provision->dns_servers->dns_addr )
    {
      LOG_MSG_INFO2_0("dhcp_clienti_configuration_set(): "
                      "DNS server address updated, setting in cfg chg msk");
      config_change_mask |= PS_IFACE_EXT_IP_CFG_MASK_DNS_ADDR;
    }
    else
    {
      LOG_MSG_INFO2_0("dhcp_clienti_configuration_set(): "
                      "DNS server address NOT updated, clearing cfg chg msk");
      config_change_mask &= ~PS_IFACE_EXT_IP_CFG_MASK_DNS_ADDR;
    }

    info->ps_iface->v4_net_info.primary_dns.ps_s_addr =
      provision->dns_servers->dns_addr;
    if ( NULL != provision->dns_servers->next )
    {
      /* If primary DNS is unchanged, check for secondary */
      if( (config_change_mask & PS_IFACE_EXT_IP_CFG_MASK_DNS_ADDR) == 0 )
      {
        /* Check if the secondary dns server address is updated or not */
        if( info->ps_iface->v4_net_info.secondary_dns.ps_s_addr !=
              provision->dns_servers->next->dns_addr )
        {
          LOG_MSG_INFO2_0("dhcp_clienti_configuration_set(): "
                          "DNS server address updated, setting in cfg chg msk");
          config_change_mask |= PS_IFACE_EXT_IP_CFG_MASK_DNS_ADDR;
        }
      }
      LOG_MSG_INFO2_1("Setting secondary DNS Addr 0x%x",
                      provision->dns_servers->next->dns_addr);
      info->ps_iface->v4_net_info.secondary_dns.ps_s_addr =
        provision->dns_servers->next->dns_addr;
    }
  }
  if ( TRUE == info->init.config_params.interface_mtu_option &&
       0 != provision->mtu )
  {
    LOG_MSG_INFO2_1("Setting MTU %d", dss_ntohs(provision->mtu) );
    PS_IFACE_SET_MTU(info->ps_iface, dss_ntohs(provision->mtu));
  }
  if ( TRUE == info->init.config_params.subnet_mask_option &&
       0 != provision->ip_mask )
  {
    LOG_MSG_INFO2_1("Setting subnet mask 0x%x", provision->ip_mask);
    info->ps_iface->v4_net_info.net_mask.ps_s_addr = provision->ip_mask;
  }
  /* SIP provisioning */
  if ( TRUE == info->init.config_params.sip_server_option )
  {
    if ( NULL != provision->sip_addrs )
    {
      LOG_MSG_INFO2_0("Setting SIP Address list");
      if ( TRUE != dhcpi_client_configuration_set_sip_addr(info,
            provision->sip_addrs, &config_change_mask) )
      {
        LOG_MSG_ERROR_0("set_sip_addr failed");
      }
    }
    if ( NULL != provision->sip_domain_names )
    {
      LOG_MSG_INFO2_0("Setting SIP Domain Name list");
      if ( TRUE != dhcpi_client_configuration_set_sip_domain_names(info,
            provision->sip_domain_names, &config_change_mask) )
      {
        LOG_MSG_ERROR_0("set_sip_domain_names failed");
      }
    }
  }
  /* NOTE: We gracefully ignore the DNS default suffix until DNS groks it*/

  /* Return the config mask indicating all config items which got updated
     as compared to previously stored values */
  return config_change_mask;
} /* dhcp_clienti_configuration_set */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_CLEAR

Description:
  This function unconfigures (or clears) the network interface associated with
  the configuration layer handle.

Arguements:
  void * handle - Handle to the configuration layer object.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_configuration_new().
  The handle must have been started by dhcp_clienti_configuration_start().
===========================================================================*/
void
dhcp_clienti_configuration_clear
(
  void * handle
)
{
  dhcp_client_configuration_info * info = handle;
  ps_ip_addr_type ip_addr;
  /* Sanity check parameters */
  if (NULL == info)
  {
    DATA_ERR_FATAL("Got NULL info");
    return;
  }
  if (info->cookie_ptr != info)
  {
    LOG_MSG_ERROR_2("Cookie pointer doesn't match %x %x",
                    info->cookie_ptr, info);
    ASSERT(0);
    return;
  }
  if (DHCP_CLIENT_CONFIGURATION_COOKIE != info->cookie)
  {
    LOG_MSG_ERROR_2("Cookie doesn't match %x %x",
                    info->cookie, DHCP_CLIENT_CONFIGURATION_COOKIE);
    ASSERT(0);
    return;
  }

  ip_addr.type = IPV4_ADDR;
  ip_addr.addr.v4.ps_s_addr = 0;

  (void)ps_iface_set_v4_addr( info->ps_iface, &ip_addr );
} /* dhcp_clienti_configuration_clear */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CONFIGURATION_EVENT_IND

Description:
  This function issues an IFACE_EXTENDED_IP_CONFIG event indication
  on the interface associated with the configuration layer handle.

Arguments:
  void    * handle   - Handle to the configuration layer object.
  boolean got_config - whether the DHCP client was successful.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_configuration_new().
  The handle must have been started by dhcp_clienti_configuration_start().
===========================================================================*/
void
dhcp_clienti_configuration_event_ind
(
  void                                      *handle,
  ps_iface_ioctl_extended_ip_config_ex_type  config_change_mask
)
{
  dhcp_client_configuration_info * info =
    (dhcp_client_configuration_info *) handle;
/*-------------------------------------------------------------------------*/

  /* Sanity check parameters */
  if ( NULL == info )
  {
    DATA_ERR_FATAL("Got NULL info");
    return;
  }
  if ( info->cookie_ptr != info )
  {
    LOG_MSG_ERROR_2("Cookie pointer doesn't match %x %x",
                    info->cookie_ptr, info);
    ASSERT(0);
    return;
  }
  if ( DHCP_CLIENT_CONFIGURATION_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2("Cookie doesn't match %x %x",
                    info->cookie, DHCP_CLIENT_CONFIGURATION_COOKIE);
    ASSERT(0);
    return;
  }

  /* No need to send indication if mask value is 0 */
  if( config_change_mask == PS_IFACE_EXT_IP_CFG_MASK_NONE )
  {
    LOG_MSG_INFO1_0( "dhcp_clienti_configuration_event_ind(): "
                     "Config change mask 0, not sending "
                     "IFACE_EXTENDED_IP_CONFIG_EX_EV" );
    return;
  }

  /* Now fire the extended ip config extension event to propagate config
     changed mask */
  LOG_MSG_INFO1_2( "dhcp_clienti_configuration_event_ind(): "
                   "Sending IFACE_EXTENDED_IP_CONFIG_EX_EV to IFACE 0x%x, "
                   "config_change_mask 0x%x", info->ps_iface, 
                                              config_change_mask );
  ps_iface_generic_ind(info->ps_iface,
                       IFACE_EXTENDED_IP_CONFIG_EX_EV,
                       (void*)&config_change_mask);
} /* dhcp_clienti_configuration_event_ind */

