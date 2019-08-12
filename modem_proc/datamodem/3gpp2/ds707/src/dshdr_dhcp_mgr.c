/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S   D H C P

                                 M A N A G E R

GENERAL DESCRIPTION
  This file contains functions used for trigerring DHCP negotiation from the
  mode handler.

EXTERNALIZED FUNCTIONS
  DSHDR_DHCP_POWERUP_INIT
    Initializes the DHCP modules, both v4 and v6, on powerup.
  dshdr_dhcp6_status_cb
    This callback function is called to convey the status of the
    ealier call to dhcp6_client_start()
  dshdr_dhcp_status_cb
    This callback function is called to convey the status of the
    ealier call to dhcp_client_start()
  dshdr_dhcp_iface_prefix_changed_ev_cb
    This function is called on when IFACE_PREFIX_UPDATE_EV happens
    on IPv6 IFACE
  dshdr_dhcp_trigger_negotiation
    This function triggers the DHCP negotiation on the given IFACE
  dshdr_stop_dhcp
    This function triggers the DHCP negotiation on the given IFACE

INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

   Copyright (c) 2006-2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/dshdr_dhcp_mgr.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/14    vm     Stateless DHCPv6 FR changes
06/30/14    niv    Added wrapper functions & API change for 
                   NV_REFRESH 
04/09/14    ash    Added Prefix Delegation support.
06/26/13    sc     Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
06/03/13    sd     ATCoP decoupling changes
01/02/12    msh    Coan: Feature cleanup
03/15/11    mg     Global variable cleanup
02/11/11    ack    Global Variable Cleanup
11/11/09    ms     Lint fixes.
11/06/09    ms     Decouple DHCP start from DHCP trigger.
10/17/08    cs     Lint errors fix.
06/23/08    sc     Added support for two new dhcp6 NV items
04/15/08    sc     Changes for MIPv6 bootstrap changes
04/09/08   sc      Changes to read correct options NV mask for dhcp6 and changes
                   to init the dhcp4 client
07/13/07   ssr     Remove unnecessary asserts and make the default
                   negotiations options as none
02/14/07   ssr     New file: contains code that triggers DHCP negotiation
                   from the mode handler
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include <stringl/stringl.h>

#include "ds_1x_profile.h"
#include "ps_iface.h"
#include "ps_iface_defs.h"
#include "dhcp_client.h"
#include "dhcp6_client.h"
#include "ds707_pkt_mgri.h"
#include "ds3gcfgmgr.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "dshdr_dhcp_mgr.h"
#include "dstaski.h"

#ifdef FEATURE_DATA_PS_MIPV6
/* Global structure to store the mip6 bootstrap info passed by mip6 module */
static ds_1x_mip6_profile_type ds_1x_mip6_profile;
/* Valid flag to check if ds_1x_mip6_profile info is valid or not */
static boolean ds_1x_mip6_profile_valid = FALSE;

#endif /* FEATURE_DATA_PS_MIPV6 */
/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*---------------------------------------------------------------------------------
 * dhcp_config_info - config info for DHCPv4
 * dhcp6_config_info - config info for DHCPv6
 ----------------------------------------------------------------------------------*/
