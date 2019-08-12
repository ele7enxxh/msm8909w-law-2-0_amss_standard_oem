#ifndef _QCMAP_CM_API_H_
#define _QCMAP_CM_API_H_

/******************************************************************************

                           QCMAP_CM_API.H

******************************************************************************/

/******************************************************************************

  @file    qcmap_cm_api.h
  @brief   Mobile AP Connection Manager Lib API

  DESCRIPTION
  Header file for Mobile AP Connection Manager Lib.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

when       who        what, where, why
--------   ---        -------------------------------------------------------
07/11/12   gk         9x25
10/26/12   cp         Added support for Dual AP and different types of NAT.
12/19/12   sb         Added support for RNDIS/ECM USB tethering
02/27/13   cp         Added support for deprecating of prefix when switching
					betweenstation mode and WWAN mode.
04/17/13   mp         Added support to get IPv6 WWAN/STA mode configuration.
06/12/13   sg         Added DHCP Reservation Feature
01/11/14   sr         Added support for connected devices information in SoftAP
03/27/14   cp         Added support to DUN+SoftAP.
02/05/14   pm         Added log message macros
01/05/15   rk         qtimap offtarget support.
******************************************************************************/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include <glib.h>
#include <stdbool.h>
#include "comdef.h"
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <linux/if_addr.h>
#include <netinet/ether.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netinet/icmp6.h>
#include "qcmap_firewall_util.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "qmi_client.h"
#include "dsi_netctrl.h"
#include "ds_util.h"
#include "ds_list.h"
#ifdef FEATURE_DATA_TARGET_MDM9607
#include "data_system_determination_v01.h"
#endif


#define QCMAP_CM_QMI_TIMEOUT_VALUE     15000
#define QCMAP_COEX_TIMEOUT_VALUE       1000
#define QCMAP_COEX_MAX_RETRY           5
#define MAX_COMMAND_STR_LEN 500
#define DATA_SIZE 512
#define IPV4_ADDR_LEN 4
#define IPV6_ADDR_LEN 16
#define NA_PACKET_SIZE 32
#define DSI_PROFILE_3GPP2_OFFSET (1000)
#define DSI_PROFILE_NUM_MAX      (9999)
#define DSI_IP_FAMILY_4   "IP"
#define DSI_IP_FAMILY_6   "IPV6"
#define DSI_IP_FAMILY_4_6 "IPV4V6"
#define QCMAP_DEFAULT_DSS_INIT_TIME    6
#define QCMAP_DSI_UDS_FILE "/etc/qcmap_dsi_uds_file"
#define QCMAP_CMDQ_UDS_FILE "/etc/qcmap_cmdq_uds_file"
#define QCMAP_NAS_UDS_FILE "/etc/qcmap_nas_uds_file"
#define QCMAP_TIMER_UDS_FILE "/etc/qcmap_timer_uds_file"
#define QCMAP_STA_UDS_FILE "/etc/qcmap_sta_uds_file"
#define IPA_XML_PATH   "/etc/IPACM_cfg.xml"
#define QCMAP_NL_UDS_FILE "/etc/qcmap_nl_uds_file"
#define AUTO_CONNECT_TIMER 1
#define QCMAP_MSGR_INTF_LEN 20
#define QCMAP_EPOCH_TIME_LEN 16
#define WLAN0 "wlan0"
#define WLAN1 "wlan1"
#define ETH0 "eth0"
#define BRIDGE_IFACE "bridge0"
#define MAX_CMD_SIZE 100
#define GATEWAY_URL "mobileap.qualcomm.com"
#define DEFAULT_SIP_SERVER "abcd.com"

/* Default network config */
//#define AR6003 "ar6003" // refer gerrit::1586737
//#define AR6004 "ar6004" // refer gerrit::1586737

/*Default CDT Config*/
#define CDT_DUAL_WIFI 3


/* WIFI iDriver/Firmware Init Delay Micro seconds */
#define WIFI_DEV_INIT_DELAY  50000
#define WIFI_DEV_INIT_DELAYS_MAX 5000000
#define DHCP_LEASE_TIME 43200 /*iLease time in seconds */
#define MIN_DHCP_LEASE 120 /*Lease time in seconds */
#define MAX_WIFI_CLIENTS 40

#define LAN_CATEGORY "LAN"
#define WAN_CATEGORY "WAN"

#define BRIDGE_MODE "BRIDGE"
#define ROUTER_MODE "ROUTER"

#define QCMAP_HOSTAPDCLI_MAX_RETRIES 15
#define QCMAP_HOSTAPDCLI_TIMEOUT_US 100000 /*100 milisecond*/
#define QCMAP_HOSTAPD_TIMEOUT_US 100000 /*100 milisecond*/

#define QCMAP_FIREWALL_CONFIG "/etc/mobileap_firewall.xml"
#define QCMAP_DEFAULT_CONFIG "/etc/mobileap_cfg.xml"
#define HOSTAPD_ENTROPY "/etc/entropy_file"
#define HOSTAPD_ENTROPY_SSID2 "/etc/entropy_file1"
#define HOSTAPD_CFG_PATH_SSID2 "/etc/hostapd-wlan1.conf"
#define STA_MODE_HOSTAPD_CFG_PATH "/etc/sta_mode_hostapd.conf"
#define HOSTAPD_CFG_PATH "/etc/hostapd.conf"


#define DDCLIENT_PID_FILE "/etc/ddclient.pid"
#define QCMAP_HOST_PATH "/etc/hosts"
#define QCMAP_DYNDNS_SERVER_URL "members.dyndns.org"
#define QCMAP_DYNDNS_PROTOCOL "dyndns2"
#define QCMAP_DDNS_CFG_PATH "/etc/ddclient.conf"
#define QCMAP_DDNS_CACHE_PATH "/etc/ddclient.cache"
#define DDNS_DEFAULT_LOGIN "ddclient"
#define DDNS_DEFAULT_PASSWORD "DYNdns123"
#define DDNS_DEFAULT_HOSTNAME "mobileap.dyndns.org"
#define DDNS_DEFAULT_TIMEOUT 60

#define WLAN_DEFAULT_DELAY 1

#define SUPPLICANT_CFG_PATH "/etc/wpa_supplicant.conf"
#define WPA_CTRL_PATH "/var/run/wpa_supplicant"
#define QCMAP_ERI_CONFIG "/etc/mobileap_eri_config.bin"

