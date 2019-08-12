/*====================================================

FILE:  QCMAP_WLAN_SCM.cpp

SERVICES:
   QCMAP Connection Manager WLAN Specific Implementation to SCM

=====================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  02/19/16   tbw         Created

===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ds_string.h"
#include "ds_util.h"
#include "qcmap_cm_api.h"
#include "QCMAP_WLAN.h"
#include "QCMAP_WLAN_SCM.h"
#include "QCMAP_Backhaul.h"
#include "QCMAP_ConnectionManager.h"
#include "qti_wlan_scm_msgr_v01.h"

#include "QCMAP_ConnectionManager.h"

bool QCMAP_WLAN_SCM::flag= false;
QCMAP_WLAN_SCM* QCMAP_WLAN_SCM::object=NULL;

QCMAP_WLAN_SCM* QCMAP_WLAN_SCM::Get_Instance(boolean obj_create)
{
  QCMAP_CM_LOG_FUNC_ENTRY();

  if(!flag && obj_create)
  {
    LOG_MSG_INFO1("Creating object : QCMAP_WLAN_SCM",0, 0, 0);
    object = new QCMAP_WLAN_SCM();
    flag = true;
LOG_MSG_ERROR("created object", 0,0,0);
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

/*===========================================================================
  FUNCTION  qcmap_msgr_qmi_qcmap_ind
  ===========================================================================*/
/*!
  @brief
  Processes an incoming QMI QCMAP Indication.

  @return
  void

  @note

  - Dependencies
  - None

  - Side Effects
  - None
 */
/*=========================================================================*/
void scm_qmi_qcmap_ind
(
 qmi_client_type user_handle,                    /* QMI user handle       */
 unsigned int    msg_id,                         /* Indicator message ID  */
 void           *ind_buf,                        /* Raw indication data   */
 unsigned int    ind_buf_len,                    /* Raw data length       */
 void           *ind_cb_data                     /* User call back handle */
 )
{
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(true);
  scm_msgr_wlan_ind_msg_v01 ind_data;
  qmi_client_error_type qmi_error;
  qmi_error_type_v01 qmi_err_num;

  LOG_MSG_INFO1("scm_qmi_qcmap_ind: user_handle %X msg_id %d ind_buf_len %d.",
          user_handle, msg_id, ind_buf_len);

  if(msg_id == QMI_SCM_MSGR_WLAN_IND_V01)
  {
    qmi_error = qmi_client_message_decode(user_handle,
                                          QMI_IDL_INDICATION,
                                          msg_id,
                                          ind_buf,
                                          ind_buf_len,
                                          &ind_data,
                                          sizeof(scm_msgr_wlan_ind_msg_v01));
    if (qmi_error != QMI_NO_ERR)
    {
      LOG_MSG_ERROR("scm_qmi_qcmap_ind: qmi_client_message_decode error %d",qmi_error,0,0);
      return;
    }

    LOG_MSG_INFO1("Indication Type %d",ind_data.wlan_ind_type,0,0);
    /* Process SCM Indication Types*/
    switch (ind_data.wlan_ind_type)
    {
      case SCM_MSGR_WLAN_DYNAMIC_RECONFIG_IND_V01:
        if (ind_data.wlan_dyn_ind_valid)
        {
          LOG_MSG_INFO1("%d ifaces need to updated",ind_data.wlan_dyn_ind_len,0,0);

          for (int i=0; i < ind_data.wlan_dyn_ind_len; i++)
          {
            if (ind_data.wlan_dyn_ind[i].wlan_if_control == SCM_MSGR_WLAN_IF_START_V01)
            {
              QcMapWLANSCMMgr->SCMProcessDynamicEnable(ind_data.wlan_dyn_ind[i].wlan_if_num,
                                                       ind_data.wlan_dyn_ind[i].wlan_if_type);
            }
            else if (ind_data.wlan_dyn_ind[i].wlan_if_control == SCM_MSGR_WLAN_IF_STOP_V01)
            {
              QcMapWLANSCMMgr->SCMProcessDynamicDisable(ind_data.wlan_dyn_ind[i].wlan_if_type);
            }
            else
            {
              LOG_MSG_ERROR("Unknown control option %d",ind_data.wlan_dyn_ind[i].wlan_if_control,0,0);
            }
          }
        }
        else
          LOG_MSG_ERROR("WLAN_DYNAMIC_RECONFIG but flag is false",0,0,0);

        break;

      case SCM_MSGR_WLAN_STATION_STATE_IND_V01:
        if (ind_data.wlan_sta_ind_valid)
        {
          qcmap_sta_buffer_t qcmap_sta_buffer;

          if(ind_data.wlan_sta_ind.wlan_sta_status == SCM_MSGR_WLAN_STATION_CONNECTED_V01)
          {
            //send STA connect
            qcmap_cm_process_sta_ind (0xDCDCDCDC, STA_CONNECTED);//we may need to add a seperate cookie value for SCM
          }
          else if (ind_data.wlan_sta_ind.wlan_sta_status == SCM_MSGR_WLAN_STATION_DISCONNECTED_V01)
          {
            //send STA disconnect
            qcmap_cm_process_sta_ind (0xDCDCDCDC, STA_DISCONNECTED);
          }
          else
          {
            LOG_MSG_ERROR("Unknown status %d",ind_data.wlan_sta_ind.wlan_sta_status,0,0);
          }
        }
        else
          LOG_MSG_ERROR("WLAN_DYNAMIC_RECONFIG but flag is false",0,0,0);
        break;

      case SCM_MSGR_WLAN_SYS_CONTROL_IND_V01:
        if (ind_data.wlan_sys_control_valid)
        {
          LOG_MSG_INFO1("Sys Control update %d",ind_data.wlan_sys_control,0,0);

          //The QCMAP_WLAN object does not need to be created for the priv client
          if (ind_data.wlan_sys_control == SCM_MSGR_WLAN_IF_START_V01)
          {
            //Start WLAN
            QCMAP_WLAN::EnableWLAN(&qmi_err_num,false,true);
          }
          else if (ind_data.wlan_sys_control == SCM_MSGR_WLAN_IF_STOP_V01)
          {
            //Stop WLAN
            QCMAP_WLAN::DisableWLAN(&qmi_err_num, true);
          }
        }
        else
          LOG_MSG_ERROR("WLAN_DYNAMIC_RECONFIG but flag is false",0,0,0);
        break;

      default:
        LOG_MSG_ERROR("Unknown indication type %d",ind_data.wlan_ind_type,0,0);
        break;
    }
  }
  else
  {
    LOG_MSG_ERROR("Unknown MSG ID %d",msg_id,0,0);
  }

  return;
}

