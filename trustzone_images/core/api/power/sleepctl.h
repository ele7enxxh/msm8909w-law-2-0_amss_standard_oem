#ifndef SLEEPCTL_H
#define SLEEPCTL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                       S L E E P   C O N T R O L L E R

GENERAL DESCRIPTION

  This module contains routines for the sleep controller.

EXTERNALIZED FUNCTIONS

  sleepctl_init()
    Initializes the sleep controllers

  sleepctl_arm(id)
    Arms the given sleep controller to start on the next sleep boundary

  sleepctl_start(id)
    Starts the given sleep controller immediately

  sleepctl_block_until_sleeping(id)
    Waits until the sleep controller is running before returning

  sleepctl_set_warmup(id, usec)
    Sets the warmup period, in microseconds.

  sleepctl_set_wakeup_callback(id, usec)
    Sets the function to call at the wakeup event.

  sleepctl_set_ending_callback(id, usec)
    Sets the function to call at the end of sleep (rtc on) event.

  sleepctl_set_duration(id, chips)
    Sets the sleep duration, in chips.  The warmup period is subtracted
    from the sleep duration to determine the wakeup point.

  sleepctl_set_duration_ms(id, milliseconds)
    Sets the sleep duration, in msecs.  The warmup period is subtracted
    from the sleep duration to determine the wakeup point.

  sleepctl_set_max_duration(id)
    Sets the sleep duration to the maximum possible duration.  This may
    be used when the wakeup needs to be changed, but the wakeup point
    is not known yet.

  sleepctl_sufficient_warmup_time(id, min_warmup_sclk)
    May be called by the wakeup_cb to determine whether or not the wakeup
    event is being processed too late.

  sleepctl_complete_wakeup(id)
    May be called by the wakeup_cb to commit to the wakeup if there is
    sufficient warmup time remaining.

  sleepctl_try_wakeup(id, min_warmup_sclk)
    Combines _sufficient_warmup_time/_complete_wakeup into one step.

  sleepctl_error_feedback(id, sys_clk_error)
    Adjust the slow clock frequency estimate, based on the sleep duration
    and the reacquisition slews.

  sleepctl_wakeup_now(id)
    Forces the sleep controller to immediately terminate operation.

  sleepctl_set_cpu_conflict_adjustment_callback(id, conflict_id, adjust_cb)
    Registers a callback which adjusts for CPU conflicts.

  sleepctl_is_active(id)
    Queries whether or not the sleep controller is running or not.

  sleepctl_get_sclk_cnt(id)
    Returns the number of sleep clocks the controller has been active for.

  sleepctl_get_chip_cnt(id)
    Returns the number of chips the sleep controller has been active for.

  sleepctl_get_sclk_till_wakeup(id)
    Returns the number of sleep clocks until the programmed wakeup interrupt.

  sleepctl_get_sclk_till_rtc_on(id)
    Returns the number of sleep clocks until the end of sleep specified by
    sleepctl_set_duration.

  sleepctl_get_wakeup_time(id)
    Return the wakeup time as an absolute timetick counter value.

  sleepctl_get_ending_time(id)
    Return the ending time as an absolute timetick counter value.

  sleepctl_get_next_wakeup()
    Returns the time, in slow clocks, of the next sleep controller wakeup
    event.

  sleepctl_set_warmup_enforcement(id,warmup_enforced)
    Sets the warmup enforcement mode for the specified sleep controller


INITIALIZATION AND SEQUENCING REQUIREMENTS

  sleepctl_init( ) must be called once, before any other sleepctl_ function.

  Sleep is initiated by calling sleepctl_arm() or sleepctl_start().

  While sleeping, sleepctl_set_duration() may be called to set the sleep
  duration.  It may be used to change the sleep duration.  The wakeup
  interrupt occurs "warmup_usec" before the duration set by this function.

  At the wakeup interrupt, sleepctl_sufficient_warmup_time() may be used to
  determine if sufficient time exists for RF warmup, or if processing of the
  wakeup event has been delayed too long by INTLOCKs, etc.  It may be called
  multiple times, at various points in the wakeup processing.  If sufficient
  time exists, sleepctl_complete_wakeup() may be used to commit to the wakeup.
  Alternately, sleepctl_try_wakeup combines sleepctl_sufficient_warmup_time()
  and sleepctl_complete_wakeup() together as one operation.

  If insufficient warmup time exists, sleepctl_set_duration() may be used to
  lengthen the sleep duration until the next required wakeup.

  After the sleep ending interrupt, sleepctl_error_feedback() may be used to
  correct the slow clock frequency estimate.


