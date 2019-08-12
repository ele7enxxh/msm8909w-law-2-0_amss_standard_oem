
/*!
@file
  rfca_handler_wcdma.cpp

@brief
  Functions to trasfer WCDMA RFCA commands to FTM commands and vice-versa.

*/

/*==============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca_handler_wcdma.cpp#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/31/14   aki     Flip the order of set sec chain and set chan in RX
01/28/14   aki     Hold LNA and SEC CHAIN change, more testing is needed
01/14/14   aki     LNA not set for RX verification functions,
                   Diversity bundled with segment time for RSSI and IQ
01/03/14   aki     Added EXIT_REQ for TXLO
12/16/13   aki     Fix input validation in tx_align
12/13/13   aki     Remove unused variables
12/12/13   krg     Fix compiler warnings
12/09/13   aki     Split RF config from RX config
12/03/13   aki     RX sweep set LNA for RX wait and fix RX flag for align and
                   special handling for TXLO missing diversity case
11/19/13   aki     Added TX align
11/14/13   aki     Updated version of antenna switching
11/14/13   aki     Updated config and tune time for RX WAIT
09/13/13   aki     Added ant switch support
08/05/13   aki     Add IQ functions
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/18/13   aki     Added rfca_common_msm_get_hw_timer_periodicty
06/10/13   aki     Mainline RFCA 
06/06/13   aki     Updates to remove KW warnings and errors
06/04/13   aki     Support variable length op-codes
05/24/13   aki     Add support for STEP_TIME_10X align config pattern
05/24/13   aki     Move LNA set into pre-exec segment
05/02/13   aki     Added new WAIT_V2 functions
04/29/13   aki     Add LNA set back to RSSI and WAIT with Pre-Exec config time 
                   and restored config and tune times for RSSI
04/10/13   aki     Use UL channel or RX wait sweep
04/04/13   aki     In TX DBM merge subsequent steps with same power to one
02/21/13   aki     RX RSSI includes all needed actions in the segment time and
                   TX DBM does not have fixed RF config or RF tune times
02/01/13   aki     Updated state cache band and channel, support multi-band sweeps
01/30/13   aki     RX ALIGN includes all needed actions in the segment time 
01/16/13   aki     Added support for RX in V3 sweep context
12/27/12   aki     No more seq wait after XO coarse
12/14/12   aki     Updated freq adj calculation
12/12/12   aki     Remove TX OFF from coarse XO
12/10/12   aki     Updated XO 
11/02/12   aki     Always use FTM_WCDMA_C to set channel
11/01/12   aki     Use lower byte of config pattern to set LNA for XO
10/31/12   aki     Timing config data, moved inital seq timing marks to top
10/03/12   aki     Execution timing updates 
10/02/12   aki     Remove RFCA_QMSL ifdefs 
09/04/12   aki     Frequency calculation to frequency adjustment functionality
08/28/12   aki     Added FREQ_ADJ to TX_DBM
08/16/12   aki     Remove compiler warnings
07/23/12   aki     TX action and HDET fixes
07/13/12   aki     ftmModeId changes, mode sanity check, config HDET avgs
05/25/12   aki     Rx Sweep, set initial level for DBM function
04/24/12   aki     Code cleanup, ENH sweep, XO sample collect, WAIT & RSSI
                   set the LNA range
02/24/12   aki     Comments, less magic numbers, RX wait timing fix
02/08/12   aki     Cleanup of type casts
02/08/12   aki     Added XO support
01/25/12   aki     Diag V2 delayed response implementation, update to booleans
10/28/11   aki     Initial revision

==============================================================================*/


#include "rfa_variation.h"
#include "comdef.h"
#include "stringl.h" /* for memscpy */

#include "rfca_hal_msg_def.h"
#include "rfca_state_cache.h"
#include "rfca_handler.h"
#include "rfca_helper.h"
#include "rfca_mem_helper.h"
#include "rfca_pending_message_queue.h"
#include "rfca_ftm_package_helper.h"
#include "rfca_defs.h"

#include "ftm.h"
#include "ftm_common_dispatch.h"
#include "ftm_command_id.h"
#include "ftm_sequencer_definitions.h"
#include "ftm_common_calibration_v3.h"

#include "rfca_to_ftm_adapter.h"

WcdmaRfcaTimeConfigData WcdmaRfcaTimeConfig = 
{
// conf   tune   exec                  pre-ex
  {50000, 25000, 10000 /* not used */, 10000}, //RX_WAIT;
  {30000, 10000, 10000 /* not used */, 10000}, //RX_RSSI;
  {30000, 10000, 10000 /* not used */, 10000}, //RX_IQ;
};

/*----------------------------------------------------------------------------*/
/*!  
  @brief Get FTM mode for diversity

  @return FTM MODE ID

*/
/*!  
  @brief Get diversity FtmMode

  @return ftmMode to be used with diversity, if rxPath is invalid then FTM_WCDMA_RX_2_C
          is returned

*/
uint16 GetWcdmaFtmCmdIdDivPath(uint16 rxPath)
{
  if((RF_SIGNAL_PATH_MAIN_2ND_TRANSCVR == rxPath) || (RF_SIGNAL_DIVERSITY_2ND_TRANSCVR == rxPath))
  {
    return FTM_WCDMA_RX_4_C;
  }
  return FTM_WCDMA_RX_2_C;
}

