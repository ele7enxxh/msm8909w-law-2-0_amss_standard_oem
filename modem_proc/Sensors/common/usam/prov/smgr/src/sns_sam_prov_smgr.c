/*============================================================================
  @file sns_sam_prov_smgr.c

  @brief
  Implementation of the SAM Provider API for the SMGR service.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
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
#include "sns_smgr_api_v01.h"
#include "sns_sam_prov_api.h"
#include "sns_sam_prov_smgr.h"

/*============================================================================
  Forward Declaration
  ===========================================================================*/

STATIC sns_sam_prov_api sns_sam_prov_smgr_api;

void sns_sam_gen_gated_sensors( sns_sam_sensor const *parentSensor );

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_sam_prov_handle_req */
STATIC sns_sam_err
sns_sam_prov_smgr_handle_req( sns_sam_prov_callback const *cb,
    sns_sam_enable_req *enableReqMsg )
{
  UNREFERENCED_PARAMETER(cb);
  UNREFERENCED_PARAMETER(enableReqMsg);
  // SMGR does not need to perform preprocessing for enable requests
  return SAM_ENONE;
}

/**
 * Process a single sensor info response message from SMGR.
 *
 * @param[i] respMsg Message to be processed
 *
 * @return SAM_ENONE
 *         SAM_ESTATE Received error response message
 */
STATIC sns_sam_err
sns_sam_smgr_handle_single_resp( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *respMsg )
{
  sns_smgr_single_sensor_info_resp_msg_v01 *resp;
  uint32_t i;
  sns_sam_err err = SAM_ENONE;

  resp = (sns_smgr_single_sensor_info_resp_msg_v01*)respMsg->msg.buf;

  if( SNS_RESULT_SUCCESS_V01 == resp->Resp.sns_result_t )
  {
    for( i = 0; i < resp->SensorInfo.data_type_info_len; i++ )
    {
      sns_sam_sensor *sensor = cb->sns_sam_prov_get_sensor( resp->suid[ i ] );

      SNS_PRINTF_STRING_LOW_3( samModule,
          "Created new sensor; SMGR ID %i, type %i, UID %x",
          resp->SensorInfo.data_type_info[ i ].SensorID, i, resp->suid[ i ] );

      *sensor->sensorUID = resp->suid[ i ];
      sensor->serviceObj = SNS_SMGR_SVC_get_service_object_v01();
      sensor->sensorReq = (sns_sam_sensor_req)*cb->sensorReq;
      sensor->provAPI = &sns_sam_prov_smgr_api;
      sensor->sensorReq.provAPI = sensor->provAPI;
      sensor->isAvailable = true;
      sensor->gatingType = SNS_SAM_SENSOR_STREAM;

      sensor->attributes[ SAM_ALGO_ATTR_POWER ].attrValue
        = resp->SensorInfo.data_type_info[ i ].MaxPower;
      sensor->attributes[ SAM_ALGO_ATTR_POWER ].supported = true;

      sensor->attributes[ SAM_ALGO_ATTR_MIN_REPORT ].supported = false;

      sensor->attributes[ SAM_ALGO_ATTR_MAX_REPORT ].attrValue =
        resp->SensorInfo.data_type_info[ i ].MaxSampleRate;
      sensor->attributes[ SAM_ALGO_ATTR_MAX_REPORT ].supported = true;

      sensor->attributes[ SAM_ALGO_ATTR_MIN_SAMPLE ].supported = false;

      sensor->attributes[ SAM_ALGO_ATTR_MAX_SAMPLE ].attrValue =
        FX_FLTTOFIX_Q16(resp->SensorInfo.data_type_info[ i ].MaxSampleRate);
      sensor->attributes[ SAM_ALGO_ATTR_MAX_SAMPLE ].supported = true;

      sensor->attributes[ SAM_ALGO_ATTR_REVISION ].attrValue =
        resp->SensorInfo.data_type_info[ i ].Version;
      sensor->attributes[ SAM_ALGO_ATTR_REVISION ].supported = true;

      if( resp->num_buffered_reports_valid &&
          i < resp->num_buffered_reports_len )
      {
        sensor->attributes[ SAM_ALGO_ATTR_MAX_BATCH ].attrValue =
          resp->num_buffered_reports[ i ];
        sensor->attributes[ SAM_ALGO_ATTR_MAX_BATCH ].supported = true;
      }

      cb->sns_sam_prov_sensor_available( sensor );
      sns_sam_gen_gated_sensors( sensor );
    }
  }
  else
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
        "Error in SMGR single info resp: %i",
        resp->Resp.sns_err_t );
    err = SAM_ESTATE;
  }

  return err;
}

/**
 * Send a single sensor info request to SMGR.
 *
 * @param[i] sensorID Sensor found in the all sensor info response message
 *
 * @return SAM_ENONE
 *         SAM_EFAILED Unable to send request message
 */
