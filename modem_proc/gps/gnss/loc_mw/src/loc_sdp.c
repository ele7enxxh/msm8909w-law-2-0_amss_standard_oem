/*============================================================================
  @file loc_sdp.c

            Handlers for ISAGNAV related commands and IPC messages

GENERAL DESCRIPTION

  This file implements the handlers for ISAGNAV related QMI commands and IPC messages.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

            Copyright (c) 2011-2013 Qualcomm Technologies Incorporated.
            All Rights Reserved
            Qualcomm Confidential and Proprietary

            Copyright (c) 2013-2014 Qualcomm Atheros, Inc.
            All Rights Reserved. 
            Qualcomm Atheros Confidential and Proprietary.
            
            Copyright (c) 2015 Qualcomm Technologies, Inc.
            Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_sdp.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/13   ssu     Fixing KW errors.
04/10/11   mac     Initial version

=============================================================================*/



#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "limits.h"
#include "msg.h"
#include "queue.h"
#include "time_svc.h"

#include "aries_os_api.h"
#include "pdapi.h"
#include "pdsm_atl.h"
#include "loc_api_2.h"

#include "loc_task.h"
#include "loc_client.h"
#include "loc_pd.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_pa.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "loc_api_internal.h"

// QCSI
#include "qmi_csi.h"
#include "loc_qmi_shim.h"

// QMI LOC headers
#include "location_service_v02.h"

//SDP headers
#include "loc_for_sdp.h"
#include "loc_sdp.h"
#include "sdp_for_loc.h"
#include "sdp_loc_eng_api.h"

//loc Eng
#include "loc_conv_locEng_qmiLoc.h"

#include "loc_utils.h"
/* ---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ---------------------------------------------------------------------------*/

/**
 * called under SDP task context to create an ipc messageand send to LocMW task.
 * save the effort to calculate size of the QMI payload
 */
#define LOC_SDP_SEND_IPC_TO_LOCMW(M,C,T,PTR) LOC_SEND_IPC_TO_LOCMW(M,C,T,PTR)
#define LOC_SDP_SEND_IPC_TO_LOCMW_EXT(M,C,T,E,PTR) loc_sdp_send_ipc_to_locmw_ext((M),(C),(T),(E),(PTR),sizeof(*(PTR)))

/**
 * called under LocMW task context to unpack an ipc messageand send to it by LOC_SDP_SEND_IPC_TO_LOCMW.
 * save the effort to calculate size of the QMI payload
 */
#define LOC_SDP_UNPACK_IPC_TO_LOCMW(M,C,T,PTR) loc_unpack_ipc_to_locmw((M),(C),(T),(PTR),sizeof(*(PTR)))
#define LOC_SDP_UNPACK_IPC_TO_LOCMW_EXT(M,C,T,E,PTR) loc_sdp_unpack_ipc_to_locmw_ext((M),(C),(T),(E),(PTR),sizeof(*(PTR)))

static void loc_qmiloc_fill_general_response(qmiLocGenRespMsgT_v02 *const pResp, const uint32 preliminary_result_from_sdp);


typedef locUserDataStructT locSdpUserDataStructT;

typedef struct loc_sdp_user_data_ext_struct_type
{
  // loc MW client handle
  int32 clientHandle;

  // transaction id
  uint32 transactionId;

  // ext data
  uint32 extData;

  //pointer to self
  struct loc_sdp_user_data_ext_struct_type *pMe;

}locSdpUserDataExtStructT;


/**
 * handle QMI command to set SPI streaming report
 *
 * @param client_ptr LocAPI client handle
 * @param flag       enable or disable SPI streaming report
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
int32 loc_qmiloc_set_spi_streaming_report(loc_client_info_s_type *const client_ptr, const boolean flag)
{
  int32 result = LOC_API_GENERAL_FAILURE;


  if (NULL != client_ptr)
  {
    // TODO: pass indication to QCSI
    LOC_MSG_MED("loc_qmiloc_set_spi_streaming_report. client: %d, flag: %d",
                client_ptr->client_handle, (int)flag, 0);

    // everything looks good, let's return with proper response
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

/**
 * handle QMI command to set SPI status
 *
 * @param client_ptr LocAPI client handle
 * @param pReq       QMI request
 * @param pResp      QMI response
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
int32 loc_qmiloc_set_spi_status_req(loc_client_info_s_type *const client_ptr,
                                    const qmiLocSetSpiStatusReqMsgT_v02 *const pReq,
                                    qmiLocGenRespMsgT_v02 *const pResp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

  if ((NULL != client_ptr) && (NULL != pReq) && (NULL != pResp))
  {
    uint32 preliminary_result_from_sdp = 0;

    LOC_MSG_HIGH("loc_qmiloc_set_spi_status_req. client: %d, stationary: %d, confidence: %d",
                 client_ptr->client_handle, (uint32)pReq->stationary, (uint32)pReq->confidenceStationary);

    // pass information to sdp component
    preliminary_result_from_sdp = sdp_qmiloc_set_spi_status_req_proxy(client_ptr->client_handle, loc_qmiloc_get_transaction_id(), pReq);
    loc_qmiloc_fill_general_response(pResp, preliminary_result_from_sdp);

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


/**
 * handle QMI command to get cradle mount status
 * request is empty thus not generated by IDL parser
 *
 * @param client_ptr LocAPI client handle
 * @param pResp      QMI response
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
int32 loc_qmiloc_get_cradle_mount_config_req(loc_client_info_s_type *const client_ptr,
                                             qmiLocGenRespMsgT_v02 *const pResp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

  if ((NULL != client_ptr) && (NULL != pResp))
  {
    int32 preliminary_result_from_sdp = 0;

    LOC_MSG_HIGH("loc_qmiloc_get_cradle_mount_config_req. client: %d",
                 client_ptr->client_handle, 0, 0);

    // pass information to sdp component
    preliminary_result_from_sdp = sdp_qmiloc_get_cradle_mount_config_req_proxy(
      client_ptr->client_handle,
      loc_qmiloc_get_transaction_id());
    loc_qmiloc_fill_general_response(pResp, preliminary_result_from_sdp);

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

/**
 * handle QMI command to get external power status
 * request is empty thus not generated by IDL parser
 *
 * @param client_ptr LocAPI client handle
 * @param pResp      QMI response
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
int32 loc_qmiloc_get_external_power_config_req(loc_client_info_s_type *const client_ptr,
                                               qmiLocGenRespMsgT_v02 *const pResp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

  if ((NULL != client_ptr) && (NULL != pResp))
  {
    int32 preliminary_result_from_sdp = 0;

    LOC_MSG_HIGH("loc_qmiloc_get_external_power_config_req. client: %d",
                 client_ptr->client_handle, 0, 0);

    // pass information to sdp component
    preliminary_result_from_sdp = sdp_qmiloc_get_external_power_config_req_proxy(
      client_ptr->client_handle,
      loc_qmiloc_get_transaction_id());
    loc_qmiloc_fill_general_response(pResp, preliminary_result_from_sdp);

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


/**
 * handle QMI command to set cradle mount status
 *
 * @param client_ptr LocAPI client handle
 * @param pReq       QMI request
 * @param pResp      QMI response
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
int32 loc_qmiloc_set_cradle_mount_config_req(loc_client_info_s_type *const client_ptr,
                                             const qmiLocSetCradleMountConfigReqMsgT_v02 *const pReq,
                                             qmiLocGenRespMsgT_v02 *const pResp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

  if ((NULL != client_ptr) && (NULL != pReq) && (NULL != pResp))
  {
    int32 preliminary_result_from_sdp = 0;

    LOC_MSG_HIGH("loc_qmiloc_set_cradle_mount_config_req. client: %d, cradle state: %u",
                 client_ptr->client_handle, (uint32)pReq->cradleMountState, 0);

    // pass information to sdp component
    preliminary_result_from_sdp = sdp_qmiloc_set_cradle_mount_config_req_proxy(
      client_ptr->client_handle,
      loc_qmiloc_get_transaction_id(),
      pReq);
    loc_qmiloc_fill_general_response(pResp, preliminary_result_from_sdp);

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

/**
 * handle QMI command to set external power status
 *
 * @param client_ptr LocAPI client handle
 * @param pReq       QMI request
 * @param pResp      QMI response
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
int32 loc_qmiloc_set_external_power_config_req(loc_client_info_s_type *const client_ptr,
                                               const qmiLocSetExternalPowerConfigReqMsgT_v02 *const pReq,
                                               qmiLocGenRespMsgT_v02 *const pResp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

  if ((NULL != client_ptr) && (NULL != pReq) && (NULL != pResp))
  {
    int32 preliminary_result_from_sdp = 0;

    LOC_MSG_HIGH("loc_qmiloc_set_external_power_config_req. client: %d, ext power: %u",
                 client_ptr->client_handle, (uint32)pReq->externalPowerState, 0);

    // pass information to sdp component
    preliminary_result_from_sdp = sdp_qmiloc_set_external_power_config_req_proxy(
      client_ptr->client_handle,
      loc_qmiloc_get_transaction_id(),
      pReq);
    loc_qmiloc_fill_general_response(pResp, preliminary_result_from_sdp);

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

/**
 * called under SDP task context to create an ipc message and 
 * send to SDP task 
 *
 * @param msg_id    ipc message id
 * @param loc_client_handle
 *                  LocAPI client handle
 * @param transaction_id
 *                  will be duplicated into the resulting indication
 * @param ext_data
 *                  additional data passed in the IPC
 * @param p_payload QMI request, usually data structure created by the IDL compiler
 * @param payload_size
 *                  size of the QMI request
 *
 * @return 0 if ipc has been sent successfully
 */
