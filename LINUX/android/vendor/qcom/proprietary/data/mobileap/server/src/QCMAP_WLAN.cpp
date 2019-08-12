/*====================================================

FILE:  QCMAP_WLAN.cpp

SERVICES:
   QCMAP Connection Manager WLAN Specific Implementation

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
#include "qti_wlan_scm_msgr_v01.h"

#include "QCMAP_WLAN.h"
#include "QCMAP_ConnectionManager.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_WLAN_SCM.h"
#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_LAN.h"
#include "QCMAP_MediaService.h"
#include "QCMAP_Firewall.h"
#include "QCMAP_Tethering.h"
#include "QCMAP_NATALG.h"
#include "ds_qmi_qcmap_msgr.h"

#define GUEST_AP_IFACE_LL_ADDR "169.254.2.1"

#define HOSTAPD_PID_FILE "/etc/hostapd_ssid1.pid"
#define AP_IFACE_LL_ADDR       "169.254.1.1"
#define STA_INTERFACE_CTRL_PATH "/usr/bin/QCMAP_StaInterface"
#define QCMAP_MULTIPLE_SYSTEM_CALL_FILE "/etc/qcmap_multiple_system_call.sh"
#define WLAN_INI_CFG_FILE "/lib/firmware/wlan/qca_cld/WCNSS_qcom_cfg.ini"

#define HOSTAPD_SSID2_PID_FILE "/etc/hostapd_ssid2.pid"
#define STA_ONLY_MODE_SUPPLICANT_FILE "/tmp/wpa_supplicant.conf"

bool QCMAP_WLAN::flag= false;
QCMAP_WLAN* QCMAP_WLAN::object=NULL;
bool QCMAP_WLAN::priv_client_restart_wlan=false;

/*=====================================================================
  FUNCTION CheckAPStatus
======================================================================*/
/*!
@brief
  Return true if wlan AP interface started beaconing

@parameters
  int iface

@return
  true  - AP is beaconing
  false - AP is not beaconing

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
static int CheckAPStatus(int iface)
{
  char cmd_res_buf[MAX_COMMAND_STR_LEN] = {0}, cmd_buf[MAX_CMD_SIZE] = {0};
  int ptr = 0, ap_status = 0, cmd_res_len = 0;
  FILE *cmd_pipe;

  snprintf(cmd_buf, MAX_CMD_SIZE, "(iwpriv wlan%d getchannel 2>&1)", iface);
  cmd_pipe = popen(cmd_buf, "r");
  if(cmd_pipe) {
    fgets(cmd_res_buf, MAX_COMMAND_STR_LEN, cmd_pipe);
    cmd_res_len = strlen(cmd_res_buf);
    pclose(cmd_pipe);
  }

  if (cmd_res_buf && cmd_res_len < MAX_COMMAND_STR_LEN) {
    char *a = cmd_res_buf;
    while(*a != ':' && ptr < cmd_res_len) {
      a++; ptr++;
    }
    if (*a == ':' && *(a+1) >= '0' && *(a+1) <= '9')
      ap_status = atoi(&cmd_res_buf[ptr+1]);
  }

  return (ap_status == 0 ? 0 : 1);
}

/*=====================================================================
  FUNCTION KillHostapd
======================================================================*/
/*!
@brief
  Kill hostapd and wait for driver cleanup

@parameters
  int pid, wlan interface index

@return
  None

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/

static void KillHostapd(int pid, int wlan_dev_index)
{

  char command[MAX_COMMAND_STR_LEN];
  int retry = 0;

  if (pid != 0) {
    snprintf(command, MAX_COMMAND_STR_LEN, "kill %d", pid);
    ds_system_call(command, strlen(command));
  }
  /* Though hostapd application has terminated, the wlan driver is not yet
   * done with AP Stop and cleanup operation. Check before exit
   */
  /* Check Interface stop */
  while (CheckAPStatus(wlan_dev_index) == 1 &&
           retry < QCMAP_HOSTAPD_END_POLL_MAX_COUNT) {
    retry++;
    usleep(QCMAP_HOSTAPD_POLL_DELAY_MS);
  }
  /* Wait for 100ms interface cleanup */
  usleep(QCMAP_HOSTAPD_POLL_DELAY_MS * 5);
  snprintf(command, MAX_COMMAND_STR_LEN,
         "echo \"QCMAP: Wlan%d Hostapd kill complete\" > /dev/kmsg",
          wlan_dev_index);

  ds_system_call(command, strlen(command));

}


QCMAP_WLAN* QCMAP_WLAN::Get_Instance(boolean obj_create)
{
  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : WLAN",0, 0, 0);
    object = new QCMAP_WLAN();
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

QCMAP_WLAN::QCMAP_WLAN()
{
  QCMAP_CM_LOG_FUNC_ENTRY();
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  memset(&this->wlan_cfg,0,sizeof(this->wlan_cfg));
  /*Convert string to lower case, to keep the comparision uniform */
  this->wifi_mode = QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
  this->activate_wlan_in_progress = false;
  this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_V01;
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile = QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].ll_ip_addr = ntohl(inet_addr(AP_IFACE_LL_ADDR));
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_ip_addr = ntohl(inet_addr(GUEST_AP_IFACE_LL_ADDR));
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].ll_subnet_mask = ntohl(inet_addr(LL_SUBNET_MASK));
  this->prev_guest_profile = QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01;
  this->debug_config.supplicant_debug = false;
  this->debug_config.hostapd_debug= false;
  this->debug_config.hostapd_guest_debug= false;

  this->is_sta_only_mode_enabled = false;
  memset(&this->current_wlan_status, 0, sizeof(this->current_wlan_status));

  if (QcMapMgr->xml_path)
  {
    if (this->ReadWLANConfigFromXML())
    {

      return;
    }
    /* Read Config from XML failed. Use defaults. */
    LOG_MSG_INFO1("QCMAP Read XML failed.",0,0,0);
  }
  //after xml read
  this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].path_to_hostapd_conf, HOSTAPD_CFG_PATH, QCMAP_CM_MAX_FILE_LEN);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].path_to_sta_mode_hostapd_conf, STA_MODE_HOSTAPD_CFG_PATH, QCMAP_CM_MAX_FILE_LEN);

  return;
}

QCMAP_WLAN::~QCMAP_WLAN()
{
  flag=false;
  object=NULL;
  LOG_MSG_INFO1("Destroying Object: WLAN",0,0,0);
}

/*=====================================================================
  FUNCTION AssociateWLANtoBridge
======================================================================*/
/*!
@brief
  This fuction will associate wlan with bridge

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
/*====================================================================*/
boolean QCMAP_WLAN::AssociateWLANtoBridge(int wlan_index)
{
  char command[MAX_COMMAND_STR_LEN];
  int ret;
  int retry = 0;

  snprintf(command, MAX_COMMAND_STR_LEN, "brctl addif %s wlan%d",
           BRIDGE_IFACE, wlan_index);

  while(retry < QCMAP_ASSOC_WLAN_TO_BRIDGE_MAX_RETRIES)
  {
    ret = system(command);
    if(ret < 0)
    {
      LOG_MSG_ERROR("system: bridge wlan association failed ", 0, 0, 0);
    }
    else
    {
      if( WIFEXITED(ret) && (WEXITSTATUS(ret) == 0))
      {
        LOG_MSG_INFO1("system: bridge wlan association passed ", 0, 0, 0);
        return true;
      }
    }
    LOG_MSG_ERROR("system: Retry bridge wlan association", 0, 0, 0);
    usleep(QCMAP_ASSOC_WLAN_TO_BRIDGE_TIMEOUT_US);
    retry++;
  }
  return false;
}

/*=====================================================================
  FUNCTION DisAssociateWLANtoBridge
======================================================================*/
/*!
@brief
  This fuction will disassociate wlan with bridge

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
/*====================================================================*/
void DisAssociateWLANtoBridge(int wlan_index)
{
  char command[MAX_COMMAND_STR_LEN];
  snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
           BRIDGE_IFACE, wlan_index);
  ds_system_call(command, strlen(command));
}


/*=====================================================
  FUNCTION hostapdclicmd
======================================================*/
/*!
@brief
 This function execute hostapd_cli commands, if the command fails it will retry
@parameters
  - qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type

@return
  void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean hostapdclicmd(int dev)
{
  char command[MAX_COMMAND_STR_LEN];
  int retry;
  int ret;
  snprintf(command,
           MAX_COMMAND_STR_LEN,
           "hostapd_cli -i wlan%d -p /var/run/hostapd -B -a %s",
           dev, STA_INTERFACE_CTRL_PATH);
  retry = 0;
  while(retry < QCMAP_HOSTAPDCLI_MAX_RETRIES)
  {
    ret = system(command);
    if(ret < 0)
    {
      LOG_MSG_ERROR("system: hostapd_cli failed ", 0, 0, 0);
    }
    else
    {
      if( WIFEXITED(ret) && (WEXITSTATUS(ret) == 0))
      {
        LOG_MSG_INFO1("system: hostapd_cli passed ", 0, 0, 0);
        return true;
      }
    }

    usleep(QCMAP_HOSTAPDCLI_TIMEOUT_US);
    retry++;
  }
  return false;
}

/*Only need to fillin the info during Enable since obj is deleted on Disable*/
static void FillinWLANStatusIND(void)
{
  QCMAP_ConnectionManager *QcMapMgr = QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  int i = 0;

  if(!QcMapWLANMgr)
    return;

  QcMapWLANMgr->current_wlan_status.wlan_mode =  QcMapWLANMgr->wifi_mode;
  snprintf(QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_name,
           QCMAP_MSGR_DEVICE_NAME_MAX_V01, "wlan%d", QcMapMgr->ap_dev_num1);
  snprintf(QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_name,
           QCMAP_MSGR_DEVICE_NAME_MAX_V01, "wlan%d", QcMapMgr->ap_dev_num2);

  if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_V01)
  {
    QcMapWLANMgr->current_wlan_status.wlan_state_len = 1;
    QcMapWLANMgr->current_wlan_status.wlan_status =
                                          QCMAP_MSGR_WLAN_ENABLED_V01;
    /* WLAN State */
    QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                       QCMAP_MSGR_WLAN_CONNECTED_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                                 QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_type =
                             QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
  }
  else if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01)
  {
    QcMapWLANMgr->current_wlan_status.wlan_state_len = 2;
    QcMapWLANMgr->current_wlan_status.wlan_status =
                                          QCMAP_MSGR_WLAN_ENABLED_V01;
    /* WLAN State */
    QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                       QCMAP_MSGR_WLAN_CONNECTED_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                                 QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_type =
                             QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_state =
                                       QCMAP_MSGR_WLAN_CONNECTED_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[1].ip_type =
                                 QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_type =
                             QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;
  }
  else if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
  {
    QcMapWLANMgr->current_wlan_status.wlan_state_len = 2;
    QcMapWLANMgr->current_wlan_status.wlan_status =
                                          QCMAP_MSGR_WLAN_ENABLED_V01;
    /* WLAN State */
    /* STA State */
    QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                       QCMAP_MSGR_WLAN_DISCONNECTED_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[0].ip_type =
                                 QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_type =
                             QCMAP_MSGR_WLAN_IFACE_TYPE_STA_V01;
    /* AP State */
    QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_state =
                                       QCMAP_MSGR_WLAN_CONNECTED_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[1].ip_type =
                                 QCMAP_MSGR_IP_FAMILY_ENUM_MIN_ENUM_VAL_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_type =
                             QCMAP_MSGR_WLAN_IFACE_TYPE_AP_V01;

  }
}

