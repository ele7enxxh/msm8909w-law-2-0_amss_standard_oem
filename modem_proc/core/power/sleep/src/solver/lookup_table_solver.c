/*==============================================================================
  FILE:         lookup_table_solver.c
 
  OVERVIEW:     This file provides the sleep solver implementation.  This is
                used by the sleep task when the CPU is idle, to gather the
                available low power modes that can be entered.

  DEPENDENCIES: None

                Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/solver/lookup_table_solver.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <limits.h>
#include <stdlib.h>
#include "CoreVerify.h"
#include "CoreMutex.h"
#include "sleep_log.h"
#include "sleep_lpri.h"
#include "npa.h"
#include "lookup_table_solver.h"
#include "LprTypes.h"
#include "CLprRegistry.h"
#include "SleepLPR_synth_modes.h"
#include "sleep_statsi.h"

/* ==================================================================
                    GLOBAL VARIABLES
   ================================================================== */
static sleep_freq_lookup_table *g_sleep_fLUT[SLEEP_NUM_CORES];
static CoreMutexType            *g_sleepfLUTMutex = NULL;

/* ==================================================================
                        INTERNAL FUNCTIONS
   ================================================================== */

/**
 * sleep_lookup_table_solver_find_lookup_ele
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
static sleep_LUT_elem *sleep_lookup_table_solver_find_lookup_ele
( 
  uint64  mode_duration,
  uint32  lutIdx
)
{
  /* Keep track of the most recently used duration.  This should help
   * with the case where the same duration range is hit over and over
   * again. */
  static uint32             mru_mode_idx  = 0;
  static sleep_lookup_table *mru_lut      = 0;
  int                       end_idx       = 0;
  uint32                    curr_core     = CORE_CURR_CORE;
  sleep_LUT_elem            *mru_mode, *last_mode, *comp_mru_mode;
  sleep_lookup_table        *pModeLUT;
  
  CORE_VERIFY(curr_core < SLEEP_NUM_CORES);

  pModeLUT  = SleepLPR_power_lookup_table[curr_core][lutIdx];
  mru_mode  = &pModeLUT->element[mru_mode_idx];
  last_mode = &pModeLUT->element[pModeLUT->numElements - 1];

  /*Set comparison mode to one idx lower (higher duration)*/
  if( mru_mode_idx != 0)
    comp_mru_mode = &pModeLUT->element[mru_mode_idx - 1];
  else
    comp_mru_mode = mru_mode;

  /* Check the last recently used entry for a match. */
  if(pModeLUT == mru_lut)
  {
    /* If given duration > most recently used duration and                              
     * most recent duration is max or given duration is <= next higher duration 
     */ 
    if(mode_duration > mru_mode->duration_thresh &&
       (mru_mode_idx == 0 || mode_duration <= comp_mru_mode->duration_thresh))
    {
      /* Found a match */
      sleepStats_updateGeneric(&mru_mode->element_stats, mode_duration);
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
    last_mode = &pModeLUT->element[pModeLUT->numElements - 1];
  }
  
  mru_mode_idx  = pModeLUT->numElements / 2;
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
    end_idx = pModeLUT->numElements;
  }
  
  /* Traverse the rest of the table to find the duration that matches. */
  for(; mru_mode_idx < end_idx; mru_mode_idx++ )
  {
    if( mode_duration > pModeLUT->element[mru_mode_idx].duration_thresh)
    {
      /* Found it, return this element. */
      sleepStats_updateGeneric(&pModeLUT->element[mru_mode_idx].element_stats, 
                                mode_duration);
      return ( &pModeLUT->element[mru_mode_idx] );
    }
  }

  /* We should never make it this far. If we do, some logic failed above that 
   * should have caught the case were there is not enough time to do any modes.
   */
   sleep_log_printf( SLEEP_LOG_LEVEL_ERROR, 2*1, 
                    "Invalid duration (Value: 0x%llx)", 
                    ULOG64_DATA(mode_duration));

  CORE_VERIFY( 0 );
  return NULL;
}

