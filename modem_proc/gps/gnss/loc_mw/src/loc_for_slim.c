/*============================================================================
  FILE:         loc_for_slim.c

  OVERVIEW:     Implementation of functions for SLIM module to communicate with
                Loc MW task. All services needed by SLIM from LocMW task should
                be implemented to this module.

  DEPENDENCIES: None.
 
                Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
                All Rights Reserved.
                Qualcomm Atheros Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_for_slim.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-08-22  lv  Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "loc_api_internal.h"
#include "loc_conv_locEng_qmiLoc.h"
#include "loc_for_slim.h"
#include "loc_slim.h"
#include "loc_qmi_shim.h"
#include "loc_utils.h"
#include "msg.h"
#include "slim_for_loc.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef locUserDataStructT locSlimUserDataStructT;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/**
 * fill in general qmiloc response based on preliminary validation result from SLIM
 *
 * @param pResp  [out] response to be filled
 * @param preliminary_result_from_slim
 *               [in] validation result from slim
 */
static void loc_slim_qmiloc_fill_general_response
(
  qmiLocGenRespMsgT_v02 *const pResp,
  const uint32 preliminary_result_from_slim
)
{
  if (NULL != pResp)
  {
    memset(pResp, 0, sizeof(*pResp));
    //initial value, changes to SUCCESS for success case
    // does not change for failure cases
    pResp->resp.result = QMI_RESULT_FAILURE_V01;
    switch (preliminary_result_from_slim)
    {
      case SLIM_QMI_LOC_SUCCESS:
        pResp->resp.error = QMI_ERR_NONE_V01;
        pResp->resp.result = QMI_RESULT_SUCCESS_V01;
        break;
      case SLIM_QMI_LOC_ERROR_UNSUPPORTED:
         pResp->resp.error = QMI_ERR_NOT_SUPPORTED_V01;
         break;
      default:
         pResp->resp.error = QMI_ERR_MISSING_ARG_V01;
         break;
    }
    LOC_MSG_HIGH("loc_slim_qmiloc_fill_general_response. validation: %u, result: %u, error: %u",
                 preliminary_result_from_slim, (uint32)pResp->resp.result, (uint32)pResp->resp.error); 
  }
  else
  {
    // this shall never happen
    LOC_MSG_ERROR("loc_slim_qmiloc_fill_general_response. null pointer", 0, 0, 0);
  }
}

