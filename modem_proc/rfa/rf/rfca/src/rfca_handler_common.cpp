/*!
@file
  rfca_handler_common.cpp

@brief
  Common or technology independent functions to trasfer RFCA commands to FTM commands and vice-versa.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca_handler_common.cpp#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/18/14   aki     Added support getting sweep results even when no results are expected
09/18/14   aki     Added support for compressed sweep payload
06/10/14   aki     Added HAL_RF_GET_FLEXCAL_SWEEP_RESULT
05/27/14   aki     Prevent sleep once sequence has started, add FLEXI_CAL_SWEEP
04/18/14   aki     Remove FLEX, update FLEX2, remove RadioSetup, stop GSM RX at end
03/20/14   aki     Added DUPLEXER_CAL and HAL_RF_SET_SECONDARY_CHAIN
03/03/14   aki     Changed 'halfSpeed' to 'fullSpeed'
03/03/14   aki     Added 'halfSpeed' to RFCA_RFFE_READ_CONFIG 
01/11/14   aki     Increase time for TX control from 30 ms to 60 ms due to CDMA
01/31/14   aki     Fix KW error MLK.MIGHT
01/27/14   aki     Set FTM fuctions as default for RX operations
01/24/14   aki     Removed RFCA_ASSERT and added debug hal info cases
12/12/13   aki     Added missing 'break' and added delay to TX control in RFIC 
12/10/13   aki     Added RFFE read support
12/09/13   aki     Removed CreateRxSweepConfig, added GetMasterDeviceMaskFromRxPath,
                   GetSlaveIdFromRxPath and AddSweepRxMeasure
11/26/13   nrk     Fixed compiler warnings
11/25/13   aki     Added HAL_RF_SWEEP_DEBUG_MESSAGES
11/25/13   aki     Added AddSweepRxConfig
11/19/13   aki     Helpers for main and div paths
11/14/13   aki     Updated version of antenna switching
11/14/13   aki     Added GetEfsSyncAfterTunerSelfCal and INFORM
11/14/13   aki     Added persistent TX mode
09/13/13   aki     Added ant switch support
08/13/13   aki     Set sweep config flag in FLEX CAL V2
08/12/13   aki     Fixed KW errors
08/09/13   aki     LNA threshold for GSM
08/09/13   aki     IQ and FTM mode id updates
08/07/13   sml     FTM changes to support GSM RxD 
07/30/13   aki     Added GetFtmModeIdV2
07/16/13   aki     Added FLEX_CAL and FLEX_CAL_V2 to tx arb 
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/24/13   aki     LTE sweep to use hardcoded BW and RB settings, TDS uses time as uS
                   Added RFIC write id HAL_RF_ARB_TX_TDD_LTE_PARM
06/18/13   aki     Added SetActiveRfDevice
06/18/13   aki     Move the rfca_common_msm_get_hw_timer_periodicty into RFCA as new
                   function to allow GPS only build pass
06/10/13   aki     Mainline RFCA 
06/06/13   aki     Updates to remove KW warnings and errors
06/04/13   aki     Support variable length op-codes
05/30/13   aki     Updated sweep result log message in error case
05/21/13   aki     Added support for LTETDD DAC via ARB TX
05/20/13   aki     Updated GSM sweep code
04/30/13   aki     Deprecate LTE and TDS LNA threshold setting, fix config parsing
                   not to support overwriting the timing config file 
04/29/13   aki     Added F3 message if RFNV write failed
04/29/13   aki     Change C4 path to C3 for CDMA in GetFtmModeId
04/25/13   aki     Added initial support for GSM sweep
04/13/13   aki     Added HAL_RF_STATUS_SWEEP_SEGMENT_OVERRUN to sweep result query
03/25/13   aki     Code to update timing config via RFIC write
02/01/13   aki     Updated state cache band and channel, support multi-band sweeps, 
                   added polling interval config item
01/16/13   aki     Added support for RX in sweep context and refactored code,
                   also sweep overrun is now reported as RFCA error
12/27/12   aki     Fixed TX OFF sending to be done only once
12/27/12   aki     Removed commong timing configs as they are now in GSM and TDS
12/12/12   aki     Remove unnecessary timing markers, enable freq adj by default
12/10/12   aki     Updated XO
11/29/12   tws     Rename FTM_GSM_C to FTM_GSM_C0_C to avoid confusion.
10/31/12   aki     Timing config data handling
10/02/12   aki     Remove RFCA_QMSL ifdefs
09/18/12   aki     Removed unnecessary header
09/14/12   aki     Added handling for TDS burst mode disabling at the end seq
09/14/12   aki     Added HAL_RF_DIAG_MODE_CHANGE and removed compiler warnings
09/11/12   aki     Log message HAL_RF_RX_LNA_RANGE_THRESHOLDS_TDSCDMA being obsolete
08/28/12   aki     Updated FREQ_ADJ argument passing
08/23/12   aki     Correct sweep result handling to work with LPM HDETs
08/16/12   aki     Added freqAdjustData, version date print, HAL_RF_SET_FREQ_ADJUST_DATA
08/02/12   aki     Ignore ESC sweep overrun error as it seems to be false alarm
07/23/12   aki 	   Fix to return correct staus for RFNV write
07/13/12   aki     Configurable sweep timings, TDSCDMA support, FtmModeId additions
05/25/12   aki     Lte timings, added CreateRxSweepConfig and AddNoOpSegment
04/23/12   aki     Lna range, dual carrier, new sweep, code cleanup, old KV and
                   envelope DC removed
02/24/12   aki     Comments, less magic numbers, PC version compatibility
02/17/12   aki     Added XO, THER and KV support
02/08/12   aki     Changes to fix possible heap corruptions
01/25/12   aki     Diag V2 delayed response implementation, update to booleans
10/28/11   aki     Initial revision

==============================================================================*/


#include "rfa_variation.h"
#include "comdef.h"
#include "stringl.h" /* for memscpy */

#include "rfca.h"
#include "rfca_defs.h"
#include "rfca_helper.h"
#include "rfca_mem_helper.h"
#include "rfca_packet_helper.h"

#include "rfca_dispatcher.h"
#include "rfca_hal_msg_def.h"
#include "rfca_diag_msg_def.h"
#include "rfca_state_cache.h"
#include "rfca_handler.h"
#include "rfca_to_ftm_adapter.h"
#include "rfca_pending_message_queue.h"
#include "rfca_ftm_package_helper.h"

#include "ftm.h"
#include "msg.h"
#include "ftm_sequencer_definitions.h"
#include "ftm_rfnv_dispatch.h"
#include "ftm_command_id.h"
extern "C" {
  #include "fs_public.h"
  }

uint8 V3_SWEEP_ENABLE_F3 = 0;

/*----------------------------------------------------------------------------*/
bool efsSyncAfterTunerSelfCal = true;

/*----------------------------------------------------------------------------*/
bool GetEfsSyncAfterTunerSelfCal()
{
  return efsSyncAfterTunerSelfCal;
}

bool persistenTxOnMode = false;
/*----------------------------------------------------------------------------*/
bool GetPersistentTxModeFlag()
{
  return persistenTxOnMode;
}

/*----------------------------------------------------------------------------*/
void SetPersistentTxModeFlag(bool newState)
{
  persistenTxOnMode = newState;
}

/*----------------------------------------------------------------------------*/
uint32 iqSampleOffset = 0;
uint32 iqSampleSize = 512;

/*----------------------------------------------------------------------------*/
struct LteFddTxArbParameters {
  uint16 bandwidth;
  uint16 numberOfRbs;
  uint16 startRb;
} LteFddTxArbParameterConfg = { LTE_BW_10M, 6, 22 };

/*----------------------------------------------------------------------------*/
const uint16 defaultLnaRangeForXo = 2;

/*----------------------------------------------------------------------------*/
int16_q8 lnaRangeForRxLevel_Gsm[MAX_NUMBER_OF_LNA_RANGE_OFFSETS] = {
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0)};

/*----------------------------------------------------------------------------*/
int16_q8 lnaRangeForRxLevel_Wcdma[MAX_NUMBER_OF_LNA_RANGE_OFFSETS] = {
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0)};

/*----------------------------------------------------------------------------*/
int16_q8 lnaRangeForRxLevel_Lte[MAX_NUMBER_OF_LNA_RANGE_OFFSETS] = {
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0)};

/*----------------------------------------------------------------------------*/
int16_q8 lnaRangeForRxLevel_Cdma[MAX_NUMBER_OF_LNA_RANGE_OFFSETS] = {
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0)};

/*----------------------------------------------------------------------------*/
int16_q8 lnaRangeForRxLevel_TdsCdma[MAX_NUMBER_OF_LNA_RANGE_OFFSETS] = {
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0),
  double_to_int16_q8(100.0)};


/*----------------------------------------------------------------------------*/
/*! Enumeration to define maximum periodicty duration in microseconds allowed 
for the timer, see ftm_common_msm.c */
#define RFCA_COMMON_O_STMR_MAX_PERIODICTY_US 40000




bool rxTxInterLeaving = false;
/*----------------------------------------------------------------------------*/
bool GetRxTxInteleavingFlag()
{
  return rxTxInterLeaving;
}

/*----------------------------------------------------------------------------*/
void SetRxTxInteleavingFlag(bool newState)
{
  rxTxInterLeaving = newState;
}

/*----------------------------------------------------------------------------*/
int32 rfca_common_msm_get_hw_timer_periodicty(void)
{
  return RFCA_COMMON_O_STMR_MAX_PERIODICTY_US;
}

/*----------------------------------------------------------------------------*/
uint16 GetLnaRangeForRxLevel( HAL_RF_PROTOCOL_SYSTEM system, int16_q8 rx_level)
{
  int16_q8 *lnaRangeForRxLevel = NULL;

  switch(system)
  {
    case HAL_RF_PROTOCOL_SYSTEM_GSM:
      lnaRangeForRxLevel = lnaRangeForRxLevel_Gsm;
    break;
    case HAL_RF_PROTOCOL_SYSTEM_WCDMA:
      lnaRangeForRxLevel = lnaRangeForRxLevel_Wcdma;
      break;
    case HAL_RF_PROTOCOL_SYSTEM_LTE:
      lnaRangeForRxLevel = lnaRangeForRxLevel_Lte;
      break;
    case HAL_RF_PROTOCOL_SYSTEM_CDMA:
      lnaRangeForRxLevel = lnaRangeForRxLevel_Cdma;
      break;
    case HAL_RF_PROTOCOL_SYSTEM_TD_SCDMA: // Intentional fall-thorough
      lnaRangeForRxLevel = lnaRangeForRxLevel_TdsCdma;
      break;
    default:
        HalLogWrite("Invalid system %d for %s\n", system, __FUNCTION__);
        return 0;
      break;
  }

  for(uint16 index = 0; index < MAX_NUMBER_OF_LNA_RANGE_OFFSETS; index++)
  {
    if(rx_level <= lnaRangeForRxLevel[index])
    {
      return index;
    }
  }
  return MAX_NUMBER_OF_LNA_RANGE_OFFSETS;
}


/*----------------------------------------------------------------------------*/
void SetLnaRxRangeForRxLevel(HAL_RF_PROTOCOL_SYSTEM system, uint16 opId, uint16 ftmModeId, uint16 rxThreshold)
{
  uint16 lnaRange = GetLnaRangeForRxLevel(system, rxThreshold);

  if(StateCache.RX_LNA_RANGE != lnaRange)
  {
    AddFtmSetLnaRange(opId, ftmModeId, lnaRange);
    StateCache.RX_LNA_RANGE = lnaRange;
  }
}

/*----------------------------------------------------------------------------*/
/*!
 @brief Second channel to be used for dual carrier.
*/
uint16 dualCarrierSecondChannel = 0xFFFF;

/*----------------------------------------------------------------------------*/
uint16 GetDualCarrierSecondChannel()
{
  return dualCarrierSecondChannel;
}

/*----------------------------------------------------------------------------*/
/*!
 @brief Second channel to be used for dual carrier.
*/
bool configHasBeenLoaded = false;

/*----------------------------------------------------------------------------*/
void SetConfigHasBeenLoaded(bool newState)
{
  configHasBeenLoaded = newState;
}

/*----------------------------------------------------------------------------*/
uint16 GetConfigHasBeenLoaded()
{
  return configHasBeenLoaded;
}


/*----------------------------------------------------------------------------*/
/*!
 @brief Active Rf device 
*/
RF_DEVICE_ID activeRfDevice = RF_DEVICE_0;

