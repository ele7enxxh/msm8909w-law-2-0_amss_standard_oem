/*============================================================================
  @file sns_sam_prov_sam.c

  @brief
  Implementation of the SAM Provider API for the SAM service.

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_common.h"
#include "qmi_idl_lib.h"
#include "sns_debug_str.h"
#include "fixed_point.h"
#include "sns_queue.h"
#include "sns_sam_ped_v01.h"
#include "sns_sam_prov_api.h"
#include "sns_sam_prov_sam.h"

/*============================================================================
  Forward Declaration
  ===========================================================================*/

STATIC sns_sam_prov_api sns_sam_prov_sam_api;

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Get the minimum batch period from sensorReq
 *
 * @param[i] sensorReq Sensor Request to check
 *
 * @return Minimum Batch period found
 */
STATIC q16_t
sns_sam_prov_get_min_batch_period( sns_sam_sensor_req const *sensorReq )
{
  uint8_t i;
  q16_t minBatch = FX_MAX_Q16;

  for( i = 0; i < SNS_SAM_MAX_SHARED_STREAM; i++ )
  {
    if( NULL != sensorReq->algoInst[i] && sensorReq->batchPeriod[i] < minBatch )
    {
      minBatch = sensorReq->batchPeriod[i];
    }
  }
  return minBatch;
}

/* See sns_sam_prov_handle_req */
STATIC sns_sam_err
sns_sam_prov_sam_handle_req( sns_sam_prov_callback const *cb,
    sns_sam_enable_req *enableReqMsg )
{
  UNREFERENCED_PARAMETER(cb);
  UNREFERENCED_PARAMETER(enableReqMsg);
  // Nothing to do here for SAM algorithms
  return SAM_ENONE;
}

/**
 * Send a batch request for the supplied sensor request.
 *
 * @param sensorRequest An active sensor request
 *
 * @return SAM_ENONE
 *         SAM_EFAILED There was a error when sending the request
 */
STATIC sns_sam_err
sns_sam_send_batch_req( sns_sam_prov_callback const *cb,
    sns_sam_sensor_req *sensorRequest )
{
  sns_sam_err err;
  struct sns_sam_msg reqMsg;
  sns_sam_ped_batch_req_msg_v01 batchReq;

  reqMsg.msg.bufSize = sizeof(sns_sam_ped_batch_req_msg_v01);
  reqMsg.msg.buf = (intptr_t)&batchReq;
  reqMsg.msgID = SNS_SAM_PED_BATCH_REQ_V01;
  reqMsg.sensorUID = sensorRequest->sensor->sensorUID;

  SNS_OS_MEMZERO( (void*)&batchReq, reqMsg.msg.bufSize );
  batchReq.instance_id = sensorRequest->instanceID;
  batchReq.req_type_valid = false;
  batchReq.batch_period = sns_sam_prov_get_min_batch_period( sensorRequest );
  if( FX_MAX_Q16 == batchReq.batch_period )
  {
    // PEND: If any client to this dependent stream is in bigImage,
    // use SNS_SAM_BATCH_ISLAND instead
    batchReq.batch_period = SNS_SAM_BATCH_INTERNAL_V01;
  }

  SNS_PRINTF_STRING_MEDIUM_2( samModule, "Sending batch request to %x (IID: %i)",
      *reqMsg.sensorUID, batchReq.instance_id );
  err = cb->sns_sam_prov_cb_send_req( sensorRequest, &reqMsg );
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "Error sending batch request %i", err );
    return SAM_EFAILED;
  }

  return SAM_ENONE;
}

/**
 * Handle an enable response message received from some SAM algorithm.
 *
 * @param[i] respMsg Response message to process
 */
