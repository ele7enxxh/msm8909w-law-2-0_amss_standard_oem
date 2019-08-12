/*==============================================================================
  FILE:         sleep_statsi.c
  
  OVERVIEW:     Contains internal variables and functions for the implementation 
                of sleep mode statistics and dynamic latency adjustments

  DEPENDENCIES: None

                Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/statistics/sleep_statsi.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <stdint.h>
#include <stdlib.h>
#include "HALcomdef.h"
#include "sleepi.h"
#include "CLprm.h"
#include "sleep_stats.h"
#include "sleep_statsi.h"
#include "CoreVerify.h"
#include "sleep_log.h"
#include "lookup_table_solver.h"
#include "sleep_target.h"

/*=============================================================================
 *                     Macros & Types
 *============================================================================*/
#define PCYCLES_TO_US(pcycles, fkhz) (pcycles*1000/fkhz)

/** 
 * power_collapse_overhead
 * 
 * @brief This struct contains OS enter and exit overhead for previous
 *        PC operation
 */
typedef struct
{
  sleepStats_generic  enter;
  sleepStats_generic  exit;
}power_collapse_overhead;

/* ==================================================================
                  FORWARD DECLARATIONS
   ==================================================================*/
int64 safe_signed_addition(int64 a, int64 b);
int32 safe_signed_truncate(int64 a);

/* ==================================================================
                  GLOBAL EXTERNAL VARIABLES
   ==================================================================*/
extern uint32 g_postedLastLatency[SLEEP_STATS_NUM_LATENCY_TYPE];
extern uint64 g_postedTimeData[SLEEP_STATS_NUM_TIMEDATA_TYPE];
extern uint32 g_postedMiscData[SLEEP_STATS_NUM_MISC_TYPE];

/* ==================================================================
                  VARIABLE DECLARATIONS
   ==================================================================*/
static RPM_master_stats         *g_sleepStatsRPM = NULL;

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
static power_collapse_overhead  *g_pcOverhead[L2_NUM_RET_MODES];
#endif

/* ==================================================================
                  INTERNAL FUNCTION DEFINITIONS
   ==================================================================*/

/*
 * sleepStats_init
 */
void sleepStats_init( void )
{
#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  uint32 i;

  for(i=0; i<L2_NUM_RET_MODES; i++)
    g_pcOverhead[i] = NULL;
#endif

  /* Get RPM statistics memory area pointer */
  g_sleepStatsRPM = (RPM_master_stats *)sleep_target_get_rpm_statistics_addr();

  return;
}

/*
 * sleepStats_getRpmDataPtr
 */
RPM_master_stats *sleepStats_getRpmDataPtr(void)
{
  return(g_sleepStatsRPM);
}

/*
 * sleepStats_addSynthWakeupElem
 */
static void sleepStats_addSynthWakeupElem(CLprm *hCLprm)
{
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  sleepStats_synth  *wakeStats;
  uint32            last_idx = hCLprm->num_data_elements - 1;
  
  CORE_VERIFY_PTR(hCLprm->wakeup_stats = 
                  realloc(hCLprm->wakeup_stats,
                          sizeof(sleepStats_synth) * 
                          hCLprm->num_data_elements));

  wakeStats = &((sleepStats_synth *)hCLprm->wakeup_stats)[last_idx];

  wakeStats->total      = 0;
  wakeStats->count      = 0;
  wakeStats->min        = INT32_MAX;
  wakeStats->max        = INT32_MIN;
  wakeStats->adjustCnt  = SLEEPSTATS_INITIAL_CYCLE_COUNT;
#endif

  return;
}

/*
 * sleepStats_addPowerCollapseOverheadElem
 */
