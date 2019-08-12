/*====================================================

FILE:  QCMAP_LAN.cpp

SERVICES:
   QCMAP Connection Manager LAN Specific Implementation

=====================================================

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*=========================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when          who        what, where, why
  --------   ---        -------------------------------------------------------
  08/26/14     ka          Created
  06/12/15     rk          Offtarget support.
==========================================================*/
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
#include "ds_qmi_qcmap_msgr.h"
#include "qualcomm_mobile_access_point_msgr_v01.h"
#include "qti_wlan_scm_msgr_v01.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_NATALG.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_LAN.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_WLAN_SCM.h"
#include "QCMAP_MediaService.h"
#include "QCMAP_BT_Tethering.h"

#define DHCP_HOSTS_FILE "/etc/dhcp_hosts"
#define DHCP_RECORD_LENGTH 100
#define MAX_DHCP_COMMAND_STR_LEN 4000
#define MTU_STRING_SIZE 5
#define DHCP_OPTION_MTU "26"

#define QCMAP_HOST_PATH "/etc/hosts"


bool QCMAP_LAN::flag= false;
QCMAP_LAN* QCMAP_LAN::object=NULL;

static FILE *dhcp_hosts_fp;


QCMAP_LAN* QCMAP_LAN::Get_Instance(boolean obj_create)
{
  if(!flag && obj_create)
  {
    qmi_error_type_v01 qmi_err_num;
    qcmap_media_svc_status dlna_mode=MEDIA_SVC_MODE_DOWN;
    qcmap_media_svc_status upnp_mode=MEDIA_SVC_MODE_DOWN;
    qcmap_media_svc_status mdns_mode=MEDIA_SVC_MODE_DOWN;
    LOG_MSG_INFO1("Creating object : LAN",0, 0, 0);
    object = new QCMAP_LAN();
    flag = true;
    QCMAP_MediaService *QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
    if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false)
    {
      if(QcMapMediaServiceMgr)
      {
        if (QcMapMediaServiceMgr->srvc_cfg.dlna_config)
          dlna_mode = MEDIA_SVC_MODE_UP;
        if (QcMapMediaServiceMgr->srvc_cfg.upnp_config)
          upnp_mode = MEDIA_SVC_MODE_UP;
         if (QcMapMediaServiceMgr->srvc_cfg.mdns_config)
          mdns_mode = MEDIA_SVC_MODE_UP;

      }
      else
      {
        QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_DLNA,GET_VALUE, &dlna_mode);
        QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_UPNP,GET_VALUE, &upnp_mode);
        QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_MDNS,GET_VALUE, &mdns_mode);
      }
      if (dlna_mode == MEDIA_SVC_MODE_UP)
      {
        if(!QCMAP_MediaService::EnableDLNA(&qmi_err_num))
        {
          LOG_MSG_ERROR("Cannot enable DLNA, error: %d.", qmi_err_num, 0, 0);
        }
      }
      if (upnp_mode == MEDIA_SVC_MODE_UP)
      {
        if(!QCMAP_MediaService::EnableUPNP(&qmi_err_num))
        {
          LOG_MSG_ERROR("Cannot enable UPnP, error: %d.", qmi_err_num, 0, 0);
        }
      }

      if (mdns_mode == MEDIA_SVC_MODE_UP)
      {
        if(!QCMAP_MediaService::EnableMDNS(&qmi_err_num))
        {
          LOG_MSG_ERROR("Cannot enable MDNS, error: %d.", qmi_err_num, 0, 0);
        }
      }
    }

    return object;
  }
  else
  {
    return object;
  }
}
/*==========================================================
                             Class Definitions
  =========================================================*/

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes Tethering variables.

@parameters

@return
  true  - on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

QCMAP_LAN::QCMAP_LAN()
{
  in_addr addr;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_msgr_lan_config_v01 lan_config;
  qcmap_msgr_station_mode_config_v01 station_config;
  memset(&this->prev_lan_config,0,sizeof(qcmap_msgr_lan_config_v01));
  memset(&(this->lan_cfg), 0,sizeof(this->lan_cfg));
  this->bridge_inited = false;
  this->enable_dns = true;
  this->lan_cfg.ip_passthrough_cfg.ip_passthrough_active = false;
  this->lan_cfg.num_dhcp_reservation_records=0;
  memset(&this->lan_cfg.ip_passthrough_cfg,0,sizeof(qcmap_cm_ip_passthrough_conf_t));
  if (QCMAP_ConnectionManager::xml_path)
  {
    if (this->ReadLANConfigFromXML())
    {
     /* DHCP config checks. */
      if((( this->lan_cfg.apps_ip_addr & this->lan_cfg.sub_net_mask) !=
          ( this->lan_cfg.dhcp_start_address & this->lan_cfg.sub_net_mask))||
          ( this->lan_cfg.dhcp_start_address <=this->lan_cfg.apps_ip_addr))
      {
        this->lan_cfg.dhcp_start_address =
        this->lan_cfg.apps_ip_addr+MAX_WIFI_CLIENTS;
        addr.s_addr =
        htonl(this->lan_cfg.dhcp_start_address );
        LOG_MSG_INFO1( "SSID1 DHCP Start addr in XML is invalid."
                       "Setting it to default value %s\n",inet_ntoa(addr),0,0);
      }
      if((( this->lan_cfg.apps_ip_addr &this->lan_cfg.sub_net_mask) !=
         ( this->lan_cfg.dhcp_end_address & this->lan_cfg.sub_net_mask))||
         ( this->lan_cfg.dhcp_end_address < this->lan_cfg.dhcp_start_address))
      {
        this->lan_cfg.dhcp_end_address =this->lan_cfg.dhcp_start_address + MAX_WIFI_CLIENTS;
        addr.s_addr =htonl(this->lan_cfg.dhcp_end_address );
        LOG_MSG_INFO1( "SSID1 DHCP End addr in XML is invalid."
                       "Setting it to default value %s\n",inet_ntoa(addr),0,0);
      }

      this->lan_cfg.ip_passthrough_cfg.rmnet_reserved_ip = inet_addr(WWAN_Reserved_IP);
      /*If dhcp lease time configured is less then 120 seconds, set it to default lease time */
      if(this->lan_cfg.dhcp_lease_time < MIN_DHCP_LEASE )
      {
        this->lan_cfg.dhcp_lease_time=DHCP_LEASE_TIME;
      }
      /* sync /etc/dhcp-hosts with the existing DHCP Reservation records*/
      dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "wb");
      if (NULL == dhcp_hosts_fp)
      {
          /* File could not be opened for writing/append*/
          LOG_MSG_ERROR("\n Failed to create /etc/dhcp_hosts!!",0,0,0);
      }
      else
      {
        this->sync_dhcp_hosts();
        fclose(dhcp_hosts_fp);
      }

      /*Check for address conflict between ap, guest ap and static sta configuration */
      lan_config.gw_ip = this->lan_cfg.apps_ip_addr;
      lan_config.netmask = this->lan_cfg.sub_net_mask;
      lan_config.enable_dhcp = this->lan_cfg.enable_dhcpd;
      lan_config.dhcp_config.lease_time = this->lan_cfg.dhcp_lease_time;
      if( this->lan_cfg.enable_dhcpd )
      {
        lan_config.dhcp_config.dhcp_start_ip = this->lan_cfg.dhcp_start_address;
        lan_config.dhcp_config.dhcp_end_ip = this->lan_cfg.dhcp_end_address;
      }

      QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(GET_VALUE, &station_config);

      if(CheckforAddrConflict(&lan_config,&station_config ))
      {
        station_config.conn_type = QCMAP_MSGR_STA_CONNECTION_DYNAMIC_V01;
        /*If there address conflict set STA mode to dynamic. */
        QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(SET_VALUE,
                                                       &station_config);
      }

      LOG_MSG_INFO1("QCMAP LAN Read XML OK\n",0,0,0);

      this->SetLANConfigToXML(&lan_config);
      /* Create the bridge Interface if not already created. */
      if ( !this->bridge_inited )
      {
        this->InitBridge();
      }
      return;
    }
    LOG_MSG_INFO1("QCMAP LAN Read XML failed.\n",0,0,0);
  }
  else
  {
    QCMAP_ConnectionManager::xml_path[0] = '\0';
  }
  this->lan_cfg.apps_ip_addr = ntohl(inet_addr(APPS_LAN_IP_ADDR));
  this->lan_cfg.sub_net_mask = ntohl(inet_addr(APPS_SUB_NET_MASK));
  this->lan_cfg.enable_dhcpd = true;
  this->lan_cfg.dhcp_start_address = ntohl(inet_addr(APPS_LAN_IP_ADDR)) + 20;
  this->lan_cfg.dhcp_end_address = ntohl(inet_addr(APPS_LAN_IP_ADDR)) + 20 + MAX_WIFI_CLIENTS;
  this->lan_cfg.dhcp_lease_time=DHCP_LEASE_TIME;

  return;
}

/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
  Destroying the LAN Object.

@parameters
  None

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

QCMAP_LAN::~QCMAP_LAN()
{
  qcmap_dhcp_information_info_list_t *dhcp_list = NULL;
  QCMAP_LAN* QCMAPLANMgr = QCMAP_LAN::Get_Instance(false);
  LOG_MSG_INFO1("Destroying Object: LAN",0,0,0);
  /* Delete the Bridge Interface. */
  this->DelBridge();
  dhcp_list = &(QCMAPLANMgr->lan_cfg.dhcp_reservation_records);
  ds_dll_delete_all(dhcp_list->dhcpInfoEntryListHead);

  dhcp_list->dhcpInfoEntryListHead = NULL;
  dhcp_list->dhcpInfoEntryListTail = NULL;

  flag=false;
  object=NULL;
}


/*=====================================================================
  FUNCTION check_non_empty_mac_addr
======================================================================*/
/*!
@brief
  Check for empty mac address

@return
  boolean

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static boolean QCMAP_LAN::check_non_empty_mac_addr(uint8 *mac, char mac_addr_string[])
{
  memset(mac_addr_string,0,QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01);
  snprintf( mac_addr_string,QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01,
            "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  if ( strncmp(mac_addr_string,MAC_NULL_STR,QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01)
       == 0 )
     return false;
  else
    return true;
}



/*===========================================================================
  FUNCTION qcmap_compare_dhcp_entries
==========================================================================*/
/*!
@brief
  To compare dhcp enteries.

@parameters
  None.

@return
  true  - If enetries match.
  false - otherwise.

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean qcmap_compare_dhcp_entries
(
  const void *first,
  const void *second
)
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  uint32 *entry_to_delete = (uint32*)first;
  qcmap_msgr_dhcp_reservation_v01*dhcp_entry =
                                       (qcmap_msgr_dhcp_reservation_v01*)second;

  if (dhcp_entry->client_reserved_ip == *entry_to_delete)
  {
    return false;//ds_dll expects zero if a match is found
  }
  return true;
}


/*=====================================================================
  FUNCTION find_record_delete_ip
======================================================================*/
/*!
@brief
  Search for a DHCP record based on IP and delete it from /etc/dhcp_hosts

@return
  boolean

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*====================================================================*/
boolean find_record_delete_ip(uint32_t client_reserved_ip)
{
  in_addr addr;
  char temp[DHCP_RECORD_LENGTH];
  boolean match_result = false;
  FILE *tmpfile = NULL;

  addr.s_addr = htonl(client_reserved_ip);
  tmpfile = fopen("/var/tmp_file","wb");
  if ( NULL == tmpfile)
  {
    LOG_MSG_ERROR("\nUnable to open /var/tmp_file file!!",0,0,0);
    return false;
  }
  while (fgets(temp,DHCP_RECORD_LENGTH,dhcp_hosts_fp)!= NULL )
  {
    if (( strstr(temp,inet_ntoa(addr)))!= NULL)
    {
      LOG_MSG_INFO1("IP-address: %s found!!\n",inet_ntoa(addr),0,0);
      match_result = true;
    }
    else
    {
      fputs(temp,tmpfile);
    }
  }
  if (match_result == false)
  {
    LOG_MSG_INFO1("IP-address: %s NOT found!!\n",inet_ntoa(addr),0,0);
  }
  fclose(tmpfile);
  fclose(dhcp_hosts_fp);
  snprintf(temp, sizeof(temp), "%s%s","mv /var/tmp_file ",DHCP_HOSTS_FILE);
  ds_system_call(temp,strlen(temp));
  return true;
}



/*=====================================================================
  FUNCTION SetLANConfig
======================================================================*/
/*!
@brief
  This function will set the lan configuration.

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_LAN::SetLANConfig(qcmap_msgr_lan_config_v01 *lan_config,
                                               qmi_error_type_v01 *qmi_err_num )
{
  in_addr addr;
  // Initialize QMI error
  *qmi_err_num = QMI_ERR_NONE_V01;
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  /* sync /etc/dhcp-hosts with the existing DHCP Reservation records*/
  dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "wb");
  if (NULL == dhcp_hosts_fp)
  {
      /* File could not be opened for writing/append*/
      LOG_MSG_ERROR("\n Failed to create /etc/dhcp_hosts!!",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
  }

  /* check for address conflicts before setting the wlan configuration */
  if( QCMAPLANMgr->CheckforAddrConflict(lan_config, NULL ))
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  if (lan_config != NULL)
  {
    QCMAPLANMgr->lan_cfg.apps_ip_addr = lan_config->gw_ip;
    QCMAPLANMgr->lan_cfg.sub_net_mask = lan_config->netmask;
    QCMAPLANMgr->lan_cfg.enable_dhcpd = lan_config->enable_dhcp;

     // Checking DHCP Configuration.
    if (QCMAPLANMgr->lan_cfg.enable_dhcpd )
    {
       QCMAPLANMgr->lan_cfg.dhcp_start_address = lan_config->dhcp_config.dhcp_start_ip;
       QCMAPLANMgr->lan_cfg.dhcp_end_address = lan_config->dhcp_config.dhcp_end_ip;

       QCMAPLANMgr->lan_cfg.dhcp_lease_time=lan_config->dhcp_config.lease_time;

       /* DHCP config checks. */
       if ( ( ( QCMAPLANMgr->lan_cfg.apps_ip_addr
            &QCMAPLANMgr->lan_cfg.sub_net_mask) !=
            ( QCMAPLANMgr->lan_cfg.dhcp_start_address &QCMAPLANMgr->lan_cfg.sub_net_mask))||
            ( QCMAPLANMgr->lan_cfg.dhcp_start_address <QCMAPLANMgr->lan_cfg.apps_ip_addr))
      {
        QCMAPLANMgr->lan_cfg.dhcp_start_address =QCMAPLANMgr->lan_cfg.apps_ip_addr+MAX_WIFI_CLIENTS;
        addr.s_addr =htonl(QCMAPLANMgr->lan_cfg.dhcp_start_address );
        LOG_MSG_INFO1("SSID1 DHCP Start addr provide is invalid."
                     "Setting it to default value %s\n",inet_ntoa(addr),0,0);
      }

      if ( ( ( QCMAPLANMgr->lan_cfg.apps_ip_addr & QCMAPLANMgr->lan_cfg.sub_net_mask) !=
          ( QCMAPLANMgr->lan_cfg.dhcp_end_address & QCMAPLANMgr->lan_cfg.sub_net_mask))||
          ( QCMAPLANMgr->lan_cfg.dhcp_end_address < QCMAPLANMgr->lan_cfg.dhcp_start_address))
      {
        QCMAPLANMgr->lan_cfg.dhcp_end_address =
       QCMAPLANMgr->lan_cfg.dhcp_start_address + MAX_WIFI_CLIENTS;
        addr.s_addr =
        htonl(QCMAPLANMgr->lan_cfg.dhcp_end_address );
        LOG_MSG_INFO1("SSID1 DHCP End addr provided is invalid."
                     "Setting it to default value %s\n",inet_ntoa(addr),0,0);
      }
    }
  }
  /*sync the /etc/dhcp-hosts file now*/
  QCMAPLANMgr->sync_dhcp_hosts();
  if ( dhcp_hosts_fp )
  {
    fclose(dhcp_hosts_fp);
  }
  if (lan_config != NULL)
  {
    QCMAP_LAN::SetLANConfigToXML(lan_config);
  }

  return true;

}
/*=====================================================================
  FUNCTION GetLANConfig
======================================================================*/
/*!
@brief
  This function will get the lan configuration.

@return
  true  - on Success
  false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_LAN::GetLANConfig(qcmap_msgr_lan_config_v01 *lan_config,
                                             qmi_error_type_v01 *qmi_err_num)
{
  uint32_t value_debug=0; //used to print the values of individual LAN config as LOG MSGs
  char str[INET6_ADDRSTRLEN];

  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  if ( lan_config == NULL )
  {
    LOG_MSG_ERROR("NULL params.\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }

  /* Populate AP Configuration. */
  lan_config->gw_ip = QCMAPLANMgr->lan_cfg.apps_ip_addr;
  lan_config->netmask =QCMAPLANMgr->lan_cfg.sub_net_mask;
  lan_config->enable_dhcp = QCMAPLANMgr->lan_cfg.enable_dhcpd;

  value_debug= htonl(lan_config->gw_ip);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP Gateway Address : '%s'",str,0,0);

  value_debug= htonl(lan_config->netmask);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP Subnet Mask : '%s'",str,0,0);

  LOG_MSG_INFO1("Enable DHCP: %b , ",lan_config->enable_dhcp,0,0)

  if ( lan_config->enable_dhcp == TRUE )
  {
    lan_config->dhcp_config.dhcp_start_ip = QCMAPLANMgr->lan_cfg.dhcp_start_address;
    lan_config->dhcp_config.dhcp_end_ip = QCMAPLANMgr->lan_cfg.dhcp_end_address;
    lan_config->dhcp_config.lease_time = QCMAPLANMgr->lan_cfg.dhcp_lease_time;
    value_debug= htonl(lan_config->dhcp_config.dhcp_start_ip);
    readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
    LOG_MSG_INFO1("\nDHCP Start Address : '%s'",str,0,0);
    value_debug= htonl(lan_config->dhcp_config.dhcp_end_ip);
    readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
    LOG_MSG_INFO1("\nDHCP End Address : '%s'",str,0,0);
    LOG_MSG_INFO1("\nDHCP Lease Time: '%d'",lan_config->dhcp_config.lease_time,0,0);

  }

  return true;
}


