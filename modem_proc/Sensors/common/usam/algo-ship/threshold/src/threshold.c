/*============================================================================
  @file threshold.c

  @brief
  Implementation of the threshold algorithm.

  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <math.h>
#include "sns_log_types.h"
#include "threshold.h"
#include "fixed_point.h"
#include "sns_profiling.h"
#include "sns_sam_algo_api.h"
#include "sns_memmgr.h"
#include "sns_sam_sensor_thresh_v01.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

/*============================================================================
  Extern Definitions
  ===========================================================================*/
extern sns_sam_algo_msg_api threshold_algo_msg_api;

/*============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * See sns_sam_algo_mem_req
 */
static sns_sam_err
threshold_mem_req( sns_sam_algo_config const *configData,
  sns_sam_algo_buf_size *bufferSizes )
{
  UNREFERENCED_PARAMETER(configData);
  bufferSizes->algoStateSize = sizeof(threshold_state_s);

  return SAM_ENONE;
}

/**
 * See sns_sam_algo_reset
 */
static sns_sam_err
threshold_state_reset( sns_sam_algo_config const *configData,
  sns_sam_algo_callback const *cbFunc, sns_sam_algo_persist *persistData,
  sns_sam_algo_state *algoState )
{
  UNREFERENCED_PARAMETER(persistData);
  threshold_state_s *thresholdState = (threshold_state_s*)algoState->buf;
  threshold_config_s *thresholdConfig = (threshold_config_s*)configData->buf;

  thresholdState->last_timestamp = 0;
  thresholdState->last_report[0] = 0;
  thresholdState->last_report[1] = 0;
  thresholdState->last_report[2] = 0;
  thresholdState->config = *thresholdConfig;
  thresholdState->ts_start_period[0] = 0;
  thresholdState->ts_start_period[1] = 0;
  thresholdState->ts_start_period[2] = 0;

  return SAM_ENONE;
}

/**
 * Updates the threshold algo results
 *
 * @param[i] cbFunc SAM callback functions
 * @param[i] state Current threshold algorithm state
 * @param[i] input Input object associated with this output; for logging
 * @param[i] outputData Output queue to which to add this new data
 *
 * @return SAM_EMAX No memory available for output buffer
 *         SAM_ENONE
 */
static sns_sam_err
threshold_update_results( sns_sam_algo_callback const *cbFunc,
  threshold_state_s *state, threshold_input_s const *input,
  sns_sam_timestamp timestamp )
{
  threshold_output_s output;
  uint32_t i;

  output.timestamp = timestamp;
  output.num_axis = state->config.num_axis;
  state->last_timestamp = timestamp;
  for( i = 0; i < output.num_axis; i++ )
  {
    output.sample[i] = input->sample[i];
    state->ts_start_period[i] = 0;
    state->last_report[i] = input->sample[i];
  }

  cbFunc->sns_sam_algo_add_output( cbFunc->algoHndl, &(sns_sam_algo_input)
    { .timestamp = 0, .data = (intptr_t)&output } );

  return SAM_ENONE;
}

/**
 * See sns_sam_algo_update
 */
