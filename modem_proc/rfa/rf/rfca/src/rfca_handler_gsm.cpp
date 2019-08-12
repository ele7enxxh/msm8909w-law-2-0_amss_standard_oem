/*!
@file
  rfca_handler_gsm.cpp

@brief
  Functions to trasfer GSM RFCA commands to FTM commands and vice-versa.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca_handler_gsm.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/10/14   aki     RSSI for combined PRx,DRx call AddFtmGsmGetMeasPwrPrxDrx
10/30/14   aki     Added RSSI for DRx to call AddFtmGsmGetMeasPwrDrx
08/29/14   aki     Chnanged GSM gain range to PRx/DRx version 
06/17/14   aki     Added XO to RX align and updated RX IQ
05/22/14   tws     Move IQ capture after start RX command
01/31/14   aki     Added set rx burst in RSSI
01/23/14   aki     KV back to GSM from EXTENDED 
01/03/14   aki     Removed RFCA_ASSERT 
11/14/13   aki     Updated version of antenna switching 
11/14/13   aki     Added EFS_SYNC after tuner self cal
11/14/13   aki     TXRX interleaving, TX on after configuring cpl frame, TX off
                   if ts activity or modulation changes
09/13/13   aki     Added ant switch support
08/09/13   aki     Update to GSM IQ
08/02/13   aki     Support for DSDA, IQ and FTM mode ID
06/29/13   hm      Add new FTM command for tuner self cal: "FTM_DO_TUNER_SELF_CAL"
06/24/13   sar     Replaced memcpy calls with safe memscpy.
06/10/13   aki     Mainline RFCA
06/06/13   aki     Updates to remove KW warnings and errors
05/01/13   aki     Added RFCA_gsm_rx_wait_v2_* functions
04/25/13   aki     Added GSM sweep support for PCL
04/09/13   aki     Move RX burst set out of config in align and add 20 ms wait
03/19/13   aki     Update GSM PCL to new multi chain NV items
02/01/13   aki     Updated state cache band and channel
12/27/12   aki     Added support for pre-dist and DA timing config
12/19/12   aki     Removed set mode, set channel from KVCO
12/12/12   aki     Removed pre-exec waits
11/29/12   tws     Rename FTM_GSM_C to FTM_GSM_C0_C to avoid confusion.
10/31/12   aki     Timing config data, moved inital seq timing marks to top
10/03/12   aki     Execution timing updates
10/02/12   aki     Remove RFCA_QMSL ifdefs
09/17/12   aki     Using 32-bit int instead of 16-bit
07/13/12   aki     Added sanity mode checks, removed gsm_tx_dac
05/25/12   aki     Timing changes to make RX more stable and possibly overcome RX stuck
04/23/12   aki     Response handling in error case, code cleanup, timing changes,
                   Rx burst support for more than one option
02/24/12   aki     Disable cal state for PCL, cleanup, RX wait timing fix
02/17/12   aki     Added GSM_TX_ALIGN support
02/08/12   aki     Cleanup of type casts
01/25/12   aki     Diag V2 delayed response implementation, update of booleans
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
#include "rfnv_items.h"

#include "ftm_command_id.h"
#include "msg.h"

#include "rfca_to_ftm_adapter.h"

extern "C" {
  #include "fs_public.h"
}


const uint16 maxFramesInGsmSweep = 200;
const uint16 maxSlotsPerFrameInGsmSweep = 8;
const uint16 maxGsmSweepBufferSize = sizeof(ftm_gsm_config_header_type) + ((sizeof(ftm_gsm_frame_header_type) + sizeof(ftm_gsm_slot_payload_type) * maxSlotsPerFrameInGsmSweep) * maxFramesInGsmSweep);

GsmRfcaTimeConfigData GsmRfcaTimeConfig =
{
  {85000, 5000}, //RX_ALIGN;
  {85000, 65000, 0, 10000}, //RX_WAIT;
  {85000, 65000, 70000}, //RX_RSSI;
  {
    {85000, 50000}, //TX_ALIGN (KVCO_CAL);
    //{0, 0},     //TX_ALIGN (SELF_CAL);
  },
  {85000, 5000, 59995}, //TX_PCL;
  {
    {85000, 5000, 45000},  // ARB_TX:DRIVER_AMP
    {85000, 5000, 20000},  // ARB_TX:PRE_DIST
  },
  {85000, 65000, 70000, 50000}, //RX_IQ;
};

/*----------------------------------------------------------------------------*/
ftm_gsm_frame_sweep_config_type masterGsmSweepConfig = {NULL, 0};

/*----------------------------------------------------------------------------*/
ftm_gsm_frame_sweep_config_type* GetMasterGsmSweepConfig()
{
  return &masterGsmSweepConfig;
}

/*----------------------------------------------------------------------------*/
void ResetGsmSweepConfig()
{
  RFCA_Free(masterGsmSweepConfig.buffer);
  masterGsmSweepConfig.buffer = NULL;
  masterGsmSweepConfig.used_bytes_count = 0;
}

/*----------------------------------------------------------------------------*/
bool gsmPclUseSweep = false;

/*----------------------------------------------------------------------------*/
void SetGsmPclUseSweep(bool newState)
{
  HalLogWrite("SetGsmPclUseSweep: GSM TX PCL will use %s\n", (true == newState)?"sweep":"frame matrix");
  gsmPclUseSweep = newState;
}

/*----------------------------------------------------------------------------*/
word GetGsmExtendedModeIdFromModeId(word ftmModeId)
{
  word extMode = FTM_GSM_EXTENDED_C0_C;
  switch (ftmModeId) {
  case FTM_GSM_C0_C:
    extMode = FTM_GSM_EXTENDED_C0_C;
    break;
  case FTM_GSM_C1_C:
    extMode = FTM_GSM_EXTENDED_C1_C;
    break;
  case FTM_GSM_C2_C:
    extMode = FTM_GSM_EXTENDED_C2_C;
    break;
  case FTM_GSM_C3_C:
    extMode = FTM_GSM_EXTENDED_C3_C;
    break;
  default:
    MSG_ERROR("Invalid GSM FTM mode id %d, cannot convert to ext mode id, using %d as default", ftmModeId, FTM_GSM_EXTENDED_C0_C, 0);
    extMode = FTM_GSM_EXTENDED_C0_C;
    break;
  }
  return extMode;
}