/*===========================================================================
FUNCTION  EnableWLAN
==========================================================================*/
/*!
@brief
Enables the wlan based on the wlan chip and  mode configured.

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

boolean QCMAP_WLAN::EnableWLAN( qmi_error_type_v01 *qmi_err_num,
                         boolean enable_wlan_onbootup,
                         boolean privileged_client )
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_MediaService* QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
  qcmap_msgr_wlan_mode_enum_v01 mode;
  qcmap_media_svc_status dlna_mode=MEDIA_SVC_MODE_DOWN;
  qcmap_media_svc_status upnp_mode=MEDIA_SVC_MODE_DOWN;
  QCMAP_Tethering *QcMapTetheringMgr = QCMAP_Tethering::Get_Instance(false);
  qcmap_msgr_station_mode_config_v01 station_config;
  int qmi_err_num_internal;
  char command[MAX_COMMAND_STR_LEN];

  char devmode[16] = { '\0' };
  QCMAP_CM_LOG_FUNC_ENTRY();

  ds_system_call("echo QCMAP:Start WLAN Enable > /dev/kmsg",\
  strlen("echo QCMAP:Start WLAN Enable > /dev/kmsg"));

  if( privileged_client && !QCMAP_WLAN::priv_client_restart_wlan)
  {
    LOG_MSG_ERROR("Privileged, not privileged restart, can't enable wlan", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  /* If wlan is enabled during bootup, we don't need to check mobileap state*/
  if ( !enable_wlan_onbootup && QcMapMgr->qcmap_cm_handle <= 0 )
  {
    LOG_MSG_ERROR("MobileAP not enabled", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  /* If STA Only Mode is enabled; disable it if we are not in Dual wifi mode
     otherwise SCM will handle it*/
  if (QcMapWLANMgr && QcMapWLANMgr->is_sta_only_mode_enabled &&
      !QcMapMgr->dual_wifi_mode)
  {
    /* Kill wpa_supplicant gracefully*/
    snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_supplicant" );
    ds_system_call(command, strlen(command));

    if (!QCMAP_Backhaul_WLAN::IsWpaSupplicantkilled())
    {
      /* Kill wpa_supplicant forcefully. */
      snprintf( command, MAX_COMMAND_STR_LEN, "killall -9 wpa_supplicant" );
      ds_system_call(command, strlen(command));
    }

    /* Remove Temp Wpa_suplicant File */
    snprintf(command, MAX_COMMAND_STR_LEN, "rm -rf %s", STA_ONLY_MODE_SUPPLICANT_FILE);
    ds_system_call(command, strlen(command));

    /* Disable WLAN */
    LOG_MSG_INFO1("Disable WLAN in STA Only Mode",0,0,0);
    snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan stop");
    ds_system_call(command, strlen(command));

    QcMapWLANMgr->is_sta_only_mode_enabled = false;
  }

  if (QcMapWLANMgr && QcMapWLANMgr->wifi_mode >= QCMAP_MSGR_WLAN_MODE_AP_V01 &&
  QcMapWLANMgr->wifi_mode <= QCMAP_MSGR_WLAN_MODE_AP_STA_V01 )
  {
    LOG_MSG_ERROR("WLAN is already enabled. Current Mode:- %d",QcMapWLANMgr->wifi_mode ,0, 0);
    return true;
  }

  /* STA mode is tied to mobile ap handle as all the NAT configuration is
  * tied to mobile ap handle. */

  QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_WLAN_MODE,GET_VALUE,(uint32*)&mode);
  if ( (mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ) && (QcMapMgr->qcmap_tear_down_in_progress ))
  {
    LOG_MSG_ERROR("Cannot enable STA mode when mobileap is in not enabled.", 0, 0, 0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  memset(&station_config,0,sizeof(qcmap_msgr_station_mode_config_v01));
  QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(GET_VALUE,&station_config);

  if (station_config.ap_sta_bridge_mode &&
      mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 &&
      (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_CRADLE ||
      ((NULL != QcMapBackhaulCradleMgr) && QcMapBackhaulCradleMgr->cradle_connect_in_progress)))
  {
    LOG_MSG_ERROR("Cannot enable AP-STA Bridge mode when in Cradle.", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  QcMapWLANMgr=QCMAP_WLAN::Get_Instance(true);//Creating WLAN object here

  /* Intialize. */
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].enable = false;
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_STATION_INDEX].enable = false;

  /* Set STA iface index in interface array in lan_cfg */
  QcMapMgr->sta_iface_index = 0;
  /* Set AP device index default values. */
  /* In dual wifi mode this may be changed in EnableWLANModule */
  /* Set defaul AP nums for legacy mode */
  QcMapMgr->ap_dev_num1 = 0;
  QcMapMgr->ap_dev_num2 = 1;

  /* Verify that the devmode is supported. Allowed ones are:
  AP, AP+AP, AP+STA. */
  switch (QcMapWLANMgr->wlan_cfg.wlan_mode )
  {
    case QCMAP_MSGR_WLAN_MODE_AP_V01:
    {
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
      strlcpy(devmode, "ap", sizeof(devmode));
      break;
    }
    case QCMAP_MSGR_WLAN_MODE_AP_AP_V01:
    {
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].enable = true;
      strlcpy(devmode, "ap,ap", sizeof(devmode));
      break;
    }
    case QCMAP_MSGR_WLAN_MODE_AP_STA_V01:
    {
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_STATION_INDEX].enable = true;
      QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(true);
      /* The backhaul instance might have changed, get it again here */
      QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
      strlcpy(devmode, "sta,ap", sizeof(devmode));

      /* In dual wifi mode this may be changed in EnableWLANModule */
      QcMapMgr->ap_dev_num1 = 1;

      /* Disable WLAN-IPA interface if AP+STA Bridge Configuration is Active */
      if (QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_mode &&
          (QcMapMgr->revert_ap_sta_router_mode == false))
      {
        /*Take Backup of the WLAN Ini File before modification*/
        snprintf(command, MAX_COMMAND_STR_LEN,"cp %s %s_bak",WLAN_INI_CFG_FILE, WLAN_INI_CFG_FILE);
        ds_system_call(command, strlen(command));
        /*Modify the INI file*/
        snprintf(command, MAX_COMMAND_STR_LEN,"sed -i '/gIPAConfig=/c\gIPAConfig=0x00' %s",WLAN_INI_CFG_FILE);
        ds_system_call(command, strlen(command));
        snprintf(command, MAX_COMMAND_STR_LEN,"sed -i '/IpaUcOffloadEnabled=/c\IpaUcOffloadEnabled=0' %s",WLAN_INI_CFG_FILE);
        ds_system_call(command, strlen(command));
        snprintf(command, MAX_COMMAND_STR_LEN,"sed -i '/gIpaUcStaOffload=/c\gIpaUcStaOffload=0' %s",WLAN_INI_CFG_FILE);
        ds_system_call(command, strlen(command));
        QcMapBackhaulWLANMgr->ap_sta_bridge_mode_activated = true;

        /* Disable DUN and ETH Tethering in AP+STA bridging. */
        if ( QcMapTetheringMgr != NULL )
        {
          if (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_enabled &&
             (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX1].link_type ==
                                         QCMAP_QTI_TETHERED_LINK_PPP))
          {
            QCMAP_Tethering::BringDownTetheredLink(QCMAP_QTI_TETHERED_LINK_PPP,
                                                   (void *)NULL,
                                                   &qmi_err_num_internal);
          }
          if ((QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_enabled &&
              (QcMapTetheringMgr->tethered_conf[QCMAP_MSGR_TETH_LINK_INDEX2].link_type ==
                          QCMAP_QTI_TETHERED_LINK_ETH)))
          {
            QCMAP_Tethering::BringDownTetheredLink(QCMAP_QTI_TETHERED_LINK_ETH,
                                                   (void *)NULL,
                                                   &qmi_err_num_internal);
          }
        }
      }
      break;
    }
    default:
    {
      /* Incorrect WLAN Mode. Restore it to default mode and continue. */
      LOG_MSG_ERROR("Incorrect WLAN Mode: %d, Start with default Mode.",QcMapWLANMgr->wlan_cfg.wlan_mode, 0, 0);
      QcMapWLANMgr->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_V01;
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = true;
      strlcpy(devmode, "ap", sizeof(devmode));
      break;
    }
  }

  /* Update Current WIFI mode. */
  QcMapWLANMgr->wifi_mode = QcMapWLANMgr->wlan_cfg.wlan_mode;

  /* Reset the revert ap_sta router mode flag to ensure that next Enable/Disable/Bootup
     AP-STA is started with bridge mode*/
  QcMapMgr->revert_ap_sta_router_mode = false;

  LOG_MSG_INFO1( "ap_dev_num1: %d",QcMapMgr->ap_dev_num1, 0, 0);

  // Enable the wlan module
  if(!QcMapWLANMgr->EnableWLANModule(devmode,enable_wlan_onbootup))
  {
    LOG_MSG_ERROR("Cannot enable WLAN Module.", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }


  LOG_MSG_INFO1( "ap_dev_num1 After EnableWLANModule: %d",QcMapMgr->ap_dev_num1, 0, 0);

  if(!QcMapWLANMgr->activate_wlan_in_progress && !QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    if(QcMapMediaServiceMgr)
    {
      if (QcMapMediaServiceMgr->srvc_cfg.dlna_config)
        dlna_mode = MEDIA_SVC_MODE_UP;
      if (QcMapMediaServiceMgr->srvc_cfg.upnp_config)
        upnp_mode = MEDIA_SVC_MODE_UP;
    }
    else
    {
      QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_DLNA,GET_VALUE, &dlna_mode);
      QCMAP_MediaService::GetSetServiceConfigFromXML(CONFIG_UPNP,GET_VALUE, &upnp_mode);
    }
    if (dlna_mode == MEDIA_SVC_MODE_UP)
    {
      if(!QCMAP_MediaService::EnableDLNA(qmi_err_num))
      {
        LOG_MSG_ERROR("Cannot enable DLNA, error: %d.", *qmi_err_num, 0, 0);
      }
    }
    if (upnp_mode == MEDIA_SVC_MODE_UP)
    {
      if(!QCMAP_MediaService::EnableUPNP(qmi_err_num))
      {
        LOG_MSG_ERROR("Cannot enable UPnP, error: %d.", *qmi_err_num, 0, 0);
      }
    }
  }

  /* Reset priviledge client flag. Priviledge client needs to first disable
  * wlan, inorder to request enable wlan */
  if( privileged_client )
  {
    QCMAP_WLAN::priv_client_restart_wlan = false;
    LOG_MSG_INFO1("EnableWLAN is called from a privilaged client",0,0,0);
  }
  ds_system_call("echo QCMAP:WLAN Enabled > /dev/kmsg",
          strlen("echo QCMAP:WLAN Enabled > /dev/kmsg"));

  /* Fill in the WLAN Status IND Struct and Send IND to Clients*/
  FillinWLANStatusIND();
  QcMapWLANMgr->SendWLANStatusIND();

  if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == true)
    QcMapBackhaulMgr->StopRadish();

  return true;
}

