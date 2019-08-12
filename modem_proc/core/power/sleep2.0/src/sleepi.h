#ifndef SLEEPI_H
#define SLEEPI_H
/*==============================================================================
  FILE:         sleepi.h
  
  OVERVIEW:     This file provides the global macro's and types needed internal
                to the sleep module.

  DEPENDENCIES: None

                Copyright (c) 2010-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/sleepi.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "DALStdDef.h"
#include "CoreVerify.h"
#include "sleep_lpr.h"
#include "CoreConfig.h"
#include "CoreTime.h"
#include "sleep_log.h"

/*==============================================================================
                            MACRO DEFINITIONS
 =============================================================================*/
/**
 * @brief SLEEP_TICK_PER_US
 *
 * Macro defining the number of QTimer Ticks per usec.
 */
#define SLEEP_TICK_PER_US (19.2)

/**
 * @brief TICKS_TO_SCLK64
 *
 * Convert tick unit (19.2 MHz) values into sclk unit values (32768 Hz) values  
 */
#define TICKS_TO_SCLK64(sclks) ((uint64)((uint64)(sclks) / (uint64)586))

/**
 * @brief SCLK_TO_TICKS64
 *
 * Convert sclk unit (32768 Hz) values into tick unit values (19.2 MHz) values  
 */
#define SCLK_TO_TICKS64(sclks) ((uint64)((uint64)(sclks) * (uint64)586))

/**
 * @brief TICKS_TO_US
 *
 * Convert tick unit (19.2 MHz) values into us time
 */
#define TICKS_TO_US(ticks)    ((uint32)((ticks)/SLEEP_TICK_PER_US))

/**
 * @brief US_TO_TICKS
 *
 * Macro to convert usec to 19.2Mhz QTimer Tick.
 */
#define US_TO_TICKS(usec) ((uint32)((usec)*SLEEP_TICK_PER_US))

/**
 * @brief ABS
 *
 * Returns absolute value of the argument
 */
#ifndef ABS
#define  ABS( x ) ( ((x) < 0) ? -(x) : (x) )
#endif /* ABS */

/**
 * @brief MAX
 *
 * Returns the greater value of the two arguments 'a' and 'b' 
 */
#ifndef MAX
#define MAX( a, b ) ((a)>(b)) ? (a) : (b)
#endif

/**
 * @brief boolean
 *
 * Type def for for boolean (True or False) values.
 */
#ifdef WINSIM
typedef unsigned char boolean;
#endif

/**
 * @brief SLEEP_QUERY_HARD_WAKEUP_TIME
 *
 * Macro to use when querying the wakeup node for hard wakeup time settings.
 */
#define SLEEP_QUERY_HARD_WAKEUP_TIME (NPA_QUERY_RESERVED_END+2)

/**
 * @brief SLEEP_QUERY_LOCAL_MIN_LATENCY
 *
 * Query id to use while querying for core specific minimum latency.
 */
#define SLEEP_QUERY_LOCAL_MIN_LATENCY (NPA_QUERY_RESERVED_END+3)

/**
 * @brief SLEEP_QUERY_MIN_LATENCY
 *
 * Query id to use while querying for minimum between global latency
 * resource and local latency resource.
 */
#define SLEEP_QUERY_MIN_LATENCY (NPA_QUERY_RESERVED_END+4)

/**
 * SLEEP_MIN_MATCH_VAL_DELTA 
 *  
 * @brief This is the minimum amount of time that the timer HW 
 *        requires us to set the timetick into the future, in
 *        order for the timer interrupt to expire.  This means
 *        that we must be able to sleep for at least this much
 *        time if we want to do a backoff.  Otherwise, the
 *        backoff match value will be set to expire after the
 *        actual timer expiration was supposed to occur.
 */
#define SLEEP_MIN_MATCH_VAL_DELTA  10

/**
 * @brief SLEEP_LATENCY_NAME_LENGTH
 *
 * Defines the maximum allowable length for each latency npa resources name.
 */
#define SLEEP_LATENCY_NAME_LENGTH 32

#if !defined(SLEEP_INLINE)
#if defined(WINSIM) || defined(__ARMCC_VERSION)
#define SLEEP_INLINE __inline
#elif defined(__clang__)
#define SLEEP_INLINE
#else
#define SLEEP_INLINE inline
#endif
#endif /* SLEEP_INLINE */

/*==============================================================================
                            TYPE DEFINITIONS
 =============================================================================*/
/**
 * sleep_status enum
 *
 * @brief This enum lists all the states a mode can be in. 
 *
 * Details of each status is as follows.
 *   SLEEP_ENABLED:    The mode is enabled and hence can be entered
 *   SLEEP_DISABLED:   The mode is disabled and cannot be entered. 
 *   SLEEP_IMPOSSIBLE: The mode can never be in enabled state and so it won't
 *                     be entered ever.
 *
 * The mode can switch between SLEEP_ENABLED and SLEEP_DIABLED but once it goes
 * into SLEEP_IMPOSSIBLE state, it cannot change from there.
 */
