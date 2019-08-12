/*==============================================================================
  FILE:         synthLPRM.c
  
  OVERVIEW:     Provides the synth lprm functions
 
  DEPENDENCIES: None

                Copyright (c) 2013-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/synthesizer/synthLPRM.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <stdlib.h>
#include "sleepi.h"
#include "sleep.h"
#include "sleep_statsi.h"
#include "sleep_lpri.h"
#include "synthLPRM.h"
#include "synthLPR.h"
#include "CoreAtomicOps.h"
#include "lookup_table_types.h"
#include "sleep_qr.h"

/*==============================================================================
                        INTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/**
 * LPRM_dummy_enter_func
 *
 * @brief Dummy enter function which will be used if enter function for an LPRM
 *        is not provided by the user.
 *
 * The main purpose of this dummy fuction is to avoid comparison everytime
 * since most of the modes are expected to provide enter function.
 *
 * @param duration_sclk: time duration in sclk (Not used now though)
 */
static void LPRM_dummy_enter_func(uint64 duration)
{
  return;
}

/**
 * LPRM_dummy_exit_func
 *
 * @brief Dummy exit function which will be used if exit function for an LPRM
 *        is not provided by user.
 *
 * The main purpose of this dummy fuction is to avoid comparison everytime
 * since most of the modes are expected to provide exit function.
 */
static void LPRM_dummy_exit_func(void)
{
  return;
}

/**
 * SynthLPRM_QueryCacheable
 *
 * @brief Determine whether all of the components have cacheable
 *        attribute set.
 * 
 * @param Pointer to Synth Mode that is being initialized
 *
 * @return Attribute value representing whether synth mode is cacheable
 *         or uncacheable
 *
 * @note This function should be moved to sleep synth for efficiency
 */
static synth_mode_attr SynthLPRM_QueryCacheable( sleep_synth_lprm *synthLPRM )
{
  /* Incoming pointer points to Sleep Synth generated data. Cannot be NULL */
  uint32 i;
  uint32 numModes = synthLPRM->num_component_modes;
  synth_mode_attr attr = SYNTH_MODE_ATTR_LPR_CACHEABLE;
  sleep_lprm *pLPRM;

  for ( i = 0; i < numModes; i++ )
  {
    pLPRM = synthLPRM->component_modes[i];
    
    if ( ( pLPRM->attributes & SLEEP_MODE_ATTR_LPR_CACHEABLE ) == 0 )
    {
      attr = SYNTH_MODE_ATTR_LPR_UNCACHEABLE;
      break;
    }
  }

  return attr;
}
/*==============================================================================
                           FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * synthLPRM_validate
 */
void synthLPRM_validate( sleep_lprm *sleepLPRM )
{
  /* If either of enter and exit function is empty, we can skip using lock for
   * the mode. */
  if(!sleepLPRM->enter_func)
  {
    sleepLPRM->enter_func = LPRM_dummy_enter_func;
    sleepLPRM->attributes |= SLEEP_MODE_ATTR_NO_FCN_LOCK;
  }
  if(!sleepLPRM->exit_func)
  {
    sleepLPRM->exit_func = LPRM_dummy_exit_func;
    sleepLPRM->attributes |= SLEEP_MODE_ATTR_NO_FCN_LOCK;
  }

  /* Setting up mode's attributes */
  synthLPRM_setComponentModeAttributes(sleepLPRM);

  if( 0 == (sleepLPRM->attributes & SLEEP_MODE_ATTR_NO_FCN_LOCK) )
  {
    /* Mode requires lock for its enter and exit functions. */
    sleepOS_createLPRMFunctionLock(sleepLPRM);
  }
  return;
}

/*
 * synthLPRM_setupComponentModeParents
 */
void synthLPRM_setupComponentModeParents( sleep_synth_lprm *synthLPRM )
{
  uint32      i;
  uint32      newParentIdx;
  sleep_lprm  *sleepLPRM;
  void        *tmpPtr;
  
  CORE_VERIFY_PTR(synthLPRM);

  /* Iterating through each component mode */
  for( i = 0; i < synthLPRM->num_component_modes; i++ )
  {
    sleepLPRM     = synthLPRM->component_modes[i];
    newParentIdx  = sleepLPRM->synth_LPRM_parent_count++;

    CORE_VERIFY_PTR(tmpPtr = realloc(sleepLPRM->synth_LPRM_parent_array,
                                     sizeof(void *) * sleepLPRM->synth_LPRM_parent_count));

    sleepLPRM->synth_LPRM_parent_array = tmpPtr;

    /* Set the component mode's parent to this synthmode */
    sleepLPRM->synth_LPRM_parent_array[newParentIdx] = synthLPRM;
  }

  return;
}

