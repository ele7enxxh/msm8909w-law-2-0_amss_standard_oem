/*====================================================

FILE:  QCMAP_Backhaul_WLAN.cpp

SERVICES:
   QCMAP Connection Manager Backhaul Specific Implementation

=====================================================

  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/07/14   ka         Created
  12/06/15   rk         Offtarget support.
===========================================================================*/
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <ifaddrs.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */

#include "ds_string.h"
#include "ds_util.h"
#include "qcmap_cm_api.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "qti_wlan_scm_msgr_v01.h"
#include "ds_qmi_qcmap_msgr.h"

#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_Tethered_Backhaul.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_WLAN_SCM.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_LAN.h"

//extern QCMAP_ConnectionManager *QcMapMgr;
extern struct sigevent sev;


#define WPA_CTRL_PATH "/var/run/wpa_supplicant"

bool QCMAP_Backhaul_WLAN::flag= false;
QCMAP_Backhaul_WLAN* QCMAP_Backhaul_WLAN::object=NULL;
boolean QCMAP_Backhaul_WLAN::ap_sta_bridge_mode_activated=false;
QCMAP_Backhaul_WLAN* QCMAP_Backhaul_WLAN::Get_Instance(boolean obj_create)
{
  if(!flag && obj_create)
  {
    QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
    QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(true);

    LOG_MSG_INFO1("Creating object : BACKHAUL WLAN",0,0,0);
    object = new QCMAP_Backhaul_WLAN(QcMapMgr->xml_path);
    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}
/*===================================================================
                             Class Definitions
  ===================================================================*/

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes Backhaul APStA variables.

@parameters
none

@return
  true  - on success
  false - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/


QCMAP_Backhaul_WLAN::QCMAP_Backhaul_WLAN
(
  char     *xml_path       /* Path and file name for XML config file */
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_msgr_station_mode_config_v01 station_config;

  this->sta_associated = false;
  this->sta_v4_available = false;
  this->sta_v6_available = false;
  //Set AP+STA Mode to default Router
  memset(&this->apsta_cfg,0,sizeof(this->apsta_cfg));
  memset(&this->path_to_supplicant_conf,0,sizeof(this->path_to_supplicant_conf));
  this->apsta_cfg.ap_sta_bridge_mode = false;
  this->ap_sta_bridge_mode_activated = false;
  this->prev_ap_sta_bridge_mode = false;
  this->apsta_cfg.ap_sta_bridge_sta_dhcp_running=false;
  memset(&this->ap_sta_bridge, 0, sizeof(qcmap_ap_sta_bridge_t));
  this->sta_connected_event_received = false;
  bzero(this->apsta_cfg.sta_interface,QCMAP_MSGR_INTF_LEN);
  memset(&this->proxy_addr_list, NULL, sizeof(qcmap_addr_info_list_t));

  /* Initialize Proxy Cleanup timer */
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIG;
  sev.sigev_value.sival_int = NS_NA_PROXY_CLEANUP;
  if (timer_create(CLOCKID, &sev, &(this->apsta_cfg.timerid_proxy_cleanup))== -1)
  {
    LOG_MSG_INFO1(" Failed to create WLAN Proxy Cleanup timer",0,0,0);
    perror("Failed to create WLAN Proxy Cleanup timer");
  }

  if (xml_path)
  {
    //strlcpy(this->xml_path, xml_path, QCMAP_CM_MAX_FILE_LEN);
    /* Read configuration from XML file. */
    if (ReadAPSTAConfigFromXML())
    {
      sev.sigev_notify = SIGEV_SIGNAL;
      sev.sigev_signo = SIG;
      sev.sigev_value.sival_int = STA_ASSOC_FAIL;
      if (timer_create(CLOCKID, &sev, &(this->apsta_cfg.timerid_sta_assoc))== -1)
      {
        LOG_MSG_INFO1(" Failed to create WLAN STA ASSOC timer",0,0,0);
        perror("Failed to create WLAN STA ASSOC timer");
      }
      this->apsta_cfg.ap_sta_bridge_sta_assoc_running=false;

      sev.sigev_notify = SIGEV_SIGNAL;
      sev.sigev_signo = SIG;
      sev.sigev_value.sival_int = STA_DHCP_FAIL;
      if (timer_create(CLOCKID, &sev, &(this->apsta_cfg.timerid_sta_dhcp))== -1)
      {
        LOG_MSG_INFO1(" Failed to create WLAN STA DHCP timer",0,0,0);
        perror("Failed to create WLAN STA DHCP timer");
      }
      this->apsta_cfg.ap_sta_bridge_sta_dhcp_running=false;
      snprintf(this->apsta_cfg.sta_interface, MAX_COMMAND_STR_LEN, "wlan%d", QcMapMgr->sta_iface_index);
      return;
    }
  /* Read Config from XML failed. Use defaults. */
    LOG_MSG_INFO1("QCMAP Backhaul APSTA Read XML failed.\n",0,0,0);
  }

  /* Interface 3 - STA - disabled */
  this->apsta_cfg.static_ip_config.dns_addr = 0;
  this->apsta_cfg.static_ip_config.gw_ip = 0;
  this->apsta_cfg.static_ip_config.netmask = 0;
  this->apsta_cfg.static_ip_config.ip_addr = 0;
  strlcpy(this->path_to_supplicant_conf, "NA", QCMAP_CM_MAX_FILE_LEN);
  snprintf(this->apsta_cfg.sta_interface, MAX_COMMAND_STR_LEN, "wlan%d", QcMapMgr->sta_iface_index);
  this->apsta_cfg.conn_type = QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01;

  this->ipv6_gateway_addr = in6addr_any;
 //this->WriteConfigToXML();
  return;
}

/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
  Destroyes the Backhaul_WLAN Object.

@parameters
none

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

QCMAP_Backhaul_WLAN::~QCMAP_Backhaul_WLAN()
{
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=
                                  QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=
                                    QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                  QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  flag=false;
  object=NULL;
  LOG_MSG_INFO1("Destroying Object: BACKHAUL_WLAN",0,0,0);
  if(!QcMapBackhaulCradleMgr && !QcMapBackhaulWWANMgr && !QcMapBackhaulEthMgr)
    delete QcMapBackhaulMgr;
}

/*===========================================================================
  FUNCTION SetWPASupplicantConfig
==========================================================================*/
/*!
@brief
  Enables/Disables the wpa_supplicant based on the status flag.

@parameters
  boolean status,
  qmi_error_type_v01 *qmi_err_num

@return
  true  - on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WLAN::SetWPASupplicantConfig
(
  boolean status,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);

  char command[MAX_COMMAND_STR_LEN];
  char temp_command[MAX_COMMAND_STR_LEN];

  LOG_MSG_INFO1("QCMAP_ConnectionManager::SetWPASupplicantConfig",0,0,0);

  if (qmi_err_num == NULL)
  {
    LOG_MSG_ERROR("SetWPASupplicantConfig: NULL Arguments",0,0,0);
    return false;
  }
  if(QcMapWLANMgr)
  {
    if (QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
    {
      LOG_MSG_ERROR("SetWPASupplicantConfig: Invalid Wifi mode",0,0,0);
      *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
      return false;
    }

    if(status == false)
    {
      if (!QcMapMgr->dual_wifi_mode)
      {
        /* Kill any running instance */
        snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_supplicant" );
        ds_system_call(command, strlen(command));

        if (!QcMapBackhaulWLANMgr->IsWpaSupplicantkilled())//null check taken care when wifi mode checked above
        {
          /* Kill wpa_supplicant forcefully. */
          snprintf( command, MAX_COMMAND_STR_LEN, "killall -9 wpa_supplicant" );
          ds_system_call(command, strlen(command));
        }

        /* Kill any wpa_cli running instance */
        snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_cli" );
        ds_system_call(command, strlen(command));

        if (!QcMapBackhaulWLANMgr->IsWpaClikilled())
        {
          /* Kill wpa_cli forcefully. */
          snprintf( command, MAX_COMMAND_STR_LEN, "killall -9 wpa_cli" );
          ds_system_call(command, strlen(command));
        }

        LOG_MSG_INFO1("SetWPASupplicantConfig:Stopped all the services.", 0,0,0);
      }
      else
      {
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_STA_1_V01, SCM_MSGR_WLAN_IF_STOP_V01);
      }
    }
    else
    {
      if (!QcMapMgr->dual_wifi_mode)
      {
        /* Restart Supplicant as per supplicant config */
        LOG_MSG_INFO1("Starting station iface %s with config as in: %s\n",
                        QcMapBackhaulWLANMgr->apsta_cfg.sta_interface,
                        QcMapBackhaulWLANMgr->path_to_supplicant_conf,0);


        /* Run wpa_supplicant for sta interface with specified config */
        snprintf(command, MAX_COMMAND_STR_LEN,
                   "wpa_supplicant -i %s -Dnl80211 -B -c %s",
                     QcMapBackhaulWLANMgr->apsta_cfg.sta_interface,
                     QcMapBackhaulWLANMgr->path_to_supplicant_conf);


        /* If debug mode is enabled add concat dbg cmd */
        if (QcMapWLANMgr->debug_config.supplicant_debug)
        {
          snprintf(temp_command, MAX_COMMAND_STR_LEN, "%s %s", command,
                   QcMapWLANMgr->debug_config.supplicant_dbg_cmd);
          memcpy(command, temp_command, MAX_COMMAND_STR_LEN);
        }

        ds_system_call(command, strlen(command));

        /* Run wpa_cli */
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "wpa_cli -i %s -p %s -B -a /usr/bin/QCMAP_StaInterface",
                 QcMapBackhaulWLANMgr->apsta_cfg.sta_interface,
                 WPA_CTRL_PATH);
        ds_system_call(command, strlen(command));

        LOG_MSG_INFO1("SetWPASupplicantConfig::Started all the services.",0,0,0);
      }
      else
      {
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_STA_1_V01, SCM_MSGR_WLAN_IF_START_V01);
      }
      return true;
    }
  }
  else
  {
    LOG_MSG_ERROR("WLAN not up,cannot set Supplicant Config",0,0,0);
    return false;
  }
}

