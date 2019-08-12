/*============================================================================
  @file oem_1_msg.c

  @brief
    Implementation of the MSG APIs for OEM_1, custom algorithm template with SAM 2.0.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <math.h>
#include "log_codes.h"
#include "log.h"
#include "oem_1.h"
#include "fixed_point.h"
#include "sns_memmgr.h"
#include "sns_reg_common.h"
#include "sns_sam_algo_utils.h"
#include "sns_reg_api_v02.h"
#include "sns_smgr_api_v01.h"
#include "sns_sam_algo_api.h"
#include "sns_oem_1_v01.h"
#include "sns_sam_common_v01.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/
#define SNS_SAM_MAX_OEM_1_SAMPLE_RATE_Q16 FX_CONV_Q16(200,0)
#define SNS_SAM_MIN_OEM_1_SAMPLE_RATE_Q16 FX_CONV_Q16(1,0)
#define SNS_SAM_DEF_OEM_1_SAMPLE_RATE_Q16 FX_CONV_Q16(20,0)

#define SNS_SAM_OEM1_REPORT_RATE FX_FLTTOFIX_Q16(0.5f) // 2 second report period

#define OEM_1_ALGO_REVISION 1

//#define OEM_1_USE_AMD

/*===========================================================================
  Function Definitions
  ===========================================================================*/

void oem_1_mem_const_req(
  sns_sam_algo_const_buf_size *bufferSizes )
{
  bufferSizes->configDataSize = sizeof(oem_1_config_s);
  bufferSizes->persistDataSize = sizeof(oem_1_persist);
  bufferSizes->inputDataSize = sizeof(oem_1_input_s);
  bufferSizes->outputDataSize = sizeof(oem_1_output_s);
}

sns_sam_err oem_1_dep_sensors(
  uint32_t *sensorCount,
  struct sns_sam_algo_dep_sensor dependentSensors[] )
{
  if( *sensorCount >= 1 )
  {
    *sensorCount = 1;
    dependentSensors[0].sensorUID = SNS_SMGR_SUID_ACCEL_1_V01;
    dependentSensors[0].serviceObj = SNS_SMGR_SVC_get_service_object_v01();

#ifdef OEM_1_USE_AMD
    *sensorCount = 2;
    dependentSensors[1].sensorUID = SNS_SAM_AMD_SUID_V01;
    dependentSensors[1].serviceObj = SNS_SAM_AMD_SVC_get_service_object_v01();
#endif

    return SAM_ENONE;
  }
  else
  {
    return SAM_EMAX;
  }
}

sns_sam_err oem_1_reg_grps(
  uint32_t *registryGrpCount,
  sns_sam_reg_group registryGroups[] )
{
  *registryGrpCount = 0;
  return SAM_ENONE;
}