uint16 SetActiveRfDevice(uint16 rfDevice)
{
  if(RF_DEVICE_1 < rfDevice )
  {
    MSG_ERROR("Invalid device (%d) selected, probably error in an earlier RFIC write (HAL_RF_ACTIVE_DEVICE).", rfDevice, 0, 0);
    return RFCA_FUNCTION_FAIL;
  }

  activeRfDevice = (RF_DEVICE_ID)rfDevice;
  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
RF_DEVICE_ID GetActiveRfDevice()
{
  return activeRfDevice;
}

/*----------------------------------------------------------------------------*/
byte GetTxFlag()
{    
    if(RF_DEVICE_0 == GetActiveRfDevice())
    {
      return 1;
    }
    else if(RF_DEVICE_1 == GetActiveRfDevice())
    {
      return 4;
    }
    return 0;
}

/*----------------------------------------------------------------------------*/
byte GetMasterDeviceMask()
{
  return GetTxFlag();
}

/*----------------------------------------------------------------------------*/
byte GetMasterDeviceMaskFromRxPath(uint16 rxPath)
{
  // Fist device = 1, second device = 4

  byte msterDeviceMask = 0;
  switch(rxPath)
  {
    case RF_SIGNAL_PATH_MAIN:
    case RF_SIGNAL_DIVERSITY:
      msterDeviceMask = 1;
      break;
    case RF_SIGNAL_PATH_MAIN_2ND_TRANSCVR:
    case RF_SIGNAL_DIVERSITY_2ND_TRANSCVR:
      msterDeviceMask = 4;
      break;
  }

  return msterDeviceMask;
}

/*----------------------------------------------------------------------------*/
uint32 GetSlaveIdFromRxPath(uint16 rxPath)
{
  uint32 slaveId = 0xFFFFFFFF;
  switch(rxPath)
  {
    case RF_SIGNAL_PATH_MAIN:
    case RF_SIGNAL_DIVERSITY:
      slaveId = 0xFFFFFFF1;
      break;
    case RF_SIGNAL_PATH_MAIN_2ND_TRANSCVR:
    case RF_SIGNAL_DIVERSITY_2ND_TRANSCVR:
      slaveId = 0xFFFFF4FF;
      break;
  }

  return slaveId;
}

/*----------------------------------------------------------------------------*/
dword GetSlaveId(void)
{
    dword slaveId = 0xFFFFFFF1;
    if(RF_DEVICE_1 == GetActiveRfDevice())
    {
      slaveId = 0xFFFFF4FF;
    }
    return slaveId;
}

/*----------------------------------------------------------------------------*/
uint16 powerMode = CDMA_POWER_MODE_NORMAL;

/*----------------------------------------------------------------------------*/
uint16 GetPowerMode()
{
  return powerMode;
}

ftm_rf_technology_type HalTechToFtmTech(HAL_RF_PROTOCOL_SYSTEM halTech)
{
  ftm_rf_technology_type ftmTech = FTM_RF_TECH_UNKNOWN;
  switch(halTech)
  {
    case HAL_RF_PROTOCOL_SYSTEM_GSM:
      ftmTech = FTM_RF_TECH_GSM;
      break;
    case HAL_RF_PROTOCOL_SYSTEM_WCDMA:
      ftmTech = FTM_RF_TECH_WCDMA;
      break;
    case HAL_RF_PROTOCOL_SYSTEM_TD_SCDMA:
      ftmTech = FTM_RF_TECH_TDSCDMA;
      break;
    case HAL_RF_PROTOCOL_SYSTEM_LTE:
      ftmTech = FTM_RF_TECH_LTE;
      break;
    case HAL_RF_PROTOCOL_SYSTEM_CDMA:
      ftmTech = FTM_RF_TECH_CDMA;
      break;
    case HAL_RF_PROTOCOL_SYSTEM_MAX:
      break;
  }

  return ftmTech;
}

/*----------------------------------------------------------------------------*/
rfm_device_enum_type GetDeviceIdFromSignalPath(uint16 signalPath)
{
    rfm_device_enum_type rxFlag = RFM_INVALID_DEVICE;

    switch(signalPath)
    {
      case RF_SIGNAL_PATH_MAIN:
        rxFlag = RFM_DEVICE_0;
        break;
      case RF_SIGNAL_DIVERSITY:
        rxFlag = RFM_DEVICE_1;
        break;
      case RF_SIGNAL_PATH_MAIN_2ND_TRANSCVR:
        rxFlag = RFM_DEVICE_2;
        break;
      case RF_SIGNAL_DIVERSITY_2ND_TRANSCVR:
        rxFlag = RFM_DEVICE_3;
        break;
    }
    
    return rxFlag;
}

/*----------------------------------------------------------------------------*/
rfm_device_enum_type GetDeviceFromActiveRfDevice()
{
    rfm_device_enum_type rfmDeviceId = RFM_INVALID_DEVICE;

    switch(GetActiveRfDevice())
    {
      case RF_DEVICE_0:
        rfmDeviceId = RFM_DEVICE_0;
        break;
      case RF_DEVICE_1:
        rfmDeviceId = RFM_DEVICE_2;
        break;
    }
    
    return rfmDeviceId;
}

/*----------------------------------------------------------------------------*/
uint16 GetMainPathFromActiveRfDevice()
{
  HAL_RF_SIGNAL_PATH pathToReturn = RF_SIGNAL_PATH_MAIN;
  if(RF_DEVICE_1 == GetActiveRfDevice())
  {
    pathToReturn = RF_SIGNAL_PATH_MAIN_2ND_TRANSCVR;
  }    
  return pathToReturn;
}

/*----------------------------------------------------------------------------*/
uint16 GetDivPathFromActiveRfDevice()
{
  HAL_RF_SIGNAL_PATH pathToReturn = RF_SIGNAL_DIVERSITY;
  if(RF_DEVICE_1 == GetActiveRfDevice())
  {
    pathToReturn = RF_SIGNAL_DIVERSITY_2ND_TRANSCVR;
  }    
  return pathToReturn;
}

/*----------------------------------------------------------------------------*/
uint16 GetSecChainEnabledFromSignalPath(uint16 signalPath)
{
    switch(signalPath)
    {
      case RF_SIGNAL_DIVERSITY:
      case RF_SIGNAL_DIVERSITY_2ND_TRANSCVR:
        return 1;
        break;
      default:
        return 0;
        break;
    }
}

/*----------------------------------------------------------------------------*/
/*!
 @brief Get information if ATP sweep has been configured.
*/
bool aptSweepConfigured = false;

/*----------------------------------------------------------------------------*/
void SetAtpSweepConfigured(bool newState)
{
  aptSweepConfigured = newState;
}

/*----------------------------------------------------------------------------*/
bool GetAtpSweepConfigured()
{
  return aptSweepConfigured;
}

/*----------------------------------------------------------------------------*/
/*!
 @brief Get information if old FTM comamnds are used for RX
*/
bool useOldFtmForRx = true;

/*----------------------------------------------------------------------------*/
bool GetUseOldFtmForRxFlag()
{
  return useOldFtmForRx;
}

/*----------------------------------------------------------------------------*/
uint16 GetFtmModeId(uint16 rfDevice, uint16 technology, uint16 rxPath, uint16 *ftmModeId)
{

  switch(technology)
  {
  case HAL_RF_PROTOCOL_SYSTEM_GSM:
    if(RF_DEVICE_0 == rfDevice)
    {
      if(RF_SIGNAL_PATH_MAIN == rxPath)
      {
        *ftmModeId = FTM_GSM_C0_C;
      }
      else
      {
        *ftmModeId = FTM_GSM_C1_C;
      }
    }
    else if (RF_DEVICE_1 == rfDevice)
    {
      if(RF_SIGNAL_PATH_MAIN == rxPath)
      {
        *ftmModeId = FTM_GSM_C2_C;
      }
      else
      {
        *ftmModeId = FTM_GSM_C3_C;
      }
    }
    break;
  case HAL_RF_PROTOCOL_SYSTEM_WCDMA:
    if(RF_DEVICE_0 == rfDevice)
    {
      if(RF_SIGNAL_PATH_MAIN == rxPath)
      {
        *ftmModeId = FTM_WCDMA_C;
      }
      else
      {
        *ftmModeId = FTM_WCDMA_RX_2_C;
      }
    }
    else if (RF_DEVICE_1 == rfDevice)
    {
      if(RF_SIGNAL_PATH_MAIN == rxPath)
      {
        *ftmModeId = FTM_WCDMA_RX_3_C;
      }
      else
      {
        *ftmModeId = FTM_WCDMA_RX_4_C;
      }
    }
    break;
  case HAL_RF_PROTOCOL_SYSTEM_TD_SCDMA:
    if(RF_SIGNAL_PATH_MAIN == rxPath)
    {
      *ftmModeId = FTM_TDSCDMA_C;
    }
    else
    {
      *ftmModeId = FTM_TDSCDMA_RX_2_C;
    }
    break;
  case HAL_RF_PROTOCOL_SYSTEM_LTE:
    if(RF_DEVICE_0 == rfDevice)
    {
      if(RF_SIGNAL_PATH_MAIN == rxPath)
      {
        *ftmModeId = FTM_LTE_C;
      }
      else
      {
        *ftmModeId = FTM_LTE_RX_2_C;
      }
    }
    else if (RF_DEVICE_1 == rfDevice)
    {
      if(RF_SIGNAL_PATH_MAIN == rxPath)
      {
        *ftmModeId = FTM_LTE_CAR1_C;
      }
      else
      {
        *ftmModeId = FTM_LTE_CAR1_RX_2_C;
      }
    }
    break;
  case HAL_RF_PROTOCOL_SYSTEM_CDMA:
    if(RF_DEVICE_0 == rfDevice)
    {
      if(RF_SIGNAL_PATH_MAIN == rxPath)
      {
        *ftmModeId = FTM_1X_C0_C;
      }
      else
      {
        *ftmModeId = FTM_1X_C1_C;
      }   
    }
    else if (RF_DEVICE_1 == rfDevice)
    {
      if(RF_SIGNAL_PATH_MAIN == rxPath)
      {
        *ftmModeId = FTM_1X_C2_C;
      }
      else
      {
        *ftmModeId = FTM_1X_C3_C;
      }   
    }
    break;
  default:  
    MSG_ERROR("Undefined protocol system. Did you forget to call hal_rf_init?", 0, 0, 0);
    return RFCA_FUNCTION_FAIL;
    break;
  }

  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
uint16 GetFtmModeIdV2(uint16 technology, uint16 rxPath, uint16 *ftmModeId)
{

  const uint16 uintSupportedSystemMax = 5; /*HAL_RF_PROTOCOL_SYSTEM_MAX*/ 
  const uint16 uintSupportedPathMax = 4;   /*RF_SIGNAL_PATH_MAX*/ 
  
  *ftmModeId = FTM_MODE_MAX_C;

  uint16 ftmModeIds[uintSupportedSystemMax][uintSupportedPathMax] = {
    /* GSM   */ {FTM_GSM_C0_C,  FTM_GSM_C1_C,       FTM_GSM_C2_C,     FTM_GSM_C3_C},
    /* WCDMA */ {FTM_WCDMA_C,   FTM_WCDMA_RX_2_C,   FTM_WCDMA_RX_3_C, FTM_WCDMA_RX_4_C},
    /* TDS   */ {FTM_TDSCDMA_C, FTM_TDSCDMA_RX_2_C, FTM_MODE_MAX_C,   FTM_MODE_MAX_C}, // Mote Dev2 entries
    /* LTE   */ {FTM_LTE_C,     FTM_LTE_RX_2_C,     FTM_LTE_CAR1_C,   FTM_LTE_CAR1_RX_2_C},
    /* CDMA  */ {FTM_1X_C0_C,   FTM_1X_C1_C,        FTM_1X_C2_C,      FTM_1X_C3_C}
  };

  if(uintSupportedPathMax<=rxPath)
  {
    MSG_ERROR("Invalid RX path = %d, max suppoted value is %d", rxPath, uintSupportedPathMax-1, 0);
    return RFCA_FUNCTION_FAIL;
  }

  if(uintSupportedSystemMax<=technology)
  {
    MSG_ERROR("Invalid technology = %d, max suppoted value is %d", technology, uintSupportedSystemMax-1, 0);
    return RFCA_FUNCTION_FAIL;
  }

  *ftmModeId = ftmModeIds[technology][rxPath];

  // Special check for TDS
  if(FTM_MODE_MAX_C == *ftmModeId)
  {
    MSG_ERROR("Invalid technology = %d, and RX path combination", technology, rxPath, 0);
    return RFCA_FUNCTION_FAIL;
  }

  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
// V3 Sweep times
// TODO Consider adding and RFIC/INFORM function to change these
/*----------------------------------------------------------------------------*/
HAL_RF_TX_DAC_SEGMENT_DURATION_CONFIG enhSweepSegmentLength = {
  TX_DAC_DEFAULT_SEGMENT_LENGTH_RF_CONFIG, 
  TX_DAC_DEFAULT_SEGMENT_LENGTH_RF_TUNE, 
  TX_DAC_DEFAULT_SEGMENT_LENGTH_WV_CONFIG,
  250
};

uint16 atpTuningSegmentLength_Tx_Lte = (uint16)LTE_SUBFRAME_LENGTH_US;          // Set to 1ms as LTE_TX_DAC has fixed duration
uint16 atpTuningSegmentLength_Tx_TdsCdma = TDSCDMA_SUBFRAME_LENGTH_US;  // Set to 5ms as TDSCDMA_TX_DAC has fixed duration

byte numberOfHdetAvgerages_wcdma = 5;
byte numberOfHdetAvgerages_cdma = 8;
byte numberOfHdetAvgerages_tdscdma = 1;
byte numberOfHdetAvgerages_lte = 5;


/*----------------------------------------------------------------------------*/
// Frequency adjustment values to be used for TX DBM
// These are common for all techs where applicable
/*----------------------------------------------------------------------------*/
HAL_RF_FREQ_ADJUST_DATA freqAdjustData = {true, 0, 0, 1};

/*----------------------------------------------------------------------------*/
/*!
  @brief 
    Indicates is any FTM messages were sent to FTM sequcner for execution.

  @details
    We only execute FTM seqeuncer is something has been added to the queue.
*/
bool messagesHaveBeenSentToFtmSequencer = false;

/*----------------------------------------------------------------------------*/
void NewMessagesBeenSentToFtmSequencer(bool value)
{
  messagesHaveBeenSentToFtmSequencer = value;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief 
    HAL response for stop won't be sent until FTM sequence is complete.

  @details
    To send the stop response @ref RfcaRespDispatcher needs to know the HAL id.
*/
uint16 stopRequestHalId = 0;

/*----------------------------------------------------------------------------*/
uint16 GetStopRequestHalId()
{
  return stopRequestHalId;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief 
    Instance of RFCA state cache
 
  @details
    This should be used by all RFCA_*_send functions to check if phone is already
    in a mode/state expected to avoid queuing unnecessary FTM messages to sequencer.
*/
_RfcaStateCacheT StateCache;



/*----------------------------------------------------------------------------*/
void RFCA_init_send(HAL_RF_INIT_REQ *data)
{

  HalLogWrite("--------------------------- RFCA v. %s ---------------------------\n", HAL_RF_RFCA_VERSION);

  // ------------------ Rfca reset stuff ---------------------------

  // Clear all possibly pending messages from previous execution from RFCA queue
  ClearRfcaCommandQueue();
  
  SetAtpSweepConfigured(false);

  stopRequestHalId = 0;

  // ---------------------------------------------

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_INIT_REQ), sizeof(HAL_RF_INIT_RESP), HAL_RF_INIT_RESP_ID, NULL);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_INIT_RESP* halResp = (HAL_RF_INIT_RESP*)rfcaResp->HAL.HalResponse;
  
  halResp->status = HAL_RF_STATUS_FAILED;

  if(false == GetConfigHasBeenLoaded())
  {
    if(false == RFCA_RefreshConfigurationData())
    {
      MSG_ERROR("Failed to read RFCA timing config file", 0, 0, 0);
      halResp->status = HAL_RF_STATUS_FAILED;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }
  }

  if(RFCA_FUNCTION_SUCCESS != RFCA_FtmLogsEnable())
  {
    HalLogWrite("Failed initialize phone communication and logs\n");
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }
  if(RFCA_FUNCTION_SUCCESS != RFCA_FtmSequencerInit())
  {
    HalLogWrite("RFCA_FtmSequencerInit call failed\n");
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }
  messagesHaveBeenSentToFtmSequencer = false;

  RFCA_StateCacheReset(&StateCache);

  RFCA_StateCacheSetTechnology(&StateCache, (HAL_RF_PROTOCOL_SYSTEM)data->system);
  StateCache.MODEM_TEST_MODE_ACTIVE = TRUE;  

  halResp->status = HAL_RF_STATUS_OK;

  SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
}



/*----------------------------------------------------------------------------*/
void RFCA_ftm_sequencer_completed(void)
{
  PendingRfcaResponse * resp = FindFromRfcaResponseQueue(stopRequestHalId);

  if(NULL == resp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_STOP_RESP* hal_resp = ((HAL_RF_STOP_RESP*)resp->HAL.HalResponse);

  if(RFCA_FUNCTION_SUCCESS != RFCA_FtmSequencerInit())
  {
    HalLogWrite("RFCA_FtmSequencerInit call failed\n");
    hal_resp->status = HAL_RF_STATUS_FAILED;
  }
  else
  {
    hal_resp->status = HAL_RF_STATUS_OK;
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_stop_send(HAL_RF_STOP_REQ *data)
{

  // TODO: Right now we don not set callback for stop. Stop HAL response will be sent when sequence has been completed.
  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_STOP_REQ), sizeof(HAL_RF_STOP_RESP), HAL_RF_STOP_RESP_ID, NULL );  

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_STOP_RESP* halResp = (HAL_RF_STOP_RESP*)rfcaResp->HAL.HalResponse;
  
  halResp->status = HAL_RF_STATUS_FAILED;

  bool needToSendCmdsToSequencer = false;


  if(FALSE == StateCache.MODEM_TEST_MODE_ACTIVE)
  {
    // Init has not been called, there's nothing to do in stop
    HalLogWrite("RFCA not in active test mode, did you forget to call hal_rf_init()?\n");
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
    SendPopDelayedFromRfcaResponseQueue(rfcaResp->HalID);
    return;
  }

  if(false == GetPersistentTxModeFlag())
  {
    if( (TRUE == StateCache.TX_STATE) 
      || (TRUE == StateCache.TDSCDMA.BURST_TX_ENABLED)
      || (TRUE == StateCache.TDSCDMA.BURST_RX_ENABLED))
    {
      uint16 ftmModeId;
      if(RFCA_FUNCTION_FAIL == GetFtmModeId(GetActiveRfDevice(), StateCache.RF_TECHNOLOGY, RF_SIGNAL_PATH_MAIN, &ftmModeId))
      {
        MSG_ERROR("Something weird in stop, tx is on but current tech is not set!\n", 0, 0, 0);
        halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
        SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
        SendPopDelayedFromRfcaResponseQueue(rfcaResp->HalID);
        return;
      }

      if(TRUE == StateCache.TX_STATE)
      {
        AddFtmSetTxOff(data->hdr.operation_id, ftmModeId);
        RFCA_StateCacheSetTxState(&StateCache, FALSE);
        needToSendCmdsToSequencer = true;
      }
      if(TRUE == StateCache.TDSCDMA.BURST_TX_ENABLED)    
      {    
        AddFtmTdsCdmaBurstTx(data->hdr.operation_id, ftmModeId, 0);
        StateCache.TDSCDMA.BURST_TX_ENABLED = FALSE;
        needToSendCmdsToSequencer = true;
      }
      if(TRUE == StateCache.TDSCDMA.BURST_RX_ENABLED)    
      {    
        AddFtmTdsCdmaBurstRx(data->hdr.operation_id, ftmModeId, -1200, 0);
        StateCache.TDSCDMA.BURST_RX_ENABLED = FALSE;
        needToSendCmdsToSequencer = true;
      }
    }
  }

  if(true == GetAtpSweepConfigured())
  {
    if( HAL_RF_PROTOCOL_SYSTEM_GSM != StateCache.RF_TECHNOLOGY )
    {
      // Add no-op segment just to start the sweep
      FTM_Cal_Sweep_Opcode_Req_List_Item noOpItem;
      ResetOpcode(&(noOpItem.opCodeHeader));
      noOpItem.next = NULL;
      noOpItem.opCodeHeader.segLen = 1000;
      noOpItem.totalSize = sizeof(FTM_Cal_Sweep_Opcode_Req_List_Item);
      AddFtmCfgAndExecCalSweep(data->hdr.operation_id, 1, 1, V3_SWEEP_ENABLE_F3, 1, &noOpItem);
    }
    else
    {
      AddFtmGsmFrameSweepConfig(rfcaResp->HalID, GetMasterGsmSweepConfig());
      AddFtmGsmFrameSweepExec(data->hdr.operation_id);
      ResetGsmSweepConfig();
    }

    needToSendCmdsToSequencer = true;
    SetAtpSweepConfigured(false);
  }

  if((HAL_RF_PROTOCOL_SYSTEM_GSM == StateCache.RF_TECHNOLOGY) && (InvalidStateCacheValue != StateCache.GSM.RX_STATE))
  {  
    AddFtmSetRxBurst(data->hdr.operation_id, FTM_GSM_C0_C, 0, RX_FUNCTION_STOP_CONTINOUS_RX);
    needToSendCmdsToSequencer = true;
  }

  if(true == needToSendCmdsToSequencer)
  {
    // Send to sequencer
    RFCA_SendFtmPackagesToSequencer(rfcaResp);    
  }

  if(true == messagesHaveBeenSentToFtmSequencer)
  {
    if(RFCA_FUNCTION_SUCCESS != RFCA_FtmSequencerExecute())
    {
      HalLogWrite("Failed to start seqeuncer execution\n");
      halResp->status = HAL_RF_STATUS_FAILED;
      SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
      SendPopDelayedFromRfcaResponseQueue(rfcaResp->HalID);
      return;
    }
    RFCA_forbid_sleep();
    stopRequestHalId = rfcaResp->HalID;     
    messagesHaveBeenSentToFtmSequencer = false;

    // We need to ack the caller, final response will be sent back later
    halResp->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);

  } else
  {
    // This gets ugly....
    // We did not send any FTM commands here so we need to send the response manually
    // There's no point to run FTM sequencer

    halResp->status = HAL_RF_STATUS_OK;
    
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
    SendPopDelayedFromRfcaResponseQueue(rfcaResp->HalID);
    // Clear all possibly pending messages from previous execution from RFCA queue
    //ClearRfcaCommandQueue();
  }

  // Will reset cache and only leave the technology
  RFCA_StateCacheSetTechnology(&StateCache, StateCache.RF_TECHNOLOGY);
  //StateCache.MODEM_TEST_MODE_ACTIVE = false;

}

/*----------------------------------------------------------------------------*/
void RFCA_calibration_values_load_receive(unsigned int rfcaID)
{
  PendingRfcaResponse * resp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == resp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }
  
  HAL_RF_CALIBRATION_VALUES_LOAD_RESP* hal_resp = (HAL_RF_CALIBRATION_VALUES_LOAD_RESP*)resp->HAL.HalResponse;

  // By default we fail
  hal_resp->status = HAL_RF_STATUS_FAILED;

  // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = resp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    uint16 respMsgId = 0xFFFF;
    if(false == ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &respMsgId))
    {
      return;
    }

    switch(respMsgId)
    {
      case _FTM_LOAD_RF_NV:
      {
        word nvStatus = FTM_Sequencer_Error_General;
        ftmItem->Packet.GetResponseWordFromOffset(10, &nvStatus);

        if( RFCOMMON_NV_READ_FAILED != nvStatus)
        {
          hal_resp->status = HAL_RF_STATUS_OK;
        }
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
void RFCA_calibration_values_load_send (HAL_RF_CALIBRATION_VALUES_LOAD_REQ *data)
{
  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_CALIBRATION_VALUES_LOAD_REQ), sizeof(HAL_RF_CALIBRATION_VALUES_LOAD_RESP), HAL_RF_CALIBRATION_VALUES_LOAD_RESP_ID, RFCA_calibration_values_load_receive);  

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }
  
  HAL_RF_CALIBRATION_VALUES_LOAD_RESP* halResp = (HAL_RF_CALIBRATION_VALUES_LOAD_RESP*)rfcaResp->HAL.HalResponse;
  
  halResp->status = HAL_RF_STATUS_FAILED;

  if(TRUE == StateCache.TX_STATE)
  {
    uint16 ftmModeId;
    if(RFCA_FUNCTION_FAIL == GetFtmModeId(GetActiveRfDevice(), StateCache.RF_TECHNOLOGY, RF_SIGNAL_PATH_MAIN, &ftmModeId))
    {
      MSG_ERROR("Something weird in RFCA_calibration_values_load_send, tx is on but current tech is not set!", 0 ,0 ,0);
      halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);     
      return;
    }

    AddFtmSetTxOff(data->hdr.operation_id, ftmModeId);
    StateCache.TX_STATE = false;
  }

  AddFtmLoadRfNv(data->hdr.operation_id);

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
  return;

}