/*===========================================================================
  FUNCTION ActivateSupplicantConfig
==========================================================================*/
/*!
@brief
  Restarts Supplicant with the latest configuration.

@parameters
  void

@return
  true  - on Success
  false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WLAN::ActivateSupplicantConfig
(
  qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN];
  char temp_command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);

  LOG_MSG_INFO1("QCMAP_ConnectionManager::ActivateSupplicantConfig",0,0,0);
  if(QcMapWLANMgr)
  {
    if ( QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
          QcMapMgr->sta_iface_index == QCMAP_LAN_INVALID_IFACE_INDEX || !QcMapBackhaulWLANMgr)
    {
        LOG_MSG_ERROR("\n WLAN is in incorrect Mode: %d.\n", QcMapWLANMgr->wifi_mode, 0, 0);
      *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
      return false;
    }

    /* Restart Supplicant as per supplicant config */
    LOG_MSG_INFO1( "Restarting station iface %s with config as in: %s\n",
                   QcMapBackhaulWLANMgr->apsta_cfg.sta_interface,
                   QcMapBackhaulWLANMgr->path_to_supplicant_conf,0 );

    if (!QcMapMgr->dual_wifi_mode)
    {
      /* Kill any running instance */
      snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_supplicant" );
      ds_system_call(command, strlen(command));

      if(!QcMapBackhaulWLANMgr->IsWpaSupplicantkilled())
      {
        /* Kill wpa_supplicant forcefully. */
        snprintf( command, MAX_COMMAND_STR_LEN, "killall -9 wpa_supplicant" );
        ds_system_call(command, strlen(command));
      }
      /* Run wpa_supplicant for sta interface with specified config */
      snprintf( command, MAX_COMMAND_STR_LEN, "wpa_supplicant -i %s -Dnl80211 -B -c %s",QcMapBackhaulWLANMgr->apsta_cfg.sta_interface,
                    QcMapBackhaulWLANMgr->path_to_supplicant_conf);


      /* If debug mode is enabled add concat dbg cmd */
      if (QcMapWLANMgr->debug_config.supplicant_debug)
      {
        snprintf(temp_command, MAX_COMMAND_STR_LEN, "%s %s", command, QcMapWLANMgr->debug_config.supplicant_dbg_cmd);
        memcpy(command, temp_command, MAX_COMMAND_STR_LEN);
      }

      ds_system_call(command, strlen(command));

      /* Kill any wpa_cli running instance */
      snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_cli" );
      ds_system_call(command, strlen(command));

      if(!QcMapBackhaulWLANMgr->IsWpaClikilled())
      {
        /* Kill wpa_cli forcefully. */
        snprintf( command, MAX_COMMAND_STR_LEN, "killall -9 wpa_cli" );
        ds_system_call(command, strlen(command));
      }

      /* Run wpa_cli */
      snprintf( command, MAX_COMMAND_STR_LEN, "wpa_cli -i %s -p %s -B -a /usr/bin/QCMAP_StaInterface",QcMapBackhaulWLANMgr->apsta_cfg.sta_interface,
              WPA_CTRL_PATH);
      ds_system_call(command, strlen(command));
    }
    else
    {
      QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_STA_1_V01, SCM_MSGR_WLAN_IF_RESTART_V01);
    }

    return true;
  }
  else
  {
    LOG_MSG_ERROR("WLAN not up,cannot set Supplicant Config",0,0,0);
    return false;
  }
}
/*===========================================================================
  FUNCTION EnableStaMode
==========================================================================*/
/*!
@brief
  Enables station mode, as a part of this brings up wpa_supplicant.
  wpa_supplicant runs in the background and acts as the backend component controlling
  the wireless connection.

@parameters
  void

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WLAN::EnableStaMode()
{
  char command[MAX_COMMAND_STR_LEN];
  char temp_command[MAX_COMMAND_STR_LEN];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  struct in_addr addr;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  LOG_MSG_INFO1("Entering EnableStaMode",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if ( !QcMapMgr->qcmap_enable )
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return false;
  }
  //no checks for WLAN object since this function is called by APSTA obj which will only come up when WLAN is enabled.
  if (!QcMapWLANMgr)
  {
    LOG_MSG_ERROR("\n WLAN is in disabled state.\n",0,0,0);
    return false;
  }

  if ( QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
       QcMapMgr->sta_iface_index == QCMAP_LAN_INVALID_IFACE_INDEX )
  {
    LOG_MSG_ERROR("\n WLAN is in incorrect Mode.\n",0,0,0);
    return false;
  }

  /* Update the sta iface index incase it's changed */
  snprintf(this->apsta_cfg.sta_interface, MAX_COMMAND_STR_LEN, "wlan%d", QcMapMgr->sta_iface_index);

  /* Connect STA to specified hotspot as per supplicant config */
  LOG_MSG_INFO1("Connecting station iface %s with config as in: %s\n",this->apsta_cfg.sta_interface,
                this->path_to_supplicant_conf,0);

  /* Validate config file path */
  if( strnlen(this->path_to_supplicant_conf,QCMAP_CM_MAX_FILE_LEN ) == 0 )
  {
    LOG_MSG_ERROR("Error: External AP SSID length 0\n", 0, 0, 0);
    return false;
  }

  /* Enable 4Addr Scheme and start Associtation timer if AP-STA is configured in Bridge Mode*/
  /* Stop DHCP Server and briing up WLAN-STA interface with Guest-LL configuration*/
  /* We will restart dnsmasq as a dhcprelay on we get IP address from EXTAP */
  /* Add WLAN-STA interface to Linux Bridge*/
  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    if ( qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED )
    {
      QcMapNatAlg->DisableNATonApps();
      QcMapFirewall->CleanIPv4MangleTable();
    }
    if ( qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED )
    {
      QcMapBackhaulMgr->DisableIPV6Forwarding(true);
      QcMapFirewall->CleanIPv6MangleTable();
    }

    if (!QcMapMgr->dual_wifi_mode)
    {
      snprintf(command, MAX_COMMAND_STR_LEN, "iw dev wlan%d set 4addr on", QcMapMgr->sta_iface_index);
      ds_system_call("command",strlen(command));
    }

    QCMAPLANMgr->StopDHCPD();
    /* Stop Radish */
    QcMapBackhaulMgr->StopRadish();
    addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_ip_addr);
    strlcpy(a5_ip, inet_ntoa(addr), 16);
    addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
    strlcpy(netmask, inet_ntoa(addr), 16);
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s %s netmask %s up",this->apsta_cfg.sta_interface, a5_ip, netmask);
    ds_system_call(command, strlen(command));
    addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_ip_addr & QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
    strlcpy(subnet, inet_ntoa(addr), 16);
    snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev %s", subnet, netmask, this->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev %s",this->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));

    ds_system_call("echo 2 > /proc/sys/net/ipv6/conf/all/forwarding",
                   strlen("echo 2 > /proc/sys/net/ipv6/conf/all/forwarding"));
  }


  if (!QcMapMgr->dual_wifi_mode)
  {
    /* Kill any running instance */
    snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_supplicant" );
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_cli" );
    ds_system_call(command, strlen(command));

    /* Run wpa_supplicant for sta interface with specified config */
    if(IsAPSTABridgeActivated())
    {
      snprintf(command, MAX_COMMAND_STR_LEN, "wpa_supplicant -b %s -i %s -Dnl80211 -B -c %s", BRIDGE_IFACE,
               this->apsta_cfg.sta_interface,this->path_to_supplicant_conf);
    }
    else
    {
      snprintf(command, MAX_COMMAND_STR_LEN, "wpa_supplicant -i %s -Dnl80211 -B -c %s",this->apsta_cfg.sta_interface,
               this->path_to_supplicant_conf);
    }

    /* If debug mode is enabled add concat dbg cmd */
    if (QcMapWLANMgr->debug_config.supplicant_debug)
    {
      snprintf(temp_command, MAX_COMMAND_STR_LEN, "%s %s", command, QcMapWLANMgr->debug_config.supplicant_dbg_cmd);
      memcpy(command, temp_command, MAX_COMMAND_STR_LEN);
    }

    ds_system_call(command, strlen(command));

    /* Run wpa_cli */
    snprintf( command, MAX_COMMAND_STR_LEN, "wpa_cli -i %s -p %s -B -a /usr/bin/QCMAP_StaInterface",this->apsta_cfg.sta_interface,
              WPA_CTRL_PATH);
    ds_system_call(command, strlen(command));
  }

  /* Start STA Association timer if we are in AP-STA Bridge Mode*/
  if(IsAPSTABridgeActivated())
  {
    start_wlan_sta_timer(STA_ASSOC_FAIL);
    LOG_MSG_INFO1("AP+STA BRIDGE Mode Activated",0,0,0);
  }
  else
  {
    LOG_MSG_INFO1("AP+STA ROUTER Mode Activated",0,0,0);
  }

  /*Set accept_ra flag to 2 if ipv6 enabled*/
  QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  if (QcMapBackhaulMgr)
  {
    if (!QcMapBackhaulMgr->enable_ipv6)
    {
      if (this->IsAPSTABridgeActivated())
      {
        /* Disable ipv6 for Bridge Iface */
        LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for STA Iface",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 BRIDGE_IFACE);
        ds_system_call(command, strlen(command));
      }
      else
      {
        /* Disable ipv6 for ecm Iface */
        LOG_MSG_INFO1("Setting disable_ipv6 flag to TRUE for STA Iface",0,0,0);
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/disable_ipv6",
                 this->apsta_cfg.sta_interface);
        ds_system_call(command, strlen(command));
      }
    }
    else
    {
      /*Set accept_ra flag to 2*/
      if (this->IsAPSTABridgeActivated())
      {
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                 BRIDGE_IFACE);
        ds_system_call(command, strlen(command));

        snprintf(command, MAX_COMMAND_STR_LEN,
                 "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra_prefix_route", BRIDGE_IFACE);
        ds_system_call(command, strlen(command));
      }
      else
      {
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 2 > /proc/sys/net/ipv6/conf/%s/accept_ra",
                 this->apsta_cfg.sta_interface);
        ds_system_call(command, strlen(command));
        /* Dont set the dft route when information is recevied in an RA*/
        snprintf(command, MAX_COMMAND_STR_LEN, "echo 0 > /proc/sys/net/ipv6/conf/%s/accept_ra_defrtr",
                 this->apsta_cfg.sta_interface);
        ds_system_call(command, strlen(command));
      }
    }

  }

  return true;

}

/*===========================================================================
FUNCTION IsAPSTABridgeActivated
==========================================================================*/
/*!
@brief
  Checks if the APSTA Bridge is Activated

@parameters
  void

@return
  true	- on Success
  false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated(void)
{
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);

  if (!QcMapBackhaulWLANMgr)
    return false;

  return QcMapBackhaulWLANMgr->ap_sta_bridge_mode_activated;
}

/*===========================================================================
FUNCTION DisableStaMode
==========================================================================*/
/*!
@brief
  Disables station mode, as a part of this disables NAT.

@parameters
  void

@return
  true  on Success
  false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WLAN::DisableStaMode()
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);

  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);

  char sta_iface[QCMAP_MSGR_INTF_LEN];
  snprintf(sta_iface, MAX_COMMAND_STR_LEN, "wlan%d", QcMapMgr->sta_iface_index);

  LOG_MSG_INFO1("Entering DisableStaMode",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return false;
  }

  if ( QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 )
  {
    LOG_MSG_ERROR("\n WLAN is in disabled state.\n",0,0,0);
    return false;
  }

  if ( QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01
       || QcMapMgr->sta_iface_index == QCMAP_LAN_INVALID_IFACE_INDEX )
  {
    LOG_MSG_ERROR("\n WLAN is in incorrect Mode.\n",0,0,0);
    return false;
  }

  if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
  {
    if (this->sta_v4_available)
    {
      /* Disable NAT on A5. */
      QcMapNatAlg->DisableNATonApps();
      QcMapFirewall->CleanIPv4MangleTable();
    }
    if (this->sta_v6_available)
    {
      /* Disable IPV6 Forwarding. */
      QcMapBackhaulMgr->DisableIPV6Forwarding(true);
      QcMapFirewall->CleanIPv6MangleTable();
      QcMapBackhaulMgr->DeleteAllV6ExternalRoute(sta_iface);
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(this->apsta_cfg.sta_interface);
    }
  }

  /* Disconnect STA from hotspot. */
  /* In dual wifi mode this is handled by SCM */
  if (!QcMapMgr->dual_wifi_mode)
  {
    this->DisconnectSTA();
  }

  /* Reset STA connected and associated flags. */
  this->sta_associated = false;

  return true;

}


