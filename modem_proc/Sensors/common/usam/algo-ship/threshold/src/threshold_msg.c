/*============================================================================
  @file threshold_msg.c

  @brief
  Implementation of the threshold service.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <math.h>
#include "sns_log_types.h"
#include "threshold.h"
#include "fixed_point.h"
#include "sns_memmgr.h"
#include "sns_reg_common.h"
#include "sns_sam_algo_utils.h"
#include "sns_reg_api_v02.h"
#include "sns_smgr_api_v01.h"
#include "sns_sam_algo_api.h"
#include "sns_sam_sensor_thresh_v01.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/
#define THRESHOLD_ALGO_REVISION 1

#define THRESHOLD_MAX_BATCH 50

#define THRESHOLD_RESERVED_FIFO_SIZE 100

/*===========================================================================
  Static Function Definitions
  ===========================================================================*/

static void
threshold_mem_const_req( sns_sam_algo_const_buf_size *bufferSizes )
{
  bufferSizes->configDataSize = sizeof(threshold_config_s);
  bufferSizes->persistDataSize = sizeof(threshold_persist_s);
  bufferSizes->inputDataSize = sizeof(threshold_input_s);
  bufferSizes->outputDataSize = sizeof(threshold_output_s);
}

static void
threshold_dep_registry_met( struct sns_sam_sensor_algo const *algo,
  sns_sam_algo_all_dep_met depMetFunc, sns_sam_algo_persist *persistData,
  sns_sam_reg_data const *regData )
{
  UNREFERENCED_PARAMETER(persistData);
  UNREFERENCED_PARAMETER(regData);
  UNREFERENCED_PARAMETER( algo );
}

static void
threshold_dep_sensors_met( struct sns_sam_sensor_algo const *algo,
  sns_sam_algo_all_dep_met depMetFunc, sns_sam_algo_persist *persistData,
  sns_sam_sensor_uid const *sensorUID,
  sns_sam_algo_attr const (*sns_sam_attr_array)[ SAM_ALGO_ATTR_CNT ] )
{
  UNREFERENCED_PARAMETER(sns_sam_attr_array);
  threshold_persist_s *thresholdPersistData = (threshold_persist_s*)persistData->buf;

  switch( *sensorUID )
  {
    case SNS_SMGR_SUID_ACCEL_1_V01:
    case SNS_SMGR_SUID_GYRO_1_V01:
    case SNS_SMGR_SUID_MAG_1_V01:
    case SNS_SMGR_SUID_PRESSURE_1_V01:
    case SNS_SMGR_SUID_PROX_1_V01:
    case SNS_SMGR_SUID_LIGH_1_V01:
      thresholdPersistData->anySensorSUID = sensorUID;
      depMetFunc( algo );
      break;
    default:
      break;
  }
}

static void
threshold_get_attr( sns_sam_algo_persist const *persistData,
  sns_sam_algo_attr attributes[] )
{
  UNREFERENCED_PARAMETER(persistData);
  uint8_t i;
  for( i = 0;
       i < SAM_ALGO_ATTR_CNT && SAM_ALGO_ATTR_CNT != attributes[i].attribute;
       i++ )
  {
    switch( attributes[i].attribute )
    {
    case SAM_ALGO_ATTR_REPORT_MODE:  //async
      attributes[i].attrValue = SNS_SAM_ASYNC_REPORT_V01;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_REVISION:
      attributes[i].attrValue = THRESHOLD_ALGO_REVISION;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_MAX_BATCH:
      attributes[i].attrValue = THRESHOLD_MAX_BATCH;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_MAX_REPORTS_PER_IND:
      attributes[i].attrValue = SNS_SAM_SENSOR_THRESH_MAX_ITEMS_IN_BATCH_V01;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_RESERVED_FIFO_SIZE:
      attributes[i].attrValue = THRESHOLD_RESERVED_FIFO_SIZE;
      attributes[i].supported = true;
      break;

    default:
      attributes[i].supported = false;
      break;
    }
  }
}