LOCAL dshdr_dhcp_negotiation_options_type dhcp_config_info;
#ifdef FEATURE_DATA_PS_IPV6
LOCAL dshdr_dhcp6_negotiation_options_type dhcp6_config_info;
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/
#ifdef FEATURE_DATA_PS_MIPV6
/*===========================================================================
FUNCTION      dshdr_dhcp6_configure_mip6_bootstrap_info

DESCRIPTION   This funtion configures the DHCP6 client with MIP6 bootstrap
              info only if MIPv6 is enabled

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_dhcp6_configure_mip6_bootstrap_info
(
  uint8 *client_id,
  dhcp6_client_params *dhcp6_params
)
{
  int duid_len = 0;
  /*------------------------------------------------------------------------
    Configure the MIPv6 bootstrap info only if the MIPv6 profile is valid.
  ------------------------------------------------------------------------*/
  if (ds_1x_mip6_profile_valid == TRUE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED,
                    "Configuring DHCP6 client with MIPv6 bootstrap info");

    duid_len = dhcp6_client_duid_en_init (client_id,
                                         DHCP6_CLIENT_MAX_DUID_LENGTH,
                                         DS_MIP6_BOOTSTRAP_ENTERPRISE_NUM,
                                         ds_1x_mip6_profile.nai,
                                           ds_1x_mip6_profile.nai_len);
    if(duid_len == -1)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not set MIPv6 bootstrap client id");
      return;
    }

    dhcp6_params->client_duid_len = (uint32) duid_len;
    dhcp6_params->client_duid = client_id;
    dhcp6_params->client_id_option = TRUE;


    switch(ds_1x_mip6_profile.bootstrap_option)
    {
      case DS_MIP6_BOOTSTRAP_IS835_VENDOR_OPT:
        DATA_IS707_MSG0(MSG_LEGACY_MED,
                        "Using IS835 option to get the bootstrap info");
        dhcp6_params->expect_vendor_opts_bootstrap_info = TRUE;
        break;

      case DS_MIP6_BOOTSTRAP_HNID_OPT:
        DATA_IS707_MSG0(MSG_LEGACY_MED,
                        "Using home network id option to get the bootstrap info");
        dhcp6_params->mip6_home_network_id_option = TRUE;
        (void)memscpy(&(dhcp6_params->mip6_home_network_id_vals),
                      sizeof(dhcp6_mip6_hnid_option_vals),
                      &ds_1x_mip6_profile.hnid_option_vals,
                      sizeof(dhcp6_mip6_hnid_option_vals));
        break;

      default:
        ERR_FATAL("Illegal MIPv6 bootstrap option type", 0, 0, 0);
    }
  } /* ds_1x_mip6_profile_valid */
} /* dshdr_dhcp6_configure_mip6_bootstrap_info */

#endif /*FEATURE_DATA_PS_MIPV6*/
/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      dshdr_dhcp6_status_cb

DESCRIPTION   This callback function is called to convey the status of the
              ealier call to dhcp6_client_start()

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_dhcp6_status_cb
(
  void * userdata,
  dhcp_client_status_enum status
)
{
#ifdef FEATURE_DATA_PS_IPV6
  void *dhcp6_handle = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  dhcp6_handle = ps_iface_get_dhcp_stateless_client_handle((ps_iface_type *)userdata);
  if (status != DHCP_CLIENT_PROVISIONED)
  {
    DATA_IS707_MSG3(MSG_LEGACY_ERROR, "dshdr_dhcp6_status_cb failed. Status:%d handle:%d"
                    ", Stopping DHCPv6 handle:0x%x",
             status, dhcp6_handle,dhcp6_handle);

    /* Call the DHCP Stop API for v6
     */
  
    dshdr_stop_dhcp((ps_iface_type *)userdata);
    return;
  }

  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "dshdr_dhcp6_status_cb success. Status:%d handle:%d",
             status, dhcp6_handle);
#endif /* FEATURE_DATA_PS_IPV6 */

}/* dshdr_dhcp6_status_cb */

/*===========================================================================
FUNCTION      dshdr_dhcp_status_cb

DESCRIPTION   This callback function is called to convey the status of the
              ealier call to dhcp_client_start()

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_dhcp_status_cb
(
  void * userdata,
  dhcp_client_status_enum status
)
{
  void *dhcp_handle = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  dhcp_handle = ps_iface_get_dhcp_stateless_client_handle((ps_iface_type *)userdata);  
  if (status != DHCP_CLIENT_PROVISIONED)
  {
    DATA_IS707_MSG3(MSG_LEGACY_ERROR, "dshdr_dhcp_status_cb failed. Status:%d handle:%d"
                    "Stopping DHCPv4 handle:0x%x",
             status, dhcp_handle,dhcp_handle);

    /* Call the DHCP Stop API for v4
     */
    dshdr_stop_dhcp((ps_iface_type *)userdata);
    return;
  }

  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "dshdr_dhcp_status_cb success. Status:%d handle:%d",
             status, dhcp_handle);
}/* dshdr_dhcp_status_cb */