/*===========================================================================
  FUNCTION ProcessStaAssoc
==========================================================================*/
/*!
@brief
  Brings up the station mode interface.

@parameters
  cb_user_data

@return
  void
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

void QCMAP_Backhaul_WLAN::ProcessStaAssoc(void *cb_user_data)
{
  char command[MAX_COMMAND_STR_LEN];
  char netMask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  in_addr addr, staIp, staMask, staGwIp;
  boolean ret = false;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  LOG_MSG_INFO1("Entering ProcessStaAssoc",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return;
  }
   //no checks for WLAN object since this function is called by APSTA obj which will only
   //come up when WLAN is enabled.
  if ( QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 )
  {
    LOG_MSG_ERROR("\n WLAN is in disabled state.\n",0,0,0);
    return;
  }

  if ( QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
       QcMapMgr->sta_iface_index == QCMAP_LAN_INVALID_IFACE_INDEX )
  {
    LOG_MSG_ERROR("\n WLAN is in incorrect Mode.\n",0,0,0);
    return;
  }

  /* Connect to the hotspot. */
  if ( this->apsta_cfg.conn_type ==QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01 )
  {
    LOG_MSG_INFO1("Bringing up STA iface %s\n",this->apsta_cfg.sta_interface,0,0);
    /* Bringup interface */
    snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s up",this->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));

    /* Kill any stale dhcpcd intance */
    LOG_MSG_INFO1("Killing previous dhcpcd process.\n",0,0,0);
    snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
              this->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));

    snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid",
              this->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));

    /* Obtain and assign IP address via dhcpcd daemon */
    if (QcMapBackhaulMgr->enable_ipv4)
    {
      LOG_MSG_INFO1("Running DHCP client on %s\n",this->apsta_cfg.sta_interface,0,0);

      snprintf( command, MAX_COMMAND_STR_LEN,
                "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",
                this->apsta_cfg.sta_interface);

      ds_system_call(command, strlen(command));
    }

    this->sta_associated = true;
  }

  else
  {
    if (cb_user_data == NULL)
    {
      LOG_MSG_ERROR("ProcessStaAssoc: NULL handle passed",0,0,0);
      return;
    }

    this->sta_associated = true;

    if (QcMapBackhaulMgr->enable_ipv4)
    {
      LOG_MSG_INFO1("Bringing up STA iface %s with static ip configuration\n",this->apsta_cfg.sta_interface,0,0);
      staIp.s_addr = htonl(this->apsta_cfg.static_ip_config.ip_addr);
      staMask.s_addr = htonl(this->apsta_cfg.static_ip_config.netmask);
      staGwIp.s_addr = htonl(this->apsta_cfg.static_ip_config.gw_ip);
      /* Bring the interface UP. */
      memset(netMask,0,QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
      strlcpy(netMask, inet_ntoa(staMask), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
      snprintf( command, MAX_COMMAND_STR_LEN,
                "ifconfig %s %s netmask %s up",this->apsta_cfg.sta_interface, inet_ntoa(staIp), netMask);
      ds_system_call(command, strlen(command));
      /* Add the default route, if AP STA has max priority. */
      if (QcMapBackhaulMgr->preffered_backhaul_first == BACKHAUL_TYPE_AP_STA_ROUTER)
      {
        #ifndef FEATURE_QTIMAP_OFFTARGET
          snprintf( command, MAX_COMMAND_STR_LEN,
                    "route add default gw %s dev %s", inet_ntoa(staGwIp),this->apsta_cfg.sta_interface);
          ds_system_call(command, strlen(command));
        #endif
      }
      /* Populate the /etc/resolv.conf file with DNS address provided. */
      addr.s_addr = htonl(this->apsta_cfg.static_ip_config.dns_addr);
      #ifndef FEATURE_QTIMAP_OFFTARGET
        snprintf(command, MAX_COMMAND_STR_LEN,
                 "echo 'nameserver %s' > /etc/resolv.conf", inet_ntoa(addr));
        ds_system_call(command, strlen(command));
      #endif
      ProcessAddrAssign(cb_user_data,this->apsta_cfg.sta_interface,IP_V4);
    }
  }
  return;
}

/*===========================================================================
 FUNCTION ProcessStaAssocFail
==========================================================================*/
/*!
@brief
  Process Association Fail timer and provide indication to clients.
  This event only occurs for AP-STA bridge mode.

@parameters
  cb_user_data

@return
  void
@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WLAN::ProcessStaAssocFail(void *cb_user_data)
{
  qmi_qcmap_msgr_softap_handle_type *softApHandle = NULL;
  qmi_qcmap_msgr_status_cb_data *cbPtr = NULL;
  qmi_error_type_v01 qmi_err_num;
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  LOG_MSG_INFO1("QCMAP_ConnectionManager::ProcessStaAssocFail()",0,0,0);

  softApHandle = (qmi_qcmap_msgr_softap_handle_type *) cb_user_data;
  if (softApHandle != NULL)
  {
    cbPtr = (qmi_qcmap_msgr_status_cb_data *) softApHandle->cb_ptr;
  }
  else
  {
    LOG_MSG_ERROR("ProcessStaAssocFail() : NULL handle passed",0,0,0);
    return;
  }

  /*Come back to AP+STA Router mode mode*/
  LOG_MSG_INFO1("Going back to AP+STA Router Mode since Association failed in AP+STA Bridge mode",0,0,0);
  QcMapMgr->revert_ap_sta_router_mode = true;
  if(QCMAP_WLAN::ActivateWLAN(&qmi_err_num) == false)
  {
    LOG_MSG_ERROR("Activate WLAN Failed",0,0,0);
  }

  if (cbPtr && cbPtr->sta_cb)
  {
    LOG_MSG_INFO1(" Sending STA Connected Indication to Client. ",0,0,0);
    cbPtr->sta_cb(cbPtr, QCMAP_MSGR_STATION_MODE_ASSOCIATION_FAIL_V01);
  }
  return;
}


