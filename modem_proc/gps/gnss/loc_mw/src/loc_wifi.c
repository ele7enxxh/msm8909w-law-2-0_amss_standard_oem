/******************************************************************************
  @file: loc_wifi.c
  @brief: Location API WiFi positioning module

  DESCRIPTION
   Qualcomm Location API WiFi Positioning Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
 
  Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
  All Rights Reserved.
  QCA Proprietary and Confidential. 

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_wifi.c#1 $
$DateTime: 2016/12/13 07:59:45 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
01/11/16   yh       LocWifi cleanup
09/05/14   ssu      GM LOWI Integration
07/24/09   dx       Initial version
======================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "msg.h"

#include "pdapi.h"
#include "loc_api_2.h"

#include "loc_task.h"
#include "loc_client.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"
#include "loc_utils.h"
#include "loc_qmi_shim.h"

// Prototypes
static void locWifi_EventCb (
   void *p_UserData,
   pdsm_pd_event_type q_PdEvent,
   const pdsm_pd_info_s_type *pz_PdInfo
);

static boolean locWifi_SendQmiIndicationProxy
(
   const loc_client_handle_type l_ClientHandle,
   const uint32                 q_CmdType,
   const uint32                 q_CmdInfo,
   const uint32                 q_ParamStatus
);

/*---------------------------------------------------------------------------
@brief
   One time initializations for the WiFi module. This function is to be called 
   only once.

@param[in] None

@retval    TRUE  if the Init was successful 
@retval    FALSE  if the Init failed
---------------------------------------------------------------------------*/
boolean locWifi_Init()
{
   boolean                       v_RetVal = FALSE; /* default: not successful */
   pdsm_client_status_e_type     e_Status = PDSM_CLIENT_OK;

   LOC_MSG_MED("locWifi_Init: entered", 0, 0, 0);

   do { /* one time loop */

      // Check if this is valid when QWiP is updated
      LOC_MSG_HIGH("locWifi_Init: installing a hook in PD client", 0, 0, 0);
      e_Status = loc_pd_install_event_cb_hook(locWifi_EventCb);
      if (e_Status != PDSM_CLIENT_OK)
      {
         LOC_MSG_ERROR ("loc_pd_install_event_cb_hook failed, error code = %d", e_Status, 0, 0);
         break;
      }

      loc_middleware_data.wifi_client_id = pdsm_client_init(PDSM_CLIENT_TYPE_WIPER);
      if (loc_middleware_data.wifi_client_id == -1)
      {
         LOC_MSG_HIGH("locWifi_Init: pdsm_client_init for WiFi failed", 0, 0, 0);
         // break;
      }


      /* Initialize the Critical Section */
      os_MutexInit( &loc_middleware_data.wifi_crit_sec, MUTEX_DATA_ONLY_CONTEXT );


      LOC_MSG_HIGH("locWifi_Init: pdsm_client_init for WiFi successful, client id=%d",
            (int) loc_middleware_data.wifi_client_id, 0, 0);

      // Check if WiFi module needs callbacks
      /*
      e_Status = pdsm_client_pd(wifi)_reg(loc_middleware_data.wifi_client_id,
                                    NULL,
                                    locWifi_EventCb,
                                    PDSM_CLIENT_EVENT_REG,
                                    PDSM_PD_EVENT_WPS_NEEDED,
                                    NULL);
      if (e_Status != PDSM_CLIENT_OK)
      {
         pdsm_client_release(loc_middleware_data.wifi_client_id);
         LOC_MSG_ERROR ("pdsm_client_pd_reg failed, error code = %d", e_Status, 0, 0);
         break;
      }

      e_Status = pdsm_client_act(loc_middleware_data.wifi_client_id);
      if (e_Status != PDSM_CLIENT_OK)
      {
         pdsm_client_release(loc_middleware_data.wifi_client_id);
         LOC_MSG_ERROR ("pdsm_client_act failed, error code =%d", e_Status, 0, 0);
         break;
      }
      */

      // Reaching here means success
      v_RetVal = TRUE;
   } while (0); /* one time loop */

   LOC_MSG_MED ("locWifi_Init: returned %d", v_RetVal, 0, 0);
   return v_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  This function processes wiper position report injection callback

@param[in] 
  p_UserData        Pointer to client provided data block 
  e_WiperCmd        Indicate which command is this error e_Status for
  e_WiperCmdErr     Command error code, indicating whether command
                    is rejected and the reason

@retval    None
---------------------------------------------------------------------------*/
static void locWifi_CmdCb (
   void                      *p_UserData,
   pdsm_wiper_cmd_e_type      e_WiperCmd,
   pdsm_wiper_cmd_err_e_type  e_WiperCmdErr
)
{
   LOC_MSG_HIGH ("locWifi_CmdCb: cmd=%d, err=%d\n", e_WiperCmd, e_WiperCmd, 0);
}

/*---------------------------------------------------------------------------
@brief
  This function processes wiper position report injection

@param[in] 
  l_ClientHandle                   LocMW client handle
  pz_NotifyWifiEnabledStatus       QMI_LOC input pointer

@retval    Loc API return values (e.g., LOC_API_SUCCESS (or 0) for success)
---------------------------------------------------------------------------*/
static int locWifi_InjectWifiPos
(
  loc_client_handle_type                            l_ClientHandle,
  const qmiLocInjectWifiPositionReqMsgT_v02*        pz_InjectWifiPos
)
{
   t_wiper_position_report_ex_struct_type *pz_PosReport= NULL;
   uint32                                  q_RetVal = LOC_API_SUCCESS;

   /* Function arguments sanity check */
   if ((NULL == pz_InjectWifiPos) || (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle))
   {
      LOC_MSG_ERROR("locWifi_InjectWifiPos: NULL parameters", 0, 0, 0);
      return LOC_API_INVALID_PARAMETER;
   }

   pz_PosReport = (t_wiper_position_report_ex_struct_type *)loc_calloc(sizeof(t_wiper_position_report_ex_struct_type));
   if (NULL == pz_PosReport)
   {
      LOC_MSG_ERROR("locWifi_InjectWifiPos: alloc failed", 0, 0, 0);
      return LOC_API_GENERAL_FAILURE;
   }

   do
   {
      /***************************
       *  Copying the data       *
       ***************************/
      if(pz_InjectWifiPos->wifiFixTime_valid)
      {
         pz_PosReport->wiper_pos_report.wiper_valid_info_flag |= WIPER_LOG_TIME_VALID;
         pz_PosReport->wiper_pos_report.wiper_fix_time.slow_clock_count = pz_InjectWifiPos->wifiFixTime.wifiPositionTime;
      }

      if(pz_InjectWifiPos->wifiFixPosition_valid)
      {
         pz_PosReport->wiper_pos_report.wiper_valid_info_flag |= WIPER_LOG_POS_VALID;
         pz_PosReport->wiper_pos_report.wiper_fix_position.lat = pz_InjectWifiPos->wifiFixPosition.lat;
         pz_PosReport->wiper_pos_report.wiper_fix_position.lon = pz_InjectWifiPos->wifiFixPosition.lon;
         pz_PosReport->wiper_pos_report.wiper_fix_position.HEPE = pz_InjectWifiPos->wifiFixPosition.hepe;
         pz_PosReport->wiper_pos_report.wiper_fix_position.num_of_aps_used = pz_InjectWifiPos->wifiFixPosition.numApsUsed;
         pz_PosReport->wiper_pos_report.wiper_fix_position.fix_error_code = pz_InjectWifiPos->wifiFixPosition.fixErrorCode;
      }

      if( ( pz_InjectWifiPos->apInfo_valid ) &&
         ( pz_InjectWifiPos->wifiApSsidInfo_valid )  &&
         ( pz_InjectWifiPos->apInfo_len  != pz_InjectWifiPos->wifiApSsidInfo_len)
      )
      {
         LOC_MSG_ERROR("locWifi_InjectWifiPos: Ap info length %d and SSID info length %dnot matching",
                        pz_InjectWifiPos->apInfo_len,pz_InjectWifiPos->wifiApSsidInfo_len,0);
         q_RetVal = LOC_API_INVALID_PARAMETER;
         break;
      }

      if(pz_InjectWifiPos->apInfo_valid)
      {
         uint8 u_ApCntr;
         pz_PosReport->wiper_pos_report.wiper_valid_info_flag |= WIPER_LOG_AP_SET_VALID;
         pz_PosReport->wiper_pos_report.wiper_ap_set.num_of_aps = 
                   (uint8) MIN ( QMI_LOC_WIFI_MAX_REPORTED_APS_PER_MSG_V02,
                                 pz_InjectWifiPos->apInfo_len );
         for ( u_ApCntr = 0;
               u_ApCntr < pz_PosReport->wiper_pos_report.wiper_ap_set.num_of_aps;
               u_ApCntr++ )
         {
           pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].ap_qualifier =
                                 pz_InjectWifiPos->apInfo[u_ApCntr].apQualifier;

           pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].rssi = 
                                 pz_InjectWifiPos->apInfo[u_ApCntr].rssi;

           pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].channel =
                                 pz_InjectWifiPos->apInfo[u_ApCntr].channel;

           memscpy(pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].mac_addr,
              sizeof(pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].mac_addr),
              pz_InjectWifiPos->apInfo[u_ApCntr].macAddr,
              sizeof(pz_InjectWifiPos->apInfo[u_ApCntr].macAddr));

           if(pz_InjectWifiPos->wifiApSsidInfo_valid)
           {
             GNSS_STRLCPY((char *)pz_PosReport->wiper_pos_report.wiper_ap_set.ap_info[u_ApCntr].ssid,
                          pz_InjectWifiPos->wifiApSsidInfo[u_ApCntr].ssid,
                          QMI_LOC_MAX_WIFI_AP_SSID_STR_LENGTH_V02);
           }
         }
      }

      if(pz_InjectWifiPos->horizontalReliability_valid)
      {
         switch(pz_InjectWifiPos->horizontalReliability)
         {
            case eQMI_LOC_RELIABILITY_NOT_SET_V02:
               pz_PosReport->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_NOT_SET;
               break;
            case eQMI_LOC_RELIABILITY_VERY_LOW_V02:
               pz_PosReport->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_VERY_LOW;
               break;
            case eQMI_LOC_RELIABILITY_LOW_V02:
               pz_PosReport->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_LOW;
               break;
            case eQMI_LOC_RELIABILITY_MEDIUM_V02:
               pz_PosReport->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_MEDIUM;
               break;
            case eQMI_LOC_RELIABILITY_HIGH_V02:
               pz_PosReport->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_HIGH;
               break;
            default:
               pz_PosReport->e_HoriRelIndicator = PDSM_POSITION_RELIABILITY_NOT_SET;
               break;
         }
      }

      /* Note: rawHepe has not been used for WIFI position yet */

      /***************************
       *  Makes the PDSM call    *
       ***************************/
      // XXX check if the client_id should be wifi_client_id
      pdsm_send_wiper_position_report_ex(locWifi_CmdCb,
             loc_middleware_data.wifi_client_id,      // Client id of user
             NULL,                                    // Pointer to client data block
             pz_PosReport);

      /* Send Indication */
      locWifi_SendQmiIndicationProxy(l_ClientHandle,
                  QMI_LOC_INJECT_WIFI_POSITION_REQ_V02,
                  0, LOC_API_SUCCESS);
   }while(0);

   loc_free(pz_PosReport);

   return LOC_API_SUCCESS;
}

