#ifndef SLEEP_QR_H
#define SLEEP_QR_H
/*==============================================================================
  FILE:         sleep_qr.h
  
  OVERVIEW:     Internal functions to support the quick response feature

  DEPENDENCIES: None

                Copyright (c) 2014-2015 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/statistics/sleep_qr.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "DALStdDef.h"
#include "synthTypes.h"
#include "sleep_lpr.h"
#include "sleep_qr_types.h"
#include "sleep_stats_dynamic.h"
#include "sleep_statsi.h"

/*==============================================================================
                              EXTERN DECLARATIONS
 =============================================================================*/
/**
 * sleepQR_reset
 *
 * @brief Resets certain QR internal data that should be done along with
 *        general sleep statistic resets for a synthmode
 *        
 * @param QRData: QR data pointer for the synthmode
 */
void sleepQR_reset(sleepQR_synthMode *QRData);

/**
 * sleepQR_perform
 *
 * @brief Analyzes the latest sleep cycle data and determines if a QR is needed 
 *        to respond to a change in latencies. 
 *  
 * @param synthMode:    Synthmode that will be used for QR analysis 
 *                      This is the mode that sleep chose to enter on the last
 *                      cycle
 * @param fLUT:         Current frequency LUT element
 * @param wakeupDelta:  Error in expected sleep exit to actual exit
 */
void sleepQR_perform(sleep_synth_lprm         *synthMode,
                     sleep_fLUT_node          *fLUT,
                     int32                    wakeupDelta);

/**
 * sleepQR_resetComponents
 *
 * @brief Used to reset the component mode QR data for the given synthmode. 
 *        This should be called after the automatic backoff feature was run on
 *        the synthmode 
 *  
 * @param synthMode:  Components of this Synthmode will be reset
 * @param fLUT:       Current frequency table to use on the synthMode data
 */ 
void sleepQR_resetComponents(sleep_synth_lprm  *synthMode, 
                             sleep_fLUT_node   *fLUT);

/**
 * sleepQR_setComponentCycleBackoff
 *
 * @brief Sets the latest backoff value for the given LPRM
 *  
 * @param sleepLPRM: LPRM to update
 * @param backoff:   Value of backoff
 * @param dynData:   Dynamic data for the given sleepLPRM 
 */ 
void sleepQR_setComponentCycleBackoff(sleep_lprm              *sleepLPRM,
                                      uint32                  backoff,
                                      sleepStats_dynamic_data *dynData);

/**
 * sleepQR_initElement
 *
 * @brief Inits QR data elements when a new frequency is encountered.
 *  
 * @param dynData: Frequency based dynamic data pointer
 */ 
void sleepQR_initElement(sleepStats_dynamic_data *dynData);

/**
 * sleepQR_clearComponentMonitoring
 *
 * @brief Stops monitoring component modes after a QR event has occurred. 
 *  
 * @param synthMode: Pointer to the synthmode of which to reset the component 
 *                   data.
 * @param dynData:   Pointer to the dynamic data structure associated with the 
 *                   synthMode. 
 */
void sleepQR_clearComponentMonitoring(sleep_synth_lprm        *synthMode,
                                      sleepStats_dynamic_data *dynData);

#endif /* SLEEP_QR_H */