/*===========================================================================
FUNCTION  EnableWLANModule
==========================================================================*/
/*!
@brief
This function is invoked, to bringup the wlan module.
This function will load the appropriate driver and brings up the wlan in
the mode configured. Enables dhcpd service for the wlan interfaces.

@return
void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/

boolean QCMAP_WLAN::EnableWLANModule
(
char *wlanmode,
boolean enable_wlan_onbootup
)
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  char devmode[16] = { '\0' };
  int devnum = 0;
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char gw_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int s, i, qcmap_cm_error, dev;
  struct ifreq buffer;
  uint32_t time_out = (WIFI_DEV_INIT_DELAYS_MAX/WIFI_DEV_INIT_DELAY);
  int j = 0;
  int ret = 0;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);

  strlcpy(devmode,wlanmode,sizeof(devmode));

  if (strncmp("ap", wlanmode, strlen(wlanmode)) == 0)
    devnum = SCM_MSGR_WLAN_CFG_1_SAP_V01;
  else if (strncmp("sta,ap", wlanmode, strlen(wlanmode)) == 0)
  {
    if (QcMapBackhaulWLANMgr->ap_sta_bridge_mode_activated)
      devnum = SCM_MSGR_WLAN_CFG_1_SAP_1_STA_BRIDGE_MODE_V01;
    else
      devnum = SCM_MSGR_WLAN_CFG_1_SAP_1_STA_ROUTER_MODE_V01;
  }
  else if (strncmp("ap,ap", wlanmode, strlen(wlanmode)) == 0)
    devnum = SCM_MSGR_WLAN_CFG_2_SAP_V01;

  /* This function stops all currently running HostAPD daemons.
  * Since hostapd is not running on bootup, to improve the wlan
  * service availablity, we are spending time stopping hostapd */
  /* In Dual WiFi SCM is in charge of hostapd */
  if(!enable_wlan_onbootup && !QcMapMgr->dual_wifi_mode)
  {
    QcMapWLANMgr->StopHostAPD();
  }

  /* Enable IP forwarding */
  ds_system_call("echo 1 > /proc/sys/net/ipv4/ip_forward",
  strlen("echo 1 > /proc/sys/net/ipv4/ip_forward"));

  if (!QcMapMgr->dual_wifi_mode)/* Legacy mode */
  {
    /* Run the WLAN Script to load the WLAN driver. */
    snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan start %s",
      devmode);
    LOG_MSG_INFO1("Bringup WLAN",0,0,0);

    ds_system_call(command, strlen(command));

    for ( i = 0, dev = 0; i < QCMAP_MAX_NUM_INTF; i++ )
    {
      if ( !this->wlan_cfg.interface[i].enable )
        continue;
      snprintf(command, MAX_COMMAND_STR_LEN, "/proc/sys/net/ipv4/conf/wlan%d",dev++);
      while ( j++ < time_out )
      {
        std::ifstream ifile( command );
        if ( ifile )
        {
          break;
        }
        usleep( WIFI_DEV_INIT_DELAY );
      }
    }

    LOG_MSG_INFO1("Wlan card brought up in mode %d.", this->wifi_mode,0,0);
    /* Bring up interfaces.
    This part needs to be done only for AP interfaces.
    Proceed only upto STA iface index since STA is always last in the
    list (ap; ap,ap; ap,sta; ap,ap,sta supported). */
    /* AP2 num is guaranteed to be higher than AP1 */
    for ( i=0, dev = QcMapMgr->ap_dev_num1; i <= QcMapMgr->ap_dev_num2;i++,dev++)
    {
      if ( !this->wlan_cfg.interface[i].enable )
        break;
      /* Start the HostAP daemon. */
      this->StartHostAPD(i, dev);

      /* Add the interfaces to the bridge. */
      if(!this->AssociateWLANtoBridge(dev))
        LOG_MSG_ERROR("Failed to associate wlan%d to bridge", dev,0,0);

      /* Config WLAN interface IP */
      addr.s_addr = htonl(this->wlan_cfg.interface[i].ll_ip_addr);
      strlcpy(a5_ip, inet_ntoa(addr), 16);
      addr.s_addr = htonl(this->wlan_cfg
                    .interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(netmask, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig wlan%d %s netmask %s up",
                dev, a5_ip, netmask);
      ds_system_call(command, strlen(command));
      addr.s_addr = htonl(this->wlan_cfg.interface[i].ll_ip_addr
       & this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(subnet, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s \
                dev wlan%d", subnet, netmask, dev);
      ds_system_call(command, strlen(command));
      /* First delete the link-local route. */
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev wlan%d",
      dev);
      ds_system_call(command, strlen(command));
    }
  }
  else /* Dual wifi mode */
  {
    if(!QcMapWLANSCMMgr->SCMEnableWLAN(devnum))
    {
      LOG_MSG_ERROR("SCM Enable Failed", 0, 0, 0);
      return false;
    }
  }

  /* Enable Station Mode. */
  if(NULL != QcMapBackhaulWLANMgr)
    QcMapBackhaulWLANMgr->EnableStaMode();

  /* Install access restrictions for the Second SSID if the profile is
  INTERNETONLY.*/
  if (this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01)
  {
    if (this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile ==
       QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
    {
      /* Disable InterBSS in WLAN Driver.. */
      this->InstallGuestAPAccessRules();
    }
    else
    {
      this->SetIPAWLANMode(QCMAP_MSGR_PROFILE_FULL_ACCESS_V01);
    }
  }

  // Store the current active configuration in the backup DS
  this->prev_guest_profile = this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile;
  if(NULL != QcMapBackhaulWLANMgr)
  {
    QcMapBackhaulWLANMgr->prev_station_mode_config.conn_type =
      QcMapBackhaulWLANMgr->apsta_cfg.conn_type;
    memcpy(&QcMapBackhaulWLANMgr->prev_station_mode_config.static_ip_config,
    &QcMapBackhaulWLANMgr->apsta_cfg.static_ip_config,
      sizeof(qcmap_msgr_sta_static_ip_config_v01));
    QcMapBackhaulWLANMgr->prev_ap_sta_bridge_mode =QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_mode;
  }

  return true;
}

/*=====================================================================
FUNCTION DisableWLAN
======================================================================*/
/*!
@brief
Brings down the wlan and stops the dhcp server.

@return
true  - on success
false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*===================================================================*/

boolean QCMAP_WLAN::DisableWLAN( qmi_error_type_v01 *qmi_err_num,
                          boolean privileged_client )
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error;
  int err_num;
  qcmap_msgr_wlan_mode_enum_v01     prev_wifi_mode;
  qcmap_msgr_device_type_enum_v01 device_type =
    QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_Firewall* QcMapFirewall=QCMAP_Firewall::Get_Instance(false);
  QCMAP_NATALG* QcMapNatAlg=QCMAP_NATALG::Get_Instance(false);
  QCMAP_MediaService* QcMapMediaServiceMgr=QCMAP_MediaService::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);


  /* Check iface management if drivers are unloaded first */


  qcmap_addr_info_list_t* addrList = &(QcMapMgr->addrList);
  qcmap_addr_info_list_t* addrList_prev = &(QcMapMgr->addrList_prev);
  ds_dll_el_t * node = NULL;
  ds_dll_el_t * nodeF = NULL;
  qcmap_cm_client_data_info_t* connectedDevicesList = NULL;

  if (!QcMapWLANMgr || QcMapWLANMgr->wifi_mode ==
       QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 )
  {
    LOG_MSG_ERROR("WLAN is already disabled.",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return true;
  }
  //From here on QCMapWLANMgr object is not null and hence can be used without null checks

  /* Stop services before bringing down bridge so byebye messages are sent */
  if( !(QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1)) &&
      !(QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX2)) &&
      !QcMapWLANMgr->activate_wlan_in_progress)
  {
    if(QcMapMediaServiceMgr)
    {
      if(QcMapMediaServiceMgr->srvc_cfg.upnp_config)
      {
        LOG_MSG_INFO1("Interim disable UPnP daemon", 0, 0, 0);
        QcMapMediaServiceMgr->InterimDisableUPNP(qmi_err_num);
      }
      if(QcMapMediaServiceMgr->srvc_cfg.dlna_config)
      {
        LOG_MSG_INFO1("Interim disable DLNA daemon", 0, 0, 0);
        QcMapMediaServiceMgr->InterimDisableDLNA(qmi_err_num);
      }
    }
  }

  /* hostapd_cli is needed for CDI in dual wifi and legacy modes */
  /*Stop all instances of hostapd_cli */
  QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_PRIMARY_AND_GUEST_AP_V01);

  if (!QcMapMgr->dual_wifi_mode)
  {
    /* Shut down interfaces.
    Proceed only upto STA iface index since STA is always last in the
    list (ap; ap,ap; ap,sta; ap,ap,sta supported).*/
    QcMapWLANMgr->StopHostAPD();
  }

  /* Disable station mode iface. */
  if(NULL != QcMapBackhaulWLANMgr)
  {
    QcMapBackhaulWLANMgr->DisableStaMode();
  }

  /* Copy connected device list so we can send deprecate RA's if we are
  switching to STA */
  if (addrList->addrListHead != NULL && addrList->addrListHead->next != NULL)
  {
    nodeF = addrList->addrListHead->next;

    if (addrList_prev->addrListHead == NULL )
    {
      /*The first node which is created is a dummy node which does not store any device
          information. This is done to make use of the doubly linked list framework which
          is already existing*/
      if (( node = ds_dll_init(NULL)) == NULL)
      {
        LOG_MSG_ERROR("Error in allocating memory for node",0,0,0);
        *qmi_err_num = QMI_ERR_NO_MEMORY_V01;
        return false;
      }
      addrList_prev->addrListHead = node;
    }

    while (nodeF != NULL)
    {
      //Copy the mac address, interface name in the linked list
      connectedDevicesList = ( qcmap_cm_client_data_info_t* )nodeF->data;
      if ((node = ds_dll_enq(addrList_prev->addrListHead,
               NULL, (void*)connectedDevicesList )) == NULL)
      {
        LOG_MSG_ERROR("Error in adding a node",0,0,0);
        *qmi_err_num = QMI_ERR_NO_MEMORY_V01;
        return false;
      }
      nodeF = nodeF->next;
    }
    addrList_prev->addrListTail = node;
  }

  else
    LOG_MSG_ERROR("list is empty",0,0,0);

  /* Delete all the WLAN clients from the Connected Devices List */
  if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_V01)
  {
    device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
    QcMapMgr->DeleteConnectedDevicesClients(device_type);
  }
  else if (QcMapWLANMgr->wifi_mode ==  QCMAP_MSGR_WLAN_MODE_AP_AP_V01)
  {
    device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
    QcMapMgr->DeleteConnectedDevicesClients(device_type);
    device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
    QcMapMgr->DeleteConnectedDevicesClients(device_type);
  }
  else if (QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
  {
    device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
    QcMapMgr->DeleteConnectedDevicesClients(device_type);
  }

  if(!QcMapWLANMgr->DisableWLANModule())
  {
    LOG_MSG_ERROR("Cannot disable WLAN Module", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  /* AP-STA Bridge Mode cleanup*/
  if (QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    /* Restore the previous WLAN INI file */
    snprintf(command, MAX_COMMAND_STR_LEN,"mv %s_bak %s",WLAN_INI_CFG_FILE, WLAN_INI_CFG_FILE);
    ds_system_call(command, strlen(command));

    QcMapMgr->CleanEbtables();
    QcMapMgr->CleanIPtables();
    /* Enable ETH link*/
    LOG_MSG_ERROR("SetupTetheredLink on ETH iface",0,0,0);
    if (!QCMAP_Tethering::SetupTetheredLink(QCMAP_QTI_TETHERED_LINK_ETH,
         &err_num))
    {
      LOG_MSG_ERROR("SetupTetheredLink failed %d",err_num,0,0);
    }
    QcMapBackhaulMgr->StopDHCPCD(BRIDGE_IFACE);
    QcMapBackhaulMgr->BlockIPv4WWANAccess();//Need to put blocking rules since all have been deleted.
    /*Set accept_ra flag to 1*/
    snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
             BRIDGE_IFACE);
    ds_system_call(command, strlen(command));
    ds_system_call("ip6tables --flush FORWARD",strlen("ip6tables --flush FORWARD")); //need to remove icmp6 rules
    QcMapBackhaulMgr->BlockIPv6WWANAccess();
  }

  /* Save the previous wifi mode. */
  prev_wifi_mode = QcMapWLANMgr->wifi_mode;

  /* Reset wifi mode */
  QcMapWLANMgr->wifi_mode = QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
  /* Reset STA mode specific parameters */
  if(NULL != QcMapBackhaulWLANMgr)
  {
    QcMapBackhaulWLANMgr->sta_v4_available = false;
    QcMapBackhaulWLANMgr->sta_v6_available = false;
    QcMapBackhaulWLANMgr->ap_sta_bridge_mode_activated = false;
    QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
    delete QcMapBackhaulWLANMgr;
  }
  QcMapMgr->sta_iface_index = QCMAP_LAN_INVALID_IFACE_INDEX;

  QcMapBackhaulMgr = QCMAP_Backhaul::Get_Instance(false);

  /* Try switching to backhaul if previous mode is AP-STA. */
  if ((NULL != QcMapBackhaulMgr) &&
      ((QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_ROUTER)
      || (QcMapBackhaulMgr->current_backhaul == BACKHAUL_TYPE_AP_STA_BRIDGE)))
  {
    if ( prev_wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 )
    {
      // Clean up stale entries if previous mode was AP-STA, i.e. WLAN0 was backhaul
      if (QcMapFirewall)
      {
        QcMapFirewall->CleanIPv4MangleTable();
        QcMapFirewall->CleanIPv6MangleTable();
      }
    }

    /* Switch to Cradle/WWAN mode if present */
    if (QCMAP_Backhaul_Cradle::IsCradleWANBridgeActivated() == false)
    {
      QCMAP_Backhaul::SwitchToOtherBackhaul(QcMapBackhaulMgr->current_backhaul,
                                            true,true);
    }
  }

  /* Set all the WLAN enabled boolean values to false */
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].enable = false;
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].enable = false;
  QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_STATION_INDEX].enable = false;

  /* Priviledge client needs to disable wlan and only than, enable wlan is
  * permitted */
  if( privileged_client )
  {
    QCMAP_WLAN::priv_client_restart_wlan = true;
    /* Reset the revert ap_sta_router_mode flag to ensure the for SSR
       we revert back to AP-BRIDGE mode */
    QcMapMgr->revert_ap_sta_router_mode = false;
    LOG_MSG_INFO1("DisableWLAN is called from a privilaged client",0,0,0);
  }

  /* For Activate WLAN Dont Send a Disable IND; other send WLAN IND*/
  if (!QcMapWLANMgr->activate_wlan_in_progress)
  {
    QcMapWLANMgr->current_wlan_status.wlan_status =
                                  QCMAP_MSGR_WLAN_DISABLED_V01;
    QcMapWLANMgr->current_wlan_status.wlan_mode = prev_wifi_mode;
    QcMapWLANMgr->current_wlan_status.wlan_state[0].wlan_iface_state =
                                     QCMAP_MSGR_WLAN_DISCONNECTED_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[1].wlan_iface_state =
                                     QCMAP_MSGR_WLAN_DISCONNECTED_V01;
    QcMapWLANMgr->current_wlan_status.wlan_state[0].ip4_addr = 0;
    QcMapWLANMgr->current_wlan_status.wlan_state[1].ip4_addr = 0;
    memset(QcMapWLANMgr->current_wlan_status.wlan_state[0].ip6_addr, 0,
                sizeof(QcMapWLANMgr->current_wlan_status.wlan_state[0].ip6_addr));
    memset(QcMapWLANMgr->current_wlan_status.wlan_state[1].ip6_addr, 0,
                sizeof(QcMapWLANMgr->current_wlan_status.wlan_state[1].ip6_addr));
    QcMapWLANMgr->SendWLANStatusIND();
  }

  delete QcMapWLANMgr;//deleting wlan  object here
  return true;
}


/*=====================================================================
FUNCTION DisableWLANModule
======================================================================*/
/*!
@brief
Brings down the for wlan chip  and
deletes the related routing informations.

@return
void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::DisableWLANModule(void)
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  int qcmap_cm_error, i;
  boolean usb_enable=false;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  qcmap_msgr_cradle_mode_v01 cradle_mode=QCMAP_MSGR_CRADLE_LAN_ROUTER_V01;
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);

  /*-----------------------------------------------------------------------
  Delete iptables rules.
  -----------------------------------------------------------------------*/
  /* Delete access restrictions for the Second SSID if the profile is
  INTERNETONLY.*/
  if ( this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 &&
  ((this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile ==
    QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01) ||
  (this->prev_guest_profile == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)))
  {
    this->DeleteGuestAPAccessRules();
  }

  /* Delete the interfaces from bridge. */
  snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
  BRIDGE_IFACE, QcMapMgr->ap_dev_num1);
  ds_system_call(command, strlen(command));
  if ( this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 )
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s wlan%d",
    BRIDGE_IFACE, QcMapMgr->ap_dev_num2);
    ds_system_call(command, strlen(command));
  }
  if(QcMapBackhaulCradleMgr)
  {
    cradle_mode = QcMapBackhaulCradleMgr->cradle_cfg.cradle_mode;
  }
  /* If bridge mode is activated Delete sta interface from bridge */
  /* If USB was enabled restart USB*/
  /* Restart DHCP On bridge, assign IP Address to Bridge*/
  if ((NULL != QcMapBackhaulWLANMgr) && QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "brctl delif %s %s",
    BRIDGE_IFACE, QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
    ds_system_call(command, strlen(command));
    if ( QCMAP_Tethering::GetTethLinkEnable(QCMAP_MSGR_TETH_LINK_INDEX1) &&
         !this->IsSSRInProgress() && (cradle_mode != QCMAP_MSGR_CRADLE_WAN_ROUTER_V01) )
    {
      usb_enable=true;
      ds_system_call("echo 0 > /sys/class/android_usb/android0/enable ",
        strlen("echo 0 > /sys/class/android_usb/android0/enable "));
    }

    // Assign IP to bridge
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s down", BRIDGE_IFACE);
    ds_system_call(command, strlen(command));
    addr.s_addr = htonl(QCMAPLANMgr->lan_cfg.apps_ip_addr);
    strlcpy(a5_ip, inet_ntoa(addr), 16);
    addr.s_addr = htonl(QCMAPLANMgr->lan_cfg.sub_net_mask);
    strlcpy(netmask, inet_ntoa(addr), 16);
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig %s %s netmask %s up",
      BRIDGE_IFACE, a5_ip, netmask);
    ds_system_call(command, strlen(command));
    // Restart DHCP Server
    QCMAPLANMgr->StopDHCPD();
    QCMAPLANMgr->StartDHCPD();

    // Assign Original MAC Address to Bridge
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig bridge0 hw ether %s",
               ether_ntoa(&QCMAPLANMgr->bridge_local_mac_addr));
    ds_system_call(command, strlen(command));

    /* Start USB*/
    if (usb_enable)
      ds_system_call("echo 1 > /sys/class/android_usb/android0/enable",
                     strlen("echo 1 > /sys/class/android_usb/android0/enable"));
  }

  if (!QcMapMgr->dual_wifi_mode) //if we change to creating the SCM client at boot, we can always just validate against that client != NULL instead of QcMapMgr var
  {
    snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan stop");
    LOG_MSG_INFO1("Bring down WLAN ",0,0,0);
    ds_system_call(command, strlen(command));
  }
  else if (QcMapMgr->dual_wifi_mode)
  {
    //Call SCM stop WIFI
    if(!QcMapWLANSCMMgr->SCMDisableWLAN())
    {
      LOG_MSG_ERROR("SCM Disable WLAN failed", 0, 0, 0);
      return false;
    }
  }

  return true;
}

/*=====================================================================
FUNCTION SetWLANConfig
======================================================================*/
/*!
@brief
This function will set the wlan configuration.

@return
true	- on Success
false - on Failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::SetWLANConfig
(
qcmap_msgr_wlan_mode_enum_v01 wlan_mode,
qcmap_msgr_access_profile_v01  guest_ap_acess_profile,
qcmap_msgr_station_mode_config_v01 *station_config,
qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  in_addr addr;
  // Initialize QMI error
  *qmi_err_num = QMI_ERR_NONE_V01;
  qcmap_cm_access_profile_type new_cfg_access_profile;
  /* check for address conflicts before setting the wlan configuration */

  if( QCMAPLANMgr->CheckforAddrConflict( NULL,station_config ))
  {
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01;
    return false;
  }
  if(QcMapWLANMgr)
  {
    if ( wlan_mode >= QCMAP_MSGR_WLAN_MODE_AP_V01 &&
    wlan_mode <= QCMAP_MSGR_WLAN_MODE_AP_STA_V01 )
    {
      QcMapWLANMgr->wlan_cfg.wlan_mode = wlan_mode;
    }

    if ( guest_ap_acess_profile != QCMAP_MSGR_ACCESS_PROFILE_MIN_ENUM_VAL_V01)
    {
      QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]
        .access_profile = guest_ap_acess_profile;
    }
  }
  if ( station_config != NULL )
  {
    if(NULL != QcMapBackhaulWLANMgr)
    {
      QcMapBackhaulWLANMgr->apsta_cfg.conn_type = station_config->conn_type;
      if (QcMapBackhaulWLANMgr->apsta_cfg.conn_type ==
           QCMAP_MSGR_STA_CONNECTION_STATIC_V01 )
      {
        QcMapBackhaulWLANMgr->apsta_cfg.static_ip_config =
         station_config->static_ip_config;
      }
      QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_mode =
       station_config->ap_sta_bridge_mode;
    }
    QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(SET_VALUE,station_config);
  }


  /* if it is OK, call write to XML */

  QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_WLAN_MODE,SET_VALUE,
    (uint32 *)&wlan_mode);

  QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_GUEST_ACCESS_PROFILE,SET_VALUE,
    (uint32 *)&guest_ap_acess_profile);

  return true;

}