Copyright (c) 2004-2009 by Qualcomm Technologies, Inc.  
All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=


==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/sleepctl.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   -----------------------------------------------------------
12/11/2009   cab   Mainlining FEATURE_SLEEPCTL_PC
10/13/2009   clm   Added MDM9x00 target
04/22/2009   cab   Added conflict_id param to sleepctl_get_cpu_conflict_adjust
04/17/2009   cab   Added QSC6695 target
09/18/2008   rkc   Added sleepctl_get_cpu_conflict_adjust()
04/18/2008   cab   Added QSC1100 target
03/27/2008   hluo  Added LCU to applicable targets
01/08/2008   hluo  Added Koel and Hawk to applicable targets
02/02/2007   cab   Reorder sleepctl hardware mappings for current targets
12/07/2006   cab   Added sleepctl_set_cpu_conflict_adjustment_callback api
10/13/2006   cab   7600 conditional before 7200 since both are defined
08/23/2006   cab   7600 update
07/21/2006  pp/cab Support for 1x/umts unification.
12/22/2005   ejv   Add support for T_QSC60X0.
06/13/2005   cab   Added power collapse functionality
04/01/2005   bt    Added sleepctl_get_ending_time.
11/10/2004   ajn   SLEEPCTL_UMTS -> SLEEPCTL_GSM
08/24/2004   ajn   chipx16->sys_clk.  Sleep ctl pointers -> enum identifiers.
08/03/2004   ajn   Modified extern definitions to support T32 debugging
06/15/2004   ajn   Added _set_duration_ms( ) for deep sleep to avoid overflow
06/01/2004   bt    Removed various warmup defines. 
04/20/2004   ajn   Added query wakeup time as absolute timetick counter value.
03/24/2004   ajn   set_warmup() updates the wakeup point, if the SC is running
03/24/2004   ajn   Code review feedback changes
03/22/2004   ajn   Separated setting event callback from other functions
02/17/2004   ajn   First checkin
01/23/2004   ajn   Created this file

============================================================================*/


/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "timetick.h"


/*============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/

/*----------------------------------------------------------------------------
  Sleep controller identifiers
----------------------------------------------------------------------------*/

typedef enum
{
#if defined(T_MDM9X00)
  SLEEPCTL_GSM,
  SLEEPCTL_1X,
  SLEEPCTL_HDR,
  SLEEPCTL_WCDMA,
  SLEEPCTL_OFDMA,
#elif defined(T_MSM7600) || defined(T_QSC6695)
  SLEEPCTL_GSM,
  SLEEPCTL_1X,
  SLEEPCTL_HDR,
  SLEEPCTL_WCDMA,
#elif defined(T_QSC1100)
  SLEEPCTL_1X,
#elif defined(T_QSC60X5)
  SLEEPCTL_1X,
  SLEEPCTL_HDR,
#elif defined(T_QSC60X0)
  SLEEPCTL_1X,
#elif defined(T_MSM7200) || defined(T_MSM6246) || defined(T_MSM6290) || defined (T_QSC6270)
  SLEEPCTL_GSM,
  SLEEPCTL_WCDMA,
#elif defined(T_MSM7500)
  SLEEPCTL_GSM,
  SLEEPCTL_1X,
  SLEEPCTL_HDR,
#elif defined(T_MSM6700)
  SLEEPCTL_GSM,
  SLEEPCTL_1X,
  SLEEPCTL_HDR,
#elif defined(T_MSM6500)
  SLEEPCTL_1X,
  SLEEPCTL_HDR,
#else
  SLEEPCTL_1X,
#endif

  SLEEPCTL_NUM_CONTROLLERS

} sleepctl_id_type;



/*----------------------------------------------------------------------------
  Sleep controller event callback function prototype
----------------------------------------------------------------------------*/

typedef void (*sleepctl_event_callback_type) (void);

/*----------------------------------------------------------------------------
  Information about the sleep controller programming for a given technology
----------------------------------------------------------------------------*/

