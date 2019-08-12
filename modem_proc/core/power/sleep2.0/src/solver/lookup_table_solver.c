/*==============================================================================
  FILE:         lookup_table_solver.c
 
  OVERVIEW:     This file provides the sleep solver implementation.  This is
                used by the sleep task when the CPU is idle, to gather the
                available low power modes that can be entered.

  DEPENDENCIES: None

                Copyright (c) 2012-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/solver/lookup_table_solver.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <limits.h>
#include <stdlib.h>
#include "CoreVerify.h"
#include "sleep_log.h"
#include "sleep_lpri.h"
#include "lookup_table_solver.h"
#include "synthTypes.h"
#include "SleepLPR_synth_modes.h"
#include "synthRegistry.h"

/*==============================================================================
                            INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * sleepLUTSolver_findElement
 *
 * @brief This function finds the lookup table element that 
 *        contains a duration range that satisfies the sleep
 *        duration passed in, and returns a pointer to that
 *        element.  This function can be optimized/modified
 *        independently of the main solver function.
 *
 * @param mode_duration:  Sleep duration to find the lookup 
 *                        table element for.
 *  
 * @param ftable_idx:     Frequency table index based on
 *                        current core frequency 
 *  
 * @return Pointer to the lookup table element whose duration 
 *         range contains the sleep duration passed in.
 */
static sleep_mLUT_elem *sleepLUTSolver_findElement
( 
  uint64  mode_duration,
  uint32  lutIdx
)
{
  /* Keep track of the most recently used duration.  This should help
   * with the case where the same duration range is hit over and over
   * again. */
  static uint32     mru_mode_idx  = 0;
  static sleep_mLUT *mru_lut      = 0;
  sleep_mLUT_elem   *mru_mode, *last_mode, *comp_mru_mode;
  sleep_mLUT        *pModeLUT;
  int32             end_idx       = 0;
  uint32            core_num      = CORE_CURR_CORE;
  
  CORE_VERIFY(core_num < SLEEP_NUM_CORES);
  pModeLUT  = SleepLPR_power_lookup_table[core_num][lutIdx];
  mru_mode  = &pModeLUT->element[mru_mode_idx];
  last_mode = &pModeLUT->element[pModeLUT->num_elements - 1];

  /*Set comparison mode to one idx lower (higher duration)*/
  if( mru_mode_idx != 0)
    comp_mru_mode = &pModeLUT->element[mru_mode_idx - 1];
  else
    comp_mru_mode = mru_mode;

  /* Check the last recently used entry for a match. */
  if(pModeLUT == mru_lut)
  {
    /* If given duration > most recently used duration and most recent duration
     * is max or given duration is <= next higher duration 
     */ 
    if(mode_duration > mru_mode->duration_thresh &&
       (mru_mode_idx == 0 || mode_duration <= comp_mru_mode->duration_thresh))
    {
      /* Found a match */
      sleepStats_updateValue(&mru_mode->element_stats, mode_duration);
      return mru_mode;
    }
    /* Check for not enough time for any mode */
    else if(mode_duration <= last_mode->duration_thresh)
    {
      return NULL;
    }
  }
  else
  {
    /* Running at different frequency from last cached value (different LUT ptr)
     *  - Need to update cache values */
    mru_lut   = pModeLUT;
    last_mode = &pModeLUT->element[pModeLUT->num_elements - 1];
  }
  
  mru_mode_idx  = pModeLUT->num_elements / 2;
  mru_mode      = &pModeLUT->element[mru_mode_idx];

  /* Check for not enough time for any mode in new frequency table */
  if(mode_duration <= last_mode->duration_thresh)
    return NULL;

  /* Use the MRU info to determine which half of lookup table to traverse.
   * The search will start at index 0 or current index + 1 and increment until 
   * it finds the correct entry or errors.
   */
  if( mode_duration > mru_mode->duration_thresh )
  {
    end_idx       = mru_mode_idx + 1;
    mru_mode_idx  = 0;
  }
  else
  {
    mru_mode_idx++;
    end_idx = pModeLUT->num_elements;
  }
  
  /* Traverse the rest of the table to find the duration that matches. */
  for(; mru_mode_idx < end_idx; mru_mode_idx++ )
  {
    if( mode_duration > pModeLUT->element[mru_mode_idx].duration_thresh)
    {
      /* Found it, return this element. */
      sleepStats_updateValue(&pModeLUT->element[mru_mode_idx].element_stats, 
                             mode_duration);
      return ( &pModeLUT->element[mru_mode_idx] );
    }
  }

  /* We should never make it this far. If we do, some logic failed above that 
   * should have caught the case were there is not enough time to do any modes.
   */
   sleepLog_printf( SLEEP_LOG_LEVEL_ERROR, 2*1, 
                    "Invalid duration (Value: 0x%llx)", 
                    ULOG64_DATA(mode_duration));

  CORE_VERIFY( 0 );
  return NULL;
}

/**
 * sleepLUTSolver_initialize
 *
 * @brief Init function for the lookup table solver.  This 
 *        function populates the mode_to_enter field in the
 *        lookup table, per the synthLPRM's registered.
 *
 * @return Returns non-zero on error.
 */