/**
 * sleep_lookup_table_solver_init
 *
 * @brief Init function for the lookup table solver.  This 
 *        function populates the mode_to_enter field in the
 *        lookup table, per the CLprms registered.
 *
 * @return Returns non-zero on error.
 */
static uint32 sleep_lookup_table_solver_init( void )
{
  CLpr               *lpr;
  CLprRegistry       *registry;
  npa_query_handle   sleep_lpr_query_handle;
  npa_query_type     qres;
  uint32             coreIdx, freqIdx, durIdx, modeIdx;
  uint32             mode_index;

  CORE_VERIFY_PTR(g_sleepfLUTMutex = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT));

  /* Get the registry and the list of LPRs in the registry. */
  CORE_VERIFY_PTR( sleep_lpr_query_handle =
                   npa_create_query_handle( "/sleep/lpr" ) );

  /* Get the current LPR registry. */
  CORE_VERIFY( NPA_QUERY_SUCCESS == npa_query( sleep_lpr_query_handle,
                                               SLEEP_LPR_QUERY_RESOURCES,
                                               &qres ) );
  CORE_VERIFY( NPA_QUERY_TYPE_REFERENCE == qres.type );
  registry = (CLprRegistry *)qres.data.reference;

  /* Fill in the synthesized mode for each core. */
  for( coreIdx = 0; coreIdx < SLEEP_NUM_CORES; coreIdx++ )
  {
    /* Set initial head pointer to NULL */
    g_sleep_fLUT[coreIdx] = NULL;

    lpr = CLprRegistry_getLprsList( registry, coreIdx );

    /* There will always be at least 1 table */
    for(freqIdx = 0; freqIdx < SLEEP_NUM_STATIC_FREQ; freqIdx++)
    {
      sleep_lookup_table *pTable = 
        SleepLPR_power_lookup_table[coreIdx][freqIdx]; 

      for( durIdx = 0; durIdx < pTable->numElements; durIdx++)
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
              lpr->m_modes[mode_index];
          }
        }
      }
    }
  }

  return 0;
}

/**
 * sleep_lookup_table_solver_fcn
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
static void sleep_lookup_table_solver_fcn
(
  sleep_solver_input  *input,
  sleep_solver_output *output
)
{
  int                       i;
  uint64                    mode_duration;
  uint32                    mode_enter_lat      = 0;
  uint32                    mode_exit_lat       = 0;
  uint32                    mode_backoff        = 0;
  uint32                    mode_enter_exit_lat = 0;
  CLprm                     *mode_chosen        = NULL;
  sleep_LUT_elem            *lookup_ele         = NULL;
  sleep_freq_lookup_table   *pfreq_LUT          = input->pfreq_LUT;

  output->num_of_selected_modes = 0;

  if( SLEEP_ENABLED == 
        CLpr_getStatus( CLprRegistry_getLprsList( input->lpr_registry, -1 )) )
  {
    mode_duration = ( input->soft_duration < input->hard_duration ?
                      input->soft_duration : input->hard_duration );

    /* Find the lookup table entry for this duration. */
    lookup_ele = 
      sleep_lookup_table_solver_find_lookup_ele( mode_duration, 
                                                 pfreq_LUT->mLUT_idx);

    if(lookup_ele != NULL)
    {
      sleep_log_printf( SLEEP_LOG_LEVEL_INFO, 3,
                        "Solver table (mLUT: %d) (Duration: %lld)",
                        pfreq_LUT->mLUT_idx,
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

        /* Make sure the mode is enabled. */
        if( SLEEP_DISABLED == CLprm_getStatus( mode_chosen ) )
        {
          continue;
        }

        mode_enter_lat = CLprm_getEnterLatency( mode_chosen, mode_duration, 
                                                pfreq_LUT, 
                                                input->cycle_id );

        mode_exit_lat  = CLprm_getExitLatency(  mode_chosen, mode_duration,
                                                pfreq_LUT,
                                                input->cycle_id );

        mode_enter_exit_lat = mode_enter_lat + mode_exit_lat;

        /* Does the mode's enter and exit latency fit in the expected sleep
         * duration, and not violate the latency budget? */
        if( mode_enter_exit_lat > mode_duration ||
            mode_enter_exit_lat > input->latency_budget )
        {
          sleep_log_printf( SLEEP_LOG_LEVEL_INFO, 4 + 2*1 ,
                            "Mode skipped (name: \"%s\") "
                            "(reason: \"Insufficient Time\") "
                            "(enter latency: 0x%08x) (exit latency: 0x%08x) "
                            "(latency budget: 0x%08x) (duration: 0x%llx)",
                            mode_chosen->lprmName, 
                            mode_enter_lat, 
                            mode_exit_lat,
                            input->latency_budget, 
                            ULOG64_DATA(mode_duration) );
          continue;
        }

        mode_backoff = CLprm_getBackOffTime(  mode_chosen, mode_duration, 
                                              pfreq_LUT,
                                              input->cycle_id );

        /* If we made it this far, then we found the right mode.
         * Record its info and return. */
        output->selected_modes[output->num_of_selected_modes++] = mode_chosen;        
        sleep_log_printf( SLEEP_LOG_LEVEL_INFO, 1, 
                          "Mode chosen: (\"%s\")",
                          mode_chosen->lprmName);

        /* Prepare the data to return. */
        output->backoff_time = mode_backoff;
        output->enter_time   = mode_enter_lat;
        output->exit_time    = mode_exit_lat;

        break;
      }
    }
  }

  if( 0 == output->num_of_selected_modes )
  {
    sleep_log_printf( SLEEP_LOG_LEVEL_INFO, 0, "No mode chosen" );
  }
}