#define RNDIS_IFACE "rndis0"
#define ECM_IFACE "ecm0"
#define QCMAP_CM_LOG(...)                         \
  fprintf( stderr, "%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, __VA_ARGS__ )

#define QCMAP_CM_LOG_FUNC_ENTRY()  \
    QCMAP_CM_LOG                   \
    (                              \
        "Entering function %s\n",  \
        __FUNCTION__               \
    )

#define QCMAP_CM_LOG_FUNC_EXIT()   \
    QCMAP_CM_LOG                   \
    (                              \
        "Exiting function %s\n",   \
        __FUNCTION__               \
    )



#define Cradle_TAG                      "Cradle"
#define EnableIPV6_TAG                  "EnableIPV6"
#define EnableIPV4_TAG                  "EnableIPV4"
#define MobileAPWanCfg_TAG              "MobileAPWanCfg"
#define EriConfig_TAG                   "EriConfig"
#define FirstPreferredBackhaul_TAG      "FirstPreferredBackhaul"
#define SecondPreferredBackhaul_TAG     "SecondPreferredBackhaul"
#define ThirdPreferredBackhaul_TAG      "ThirdPreferredBackhaul"
#define FourthPreferredBackhaul_TAG     "FourthPreferredBackhaul"
#define EthBackhaul_TAG                 "EthBackhaul"

/* Router/Bridge Config flags */


#define StationModeCfg_TAG              "StationModeCfg"
#define STAModeConnType_TAG             "STAModeConnType"
#define STAModeStaticIPAddr_TAG         "StaticIPAddr"
#define STAModeStaticConfigDNSAddr_TAG  "StaticConfigDNSAddr"
#define STAModeStaticConfigGWAddr_TAG   "StaticConfigGWAddr"
#define STAModeStaticConfigNetMask_TAG  "StaticConfigNetMask"
#define Roaming_TAG                     "Roaming"
#define TECH_TAG                        "TECH"
#define V4_UMTS_PROFILE_INDEX_TAG       "V4_UMTS_PROFILE_INDEX"
#define V4_CDMA_PROFILE_INDEX_TAG       "V4_CDMA_PROFILE_INDEX"
#define V6_UMTS_PROFILE_INDEX_TAG       "V6_UMTS_PROFILE_INDEX"
#define V6_CDMA_PROFILE_INDEX_TAG       "V6_CDMA_PROFILE_INDEX"
#define IPFamily_TAG                    "IPFamily"
#define TECH_ANY_TAG                    "ANY"
#define TECH_3GPP_TAG                   "3GPP"
#define TECH_3GPP2_TAG                  "3GPP2"
#define DefaultSIPServerConfigType_TAG  "DefaultSIPServerConfigType"
#define DefaultSIPServerConfig_TAG      "DefaultSIPServerConfig"
#define FQDN_TAG                        "FQDN"
#define IP_TAG                          "IP"
#define PrefixDelegation_TAG            "PrefixDelegation"
#define GatewayURL_TAG                  "GatewayURL"
/* DDNS server */
#define DDNSCFG_TAG                      "DDNSCfg"
#define DDNS_Enable_TAG                  "EnableDDNS"
#define DDNS_Server_TAG                  "DDNSServer"
#define DDNS_Protocol_TAG                "DDNSProtocol"
#define DDNS_Login_TAG                   "DDNSLogin"
#define DDNS_Password_TAG                "DDNSPassword"
#define DDNS_Hostname_TAG                "DDNSHostname"
#define DDNS_Timeout_TAG                 "DDNSTimeout"
#define FirewallEnabled_TAG             "FirewallEnabled"
#define FirewallPktsAllowed_TAG         "FirewallPktsAllowed"
#define Firewall_TAG                    "Firewall"
#define FirewallStartPort_TAG           "FirewallStartPort"
#define FirewallEndPort_TAG             "FirewallEndPort"
#define FirewallProtocol_TAG            "FirewallProtocol"
#define HostAPDEntropy_TAG              "HostAPDEntropy"
#define STAModeHostAPDCfg_TAG           "STAModeHostAPDCfg"
/*Tiny Proxy Entries*/
#define TINYPROXYCFG_TAG                "TinyProxyCfg"
#define TINY_PROXY_Enable_TAG           "EnableTinyProxy"
/* LAN Config Entries */
#define WlanMode_TAG                    "WlanMode"
#define WlanRestartDelay_TAG            "WlanRestartDelay"
#define GuestAPCfg_TAG                  "GuestAPCfg"
#define EnableBridgeMode_TAG            "MobileAPSTABridgeEnable"

#define MobileAPLanCfg_TAG              "MobileAPLanCfg"
#define Enable_TAG                      "Enable"
#define Module_TAG                      "Module"
#define Wlan_Gpio_TAG                   "WlanGpioNum"
#define DevMode_TAG                     "DevMode"
#define HostAPDCfg_TAG                  "HostAPDCfg"
#define APIPAddr_TAG                    "APIPAddr"
#define SubNetMask_TAG                  "SubNetMask"
#define AccessProfile_TAG               "AccessProfile"
#define EnableDHCPServer_TAG            "EnableDHCPServer"
#define DHCPCfg_TAG                     "DHCPCfg"
#define DHCPStartIP_TAG                 "StartIP"
#define DHCPEndIP_TAG                   "EndIP"
#define DHCPLeaseTime_TAG               "LeaseTime"
#define EnableSupplicant_TAG            "EnableSupplicant"
#define SupplicantCfg_TAG               "SupplicantCfg"
#define AP_TAG                          "AP"
#define AP_AP_TAG                       "AP-AP"
#define AP_STA_TAG                      "AP-STA"
#define FULL_TAG                        "FULL"
#define INTERNET_TAG                    "INTERNETONLY"
#define AccessProfile_TAG               "AccessProfile"
#define EnableDHCPServer_TAG            "EnableDHCPServer"
#define DHCPCfg_TAG                     "DHCPCfg"
#define DHCPStartIP_TAG                 "StartIP"
#define DHCPEndIP_TAG                   "EndIP"
#define DHCPReservationRecord_TAG       "DHCPReservationRecord"
#define ClientReservedIP_TAG            "ClientReservedIP"
#define ClientName_TAG                  "ClientName"
#define ClientMACAddr_TAG               "ClientMACAddr"
#define AutoConnect_TAG                 "AutoConnect"
#define Reservation_TAG                 "Reservation"
#define IPPassthroughCfg_TAG            "IPPassthroughCfg"
#define IPPassthroughEnable_TAG         "IPPassthroughEnable"
#define IPPassthroughDeviceType_TAG     "IPPassthroughDeviceType"
#define IPPassthroughHostName_TAG       "IPPassthroughHostName"
#define IPPassthroughMacAddr_TAG        "IPPassthroughMacAddr"

/* IP Passthrough configuration needed for IPACM. */
#define IPPassthroughFlag_TAG           "IPPassthroughFlag"
#define IPPassthroughMode_TAG           "IPPassthroughMode"

/* Debug Tags */

#define SUPPLICANT_DEBUG_TAG            "SupplicantCmdAppend"
#define HOSTAPD_DEBUG_TAG               "HostapdCmdAppend"
#define HOSTAPD_GUEST_DEBUG_TAG         "GuestHostapdCmdAppend"


/* QCMAP bootup flags*/
#define MobileAPBootUp_TAG             "MobileAPBootUpCfg"
#define MobileAPEnable_TAG             "MobileAPEnableAtBootup"
#define WLANEnable_TAG                 "WLANEnableAtBootup"


/* IPA ETH Tags */
#define odu_cfg_TAG                    "ODUCFG"
#define mode_TAG                       "Mode"
#define Bridge_TAG                     "bridge"
#define Router_TAG                     "router"

/* IPA Iface Tags */
#define IPACM_TAG                       "IPACM"
#define IPACMIface_TAG                  "IPACMIface"
#define Iface_TAG                       "Iface"
#define Name_TAG                        "Name"
#define WAN_TAG                         "Wan"
#define Category_TAG                    "Category"
#define Mode_TAG                        "Mode"
#define WLANMode_TAG                    "WlanMode"
#define WLAN_Full_TAG                   "full"
#define WLAN_Internet_TAG               "internet"

/*Firewall Tags*/
#define FirewallEnabled_TAG             "FirewallEnabled"
#define FirewallPktsAllowed_TAG         "FirewallPktsAllowed"
#define UPNPInboundPinhole_TAG          "UPNPInboundPinhole"

#define Firewall_TAG                    "Firewall"
#define FirewallStartPort_TAG           "FirewallStartPort"
#define FirewallEndPort_TAG             "FirewallEndPort"
#define FirewallProtocol_TAG            "FirewallProtocol"
#define FirewallHandle_TAG              "FirewallHandle"
#define MobileAPFirewallCfg_TAG         "MobileAPFirewallCfg"
#define IPFamily_TAG                    "IPFamily"
#define IPV4SourceAddress_TAG           "IPV4SourceAddress"
#define IPV4SourceIPAddress_TAG         "IPV4SourceIPAddress"
#define IPV4SourceSubnetMask_TAG        "IPV4SourceSubnetMask"
#define PinholeEntry_TAG                "PinholeEntry"


#define IPV4DestinationAddress_TAG "IPV4DestinationAddress"
#define IPV4DestinationIPAddress_TAG "IPV4DestinationIPAddress"
#define IPV4DestinationSubnetMask_TAG "IPV4DestinationSubnetMask"

#define IPV4TypeOfService_TAG "IPV4TypeOfService"
#define TOSValue_TAG "TOSValue"
#define TOSMask_TAG "TOSMask"

#define IPV4NextHeaderProtocol_TAG "IPV4NextHeaderProtocol"

#define IPV6SourceAddress_TAG "IPV6SourceAddress"
#define IPV6SourceIPAddress_TAG "IPV6SourceIPAddress"
#define IPV6SourcePrefix_TAG "IPV6SourcePrefix"

#define IPV6DestinationAddress_TAG "IPV6DestinationAddress"
#define IPV6DestinationIPAddress_TAG "IPV6DestinationIPAddress"
#define IPV6DestinationPrefix_TAG "IPV6DestinationPrefix"

#define IPV6TrafficClass_TAG "IPV6TrafficClass"
#define TrfClsValue_TAG "TrfClsValue"
#define TrfClsMask_TAG "TrfClsMask"

#define IPV6NextHeaderProtocol_TAG "IPV6NextHeaderProtocol"

#define TCPSource_TAG "TCPSource"
#define TCPSourcePort_TAG "TCPSourcePort"
#define TCPSourceRange_TAG "TCPSourceRange"

#define TCPDestination_TAG "TCPDestination"
#define TCPDestinationPort_TAG "TCPDestinationPort"
#define TCPDestinationRange_TAG "TCPDestinationRange"

#define UDPSource_TAG "UDPSource"
#define UDPSourcePort_TAG "UDPSourcePort"
#define UDPSourceRange_TAG "UDPSourceRange"

#define UDPDestination_TAG "UDPDestination"
#define UDPDestinationPort_TAG "UDPDestinationPort"
#define UDPDestinationRange_TAG "UDPDestinationRange"

#define ICMPType_TAG "ICMPType"
#define ICMPCode_TAG "ICMPCode"

#define ESP_TAG "ESP"
#define ESPSPI_TAG "ESPSPI"

#define TCP_UDPSource_TAG "TCP_UDPSource"
#define TCP_UDPSourcePort_TAG "TCP_UDPSourcePort"
#define TCP_UDPSourceRange_TAG "TCP_UDPSourceRange"

#define TCP_UDPDestination_TAG "TCP_UDPDestination"
#define TCP_UDPDestinationPort_TAG "TCP_UDPDestinationPort"
#define TCP_UDPDestinationRange_TAG "TCP_UDPDestinationRange"




/* PPP Generic Driver Init Delay Micro seconds */
#define PPP_DEV_INIT_DELAY  50000
#define PPP_DEV_INIT_DELAYS_MAX 5000000

#define USB_GW_IP_OFFSET 1
#define USB_IP_OFFSET 2
#define DHCP_IP_OFFSET 4
#define NUM_USB_ADDR 4
#define MIN_DHCP_ADDR_RANGE 7

/* Default Timeout Values. */
#define QCMAP_NAT_ENTRY_DEFAULT_GENERIC_TIMEOUT 200
#define QCMAP_NAT_ENTRY_DEFAULT_ICMP_TIMEOUT 30
#define QCMAP_NAT_ENTRY_DEFAULT_TCP_TIMEOUT 3600
#define QCMAP_NAT_ENTRY_DEFAULT_UDP_TIMEOUT 60
#define QCMAP_NAT_ENTRY_MIN_TIMEOUT 30

#define MAC_NULL_STR "00:00:00:00:00:00" /*MAC Null String*/
#define QCMAP_PROCESS_KILL_WAIT_MS  50000 /* 50 mili seconds*/
#define QCMAP_PROCESS_KILL_RETRY  40
#define QCMAP_BRIDGE_MAX_RETRY 10
#define QCMAP_BRIDGE_MAX_TIMEOUT_MS 500 /* Micro seconds */
#define QCMAP_RESET_CONFIG_TIMEOUT 5 /* 5 seconds*/
#define QCMAP_QTI_MSG_TIMEOUT_S 2
#define DSI_INITED 1
#define QCMAP_HOSTAPD_START_POLL_MAX_COUNT 300 /*3Sec considering WLAN 11AC ACS*/
#define QCMAP_HOSTAPD_END_POLL_MAX_COUNT 30    /* 300 ms */
#define QCMAP_HOSTAPD_POLL_DELAY_MS 10000 /*10 ms or 10000 us*/
#define QCMAP_ASSOC_WLAN_TO_BRIDGE_MAX_RETRIES 200
#define QCMAP_ASSOC_WLAN_TO_BRIDGE_TIMEOUT_US 100000 /*100 milisecond*/
#define LL_SUBNET_MASK         "255.255.255.0"
#define RANDOM_MAC_KEY  3

#define STATE_FDB_TABLE "/etc/statefdbtable.txt"
#define PERMANENT "permanent"
#define QCMAP_RETRY_FDB_TIMEOUT_US 100000 /*100 milisecond*/
#define QCMAP_MAX_FDB_RETRY 10

/* Timer values for WLAN-STA events */
#define QCMAP_WLAN_STA_ASSOC_TIMEOUT 60 /* Seconds */
#define QCMAP_WLAN_STA_DHCP_TIMEOUT  60 /* Seconds */
#define QCMAP_BRIDGE_PROXY_TIMEOUT     60 /* Seconds */

/*Address lengths*/
#define INET_ADDRSTRLEN        16
#define INET6_ADDRSTRLEN       46

#define QCMAP_DDNS_PROTOCOL_LEN 100
#define QCMAP_MSGR_MAX_DDNS_CONF 2
#define DDNS_ENABLED 1
#define DDNS_DISABLED 0

/*Service tags */
#define System_TAG                      "system"
#define Debug_TAG                       "MobileAPDbg"
#define MobileAPCfg_TAG                 "MobileAPCfg"
#define MobileAPSrvcCfg_TAG             "MobileAPSrvcCfg"

/* Daemon Services Congig Entries*/
#define UPNP_TAG                        "UPnP"
#define DLNA_TAG                        "DLNA"
#define MDNS_TAG                        "MDNS"
#define DLNAWhitelistingAllow_TAG       "DLNAWhitelistingAllow"
#define DLNAWhitelistIPList_TAG         "DLNAWhitelistIPList"
#define WhitelistedIP_TAG               "WhitelistedIP"

/* NAT Config Tags */
#define MobileAPNatCfg_TAG              "MobileAPNatCfg"
#define PortFwding_TAG                  "PortFwding"
#define PortFwdingPrivateIP_TAG         "PortFwdingPrivateIP"
#define PortFwdingPrivatePort_TAG       "PortFwdingPrivatePort"
#define PortFwdingGlobalPort_TAG        "PortFwdingGlobalPort"
#define PortFwdingProtocol_TAG          "PortFwdingProtocol"
#define NatEntryGenericTimeout_TAG         "NatEntryGenericTimeout"
#define NatEntryICMPTimeout_TAG            "NatEntryICMPTimeout"
#define NatEntryTCPEstablishedTimeout_TAG  "NatEntryTCPEstablishedTimeout"
#define NatEntryUDPTimeout_TAG             "NatEntryUDPTimeout"
#define DmzIP_TAG                       "DmzIP"
#define EnableIPSECVpnPassthrough_TAG   "EnableIPSECVpnPassthrough"
#define EnablePPTPVpnPassthrough_TAG    "EnablePPTPVpnPassthrough"
#define EnableL2TPVpnPassthrough_TAG    "EnableL2TPVpnPassthrough"
#define EnableWebserverWWANAccess_TAG   "EnableWebserverWWANAccess"
#define ALGCfg_TAG                      "ALGCfg"
#define EnableRTSPAlg_TAG               "EnableRTSPAlg"
#define EnableSIPAlg_TAG                "EnableSIPAlg"
#define NATType_TAG                     "NATType"
#define SYMMETRIC_TAG                   "SYM"
#define PRC_TAG                         "PRC"
#define FULLCONE_TAG                    "FC"
#define ARC_TAG                         "ARC"

/*BT tethering Tags*/
#define BTTetheringCfg          "BTTetheringCfg"
#define BTTethFlag              "BTTethFlag"


//Used to define the maximum value of the variable passed in getset apis
#define MAX_STRING_LENGTH 100

/**
  Checks whether the prefix is link local.
 */
#define QCMAP_IN6_IS_PREFIX_LINKLOCAL(v6)                                      \
  ((ntohl(*v6) & 0xffff0000) == 0xfe800000)
#define QCMAP_IN6_IS_ZERO(v6) ((ntohl(*v6)) == 0)


#define APPS_LAN_IP_ADDR "192.168.225.1"
#define APPS_SUB_NET_MASK "255.255.255.0"

#define QCMAP_LAN_CONNECTED_TIMEOUT 60 // 60 seconds for LAN connected
#define QCMAP_WAN_CONNECTED_TIMEOUT 60 // 60 seconds for WAN connected
#define ETH_IFACE "eth0"
#define ETH_LINK "eth"
#define ETH_NET_CARRIER "/sys/class/net/eth0/carrier"
#define STATIC_WLAN "Static IP WLAN Client"


/* RS/RA Constants */
#define ROUTER_MULTICAST "ff02::2"
#define FRAME_SIZE 100
#define MAC_SIZE 6
#define SENDING_PID 1000
#define MAX_HOPS 255
#define MESSAGE_CODE 0
#define SEQUENCE_NUM 0
#define ICMP_HEADER_CHKSUM 0;
#define ETH_TYPE 2
#define MAX_IPV6_PREFIX 40
#define RANDOM_MAC_KEY 3



#define KERNEL_VERSION_LENGTH 16

#define FLOW_TOTAL 28
#define FLOW_LABEL 20

#ifdef QTIMAPSERVER_OFFTARGET
	#define ds_log_med printf
#endif

#define MAX_DHCPCD_INFO_RETRY 3
#define DHCPCD_INFO_RETRY_DELAY 1

typedef enum{
  UPDATE_MOBILEAP_XML = 0,
  UPDATE_FIREWALL_XML,
  UPDATE_IPACFG_XML
}qcmap_update_xml;

typedef enum{
  NO_BACKHAUL = 0,
  BACKHAUL_TYPE_WWAN,
  BACKHAUL_TYPE_AP_STA_BRIDGE,
  BACKHAUL_TYPE_AP_STA_ROUTER,
  BACKHAUL_TYPE_CRADLE,
  BACKHAUL_TYPE_ETHERNET
}qcmap_backhaul_type;


typedef enum {
CONFIG_MOBILEAP= 1,
CONFIG_WLAN,
}qcmap_bootup_config_enum;


typedef enum {
  CONFIG_WLAN_MODE = 1,
  CONFIG_GUEST_ACCESS_PROFILE,
}qcmap_wlan_config_enum;

typedef enum
{
  QCMAP_MSGR_INTF_AP_INDEX          = 0x00,
  QCMAP_MSGR_INTF_GUEST_AP_INDEX    = 0x01,
  QCMAP_MSGR_INTF_STATION_INDEX     = 0x02
} qcmap_cm_intf_index_type;

typedef enum
{
  QCMAP_MSGR_TETH_LINK_INDEX_NONE      = -1,
  QCMAP_MSGR_TETH_LINK_INDEX1          = 0,
  QCMAP_MSGR_TETH_LINK_INDEX2          = 1
} qcmap_cm_teth_index_type;

typedef enum
{
  QCMAP_CM_PROFILE_FULL_ACCESS         = 0x01,
  QCMAP_CM_PROFILE_INTERNET_ONLY       = 0x02
} qcmap_cm_access_profile_type;

typedef enum
{
  QCMAP_CM_DEVMODE_AP                  = 0x01,
  QCMAP_CM_DEVMODE_STA                 = 0x02
} qcmap_cm_devmode_type;

typedef enum
{
  QCMAP_MSGR_INPUT_CHAIN,
  QCMAP_MSGR_OUTPUT_CHAIN,
  QCMAP_MSGR_FORWARD_CHAIN,
} qcmap_msgr_iptable_chain;

/*----------------------------------------------------------------------------
  RNDIS and ECM enum types
----------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_QTI_TETHERED_LINK_NONE                       = 0x00,
  QCMAP_QTI_TETHERED_LINK_RNDIS                      = 0x01,
  QCMAP_QTI_TETHERED_LINK_ECM                        = 0x02,
  QCMAP_QTI_TETHERED_LINK_PPP                        = 0x03,
  QCMAP_QTI_TETHERED_LINK_ETH                        = 0x04
}qcmap_qti_tethered_link_type;

#define IPTABLE_CHAIN 8

typedef struct {
  dsi_hndl_t handle;
  const char* tech;
  const char* family;
  int profile;
} dsi_call_info_t;



/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef struct qcmap_dsi_buffer_s {
  dsi_hndl_t            dsi_nethandle;
  void                  *user_data;
  dsi_net_evt_t         evt;
  dsi_evt_payload_t     *payload_ptr;
} qcmap_dsi_buffer_t;

typedef struct qcmap_nas_buffer_s {
  qmi_client_type user_handle;                    /* QMI user handle       */
  unsigned int    msg_id;                         /* Indicator message ID  */
  void           *ind_buf;                        /* Raw indication data   */
  unsigned int    ind_buf_len;                    /* Raw data length       */
  void           *ind_cb_data;                     /* User call back handle */
} qcmap_nas_buffer_t;

