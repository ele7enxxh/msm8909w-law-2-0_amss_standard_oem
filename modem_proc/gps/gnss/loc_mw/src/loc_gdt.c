/******************************************************************************
  @file: loc_gdt.c
  @brief:   This module handles gpsone GDT library
  

  DESCRIPTION
   Qualcomm Location API GDT Module

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
  Copyright (c) 2014 QUALCOMM Atheros, Inc.
  All Rights Reserved.
  QCA Proprietary and Confidential. 
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_gdt.c#1 $
$DateTime: 2016/12/13 07:59:45 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
09/15/14   ah       Intial version
======================================================================*/

#include "loc_api_2.h"
#include "loc_api_internal.h"
#include "loc_utils.h"
#include "loc_qmi_shim.h"
#include "gnss_gdt.h"

/*===========================================================================
FUNCTION loc_gdt_SendBeginIndProxy

DESCRIPTION
  This function sends a GDT Begin indication to AP. In other words, MP is
  requesting to start a session by sending this indication.
 
PARAMETERS 
 
  client_handle - LocAPI client handle  
  (input)

  transaction_id  - Transaction id 
  (input)
 
  p_ind  - Points to the QMI indication
  (output)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
 
SIDE EFFECTS

===========================================================================*/
static uint32 loc_gdt_SendBeginIndProxy(const int32 client_handle, 
                                        const uint32 transaction_id, 
                                        const qmiLocEventGdtUploadBeginStatusReqIndMsgT_v02 * const p_ind)
{
  uint32 error_code = 1;
  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_GDT_BEGIN_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_gdt_SendBeginIndProxy: error code:%lu", 
                    error_code, 0, 0);
    }
  }
  return error_code; 
}

/*===========================================================================
FUNCTION loc_gdt_SendEndIndProxy

DESCRIPTION
  This function sends a GDT end indication to AP. In other words, MP is
  requesting to end a session by sending this indication.
 
PARAMETERS 
 
  client_handle - LocAPI client handle  
  (input)

  transaction_id  - Transaction id 
  (input)
 
  p_ind  - Points to the QMI indication
  (output)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
 
SIDE EFFECTS

===========================================================================*/
static uint32 loc_gdt_SendEndIndProxy(const int32 client_handle, 
                                      const uint32 transaction_id, 
                                      const qmiLocEventGdtUploadEndReqIndMsgT_v02 * const p_ind)
{
  uint32 error_code = 1;
  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_GDT_END_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_gdt_SendEndIndProxy: error code:%lu", 
                    error_code, 0, 0);
    }
  }
  return error_code; 
}

/*===========================================================================
FUNCTION loc_gdt_BeginReqIndProxy

DESCRIPTION
  This function sends gdt begin request status indication.
  MP has recievd a request from AP and now this function sends the corresponding
  indication back to AP.
 
PARAMETERS 
 
  client_handle - LocAPI client handle
  (input)

  transaction_id - Same transaction id passed in earlier by the request
  (input)

  p_ind   - Points to the The QMI indication
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
SIDE EFFECTS

===========================================================================*/
uint32 loc_gdt_BeginReqIndProxy
(
  const int32 client_handle, 
  const uint32 transaction_id, 
  const qmiLocGdtUploadBeginStatusIndMsgT_v02 * const p_ind
)
{
  uint32 error_code = 1;
  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_GDT_BEGIN_REQ_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_gdt_BeginReqIndProxy: "
                    "error code: %u", error_code, 0, 0);
    }
  }
  return error_code;
}

/*===========================================================================
FUNCTION loc_gdt_EndIndProxy

DESCRIPTION
  This function sends gdt end request status indication.
  MP has recievd a request from AP and now this function sends the corresponding
  indication back to AP.
 
PARAMETERS 
 
  client_handle - LocAPI client handle
  (input)

  transaction_id - Same transaction id passed in earlier by the request
  (input)

  p_ind   - Points to the The QMI indication
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if message is sent successfully.
SIDE EFFECTS

===========================================================================*/
uint32 loc_gdt_EndReqIndProxy
(
  const int32 client_handle, 
  const uint32 transaction_id, 
  const qmiLocGdtUploadEndIndMsgT_v02 * const p_ind
)
{
  uint32 error_code = 1;

  if (NULL != p_ind)
  {
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_GDT_END_REQ_IND)

    if (0 != error_code)
    {
      LOC_MSG_ERROR("[LOC] loc_gdt_EndReqIndProxy: , error code: %lu",
                    error_code, 0, 0);
    }
  }
  return error_code;
}

