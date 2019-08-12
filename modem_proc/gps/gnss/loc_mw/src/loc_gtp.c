/******************************************************************************
  @file: loc_gtp.c
  @brief:   This module handles gpsone GTP related features .
  

  DESCRIPTION
   Qualcomm Location API GTP Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2014 QUALCOMM Atheros, Inc.
  All Rights Reserved.
  QCA Proprietary and Confidential. 
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_gtp.c#1 $
$DateTime: 2016/12/13 07:59:45 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
08/28/14   ah       Intial version
======================================================================*/

#include "loc_api_2.h"
#include "loc_api_internal.h"
#include "tle_api.h"
#include "loc_utils.h"
#include "loc_qmi_shim.h"


/*===========================================================================
FUNCTION loc_gtp_ClientDownloadedDataIndProxy

DESCRIPTION
  This function Sends gtp ClientDownloadedData indication 
 
PARAMETERS 
 
  client_handle - LocAPI client handle  
  (input)

  transaction_id  - Same transaction id passed in earlier by the request
  (input)
 
  p_ind  - The QMI indication
  (output)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
 
SIDE EFFECTS

===========================================================================*/
uint32 loc_gtp_ClientDownloadedDataIndProxy(const int32 client_handle, 
                                            const uint32 transaction_id, 
                                            const qmiLocInjectGtpClientDownloadedDataIndMsgT_v02 * const p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_GTP_WWAN_CLIENT_DLD_DATA_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[LOC] loc_gtp_clientDownloadedData_ind_proxy: "
                  "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/*===========================================================================
FUNCTION loc_gtp_InjectGtpClientDownloadedDataReq

DESCRIPTION
  This function handles QMI command to inject GTP client downloaded data 
 
PARAMETERS 
 
  cpz_Client - Points to LocAPI client 
  (input)

  cpz_Req  - Points to QMI request
  (input)
 
  pz_Resp  - Points to QMI response
  (output)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
int32 loc_gtp_InjectGtpClientDownloadedDataReq(const loc_client_info_s_type *const cpz_Client,
                                               const qmiLocInjectGtpClientDownloadedDataReqMsgT_v02 *const cpz_Req,
                                               qmiLocGenRespMsgT_v02 *const pz_Resp)
{
  int32 result = LOC_API_GENERAL_FAILURE;
  tle_GtpClientDownloadedReqType z_ClientDownloadedData;

  if ((NULL != cpz_Client) && (NULL != cpz_Req) && (NULL != pz_Resp))
  {
    memscpy(&z_ClientDownloadedData.z_ClienDownloadedReq, 
            sizeof(z_ClientDownloadedData.z_ClienDownloadedReq),
            cpz_Req, sizeof(*cpz_Req));

    z_ClientDownloadedData.l_Client_handle = cpz_Client->client_handle;
    z_ClientDownloadedData.q_TransactionId = loc_qmiloc_get_transaction_id();

            
    LOC_MSG_HIGH("loc_gtp_InjectGtpClientDownloadedDataReq. client:%d, data_len:%lu",
                 cpz_Client->client_handle, cpz_Req->ClientDownloadedData_len, 0);

    memset(pz_Resp, 0, sizeof(*pz_Resp));
    #ifdef FEATURE_CGPS_XTRA_T
    if ((cpz_Req->ClientDownloadedData_len < QMI_LOC_MAX_GTP_WWAN_CLIENT_DOWNLOADED_DATA_LEN_V02) && 
        tle_InjectGtpWwanClientData(&z_ClientDownloadedData))
    {
      pz_Resp->resp.error = QMI_ERR_NONE_V01;
      pz_Resp->resp.result = QMI_RESULT_SUCCESS_V01;
    }
    else
    #endif /* FEATURE_CGPS_XTRA_T */
    {
      pz_Resp->resp.error = QMI_ERR_GENERAL_V01;
      pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
    }

    // the result of this function is success as long as pResp is filled
    result = LOC_API_SUCCESS;
  }
  else
  {
    // this null pointer reference shall never happen
    // the error code indicates pResp is not filled
    result = LOC_API_GENERAL_FAILURE;
  }
  return result;
}


/*===========================================================================
FUNCTION loc_gtp_WwanClientDloadedDataIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocInjectGtpClientDownloadedDataIndMsgT_v02 
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gtp_WwanClientDloadedDataIndHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_HIGH("[LOC] loc_gtp_WwanClientDloadedDataIndHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocInjectGtpClientDownloadedDataIndMsgT_v02 z_Ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_INJECT_GTP_CLIENT_DOWNLOADED_DATA_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_HIGH("[LOC] loc_gtp_WwanClientDloadedData_ind_handler. client: %d, tx id: %u, status: %d",
                   l_ClientHandle, q_TransactionId, (int32) z_Ind.status);

      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(l_ClientHandle, &z_IndInfo))
      {
        // done
        q_ErrorCode = 0;
      }
      else
      {
        // failed
        q_ErrorCode = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      q_ErrorCode = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    q_ErrorCode = 4;
  }

  if (0 != q_ErrorCode)
  {
    LOC_MSG_ERROR("[LOC] loc_gtp_WwanClientDloadedData_ind_handler: error code: %u", q_ErrorCode, 0, 0);
  }
}

