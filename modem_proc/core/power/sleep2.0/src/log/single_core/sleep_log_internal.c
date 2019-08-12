/*==============================================================================
  FILE:         sleep_log_internal.c
  
  OVERVIEW:     Contains variables and internal functions implementation of
                sleep logs for single core environment.
  
  DEPENDENCIES: None
  
  NOTE:         Sleep log implementation for multi core can work for single
                core as well provided number of cores defined as one. However,
                it has some additional overhead in terms of memory and log
                handle lookup.
  
                Copyright (c) 2011-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/log/single_core/sleep_log_internal.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "sleep_log.h"
#include "sleep_log_internal.h"

/*==============================================================================
                          INTERNAL MACRO DEFINITIONS
 =============================================================================*/
/* Macros indicating size of various sleep logs in bytes */
#define SLEEP_ERR_WARNING_LOG_SIZE     256
#define SLEEP_REQUESTS_LOG_SIZE        4096
#define SLEEP_INFO_LOG_SIZE            8192
#define SLEEP_PROFILE_LOG_SIZE         8192
#define SLEEP_DEBUG_LOG_SIZE           4096
#define SLEEP_AUTO_LATENCY_LOG_SIZE    4096

/* Number of available logs */
const uint32  g_sleepNumberOfLogs = SLEEP_TOTAL_LOG_LEVELS;

/* Array of log handles */
ULogHandle    g_sleepLogHandles[SLEEP_TOTAL_LOG_LEVELS] = { NULL };

/* Size array for the logs */
uint32 g_sleepLogSizeArray[SLEEP_TOTAL_LOG_LEVELS] = 
{
  SLEEP_ERR_WARNING_LOG_SIZE,
  SLEEP_REQUESTS_LOG_SIZE,
  SLEEP_INFO_LOG_SIZE,
  SLEEP_PROFILE_LOG_SIZE,
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  SLEEP_AUTO_LATENCY_LOG_SIZE,
#endif
  SLEEP_DEBUG_LOG_SIZE
};

/* Names of the logs */
const char *g_sleepLogNameArray[SLEEP_TOTAL_LOG_LEVELS] = 
{
  "Sleep WarningError",
  "Sleep Requests",
  "Sleep Info",
  "Sleep Profiling",
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  "Sleep Auto Latency",
#endif
  "Sleep Debug",
};

/* Lock types for the logs */
ULOG_LOCK_TYPE g_sleepLogLockTypeArray[SLEEP_TOTAL_LOG_LEVELS] = 
{
  ULOG_LOCK_OS,
  ULOG_LOCK_OS,
  ULOG_LOCK_OS,
  ULOG_LOCK_OS,
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  ULOG_LOCK_OS,
#endif
  ULOG_LOCK_OS
};

/*==============================================================================
                              INTERNAL FUNCTIONS
 =============================================================================*/
/*
 * sleepLog_initializeTargetCfg
 */
void sleepLog_initializeTargetCfg(void)
{
  return;
}

/*
 * sleepLog_lookup
 */
uint32 sleepLog_lookup
(
  uint32          core_num,
  sleep_log_level level
)
{
  return level;
}