/*==========================================================================
  FUNCTION ProcessAddrAssign
==========================================================================*/
/*!
@brief
  Process the address assignment from Sta

@parameters
  cb_user_data

@return
  void
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

void QCMAP_Backhaul_WLAN::ProcessAddrAssign(void *cb_user_data, char *devname, ip_version_enum_type ip_vsn)
{
  char command[MAX_COMMAND_STR_LEN];
  in_addr staIp, staMask, staGwIp;
  in_addr_t default_gw_addr=0;
  boolean ret = false;
  boolean backhaul_switch_required = false;
  qmi_error_type_v01 qmi_err_num;
  qmi_qcmap_msgr_softap_handle_type *softApHandle = NULL;
  qmi_qcmap_msgr_status_cb_data *cbPtr = NULL;
  char gw[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char iface_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];

  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  qcmap_msgr_cradle_mode_v01 mode;

  memset(gw, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(iface_ip, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  ds_log_med("Entering ProcessAddrAssign for further handling for dev %s",devname);

  if ( cb_user_data == NULL)
  {
    LOG_MSG_ERROR("ProcessAddrAssign : NULL arguments passed",0,0,0);
    return;
  }

  softApHandle = (qmi_qcmap_msgr_softap_handle_type *) cb_user_data;
  if (softApHandle != NULL)
  {
    cbPtr = (qmi_qcmap_msgr_status_cb_data *) softApHandle->cb_ptr;
  }
  else
  {
    LOG_MSG_ERROR("ProcessAddrAssign : NULL handle passed",0,0,0);
    return;
  }

  if(!QcMapBackhaulWLANMgr || !QcMapBackhaulWLANMgr->sta_associated)
  {
    LOG_MSG_INFO1("ProcessAddrAssign : Exiting STA Disassociated",0,0,0);
    return;
  }

  if ( QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01 )
  {
    LOG_MSG_ERROR(" WLAN is in incorrect Mode.",0,0,0);
    return;
  }

  /*Logic to decide whether backhaul needs to be switched or not*/
  if (QcMapBackhaulMgr->ComparePriority(BACKHAUL_TYPE_AP_STA_ROUTER,QcMapBackhaulMgr->current_backhaul))
  {
    backhaul_switch_required = true;
  }

  if (ip_vsn == IP_V4)
  {
    ret = QcMapBackhaulMgr->GetIP(&staIp.s_addr, &staMask.s_addr, devname);
    if ( true == ret )
    {
      LOG_MSG_INFO1("STA Connected to external hotspot for v4.\n",0,0,0);
      ds_log_med("STA Connected to external hotspot with IP %s\n",inet_ntoa(staIp));

      if(QcMapBackhaulWLANMgr->sta_v4_available &&
         (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER ||
          QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_BRIDGE))
      {
        LOG_MSG_INFO1("ProcessAddrAssign: Exiting: Duplicate function Call for IPv4",0,0,0);
        return;
      }
      LOG_MSG_INFO1("STA V4 Available",0,0,0);
      QcMapBackhaulWLANMgr->sta_v4_available = true;
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() && (strncmp(devname,BRIDGE_IFACE,strlen(BRIDGE_IFACE)) == 0))
      {
        LOG_MSG_INFO1("Stopping DHCP IP Assignment Timer for WLAN STA",0,0,0);
        stop_wlan_sta_timer(STA_DHCP_FAIL);
        memcpy(&QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_ip, &staIp, sizeof(in_addr));
        memcpy(&QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_netmask, &staMask, sizeof(in_addr));
      }

        /*If in bridge mode, cleanup is done as path of STA_CONNECTED */
      if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false &&
          QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false)
      {
        if (backhaul_switch_required)
        {
          /* Switch to STA interface. */
          /* Disconnect from WWAN */
          if (QcMapBackhaulMgr->current_backhaul != BACKHAUL_TYPE_AP_STA_ROUTER &&
              QcMapBackhaulMgr->current_backhaul != NO_BACKHAUL)
          {
            if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() ||
                qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED ||
                QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
            {
              LOG_MSG_INFO1("Disabling WWAN V4 Backhaul Settings",0,0,0);
              QcMapNatAlg->DisableNATonApps();
              QcMapFirewall->CleanIPv4MangleTable();
            }
            if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() ||
                qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED ||
                QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
            {
              LOG_MSG_INFO1("Disabling WWAN V6 Backhaul Settings",0,0,0);
              QcMapBackhaulMgr->DisableIPV6Forwarding(true);
              QcMapFirewall->CleanIPv6MangleTable();
              if (QcMapBackhaulCradleMgr && QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
              {
                if (QcMapBackhaulCradleMgr && QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
                  QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
                else
                {
                  QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ECM_IFACE);
                  QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ECM_IFACE);
                }
              }
              if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
              {
                QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ETH_IFACE);
                QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ETH_IFACE);
              }
            }
          }
          QcMapBackhaulMgr->AllowBackhaulAccessOnIface(IP_V4, QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
          QcMapBackhaulMgr->current_backhaul = BACKHAUL_TYPE_AP_STA_ROUTER;
          QcMapNatAlg->EnableNATonApps();

          /* Enable IPv4 MCAST */
          LOG_MSG_INFO1(" Start PIMD to enable MCAST forwarding for AP-STA \n",0,0,0);
          QcMapMgr->StartMcastDaemon();
          LOG_MSG_INFO1("ProcessAddrAssign : V4 Addr Assigned successfully",0,0,0);
        }
      }
      else if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == true)/* Brige Mode Activated. Start DHCP Relay */
      {
        QCMAPLANMgr->StopDHCPD();
        if (false == QcMapMgr->GetGatewayIP(&default_gw_addr))
        {
           LOG_MSG_ERROR("Unable to get the Gateway IP",0,0,0);
        }
        else
        {
          staGwIp.s_addr = default_gw_addr;
          strlcpy(iface_ip, inet_ntoa(staIp), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
          strlcpy(gw, inet_ntoa(staGwIp), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
          memcpy(&QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_def_gw, &staGwIp, sizeof(in_addr));
          ds_log_med("STA Connected to external hotspot with IP %s\n",iface_ip);
          ds_log_med("STA Connected to external hotspot with gateway IP %s\n",gw);
          snprintf( command,MAX_COMMAND_STR_LEN,"dnsmasq -i %s -I lo -z --dhcp-relay=%s,"
                    "%s,%s --dhcp-proxy",BRIDGE_IFACE, iface_ip, gw, BRIDGE_IFACE);
          ds_system_call(command, strlen(command));
          /* Assign STA MAC explicitly to Bridge MAC Address */
          snprintf( command,MAX_COMMAND_STR_LEN, "ifconfig %s hw ether %02x:%02x:%02x:%02x:%02x:%02x",
                   BRIDGE_IFACE, QcMapBackhaulWLANMgr->ap_sta_bridge.sta_mac[0], QcMapBackhaulWLANMgr->ap_sta_bridge.sta_mac[1],
                   QcMapBackhaulWLANMgr->ap_sta_bridge.sta_mac[2], QcMapBackhaulWLANMgr->ap_sta_bridge.sta_mac[3],
                   QcMapBackhaulWLANMgr->ap_sta_bridge.sta_mac[4], QcMapBackhaulWLANMgr->ap_sta_bridge.sta_mac[5]);
          ds_system_call(command, strlen(command));

          QCMAP_Tethered_Backhaul::GetSetConfigFromXML(
                                                 GET_VALUE,
                                                 QCMAP_TETH_CRADLE_BACKHAUL,
                                                 (void *)&mode);

          if ((NULL != QcMapTetheringMgr) &&
              ((mode != QCMAP_MSGR_CRADLE_WAN_ROUTER_V01) ||
               (NULL == QcMapBackhaulCradleMgr)))
            QcMapBackhaulWLANMgr->RestartUSBForBridgeMode();

          QcMapBackhaulWLANMgr->EnterDefaultIPRulesForBridgeMode();
        }
      }
    }
    else
    {
      LOG_MSG_INFO1("Error: GetStaIP Failed. Bailing out.\n",0,0,0);
      return;
    }
  }
  else if (ip_vsn == IP_V6)
  {
    LOG_MSG_INFO1("STA Connected to external hotspot.\n",0,0,0);
    /*Logic to decide whether backhaul needs to be switched or not*/
    if (QcMapBackhaulMgr->ComparePriority(BACKHAUL_TYPE_AP_STA_ROUTER,QcMapBackhaulMgr->current_backhaul))
    {
      backhaul_switch_required = true;
    }

    if(QcMapBackhaulWLANMgr->sta_v6_available &&
       QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
    {
      LOG_MSG_INFO1("ProcessAddrAssign: Exiting: Duplicate function Call for IPv6",0,0,0);
      return;
    }
    QcMapBackhaulWLANMgr->sta_v6_available = true;

    LOG_MSG_INFO1("ProcessAddrAssign: Processing Address Assoc for IPv6",0,0,0);

    if (backhaul_switch_required &&
        (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false) &&
        (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false))
    {
      /* Switch to STA interface. */
      /* Disconnect from WWAN */
      if (QcMapBackhaulMgr->current_backhaul != BACKHAUL_TYPE_AP_STA_ROUTER &&
          QcMapBackhaulMgr->current_backhaul != NO_BACKHAUL)
      {
        if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4()||
            qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED ||
            QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
        {
          QcMapNatAlg->DisableNATonApps();
          QcMapFirewall->CleanIPv4MangleTable();
        }
        if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() ||
            qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED ||
            QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
        {
          QcMapBackhaulMgr->DisableIPV6Forwarding(true);
          QcMapFirewall->CleanIPv6MangleTable();
          if (QcMapBackhaulCradleMgr && QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
          {
            if (QcMapBackhaulCradleMgr && QcMapBackhaulCradleMgr->IsCradleWANBridgeActivated())
              QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
            else
             {
               QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ECM_IFACE);
               QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ECM_IFACE);
             }
          }
          if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
          {
            QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ETH_IFACE);
            QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ETH_IFACE);
          }
        }
      }

      QcMapBackhaulMgr->AllowBackhaulAccessOnIface(IP_V6, QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
      QcMapBackhaulMgr->current_backhaul = BACKHAUL_TYPE_AP_STA_ROUTER;
      QcMapBackhaulMgr->EnableIPV6Forwarding();
      QcMapFirewall->EnableIPV6Firewall();
      LOG_MSG_INFO1("ProcessAddrAssign : V6 Addr Assigned successfully",0,0,0);
    }
  }

  if (cbPtr && cbPtr->sta_cb)
  {
    LOG_MSG_INFO1(" Sending STA Connected Indication to Client. ",0,0,0);
    cbPtr->sta_cb(cbPtr, QCMAP_MSGR_STATION_MODE_CONNECTED_V01);
  }
  return;
}

/*===========================================================================
  FUNCTION ProcessIPv6AddrAssign
==========================================================================*/
/*
 @brief
   Clean IP6Tables Filter, Mangle and Raw tables

@parameters
   void

@return
   void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
static void QCMAP_Backhaul_WLAN::ProcessIPv6AddrAssign(void *cb_user_data, qcmap_nl_sock_msg_t *nlmsg)
{
  char command[MAX_COMMAND_STR_LEN];
  unsigned char ip6_addr[MAX_IPV6_PREFIX+1];   // Getting IPv6 Address
  struct ifaddrs *ifaddr, *ifa;
  memset(ip6_addr, 0, MAX_IPV6_PREFIX + 1);

  in_addr staIp, staMask, staGwIp;
  in_addr_t default_gw_addr=0;
  boolean ret = false;
  qmi_error_type_v01 qmi_err_num;
  qmi_qcmap_msgr_softap_handle_type *softApHandle = NULL;
  qmi_qcmap_msgr_status_cb_data *cbPtr = NULL;
  char gw[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char iface_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];

  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  memset(gw, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
  memset(iface_ip, 0, QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  //ds_log_med("Entering ProcessIPv6AddrAssign for further handling for dev %s",devname);

  if ( cb_user_data == NULL)
  {
    LOG_MSG_ERROR("ProcessIPv6AddrAssign : NULL arguments passed",0,0,0);
    return;
  }

  softApHandle = (qmi_qcmap_msgr_softap_handle_type *) cb_user_data;
  if (softApHandle != NULL)
  {
    cbPtr = (qmi_qcmap_msgr_status_cb_data *) softApHandle->cb_ptr;
  }
  else
  {
    LOG_MSG_ERROR("ProcessIPv6AddrAssign : NULL handle passed",0,0,0);
    return;
  }

  if(!QcMapBackhaulWLANMgr || !QcMapBackhaulWLANMgr->sta_associated)
  {
    LOG_MSG_INFO1("ProcessIPv6AddrAssign : Exiting STA Disassociated",0,0,0);
    return;
  }

  if ( QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01 )
  {
    LOG_MSG_ERROR(" WLAN is in incorrect Mode.",0,0,0);
    return;
  }

  if(!QcMapBackhaulMgr->enable_ipv6)
  {
    LOG_MSG_INFO1("ProcessIPv6AddrAssign : IPv6 not enabled %d", QcMapBackhaulWLANMgr->sta_associated, QcMapBackhaulMgr->enable_ipv6,0);
    return;
  }

  inet_ntop(AF_INET6, nlmsg->nl_addr.ip_v6_addr, ip6_addr, MAX_IPV6_PREFIX);
  ds_log_med("ProcessIPv6AddrAssign with IPv6 addr %s \n",ip6_addr);

  if (QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid &&
      memcmp(&(((struct sockaddr_in6*)&(QcMapBackhaulMgr->ipv6_prefix_info.prefix_addr))->sin6_addr),
             nlmsg->nl_addr.ip_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01) == 0)
  {
    /* This will ocur in 2 situations
       1. Deprecate RA we send for WWAN during AP-STA Bridge assoc
       2. Duplicate RA's from AP-STA
       */
    LOG_MSG_INFO1("Same IPv6 address has already been assigned, ignore", 0,0,0);
    return;
  }
  memcpy(&QcMapMgr->ap_sta_bridge.bridge_ip6, nlmsg->nl_addr.ip_v6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);

  /* Update Local IPv6 Structure*/
  memset(&QcMapBackhaulMgr->ipv6_prefix_info, 0, sizeof(qcmap_cm_nl_prefix_info_t));
  QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_prefered = IPV6_DEFAULT_PREFERED_LIFETIME;
  QcMapBackhaulMgr->ipv6_prefix_info.cache_info.ifa_valid = IPV6_DEFAULT_VALID_LIFETIME;
  QcMapBackhaulMgr->GetIPV6PrefixInfo(BRIDGE_IFACE,&(QcMapBackhaulMgr->ipv6_prefix_info));

  QcMapMgr->ap_sta_bridge.prefix_len = QcMapBackhaulMgr->ipv6_prefix_info.prefix_len;

  /* Get Link Local Addr */
  if (getifaddrs(&ifaddr) == -1)
  {
    LOG_MSG_INFO1("getifaddrs failed", 0,0,0);
    return;
  }

  /* Walk through linked list, maintaining head pointer so we
  can free list later */
  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
  {
    if (ifa->ifa_addr == NULL)
      continue;

    if (ifa->ifa_addr->sa_family != AF_INET6 ||
        strncmp(ifa->ifa_name, BRIDGE_IFACE, strlen(BRIDGE_IFACE)) != 0)
      continue;

    /* For an AF_INET* interface address, display the address */
    struct sockaddr_in6 *current_addr = (struct sockaddr_in6 *) ifa->ifa_addr;
    if (!IN6_IS_ADDR_LINKLOCAL(&(current_addr->sin6_addr)))
      continue;

    memcpy(&QcMapMgr->ap_sta_bridge.bridge_ip6_link_local, &current_addr->sin6_addr, QCMAP_MSGR_IPV6_ADDR_LEN_V01);
    char str[INET6_ADDRSTRLEN]; inet_ntop(AF_INET6, &QcMapMgr->ap_sta_bridge.bridge_ip6_link_local, str, INET6_ADDRSTRLEN);
    ds_log_med("link local addr %s", str);
  }
  freeifaddrs(ifaddr);

  LOG_MSG_INFO1("Stopping DHCP IP Assignment Timer for WLAN STA, v6 address assigned",0,0,0);
  stop_wlan_sta_timer(STA_DHCP_FAIL);

  /* Enter Default IPv6 rules, we only need to enter rules for the first prefix we get */
  if (!QcMapBackhaulWLANMgr->sta_v6_available)
    QcMapBackhaulWLANMgr->EnterDefaultIPv6RulesForBridgeMode();

  if (QcMapBackhaulMgr->current_backhaul != BACKHAUL_TYPE_AP_STA_BRIDGE &&
      QcMapBackhaulWLANMgr->sta_v4_available == false)
    QCMAPLANMgr->StopDHCPD();

  QcMapBackhaulWLANMgr->sta_v6_available = true;
  QcMapBackhaulMgr->current_backhaul = BACKHAUL_TYPE_AP_STA_BRIDGE;

  start_wlan_sta_timer(NS_NA_PROXY_CLEANUP);

  if (cbPtr && cbPtr->sta_cb)
  {
    LOG_MSG_INFO1(" Sending STA Connected Indication to Client. ",0,0,0);
    cbPtr->sta_cb(cbPtr, QCMAP_MSGR_STATION_MODE_CONNECTED_V01);
  }
}


