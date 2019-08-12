/*==============================================================================
  FILE:         CLprm.c
  
  OVERVIEW:     Provides the CLprm class implementation for offline
                synthesis.
 
  DEPENDENCIES: None

                Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/compiler/CLprm.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <stdint.h>
#include <stdlib.h>
#include "CLprm.h"
#include "sleepi.h"
#include "sleep_lpr.h"
#include "sleep_lpri.h"
#include "sleep_stats.h"
#include "sleep_statsi.h"
#include "CoreTime.h"
#include "CoreVerify.h"
#include "sleep_log.h"
#include "CoreAtomicOps.h"
#include "lookup_table_solver.h"
#include "lookup_table_types.h"

/*
===============================================================================
                        Function Declarations & Macros
===============================================================================
*/

/* Undo hokey C preprocessor defines that make numeric_limits freak out. */
#undef min
#undef max

/**
 * sleep_cache_valid_flags
 *
 * @brief Indicates the bit corresponding to the CLprm latency type.
 *
 * If the bit is set, the cached value for that latency is valid.
 */
typedef enum
{
  SLEEP_ENTER_LATENCY_VALID   = 1,
  SLEEP_EXIT_LATENCY_VALID    = 8,
  SLEEP_BACKOFF_LATENCY_VALID = 16
} sleep_cache_valid_flags;

/*
===============================================================================
                        INTERNAL FUNCTION DEFINITIONS
===============================================================================
*/

/**
 * safe_signed_addition
 *
 * @brief Adds two signed 64 bit numbers and if the total is greater than the
 *        maximum value 64 bit number can hold (or is less than minimum value)
 *        it clips to max (or min) value.
 *
 * @param a: First integer
 * @param b: Second integer
 *
 * @return Total of two input integers (with clipping if requried).
 */
int64 safe_signed_addition(int64 a, int64 b)
{
  if((a < 0) && (b < 0) && ((INT64_MIN - a) > b))
  {
    /* clip to minimum */
    return INT64_MIN;
  }

  if((a > 0) && (b > 0) && ((INT64_MAX - a) < b))
  {
    /* clip to maximum */
    return INT64_MAX;
  }

  return (a + b);
}

/**
 * safe_unsigned_addition
 *
 * @brief Adds two unsigned integers of 64 bit and if their total exceeds the
 *        maximum value 64 bits can hold, it will clip the result.
 *
 * @param a: First unsigned integer.
 * @param b: Second unsigned integer.
 *
 * @return Total of two input unsigned numbers (with clipping if required).
 */
uint64 safe_unsigned_addition(uint64 a, uint64 b)
{
  if((UINT64_MAX - a) < b)
  {
    /* clip to maximum */
    return UINT64_MAX;
  }

  return (a + b);
}

/**
 * safe_signed_truncate
 *
 * @brief Converts the 64 bit integer to 32 bit integer and in case the input
 *        integer lies outside of range of 32 bit, it clips the number to
 *        the maximum or minimum limit.
 *
 * @param a: input 64 bit integer
 *
 * @return 32 bit integer equivalent of input number.
 */
int32 safe_signed_truncate(int64 a)
{
  if(a < INT32_MIN)
    return INT32_MIN;

  if(a > INT32_MAX)
    return INT32_MAX;

  return (int32)a;
}

/**
 * safe_unsigned_truncate
 *
 * @brief Converts the 64 bit unsigned integer to 32 bit unsigned integer and in
 *        case the input integer is greater than the maximum value 32 bit can
 *        hold, it clips.
 *
 * @param a: input 64 bit unsigned integer.
 *
 * @return 32 bit unsigned integer equivalent of input number.
 */
uint32 safe_unsigned_truncate(uint64 a)
{
  if(a > UINT32_MAX)
    return UINT32_MAX;

  return (uint32)a;
}

/**
 * dummy_enter_func
 *
 * @brief Dummy enter function which will be used if enter function for an LPRM
 *        is not provided by the user.
 *
 * The main purpose of this dummy fuction is to avoid comparison everytime
 * since most of the modes are expected to provide enter function.
 *
 * @param duration_sclk: time duration in sclk (Not used now though)
 */
static void dummy_enter_func(uint64 duration)
{
}

/**
 * dummy_exit_func
 *
 * @brief Dummy exit function which will be used if exit function for an LPRM
 *        is not provided by user.
 *
 * The main purpose of this dummy fuction is to avoid comparison everytime
 * since most of the modes are expected to provide exit function.
 */
static void dummy_exit_func(void)
{
}