/*=====================================================================
  FUNCTION ActivateLAN
======================================================================*/
/*!
@brief
  Brings up the lan with the latest wlan configuration and mode.

@parameters
  admin - If set to true forcefully re-activate LAN, even if no change in cfg.

@return
  true  - on Success
  false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_LAN::ActivateLAN
(
  qmi_error_type_v01 *qmi_err_num,
  boolean admin
)
{
  LOG_MSG_INFO1("QCMAP_ConnectionManager::ActivateLAN",0,0,0);
  boolean teth_link1_enable = false, teth_link2_enable = false;
  boolean wlan_enable = false;
  char teth_intf_name1 [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char teth_intf_name2 [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char command[MAX_COMMAND_STR_LEN];
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE], subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char teth_gw_ip1[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char teth_gw_ip2[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char teth_netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  struct in_addr addr;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(FALSE);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  QCMAP_BT_TETHERING* QcMapBTTethMgr=QCMAP_BT_TETHERING::Get_Instance(false);
  qmi_qcmap_msgr_status_cb_data *cbPtr = NULL;

  // Wlan related addresses
  if (QcMapWLANMgr)
  {
    addr.s_addr =
    htonl(QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_ip_addr);
    strlcpy(a5_ip, inet_ntoa(addr), 16);
    addr.s_addr =
    htonl(QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
    strlcpy(netmask, inet_ntoa(addr), 16);
  }
  // USB related link local address
  if(QcMapTetheringMgr)
  {
    // TETH related link local address
    addr.s_addr = htonl(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].gateway_addr);
    strlcat(teth_gw_ip1, inet_ntoa(addr), 16);
    addr.s_addr = htonl(QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].gateway_addr);
    strlcat(teth_gw_ip2, inet_ntoa(addr), 16);
    addr.s_addr =
    htonl(QcMapTetheringMgr->ll_subnet_mask);
    strlcpy(teth_netmask, inet_ntoa(addr), 16);
  }
  // Sanity check to ensure that we only proceed if either the mode or AP configuration
  //has changed
  // Added check to ensure when in AP-STA bridge mode; ActivateLAN is also called from
  //DHCP Fail Case
  if (!QCMAPLANMgr->IsLanCfgUpdated() && !admin)
  {
    LOG_MSG_INFO1(" There is no change in the LAN Config",0,0,0);
    return true;
  }
  bzero(teth_intf_name1,sizeof(teth_intf_name1));
  bzero(teth_intf_name2,sizeof(teth_intf_name2));
  // AP Configuration has changed and Bridge interface was up, need to apply the new configuration.
  // This will also take care if the mode has changed
  if (QCMAPLANMgr->bridge_inited)
  {
    // Bring down USB TE
    if (QcMapTetheringMgr)
    {
      if ( QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
           (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type !=
                    QCMAP_QTI_TETHERED_LINK_PPP))
      {
         teth_link1_enable = true;
         if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type
                                     == QCMAP_QTI_TETHERED_LINK_RNDIS )
         {
           strlcpy(teth_intf_name1, "rndis0",sizeof(teth_intf_name1));
         }
         else if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type
                                     == QCMAP_QTI_TETHERED_LINK_ECM )
         {
           strlcpy(teth_intf_name1, "ecm0",sizeof(teth_intf_name1));
         }
         else
         {
           LOG_MSG_ERROR("Incorrect USB LINK Detected Link Type = %x",
                         QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,0,0);
           *qmi_err_num = QMI_ERR_INTERNAL_V01;
           return false;
         }
         ds_system_call("echo 0 > /sys/class/android_usb/android0/enable ",
                         strlen("echo 0 > /sys/class/android_usb/android0/enable "));
         snprintf( command, MAX_COMMAND_STR_LEN, "brctl delif %s %s",BRIDGE_IFACE,teth_intf_name1);
         ds_system_call(command, strlen(command));
      }
     else if ( QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
               (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
                    QCMAP_QTI_TETHERED_LINK_PPP))
      {
        /* Kill the PPP Daemon. Right now there is no way we can make PPP to renegotiate. */
        ds_system_call("killall -TERM pppd", strlen("killall -TERM pppd"));
      }

     if ( QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) )
     {
         teth_link2_enable = true;
         if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type == QCMAP_QTI_TETHERED_LINK_ETH )
         {
             strlcpy(teth_intf_name2, ETH_IFACE,sizeof(teth_intf_name2));
         } else
         {
            LOG_MSG_ERROR("Incorrect USB LINK Detected Link Type = %x",
                          QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type,0,0);
           *qmi_err_num = QMI_ERR_INTERNAL_V01;
           return false;
         }
         snprintf( command, MAX_COMMAND_STR_LEN, "brctl delif %s %s",BRIDGE_IFACE,teth_intf_name2);
         ds_system_call(command, strlen(command));
     }
    }
    // Disable WLAN if we are not in STA mode and WLAN is enabled
    // Also Disable WLAN if we are in STA mode and admin=true; DHCP Fail case
    if (QcMapWLANMgr && ( (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_V01) ||
        (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_V01) ||
        (admin == true)))
    {
      wlan_enable = true;
      QCMAP_WLAN::DisableWLAN(qmi_err_num);
    }
    else if (QcMapWLANMgr && QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
    {
      /* Disable WLAN and remove wlan from bridge Iface */
      snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig wlan%d down",QcMapMgr->ap_dev_num1);
      ds_system_call(command, strlen(command));
      wlan_enable = true;

      if (!QcMapMgr->dual_wifi_mode)
      {
        QcMapWLANMgr->StopHostAPD();
      }
      else
      {
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_1_V01, SCM_MSGR_WLAN_IF_STOP_V01); //check if we would need to stop guest AP in AP-AP mode
      }

      snprintf( command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
                BRIDGE_IFACE,QcMapMgr->ap_dev_num1);
      ds_system_call(command,strlen(command));
    }
    // Bring down BT Tethering & Send Indication If BT Tethering is UP
    if (QcMapBTTethMgr && QcMapBTTethMgr->bt_tethering_status ==
             QCMAP_MSGR_BT_TETHERING_MODE_UP_V01)
    {
      if (!QCMAP_BT_TETHERING::BringdownBTTethering(qmi_err_num))
      {
        LOG_MSG_ERROR("Error in Bring down BT tethering",0,0,0);
        return false;
      }
      else
      {
        cbPtr = (qmi_qcmap_msgr_status_cb_data *)QcMapMgr->GetQCMAPClientCBDatata();
        //send indication
        if (cbPtr && cbPtr->bt_tethering_status_cb)
        {
          LOG_MSG_INFO1(" Sending bt TETHERING BRING DOWN Indication to BT module. ",0,0,0);
          cbPtr->bt_tethering_status_cb(cbPtr, QCMAP_MSGR_BT_TETHERING_MODE_DOWN_V01);
        }
      }
    }

    // Disable Bridge force-fully.
    if (QCMAPLANMgr->bridge_inited)
    {
      LOG_MSG_INFO1(" Delete the bridge interface. forcefully \n",0,0,0);
      QCMAPLANMgr->DelBridge();
    }

    QCMAPLANMgr->InitBridge();

    // Enable WLAN
    if (wlan_enable)
    {
      sleep(QcMapMgr->wlan_restart_delay);

      if ((!QcMapWLANMgr || QcMapWLANMgr->wifi_mode!=
           QCMAP_MSGR_WLAN_MODE_AP_STA_V01) ||(admin == true))
      {
        QCMAP_WLAN::EnableWLAN(qmi_err_num);
      }
      else
      {
        if (!QcMapMgr->dual_wifi_mode)
        {
          // Start Hostapd
          QcMapWLANMgr->StartHostAPD(QCMAP_MSGR_INTF_AP_INDEX, QcMapMgr->ap_dev_num1);
        }
        else
        {
          QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_1_V01, SCM_MSGR_WLAN_IF_START_V01);
        }

        //add to bridge
        snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig wlan%d %s netmask %s up",
                 QcMapMgr->ap_dev_num1, a5_ip, netmask);
        ds_system_call(command, strlen(command));

        addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface
                            [QCMAP_MSGR_INTF_AP_INDEX].ll_ip_addr &
                            QcMapWLANMgr->wlan_cfg.interface
                            [QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);

         strlcpy(subnet, inet_ntoa(addr), 16);
         snprintf(command, MAX_COMMAND_STR_LEN,
                  "route del -net %s netmask %s dev wlan%d", subnet, netmask,
                  QcMapMgr->ap_dev_num1);
         ds_system_call(command, strlen(command));
         snprintf( command, MAX_COMMAND_STR_LEN, "brctl addif %s wlan%d",
                   BRIDGE_IFACE,QcMapMgr->ap_dev_num1);
         ds_system_call(command,strlen(command));
      }
    }

    // Enable USB if it was enabled before
    if (teth_link1_enable)
    {
      ds_system_call("echo 1 > /sys/class/android_usb/android0/enable ",
                     strlen("echo 1 > /sys/class/android_usb/android0/enable "));
      snprintf( command, MAX_COMMAND_STR_LEN, "ifconfig %s %s netmask %s up",
                teth_intf_name1,teth_gw_ip1,teth_netmask);
      ds_system_call(command, strlen(command));
      snprintf( command, MAX_COMMAND_STR_LEN,
                "brctl addif %s %s",BRIDGE_IFACE,teth_intf_name1);
      ds_system_call(command, strlen(command));
    }

    if (teth_link2_enable)
    {
      snprintf( command, MAX_COMMAND_STR_LEN, "brctl addif %s %s",
                BRIDGE_IFACE,teth_intf_name2);
      ds_system_call(command, strlen(command));
      QCMAP_LAN::ToggleAutoNegForEth(teth_intf_name2);
    }
  }
  return true;
}


/*=====================================================================
  FUNCTION IsLanCfgUpdated
======================================================================*/
/*!
@brief
  Check is current applied LAN config and the prev LAN config are the same

@parameters
   none

@return
   returns succesful if prev and current lan config are different

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_LAN::IsLanCfgUpdated(void)
{

  if ((this->lan_cfg.apps_ip_addr != this->prev_lan_config.gw_ip) ||
       (this->lan_cfg.sub_net_mask != this->prev_lan_config.netmask) ||
       (this->lan_cfg.enable_dhcpd != this->prev_lan_config.enable_dhcp)) {
     return true;
  }

  if (this->lan_cfg.enable_dhcpd) {
    if ((this->lan_cfg.dhcp_start_address != this->prev_lan_config.
         dhcp_config.dhcp_start_ip) ||
        (this->lan_cfg.dhcp_end_address != this->prev_lan_config.
         dhcp_config.dhcp_end_ip ) ||
        (this->lan_cfg.dhcp_lease_time!= this->prev_lan_config.
         dhcp_config.lease_time)){
       return true;
    }
    /* Check if the reservations are updated. */
    if (this->lan_cfg.dhcp_reservations_updated == TRUE)
    {
      return true;
    }
  }
  return false;
}




/*=====================================================================
  FUNCTION StartDHCPD
======================================================================*/
/*!
@brief
  This function will start the dhcp service with the configured values
  for the enabled interfaces.

@parameters
  void

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_LAN::StartDHCPD()
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];
  char scratch_buf[MAX_COMMAND_STR_LEN];
  in_addr start, end, subnet_mask, gw_addr;
  QCMAP_CM_LOG_FUNC_ENTRY();

  ds_system_call("echo QCMAP:Start DHCP server > /dev/kmsg",
          strlen("echo QCMAP:Start DHCP server > /dev/kmsg"));

  if (this->lan_cfg.dhcp_start_address == NULL || this->lan_cfg.dhcp_end_address == NULL
      || this->lan_cfg.sub_net_mask == NULL || this->lan_cfg.dhcp_lease_time == NULL)
  {

    ds_system_call("echo QCMAP:Start DHCP server FAILED > /dev/kmsg",
          strlen("echo QCMAP:Start DHCP server FAILED > /dev/kmsg"));
    return false;
  }

  bzero(command, MAX_COMMAND_STR_LEN);
  bzero(scratch_buf, MAX_COMMAND_STR_LEN);
  if (this->lan_cfg.enable_dhcpd)
  {
    strlcpy(command, "dnsmasq", MAX_COMMAND_STR_LEN);
    snprintf(scratch_buf, MAX_COMMAND_STR_LEN, " -i %s -I lo -z", BRIDGE_IFACE);
    strlcat(command, scratch_buf, MAX_COMMAND_STR_LEN);

    strlcat(command, "  --dhcp-range=", MAX_COMMAND_STR_LEN);
    /* Insert a GetInterfaceName call here.*/
    snprintf(scratch_buf, MAX_COMMAND_STR_LEN, "%s,", BRIDGE_IFACE);

    strlcat(command, scratch_buf, MAX_COMMAND_STR_LEN);

    start.s_addr = htonl(this->lan_cfg.dhcp_start_address);
    /* INET_NTOA_SIZE is the size of static buff used in inet_ntoa function */
    strlcat(command, inet_ntoa(start), MAX_COMMAND_STR_LEN);

    strlcat(command, ",", MAX_COMMAND_STR_LEN);

    /* Reserve IP for PPP only if we have more than 1 IP available.
       Also check if it is in DUN+SoftAp Mode*/
    if (( this->lan_cfg.dhcp_end_address >
          this->lan_cfg.dhcp_start_address) && (QcMapMgr->DUN_SoftAP_enable_flag))
    {
      end.s_addr =
             htonl(this->lan_cfg.dhcp_end_address - 1);
      this->lan_cfg.ppp_reserved_ip = this->lan_cfg.dhcp_end_address;
    }
    else
    {
      LOG_MSG_INFO1("No IP reserved for DUN client", 0, 0, 0);
      end.s_addr =htonl(this->lan_cfg.dhcp_end_address);
      this->lan_cfg.ppp_reserved_ip = 0;
    }
    /* INET_NTOA_SIZE is the size of static buff used in inet_ntoa function */
    strlcat(command, inet_ntoa(end), MAX_COMMAND_STR_LEN);

    strlcat(command, ",", MAX_COMMAND_STR_LEN);

    subnet_mask.s_addr = htonl(this->lan_cfg.sub_net_mask);
    strlcat(command, inet_ntoa(subnet_mask), MAX_COMMAND_STR_LEN);

    strlcat(command, ",", MAX_COMMAND_STR_LEN);

    snprintf( scratch_buf, MAX_COMMAND_STR_LEN,"%d",this->lan_cfg.dhcp_lease_time);
    strlcat(command , scratch_buf , MAX_COMMAND_STR_LEN);

    /*if dhcp-hostsfile exits append this to the dnsmasq option*/
    dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "rb");
    /* File could not be opened for writing/append*/
    if (NULL == dhcp_hosts_fp)
    {
        LOG_MSG_INFO1("\nFile /etc/dhcp_hosts does not exists!!",0,0,0);
    }
    else
    {

        strlcat(command, " --dhcp-hostsfile=", MAX_COMMAND_STR_LEN);
        /* Insert the DHCP hosts file.*/
        snprintf(scratch_buf, MAX_COMMAND_STR_LEN, "%s", DHCP_HOSTS_FILE);
        strlcat(command, scratch_buf, MAX_COMMAND_STR_LEN);
        fclose(dhcp_hosts_fp);
    }

    if (this->lan_cfg.apps_ip_addr != NULL)
    {
      /* DNS Gateway address, to used in DHCP ACK message*/
      strlcat(command, " --dhcp-option-force=6,", MAX_COMMAND_STR_LEN);
      gw_addr.s_addr = htonl(this->lan_cfg.apps_ip_addr);
      strlcat(command, inet_ntoa(gw_addr), MAX_COMMAND_STR_LEN);
    }

    /* Copy DHCP command. Used when LAN config has not been re-activated
       with changed values, but few other components of DHCPD change. */
    strlcpy(this->dhcp_command_str, command, MAX_COMMAND_STR_LEN);

    /*Update DHCPD daemon with network SIP server, MTU info */
    this->UpdateDHCPDNetworkInfo();
  }

  ds_system_call("echo QCMAP:DHCP server started > /dev/kmsg",
          strlen("echo QCMAP:DHCP server started > /dev/kmsg"));
  return true;
}