/*
 * synthLPRM_initSynth
 */
void synthLPRM_initSynth(sleep_synth_lprm *synthLPRM)
{
  CORE_LOG_VERIFY_PTR( synthLPRM, sleepLog_printf(SLEEP_LOG_LEVEL_ERROR, 0,
                       "ERROR (message: \"NULL Synthesized LPRM\")" ) );

  /* Default is synthmode disabled */
  synthLPRM->mode_status = SLEEP_DISABLED;
  synthLPRM->attr = SynthLPRM_QueryCacheable(synthLPRM);
  synthLPRM_setupComponentModeParents(synthLPRM);

  return;
}

/*
 * synthLPRM_getLPRMLatency
 */
uint32 synthLPRM_getLPRMLatency(sleep_synth_lprm  *synthLPRM,
                                uint32            mLUTidx)
{
  CORE_VERIFY_PTR(synthLPRM);
  CORE_VERIFY(mLUTidx < SLEEP_NUM_STATIC_FREQ);

  /* Since enter & exit latencies are not dynamically adjusted,
   * always return the appropriate seed value */
  return synthLPRM->seed_latency->enter_exit[mLUTidx];
}

/*
 * synthLPRM_getBackOffTime
 */
uint32 synthLPRM_getBackOffTime(sleep_synth_lprm  *synthLPRM,
                                sleep_fLUT_node   *fLUT)
{
  uint64  workingLatency;

#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  uint32  dynIndex;
  uint32  core_num = CORE_CURR_CORE;

  CORE_VERIFY_PTR(synthLPRM);
  CORE_VERIFY(core_num < SLEEP_NUM_CORES);

  dynIndex = synthLPRM->dynamic_num;

  /* If the backoff adjustment feature is enabled, then get the backoff value
   * from the frequency dependant list */
  workingLatency = fLUT->dynamic_data.synth[core_num].LPRM[dynIndex].backoff;

#else

  CORE_VERIFY_PTR(synthLPRM);
  /* If the backoff feature is disabled, then get the value from the initial
   * seed value(s) given */
  workingLatency = synthLPRM->seed_latency->backoff[fLUT->mLUT_idx];

#endif

  return(workingLatency);
}

/*
 * synthLPRM_isCacheable
 */
boolean synthLPRM_isCacheable( sleep_synth_lprm  *synthLPRM )
{
  return ( ( synthLPRM->attr & SYNTH_MODE_ATTR_LPR_CACHEABLE ) ==
           SYNTH_MODE_ATTR_LPR_CACHEABLE );
}

/*
 * synthLPRM_enter
 */