/*----------------------------------------------------------------------------*/
void RFCA_calibration_values_store_send(HAL_RF_CALIBRATION_VALUES_STORE_REQ *data)
{
  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_CALIBRATION_VALUES_STORE_REQ), sizeof(HAL_RF_CALIBRATION_VALUES_STORE_RESP), HAL_RF_CALIBRATION_VALUES_STORE_RESP_ID, NULL);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_CALIBRATION_VALUES_STORE_RESP* halResp = (HAL_RF_CALIBRATION_VALUES_STORE_RESP*)rfcaResp->HAL.HalResponse;
  halResp->status = HAL_RF_STATUS_FAILED;

  if(0 == efs_sync("/"))
  {
    halResp->status = HAL_RF_STATUS_OK;
  }

  // Send ACK only, no delayed response
  SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);

  return;

}

/*----------------------------------------------------------------------------*/
void RFCA_rfic_read_receive(unsigned int rfcaID)
{
  PendingRfcaResponse * resp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == resp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_RFIC_READ_RESP* hal_resp = (HAL_RF_RFIC_READ_RESP*)resp->HAL.HalResponse;
  HAL_RF_RFIC_READ_REQ* hal_req = (HAL_RF_RFIC_READ_REQ*)resp->HAL.HalRequest;

  // By default we fail
  hal_resp->status = HAL_RF_STATUS_FAILED;;

  // One HAL request can send multiple RFFE reads
  uint16 rffeIndex = 0;

  // Loop through ftmMessages
  FtmPacketQueueItem *ftmItem = resp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    uint16 respMsgId = 0xFFFF;
    ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &respMsgId);
      
    uint8 * responseData = ftmItem->Packet.GetResponseDataPtr();
    switch ( respMsgId ) 
    {
    case _FTM_GET_ADC_VAL:
    {
      hal_resp->status = HAL_RF_STATUS_OK;

      uint16 _iADC_Result_Offset = 0;
      if(responseData[2] == FTM_LTE_C)
      {
        // the LTE version of the response doesn't return ADC channel number, hence 2 byte less than other techs
        _iADC_Result_Offset = 6;
      }
      else
      {
        _iADC_Result_Offset = 8;
      }
      ftmItem->Packet.GetResponseWordFromOffset(_iADC_Result_Offset, &hal_resp->data_array[0]);
    }
    break;
    case _FTM_RFNV_READ:
    {
      ftm_rfnv_read_response_type * rfnv_resp = (ftm_rfnv_read_response_type * )responseData;

      hal_resp->data_length = 0;
      
      if(RFCOMMON_NV_READ_FAILED == rfnv_resp->header.err_code)
      {
        // We can exit  error has been set earlier and size is 0 so there nothing to do.
        HalLogWrite("RFNV READ FAILED\n");      
        break;
      }      

      // If item was not found and we got RFCOMMON_NV_INACTIVE_ITEM then we just say OK and return 0 bytes
      if(RFCOMMON_NV_INACTIVE_ITEM == rfnv_resp->header.err_code)
      {
        HalLogWrite("RFNV INACTIVE ITEM\n");      
        hal_resp->status = HAL_RF_STATUS_OK;
        break;        
      }

      // We succesfully read the RFNV item
      // Free old response as we did not know the correct size when hal_rf_rfic_read was called
      RFCA_Free(resp->HAL.HalResponse);
      resp->HAL.HalResponse = NULL;
      resp->HAL.SizeOfHalResponse = 0;
      
      uint16 reguestedDataLengthAsBytes = hal_req->data_length * sizeof(uint16);

      if(reguestedDataLengthAsBytes < rfnv_resp->header.rfnv_item_size)
      {
        rfnv_resp->header.rfnv_item_size = reguestedDataLengthAsBytes;
      }

      // TODO: Refactor this to a sub function as similar code is also in AddToRfcaResponseQueue function  
      unsigned int halPayloadSize = rfnv_resp->header.rfnv_item_size + ((rfnv_resp->header.rfnv_item_size&1)?1:0) + sizeof(HAL_RF_RFIC_ID_NV_ITEM_CFG); // Rounded up to even number      
      resp->HAL.HalResponse = RFCA_Malloc(sizeof(HAL_RF_RFIC_READ_RESP) + halPayloadSize);

      if(NULL == resp->HAL.HalResponse)
      {
        MSG_ERROR("Failed to re-allocate memory for new HAL response.\n",0 ,0 ,0);
        return;
      }
      resp->HAL.SizeOfHalResponse = sizeof(HAL_RF_RFIC_READ_RESP) + halPayloadSize;
            
      hal_resp = (HAL_RF_RFIC_READ_RESP*)resp->HAL.HalResponse;
      hal_resp->hdr.operation_id = rfcaID;
      hal_resp->data_length = halPayloadSize/2;
      HAL_RF_RFIC_ID_NV_ITEM_CFG* nvItemConfig= (HAL_RF_RFIC_ID_NV_ITEM_CFG* )hal_resp->data_array;
      nvItemConfig->size = rfnv_resp->header.rfnv_item_size;
      memscpy(nvItemConfig->data, rfnv_resp->header.rfnv_item_size, &responseData[16], rfnv_resp->header.rfnv_item_size);
      hal_resp->status = HAL_RF_STATUS_OK;
    }
    break;
    case _FTM_SET_PATH_DELAY:
    {
      hal_resp->status = HAL_RF_STATUS_OK;
      memscpy(&hal_resp->data_array[0], sizeof(word), &responseData[8], sizeof(word));
    }
    break;
    case _FTM_GET_ENH_THERM:
    {
      word status = 1;
      ftmItem->Packet.GetResponseWordFromOffset(10, &status);
      if(0 != status)
      {
        hal_resp->status = HAL_RF_STATUS_FAILED;
        hal_resp->data_array[0] = 0;
        hal_resp->data_array[1] = 0;
      }
      else
      {
        hal_resp->status = HAL_RF_STATUS_OK;
        memscpy(&hal_resp->data_array[0], sizeof(dword), &responseData[12], sizeof(dword));
      }

    }
    break;
    case _FTM_CALIBRATION_SWEEP_GET_RESULTS:
    {
      ftm_cal_sweep_get_results_rsp_pkt_type* calResponse = (ftm_cal_sweep_get_results_rsp_pkt_type*)ftmItem->Packet.GetResponseDataPtr();
      if((FTM_CALIBRATION_RESULTS_READ_SUCCESS == calResponse->status && FTM_CALIBRATION_RESULTS_READ_SUCCESS == calResponse->seg_error_code)
         || (HAL_RF_GET_FLEXCAL_SWEEP_RESULT == hal_req->identifier))
      {
        HalLogWrite("Cal Sweep Results success, payload length = %d\n",  calResponse->cal_result_len);
        hal_resp->status = HAL_RF_STATUS_OK;
        
        uint32 halPayloadSize =  0;
        // TODO: Refactor this to a sub function as similar code is also in AddToRfcaResponseQueue and if RFIC RFNV function              
        if(HAL_RF_SWEEP_RESULTS == hal_req->identifier)
        {

          // Count how many results 
          uint32 totalSizeScanned = 0;
          HdetReading* currentResultItem = (HdetReading*)&calResponse->cal_result;
          uint16 totalSweepResults = 0;
          while(totalSizeScanned < calResponse->cal_result_len)
          {
            totalSweepResults += (currentResultItem->resultSize)/2;
            totalSizeScanned += 3 + currentResultItem->resultSize;
            currentResultItem = (HdetReading *)((char*)currentResultItem + 3 + currentResultItem->resultSize);              
          }
          halPayloadSize =  totalSweepResults * sizeof(uint16);
        }
        else if(HAL_RF_SWEEP_RESULTS_V2 == hal_req->identifier)
        {
          // This is the case for V4 results which just return data from FTM
          halPayloadSize = calResponse->cal_result_len + ((calResponse->cal_result_len & 1)?1:0);
        }
        else { // HAL_RF_GET_FLEXCAL_SWEEP_RESULT
          // This is the case for V4 results which just return data from FTM, including status field
          uint16 structSize = sizeof(ftm_cal_sweep_get_results_rsp_pkt_type);
          uint16 headerSize = ((unsigned char*)&(calResponse->status)) - ((unsigned char*)&(calResponse->cmd));
          uint16 payloadSize = structSize - headerSize + calResponse->cal_result_len - sizeof(calResponse->cal_result);
          halPayloadSize = payloadSize + ((payloadSize & 1)?1:0);
		  HalLogWrite("V4 results: sizeof(ftm_cal_sweep_get_results_rsp_pkt_type) =  %d ", structSize);
		  HalLogWrite("V4 results: calculated headerSize =  %d ", headerSize);
		  HalLogWrite("V4 results: calculated halPayloadSize =  %d ", payloadSize);
        }

        // Re-allocate response       
        HAL_RF_RFIC_READ_RESP * newHalResponse = (HAL_RF_RFIC_READ_RESP *)RFCA_Malloc(sizeof(HAL_RF_RFIC_READ_RESP) + halPayloadSize);

        if(NULL == newHalResponse)
        {
          MSG_ERROR("Failed to re-allocate memory for new HAL response.\n", 0, 0, 0);
          return;
        }
    
        // Copy data from previously allocated response
        memscpy(newHalResponse, resp->HAL.SizeOfHalResponse, resp->HAL.HalResponse, resp->HAL.SizeOfHalResponse); 

        // Free old response
        RFCA_Free(resp->HAL.HalResponse);

        // Update to match new reponse
        resp->HAL.HalResponse = newHalResponse;
        resp->HAL.SizeOfHalResponse = sizeof(HAL_RF_RFIC_READ_RESP) + halPayloadSize;
        // Update to new HAL response
        hal_resp = newHalResponse;
        hal_resp->data_length = halPayloadSize / sizeof(uint16); 
       
        if(HAL_RF_SWEEP_RESULTS == hal_req->identifier)
        {
          uint32 totalSizeScanned = 0;
          HdetReading* currentResultItem = (HdetReading*)&calResponse->cal_result;
          uint16 totalSweepResults = 0;
          uint16 resultIndex = 0;
          while(totalSizeScanned < calResponse->cal_result_len)
          { 
            uint16 curretSweepResults = (currentResultItem->resultSize)/2;
            totalSweepResults += curretSweepResults;
            for(uint16 i=0; i<curretSweepResults; i++)
            {            
              hal_resp->data_array[resultIndex] = currentResultItem->resultData[i];
              resultIndex++;
            }              
            totalSizeScanned += 3 + currentResultItem->resultSize;
            currentResultItem = (HdetReading *)((char*)currentResultItem + 3 + currentResultItem->resultSize);              
          }
        }
        else if(HAL_RF_SWEEP_RESULTS_V2 == hal_req->identifier)
        {
          HalLogWrite("V4 results going to copy %d bytes", halPayloadSize);
          memscpy(&hal_resp->data_array, halPayloadSize, &calResponse->cal_result, halPayloadSize);
        }
        else { // HAL_RF_GET_FLEXCAL_SWEEP_RESULT
          uint16 structSize = sizeof(ftm_cal_sweep_get_results_rsp_pkt_type);
          uint16 headerSize = ((unsigned char*)&(calResponse->status)) - ((unsigned char*)&(calResponse->cmd));  
          uint16 payloadSize = structSize - headerSize + calResponse->cal_result_len - sizeof(calResponse->cal_result);

          HalLogWrite("V4 results going to copy %d bytes from FTM response starting from 'status' field", payloadSize);
          memscpy(&hal_resp->data_array, halPayloadSize, &calResponse->status, payloadSize);
        }
        hal_resp->status = HAL_RF_STATUS_OK;
      }
      else
      {
        MSG_ERROR("Cal Sweep Results failed: status = %u, error_code = %I64i, first segment to fail = %u\n",  calResponse->status, calResponse->seg_error_code, calResponse->faulty_segment_num);
        if(FTM_CALIBRATION_RESULTS_READ_SUCCESS == calResponse->status && ( calResponse->seg_error_code & ( 1 << FTM_CAL_SEG_ERR_SEG_DURATION_OVERRUN)))
        {
          hal_resp->status = HAL_RF_STATUS_SWEEP_SEGMENT_OVERRUN;
        }
        return;
      }
    }
    break;
    case _FTM_GSM_FRAME_SWEEP_GET_RESULTS_CMD:
    {
      if(sizeof(ftm_gsm_frame_sweep_response_type) < ftmItem->Packet.GetResponseLength())
      {
        HalLogWrite("Unexpected response size %d for GSM frame sweep results query\n", ftmItem->Packet.GetResponseLength() );
        hal_resp->status = HAL_RF_STATUS_FAILED;
        return;
      }

      ftm_gsm_frame_sweep_result_buffer_type* sweepResponse = (ftm_gsm_frame_sweep_result_buffer_type*)(&(ftmItem->Packet.GetResponseDataPtr()[10]));

      if( 0 == sweepResponse->empty_result_buffer)
      {
        MSG_ERROR("Not all responses received from phone, this situation cannot be handled at this point\n", 0, 0, 0);
        hal_resp->status = HAL_RF_STATUS_FAILED;
        return;
      }

      ftm_gsm_frame_sweep_response_type* sweepResults = (ftm_gsm_frame_sweep_response_type*)(&(sweepResponse->buffer_data));

      // Re-allocate response
      // TODO: Refactor this to a sub function as similar code is also in AddToRfcaResponseQueue and if RFIC RFNV function              
      unsigned int halPayloadSize =  sweepResponse->buffer_size - sizeof(sweepResults->payload_count);
      halPayloadSize += ((1 == (halPayloadSize & 1))?1:0); // Pad if needed
      HAL_RF_RFIC_READ_RESP * newHalResponse = (HAL_RF_RFIC_READ_RESP *)RFCA_Malloc(sizeof(HAL_RF_RFIC_READ_RESP) + halPayloadSize);

      if(NULL == newHalResponse)
      {
        MSG_ERROR("Failed to re-allocate memory for new HAL response.\n", 0, 0, 0);
        hal_resp->status = HAL_RF_STATUS_MEMORY_ALLOCATION_FAIL;
        return;
      }

      // Copy data from previously allocated response
      memscpy(newHalResponse, resp->HAL.SizeOfHalResponse, resp->HAL.HalResponse, resp->HAL.SizeOfHalResponse); 

      // Free old response
      RFCA_Free(resp->HAL.HalResponse);

      // Update to match new reponse
      resp->HAL.HalResponse = newHalResponse;
      resp->HAL.SizeOfHalResponse = sizeof(HAL_RF_RFIC_READ_RESP) + halPayloadSize;
      // Update to new HAL response
      hal_resp = newHalResponse;
      hal_resp->data_length = halPayloadSize/sizeof(uint16); 

      // Copy sweep result data
      memscpy(hal_resp->data_array, halPayloadSize, sweepResults->result_payload, halPayloadSize); 
              
      hal_resp->status = HAL_RF_STATUS_OK;
    }
    break;
    case FTM_DO_RFFE_CMD:
    {
      uint16 rffeAddressOffset = 16;
      uint16 rffeDataOffset = 18;
      word address;
      ftmItem->Packet.GetResponseWordFromOffset(rffeAddressOffset, &address);

      HalLogWrite("RFFE Response: halId = %d, address = %d, data = %d", rfcaID, address, responseData[rffeDataOffset]);
      hal_resp->data_array[rffeIndex] = responseData[rffeDataOffset];
      rffeIndex++;
      if(hal_resp->data_length == rffeIndex)
      {
        // All RFFE responses recevied
        hal_resp->status = HAL_RF_STATUS_OK;
        return;
      }
    }
    break;
    default:
      MSG_ERROR("Invalid FTM response from RFIC READ, did you add new command but not handler for response?", 0, 0, 0);
      break;
    }
    ftmItem = ftmItem->NextItem;
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_rfic_read_send(HAL_RF_RFIC_READ_REQ *data)
{
  // Note: Each case should call return(), otherwise default error response is sent 
  switch(data->identifier)
  {
  case HAL_RF_VENDOR_ID:
    {
      const uint16 responseDataSize = 1; // as uint16
      PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_READ_REQ), sizeof(HAL_RF_RFIC_READ_RESP) + responseDataSize * sizeof(uint16), HAL_RF_RFIC_READ_RESP_ID, NULL);

      if(NULL == rfcaResp)
      {
        MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
        return;
      }

      HAL_RF_RFIC_READ_RESP* halResp = (HAL_RF_RFIC_READ_RESP*)rfcaResp->HAL.HalResponse;
      halResp->status = HAL_RF_STATUS_OK;
      halResp->data_length = responseDataSize; // One 16-bit word
      halResp->data_array[0] = 0xFFFF;      
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);   
      return;
    }
    break;
  case HAL_RF_BUFFER_SIZE:
    {
      const uint16 responseDataSize = 2; // as uint16
      PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_READ_REQ), sizeof(HAL_RF_RFIC_READ_RESP) + responseDataSize * sizeof(uint16), HAL_RF_RFIC_READ_RESP_ID, NULL);

      if(NULL == rfcaResp)
      {
        MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
        return;
      }

      HAL_RF_RFIC_READ_RESP* halResp = (HAL_RF_RFIC_READ_RESP*)rfcaResp->HAL.HalResponse;
      halResp->status = HAL_RF_STATUS_OK;
      halResp->data_length = responseDataSize; 
      dword bufferSize = 100000;
      memscpy(halResp->data_array, sizeof(bufferSize), &bufferSize, sizeof(bufferSize));      
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);        
      return;
    }
    break;
  case HAL_RF_CSAPI_VERSION:
    {
      const uint16 responseDataSize = 1; // as uint16
      PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_READ_REQ), sizeof(HAL_RF_RFIC_READ_RESP) + responseDataSize * sizeof(uint16), HAL_RF_RFIC_READ_RESP_ID, NULL);

      if(NULL == rfcaResp)
      {
        MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
        return;
      }

      HAL_RF_RFIC_READ_RESP* halResp = (HAL_RF_RFIC_READ_RESP*)rfcaResp->HAL.HalResponse;

      halResp->status = HAL_RF_STATUS_OK;
      halResp->data_length = responseDataSize; 
      halResp->data_array[0] = (uint16)(HAL_RF_CHIPSET_API_VERSION*100);

      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);        
      return;
    }
    break;
  case HAL_RF_RF_LATENCY:
    {
      const uint16 responseDataSize = 1; // as uint16
      PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_READ_REQ), sizeof(HAL_RF_RFIC_READ_RESP) + responseDataSize * sizeof(uint16), HAL_RF_RFIC_READ_RESP_ID, NULL);

      if(NULL == rfcaResp)
      {
        MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
        return;
      }

      HAL_RF_RFIC_READ_RESP* halResp = (HAL_RF_RFIC_READ_RESP*)rfcaResp->HAL.HalResponse;
      halResp->status = HAL_RF_STATUS_OK;
      halResp->data_length = responseDataSize; 