/*----------------------------------------------------------------------------*/
void RFCA_wcdma_rx_align_receive(unsigned int rfcaID)
{
  
  PendingRfcaResponse * rfcaResp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_RX_ALIGN_RESP* hal_resp = ((HAL_RF_WCDMA_RX_ALIGN_RESP*)rfcaResp->HAL.HalResponse);
  HAL_RF_WCDMA_RX_ALIGN_REQ* hal_req = ((HAL_RF_WCDMA_RX_ALIGN_REQ*)rfcaResp->HAL.HalRequest);

  // By default we fail
  hal_resp->status = HAL_RF_STATUS_FAILED;
  hal_resp->data_length = 0;

  // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = rfcaResp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    uint16 respMsgId = 0xFFFF;
    if(false == ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &respMsgId))
    {
      return;
    }

    switch(respMsgId)
    {
      case _FTM_GET_LNA_OFFSET:
      {
        hal_resp->data_length = 1;
        if(false == ftmItem->Packet.GetResponseWordFromOffset(FTM_LNA_OFFSET, &hal_resp->data_array[0]))
        {
          return;
        }
        hal_resp->status = HAL_RF_STATUS_OK;
        return;
      }
      break;
      case _FTM_GET_DVGA_OFFSET:
      {
        hal_resp->data_length = 1;
        if(false == ftmItem->Packet.GetResponseWordFromOffset(FTM_DVGA_OFFSET, &hal_resp->data_array[0]))
        {
          return;
        }
        hal_resp->status = HAL_RF_STATUS_OK;
        return;
      }
      break;
      case _FTM_XO_CALIBRATION:
      {
        if(ftmItem->Packet.GetResponseLength() < (RFCA_XO_DATA_ARRAY_SIZE * sizeof(uint16)))
        {
          return;
        }

        hal_resp->data_length = RFCA_XO_DATA_ARRAY_SIZE;

        // Sizes are from 80-VA888-1
        memscpy(&hal_resp->data_array[RFCA_XO_STATE_OFFSET], FTM_XO_STATE_SIZE,
                &(ftmItem->Packet.GetResponseDataPtr()[FTM_XO_STATE_OFFSET]), FTM_XO_STATE_SIZE);
        memscpy(&hal_resp->data_array[RFCA_XO_NOM_COEF_OFFSET], FTM_XO_NOM_COEF_SIZE,
                &(ftmItem->Packet.GetResponseDataPtr()[FTM_XO_NOM_COEF_OFFSET]), FTM_XO_NOM_COEF_SIZE);
        memscpy(&hal_resp->data_array[RFCA_XO_CAL_COEF_OFFSET], FTM_XO_CAL_COEF_SIZE, 
                &(ftmItem->Packet.GetResponseDataPtr()[FTM_XO_CAL_COEF_OFFSET]), FTM_XO_CAL_COEF_SIZE);
        memscpy(&hal_resp->data_array[RFCA_XO_CAL_INFO_OFFSET], FTM_XO_CAL_INFO_SIZE,
                &(ftmItem->Packet.GetResponseDataPtr()[FTM_XO_CAL_INFO_OFFSET]), FTM_XO_CAL_INFO_SIZE);

        // Check the Collect sample first
        if(XO_CAL_COLLECT & hal_req->config_pattern)
        {
          if(FTM_FT_COLLECT_SAMPLE_PASS == hal_resp->data_array[RFCA_XO_STATE_OFFSET])
          {
            hal_resp->status = HAL_RF_STATUS_OK;
          }
        }
        else if(XO_CAL_SELECT & hal_req->config_pattern)
        { 
          // Fine XO
          if(FTM_FT_CURVE_STATE_PASS == hal_resp->data_array[RFCA_XO_STATE_OFFSET])
          {
            hal_resp->status = HAL_RF_STATUS_OK;
          }
        }
        else 
        { 
          // Coarse XO
          if(FTM_XO_COARSE_STATE_PASS == hal_resp->data_array[RFCA_XO_STATE_OFFSET])
          {
            hal_resp->status = HAL_RF_STATUS_OK;
          }
        }

        return;
      }
      break;
      case _FTM_CALIBRATION_SWEEP: /* _FTM_CFG_AND_EXEC_CAL_SWEEP */
      {
        const uint16 statusOffset = 10;
        char status = -127;

        if(ftmItem->Packet.GetResponseLength() < ENH_SWEEP_RESP_SIZE)
        {
          HalLogWrite("_FTM_CALIBRATION_SWEEP response is too short, expecting 14 got %d\n", ftmItem->Packet.GetResponseLength());
        }
        byte* rawResponse = ftmItem->Packet.GetResponseDataPtr();
        status = rawResponse[statusOffset];
        if(FTM_CALIBRATION_PACKET_ALLOCATED == status)
        {
          hal_resp->status = HAL_RF_STATUS_OK;
          return;
        }
        else
        {
          HalLogWrite("_FTM_CALIBRATION_SWEEP failed with status code %d\n", status);
        }
      }
      break;
      default:
        break;
    }
    ftmItem = ftmItem->NextItem;
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_wcdma_rx_align_send(HAL_RF_WCDMA_RX_ALIGN_REQ *data)
{  

  uint16 responseDataArraySize = 1; // uint16
  uint16 config_pattern_LB = data->config_pattern & 0x00FF;
  if(XO_CAL_ENABLE & data->config_pattern)
  {
    responseDataArraySize = RFCA_XO_DATA_ARRAY_SIZE;
  }

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_WCDMA_RX_ALIGN_REQ), sizeof(HAL_RF_WCDMA_RX_ALIGN_RESP) + responseDataArraySize * sizeof(uint16), HAL_RF_WCDMA_RX_ALIGN_RESP_ID, RFCA_wcdma_rx_align_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_RX_ALIGN_RESP* halResp = (HAL_RF_WCDMA_RX_ALIGN_RESP*)rfcaResp->HAL.HalResponse;
  
  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->data_length = 0;

  if(HAL_RF_PROTOCOL_SYSTEM_WCDMA != StateCache.RF_TECHNOLOGY)
  {
    halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 ftmModeId = FTM_MODE_MAX_C;
  if(RFCA_FUNCTION_FAIL == GetFtmModeIdV2(StateCache.RF_TECHNOLOGY, data->rf_signal_path, &ftmModeId))
  {
    halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 currentBand = InvalidStateCacheValue;

  uint32 segmentLength = (uint16)(data->duration/wcdmaTimeToUs);

  uint16 ulChannel = WCDMA_CalculateChannel((HAL_RF_WCDMA_BAND)data->rf_band, data->channel, DL_CHANNEL);
  if(0==ulChannel)
  {
    HalLogWrite("Invalid channel %d.", GetDualCarrierSecondChannel());
    ((HAL_RF_WCDMA_RX_ALIGN_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 ulSecondChannel = ulChannel;
  if( config_pattern_LB & DUAL_CARRIER )
  {
    // Convert to uplink channel
    ulSecondChannel = WCDMA_CalculateChannel((HAL_RF_WCDMA_BAND)data->rf_band, GetDualCarrierSecondChannel(), DL_CHANNEL);
    if(0==ulSecondChannel)
    {
      HalLogWrite("Invalid channel %d for dual carrier.", GetDualCarrierSecondChannel());
      halResp->status = HAL_RF_STATUS_FAILED;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }
  }

  // Convert Q8 number to double
  double expected_level_dbm = int16_q8_to_double(data->expected_level);
  // Convert dBm to AGC
  int16 expected_level_agc = (int16)(10 * expected_level_dbm + 548);

  if(false == GetUseOldFtmForRxFlag())
  {
    // USE SWEEP   
    FTM_Cal_Sweep_Opcode_Req_List_Item *head = NULL;
    FTM_Cal_Sweep_Opcode_Req_List_Item *tail = NULL;

    if(StateCache.BAND != data->rf_band)
    {
      AddSweepConfig(&head, &tail, data->rf_band, ulChannel, ulSecondChannel-ulChannel, 3, 3, data->rf_signal_path, enhSweepSegmentLength.rfConfig);      
    }
    else if(StateCache.CHANNEL != ulChannel)
    {
      AddSweepTune(&head, &tail, ulChannel, ulSecondChannel-ulChannel, enhSweepSegmentLength.rfTune);
    }
    {
      HAL_RF_RX_SWEEP_STEP_CONFIG rxCtrlData;
      uint16 modified_config_pattern_LB = config_pattern_LB & (~DUAL_CARRIER);
      rxCtrlData.actions = (DVGA_OFFSET == modified_config_pattern_LB)?1<<ENH_SWEEP_RX_ACTION_DVGA:1<<modified_config_pattern_LB;
      rxCtrlData.powerMode = 0;
      rxCtrlData.expectedRxAgc = expected_level_agc;
      rxCtrlData.rxFlag = (1<<GetDeviceIdFromSignalPath(data->rf_signal_path));
      AddSweepRxMeasure(&head, &tail, segmentLength, rxCtrlData);
    }
    AddSweepToFtmCmdQueue(data->hdr.operation_id, &head, &tail, true);
  }
  else
  {
    bool addSeqWait = true;
    AddFtmSeqAddTimingMarker(data->hdr.operation_id);

    // Set MODE
    if(false == RFCA_GetCurrentBand(&currentBand) || data->rf_band != currentBand)
    {

      unsigned short rfMode = -1;

      if(false == RFCA_ModeToFtmRfMode(&rfMode, StateCache.RF_TECHNOLOGY, data->rf_band))
      {
        halResp->status = HAL_RF_STATUS_FAILED;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
        return;
      }

      AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);     
      RFCA_StateCacheSetBand(&StateCache, data->rf_band);

    }

    // Set CHANNEL
    // Convert to uplink channel
    if( config_pattern_LB & DUAL_CARRIER )
    {    
      if((StateCache.CHANNEL != ulChannel) || (StateCache.WCDMA.SECOND_CHANNEL != ulSecondChannel))
      {
        // DUAL carrier case
        AddFtmSetMultiChan(data->hdr.operation_id, FTM_WCDMA_C, ulChannel, ulSecondChannel);
        StateCache.WCDMA.SECOND_CHANNEL= ulSecondChannel;
        StateCache.CHANNEL= ulChannel;
      }
    }
    else
    {
      if(StateCache.CHANNEL != ulChannel)
      {
        // Always use MAIN PATH FTM MODE ID otherwise deiversity channel change does not happen
        AddFtmSetChan(data->hdr.operation_id, FTM_WCDMA_C, ulChannel); 
        StateCache.CHANNEL= ulChannel;
      }
    }

    // DIVERSITY
    if(StateCache.WCDMA.SIGNAL_PATH != data->rf_signal_path)
    {    
      AddFtmSetSecondaryChain(data->hdr.operation_id, GetWcdmaFtmCmdIdDivPath(data->rf_signal_path), GetSecChainEnabledFromSignalPath(data->rf_signal_path));
      StateCache.WCDMA.SIGNAL_PATH = data->rf_signal_path;
    }

    // Check XO first
    if(XO_CAL_ENABLE & data->config_pattern)
    {
      word newLna = defaultLnaRangeForXo;
      // Special handling for now, use defaultLnaRangeForXo as default in case nothing is set
      // This also means that LNA 0 cannot be used
      if(0 != config_pattern_LB)
      {
        newLna = config_pattern_LB;        
      }
      AddFtmSetLnaRange(data->hdr.operation_id, ftmModeId, newLna);
      // For XO WCDMA MAIN PATH FTM_WCDMA_C=1 is used for all commands
      currentXoParameters.technology = XO_TECH_WCDMA;
      currentXoParameters.rx_freq_khz = (uint32)(WCDMA_CalculateFrequency((HAL_RF_WCDMA_BAND)data->rf_band, data->channel, false) * 1000);

      // Check the Collect sample first
      if(XO_CAL_COLLECT & data->config_pattern)
      {
        AddFtmXoCal(data->hdr.operation_id, XO_COLLECT_SAMPLES, XO_VERSION);
      }
      else if(XO_CAL_SELECT & data->config_pattern)
      { 
        // Fine XO
        AddFtmXoCal(data->hdr.operation_id, XO_FT_CURVE, XO_VERSION);
      }
      else 
      { 
        // Coarse XO
        AddFtmXoCal(data->hdr.operation_id, XO_COARSE_CAL, XO_VERSION);     
        addSeqWait = false;
      }
    }
    else 
    {

      // Exclude Dual Carrier bit
      config_pattern_LB = (config_pattern_LB & (~DUAL_CARRIER));
      if(LNA_OFFSET_5 >= config_pattern_LB) // Exclude Dual Carrier bit
      {
        // Do LNA offset
        AddFtmGetLnaOffset(data->hdr.operation_id, ftmModeId, config_pattern_LB, expected_level_agc ); 
      }
      else if(DVGA_OFFSET == config_pattern_LB)
      {
        AddFtmGetDvgaOffset(data->hdr.operation_id, ftmModeId, expected_level_agc);
      }
      else
      {
        // Invalid value
        HalLogWrite("Invalid value (%d) for DVGA or LNA reading\n", config_pattern_LB);
        halResp->status = HAL_RF_STATUS_FAILED;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
        return;
      }
    }
    if( true == addSeqWait )
    {
      if(STEP_TIME_10X & data->config_pattern)
      {
        segmentLength *= 10;
      }
      AddFtmSeqAddWait(data->hdr.operation_id, segmentLength);
    }
  }
  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  }
  else
  {
    // Send ACK delayed response will be sent later
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_wcdma_tx_align_receive(unsigned int rfcaID)
{
  PendingRfcaResponse * rfcaResp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_TX_ALIGN_RESP *hal_resp = (HAL_RF_WCDMA_TX_ALIGN_RESP*)rfcaResp->HAL.HalResponse;

  // By default we fail
  hal_resp->status = HAL_RF_STATUS_FAILED;

  // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = rfcaResp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    word subSysCmdId = 0xFFFF;
    ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &subSysCmdId);
    switch(subSysCmdId)
    {      
      case FTM_DO_RSB_TXLO_CAL:
      {
        const uint16 statusOffset = 10;
        const uint16 calStatusOffset = 12;
        const signed char statusPass = 1;


        byte *rawResponse = ftmItem->Packet.GetResponseDataPtr();
        signed char status = rawResponse[statusOffset];       
        signed char calStatus = rawResponse[calStatusOffset];

        if(statusPass != status || statusPass != calStatus)
        {
          switch(calStatus)
          {
          case FTM_RSB_IQ_CAP_FAILURE:
            hal_resp->status = HAL_RF_STATUS_RSB_IQ_CAP_FAILURE;
            break;
          case FTM_RSB_LM_CON_FAILURE:
            hal_resp->status = HAL_RF_STATUS_RSB_LM_CON_FAILURE;
            break;
          case FTM_TXLO_IQ_CAP_FAILURE:
            hal_resp->status = HAL_RF_STATUS_TXLO_IQ_CAP_FAILURE;
            break;
          case FTM_TXLO_LM_CON_FAILURE:
            hal_resp->status = HAL_RF_STATUS_TXLO_LM_CON_FAILURE;
            break;
          case FTM_UNSUPPORTED_BAND:
            hal_resp->status = HAL_RF_STATUS_UNSUPPORTED_BAND;
            break;
          default:
            MSG_ERROR("Unknown error code %d from TXLO/RSB", calStatus, 0, 0);
            hal_resp->status = HAL_RF_STATUS_FAILED;
            break;
          }
       }
       else
       {
          hal_resp->status = HAL_RF_STATUS_OK;
       }
       return;
      }
      break;
      default:
        break;
    }
    ftmItem = ftmItem->NextItem;
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_wcdma_tx_align_send(HAL_RF_WCDMA_TX_ALIGN_REQ *data)
{
  uint16 requestSpecificSize = 0;
  bool invalidConfigPattern = false;
  switch(data->config_pattern & (0x00FF))
  {
  case HAL_RF_TX_ALIGN_CONFIG_TXLO_RSB:
    requestSpecificSize = 0;
    break;
  default:
    invalidConfigPattern = true;
    break;
  }

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_WCDMA_TX_ALIGN_REQ), sizeof(HAL_RF_WCDMA_TX_ALIGN_RESP) + requestSpecificSize , HAL_RF_WCDMA_TX_ALIGN_RESP_ID, RFCA_wcdma_tx_align_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_TX_ALIGN_RESP* halResp = (HAL_RF_WCDMA_TX_ALIGN_RESP*)rfcaResp->HAL.HalResponse;
  halResp->status = HAL_RF_STATUS_FAILED;

  if(HAL_RF_PROTOCOL_SYSTEM_WCDMA != StateCache.RF_TECHNOLOGY)
  {
    halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  if(true == invalidConfigPattern)
  {
    MSG_ERROR("Invalid TX align config_pattern %d", data->config_pattern, 0, 0);
    halResp->status = HAL_RF_STATUS_ARGUMENT_ERROR;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  AddFtmSeqAddTimingMarker(data->hdr.operation_id);

  uint16 ftmModeId = FTM_MODE_MAX_C;
  if(RFCA_FUNCTION_FAIL == GetFtmModeIdV2(StateCache.RF_TECHNOLOGY, GetMainPathFromActiveRfDevice(), &ftmModeId))
  {
    halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  unsigned short rfMode = -1;
  if(false == RFCA_ModeToFtmRfMode(&rfMode, StateCache.RF_TECHNOLOGY, data->rf_band))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
    return;
  }

  uint32 segmentLength = (uint16)(data->duration/wcdmaTimeToUs);
  if(STEP_TIME_10X & data->config_pattern)
  {
    segmentLength *= 10;
  }

  byte divDevice = GetDeviceIdFromSignalPath(GetDivPathFromActiveRfDevice());
  if(RSB_NO_DIV & data->config_pattern)
  {
    divDevice = 255;
  }

  byte exitRequired = 1;
  if(NO_EXIT_REQ & data->config_pattern)
  {
    exitRequired = 0;
  }

  AddFtmDoRsbTxloCal(data->hdr.operation_id, rfMode, GetDeviceIdFromSignalPath(GetMainPathFromActiveRfDevice()), divDevice, exitRequired);
  AddFtmSeqAddWait(data->hdr.operation_id, segmentLength);

  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  }
  else
  {
    // Send ACK delayed response will be sent later
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_wcdma_tx_dac_receive(unsigned int rfcaID)
{
  PendingRfcaResponse *rfcaResp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_TX_DAC_RESP* hal_resp = ((HAL_RF_WCDMA_TX_DAC_RESP*)rfcaResp->HAL.HalResponse);
  HAL_RF_WCDMA_TX_DAC_REQ*  hal_req = ((HAL_RF_WCDMA_TX_DAC_REQ*)rfcaResp->HAL.HalRequest);

  // By default we fail
  hal_resp->status = HAL_RF_STATUS_FAILED;
  
  // Assuming it worked if we got all reasponses back.
  hal_resp->rf_band = hal_req->rf_band;
  hal_resp->channel = hal_req->channel;
  
  // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = rfcaResp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    uint16 respFtmModeId = 0xFFFF;
    ftmItem->Packet.GetResponseWordFromOffset(FTM_MODE_ID_OFFSET, &respFtmModeId);

    uint16 respFtmCmdId = 0xFFFF;
    ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &respFtmCmdId);

    switch(respFtmModeId)
    {
    case FTM_SEQ_C:
      {
        switch(respFtmCmdId)
        {
        case _FTM_SEQ_ADD_WAIT:
          {
          }
          break;
        default:
          break;
        }
      }
      break;
    case FTM_COMMON_C:
      {
        switch(respFtmCmdId)
        {
        case _FTM_CALIBRATION_SWEEP: /* _FTM_CFG_AND_EXEC_CAL_SWEEP */
          {
            const uint16 statusOffset = 10;
            char status = -127;

            if(ftmItem->Packet.GetResponseLength() < ENH_SWEEP_RESP_SIZE)
            {
              HalLogWrite("_FTM_CALIBRATION_SWEEP response is too short, expecting 14 got %d\n", ftmItem->Packet.GetResponseLength());
            }
            byte* rawResponse = ftmItem->Packet.GetResponseDataPtr();
            status = rawResponse[statusOffset];
            if(FTM_CALIBRATION_PACKET_ALLOCATED == status)
            {
              hal_resp->status = HAL_RF_STATUS_OK;
              return;
            }
            else
            {
              HalLogWrite("_FTM_CALIBRATION_SWEEP failed with status code %d\n", status);
            }
          }
        break;
        default:
        break;
        }
      }
      break;
    default:
      break;
    }
    ftmItem = ftmItem->NextItem;
  }
}


/*----------------------------------------------------------------------------*/
void RFCA_wcdma_tx_dac_send(HAL_RF_WCDMA_TX_DAC_REQ *data)
{

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_WCDMA_TX_DAC_REQ), sizeof(HAL_RF_WCDMA_TX_DAC_RESP), HAL_RF_WCDMA_TX_DAC_RESP_ID, RFCA_wcdma_tx_dac_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_TX_DAC_RESP* halResp = (HAL_RF_WCDMA_TX_DAC_RESP*)rfcaResp->HAL.HalResponse;

  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->channel = data->channel;
  halResp->rf_band = data->rf_band;

  // Fail right away if needed
  if(HAL_RF_PROTOCOL_SYSTEM_WCDMA != StateCache.RF_TECHNOLOGY)
  {
    halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 ftmModeId = FTM_WCDMA_C;
  if(RFCA_FUNCTION_FAIL == GetFtmModeId(GetActiveRfDevice(), StateCache.RF_TECHNOLOGY, RF_SIGNAL_PATH_MAIN, &ftmModeId))
  {
    halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 segmentLength = (uint16)(data->step_duration/wcdmaTimeToUs);

  {

    uint32 totalSweepQueueSize = 0;
    uint16 number_of_config_steps = 0;
    FTM_Cal_Sweep_Opcode_Req_List_Item *head = NULL;
    FTM_Cal_Sweep_Opcode_Req_List_Item *tail = NULL;

    int32 timeLeftToDoNothinig = 0;

    // Check if the phone is already in correct mode
    uint16 currentBand = InvalidStateCacheValue;
    unsigned short rfMode = -1;
    if(false == RFCA_GetCurrentBand(&currentBand) || data->rf_band != currentBand)
    {
      if(false == RFCA_ModeToFtmRfMode(&rfMode, StateCache.RF_TECHNOLOGY, data->rf_band))
      {
        halResp->status = HAL_RF_STATUS_FAILED;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
        return;
      }

      RFCA_StateCacheSetBand(&StateCache, data->rf_band);

      // Add config OP
      {
        FTM_Cal_Sweep_Opcode_Req_List_Item *configItem = NULL;
        const uint16 configItemSize = sizeof(FTM_Cal_Sweep_Opcode_Req_List_Item) + sizeof(FTM_Cal_Sweep_Cfg_Req);
        configItem = (FTM_Cal_Sweep_Opcode_Req_List_Item *)RFCA_Malloc(configItemSize);
        if(NULL == configItem)
        {
          HalLogWrite("Could not allocate memory for sweep config item");
          halResp->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
          return;
        }
        totalSweepQueueSize += configItemSize;
        number_of_config_steps++;
        configItem->next = NULL;
        configItem->totalSize = configItemSize;

        ResetOpcode(&(configItem->opCodeHeader));
        SetFtmCalSweepOpCodeBit(&(configItem->opCodeHeader), FTM_CAL_OPCODE_CONFIG_RADIO);        

        configItem->opCodeHeader.segLen = (word)((enhSweepSegmentLength.rfConfig>rfca_common_msm_get_hw_timer_periodicty())?rfca_common_msm_get_hw_timer_periodicty():enhSweepSegmentLength.rfConfig);

        FTM_Cal_Sweep_Cfg_Req* configPayload = (FTM_Cal_Sweep_Cfg_Req*)(&configItem->opCodeHeader.opCodePayload);
        configPayload->masterDev = GetMasterDeviceMask();
        configPayload->slaveDev = 0xFFFFFFFF;
        configPayload->rfmode = (byte)rfMode;
        configPayload->ulChan = data->channel;
        configPayload->txBW = 3;
        configPayload->rxBW = 3;
        configPayload->chanOffset = 0;

        StateCache.CHANNEL = data->channel;

        timeLeftToDoNothinig = enhSweepSegmentLength.rfConfig - configItem->opCodeHeader.segLen;
        head = configItem;
        tail = configItem;
      }
    }
    else
    {
      if(data->channel != StateCache.CHANNEL)
      {
        // Add retune OP
        FTM_Cal_Sweep_Opcode_Req_List_Item *configItem = NULL;
        const uint16 configItemSize = sizeof(FTM_Cal_Sweep_Opcode_Req_List_Item) + sizeof(FTM_Cal_Sweep_Retune_Req) ;
        configItem = (FTM_Cal_Sweep_Opcode_Req_List_Item *)RFCA_Malloc(configItemSize);
        if(NULL == configItem)
        {
          HalLogWrite("Could not allocate memory for sweep retune item");
          halResp->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
          return;
        }
        totalSweepQueueSize += configItemSize;
        number_of_config_steps++;
        configItem->next = NULL;
        configItem->totalSize = configItemSize;

        ResetOpcode(&(configItem->opCodeHeader));
        SetFtmCalSweepOpCodeBit(&(configItem->opCodeHeader), FTM_CAL_OPCODE_RETUNE_RADIO);
        configItem->opCodeHeader.segLen = (enhSweepSegmentLength.rfTune>rfca_common_msm_get_hw_timer_periodicty())?rfca_common_msm_get_hw_timer_periodicty():enhSweepSegmentLength.rfTune;

        FTM_Cal_Sweep_Retune_Req* configPayload = (FTM_Cal_Sweep_Retune_Req*)(&configItem->opCodeHeader.opCodePayload);
        configPayload->ulChan = data->channel;
        configPayload->chanOffset = 0;
        configPayload->masterDev = GetMasterDeviceMask();
        timeLeftToDoNothinig = enhSweepSegmentLength.rfTune - configItem->opCodeHeader.segLen;
        head = configItem;
        tail = configItem;

        StateCache.CHANNEL = data->channel;

      }
      else
      {
        head = NULL;
        tail = NULL;
      }

    }

    // Add no OP
    if(RFCA_FUNCTION_FAIL == AddNoOpSegment(&tail, timeLeftToDoNothinig))
    {
      // Clear queue
      while(NULL != head)
      {
        tail = head->next;
        RFCA_Free(head);
        head = tail;
      }

      HalLogWrite("Could not allocate memory for sweep noOp item");
      halResp->status = HAL_RF_STATUS_FAILED;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }

    for(int step=0; step<data->number_of_steps; step++)
    {    
      {
        HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA_APT txCtrlData = ((HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA_APT *)data->tx_control_data)[step];

        uint16 txItemSize = sizeof(FTM_Cal_Sweep_Opcode_Req_List_Item) + sizeof(FTM_Cal_Sweep_TxMeas_Req);
        const byte hdetBitMask = 1<<7;  // Bit 7
        const byte lpHdetBitMask = 1<<6;  // Bit 6
        if( (hdetBitMask|lpHdetBitMask) & txCtrlData.paState )
        {
          // Add HDET reading and TX power level config
          txItemSize += sizeof(FTM_Cal_Sweep_HDET_Meas_Req);
        }

        FTM_Cal_Sweep_Opcode_Req_List_Item *txItem = (FTM_Cal_Sweep_Opcode_Req_List_Item* )RFCA_Malloc(txItemSize);

        if(NULL == txItem)
        {
          HalLogWrite("Could not allocate memory for sweep tx item");
          halResp->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  

          // TODO Need to clear all allocated items from list....
          while(NULL != head)
          {
            tail = head->next;
            RFCA_Free(head);
            head = tail;
          }
          return;
        }

        totalSweepQueueSize += txItemSize;
        if(NULL != tail)
        {
          tail->next = txItem;
        }
        if(NULL == head)
        {
          head = txItem;
        }
        tail = txItem;
        txItem->next = NULL;
        txItem->totalSize = txItemSize;

        ResetOpcode(&(txItem->opCodeHeader));
        SetFtmCalSweepOpCodeBit(&(txItem->opCodeHeader), FTM_CAL_OPCODE_CONFIG_TX);
        txItem->opCodeHeader.segLen = segmentLength;

        FTM_Cal_Sweep_TxMeas_Req* txPayload = (FTM_Cal_Sweep_TxMeas_Req*)(&(txItem->opCodeHeader.opCodePayload[0]));
        txPayload->txAction = 0;
        txPayload->paState = txCtrlData.paState & (~(hdetBitMask|lpHdetBitMask));
        txPayload->rgi = txCtrlData.rgi;
        txPayload->paBias = txCtrlData.paBias;
        txPayload->txFlag = GetTxFlag();

        if( (hdetBitMask|lpHdetBitMask) & txCtrlData.paState )
        {
          SetFtmCalSweepOpCodeBit(&(txItem->opCodeHeader), FTM_CAL_OPCODE_MEAS_HDET);
          FTM_Cal_Sweep_HDET_Meas_Req* hdetPayload = (FTM_Cal_Sweep_HDET_Meas_Req*)(&(txItem->opCodeHeader.opCodePayload[sizeof(FTM_Cal_Sweep_TxMeas_Req)]));
          hdetPayload->numHdetAvg = 0;
          hdetPayload->numLpmHdetAvg = 0;
          if( hdetBitMask & txCtrlData.paState )
          {
            hdetPayload->numHdetAvg = numberOfHdetAvgerages_wcdma;
          }
          if( lpHdetBitMask & txCtrlData.paState )
          {
            hdetPayload->numLpmHdetAvg = numberOfHdetAvgerages_wcdma;
          }
          hdetPayload->hdetFlag = GetTxFlag();
        }
      }
    }

    
    if(NULL != head)
    {
      // Need to split into multiple requests
      FTM_Cal_Sweep_Opcode_Req_List_Item *splitHead = head;
      FTM_Cal_Sweep_Opcode_Req_List_Item *splitTail = head;
      FTM_Cal_Sweep_Opcode_Req_List_Item *nextSplitHead = head->next;

      const uint16 doubleHeaderSize = 12 + 19; // FTM_SEQ_QUEUE_COMMANDS + FTM_CFG_AND_EXEC_CAL_SWEEP headers
      while (NULL != splitTail)
      {
        uint16 splitTotalSize = 0;
        uint16 splitNumberOfSteps = 1;
        while((NULL != nextSplitHead) && (splitTotalSize + (splitTail->next)->totalSize + doubleHeaderSize) <RFCA_PACKET_HELPER_MAX_PACKET_SIZE)   
        {
          splitTotalSize += splitTail->totalSize;
          splitNumberOfSteps++;
          nextSplitHead = nextSplitHead->next;
          splitTail = splitTail->next;
        }

        // Temporarily terminate linked list
        splitTail->next = NULL;
        // More items to be sent
        AddFtmCfgAndExecCalSweep(data->hdr.operation_id, 0, 0, V3_SWEEP_ENABLE_F3, splitNumberOfSteps, splitHead);

        splitHead = nextSplitHead;
        splitTail->next = nextSplitHead;
        splitTail = nextSplitHead;
        nextSplitHead = (NULL != nextSplitHead)?(nextSplitHead->next):(NULL);
      }
    }

    while(NULL != head)
    {
      tail = head->next;
      RFCA_Free(head);
      head = tail;
    }
  }

  SetAtpSweepConfigured(true);

  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  }
  else
  {
    // Send ACK delayed response will be sent later
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }

}

/*----------------------------------------------------------------------------*/
void RFCA_wcdma_tx_dbm_receive(unsigned int rfcaID)
{
  PendingRfcaResponse *rfcaResp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_TX_DBM_RESP* hal_resp = ((HAL_RF_WCDMA_TX_DBM_RESP*)rfcaResp->HAL.HalResponse);
  HAL_RF_WCDMA_TX_DBM_REQ*  hal_req = ((HAL_RF_WCDMA_TX_DBM_REQ*)rfcaResp->HAL.HalRequest);

  hal_resp->rf_band = hal_req->rf_band;
  hal_resp->channel = hal_req->channel;

  // By default we fail
  hal_resp->status = HAL_RF_STATUS_FAILED;

    // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = rfcaResp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    word subSysCmdId = 0xFFFF;
    ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &subSysCmdId);
    switch(subSysCmdId)
    {
      case _FTM_SET_TX_POWER_DBM:
      {
        // Seems that response is same as request
        //hal_resp->status = (0 == ftmItem->Packet.GetResponseDataPtr()[6])?(HAL_RF_STATUS_OK):(HAL_RF_STATUS_FAILED);
        hal_resp->status = HAL_RF_STATUS_OK;
        return;
        break;
      }
      default:
        break;
    }
    ftmItem = ftmItem->NextItem;
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_wcdma_tx_dbm_send(HAL_RF_WCDMA_TX_DBM_REQ *data)
{

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_WCDMA_TX_DBM_REQ), sizeof(HAL_RF_WCDMA_TX_DBM_RESP), HAL_RF_WCDMA_TX_DBM_RESP_ID, RFCA_wcdma_tx_dbm_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_TX_DBM_RESP* halResp = (HAL_RF_WCDMA_TX_DBM_RESP*)rfcaResp->HAL.HalResponse;
  
  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->channel = data->channel;
  halResp->rf_band = data->rf_band;

  if(HAL_RF_PROTOCOL_SYSTEM_WCDMA != StateCache.RF_TECHNOLOGY)
  {
    halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 ftmModeId = FTM_WCDMA_C;
  if(RFCA_FUNCTION_FAIL == GetFtmModeId(GetActiveRfDevice(), StateCache.RF_TECHNOLOGY, RF_SIGNAL_PATH_MAIN, &ftmModeId))
  {
    halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 segmentLength = (uint16)(data->step_duration/wcdmaTimeToUs);

  uint16 currentBand = InvalidStateCacheValue;

  // Check if the phone is already in correct mode
  if(false == RFCA_GetCurrentBand(&currentBand) || data->rf_band != currentBand)
  {
    unsigned short rfMode = -1;
     
    if(false == RFCA_ModeToFtmRfMode(&rfMode, StateCache.RF_TECHNOLOGY, data->rf_band))
    {
      halResp->status = HAL_RF_STATUS_FAILED;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }

    // Set mode, see FTM_RF_Mode_Enum
    AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);   
    RFCA_StateCacheSetBand(&StateCache, data->rf_band);
  }

  // Set channel if needed
  if(StateCache.CHANNEL != data->channel)
  {
    AddFtmSetChan(data->hdr.operation_id, FTM_WCDMA_C, data->channel); 
    StateCache.CHANNEL= data->channel;

    // Turn TX on if needed
    if(false == StateCache.TX_STATE)
    {
      AddFtmSetTxOn(data->hdr.operation_id, ftmModeId);
      RFCA_StateCacheSetTxState(&StateCache, true);    
    }
	
    // Need to reset this so that adjustment happens for each channel
    StateCache.FREQ_ADJ_APPLIED = FALSE;
  }

  uint32 combinedSegmentTime = segmentLength;
  int16 lastSegmentPower = (int16)0xFFFF; 
  AddFtmSeqAddTimingMarker(data->hdr.operation_id);

  for(int step=0; step<data->number_of_steps; step++)
  {
	if(lastSegmentPower != data->power[step])
	{
      AddFtmSetTxPowerDbm(data->hdr.operation_id, ftmModeId, true, (int16)(int16_q8_to_double(data->power[step])*10));
	  lastSegmentPower = data->power[step];
	}
	else
	{
	  combinedSegmentTime += segmentLength;
	}
    if(true == freqAdjustData.freqAdjEnabled)  
    {
      if(StateCache.FREQ_ADJ_APPLIED != TRUE)  
      {
        double centerFreqMhz = WCDMA_CalculateFrequency((HAL_RF_WCDMA_BAND)data->rf_band, data->channel, true);
        if(0.0 == centerFreqMhz)
        {
          halResp->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE); 
          return;
        }
        freqAdjustData.centerFreqHz = (uint32)(centerFreqMhz * 1000000);
        AddFtmSetFreqAdjust(data->hdr.operation_id, ftmModeId, freqAdjustData.userFreqAdjustPpb, freqAdjustData.centerFreqHz, freqAdjustData.enableXoFreqAdjust); 
        StateCache.FREQ_ADJ_APPLIED = TRUE;
      }
    }
    if( (step == data->number_of_steps-1) || lastSegmentPower != data->power[step+1])
    {
      AddFtmSeqAddWait(data->hdr.operation_id, combinedSegmentTime);
      combinedSegmentTime = segmentLength;
    }
  }

  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  }
  else
  {
    // Send ACK delayed response will be sent later
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_wcdma_rx_wait_receive(unsigned int rfcaID)
{

  PendingRfcaResponse * rfcaResp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }  

  HAL_RF_WCDMA_RX_WAIT_RESP* hal_resp = ((HAL_RF_WCDMA_RX_WAIT_RESP*)rfcaResp->HAL.HalResponse);
  HAL_RF_WCDMA_RX_WAIT_REQ* hal_req = ((HAL_RF_WCDMA_RX_WAIT_REQ*)rfcaResp->HAL.HalRequest);
  // By default we fail
  hal_resp->status = HAL_RF_STATUS_FAILED;
  hal_resp->channel = hal_req->channel;
  hal_resp->rf_band = hal_req->rf_band;

  // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = rfcaResp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    word subSysCmdId = 0xFFFF;
    if(FALSE == ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &subSysCmdId))
    {
      HalLogWrite("Bad response for RX Wait, ignoring...\n");
      continue;
    }

    switch(subSysCmdId)
    {
    case _FTM_RX_WAIT_DBM:
      {
        ftm_rx_wait_dbm_response_type *rxWaitFtmResp = (ftm_rx_wait_dbm_response_type*)ftmItem->Packet.GetResponseDataPtr();
        hal_resp->rssi_value = double_to_int16_q8((rxWaitFtmResp->rx_power/10.0));
        hal_resp->status = (FTM_RX_WAIT_DBM_SUCCESS == rxWaitFtmResp->status) ? (HAL_RF_STATUS_OK) : (HAL_RF_STATUS_FAILED);
      }
      break;
    case _FTM_RX_WAIT_DBM_V2:
      {
        ftm_rx_wait_dbm_v2_response_type *rxWaitFtmResp = (ftm_rx_wait_dbm_v2_response_type *)ftmItem->Packet.GetResponseDataPtr();
        hal_resp->rssi_value = double_to_int16_q8((rxWaitFtmResp->rx_power / 10.0));
        switch(rxWaitFtmResp->status)
        {
          case FTM_RX_WAIT_DBM_SUCCESS:
            hal_resp->status = HAL_RF_STATUS_OK;
            break;
          case FTM_RX_WAIT_DBM_TIMEOUT:
            hal_resp->status = HAL_RF_STATUS_RX_WAIT_TIMEOUT;
            break;
          default:
            hal_resp->status = HAL_RF_STATUS_FAILED;
            break;
        }
        return;
      }
      break;
    case _FTM_CALIBRATION_SWEEP: /* _FTM_CFG_AND_EXEC_CAL_SWEEP */
      {
        const uint16 statusOffset = 10;
        char status = -127;

        if(ftmItem->Packet.GetResponseLength() < ENH_SWEEP_RESP_SIZE)
        {
          HalLogWrite("_FTM_CALIBRATION_SWEEP response is too short, expecting 14 got %d\n", ftmItem->Packet.GetResponseLength());
        }
        byte *rawResponse = ftmItem->Packet.GetResponseDataPtr();
        status = rawResponse[statusOffset];
        if(FTM_CALIBRATION_PACKET_ALLOCATED == status)
        {
          hal_resp->status = HAL_RF_STATUS_OK;
          return;
        } else
        {
          HalLogWrite("_FTM_CALIBRATION_SWEEP failed with status code %d\n", status);
        }
      }
      break;
    }
    ftmItem = ftmItem->NextItem;
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_wcdma_rx_wait_send(HAL_RF_WCDMA_RX_WAIT_REQ *data)
{

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_WCDMA_RX_WAIT_REQ), sizeof(HAL_RF_WCDMA_RX_WAIT_RESP), HAL_RF_WCDMA_RX_WAIT_RESP_ID, RFCA_wcdma_rx_wait_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_RX_WAIT_RESP* halResp = (HAL_RF_WCDMA_RX_WAIT_RESP*)rfcaResp->HAL.HalResponse;
  
  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->channel = data->channel;
  halResp->rf_band = data->rf_band;
  halResp->rssi_value = (int16)0x8000;

  if(HAL_RF_PROTOCOL_SYSTEM_WCDMA != StateCache.RF_TECHNOLOGY)
  {
    halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 ftmModeId = FTM_MODE_MAX_C;
  if(RFCA_FUNCTION_FAIL == GetFtmModeIdV2(StateCache.RF_TECHNOLOGY, data->rf_signal_path, &ftmModeId))
  {
    halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 currentBand = InvalidStateCacheValue;

  unsigned short rfMode = -1;

  if(false == RFCA_ModeToFtmRfMode(&rfMode, StateCache.RF_TECHNOLOGY, data->rf_band))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  // Calculate channel
  uint16 ulChannel = WCDMA_CalculateChannel((HAL_RF_WCDMA_BAND)data->rf_band, data->channel, DL_CHANNEL);
  if(0==ulChannel)
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  int16 pwrdBm10 = (int16)(int16_q8_to_double(data->trigger_level)*10);

  if(true == GetUseOldFtmForRxFlag())
  {

    AddFtmSeqAddTimingMarker(data->hdr.operation_id);

    if(false == RFCA_GetCurrentBand(&currentBand) || data->rf_band != currentBand)
    {

      AddFtmSetMode(data->hdr.operation_id, FTM_WCDMA_C, rfMode);     
      RFCA_StateCacheSetBand(&StateCache, data->rf_band);


      AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_WAIT.rfConfig);
    }

    // Set CHANNEL
    if(StateCache.CHANNEL != ulChannel)
    {
      AddFtmSetChan(data->hdr.operation_id, FTM_WCDMA_C, ulChannel); 
      StateCache.CHANNEL= ulChannel;
      AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_WAIT.rfTune);
    }

    // DIVERSITY
    if(StateCache.WCDMA.SIGNAL_PATH != data->rf_signal_path)
    {
      AddFtmSetSecondaryChain(data->hdr.operation_id, GetWcdmaFtmCmdIdDivPath(data->rf_signal_path), GetSecChainEnabledFromSignalPath(data->rf_signal_path));
      StateCache.WCDMA.SIGNAL_PATH = data->rf_signal_path;
      if(FALSE == StateCache.CALIBRATION_STATE_ACTIVE)
      {
        AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_WAIT.rfPreExec);
      }
    }

    if(TRUE == StateCache.CALIBRATION_STATE_ACTIVE)
    {
      AddFtmSeqAddTimingMarker(data->hdr.operation_id);
      SetLnaRxRangeForRxLevel(HAL_RF_PROTOCOL_SYSTEM_WCDMA, data->hdr.operation_id, ftmModeId, data->trigger_level);
      AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_WAIT.rfPreExec);
    }

    AddFtmRxWaitDbm(data->hdr.operation_id, ftmModeId, pwrdBm10, (data->timeout * WCDMA_FRAME_LENGTH_US));
  }
  else
  {
    FTM_Cal_Sweep_Opcode_Req_List_Item *head = NULL;
    FTM_Cal_Sweep_Opcode_Req_List_Item *tail = NULL;

    if(StateCache.BAND != data->rf_band)
    {
      AddSweepConfig(&head, &tail, data->rf_band, ulChannel, 0, 3, 3, data->rf_signal_path, enhSweepSegmentLength.rfConfig);      
    }
    else if(StateCache.CHANNEL != ulChannel)
    {
      AddSweepTune(&head, &tail, ulChannel, 0, enhSweepSegmentLength.rfTune);
    }
    {
      HAL_RF_RX_SWEEP_STEP_CONFIG rxCtrlData;
      rxCtrlData.actions = (1<< GetLnaRangeForRxLevel(StateCache.RF_TECHNOLOGY, data->trigger_level));
      rxCtrlData.powerMode = 0;
      rxCtrlData.expectedRxAgc = data->trigger_level; // Does not matter for this
      rxCtrlData.rxFlag = (1<<GetDeviceIdFromSignalPath(data->rf_signal_path));
      AddSweepRxConfig(&head, &tail, WcdmaRfcaTimeConfig.RX_WAIT.rfPreExec, rxCtrlData);
    }
    AddSweepRxWait(&head, &tail, (data->timeout * WCDMA_FRAME_LENGTH_US), enhSweepSegmentLength.pollingInterval_us, pwrdBm10);
    AddSweepToFtmCmdQueue(data->hdr.operation_id, &head, &tail, true);
  }
  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
   halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  }
  else
  {
    // Send ACK delayed response will be sent later
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }
}