void sleepStats_addPowerCollapseOverheadElem(void)
{
#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  static uint32 nPCOverhead = 0;
  uint32 nMode;
  uint32 last_idx = nPCOverhead;

  nPCOverhead++;

  for(nMode=0; nMode<L2_NUM_RET_MODES; nMode++)
  {
    CORE_VERIFY_PTR(g_pcOverhead[nMode] =
                    realloc(g_pcOverhead[nMode],
                            sizeof(power_collapse_overhead) * nPCOverhead));

    memset(&g_pcOverhead[nMode][last_idx], 0, 
           sizeof(power_collapse_overhead)); 
    g_pcOverhead[nMode][last_idx].enter.min  = UINT64_MAX;
    g_pcOverhead[nMode][last_idx].exit.min   = UINT64_MAX;
  }

#endif

  return;
}

/*
 * sleepStats_addStatsElem
 */
void sleepStats_addStatsElem(CLprm *hCLprm)
{
  sleep_lprm  *lprm;
  uint32      i;

  /* Add statistic elements */
  sleepStats_addSynthWakeupElem(hCLprm);

  /* Iterating through each component mode */
  for( i = 0; i < hCLprm->num_expected_component_modes; i++ )
  {
    lprm = hCLprm->m_componentModes[i];

    if(lprm->num_stats_elements < hCLprm->num_data_elements)
    {
      uint32 idx = lprm->num_stats_elements;

      lprm->num_stats_elements++;

      CORE_VERIFY_PTR(lprm->mode_cycles_exact =
                      realloc(lprm->mode_cycles_exact,
                              sizeof(uint32) * lprm->num_stats_elements));

      lprm->mode_cycles_exact[idx] = 0;

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
      CORE_VERIFY_PTR(lprm->mode_stats_enter =
                      realloc(lprm->mode_stats_enter,
                              sizeof(sleepStats_lprm) * 
                              lprm->num_stats_elements));

      CORE_VERIFY_PTR(lprm->mode_stats_backoff =
                      realloc(lprm->mode_stats_backoff,
                              sizeof(sleepStats_lprm) *
                              lprm->num_stats_elements));

      ((sleepStats_lprm *)lprm->mode_stats_enter)[idx].running_time  = 0;
      ((sleepStats_lprm *)lprm->mode_stats_enter)[idx].max_lat       = 0;
      ((sleepStats_lprm *)lprm->mode_stats_enter)[idx].min_lat       = -1;

      ((sleepStats_lprm *)lprm->mode_stats_backoff)[idx].running_time  = 0;
      ((sleepStats_lprm *)lprm->mode_stats_backoff)[idx].max_lat       = 0;
      ((sleepStats_lprm *)lprm->mode_stats_backoff)[idx].min_lat       = -1;
#endif
    }
  }
  return;
}

/*
 * sleepStats_initLprmStatData
 */
void sleepStats_initLprmStatData( CLprm *hCLprm )
{
  uint32      nMode;
  sleep_lprm  *lprm;

  CORE_VERIFY_PTR(hCLprm);

#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  hCLprm->wakeup_stats = NULL;
#endif

  /* Iterating through each component mode */
  for(nMode = 0; nMode < hCLprm->num_expected_component_modes; nMode++ )
  {
    lprm = hCLprm->m_componentModes[nMode];

    lprm->num_stats_elements          = 0;
    lprm->mode_cycles_exact           = NULL;
    lprm->cycleStats.mode_run_cycles  = 0;
    lprm->cycleStats.in_mode_time     = 0;
  }

  return;
}

#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
/*
 * sleepStats_adjustSynthmode
 */