static sns_sam_err
threshold_dep_sensors( uint32_t *sensorCount,
  struct sns_sam_algo_dep_sensor dependentSensors[] )
{
  if( *sensorCount >= 5 )
  {
    *sensorCount = 5;
    dependentSensors[0].sensorUID = SNS_SMGR_SUID_ACCEL_1_V01;
    dependentSensors[0].serviceObj = SNS_SMGR_SVC_get_service_object_v01();
    dependentSensors[1].sensorUID = SNS_SMGR_SUID_GYRO_1_V01;
    dependentSensors[1].serviceObj = SNS_SMGR_SVC_get_service_object_v01();
    dependentSensors[2].sensorUID = SNS_SMGR_SUID_MAG_1_V01;
    dependentSensors[2].serviceObj = SNS_SMGR_SVC_get_service_object_v01();
    dependentSensors[3].sensorUID = SNS_SMGR_SUID_PRESSURE_1_V01;
    dependentSensors[3].serviceObj = SNS_SMGR_SVC_get_service_object_v01();
    dependentSensors[4].sensorUID = SNS_SMGR_SUID_PROX_1_V01;
    dependentSensors[4].serviceObj = SNS_SMGR_SVC_get_service_object_v01();

    return SAM_ENONE;
  }
  else
  {
    return SAM_EMAX;
  }
}

static sns_sam_err
threshold_reg_grps( uint32_t *registryGrpCount, sns_sam_reg_group registryGroups[] )
{
  UNREFERENCED_PARAMETER(registryGroups);
  *registryGrpCount = 0;
  return SAM_ENONE;
}

static sns_sam_err
threshold_process_enable_req( sns_sam_enable_req const *enableReq,
  sns_sam_algo_persist *persistData, sns_sam_client_attr *clientAttr,
  sns_sam_algo_config *configData )
{
  UNREFERENCED_PARAMETER(persistData);

  threshold_config_s *thresholdConfig = (threshold_config_s*)configData->buf;
  sns_sam_sensor_thresh_enable_req_msg_v01 *reqMsg =
    (sns_sam_sensor_thresh_enable_req_msg_v01*) enableReq->msg.buf;

  thresholdConfig->sensor_id = reqMsg->sensor_id;
  thresholdConfig->data_type = reqMsg->data_type;
  thresholdConfig->sample_rate = reqMsg->sample_rate;
  if (reqMsg->threshold_type_valid)
  {
    thresholdConfig->threshold_type = reqMsg ->threshold_type;
  }
  if( reqMsg->report_period_valid != false)
  {
    thresholdConfig->report_period = ((reqMsg->report_period)*32768)/1000;
  }
  else
  {
    thresholdConfig->report_period = 0;
  }

  if (reqMsg->sensor_id == SNS_SMGR_ID_PROX_LIGHT_V01)
  {
    thresholdConfig->thresh[0] = reqMsg->threshold[0];
    thresholdConfig->num_axis = 1;
    // Proximity is an event sensor so operated in async mode
    thresholdConfig->report_period = 0;
  }
  else if (reqMsg->sensor_id == SNS_SMGR_ID_PRESSURE_V01)
  {
    thresholdConfig->thresh[0] = reqMsg->threshold[0];
    thresholdConfig->num_axis = 1;
  }
  else if ((reqMsg->sensor_id == SNS_SMGR_ID_ACCEL_V01) ||
           (reqMsg->sensor_id == SNS_SMGR_ID_GYRO_V01)  ||
           (reqMsg->sensor_id == SNS_SMGR_ID_MAG_V01))
  {
    thresholdConfig->thresh[0] = reqMsg->threshold[0];
    thresholdConfig->thresh[1] = reqMsg->threshold[1];
    thresholdConfig->thresh[2] = reqMsg->threshold[2];
    thresholdConfig->num_axis = 3;
  }

  clientAttr->reportPeriod = thresholdConfig->report_period;
  clientAttr->reportMode = SNS_SAM_REPORT_MODE_ASYNC;
  if( reqMsg->notify_suspend_valid )
  {
    clientAttr->notifySuspend.send_indications_during_suspend =
      reqMsg->notify_suspend.send_indications_during_suspend;

    clientAttr->notifySuspend.proc_type = reqMsg->notify_suspend.proc_type;
  }
  else
  {
    clientAttr->notifySuspend.send_indications_during_suspend = false;
  }

  return SAM_ENONE;
}