static boolean validateQmiLocInjectVehicleSensorDataReq
(
  const qmiLocInjectVehicleSensorDataReqMsgT_v02 *pQmiLocInjectVehicleSensorDataReq
)
{
  boolean retVal = TRUE;

  // validate input pointers
  if (NULL == pQmiLocInjectVehicleSensorDataReq)
  {
    LOC_MSG_ERROR("validateQmiLocInjectVehicleSensorDataReq. null pointer", 0, 0, 0);
    return FALSE;
  }

  // perform preliminary parameter validation here as per requested
  if (FALSE == pQmiLocInjectVehicleSensorDataReq->accelData_valid &&
      FALSE == pQmiLocInjectVehicleSensorDataReq->angRotationData_valid &&
      FALSE == pQmiLocInjectVehicleSensorDataReq->odometryData_valid &&
      FALSE == pQmiLocInjectVehicleSensorDataReq->changeInTimeScales_valid) /* add AND check for newer params here. Atleast one should be valid */
  {
    // one of them must be valid
    retVal = FALSE;
  }

  return retVal;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/**
@brief Initiates a time sync request via QMI_LOC.
  
SLIM can send time synchronization request via QMI_LOC using this function.
  
@param  ptr_ind: Pointer to the message data.
@return 0 if message is sent successfully.
*/
uint32 loc_slim_send_event_time_sync_needed_ind
(
  const qmiLocEventTimeSyncReqIndMsgT_v02 *const ptr_ind
)
{
  uint32 error_code = 1;
  locQmiShimIndInfoStructT ind_info =
    { QMI_LOC_EVENT_TIME_SYNC_REQ_IND_V02, NULL, (int32)sizeof(*ptr_ind) };
  ind_info.p_Msg = (void *)ptr_ind;

  if (NULL != ptr_ind)
  {
    // send indication through QCSI
    if (TRUE == locQmiShimBroadcastInd(QMI_LOC_EVENT_MASK_TIME_SYNC_REQ_V02,
                                       &ind_info))
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
    LOC_MSG_ERROR("loc_slim_send_event_time_sync_needed_ind: error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
@brief Initiates a sensor data request via QMI_LOC.
  
SLIM can send sensor data streaming request via QMI_LOC using this function.
  
@param  ptr_ind: Pointer to the message data.
@return 0 if message is sent successfully.
*/
uint32 loc_slim_send_event_sensor_streaming_readiness_ind
(
  const qmiLocEventSensorStreamingReadyStatusIndMsgT_v02 *const ptr_ind
)
{
  uint32 error_code = 1;
  uint64_t mask = 0;
  locQmiShimIndInfoStructT ind_info =
    { QMI_LOC_EVENT_SENSOR_STREAMING_READY_STATUS_IND_V02, NULL, (int32)sizeof(*ptr_ind) };
  ind_info.p_Msg = (void *)ptr_ind;

  if (NULL != ptr_ind)
  {
    if (TRUE == ptr_ind->accelReady_valid
      || TRUE == ptr_ind->gyroReady_valid
      || TRUE == ptr_ind->accelTemperatureReady_valid
      || TRUE == ptr_ind->gyroTemperatureReady_valid
      || TRUE == ptr_ind->calibratedMagReady_valid
      || TRUE == ptr_ind->uncalibratedMagReady_valid)
    {
      mask |= QMI_LOC_EVENT_MASK_SENSOR_STREAMING_READY_STATUS_V02;
    }

    if (0 != mask)
    {
      // send indication through QCSI
      if (TRUE == locQmiShimBroadcastInd(mask, &ind_info))
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
      // nothing to send. this shall not happen
      error_code = 3;
    }
  }
  else
  {
    // NULL indication, this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_slim_send_event_sensor_streaming_readiness_ind: "
      "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
@brief Initiates a motion data request via QMI_LOC.
  
SLIM can send motion data streaming request via QMI_LOC using this function.
  
@param  ptr_ind: Pointer to the message data.
@return 0 if message is sent successfully.
*/
uint32 loc_slim_send_event_motion_data_control_ind
(
  const qmiLocEventMotionDataControlIndMsgT_v02 *const ptr_ind
)
{
  uint32 error_code = 0;
  uint64_t mask = QMI_LOC_EVENT_MASK_MOTION_DATA_CONTROL_V02;
  locQmiShimIndInfoStructT ind_info =
    { QMI_LOC_EVENT_MOTION_DATA_CONTROL_IND_V02, NULL, (int32)sizeof(*ptr_ind) };
  ind_info.p_Msg = (void *)ptr_ind;

  if (NULL != ptr_ind)
  {
    // send indication through QCSI
    if (TRUE == locQmiShimBroadcastInd(mask, &ind_info))
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
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_slim_send_event_motion_data_control_ind:"
                  " error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
@brief Initiates a pedometer request via QMI_LOC.
  
SLIM can send pedometer streaming request via QMI_LOC using this function.
  
@param  ptr_ind: Pointer to the message data.
@return 0 if message is sent successfully.
*/
uint32 loc_slim_send_event_pedometer_control_ind
(
  const qmiLocEventPedometerControlIndMsgT_v02 *const ptr_ind
)
{
  uint32 error_code = 0;
  uint64_t mask = QMI_LOC_EVENT_MASK_PEDOMETER_CONTROL_V02;
  locQmiShimIndInfoStructT ind_info =
    { QMI_LOC_EVENT_PEDOMETER_CONTROL_IND_V02, NULL, (int32)sizeof(*ptr_ind) };
  ind_info.p_Msg = (void *)ptr_ind;

  if (NULL != ptr_ind)
  {
    // send indication through QCSI
    if (TRUE == locQmiShimBroadcastInd(mask, &ind_info))
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
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_slim_send_event_pedometer_control_ind:"
                  " error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
@brief Initiates a vehicle data request via QMI_LOC.
  
SLIM can send vehicle data streaming request via QMI_LOC using this function.
  
@param  ptr_ind: Pointer to the message data.
@return 0 if message is sent successfully.
*/
uint32 loc_slim_send_event_vehicle_sensor_injection_ind
(
  const qmiLocEventVehicleDataReadyIndMsgT_v02 *const ptr_ind
)
{
  uint32 error_code = 1;
  uint64_t mask = 0;
  locQmiShimIndInfoStructT ind_info =
    { QMI_LOC_EVENT_VEHICLE_DATA_READY_STATUS_IND_V02, NULL, (int32)sizeof(*ptr_ind) };
  ind_info.p_Msg = (void *)ptr_ind;

  LOC_MSG_HIGH("[SLIM] loc_slim_send_event_vehicle_sensor_injection_ind.", 0, 0, 0);

  if (NULL != ptr_ind)
  {
    if (TRUE == ptr_ind->vehicleAccelReadyStatus_valid ||
        TRUE == ptr_ind->vehicleAngularRateReadyStatus_valid ||
        TRUE == ptr_ind->vehicleOdometryReadyStatus_valid)
    {
      mask |= QMI_LOC_EVENT_MASK_VEHICLE_DATA_READY_STATUS_V02;
    }
    if (0 != mask)
    {
      // send indication through QCSI
      if (TRUE == locQmiShimBroadcastInd(mask, &ind_info))
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
      // nothing to send. this shall not happen
      error_code = 3;
    }
  }
  else
  {
    // NULL indication, this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[LOC] loc_slim_send_event_vehicle_sensor_injection_ind: "
      "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
@brief Sends time sync request status indication.
  
SLIM can send the status of the time synchronization request handling via QMI_LOC
using this function.
  
@param  client_handle: LocAPI client handle
@param  transaction_id: Same transaction id passed in earlier by the request
@param  p_ind: The QMI indication
@return 0 if message is sent successfully.
*/
uint32 loc_slim_inject_time_sync_data_ind_proxy
(
  const int32 client_handle,
  const uint32 transaction_id,
  const qmiLocInjectTimeSyncDataIndMsgT_v02 *const p_ind
)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_SDP_INJECT_TIME_SYNC_DATA_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[LOC] loc_slim_inject_time_sync_data_ind_proxy: "
      "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
@brief Sends sensor data request status indication.
  
SLIM can send the status of the sensor data request handling via QMI_LOC
using this function.
  
@param  client_handle: LocAPI client handle
@param  transaction_id: Same transaction id passed in earlier by the request
@param  p_ind: The QMI indication
@return 0 if message is sent successfully.
*/
uint32 loc_slim_inject_sensor_data_ind_proxy
(
  const int32 client_handle, 
  const uint32 transaction_id, 
  const qmiLocInjectSensorDataIndMsgT_v02 * const p_ind
)
{
  uint32 error_code = 1;

  IND_PROXY(LM_MIDDLEWARE_MSG_ID_SDP_INJECT_SENSOR_DATA_IND)

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[LOC] loc_slim_inject_sensor_data_ind_proxy: "
      "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
@brief Sends motion data request status indication.
  
SLIM can send the status of the motion data request handling via QMI_LOC
using this function.
  
@param  p_ind: The QMI indication
@return 0 if message is sent successfully.
*/
uint32 loc_slim_inject_motion_data_ind_proxy
(
  const locEngInjectMotionDataIndMsgT_v01 * const p_ind
)
{
  uint32 error_code = 0;
  int32 client_handle = 0;
  uint32 transaction_id = 0;
  locSlimUserDataStructT *pUserData = NULL;
  do
  {
    if (NULL == p_ind)
    {
      //assign a different error code than IND_PROXY
      error_code = 4;
      break;
    }

    pUserData = (locSlimUserDataStructT *)((uint32_t)p_ind->userData);

    if (FALSE == p_ind->userData_valid ||
        eLOC_ENG_MSG_SOURCE_SDP_V01 != p_ind->messageSource ||
        NULL == pUserData ||
        pUserData != pUserData->pMe)
    {
      //assign a different error code than IND_PROXY
      error_code = 5;
      break;
    }

    client_handle = pUserData->clientHandle;
    transaction_id = pUserData->transactionId;

    IND_PROXY(LM_MIDDLEWARE_MSG_ID_INJECT_MOTION_DATA_IND)

  }while (0);

  // free up the user data
  if (NULL != pUserData)
  {
    os_MemFree((void **)&pUserData);
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_slim_inject_motion_data_ind_proxy: "
                  "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
@brief Sends pedometer request status indication.
  
SLIM can send the status of the pedometer request handling via QMI_LOC
using this function.
  
@param  p_ind: The QMI indication
@return 0 if message is sent successfully.
*/
uint32 loc_slim_inject_pedometer_ind_proxy
(
  const locEngPedometerReportIndMsgT_v01 * const p_ind
)
{
  uint32 error_code = 0;
  int32 client_handle = 0;
  uint32 transaction_id = 0;
  locSlimUserDataStructT *pUserData = NULL;
  do
  {
    if (NULL == p_ind)
    {
      //assign a different error code than IND_PROXY
      error_code = 4;
      break;
    }

    pUserData = (locSlimUserDataStructT *)((uint32_t)p_ind->userData);

    if ((FALSE == p_ind->userData_valid) ||
        (eLOC_ENG_MSG_SOURCE_SDP_V01 != p_ind->messageSource) ||
        (NULL == pUserData) ||
        (pUserData != pUserData->pMe))
    {
      //assign a different error code than IND_PROXY
      error_code = 5;
      break;
    }

    client_handle = pUserData->clientHandle;
    transaction_id = pUserData->transactionId;
    IND_PROXY(LM_MIDDLEWARE_MSG_ID_PEDOMTER_REPORT_IND)


  }while (0);

  // free up the user data
  if (NULL != pUserData)
  {
    os_MemFree((void **)&pUserData);
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_slim_pedometer_report_ind_proxy: error code: %u",
                  error_code, 0, 0);
  }

  return error_code;
}

/**
@brief Sends vehicle data request status indication.
  
SLIM can send the status of the vehicle data request handling via QMI_LOC
using this function.
  
@param  p_ind: The QMI indication
@return 0 if message is sent successfully.
*/
uint32 loc_slim_inject_vehicle_sensor_data_resp_ind_proxy
(
  const locEngInjectVehicleSensorDataRespMsgT_v01 * const p_ind
)
{
  uint32 error_code = 0;
  int32 client_handle = 0;
  uint32 transaction_id = 0;
  locSlimUserDataStructT *pUserData = NULL;

  do
  {
    if (NULL == p_ind)
    {
      //assign a different error code than IND_PROXY
      error_code = 6;
      break;
    }

    pUserData = (locSlimUserDataStructT *)((uint32_t)p_ind->userData);

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

    IND_PROXY(LM_MIDDLEWARE_MSG_ID_SDP_INJECT_VEHICLE_SENSOR_DATA_IND)

    LOC_MSG_MED("[LOC] LM_MIDDLEWARE_MSG_ID_SDP_INJECT_VEHICLE_SENSOR_DATA_IND  posted", 0, 0, 0);

  }while (0);

  // free up the user data
  if (NULL != pUserData)
  {
    os_MemFree((void **)&pUserData);
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_slim_inject_vehicle_sensor_data_resp_ind_proxy: "
                  "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
 * handle QMI command to inject sensor data
 *
 * @param client_ptr LocAPI client handle
 * @param pReq       QMI request
 * @param pResp      QMI response
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
int32 loc_qmiloc_inject_sensor_data_req(loc_client_info_s_type *const client_ptr,
                                        const qmiLocInjectSensorDataReqMsgT_v02 *const pReq,
                                        qmiLocGenRespMsgT_v02 *const pResp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

  if ((NULL != client_ptr) && (NULL != pReq) && (NULL != pResp))
  {
    // failure and aborted is the closest match i can find in the set of qmi error code
    uint32 preliminary_result_from_slim = 0;

    LOC_MSG_HIGH("loc_qmiloc_inject_sensor_data_req. client: %d, opaque id: %u",
                 client_ptr->client_handle, (uint32)pReq->opaqueIdentifier, 0);

    // pass information to slim component
    preliminary_result_from_slim = 
      slim_LocInjectSensorDataRequest(client_ptr->client_handle,
                                      loc_qmiloc_get_transaction_id(),
                                      pReq);
    loc_slim_qmiloc_fill_general_response(pResp, preliminary_result_from_slim);

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
 * handle QMI command to inject time sync data
 *
 * @param client_ptr LocAPI client handle
 * @param pReq       QMI request
 * @param pResp      QMI response
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
int32 loc_qmiloc_inject_time_sync_data_req(loc_client_info_s_type *const client_ptr,
                                           const qmiLocInjectTimeSyncDataReqMsgT_v02 *const pReq,
                                           qmiLocGenRespMsgT_v02 *const pResp)
{
  int32 result = LOC_API_GENERAL_FAILURE;

  if ((NULL != client_ptr) && (NULL != pReq) && (NULL != pResp))
  {
    int32 preliminary_result_from_slim = 0;

    LOC_MSG_HIGH("loc_qmiloc_inject_time_sync_data_req. client: %d, ref counter: %u",
                 client_ptr->client_handle, (uint32)pReq->refCounter, 0);

    // pass information to slim component
    preliminary_result_from_slim = 
      slim_LocInjectTimeSyncDataRequest(client_ptr->client_handle,
                                        loc_qmiloc_get_transaction_id(),
                                        pReq);
    loc_slim_qmiloc_fill_general_response(pResp, preliminary_result_from_slim);

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
 * called by LocMW task loop to handle ipc message for QMI indication qmiLocInjectSensorDataIndMsgT_v02
 *
 * @param p_ipc_msg_in   ipc message
 */
void loc_qmiloc_inject_sensor_data_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 1;

  LOC_MSG_HIGH("[LOC] loc_qmiloc_inject_sensor_data_ind_handler.", 0, 0, 0);

  if (NULL != p_ipc_msg_in)
  {
    int32 loc_client_handle = 0;
    uint32 transaction_id = 0;
    qmiLocInjectSensorDataIndMsgT_v02 ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT ind_info = { QMI_LOC_INJECT_SENSOR_DATA_IND_V02, NULL, (int32)sizeof(ind) };
    ind_info.p_Msg = &ind;

    if (0 == LOC_SLIM_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in, &loc_client_handle, &transaction_id, &ind))
    {
      LOC_MSG_HIGH("[LOC] loc_qmiloc_inject_sensor_data_ind_handler. client: %d, tx id: %u, status: %d",
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
    LOC_MSG_ERROR("[LOC] loc_slim_inject_sensor_data_ind_handler: error code: %u", error_code, 0, 0);
  }
  // no return value
  return;
}

/**
 * called by LocMW task loop to handle ipc message for QMI indication qmiLocInjectTimeSyncDataIndMsgT_v02
 *
 * @param p_ipc_msg_in   ipc message
 */
void loc_qmiloc_inject_time_sync_data_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 1;

  LOC_MSG_HIGH("[LOC] loc_qmiloc_inject_time_sync_data_ind_handler.", 0, 0, 0);

  if (NULL != p_ipc_msg_in)
  {
    int32 loc_client_handle = 0;
    uint32 transaction_id = 0;
    qmiLocInjectTimeSyncDataIndMsgT_v02 ind;

    // cannot use 'const' here because we need to set p_Msg to a different value after aggregate initialization
    // C89 doesn't allow non-const aggregate initializer
    locQmiShimIndInfoStructT ind_info = { QMI_LOC_INJECT_TIME_SYNC_DATA_IND_V02, NULL, (int32)sizeof(ind) };
    ind_info.p_Msg = &ind;

    if (0 == LOC_SLIM_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in, &loc_client_handle, &transaction_id, &ind))
    {
      LOC_MSG_HIGH("[LOC] loc_qmiloc_inject_time_sync_data_ind_handler. client: %d, tx id: %u, status: %d",
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
    LOC_MSG_ERROR("[LOC] loc_qmiloc_inject_time_sync_data_ind_handler: error code: %u", error_code, 0, 0);
  }
  // no return value
  return;
}

/** loc_slim_pedometer_report_req
 * @param client_ptr LocAPI client info 
 * @param p_req  QMI request
 * @param pResp  QMI response
 *
 * @return TRUE : If the request has passed parameter 
 *                checking and an ipc sent to SLIM for
 *                further processing
 *         FALSE: Otherwise
 */
boolean loc_slim_pedometer_report_req(
  loc_client_info_s_type *const client_ptr,
  const qmiLocPedometerReportReqMsgT_v02 *pReq,
  qmiLocGenRespMsgT_v02 *const pResp)
{
  uint32 error_code = 0;
  locEngPedometerReportReqMsgT_v01 locEngReq;
  locSlimUserDataStructT *pLocSlimUserData = NULL;
  do
  {
    locEngStatusEnumT_v01 status = eLOC_ENG_GENERAL_FAILURE_V01;
    if(NULL == pResp)
    {
      error_code = 1;
      break;
    }
    if ((NULL == client_ptr) || (NULL == pReq))
    {
      error_code = 1;
      pResp->resp.error = QMI_ERR_INTERNAL_V01;
      break;
    }

    pLocSlimUserData = os_MemAlloc(sizeof(*pLocSlimUserData), OS_MEM_SCOPE_TASK);

    if (NULL == pLocSlimUserData)
    {
      error_code = 2;
      pResp->resp.error = QMI_ERR_NO_MEMORY_V01;
      break;
    }

    if (FALSE == convertQmiLocPedomterReportReq(pReq,
                                                &locEngReq))
    {
      error_code = 3;
      pResp->resp.error = QMI_ERR_MISSING_ARG_V01;
      break;
    }

    pLocSlimUserData->clientHandle = client_ptr->client_handle;
    pLocSlimUserData->transactionId = loc_qmiloc_get_transaction_id();
    pLocSlimUserData->pMe = pLocSlimUserData;
    locEngReq.userData_valid = 1;
    /* Compiler complains about the direct pointer cast to 64bits,
       so need to explicitly define it */
    locEngReq.userData = (uint64_t)((uint32_t)pLocSlimUserData);

    status = slim_LocInjectPedometerRequest(&locEngReq);
    if (eLOC_ENG_SUCCESS_V01 != status)
    {
      error_code = 4;
      pResp->resp.error = convertLocEngStatusToQmiError(status);
      break;
    }

    pResp->resp.error = QMI_ERR_NONE_V01;
    pResp->resp.result = QMI_RESULT_SUCCESS_V01;
    return TRUE;

  }while (0);
  
  if (NULL != pLocSlimUserData)
  {
    os_MemFree((void **)&pLocSlimUserData);
  }
  if(NULL != pResp)
  {
    pResp->resp.result = QMI_RESULT_FAILURE_V01;
  }
  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_slim_pedometer_report_req: error: %u",
                  error_code, 0, 0);
  }
  return FALSE;
}

/**
 * handle QMI indication containing the status for pedometer 
 * report request , called from loc MW task context, sends 
 * QMI_LOC indication back. 
 *  
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_slim_pedometer_report_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 0;
  int32 loc_client_handle = 0;
  uint32 transaction_id = 0;
  locEngPedometerReportIndMsgT_v01 locEngInd;
  qmiLocPedometerReportIndMsgT_v02 ind;
  locQmiShimIndInfoStructT ind_info = {
    QMI_LOC_PEDOMETER_REPORT_IND_V02,
    NULL,
    (int32)sizeof(ind) };

  do
  {
    if (NULL == p_ipc_msg_in)
    {
      error_code = 1;
      break;
    }

    ind_info.p_Msg = &ind;

    if (0 != LOC_SLIM_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in,
                                         &loc_client_handle,
                                         &transaction_id,
                                         &locEngInd))
    {
      error_code = 2;
      break;
    }

    LOC_MSG_MED("loc_slim_pedometer_report_ind_handler"
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
    LOC_MSG_ERROR("loc_slim_pedometer_report_ind_handler: error code: %u",
                  error_code, 0, 0);
  }
  // no return value
  return;
}

/**
 * handle QMI command to inject motion data
 *
 * @param client_ptr  Loc MW client info 
 * @param pReq        motion data payload  
 * @param pResp       generic response from this function 
 *
 * @return TRUE : If the request has passed parameter 
 *                checking and an ipc sent to SLIM for
 *                further processing
 *         FALSE: Otherwise
 *
 */
boolean loc_slim_inject_motion_data_req(
  loc_client_info_s_type *const client_ptr,
  const qmiLocInjectMotionDataReqMsgT_v02 *pReq,
  qmiLocGenRespMsgT_v02 *const pResp)
{ 
  uint32 error_code = 0;
  locEngInjectMotionDataReqMsgT_v01 locEngReq;
  locSlimUserDataStructT *pLocSlimUserData = NULL;
  do
  {
    locEngStatusEnumT_v01 status = eLOC_ENG_GENERAL_FAILURE_V01;
    if(NULL == pResp)
    {
      error_code = 1;
      break;
    }
    if ((NULL == client_ptr) || (NULL == pReq))
    {
      error_code = 1;
      pResp->resp.error = QMI_ERR_INTERNAL_V01;
      break;
    }

    pLocSlimUserData = os_MemAlloc(sizeof(*pLocSlimUserData), OS_MEM_SCOPE_TASK);

    if (NULL == pLocSlimUserData)
    {
      error_code = 2;
      pResp->resp.error = QMI_ERR_NO_MEMORY_V01;
      break;
    }

    if (FALSE == convertQmiLocInjectMotionDataReq(pReq,
                                                  &locEngReq))
    {
      error_code = 3;
      pResp->resp.error = QMI_ERR_MISSING_ARG_V01;
      break;
    }

    pLocSlimUserData->clientHandle = client_ptr->client_handle;
    pLocSlimUserData->transactionId = loc_qmiloc_get_transaction_id();
    pLocSlimUserData->pMe = pLocSlimUserData;
    locEngReq.userData_valid = 1;
    /* Compiler complains about the direct pointer cast to 64bits,
       so need to explicitly define it */
    locEngReq.userData = (uint64_t)((uint32_t)pLocSlimUserData);

    status = slim_LocInjectMotionDataRequest(&locEngReq);
    if (eLOC_ENG_SUCCESS_V01 != status)
    {
      error_code = 4;
      pResp->resp.error = convertLocEngStatusToQmiError(status);
      break;
    }

    pResp->resp.error = QMI_ERR_NONE_V01;
    pResp->resp.result = QMI_RESULT_SUCCESS_V01;
    return TRUE;

  }while (0);
  
  if (NULL != pLocSlimUserData)
  {
    os_MemFree((void **)&pLocSlimUserData);
  }
  if(NULL != pResp)
  {
    pResp->resp.result = QMI_RESULT_FAILURE_V01;
  }
  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_slim_inject_motion_data: error: %u",
                  error_code, 0, 0);
  }
  return FALSE;
}


/**
 * handle QMI indication containing the status for pedometer 
 * report request , called from loc MW task context, sends 
 * QMI_LOC indication back. 
 *  
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_slim_inject_motion_data_ind_handler(
  const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 0;
  int32 loc_client_handle = 0;
  uint32 transaction_id = 0;
  locEngInjectMotionDataIndMsgT_v01 locEngInd;
  qmiLocInjectMotionDataIndMsgT_v02 ind;
  locQmiShimIndInfoStructT ind_info = {
    QMI_LOC_INJECT_MOTION_DATA_IND_V02,
    NULL,
    (int32)sizeof(ind) };

  do
  {
    if (NULL == p_ipc_msg_in)
    {
      error_code = 1;
      break;
    }

    ind_info.p_Msg = &ind;

    if (0 != LOC_SLIM_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in,
                                         &loc_client_handle,
                                         &transaction_id,
                                         &locEngInd))
    {
      error_code = 2;
      break;
    }

    LOC_MSG_MED("loc_slim_inject_motion_data_ind_handler"
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
    LOC_MSG_ERROR("loc_slim_inject_motion_data_ind_handler: error code: %u",
                  error_code, 0, 0);
  }
  // no return value
  return;
}


/**
 * handle QMI indication containing the status for inject 
 * vehicle sensor data report request , called from loc MW task 
 * context, sends QMI_LOC indication back. 
 *  
 * @param p_ipc_msg_in pointer to ipc message containing the
 *                     request
 *
 * @return none
 */
void loc_slim_inject_vehicle_sensor_data_ind_handler(const os_IpcMsgType *const p_ipc_msg_in)
{
  uint32 error_code = 0;
  int32 loc_client_handle = 0;
  uint32 transaction_id = 0;
  locEngInjectVehicleSensorDataRespMsgT_v01 locEngInd;
  qmiLocInjectVehicleSensorDataIndMsgT_v02 ind;

  locQmiShimIndInfoStructT ind_info = { QMI_LOC_INJECT_VEHICLE_SENSOR_DATA_IND_V02, NULL, (int32)sizeof(ind) };

  LOC_MSG_HIGH("[LOC] loc_slim_inject_vehicle_sensor_data_ind_handler.", 0, 0, 0);

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

    if (0 != LOC_SLIM_UNPACK_IPC_TO_LOCMW(p_ipc_msg_in,
                                         &loc_client_handle,
                                         &transaction_id,
                                         &locEngInd))
    {
      error_code = 2;
      break;
    }

    LOC_MSG_MED("loc_slim_inject_vehicle_sensor_data_ind_handler"
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
    LOC_MSG_ERROR("loc_slim_inject_vehicle_sensor_data_ind_handler: error code: %u",
                  error_code, 0, 0);
  }
  // no return value
  return;
}


/**
 * handle QMI command to inject vehicle sensor data
 *
 * @param client_ptr LocAPI client handle
 * @param pReq       QMI request
 * @param pResp      QMI response
 *
 * @return LOC_API_SUCCESS if the command has been handled successfully
 */
int32 loc_slim_inject_vehicle_sensor_data_req(loc_client_info_s_type *const client_ptr,
                                             const qmiLocInjectVehicleSensorDataReqMsgT_v02 *const pReq,
                                             qmiLocGenRespMsgT_v02 *const pResp)
{
  locEngInjectVehicleSensorDataReqMsgT_v01 locEngReq;
  uint32 error_code = 0;
  locSlimUserDataStructT *pLocSlimUserData = NULL;

  do
  {
    locEngStatusEnumT_v01 status = eLOC_ENG_GENERAL_FAILURE_V01;
    if(NULL == pResp)
    {
      error_code = 1;
      break;
    }

    if ((NULL == client_ptr) || (NULL == pReq))
    {
      error_code = 1;
      pResp->resp.error = QMI_ERR_INTERNAL_V01;
      break;
    }

    LOC_MSG_HIGH("loc_slim_inject_vehicle_sensor_data_req. client: %d",
                 client_ptr->client_handle, 0, 0);

    pLocSlimUserData = os_MemAlloc(sizeof(*pLocSlimUserData), OS_MEM_SCOPE_TASK);

    if (NULL == pLocSlimUserData)
    {
      error_code = 2;
      pResp->resp.error = QMI_ERR_NO_MEMORY_V01;
      break;
    }

    /* first validate and then convert to locEng format */
    if( FALSE == validateQmiLocInjectVehicleSensorDataReq(pReq) )
    {
      error_code = 3;
      pResp->resp.error = QMI_ERR_MALFORMED_MSG_V01;
      break;
    }

    /* request is valid: do locEng conversion and send */
    if (FALSE == convertToLocEngInjectVehicleSensorDataReq(pReq, &locEngReq))
    {
      error_code = 4;
      pResp->resp.error = QMI_ERR_MISSING_ARG_V01;
      break;
    }

    pLocSlimUserData->clientHandle = client_ptr->client_handle;
    pLocSlimUserData->transactionId = loc_qmiloc_get_transaction_id();
    pLocSlimUserData->pMe = pLocSlimUserData;
    locEngReq.userData_valid = 1;
    /* Compiler complains about the direct pointer cast to 64bits,
       so need to explicitly define it */
    locEngReq.userData = (uint64_t)((uint32_t)pLocSlimUserData);

    /* send data to SLIM */
    status = slim_LocInjectVehicleDataRequest(&locEngReq);
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

  if (NULL != pLocSlimUserData)
  {
    os_MemFree((void **)&pLocSlimUserData);
  }

  if(NULL != pResp)
  {
    pResp->resp.result = QMI_RESULT_FAILURE_V01;
  }
  if (0 != error_code)
  {
    LOC_MSG_ERROR("loc_slim_inject_vehicle_sensor_data_req: error: %u",
                  error_code, 0, 0);
  }
  return FALSE;
}

/**
 * called from LOC-MW task in response to a QMI command to notify
 * the sensor control configuration to SLIM.
 *
 * @param p_req  QMI request
 */
void loc_slim_set_sensor_control_config_req(const qmiLocSetSensorControlConfigReqMsgT_v02 *p_req)
{

  // Pass request also to SLIM to notify sensor provider configuration.
  // Return value is ignored since SDP is the one actually handling this
  // request.
  slim_LocInjectSensorControlConfigRequest(p_req);
}

