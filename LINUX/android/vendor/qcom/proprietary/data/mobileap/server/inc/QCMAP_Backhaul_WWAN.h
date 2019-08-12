#ifndef _QCMAP_BACKHAUL_WWAN_H_
#define _QCMAP_BACKHAUL_WWAN_H_

/*====================================================

FILE:  QCMAP_Backhaul_WWAN.h

SERVICES:
   QCMAP Connection Manager Backhaul WWAN Class

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*======================================================
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
#include "pugixml/pugixml.hpp"

typedef enum
{
  CONFIG_V4_UMTS_PROFILE_INDEX = 1,
  CONFIG_V4_CDMA_PROFILE_INDEX,
  CONFIG_V6_UMTS_PROFILE_INDEX,
  CONFIG_V6_CDMA_PROFILE_INDEX,
  CONFIG_ROAMING,  CONFIG_TECH,
  CONFIG_DEFAULT_SIPSERVER_CONFIG_TYPE,
  CONFIG_DEFAULT_SIPSERVER_CONFIG,
  CONFIG_PREFIX_DELEGATION,
}qcmap_wwan_config_enum;



class QCMAP_Backhaul_WWAN
{
  private:
    static bool flag;
    static QCMAP_Backhaul_WWAN *object;
    QCMAP_Backhaul_WWAN();

  public:
    static QCMAP_Backhaul_WWAN *Get_Instance(boolean obj_create=false);

    ~QCMAP_Backhaul_WWAN();

    /* ---------------------------WWAN Execution---------------------------*/

    //static boolean  roaming;
    qcmap_cm_wwan_conf_t wwan_cfg;
    boolean ipv6_prefix_based_rules_added;
    /* Prefix Delegation Status */
    boolean prefix_delegation_activated;


    /* ConnectBackHaul */
    boolean ReadWWANConfigFromXML();
    static boolean GetSetWWANConfigFromXML( qcmap_wwan_config_enum conf,
                                                           qcmap_action_type action,
                                                           char *data, int data_len );

    static boolean GetSetDefaultSIPServerConfigFromXML
                               ( qcmap_action_type,qcmap_cm_sip_server_info_t*);

    static boolean ConnectBackHaul( qcmap_msgr_wwan_call_type_v01 call_type,
                                            int *err_num,
                                            qmi_error_type_v01 *qmi_err_num);

    static boolean DisconnectBackHaul( qcmap_msgr_wwan_call_type_v01 call_type,
                                               int *err_num,
                                               qmi_error_type_v01 *qmi_err_num);

    static boolean SetAutoconnect( boolean enable,
                                         qmi_error_type_v01 *qmi_err_num);

    static boolean GetAutoconnect(void);
    static boolean SetRoaming( boolean enable,qmi_error_type_v01 *qmi_err_num);
    static boolean GetRoaming();

    /* Get WWAN statistics */
    static boolean GetWWANStatistics( qcmap_msgr_ip_family_enum_v01 ip_family,
                                              qcmap_msgr_wwan_statistics_type_v01 *wwan_stats,
                                              qmi_error_type_v01 *qmi_err_num);
    /* Reset WWAN statistics */
    static boolean ResetWWANStatistics( qcmap_msgr_ip_family_enum_v01 ip_family,
                                                qmi_error_type_v01 *qmi_err_num);

    static boolean GetWWANStatus( qcmap_msgr_wwan_call_type_v01 call_type,
                                          uint8_t *status,
                                          qmi_error_type_v01 *qmi_err_num);

    static boolean GetWWANPolicy( qcmap_msgr_net_policy_info_v01 *wwan_policy,
                                          qmi_error_type_v01 *qmi_err_num );

    static boolean SetWWANPolicy( qcmap_msgr_net_policy_info_v01 wwan_policy,
                                         qmi_error_type_v01 *qmi_err_num );

    /* Get the data bitrates */
    static boolean GetDataBitrate( qcmap_msgr_data_bitrate_v01 *data_rate,
                                       qmi_error_type_v01 *qmi_err_num);

    /*Add WWAN IPv4 SIP server information*/
    boolean AddWWANIPv4SIPServerInfo();

    /*Delete WWAN IPv4 SIP server information*/
    boolean DeleteWWANIPv4SIPServerInfo();

    /*Set SIP Server information*/
    static boolean SetSIPServerInfo
    (
      qcmap_msgr_sip_server_info_v01 default_sip_server_info,
      qmi_error_type_v01 *qmi_err_num
    );

    /*Get SIP Server information*/
    static boolean GetSIPServerInfo
                            ( qcmap_msgr_sip_server_info_v01 *default_sip_info,
                              qcmap_msgr_sip_server_info_v01 *network_sip_info,
                              int *count_network_sip_info,
                              qmi_error_type_v01 *qmi_err_num);

    /*Get IPV6 SIP Server information */
    static boolean GetV6SIPServerInfo
              ( qcmap_msgr_ipv6_sip_server_info_v01 *network_v6_sip_info,
                int *count_network_v6_sip_info,qmi_error_type_v01 *qmi_err_num);

    static boolean DeleteDelegatedPrefix(void*, qmi_error_type_v01*);

    /* Prefix Delegation Config */
    static boolean GetPrefixDelegationConfig( boolean *pd_mode,
                                                      qmi_error_type_v01 *qmi_err_num);

    static boolean SetPrefixDelegationConfig( boolean pd_mode,
                                                      qmi_error_type_v01 *qmi_err_num);

    static boolean GetPrefixDelegationStatus( boolean *pd_mode,
                                                      qmi_error_type_v01 *qmi_err_num);

    boolean AddIPv4SIPServerInfoToList(qcmap_cm_sip_server_info_t);

    boolean SendRSForClient(uint8_t *src_v6_addr);
    boolean SendRS(char *devname, uint8_t *src_v6_addr);
    boolean SendRSForClients(void);
    static boolean EnableDDNS( qmi_error_type_v01 *qmi_err_num);
    static boolean DisableDDNS( qmi_error_type_v01 *qmi_err_num);

    static boolean SetDDNS
                        ( qcmap_msgr_set_dynamic_dns_config_req_msg_v01 setddns,
                          qmi_error_type_v01 *qmi_err_num);

    static boolean GetDDNS
                       ( qcmap_msgr_get_dynamic_dns_config_resp_msg_v01 *getddns,
                         qmi_error_type_v01 *qmi_err_num);

    boolean DeleteDDNS();
    boolean StopDDNS();
    static boolean StartDDNS(qmi_error_type_v01 *qmi_err_num);
    static boolean GetSetDDNSConfigFromXML( qcmap_action_type,
                                                          qcmap_dynamic_dns_t *);
    static boolean GetTinyProxyStatus(qcmap_msgr_tiny_proxy_mode_enum_v01 *status,
                                            qmi_error_type_v01 *qmi_err_num);
    static boolean EnableTinyProxy(qmi_error_type_v01 *qmi_err_num);

    static boolean DisableTinyProxy(qmi_error_type_v01 *qmi_err_num);
    static boolean StopTinyProxy(qmi_error_type_v01 *qmi_err_num);
    static boolean GetSetTinyProxyFromXML(qcmap_action_type action,
                           qcmap_msgr_tiny_proxy_mode_enum_v01 *proxy_config);

    static boolean SetupTinyProxy(qmi_error_type_v01 *qmi_err_num);


};
#endif