/*===========================================================================
FUNCTION loc_gdt_UploadBeginStatusReq

DESCRIPTION
  This function handles QMI command to process gdt upload begin status request.
  MP has sent a session begin to AP, and now the response from AP is recieved
  through this request
 
PARAMETERS 
 
  cpz_Client - Points to the LocAPI client
  (input)

  cpz_Req   - Points to the QMI Request message received from AP
  (input)

  pz_Resp   - Points to the generated response
  (output)
 
DEPENDENCIES

RETURN VALUE 
  LOC_API_SUCCESS:          If the command has been handled successfully
  LOC_API_GENERAL_FAILURE : Otherwise
SIDE EFFECTS

===========================================================================*/
int32 loc_gdt_UploadBeginStatusReq(const loc_client_info_s_type * cpz_Client,
                                   const qmiLocGdtUploadBeginStatusReqMsgT_v02 *const cpz_Req,
                                   qmiLocGenRespMsgT_v02 *const pz_Resp)
{
  int32 result = LOC_API_GENERAL_FAILURE;
  e_GDTReturnCodesType e_GdtResult;

  if ((NULL != cpz_Client) && (NULL != cpz_Req) && (NULL != pz_Resp))
  {
    gdt_StartResponseType z_StartResponse;
    gdt_StartArgumentType z_Response;

    memset(&z_StartResponse, 0x00, sizeof(z_StartResponse));
    z_Response.pz_StartResponse = &z_StartResponse;

    LOC_MSG_HIGH("loc_gdt_UploadBeginStatusReq•. client:%d, serv ID:%lu, status:%d",
                 cpz_Client->client_handle, cpz_Req->serviceId, (int) cpz_Req->gdtAccessStatus);

    memset(pz_Resp, 0, sizeof(*pz_Resp));
    switch (cpz_Req->gdtAccessStatus)
    {
      case eQMI_LOC_GDT_ACCESS_ALLOWED_V02:
        z_StartResponse.e_StartStatus = e_GDT_ACCESS_ALLOWED;
        break;

       case eQMI_LOC_GDT_ACCESS_NOT_ALLOWED_V02:
         z_StartResponse.e_StartStatus = e_GDT_ACCESS_NOT_ALLOWED;
         break;

       case eQMI_LOC_GDT_ACCESS_FAILED_V02:
       default:
        z_StartResponse.e_StartStatus = e_GDT_ACCESS_FAILED;
    }
    
    switch (cpz_Req->serviceId)
    {
      case eQMI_LOC_GDT_SERVICE_WWAN_V02:
        z_StartResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_GTP_WWAN;
        break;

      default:
        z_StartResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_INVALID; 
    }
     
    z_StartResponse.z_SessionInfo.q_SessId = cpz_Req->sessionId;
    z_StartResponse.l_Client_handle = cpz_Client->client_handle;
    z_StartResponse.q_TransactionId = loc_qmiloc_get_transaction_id();

    e_GdtResult = gdt_RcvdSendBeginResponse(z_Response);
    if (e_GDT_SUCCESS == e_GdtResult) 
    {
      LOC_MSG_MED("Begin response send to GDT client",
                  0, 0, 0);
      pz_Resp->resp.error = QMI_ERR_NONE_V01;
      pz_Resp->resp.result = QMI_RESULT_SUCCESS_V01;
    }
    else
    {
      LOC_MSG_HIGH("Failed to send begin response to GDT client. err:%d",
                   (int32) e_GdtResult, 0, 0);
      pz_Resp->resp.error = QMI_ERR_GENERAL_V01;
      pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
    }

    result = LOC_API_SUCCESS;
  }
  else
  {
    // this null pointer reference shall never happen
    // the error code indicates pResp is not filled
    result = LOC_API_GENERAL_FAILURE;
  }


  LOC_MSG_MED("GDT Begin response result:%d", result, 0, 0);
  return result;
}