/*==========================================================
  FUNCTION Constructor
==========================================================*/
/*!
@brief
  Initializes WLAN SCM variables.

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

QCMAP_WLAN_SCM::QCMAP_WLAN_SCM()
{
  qmi_idl_service_object_type scm_msgr_qmi_idl_service_object;
  uint32_t num_services = 0, num_entries = 0;
  qmi_service_info info[10];
  qmi_client_error_type qmi_error;

  QCMAP_CM_LOG_FUNC_ENTRY();  //why isn't this showing in QXDM?

  this->qmi_scm_handle = 0;

  memset(&qmi_scm_os_params, 0, sizeof(qmi_client_os_params));
  /* Create the QCCI client */
  qmi_error = qmi_client_init_instance(scm_msgr_get_service_object_v01(),
                                QMI_CLIENT_INSTANCE_ANY,
                                scm_qmi_qcmap_ind,
                                NULL,
                                &qmi_scm_os_params,
                                QCMAP_CM_QMI_TIMEOUT_VALUE,
                                &this->qmi_scm_handle);

  LOG_MSG_INFO1("qmi_client_init: %d",
                 qmi_error,0,0);

  if (qmi_error != QMI_NO_ERR)
  {
    qmi_client_release(this->qmi_scm_notifier);
    this->qmi_scm_notifier = NULL;
    LOG_MSG_ERROR("Can not init SCM client %d",
        qmi_error,0,0);
    return;
  }

  //register for WLAN indications
  scm_msgr_wlan_setup_ind_register_req_msg_v01 ind_req;
  scm_msgr_wlan_setup_ind_register_resp_msg_v01 ind_resp;

  memset(&ind_req, 0, sizeof(scm_msgr_wlan_setup_ind_register_req_msg_v01));
  memset(&ind_resp, 0, sizeof(scm_msgr_wlan_setup_ind_register_resp_msg_v01));

  qcmap_cm_wlan_paths_conf_t wlan_paths;
  memset(&wlan_paths, 0, sizeof(qcmap_cm_wlan_paths_conf_t));

  /* If config paths are not set SCM will use default values, no need to fail */
  if (QCMAP_WLAN::ReadWLANConfigPathsFromXML(&wlan_paths))
  {
    LOG_MSG_INFO1("Setting config paths",0,0,0);

    /* Primary AP config */
    ind_req.wlan_cfg[0].wlan_if_type = SCM_MSGR_WLAN_IF_SAP_1_V01;
    strncpy(ind_req.wlan_cfg[0].conf_file, wlan_paths.hostapd.path_to_hostapd_conf,
            strlen(wlan_paths.hostapd.path_to_hostapd_conf));
    strncpy(ind_req.wlan_cfg[0].entropy_file, wlan_paths.hostapd.path_to_hostapd_entropy,
            strlen(wlan_paths.hostapd.path_to_hostapd_entropy));

    if (wlan_paths.debug.hostapd_debug)
    {
      strncpy(ind_req.wlan_cfg[0].extra_cmd_line_args, wlan_paths.debug.hostapd_dbg_cmd,
              strlen(wlan_paths.debug.hostapd_dbg_cmd));
      ds_log_med("Debug %s", ind_req.wlan_cfg[0].extra_cmd_line_args);
    }

    /* Guest AP config */
    ind_req.wlan_cfg[1].wlan_if_type = SCM_MSGR_WLAN_IF_SAP_2_V01;
    strncpy(ind_req.wlan_cfg[1].conf_file, wlan_paths.hostapd_guest.path_to_hostapd_conf,
            strlen(wlan_paths.hostapd_guest.path_to_hostapd_conf));
    strncpy(ind_req.wlan_cfg[1].entropy_file, wlan_paths.hostapd_guest.path_to_hostapd_entropy,
            strlen(wlan_paths.hostapd_guest.path_to_hostapd_entropy));

    if (wlan_paths.debug.hostapd_guest_debug)
    {
      strncpy(ind_req.wlan_cfg[1].extra_cmd_line_args, wlan_paths.debug.hostapd_guest_dbg_cmd,
              strlen(wlan_paths.debug.hostapd_guest_dbg_cmd));
    }

    /* STA iface config */
    ind_req.wlan_cfg[2].wlan_if_type = SCM_MSGR_WLAN_IF_STA_1_V01;
    strncpy(ind_req.wlan_cfg[2].conf_file, wlan_paths.supplicant, strlen(wlan_paths.supplicant));

    ds_log_med("Supp path %s", wlan_paths.supplicant);

    if (wlan_paths.debug.supplicant_debug)
    {
      strncpy(ind_req.wlan_cfg[2].extra_cmd_line_args, wlan_paths.debug.supplicant_dbg_cmd,
              strlen(wlan_paths.debug.supplicant_dbg_cmd));
    }

    /* Internal STA Iface, used for CNE */
    ind_req.wlan_cfg[3].wlan_if_type = SCM_MSGR_WLAN_IF_STA_1_INT_V01;
    strncpy(ind_req.wlan_cfg[3].conf_file, wlan_paths.supplicant, strlen(wlan_paths.supplicant));
    strncpy(ind_req.wlan_cfg[3].extra_cmd_line_args, "-d", strlen("-d"));
  }

  ind_req.wlan_cfg_valid = true;
  ind_req.register_indication = 1;
  ind_req.wlan_cfg_len = 4;

  qmi_error = qmi_client_send_msg_sync(this->qmi_scm_handle,
                                       QMI_SCM_MSGR_WLAN_SETUP_IND_REG_REQ_V01,
                                       &ind_req,
                                       sizeof(scm_msgr_wlan_setup_ind_register_req_msg_v01),
                                       &ind_resp,
                                       sizeof(scm_msgr_wlan_setup_ind_register_resp_msg_v01),
                                       QCMAP_CM_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d", qmi_error,
               ind_resp.resp.result,0);

  if ((qmi_error == QMI_TIMEOUT_ERR) || (qmi_error != QMI_NO_ERR) ||
      (ind_resp.resp.result != QMI_NO_ERR))
  {
    LOG_MSG_ERROR("Failed to enable ind %d : %d",  qmi_error,
               ind_resp.resp.error,0);
  }

  return;
}

