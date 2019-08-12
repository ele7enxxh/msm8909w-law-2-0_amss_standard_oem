/*==============================================================================
  FILE:         sleep_npa_scheduler.c
  
  OVERVIEW:     This file implements the sleep interface to the NPA 
                scheduler subsystem. 

  DEPENDENCIES: None

                Copyright (c) 2011-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/npa_scheduler/sleep_npa_scheduler.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "DALStdDef.h"
#include "sleep_npa_scheduler.h"
#include "sleep_log.h"
#include "sleepi.h"
#include "sleep_osi.h"
#include "CoreTime.h"
#include "CoreVerify.h"

/*==============================================================================
                              INTERNAL DEFINITIONS
 =============================================================================*/
/*
 * This variable reflects the NPA scheduler deadline.  This is 
 * expressed in absolute ticks.  It is only written by the 
 * function sleepNPAScheduler_setDeadline, which is only 
 * called from a locked context.  Sleep uses this to determine 
 * the hard wakeup time. 
 */
static uint64 g_NPADeadlineAbsTick;

/*
 * Indicates whether the time reflected in 
 * g_NPADeadlineAbsTick is a valid value.  If not valid, 
 * then sleep should not use it. 
 */
static boolean g_NPAdeadlineValid = FALSE;

/*==============================================================================
                         EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/* 
 * sleepNPAScheduler_setDeadline
 */
void sleepNPAScheduler_setDeadline( uint64 deadlineAbsTick )
{
  /* Record the new deadline, for sleep to use the next time through. */
  g_NPADeadlineAbsTick = deadlineAbsTick;

  /* Sleep will now start using this deadline. */
  g_NPAdeadlineValid = TRUE;

  return;
}

/* 
 * sleepNPAScheduler_adjustDeadline
*/
void sleepNPAScheduler_adjustDeadline( uint64 new_npa_deadline )
{
  uint64 old_npa_deadline = g_NPADeadlineAbsTick;
  uint64 timer_deadline   = sleepOS_getLastTimerDeadline();
  int64  offset           = 0;

  /* The deadline should always be valid before adjusting it during sleep. */
  CORE_VERIFY( TRUE == g_NPAdeadlineValid );

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
      sleepLog_printf( SLEEP_LOG_LEVEL_WARNING, 1*2, 
                       "Negative offset for NPA scheduler adjustment, "
                       "offset not applied (offset: 0x%llx)", 
                       ULOG64_DATA(offset) );
    }
    else
    {
      /* Offset the sleep duration.  The new value will be passed into the
       * next enter function. */
      sleepOS_offsetSleepDuration( offset );

    }

    sleepLog_printf( SLEEP_LOG_LEVEL_INFO, 4*2, 
                     " Adjusted NPA scheduler deadline "
                     "(old NPA deadline: 0x%llx) (new NPA deadline: 0x%llx) "
                     "(timer deadline: 0x%llx) (offset: 0x%llx) ", 
                     ULOG64_DATA(old_npa_deadline), 
                     ULOG64_DATA(new_npa_deadline), 
                     ULOG64_DATA(timer_deadline), 
                     ULOG64_DATA(offset) ); 
  }

  return;
}

/* 
 * sleepNPAScheduler_cancelDeadline
*/
void sleepNPAScheduler_cancelDeadline()
{
  g_NPAdeadlineValid = FALSE;
  return;
}

/* 
 * sleepNPAScheduler_getTimer
*/
uint64 sleepNPAScheduler_getTimer()
{
  uint64 uNpaSchedulerTick;
  
  /* If not valid, return max deadline */
  if(FALSE == g_NPAdeadlineValid)
  {
    uNpaSchedulerTick = UINT64_MAX;
  }
  /* Otherwise return the current setting */
  else
  {
    uNpaSchedulerTick = g_NPADeadlineAbsTick;
  }

  return uNpaSchedulerTick;
}