/*===========================================================================
FUNCTION loc_gdt_UploadEndStatusReq

DESCRIPTION
  This function handles QMI command to process gdt upload end status request.
  MP has sent a session end to AP, and now the response from AP is recieved
  through this request
 
PARAMETERS 
 
  cpz_Client - Points to the LocAPI client
  (input)

  cpz_Req   - Points to the QMI Request message received from AP
  (input)

  pz_Resp   - Points to the generated response
  (output)
 
DEPENDENCIES

RETURN VALUE 
  LOC_API_SUCCESS:          If the command has been handled successfully
  LOC_API_GENERAL_FAILURE : Otherwise
SIDE EFFECTS

===========================================================================*/
int32 loc_gdt_UploadEndStatusReq(const loc_client_info_s_type *const cpz_Client,
                                 const qmiLocGdtUploadEndReqMsgT_v02 *const cpz_Req,
                                 qmiLocGenRespMsgT_v02 *const pz_Resp)
{
  int32 result = LOC_API_GENERAL_FAILURE;
  e_GDTReturnCodesType e_GdtResult;

  if ((NULL != cpz_Client) && (NULL != cpz_Req) && (NULL != pz_Resp))
  {
    gdt_EndResponseType z_EndResponse;
    gdt_EndArgumentType z_Response;

    memset(&z_EndResponse, 0x00, sizeof(z_EndResponse));
    z_Response.pz_EndResponse = &z_EndResponse;
    
    LOC_MSG_HIGH("loc_qmiloc_gdt_upload_end_status_req. client:%d, serv ID:%lu, status:%d",
                 cpz_Client->client_handle, cpz_Req->serviceId, (int) cpz_Req->gdtEndStatus);

    memset(pz_Resp, 0, sizeof(*pz_Resp));
    switch (cpz_Req->gdtEndStatus)
    {
      case eQMI_LOC_GDT_ACK_SUCCESS_V02:
        z_EndResponse.e_EndStatus = e_GDT_ACK_SUCCESS;
        break;

      case eQMI_LOC_GDT_ACK_FAILED_V02:
        z_EndResponse.e_EndStatus = e_GDT_ACK_FAILED;
        break;

      case eQMI_LOC_GDT_ACK_INVALID_V02:
      default:
        z_EndResponse.e_EndStatus = e_GDT_ACK_FAILED;
    }
    
    switch (cpz_Req->serviceId)
    {
      case eQMI_LOC_GDT_SERVICE_WWAN_V02:
        z_EndResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_GTP_WWAN;
        break;

      default:
        z_EndResponse.z_SessionInfo.e_ServiceId = e_GDT_SERVICE_INVALID;
    }
     
    z_EndResponse.z_SessionInfo.q_SessId = cpz_Req->sessionId;
    z_EndResponse.l_Client_handle = cpz_Client->client_handle;
    z_EndResponse.q_TransactionId = loc_qmiloc_get_transaction_id();

    e_GdtResult = gdt_RcvdSendEndResponse(z_Response);
    if ( e_GDT_SUCCESS == e_GdtResult)
    {
      LOC_MSG_MED("End response send to GDT client",
                  0, 0, 0);
      pz_Resp->resp.error = QMI_ERR_NONE_V01;
      pz_Resp->resp.result = QMI_RESULT_SUCCESS_V01;
    }
    else
    {
      LOC_MSG_HIGH("Failed to send end response to GDT client. err:%d",
                   (int32) e_GdtResult, 0, 0);

      pz_Resp->resp.error = QMI_ERR_GENERAL_V01;
      pz_Resp->resp.result = QMI_RESULT_FAILURE_V01;
    }

    result = LOC_API_SUCCESS;
  }
  else
  {
    // this null pointer reference shall never happen
    // the error code indicates pResp is not filled
    result = LOC_API_GENERAL_FAILURE;
  }

  LOC_MSG_MED("GDT End response result:%d", result, 0, 0);
  return result;
}