/*===========================================================================
FUNCTION      dshdr_dhcp_client_params_init

DESCRIPTION   This function initializes the DHCPv4 params structure to suggested
              default values

DEPENDENCIES  None

RETURN VALUE  0 - success, -1 - failure

SIDE EFFECTS  None
===========================================================================*/
int dshdr_dhcp_client_params_init
(
ps_iface_type *this_iface_ptr,
dhcp_client_params *dhcp_params_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* The default values are taken from the structure definition in dhcp_client.h
   */

  if (dhcp_client_params_init(dhcp_params_ptr,
                              NULL,
                              0) < 0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Unable to init DHCP client params");
    return -1;
  }

   /* Modify the client params structure to incorporate the negotiation of the
      required options.*/

  dhcp_params_ptr->domain_name_server_option = dhcp_config_info.domain_name_server_option;
  dhcp_params_ptr->subnet_mask_option = dhcp_config_info.subnet_mask_option;
  dhcp_params_ptr->router_option = dhcp_config_info.router_option;
  dhcp_params_ptr->interface_mtu_option = dhcp_config_info.interface_mtu_option;
  dhcp_params_ptr->domain_name_option = dhcp_config_info.domain_name_option;
  dhcp_params_ptr->sip_server_option = dhcp_config_info.sip_server_option;

  return 0;

}/* dshdr_dhcp_client_params_init */

/*===========================================================================
FUNCTION      dshdr_dhcp_client_params_init

DESCRIPTION   This function initializes the DHCP negotiation options structure
              for both v4 and v6

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_dhcp_configure_negotiation_options()
{
  nv_item_type         ds_nv_item_local;
  nv_stat_enum_type    nv_status;
  uint32               options_mask4 = 0;
  nv_item_type         ds707_nv_item;
#ifdef FEATURE_DATA_PS_IPV6
  uint32               options_mask6 = 0;
#endif /* FEATURE_DATA_PS_IPV6 */
/*---------------------------------------------------------------------------*/

  /* Read the negotiable options from NV item
  */
  nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_DHCP4_OPTIONS_MASK_I, &ds_nv_item_local, ds707_curr_subs_id());
  if(nv_status == NV_DONE_S)
  {
    options_mask4 = ds_nv_item_local.dhcp4_options_mask;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW, "DHCP6 NV negotiation options not written. Using default");
    options_mask4 = (DEFAULT_DOMAIN_NAME_SERVER_OPTION << DOMAIN_NAME_SERVER_OPTION) |
                    (DEFAULT_SUBNET_MASK_OPTION << SUBNET_MASK_OPTION)               |
                    (DEFAULT_ROUTER_OPTION << ROUTER_OPTION)                         |
                    (DEFAULT_INTERFACE_MTU_OPTION << INTERFACE_MTU_OPTION)           |
					/*lint -e506 */
                    (DEFAULT_DOMAIN_NAME_OPTION ? DOMAIN_NAME_OPTION : 0)            |
                    (DEFAULT_SIP_SERVER_OPTION  ? SIP_SERVER_OPTION : 0);
					/*lint -restore */
  }

#ifdef FEATURE_DATA_PS_IPV6
  nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_DHCP6_OPTIONS_MASK_I, &ds_nv_item_local, ds707_curr_subs_id());
  if(nv_status == NV_DONE_S)
  {
    options_mask6 = ds_nv_item_local.dhcp6_options_mask;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW, "DHCP6 NV negotiation options not written. Using default");
    options_mask6 = (DEFAULT_SIP_DOMAIN_NAME_LIST_OPTION << SIP_DOMAIN_NAME_LIST_OPTION) |
                    (DEFAULT_SIP_IPV6_LIST_OPTION << SIP_IPV6_LIST_OPTION)               |
                    (DEFAULT_DNS_SERVERS_OPTION << DNS_SERVERS_OPTION)                   |
                    (DEFAULT_DNS_DOMAIN_SEARCHLIST_OPTION << DNS_DOMAIN_SEARCHLIST_OPTION);
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  /* Configure DHCPv4 negotiation options. If not a MIP CCoA call expected
   * default is to negotiate only the SIP server (PCSCF) address. Otherwise
   * negotiate nothing (DHCP not even triggered in this case).
   */
  if (!dshdr_get_mobile_ip_uses_ccoa())
  {
    dhcp_config_info.domain_name_server_option = (options_mask4 & DOMAIN_NAME_SERVER_OPTION)? TRUE : FALSE;
    dhcp_config_info.subnet_mask_option = (options_mask4 & SUBNET_MASK_OPTION)? TRUE : FALSE;;
    dhcp_config_info.router_option = (options_mask4 & ROUTER_OPTION)? TRUE : FALSE;;
    dhcp_config_info.interface_mtu_option = (options_mask4 & INTERFACE_MTU_OPTION)? TRUE : FALSE;;
    dhcp_config_info.domain_name_option = (options_mask4 & DOMAIN_NAME_OPTION)? TRUE : FALSE;
    dhcp_config_info.sip_server_option = (options_mask4 & SIP_SERVER_OPTION)? TRUE : FALSE;
    dhcp_config_info.is_negotiation_needed = options_mask4 ? TRUE : FALSE;
  }
  else
  {
    dhcp_config_info.domain_name_server_option = FALSE;
    dhcp_config_info.subnet_mask_option = FALSE;
    dhcp_config_info.router_option = FALSE;
    dhcp_config_info.interface_mtu_option = FALSE;
    dhcp_config_info.domain_name_option = FALSE;
    dhcp_config_info.sip_server_option = FALSE;
    dhcp_config_info.is_negotiation_needed = FALSE;
  }