STATIC sns_sam_err
sns_sam_send_smgr_single_req( sns_sam_prov_callback const *cb,
    uint8_t sensorID )
{
  sns_sam_err err;
  struct sns_sam_msg reqMsg;
  sns_smgr_single_sensor_info_req_msg_v01 singleReq;

  reqMsg.msg.buf = (intptr_t)&singleReq;
  reqMsg.msg.bufSize = sizeof(sns_smgr_single_sensor_info_req_msg_v01);
  reqMsg.sensorUID = NULL;
  reqMsg.msgID = SNS_SMGR_SINGLE_SENSOR_INFO_REQ_V01;

  SNS_OS_MEMZERO( (void*)reqMsg.msg.buf, reqMsg.msg.bufSize );
  singleReq.SensorID = sensorID;

  err = cb->sns_sam_prov_cb_send_req( cb->sensorReq, &reqMsg );
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "Error sending request message %i", err );
    err = SAM_EFAILED;
  }

  return err;
}

/**
 * Process the all sensor info response message from SMGR.
 *
 * @param[i] respMsg Message to be processed
 *
 * @return SAM_ENONE
 *         SAM_EFAILED Unable to send request message
 *         SAM_ESTATE Received error response message
 */
STATIC sns_sam_err
sns_sam_smgr_handle_all_resp( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *respMsg )
{
  sns_sam_err err = SAM_ENONE;
  uint32_t i;
  sns_smgr_all_sensor_info_resp_msg_v01 *resp =
    (sns_smgr_all_sensor_info_resp_msg_v01*)respMsg->msg.buf;

  if( SNS_RESULT_SUCCESS_V01 == resp->Resp.sns_result_t )
  {
    for( i = 0; i < resp->SensorInfo_len; i++ )
    {
      err = sns_sam_send_smgr_single_req( cb, resp->SensorInfo[ i ].SensorID );

      if( SAM_ENONE != err )
      {
        SNS_PRINTF_STRING_ERROR_2( samModule,
          "Unable to send SMGR single sensor info req for sensor %i (%i)",
          resp->SensorInfo[ i ].SensorID, err );
        err = SAM_EFAILED;
      }
    }
  }
  else
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
        "Error in SMGR all sensor info response: %i",
        resp->Resp.sns_err_t );
    err = SAM_ESTATE;
  }

  return err;
}

/**
 * Handle a buffered response message from SMGR or SMGRI.
 *
 * @param[i] respMsg Message to be processed
 *
 * @return SAM_ENONE
 *         SAM_ESTATE Received error response message
 */
sns_sam_err
sns_sam_smgr_handle_buffering_resp( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *respMsg )
{
  sns_sam_err rv = SAM_ENONE;
  sns_sam_sensor_req *sensorReq = (sns_sam_sensor_req*)cb->sensorReq;
  sns_smgr_buffering_resp_msg_v01 *resp =
    (sns_smgr_buffering_resp_msg_v01*)respMsg->msg.buf;

  if( SNS_RESULT_SUCCESS_V01 != resp->Resp.sns_result_t )
  {
    if( SNS_SMGR_RESPONSE_NAK_CANCELLED_V01 == resp->AckNak )
    {
      SNS_PRINTF_STRING_MEDIUM_1( samModule,
        "Received SMGR warning 'NAK_CANCELLED' response; suid: %x",
        *respMsg->sensorUID );
    }
    else
    {
      SNS_PRINTF_STRING_ERROR_3( samModule,
          "Received SMGR error response: %i, suid: %x msg: %i",
          resp->Resp.sns_err_t, *sensorReq->sensorUID, respMsg->msgID );
      SNS_PRINTF_STRING_ERROR_2( samModule,
          "AckNak %i, reason: %i", resp->AckNak, resp->ReasonPair[0].Reason );

      cb->sns_sam_prov_stop_stream( cb->sensorReq );
    }
    rv = SAM_ENONE;
  }
  else
  {
    SNS_PRINTF_STRING_LOW_2( samModule,
        "Received SMGR success response; suid: %x msg: %i",
        *sensorReq->sensorUID, respMsg->msgID );
  }

  sensorReq->flushInProgress = false;
  cb->sns_sam_prov_get_data_complete( cb->sensorReq );

  return rv;
}