#ifdef _MSC_VER
      // Widows desktop
      halResp->data_array[0] = 50;
#else
      // Embedded
      halResp->data_array[0] = 2;
#endif

      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }
    break;
  default:
    {
      // Following cases expect init to be called first
      if(TRUE != StateCache.MODEM_TEST_MODE_ACTIVE)
      {
        PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_READ_REQ), sizeof(HAL_RF_RFIC_READ_RESP), HAL_RF_RFIC_READ_RESP_ID, NULL);

        if(NULL == rfcaResp)
        {
          MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
          return;
        }

        HAL_RF_RFIC_READ_RESP* halResp = (HAL_RF_RFIC_READ_RESP*)rfcaResp->HAL.HalResponse;
        halResp->status = HAL_RF_STATUS_NOT_IN_TEST_MODE;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
        return;
      }
      switch (data->identifier)
      {
      case HAL_RF_NV_ITEM:
        {
          // TODO: Must realloc more memory in receive function when we know the actual RFNV item size
          PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_READ_REQ), sizeof(HAL_RF_RFIC_READ_RESP), HAL_RF_RFIC_READ_RESP_ID, RFCA_rfic_read_receive);

          if(NULL == rfcaResp)
          {
            MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
            return;
          }

          HAL_RF_RFIC_READ_RESP* halResp = (HAL_RF_RFIC_READ_RESP*)rfcaResp->HAL.HalResponse;
          halResp->status = HAL_RF_STATUS_FAILED;
          halResp->data_length = 0;
          AddFtmRfnvRead(data->hdr.operation_id, data->data_offset);

          // Send above FTM messages to UUT
          if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
          {
            halResp->status = HAL_RF_STATUS_FAILED;
            SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
          }
          else
          {
            // Send ACK response, actual RFNV data will be sent in delayed response
            halResp->status = HAL_RF_STATUS_OK;
            SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
          }
          // Return here for as this was added to FTM sequence or we failed
          return; 
        }
        break;
      case HAL_RF_ADC_READ:   
        {      
          const uint16 responseDataSize = 1; // as uint16
          PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_READ_REQ), sizeof(HAL_RF_RFIC_READ_RESP) + responseDataSize * sizeof(uint16), HAL_RF_RFIC_READ_RESP_ID, RFCA_rfic_read_receive);

          if(NULL == rfcaResp)
          {
            MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
            return;
          }

          HAL_RF_RFIC_READ_RESP* halResp = (HAL_RF_RFIC_READ_RESP*)rfcaResp->HAL.HalResponse;
          halResp->data_length = responseDataSize; // One 16-bit word

          uint16 ftmModeId = InvalidStateCacheValue;
          if(RFCA_FUNCTION_SUCCESS == GetFtmModeId(GetActiveRfDevice(), StateCache.RF_TECHNOLOGY, RF_SIGNAL_PATH_MAIN, &ftmModeId))
          {      
            AddFtmGetAdc(data->hdr.operation_id, ftmModeId, data->data_offset);

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

            // Return here for as this was added to FTM sequence
            return; 
          }
        }    	

        break;
      case HAL_RF_RFIC_READ:
        MSG_ERROR("Not implemented: HAL_RF_RFIC_READ", 0 ,0, 0);
        break; // Code block at the end will send an error
      case HAL_RF_THERMISTOR:
        {      
          const uint16 responseDataSize = 2; // as uint16
          PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_READ_REQ), sizeof(HAL_RF_RFIC_READ_RESP) + responseDataSize * sizeof(uint16), HAL_RF_RFIC_READ_RESP_ID, RFCA_rfic_read_receive);

          if(NULL == rfcaResp)
          {
            MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
            return;
          }

          HAL_RF_RFIC_READ_RESP* halResp = (HAL_RF_RFIC_READ_RESP*)rfcaResp->HAL.HalResponse;
          halResp->data_length = responseDataSize; // One 32-bit int

          // Need to set mode otherwise therm cannot be read, need to figure out how to config this
          uint16 rfMode = PHONE_MODE_GSM_900;
          uint16 ftmModeId = FTM_GSM_C0_C;


          AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);

          AddFtmGetEnhTherm(data->hdr.operation_id);

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

          // Return here for as this was added to FTM sequence
          return; 
        }   
        break;
      case HAL_RF_SWEEP_RESULTS:
      case HAL_RF_SWEEP_RESULTS_V2:
      case HAL_RF_GET_FLEXCAL_SWEEP_RESULT:
        {      
          // Wo don't know the number of HDET readings at this point, need to reallocate memory once TX DAC has been executed
          PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_READ_REQ), sizeof(HAL_RF_RFIC_READ_RESP), HAL_RF_RFIC_READ_RESP_ID, RFCA_rfic_read_receive);

          if(NULL == rfcaResp)
          {
            MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
            return;
          }

          HAL_RF_RFIC_READ_RESP* halResp = (HAL_RF_RFIC_READ_RESP*)rfcaResp->HAL.HalResponse;
          halResp->status = HAL_RF_STATUS_FAILED;
          halResp->data_length = 0;

          AddFtmGetCalSweepResults(data->hdr.operation_id);

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

          // Return here for as this was added to FTM sequence          
          return; 
        }   
        break;
      case HAL_RF_GSM_FRAME_SWEEP_RESULTS:
        {      
          // Wo don't know the number of results at this point, need to reallocate memory once TX DAC has been executed
          PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_READ_REQ), sizeof(HAL_RF_RFIC_READ_RESP), HAL_RF_RFIC_READ_RESP_ID, RFCA_rfic_read_receive);

          if(NULL == rfcaResp)
          {
            MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
            return;
          }

          HAL_RF_RFIC_READ_RESP* halResp = (HAL_RF_RFIC_READ_RESP*)rfcaResp->HAL.HalResponse;
          halResp->status = HAL_RF_STATUS_FAILED;
          halResp->data_length = 0;

          AddFtmGsmFrameSweepGetResults(data->hdr.operation_id);

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

          // Return here for as this was added to FTM sequence          
          return; 
        }   
        break;
      case HAL_RF_RFFE_READ:
        {      
          PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_READ_REQ), sizeof(HAL_RF_RFIC_READ_RESP) + (data->data_length * sizeof(uint16)), HAL_RF_RFIC_READ_RESP_ID, RFCA_rfic_read_receive);

          if(NULL == rfcaResp)
          {
            MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
            return;
          }

          HAL_RF_RFIC_READ_RESP* halResp = (HAL_RF_RFIC_READ_RESP*)rfcaResp->HAL.HalResponse;
          halResp->status = HAL_RF_STATUS_FAILED;
          halResp->data_length = data->data_length;

          RFCA_RFFE_READ_CONFIG *rffeCfg = (RFCA_RFFE_READ_CONFIG*)(&(data->data_offset));
          HalLogWrite("Reading %d RFFE fields from channel = %d, slave = %d, address = %d, fullSpeed = %d", data->data_length, rffeCfg->channel, rffeCfg->slave, rffeCfg->address, rffeCfg->fullSpeed);
          {
            int i=0;
            for(i=0; i<data->data_length; i++)
            {
              AddFtmDoRffeCmd(data->hdr.operation_id, 0, 1, rffeCfg->channel, rffeCfg->slave, rffeCfg->address + i, rffeCfg->fullSpeed^1 /* XOR */);
              halResp->data_array[i] = 0; // Reset to a known value
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
            halResp->status = HAL_RF_STATUS_OK;
            SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);  
          }

          // Return here for as this was added to FTM sequence          
          return; 
        }   
        break;
      default:
        HalLogWrite("Unknown command for RFIC READ\n");
        break;
      }
    }
    break;
  }
  {    
    // If we got this far something went wrong we need to send an error
    PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_READ_REQ), sizeof(HAL_RF_RFIC_READ_RESP), HAL_RF_RFIC_READ_RESP_ID, NULL);

    if(NULL == rfcaResp)
    {
      MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
      return;
    }

    HAL_RF_RFIC_READ_RESP* halResp = (HAL_RF_RFIC_READ_RESP*)rfcaResp->HAL.HalResponse;
    halResp->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  }
  return;
}

