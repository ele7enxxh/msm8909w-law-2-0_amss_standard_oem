/*====================================================

FILE:  QCMAP_MediaService.cpp

SERVICES:
   QCMAP Connection Manager Service Specific Implementation

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/31/14   us           Created

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
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_MediaService.h"
#include "QCMAP_LAN.h"
#include "QCMAP_WLAN.h"

// Define some constants
#define MAX_CMD_SIZE 100
#define MAX_SCAN_SIZE 100
#define MIN_NOTIFY_INTERVAL 30
#define MAX_NOTIFY_INTERVAL 60000
#define PROCESS_ID_OF_CMD "pidof"

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

std::string media_dir_blacklist[] = {
  "..",
  "./",
  "/bin",
  "/boot",
  "/cache",
  "/dev",
  "/lib",
  "/linuxrc",
  "/lost+found",
  "/proc",
  "/sbin",
  "/sys"
};

bool QCMAP_MediaService::flag= false;
QCMAP_MediaService* QCMAP_MediaService::object=NULL;

/*==========================================================
                             Class Definitions
  =========================================================*/


/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes MediaService variables.

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

QCMAP_MediaService::QCMAP_MediaService()
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_CM_LOG_FUNC_ENTRY();
  int status;
  memset(&this->srvc_cfg,0,sizeof(this->srvc_cfg));
  // Initialize M-DNS State
  this->mdns_state = QCMAP_MSGR_MDNS_MODE_DOWN_V01;
  // Initialize UPnP and DLNA states
  this->upnp_state = QCMAP_MSGR_UPNP_MODE_DOWN_V01;
  this->dlna_state = QCMAP_MSGR_DLNA_MODE_DOWN_V01;
  if(this->ReadServiceConfigFromXML() == FALSE)
  {
    LOG_MSG_ERROR("Error in loading MediaService Configuration\n", 0, 0, 0);
    return;
  }
  QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_DLNAWhitelisting, GET_VALUE,
                                                  (qcmap_media_svc_status *)&status);
  this->dlna_whitelisting_state = status;
  memset(&(this->dlna_whitelist_config), 0, sizeof(this->dlna_whitelist_config));
}

 /*==========================================================
   FUNCTION Destructor
 ==========================================================*/
 /*!
 @brief
   Destroyes MediaService variables.

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

 QCMAP_MediaService::~QCMAP_MediaService()
{
  qmi_error_type_v01 qmi_err_num;
  qcmap_dlna_whitelist_entry_list_t* dlnaWhitelist = &(this->dlna_whitelist_config.whitelist_entries);;
  LOG_MSG_INFO1("Destroying Object: MEDIA-SERVICE",0,0,0);

  if(this->srvc_cfg.upnp_config)
  {
    LOG_MSG_INFO1("Interim disable UPnP daemon", 0, 0, 0);
    this->InterimDisableUPNP(&qmi_err_num);
  }
  if(this->srvc_cfg.dlna_config)
  {
    LOG_MSG_INFO1("Interim disable DLNA daemon", 0, 0, 0);
    this->InterimDisableDLNA(&qmi_err_num);
  }
  if(this->srvc_cfg.mdns_config)
  {
    LOG_MSG_INFO1("Interim disable DLNA daemon", 0, 0, 0);
    this->InterimDisableMDNS(&qmi_err_num);
  }

  flag = false;
  object = NULL;
}

/*===========================================================================
  FUNCTION Get_Instance()
==========================================================================*/
/*!
@brief
   Returns the object of the QCMAP_MediaService Class

@parameters
  boolean obj_create

@return
  QCMAP_MediaService* object

@note

 - Dependencies
 - None

   Side Effects
   None
*/

QCMAP_MediaService* QCMAP_MediaService::Get_Instance(boolean obj_create)
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : MEDIA SERVICE",0, 0, 0);
    object = new QCMAP_MediaService();
    flag = true;
    return object;
  }
  else
  {
    return object;
  }
}

/*===========================================================================
  FUNCTION ReadServiceConfigFromXML()
==========================================================================*/
/*!
@brief
   Reads the QCMAP Media Service XML file

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

boolean QCMAP_MediaService::ReadServiceConfigFromXML()
{
  pugi::xml_document xml_file;
  pugi::xml_node root;

  if (!xml_file.load_file(QCMAP_ConnectionManager::xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPSrvcCfg_TAG);
  this->srvc_cfg.dlna_config = atoi(root.child(DLNA_TAG).child_value());
  this->srvc_cfg.upnp_config = atoi(root.child(UPNP_TAG).child_value());
  this->srvc_cfg.mdns_config = atoi(root.child(MDNS_TAG).child_value());
  this->srvc_cfg.dlna_whitelisting_config= atoi(root.child(DLNAWhitelistingAllow_TAG).child_value());

  LOG_MSG_INFO1("Upnp Config: %d Dlna Config: %d Mdns Config: %d" ,
                this->srvc_cfg.dlna_config, this->srvc_cfg.upnp_config,
                this->srvc_cfg.mdns_config);
  return true;
}

/*===========================================================================
  FUNCTION GetSetServiceConfigFromXML
==========================================================================*/
/*!
@brief
  Reads and writes into the QCMAP Media Service XML file

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

boolean QCMAP_MediaService::GetSetServiceConfigFromXML
(
  qcmap_service_config_enum conf,
  qcmap_action_type action,
  qcmap_media_svc_status *state
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root;
  const char *tag_ptr;
  char data[MAX_STRING_LENGTH] = {0};
  boolean status;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  if (!xml_file.load_file(QCMAP_ConnectionManager::xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPSrvcCfg_TAG);
  LOG_MSG_INFO1("Config: %d Action: %d value: %d" , conf, action, *state);

  switch (conf)
  {
    case CONFIG_UPNP:
      tag_ptr = UPNP_TAG;
    break;

    case CONFIG_DLNA:
      tag_ptr = DLNA_TAG;
    break;

    case CONFIG_MDNS:
      tag_ptr = MDNS_TAG;
    break;

    case CONFIG_DLNAWhitelisting:
      tag_ptr = DLNAWhitelistingAllow_TAG;
    break;

    default:
      LOG_MSG_ERROR("Invalid TAG.", 0, 0, 0);
      return false;
    break;
  }

  if (action == SET_VALUE)
  {
    if (*state == MEDIA_SVC_MODE_UP)
      status = true;
    else
      status = false;

    snprintf(data, sizeof(data), "%d", status);
    root.child(tag_ptr).text() = data;
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  }
  else
  {
    atoi(root.child(tag_ptr).child_value())? *state = MEDIA_SVC_MODE_UP: *state = MEDIA_SVC_MODE_DOWN;
  }

  LOG_MSG_INFO1("Config: %d Action: %d value: %d" , conf, action, *state);
  return true;
}

/*===========================================================================
  FUNCTION  EnableUPNP
==========================================================================*/
/*!
@brief
  Enables UPnP daemon.

@param
  qmi_error_type_v01 *qmi_err_num

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

boolean QCMAP_MediaService::EnableUPNP(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_LAN* QcMapLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_MediaService *QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(true);//creating MediaService object here
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  int ret = 0;
  qcmap_media_svc_status status;
  qmi_error_type_v01 wan_err_num;
  QCMAP_CM_LOG_FUNC_ENTRY();
  char tmp[MAX_CMD_SIZE];
  char bef[] = "sed -i \"56 apresentation_url=/";
  char aft[] = ":8201\" /etc/miniupnpd/miniupnpd.conf";
  in_addr addr;

  if(QcMapMediaServiceMgr->upnp_state == QCMAP_MSGR_UPNP_MODE_UP_V01)
  {
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  if(QcMapMediaServiceMgr->IsMediaServiceKilled(UPNP_SERVICE) == FALSE)
  {
    /* Stop UPnP in case it was started manually by the user */
    ds_system_call("/etc/init.d/miniupnpd stop", strlen("/etc/init.d/miniupnpd stop"));
    /* Once stopped check if it has actually stopped or not. */
    if (QcMapMediaServiceMgr->IsMediaServiceKilled(UPNP_SERVICE) == FALSE)
    {
      LOG_MSG_ERROR("Disable UPNP Failed...", 0, 0, 0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return false;
    }
  }

  /* Check that the bridge is up */
  if(QcMapLANMgr->bridge_inited)
  {
    /* update the presentation page url to the current IP address */
    ds_system_call("sed -i \"/presentation_url=/d\" /etc/miniupnpd/miniupnpd.conf",
                 strlen("sed -i \"/presentation_url=/d\" /etc/miniupnpd/miniupnpd.conf"));
    addr.s_addr = htonl(QCMAPLANMgr->lan_cfg.apps_ip_addr);
    snprintf(tmp, sizeof(tmp), "%s%s%s", bef, inet_ntoa(addr), aft);
    ds_system_call(tmp, strlen(tmp));
    ret = system("/etc/init.d/miniupnpd start");
  }
  else
  {
    /* Incorrect Mode. Restore it to default mode and continue. */
    LOG_MSG_ERROR("Bridge0 is not up: , UPnP not starting.\n",
                   0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    return false;
  }

  if(ret != 0)
  {
    LOG_MSG_ERROR("UPNP: Ret value = %d.\n", ret, 0, 0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    return false;
  }
  else
  {
    QcMapMediaServiceMgr->srvc_cfg.upnp_config = true;
    QcMapMediaServiceMgr->upnp_state = QCMAP_MSGR_UPNP_MODE_UP_V01;
    status=MEDIA_SVC_MODE_UP;
    QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_UPNP, SET_VALUE, &status);
    return true;
  }
}

