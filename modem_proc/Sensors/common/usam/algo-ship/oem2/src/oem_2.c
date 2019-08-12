/*============================================================================
  @file oem_2.c

  @brief
  Implementation of the OEM_2, custom algorithm template with SAM 2.0.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <math.h>
#include "sns_log_types.h"
#include "oem_2.h"
#include "fixed_point.h"
#include "sns_sam_algo_api.h"
#include "sns_memmgr.h"
#include "sns_oem_2_v01.h"
#include "location_service_v02.h"
#include "sns_debug_str.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

//DSPS clock frequency 32768Hz
#define OEM_2_DSPS_CLK_FREQ 32768

/*============================================================================
  Extern Definitions
  ===========================================================================*/
extern sns_sam_algo_msg_api oem_2_algo_msg_api;
extern sns_sam_algo_api oem_2_algo_api;

/*============================================================================
  Function Definitions
  ===========================================================================*/

/**
 * See sns_sam_algo_mem_req
 */
sns_sam_err
oem_2_mem_req(
  sns_sam_algo_config const *configData,
  sns_sam_algo_buf_size *bufferSizes )
{
  UNREFERENCED_PARAMETER(configData);
  bufferSizes->algoStateSize = sizeof(oem_2_state_s);

  return SAM_ENONE;
}

/**
 * See sns_sam_algo_reset
 */
sns_sam_err
oem_2_reset(
  sns_sam_algo_config const *configData,
  sns_sam_algo_callback const *cbFunc, sns_sam_algo_persist *persistData,
  sns_sam_algo_state *algoState )
{
  UNREFERENCED_PARAMETER(persistData);

  oem_2_state_s *oem_2State = (oem_2_state_s*)algoState->buf;
  oem_2_config_s *oem_2Config = (oem_2_config_s*)configData->buf;
  oem_2State->config = *oem_2Config;

  return SAM_ENONE;
}

/**
 * See sns_sam_algo_check_config
 */
bool
oem_2_check_config( sns_sam_algo_state const *algoState,
  sns_sam_algo_config const *configData )
{
  oem_2_state_s *oem_2State = (oem_2_state_s*)algoState->buf;
  oem_2_config_s *oem_2Config = (oem_2_config_s*)configData->buf;

  if( oem_2State->config.sample_rate == oem_2Config->sample_rate &&
      oem_2State->config.inputType == oem_2Config->inputType )
  {
    return true;
  }

  return false;
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_sam_err oem_2_sam_register(
  sns_sam_algo_api **algoAPI,
  sns_sam_algo_msg_api **msgAPI,
  qmi_idl_service_object_type *serviceObj,
  sns_sam_sensor_uid *sensorUID )
{
  *algoAPI = &oem_2_algo_api;
  *msgAPI = &oem_2_algo_msg_api;
  *serviceObj = SNS_OEM_2_SVC_get_service_object_v01();
  *sensorUID = SNS_SAM_OEM_2_SUID_V01;
  return SAM_ENONE;
}
