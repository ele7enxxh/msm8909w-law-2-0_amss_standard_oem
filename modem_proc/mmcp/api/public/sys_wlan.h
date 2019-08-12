#ifndef SYS_WLAN_H
#define SYS_WLAN_H

/*===========================================================================

              WLAN S Y S T E M   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions that are shared between Call Manager,
  Call Manager clients and the WLAN protocol stacks.

Copyright (c) 2004 - 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/sys_wlan.h#1 $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
06/18/10   xs     Add SENTINEL metacomments for maximum value sentinel in the enum 
04/06/09   sv     Updated WPS status enum.
02/23/09   rm     CMI Phase-2 : Removing inclusion of private header files
                  from public
11/28/08   rm     CMI defeaturisation
09/04/08   sv     Added WPS data-structures.
12/07/07   ra     Added support for IMS VCC in HOMER files 
06/12/07   tbh    Fixed ADS1.2 compiler warning: unsigned comparison with 0
06/08/07   tbh    Added new WLAN security enum & a misc structure with all the
                  wlan params which SD should pass blindly to WLAN as a part of
                  existing system record structure 
03/09/07   sk     RAM usage reduction.
11/07/06   sk     Memory reduction effort.
08/29/06   sk     Added SYS_WLAN_BSS_TYPE_ADHOC_START.
10/08/05   ic     Added Header: field 
05/11/05   ic     Added metacomments under FEATURE_HTORPC_METACOMMENTS
10/28/04   dk     Initial version
===========================================================================*/
/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
#include "comdef.h"    /* Common definitions such as byte, uint16, etc. */
#include "sys.h"

/*---------------------------------------------------------------------------
   General macros
---------------------------------------------------------------------------*/

/* No technology selected 
*/
#define SYS_TECH_MASK_EMPTY             0

/* Acquire wlan 80211a systems only.
*/
#define SYS_TECH_PREF_MASK_80211A       SYS_BM_32BIT(SYS_TECH_WLAN_80211A)

/* Acquire wlan 80211b systems only.
*/
#define SYS_TECH_PREF_MASK_80211B       SYS_BM_32BIT(SYS_TECH_WLAN_80211B)

/* Acquire wlan 80211g systems only.
*/
#define SYS_TECH_PREF_MASK_80211G       SYS_BM_32BIT(SYS_TECH_WLAN_80211G)

/* Add new technology/versions here.
*/

/* Acquire any WLAN system
*/
#define SYS_TECH_PREF_MASK_ANY         SYS_BM_32BIT(SYS_TECH_RESERVED)

/* No Change in WLAN system preference
*/
#define SYS_TECH_PREF_MASK_NO_CHANGE   (sys_tech_mask_type)(SYS_BM_32BIT(SYS_TECH_MASK_ANY)+1)


/*------------------------------------------------------------------------------
   System Identifier Enums
------------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*
** Enumeration of technology type mask.
** It converts bit position of sys_tech_e_type to a bit mask.
** To keep numbering scheme consistent,
** use sys_tech_e_type instead of numbers directly.
**
*/
/*
** Technology type bit mask data type. It is a combination of enumeration of
** system band class mask.
*/

typedef uint32 sys_tech_mask_type;

/* Enumeration of WLAN Scan preference.
*/
typedef enum
{

  /* FOR INTERNAL USE OF CM ONLY!.
  */
  SYS_WLAN_SCAN_PREF_NONE               = -1,

  /* Active WLAN Scan.
  */
  SYS_WLAN_SCAN_PREF_ACTIVE              = 0,

  /* Passive WLAN Scan.
  */
  SYS_WLAN_SCAN_PREF_PASSIVE             = 1,

  /* WLAN Scanning to be decided automatically.
  */
  SYS_WLAN_SCAN_PREF_AUTO                = 2,

  /* FOR INTERNAL USE OF CM ONLY!.
  */
  SYS_WLAN_SCAN_PREF_MAX

} sys_wlan_scan_pref_e_type;
/*~ SENTINEL sys_wlan_scan_pref_e_type.SYS_WLAN_SCAN_PREF_MAX */

