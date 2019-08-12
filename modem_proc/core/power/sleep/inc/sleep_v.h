#ifndef SLEEP_V_H
#define SLEEP_V_H
/*==============================================================================
  FILE:           sleep_v.h

  OVERVIEW:       This file contains declarations of functions that are used
                  initialize sleep subsystem and select low power mode during
                  idle time.

  DEPENDENCIES: None

                Copyright (c) 2010-2015 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/inc/sleep_v.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "DALStdDef.h"

/*
  ===============================================================================
                                MACRO DEFINITIONS
  ===============================================================================
*/

/**
 * @brief SLEEP_QUERY_WAKEUP_TIME
 *
 * Macro to use when querying the wakeup node for soft (hint of) wakeup time
 * settings.
 */
#define SLEEP_QUERY_WAKEUP_TIME      (NPA_QUERY_RESERVED_END+1)
#define SLEEP_QUERY_MAX_DURATION     (NPA_QUERY_RESERVED_END+5)

/*
===============================================================================
                           GLOBAL FUNCTION DECLARATIONS
===============================================================================
*/

/**
 * sleep_perform_lpm
 *
 * @brief Put the system into the lowest power state possible.
 *
 * This function will eventually call the sleep subsystem and solver to
 * determine the list of low power modes that can be entered during idle time.
 *
 * @return Non-zero on failure.
 */
uint32 sleep_perform_lpm(void);

/**
 * sleep_get_hard_deadline 
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
uint64 sleep_get_hard_deadline( void );

#endif /* SLEEP_V_H */