static uint32 loc_sdp_send_ipc_to_locmw_ext(const uint32 msg_id,
                                        const int32 loc_client_handle,
                                        const uint32 transaction_id,
                                        const uint32 ext_data,
                                        const void *const p_payload,
                                        const uint32 payload_size)
{
  uint32 error_code = 1;
  const uint32 msg_size = sizeof(loc_client_handle) + sizeof(transaction_id) + sizeof(ext_data) + payload_size;

  if (NULL != p_payload)
  {
    os_IpcMsgType *const ipc_msg_ptr = os_IpcCreate(msg_size, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_LOC_MIDDLEWARE);
    if (NULL != ipc_msg_ptr)
    {
      // casting to void * first to avoid lint error of memory size
      byte *p_insertion_point = (byte *)(void *)ipc_msg_ptr->p_Data;
      int32 *p_client_handle = NULL;
      uint32 *p_transaction_id = NULL;
      uint32 *p_ext_data = NULL;
      void *p_payload_store = NULL;

      // calculate offset
      p_client_handle = (int32 *)p_insertion_point;
      p_insertion_point += sizeof(int);
      p_transaction_id = (uint32 *)p_insertion_point;
      p_insertion_point += sizeof(uint32);

      p_ext_data = (uint32 *)p_insertion_point;
      p_insertion_point += sizeof(uint32);

      p_payload_store = (void *)p_insertion_point;

      // first is client handle
      *p_client_handle = (uint32)loc_client_handle;
      // copy the transaction id
      *p_transaction_id = transaction_id;
      // copy the ext_data
      *p_ext_data = ext_data;

      // make a shallow copy of the request
      (void)memscpy(p_payload_store, payload_size, p_payload, payload_size);

      ipc_msg_ptr->q_MsgId = msg_id;
      if (TRUE == os_IpcSend(ipc_msg_ptr, THREAD_ID_LOC_MIDDLEWARE))
      {
        // ipc sent
        error_code = 0;
      }
      else
      {
        // ipc not sent
        (void)os_IpcDelete(ipc_msg_ptr);
        error_code = 2;
      }
    }
    else
    {
      // ipc memory allocation failure, ipc not sent
      error_code = 3;
    }
  }
  else
  {
    // null pointer for payload. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_send_ipc_to_locmw_ext: error code: %u", error_code, 0, 0);
  }
  return error_code;
}


/**
 * called under LocMW task context to unpack an ipc messageand send to it by SDP_QMILOC_SEND_IPC_TO_LOCMW.
 *
 * @param p_ipc_msg_in
 *                  [in] ipc message
 * @param p_loc_client_handle
 *                  [out] LocAPI client handle
 * @param p_transaction_id
 *                  [out] transaction id in the request
 * @param p_payload [out] QMI request. structure created by IDL compiler.
 *                  size must be enough to hold payload_size
 * @param payload_size
 *                  [in] size of the QMI request
 *
 * @return 0 if ipc has been sent successfully
 */
static uint32 loc_sdp_unpack_ipc_to_locmw_ext(const os_IpcMsgType *const p_ipc_msg_in,
                                          int32 *const p_loc_client_handle,
                                          uint32 *const p_transaction_id,
                                          uint32 *const p_ext_data,
                                          void *const p_payload,
                                          const uint32 payload_size)
{
  uint32 error_code = 1;

  if ((NULL != p_ipc_msg_in) && (NULL != p_loc_client_handle) && (NULL != p_payload))
  {
    const byte *p_extraction_point = (byte *)(void *)p_ipc_msg_in->p_Data;
    const int32 *p_client_handle = NULL;
    const uint32 *p_transaction_id_store = NULL;
    const uint32 *p_ext_data_store = NULL;
    const void *p_payload_store = NULL;

    // calculate offset
    p_client_handle = (const int32 *)p_extraction_point;
    p_extraction_point += sizeof(int);
    p_transaction_id_store = (const uint32 *)p_extraction_point;
    p_extraction_point += sizeof(uint32);
    p_ext_data_store = (const uint32 *)p_extraction_point;
    p_extraction_point += sizeof(uint32);

    p_payload_store = (const void *)p_extraction_point;

    // retrieve client id
    *p_loc_client_handle = *p_client_handle;
    // retrieve transaction id
    *p_transaction_id = *p_transaction_id_store;
    // retrieve ext_data
    *p_ext_data = *p_ext_data_store;
    // retrieve payload
    (void)memscpy(p_payload, payload_size, p_payload_store, payload_size);

    error_code = 0;
  }
  else
  {
    // null pointer for something. this shall never happen
    error_code = 2;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_unpack_ipc_to_locmw_ext: error code: %u", error_code, 0, 0);
  }
  return error_code;
}