typedef struct
{
  /* The absolute sclk time the sleep controller started sleep counting */
  timetick_type                   starting_timetick;

  /* The absolute sclk time the sleep controller wakeup int will fire */
  timetick_type                   wakeup_timetick;

  /* The absolute sclk time the sleep controller sleep ending int will fire */
  timetick_type                   ending_timetick;

}
sleepctl_wakeup_info_type;


/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/



/*============================================================================

FUNCTION SLEEPCTL_INIT

DESCRIPTION
  This function initializes the Sleep Controller(s)

DEPENDENCIES
  TRAMP must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  Installs ISR's for the sleep controller(s)

============================================================================*/

void sleepctl_init( void );



/*============================================================================

FUNCTION SLEEPCTL_ARM

DESCRIPTION
  This function is called to arm the sleep controller.
  The sleep duration is set to near-maximum (~1.5 days).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The sleep controller will start running on the next event boundary,
  which will be within ...
    26.7ms for the 1x controller (immediately if manually triggered)
    1.67ms for the HDR controller

============================================================================*/

void sleepctl_arm
(
  /* Sleep controller to be armed */
  sleepctl_id_type                id
);


/*============================================================================

FUNCTION SLEEPCTL_START

DESCRIPTION
  This function is called to start the sleep controller.
  The sleep duration is set to near-maximum (~1.5 days).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The sleep controller will start running immediately

============================================================================*/

void sleepctl_start
(
  /* Sleep controller to be started */
  sleepctl_id_type                id
);



/*============================================================================

FUNCTION SLEEPCTL_BLOCK_UNTIL_SLEEPING

DESCRIPTION
  This function waits until the sleep controller is counting sleep clocks,
  indicating that the sys_clk clock source (eg, TCXO) is no longer required.

DEPENDENCIES
  Must only be called after the sleep controller has been armed, and has
  been triggered, or the trigger event will occur within 2ms (such as an HDR
  1.67ms slot boundary)

RETURN VALUE
  None

SIDE EFFECTS
  Consumes CPU time until the sleep controller starts

============================================================================*/

void sleepctl_block_until_sleeping
(
  /* Sleep controller to monitor */
  sleepctl_id_type                id
);



/*============================================================================

FUNCTION SLEEPCTL_SET_WARMUP

DESCRIPTION
  This function sets the interval between the wakeup interrupt and the
  sleep ending interrupt.

  The warmup duration will be subtracted from the expiry point to get the
  wakeup point.

DEPENDENCIES
  Valid slow clock estimate.

RETURN VALUE
  None

SIDE EFFECTS
  If the sleep controller is running, the wakeup point is reprogrammed,
  and the sleep controller's s/w state changed back to the SLEEP state.

============================================================================*/

void sleepctl_set_warmup
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Warmup time, in microseconds */
  uint32                          usec
);


/*============================================================================

FUNCTION SLEEPCTL_SET_START_CALLBACK

DESCRIPTION
  This function sets the callback function to call when the sleep start point 
  is reached.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void sleepctl_set_start_callback
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Sleep start callback */
  sleepctl_event_callback_type    start_cb
);


/*============================================================================

FUNCTION SLEEPCTL_SET_WAKEUP_CALLBACK

DESCRIPTION
  This function sets the callback function to call when the wakeup point is
  reached.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void sleepctl_set_wakeup_callback
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Wakeup callback */
  sleepctl_event_callback_type    wakeup_cb
);


/*============================================================================

FUNCTION SLEEPCTL_SET_ENDING_CALLBACK

DESCRIPTION
  This function sets the callback function to call when the ending point
  (rtc on) is reached.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void sleepctl_set_ending_callback
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Wakeup callback */
  sleepctl_event_callback_type    ending_cb
);


/*============================================================================

FUNCTION SLEEPCTL_SET_DURATION

DESCRIPTION
  This function computes the point when the sleep controller should expire,
  and sets the appropriate wakeup point to the hardware.

  The sleep controller expiry point is left at ~1.5 days.

DEPENDENCIES
  Valid slow clock estimate.
  sleepctl_set_warmup( ) must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  Changes the wakeup point.

============================================================================*/

void sleepctl_set_duration
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Sleep duration, in chips */
  uint32                          chips
);



