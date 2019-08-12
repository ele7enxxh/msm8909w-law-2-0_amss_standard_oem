/*============================================================================
  @file oem_1.c

  @brief
  Implementation of the OEM_1, custom algorithm template with SAM 2.0.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <math.h>
#include "sns_log_types.h"
#include "oem_1.h"
#include "fixed_point.h"
#include "sns_sam_algo_api.h"
#include "sns_memmgr.h"
#include "sns_oem_1_v01.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

//DSPS clock frequency 32768Hz
#define OEM_1_DSPS_CLK_FREQ 32768

/*============================================================================
  Extern Definitions
  ===========================================================================*/
extern sns_sam_algo_msg_api oem_1_algo_msg_api;
extern sns_sam_algo_api oem_1_algo_api;

/*============================================================================
  Function Definitions
  ===========================================================================*/

/**
 * See sns_sam_algo_mem_req
 */
sns_sam_err
oem_1_mem_req(
  sns_sam_algo_config const *configData,
  sns_sam_algo_buf_size *bufferSizes )
{
  UNREFERENCED_PARAMETER(configData);
  bufferSizes->algoStateSize = sizeof(oem_1_state_s);

  return SAM_ENONE;
}

/**
 * See sns_sam_algo_reset
 */
sns_sam_err
oem_1_reset(
  sns_sam_algo_config const *configData,
  sns_sam_algo_callback const *cbFunc, sns_sam_algo_persist *persistData,
  sns_sam_algo_state *algoState )
{
  UNREFERENCED_PARAMETER(persistData);

  oem_1_state_s *oem_1State = (oem_1_state_s*)algoState->buf;
  oem_1_config_s *oem_1Config = (oem_1_config_s*)configData->buf;
  oem_1State->state_param1= 0;
  oem_1State->config = *oem_1Config;
  oem_1State->amdState = SNS_SAM_MOTION_UNKNOWN_V01;

  return SAM_ENONE;
}

/**
 * See sns_sam_algo_check_config
 */
bool
oem_1_check_config( sns_sam_algo_state const *algoState,
  sns_sam_algo_config const *configData )
{
  oem_1_state_s *oem_1State = (oem_1_state_s*)algoState->buf;
  oem_1_config_s *oem_1Config = (oem_1_config_s*)configData->buf;
  return oem_1State->config.sample_rate == oem_1Config->sample_rate;
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_sam_err oem_1_sam_register(
  sns_sam_algo_api **algoAPI,
  sns_sam_algo_msg_api **msgAPI,
  qmi_idl_service_object_type *serviceObj,
  sns_sam_sensor_uid *sensorUID )
{
  *algoAPI = &oem_1_algo_api;
  *msgAPI = &oem_1_algo_msg_api;
  *serviceObj = SNS_OEM_1_SVC_get_service_object_v01();
  *sensorUID = SNS_OEM_1_SUID_V01;
  return SAM_ENONE;
}