/*==========================================================
  FUNCTION Destructor
==========================================================*/
/*!
@brief
  Destroying the WLAN SCM Object.

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

QCMAP_WLAN_SCM::~QCMAP_WLAN_SCM()
{
  qmi_client_error_type qmi_error;

  QCMAP_CM_LOG_FUNC_ENTRY();

  flag=false;
  object=NULL;
  LOG_MSG_INFO1("Destroying Object: QCMAP_WLAN_SCM",0,0,0);

  qmi_error = qmi_client_release(this->qmi_scm_notifier);
  this->qmi_scm_notifier = NULL;

  if (qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Can not release client scm notifier %d",qmi_error,0,0);
  }

  qmi_error = qmi_client_release(this->qmi_scm_handle);
  this->qmi_scm_handle = NULL;

  if (qmi_error != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Can not release client scm handle %d",
        qmi_error,0,0);
  }
}


/*=====================================================================
  FUNCTION SCMEnableWLAN
======================================================================*/
/*!
@brief
  Return true if SCM was able to enable WLAN in mode

@parameters
  int mode

@return
  true  - Enable WLAN succeeded
  false - Enable WLAN failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN_SCM::SCMEnableWLAN(int mode)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  QCMAP_WLAN_SCM* QcMapWLANSCMMgr=QCMAP_WLAN_SCM::Get_Instance(true);

  char command[MAX_COMMAND_STR_LEN];
  struct in_addr addr;
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  qmi_client_error_type qmi_error;
  scm_msgr_wlan_enable_req_msg_v01 req;
  scm_msgr_wlan_enable_resp_msg_v01 resp;
  int num, type, i;

  if(QcMapWLANMgr == NULL)
  {
    LOG_MSG_ERROR("WLAN object is NULL", 0,0,0);
    return false;
  }

  memset(&req, 0, sizeof(scm_msgr_wlan_enable_req_msg_v01));
  memset(&resp, 0, sizeof(scm_msgr_wlan_enable_resp_msg_v01));

  LOG_MSG_INFO1("SCM Enable WLAN mode %d", mode, 0,0);

  req.wlan_concurrency_cfg = mode;
  qmi_error = qmi_client_send_msg_sync(QcMapWLANSCMMgr->qmi_scm_handle,
                                       QMI_SCM_MSGR_WLAN_ENABLE_REQ_V01,
                                       &req,
                                       sizeof(scm_msgr_wlan_enable_req_msg_v01),
                                       &resp,
                                       sizeof(scm_msgr_wlan_enable_resp_msg_v01),
                                       QCMAP_CM_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d", qmi_error, resp.resp.result,0);

  if ((qmi_error == QMI_TIMEOUT_ERR) || (qmi_error != QMI_NO_ERR) ||
      (resp.resp.result != QMI_NO_ERR))
  {
    LOG_MSG_ERROR("Failed enable wlan %d : %d", qmi_error,resp.resp.error,0);
    return false;
  }

  LOG_MSG_INFO1("Number of new Wlan interfaces %d", resp.wlan_concurrency_cfg_len,0,0);
  for (i = 0; i < resp.wlan_concurrency_cfg_len; i++)
  {
    LOG_MSG_INFO1("WLAN IF Num: %d, Type: %d",
                  resp.wlan_concurrency_cfg[i].wlan_if_num,
                  resp.wlan_concurrency_cfg[i].wlan_if_type,0);

    //need to store if_name correctly based on type
    num = resp.wlan_concurrency_cfg[i].wlan_if_num;
    type = resp.wlan_concurrency_cfg[i].wlan_if_type;

    //store first AP num in QcMapMgr->ap_dev_num, all other AP's should immediately follow
    switch (type)
    {
    case SCM_MSGR_WLAN_IF_SAP_1_V01:
      QcMapMgr->ap_dev_num1 = num;
      QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_PRIMARY_AP_V01);
      break;
    case SCM_MSGR_WLAN_IF_SAP_2_V01:
      QcMapMgr->ap_dev_num2 = num;
      QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_GUEST_AP_V01);
      break;
    case SCM_MSGR_WLAN_IF_STA_1_V01:
    case SCM_MSGR_WLAN_IF_STA_1_INT_V01:
      QcMapMgr->sta_iface_index = num;
      break;
    default:
      LOG_MSG_ERROR("Unsupported WLAN iface type %d", resp.wlan_concurrency_cfg[i].wlan_if_type,0,0);
      break;
    }

    if (type == SCM_MSGR_WLAN_IF_SAP_1_V01 || type == SCM_MSGR_WLAN_IF_SAP_2_V01)
    {
      /* Add the interfaces to the bridge. */
      if(!QcMapWLANMgr->AssociateWLANtoBridge(num))
        LOG_MSG_ERROR("Failed to associate wlan%d to bridge", num,0,0);

      /* Config WLAN interface IP */
      addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[i].ll_ip_addr);
      strlcpy(a5_ip, inet_ntoa(addr), 16);
      addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg
                    .interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(netmask, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig wlan%d %s netmask %s up",
               num, a5_ip, netmask);
      ds_system_call(command, strlen(command));
      addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[i].ll_ip_addr
       & QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
      strlcpy(subnet, inet_ntoa(addr), 16);
      snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev wlan%d",
               subnet, netmask, num);
      ds_system_call(command, strlen(command));
      /* First delete the link-local route. */
      snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev wlan%d",num);
      ds_system_call(command, strlen(command));
    }
  }
  return true;
}