STATIC void
sns_sam_handle_sam_enable_resp( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *respMsg )
{
  sns_sam_ped_enable_resp_msg_v01 *resp =
    (sns_sam_ped_enable_resp_msg_v01*)respMsg->msg.buf;

  if( SNS_RESULT_SUCCESS_V01 != resp->resp.sns_result_t )
  {
    SNS_PRINTF_STRING_ERROR_3( samModule,
        "Received error response: %i, suid: %x msg: %i",
        resp->resp.sns_err_t, *respMsg->sensorUID, respMsg->msgID );
    cb->sns_sam_prov_stop_stream( cb->sensorReq );
  }
  else
  {
    sns_sam_sensor_req *sensorReq = (sns_sam_sensor_req*)cb->sensorReq;
    // AMD response message is smaller than all others
    sensorReq->instanceID =
      ( sizeof(sns_sam_ped_enable_resp_msg_v01) > respMsg->msg.bufSize )
        ? resp->instance_id_valid : resp->instance_id;

    SNS_PRINTF_STRING_LOW_2( samModule, "Update instance ID for: %i, to: %x",
        sensorReq->instanceID, *sensorReq->sensor->sensorUID );

    if( sensorReq->deferredBatchReq )
    {
      sns_sam_send_batch_req( cb, sensorReq );
      sensorReq->deferredBatchReq = false;
      SNS_PRINTF_STRING_MEDIUM_0( samModule, "Sending deferred batch request" );
    }
  }
}

/**
 * Handle the SAM batch response message.  If an in-progress flush was
 * waiting on this message, send all appropriate indication/response messages.
 *
 * @param[i] respMsg Response message to process
 */
STATIC void
sns_sam_handle_sam_batch_resp( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *respMsg )
{
  sns_sam_ped_batch_resp_msg_v01 *resp =
    (sns_sam_ped_batch_resp_msg_v01*)respMsg->msg.buf;
  sns_sam_sensor_req *sensorReq = (sns_sam_sensor_req*)cb->sensorReq;

  if( SNS_RESULT_SUCCESS_V01 != resp->resp.sns_result_t )
  {
    SNS_PRINTF_STRING_ERROR_3( samModule,
        "Received batch error response: %i, suid: %x msg: %i",
        resp->resp.sns_err_t, *respMsg->sensorUID, respMsg->msgID );
    if( SENSOR1_ENOTALLOWED == resp->resp.sns_err_t )
    {
      cb->sns_sam_prov_stop_stream( cb->sensorReq );
    }
  }

  sensorReq->flushInProgress = false;
  cb->sns_sam_prov_get_data_complete( cb->sensorReq );
}

/**
 * Handle an attribute response message received from another SAM service
 *
 * @param[i] respMsg Response message to process
 */
STATIC void
sns_sam_handle_sam_attr_resp( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *respMsg )
{
  sns_sam_get_algo_attrib_resp_msg_v01 *resp =
    (sns_sam_get_algo_attrib_resp_msg_v01*)respMsg->msg.buf;

  if( SNS_RESULT_SUCCESS_V01 == resp->resp.sns_result_t )
  {
    sns_sam_sensor *sensor = cb->sns_sam_prov_get_sensor( resp->sensorUID );
    sensor->provAPI = &sns_sam_prov_sam_api;
    sensor->sensorReq.provAPI = sensor->provAPI;

    sensor->isAvailable = true;
    sensor->attributes[ SAM_ALGO_ATTR_POWER ].attrValue =
      resp->power;
    sensor->attributes[ SAM_ALGO_ATTR_POWER ].supported = true;

    sensor->attributes[ SAM_ALGO_ATTR_MIN_REPORT ].supported = true;
    sensor->attributes[ SAM_ALGO_ATTR_MIN_REPORT ].attrValue =
      resp->min_report_rate;

    sensor->attributes[ SAM_ALGO_ATTR_MAX_REPORT ].attrValue =
      resp->max_report_rate;
    sensor->attributes[ SAM_ALGO_ATTR_MAX_REPORT ].supported = true;

    sensor->attributes[ SAM_ALGO_ATTR_MIN_SAMPLE ].supported = true;
    sensor->attributes[ SAM_ALGO_ATTR_MIN_SAMPLE ].attrValue =
      resp->min_sample_rate;

    sensor->attributes[ SAM_ALGO_ATTR_MAX_SAMPLE ].attrValue =
      resp->max_sample_rate;
    sensor->attributes[ SAM_ALGO_ATTR_MAX_SAMPLE ].supported = true;

    sensor->attributes[ SAM_ALGO_ATTR_REVISION ].attrValue =
      resp->algorithm_revision;
    sensor->attributes[ SAM_ALGO_ATTR_REVISION ].supported = true;

    sensor->attributes[ SAM_ALGO_ATTR_MAX_BATCH ].attrValue =
      resp->max_batch_size;
    sensor->attributes[ SAM_ALGO_ATTR_MAX_BATCH ].supported = true;

    sensor->sensorLocation = resp->proc_type;

    sensor->attributes[ SAM_ALGO_ATTR_REPORT_MODE ].attrValue =
      resp->supported_reporting_modes;
    sensor->attributes[ SAM_ALGO_ATTR_REPORT_MODE ].supported = true;

    cb->sns_sam_prov_sensor_available( sensor );
  }
  else
  {
    SNS_PRINTF_STRING_ERROR_2( samModule,
        "Error in SAM attribute response %i %i",
        resp->resp.sns_result_t, resp->resp.sns_err_t );
  }
}