/*=====================================================================
  FUNCTION StopDHCPD
======================================================================*/
/*!
@brief
  Stops the dhcp services started.

@parameters
  void

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/

boolean QCMAP_LAN::StopDHCPD()
{
  char command[MAX_COMMAND_STR_LEN];

   snprintf(command,MAX_COMMAND_STR_LEN, " killall -9 dnsmasq");
   ds_system_call(command, strlen(command));

/*-------------------------------------------------------------------------
  Delete the DNS masq leases which were allocated earlier. This is
  required because
  - the dnsmasq lease file stores the MAC address to IP address mapping
    and does not allocate the stored IP addresses to DHCP requests
    coming in with different MAC address
  - but with every reboot in case of USB the host uses a different MAC address
  - thus in order to assign the one constant IP address to the USB tethered
    device we delete this mapping.
--------------------------------------------------------------------------*/
  snprintf(command, MAX_COMMAND_STR_LEN,
             "rm /var/lib/misc/dnsmasq.leases");
  ds_system_call(command, strlen(command));

  /* Clear off the reservatiosn flag. */
  this->lan_cfg.dhcp_reservations_updated = FALSE;
  QCMAP_CM_LOG_FUNC_ENTRY();

  return true;
}


/*=====================================================================
  FUNCTION AddDHCPReservRecord
======================================================================*/
/*!
@brief
  Checks existing DHCP Reservation Records for the redundancy and if unique
  adds a new DHCP Reservation Record.

@parameters
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_LAN::AddDHCPReservRecord
( qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record,
  qmi_error_type_v01 *qmi_err_num
)
{

  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  int i;
  in_addr addr;
  boolean mac_addr_non_empty = true, client_ip_match = false;
  int  client_mac_match = 0, client_name_match = 0;
  qcmap_msgr_dhcp_reservation_v01 *local_rec = NULL;
  uint32 num_records = 0;

  qcmap_msgr_dhcp_reservation_v01 *dhcp_record = NULL;
  qcmap_dhcp_information_info_list_t* dhcpList =
                               &(QCMAPLANMgr->lan_cfg.dhcp_reservation_records);
  ds_dll_el_t * node = NULL, *temp_node = NULL;

  QCMAP_CM_LOG_FUNC_ENTRY();


  num_records = QCMAPLANMgr->lan_cfg.num_dhcp_reservation_records;

   /* Check if we have already reached a max limit on the records*/
  if ( num_records  == QCMAP_MSGR_MAX_DHCP_RESERVATION_ENTRIES_V01 )
  {
    /* we reach MAX entries */
    LOG_MSG_ERROR("\nReached Max limit on DHCP Reservation Record Entries!!",0,0,0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return false;
  }

  node = dhcpList->dhcpInfoEntryListHead;
  node = ds_dll_next (node, (const void**)(&dhcp_record));

  mac_addr_non_empty = check_non_empty_mac_addr(dhcp_reserv_record->client_mac_addr,
                                                mac_addr_string);

  /*check for duplicate entries*/
  for ( i = 0; i < num_records && dhcp_record; i++ )
  {

    local_rec = dhcp_record;
    client_ip_match = ( dhcp_reserv_record->client_reserved_ip ==
                        local_rec->client_reserved_ip );
    client_mac_match = memcmp(dhcp_reserv_record->client_mac_addr,
                              local_rec->client_mac_addr,
                              sizeof(dhcp_reserv_record->client_mac_addr));
    client_name_match = memcmp(dhcp_reserv_record->client_device_name,
                               local_rec->client_device_name,
                               QCMAP_MSGR_DEVICE_NAME_MAX_V01);

    if ( ( ( 0 == client_mac_match )&& mac_addr_non_empty ) ||
         ( ( 0 == client_name_match )&&
            dhcp_reserv_record->client_device_name[0] != '\0' )||
         ( client_ip_match ) )
    {
      /* we already added this one */
      LOG_MSG_ERROR("\n IP Address already reserved for another Client!!",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
    node = ds_dll_next (node, (const void**)(&dhcp_record));
  }

  /* if  <ipaddr>  is gievn outside the AP dhcp-range , throw an error */
  if ( ( QCMAPLANMgr->lan_cfg.dhcp_start_address >
                   dhcp_reserv_record->client_reserved_ip) ||
        ( (QCMAPLANMgr->lan_cfg.dhcp_end_address - 1) <
                  dhcp_reserv_record->client_reserved_ip) )
  {
      LOG_MSG_ERROR("\nIP address is outside the dhcp range!!",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
  }

  /* if DHCP enable flag is FALSE for a record during addition, then the corresponding
     record would be not added in the dhcp-hosts file.
     however, we would be still adding the record in the XML */
  if ( dhcp_reserv_record->enable_reservation )
  {
   /* If we have reached this far, then check if /etc/dhcp_hosts file exists.
     if it does not, then create a file and add the following per line
     for AP clients:
     [<hwaddr>] [,<hostname>] [,<ipaddr>]
     for USB client:
     [,<hostname>] [,<ipaddr>]
   */
   dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "ab");

   /* File could not be opened for writing/append*/
   if (NULL == dhcp_hosts_fp)
   {
      LOG_MSG_ERROR("\nUnable to open /etc/dhcp_hosts file!!",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
   }
   else
   {
     if ( dhcp_reserv_record->client_device_name[0] != '\0' )
     {
         fprintf(dhcp_hosts_fp,"%s,",dhcp_reserv_record->client_device_name);
     }
     /* mac address is not provided for USB client*/
     if ( mac_addr_non_empty )
     {
       fprintf(dhcp_hosts_fp,"%s,",mac_addr_string);
     }
     addr.s_addr = htonl(dhcp_reserv_record->client_reserved_ip);
     fprintf(dhcp_hosts_fp,"%s\n",inet_ntoa(addr));
     fclose(dhcp_hosts_fp);
     QCMAPLANMgr->lan_cfg.dhcp_reservations_updated = TRUE;
   }
  }

  /* save into the config */
  /* TODO: Have to move the below code to firewall constructor to avoid it from doing
    while adding a Firewall entry and then while fetching from XML file. */
   if (dhcpList->dhcpInfoEntryListHead == NULL )
    {
      /*The first node which is created is a dummy node which does not store any device
             information. This is done to make use of the doubly linked list framework which
             is already existing*/
      if (( node = ds_dll_init(NULL)) == NULL)
      {
        LOG_MSG_ERROR("AddDHCPReservRecord - Error in allocating memory for node",
                      0,0,0);
        return false;
      }
      dhcpList->dhcpInfoEntryListHead = node;
    }

   dhcp_record = (qcmap_msgr_dhcp_reservation_v01*)ds_malloc
                                      (sizeof(qcmap_msgr_dhcp_reservation_v01));

   if( dhcp_record == NULL )
   {
     LOG_MSG_ERROR("AddDHCPReservRecord - Error in allocating memory for"
                   "dhcp reservation entry",0,0,0);
     return false;
   }

   memset(dhcp_record, 0, sizeof(qcmap_msgr_dhcp_reservation_v01));

  memcpy(dhcp_record, dhcp_reserv_record,
         sizeof(qcmap_msgr_dhcp_reservation_v01));

  //Store the dhcp entry in the linked list
  if ((node = ds_dll_enq(dhcpList->dhcpInfoEntryListHead,
                           NULL, (void*)dhcp_record)) == NULL)
  {
      LOG_MSG_ERROR("AddDHCPReservRecord - Error in adding a node",0,0,0);
      ds_free(dhcp_record);
      return false;
  }
  dhcpList->dhcpInfoEntryListTail = node;
  QCMAPLANMgr->lan_cfg.num_dhcp_reservation_records++;

  LOG_MSG_INFO1("\n Writing the DHCP record to XML config!!",0,0,0);
  QCMAPLANMgr->AddDHCPResRecToXML(dhcp_record);
  //Adding to XML here if all the conditions are met.
  return true;
}

/*=====================================================================
  FUNCTION EditDHCPReservRecord
======================================================================*/
/*!
@brief
  Edit a DHCP record

@parameters
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_LAN::EditDHCPReservRecord
( uint32_t *client_ip,
  qcmap_msgr_dhcp_reservation_v01 *record,
  qmi_error_type_v01 *qmi_err_num
)
{
  int i,j;
  in_addr addr;
  boolean mac_addr_non_empty = true;
  boolean match_result = false, client_ip_match = false;
  uint32 num_records = 0;
  int  client_mac_match = 0, client_name_match = 0;
  qcmap_msgr_dhcp_reservation_v01 *local_rec = NULL;
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];/*char array of mac addr*/
  qcmap_msgr_dhcp_reservation_v01 *dhcp_record = NULL;
  qcmap_dhcp_information_info_list_t* dhcpList =
                               &(QCMAPLANMgr->lan_cfg.dhcp_reservation_records);

  ds_dll_el_t * node = NULL;

  QCMAP_CM_LOG_FUNC_ENTRY();


  node = dhcpList->dhcpInfoEntryListHead;
  node = ds_dll_next (node, (const void**)(&dhcp_record));

  /*check if record is existing*/
  num_records = QCMAPLANMgr->lan_cfg.num_dhcp_reservation_records;
  for ( i = 0; i < num_records && dhcp_record; i++ )
  {
    if (*client_ip == dhcp_record->client_reserved_ip )
      break;
    node = ds_dll_next (node, (const void**)(&dhcp_record));
  }
  if ( i == num_records  || !dhcp_record)
  {
      /* Record not found */
      LOG_MSG_ERROR("\nDHCP record not found!!",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
  }

  /* if  updated <ipaddr>  is gievn outside the AP dhcp-range , throw an error */
  if ( record->client_reserved_ip != 0 )
  {
    if ( ( QCMAPLANMgr->lan_cfg.dhcp_start_address >
                   record->client_reserved_ip) ||
        ( (QCMAPLANMgr->lan_cfg.dhcp_end_address - 1) <
                  record->client_reserved_ip) )
    {
      LOG_MSG_ERROR("\nIP address is outside the dhcp range!!",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
  }
  /*Check if the mac address is not entered by the user
  If its empty then we have don't have edit the mac address
  for AP client*/
  mac_addr_non_empty = check_non_empty_mac_addr(record->client_mac_addr,
                                                mac_addr_string);

  /*mac address should not be edited for USB clients.Return error*/
  if ( ( check_non_empty_mac_addr(dhcp_record->client_mac_addr, mac_addr_string) == false ) &&
        ( mac_addr_non_empty == true ) )
  {
      LOG_MSG_ERROR("\nMAC address cannot be entered for USB client!!",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
  }


  node = ds_dll_next (dhcpList->dhcpInfoEntryListHead, (const void**)(&local_rec));

  /*check for duplicate entries*/
  /* We need to increment J to skip the node below, but should go till node is NULL */
  for ( j = 0; node != NULL; j++ )
  {
    /*skip the record which has been matched already*/
    if ( j == i )
    {
      continue;
    }
    client_ip_match = ( local_rec->client_reserved_ip == record->client_reserved_ip );
    client_mac_match = memcmp(record->client_mac_addr,local_rec->client_mac_addr,
                              sizeof(record->client_mac_addr));
    client_name_match = strncmp(record->client_device_name,local_rec->client_device_name,
                                strlen(record->client_device_name));

    if ( ( ( 0 == client_mac_match )&& mac_addr_non_empty ) ||
         ( ( 0 == client_name_match )&& record->client_device_name[0] != '\0') ||
         client_ip_match )
    {
      /* we already added this one */
      LOG_MSG_ERROR("\n IP Address already reserved for another Client!!",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
    node = ds_dll_next (node, (const void**)(&local_rec));
  }
  local_rec = dhcp_record;
   /*replace the existing record with the records that were entered*/
  if ( mac_addr_non_empty )
  {
     memcpy(local_rec->client_mac_addr,record->client_mac_addr,
            sizeof(local_rec->client_mac_addr));
  }
  if ( record->client_reserved_ip != 0)
  {
     local_rec->client_reserved_ip = record->client_reserved_ip;
  }
  if ( record->client_device_name[0] != '\0')
  {
     memcpy(local_rec->client_device_name,record->client_device_name,
            sizeof(local_rec->client_device_name));
  }
  if (local_rec->enable_reservation != record->enable_reservation)
  {
     local_rec->enable_reservation = record->enable_reservation;
  }

  /* check if dhcp-host files exists. it could be possible that dhcp-host files
      has been removed from the user space. if thats the case, create the file */
  dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "rb+");
  /* File could not be opened for writing/append*/
  if (NULL == dhcp_hosts_fp)
  {
       /*dhcp-hosts files might have been removed from user space.
      creating a new file below*/
      LOG_MSG_INFO1("\nUnable to open /etc/dhcp_hosts file for read!!",0,0,0);
  }
  else
  {
     /* if dhcp-hosts files exits, search for the record based on ip-address.
     if found, delete the same. If not found , it could be possible that
     the entry might have been removed from the user space.
     Go ahead and add the record if dhcp-reservation flag is true*/
     if ( false == ( match_result = find_record_delete_ip(*client_ip)) )
     {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      fclose(dhcp_hosts_fp);
      return false;
     }
  }

  if ( match_result == true ||record->enable_reservation == true)
  {
    dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "ab");
    /* File could not be opened for writing/append*/
    if (NULL == dhcp_hosts_fp)
    {
        LOG_MSG_ERROR("\nUnable to open /etc/dhcp_hosts file!!",0,0,0);
        *qmi_err_num = QMI_ERR_INTERNAL_V01;
        return false;
    }
    else
    {
       if ( record->enable_reservation == true )
       {
           /* If we have reached this far, then edit as below
             for AP clients:
             [<hwaddr>] [,<hostname>] [,<ipaddr>]
             for USB client:
             [,<hostname>] [,<ipaddr>]
            */
         if ( local_rec->client_device_name[0] != '\0')
         {
           fprintf(dhcp_hosts_fp,"%s,",local_rec->client_device_name);
         }
         /* mac address is not provided for USB client*/
         if ( check_non_empty_mac_addr(local_rec->client_mac_addr, mac_addr_string))
         {
             fprintf(dhcp_hosts_fp,"%s,",mac_addr_string);
         }
         addr.s_addr = htonl(local_rec->client_reserved_ip);
         fprintf(dhcp_hosts_fp,"%s\n",inet_ntoa(addr));
       }
       fclose(dhcp_hosts_fp);
       QCMAPLANMgr->lan_cfg.dhcp_reservations_updated = TRUE;
    }

  /* if it is OK, call write to XML */

  QCMAPLANMgr->EditDHCPResRecToXML(*client_ip, local_rec);
  }

  return true;
}


/*=====================================================================
  FUNCTION DeleteDHCPReservRecord
======================================================================*/
/*!
@brief
  Delete a DHCP record

@parameters
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_LAN::DeleteDHCPReservRecord
( uint32_t *client_reserved_ip,
  qmi_error_type_v01 *qmi_err_num
)
{
  int i, j;
  uint32 num_records = 0;
  qcmap_msgr_dhcp_reservation_v01 *dhcp_record = NULL;
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  qcmap_dhcp_information_info_list_t* dhcpList =
                               &(QCMAPLANMgr->lan_cfg.dhcp_reservation_records);
  ds_dll_el_t * node = NULL, *temp_node = NULL;

  QCMAP_CM_LOG_FUNC_ENTRY();

  LOG_MSG_INFO1("client IP is 0x%x",*client_reserved_ip,0,0);

  if (!(node = ds_dll_delete( dhcpList->dhcpInfoEntryListHead,
      &(dhcpList->dhcpInfoEntryListTail),
        (void*)client_reserved_ip, qcmap_compare_dhcp_entries)))

  {
    LOG_MSG_ERROR("DeleteDHCPReservRecord - Error in finding the entry",0,0,0);
    return false;
  }



 /* check if dhcp-host files exists. if
      it does not exists its fine. this might have
      been removed from the user space. Return TRUE*/
    dhcp_hosts_fp = fopen(DHCP_HOSTS_FILE, "rb+");
  /* File could not be opened for writing/append*/
  if (NULL == dhcp_hosts_fp)
  {
      LOG_MSG_INFO1("\nFile /etc/dhcp_hosts does not exists!!",0,0,0);
  }
  else
  {
     /* if dhcp-hosts files exits, seach for the record based on ip-address.
     if found, delete the same. If not found , it could be possible that
     the entry might have been already removed from the user space.*/
     if ( false == find_record_delete_ip(*client_reserved_ip) )
     {
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      fclose(dhcp_hosts_fp);
      return false;
     }
     QCMAPLANMgr->lan_cfg.dhcp_reservations_updated = TRUE;
  }


  dhcp_record = (qcmap_msgr_dhcp_reservation_v01*)ds_dll_data(node);
  if (dhcp_record == NULL)
  {
    LOG_MSG_ERROR("DeleteStaticNatEntry - NAT Info is NULL", 0,0,0);
    //Free the memory of the linked list node
    ds_dll_free(node);
    node = NULL;
    return false;
  }

  //Free the device information structure
  ds_free (dhcp_record);
  dhcp_record = NULL;

  //Free the memory of the linked list node
  ds_dll_free(node);
  node = NULL;

  QCMAPLANMgr->lan_cfg.num_dhcp_reservation_records--;

  /* if it is OK, call write to XML */

  QCMAPLANMgr->DeleteDHCPResRecFromXML(*client_reserved_ip);
  return true;
}


/*=====================================================================
  FUNCTION GetDHCPReservRecords
======================================================================*/
/*!
@brief
  Displays all the DHCP Reservation Records.

@parameters
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_records

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_LAN::GetDHCPReservRecords
( qcmap_msgr_dhcp_reservation_v01* dhcp_reservation_record,
  unsigned int* num_entries,
  qmi_error_type_v01 *qmi_err_num
)
{
  int i;

  QCMAP_CM_LOG_FUNC_ENTRY();
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  qcmap_msgr_dhcp_reservation_v01 *dhcp_record = NULL;
  qcmap_dhcp_information_info_list_t* dhcpList =
                               &(QCMAPLANMgr->lan_cfg.dhcp_reservation_records);
  ds_dll_el_t * node = NULL;


  node = dhcpList->dhcpInfoEntryListHead;
  node = ds_dll_next (node, (const void**)(&dhcp_record));


  *num_entries = QCMAPLANMgr->lan_cfg.num_dhcp_reservation_records;
  for ( i=0; i < *num_entries && dhcp_record; i++ )
  {
    memcpy(&dhcp_reservation_record[i], dhcp_record,
           sizeof(qcmap_msgr_dhcp_reservation_v01));
    node = ds_dll_next (node, (const void**)(&dhcp_record));
  }
  return true;
}


/*===========================================================================
  FUNCTION UpdateDHCPDNetworkInfo
==========================================================================*/
/*!
@brief
  This function will re-start the dhcpd service with the configured SIP server, mtu
  information.

@parameters
  void

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_LAN::UpdateDHCPDNetworkInfo()
{
  char dhcp_command[MAX_DHCP_COMMAND_STR_LEN];
  in_addr pcscf_addr, addr;
  int mtu_size_wwan_ipv4;
  char mtu_string[MTU_STRING_SIZE];
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  qcmap_sip_server_info_list_t *sip_server_info_list = NULL;
  qcmap_cm_sip_server_info_t *sip_entry = NULL;
  qcmap_cm_sip_server_info_t default_sip_entry;
  ds_dll_el_t * node = NULL;
  /*variable for storing network sip server type*/
  qcmap_cm_sip_server_type_e  network_sip_server_type= QCMAP_CM_SIP_SERVER_MIN;
  QCMAP_CM_LOG_FUNC_ENTRY();
  if (this->lan_cfg.enable_dhcpd)
  {
    memset(dhcp_command, 0, MAX_DHCP_COMMAND_STR_LEN);
    memset(&default_sip_entry, 0, sizeof(default_sip_entry));
    strlcpy(dhcp_command,this->dhcp_command_str, MAX_DHCP_COMMAND_STR_LEN);

    /* if either sta backhaul connected or cradle backhaul connected ignore mtu wwanipv4*/
    if ( !QCMAP_Backhaul::IsNonWWANBackhaulActive())
    {
      QCMAP_CM_LOG_FUNC_ENTRY();
      if( QCMAP_ConnectionManager::GetWWANMTUInfo(&mtu_size_wwan_ipv4) == true )
      {
        /* add mtu wwan ipv4 as dhcp option 26 starts */
        if(mtu_size_wwan_ipv4 > 0 )
        {
          strlcat(dhcp_command, " --dhcp-option-force=", MAX_DHCP_COMMAND_STR_LEN);
          strlcat(dhcp_command, DHCP_OPTION_MTU, MAX_DHCP_COMMAND_STR_LEN);
          strlcat(dhcp_command, ",", MAX_DHCP_COMMAND_STR_LEN);
          memset(mtu_string, 0, sizeof(mtu_string));
          snprintf(mtu_string,sizeof(mtu_string),"%d",(int) mtu_size_wwan_ipv4);
          strlcat(dhcp_command, mtu_string, MAX_DHCP_COMMAND_STR_LEN);
        }
        else
        {
          LOG_MSG_ERROR( "Couldn't set .mtu i.e not got it from wwan  %d\n",
                         mtu_size_wwan_ipv4,0,0 );
        }
        /* add mtu wwan ipv4 as dhcp option 26 ends */
      }
    }

    /*If default SIP Server configuration is not yet available get the
      configuraiton directly from XML*/
    if (QcMapBackhaulWWANMgr)
    {
      sip_server_info_list = &(QcMapBackhaulWWANMgr->wwan_cfg.sip_server_info_list);
      memcpy(&default_sip_entry,
             &(QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info),
             sizeof(default_sip_entry));
    }
    else
    {
      QCMAP_Backhaul_WWAN::GetSetDefaultSIPServerConfigFromXML(GET_VALUE,
                                                               &default_sip_entry);
    }

    if (default_sip_entry.sip_serv_type != QCMAP_CM_SIP_SERVER_MIN ||
        (QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->wwan_cfg.sip_server_list_count > 0))
    {
      strlcat(dhcp_command, " --dhcp-option-force=120", MAX_DHCP_COMMAND_STR_LEN);
    }

    if (QcMapBackhaulWWANMgr)
    {
      sip_server_info_list = &(QcMapBackhaulWWANMgr->wwan_cfg.sip_server_info_list);
      LOG_MSG_INFO1(" Sip server count: %d ",
                      QcMapBackhaulWWANMgr->wwan_cfg.sip_server_list_count, 0, 0);
      LOG_MSG_INFO1("%d network assigned PCSCF info",
                     QcMapBackhaulWWANMgr->wwan_cfg.sip_server_list_count, 0, 0);

      if (QcMapBackhaulWWANMgr->wwan_cfg.sip_server_list_count > 0)
      {

        node = sip_server_info_list->sipServerEntryListHead;
        node = ds_dll_next (node, (const void**)(&sip_entry));

        for (int i = 0; i <QcMapBackhaulWWANMgr->wwan_cfg.sip_server_list_count; i++)
        {
          strlcat(dhcp_command, ",", MAX_DHCP_COMMAND_STR_LEN);
          if (sip_entry->sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
          {
            network_sip_server_type = QCMAP_CM_SIP_SERVER_ADDR;
            LOG_MSG_INFO1("Adding network assigned PCSCF address", 0, 0, 0);
            pcscf_addr.s_addr = sip_entry->sip_serv_info.sip_serv_ip;
            /* INET_NTOA_SIZE is the size of static buff used in inet_ntoa function */
          strlcat(dhcp_command, inet_ntoa(pcscf_addr), MAX_DHCP_COMMAND_STR_LEN);
          }
          else if (sip_entry->sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN)
          {
            network_sip_server_type = QCMAP_CM_SIP_SERVER_FQDN;
            strlcat(dhcp_command, "\"", MAX_DHCP_COMMAND_STR_LEN);
            strlcat(dhcp_command,
                    sip_entry->sip_serv_info.sip_fqdn,
                    MAX_DHCP_COMMAND_STR_LEN);
            strlcat(dhcp_command, "\"", MAX_DHCP_COMMAND_STR_LEN);
          }
          node = ds_dll_next (node, (const void**)(&sip_entry));
        }
      }
    }

    /*adding default sip server info on basis of network sip server type*/
    if ((default_sip_entry.sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR) &&
        ((network_sip_server_type == QCMAP_CM_SIP_SERVER_ADDR) ||
        (network_sip_server_type == QCMAP_CM_SIP_SERVER_MIN )))
    {
      strlcat(dhcp_command, ",", MAX_DHCP_COMMAND_STR_LEN);
      pcscf_addr.s_addr = default_sip_entry.sip_serv_info.sip_serv_ip;
      strlcat(dhcp_command,inet_ntoa(pcscf_addr),MAX_DHCP_COMMAND_STR_LEN);
    }
    else if ((default_sip_entry.sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN) &&
              ((network_sip_server_type == QCMAP_CM_SIP_SERVER_FQDN) ||
             (network_sip_server_type == QCMAP_CM_SIP_SERVER_MIN)))
    {
      strlcat(dhcp_command, ",", MAX_DHCP_COMMAND_STR_LEN);
      strlcat(dhcp_command, "\"", MAX_DHCP_COMMAND_STR_LEN);
      strlcat(dhcp_command,
              default_sip_entry.sip_serv_info.sip_fqdn,
              MAX_DHCP_COMMAND_STR_LEN);
      strlcat(dhcp_command, "\"", MAX_DHCP_COMMAND_STR_LEN);
    }

    ds_system_call("chown nobody:nogroup /bin/dnsmasq_script.sh",
                   strlen("chown nobody:nogroup /bin/dnsmasq_script.sh"));

    strlcat (dhcp_command,"  --dhcp-script=/bin/dnsmasq_script.sh",
             MAX_DHCP_COMMAND_STR_LEN);
    ds_system_call("killall -9 dnsmasq", strlen("killall -9 dnsmasq"));
    ds_system_call(dhcp_command, strlen(dhcp_command));
  }
  return true;
}


/*===========================================================================
  FUNCTION EnableDNS
==========================================================================*/
/*!
@brief
  Adds the IP addresses of nameservers available, received from network.

@parameters
  void

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_LAN::EnableDNS()
{
  int qcmap_cm_error;
  int ret_val;
  int rc = 0;
  struct timespec ts;
  in_addr_t public_ip = 0,  sec_dns_addr = 0, default_gw_addr = 0;
  uint32 pri_dns_addr = 0;
  struct in_addr addr;
  char command[MAX_COMMAND_STR_LEN];
  qmi_error_type_v01 qmi_err_num;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  if ( qcmap_cm_get_ipv4_net_conf(QcMapMgr->qcmap_cm_handle, &public_ip, &pri_dns_addr,
       &sec_dns_addr, &default_gw_addr, &qmi_err_num) != QCMAP_CM_SUCCESS )
  {
    LOG_MSG_ERROR("Error in qcmap_cm_get_ipv4_net_conf %d\n", qmi_err_num,0,0);
  }

  if ( !pri_dns_addr && !sec_dns_addr )
  {
    LOG_MSG_INFO1("QCMAP DNS not enabled \n",0,0,0);
    return false;
  }

  if ( pri_dns_addr )
  {
    addr.s_addr = pri_dns_addr;
    #ifndef FEATURE_QTIMAP_OFFTARGET
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 'nameserver %s' > /etc/resolv.conf", inet_ntoa(addr));
      LOG_MSG_INFO1("QCMAP PRI DNS %s\n", inet_ntoa(addr),0,0);
      ds_system_call(command, strlen(command));
    #endif
  }

  if ( sec_dns_addr )
  {
    addr.s_addr = sec_dns_addr;
    #ifndef FEATURE_QTIMAP_OFFTARGET
      snprintf(command, MAX_COMMAND_STR_LEN,
               "echo 'nameserver %s' >> /etc/resolv.conf", inet_ntoa(addr));
      LOG_MSG_INFO1("QCMAP SEC DNS %s\n", inet_ntoa(addr),0,0);
      ds_system_call(command, strlen(command));
    #endif
  }

  LOG_MSG_INFO1("QCMAP DNS Enabled \n",0,0,0);
  return true;
}


/*===========================================================================
FUNCTION CheckforAddrConflict
==========================================================================*/
/*!
@brief
This function will check if ap, guest ap and sta are in different subnets.
To avoid address conflicts.

@return
true - on Success
false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_LAN::CheckforAddrConflict( qcmap_msgr_lan_config_v01 *lan_config,
qcmap_msgr_station_mode_config_v01 *station_config )
{
  qcmap_msgr_station_mode_config_v01 station_cfg;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);



/* Check for LAN Configuration for address conflicts */
if ( lan_config != NULL )
{
  /* Check with sta configuratin for address conflicts */
  if ( station_config !=NULL )
  {
    if ( ( station_config->conn_type == QCMAP_MSGR_STA_CONNECTION_STATIC_V01 ) &&
    ( ( lan_config->gw_ip & lan_config->netmask ) ==
    ( station_config->static_ip_config.gw_ip & station_config->static_ip_config.netmask )))
    {
      return true;
    }
  }
  /* Check with default sta configuratin for address conflicts */
  QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(GET_VALUE,&station_cfg);

  if ( station_cfg.conn_type == QCMAP_MSGR_STA_CONNECTION_STATIC_V01 )
  {
    if ( ( lan_config->gw_ip & lan_config->netmask ) ==
    ( station_cfg.static_ip_config.gw_ip
    & station_cfg.static_ip_config.netmask ))//read from xml check full api
    {
      LOG_MSG_INFO1("\n LAN configured address overlapped with STA IP \n",0,0,0);
      return true;
    }
  }
}

/* Check for sta Configuratin for address conflicts with AP and Guest AP mode*/
if ( station_config != NULL && (station_config->conn_type ==
     QCMAP_MSGR_STA_CONNECTION_STATIC_V01 ))
{
if ( ( station_config->static_ip_config.gw_ip
     & station_config->static_ip_config.netmask) ==
     ( this->lan_cfg.apps_ip_addr &
     this->lan_cfg.sub_net_mask ))
{
LOG_MSG_INFO1("\n STA configured address overlapped with LAN IP\n",0,0,0);
return true;
}
}
return false;
}


/*===========================================================================
  FUNCTION sync_dhcp_hosts
==========================================================================*/
/*!
@brief
  Sync /etc/dhcp-hosts with the current DHCP reservation recordds
  on boot or when SetLANConfig API is called

@return
  boolean

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void QCMAP_LAN::sync_dhcp_hosts(void)
{
  in_addr addr;

  qcmap_msgr_dhcp_reservation_v01 *dhcp_reserv_record = NULL;
  boolean mac_addr_non_empty = true;
  uint32_t client_ip;
  uint32 num_records = 0;
  qcmap_dhcp_information_info_list_t* dhcpList = &(this->lan_cfg.dhcp_reservation_records);
  ds_dll_el_t * node = NULL, *temp_node = NULL;
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];/*char array of mac addr*/

  /*-------------------------------------------------------------------------*/


  num_records = this->lan_cfg.num_dhcp_reservation_records;
  this->lan_cfg.num_dhcp_reservation_records = 0;
  node = dhcpList->dhcpInfoEntryListHead;

  node = ds_dll_next (node, (const void**)(&dhcp_reserv_record));

  //for ( i = 0; i < num_records && dhcp_reserv_record; i++ )
  while(node)
  {
    client_ip = dhcp_reserv_record->client_reserved_ip;
    addr.s_addr = htonl(client_ip);
    if ( ( this->lan_cfg.dhcp_start_address <= client_ip ) &&
         ( (this->lan_cfg.dhcp_end_address - 1)   >= client_ip ) )
    {
      this->lan_cfg.num_dhcp_reservation_records++;

      /*sync /etc/dhcp-hosts file*/
      if ( dhcp_reserv_record->enable_reservation )
      {
        if ( dhcp_reserv_record->client_device_name[0] != '\0' )
        {
          fprintf(dhcp_hosts_fp,"%s,",dhcp_reserv_record->client_device_name);
        }
        /* mac address is not provided for USB client*/
        mac_addr_non_empty = check_non_empty_mac_addr(dhcp_reserv_record->client_mac_addr,
                                                      mac_addr_string);
        if ( mac_addr_non_empty )
        {
          fprintf(dhcp_hosts_fp,"%s,",mac_addr_string);
        }
        fprintf(dhcp_hosts_fp,"%s\n",inet_ntoa(addr));
      }
      node = ds_dll_next (node, (const void**)(&dhcp_reserv_record));
    }
    else
    {
      temp_node = node->next;
      ds_dll_delete_node(dhcpList->dhcpInfoEntryListHead,
                         &(dhcpList->dhcpInfoEntryListTail), node);

      //Free the device information structure
      ds_free (dhcp_reserv_record);
      dhcp_reserv_record = NULL;

      //Free the memory of the linked list node
      ds_dll_free(node);

      node = temp_node;
      dhcp_reserv_record = ds_dll_data(node);
    }
  }
}

/*===========================================================================
  FUNCTION ReadLANConfigFromXML
==========================================================================*/
/*!
@brief
  Reads the XML config from XML

@return
  boolean

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/

boolean QCMAP_LAN::ReadLANConfigFromXML()
{

  pugi::xml_document xml_file;
  pugi::xml_node parent, root, child, subchild;
  qcmap_cm_intf_index_type ssid = QCMAP_MSGR_INTF_AP_INDEX;
  in_addr addr;
  uint32_t value_debug=0;
  char str[INET6_ADDRSTRLEN];
   qcmap_msgr_dhcp_reservation_v01 *dhcp_record = NULL;
  qcmap_msgr_dhcp_reservation_v01 dhcp_reserv_record;
  uint8 mac[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  char content_buf[MAX_STRING_LENGTH];
  qmi_error_type_v01 qmi_err_num;
  memset(&dhcp_reserv_record,0,sizeof(qcmap_msgr_dhcp_reservation_v01));

  if (!xml_file.load_file(QCMAP_ConnectionManager::xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  this->lan_cfg.enable_dhcpd = atoi(root.child(EnableDHCPServer_TAG).child_value());

  if (inet_aton(root.child(APIPAddr_TAG).child_value(), &addr))
    this->lan_cfg.apps_ip_addr = ntohl(addr.s_addr);

  if (inet_aton(root.child(SubNetMask_TAG).child_value(), &addr))
     this->lan_cfg.sub_net_mask = ntohl(addr.s_addr);

  strlcpy(this->lan_cfg.gateway_url, root.child(GatewayURL_TAG).child_value(),
          QCMAP_MSGR_MAX_GATEWAY_URL_V01);


  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG).
         child(DHCPCfg_TAG);

  if (inet_aton(root.child(DHCPStartIP_TAG).child_value(), &addr))
    this->lan_cfg.dhcp_start_address = ntohl(addr.s_addr);

  if (inet_aton(root.child(DHCPEndIP_TAG).child_value(), &addr))
    this->lan_cfg.dhcp_end_address = ntohl(addr.s_addr);

  this->lan_cfg.dhcp_lease_time = atoi(root.child(DHCPLeaseTime_TAG).child_value());

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);
  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strcmp(child.name(), DHCPReservationRecord_TAG))
    {
      /*memset to NULL*/
      memset(&dhcp_reserv_record,0,sizeof(qcmap_msgr_dhcp_reservation_v01));
      if(child.child(ClientMACAddr_TAG))
      {
        strlcpy(content_buf,child.child(ClientMACAddr_TAG).child_value(),MAX_STRING_LENGTH);
        for (int i = 0; i < QCMAP_MSGR_MAC_ADDR_LEN_V01; i++)
        {
          mac[i] = (ds_hex_to_dec(content_buf[i * 3]) << 4) | ds_hex_to_dec(content_buf[i * 3 + 1]);
        }
        LOG_MSG_INFO1("content_buf: %s",content_buf,0,0);
        strlcpy(dhcp_reserv_record.client_mac_addr,mac,QCMAP_MSGR_MAC_ADDR_LEN_V01+1);
        inet_aton( child.child(ClientReservedIP_TAG).child_value(),
                  (in_addr *)&(dhcp_reserv_record.client_reserved_ip) );
        dhcp_reserv_record.client_reserved_ip = ntohl(dhcp_reserv_record.client_reserved_ip);

        dhcp_reserv_record.enable_reservation =
                               atoi(child.child(Reservation_TAG).child_value());
      }


       else if(child.child(ClientName_TAG))
       {
         LOG_MSG_INFO1("here: ",0,0,0);
         strlcpy( dhcp_reserv_record.client_device_name,
                  child.child(ClientName_TAG).child_value(),
                  QCMAP_MSGR_DEVICE_NAME_MAX_V01+1 );

         inet_aton(child.child(ClientReservedIP_TAG).child_value(),
                  (in_addr *)&(dhcp_reserv_record.client_reserved_ip));

         dhcp_reserv_record.client_reserved_ip =
                                   ntohl(dhcp_reserv_record.client_reserved_ip);

         dhcp_reserv_record.enable_reservation =
                            atoi(child.child(Reservation_TAG).child_value());
       }

       if(this->AddDHCPResRecEntryToList( &dhcp_reserv_record, dhcp_record) == false)
      {
        LOG_MSG_ERROR("Error while reading config from XML", 0, 0, 0);
        return false;
      }
    }
  }

  value_debug= htonl(this->lan_cfg.dhcp_start_address);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP Start Address : '%s'",str,0,0);

  value_debug= htonl(this->lan_cfg.dhcp_end_address);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP End Address : '%s'",str,0,0);


  LOG_MSG_INFO1("\nDHCP Lease Time: '%d'",this->lan_cfg.dhcp_lease_time,0,0);

  value_debug= htonl(this->lan_cfg.apps_ip_addr);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP API Address : '%s'",str,0,0);

  value_debug= htonl(this->lan_cfg.sub_net_mask);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP Subnet Mask : '%s'",str,0,0);

  LOG_MSG_INFO1("Enable DHCP: %b , ",this->lan_cfg.enable_dhcpd,0,0)


  return true;
}
/*===========================================================================
  FUNCTION AddDHCPResRecEntryToList
==========================================================================*/
/*!
@brief
  Adds DHCP entry to List

@parameters
qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record,
  qcmap_msgr_dhcp_reservation_v01* dhcp_record

@return
  -1 - on failure
   0 - on success

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_LAN::AddDHCPResRecEntryToList
(
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record,
  qcmap_msgr_dhcp_reservation_v01* dhcp_record
)
{

  LOG_MSG_INFO1("Entering AddDHCPResRecEntryToList",0,0,0);
  ds_dll_el_t * node = NULL, *temp_node = NULL;

  qcmap_dhcp_information_info_list_t* dhcpList = &(this->lan_cfg.dhcp_reservation_records);

  if (dhcpList->dhcpInfoEntryListHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
          information. This is done to make use of the doubly linked list framework which
           is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("AddDHCPReservRecord - Error in allocating memory for node",
                    0,0,0);
      return false;
    }
    dhcpList->dhcpInfoEntryListHead = node;
  }

  dhcp_record = (qcmap_msgr_dhcp_reservation_v01*)ds_malloc(sizeof(qcmap_msgr_dhcp_reservation_v01));
  if( dhcp_record == NULL )
  {
    LOG_MSG_ERROR("AddDHCPReservRecord - Error in allocating memory for"
                 "dhcp reservation entry",0,0,0);
    return false;
  }


  memset(dhcp_record, 0, sizeof(qcmap_msgr_dhcp_reservation_v01));

  memcpy(dhcp_record, dhcp_reserv_record,
         sizeof(qcmap_msgr_dhcp_reservation_v01));

  //Store the firewall entry in the linked list
  if ((node = ds_dll_enq(dhcpList->dhcpInfoEntryListHead,
                         NULL, (void*)dhcp_record)) == NULL)
  {
    LOG_MSG_ERROR("AddDHCPReservRecord - Error in adding a node",0,0,0);
    ds_free(dhcp_record);
    return false;
  }
  dhcpList->dhcpInfoEntryListTail = node;
  this->lan_cfg.num_dhcp_reservation_records++;

  return true;

}

/*===========================================================================
  FUNCTION SetLANConfigToXML
==========================================================================*/
/*!
@brief
  Get /set LAN config from XML

@return
  boolean

@note

@Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
boolean QCMAP_LAN::SetLANConfigToXML(qcmap_msgr_lan_config_v01 *lan_config)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child ;
  struct in_addr addr;
  uint32_t value_debug=0;
  const char *tag_ptr;
  qcmap_cm_intf_index_type ssid = QCMAP_MSGR_INTF_STATION_INDEX;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  char data[MAX_STRING_LENGTH] = {0};
  char str[INET6_ADDRSTRLEN];
  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  snprintf(data, sizeof(data), "%d", lan_config->enable_dhcp);
  root.child(EnableDHCPServer_TAG).text() = data;

  addr.s_addr = htonl(lan_config->gw_ip);
  root.child(APIPAddr_TAG).text() = inet_ntoa(addr);

  addr.s_addr = htonl(lan_config->netmask);
  root.child(SubNetMask_TAG).text() = inet_ntoa(addr);

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).
         child(MobileAPLanCfg_TAG).child(DHCPCfg_TAG);

  addr.s_addr = htonl(lan_config->dhcp_config.dhcp_start_ip);
  root.child(DHCPStartIP_TAG).text() = inet_ntoa(addr);

  addr.s_addr = htonl(lan_config->dhcp_config.dhcp_end_ip);
  root.child(DHCPEndIP_TAG).text() = inet_ntoa(addr);

  snprintf(data, sizeof(data), "%d", lan_config->dhcp_config.lease_time);
  root.child(DHCPLeaseTime_TAG).text() = data;

  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  value_debug= htonl(lan_config->dhcp_config.dhcp_start_ip);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP Start Address : '%s'",str,0,0);

  value_debug= htonl(lan_config->dhcp_config.dhcp_end_ip);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP End Address : '%s'",str,0,0);

  LOG_MSG_INFO1("\nDHCP Lease Time: '%d'",lan_config->dhcp_config.lease_time,0,0);

  value_debug= htonl(lan_config->gw_ip);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP API Address : '%s'",str,0,0);

  value_debug= htonl(lan_config->netmask);
  readable_addr(AF_INET,(const uint32_t *)&value_debug,(char *)&str);
  LOG_MSG_INFO1("\nDHCP Subnet Mask : '%s'",str,0,0);

  LOG_MSG_INFO1("Enable DHCP: %d",lan_config->enable_dhcp,0,0)

  return TRUE;
}

/*===========================================================================
  FUNCTION SetGatewayConfigToXML
==========================================================================*/
/*!
@brief
  Sets the Gateway to the XML

@parameters
  char *url

@return
  true  - Get URL success
  flase - Get URL failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/


boolean QCMAP_LAN::SetGatewayConfigToXML(char *url)
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);


  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  root.child(GatewayURL_TAG).text() = url;

  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  return TRUE;
}


/*===========================================================================
  FUNCTION GetGatewayUrl
==========================================================================*/
/*!
@brief
  Retreives the Gateway url.

@parameters
  char *url

@return
  true  - Get URL success
  flase - Get URL failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_LAN::GetGatewayUrl
(
  char *url,
  uint32_t *url_len,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  if( url != NULL && url_len != NULL )
  {
    memcpy(url,QCMAPLANMgr->lan_cfg.gateway_url,QCMAP_MSGR_MAX_GATEWAY_URL_V01);
    *url_len = strlen(url);
  }
  else
  {
    LOG_MSG_ERROR("Invalid argument\n", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION EnableGatewayUrl
==========================================================================*/
/*!
@brief
  Enables the Gateway url.

@parameters
  None

@return
  true  - Get URL success
  flase - Get URL failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_LAN::EnableGatewayUrl()
{
  char command[MAX_COMMAND_STR_LEN];
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  struct in_addr addr;

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "echo 127.0.0.1 localhost.localdomain localhost > %s",QCMAP_HOST_PATH);
  ds_system_call(command, strlen(command));

  addr.s_addr = htonl(this->lan_cfg.apps_ip_addr);
  strlcpy(a5_ip, inet_ntoa(addr),QCMAP_LAN_MAX_IPV4_ADDR_SIZE);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "echo %s %s >> %s",a5_ip,this->lan_cfg.gateway_url,QCMAP_HOST_PATH);
  ds_system_call(command, strlen(command));

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "killall -s SIGHUP dnsmasq");
  ds_system_call(command, strlen(command));
}

/*===========================================================================
  FUNCTION SetGatewayUrl
==========================================================================*/
/*!
@brief
  Set the Gateway url.

@parameters
  char *url

@return
  true  - Set URL success
  flase - Set URL failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_LAN::SetGatewayUrl
(
  char *url,
  uint32_t url_len,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  if(url != NULL && url_len != 0)
  {
    bzero(QCMAPLANMgr->lan_cfg.gateway_url,QCMAP_MSGR_MAX_GATEWAY_URL_V01);
    memcpy(QCMAPLANMgr->lan_cfg.gateway_url,url,url_len);
  }
  else
  {
    LOG_MSG_ERROR("Invalid argument\n", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  QCMAPLANMgr->EnableGatewayUrl();
  QCMAPLANMgr->SetGatewayConfigToXML(url);
  return true;
}

/*===========================================================================
 FUNCTION AddEbtablesRuleForBridgeMode
==========================================================================*/
/*!
@brief
  - Match the MAC address to find the node
  - If a match is found, add the default Ebtables rules for various interfaces

@parameters
  Pointer to qcmap_cm_client_data_info_t
@return
  none
@note
  - Dependencies
  - None
  - Side Effects
  - None
*/
/*=========================================================================*/
bool QCMAP_LAN::AddEbtablesRuleForBridgeMode(qcmap_cm_client_data_info_t* data)
{
  in_addr addr;
  char usb_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  LOG_MSG_INFO1("QCMAP_ConnectionManager::AddEbtablesRuleForBridgeMode()",0,0,0);

  if ( data == NULL)
  {
    LOG_MSG_ERROR("AddEbtablesRuleForBridgeMode - NULL pointer passed \n",0,0,0);
    return false;
  }

  addr.s_addr = data->ip_addr;
  ds_log_med(" AddEbtablesRuleForBridgeMode for IP address %s \n",inet_ntoa(addr));
  /* If the Update for IP address is for Primary AP or USB TE*
   * Update Ebtables WLAN-STA side for ARP-REPLY target for this entry
   * Update Ebtables WLAN-AP, USB side for ARP-ACCEPT target for local LAN*/
  if ((data->device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
      (data->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01) ||
      (data->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01))
  {
    /* WLAN-STA Side Entry */
    if(NULL != QcMapBackhaulWLANMgr)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,
                "ebtables -t nat -A PREROUTING -i %s -p arp --arp-opcode Request\
                --arp-ip-dst %s -j arpreply \
                --arpreply-mac %02x:%02x:%02x:%02x:%02x:%02x\
                --arpreply-target ACCEPT",
                 QcMapBackhaulWLANMgr->apsta_cfg.sta_interface, inet_ntoa(addr),
                 QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[0],
                 QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[1],
                 QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[2],
                 QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[3],
                 QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[4],
                 QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[5]);
      ds_system_call(command, strlen(command));
    }
    /* AP Side Entry */
    snprintf(command, MAX_COMMAND_STR_LEN,
              "ebtables -t nat -I PREROUTING -i wlan%d -p arp --arp-opcode\
              Request --arp-ip-dst %s -j ACCEPT",
               QcMapMgr->ap_dev_num1, inet_ntoa(addr));
    ds_system_call(command, strlen(command));

    /* USB Side Entry */
    bzero(usb_intf_name,sizeof(usb_intf_name));
    if ( QcMapTetheringMgr && QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1))
    {
      if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS )
      {
        strlcpy(usb_intf_name, "rndis0", sizeof(usb_intf_name));
      }
      else if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM )
      {
        strlcpy(usb_intf_name, "ecm0", sizeof(usb_intf_name));
      }
      else
      {
        LOG_MSG_ERROR("Incorrect USB LINK Detected Link Type = %x",
                       QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,0,0);
        return false;
      }
      snprintf(command, MAX_COMMAND_STR_LEN,
                "ebtables -t nat -I PREROUTING -i %s -p arp --arp-opcode Request\
                --arp-ip-dst %s -j ACCEPT",
                 usb_intf_name, inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    }
  }
  return true;
}