#ifdef FEATURE_DATA_PS_IPV6
  /* Configure DHCPv6 negotiation options from NV. Expected default is to
   * negotiate only SIP server (PCSCF) address.
   */
  dhcp6_config_info.sip_domain_name_list_option = (options_mask6 & SIP_DOMAIN_NAME_LIST_OPTION)? TRUE : FALSE;
  dhcp6_config_info.sip_ipv6_list_option = (options_mask6 & SIP_IPV6_LIST_OPTION)? TRUE : FALSE;
  dhcp6_config_info.dns_servers_option = (options_mask6 & DNS_SERVERS_OPTION)? TRUE : FALSE;
  dhcp6_config_info.dns_domain_searchlist_option = (options_mask6 & DNS_DOMAIN_SEARCHLIST_OPTION)? TRUE : FALSE;
  dhcp6_config_info.is_negotiation_needed = options_mask6 ? TRUE : FALSE;

  nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_DHCP6_INFORM_REQ_TRIES_I, &ds707_nv_item, ds707_curr_subs_id());
  if(nv_status == NV_DONE_S)
  {
    dhcp6_config_info.max_inform_req_tries =
                                     ds707_nv_item.dhcp6_inform_req_tries;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW, "DHCP6 NV inform request tries not written. Using default = 4");
    dhcp6_config_info.max_inform_req_tries = 4;
  }

  nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_DHCP6_INFORM_REQ_INIT_RETRY_INTERVAL_I, 
                              &ds707_nv_item, ds707_curr_subs_id());
  if(nv_status == NV_DONE_S)
  {
    dhcp6_config_info.inform_req_init_retry_interval =
      ds707_nv_item.dhcp6_inform_req_init_retry_interval;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_LOW,
                    "DHCP6 NV inform request init retry interval not written. Using default = 1");
    dhcp6_config_info.inform_req_init_retry_interval = 1;
  }
#endif /* FEATURE_DATA_PS_IPV6 */
}/* dshdr_dhcp_configure_negotiation_options */

/*===========================================================================
FUNCTION      dshdr_dhcp_trigger_negotiation

DESCRIPTION   This function triggers the DHCP negotiation on the given IFACE.

DEPENDENCIES  dshdr_dhcp_start should be called prior to this API.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_dhcp_trigger_negotiation (ps_iface_type *this_iface_ptr)
{
  boolean dhcp_version_4;
  void *dhcp_handle = NULL;
  void *dhcp6_handle = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Find out if this IFACE is v4 or v6
   */
  dhcp_version_4 = ps_iface_addr_family_is_v4(this_iface_ptr);

  if (!dhcp_version_4)
  {
#ifdef FEATURE_DATA_PS_IPV6
 
    dhcp6_handle = ps_iface_get_dhcp_stateless_client_handle(this_iface_ptr);
    if (!dhcp6_handle)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Uninitalized DHCPv6 handle");
      return;
    }
    
    dhcp6_client_inform (dhcp6_handle);

    DATA_IS707_MSG2(MSG_LEGACY_MED, "Starting DHCPv6 negotiation on IPv6 "
                                    "iface : 0x%x DHCPv6 handle:0x%x",
                    this_iface_ptr, dhcp6_handle);
