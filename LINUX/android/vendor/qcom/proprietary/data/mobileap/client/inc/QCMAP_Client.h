#ifndef _QCMAP_CLIENT_H_
#define _QCMAP_CLIENT_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        _ Q C M A P _ C L I E N T . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/**
  @file QCMAP_Client.h
  @brief QCMAP Client public function declarations.
 */

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is maintained in the
      xxx_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*===========================================================================

FILE:  QCMAP_Client.h

SERVICES:
   QCMAP Client Class

===========================================================================*/
/*===========================================================================

Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
/*===========================================================================

                         EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  05/28/15   kl         (Tech Pubs) Edited/added Doxygen comments and markup.
  01/05/15   rk         qtimap off-target support.
  10/20/14   kl         (Tech Pubs) Edited/added Doxygen comments and markup.
  07/11/12   gk         Created module.
  10/26/12   cp         Added support for Dual AP and different types of NAT.
  02/27/13   cp         Added support to get IPV6 WAN status.
  04/17/13   mp         Added support to get IPv6 WWAN/STA mode configuration.
  06/12/13   sg         Added support for DHCP Reservation
  01/11/14   sr         Added support for connected devices information in SoftAP
  02/24/14   vm         Changes to Enable/Disable Station Mode to be in
                        accordance with IoE 9x15
===========================================================================*/
/* group: qcmap */
#ifdef FEATURE_QTIMAP_OFFTARGET
#include <glib.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include "stringl.h"
#include "ds_util.h"
#include "comdef.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "qcmap_client_util.h"
#ifdef FEATURE_QTIMAP_OFFTARGET
#include <tf_qcmap.h>
#endif

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#define strlcat g_strlcat
#endif

/** @addtogroup qcmap_constants
@{ */
/** Maximum size of the file. */
#define QCMAP_MSGR_MAX_FILE_PATH_LEN 100

/** Maximum port value. */
#define MAX_PORT_VALUE           65535

/** Maximum valid protocol value. */
#define MAX_PROTO_VALUE          255

/** Maximum valid Type of Service (TOS) value. */
#define MAX_TOS_VALUE          255

/** @} */ /* end_addtogroup qcmap_constants */

#ifdef __cplusplus
extern "C"
{
  #endif

  #include "qmi_client.h"

  #ifdef __cplusplus
}
#endif
#ifndef FEATURE_QTIMAP_OFFTARGET
/** @addtogroup qcmap_macros
@{ */

/*
Log Message Macros
*/
/** Macro for a high-level message. */
#define LOG_MSG_INFO1_LEVEL           MSG_LEGACY_HIGH

/** Macro for a medium-level message. */
#define LOG_MSG_INFO2_LEVEL           MSG_LEGACY_MED

/** Macro for a low-level message. */
#define LOG_MSG_INFO3_LEVEL           MSG_LEGACY_LOW

/**  Macro for a error message. */
#define LOG_MSG_ERROR_LEVEL           MSG_LEGACY_ERROR

/** Macro to print the log message information. */
#define PRINT_MSG( level, fmtString, x, y, z)                         \
        MSG_SPRINTF_4( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,      \
                       __FUNCTION__, x, y, z);

/** Macro to print a high-level message. */
#define LOG_MSG_INFO1( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO1_LEVEL, fmtString, x, y, z);                \
}

/** Macro to print a medium-level message. */
#define LOG_MSG_INFO2( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO2_LEVEL, fmtString, x, y, z);                \
}
/** Macro to print a low-level message. */
#define LOG_MSG_INFO3( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO3_LEVEL, fmtString, x, y, z);                \
}
/** Macro to print an error message. */
#define LOG_MSG_ERROR( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_ERROR_LEVEL, fmtString, x, y, z);                \
}

/** @} */ /* end_addtogroup qcmap_macros */
#endif
/** @addtogroup qcmap_datatypes
@{ */
/** Parameters that control QCMAP client behavior. */
typedef void (*client_status_ind_t)
(
  qmi_client_type user_handle,   /**< QMI user handle. */
  unsigned int msg_id,           /**< Indicator message ID. */
  void *ind_buf,                 /**< Raw indication data. */
  unsigned int ind_buf_len,      /**< Raw data length. */
  void *ind_cb_data              /**< User callback handle. */
);

/** Data type for IPv4 configuration. */
typedef struct
{
  in_addr public_ip;       /**< Public IP address. */
  in_addr primary_dns;     /**< Primary Domain Name Service (DNS) IP address. */
  in_addr secondary_dns;   /**< Secondary Domain Name Service (DNS) IP
                                address. */
}v4_conf_t;

/** Data type for IPv6 configuration. */
typedef struct
{
  struct in6_addr public_ip_v6;      /**< Public IPv6 address. */
  struct in6_addr primary_dns_v6;    /**< Primary Domain Name Service (DNS)
                                          IPv6 address. */
  struct in6_addr secondary_dns_v6;  /**< Secondary Domain Name Service (DNS)
                                          IPv6 address. */
}v6_conf_t;

/** Data type for the backhaul preference. */
typedef struct
{
  qcmap_msgr_backhaul_type_enum_v01 first;  /**< First backhaul preference. */
  qcmap_msgr_backhaul_type_enum_v01 second; /**< Second backhaul preference. */
  qcmap_msgr_backhaul_type_enum_v01 third;  /**< Third backhaul preference. */
  qcmap_msgr_backhaul_type_enum_v01 fourth;  /**< Fourth backhaul preference. */
}backhaul_pref_t;

/** Data type for network configuration. */
typedef union
{
  v4_conf_t v4_conf;      /**< IPv4 configuration. */
  v6_conf_t v6_conf;      /**< IPv6 configuration. */
}qcmap_nw_params_t;

/** @} */ /* end_addtogroup qcmap_datatypes */

//===================================================================
//              Class Definitions
//===================================================================