void RFCA_wcdma_rx_wait_v2_send(HAL_RF_WCDMA_RX_WAIT_V2_REQ *data)
{

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_WCDMA_RX_WAIT_V2_REQ), sizeof(HAL_RF_WCDMA_RX_WAIT_V2_RESP), HAL_RF_WCDMA_RX_WAIT_V2_RESP_ID, RFCA_wcdma_rx_wait_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_RX_WAIT_V2_RESP* halResp = (HAL_RF_WCDMA_RX_WAIT_V2_RESP*)rfcaResp->HAL.HalResponse;
  
  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->channel = data->channel;
  halResp->rf_band = data->rf_band;
  halResp->rssi_value = (int16)0x8000;

  if(HAL_RF_PROTOCOL_SYSTEM_WCDMA != StateCache.RF_TECHNOLOGY)
  {
    halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 ftmModeId = FTM_MODE_MAX_C;
  if(RFCA_FUNCTION_FAIL == GetFtmModeIdV2(StateCache.RF_TECHNOLOGY, data->rf_signal_path, &ftmModeId))
  {
    halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 currentBand = InvalidStateCacheValue;

  unsigned short rfMode = -1;

  if(false == RFCA_ModeToFtmRfMode(&rfMode, StateCache.RF_TECHNOLOGY, data->rf_band))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  // Calculate channel
  uint16 ulChannel = WCDMA_CalculateChannel((HAL_RF_WCDMA_BAND)data->rf_band, data->channel, DL_CHANNEL);
  if(0==ulChannel)
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  int16 pwrdBm10 = (int16)(int16_q8_to_double(data->trigger_level)*10);

  if(true == GetUseOldFtmForRxFlag())
  {

    AddFtmSeqAddTimingMarker(data->hdr.operation_id);

    if(false == RFCA_GetCurrentBand(&currentBand) || data->rf_band != currentBand)
    {

      AddFtmSetMode(data->hdr.operation_id, FTM_WCDMA_C, rfMode);     
      RFCA_StateCacheSetBand(&StateCache, data->rf_band);

      AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_WAIT.rfConfig);
    }

    // Set CHANNEL
    if(StateCache.CHANNEL != ulChannel)
    {
      AddFtmSetChan(data->hdr.operation_id, FTM_WCDMA_C, ulChannel); 
      StateCache.CHANNEL= ulChannel;
      AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_WAIT.rfTune);
    }

    // DIVERSITY
    if(StateCache.WCDMA.SIGNAL_PATH != data->rf_signal_path)
    {    
      AddFtmSetSecondaryChain(data->hdr.operation_id, GetWcdmaFtmCmdIdDivPath(data->rf_signal_path), GetSecChainEnabledFromSignalPath(data->rf_signal_path));
      StateCache.WCDMA.SIGNAL_PATH = data->rf_signal_path;
      if(FALSE == StateCache.CALIBRATION_STATE_ACTIVE)
      {
        AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_WAIT.rfPreExec);
      }
    }

    if(TRUE == StateCache.CALIBRATION_STATE_ACTIVE)
    {
      AddFtmSeqAddTimingMarker(data->hdr.operation_id);
      SetLnaRxRangeForRxLevel(HAL_RF_PROTOCOL_SYSTEM_WCDMA, data->hdr.operation_id, ftmModeId, data->trigger_level);
      AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_WAIT.rfPreExec);
    }

    AddFtmRxWaitDbmV2(data->hdr.operation_id, FTM_RF_TECH_WCDMA, GetDeviceIdFromSignalPath(data->rf_signal_path), data->trigger_mode, pwrdBm10, (data->timeout * WCDMA_FRAME_LENGTH_US));
  }
  else
  {
    FTM_Cal_Sweep_Opcode_Req_List_Item *head = NULL;
    FTM_Cal_Sweep_Opcode_Req_List_Item *tail = NULL;

    if(StateCache.BAND != data->rf_band)
    {
      AddSweepConfig(&head, &tail, data->rf_band, ulChannel, 0, 3, 3, data->rf_signal_path, enhSweepSegmentLength.rfConfig);      
    }
    else if(StateCache.CHANNEL != ulChannel)
    {
      AddSweepTune(&head, &tail, ulChannel, 0, enhSweepSegmentLength.rfTune);
    }
    {
      HAL_RF_RX_SWEEP_STEP_CONFIG rxCtrlData;
      rxCtrlData.actions = (1<< GetLnaRangeForRxLevel(StateCache.RF_TECHNOLOGY, data->trigger_level));
      rxCtrlData.powerMode = 0;
      rxCtrlData.expectedRxAgc = data->trigger_level; // Does not matter for this
      rxCtrlData.rxFlag = (1<<GetDeviceIdFromSignalPath(data->rf_signal_path));
      AddSweepRxConfig(&head, &tail, WcdmaRfcaTimeConfig.RX_WAIT.rfPreExec, rxCtrlData);
    }
    AddSweepRxWait(&head, &tail, (data->timeout * WCDMA_FRAME_LENGTH_US), enhSweepSegmentLength.pollingInterval_us, pwrdBm10);
    AddSweepToFtmCmdQueue(data->hdr.operation_id, &head, &tail, true);
  }
  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
   halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  }
  else
  {
    // Send ACK delayed response will be sent later
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_wcdma_rx_rssi_receive(unsigned int rfcaID)
{
  PendingRfcaResponse * rfcaResp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_RX_RSSI_RESP *hal_resp = (HAL_RF_WCDMA_RX_RSSI_RESP*)rfcaResp->HAL.HalResponse;
  HAL_RF_WCDMA_RX_RSSI_REQ* hal_req = ((HAL_RF_WCDMA_RX_RSSI_REQ*)rfcaResp->HAL.HalRequest);
  // By default we fail
  hal_resp->status = HAL_RF_STATUS_FAILED;
  hal_resp->rssi_value = (int16_q8)0xFFFF;

  // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = rfcaResp->FTM.FtmItems;
  while(NULL != ftmItem)
  {

    word subSysCmdId = 0xFFFF;
    ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &subSysCmdId);
    switch(subSysCmdId)
    {
      case _FTM_GET_RX_LEVEL_DBM:
      {
        const uint16 rxDbmOffset = 8;
        word rssi = 0x8000;
        ftmItem->Packet.GetResponseWordFromOffset(rxDbmOffset, &rssi);  
        hal_resp->rssi_value = double_to_int16_q8(((int16)rssi)/10.0);  ;
        hal_resp->channel = hal_req->channel;
        hal_resp->rf_band = hal_req->rf_band;
        hal_resp->status = HAL_RF_STATUS_OK;
        return;
        break;
      }
      default:
        break;
    }
    ftmItem = ftmItem->NextItem;
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_wcdma_rx_rssi_send(HAL_RF_WCDMA_RX_RSSI_REQ *data)
{

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_WCDMA_RX_RSSI_REQ), sizeof(HAL_RF_WCDMA_RX_RSSI_RESP), HAL_RF_WCDMA_RX_RSSI_RESP_ID, RFCA_wcdma_rx_rssi_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_RX_RSSI_RESP* halResp = (HAL_RF_WCDMA_RX_RSSI_RESP*)rfcaResp->HAL.HalResponse;
  
  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->channel = data->channel;
  halResp->rf_band = data->rf_band;    
  halResp->rssi_value = (int16_q8)0xFFFF;  // Spec requests that in failure case RSSI is 0xFFFF

  if(HAL_RF_PROTOCOL_SYSTEM_WCDMA != StateCache.RF_TECHNOLOGY)
  {
    halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 segmentLength = (uint16)(data->duration/wcdmaTimeToUs);

  uint16 ftmModeId = FTM_MODE_MAX_C;
  if(RFCA_FUNCTION_FAIL == GetFtmModeIdV2(StateCache.RF_TECHNOLOGY, data->rf_signal_path, &ftmModeId))
  {
    halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 currentBand = InvalidStateCacheValue;

  AddFtmSeqAddTimingMarker(data->hdr.operation_id);

  if(false == RFCA_GetCurrentBand(&currentBand) || data->rf_band != currentBand)
  {

    unsigned short rfMode = -1;

    if(false == RFCA_ModeToFtmRfMode(&rfMode, StateCache.RF_TECHNOLOGY, data->rf_band))
    {
      halResp->status = HAL_RF_STATUS_FAILED;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }

    AddFtmSetMode(data->hdr.operation_id, FTM_WCDMA_C, rfMode);     
    RFCA_StateCacheSetBand(&StateCache, data->rf_band);

    AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_RSSI.rfConfig);
  }
  
  // Set CHANNEL
  uint16 ulChannel = WCDMA_CalculateChannel((HAL_RF_WCDMA_BAND)data->rf_band, data->channel, DL_CHANNEL);
  if(0==ulChannel)
  {
      halResp->status = HAL_RF_STATUS_FAILED;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
  }

  if(StateCache.CHANNEL != ulChannel)
  {
    AddFtmSetChan(data->hdr.operation_id, FTM_WCDMA_C, ulChannel); 
    StateCache.CHANNEL= ulChannel;
    AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_RSSI.rfTune);
  }

  // DIVERSITY
  if(StateCache.WCDMA.SIGNAL_PATH != data->rf_signal_path)
  {    
    AddFtmSetSecondaryChain(data->hdr.operation_id, GetWcdmaFtmCmdIdDivPath(data->rf_signal_path), GetSecChainEnabledFromSignalPath(data->rf_signal_path));
    StateCache.WCDMA.SIGNAL_PATH = data->rf_signal_path;
    if(WcdmaRfcaTimeConfig.RX_RSSI.rfPreExec >= segmentLength)
    {
      MSG_ERROR("Sgement length too short to include %d pre-execution time for diversity", WcdmaRfcaTimeConfig.RX_IQ.rfPreExec, 0, 0);
      halResp->status = HAL_RF_STATUS_SEGMENT_LENGTH_TOO_SHORT;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }
    AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_RSSI.rfPreExec);
    segmentLength -= WcdmaRfcaTimeConfig.RX_RSSI.rfPreExec;
  }

  AddFtmGetRxLevelDbm(data->hdr.operation_id, ftmModeId);
  AddFtmSeqAddWait(data->hdr.operation_id, segmentLength);

  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  }
  else
  {
    // Send ACK delayed response will be sent later
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }
}

