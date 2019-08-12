/*==============================================================================
  FILE:         sleep_qr.c
  
  OVERVIEW:     Contains internal variables and functions for the implementation 
                of sleep Quick Response

  DEPENDENCIES: None

                Copyright (c) 2014-2015 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/statistics/sleep_qr.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <stdlib.h>
#include "DALStdDef.h"
#include "sleep_qr.h"
#include "synthTypes.h"
#include "lookup_table_types.h"
#include "sleep_statsi.h"
#include "sleep_log.h"
#include "synthLPRM.h"
#include "synthRegistry.h"

/* Generate compiler error if settings are not correct */
#if !defined(SLEEP_QR_LATE_SENSITIVITY) || (SLEEP_QR_LATE_SENSITIVITY >= SLEEPSTATS_INITIAL_CYCLE_COUNT)
  #error "SLEEP_QR_LATE_SENSITIVITY must be defined and less than SLEEPSTATS_INITIAL_CYCLE_COUNT"
#endif

#if (0 != SLEEP_QR_LATE_SENSITIVITY)
  #if (SLEEP_QR_COMPONENT_THRESHOLD_USEC >= SLEEP_QR_ACTIVE_THRESHOLD_USEC)
    #error "SLEEP_QR_COMPONENT_THRESHOLD_USEC must be less than SLEEP_QR_ACTIVE_THRESHOLD_USEC" 
  #endif
#endif

/*==============================================================================
                        INTERNAL MACRO DEFINITIONS
 =============================================================================*/
/* Returns TRUE if int32 value is negative, FALSE otherwise */
#define IS_NEGATIVE(value) ((value < 0) ? TRUE : FALSE)

/*==============================================================================
                        INTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
#if (0 != SLEEP_QR_LATE_SENSITIVITY)
/**
 * sleepQR_updateSynthmodeBackoff
 *  
 * @brief Updates the Synthmode backoff based off of QR event data
 *  
 * @param synthMode:  Synthmode to adjust
 * @param pfreq_LUT:  Frequecy table data that was used in the last sleep cycle 
 * @param delat:      SynthMode error.  If 0, the synthmode backoff will be
 *                    calculated based on it's components
 */
static void sleepQR_updateSynthmodeBackoff(sleep_synth_lprm *synthMode,
                                           sleep_fLUT_node  *fLUT,
                                           int32            delta)
{
  uint32                  seedEnterExitLatency;
  int64                   newBackoff;
  uint32                  modeIdx;
  uint32                  dynNum;
  uint32                  componentBackoff;
  sleepStats_dynamic_data *dynData            = &fLUT->dynamic_data;
  uint32                  synthDynNum         = synthMode->dynamic_num;
  uint32                  synthCoreNum        = synthMode->synth_LPR_parent->core_num;
  synth_LPRM_dyn_data     *synthLPRMDynData   = &dynData->synth[synthCoreNum].LPRM[synthDynNum];
  uint32                  *currentBackoff     = &synthLPRMDynData->backoff;

  /* Skip if this Synthmode has already been updated unless this is the synthmode
   * that caused the event or this QR event is in the opposite direction from the 
   * last adjusted event. */
  if(0 == delta)
  {
    if(synthLPRMDynData->QR_data.QR_performed > SLEEP_QR_PERFORMED_NONE)
    {
      sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 1,
                      "  QR: Skipping adjustment "
                      "(Reason: Already adjusted) "
                      "(Mode: %s)",
                      synthMode->name);

      return;
    }

    newBackoff = 0;

    /* Loop through component modes to compute new synthmode backoff*/
    for(modeIdx = 0; modeIdx < synthMode->num_component_modes; modeIdx++)
    {
      dynNum            = synthMode->component_modes[modeIdx]->dynamic_num;
      componentBackoff  = dynData->sleep_LPRM[dynNum].QR_data.current_backoff;

      if(0 != componentBackoff)
      {
        newBackoff += componentBackoff;
      }
      else
      {
        synthLPRMDynData->QR_data.QR_performed = SLEEP_QR_PERFORMED_COMPONENT_SKIP;

        sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 1,
                        "  QR: Skipping adjustment "
                        "(Reason: Component data incomplete) "
                        "(Mode: %s)",
                        synthMode->name);
        return;
      }
    }
  }
  else
  {
    /* Compute backoff for triggered synthmode. */
    newBackoff = *currentBackoff + delta;
  }