typedef enum
{
  AUTO_CONNECT_V4 = 0x0,
  AUTO_CONNECT_V6,
  STA_ASSOC_FAIL,
  STA_DHCP_FAIL,
  NS_NA_PROXY_CLEANUP
}qcmap_timer_enum_t;

typedef struct qcmap_timer_buffer_s {
qcmap_timer_enum_t msg_id;                         /* Indicator message ID  */
} qcmap_timer_buffer_t;

typedef enum {
  STA_CONNECTED,
  STA_DISCONNECTED,
  AP_STA_CONNECTED,
  AP_STA_DISCONNECTED
}qcmap_sta_event_t;

typedef enum {
  CRADLE_CONNECTED,
  CRADLE_DISCONNECTED
}qcmap_cradle_event_t;

typedef enum {
  ETH_BACKHAUL_CONNECTED,
  ETH_BACKHAUL_DISCONNECTED
}qcmap_eth_backhaul_event_t;

typedef struct qcmap_sta_buffer_s {
  uint32 sta_cookie;
  qcmap_msgr_device_type_enum_v01 device_type;
  char mac_addr[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  qcmap_sta_event_t event;
}qcmap_sta_buffer_t;

typedef struct qcmap_dynamic_dns_server_config_s {
 char server_url[QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01];
 char protocol[QCMAP_DDNS_PROTOCOL_LEN];
}qcmap_ddns_server_info_t;

typedef struct qcmap_dynamic_dns_s {
 char login[QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01];
 char password[QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01];
 char hostname[QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01];
 uint32 timeout;
 qcmap_ddns_server_info_t server_info;
 uint8 enable;
}qcmap_dynamic_dns_t;


/*---------------------------------------------------------------------------
           Port Forwarding Entry Configuration.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32   port_fwding_private_ip;
  uint16   port_fwding_private_port;
  uint16   port_fwding_global_port;
  uint8    port_fwding_protocol;
} qcmap_cm_port_fwding_entry_conf_t;

typedef struct
{
  ds_dll_el_t *natEntryListHead;
  ds_dll_el_t *natEntryListTail;
}qcmap_nat_entry_list_t;

typedef struct
{
  ds_dll_el_t *sipServerEntryListHead;
  ds_dll_el_t *sipServerEntryListTail;
}qcmap_sip_server_info_list_t;


typedef struct{
  ds_dll_el_t *dhcpInfoEntryListHead;
  ds_dll_el_t *dhcpInfoEntryListTail;
}qcmap_dhcp_information_info_list_t;

typedef struct{
  ds_dll_el_t *firewallEntryListHead;
  ds_dll_el_t *firewallEntryListTail;
}qcmap_firewall_entry_list_t;

typedef struct{
  ds_dll_el_t *dlnawhitelistListHead;
  ds_dll_el_t *dlnawhitelistListTail;
}qcmap_dlna_whitelist_entry_list_t;

typedef struct
{
  boolean ip_passthrough_active;
  uint32  rmnet_reserved_ip;
  in_addr_t  public_ip;
  in_addr_t rmnet_gateway_ip;
  qcmap_msgr_device_type_enum_v01       device_type;
  in_addr_t  public_netmask;
  char rmnet_def_route [MAX_COMMAND_STR_LEN];
} qcmap_cm_ip_passthrough_conf_t;


/*---------------------------------------------------------------------------
           Extended FireWall Entry Configuration.
---------------------------------------------------------------------------*/
typedef struct
{
  ip_filter_type       filter_spec;
  uint32               firewall_handle;
  /* Boolean to indicate pinhole. */
  boolean upnp_pinhole_handle;
} qcmap_msgr_firewall_entry_conf_t;

/*---------------------------------------------------------------------------
           Extended FireWall handle list configuration.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 handle_list[QCMAP_MSGR_MAX_FIREWALL_ENTRIES_V01];
  ip_version_enum_type ip_family;
  int num_of_entries;
} qcmap_msgr_get_firewall_handle_list_conf_t;

/*---------------------------------------------------------------------------
           Extended FireWall configuration.
---------------------------------------------------------------------------*/
typedef union
{
  qcmap_msgr_firewall_entry_conf_t extd_firewall_entry;
  qcmap_msgr_get_firewall_handle_list_conf_t extd_firewall_handle_list;
  ip_version_enum_type ip_family;
} qcmap_msgr_firewall_conf_t;

typedef enum
{
SET_VALUE = 0,
GET_VALUE
}qcmap_action_type;

/*---------------------------------------------------------------------------
           APSTA Configuration.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean  ap_sta_bridge_mode;
  boolean  ap_sta_bridge_sta_assoc_running;
  timer_t  timerid_sta_assoc;
  qcmap_msgr_sta_static_ip_config_v01 static_ip_config;
  boolean  ap_sta_bridge_sta_dhcp_running;
  timer_t  timerid_sta_dhcp;
  char     sta_interface[QCMAP_MSGR_INTF_LEN];
   qcmap_msgr_sta_connection_enum_v01 conn_type;
  timer_t   timerid_proxy_cleanup;
}qcmap_cm_apsta_conf_t;

/*---------------------------------------------------------------------------
   IPV6 Address Type.
---------------------------------------------------------------------------*/
typedef enum {
  LINK_LOCAL_IPV6_ADDR = 1,
  GLOBAL_IPV6_ADDR
} qcmap_ipv6_addr_t;

/*---------------------------------------------------------------------------
  Data Structure to store AP-STA router mode info
---------------------------------------------------------------------------*/
typedef struct
{
  char bridge_mac[ETH_ALEN];
  char ap_mac[ETH_ALEN];
  char sta_mac[ETH_ALEN];
  struct in_addr bridge_ip;
  struct in6_addr bridge_ip6;
  struct in6_addr bridge_ip6_link_local;
  uint8_t prefix_len;
  struct in_addr bridge_netmask;
  struct in_addr bridge_def_gw;
}qcmap_ap_sta_bridge_t;

/*---------------------------------------------------------------------------
           NAT Configuration.
---------------------------------------------------------------------------*/

#define QCMAP_CM_MAX_FILE_LEN          120

typedef struct
{
  uint32   nat_entry_generic_timeout;
  uint32   nat_entry_icmp_timeout;
  uint32   nat_entry_tcp_established_timeout;
  uint32   nat_entry_udp_timeout;
  uint32   dmz_ip; /* 0 mean disable DMZ */
  uint8    enable_ipsec_vpn_pass_through;
  uint8    enable_pptp_vpn_pass_through;
  uint8    enable_l2tp_vpn_pass_through;
  uint8    enable_alg_mask;
  uint8    num_port_fwding_entries;
  qcmap_nat_entry_list_t port_fwding_entries;
  qcmap_msgr_nat_enum_v01
           nat_type;
  qcmap_msgr_nat_enum_v01
           prev_nat_type;
} qcmap_cm_nat_conf_t;

/*---------------------------------------------------------------------------
           LAN Configuration.
---------------------------------------------------------------------------*/
#define QCMAP_LAN_INVALID_QCMAP_HANDLE (-1)
#define QCMAP_LAN_INVALID_IFACE_INDEX  (-1)
#define QCMAP_LAN_MAX_IPV4_ADDR_SIZE   16    /* 3 dots + 4 * 3 #s + 1 null */
/* 3 Interfaces - Possible Modes: AP, AP+AP, STA+AP */
#define QCMAP_MAX_NUM_INTF             3
/* Maximum Tethered Instances Supported USB+CPE, USB+USB */
#define QCMAP_QTI_MAX_TETH_LINK        2

#define QCMAP_V4_DEFAULT_DEVICE_NAME "rmnet0"
#define QCMAP_V6_DEFAULT_DEVICE_NAME "rmnet1"

/*-------------------------------------------------------------------------
  USB config structure to store IP addresses for RNDIS/ECM tethering
--------------------------------------------------------------------------*/
typedef struct
{
  boolean link_enabled;
  qcmap_qti_tethered_link_type link_type;
  uint32 gateway_addr;
  uint32 subnet_mask;
  uint32 remote_link_ip4_address;
  uint8_t remote_link_mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  uint32   dhcp_lease_time;
}qcmap_cm_tethered_conf_t;

typedef struct
{
  /*------------ Interface parameters common to STA,AP modes ------------- */
  /* Enable and configure main interface. */
  boolean  enable;
  /*------------ Interface parameters specific to AP Mode ---------------- */
  /* Path to WLAN AP config which contain SSID/Mode/Encryption info */
  char     path_to_hostapd_conf[QCMAP_CM_MAX_FILE_LEN];
  char     path_to_hostapd_entropy[QCMAP_CM_MAX_FILE_LEN];
  /* Path to WLAN AP config which contain SSID/Mode/Encryption info in
   * Station Mode. Applicable only to Primary AP. */
  char     path_to_sta_mode_hostapd_conf[QCMAP_CM_MAX_FILE_LEN];

  /* LL interface configuration. All Addresses are in host order */
  uint32   ll_ip_addr;
  uint32   ll_subnet_mask;

  /* Type of access main interface has to networks. */
  qcmap_msgr_access_profile_v01 access_profile;


} qcmap_cm_intf_conf_t;

typedef struct
{
  qcmap_cm_intf_conf_t interface[QCMAP_MAX_NUM_INTF];
  char  module[QCMAP_CM_MAX_FILE_LEN];
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
}qcmap_cm_wlan_conf_t;

typedef struct
{
  /* Interface information. */
  uint32   ppp_reserved_ip;
  uint32   apps_ip_addr;
  uint32   sub_net_mask;
   /* DHCP server config */
  boolean  enable_dhcpd;
  uint32   dhcp_start_address;
  uint32   dhcp_end_address;
  uint32     dhcp_lease_time;
  uint32 num_dhcp_reservation_records;
  qcmap_dhcp_information_info_list_t dhcp_reservation_records;
  boolean dhcp_reservations_updated;
  char gateway_url[QCMAP_MSGR_MAX_GATEWAY_URL_V01];
  char bridge_mac[ETH_ALEN];
  qcmap_cm_ip_passthrough_conf_t ip_passthrough_cfg;
} qcmap_cm_lan_conf_t;



/*---------------------------------------------------------------------------
           WAN Configuration.
---------------------------------------------------------------------------*/
#define QCMAP_WAN_INVALID_QCMAP_HANDLE 0xFFFFFFFF
#define QCMAP_WAN_MAX_ERI_DATA_SIZE    256
#define QCMAP_WAN_TECH_ANY             0
#define QCMAP_WAN_TECH_3GPP            1
#define QCMAP_WAN_TECH_3GPP2           2
#define MAX_WAN_CON_TIMEOUT 120
#define CLOCKID CLOCK_REALTIME
#define SIG SIGUSR1

typedef struct
{
  struct
  {
    int  umts_profile_index;
    int  cdma_profile_index;
  }v4;
  struct
  {
    int  umts_profile_index;
    int  cdma_profile_index;
  }v6;
}qcmap_cm_profile_index;


// Backhaul Interface
typedef enum{
  TYPE_IPV4  =1,
  TYPE_IPV6
}qcmap_backhaul_interface_type;

typedef enum
{
  QCMAP_CM_SIP_SERVER_MIN,
  QCMAP_CM_SIP_SERVER_ADDR,
  QCMAP_CM_SIP_SERVER_FQDN,
  QCMAP_CM_SIP_SERVER_MAX
} qcmap_cm_sip_server_type_e;

typedef union
{
  uint32 sip_serv_ip;
  char sip_fqdn[QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01];
} qcmap_cm_sip_server_info;

typedef struct
{
  qcmap_cm_sip_server_type_e sip_serv_type;
  qcmap_cm_sip_server_info sip_serv_info;
} qcmap_cm_sip_server_info_t ;

typedef struct{
  uint8 num_firewall_entries;
  qcmap_firewall_entry_list_t extd_firewall_entries;
  uint8 firewall_enabled;
  uint8 firewall_pkts_allowed;
  uint8 upnp_inbound_pinhole;
}qcmap_cm_firewall_conf_t;

typedef struct{
  uint8 num_whitelist_entries;
  qcmap_dlna_whitelist_entry_list_t whitelist_entries;
}qcmap_cm_dlna_whitelist_conf_t;

typedef struct
{
  char     eri_config_file[QCMAP_CM_MAX_FILE_LEN];
  int      ip_family;
  uint8    enable_webserver_wwan_access;
  char     ipv4_interface[QCMAP_MSGR_INTF_LEN];
  char     ipv6_interface[QCMAP_MSGR_INTF_LEN];
} qcmap_cm_wan_conf_t;

typedef struct
{
  boolean  roaming;
  qcmap_cm_profile_index
                        profile_id;
  int      tech;
  int      prefix_delegation;
  qcmap_cm_sip_server_info_t default_sip_server_info;
  int sip_server_list_count;
  qcmap_sip_server_info_list_t sip_server_info_list;
  qcmap_dynamic_dns_t ddns_conf;
  qcmap_ddns_server_info_t ddns_server_info;
}qcmap_cm_wwan_conf_t;


typedef struct
{
  boolean upnp_config;
  boolean dlna_config;
  boolean mdns_config;
  boolean dlna_whitelisting_config;
} qcmap_cm_srvc_conf_t;

typedef struct
{
  boolean enable_wlan_at_bootup;
  boolean enable_mobileap_at_bootup;
}qcmap_cm_bootup_conf_t;

typedef struct
{
  boolean supplicant_debug;
  char    supplicant_dbg_cmd[QCMAP_CM_MAX_FILE_LEN];
  boolean hostapd_debug;
  char    hostapd_dbg_cmd[QCMAP_CM_MAX_FILE_LEN];
  boolean hostapd_guest_debug;
  char    hostapd_guest_dbg_cmd[QCMAP_CM_MAX_FILE_LEN];
}qcmap_cm_debug_conf_t;

typedef struct
{
  qcmap_cm_intf_conf_t  hostapd;
  qcmap_cm_intf_conf_t  hostapd_guest;
  char                  supplicant[QCMAP_CM_MAX_FILE_LEN];
  qcmap_cm_debug_conf_t debug;
} qcmap_cm_wlan_paths_conf_t;

typedef struct
{
  qcmap_msgr_cradle_mode_v01 cradle_mode;
}qcmap_cm_cradle_conf_t;

typedef struct
{
  qcmap_msgr_eth_backhaul_mode_v01 eth_mode;
}qcmap_cm_eth_backhaul_conf_t;
/*---------------------------------------------------------------------------
           Master Mobile AP Config.
---------------------------------------------------------------------------*/
typedef struct
{

  qcmap_cm_bootup_conf_t bootup_config;
  char firewall_config_file[QCMAP_CM_MAX_FILE_LEN];
} qcmap_cm_conf_t;


/*---------------------------------------------------------------------------
           Embedded Profile
---------------------------------------------------------------------------*/
typedef struct
{
  int      tech;
  int      umts_profile_index;
  int      cdma_profile_index;
  int      ip_family;

} qcmap_cm_embd_conf;

typedef struct qcmap_cm_nl_prefix_info_s {
  boolean prefix_info_valid;
  unsigned char prefix_len;
  unsigned int mtu;
  struct sockaddr_storage prefix_addr;
  struct ifa_cacheinfo          cache_info;
} qcmap_cm_nl_prefix_info_t;

/*------------------------------------------------------------
Store the head of the list which stores the MAC and IP address
-------------------------------------------------------------*/
typedef struct
{
  ds_dll_el_t *addrListHead;
  ds_dll_el_t *addrListTail;
}qcmap_addr_info_list_t;

typedef struct
{
  uint8 src_mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  uint8_t src_ip_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t dst_ip_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  time_t time;
}qcmap_cm_proxy_data_info_t;

typedef struct
{
  uint8 mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  qcmap_msgr_device_type_enum_v01 device_type;
  uint32 ip_addr;
  uint8_t ip_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t link_local_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  char host_name[QCMAP_MSGR_DEVICE_NAME_MAX_V01];
}qcmap_cm_client_data_info_t;

typedef struct
{
  uint32_t    numOfNodes;
  bool        wlanclientpresent;
  uint8_t     numofWlanclients;
}qcmap_cm_connected_info_t;

typedef enum
{
  QCMAP_AP_STA_DISCONNECTED = 0,
  QCMAP_DELNEIGH
}qcmap_cm_cdi_del_evt_t;

#ifdef FEATURE_DATA_TARGET_MDM9607
typedef struct
{

  /* Optional */
  /*  Available Systems */
  uint8_t avail_sys_valid;  /**< Must be set to true if avail_sys is being passed */
  uint32_t avail_sys_len;  /**< Must be set to # of elements in avail_sys */
  dsd_system_status_info_type_v01 avail_sys[QMI_DSD_MAX_AVAIL_SYS_V01];
  /**<  \n Array of all available systems. The first entry in the list is
       the preferred system.
  */

  /* Optional */
  /*  APN Available System Info */
  uint8_t apn_avail_sys_info_valid;  /**< Must be set to true if apn_avail_sys_info is being passed */
  uint32_t apn_avail_sys_info_len;  /**< Must be set to # of elements in apn_avail_sys_info */
  dsd_apn_avail_sys_info_type_v01 apn_avail_sys_info[QMI_DSD_MAX_APNS_V01];
  /**<  \n Array of all available APNs and their preferred/available systems.
  */

  /* Optional */
  /* Current and Recommended Global Preferred System Info */
  uint8_t global_pref_sys_valid;  /**< Must be set to true if global_pref_sys is being passed */
  dsd_global_pref_sys_info_type_v01 global_pref_sys;
  /**<  \n The Global current and recommended preferred systems.
  */

  /* Optional */
  /* APN Current and Recommended Preferred System Info */
  uint8_t apn_pref_sys_valid;  /**< Must be set to true if apn_pref_sys is being passed */
  uint32_t apn_pref_sys_len;  /**< Must be set to # of elements in apn_pref_sys */
  dsd_apn_pref_sys_info_type_v01 apn_pref_sys[QMI_DSD_MAX_APNS_V01];
  /**<  \n Array of all available APNs and their current and recommended
         preferred systems.
  */

}qcmap_cm_dsd_status_info_t;
#endif

typedef struct
{
  struct ps_in6_addr *iid_ptr;
}qcmap_cm_prefix_iid_info_t;

/*---------------------------------------------------------------------------
                    Return values indicating error status
---------------------------------------------------------------------------*/
#define QCMAP_CM_SUCCESS               0         /* Successful operation   */
#define QCMAP_CM_ERROR                -1         /* Unsuccessful operation */

/*---------------------------------------------------------------------------
           Error Condition Values
---------------------------------------------------------------------------*/
#define QCMAP_CM_ENOERROR              0        /* No error                */
#define QCMAP_CM_EWOULDBLOCK           1        /* Operation would block   */
#define QCMAP_CM_EINVAL                2        /* Invalid operation       */
#define QCMAP_CM_EOPNOTSUPP            3        /* Operation not supported */
#define QCMAP_CM_EBADAPP               4        /* Invalid application ID  */
#define QCMAP_CM_ENOWWAN               5        /* WWAN not connected      */
#define QCMAP_CM_EALDCONN              6        /* Already connected  */
#define QCMAP_CM_EALDDISCONN           7        /* Already disconnected  */
#define QCMAP_CM_DISCONNECTED          8        /* Disconnected */
#define QCMAP_MSGR_ENTRY_PRESENT       -8
#define QCMAP_MSGR_ENTRY_FULL          -9
#define QCMAP_MSGR_INVALID_PARAM       -10

#define SA_FAMILY(addr)         (addr).sa_family
#define SA_DATA(addr)           (addr).sa_data
#define SASTORAGE_FAMILY(addr)  (addr).ss_family
#define SASTORAGE_DATA(addr)    (addr).__ss_padding

/*---------------------------------------------------------------------------
           Mobile AP Events
---------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_CM_EVENT_ENABLED = 0,
  QCMAP_CM_EVENT_STA_CONNECTED,
  QCMAP_CM_EVENT_STA_DISCONNECTED,
  QCMAP_CM_EVENT_STA_ASSOCIATION_FAIL,
  QCMAP_CM_EVENT_STA_DHCP_IP_ASSIGNMENT_FAIL,
  QCMAP_CM_EVENT_WAN_CONNECTING,
  QCMAP_CM_EVENT_WAN_CONNECTING_FAIL,
  QCMAP_CM_EVENT_WAN_IPv6_CONNECTING_FAIL,
  QCMAP_CM_EVENT_WAN_CONNECTED,
  QCMAP_CM_EVENT_WAN_IPv6_CONNECTED,
  QCMAP_CM_EVENT_WAN_DISCONNECTED,
  QCMAP_CM_EVENT_WAN_IPv6_DISCONNECTED,
  QCMAP_CM_EVENT_WAN_ADDR_RECONF,
  QCMAP_CM_EVENT_WAN_IPv6_ADDR_RECONF,
  QCMAP_CM_EVENT_CRADLE_CONNECTED,
  QCMAP_CM_EVENT_CRADLE_DISCONNECTED,
  QCMAP_CM_EVENT_ETH_BACKHAUL_CONNECTED,
  QCMAP_CM_EVENT_ETH_BACKHAUL_DISCONNECTED,
  QCMAP_CM_EVENT_DISABLED
} qcmap_cm_event_e;

/*---------------------------------------------------------------------------
   Type representing enumeration of QCMAP CM states
---------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_CM_DISABLE = 0,
  QCMAP_CM_ENABLE,
  QCMAP_CM_WAN_CONNECTING,
  QCMAP_CM_WAN_DISCONNECTING,
  QCMAP_CM_WAN_CONNECTED
} qcmap_cm_state_e;

typedef enum
{
  QCMAP_CM_V6_DISABLE = 0,
  QCMAP_CM_V6_ENABLE,
  QCMAP_CM_V6_WAN_CONNECTING,
  QCMAP_CM_V6_WAN_DISCONNECTING,
  QCMAP_CM_V6_WAN_CONNECTED
} qcmap_cm_v6_state_e;

#define QCMAP_CM_IND_V4_WAN_CONNECTED            1002
#define QCMAP_CM_IND_V4_WAN_DISCONNECTED         1003
#define QCMAP_CM_IND_V6_WAN_CONNECTED            1004
#define QCMAP_CM_IND_V6_WAN_DISCONNECTED         1005

/*---------------------------------------------------------------------------
           Mobile AP CM call back declarations.
---------------------------------------------------------------------------*/
typedef void (*qcmap_cm_cb_fcn)
(
  int                 handle,                  /* Mobile AP Application id */
  qcmap_cm_event_e    event,                   /* Type of Mobile AP Event  */
  void               *qcmap_cm_cb_user_data,    /* Call back User data      */
  dsi_ce_reason_t    *callend_reason
);

/*---------------------------------------------------------------------------
  Mobile AP Statistics
---------------------------------------------------------------------------*/
typedef struct
{
  uint64     bytes_rx;
  uint64     bytes_tx;
  uint32     pkts_rx;
  uint32     pkts_tx;
  uint32     pkts_dropped_rx;
  uint32     pkts_dropped_tx;
}qcmap_cm_statistics_t;

/*---------------------------------------------------------------------------
  QCMAP Netlink Events
----------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_NL_NEWNEIGH = 0,
  QCMAP_NL_DELNEIGH,
  QCMAP_NL_NEWADDR,
  QCMAP_NL_PPP_IPV6_ROUTE,
  QCMAP_NL_RS,
  QCMAP_NL_RA,
  QCMAP_NL_NS,
  QCMAP_NL_NA
}qcmap_nl_event_t;

/*---------------------------------------------------------------------------
  QCMAP Netlink Interface
----------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_NL_USB = 0,
  QCMAP_NL_WLAN,
  QCMAP_NL_PPP,
  QCMAP_NL_BRIDGE,
  QCMAP_NL_ETH,
  QCMAP_NL_ANY
}qcmap_nl_iface_t;

/*---------------------------------------------------------------------------
  QCMAP Netlink Addresses
----------------------------------------------------------------------------*/
typedef struct
{
  bool isValidIPv4address;
  bool isValidIPv6address;
  char                  iface_name[QCMAP_MSGR_INTF_LEN];
  uint8                 ip_vsn;
  uint32_t              ip_addr;
  uint8_t               ip_v6_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t               mac_addr[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  uint8                 ipv6_dst_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8                 ipv6_src_addr[QCMAP_MSGR_IPV6_ADDR_LEN_V01];
  uint8_t               prefix_len;
#define MAX_ND_PKT_SIZE 1280
  char                  nd_packet_buf[MAX_ND_PKT_SIZE];
}qcmap_nl_addr_t;

/*------------------------------------------------------------
  QCMAP Netlink Socket Payload
-------------------------------------------------------------*/
typedef struct
{
  qcmap_nl_event_t      nl_event;
  qcmap_nl_iface_t      nl_iface;
  qcmap_nl_addr_t       nl_addr;
} qcmap_nl_sock_msg_t;

/*---------------------------------------------------------------------------
  QCMAP V6 MCAST SOCK IFACE
----------------------------------------------------------------------------*/
typedef enum
{
  QCMAP_V6_SOCK_BRIDGE = 0,
  QCMAP_V6_SOCK_WLAN,
  QCMAP_V6_SOCK_ETH,
  QCMAP_V6_SOCK_ECM
}qcmap_v6_sock_iface_t;


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION start_wlan_sta_timer
===========================================================================*/
/*!
@brief
  Funtion to WLAN STA timer

@return
  QCMAP_CM_SUCCESS
  QCMAP_CM_ERROR

@note
 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
int start_wlan_sta_timer(qcmap_timer_enum_t sta_event);

int readable_addr(int domain,uint32_t *addr, char *str);



/*===========================================================================
  FUNCTION stop_wlan_sta_timer
===========================================================================*/
/*!
@brief
  Funtion to stop WLAN STA timer

@return
  QCMAP_CM_SUCESS
  QCMAP_CM_ERROR

@note

  - Dependencies
  - None

  - Side Effects
  - None
*/
/*=========================================================================*/
int stop_wlan_sta_timer(qcmap_timer_enum_t sta_event);

/*===========================================================================
  FUNCTION stop_auto_timer
===========================================================================*/
/*!
@brief
  Funtion to stop auto timer

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
extern void stop_auto_timer(ip_version_enum_type family);

/*===========================================================================
  FUNCTION qcmap_timer_handler
===========================================================================*/
/*!
@brief
   Function to handle various QCMAP timers
@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
extern void qcmap_timer_handler(int sig, siginfo_t *si, void *uc );

/*===========================================================================
   FUNCTION  qcmap_cm_process_dsi_net_cb_fcn
===========================================================================*/
/*!
@brief
    QCMAP CM DSS net callback

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
extern void qcmap_cm_process_dsi_net_evt
(
dsi_hndl_t hndl,
void * user_data,
dsi_net_evt_t evt,
dsi_evt_payload_t *payload_ptr
);

extern void
qcmap_cm_process_qmi_nas_ind
(
  qmi_client_type user_handle,                    /* QMI user handle       */
  unsigned int    msg_id,                         /* Indicator message ID  */
  void           *ind_buf,                        /* Raw indication data   */
  unsigned int    ind_buf_len,                    /* Raw data length       */
  void           *ind_cb_data                     /* User call back handle */
);

extern void qcmap_cm_process_qmi_timer_ind ( unsigned int msg_id );

extern void
qcmap_cm_process_sta_ind
(
  uint32 sta_cookie,
  qcmap_sta_event_t event
);

extern void
qcmap_cm_process_cradle_ind
(
  qcmap_cradle_event_t event
);

extern void
qcmap_cm_process_eth_backhaul_ind
(
  qcmap_eth_backhaul_event_t event
);
/*===========================================================================

FUNCTION QCMAP_CM_ENABLE()

DESCRIPTION

  Enable Mobile AP CM based on the config.
  It will register Mobile AP event callback.
  It will configure Modem in Mobile AP Mode and bring up RmNet between Q6 and A5.
  It will also bring up LAN if it is config.

DEPENDENCIES
  None.

RETURN VALUE
  Returns Mobile AP CM application ID on success.

  On error, return 0 and place the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_enable
(
  qcmap_cm_conf_t   *qcmap_cm_cfg,              /* Config for Mobile AP CM */
  qcmap_cm_cb_fcn    qcmap_cm_cb,               /* Callback function       */
  void              *qcmap_cm_cb_user_data,     /* Callback user data      */
  int               *qcmap_cm_errno,            /* Error condition value   */
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================

FUNCTION QCMAP_CM_DISABLE()

DESCRIPTION

  Disable Mobile AP CM.
  It will teardown LAN.
  It will configure Modem in non-Mobile AP mode.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_disable
(
  int    qcmap_cm_handle,                      /* Handle for Mobile AP CM  */
  int   *qcmap_cm_errno,                       /* Error condition value    */
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================

FUNCTION QCMAP_CM_ENABLE_IPV6()

DESCRIPTION
  Enable IPV6 Functionality.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS
  None.

===========================================================================*/
int qcmap_cm_enable_ipv6
(
  int    qcmap_cm_handle,                         /* Handler for QCMAP CM  */
  qmi_error_type_v01 *qmi_err_num                 /* error condition value */
);


/*===========================================================================

FUNCTION QCMAP_CM_DISABLE_IPV6()

DESCRIPTION
  Disable IPV6 Functionality.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS
  None.

===========================================================================*/
int qcmap_cm_disable_ipv6
(
  int    qcmap_cm_handle,                           /* Handle for QCMAP CM  */
  int     *qcmap_cm_errno,
  qmi_error_type_v01 *qmi_err_num                   /* error condition value */
);

/*===========================================================================

FUNCTION QCMAP_CM_ENABLE_IPV4()

DESCRIPTION
  Enable IPV4 backhaul Functionality.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS
  None.

===========================================================================*/
int qcmap_cm_enable_ipv4
(
  int    qcmap_cm_handle,                         /* Handler for QCMAP CM  */
  qmi_error_type_v01 *qmi_err_num                 /* error condition value */
);


/*===========================================================================

FUNCTION QCMAP_CM_DISABLE_IPV4()

DESCRIPTION
  Disable IPV4 Functionality.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS
  None.

===========================================================================*/
int qcmap_cm_disable_ipv4
(
  int    qcmap_cm_handle,                           /* Handle for QCMAP CM  */
  int     *qcmap_cm_errno,
  qmi_error_type_v01 *qmi_err_num                   /* error condition value */
);

/*===========================================================================

FUNCTION QCMAP_CM_CONNECT_BACKHAUL()

DESCRIPTION

  It will bringup WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already connected, returns QCMAP_CM_SUCCESS.
  return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

  qcmap_cm_errno Values
  ----------------
  QCMAP_CM_EBADAPP           invalid application ID specified
  QCMAP_CM_EWOULDBLOCK       the operation would block
  QCMAP_CM_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_connect_backhaul
(
  int    qcmap_cm_handle,                      /* Handle for Mobile AP CM  */
  qcmap_msgr_wwan_call_type_v01    call_type,  /* Call type to be brought UP. */
  int    *qcmap_cm_errno,                       /* Error condition value    */
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================

FUNCTION QCMAP_CM_DISCONNECT_BACKHAUL()

DESCRIPTION

  It will teardown WWAN.

DEPENDENCIES
  None.

RETURN VALUE
  If WAN is already disconnected, returns QCMAP_CM_SUCCESS.
  return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

  qcmap_cm_errno Values
  ----------------
  QCMAP_CM_EBADAPP           invalid application ID specified
  QCMAP_CM_EWOULDBLOCK       the operation would block
  QCMAP_CM_EOPNOTSUPP        backhaul bringup/teardown in progress


SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_disconnect_backhaul
(
  int    qcmap_cm_handle,                      /* Handle for Mobile AP CM  */
  qcmap_msgr_wwan_call_type_v01    call_type, /* Call type to be brought down. */
  int    *qcmap_cm_errno,                       /* error condition value    */
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================

FUNCTION QCMAP_CM_GET_STATE()

DESCRIPTION

  It will get QCMAP CM current state.

DEPENDENCIES
  None.

SIDE EFFECTS

===========================================================================*/

extern qcmap_cm_state_e qcmap_cm_get_state
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================

FUNCTION QCMAP_CM_GET_IPV6_STATE()

DESCRIPTION

  It will get QCMAP CM current IPV6 state.

DEPENDENCIES
  None.

SIDE EFFECTS

===========================================================================*/

extern qcmap_cm_v6_state_e qcmap_cm_get_ipv6_state
(
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================
  FUNCTION:  qcmap_cm_write_xml

  DESCRIPTION

  This function write QCMAP CM XML based on QCMAP CM Cfg

DEPENDENCIES
  None.

RETURN VALUE

  QCMAP_CM_ERROR
  QCMAP_CM_SUCCESS

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_write_xml
(
  char *xml_file,                                 /* Filename and path     */
  qcmap_cm_conf_t *config                         /* Mobile AP config data */
);

/*===========================================================================
FUNCTION:  qcmap_cm_write_firewall_xml

DESCRIPTION

  This function write QCMAP CM Firewall XML based on QCMAP CM Cfg

DEPENDENCIES
  None.

RETURN VALUE

  QCMAP_CM_ERROR
  QCMAP_CM_SUCCESS

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_write_firewall_xml
(
  char *xml_file,                                 /* Filename and path     */
  qcmap_cm_conf_t *config                         /* Mobile AP config data */
);

/*===========================================================================
FUNCTION:  qcmap_cm_read_xml

DESCRIPTION

  This function read QCMAP CM XML and populate the QCMAP CM Cfg

DEPENDENCIES
  None.

RETURN VALUE

  QCMAP_CM_ERROR
  QCMAP_CM_SUCCESS

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_read_xml
(
  char *xml_file,                                 /* Filename and path     */
  qcmap_cm_conf_t *config                         /* Mobile AP config data */
);


/*===========================================================================
FUNCTION:  qcmap_cm_read_odu_xml

DESCRIPTION

  This function read QCMAP CM XML and populate the QCMAP CM Cfg

DEPENDENCIES
  None.

RETURN VALUE

  QCMAP_CM_ERROR
  QCMAP_CM_SUCCESS

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_read_odu_xml
(
  char *xml_file,                                 /* Filename and path     */
  int  *mode                                      /* ETH Mode */
);


/*===========================================================================
FUNCTION:  qcmap_cm_get_ecm_cat_ipa_xml

DESCRIPTION

  This function reads IPA XML and populate the category

DEPENDENCIES
  None.

RETURN VALUE

  QCMAP_CM_ERROR
  QCMAP_CM_SUCCESS

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_get_ecm_cat_ipa_xml
(
  char *xml_file,                                 /* Filename and path     */
  int  *category                                      /* ETH Mode */
);