/*===========================================================================
FUNCTION loc_gdt_BeginIndicationHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocEventGdtUploadBeginStatusReqIndMsgT_v02.
  MP is sending a GDT begin message as an indication to AP.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
  0 if ipc has been processed successfully  
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_BeginIndicationHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_HIGH("[LOC] loc_gdt_BeginIndicationHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocEventGdtUploadBeginStatusReqIndMsgT_v02 z_Ind;

    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_EVENT_GDT_UPLOAD_BEGIN_STATUS_REQ_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_HIGH("[LOC] loc_gdt_BeginIndicationHandler, client: %d, tx id: %lu",
                   l_ClientHandle, q_TransactionId, 0);

      LOC_MSG_HIGH("[LOC] loc_gdt_BeginIndicationHandler, serviceId:%d, sessId:%lu, filePathLen:%lu",
                   (int32) z_Ind.serviceId, z_Ind.sessionId, z_Ind.filePath_len);

      
      // send indication through QCSI
      if (locQmiShimBroadcastInd(QMI_LOC_EVENT_MASK_GDT_UPLOAD_BEGIN_REQ_V02, 
                                 &z_IndInfo))
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
    LOC_MSG_ERROR("[LOC] loc_gdt_BeginIndicationHandler, error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
}

/*===========================================================================
FUNCTION loc_gdt_EndIndicationHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtUploadEndIndMsgT_v02
  MP is sending a GDT end message as an indication to AP.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_EndIndicationHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 l_ErrorCode = 1;

  LOC_MSG_HIGH("[LOC] loc_gdt_EndIndicationHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocEventGdtUploadEndReqIndMsgT_v02 z_Ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_EVENT_GDT_UPLOAD_END_REQ_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_HIGH("[LOC] loc_gdt_EndIndicationHandler, client: %d, tx id: %u",
                   l_ClientHandle, q_TransactionId, 0);

      LOC_MSG_HIGH("[LOC] loc_gdt_EndIndicationHandler, serviceId: %d, sessId: %lu, status: %d",
                   (int32) z_Ind.serviceId, z_Ind.sessionId, (int32) z_Ind.endStatus);

      // send indication through QCSI
      if (locQmiShimBroadcastInd(QMI_LOC_EVENT_MASK_GDT_UPLOAD_END_REQ_V02, &z_IndInfo))
      {
        // done
        l_ErrorCode = 0;
      }
      else
      {
        // failed
        l_ErrorCode = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      l_ErrorCode = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    l_ErrorCode = 4;
  }

  if (0 != l_ErrorCode)
  {
    LOC_MSG_ERROR("[LOC] loc_gdt_EndIndicationHandler, error code: %u", l_ErrorCode, 0, 0);
  }
}

/*===========================================================================
FUNCTION loc_gdt_SendBeginRequest

DESCRIPTION
  This function sends a GDT begin indication to AP.
 
PARAMETERS 
 
  e_ServiceId - GDT Client service ID
  (input)
 
  q_SessionId - GDt session ID
  (input)
 
  q_FilePathLen - The number valid chars in file path
  (input)
 
  c_FilePath  - Points to the file path
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the request, and sent the access request
                 to AP to get the permission
 
  e_GDT_FAIL: Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType loc_gdt_SendBeginRequest(e_GDTServiceIDType e_ServiceId,
                                              uint32 q_SessionId, uint32 q_FilePathLen,
                                              const char * c_FilePath)
{
  qmiLocEventGdtUploadBeginStatusReqIndMsgT_v02 z_BeginInd;
  int32 client_handle = 0;
  uint32 transaction_id = 0;

  if ((NULL == c_FilePath) ||
      (q_FilePathLen >= QMI_LOC_MAX_GDT_PATH_LEN_V02))
  {
    return e_GDT_FAIL;
  }

  z_BeginInd.filePath_len = q_FilePathLen;

  switch (e_ServiceId)
  {
    case e_GDT_SERVICE_GTP_WWAN:
      z_BeginInd.serviceId = eQMI_LOC_GDT_SERVICE_WWAN_V02;
      break;

    default:
      return e_GDT_FAIL;
  }

  memscpy(z_BeginInd.filePath, sizeof(z_BeginInd.filePath), 
          c_FilePath, q_FilePathLen);

  z_BeginInd.sessionId = q_SessionId;

  /* call an API in loc_task */
  if( loc_gdt_SendBeginIndProxy(client_handle, transaction_id, &z_BeginInd) == 0)
  {
    return e_GDT_SUCCESS;
  }

  return e_GDT_FAIL;
}

