/******************************************************************************
  @file: loc_wle.c
  @brief:   This module handles gpsone WLE related features .
  

  DESCRIPTION
   Qualcomm Location API WLE Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2014 QUALCOMM Atheros, Inc.
  All Rights Reserved.
  QCA Proprietary and Confidential. 
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_wle.c#1 $
$DateTime: 2016/12/13 07:59:45 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
02/24/15   dc       Intial version
======================================================================*/

#include "loc_api_2.h"
#include "loc_api_internal.h"
#include "wle_api.h"
#include "loc_utils.h"
#include "loc_qmi_shim.h"

/*===========================================================================
FUNCTION loc_wle_InjectApCacheDataReq

DESCRIPTION
  This function handles QMI command to inject WLE ApCache data 
 
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
int32 loc_wle_InjectApCacheDataReq(const loc_client_info_s_type *const cpz_Client,
                                               const qmiLocInjectApCacheDataReqMsgT_v02 *const cpz_Req,
                                               qmiLocGenRespMsgT_v02 *const pz_Resp)
{
  int32 result = LOC_API_GENERAL_FAILURE;
  wle_InjectApCacheDataReqType z_ApCacheData;

  if ((NULL != cpz_Client) && (NULL != cpz_Req) && (NULL != pz_Resp))
  {
    memscpy(&z_ApCacheData.z_ApCacheReq, 
            sizeof(z_ApCacheData.z_ApCacheReq),
            cpz_Req, sizeof(*cpz_Req));

    z_ApCacheData.l_Client_handle = cpz_Client->client_handle;
    z_ApCacheData.q_TransactionId = loc_qmiloc_get_transaction_id();

            
    LOC_MSG_HIGH("loc_wle_InjectApCacheDataReq. client:%d, data_len:%lu",
                 cpz_Client->client_handle, cpz_Req->apCacheData_len, 0);

    memset(pz_Resp, 0, sizeof(*pz_Resp));
    #ifdef FEATURE_CGPS_XTRA_T
    if ((cpz_Req->apCacheData_len < QMI_LOC_MAX_GTP_WWAN_CLIENT_DOWNLOADED_DATA_LEN_V02) && 
        wle_InjectApCacheData(&z_ApCacheData))
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
FUNCTION loc_wle_InjectNoApCacheDataReq

DESCRIPTION
  This function handles QMI command to inject WLE NoApCache data 
 
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
int32 loc_wle_InjectNoApCacheDataReq(const loc_client_info_s_type *const cpz_Client,
                                               const qmiLocInjectApDoNotCacheDataReqMsgT_v02 *const cpz_Req,
                                               qmiLocGenRespMsgT_v02 *const pz_Resp)
{
  int32 result = LOC_API_GENERAL_FAILURE;
  wle_InjectNoApCacheDataReqType z_NoApCacheData;

  if ((NULL != cpz_Client) && (NULL != cpz_Req) && (NULL != pz_Resp))
  {
    memscpy(&z_NoApCacheData.z_NoApCacheReq, 
            sizeof(z_NoApCacheData.z_NoApCacheReq),
            cpz_Req, sizeof(*cpz_Req));

    z_NoApCacheData.l_Client_handle = cpz_Client->client_handle;
    z_NoApCacheData.q_TransactionId = loc_qmiloc_get_transaction_id();

            
    LOC_MSG_HIGH("loc_wle_InjectNoApCacheDataReq. client:%d, data_len:%lu",
                 cpz_Client->client_handle, cpz_Req->apDoNotCacheData_len, 0);

    memset(pz_Resp, 0, sizeof(*pz_Resp));
    #ifdef FEATURE_CGPS_XTRA_T
    if ((cpz_Req->apDoNotCacheData_len < QMI_LOC_MAX_GTP_WWAN_CLIENT_DOWNLOADED_DATA_LEN_V02) && 
        wle_InjectNoApCacheData(&z_NoApCacheData))
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
FUNCTION loc_wle_ApCacheIndProxy

DESCRIPTION
  This function Sends wle ApCache indication 
 
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
uint32 loc_wle_ApCacheIndProxy(const int32 client_handle, 
                                            const uint32 transaction_id, 
                                            const qmiLocInjectApCacheDataIndMsgT_v02 * const p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_WLE_APCACHE_REQ_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[LOC] loc_wle_ApCache_ind_proxy: "
                  "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/*===========================================================================
FUNCTION loc_wle_ApCacheIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocInjectApCacheDataIndMsgT_v02 
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/

void loc_wle_ApCacheIndHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_HIGH("[LOC] loc_wle_ApCacheIndHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocInjectApCacheDataIndMsgT_v02 z_Ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_INJECT_APCACHE_DATA_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_HIGH("[LOC] loc_wle_ApCache_ind_handler. client: %d, tx id: %u, status: %d",
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
    LOC_MSG_ERROR("[LOC] loc_wle_ApCache_ind_handler: error code: %u", q_ErrorCode, 0, 0);
  }
}

/*===========================================================================
FUNCTION loc_wle_NoApCacheIndProxy

DESCRIPTION
  This function Sends wle NoApCache indication 
 
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
uint32 loc_wle_NoApCacheIndProxy(const int32 client_handle, 
                                            const uint32 transaction_id, 
                                            const qmiLocInjectApDoNotCacheDataIndMsgT_v02 * const p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_WLE_NOAPCACHE_REQ_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[LOC] loc_wle_NoApCache_ind_proxy: "
                  "error code: %u", error_code, 0, 0);
  }
  return error_code;
}


/*===========================================================================
FUNCTION loc_wle_NoApCacheIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocInjectNoApCacheDataIndMsgT_v02 
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/

void loc_wle_NoApCacheIndHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_HIGH("[LOC] loc_wle_NoApCacheIndHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocInjectApDoNotCacheDataIndMsgT_v02 z_Ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_INJECT_APDONOTCACHE_DATA_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_HIGH("[LOC] loc_wle_NoApCache_ind_handler. client: %d, tx id: %u, status: %d",
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
    LOC_MSG_ERROR("[LOC] loc_wle_ApCache_ind_handler: error code: %u", q_ErrorCode, 0, 0);
  }
}