/**
 * useLPRMCachedData
 *
 * @brief Determines if we can use cached data for latency for a given lprm.
 *
 * @note
 * Cycle id as zero is considered as invalid. This is mainly so that
 * we can call latency functions from outside of idle context e.g.
 * while  updating minimum latency LPRM after enabling/disabling some
 * LPRM. This is mainly useful in case we have all latency functions as
 * constants otherwise we would not have minimum latency enabled mode
 * known before sleep cycle starts.
 *
 * @param cached_data: Cached latency for lprm
 * @param cached_cycle_id: Cycle id stored in the cache for a given parameter.
 * @param cycle_id: Indicates a particular sleep cycle where we may want
 *                  to use cached function results.
 *
 * @return 1 if we can use cached data else 0.
 */
static inline int useLPRMCachedData
(
  uint32 cached_cycle_id,
  uint32 cycle_id
)
{
  return ((0 != cycle_id) && (cached_cycle_id == cycle_id) ? 1 : 0);
}

/*
===============================================================================

                        CLASS FUNCTION DEFINITIONS

===============================================================================
*/

/*
 * validate
 */
void CLprm_validate( sleep_lprm *lprm )
{
  /* If either of enter and exit function is empty, we can skip using lock for
   * the mode. */
  if(!lprm->enter_func)
  {
    lprm->enter_func = dummy_enter_func;
    lprm->attributes |= SLEEP_MODE_ATTR_NO_FCN_LOCK;
  }
  if(!lprm->exit_func)
  {
    lprm->exit_func = dummy_exit_func;
    lprm->attributes |= SLEEP_MODE_ATTR_NO_FCN_LOCK;
  }

  /* Setting up mode's attributes */
  CLprm_setComponentModeAttributes( lprm );

  if( 0 == (lprm->attributes & SLEEP_MODE_ATTR_NO_FCN_LOCK) )
  {
    /* Mode requires lock for its enter and exit functions. */
    sleep_os_create_mode_fcn_lock(lprm);
  }

  if( 0 == (lprm->attributes & SLEEP_MODE_ATTR_NO_FCN_OPTIMIZATION) )
  {
    /* Setting up cached data for optimization to avoid repetitive calls to
     * non constant latency function */
    lprm->cached_data = (void *)malloc(sizeof(sleep_lprm_cached_fcn_data));

    if( NULL != lprm->cached_data )
    {
      memset( lprm->cached_data, 0, sizeof(sleep_lprm_cached_fcn_data) );
    }
    else
    {
      sleep_log_printf(SLEEP_LOG_LEVEL_WARNING, 2,
                       "WARNING (message: \"Malloc failure for caching data."
                       " Some optimizations will not be performed.\")"
                       " (LPR: \"%s\") (LPRM: \"%s\")",
                       lprm->lpr->resource_name, lprm->mode_name );
    }
  }
  else
  {
    /* No latency function optimization is requested from mode owner.
     * Setting cached data field to NULL pointer has advantage here.
     * Before writting to cached_data we ensure it to be non-null. So in the
     * case where we have SLEEP_MODE_ATTR_NO_FCN_OPTIMIZATION flag on, we
     * always call latency functions and don't cached them in vain.
     */
    lprm->cached_data = NULL;
  }

}

/*
 * setupLatencyCache
 */