/*===========================================================================

FUNCTION:  qcmap_cm_read_firewall_xml

DESCRIPTION

  This function reads QCMAP CM Firewall XML and populate the QCMAP CM Cfg

DEPENDENCIES
  None.

RETURN VALUE

  QCMAP_CM_ERROR
  QCMAP_CM_SUCCESS

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_read_firewall_xml
(
  char *xml_file,                                 /* Filename and path     */
  qcmap_cm_conf_t *config                         /* Mobile AP config data */
);


/*===========================================================================

FUNCTION QCMAP_CM_SET_AUTO_CONNECT()

DESCRIPTION

  It will set autoconnect.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_cm_errno Values
  ----------------
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_set_auto_connect
(
  int    qcmap_cm_handle,                     /* Handle for Mobile AP CM   */
  int    *qcmap_cm_errno,                     /* Error condition value     */
  boolean auto_connect,                        /* Autoconnect Enable or Not */
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================

FUNCTION QCMAP_CM_SET_ROAMING()

DESCRIPTION

  It will set roaming.

DEPENDENCIES
  None.

RETURN VALUE

  qcmap_cm_errno Values
  ----------------
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_set_roaming
(
  int    qcmap_cm_handle,                      /* Handle for Mobile AP CM  */
  qmi_error_type_v01 *qmi_err_num,             /* Error condition value    */
  boolean roaming                              /* Roaming Enable or Not    */
);