/*=====================================================================
  FUNCTION SCMDisableWLAN
======================================================================*/
/*!
@brief
  Return true if SCM was able to enable WLAN in mode

@parameters

@return
  true  - Enable WLAN succeeded
  false - Enable WLAN failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
int QCMAP_WLAN_SCM::SCMDisableWLAN()
{
  qmi_client_error_type qmi_error;
  scm_msgr_wlan_disable_req_msg_v01 req;
  scm_msgr_wlan_disable_resp_msg_v01 resp;

  memset(&req, 0, sizeof(scm_msgr_wlan_disable_req_msg_v01));
  memset(&resp, 0, sizeof(scm_msgr_wlan_disable_resp_msg_v01));

  qmi_error = qmi_client_send_msg_sync(this->qmi_scm_handle,
                                       QMI_SCM_MSGR_WLAN_DISABLE_REQ_V01,
                                       &req,
                                       sizeof(scm_msgr_wlan_disable_req_msg_v01),
                                       &resp,
                                       sizeof(scm_msgr_wlan_disable_resp_msg_v01),
                                       QCMAP_CM_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d",
          qmi_error, resp.resp.result,0);

  if ((qmi_error == QMI_TIMEOUT_ERR) || (qmi_error != QMI_NO_ERR) ||
      (resp.resp.result != QMI_NO_ERR)) {
    LOG_MSG_ERROR("Failed to disable wlan %d : %d",  qmi_error,
            resp.resp.error,0);
    return false;
  }

  return true;
}


/*=====================================================================
  FUNCTION DualWifiEnabled
======================================================================*/
/*!
@brief
  Return true if Dual wifi mode is enabled

@parameters

@return
  true  - Dual Wifi enabled
  false - Legacy (Rome) solution

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
boolean QCMAP_WLAN_SCM::DualWifiEnabled()
{
  FILE *stream = NULL;
  char subtype = ' ';
  stream = fopen("/sys/devices/soc0/platform_subtype_id", "rt");
  if(NULL == stream)
  {
    LOG_MSG_INFO1("Failed to open CDT config",0,0,0);
    return false;
  }

  subtype = fgetc(stream);

  if (EOF != subtype)
  {
    if(CDT_DUAL_WIFI == atoi(&subtype))
    {
      LOG_MSG_INFO1("Dual Wifi Enabled",0,0,0);
      fclose(stream);
      return true;
    }
  }
  fclose(stream);
  return false;

}



/*=====================================================================
  FUNCTION SCMReconfig
======================================================================*/
/*!
@brief
  Return true if SCM was able to reconfigure to new mode

@parameters

@return
  true  - Reconfig WLAN succeeded
  false - Reconfig WLAN failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
int QCMAP_WLAN_SCM::SCMReconfig(scm_msgr_wlan_if_type_enum_v01 mode, scm_msgr_wlan_if_control_enum_v01 control_option)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  scm_msgr_wlan_if_reconfig_req_msg_v01 req;
  scm_msgr_wlan_if_reconfig_resp_msg_v01 resp;
  qmi_client_error_type qmi_error;
  int i, num, type;

  memset(&req, 0, sizeof(scm_msgr_wlan_if_reconfig_req_msg_v01));
  memset(&resp, 0, sizeof(scm_msgr_wlan_if_reconfig_resp_msg_v01));

  req.wlan_if_type_len = 1;
  req.wlan_if_type[0] = mode;
  req.wlan_if_control_len = 1;
  req.wlan_if_control[0] = control_option;

  LOG_MSG_INFO1("Reconfig %d with option %d", mode, control_option,0);

  qmi_error = qmi_client_send_msg_sync(this->qmi_scm_handle,
                                       QMI_SCM_MSGR_WLAN_IF_RECONFIG_REQ_V01,
                                       &req,
                                       sizeof(scm_msgr_wlan_if_reconfig_req_msg_v01),
                                       &resp,
                                       sizeof(scm_msgr_wlan_if_reconfig_resp_msg_v01),
                                       QCMAP_CM_QMI_TIMEOUT_VALUE);

  LOG_MSG_INFO1("qmi_client_send_msg_sync: error %d result %d",
          qmi_error, resp.resp.result,0);

  if ((qmi_error == QMI_TIMEOUT_ERR) || (qmi_error != QMI_NO_ERR) ||
      (resp.resp.result != QMI_NO_ERR)) {
    LOG_MSG_ERROR("Failed WLAN IF recfg %d : %d",qmi_error,resp.resp.error,0);
    return false;
  }

  LOG_MSG_INFO1("Number of new Wlan interfaces %d", resp.wlan_concurrency_cfg_len,0,0);
  for (i = 0; i < resp.wlan_concurrency_cfg_len; i++)
  {
    LOG_MSG_INFO1("%d %d", resp.wlan_concurrency_cfg_len,i,0);
    LOG_MSG_INFO1("WLAN IF Name: %d, Type: %d",
            resp.wlan_concurrency_cfg[i].wlan_if_num,
            resp.wlan_concurrency_cfg[i].wlan_if_type,0);

    //need to store if_name correctly based on type
    num = resp.wlan_concurrency_cfg[i].wlan_if_num;
    type = resp.wlan_concurrency_cfg[i].wlan_if_type;

    //store first AP num in QcMapMgr->ap_dev_num, all other AP's should immediately follow
    switch (type)
    {
    case SCM_MSGR_WLAN_IF_SAP_1_V01:
      QcMapMgr->ap_dev_num1 = num;

      if (control_option == SCM_MSGR_WLAN_IF_START_V01 ||
          control_option == SCM_MSGR_WLAN_IF_RESTART_V01)
        QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_PRIMARY_AP_V01);
      else
        QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_PRIMARY_AP_V01);
      break;

    case SCM_MSGR_WLAN_IF_SAP_2_V01:
      QcMapMgr->ap_dev_num2 = num;

      if (control_option == SCM_MSGR_WLAN_IF_START_V01 ||
          control_option == SCM_MSGR_WLAN_IF_RESTART_V01)
        QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_GUEST_AP_V01);
      else
        QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_GUEST_AP_V01);
      break;

    case SCM_MSGR_WLAN_IF_STA_1_V01:
    case SCM_MSGR_WLAN_IF_STA_1_INT_V01:
      QcMapMgr->sta_iface_index = num;
      break;

    default:
      LOG_MSG_ERROR("Unsupported WLAN iface type %d", resp.wlan_concurrency_cfg[i].wlan_if_type,0,0);
      break;
    }
  }

  return true;
}


/*=====================================================================
  FUNCTION SCMProcessDynamicEnable
======================================================================*/
/*!
@brief
  Process the indication data for Enable iface

@parameters

@return
  true  - Reconfig WLAN succeeded
  false - Reconfig WLAN failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
int QCMAP_WLAN_SCM::SCMProcessDynamicEnable(int iface_num, int type)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];
  int index = 0;
  struct in_addr addr;
  char a5_ip[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char netmask[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];
  char subnet[QCMAP_LAN_MAX_IPV4_ADDR_SIZE];

  /* Enable IP forwarding */
  ds_system_call("echo 1 > /proc/sys/net/ipv4/ip_forward",
                 strlen("echo 1 > /proc/sys/net/ipv4/ip_forward")); //may not be needed...***********************

  //store first AP num in QcMapMgr->ap_dev_num, all other AP's should immediately follow
  //Check with Mani, STA should either be before all AP's or after, otherwise all logic needs to change
  switch (type)
  {
  case SCM_MSGR_WLAN_IF_SAP_1_V01:
    QcMapMgr->ap_dev_num1 = iface_num;
    QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_PRIMARY_AP_V01);

    /* Primary AP index will always be 0 in wlan_cfg.interface[] */
    index = 0;
    break;
  case SCM_MSGR_WLAN_IF_SAP_2_V01:
    QcMapMgr->ap_dev_num2 = iface_num;
    QcMapWLANMgr->StartHostapdCli(QCMAP_MSGR_GUEST_AP_V01);

    /* Guest AP index will always be 1 in wlan_cfg.interface[] */
    index = 1;
    break;
  case SCM_MSGR_WLAN_IF_STA_1_V01:
  case SCM_MSGR_WLAN_IF_STA_1_INT_V01:
    QcMapMgr->sta_iface_index = iface_num;
    break;
  default:
    LOG_MSG_ERROR("Unsupported WLAN iface type %d", type,0,0);
    return false;
  }

  if (type == SCM_MSGR_WLAN_IF_SAP_1_V01 || type == SCM_MSGR_WLAN_IF_SAP_2_V01)
  {
    /* Add the interfaces to the bridge. */
    if(!QcMapWLANMgr->AssociateWLANtoBridge(iface_num))
      LOG_MSG_ERROR("Failed to associate wlan%d to bridge", iface_num,0,0);

    /* Config WLAN interface IP */
    addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[index].ll_ip_addr);
    strlcpy(a5_ip, inet_ntoa(addr), 16);
    addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg
                  .interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
    strlcpy(netmask, inet_ntoa(addr), 16);
    snprintf(command, MAX_COMMAND_STR_LEN, "ifconfig wlan%d %s netmask %s up",
             iface_num, a5_ip, netmask);
    ds_system_call(command, strlen(command));
    addr.s_addr = htonl(QcMapWLANMgr->wlan_cfg.interface[index].ll_ip_addr
     & QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].ll_subnet_mask);
    strlcpy(subnet, inet_ntoa(addr), 16);
    snprintf(command, MAX_COMMAND_STR_LEN, "route del -net %s netmask %s dev wlan%d",
             subnet, netmask, iface_num);
    ds_system_call(command, strlen(command));
    /* First delete the link-local route. */
    snprintf(command, MAX_COMMAND_STR_LEN, "ip -6 route del fe80::/64 dev wlan%d",iface_num);
    ds_system_call(command, strlen(command));
  }

  /* Enable Station Mode. */
  if(NULL != QcMapBackhaulWLANMgr && (type == SCM_MSGR_WLAN_IF_STA_1_V01||
                                      type == SCM_MSGR_WLAN_IF_STA_1_INT_V01))
    QcMapBackhaulWLANMgr->EnableStaMode();

  /* Install access restrictions for the Second SSID if the profile is
  INTERNETONLY.*/
  if (type == SCM_MSGR_WLAN_IF_SAP_2_V01)
  {
    if (QcMapWLANMgr->wlan_cfg.interface[QCMAP_MSGR_INTF_GUEST_AP_INDEX].access_profile ==
       QCMAP_MSGR_PROFILE_INTERNET_ONLY_V01)
    {
      /* Disable InterBSS in WLAN Driver.. */
      QcMapWLANMgr->InstallGuestAPAccessRules();
    }
    else
    {
      QcMapWLANMgr->SetIPAWLANMode(QCMAP_MSGR_PROFILE_FULL_ACCESS_V01);
    }
  }

  return true;
}