#endif /* FEATURE_DATA_PS_IPV6 */
  }
  else
  {
    dhcp_handle = ps_iface_get_dhcp_stateless_client_handle(this_iface_ptr);
    if (!dhcp_handle)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Uninitialized DHCPv4 handle");
      return;
    }

    dhcp_client_inform (dhcp_handle, ps_iface_get_v4_addr(this_iface_ptr));

    DATA_IS707_MSG2(MSG_LEGACY_MED, "Starting DHCPv4 negotiation on IPv4 "
                                    "iface : 0x%x DHCPv4 handle:0x%x",
                    this_iface_ptr, dhcp_handle);
  }
  return;
}/* dshdr_dhcp_trigger_negotiation */

/*===========================================================================
FUNCTION      dshdr_dhcp_start

DESCRIPTION   This function triggers the DHCP allocation only.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_dhcp_start (ps_iface_type *this_iface_ptr)
{
  boolean dhcp_version_4;
  ds707_pdn_context_cb_type *ds707_pdn_cb_ptr = NULL;
  boolean                   request_dhcp_dns = FALSE;
  boolean                   request_dhcp_sip = FALSE;
  dhcp_client_params        dhcp_params;  
  void                      *dhcp_handle = NULL;
  uint64                    duid_addr;       /* Device Unique ID for DHCP */

#ifdef FEATURE_DATA_PS_IPV6
  dhcp6_client_params dhcp6_params;
  void *dhcp6_handle = NULL;
#endif /* FEATURE_DATA_PS_IPV6 */
  
#ifdef FEATURE_DATA_PS_MIPV6
  uint8 dhcp6_clientid[DHCP6_CLIENT_MAX_DUID_LENGTH];
#endif /* FEATURE_DATA_PS_MIPV6 */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Find out if this IFACE is v4 or v6
   */
  dhcp_version_4 = ps_iface_addr_family_is_v4(this_iface_ptr);

  /* Get PDN_CNTX_CB_PTR from iface
   */
  ds707_pdn_cb_ptr = ds707_pdncntx_get_pdncntx_cb_ptr_from_iface_ptr(this_iface_ptr);
  if ( ds707_pdn_cb_ptr )
  {
    request_dhcp_dns = ds707_pdn_cb_ptr->data_profile.iface_profile_info.current_data_session_profile.dns_addr_using_dhcp;
    request_dhcp_sip = ds707_pdn_cb_ptr->data_profile.iface_profile_info.current_data_session_profile.pcscf_addr_using_dhcp;
  }
  if (!dhcp_version_4)
  {
#ifdef FEATURE_DATA_PS_IPV6
    if ((dhcp6_config_info.is_negotiation_needed == FALSE) &&
        (request_dhcp_dns == FALSE) &&
        (request_dhcp_sip == FALSE))
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "Not triggering DHCP6");
      return;
    }
    /* Assign the predefined value for DUID */
    duid_addr = DHCP_CLIENT_DUID_ADDR;
	
    /* Form the parameters structure to be passed in when calling DHCP start
     */
    if (dhcp6_client_params_init(&dhcp6_params,
                                 DHCP6_CLIENT_MODE_STATELESS,
                                 &duid_addr, 
                                 DHCP_CLIENT_DUID_ADDR_LEN) < 0)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Unable to init DHCP6 client params, not triggering DHCP6");
      return;
    }

    /* Modify the client params structure to incorporate the negotiation of the
     * required options. Do not negotiate any option if it is a MIP CCOA call.
     */
    dhcp6_params.sip_ipv6_list_option         = \
                       dhcp6_config_info.sip_ipv6_list_option;
    dhcp6_params.sip_domain_name_list_option  = \
                       dhcp6_config_info.sip_domain_name_list_option;
    dhcp6_params.dns_servers_option           = \
                       dhcp6_config_info.dns_servers_option;
    dhcp6_params.dns_domain_searchlist_option = \
                       dhcp6_config_info.dns_domain_searchlist_option;

    dhcp6_params.informationrequest_tries =
                       dhcp6_config_info.max_inform_req_tries;
    dhcp6_params.informationrequest_irt =
                       dhcp6_config_info.inform_req_init_retry_interval;

    DATA_IS707_MSG0(MSG_LEGACY_MED, "Configuring DHCPv6 options from data session profile");
    
    dhcp6_params.sip_ipv6_list_option = request_dhcp_sip;
    dhcp6_params.sip_domain_name_list_option = request_dhcp_sip;
    dhcp6_params.dns_servers_option   = request_dhcp_dns;
    dhcp6_params.dns_domain_searchlist_option = request_dhcp_dns;

    DATA_IS707_MSG2(MSG_LEGACY_MED, "sip requested %d, dns requested %d", request_dhcp_sip, request_dhcp_dns);

    /* Get the MIPv6 bootstrap information */