#ifdef SLEEP_ENABLE_FREQUENCY_SCALING
  seedEnterExitLatency = synthMode->seed_latency->enter_exit[fLUT->mLUT_idx];
#else
  seedEnterExitLatency = synthMode->seed_latency->enter_exit[0];
#endif

  /* Validate the new backoff and take the appropriate action */
  sleepStats_verifyBackoffRange(*currentBackoff, (uint32)newBackoff, 
                                seedEnterExitLatency);

  /* Indicate a triggered synthmode has been adjusted. */
  if(0 != delta)
  {
    synthLPRMDynData->QR_data.QR_performed = SLEEP_QR_PERFORMED_TRIGGER_ADJUST;
  }
  else
  {
    synthLPRMDynData->QR_data.QR_performed = SLEEP_QR_PERFORMED_SYNTH_ADJUST;
  }

  sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 2,
                  "  QR: Synth adjustment: " 
                  "(Current backoff: %d) "
                  "(New backoff: %d)",
                  *currentBackoff,
                  (uint32)newBackoff);

  /* Update the synthmode backoff */
  *currentBackoff = (uint32)newBackoff;

  return;
}

#if (0 != SLEEP_QR_COMPONENT_THRESHOLD_USEC)
/**
 * sleepQR_resetAllSynths
 * 
 * @brief Resets statistical and QR data fields in the given synth LPR
 *  
 * @param synthLPR: Function will reset all the synth LPRMs in this synth LPR
 * @param fLUT:     Associated fLUT entry
 */
static void sleepQR_resetAllSynths(sleep_synth_lpr *synthLPR,
                                   sleep_fLUT_node *fLUT)
{
  uint32 synthIdx;
  uint32                  synthDynNum;
  uint32                  synthCoreNum;
  synth_LPRM_dyn_data     *synthLPRMDynData;
  sleep_synth_lprm        *synthLPRM;
  sleepStats_dynamic_data *dynData = &fLUT->dynamic_data;

  /* Reset Synthmode statistics for those that were adjusted and start over */
  synthCoreNum = synthLPR->core_num;

  /* Walk through each of the synthmodes modes */
  for(synthIdx=0; synthIdx < synthLPR->mode_count; synthIdx++) 
  {
    synthLPRM           = &synthLPR->modes[synthIdx];
    synthDynNum         = synthLPRM->dynamic_num;
    synthLPRMDynData    = &dynData->synth[synthCoreNum].LPRM[synthDynNum];
    
    /* Check for parent adjustments that were made on this event */
    if(synthLPRMDynData->QR_data.QR_performed > SLEEP_QR_PERFORMED_NONE) 
    {
      /* Reset synthmode */
      sleepStats_resetSynth(synthLPRMDynData);

      /* All adjusted modes start auto-backoff over */
      synthLPRMDynData->adjust_cnt = SLEEPSTATS_INITIAL_CYCLE_COUNT; 
    }
  }

  return;
}

/**
 * sleepQR_adjustAllSynths
 * 
 * @brief Adjusts synthmode backoff times if a the synthmode has been marked to 
 *        have member component mode backoff change
 *  
 * @param synthLPR: Function will check all the synth LPRMs in this synth LPR
 * @param dynData:  Dynamic data array associated with this synthmode
 */
static void sleepQR_adjustAllSynths(sleep_synth_lpr *synthLPR,
                                    sleep_fLUT_node *fLUT)

{
  uint32                  synthIdx;
  uint32                  synthDynNum;
  uint32                  synthCoreNum;
  synth_LPRM_dyn_data     *synthLPRMDynData;
  sleep_synth_lprm        *synthLPRM;
  sleepStats_dynamic_data *dynData = &fLUT->dynamic_data;

  CORE_VERIFY_PTR(synthLPR);

  synthCoreNum = synthLPR->core_num;

  /* Walk through each of the synthmodes modes */
  for(synthIdx=0; synthIdx < synthLPR->mode_count; synthIdx++) 
  {
    synthLPRM           = &synthLPR->modes[synthIdx];
    synthDynNum         = synthLPRM->dynamic_num;
    synthLPRMDynData    = &dynData->synth[synthCoreNum].LPRM[synthDynNum];

    /* Check if parent mode has had a component adjustment */
    if(TRUE == synthLPRMDynData->QR_data.component_adjusted)
    {
      sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 1,
                      " QR: Synth reset "
                      "(Name: %s)",
                      synthLPRM->name);

      sleepQR_updateSynthmodeBackoff(synthLPRM, fLUT, 0);

      /* Reset the synth flag */
      synthLPRMDynData->QR_data.component_adjusted = FALSE;
    }
  }

  return;
}