static sns_sam_err
threshold_gen_req( sns_sam_algo_state const *algoState,
  sns_sam_sensor_uid const *sensorUID, sns_sam_enable_req *enableReqMsg )
{
  threshold_state_s *thresholdState = (threshold_state_s*)algoState->buf;
  sns_sam_sensor_data_info_req sensorReq;
  switch( *sensorUID )
  {
  case SNS_SMGR_SUID_ACCEL_1_V01:
    sensorReq.sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
    break;
  case SNS_SMGR_SUID_GYRO_1_V01:
    sensorReq.sensorDbase[0].sensorId = SNS_SMGR_ID_GYRO_V01;
    break;
  case SNS_SMGR_SUID_MAG_1_V01:
    sensorReq.sensorDbase[0].sensorId = SNS_SMGR_ID_MAG_V01;
    break;
  case SNS_SMGR_SUID_PRESSURE_1_V01:
    sensorReq.sensorDbase[0].sensorId = SNS_SMGR_ID_PRESSURE_V01;
    break;
  case SNS_SMGR_SUID_PROX_1_V01:
    sensorReq.sensorDbase[0].sensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
    break;
  default:
  return SAM_ESTATE;
  }

  if( thresholdState->config.sensor_id != sensorReq.sensorDbase[0].sensorId)
  {
    return SAM_ESTATE;
  }

  sensorReq.sensorCount = 1;
  sensorReq.sampleRate = thresholdState->config.sample_rate;
  sensorReq.sensorDbase[0].dataType = thresholdState->config.data_type;
  sensorReq.sensorDbase[0].sampleQual = SNS_SAM_DEFAULT_SAMPLE_QUALITY;
  sensorReq.sensorDbase[0].buffered_sampleRate = thresholdState->config.sample_rate;
  sensorReq.srcUID = SNS_SAM_THRESHOLD_SUID_V01;
  sensorReq.reportRate = thresholdState->config.sample_rate;

  return sns_sam_util_gen_smgr_buffer_req( &sensorReq, enableReqMsg );
}

static sns_sam_err
threshold_gen_ind( sns_sam_algo_callback const *cbFunc,
  sns_sam_algo_output const *outputData, uint8_t extInstanceID,
  sns_sam_ind_msg *indMsg)
{
  threshold_output_s *output = (threshold_output_s*)outputData->data;
  sns_sam_sensor_thresh_report_ind_msg_v01 *ind_msg =
    (sns_sam_sensor_thresh_report_ind_msg_v01*)indMsg->msg.buf;

  ind_msg->timestamp = output->timestamp;
  ind_msg->instance_id = extInstanceID;
  SNS_OS_MEMSCPY( ind_msg->sample_value,sizeof(ind_msg->sample_value), output->sample,
    sizeof(ind_msg->sample_value) );

  return SAM_ENONE;
}

STATIC sns_sam_err threshold_gen_batch_ind(
    sns_sam_algo_callback const *cbFunc,
    uint8_t extInstanceID,
    sns_sam_batch_ind_type indType,
    sns_sam_ind_msg *indMsg)
{
  sns_sam_sensor_thresh_batch_ind_msg_v01 *batchIndMsg =
      (sns_sam_sensor_thresh_batch_ind_msg_v01 *) indMsg->msg.buf;

  sns_sam_algo_debug_params params;
  sns_sam_algo_output algoOutput;
  uint32_t i;

  params.num_params_valid = 0;
  cbFunc->sns_sam_algo_log(cbFunc->algoHndl, "threshold_gen_batch_ind", &params);

  indMsg->msgID = SNS_SAM_SENSOR_THRESH_BATCH_IND_V01;
  batchIndMsg->instance_id = extInstanceID;
  batchIndMsg->ind_type_valid = true;
  batchIndMsg->ind_type = indType;
  batchIndMsg->items_len = 0;

  for( i = 0; i < SNS_SAM_SENSOR_THRESH_MAX_ITEMS_IN_BATCH_V01; i++ )
  {
    algoOutput = cbFunc->sns_sam_algo_get_output( cbFunc->algoHndl );
    if( (intptr_t)NULL == algoOutput.data )
    {
      break;
    }

    threshold_output_s *output = (threshold_output_s *)algoOutput.data;

    batchIndMsg->items_len++;
    batchIndMsg->items[i].timestamp = output->timestamp;
    batchIndMsg->items[i].sample_value[0] = output->sample[0];
    batchIndMsg->items[i].sample_value[1] = output->sample[1];
    batchIndMsg->items[i].sample_value[2] = output->sample[2];
  }

  return SAM_ENONE;
}

