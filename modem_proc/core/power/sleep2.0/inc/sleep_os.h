#ifndef SLEEP_OS_H
#define SLEEP_OS_H
/*==============================================================================
  FILE:           sleep_os.h

  OVERVIEW:       This file contains declarations of functions that are used
                  in the common main sleep subsystem

  DEPENDENCIES: None

                Copyright (c) 2013-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/inc/sleep_os.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "DALStdDef.h"
#include "npa.h"

/*==============================================================================
                                MACRO DEFINITIONS
 =============================================================================*/
/**
 * @brief SLEEP_QUERY_WAKEUP_TIME
 *
 * Macro to use when querying the wakeup node for soft (hint of) wakeup time
 * settings.
 */
#define SLEEP_QUERY_WAKEUP_TIME      (NPA_QUERY_RESERVED_END+1)

/**
 * @brief SLEEP_QUERY_ABS_WAKEUP_TIME
 *
 * Macro that is the same as SLEEP_QUERY_WAKEUP_TIME, except it returns the
 * absolute wakeup time. 
 */
#define SLEEP_QUERY_ABS_WAKEUP_TIME   (NPA_QUERY_RESERVED_END+2)

/**
 * @brief SLEEP_QUERY_MAX_DURATION
 *
 * Macro to use when querying the max duration node for max duration
 * settings.
 */
#define SLEEP_QUERY_MAX_DURATION     (NPA_QUERY_RESERVED_END+5)

/*==============================================================================
                           GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * sleepOS_getHardDeadline 
 *  
 * @brief Returns the hard deadline that the sleep task must be 
 *        awake by, in absolute ticks.  This is the time that
 *        sleep expects to exit by.
 *  
 *        NOTE:  It only makes sense to call this function from
 *               within the idle context (i.e., from within an
 *               LPR).
 *  
 * @return The hard deadline, in absolute ticks, that sleep 
 *         expects to exit by.
 */
uint64 sleepOS_getHardDeadline(void);

/**
 * sleepOS_getSoftDeadline 
 *  
 * @brief Returns the soft deadline that the sleep task must use
 *        as a hint of when the next wakeup might occur, in absolute ticks.
 *
 *        NOTE:  It only makes sense to call this function from
 *               within the idle context (i.e., from within an
 *               LPR).
 *  
 * @return The soft deadline, in absolute ticks, that sleep is hinted of the
 *         next wakeup.
 */
uint64 sleepOS_getSoftDeadline(void);

#endif /* SLEEP_OS_H */