/*===========================================================================
 FUNCTION DelEbtablesRuleForBridgeMode
==========================================================================*/
/*!
@brief
  - Match the MAC address to find the node
  - If a match is found, delete the Ebtables rules for various interfaces

@parameters
  Pointer to qcmap_cm_client_data_info_t
@return
  none
@note
 - Dependencies
 - None
 - Side Effects
 - None
*/
/*=========================================================================*/
bool QCMAP_LAN::DelEbtablesRuleForBridgeMode(qcmap_cm_client_data_info_t* data)
{
  in_addr addr;
  char usb_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  LOG_MSG_INFO1("QCMAP_ConnectionManager::DelEbtablesRulesForBridgeSingle()",0,0,0);
  if ( data == NULL)
  {
    LOG_MSG_ERROR("DelEbtablesRuleForBridgeMode - NULL pointer passed \n",0,0,0);
    return false;
  }

  addr.s_addr = data->ip_addr;
  /* If the Update for IP address is for Primary AP or USB TE*
   * Update Ebtables WLAN-STA side for ARP-REPLY target for this entry
   * Update Ebtables WLAN-AP, USB side for ARP-ACCEPT target for local LAN
   * Also Delete the ARP-Entry Manually, instead of wating for it to time.
   * This ensures imediate NEWNEIGH Event on re-connection*/
 if ((data->device_type == QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01) ||
     (data->device_type == QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01) ||
     (data->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01))
 {
   /* WLAN-STA Side Entry */
   if(NULL != QcMapBackhaulWLANMgr)
   {
     snprintf(command, MAX_COMMAND_STR_LEN,
               "ebtables -t nat -D PREROUTING -i %s -p arp --arp-opcode Request\
               --arp-ip-dst %s -j arpreply --arpreply-mac\
               %02x:%02x:%02x:%02x:%02x:%02x --arpreply-target ACCEPT",
                QcMapBackhaulWLANMgr->apsta_cfg.sta_interface, inet_ntoa(addr),
                QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[0],
                QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[1],
                QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[2],
                QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[3],
                QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[4],
                QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[5]);
     ds_system_call(command, strlen(command));
   }
   /* AP Side Entry */
   snprintf(command, MAX_COMMAND_STR_LEN,
            "ebtables -t nat -D PREROUTING -i wlan%d -p arp --arp-opcode Request\
            --arp-ip-dst %s -j ACCEPT",
             QcMapMgr->ap_dev_num1, inet_ntoa(addr));
   ds_system_call(command, strlen(command));
   /* USB Side Entry */
   bzero(usb_intf_name, sizeof(usb_intf_name));
   if ( QcMapTetheringMgr && QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1))
   {
     if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS )
     {
       strlcpy(usb_intf_name, "rndis0", sizeof(usb_intf_name));
     }
     else if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM )
     {
       strlcpy(usb_intf_name, "ecm0", sizeof(usb_intf_name));
     }
     else
     {
       LOG_MSG_ERROR("Incorrect USB LINK Detected Link Type = %x",
                      QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,0,0);
       return false;
     }
     snprintf(command, MAX_COMMAND_STR_LEN,
              "ebtables -t nat -D PREROUTING -i %s -p arp --arp-opcode Request\
              --arp-ip-dst %s -j ACCEPT",
              usb_intf_name, inet_ntoa(addr));
     ds_system_call(command, strlen(command));
   }

   /* Delete ARP Entry */
   snprintf(command, MAX_COMMAND_STR_LEN,"arp -d %s",inet_ntoa(addr));
   ds_system_call(command, strlen(command));
 }
  return true;
}