static uint32 sleepLUTSolver_initialize( void )
{
  sleep_synth_lpr *lpr;
  uint32          coreIdx, freqIdx, durIdx, modeIdx;
  uint32          mode_index;

  /* Fill in the synthesized mode for each core. */
  for( coreIdx = 0; coreIdx < g_SleepNumOfCores; coreIdx++ )
  {
    lpr = synthRegistry_getSynthLPRList(coreIdx);

    /* There will always be at least 1 table */
    for(freqIdx = 0; freqIdx < g_SleepNumOfFreqs; freqIdx++)
    {
      sleep_mLUT *pTable = 
        SleepLPR_power_lookup_table[coreIdx][freqIdx]; 

      for( durIdx = 0; durIdx < pTable->num_elements; durIdx++)
      {
        pTable->element[durIdx].element_stats.min = UINT64_MAX;

        for( modeIdx = 0; modeIdx < SLEEP_LPR_NUM_SYNTHESIZED_MODES; modeIdx++)
        {
          if( -1 == pTable->element[durIdx].mode[modeIdx].mode_idx ) 
          {
            pTable->element[durIdx].mode[modeIdx].mode_ptr = NULL; 
          }
          else
          {
            mode_index = pTable->element[durIdx].mode[modeIdx].mode_idx;
            pTable->element[durIdx].mode[modeIdx].mode_ptr = 
              &lpr->modes[mode_index];
          }
        }
      }
    }
  }

  return 0;
}

/**
 * sleepLUTSolver_mainFunction
 *
 * @brief This function finds the right low power mode to enter, 
 *        based on a pre-determined lookup table.  Given
 *        duration and latency restrictions, the low power mode
 *        is chosen from the table.
 *
 * @param input:  List of constraints that are used by the solver 
 *                to select modes.
 * @param output: List of output data from the solver -- 
 *                contains the list of modes to enter, as well as
 *                the backoff time that should be used.
 */
static void sleepLUTSolver_mainFunction
(
  sleep_solver_input  *input,
  sleep_solver_output *output
)
{
  int32             i;
  uint64            mode_duration;
  uint32            mode_backoff        = 0;
  uint32            mode_enter_exit_lat = 0;
  sleep_synth_lprm  *mode_chosen        = NULL;
  sleep_mLUT_elem   *lookup_ele         = NULL;
  sleep_fLUT_node   *fLUT               = input->fLUT;

  /* Set default to no mode selected */
  output->selected_synthLPRM = NULL;

  if( SLEEP_ENABLED == 
        synthLPR_getStatus( synthRegistry_getSynthLPRList(-1) ) )
  {
    mode_duration = ( input->soft_duration < input->hard_duration ?
                      input->soft_duration : input->hard_duration );

    /* Find the lookup table entry for this duration. */
    lookup_ele = 
      sleepLUTSolver_findElement( mode_duration, fLUT->mLUT_idx);

    if(lookup_ele != NULL)
    {
      sleepLog_printf( SLEEP_LOG_LEVEL_INFO, 3,
                       "Solver table (mLUT: %d) (Duration: %lld)",
                       fLUT->mLUT_idx,
                       ULOG64_DATA(lookup_ele->duration_thresh));

      /* Traverse the modes for the duration range found, and find the one
       * that fits the current criteria. */
      for( i = 0; i < SLEEP_LPR_NUM_SYNTHESIZED_MODES; i++ )
      {
        mode_chosen = lookup_ele->mode[i].mode_ptr;

        /* If we hit a NULL mode, then we've reached the end of the mode list,
         * and didn't find a mode to enter. */
        if( NULL == mode_chosen )
        {
          break;
        }

        /* Skip if mode is disabled or if input attr mask does not match */
        if( ( SLEEP_DISABLED == mode_chosen->mode_status ) || 
            ( ( input->synth_attr_mask & mode_chosen->attr ) == 0 ) )
        {
          continue;
        }

        mode_enter_exit_lat = synthLPRM_getLPRMLatency( mode_chosen, 
                                                        fLUT->mLUT_idx);

        /* Does the mode's enter and exit latency fit in the expected sleep
         * duration, and not violate the latency budget? */
        if( mode_enter_exit_lat > mode_duration ||
            mode_enter_exit_lat > input->latency_budget )
        {
          sleepLog_printf( SLEEP_LOG_LEVEL_INFO, 3 + (2*1),
                           "Mode skipped (name: \"%s\") "
                           "(reason: \"Insufficient Time\") "
                           "(Synth latency: 0x%08x) "
                           "(latency budget: 0x%08x) (duration: 0x%llx)",
                           mode_chosen->name, 
                           mode_enter_exit_lat, 
                           input->latency_budget, 
                           ULOG64_DATA(mode_duration) );
          continue;
        }

        mode_backoff = synthLPRM_getBackOffTime(  mode_chosen, fLUT);

        /* If we made it this far, then we found the right mode.
         * Record its info and return. */
        output->selected_synthLPRM = mode_chosen;
        sleepLog_printf( SLEEP_LOG_LEVEL_INFO, 1, 
                         "Mode chosen: (\"%s\")",
                         mode_chosen->name);

        /* Prepare the data to return. */
        output->backoff_time    = mode_backoff;
        output->enter_exit_time = mode_enter_exit_lat;
        break;
      }
    }
  }

  if( NULL == output->selected_synthLPRM )
  {
    sleepLog_printf( SLEEP_LOG_LEVEL_INFO, 0, "No mode chosen" );
  }

  return;
}

sleep_solver_type sleep_lookup_table_solver =
{
  sleepLUTSolver_initialize,
  NULL, /* no deinit function */
  sleepLUTSolver_mainFunction,
  "Lookup Table"
};

