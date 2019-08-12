/*!
  @file
  rfca_state_cache.h
 
  @brief
  Types and functions to manage RFCA state cache.

*/


/*==============================================================================

  Copyright (c) 2011-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_state_cache.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/14/13   aki     Slot config and modulation
06/10/13   aki     Mainline RFCA
02/04/13   aki     Added SF_ACTIVITY for LTE
02/01/13   aki     Made band and channel common in state cache
09/14/12   aki	   Added burst mode flags for TDSCDMA
08/28/12   aki	   Moved FREQ_ADJ to common section
08/15/12   aki     Added support for FREQ_ADJ
07/30/12   aki     LTE NET_SIG_VAL added
07/13/12   aki     CDMA power mode, TDSCDMA signal path
04/23/12   aki     LNA rangfe to common section, second channel for wcdma,
                   bandwidth, rb and waveform added for lte
02/24/12   aki     Calibration state & TX burst
01/25/12   aki     Added PACK to all structs
10/28/11   aki     Initial revision

==============================================================================*/



#ifndef _RFCA_STATE_CACHE_H_
#define _RFCA_STATE_CACHE_H_

#include "rfa_variation.h"
#include "comdef.h"

#include "rfca_hal_msg_def.h"

#ifdef _WIN32
  #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*----------------------------------------------------------------------------*/
///< Value to be used to indicate that an item in state cache has not been set
const int InvalidStateCacheValue = -1;

/*----------------------------------------------------------------------------*/
/*!
 @brief RFCA State Cache

 @details See CacheTechnology for currently supported technologies.

 @todo Rewrite using object oriented style, base class + inheritance.
       There's no need to store settings for each system all the time because
       when technology is changed all settings are reset. So instead when technology
       changes create an instance of correct cache object and assign it as current.

 */
typedef PACK(struct) {
  boolean MODEM_TEST_MODE_ACTIVE;
  HAL_RF_PROTOCOL_SYSTEM RF_TECHNOLOGY;
  boolean CALIBRATION_STATE_ACTIVE;
  boolean TX_STATE;
  int RX_LNA_RANGE;
  boolean FREQ_ADJ_APPLIED;
  int BAND;
  int CHANNEL;
  PACK(struct) {
    int RX_STATE;
    int TX_SLOT_MODULATION;
    int TX_SLOT_ACTIVITY;
    int TX_PA_RANGE;
    int TX_PA_BIAS;
    int TX_RGI;
    boolean RX_GAIN_RANGE_CAL_INITIALIZED;
    int TX_POWER_LEVELS_READ_FOR_BAND;
    short TX_POWER_LEVELS[16];
    int TX_POWER_LEVEL_COUNT;
    boolean TX_BURST_SET;
  } GSM;
  // TODO: Change to HAL_RF_PROTOCOL_SYSTEM 
  PACK(struct) {
    int TX_MODULATION;
    int TX_PA_RANGE;
    int TX_PA_BIAS;
    int TX_AGC_PDM;
    int SIGNAL_PATH; 
    int POWER_MODE;  // FTM POWER MODE
  } CDMA;
  PACK(struct) {
    int SECOND_CHANNEL;
    int TX_MODULATION;
    int TX_PA_RANGE;
    int TX_PA_BIAS;
    int TX_AGC_PDM;
    int SIGNAL_PATH; 
  } WCDMA;
  PACK(struct) {
    int TX_MODULATION;
    int TX_PA_RANGE;
    int TX_PA_BIAS;
    int TX_RGI;
    int SIGNAL_PATH;
    int BAND_WIDTH;
    int WAVEFORM;
    int PUSCH_RB_NUMBER_OF;
    int PUSCH_RB_START_INDEX;
    int NET_SIG_VAL;
    int SF_ACTIVITY;
  } LTE;
  PACK(struct) {
    int TX_MODULATION;
    int TX_PA_RANGE;
    int TX_PA_BIAS;
    int SIGNAL_PATH;
    boolean BURST_TX_ENABLED;
    boolean BURST_RX_ENABLED;
  } TDSCDMA;
} _RfcaStateCacheT;

#ifdef USE_STATE_CACHE
  extern "C" _declspec( dllimport )  _RfcaStateCacheT StateCache;
#else
  extern "C" _RfcaStateCacheT StateCache;
#endif

/*----------------------------------------------------------------------------*/
/*!
 @brief Resets cache values

 @details Integer types in cache are reset to InvalidStateCacheValue.
          Boolean types are set to false.
          Technology is reset to MaxCacheTechnology.

 @param [in] cache Pointer to cache instance
 */
void RFCA_StateCacheReset(_RfcaStateCacheT *cache);

/*----------------------------------------------------------------------------*/
/*!
 @brief Reset cache values for single technology.

 @param [in] cache  Pointer to cache instance
 @param [in] technology  Technology to be reset
 */
void RFCA_StateCacheResetTechnology(_RfcaStateCacheT *cache, HAL_RF_PROTOCOL_SYSTEM technology);

/*----------------------------------------------------------------------------*/
/*!
 @brief Sets cache technology

 @details All other cache values are reset. See RFCA_StateCacheReset

 @param [in] cache  Pointer to cache instance
 @param [in] technology  Technology to set
 */
void RFCA_StateCacheSetTechnology(_RfcaStateCacheT *cache, HAL_RF_PROTOCOL_SYSTEM technology);

/*----------------------------------------------------------------------------*/
/*!
 @brief RFCA state cache set band.

 @note Technology has to be set before calling this function. See RFCA_StateCacheSetTechnology

 @param [in] cache  Pointer to cache instance
 @param [in] band   Band to set
 */
void RFCA_StateCacheSetBand(_RfcaStateCacheT *cache, int band);

/*----------------------------------------------------------------------------*/
/*!
 @brief RFCA state cache set transmit state.

 @param [in] cache    Pointer to cache instance
 @param [in] txState  TX on or off
 */
void RFCA_StateCacheSetTxState(_RfcaStateCacheT *cache, boolean txState);

#ifdef _WIN32
  #pragma pack(pop) // Restore
#endif

#endif  // _RFCA_STATE_CACHE_H_