/*=====================================================================
FUNCTION GetWLANConfig
======================================================================*/
/*!
@brief
This function will get the wlan configuration.

@return
true	- on Success
false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::GetWLANConfig(qcmap_msgr_wlan_mode_enum_v01 *wlan_mode,
                            qcmap_msgr_access_profile_v01 *guest_access_profile,
                            qcmap_msgr_station_mode_config_v01 *station_config,
                            qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);


  if ( wlan_mode == NULL || guest_access_profile == NULL ||
      station_config == NULL )
  {
    LOG_MSG_ERROR("NULL params.",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }
  if(QcMapWLANMgr)
  {
    *wlan_mode = QcMapWLANMgr->wlan_cfg.wlan_mode;
    *guest_access_profile = QcMapWLANMgr->wlan_cfg.
      interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile;
  }
  else
  {
    QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_WLAN_MODE,GET_VALUE,
                                                          (uint32*)(wlan_mode));
    QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_GUEST_ACCESS_PROFILE,GET_VALUE,
                                               (uint32*)(guest_access_profile));
  }
  if(NULL != QcMapBackhaulWLANMgr)
  {
    station_config->static_ip_config =
                               QcMapBackhaulWLANMgr->apsta_cfg.static_ip_config;
    station_config->ap_sta_bridge_mode =
                             QcMapBackhaulWLANMgr->apsta_cfg.ap_sta_bridge_mode;
    station_config->conn_type = QcMapBackhaulWLANMgr->apsta_cfg.conn_type;
  }
  else
  {
    memset(station_config,0,sizeof(qcmap_msgr_station_mode_config_v01));
    QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(GET_VALUE,station_config);
  }
  return true;
}


/*=====================================================================
FUNCTION ActivateWLAN
======================================================================*/
/*!
@brief
Brings up the lan with the latest wlan configuration, wlan mode and access profile.

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
/*====================================================================*/
boolean QCMAP_WLAN::ActivateWLAN
(
qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
  qcmap_msgr_station_mode_config_v01 station_config;


  LOG_MSG_INFO1("QCMAP_WLAN::ActivateWLAN",0,0,0);
  /* STA mode is tied to mobile ap handle as all the NAT configuration is
  * tied to mobile ap handle. */
  if (QcMapWLANMgr)
  {
    wlan_mode = QcMapWLANMgr->wlan_cfg.wlan_mode;
  }
  else
  {
    QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_WLAN_MODE,GET_VALUE,
                                        (uint32*)(&wlan_mode));
  }

  if ((wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ) &&
  ( QcMapMgr->qcmap_tear_down_in_progress || QcMapMgr->qcmap_cm_handle <= 0 ) )
  {
    LOG_MSG_ERROR("Cannot enable STA mode when mobileap is not enabled.",0,0,0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  memset(&station_config,0,sizeof(qcmap_msgr_station_mode_config_v01));
  QCMAP_Backhaul_WLAN::GetSetAPSTAConfigFromXML(GET_VALUE,&station_config);

  if (station_config.ap_sta_bridge_mode &&
      wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 &&
      (QcMapBackhaulCradleMgr && (QcMapBackhaulCradleMgr->cradle_v4_available
      || QcMapBackhaulCradleMgr->cradle_v6_available
      || QcMapBackhaulCradleMgr->cradle_connect_in_progress)))
  {
    LOG_MSG_ERROR("Cannot enable AP-STA Bridge mode when in Cradle.", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  // If Access profile has changed and we are in AP-AP mode or previously
  //we were in ap-ap mode, then, we will delete or add the ebtables rules
  if (QcMapWLANMgr && QcMapWLANMgr->IsGuestProfileUpdated() &&
      (QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_AP_V01))
  {
    QcMapWLANMgr->UpdateAccessProfileRules();
    QcMapWLANMgr->prev_guest_profile = QcMapWLANMgr->wlan_cfg.
     interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile;
  }

  // We need to disable WLAN, re-enable it, if either WLAN MODE or
  //Access Profile or Station Mode config has changed
  if ( (QcMapWLANMgr && QcMapWLANMgr->IsWlanModeUpdated() &&
       (QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01))
       ||(((NULL != QcMapBackhaulWLANMgr) && QcMapBackhaulWLANMgr->IsStaCfgUpdated())&&
       (QcMapWLANMgr && QcMapWLANMgr->wifi_mode  ==
       QCMAP_MSGR_WLAN_MODE_AP_STA_V01))||(((NULL != QcMapBackhaulWLANMgr) &&
       QcMapMgr->revert_ap_sta_router_mode == true) && (QcMapWLANMgr
       && QcMapWLANMgr->wifi_mode  == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)))
  {
    /* Disable WLAN. */
    QcMapWLANMgr->activate_wlan_in_progress = true;
    QCMAP_WLAN::DisableWLAN(qmi_err_num);

    sleep(QcMapMgr->wlan_restart_delay);
  }

  /* Enable WLAN. */
  QCMAP_WLAN::EnableWLAN(qmi_err_num);

  return true;
}


/*=====================================================================
FUNCTION RestartWLAN
======================================================================*/
/*!
@brief
Restarts WLAN.

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
/*====================================================================*/
boolean QCMAP_WLAN::RestartWLAN(void)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  qcmap_msgr_wlan_mode_enum_v01 wlan_mode;
  qmi_error_type_v01 qmi_err_num;

  if (!QcMapWLANMgr)
  {
    LOG_MSG_ERROR("WLAN not enabled.",0,0,0);
    return false;
  }

  LOG_MSG_INFO1("QCMAP_WLAN::RestartWLAN",0,0,0);

  // We need to disable WLAN, re-enable it, if either WLAN MODE or
  //Access Profile or Station Mode config has changed
  if ( QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 )
  {
    LOG_MSG_ERROR("WLAN is in disabled state.",0,0,0);
    return false;
  }

  /* STA mode is tied to mobile ap handle as all the NAT configuration is
  * tied to mobile ap handle. */
  if (QcMapWLANMgr)
  {
    wlan_mode = QcMapWLANMgr->wlan_cfg.wlan_mode;
  }
  else
  {
    QCMAP_WLAN::GetsetWLANConfigFromXML(CONFIG_WLAN_MODE,GET_VALUE,
                                        (uint32*)(&wlan_mode));
  }

  if ((wlan_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 ) &&
  ( QcMapMgr->qcmap_tear_down_in_progress || QcMapMgr->qcmap_cm_handle <= 0 ) )
  {
    LOG_MSG_ERROR("Cannot enable STA mode when mobileap is not enabled.",0,0,0);
    return false;
  }

  /* Disable WLAN. */
  QcMapWLANMgr->activate_wlan_in_progress = true;
  QCMAP_WLAN::DisableWLAN(&qmi_err_num);

  sleep(QcMapMgr->wlan_restart_delay);

  /* Enable WLAN. */
  QCMAP_WLAN::EnableWLAN(&qmi_err_num);

  return true;
}


boolean QCMAP_WLAN::IsGuestProfileUpdated()
{
  return  (this->prev_guest_profile != this->wlan_cfg.
    interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile);
}


/*=====================================================================
FUNCTION GetWLANStatus
======================================================================*/
/*!
@brief
Gets the wlan configured mode.

@parameters
qcmap_msgr_wlan_mode_enum_v01 *wlan_mode

@return
true	- on Success
false - on Failure
@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::GetWLANStatus
(
qcmap_msgr_wlan_mode_enum_v01 *wlan_mode,
qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_Backhaul_WLAN *QcMapWLANBackhaulMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);

  if (wlan_mode == NULL )
  {
    LOG_MSG_ERROR("NULL params.",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_ARG_V01 ;
    return false;
  }
  if(QcMapWLANMgr)
  {
    if (QcMapWLANBackhaulMgr && QcMapWLANBackhaulMgr->IsAPSTABridgeActivated())
      *wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_STA_BRIDGE_V01;
    else
      *wlan_mode = QcMapWLANMgr->wifi_mode;
  }

  return true;
}

/*=====================================================================
FUNCTION ActivateHostapdConfig
======================================================================*/
/*!
@brief
Restarts Hostapd with the latest configuration.

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
/*====================================================================*/
boolean QCMAP_WLAN::ActivateHostapdConfig
(
qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
qcmap_msgr_activate_hostapd_action_enum_v01 action_type,
qmi_error_type_v01 *qmi_err_num
)
{
  FILE *fp = NULL;
  int ap_pid =0, guest_ap_pid = 0;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);


  LOG_MSG_INFO1("QCMAP_WLAN::ActivateHostapdConfig ap_type = %d, action = %d",
    ap_type,action_type,0);

  if (!QcMapWLANMgr || QcMapWLANMgr->wifi_mode ==
        QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01 )
  {
    LOG_MSG_ERROR("Cannot activate hostapd when WLAN is disabled.", 0, 0, 0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if ( ap_type < QCMAP_MSGR_PRIMARY_AP_V01 ||
  ap_type > QCMAP_MSGR_PRIMARY_AND_GUEST_AP_V01 )
  {
    LOG_MSG_ERROR("Cannot activate hostapd incorrect ap_type: %d.",ap_type, 0, 0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if ( action_type < QCMAP_MSGR_HOSTAPD_START_V01 ||
  action_type > QCMAP_MSGR_HOSTAPD_RESTART_V01 )
  {
    LOG_MSG_ERROR("Cannot activate hostapd incorrect action_type: %d.",action_type, 0, 0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if ( ( ap_type == QCMAP_MSGR_PRIMARY_AND_GUEST_AP_V01 ||
  ap_type == QCMAP_MSGR_GUEST_AP_V01 ) &&
  QcMapWLANMgr->wifi_mode != QCMAP_MSGR_WLAN_MODE_AP_AP_V01 )
  {
    LOG_MSG_ERROR("Cannot activate hostapd when AP-AP is not enabled: %d.",QcMapWLANMgr->wifi_mode, 0, 0);
    *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
    return false;
  }

  if (!QcMapMgr->dual_wifi_mode)
  {
    /* Get the Primary AP PID. */
    fp = fopen(HOSTAPD_PID_FILE, "r");
    if ( fp == NULL )
    {
      LOG_MSG_ERROR("Error opening hostapd pid file: %d.", errno, 0, 0);
    }
    else if (fscanf(fp, "%d", &ap_pid) != 1)
    {
      LOG_MSG_ERROR("Error reading hostapd file: %d.", errno, 0, 0);
      ap_pid = 0;
      fclose(fp);
    }
    else
    {
      LOG_MSG_INFO1("Primary AP PID = %d",ap_pid,0,0);
      fclose(fp);
    }

    /* Get the Guest AP PID. */
    if ( QcMapWLANMgr->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_AP_V01 )
    {
      fp = fopen(HOSTAPD_SSID2_PID_FILE, "r");
      if ( fp == NULL )
      {
        LOG_MSG_ERROR("Error opening Guest AP hostapd pid file: %d.",errno,0,0);
      }
      else if (fscanf(fp, "%d", &guest_ap_pid) != 1)
      {
        LOG_MSG_ERROR("Error reading Guest AP hostapd file: %d.", errno, 0, 0);
        guest_ap_pid = 0;
        fclose(fp);
      }
      else
      {
        LOG_MSG_INFO1("Guest AP PID = %d",guest_ap_pid,0,0);
        fclose(fp);
      }
    }
  }

  if ( action_type == QCMAP_MSGR_HOSTAPD_START_V01 )
  {
    return QcMapWLANMgr->ActivateHostapdActionStart(ap_type, ap_pid,
      guest_ap_pid, qmi_err_num);
  }
  else if ( action_type == QCMAP_MSGR_HOSTAPD_STOP_V01 )
  {
    return QcMapWLANMgr->ActivateHostapdActionStop(ap_type, ap_pid,
      guest_ap_pid, qmi_err_num);
  }
  else if ( action_type == QCMAP_MSGR_HOSTAPD_RESTART_V01 )
  {
    return QcMapWLANMgr->ActivateHostapdActionRestart(ap_type, ap_pid,
      guest_ap_pid, qmi_err_num);
  }

  return true;
}


/*=====================================================================
FUNCTION ActivateHostapdActionStart
======================================================================*/
/*!
@brief
Starts Hostapd with the latest configuration.

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
/*====================================================================*/
boolean QCMAP_WLAN::ActivateHostapdActionStart
(
qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
int ap_pid,
int guest_ap_pid,
qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  int i, dev;
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  FILE *fp = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);

  LOG_MSG_INFO1("QCMAP_WLAN::ActivateHostapdActionStart",0,0,0);

  switch ( ap_type )
  {
    case QCMAP_MSGR_PRIMARY_AP_V01:
    {
      /* Start Hostapd for Primary AP. */
      if (!QcMapMgr->dual_wifi_mode)
      {
        if ( ap_pid != 0 )
        {
          LOG_MSG_ERROR("Hostapd already running for Primary AP. PID : %d.",ap_pid, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }
        this->StartHostAPD(QCMAP_MSGR_INTF_AP_INDEX, QcMapMgr->ap_dev_num1);
      }
      else
      {
        //CALL to SCM start Primary AP
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_1_V01, SCM_MSGR_WLAN_IF_START_V01);
      }

      addr.s_addr = htonl(this->wlan_cfg
                   .interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(netmask, inet_ntoa(addr), 16);
      addr.s_addr = htonl(this->wlan_cfg
                   .interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_ip_addr
                   &this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(subnet, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev \
        wlan%d", subnet, netmask, QcMapMgr->ap_dev_num1);
      ds_system_call(command, strlen(command));
      if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num1))
      {
        LOG_MSG_ERROR("AssociateWLANtoBridge failed: %d.",0 , 0, 0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return false;
      }
    }
    break;
    case QCMAP_MSGR_GUEST_AP_V01:
    {
      /* Start Hostapd for GuestAP. */
      if (!QcMapMgr->dual_wifi_mode)
      {
        if ( guest_ap_pid != 0 )
        {
          LOG_MSG_ERROR("Hostapd already running for Guest AP. PID : %d.",guest_ap_pid, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }
        this->StartHostAPD(QCMAP_MSGR_INTF_GUEST_AP_INDEX, QcMapMgr->ap_dev_num1);
      }
      else
      {
        //CALL to SCM start Guest AP
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_2_V01, SCM_MSGR_WLAN_IF_START_V01);
      }

      addr.s_addr = htonl(this->wlan_cfg.
                    interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(netmask, inet_ntoa(addr), 16);
      addr.s_addr = htonl(this->wlan_cfg.
                    interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_ip_addr &this->
                    wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(subnet, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s \
                 dev wlan%d", subnet, netmask, QcMapMgr->ap_dev_num2);
      ds_system_call(command, strlen(command));
      if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num2))
      {
        LOG_MSG_ERROR("AssociateWLANtoBridge failed: %d.",0 , 0, 0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return false;
      }
    }
    break;
    case QCMAP_MSGR_PRIMARY_AND_GUEST_AP_V01:
    {
      if (!QcMapMgr->dual_wifi_mode)
      {
        if ( ap_pid != 0 && guest_ap_pid != 0 )
        {
          LOG_MSG_ERROR("Hostapd already running for Both AP's. PID1 : %d and PID2: %d.",ap_pid, guest_ap_pid, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }

        /* Start HostAP for both the AP's.
        This part needs to be done only for AP interfaces.
        Proceed only upto STA iface index since STA is always last in the
        list (ap; ap,ap; ap,sta; ap,ap,sta supported). */
        for ( i=0, dev = QcMapMgr->ap_dev_num1; i < QcMapMgr->ap_dev_num2;
             i++, dev++ )
        {
          if ( !this->wlan_cfg.interface[i].enable )
            break;
          /* Start only the non-running hostapd process. */
          if ( i == QCMAP_MSGR_INTF_AP_INDEX && ap_pid != 0 )
            continue;
          else if ( i == QCMAP_MSGR_INTF_GUEST_AP_INDEX && guest_ap_pid != 0 )
            continue;
          /* Start the HostAP daemon. */
          this->StartHostAPD(i, dev);
          addr.s_addr = htonl(this->wlan_cfg.
            interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
          strlcpy(netmask, inet_ntoa(addr), 16);
          addr.s_addr = htonl(this->wlan_cfg.interface[i].ll_ip_addr
            & this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
          strlcpy(subnet, inet_ntoa(addr), 16);
          snprintf(command, MAX_COMMAND_STR_LEN,
            "route del -net %s netmask %s dev wlan%d", subnet, netmask, dev);
          ds_system_call(command, strlen(command));
        }
      }
      else
      {
        //CALL to SCM start AP-AP
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_1_V01, SCM_MSGR_WLAN_IF_START_V01);
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_2_V01, SCM_MSGR_WLAN_IF_START_V01);
      }

      if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num1))
      {
        LOG_MSG_ERROR("AssociateWLANtoBridge dev1 failed: %d.",0 , 0, 0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return false;
      }
      if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num2))
      {
        LOG_MSG_ERROR("AssociateWLANtoBridge dev2 failed: %d.",0 , 0, 0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return false;
      }
    }
    break;
  }

  return true;
}


/*=====================================================================
FUNCTION ActivateHostapdActionStop
======================================================================*/
/*!
@brief
Stops Hostapd for the required AP.

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
/*====================================================================*/
boolean QCMAP_WLAN::ActivateHostapdActionStop
(
qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
int ap_pid,
int guest_ap_pid,
qmi_error_type_v01 *qmi_err_num
)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  int i, dev;
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  FILE *fp = NULL;
  qcmap_msgr_device_type_enum_v01 device_type =
    QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;

  LOG_MSG_INFO1("QCMAP_WLAN::ActivateHostapdActionStop",0,0,0);

  switch ( ap_type )
  {
    case QCMAP_MSGR_PRIMARY_AP_V01:
    {
      if ( !QcMapMgr->dual_wifi_mode && ap_pid == 0 )
      {
        LOG_MSG_ERROR("Hostapd already stopped for Primary AP. PID : %d.",ap_pid, 0, 0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
      }
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num1);

      if (!QcMapMgr->dual_wifi_mode)
      {
        this->StopHostapdCli(ap_type);
        /* Stop the hostapd for Primary AP. */
        KillHostapd(ap_pid, QcMapMgr->ap_dev_num1);
        if ( !IsHostapdkilled(guest_ap_pid) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }
      }
      else
      {
        //CALL to SCM stop Primary AP
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_1_V01, SCM_MSGR_WLAN_IF_STOP_V01);
      }

      LOG_MSG_INFO1("Delete CDI information", 0, 0, 0);
      if (this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
      {
        device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
      }
      else
      {
        device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
      }
    }
    break;
    case QCMAP_MSGR_GUEST_AP_V01:
    {
      if ( !QcMapMgr->dual_wifi_mode && guest_ap_pid == 0 )
      {
        LOG_MSG_ERROR("Hostapd already stopped for Guest AP. PID : %d.",guest_ap_pid, 0, 0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
      }
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num2);

      if (!QcMapMgr->dual_wifi_mode)
      {
        this->StopHostapdCli(ap_type);
        /* Stop the hostapd for Guest AP. */
        KillHostapd(guest_ap_pid, QcMapMgr->ap_dev_num2);
        if ( !IsHostapdkilled(ap_pid) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }
      }
      else
      {
        //CALL to SCM stop Guest AP
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_2_V01, SCM_MSGR_WLAN_IF_STOP_V01);
      }

      LOG_MSG_INFO1("Delete CDI information", 0, 0, 0);
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
    }
    break;
    case QCMAP_MSGR_PRIMARY_AND_GUEST_AP_V01:
    {
      if ( !QcMapMgr->dual_wifi_mode && ap_pid == 0 && guest_ap_pid == 0 )
      {
        LOG_MSG_ERROR("Hostapd already stopped for Both AP's",0, 0, 0);
        *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
        return false;
      }
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num1);
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num2);

      if (!QcMapMgr->dual_wifi_mode)
      {
        this->StopHostapdCli(ap_type);
        /* Stop the Hostapd for both the AP's. */
        StopHostAPD();

        if ( !IsHostapdkilled(0) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }
      }
      else
      {
        //CALL to SCM stop AP-AP
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_1_V01, SCM_MSGR_WLAN_IF_STOP_V01);
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_2_V01, SCM_MSGR_WLAN_IF_STOP_V01);
      }

      LOG_MSG_INFO1("Delete CDI information", 0, 0, 0);
      device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
    }
    break;
  }

  return true;
}

/*=====================================================================
FUNCTION ActivateHostapdActionRestart
======================================================================*/
/*!
@brief
Restarts Hostapd with the latest configuration.

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
/*====================================================================*/
boolean QCMAP_WLAN::ActivateHostapdActionRestart
(
qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
int ap_pid,
int guest_ap_pid,
qmi_error_type_v01 *qmi_err_num
)
{
  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  int i, dev;
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  FILE *fp = NULL;
  qcmap_msgr_device_type_enum_v01 device_type =
                                   QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(false);


  LOG_MSG_INFO1("QCMAP_WLAN::ActivateHostapdActionRestart", 0, 0, 0);

  switch ( ap_type )
  {
    case QCMAP_MSGR_PRIMARY_AP_V01:
    {
      /* This function was required, when wlan is configured in ht_capable mode,
          to restart hostapd, wlan interface needs to be disassociate with bridge */
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num1);

      if (!QcMapMgr->dual_wifi_mode)
      {
        this->StopHostapdCli(ap_type);
        /* Stop the process only if it is not already killed. */
        if ( ap_pid != 0 )
        {
          KillHostapd(ap_pid, QcMapMgr->ap_dev_num1);
        }
        if ( !IsHostapdkilled(guest_ap_pid) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }

        /* Sleep so clients will disconnect */
        sleep(QcMapMgr->wlan_restart_delay);
      }

      LOG_MSG_INFO1("Delete CDI information", 0, 0, 0);
      if (this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
      {
        device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type); //why is this present?
      }
      else
      {
        device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
        QcMapMgr->DeleteConnectedDevicesClients(device_type);
      }

      /* Restart Hostapd for Primary AP. */
      if (!QcMapMgr->dual_wifi_mode)
      {
        this->StartHostAPD(QCMAP_MSGR_INTF_AP_INDEX, QcMapMgr->ap_dev_num1);
      }
      else
      {
        //CALL to SCM start Primary AP
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_1_V01, SCM_MSGR_WLAN_IF_RESTART_V01);
      }

      addr.s_addr = htonl(this->wlan_cfg.
       interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(netmask, inet_ntoa(addr), 16);
      addr.s_addr = htonl(this->wlan_cfg.
        interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_ip_addr & this->wlan_cfg.
        interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(subnet, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev \
        wlan%d", subnet, netmask, QcMapMgr->ap_dev_num1);
      ds_system_call(command, strlen(command));
      if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num1))
      {
        LOG_MSG_ERROR("AssociateWLANtoBridge failed: %d.",0 , 0, 0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return false;
      }
    }
    break;
    case QCMAP_MSGR_GUEST_AP_V01:
    {
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num2);

      if (!QcMapMgr->dual_wifi_mode)
      {
        this->StopHostapdCli(ap_type);

        /* Stop the process only if it not already killed. */
        if ( guest_ap_pid != 0 )
        {
          /* Stop the hostapd for Guest AP. */
          KillHostapd(guest_ap_pid, QcMapMgr->ap_dev_num2);
        }
        if ( !IsHostapdkilled(ap_pid) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }

        /* Sleep so clients will disconnect */
        sleep(QcMapMgr->wlan_restart_delay);
      }

      LOG_MSG_INFO1("Delete CDI information", 0, 0, 0);
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);

      /* Restart Hostapd for GuestAP. */
      if (!QcMapMgr->dual_wifi_mode)
      {
        this->StartHostAPD(QCMAP_MSGR_INTF_GUEST_AP_INDEX, QcMapMgr->ap_dev_num2);
      }
      else
      {
        //CALL to SCM start Guest AP
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_2_V01, SCM_MSGR_WLAN_IF_RESTART_V01);
      }

      addr.s_addr = htonl(this->wlan_cfg.
        interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(netmask, inet_ntoa(addr), 16);
      addr.s_addr = htonl(this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]
        .ll_ip_addr & this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]
        .ll_subnet_mask);
      strlcpy(subnet, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev \
        wlan%d", subnet, netmask, QcMapMgr->ap_dev_num2);
      ds_system_call(command, strlen(command));
      if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num2))
      {
        LOG_MSG_ERROR("AssociateWLANtoBridge failed: %d.",0 , 0, 0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return false;
      }
    }
    break;

    case QCMAP_MSGR_PRIMARY_AND_GUEST_AP_V01:
    {
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num1);
      DisAssociateWLANtoBridge(QcMapMgr->ap_dev_num2);

      if (!QcMapMgr->dual_wifi_mode)
      {
        this->StopHostapdCli(ap_type);

        /* Stop Hostapd. */
        StopHostAPD();

        if ( !IsHostapdkilled(0) )
        {
          LOG_MSG_ERROR("Another instance of hostapd is running", 0, 0, 0);
          *qmi_err_num = QMI_ERR_NOT_SUPPORTED_V01;
          return false;
        }

        /* Sleep so clients will disconnect */
        sleep(QcMapMgr->wlan_restart_delay);
      }

      LOG_MSG_INFO1("Delete CDI information", 0, 0, 0);
      device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);
      device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
      QcMapMgr->DeleteConnectedDevicesClients(device_type);

      if (!QcMapMgr->dual_wifi_mode)
      {
        /* Bring up interfaces.
          This part needs to be done only for AP interfaces.
          Proceed only upto STA iface index since STA is always last in the
          list (ap; ap,ap; ap,sta; ap,ap,sta supported). */
        for ( i=0, dev = QcMapMgr->ap_dev_num1; i < QcMapMgr->ap_dev_num2; i++,
              dev++ )
        {
          if ( !this->wlan_cfg.interface[i].enable )
          break;
          /* Start the HostAP daemon. */
          this->StartHostAPD(i, dev);
          addr.s_addr = htonl(this->wlan_cfg.
            interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
          strlcpy(netmask, inet_ntoa(addr), 16);
          addr.s_addr = htonl(this->wlan_cfg.interface[i].ll_ip_addr & this->wlan_cfg
           .interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
          strlcpy(subnet, inet_ntoa(addr), 16);
          snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev \
            wlan%d", subnet, netmask, dev);
          ds_system_call(command, strlen(command));
        }
      }
      else
      {
        //CALL to SCM start AP-AP
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_1_V01, SCM_MSGR_WLAN_IF_RESTART_V01);
        QcMapWLANSCMMgr->SCMReconfig(SCM_MSGR_WLAN_IF_SAP_2_V01, SCM_MSGR_WLAN_IF_RESTART_V01);
      }

      if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num1))
      {
        LOG_MSG_ERROR("AssociateWLANtoBridge dev1 failed: %d.",0 , 0, 0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return false;
      }
      if(!this->AssociateWLANtoBridge(QcMapMgr->ap_dev_num2))
      {
        LOG_MSG_ERROR("AssociateWLANtoBridge dev2 failed: %d.",0 , 0, 0);
        *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
        return false;
      }
    }
    break;
  }

  return true;
}