#ifdef FEATURE_DATA_PS_MIPV6
      /*----------------------------------------------------------------------
        retrieve mip6 bootstrap params from mip6 module
      ----------------------------------------------------------------------*/
      dshdr_dhcp6_configure_mip6_bootstrap_info(dhcp6_clientid, &dhcp6_params);
#endif /* FEATURE_DATA_PS_MIPV6 */

    /* If the DHCP handle for this iface is not NULL, it means that an inform 
     * request is already pending, so stop the current DHCP instance and call 
     * start again to get a new instance
     */
    if (ps_iface_get_dhcp_stateless_client_handle(this_iface_ptr))
    {
      DATA_IS707_MSG1(MSG_LEGACY_MED, "Current DHCPv6 handle 0x%x not NULL. Stopping it", dhcp6_handle);
      dshdr_stop_dhcp(this_iface_ptr);
      return;
    }
    /* Call DHCP start API
     */
    dhcp6_handle = dhcp6_client_start(PS_IFACE_GET_ID(this_iface_ptr),
                                      &dhcp6_params,
                                      dshdr_dhcp6_status_cb,
                                      (void *)this_iface_ptr);

    if (!dhcp6_handle)
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "dhcp6_client_start returned NULL");
      return;
    }

    /* Set DHCP handle in iface
     */
    if (-1 == ps_iface_set_dhcp_stateless_client_handle(this_iface_ptr, dhcp6_handle))
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,"ps_iface_set_dhcp_handle failed");
      return;
    }
    else
    {
      DATA_IS707_MSG2(MSG_LEGACY_MED,"Set DHCP6 handle 0x%p in iface 0x%p",
                      dhcp6_handle, this_iface_ptr);
    }

#endif /* FEATURE_DATA_PS_IPV6 */
  }
  else
  {
    if ((dhcp_config_info.is_negotiation_needed == FALSE) &&
        (request_dhcp_dns == FALSE) &&
        (request_dhcp_sip == FALSE))
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "Not triggering DHCP4");
      return;
    }
    /* Form the parameters structure to be passed in when calling DHCP start.
     * This is our own API. Just using recommended default values.
     */
    if (dshdr_dhcp_client_params_init(this_iface_ptr, &dhcp_params) < 0 )
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Unable to init DHCP client params, not triggering DHCP4");
      return;
    }

    DATA_IS707_MSG0(MSG_LEGACY_MED, "Configuring DHCPv4 options from data session profile");
    
    dhcp_params.domain_name_server_option = request_dhcp_dns;
    dhcp_params.domain_name_option = request_dhcp_dns;
    dhcp_params.sip_server_option = request_dhcp_sip;

    DATA_IS707_MSG2(MSG_LEGACY_MED, "sip requested %d, dns requested %d", request_dhcp_sip, request_dhcp_dns);

    /* If the DHCP handle for this iface is not NULL, it means that an inform 
     * request is already pending, so stop the current DHCP instance and call 
     * start again to get a new instance
     */
    if (ps_iface_get_dhcp_stateless_client_handle(this_iface_ptr))
    {
      DATA_IS707_MSG1(MSG_LEGACY_MED, "Current DHCPv4 handle 0x%x not NULL. Stopping it", dhcp_handle);
      dshdr_stop_dhcp(this_iface_ptr);
      return;
    }

    /* Trigger DHCP only if it is not a MIP CCoA call
     */
    dhcp_handle = dhcp_client_start(PS_IFACE_GET_ID(this_iface_ptr), 
                                                    &dhcp_params,
                                                    dshdr_dhcp_status_cb, 
                                                    (void *)this_iface_ptr);

    if (!dhcp_handle)
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "dhcp_client_start returned NULL");
      return;
    }

    /* Set DHCP handle in iface
     */
    if (-1 == ps_iface_set_dhcp_stateless_client_handle(this_iface_ptr, dhcp_handle))
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,"ps_iface_set_dhcp_handle failed");
      return;
    }
    else
    {
      DATA_IS707_MSG2(MSG_LEGACY_MED,"Set DHCP4 handle 0x%p in iface 0x%p",
                      dhcp_handle, this_iface_ptr);
    }

  }
  return;
}/* dshdr_dhcp_start */