/*===========================================================================
 FUNCTION DelEbtablesUSBRulesForBridgeMode
==========================================================================*/
/*!
@brief
 Del Etables rules for LAN clients in AP-STA Bridge Mode

@parameters

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
bool QCMAP_LAN::DelEbtablesUSBRulesForBridgeMode(void)
{
  char netmask [QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char usb_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char command[MAX_COMMAND_STR_LEN];
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t* list_data = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  in_addr addr;
  qcmap_msgr_device_type_enum_v01 device_type = QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);


  LOG_MSG_INFO1("QCMAP_Tethering::DelEbtablesUSBRulesForBridgeMode",0,0,0);
  bzero(usb_intf_name,sizeof(usb_intf_name));
  if (QcMapTetheringMgr && QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS )
  {
    strlcpy(usb_intf_name, "rndis0",sizeof(usb_intf_name));
  }
  else if (QcMapTetheringMgr && QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM )
  {
    strlcpy(usb_intf_name, "ecm0",sizeof(usb_intf_name));
  }
  else if (QcMapTetheringMgr)
  {
    LOG_MSG_ERROR("Incorrect USB LINK Detected: %d",
                  QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,
                  0,0);
    return false;
  }
  else
  {
    LOG_MSG_ERROR("Incorrect USB LINK Detected: & QcMapTetheringMgr"
                 "is NULL",0,0,0);
    return false;
  }
  /* Add the default Rule for LAN Side Reply */
  if(NULL != QcMapBackhaulWLANMgr)
  {
    strlcpy(netmask, inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_netmask), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    snprintf(command, MAX_COMMAND_STR_LEN,
                       "ebtables -t nat -D PREROUTING -i %s -p arp --arp-ip-src\
                       =%s/%s --arp-opcode Request -j arpreply \
                       --arpreply-mac %02x:%02x:%02x:%02x:%02x:%02x \
                       --arpreply-target ACCEPT",
                       usb_intf_name,
                       inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_def_gw),
                       netmask, QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[0],
                       QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[1],
                       QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[2],
                       QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[3],
                       QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[4],
                       QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[5]);
    ds_system_call(command, strlen(command));
  }
  if ( addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Empty List: No rules to be deleted, per WLAN Iface\n",0,0,0);
    return true;
  }

  /* Del Rules for WLAN Clients connected to Secondary AP */
  device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
  node = ds_dll_search (addrList->addrListHead , (void*)&device_type,
                        qcmap_match_device_type);
  while(node)
  {
    list_data = (qcmap_cm_client_data_info_t*)node->data;
    if (list_data != NULL)
    {
      addr.s_addr = list_data->ip_addr;
      ds_log_med(" DelEbtablesUSBRulesForBridgeMode() for IP address %s \n",inet_ntoa(addr));
      snprintf(command, MAX_COMMAND_STR_LEN,
                  "ebtables -t nat -D PREROUTING -i %s -p arp --arp-opcode\
                  Request --arp-ip-dst %s -j ACCEPT",
                   usb_intf_name, inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR(" The device information data  node is NULL\n",0,0,0);
    }
    node = ds_dll_search (node, (void*)&device_type, qcmap_match_device_type);
  }

  /* Del Rules for WLAN Clients connected to Primary AP */
  device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
  node = ds_dll_search (addrList->addrListHead , (void*)&device_type,
                        qcmap_match_device_type);
  while(node)
  {
    list_data = (qcmap_cm_client_data_info_t*)node->data;
    if (list_data != NULL)
    {
      addr.s_addr = list_data->ip_addr;
      ds_log_med(" DelEbtablesUSBRulesForBridgeMode() for IP address %s \n",
                   inet_ntoa(addr));
      snprintf(command, MAX_COMMAND_STR_LEN,
                  "ebtables -t nat -D PREROUTING -i %s -p arp --arp-opcode\
                  Request --arp-ip-dst %s -j ACCEPT",
                   usb_intf_name, inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR(" The device information node data is NULL\n",0,0,0);
    }
    node = ds_dll_search (node , (void*)&device_type, qcmap_match_device_type);
  }
  return true;
}


/*===========================================================================
  FUNCTION AddEbtablesUSBRulesForBridgeMode
==========================================================================*/
/*!
@brief
  Add Etables rules for LAN clients in AP-STA Bridge Mode

@parameters

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
bool QCMAP_LAN::AddEbtablesUSBRulesForBridgeMode(void)
{
  char netmask [QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char teth_intf_name [DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char command[MAX_COMMAND_STR_LEN];
  ds_dll_el_t * node = NULL;
  qcmap_cm_client_data_info_t* list_data = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  in_addr addr;
  qcmap_msgr_device_type_enum_v01 device_type = QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  LOG_MSG_INFO1("QCMAP_Tethering::AddEbtablesUSBRulesForBridgeMode",0,0,0);
  snprintf(command,MAX_COMMAND_STR_LEN,"AddEbtablesUSBRulesForBridgeMode() > /dev/kmsg");
  ds_system_call(command,strlen(command));
  bzero(teth_intf_name,sizeof(teth_intf_name));
  if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_RNDIS )
  {
    strlcpy(teth_intf_name, "rndis0",sizeof(teth_intf_name));
  }
  else if ( QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type == QCMAP_QTI_TETHERED_LINK_ECM )
  {
    strlcpy(teth_intf_name, "ecm0",sizeof(teth_intf_name));
  }
  else
  {
    LOG_MSG_ERROR("Incorrect USB LINK Detected Link Type = %x", QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type,0,0);
    return false;
  }
  if(NULL != QcMapBackhaulWLANMgr)
  {
    strlcpy(netmask, inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_netmask), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
    //here no null check for APSTA since bridge mode check by caller ensures object is created
    snprintf(command, MAX_COMMAND_STR_LEN,
                       "ebtables -t nat -A PREROUTING -i %s -p arp \
                       --arp-ip-src=%s/%s --arp-opcode Request -j arpreply\
                       --arpreply-mac %02x:%02x:%02x:%02x:%02x:%02x\
                       --arpreply-target ACCEPT",
                       teth_intf_name,
                       inet_ntoa(QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_def_gw),
                       netmask,QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[0],
                       QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[1],
                     QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[2],
                     QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[3],
                     QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[4],
                     QcMapBackhaulWLANMgr->ap_sta_bridge.bridge_mac[5]);
    ds_system_call(command, strlen(command));
  }
  /* Iterate through the list and get check for any WLAN-AP based entries */
  if ( addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("\nMatchMacAddrList() - Linked list head is NULL \n",0,0,0);
    return false;
  }

  if ( addrList->addrListHead == NULL)
  {
    LOG_MSG_ERROR("Empty List: No rules to be entered, per WLAN Iface\n",0,0,0);
    return true;
  }

  /* Add Rules for WLAN Clients connected to Secondary AP */
  device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
  node = ds_dll_search (addrList->addrListHead , (void*)&device_type,
                        qcmap_match_device_type);
  while(node)
  {
    list_data = (qcmap_cm_client_data_info_t*)node->data;
    if (list_data != NULL)
    {
      addr.s_addr = list_data->ip_addr;
      ds_log_med(" AddEbtablesUSBRulesForBridgeMode() for IP address %s \n",
                   inet_ntoa(addr));
      snprintf(command, MAX_COMMAND_STR_LEN,
                  "ebtables -t nat -I PREROUTING -i %s -p arp --arp-opcode\
                  Request --arp-ip-dst %s -j ACCEPT",
                   teth_intf_name, inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR(" The device information node data is NULL\n",0,0,0);
    }
    node = ds_dll_search (node , (void*)&device_type, qcmap_match_device_type);
  }

  /* Add Rules for WLAN Clients connected to Primary AP */
  device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
  node = ds_dll_search (addrList->addrListHead , (void*)&device_type,
                        qcmap_match_device_type);
  while(node)
  {
    list_data = (qcmap_cm_client_data_info_t*)node->data;
    if (list_data != NULL)
    {
      addr.s_addr = list_data->ip_addr;
      ds_log_med(" AddEbtablesUSBRulesForBridgeMode() for IP address %s \n",
                 inet_ntoa(addr));
      snprintf(command, MAX_COMMAND_STR_LEN,
                  "ebtables -t nat -I PREROUTING -i %s -p arp --arp-opcode\
                   Request --arp-ip-dst %s -j ACCEPT",
                   teth_intf_name, inet_ntoa(addr));
      ds_system_call(command, strlen(command));
    }
    else
    {
      LOG_MSG_ERROR(" The device information node is NULL\n",0,0,0);
    }
    node = ds_dll_search (node , (void*)&device_type, qcmap_match_device_type);
  }
  return true;
}

/*==========================================================
 FUNCTION    qcmap_match_device_type
===========================================================*/
/*!
@brief
  Comparator function for match ConnectedDevices device type

@parameters
  Two void pointers with the device type to be compared

@return
  0 - on a match
  1 - otherwise

@note

@ Dependencies
   - None

@ Side Effects
   - None
*/
/*==========================================================*/
static long int QCMAP_LAN::qcmap_match_device_type
(
  const void *first,
  const void *second
)
{
  if( (first == NULL) || (second == NULL))
  {
    return 1;
  }
  qcmap_msgr_device_type_enum_v01* device_type = (qcmap_msgr_device_type_enum_v01*)first;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;
  connectedDevicesList = ( qcmap_cm_client_data_info_t*)second;

  return((*device_type == connectedDevicesList->device_type) ? false : true );
}


/*==========================================================
  FUNCTION AddDHCPResRecToXML
==========================================================*/
/*!
@brief
  Add DHCP Reservation Record to the XML.

@parameters
  pugi::xml_document *xml_file,
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record
@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

int QCMAP_LAN::AddDHCPResRecToXML(qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record)
{

  struct in_addr addr;
  struct in6_addr ip6_addr;
  char data[MAX_STRING_LENGTH] = {0};
  char str[INET6_ADDRSTRLEN];
  uint32_t value_debug=0;
  char mac_addr_str[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  uint8 *mac = NULL;
  pugi::xml_document xml_file;
  pugi::xml_node root, child, subchild;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);


  // add new DHCP Reservation Record here.
  child = root.append_child(DHCPReservationRecord_TAG);
  xml_file.print(std::cout);
  if(dhcp_reserv_record->client_device_name[0]=='\0')//AP client
  {
  //MAC Address
  mac=dhcp_reserv_record->client_mac_addr;
  snprintf(mac_addr_str,QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01,"%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  subchild = child.append_child(ClientMACAddr_TAG);
  subchild.append_child(pugi::node_pcdata).set_value(mac_addr_str);
  }
  //IP to reserve
  subchild = child.append_child(ClientReservedIP_TAG);
  addr.s_addr = htonl(dhcp_reserv_record->client_reserved_ip);
  subchild.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));

  if(dhcp_reserv_record->client_device_name[0]!='\0')//USB client
  {
    subchild = child.append_child(ClientName_TAG);
    subchild.append_child(pugi::node_pcdata).set_value(dhcp_reserv_record->client_device_name);
  }

  subchild = child.append_child(Reservation_TAG);
  snprintf(data, sizeof(data), "%d", dhcp_reserv_record->enable_reservation);
  subchild.append_child(pugi::node_pcdata).set_value(data);

  xml_file.print(std::cout);

  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

}

/*==========================================================
  FUNCTION EditDHCPResRecToXML
==========================================================*/
/*!
@brief
  Edit DHCP Reservation Record to the XML.

@parameters
  uint32_t client_ip,
  qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record
@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

int QCMAP_LAN::EditDHCPResRecToXML(uint32_t client_ip,
                                            qcmap_msgr_dhcp_reservation_v01* dhcp_reserv_record)
{
    char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];/*char array of mac addr*/
    char data[MAX_STRING_LENGTH] = {0};
    in_addr addr;
    pugi::xml_document xml_file;
    pugi::xml_node root, child;
    char *main_c,*main_t;
    QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
    addr.s_addr = htonl(client_ip);
    if (!xml_file.load_file(QcMapMgr->xml_path))
    {
      LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
      return false;
    }
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);
    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if (!strcmp(child.name(), DHCPReservationRecord_TAG))
      {
        main_t=child.child(ClientReservedIP_TAG).child_value();  /*xml stored  ip address*/
        main_c=inet_ntoa(addr);  /* client address to delete*/
        if (strcmp (main_t,main_c)==0) /*if mach found*/
        {
          LOG_MSG_ERROR("Editing client with ip=%s to xml", main_c, 0, 0);
          if ( dhcp_reserv_record->client_device_name[0] != '\0')
          {
            child.child(ClientName_TAG).text()=dhcp_reserv_record->client_device_name;
          }
          /* mac address is not provided for USB client*/
          if ( check_non_empty_mac_addr(dhcp_reserv_record->client_mac_addr, mac_addr_string))
          {
            LOG_MSG_ERROR("Editing mac=%s to xml", mac_addr_string, 0, 0);
            child.child(ClientMACAddr_TAG).text()=mac_addr_string;
          }
          addr.s_addr = htonl(dhcp_reserv_record->client_reserved_ip);
          child.child(ClientReservedIP_TAG).text()=inet_ntoa(addr);
          snprintf(data, sizeof(data), "%d", dhcp_reserv_record->enable_reservation);
          child.child(Reservation_TAG).text()=data;

          QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
          break;
        }
      }
    }

}


