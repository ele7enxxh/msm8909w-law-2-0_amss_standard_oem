/*============================================================================
  @file sns_sam_prov_smgri.c

  @brief
  Implementation of the SAM Provider API for the SMGRI service.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
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
#include "sns_smgr_internal_api_v02.h"
#include "sns_sam_prov_api.h"
#include "sns_sam_prov_smgri.h"

/*============================================================================
  Forward Declaration
  ===========================================================================*/

STATIC sns_sam_prov_api sns_sam_prov_smgri_api;

sns_sam_err sns_sam_smgr_handle_buffering_resp(
    sns_sam_prov_callback const *cb, sns_sam_resp_msg const *respMsg );

void sns_sam_gen_gated_sensors( sns_sam_sensor const *parentSensor );

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_sam_prov_handle_req */
STATIC sns_sam_err
sns_sam_prov_smgri_handle_req( sns_sam_prov_callback const *cb,
    sns_sam_enable_req *enableReqMsg )
{
  // SMGRI does not need to perform an pre-processing for request messages

  return SAM_ENONE;
}

/**
 * Process the events query response message from SMGR.
 *
 * @param[i] respMsg Message to be processed
 *
 * @return SAM_ENONE
 *         SAM_ESTATE Received error response message
 */
STATIC sns_sam_err
sns_sam_smgri_handle_query_resp( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *respMsg )
{
  sns_sam_err err = SAM_ENONE;
  uint32_t i;
  sns_smgr_sensor_events_query_resp_msg_v02 *resp =
    (sns_smgr_sensor_events_query_resp_msg_v02*)respMsg->msg.buf;

  if( SNS_RESULT_SUCCESS_V01 == resp->resp.sns_result_t &&
      resp->sensor_events_valid )
  {
    for( i = 0; i < resp->sensor_events_len; i++ )
    {
      sns_sam_sensor *sensor = cb->sns_sam_prov_get_sensor(
          resp->sensor_events[ i ] );

      SNS_PRINTF_STRING_LOW_1( samModule,
        "Created new sensor; UID %x", resp->sensor_events[ i ] );

      *sensor->sensorUID = resp->sensor_events[ i ];
      sensor->serviceObj = SNS_SMGR_INTERNAL_SVC_get_service_object_v02();
      sensor->sensorReq = *cb->sensorReq;
      sensor->provAPI = &sns_sam_prov_smgri_api;
      sensor->sensorReq.provAPI = sensor->provAPI;
      sensor->isAvailable = true;
      sensor->gatingType = SNS_SAM_SENSOR_EVENT;

      cb->sns_sam_prov_sensor_available( sensor );
      sns_sam_gen_gated_sensors( sensor );
    }
  }
  else
  {
    err = SAM_ESTATE;
    SNS_PRINTF_STRING_ERROR_1( samModule,
        "Error in SMGR all sensor info response: %i",
        resp->resp.sns_err_t );
  }

  return err;
}

/**
 * Handle a sensor event response message.
 *
 * @param[i] respMsg Message to be processed
 *
 * @return SAM_ENONE
 *         SAM_ESTATE Received error response message
 */
STATIC sns_sam_err
sns_sam_smgri_handle_event_response( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *respMsg )
{
  sns_sam_err rv = SAM_ENONE;
  sns_smgr_sensor_event_resp_msg_v02 *resp =
    (sns_smgr_sensor_event_resp_msg_v02*)respMsg->msg.buf;

  if( SNS_RESULT_SUCCESS_V01 != resp->resp.sns_result_t )
  {
    SNS_PRINTF_STRING_ERROR_3( samModule,
        "Received SMGRI error response: %i, suid: %x msg: %i",
        resp->resp.sns_err_t, *respMsg->sensorUID, respMsg->msgID );

    cb->sns_sam_prov_stop_stream( cb->sensorReq );
    rv = SAM_ESTATE;
  }
  else
  {
    SNS_PRINTF_STRING_LOW_2( samModule,
        "Received SMGRI success response; suid: %lx msg: %i",
        *respMsg->sensorUID, respMsg->msgID );
  }

  return rv;
}

/* See sns_sam_prov_handle_resp */
STATIC sns_sam_err
sns_sam_prov_smgri_handle_resp( sns_sam_prov_callback const *cb,
    sns_sam_resp_msg const *respMsg )
{
  sns_sam_err err = SAM_EFAILED;

  SNS_PRINTF_STRING_LOW_1( samModule,
    "Handle SMGRI response %i", respMsg->msgID );

  if( SNS_SMGR_SENSOR_EVENTS_QUERY_RESP_V02 == respMsg->msgID )
  {
    err = sns_sam_smgri_handle_query_resp( cb, respMsg );
  }
  else if( SNS_SMGR_SENSOR_EVENT_RESP_V02 == respMsg->msgID )
  {
    err = sns_sam_smgri_handle_event_response( cb, respMsg );
  }
  else if( SNS_SMGR_EVENT_GATED_BUFFERING_RESP_V02 == respMsg->msgID )
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

/* See sns_sam_prov_service */
STATIC qmi_idl_service_object_type
sns_sam_prov_smgri_service( sns_sam_prov_callback const *cb )
{
  return SNS_SMGR_INTERNAL_SVC_get_service_object_v02();
}

/* See sns_sam_prov_sensor_list */
STATIC sns_sam_err
sns_sam_prov_smgri_sensor_list( sns_sam_prov_callback const *cb )
{
  sns_sam_err err;
  struct sns_sam_msg reqMsg;
  sns_smgr_sensor_events_query_req_msg_v02 eventReq;

  SNS_PRINTF_STRING_LOW_0( samModule, "sns_sam_prov_smgri_sensor_list" );

  reqMsg.msg.buf = (intptr_t)&eventReq;
  reqMsg.msg.bufSize = sizeof(sns_smgr_sensor_events_query_req_msg_v02);
  reqMsg.msgID = SNS_SMGR_SENSOR_EVENTS_QUERY_REQ_V02;
  reqMsg.sensorUID = NULL;
  SNS_OS_MEMZERO( (void*)reqMsg.msg.buf, reqMsg.msg.bufSize );

  err = cb->sns_sam_prov_cb_send_req( cb->sensorReq, &reqMsg );
  if( SAM_ENONE != err )
  {
    SNS_PRINTF_STRING_ERROR_1( samModule,
      "Unable to send SMGRI event query %i", err );
    return SAM_EFAILED;
  }

  return SAM_ENONE;
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

/**
* QMI_LOC Provider API
*
* See sns_sam_prov_api.
*/
STATIC sns_sam_prov_api sns_sam_prov_smgri_api SNS_SAM_UIMAGE_DATA =
{
  .structSize = sizeof(sns_sam_prov_api),
  .sns_sam_prov_handle_req = &sns_sam_prov_smgri_handle_req,
  .sns_sam_prov_handle_resp = &sns_sam_prov_smgri_handle_resp,
  .sns_sam_prov_handle_ind = NULL, // Do nothing, always pass indication to algorithm
  .sns_sam_prov_service = &sns_sam_prov_smgri_service,
  .sns_sam_prov_sensor_list = &sns_sam_prov_smgri_sensor_list,
  .sns_sam_prov_set_batch_period = NULL,
  .sns_sam_prov_get_data = NULL
};

sns_sam_err sns_sam_prov_smgri_register(
      sns_sam_prov_api **provAPI )
{
  *provAPI = &sns_sam_prov_smgri_api;
  return SAM_ENONE;
}
