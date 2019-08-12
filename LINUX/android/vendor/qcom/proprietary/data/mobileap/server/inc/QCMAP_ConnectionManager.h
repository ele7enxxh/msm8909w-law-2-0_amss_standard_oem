#ifndef _QCMAP_CONNECTION_MANAGER_H_
#define _QCMAP_CONNECTION_MANAGER_H_

/*====================================================

FILE:  QCMAP_ConnectionManager.h

SERVICES:
   QCMAP Connection Manager Class

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/11/12   gk         9x25
  10/26/12   cp         Added support for Dual AP and different types of NAT.
  02/27/13   cp         Added support for deprecating of prefix when switching
                        between station mode and WWAN mode.
  04/16/13   mp         Added support to get IPv6 WWAN/STA mode configuration.
  06/12/13   sg         Added DHCP Reservation feature
  09/17/13   at         Added support to Enable/Disable ALGs
  01/11/14   sr         Added support for connected devices in SoftAP
  02/24/14   vm         Changes to Enable/Disable Station Mode to be in
                        accordance with IoE 9x15
  03/27/14   cp         Added support to DUN+SoftAP.
  06/06/14   rk         Added support DHCP option 26 for mtu in dnsmasq/dhcp server.
  08/08/14   rk         kw fixes.
=====================================================================*/

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

//===================================================================
//              Class Definitions
//===================================================================

#define IPV6_NEIGH_ADDR_TABLE                  "/etc/ipv6neighaddr.txt"
#define TEMP_HOST_FILE "/tmp/dnsmasq_host.txt"

#ifdef FEATURE_DATA_TARGET_MDM9607
#define QMI_MAX_TIMEOUT_MS                     10000
#define USB_CLK_UPDATE_NODE                    "/sys/class/udc/msm_hsusb/device/perf_mode"
#endif

class QCMAP_ConnectionManager
{
  private:
    static bool flag;
   static QCMAP_ConnectionManager* object;
   QCMAP_ConnectionManager();
  public:
    bool DUN_SoftAP_enable_flag;
    bool dual_wifi_mode;

    // Data Structure to hold AP-STA v6 bridge info for NS/NA proxy
    qcmap_ap_sta_bridge_t ap_sta_bridge;

    /* AutoConnect */
    boolean  auto_connect;
    boolean  auto_connect_timer_running_v4;
    timer_t  timerid_v4;
    int      auto_timer_value_v4;
    boolean  auto_connect_timer_running_v6;
    timer_t  timerid_v6;
    int      auto_timer_value_v6;

    int       wlan_restart_delay;

    /* Use for Signal the LAN/WAN connected */
    pthread_mutex_t             cm_mutex;
    pthread_cond_t              cm_cond;
    /* STA iface device number */
    int       sta_iface_index;
    /* AP iface device numbers */
    int       ap_dev_num1;
    int       ap_dev_num2;
    /*To track qcmap tear down*/
    static boolean                     qcmap_tear_down_in_progress;
    /* To check whether bridge interface is initialized or not. */
    /* Socket used for sending RA's to deprecate prefix. */
    int                         bridge_sock;
    /* This variable is used to revert back to ap-sta router mode when
       a failure occurs in ap-sta bridge mode */
    boolean revert_ap_sta_router_mode;
    /*Connected Device Info storage*/
    qcmap_cm_connected_info_t  conn_device_info;
    qcmap_addr_info_list_t addrList;
    qcmap_addr_info_list_t addrList_prev;
    /* XML file name */
    static char xml_path[QCMAP_CM_MAX_FILE_LEN];
    static char firewall_xml_path[QCMAP_CM_MAX_FILE_LEN];
    /* QCMAP CM Config */
    boolean           qcmap_enable;
    qcmap_cm_conf_t   cfg;
    /* Below variable is being used in GetWWANMTUInfo() which is invoked during constructor
           execution and this has to be referenced using dereference operator.*/
    static int    qcmap_cm_handle;

    /* QCMAP CM CB and Client Information Ref Ptr */
    unsigned int* qcmap_cb_handle;

#ifdef FEATURE_DATA_TARGET_MDM9607

    bool usb_clk_node_present;

    /* DSD Client Handle*/
    qmi_client_type qmi_dsd_client_handle;

#endif

    inline unsigned int* GetQCMAPClientCBDatata(void)
           { return this->qcmap_cb_handle; }

    static QCMAP_ConnectionManager *Get_Instance\
                                  (char *xml_path = NULL,boolean obj_create =0);
    ~QCMAP_ConnectionManager();

    /* ---------------------------MobileAP Execution---------------------------*/

    /* Enable MobileAP */
    static boolean Enable(int *handle, void *cb_user_data, \
                                               qmi_error_type_v01 *qmi_err_num);

    /* Disable MobileAP */
    static boolean Disable(int *err_num, qmi_error_type_v01 *qmi_err_num);
    static boolean CheckDUNSoftAPMode(void);

    /* Intialize Bridge Interface. */
    void InitBridge();
    int CreateRawSocket(char *devname);
    void DelBridge();

    /* Disable MobileAP handle. */
    void DisableHandle(void);

    /* ----------------------------Modem Config-------------------------*/

    /* Display the current modem configuration. */
    boolean DisplayModemConfig(void);

    /* -----------------------------Linux Config--------------------------*/

    static void CleanIPtables(void);
    static void CleanEbtables(void);