/*==========================================================
  FUNCTION DeleteDHCPResRecFromXML
==========================================================*/
/*!
@brief
  Delete DHCP Reservation Record from  the XML.

@parameters
  uint32_t client_ip

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/


int QCMAP_LAN::DeleteDHCPResRecFromXML(uint32_t client_ip)
{
    in_addr addr;
    pugi::xml_document xml_file;
    pugi::xml_node root, child;
    char *main_c,*main_t;
    QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
    addr.s_addr = htonl(client_ip);
    if (!xml_file.load_file(QcMapMgr->xml_path))
    {
      LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
      return false;
    }
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);
    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if (!strcmp(child.name(), DHCPReservationRecord_TAG))
      {
        main_t=child.child(ClientReservedIP_TAG).child_value();  /*xml stored  ip address*/
        main_c=inet_ntoa(addr);  /* client address to delete*/
        if (strcmp (main_t,main_c)==0) /*if mach found*/
        {
          LOG_MSG_ERROR("deleting client with ip=%s from xml", main_c, 0, 0);
          root.remove_child(child);
          QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
          break;
        }

      }
    }
 }



/*===========================================================================
  FUNCTION InitBridge
==========================================================================*/
/*!
@brief
Initialize bridge interface.
@return
None
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_LAN::InitBridge()
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  char apps_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  qmi_error_type_v01 qmi_err_num;
  int retry_count = 0;
  int ret;
  struct ether_addr bridge_mac_addr;
  uint8 i = 0;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  qcmap_msgr_cradle_mode_v01 cradle_mode=QCMAP_MSGR_CRADLE_LAN_ROUTER_V01;
  /* Create the Bridge Interface. */
  snprintf(command, MAX_COMMAND_STR_LEN, "brctl addbr %s", BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  /* Bring up and configure the bridge interface. */
  /* Configure Static IP*/
  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false &&
      QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false)
  {
    addr.s_addr = htonl(this->lan_cfg.apps_ip_addr);
    strlcpy(apps_ip, inet_ntoa(addr), 16);
    addr.s_addr = htonl(this->lan_cfg.sub_net_mask);
    strlcpy(netmask, inet_ntoa(addr), 16);
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s %s netmask %s up",
             BRIDGE_IFACE, apps_ip, netmask);
  }
  else /*Get IP via DHCP*/
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s up", BRIDGE_IFACE);
  }
  ds_system_call(command, strlen(command));

  if(QCMAP_CM_ERROR == QcMapMgr->GetHWAddr((char*) &bridge_local_mac_addr.ether_addr_octet,BRIDGE_IFACE))
  {
    LOG_MSG_INFO1("Error in fetching Bridge MAC ",0,0,0);
  }
  else
  {
    for ( i = 0 ; i < 3 ; i++)
    {
      bridge_local_mac_addr.ether_addr_octet[i+3]+=RANDOM_MAC_KEY;
    }
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig bridge0 hw ether %s",
               ether_ntoa(&bridge_local_mac_addr));
    ds_system_call(command, strlen(command));
  }

  this->EnableGatewayUrl();
  //Enable the bridge forwarding for Ipv6
  snprintf(command, MAX_COMMAND_STR_LEN,
           "echo 1 > /proc/sys/net/ipv6/conf/%s/forwarding",BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  //Set the Garbage Collector threshold to 1
  snprintf(command, MAX_COMMAND_STR_LEN,
           "echo 1 > /proc/sys/net/ipv4/neigh/default/gc_thresh1");
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "echo 1 > /proc/sys/net/ipv6/neigh/default/gc_thresh1");
  ds_system_call(command, strlen(command));

  /*Check for rndis interface is enabled, if enabled bind it to bridge, so that dhcp request
    message are honoured earliest*/
  ret = QcMapMgr->IsInterfaceEnabled("rndis0");
  if( ret > 0 )
  {
    LOG_MSG_INFO1("rndis is enabled ",0,0,0);
    snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s rndis0",
             BRIDGE_IFACE);
    ds_system_call(command, strlen(command));
  }
  else if ( ret == 0)
  {
    LOG_MSG_INFO1("rndis is disabled",0,0,0);
  }
  else
  {
    LOG_MSG_ERROR("Unable to read rndis interface status",0,0,0);
  }

  /*Check for ecm interface is enabled, if enabled bind it to bridge, so that dhcp request
    message are honoured earliest*/
  ret = QcMapMgr->IsInterfaceEnabled("ecm0");

    if( ret > 0)
    {
      QCMAP_Backhaul_Cradle::GetCradleMode(&cradle_mode,&qmi_err_num);

      if(cradle_mode == QCMAP_MSGR_CRADLE_LAN_ROUTER_V01 ||
         cradle_mode == QCMAP_MSGR_CRADLE_LAN_BRIDGE_V01 ||
         cradle_mode == QCMAP_MSGR_CRADLE_DISABLED_V01)
      {
        snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s ecm0",
                 BRIDGE_IFACE);
        ds_system_call(command, strlen(command));
      }
    }

    else if ( ret == 0)
    {
      LOG_MSG_INFO1("ecm is disabled",0,0,0);
    }
    else
    {
      LOG_MSG_ERROR("Unable to read ecm interface status",0,0,0);

  }

  /* Enable Proxy ARP. */
  snprintf( command, MAX_COMMAND_STR_LEN,
            "echo 1 > /proc/sys/net/ipv4/conf/%s/proxy_arp", BRIDGE_IFACE );
  ds_system_call( command, strlen(command));

  /* Enable v4 Neigh Probe. */
  snprintf( command, MAX_COMMAND_STR_LEN,
            "echo 1 > /proc/sys/net/ipv4/neigh/default/neigh_probe");
  ds_system_call( command, strlen(command));

  /* Enable v6 Neigh Probe. */
  snprintf( command, MAX_COMMAND_STR_LEN,
            "echo 1 > /proc/sys/net/ipv6/neigh/default/neigh_probe");
  ds_system_call( command, strlen(command));

  /* Restart DHCPD */
  this->StopDHCPD();
  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false)
  {
    this->StartDHCPD();
  }

  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false)
  {
    ds_system_call("echo QCMAP:DHCP server started > /dev/kmsg",
            strlen("echo QCMAP:DHCP server started > /dev/kmsg"));
  }

  /* Bring up rndis and ecm interfaces up on bootup,so that it improves ip
        address assignment time during bootup */
  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig rndis0 up");
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig ecm0 up");
  ds_system_call(command, strlen(command));

  if ( (qcmap_cm_get_ipv6_state(&qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED) &&
       (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated() == false) )
  {
    /* Just Restart RADISH. */
    QcMapBackhaulMgr->StopRadish();
    QcMapBackhaulMgr->StartRadish();
  }

  this->bridge_inited = TRUE;

  // Update Prev LAN Config with the currrent config with which the bridge is created
  this->prev_lan_config.gw_ip = this->lan_cfg.apps_ip_addr;
  this->prev_lan_config.netmask =this->lan_cfg.sub_net_mask;
  this->prev_lan_config.enable_dhcp =this->lan_cfg.enable_dhcpd;
  this->prev_lan_config.dhcp_config.dhcp_start_ip =this->lan_cfg.dhcp_start_address;
  this->prev_lan_config.dhcp_config.dhcp_end_ip = this->lan_cfg.dhcp_end_address;
  this->prev_lan_config.dhcp_config.lease_time = this->lan_cfg.dhcp_lease_time;

  /* Create a socket over the bridge interface, to send RA's for prefix deperecation. */
  while(retry_count < QCMAP_BRIDGE_MAX_RETRY )
  {
    this->bridge_sock = this->CreateRawSocket(BRIDGE_IFACE);
    if ( this->bridge_sock != QCMAP_CM_ERROR )
    {
      break;
    }
    LOG_MSG_ERROR("Error: Bridge Sock cannot be created: %d. retry=%d \n",
                  errno, retry_count, 0);
    usleep(QCMAP_BRIDGE_MAX_TIMEOUT_MS);
    retry_count++;
  }

  if( retry_count == QCMAP_BRIDGE_MAX_RETRY )
  {
    LOG_MSG_ERROR("Reached maximum retry attempts for CreateRawSocket %d",
                  retry_count, 0, 0);
  }

  snprintf(command, MAX_COMMAND_STR_LEN,
           "echo 1 > /sys/devices/virtual/net/%s/bridge/multicast_querier",
           BRIDGE_IFACE);
  ds_system_call(command, strlen(command));
}