/*===========================================================================
  FUNCTION ProcessStaDisAssoc
==========================================================================*/
/*!
@brief
 Brings down station mode interfaces.

@parameters
  void

@return
  void
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WLAN::ProcessStaDisAssoc()
{
  char command[MAX_COMMAND_STR_LEN];
  qmi_error_type_v01 qmi_err_num;

  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  char sta_iface[QCMAP_MSGR_INTF_LEN];
  snprintf(sta_iface, MAX_COMMAND_STR_LEN, "wlan%d", QcMapMgr->sta_iface_index);


  LOG_MSG_INFO1("Entering ProcessStaDisAssoc",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return;
  }

  if ( QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 )
  {
    LOG_MSG_ERROR("\n WLAN is in disabled state.\n",0,0,0);
    return;
  }

  if ( QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
       QcMapMgr->sta_iface_index == QCMAP_LAN_INVALID_IFACE_INDEX )
  {
    LOG_MSG_ERROR("\n WLAN is in incorrect Mode.\n",0,0,0);
    return;
  }

  if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
  {
    if (this->sta_v4_available)
    {
      /* Disable NAT on A5. and clean IPV4 firewall */
      QcMapNatAlg->DisableNATonApps();
      QcMapFirewall->CleanIPv4MangleTable();
    }

    if (this->sta_v6_available)
    {
      /* Disable IPV6 Forwarding. and firewall */
      QcMapBackhaulMgr->DisableIPV6Forwarding(true);
      QcMapFirewall->CleanIPv6MangleTable();
      QcMapBackhaulMgr->DeleteAllV6ExternalRoute(sta_iface);
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(this->apsta_cfg.sta_interface);
    }
  }

  /*Reset STA Associated flag*/
  this->sta_associated = false;

  /* Reset STA V4 available flag */
  this->sta_v4_available = false;

  /* Reset STA V6 available flag */
  this->sta_v6_available = false;

  /* Kill DHCPCD Process. */
  if ((NULL != QcMapBackhaulWLANMgr) && this->IsAPSTABridgeActivated())
  {
    snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
               BRIDGE_IFACE );
    ds_system_call(command, strlen(command));

    QcMapBackhaulWLANMgr->RemoveDefaultIPv6RulesForBridgeMode();

    stop_wlan_sta_timer(NS_NA_PROXY_CLEANUP);
  }
  else
  {
    snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
               this->apsta_cfg.sta_interface);
  }
  ds_system_call(command, strlen(command));

  if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER &&
      (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false))
  {
    /* Switch to Cradle/WWAN mode if present */
    QCMAP_Backhaul::SwitchToOtherBackhaul(BACKHAUL_TYPE_AP_STA_ROUTER,
                                                    true,true);
  }
}

/*===========================================================================
 FUNCTION ProcessStaDHCPIPFail
==========================================================================*/
/*!
@brief
 Process DHCP IP Assignmentment Fail timer for AP-STA bridge mode.
 Send an indication to client

@parameters
 cb_user_data

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WLAN::ProcessStaDHCPIPFail(void *cb_user_data)
{
  qmi_qcmap_msgr_softap_handle_type *softApHandle = NULL;
  qmi_qcmap_msgr_status_cb_data *cbPtr = NULL;
  qmi_error_type_v01 qmi_err_num;
  in_addr addr, staIp, staMask, staGwIp;
  char netMask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  LOG_MSG_INFO1("QCMAP_ConnectionManager::ProcessStaDHCPIPFail()",0,0,0);

  softApHandle = (qmi_qcmap_msgr_softap_handle_type *) cb_user_data;
  if (softApHandle != NULL)
  {
    cbPtr = (qmi_qcmap_msgr_status_cb_data *) softApHandle->cb_ptr;
  }
  else
  {
    LOG_MSG_ERROR(" ProcessStaDHCPIPFail() : NULL handle passed",0,0,0);
    return;
  }

  QCMAP_Backhaul_WLAN::RemoveDefaultIPv6RulesForBridgeMode();

  /* Check if Static IP is configured */
  if ((this->apsta_cfg.static_ip_config.ip_addr == 0) ||
     (this->apsta_cfg.static_ip_config.netmask == 0) ||
     (this->apsta_cfg.static_ip_config.gw_ip == 0))
  {
    LOG_MSG_ERROR("Static IP configuration for WLAN STA mode not correctly\
                   configured, hence moving back to AP+STA Router Mode",0,0,0);
    QcMapMgr->revert_ap_sta_router_mode = true;
    if ((QCMAP_LAN::ActivateLAN(&qmi_err_num, true) == false)) // Forcefully reactivate LAN
      LOG_MSG_ERROR("Failed to Re-Activate LAN on ProcessStaDHCPIPFail()",0,0,0);

    /* Switch to backhaul mode. */
    if ( qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED )
    {
      QcMapNatAlg->EnableNATonApps();
      QCMAPLANMgr->EnableDNS();
    }
    if ( qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED )
    {
      QcMapBackhaulMgr->EnableIPV6Forwarding();
      QcMapFirewall->EnableIPV6Firewall();
    }
  }
  else /* If Static IP is configured then configure Bridge with static IP */
  {
    LOG_MSG_INFO1("Bringing up Bridge iface %s with static ip configuration\n"\
                  ,BRIDGE_IFACE,0,0);
    staIp.s_addr = htonl(this->apsta_cfg.static_ip_config.ip_addr);
    staMask.s_addr = htonl(this->apsta_cfg.static_ip_config.netmask);
    staGwIp.s_addr = htonl(this->apsta_cfg.static_ip_config.gw_ip);
    /* Bring the interface UP. */
    memset(netMask,0,QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    strlcpy(netMask, inet_ntoa(staMask), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    snprintf( command, MAX_COMMAND_STR_LEN,
              "ifconfig %s %s netmask %s up",BRIDGE_IFACE, inet_ntoa(staIp), netMask);
    ds_system_call(command, strlen(command));
    /* Add the default route. */
    #ifndef FEATURE_QTIMAP_OFFTARGET
      snprintf( command, MAX_COMMAND_STR_LEN,
               "route add default gw %s dev %s", inet_ntoa(staGwIp),BRIDGE_IFACE);
      ds_system_call(command, strlen(command));
    #endif
    /* Populate the /etc/resolv.conf file with DNS address provided. */
    addr.s_addr = htonl(this->apsta_cfg.static_ip_config.dns_addr);
    #ifndef FEATURE_QTIMAP_OFFTARGET
      snprintf(command, MAX_COMMAND_STR_LEN,
              "echo 'nameserver %s' > /etc/resolv.conf", inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    #endif
    this->sta_associated = true;
    QCMAP_Backhaul_WLAN::ProcessAddrAssign(cb_user_data,BRIDGE_IFACE,IP_V4);
  }

  if (cbPtr && cbPtr->sta_cb)
  {
    LOG_MSG_INFO1(" Sending STA Connected Indication to Client. ",0,0,0);
    cbPtr->sta_cb(cbPtr, QCMAP_MSGR_STATION_MODE_DHCP_IP_ASSIGNMENT_FAIL_V01);
  }
  return;
}



/*===========================================================================
  FUNCTION IsStaCfgUpdated
==========================================================================*/
/*!
@brief
  Check is current applied Sta config and the previous are the same.

@parameters
   none

@return
   returns succesful if prev and current sta config are different

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WLAN::IsStaCfgUpdated(void)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

   if ( (memcmp(&this->prev_station_mode_config.static_ip_config,&this->apsta_cfg.static_ip_config,sizeof(qcmap_msgr_sta_static_ip_config_v01))) ||
         (this->prev_station_mode_config.conn_type != this->apsta_cfg.conn_type) ||
         (this->prev_ap_sta_bridge_mode != this->apsta_cfg.ap_sta_bridge_mode)  )
   {
      return true;
   }

   return false;
}

/*===========================================================================
  FUNCTION IsWpaClikilled
==========================================================================*/
/*!
@brief
  Waits 1 second for the process to die.

@parameters
  char *process

@return
  true  - process killed
  flase - process running after waiting 1 second

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WLAN::IsWpaClikilled()
{
  int i = 0;
  FILE *cmd;
  char pid_s[MAX_CMD_SIZE];
  int pid;
  const char process[] = "pidof wpa_cli";

  while (i++ < QCMAP_PROCESS_KILL_RETRY)
  {
    usleep(QCMAP_PROCESS_KILL_WAIT_MS);/*50 mini second*/
    cmd = popen(process, "r");
    pid = 0;
    memset(pid_s, 0, MAX_CMD_SIZE);
    if(cmd)
    {
      fgets(pid_s, MAX_CMD_SIZE, cmd);
      pid = atoi(pid_s);
      LOG_MSG_INFO1("\n wpa_cli pid =%d \n", pid, 0, 0);
      pclose(cmd);
      /* If pid is zero we break from while*/
      if(pid == 0)
      {
        return true;
      }
    }
  }

  LOG_MSG_ERROR("PID still running after waiting 2 second\n", 0, 0, 0);
  return false;
}

/*===========================================================================
  FUNCTION IsWpaSupplicantkilled
==========================================================================*/
/*!
@brief
  Waits 1 second for the process to die.

@parameters
  char *process

@return
  true  - process killed
  flase - process running after waiting 1 second

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WLAN::IsWpaSupplicantkilled()
{
  int i = 0;
  FILE *cmd;
  char pid_s[MAX_CMD_SIZE];
  int pid;
  const char process[] = "pidof wpa_supplicant";

  while (i++ < QCMAP_PROCESS_KILL_RETRY)
  {
    usleep(QCMAP_PROCESS_KILL_WAIT_MS);/*10 mili second*/
    cmd = popen(process, "r");
    pid = 0;
    memset(pid_s, 0, MAX_CMD_SIZE);
    if(cmd)
    {
      fgets(pid_s, MAX_CMD_SIZE, cmd);
      pclose(cmd);
    }
    pid = atoi(pid_s);
    LOG_MSG_INFO1("\n wpa_supplicant pid =%d \n", pid, 0, 0);
    /* If pid is zero we break from while*/
    if(pid == 0)
    {
      return true;
    }
  }

  LOG_MSG_ERROR("PID still running after waiting 2 second\n", 0, 0, 0);
  return false;
}


