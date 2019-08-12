#ifndef LOOKUP_TABLE_SOLVER_H
#define LOOKUP_TABLE_SOLVER_H
/*==============================================================================
  FILE:         lookup_table_solver.h
  
  OVERVIEW:     Determines the low power mode to enter, based on a lookup
                table provided by SleepSynth.

  DEPENDENCIES: None

                Copyright (c) 2012-2013 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/solver/lookup_table_solver.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "lookup_table_types.h"
#include "sleep_solver.h"
#include "SleepLPR_synth_modes.h"
#include "sleep_statsi.h"
#include "synthTypes.h"

/*==============================================================================
                              EXTERN DECLARATIONS
 =============================================================================*/
/**
 *
 * @brief sleep_lookup_table_solver
 *
 * Chooses the low power mode to enter, based on the lookup table provided
 * by SleepSynth.  Compares the expected sleep duration, to find the mode that
 * would save the most power.  Then ensures that the mode is enabled,
 * doesn't violate the latency restricions, and fits within the expected
 * sleep duration.
 */
extern sleep_solver_type sleep_lookup_table_solver;

/**
 *
 * @brief SleepLPR_power_lookup_table
 *
 * Master lookup table that contains data to select the correct low power modes 
 * to enter based on allowable sleep time.
 */
extern sleep_mLUT
         *SleepLPR_power_lookup_table[SLEEP_NUM_CORES][SLEEP_NUM_STATIC_FREQ];

/**
 *
 * @brief SleepLPR_static_freq_list
 *
 * Initial frequency list that will be added to the master frequency lookup table
 */ 
extern uint32 SleepLPR_static_freq_list[SLEEP_NUM_STATIC_FREQ];

#endif /* LOOKUP_TABLE_SOLVER_H */

