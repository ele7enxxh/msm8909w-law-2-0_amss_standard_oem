/*====================================================

FILE:  QCMAP_Backhaul_WWAN.cpp

SERVICES:
   QCMAP Connection Manager Backhaul (WWAN) Specific Implementation

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/07/14   ka         Created

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
#include "ds_qmi_qcmap_msgr.h"



#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_LAN.h"







bool QCMAP_Backhaul_WWAN::flag= false;
//boolean QCMAP_Backhaul_WWAN::roaming= 1;
QCMAP_Backhaul_WWAN* QCMAP_Backhaul_WWAN::object=NULL;
QCMAP_Backhaul_WWAN* QCMAP_Backhaul_WWAN::Get_Instance(boolean obj_create)
{
  if(!flag && obj_create)
  {
    QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(true);
    LOG_MSG_INFO1("Creating object : WWAN",0,0,0);
    object = new QCMAP_Backhaul_WWAN();
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
  Initializes Backhaul WWAN variables.

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


QCMAP_Backhaul_WWAN::QCMAP_Backhaul_WWAN
(
)
{
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  memset(&this->wwan_cfg, 0, sizeof(this->wwan_cfg));

  this->ipv6_prefix_based_rules_added = false;
  this->wwan_cfg.default_sip_server_info.sip_serv_type = QCMAP_CM_SIP_SERVER_FQDN;
  strlcpy( this->wwan_cfg.default_sip_server_info.sip_serv_info.sip_fqdn,
           "abcd.com", ( QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01 ));
  strlcpy( this->wwan_cfg.ddns_server_info.server_url,
          QCMAP_DYNDNS_SERVER_URL, QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);
  strlcpy( this->wwan_cfg.ddns_server_info.protocol,
          QCMAP_DYNDNS_PROTOCOL, QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);

  if (QcMapMgr->xml_path)
  {
   if (ReadWWANConfigFromXML())
   {
   return;
   }

   /* Read Config from XML failed. Use defaults. */
   LOG_MSG_INFO1("QCMAP Read XML failed.\n",0,0,0);
  }


  this->wwan_cfg.tech = QCMAP_WAN_TECH_ANY;
  this->wwan_cfg.profile_id.v4.umts_profile_index =
  this->wwan_cfg.profile_id.v6.umts_profile_index = 0;
  this->wwan_cfg.profile_id.v4.cdma_profile_index =
  this->wwan_cfg.profile_id.v6.cdma_profile_index = 0;

  LOG_MSG_INFO1(" Sip server count: %d ", this->wwan_cfg.sip_server_list_count, 0, 0);

  return;

}

/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
  Destroyes the Backhaul_WWAN Object.

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

QCMAP_Backhaul_WWAN::~QCMAP_Backhaul_WWAN()
{
  qcmap_sip_server_info_list_t *sip_server_info_list = NULL;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=
                                QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=
                                     QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_Ethernet* QcMapBackhaulEthMgr=
                                   QCMAP_Backhaul_Ethernet::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=
                                     QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  LOG_MSG_INFO1("Destroying Object: WWAN",0,0,0);

  flag=false;
  object=NULL;
  if(!QcMapBackhaulWLANMgr && !QcMapBackhaulCradleMgr && !QcMapBackhaulEthMgr)
    delete QcMapBackhaulMgr;
}


/*===========================================================================
  FUNCTION StopDDNS
==========================================================================*/
/*!
@brief
  Stop dynamic dns.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::StopDDNS()
{
  char command[MAX_COMMAND_STR_LEN];
  FILE *fp=NULL;
  int ddns_pid=0;

  LOG_MSG_INFO1("Stop DDNS",0,0,0);
  fp = fopen(DDCLIENT_PID_FILE, "r");

  if ( fp == NULL )
  {
    LOG_MSG_ERROR("Error opening ddclient pid file: %d.\n", errno, 0, 0);
    return false;
  }
  else if (fscanf(fp, "%d", &ddns_pid) != 1)
  {
    LOG_MSG_ERROR("Error reading ddclient file: %d.\n", errno, 0, 0);
    ddns_pid = 0;
    fclose(fp);
    return false;
  }
  else
  {
    LOG_MSG_INFO1("ddclient PID = %d",ddns_pid,0,0);
    snprintf(command,
           MAX_COMMAND_STR_LEN,
           "kill -9 %d",ddns_pid);
    ds_system_call(command, strlen(command));
    fclose(fp);
  }
  return true ;
}

/*===========================================================================
  FUNCTION : ReadWWANConfigFromXML
==========================================================================*/
/*!
@brief
  Reads Writes Config from xml

@parameters
-None

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


boolean QCMAP_Backhaul_WWAN::ReadWWANConfigFromXML()
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  struct in_addr addr;
  uint32_t debug_string=0;
  char str[INET6_ADDRSTRLEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);
  this->wwan_cfg.roaming =  atoi(root.child(Roaming_TAG).child_value());


  strlcpy(QcMapBackhaulMgr->wan_cfg.eri_config_file, root.child(EriConfig_TAG).child_value(),
          QCMAP_CM_MAX_FILE_LEN);


  this->wwan_cfg.profile_id.v4.umts_profile_index = atoi(root.child(V4_UMTS_PROFILE_INDEX_TAG).child_value());
  this->wwan_cfg.profile_id.v4.cdma_profile_index = atoi(root.child(V4_CDMA_PROFILE_INDEX_TAG).child_value());
  this->wwan_cfg.profile_id.v6.umts_profile_index = atoi(root.child(V6_UMTS_PROFILE_INDEX_TAG).child_value());
  this->wwan_cfg.profile_id.v6.cdma_profile_index = atoi(root.child(V6_CDMA_PROFILE_INDEX_TAG).child_value());



  if (0 == strcasecmp(root.child(TECH_TAG).child_value(), TECH_ANY_TAG))
  {
    this->wwan_cfg.tech = QCMAP_WAN_TECH_ANY;
  }
  if (0 == strcasecmp(root.child(TECH_TAG).child_value(), TECH_3GPP_TAG))
  {
    this->wwan_cfg.tech = QCMAP_WAN_TECH_3GPP;
  }
  if (0 == strcasecmp(root.child(TECH_TAG).child_value(), TECH_3GPP2_TAG))
  {
    this->wwan_cfg.tech = QCMAP_WAN_TECH_3GPP2;
  }

  if (strncasecmp(IP_TAG, root.child(DefaultSIPServerConfigType_TAG).child_value(), strlen(IP_TAG)) == 0)
  {
    this->wwan_cfg.default_sip_server_info.sip_serv_type =
    QCMAP_CM_SIP_SERVER_ADDR;
  }
  else if (strncasecmp(FQDN_TAG, root.child(DefaultSIPServerConfig_TAG).child_value(), strlen(FQDN_TAG)) == 0)
  {
    this->wwan_cfg.default_sip_server_info.sip_serv_type =
    QCMAP_CM_SIP_SERVER_FQDN;
  }

  if (this->wwan_cfg.default_sip_server_info.sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
  {
    if (inet_aton(root.child(DefaultSIPServerConfig_TAG).child_value(), &addr))
      this->wwan_cfg.default_sip_server_info.sip_serv_info.sip_serv_ip = addr.s_addr;

    debug_string=htonl(this->wwan_cfg.default_sip_server_info.sip_serv_info.sip_serv_ip);
    readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
    LOG_MSG_INFO1("\nSIP Server IP:'%s'",str,0,0);

  }
  else if (this->wwan_cfg.default_sip_server_info.sip_serv_type ==
           QCMAP_CM_SIP_SERVER_FQDN)
  {
    strlcpy(this->wwan_cfg.default_sip_server_info.sip_serv_info.sip_fqdn,
            root.child(DefaultSIPServerConfig_TAG).child_value(),
            sizeof(this->wwan_cfg.default_sip_server_info.sip_serv_info.sip_fqdn));
    LOG_MSG_INFO1("SIP FQDN: %s",this->wwan_cfg.default_sip_server_info.sip_serv_info.sip_fqdn,0,0)
  }

  this->wwan_cfg.prefix_delegation = atoi(root.child(PrefixDelegation_TAG).child_value());
  LOG_MSG_INFO1("WWAN Tech: %d SIP Server type: %d " , this->wwan_cfg.tech,
  this->wwan_cfg.default_sip_server_info.sip_serv_type ,0);


  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(DDNSCFG_TAG);

  this->wwan_cfg.ddns_conf.enable = atoi(root.child(DDNS_Enable_TAG).child_value());
  LOG_MSG_INFO1("DDNS Config: %d", this->wwan_cfg.ddns_conf.enable, 0, 0);

  strlcpy(this->wwan_cfg.ddns_conf.server_info.server_url,
          root.child(DDNS_Server_TAG).child_value(), QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);

  strlcpy(this->wwan_cfg.ddns_conf.server_info.protocol,
          root.child(DDNS_Protocol_TAG).child_value(), QCMAP_DDNS_PROTOCOL_LEN);

  strlcpy(this->wwan_cfg.ddns_conf.login,
          root.child(DDNS_Login_TAG).child_value(), QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01);

  strlcpy(this->wwan_cfg.ddns_conf.password,
          root.child(DDNS_Password_TAG).child_value(), QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01);

  strlcpy(this->wwan_cfg.ddns_conf.hostname,
        root.child(DDNS_Hostname_TAG).child_value(), QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);

  this->wwan_cfg.ddns_conf.timeout = atoi(root.child(DDNS_Timeout_TAG).child_value());

  LOG_MSG_INFO1("Server: %s Protocol: %s Login: %s",
                this->wwan_cfg.ddns_conf.server_info.server_url,
                this->wwan_cfg.ddns_conf.server_info.protocol,
                this->wwan_cfg.ddns_conf.login);

  LOG_MSG_INFO1("Password: %s Hostname: %s Timeout: %d",
                this->wwan_cfg.ddns_conf.password,
                this->wwan_cfg.ddns_conf.hostname,
                this->wwan_cfg.ddns_conf.timeout);

  return true;
}
/*===========================================================================
  FUNCTION : GetSetWWANConfigFromXML
==========================================================================*/
/*!
@brief
 Gets/Sets WWAN specific config

@parameters
-qcmap_wwan_config_enum conf
-qcmap_action_type action
-char *data
-int data_len

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

boolean QCMAP_Backhaul_WWAN::GetSetWWANConfigFromXML
(
qcmap_wwan_config_enum conf,
qcmap_action_type action,
char *data,
int data_len
)
{

  pugi::xml_document xml_file;
  pugi::xml_node  root, child;
  char *tag_ptr = NULL;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }


  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);

  switch(conf)
  {
    case CONFIG_V4_UMTS_PROFILE_INDEX:
      tag_ptr = V4_UMTS_PROFILE_INDEX_TAG;
    break;

    case CONFIG_V4_CDMA_PROFILE_INDEX:
     tag_ptr = V4_CDMA_PROFILE_INDEX_TAG;
    break;

    case CONFIG_V6_UMTS_PROFILE_INDEX:
     tag_ptr = V6_UMTS_PROFILE_INDEX_TAG;
    break;

    case CONFIG_V6_CDMA_PROFILE_INDEX:
      tag_ptr = V6_CDMA_PROFILE_INDEX_TAG;
    break;

    case CONFIG_ROAMING:
      tag_ptr = Roaming_TAG;
    break;

    case CONFIG_TECH:
     tag_ptr = TECH_TAG;
    break;

    case CONFIG_PREFIX_DELEGATION:
     tag_ptr = PrefixDelegation_TAG;
    break;

    default:
      LOG_MSG_ERROR("Invalid TAG.", 0, 0, 0);
  }


  if (action == SET_VALUE)
  {
     root.child(tag_ptr).text() = data;
     QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
   else
     strlcpy(data, root.child(tag_ptr).child_value(), data_len);

  LOG_MSG_INFO1("Config: %d Action: %d data: %d " , conf, action, atoi(data));

  return true;
}

/*===========================================================================
  FUNCTION : GetSetDefaultSIPServerConfigFromXML
==========================================================================*/
/*!
@brief
 Gets/Sets Default SIP Server specific config

@parameters
- qcmap_action_type action,
  qcmap_cm_sip_server_info_t *default_sip_server_info

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

boolean QCMAP_Backhaul_WWAN::GetSetDefaultSIPServerConfigFromXML
(
  qcmap_action_type action,
  qcmap_cm_sip_server_info_t *default_sip_server_info
)
{
  /*
     qcmap_cm_sip_server_info_t default_sip_server_info;
     boolean  auto_connect;
    */
  pugi::xml_document xml_file;
  pugi::xml_node root,child;
  in_addr addr;
  uint32_t debug_string=0;
  char str[INET6_ADDRSTRLEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);

  switch (action)
  {
    case GET_VALUE:
      if ( strncasecmp(IP_TAG, root.child(DefaultSIPServerConfigType_TAG).child_value(),
           strlen(IP_TAG)) == 0 )
      {
        default_sip_server_info->sip_serv_type = QCMAP_CM_SIP_SERVER_ADDR;
      }
      else if ( strncasecmp(FQDN_TAG, root.child(DefaultSIPServerConfigType_TAG).child_value(),
                strlen(FQDN_TAG)) == 0 )
      {
        default_sip_server_info->sip_serv_type = QCMAP_CM_SIP_SERVER_FQDN;
      }

      if (default_sip_server_info->sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
      {
        if (inet_aton(root.child(DefaultSIPServerConfig_TAG).child_value(), &addr))
          default_sip_server_info->sip_serv_info.sip_serv_ip = addr.s_addr;
      }
      else if (default_sip_server_info->sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN)
      {
        strlcpy(default_sip_server_info->sip_serv_info.sip_fqdn,
                root.child(DefaultSIPServerConfig_TAG).child_value(),
                sizeof(default_sip_server_info->sip_serv_info.sip_fqdn));
      }
    break;

    case SET_VALUE:
      if (default_sip_server_info->sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
      {
        root.child(DefaultSIPServerConfigType_TAG).text() = IP_TAG;
        addr.s_addr = default_sip_server_info->sip_serv_info.sip_serv_ip;
        root.child(DefaultSIPServerConfig_TAG).text() = inet_ntoa(addr);
        readable_addr( AF_INET,
                       (const uint32_t *)&default_sip_server_info->sip_serv_info.sip_serv_ip,
                       (char *)&str );
        LOG_MSG_INFO1("\nSIP Server IP:'%s'",str,0,0);
      }
      else if (default_sip_server_info->sip_serv_type == QCMAP_CM_SIP_SERVER_FQDN)
      {
        root.child(DefaultSIPServerConfigType_TAG).text() = FQDN_TAG;
        root.child(DefaultSIPServerConfig_TAG).text() =
                                default_sip_server_info->sip_serv_info.sip_fqdn;

        LOG_MSG_INFO1("SIP fqdn: '%s' " ,default_sip_server_info->sip_serv_info.sip_fqdn,
                      0,0);
      }
      QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML,&xml_file);
      break;
  }

  LOG_MSG_INFO1("SIP Server type: %d Action: %d    " ,
                default_sip_server_info->sip_serv_type,action ,0);
  return true;
}