ftm_iq_capture_get_config_type wcdmaIqConfig = {FTM_IQ_MAX_ERROR, 0, 0, 0, 0};

/*----------------------------------------------------------------------------*/
void RFCA_wcdma_rx_iq_receive(unsigned int rfcaID)
{
  PendingRfcaResponse * rfcaResp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_RX_IQ_RESP *hal_resp = (HAL_RF_WCDMA_RX_IQ_RESP*)rfcaResp->HAL.HalResponse;

  // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = rfcaResp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    uint16 respFtmModeId = 0xFFFF;
    ftmItem->Packet.GetResponseWordFromOffset(FTM_MODE_ID_OFFSET, &respFtmModeId);

    uint16 respFtmCmdId = 0xFFFF;
    ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &respFtmCmdId);

    switch(respFtmModeId)
    {
    case FTM_WCDMA_C:
    case FTM_WCDMA_RX_2_C:
    case FTM_WCDMA_RX_3_C:
    case FTM_WCDMA_RX_4_C:
      {
        switch(respFtmCmdId)
        {
        case FTM_IQ_CAPTURE_GET_CONFIG:
        case FTM_IQ_CAPTURE_ACQUIRE_SAMPLES: 
        {
            const uint16 sampleErrorOffset = 6;

            uint16 errorCode;
            ftmItem->Packet.GetResponseWordFromOffset(sampleErrorOffset, &errorCode);  

            if(FTM_IQ_SUCCESS != errorCode)
            {
              MSG_ERROR("IQ sample acquisition or config failed. FMT cmd id = %d", respFtmCmdId, 0, 0);
              hal_resp->status = HAL_RF_STATUS_RX_IQ_GENERAL_FAILURE;
              return;
            }
          }
          break;
        case FTM_IQ_CAPTURE_GET_SAMPLES:
          {
            const uint16 sampleErrorOffset = 6;
            const uint16 sampleSizeOffset = 12;
            const uint16 sampleOffset = 16;

            uint16 errorCode;
            ftmItem->Packet.GetResponseWordFromOffset(sampleErrorOffset, &errorCode);  

            if(FTM_IQ_SUCCESS != errorCode)
            {
              MSG_ERROR("IQ sample retrieval failed.", 0, 0, 0);
              switch(errorCode)
              {
              case FTM_IQ_GENERAL_FAILURE:
                hal_resp->status = HAL_RF_STATUS_RX_IQ_GENERAL_FAILURE;
                break;
              case FTM_IQ_IQ_SAMPLES_UNAVAILABLE_ON_RFM_DEV:
                hal_resp->status = HAL_RF_STATUS_RX_IQ_SAMPLES_UNAVAILABLE_ON_DEV;
                break;
              case FTM_IQ_IQ_SAMPLES_UNAVAILABLE:
                hal_resp->status = HAL_RF_STATUS_RX_IQ_SAMPLES_UNAVAILABLE;
                break;
              case FTM_IQ_ILLEGAL_COMBIMATION:
                hal_resp->status = HAL_RF_STATUS_RX_IQ_ILLEGAL_COMBIMATION;
                break;
              }
              return;
            }
            uint32 returnedSampleSize; 
            ftmItem->Packet.GetResponseDwordFromOffset(sampleSizeOffset, &returnedSampleSize);  

            // Re-allocate response
            // TODO: Refactor this to a sub function as similar code is also in AddToRfcaResponseQueue and if RFIC RFNV function              
            unsigned int halPayloadSize = returnedSampleSize * 4;
            HAL_RF_WCDMA_RX_IQ_RESP * newHalResponse = (HAL_RF_WCDMA_RX_IQ_RESP *)RFCA_Malloc(sizeof(HAL_RF_WCDMA_RX_IQ_RESP) + halPayloadSize);

            if(NULL == newHalResponse)
            {
              MSG_ERROR("Failed to re-allocate memory for new HAL response.\n", 0, 0, 0);
              hal_resp->status = HAL_RF_STATUS_MEMORY_ALLOCATION_FAIL;
              return;
            }

            // Copy data from previously allocated response
            memscpy(newHalResponse, rfcaResp->HAL.SizeOfHalResponse, rfcaResp->HAL.HalResponse, rfcaResp->HAL.SizeOfHalResponse); 

            // Free old response
            RFCA_Free(rfcaResp->HAL.HalResponse);

            // Update to match new reponse
            rfcaResp->HAL.HalResponse = newHalResponse;
            rfcaResp->HAL.SizeOfHalResponse = sizeof(HAL_RF_WCDMA_RX_IQ_RESP) + halPayloadSize;
            // Update to new HAL response
            hal_resp = newHalResponse;
            hal_resp->vector_length = returnedSampleSize; 

           
            if( FTM_IQ_8_BIT_FORMAT == wcdmaIqConfig.sample_data_format )
            {
              
              EightBitIqSample *iqSamples = (EightBitIqSample *)(ftmItem->Packet.GetResponseDataPtr()+sampleOffset);
              uint16 * I = hal_resp->iq_data;
              uint16 * Q = hal_resp->iq_data + returnedSampleSize;

              HalLogWrite("Parsing 8-bit IQ data of %d samples where first I=%d and Q=%d)", returnedSampleSize, iqSamples[0].I, iqSamples[0].Q);

              for(uint32 i = 0; i < returnedSampleSize; i++)
              {
                I[i] = iqSamples[i].I;
                Q[i] = iqSamples[i].Q;
              }                
            }
            else if( FTM_IQ_16_BIT_FORMAT == wcdmaIqConfig.sample_data_format )
            {
              SixteenBitIqSample *iqSamples = (SixteenBitIqSample *)(ftmItem->Packet.GetResponseDataPtr()+sampleOffset);
              uint16 * I = hal_resp->iq_data;
              uint16 * Q = hal_resp->iq_data + returnedSampleSize;

              HalLogWrite("Parsing 16-bit IQ data of %d samples where first I=%d and Q=%d)", returnedSampleSize, iqSamples[0].I, iqSamples[0].Q);

              for(uint32 i = 0; i < returnedSampleSize; i++)
              {
                I[i] = iqSamples[i].I;
                Q[i] = iqSamples[i].Q;
              }   
            }
            else
            {
              MSG_ERROR("Invalid IQ sample format from FTM, RFCA only supports 8-bit and 16-bit IQ samples.", 0, 0, 0);
              hal_resp->status = HAL_RF_STATUS_RX_IQ_GENERAL_FAILURE;
              return;
            }

            hal_resp->status = HAL_RF_STATUS_OK;
            return;
          }
          break;
        default:
          break;
        }
      }
    }
    ftmItem = ftmItem->NextItem;
  }
}