/* Enumeration of WLAN Rates.
*/
typedef enum {

  /* FOR INTERNAL USE ONLY!.
  */
  SYS_WLAN_RATE_NONE = -1,

  /* 1 Mbps.
  */
  SYS_WLAN_RATE_1_MBPS,

  /* 2 Mbps.
  */
  SYS_WLAN_RATE_2_MBPS,

  /* 5.5 Mbps.
  */
  SYS_WLAN_RATE_5_5_MBPS,

  /* 11 Mbps.
  */
  SYS_WLAN_RATE_11_MBPS,

  /* FOR INTERNAL USE ONLY!.
  */
  SYS_WLAN_RATE_MAX

} sys_wlan_rate_e_type;
/*~ SENTINEL sys_wlan_rate_e_type.SYS_WLAN_RATE_MAX */

typedef enum{
  /* For Internal Use Only! */
  SYS_WLAN_SEC_TYPE_NONE               = 0,
    
  SYS_WLAN_SEC_TYPE_LEGACY             = 1,

  SYS_WLAN_SEC_TYPE_WPA_OR_WPA2        = 2,

  SYS_WLAN_SEC_TYPE_WPA2_ONLY          = 3,

  SYS_WLAN_SEC_TYPE_MAX

} sys_wlan_sec_mode_e_type;
/*~ SENTINEL sys_wlan_sec_mode_e_type.SYS_WLAN_SEC_TYPE_MAX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**
** Enumeration for WLAN link quality
*/
typedef enum sys_wlan_link_qual_e {

  SYS_WLAN_LINK_QUAL_NONE = -1,
  /**< @internal */

  SYS_WLAN_LINK_QUAL_POOR,
  /**< POOR WLAN link quality  */

  SYS_WLAN_LINK_QUAL_GOOD,
 /**< GOOD WLAN link quality  */

  SYS_WLAN_LINK_QUAL_VERY_GOOD,
 /**< VERY GOOD WLAN link quality  */

  SYS_WLAN_LINK_QUAL_EXCELLENT,
 /**< EXCELLENT WLAN link quality  */

  SYS_WLAN_LINK_QUAL_MAX
 /**< @internal */

} sys_wlan_link_qual_e_type;
/*~ SENTINEL sys_wlan_link_qual_e_type.SYS_WLAN_LINK_QUAL_MAX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* WLAN Statistics
*/
typedef struct sys_wlan_stats_s
{
  sys_wlan_rate_e_type    current_xmit_rate;
  /* Xmit rate of the last packet successfully transmitted */

  uint32                  total_tx_bytes;
  /* Number of bytes transmitted over the WLAN interface  */

  uint32                  total_rx_bytes;
  /* Number of bytes received over the WLAN interface     */
  
  sys_wlan_link_qual_e_type wlan_link_quality;
  /**< Link quality reported by WLAN */

} sys_wlan_stats_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* WLAN Record with all the fields which SD doesn't care about & are passed 
 * down by SD to WLAN as it is
 */
typedef struct {
  
  /* Whether Handoff is permitted on the current SSID */
  boolean                                          is_handoff_permitted;

  /* Whether the current SSID is hidden */
  boolean                                          is_hidden_ssid;

  /* WLAN security mode such as LEGACY, WPA, WPA2 */
  sys_wlan_sec_mode_e_type                         wlan_sec_mode;

} sys_wlan_misc_params_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** WLAN system record using sys_* types.
*/
typedef struct sys_wlan_sys_rec_s {

  sys_tech_mask_type                               tech;
  /**< WLAN Technology */

  sys_band_mask_type                               band;
  /**< WLAN Bands */

  sys_chan_type                                    chan;
  /**< WLAN channels to acquire.
  ** Channel 0 means any channel.
  */

  word                                             groups;
  /**< Group this record belongs to. */

  sys_wlan_bss_e_type                              mode;
  /**< WLAN BSS type such as AD-HOC,INFRA, etc. */

  sys_profile_id_type                              profile_id;
  /**< Profile Id */

  sys_wlan_ssid_s_type                             ssid;
  /**< SSID */

  sys_wlan_misc_params_s_type                      wlan_misc_params;
  /**< Misc fields relevant for the WLAN stack. */

} sys_wlan_sys_rec_s_type;

