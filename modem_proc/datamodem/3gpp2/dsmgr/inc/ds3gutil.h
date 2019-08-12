#ifndef DS3GUTIL_H
#define DS3GUTIL_H
/*===========================================================================

                      D S 3 G U T I L . H
 
GENERAL DESCRIPTION
  Utility functions for DS module

Copyright (c) 2004-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/ds3gutil.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/15/13    sgd    Add ds3g_util_init to register callback functions

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "ps_sys.h"
#include "ds_dsd_ext_i.h"
#include "ds3gmgr.h"
#include "nv_items.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*-------------------------------------------------------------------------
  PAP and CHAP definitions used by 3GPP and 3GPP2 MH
-------------------------------------------------------------------------*/
#define DS3G_PDN_AUTH_PAP   0x0
#define DS3G_PDN_AUTH_CHAP  0x1

/*-------------------------------------------------------------------------
  Structure for IPv6 IID type
-------------------------------------------------------------------------*/
typedef struct
{
  byte    iid_enum_type;
  uint64  iid_val;
}ds3g_ipv6_iid_type;

/*===========================================================================
FUNCTION      DS3G_UTIL_INIT

DESCRIPTION   This function is used to initialize the ds3g util module 
              during power up 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_util_init( void );

/*===========================================================================
FUNCTION      ds3g_util_read_nv

DESCRIPTION   This function reads all the ds3g specific NV items
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_util_read_nv( void );

/*===========================================================================
FUNCTION      DS3G_PROCESS_MCFG_REFRESH_IND

DESCRIPTION   This function process MCFG refresh ind and re-reads all ds3g/dsd 
              NV items.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_process_mcfg_refresh_ind
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_SYS_SYS_MODE_TO_DS_APN_SYS_MODE

DESCRIPTION   This function is a utility function to convert cm sys sys mode 
              to ds apn pkt sys mode.
  
DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
ds_pkt_sys_mode_e_type ds3g_sys_sys_mode_to_ds_apn_sys_mode
(
  sys_sys_mode_e_type  sys_sys_mode
);

/*===========================================================================
FUNCTION      DS3G_GET_APN_PREF_PKT_MODE

DESCRIPTION   This function returns the currently preferred mode for packet 
              data calls on the specified PDN when MPPM is enabled.

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
ds_pkt_sys_mode_e_type ds3g_get_apn_pref_pkt_mode
(
  ds3g_apn_name_type  *apn_name
);

/*===========================================================================
FUNCTION      DS3G_GET_APN_PREF_PKT_MODE_EX

DESCRIPTION   This function returns the currently preferred mode for packet 
              data calls on the specified PDN when MPPM is enabled on
              passed sub.

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
ds_pkt_sys_mode_e_type ds3g_get_apn_pref_pkt_mode_ex
( 
   ds3g_apn_name_type      *apn_name,
   sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION      DS3G_IS_APN_PREF_PKT_MODE

DESCRIPTION   This function determines for a PDN if the specified mode is the 
              currently preferred mode for packet data calls.
  
DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_apn_pref_pkt_mode
( 
  ds_pkt_sys_mode_e_type  mode,
  ds3g_apn_name_type     *apn_name
);

/*===========================================================================
FUNCTION      DS3G_IS_APN_PREF_PKT_MODE_EX

DESCRIPTION   This function determines for a PDN if the specified mode is the 
              currently preferred mode for packet data calls for passed sub.

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_apn_pref_pkt_mode_ex
( 
  ds_pkt_sys_mode_e_type  mode,
  ds3g_apn_name_type     *apn_name,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_PREF_SYS_MODE_FOR_PACKET_ORIG

DESCRIPTION   This function determines the currently preferred mode for packet
              data calls.

DEPENDENCIES  None

RETURN VALUE  sys_sys_mode_e_type: sys mode

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_get_pref_sys_mode_for_packet_orig( void );

/*===========================================================================
FUNCTION      DS3G_IS_DORM_ALLOWED_IN_PKT_MODE

DESCRIPTION   This function determines if dormancy is allowed in the currently 
              preferred mode for packet data calls.

DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_dorm_allowed_in_pkt_mode
(
  sys_sys_mode_e_type  mode
);

/*===========================================================================
FUNCTION      DS3G_IS_PREF_CS_DOMAIN_MODE

DESCRIPTION   This function determines if the specified mode is the currently 
              preferred mode for CS data calls.
  
DEPENDENCIES  None

RETURN VALUE  TRUE or FALSE

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_pref_cs_domain_mode
(
  sys_sys_mode_e_type  mode
);

/*===========================================================================
FUNCTION       DS3G_SET_CM_OPRT_LPM_MODE

DESCRIPTION    Set CM operation mode to LPM

DEPENDENCIES   None

RETURN VALUE   TRUE: Successfully set 
               FALSE: Fail to set 

SIDE EFFECTS   None

===========================================================================*/
boolean ds3g_set_cm_oprt_lpm_mode( void );

