
#ifndef DSHDR_DHCP_MGR_H
#define DSHDR_DHCP_MGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S   D H C P

                                 M A N A G E R

GENERAL DESCRIPTION
  This file contains functions used to handle FEATURE_DS_CORD changes.

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
  dshdr_dhcp_start
    This function triggers the DHCP handle allocation only
  dshdr_stop_dhcp
    This function triggers the DHCP negotiation on the given IFACE

INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

   Copyright (c) 2006 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/dshdr_dhcp_mgr.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/02/12   msh     Coan: Feature cleanup
11/06/09   ms      Decouple DHCP start from DHCP trigger.
06/23/08   sc      Extending dshdr_dhcp6_negotiation_options_type to support
                   new NV items
04/15/08   sc      Changes for MIPv6 bootstrap changes
07/13/07   ssr     Remove unnecessary asserts and make the default
                   negotiations options as none
07/2/07    ssr     Removing compilation error by changing dhcp6_client_status
                   to dhcp_client_status
02/14/07   ssr     New file: contains prototype definitions for code that
                   triggers DHCP negotiation from the mode handler
===========================================================================*/

/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dhcp_client.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "dhcp6_client.h"

/* Negotiation options masks for DHCPv6
 */
#define DEFAULT_SIP_DOMAIN_NAME_LIST_OPTION  0x0
#define DEFAULT_SIP_IPV6_LIST_OPTION         0x0
#define DEFAULT_DNS_SERVERS_OPTION           0x0
#define DEFAULT_DNS_DOMAIN_SEARCHLIST_OPTION 0x0

/*--------------------------------------------------------------
 Config structure for holding DHCPv6 negotiation options
 *-------------------------------------------------------------*/
typedef struct dshdr_dhcp6_negotiation_options
{
  boolean sip_domain_name_list_option;
  boolean sip_ipv6_list_option;
  boolean dns_servers_option;
  boolean dns_domain_searchlist_option;
  boolean is_negotiation_needed;
  uint32  max_inform_req_tries;
  uint32  inform_req_init_retry_interval;
}dshdr_dhcp6_negotiation_options_type;

#endif /* FEATURE_DATA_PS_IPV6 */

/* Negotiation options masks for DHCPv4
 */
#define DEFAULT_DOMAIN_NAME_SERVER_OPTION 0x0
#define DEFAULT_SUBNET_MASK_OPTION 0x0
#define DEFAULT_ROUTER_OPTION 0x0
#define DEFAULT_INTERFACE_MTU_OPTION 0x0
#define DEFAULT_DOMAIN_NAME_OPTION 0x0
#define DEFAULT_SIP_SERVER_OPTION 0x0

/*--------------------------------------------------------------
 Config structure for holding DHCPv4 negotiation options
 *-------------------------------------------------------------*/
typedef struct dshdr_dhcp_negotiation_options
{
  boolean domain_name_server_option;
  boolean subnet_mask_option;
  boolean router_option;
  boolean interface_mtu_option;
  boolean domain_name_option;
  boolean sip_server_option;
  boolean is_negotiation_needed;
}dshdr_dhcp_negotiation_options_type;

/* Negotiation options mask for DHCPv4
*/
#define DOMAIN_NAME_SERVER_OPTION 0x1
#define SUBNET_MASK_OPTION        0x2
#define ROUTER_OPTION             0x4
#define INTERFACE_MTU_OPTION      0x8
#define DOMAIN_NAME_OPTION        0x10
#define SIP_SERVER_OPTION         0x20

/* Negotiation options mask for DHCPv6
*/
#ifdef FEATURE_DATA_PS_IPV6

#define SIP_DOMAIN_NAME_LIST_OPTION  0x1
#define SIP_IPV6_LIST_OPTION         0x2
#define DNS_SERVERS_OPTION           0x4
#define DNS_DOMAIN_SEARCHLIST_OPTION 0x8

#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_DATA_PS_MIPV6
typedef enum
{
  DS_MIP6_BOOTSTRAP_MIN_OPT          = 0,
  DS_MIP6_BOOTSTRAP_IS835_VENDOR_OPT = 0,
  DS_MIP6_BOOTSTRAP_HNID_OPT         = 1,
  DS_MIP6_BOOTSTRAP_MAX_OPT
} ds_mip6_bootstrap_option_e_type;

#define DS_MIP6_MAX_NAI_LEN                      (72)
#define DS_MIP6_BOOTSTRAP_ENTERPRISE_NUM         (5553)

typedef struct
{
  uint8                              nai[DS_MIP6_MAX_NAI_LEN];
  uint8                              nai_len;
  ds_mip6_bootstrap_option_e_type    bootstrap_option;
  dhcp6_mip6_hnid_option_vals        hnid_option_vals;
} ds_1x_mip6_profile_type; // change the name?

#endif /* FEATURE_DATA_PS_MIPV6 */

/*-------------------------------------------------------------------------
  Constant definition for DHCP Unique Client ID. This ID needs to be
  passed during DHCP Client start.

  This is just a dummy number without any significance. The enterprise
  number and identifier are completely random. This has been defined to 
  provide an interim id until we get some requirements from client or make
  this configurable. This is used by the DHCP server to identify the
  client uniquely.

  Value = 02 + Enterprise Number + Identifier
          02 + 0006 + 0C
        
-------------------------------------------------------------------------*/
#define DHCP_CLIENT_DUID_ADDR              0x0200060C

/* DUID Length */
#define DHCP_CLIENT_DUID_ADDR_LEN          16

/*===========================================================================
                                 EXTERNAL FUNCTION DEFINIZTIONS
===========================================================================*/

#ifdef FEATURE_DATA_PS_IPV6

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
);
#endif /* FEATURE_DATA_PS_IPV6 */

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
);

/*===========================================================================
FUNCTION      dshdr_dhcp_client_params_init

DESCRIPTION   This function initializes the DHCP negotiation options structure
              for both v4 and v6

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_dhcp_configure_negotiation_options(void);

/*===========================================================================
FUNCTION      dshdr_dhcp_trigger_negotiation

DESCRIPTION   This function triggers the DHCP negotiation on the given IFACE.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_dhcp_trigger_negotiation
(
  ps_iface_type *this_iface_ptr
);

/*===========================================================================
FUNCTION      dshdr_dhcp_start

DESCRIPTION   This function triggers the DHCP allocation only.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_dhcp_start 
(
  ps_iface_type *this_iface_ptr
);

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION      dshdr_dhcp_iface_prefix_changed_ev_cb

DESCRIPTION   This function is called on when IFACE_PREFIX_UPDATE_EV happens
              on IPv6 IFACE.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_dhcp_iface_prefix_changed_ev_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
);
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION      DSHDR_DHCP_POWERUP_INIT

DESCRIPTION   This function is called on powerup and initializes the DHCP
              module and also take a copy of the NV DHCP config info for
              use during DHCP negotiation

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_dhcp_powerup_init (void);


/*===========================================================================
FUNCTION      dshdr_stop_dhcp

DESCRIPTION   This function triggers the DHCP negotiation on the given IFACE.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dshdr_stop_dhcp (ps_iface_type *this_iface_ptr);

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
);
#endif /* FEATURE_DATA_PS_MIPV6 */
#endif /* DSHDR_DHCP_MGR_H */