/**  @ingroup qcmap_class */
class QCMAP_Client
{
  public:

/*===========================================================================
FUNCTION QCMAP_Client()
===========================================================================*/
/** @ingroup qcmap_class

  Constructor for the client library QCMAP_Client class.

  This constructor calls the QMI Common Client Interface (QCCI) functions
  to connect with the QCMAP_MSGR service and  registers the client callback
  function for any asynchronous indications received from the QCMAP_MSGR
  service.

  @datatypes
  #client_status_ind_t

  @param[in,out] client_cb_ind       Client callback function registered with
                                     the QMI framework.

  @return
  None.

  @dependencies
  None.
*/
QCMAP_Client
(
  client_status_ind_t client_cb_ind
);

/*===========================================================================
FUNCTION QCMAP_Client clean up
===========================================================================*/
/** @ingroup qcmap_class

  Destructor for the client object.

  @return
  None.

  @dependencies
  None.
*/
virtual ~QCMAP_Client
(
  void
);

/* ---------------------------MobileAP Execution---------------------------*/

/*===========================================================================
FUNCTION EnableMobileAP()
===========================================================================*/
/** @ingroup qcmap_enable_mobile_ap

  Enables the MobileAP interface.

  This function enables the QCMobileAP based on the configuration provided.
  The server, as part of enabling the QCMobileAP, gets the appropriate
  dsi_netctrl module and NAS handles.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE -- Successfully set up the dsi_netctrl module and
          NAS client handles. \n
  FALSE -- Any of the above operations failed.

  @dependencies
  None.
*/
boolean
EnableMobileAP
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION DisableMobileAP()
===========================================================================*/
/** @ingroup qcmap_disable_mobile_ap

  Disables the QCMobileAP solution.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE -- Successfully released the handles and NAS client. \n
  FALSE -- Any of the above operations failed.

  @dependencies
  QCMobileAP must be enabled.
*/

boolean
DisableMobileAP
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION EnableIPV4()
===========================================================================*/
/** @ingroup qcmap_enable_ipv4

  This command enables IPv4 functionality; establishes the IPv4 call using
  the stored network policy if autoconnect is enabled and establishes the
  NAT rules.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
EnableIPV4
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION DisableIPV4()
===========================================================================*/
/** @ingroup qcmap_disable_ipv4

  Disables IPv4 functionality; brings down the IPv4 call if it is already
  connected and disables NAT.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
DisableIPV4
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION EnableIPV6()
===========================================================================*/
/** @ingroup qcmap_enable_ipv6

  Enables IPv6 functionality.  The IPv6 call is established using the
  stored network policy if autoconnect is enabled and enables IPv6 forwarding.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
EnableIPV6
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION DisableIPV6()
===========================================================================*/
/** @ingroup qcmap_disable_ipv6

  Disables IPv6 functionality; brings down the IPv6 call
  if it is already connected and disables IPv6 forwarding.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
DisableIPV6
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION ConnectBackHaul()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:connect_back_haul}

  Connects the Wireless Wide Area Network (WWAN) backhaul.This
  function connects the WWAN based on the configuration provided.
  This function calls dsi_netctrl to connect the backhaul.

  @datatypes
  qcmap_msgr_wwan_call_type_v01\n
  qmi_error_type_v01

  @param[in] call_type          Identifies call type.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
ConnectBackHaul
(
  qcmap_msgr_wwan_call_type_v01 call_type,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION DisconnectBackHaul()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:disconnect_back_haul}

  Disconnects the WWAN backhaul. This function calls
  dsi_netctrl to disconnect the backhaul.

  @datatypes
  qcmap_msgr_wwan_call_type_v01\n
  qmi_error_type_v01

  @param[in] call_type         Identifies call type.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
DisconnectBackHaul
(
  qcmap_msgr_wwan_call_type_v01 call_type,
  qmi_error_type_v01 *qmi_err_num
);

/** @cond */
/*===========================================================================
* FUNCTION EnableSTAMode()
* ===========================================================================*/
/** @ingroup qcmap_enable_sta_mode

  Enables WLAN in STA-Only mode.
  This function is used internally by the eCNE module. Other QCMAP Clients
  must not use this function.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server. A few important
                                error codes are as follows:\n
                                - QMI_ERR_NO_EFFECT_V01 -- WLAN is already enabled. \n
                                - QMI_ERR_NONE_V01 -- Success. \n
                                - QMI_ERR_INTERNAL_V01 -- Failure.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.

*/
boolean
EnableSTAMode
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
* FUNCTION DisableSTAMode()
* ===========================================================================*/
/** @ingroup qcmap_disable_sta_mode

  Disables WLAN in STA-Only mode.
  This is only to be used internally by eCNE module and other QCMAP Clients
  should not use this API.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num  Error code returned by the server. Important
                           error codes are as follows:
                           QMI_ERR_NO_EFFECT_V01- WLAN is already Enabled
                           QMI_ERR_NONE_V01 - Success
                           QMI_ERR_INTERNAL_V01 - Failure.
                           @tablebulletend

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.

*/
boolean
DisableSTAMode
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION RegisterForWLANStatusIND()
===========================================================================*/
/** @ingroup qcmap_register_for_wlan_status_ind

  Registers for WLAN status indications.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num         Error code returned by the server.
  @param[in]  register_indication Boolean value to either register or deregister
                                  for a WLAN status indication.

  @return
  TRUE upon success. \n
  FALSE upon failure.

*/
boolean
RegisterForWLANStatusIND
(
  qmi_error_type_v01 *qmi_err_num,
  boolean register_indication
);

/** @endcond */

/*===========================================================================
FUNCTION EnableWLAN()
===========================================================================*/
/** @ingroup qcmap_enable_wlan

  Enables the WLAN. This function enables the WLAN based on the
  hostpad configuration provided. This function inserts the WLAN
  kernel module and advertises the SSID.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
EnableWLAN
(
  qmi_error_type_v01 *qmi_err_num,
  /** @cond */
  boolean privileged_client = false
  /** @endcond */
);

/*===========================================================================
FUNCTION DisableWLAN()
===========================================================================*/
/** @ingroup qcmap_disable_wlan

  Disables the WLAN. This function removes the kernel module
  for the Wi-Fi driver.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
DisableWLAN
(
  qmi_error_type_v01 *qmi_err_num,
  /** @cond */
  boolean privileged_client = false
  /** @endcond */
);

