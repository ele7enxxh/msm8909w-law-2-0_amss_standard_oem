/*!
@file
  rfca_state_cache.cpp

@brief
  Implementes RFCA state cache.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca_state_cache.cpp#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/03/14   aki     Removed RFCA_ASSERT 
11/25/13   aki     Start using CALIBRATION_STATE_ACTIVE, only RFIC can modify it 
11/14/13   aki     Modulation and ts activity
06/10/13   aki     Mainline RFCA 
02/01/13   aki     Added SF_ACTIVITY for LTE 
02/01/13   aki     Updated state cache band and channel
10/16/12   aki     Bool to boolean of BURST_TX/RX_ENABLED
08/28/12   aki	   Moved FREQ_ADJ to common section
08/15/12   aki     Added support for FREQ_ADJ
07/30/12   aki     LTE NET_SIG_VAL, LTE TX state setting does not any more invalidate
                   SIGNAL_PATH, BAND_WIDTH, PUSCH_RB_NUMBER_OF, PUSCH_RB_START_INDEX
07/13/12   aki     CDMA power mode, TDSCDMA signal path
04/24/12   aki     LNA range to common, dual carrier, lte additions
02/24/12   aki     Calibration state flag
01/25/12   aki     Signal path to CDMA
10/28/11   aki     Initial revision
==============================================================================*/


#include "rfa_variation.h"
#include "comdef.h"
#include "msg.h"

#include <rfca_state_cache.h>
#include <rfca_defs.h>