#define IND_PROXY_EXT(MSG)\
{\
    if(NULL != p_ind)\
    {\
        if(0 == LOC_SDP_SEND_IPC_TO_LOCMW_EXT(MSG, client_handle, transaction_id, ext_data, p_ind))\
        {\
            error_code = 0;\
        }\
        else\
        {\
            error_code = 2;\
        }\
    }\
    else\
    {\
        error_code = 3;\
    }\
}

/**
 * called by SDP task to send QMI indication to LocMW task
 *
 * @param client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               same transaction id passed in earlier by the request
 * @param p_ind  the QMI indication
 *
 * @return
 */
uint32 loc_sdp_set_spi_status_ind_proxy(const int32 client_handle, const uint32 transaction_id, const qmiLocSetSpiStatusIndMsgT_v02 *const p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_SDP_SET_SPI_STATUS_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_set_spi_status_ind_proxy: error code: %u", error_code, 0, 0);
  }
  return error_code;
}

uint32 loc_sdp_get_cradle_mount_config_ind_proxy(const int32 client_handle, const uint32 transaction_id, const qmiLocGetCradleMountConfigIndMsgT_v02 *const p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_SDP_GET_CRADLE_MOUNT_CONFIG_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_get_cradle_mount_config_ind_proxy: error code: %u", error_code, 0, 0);
  }
  return error_code;
}

uint32 loc_sdp_get_external_power_config_ind_proxy(const int32 client_handle, const uint32 transaction_id, const qmiLocGetExternalPowerConfigIndMsgT_v02 *const p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_SDP_GET_EXTERNAL_POWER_CONFIG_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_get_external_power_config_ind_proxy: error code: %u", error_code, 0, 0);
  }
  return error_code;
}

uint32 loc_sdp_set_cradle_mount_config_ind_proxy(const int32 client_handle, const uint32 transaction_id, const qmiLocSetCradleMountConfigIndMsgT_v02 *const p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_SDP_SET_CRADLE_MOUNT_CONFIG_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_set_cradle_mount_config_ind_proxy: error code: %u", error_code, 0, 0);
  }
  return error_code;
}

uint32 loc_sdp_set_external_power_config_ind_proxy(const int32 client_handle, const uint32 transaction_id, const qmiLocSetExternalPowerConfigIndMsgT_v02 *const p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_SDP_SET_EXTERNAL_POWER_CONFIG_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_set_external_power_config_ind_proxy: error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 *
 * @param [in] p_ind  the QMI indication
 *
 * @return 0 if operation is successful
 */
uint32 loc_sdp_event_switch_spi_streaming_report_ind_proxy(const qmiLocEventSetSpiStreamingReportIndMsgT_v02 *const p_ind)
{
  uint32 error_code = 1;
  const int32 client_handle = LOC_HANDLE_REGISTERED_CLIENTS;
  const uint32 transaction_id = 0;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_SDP_EVENT_SWITCH_SPI_STREAMING_REPORT_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_event_set_spi_streaming_report_ind_proxy: error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
 * called by LocMW task loop to handle ipc message for QMI indication qmiLocSetSpiStatusIndMsgT_v02
 *
 * @param p_ipc_msg_in
 *               ipc message
 */
void loc_qmiloc_set_spi_status_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 1;

  LOC_MSG_HIGH("[SDP] loc_sdp_set_spi_status_ind_handler.", 0, 0, 0);

  if (NULL != p_ipc_msg_in)
  {
    int32 loc_client_handle = 0;
    uint32 transaction_id = 0;
    qmiLocSetSpiStatusIndMsgT_v02 ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT ind_info = { QMI_LOC_SET_SPI_STATUS_IND_V02, NULL, (int32)sizeof(ind) };
    ind_info.p_Msg = &ind;

    if (0 == LOC_SDP_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in, &loc_client_handle, &transaction_id, &ind))
    {
      LOC_MSG_HIGH("[SDP] loc_sdp_set_spi_status_ind_handler. client: %d, tx id: %u, status: %d",
                   loc_client_handle, transaction_id, ind.status);

      // TODO: verify the client ID and transaction id to be valid
      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(loc_client_handle, &ind_info))
      {
        // done
        error_code = 0;
      }
      else
      {
        // failed
        error_code = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      error_code = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_set_spi_status_ind_handler: error code: %u", error_code, 0, 0);
  }
  // no return value
  return;
}

void loc_qmiloc_get_cradle_mount_config_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 1;

  LOC_MSG_HIGH("[SDP] loc_qmiloc_get_cradle_mount_config_ind_handler.", 0, 0, 0);

  if (NULL != p_ipc_msg_in)
  {
    int32 loc_client_handle = 0;
    uint32 transaction_id = 0;
    qmiLocGetCradleMountConfigIndMsgT_v02 ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT ind_info = { QMI_LOC_GET_CRADLE_MOUNT_CONFIG_IND_V02, NULL, (int32)sizeof(ind) };
    ind_info.p_Msg = &ind;

    if (0 == LOC_SDP_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in, &loc_client_handle, &transaction_id, &ind))
    {
      LOC_MSG_HIGH("[SDP] loc_qmiloc_get_cradle_mount_config_ind_handler. client: %d, tx id: %u, status: %d",
                   loc_client_handle, transaction_id, ind.status);

      // TODO: verify the client ID and transaction id to be valid
      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(loc_client_handle, &ind_info))
      {
        // done
        error_code = 0;
      }
      else
      {
        // failed
        error_code = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      error_code = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_qmiloc_get_cradle_mount_config_ind_handler: error code: %u", error_code, 0, 0);
  }
  // no return value
  return;
}

void loc_qmiloc_get_external_power_config_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 1;

  LOC_MSG_HIGH("[SDP] loc_qmiloc_get_external_power_config_ind_handler.", 0, 0, 0);

  if (NULL != p_ipc_msg_in)
  {
    int32 loc_client_handle = 0;
    uint32 transaction_id = 0;
    qmiLocGetExternalPowerConfigIndMsgT_v02 ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT ind_info = { QMI_LOC_GET_EXTERNAL_POWER_CONFIG_IND_V02, NULL, (int32)sizeof(ind) };
    ind_info.p_Msg = &ind;

    if (0 == LOC_SDP_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in, &loc_client_handle, &transaction_id, &ind))
    {
      LOC_MSG_HIGH("[SDP] loc_qmiloc_get_external_power_config_ind_handler. client: %d, tx id: %u, status: %d",
                   loc_client_handle, transaction_id, ind.status);

      // TODO: verify the client ID and transaction id to be valid
      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(loc_client_handle, &ind_info))
      {
        // done
        error_code = 0;
      }
      else
      {
        // failed
        error_code = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      error_code = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_qmiloc_get_external_power_config_ind_handler: error code: %u", error_code, 0, 0);
  }
  // no return value
  return;
}