/*=====================================================================
FUNCTION StopHostapdCli
======================================================================*/
/*!
@brief
Kills appropriate hostapd_cli process

@parameters
- qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type

@return
void

@note

- Dependencies
- None

- Side Effects
- None

/*====================================================================*/
void QCMAP_WLAN::StopHostapdCli
(
qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type
)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);


  LOG_MSG_INFO1("QCMAP_WLAN::StopHostapdCli", 0, 0, 0);
  switch ( ap_type )
  {
    case QCMAP_MSGR_PRIMARY_AP_V01:
    {
      snprintf(command,
      MAX_COMMAND_STR_LEN,
      "kill -15 `ps -ef | grep hostapd_cli | grep wlan%d"
      "| awk '{print $1}'`",QcMapMgr->ap_dev_num1);
      ds_system_call(command, strlen(command));
    }
    break;

    case QCMAP_MSGR_GUEST_AP_V01:
    {
      snprintf(command,
      MAX_COMMAND_STR_LEN,
      "kill -15 `ps -ef | grep hostapd_cli | grep wlan%d"
      "| awk '{print $1}'`",QcMapMgr->ap_dev_num2);
      ds_system_call(command, strlen(command));
    }
    break;

    case QCMAP_MSGR_PRIMARY_AND_GUEST_AP_V01:
    {
      /*Kill the hostapd_cli */
      snprintf(command,
      MAX_COMMAND_STR_LEN,
      "killall hostapd_cli");
      ds_system_call(command, strlen(command));
    }
    break;
    default:
    {
      LOG_MSG_ERROR("QCMAP_WLAN::StopHostapdCli-Invalid ap type %d ",ap_type, 0, 0);
    }
    break;
  }
}