/*===========================================================================

              W L A N   W P S   S Y S T E M   I D E N T I F I E R

============================================================================*/

/** Maximum size of WPS device password.
*/
#define SYS_WLAN_WPS_DEV_PASSWD_LEN_MAX (uint16)(64)

/** Maximum number of credentials from a WPS procedure.
*/
#define SYS_WLAN_WPS_CREDENTIAL_COUNT_MAX (uint8)(5)

/** WPS methods.
*/
typedef enum {

  SYS_WLAN_WPS_MODE_NONE              = -1,
  /**< @internal None.
  */

  SYS_WLAN_WPS_MODE_MANUAL,
  /**< WPS procedure requiring user to select SSID.
  */

  SYS_WLAN_WPS_MODE_AUTO,
  /**< WPS procedure not requiring user to select SSID.
  */

  SYS_WLAN_WPS_MODE_MAX
  /**< @internal Maximum.
  */

} sys_wlan_wps_mode_e_type;
/*~ SENTINEL sys_wlan_wps_mode_e_type.SYS_WLAN_WPS_MODE_MAX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Various configuration methods for WPS.
*/ 
typedef enum {

  SYS_WLAN_WPS_CONFIG_METHOD_NONE                  = 0,
  /**< @internal None.
  */

  SYS_WLAN_WPS_CONFIG_METHOD_USBA                  = 1,
  /**< USB flash drive.
  */

  SYS_WLAN_WPS_CONFIG_METHOD_ETHERNET              = 2,
  /**< Ethernet.
  */

  SYS_WLAN_WPS_CONFIG_METHOD_LABEL                 = 4,
  /**< Label on the hardware.
  */

  SYS_WLAN_WPS_CONFIG_METHOD_DISPLAY               = 8,
  /**< Display on hardware.
  */
  
  SYS_WLAN_WPS_CONFIG_METHOD_EXT_NFC_TOKEN         = 16,
  /**< External NFC token.
  */

  SYS_WLAN_WPS_CONFIG_METHOD_INT_NFC_TKN           = 32,
  /**< Internal NFC token.
  */

  SYS_WLAN_WPS_CONFIG_METHOD_NFC_IFACE             = 64,
  /**< NFC interface.
  */

  SYS_WLAN_WPS_CONFIG_METHOD_PUSHBUTTON            = 128,
  /**< Push button.
  */

  SYS_WLAN_WPS_CONFIG_METHOD_KEYPAD                = 256,
  /**< Keypad.
  */

  SYS_WLAN_WPS_CONFIG_METHOD_MAX
  /**< @internal Maximum value.
  */

} sys_wlan_wps_config_method_e_type;
/*~ SENTINEL sys_wlan_wps_config_method_e_type.SYS_WLAN_WPS_CONFIG_METHOD_MAX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** WPS devive password types.
*/
typedef enum {

  SYS_WLAN_WPS_DEV_PASSWD_ID_NONE                  = -1,
  /**< @internal None. */

  SYS_WLAN_WPS_DEV_PASSWD_ID_PIN                   =  0,
  /**< PIN. */

  SYS_WLAN_WPS_DEV_PASSWD_ID_USER_SPECIFIED        =  1,
  /**< User specified. */

  SYS_WLAN_WPS_DEV_PASSWD_ID_MACHINE_SPECIFIED     =  2,
  /**< Machine specified. */

  SYS_WLAN_WPS_DEV_PASSWD_ID_REKEY                 =  3,
  /**< 256 bit rekeying password. */

  SYS_WLAN_WPS_DEV_PASSWD_ID_PUSHBUTTON            =  4,
  /**< Push button. */

  SYS_WLAN_WPS_DEV_PASSWD_ID_REGISTRAR_SPECIFIED   =  5,
  /**< Registrar specified. */

  SYS_WLAN_WPS_DEV_PASSWD_ID_MAX
  /**< @internal Maximum. */

} sys_wlan_wps_dev_passwd_id_e_type;
/*~ SENTINEL sys_wlan_wps_dev_passwd_id_e_type.SYS_WLAN_WPS_DEV_PASSWD_ID_MAX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** WPS procedure end status.
*/
typedef enum {

  /** @internal None. */
  SYS_WLAN_WPS_STATUS_NONE             = -1,

  /** WPS procedure completed without any errors. */
  SYS_WLAN_WPS_STATUS_NO_ERROR         =  0,

  /*------------------------------------------------------*/
  /* Configuration Errors in registration protocol. */
  /*------------------------------------------------------*/

  /** Out of band interface read error. */
  SYS_WLAN_WPS_STATUS_OOB_IFACE_READ_ERROR,

  /** Decryption CRC failure. */
  SYS_WLAN_WPS_STATUS_DECRYPT_CRC_FAILURE,

  /** 2.4 channel not supported. */
  SYS_WLAN_WPS_STATUS_2_4_CHAN_NOT_SUPPORTED,

  /** 5.0 channel not supported. */
  SYS_WLAN_WPS_STATUS_5_0_CHAN_NOT_SUPPORTED = 4,
  
  /** Signal too weak. */
  SYS_WLAN_WPS_STATUS_SIGNAL_TOO_WEAK,

  /** Network authentication failure. */
  SYS_WLAN_WPS_STATUS_NETWORK_AUTH_FAILURE,

  /** Network association failure. */
  SYS_WLAN_WPS_STATUS_NETWORK_ASSOC_FAILURE,
  
  /** No DHCP response. */
  SYS_WLAN_WPS_STATUS_NO_DHCP_RESP           = 8,

  /** Failed DHCP config. */
  SYS_WLAN_WPS_STATUS_FAILED_DHCP_CONFIG,

  /** IP address conflict. */
  SYS_WLAN_WPS_STATUS_IP_ADDR_CONFLICT,

  /** Couldn't connect to Registrar. */
  SYS_WLAN_WPS_STATUS_NO_REGISTRAR_CONNECT,

  /** Multiple PBC sessions detected. */
  SYS_WLAN_WPS_STATUS_PBC_SESSION_OVERLAP   = 12,

  /** Rogue activity suspected. */
  SYS_WLAN_WPS_STATUS_ROGUE_ACTIVITY_SUSPECTED,

  /** Device busy. */
  SYS_WLAN_WPS_STATUS_DEVICE_BUSY,

  /** Setup locked. */
  SYS_WLAN_WPS_STATUS_SETUP_LOCKED,

  /** Message timeout. */
  SYS_WLAN_WPS_STATUS_MSG_TIMEOUT           = 16,

  /** Registration session timeout. */
  SYS_WLAN_WPS_STATUS_REG_SESSION_TIMEOUT,

  /** Device password authentication failure. */
  SYS_WLAN_WPS_STATUS_DEV_PASSWD_AUTH_FAILURE,

  /*------------------------------------------------------*/
  /* Failures detected in WLAN CP */
  /*------------------------------------------------------*/

  /** WPS capable access points not found. */
  SYS_WLAN_WPS_STATUS_WPS_CAPABLE_APS_NOT_FOUND    = 100,

  /*---------------------------------------------------------*/
  /* Internal Failures detected in WPS Registration Protocol */
  /*---------------------------------------------------------*/

  /** WPS Protocol error */
  SYS_WLAN_WPS_STATUS_GENERIC_PROTOCOL_ERROR       = 200,

  /** WPS registrar has suggested PBC */
  SYS_WLAN_WPS_STATUS_REGISTRAR_SUGGESTED_PBC,

  /** WPS registrar has suggested PIN */
  SYS_WLAN_WPS_STATUS_REGISTRAR_SUGGESTED_PIN,

  /** Invalid PIN entered */
  SYS_WLAN_WPS_STATUS_INVALID_PIN,

  /** Unexpected discovery */
  SYS_WLAN_WPS_STATUS_UNEXPECTED_DISCOVERY         = 204,

} sys_wlan_wps_status_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** WPS credentials.
*/
typedef struct sys_wlan_wps_credential_s {

  sys_wlan_sys_rec_s_type           sys_rec;
  /**< WLAN system record info.
  */

  sys_manual_sys_profile_s_type     profile;
  /**< WLAN security profile info.
  */

  boolean                           is_key_provided_auto;
  /**< Key provided automatically.
  */

  sys_wlan_bssid_type               bssid;
  /**< MAC address.
  */

} sys_wlan_wps_credential_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** WPS credential list.
*/
typedef struct sys_wlan_wps_credential_list_s {

  uint8                             count;
  /**< Number of credentials.
  */

  sys_wlan_wps_credential_s_type    cred[SYS_WLAN_WPS_CREDENTIAL_COUNT_MAX];
  /**< WLAN credentials.
  */

} sys_wlan_wps_credential_list_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** WPS result
*/
typedef struct sys_wlan_wps_result_s {

  sys_wlan_wps_status_e_type            status;
  /**< WPS procedure end status.
  ** @NOTE: If status is not SYS_WLAN_WPS_STATUS_NO_ERROR, then the cred_list
  ** has no meaning.
  */

  sys_wlan_wps_credential_list_s_type   cred_list;
  /**< WLAN credential list.
  */

} sys_wlan_wps_result_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** WPS Device password info.
*/
typedef struct sys_wlan_wps_dev_passwd_s {

  sys_wlan_wps_dev_passwd_id_e_type  id;

  uint16                             len;

  byte                               passwd[SYS_WLAN_WPS_DEV_PASSWD_LEN_MAX];

} sys_wlan_wps_dev_passwd_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Parameters needed for initiating WPS procedure.
*/
typedef struct sys_wlan_wps_start_param_s {

  sys_wlan_wps_mode_e_type            wps_mode;
  /**< Automatic or Manual mode of WPS operation.
  */

  sys_wlan_wps_config_method_e_type   config_method;
  /**< WPS configuration method to use.
  */
  
  sys_wlan_wps_dev_passwd_s_type      dev_passwd;
  /**< WPS Device password info.
  */

} sys_wlan_wps_start_param_s_type;