/*----------------------------------------------------------------------------*/
void RFCA_StateCacheReset(_RfcaStateCacheT *cache)
{

  // Common
  cache->MODEM_TEST_MODE_ACTIVE = FALSE;
  cache->RF_TECHNOLOGY = HAL_RF_PROTOCOL_SYSTEM_MAX;
  cache->FREQ_ADJ_APPLIED = FALSE;

  for(int tech = HAL_RF_PROTOCOL_SYSTEM_GSM; tech < HAL_RF_PROTOCOL_SYSTEM_MAX; tech++)
  {
    RFCA_StateCacheResetTechnology(cache, (HAL_RF_PROTOCOL_SYSTEM)tech);
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_StateCacheResetTechnology(_RfcaStateCacheT *cache, HAL_RF_PROTOCOL_SYSTEM technology)
{

  cache->TX_STATE = FALSE;
  cache->RX_LNA_RANGE = InvalidStateCacheValue;
  cache->FREQ_ADJ_APPLIED = FALSE;
  cache->BAND = InvalidStateCacheValue;
  cache->CHANNEL = InvalidStateCacheValue;
  switch(technology)
  {
  case HAL_RF_PROTOCOL_SYSTEM_GSM:
    cache->GSM.RX_STATE = InvalidStateCacheValue;
    cache->GSM.TX_SLOT_MODULATION = InvalidStateCacheValue;
    cache->GSM.TX_SLOT_ACTIVITY = InvalidStateCacheValue;
    cache->GSM.TX_PA_RANGE = InvalidStateCacheValue;
    cache->GSM.TX_PA_BIAS = InvalidStateCacheValue;
    cache->GSM.TX_RGI = InvalidStateCacheValue;
    cache->GSM.RX_GAIN_RANGE_CAL_INITIALIZED = FALSE;
    cache->GSM.TX_POWER_LEVELS_READ_FOR_BAND = InvalidStateCacheValue;
    cache->GSM.TX_POWER_LEVEL_COUNT = InvalidStateCacheValue;
    for(int i=0;i<16;i++)
    {
      cache->GSM.TX_POWER_LEVELS[i] = -8000;  // Some dummy value for now
    }
    cache->GSM.TX_BURST_SET = FALSE;
    break;
  case HAL_RF_PROTOCOL_SYSTEM_CDMA:
    cache->CDMA.TX_MODULATION = InvalidStateCacheValue;
    cache->CDMA.TX_PA_RANGE = InvalidStateCacheValue;
    cache->CDMA.TX_PA_BIAS = InvalidStateCacheValue;
    cache->CDMA.TX_AGC_PDM = InvalidStateCacheValue;
    cache->CDMA.SIGNAL_PATH = InvalidStateCacheValue;
    cache->CDMA.POWER_MODE = InvalidStateCacheValue;
    break;
  case HAL_RF_PROTOCOL_SYSTEM_WCDMA:
    cache->WCDMA.SECOND_CHANNEL = InvalidStateCacheValue;
    cache->WCDMA.TX_MODULATION = InvalidStateCacheValue;
    cache->WCDMA.TX_PA_RANGE = InvalidStateCacheValue;
    cache->WCDMA.TX_PA_BIAS = InvalidStateCacheValue;
    cache->WCDMA.TX_AGC_PDM = InvalidStateCacheValue;
    cache->WCDMA.SIGNAL_PATH = InvalidStateCacheValue;
    break;
  case HAL_RF_PROTOCOL_SYSTEM_LTE:
    cache->LTE.TX_MODULATION = InvalidStateCacheValue;
    cache->LTE.TX_PA_RANGE = InvalidStateCacheValue;
    cache->LTE.TX_PA_BIAS = InvalidStateCacheValue;
    cache->LTE.TX_RGI = InvalidStateCacheValue;
    cache->LTE.SIGNAL_PATH = InvalidStateCacheValue;
    cache->LTE.BAND_WIDTH = InvalidStateCacheValue;
    cache->LTE.WAVEFORM = InvalidStateCacheValue;
    cache->LTE.PUSCH_RB_NUMBER_OF = InvalidStateCacheValue;
    cache->LTE.PUSCH_RB_START_INDEX = InvalidStateCacheValue;
    cache->LTE.NET_SIG_VAL = InvalidStateCacheValue;
    cache->LTE.SF_ACTIVITY = InvalidStateCacheValue;
    break;
  case HAL_RF_PROTOCOL_SYSTEM_TD_SCDMA:
    cache->TDSCDMA.TX_MODULATION = InvalidStateCacheValue;
    cache->TDSCDMA.TX_PA_RANGE = InvalidStateCacheValue;
    cache->TDSCDMA.TX_PA_BIAS = InvalidStateCacheValue;
    cache->TDSCDMA.SIGNAL_PATH = InvalidStateCacheValue;
    cache->TDSCDMA.BURST_RX_ENABLED = FALSE;
    cache->TDSCDMA.BURST_TX_ENABLED = FALSE;
    break;
  case HAL_RF_PROTOCOL_SYSTEM_MAX: // Intentional fall through
  default:
    MSG_ERROR("Invalid technology to reset", 0, 0, 0);
    break;
  }
}

/*----------------------------------------------------------------------------*/
void RFCA_StateCacheSetTechnology(_RfcaStateCacheT *cache, HAL_RF_PROTOCOL_SYSTEM technology)
{

  boolean oldTestMode = cache->MODEM_TEST_MODE_ACTIVE;
  RFCA_StateCacheReset(cache);
  cache->MODEM_TEST_MODE_ACTIVE = oldTestMode;
  cache->RF_TECHNOLOGY = technology;
}

/*----------------------------------------------------------------------------*/
void RFCA_StateCacheSetBand(_RfcaStateCacheT *cache, int band)
{  

  switch(cache->RF_TECHNOLOGY)
  {
  case HAL_RF_PROTOCOL_SYSTEM_GSM:
    {
      RFCA_StateCacheSetTechnology(cache, HAL_RF_PROTOCOL_SYSTEM_GSM);
      cache->GSM.TX_BURST_SET = FALSE;
    }
    break;
  case HAL_RF_PROTOCOL_SYSTEM_CDMA:
    {
      // Will reset most, we just need to store MODULATION
      int oldTxModulation=cache->CDMA.TX_MODULATION;
      RFCA_StateCacheSetTechnology(cache, HAL_RF_PROTOCOL_SYSTEM_CDMA);
      cache->CDMA.TX_MODULATION = oldTxModulation;
    }    
    break;
  case HAL_RF_PROTOCOL_SYSTEM_WCDMA:
    {
      // Will reset most, we just need to store MODULATION
      int oldTxModulation=cache->WCDMA.TX_MODULATION;
      RFCA_StateCacheSetTechnology(cache, HAL_RF_PROTOCOL_SYSTEM_WCDMA);
      cache->WCDMA.TX_MODULATION = oldTxModulation;
    }   
    break;
  case HAL_RF_PROTOCOL_SYSTEM_LTE:
    {
      // Will reset most, we just need to store MODULATION
      int oldTxModulation=cache->LTE.TX_MODULATION;
      RFCA_StateCacheSetTechnology(cache, HAL_RF_PROTOCOL_SYSTEM_LTE);
      cache->LTE.TX_MODULATION = oldTxModulation;
    }  
    break;
  case HAL_RF_PROTOCOL_SYSTEM_TD_SCDMA:
    {
      // Will reset most, we just need to store MODULATION
      int oldTxModulation=cache->TDSCDMA.TX_MODULATION;
      RFCA_StateCacheSetTechnology(cache, HAL_RF_PROTOCOL_SYSTEM_TD_SCDMA);
      cache->TDSCDMA.TX_MODULATION = oldTxModulation;
      cache->TDSCDMA.BURST_RX_ENABLED = FALSE;
      cache->TDSCDMA.BURST_TX_ENABLED = FALSE;
    }  
    break;
  case HAL_RF_PROTOCOL_SYSTEM_MAX: // Intentional fall through
  default:
    MSG_ERROR("Invalid technology to set band\n", 0, 0, 0);
    break;
  }
  cache->BAND = band;
}

/*----------------------------------------------------------------------------*/
void RFCA_StateCacheSetTxState(_RfcaStateCacheT *cache, boolean txState)
{

  cache->TX_STATE = txState;
  cache->FREQ_ADJ_APPLIED = FALSE;
  switch(cache->RF_TECHNOLOGY)
  {
  case HAL_RF_PROTOCOL_SYSTEM_GSM:
    cache->GSM.TX_PA_RANGE = InvalidStateCacheValue;
    cache->GSM.TX_PA_BIAS = InvalidStateCacheValue;
    cache->GSM.TX_RGI = InvalidStateCacheValue;
    break;
  case HAL_RF_PROTOCOL_SYSTEM_CDMA:
    cache->CDMA.TX_PA_RANGE = InvalidStateCacheValue;
    cache->CDMA.TX_PA_BIAS = InvalidStateCacheValue;
    cache->CDMA.TX_AGC_PDM = InvalidStateCacheValue;
    cache->CDMA.SIGNAL_PATH = InvalidStateCacheValue;
    cache->CDMA.POWER_MODE = InvalidStateCacheValue;
    break;
  case HAL_RF_PROTOCOL_SYSTEM_WCDMA:
    cache->WCDMA.TX_PA_RANGE = InvalidStateCacheValue;
    cache->WCDMA.TX_PA_BIAS = InvalidStateCacheValue;
    cache->WCDMA.TX_AGC_PDM = InvalidStateCacheValue;
    cache->WCDMA.SIGNAL_PATH = InvalidStateCacheValue;
    break;
  case HAL_RF_PROTOCOL_SYSTEM_LTE:
    cache->LTE.TX_PA_RANGE = InvalidStateCacheValue;
    cache->LTE.TX_PA_BIAS = InvalidStateCacheValue;
    cache->LTE.TX_RGI = InvalidStateCacheValue;
    cache->LTE.WAVEFORM = InvalidStateCacheValue;
    break;
  case HAL_RF_PROTOCOL_SYSTEM_TD_SCDMA:
    cache->TDSCDMA.TX_PA_RANGE = InvalidStateCacheValue;
    cache->TDSCDMA.TX_PA_BIAS = InvalidStateCacheValue;
    cache->TDSCDMA.SIGNAL_PATH = InvalidStateCacheValue;
    break;
  case HAL_RF_PROTOCOL_SYSTEM_MAX: // Intentional fall through
  default:
    MSG_ERROR("Invalid technology to reset\n", 0, 0, 0);
    break;
  }

}