/*===========================================================================
  FUNCTION ConnectBackHaul
==========================================================================*/
/*!
@brief
  Tries to connect to backhaul. As a part of connect to backhaul checks for the
  network service and nas configuration.
  If the network is in roaming mode,  connects to backhaul based on the roaming flag configuration.

@parameters
  qcmap_msgr_wwan_call_type_v01 call_type
  int                           *err_num

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
boolean QCMAP_Backhaul_WWAN::ConnectBackHaul
(
  qcmap_msgr_wwan_call_type_v01 call_type,
  int *err_num,
  qmi_error_type_v01 *qmi_err_num
)
{
  int qcmap_cm_errno;
  int ret_val;
  int rc = 0;
  struct timespec ts;
  in_addr_t public_ip = 0,  sec_dns_addr = 0, default_gw_addr = 0;
  uint32 pri_dns_addr = 0;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_CM_LOG_FUNC_ENTRY();

  LOG_MSG_INFO1("In ConnectBackHaul %x Call Type:%d\n",
                QcMapMgr->qcmap_cm_handle, call_type, 0);

  ret_val = qcmap_cm_connect_backhaul(QcMapMgr->qcmap_cm_handle, call_type, &qcmap_cm_errno, qmi_err_num);

  if (ret_val == QCMAP_CM_SUCCESS)
  {
    *err_num = qcmap_cm_errno;
    return true;
  }

  if (ret_val == QCMAP_CM_ERROR && qcmap_cm_errno == QCMAP_CM_EWOULDBLOCK)
  {
    LOG_MSG_INFO1("QCMAP WAN Connecting Inprogress \n",0,0,0);
    return true;
  }
  else
  {
    LOG_MSG_INFO1( "QCMAP WAN Connecting Fail, ret_val %d qcmap_cm_errno %d\n",
                  ret_val, qcmap_cm_errno,0 );
    return false;
  }
}

/*===========================================================================
  FUNCTION DisconnectBackHaul
==========================================================================*/
/*!
@brief
  Brings down the bachaul service.

@parameters
  qcmap_msgr_wwan_call_type_v01 call_type
  int                           *err_num

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
boolean QCMAP_Backhaul_WWAN::DisconnectBackHaul
(
  qcmap_msgr_wwan_call_type_v01 call_type,
  int *err_num,
  qmi_error_type_v01 *qmi_err_num
)
{
  int qcmap_cm_errno;
  int ret_val;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_CM_LOG_FUNC_ENTRY();

  ret_val = qcmap_cm_disconnect_backhaul(QcMapMgr->qcmap_cm_handle, call_type, &qcmap_cm_errno, qmi_err_num);

  if (ret_val == QCMAP_CM_SUCCESS)
  {
    QCMAPLANMgr->enable_dns = true;
    *err_num = qcmap_cm_errno;
    return true;
  }
  if (ret_val == QCMAP_CM_ERROR && qcmap_cm_errno == QCMAP_CM_EWOULDBLOCK)
  {
    /* Backhaul disconnecting in progress */
    QCMAPLANMgr->enable_dns = true;
    return true;
  }
  else
  {
    return false;
  }
}

