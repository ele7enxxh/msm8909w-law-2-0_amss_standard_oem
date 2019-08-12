#ifndef SLEEP_NPA_SCHEDULER_H
#define SLEEP_NPA_SCHEDULER_H

/*=============================================================================
   FILE:           sleep_npa_scheduler.h

   OVERVIEW:       This file contains declarations of functions that are used
                   to interface with sleep by the NPA scheduler subsystem.

                   Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
                   All Rights Reserved.
                   Qualcomm Confidential and Proprietary

   $Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/inc/sleep_npa_scheduler.h#1 $
==============================================================================*/


/* ============================================================================
                         EXTERNAL FUNCTION DEFINITIONS
============================================================================ */
           
/**
 * @brief Set the deadline for the NPA scheduler, in absolute ticks.  Sleep
 * will honor this deadline when choosing low power modes.  This function
 * should NOT be called from the LPR in the sleep context.
 * 
 * @param deadline_abs_ticks:  Next deadline for the NPA scheduler event, in
 *                             absolute tick ticks.
 *                             NOTE:  The tick tick here should
 *                             already be adjusted with the
 *                             local delta kept by the timetick
 *                             subsystem.
 * 
 */
void sleep_npa_scheduler_set_deadline( uint64 deadline_abs_tick );

/**
 * @brief Adjust the previously set deadline for the NPA scheduler.  This
 * function is ONLY meant to be called from the NPA scheduler LPR, when
 * the deadline needs to be pushed out because of time savings from the LPR.
 * The new deadline should be expressed here in absolute ticks.
 * 
 * @param deadline_abs_ticks:  Next deadline for the NPA scheduler event, in
 *                             absolute tick ticks.
 *                             NOTE:  The tick tick here should
 *                             already be adjusted with the
 *                             local delta kept by the timetick
 *                             subsystem.
 * 
 */
void sleep_npa_scheduler_adjust_deadline( uint64 deadline_abs_tick );

/**
 * @brief Cancel the deadline for the NPA scheduler, by recording that the
 * current deadline is no longer valid.
 * 
 */
void sleep_npa_scheduler_cancel_deadline( void );

/**
 * @brief Return the duration (in slcks) until the next NPA 
 * deadline.  This function is meant to be called by sleep, when 
 * determining how long until the next scheduled wakeup. 
 * 
 * @return Duration (in sclks) until the next NPA scheduler deadline.
 * 
 */
uint64 sleep_npa_scheduler_get_duration( void );

#endif /* SLEEP_NPA_SCHEDULER_H */