/**
 * Handle the SAM update response message.
 *
 * @param[i] respMsg Response message to process
 */
STATIC void
sns_sam_handle_sam_update_resp( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *respMsg )
{
  sns_sam_ped_reset_resp_msg_v01 *resp =
    (sns_sam_ped_reset_resp_msg_v01*)respMsg->msg.buf;

  if( SNS_RESULT_SUCCESS_V01 != resp->resp.sns_result_t )
  {
    SNS_PRINTF_STRING_ERROR_3( samModule,
        "Received update error response: %i, suid: %x msg: %i",
        resp->resp.sns_err_t, *respMsg->sensorUID, respMsg->msgID );
  }
}

/* See sns_sam_prov_handle_resp */
STATIC sns_sam_err
sns_sam_prov_sam_handle_resp( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *respMsg )
{
  SNS_PRINTF_STRING_LOW_1( samModule,
    "sns_sam_handle_sam_resp %i", respMsg->msgID );

  if( SNS_SAM_PED_ENABLE_RESP_V01 == respMsg->msgID )
  {
    sns_sam_handle_sam_enable_resp( cb, respMsg );
  }
  else if( SNS_SAM_PED_BATCH_RESP_V01 == respMsg->msgID )
  {
    sns_sam_handle_sam_batch_resp( cb, respMsg );
  }
  else if( SNS_SAM_PED_GET_ATTRIBUTES_RESP_V01 == respMsg->msgID )
  {
    sns_sam_handle_sam_attr_resp( cb, respMsg );
  }
  else if( SNS_SAM_PED_RESET_RESP_V01 == respMsg->msgID )
  {
    sns_sam_handle_sam_update_resp( cb, respMsg );
  }
  else
  {
    SNS_PRINTF_STRING_ERROR_2( samModule,
        "Received unknown SAM message type; sensor: %x, msg: %i",
        *respMsg->sensorUID, respMsg->msgID );
    return SAM_ENOT_SUPPORTED;
  }

  return SAM_ENONE;
}

/* See sns_sam_prov_handle_ind */
STATIC SNS_SAM_UIMAGE_CODE sns_sam_err
sns_sam_prov_sam_handle_ind( sns_sam_prov_callback const *cb,
    sns_sam_ind_msg const *indMsg )
{
  if( SNS_SAM_PED_ERROR_IND_V01 == indMsg->msgID )
  {
    cb->sns_sam_prov_uimage_exit();
    cb->sns_sam_prov_stop_stream( cb->sensorReq );
    return SAM_ESTATE;
  }

  return SAM_ENONE;
}

/* See sns_sam_prov_service */
STATIC qmi_idl_service_object_type
sns_sam_prov_sam_service( sns_sam_prov_callback const *cb )
{
  // List of SAM service objects are returned from the algorithms
  return NULL;
}

