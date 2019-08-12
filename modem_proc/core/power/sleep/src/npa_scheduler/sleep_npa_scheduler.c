/*==============================================================================
  FILE:         sleep_npa_scheduler.c
  
  OVERVIEW:     This file implements the sleep interface to the NPA 
                scheduler subsystem. 

  DEPENDENCIES: None

                Copyright (c) 2011-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/npa_scheduler/sleep_npa_scheduler.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "DALStdDef.h"
#include "sleep_npa_scheduler.h"
#include "sleep_log.h"
#include "sleepi.h"
#include "CoreTime.h"
#include "CoreVerify.h"


/* ============================================================================
                              INTERNAL DEFINITIONS
============================================================================ */

/*
 * This variable reflects the NPA scheduler deadline.  This is 
 * expressed in absolute sclk ticks.  It is only written by the 
 * function sleep_npa_scheduler_set_deadline, which is only 
 * called from a locked context.  Sleep uses this to determine 
 * the hard wakeup time. 
 */
static uint64 gNpaDeadlineAbsTick;

/*
 * Indicates whether the time reflected in 
 * gNpaSchedulerDeadlineAbsSclk is a valid value.  If not valid, 
 * then sleep should not use it. 
 */
static boolean gDeadlineValid = FALSE;


/* ============================================================================
                         EXTERNAL FUNCTION DEFINITIONS
============================================================================ */
           
/* 
 * sleep_npa_scheduler_set_deadline
 *
*/
void sleep_npa_scheduler_set_deadline( uint64 deadline_abs_tick )
{
  /* Record the new deadline, for sleep to use the next time through. */
  gNpaDeadlineAbsTick = deadline_abs_tick;
                                                  
  /* Sleep will now start using this deadline. */
  gDeadlineValid = TRUE;
}

/* 
 * sleep_npa_scheduler_adjust_deadline
 *
*/
void sleep_npa_scheduler_adjust_deadline( uint64 new_npa_deadline )
{
  uint64 old_npa_deadline = gNpaDeadlineAbsTick;
  uint64 timer_deadline   = sleep_os_get_last_timer_deadline();
  int64  offset           = 0;

  /* The deadline should always be valid before adjusting it during sleep. */
  CORE_VERIFY( TRUE == gDeadlineValid );

  /* If the NPA deadline was the limiting factor in deciding the hard deadline,
   * then force a recalculation of the hard deadline, with the new value. */
  if ( old_npa_deadline < timer_deadline )
  {
    /* If the NPA deadline is still the next deadline, then calculate the
     * offset between the new NPA deadline and the old one. */
    if ( new_npa_deadline <= timer_deadline )
    {
      offset = new_npa_deadline - old_npa_deadline;
    }
    else
    {
      /* The timer deadline is next, so calculate the offset between that and
       * the old NPA deadline. */
      offset = timer_deadline - old_npa_deadline;
    }

    /* The offset should never be negative.  If it is, print a warning and skip
     * the adjustments. */
    if ( offset < 0 )
    {
      sleep_log_printf( SLEEP_LOG_LEVEL_WARNING, 1*2, 
                        "Negative offset for NPA scheduler adjustment, "
                        "offset not applied (offset: 0x%llx)", ULOG64_DATA(offset) );
    }
    else
    {
      /* Offset the sleep duration.  The new value will be passed into the
       * next enter function. */
      sleep_os_offset_sleep_duration( offset );

    }

    sleep_log_printf( SLEEP_LOG_LEVEL_INFO, 4*2, 
                      " Adjusted NPA scheduler deadline "
                      "(old NPA deadline: 0x%llx) (new NPA deadline: 0x%llx) "
                      "(timer deadline: 0x%llx) (offset: 0x%llx) ", 
                      ULOG64_DATA(old_npa_deadline), ULOG64_DATA(new_npa_deadline), 
                      ULOG64_DATA(timer_deadline), ULOG64_DATA(offset) ); 
  }
}

/* 
 * sleep_npa_scheduler_cancel_deadline
 *
*/
void sleep_npa_scheduler_cancel_deadline()
{
  gDeadlineValid = FALSE;
}

/* 
 * sleep_npa_scheduler_get_duration
 *
*/
uint64 sleep_npa_scheduler_get_duration()
{
  uint64 uNpaSchedulerTick;
  uint64 uCurrentTick;
  
  /* Calculate the NPA scheduler duration */
  if ( FALSE == gDeadlineValid )
  {
    uNpaSchedulerTick = UINT64_MAX;
  }
  else
  {
    /* There is a valid NPA scheduler deadline registered, 
       so calculate the duration */
    uCurrentTick = CoreTimetick_Get64();
    
    /* Check for NPA scheduler deadline in the past */
    if ( uCurrentTick >= gNpaDeadlineAbsTick)
    {
      uNpaSchedulerTick = 0;
    }
    else
    {
      uNpaSchedulerTick = ( gNpaDeadlineAbsTick - uCurrentTick );    
    }
    
  }

  return uNpaSchedulerTick ;
}
