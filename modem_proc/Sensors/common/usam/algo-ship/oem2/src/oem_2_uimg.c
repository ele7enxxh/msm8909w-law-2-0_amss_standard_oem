/*============================================================================
  @file oem_2_uimg.c

  @brief
  Implementation of the OEM_2, custom algorithm template with SAM 2.0,
  with uImage support.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "oem_2.h"
#include "fixed_point.h"
#include "sns_debug_str.h"
#include "sns_memmgr.h"
#include "sns_sam_algo_api.h"
#include "sns_sam_algo_utils.h"
#include "sns_oem_2_v01.h"
#include "sns_em.h"
#include "location_service_v02.h"

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
sns_sam_err oem_2_update(
  sns_sam_algo_state *algoState,
  sns_sam_algo_callback const *cbFunc,
  sns_sam_algo_persist *persistData )
{
  oem_2_input_s const *input;
  sns_sam_algo_input algoInputBuf;
  oem_2_state_s *state = (oem_2_state_s*)algoState->buf;

  for( algoInputBuf = cbFunc->sns_sam_algo_get_input( cbFunc->algoHndl, NULL );
       NULL != (void*)algoInputBuf.data;
       algoInputBuf = cbFunc->sns_sam_algo_get_input( cbFunc->algoHndl, NULL ))
  {
    input = (oem_2_input_s*)algoInputBuf.data;

    oem_2_output_s output;

    output.sample.dataLen = input->sample.dataLen;
    SNS_OS_MEMCOPY( output.sample.data, input->sample.data, output.sample.dataLen );
    output.sample.inputType = input->sample.inputType;
    output.sample.timestamp = algoInputBuf.timestamp;

    state->sample.dataLen = input->sample.dataLen;
    SNS_OS_MEMCOPY( state->sample.data, input->sample.data, state->sample.dataLen );
    state->sample.inputType = input->sample.inputType;
    state->sample.timestamp = algoInputBuf.timestamp;

    cbFunc->sns_sam_algo_add_output( cbFunc->algoHndl, &(sns_sam_algo_input)
      { .timestamp = 0, .data = (intptr_t)&output } );
  }

  return SAM_ENONE;
}

/**
 * See sns_sam_algo_generate
 */
sns_sam_err oem_2_generate(
  sns_sam_algo_state *algoState,
  sns_sam_algo_callback const *cbFunc,
  sns_sam_algo_output *outputData )
{
  UNREFERENCED_PARAMETER(cbFunc);
  oem_2_state_s *state = (oem_2_state_s*)algoState->buf;
  oem_2_output_s *output = (oem_2_output_s*)outputData->data;

  output->sample.dataLen = state->sample.dataLen;
  SNS_OS_MEMCOPY( output->sample.data, state->sample.data, output->sample.dataLen );
  output->sample.inputType = state->sample.inputType;
  output->sample.timestamp = state->sample.timestamp;

  return 0 == output->sample.timestamp ? SAM_ESTATE : SAM_ENONE;
}

sns_sam_err oem_2_process_ind(
  sns_sam_algo_callback const *cbFunc,
  sns_sam_ind_msg const *indMsg )
{
  SNS_PRINTF_STRING_HIGH_2( SNS_SAM_DBG_MOD,
      "Received indication with SUID %x (len %i)",
      *indMsg->sensorUID, indMsg->msg.bufSize );

  if( SNS_SAM_PROV_LOC_POS_SUID == *indMsg->sensorUID)
  {
    oem_2_input_s input;
    qmiLocEventPositionReportIndMsgT_v02 *posInd =
      (qmiLocEventPositionReportIndMsgT_v02*)indMsg->msg.buf;

    input.sample.dataLen = sizeof( qmiLocEventPositionReportIndMsgT_v02 );
    SNS_OS_MEMCOPY( input.sample.data, posInd, input.sample.dataLen );

    input.sample.inputType = SNS_OEM_2_POSITION_REPORT_V01;

    cbFunc->sns_sam_algo_add_input( cbFunc->algoHndl, indMsg->sensorUID,
            &(sns_sam_algo_input){ .timestamp = sns_em_get_timestamp(),
            .data = (intptr_t)&input } );
  }
  else if( SNS_SAM_PROV_LOC_SV_INFO_SUID == *indMsg->sensorUID )
  {
    oem_2_input_s input;
    qmiLocEventGnssSvInfoIndMsgT_v02 *gnssInd =
      (qmiLocEventGnssSvInfoIndMsgT_v02*)indMsg->msg.buf;

    input.sample.dataLen = sizeof(gnssInd->altitudeAssumed) +
      sizeof(gnssInd->svList_valid) + sizeof(gnssInd->svList_len) +
      10 * sizeof(qmiLocSvInfoStructT_v02);

    gnssInd->svList_len = gnssInd->svList_len >= 10 ? 10 : gnssInd->svList_len;
    SNS_OS_MEMCOPY( input.sample.data, gnssInd, input.sample.dataLen );

    input.sample.inputType = SNS_OEM_2_GNSS_SV_INFO_TYPE_V01;

    cbFunc->sns_sam_algo_add_input( cbFunc->algoHndl, indMsg->sensorUID,
            &(sns_sam_algo_input){ .timestamp = sns_em_get_timestamp(),
            .data = (intptr_t)&input } );
  }

  return SAM_ENONE;
}

sns_sam_err oem_2_gen_ind(
  sns_sam_algo_callback const *cbFunc,
  sns_sam_algo_output const *outputData,
  uint8_t extInstanceID,
  sns_sam_ind_msg *indMsg)
{
  oem_2_output_s *output = (oem_2_output_s*)outputData->data;
  sns_oem_2_report_ind_msg_v01 *ind_msg =
    (sns_oem_2_report_ind_msg_v01*)indMsg->msg.buf;

  ind_msg->timestamp = output->sample.timestamp;

  ind_msg->data_valid = true;
  ind_msg->data_len = output->sample.dataLen;
  SNS_OS_MEMCOPY( ind_msg->data, output->sample.data, output->sample.dataLen );

  ind_msg->request_type = output->sample.inputType;

  return 0 == ind_msg->timestamp ? SAM_ESTATE : SAM_ENONE;
}

/*===========================================================================
  Public Definitions
  ===========================================================================*/

/**
* OEM_2 Algorithm message API
*
* See sns_sam_algo_msg_api
*/

sns_sam_algo_msg_api oem_2_algo_msg_api =
{
  sizeof(sns_sam_algo_msg_api),
  &oem_2_mem_const_req,
  &oem_2_get_attr,
  &oem_2_dep_sensors_met,
  NULL,
  &oem_2_dep_sensors,
  &oem_2_reg_grps,
  &oem_2_process_enable_req,
  NULL,
  &oem_2_process_ind,
  &oem_2_gen_ind,
  NULL,
  &oem_2_gen_report,
  &oem_2_gen_req,
  &oem_2_check_sensor
};

/**
* OEM_2 Algorithm API
*
* See sns_sam_algo_api
*/
sns_sam_algo_api oem_2_algo_api =
{
  sizeof(sns_sam_algo_api),
  &oem_2_mem_req,
  &oem_2_reset,
  &oem_2_update,
  &oem_2_generate,
  &oem_2_check_config
};
