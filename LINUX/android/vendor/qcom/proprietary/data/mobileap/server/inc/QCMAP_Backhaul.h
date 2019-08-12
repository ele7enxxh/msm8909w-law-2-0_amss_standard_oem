#ifndef _QCMAP_BACKHAUL_H_
#define _QCMAP_BACKHAUL_H_

/*====================================================

FILE:  QCMAP_Backhaul.h

SERVICES:
   QCMAP Connection Manager Backhaul Class

=====================================================

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
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
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_Tethered_Backhaul.h"
#include "QCMAP_Backhaul_Ethernet.h"

#define WWAN_BACKHAUL          "wwan"
#define USB_CRADLE_BACKHAUL    "usb_cradle"
#define WLAN_BACKHAUL          "wlan"
#define ETH_BACKHAUL           "eth"
#define MULTICAST_ADDR         "FF02::1"



typedef enum
{
CONFIG_ENABLE_IPV4 = 1,
CONFIG_ENABLE_IPV6,
CONFIG_WEBSERVER_WWAN_ACCESS,
CONFIG_ERI_CONFIG,
CONFIG_FIRST_PREFFERED_BACKHAUL,
CONFIG_SECOND_PREFFERED_BACKHAUL,
CONFIG_THIRD_PREFFERED_BACKHAUL,
CONFIG_FOURTH_PREFFERED_BACKHAUL
}qcmap_backhaul_config_enum;


class QCMAP_Backhaul
{
private:
   static bool flag;
   static QCMAP_Backhaul *object;
   QCMAP_Backhaul();


public:
    //Function to get the instance of the class
    static QCMAP_Backhaul *Get_Instance(boolean obj_create=false);
    //Destructor of the class
   ~QCMAP_Backhaul();
    /* ----------------------MobileAP Backhaul Execution--------------------*/

  //ALL VARIABLES HERE//

   /*Priority list for Backhaul Switching*/
   static qcmap_backhaul_type         preffered_backhaul_first;
   static qcmap_backhaul_type         preffered_backhaul_second;
   static qcmap_backhaul_type         preffered_backhaul_third;
   static qcmap_backhaul_type         preffered_backhaul_fourth;
   /*Current Backhaul*/
   static qcmap_backhaul_type         current_backhaul;

   qcmap_addr_info_list_t external_client_addr_list;

    bool enable_ipv6;
    bool enable_ipv4;
  qcmap_cm_nl_prefix_info_t  ipv6_prefix_info;
  qcmap_cm_wan_conf_t wan_cfg;
    /* Holds IPv4 public address of a connected interface*/
    in_addr_t ipv4_public_ip;
    /* Holds IPv4 gateway address of a connected interface*/
    in_addr_t ipv4_default_gw_addr;
    /*variable to hold tinyproxy state*/
    qcmap_msgr_tiny_proxy_mode_enum_v01  tiny_proxy_enable_state;

  static boolean GetSetBackhaulConfigFromXML
                                              ( qcmap_backhaul_config_enum conf,
  qcmap_action_type action,
                                                char *data,int datalen );


  static boolean SetBackhaulPrioConfigToXML(
                            qcmap_msgr_backhaul_type_enum_v01 backhaul_type,
                            qcmap_backhaul_config_enum backhaul_config);


    //Read Backhaul Config from XML
     boolean ReadBackhaulConfigFromXML();
   static boolean GetNetworkConfig(in_addr_t *public_ip,
                                uint32 *primary_dns,
                                in_addr_t *secondary_dns,
                                qmi_error_type_v01 *qmi_err_num);

   static boolean GetIPv6NetworkConfig(uint8_t public_ip[],
                                    uint8_t primary_dns[],
                                    uint8_t secondary_dns[],
                                    qmi_error_type_v01 *qmi_err_num);

   /* Enable IPV4 */
    static boolean EnableIPV4
    (qmi_error_type_v01 *qmi_err_num);
   /* Disable IPV4 */
   static boolean DisableIPV4(qmi_error_type_v01 *qmi_err_num);

   /* Enable IPV6 */
   static boolean EnableIPV6(qmi_error_type_v01 *qmi_err_num);
   /* Disable IPV6 */
   static boolean DisableIPV6(qmi_error_type_v01 *qmi_err_num);
   /* Validates the prefix present in RA packet. */
    boolean ValidateGlobalV6addr(struct ps_in6_addr *);
   /* Enable IPV6 Forwarding */
   boolean EnableIPV6Forwarding(void);
   /* Disable IPV6 Forwarding */
   boolean DisableIPV6Forwarding(boolean send_ra);
   /* Get Backhaul IPV6 Prefix Info. */
   void GetIPV6PrefixInfo(char *devname,
                          qcmap_cm_nl_prefix_info_t   *ipv6_prefix_info);

   /* Update Global Ipv6 from Received RA message */
   static void UpdateGlobalV6addr(qcmap_nl_addr_t* nl_addr);

  /* Assign Global Ipv6 address to all client */
    bool AssignGlobalV6AddrAllClients
                                        ( struct ps_in6_addr *prefix_addr_ptr );
    int UpdatePrefix( qcmap_cm_nl_prefix_info_t *prefix_info,
                          boolean deprecate,boolean send_ra,
                          uint8_t *dest_v6_ip );
   static boolean SendRS(char *devname, uint8_t *src_v6_addr);
   boolean SendRSForClient(uint8_t*);
   boolean SendRSForClients();
   static void SendRSwithUnspecifiedSrc(qcmap_nl_addr_t* nl_addr);
   static boolean IsNonWWANBackhaulActive();
   void StartRadish(void);
   void StopRadish(void);
   void RestartRadish(void);
   boolean IsRadishRunning(void);
    static boolean GetIPv4State( uint8_t *status,
                                     qmi_error_type_v01 *qmi_err_num );
    static boolean GetIPv6State( uint8_t *status,
                                     qmi_error_type_v01 *qmi_err_num );

   void DeletePPPIPv6Route();
   static boolean IsPPPkilled();
   void AddPPPIPv6Route();
   boolean BlockIPv4WWANAccess();
   boolean BlockIPv6WWANAccess();
   /*Deleting the Global Ipv6 address when the Ipv6 call is brought down*/
   boolean DeleteGlobalipv6AddressInfo ();
   boolean DeprecateClientRA(void);
   boolean GetIP(uint32 *staIP, uint32 *netMask, char *iface);

   /* Configure Webserver WWAN Access. */
    static boolean SetWebserverWWANAccess
                                             ( boolean enable,
                                               qmi_error_type_v01 *qmi_err_num );

    boolean SetWebserverWWANAccessOnApps
                                             ( boolean enable,
                                               qmi_error_type_v01 *qmi_err_num );

    static boolean GetWebserverWWANAccessFlag
                                              ( uint8 *flag,
                                                qmi_error_type_v01 *qmi_err_num );
    boolean ComparePriority(qcmap_backhaul_type intf1,qcmap_backhaul_type intf2);
    void BlockWWANAccessOnIface(ip_version_enum_type ip_vsn, char *devname);
    void AllowBackhaulAccessOnIface(ip_version_enum_type ip_vsn, char *devname);
    void ClearIfaceRelatedRulesForV6(char *devname);
    void ClearIfaceRelatedRulesForV4(char *devname);
    boolean QCMAP_Backhaul::RemoveIPV6Address(char *devname);


    void StopDHCPCD(char* iface);

    /* Send RS on Bridge Iface*/
    void SendRSOnBridgeIface(void);

    /* IPv6 Proxy functions */
    static void ProcessNS(qcmap_nl_addr_t* nl_addr);
    static void ProcessNA(qcmap_nl_addr_t* nl_addr);
    void SendProxyNS(char *nd_packet_buf);
    void SendProxyNA(struct ps_in6_addr *dst_v6_addr, char *nd_packet_buf, uint8 *src_mac_addr);

    /* Router mode External Routing Functions */
    static void AddV6ExternalRoute(qcmap_nl_addr_t* nl_addr, char* iface);
    static void DeleteV6ExternalRoute(uint8_t* v6_addr, char* iface);
    static void DeleteAllV6ExternalRoute(char* iface);

    /*Join Multicast group*/
    static void JoinMulticastGroup (qcmap_v6_sock_iface_t iface_type, char* iface);

   /* Add default gateway information from RA*/
   void AddIPv6DefaultRouteInfo(qcmap_nl_addr_t *nladdr);
   void SwitchToWWANBackhaul();
   static void SwitchToOtherBackhaul(
                                   qcmap_backhaul_type backhaul_type,
                                   boolean allow_ipv4,
                                   boolean allow_ipv6);
   static boolean SwitchToBackhaul(qcmap_backhaul_type backhaul_type);
};
#endif