/**
 * sleepQR_componentCaused
 * 
 * @brief Determines if a component mode caused the change in the synthmode
 *        wakeup error.
 *
 * @param sleepLPRM:        Component mode to check.
 * @param QRComponentStats: Component mode QR data for given sleepLPRM.
 * 
 * @return  >0 -> Component exceeded threshold and contributed this amount 
 *                of late wakeup error
 *          0  -> Component did not contribute to wakeup error
 */
static uint32 sleepQR_componentCaused(sleep_lprm             *sleepLPRM,
                                      sleepQR_componentMode  *QRComponentStats)
{
  int64   componentDelta;

  /* If current backoff has not been initialized yet, we can't determine if it
   * contributed. */ 
  if(0 == QRComponentStats->current_backoff)
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 2,
                    "  QR: Component data incomplete, skipping "
                    "(Mode: %s.%s)",
                    sleepLPRM->parent_LPR->resource_name,
                    sleepLPRM->mode_name);

    componentDelta = 0;
  }
  else
  {
    /* Check if component contributed to synth mode error. */
    sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 6,
                    "  QR: Checking component "
                    "(Mode: %s.%s) "
                    "(Max: %d) "
                    "(Min: %d) "
                    "(Current: %d) "
                    "(Last: %d)",
                    sleepLPRM->parent_LPR->resource_name,
                    sleepLPRM->mode_name,
                    QRComponentStats->max_cycle_backoff,
                    QRComponentStats->min_cycle_backoff,
                    QRComponentStats->current_backoff,
                    QRComponentStats->last_cycle_backoff);

    componentDelta = ((int64)QRComponentStats->max_cycle_backoff - 
                      (int64)QRComponentStats->current_backoff);

    /* Check if component passed the threshold to say it contributed to the
     * synth error */
    if(componentDelta < (int64)US_TO_TICKS(SLEEP_QR_COMPONENT_THRESHOLD_USEC))
    {
      /* Component did not contribute */
      componentDelta = 0;
    }
  }

  /* Set flag to monitor component backoff's */
  QRComponentStats->monitor_backoff = TRUE;

  return (uint32)componentDelta;
}
#endif /* #if (0 != SLEEP_QR_COMPONENT_THRESHOLD_USEC) */

/**
 * sleepQR_adjustLatency
 *  
 * @brief Performs the adjustments required for a QR event. 
 *  
 * @param synthMode: Synthmode that caused the QR event. 
 * @param fLUT:      Frequecy table data that was used in the last sleep cycle.
 * @param delta:     Wakeup error  
 */