void CLprm_setupLatencyCache( CLprm *hCLprm )
{
  uint32                  i, j;
  CLprm                   **parents;
  sleep_lprm              *lprm;
  sleep_cache_valid_flags cacheFlags;
  uint32                  enterCache[SLEEP_NUM_STATIC_FREQ] = {0};
  
  CORE_VERIFY_PTR(hCLprm);

  hCLprm->cached_enter_latency    = NULL;
  hCLprm->cached_exit_latency     = NULL;
  hCLprm->cached_backoff_latency  = NULL;
  hCLprm->num_data_elements       = 0;

  cacheFlags = ( SLEEP_ENTER_LATENCY_VALID |
                 SLEEP_EXIT_LATENCY_VALID |
                 SLEEP_BACKOFF_LATENCY_VALID );

  /* Iterating through each component mode */
  for( i = 0; i < hCLprm->num_expected_component_modes; i++ )
  {
    lprm = hCLprm->m_componentModes[i];

    /* Initialize the list of parents for this mode, if it's not already. */
    if( NULL == lprm->cLprmParents )
    {
      /* NBG: Multicore environment has been considered while allocating 
       *      memory. We need number of cores as a multiplicand in size */
      CORE_VERIFY_PTR( lprm->cLprmParents = 
                       malloc( sizeof( void * ) * 
                               SLEEP_LPR_NUM_SYNTHESIZED_MODES * 
                               SLEEP_NUM_CORES) );

       memset( lprm->cLprmParents, 0, 
               sizeof( void * ) * SLEEP_LPR_NUM_SYNTHESIZED_MODES * 
               SLEEP_NUM_CORES); 
    }

    parents = ( CLprm ** )lprm->cLprmParents;

    /* Set the component mode's parent to this CLprm */
    parents[lprm->parentsIdx++] = hCLprm;

    /* Checking if enter latency can be cached */
    if( lprm->enter_lat.tag == LAT_FUNC_CONST_SCALE )
    {
      for(j=0; j<gSleepNumOfFreqs; j++)
      {
        enterCache[j] += lprm->enter_lat.latency.fList[j];
      }
    }
    else
    {
      cacheFlags &= ~(SLEEP_ENTER_LATENCY_VALID);
    }

    /* Checking if exit latency can be cached */
    if( lprm->exit_lat.tag != LAT_FUNC_CONST_SCALE )
    {
      cacheFlags &= ~(SLEEP_EXIT_LATENCY_VALID);
    }

    /* Back off latency caching */
    if( (lprm->backoff_lat.tag == LAT_FUNC_CUSTOM) ||
        ((lprm->backoff_lat.tag == LAT_FUNC_NONE) &&
         (lprm->exit_lat.tag != LAT_FUNC_CONST_SCALE)) )
    {
      cacheFlags &= ~(SLEEP_BACKOFF_LATENCY_VALID);
    }
  }

  /* Only allocate cache memory if there are no custom latency functions  
   * Since enter latency is not adjusted, populate available entries */
  if(cacheFlags & SLEEP_ENTER_LATENCY_VALID)
  {
    CORE_VERIFY_PTR( hCLprm->cached_enter_latency = 
                     malloc( sizeof(uint32) * gSleepNumOfFreqs) );

    for(j=0; j<gSleepNumOfFreqs; j++)
      hCLprm->cached_enter_latency[j] = enterCache[j];
  }

  /* For exit & backoff latency, allocate 1 empty entry which will be populated
   * when the first frequency LUT is added during the first sleep cycle.  This
   * is done to indicate that there are no custom functions that need to be
   * called.
   */ 
  if(cacheFlags & SLEEP_EXIT_LATENCY_VALID)
  {
    CORE_VERIFY_PTR( hCLprm->cached_exit_latency = malloc( sizeof(uint32)));
    hCLprm->cached_exit_latency[0] = 0;
  }

  if(cacheFlags & SLEEP_BACKOFF_LATENCY_VALID)
  {
    CORE_VERIFY_PTR( hCLprm->cached_backoff_latency = malloc( sizeof(uint32)));
    hCLprm->cached_backoff_latency[0] = 0;
  }

  return;
}

/*
 * CLprm
 */
void CLprm_initSynth( CLprm *hCLprm, sleep_lprm_synth *lprm)
{
  size_t alloc_size;

  CORE_VERIFY_PTR(hCLprm);
  CORE_LOG_VERIFY_PTR( lprm, sleep_log_printf(SLEEP_LOG_LEVEL_ERROR, 0,
                             "ERROR (message: \"NULL Synthesized LPRM\")" ) );

  hCLprm->num_expected_component_modes = lprm->num_component_modes;
  hCLprm->num_enabled_component_modes = 0;
  hCLprm->lprmName = lprm->mode_name;
  hCLprm->m_parent = NULL;
  hCLprm->m_status = SLEEP_DISABLED;

  alloc_size = sizeof(sleep_lprm *) * hCLprm->num_expected_component_modes;

  hCLprm->m_componentModes = lprm->m_componentModes;

  CLprm_setupLatencyCache( hCLprm );

  sleepStats_initLprmStatData(hCLprm);

  /* NBG: With arrays for component modes already in place, we may not 
   * need this entered_modes array. */
  CORE_VERIFY_PTR(hCLprm->entered_modes = malloc(alloc_size));
  memset(hCLprm->entered_modes, 0, alloc_size);
}

/*
 * createSynth
 */
CLprm* CLprm_createSynth(sleep_lprm_synth *lprm)
{
  CLprm* self = malloc(sizeof(CLprm));
  CORE_VERIFY_PTR(self);
  CLprm_initSynth(self, lprm);

  return self;
}

/*
 * ~CLprm (Destructor)
 */
void CLprm_deinit( CLprm *hCLprm )
{
}

/*
 * setParent
 */
void CLprm_setParent( CLprm *hCLprm, CLpr* parent )
{
  CORE_VERIFY_PTR(hCLprm);

  hCLprm->m_parent = parent;
}

/*
 * getStatus
 */
sleep_status CLprm_getStatus( CLprm *hCLprm )
{
  CORE_VERIFY_PTR(hCLprm);

  return hCLprm->m_status;
}