/* See sns_sam_prov_sensor_list.
 * For SAM, we send an attribute request message to the service associated
 * with the cb->sensorReq.
 */
STATIC sns_sam_err
sns_sam_prov_sam_sensor_list( sns_sam_prov_callback const *cb )
{
  sns_sam_err rv = SAM_ENONE;
  struct sns_sam_msg reqMsg;
  sns_sam_get_algo_attrib_req_msg_v01 attrReq;

  SNS_PRINTF_STRING_LOW_1( samModule, "Send attribute request for %x",
      *cb->sensorReq->sensorUID );

  reqMsg.msg.buf = (intptr_t)&attrReq;
  reqMsg.msg.bufSize = sizeof(sns_sam_get_algo_attrib_req_msg_v01);
  reqMsg.msgID = SNS_SAM_PED_GET_ATTRIBUTES_REQ_V01;
  reqMsg.sensorUID = cb->sensorReq->sensorUID;
  SNS_OS_MEMZERO( (void*)reqMsg.msg.buf, reqMsg.msg.bufSize );

  rv = cb->sns_sam_prov_cb_send_req( cb->sensorReq, &reqMsg );
  if( SAM_ENONE != rv )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "Error sending attr request %i", rv );
    rv = SAM_EFAILED;
  }

  return rv;
}

/* See sns_sam_prov_set_batch_period */
STATIC sns_sam_err
sns_sam_prov_sam_set_batch_period( sns_sam_prov_callback const *cb )
{
  sns_sam_sensor_req *sensorReq = (sns_sam_sensor_req*)cb->sensorReq;

  if( UINT16_MAX == sensorReq->instanceID )
  {
    sensorReq->deferredBatchReq = true;
    SNS_PRINTF_STRING_MEDIUM_0( samModule,
        "Instance ID uninitialized. Deferring batch request" );
    return SAM_ENONE;
  }

  return sns_sam_send_batch_req( cb, sensorReq );
}

/* See sns_sam_prov_get_data */
STATIC sns_sam_err
sns_sam_prov_sam_get_data( sns_sam_prov_callback const *cb )
{
  sns_sam_sensor_req *sensorReq = (sns_sam_sensor_req*)cb->sensorReq;
  sns_sam_err rv = SAM_ENONE;

  if( 0 == sns_sam_prov_get_min_batch_period( cb->sensorReq ) )
  {
    SNS_PRINTF_STRING_MEDIUM_0( samModule,
        "Attempt to flush a non-batching SAM stream" );
    rv = SAM_EFAILED;
  }
  else if( UINT16_MAX == sensorReq->instanceID )
  {
    sensorReq->deferredBatchReq = true;
    SNS_PRINTF_STRING_MEDIUM_0( samModule,
        "Instance ID uninitialized. Deferring batch request" );
  }
  else
  {
    rv = sns_sam_send_batch_req( cb, sensorReq );
  }

  return rv;
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

/**
* QMI_LOC Provider API
*
* See sns_sam_prov_api.
*/
STATIC sns_sam_prov_api sns_sam_prov_sam_api SNS_SAM_UIMAGE_DATA =
{
  .structSize = sizeof(sns_sam_prov_api),
  .sns_sam_prov_handle_req = &sns_sam_prov_sam_handle_req,
  .sns_sam_prov_handle_resp = &sns_sam_prov_sam_handle_resp,
  .sns_sam_prov_handle_ind = &sns_sam_prov_sam_handle_ind,
  .sns_sam_prov_service = &sns_sam_prov_sam_service,
  .sns_sam_prov_sensor_list = &sns_sam_prov_sam_sensor_list,
  .sns_sam_prov_set_batch_period = &sns_sam_prov_sam_set_batch_period,
  .sns_sam_prov_get_data = &sns_sam_prov_sam_get_data
};

sns_sam_err sns_sam_prov_sam_register(
      sns_sam_prov_api **provAPI )
{
  *provAPI = &sns_sam_prov_sam_api;
  return SAM_ENONE;
}