typedef enum sleep_status
{
  SLEEP_ENABLED,
  SLEEP_DISABLED,
  SLEEP_IMPOSSIBLE
} sleep_status;

/*==============================================================================
                         GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * @brief sleepNPA_initialize
 * 
 * Initialize the sleep NPA subsystem.  This function registers the latency
 * and wakeup nodes that are used by the sleep task with the NPA framework.
 */
void sleepNPA_initialize( void );

/**
 * @brief sleepLPR_initialize
 * 
 * Initialize the sleep LPR subsystem.  This function registers the sleep
 * LPR node with the NPA framework.
 */
void sleepLPR_initialize( void );

/**
 * sleepOS_getHardWakeupTime
 * 
 * @brief Returns the amount of time until the next timer expiration. This is used
 *        to further decide on which low-power mode(s) can be entered.
 *  
 * @return Absolute time (in ticks) of the next scheduled wakeup. 
 *         This will be the lesser of the first non-deferrable, the NPA
 *         deadline or the first non-deferrable island mode timer (if supported).
 */ 
uint64 sleepOS_getHardWakeupTime(void);

/**
 * @brief sleepOS_getSoftWakeupTime
 * 
 * Returns the amount of time until the next wakeup as hinted by the client.
 * This hint is used to select appropriate sleep modes so that we would not 
 * enter any mode(s) from which we might be required to exit soon after
 * entering.
 * 
 * @return Absolute time (in ticks) of the next wakeup hinted by the client. 
 *         Calculated as min(wakeup_node.state, max_duration_node.state)
 */
uint64 sleepOS_getSoftWakeupTime(void);

/**
 * @brief sleepOS_createConfigHandle
 * 
 * Creates a CoreConfigHandle, to be used when reading the sleep 
 * configuration data.  The mechanism for reading the config 
 * data varies per OS. 
 * 
 * @param core_num: Core to read the config data for. 
 *  
 * @return NULL if an error occurred.  Otherwise, a valid 
 *         CoreConfigHandle is returned.
 */
CoreConfigHandle sleepOS_createConfigHandle( unsigned int core_num );

/**
 * @brief sleepOS_destroyConfigHandle
 * 
 * Destroys a CoreConfigHandle.  Closes any file descriptors 
 * that might be open, and frees the memory that was allocated 
 * for the object. 
 * 
 * @param config: Handle to the CoreConfig object to destroy.
 */
void sleepOS_destroyConfigHandle( CoreConfigHandle config );

/**
 * @brief sleepOS_createLPRMFunctionLock
 *
 * Creates a lock object and assigns its handle to the mode for 
 * serialization of its enter and exit functions.
 *
 * @Note
 * The lock create here must not yield since it is going be used
 * in idle path.
 *
 * @param mode: Mode to which newly created lock will be assigned
 */
void sleepOS_createLPRMFunctionLock( sleep_lprm *mode );
 
/**
 * @brief sleepOS_LPRMFunctionLock
 *
 * Obtains the mode's lock in order to avoid overlapping execution of
 * mode's enter and exit function in multicore environment.
 *
 * @see sleepOS_LPRMFunctionUnlock
 *
 * @Note 
 * Improper use of these lock and unlock functions can lead to uncertain 
 * behavior based on underlying lock implementation. 
 *
 * @param mode_lock: Lock object of the mode for which we want to 
 *                   serialize enter and exit function calls.
 */
void sleepOS_LPRMFunctionLock( sleep_mode_fn_lock mode_lock );

/**
 * @brief sleepOS_LPRMFunctionUnlock
 *
 * Releases mode's lock grabbed using sleep_fn_lock.
 *
 * @see sleepOS_LPRMFunctionLock
 *
 * @param mode_lock: Lock object of the mode for which we want to 
 *                   serialize enter and exit function calls.
 */
void sleepOS_LPRMFunctionUnlock( sleep_mode_fn_lock mode_lock );

/**
 * @brief sleepOS_getLastTimerDeadline
 * 
 * Returns the last deadline (in absolute ticks) that was used for the
 * hard wakeup for timers.  This is used by the NPA scheduler code to determine
 * whether the NPA scheduler deadline is the limiting factor in sleep, or
 * whether it's the timer deadline.
 * 
 * @return The last timer deadline (in absolute ticks).
 *
*/
uint64 sleepOS_getLastTimerDeadline( void );

/**
 * @brief sleepOS_offsetSleepDuration
 * 
 * Adds the offset passed in to the currently-used sleep duration.  This is
 * called by the NPA scheduler LPR, to adjust the sleep duration that's passed
 * to the LPRMs, after adjusting the NPA scheduler deadline.
 * 
 * @param offset: Amount to offset the sleep duration by, in ticks.
 *
*/
void sleepOS_offsetSleepDuration( uint64 offset );

#ifdef __cplusplus
}
#endif

#endif /* SLEEPI_H */