/*===========================================================================
FUNCTION       DS3G_SET_CM_OPRT_ONLINE_MODE

DESCRIPTION    Set CM operation mode to online

DEPENDENCIES   None

RETURN VALUE   TRUE: Successfully set 
               FALSE: Fail to set 

SIDE EFFECTS   None

===========================================================================*/
boolean ds3g_set_cm_oprt_online_mode( void );

/*===========================================================================
FUNCTION      DS3G_GET_IPV6_SM_CONFIG_INFO

DESCRIPTION   Get IPv6 SM config info configured through NV item

DEPENDENCIES  None

RETURN VALUE  ip6_sm_config_type* - IPv6 SM config info

SIDE EFFECTS  None
===========================================================================*/
ip6_sm_config_type* ds3g_get_ipv6_sm_config_info
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_IPV6_PRIMARY_DNS_ADDR

DESCRIPTION   Get IPv6 primary DNS server address

DEPENDENCIES  None

RETURN VALUE  ps_in6_addr* - Ptr containing the primary DNS server addr

SIDE EFFECTS  None
===========================================================================*/
struct ps_in6_addr* ds3g_get_ipv6_primary_dns_addr
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_IPV6_SECONDARY_DNS_ADDR

DESCRIPTION   Get IPv6 secondary DNS server address

DEPENDENCIES  None

RETURN VALUE  ps_in6_addr* - Ptr containing the secondary DNS server addr

SIDE EFFECTS  None
===========================================================================*/

struct ps_in6_addr* ds3g_get_ipv6_secondary_dns_addr
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_IS_IPV6_ENABLED

DESCRIPTION   Returns whether IPv6 is enabled through NV or not.

DEPENDENCIES  None

RETURN VALUE  TRUE: IPv6 enabled
              FALSE: IPv6 disabled

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_ipv6_enabled( void );

/*===========================================================================
FUNCTION      DS3G_IS_PDSN_AS_PROXY_IPv6_DNS_SERVER

DESCRIPTION   Returns whether PDSN acts as proxy IPv6 DNS server

DEPENDENCIES  None

RETURN VALUE  Boolean

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_pdsn_as_proxy_ipv6_dns_server
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_FAILOVER_MODE

DESCRIPTION   Returns the failover mode set in NV

DEPENDENCIES  None

RETURN VALUE  Boolean

SIDE EFFECTS  None
===========================================================================*/
ps_iface_ip_ver_failover_e_type ds3g_get_failover_mode( void );

/*===========================================================================
FUNCTION      DS3G_GET_IPV6_IID_INFO

DESCRIPTION   Returns IPv6 iid info

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_get_ipv6_iid_info
(
  sys_modem_as_id_e_type  subs_id,
  ds3g_ipv6_iid_type     *iid_info_p
);

/*===========================================================================
FUNCTION      DS3G_READ_EHRPD_AUTH_IN_USIM

DESCRIPTION   Returns ehrpd auth in usim NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_ehrpd_auth_in_usim( void );

#ifdef FEATURE_DATA_MUX_USB_TEST
/*===========================================================================
FUNCTION      DS3G_IS_MUX_NV_ITEM_SET

DESCRIPTION   This function returns true if NV item 6873 is set to MUX ports 

DEPENDENCIES  None

RETURN VALUE  TRUE  - true if nv item has mux port 
              FALSE - false if NV item doesnt have mux ports

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_is_mux_nv_item_set( void );
#endif /* FEATURE_DATA_MUX_USB_TEST */

/*===========================================================================
FUNCTION      DS3G_GET_NV_MODE_PREF

DESCRIPTION   This function returns mode preference NV value
 
DEPENDENCIES  None

RETURN VALUE  NV Mode pref

SIDE EFFECTS  None
===========================================================================*/
nv_mode_enum_type ds3g_get_nv_mode_pref
(
  sys_modem_as_id_e_type  subs_id
);

#ifdef FEATURE_8960_SGLTE_FUSION
/*===========================================================================
FUNCTION      DS3G_GET_DISABLE_DSD_SYNC

DESCRIPTION   Returns Disable DSD sync NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_disable_dsd_sync( void );
#endif /* FEATURE_8960_SGLTE_FUSION */

#ifdef FEATURE_DATA_SDIO_MUX_A2_DUN
/*===========================================================================
FUNCTION      DS3G_GET_MDM_TETHERING_ENABLED

DESCRIPTION   Returns MDM tethering enabled NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_mdm_tethering_enabled( void );
#endif /* FEATURE_DATA_SDIO_MUX_A2_DUN */