/*----------------------------------------------------------------------------*/
void RFCA_rfic_write_receive(unsigned int rfcaID)
{
  PendingRfcaResponse * resp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == resp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_RFIC_WRITE_RESP* hal_resp = (HAL_RF_RFIC_WRITE_RESP*)resp->HAL.HalResponse;

  // By default we are OK
  hal_resp->status = HAL_RF_STATUS_OK;

  // Right now only one messages was sent
  FtmPacketQueueItem *ftmItem = resp->FTM.FtmItems;

  uint16 respMsgId = 0xFFFF;
  ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &respMsgId);
  switch (respMsgId )
  {
  case _FTM_RFNV_WRITE:
    { 
      ftm_rfnv_write_response_type * rfnv_response= (ftm_rfnv_write_response_type *)ftmItem->Packet.GetResponseDataPtr();
      // See if write failed or not
      if(RFCOMMON_NV_WRITE_SUCCESS != rfnv_response->err_code)
      {
        MSG_ERROR("RFNV ID: %d failed to write item, error: %d\n", rfnv_response->rfnv_item_code, rfnv_response->err_code, 0);
        hal_resp->status = HAL_RF_STATUS_FAILED;
        return;
      }
    }
    break;
  case FTM_SET_PATH_SEL_OVERRIDE_INDEX:
    {
      const uint16 statusOffset = 5;

      byte* rawResponse = ftmItem->Packet.GetResponseDataPtr();
      byte status = rawResponse[statusOffset];
      if(0 == status)
      {
        hal_resp->status = HAL_RF_STATUS_OK;
        return;
      }
      else
      {
        HalLogWrite("FTM_SET_PATH_SEL_OVERRIDE_INDEX failed with error code %d\n", status);
        hal_resp->status = HAL_RF_STATUS_FAILED;
      }
    }
    break;
  default:    
    break;
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_rfic_write_send(HAL_RF_RFIC_WRITE_REQ *data)
{
  // TODO Turn this to a dispatcher and just call sub functions to do the actions 

  // Send response
  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_RFIC_WRITE_REQ), sizeof(HAL_RF_RFIC_WRITE_RESP), HAL_RF_RFIC_WRITE_RESP_ID, RFCA_rfic_write_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_RFIC_WRITE_RESP* halResp = (HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse;
  // Here we asssume that everything worked unless told otherwise
  halResp->status = HAL_RF_STATUS_OK;
  uint16 weFailed = 0;

  // TODO: The huge switch-case moster goes here....
  switch(data->identifier)
  {  
  case HAL_RF_NV_ITEM:
    {
      HAL_RF_RFIC_ID_NV_ITEM_CFG * nvItemConfig = (HAL_RF_RFIC_ID_NV_ITEM_CFG *)data->data_array;
      HalLogWrite("RFNV ID: %d, size: %d\n", data->data_offset, nvItemConfig->size);
      if(MAX_RFNV_ITEM_SIZE < nvItemConfig->size )
      {
        HalLogWrite("RFNV ID: %d item is too big, max size for now is %d bytes.\n", data->data_offset, MAX_RFNV_ITEM_SIZE);
        weFailed = 1;
      }
      else
      {
        AddFtmRfnvWrite(data->hdr.operation_id, data->data_offset, nvItemConfig->data, nvItemConfig->size);
      }
    }
    break;
  case HAL_RF_PDM_WRITE:   
    {

      uint16 ftmModeId = InvalidStateCacheValue;
      if(RFCA_FUNCTION_SUCCESS == GetFtmModeId(GetActiveRfDevice(), StateCache.RF_TECHNOLOGY, RF_SIGNAL_PATH_MAIN, &ftmModeId))
      { 
        AddFtmSetPdm(data->hdr.operation_id, ftmModeId, data->data_offset, data->data_array[0]);
      }
      else
      {
        weFailed = 1;
      }
    }
    break;
  case HAL_RF_FTM_SEQ_EXEC:   
    {      
      if(RFCA_FUNCTION_SUCCESS == RFCA_FtmSequencerExecute())
      {
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      }
      else
      {
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
      }
      // Need to respond right away      
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);

      return;      
    }
    break;
  case HAL_RF_CONFIG_TX:
    {
      HAL_RF_RFIC_ID_CONFIG_TX_CFG * configData = (HAL_RF_RFIC_ID_CONFIG_TX_CFG *)data->data_array;

      unsigned short rfMode = -1;
      uint16 ftmModeId = -1;
      // Try to get rf mode and ftm mode and fail this function if either fails
      if( (false == RFCA_ModeToFtmRfMode(&rfMode, StateCache.RF_TECHNOLOGY, configData->band) )
       || (RFCA_FUNCTION_FAIL == GetFtmModeId(GetActiveRfDevice(), StateCache.RF_TECHNOLOGY, RF_SIGNAL_PATH_MAIN, &ftmModeId)) )
      {        
        weFailed = 1;
        break;
      }
      
      AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);     
      RFCA_StateCacheSetBand(&StateCache, configData->band);
      
      AddFtmSeqAddWait(data->hdr.operation_id, 60000);

      AddFtmSetChan(data->hdr.operation_id, ftmModeId, configData->channel);
      StateCache.CHANNEL= configData->channel;
      
      AddFtmSeqAddWait(data->hdr.operation_id, 60000);

       //TODO Should we turn off TX?
      if(0 == configData->txState)
      {  
        AddFtmSetTxOff(data->hdr.operation_id, ftmModeId);
        //RFCA_StateCacheSetTxState(&StateCache, false);  
      }
      else
      {
        AddFtmSetTxOn(data->hdr.operation_id, ftmModeId);
        //RFCA_StateCacheSetTxState(&StateCache, true);  
      }
    }
    break;
  case HAL_RF_FTM_SEQ_WAIT:
    {
      dword waitTime;
      memscpy(&waitTime, sizeof(dword), (void*)(data->data_array), sizeof(dword));
      AddFtmSeqAddTimingMarker(data->hdr.operation_id);
      AddFtmSeqAddWait(data->hdr.operation_id, waitTime);
      AddFtmSeqAddTimingMarker(data->hdr.operation_id);
    }
    break;
  case HAL_CALIBRATION_STATE:
    {
      dword calStateTimeBudget = 5000;
      AddFtmSeqAddTimingMarker(data->hdr.operation_id);
      AddFtmSetCalibrationState(data->hdr.operation_id, (byte)data->data_array[0]);
      StateCache.CALIBRATION_STATE_ACTIVE = (( 1 == data->data_array[0])?TRUE:FALSE);
      if(data->data_length > 1 && 0 != data->data_array[1])
      {
        calStateTimeBudget = data->data_array[1] * 1000;
      }
      AddFtmSeqAddWait(data->hdr.operation_id, calStateTimeBudget);
    }
    break;
  case HAL_RF_FTM_SEQ_TIMING_MARKER:
    {
      AddFtmSeqAddTimingMarker(data->hdr.operation_id);
      
      // Send above FTM messages to UUT
      if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
      {
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;        
      }
      else
      {
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      }
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      // Exit here there won't be log message for TIMING MARKER from FTM sequencer
      return;
    }
    break;
  case HAL_RF_PARM_XO:
    {      
      for(uint16 offset = data->data_offset, array_index = 0; offset < (data->data_offset + data->data_length); offset++, array_index++)
      {
        switch(offset)
        {
        case HAL_RF_XO_PARAM_FREQUENCY_OFFSET_KHZ:
          currentXoParameters.offset = data->data_array[array_index]*1000;
          break;
        case HAL_RF_XO_PARAM_TEMP_SPAN:
          currentXoParameters.temp_span = data->data_array[array_index];
          break;
        case HAL_RF_XO_PARAM_MIN_FT_SAMPLE:
          currentXoParameters.min_ft_samples = data->data_array[array_index];
          break;
        case HAL_RF_XO_PARAM_MAX_FT_SAMPLE:
          currentXoParameters.max_ft_samples = data->data_array[array_index];
          break;
        case HAL_RF_XO_PARAM_SUBSTAGE_TIME_MS:
          currentXoParameters.substage_time = data->data_array[array_index];
          break;
        case HAL_RF_XO_PARAM_TIMEOUT_MS:
          currentXoParameters.timeout_time = data->data_array[array_index];
          break;
        case HAL_RF_XO_PARAM_WRITE_NV:
          currentXoParameters.update_nv = data->data_array[array_index];
          break;
        case HAL_RF_XO_PARAM_TEMP_GRADIENT_TIMEOUT:
          currentXoParameters.temp_grad_timeout = data->data_array[array_index];
          break;
        case HAL_RF_XO_PARAM_XTAL:
          currentXoParameters.xtal = data->data_array[array_index];
          break;
        default:
          HalLogWrite("Invalid XO offset value = %d.\n", offset);
          ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
          return;
          break;
        }
      }
      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_PARM_KVCO:
    {
      for(uint16 offset = data->data_offset, array_index = 0; offset < (data->data_offset + data->data_length); offset++, array_index++)
      {
        switch(offset)
        {
        case HAL_RF_KV_PARAM_NO_OF_TX_CYCLES:
          currentKvParameters.numberOfTxCycles = data->data_array[array_index];
          break;
        case HAL_RF_KV_PARAM_NO_OF_MEASUREMENTS:
          currentKvParameters.numberOfMeasurements = data->data_array[array_index];
          break;
        case HAL_RF_KV_PARAM_BAND_SELECT:
          {
            kv_channel defaultChannels[4] = KVCO_DEFAULT_CHANNEL_TABLE;

            uint16 bandToMmode[4] = {
              PHONE_MODE_GSM_850,
              PHONE_MODE_GSM_900,
              PHONE_MODE_GSM_1800,
              PHONE_MODE_GSM_1900
            };

            // Clear channel definitions
            memset(&currentKvParameters.channelList, 0, sizeof(currentKvParameters.channelList));

            currentKvParameters.numberOfBands = 0;
            const uint16 NUMBER_OF_GSM_BANDS = 4;
            for(uint16 index = 0, bit = HAL_RF_RFIC_KV_BAND_SELECT_850; index < NUMBER_OF_GSM_BANDS; index++, bit<<=1)
            {
              if(data->data_array[array_index] & bit)
              {
                currentKvParameters.channelList[currentKvParameters.numberOfBands].lowChannel = defaultChannels[index].lowChannel;
                currentKvParameters.channelList[currentKvParameters.numberOfBands].highChannel = defaultChannels[index].highChannel;
                currentKvParameters.gsmRfModeList[currentKvParameters.numberOfBands] = bandToMmode[index]; 
                currentKvParameters.numberOfBands++;
              }
            }
          }
          break;
        default:
          HalLogWrite("Invalid KV offset value = %d.\n", offset);        
          ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
          return;
          break;
        }
      }
      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_DUAL_CARRIER_CHANNEL:
    {
      dualCarrierSecondChannel = data->data_array[0];
      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_RX_LNA_RANGE_THRESHOLDS_LTE:       // Intentional fall-through
  case HAL_RF_RX_LNA_RANGE_THRESHOLDS_TDSCDMA:   
    {
      MSG_ERROR("RFIC ID: HAL_RF_RX_LNA_RANGE_THRESHOLDS_ TDSCDMA and LTE are obsolete.\n", 0, 0, 0);
      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_ARGUMENT_ERROR;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
  case HAL_RF_RX_LNA_RANGE_THRESHOLDS_WCDMA:
  case HAL_RF_RX_LNA_RANGE_THRESHOLDS_CDMA:   // Intentional fall-through
    {      
      if( MAX_NUMBER_OF_LNA_RANGE_OFFSETS < (data->data_offset + data->data_length))
      {
        HalLogWrite("Trying to write too many items for LNA Range array. Max number of items is %d\n", MAX_NUMBER_OF_LNA_RANGE_OFFSETS);
        weFailed = 1;
        break;
      }

      int16_q8 *lnaRangeForRxLevel = NULL;

      switch(data->identifier)
      {
        case HAL_RF_RX_LNA_RANGE_THRESHOLDS_GSM:
          lnaRangeForRxLevel = lnaRangeForRxLevel_Gsm;
          break;
        case HAL_RF_RX_LNA_RANGE_THRESHOLDS_WCDMA:
          lnaRangeForRxLevel = lnaRangeForRxLevel_Wcdma;
          break;
        case HAL_RF_RX_LNA_RANGE_THRESHOLDS_CDMA:
          lnaRangeForRxLevel = lnaRangeForRxLevel_Cdma;
          break;
        default:
          break;
      }

      for(uint16 offset = data->data_offset, array_index = 0; offset < (data->data_offset + data->data_length); offset++, array_index++)
      {
        lnaRangeForRxLevel[offset] = data->data_array[array_index];
      }
      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_SET_GSM_RX_BURST:
    if( data->data_length == 1)
    {
      AddFtmSetRxBurst(data->hdr.operation_id, FTM_GSM_C0_C, (byte)data->data_offset, data->data_array[0]);
    }
    else
    {
      weFailed = 1;
    }
    break;
  case HAL_RF_ACTIVE_DEVICE:
    {
      if(RFCA_FUNCTION_SUCCESS == SetActiveRfDevice(data->data_array[0]))
      {
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      }
      else
      {
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
      }
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_POWER_MODE:
    {
      powerMode = data->data_array[0];
      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_TX_DAC_SEGMENT_DURATION:
    {      
      for(uint16 offset = data->data_offset, array_index = 0; offset < (data->data_offset + data->data_length); offset++, array_index++)
      {
        switch(offset)
        {
        case HAL_RF_TX_DAC_SEGMENT_DURATION_CONFIG_OFFSET_RF_CONFIG:
          enhSweepSegmentLength.rfConfig = data->data_array[array_index];
          break;
        case HAL_RF_TX_DAC_SEGMENT_DURATION_CONFIG_OFFSET_RF_TUNE:
          enhSweepSegmentLength.rfTune = data->data_array[array_index];
          break;
        case HAL_RF_TX_DAC_SEGMENT_DURATION_CONFIG_OFFSET_WV_CONFIG:
          enhSweepSegmentLength.wvConfig = data->data_array[array_index];
          break;
        case HAL_RF_TX_DAC_SEGMENT_DURATION_LTE_TX:
          atpTuningSegmentLength_Tx_Lte = data->data_array[array_index];
          break;
        case HAL_RF_TX_DAC_SEGMENT_DURATION_TDSCDMA_TX:
          atpTuningSegmentLength_Tx_TdsCdma = data->data_array[array_index];
          break;
        case HAL_RF_TX_DAC_SEGMENT_DURATION_POLLING_INTERVAL:
          enhSweepSegmentLength.pollingInterval_us = data->data_array[array_index];
          break;
        default:
          HalLogWrite("Invalid TX DAC segment duration offset value = %d.\n", offset);
          ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
          return;
          break;
        }
      }
      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_FTM_SET_MODE:
    {
      AddFtmSetMode(data->hdr.operation_id, data->data_offset, data->data_array[0]);
    }
    break;
  case HAL_RF_FTM_PASS_THROUGH:
    {
      AddFtmRawMsg(data->hdr.operation_id, data->data_length*sizeof(uint16), (byte*)(&data->data_array[0]));
    }
    break;
  case HAL_RF_TX_DAC_NUM_OF_HDET_AVG:
    {      
      for(uint16 offset = data->data_offset, array_index = 0; offset < (data->data_offset + data->data_length); offset++, array_index++)
      {
        switch(offset)
        {
        case HAL_RF_TX_DAC_NUM_OF_HDET_AVG_WCDMA:
          numberOfHdetAvgerages_wcdma = (byte)data->data_array[array_index];
          break;
        case HAL_RF_TX_DAC_NUM_OF_HDET_AVG_CDMA:
          numberOfHdetAvgerages_cdma = (byte)data->data_array[array_index];
          break;
        case HAL_RF_TX_DAC_NUM_OF_HDET_AVG_TDSCDMA:
          numberOfHdetAvgerages_tdscdma = (byte)data->data_array[array_index];
          break;
        case HAL_RF_TX_DAC_NUM_OF_HDET_AVG_LTE:
          numberOfHdetAvgerages_lte = (byte)data->data_array[array_index];
          break;
        default:
          HalLogWrite("Invalid TX DAC HDET average offset value = %d.\n", offset);
          ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
          return;
          break;
        }
      }
      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_SET_FREQ_ADJUST_DATA:
    {      
      for(uint16 offset = data->data_offset, array_index = 0; offset < (data->data_offset + data->data_length); offset++, array_index++)
      {
        switch(offset)
        {
        case HAL_RF_FREQ_ADJUST_DATA_OFFSET_FREQ_ADJUST_ENABLED:
          freqAdjustData.freqAdjEnabled = (0 == data->data_array[array_index])?false:true;
          break;
        case HAL_RF_FREQ_ADJUST_DATA_OFFSET_USER_FREQ_ADJUST_PPB_LW:
          freqAdjustData.userFreqAdjustPpb = (data->data_array[array_index]) | (data->data_array[array_index+1]<<16);
          array_index++;
          offset++;
          break;
        case HAL_RF_FREQ_ADJUST_DATA_OFFSET_CENTER_FREQ_LW:
          freqAdjustData.centerFreqHz = (data->data_array[array_index]) | (data->data_array[array_index+1]<<16);
          array_index++;
          offset++;
          break;
        case HAL_RF_FREQ_ADJUST_DATA_OFFSET_ENABLE_XO_FREQ_ADJUST:
          freqAdjustData.enableXoFreqAdjust = (byte)(data->data_array[array_index]);
          break;
        default:
          HalLogWrite("Invalid FREQ ADJUST offset value = %d.\n", offset);
          ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
          return;
          break;
        }
      }
      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_TDSCDMA_RX_BURST_OFF:
    {
      dword rxBurstOffTimeBudget = 20000;
      AddFtmSeqAddTimingMarker(data->hdr.operation_id);
      AddFtmTdsCdmaBurstRx(data->hdr.operation_id, FTM_TDSCDMA_C, -450, 0);    
      StateCache.TDSCDMA.BURST_RX_ENABLED = FALSE;
      if(data->data_length > 0 && 0 != data->data_array[1])
      {
        rxBurstOffTimeBudget = data->data_array[0] * 1000;
      }
      AddFtmSeqAddWait(data->hdr.operation_id, rxBurstOffTimeBudget);
    }
    break;
  case HAL_RF_RELOAD_CONFIG_FILE:
    {
      if(true == RFCA_RefreshConfigurationData())
      {
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      }
      else
      {
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
      }
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }
    break;
  case HAL_RF_USE_OLD_FTM_FOR_RX:
    {
      if( data->data_length == 1)
      {
        useOldFtmForRx = (0 == data->data_array[0])?false:true;
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
        return;
      }
      else
      {
        weFailed = 1;
      }
    }
    break;
  case HAL_RF_UPDATE_CONFIG:
    {
      if(false == ParseConfigData((char*)(data->data_array)))
      {
        MSG_ERROR("Failed to parse RFCA timing config data", 0, 0, 0);
        halResp->status = HAL_RF_STATUS_ARGUMENT_ERROR;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
        return;
      }

      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_USE_GSM_FRAME_SWEEP_FOR_PCL:
    {
      if( data->data_length == 1)
      {
        SetGsmPclUseSweep((0 == data->data_array[0])?false:true);
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
        return;
      }
      MSG_ERROR("Wrong data_length (%d) for HAL_RF_USE_GSM_FRAME_SWEEP_FOR_PCL, must be 1", data->data_length, 0, 0);
      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_ARGUMENT_ERROR;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_ARB_TX_TDD_LTE_PARM:
    {
      for(uint16 offset = data->data_offset, array_index = 0; offset < (data->data_offset + data->data_length); offset++, array_index++)
      {
        switch(offset)
        {
        case HAL_RF_ARB_TX_TDD_LTE_PARM_OFFSET_BANDWIDTH:
          if(LTE_BW_MAX_HAL <= data->data_array[array_index])
          {
            MSG_ERROR("Invalid HAL_RF_ARB_TX_TDD_LTE_PARM bandwidth value = %d, max is %d\n", data->data_array[array_index], LTE_BW_MAX_HAL-1, 0);
            ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_ARGUMENT_ERROR;
            SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
            return;
          }
          LteFddTxArbParameterConfg.bandwidth = data->data_array[array_index];
          HalLogWrite("HAL_RF_ARB_TX_TDD_LTE_PARM:HAL_RF_ARB_TX_TDD_LTE_PARM_OFFSET_BANDWIDTH = %u", LteFddTxArbParameterConfg.bandwidth);
          break;
        case HAL_RF_ARB_TX_TDD_LTE_PARM_OFFSET_RBS:
          LteFddTxArbParameterConfg.numberOfRbs = data->data_array[array_index];
          HalLogWrite("HAL_RF_ARB_TX_TDD_LTE_PARM:HAL_RF_ARB_TX_TDD_LTE_PARM_OFFSET_RBS = %u", LteFddTxArbParameterConfg.numberOfRbs);
          break;
        case HAL_RF_ARB_TX_TDD_LTE_PARM_OFFSET_START_RB:
          LteFddTxArbParameterConfg.startRb = data->data_array[array_index];
          HalLogWrite("HAL_RF_ARB_TX_TDD_LTE_PARM:HAL_RF_ARB_TX_TDD_LTE_PARM_OFFSET_START_RB = %u", LteFddTxArbParameterConfg.startRb);
          break;
        default:
          MSG_ERROR("Invalid HAL_RF_ARB_TX_TDD_LTE_PARM offset value = %d.\n", offset, 0, 0);
          ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_ARGUMENT_ERROR;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
          return;
          break;
        }
      }
      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_IQ_SIZE_AND_OFFSET:
    {
      if( data->data_length == 2)
      {
        iqSampleSize = data->data_array[0];
        iqSampleOffset = data->data_array[1];        
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
        return;
      }
      MSG_ERROR("Wrong data_length (%d) for HAL_RF_IQ_SIZE_AND_OFFSET, must be 2", data->data_length, 0, 0);
      ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_ARGUMENT_ERROR;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
      return;
    }
    break;
  case HAL_RF_PATH_CTRL:
    {
      HAL_RF_RFIC_ANT_SWITCH_CFG *configData = (HAL_RF_RFIC_ANT_SWITCH_CFG *)(data->data_array);
      uint16 ftmModeId = FTM_MODE_MAX_C;
      uint16 currentBand = InvalidStateCacheValue;
      uint16 rfMode = -1;
      // Does not apply to GSM_COEX
      if(GSM_COEX != configData->path)
      {
        if(configData->system != StateCache.RF_TECHNOLOGY)
        {
          halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
          return;
        }

        if(RFCA_FUNCTION_FAIL == GetFtmModeId(GetActiveRfDevice(), configData->system, RF_SIGNAL_PATH_MAIN, &ftmModeId))
        {
          halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
          return;
        }

        // Check if the phone is already in correct mode
        if(false == RFCA_GetCurrentBand(&currentBand) || configData->band != currentBand)
        {
          if(false == RFCA_ModeToFtmRfMode(&rfMode, StateCache.RF_TECHNOLOGY, configData->band))
          {
            halResp->status = HAL_RF_STATUS_FAILED;
            SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
            return;
          }
        }
      }
      AddFtmSeqAddTimingMarker(data->hdr.operation_id);
      if(DPDT_MAIN_DIV == configData->path)
      {
        if(configData->band != currentBand)
        {
          // Set mode, see FTM_RF_Mode_Enum
          AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);
          RFCA_StateCacheSetBand(&StateCache, configData->band);
        }
        AddFtmSetAntenna(data->hdr.operation_id, GetDeviceFromActiveRfDevice(), configData->state, 1);
      }
      else if(GSM_COEX == configData->path)
      {
        AddFtmSetFilteredPath(data->hdr.operation_id, configData->state);
      }
      else if(DUPLEXER_CAL == configData->path)
      {
        AddFtmSetPathOverrideIndex(data->hdr.operation_id, FTM_LTE_C, 0, rfMode, configData->state, configData->state, configData->state);
      }
      else
      {
        MSG_ERROR("Invalide path value (%d) given for antenna swithing", configData->path, 0, 0);
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_ARGUMENT_ERROR;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
        return;
      }
      AddFtmSeqAddWait(data->hdr.operation_id, configData->duration * 1000);
    }
    break;
  case HAL_RF_SWEEP_DEBUG_MESSAGES:
    {
      if( data->data_length == 1)
      {
        V3_SWEEP_ENABLE_F3 = (0 == data->data_array[0])?0:1;
        HalLogWrite("HAL_RF_SWEEP_DEBUG_MESSAGES new value set to %d\n", V3_SWEEP_ENABLE_F3);
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
        return;
      }
      else
      {
        weFailed = 1;
      }
    }
    break;
  case HAL_RF_SET_SECONDARY_CHAIN:
    {
      uint16 ftmModeId = FTM_MODE_MAX_C;

      if(RFCA_FUNCTION_FAIL == GetFtmModeIdV2(StateCache.RF_TECHNOLOGY, ((1==data->data_array[0])?RF_SIGNAL_DIVERSITY:RF_SIGNAL_DIVERSITY_2ND_TRANSCVR), &ftmModeId))
      {
        halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
        return;
      }
      AddFtmSeqAddTimingMarker(data->hdr.operation_id);
      AddFtmSetSecondaryChain(data->hdr.operation_id, ftmModeId, data->data_array[1]);
      AddFtmSeqAddWait(data->hdr.operation_id, data->data_array[2] * 1000);
    }
    break;
  default:
    // Unknown command 
    MSG_ERROR("Error: Unknown RFIC ID: %d", data->identifier, 0, 0);
    weFailed = 1;
    break;
  }

  if(0 == weFailed)
  {
      // Send above FTM messages to UUT
      if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
      {
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;        
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      }
      else
      {
        // Send ACK response, actual RFNV data will be sent in delayed response
        ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
        SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
      }
  }
  else
  {
    // Something went wrong need to response right away
    ((HAL_RF_RFIC_WRITE_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  }  
 
}

/*----------------------------------------------------------------------------*/
void RFCA_arb_tx_receive(unsigned int rfcaID)
{
  PendingRfcaResponse * resp = FindFromRfcaResponseQueue(rfcaID);

  if(NULL == resp)
  {
    MSG_ERROR("Failed to find RFCA response from pending message queue. Terminating processing response.", 0, 0, 0);
    return;
  }

  HAL_RF_ARB_TX_RESP* hal_resp = ((HAL_RF_ARB_TX_RESP*)resp->HAL.HalResponse);
  // By default we fail
  // TODO Might need to think about if by default we should pass
  hal_resp->status = HAL_RF_STATUS_FAILED;
  hal_resp->data_length = 0;

  // Loop through ftmMessages
  //((HAL_RF_ARB_TX_REQ*)resp->HAL.HalRequest)->config_pattern;
  FtmPacketQueueItem *ftmItem = resp->FTM.FtmItems;
  while(NULL != ftmItem)
  {
    uint16 respMsgId = 0xFFFF;
    ftmItem->Packet.GetResponseWordFromOffset(FTM_CMD_ID_OFFSET, &respMsgId);

    if( _FTM_GSM_TX_GAIN_SWEEP == respMsgId )
    {
      uint16 status = 1;
      ftmItem->Packet.GetResponseWordFromOffset(10, &status);
      if(0 == status )   
      {
        hal_resp->status = HAL_RF_STATUS_OK;
        break;
      }
    }
    else if ( _FTM_TX_CFG2_AMAM_SWEEP_V2 == respMsgId )
    {
      uint16 status = 1;
      ftmItem->Packet.GetResponseWordFromOffset(12, &status);
      if(0 == status )      
      {
        hal_resp->status = HAL_RF_STATUS_OK;
        hal_resp->data_length = 1; // one word
        ftmItem->Packet.GetResponseWordFromOffset(10, &hal_resp->data_array[0]);
        break;
      }
    }
    else  if ( FTM_CALIBRATION_SWEEP == respMsgId || FTM_CALIBRATION_SWEEP_COMPRESSED == respMsgId )
    {
      const uint16 statusOffset = 10;
      char status = -127;

      if(ftmItem->Packet.GetResponseLength() < ENH_SWEEP_RESP_SIZE)
      {
        HalLogWrite("%s response is too short, expecting 14 got %d\n", 
                    (FTM_CALIBRATION_SWEEP_COMPRESSED == respMsgId ? "FTM_CALIBRATION_SWEEP_COMPRESSED" : "FTM_CALIBRATION_SWEEP"), 
                    ftmItem->Packet.GetResponseLength());
      }
      byte* rawResponse = ftmItem->Packet.GetResponseDataPtr();
      status = rawResponse[statusOffset];
      if(FTM_CALIBRATION_PACKET_ALLOCATED == status)
      {
        hal_resp->status = HAL_RF_STATUS_OK;
        break;
      }
      else
      {
        HalLogWrite("%s failed with status code %d\n", 
                    (FTM_CALIBRATION_SWEEP_COMPRESSED == respMsgId ? "FTM_CALIBRATION_SWEEP_COMPRESSED" : "FTM_CALIBRATION_SWEEP"),
                    status);
      }
    }
    ftmItem = ftmItem->NextItem;
  }

  //TODO Check errors
}


void RFCA_lte_tx_dac_flexible_send(HAL_RF_ARB_TX_REQ *data)
{

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_ARB_TX_REQ), sizeof(HAL_RF_ARB_TX_RESP), HAL_RF_ARB_TX_RESP_ID, RFCA_arb_tx_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_ARB_TX_RESP* halResp = (HAL_RF_ARB_TX_RESP*)rfcaResp->HAL.HalResponse;

  halResp->status = HAL_RF_STATUS_FAILED;

  if(HAL_RF_PROTOCOL_SYSTEM_LTE != StateCache.RF_TECHNOLOGY)
  {
    halResp->status = HAL_RF_STATUS_SYSTEM_MISMATCH;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

  uint16 ftmModeId = FTM_LTE_C;
  if(RFCA_FUNCTION_FAIL == GetFtmModeId(GetActiveRfDevice(), StateCache.RF_TECHNOLOGY, RF_SIGNAL_PATH_MAIN, &ftmModeId))
  {
    halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

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

    //AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);     
    //RFCA_StateCacheSetBand(&StateCache, data->rf_band);
  }

  uint16 segmentLength = data->step_duration;
  static uint16 lastChannel=0xFFFF;
  {

    uint32 totalSweepQueueSize = 0;
    uint16 number_of_config_steps = 0;
    FTM_Cal_Sweep_Opcode_Req_List_Item *head = NULL;
    FTM_Cal_Sweep_Opcode_Req_List_Item *tail = NULL;

    int32 timeLeftToDoNothinig = 0;

    if(false == GetAtpSweepConfigured())
    {
      // If this is the first call to TX DAC we need a config block
      // Add config and tx waveform config OP
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
        configPayload->txBW = LteFddTxArbParameterConfg.bandwidth;
        configPayload->rxBW = LteFddTxArbParameterConfg.bandwidth;
        configPayload->chanOffset = 0;

        lastChannel = data->channel;

        timeLeftToDoNothinig = enhSweepSegmentLength.rfConfig - configItem->opCodeHeader.segLen;
        head = configItem;
        tail = configItem;
      }

      // Add no OP
      if(RFCA_FUNCTION_FAIL == AddNoOpSegment(&tail, timeLeftToDoNothinig))
      {
        HalLogWrite("Could not allocate memory for sweep noOp item");
        halResp->status = HAL_RF_STATUS_FAILED;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  

        // Clear queue
        while(NULL != head)
        {
          tail = head->next;
          RFCA_Free(head);
          head = tail;
        }
        return;
      }

      // If this is the first call to TX DAC we need a config block
      // Add config and tx waveform config OP
      {
        FTM_Cal_Sweep_Opcode_Req_List_Item *configItem = NULL;
        const uint16 configItemSize = sizeof(FTM_Cal_Sweep_Opcode_Req_List_Item) + sizeof(FTM_Cal_Sweep_Tx_Wfm_Cfg_Req);
        configItem = (FTM_Cal_Sweep_Opcode_Req_List_Item *)RFCA_Malloc(configItemSize);
        if(NULL == configItem)
        {
          HalLogWrite("Could not allocate memory for sweep config item");
          halResp->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
          // Clear queue
          while(NULL != head)
          {
            tail = head->next;
            RFCA_Free(head);
            head = tail;
          }
          return;
        }
        totalSweepQueueSize += configItemSize;
        number_of_config_steps++;
        configItem->next = NULL;
        configItem->totalSize = configItemSize;

        ResetOpcode(&(configItem->opCodeHeader));
        SetFtmCalSweepOpCodeBit(&(configItem->opCodeHeader), FTM_CAL_OPCODE_CONFIG_TX_WF);

        configItem->opCodeHeader.segLen = (word)((enhSweepSegmentLength.wvConfig>rfca_common_msm_get_hw_timer_periodicty())?rfca_common_msm_get_hw_timer_periodicty():enhSweepSegmentLength.wvConfig);

        FTM_Cal_Sweep_Tx_Wfm_Cfg_Req* wfConfigPayload = (FTM_Cal_Sweep_Tx_Wfm_Cfg_Req*)(&configItem->opCodeHeader.opCodePayload);

        wfConfigPayload->numRBs = LteFddTxArbParameterConfg.numberOfRbs;
        wfConfigPayload->startRB = LteFddTxArbParameterConfg.startRb;
        wfConfigPayload->txWaveform = 1; // PUSCH

        timeLeftToDoNothinig = enhSweepSegmentLength.wvConfig - configItem->opCodeHeader.segLen;
        tail->next = configItem;
        tail = configItem;
      }

    }
    else
    {
      if(data->channel != lastChannel)
      {
        // Subsequent call to TX DAC we only add re tune block
        // If caller used different band then the sweep will fail

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

        configItem->opCodeHeader.segLen = (word)((enhSweepSegmentLength.rfTune>rfca_common_msm_get_hw_timer_periodicty())?rfca_common_msm_get_hw_timer_periodicty():enhSweepSegmentLength.rfTune);

        FTM_Cal_Sweep_Retune_Req* configPayload = (FTM_Cal_Sweep_Retune_Req*)(&configItem->opCodeHeader.opCodePayload);
        configPayload->ulChan = data->channel;
        configPayload->chanOffset = 0;
        configPayload->masterDev = GetMasterDeviceMask();
        timeLeftToDoNothinig = enhSweepSegmentLength.rfTune - configItem->opCodeHeader.segLen;
        head = configItem;
        tail = configItem;

        lastChannel = data->channel;

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
        HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA_APT txCtrlData = ((HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA_APT*)data->data_array)[step];


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
            hdetPayload->numHdetAvg = numberOfHdetAvgerages_lte;
          }
          if( lpHdetBitMask & txCtrlData.paState )
          {
            hdetPayload->numLpmHdetAvg = numberOfHdetAvgerages_lte;
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
void RFCA_arb_tx_send(HAL_RF_ARB_TX_REQ *data)
{
  uint16 additionalResponseDataLength = 0; // This is number of uint16, see data_length in HAL_RF_ARB_TX_RESP
  switch(data->config_pattern)
  {
    case GSM_PREDISTRORTION_CAL: 
      additionalResponseDataLength = 1; 
      break;
  }

  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_ARB_TX_REQ), sizeof(HAL_RF_ARB_TX_RESP) + (additionalResponseDataLength*sizeof(uint16)), HAL_RF_ARB_TX_RESP_ID, RFCA_arb_tx_receive);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_ARB_TX_RESP* halResp = (HAL_RF_ARB_TX_RESP*)rfcaResp->HAL.HalResponse;
  halResp->data_length = additionalResponseDataLength;

  uint16 ftmModeId = FTM_MODE_MAX_C;
  
  // Get FTM mode id
  if(RFCA_FUNCTION_FAIL == GetFtmModeId(GetActiveRfDevice(), StateCache.RF_TECHNOLOGY, RF_SIGNAL_PATH_MAIN, &ftmModeId))
  {
    halResp->status = HAL_RF_STATUS_INVALID_RF_DEVICE;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
    return;
  }

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
  }

  if(GSM_DRIVER_AMP_CAL == data->config_pattern || GSM_PREDISTRORTION_CAL == data->config_pattern)
  {

    if(data->rf_band != currentBand)
    {
      AddFtmSeqAddTimingMarker(data->hdr.operation_id); 

      AddFtmSetMode(data->hdr.operation_id, ftmModeId, rfMode);
      RFCA_StateCacheSetBand(&StateCache, data->rf_band);

      if(GSM_DRIVER_AMP_CAL == data->config_pattern)
      {
        AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.ARB_TX.DRIVER_AMP.rfConfig); 
      }
      else // GSM_PREDISTRORTION_CAL == data->config_pattern
      {
        AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.ARB_TX.PRE_DIST.rfConfig); 
      }
    }
    {
      //AddFtmSetTxBurst(data->hdr.operation_id, ftmModeId, 0, 0, 5, 0, 1);

      if(StateCache.CHANNEL != data->channel)
      {
        AddFtmSetChan(data->hdr.operation_id, ftmModeId, data->channel);
        StateCache.CHANNEL= data->channel;
        if(GSM_DRIVER_AMP_CAL == data->config_pattern)
        {
          AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.ARB_TX.DRIVER_AMP.rfTune); 
        }
        else // GSM_PREDISTRORTION_CAL == data->config_pattern
        {
          AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.ARB_TX.PRE_DIST.rfTune); 
        }
      }
              
      word ftmModeIdExtended = GetGsmExtendedModeIdFromModeId(ftmModeId);             
      if(GSM_DRIVER_AMP_CAL == data->config_pattern)
      {
        GSM_GAIN_SWEEP_DATA *sweepData = (GSM_GAIN_SWEEP_DATA *)RFCA_Malloc(data->number_of_steps*sizeof(GSM_GAIN_SWEEP_DATA));
        if(NULL == sweepData)
        {
          halResp->status = HAL_RF_STATUS_FAILED;
          SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
          return;
        }
        for(int i= 0; i<data->number_of_steps; i++)
        {
          GSM_DRIVER_AMP_CAL_DATA daCalData = ((GSM_DRIVER_AMP_CAL_DATA*)(data->data_array))[i];
          sweepData[i].segmentLength = data->step_duration;
          sweepData[i].rgi =  daCalData.rgi;
          sweepData[i].gsmMode =  daCalData.gsmMode;          
          sweepData[i].paRange =  (byte)(daCalData.paRange);
        }
        AddFtmSetPdm(data->hdr.operation_id, ftmModeId, 0, 0);      
        AddFtmSetTxOff(data->hdr.operation_id, ftmModeId);
        RFCA_StateCacheSetTxState(&StateCache, false);
        AddFtmGsmTxGainSweep(data->hdr.operation_id, ftmModeIdExtended, data->number_of_steps, sweepData);
        AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.ARB_TX.DRIVER_AMP.rfExec); 
        RFCA_Free(sweepData);
        sweepData = NULL;
      }
      else // GSM_PREDISTRORTION_CAL
      {
        AddFtmSeqAddTimingMarker(data->hdr.operation_id); 

        GSM_PREDISTORTION_CAL_DATA *calData = (GSM_PREDISTORTION_CAL_DATA*) data->data_array;
        //if( (data->step_duration * 4) != (calData->preDcDuration + calData->edgeDuration))
        //{            
        //    ((HAL_RF_ARB_TX_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
        //    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);    
        //    return;
        //}

        AddFtmSetTxOff(data->hdr.operation_id, ftmModeId);
        RFCA_StateCacheSetTxState(&StateCache, false);

        AddFtmSetGsmLinearPaRange(data->hdr.operation_id, ftmModeId, 0, 0);
        AddFtmTxCfg2AmamSweepV2(data->hdr.operation_id, ftmModeIdExtended, calData->preDcDuration, calData->edgeDuration, calData->calRgi);
        AddFtmSeqAddWait(data->hdr.operation_id, GsmRfcaTimeConfig.ARB_TX.PRE_DIST.rfExec); 
      }

    }
  }
  else if(TDSCDMA_TX_CAL == data->config_pattern)
  {
    // TDSCDMA DAC
    uint16 segmentLength = (uint16)(data->step_duration);
    {

      uint32 totalSweepQueueSize = 0;
      FTM_Cal_Sweep_Opcode_Req_List_Item *head = NULL;
      FTM_Cal_Sweep_Opcode_Req_List_Item *tail = NULL;

      int32 timeLeftToDoNothinig = 0;

      if(data->rf_band != currentBand)
      {
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
        if(StateCache.CHANNEL != data->channel)
        {
          // Subsequent call to TX DAC we only add re tune block
          // If caller used different band then the sweep will fail

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
        HalLogWrite("Could not allocate memory for sweep noOp item");
        halResp->status = HAL_RF_STATUS_FAILED;
        SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  

        // Clear queue
        while(NULL != head)
        {
          tail = head->next;
          RFCA_Free(head);
          head = tail;
        }
        return;
      }

      for(int step=0; step<data->number_of_steps; step++)
      {    
        
        {
          HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA_APT txCtrlData = ((HAL_RF_x_TX_DAC_REQ_TX_CONTROL_DATA_APT *)data->data_array)[step];


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
              hdetPayload->numHdetAvg = numberOfHdetAvgerages_tdscdma;
            }
            if( lpHdetBitMask & txCtrlData.paState )
            {
              hdetPayload->numLpmHdetAvg = numberOfHdetAvgerages_tdscdma;
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

  }
  else if(LTETDD_TX_CAL == data->config_pattern)
  {
    RFCA_lte_tx_dac_flexible_send(data);
  }
  else if(FLEX_CAL_V2 == data->config_pattern)
  {
    if(NULL == AddFtmCfgAndExecCalSweepFlex(rfcaResp->HalID, false, 0, 0, V3_SWEEP_ENABLE_F3, (HAL_RF_SWEEP_SEGMENT_CONFIG_DATA*)(data->data_array)))
    {
      halResp->status = HAL_RF_STATUS_FAILED;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }
    SetAtpSweepConfigured(true);
  }
  else if(FLEX_CAL_V2_COMPRESSED == data->config_pattern)
  {
    if(NULL == AddFtmCfgAndExecCalSweepFlex(rfcaResp->HalID, true, 0, 0, V3_SWEEP_ENABLE_F3, (HAL_RF_SWEEP_SEGMENT_CONFIG_DATA*)(data->data_array)))
    {
      halResp->status = HAL_RF_STATUS_FAILED;
      SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);
      return;
    }
    SetAtpSweepConfigured(true);
  }
  else
  {
    MSG_ERROR("Invalid ARB config %d", data->config_pattern, 0, 0);
    ((HAL_RF_ARB_TX_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_ARGUMENT_ERROR;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  }

  // Send above FTM messages to UUT
  if(RFCA_FUNCTION_SUCCESS != RFCA_SendFtmPackagesToSequencer(rfcaResp))
  {
    ((HAL_RF_ARB_TX_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
    SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  }
  else
  {
    // Send ACK delayed response will be sent later
    ((HAL_RF_ARB_TX_RESP*)rfcaResp->HAL.HalResponse)->status = HAL_RF_STATUS_OK;
    SendAckFromRfcaResponseQueue(rfcaResp->HalID, FALSE);
  }

}

/*----------------------------------------------------------------------------*/
int AddSweepToFtmCmdQueue(uint16 halOpId, FTM_Cal_Sweep_Opcode_Req_List_Item **head, FTM_Cal_Sweep_Opcode_Req_List_Item **tail, bool cleanList)
{
  FTM_Cal_Sweep_Opcode_Req_List_Item *splitHead = *head;
  FTM_Cal_Sweep_Opcode_Req_List_Item *splitTail = *head;
  FTM_Cal_Sweep_Opcode_Req_List_Item *nextSplitHead = (*head)->next;

  const uint16 doubleHeaderSize = 12 + 19; // FTM_SEQ_QUEUE_COMMANDS + FTM_CFG_AND_EXEC_CAL_SWEEP headers
  while (NULL != splitTail)
  {
    uint16 splitTotalSize = 0;
    uint16 splitNumberOfSteps = 1;
    while((NULL != nextSplitHead) && (splitTotalSize + (splitTail->next)->totalSize + doubleHeaderSize) < RFCA_PACKET_HELPER_MAX_PACKET_SIZE)   
    {
      splitTotalSize += splitTail->totalSize;
      splitNumberOfSteps++;
      nextSplitHead = nextSplitHead->next;
      splitTail = splitTail->next;
    }

    // Temporarily terminate linked list
    splitTail->next = NULL;
    AddFtmCfgAndExecCalSweep(halOpId, 0, 0, V3_SWEEP_ENABLE_F3, splitNumberOfSteps, splitHead);
    splitHead = nextSplitHead;
    splitTail->next = nextSplitHead;
    splitTail = nextSplitHead;
    nextSplitHead = (NULL != nextSplitHead)?(nextSplitHead->next):(NULL);
  }

  if(true == cleanList)
  {
    while(NULL != (*head))
    {
      *tail = (*head)->next;
      RFCA_Free(*head);
      *head = *tail;
    }
  }
  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
int AddNoOpSegment(FTM_Cal_Sweep_Opcode_Req_List_Item **tailOfList, int32 totalTimeToDoNothing)
{  

  while(0 < totalTimeToDoNothing)
  {
    const uint16 sweeptemSize = sizeof(FTM_Cal_Sweep_Opcode_Req_List_Item);
    FTM_Cal_Sweep_Opcode_Req_List_Item *sweepItem = (FTM_Cal_Sweep_Opcode_Req_List_Item *)RFCA_Malloc(sweeptemSize);
    if(NULL == sweepItem)
    {
      MSG_ERROR("Could not allocate memory for sweep no-op item.", 0, 0, 0);      
      return RFCA_FUNCTION_FAIL;
    }
    if(NULL != *tailOfList)
    {
      (*tailOfList)->next = sweepItem;
    }
    *tailOfList = sweepItem;
    sweepItem->next = NULL;
    sweepItem->totalSize = sweeptemSize;

    ResetOpcode(&(sweepItem->opCodeHeader));
    sweepItem->opCodeHeader.segLen = (word)((totalTimeToDoNothing>rfca_common_msm_get_hw_timer_periodicty())?rfca_common_msm_get_hw_timer_periodicty():totalTimeToDoNothing);
    totalTimeToDoNothing -= sweepItem->opCodeHeader.segLen;
  }
  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
uint16 AddSweepRxWait(FTM_Cal_Sweep_Opcode_Req_List_Item **head, FTM_Cal_Sweep_Opcode_Req_List_Item **tail, uint32 timeout, uint16 pollingInterval, int16 threshold)
{
  const uint16 sweepItemSize = sizeof(FTM_Cal_Sweep_Opcode_Req_List_Item) + sizeof(FTM_Cal_Sweep_Rx_Wait_Req);
  FTM_Cal_Sweep_Opcode_Req_List_Item *sweepItem = NULL;
  sweepItem = (FTM_Cal_Sweep_Opcode_Req_List_Item *)RFCA_Malloc(sweepItemSize);

  if(NULL == sweepItem)
  {
    HalLogWrite("Could not allocate memory for sweep rx wait item");
    return RFCA_FUNCTION_FAIL;
  }

  sweepItem->next = NULL;
  sweepItem->totalSize = sweepItemSize;

  ResetOpcode(&(sweepItem->opCodeHeader));
  SetFtmCalSweepOpCodeBit(&(sweepItem->opCodeHeader), FTM_CAL_OPCODE_POLLING);

  sweepItem->opCodeHeader.segLen = pollingInterval;

  FTM_Cal_Sweep_Rx_Wait_Req* waitPayload = (FTM_Cal_Sweep_Rx_Wait_Req*)(&sweepItem->opCodeHeader.opCodePayload);
  waitPayload->version = 0;
  waitPayload->debug_mode_en = V3_SWEEP_ENABLE_F3;
  waitPayload->flag = 0;
  waitPayload->timeout_us = timeout;
  waitPayload->polling_event = (1 << POLLING_EVENT_RXAGC_THRES_REACHED);
  waitPayload->primary_device = GetActiveRfDevice();
  waitPayload->direction = SIGNAL_RISING;
  waitPayload->threshold = threshold;

  if(NULL == *head)
  {
    *head = sweepItem;
  }
  else
  {
    (*tail)->next = sweepItem;
  }
  *tail = sweepItem;

  SetAtpSweepConfigured(true);

  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
uint16 AddSweepRxConfig(FTM_Cal_Sweep_Opcode_Req_List_Item **head, FTM_Cal_Sweep_Opcode_Req_List_Item **tail, uint16 duration, HAL_RF_RX_SWEEP_STEP_CONFIG rxCtrlData)
{
  uint16 segmentLength = duration; 
  uint16 rxItemSize = sizeof(FTM_Cal_Sweep_Opcode_Req_List_Item) + sizeof(FTM_Cal_Sweep_RxCfg_Req);
  FTM_Cal_Sweep_Opcode_Req_List_Item *rxItem = (FTM_Cal_Sweep_Opcode_Req_List_Item* )RFCA_Malloc(rxItemSize);

  if(NULL == rxItem)
  {
    HalLogWrite("Could not allocate memory for sweep rx item");
    return RFCA_FUNCTION_FAIL;
  }

  rxItem->next = NULL;
  rxItem->totalSize = rxItemSize;

  ResetOpcode(&(rxItem->opCodeHeader));
  SetFtmCalSweepOpCodeBit(&(rxItem->opCodeHeader), FTM_CAL_OPCODE_CONFIG_RX);

  rxItem->opCodeHeader.segLen = segmentLength;

  FTM_Cal_Sweep_RxCfg_Req *rxConfPayload = (FTM_Cal_Sweep_RxCfg_Req*)(&(rxItem->opCodeHeader.opCodePayload[0]));
  rxConfPayload->rxActions = rxCtrlData.actions;
  rxConfPayload->rxFlag = rxCtrlData.rxFlag;
  rxConfPayload->rxPM = rxCtrlData.powerMode;

  if(NULL == *head)
  {
    *head = rxItem;
  }
  else
  { 
    (*tail)->next = rxItem;
  }
  *tail = rxItem;

  SetAtpSweepConfigured(true);

  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
uint16 AddSweepRxMeasure(FTM_Cal_Sweep_Opcode_Req_List_Item **head, FTM_Cal_Sweep_Opcode_Req_List_Item **tail, uint16 duration, HAL_RF_RX_SWEEP_STEP_CONFIG rxCtrlData)
{
  uint16 segmentLength = duration; 
  uint16 rxItemSize = sizeof(FTM_Cal_Sweep_Opcode_Req_List_Item) + sizeof(FTM_Cal_Sweep_RxCfg_Req) + sizeof(FTM_Cal_Sweep_RxMeas_Req);;
  FTM_Cal_Sweep_Opcode_Req_List_Item *rxItem = (FTM_Cal_Sweep_Opcode_Req_List_Item* )RFCA_Malloc(rxItemSize);

  if(NULL == rxItem)
  {
    HalLogWrite("Could not allocate memory for sweep rx item");
    return RFCA_FUNCTION_FAIL;
  }

  rxItem->next = NULL;
  rxItem->totalSize = rxItemSize;

  ResetOpcode(&(rxItem->opCodeHeader));
  SetFtmCalSweepOpCodeBit(&(rxItem->opCodeHeader), FTM_CAL_OPCODE_CONFIG_RX);
  SetFtmCalSweepOpCodeBit(&(rxItem->opCodeHeader), FTM_CAL_OPCODE_MEAS_RX);

  rxItem->opCodeHeader.segLen = segmentLength;

  FTM_Cal_Sweep_RxCfg_Req *rxConfPayload = (FTM_Cal_Sweep_RxCfg_Req*)(&(rxItem->opCodeHeader.opCodePayload[0]));
  rxConfPayload->rxActions = rxCtrlData.actions;
  rxConfPayload->rxFlag = rxCtrlData.rxFlag;
  rxConfPayload->rxPM = rxCtrlData.powerMode;

  FTM_Cal_Sweep_RxMeas_Req* rxMeasPayload = (FTM_Cal_Sweep_RxMeas_Req*)(&(rxItem->opCodeHeader.opCodePayload[sizeof(FTM_Cal_Sweep_RxCfg_Req)]));
  rxMeasPayload->expRxAGC = rxCtrlData.expectedRxAgc;
  rxMeasPayload->rxActions = rxCtrlData.actions;
  rxMeasPayload->rxFlag = rxCtrlData.rxFlag;
  rxMeasPayload->rxPM = rxCtrlData.powerMode;

  if(NULL == *head)
  {
    *head = rxItem;
  }
  else
  { 
    (*tail)->next = rxItem;
  }
  *tail = rxItem;

  SetAtpSweepConfigured(true);

  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
uint16 AddSweepConfig(FTM_Cal_Sweep_Opcode_Req_List_Item **head, FTM_Cal_Sweep_Opcode_Req_List_Item **tail, uint16 band, uint16 channel, uint16 channelOffset, uint16 txBW, uint16 rxBW, uint16 rxPath, uint16 duration)
{
  uint16 rfMode = -1;
  if(false == RFCA_ModeToFtmRfMode(&rfMode, StateCache.RF_TECHNOLOGY, band))
  {    
    return RFCA_FUNCTION_FAIL;
  }
 // Add config OP
  const uint16 sweepItemSize = sizeof(FTM_Cal_Sweep_Opcode_Req_List_Item) + sizeof(FTM_Cal_Sweep_Cfg_Req);
  FTM_Cal_Sweep_Opcode_Req_List_Item *sweepItem = NULL;
  sweepItem = (FTM_Cal_Sweep_Opcode_Req_List_Item *)RFCA_Malloc(sweepItemSize);
  if(NULL == sweepItem)
  {
    MSG_ERROR("Could not allocate memory for sweep config item", 0, 0, 0);
    return RFCA_FUNCTION_FAIL;
  }
  sweepItem->next = NULL;
  sweepItem->totalSize = sweepItemSize;

  ResetOpcode(&(sweepItem->opCodeHeader));
  SetFtmCalSweepOpCodeBit(&(sweepItem->opCodeHeader), FTM_CAL_OPCODE_CONFIG_RADIO);

  sweepItem->opCodeHeader.segLen = (word)((enhSweepSegmentLength.rfConfig>rfca_common_msm_get_hw_timer_periodicty())?rfca_common_msm_get_hw_timer_periodicty():enhSweepSegmentLength.rfConfig);

  FTM_Cal_Sweep_Cfg_Req* configPayload = (FTM_Cal_Sweep_Cfg_Req*)(&sweepItem->opCodeHeader.opCodePayload);
  configPayload->masterDev = GetMasterDeviceMaskFromRxPath(rxPath);
  configPayload->slaveDev = GetSlaveIdFromRxPath(rxPath);
  configPayload->rfmode = (byte)rfMode;
  configPayload->ulChan = channel;
  configPayload->txBW = txBW;
  configPayload->rxBW = rxBW;
  configPayload->chanOffset = channelOffset;
  uint32 timeLeftToDoNothinig = 0;
  timeLeftToDoNothinig = enhSweepSegmentLength.rfConfig - sweepItem->opCodeHeader.segLen;

  StateCache.BAND = band;
  StateCache.CHANNEL = channel;

  if(NULL == *head)
  {
    *head = sweepItem;
    *tail = sweepItem;
  }
  else
  {
    (*tail)->next = sweepItem;
    *tail = sweepItem;
  }

  // Add no OP
  if(RFCA_FUNCTION_FAIL == AddNoOpSegment(tail, timeLeftToDoNothinig))
  {
    return RFCA_FUNCTION_FAIL;
  }
  SetAtpSweepConfigured(true);
  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
uint16 AddSweepTune(FTM_Cal_Sweep_Opcode_Req_List_Item **head, FTM_Cal_Sweep_Opcode_Req_List_Item **tail, uint16 channel, uint16 channelOffset, uint16 duration)
{
  // Add retune OP
  const uint16 sweepItemSize = sizeof(FTM_Cal_Sweep_Opcode_Req_List_Item) + sizeof(FTM_Cal_Sweep_Retune_Req) ;
  FTM_Cal_Sweep_Opcode_Req_List_Item *sweepItem = NULL;
  sweepItem = (FTM_Cal_Sweep_Opcode_Req_List_Item *)RFCA_Malloc(sweepItemSize);
  if(NULL == sweepItem)
  {
    MSG_ERROR("Could not allocate memory for sweep retune item", 0, 0, 0);
    return RFCA_FUNCTION_FAIL;
  }
  sweepItem->next = NULL;
  sweepItem->totalSize = sweepItemSize;

  ResetOpcode(&(sweepItem->opCodeHeader));
  SetFtmCalSweepOpCodeBit(&(sweepItem->opCodeHeader), FTM_CAL_OPCODE_RETUNE_RADIO);

  sweepItem->opCodeHeader.segLen = (word)((enhSweepSegmentLength.rfTune>rfca_common_msm_get_hw_timer_periodicty())?rfca_common_msm_get_hw_timer_periodicty():enhSweepSegmentLength.rfTune);

  FTM_Cal_Sweep_Retune_Req* tunePayload = (FTM_Cal_Sweep_Retune_Req*)(&sweepItem->opCodeHeader.opCodePayload);
  tunePayload->ulChan = channel;
  tunePayload->chanOffset = channelOffset;
  tunePayload->masterDev = GetMasterDeviceMask();

  StateCache.CHANNEL = channel;

  uint32 timeLeftToDoNothinig = 0;
  timeLeftToDoNothinig = enhSweepSegmentLength.rfTune - sweepItem->opCodeHeader.segLen;

  if(NULL == *head)
  {
    *head = sweepItem;
    *tail = sweepItem;
  }
  else
  {
    (*tail)->next = sweepItem;
    *tail = sweepItem;
  }

  // Add no OP
  if(RFCA_FUNCTION_FAIL == AddNoOpSegment(tail, timeLeftToDoNothinig))
  {
    return RFCA_FUNCTION_FAIL;
  }

  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
void RFCA_inform_send(HAL_RF_INFORM_REQ *data)
{
  PendingRfcaResponse *rfcaResp = AddToRfcaResponseQueue(data->hdr.operation_id, data, sizeof(HAL_RF_INFORM_REQ), sizeof(HAL_RF_INFORM_RESP), HAL_RF_INFORM_RESP_ID, NULL);

  if(NULL == rfcaResp)
  {
    MSG_ERROR("Failed to allocate RFCA response. Terminating processing request.", 0, 0, 0);
    return;
  }

  HAL_RF_INFORM_RESP* halResp = (HAL_RF_INFORM_RESP*)rfcaResp->HAL.HalResponse;

  // At the moment it is assume all commands return success 
  // if fail is needed just overwrite like in default case below

  halResp->status = HAL_RF_STATUS_OK;

  switch(data->information)
  {
  case HAL_RF_DISABLE_EFS_SYNC_AFTER_TUNER_CAL:
    {
      efsSyncAfterTunerSelfCal = false;
      HalLogWrite("Disabling EFS sync after tuner self cal");
    }
    break;
  case HAL_RF_ENABLE_EFS_SYNC_AFTER_TUNER_CAL:
    {
      efsSyncAfterTunerSelfCal = true;
      HalLogWrite("Enabling EFS sync after tuner self cal");
    }
    break;	
  case HAL_RF_ENABLE_PERSISTENT_TX_MODE:
    {
      SetPersistentTxModeFlag(true);
      HalLogWrite("Enabling persistent TX mode");
    }
    break;
  case HAL_RF_DISABLE_PERSISTENT_TX_MODE:
    {
      SetPersistentTxModeFlag(false);
      HalLogWrite("Disabling persistent TX mode");
    }
    break;
  case HAL_RF_ENABLE_RX_TX_INTERLEAVING:
    {
      SetRxTxInteleavingFlag(TRUE);
      HalLogWrite("Enabling RX-TX interleaving");
    }
    break;
  case HAL_RF_DISABLE_RX_TX_INTERLEAVING:
    {
      SetRxTxInteleavingFlag(FALSE);
      HalLogWrite("Disabling RX-TX interleaving");
    }
    break;
  case HAL_RF_ENABLE_DEBUG_MESSAGES:
    {
      rfcaDebugMode = true;
      HalLogWrite("Enabling RFCA debug mode");
    }
    break;
  case HAL_RF_DISABLE_DEBUG_MESSAGES:
    {
      rfcaDebugMode = false;
      HalLogWrite("Disabling RFCA debug mode");
    }
    break;
  default:
    MSG_ERROR("Invalid hal inform value (%d).", data->information, 0, 0);
    halResp->status = HAL_RF_STATUS_ARGUMENT_ERROR;
    break;
  }

  SendPopAckFromRfcaResponseQueue(rfcaResp->HalID, TRUE);  
  return;
}