/*===========================================================================
  FUNCTION DisconnectSTA
==========================================================================*/
/*!
@brief
  Disconnects Station interface from the external hotspot.

@parameters
  void

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WLAN::DisconnectSTA()
{
  char command[MAX_COMMAND_STR_LEN];
/*-------------------------------------------------------------------------*/
  QCMAP_CM_LOG_FUNC_ENTRY();

  LOG_MSG_INFO1("Disconnecting STA\n",0,0,0);

  /* Kill WPA Supplicant Process. */
  snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_supplicant" );
  ds_system_call(command, strlen(command));

  if(!this->IsWpaSupplicantkilled())
  {
    LOG_MSG_ERROR("Forcefull terminating wpa_supplicant", 0, 0, 0);
    bzero( command, MAX_COMMAND_STR_LEN );
    snprintf( command, MAX_COMMAND_STR_LEN, "killall -9 wpa_supplicant" );
    ds_system_call(command, strlen(command));
  }

  /* Kill WPA Cli Process. */
  snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_cli" );
  ds_system_call(command, strlen(command));

  if (!this->IsWpaClikilled())
  {
    /* Kill wpa_cli forcefully. */
    snprintf( command, MAX_COMMAND_STR_LEN, "killall -9 wpa_cli" );
    ds_system_call(command, strlen(command));
  }

  /* Kill DHCPCD Process. */
   //no checks for WLAN object since this function is called by APSTA obj which will only come up when WLAN is enabled.
  snprintf( command, MAX_COMMAND_STR_LEN, "kill -15 $(cat /var/run/dhcpcd-%s.pid)",
            this->apsta_cfg.sta_interface );
  ds_system_call(command, strlen(command));

  /* Reset the STA Connected Event recieved*/
  this->sta_connected_event_received = false;

  return true;
}


/*===========================================================================
 FUNCTION RestartUSBForBridgeMode
==========================================================================*/
/*!
 @brief
  Restart USB Link to the state that it was before ProcessAssocSTAForBridge()
  was called

@parameters

@return
  void
@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WLAN::RestartUSBForBridgeMode(void)
{
  char command[MAX_COMMAND_STR_LEN];
  in_addr addr;
  boolean ret = false;
  char teth_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char teth_gw_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  boolean usb_enable=false;
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);


  // TETH related link local address
  if (QcMapTetheringMgr)
  {
    addr.s_addr = htonl(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].gateway_addr);
    strlcat(teth_gw_ip, inet_ntoa(addr), 16);
    addr.s_addr = htonl(QcMapTetheringMgr->ll_subnet_mask);
    strlcpy(netmask, inet_ntoa(addr), 16);
  }
  else
  {
    return;
  }

  LOG_MSG_INFO1("RestoreUSBForBridgeMode()",0,0,0);
  bzero(teth_intf_name,sizeof(teth_intf_name));
  /* Restart USB Link */
  if (QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1))
  {
    if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS )
    {
      strlcpy(teth_intf_name, "rndis0",sizeof(teth_intf_name));
    }
    else if (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM )
    {
      strlcpy(teth_intf_name, "ecm0",sizeof(teth_intf_name));
    }
    else
    {
      LOG_MSG_ERROR("Incorrect USB TETH LINK Detected Link Type = %x",
                    QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,
                    0,0);
      return;
    }
    snprintf( command, MAX_COMMAND_STR_LEN, "brctl delif %s %s",BRIDGE_IFACE,teth_intf_name);
    ds_system_call(command, strlen(command));
    ds_system_call("echo 0 > /sys/class/android_usb/android0/enable ",strlen("echo 0 > /sys/class/android_usb/android0/enable "));
    usleep(500000);

    /* Delay for 1 secs since USB UP/DOWN in immediate instance causes issues*/
    ds_system_call("echo 1 > /sys/class/android_usb/android0/enable ",strlen("echo 1 > /sys/class/android_usb/android0/enable "));
    snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s %s netmask %s up",teth_intf_name,teth_gw_ip,netmask);
    ds_system_call(command, strlen(command));
    /* Delete LL Address */
    addr.s_addr = htonl(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].gateway_addr & QcMapTetheringMgr->ll_subnet_mask);
    strlcpy(subnet, inet_ntoa(addr), 16);
    snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev %s", subnet, netmask, teth_intf_name);
    ds_system_call(command, strlen(command));
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev %s", teth_intf_name);
    ds_system_call(command, strlen(command));
  }
}


/*===========================================================================
  FUNCTION EnterDefaultIPRulesForBridgeMode
==========================================================================*/
/*!
@brief
  Clean EbTables and IPtables default IP based for rules AP-STA Bridge Mode

@parameters
  void

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WLAN::EnterDefaultIPRulesForBridgeMode(void)
{
  char command[MAX_COMMAND_STR_LEN];
  char teth_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char netmask [QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);


  LOG_MSG_INFO1("EnterDefaultIPRulesForBridgeMode()",0,0,0);

  /* Since we want bridge as a proxy for STA iface. Ensure all arp-requests are replied by Bridge and not forwarded*/
  snprintf(command, MAX_COMMAND_STR_LEN,"ebtables -t filter -A FORWARD -i %s -p ARP -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"ebtables -t filter -A FORWARD -o %s -p ARP -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));

  /* Ensure DHCP-ARP DAD works fine */
  snprintf(command, MAX_COMMAND_STR_LEN,"ebtables -t nat -I PREROUTING -p arp --arp-ip-src=0.0.0.0 --arp-opcode Request -j ACCEPT");
  ds_system_call(command, strlen(command));

  /* Ensure we dont reply to gratitious ARP requests */
  snprintf(command, MAX_COMMAND_STR_LEN,"ebtables -t nat -I PREROUTING -p arp --arp-opcode Request --arp-gratuitous -j ACCEPT");
  ds_system_call(command, strlen(command));

  strlcpy(netmask, inet_ntoa(this->ap_sta_bridge.bridge_netmask), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  /* Default Subnet Based ARP Reply Rules for WLAN and USB Interface*/
  snprintf(command, MAX_COMMAND_STR_LEN,
                   "ebtables -t nat -A PREROUTING -i wlan%d -p arp --arp-ip-src=%s/%s --arp-opcode Request -j arpreply --arpreply-mac %02x:%02x:%02x:%02x:%02x:%02x --arpreply-target ACCEPT",
                   QcMapMgr->ap_dev_num1, inet_ntoa(this->ap_sta_bridge.bridge_def_gw), netmask, this->ap_sta_bridge.bridge_mac[0], this->ap_sta_bridge.bridge_mac[1],
                   this->ap_sta_bridge.bridge_mac[2], this->ap_sta_bridge.bridge_mac[3], this->ap_sta_bridge.bridge_mac[4], this->ap_sta_bridge.bridge_mac[5]);
  ds_system_call(command, strlen(command));
  bzero(teth_intf_name,sizeof(teth_intf_name));
  if ( QcMapTetheringMgr &&
       QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1))
  {
    if (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS )
    {
      strlcpy(teth_intf_name, "rndis0",sizeof(teth_intf_name));
    }
    else if (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM )
    {
      strlcpy(teth_intf_name, "ecm0",sizeof(teth_intf_name));
    }
    else
    {
      LOG_MSG_ERROR("Incorrect TETH LINK Detected Link Type = %x",
                    QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,
                    0,0);
      return;
    }
    snprintf(command, MAX_COMMAND_STR_LEN,
                     "ebtables -t nat -A PREROUTING -i %s -p arp --arp-ip-src=%s/%s --arp-opcode Request -j arpreply --arpreply-mac %02x:%02x:%02x:%02x:%02x:%02x --arpreply-target ACCEPT",
                     teth_intf_name, inet_ntoa(this->ap_sta_bridge.bridge_def_gw), netmask, this->ap_sta_bridge.bridge_mac[0], this->ap_sta_bridge.bridge_mac[1],
                     this->ap_sta_bridge.bridge_mac[2], this->ap_sta_bridge.bridge_mac[3], this->ap_sta_bridge.bridge_mac[4], this->ap_sta_bridge.bridge_mac[5]);
    ds_system_call(command, strlen(command));
  }
}

/*===========================================================================
 * FUNCTION EnterDefaultIPv6RulesForBridgeMode
==========================================================================*/
/*!
@brief
  Setup IP6tables default IPv6 based for rules AP-STA Bridge Mode

@parameters
  void

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WLAN::EnterDefaultIPv6RulesForBridgeMode(void)
{
  char command[MAX_COMMAND_STR_LEN];
  char usb_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char netmask [QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  LOG_MSG_INFO1("EnterDefaultIPv6RulesForBridgeMode()",0,0,0);

  /* Make sure any duplicate rules are removed first */
  this->RemoveDefaultIPv6RulesForBridgeMode();

  snprintf(command, MAX_COMMAND_STR_LEN,"ip6tables -t filter -A FORWARD -o %s -p ipv6-icmp -m icmp6 --icmpv6-type 135 -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"ip6tables -t filter -A FORWARD -i %s -p ipv6-icmp -m icmp6 --icmpv6-type 135 -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"ip6tables -t filter -A FORWARD -o %s -p ipv6-icmp -m icmp6 --icmpv6-type 136 -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"ip6tables -t filter -A FORWARD -i %s -p ipv6-icmp -m icmp6 --icmpv6-type 136 -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"ebtables -t filter -A FORWARD -o %s -p ip6 --ip6-protocol ipv6-icmp --ip6-icmp-type 135:136 -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"ebtables -t filter -A FORWARD -i %s -p ip6 --ip6-protocol ipv6-icmp --ip6-icmp-type 135:136 -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));

  /* Drop all redirect messages */
  snprintf(command, MAX_COMMAND_STR_LEN,"ip6tables -t filter -A OUTPUT -p ipv6-icmp -m icmp6 --icmpv6-type 137 -j DROP");
  ds_system_call(command, strlen(command));
}

/*===========================================================================
 * FUNCTION RemoveDefaultIPv6RulesForBridgeMode
==========================================================================*/
/*!
@brief
  Remove IP6tables default IPv6 based for rules AP-STA Bridge Mode

@parameters
  void

@return
  void

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WLAN::RemoveDefaultIPv6RulesForBridgeMode(void)
{
  char command[MAX_COMMAND_STR_LEN];
  char usb_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char netmask [QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  LOG_MSG_INFO1("RemoveDefaultIPv6RulesForBridgeMode()",0,0,0);

  snprintf(command, MAX_COMMAND_STR_LEN,"ip6tables -t filter -D FORWARD -o %s -p ipv6-icmp -m icmp6 --icmpv6-type 135 -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"ip6tables -t filter -D FORWARD -i %s -p ipv6-icmp -m icmp6 --icmpv6-type 135 -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"ip6tables -t filter -D FORWARD -o %s -p ipv6-icmp -m icmp6 --icmpv6-type 136 -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"ip6tables -t filter -D FORWARD -i %s -p ipv6-icmp -m icmp6 --icmpv6-type 136 -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"ebtables -t filter -D FORWARD -o %s -p ip6 --ip6-protocol ipv6-icmp --ip6-icmp-type 135:136 -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"ebtables -t filter -D FORWARD -i %s -p ip6 --ip6-protocol ipv6-icmp --ip6-icmp-type 135:136 -j DROP", this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));

  /* Drop all redirect messages */
  snprintf(command, MAX_COMMAND_STR_LEN,"ip6tables -t filter -D OUTPUT -p ipv6-icmp -m icmp6 --icmpv6-type 137 -j DROP");
  ds_system_call(command, strlen(command));
}