static void sleepQR_adjustLatency(sleep_synth_lprm  *synthMode,
                                  sleep_fLUT_node   *fLUT,
                                  int32             delta)
{
#if (0 != SLEEP_QR_COMPONENT_THRESHOLD_USEC)
  uint32                  parentIdx;
  uint32                  numParents;
  uint32                  modeIdx;
  uint32                  componentDelta;
  sleepQR_componentMode   *QRComponentStats;
  sleep_lprm              *sleepLPRM;
  boolean                 needToAdjust  = FALSE;
  uint32                  numMode       = synthMode->num_component_modes;
#endif
  uint32                  synthDynNum;
  synth_LPRM_dyn_data     *synthLPRMDynData;
  sleep_synth_lprm        *synthParent;
  uint32                  synthCoreNum  = synthMode->synth_LPR_parent->core_num;
  sleepStats_dynamic_data *dynData      = &fLUT->dynamic_data;

  /* This should never happen, catch if it does */
  CORE_VERIFY(0 != delta);

  /* Updated the Synthmode that triggered the QR event */
  sleepQR_updateSynthmodeBackoff(synthMode, fLUT, delta);

#if (0 != SLEEP_QR_COMPONENT_THRESHOLD_USEC)
  /* Search for component modes that contributed to the QR adjustement, and 
   * adjust all parent Synthmodes */
  for(modeIdx = 0; modeIdx < numMode; modeIdx++)
  {
    sleepLPRM         = synthMode->component_modes[modeIdx];
    QRComponentStats  = &dynData->sleep_LPRM[sleepLPRM->dynamic_num].QR_data;

    /* Check if this component caused synthmode wakeup error */
    componentDelta = sleepQR_componentCaused(sleepLPRM, QRComponentStats);

    /* Continue to next component if this one didn't contribute */
    if(0 == componentDelta)
    {
      continue;
    }

    /* Immedialy set the component current backoff to the maximum QR cycle value
     * found */ 
    QRComponentStats->current_backoff = QRComponentStats->max_cycle_backoff;
    
    numParents = sleepLPRM->synth_LPRM_parent_count;

    /* Mark all the parents of this component so they can be adjusted */
    for(parentIdx = 0; parentIdx < numParents; parentIdx++)
    {
      synthParent       = ((sleep_synth_lprm **)sleepLPRM->synth_LPRM_parent_array)[parentIdx];
      synthDynNum       = synthParent->dynamic_num;
      synthLPRMDynData  = &dynData->synth[synthCoreNum].LPRM[synthDynNum];

      /* Mark that this synth needs to be adjusted due to a component contribution */ 
      synthLPRMDynData->QR_data.component_adjusted = TRUE;
      needToAdjust = TRUE;
    }
  }

  if(TRUE == needToAdjust)
  {
    sleepQR_adjustAllSynths(synthMode->synth_LPR_parent, fLUT);
  }
#endif /* #if (0 != SLEEP_QR_COMPONENT_THRESHOLD_USEC) */

  sleepQR_resetAllSynths(synthMode->synth_LPR_parent, fLUT);

  return;
}
#endif /* (0 != SLEEP_QR_LATE_SENSITIVITY) */

/*==============================================================================
                        EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * sleepQR_perform
 */
void sleepQR_perform(sleep_synth_lprm *synthMode,
                     sleep_fLUT_node  *fLUT,
                     int32            wakeupDelta)
{
#if (0 != SLEEP_QR_LATE_SENSITIVITY)
  uint32                  modeIdx;
  uint32                  dynNum;
  sleepQR_componentMode   *QRComponentStats;
  sleep_lprm              *sleepLPRM;
  sleepStats_dynamic_data *dynData;
  uint32                  numMode;
  uint32                  synthCoreNum;
  uint32                  synthDynNum;
  sleepQR_synthMode       *synthQRData;

  CORE_VERIFY_PTR(fLUT);
  CORE_VERIFY_PTR(synthMode);

  dynData       = &fLUT->dynamic_data;
  numMode       = synthMode->num_component_modes;
  synthCoreNum  = synthMode->synth_LPR_parent->core_num;
  synthDynNum   = synthMode->dynamic_num;
  synthQRData   = &dynData->synth[synthCoreNum].LPRM[synthDynNum].QR_data;

  /* Update component mode data */
  for(modeIdx = 0; modeIdx < numMode; modeIdx++)
  {
    sleepLPRM         = synthMode->component_modes[modeIdx];
    dynNum            = sleepLPRM->dynamic_num;
    QRComponentStats  = &dynData->sleep_LPRM[dynNum].QR_data;

    if(TRUE == QRComponentStats->monitor_backoff)
    {
      QRComponentStats->cycle_count++;
      QRComponentStats->total_backoff += QRComponentStats->last_cycle_backoff;
    }
  }

  /* Check if the current sleep exit delta trips the late threshold for QR. */
  if(wakeupDelta > (int32)(US_TO_TICKS(SLEEP_QR_ACTIVE_THRESHOLD_USEC)))
  {
    /* If this is the first QR event, set intial values */
    if(synthQRData->last_event_cnt == 0)
    {
      synthQRData->last_delta = wakeupDelta;

      /* Set initial min/max cycle values */
      for(modeIdx = 0; modeIdx < numMode; modeIdx++)
      {
        sleepLPRM         = synthMode->component_modes[modeIdx];
        dynNum            = sleepLPRM->dynamic_num;
        QRComponentStats  = &dynData->sleep_LPRM[dynNum].QR_data;

        QRComponentStats->max_cycle_backoff = QRComponentStats->last_cycle_backoff;
        QRComponentStats->min_cycle_backoff = QRComponentStats->last_cycle_backoff;
      }
    }
    else
    {
      /* Store max delta value which will be used to update backoff */
      synthQRData->last_delta = MAX(wakeupDelta, synthQRData->last_delta);

      /* Record component mode max & min backoff times */
      for(modeIdx = 0; modeIdx < numMode; modeIdx++)
      {
        sleepLPRM         = synthMode->component_modes[modeIdx];
        dynNum            = sleepLPRM->dynamic_num;
        QRComponentStats  = &dynData->sleep_LPRM[dynNum].QR_data;

        QRComponentStats->max_cycle_backoff = MAX(QRComponentStats->max_cycle_backoff, 
                                                  QRComponentStats->last_cycle_backoff);

        QRComponentStats->min_cycle_backoff = MIN(QRComponentStats->min_cycle_backoff, 
                                                  QRComponentStats->last_cycle_backoff);
      }
    }

    /* Count the number of times we were late in a row */
    synthQRData->last_event_cnt++;

    sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 4,
                    "QR: Triggered " 
                    "(Frequency: %d) "
                    "(Delta: %d) "
                    "(Count: %d) "
                    "(Mode: %s)", 
                    fLUT->frequency,
                    wakeupDelta,
                    synthQRData->last_event_cnt,
                    synthMode->name);

    /* Check for the number of times above or below the threshold. */
    if(synthQRData->last_event_cnt >= SLEEP_QR_LATE_SENSITIVITY) 
    {
      /* QR has been activated, update the backoff latencies. */
      sleepQR_adjustLatency(synthMode, fLUT, synthQRData->last_delta);

      synthQRData->last_event_cnt = 0; 
    }
  }
  else
  {
    /* Last cycle was good, reset count */
    synthQRData->last_event_cnt = 0;
  }