void loc_qmiloc_set_cradle_mount_config_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 1;

  LOC_MSG_HIGH("[SDP] loc_qmiloc_set_cradle_mount_config_ind_handler.", 0, 0, 0);

  if (NULL != p_ipc_msg_in)
  {
    int32 loc_client_handle = 0;
    uint32 transaction_id = 0;
    qmiLocSetCradleMountConfigIndMsgT_v02 ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT ind_info = { QMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND_V02, NULL, (int32)sizeof(ind) };
    ind_info.p_Msg = &ind;

    if (0 == LOC_SDP_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in, &loc_client_handle, &transaction_id, &ind))
    {
      LOC_MSG_HIGH("[SDP] loc_qmiloc_set_cradle_mount_config_ind_handler. client: %d, tx id: %u, status: %d",
                   loc_client_handle, transaction_id, ind.status);

      // TODO: verify the client ID and transaction id to be valid
      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(loc_client_handle, &ind_info))
      {
        // done
        error_code = 0;
      }
      else
      {
        // failed
        error_code = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      error_code = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_qmiloc_set_cradle_mount_config_ind_handler: error code: %u", error_code, 0, 0);
  }
  // no return value
  return;
}

void loc_qmiloc_set_external_power_config_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 1;

  LOC_MSG_HIGH("[SDP] loc_qmiloc_set_external_power_config_ind_handler.", 0, 0, 0);

  if (NULL != p_ipc_msg_in)
  {
    int32 loc_client_handle = 0;
    uint32 transaction_id = 0;
    qmiLocSetExternalPowerConfigIndMsgT_v02 ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT ind_info = { QMI_LOC_SET_EXTERNAL_POWER_CONFIG_IND_V02, NULL, (int32)sizeof(ind) };
    ind_info.p_Msg = &ind;

    if (0 == LOC_SDP_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in, &loc_client_handle, &transaction_id, &ind))
    {
      LOC_MSG_HIGH("[SDP] loc_qmiloc_set_external_power_config_ind_handler. client: %d, tx id: %u, status: %d",
                   loc_client_handle, transaction_id, ind.status);

      // TODO: verify the client ID and transaction id to be valid

      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(loc_client_handle, &ind_info))
      {
        // done
        error_code = 0;
      }
      else
      {
        // failed
        error_code = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      error_code = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_qmiloc_set_external_power_config_ind_handler: error code: %u", error_code, 0, 0);
  }
  // no return value
  return;
}

uint32 loc_sdp_send_event_switch_spi_streaming_report_ind(const qmiLocEventSetSpiStreamingReportIndMsgT_v02 *const ptr_ind)
{
  uint32 error_code = 1;
  locQmiShimIndInfoStructT ind_info = { QMI_LOC_EVENT_SET_SPI_STREAMING_REPORT_IND_V02, NULL, (int32)sizeof(*ptr_ind) };
  ind_info.p_Msg = (void *)ptr_ind;

  LOC_MSG_HIGH("[SDP] loc_sdp_send_event_switch_spi_streaming_report_ind.", 0, 0, 0);

  if (NULL != ptr_ind)
  {
    // send indication through QCSI
    if (TRUE == locQmiShimBroadcastInd(QMI_LOC_EVENT_MASK_SET_SPI_STREAMING_REPORT_V02, &ind_info))
    {
      // done
      error_code = 0;
    }
    else
    {
      // failed
      error_code = 2;
    }
  }
  else
  {
    // NULL indication, this shall never happen
    error_code = 3;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_send_event_switch_spi_streaming_report_ind: error code: %u", error_code, 0, 0);
  }
  return error_code;
}

void loc_sdp_event_switch_spi_streaming_report_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 1;

  LOC_MSG_HIGH("[SDP] loc_sdp_event_switch_spi_streaming_report_ind_handler.", 0, 0, 0);

  if (NULL != p_ipc_msg_in)
  {
    int32 loc_client_handle = 0;
    uint32 transaction_id = 0;
    qmiLocEventSetSpiStreamingReportIndMsgT_v02 ind;

    if (0 == LOC_SDP_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in, &loc_client_handle, &transaction_id, &ind))
    {
      // send indication through QCSI
      if (TRUE == loc_sdp_send_event_switch_spi_streaming_report_ind(&ind))
      {
        // done
        error_code = 0;
      }
      else
      {
        // failed
        error_code = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      error_code = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_event_switch_spi_streaming_report_ind_handler: error code: %u", error_code, 0, 0);
  }
  // no return value
  return;
}


/**
 * fill in general qmiloc response based on preliminary validation result from sdp
 *
 * @param pResp  [out] response to be filled
 * @param preliminary_result_from_sdp
 *               [in] validation result from sdp
 */
static void loc_qmiloc_fill_general_response(qmiLocGenRespMsgT_v02 *const pResp, const uint32 preliminary_result_from_sdp)
{
  if (NULL != pResp)
  {
    memset(pResp, 0, sizeof(*pResp));
    //initial value, changes to SUCCESS for success case
    // does not change for failure cases
    pResp->resp.result = QMI_RESULT_FAILURE_V01;
    switch (preliminary_result_from_sdp)
    {
      case SDP_ERROR_CODE_SUCCESS:
        pResp->resp.error = QMI_ERR_NONE_V01;
        pResp->resp.result = QMI_RESULT_SUCCESS_V01;
        break;
      case SDP_ERROR_CODE_UNSUPPORTED:
         pResp->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
         break;
      default:
         pResp->resp.error = QMI_ERR_MISSING_ARG_V01;
         break;
    }
    LOC_MSG_HIGH("loc_qmiloc_fill_general_response. validation: %u, result: %u, error: %u",
                 preliminary_result_from_sdp, (uint32)pResp->resp.result, (uint32)pResp->resp.error); 
  }
  else
  {
    // this shall never happen
    LOC_MSG_ERROR("loc_qmiloc_fill_general_response. null pointer", 0, 0, 0);
  }
}


boolean validateQmiLocGetSensorPropertiesReq(const qmiLocGetSensorPropertiesReqMsgT_v02 *pQmiLocGetSensorPropertiesReq)
{
  boolean retVal = TRUE;

  /* validate input pointers */
  if (NULL == pQmiLocGetSensorPropertiesReq)
  {
    return FALSE;
  }

  // perform preliminary parameter validation here as per requested
  if (0 == pQmiLocGetSensorPropertiesReq->getSensorPropertiesMask) /* if bitmask is 0 */
  {
    // one of them must be valid
    retVal = FALSE;
  } else
  {
    retVal = TRUE;
  }
  return retVal;
}