/*=====================================================================
FUNCTION StartHostapdCli
======================================================================*/
/*!
@brief
Start appropriate hostapd_cli process

@parameters
- qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type

@return
void

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void QCMAP_WLAN::StartHostapdCli
(
qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type
)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  switch ( ap_type )
  {
    case QCMAP_MSGR_PRIMARY_AP_V01:
    {
      if(!hostapdclicmd(QcMapMgr->ap_dev_num1))
      {
        LOG_MSG_ERROR("QCMAP_ConnectionManager::start hostapd_cli failed %d",ap_type, 0, 0);
      }
    }
    break;

    case QCMAP_MSGR_GUEST_AP_V01:
    {
      if(!hostapdclicmd(QcMapMgr->ap_dev_num2))
      {
        LOG_MSG_ERROR("QCMAP_ConnectionManager::start hostapd_cli failed %d",ap_type, 0, 0);
      }
    }
    break;

    case QCMAP_MSGR_PRIMARY_AND_GUEST_AP_V01:
    {
      if(!hostapdclicmd(QcMapMgr->ap_dev_num1))
      {
        LOG_MSG_ERROR("QCMAP_ConnectionManager::start hostapd_cli failed %d",ap_type, 0, 0);
      }
      if(!hostapdclicmd(QcMapMgr->ap_dev_num2))
      {
        LOG_MSG_ERROR("QCMAP_ConnectionManager::start hostapd_cli failed %d",ap_type, 0, 0);
      }
    }
    break;

    default:
    {
      LOG_MSG_ERROR("QCMAP_WLAN::StartHostapdCli-Invalid ap type %d",ap_type, 0, 0);
    }
    break;
  }
}

/*=====================================================================
FUNCTION StopHostAPD
======================================================================*/
/*!
@brief
Stops hostapd service.

@parameters
int interface

@return
true	- on success
false - on failure

@note

- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN::StopHostAPD(void)
{
  FILE *fp = NULL;
  int ap_pid = 0, guest_ap_pid = 0;
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  ds_system_call("killall hostapd",
   strlen("killall hostapd"));

  switch (this->wifi_mode)
  {
    case QCMAP_MSGR_WLAN_MODE_AP_V01:
    case QCMAP_MSGR_WLAN_MODE_AP_STA_V01:
    /* Only one instance of hostapd is running */
    if (!IsHostapdkilled(0) )
    {
      LOG_MSG_INFO1("Forcefully terminating hostapd", 0, 0, 0);
      ds_system_call("killall -9 hostapd", strlen("killall -9 hostapd"));
    }
    snprintf( command, MAX_COMMAND_STR_LEN,"rm %s",HOSTAPD_PID_FILE);
    ds_system_call(command, strlen(command));
    KillHostapd(0, QcMapMgr->ap_dev_num1);
    break;

    case QCMAP_MSGR_WLAN_MODE_AP_AP_V01:
    /* Get the Primary AP PID. */
    fp = fopen(HOSTAPD_PID_FILE, "r");
    if ( fp == NULL )
    {
      LOG_MSG_ERROR("Error opening hostapd pid file: %d.", errno, 0, 0);
    }
    else if (fscanf(fp, "%d", &ap_pid) != 1)
    {
      LOG_MSG_ERROR("primary AP hostapd is stopped: %d.", errno, 0, 0);
      ap_pid = 0;
      fclose(fp);
    }
    else
    {
      LOG_MSG_INFO1("Primary AP PID = %d",ap_pid,0,0);
      fclose(fp);
    }

    /* Get the Guest AP PID. */
    fp = fopen(HOSTAPD_SSID2_PID_FILE, "r");
    if ( fp == NULL )
    {
      LOG_MSG_ERROR("Error opening Guest AP hostapd pid file: %d.",errno,0,0);
    }
    else if (fscanf(fp, "%d", &guest_ap_pid) != 1)
    {
      LOG_MSG_ERROR("Guest AP hostapd is stopped: %d.", errno, 0, 0);
      guest_ap_pid = 0;
      fclose(fp);
    }
    else
    {
      LOG_MSG_INFO1("Guest AP PID = %d",guest_ap_pid,0,0);
      fclose(fp);
    }
    /* Check if Primary ap hostapd is killed */
    if(guest_ap_pid !=0 )
    {
      if ( !IsHostapdkilled(guest_ap_pid) )
      {
        LOG_MSG_INFO1("Forcefully terminating hostapd", 0, 0, 0);
        ds_system_call("killall -9 hostapd", strlen("killall -9 hostapd"));
      }
    }

    KillHostapd(0, QcMapMgr->ap_dev_num2);
    snprintf( command, MAX_COMMAND_STR_LEN,"rm %s",HOSTAPD_PID_FILE);
    ds_system_call(command, strlen(command));
    /* Check if Guest ap hostapd is killed */
    if(ap_pid !=0 )
    {
      if (!IsHostapdkilled(ap_pid) )
      {
        LOG_MSG_INFO1("Forcefully terminating hostapd", 0, 0, 0);
        ds_system_call("killall -9 hostapd", strlen("killall -9 hostapd"));
      }
    }
    KillHostapd(0, QcMapMgr->ap_dev_num1);
    snprintf( command, MAX_COMMAND_STR_LEN,"rm %s",HOSTAPD_SSID2_PID_FILE);
    ds_system_call(command, strlen(command));
    break ;

    default:
    break;
  }
  return true;
}


/*=====================================================================
FUNCTION StartHostAPD
======================================================================*/
/*!
@brief
Starts hostapd service with the appropriate configuration
files based on the wlan mode configured.

@parameters
int interface

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
boolean QCMAP_WLAN::StartHostAPD(int interface, int wlan_dev_index)
{
  char command[MAX_COMMAND_STR_LEN];
  char temp_command[MAX_COMMAND_STR_LEN];
  qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  int retry = 0, ret = true;
  QCMAP_CM_LOG_FUNC_ENTRY();


  ds_system_call("echo QCMAP:Start Hostapd > /dev/kmsg",\
    strlen("echo QCMAP:Start Hostapd > /dev/kmsg"));

  snprintf(command, MAX_COMMAND_STR_LEN, "sh /etc/cld-hostapd-wlan%d.sh",
  wlan_dev_index);
  ds_system_call(command, strlen(command));

  if ( interface == QCMAP_MSGR_INTF_AP_INDEX &&
  this->wifi_mode == QCMAP_MSGR_WLAN_MODE_AP_STA_V01 )
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "hostapd -B %s -P %s -e %s ",
    this->wlan_cfg.interface[interface].path_to_sta_mode_hostapd_conf,
    HOSTAPD_PID_FILE,
    this->wlan_cfg.interface[interface].path_to_hostapd_entropy);
    ap_type = QCMAP_MSGR_PRIMARY_AP_V01;
  }
  else if ( interface == QCMAP_MSGR_INTF_AP_INDEX )
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "hostapd -B %s -P %s -e %s ",
    this->wlan_cfg.interface[interface].path_to_hostapd_conf,
    HOSTAPD_PID_FILE,
    this->wlan_cfg.interface[interface].path_to_hostapd_entropy);
    LOG_MSG_INFO1("AP MODE: Hostapdconf: %s entropy: %s interface: %d",
        this->wlan_cfg.interface[interface].path_to_hostapd_conf,
        this->wlan_cfg.interface[interface].path_to_hostapd_entropy,interface);

    ap_type = QCMAP_MSGR_PRIMARY_AP_V01;
  }
  else
  {
    snprintf(command, MAX_COMMAND_STR_LEN, "hostapd -B %s -P %s -e %s ",
    this->wlan_cfg.interface[interface].path_to_hostapd_conf,
    HOSTAPD_SSID2_PID_FILE,
    this->wlan_cfg.interface[interface].path_to_hostapd_entropy);

    LOG_MSG_INFO1("Hostapdconf: %s entropy: %s interface: %d",
            this->wlan_cfg.interface[interface].path_to_hostapd_conf,
            this->wlan_cfg.interface[interface].path_to_hostapd_entropy,interface);
    ap_type = QCMAP_MSGR_GUEST_AP_V01;
  }

  /* If debug mode is enabled add concat dbg cmd  */
  if (interface == QCMAP_MSGR_INTF_AP_INDEX && this->debug_config.hostapd_debug)
  {
    snprintf(temp_command,
    MAX_COMMAND_STR_LEN, "%s %s", command,
    this->debug_config.hostapd_dbg_cmd);
    memcpy(command, temp_command, MAX_COMMAND_STR_LEN);
  }
  else if (interface == QCMAP_MSGR_INTF_GUEST_AP_INDEX
            && this->debug_config.hostapd_guest_debug)
  {
    snprintf(temp_command, MAX_COMMAND_STR_LEN, "%s %s", command,
                    this->debug_config.hostapd_guest_dbg_cmd);
    memcpy(command, temp_command, MAX_COMMAND_STR_LEN);
  }

  ds_system_call(command, strlen(command));


  if(IsHostapdRunning(interface))
  {
    /* Though hostapd application has started, the wlan driver is not yet
     * done with AP Start and beacon operation. Check to confirm
     */
    while (CheckAPStatus(wlan_dev_index) == 0 &&
             retry < QCMAP_HOSTAPD_START_POLL_MAX_COUNT) {
      retry++;
      usleep(QCMAP_HOSTAPD_POLL_DELAY_MS);
    }
  } else
    retry = QCMAP_HOSTAPD_START_POLL_MAX_COUNT;

  if (retry >= QCMAP_HOSTAPD_START_POLL_MAX_COUNT) {
    LOG_MSG_ERROR("hostapd start failed", 0, 0, 0);
    ret = false;
  }
  //Start Hostapd_cli on the interface
  this->StartHostapdCli(ap_type);

  snprintf(temp_command, MAX_COMMAND_STR_LEN,
            "echo \"QCMAP: Wlan%d Hostapd Start %s\" > /dev/kmsg",
            wlan_dev_index, (ret == true ? "Success" : "Fail"));
  ds_system_call(temp_command, strlen(temp_command));

  return ret;
}


/*=====================================================================
FUNCTION UpdateAccessProfileRules
======================================================================*/
/*!
@brief
Updates Ebtables rules for changes in access profile.

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
/*====================================================================*/
void QCMAP_WLAN::UpdateAccessProfileRules(void)
{

  if (this->prev_guest_profile == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
  {
    this->DeleteGuestAPAccessRules();
  }
  else if (this->prev_guest_profile == QCMAP_MSGR_PROFILE_FULL_ACCESS_V01)
  {
    this->InstallGuestAPAccessRules();
  }
}


/*=====================================================================
FUNCTION InstallGuestAPAccessRules
======================================================================*/
/*!
@brief
Install Ebtables/iptables rules for Guest AP access.

@parameters
none

@return
none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void QCMAP_WLAN::InstallGuestAPAccessRules(void)
{
  char command[MAX_COMMAND_STR_LEN];
  char devname[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN+2];
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  struct in_addr addr;
  char prefix_v6[MAX_IPV6_PREFIX + 1];
  struct in6_addr *prefix_ptr = NULL;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  QCMAP_Backhaul_Cradle* QcMapBackhaulCradleMgr=QCMAP_Backhaul_Cradle::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  FILE *access_fd = NULL;
  char publicIpAddr[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnetIP[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  qmi_error_type_v01 qmi_err_num;

  qcmap_cm_nl_prefix_info_t* ipv6_prefix_info_ptr=NULL;

  if(QcMapBackhaulMgr)
  {
    ipv6_prefix_info_ptr = &QcMapBackhaulMgr->ipv6_prefix_info;
    memset(prefix_v6, 0, MAX_IPV6_PREFIX + 1);
    prefix_ptr = &((struct sockaddr_in6 *)&(ipv6_prefix_info_ptr->prefix_addr))->sin6_addr;
    inet_ntop(AF_INET6, (struct in6_addr *)prefix_ptr->s6_addr, prefix_v6,
      MAX_IPV6_PREFIX );
  }
  addr.s_addr = htonl(QCMAPLANMgr->lan_cfg.apps_ip_addr);
  strlcpy(a5_ip, inet_ntoa(addr), 16);
  addr.s_addr = htonl(QCMAPLANMgr->lan_cfg.sub_net_mask);
  strlcpy(netmask, inet_ntoa(addr), 16);
  access_fd = fopen(QCMAP_MULTIPLE_SYSTEM_CALL_FILE,"w");
  /* File could not be opened for writing/append*/
  if (access_fd == NULL)
  {
    LOG_MSG_ERROR("Cannot open %s file for writing",QCMAP_MULTIPLE_SYSTEM_CALL_FILE,0,0);
    return;
  }
  fprintf( access_fd,"#!/bin/sh \n");

  /* ----------------------------------------------------- */
  /* ------------------ IPv4 Rules ----------------------- */
  /* ----------------------------------------------------- */
  /* Rules to allow DNS/DHCP and block the rest of the traffic from Guest AP Client to Appsocal subnet*/
  fprintf( access_fd,"ebtables -I INPUT -i wlan%d -p IPv4 --ip-proto udp\
    --ip-dport 53 -j ACCEPT \n",QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -I INPUT -i wlan%d -p IPv4 --ip-proto tcp\
    --ip-dport 53 -j ACCEPT \n",QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -I INPUT -i wlan%d -p IPv4 --ip-proto udp\
    --ip-dport 67 -j ACCEPT \n",QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -A INPUT -i wlan%d -p IPv4 --ip-destination\
    %s/%s -j DROP \n",QcMapMgr->ap_dev_num2,a5_ip,netmask);
  fprintf( access_fd,"ebtables -I INPUT -i wlan%d -p IPv4 --ip-proto tcp\
    --ip-dport 8888 -j ACCEPT \n",QcMapMgr->ap_dev_num2);


  /* Rules to allow DNS/DHCP and block the rest of the traffic from A5/local subnet to GuestAPClient*/
  fprintf( access_fd, "ebtables -I OUTPUT -o wlan%d -p IPv4 --ip-proto udp\
    --ip-sport 53 -j ACCEPT \n",QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -I OUTPUT -o wlan%d -p IPv4 --ip-proto tcp\
    --ip-sport 53 -j ACCEPT \n",QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -I OUTPUT -o wlan%d -p IPv4 --ip-proto udp\
    --ip-sport 67 -j ACCEPT \n",QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -A OUTPUT -o wlan%d -p IPv4 --ip-source\
    %s/%s -j DROP \n",QcMapMgr->ap_dev_num2,a5_ip,netmask);

  /* Rules to block traffic to pubilc ip on A5 */
  if(QcMapBackhaulMgr && (qcmap_cm_get_state(&qmi_err_num) == QCMAP_CM_WAN_CONNECTED ||
      QcMapBackhaulCradleMgr|| QcMapBackhaulWLANMgr ))
  {
      addr.s_addr = QcMapBackhaulMgr->ipv4_public_ip;
      strlcpy(publicIpAddr, inet_ntoa(addr), QCMAP_LAN_MAX_IPV4_ADDR_SIZE);
      fprintf( access_fd, "ebtables -D OUTPUT -o wlan%d -p IPv4 --ip-source %s\
      -j DROP \n",QcMapMgr->ap_dev_num2, publicIpAddr);
      fprintf( access_fd, "ebtables -A OUTPUT -o wlan%d -p IPv4 --ip-source %s\
        -j DROP \n",QcMapMgr->ap_dev_num2, publicIpAddr);
      fprintf( access_fd, "ebtables -D INPUT -i wlan%d -p IPv4 --ip-destination\
        %s -j DROP \n",QcMapMgr->ap_dev_num2, publicIpAddr);
      fprintf( access_fd, "ebtables -A INPUT -i wlan%d -p IPv4 --ip-destination\
        %s -j DROP \n",QcMapMgr->ap_dev_num2, publicIpAddr);
  }
  else
  {
    /* This will make sure to drop all data, since backhaul in not connected.
       It will make sure to drop packet during embedded call also. */
    fprintf( access_fd, "ebtables -A INPUT -i wlan%d -p IPv4 -j DROP \n",\
             QcMapMgr->ap_dev_num2);
    fprintf( access_fd, "ebtables -A OUTPUT -o wlan%d -p IPv4 -j DROP \n",\
             QcMapMgr->ap_dev_num2);
  }

  /* ----------------------------------------------------- */
  /* ------------------ IPv6 Rules ----------------------- */
  /* ----------------------------------------------------- */
  /* Rule to allow ICMP/DNS messages and block rest of the traffic to A5 or local LAN*/
  fprintf( access_fd, "ebtables -I INPUT -i wlan%d -p IPv6 --ip6-proto ipv6-icmp\
    --ip6-icmp-type 133:136/0 -j ACCEPT \n",QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -I INPUT -i wlan%d -p IPv6 --ip6-proto udp\
    --ip6-dport 53 -j ACCEPT \n",QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -I INPUT -i wlan%d -p IPv6 --ip6-proto tcp \
  --ip6-dport 53 -j ACCEPT \n",QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -A INPUT -i wlan%d -p IPv6 --ip6-destination \
    fe80::/64 -j DROP \n",QcMapMgr->ap_dev_num2);

    // If a global IPv6 prefix is available, also add prefix based rules.
  if (QcMapBackhaulMgr && QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid )
  {
    fprintf( access_fd, "ebtables -A INPUT -i wlan%d -p IPv6 --ip6-destination\
      %s/%d -j DROP \n",QcMapMgr->ap_dev_num2,prefix_v6,ipv6_prefix_info_ptr->prefix_len);
    // Set the flag to indicate ipv6 prefix based rules are added
    if(QcMapBackhaulWWANMgr)
      QcMapBackhaulWWANMgr->ipv6_prefix_based_rules_added = true;
  }

  /* Rule to Drop traffic routed from local subnet to guest AP. Allow DNS/ICMP responses*/
  fprintf( access_fd, "ebtables -I OUTPUT -o wlan%d -p IPv6 --ip6-proto ipv6-icmp\
    --ip6-icmp-type 133:136/0 -j ACCEPT \n",QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -I OUTPUT -o wlan%d -p IPv6 --ip6-proto udp --ip6\
    -sport 53 -j ACCEPT \n",QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -I OUTPUT -o wlan%d -p IPv6 --ip6-proto tcp --ip6\
    -sport 53 -j ACCEPT \n",QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -A OUTPUT -o wlan%d -p IPv6 --ip6-source \
    fe80::/64 -j DROP \n",QcMapMgr->ap_dev_num2);

  // If a global IPv6 prefix is available, also add prefix based rules.
  if (QcMapBackhaulMgr && QcMapBackhaulMgr->ipv6_prefix_info.prefix_info_valid)
  {
    fprintf( access_fd, "ebtables -A OUTPUT -o wlan%d -p IPv6 --ip6-source %s/%d\
    -j DROP \n",QcMapMgr->ap_dev_num2,prefix_v6,ipv6_prefix_info_ptr->prefix_len);
  }

  /* Rule to Drop traffic, if embedded call is present */
  if ( qcmap_cm_get_ipv6_state(&qmi_err_num) != QCMAP_CM_V6_WAN_CONNECTED &&
       !QcMapBackhaulCradleMgr && !QcMapBackhaulWLANMgr )
  {
      fprintf( access_fd, "ebtables -A OUTPUT -o wlan%d -p IPv6 -j DROP \n",\
               QcMapMgr->ap_dev_num2);
      fprintf( access_fd, "ebtables -A INPUT -i wlan%d -p IPv6 -j DROP \n",\
               QcMapMgr->ap_dev_num2);
  }


  /* Rules to drop briged traffic to/from LAN from/to guest AP*/
  fprintf( access_fd, "ebtables -A FORWARD -o wlan%d -j DROP \n",
  QcMapMgr->ap_dev_num2);
  fprintf( access_fd, "ebtables -A FORWARD -i wlan%d -j DROP \n",
  QcMapMgr->ap_dev_num2);

  fclose(access_fd);

  /* Changing permissions */
  snprintf( command, MAX_COMMAND_STR_LEN,
  "chmod 755 %s",QCMAP_MULTIPLE_SYSTEM_CALL_FILE);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,QCMAP_MULTIPLE_SYSTEM_CALL_FILE);
  ds_system_call( command, strlen(command));
  snprintf( command, MAX_COMMAND_STR_LEN,
  "rm -f %s",QCMAP_MULTIPLE_SYSTEM_CALL_FILE);
  ds_system_call( command, strlen(command));

  /* Update IPACM_cfg*/
  this->SetIPAWLANMode(QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01);
}


/*=====================================================================
FUNCTION IsHostapdkilled
======================================================================*/
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
/*====================================================================*/
boolean QCMAP_WLAN::IsHostapdkilled(int omit_pid)
{
  char process[MAX_COMMAND_STR_LEN];
  int i = 0;
  FILE *cmd;
  char pid_s[MAX_CMD_SIZE];
  int pid;

  memset(process, 0, MAX_COMMAND_STR_LEN);

  if ( omit_pid == 0 )
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof hostapd");
  }
  else
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof -o %d hostapd", omit_pid);
  }

  while (i++ < QCMAP_PROCESS_KILL_RETRY)
  {
    usleep(QCMAP_PROCESS_KILL_WAIT_MS);/*50 mini second*/
    cmd = popen(process, "r");
    pid = 0;
    memset(pid_s, 0, MAX_CMD_SIZE);
    if(cmd)
    {
      fgets(pid_s, MAX_CMD_SIZE, cmd);
      pclose(cmd);
    }
    pid = atoi(pid_s);
    /* If pid is zero we break from while*/
    if(pid == 0)
    {
      return true;
    }
  }

  LOG_MSG_ERROR("PID still running after waiting 2 second", 0, 0, 0);
  return false;
}


/*=====================================================================
  FUNCTION DeleteGuestAPAccessRules
======================================================================*/
/*!
@brief
  Delete Ebtables/iptables rules for Guest AP access.

@parameters
   none

@return
   none

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
void QCMAP_WLAN::DeleteGuestAPAccessRules(void)
{
  char command[MAX_COMMAND_STR_LEN];
  QCMAP_Backhaul_WWAN* QcMapBackhaulWWANMgr=QCMAP_Backhaul_WWAN::Get_Instance(false);
  /* Flush complete Ebtables  */
  snprintf( command, MAX_COMMAND_STR_LEN,
             "ebtables -t filter --flush");
  ds_system_call( command, strlen(command));
  if(QcMapBackhaulWWANMgr)
    QcMapBackhaulWWANMgr->ipv6_prefix_based_rules_added = false;

  /* Update IPACM_cfg*/
  this->SetIPAWLANMode(QCMAP_MSGR_PROFILE_FULL_ACCESS_V01);
}