/*===========================================================================

FUNCTION QCMAP_CM_GET_IPV4_NET_CONF()

DESCRIPTION
  This Function get the ipv4 WWAN network configuration.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_get_ipv4_net_conf
(
  int     qcmap_cm_handle,                     /* Handle for Mobile AP CM  */
  in_addr_t *public_ip,                           /* IP addr assigned to WWAN */
  uint32 *pri_dns_addr,                        /* Primary DNS IP address   */
  in_addr_t *sec_dns_addr,                        /* Secondary DNS IP address */
  in_addr_t *gw_addr,                             /* UM Iface default gw address */
  qmi_error_type_v01 *qmi_err_num
);

/*===========================================================================

FUNCTION QCMAP_CM_GET_IPV6_NET_CONF()

DESCRIPTION
  This Function get the IPv6 WWAN network configuration.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_get_ipv6_net_conf
(
  int                qcmap_cm_handle,   /* Handle for Mobile AP CM  */
  uint8_t            public_ip[],       /* IP addr assigned to WWAN */
  uint8_t            pri_dns_addr[],    /* Primary DNS IP address   */
  uint8_t            sec_dns_addr[],    /* Secondary DNS IP address */
  qmi_error_type_v01 *qmi_err_num
);


/*===========================================================================

FUNCTION QCMAP_CM_GET_DEV_NAME()

DESCRIPTION
  This function gets the rmnet device name for a dsi handle.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_get_dev_name
(
  int     qcmap_cm_handle,                     /* Handle for Mobile AP CM  */
  qcmap_msgr_ip_family_enum_v01
          qcmap_dev_type, /* Dev type                 */
  char   *qcmap_dev_name,                      /* Device name              */
  int    *qcmap_cm_errno                       /* Error condition value    */
);