#endif /* feature is disabled */

  return;
}

/*
 * sleepQR_reset
 */
void sleepQR_reset(sleepQR_synthMode *QRData)
{
  /* Clear the event counts */
  QRData->last_event_cnt  = 0;
  QRData->QR_performed    = SLEEP_QR_PERFORMED_NONE;
  return;
}

/*
 * sleepQR_resetComponents
 */
void sleepQR_resetComponents(sleep_synth_lprm  *synthMode,
                             sleep_fLUT_node   *fLUT)
{
#if (0 != SLEEP_QR_LATE_SENSITIVITY) && (0 != SLEEP_QR_COMPONENT_THRESHOLD_USEC)
  uint32                  modeIdx;
  sleep_lprm              *componentLPRM;
  sleepQR_componentMode   *QRComponentData;
  uint32                  newBackoff;
  int32                   backoffDiff;
  sleepStats_dynamic_data *dynData            = &fLUT->dynamic_data;
  boolean                 performSynthAdjust  = FALSE;
  
  /* Reset the components of the given synthMode */
  for(modeIdx=0; modeIdx < synthMode->num_component_modes; modeIdx++) 
  {
    componentLPRM   = synthMode->component_modes[modeIdx];
    QRComponentData = &dynData->sleep_LPRM[componentLPRM->dynamic_num].QR_data;

    if(TRUE == QRComponentData->monitor_backoff)
    {
      /* Set current backoff to average of last measurment period */
      newBackoff = QRComponentData->total_backoff /
                     QRComponentData->cycle_count;
      
      backoffDiff = (int32)((int64)newBackoff - 
                            (int64)QRComponentData->current_backoff);

      /* If measured component latency is greater than a fraction of the
       * component threadhold, then update the current latency, otherwise, the
       * change in latency isn't worth the time to update. */ 
      if(ABS(backoffDiff) > (US_TO_TICKS(SLEEP_QR_COMPONENT_THRESHOLD_USEC) / 2)) 
      {
        sleep_synth_lprm      *parentSynth;
        uint32                synthDynNum;
        uint32                synthCoreNum;
        synth_LPRM_dyn_data   *synthLPRMDynData;
        uint32                parentIdx;

        sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 4,
                        "QR: Reset component "
                        "(Mode: %s.%s) "
                        "(Old backoff: %d) "
                        "(New backoff: %d)",
                        componentLPRM->parent_LPR->resource_name,
                        componentLPRM->mode_name,
                        QRComponentData->current_backoff,
                        newBackoff);

        /* Update component backoff */
        QRComponentData->current_backoff = newBackoff;

        /* Set the component adjusted flag on the parent synthmodes */
        for(parentIdx = 0; parentIdx < componentLPRM->synth_LPRM_parent_count; parentIdx++) 
        {
          parentSynth       = componentLPRM->synth_LPRM_parent_array[parentIdx];
          synthDynNum       = parentSynth->dynamic_num;
          synthCoreNum      = parentSynth->synth_LPR_parent->core_num;
          synthLPRMDynData  = &dynData->synth[synthCoreNum].LPRM[synthDynNum];

          /* Don't adjust the synthmode that was just auto-backoff adjusted */
          if(synthMode != parentSynth)
          {
            /* Indicate the the synthmode parent needs to be adjusted due to
             * a component backoff adjustment */
            synthLPRMDynData->QR_data.component_adjusted = TRUE;

            /* At least 1 synth needs to be updated */
            performSynthAdjust = TRUE;
          }
        }
      }
      else if(0 == QRComponentData->current_backoff)
      {
        sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 3,
                        "QR: Set initial component backoff "
                        "(Mode: %s.%s) "
                        "(Backoff: %d)",
                        componentLPRM->parent_LPR->resource_name,
                        componentLPRM->mode_name,
                        newBackoff);

        QRComponentData->current_backoff = newBackoff;
      }

      /* Reset counts for next run */
      QRComponentData->cycle_count    = 0;
      QRComponentData->total_backoff  = 0;
    }
  }

  if(TRUE == performSynthAdjust)
  {
    sleepQR_adjustAllSynths(synthMode->synth_LPR_parent, fLUT);
    sleepQR_resetAllSynths(synthMode->synth_LPR_parent, fLUT);
  }