/*============================================================================

FUNCTION SLEEPCTL_SET_DURATION_SYS_CLK

DESCRIPTION
  This function computes the point when the sleep controller should expire,
  and sets the appropriate wakeup point to the hardware.

  The sleep controller expiry point is left at ~1.5 days.

DEPENDENCIES
  Valid slow clock estimate.
  sleepctl_set_warmup( ) must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  Changes the wakeup point.

============================================================================*/

void sleepctl_set_duration_sys_clk
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Sleep duration, in sys clks */
  uint32                          sys_clks
);



/*============================================================================

FUNCTION SLEEPCTL_SET_DURATION_MS

DESCRIPTION
  This function computes the point when the sleep controller should expire,
  and sets the appropriate wakeup point to the hardware.

  The sleep controller expiry point is left at ~1.5 days.

DEPENDENCIES
  Valid slow clock estimate.
  sleepctl_set_warmup( ) must have been called.
  
  *** Deprecated ***
  This function will be removed when INACTIVE state changes are implemented

RETURN VALUE
  None

SIDE EFFECTS
  Changes the wakeup point.

============================================================================*/

void sleepctl_set_duration_ms
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Sleep duration, in milliseconds */
  uint32                          ms
);



/*============================================================================

FUNCTION SLEEPCTL_SET_MAX_DURATION

DESCRIPTION
  This function sets the wakeup point to ~1.5 days.

DEPENDENCIES
  Sleep controller must be running

RETURN VALUE
  None

SIDE EFFECTS
  Changes the wakeup point.

============================================================================*/

void sleepctl_set_max_duration
(
  /* Sleep controller */
  sleepctl_id_type                id
);



/*============================================================================

FUNCTION SLEEPCTL_ADJUST_WAKEUP_CALLBACK_TYPE

DESCRIPTION
  This callback function returns the new sleep duration in chipx8s to 
  account for a perceived CPU conflict with another technology.

DEPENDENCIES
  Function must return a time in the past or within 5 sclks as a safety
  margin.

RETURN VALUE
  The new sleep duration in chipx8s from the start of the sleep controller
  counting until the sleep ending interrupt.

SIDE EFFECTS
  None

============================================================================*/

typedef uint32 (*sleepctl_adjust_wakeup_callback_type) 
(
  /* Current wakeup information for the client doing the adjusting */
  const sleepctl_wakeup_info_type*,

  /* Current wakeup information for the conflicting client */
  const sleepctl_wakeup_info_type*,

  boolean
    /* Save adjusted values.  TRUE means save adjust value in
       callback's database. */
);

/*============================================================================

FUNCTION SLEEPCTL_SET_CPU_CONFLICT_ADJUSTMENT_CALLBACK

DESCRIPTION
  This function sets the callback function to call when either of the two
  listed sleep controllers are programmed. The callback will adjust id's
  sleep time to account for the CPU conflict.

DEPENDENCIES
  Must be called prior to any conflicting wakeup is scheduled. This also
  must be called for earch sleep controller arming as it is only active for
  the next wakeup before it is reset.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void sleepctl_set_cpu_conflict_adjustment_callback
(
  /* Sleep controller which will adjust for a CPU conflict */
  sleepctl_id_type                      id,

  /* Conflicting sleep controller */
  sleepctl_id_type                      conflict_id,

  /* Callback which will make the sleep time adjustment */
  sleepctl_adjust_wakeup_callback_type  adjust_cb
);



/*============================================================================

FUNCTION SLEEPCTL_GET_CPU_CONFLICT_ADJUST

DESCRIPTION
  This function calculates the wakeup information for a given sleepctl_id_type.
  After this, a callback function is called with the new wakeup info and the
  conflicting wakeup info passed as arguments to calculate the CPU adjustment,
  if any.  This function is useful, e.g., for smaller sleep cycles, when it is
  necessary to calculate if there is enough time to sleep based on any CPU
  conflict adjustments.

INPUTS
  id                    - Sleepctl id
  conflict_id           - Sleepctl conflict id
  sleep_dur_sysclks     - Sleep duration in sys_clks
  adjust_cb             - Callback which will calculate the sleep time adjustment

DEPENDENCIES
  adjust_cb must return an adjustment time in slots (uint32)

RETURN VALUE
  Sleep time adjustment in slots

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_cpu_conflict_adjust
(
  /* Sleep controller id */
  sleepctl_id_type                      id,

  /* Sleep controller conflict id */
  sleepctl_id_type                      conflict_id,

  /* Sleep duration, in sys_clks */
  uint32                                sleep_dur_sysclks,

  /* Callback which will calculate the sleep time adjustment */
  sleepctl_adjust_wakeup_callback_type  adjust_cb
);