/*===========================================================================
  FUNCTION  DisableUPNP
==========================================================================*/
/*!
@brief
  Disables UPnP daemon.

@param
  qmi_error_type_v01 *qmi_err_num

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

boolean QCMAP_MediaService::DisableUPNP(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_MediaService *QcMapMediaServiceMgr = QCMAP_MediaService::Get_Instance(false);
  qcmap_media_svc_status status;
  int ret = 0;
  QCMAP_CM_LOG_FUNC_ENTRY();
  ret = system("/etc/init.d/miniupnpd stop");
  if(ret != 0)
  {
    LOG_MSG_ERROR("UPNP Disable: Unable to stop UPNP,ds_system_call failed, Ret value = %d.\n",
                  ret, 0, 0);
    return false;
  }

  if(QcMapMediaServiceMgr)
  {
    if(QcMapMediaServiceMgr->upnp_state == QCMAP_MSGR_UPNP_MODE_DOWN_V01)
    {
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      LOG_MSG_INFO1("UPNP Already Disabled \n", 0, 0, 0);
      return false;
    }
    else
    {
      QcMapMediaServiceMgr->srvc_cfg.upnp_config = false;
      QcMapMediaServiceMgr->upnp_state = QCMAP_MSGR_UPNP_MODE_DOWN_V01;
      if( (QcMapMediaServiceMgr->dlna_state == QCMAP_MSGR_DLNA_MODE_DOWN_V01 ) &&
          (QcMapMediaServiceMgr->mdns_state == QCMAP_MSGR_MDNS_MODE_DOWN_V01 ) )
      {
      /* Delete the object when the state of upnp,dlna and mdns is down at the same time*/
        delete QcMapMediaServiceMgr;
      }
      status = MEDIA_SVC_MODE_DOWN;
      QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_UPNP, SET_VALUE, &status);
      return true;
    }
  }
  else
  {
    LOG_MSG_INFO1("UPNP DISABLED Failed, UPNP not Enabled", 0, 0, 0);
    /* need to return appropriate QMI error*/
    return false;
  }
}