/*=====================================================================
  FUNCTION CheckHostapd
======================================================================*/
/*!
@brief
  Return true if the hostapd is running.

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
/*====================================================================*/
boolean CheckHostapd(int omit_pid)
{
  char process[MAX_COMMAND_STR_LEN];
  int i = 0;
  FILE *cmd;
  char pid_s[MAX_CMD_SIZE];
  int pid;

  memset(process, 0, MAX_COMMAND_STR_LEN);

  if ( omit_pid == 0 )
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof hostapd");
  }
  else
  {
    snprintf(process, MAX_COMMAND_STR_LEN, "pidof -o %d hostapd", omit_pid);
  }
  cmd = popen(process, "r");
  pid = 0;
  memset(pid_s, 0, MAX_CMD_SIZE);
  if(cmd)
  {
    fgets(pid_s, MAX_CMD_SIZE, cmd);
    pid = atoi(pid_s);
    LOG_MSG_INFO1("pid =%d", pid, 0, 0);
    pclose(cmd);
    /* If pid is zero we break from while*/
    if(pid != 0)
    {
      return true;
    }
  }
  LOG_MSG_ERROR("hostapd not running!!", 0, 0, 0);
  return false;
}

/*=====================================================================
  FUNCTION IsSupplicantRunning
======================================================================*/
/*!
@brief
  Return true if the wpa_supplicant is running.

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
/*====================================================================*/
boolean QCMAP_WLAN::IsSupplicantRunning(void)
{
  char process[MAX_COMMAND_STR_LEN];
  int i = 0;
  FILE *cmd;
  char pid_s[MAX_CMD_SIZE];
  int pid;

  memset(process, 0, MAX_COMMAND_STR_LEN);

  snprintf(process, MAX_COMMAND_STR_LEN, "pidof wpa_supplicant");
  cmd = popen(process, "r");
  pid = 0;
  memset(pid_s, 0, MAX_CMD_SIZE);
  if(cmd)
  {
    fgets(pid_s, MAX_CMD_SIZE, cmd);
    pid = atoi(pid_s);
    LOG_MSG_INFO1("pid =%d", pid, 0, 0);
    pclose(cmd);
    /* If pid is zero then process is not running*/
    if(pid != 0)
    {
      return true;
    }
  }

  LOG_MSG_ERROR("wpa_supplicant not running!!",0,0,0);
  return false;
}

/*=====================================================================
  FUNCTION IsHostapdRunning
======================================================================*/
/*!
@brief
  Checks if the hostapd is running.

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
/*====================================================================*/
boolean QCMAP_WLAN::IsHostapdRunning(int iface)
{
  FILE *fp = NULL;
  int ap_pid = 0;

  switch ( iface )
  {
    case QCMAP_MSGR_INTF_AP_INDEX:
     {
        LOG_MSG_INFO1("Primary AP mode %d",iface,0,0);
        /* Only instance of hostapd will be running for the first time */
        if ( CheckHostapd(0) )
        {
          LOG_MSG_INFO1("Hostapd is Running", 0, 0, 0);
          return true;
        }
        else
        {
          return false;
        }
     }
     break;
    case QCMAP_MSGR_INTF_GUEST_AP_INDEX:
     {
        LOG_MSG_INFO1("Guest AP mode %d",iface,0,0);
        /* Get the Primary AP PID. */
        fp = fopen(HOSTAPD_PID_FILE, "r");

        if ( fp == NULL )
        {
          LOG_MSG_ERROR("Error opening hostapd pid file: %d.", errno, 0, 0);
        }
        else if (fscanf(fp, "%d", &ap_pid) != 1)
        {
          LOG_MSG_ERROR("Error reading hostapd file: %d.", errno, 0, 0);
          ap_pid = 0;
          fclose(fp);
        }
        else
        {
          LOG_MSG_INFO1("Primary AP PID = %d",ap_pid,0,0);
          fclose(fp);
        }

        if ( ap_pid == 0 )
        {
          LOG_MSG_ERROR("Hostapd for AP is not yet started", 0, 0, 0);
        }

        if ( CheckHostapd(ap_pid) )
        {
          LOG_MSG_INFO1("Guest AP mode Hostapd is Running", 0, 0, 0);
          return true;
        }
        else
        {
         return false;
        }
     }
     break;
    default:
          LOG_MSG_ERROR("No Hostapd of this mode %d", iface, 0, 0);
     break;
  }
  return true;
}

/*=====================================================================
  FUNCTION GetsetWLANConfigFromXML
======================================================================*/
/*!
@brief
  Get / sets the wlan config from XML.

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*====================================================================*/

boolean QCMAP_WLAN::GetsetWLANConfigFromXML
(
  qcmap_wlan_config_enum conf,
  qcmap_action_type action,
  uint32 *value
)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  in_addr addr;
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  switch (conf)
  {
    case CONFIG_WLAN_MODE:
      if (action == GET_VALUE)
      {
        if (strncasecmp(AP_STA_TAG, root.child(WlanMode_TAG).child_value(),\
          strlen(AP_STA_TAG)) == 0)
          *value = (uint8)QCMAP_MSGR_WLAN_MODE_AP_STA_V01;
        else if (strncasecmp(AP_AP_TAG, root.child(WlanMode_TAG).child_value(),\
          strlen(AP_AP_TAG)) == 0)
          *value = (uint8)QCMAP_MSGR_WLAN_MODE_AP_AP_V01;
        else
          /* Default Value. */
          *value = (uint8)QCMAP_MSGR_WLAN_MODE_AP_V01;
      }
      else
      {
        if (*value  == QCMAP_MSGR_WLAN_MODE_AP_AP_V01)
          root.child(WlanMode_TAG).text() = AP_AP_TAG;
        else if (*value  == QCMAP_MSGR_WLAN_MODE_AP_STA_V01)
          root.child(WlanMode_TAG).text() = AP_STA_TAG;
        else
          root.child(WlanMode_TAG).text() = AP_TAG;
      }
    break;

    case CONFIG_GUEST_ACCESS_PROFILE:
      root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).\
        child(MobileAPLanCfg_TAG).child(GuestAPCfg_TAG);
      if (action == GET_VALUE)
      {
        if (strncasecmp(FULL_TAG, root.child(AccessProfile_TAG).child_value(),\
              MIN(sizeof(FULL_TAG) - 1, strlen(root.child(AccessProfile_TAG).\
              child_value()))) == 0)
        {
          *value = (uint8)QCMAP_MSGR_PROFILE_FULL_ACCESS_V01;
        }
        else if (strncasecmp(INTERNET_TAG, root.child(AccessProfile_TAG).\
                 child_value(), MIN(sizeof(INTERNET_TAG) - 1,\
                 strlen(root.child(AccessProfile_TAG).child_value()))) == 0)
        {
          *value = (uint8)QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
        }
      }
      else
      {
        if(*value == (uint8)QCMAP_MSGR_PROFILE_FULL_ACCESS_V01)
        {
          root.child(AccessProfile_TAG).text() = FULL_TAG;
        }
        else if(*value == (uint8)QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
        {
          root.child(AccessProfile_TAG).text() = INTERNET_TAG;
        }
      }
    break;
  }

  if (action == SET_VALUE)
     QcMapMgr->WriteConfigToXML(UPDATE_MOBILEAP_XML, &xml_file);
  return TRUE;
}