/*===========================================================================
  FUNCTION SetAutoconnect
==========================================================================*/
/*!
@brief
  This function will enable the autoconnect flag.
  If the mobileap and auto connect is enabled, connect to backhaul is triggered.
  In case connect to backhaul fails, then a autoconnect timer is triggered. After the expiry of
  autoconnect timer a new request to connect to backhaul is initiated.

@parameters
  boolean enable

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
boolean QCMAP_Backhaul_WWAN::SetAutoconnect
(
  boolean enable,
  qmi_error_type_v01 *qmi_err_num
)
{
  int qcmap_cm_errno;
  int ret_val;
  boolean timer_running;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  char data[MAX_STRING_LENGTH] = {0};
  pugi::xml_document xml_file;
  pugi::xml_node root;
  QCMAP_CM_LOG_FUNC_ENTRY();

  snprintf(data, sizeof(data), "%d", enable);
  /* Config Auto Connect */
  ret_val = qcmap_cm_set_auto_connect(QcMapMgr->qcmap_cm_handle, &qcmap_cm_errno, enable, qmi_err_num);
  if (ret_val != QCMAP_CM_SUCCESS)
    return false;

  QcMapMgr->auto_connect = enable;

  /* Update XML file directly. */
  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPWanCfg_TAG);

  snprintf(data, sizeof(data), "%d", enable);
  root.child(AutoConnect_TAG).text() = data;
  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  return true;
}
/*===========================================================================
  FUNCTION GetAutoconnect
==========================================================================*/
/*!
@brief
  Returns autoconnect flag value.

@parameters
  value

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

boolean QCMAP_Backhaul_WWAN::GetAutoconnect()
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  return QcMapMgr->auto_connect;
}
/*===========================================================================
  FUNCTION SetRoaming
==========================================================================*/
/*!
@brief
  This function will set the roaming flag value. Enabling the roaming flag
  will enable connecting to a Network in roaming mode.

@parameters
  boolean enable

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

boolean QCMAP_Backhaul_WWAN::SetRoaming(boolean enable, qmi_error_type_v01 *qmi_err_num)
{
  int ret_val;
  char roaming_state[MAX_STRING_LENGTH]={0};
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  snprintf(roaming_state, sizeof(roaming_state), "%d", enable);
  if(QcMapBackhaulWWANMgr)
  {
    /* Config Auto Connect */
    ret_val = qcmap_cm_set_roaming(QcMapMgr->qcmap_cm_handle, qmi_err_num, enable);
    if (ret_val != QCMAP_CM_SUCCESS)
      return false;


    QcMapBackhaulWWANMgr->wwan_cfg.roaming = enable;
  }
  /* if it is OK, call write to XML */
  GetSetWWANConfigFromXML(CONFIG_ROAMING,SET_VALUE,roaming_state,strlen(roaming_state));


  return true;
}


/*===========================================================================
  FUNCTION GetRoaming
==========================================================================*/
/*!
@brief
  Returns roaming flag value.

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

boolean QCMAP_Backhaul_WWAN::GetRoaming()
{
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  char enable[MAX_STRING_LENGTH] = {0};

  if (QcMapBackhaulWWANMgr)
    return QcMapBackhaulWWANMgr->wwan_cfg.roaming;
else
{
  if(GetSetWWANConfigFromXML(CONFIG_ROAMING,GET_VALUE,enable,MAX_STRING_LENGTH))
  {
    return ((boolean)(atoi(enable)));
  }
  else
  return false;
  LOG_MSG_ERROR("Could not read value from XML",0,0,0)
}

}
/*===========================================================================
  FUNCTION GetWWANStatistics
==========================================================================*/
/*!
@brief
  Used to Get the WWAN data transfer statistics.

@parameters
  qcmap_msgr_ip_family_enum_v01       ip_family
  qcmap_msgr_wwan_statistics_type_v01 *wwan_stats
  int                                 *p_error

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
boolean QCMAP_Backhaul_WWAN::GetWWANStatistics(
                                   qcmap_msgr_ip_family_enum_v01 ip_family,
                                   qcmap_msgr_wwan_statistics_type_v01 *wwan_stats,
                                   qmi_error_type_v01 *qmi_err_num)
{
  int ret_val;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_CM_LOG_FUNC_ENTRY();

  /* Get WWAN Statistics */
  ret_val = qcmap_cm_get_wwan_statistics(QcMapMgr->qcmap_cm_handle,
                                         ip_family,
                                         wwan_stats,
                                         qmi_err_num);
  if ( ret_val != QCMAP_CM_SUCCESS )
    return false;

  return true;
}

/*===========================================================================
  FUNCTION ResetWWANStatistics
==========================================================================*/
/*!
@brief
  Resets WWAN data transfer statistics to zero.

@parameters
  qcmap_msgr_ip_family_enum_v01       ip_family
  int                                 *p_error

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
boolean QCMAP_Backhaul_WWAN::ResetWWANStatistics(
                                   qcmap_msgr_ip_family_enum_v01 ip_family,
                                   qmi_error_type_v01 *qmi_err_num)
{
  int ret_val;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_CM_LOG_FUNC_ENTRY();

  /* Reset WWAN Statistics */
  ret_val = qcmap_cm_reset_wwan_statistics(QcMapMgr->qcmap_cm_handle,
                                           ip_family,
                                           qmi_err_num);
  if ( ret_val != QCMAP_CM_SUCCESS )
    return false;

  return true;
}
/*===========================================================================
  FUNCTION GetWWANStatus
==========================================================================*/
/*!
@brief
  Gets the WWAN status.

@parameters
  uint8_t *status

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

boolean QCMAP_Backhaul_WWAN::GetWWANStatus(qcmap_msgr_wwan_call_type_v01 call_type, uint8_t *status, qmi_error_type_v01 *qmi_err_num)
{
  if ( status == NULL )
  {
      LOG_MSG_ERROR("NULL args", 0, 0, 0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
    if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V4_V01 )
      *status=qcmap_cm_get_state(qmi_err_num);
    else if ( call_type == QCMAP_MSGR_WWAN_CALL_TYPE_V6_V01 )
      *status=qcmap_cm_get_ipv6_state(qmi_err_num);
    else
    {
       LOG_MSG_ERROR("Invalid Call Type:%d", call_type, 0, 0);
       *qmi_err_num = QMI_ERR_INVALID_IP_FAMILY_PREF_V01;
       return false;
    }
   return true;
}

/*===========================================================================
  FUNCTION GetWWANPolicy
==========================================================================*/
/*!
@brief
  Gets the WWAN profile configured value.

@parameters
  qcmap_msgr_net_policy_info_v01 *wwan_policy,
  qmi_error_type_v01 *qmi_err_num

@return
  true  - on Success
  false - on Failure

@note

Dependencies
  None

Side Effects
  None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetWWANPolicy(qcmap_msgr_net_policy_info_v01 *wwan_policy,
                                 qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  char val[MAX_STRING_LENGTH]={0};
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);


  if(!QcMapBackhaulWWANMgr)
  {
    GetSetWWANConfigFromXML(CONFIG_TECH,GET_VALUE,val,MAX_STRING_LENGTH);
    wwan_policy->tech_pref = (atoi(val));

    wwan_policy->ip_family = QCMAP_MSGR_IP_FAMILY_V4V6_V01;

    GetSetWWANConfigFromXML(CONFIG_V4_UMTS_PROFILE_INDEX,GET_VALUE,val,MAX_STRING_LENGTH);
    wwan_policy->v4_profile_id_3gpp = (uint8_t)(atoi(val));

    GetSetWWANConfigFromXML(CONFIG_V4_CDMA_PROFILE_INDEX,GET_VALUE,val,MAX_STRING_LENGTH);
    wwan_policy->v4_profile_id_3gpp2 = (uint8_t)(atoi(val));

    GetSetWWANConfigFromXML(CONFIG_V6_UMTS_PROFILE_INDEX,GET_VALUE,val,MAX_STRING_LENGTH);
    wwan_policy->v6_profile_id_3gpp = (uint8_t)(atoi(val));

    GetSetWWANConfigFromXML(CONFIG_V6_CDMA_PROFILE_INDEX,GET_VALUE,val,MAX_STRING_LENGTH);
    wwan_policy->v6_profile_id_3gpp2 =(uint8_t)(atoi(val));
  }

  else
  {

    /* Get WWAN config */
    wwan_policy->tech_pref = QcMapBackhaulWWANMgr->wwan_cfg.tech;
    wwan_policy->ip_family = QCMAP_MSGR_IP_FAMILY_V4V6_V01;
    wwan_policy->v4_profile_id_3gpp = QcMapBackhaulWWANMgr->wwan_cfg.profile_id.v4.umts_profile_index;
    wwan_policy->v4_profile_id_3gpp2 = QcMapBackhaulWWANMgr->wwan_cfg.profile_id.v4.cdma_profile_index;
    wwan_policy->v6_profile_id_3gpp = QcMapBackhaulWWANMgr->wwan_cfg.profile_id.v6.umts_profile_index;
    wwan_policy->v6_profile_id_3gpp2 =QcMapBackhaulWWANMgr->wwan_cfg.profile_id.v6.cdma_profile_index;
   }
  return true;

}
/*===========================================================================
  FUNCTION SetWWANPolicy
==========================================================================*/
/*!
@brief
  Sets the WWAN profile.

@parameters
  qcmap_msgr_net_policy_info_v01 *wwan_policy,
  qmi_error_type_v01 *qmi_err_num

@return
  true  - on Success
  false - on Failure

@note

Dependencies
  None

Side Effects
  None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::SetWWANPolicy(qcmap_msgr_net_policy_info_v01 wwan_policy,
                                               qmi_error_type_v01 *qmi_err_num)
{
  int ret_val = QCMAP_CM_SUCCESS;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  /* Set WWAN config */
  ret_val = qcmap_cm_set_wwan_policy(QcMapMgr->qcmap_cm_handle,
                                       wwan_policy, qmi_err_num);

  if (ret_val != QCMAP_CM_SUCCESS)
  return false;

  return true;

}
/*===========================================================================
  FUNCTION GetDataBitrate(void)
==========================================================================*/
/*!
@brief
  Gets the current channel rate from dsi_netctrl

@parameters
   none

@return
   returns succesful

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetDataBitrate(
                                   qcmap_msgr_data_bitrate_v01 *data_rate,
                                   qmi_error_type_v01 *qmi_err_num)
{
  int ret_val;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_CM_LOG_FUNC_ENTRY();

  /* Get Data Bitrate */
  ret_val = qcmap_cm_get_data_bitrate(QcMapMgr->qcmap_cm_handle,
                                      data_rate,
                                      qmi_err_num);
  if ( ret_val != QCMAP_CM_SUCCESS )
    return false;

  return true;
}