/*===========================================================================
  FUNCTION DelBridge
==========================================================================*/
/*!
@brief
Deletes the bridge interface.
@return
None
@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
void QCMAP_LAN::DelBridge()
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  /* Flush the BROUTING chain. */
  snprintf(command, MAX_COMMAND_STR_LEN,
           "ebtables -t broute -D BROUTING -F");
  ds_system_call(command, strlen(command));

  /* Flush the Filter chain. */
  snprintf(command, MAX_COMMAND_STR_LEN,
           "ebtables -F");
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "echo 0 > /sys/devices/virtual/net/%s/bridge/multicast_querier",BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  /* Stop DHCP */
  this->StopDHCPD();
 /* Bring down the bridge interface. */
  snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s down", BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  /* Deletes the Bridge Interface. */
  snprintf(command, MAX_COMMAND_STR_LEN, "brctl delbr %s", BRIDGE_IFACE);
  ds_system_call(command, strlen(command));

  this->bridge_inited = false;

  if ( this->bridge_sock >=0 )
  {
    close(this->bridge_sock);
  }
}


/*===========================================================================
  FUNCTION CreateRawSocket
==========================================================================*/
/*!
@brief
Creates the socket

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

int QCMAP_LAN::CreateRawSocket(char *dev_name)
{
  int sock;
  /* According to RFC Hop Limit must be set to 255 in order
    to protect against off-link packets.
   */
  int hop_limit = 255;

  /* Create socket and set required options */
  sock = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
  if (sock < 0)
  {
    LOG_MSG_ERROR("%s: Unable to create socket %d", __func__, errno, 0);
    goto error;
  }

  if (setsockopt(sock,
                 IPPROTO_IPV6,
                 IPV6_MULTICAST_HOPS,
                 (char *)&hop_limit,
                 sizeof(hop_limit)) < 0)
  {
    LOG_MSG_ERROR("%s: Set sockopt failed %d", __func__, errno, 0);
    goto error;
  }

  if (setsockopt(sock,
                 IPPROTO_IPV6,
                 IPV6_UNICAST_HOPS,
                 (char *)&hop_limit,
                 sizeof(hop_limit)) < 0)
  {
    LOG_MSG_ERROR("%s: Set sockopt failed %d", __func__, errno, 0);
    goto error;
  }

  if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, dev_name, strlen(dev_name)+1) < 0)
  {
    LOG_MSG_ERROR("%s: Unable to bind socket to interface %s", __func__, dev_name, 0);
    goto error;
  }

  return sock;

error:
  if (sock >=0)
  {
    close(sock);
  }
  return -1;
}


/*===========================================================================
  FUNCTION EnableIPPassthrough
==========================================================================*/
/*!
@brief
  Enables IP Passthrough.

@parameters
  void

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_LAN::EnableIPPassthrough
(
  char *mac,
  char *host,
  qcmap_msgr_device_type_enum_v01 dev_type
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  qmi_error_type_v01 qmi_err_num;
  int rc = 0;
  int s, ret;
  struct ifreq buffer;
  struct sockaddr *sa;
  struct timespec ts;
  in_addr_t public_ip = 0,  sec_dns_addr = 0, default_gw_addr = 0;
  uint32 *gw_netmask;
  uint32 pri_dns_addr = 0;
  struct in_addr addr,gw_addr;
  char command[MAX_COMMAND_STR_LEN];
  char temp_buf[MAX_COMMAND_STR_LEN];
  char data[MAX_COMMAND_STR_LEN];
  FILE *in=NULL;
  in_addr netip,netmask,netw;
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_LAN *QcMapLANMgr = QCMAP_LAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  memset(&buffer, 0x00, sizeof(buffer));
  QcMapLANMgr->lan_cfg.ip_passthrough_cfg.device_type= dev_type;
  memset(QcMapLANMgr->lan_cfg.ip_passthrough_cfg.rmnet_def_route , 0, MAX_COMMAND_STR_LEN);

  if (QcMapLANMgr->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
  {
    LOG_MSG_INFO1("\nIP Passthrough is Already Running\n",0,0,0);
    return false;
  }

  LOG_MSG_INFO1("\nDevice type %d",dev_type,0,0);

  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
  {
    if(!( QcMapTetheringMgr &&
           QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) == true))
    {
      LOG_MSG_ERROR("\nUSB Link Not Enabled",0,0,0);
      return false;
    }

  }

  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
  {
    if(!( QcMapTetheringMgr &&
           QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) == true))
   {
      LOG_MSG_ERROR("\nEthernet Link Not Enabled",0,0,0);
      return false;
    }
  }

  //Check if backhaul is connected
  if (qcmap_cm_get_state(&qmi_err_num) != QCMAP_CM_WAN_CONNECTED )
  {
    LOG_MSG_INFO1("WWAN is not Connected.",0,0,0);
    return false;
  }

  //Check if current backhaul is WWAN
  if (QcMapBackhaulMgr->current_backhaul != BACKHAUL_TYPE_WWAN )
  {
    LOG_MSG_INFO1("Current backhaul mode is not WWAN.",0,0,0);
    return false;
  }

  if ( qcmap_cm_get_ipv4_net_conf(QcMapMgr->qcmap_cm_handle, &public_ip, &pri_dns_addr,
       &sec_dns_addr, &default_gw_addr, &qmi_err_num) != QCMAP_CM_SUCCESS )
  {
    LOG_MSG_INFO1("Error in qcmap_cm_get_ipv4_net_conf %d\n", qmi_err_num,0,0);
    return false;
  }

  //getting network netmask
  if ( !QcMapBackhaulMgr->GetIP(&netip.s_addr, &netmask.s_addr,
      QcMapBackhaulMgr->wan_cfg.ipv4_interface))
  {
    LOG_MSG_ERROR("Unable to get Netmask",0,0,0);
    return false;
  }
  addr.s_addr = public_ip;

  //add dhcp reservation record
  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
  {
    /*Remove any other dhcp reservation from file for this ip & host*/
    snprintf(command, MAX_COMMAND_STR_LEN,
               "sed -i '/%s,%s/d' %s", host, inet_ntoa(addr),DHCP_HOSTS_FILE);
    ds_system_call(command, strlen(command));

    //dhcp reservation entry in file
    snprintf(command, MAX_COMMAND_STR_LEN,
               "echo %s,%s >> %s", host, inet_ntoa(addr),DHCP_HOSTS_FILE);
    ds_system_call(command, strlen(command));
  }

  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
  {
    /*Remove any other dhcp reservation from file for this ip & mac*/
    snprintf(command, MAX_COMMAND_STR_LEN,
               "sed -i '/%s,%s/d' %s", mac, inet_ntoa(addr),DHCP_HOSTS_FILE);
    ds_system_call(command, strlen(command));

    //dhcp reservation entry in file
    snprintf(command, MAX_COMMAND_STR_LEN,
               "echo %s,%s >> %s", mac,inet_ntoa(addr),DHCP_HOSTS_FILE);
    ds_system_call(command, strlen(command));
  }

  /*Set the IP Passthrough Enable state*/
  QcMapLANMgr->lan_cfg.ip_passthrough_cfg.ip_passthrough_active = true;

  /* Removing conntrack entry for public ip. */
  QcMapTetheringMgr->DelConntrack(QcMapLANMgr->lan_cfg.ip_passthrough_cfg.public_ip);

  //storing WWAN Interface default route
  snprintf(data, MAX_COMMAND_STR_LEN,
           "ip route show | grep default | grep %s",
           QcMapBackhaulMgr->wan_cfg.ipv4_interface);
  if ((in = popen(data, "r")))
  {
    //reading buffer return by popen
    if(fgets(data, sizeof(data), in)!=NULL)
    {
       data[strlen(data) - 1] = '\0' ;
       snprintf(QcMapLANMgr->lan_cfg.ip_passthrough_cfg.rmnet_def_route,
                  MAX_COMMAND_STR_LEN,"%s", data);
    }
    pclose(in);
  }

  /* Assign local ip to WAN Interface iface. */
  addr.s_addr = QcMapLANMgr->lan_cfg.ip_passthrough_cfg.rmnet_reserved_ip;
  snprintf(command, MAX_COMMAND_STR_LEN,"ifconfig %s %s netmask %s up ",
           QcMapBackhaulMgr->wan_cfg.ipv4_interface, inet_ntoa(addr), "255.255.255.255");
  ds_system_call( command, strlen(command));


  /* Add WWAN Interface gw ip to bridge0. */
  addr.s_addr = default_gw_addr;
  snprintf(command, MAX_COMMAND_STR_LEN,"ip addr add %s/32 dev %s ", inet_ntoa(addr),BRIDGE_IFACE);
  ds_system_call( command, strlen(command));

  /* Route for gateway ip .reachable from WWAN Interface. */
  snprintf(command, MAX_COMMAND_STR_LEN,"ip route add %s dev %s",
           inet_ntoa(addr), QcMapBackhaulMgr->wan_cfg.ipv4_interface);
  ds_system_call( command, strlen(command));

  /* Adding stored default route as adding ip to WAN Interface removes default route */
  if (strlen(QcMapLANMgr->lan_cfg.ip_passthrough_cfg.rmnet_def_route) > 0)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,"ip route add default dev %s",
             QcMapBackhaulMgr->wan_cfg.ipv4_interface);
    ds_system_call( command, strlen(command));
  }

  addr.s_addr = public_ip;
  /* starting dnsmasq to provide reserve public IP */
  snprintf(data, MAX_COMMAND_STR_LEN,
           "ps -eo args | grep dnsmasq");
  if ((in = popen(data, "r")))
  {
    //reading buffer return by popen
    if(fgets(data, sizeof(data), in)!=NULL)
    {
      data[strlen(data) - 1] = '\0' ;

      ds_system_call("killall -9 dnsmasq", strlen("killall -9 dnsmasq"));

      snprintf(command, MAX_COMMAND_STR_LEN,"%s --dhcp-range=%s,%s,%s" ,
               data,inet_ntoa(addr),inet_ntoa(addr), APPS_SUB_NET_MASK);
      ds_system_call(command, strlen(command));

      //sending signal to dnsmasq to read reservervation file again
      snprintf(command, MAX_COMMAND_STR_LEN, "killall -1 dnsmasq");
      ds_system_call(command, strlen(command));

    }
    pclose(in);
  }
  else
  {
    LOG_MSG_ERROR("\nUnable to start dnsmasq to provide public ip\n",0,0,0);
  }

  /* Add WWAN Interface gw ip to bridge0. */
  addr.s_addr = default_gw_addr;
  snprintf(command, MAX_COMMAND_STR_LEN,"ip addr add %s/32 dev %s ", inet_ntoa(addr),BRIDGE_IFACE);
  ds_system_call( command, strlen(command));

  /**
    * Marking DHCP packets so that only dhcp packets can be handled by bridge
    * creating custom routing table 'ippastbl' for dhcp request  handle
    *
    */
  addr.s_addr = default_gw_addr;
  snprintf(data, MAX_COMMAND_STR_LEN,
           "ip route show table local | grep \"local %s\"",inet_ntoa(addr));
  if ((in = popen(data, "r")))
  {
     /* Reading buffer return by popen. */
    if(fgets(data, sizeof(data), in)!=NULL)
    {
      data[strlen(data) - 1] = '\0' ;
      ds_system_call("iptables -A PREROUTING -i bridge0 -t nat -p udp  --dport 67  -j"
                           " MARK --set-mark 1", strlen("iptables -A PREROUTING -i bridge0"
                           " -t nat -p udp  --dport 67  -j MARK --set-mark 1"));

      /* Adding custom routing table ippastbl. */
      ds_system_call("echo 200 ippastbl >> /etc/iproute2/rt_tables",
                            strlen("echo 200 ippastbl >> /etc/iproute2/rt_tables"));

      /* Removing gatway ip route from local table. */
      snprintf(command, MAX_COMMAND_STR_LEN,"ip route del %s table local" ,
                 data);
      ds_system_call(command, strlen(command));

      /* ip rule to handle marked packets. */
      snprintf(command, MAX_COMMAND_STR_LEN, "ip rule add fwmark 1 table ippastbl");
      ds_system_call(command, strlen(command));

      /* Handling gatway route from ippastbl table.*/
      snprintf(command, MAX_COMMAND_STR_LEN, "ip route add %s table ippastbl",data);
      ds_system_call(command, strlen(command));
      pclose(in);


    }
    else
    {
      LOG_MSG_ERROR("\nUnable to get gatway ip route from local table\n",0,0,0);
    }
  }

  /* Add route for the the IP. */
  addr.s_addr = public_ip;
  snprintf(command, MAX_COMMAND_STR_LEN,
             " ip route add %s dev %s", inet_ntoa(addr), BRIDGE_IFACE);
  ds_system_call( command, strlen(command));

  /* Add SNAT entry. */
  addr.s_addr = public_ip;
  snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -I POSTROUTING -o %s -j SNAT --to %s",
              QcMapBackhaulMgr->wan_cfg.ipv4_interface, inet_ntoa(addr));
  ds_system_call(command, strlen(command));

  /* Saving public ip and netmask*/
  QcMapLANMgr->lan_cfg.ip_passthrough_cfg.public_ip = public_ip;
  QcMapLANMgr->lan_cfg.ip_passthrough_cfg.public_netmask= netmask.s_addr;
  QcMapLANMgr->lan_cfg.ip_passthrough_cfg.rmnet_gateway_ip = default_gw_addr;

