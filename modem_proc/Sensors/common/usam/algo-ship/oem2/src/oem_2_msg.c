/*============================================================================
  @file oem_2_msg.c

  @brief
    Implementation of the MSG APIs for OEM_2, custom algorithm template with SAM 2.0.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <math.h>
#include "log_codes.h"
#include "log.h"
#include "oem_2.h"
#include "fixed_point.h"
#include "sns_memmgr.h"
#include "sns_reg_common.h"
#include "sns_sam_algo_utils.h"
#include "sns_reg_api_v02.h"
#include "sns_smgr_api_v01.h"
#include "sns_sam_algo_api.h"
#include "sns_oem_2_v01.h"
#include "location_service_v02.h"
#include "sns_sam_common_v01.h"
#include "sns_debug_str.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/
#define SNS_SAM_OEM_2_REPORT_RATE_MAX_Q16 FX_FLTTOFIX_Q16(100)
#define SNS_SAM_OEM_2_REPORT_RATE_MIN_Q16 0

#define SNS_SAM_OEM_2_SAMPLE_RATE_MAX_Q16 FX_FLTTOFIX_Q16(100)
#define SNS_SAM_OEM_2_SAMPLE_RATE_MIN_Q16 FX_FLTTOFIX_Q16(1)
#define OEM_2_ALGO_REVISION 1
#define SNS_SAM_OEM_2_POWER 0

/*===========================================================================
  Function Definitions
  ===========================================================================*/

void oem_2_mem_const_req(
  sns_sam_algo_const_buf_size *bufferSizes )
{
  bufferSizes->configDataSize = sizeof(oem_2_config_s);
  bufferSizes->persistDataSize = sizeof(oem_2_persist);
  bufferSizes->inputDataSize = sizeof(oem_2_input_s);
  bufferSizes->outputDataSize = sizeof(oem_2_output_s);
}

sns_sam_err oem_2_dep_sensors(
  uint32_t *sensorCount,
  struct sns_sam_algo_dep_sensor dependentSensors[] )
{
  if( *sensorCount >= 2 )
  {
    *sensorCount = 2;

    dependentSensors[0].sensorUID = SNS_SAM_PROV_LOC_POS_SUID;
    dependentSensors[0].serviceObj = loc_get_service_object_v02();

    dependentSensors[1].sensorUID = SNS_SAM_PROV_LOC_SV_INFO_SUID;
    dependentSensors[1].serviceObj = loc_get_service_object_v02();

    return SAM_ENONE;
  }
  else
  {
    return SAM_EMAX;
  }
}

sns_sam_err oem_2_reg_grps(
  uint32_t *registryGrpCount,
  sns_sam_reg_group registryGroups[] )
{
  *registryGrpCount = 0;
  return SAM_ENONE;
}

void oem_2_dep_sensors_met(
  struct sns_sam_sensor_algo const *algo,
  sns_sam_algo_all_dep_met depMetFunc,
  sns_sam_algo_persist *persistData,
  sns_sam_sensor_uid const *sensorUID,
  sns_sam_algo_attr const (*sns_sam_attr_array)[ SAM_ALGO_ATTR_CNT ] )
{
  oem_2_persist *oem_2PersistData = (oem_2_persist*)persistData->buf;

  if( SNS_SAM_PROV_LOC_POS_SUID == *sensorUID )
  {
    oem_2PersistData->locSUID = sensorUID;
  }
  else if( SNS_SAM_PROV_LOC_SV_INFO_SUID == *sensorUID )
  {
    oem_2PersistData->satSUID = sensorUID;
  }

  if( NULL != oem_2PersistData->locSUID &&
      NULL != oem_2PersistData->satSUID )
  {
    depMetFunc( algo );
  }
}

