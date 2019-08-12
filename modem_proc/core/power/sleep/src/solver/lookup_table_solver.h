#ifndef LOOKUP_TABLE_SOLVER_H
#define LOOKUP_TABLE_SOLVER_H
/*==============================================================================
  FILE:         lookup_table_solver.h
  
  OVERVIEW:     Determines the low power mode to enter, based on a lookup
                table provided by SleepSynth.

  DEPENDENCIES: None

                Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/solver/lookup_table_solver.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "lookup_table_types.h"
#include "sleep_solver.h"
#include "SleepLPR_synth_modes.h"
#include "sleep_statsi.h"
#include "LprTypes.h"

/* ========================================================================
                              EXTERN DECLARATIONS
=========================================================================== */

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
extern sleep_lookup_table 
         *SleepLPR_power_lookup_table[SLEEP_NUM_CORES][SLEEP_NUM_STATIC_FREQ];

/**
 *
 * @brief SleepLPR_static_freq_list
 *
 * Initial frequency list that will be added to the master frequency lookup table
 */ 
extern uint32 SleepLPR_static_freq_list[SLEEP_NUM_STATIC_FREQ];

/* ========================================================================
                           GLOBAL FUNCTION DECLARATIONS
=========================================================================== */

/**
 * sleep_lookup_table_get_entry
 *
 * @brief Finds the lookup table entry index that 
 *        corresponds to the current core frequency.  It will
 *        select the closest entry that is less than or equal
 *        to the core frequency.
 *  
 * @param freq: Current frequency (in Khz) of the core
 * @param freq_LUT: Frequency table pointer for the current core
 *                  frequency or closest match
 *                  Return value:
 *                    TRUE  -> Existing fLUT match
 *                    FALSE -> Closest fLUT (or NULL if no entries) 
 *                 
 * @return TRUE   if found existing entry
 *         FALSE  if no match was found and new cache/data entries need to be
 *                added.
 */ 
boolean sleep_lookup_table_get_entry(uint32 freq, 
                                     sleep_freq_lookup_table **freq_LUT);

/**
 * sleep_get_freq_lut
 *
 * @brief Returns the head pointer of the frequency LUT for the given core
 * 
 * @param core: Core number
 * 
 * @return Frequency lookup table pointer
 */
sleep_freq_lookup_table *sleep_get_freq_lut(uint32 core);

/**
 * sleep_lookup_table_add_fLUT
 *
 * @brief Adds a latency cache data element when a new frequency is encountered
 * 
 * @param freq_LUT: The closest frequency element
 *                 (as returned from sleep_lookup_table_get_entry)                 
 * @param cur_freq: Current core frequency in Khz
 * @param registry: Current LPR registry
 *  
 * @return New frequency element pointer
 */
sleep_freq_lookup_table* sleep_lookup_table_add_fLUT(
    sleep_freq_lookup_table *closest_freq_LUT,
    uint32                  cur_freq,
    CLprRegistry            *registry);

/**
 * sleep_lookup_table_lock_fLUT
 *
 * @brief Mutex locking function for fLUT data access
 */
void sleep_lookup_table_lock_fLUT(void);

/**
 * sleep_lookup_table_unlock_fLUT
 *
 * @brief Mutex unlocking function for fLUT data access
 */
void sleep_lookup_table_unlock_fLUT(void);

#endif /* LOOKUP_TABLE_SOLVER_H */