boolean QCMAP_Backhaul_WWAN::AddIPv4SIPServerInfoToList
(
  qcmap_cm_sip_server_info_t sip_entry
)
{

  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  qcmap_sip_server_info_list_t *sip_server_info_list = &(QcMapBackhaulWWANMgr->wwan_cfg.sip_server_info_list);
  qcmap_cm_sip_server_info_t *new_sip_entry = NULL;
  ds_dll_el_t * node = NULL;
  QCMAP_CM_LOG_FUNC_ENTRY();

  if (sip_server_info_list->sipServerEntryListHead == NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
           information. This is done to make use of the doubly linked list framework which
           is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      LOG_MSG_ERROR("AddIPv4SIPServerInfoToList - Error in allocating memory for node",
                    0,0,0);
      return false;
    }
    sip_server_info_list->sipServerEntryListHead = node;
  }

  new_sip_entry = (qcmap_cm_sip_server_info_t*)ds_malloc(sizeof(qcmap_cm_sip_server_info_t));

  if( new_sip_entry == NULL )
  {
    LOG_MSG_ERROR("AddIPv4SIPServerInfoToList - Error in allocating memory for"
                  "new NAT entry",0,0,0);
    return false;
  }

  memset(new_sip_entry, 0, sizeof(qcmap_cm_sip_server_info_t));

  /* save into the config */
  memcpy(new_sip_entry, &sip_entry, sizeof(qcmap_cm_sip_server_info_t));

  //Enque the node
  if ((node = ds_dll_enq(sip_server_info_list->sipServerEntryListHead,
                          NULL, (void*)new_sip_entry )) == NULL)
  {
    LOG_MSG_ERROR("AddIPv4SIPServerInfoToList - Error in adding a node",0,0,0);
    ds_free(new_sip_entry);
    new_sip_entry = NULL;
    return false;
  }

  QcMapBackhaulWWANMgr->wwan_cfg.sip_server_list_count++;
  return TRUE;
}


/*===========================================================================
  FUNCTION  AddWWANIPv4SIPServerInfo
==========================================================================*/
/*!
@brief
- retrieves the network assigned PCSCF address list and PCSCF FQDN list
  from modem
- restarts dnsmasq with the network assigned PCSCF address and PCSCF
  FQDN list

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
boolean QCMAP_Backhaul_WWAN::AddWWANIPv4SIPServerInfo()
{
  dsi_pcscf_addr_info_t pcscf_addr_list;
  dsi_pcscf_fqdn_list_t pcscf_fqdn_list;
  qmi_error_type_v01    qmi_err_num;
  int                   ret_val;
  int                   addr_count;
  int                   fqdn_count;
  struct sockaddr_in    *sin   = NULL;
  qcmap_cm_sip_server_info_t sip_entry;

/*-------------------------------------------------------------------------*/
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);

  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  memset(&pcscf_addr_list, 0, sizeof(dsi_pcscf_addr_info_t));
  memset(&pcscf_fqdn_list, 0, sizeof(dsi_pcscf_fqdn_list_t));

  ret_val = qcmap_cm_get_ipv4_sip_server_info(QcMapMgr->qcmap_cm_handle,
                                              &pcscf_addr_list,
                                              &pcscf_fqdn_list,
                                              &qmi_err_num);
    if (ret_val == QCMAP_CM_SUCCESS)
    {
      LOG_MSG_INFO1("Successfully obtained PCSCF info", 0, 0, 0);

      LOG_MSG_INFO1("Got %d PCSCF addresses", pcscf_addr_list.addr_count, 0, 0);

      for (addr_count = 0; addr_count < pcscf_addr_list.addr_count; addr_count ++)
      {
        if (pcscf_addr_list.pcscf_address[addr_count].valid_addr)
        {
          memset(&sip_entry, 0, sizeof(qcmap_cm_sip_server_info));
          sin = (struct sockaddr_in *)&pcscf_addr_list.pcscf_address[addr_count].addr;
          LOG_MSG_INFO1("pcscf_ip:0x%x", sin->sin_addr.s_addr, 0, 0);

          sip_entry.sip_serv_type = QCMAP_CM_SIP_SERVER_ADDR;
          sip_entry.sip_serv_info.sip_serv_ip = (uint32)sin->sin_addr.s_addr;
          QcMapBackhaulWWANMgr->AddIPv4SIPServerInfoToList(sip_entry);
        }
      }

      LOG_MSG_INFO1("Successfully got %d PCSCF FQDN", pcscf_fqdn_list.fqdn_count, 0, 0);

      for (fqdn_count = 0; fqdn_count < pcscf_fqdn_list.fqdn_count; fqdn_count ++)
      {
        if (pcscf_fqdn_list.pcscf_domain_list[fqdn_count].fqdn_length > 0)
        {
          memset(&sip_entry, 0, sizeof(qcmap_cm_sip_server_info));
          sip_entry.sip_serv_type = QCMAP_CM_SIP_SERVER_FQDN;
          memcpy
            (sip_entry.sip_serv_info.sip_fqdn,
             pcscf_fqdn_list.pcscf_domain_list[fqdn_count].fqdn_string,
             pcscf_fqdn_list.pcscf_domain_list[fqdn_count].fqdn_length);
          QcMapBackhaulWWANMgr->AddIPv4SIPServerInfoToList(sip_entry);
        }
      }
      QCMAPLANMgr->UpdateDHCPDNetworkInfo();
    }
    else
    {
      LOG_MSG_ERROR("Failure in getting PCSCF info %d", qmi_err_num, 0, 0);
      return false;
    }

  return true;
}
/*===========================================================================
  FUNCTION  DeleteWWANIPv4SIPServerInfo
==========================================================================*/
/*!
@brief
- resets the WWAN provided IPv4 SIP server information

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
boolean QCMAP_Backhaul_WWAN::DeleteWWANIPv4SIPServerInfo()
{
  qcmap_sip_server_info_list_t *sip_server_info_list = NULL;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_CM_LOG_FUNC_ENTRY();

  QcMapBackhaulWWANMgr->wwan_cfg.sip_server_list_count = 0;
  sip_server_info_list = &(QcMapBackhaulWWANMgr->wwan_cfg.sip_server_info_list);
  ds_dll_delete_all(sip_server_info_list->sipServerEntryListHead);
  /*making head pointer NULL*/
  sip_server_info_list->sipServerEntryListHead = NULL;
  sip_server_info_list->sipServerEntryListTail = NULL;
  QCMAPLANMgr->UpdateDHCPDNetworkInfo();

  return true;
}
/*===========================================================================
  FUNCTION SetSIPServerInfo
==========================================================================*/
/*!
@brief
  - Sets the default user configured SIP Server Information onto qcmap config.
  - Restarts DHCP server with the user provided SIP server information.

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

boolean QCMAP_Backhaul_WWAN::SetSIPServerInfo
(
   qcmap_msgr_sip_server_info_v01 default_sip_server_info,
   qmi_error_type_v01 *qmi_err_num
)
{
  qcmap_cm_sip_server_info_t default_sip_entry;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);

  if (QcMapBackhaulWWANMgr)
  {
    memset(QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_info.sip_fqdn,0,
           QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01);
    QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_type =
                                   (uint32)default_sip_server_info.pcscf_info_type;

    if (QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_type ==
                                                  QCMAP_CM_SIP_SERVER_ADDR)
    {
      QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_info.sip_serv_ip =
                                      default_sip_server_info.pcscf_ip_addr;
    }
    else if (QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_type ==
                                                  QCMAP_CM_SIP_SERVER_FQDN)
    {
      strlcpy(QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_info.sip_fqdn,
              default_sip_server_info.pcscf_fqdn,
              strlen(default_sip_server_info.pcscf_fqdn)+1);
    }

    LOG_MSG_INFO1("Default SIP server info type %d set",
                QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_type,
                0, 0);
  }

  default_sip_entry.sip_serv_type=(uint32)default_sip_server_info.pcscf_info_type;
  if (default_sip_entry.sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
     default_sip_entry.sip_serv_info.sip_serv_ip = default_sip_server_info.pcscf_ip_addr;
  else
  {
    memset(default_sip_entry.sip_serv_info.sip_fqdn,0,
           QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01);
    strlcpy(default_sip_entry.sip_serv_info.sip_fqdn,
            default_sip_server_info.pcscf_fqdn,
            QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01);
  }

  QCMAP_Backhaul_WWAN::GetSetDefaultSIPServerConfigFromXML(
                                    SET_VALUE, &default_sip_entry);


  QCMAPLANMgr->UpdateDHCPDNetworkInfo();

  return true;
}

/*===========================================================================
  FUNCTION GetSIPServerInfo
==========================================================================*/
/*!
@brief
  - Retrives default user configured SIP server Information
  - Also retrieves network assigned SIP server information list.

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

boolean QCMAP_Backhaul_WWAN::GetSIPServerInfo
(
   qcmap_msgr_sip_server_info_v01 *default_sip_info,
   qcmap_msgr_sip_server_info_v01 *network_sip_info,
   int *count_network_sip_info,
   qmi_error_type_v01 *qmi_err_num
)
{

  int cnt;
  qcmap_sip_server_info_list_t *sip_server_info_list = NULL;
  qcmap_cm_sip_server_info_t *sip_entry = NULL;
  qcmap_cm_sip_server_info_t default_sip_entry;
  ds_dll_el_t * node = NULL;
  in_addr addr;
  char data[MAX_STRING_LENGTH] = {0};

  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
/*-------------------------------------------------------------------------*/
  if(QcMapBackhaulWWANMgr)
  {
    if (QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_type !=
        QCMAP_CM_SIP_SERVER_MIN)
    {
      default_sip_info->pcscf_info_type =
        (uint32)QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_type;

      if (QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_type ==
                                                      QCMAP_CM_SIP_SERVER_ADDR)
      {
        default_sip_info->pcscf_ip_addr =
          QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_info.sip_serv_ip;
      }
      else if (QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_type ==
                                                      QCMAP_CM_SIP_SERVER_FQDN)
      {
        strlcpy(default_sip_info->pcscf_fqdn,
                QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_info.sip_fqdn,
                strlen(QcMapBackhaulWWANMgr->wwan_cfg.default_sip_server_info.sip_serv_info.sip_fqdn)+1);
      }

      LOG_MSG_INFO1("Default SIP server info type %d retrieved",
                    default_sip_info->pcscf_info_type, 0, 0);
    }
    else
    {
      LOG_MSG_INFO1("Default SIP server info not configured", 0, 0, 0);
    }
    sip_server_info_list = &(QcMapBackhaulWWANMgr->wwan_cfg.sip_server_info_list);

    if (QcMapBackhaulWWANMgr->wwan_cfg.sip_server_list_count > 0)
    {
      LOG_MSG_INFO1(" Sip server count: %d ", QcMapBackhaulWWANMgr->wwan_cfg.sip_server_list_count, 0, 0);
      LOG_MSG_INFO1("%d network assigned SIP server info present",
                     QcMapBackhaulWWANMgr->wwan_cfg.sip_server_list_count, 0, 0);
      *count_network_sip_info = QcMapBackhaulWWANMgr->wwan_cfg.sip_server_list_count;

      node = sip_server_info_list->sipServerEntryListHead;
      node = ds_dll_next (node, (const void**)(&sip_entry));

      for (cnt=0; cnt<QcMapBackhaulWWANMgr->wwan_cfg.sip_server_list_count && sip_entry; cnt++)
      {
        network_sip_info[cnt].pcscf_info_type =
          (uint32)sip_entry->sip_serv_type;
        if (network_sip_info[cnt].pcscf_info_type == QCMAP_CM_SIP_SERVER_ADDR)
        {
          network_sip_info[cnt].pcscf_ip_addr =
            sip_entry->sip_serv_info.sip_serv_ip;
        }
        else if (network_sip_info[cnt].pcscf_info_type == QCMAP_CM_SIP_SERVER_FQDN)
        {
          strlcpy(network_sip_info[cnt].pcscf_fqdn,
                  sip_entry->sip_serv_info.sip_fqdn,
                  strlen(sip_entry->sip_serv_info.sip_fqdn)+1);
        }
        node = ds_dll_next (node, (const void**)(&sip_entry));
      }
    }
    else
    {
       LOG_MSG_INFO1("Network SIP server info not available", 0, 0, 0);
    }
  }
  else
  {
    QCMAP_Backhaul_WWAN::GetSetDefaultSIPServerConfigFromXML(
                                      GET_VALUE, &default_sip_entry);
	if (default_sip_entry.sip_serv_type != QCMAP_CM_SIP_SERVER_MIN)
        {
          default_sip_info->pcscf_info_type = (uint32)default_sip_entry.sip_serv_type;
       }
    if(default_sip_entry.sip_serv_type == QCMAP_CM_SIP_SERVER_ADDR)
     default_sip_info->pcscf_ip_addr = default_sip_entry.sip_serv_info.sip_serv_ip;
    else
    {
      strlcpy(default_sip_info->pcscf_fqdn,
              default_sip_entry.sip_serv_info.sip_fqdn,
              QCMAP_MSGR_PCSCF_FQDN_MAX_LENGTH_V01);
    }
  }
  return true;
}
/*===========================================================================
  FUNCTION GetV6SIPServerInfo
==========================================================================*/
/*!
@brief
  -retrieves network assigned SIP server information list.

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

boolean QCMAP_Backhaul_WWAN::GetV6SIPServerInfo
(
   qcmap_msgr_ipv6_sip_server_info_v01 *network_v6_sip_info,
   int *count_network_v6_sip_info,
   qmi_error_type_v01 *qmi_err_num
)
{
  dsi_pcscf_addr_info_t pcscf_addr_list;
  dsi_pcscf_fqdn_list_t pcscf_fqdn_list;
  int                   addr_count, count = 0;
  int                   ret_val = QCMAP_CM_SUCCESS;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);


  if ( qmi_err_num == NULL )
  {
    LOG_MSG_ERROR("NULL QMI error pointer.\n",0,0,0);
    return false;
  }
  if ( network_v6_sip_info == NULL || count_network_v6_sip_info == NULL )
  {
    LOG_MSG_ERROR("NULL params.\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }

  memset(&pcscf_addr_list, 0, sizeof(dsi_pcscf_addr_info_t));
  memset(&pcscf_fqdn_list, 0, sizeof(dsi_pcscf_fqdn_list_t));
  *count_network_v6_sip_info = 0;

/*-------------------------------------------------------------------------*/

  QCMAP_CM_LOG_FUNC_ENTRY();

  ret_val = qcmap_cm_get_ipv6_sip_server_info(QcMapMgr->qcmap_cm_handle,
                                              &pcscf_addr_list,
                                              &pcscf_fqdn_list,
                                              qmi_err_num);

  LOG_MSG_INFO1("qcmap_cm_get_ipv6_sip_server_info: pcscf_addr, rval:%d",
                 ret_val, 0, 0);
  if(ret_val != QCMAP_CM_SUCCESS)
  {
    LOG_MSG_ERROR("Couldn't get pcscf ipv6  address. rval %d",
                  ret_val, 0, 0);
    return false;
  }
  else
  {
    LOG_MSG_INFO1("Got %dpcscf %dFQDN IPv6 Address, rval:%d", pcscf_addr_list.addr_count,
                   pcscf_fqdn_list.fqdn_count, ret_val);
  }

  // store valid IPV6 pcscf address recieved
  for (addr_count = 0; addr_count < pcscf_addr_list.addr_count; addr_count ++)
  {
    if (pcscf_addr_list.pcscf_address[addr_count].valid_addr)
    {
      if (SASTORAGE_FAMILY(pcscf_addr_list.pcscf_address[addr_count].addr) == AF_INET6)
      {
        memcpy(network_v6_sip_info[count].pcscf_ipv6_addr ,
               SASTORAGE_DATA(pcscf_addr_list.pcscf_address[addr_count].addr),
               QCMAP_MSGR_IPV6_ADDR_LEN_V01*sizeof(uint8));
        network_v6_sip_info[count].pcscf_info_type =
                   (qcmap_msgr_pcscf_info_type_v01)QCMAP_CM_SIP_SERVER_ADDR;
        count++;
      }
      else
      {
        LOG_MSG_INFO1("Network IPV6 SIP server info not available", 0, 0, 0);
      }
    }
    else
    {
      LOG_MSG_INFO1("Network IPV6 SIP server %d info not valid", addr_count, 0, 0);
    }
  }

  // store valid pcscf fqdn list rcieved
  for (addr_count = 0; addr_count < pcscf_fqdn_list.fqdn_count; addr_count ++)
  {
    if (pcscf_fqdn_list.pcscf_domain_list[addr_count].fqdn_length > 0)
    {
      memcpy(network_v6_sip_info[count].pcscf_fqdn,
             pcscf_fqdn_list.pcscf_domain_list[addr_count].fqdn_string,
             pcscf_fqdn_list.pcscf_domain_list[addr_count].fqdn_length);
      network_v6_sip_info[count].pcscf_info_type =
               (qcmap_msgr_pcscf_info_type_v01)QCMAP_CM_SIP_SERVER_FQDN;
      count++;
    }
    else
    {
      LOG_MSG_INFO1("Network %d IPV6 SIP server FQDN not available", addr_count, 0, 0);
    }
  }
  *count_network_v6_sip_info = count;
  LOG_MSG_INFO1("Network %d IPV6 SIP server  available", count, 0, 0);
  return true;
}