/*
 * getLPRMLatency
 */
uint32 CLprm_getLPRMLatency
(
  CLprm           *hCLprm,
  sleep_lat_data  *latData,
  uint32          mLUTIdx,
  uint64          duration
)
{
  uint32 latency_val = 0;

  CORE_VERIFY_PTR(hCLprm);
  switch(latData->tag)
  {
    case LAT_FUNC_CONST_SCALE: /* LPRM has scaled latency */
    {
      latency_val = latData->latency.fList[mLUTIdx];
      break;
    }

    case LAT_FUNC_CUSTOM: /* LPRM has custom function */
    {
      latency_val =
       (((sleep_latency_func_type1)(latData->latency.function))(NULL,duration));
      break;
    }

    case LAT_FUNC_NONE:
    default:
    {
      break;
    }
  }

  return latency_val;
}

/*
 * getEnterLatency
 */
uint32 CLprm_getEnterLatency( CLprm                   *hCLprm,
                              uint64                  duration,
                              sleep_freq_lookup_table *pfreq_LUT,
                              uint32                  cycle_id )
{
  uint64                      temp_enter;
  int                         i;
  sleep_lprm                  *component;
  sleep_lprm_cached_fcn_data  *cached_data;
  uint64                      enter         = 0;
  uint32                      curr_core     = CORE_CURR_CORE;
  uint32                      mode_LUT_idx  = pfreq_LUT->mLUT_idx;

  CORE_VERIFY_PTR(hCLprm);

  /* Check if all component latencies cached */
  if( hCLprm->cached_enter_latency != NULL )
  {
    return hCLprm->cached_enter_latency[mode_LUT_idx];
  }

  /* Iterating through each component mode */
  for( i = 0; i < hCLprm->num_expected_component_modes; i++ )
  {
    component     = hCLprm->m_componentModes[i];
    cached_data   = (sleep_lprm_cached_fcn_data *)component->cached_data;
    
    /* Checking if we can use the cached data for enter latency */
    if( NULL != cached_data && 
        useLPRMCachedData(cached_data->enter_cycle_id[curr_core][mode_LUT_idx], 
                          cycle_id) )
    {
      temp_enter = cached_data->enter_latency[curr_core][mode_LUT_idx]; 
    }
    else
    {
      temp_enter = CLprm_getLPRMLatency(hCLprm,
                                        &component->enter_lat,
                                        mode_LUT_idx,
                                        duration);

      if((NULL != cached_data) && (0 != cycle_id))
      {
        cached_data->enter_cycle_id[curr_core][mode_LUT_idx] = cycle_id;
        cached_data->enter_latency[curr_core][mode_LUT_idx]  = temp_enter;
      }
    }

    enter = safe_unsigned_addition(enter, temp_enter);
  }

  return (safe_unsigned_truncate(enter));
}

/*
 * getExitLatency
 */
uint32 CLprm_getExitLatency(CLprm                   *hCLprm,
                            uint64                  duration,
                            sleep_freq_lookup_table *pfreq_LUT,
                            uint32                  cycle_id )
{
  int                         i;
  sleep_lprm                  *component;
  sleep_lprm_cached_fcn_data  *cached_data;
  uint64                      temp_exit;
  uint64                      exit_lat      = 0;
  uint32                      curr_core     = CORE_CURR_CORE;
  uint32                      mode_LUT_idx  = pfreq_LUT->mLUT_idx;
  uint32                      data_idx      = pfreq_LUT->data_idx;

  CORE_VERIFY_PTR(hCLprm);

  /* Check if component latencies are cached */
  if( hCLprm->cached_exit_latency != NULL)
  {
    if(hCLprm->cached_exit_latency[data_idx] != 0)
      return hCLprm->cached_exit_latency[data_idx];
  }

  /* If cached value is 0, a new frequency table has been added and this
   * is the first time the mode has been checked at this frequency.
   * 
   * Since the automatic backoff adjustment feature could possibly adjust exit
   * latency as well as backoff, we need to initialize the new exit element.
   * 
   * This does not need to be done for enter latency since it is never
   * adjusted. */

  /* Iterating through each component mode */
  for( i = 0; i < hCLprm->num_expected_component_modes; i++ )
  {
    component     = hCLprm->m_componentModes[i];
    cached_data   = (sleep_lprm_cached_fcn_data *)component->cached_data;    

    if( NULL != cached_data &&
        useLPRMCachedData(cached_data->exit_cycle_id[curr_core][mode_LUT_idx], 
                          cycle_id) )
    {
      temp_exit = cached_data->exit_latency[curr_core][mode_LUT_idx];
    }
    else
    {
      temp_exit = CLprm_getLPRMLatency(hCLprm,
                                       &component->exit_lat,
                                       mode_LUT_idx,
                                       duration);

      if((NULL != cached_data) && (0 != cycle_id))
      {
        cached_data->exit_cycle_id[curr_core][mode_LUT_idx] = cycle_id;
        cached_data->exit_latency[curr_core][mode_LUT_idx]  = temp_exit;
      }
    }

    exit_lat = safe_unsigned_addition(exit_lat, temp_exit);
  }

  /* If cached entry is not NULL, this means that cacheing of the synthmode
   * is possible, but this is the first time we're checking it, so set it to
   * the correct value for future calls */
  if( hCLprm->cached_exit_latency != NULL )
  {
    hCLprm->cached_exit_latency[data_idx] = safe_unsigned_truncate(exit_lat);
  }

  return safe_unsigned_truncate(exit_lat);
}