static sns_sam_err
threshold_update( sns_sam_algo_state *algoState,
  sns_sam_algo_callback const *cbFunc, sns_sam_algo_persist *persistData )
{
  UNREFERENCED_PARAMETER(persistData);
  threshold_state_s *state = (threshold_state_s*)algoState->buf;
  sns_sam_algo_input algoInput;
  uint8_t num_axis = state->config.num_axis;

  for( algoInput = cbFunc->sns_sam_algo_get_input( cbFunc->algoHndl, NULL );
       NULL != (void*)algoInput.data;
       algoInput = cbFunc->sns_sam_algo_get_input( cbFunc->algoHndl, NULL ) )
  {
    uint32_t i;
    threshold_input_s const *input = (threshold_input_s*)algoInput.data;
    cbFunc->sns_sam_algo_log_qdss( cbFunc->algoHndl,
      SNS_SAM_ALGO_PROCESS_INPUT_ENTER, 0 );

    /* First time running the algo, always report */
    if( 0 == state->last_timestamp )
    {
      threshold_update_results( cbFunc, state, input, algoInput.timestamp );
      return SAM_ENONE;
    }

    for( i = 0; i < num_axis; i++ )
    {
      bool threshold_met = false;

      if( THRESH_TYPE_ABSOLUTE == state->config.threshold_type )
      {
        if( (uint32_t)FX_ABS(state->last_report[i] - input->sample[i]) >=
            (uint32_t)state->config.thresh[i] )
        {
          threshold_met = true;
        }
      }
      else if( THRESH_TYPE_RELATIVE == state->config.threshold_type )
      {
        if( 0 == state->config.thresh[i] )
        {
          threshold_met = true;
        }
        else if( 0 == state->last_report[i] && 0 == input->sample[i] )
        {
          /* samples of 0's should not be continuously reported with non-zero threshold */
          threshold_met = false;
        }
        else if( (uint32_t)FX_ABS(state->last_report[i] - input->sample[i]) >=
                 (uint32_t)FX_ABS(FX_MUL_Q16(state->config.thresh[i], state->last_report[i])) )
        {
          threshold_met = true;
        }
      }

      if( threshold_met )
      {
        /* We must meet the threshold for at least report_period */
        if( 0 == state->ts_start_period[i] )
        {
          state->ts_start_period[i] = algoInput.timestamp;
        }

        if( algoInput.timestamp >= state->ts_start_period[i] )
        {
          if( algoInput.timestamp - state->ts_start_period[i] >=
              state->config.report_period )
          {
            threshold_update_results( cbFunc, state, input, algoInput.timestamp );
            break;
          }
        }
        else
        {
          if( ((UINT32_MAX - state->ts_start_period[i]) +
                algoInput.timestamp) >= state->config.report_period )
          {
            /* A timestamp rollover has occurred */
            threshold_update_results( cbFunc, state, input, algoInput.timestamp );
            break;
          }
        }
      }
      else
      {
        state->ts_start_period[i] = 0;
      }
    }

    cbFunc->sns_sam_algo_log_qdss( cbFunc->algoHndl,
        SNS_SAM_ALGO_PROCESS_INPUT_EXIT, 0 );
  }

  return SAM_ENONE;
}

/**
 * See sns_sam_algo_generate
 */
static sns_sam_err
threshold_generate( sns_sam_algo_state *algoState,
  sns_sam_algo_callback const *cbFunc, sns_sam_algo_output *outputData )
{
  UNREFERENCED_PARAMETER(cbFunc);
  threshold_state_s *state = (threshold_state_s*)algoState->buf;
  threshold_output_s *output = (threshold_output_s*)outputData->data;

  if( 0 == state->last_timestamp )
  {
    return SAM_ESTATE;
  }
  else
  {
    SNS_OS_MEMSCPY( output->sample, sizeof(output->sample),
        state->last_report, sizeof(output->sample) );
    output->timestamp = state->last_timestamp;
    return SAM_ENONE;
  }
}

/**
 * See sns_sam_algo_check_config
 */
static bool
threshold_check_config( sns_sam_algo_state const *algoState,
  sns_sam_algo_config const *configData )
{
  bool configMatch = false;
  threshold_state_s *thresholdState = (threshold_state_s *) algoState->buf;
  configMatch = (0 == SNS_OS_MEMCMP((void *)configData->buf, &thresholdState->config,
    sizeof(threshold_config_s))) ? true : false;
  return configMatch;
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

/**
* Threshold Algo API
*
* See sns_sam_algo_api
*/
static sns_sam_algo_api threshold_algo_api =
{
  sizeof(sns_sam_algo_api),
  &threshold_mem_req,
  &threshold_state_reset,
  &threshold_update,
  &threshold_generate,
  &threshold_check_config
};

sns_sam_err threshold_sam_register(
      sns_sam_algo_api **algoAPI,
      sns_sam_algo_msg_api **msgAPI,
      qmi_idl_service_object_type *serviceObj,
      sns_sam_sensor_uid *sensorUID )
{
  *algoAPI = &threshold_algo_api;
  *msgAPI = &threshold_algo_msg_api;
  *serviceObj = SNS_SAM_SENSOR_THRESH_SVC_get_service_object_v01();
  *sensorUID = SNS_SAM_THRESHOLD_SUID_V01;
  return SAM_ENONE;
}
