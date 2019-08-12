#ifndef SLEEP_LOG_H
#define SLEEP_LOG_H
/*============================================================================
  FILE:         sleep_log.h

  OVERVIEW:     This file declares the interface to the sleep module's log.

  DEPENDENCIES: None

                Copyright (c) 2010-2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
==============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/inc/sleep_log.h#1 $
$DateTime: 2016/12/13 07:59:23 $
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "ULogFront.h"

/*
===============================================================================
                             TYPE DEFINITION(S)
===============================================================================
*/

/**
 * @brief Enum for log level severity levels.
 *
 * Based on target we will have different logs for different levels. For
 *
 * Modem:
 * We will have one log for each level upto SLEEP_LOG_LEVEL_DEBUG (inclusive).
 * Since ERROR and WARNING are at same level, they share the log.
 *
 * RIVA and Othe low memory Targets:
 * They will have one shared log for ERROR and WARNING level, one for STATS
 * and one shared for all other levels upto DEBUG (inclusive).
 *
 * Apps:
 * Each core will have its own set of logs with one log for each level. But
 * -   ERROR and WARNING levels share a log on each core.
 * -   STATS and REQUESTS logs are shared across cores.
 *
 * Noraml Usage:
 * -   Resource registration will log at ERROR and WARNING level.
 * -   Requests to enable/disable low power modes will be logged at REQUESTS
 *     level.
 * -   Idle logging will be done at all levels but REQUESTS.
 */
typedef enum sleep_log_level
{
  SLEEP_LOG_LEVEL_ERROR = 0,
  SLEEP_LOG_LEVEL_WARNING = 0,
  SLEEP_LOG_LEVEL_REQUESTS,
  SLEEP_LOG_LEVEL_INFO,
  SLEEP_LOG_LEVEL_PROFILING,
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  SLEEP_LOG_LEVEL_AUTO_LATENCY,
#endif
  SLEEP_LOG_LEVEL_DEBUG,
  SLEEP_LOG_LEVEL_MAX = SLEEP_LOG_LEVEL_DEBUG,
  SLEEP_TOTAL_LOG_LEVELS
} sleep_log_level;

/*
===============================================================================

                           GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
 * sleep_log_init
 *
 * @brief Initializes the log system for sleep with all the logs being disabled
 *        initially.
 *
 * If user wants to enable a particular log level, he should call
 * sleep_log_set_level with valid sleep log level once logs are initialized.
 */
void sleep_log_init(void);

/**
 * sleep_log_deinit
 *
 * @brief Deinitializes (tears down) the log system for sleep.
 */
void sleep_log_deinit(void);

/**
 * sleep_log_get_level
 *
 * @brief Inquire the current level where sleep log is set.
 *
 * @return Current level of sleep log as listed in sleep_log_level enum
 */
sleep_log_level sleep_log_get_level(void);

/**
 * sleep_log_set_level
 *
 * @brief Changes the current sleep log level to the one passed as
 *        argument.
 *
 * After calling this function, all the sleep log levels upto the
 * 'level' will be enabled. As of now, once the log at a particular
 * level is enabled, it cannot be disabled.
 *
 * @param level: new level for the sleep log.
 */
void sleep_log_set_level(sleep_log_level level);

/**
 * sleep_log_printf
 *
 * @brief Generates the actual log using ULog system.
 *
 * @param level: Log level severity of current message
 * @param arg_count: Number of parameters that are being logged.
 * @param format: The format string for printing log.
 */
void sleep_log_printf(sleep_log_level level, int arg_count, const char *format, ...);

/**
 * sleep_log_printf_qdss
 *
 * @brief This function is used to log an event both to QDSS, 
 *        as well as to the sleep log indicated in the first
 *        parameter.
 *  
 *         NOTE that if QDSS is not enabled, sleep_log_printf
 *         will be called instead, to minimize timeline impact.
 *
 * @param level: Log level severity of current message
 * @param arg_count: Number of parameters that are being logged.
 * @param format: The format string for printing log.
 * @param tracer_id: QDSS tracer ID to use.
 */
#ifdef SLEEP_ENABLE_QDSS
#include "sleep_tracer_event_ids.h"
#include "tracer.h"
#define sleep_log_qdss_event( numArg, evntID, ... ) \
  do \
  { \
    tracer_event_simple_vargs( ( tracer_event_id_t )evntID, numArg, ## __VA_ARGS__ ); \
  } while (0)

#define sleep_log_printf_qdss( logLvl, numArg, str, evntID, ... ) \
  do \
  { \
    tracer_event_simple_vargs( ( tracer_event_id_t )evntID, numArg, ## __VA_ARGS__ ); \
    sleep_log_printf( logLvl, numArg, str, ## __VA_ARGS__ ); \
  } while (0)

#else /* !SLEEP_ENABLE_QDSS */

#define sleep_log_qdss_event(numArg, evntID, ...)
#define sleep_log_printf_qdss(logLvl, numArg, str, evntID, ...) \
 sleep_log_printf(logLvl, numArg, str, ## __VA_ARGS__ )

#endif /* !SLEEP_ENABLE_QDSS */

#ifdef __cplusplus
}
#endif

#endif /* SLEEP_LOG_H */