void synthLPRM_enter(sleep_synth_lprm  *synthLPRM, 
                     uint64            wakeupTick, 
                     sleep_fLUT_node   *fLUT)
{
  sleep_lprm  *component;
  uint32      refCountAttr, sharedModeAttr;
  uint64      currentTime;
  int         i;
  uint64      LPRMTime   = CoreTimetick_Get64();
  
  CORE_VERIFY_PTR(synthLPRM);

  /* setup initial value */
  synthLPRM->wakeup_tick = wakeupTick;

  /* Iterating through each component mode */
  for(i = 0; i < synthLPRM->num_component_modes; i++)
  {
    /* Calling the actual enter functions of the component modes */
    component       = synthLPRM->component_modes[i];
    sharedModeAttr  = component->parent_LPR->sharing_cores &
                       (component->parent_LPR->sharing_cores - 1);
    refCountAttr    = component->attributes & SLEEP_MODE_ATTR_NO_REF_COUNT;
    currentTime     = 0;

    /* Actually entering the component mode based on condition */
    if((0 != refCountAttr) ||
       (0 == Core_AtomicDec((volatile uint32_t *)&component->cores_count)))
    {
      /* Sleep statistics */
      component->parent_LPR->run_count++;            /* Update LPR cycles*/
      component->mode_statistics.mode_run_cycles++;  /* Update LPRM cycles */
      
      if(0 == (component->attributes & SLEEP_MODE_ATTR_NO_FCN_LOCK))
      {
        sleepOS_LPRMFunctionLock(component->fn_lock);
      }

      /* Checks to avoid unnecessary calculations of sleep statistics. Sleep
       * will calculate stats if it is handling reference counting for the
       * shared mode (first condition) or the mode is local (second
       * condition). If LPR owner chooses to handle ref count for shared
       * mode(s) by himself, it is his responsibility to calculate mode
       * statistics */
      if((0 == refCountAttr) || (0 == sharedModeAttr))
      {
        component->mode_statistics.last_mode_time = LPRMTime;
      }

      /* Logging mode entry message */
      sleepLog_QDSSPrintf( SLEEP_LOG_LEVEL_PROFILING, 
                           SLEEP_ENTER_MODE_NUM_ARGS,
                           SLEEP_ENTER_MODE_STR, SLEEP_ENTER_MODE, 
                           component->parent_LPR->resource_name,
                           component->mode_name,
                           ULOG64_DATA(LPRMTime));

      /* Call the enter function with the latest sleep duration value */
      component->enter_func(synthLPRM->wakeup_tick);

      if(0 == (component->attributes & SLEEP_MODE_ATTR_NO_FCN_LOCK))
      {
        sleepOS_LPRMFunctionUnlock(component->fn_lock);
      }

      /* Get the end of the current LPRM exit, and also use this as the
       * beginning of the next LPRM exit so that multiple calls to timetick
       * are avoided. */
      currentTime = CoreTimetick_Get64();

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
      /* Only need to do statistic calculations if we are calling the enter
       * function in a non-cached mode */
      if(TRUE == sleepOS_isProcessorInSTM())
      {
        if(component->attributes & SLEEP_MODE_ATTR_NO_AUTO_LATENCY_TRACK)
        {
          LPRMTime = sleepStats_getLastLprLatency(SLEEP_STATS_ENTER_LATENCY_TYPE);
        }
        else
        {
          LPRMTime = currentTime - LPRMTime;
        }

        /* Update statistics for enter function */
        sleepStats_updateValue(
            &(fLUT->dynamic_data.sleep_LPRM[component->dynamic_num].enter), LPRMTime);
      }
#endif
    }

    /* To avoid multiple calls to get the current time use the point after
     * the exit function returns, which is close enough to the start of the
     * next LRPM exit for statistical purposes. */
    if(0 != currentTime)
    {
      LPRMTime = currentTime;
    }
    else
    {
      LPRMTime = CoreTimetick_Get64();
    }
  }

  return;
}

/*
 * synthLPRM_exit
 */
void synthLPRM_exit( sleep_synth_lprm *synthLPRM,
                     sleep_fLUT_node  *fLUT)
{
  int         index;
  sleep_lprm  *component;
  uint32      refCountAttr;
  uint32      sharedModeAttr;
  uint32      noAutoLat;
  uint64      currentTime;
  uint64      LPRMTime = CoreTimetick_Get64();
  
  CORE_VERIFY_PTR(synthLPRM);

  for(index = synthLPRM->num_component_modes - 1; index >=0; index--)
  {
    /* Calling the exit functions of the component modes */
    component       = synthLPRM->component_modes[index];
    sharedModeAttr  = component->parent_LPR->sharing_cores &
                       (component->parent_LPR->sharing_cores - 1);
    refCountAttr    = component->attributes & SLEEP_MODE_ATTR_NO_REF_COUNT;
    noAutoLat       = component->attributes & SLEEP_MODE_ATTR_NO_AUTO_LATENCY_TRACK;
    currentTime     = 0;

    sleepLog_QDSSPrintf( SLEEP_LOG_LEVEL_PROFILING, 
                         SLEEP_EXIT_MODE_NUM_ARGS,
                         SLEEP_EXIT_MODE_STR, SLEEP_EXIT_MODE, 
                         component->parent_LPR->resource_name,
                         component->mode_name,
                         ULOG64_DATA(LPRMTime));

    if((0 != refCountAttr) ||
       (1 == Core_AtomicInc((volatile uint32_t *)&component->cores_count)))
    {
      /* Actually exiting the component mode */
      if(0 == (component->attributes & SLEEP_MODE_ATTR_NO_FCN_LOCK))
      {
        sleepOS_LPRMFunctionLock(component->fn_lock);
      }
     
      /* Call LRPM exit function */
      component->exit_func();

      if(0 == (component->attributes & SLEEP_MODE_ATTR_NO_FCN_LOCK))
      {
        sleepOS_LPRMFunctionUnlock(component->fn_lock);
      }

      /* Get the end of the current LPRM exit, and also use this as the
       * beginning of the next LPRM exit so that multiple calls to timetick
       * are avoided. */
      currentTime = CoreTimetick_Get64();

      /* Only need to do statistic calculations if we are calling the exit
       * function in a non-cached mode */
      if(TRUE == sleepOS_isProcessorInSTM())
      {
        if(0 == noAutoLat)
        {
          LPRMTime = currentTime - LPRMTime;
        }
        else
        {
          LPRMTime = sleepStats_getLastLprLatency(SLEEP_STATS_BACKOFF_LATENCY_TYPE);
        }

        /* Record the backoff time for QR handling */
        sleepQR_setComponentCycleBackoff(component, LPRMTime, &fLUT->dynamic_data);

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
        /* Add backoff statistics */
        sleepStats_updateValue(
            &(fLUT->dynamic_data.sleep_LPRM[component->dynamic_num].backoff), LPRMTime);
#endif

        if( (0 == refCountAttr) || (0 == sharedModeAttr) ) 
        {
          /* calculate last time in mode stats */
          component->mode_statistics.last_mode_time = currentTime - 
            component->mode_statistics.last_mode_time;

          /* Keep running total of mode time */
          component->mode_statistics.in_mode_time += 
            component->mode_statistics.last_mode_time;
        }
      }
    }

    /* To avoid multiple calls to get the current time use the point after
     * the exit function returns, which is close enough to the start of the
     * next LRPM exit for statistical purposes. */
    if(0 != currentTime)
    {
      LPRMTime = currentTime;
    }
    else
    {
      LPRMTime = CoreTimetick_Get64();
    }
  }

  return;
}