double CalculateULFrequency(HAL_RF_GSM_BAND band, uint16 channel) 
{
  double freq = 0.0;

  switch (band) 
  {
    case HAL_RF_BAND_GSM_900:
      if (channel <= 124) 
      {
        freq = 890. + 0.2 * channel;
      }
      else if (channel >= 975 && channel <= 1023)
      {
         freq = 890. + 0.2 * (channel - 1024);
      }
      break;
    case HAL_RF_BAND_GSM_1800:
      freq = 1710.2 + 0.2 * (channel - 512);
      break;
    case HAL_RF_BAND_GSM_1900:
      freq = 1850.2 + 0.2 * (channel - 512);
      break;
    case HAL_RF_BAND_GSM_850:
      freq = 824.2 + 0.2 * (channel - 128);
      break;
    default:
      HalLogWrite("Invalid GSM band enum (%d) for frequency calculation\n", band);
      return 0.0;
      break;
  }

  return freq;
}
/*----------------------------------------------------------------------------*/
void RFCA_gsm_rx_align_receive(unsigned int rfcaID)
{

  PendingRfcaResponse * rfcaResp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_RX_ALIGN_RESP* hal_resp = ((HAL_RF_GSM_RX_ALIGN_RESP*)rfcaResp->HAL.HalResponse);
  HAL_RF_GSM_RX_ALIGN_REQ* hal_req = ((HAL_RF_GSM_RX_ALIGN_REQ*)rfcaResp->HAL.HalRequest);


  // By default we fail
  hal_resp->status = HAL_RF_STATUS_FAILED;
  hal_resp->data_length = 0;

  // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = rfcaResp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    uint16 respMsgId = 0xFFFF;
    ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &respMsgId);

    switch(respMsgId)
    {
      case FTM_RX_GAIN_RANGE_PRX_DRX_CAL:
      {
        hal_resp->data_length = 32;
        memscpy(&(hal_resp->data_array), hal_resp->data_length*2, &(ftmItem->Packet.GetResponseDataPtr()[11]), hal_resp->data_length*2);
        hal_resp->status = HAL_RF_STATUS_OK;
      }
      break;
      case _FTM_XO_CALIBRATION:
      {
        if(ftmItem->Packet.GetResponseLength() < (RFCA_XO_DATA_ARRAY_SIZE * sizeof(uint16)))
        {
          MSG_ERROR("DEBUG: _FTM_XO_CALIBRATION Response End, response too small, failing", 0, 0, 0);
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
      default:
        break;
    }
    ftmItem = ftmItem->NextItem;
  }
}
/*----------------------------------------------------------------------------*/
void RFCA_gsm_rx_align_send(HAL_RF_GSM_RX_ALIGN_REQ *data)
{

  uint16 responseDataArraySize = 32; // Gets multiplied by 2 later, is the gain range cal response size
  uint16 config_pattern_LB = data->config_pattern & 0x00FF;
  bool isXoCal = false;
  if((0xFF == config_pattern_LB) && ( XO_CAL_ENABLE & data->config_pattern))
  {
    responseDataArraySize = RFCA_XO_DATA_ARRAY_SIZE;
    isXoCal = true;
  }

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_GSM_RX_ALIGN_REQ), sizeof(HAL_RF_GSM_RX_ALIGN_RESP) + responseDataArraySize * sizeof(uint16), HAL_RF_GSM_RX_ALIGN_RESP_ID, RFCA_gsm_rx_align_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_RX_ALIGN_RESP* halResp = (HAL_RF_GSM_RX_ALIGN_RESP*) rfcaResp->HAL.HalResponse;
  halResp->data_length = 0;
  halResp->status = HAL_RF_STATUS_FAILED;

  if(HAL_RF_PROTOCOL_SYSTEM_GSM != StateCache.RF_TECHNOLOGY)
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

    AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);
    RFCA_StateCacheSetBand(&StateCache, data->rf_band);

    if (false == isXoCal) 
    {
      // Just call this always as part of RF Config
      AddFtmSetChan(data->hdr.operation_id, ftmModeId, data->channel);

      AddFtmSetRxBurst(data->hdr.operation_id, ftmModeId, 0, RX_FUNCTION_STOP_CONTINOUS_RX);
      StateCache.GSM.RX_STATE = RX_FUNCTION_STOP_CONTINOUS_RX;
    }

    AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_ALIGN.rfConfig);
  }

  if(StateCache.CHANNEL != data->channel)
  {
    AddFtmSetChan(data->hdr.operation_id, ftmModeId, data->channel);
    StateCache.CHANNEL= data->channel;
    AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_ALIGN.rfTune);
  }

  if (false == isXoCal) 
  {
    if(RX_FUNCTION_STOP_CONTINOUS_RX != StateCache.GSM.RX_STATE)
    {
      AddFtmSetRxBurst(data->hdr.operation_id, ftmModeId, 0, RX_FUNCTION_STOP_CONTINOUS_RX);
      StateCache.GSM.RX_STATE = RX_FUNCTION_STOP_CONTINOUS_RX;
      AddFtmSeqAddWait(data->hdr.operation_id, 20000);
    }
  }

  bool addSeqWait = true;

  if (true == isXoCal) 
  {

    AddFtmSetLnaRange(data->hdr.operation_id, ftmModeId, 2);

    AddFtmSetRxContinuous(data->hdr.operation_id, ftmModeId, RX_FUNCTION_START_CONTINOUS_RX);
    // Set to invalid so that hal_rf_stop won't add any commands
    StateCache.GSM.RX_STATE = InvalidStateCacheValue;

    currentXoParameters.technology = XO_TECH_GSM;
    currentXoParameters.rx_freq_khz = (uint32)(CalculateULFrequency((HAL_RF_GSM_BAND)data->rf_band, data->channel) * 1000);
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
    HAL_RF_GSM_RX_ALIGN_CONFIG *configData = (HAL_RF_GSM_RX_ALIGN_CONFIG*) &data->config_pattern;
    // Set rxToggle to 1 only on the first time
    AddFtmRxGainRangeCal(data->hdr.operation_id, ftmModeId, configData->gainRangeBitMask, configData->numberOfAverages, (false == StateCache.GSM.RX_GAIN_RANGE_CAL_INITIALIZED ? 1 : 0));
  }

  if( true == addSeqWait )
  {
    AddFtmSeqAddWait(data->hdr.operation_id, (data->duration * GSM_FRAME_LENGTH_US));
  }
  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
  }
  else
  {
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }

  StateCache.GSM.RX_GAIN_RANGE_CAL_INITIALIZED = true;
}