/*===========================================================================
 FUNCTION EnterDefaultMACRulesForBridgeMode
==========================================================================*/
/*!
@brief
  Clean EbTables and IPtables default rules for AP-STA Bridge Mode,
  related to MAC address

@parameters
  void

@return
   void

@note

  - Dependencies
  - None

  - Side Effects
  - None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WLAN::EnterDefaultMACRulesForBridgeMode(void)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  LOG_MSG_INFO1("EnterDefaultRulesForBridgeMode()",0,0,0);

  /* Enter Rules to Perform MAC-SNAT to ensure WIFI bridge mode works fine with 3-addr scheme */
  snprintf(command, MAX_COMMAND_STR_LEN,"ebtables -t nat -A POSTROUTING -o %s -j snat --to-source %02x:%02x:%02x:%02x:%02x:%02x",
                   this->apsta_cfg.sta_interface, this->ap_sta_bridge.sta_mac[0],this->ap_sta_bridge.sta_mac[1],this->ap_sta_bridge.sta_mac[2],
                   this->ap_sta_bridge.sta_mac[3], this->ap_sta_bridge.sta_mac[4],this->ap_sta_bridge.sta_mac[5]);
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,"ebtables -t nat -A POSTROUTING -o wlan%d -j snat --to-source %02x:%02x:%02x:%02x:%02x:%02x",
                   QcMapMgr->ap_dev_num1, this->ap_sta_bridge.ap_mac[0],this->ap_sta_bridge.ap_mac[1],this->ap_sta_bridge.ap_mac[2],
                   this->ap_sta_bridge.ap_mac[3], this->ap_sta_bridge.ap_mac[4], this->ap_sta_bridge.ap_mac[5]);
  ds_system_call(command, strlen(command));
}



/*===========================================================================
 FUNCTION ProcessStaAssocForWLANBridge
==========================================================================*/
/*!
@brief
  Brings up the station mode interface.

@parameters
  cb_user_data

@return
  void
@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WLAN::ProcessStaAssocForWLANBridge(void *cb_user_data)
{
  char command[MAX_COMMAND_STR_LEN];
  in_addr addr, staIp, staMask, staGwIp;
  boolean ret = false;
  qmi_error_type_v01 qmi_err_num;
  char ap_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  LOG_MSG_INFO1("Entering ProcessStaAssocForWLANBridge",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return;
  }

  if ( QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 )
  {
    LOG_MSG_ERROR("\n WLAN is in disabled state.\n",0,0,0);
    return;
  }

  if ( QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
       QcMapMgr->sta_iface_index == QCMAP_LAN_INVALID_IFACE_INDEX )
  {
    LOG_MSG_ERROR("\n WLAN is in incorrect Mode.\n",0,0,0);
    return;
  }

  /* Stop WLAN-STA Assoc Timer*/
  stop_wlan_sta_timer(STA_ASSOC_FAIL);

  if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() ||
      qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED ||
      QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4())
  {
    LOG_MSG_INFO1("Disabling WWAN V4 Backhaul Settings",0,0,0);
    QcMapNatAlg->DisableNATonApps();
    QcMapFirewall->CleanIPv4MangleTable();
    QcMapBackhaulMgr->current_backhaul = NO_BACKHAUL;
  }
  if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6() ||
      qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED)
  {
    LOG_MSG_INFO1("Disabling WWAN V6 Backhaul Settings",0,0,0);
    QcMapBackhaulMgr->DisableIPV6Forwarding(true);
    QcMapFirewall->CleanIPv6MangleTable();
    if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated())
    {
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(BRIDGE_IFACE);
      snprintf(command, MAX_COMMAND_STR_LEN, "ebtables -D FORWARD -p IPv4 --ip-proto udp --ip-dport 67 -j DROP");
      ds_system_call(command, strlen(command));

      snprintf(command, MAX_COMMAND_STR_LEN, "ebtables -D FORWARD -p IPv4 --ip-proto udp --ip-dport 68 -j DROP");
      ds_system_call(command, strlen(command));
    }
    else
    {
      QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ECM_IFACE);
      QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ECM_IFACE);
    }

    /* Reset forwarding as it was cleared in disableIPv6 */
    ds_system_call("echo 2 > /proc/sys/net/ipv6/conf/all/forwarding",
                   strlen("echo 2 > /proc/sys/net/ipv6/conf/all/forwarding"));

    QcMapBackhaulMgr->current_backhaul = NO_BACKHAUL;
  }
  if(QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
  {
    QcMapBackhaulMgr->DeleteAllV6ExternalRoute(ETH_IFACE);
    QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(ETH_IFACE);
  }

  /* Clean All Firewall/Filtering Rules */
  QcMapMgr->CleanIPtables();
  QcMapMgr->CleanEbtables();

  snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s %s",
           BRIDGE_IFACE, this->apsta_cfg.sta_interface);
  ds_system_call(command, strlen(command));

  snprintf(ap_intf_name, (DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2), "wlan%d", QcMapMgr->ap_dev_num1);
  /*Get all HW Address */
  if ((QcMapMgr->GetHWAddr(this->ap_sta_bridge.ap_mac, ap_intf_name) != QCMAP_CM_SUCCESS)     ||
      (QcMapMgr->GetHWAddr(this->ap_sta_bridge.sta_mac, this->apsta_cfg.sta_interface) != QCMAP_CM_SUCCESS))
  {
    LOG_MSG_ERROR("MAC Address Get returned error on AP-STA Bridge Mode. Cannot proceed further \n",0,0,0);
    return;
  }
  else
  {
    memcpy(this->ap_sta_bridge.bridge_mac, this->ap_sta_bridge.sta_mac, ETH_ALEN);
  }

  /* Assign STA MAC explicitly to Bridge MAC Address */
  snprintf( command,MAX_COMMAND_STR_LEN, "ifconfig %s hw ether %02x:%02x:%02x:%02x:%02x:%02x",
              BRIDGE_IFACE, this->ap_sta_bridge.sta_mac[0], this->ap_sta_bridge.sta_mac[1],
              this->ap_sta_bridge.sta_mac[2], this->ap_sta_bridge.sta_mac[3],
              this->ap_sta_bridge.sta_mac[4], this->ap_sta_bridge.sta_mac[5]);
  ds_system_call(command, strlen(command));

  /* Enter Generic rules Ebtables/IPtables rules for AP-STA in Bridge mode*/
  this->EnterDefaultMACRulesForBridgeMode();

  this->sta_associated = true;
  QcMapBackhaulMgr->current_backhaul = BACKHAUL_TYPE_AP_STA_BRIDGE;

  /* Delete old IP address and default route*/
  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s 0", BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  /* If IPv6 is enabled then send RS to get IPv6 address */
  if (QcMapBackhaulMgr->enable_ipv6)
    QcMapBackhaulMgr->SendRSOnBridgeIface();

  /* Connect to the hotspot. */
  if ( this->apsta_cfg.conn_type ==QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01 )
  {
    LOG_MSG_INFO1("Dynamic IP Assignment Settings for %s\n",this->apsta_cfg.sta_interface,0,0);

    /* Kill any stale dhcpcd intance */
    LOG_MSG_INFO1("Killing previous dhcpcd process.\n",0,0,0);

    /*Kill only for bridge iface should not killall since WAN Router may be on*/
    QcMapBackhaulMgr->StopDHCPCD(BRIDGE_IFACE);
    QcMapBackhaulMgr->StopDHCPCD(this->apsta_cfg.sta_interface);
    /* Clean the PIDs */
    snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid",this->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/run/dhcpcd-%s.pid",BRIDGE_IFACE);
    ds_system_call(command, strlen(command));
    /* Clean the leases */
    snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/db/dhcpcd-%s.lease",this->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));
    snprintf( command, MAX_COMMAND_STR_LEN, "rm -rf /var/db/dhcpcd-%s.lease",BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    /* Obtain and assign IP address via dhcpcd daemon */
    LOG_MSG_INFO1("Running DHCP client on %s\n",BRIDGE_IFACE,0,0);
    snprintf( command, MAX_COMMAND_STR_LEN,
              "dhcpcd %s -t 0 -o domain_name_servers --noipv4ll -b -G",BRIDGE_IFACE);
    ds_system_call(command, strlen(command));

    /* Start DHCP Timer */
    if (QCMAP_CM_SUCCESS == start_wlan_sta_timer(STA_DHCP_FAIL))
    {
      LOG_MSG_INFO1("Timer Started for STA DHCP IP assignment",0,0,0);
    }
    else
    {
      LOG_MSG_ERROR("Starting DHCP IP Assignment Timer Failed",0,0,0);
    }
  }
  else /* If Static Configuration Provided */
  {
    if (cb_user_data == NULL)
    {
      LOG_MSG_ERROR("ProcessStaAssoc: NULL handle passed",0,0,0);
      return;
    }
    LOG_MSG_INFO1("Bringing up STA iface %s with static ip configuration\n",BRIDGE_IFACE,0,0);
    staIp.s_addr = htonl(this->apsta_cfg.static_ip_config.ip_addr);
    staMask.s_addr = htonl(this->apsta_cfg.static_ip_config.netmask);
    staGwIp.s_addr = htonl(this->apsta_cfg.static_ip_config.gw_ip);
    /* Bring the interface UP. */
    memset(netmask,0,QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    strlcpy(netmask, inet_ntoa(staMask), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    snprintf( command, MAX_COMMAND_STR_LEN,
              "ifconfig %s %s netmask %s up",BRIDGE_IFACE, inet_ntoa(staIp), netmask);
    ds_system_call(command, strlen(command));
    /* Add the default route. */
    #ifndef FEATURE_QTIMAP_OFFTARGET
      snprintf( command, MAX_COMMAND_STR_LEN,
                "route add default gw %s dev %s", inet_ntoa(staGwIp),BRIDGE_IFACE);
      ds_system_call(command, strlen(command));
    #endif
    /* Populate the /etc/resolv.conf file with DNS address provided. */
    addr.s_addr = htonl(this->apsta_cfg.static_ip_config.dns_addr);
    #ifndef FEATURE_QTIMAP_OFFTARGET
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 'nameserver %s' > /etc/resolv.conf", inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    #endif
    this->sta_associated = true;
    ProcessAddrAssign(cb_user_data,BRIDGE_IFACE,IP_V4);
  }
  return;
}


/*===========================================================================
 FUNCTION ProcessStaDisAssocForWLANBridge
==========================================================================*/
/*!
@brief
  Brings down station mode interfaces when we are in AP-STA mode

@parameters
 void

@return
  void
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_Backhaul_WLAN::ProcessStaDisAssocForWLANBridge()
{
  char command[MAX_COMMAND_STR_LEN];
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  LOG_MSG_INFO1("Entering ProcessStaDisAssocForWLANBridge",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if (!QcMapMgr->qcmap_enable)
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.\n",0,0,0);
    return;
  }

  if ( QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 )
  {
    LOG_MSG_ERROR("\n WLAN is in disabled state.\n",0,0,0);
    return;
  }

  if ( QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ||
     QcMapMgr->sta_iface_index == QCMAP_LAN_INVALID_IFACE_INDEX )
  {
    LOG_MSG_ERROR("\n WLAN is in incorrect Mode.\n",0,0,0);
    return;
  }

  /* Clean IPtables and Ebtables */
  QcMapMgr->CleanIPtables();
  QcMapMgr->CleanEbtables();

  /* Moving back to AP-STA Router Mode */
  LOG_MSG_ERROR("Moving Back to AP+STA Router Mode, so webserver is accessable",0,0,0);
  QcMapMgr->revert_ap_sta_router_mode = true;
  if ((QCMAP_LAN::ActivateLAN(&qmi_err_num, true) == false)) // Forcefully reactivate LAN
     LOG_MSG_ERROR("Failed to Re-Activate LAN",0,0,0);

  /*Reset STA Associated flag*/
  this->sta_associated = false;
  this->sta_v4_available = false;
  this->sta_v6_available = false;

  if (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_BRIDGE &&
      (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false))
  {
    /* Kill DHCPCD Process. */
    QcMapBackhaulMgr->StopDHCPCD(BRIDGE_IFACE); // Switching Not required since bridge configured in AP-STA Bridge
    QcMapBackhaulMgr->current_backhaul = NO_BACKHAUL;
  }
}