/*
 * getBackOffTime
 */
uint32 CLprm_getBackOffTime( CLprm                    *hCLprm,
                             uint64                   duration,
                             sleep_freq_lookup_table  *pfreq_LUT,
                             uint32                   cycle_id )
{
  int                         i;
  sleep_lprm                  *component;
  sleep_lprm_cached_fcn_data  *cached_data;
  uint64                      total_backoff_time  = 0;
  uint32                      curr_core           = CORE_CURR_CORE;
  uint64                      component_backoff   = 0;
  uint32                      mode_LUT_idx        = pfreq_LUT->mLUT_idx;
  uint32                      data_idx            = pfreq_LUT->data_idx;
  
  CORE_VERIFY_PTR(hCLprm);

  /* Component latencies cached */
  if( hCLprm->cached_backoff_latency != NULL )
  {
    /* See comment in CLprm_getExitLatency for more details */
    if(hCLprm->cached_backoff_latency[data_idx] != 0)
      return hCLprm->cached_backoff_latency[data_idx];
  }

  /* Iterating through each component mode */
  for( i = 0; i < hCLprm->num_expected_component_modes; i++ )
  {
    component = hCLprm->m_componentModes[i];
    cached_data = (sleep_lprm_cached_fcn_data *)component->cached_data;

    if( NULL != cached_data &&
        useLPRMCachedData(cached_data->backoff_cycle_id[curr_core][mode_LUT_idx],
                          cycle_id) )
    {
      component_backoff = cached_data->backoff[curr_core][mode_LUT_idx];
    }
    else
    {
      if(component->backoff_lat.tag != LAT_FUNC_NONE)
      {
        /* Back off timing is specified. */
        component_backoff = CLprm_getLPRMLatency(hCLprm,
                                                 &component->backoff_lat,
                                                 mode_LUT_idx,
                                                 duration);
      }
      else
      {
        /* Back off timing for this component mode was not provided.
         * We will use exit latency instead.
         */
        component_backoff = CLprm_getLPRMLatency(hCLprm,
                                                 &component->exit_lat,
                                                 mode_LUT_idx,
                                                 duration);
      }

      if((NULL != cached_data) && (0 != cycle_id))
      {
        cached_data->backoff_cycle_id[curr_core][mode_LUT_idx] = cycle_id;
        cached_data->backoff[curr_core][mode_LUT_idx] = component_backoff;
      }
    }

    total_backoff_time =
      safe_unsigned_addition(total_backoff_time, component_backoff);
  }

  if( hCLprm->cached_backoff_latency != NULL )
  {
    hCLprm->cached_backoff_latency[data_idx] = 
      safe_unsigned_truncate(total_backoff_time);
  }

  return safe_unsigned_truncate(total_backoff_time);
}

/*
 * enter
 */