/*===========================================================================

FUNCTION QCMAP_CM_ENABLE_STA_MODE()

DESCRIPTION
  Enable QCMobileAP CM based on the config.
  It will register QCMAP event callback.
  It will configure Modem in QCMAP Mode and bring up RmNet between Q6 and A5.
  It will also bring up LAN if it is config.

DEPENDENCIES
  None.

RETURN VALUE
  Returns QCMobileAP CM application ID on success.
  On error, return 0 and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS
  None.

===========================================================================*/
int qcmap_cm_enable_sta_mode
(
  int    qcmap_cm_handle,                         /* Handler for QCMAP CM  */
  int   *qcmap_cm_errno                           /* error condition value */
);


/*===========================================================================

FUNCTION QCMAP_CM_DISABLE_STA_MODE()

DESCRIPTION
  Disable QCMAP CM.
  Send disable_sta_mode msg to modem.

DEPENDENCIES
  None.

RETURN VALUE
  Returns QCMAP CM application ID on success.
  On error, return 0 and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS
  None.

===========================================================================*/
int qcmap_cm_disable_sta_mode
(
  int    qcmap_cm_handle,                           /* Handle for QCMAP CM  */
  int   *qcmap_cm_errno                             /* error condition value */
);

/*===========================================================================


FUNCTION QCMAP_CM_ERI_READ_CONFIG()

DESCRIPTION
  This function reads the passed file name to store the ERI config for
  processing.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

=============================================================================*/
int
qcmap_cm_eri_read_config
(
  int    qcmap_cm_handle,                           /* Mobile AP CM Handle   */
  char  *file_name,                                 /* ERI config file       */
  int   *qcmap_cm_errno                             /* error condition value */
);