void
oem_1_dep_sensors_met(
  struct sns_sam_sensor_algo const *algo,
  sns_sam_algo_all_dep_met depMetFunc,
  sns_sam_algo_persist *persistData,
  sns_sam_sensor_uid const *sensorUID,
  sns_sam_algo_attr const (*sns_sam_attr_array)[ SAM_ALGO_ATTR_CNT ] )
{
  sns_sam_algo_attr *attr_array;
  oem_1_persist *oem_1PersistData = (oem_1_persist*)persistData->buf;
  uint32_t i;
  attr_array = (sns_sam_algo_attr*)sns_sam_attr_array;

  if( SNS_SMGR_SUID_ACCEL_1_V01 == *sensorUID )
  {
    oem_1PersistData->accelSUID = sensorUID;
    oem_1PersistData->defSampleRate = SNS_SAM_DEF_OEM_1_SAMPLE_RATE_Q16;
    oem_1PersistData->minSampleRate = SNS_SAM_MIN_OEM_1_SAMPLE_RATE_Q16;
    oem_1PersistData->maxSampleRate = SNS_SAM_MAX_OEM_1_SAMPLE_RATE_Q16;

    for( i = 0;
         i < SAM_ALGO_ATTR_CNT && SAM_ALGO_ATTR_CNT != attr_array[i].attribute;
         i++ )
    {
      if( attr_array[i].supported )
      {
        if( SAM_ALGO_ATTR_POWER == attr_array[i].attribute )
        {
          oem_1PersistData->power = FX_CONV_Q16(attr_array[i].attrValue, 0) / 1000;
        }
        else if( SAM_ALGO_ATTR_MIN_SAMPLE == attr_array[i].attribute )
        {
          oem_1PersistData->minSampleRate = attr_array[i].attrValue > SNS_SAM_MIN_OEM_1_SAMPLE_RATE_Q16 ?
             attr_array[i].attrValue : SNS_SAM_MIN_OEM_1_SAMPLE_RATE_Q16;
        }
        else if( SAM_ALGO_ATTR_MAX_SAMPLE == attr_array[i].attribute )
        {
          oem_1PersistData->maxSampleRate = attr_array[i].attrValue < SNS_SAM_MAX_OEM_1_SAMPLE_RATE_Q16 ?
            attr_array[i].attrValue : SNS_SAM_MAX_OEM_1_SAMPLE_RATE_Q16;
        }
      }
    }
  }
  else if( SNS_SAM_AMD_SUID_V01 == *sensorUID )
  {
    oem_1PersistData->amdSUID = sensorUID;
  }

  if( NULL != oem_1PersistData->accelSUID
#ifdef OEM_1_USE_AMD
      && NULL != oem_1PersistData->amdSUID
#endif
      )
  {
    depMetFunc( algo );
  }
}

void oem_1_get_attr(
  sns_sam_algo_persist const *persistData,
  sns_sam_algo_attr attributes[] )
{
  uint8_t i;
  oem_1_persist *oem_1PersistData = (oem_1_persist*)persistData->buf;

  for( i = 0;
       i < SAM_ALGO_ATTR_CNT && SAM_ALGO_ATTR_CNT != attributes[i].attribute;
       i++ )
  {
    switch( attributes[i].attribute )
    {
    case SAM_ALGO_ATTR_POWER:
      attributes[i].attrValue = oem_1PersistData->power;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_REPORT_MODE:
      attributes[i].attrValue = SNS_SAM_SYNC_REPORT_V01;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_MIN_REPORT:
      attributes[i].attrValue = 0;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_MAX_REPORT:
      attributes[i].attrValue = oem_1PersistData->maxSampleRate;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_MIN_SAMPLE:
      attributes[i].attrValue = oem_1PersistData->minSampleRate;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_MAX_SAMPLE:
      attributes[i].attrValue = oem_1PersistData->maxSampleRate;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_REVISION:
      attributes[i].attrValue = OEM_1_ALGO_REVISION;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_MAX_BATCH:
      attributes[i].attrValue = 0;
      attributes[i].supported = false;
      break;

    default:
      attributes[i].supported = false;
    }
  }

}

sns_sam_err oem_1_process_enable_req(
  sns_sam_enable_req const *enableReq,
  sns_sam_algo_persist *persistData, sns_sam_client_attr *clientAttr,
  sns_sam_algo_config *configData )
{
  oem_1_persist *oem_1Persist = (oem_1_persist*)persistData->buf;
  oem_1_config_s *oem_1Config = (oem_1_config_s*)configData->buf;
  sns_oem_1_enable_req_msg_v01 *reqMsg =
    (sns_oem_1_enable_req_msg_v01*) enableReq->msg.buf;

  clientAttr->reportMode = SNS_SAM_REPORT_MODE_SYNC;

  if( !reqMsg->sample_rate_valid )
  {
    oem_1Config->sample_rate = oem_1Persist->defSampleRate;
  }
  else if( oem_1Persist->minSampleRate < (q16_t)reqMsg->sample_rate &&
           oem_1Persist->maxSampleRate > (q16_t)reqMsg->sample_rate )
  {
    oem_1Config->sample_rate = reqMsg->sample_rate;
  }
  else
  {
    return SAM_EFAILED;
  }

  clientAttr->reportPeriod =
    FX_FLTTOFIX_Q16(1.0f / FX_FIXTOFLT_Q16_SP(oem_1Config->sample_rate));
  oem_1Config->report_period = clientAttr->reportPeriod;

  return SAM_ENONE;
}