/* ==================================================================
                    EXTERNAL FUNCTIONS
   ================================================================== */

/*
 * sleep_lookup_table_add_fLUT
 */
sleep_freq_lookup_table* sleep_lookup_table_add_fLUT(
    sleep_freq_lookup_table   *closest_freq_LUT,
    uint32                    cur_freq,
    CLprRegistry              *registry)
{
  sleep_freq_lookup_table *newTable;
  sleep_freq_lookup_table *freqTable;
  uint32                  overWriteFlag = 0;
  uint32                  tmpDataIndex  = 0;
  uint32                  curr_core     = CORE_CURR_CORE;
  
  CORE_VERIFY_PTR(registry);
  CORE_VERIFY(curr_core < SLEEP_NUM_CORES);

  /* Lock access to fLUT and other assocaited data structures while the new
   * frequency is added */
  sleep_lookup_table_lock_fLUT();

  freqTable = g_sleep_fLUT[curr_core];

  /* Count number of FLUTs which will determine data index to use when
   * reallocating below */
  while(NULL != freqTable)
  {
    if(freqTable->frequency == 0)
    {
      /* Initial entry could possibly have a 0 frequency if scaling feature
       * is disabled. In this case, overwrite this entry with the first
       * legit frequency encountered.
       */ 
      overWriteFlag = 1;
      break;
    }

    tmpDataIndex++;
    freqTable = freqTable->next;
  }

  if(0 == overWriteFlag)
  {
    /* Add new fLUT element */
    CORE_VERIFY_PTR(newTable = malloc(sizeof(sleep_freq_lookup_table)));

    newTable->frequency = cur_freq;
    newTable->data_idx  = tmpDataIndex;
    newTable->next      = NULL;
  }
  else
  {
    /* Overwrite fLUT element that has a frequency of 0. This avoids the extra
     * memory allocation and the extra minLPRM update on an unused element */
    closest_freq_LUT  = NULL;
    newTable          = freqTable;
    newTable->frequency = cur_freq;
  }

  /* Find closest mLUT.
   * Note that SLEEP_NUM_STATIC_FREQ will always be at least 1 */
  for(newTable->mLUT_idx = 1; 
       newTable->mLUT_idx < SLEEP_NUM_STATIC_FREQ; 
       newTable->mLUT_idx++)
  {
    if(cur_freq < SleepLPR_static_freq_list[newTable->mLUT_idx])
      break;
  }

  /* Subtract one due to logic of finding closest, but less than frequency */
  newTable->mLUT_idx--;

  /* Verify that a closest frequency found, if not, initialize the head
   * fLUT pointer */
  if(closest_freq_LUT != NULL)
  {
    /* The closest frequency will be less than the current in all but one case
     * where it is the only (head) element.  In this case, the new element will
     * become the head. 
     */
    if(closest_freq_LUT->frequency > cur_freq)
    {
      newTable->next                = closest_freq_LUT;
      g_sleep_fLUT[curr_core] = newTable;
    }
    else
    {
      /* Insert new element in frequency order which is after closest */
      newTable->next          = closest_freq_LUT->next;
      closest_freq_LUT->next  = newTable;
    }
  }
  else
  {
    /* Set very first fLUT element */
    g_sleep_fLUT[curr_core] = newTable;
  }

  sleep_log_printf( SLEEP_LOG_LEVEL_DEBUG, 3,
                    "Adding frequency LUT "
                    "(freq: %d) (mode_LUT_idx: %d) (data_idx: %d)",
                    cur_freq, newTable->mLUT_idx, newTable->data_idx);

  if(0 == overWriteFlag)
  {
    /* Adjust data & cache arrays for new frequency entry */
    CLprm_add_data_elements(registry, newTable->data_idx);
  }

  /* New entry as been fully added - release the lock */
  sleep_lookup_table_unlock_fLUT();

  return newTable;
}