/*===========================================================================
FUNCTION      DSHDR_DHCP_POWERUP_INIT

DESCRIPTION   This function is called on powerup and initializes the DHCP
              module and also take a copy of the NV DHCP config info for
              use during DHCP negotiation

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_dhcp_powerup_init (void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Configure the DHCP options to be negotiated for both v4 and v6.
  */
  dshdr_dhcp_configure_negotiation_options();

} /* dshdr_dhcp_powerup_init() */

/*===========================================================================
FUNCTION      dshdr_stop_dhcp

DESCRIPTION   This function triggers the DHCP negotiation on the given IFACE.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_stop_dhcp (ps_iface_type *this_iface_ptr)
{
boolean dhcp_version_4;
void *dhcp_handle = NULL;
void *dhcp6_handle = NULL;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Find out if this IFACE is v4 or v6 */
  dhcp_version_4 = ps_iface_addr_family_is_v4(this_iface_ptr);

  if (!dhcp_version_4)
  {
#ifdef FEATURE_DATA_PS_IPV6
    dhcp6_handle = ps_iface_get_dhcp_stateless_client_handle(this_iface_ptr);

    if (dhcp6_handle != NULL)
    {
      DATA_IS707_MSG1(MSG_LEGACY_MED, "Stopping DHCPv6 handle:0x%x", dhcp6_handle);
      dhcp6_client_free(&dhcp6_handle);
    }
#endif /* FEATURE_DATA_PS_IPV6 */
  }
  else
  {
    /* Call the DHCP Stop API for v4
     */
    dhcp_handle = ps_iface_get_dhcp_stateless_client_handle(this_iface_ptr);
	
    if (dhcp_handle != NULL)
    {
      DATA_IS707_MSG1(MSG_LEGACY_MED, "Stopping DHCPv4 handle:0x%x", dhcp_handle);
      dhcp_client_stop(&dhcp_handle);
    }
  }

  /* Reset DHCP handle in iface
   */
  if (-1 == ps_iface_set_dhcp_stateless_client_handle(this_iface_ptr, NULL))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"ps_iface_set_dhcp_handle failed");
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED,"Rst DHCP handle in iface 0x%p", 
                    this_iface_ptr);
  }

}/* dshdr_stop_dhcp */
#ifdef FEATURE_DATA_PS_MIPV6
/*===========================================================================
FUNCTION      dshdr_dhcp_set_mip6_profile

DESCRIPTION   This function sets the mip6 profile for the bootstrap info.

DEPENDENCIES  None

RETURN VALUE  0 - success
              -1 - failure

SIDE EFFECTS  None
===========================================================================*/
int dshdr_dhcp_set_mip6_profile
(
  ds_1x_mip6_profile_type *mip6_profile_ptr
)
{

  if (mip6_profile_ptr == NULL ||
      mip6_profile_ptr->bootstrap_option >= DS_MIP6_BOOTSTRAP_MAX_OPT)
  {
    return -1;
  }

  if ((mip6_profile_ptr->nai_len > 0) &&
      (mip6_profile_ptr->nai_len <= DS_MIP6_MAX_NAI_LEN))
  {
    memscpy(ds_1x_mip6_profile.nai,
            DS_MIP6_MAX_NAI_LEN,
            mip6_profile_ptr->nai,
            mip6_profile_ptr->nai_len);
    ds_1x_mip6_profile.nai_len = mip6_profile_ptr->nai_len;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "MIP6 NAI length not correct");
    return -1;
  }

  ds_1x_mip6_profile.bootstrap_option = mip6_profile_ptr->bootstrap_option;

  memscpy(&(ds_1x_mip6_profile.hnid_option_vals),
          sizeof(dhcp6_mip6_hnid_option_vals),
          &(mip6_profile_ptr->hnid_option_vals),
          sizeof(dhcp6_mip6_hnid_option_vals));

  ds_1x_mip6_profile_valid = TRUE;
  /*-------------------------------------------------------------------------
    setting up the is_negotiation_needed flag to TRUE. This will trigger
    DHCP6 negotiations even if the DHCP6 options mask NV is not set
  -------------------------------------------------------------------------*/
  dhcp6_config_info.is_negotiation_needed = TRUE;

  return 0;

}
#endif /* FEATURE_DATA_PS_MIPV6 */