/*===========================================================================
FUNCTION      DS3G_GET_DUN_ACC_CFG

DESCRIPTION   Returns DUN acc cfg NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
uint8 ds3g_get_dun_acc_cfg( void );

/*===========================================================================
FUNCTION      DS3G_GET_REFRESH_VOTE_OK_INFO

DESCRIPTION   Returns refresh vote ok NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_refresh_vote_ok_info
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_CONFIG_PDN_LEVEL_AUTH

DESCRIPTION   Returns config_pdn_level_auth NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
uint8 ds3g_get_config_pdn_level_auth
(
  sys_modem_as_id_e_type  subs_id
);

#ifdef FEATURE_DATA_IS707
/*===========================================================================
FUNCTION      DS3G_GET_AUTH_INTERNAL_NV

DESCRIPTION   Returns Auth internal NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_auth_internal_nv( void );
#endif /* FEATURE_DATA_IS707 */

/*===========================================================================
FUNCTION      DS3G_GET_ENABLE_ARBITRATION

DESCRIPTION   Returns enable arbitartion NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_enable_arbitration( void );

/*===========================================================================
FUNCTION      DS3G_GET_SUPPRESS_GSM_ON_SRVCC_CSFB

DESCRIPTION   Returns Suppress GSM on SRVCC/CSFB NV item

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_suppress_gsm_on_srvcc_csfb
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION      DS3G_GET_CONFIG_LATENCY_INFO

DESCRIPTION   Returns config latency info

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE - return Latency Info nv value

SIDE EFFECTS  None
===========================================================================*/
boolean ds3g_get_config_latency_info(void);
/*===========================================================================
FUNCTION      DS3G_PKT_SYS_MODE_TO_SYS_SYS_MODE

DESCRIPTION   This function is a utility function to convert ds apn pkt 
              sys mode to sys sys mode.

DEPENDENCIES  None

RETURN VALUE  Mapped SYS mode : sys_sys_mode_e_type

SIDE EFFECTS  None
===========================================================================*/
sys_sys_mode_e_type ds3g_pkt_sys_mode_to_sys_sys_mode
(
  ds_pkt_sys_mode_e_type  ds_pkt_mode
);

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION      DS3G_GET_DIV_DUPLEX_FROM_SO_MASK

DESCRIPTION   Utility function to get LTE div duplex from so mask.

DEPENDENCIES  None.

RETURN VALUE  The div duplex for LTE

SIDE EFFECTS  None.
===========================================================================*/
sys_div_duplex_e_type ds3g_get_div_duplex_from_so_mask
(
  uint32  so_mask
);

/*===========================================================================
FUNCTION      DS3G_GET_SO_MASK_FROM_DIV_DUPLEX

DESCRIPTION   Utility function to get so mask from div duplex.

DEPENDENCIES  None.

RETURN VALUE  The so mask for LTE

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds3g_get_so_mask_from_div_duplex
(
  sys_div_duplex_e_type  div_duplex
);
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION      DS3G_CONVERT_DSD_RADIO_INFO

DESCRIPTION   This is the function to convert dsd radio info to ps status

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_convert_dsd_radio_info
(
  dsd_radio_type                   radio_idx,
  ds_dsd_ext_radio_info_type      *dsd_radio_info,
  ps_sys_subscription_enum_type    ps_subs_id,
  ps_sys_system_status_info_type  *dsd_sys_info
);

/*===========================================================================
FUNCTION      DS3G_UPDATE_DATA_CONFIG_INFO

DESCRIPTION   Puts all the required nv item file paths in the 
              data_config_info.conf file. Would be called during powerup.
 
DEPENDENCIES  None

RETURN VALUE  length of string added to buffer for success. 
              -1 for failure.

SIDE EFFECTS  None
===========================================================================*/
int32 ds3g_update_data_config_info
(
  char   *file_paths_buf,
  uint32  efs_conf_file_size
);

/*===========================================================================
FUNCTION      DS3G_GET_EFS_FILE_PATH_SIZE

DESCRIPTION   Returns the total ds3g EFS item file paths size

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
uint32 ds3g_get_efs_file_path_size( void );

/*===========================================================================
FUNCTION DS3G_IS_EPDG_3G_IP_CONTINUITY_ENABLED

DESCRIPTION
  This functions tell whether EPDG-3G IP Continuity is enabled or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE, EPDG-3G IP Continuity is enabled
  FALSE, otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_is_epdg_3g_ip_continuity_enabled
(
  ps_sys_subscription_enum_type    ps_subs_id
);

#endif /* DS3GUTIL_H */