sns_sam_err oem_1_gen_req(
  sns_sam_algo_state const *algoState,
  sns_sam_sensor_uid const *sensorUID,
  sns_sam_enable_req *enableReqMsg )
{
  oem_1_state_s *oem_1State = (oem_1_state_s*)algoState->buf;

  if( SNS_SMGR_SUID_ACCEL_1_V01 == *sensorUID )
  {
    sns_sam_sensor_data_info_req accelReq;

    accelReq.sensorCount = 1;
    accelReq.sampleRate = oem_1State->config.sample_rate;
    accelReq.sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
    accelReq.sensorDbase[0].sampleQual = SNS_SAM_DEFAULT_SAMPLE_QUALITY;
    accelReq.sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
    accelReq.sensorDbase[0].buffered_sampleRate = oem_1State->config.sample_rate;
    accelReq.srcUID = SNS_OEM_1_SUID_V01;

    if( SNS_SAM_MOTION_REST_V01 == oem_1State->amdState )
    {
      accelReq.reportRate = SNS_SAM_OEM1_REPORT_RATE;
    }
    else
    {
      accelReq.reportRate = oem_1State->config.sample_rate;
    }

    return sns_sam_util_gen_smgr_buffer_req( &accelReq, enableReqMsg );
  }
  else if( SNS_SAM_AMD_SUID_V01 == *sensorUID )
  {
    sns_sam_qmd_enable_req_msg_v01 *amdReq =
      (sns_sam_qmd_enable_req_msg_v01*)enableReqMsg->msg.buf;

    enableReqMsg->msg.bufSize = sizeof(sns_sam_qmd_enable_req_msg_v01);
    enableReqMsg->msgID = SNS_SAM_AMD_ENABLE_REQ_V01;

    amdReq->report_period = 0;
    amdReq->config_valid = 0;

    amdReq->notify_suspend_valid = true;
    amdReq->notify_suspend.proc_type = SNS_PROC_SSC_V01;
    amdReq->notify_suspend.send_indications_during_suspend = true;

    return SAM_ENONE;
  }

  return SAM_ESTATE;
}

sns_sam_err oem_1_gen_report(
  sns_sam_algo_output const *outputData,
  sns_sam_resp_msg *respMsg)
{
  oem_1_output_s *output = (oem_1_output_s*)outputData->data;
  sns_oem_1_get_report_resp_msg_v01 *reportResp =
    (sns_oem_1_get_report_resp_msg_v01*)respMsg->msg.buf;

  reportResp->resp.sns_result_t = SNS_SUCCESS;

  reportResp->output_valid = true;
  SNS_OS_MEMCOPY( reportResp->output.data, output->data,
    sizeof(reportResp->output.data) );

  reportResp->timestamp_valid = true;
  reportResp->timestamp = output->timestamp;

  return SAM_ENONE;
}

bool oem_1_check_sensor(
  sns_sam_algo_state const *algoState,
  sns_sam_enable_req const *enableReq )
{
  oem_1_state_s *oem_1State = (oem_1_state_s*)algoState->buf;

  if( SNS_SMGR_SUID_ACCEL_1_V01 == *enableReq->sensorUID)
  {
    sns_smgr_buffering_req_msg_v01 *accelReq =
      (sns_smgr_buffering_req_msg_v01*)enableReq->msg.buf;

    if( (q16_t)accelReq->ReportRate == oem_1State->config.sample_rate &&
        FX_CONV_Q16(accelReq->Item[0].SamplingRate, 0) == oem_1State->config.sample_rate &&
        accelReq->Item[0].SampleQuality == SNS_SAM_DEFAULT_SAMPLE_QUALITY )
    {
      return true;
    }
  }
  return false;
}
