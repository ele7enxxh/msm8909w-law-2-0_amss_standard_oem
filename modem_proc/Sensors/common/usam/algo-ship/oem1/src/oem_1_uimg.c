/*============================================================================
  @file oem_1_uimg.c

  @brief
  Implementation of the OEM_1, custom algorithm template with SAM 2.0,
  with uImage support.

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "oem_1.h"
#include "fixed_point.h"
#include "sns_memmgr.h"
#include "sns_sam_algo_api.h"
#include "sns_sam_algo_utils.h"
#include "sns_oem_1_v01.h"

/*============================================================================
  Function Definitions
  ===========================================================================*/

/**
  * Updates an algorithm with some input data. Zero or more pieces of output
  * data might be generated.
  *
  * @param[in] algoState Algorithm state that might be updated as a result.
  * @param[in] cbFunc Provides additional functionality to the algorithms.
  * @param[in,out] inputData List of input data objects to be processed.
  * @param[in,out] persistData Algorithm-wide persistent data.
  * @param[out] processedCnt Number of items processed or to be removed from
  *                          inputData.
  *
  * @return
  * SAM_ENONE   -- Operation was successful.
  * SAM_EFAILED -- Algorithm failed to process any data at this time.
  * SAM_ESTATE  -- Algorithm cannot process any further input.
  * SAM_EMAX    -- Algorithm is unable to create enough output objects.
  */
sns_sam_err oem_1_update(
  sns_sam_algo_state *algoState,
  sns_sam_algo_callback const *cbFunc,
  sns_sam_algo_persist *persistData )
{
  oem_1_persist *oem1Persist = (oem_1_persist*)persistData->buf;
  oem_1_state_s *state = (oem_1_state_s*)algoState->buf;
  oem_1_input_s const *input;
  sns_sam_algo_input algoInputBuf;
  sns_sam_algo_debug_params params;
  char filename[] = __FILE__;
  params.filename = filename;

  for( algoInputBuf = cbFunc->sns_sam_algo_get_input( cbFunc->algoHndl, NULL );
       NULL != (void*)algoInputBuf.data;
       algoInputBuf = cbFunc->sns_sam_algo_get_input( cbFunc->algoHndl, NULL ))
  {
    input = (oem_1_input_s*)algoInputBuf.data;

    if( OEM1_INPUT_AMD == input->inputType )
    {
      if( state->amdState != input->amdState )
      {
        state->amdState = input->amdState;

        cbFunc->sns_sam_algo_sensor_change(cbFunc->algoHndl,
            oem1Persist->accelSUID);

        params.num_params_valid = 1; // upto 3 params supported
        params.param1 = input->amdState;
        params.line_num =__LINE__;
        cbFunc->sns_sam_algo_log(cbFunc->algoHndl, "OEM1 AMD Input:%i",&params);
      }
    }
    else if( OEM1_INPUT_ACCEL == input->inputType )
    {
      oem_1_output_s output;

      state->data[0] = input->sample_1[0];
      state->data[1] = input->sample_1[1];
      state->data[2] = input->sample_1[2];
      state->timestamp = algoInputBuf.timestamp;

      //OEM_1 algorithm logic
      //   [0] = 1 if SAE Z-axis is negative and 2 if SAE Z-axis is positive
      //   [1] = SAE accel z value
      //   [2] = hardcoded value, 100
      output.data[0] = (state->data[2] < 0) ? (1) : (2);
      output.data[1] = state->data[2];
      output.data[2] = 100;
      output.timestamp = algoInputBuf.timestamp;

      cbFunc->sns_sam_algo_add_output( cbFunc->algoHndl, &(sns_sam_algo_input)
        { .timestamp = 0, .data = (intptr_t)&output } );
    }
  }

  return SAM_ENONE;
}

/**
 * See sns_sam_algo_generate
 */
sns_sam_err oem_1_generate(
  sns_sam_algo_state *algoState,
  sns_sam_algo_callback const *cbFunc,
  sns_sam_algo_output *outputData )
{
  UNREFERENCED_PARAMETER(cbFunc);
  oem_1_state_s *oem_1State = (oem_1_state_s*)algoState->buf;
  oem_1_output_s *output = (oem_1_output_s*)outputData->data;

  SNS_OS_MEMCOPY( output->data, oem_1State->data, sizeof(output->data) );
  output->timestamp = oem_1State->timestamp;

  return SAM_ENONE;
}