/*===========================================================================

FUNCTION QCMAP_CM_GET_WWAN_STATISTICS()

DESCRIPTION
  This Function gets WWAN statistics

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_get_wwan_statistics
(
  int                             qcmap_cm_handle,/*Handle for Mobile AP CM*/
  qcmap_msgr_ip_family_enum_v01   ip_family,     /* V4 or V6 family        */
  qcmap_msgr_wwan_statistics_type_v01 *wwan_stats,    /* WWAN Statistics values */
  qmi_error_type_v01             *qmi_err_num
);

/*===========================================================================

FUNCTION QCMAP_CM_RESET_WWAN_STATISTICS()

DESCRIPTION
  This Function resets WWAN statistics

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.
  On error, return QCMAP_CM_ERROR and places the error condition value in
  *qcmap_cm_errno.

SIDE EFFECTS

===========================================================================*/
extern int qcmap_cm_reset_wwan_statistics
(
  int                             qcmap_cm_handle,/*Handle for Mobile AP CM*/
  qcmap_msgr_ip_family_enum_v01   ip_family,     /* V4 or V6 family        */
  qmi_error_type_v01             *qmi_err_num
);

/*===========================================================================

FUNCTION COEX_GET_WWAN_STATUS()

DESCRIPTION
  This Function retrieves the LTE frequency used by WiFi-LTE coex feature.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns TRUE.
  On error, return FALSE.

SIDE EFFECTS

===========================================================================*/
boolean coex_get_wwan_status
(
int                              *lte_frequency
);
/*===========================================================================

FUNCTION QCMAP_CM_SET_WWAN_POLICY()

DESCRIPTION
  This Function gets WWAN profile index
DEPENDENCIES
  None.

RETURN VALUE
  On success, returns QCMAP_CM_SUCCESS.

SIDE EFFECTS

===========================================================================*/

