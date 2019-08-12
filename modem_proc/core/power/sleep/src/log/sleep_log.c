/*==============================================================================
  FILE:         sleep_log.c
  
  OVERVIEW:     This file implements the sleep module's log interface.

  DEPENDENCIES: None

                Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/log/sleep_log.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/

#include <stdarg.h>
#include <string.h>
#include "DALStdDef.h"
#include "ULog.h"
#include "ULogFront.h"
#include "CoreVerify.h"
#include "CoreString.h"
#include "sleep_lpr.h"
#include "sleep_lpri.h"
#include "sleep_log.h"
#include "sleep_log_internal.h"


/* ==================================================================
                          TYPE DEFINITIONS
   ==================================================================*/
typedef enum sleep_features
{
  SLEEP_FEATURE_NONE              = 0x0000,
  SLEEP_FEATURE_QDSS              = 0x0001,
  SLEEP_FEATURE_LPR_PROFILING     = 0x0002,
  SLEEP_FEATURE_SYNTH_BACKOFF_ADJ = 0x0004,
  SLEEP_FEATURE_FREQUENCY_SCALING = 0x0008,
  SLEEP_FEATURE_DISABLE_MODES     = 0x0010
}sleep_features;

/* ==================================================================
                          VARIABLE DECLARATIONS
   ==================================================================*/
static volatile sleep_features gSleepFeaturesEnabled = SLEEP_FEATURE_NONE;

/* ==================================================================
                          INTERNAL DEFINITIONS
   ==================================================================*/

/**
 * @brief sleepLogLevel
 *
 * Current sleep log level. It must be noted that in multicore environment
 * each core will have same level of logging.
 */
static sleep_log_level sleepLogLevel;

/**
 * @brief sleepEnabledLogMask
 *
 * Contains the set bit for the sleep logs that have been enabled (currently
 * limited to total of 32 logs. Will need a different data type in case of 
 * more logs).
 */
static uint32 sleepEnabledLogMask = 0;

/**
 * SLEEP_LOG_IS_VALID_LEVEL
 *
 * @brief Macro to ensure that the passed log level is valid. If the level is
 *        valid, the condition will be TRUE else FALSE.
 *
 * @param level: sleep_log_level we want to verify.
 */
#define SLEEP_LOG_IS_VALID_LEVEL(level) (level < SLEEP_TOTAL_LOG_LEVELS)

/* ==================================================================
                       GLOBAL FUNCTION DEFINITIONS
   ==================================================================*/

/*
 * sleep_log_init
 */
void sleep_log_init(void)
{
  uint32 i;

  #define MAX_BUF_LEN 80

  /* ULog Header substring. */
  char uLogBaseHeader[MAX_BUF_LEN];

  /* Preparing handle lookup table/map if any. */
  sleep_log_initialize_target_config();
  
  for(i = 0; i < sleepNumAvailLogs ; i++)
  {
    if(!sleepLogHandles[i])
    {
      /* All the modes are disabled initially */
      CORE_VERIFY(ULOG_ERR_INITINCOMPLETE == 
                            ULogFront_RealTimeInit(&sleepLogHandles[i],
                            sleepLogNames[i],
                            0,
                            ULOG_MEMORY_LOCAL,
                            sleepLogLocks[i]));

      /* Preparing ULog Header messages */
      core_snprintf(uLogBaseHeader, MAX_BUF_LEN, 
                     "Content-Type: text/dyn-sleep-log-1.0; title=:%s",
                     sleepLogNames[i]);

      /* Setting ULog header */
      ULogCore_HeaderSet(sleepLogHandles[i], uLogBaseHeader);
    }
  }
}

/**
 * sleep_log_deinit
 *
 * @brief Deinitializes (Tears down) the logging for sleep and sets the logging
 *        handle to NULL.
 */
void sleep_log_deinit(void)
{
  uint32 i;
  for(i = 0; i < sleepNumAvailLogs ; i++)
  {
    /* FIXME: ULog currently doesn't have a way to delete a log. */
    sleepLogHandles[i] = NULL;
  }
}

/*
 * sleep_log_get_level
 */
sleep_log_level sleep_log_get_level(void)
{
  return sleepLogLevel;
}

/**
 * sleep_log_set_level
 *
 * @brief Sets the current sleep log level to a valid input sleep log level.
 *
 * In case of invalid input log level, current level won't be changed.
 *
 * @param level Level where we want to set sleep log at.
 */
void sleep_log_set_level(sleep_log_level level)
{
  sleep_log_level i;
  int             j;         /* Loop Counters */
  int             index;     /* Index to Log Handles */

  if(!SLEEP_LOG_IS_VALID_LEVEL(level))
    return;

  for(i = (sleep_log_level)0; i <= level; i++)
  {
    for(j = 0; j < SLEEP_NUM_CORES; j++)
    {
      index = sleep_log_handle_lookup(j, i);
      
      if( !(sleepEnabledLogMask & (1 << index)) )
      {
        /* Allocating the required log memory */
        ULogCore_Allocate(sleepLogHandles[index], sleepLogSize[index]);

        /* Enabling the disabled log */
        ULogCore_Enable(sleepLogHandles[index]);

        sleepEnabledLogMask |= (1 << index);
      }
    }
  }

  sleepLogLevel = level;

  /* Set internal sleep options */
#ifdef SLEEP_ENABLE_QDSS
  gSleepFeaturesEnabled |= SLEEP_FEATURE_QDSS;
#endif

#ifdef SLEEP_ENABLE_AUTO_LPR_PROFILING
  gSleepFeaturesEnabled |= SLEEP_FEATURE_LPR_PROFILING;
#endif

#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  gSleepFeaturesEnabled |= SLEEP_FEATURE_SYNTH_BACKOFF_ADJ;
#endif

#ifdef SLEEP_ENABLE_FREQUENCY_SCALING
  gSleepFeaturesEnabled |= SLEEP_FEATURE_FREQUENCY_SCALING;
#endif

  /* Set feature options*/
#ifdef FEATURE_DISABLE_SLEEP_MODES
  gSleepFeaturesEnabled |= SLEEP_FEATURE_DISABLE_MODES;
#endif
}

/**
 * sleep_log_printf
 *
 * @brief Actual function that will log the messages.
 *
 * @param level: Level at which want to log the message.
 * @param arg_count: Number of parameteres that are being logged.
 * @param format: Format string for logging function.
 */
void sleep_log_printf(sleep_log_level level, int arg_count, 
                      const char *format, ...)
{
  va_list args;
  int index;                 /* Index to log handle */

  if(!SLEEP_LOG_IS_VALID_LEVEL(level))
    return;

  index = sleep_log_handle_lookup(CORE_CURR_CORE, level);
  va_start(args, format);
  ULogFront_RealTimeVprintf(sleepLogHandles[index], arg_count, format, args);
  va_end(args);
}