/*===========================================================================
  FUNCTION  InterimDisableUPNP
==========================================================================*/
/*!
@brief
  Disables UPnP daemon.

@param
  qmi_error_type_v01 *qmi_err_num

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

boolean QCMAP_MediaService::InterimDisableUPNP(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_MediaService *QcMapMediaServiceMgr = QCMAP_MediaService::Get_Instance(false);
  int ret = 0;
  QCMAP_CM_LOG_FUNC_ENTRY();
  ret = system("/etc/init.d/miniupnpd stop");
  if(ret != 0)
  {
    LOG_MSG_ERROR("UPNP Interim Disable: Unable to stop UPNP, Ret value = %d.\n",
                  ret, 0, 0);
  }
  QcMapMediaServiceMgr->upnp_state = QCMAP_MSGR_UPNP_MODE_DOWN_V01;
  return true;
}

/*===========================================================================
  FUNCTION GetUPNPStatus
==========================================================================*/
/*!
@brief
  Gets the UPNP status.

@parameters
  qcmap_msgr_upnp_mode_enum_v01 *status,
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

boolean QCMAP_MediaService::GetUPNPStatus
(
  qcmap_msgr_upnp_mode_enum_v01 *status,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_MediaService *QcMapMediaServiceMgr =QCMAP_MediaService::Get_Instance(false);
  if(QcMapMediaServiceMgr)
    *status = QcMapMediaServiceMgr->upnp_state;
  else
  {
    QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_UPNP, GET_VALUE,
                                                  (qcmap_media_svc_status *)status);
  }

  return true;
}

/*===========================================================================
  FUNCTION  EnableDLNA
==========================================================================*/
/*!
@brief
  Enables DLNA.

@param
  qmi_error_type_v01 *qmi_err_num

@return
  true  - on success
  false - on failure


- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_MediaService::EnableDLNA(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_MediaService *QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(true);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  uint32 *curr_dlnawhitelist_entry = NULL;
  int i,ret,portno = 0;
  ds_dll_el_t * node = NULL;
  qcmap_media_svc_status status;
  char command[MAX_COMMAND_STR_LEN]={0};
  QCMAP_CM_LOG_FUNC_ENTRY();

  if(QcMapMediaServiceMgr->dlna_state == QCMAP_MSGR_DLNA_MODE_UP_V01)
  {
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  if(QcMapMediaServiceMgr->dlna_whitelisting_state == true && QcMapMediaServiceMgr->GetDLNAPortNumber(&portno,qmi_err_num))
  {
    //Default Rule Added when Whitelisting is Enabled
    QcMapMediaServiceMgr->GetDLNAPortNumber(&portno,qmi_err_num);
    snprintf( command, MAX_COMMAND_STR_LEN,
    "iptables -A INPUT -p tcp -i %s --dport %d -j DROP", BRIDGE_IFACE,portno );
    ds_system_call(command, strlen(command));
  }
  /*  Check if the DLNA is already running or not.(could be started by a user manually)*/
  /* IsMediaServiceKilled() returns false if DLNA is running.*/
  if(QcMapMediaServiceMgr->IsMediaServiceKilled(DLNA_SERVICE) == FALSE)
  {
    ds_system_call("/etc/init.d/minidlna stop", strlen("/etc/init.d/minidlna stop"));
    /* Once stopped check if it has actually stopped or not. */
    if (QcMapMediaServiceMgr->IsMediaServiceKilled(DLNA_SERVICE) == FALSE)
    {
      LOG_MSG_ERROR("Unable to stop DLNA in 1 second.\n", 0, 0, 0);
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return false;
    }
  }
  /* Check that bridge0 is up */
  if(QCMAPLANMgr->bridge_inited)
  {
    ret = system("/etc/init.d/minidlna start");
  }
  else
  {
    /* Incorrect Mode. */
    LOG_MSG_ERROR("Bridge0 is not up: DLNA not starting.\n",
                   0, 0, 0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    return false;
  }

  if(ret != 0)
  {
    LOG_MSG_ERROR("DLNA: Ret value = %d.\n", ret, 0, 0);
    *qmi_err_num = QMI_ERR_INTERFACE_NOT_FOUND_V01;
    return false;
  }
  else
  {
    QcMapMediaServiceMgr->srvc_cfg.dlna_config = true;
    QcMapMediaServiceMgr->dlna_state = QCMAP_MSGR_DLNA_MODE_UP_V01;
    status=MEDIA_SVC_MODE_UP;
    QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_DLNA, SET_VALUE, &status);
  }
  LOG_MSG_INFO1("ReadDLNAWhitelistIPFromXML HERE",0,0,0);
  if(QcMapMediaServiceMgr->ReadDLNAWhitelistIPFromXML(QcMapMgr->xml_path, false)== false)
  {
    LOG_MSG_ERROR("Error in loading DLNA WHitelist Configuration\n", 0, 0, 0);
    return;
  }
  qcmap_dlna_whitelist_entry_list_t* dlnaWhitelist = &(QcMapMediaServiceMgr->dlna_whitelist_config.whitelist_entries);
  node = dlnaWhitelist->dlnawhitelistListHead;
  if (node)
  {
    node = ds_dll_next (node, (const void**)(&curr_dlnawhitelist_entry));
    if (node == NULL || curr_dlnawhitelist_entry == NULL)
    {
      LOG_MSG_ERROR("EnableDLNAWhitelist IPinfo is NULL"
                    "Cant fetch info",0,0,0);
      return false;
    }

    for (i = 0; i < QcMapMediaServiceMgr->dlna_whitelist_config.num_whitelist_entries; i++)
    {
      if (QcMapMediaServiceMgr->dlna_whitelisting_state)
        QcMapMediaServiceMgr->SetDLNAWhitelistRule((curr_dlnawhitelist_entry),true, qmi_err_num);
      else
        QcMapMediaServiceMgr->SetDLNAWhitelistRule(curr_dlnawhitelist_entry,false, qmi_err_num);
      node = ds_dll_next (node, (const void**)(&curr_dlnawhitelist_entry));
    }
  }

  return true;
}
/*===========================================================================
  FUNCTION  DisableDLNA
==========================================================================*/
/*!
@brief
  Disable DLNA based

@param
  qmi_error_type_v01 *qmi_err_num

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

boolean QCMAP_MediaService::DisableDLNA(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_MediaService *QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
  uint32 *curr_dlnawhitelist_entry = NULL;
  int i,ret,portno = 0;
  ds_dll_el_t * node = NULL;
  qcmap_media_svc_status status;
  char command[MAX_COMMAND_STR_LEN]={0};
  QCMAP_CM_LOG_FUNC_ENTRY();
  ret = system("/etc/init.d/minidlna stop");
  if(ret != 0)
  {
    LOG_MSG_ERROR("DLNA Disable: Unable to stop DLNA,ds_system_call failed, Ret value = %d.\n",
                  ret, 0, 0);
    return false;
  }
  if(QcMapMediaServiceMgr)
  {
    if(QcMapMediaServiceMgr->dlna_state == QCMAP_MSGR_DLNA_MODE_DOWN_V01)
    {
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      LOG_MSG_INFO1("DLNA Already Disabled \n", 0, 0, 0);
      return false;
    }
    else
    {
      if(QcMapMediaServiceMgr->dlna_whitelisting_state == true && QcMapMediaServiceMgr->GetDLNAPortNumber(&portno,qmi_err_num))
      {
        //Default Rule Added when Whitelisting is Enabled
        QcMapMediaServiceMgr->GetDLNAPortNumber(&portno,qmi_err_num);
        snprintf( command, MAX_COMMAND_STR_LEN,
        "iptables -D INPUT -p tcp -i %s --dport %d -j DROP", BRIDGE_IFACE,portno );
        ds_system_call(command, strlen(command));
      }
      //Disable DLNA
      QcMapMediaServiceMgr->srvc_cfg.dlna_config = false;
      QcMapMediaServiceMgr->dlna_state = QCMAP_MSGR_DLNA_MODE_DOWN_V01;

      status=MEDIA_SVC_MODE_DOWN;
      ret = true;
      QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_DLNA, SET_VALUE,&status);
      if (QcMapMediaServiceMgr->IsMediaServiceKilled(DLNA_SERVICE) == FALSE)
      {
        LOG_MSG_ERROR("Unable to stop DLNA in 1 second.\n", 0, 0, 0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        ret = false;
      }
      qcmap_dlna_whitelist_entry_list_t* dlnaWhitelist = &(QcMapMediaServiceMgr->dlna_whitelist_config.whitelist_entries);
      node = dlnaWhitelist->dlnawhitelistListHead;
      if (node)
      {
        node = ds_dll_next (node, (const void**)(&curr_dlnawhitelist_entry));
        if (node == NULL || curr_dlnawhitelist_entry == NULL)
        {
          LOG_MSG_ERROR("DisableDLNAWhitelist IPinfo is NULL Cant fetch info",0,0,0);
          return false;
        }
        
        for (i = 0; i < QcMapMediaServiceMgr->dlna_whitelist_config.num_whitelist_entries; i++)
        {
          QcMapMediaServiceMgr->SetDLNAWhitelistRule(curr_dlnawhitelist_entry,false, qmi_err_num);
          node = ds_dll_next (node, (const void**)(&curr_dlnawhitelist_entry));
        }
      }

      if( (QcMapMediaServiceMgr->upnp_state == QCMAP_MSGR_UPNP_MODE_DOWN_V01 ) &&
          (QcMapMediaServiceMgr->mdns_state == QCMAP_MSGR_MDNS_MODE_DOWN_V01 ) )
      {
        delete QcMapMediaServiceMgr;
      }
      return ret;
    }
  }
  else
  {
    LOG_MSG_INFO1("DLNA DISABLED Failed, DLNA not previously Enabled", 0, 0, 0);
    /* need to return appropriate QMI error*/
    return false;
  }
}

/*===========================================================================
  FUNCTION  InterimDisableDLNA
==========================================================================*/
/*!
@brief
  Disable DLNA based

@param
  qmi_error_type_v01 *qmi_err_num

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

boolean QCMAP_MediaService::InterimDisableDLNA(qmi_error_type_v01 *qmi_err_num)
{
  int ret = 0;
  ret = system("/etc/init.d/minidlna stop");
  if(ret != 0)
  {
    LOG_MSG_ERROR("DLNA Interim Disable: Unable to stop DLNA, Ret value = %d.\n",
                  ret, 0, 0);
  }
  this->dlna_state = QCMAP_MSGR_DLNA_MODE_DOWN_V01;
  if (this->IsMediaServiceKilled(DLNA_SERVICE) == FALSE)
  {
    LOG_MSG_ERROR("Unable to stop DLNA in 1 second.\n", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION  RestartDLNA
==========================================================================*/
/*!
@brief
  Restart DLNA daemon if already running

@param
  qmi_error_type_v01 *qmi_err_num

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
boolean QCMAP_MediaService::RestartDLNA(qmi_error_type_v01 *qmi_err_num)
{
  /* Start DLNA if auto start is enabled. Restart if already running */

  if(this->srvc_cfg.dlna_config)
  {
    if (this->dlna_state == QCMAP_MSGR_DLNA_MODE_UP_V01)
    {
      if (!this->InterimDisableDLNA(qmi_err_num))
      {
        LOG_MSG_INFO1("Disable DLNA failed: %d.", *qmi_err_num,0,0);
      }
    }
    if(!this->EnableDLNA(qmi_err_num))
    {
      LOG_MSG_ERROR("Cannot enable DLNA, error: %d.", *qmi_err_num, 0, 0);
      return false;
    }
  }

  return true;
}