/*============================================================================

FUNCTION SLEEPCTL_IS_ACTIVE

DESCRIPTION
  This function returns whether the sleep controller is active or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  if the sleep controller is running
  FALSE if the sleep controller is not running

SIDE EFFECTS
  None

============================================================================*/

boolean sleepctl_is_active
(
  /* Sleep controller */
  sleepctl_id_type                id
);



/*============================================================================

FUNCTION SLEEPCTL_GET_SCLK_CNT

DESCRIPTION
  Return the number of sleep clocks the sleep controller has slept for.

DEPENDENCIES
  The given sleep controller must be running.
  Must only be called from an INTLOCK'd context.

RETURN VALUE
  Number of sleep clocks

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_sclk_cnt
(
  /* Sleep controller */
  sleepctl_id_type                id
);



/*============================================================================

FUNCTION SLEEPCTL_GET_SYS_CLK_CNT

DESCRIPTION
  Return the number of sys_clk's the sleep controller has slept for.

DEPENDENCIES
  The given sleep controller must be running.
  Must only be called from an INTLOCK'd context.

RETURN VALUE
  Length of sleep, in sys_clk's

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_sys_clk_cnt
(
  /* Sleep controller */
  sleepctl_id_type                id
);



/*============================================================================

FUNCTION SLEEPCTL_GET_CHIP_CNT

DESCRIPTION
  Return the number of chips the sleep controller has slept for.

DEPENDENCIES
  The given sleep controller must be running.
  Must only be called from an INTLOCK'd context.

RETURN VALUE
  Length of sleep, in chips

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_chip_cnt
(
  /* Sleep controller */
  sleepctl_id_type                id
);



/*============================================================================

FUNCTION SLEEPCTL_GET_SCLK_TILL_WAKEUP

DESCRIPTION
  Return the number of sleep clocks until the wakeup interrupt.

DEPENDENCIES
  The given sleep controller must be running.
  Must only be called from an INTLOCK'd context.

RETURN VALUE
  Number of sleep clocks

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_sclk_till_wakeup
(
  /* Sleep controller */
  sleepctl_id_type                id
);



/*============================================================================

FUNCTION SLEEPCTL_GET_SCLK_TILL_RTC_ON

DESCRIPTION
  Return the number of sleep clocks until the sleep ending interrupt.

DEPENDENCIES
  The given sleep controller must be running.
  Must only be called from an INTLOCK'd context.

RETURN VALUE
  Number of sleep clocks

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_sclk_till_rtc_on
(
  /* Sleep controller */
  sleepctl_id_type                id
);



/*============================================================================

FUNCTION SLEEPCTL_GET_WAKEUP_TIME

DESCRIPTION
  Return the scheduled wakeup time as an absolute timetick counter value.

DEPENDENCIES
  The given sleep controller must be running.

RETURN VALUE
  Timetick counter value at the wakeup interrupt

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_wakeup_time
(
  /* Sleep controller */
  sleepctl_id_type                id
);


/*============================================================================

FUNCTION SLEEPCTL_GET_ENDING_TIME

DESCRIPTION
  Return the scheduled ending time as an absolute timetick counter value.

DEPENDENCIES
  The given sleep controller must be running.

RETURN VALUE
  Timetick counter value at the sleep ending interrupt

SIDE EFFECTS
  None

============================================================================*/
uint32 sleepctl_get_ending_time
(
  /* Sleep controller */
  sleepctl_id_type                id
);


/*============================================================================

FUNCTION SLEEPCTL_SUFFICIENT_WARMUP_TIME

DESCRIPTION
  Determine if sufficient time exists before the computed end-of-sleep point
  to perform RF warmup.

DEPENDENCIES
  Should only be called in response to the wakeup ISR.
  Should only be called from an INTLOCK'd context.

RETURN VALUE
  TRUE  - Sufficient warmup time exists
  FALSE - Insufficient warmup time exists

SIDE EFFECTS
  None

============================================================================*/

boolean sleepctl_sufficient_warmup_time
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Minimum warmup time, in slow clocks */
  uint32                          min_warmup_sclk
);