boolean validateQmiLocSetSensorPropertiesReq(const qmiLocSetSensorPropertiesReqMsgT_v02 *pQmiLocSetSensorPropertiesReq)
{
  boolean retVal = TRUE;

  /* validate input pointers */
  if (NULL == pQmiLocSetSensorPropertiesReq)
  {
    return FALSE;
  }

  // perform preliminary parameter validation here as per requested
  if (FALSE == pQmiLocSetSensorPropertiesReq->gyroBiasVarianceRandomWalk_valid &&
      FALSE == pQmiLocSetSensorPropertiesReq->accelerationRandomWalkSpectralDensity_valid &&
      FALSE == pQmiLocSetSensorPropertiesReq->angleRandomWalkSpectralDensity_valid &&
      FALSE == pQmiLocSetSensorPropertiesReq->rateRandomWalkSpectralDensity_valid &&
      FALSE == pQmiLocSetSensorPropertiesReq->velocityRandomWalkSpectralDensity_valid &&
      FALSE == pQmiLocSetSensorPropertiesReq->vehicleDataUse_valid &&
      FALSE == pQmiLocSetSensorPropertiesReq->vehicleVelocityRandomWalkSpectralDensity_valid &&
      FALSE == pQmiLocSetSensorPropertiesReq->vehicleAccelRandomWalkSpectralDensity_valid &&
      FALSE == pQmiLocSetSensorPropertiesReq->vehicleAngleRandomWalkSpectralDensity_valid &&
      FALSE == pQmiLocSetSensorPropertiesReq->vehicleAngularRateRandomWalkSpectralDensity_valid &&
      FALSE == pQmiLocSetSensorPropertiesReq->vehicleOdometryScaleFactorRandomWalkSpectralDensity_valid &&
      FALSE == pQmiLocSetSensorPropertiesReq->vehicleOdometryVariance_valid) /* add AND check for newer params here. Atleast one should be valid */
  {
    // one of them must be valid
    retVal = FALSE;
  } else
  {
    retVal = TRUE;
  }
  return retVal;
}


/**
 * called from LOC-MW task in response to a QMI command to set
 * the sensor properties. This is a proxy
 * because it runs under the caller task context, while the
 * actual processing would happen later in corresponding
 * handler function.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               some unique transaction id to be returned in the resulting indication
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_set_sensor_properties_req(loc_client_info_s_type *const client_ptr,
                                           const qmiLocSetSensorPropertiesReqMsgT_v02 *p_req,
                                           qmiLocGenRespMsgT_v02 *const pResp)
{
  locEngSetNavigationConfigReqMsgT_v01 locEngReq;
  uint32 error_code = 0;
  locSdpUserDataStructT *pLocSdpUserData = NULL;

  do
  {
    locEngStatusEnumT_v01 status = eLOC_ENG_GENERAL_FAILURE_V01;
    if(NULL == pResp)
    {
      error_code = 1;
      break;
    }
    if ((NULL == client_ptr) || (NULL == p_req))
    {
      error_code = 1;
      pResp->resp.error = QMI_ERR_INTERNAL_V01;
      break;
    }

    LOC_MSG_MED("loc_qmiloc_set_sensor_properties_req. client: %d",
                 client_ptr->client_handle, 0, 0);

    pLocSdpUserData = os_MemAlloc(sizeof(*pLocSdpUserData), OS_MEM_SCOPE_TASK);

    if (NULL == pLocSdpUserData)
    {
      error_code = 2;
      pResp->resp.error = QMI_ERR_NO_MEMORY_V01;
      break;
    }

    /* first validate and then convert to locEng format */
    if( FALSE == validateQmiLocSetSensorPropertiesReq(p_req) )
    {
      error_code = 3;
      pResp->resp.error = QMI_ERR_MALFORMED_MSG_V01;
      break;
    }

    /* request is valid: do locEng conversion and send */
    if (FALSE == convertTolocEngSetNavigationConfigReq(p_req, &locEngReq))
    {
      error_code = 4;
      pResp->resp.error = QMI_ERR_MISSING_ARG_V01;
      break;
    }

    pLocSdpUserData->clientHandle = client_ptr->client_handle;
    pLocSdpUserData->transactionId = loc_qmiloc_get_transaction_id();
    pLocSdpUserData->pMe = pLocSdpUserData;
    locEngReq.userData_valid = 1;
    /* Compiler complains about the direct pointer cast to 64bits,
       so need to explicitly define it */
    locEngReq.userData = (uint64_t)((uint32_t)pLocSdpUserData);

    /* use proxy to send */
    status = sdp_locEngSetNavigationConfigReqProxy(&locEngReq);
    if (eLOC_ENG_SUCCESS_V01 != status)
    {
      error_code = 5;
      pResp->resp.error = convertLocEngStatusToQmiError(status);
      break;
    }

    pResp->resp.error = QMI_ERR_NONE_V01;
    pResp->resp.result = QMI_RESULT_SUCCESS_V01;
    return TRUE; /* returns here if success, so free will be called in IND proxy */

  }while (0);

  if (NULL != pLocSdpUserData)
  {
    os_MemFree((void **)&pLocSdpUserData);
  }

  if(NULL != pResp)
  {
    pResp->resp.result = QMI_RESULT_FAILURE_V01;
  }
  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_qmiloc_set_sensor_properties_req: error: %u",
                  error_code, 0, 0);
  }
  return FALSE;
}


/**
 * called from LOC-MW task in response to a QMI command to get
 * the sensor properties. This is a proxy
 * because it runs under the caller task context, while the
 * actual processing would happen later in corresponding
 * handler function.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_get_sensor_properties_req(loc_client_info_s_type *const client_ptr,
                                           const qmiLocGetSensorPropertiesReqMsgT_v02 *p_req,
                                           qmiLocGenRespMsgT_v02 *const pResp)
{
  locEngGetNavigationConfigReqMsgT_v01 locEngReq;
  uint32 error_code = 0;
  locSdpUserDataExtStructT *pLocSdpUserDataExt = NULL;

  do
  {
    locEngStatusEnumT_v01 status = eLOC_ENG_GENERAL_FAILURE_V01;
    if(NULL == pResp)
    {
      error_code = 1;
      break;
    }
    if ((NULL == client_ptr) || (NULL == p_req))
    {
      error_code = 1;
      pResp->resp.error = QMI_ERR_INTERNAL_V01;
      break;
    }

    LOC_MSG_MED("loc_qmiloc_get_sensor_properties_req. client: %d",
                 client_ptr->client_handle, 0, 0);

    pLocSdpUserDataExt = os_MemAlloc(sizeof(*pLocSdpUserDataExt), OS_MEM_SCOPE_TASK);

    if (NULL == pLocSdpUserDataExt)
    {
      error_code = 2;
      pResp->resp.error = QMI_ERR_NO_MEMORY_V01;
      break;
    }

    /* first validate and then convert to locEng format */
    if( FALSE == validateQmiLocGetSensorPropertiesReq(p_req) )
    {
      error_code = 3;
      pResp->resp.error = QMI_ERR_MALFORMED_MSG_V01;
      break;
    }

    /* request is valid: do locEng conversion and send */
    if (FALSE == convertTolocEngGetNavigationConfigReq(p_req, &locEngReq))
    {
      error_code = 4;
      pResp->resp.error = QMI_ERR_MISSING_ARG_V01;
      break;
    }

    pLocSdpUserDataExt->clientHandle = client_ptr->client_handle;
    pLocSdpUserDataExt->transactionId = loc_qmiloc_get_transaction_id();

    pLocSdpUserDataExt->extData = p_req->getSensorPropertiesMask; /* save the bitmask (uint32_t) for later use in IND */

    pLocSdpUserDataExt->pMe = pLocSdpUserDataExt;
    locEngReq.userData_valid = 1;
    /* Compiler complains about the direct pointer cast to 64bits,
       so need to explicitly define it */
    locEngReq.userData = (uint64_t)((uint32_t)pLocSdpUserDataExt);

    /* use proxy to send */
    status = sdp_locEngGetNavigationConfigReqProxy(&locEngReq);
    if (eLOC_ENG_SUCCESS_V01 != status)
    {
      error_code = 5;
      pResp->resp.error = convertLocEngStatusToQmiError(status);
      break;
    }

    pResp->resp.error = QMI_ERR_NONE_V01;
    pResp->resp.result = QMI_RESULT_SUCCESS_V01;
    return TRUE; /* returns here if success, so free will be called in IND proxy */

  }while (0);

  if (NULL != pLocSdpUserDataExt)
  {
    os_MemFree((void **)&pLocSdpUserDataExt);
  }

  if(NULL != pResp)
  {
    pResp->resp.result = QMI_RESULT_FAILURE_V01;
  }
  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_qmiloc_get_sensor_properties_req: error: %u",
                  error_code, 0, 0);
  }
  return FALSE;
}