void CLprm_enter(CLprm    *hCLprm, 
                 uint64   wakeupTick, 
                 uint32   data_idx)
{
  sleep_lprm  *component;
  uint32      mode_attrs, no_ref_count, shared_mode;
  int         i;
#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  uint32      no_autoLat;
#endif
  int         index       = 0;
  uint64      lprm_time   = CoreTimetick_Get64();
  boolean     recordStats = (data_idx != (uint32)-1) ? TRUE : FALSE;
  
  CORE_VERIFY_PTR(hCLprm);

  /* setup initial value */
  hCLprm->wakeup_tick = wakeupTick;

  /* Iterating through each component mode */
  for( i = 0; i < hCLprm->num_expected_component_modes; i++ )
  {
    /* Calling the actual enter functions of the component modes */
    component     = hCLprm->m_componentModes[i];
    mode_attrs    = component->attributes;
    no_ref_count  = mode_attrs & SLEEP_MODE_ATTR_NO_REF_COUNT;
    shared_mode   = component->lpr->sharing_cores &
                            (component->lpr->sharing_cores - 1);
#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
    no_autoLat    = mode_attrs & SLEEP_MODE_ATTR_NO_AUTO_LATENCY_TRACK;
#endif

    /* Preparing stack for reverse order of exit function calls */
    hCLprm->entered_modes[index++] = component;

    /* Actually entering the component mode based on condition */
    if( (0 != no_ref_count) ||
        ((0 == no_ref_count) &&
         (0 == Core_AtomicDec((volatile uint32_t *)&component->cores_count))) )
    {
      /* Sleep statistics */
      component->lpr->run_count++;              /* Update LPR cycles*/
      component->cycleStats.mode_run_cycles++;  /* Update LPRM cycles */

      /* update LPRM cycles @ specific freq */
      if(recordStats == TRUE)
        component->mode_cycles_exact[data_idx]++; 

      if( 0 == (mode_attrs & SLEEP_MODE_ATTR_NO_FCN_LOCK) )
        sleep_fcn_lock(component->fn_lock);

      /* Checks to avoid unnecessary calculations of sleep statistics. Sleep
       * will calculate stats if it is handling reference counting for the
       * shared mode (first condition) or the mode is local (second
       * condition). If LPR owner chooses to handle ref count for shared
       * mode(s) by himself, it is his responsibility to calculate mode
       * statistics */
      if( (0 == no_ref_count) || (0 == shared_mode) )
        component->cycleStats.last_mode_time = lprm_time; /* For time in mode */

      /* Logging mode entry message */
      sleep_log_printf_qdss( SLEEP_LOG_LEVEL_PROFILING, 
                             SLEEP_ENTER_MODE_NUM_ARGS,
                             SLEEP_ENTER_MODE_STR, SLEEP_ENTER_MODE, 
                             component->lpr->resource_name,
                             component->mode_name,
                             ULOG64_DATA(lprm_time));

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
      /* Record local enter function delta befor executing LPRM enter function 
       * if the LPRM manual latency bit is set
       */
      if(0 != no_autoLat)
        lprm_time = CoreTimetick_Get64() - lprm_time;
#endif

      /* Call the enter function with the latest sleep duration value */
      component->enter_func(hCLprm->wakeup_tick);

      if( 0 == (mode_attrs & SLEEP_MODE_ATTR_NO_FCN_LOCK) )
        sleep_fcn_unlock(component->fn_lock);

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
      /* Record LPRM enter time. Include LPRM enter function if automatic
       * mode. Otherwise get latency data from LPRM and add it to local
       * execution time delta.
       */
      if(0 == no_autoLat)
        lprm_time = CoreTimetick_Get64() - lprm_time;
      else
        lprm_time += 
            sleepStats_getLastLprLatency(SLEEP_STATS_ENTER_LATENCY_TYPE);

      /* Get statistics for enter function, only track exact match in frequency 
         table */
      if(recordStats == TRUE)
      {
        sleepStats_lprm *enterStats = 
            &(((sleepStats_lprm *)component->mode_stats_enter)[data_idx]);

        enterStats->running_time += lprm_time;

        if(lprm_time > enterStats->max_lat)
          enterStats->max_lat = lprm_time;

        if(lprm_time < enterStats->min_lat)
          enterStats->min_lat = lprm_time;
      }
#endif
    }
    /* Get start of next LPRM */
    lprm_time = CoreTimetick_Get64();
  }

  return;
}

/*
 * exit
 */