/*----------------------------------------------------------------------------*/
void RFCA_gsm_tx_align_receive(unsigned int rfcaID)
{
  PendingRfcaResponse * resp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == resp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_TX_ALIGN_RESP* hal_resp = ((HAL_RF_GSM_TX_ALIGN_RESP*)resp->HAL.HalResponse);
  HAL_RF_GSM_TX_ALIGN_REQ* hal_req = ((HAL_RF_GSM_TX_ALIGN_REQ*)resp->HAL.HalRequest);

  // By default we fail
  hal_resp->status = HAL_RF_STATUS_FAILED;
  hal_resp->data_length = 0;

  // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = resp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    uint16 respMsgId = 0xFFFF;
    ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &respMsgId);

    switch(respMsgId)
    {
      case _FTM_GSM_TX_KV_CAL_V2:
      {
        const uint16 FTM_KVCO_RESULT_OFFSET = 22;
        uint16 numberOfBandsRequested = hal_req->config_pattern >> 8;
        hal_resp->data_length = (sizeof(kv_channel) * numberOfBandsRequested)/sizeof(uint16); // As uint16
        memscpy(&(hal_resp->data_array),hal_resp->data_length * sizeof(uint16),
                &(ftmItem->Packet.GetResponseDataPtr()[FTM_KVCO_RESULT_OFFSET]), hal_resp->data_length * sizeof(uint16));
        hal_resp->status = HAL_RF_STATUS_OK;
      }
      break;
      case _FTM_DO_SELF_CAL:
      {
        const uint16 statusOffset = 10;
        uint16 cmdStatus = 0xFFFF;
        ftmItem->Packet.GetResponseWordFromOffset(statusOffset, &cmdStatus);
        hal_resp->data_length = 0;
        if(FTM_COMMON_SUCCESS == cmdStatus)
        {
          hal_resp->status = HAL_RF_STATUS_OK;
        }
      }
      break;
      case _FTM_DO_TUNER_SELF_CAL:  // Intentional fall-through, the responses are identical
      {
        const uint16 statusOffset = 10;
        uint16 cmdStatus = 0xFFFF;
        ftmItem->Packet.GetResponseWordFromOffset(statusOffset, &cmdStatus);
        hal_resp->data_length = 0;
        if(FTM_COMMON_SUCCESS == cmdStatus)
        {
          HalLogWrite("Tuner self call passed ok");
          hal_resp->status = HAL_RF_STATUS_OK;
          if(true == GetEfsSyncAfterTunerSelfCal())
          {          
            if(0 == efs_sync("/"))
            {
              HalLogWrite("EFS sync success");
              hal_resp->status = HAL_RF_STATUS_OK;
            }
            else
            {
              HalLogWrite("EFS sync fail");
              hal_resp->status = HAL_RF_STATUS_FAILED;
            }
          }
        }
        else
        {
          HalLogWrite("Tuner self call failed, efs sync won't be done");
          hal_resp->status = HAL_RF_STATUS_FAILED;
          return;
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
void RFCA_gsm_tx_align_send(HAL_RF_GSM_TX_ALIGN_REQ *data)
{

  PendingRfcaResponse *rfcaResp = NULL;

  unsigned long timeBudget = data->duration *  GSM_FRAME_LENGTH_US;

  if(HAL_RF_GSM_TX_ALIGN_CONFIG_KVCO_CAL == data->config_pattern)
  {

    rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_GSM_TX_ALIGN_REQ), sizeof(HAL_RF_GSM_TX_ALIGN_RESP) + (currentKvParameters.numberOfBands * sizeof(kv_channel)), HAL_RF_GSM_TX_ALIGN_RESP_ID, RFCA_gsm_tx_align_receive);

    if(NULL == rfcaResp)
    {
      MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
      return;
    }

    HAL_RF_GSM_TX_ALIGN_RESP* halResp = (HAL_RF_GSM_TX_ALIGN_RESP*)rfcaResp->HAL.HalResponse;
    halResp->status = HAL_RF_STATUS_FAILED;
    halResp->data_length = 0;

    uint16 ftmModeId = FTM_MODE_MAX_C;
    if(RFCA_FUNCTION_FAIL == GetFtmModeId(GetActiveRfDevice(), StateCache.RF_TECHNOLOGY, RF_SIGNAL_PATH_MAIN, &ftmModeId))
    {
      halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }

    if(HAL_RF_PROTOCOL_SYSTEM_GSM != StateCache.RF_TECHNOLOGY)
    {
      halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }

    AddFtmSeqAddTimingMarker(data->hdr.operation_id);

    // All parameters should have been set by earlier RFIC WRITE
    AddFtmGsmTxKvCalV2(data->hdr.operation_id, ftmModeId);
    ((HAL_RF_GSM_TX_ALIGN_REQ*) rfcaResp->HAL.HalRequest)->config_pattern = (currentKvParameters.numberOfBands << 8) | ((HAL_RF_GSM_TX_ALIGN_REQ*) rfcaResp->HAL.HalRequest)->config_pattern;

    RFCA_StateCacheResetTechnology(&StateCache, HAL_RF_PROTOCOL_SYSTEM_GSM);
    AddFtmSeqAddWait(data->hdr.operation_id, timeBudget);
  }
  else
  {
    rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_GSM_RX_ALIGN_REQ), sizeof(HAL_RF_GSM_RX_ALIGN_RESP), HAL_RF_GSM_TX_ALIGN_RESP_ID, RFCA_gsm_tx_align_receive);

    if(NULL == rfcaResp)
    {
      MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
      return;
    }

    AddFtmSeqAddTimingMarker(data->hdr.operation_id);

    HAL_RF_GSM_TX_ALIGN_RESP* halResp = (HAL_RF_GSM_TX_ALIGN_RESP*)rfcaResp->HAL.HalResponse;
    halResp->status = HAL_RF_STATUS_FAILED;
    halResp->data_length = 0;

    if(HAL_RF_PROTOCOL_SYSTEM_GSM != StateCache.RF_TECHNOLOGY)
    {
      halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }

    if(HAL_RF_GSM_TX_ALIGN_CONFIG_SELF_CAL == data->config_pattern)
    {
      AddFtmDoSelfCal(data->hdr.operation_id);
    }
    else if(HAL_RF_GSM_TX_ALIGN_CONFIG_TUNER_SELF_CAL == data->config_pattern)
    {
      AddFtmDoTunerSelfCal(data->hdr.operation_id);
    }
    else
    {
      HalLogWrite("Invalid config_pattern %d\n", data->config_pattern);

      ((HAL_RF_GSM_RX_ALIGN_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }
    AddFtmSeqAddWait(data->hdr.operation_id, timeBudget);
  }

  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    ((HAL_RF_GSM_TX_ALIGN_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
  }
  else
  {
    ((HAL_RF_GSM_TX_ALIGN_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }

}

/*----------------------------------------------------------------------------*/
void RFCA_gsm_rx_wait_receive(unsigned int rfcaID)
{

  PendingRfcaResponse * resp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == resp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_RX_WAIT_RESP* hal_resp = ((HAL_RF_GSM_RX_WAIT_RESP*)resp->HAL.HalResponse);
  HAL_RF_GSM_RX_WAIT_REQ* hal_req = ((HAL_RF_GSM_RX_WAIT_REQ*)resp->HAL.HalRequest);
  // By default we fail
  hal_resp->status = HAL_RF_STATUS_FAILED;

  hal_resp->channel = hal_req->channel;
  hal_resp->rf_band = hal_req->rf_band;

  // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = resp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    word ftmMsgId = 0xFFFF;

    if(TRUE == ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &ftmMsgId))
    {
      if( _FTM_RX_WAIT_DBM ==  ftmMsgId  )
      {
        ftm_rx_wait_dbm_response_type* rxWaitFtmResp = (ftm_rx_wait_dbm_response_type*)ftmItem->Packet.GetResponseDataPtr();
        {
          hal_resp->rssi_value = double_to_int16_q8((rxWaitFtmResp->rx_power/16.0));
          hal_resp->status = (FTM_RX_WAIT_DBM_SUCCESS == rxWaitFtmResp->status)?(HAL_RF_STATUS_OK):(HAL_RF_STATUS_FAILED);
        }
        break;
      }
      else if( _FTM_RX_WAIT_DBM_V2 == ftmMsgId )
      {
        ftm_rx_wait_dbm_v2_response_type* rxWaitFtmResp = (ftm_rx_wait_dbm_v2_response_type*)ftmItem->Packet.GetResponseDataPtr();
        hal_resp->rssi_value = double_to_int16_q8((rxWaitFtmResp->rx_power/16.0));
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
        break;
      }
    }
    ftmItem = ftmItem->NextItem;
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_gsm_rx_wait_send(HAL_RF_GSM_RX_WAIT_REQ *data)
{

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_GSM_RX_WAIT_REQ), sizeof(HAL_RF_GSM_RX_WAIT_RESP), HAL_RF_GSM_RX_WAIT_RESP_ID, RFCA_gsm_rx_wait_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_RX_WAIT_RESP* halResp = (HAL_RF_GSM_RX_WAIT_RESP*)rfcaResp->HAL.HalResponse;
  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->rssi_value = (int16)0x8000;
  halResp->channel = data->channel;
  halResp->rf_band = data->rf_band;

  if(HAL_RF_PROTOCOL_SYSTEM_GSM != StateCache.RF_TECHNOLOGY)
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

  word ftmModeIdExtended = GetGsmExtendedModeIdFromModeId(ftmModeId);

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

    AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);
    RFCA_StateCacheSetBand(&StateCache, data->rf_band);

    AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_WAIT.rfConfig);

  }

  int16 pwrdBm16 = (int16)(int16_q8_to_double(data->trigger_level)*16);

  if(StateCache.CHANNEL != data->channel)
  {
    AddFtmSetChan(data->hdr.operation_id, ftmModeId, data->channel);
    StateCache.CHANNEL= data->channel;
    AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_WAIT.rfTune);
  }

  AddFtmGsmSetupRxBurstForExpectedPower(data->hdr.operation_id, ftmModeIdExtended, pwrdBm16, 1);

  AddFtmRxWaitDbm(data->hdr.operation_id, ftmModeId, pwrdBm16, (data->timeout * GSM_FRAME_LENGTH_US));

  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
  }
  else
  {
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_gsm_rx_wait_v2_send(HAL_RF_GSM_RX_WAIT_V2_REQ *data)
{

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_GSM_RX_WAIT_V2_REQ), sizeof(HAL_RF_GSM_RX_WAIT_V2_RESP), HAL_RF_GSM_RX_WAIT_V2_RESP_ID, RFCA_gsm_rx_wait_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_RX_WAIT_V2_RESP* halResp = (HAL_RF_GSM_RX_WAIT_V2_RESP*)rfcaResp->HAL.HalResponse;
  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->rssi_value = (int16)0x8000;
  halResp->channel = data->channel;
  halResp->rf_band = data->rf_band;

  if(HAL_RF_PROTOCOL_SYSTEM_GSM != StateCache.RF_TECHNOLOGY)
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

  word ftmModeIdExtended = GetGsmExtendedModeIdFromModeId(ftmModeId);

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

    AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);
    RFCA_StateCacheSetBand(&StateCache, data->rf_band);

    AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_WAIT.rfConfig);

  }

  int16 pwrdBm16 = (int16)(int16_q8_to_double(data->trigger_level)*16);

  if(StateCache.CHANNEL != data->channel)
  {
    AddFtmSetChan(data->hdr.operation_id, ftmModeId, data->channel);
    StateCache.CHANNEL= data->channel;
    AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_WAIT.rfTune);
  }

  AddFtmGsmSetupRxBurstForExpectedPower(data->hdr.operation_id, ftmModeIdExtended, pwrdBm16, 1);

  AddFtmRxWaitDbmV2(data->hdr.operation_id, FTM_RF_TECH_GSM, GetDeviceIdFromSignalPath(data->rf_signal_path), data->trigger_mode, pwrdBm16, (data->timeout * GSM_FRAME_LENGTH_US));

  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
  }
  else
  {
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_gsm_rx_rssi_receive(unsigned int rfcaID)
{

  PendingRfcaResponse * resp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == resp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_RX_RSSI_RESP* hal_resp = ((HAL_RF_GSM_RX_RSSI_RESP*)resp->HAL.HalResponse);

  // By default we fail
  hal_resp->status = HAL_RF_STATUS_FAILED;

  // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = resp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    word subSysCmdId = 0xFFFF;
    ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &subSysCmdId);
    if( FTM_GSM_GET_MEAS_PWR_PRX_DRX == subSysCmdId )
    {
      const uint16 GSM_MEAS_PWR_OFFSET = 10;
      word dBmRssiPrx = 0x8000;
      dword rawRssiPrx = 0x80000000;
      word dBmRssiDrx = 0x8000;
      dword rawRssiDrx = 0x80000000;
      ftmItem->Packet.GetResponseWordFromOffset(GSM_MEAS_PWR_OFFSET, &dBmRssiPrx);
      ftmItem->Packet.GetResponseDwordFromOffset(GSM_MEAS_PWR_OFFSET+2, &rawRssiPrx);
      ftmItem->Packet.GetResponseWordFromOffset(GSM_MEAS_PWR_OFFSET+6, &dBmRssiDrx);
      ftmItem->Packet.GetResponseDwordFromOffset(GSM_MEAS_PWR_OFFSET+8, &rawRssiDrx);
      HalLogWrite("PRx: raw=%u, dBm16=%u, DRx: raw=%u, dBm16=%u\n", rawRssiPrx, dBmRssiPrx, rawRssiDrx, dBmRssiDrx);
      dBmRssiPrx = double_to_int16_q8(((int16)dBmRssiPrx)/16.0);
      dBmRssiDrx = double_to_int16_q8(((int16)dBmRssiDrx)/16.0);
      hal_resp->rssi_ts0 = dBmRssiPrx;  // Put PRx to slot 0
      hal_resp->rssi_ts1 = dBmRssiDrx;  // Put DRx to slot 1

      hal_resp->status = HAL_RF_STATUS_OK;
      break;
    }
    else if( _FTM_GET_RSSI ==  subSysCmdId )
    {
      const uint16 GET_RSSI_OFFSET = 8;
      dword rssi = 0x00000000;
      ftmItem->Packet.GetResponseDwordFromOffset(GET_RSSI_OFFSET, &rssi);
      HalLogWrite(" Raw RSSI %u\n", rssi);
      // For testing split to 2
      word rssi2;
      ftmItem->Packet.GetResponseWordFromOffset(GET_RSSI_OFFSET, &rssi2);
      hal_resp->rssi_ts0 = (int16_q8)rssi2;
      ftmItem->Packet.GetResponseWordFromOffset(GET_RSSI_OFFSET+sizeof(word), &rssi2);
      hal_resp->rssi_ts1 = (int16_q8)rssi2;

      hal_resp->status = HAL_RF_STATUS_OK;
      break;
    }

    ftmItem = ftmItem->NextItem;
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_gsm_rx_rssi_send(HAL_RF_GSM_RX_RSSI_REQ *data)
{

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_GSM_RX_RSSI_REQ), sizeof(HAL_RF_GSM_RX_RSSI_RESP), HAL_RF_GSM_RX_RSSI_RESP_ID, RFCA_gsm_rx_rssi_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_RX_RSSI_RESP* halResp = ((HAL_RF_GSM_RX_RSSI_RESP*)rfcaResp->HAL.HalResponse);

  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->channel = data->channel;
  halResp->rf_band = data->rf_band;

  // According the spec in failure case RSSI must be 0xFFFF so we initialize to that
  halResp->rssi_ts0 = (int16_q8) 0xFFFF;
  halResp->rssi_ts1 = (int16_q8) 0xFFFF;
  halResp->rssi_ts2 = (int16_q8) 0xFFFF;
  halResp->rssi_ts3 = (int16_q8) 0xFFFF;
  halResp->rssi_ts4 = (int16_q8) 0xFFFF;
  halResp->rssi_ts5 = (int16_q8) 0xFFFF;
  halResp->rssi_ts6 = (int16_q8) 0xFFFF;
  halResp->rssi_ts7 = (int16_q8) 0xFFFF;

  if(HAL_RF_PROTOCOL_SYSTEM_GSM != StateCache.RF_TECHNOLOGY)
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

  word ftmModeIdExtended = GetGsmExtendedModeIdFromModeId(ftmModeId);

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

    AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);
    RFCA_StateCacheSetBand(&StateCache, data->rf_band);

    AddFtmSetChan(data->hdr.operation_id, ftmModeId, data->channel);
    AddFtmSetRxBurst(data->hdr.operation_id, ftmModeId, 0, RX_FUNCTION_STOP_CONTINOUS_RX);

    AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_RSSI.rfConfig);
  }

  if(StateCache.CHANNEL != data->channel)
  {
    AddFtmSetChan(data->hdr.operation_id, ftmModeId, data->channel);
    StateCache.CHANNEL= data->channel;
    AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_RSSI.rfTune);
  }

  if(true == GetRxTxInteleavingFlag())
  {
    if(TRUE == StateCache.TX_STATE)
    { 
      AddFtmSetTxOff(data->hdr.operation_id, ftmModeId);
      RFCA_StateCacheSetTxState(&StateCache, false);  
    }
  }

  AddFtmSetRxBurst(data->hdr.operation_id, ftmModeId, 0, RX_FUNCTION_STOP_CONTINOUS_RX);  
  
  int16 pwrdBm16 = (int16)(int16_q8_to_double(data->expected_level)*16);
  AddFtmGsmSetupRxBurstForExpectedPower(data->hdr.operation_id, ftmModeIdExtended, pwrdBm16, 1);

  AddFtmGsmGetMeasPwrPrxDrx(data->hdr.operation_id, ftmModeIdExtended);

  AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_RSSI.rfExec);

  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
  }
  else
  {
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }

}