boolean QCMAP_Backhaul_WWAN::DeleteDelegatedPrefix(void *mac_addr, qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  uint8 mac_addr_hex[QCMAP_MSGR_MAC_ADDR_LEN_V01];
  int num_entries, i;
  qcmap_msgr_connected_device_info_v01 connected_devices[QCMAP_MSGR_MAX_CONNECTED_DEVICES_V01];

  if (QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->prefix_delegation_activated)
  {
    //Convert the MAC from char to hex
    if(!ds_mac_addr_pton((char*)mac_addr, mac_addr_hex))
    {
      LOG_MSG_ERROR("DeleteClientAddrInfo - Error in MAC address conversion",
                    0,0,0);
      return false;
    }
    mac_addr = (void*)mac_addr_hex;

    if(QcMapMgr->GetConnectedDevicesInfo(connected_devices,
                                          &num_entries, qmi_err_num))
    {
      for (i =0; i < num_entries; i++)
      {
        if (memcmp(connected_devices[i].client_mac_addr, mac_addr, sizeof(mac_addr)) == 0)
          break;
      }
      if (!qcmap_cm_delete_ipv6_delegated_prefix(true,
                                                 connected_devices[i].ipv6_addr,
                                                 qmi_err_num))
      {
        LOG_MSG_ERROR("Error: Unable flush prefix's %d", *qmi_err_num, 0, 0);
      }
    }
    else{
      LOG_MSG_ERROR("Error: Unable get connectedDevices %d", *qmi_err_num, 0, 0);

      return false;
    }
  }

  return true;
}