/*===========================================================================
FUNCTION SetWLANConfig()
===========================================================================*/
/** @ingroup qcmap_set_wlan_config

  Sets the WLAN configuration: WLAN mode,  guest access profile, and
  station mode configuration.

  @datatypes
  qcmap_msgr_wlan_mode_enum_v01\n
  qcmap_msgr_access_profile_v01\n
  qcmap_msgr_station_mode_config_v01\n
  qmi_error_type_v01

  @param[in] wlan_mode                WLAN mode to be set.
  @param[in] guest_ap_access_profile  Guest AP access profile configuration.
  @param[in] station_config           Station mode WLAN configuration.
  @param[out] qmi_err_num             Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
SetWLANConfig
(
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode,
  qcmap_msgr_access_profile_v01 guest_ap_access_profile,
  qcmap_msgr_station_mode_config_v01 station_config,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetWLANConfig()
===========================================================================*/
/** @ingroup qcmap_get_wlan_config

  Gets the WLAN configuration: WLAN mode, guest AP access profile, and station mode
  configuration.

  @datatypes
  qcmap_msgr_wlan_mode_enum_v01\n
  qcmap_msgr_access_profile_v01\n
  qcmap_msgr_station_mode_config_v01\n
  qmi_error_type_v01

  @param[out] wlan_mode                WLAN mode to be set.
  @param[out] guest_ap_access_profile  Guest AP access profile configuration.
  @param[out] station_config           Station mode WLAN configuration.
  @param[out] qmi_err_num              Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetWLANConfig
(
  qcmap_msgr_wlan_mode_enum_v01 *wlan_mode,
  qcmap_msgr_access_profile_v01 *guest_ap_access_profile,
  qcmap_msgr_station_mode_config_v01 *station_config,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION ActivateWLAN()
===========================================================================*/
/** @ingroup qcmap_activate_wlan

  Activates the WLAN with the latest configuration. This
  activation might cause the WLAN to be disabled and re-enabled depending
  on the changes to the configuration.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
ActivateWLAN
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetLANConfig()
===========================================================================*/
/** @ingroup qcmap_set_lan_config

  Sets the LAN configuration parameters like gateway IP, subnet mask
  and the enable DHCP flag, along with DHCP parameters. All LAN
  interfaces are bridged together using a bridge interface. A single
  instance of the DHCP server, running on the bridge interface,
  assigns IP addresses to all clients including WLAN clients and USB
  Terminal Equipment (TE). Therefore, all the clients get IP addresses
  allocated from the same subnet and address range.

  @datatypes
  qcmap_msgr_lan_config_v01\n
  qmi_error_type_v01

  @param[in] lan_config         LAN configuration parameters to set.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetLANConfig
(
  qcmap_msgr_lan_config_v01 lan_config,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetLANConfig()
===========================================================================*/
/** @ingroup qcmap_get_lan_config

  Gets the configured LAN configuration parameters. All LAN interfaces
  are bridged together using a bridge interface. A single instance of
  a DHCP server, running on the bridge interface, assigns IP addresses
  to all clients including WLAN clients and USB TE. Therefore, all the
  clients get IP addresses allocated from the same subnet and
  address range.

  @datatypes
  qcmap_msgr_lan_config_v01\n
  qmi_error_type_v01

  @param[out] lan_config        LAN configuration buffer to use.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetLANConfig
(
  qcmap_msgr_lan_config_v01 *lan_config,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION ActivateLAN()
===========================================================================*/
/** @ingroup qcmap_activate_lan

  Activates the LAN with the latest configuration.

  This activation might cause the WLAN to be disabled and re-enabled depending
  on the changes to the configuration. In either case, the backhaul
  is not affected.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
ActivateLAN
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetWLANStatus()
===========================================================================*/
/** @ingroup qcmap_get_wlan_status

  Gets the WLAN status.

  @datatypes
  qcmap_msgr_wlan_mode_enum_v01\n
  qmi_error_type_v01

  @param[out] wlan_mode         WLAN mode used.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetWLANStatus
(
  qcmap_msgr_wlan_mode_enum_v01 *wlan_mode,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION ActivateHostapdConfig()
===========================================================================*/
/** @ingroup qcmap_activate_hostapd_config

  Activates the hostapd configuration after the
  appropriate hostapd.conf is modified.

  @datatypes
  qcmap_msgr_activate_hostapd_ap_enum_v01\n
  qcmap_msgr_activate_hostapd_action_enum_v01\n
  qmi_error_type_v01

  @param[in] ap_type           Indicates hostapd.conf associated with the
                               ap_type to be activated.
  @param[in] action_type       Indicates the action to be performed: start,
                               stop, restart.
  @param[out] qmi_err_num      Error code returned by the server.
  @param[in] privileged_client Sets the status of the privileged client.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
ActivateHostapdConfig
(
  qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
  qcmap_msgr_activate_hostapd_action_enum_v01 action_type,
  qmi_error_type_v01 *qmi_err_num,
  boolean privileged_client = false
);

/*===========================================================================
FUNCTION ActivateSupplicantConfig()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wlan
    @xreflabel{sec:activate_supplicant_config}

  Activates the WPA supplicant configuration after wpa_supplicant.conf
  is modified.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
ActivateSupplicantConfig
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION AddStaticNatEntry()
===========================================================================*/
/** @ingroup qcmap_add_static_nat_entry

  Adds a static entry in the Network Address Translation (NAT) table and
  updates the XML file with the new entry.

  @datatypes
  qcmap_msgr_snat_entry_config_v01\n
  qmi_error_type_v01

  @param[in] snat_entry        Static NAT configuration to add.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/

boolean
AddStaticNatEntry
(
  qcmap_msgr_snat_entry_config_v01 *snat_entry,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION DeleteStaticNatEntry()
===========================================================================*/
/** @ingroup qcmap_delete_static_nat_entry

  Deletes the static entry from the NAT table. If the deletion is
  successful, this function also deletes the entry from the XML
  file.

  @datatypes
  qcmap_msgr_snat_entry_config_v01\n
  qmi_error_type_v01

  @param[in] snat_entry         Static NAT configuration to be deleted.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
DeleteStaticNatEntry
(
  qcmap_msgr_snat_entry_config_v01 *snat_entry,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetStaticNatConfig()
===========================================================================*/
/** @ingroup qcmap_get_static_nat_config

  Gets the static entries in the NAT table.

  @datatypes
  qcmap_msgr_snat_entry_config_v01\n
  qmi_error_type_v01

  @param[out] snat_config       Static NAT entries configured.
  @param[out] num_entries       Number of static NAT entries configured.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetStaticNatConfig
(
  qcmap_msgr_snat_entry_config_v01 *snat_config,
  int *num_entries,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION AddDMZ()
===========================================================================*/
/** @ingroup qcmap_add_dmz

  Adds the DMZ IP address and updates the XML file.

  This function gets the DMZ IP from the user and sends it to the QCMAP_MSGR
  server to be added as the DMZ server. It also updates the DMZ IP in the
  XML file.

  @datatypes
  qmi_error_type_v01

  @param[in] dmz_ip            DMZ IP to be added; address is in host byte order.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
AddDMZ
(
  uint32 dmz_ip,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION DeleteDMZ()
===========================================================================*/
/** @ingroup qcmap_delete_dmz

  Deletes the DMZ IP address and updates the DMZ IP address in the
  configuration XML file.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
DeleteDMZ
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetDMZ()
===========================================================================*/
/** @ingroup qcmap_get_dmz

  Queries the DMZ entry that was previously set with the
  QMI_QCMAP_MSGR_SET_DMZ command. If no DMZ is set by the client, an
  IP address of 0.0.0.0 is returned.

  @datatypes
  qmi_error_type_v01

  @param[out] dmz_ip           DMZ IP configured.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetDMZ
(
  uint32_t *dmz_ip,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetWWANStatistics()
===========================================================================*/
/* TODO fix formatting/values on params!!*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:get_wwan_statistics}

  Gets the WWAN statistics.

  This function populates the QCMAP_Client::wwan_stats obtained from the
  QCMAP_MSGR server.

  @datatypes
  qcmap_msgr_ip_family_enum_v01\n
  qcmap_msgr_wwan_statistics_type_v01\n
  qmi_error_type_v01

  @param[in] ip_family         @values
                               - 4 -- Gets the IPv4 WWAN statistics.\n
                               - 6 -- Gets the IPv6 WWAN statistics.
                               @tablebulletend
  @param[out] wwan_stats       Statistics for the identified IP version.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetWWANStatistics
(
  qcmap_msgr_ip_family_enum_v01 ip_family,
                             qcmap_msgr_wwan_statistics_type_v01 *wwan_stats,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION ResetWWANStatistics()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:reset_wwan_statistics}

  Resets the WWAN statistics for the specified IP version family.

  @datatypes
  qcmap_msgr_ip_family_enum_v01\n
  qmi_error_type_v01

  @param[in] ip_family          Values:\n
                                - 4 -- Resets the IPv4 WWAN statistics.
                                - 6 -- Resets the IPv6 WWAN statistics.
                                @tablebulletend
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
ResetWWANStatistics
(
  qcmap_msgr_ip_family_enum_v01 ip_family,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetIPSECVpnPassthrough()
===========================================================================*/
/** @ingroup qcmap_set_ipsec_vpn_passthrough

  Sets the Internet Protocol Security (IPSec) Virtual Private Network (VPN)
  passthrough control flag.

  @datatypes
  qmi_error_type_v01

  @param[in] enable            Sets the IPSec VPN passthrough control flag.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
SetIPSECVpnPassthrough
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetIPSECVpnPassthrough()
===========================================================================*/
/** @ingroup qcmap_get_ipsec_vpn_passthrough

  Gets the IPSec VPN passthrough control flag.

  @datatypes
  qmi_error_type_v01

  @param[out] enable           Gets the IPSec VPN passthrough control flag.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetIPSECVpnPassthrough
(
  boolean *enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetPPTPVpnPassthrough()
===========================================================================*/
/** @ingroup qcmap_set_pptp_vpn_passthrough

  Sets the Point-to-Point Tunneling Protocol (PPTP) VPN
  passthrough control flag.

  @datatypes
  qmi_error_type_v01

  @param[in] enable           Sets the PPTP VPN passthrough control flag.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
SetPPTPVpnPassthrough
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetPPTPVpnPassthrough()
===========================================================================*/
/** @ingroup qcmap_get_pptp_vpn_passthrough

  Gets the PPTP VPN passthrough control flag.

  @datatypes
  qmi_error_type_v01

  @param[out] enable          Gets the PPTP VPN passthrough control flag.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetPPTPVpnPassthrough
(
  boolean *enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetL2TPVpnPassthrough()
===========================================================================*/
/** @ingroup qcmap_set_l2tp_vpn_passthrough

  Sets the Layer 2 Tunneling Protocol (L2TP) VPN passthrough control flag.

  @datatypes
  qmi_error_type_v01

  @param[in] enable           Sets the L2TP VPN passthrough control flag
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
SetL2TPVpnPassthrough
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetL2TPVpnPassthrough()
===========================================================================*/
/** @ingroup qcmap_get_l2tp_vpn_passthrough

  Gets the L2TP VPN passthrough control flag.

  @datatypes
  qmi_error_type_v01

  @param[out] enable          Gets the L2TP VPN passthrough control flag
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetL2TPVpnPassthrough
(
  boolean *enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetWebserverWWANAccess()
===========================================================================*/
/** @ingroup qcmap_set_webserver_wwan_access

  Sets whether or not the webserver can be accessed from the WWAN
  interface. The command handler overwrites any previously
  configured value with the current value.

  @datatypes
  qmi_error_type_v01

  @param[in] enable           Sets the web server WWAN access flag.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetWebserverWWANAccess
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetWebserverWWANAccess()
===========================================================================*/
/** @ingroup qcmap_get_webserver_wwan_access

  Queries the webserver WWAN access status on the device.

  @datatypes
  qmi_error_type_v01

  @param[out] enable          Gets the web server's WWAN access status.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetWebserverWWANAccess
(
  boolean *enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetNatType()
===========================================================================*/
/** @ingroup qcmap_set_nat_type

  Sets the NAT type on the device. The command
  handler overwrites any previously configured value with the current value.

  @datatypes
  qcmap_msgr_nat_enum_v01\n
  qmi_error_type_v01

  @param[in] nat_type          NAT type to be configured.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
SetNatType
(
  qcmap_msgr_nat_enum_v01 nat_type,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetNatType()
===========================================================================*/
/** @ingroup qcmap_get_nat_type

  Gets the configured NAT type.

  @datatypes
  qcmap_msgr_nat_enum_v01\n
  qmi_error_type_v01

  @param[out] nat_type         NAT type which is configured.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetNatType
(
  qcmap_msgr_nat_enum_v01 *nat_type,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetAutoconnect()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:set_autoconnect}

  Sets the Auto Connect flag, which is then updated in the
  configuration file by the Connection Manager. This enables the
  QCMobileAP to connect to the backhaul after the QCMobileAP is
  enabled and to retry to connect to the backhaul on failure.

  @datatypes
  qmi_error_type_v01

  @param[in]  enable          Sets the auto connect flag.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
SetAutoconnect
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetAutoconnect()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:get_autoconnect}

  Gets the status of the auto connect flag.

  @datatypes
  qmi_error_type_v01

  @param[out] enable          Gets the auto connect control flag status.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetAutoconnect
(
  boolean *enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetRoaming()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:set_roaming}

  Sets the roaming flag.

  The roaming flag is then updated in the configuration file by the
  Connection Manager. This enables the QCMobileAP to connect to the network
  in roaming mode.

  @datatypes
  qmi_error_type_v01

  @param[in]  enable          Sets the roaming flag.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
SetRoaming
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetRoaming()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:get_roaming}

  Gets the status of the roaming flag.

  @datatypes
  qmi_error_type_v01

  @param[out] enable          Gets the status of the roaming flag.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetRoaming
(
  boolean *enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetNetworkConfiguration()
===========================================================================*/
/** @ingroup qcmap_get_network_configuration

  Gets the IPv4 network configuration.

  @datatypes
  qcmap_msgr_ip_family_enum_v01\n
  #qcmap_nw_params_t\n
  qmi_error_type_v01

  @param[in] ip_family          IP family for which to get the configuration.
  @param[out] qcmap_nw_params   Network configuration, such as public IP, and
                                primary and secondary DNS IP addresses.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetNetworkConfiguration
(
  qcmap_msgr_ip_family_enum_v01 ip_family,
  qcmap_nw_params_t *qcmap_nw_params,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetIPv4NetworkConfiguration()
===========================================================================*/
/** @ingroup qcmap_get_ipv4_network_configuration

  Gets the IPv4 network configuration.

  @datatypes
  qmi_error_type_v01

  @param[out] public_ip         Public IP address.
  @param[out] primary_dns       Primary DNS IP address.
  @param[out] secondary_dns     Secondary DNS IP address.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetIPv4NetworkConfiguration
(
  in_addr_t *public_ip,
  uint32 *primary_dns,
  in_addr_t *secondary_dns,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetIPv6NetworkConfiguration()
===========================================================================*/
/** @ingroup qcmap_get_ipv6_network_configuration

  Gets the IPV6 configuration.

  @datatypes
  struct in6_addr\n
  qmi_error_type_v01

  @param[out] public_ip         Public IP address.
  @param[out] primary_dns       Primary DNS IP address.
  @param[out] secondary_dns     Secondary DNS IP address.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetIPv6NetworkConfiguration
(
  struct in6_addr *public_ip,
  struct in6_addr *primary_dns,
  struct in6_addr *secondary_dns,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetNatTimeout()
===========================================================================*/
/** @ingroup qcmap_set_nat_timeout

  Sets different NAT timeouts on the device. The command
  handler overwrites any previously configured value with the current value.

  @datatypes
  qcmap_msgr_nat_timeout_enum_v01\n
  qmi_error_type_v01

  @param[in] timeout_type      NAT timeout type for which the timeout is
                               to be set.
  @param[in] timeout_value     Timeout value.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
SetNatTimeout
(
  qcmap_msgr_nat_timeout_enum_v01 timeout_type,
  uint32 timeout_value,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetNatTimeout()
===========================================================================*/
/** @ingroup qcmap_get_nat_timeout

  Gets the NAT timeout value configured for the NAT type.

  @datatypes
  qcmap_msgr_nat_timeout_enum_v01\n
  qmi_error_type_v01

  @param[in] timeout_type      NAT timeout type for which the timeout is
                               to be fetched.
  @param[out] timeout_value    Timeout value.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetNatTimeout
(
  qcmap_msgr_nat_timeout_enum_v01 timeout_type,
  uint32 *timeout_value,
  qmi_error_type_v01 *qmi_err_num
);


/* @addtogroup qcmap_fields
@{ */
/* QMI QCMAP_CM service information. */
qmi_client_type           qmi_qcmap_msgr_handle;
/*< Connection manager messenger handle. */
qmi_client_type           qmi_qcmap_msgr_notifier;
/*< Connection manager messenger notifier. */
qmi_cci_os_signal_type    qmi_qcmap_msgr_os_params;
/*< Connection manager messenger OS parameters.  */