#ifndef FEATURE_DATA_TARGET_MDM9607
  /* Updating the IPACM XML with the wwan iface IP. */
  if(!xml_file.load_file(IPA_XML_PATH))
  {
    LOG_MSG_ERROR("Unable to load IPACM XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(IPACM_TAG).child(IPPassthroughFlag_TAG);
  root.child(IPPassthroughMode_TAG).text() = 1;
  QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
#endif

  LOG_MSG_INFO1("IP Passthrough Enabled \n",0,0,0);
  return true;
}

/*===========================================================================
  FUNCTION DisableIPPassthrough
==========================================================================*/
/*!
@brief
  Disables IP Passthrough.

@parameters
  bool - Boolean to indicate whether default route is added or not.

@return
  true  - on success
  false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_LAN::DisableIPPassthrough(bool default_route)
{
  qmi_error_type_v01 qmi_err_num;
  struct timespec ts;
  in_addr_t public_ip = 0,  sec_dns_addr = 0, default_gw_addr = 0;
  uint32 pri_dns_addr = 0;
  struct in_addr addr,gw_addr,netmask;
  char command[MAX_COMMAND_STR_LEN];
  char temp_buf[MAX_COMMAND_STR_LEN];
  qcmap_msgr_device_type_enum_v01 dev_type ;
  char host_str[MAX_STRING_LENGTH]={0};
  char mac_str[MAX_STRING_LENGTH]={0};
  qcmap_msgr_ip_passthrough_mode_enum_v01 status;
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_LAN *QcMapLANMgr = QCMAP_LAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr = QCMAP_Backhaul::Get_Instance(false);


  QCMAP_CM_LOG_FUNC_ENTRY();

  if (!QcMapLANMgr->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
  {
    LOG_MSG_ERROR("\nIP Passthrough is Already Disabled\n",0,0,0);
    return false;
  }

  QcMapLANMgr->lan_cfg.ip_passthrough_cfg.ip_passthrough_active = false;

  /* Delete the WWAN gw ip from bridge. */
  addr.s_addr = QcMapLANMgr->lan_cfg.ip_passthrough_cfg.rmnet_gateway_ip;
  snprintf(command, MAX_COMMAND_STR_LEN,
             " ip addr delete %s/32 dev %s", inet_ntoa(addr), BRIDGE_IFACE);
  ds_system_call( command, strlen(command));

  /* Delete route for the the IP. */
  addr.s_addr = QcMapLANMgr->lan_cfg.ip_passthrough_cfg.public_ip;
  snprintf(command, MAX_COMMAND_STR_LEN,
             " ip route delete %s dev %s", inet_ntoa(addr), BRIDGE_IFACE);
  ds_system_call( command, strlen(command));

  /* Delete SNAT entry. */
  addr.s_addr = QcMapLANMgr->lan_cfg.ip_passthrough_cfg.public_ip;
  snprintf( command, MAX_COMMAND_STR_LEN,
              "iptables -t nat -D POSTROUTING -o %s -j SNAT --to %s",
              QcMapBackhaulMgr->wan_cfg.ipv4_interface,
              inet_ntoa(addr));
  ds_system_call(command, strlen(command));

  /*Remove  dhcp reservation from file*/
  dev_type =  QcMapLANMgr->lan_cfg.ip_passthrough_cfg.device_type;

  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
               "sed -i '/%s,%s/d' %s", host_str, inet_ntoa(addr),DHCP_HOSTS_FILE);
    ds_system_call(command, strlen(command));
  }

  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
  {
    snprintf(command, MAX_COMMAND_STR_LEN,
               "sed -i '/%s,%s/d' %s", mac_str, inet_ntoa(addr),DHCP_HOSTS_FILE);
    ds_system_call(command, strlen(command));
  }

  /* Sending signal to dnsmasq to read reservervation file again. */
  snprintf(command, MAX_COMMAND_STR_LEN, "killall -1 dnsmasq");
  ds_system_call(command, strlen(command));
  QcMapTetheringMgr->DelConntrack(QcMapLANMgr->lan_cfg.ip_passthrough_cfg.rmnet_reserved_ip);

  /* Remove marking for DHCP packets.*/
  ds_system_call("iptables -D PREROUTING -i bridge0 -t nat -p udp  --dport 67  -j"
                       " MARK --set-mark 1", strlen("iptables -A PREROUTING -i bridge0"
                       " -t nat -p udp  --dport 67  -j MARK --set-mark 1"));

  /* Delete ip rule to handle marked packets */
  snprintf(command, MAX_COMMAND_STR_LEN, "ip rule del fwmark 1 table ippastbl");
  ds_system_call(command, strlen(command));

  /* Flush ippastbl  table. */
  snprintf(command, MAX_COMMAND_STR_LEN, "ip route flush table ippastbl");
  ds_system_call(command, strlen(command));

  /* Remove custom routing table ippastbl. */
  ds_system_call("sed -i '/200 ippastbl/d' /etc/iproute2/rt_tables",
                        strlen("sed -i '/200 ippastbl/d' /etc/iproute2/rt_tables"));
  if (QcMapBackhaulMgr &&
      QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_WWAN)
  {

    //Assign public ip to WWAN Interface iface
    addr.s_addr = QcMapLANMgr->lan_cfg.ip_passthrough_cfg.public_ip;
    netmask.s_addr = QcMapLANMgr->lan_cfg.ip_passthrough_cfg.public_netmask;
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s %s ",
             QcMapBackhaulMgr->wan_cfg.ipv4_interface, inet_ntoa(addr));
    snprintf(temp_buf, MAX_COMMAND_STR_LEN, " netmask %s up",inet_ntoa(netmask));
    strlcat(command, temp_buf, MAX_COMMAND_STR_LEN);
    ds_system_call( command, strlen(command));

    //route for gateway ip .reachable from WAN Interface
    addr.s_addr = QcMapLANMgr->lan_cfg.ip_passthrough_cfg.rmnet_gateway_ip;

    /*addding stored default route as adding ip to WAN Interface removes default route */
    if ((strlen(QcMapLANMgr->lan_cfg.ip_passthrough_cfg.rmnet_def_route) > 0) &&
        default_route)
    {
      snprintf(command, MAX_COMMAND_STR_LEN,"ip route add %s ",
               QcMapLANMgr->lan_cfg.ip_passthrough_cfg.rmnet_def_route);
      ds_system_call( command, strlen(command));
    }
  }

  memset(&QcMapLANMgr->lan_cfg.ip_passthrough_cfg.rmnet_def_route, 0,
         MAX_COMMAND_STR_LEN);

#ifndef FEATURE_DATA_TARGET_MDM9607
/* Update IPACM XML. */
  if (!xml_file.load_file(IPA_XML_PATH))
  {
    LOG_MSG_ERROR("Unable to load IPACM XML file.",0,0,0);
    return false;
  }
    root = xml_file.child(System_TAG).child(IPACM_TAG).child(IPPassthroughFlag_TAG);
    root.child(IPPassthroughMode_TAG).text() = 0;
    QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
#endif

  LOG_MSG_INFO1("IP Passthrough Disabled \n",0,0,0);
  return true;
}

/*==========================================================
  FUNCTION SetIPPassthroughConfig
==========================================================*/
/*!
@brief
  Sets the IP Passthrough configuration

@parameters
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state,
  bool new_config ,
  qcmap_msgr_ip_passthrough_config_v01 *passthrough_config,
  qmi_error_type_v01 * qmi_err_num

@return
  true  - on Success
  false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================*/

boolean QCMAP_LAN::SetIPPassthroughConfig
(
  qcmap_msgr_ip_passthrough_mode_enum_v01 enable_state,
  bool new_config ,
  qcmap_msgr_ip_passthrough_config_v01 *passthrough_config,
  qmi_error_type_v01 * qmi_err_num
)
{
  struct in_addr addr;
  char data[MAX_STRING_LENGTH] = {0};
  char str[INET6_ADDRSTRLEN];
  pugi::xml_document xml_file;
  pugi::xml_node root, parent,child, subchild;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  QCMAP_LAN * QcMapLANMgr = QCMAP_LAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];
  char temp_buf[MAX_COMMAND_STR_LEN];
  boolean mac_addr_non_empty = true;
  char mac_addr_string[QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01];
  qcmap_msgr_ip_passthrough_mode_enum_v01 old_enable_state;

  QCMAP_CM_LOG_FUNC_ENTRY();

  mac_addr_non_empty = check_non_empty_mac_addr(passthrough_config->mac_addr,
                                                mac_addr_string);

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG)\
            .child(MobileAPLanCfg_TAG).child(IPPassthroughCfg_TAG);
  snprintf(data, sizeof(data), "%d", enable_state);

  if (enable_state == QCMAP_MSGR_IP_PASSTHROUGH_MODE_UP_V01)
  {
    LOG_MSG_INFO1("SetIP passthrough mode up",0,0,0);

    if (QcMapLANMgr->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
    {
        LOG_MSG_ERROR("\nAlready enabled IP Passthrough",0,0,0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return false;
    }

    if (new_config)// Give a new config for IP Passthrough
    {
      /*
        Update the XML with the new ip passthrough config
      */
      switch (passthrough_config->device_type)
      {
        case QCMAP_MSGR_DEVICE_TYPE_USB_V01:
          root.child(IPPassthroughDeviceType_TAG).text() = "USB";
          root.child(IPPassthroughHostName_TAG).text() = passthrough_config->client_device_name;
          LOG_MSG_INFO1("\nDevice Type = USB",0,0,0);
        break;

        case QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01:
          root.child(IPPassthroughDeviceType_TAG).text() = "ETH";
          if (mac_addr_non_empty) {
             root.child(IPPassthroughMacAddr_TAG).text() = mac_addr_string;
          }
          else {
          LOG_MSG_ERROR("Empty MAC Address.", 0, 0, 0);
          return false;
          }
          LOG_MSG_INFO1("\nDevice Type = ETH", 0, 0, 0);
        break;

        default:
          LOG_MSG_ERROR("Invalid Device type.", 0, 0, 0);
          return false;
      }
    }
    else
    {
      // Check if there is an existing configuration
      if(QcMapLANMgr->GetIPPassthroughConfig
        (&old_enable_state, passthrough_config,qmi_err_num))
      {
        if (!(passthrough_config->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01 ||
            passthrough_config->device_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01))
        {
          LOG_MSG_ERROR("No existing configuration\n",0,0,0);
          *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
          return false;
        }
      }
      else
      {
        LOG_MSG_ERROR("Get IP Passthrough Flag failed\n",0,0,0);
        return false;
      }
    }

    // Set the XML flag with the ip passthrough state
    root.child(IPPassthroughEnable_TAG).text() = data;

    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

    if (passthrough_config->device_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
    {
      if (!(QcMapTetheringMgr &&
          QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) == true))
      {
        LOG_MSG_ERROR("\nUSB Link Not Enabled",0,0,0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return true;
      }
    }
    if (passthrough_config->device_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
    {
      if (!(QcMapTetheringMgr &&
            QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) == true))
      {
        LOG_MSG_ERROR("\nEthernet Link Not Enabled",0,0,0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return true;
      }
    }

    //Check if backhaul is connected
    if (qcmap_cm_get_state(qmi_err_num) != QCMAP_CM_WAN_CONNECTED )
    {
      LOG_MSG_ERROR("WWAN is not Connected.",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return true;
    }

    //Check if current backhaul is WWAN
    if (!(QcMapBackhaulMgr &&
        QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_WWAN ))
    {
      LOG_MSG_INFO1("Current backhaul mode is not WWAN.",0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return true;
    }

    LOG_MSG_INFO1("\nRestart the tethered client ",0,0,0);
    QcMapLANMgr->RestartTetheredClient(passthrough_config->device_type);
  }

  else if (enable_state == QCMAP_MSGR_IP_PASSTHROUGH_MODE_DOWN_V01)
  {
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG)\
            .child(MobileAPLanCfg_TAG).child(IPPassthroughCfg_TAG);
    snprintf(data, sizeof(data), "%d", enable_state);

    // Set the XML flag with the ip passthrough state
    root.child(IPPassthroughEnable_TAG).text() = data;
    /* Update the XML. */
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

    /* Restart the tethered client if passthrough active. */
    if(QcMapLANMgr->lan_cfg.ip_passthrough_cfg.ip_passthrough_active)
    {
      QcMapLANMgr->RestartTetheredClient(
      QcMapLANMgr->lan_cfg.ip_passthrough_cfg.device_type);
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION GetIPPassthroughConfigFromXML
==========================================================================*/
/*!
@brief
  Get IP Passthrough Status/Configuration

@parameters
  qcmap_msgr_ip_passthrough_mode_enum_v01 *enable_state,
  qcmap_msgr_ip_passthrough_config_v01 *passthrough_config,
  qmi_error_type_v01 *qmi_err_num

@return
  true  -on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_LAN::GetIPPassthroughConfig
(
  qcmap_msgr_ip_passthrough_mode_enum_v01 *enable_state,
  qcmap_msgr_ip_passthrough_config_v01 *passthrough_config,
  qmi_error_type_v01 *qmi_err_num
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  char content_buf[MAX_STRING_LENGTH];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN* QcMLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_CM_LOG_FUNC_ENTRY();
  LOG_MSG_INFO1("\nGetIPPassthrough in API",0,0,0);
  if (!xml_file.load_file(QCMAP_ConnectionManager::xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG)\
          .child(MobileAPLanCfg_TAG).child(IPPassthroughCfg_TAG);
  *enable_state = atoi(root.child(IPPassthroughEnable_TAG ).child_value());

  strlcpy(content_buf,root.child(IPPassthroughDeviceType_TAG).child_value(),MAX_STRING_LENGTH);
  if (strncmp(content_buf,"USB",MAX_STRING_LENGTH) == 0)
  {
    passthrough_config->device_type = QCMAP_MSGR_DEVICE_TYPE_USB_V01;
    strlcpy(passthrough_config->client_device_name,
              root.child(IPPassthroughHostName_TAG).child_value(),
              QCMAP_MSGR_DEVICE_NAME_MAX_V01);
  }
  else if (strncmp(content_buf,"ETH",MAX_STRING_LENGTH) == 0)
  {
    passthrough_config->device_type = QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01;
    strlcpy(content_buf,
              root.child(IPPassthroughMacAddr_TAG).child_value(), MAX_STRING_LENGTH);
    LOG_MSG_INFO1("content_buf: %s",content_buf,0,0);
    for (int i = 0; i < QCMAP_MSGR_MAC_ADDR_LEN_V01; i++)
    {
      passthrough_config->mac_addr[i] =
      (ds_hex_to_dec(content_buf[i * 3]) << 4) |ds_hex_to_dec(content_buf[i * 3 + 1]);
    }
  }
  else
  {
    passthrough_config->device_type = -1;
  }

  return true;
}

/*===========================================================================
  FUNCTION RestartTetheredClient
==========================================================================*/
/*!
@brief
  Restart the tethered client

@parameters
  qcmap_msgr_device_type_enum_v01 dev_type

@return
  true  -on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_LAN::RestartTetheredClient
(
  qcmap_msgr_device_type_enum_v01 dev_type
)

{
  char command[MAX_COMMAND_STR_LEN];
  char temp_buf[MAX_COMMAND_STR_LEN];
  QCMAP_CM_LOG_FUNC_ENTRY();
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);

  /*Restart USB client*/
  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_USB_V01)
  {
    //Check if USB is enabled
    if (!(QcMapTetheringMgr &&
      QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) == true))
    {
      LOG_MSG_ERROR("USB Link Not Enabled\n",0,0,0);
      return false;
    }

    if (QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV4() ||
        QCMAP_Backhaul_Cradle::IsCradleBackhaulAvailableV6())
    {
      LOG_MSG_ERROR("Cradle Active. Do not Restart. \n",0,0,0);
      return false;
    }

    LOG_MSG_INFO1("RestartTetheredClient: USB",0,0,0);

    ds_system_call("echo 0 > /sys/class/android_usb/android0/enable ",
                          strlen("echo 0 > /sys/class/android_usb/android0/enable "));
    usleep(500000);
    /* Delay for 1 secs since USB UP/DOWN in immediate instance causes issues*/
    ds_system_call("echo 1 > /sys/class/android_usb/android0/enable ",
                          strlen("echo 1 > /sys/class/android_usb/android0/enable "));

  }

  /* Restart Ethernet client*/
  if (dev_type == QCMAP_MSGR_DEVICE_TYPE_ETHERNET_V01)
  {
    //Check if ETHERNET is enabled
    if (!(QcMapTetheringMgr &&
        QcMapTetheringMgr->GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2) == true))
    {
      //Check if Ethernet is enabled
      LOG_MSG_ERROR("\nEthernet Link Not Enabled",0,0,0);
      return false;
    }

    if (QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV4() ||
        QCMAP_Backhaul_Ethernet::IsEthBackhaulAvailableV6())
    {
      LOG_MSG_ERROR("Ethernet Backhaul Active. Do not Restart. \n",0,0,0);
      return false;
    }

    LOG_MSG_INFO1("RestartTetheredClient: Ethernet",0,0,0);

    strlcpy(command, "ifconfig ", MAX_COMMAND_STR_LEN);
    snprintf(temp_buf, MAX_COMMAND_STR_LEN, "%s down ", ETH0);
    strlcat(command, temp_buf, MAX_COMMAND_STR_LEN);
    ds_system_call( command, strlen(command));
  }
  return true;
}

/*===========================================================================
  FUNCTION GetIPPassthroughState
==========================================================================*/
/*!
@brief
  Get IP Passthrough State

@parameters
  bool *active_state

@return
  true  -on success
  flase - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_LAN::GetIPPassthroughState
(
  bool *active_state,
  qmi_error_type_v01 *qmi_err_num
)

{
  char command[MAX_COMMAND_STR_LEN];
  char temp_buf[MAX_COMMAND_STR_LEN];
  QCMAP_CM_LOG_FUNC_ENTRY();
  QCMAP_LAN* QcMapLANMgr=QCMAP_LAN::Get_Instance(false);

  if (!QcMapLANMgr)
  {
    LOG_MSG_ERROR("LAN Not active \n",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  *active_state = QcMapLANMgr->lan_cfg.ip_passthrough_cfg.ip_passthrough_active;

  return true;
}