void oem_2_get_attr(
  sns_sam_algo_persist const *persistData,
  sns_sam_algo_attr attributes[] )
{
  uint8_t i;

  for( i = 0;
       i < SAM_ALGO_ATTR_CNT && SAM_ALGO_ATTR_CNT != attributes[i].attribute;
       i++ )
  {
    switch( attributes[i].attribute )
    {
    case SAM_ALGO_ATTR_POWER:
      attributes[i].attrValue = SNS_SAM_OEM_2_POWER;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_REPORT_MODE:
      attributes[i].attrValue =
        SNS_SAM_PERIODIC_REPORT_V01 | SNS_SAM_SYNC_REPORT_V01;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_MIN_REPORT:
      attributes[i].attrValue = 0;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_MAX_REPORT:
      attributes[i].attrValue = SNS_SAM_OEM_2_REPORT_RATE_MAX_Q16;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_MIN_SAMPLE:
      attributes[i].attrValue = SNS_SAM_OEM_2_SAMPLE_RATE_MIN_Q16;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_MAX_SAMPLE:
      attributes[i].attrValue = SNS_SAM_OEM_2_SAMPLE_RATE_MAX_Q16;
      attributes[i].supported = true;
      break;

    case SAM_ALGO_ATTR_REVISION:
      attributes[i].attrValue = OEM_2_ALGO_REVISION;
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

sns_sam_err oem_2_process_enable_req(
  sns_sam_enable_req const *enableReq,
  sns_sam_algo_persist *persistData, sns_sam_client_attr *clientAttr,
  sns_sam_algo_config *configData )
{
  oem_2_config_s *oem_2Config = (oem_2_config_s*)configData->buf;
  sns_oem_2_enable_req_msg_v01 *reqMsg =
    (sns_oem_2_enable_req_msg_v01*) enableReq->msg.buf;

  if( !reqMsg->sample_rate_valid )
  {
    oem_2Config->sample_rate = -1;
  }
  else
  {
    oem_2Config->sample_rate = FX_FLTTOFIX_Q16(reqMsg->sample_rate);
  }

  if( 0 != reqMsg->report_period )
  {
    clientAttr->reportMode = SNS_SAM_REPORT_MODE_PERIODIC;
    clientAttr->reportPeriod = (q16_t)reqMsg->report_period;
    oem_2Config->report_period = reqMsg->report_period;
  }
  else
  {
    clientAttr->reportMode = SNS_SAM_REPORT_MODE_SYNC;
    oem_2Config->report_period = 0;
  }

  oem_2Config->inputType = reqMsg->request_type;
  return SAM_ENONE;
}

sns_sam_err oem_2_gen_req(
  sns_sam_algo_state const *algoState,
  sns_sam_sensor_uid const *sensorUID,
  sns_sam_enable_req *enableReqMsg )
{
  oem_2_state_s *oem_2State = (oem_2_state_s*)algoState->buf;

  qmiLocStartReqMsgT_v02 *gnssReq =
  (qmiLocStartReqMsgT_v02*)enableReqMsg->msg.buf;

  enableReqMsg->msg.bufSize = sizeof(qmiLocStartReqMsgT_v02);
  enableReqMsg->msgID = QMI_LOC_START_REQ_V02;

  if( ( SNS_SAM_PROV_LOC_POS_SUID == *sensorUID && SNS_OEM_2_POSITION_REPORT_V01 == oem_2State->config.inputType ) ||
      ( SNS_SAM_PROV_LOC_SV_INFO_SUID == *sensorUID && SNS_OEM_2_GNSS_SV_INFO_TYPE_V01 == oem_2State->config.inputType ) )
  {
    gnssReq->sessionId = 0;
    gnssReq->fixRecurrence_valid = true;
    gnssReq->fixRecurrence = eQMI_LOC_RECURRENCE_PERIODIC_V02;
    //gnssReq->intermediateReportState_valid = true;
    //gnssReq->intermediateReportState = eQMI_LOC_INTERMEDIATE_REPORTS_OFF_V02;

    if( -1 == oem_2State->config.sample_rate )
    {
      gnssReq->minInterval_valid = false;
    }
    else
    {
      gnssReq->minInterval_valid = true;
      gnssReq->minInterval = (1 / oem_2State->config.sample_rate) * 1000;
    }

    return SAM_ENONE;
  }

  return SAM_ESTATE;

}

sns_sam_err oem_2_gen_report(
  sns_sam_algo_output const *outputData,
  sns_sam_resp_msg *respMsg)
{
  oem_2_output_s *output = (oem_2_output_s*)outputData->data;
  sns_oem_2_get_report_resp_msg_v01 *reportResp =
    (sns_oem_2_get_report_resp_msg_v01*)respMsg->msg.buf;

  reportResp->resp.sns_result_t = SNS_SUCCESS;

  reportResp->data_valid = true;
  reportResp->data_len = output->sample.dataLen;
  SNS_OS_MEMCOPY( reportResp->data, output->sample.data, output->sample.dataLen );

  reportResp->timestamp_valid = true;
  reportResp->timestamp = output->sample.timestamp;

  reportResp->request_type_valid = true;
  reportResp->request_type = output->sample.inputType;

  return SAM_ENONE;
}

bool oem_2_check_sensor(
  sns_sam_algo_state const *algoState,
  sns_sam_enable_req const *enableReq )
{
  oem_2_state_s *oem_2State = (oem_2_state_s*)algoState->buf;

  if( ( SNS_SAM_PROV_LOC_POS_SUID == *enableReq->sensorUID && SNS_OEM_2_POSITION_REPORT_V01 == oem_2State->config.inputType ) ||
      ( SNS_SAM_PROV_LOC_SV_INFO_SUID == *enableReq->sensorUID && SNS_OEM_2_GNSS_SV_INFO_TYPE_V01 == oem_2State->config.inputType ) )
  {
    qmiLocStartReqMsgT_v02 *locStartReq =
      (qmiLocStartReqMsgT_v02*)enableReq->msg.buf;

    if( locStartReq->minInterval == (1 / oem_2State->config.sample_rate) * 1000 )
    {
      return true;
    }
  }

  return false;
}