void CLprm_exit( CLprm    *hCLprm, 
                 uint32   data_idx)
{
  int         index;
  sleep_lprm  *component;
  uint32      mode_attrs, no_ref_count, shared_mode;
  uint64      lprm_time   = CoreTimetick_Get64();
#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  boolean     recordStats = (data_idx != (uint32)-1) ? TRUE : FALSE;
  uint32      no_autoLat;
#endif
  
  CORE_VERIFY_PTR(hCLprm);

  for( index = hCLprm->num_expected_component_modes - 1; index >=0; index--)
  {
    /* Calling the exit functions of the component modes */
    component     = hCLprm->entered_modes[index];
    mode_attrs    = component->attributes;
    no_ref_count  = mode_attrs & SLEEP_MODE_ATTR_NO_REF_COUNT;
    shared_mode   = component->lpr->sharing_cores &
                          (component->lpr->sharing_cores - 1);
#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
    no_autoLat    = mode_attrs & SLEEP_MODE_ATTR_NO_AUTO_LATENCY_TRACK;
#endif

    sleep_log_printf_qdss( SLEEP_LOG_LEVEL_PROFILING, 
                           SLEEP_EXIT_MODE_NUM_ARGS,
                           SLEEP_EXIT_MODE_STR, SLEEP_EXIT_MODE, 
                           component->lpr->resource_name,
                           component->mode_name,
                           ULOG64_DATA(lprm_time));

    if( (0 != no_ref_count) ||
        ((0 == no_ref_count) &&
         (1 == Core_AtomicInc((volatile uint32_t *)&component->cores_count))) )
    {
      /* Actually exiting the component mode */
      if( 0 == (mode_attrs & SLEEP_MODE_ATTR_NO_FCN_LOCK) )
        sleep_fcn_lock(component->fn_lock);
      
#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
      if(0 != no_autoLat)
        lprm_time = CoreTimetick_Get64() - lprm_time;
#endif

      /* Call LRPM exit function */
      component->exit_func();

      if( 0 == (mode_attrs & SLEEP_MODE_ATTR_NO_FCN_LOCK) )
        sleep_fcn_unlock(component->fn_lock);
        
#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
      if(0 == no_autoLat)
        lprm_time = CoreTimetick_Get64() - lprm_time;
      else
        lprm_time += 
          sleepStats_getLastLprLatency(SLEEP_STATS_BACKOFF_LATENCY_TYPE);

      /* Only track an exact frequency match in the lookup table */
      if(recordStats == TRUE)
      {
        sleepStats_lprm *backoffStats =
          &(((sleepStats_lprm *)component->mode_stats_backoff)[data_idx]);

        backoffStats->running_time += lprm_time;

        if(lprm_time > backoffStats->max_lat)
          backoffStats->max_lat = lprm_time;

        if(lprm_time < backoffStats->min_lat)
          backoffStats->min_lat = lprm_time;
      }
#endif

      if( (0 == no_ref_count) || (0 == shared_mode) ) 
      {
        /* calculate last time in mode stats */
        component->cycleStats.last_mode_time = CoreTimetick_Get64() - 
          component->cycleStats.last_mode_time;

        /* Keep running total of mode time */
        component->cycleStats.in_mode_time += 
          component->cycleStats.last_mode_time;
      }
    }
    lprm_time = CoreTimetick_Get64();

    /* Not required but just for safety*/
    hCLprm->entered_modes[index] = NULL;
  }
}

/*
 * update
 */
void CLprm_update( CLprm *hCLprm, sleep_lprm *changed_mode )
{
  sleep_status                old_status;
  unsigned int                i, j;
  sleep_lprm_cached_fcn_data  *temp_cached_data = NULL;

  CORE_VERIFY_PTR(hCLprm);
  CORE_VERIFY_PTR(changed_mode);

  old_status = hCLprm->m_status;

  /* Update the number of enabled modes */
  if( TRUE == changed_mode->mode_enabled )
  {
    hCLprm->num_enabled_component_modes++;
  }
  else
  {
    /* Invalidating cache to avoid stale feed in rollover scenario. */
    if( ( 0 == ( changed_mode->attributes & 
                 SLEEP_MODE_ATTR_NO_FCN_OPTIMIZATION ) ) &&
        ( NULL != changed_mode->cached_data ) )
    {
      temp_cached_data = 
        ( sleep_lprm_cached_fcn_data * )changed_mode->cached_data;

      for(i = 0; i < gSleepNumOfCores; i++)
      {
        for(j = 0; j < gSleepNumOfFreqs; j++)
        {
          temp_cached_data->enter_cycle_id[i][j]   = 0;
          temp_cached_data->exit_cycle_id[i][j]    = 0;
          temp_cached_data->backoff_cycle_id[i][j] = 0;
        }
      }
    }

    hCLprm->num_enabled_component_modes--;
  }

  /* Make sure the number of enabled component modes is not out of range. */
  CORE_VERIFY( hCLprm->num_enabled_component_modes >= 0 &&
               hCLprm->num_enabled_component_modes <= 
                 hCLprm->num_expected_component_modes );
  
  if( hCLprm->num_enabled_component_modes == 
      hCLprm->num_expected_component_modes )
  {
    hCLprm->m_status = SLEEP_ENABLED;
  }
  else
  {
    hCLprm->m_status = SLEEP_DISABLED;
  }

  /* If we changed state, check for a parent and update it if we have one */
  if(hCLprm->m_status != old_status)
  {
    if(hCLprm->m_parent)
    {
      CLpr_modeUpdated( hCLprm->m_parent, hCLprm );
    }
  }
}

/*
 * hasConstantLatencies
 */
