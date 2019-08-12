#ifndef _QCMAP_Backhaul_WLAN_H_
#define _QCMAP_Backhaul_WLAN_H_

/*====================================================

FILE:  QCMAP_Backhaul_WLAN.h

SERVICES:
   QCMAP Connection Manager Backhaul AP-STA Class

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=====================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/04/14   ka           Created
======================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include "stringl.h"
#include "ds_util.h"
#include "comdef.h"
#include "qcmap_cm_api.h"

#define IPV6_DEFAULT_PREFERED_LIFETIME 2700;
#define IPV6_DEFAULT_VALID_LIFETIME 7200;

class QCMAP_Backhaul_WLAN
{
  private:
    static bool flag;
    static QCMAP_Backhaul_WLAN*object;
    QCMAP_Backhaul_WLAN(char *xml_path = NULL);

  public:
    static QCMAP_Backhaul_WLAN* Get_Instance(boolean obj_create=false);
    ~QCMAP_Backhaul_WLAN();

    /* ---------------------------APSTA Execution---------------------------*/
    boolean sta_connected_event_received;
    /* V4 connection available on STA*/
    boolean sta_v4_available;
    /* V6 connection available on STA*/
    boolean sta_v6_available;
    /* Is STA associaited.*/
    boolean sta_associated;
    boolean prev_ap_sta_bridge_mode;
    qcmap_msgr_station_mode_config_v01 prev_station_mode_config;
    /* AP-STA Bridge Mode Bookkeeping Variable */
    static boolean ap_sta_bridge_mode_activated;
    qcmap_ap_sta_bridge_t ap_sta_bridge;
    qcmap_cm_apsta_conf_t apsta_cfg;
    char path_to_supplicant_conf[QCMAP_CM_MAX_FILE_LEN];

    qcmap_addr_info_list_t proxy_addr_list;
   /* Use to store the gateway information from RA when acting as backhaul*/
   struct in6_addr ipv6_gateway_addr;

    boolean ReadAPSTAConfigFromXML();
    static boolean GetSetAPSTAConfigFromXML
                          ( qcmap_action_type action,
                            qcmap_msgr_station_mode_config_v01 *station_config );

    static boolean IsSTAAvailableV4();
    static boolean IsSTAAvailableV6();
    static boolean IsAPSTABridgeActivated(void);

    /* Activate/Deactivate Supplicant config. */
    static boolean SetWPASupplicantConfig
                                              ( boolean status,
                                                qmi_error_type_v01 *qmi_err_num );
    /* Actiavte Supplicant Request. */
    static boolean ActivateSupplicantConfig
                                            ( qmi_error_type_v01 *qmi_err_num );

    /* Enable STA mode (Concurrent AP+STA) */
    boolean EnableStaMode(void);
    /* Disable STA mode */
    boolean DisableStaMode(void);
    /* Boolean to indicate whether STA mode is connected or not. */
    /* Handle STA Associated Event. */
    void ProcessStaAssoc(void *cb_user_data);

    /* Handle STA Associated Fail Event. */
    void ProcessStaAssocFail(void *cb_user_data);
    /*Handle Address Assignment during STA Association*/
    static void ProcessAddrAssign(void *cb_user_data, char* devname, ip_version_enum_type ip_vsn);
    /*Handle IPV6 Address Assignment during STA Association*/
    static void ProcessIPv6AddrAssign(void *cb_user_data, qcmap_nl_sock_msg_t *nlmsg);
    /* Handle STA Disassociation event. */
    void ProcessStaDisAssoc(void);
    boolean IsStaCfgUpdated(void);
    boolean IsWpaClikilled();
    static boolean IsWpaSupplicantkilled();
    boolean DisconnectSTA(void);
    /* Handle STA Associated Event for AP-STA in Bridge mode */
    void ProcessStaAssocForWLANBridge(void *cb_user_data);

    /* Handle STA Disassociation event, for AP-STA in Bridge mode */
    void ProcessStaDisAssocForWLANBridge(void);

    /* Handle STA Associated DHCP IP Assignment Failure Event. */
    void ProcessStaDHCPIPFail(void *cb_user_data);
    void RestartUSBForBridgeMode(void);
    void EnterDefaultIPRulesForBridgeMode(void);
    void EnterDefaultMACRulesForBridgeMode(void);

    /* Drop RS/RA packets going across bridge */
    void EnterDefaultIPv6RulesForBridgeMode(void);
    void RemoveDefaultIPv6RulesForBridgeMode(void);

    boolean CleanV6ProxyEntries();
    void SwitchToAPSTABackhaul();
};
#endif