/*
 * synthLPRM_update
 */
void synthLPRM_update( sleep_synth_lprm *synthLPRM,
                       sleep_lprm       *changedSleepLPRM )
{
  sleep_status oldStatus;
  
  CORE_VERIFY_PTR(synthLPRM);
  CORE_VERIFY_PTR(changedSleepLPRM);

  oldStatus = synthLPRM->mode_status;

  /* Update the number of enabled modes */
  if( TRUE == changedSleepLPRM->mode_enabled )
    synthLPRM->num_enabled_component_modes++;
  else
    synthLPRM->num_enabled_component_modes--;

  /* Make sure the number of enabled component modes is not out of range. */
  CORE_VERIFY( synthLPRM->num_enabled_component_modes >= 0 &&
               (synthLPRM->num_enabled_component_modes <= 
                synthLPRM->num_component_modes) );
  
  if(synthLPRM->num_enabled_component_modes == synthLPRM->num_component_modes)
  {
    synthLPRM->mode_status = SLEEP_ENABLED;
  }
  else
  {
    synthLPRM->mode_status = SLEEP_DISABLED;
  }

  /* If we changed state, check for a parent and update it if we have one */
  if(synthLPRM->mode_status != oldStatus)
  {
    synthLPR_modeUpdated(synthLPRM);
  }

  return;
}

/*
 * synthLPRM_setComponentModeAttributes
 */
void synthLPRM_setComponentModeAttributes( sleep_lprm *sleepLPRM )
{
  uint32 sharingCores;

  CORE_VERIFY_PTR(sleepLPRM);
  
  sharingCores = sleepLPRM->parent_LPR->sharing_cores;

  if( (1 == SLEEP_NUM_CORES) ||
      (0 == (sharingCores & (sharingCores-1)))  )
  {
    /* Single core environment or local mode */
    sleepLPRM->attributes |=
      (SLEEP_MODE_ATTR_NO_REF_COUNT | SLEEP_MODE_ATTR_NO_FCN_LOCK);
    sleepLPRM->cores_count = 1;
  }
  else
  {
    /* Multicore environment with non-local mode */
    if( 0 == (sleepLPRM->attributes & SLEEP_MODE_ATTR_NO_REF_COUNT) )
    {
      /* Sleep will handle reference count for the mode */
      if( sharingCores == SLEEP_ALL_CORES )
      {
        sleepLPRM->cores_count = SLEEP_NUM_CORES; /* shared on all cores */
      }
      else
      {
        unsigned int i;
        for(i = 0; i < SLEEP_NUM_CORES; i++)
        {
          if( sharingCores & SLEEP_CORE(i) )
          {
            sleepLPRM->cores_count++;
          }
        }
      }
    }
  }

  return;
}