/*===========================================================================
 FUNCTION IsSTAAvailableV4
==========================================================================*/
/*!
@brief
  Utility API to check if STA is available for V4 connection

@parameters
 void

@return
  boolean

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WLAN::IsSTAAvailableV4()
{
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  if((NULL != QcMapBackhaulWLANMgr) && QcMapBackhaulWLANMgr->sta_v4_available)
    return true;
  else
    return false;
}

/*===========================================================================
 FUNCTION IsSTAAvailableV6
==========================================================================*/
/*!
@brief
  Utility API to check if STA is available for V6 connection

@parameters
 void

@return
  boolean

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WLAN::IsSTAAvailableV6()
{
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  if((NULL != QcMapBackhaulWLANMgr) && QcMapBackhaulWLANMgr->sta_v6_available)
    return true;
  else
    return false;
}

/*===========================================================================
  FUNCTION GetSetAPSTAConfigFromXML
==========================================================================*/
/*!
@brief
  Get and sets the APSTA Config from XML.

@parameters
  None

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML
(
  qcmap_action_type action,
  qcmap_msgr_station_mode_config_v01 *station_config
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, root1;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  in_addr addr;
  uint32_t debug_string=0;
  char data[MAX_STRING_LENGTH] = {0};
  qcmap_msgr_sta_connection_enum_v01 conn_type;
  /* IP configuration for STATIC STA mode. */
  qcmap_msgr_sta_static_ip_config_v01 static_ip_config;
  char str[INET6_ADDRSTRLEN];
  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG).child(StationModeCfg_TAG);
  root1 = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);
  if (action == GET_VALUE)
  {
    if (atoi(root.child(STAModeConnType_TAG).child_value()) == 2)
      station_config->conn_type = QCMAP_MSGR_STA_CONNECTION_STATIC_V01;
    else
      station_config->conn_type = QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01;
    if (inet_aton(root.child(STAModeStaticIPAddr_TAG).child_value(), &addr))
      station_config->static_ip_config.ip_addr = ntohl(addr.s_addr);
    if (inet_aton(root.child(STAModeStaticConfigDNSAddr_TAG).child_value(), &addr))
      station_config->static_ip_config.dns_addr = ntohl(addr.s_addr);
    if (inet_aton(root.child(STAModeStaticConfigGWAddr_TAG).child_value(), &addr))
      station_config->static_ip_config.gw_ip = ntohl(addr.s_addr);
    if (inet_aton(root.child(STAModeStaticConfigNetMask_TAG).child_value(), &addr))
      station_config->static_ip_config.netmask = ntohl(addr.s_addr);
    station_config->ap_sta_bridge_mode = atoi(root1.child(EnableBridgeMode_TAG).child_value());
  }
  else
  {
    snprintf(data, sizeof(data), "%d", station_config->conn_type);
    root.child(STAModeConnType_TAG).text() = data;
    if (station_config->conn_type == QCMAP_MSGR_STA_CONNECTION_STATIC_V01)
    {
      addr.s_addr = htonl(station_config->static_ip_config.ip_addr);
      root.child(STAModeStaticIPAddr_TAG).text() = inet_ntoa(addr);
      addr.s_addr = htonl(station_config->static_ip_config.dns_addr);
      root.child(STAModeStaticConfigDNSAddr_TAG).text() = inet_ntoa(addr);
      addr.s_addr = htonl(station_config->static_ip_config.gw_ip);
      root.child(STAModeStaticConfigGWAddr_TAG).text() = inet_ntoa(addr);
      addr.s_addr = htonl(station_config->static_ip_config.netmask);
      root.child(STAModeStaticConfigNetMask_TAG).text() = inet_ntoa(addr);
    }
    snprintf(data, sizeof(data), "%d", station_config->ap_sta_bridge_mode);
    root1.child(EnableBridgeMode_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  LOG_MSG_INFO1("Connection type: %d Action: %d Bridge Mode: %d" ,
                station_config->conn_type, action, station_config->ap_sta_bridge_mode);
  debug_string=htonl(station_config->static_ip_config.ip_addr);
  readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
  LOG_MSG_INFO1("Ip address: %s", str,0,0);

  return true;
}

/*===========================================================================
  FUNCTION ReadAPSTAConfigFromXML
==========================================================================*/
/*!
@brief
  Reads the APSTA Config from XML.

@parameters
  None

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Backhaul_WLAN::ReadAPSTAConfigFromXML()
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  in_addr addr;
  uint32_t debug_string=0;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QCMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  qcmap_cm_intf_index_type ssid = QCMAP_MSGR_INTF_STATION_INDEX;
  char str[INET6_ADDRSTRLEN];
  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG).child(StationModeCfg_TAG);

  if ( atoi(root.child(STAModeConnType_TAG).child_value()) == 2)
    this->apsta_cfg.conn_type = QCMAP_MSGR_STA_CONNECTION_STATIC_V01;
  else
    this->apsta_cfg.conn_type = QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01;

  if (inet_aton(root.child(STAModeStaticIPAddr_TAG).child_value(), &addr))
    this->apsta_cfg.static_ip_config.ip_addr = ntohl(addr.s_addr);
  if (inet_aton(root.child(STAModeStaticConfigDNSAddr_TAG).child_value(), &addr))
    this->apsta_cfg.static_ip_config.dns_addr = ntohl(addr.s_addr);
  if (inet_aton(root.child(STAModeStaticConfigGWAddr_TAG).child_value(), &addr))
    this->apsta_cfg.static_ip_config.gw_ip = ntohl(addr.s_addr);
  if (inet_aton(root.child(STAModeStaticConfigNetMask_TAG).child_value(), &addr))
    this->apsta_cfg.static_ip_config.netmask = ntohl(addr.s_addr);

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  this->apsta_cfg.ap_sta_bridge_mode = atoi(root.child(EnableBridgeMode_TAG).child_value());

  strlcpy( this->path_to_supplicant_conf,
           root.child(StationModeCfg_TAG).child(SupplicantCfg_TAG).child_value(),
           QCMAP_CM_MAX_FILE_LEN );

  LOG_MSG_INFO1("Connection type: %d " ,this->apsta_cfg.conn_type,0 ,0);
  debug_string =htonl(this->apsta_cfg.static_ip_config.ip_addr);
  readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
  LOG_MSG_INFO1("\nIp address: '%s'",str,0,0);
  debug_string =htonl(this->apsta_cfg.static_ip_config.netmask);
  readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
  LOG_MSG_INFO1("\nNetmask: '%s'",str,0,0);
  debug_string =htonl(this->apsta_cfg.static_ip_config.dns_addr);
  readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
  LOG_MSG_INFO1("\nDNS Adress: '%s'",str,0,0);
  debug_string =htonl(this->apsta_cfg.static_ip_config.gw_ip);
  readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
  LOG_MSG_INFO1("\nGateway Address : '%s'",str,0,0);
  return true;
}


/*===========================================================================
  FUNCTION CleanV6ProxyEntries
==========================================================================*/
/*!
@brief
  Stops the proxy cleanup.

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/

boolean QCMAP_Backhaul_WLAN::CleanV6ProxyEntries()
{
  ds_dll_el_t *node = NULL, *temp_node = NULL;
  struct timeval sys_time;
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  qcmap_addr_info_list_t* proxy_addrList = &(QcMapBackhaulWLANMgr->proxy_addr_list);
  qcmap_cm_proxy_data_info_t *proxyInfo = NULL, *temp_proxyInfo = NULL;

  if(proxy_addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Error: Proxy list is empty", 0, 0, 0);
    return false;
  }

  LOG_MSG_INFO1("CleanV6ProxyEntries",0,0,0);

  if (gettimeofday(&sys_time, 0) != 0)
  {
    LOG_MSG_ERROR("Gettimeofday() failed", 0, 0, 0);
    return false;
  }

  node = proxy_addrList->addrListHead;
  node = ds_dll_next (node, (const void**)(&proxyInfo));

  if(proxyInfo == NULL)
  {
    LOG_MSG_ERROR("proxyinfo is null", 0, 0, 0);
    return false;
  }

  while ( (node != NULL) && ( proxyInfo ) )
  {

    if (sys_time.tv_sec > (proxyInfo->time + 10))
    {
      /* save the next node before deleting current node */
      temp_node = ds_dll_next (node, (const void**)(&temp_proxyInfo));

      /* Delete node from list */
      ds_dll_delete_node(proxy_addrList->addrListHead, &(proxy_addrList->addrListTail), node);

      //Free the proxyInfo entry structure
      if (proxyInfo != NULL)
      {
        ds_free (proxyInfo);
        proxyInfo = NULL;
      }

      //Free the memory of the linked list node
      ds_dll_free(node);

      if (proxy_addrList->addrListHead->next == NULL)
      {
        LOG_MSG_INFO1("No more entries",0,0,0);
        ds_dll_free(proxy_addrList->addrListHead);
        proxy_addrList->addrListHead = NULL;
        break;
      }
      node = temp_node;
      proxyInfo = temp_proxyInfo;
    }
    else
    {
      node = ds_dll_next (node, (const void**)(&proxyInfo));
    }
  }
}

/*===========================================================================
  FUNCTION SwitchToAPSTABackhaul
==========================================================================*/
/*!
@brief
  Switches current backhaul to AP-STA

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/

void QCMAP_Backhaul_WLAN::SwitchToAPSTABackhaul()
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);

  if (NULL != QcMapBackhaulMgr)
    QcMapBackhaulMgr->current_backhaul = BACKHAUL_TYPE_AP_STA_ROUTER;

  if (QcMapBackhaulWLANMgr->sta_v4_available)
  {
    QcMapBackhaulMgr->AllowBackhaulAccessOnIface(IP_V4, QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
    QcMapNatAlg->EnableNATonApps();
  }
  if (QcMapBackhaulWLANMgr->sta_v6_available)
  {
    if(QcMapBackhaulMgr)
    {
      QcMapBackhaulMgr->AllowBackhaulAccessOnIface(IP_V6, QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
      QcMapBackhaulMgr->EnableIPV6Forwarding();
    }
    if(QcMapFirewall)
      QcMapFirewall->EnableIPV6Firewall();
  }
  /* Enable IPv4 MCAST */
  LOG_MSG_INFO1(" Restart PIMD to enable MCAST forwarding for AP-STA \n",0,0,0);
  QcMapMgr->StopMcastDaemon();
  QcMapMgr->StartMcastDaemon();
}