/**
 * called from LOC-MW task in response to a QMI command to set
 * the sensor control configuration. This is a proxy
 * because it runs under the caller task context, while the
 * actual processing would happen later in corresponding
 * handler function.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               some unique transaction id to be returned in the resulting indication
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_set_sensor_control_config_req(loc_client_info_s_type *const client_ptr,
                                               const qmiLocSetSensorControlConfigReqMsgT_v02 *p_req,
                                               qmiLocGenRespMsgT_v02 *const pResp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

  if ((NULL != client_ptr) && (NULL != pResp) && (NULL != p_req))
  {
    int32 preliminary_result_from_sdp = 0;

    LOC_MSG_HIGH("loc_qmiloc_set_sensor_control_config_req. client: %d",
                 client_ptr->client_handle, 0, 0);

    // pass information to sdp component
    preliminary_result_from_sdp =
      sdp_qmiloc_set_sensor_control_config_proxy(client_ptr->client_handle, p_req);
    loc_qmiloc_fill_general_response(pResp, preliminary_result_from_sdp);

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


/**
 * called from LOC-MW task in response to a QMI command to get
 * the sensor performance control configuration. This is a proxy
 * because it runs under the caller task context, while the
 * actual processing would happen later in corresponding
 * handler function.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               some unique transaction id to be returned in the resulting indication
 * @param pResp  QMI response
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
int32 loc_qmiloc_get_sensor_control_config_req(loc_client_info_s_type *const client_ptr,
                                               qmiLocGenRespMsgT_v02 *const pResp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

  if ((NULL != client_ptr) && (NULL != pResp))
  {
    int32 preliminary_result_from_sdp = 0;

    LOC_MSG_HIGH("loc_qmiloc_get_sensor_control_config_req. client: %d",
                 client_ptr->client_handle, 0, 0);

    // pass information to sdp component
    preliminary_result_from_sdp =
      sdp_qmiloc_get_sensor_control_config_proxy(client_ptr->client_handle);
    loc_qmiloc_fill_general_response(pResp, preliminary_result_from_sdp);

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


/**
 * called from SDP task to indicate success or failure of a QMI
 * command to get the sensor performance control configuration.
 * This is a proxy because it runs under the caller
 * task context, while the actual processing would happen later
 * in corresponding handler function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return LOC_API_SUCCESS if the request has passed parameter
 *         checking and an ipc sent to SDP task for further
 *         processing
 */