/*===========================================================================
  FUNCTION GetDLNAStatus
==========================================================================*/
/*!
@brief
  Gets the DLNA status.

@parameters
  qcmap_msgr_dlna_mode_enum_v01 *status,
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

boolean QCMAP_MediaService::GetDLNAStatus
(
  qcmap_msgr_dlna_mode_enum_v01 *status,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_MediaService *QcMapMediaServiceMgr = QCMAP_MediaService::Get_Instance(false);
  if(QcMapMediaServiceMgr)
    *status = QcMapMediaServiceMgr->dlna_state;
  else
  {
    QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_DLNA, GET_VALUE,
                                                  (qcmap_media_svc_status *)status);
  }
  return true;
}

/*===========================================================================
  FUNCTION SetDLNAMediaDir
==========================================================================*/
/*!
@brief
  Sets the DLNA media directory.

@parameters
  char media_dir[],
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
boolean QCMAP_MediaService::SetDLNAMediaDir
(
  char media_dir[],
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_MediaService *QcMapMediaServiceMgr = QCMAP_MediaService::Get_Instance(false);
  int ret;
  int size = sizeof(media_dir_blacklist)/sizeof(media_dir_blacklist[0]);
  char tmp[MAX_CMD_SIZE];
  char bef[] = "sed -i \"13 amedia_dir=";
  char aft[] = "\" /etc/minidlna.conf";
  char *token, *ptr;
  char *str_start;   /*pointer for storing  start address  of string*/
  //check against black list
  for (int i=0; i < size ; i++)
  {
    if (strstr(media_dir, media_dir_blacklist[i].c_str()))
    {
      LOG_MSG_ERROR("DLNA Media dir matches blacklist, not adding: %d", i, 0, 0);
      *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
      return false;
    }
  }
  ret = system("sed -i \"/media_dir=\\/.*/d\" /etc/minidlna.conf");
  token = strtok_r(media_dir, ",", &ptr);
  while (token != NULL)
  {
    /*removing leading spaces if present*/
    str_start = token;
    while ((*str_start != '\0') && (*str_start == ' '))
    {
      ++str_start;
    }
    if (str_start != token)  /*if leading space is present*/
    {
      memmove(token, str_start, strlen (str_start)+1);
    }
    /*end of removing leading space*/
    snprintf(tmp, sizeof(tmp), "%s%s%s", bef, token, aft);
    ret = system(tmp);
    token = strtok_r(NULL, ",", &ptr);
  }
  if(QcMapMediaServiceMgr && !QcMapMediaServiceMgr->RestartDLNA(qmi_err_num))
  {
    LOG_MSG_ERROR("Restart DLNA failed. \n",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  return true;
}

/*===========================================================================
  FUNCTION GetDLNAMediaDir
==========================================================================*/
/*!
@brief
  Gets the DLNA media directory.

@parameters
  char media_dir[],
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
boolean QCMAP_MediaService::GetDLNAMediaDir
(
  char media_dir[],
  qmi_error_type_v01 *qmi_err_num
)
{
  FILE *file = NULL;
  char temp[MAX_SCAN_SIZE] = "";
  file = fopen("/etc/minidlna.conf", "rt");
  if (file == NULL)
  {
    LOG_MSG_ERROR("GetDLNAMediaDir unable to mind /etc/minidlna.conf!", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  while (fgets(temp, MAX_SCAN_SIZE, file) != NULL)
  {
    if (strstr(temp, "media_dir=") != NULL && temp[0] != '#')
    {
      strlcat(media_dir, &temp[10], QCMAP_MSGR_MAX_DLNA_DIR_LEN_V01);
    }
  }
  fclose(file);
  return true;
}

/*===========================================================================
  FUNCTION  EnableMDNS
==========================================================================*/
/*!
@brief
  Enable M-DNS if it not already running

@param
qmi_error_type_v01 *qmi_err_num

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

boolean QCMAP_MediaService::EnableMDNS(qmi_error_type_v01 *qmi_err_num)
{
  qcmap_media_svc_status status;
  QCMAP_CM_LOG_FUNC_ENTRY();
  int ret=0;
  QCMAP_MediaService *QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(true);
  if(QcMapMediaServiceMgr->mdns_state == QCMAP_MSGR_MDNS_MODE_UP_V01)
  {
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    LOG_MSG_INFO1("M-DNS Already Enabled \n", 0, 0, 0);
    return false;
  }
  else
  {
    // Enable MDNS
    ret = ds_system_call("/etc/init.d/avahi-daemon start",strlen("/etc/init.d/avahi-daemon start"));
    if (ret == 0)
    {
      LOG_MSG_INFO1("M-DNS ENABLED!! ", 0, 0, 0);
      QcMapMediaServiceMgr->mdns_state = QCMAP_MSGR_MDNS_MODE_UP_V01;
      QcMapMediaServiceMgr->srvc_cfg.mdns_config = true;
      status= MEDIA_SVC_MODE_UP;
      QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_MDNS, SET_VALUE, &status);
      return true;
    }
    else
    {
      LOG_MSG_INFO1("M-DNS ENABLE Failed, ds_system_call failed", 0, 0, 0);
      /* need to return appropriate QMI error*/
      return false;
    }
  }
}


/*===========================================================================
  FUNCTION  InterimDisableMDNS
==========================================================================*/
/*!
@brief
  Disables MDNS daemon.

@param
  qmi_error_type_v01 *qmi_err_num

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

boolean QCMAP_MediaService::InterimDisableMDNS(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_MediaService *QcMapMediaServiceMgr = QCMAP_MediaService::Get_Instance(false);
  int ret = 0;
  QCMAP_CM_LOG_FUNC_ENTRY();

  ret = ds_system_call("/etc/init.d/avahi-daemon stop",strlen("/etc/init.d/avahi-daemon stop"));
  if (ret == 0)
  {
    LOG_MSG_INFO1("M-DNS DISABLED", 0, 0, 0);
    QcMapMediaServiceMgr->mdns_state = QCMAP_MSGR_MDNS_MODE_DOWN_V01;
    return true;
  }
  else
  {
    LOG_MSG_INFO1("M-DNS Interim DISABLE Failed, ds_system_call failed, Ret value=%d ", ret, 0, 0);
    /* need to return appropriate QMI error*/
    return false;
  }

}