static sns_sam_err
threshold_gen_report( sns_sam_algo_output const *outputData,
  sns_sam_resp_msg *respMsg)
{
  threshold_output_s *output = (threshold_output_s*)outputData->data;
  sns_sam_sensor_thresh_get_report_resp_msg_v01 *reportResp =
    (sns_sam_sensor_thresh_get_report_resp_msg_v01*)respMsg->msg.buf;

  reportResp->resp.sns_result_t = SNS_SUCCESS;

  reportResp->sample_value_valid = true;
  SNS_OS_MEMSCPY( reportResp->sample_value, sizeof(reportResp->sample_value), output->sample,
    sizeof(reportResp->sample_value) );

  reportResp->timestamp_valid = true;
  reportResp->timestamp = output->timestamp;

  return SAM_ENONE;
}

static bool
threshold_check_sensor( sns_sam_algo_state const *algoState,
  sns_sam_enable_req const *enableReq )
{
  threshold_state_s *thresholdState = (threshold_state_s*)algoState->buf;
  sns_sam_sensor_thresh_enable_req_msg_v01 *reqMsg =
    (sns_sam_sensor_thresh_enable_req_msg_v01*) enableReq->msg.buf;

  if( thresholdState->config.sensor_id == reqMsg->sensor_id &&
      thresholdState->config.data_type == reqMsg->data_type &&
      thresholdState->config.sample_rate == reqMsg->sample_rate &&
      (!SNS_OS_MEMCMP(thresholdState->config.thresh, reqMsg->threshold,
                      sizeof(thresholdState->config.thresh))))
  {
    return true;
  }
  return false;
}

static sns_sam_err
threshold_process_ind( sns_sam_algo_callback const *cbFunc,
  sns_sam_ind_msg const *indMsg )
{
  uint32_t buf_samples_cnt = 0;
  sns_smgr_buffering_ind_msg_v01 *bufInd =
    (sns_smgr_buffering_ind_msg_v01*)indMsg->msg.buf;
  sns_sam_buffered_data bufSample;
  uint32_t i;
  sns_sam_err err;

  sns_sam_buffered_data_init( &bufSample );

  for(err = sns_sam_get_single_buffered_sample( bufInd, &bufSample, &buf_samples_cnt);
    SAM_EMAX != err; err = sns_sam_get_single_buffered_sample( bufInd, &bufSample, &buf_samples_cnt) )
  {
    if( SAM_ENONE == err )
    {
      threshold_input_s input;

      for(i = 0; i < 3; i++)
      {
        input.sample[i] = bufSample.current_sample.data[i];
      }

      cbFunc->sns_sam_algo_add_input( cbFunc->algoHndl, indMsg->sensorUID, &(sns_sam_algo_input)
          { .timestamp = bufSample.current_sample.timestamp,
            .data = (intptr_t)&input } );
    }
  }
  return SAM_ENONE;
}

/*===========================================================================
  Public Definitions
  ===========================================================================*/

sns_sam_algo_msg_api threshold_algo_msg_api =
{
  sizeof(sns_sam_algo_msg_api),
  &threshold_mem_const_req,
  &threshold_get_attr,
  &threshold_dep_sensors_met,
  &threshold_dep_registry_met,
  &threshold_dep_sensors,
  &threshold_reg_grps,
  &threshold_process_enable_req,
  NULL,
  &threshold_process_ind,
  &threshold_gen_ind,
  &threshold_gen_batch_ind,
  &threshold_gen_report,
  &threshold_gen_req,
  &threshold_check_sensor
};