/* See sns_sam_prov_handle_resp */
STATIC sns_sam_err
sns_sam_prov_smgr_handle_resp( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *respMsg )
{
  sns_sam_err err = SAM_EFAILED;

  SNS_PRINTF_STRING_LOW_1( samModule,
    "sns_sam_handle_smgr_resp %i", respMsg->msgID );

  if( SNS_SMGR_SINGLE_SENSOR_INFO_RESP_V01 == respMsg->msgID )
  {
    err = sns_sam_smgr_handle_single_resp( cb, respMsg );
  }
  else if( SNS_SMGR_ALL_SENSOR_INFO_RESP_V01 == respMsg->msgID )
  {
    err = sns_sam_smgr_handle_all_resp( cb, respMsg );
  }
  else if( SNS_SMGR_BUFFERING_RESP_V01 == respMsg->msgID ||
           SNS_SMGR_REPORT_RESP_V01 == respMsg->msgID )
  {
    err = sns_sam_smgr_handle_buffering_resp( cb, respMsg );
  }
  else
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "Unknown SMGR message ID: %i", respMsg->msgID );
  }

  return SAM_ENONE;
}

/* See sns_sam_prov_handle_ind */
STATIC SNS_SAM_UIMAGE_CODE sns_sam_err
sns_sam_prov_smgr_handle_ind( sns_sam_prov_callback const *cb,
    sns_sam_ind_msg const *indMsg )
{
  if( SNS_SMGR_REPORT_ERROR_IND_V01 == indMsg->msgID )
  {
    cb->sns_sam_prov_uimage_exit();
    cb->sns_sam_prov_stop_stream( cb->sensorReq );
    return SAM_ESTATE;
  }

  return SAM_ENONE;
}

/* See sns_sam_prov_service */
STATIC qmi_idl_service_object_type
sns_sam_prov_smgr_service( sns_sam_prov_callback const *cb )
{
  return SNS_SMGR_SVC_get_service_object_v01();
}

/* See sns_sam_prov_sensor_list */
STATIC sns_sam_err
sns_sam_prov_smgr_sensor_list( sns_sam_prov_callback const *cb )
{
  sns_sam_err err = SAM_EFAILED;
  struct sns_sam_msg reqMsg;
  sns_smgr_all_sensor_info_req_msg_v01 allInfoReq;

  SNS_PRINTF_STRING_LOW_0( samModule, "sns_sam_smgr_req_all" );

  reqMsg.msg.buf = (intptr_t)&allInfoReq;
  reqMsg.msg.bufSize = sizeof(sns_smgr_all_sensor_info_req_msg_v01);
  SNS_OS_MEMZERO( (void*)reqMsg.msg.buf, reqMsg.msg.bufSize );
  reqMsg.sensorUID = NULL;
  reqMsg.msgID = SNS_SMGR_ALL_SENSOR_INFO_REQ_V01;

  err = cb->sns_sam_prov_cb_send_req( cb->sensorReq, &reqMsg );
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "Unable to send SMGR all sensor req %i", err );
    err = SAM_EFAILED;
  }

  return err;
}

/* See sns_sam_prov_get_data */
STATIC sns_sam_err
sns_sam_prov_smgr_get_data( sns_sam_prov_callback const *cb )
{
  sns_sam_err err;
  struct sns_sam_msg reqMsg;
  sns_smgr_buffering_req_msg_v01 flushReq;

  reqMsg.msg.bufSize = sizeof(sns_smgr_buffering_req_msg_v01);
  reqMsg.msg.buf = (intptr_t)&flushReq;
  reqMsg.msgID = SNS_SMGR_BUFFERING_REQ_V01;
  reqMsg.sensorUID = cb->sensorReq->sensor->sensorUID;

  SNS_OS_MEMZERO( (void*)reqMsg.msg.buf, reqMsg.msg.bufSize );
  flushReq.Action = SNS_SMGR_BUFFERING_ACTION_FLUSH_V01;

  err = cb->sns_sam_prov_cb_send_req( cb->sensorReq, &reqMsg );
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "Unable to send SMGR flush req %i", err );
    err = SAM_EFAILED;
  }

  return err;
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

/**
* QMI_LOC Provider API
*
* See sns_sam_prov_api.
*/
STATIC sns_sam_prov_api sns_sam_prov_smgr_api SNS_SAM_UIMAGE_DATA =
{
  .structSize = sizeof(sns_sam_prov_api),
  .sns_sam_prov_handle_req = &sns_sam_prov_smgr_handle_req,
  .sns_sam_prov_handle_resp = &sns_sam_prov_smgr_handle_resp,
  .sns_sam_prov_handle_ind = &sns_sam_prov_smgr_handle_ind,
  .sns_sam_prov_service = &sns_sam_prov_smgr_service,
  .sns_sam_prov_sensor_list = &sns_sam_prov_smgr_sensor_list,
  .sns_sam_prov_set_batch_period = NULL,
  .sns_sam_prov_get_data = &sns_sam_prov_smgr_get_data
};

sns_sam_err sns_sam_prov_smgr_register(
      sns_sam_prov_api **provAPI )
{
  *provAPI = &sns_sam_prov_smgr_api;
  return SAM_ENONE;
}