/*===========================================================================
  FUNCTION  DisableMDNS
==========================================================================*/
/*!
@brief
  Disable M-DNS if it is already running

@param
qmi_error_type_v01 *qmi_err_num

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

boolean QCMAP_MediaService::DisableMDNS(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_MediaService *QcMapMediaServiceMgr = QCMAP_MediaService::Get_Instance(false);
  qcmap_media_svc_status status ;
  QCMAP_CM_LOG_FUNC_ENTRY();
  int ret=0;

  if(QcMapMediaServiceMgr)
  {
    if(QcMapMediaServiceMgr->mdns_state == QCMAP_MSGR_MDNS_MODE_DOWN_V01)
    {
      *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      LOG_MSG_INFO1("M-DNS Already Disabled \n", 0, 0, 0);
      return false;
    }
    else
    {
      // Disable MDNS
      ret = ds_system_call("/etc/init.d/avahi-daemon stop",strlen("/etc/init.d/avahi-daemon stop"));
      if (ret == 0)
      {
        LOG_MSG_INFO1("M-DNS DISABLED", 0, 0, 0);
        QcMapMediaServiceMgr->mdns_state = QCMAP_MSGR_MDNS_MODE_DOWN_V01;
        QcMapMediaServiceMgr->srvc_cfg.mdns_config = false;
        if((QcMapMediaServiceMgr->dlna_state == QCMAP_MSGR_DLNA_MODE_DOWN_V01 ) &&
               (QcMapMediaServiceMgr->upnp_state == QCMAP_MSGR_UPNP_MODE_DOWN_V01 ) )
        {
          delete QcMapMediaServiceMgr;
        }
        status=MEDIA_SVC_MODE_DOWN;
        QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_MDNS, SET_VALUE, &status);
        return true;
      }
      else
      {
        LOG_MSG_INFO1("M-DNS DISABLED Failed, ds_system_call failed, Ret value=%d ", ret, 0, 0);
        /* need to return appropriate QMI error*/
        return false;
      }
      return true;
    }
  }
  else
  {
    LOG_MSG_INFO1("M-DNS DISABLED Failed, M-DNS not previously Enabled", 0, 0, 0);
    /* need to return appropriate QMI error*/
    return false;
  }
}