int32 loc_qmiloc_set_sensor_performance_control_config_req(loc_client_info_s_type *const client_ptr,
                                                           const qmiLocSetSensorPerformanceControlConfigReqMsgT_v02 *p_req,
                                                           qmiLocGenRespMsgT_v02 *const pResp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

  if ((NULL != client_ptr) && (NULL != pResp) && (NULL != p_req))
  {
    int32 preliminary_result_from_sdp = 0;

    LOC_MSG_HIGH("loc_qmiloc_set_sensor_performance_control_config_req. client: %d",
                 client_ptr->client_handle, 0, 0);

    // pass information to sdp component
    preliminary_result_from_sdp =
      sdp_qmiloc_set_sensor_performance_control_config_proxy(client_ptr->client_handle, p_req);
    loc_qmiloc_fill_general_response(pResp, preliminary_result_from_sdp);

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


/**
 * called from SDP task to indicate success or failure of a QMI
 * command to get the sensor performance control configuration.
 * This is a proxy because it runs under the caller
 * task context, while the actual processing would happen later
 * in corresponding handler function.
 *
 * @param client_ptr
 *               LocAPI client info
 * @param pResp generic response from this function
 *
 * @return LOC_API_SUCCESS if the request has passed parameter
 *         checking and an ipc sent to SDP task for further
 *         processing
 */
int32 loc_qmiloc_get_sensor_performance_control_config_req(loc_client_info_s_type *const client_ptr,
                                                           qmiLocGenRespMsgT_v02 *const pResp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

  if ((NULL != client_ptr) && (NULL != pResp))
  {
    int32 preliminary_result_from_sdp = 0;

    LOC_MSG_HIGH("loc_qmiloc_get_sensor_performance_control_config_req. client: %d",
                 client_ptr->client_handle, 0, 0);

    // pass information to sdp component
    preliminary_result_from_sdp =
      sdp_qmiloc_get_sensor_performance_control_config_proxy(client_ptr->client_handle);
    loc_qmiloc_fill_general_response(pResp, preliminary_result_from_sdp);

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


/**
 * called from SDP task to indicate success or failure of a QMI
 * command to set the sensor control configuration. This is a
 * proxy because it runs under the caller task context, while
 * the actual processing would happen later in corresponding
 * handler function.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               some unique transaction id to be returned in the resulting indication
 * @param p_req  QMI request
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
uint32 loc_sdp_set_sensor_control_config_ind_proxy(const int32 client_handle, const uint32 transaction_id, const qmiLocSetSensorControlConfigIndMsgT_v02 *p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_SET_SENSOR_CONTROL_CONFIG_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_set_sensor_control_config_ind_proxy: error code: %u", error_code, 0, 0);
  }
  return error_code;

}


/**
 * called from SDP task to indicate success or failure of a QMI
 * command to get the sensor control configuration. This is
 * a proxy because it runs under the caller task context, while
 * the actual processing would happen later in corresponding
 * handler function.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               some unique transaction id to be returned in the resulting indication
 * @param p_req  QMI request
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
uint32 loc_sdp_get_sensor_control_config_ind_proxy(const int32 client_handle, const uint32 transaction_id, const qmiLocGetSensorControlConfigIndMsgT_v02 *p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_GET_SENSOR_CONTROL_CONFIG_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_get_sensor_control_config_ind_proxy: error code: %u", error_code, 0, 0);
  }
  return error_code;

}

/**
 * called from SDP task to indicate success or failure of a QMI
 * command to set the performance control configuration. This is
 * a proxy because it runs under the caller task context, while
 * the actual processing would happen later in corresponding
 * handler function.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               some unique transaction id to be returned in the resulting indication
 * @param p_req  QMI request
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
uint32 loc_sdp_set_sensor_performance_control_config_ind_proxy(const int32 client_handle, const uint32 transaction_id, const qmiLocSetSensorPerformanceControlConfigIndMsgT_v02 *p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_set_sensor_performance_control_config_ind_proxy: error code: %u", error_code, 0, 0);
  }
  return error_code;

}


/**
 * called from SDP task to indicate success or failure of a QMI
 * command to get the performance control configuration. This is
 * a proxy because it runs under the caller task context, while
 * the actual processing would happen later in corresponding
 * handler function.
 *
 * @param loc_client_handle
 *               LocAPI client handle
 * @param transaction_id
 *               some unique transaction id to be returned in the resulting indication
 * @param p_req  QMI request
 *
 * @return 0 if the request has passed parameter checking and an ipc sent to SDP task for further processing
 */
uint32 loc_sdp_get_sensor_performance_control_config_ind_proxy(const int32 client_handle, const uint32 transaction_id, const qmiLocGetSensorPerformanceControlConfigIndMsgT_v02 *p_ind)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_get_sensor_performance_control_config_ind_proxy: error code: %u", error_code, 0, 0);
  }
  return error_code;

}

/**
 * handle QMI command to set sensor control
 * configuration
 *
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_sdp_set_sensor_control_config_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 1;

  LOC_MSG_HIGH("[SDP] loc_sdp_set_sensor_control_config_ind_handler.", 0, 0, 0);

  if (NULL != p_ipc_msg_in)
  {
    int32 loc_client_handle = 0;
    uint32 transaction_id = 0;
    qmiLocSetSensorControlConfigIndMsgT_v02 ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT ind_info = { QMI_LOC_SET_SENSOR_CONTROL_CONFIG_IND_V02, NULL, (int32)sizeof(ind) };
    ind_info.p_Msg = &ind;

    if (0 == LOC_SDP_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in, &loc_client_handle, &transaction_id, &ind))
    {
      LOC_MSG_HIGH("[SDP] loc_sdp_set_sensor_control_config_ind_handler. client: %d, tx id: %u, status: %d",
                   loc_client_handle, transaction_id, ind.status);

      // TODO: verify the client ID and transaction id to be valid
      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(loc_client_handle, &ind_info))
      {
        // done
        error_code = 0;
      }
      else
      {
        // failed
        error_code = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      error_code = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_set_sensor_control_config_ind_handler: error code: %u", error_code, 0, 0);
  }
  // no return value
  return;
}

/**
 * handle QMI command to get sensor control
 * configuration
 *
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_sdp_get_sensor_control_config_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 1;

  LOC_MSG_HIGH("[SDP] loc_sdp_get_sensor_control_config_ind_handler.", 0, 0, 0);

  if (NULL != p_ipc_msg_in)
  {
    int32 loc_client_handle = 0;
    uint32 transaction_id = 0;
    qmiLocGetSensorControlConfigIndMsgT_v02 ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT ind_info = { QMI_LOC_GET_SENSOR_CONTROL_CONFIG_IND_V02, NULL, (int32)sizeof(ind) };
    ind_info.p_Msg = &ind;

    if (0 == LOC_SDP_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in, &loc_client_handle, &transaction_id, &ind))
    {
      LOC_MSG_HIGH("[SDP] loc_sdp_get_sensor_control_config_ind_handler. client: %d, tx id: %u, status: %d",
                   loc_client_handle, transaction_id, ind.status);

      // TODO: verify the client ID and transaction id to be valid
      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(loc_client_handle, &ind_info))
      {
        // done
        error_code = 0;
      }
      else
      {
        // failed
        error_code = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      error_code = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_get_sensor_control_config_ind_handler: error code: %u", error_code, 0, 0);
  }
  // no return value
  return;
}

/**
 * handle QMI command to set sensor properties
 *
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_sdp_set_sensor_properties_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 0;
  int32 loc_client_handle = 0;
  uint32 transaction_id = 0;
  locEngSetNavigationConfigRespMsgT_v01 locEngInd;
  qmiLocSetSensorPropertiesIndMsgT_v02 ind;

  locQmiShimIndInfoStructT ind_info = { QMI_LOC_SET_SENSOR_PROPERTIES_IND_V02, NULL, (int32)sizeof(ind) };

  LOC_MSG_MED("[SDP] loc_sdp_set_sensor_properties_ind_handler.", 0, 0, 0);

  /* reset memory */
  memset(&ind, 0, sizeof(ind));

  do
  {
    if (NULL == p_ipc_msg_in)
    {
      error_code = 1;
      break;
    }

    ind_info.p_Msg = &ind;

    if (0 != LOC_SDP_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in,
                                         &loc_client_handle,
                                         &transaction_id,
                                         &locEngInd))
    {
      error_code = 2;
      break;
    }

    LOC_MSG_MED("loc_sdp_set_sensor_properties_ind_handler"
                "client: %d, tx id: %u, status: %d",
                loc_client_handle, transaction_id, locEngInd.status);

    ind.status = convertLocEngStatus(locEngInd.status);

    // TODO: verify the client ID and transaction id to be valid
    // send indication through QCSI
    if (FALSE == locQmiShimSendInd(loc_client_handle, &ind_info))
    {
      // done
      error_code = 3;
      break;
    }


  }while (0);

  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_sdp_set_sensor_properties_ind_handler: error code: %u",
                  error_code, 0, 0);
  }
  // no return value
  return;
}

/**
 * handle QMI command to get sensor properties
 *
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_sdp_get_sensor_properties_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 0;
  int32 loc_client_handle = 0;
  uint32 transaction_id = 0;
  uint32 ext_data = 0;
  locEngGetNavigationConfigRespMsgT_v01 locEngInd;
  qmiLocGetSensorPropertiesIndMsgT_v02 ind;

  locQmiShimIndInfoStructT ind_info = { QMI_LOC_GET_SENSOR_PROPERTIES_IND_V02, NULL, (int32)sizeof(ind) };

  LOC_MSG_MED("[SDP] loc_sdp_get_sensor_properties_ind_handler.", 0, 0, 0);

    /* reset memory */
  memset(&ind, 0, sizeof(ind));

  do
  {
    if (NULL == p_ipc_msg_in)
    {
      error_code = 1;
      break;
    }

    ind_info.p_Msg = &ind;

    if (0 != LOC_SDP_UNPACK_IPC_TO_LOCMW_EXT(p_ipc_msg_in,
                                             &loc_client_handle,
                                             &transaction_id,
                                             &ext_data,
                                             &locEngInd))
    {
      error_code = 2;
      break;
    }

    LOC_MSG_MED("loc_sdp_get_sensor_properties_ind_handler"
                "client: %d, tx id: %u, status: %d",
                loc_client_handle, transaction_id, locEngInd.status);

    ind.status = convertLocEngStatus(locEngInd.status);

    if ( eQMI_LOC_SUCCESS_V02 == ind.status)
    {
      /* fill the fields in QMI-LOC Indication */
      convertFromlocEngGetNavigationConfigResp(&locEngInd, &ind, (qmiLocSensorPropertiesMaskT_v02)ext_data);
    }

    // TODO: verify the client ID and transaction id to be valid
    // send indication through QCSI
    if (FALSE == locQmiShimSendInd(loc_client_handle, &ind_info))
    {
      // done
      error_code = 3;
      break;
    }

  }while (0);

  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_sdp_get_sensor_properties_ind_handler: error code: %u",
                  error_code, 0, 0);
  }
  // no return value
  return;
}