/*===========================================================================
  FUNCTION ReadWLANConfigFromXML
==========================================================================*/
/*!
@brief
  Reads WLAN Config from XML.

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/

boolean QCMAP_WLAN::ReadWLANConfigFromXML()
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  pugi::xml_document xml_file;
  pugi::xml_node  root, child;
  in_addr addr;
  qcmap_cm_intf_index_type ssid = QCMAP_MSGR_INTF_STATION_INDEX;

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  if (strncasecmp(AP_STA_TAG, root.child(WlanMode_TAG).child_value(),\
        strlen(AP_STA_TAG)) == 0)
  {
    this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_STA_V01;
  }
  else if (strncasecmp(AP_AP_TAG, root.child(WlanMode_TAG).child_value(),\
           strlen(AP_AP_TAG)) == 0)
  {
    this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_AP_V01;
  }
  else
  {
    /* Default Value. */
    this->wlan_cfg.wlan_mode = QCMAP_MSGR_WLAN_MODE_AP_V01;
  }

  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX]\
          .path_to_hostapd_conf, root.child(HostAPDCfg_TAG).child_value(),\
          QCMAP_CM_MAX_FILE_LEN);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX]\
          .path_to_hostapd_entropy, root.child(HostAPDEntropy_TAG).child_value(),\
           QCMAP_CM_MAX_FILE_LEN);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX]\
          .path_to_sta_mode_hostapd_conf, root.child(STAModeHostAPDCfg_TAG)\
          .child_value(), QCMAP_CM_MAX_FILE_LEN);

  LOG_MSG_INFO1("READ: Hostapdconf: %s entropy: %s interface: %d",\
                this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX].path_to_hostapd_conf,\
                this->wlan_cfg.interface[QCMAP_MSGR_INTF_AP_INDEX]\
                .path_to_hostapd_entropy, QCMAP_MSGR_INTF_AP_INDEX);

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).\
         child(MobileAPLanCfg_TAG).child(GuestAPCfg_TAG);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]//
          .path_to_hostapd_conf, root.child(HostAPDCfg_TAG).child_value(),\
          QCMAP_CM_MAX_FILE_LEN);
  strlcpy(this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]\
          .path_to_hostapd_entropy, root.child(HostAPDEntropy_TAG)\
          .child_value(), QCMAP_CM_MAX_FILE_LEN);

  LOG_MSG_INFO1( "READ: Hostapdconf: %s entropy: %s interface: %d",\
                 this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]\
                 .path_to_hostapd_conf,
                 this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX]
                 .path_to_hostapd_entropy, QCMAP_MSGR_INTF_GUEST_AP_INDEX);

  if (strncasecmp(FULL_TAG, root.child(AccessProfile_TAG).child_value(),
        MIN(sizeof(FULL_TAG) - 1, strlen(root.child(AccessProfile_TAG).
        child_value()))) == 0)
  {
    this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile =\
                                             QCMAP_MSGR_PROFILE_FULL_ACCESS_V01;
  }
  else if (strncasecmp(INTERNET_TAG, root.child(AccessProfile_TAG).\
            child_value(), MIN(sizeof(INTERNET_TAG) - 1,
            strlen(root.child(AccessProfile_TAG).child_value()))) == 0)
  {
    this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile =\
                                           QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01;
  }
    LOG_MSG_INFO1("WLAN Mode: %d Access profile: %d" , this->wlan_cfg.wlan_mode,\
      this->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile, 0);


  root= xml_file.child(System_TAG).child(Debug_TAG);
  if(root)
  {
    if (root.child(SUPPLICANT_DEBUG_TAG).child_value())
    {
      this->debug_config.supplicant_debug = 1;
      strlcpy(this->debug_config.supplicant_dbg_cmd,\
      root.child(SUPPLICANT_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
    if (root.child(HOSTAPD_DEBUG_TAG).child_value())
    {
      this->debug_config.hostapd_debug= 1;
      strlcpy(this->debug_config.hostapd_dbg_cmd,\
      root.child(HOSTAPD_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
    if (root.child(HOSTAPD_GUEST_DEBUG_TAG).child_value())
    {
      this->debug_config.hostapd_guest_debug= 1;
      strlcpy(this->debug_config.hostapd_guest_dbg_cmd,\
      root.child(HOSTAPD_GUEST_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
  }
  return true;
}


/*===========================================================================
  FUNCTION ReadWLANConfigPathsFromXML
==========================================================================*/
/*!
@brief
  Reads WLAN Config from XML.

@return

@note

 - Dependencies
 - None

 - Side Effects
 - None
*/
/*=========================================================================*/

boolean QCMAP_WLAN::ReadWLANConfigPathsFromXML(qcmap_cm_wlan_paths_conf_t *wlan_paths)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  pugi::xml_document xml_file;
  pugi::xml_node  root, child;
  in_addr addr;
  qcmap_cm_intf_index_type ssid = QCMAP_MSGR_INTF_STATION_INDEX;

  if (!xml_file.load_file(QcMapMgr->xml_path))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);

  /* Copy Primary AP Path */
  strlcpy(wlan_paths->hostapd.path_to_hostapd_conf, root.child(HostAPDCfg_TAG).child_value(),\
          QCMAP_CM_MAX_FILE_LEN);
  strlcpy(wlan_paths->hostapd.path_to_hostapd_entropy, root.child(HostAPDEntropy_TAG).child_value(),\
           QCMAP_CM_MAX_FILE_LEN);
  strlcpy(wlan_paths->hostapd.path_to_sta_mode_hostapd_conf, root.child(STAModeHostAPDCfg_TAG)\
          .child_value(), QCMAP_CM_MAX_FILE_LEN);

  LOG_MSG_INFO1("READ: Hostapdconf: %s entropy: %s interface: %d",\
                wlan_paths->hostapd.path_to_hostapd_conf,\
                wlan_paths->hostapd.path_to_hostapd_entropy, QCMAP_MSGR_INTF_AP_INDEX);

  /* Copy Guest AP Path */
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).\
         child(MobileAPLanCfg_TAG).child(GuestAPCfg_TAG);
  strlcpy(wlan_paths->hostapd_guest.path_to_hostapd_conf, root.child(HostAPDCfg_TAG).child_value(),\
          QCMAP_CM_MAX_FILE_LEN);
  strlcpy(wlan_paths->hostapd_guest.path_to_hostapd_entropy, root.child(HostAPDEntropy_TAG)\
          .child_value(), QCMAP_CM_MAX_FILE_LEN);

  LOG_MSG_INFO1( "READ: Hostapdconf: %s entropy: %s interface: %d",\
                 wlan_paths->hostapd_guest.path_to_hostapd_conf,
                 wlan_paths->hostapd_guest.path_to_hostapd_entropy, QCMAP_MSGR_INTF_GUEST_AP_INDEX);

  /* Copy Supplicant Path */
  root = xml_file.child(System_TAG).child(MobileAPCfg_TAG).child(MobileAPLanCfg_TAG);
  strlcpy( wlan_paths->supplicant,
           root.child(StationModeCfg_TAG).child(SupplicantCfg_TAG).child_value(),
           QCMAP_CM_MAX_FILE_LEN );

  root= xml_file.child(System_TAG).child(Debug_TAG);
  if(root)
  {
    if (root.child(SUPPLICANT_DEBUG_TAG).child_value())
    {
      wlan_paths->debug.supplicant_debug = true;
      strlcpy(wlan_paths->debug.supplicant_dbg_cmd,\
      root.child(SUPPLICANT_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
    if (root.child(HOSTAPD_DEBUG_TAG).child_value())
    {
      wlan_paths->debug.hostapd_debug = true;
      strlcpy(wlan_paths->debug.hostapd_dbg_cmd,\
      root.child(HOSTAPD_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
    if (root.child(HOSTAPD_GUEST_DEBUG_TAG).child_value())
    {
      wlan_paths->debug.hostapd_guest_debug = true;
      strlcpy(wlan_paths->debug.hostapd_guest_dbg_cmd,\
      root.child(HOSTAPD_GUEST_DEBUG_TAG).child_value(),\
      QCMAP_CM_MAX_FILE_LEN);
    }
  }

  return true;
}


/*=====================================================
  FUNCTION SetIPAWLANMode
======================================================*/
/*!
@brief
  This function sets the WLAN mode in IPACM XML

@parameters
  int mode

@return
  true  - on success
  flase - on failure


- Side Effects
- None

- Side Effects
- None
*/
/*=====================================================*/
boolean QCMAP_WLAN::SetIPAWLANMode(int mode)
{
  pugi::xml_document xml_file;
  pugi::xml_node root, child;
  char guest_iface[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 2];
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);

  snprintf(guest_iface, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 2, "wlan%d", QcMapMgr->ap_dev_num2);

  if (!xml_file.load_file(IPA_XML_PATH))
  {
    LOG_MSG_ERROR("Unable to load XML file.",0,0,0);
    return false;
  }

  root = xml_file.child(System_TAG).child(IPACM_TAG).child(IPACMIface_TAG);

  for (child = root.first_child(); child; child = child.next_sibling())
  {
    if (!strncasecmp(child.child(Name_TAG).child_value(),
                     guest_iface, strlen(guest_iface)))
    {
      if (mode == QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
      {
        child.child(WLANMode_TAG).text() = WLAN_Internet_TAG;
      }
      else
      {
        child.child(WLANMode_TAG).text() = WLAN_Full_TAG;
      }
      QcMapMgr->WriteConfigToXML(UPDATE_IPACFG_XML,&xml_file);
      return true;
    }
  }
  return false;
}

/*===========================================================================
 FUNCTION EnableSTAOnlyMode
==========================================================================*/
/*!
@brief
  Brings up WLAN in STA only Mode. If WLAN is enabled it returns error.

@parameters
  void

@return
  true  - on Success
  false - on Failure

@note
  This API is only going to be used by eCNE-QCMAP client.
  Other QCMAP Clients should not use this API.

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_WLAN::EnableSTAOnlyMode(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];
  uint32_t time_out = (WIFI_DEV_INIT_DELAYS_MAX/WIFI_DEV_INIT_DELAY);
  int i = 0;

  LOG_MSG_INFO1("Entering EnableSTAOnlyMode",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if ( !QcMapMgr->qcmap_enable )
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  if (QcMapWLANMgr)
  {
    LOG_MSG_ERROR("WLAN is already enabled. Current Mode:- %d",QcMapWLANMgr->wifi_mode ,0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  else
  {
    QcMapWLANMgr = QCMAP_WLAN::Get_Instance(true);//Creating WLAN object here
    if (!QcMapWLANMgr)
    {
      LOG_MSG_ERROR("Could not create WLAN obj",0,0,0);
      *qmi_err_num = QMI_ERR_NO_MEMORY_V01;
      return false;
    }
  }

  /* Enable WLAN */
  LOG_MSG_INFO1("Bringup WLAN",0,0,0);
  snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan start");
  ds_system_call(command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN, "/proc/sys/net/ipv4/conf/wlan0");
  while ( i++ < time_out )
  {
    std::ifstream ifile( command );
    if ( ifile )
    {
      break;
    }
    usleep( WIFI_DEV_INIT_DELAY );
  }

  LOG_MSG_INFO1("Wlan card brought up", 0,0,0);


  snprintf(command, MAX_COMMAND_STR_LEN,
              "echo \"ctrl_interface=\/var\/run\/wpa_supplicant\" > %s",
               STA_ONLY_MODE_SUPPLICANT_FILE);
  ds_system_call( command, strlen(command));

  snprintf(command, MAX_COMMAND_STR_LEN, "wpa_supplicant -i wlan0 -Dnl80211 -c %s -B",
               STA_ONLY_MODE_SUPPLICANT_FILE); //should be changed to STA_ONLY_FILE or internal
  ds_system_call( command, strlen(command));

  if (QcMapWLANMgr->IsSupplicantRunning() == false)
  {
    LOG_MSG_ERROR("Could not start wpa_supplicant",0,0,0);
    *qmi_err_num = QMI_ERR_INTERNAL_V01;
    return false;
  }

  /* Ensure WiFi is not enabled for other clients */
  QcMapWLANMgr->wifi_mode = QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;
  /* Set STA Mode only variable */
  QcMapWLANMgr->is_sta_only_mode_enabled = true;

  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;
}

/*===========================================================================
 FUNCTION DisableSTAOnlyMode
==========================================================================*/
/*!
@brief
  Brings down WLAN in STA only Mode.
  If WLAN is enabled in any other mode like AP,AP+AP or AP+STA - returns error.

@parameters
  void

@return
  true  - on Success
  false - on Failure

@note
  This API is only going to be used by eCNE-QCMAP client.
  Other QCMAP Clients should not use this API.

- Dependencies
- None

- Side Effects
- None
*/
/*=========================================================================*/
boolean QCMAP_WLAN::DisableSTAOnlyMode(qmi_error_type_v01 *qmi_err_num)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_LAN* QCMAPLANMgr=QCMAP_LAN::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];
  uint32_t time_out = (WIFI_DEV_INIT_DELAYS_MAX/WIFI_DEV_INIT_DELAY);
  int i = 0;

  LOG_MSG_INFO1("Entering DisableSTAOnlyMode",0,0,0);

  /* Cant continue if MobileAP is not enabled! */
  if ( !QcMapMgr->qcmap_enable )
  {
    LOG_MSG_ERROR("Error: QCMAP not enabled.",0,0,0);
    *qmi_err_num = QMI_ERR_INVALID_HANDLE_V01;
    return false;
  }

  if (QcMapWLANMgr && (QcMapWLANMgr->is_sta_only_mode_enabled != true))
  {
    LOG_MSG_ERROR("WLAN is not enabled in STAOnly Mode.",0,0,0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }
  else if (!QcMapWLANMgr)
  {
    LOG_MSG_ERROR("WLAN is already disabled", 0,0, 0);
    *qmi_err_num = QMI_ERR_NO_EFFECT_V01;
    return false;
  }

  /* Kill wpa_supplicant gracefully*/
  snprintf( command, MAX_COMMAND_STR_LEN, "killall -15 wpa_supplicant" );
  ds_system_call(command, strlen(command));

  if (!QCMAP_Backhaul_WLAN::IsWpaSupplicantkilled())
  {
    /* Kill wpa_supplicant forcefully. */
    snprintf( command, MAX_COMMAND_STR_LEN, "killall -9 wpa_supplicant" );
    ds_system_call(command, strlen(command));
  }

  /* Remove Temp Wpa_suplicant File */
  snprintf(command, MAX_COMMAND_STR_LEN, "rm -rf %s", STA_ONLY_MODE_SUPPLICANT_FILE);
  ds_system_call(command, strlen(command));

  /* Disable WLAN */
  LOG_MSG_INFO1("Disable WLAN in STA Only Mode",0,0,0);
  snprintf(command, MAX_COMMAND_STR_LEN,"/etc/init.d/wlan stop");
  ds_system_call(command, strlen(command));

  /* Set STA Mode only variable */
  QcMapWLANMgr->is_sta_only_mode_enabled = false;

  LOG_MSG_INFO1("Wlan card brought down",0,0,0);

  /* Ensure WiFi is not enabled for other clients */
  QcMapWLANMgr->wifi_mode = QCMAP_MSGR_WLAN_MODE_ENUM_MIN_ENUM_VAL_V01;

  *qmi_err_num = QMI_ERR_NONE_V01;
  return true;
}

/*===========================================================================
 FUNCTION SendWLANStatusIND
==========================================================================*/
/*!
@brief
  Send WLAN Status IND to registered clients

@parameters
  void

@return
  void

@note

- Dependencies
- None

- Side Effects
==========================================================================*/
void QCMAP_WLAN::SendWLANStatusIND(void)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  qcmap_msgr_wlan_status_ind_msg_v01 wlan_ind_msg;
  qmi_qcmap_msgr_status_cb_data *cbPtr = NULL;

  if (!QcMapWLANMgr || !QcMapMgr)
  {
    LOG_MSG_ERROR("Invalid Objects!!! WLAN %p, QCMAP %p",QcMapWLANMgr, QcMapMgr, 0);
    return;
  }

  memset(&wlan_ind_msg, 0, sizeof(wlan_ind_msg));
  memcpy(&wlan_ind_msg, &QcMapWLANMgr->current_wlan_status, sizeof(wlan_ind_msg));

  cbPtr = (qmi_qcmap_msgr_status_cb_data *)QcMapMgr->GetQCMAPClientCBDatata();
  /* Send WLAN Status IND to client */
  if (cbPtr && cbPtr->wlan_status_cb)
    cbPtr->wlan_status_cb(QcMapMgr->GetQCMAPClientCBDatata(), &wlan_ind_msg);
}
