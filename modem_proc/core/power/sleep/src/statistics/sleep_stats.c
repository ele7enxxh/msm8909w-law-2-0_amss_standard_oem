/*==============================================================================
  FILE:         sleep_stats.c
  
  OVERVIEW:     Contains variables and internal functions implementation of
                sleep mode statistics and dynamic latency adjustments
                
                Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/statistics/sleep_stats.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include <stdlib.h>
#include <string.h>
#include "CoreVerify.h"
#include "sleep_lpr.h"
#include "sleep_lpri.h"
#include "sleep_stats.h"
#include "SleepLPR_lookup_table.h"

/* ==================================================================
                      FORWARD DECLARATIONS
   ==================================================================*/
uint64 safe_unsigned_addition(uint64 a, uint64 b);

/* ==================================================================
                        GLOBAL VARIABLES
   ==================================================================*/
/**
 * @brief g_postedLastLatency
 *
 * Variable for the last latency value of the last LPR
 */
uint32 g_postedLastLatency[SLEEP_STATS_NUM_LATENCY_TYPE] = {0};

/**
 * @brief g_postedTimeData
 *
 * Variable for the timestamp values of certain LPR events
 */
uint64 g_postedTimeData[SLEEP_STATS_NUM_TIMEDATA_TYPE] = {0};

/**
 * @brief g_postedMiscData
 *
 * Variable for miscellaneous values of certain LPR events
 */
uint32 g_postedMiscData[SLEEP_STATS_NUM_MISC_TYPE] = {0};

/* ==================================================================
                  GLOBAL FUNCTION DEFINITIONS
   ==================================================================*/

/*
 * sleepStats_putLprLatency
 */
void sleepStats_putLprLatency(uint32 value, sleepStats_latency_type type)
{
  CORE_VERIFY(type < SLEEP_STATS_NUM_LATENCY_TYPE);
  
  /* Verify CLprm_enter & exit functions have retreived last value before
   * putting a new one */
  CORE_VERIFY(g_postedLastLatency[type] == 0);
  g_postedLastLatency[type] = value;
  return;
}

/*
 * sleepStats_putLprTimeData
 */
void sleepStats_putLprTimeData(uint64 value, sleepStats_timedata_type type)
{
  CORE_VERIFY(type < SLEEP_STATS_NUM_TIMEDATA_TYPE);
  g_postedTimeData[type] = value;
  return;
}

/*
 * sleepStats_putMiscData
 */
void sleepStats_putMiscData(uint32 value, sleepStats_misc_type type)
{
  CORE_VERIFY(type < SLEEP_STATS_NUM_MISC_TYPE);
  g_postedMiscData[type] = value;
  return;
}

/*
 * sleepStats_updateGeneric
 */
void sleepStats_updateGeneric(sleepStats_generic *stats, uint64 value)
{
  stats->count++;
  stats->total = safe_unsigned_addition(stats->total, value);

  if(value > stats->max)
    stats->max = value;

  if(value < stats->min)
    stats->min = value;

  return;
}

/*
 * sleepStats_get_lpr_handle
 */
sleepStats_lpr_handle sleepStats_get_lpr_handle(const char *lprName)
{
  sleepStats_lpr_handle result = 0;
  uint32                lprCnt = sizeof(SleepLPR_LookupTable) / 
                                  sizeof(SleepLPR_LookupTable[0]);
  uint32                i;

  CORE_VERIFY_PTR(lprName);

  /* Loop through table entries for LPR name match */
  for(i=0; i<lprCnt; i++)
  {
    /* Table entry will be null terminated */
    uint32 lprStrLen = strlen(SleepLPR_LookupTable[i].pName);

    /* Check for LPR name match */
    if((strncmp(lprName, SleepLPR_LookupTable[i].pName, lprStrLen) == 0) &&
       (lprName[lprStrLen] == '\0'))
    {
      sleep_lpr *pLPR = SleepLPR_LookupTable[i].pLPR;
      result          = (sleepStats_lpr_handle)pLPR;
      break;
    }
  }

  return result;
}

/*
 * sleepStats_get_lpr_stats
 */
void sleepStats_get_lpr_stats(sleepStats_lpr_handle lprHandle,
                              sleepStats_mode_stats *modeStats)
{
  sleep_lpr *lpr = (sleep_lpr *)lprHandle;

  CORE_VERIFY_PTR(lpr);

  /* lprHandle is currenlty a pointer to the LPR data structure which does not
   * currently contain information about the mode times. 
   * Set unused stats to 0 */ 
  modeStats->in_mode_time     = 0;
  modeStats->last_mode_time   = 0;

  /* Set LPR cycle count */
  modeStats->mode_run_cycles  = lpr->run_count;

  return;
}

void sleepStats_trackPowerCollapseTime(uint8 enable)
{
  return;
}

const sleepStats_mode_stats *sleepStats_getPowerCollapseTime(void) 
{
  return NULL;
}