/* @} */ /* endaddtogroup qcmap_fields */

/*===========================================================================
FUNCTION GetFireWallHandlesList()
===========================================================================*/
/** @ingroup qcmap_get_fire_wall_handles_list

  Gets all the firewall handles associated with a single
  MobileAP instance.

  @datatypes
  qcmap_msgr_get_firewall_handle_list_conf_t\n
  qmi_error_type_v01

  @param[out] handlelist       List of firewall handles.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/

boolean
GetFireWallHandlesList
(
  qcmap_msgr_get_firewall_handle_list_conf_t *handlelist,
  qmi_error_type_v01 *qmi_err_num
);
/*===========================================================================
FUNCTION AddFireWallEntry()
===========================================================================*/
/** @ingroup qcmap_add_fire_wall_entry

  Adds the firewall rule, which is then updated in
  the configuration file by the Connection Manager

  @datatypes
  qcmap_msgr_firewall_conf_t\n
  qmi_error_type_v01

  @param[in] extd_firewall_entry    Firewall rule to be set.
  @param[out] qmi_err_num           Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
AddFireWallEntry
(
  qcmap_msgr_firewall_conf_t *extd_firewall_entry,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION AddUPNPPinholeEntry()
===========================================================================*/
/** @ingroup qcmap_add_upnp_pinhole_entry

  Adds a UPNP pinhole entry.

  @datatypes
  qcmap_msgr_firewall_conf_t\n
  qmi_error_type_v01

  @param[in] extd_firewall_entry    UPNP pinhole entry to be set.
  @param[out] qmi_err_num           Error code returned by the server.
  @param[in] upnp_pinhole           Indicates whether to add the pinhole entry.


  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
AddUPNPPinholeEntry
(
  qcmap_msgr_firewall_conf_t *extd_firewall_entry,
  qmi_error_type_v01 *qmi_err_num,
  boolean upnp_pinhole = false
);

/*===========================================================================
FUNCTION GetFireWallEntry()
===========================================================================*/
/** @ingroup qcmap_get_fire_wall_entry

  Gets a firewall rule associated with a single firewall handle.

  @datatypes
  qcmap_msgr_firewall_entry_conf_t\n
  qmi_error_type_v01

  @param[in,out] firewall_entry     Firewall entry pointed to by the
                                    handle requested.
  @param[out] qmi_err_num           Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetFireWallEntry
(
  qcmap_msgr_firewall_entry_conf_t *firewall_entry,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION DeleteFireWallEntry()
===========================================================================*/
/** @ingroup qcmap_delete_fire_wall_entry

  Deletes the firewall rule pointed by the handle, which is
  then updated by the Connection Manager in the configuration file.

  @datatypes
  qmi_error_type_v01

  @param[in] handle           Handle that points to the firewall to be deleted.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
int
DeleteFireWallEntry
(
  int handle,
  qmi_error_type_v01  *qmi_err_num
);

/*===========================================================================
FUNCTION GetMobileAPStatus()
===========================================================================*/
/** @ingroup qcmap_get_mobile_ap_status

  Gets the QCMobileAP status.

  @datatypes
  qcmap_msgr_mobile_ap_status_enum_v01\n
  qmi_error_type_v01

  @param[out] status           QCMobileAP status.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetMobileAPStatus
(
  qcmap_msgr_mobile_ap_status_enum_v01 *status,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetWWANStatus()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:get_wwan_status}

  Gets the status of the WWAN connection.

  @datatypes
  qcmap_msgr_wwan_status_enum_v01\n
  qcmap_msgr_wwan_status_enum_v01\n
  qmi_error_type_v01

  @param[out] v4_status         WWAN connection status for the IPv4 call type.
  @param[out] v6_status         WWAN connection status for the IPv6 call type.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled. @newpage
*/
boolean
GetWWANStatus
(
  qcmap_msgr_wwan_status_enum_v01 *v4_status,
  qcmap_msgr_wwan_status_enum_v01 *v6_status,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetStationModeStatus()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wlan
    @xreflabel{sec:get_station_mode_status}

  Gets the status of the WLAN station mode connection.

  @datatypes
  qcmap_msgr_station_mode_status_enum_v01\n
  qmi_error_type_v01

  @param[out] status            WLAN station mode connection status.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetStationModeStatus
(
  qcmap_msgr_station_mode_status_enum_v01 *status,
  qmi_error_type_v01 *qmi_err_num
);

/*=====================================sec======================================
FUNCTION SetFirewall()
===========================================================================*/
/** @ingroup qcmap_set_firewall

  Enables or disables the firewall and sets the configuration to
  drop or accept the packets matching the rules.

  @datatypes
  qmi_error_type_v01

  @param[in] enable_firewall   Indicates whether the firewall is enabled.
  @param[in] pkts_allowed      Indicates whether to accept or drop packets
                               matching the rules.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
SetFirewall
(
  boolean enable_firewall,
  boolean pkts_allowed,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetFirewall()
===========================================================================*/
/** @ingroup qcmap_get_firewall

  Gets the firewall configuration. This function indicates whether the firewall
  is enabled or disabled and whether the firewall is configured to drop or
  accept the packets matching the rules.

  @datatypes
  qmi_error_type_v01

  @param[out] enable_firewall   Indicates whether the firewall is enabled.
  @param[out] pkts_allowed      Indicates whether to accept or drop packets
                                matching the rules.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetFirewall
(
  boolean *enable_firewall,
  boolean *pkts_allowed,
  qmi_error_type_v01 *qmi_err_num
);

/*=====================================sec======================================
FUNCTION SetUPNPState()
===========================================================================*/
/** @ingroup qcmap_set_upnp_state

  Enables or disables the UPNP pinhole.

  @datatypes
  qmi_error_type_v01

  @param[in] firewall_state        Indicates whether the firewall is enabled.
  @param[in] upnp_pinhole_flag     Indicates whether to enable or disable the UPNP pinhole. \n
  @param[out] qmi_err_num          Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/

boolean
SetUPNPState
(
  boolean firewall_state,
  boolean upnp_pinhole_flag,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetUPNPState()
===========================================================================*/
/** @ingroup qcmap_get_upnp_state

  Gets the pinhole state.

  @datatypes
  qmi_error_type_v01

  @param[out] upnp_pinhole_flag    Indicates whether the pinhole is enabled.
  @param[out] qmi_err_num          Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetUPNPState
(
  boolean *upnp_pinhole_flag,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetIPv4State()
===========================================================================*/
/** @ingroup qcmap_get_ipv4_state

  Gets the current IPv4 state.

  @datatypes
  qmi_error_type_v01

  @param[out] ipv4_state        Indicates whether IPv4 is enabled.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetIPv4State
(
  boolean *ipv4_state,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetIPv6State()
===========================================================================*/
/** @ingroup qcmap_get_ipv6_state

  Gets the current IPv6 state.

  @datatypes
  qmi_error_type_v01

  @param[out] ipv6_state        Indicates whether IPv6 is enabled.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetIPv6State
(
  boolean *ipv6_state,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetWWANPolicy()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:get_wwan_policy}

  Gets the current configured WWAN policy.

  @datatypes
  qcmap_msgr_net_policy_info_v01\n
  qmi_error_type_v01

  @param[out] WWAN_config       WWAN policy configured in the XML file.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetWWANPolicy
(
  qcmap_msgr_net_policy_info_v01 *WWAN_config,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetWWANPolicy()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:set_wwan_policy}

  Configures the WWAN policy.

  @datatypes
  qcmap_msgr_net_policy_info_v01\n
  qmi_error_type_v01

  @param[in] WWAN_config       WWAN policy information to be configured.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
SetWWANPolicy
(
  qcmap_msgr_net_policy_info_v01 WWAN_config,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION EnableUPNP()
===========================================================================*/
/** @ingroup qcmap_enable_upnp

  Enables the Universal Plug and Play (UPnP) daemon, unless it is already
  running.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
EnableUPNP
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION DisableUPNP()
===========================================================================*/
/** @ingroup qcmap_disable_upnp

  Disables the UPnP daemon, if it is running.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
DisableUPNP
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetUPNPStatus()
===========================================================================*/
/** @ingroup qcmap_get_upnp_status

  Returns the status of the UPnP daemon.

  @datatypes
  qcmap_msgr_upnp_mode_enum_v01\n
  qmi_error_type_v01

  @param[out] upnp_status      UPnP status.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetUPNPStatus
(
  qcmap_msgr_upnp_mode_enum_v01 *upnp_status,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION EnableDLNA()
===========================================================================*/
/** @ingroup qcmap_enable_dlna

  Enables the Digital Living Network Alliance (DLNA) daemon,
  unless it is already running

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
EnableDLNA
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION DisableDLNA()
===========================================================================*/
/** @ingroup qcmap_disable_dlna

  Disables the DLNA daemon, if it is running.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
DisableDLNA
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetDLNAStatus()
===========================================================================*/
/** @ingroup qcmap_get_dlna_status

  Returns the status of the DLNA daemon.

  @datatypes
  qcmap_msgr_dlna_mode_enum_v01\n
  qmi_error_type_v01

  @param[out] dlna_status       DLNA status.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.
*/
boolean
GetDLNAStatus
(
  qcmap_msgr_dlna_mode_enum_v01 *dlna_status,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetDLNAMediaDir()
===========================================================================*/
/** @ingroup qcmap_get_dlna_media_dir

  Returns the DLNA media directory.

  @datatypes
  qmi_error_type_v01

  @param[out] media_dir         DLNA media directory path.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetDLNAMediaDir
(
  char media_dir[],
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetDLNAMediaDir()
===========================================================================*/
/** @ingroup qcmap_set_dlna_media_dir

  Sets the DLNA media directory.

  @datatypes
  qmi_error_type_v01

  @param[in] media_dir         DLNA media directory.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetDLNAMediaDir
(
  char media_dir[],
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION EnableMDNS()
===========================================================================*/
/** @ingroup qcmap_enable_mdns

  Enables the Multicast DNS (MDNS) responder for QCMAP.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
EnableMDNS
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION DisableMDNS()
===========================================================================*/
/** @ingroup qcmap_disable_mdns

  Disables the MDNS responder for QCMAP.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
DisableMDNS
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetMDNSStatus()
===========================================================================*/
/** @ingroup qcmap_get_mdns_status

  Returns the status of the Multicast DNS mode.

  @datatypes
  qcmap_msgr_mdns_mode_enum_v01\n
  qmi_error_type_v01

  @param[out] mdns_status       MDNS status.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetMDNSStatus
(
  qcmap_msgr_mdns_mode_enum_v01 *mdns_status,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetQCMAPBootupCfg()
===========================================================================*/
/** @ingroup qcmap_set_qcmap_bootup_cfg

  Sets the MobileAP boot-up configuration.

  @datatypes
  qcmap_msgr_bootup_flag_v01\n
  qcmap_msgr_bootup_flag_v01\n
  qmi_error_type_v01

  @param[in] mobileap_enable   Flag to indicate MobileAP bring-up at boot-up.
  @param[in] wlan_enable       Flag to indicate WLAN bring-up at boot-up.
  @param[out] qmi_err_num      Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetQCMAPBootupCfg
(
  qcmap_msgr_bootup_flag_v01 mobileap_enable,
  qcmap_msgr_bootup_flag_v01 wlan_enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetQCMAPBootupCfg()
===========================================================================*/
/** @ingroup qcmap_get_qcmap_bootup_cfg

  Gets the MobileAP boot-up configuration.

  @datatypes
  qcmap_msgr_bootup_flag_v01\n
  qcmap_msgr_bootup_flag_v01\n
  qmi_error_type_v01

  @param[out] mobileap_enable   Flag to indicate MobileAP bring-up at boot-up.
  @param[out] wlan_enable       Flag to indicate WLAN bring-up at boot-up.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetQCMAPBootupCfg
(
  qcmap_msgr_bootup_flag_v01 *mobileap_enable,
  qcmap_msgr_bootup_flag_v01 *wlan_enable,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetDataRate()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:get_data_rate}

  Gets the current and maximum transmit and receive data rates.

  @datatypes
  qcmap_msgr_data_bitrate_v01\n
  qmi_error_type_v01

  @param[out] data_rate       Tx and Rx current and maximum data rates, in bps.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetDataRate
(
  qcmap_msgr_data_bitrate_v01 *data_rate,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetUPNPNotifyInterval()
===========================================================================*/
/** @ingroup qcmap_set_upnp_notify_interval

  Sets the UPnP notify interval.

  @datatypes
  qmi_error_type_v01

  @param[in] notify_int       Notify interval, in seconds.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetUPNPNotifyInterval
(
  int notify_int,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetUPNPNotifyInterval()
===========================================================================*/
/** @ingroup qcmap_get_upnp_notify_interval

  Gets the UPnP notify interval.

  @datatypes
  qmi_error_type_v01

  @param[out] notify_int      Notify interval, in seconds.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetUPNPNotifyInterval
(
  int *notify_int,
  qmi_error_type_v01 *qmi_err_num
);

/* Set/Get the DLNA SSDP notify interval rate */
/*===========================================================================
FUNCTION SetDLNANotifyInterval()
===========================================================================*/
/** @ingroup qcmap_set_dlna_notify_interval

  Sets the DLNA notify interval.

  @datatypes
  qmi_error_type_v01

  @param[in] notify_int       Notify interval, in seconds.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetDLNANotifyInterval
(
  int notify_int,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetDLNANotifyInterval()
===========================================================================*/
/** @ingroup qcmap_get_dlna_notify_interval

  Gets the DLNA notify interval.

  @datatypes
  qmi_error_type_v01

  @param[out] notify_int      Notify interval, in seconds.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetDLNANotifyInterval
(
  int *notify_int,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION AddDHCPReservRecord()
===========================================================================*/
/** @ingroup qcmap_add_dhcp_reserv_record

  Adds a DHCP reservation record.

  @datatypes
  qcmap_msgr_dhcp_reservation_v01\n
  qmi_error_type_v01

  @param[in] dhcp_reserv_record       DHCP reservation record.
  @param[out] qmi_err_num             Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
AddDHCPReservRecord
(
  qcmap_msgr_dhcp_reservation_v01 *dhcp_reserv_record,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetDHCPReservRecords()
===========================================================================*/
/** @ingroup qcmap_get_dhcp_reserv_records

  Gets all the configured DHCP reservation records.

  @datatypes
  qcmap_msgr_dhcp_reservation_v01\n
  qmi_error_type_v01

  @param[out] dhcp_reserv_record       DHCP reservation record.
  @param[out] num_entries              Number of DHCP reservation records.
  @param[out] qmi_err_num              Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetDHCPReservRecords
(
  qcmap_msgr_dhcp_reservation_v01 *dhcp_reserv_record,
  uint32_t *num_entries,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION EditDHCPReservRecord()
===========================================================================*/
/** @ingroup qcmap_edit_dhcp_reserv_record

  Edits an already configured DHCP reservation record.

  @datatypes
  qcmap_msgr_dhcp_reservation_v01\n
  qmi_error_type_v01

  @param[in] addr                     IP address used as an index in the DHCP
                                      reservation record list.
  @param[in] dhcp_reserv_record       Modified DHCP reservation record.
  @param[out] qmi_err_num             Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
EditDHCPReservRecord
(
  uint32_t *addr,
  qcmap_msgr_dhcp_reservation_v01 *dhcp_reserv_record,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION DeleteDHCPReservRecord()
===========================================================================*/
/** @ingroup qcmap_delete_dhcp_reserv_record

  Deletes a DHCP reservation record.

  @datatypes
  qmi_error_type_v01

  @param[in] addr                     IP address used to index into the DHCP
                                      reservation record list.
  @param[out] qmi_err_num             Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
DeleteDHCPReservRecord
(
  uint32_t *addr,
  qmi_error_type_v01 *qmi_err_num
);

uint32_t mobile_ap_handle;
/*< Handle for the mobile ap*/

/*===========================================================================
FUNCTION EnableAlg()
===========================================================================*/
/** @ingroup qcmap_enable_alg

  Enables Application Level Gateways (ALGs) specified in the bitmask at
  runtime.

  @datatypes
  qcmap_msgr_alg_type_mask_v01\n
  qmi_error_type_v01

  @param[in] alg_types          Bitmask indicating the ALGs to be loaded.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
EnableAlg
(
  qcmap_msgr_alg_type_mask_v01 alg_types,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION DisableAlg()
===========================================================================*/
/** @ingroup qcmap_disable_alg

  Disables the ALGs specified in the bitmask at runtime.

  @datatypes
  qcmap_msgr_alg_type_mask_v01\n
  qmi_error_type_v01

  @param[in] alg_types          Bitmask indicating the algorithms to be loaded.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
DisableAlg
(
  qcmap_msgr_alg_type_mask_v01 alg_types,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetSIPServerInfo()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:set_sip_server_info}

  Sets the default SIP server information in the QCMAP server,
  which is propagated to the LAN clients through DHCP.

  @datatypes
  qcmap_msgr_sip_server_info_v01\n
  qmi_error_type_v01

  @param[in] sip_server_info     Default SIP server information.
  @param[out] qmi_err_num        Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetSIPServerInfo
(
  qcmap_msgr_sip_server_info_v01 *sip_server_info,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetSIPServerInfo()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:get_sip_server_info}

  Obtains the default SIP server information and the network assigned SIP
  server information from the QCMAP server.

  @datatypes
  qcmap_msgr_sip_server_info_v01\n
  qmi_error_type_v01

  @param[out] default_sip_info          Default SIP server information.
  @param[out] network_sip_info          Network assigned SIP server information.
  @param[out] count_network_sip_info    Number of network assigned SIP servers
                                        returned.
  @param[out] qmi_err_num               Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetSIPServerInfo
(
  qcmap_msgr_sip_server_info_v01 *default_sip_info,
  qcmap_msgr_sip_server_info_v01 *network_sip_info,
  int *count_network_sip_info,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetV6SIPServerInfo()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:get_v6sip_server_info}

  Sends a QMI message to the QCMAP server to get the V6SIP server information.
  This function populates the necessary fields for the
  QMI_QCMAP_MSGR_GET_IPV6_SIP_SERVER_INFO_REQ request.

  @datatypes
  qcmap_msgr_ipv6_sip_server_info_v01\n
  qmi_error_type_v01

  @param[out] network_v6_sip_info       Network V6 assigned SIP server
                                        information.
  @param[out] count_network_sip_info    Number of network V6 assigned SIP
                                        servers returned.
  @param[out] qmi_err_num               Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetV6SIPServerInfo
(
  qcmap_msgr_ipv6_sip_server_info_v01 *network_v6_sip_info,
  int *count_network_sip_info,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION RestoreFactoryConfig()
===========================================================================*/
/** @ingroup qcmap_restore_factory_config

  Restores the QCMAP configuration to a factory default state
  and reboots the device.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
RestoreFactoryConfig
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetConnectedDevicesInfo()
===========================================================================*/
/** @ingroup qcmap_get_connected_devices_info

  Gets information about the connected LAN hosts.

  LAN hosts include both USB and WLAN clients. The information includes the
  MAC address, IP address, host name, and lease expiry time.

  @datatypes
  qcmap_msgr_connected_device_info_v01\n
  qmi_error_type_v01

  @param[out] conn_dev_info     Connected device information retrieved from
                                the server.
  @param[out] num_entries       Number of entries.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetConnectedDevicesInfo
(
  qcmap_msgr_connected_device_info_v01 *conn_dev_info,
  int *num_entries,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetSupplicantConfig()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wlan
    @xreflabel{sec:set_supplicant_config}

  Activates and deactivates the WPA supplicant and WPA CLI based on the
  supplicant configuration status flag.

  @datatypes
  qmi_error_type_v01

  @param[out] status            Flag indicating whether to activate or
                                deactivate the WPA supplicant.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean SetSupplicantConfig
(
  boolean status,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetCradleMode()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_cradle
    @xreflabel{sec:get_cradle_mode}

  Gets the current cradle mode configured on the device.

  @datatypes
  qcmap_msgr_cradle_mode_v01\n
  qmi_error_type_v01

  @param[out] mode              Gets the cradle mode.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetCradleMode
(
  qcmap_msgr_cradle_mode_v01 *mode,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetCradleMode()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_cradle
    @xreflabel{sec:set_cradle_mode}

  Configures a new cradle mode unless the mode is already set.

  @datatypes
  qcmap_msgr_cradle_mode_v01\n
  qmi_error_type_v01

  @param[in] mode               Sets the cradle mode.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetCradleMode
(
  qcmap_msgr_cradle_mode_v01 mode,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetPrefixDelegationConfig()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:get_prefix_delegation_config}

  Gets whether the prefix delegation configuration
  is enabled or disabled.

  @datatypes
  qmi_error_type_v01

  @param[out] pd_mode           Prefix delegation configuration.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetPrefixDelegationConfig
(
  boolean *pd_mode,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetPrefixDelegationConfig()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:set_prefix_delegation_config}

   Sets the prefix delegation configuration.

  @datatypes
  qmi_error_type_v01

  @param[in] pd_mode            Prefix delegation configuration.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetPrefixDelegationConfig
(
  boolean pd_mode,
  qmi_error_type_v01 *qmi_err_num
);

/*====================================================================
FUNCTION GetPrefixDelegationStatus()
=====================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:get_prefix_delegation_status}

  Gets the current prefix delegation mode.

  @datatypes
  qmi_error_type_v01

  @param[out] pd_mode           Prefix delegation mode.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetPrefixDelegationStatus
(
  boolean *pd_mode,
  qmi_error_type_v01 *qmi_err_num
);

/*====================================================================
FUNCTION SetGatewayUrl()
=====================================================================*/
/** @ingroup qcmap_set_gateway_url

  Sets the default gateway URL for the access point.

  @datatypes
  qmi_error_type_v01

  @param[in] url                URL to set.
  @param[in] url_len            URL length.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetGatewayUrl
(
  uint8_t *url,
  uint32_t url_len,
  qmi_error_type_v01 *qmi_err_num
);

/*====================================================================
FUNCTION GetGatewayUrl()
=====================================================================*/
/** @ingroup qcmap_get_gateway_url

  Gets the default gateway URL for the access point.

  @datatypes
  qmi_error_type_v01

  @param[out] url                URL to get.
  @param[out] url_len            URL length.
  @param[out] qmi_err_num        Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetGatewayUrl
(
  uint8_t *url,
  uint32_t *url_len,
  qmi_error_type_v01 *qmi_err_num
);

/*====================================================================
FUNCTION EnableDDNS()
=====================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:enable_ddns}

  Enables dynamic DNS, which updates the IP address of the name
  server in the Domain Name System.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
EnableDDNS
(
  qmi_error_type_v01 *qmi_err_num
);

/*====================================================================
FUNCTION DisableDDNS()
=====================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:disable_ddns}

  Disables the dynamic DNS service.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
DisableDDNS
(
  qmi_error_type_v01 *qmi_err_num
);

/*====================================================================
FUNCTION SetDDNSConfig()
=====================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:set_ddns_config}

  Sets the dynamic DNS configuration.

  @datatypes
  qcmap_msgr_set_dynamic_dns_config_req_msg_v01\n
  qmi_error_type_v01

  @param[in] setddns_cfg_req       Required dynamic DNS configuration
                                   to set.
  @param[out] qmi_err_num          Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetDDNSConfig
(
  qcmap_msgr_set_dynamic_dns_config_req_msg_v01 *setddns_cfg_req,
  qmi_error_type_v01 *qmi_err_num
);

/*====================================================================
FUNCTION GetDDNSConfig()
=====================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:get_ddns_config}

  Gets the current dynamic DNS configuration.

  @datatypes
  qcmap_msgr_get_dynamic_dns_config_resp_msg_v01\n
  qmi_error_type_v01

  @param[in] ddns_server          Dynamic DNS configuration.
  @param[out] qmi_err_num         Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetDDNSConfig
(
  qcmap_msgr_get_dynamic_dns_config_resp_msg_v01 *ddns_server,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetTinyProxyStatus()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
     @xreflabel{sec:get_tiny_proxy_status}

  Returns the status of the Tiny Proxy.

  @datatypes
  qcmap_msgr_tiny_proxy_mode_enum_v01\n
  qmi_error_type_v01

  @param[out] tiny_proxy_status       Tiny Proxy status.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.
*/

boolean
 GetTinyProxyStatus(qcmap_msgr_tiny_proxy_mode_enum_v01 *tiny_proxy_status,
                                    qmi_error_type_v01 *qmi_err_num);


/*====================================================================
FUNCTION EnableTinyProxy()
=====================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:enable_tiny_proxy}

  Enables Tiny Proxy.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean EnableTinyProxy(qmi_error_type_v01 *qmi_err_num);

/*====================================================================
FUNCTION DisableTinyProxy()
=====================================================================*/
/** @ingroup section_qcmap_backhaul_wwan
    @xreflabel{sec:disable_tiny_proxy}

  Disables the Tiny Proxy.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/

boolean DisableTinyProxy(qmi_error_type_v01 *qmi_err_num);

/*====================================================================
FUNCTION SetDLNAWhitelisting()
=====================================================================*/
/** @ingroup qcmap_set_dlna_whitelisting

  Sets the DLNA whitelisting state.

  @datatypes
  qmi_error_type_v01

  @param[in] dlna_whitelisting_allow       Indicates whether to set the
                                           whitelisting IP state.
  @param[out] qmi_err_num                  Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/

boolean SetDLNAWhitelisting(boolean dlna_whitelisting_allow,
qmi_error_type_v01 *qmi_err_num);

/*====================================================================
FUNCTION GetDLNAWhitelisting()
=====================================================================*/
/** @ingroup qcmap_get_dlna_whitelisting

  Gets the DLNA whitelisting state.

  @datatypes
  qmi_error_type_v01

  @param[out] dlna_whitelisting_allow     Indicates whether to get the
                                          whitelisting IP state.
  @param[out] qmi_err_num                 Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/

boolean GetDLNAWhitelisting(boolean *dlna_whitelisting_allow,
qmi_error_type_v01 *qmi_err_num);

/*====================================================================
FUNCTION AddDLNAWhitelistIP()
=====================================================================*/
/** @ingroup qcmap_add_dlna_whitelist_ip

  Adds the DLNA whitelisting IP.

  @datatypes
  qmi_error_type_v01

  @param[in] dlna_whitelisting_ip       DLNA whitelisting IP to add.
  @param[out] qmi_err_num               Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/


boolean AddDLNAWhitelistIP(uint32 dlna_whitelisting_ip ,
qmi_error_type_v01 *qmi_err_num);

/*====================================================================
FUNCTION DeleteDLNAWhitelistIP()
=====================================================================*/
/** @ingroup qcmap_delete_dlna_whitelist_ip

  Deletes the DLNA whitelisting IP.

  @datatypes
  qmi_error_type_v01

  @param[in] dlna_whitelisting_ip       DLNA whitelisting IP to delete.
  @param[out] qmi_err_num               Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/

boolean DeleteDLNAWhitelistIP(uint32 dlna_whitelisting_ip,
qmi_error_type_v01 *qmi_err_num);

/*====================================================================
FUNCTION SetActiveBackhaulPref()
=====================================================================*/
/** @ingroup qcmap_msgr_set_active_backhaul_pref

  Sets the backhaul preference.

  @datatypes
  qcmap_msgr_set_backhaul_pref_req_msg_v01
  qmi_error_type_v01

  @param[in] backhaul_pref_req   Backhaul preference.
  @param[out] qmi_err_num        Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetActiveBackhaulPref
(
  backhaul_pref_t *backhaul_pref_req,
  qmi_error_type_v01 *qmi_err_num
);

/*====================================================================
FUNCTION GetBackhaulPref()
=====================================================================*/
/** @ingroup qcmap_msgr_get_backhaul_pref

  Gets the backhaul preference.

  @datatypes
  qcmap_msgr_backhaul_type_enum_v01
  qmi_error_type_v01

  @param[in] resp            Backhaul preference.
  @param[out] qmi_err_num    Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetBackhaulPref
(
  backhaul_pref_t *resp,
  qmi_error_type_v01 *qmi_err_num
);
/*===========================================================================
FUNCTION GetEthBackhaulMode()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_eth_backhaul
    @xreflabel{sec:get_eth_backhaul_mode}

  Gets the current eth backhaul configured on the device.

  @datatypes
  qcmap_msgr_eth_backhaul_mode_v01\n
  qmi_error_type_v01

  @param[out] mode              Gets the Eth Backhaul mode.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
GetEthBackhaulMode
(
  qcmap_msgr_eth_backhaul_mode_v01 *mode,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetETHBackhaulMode()
===========================================================================*/
/** @ingroup section_qcmap_backhaul_eth_backhaul
    @xreflabel{sec:set_eth_backhaul_mode}

  Configures a new Eth Backhaul mode unless the mode is already set.

  @datatypes
  qcmap_msgr_eth_backhaul_mode_v01\n
  qmi_error_type_v01

  @param[in] mode               Sets the eth backhaul mode.
  @param[out] qmi_err_num       Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean
SetEthBackhaulMode
(
  qcmap_msgr_eth_backhaul_mode_v01 mode,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION SetIPPassthroughConfig()
===========================================================================*/
/** @ingroup qcmap_set_ip_passthrough_config

  Set the IP Passthrough configuration for tethered client.

  @datatypes
  qcmap_msgr_ip_passthrough_mode_enum_v01
  qcmap_msgr_ip_passthrough_config_v01
  qmi_error_type_v01

  @param[in] enable_state                IP passthrough enable state.
  @param[in] new_config                  Use New/Existing config
  @param[in] ip_passthrough_config       IP passthrough Configuration.
  @param[out] qmi_err_num                Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
SetIPPassthroughConfig
(
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state,
  bool new_config,
  qcmap_msgr_ip_passthrough_config_v01 *ip_passthrough_config,
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
FUNCTION GetIPPassthroughConfig()
===========================================================================*/
/** @ingroup qcmap_get_ip_passthrough_flag

  Get the IP Passthrough configuration.

  @datatypes
  qcmap_msgr_ip_passthrough_mode_enum_v01
  qcmap_msgr_ip_passthrough_config_v01
  qmi_error_type_v01

  @param[out] enable_state                Current state of IP Passthrough
  @param[out] ip_passthrough_config       IP Passthrough Configuration.
  @param[out] qmi_err_num                 Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/
boolean
GetIPPassthroughConfig
(
  qcmap_msgr_ip_passthrough_mode_enum_v01 *enable_state,
  qcmap_msgr_ip_passthrough_config_v01 *ip_passthrough_config,
  qmi_error_type_v01 * qmi_err_num
);

/*===========================================================================
FUNCTION GetIPPassthroughState()
===========================================================================*/
/** @ingroup qcmap_get_ip_passthrough_state

  Get the IP Passthrough active state.

  @datatypes
  qmi_error_type_v01

  @param[out] state                Current active state of IP Passthrough
  @param[out] qmi_err_num                 Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  QCMobileAP must be enabled.
*/

/*=========================================================================*/
boolean GetIPPassthroughState
(
  boolean *state,
  qmi_error_type_v01 *qmi_err_num
);

/*====================================================================
FUNCTION BringupBTTethering()
=====================================================================*/
/** @ingroup qcmap_msgr_bring_up_bt_tethering

  Bringup BT Tethering.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num    Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean BringupBTTethering(qmi_error_type_v01 *qmi_err_num);

/*====================================================================
FUNCTION BringdownBTTethering()
=====================================================================*/
/** @ingroup qcmap_msgr_bring_down_bt_tethering

  Bringdown BT Tethering.

  @datatypes
  qmi_error_type_v01

  @param[out] qmi_err_num    Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean BringdownBTTethering(qmi_error_type_v01 *qmi_err_num);

/*===========================================================================
FUNCTION GetBTTetheringStatus()
===========================================================================*/
/** @ingroup  qcmap_msgr_get_bt_tethering_status

  get bt tethering  status

  @datatypes
  qmi_error_type_v01

  @param[out] enable          get bt tethering status.
  @param[out] qmi_err_num     Error code returned by the server.

  @return
  TRUE upon success. \n
  FALSE upon failure.

  @dependencies
  None.
*/
boolean GetBTTetheringStatus(boolean *enable,
qmi_error_type_v01 *qmi_err_num);

private:

  boolean  qcmap_msgr_enable;
  /* Whether the QCMAP Connection Manager is enabled. */
};

#endif  /* QCMAP_CLIENT_H */