/*===========================================================================
  FUNCTION GetMDNSStatus
==========================================================================*/
/*!
@brief
  Gets the MDNS status.

@parameters
  qcmap_msgr_mdns_mode_enum_v01 *status,
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

boolean QCMAP_MediaService::GetMDNSStatus
(
  qcmap_msgr_mdns_mode_enum_v01 *status,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_MediaService *QcMapMediaServiceMgr = QCMAP_MediaService::Get_Instance(false);
  if(QcMapMediaServiceMgr)
    *status = QcMapMediaServiceMgr->mdns_state;
  else
  {
    QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_MDNS, GET_VALUE,
                                                  (qcmap_media_svc_status *)status);
  }

  return true;
}

/*===========================================================================
  FUNCTION SetUPNPNotifyInterval
==========================================================================*/
/*!
@brief
  Sets the UPnP notify interval.

@parameters
  int notify_int,
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

boolean QCMAP_MediaService::SetUPNPNotifyInterval
(
  int notify_int,
  qmi_error_type_v01 *qmi_err_num
)
{
  char tmp[MAX_CMD_SIZE];
  char bef[] = "sed -i \"62 anotify_interval=";
  char aft[] = "\" /etc/miniupnpd/miniupnpd.conf";
  if (notify_int < MIN_NOTIFY_INTERVAL || notify_int > MAX_NOTIFY_INTERVAL)
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  ds_system_call("sed -i \"/notify_interval=/d\" /etc/miniupnpd/miniupnpd.conf",
                 strlen("sed -i \"/notify_interval=/d\" /etc/miniupnpd/miniupnpd.conf"));
  snprintf(tmp, sizeof(tmp), "%s%d%s", bef, notify_int, aft);
  ds_system_call(tmp, strlen(tmp));
  return true;
}

/*===========================================================================
  FUNCTION GetUPNPNotifyInterval
==========================================================================*/
/*!
@brief
  Gets the UPnP notify interval.

@parameters
  int *notify_int,
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
boolean QCMAP_MediaService::GetUPNPNotifyInterval
(
  int *notify_int,
  qmi_error_type_v01 *qmi_err_num
)
{
  FILE *file = NULL;
  char temp[MAX_SCAN_SIZE] = "";
  file = fopen("/etc/miniupnpd/miniupnpd.conf", "rt");
  if (file == NULL)
  {
    LOG_MSG_ERROR("GetUPNPNotifyInterval unable to find /etc/miniupnpd/miniupnpd.conf!", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return false;
  }
  while (fgets(temp, MAX_SCAN_SIZE, file) != NULL)
  {
    if (strstr(temp, "notify_interval=") != NULL && temp[0] != '#')
    {
      *notify_int = atoi(&temp[16]);
      fclose(file);
      return true;
    }
  }

  fclose(file);
  *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
  return false;
}

/*===========================================================================
  FUNCTION SetDLNANotifyInterval
==========================================================================*/
/*!
@brief
  Sets the DLNA notify interval.

@parameters
  int notify_int,
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

boolean QCMAP_MediaService::SetDLNANotifyInterval
(
  int notify_int,
  qmi_error_type_v01 *qmi_err_num
)
{
  char tmp[MAX_CMD_SIZE];
  char bef[] = "sed -i \"50 anotify_interval=";
  char aft[] = "\" /etc/minidlna.conf";

  if (notify_int < MIN_NOTIFY_INTERVAL || notify_int > MAX_NOTIFY_INTERVAL)
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }

  ds_system_call("sed -i \"/notify_interval=/d\" /etc/minidlna.conf",
                 strlen("sed -i \"/notify_interval=/d\" /etc/minidlna.conf"));

  snprintf(tmp, sizeof(tmp), "%s%d%s", bef, notify_int, aft);
  ds_system_call(tmp, strlen(tmp));
  return true;
}


/*===========================================================================
  FUNCTION GetDLNANotifyInterval
==========================================================================*/
/*!
@brief
  Gets the DLNA notify interval.

@parameters
  int *notify_int,
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

boolean QCMAP_MediaService::GetDLNANotifyInterval
(
  int *notify_int,
  qmi_error_type_v01 *qmi_err_num
)
{
  FILE *file = NULL;
  char temp[MAX_SCAN_SIZE] = "";
  file = fopen("/etc/minidlna.conf", "rt");
  if (file == NULL)
  {
    LOG_MSG_ERROR("GetUPNPNotifyInterval unable to find /etc/minidlna.conf!", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return false;
  }

  while (fgets(temp, MAX_SCAN_SIZE, file) != NULL)
  {
    if (strstr(temp, "notify_interval=") != NULL && temp[0] != '#')
    {
      *notify_int = atoi(&temp[16]);
      fclose(file);
      return true;
    }
  }
  fclose(file);
  *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
  return false;
}

/*===========================================================================
  FUNCTION IsMediaServiceKilled
==========================================================================*/
/*!
@brief
  Waits 1 second for the process to die.

@parameters
  service name

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
boolean QCMAP_MediaService::IsMediaServiceKilled(char *service)
{
  int i = 0;
  FILE *cmd=NULL;
  char pid_s[MAX_CMD_SIZE];
  int pid = 0;
  char process[MAX_CMD_SIZE] = {0};

  snprintf(process, sizeof(process), "%s %s", PROCESS_ID_OF_CMD, service);
  while (i++ < 5)
  {
    usleep(200000);
    cmd = popen(process, "r");
    pid = 0;
    memset(pid_s, 0, MAX_CMD_SIZE);
    if(cmd)
    {
      fgets(pid_s, MAX_CMD_SIZE, cmd);
      pclose(cmd);
    }
    pid = atoi(pid_s);
    if(pid == 0)
    {
      return true;
    }
  }
  LOG_MSG_ERROR("PID still running after waiting 1 second\n", 0, 0, 0);
  return false;
}

/*===========================================================================
  FUNCTION GetDLNAWhitelisting
==========================================================================*/
/*!
@brief
  Gets the DLNA Whitelisting status.

@parameters
  boolean *status,
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

boolean QCMAP_MediaService::GetDLNAWhitelisting
(
  int *status,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_MediaService *QcMapMediaServiceMgr =QCMAP_MediaService::Get_Instance(false);
  if(QcMapMediaServiceMgr )
  {
    if (QcMapMediaServiceMgr->dlna_whitelisting_state == true)
    {
      *status = true;
    }
    else
      *status = false;
  }
  else
  {
      QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_DLNAWhitelisting, GET_VALUE,
                                                  (qcmap_media_svc_status *)status);
      LOG_MSG_INFO1("GET STATUS %d",*status,0,0);
  }
  return true;
}

/*===========================================================================
  FUNCTION SetDLNAWhitelisting
==========================================================================*/
/*!
@brief
  Sets the DLNA Whitelisting status.

@parameters
  boolean *status,
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

boolean QCMAP_MediaService::SetDLNAWhitelisting
(
  int status,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_MediaService *QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
  int i,ret,portno = 0;
  char command[MAX_COMMAND_STR_LEN]={0};
  QCMAP_CM_LOG_FUNC_ENTRY();
  LOG_MSG_INFO1("Set DLNA status: %d",status,0,0);
  ds_dll_el_t * node = NULL;
  uint32 *curr_dlnawhitelist_entry = NULL;
  qcmap_dlna_whitelist_entry_list_t* dlnaWhitelist = NULL;
  if(QcMapMediaServiceMgr)
  {
    LOG_MSG_INFO1(" MEdia srvc object %d",status,0,0);
    if (QcMapMediaServiceMgr->dlna_whitelisting_state == true && status)
    {
     *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
      return false;
    }
    if(QcMapMediaServiceMgr->ReadDLNAWhitelistIPFromXML(QcMapMgr->xml_path , TRUE)== false)
    {
      LOG_MSG_ERROR("Error in loading DLNA WHitelist Configuration\n", 0, 0, 0);
      return;
    }
    if (status)
    {
      QcMapMediaServiceMgr->dlna_whitelisting_state = true;
      QcMapMediaServiceMgr->srvc_cfg.dlna_whitelisting_config = true;
    }
    else
    {
      QcMapMediaServiceMgr->dlna_whitelisting_state = false;
      QcMapMediaServiceMgr->srvc_cfg.dlna_whitelisting_config = false;
    }

    if(QcMapMediaServiceMgr->dlna_whitelisting_state == true && QcMapMediaServiceMgr->GetDLNAPortNumber(&portno,qmi_err_num))
    {
      //Default Rule Added when Whitelisting is Enabled
      QcMapMediaServiceMgr->GetDLNAPortNumber(&portno,qmi_err_num);
      snprintf( command, MAX_COMMAND_STR_LEN,
      "iptables -A INPUT -p tcp -i %s --dport %d -j DROP", BRIDGE_IFACE,portno );
      ds_system_call(command, strlen(command));
    }
    if(QcMapMediaServiceMgr->dlna_whitelisting_state == false && QcMapMediaServiceMgr->GetDLNAPortNumber(&portno,qmi_err_num))
    {
      snprintf( command, MAX_COMMAND_STR_LEN,
      "iptables -D INPUT -p tcp -i %s --dport %d -j DROP", BRIDGE_IFACE,portno );
      ds_system_call(command, strlen(command));
    }
    qcmap_dlna_whitelist_entry_list_t* dlnaWhitelist = &(QcMapMediaServiceMgr->dlna_whitelist_config.whitelist_entries);
    node = dlnaWhitelist->dlnawhitelistListHead;
    if (node)
    {
      node = ds_dll_next (node, (const void**)(&curr_dlnawhitelist_entry));
      if (node == NULL || curr_dlnawhitelist_entry == NULL)
      {
        LOG_MSG_ERROR("SetDLNAWhitelist IPinfo is NULL"
                      "Cant fetch info",0,0,0);
        return false;
      }

      for (i = 0; i < QcMapMediaServiceMgr->dlna_whitelist_config.num_whitelist_entries; i++)
      {
        if (curr_dlnawhitelist_entry != NULL )
        {
          LOG_MSG_INFO1(" AddDLNAWhitelist LOOP %d",(*curr_dlnawhitelist_entry),0,0);
        }
        else
        {
          LOG_MSG_INFO1(" AddDLNAWhitelist LOOP curr_dlnawhitelist_entry is null",0,0,0);
        }

        if (QcMapMediaServiceMgr->dlna_whitelisting_state)
          QcMapMediaServiceMgr->SetDLNAWhitelistRule(curr_dlnawhitelist_entry,true, qmi_err_num);
        else
          QcMapMediaServiceMgr->SetDLNAWhitelistRule(curr_dlnawhitelist_entry,false, qmi_err_num);
        node = ds_dll_next (node, (const void**)(&curr_dlnawhitelist_entry));
      }
    }
  }
  QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_DLNAWhitelisting, SET_VALUE,
                                                (qcmap_media_svc_status *)&status);
  return true;
}

/*===========================================================================
  FUNCTION AddDLNAWhitelistConnection
==========================================================================*/
/*!
@brief
  Checks for the duplicate entries of firewall entryi, if unique
  adds the firewall entry and update the same in the mobileap configuration file.

@parameters
  qcmap_msgr_firewall_conf_t* extd_firewall_conf

@return
  0  - on success
  -1 - on failure

@note
- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_MediaService::AddDLNAWhitelistConnection
(
  uint32_t dlna_whitelist_ip,
  qmi_error_type_v01 *qmi_err_num
)
{
  int i, add_firewall_entry = 0;
  int next_hdr_prot;
  ds_dll_el_t * node = NULL;
  char command[MAX_COMMAND_STR_LEN] , tempIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  QCMAP_MediaService *QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
  struct in_addr addr;
  uint32 *curr_dlnawhitelist_entry = NULL;
  qcmap_dlna_whitelist_entry_list_t* dlnaWhitelist = NULL;
  pugi::xml_document xml_file;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  if (QcMapMediaServiceMgr )
  {
    dlnaWhitelist = &(QcMapMediaServiceMgr->dlna_whitelist_config.whitelist_entries);
    if (dlnaWhitelist->dlnawhitelistListHead)
    {
      node = ds_dll_next(dlnaWhitelist->dlnawhitelistListHead,
                        (&curr_dlnawhitelist_entry));
      if (node == NULL)
      {
        LOG_MSG_ERROR("AddDLNA WHITELIST is NULL Cant fetch  info",0,0,0);
        return false;
      }
      for ( i=0; i < QcMapMediaServiceMgr->dlna_whitelist_config.num_whitelist_entries; i++ )
      {
        LOG_MSG_INFO1("Checking for duplicate here",0,0,0);
        if ( dlna_whitelist_ip == *curr_dlnawhitelist_entry)
        {
          /* we already added this one */
          LOG_MSG_ERROR("\nWHITELIST IP already present!!",0,0,0);
          *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
          return false;
        }
        node = ds_dll_next(node, (&curr_dlnawhitelist_entry));
      }
    }

    if ((QcMapMediaServiceMgr->dlna_whitelisting_state == true ) &&
         QcMapMediaServiceMgr->SetDLNAWhitelistRule(&dlna_whitelist_ip,TRUE, qmi_err_num ) != QCMAP_CM_SUCCESS)
    {
        LOG_MSG_ERROR("SetDLNAWhitelist Entry failed\n",0,0,0);
        return  QCMAP_CM_ERROR;
      }

    if(QcMapMediaServiceMgr->AddDLNAWhitelistIPToList(&dlna_whitelist_ip) == false)
    {

      LOG_MSG_ERROR("Failed to Add NAT Entry DLNA WHitelisting IP to List", 0, 0, 0);
      return false;
    }
  }
  if( dlna_whitelist_ip == 0 )
  {
    LOG_MSG_ERROR("Invalid Whitelisting IP\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  if (QCMAP_MediaService::searchDLNAWhitelistIPInXML(&xml_file,&dlna_whitelist_ip))
  {
    /* we already added this one */
    LOG_MSG_ERROR("\nWhitelistEntry already present!!",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  LOG_MSG_INFO1("Adding Whitelist IP to XML file", 0, 0, 0);
  QCMAP_MediaService::AddDLNAWhitelistingIPToXML(&xml_file,&dlna_whitelist_ip);

return true;
}


/*===========================================================================
  FUNCTION AddDLNAWhitelistIPToList
==========================================================================*/
/*!
@brief
  Adds the DLNAWhitelistIP to the List.

@parameters
  qcmap_cm_port_fwding_entry_conf_t *nat_entry

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


boolean QCMAP_MediaService::AddDLNAWhitelistIPToList(  uint32_t *dlna_whitelist_ip)
{
  ds_dll_el_t * node = NULL;
  qcmap_dlna_whitelist_entry_list_t* dlnaWhitelist = &(this->dlna_whitelist_config.whitelist_entries);
  uint32_t *dlna_whitelist_ip_new = NULL;
  if (dlnaWhitelist->dlnawhitelistListHead== NULL )
  {
    /*The first node which is created is a dummy node which does not store any device
            information. This is done to make use of the doubly linked list framework which
            is already existing*/
    if (( node = ds_dll_init(NULL)) == NULL)
    {
      return false;
    }
    dlnaWhitelist->dlnawhitelistListHead = node;
  }
  dlna_whitelist_ip_new = (uint32_t*)ds_malloc(sizeof(uint32_t));
  if( dlna_whitelist_ip_new == NULL )
  {
    LOG_MSG_ERROR("AddDLNAWhitelistIPToList - Error in allocating memory for"
                  "new NAT entry",0,0,0);
    return false;
  }
  memset(dlna_whitelist_ip_new, 0, sizeof(uint32_t));
  /* save into the config */
  memcpy(dlna_whitelist_ip_new, dlna_whitelist_ip, sizeof(uint32_t));
  //Enque the node
  if ((node = ds_dll_enq(dlnaWhitelist->dlnawhitelistListHead,
                          NULL, (void*)dlna_whitelist_ip_new )) == NULL)
  {
    ds_free(dlna_whitelist_ip_new);
    dlna_whitelist_ip_new = NULL;
    return false;
  }
  dlnaWhitelist->dlnawhitelistListTail= node;
  this->dlna_whitelist_config.num_whitelist_entries++;
  return true;
}

/*==========================================================
  FUNCTION AddDLNAWhitelistingIPToXML
==========================================================*/
/*!
@brief
  Add DLNA Whitelisting Entry to the XML.

@parameters
  pugi::xml_document *xml_file,
  uint32_t *dlna_whitelist_ip

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

void QCMAP_MediaService::AddDLNAWhitelistingIPToXML(pugi::xml_document *xml_file,
                                                             uint32_t *dlna_whitelist_ip )
{

  struct in_addr addr;
  char data[MAX_STRING_LENGTH] = {0};
  char str[INET6_ADDRSTRLEN];
  uint32_t debug_string=0;

  pugi::xml_node root, child, subchild;
  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);

  root = xml_file->child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPSrvcCfg_TAG);

 // add new WhitelistedIP entry here.
  child = root.append_child(WhitelistedIP_TAG);

  //WhitelistedIP Address
//  subchild = child.append_child(WhitelistedIP_TAG);
  addr.s_addr = htonl(*dlna_whitelist_ip);
  child.append_child(pugi::node_pcdata).set_value(inet_ntoa(addr));

  QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, xml_file);

  debug_string=htonl( *dlna_whitelist_ip);
  readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
  LOG_MSG_INFO1("\nWhitelistedIP: '%s'",str,0,0);
}

/*==========================================================
  FUNCTION searchDLNAWhitelistIPInXML
==========================================================*/
/*!
@brief
  Searches the NAT Config in the XML

@parameters
  pugi::xml_document *xml_file,
  qcmap_cm_port_fwding_entry_conf_t* nat_entry

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

pugi::xml_node QCMAP_MediaService::searchDLNAWhitelistIPInXML
(
  pugi::xml_document *xml_file,
  uint32_t *dlna_whitelist_ip
)
{
  pugi::xml_node root, child;
  qcmap_msgr_firewall_entry_conf_t firewall_conf;
  char str[INET6_ADDRSTRLEN];
  uint32_t debug_string=0;
  root = xml_file->child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPSrvcCfg_TAG);
  in_addr addr;
  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strcmp(child.name(),WhitelistedIP_TAG))
    {
      inet_aton(child.child_value(), &addr);
      if(ntohl(addr.s_addr) == *dlna_whitelist_ip)
      {
        LOG_MSG_INFO1("Existing WhitelistIP Entry found", 0, 0, 0);
        return child;
      }
    }
  }
  debug_string=htonl( *dlna_whitelist_ip);
  readable_addr(AF_INET,(const uint32_t *)&debug_string,(char *)&str);
  LOG_MSG_INFO1("\nWhitelistedIP: '%s'",str,0,0);
  return child;
}

/*==========================================================
  FUNCTION DeleteDLNAWhitelistingIPEntry
==========================================================*/
/*!
@brief
  Deletes the snat entry on A5 and in mobileap configuration file.

@parameters
  qcmap_cm_port_fwding_entry_conf_t* nat_entry

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

boolean QCMAP_MediaService::DeleteDLNAWhitelistingIPEntry
(
  uint32_t dlna_whitelist_ip,
  qmi_error_type_v01 *qmi_err_num
)
{
  ds_dll_el_t * node = NULL;
  qcmap_nat_entry_list_t* natList = NULL;

  uint32 *curr_dlnawhitelist_entry = NULL;
  qcmap_dlna_whitelist_entry_list_t* dlnaWhitelist = NULL;

  QCMAP_ConnectionManager* QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL, false);
  QCMAP_MediaService *QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);

  struct in_addr addr;
  char tempIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char command[MAX_COMMAND_STR_LEN];

  pugi::xml_document xml_file;
  pugi::xml_node parent, child ;

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  if (QcMapMediaServiceMgr)
  {
    dlnaWhitelist = &(QcMapMediaServiceMgr->dlna_whitelist_config.whitelist_entries);
    node = ds_dll_next(dlnaWhitelist->dlnawhitelistListHead, (void *)&curr_dlnawhitelist_entry);
    while(node && *curr_dlnawhitelist_entry!= dlna_whitelist_ip)
    {
      node = ds_dll_next(node, (void *)&curr_dlnawhitelist_entry);
    }
    if(!node)
    {
      LOG_MSG_ERROR("Invalid DeleteDLNAWhitelistingIPEntry",0,0,0);
      *qmi_err_num = QMI_ERR_INVALID_INDEX_V01;
      return false;
    }
    if ( QcMapMediaServiceMgr->SetDLNAWhitelistRule(&dlna_whitelist_ip,false, qmi_err_num ))
    {
      LOG_MSG_INFO1("Deleted WhitelistIP Rule",0,0,0);
    }
    if (node)
    {
       LOG_MSG_INFO1("FOUND ENTRY TO DELETE",0,0,0);
      ds_dll_delete_node(dlnaWhitelist->dlnawhitelistListHead,
                         &(dlnaWhitelist->dlnawhitelistListTail),
                         node);

      //Free the firewall entry structure
      ds_free (curr_dlnawhitelist_entry);
      curr_dlnawhitelist_entry = NULL;

      //Free the memory of the linked list node
      ds_dll_free(node);
      node = NULL;
    }
    if (dlnaWhitelist->dlnawhitelistListHead->next == NULL)
    {
      LOG_MSG_INFO1("\n No WhitelistIP entry exist\n",0,0,0);
      ds_dll_free(dlnaWhitelist->dlnawhitelistListHead);
      dlnaWhitelist->dlnawhitelistListHead = NULL;
    }

    QcMapMediaServiceMgr->dlna_whitelist_config.num_whitelist_entries--;
  }
  if (child = QCMAP_MediaService::searchDLNAWhitelistIPInXML(&xml_file, &dlna_whitelist_ip))
  {
    parent = child.parent();
    parent.remove_child(child);
    QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
    LOG_MSG_INFO1("Deleted the DLNAWhitelistingIP entry successfully from XML.", 0, 0, 0);
  }
  else
  {
    LOG_MSG_ERROR("DLNAWhitelistingIP - Error in finding the entry",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_INDEX_V01;
    return false;
  }

  return true;
}

/*==========================================================
  FUNCTION ReadDLNAWhitelistIPFromXML
==========================================================*/
/*!
@brief
  Reads the DLNA Whitelist IP from the XML

@parameters
  char *xml_path

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

boolean QCMAP_MediaService::ReadDLNAWhitelistIPFromXML(char *xml_path , bool check)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child, subchild;
  char str[INET6_ADDRSTRLEN];
  uint32_t debug_string =0;
  ds_dll_el_t * node = NULL;
    in_addr addr;
  QCMAP_MediaService *QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
  uint32 *curr_dlnawhitelist_entry = NULL;
  uint32_t dlnawhitelist_entry;
  qcmap_dlna_whitelist_entry_list_t* dlnaWhitelist = NULL;
  if (!xml_file.load_file(xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPSrvcCfg_TAG);
  this->dlna_whitelisting_state = atoi(root.child(DLNAWhitelistingAllow_TAG).child_value());

  dlnaWhitelist = &(QcMapMediaServiceMgr->dlna_whitelist_config.whitelist_entries);
  if (dlnaWhitelist->dlnawhitelistListHead)
  {
    node = ds_dll_next(dlnaWhitelist->dlnawhitelistListHead,(&curr_dlnawhitelist_entry));
    if (node == NULL || curr_dlnawhitelist_entry == NULL)
    {
      LOG_MSG_ERROR("ReadDLNA WHITELIST is NULL Cant fetch  info",0,0,0);
      return false;
    }

    for (child = root.first_child(); child; child = child.next_sibling())
    {
      if (!strcmp(child.name(), WhitelistedIP_TAG))
      {
        subchild=child;
        inet_aton(subchild.child_value(), (in_addr *)&(dlnawhitelist_entry));
        dlnawhitelist_entry = ntohl(dlnawhitelist_entry);
        if (check)
        {
          if (curr_dlnawhitelist_entry)
          {
            if ( dlnawhitelist_entry == *curr_dlnawhitelist_entry)
            {
                /* we already added this one */
                LOG_MSG_ERROR("\nWHITELIST IP already present!!",0,0,0);
            }
            else
            {
              if(this->AddDLNAWhitelistIPToList(&dlnawhitelist_entry) == false)
              {
                LOG_MSG_ERROR("Failed to Add Whitelist IP Entry to List", 0, 0, 0);
                return false;
              }
            }
          }
          else
          {
            LOG_MSG_ERROR("Failed to Add Whitelist IP Entry to List", 0, 0, 0);
            return false;
          }
        }
        else
        {
          if(this->AddDLNAWhitelistIPToList(&dlnawhitelist_entry) == false)
          {
              /* we already added this one */
              LOG_MSG_ERROR("\nWHITELIST IP already present!!",0,0,0);
          }
        }
        node = ds_dll_next(node, (&curr_dlnawhitelist_entry));
      }
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION SetDLNAWhitelistRule
==========================================================================*/
/*!
@brief
  SetDLNAWhitelistRule

@parameters
  qcmap_msgr_firewall_conf_t *firewall
  uint8_t *del_command

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
boolean QCMAP_MediaService::SetDLNAWhitelistRule
(
  uint32_t *dlna_whitelist_ip ,
  boolean add_rule,
  qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_MediaService *QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  char command[MAX_COMMAND_STR_LEN]={0};
  char command_sec[MAX_COMMAND_STR_LEN];
  char tmp[MAX_COMMAND_STR_LEN]={0};
  char str[INET6_ADDRSTRLEN];
  int portno=0;
  struct in_addr addr;
  uint8 next_hdr_prot;
  char tempIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  if ( dlna_whitelist_ip == NULL )
  {
    LOG_MSG_ERROR("NULL Entry\n",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return QCMAP_CM_ERROR;
  }
  /* Convert IP to ASCII format */
  addr.s_addr = htonl(*dlna_whitelist_ip);
  strlcpy(tempIP, inet_ntoa(addr), sizeof(tempIP));
  QcMapMediaServiceMgr->GetDLNAPortNumber(&portno,qmi_err_num);
   /*In comming traffic from wwan is handled in PREROUTING chain in mangle table */
   if ( add_rule )
   {
       snprintf( command, MAX_COMMAND_STR_LEN,
            "iptables -I INPUT -p tcp -s %s -i %s --dport %d -j ACCEPT",
            tempIP,BRIDGE_IFACE,portno );
   }
   else
   {
       snprintf( command, MAX_COMMAND_STR_LEN,
            "iptables -D INPUT -p tcp -s %s -i %s --dport %d -j ACCEPT",
            tempIP,BRIDGE_IFACE,portno);
   }
   ds_system_call(command, strlen(command));
   LOG_MSG_INFO1("SET DLNA Whitelist flag = %d",
                 QcMapMediaServiceMgr->dlna_whitelisting_state,0,0);
   return QCMAP_CM_SUCCESS;
}

/*===========================================================================
  FUNCTION GetDLNAPortNumber
==========================================================================*/
/*!
@brief
  Gets the DLNA port nol.

@parameters
  int *portno,
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

boolean QCMAP_MediaService::GetDLNAPortNumber
(
  int *portno,
  qmi_error_type_v01 *qmi_err_num
)
{
  FILE *file = NULL;
  char temp[MAX_SCAN_SIZE] = "";
  file = fopen("/etc/minidlna.conf", "rt");
  if (file == NULL)
  {
    LOG_MSG_ERROR("Get DLNAPortNumber unable to find /etc/minidlna.conf!", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INSUFFICIENT_RESOURCES_V01;
    return false;
  }

  while (fgets(temp, MAX_SCAN_SIZE, file) != NULL)
  {
    if (strstr(temp, "port=") != NULL && temp[0] != '#')
    {
      *portno = atoi(&temp[5]);
      fclose(file);
      return true;
    }
  }
  fclose(file);
  *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
  return false;
}