/*----------------------------------------------------------------------------*/
void RFCA_gsm_tx_pcl_receive(unsigned int rfcaID)
{
  PendingRfcaResponse * resp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == resp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_TX_PCL_RESP* hal_resp = ((HAL_RF_GSM_TX_PCL_RESP*)resp->HAL.HalResponse);
  HAL_RF_GSM_TX_PCL_REQ* hal_req = ((HAL_RF_GSM_TX_PCL_REQ*)resp->HAL.HalRequest);

  hal_resp->channel = hal_req->channel;
  hal_resp->rf_band = hal_req->rf_band;
  hal_resp->status = HAL_RF_STATUS_OK;

  //TODO Check errors
}

/*----------------------------------------------------------------------------*/
void RFCA_gsm_tx_pcl_send_frame_matrix(HAL_RF_GSM_TX_PCL_REQ *data)
{

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_GSM_TX_PCL_REQ), sizeof(HAL_RF_GSM_TX_PCL_RESP), HAL_RF_GSM_TX_PCL_RESP_ID, RFCA_gsm_tx_pcl_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_TX_PCL_RESP* halResp = ((HAL_RF_GSM_TX_PCL_RESP*)rfcaResp->HAL.HalResponse);

  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->channel = data->channel;
  halResp->rf_band = data->rf_band;

  if(HAL_RF_PROTOCOL_SYSTEM_GSM != StateCache.RF_TECHNOLOGY)
  {
    halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 ftmModeId = FTM_MODE_MAX_C;
  if(RFCA_FUNCTION_FAIL == GetFtmModeId(GetActiveRfDevice(), StateCache.RF_TECHNOLOGY, RF_SIGNAL_PATH_MAIN, &ftmModeId))
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

    AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);
    RFCA_StateCacheSetBand(&StateCache, data->rf_band);

    if(data->rf_band != StateCache.GSM.TX_POWER_LEVELS_READ_FOR_BAND)
    {
      word rfnvID;
      switch(data->rf_band)
      {
      case HAL_RF_BAND_GSM_850:
        if(RF_DEVICE_0 == GetActiveRfDevice())
        {
          rfnvID = RFNV_GSM_C0_GSM850_POWER_LEVELS_I;
        }
        else
        {
          rfnvID = RFNV_GSM_C2_GSM850_POWER_LEVELS_I;
        }
        StateCache.GSM.TX_POWER_LEVEL_COUNT = 15;
        break;
      case HAL_RF_BAND_GSM_900:
        if(RF_DEVICE_0 == GetActiveRfDevice())
        {
          rfnvID = RFNV_GSM_C0_GSM900_POWER_LEVELS_I;
        }
        else
        {
          rfnvID = RFNV_GSM_C2_GSM900_POWER_LEVELS_I;
        }
        StateCache.GSM.TX_POWER_LEVEL_COUNT = 15;
        break;
      case HAL_RF_BAND_GSM_1800:
        if(RF_DEVICE_0 == GetActiveRfDevice())
        {
          rfnvID = RFNV_GSM_C0_GSM1800_POWER_LEVELS_I;
        }
        else
        {
          rfnvID = RFNV_GSM_C2_GSM1800_POWER_LEVELS_I;
        }
        StateCache.GSM.TX_POWER_LEVEL_COUNT = 16;
        break;
      case HAL_RF_BAND_GSM_1900:
        if(RF_DEVICE_0 == GetActiveRfDevice())
        {
          rfnvID = RFNV_GSM_C0_GSM1900_POWER_LEVELS_I;
        }
        else
        {
          rfnvID = RFNV_GSM_C2_GSM1900_POWER_LEVELS_I;
        }
        StateCache.GSM.TX_POWER_LEVEL_COUNT = 16;
        break;
      default:
        HalLogWrite("%s: Invalid GSM band enum (%d)\n", __FUNCTION__, data->rf_band);
        halResp->status = HAL_RF_STATUS_FAILED;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
        return;
        break;
      }
      // Read NV
      //uint16 bufferSize = sizeof(StateCache.GSM.TX_POWER_LEVELS);
      byte tempNvBuffer[128];
      uint16 bufferSize = 128;
      if( (RFCA_FUNCTION_SUCCESS != RFCA_ReadRfNv(rfnvID, tempNvBuffer, &bufferSize)) || (0 == bufferSize) )
      {
        HalLogWrite("%s: Failed to read PCL table from NV (rfnvId = %d)\n", __FUNCTION__, rfnvID);
        halResp->status = HAL_RF_STATUS_FAILED;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
        return;
      }
      for(int i=0;i<StateCache.GSM.TX_POWER_LEVEL_COUNT;i++)
      {
        StateCache.GSM.TX_POWER_LEVELS[i] = ((word*)tempNvBuffer)[StateCache.GSM.TX_POWER_LEVEL_COUNT-i-1];
      }
      StateCache.GSM.TX_POWER_LEVELS_READ_FOR_BAND = data->rf_band;
    }

    AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.TX_PCL.rfConfig);
  }
  if(StateCache.CHANNEL != data->channel)
  {
    AddFtmSetChan(data->hdr.operation_id, ftmModeId, data->channel);
    StateCache.CHANNEL= data->channel;
    // Need to reset this so that adjustment happens for each channel
    StateCache.FREQ_ADJ_APPLIED = FALSE;
    {
      word dataSource;
      switch(data->tx_data_type)
      {
      case TEST_ALL_0:
        dataSource = TONE;
        break;
      case TEST_PSEUDO:
        dataSource = RANDOM_DATA;
        break;
      case TEST_STATIC:
        dataSource = BUFFER_DATA;
        break;
      default:
        HalLogWrite("Unsupported TX DATA TYPE (%d)\n", data->tx_data_type);
        halResp->status = HAL_RF_STATUS_FAILED;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
        return;
        break;
      }
      // TODO Some values are hard coded
      AddFtmSetTxBurst(data->hdr.operation_id, ftmModeId, 0, dataSource, 0, 0, 1);

    }

    AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.TX_PCL.rfTune);
  }

  if(StateCache.GSM.TX_SLOT_MODULATION != data->modulation
     || StateCache.GSM.TX_SLOT_ACTIVITY != data->ts_activity)
  {
    AddFtmSetTxOff(data->hdr.operation_id, ftmModeId);
    RFCA_StateCacheSetTxState(&StateCache, false);
    StateCache.GSM.TX_SLOT_MODULATION = data->modulation;
    StateCache.GSM.TX_SLOT_ACTIVITY = data->ts_activity;
  }

  for(byte slotNum=0; slotNum<8; slotNum++)
  {
    const uint16 MIN_PCL_GSM_850_AND_900 = 5;
    const uint16 MAX_PCL_GSM_850_AND_900 = 19;
    const uint16 MIN_PCL_GSM_1800_AND_1900 = 0;
    const uint16 MAX_PCL_GSM_1800_AND_1900 = 15;

    byte bitMask = (1<<slotNum);
    uint16 pclTableIndex = StateCache.GSM.TX_POWER_LEVEL_COUNT -1;
    if( 0!= (data->ts_activity & bitMask))
    {
      // Quick validation that we don't go out of array bounds
      if(HAL_RF_BAND_GSM_850 == data->rf_band || HAL_RF_BAND_GSM_900 == data->rf_band)
      {
        // Valid PCLs are from 5 to 19
        if( data->pcl[slotNum]<MIN_PCL_GSM_850_AND_900 || data->pcl[slotNum]>MAX_PCL_GSM_850_AND_900)
        {
          HalLogWrite("%s: Invalid PCL for slot %d, valid range is from %d to %d.", __FUNCTION__, slotNum, MIN_PCL_GSM_850_AND_900, MAX_PCL_GSM_850_AND_900);
          halResp->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
          return;
        }
        pclTableIndex = data->pcl[slotNum] - MIN_PCL_GSM_850_AND_900;
      }
      else
      {
        if( data->pcl[slotNum]>MAX_PCL_GSM_1800_AND_1900 )
        {
          (void)MIN_PCL_GSM_1800_AND_1900; // Silence compiler warning unused variable 'MIN_PCL_GSM_1800_AND_1900'
          HalLogWrite("%s: Invalid PCL for slot %d, valid range is from %d to %d.", __FUNCTION__, slotNum, MIN_PCL_GSM_1800_AND_1900, MAX_PCL_GSM_1800_AND_1900);
          halResp->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
          return;
        }
        pclTableIndex = data->pcl[slotNum];
      }
    }

    // Modulation mapping
    // MODULATION_GMSK is mapped to MCS 1,
    // MODULATION_8PSK is mapped to MCS 9,
    byte modulationType = 0xFF;
    if(MODULATION_GMSK == (data->modulation & bitMask))
    {
      modulationType = MCS1;
    }
    else
    {
      modulationType = MCS9;
    } // no default case, user is responsible of validating inputs

    if(0!=(data->ts_activity & bitMask))
    {
      AddFtmSetTxFrameMatrix(data->hdr.operation_id, ftmModeId, slotNum, 1,  StateCache.GSM.TX_POWER_LEVELS[pclTableIndex], modulationType );
    }
    else
    {
      // Follow the QRCT which set all zeros for unused slots: QLIB_FTM_SET_TX_FRAME_MATRIX(5,0,0,0)
      AddFtmSetTxFrameMatrix(data->hdr.operation_id, ftmModeId, slotNum, 0,  0, 0);
    }
  }

  if(FALSE == StateCache.TX_STATE)
  {
	AddFtmSetTxOn(data->hdr.operation_id, ftmModeId);
	RFCA_StateCacheSetTxState(&StateCache, true);
  }

  if(true == freqAdjustData.freqAdjEnabled)  
  {
    if(StateCache.FREQ_ADJ_APPLIED != TRUE)  
    {
      double centerFreqMhz = GSM_CalculateFrequency((HAL_RF_GSM_BAND)data->rf_band, data->channel, true);
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

  AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.TX_PCL.rfExec);

  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
  }
  else
  {
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_gsm_tx_pcl_send_sweep(HAL_RF_GSM_TX_PCL_REQ *data)
{

  static ftm_gsm_frame_header_type* nextFrameStart = NULL;
  static ftm_gsm_slot_payload_type* nextFrameSlotStart = NULL;
  ftm_gsm_config_header_type* gsmSweepConfig = (ftm_gsm_config_header_type*)(GetMasterGsmSweepConfig()->buffer);

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_GSM_TX_PCL_REQ), sizeof(HAL_RF_GSM_TX_PCL_RESP), HAL_RF_GSM_TX_PCL_RESP_ID, RFCA_gsm_tx_pcl_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_TX_PCL_RESP* halResp = ((HAL_RF_GSM_TX_PCL_RESP*)rfcaResp->HAL.HalResponse);

  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->channel = data->channel;
  halResp->rf_band = data->rf_band;

  if(HAL_RF_PROTOCOL_SYSTEM_GSM != StateCache.RF_TECHNOLOGY)
  {
    halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 currentBand = InvalidStateCacheValue;

  uint16 ftmModeId = FTM_MODE_MAX_C;
  if(RFCA_FUNCTION_FAIL == GetFtmModeId(GetActiveRfDevice(), StateCache.RF_TECHNOLOGY, RF_SIGNAL_PATH_MAIN, &ftmModeId))
  {
    halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  if(false == GetAtpSweepConfigured())
  {
    if(NULL != gsmSweepConfig)
    {
      HalLogWrite("GSM Sweep Config: Previous config buffer not NULL, cleaning up.\n");
      ResetGsmSweepConfig();
    }

    // Reset pointers
    gsmSweepConfig = NULL;
    nextFrameStart = NULL;
    nextFrameSlotStart = NULL;

    GetMasterGsmSweepConfig()->buffer = (uint8*)RFCA_Malloc(maxGsmSweepBufferSize);
    gsmSweepConfig = (ftm_gsm_config_header_type*)GetMasterGsmSweepConfig()->buffer;
    if(NULL == gsmSweepConfig)
    {
      MSG_ERROR("GSM Sweep Config: Failed to allocate memory for the sweep configuration.", 0, 0, 0);
      halResp->status = HAL_RF_STATUS_MEMORY_ALLOCATION_FAIL;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }

    memset(gsmSweepConfig, 0x00, maxGsmSweepBufferSize);
    gsmSweepConfig->num_frame = 0;
    gsmSweepConfig->num_device = 1;
    gsmSweepConfig->device_id[0] = GetDeviceFromActiveRfDevice();
    gsmSweepConfig->device_id[1] = 0;
    HalLogWrite("GSM Sweep Config: Number of devices = %d, Device[0] = %d, Device[1] = %d\n", gsmSweepConfig->num_device, gsmSweepConfig->device_id[0], gsmSweepConfig->device_id[1]);
    // First frame starts right after ftm_gsm_config_header_type
    nextFrameStart = (ftm_gsm_frame_header_type*)( ( (char*)gsmSweepConfig) + sizeof(ftm_gsm_config_header_type));
    // First slot for a frame is right after the ftm_gsm_config_header_type
    nextFrameSlotStart = (ftm_gsm_slot_payload_type*)( ( (char*)nextFrameStart) + sizeof(ftm_gsm_frame_header_type));

    masterGsmSweepConfig.used_bytes_count += sizeof(ftm_gsm_config_header_type);
  }
  else
  {
    if(MAX_NUM_OF_FRAMES < gsmSweepConfig->num_frame)
    {
      halResp->status = HAL_RF_STATUS_TOO_MANY_ITEMS_IN_SWEEP ;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }
  }

  unsigned short rfMode = -1;
  if(false == RFCA_GetCurrentBand(&currentBand) || data->rf_band != currentBand)
  {

    if(false == RFCA_ModeToFtmRfMode(&rfMode, StateCache.RF_TECHNOLOGY, data->rf_band))
    {
      halResp->status = HAL_RF_STATUS_FAILED;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }
  }

  nextFrameStart->band = (byte)rfMode;
  nextFrameStart->chan = data->channel;
  nextFrameStart->num_slot_payload = 0; // For now
  HalLogWrite("GSM Sweep Config: Band = %d, Channel = %d\n", rfMode, data->channel);

  masterGsmSweepConfig.used_bytes_count += sizeof(ftm_gsm_frame_header_type);

  for(byte slotNum=0; slotNum<8; slotNum++)
  {
    const uint16 MIN_PCL_GSM_850_AND_900 = 5;
    const uint16 MAX_PCL_GSM_850_AND_900 = 19;
    const uint16 MIN_PCL_GSM_1800_AND_1900 = 0;
    const uint16 MAX_PCL_GSM_1800_AND_1900 = 15;

    byte bitMask = (1<<slotNum);
    byte pclTableIndex = 0;
    if( 0 != (data->ts_activity & bitMask))
    {
      // Quick validation that we don't go out of array bounds
      if(HAL_RF_BAND_GSM_850 == data->rf_band || HAL_RF_BAND_GSM_900 == data->rf_band)
      {
        // Valid PCLs are from 5 to 19
        if( data->pcl[slotNum]<MIN_PCL_GSM_850_AND_900 || data->pcl[slotNum]>MAX_PCL_GSM_850_AND_900)
        {
          HalLogWrite("%s: Invalid PCL for slot %d, valid range is from %d to %d.", __FUNCTION__, slotNum, MIN_PCL_GSM_850_AND_900, MAX_PCL_GSM_850_AND_900);
          halResp->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
          return;
        }
        pclTableIndex = (MAX_PCL_GSM_850_AND_900 - data->pcl[slotNum]);
      }
      else
      {
        if( data->pcl[slotNum]>MAX_PCL_GSM_1800_AND_1900 )
        {
          (void)MIN_PCL_GSM_1800_AND_1900; // Silence compiler warning unused variable 'MIN_PCL_GSM_1800_AND_1900'
          HalLogWrite("%s: Invalid PCL for slot %d, valid range is from %d to %d.", __FUNCTION__, slotNum, MIN_PCL_GSM_1800_AND_1900, MAX_PCL_GSM_1800_AND_1900);
          halResp->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
          return;
        }
        pclTableIndex = (MAX_PCL_GSM_1800_AND_1900 - data->pcl[slotNum]);
      }

      // Modulation mapping
      // MODULATION_GMSK is mapped to MCS 1
      // MODULATION_8PSK is mapped to MCS 9, which is default
      byte modulationType = MCS9;
      if(MODULATION_GMSK == (data->modulation & bitMask))
      {
        modulationType = MCS1;
      }

      nextFrameSlotStart->device_id = GetDeviceFromActiveRfDevice();
      nextFrameSlotStart->slot_num = slotNum;
      nextFrameSlotStart->op_id = TX_PCL_MEAS_OP;
      nextFrameSlotStart->op_data.tx_pcl_meas_data.mcs = modulationType;
      nextFrameSlotStart->op_data.tx_pcl_meas_data.pcl = pclTableIndex;

      HalLogWrite("GSM Sweep Config: Slot = %d, Device = %d, Op = TX_PCL_MEAS_OP(0), Mcs = %d, pclTableIndex = %d\n", slotNum, 0, modulationType, pclTableIndex);
      nextFrameStart->num_slot_payload++;

      // Move to next slot
      nextFrameSlotStart++;
      masterGsmSweepConfig.used_bytes_count += sizeof(ftm_gsm_slot_payload_type);
    }
  }

  // Next frame starts right after previous one and the nextFrameSlotStart points to it already
  nextFrameStart = (ftm_gsm_frame_header_type*)nextFrameSlotStart;
  // First slot for a frame is right after the ftm_gsm_config_header_type
  nextFrameSlotStart = (ftm_gsm_slot_payload_type*)( ( (char*)nextFrameStart) + sizeof(ftm_gsm_frame_header_type));

  gsmSweepConfig->num_frame++;

  SetAtpSweepConfigured(true);

  // Send above FTM messages, if any, to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
  }
  else
  {
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_gsm_tx_pcl_send(HAL_RF_GSM_TX_PCL_REQ *data)
{
  if(false == gsmPclUseSweep)
  {
    HalLogWrite("RFCA_gsm_tx_pcl_send: Calling RFCA_gsm_tx_pcl_send_frame_matrix\n");
    RFCA_gsm_tx_pcl_send_frame_matrix(data);
  }
  else
  {
    HalLogWrite("RFCA_gsm_tx_pcl_send: Calling RFCA_gsm_tx_pcl_send_sweep\n");
    RFCA_gsm_tx_pcl_send_sweep(data);
  }
}

ftm_iq_capture_get_config_type gsmIqConfig = {FTM_IQ_MAX_ERROR, 0, 0, 0, 0};

/*----------------------------------------------------------------------------*/
void RFCA_gsm_rx_iq_receive(unsigned int rfcaID)
{
  PendingRfcaResponse * rfcaResp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_RX_IQ_RESP *hal_resp = (HAL_RF_GSM_RX_IQ_RESP*)rfcaResp->HAL.HalResponse;

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
    case FTM_GSM_C0_C:
    case FTM_GSM_C1_C:
    case FTM_GSM_C2_C:
    case FTM_GSM_C3_C:
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
              MSG_ERROR("IQ sample acquisition or config failed. FTM cmd id = %d", respFtmCmdId, 0, 0);
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
            HAL_RF_GSM_RX_IQ_RESP * newHalResponse = (HAL_RF_GSM_RX_IQ_RESP *)RFCA_Malloc(sizeof(HAL_RF_GSM_RX_IQ_RESP) + halPayloadSize);

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
            rfcaResp->HAL.SizeOfHalResponse = sizeof(HAL_RF_GSM_RX_IQ_RESP) + halPayloadSize;
            // Update to new HAL response
            hal_resp = newHalResponse;
            hal_resp->vector_length = returnedSampleSize;


            if( FTM_IQ_8_BIT_FORMAT == gsmIqConfig.sample_data_format )
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
            else if( FTM_IQ_16_BIT_FORMAT == gsmIqConfig.sample_data_format )
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

/*----------------------------------------------------------------------------*/
void RFCA_gsm_rx_iq_send(HAL_RF_GSM_RX_IQ_REQ *data)
{

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_GSM_RX_IQ_REQ), sizeof(HAL_RF_GSM_RX_IQ_RESP), HAL_RF_GSM_RX_IQ_RESP_ID, RFCA_gsm_rx_iq_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_GSM_RX_IQ_RESP* halResp = ((HAL_RF_GSM_RX_IQ_RESP*)rfcaResp->HAL.HalResponse);

  halResp->status = HAL_RF_STATUS_FAILED;
  halResp->rf_band = data->rf_band;
  halResp->channel = data->channel;
  halResp->ts_activity = data->ts_activity;
  halResp->iq_select = data->iq_select;
  halResp->vector_length = 0;

  if(HAL_RF_PROTOCOL_SYSTEM_GSM != StateCache.RF_TECHNOLOGY)
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

    AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);
    RFCA_StateCacheSetBand(&StateCache, data->rf_band);

    AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_IQ.rfConfig);
  }

  if(StateCache.CHANNEL != data->channel)
  {
    AddFtmSetChan(data->hdr.operation_id, ftmModeId, data->channel);
    StateCache.CHANNEL= data->channel;
    AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_IQ.rfTune);
  }

  if(FTM_IQ_MAX_ERROR == gsmIqConfig.ftm_error_code)
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
    gsmIqConfig.ftm_error_code = iqConfigData->ftm_error_code;
    gsmIqConfig.max_diag_sample_size = iqConfigData->max_diag_sample_size;
    gsmIqConfig.max_sample_size = iqConfigData->max_sample_size;
    gsmIqConfig.sample_data_format = iqConfigData->sample_data_format;
    gsmIqConfig.sampling_freq = iqConfigData->sampling_freq;
  }


  SetLnaRxRangeForRxLevel(HAL_RF_PROTOCOL_SYSTEM_GSM, data->hdr.operation_id, ftmModeId, data->expected_level);
  AddFtmSetRxContinuous(data->hdr.operation_id, ftmModeId, RX_FUNCTION_START_CONTINOUS_RX);
  AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_IQ.rfPreExec/2);

  AddFtmGsmCaptureIq(data->hdr.operation_id, ftmModeId, FTM_IQ_CAPTURE_SINGLE_SHOT);
  AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_IQ.rfPreExec/2);

  AddFtmIqCaptureAquireSamples(data->hdr.operation_id, ftmModeId, gsmIqConfig.max_sample_size, 0);
  AddFtmIqCaptureGetSamples(data->hdr.operation_id, ftmModeId, iqSampleOffset, iqSampleSize);
  AddFtmGsmCaptureIq(data->hdr.operation_id, ftmModeId, FTM_IQ_CAPTURE_STOP);
  AddFtmSetRxBurst(data->hdr.operation_id, ftmModeId, 0, RX_FUNCTION_STOP_CONTINOUS_RX);
  AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.RX_IQ.rfExec-GsmRfcaTimeConfig.RX_IQ.rfPreExec);

  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
  }
  else
  {
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }

}

