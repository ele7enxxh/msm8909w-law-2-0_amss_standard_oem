#ifndef _QCMAP_LAN_H_
#define _QCMAP_LAN_H_

/*====================================================

FILE:  QCMAP_LAN.h

SERVICES:
   QCMAP LAN Class

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  08/26/14   ka           Created
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

#define GatewayURL_TAG                  "GatewayURL"
#define WWAN_Reserved_IP                "169.254.5.1"


typedef enum {
  CONFIG_START_IP = 1,
  CONFIG_END_IP,
  CONFIG_LEASE_TIME,
  CONFIG_AP_IP,
  CONFIG_SUBNET_MASK,
  CONFIG_ENABLE_DHCP
}qcmap_lan_config_enum;


class QCMAP_LAN
{
  private:
    static bool flag;
    static QCMAP_LAN *object;

    QCMAP_LAN();

  public:
    static QCMAP_LAN *Get_Instance(boolean obj_create=false);

    ~QCMAP_LAN();

    /* --------------------------MobileAP LAN Execution--------------------*/

    //ALL VARIABLES HERE//
    boolean  bridge_inited;
    qcmap_cm_lan_conf_t lan_cfg;
    qcmap_msgr_lan_config_v01                prev_lan_config;

    /* Variable used to store the local mac address assigned to bridge on bootup */
    struct ether_addr bridge_local_mac_addr;

    /* Socket used for sending RA's to deprecate prefix. */
    int                         bridge_sock;

    /*String to hold the DHCP command. Useful when LAN config has changed,
          but LAN has not been reactivated */
    char                        dhcp_command_str[MAX_COMMAND_STR_LEN];
    boolean enable_dns;
    static long int qcmap_match_device_type( const void *first,
                                                         const void *second );
    static boolean SetLANConfigToXML(qcmap_msgr_lan_config_v01 *);

    boolean ReadLANConfigFromXML();
    /* Set LAN Mode. */
    static boolean SetLANConfig( qcmap_msgr_lan_config_v01 *lan_config,
                                       qmi_error_type_v01 *qmi_err_num );

    /* Get Configured LAN Mode */
    static boolean GetLANConfig( qcmap_msgr_lan_config_v01 *lan_config,
                                       qmi_error_type_v01 *qmi_err_num );

    /* Actiavte LAN Request. */
    static boolean ActivateLAN( qmi_error_type_v01 *qmi_err_num,
                                     boolean admin = false );

    // Utility functions to check for update in lan/wlan configs
    boolean IsLanCfgUpdated(void);


    /* Configure, start and stop the Linux DHCPD server. */
    boolean SetDHCPDConfig( int intf, uint32 start, uint32 end,
                                    char * leasetime );
    boolean StartDHCPD(void);
    boolean StopDHCPD(void);
    static boolean check_non_empty_mac_addr(uint8 *mac, char mac_addr_string[]);

    /*DHCP Reservation*/
    /* Add a DHCP Reservation Record to the configuration and update XML file. */
    static boolean AddDHCPReservRecord
                      ( qcmap_msgr_dhcp_reservation_v01* dhcp_reservation_record,
                        qmi_error_type_v01 *qmi_err_num );

    int QCMAP_LAN::AddDHCPResRecToXML
                          ( qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record );
    int QCMAP_LAN::EditDHCPResRecToXML(uint32_t client_ip,
                             qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record);
    int QCMAP_LAN::DeleteDHCPResRecFromXML(uint32_t client_ip);

    /* Edit a DHCP Reservation Record from the configuration and update XML file. */
    static boolean EditDHCPReservRecord
                      ( uint32_t *client_ip,
                        qcmap_msgr_dhcp_reservation_v01 *dhcp_reservation_record,
                        qmi_error_type_v01 *qmi_err_num );

    /* Delete a DHCP Reservation Record from the configuration and update XML file. */
    static boolean DeleteDHCPReservRecord
                                              ( uint32_t *client_reserved_ip,
                                                qmi_error_type_v01 *qmi_err_num );

    /* Get DHCP Reservation Records from the XML configuration. */
    static boolean GetDHCPReservRecords
                  ( qcmap_msgr_dhcp_reservation_v01* dhcp_reservation_record,
                    unsigned int* num_entries, qmi_error_type_v01 *qmi_err_num );

    void sync_dhcp_hosts(void);

    /*Update DHCPD daemon with network SIP server, MTU info */
    boolean UpdateDHCPDNetworkInfo();

    /* Enable DNS */
    boolean EnableDNS(void);

    boolean CheckforAddrConflict( qcmap_msgr_lan_config_v01 *lan_config,
                                         qcmap_msgr_station_mode_config_v01 *station_config );

    bool AddEbtablesRuleForBridgeMode(qcmap_cm_client_data_info_t *data);
    bool DelEbtablesRuleForBridgeMode(qcmap_cm_client_data_info_t *data);

    bool DelEbtablesUSBRulesForBridgeMode(void);
    bool AddEbtablesUSBRulesForBridgeMode(void);
    boolean AddDHCPResRecEntryToList
                           ( qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record,
                             qcmap_msgr_dhcp_reservation_v01* dhcp_record );

    static boolean GetGatewayUrl( char *url, uint32_t *url_len,
                                        qmi_error_type_v01 *qmi_err_num );
    static boolean SetGatewayUrl( char *url, uint32_t url_len,
                                        qmi_error_type_v01 *qmi_err_num );

    boolean EnableGatewayUrl();
    boolean SetGatewayConfigToXML(char*);
    /* Intialize Bridge Interface. */
    void InitBridge();
    void DelBridge();
    int CreateRawSocket(char *devname);

    boolean EnableIPPassthrough
    (
      char *mac,
      char *host,
      qcmap_msgr_device_type_enum_v01 dev_type
    );

    boolean DisableIPPassthrough(bool default_route);
    boolean SetIPPassthroughConfig
    (
      qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state,
      bool new_config ,
      qcmap_msgr_ip_passthrough_config_v01 *passthrough_config,
      qmi_error_type_v01 * qmi_err_num
    );

    boolean GetIPPassthroughConfig
    (
      qcmap_msgr_ip_passthrough_mode_enum_v01 *enable_state,
      qcmap_msgr_ip_passthrough_config_v01 *passthrough_config,
      qmi_error_type_v01 *qmi_err_num
    );

    boolean QCMAP_LAN::RestartTetheredClient
    (
      qcmap_msgr_device_type_enum_v01 dev_type
    );

    boolean QCMAP_LAN::GetIPPassthroughState
    (
      bool *active_state,
      qmi_error_type_v01 *qmi_err_num
    );
    /*Toggles Autoneg parameter for ethernet iface*/
    static inline void ToggleAutoNegForEth(char* iface)
    {
      char command[MAX_COMMAND_STR_LEN];
      snprintf(command, MAX_COMMAND_STR_LEN,
               "ethtool -s %s autoneg off ; ethtool -s %s autoneg on", iface , iface);
      ds_system_call(command, strlen(command));
    }
};
#endif