/**
 * handle QMI command to set sensor performance control
 * configuration
 *
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_sdp_set_sensor_performance_control_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 1;

  LOC_MSG_HIGH("[SDP] loc_sdp_set_sensor_performance_control_ind_handler.", 0, 0, 0);

  if (NULL != p_ipc_msg_in)
  {
    int32 loc_client_handle = 0;
    uint32 transaction_id = 0;
    qmiLocSetSensorPerformanceControlConfigIndMsgT_v02 ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT ind_info = { QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02, NULL, (int32)sizeof(ind) };
    ind_info.p_Msg = &ind;

    if (0 == LOC_SDP_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in, &loc_client_handle, &transaction_id, &ind))
    {
      LOC_MSG_HIGH("[SDP] loc_sdp_set_sensor_performance_control_ind_handler. client: %d, tx id: %u, status: %d",
                   loc_client_handle, transaction_id, ind.status);

      // TODO: verify the client ID and transaction id to be valid
      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(loc_client_handle, &ind_info))
      {
        // done
        error_code = 0;
      }
      else
      {
        // failed
        error_code = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      error_code = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_set_sensor_performance_control_ind_handler: error code: %u", error_code, 0, 0);
  }
  // no return value
  return;
}

/**
 * handle QMI command to get sensor performance control
 * configuration
 *
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_sdp_get_sensor_performance_control_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 1;

  LOC_MSG_HIGH("[SDP] loc_sdp_get_sensor_performance_control_ind_handler.", 0, 0, 0);

  if (NULL != p_ipc_msg_in)
  {
    int32 loc_client_handle = 0;
    uint32 transaction_id = 0;
    qmiLocGetSensorPerformanceControlConfigIndMsgT_v02 ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT ind_info = { QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02, NULL, (int32)sizeof(ind) };
    ind_info.p_Msg = &ind;

    if (0 == LOC_SDP_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in, &loc_client_handle, &transaction_id, &ind))
    {
      LOC_MSG_HIGH("[SDP] loc_sdp_get_sensor_performance_control_ind_handler. client: %d, tx id: %u, status: %d",
                   loc_client_handle, transaction_id, ind.status);

      // TODO: verify the client ID and transaction id to be valid
      // send indication through QCSI
      if (TRUE == locQmiShimSendInd(loc_client_handle, &ind_info))
      {
        // done
        error_code = 0;
      }
      else
      {
        // failed
        error_code = 2;
      }
    }
    else
    {
      // ipc unpacking unsuccessful
      // no indication would be sent back
      error_code = 3;
    }
  }
  else
  {
    // null pointer for request. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[SDP] loc_sdp_get_sensor_performance_control_ind_handler: error code: %u", error_code, 0, 0);
  }
  // no return value
  return;
}

/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 *
 * @param [in] p_ind  the QMI indication
 *
 * @return 0 if operation is successful
 */
uint32 loc_sdp_get_navigation_config_resp_ind_proxy (const locEngGetNavigationConfigRespMsgT_v01 * const p_ind)
{
  uint32 error_code = 0;
  int32 client_handle = 0;
  uint32 transaction_id = 0;
  uint32 ext_data = 0;
  locSdpUserDataExtStructT *pUserDataExt = NULL;

  do
  {
    if (NULL == p_ind)
    {
      //assign a different error code than IND_PROXY
      error_code = 6;
      break;
    }

    pUserDataExt = (locSdpUserDataExtStructT *)((uint32_t)p_ind->userData);

    if (FALSE == p_ind->userData_valid ||
        eLOC_ENG_MSG_SOURCE_SDP_V01 != p_ind->messageSource ||
        NULL == pUserDataExt ||
        pUserDataExt != pUserDataExt->pMe)
    {
      //assign a different error code than IND_PROXY
      error_code = 7;
      break;
    }

    /* extract clientId and transationID fields */
    client_handle = pUserDataExt->clientHandle;
    transaction_id = pUserDataExt->transactionId;
    ext_data = pUserDataExt->extData;

    IND_PROXY_EXT(LM_MIDDLEWARE_MSG_ID_GET_SENSOR_PROPERTIES_IND)

    LOC_MSG_MED("[SDP] LM_MIDDLEWARE_MSG_ID_GET_SENSOR_PROPERTIES_IND  posted", 0, 0, 0);

  }while (0);

  // free up the user data
  if (NULL != pUserDataExt)
  {
    os_MemFree((void **)&pUserDataExt);
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_sdp_get_navigation_config_resp_ind_proxy: "
                  "error code: %u", error_code, 0, 0);
  }
  return error_code;
}


/**
 * called by SDP task to send QMI indication to LocMW task or directly to QCSI
 *
 * @param [in] p_ind  the QMI indication
 *
 * @return 0 if operation is successful
 */
uint32 loc_sdp_set_navigation_config_resp_ind_proxy (const locEngSetNavigationConfigRespMsgT_v01 * const p_ind)
{
  uint32 error_code = 0;
  int32 client_handle = 0;
  uint32 transaction_id = 0;
  locSdpUserDataStructT *pUserData = NULL;

  do
  {
    if (NULL == p_ind)
    {
      //assign a different error code than IND_PROXY
      error_code = 6;
      break;
    }

    pUserData = (locSdpUserDataStructT *)((uint32_t)p_ind->userData);

    if (FALSE == p_ind->userData_valid ||
        eLOC_ENG_MSG_SOURCE_SDP_V01 != p_ind->messageSource ||
        NULL == pUserData ||
        pUserData != pUserData->pMe)
    {
      //assign a different error code than IND_PROXY
      error_code = 7;
      break;
    }

    /* extract clientId and transationID fields */
    client_handle = pUserData->clientHandle;
    transaction_id = pUserData->transactionId;

    IND_PROXY(LM_MIDDLEWARE_MSG_ID_SET_SENSOR_PROPERTIES_IND)

    LOC_MSG_MED("[SDP] LM_MIDDLEWARE_MSG_ID_SET_SENSOR_PROPERTIES_IND  posted", 0, 0, 0);

  }while (0);

  // free up the user data
  if (NULL != pUserData)
  {
    os_MemFree((void **)&pUserData);
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_sdp_set_navigation_config_resp_ind_proxy: "
                  "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