/*===========================================================================
FUNCTION loc_gdt_SendEndRequest

DESCRIPTION
  This function sends a GDT END indication to AP.
 
PARAMETERS 
 
  e_ServiceId - GDT Client service ID
  (input)
 
  q_SessionId - GDT session ID
  (input)
 
  e_Status   - End Status (session succeeded or aborted)
  (input)
 
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the request, and sent the indication
                 to AP
 
  e_GDT_FAIL: Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType loc_gdt_SendEndRequest(e_GDTServiceIDType e_ServiceId,
                                            uint32 q_SessionId, 
                                            e_GDTResponseType e_Status)
{
  qmiLocEventGdtUploadEndReqIndMsgT_v02 z_EndInd;
  int32  l_ClientHandle = 0;
  uint32 q_Transaction_id = 0;

  z_EndInd.sessionId = q_SessionId;
  switch (e_ServiceId)
  {
    case e_GDT_SERVICE_GTP_WWAN:
      z_EndInd.serviceId = eQMI_LOC_GDT_SERVICE_WWAN_V02;
      break;

    default:
      return e_GDT_FAIL;
  }

  switch (e_Status)
  {
    case e_GDT_ACCESS_ALLOWED:
      z_EndInd.endStatus = eQMI_LOC_GDT_SUCCESS_V02;
      break;

    default:
      z_EndInd.endStatus = eQMI_LOC_GDT_FAILED_V02;
  }

  /* call an API in loc_task */
  if( loc_gdt_SendEndIndProxy(l_ClientHandle, q_Transaction_id, &z_EndInd) == 0)
  {
    return e_GDT_SUCCESS;
  }

  return e_GDT_FAIL;

}

/*===========================================================================
FUNCTION loc_gdt_BeginReqIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtUploadBeginStatusIndMsgT_v02
  MP has received the request for GDT begin response from AP, now it is
  sending the indication for the request.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_BeginReqIndHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_HIGH("[LOC] loc_gdt_BeginReqIndHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocGdtUploadBeginStatusIndMsgT_v02 z_Ind;

    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_GDT_UPLOAD_BEGIN_STATUS_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_HIGH("[LOC] loc_gdt_BeginReqIndHandler. client: %d, tx id: %lu, status: %d",
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
    LOC_MSG_ERROR("[LOC] loc_gdt_BeginReqIndHandler: error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
}

/*===========================================================================
FUNCTION loc_gdt_EndReqIndHandler

DESCRIPTION
  This function is called by LocMW task loop to handle ipc message for QMI 
  indication qmiLocGdtUploadEndIndMsgT_v02 
  MP has received the request for GDT end response from AP, now it is
  sending the indication for the request.
 
PARAMETERS 
 
  cpz_IpcMsg - Points to the recieved IPC message
  (input)
 
DEPENDENCIES

RETURN VALUE 
 
SIDE EFFECTS

===========================================================================*/
void loc_gdt_EndReqIndHandler(const os_IpcMsgType *const cpz_IpcMsg)
{
  uint32 q_ErrorCode = 1;

  LOC_MSG_HIGH("[LOC] loc_gdt_EndReqIndHandler.", 0, 0, 0);

  if (NULL != cpz_IpcMsg)
  {
    int32 l_ClientHandle = 0;
    uint32 q_TransactionId = 0;
    qmiLocGdtUploadEndIndMsgT_v02 z_Ind;
    
    locQmiShimIndInfoStructT z_IndInfo = { QMI_LOC_GDT_UPLOAD_END_IND_V02, NULL, (int32)sizeof(z_Ind) };
    z_IndInfo.p_Msg = &z_Ind;

    if (0 == LOC_UNPACK_IPC_TO_LOCMW(cpz_IpcMsg, &l_ClientHandle, &q_TransactionId, &z_Ind))
    {
      LOC_MSG_HIGH("[LOC] loc_gdt_EndReqIndHandler. client: %d, tx id: %lu, status: %d",
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
    LOC_MSG_ERROR("[LOC] loc_gdt_EndReqIndHandler: error code: %lu", 
                  q_ErrorCode, 0, 0);
  }
}