#endif

  return;
}

/*
 * sleepQR_setComponentCycleBackoff
 */
void sleepQR_setComponentCycleBackoff(sleep_lprm              *sleepLPRM,
                                      uint32                  backoff,
                                      sleepStats_dynamic_data *dynData)
{
  uint32 dynNum = sleepLPRM->dynamic_num;

  dynData->sleep_LPRM[dynNum].QR_data.last_cycle_backoff = backoff;

  return;
}

/*
 * sleepQR_initElement
 */
void sleepQR_initElement(sleepStats_dynamic_data *dynData)
{
  uint32 nCore, nMode;

  for(nCore = 0; nCore < SLEEP_NUM_CORES; nCore++)
  {
    synth_LPRM_dyn_data *synthLPRMDynData = dynData->synth[nCore].LPRM;

    for(nMode = 0; nMode < SLEEP_LPR_NUM_SYNTHESIZED_MODES; nMode++)
    {
      /* Default is to monitor components so we get an initial seed value
       * of their latencies. */
      memset(&synthLPRMDynData[nMode].QR_data, 0, sizeof(sleepQR_synthMode));
    }
  }

  for(nMode = 0; nMode < SLEEP_NUM_TOTAL_LPRMS; nMode++)
  {
    sleep_LPRM_dyn_data *sleepLPRMDynDataP = &dynData->sleep_LPRM[nMode];
    memset(&sleepLPRMDynDataP->QR_data, 0, sizeof(sleepQR_componentMode));

    sleepLPRMDynDataP->QR_data.monitor_backoff = TRUE;
  }

  return;
}

/*
 * sleepQR_clearComponentMonitoring
 */
void sleepQR_clearComponentMonitoring(sleep_synth_lprm        *synthMode,
                                      sleepStats_dynamic_data *dynData)
{
  uint32                modeIdx;
  sleep_lprm            *lprm;
  sleepQR_componentMode *QRComponentStats;
 
  /* Reset the component QR backoff monitor flag. */
  for(modeIdx=0; modeIdx < synthMode->num_component_modes; modeIdx++) 
  {
    lprm              = synthMode->component_modes[modeIdx];
    QRComponentStats  = &dynData->sleep_LPRM[lprm->dynamic_num].QR_data;

    QRComponentStats->monitor_backoff = FALSE; 
  }

  return;
}