/*---------------------------------------------------------------------------
@brief
  This function processes wiper e_Status report

@param[in] 
  l_ClientHandle                   LocMW client handle
  pz_NotifyWifiEnabledStatus       QMI_LOC input pointer

@retval    Loc API return values (e.g., LOC_API_SUCCESS (or 0) for success)
---------------------------------------------------------------------------*/
static int locWifi_NotifyWifiStatus
(
   loc_client_handle_type                          l_ClientHandle,
   const qmiLocNotifyWifiStatusReqMsgT_v02*        pz_NotifyWifiStatus
)
{
   uint32       q_RetVal = LOC_API_SUCCESS;
   boolean      v_WiperStatus = FALSE;

  /* Function arguments sanity check */
  if ((NULL == pz_NotifyWifiStatus) || (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle))
  {
    LOC_MSG_ERROR("locWifi_NotifyWifiStatus: NULL parameters", 0, 0, 0);
    return LOC_API_INVALID_PARAMETER;
  }

   /***************************
    *  Makes the PDSM call    *
    ***************************/
   switch (pz_NotifyWifiStatus->wifiStatus)
   {
      case eQMI_LOC_WIFI_STATUS_AVAILABLE_V02:
         v_WiperStatus = TRUE;
         break;
      case eQMI_LOC_WIFI_STATUS_UNAVAILABLE_V02:
         v_WiperStatus = FALSE;
         break;
      default:
         q_RetVal = LOC_API_INVALID_PARAMETER;
   }

   if(LOC_API_SUCCESS == q_RetVal)
   {
      pdsm_notify_wiper_status(v_WiperStatus);
   }

   /* Send Indication */
   locWifi_SendQmiIndicationProxy(l_ClientHandle,
               QMI_LOC_NOTIFY_WIFI_STATUS_REQ_V02,
               0, q_RetVal);

   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  This function processes wiper attachment e_Status report

@param[in] 
  l_ClientHandle                   LocMW client handle
  pz_NotifyWifiEnabledStatus       QMI_LOC input pointer

@retval    Loc API return values (e.g., LOC_API_SUCCESS (or 0) for success)
---------------------------------------------------------------------------*/
static int locWifi_NotifyWifiAttachStatus
(
   loc_client_handle_type                              l_ClientHandle,
   const qmiLocNotifyWifiAttachmentStatusReqMsgT_v02*  pz_NotifyWifiAttachStatus
)
{
   uint32       q_RetVal = LOC_API_SUCCESS;
   pdsm_wifi_attachment_status_ind_info z_WifiAttachStatusIndInfo = {0};
   int size = sizeof(z_WifiAttachStatusIndInfo);

   /* Function arguments sanity check */
   if ((NULL == pz_NotifyWifiAttachStatus) || (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle))
   {
      LOC_MSG_ERROR("locWifi_NotifyWifiAttachStatus: NULL parameters", 0, 0, 0);
      return LOC_API_INVALID_PARAMETER;
   }

   /***************************
    *  Makes the PDSM call    *
    ***************************/
   switch (pz_NotifyWifiAttachStatus->attachState)
   {
     case eQMI_LOC_WIFI_ACCESS_POINT_ATTACHED_V02:
        z_WifiAttachStatusIndInfo.wifi_attachment_status_ind_type = WIFI_ATTACHMENT_STATUS_CONNECTED;
        break;
      case eQMI_LOC_WIFI_ACCESS_POINT_DETACHED_V02:
        z_WifiAttachStatusIndInfo.wifi_attachment_status_ind_type = WIFI_ATTACHMENT_STATUS_DISCONNECTED;
        break;
     case eQMI_LOC_WIFI_ACCESS_POINT_HANDOVER_V02:
        z_WifiAttachStatusIndInfo.wifi_attachment_status_ind_type = WIFI_ATTACHMENT_STATUS_HANDOVER;
        break;
     default:
        q_RetVal = LOC_API_INVALID_PARAMETER;
   }

   if(TRUE == pz_NotifyWifiAttachStatus->accessPointMacAddress_valid)
   {
      memscpy(z_WifiAttachStatusIndInfo.u.wifi_ap_info.mac_addr,
           sizeof(z_WifiAttachStatusIndInfo.u.wifi_ap_info.mac_addr),
           pz_NotifyWifiAttachStatus->accessPointMacAddress,
           sizeof(pz_NotifyWifiAttachStatus->accessPointMacAddress));
   }

   if(TRUE == pz_NotifyWifiAttachStatus->wifiApSsid_valid)
   {
      GNSS_STRLCPY((char *)z_WifiAttachStatusIndInfo.u.wifi_ap_info.ssid,
                   pz_NotifyWifiAttachStatus->wifiApSsid,
                   QMI_LOC_MAX_WIFI_AP_SSID_STR_LENGTH_V02);
   }


   if(LOC_API_SUCCESS == q_RetVal)
   {
      pdsm_wifi_attachment_status_ind_report(&z_WifiAttachStatusIndInfo);
   }

   /* Send Indication */
   locWifi_SendQmiIndicationProxy(l_ClientHandle,
               QMI_LOC_NOTIFY_WIFI_ATTACHMENT_STATUS_REQ_V02,
               0, q_RetVal);

   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  This function processes wifi enabled e_Status

@param[in] 
  l_ClientHandle                   LocMW client handle
  pz_NotifyWifiEnabledStatus       QMI_LOC input pointer

@retval    Loc API return values (e.g., LOC_API_SUCCESS (or 0) for success)
---------------------------------------------------------------------------*/
static int locWifi_NotifyWifiEnabledStatus
(
   loc_client_handle_type                            l_ClientHandle,
   const qmiLocNotifyWifiEnabledStatusReqMsgT_v02*   pz_NotifyWifiEnabledStatus
)
{
   uint32                            q_RetVal = LOC_API_SUCCESS;
   pdsm_wifi_enabled_status_ind_type e_WifiEnabledStatus;

  /* Function arguments sanity check */
  if ((NULL == pz_NotifyWifiEnabledStatus) || (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle))
  {
    LOC_MSG_ERROR("locWifi_NotifyWifiEnabledStatus: NULL parameters", 0, 0, 0);
    return LOC_API_INVALID_PARAMETER;
  }

   /***************************
    *  Makes the PDSM call    *
    ***************************/
   switch (pz_NotifyWifiEnabledStatus->enabledStatus)
   {
      case eQMI_LOC_WIFI_ENABLED_TRUE_V02:
         e_WifiEnabledStatus = WIFI_ENABLED_STATUS_TRUE;
         break;
      case eQMI_LOC_WIFI_ENABLED_FALSE_V02:
         e_WifiEnabledStatus = WIFI_ENABLED_STATUS_FALSE;
         break;
      default:
         q_RetVal = LOC_API_INVALID_PARAMETER;
   }

   if(LOC_API_SUCCESS == q_RetVal)
   {
      pdsm_wifi_enabled_status_report(e_WifiEnabledStatus);
   }

   /* Send Indication */
   locWifi_SendQmiIndicationProxy(l_ClientHandle,
               QMI_LOC_NOTIFY_WIFI_ENABLED_STATUS_REQ_V02,
               0, q_RetVal);

   return q_RetVal;
}

/*---------------------------------------------------------------------------
@brief
  PD event callback, this is where WPS_NEEDED event will come

@param[in] 
  p_UserData      user data pointer 
  q_PdEvent       Indicate PD event
  pz_PdInfo       PD info pointer

@retval    None
---------------------------------------------------------------------------*/
static void locWifi_EventCb
(
   void                           *p_UserData,
   pdsm_pd_event_type             q_PdEvent,
   const pdsm_pd_info_s_type      *pz_PdInfo
)
{
   loc_cmd_nty_client_s_type       *pz_WifiClntData = NULL;

   LOC_MSG_MED("locWifi_EventCb entered: psdm client id=%d, q_PdEvent=%d",
         (int) pz_PdInfo->client_id,
         (int) q_PdEvent, 0);

   // WPS needed event
   if (q_PdEvent & PDSM_PD_EVENT_WPS_NEEDED)
   {
      // Send an IPC to LOC_MW task and then call the callback in LOC QMI SHIM 
      // instead of directly calling the callback in LOC QMI SHIM.
      LOC_MSG_MED ("locWifi_EventCb forwards PDSM_PD_EVENT_WPS_NEEDED request, type=%d",
                  (int) pz_PdInfo->pd_info.qwip_data.request_type, 0, 0);
      pz_WifiClntData = (loc_cmd_nty_client_s_type *)loc_calloc(sizeof(loc_cmd_nty_client_s_type));

      if(NULL != pz_WifiClntData)
      {
         pz_WifiClntData->event_type = LOC_EVENT_WPS_NEEDED_REQUEST;
         pz_WifiClntData->event_data.z_pdQwip = pz_PdInfo->pd_info.qwip_data;

         if(FALSE == loc_middleware_queue_ipc(LOC_HANDLE_REGISTERED_CLIENTS, 
                                                LOC_CMD_TYPE_NOTIFY_CLIENT, 
                                                (void*)pz_WifiClntData))
         {
            LOC_MSG_ERROR("locWifi_EventCb: loc_middleware_queue_ipc failed ",
                           0, 0, 0);
            loc_free(pz_WifiClntData);
            pz_WifiClntData = NULL;
         }
      }// end of if(NULL != pz_WifiClntData) 
      else
      {
         LOC_MSG_ERROR("locWifi_EventCb failed at loc_calloc ",
                        0, 0, 0);
      }
   }// end of if (q_PdEvent & PDSM_PD_EVENT_WPS_NEEDED)
}

/*---------------------------------------------------------------------------
@brief
  Function to process QMI_LOC WIFI related requests

@param[in] 
  l_ClientHandle      Loc API client handle
  q_QmiLocMsgId       QMI_LOC Msg Id
  p_QmiLocMsgData     QMI_LOC Msg Data
  q_QmiLocMsgLen      QMI_LOC Msg Length

@retval   TRUE/FALSE    If process is successful or failed
---------------------------------------------------------------------------*/
boolean locWifi_ProcessQmiRequest
(
   const loc_client_handle_type l_ClientHandle,
   const uint32                 q_QmiLocMsgId,
   const void*                  p_QmiLocMsgData,
   const uint32                 q_QmiLocMsgLen
)
{
   locWifi_ModuleDataType* pz_LocWifiData = &(loc_middleware_data.z_locWifi_Data);
   uint32                  q_RetVal      = LOC_API_UNSUPPORTED;

   LOC_MSG_MED("locWifi_ProcessQmiRequest: ClientHandle=%d, QmiLocMsgId=0x00%X, QmiLocMsgLen=%d", l_ClientHandle, q_QmiLocMsgId, q_QmiLocMsgLen);

   /* Function arguments sanity check */
   if ((NULL == p_QmiLocMsgData) || (LOC_CLIENT_HANDLE_INVALID == l_ClientHandle))
   {
      LOC_MSG_ERROR("locWifi_ProcessQmiRequest: NULL pointer", 0,0,0);
      return FALSE;
   }

   LOC_MW_ENTER_CRIT_SECTION(&loc_middleware_data.wifi_crit_sec);

   pz_LocWifiData->l_ClientHandle      = l_ClientHandle;
   pz_LocWifiData->q_QmiLocMsgId       = q_QmiLocMsgId;

   LOC_MW_LEAVE_CRIT_SECTION(&loc_middleware_data.wifi_crit_sec);

   switch (q_QmiLocMsgId)
   {
      case QMI_LOC_INJECT_WIFI_POSITION_REQ_V02:
      {
         q_RetVal = locWifi_InjectWifiPos(l_ClientHandle, (qmiLocInjectWifiPositionReqMsgT_v02 *)p_QmiLocMsgData);
         break;
      }

      case QMI_LOC_NOTIFY_WIFI_STATUS_REQ_V02: 
      {
         q_RetVal = locWifi_NotifyWifiStatus(l_ClientHandle, (qmiLocNotifyWifiStatusReqMsgT_v02 *)p_QmiLocMsgData);
         break;
      }

      case QMI_LOC_NOTIFY_WIFI_ATTACHMENT_STATUS_REQ_V02:
      {
         q_RetVal = locWifi_NotifyWifiAttachStatus(l_ClientHandle, (qmiLocNotifyWifiAttachmentStatusReqMsgT_v02 *)p_QmiLocMsgData);
         break;
      }

      case QMI_LOC_NOTIFY_WIFI_ENABLED_STATUS_REQ_V02:
      {
         q_RetVal = locWifi_NotifyWifiEnabledStatus(l_ClientHandle, (qmiLocNotifyWifiEnabledStatusReqMsgT_v02 *)p_QmiLocMsgData);
         break;
      }

      default:
         q_RetVal = LOC_API_UNSUPPORTED;
         break;
   }

   if( LOC_API_SUCCESS == q_RetVal)
   {
      return TRUE;
   }
   else
   {
      LOC_MSG_ERROR("locWifi_ProcessQmiRequest: QMI_LOC 0x00%x returns error=%d", q_QmiLocMsgId, q_RetVal, 0);
      return FALSE;
   }
}

/*---------------------------------------------------------------------------
@brief
  Function to send IPC to LocMW task for servicing Indication to previous
  QMI_LOC request

@param[in] l_ClientHandle : Loc API client handle
@param[in] q_CmdType      : QMI_LOC Msg Id
@param[in] p_CmdInfo      : QMI_LOC Msg Information Response
@param[in] q_ParamStatus  : Parameter Status from Loc API

@retval    TRUE    if IPC posted to LocMW successfully
@retval    FALSE   if IPC posted to LocMW unsuccessfully
---------------------------------------------------------------------------*/
static boolean locWifi_SendQmiIndicationProxy
(
   const loc_client_handle_type l_ClientHandle,
   const uint32                 q_CmdType,
   const uint32                 q_CmdInfo,
   const uint32                 q_ParamStatus
)
{
   locWifi_IndMsgType z_LocIndMsg = {0};

   LOC_MSG_MED("locWifi_SendQmiIndicationProxy: ClientHandle=%d, QMI MsgID=0x00%x, ParamStatus=%d",
               l_ClientHandle, q_CmdType, q_ParamStatus);

   z_LocIndMsg.l_ClientHandle = l_ClientHandle;
   z_LocIndMsg.q_CmdType      = q_CmdType;
   z_LocIndMsg.q_CmdInfo      = q_CmdInfo;
   z_LocIndMsg.q_ParamStatus  = q_ParamStatus;

   /* Generate IPC message to LocMW */
   if (!locMW_SendIpcMsg(LM_MIDDLEWARE_MSG_ID_QMI_LOC_WIFI_IND, sizeof(z_LocIndMsg), (void*)&z_LocIndMsg))
   {
      LOC_MSG_ERROR("locWifi_SendQmiIndicationProxy: Could not send IPC to LocMW", 0, 0, 0);
      return FALSE;
   }

   return TRUE;
}

/*---------------------------------------------------------------------------
@brief
  Function to send QMI_LOC indication to client for previous QMI_LOC request
  related to WIFI module

@param[in] p_IpcMsg : IPC Message containing the QMI_LOC indication data

@retval    TRUE    if IPC posted to LocMW successfully
@retval    FALSE   if IPC posted to LocMW unsuccessfully
---------------------------------------------------------------------------*/
boolean locWifi_SendQmiIndication
(
   const os_IpcMsgType* const p_IpcMsg
)
{
   qmiLocStatusEnumT_v02 e_IndStatus;

   if (NULL == p_IpcMsg)
   {
      LOC_MSG_ERROR("locWifi_SendQmiIndication: Invalid IPC message", 0, 0, 0);
      return FALSE;
   }
   else
   {
      locWifi_IndMsgType z_LocIndMsg = *((locWifi_IndMsgType*)p_IpcMsg->p_Data);

      LOC_MSG_HIGH("locWifi_SendQmiIndication: IPC received size=%d, QMI MsgID=0x00%x",
                  p_IpcMsg->q_Size, z_LocIndMsg.q_CmdType,0);

      /* Translate Indication Status from LocMW to QMI_LOC type */
      switch(z_LocIndMsg.q_ParamStatus)
      {
         case LOC_API_SUCCESS:
            e_IndStatus = eQMI_LOC_SUCCESS_V02;
            break;

         case LOC_API_INVALID_PARAMETER:
            e_IndStatus = eQMI_LOC_INVALID_PARAMETER_V02;
            break;

         default:
            e_IndStatus = eQMI_LOC_GENERAL_FAILURE_V02;
            break;
      }

      return locQmi_ProcessInd(z_LocIndMsg.l_ClientHandle,
                              z_LocIndMsg.q_CmdType,
                              &z_LocIndMsg.q_CmdInfo, e_IndStatus);
   }
}