    /*Handle Connected Device events*/
    static bool AddNewDeviceEntry
    (
      void* mac_addr,
      qcmap_msgr_device_type_enum_v01 device_type,
      uint32* ip_addr,
      uint8* ipv6_addr,
      bool isIpv4Valid = false,
      bool isIPv6Valid = false
    );
    /* Start, stop and restart the linux MCAST routing daemon. */
    void StartMcastDaemon(void);
    void StopMcastDaemon(void);
    void  strtolower(uint8 *str);

    int readable_addr(int domain,const uint32_t *addr, char *str);

    static int GetMobileAPhandle(qmi_error_type_v01 *qmi_err_num);

    /* Set/get QCMAP bootup fucntions*/
    static boolean SetQCMAPBootupConfig
    (
      qcmap_msgr_bootup_flag_v01 mobileap_enable,
      qcmap_msgr_bootup_flag_v01 wlan_enable,
      qmi_error_type_v01 *qmi_err_num
    );

    static boolean GetQCMAPBootupConfig
    (
      qcmap_msgr_bootup_flag_v01 *mobileap_enable,
      qcmap_msgr_bootup_flag_v01 *wlan_enable,
      qmi_error_type_v01 *qmi_err_num
    );

    // Get MobileAPEnable on bootup Flag
    static inline boolean get_mobileap_enable_cfg_flag(void)
    {
      QCMAP_ConnectionManager* QcMapMgr =
                             QCMAP_ConnectionManager::Get_Instance(NULL, false);
      return QcMapMgr->cfg.bootup_config.enable_mobileap_at_bootup;
    }

    // Get WLAN Enable on bootup flag
    static inline boolean get_wlan_enable_cfg_flag(void)
    {
      QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
      return QcMapMgr->cfg.bootup_config.enable_wlan_at_bootup;
    }

    /* get kernel version */
    boolean GetKernelVer(char *version);

    //Get number of connected devices
    inline int32 GetNumofConnectedDevices (void)
    {
      return conn_device_info.numOfNodes ;
    }

    /* Check if a WLAN client is connected */
    inline int32 CheckWlanClientConnected (void)
    {
      return conn_device_info.wlanclientpresent;
    }

    /* Find match a match for MAC address in the linked list*/
    static bool MatchMacAddrInList(qcmap_nl_addr_t* nl_addr);

    /*Deleting information for a device from list*/
    static bool DeleteDeviceEntryInfo
                                 ( void* mac_addr,
                                   qcmap_msgr_device_type_enum_v01 device_type = QCMAP_MSGR_DEVICE_TYPE_USB_V01,
                                   qcmap_cm_cdi_del_evt_t del_evt = QCMAP_DELNEIGH);


    /*Get MTU size of wwan ipv4 interface */
    static boolean GetWWANMTUInfo(int* mtu_wwan_ipv4);

    static boolean RestoreFactoryConfig(qmi_error_type_v01 *qmi_err_num);

    /*Get Connected Devices Information*/
    static boolean GetConnectedDevicesInfo
                            ( qcmap_msgr_connected_device_info_v01 *conn_devices,
                              unsigned int* num_entries,
                              qmi_error_type_v01 *qmi_err_num);

    /* Write QCMAP config to XML file */
    boolean WriteConfigToXML(qcmap_update_xml, pugi::xml_document*);
    static int GetDeviceName( char*, qcmap_msgr_ip_family_enum_v01,
                                     qmi_error_type_v01*);

    static long int qcmap_match_device_type( const void *first,
                                                        const void *second);
    boolean ReadQCMAPConfigFromXML(char *);
    int IsInterfaceEnabled(char *interface);


    /* Find Gateway IP address assigned to the STA interface */
    boolean GetGatewayIP(uint32 *gwIP);

    static long int qcmap_match_v6_iid(const void *first,
                                               const void *second);
    static long int qcmap_match_v6_addr(const void *first,
                                               const void *second);
    static long int qcmap_match_mac_address( const void *first,
                                                          const void *second);
    static long int qcmap_v6_proxy_match(const void *first, const void *second);
    static long int qcmap_match_proxy_entry(const void *first, const void *second);

    static boolean GetSetBootupConfigFromXML
                                        ( qcmap_bootup_config_enum conf,
                                          qcmap_action_type action,boolean mode);
    static void WriteDefaultsToXML();


    /* Utility function */
    static int GetHWAddr(char *mac,char* devname);
    void DeleteConnectedDevicesClients
                                  ( qcmap_msgr_device_type_enum_v01 device_type );

    boolean CleanIPv4FilterTableFwdChain();
    boolean CleanIPv4FilterTableInChain();
    boolean CleanIPv4NatTable();
    boolean CleanIPv6FilterTableFwdChain();

    /* Update device entry info in the linked list as needed*/
    static bool UpdateDeviceEntryInfo(qcmap_nl_addr_t*  nl_addr,
                                      qcmap_msgr_device_type_enum_v01  device_entry_type);

    void DelV6Conntrack( uint8* ipv6addr, uint8* macaddr);
    static void fetchHostName(char *host, char *mac_str);


#ifdef FEATURE_DATA_TARGET_MDM9607
    void Dsd_client_init();

    /*DSD client callback function */
    static void Dsd_client_cb ( qmi_client_type user_handle,  /* QMI user handle  */
                                   unsigned int    msg_id,       /* Indicator message ID  */
                                   void           *ind_buf,      /* Raw indication data   */
                                   unsigned int    ind_buf_len,  /* Raw data length       */
                                   void           *ind_cb_data   /* User call back handle */
                                 );

    /* Vote for new USB Clock frequency */
    void SetUSBClockFreq ( boolean setNOM );

    boolean CheckRatforLTE();
#endif

};

#endif