/*
 * sleep_lookup_table_get_entry
 */
boolean sleep_lookup_table_get_entry(uint32 freq, 
                                     sleep_freq_lookup_table **freq_LUT)
{
  static sleep_freq_lookup_table *cachedEntry[SLEEP_NUM_CORES] = {0};
  
  sleep_freq_lookup_table *freqTable;
  sleep_freq_lookup_table *workingCachePtr;
  sleep_freq_lookup_table *closest;
  uint32                  curr_core = CORE_CURR_CORE;
  boolean                 rtnCode           = TRUE;
  
  CORE_VERIFY(curr_core < SLEEP_NUM_CORES);
    
  /* Set initial returned LUT to cached LUT */
  workingCachePtr = cachedEntry[curr_core];
  *freq_LUT = workingCachePtr;

  freqTable       = g_sleep_fLUT[curr_core];  
  closest         = freqTable;

  /* Only scan tables if given frequency is different from cached value */
  if(workingCachePtr == 0 || (freq != workingCachePtr->frequency))
  {
    while(freqTable != NULL)
    {
      /* Check if we already have a frequency entry in the table */
      if(freq == freqTable->frequency)
      {
        /* update cache pointer */
        *freq_LUT                   = freqTable;
        cachedEntry[curr_core] = freqTable;
        return (TRUE);
      }

      /* Remember closest (but less than) entry */
      if(freqTable->frequency < freq)
        closest = freqTable;

      /* Check next entry */
      freqTable = freqTable->next;
    };

    /* At this point, a new entry needs to be added
     * Set the frequency LUT to the closest entry and return that new elements
     * need to be added.
     */
    *freq_LUT = closest;
    rtnCode   = FALSE;
  }

  return(rtnCode);
}

/*
 * sleep_lookup_table_lock_fLUT
 */
void sleep_lookup_table_lock_fLUT(void)
{
  Core_MutexLock(g_sleepfLUTMutex);
  return;
}

/*
 * sleep_lookup_table_unlock_fLUT
 */
void sleep_lookup_table_unlock_fLUT(void)
{
  Core_MutexUnlock(g_sleepfLUTMutex);
  return;
}

/*
 * sleep_get_freq_lut
 */
sleep_freq_lookup_table *sleep_get_freq_lut(uint32 core)
{
  CORE_VERIFY(core < SLEEP_NUM_CORES);
  return(g_sleep_fLUT[core]);
}

sleep_solver_type sleep_lookup_table_solver =
{
  sleep_lookup_table_solver_init,
  NULL, /* no deinit function */
  sleep_lookup_table_solver_fcn,
  "Lookup Table"
};