/*===========================================================================
  FUNCTION GetPrefixDelegationConfig
==========================================================================*/
/*!
@brief
  Retreives the Prefix Delegation config.

@parameters
  char *process

@return
  true  - Prefix Delegation config set
  flase - Prefix Delegation config not set

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetPrefixDelegationConfig
(
  boolean *pd_mode,
  qmi_error_type_v01 *qmi_err_num
)
{
  char enable[MAX_STRING_LENGTH] = {0};

  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  if ( qmi_err_num == NULL )
  {
    LOG_MSG_ERROR("NULL QMI error pointer.\n",0,0,0);
    return false;
  }

  if (pd_mode == NULL)
  {
    LOG_MSG_ERROR("NULL Args.\n", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  if(QcMapBackhaulWWANMgr)
  *pd_mode = QcMapBackhaulWWANMgr->wwan_cfg.prefix_delegation;
  else
  {

  GetSetWWANConfigFromXML(CONFIG_PREFIX_DELEGATION,GET_VALUE,enable, MAX_STRING_LENGTH);
  *pd_mode=(boolean)(atoi(enable));

}
  return true;

}
/*===========================================================================
  FUNCTION SetPrefixDelegationConfig
==========================================================================*/
/*!
@brief
  Enable/disable the Prefix Delegation config.

@parameters
  char *process

@return
  true  - Prefix Delegation config set
  flase - Prefix Delegation config not set

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::SetPrefixDelegationConfig
(
  boolean pd_mode,
  qmi_error_type_v01 *qmi_err_num
)
{
  char val[MAX_STRING_LENGTH] = {0};

  snprintf(val, sizeof(val), "%d", pd_mode);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  if (qmi_err_num == NULL)
  {
    LOG_MSG_ERROR("NULL Arg.\n", 0,0,0);
    return false;
  }
  if(QcMapBackhaulWWANMgr)
  {
    if (pd_mode == QcMapBackhaulWWANMgr->wwan_cfg.prefix_delegation)
    {
      LOG_MSG_ERROR("Prefix delegation config requested is already set.\n", 0,0,0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return false;
    }

    QcMapBackhaulWWANMgr->wwan_cfg.prefix_delegation = pd_mode;
  }
  GetSetWWANConfigFromXML(CONFIG_PREFIX_DELEGATION,SET_VALUE,val,MAX_STRING_LENGTH);

  if ( qcmap_cm_get_ipv6_state(qmi_err_num) == QCMAP_CM_V6_WAN_CONNECTED )
  {
    LOG_MSG_ERROR("Prefix delegation config set, but v6 call is active. Will "
                  "take effect on next Call\n",0,0,0);
    *qmi_err_num = QMI_ERR_DEVICE_IN_USE_V01;
    return false;
  }
}

/*===========================================================================
  FUNCTION GetPrefixDelegationStatus
  ==========================================================================*/
  /*!
  @brief
   Retreives the current Prefix Delegation mode.

  @parameters
  char *process

  @return
  true  - Prefix Delegation mode set
  flase - Prefix Delegation mode not set

  @note
  - Dependencies
  - None

  - Side Effects
  - None
  */
  /*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetPrefixDelegationStatus
(
  boolean *pd_mode,
  qmi_error_type_v01 *qmi_err_num
  )
{
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  if ( qmi_err_num == NULL )
  {
    LOG_MSG_ERROR("NULL QMI error pointer.\n",0,0,0);
    return false;
  }

  if (pd_mode == NULL)
  {
    LOG_MSG_ERROR("NULL Args.\n", 0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  *pd_mode = false;

  /* If WAN v6 call is disconnected or STA/Cradle is active, we return true,
  but pd_mode is always false */
  if ( qcmap_cm_get_ipv6_state(qmi_err_num) != QCMAP_CM_V6_WAN_CONNECTED )
  {
    LOG_MSG_ERROR("Prefix delegation can only be enabled when a v6 call is active\n", 0,0,0);
    return true;
  }

  /* We only have control for Prefix Delegation over WAN */
  if (QCMAP_Backhaul::IsNonWWANBackhaulActive())
  {
    LOG_MSG_ERROR("STA/Cradle connected, Prefix Delegation query invalid\n", 0,0,0);
    return true;
  }

  *pd_mode = QcMapBackhaulWWANMgr->prefix_delegation_activated;
  return true;
  }