/*============================================================================

FUNCTION SLEEPCTL_COMPLETE_WAKEUP

DESCRIPTION
  Reprogram the sleep controller with the correct ending point

DEPENDENCIES
  May only be called in response to the wakeup ISR, and should only be called
  if sleepctl_sufficient_warmup_time( ) returns TRUE.

RETURN VALUE
  None

SIDE EFFECTS
  Sleep ending interrupt will be generated.

============================================================================*/

void sleepctl_complete_wakeup
(
  /* Sleep controller */
  sleepctl_id_type                id
);



/*============================================================================

FUNCTION SLEEPCTL_TRY_WAKEUP

DESCRIPTION
  Determine if sufficient time exists before the computed end-of-sleep point
  to perform RF warmup.  If there is, commit to the wakeup.

DEPENDENCIES
  May only be called in response to the wakeup ISR.

RETURN VALUE
  TRUE  - Sufficient warmup time exists, end of sleep set
  FALSE - Insufficient warmup time exists

SIDE EFFECTS
  May generate sleep ending interrupt

============================================================================*/

boolean sleepctl_try_wakeup
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Warmup time, in slow clocks */
  uint32                          min_warmup_sclk
);



/*============================================================================

FUNCTION SLEEPCTL_ERROR_FEEDBACK

DESCRIPTION
  This function is used to notify the sleep controller of the perceived
  "error" in the most recent sleep interval's length, which the sleep
  controller may use to fine tune its estimate of the sleep clock.

DEPENDENCIES
  An initial estimate of the sleep clock frequency must be known, from
  sleep_clock_freq_estimate_complete( ).

RETURN VALUE
  None

SIDE EFFECTS
  Sleep clock frequency estimate is update.

============================================================================*/

void sleepctl_error_feedback
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Perceived error in the sleep duration of the most recent sleep */
  int32                           sys_clk_error
);



/*===========================================================================

FUNCTION SLEEPCTL_WAKEUP_NOW

DESCRIPTION
  This function wakes up the sleep controller as soon as possible. The
  function will not return until the sleep controller is awake.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sleepctl_wakeup_now
(
  /* Sleep controller */
  sleepctl_id_type                id
);



/*============================================================================

FUNCTION SLEEPCTL_GET_NEXT_WAKEUP

DESCRIPTION
  This function returns the time, in sleep clocks, until the next
  wakeup event (1x, HDR, etc.)

DEPENDENCIES
  None

RETURN VALUE
  Number of sleep clocks until the next wakeup event

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_next_wakeup( void );


/*============================================================================

FUNCTION SLEEPCTL_RESTORE_FROM_PC

DESCRIPTION
  This function is called to when coming out of power collapse

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The sleep controller will resume running on the next event boundary,
  which will be within ...
    26.7ms for the 1x controller (immediately if manually triggered)
    1.67ms for the HDR controller

============================================================================*/

void sleepctl_restore_from_pc ( void );


/*============================================================================

FUNCTION SLEEPCTL_SET_WARMUP_ENFORCEMENT

DESCRIPTION
  This function sets the specified sleep controller warmup enforcement mode.
  If warmup is enforced, then the sleep controller will make sure that
  the wakeup occurs and that there is enough time for warmup before setting
  the ending expiry point.  If warmup is not enforced, the sleep controller
  wakeup point can be latent, and even if there is not enough warmup time, 
  the ending point will occur as specified in set_duration anyway.
  
DEPENDENCIES
  NONE

RETURN VALUE
  None

SIDE EFFECTS
  Changes the warmup mode for the sleep controller

============================================================================*/

void sleepctl_set_warmup_enforcement
( 
  /* Sleep controller to be restarted */
  sleepctl_id_type                id,

  /* Indicates whether warmup should be enforced or not */
  boolean                         warmup_enforced
);


#ifdef FEATURE_UNIFIED_SLEEP_CTLR_P1
/*============================================================================

FUNCTION SLEEPCTL_FORCE_STATE, SLEEPCTL_FORCE_WAKEUP_SCLK

DESCRIPTION
  Temporary function hacks to make WCDMA sleep work.
  TBD: These functions and calls to them should be removed once WCDMA has
       code in place to use the common sleep controller.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void sleepctl_force_state(sleepctl_id_type i,int state);
void sleepctl_force_wakeup_sclk(sleepctl_id_type id,uint32 sclk);
#endif

#endif /* !SLEEPCTL_H */