boolean CLprm_hasConstantLatencies( CLprm *hCLprm )
{
  CORE_VERIFY_PTR(hCLprm);
  /*
   * Right now we are just caching constant latencies and hence if any latency
   * cache is valid, we can say it is constant. However, if we add caching for
   * other types of latencies then valid cache may not mean constant latency
   * and we will need to traverse component modes to determine this.
   */
  if( (hCLprm->cached_enter_latency != NULL) && 
      (hCLprm->cached_exit_latency != NULL )) 
  {
    /* Both enter and exit latencies are constant for all component modes */
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*
 * setComponentModeAttribute
 */
void CLprm_setComponentModeAttributes( sleep_lprm *mode )
{
  uint32 sharing_cores;

  CORE_VERIFY_PTR(mode);

  /* Set up the component mode cache attribute */
  if( SLEEP_NUM_CORES > 1 )
  {
    /* Component mode caching is not currently supported on multicore.
     * In order to support it, we'd need to resolve the possibility of
     * modifying the cycle ID both at idle time, as well as in the mode 
     * disable path. */ 
    mode->attributes |= SLEEP_MODE_ATTR_NO_FCN_OPTIMIZATION;
  }
  
  sharing_cores = mode->lpr->sharing_cores;

  if( (1 == SLEEP_NUM_CORES) ||
      (0 == (sharing_cores & (sharing_cores-1)))  )
  {
    /* Single core environment or local mode */

    mode->attributes |=
         (SLEEP_MODE_ATTR_NO_REF_COUNT | SLEEP_MODE_ATTR_NO_FCN_LOCK);
    mode->cores_count = 1;
  }
  else
  {
    /* Multicore environment with non-local mode */
    if( 0 == (mode->attributes & SLEEP_MODE_ATTR_NO_REF_COUNT) )
    {
      /* Sleep will handle reference count for the mode */
      if( sharing_cores == SLEEP_ALL_CORES )
      {
        mode->cores_count = SLEEP_NUM_CORES;         /* shared on all cores */
      }
      else
      {
        unsigned int i;
        for(i = 0; i < SLEEP_NUM_CORES; i++)
        {
          if( sharing_cores & SLEEP_CORE(i) )
          {
            mode->cores_count++;
          }
        }
      }
    }

    /* Code for other attributes if any */
  }
}

/*
 * CLprm_add_data_elements
 */
void CLprm_add_data_elements(CLprRegistry  *registry,
                             uint32        data_idx)
{
  uint32  coreIdx, synthIdx;
  CLpr    *lpr;
  CLprm   *lprm;

  /* Add OS power collapse overhead element */
  sleepStats_addPowerCollapseOverheadElem();

  for( coreIdx = 0; coreIdx < SLEEP_NUM_CORES; coreIdx++ )
  {
    CORE_VERIFY_PTR(registry->m_minLprm[coreIdx] = 
                    (CLprm**)realloc(registry->m_minLprm[coreIdx],
                                     sizeof(CLprm*) * 
                                     (data_idx + 1)));

    registry->m_minLprm[coreIdx][data_idx] = NULL;

    lpr = CLprRegistry_getLprsList( registry, coreIdx );

    for( synthIdx = 0; synthIdx < SLEEP_LPR_NUM_SYNTHESIZED_MODES; synthIdx++)
    {
      lprm = lpr->m_modes[synthIdx];

      /* The fLUT passed in will have the last (and hence the number of) data
       * index */
      if(lprm->num_data_elements <= data_idx)
      {
        /* Increase number of data elements allocated which will always start
         * at 0 */
        lprm->num_data_elements++;

        /* If there are no custom latency functions, the first entry will
         * already be malloc'ed as part of init code.
         * Realloc only needs to be done if we have more than 1 entry.
         */
        if(lprm->num_data_elements > 1)
        {
          /* Since we don't currently adjust enter latencies, there is no need
           * to realloc it, only need to do exit & backoff */
          if(lprm->cached_exit_latency != NULL)
          {
            CORE_VERIFY_PTR(lprm->cached_exit_latency = 
                            (uint32 *)realloc(lprm->cached_exit_latency,
                                              sizeof(uint32) * 
                                              lprm->num_data_elements));

            lprm->cached_exit_latency[lprm->num_data_elements - 1] = 0;
          }

          if(lprm->cached_backoff_latency != NULL)
          {
            CORE_VERIFY_PTR(lprm->cached_backoff_latency = 
                            (uint32 *)realloc(lprm->cached_backoff_latency,
                                              sizeof(uint32) * 
                                              lprm->num_data_elements));

            lprm->cached_backoff_latency[lprm->num_data_elements - 1] = 0;
          }
        }

        /* Adjust statistic arrays */
        sleepStats_addStatsElem(lprm);
      }
    }
  }
  return;
}