int qcmap_cm_set_wwan_policy
(
  int                             qcmap_cm_handle,/*Handle for Mobile AP CM*/
  qcmap_msgr_net_policy_info_v01 wwan_policy,  /* WWAN config values */
  qmi_error_type_v01             *qmi_err_num
);
/*===========================================================================

FUNCTION QCMAP_CM_GET_DATA_BITRATE()

DESCRIPTION

  Returns the current data bitrates.

DEPENDENCIES
  None.

SIDE EFFECTS

===========================================================================*/

extern int qcmap_cm_get_data_bitrate
(
  int                             qcmap_cm_handle,/*Handle for Mobile AP CM*/
  qcmap_msgr_data_bitrate_v01    *data_rate,    /* WWAN Statistics values */
  qmi_error_type_v01             *qmi_err_num
);


/*===========================================================================

FUNCTION QCMAP_CM_GET_IPV4_SIP_SERVER_INFO()

DESCRIPTION
 - Calls into dsi_netctrl IOCTL to get PCSCF address list and
   PCSCF FQDN list

DEPENDENCIES
  None.

SIDE EFFECTS

===========================================================================*/
int qcmap_cm_get_ipv4_sip_server_info
(
  int                             qcmap_cm_handle, /*Handle for Mobile AP CM*/
  dsi_pcscf_addr_info_t           * pcscf_addr_list,/*PCSCF address list*/
  dsi_pcscf_fqdn_list_t           * pcscf_fqdn_list,/*PCSCF FQDN list*/
  qmi_error_type_v01              *qmi_err_num /*QMI error number*/
);

/*===========================================================================

FUNCTION QCMAP_CM_GET_IPV6_SIP_SERVER_INFO()

DESCRIPTION
 - Calls into dsi_netctrl IOCTL to get PCSCF address list and
   PCSCF FQDN list

DEPENDENCIES
  None.

SIDE EFFECTS

===========================================================================*/

int  qcmap_cm_get_ipv6_sip_server_info
(
  int                             qcmap_cm_handle,/*Handle for Mobile AP CM*/
  dsi_pcscf_addr_info_t           *pcscf_addr_list,/*PCSCF address list*/
  dsi_pcscf_fqdn_list_t           *pcscf_fqdn_list,/*PCSCF FQDN list*/
  qmi_error_type_v01              *qmi_err_num /*QMI error number*/
);

/*===========================================================================

FUNCTION qcmap_cm_process_dsi_init_ind()

DESCRIPTION
 - Once the DSI is initialized, It sends an initication to QCMAP, to handle
   backhaul conection request.

DEPENDENCIES
  None.

SIDE EFFECTS

===========================================================================*/
void qcmap_cm_process_dsi_init_ind(void);

/*===========================================================================

FUNCTION qcmap_cm_dsi_net_init_cb()

DESCRIPTION
 - Its a call back function passed to dsi_init_ex. After dsi is initialized,
   this call back function will be called.

DEPENDENCIES
  None.

SIDE EFFECTS

===========================================================================*/
void qcmap_cm_dsi_net_init_cb(void);

/*===========================================================================

FUNCTION QCMAP_CM_DELETE_IPV6_DELEGATED_PREFIX()

DESCRIPTION
 - Removes a single prefix if prefix_valid is set, othwise removes all
 delegated prefix's

DEPENDENCIES
  None.

SIDE EFFECTS

===========================================================================*/

/*===========================================================================

FUNCTION Getclientaddr

DESCRIPTION

  This function
  - Uses mac address to find out the public ip address of the connect device.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None
==========================================================================*/
void Getclientaddr(qcmap_nl_addr_t* nl_addr);

int qcmap_cm_delete_ipv6_delegated_prefix
(
  boolean                         prefix_valid,/*Boolean to flush single or all*/
  uint8_t                         *ipv6_addr,/*Prefix to delete*/
  qmi_error_type_v01              *qmi_err_num /*QMI error number*/
);

/*===========================================================================
LOG Msg Macros
=============================================================================*/
#ifdef FEATURE_QTIMAP_OFFTARGET

#undef LOG_MSG_INFO1
#define LOG_MSG_INFO1(fmtString, x, y, z) \
	{ \
	  if ( x != 0 && y !=0 && z != 0) \
		printf("\nINFO1:"fmtString"\n", x, y, z); \
	  else if ( x != 0 && y != 0) \
		printf("\nINFO1:"fmtString"\n", x, y); \
	  else if ( x != 0) \
		printf("\nINFO1:"fmtString"\n", x); \
	  else \
		printf("\nINFO1:"fmtString"\n"); \
	}
#undef LOG_MSG_INFO2
#define LOG_MSG_INFO2(fmtString, x, y, z) \
	{ \
	  if ( x != 0 && y !=0 && z != 0) \
		printf("\nINFO1:"fmtString"\n", x, y, z); \
	  else if ( x != 0 && y != 0) \
		printf("\nINFO1:"fmtString"\n", x, y); \
	  else if ( x != 0) \
		printf("\nINFO1:"fmtString"\n", x); \
	  else \
		printf("\nINFO1:"fmtString"\n"); \
	}

#undef LOG_MSG_INFO3
#define LOG_MSG_INFO3(fmtString, x, y, z) \
	{ \
	  if ( x != 0 && y !=0 && z != 0) \
		printf("\nINFO3:"fmtString"\n", x, y, z); \
	  else if ( x != 0 && y != 0) \
		printf("\nINFO3:"fmtString"\n", x, y); \
	  else if ( x != 0) \
		printf("\nINFO3:"fmtString"\n", x); \
	  else \
		printf("\nINFO3:"fmtString"\n"); \
	}

#undef LOG_MSG_ERROR
#define LOG_MSG_ERROR(fmtString, x, y, z) \
	{ \
	  if ( x != 0 && y !=0 && z != 0) \
		printf("\nError:"fmtString"\n", x, y, z); \
	  else if ( x != 0 && y != 0) \
		printf("\nError:"fmtString"\n", x, y); \
	  else if ( x != 0) \
		printf("\nError:"fmtString"\n", x); \
	  else \
		printf("\nError:"fmtString"\n"); \
	}
#undef IPV4_ADDR_MSG
#define IPV4_ADDR_MSG(ip_addr) LOG_MSG_INFO1("IPV4 Address is %d.%d.%d.", \
							(unsigned char)(ip_addr), \
							(unsigned char)(ip_addr >> 8), \
							(unsigned char)(ip_addr >> 16))
#undef IPV6_ADDR_MSG
#define IPV6_ADDR_MSG(ip_addr) LOG_MSG_INFO1("IPV6 Address is %x.%x.%x.", \
							(uint16)(ps_ntohs(ip_addr[0])), \
							(uint16)(ps_ntohs(ip_addr[0] >> 16)), \
							(uint16)(ps_ntohs(ip_addr[0] >> 32)))
#else /* FEATURE_QCMAP_OFFTARGET */
#define LOG_MSG_INFO1_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO2_LEVEL           MSG_LEGACY_MED
#define LOG_MSG_INFO3_LEVEL           MSG_LEGACY_LOW
#define LOG_MSG_ERROR_LEVEL           MSG_LEGACY_ERROR
#define PRINT_MSG( level, fmtString, x, y, z)                         \
        MSG_SPRINTF_4( MSG_SSID_LINUX_DATA, level, "%s(): " fmtString,      \
                       __FUNCTION__, x, y, z);
#define LOG_MSG_INFO1( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO1_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO2( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO2_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_INFO3( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_INFO3_LEVEL, fmtString, x, y, z);                \
}
#define LOG_MSG_ERROR( fmtString, x, y, z)                            \
{                                                                     \
  PRINT_MSG( LOG_MSG_ERROR_LEVEL, fmtString, x, y, z);                \
}


#define IPV6_ADDR_MSG(ip_addr) MSG_8(MSG_SSID_DS, \
                        MSG_LEGACY_HIGH, \
                        "IPV6 Address %x:%x:%x:%x:%x:%x:%x:%x", \
                        (uint16)(ps_ntohs(ip_addr[0])), \
                        (uint16)(ps_ntohs(ip_addr[0] >> 16)), \
                        (uint16)(ps_ntohs(ip_addr[0] >> 32)) , \
                        (uint16)(ps_ntohs(ip_addr[0] >> 48)), \
                        (uint16)(ps_ntohs(ip_addr[1])), \
                        (uint16)(ps_ntohs(ip_addr[1] >> 16)), \
                        (uint16)(ps_ntohs(ip_addr[1] >> 32)) , \
                        (uint16)(ps_ntohs(ip_addr[1] >> 48)))


#define IPV4_ADDR_MSG(ip_addr) MSG_4(MSG_SSID_DS, \
                        MSG_LEGACY_HIGH, \
                        "IPV4 Address is %d.%d.%d.%d", \
                        (unsigned char)(ip_addr), \
                        (unsigned char)(ip_addr >> 8), \
                        (unsigned char)(ip_addr >> 16) , \
                        (unsigned char)(ip_addr >> 24))
#endif

/*Other required utilities*/

#define PS_IN6_IS_ADDR_UNSPECIFIED(v6)                                      \
  (((const unsigned int *) (v6))[0] == 0 &&                                 \
   ((const unsigned int *) (v6))[1] == 0 &&                                 \
   ((const unsigned int *) (v6))[2] == 0 &&                                 \
   ((const unsigned int *) (v6))[3] == 0)

inline const char* WLAN_IFACE_STRING(int num){
  switch ( num )
  {
  case 0:
    return "wlan0";
  case 1:
    return "wlan1";
  case 2:
    return "wlan2";
  case 3:
    return "wlan3";
  }
}


#ifdef __cplusplus
}
#endif
#endif