/*=====================================================================
  FUNCTION SCMProcessDynamicDisable
======================================================================*/
/*!
@brief
  Process the indication data for Disable iface

@parameters

@return
  true  - Reconfig WLAN succeeded
  false - Reconfig WLAN failed

@note
- Dependencies
- None

- Side Effects
- None
*/
/*====================================================================*/
int QCMAP_WLAN_SCM::SCMProcessDynamicDisable(int type)
{
  QCMAP_ConnectionManager* QcMapMgr=QCMAP_ConnectionManager::Get_Instance(NULL,false);
  QCMAP_Backhaul_WLAN* QcMapBackhaulWLANMgr=QCMAP_Backhaul_WLAN::Get_Instance(false);
  QCMAP_Backhaul* QcMapBackhaulMgr=QCMAP_Backhaul::Get_Instance(false);
  QCMAP_WLAN* QcMapWLANMgr=QCMAP_WLAN::Get_Instance(false);
  char command[MAX_COMMAND_STR_LEN];
  qcmap_msgr_device_type_enum_v01 device_type =
    QCMAP_MSGR_DEVICE_TYPE_ENUM_MIN_ENUM_VAL_V01;

  /* hostapd_cli is needed for CDI in dual wifi and legacy modes */
  /*Stop all instances of hostapd_cli */
  if (type = SCM_MSGR_WLAN_IF_SAP_1_V01)
  {
    QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_PRIMARY_AP_V01);
  }
  else if (type = SCM_MSGR_WLAN_IF_SAP_2_V01)
  {
    QcMapWLANMgr->StopHostapdCli(QCMAP_MSGR_GUEST_AP_V01);
  }

  /* Disable station mode iface. */
  if(NULL != QcMapBackhaulWLANMgr && (type == SCM_MSGR_WLAN_IF_STA_1_V01||
                                      type == SCM_MSGR_WLAN_IF_STA_1_INT_V01))
  {
    QcMapBackhaulWLANMgr->DisableStaMode();
    QcMapMgr->sta_iface_index = QCMAP_LAN_INVALID_IFACE_INDEX;
  }

  /* AP-STA Bridge Mode cleanup*/
  if (type == SCM_MSGR_WLAN_IF_STA_1_V01 && QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    QcMapMgr->CleanEbtables();
    QcMapMgr->CleanIPtables();
    QcMapBackhaulMgr->StopDHCPCD(BRIDGE_IFACE);
    QcMapBackhaulMgr->BlockIPv4WWANAccess();//Need to put blocking rules since all have been deleted.
    /*Set accept_ra flag to 1*/
    snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
             BRIDGE_IFACE);
    ds_system_call(command, strlen(command));
    ds_system_call("ip6tables --flush FORWARD",strlen("ip6tables --flush FORWARD")); //need to remove icmp6 rules
    QcMapBackhaulMgr->BlockIPv6WWANAccess();
  }

  /* Delete all the WLAN clients from the Connected Devices List */
  if (type == SCM_MSGR_WLAN_IF_SAP_1_V01)
  {
    device_type = QCMAP_MSGR_DEVICE_TYPE_PRIMARY_AP_V01;
    QcMapMgr->DeleteConnectedDevicesClients(device_type);
  }
  else if (type == SCM_MSGR_WLAN_IF_SAP_2_V01)
  {
    device_type = QCMAP_MSGR_DEVICE_TYPE_GUEST_AP_V01;
    QcMapMgr->DeleteConnectedDevicesClients(device_type);
  }

  /* AP-STA Bridge Mode cleanup*/
  if (type == SCM_MSGR_WLAN_IF_STA_1_V01 && QCMAP_Backhaul_WLAN::IsAPSTABridgeActivated())
  {
    QcMapMgr->CleanEbtables();
    QcMapMgr->CleanIPtables();
    QcMapBackhaulMgr->StopDHCPCD(BRIDGE_IFACE);
    QcMapBackhaulMgr->BlockIPv4WWANAccess();//Need to put blocking rules since all have been deleted.
    /*Set accept_ra flag to 1*/
    snprintf(command, MAX_COMMAND_STR_LEN, "echo 1 > /proc/sys/net/ipv6/conf/%s/accept_ra",
             BRIDGE_IFACE);
    ds_system_call(command, strlen(command));
    ds_system_call("ip6tables --flush FORWARD",strlen("ip6tables --flush FORWARD")); //need to remove icmp6 rules
    QcMapBackhaulMgr->BlockIPv6WWANAccess();
  }

  /* Reset STA mode specific parameters */
  if(type == SCM_MSGR_WLAN_IF_STA_1_V01 &&  NULL != QcMapBackhaulWLANMgr)
  {
    QcMapBackhaulWLANMgr->sta_v4_available = false;
    QcMapBackhaulWLANMgr->sta_v6_available = false;
    QcMapBackhaulWLANMgr->ap_sta_bridge_mode_activated = false;
    QcMapBackhaulMgr->ClearIfaceRelatedRulesForV6(QcMapBackhaulWLANMgr->apsta_cfg.sta_interface);
  }
}