sns_sam_err oem_1_process_ind(
  sns_sam_algo_callback const *cbFunc,
  sns_sam_ind_msg const *indMsg )
{
  if( SNS_SMGR_SUID_ACCEL_1_V01 == *indMsg->sensorUID)
  {
    uint32_t buf_samples_cnt = 0;
    sns_smgr_buffering_ind_msg_v01 *bufInd =
      (sns_smgr_buffering_ind_msg_v01*)indMsg->msg.buf;
    sns_sam_buffered_data bufSample;
    uint32_t i;
    sns_sam_err err;

    sns_sam_buffered_data_init( &bufSample );

    for(err = sns_sam_get_single_buffered_sample( bufInd, &bufSample, &buf_samples_cnt);
      SAM_EMAX != err; err = sns_sam_get_single_buffered_sample( bufInd, &bufSample,
      &buf_samples_cnt) )
    {
      if( SAM_ENONE == err)
      {
        oem_1_input_s input;
        for(i = 0; i < 3; i++)
        {
          input.sample_1[i] = bufSample.current_sample.data[i];
        }
        input.inputType = OEM1_INPUT_ACCEL;

        cbFunc->sns_sam_algo_add_input( cbFunc->algoHndl, indMsg->sensorUID,
            &(sns_sam_algo_input){ .timestamp = bufSample.current_sample.timestamp,
            .data = (intptr_t)&input } );
      }
    }
  }
  else if( SNS_SAM_AMD_SUID_V01 == *indMsg->sensorUID )
  {
    oem_1_input_s input;
    sns_sam_qmd_report_ind_msg_v01 *amdInd =
      (sns_sam_qmd_report_ind_msg_v01*)indMsg->msg.buf;

    input.amdState = amdInd->state;
    input.inputType = OEM1_INPUT_AMD;

    cbFunc->sns_sam_algo_add_input( cbFunc->algoHndl, indMsg->sensorUID,
            &(sns_sam_algo_input){ .timestamp = amdInd->timestamp,
            .data = (intptr_t)&input } );
  }

  return SAM_ENONE;
}

sns_sam_err oem_1_gen_ind(
  sns_sam_algo_callback const *cbFunc,
  sns_sam_algo_output const *outputData,
  uint8_t extInstanceID,
  sns_sam_ind_msg *indMsg)
{
  oem_1_output_s *output = (oem_1_output_s*)outputData->data;
  sns_oem_1_report_ind_msg_v01 *ind_msg =
    (sns_oem_1_report_ind_msg_v01*)indMsg->msg.buf;

  ind_msg->timestamp = output->timestamp;
  SNS_OS_MEMCOPY( ind_msg->output.data, output->data,
    sizeof(ind_msg->output.data) );
  ind_msg->output.data_len = OEM_1_ELEMS;

  return SAM_ENONE;
}

/**
 * See sns_sam_algo_gen_batch_ind
 */
sns_sam_err oem_1_gen_batch_ind(
   sns_sam_algo_callback const *cbFunc,
   uint8_t extInstanceID, sns_sam_batch_ind_type indType,
   sns_sam_ind_msg *indMsg )
{
  sns_sam_oem_1_batch_ind_msg_v01 *ind_msg =
    (sns_sam_oem_1_batch_ind_msg_v01*)indMsg->msg.buf;

  sns_sam_algo_output algoOutput;
  oem_1_output_s *output;
  uint32_t i = 0, j;
  uint64_t last_ts = 0;

  ind_msg->ind_type_valid = true;
  ind_msg->ind_type = indType;
  ind_msg->reports_len = 0;
  ind_msg->timestamp_offsets_len = 0;

  for( i = 0; i < SNS_SAM_OEM_1_MAX_ITEMS_IN_BATCH_V01; i++ )
  {
    algoOutput = cbFunc->sns_sam_algo_get_output( cbFunc->algoHndl );
    if( (intptr_t)NULL == algoOutput.data )
    {
      break;
    }
    output = (oem_1_output_s*)algoOutput.data;

    if( 0 == ind_msg->timestamp_offsets_len )
    {
      ind_msg->first_report_timestamp = output->timestamp;
      last_ts = output->timestamp;
    }

    ind_msg->timestamp_offsets[i] = output->timestamp - last_ts;
    ind_msg->reports_len++;
    ind_msg->timestamp_offsets_len++;

    for( j = 0; j < SNS_OEM_1_MAX_ITEMS_PER_OUTPUT_DATA_V01; j++ )
    {
      ind_msg->reports[i].data[j] = output->data[j];
    }
    ind_msg->reports[i].data_len = SNS_OEM_1_MAX_ITEMS_PER_OUTPUT_DATA_V01;

    last_ts = output->timestamp;
  }
  return SAM_ENONE;
}

/*===========================================================================
  Public Definitions
  ===========================================================================*/

/**
* OEM_1 Algorithm message API
*
* See sns_sam_algo_msg_api
*/

sns_sam_algo_msg_api oem_1_algo_msg_api =
{
  sizeof(sns_sam_algo_msg_api),
  &oem_1_mem_const_req,
  &oem_1_get_attr,
  &oem_1_dep_sensors_met,
  NULL,
  &oem_1_dep_sensors,
  &oem_1_reg_grps,
  &oem_1_process_enable_req,
  NULL,
  &oem_1_process_ind,
  &oem_1_gen_ind,
  &oem_1_gen_batch_ind,
  &oem_1_gen_report,
  &oem_1_gen_req,
  &oem_1_check_sensor
};

/**
* OEM_1 Algorithm API
*
* See sns_sam_algo_api
*/
sns_sam_algo_api oem_1_algo_api =
{
  sizeof(sns_sam_algo_api),
  &oem_1_mem_req,
  &oem_1_reset,
  &oem_1_update,
  &oem_1_generate,
  &oem_1_check_config
};