/*===========================================================================

FUNCTION sys_is_wlan_ssid_undefined

DESCRIPTION
  This function checks if WLAN ssid is undefined.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sys_is_wlan_ssid_undefined(

  const sys_wlan_ssid_s_type     *sys_id_ptr
);

/*===========================================================================

FUNCTION sys_is_wlan_ssid_match

DESCRIPTION
  This function compares two WLAN SSIDs.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  = if the two system identifiers are equal
  FALSE = if the two system identifiers are not equal

  Note: TRUE is returned if both system identifiers are undefined.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sys_is_wlan_ssid_match(

  const sys_wlan_ssid_s_type     *ssid_1_ptr,
  const sys_wlan_ssid_s_type     *ssid_2_ptr
);

/*===========================================================================

FUNCTION sys_undefine_wlan_ssid

DESCRIPTION
  This function undefines or initializes a WLAN SSID.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sys_undefine_wlan_ssid(

  sys_wlan_ssid_s_type     *sys_id_ptr
);

/*===========================================================================

FUNCTION sys_is_wlan_bssid_undefined

DESCRIPTION
  This function checks if WLAN bssid is undefined.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sys_is_wlan_bssid_undefined(

  const sys_wlan_bssid_type    *bss_id_ptr
);

/*===========================================================================

FUNCTION sys_undefine_wlan_bssid

DESCRIPTION
  This function undefines or initializes a WLAN BSSID.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sys_undefine_wlan_bssid(

  sys_wlan_bssid_type     *bss_id_ptr
);

/*===========================================================================

FUNCTION sys_is_wlan_bssid_match

DESCRIPTION
  This function compares two WLAN BSSIDs.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  = if the two system identifiers are equal
  FALSE = if the two system identifiers are not equal


SIDE EFFECTS
  None

===========================================================================*/
extern boolean sys_is_wlan_bssid_match(

  const sys_wlan_bssid_type     *bssid_1_ptr,
  const sys_wlan_bssid_type     *bssid_2_ptr
);

#endif /* #ifndef SYS_WLAN_H */