void sleepStats_adjustSynthmode(sleepStats_input *statInput,
                                int64            wakeupDelta,
                                int64            wakeupAdjust)
{
  sleep_freq_lookup_table *pfreq_LUT = statInput->pfreq_LUT;

  /* Only update statistics if the synth mode doesn't have a custom backoff
   * latency function LPRM */
  if(statInput->mode->cached_backoff_latency != NULL) 
  {
    int32             tmpCompare;
    sleepStats_synth  *timeStatP;

    timeStatP = 
       &((sleepStats_synth *)statInput->mode->wakeup_stats)[pfreq_LUT->data_idx];

    /* Remove any wakeup latency errors so only the masters local latency is
     * adjusted
     */
    wakeupDelta -= wakeupAdjust;
    tmpCompare = safe_signed_truncate(wakeupDelta);

    timeStatP->count++;
    timeStatP->total = safe_signed_addition(timeStatP->total, wakeupDelta);

    if(tmpCompare > timeStatP->max)
      timeStatP->max = tmpCompare;

    if(tmpCompare < timeStatP->min)
      timeStatP->min = tmpCompare;
  
    /* Adjust synth mode latency if it's time to do so */
    if(timeStatP->count == timeStatP->adjustCnt)
    {
      int64 actualAvg  = (timeStatP->total / (int64)timeStatP->count);
      uint32 *cBackoffP = 
        &statInput->mode->cached_backoff_latency[pfreq_LUT->data_idx];
      uint32 *cExitP = 
        &statInput->mode->cached_exit_latency[pfreq_LUT->data_idx];

      sleep_log_printf_qdss(SLEEP_LOG_LEVEL_AUTO_LATENCY, 
                            SLEEP_BKOFF_STATS_NUM_ARGS,
                            SLEEP_BKOFF_STATS_STR, 
                            SLEEP_BKOFF_STATS,
                        statInput->mode->lprmName,
                        pfreq_LUT->frequency,
                        timeStatP->min,
                        timeStatP->max,
                        ULOG64_DATA(timeStatP->total),
                        timeStatP->count,
                            ULOG64_DATA(actualAvg));

      /* Sanity checking */
      if((actualAvg > INT32_MIN) && (actualAvg < INT32_MAX))
      {
        /* Want to keep synth mode slightly early due to statistic gathering
         * when exiting sleep */
        if((actualAvg < -250) || (actualAvg > -50))
        {
          int32 adjustment;
          int32 newBackoff;

          adjustment = (int32)(actualAvg + 150);
          newBackoff = *cBackoffP + adjustment;
          
          /* Make sure new latency value is something reasonable:
           *  - Less than 16mS
           *  - Greater than [at least a few ticks] */
          if((newBackoff < US_TO_TICKS(16000)) && (newBackoff > 32))
          {
            sleep_log_printf( SLEEP_LOG_LEVEL_AUTO_LATENCY, 3 + (1 * 2),
                              " Adjusting (Wakeup Error: %lld) "
                              "(Current Backoff: %d) (Adjustment: %d) "
                              "(New Backoff: %d)",
                              ULOG64_DATA(wakeupAdjust),
                              *cBackoffP,
                              adjustment,
                              newBackoff);

            *cBackoffP = (uint32)newBackoff;

            /* Ensure exit latency is always >= than backoff */
            if( *cBackoffP > *cExitP )
            {
              *cExitP = *cBackoffP;
              sleep_log_printf( SLEEP_LOG_LEVEL_AUTO_LATENCY, 0,
                                "  Increasing exit latency to current backoff");
            }
          }
          else
          {
            sleep_log_printf( SLEEP_LOG_LEVEL_AUTO_LATENCY, 3 + (1 * 2),
                              " Warning: New backoff out of range"
                              " (Wakeup Error: %lld) (Cur Backoff: %d)"
                              " (Adjust: %d) (NewBackoff %d)",
                              ULOG64_DATA(wakeupAdjust), 
                              *cBackoffP,
                              adjustment,
                              newBackoff);
          }
        }
        else
        {
          sleep_log_printf( SLEEP_LOG_LEVEL_AUTO_LATENCY, 1 + (1 * 2),
                            " No adjustment needed (Current: %d) (avg: %lld)",
                            *cBackoffP,
                            ULOG64_DATA(actualAvg));
        }

        /* Increase next adjust time but limit to maximum number of cycles  */
        if(timeStatP->adjustCnt < SLEEPSTATS_FINAL_CYCLE_COUNT)
          timeStatP->adjustCnt = (timeStatP->adjustCnt << 1);
        else
          timeStatP->adjustCnt = SLEEPSTATS_FINAL_CYCLE_COUNT;
      }
      else
      {
        sleep_log_printf( SLEEP_LOG_LEVEL_AUTO_LATENCY, 0,
                          " Adjustment failed sanity check");
      }

      /* reset min/max stats for next run */
      timeStatP->min    = INT32_MAX;
      timeStatP->max    = INT32_MIN;
      timeStatP->total  = 0;
      timeStatP->count  = 0;
    }
  }
  return;
}
#endif /* SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT */

/*
 * sleepStats_logWakeup
 */
void sleepStats_logWakeup(sleepStats_input *statInput)
{
  int64                   wakeup_delta;
  int64                   wakeup_adjust;
  const char              *wake_reason;
  sleep_freq_lookup_table *pfreq_LUT      = statInput->pfreq_LUT;
  boolean                 is_rpm_assisted = FALSE;
  RPM_master_stats        *rpmStats       = sleepStats_getRpmDataPtr();
  
  CORE_VERIFY_PTR(rpmStats);

  switch(statInput->master_wakeup_reason)
  {
    case SLEEP_STATS_WAKEUP_RPM_SCHEDULED:
    {
      is_rpm_assisted = TRUE;
    }

    case SLEEP_STATS_WAKEUP_SA_SCHEDULED:
    {
      wake_reason = "Timer";
      break;
    }

    case SLEEP_STATS_WAKEUP_RPM_UNSCHEDULED:
    {
      is_rpm_assisted = TRUE;
    }

    case SLEEP_STATS_WAKEUP_SA_UNSCHEDULED:
    {
      wake_reason = "Rude";
      break;
    }

    case SLEEP_STATS_WAKEUP_RPM_UNKNOWN:
    {
      is_rpm_assisted = TRUE;
    }

    default:
    {
      wake_reason = "Unknown";
    }
  }

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  if((is_rpm_assisted == TRUE || 
      SLEEP_STATS_WAKEUP_SA_SCHEDULED == statInput->master_wakeup_reason))
  {
    /* Exit overhead is actual wakeup to first point in master code */
    uint64 exit_time =
      sleepStats_getLprTimeData(SLEEP_STATS_TIME_MSTR_RETURN_TYPE) -
       statInput->actual_wakeup_time;

    uint32 idx = pfreq_LUT->data_idx;

    l2_lprm pc_type = 
      (l2_lprm)sleepStats_getMiscData(SLEEP_STATS_MISC_PC_L2_MODE_TYPE);

    sleepStats_updateGeneric(&g_pcOverhead[pc_type][idx].exit, exit_time);

    /* Can only compute PC enter overhead if RPM assisted */
    if(is_rpm_assisted == TRUE)
    {
      /* Enter overhead is last point in master code to actual shutdown */
      uint64 enter_time = inp64(&rpmStats->shutdown_req) -
        sleepStats_getLprTimeData(SLEEP_STATS_TIME_MSTR_SHUTDOWN_TYPE);

      sleepStats_updateGeneric(&g_pcOverhead[pc_type][idx].enter, enter_time);
    }
  }
#endif

  /* Get inaccuracy in the master wakeup so only the local latency
   * is adjusted later */
  if( statInput->actual_wakeup_time >= statInput->backoff_deadline )
  {
    /* RPM or idle timer woke up the master late */
    wakeup_adjust = (int64)(statInput->actual_wakeup_time - 
                              statInput->backoff_deadline); 
  }
  else
  {
    /* RPM or idle timer woke up the master early */
    wakeup_adjust = -((int64)(statInput->backoff_deadline - 
                                statInput->actual_wakeup_time));
  }
  
  if(statInput->os_overhead.count > 0)
  {
    /* Log OS bringup times */
    sleep_log_printf(SLEEP_LOG_LEVEL_PROFILING, 1 + (2*2),
                     "OS stats (Count: %d) "
                     "(Enter: %llduS) (Exit: %llduS) ",
                     statInput->os_overhead.count,
                     ULOG64_DATA(PCYCLES_TO_US(statInput->os_overhead.enter,
                                               pfreq_LUT->frequency)),
                     ULOG64_DATA(PCYCLES_TO_US(statInput->os_overhead.exit,
                                               pfreq_LUT->frequency)) );
  }

  if(is_rpm_assisted == TRUE)
  {
    /* Log RPM bringup info */
    sleep_log_printf(SLEEP_LOG_LEVEL_PROFILING, 2 + (3*2),
                     "RPM stats (wakeup_ind: 0x%llx) (bringup_req: 0x%llx) "
                     "(bringup_ack: 0x%llx) (sleep trans time: %d) "
                     "(wake trans time: %d)",
                     ULOG64_DATA(inp64(&rpmStats->wakeup_ind)),
                     ULOG64_DATA(inp64(&rpmStats->bringup_req)),
                     ULOG64_DATA(inp64(&rpmStats->bringup_ack)),
                     rpmStats->last_sleep_transition_duration,
                     rpmStats->last_wake_transition_duration );
  }
  
  /* Log master wakeup */
  sleep_log_printf_qdss( 
    SLEEP_LOG_LEVEL_INFO, SLEEP_WAKEUP_NUM_ARGS,
    SLEEP_WAKEUP_STR, SLEEP_WAKEUP, 
    wake_reason, 
    statInput->master_interrupt,
    ULOG64_DATA(statInput->actual_wakeup_time),
    ULOG64_DATA(statInput->backoff_deadline),
    ULOG64_DATA(wakeup_adjust) ); 

  /* Log late sleep exits */
  if( statInput->sleep_exit_stm > statInput->hard_deadline )
  {
    /* Get difference of sleep exit time to expected exit */
    wakeup_delta = statInput->sleep_exit_stm - statInput->hard_deadline;
    
    sleep_log_printf( SLEEP_LOG_LEVEL_WARNING, 4*2,
                      "WARNING (message: \"Late sleep exit\") "
                      "(Actual: 0x%llx) (Expected: 0x%llx) "
                      "(diff ticks: %lld) (diff us: %lld)",
                      ULOG64_DATA(statInput->sleep_exit_stm), 
                      ULOG64_DATA(statInput->hard_deadline),
                      ULOG64_DATA(wakeup_delta),
                      ULOG64_DATA(TICKS_TO_US(wakeup_delta)));
  }
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  else
  {
    /* Early wake ups are negative values */
    wakeup_delta = -(statInput->hard_deadline - statInput->sleep_exit_stm);
  }

  /* Record stats and make any adjustments only for RPM assisted or scheduled
   * stand alone PC modes.
   * Since we don't have an accurate wakeup time for
   * unscheduled standalone mode, we're unable to adjust latencies for that
   * mode*/ 
  if(is_rpm_assisted == TRUE ||
     SLEEP_STATS_WAKEUP_SA_SCHEDULED == statInput->master_wakeup_reason )
  {
    sleepStats_adjustSynthmode(statInput, wakeup_delta, wakeup_adjust);
  }

#endif /* SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT */

  return;
}

/*
 * sleepStats_getLastLprLatency
 */
uint32 sleepStats_getLastLprLatency(sleepStats_latency_type type)
{
  uint32 tmp;

  CORE_VERIFY(type < SLEEP_STATS_NUM_LATENCY_TYPE);

  tmp = g_postedLastLatency[type];

  /* Verify an LPR put a valid value */
  CORE_VERIFY(tmp != 0);
  g_postedLastLatency[type] = 0;

  return(tmp);
}

/*
 * sleepStats_getLprTimeData
 */
uint64 sleepStats_getLprTimeData(sleepStats_timedata_type type)
{
  uint64 tmp;

  CORE_VERIFY(type < SLEEP_STATS_NUM_TIMEDATA_TYPE);

  tmp = g_postedTimeData[type];
  g_postedTimeData[type] = 0;
  
  return(tmp);
}

/*
 * sleepStats_getMiscData
 */
uint32 sleepStats_getMiscData(sleepStats_misc_type type)
{
  CORE_VERIFY(type < SLEEP_STATS_NUM_MISC_TYPE);
  return(g_postedMiscData[type]);
}