boolean QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML
(
  qcmap_action_type action,
  qcmap_dynamic_dns_t *ddns_config
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  struct in_addr addr;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(DDNSCFG_TAG);

  if (action == SET_VALUE)
  {
    snprintf(data, sizeof(data), "%d", ddns_config->enable);
    root.child(DDNS_Enable_TAG).text() = data;

    root.child(DDNS_Server_TAG).text() = ddns_config->server_info.server_url;
    root.child(DDNS_Protocol_TAG).text() = ddns_config->server_info.protocol;
    root.child(DDNS_Login_TAG).text() = ddns_config->login;
    root.child(DDNS_Password_TAG).text() = ddns_config->password;
    root.child(DDNS_Hostname_TAG).text() = ddns_config->hostname;

    snprintf(data, sizeof(data), "%d", ddns_config->timeout);
    root.child(DDNS_Timeout_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
  {
    ddns_config->enable = atoi(root.child(DDNS_Enable_TAG).child_value());

    strlcpy(ddns_config->server_info.server_url,
            root.child(DDNS_Server_TAG).child_value(), QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);

    strlcpy(ddns_config->server_info.protocol,
            root.child(DDNS_Protocol_TAG).child_value(), QCMAP_DDNS_PROTOCOL_LEN);

    strlcpy(ddns_config->login,
            root.child(DDNS_Login_TAG).child_value(), QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01);

    strlcpy(ddns_config->password,
            root.child(DDNS_Password_TAG).child_value(), QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01);

    strlcpy(ddns_config->hostname,
          root.child(DDNS_Hostname_TAG).child_value(), QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);

    ddns_config->timeout = atoi(root.child(DDNS_Timeout_TAG).child_value());
  }
  LOG_MSG_INFO1("DDNS Config: Enable: %d ", ddns_config->enable, 0, 0);
  LOG_MSG_INFO1("Server: %s Protocol: %s Login: %s",
                ddns_config->server_info.server_url,
                ddns_config->server_info.protocol, ddns_config->login);

  LOG_MSG_INFO1("Password: %s Hostname: %s Timeout: %d",
                ddns_config->password,
                ddns_config->hostname, ddns_config->timeout);
}


/*===========================================================================
  FUNCTION DeleteDDNS
==========================================================================*/
/*!
@brief
  The function will delete the dyndns configuration values

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::DeleteDDNS()
{
  char command[MAX_COMMAND_STR_LEN];

  LOG_MSG_INFO1("Deleting old configuration", 0, 0, 0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i s/server=.*/server=/g %s",QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i s/protocol=.*/protocol=/g %s",QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
  snprintf(command,
      MAX_COMMAND_STR_LEN,
      "sed -i s/daemon=.*/daemon=/g %s",QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
  snprintf(command,
      MAX_COMMAND_STR_LEN,
      "sed -i s/login=.*/login=/g %s",QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i s/password=.*/password=/g %s",QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i s/use=if, if=.*/use=if, if=/g %s",QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
   snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i s/usev6=if, if=.*/usev6=if, if=/g %s",QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));

  if(strlen(this->wwan_cfg.ddns_conf.hostname)!=0)
  {
    snprintf(command,
        MAX_COMMAND_STR_LEN,
        "sed -i '/%s/d' %s",this->wwan_cfg.ddns_conf.hostname,QCMAP_DDNS_CFG_PATH);
    ds_system_call(command, strlen(command));
  }

  return true ;
}


/*===========================================================================
  FUNCTION EnableDDNS
==========================================================================*/
/*!
@brief
  Enable Dynamic dns.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::StartDDNS(qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  LOG_MSG_INFO1("Enable DDNS ",0,0,0);

  if((QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE) ||
     (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER))
  {
    LOG_MSG_ERROR("STA or Cradle backhaul is connected \n",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

 /* supported of IPV4 only */
  if( qcmap_cm_get_state(qmi_err_num) != QCMAP_CM_WAN_CONNECTED &&
      qcmap_cm_get_ipv6_state(qmi_err_num) != QCMAP_CM_V6_WAN_CONNECTED)//hence assuming wwan object to be available beyond this point and using directly
  {
    LOG_MSG_ERROR("QCMAP backhaul not connected \n",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  /*only stop DDNS*/
  QcMapBackhaulWWANMgr->StopDDNS();

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i s/use=if, if=.*/use=if, if=/g %s",QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
   snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i s/usev6=if, if=.*/usev6=if, if=/g %s",QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "rm %s",QCMAP_DDNS_CACHE_PATH);
  ds_system_call(command, strlen(command));

  if ( qcmap_cm_get_dev_name(QcMapMgr->qcmap_cm_handle,
                               QCMAP_MSGR_IP_FAMILY_V4_V01,
                               devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS )
  {
      LOG_MSG_ERROR("Couldn't get ipv4 rmnet name. error %d\n", qcmap_cm_error,0,0);
  }
  else
  {
    snprintf(command,
        MAX_COMMAND_STR_LEN,
        "sed -i \"s/use=if, if=.*/use=if, if=%s/g\" %s",devname,QCMAP_DDNS_CFG_PATH);
    ds_system_call(command, strlen(command));
    LOG_MSG_INFO1(" Interface used for ipv4 ddclient: %s ",devname,0,0);
  }

 if ( qcmap_cm_get_dev_name(QcMapMgr->qcmap_cm_handle,
                               QCMAP_MSGR_IP_FAMILY_V6_V01,
                               devname, &qcmap_cm_error) != QCMAP_CM_SUCCESS )
  {
      LOG_MSG_ERROR("Couldn't get ipv6 rmnet name. error %d\n", qcmap_cm_error,0,0);
  }
  else
  {
    snprintf(command,
        MAX_COMMAND_STR_LEN,
        "sed -i \"s/usev6=if, if=.*/usev6=if, if=%s/g\" %s",devname,QCMAP_DDNS_CFG_PATH);
    ds_system_call(command, strlen(command));
    LOG_MSG_INFO1(" Interface used for ipv6 ddclient: %s ",devname,0,0);
  }

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "ddclient -file %s &",QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));

  return true;
}

/*===========================================================================
  FUNCTION SetDDNS
==========================================================================*/
/*!
@brief
  Set dynamic dns.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::SetDDNS(qcmap_msgr_set_dynamic_dns_config_req_msg_v01 setddns,qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];
  qcmap_dynamic_dns_t ddns_conf, *ddns_ptr = NULL;

  if(QcMapBackhaulWWANMgr)
  {
    QcMapBackhaulWWANMgr->DeleteDDNS();
    if( setddns.timeout_valid)
      QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.timeout = setddns.timeout;
    bzero(&(QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.hostname),QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);
    strlcpy(QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.hostname,setddns.hostname,QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);
    if(!strncmp(QcMapBackhaulWWANMgr->wwan_cfg.ddns_server_info.server_url,setddns.ddns_server,
                strlen(QcMapBackhaulWWANMgr->wwan_cfg.ddns_server_info.server_url)))
    {
      strlcpy(QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.server_info.server_url,
              setddns.ddns_server, QCMAP_MSGR_DDNS_URL_LENGTH_V01);

      strlcpy(QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.server_info.protocol,
              QcMapBackhaulWWANMgr->wwan_cfg.ddns_server_info.protocol,
              QCMAP_DDNS_PROTOCOL_LEN);
    }
    else
    {
      LOG_MSG_ERROR("Server URL not supported : %s\n", setddns.ddns_server, 0, 0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
    strlcpy(QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.login,setddns.login,QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01);
    strlcpy(QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.password,setddns.password,QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01);
    ddns_ptr = &(QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf);
  }
  else
  {
    memset(&ddns_conf, 0, sizeof(ddns_conf));
    QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML(GET_VALUE, &ddns_conf);
    if( setddns.timeout_valid)
      ddns_conf.timeout = setddns.timeout;

    bzero(&(ddns_conf.hostname),QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);
    strlcpy(ddns_conf.hostname,setddns.hostname,QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);

    if(!strncmp(QCMAP_DYNDNS_SERVER_URL, setddns.ddns_server,
                QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01))
    {
      bzero(&(ddns_conf.server_info),sizeof(qcmap_ddns_server_info_t));
      strlcpy(ddns_conf.server_info.server_url,setddns.ddns_server,QCMAP_MSGR_DDNS_URL_LENGTH_V01);
      strlcpy(ddns_conf.server_info.protocol, QCMAP_DYNDNS_PROTOCOL,QCMAP_DDNS_PROTOCOL_LEN);
    }
    else
    {
      LOG_MSG_ERROR("Server URL not supported : %s\n", setddns.ddns_server, 0, 0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }

    bzero(&(ddns_conf.login),QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01);
    strlcpy(ddns_conf.login,setddns.login,QCMAP_MSGR_DDNS_LOGIN_LENGTH_V01);
    bzero(&(ddns_conf.password),QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01);
    strlcpy(ddns_conf.password,setddns.password,QCMAP_MSGR_DDNS_PASSWORD_LENGTH_V01);
    ddns_ptr = &ddns_conf;
  }

  LOG_MSG_INFO1("A server URL =%s ",ddns_ptr->server_info.server_url,0,0);
  LOG_MSG_INFO1("A login      =%s ",ddns_ptr->login,0,0);
  LOG_MSG_INFO1("A password   =%s ",ddns_ptr->password,0,0);
  LOG_MSG_INFO1("A hostname   =%s ",ddns_ptr->hostname,0,0);
  LOG_MSG_INFO1("A timeout    =%d ",ddns_ptr->timeout,0,0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i s/login=.*/login=%s/g %s",ddns_ptr->login,QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
  LOG_MSG_INFO1(" LOGIN : %s ",ddns_ptr->login,0,0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i \"s/password=.*/password=%s/g\" %s",ddns_ptr->password,QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
  LOG_MSG_INFO1(" PASSWORD : %s ",ddns_ptr->password,0,0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i s/server=.*/server=%s/g %s",ddns_ptr->server_info.server_url,QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
  LOG_MSG_INFO1(" SERVER URL : %s ",ddns_ptr->server_info.server_url,0,0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i s/protocol=.*/protocol=%s/g %s",ddns_ptr->server_info.protocol,QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
  LOG_MSG_INFO1(" PROTOCOL : %s ",ddns_ptr->server_info.protocol,0,0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i s/\"# your-dynamic-host.dyndns.org.*\"/\"# your-dynamic-host.dyndns.org \\n %s\"/g %s",ddns_ptr->hostname,QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
  LOG_MSG_INFO1(" Enable HOSTNAME : %s ",ddns_ptr->hostname,0,0);

  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "sed -i s/daemon=.*/daemon=%d/g %s",ddns_ptr->timeout,QCMAP_DDNS_CFG_PATH);
  ds_system_call(command, strlen(command));
  LOG_MSG_INFO1(" timeout : %d ",ddns_ptr->timeout,0,0);

  QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML(SET_VALUE, ddns_ptr);
  /*restart ddns*/
  if( !QCMAP_Backhaul::IsNonWWANBackhaulActive())
  {
    if ( qcmap_cm_get_ipv6_state(qmi_err_num) != QCMAP_CM_V6_WAN_CONNECTED
         && qcmap_cm_get_state(qmi_err_num) != QCMAP_CM_WAN_CONNECTED )
    {
      LOG_MSG_INFO1("none of backhaul is enable", 0, 0, 0);
    }
    else if(QcMapBackhaulWWANMgr && QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.enable\
            == DDNS_ENABLED)
    {
      QcMapBackhaulWWANMgr->StartDDNS(qmi_err_num);
    }
  }

  return true ;
}


/*===========================================================================
  FUNCTION GetDDNS
==========================================================================*/
/*!
@brief
  Get dynamic dns.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_Backhaul_WWAN::GetDDNS(qcmap_msgr_get_dynamic_dns_config_resp_msg_v01 *getddns,qmi_error_type_v01 *qmi_err_num)
{
  int len=0;
  qcmap_dynamic_dns_t ddns_conf;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  if(QcMapBackhaulWWANMgr)
  {
    len = strlen(QcMapBackhaulWWANMgr->wwan_cfg.ddns_server_info.server_url);
    if(len == 0)
    {
      LOG_MSG_ERROR("invalid argument\n", 0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
    LOG_MSG_INFO1(" GetDDNS  ",0,0,0);
    LOG_MSG_INFO1(" Hostname =%s ",QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.hostname,0,0);
    LOG_MSG_INFO1(" server URL =%s ",QcMapBackhaulWWANMgr->wwan_cfg.ddns_server_info.server_url,0,0);
    LOG_MSG_INFO1(" timeout =%d ",QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.timeout,0,0);
    getddns->timeout = QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.timeout;
    getddns->timeout_valid = true;
    strlcpy(getddns->hostname,QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.hostname,QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);
    getddns->hostname_valid = true;
    strlcpy( getddns->ddns_config[0].server_url,
             QcMapBackhaulWWANMgr->wwan_cfg.ddns_server_info.server_url,
             QCMAP_MSGR_DDNS_URL_LENGTH_V01);
    /* Currently we are supportying only one Server */
    getddns->ddns_config_len = 1;
    if( QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.enable == DDNS_ENABLED)
      getddns->enable = TRUE;
    else
      getddns->enable = false;
  }
  else
  {
    memset(&ddns_conf, 0, sizeof(ddns_conf));
    QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML(GET_VALUE, &ddns_conf);
    len = strlen(ddns_conf.server_info.server_url);
    if(len == 0)
    {
      LOG_MSG_ERROR("invalid argument\n", 0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
    getddns->timeout = ddns_conf.timeout;
    getddns->timeout_valid = true;
    strlcpy(getddns->hostname, ddns_conf.hostname,QCMAP_MSGR_DDNS_HOSTNAME_LENGTH_V01);
    getddns->hostname_valid = true;
    strlcpy( getddns->ddns_config[0].server_url,
             ddns_conf.server_info.server_url,
             QCMAP_MSGR_DDNS_URL_LENGTH_V01);
    /* Currently we are supportying only one Server */
    getddns->ddns_config_len = 1;
    if(ddns_conf.enable == DDNS_ENABLED)
      getddns->enable = TRUE;
    else
      getddns->enable = false;
  }
  return true ;
}

/*===========================================================================
  FUNCTION EnableDDNS
==========================================================================*/
/*!
@brief
  Enable dynamic dns.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Backhaul_WWAN::EnableDDNS(qmi_error_type_v01 *qmi_err_num)
{
  char data[MAX_STRING_LENGTH] = {0};
  pugi::xml_document xml_file;
  pugi::xml_node root;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  if(QcMapBackhaulWWANMgr)
  {
    QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.enable = DDNS_ENABLED;
    QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML(SET_VALUE, &QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf);
    QcMapBackhaulWWANMgr->StartDDNS(qmi_err_num);
  }
  else
  {
    /* Update XML file directly. */
    if (!xml_file.load_file(QcMapMgr->xml_path))
    {
      LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(DDNSCFG_TAG);

    snprintf(data, sizeof(data), "%d", DDNS_ENABLED);
    root.child(DDNS_Enable_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  return true ;
}

/*===========================================================================
  FUNCTION DisableDDNS
==========================================================================*/
/*!
@brief
  Disable dynamic dns.

@parameters

@return

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_Backhaul_WWAN::DisableDDNS(qmi_error_type_v01 *qmi_err_num)
{
  char data[MAX_STRING_LENGTH] = {0};
  pugi::xml_document xml_file;
  pugi::xml_node root;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  if(QcMapBackhaulWWANMgr)
  {
    QcMapBackhaulWWANMgr->StopDDNS();
    QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf.enable = DDNS_DISABLED;
    QCMAP_Backhaul_WWAN::GetSetDDNSConfigFromXML(SET_VALUE, &QcMapBackhaulWWANMgr->wwan_cfg.ddns_conf);
  }
  else
  {
    /* Update XML file directly. */
    if (!xml_file.load_file(QcMapMgr->xml_path))
    {
      LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(DDNSCFG_TAG);

    snprintf(data, sizeof(data), "%d", DDNS_DISABLED);
    root.child(DDNS_Enable_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  return true ;
}

boolean QCMAP_Backhaul_WWAN::GetTinyProxyStatus
                             (qcmap_msgr_tiny_proxy_mode_enum_v01 *status,
                              qmi_error_type_v01 *qmi_err_num)
{
   QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
   QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

   pugi::xml_document xml_file;
   pugi::xml_node root;
   if (QcMapBackhaulMgr)
   {
    *status = QcMapBackhaulMgr->tiny_proxy_enable_state;
   }
   else
   {
     LOG_MSG_ERROR("Backhaul is not enable.",0,0,0);
     /* read XML file directly. */
     if (!xml_file.load_file(QcMapMgr->xml_path))
     {
       LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
       *qmi_err_num = QMI_ERR_INTERNAL_V01;
       return false;
     }
     root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(TINYPROXYCFG_TAG);
     atoi(root.child(TINY_PROXY_Enable_TAG).child_value())? (*status =
          QCMAP_MSGR_TINY_PROXY_MODE_UP_V01) :
          (*status = QCMAP_MSGR_TINY_PROXY_MODE_DOWN_V01);
   }

  return true;
}

boolean QCMAP_Backhaul_WWAN::EnableTinyProxy(qmi_error_type_v01 *qmi_err_num)
{
  char data[MAX_STRING_LENGTH] = {0};
  pugi::xml_document xml_file;
  pugi::xml_node root;
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  QCMAP_CM_LOG_FUNC_ENTRY();

  if(QcMapBackhaulMgr &&((QcMapBackhaulWWANMgr && (qcmap_cm_get_state(qmi_err_num) ==
      QCMAP_CM_WAN_CONNECTED)) || (QCMAP_Backhaul::IsNonWWANBackhaulActive())))

  {
    QcMapBackhaulMgr->tiny_proxy_enable_state= QCMAP_MSGR_TINY_PROXY_MODE_UP_V01;
    QCMAP_Backhaul_WWAN::GetSetTinyProxyFromXML(SET_VALUE,
                                                &QcMapBackhaulMgr->tiny_proxy_enable_state);

    /*stoping tinyproxy*/
    QCMAP_Backhaul_WWAN::StopTinyProxy(qmi_err_num);
    /*end of stoping tinyproxy*/

    QCMAP_Backhaul_WWAN::SetupTinyProxy(qmi_err_num);
    snprintf(command, MAX_COMMAND_STR_LEN,
             "tinyproxy -c /etc/tinyproxy.conf");
    ds_system_call(command, strlen(command));

  }
  else
  {
    LOG_MSG_ERROR("Backhaul or cradle or station mode required.",0,0,0);
    /* Update XML file directly. */
    if (!xml_file.load_file(QcMapMgr->xml_path))
    {
      LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }

    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(TINYPROXYCFG_TAG);
    snprintf(data, MAX_COMMAND_STR_LEN, "%d", true);
    root.child(TINY_PROXY_Enable_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  }
  return true;

}

boolean QCMAP_Backhaul_WWAN::DisableTinyProxy(qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN];
  char data[MAX_STRING_LENGTH] = {0};
  pugi::xml_document xml_file;
  pugi::xml_node root;
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);

  if(QcMapBackhaulMgr && ((QcMapBackhaulWWANMgr && (qcmap_cm_get_state(qmi_err_num) ==
     QCMAP_CM_WAN_CONNECTED)) || (QCMAP_Backhaul::IsNonWWANBackhaulActive())))
  {
    QcMapBackhaulMgr->tiny_proxy_enable_state= QCMAP_MSGR_TINY_PROXY_MODE_DOWN_V01;
    QCMAP_Backhaul_WWAN::GetSetTinyProxyFromXML(SET_VALUE,
                                                &QcMapBackhaulMgr->tiny_proxy_enable_state);
    /*stoping tinyproxy*/
    QCMAP_Backhaul_WWAN::StopTinyProxy(qmi_err_num);
    /*end of stoping tinyproxy*/
  }
  else
  {
    LOG_MSG_ERROR("Backhaul or cradle or station mode required.",0,0,0);
    /* Update XML file directly. */
    if (!xml_file.load_file(QcMapMgr->xml_path))
    {
      LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
      *qmi_err_num = QMI_ERR_INTERNAL_V01;
      return false;
    }
    root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(TINYPROXYCFG_TAG);
    snprintf(data, MAX_COMMAND_STR_LEN, "%d", false);
    root.child(TINY_PROXY_Enable_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);

  }
  return true;
}

boolean QCMAP_Backhaul_WWAN::StopTinyProxy(qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN];
  char data[MAX_STRING_LENGTH] = {0};
  struct ifreq        req_if_info;
  int val;
  struct sockaddr     *saddr;
  int   sockfd;

  memset(&req_if_info, 0, sizeof(req_if_info));
  strlcpy(req_if_info.ifr_name, BRIDGE_IFACE,sizeof(BRIDGE_IFACE));
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  val = ioctl(sockfd, SIOCGIFADDR, &req_if_info);
  if(val < 0)
  {
    LOG_MSG_ERROR("Get bridge iface address failed",0,0,0);
    return false;
  }
  saddr = (struct sockaddr *)&(req_if_info.ifr_addr);

  /*removing  user nobody from	inet group & removing inet group*/
  snprintf(command, MAX_COMMAND_STR_LEN,
           "deluser nobody inet");
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "groupdel inet");
  ds_system_call(command, strlen(command));
  /*end of removing  user nobody from  inet group & removing inet group*/

  snprintf(command, MAX_COMMAND_STR_LEN,
            "rm -rf /var/log/tinyproxy");
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
             "rm -rf /var/run/tinyproxy");
  ds_system_call(command, strlen(command));

  /*clearing NAT rule*/
  snprintf(command, MAX_COMMAND_STR_LEN,
            "iptables -t nat -D PREROUTING -i bridge0 ! -d %s"
            " -p tcp --dport 80 -j REDIRECT --to-ports 8888",
            inet_ntoa(((struct sockaddr_in *)saddr)->sin_addr));
  ds_system_call(command, strlen(command));


  snprintf(command,MAX_COMMAND_STR_LEN, " killall -9 tinyproxy");
  ds_system_call(command, strlen(command));
  return true;
}

boolean QCMAP_Backhaul_WWAN::GetSetTinyProxyFromXML
(
  qcmap_action_type action,
  qcmap_msgr_tiny_proxy_mode_enum_v01 *proxy_config
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  struct in_addr addr;
  char data[MAX_STRING_LENGTH] = {0};
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  boolean status;
  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(TINYPROXYCFG_TAG);

  if (action == SET_VALUE)
  {
    if (*proxy_config == QCMAP_MSGR_TINY_PROXY_MODE_UP_V01)
      status = true;
    else
      status = false;
    snprintf(data, MAX_COMMAND_STR_LEN, "%d", status);
    root.child(TINY_PROXY_Enable_TAG).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
  {
    atoi(root.child(TINY_PROXY_Enable_TAG).child_value())? (*proxy_config =
         QCMAP_MSGR_TINY_PROXY_MODE_UP_V01) :
         (*proxy_config = QCMAP_MSGR_TINY_PROXY_MODE_DOWN_V01);
  }
  LOG_MSG_INFO1("Tiny Proxy: Enable: %d ", proxy_config, 0, 0);
  return true;
}

boolean QCMAP_Backhaul_WWAN::SetupTinyProxy(qmi_error_type_v01 *qmi_err_num)
{
  char command[MAX_COMMAND_STR_LEN];
  struct ifreq        req_if_info;
  int val;
  struct sockaddr     *saddr;
  int   sockfd;

  memset(&req_if_info, 0, sizeof(req_if_info));
  strlcpy(req_if_info.ifr_name, BRIDGE_IFACE,sizeof(BRIDGE_IFACE));
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  val = ioctl(sockfd, SIOCGIFADDR, &req_if_info);
  if(val < 0)
  {
    LOG_MSG_ERROR("Get bridge iface address failed",0,0,0);
    return false;
  }
  saddr = (struct sockaddr *)&(req_if_info.ifr_addr);

    /*adding user nobody to inet group for socket  permission*/
  snprintf(command, MAX_COMMAND_STR_LEN,
           "groupadd -g 3003 inet");
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "usermod -aG inet nobody");
  ds_system_call(command, strlen(command));
  /*end of adding user to inet group for socket  permission*/

  /*creating necessry files & setting owenership*/
  snprintf(command, MAX_COMMAND_STR_LEN,
           "mkdir /var/log/tinyproxy");
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "mkdir /var/run/tinyproxy");
  ds_system_call(command, strlen(command));
  snprintf(command, MAX_COMMAND_STR_LEN,
           "chown -R nobody:inet /var/log/tinyproxy");
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN,
           "chown -R nobody:inet /var/run/tinyproxy");
  ds_system_call(command, strlen(command));
  /*end of creating necessry files & setting owenership*/

  /*setting NAT rule*/
  snprintf(command, MAX_COMMAND_STR_LEN,
           "iptables -t nat -A PREROUTING -i bridge0 ! -d %s"
           " -p tcp --dport 80 -j REDIRECT --to-ports 8888",
           inet_ntoa(((struct sockaddr_in *)saddr)->sin_addr));
  ds_system_call(command, strlen(command));

  /*end setting NAT rule*/
  return true;
}