void RFCA_wcdma_rx_iq_send(HAL_RF_WCDMA_RX_IQ_REQ* data)
{

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_WCDMA_RX_IQ_REQ), sizeof(HAL_RF_WCDMA_RX_IQ_RESP), HAL_RF_WCDMA_RX_IQ_RESP_ID, RFCA_wcdma_rx_iq_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_WCDMA_RX_IQ_RESP* halResp = (HAL_RF_WCDMA_RX_IQ_RESP*)rfcaResp->HAL.HalResponse;

  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->channel = data->channel;
  halResp->rf_band = data->rf_band;
  halResp->iq_select = data->iq_select;
  halResp->vector_length = 0;

  if(HAL_RF_PROTOCOL_SYSTEM_WCDMA != StateCache.RF_TECHNOLOGY)
  {
    halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 ftmModeId = FTM_MODE_MAX_C;
  if(RFCA_FUNCTION_FAIL == GetFtmModeIdV2(StateCache.RF_TECHNOLOGY, data->rf_signal_path, &ftmModeId))
  {
    halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 segmentLength = (uint16)(data->duration/wcdmaTimeToUs);

  uint16 currentBand = InvalidStateCacheValue;

  unsigned short rfMode = -1;

  if(false == RFCA_ModeToFtmRfMode(&rfMode, StateCache.RF_TECHNOLOGY, data->rf_band))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }
  
  uint16 ulChannel = WCDMA_CalculateChannel((HAL_RF_WCDMA_BAND)data->rf_band, data->channel, DL_CHANNEL);
  if(0==ulChannel)
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  {

    AddFtmSeqAddTimingMarker(data->hdr.operation_id);

    if(false == RFCA_GetCurrentBand(&currentBand) || data->rf_band != currentBand)
    {

      AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);     
      RFCA_StateCacheSetBand(&StateCache, data->rf_band);

      AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_IQ.rfConfig);
    }

    // Set CHANNEL
    if(StateCache.CHANNEL != ulChannel)
    {
      AddFtmSetChan(data->hdr.operation_id, ftmModeId, ulChannel); 
      StateCache.CHANNEL= ulChannel;
      AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_IQ.rfTune);
    }  

    // DIVERSITY
    if(StateCache.WCDMA.SIGNAL_PATH != data->rf_signal_path)
    {
      AddFtmSetSecondaryChain(data->hdr.operation_id, GetWcdmaFtmCmdIdDivPath(data->rf_signal_path), GetSecChainEnabledFromSignalPath(data->rf_signal_path));
      StateCache.WCDMA.SIGNAL_PATH = data->rf_signal_path;

      if(WcdmaRfcaTimeConfig.RX_IQ.rfPreExec >= segmentLength)
      {
        MSG_ERROR("Sgement length too short to include %d pre-execution time for diversity", WcdmaRfcaTimeConfig.RX_IQ.rfPreExec, 0, 0);
        halResp->status = HAL_RF_STATUS_SEGMENT_LENGTH_TOO_SHORT;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
        return;
      }
      AddFtmSeqAddWait(data->hdr.operation_id, WcdmaRfcaTimeConfig.RX_IQ.rfPreExec);
      segmentLength -= WcdmaRfcaTimeConfig.RX_IQ.rfPreExec;
    }

    if(FTM_IQ_MAX_ERROR == wcdmaIqConfig.ftm_error_code)
    {
      ftm_iq_capture_get_config_type * iqConfigData = NULL;
      FtmPacketQueueItem *ftmQueueItem = AddFtmIqCaptureGetConfig(data->hdr.operation_id, ftmModeId);
    
      const uint16 ftmSyncResponseMaxSize = 1024;
      uint16 ftmSyncResponseSize = ftmSyncResponseMaxSize;
      byte ftmSyncResponse[ftmSyncResponseMaxSize] = {0};
      if(RFCA_FUNCTION_SUCCESS != RFCA_SendToFtmTaskSync(ftmQueueItem->Packet.GetRequestDataPtr(), ftmQueueItem->Packet.GetRequestLength(), ftmSyncResponse, &ftmSyncResponseSize))
      {
        MSG_ERROR("Failed to get IQ capture config, communication error", 0, 0, 0);
        halResp->status = HAL_RF_STATUS_FAILED;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
        return;
      }
      const uint16 iqDataOffset = 6;
      iqConfigData = (ftm_iq_capture_get_config_type * )(&ftmSyncResponse[iqDataOffset]);
      if(FTM_IQ_SUCCESS != iqConfigData->ftm_error_code)
      {
        MSG_ERROR("Failed to get IQ capture config, function failed", 0, 0, 0);
        halResp->status = HAL_RF_STATUS_FAILED;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
        return;
      }
      HalLogWrite("iqData: max_sample_size = %d, sample_data_format  = %d, sampling_freq  = %d, max_diag_sample_size  = %d", iqConfigData->max_sample_size, iqConfigData->sample_data_format, iqConfigData->sampling_freq, iqConfigData->max_diag_sample_size);
      wcdmaIqConfig.ftm_error_code = iqConfigData->ftm_error_code;
      wcdmaIqConfig.max_diag_sample_size = iqConfigData->max_diag_sample_size;
      wcdmaIqConfig.max_sample_size = iqConfigData->max_sample_size;
      wcdmaIqConfig.sample_data_format = iqConfigData->sample_data_format;
      wcdmaIqConfig.sampling_freq = iqConfigData->sampling_freq;
    }    

    AddFtmIqCaptureAquireSamples(data->hdr.operation_id, ftmModeId, wcdmaIqConfig.max_sample_size, 0);
    AddFtmIqCaptureGetSamples(data->hdr.operation_id, ftmModeId, iqSampleOffset, iqSampleSize);
    AddFtmSeqAddWait(data->hdr.operation_id, segmentLength);
  }
  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  }
  else
  {
    // Send ACK delayed response will be sent later
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }
}

