/*=============================================================================

                  T I M E R   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using the slow clock.

  Slow Clock Frequency          Granularity     Timer Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


REGIONAL FUNCTIONS 

  timer_init()
    Initializes the timer subsytem.  Call only from time_init( ).


EXTERNALIZED FUNCTIONS

  timer_def(timer, group, tcb, sigs, func, data)
    Defines and initializes a timer.

  timer_def_priority(timer, group, tcb, sigs, func, data, timer_prio)
    Same as timer_def, but with an option to defnie the timer as a priority timer also.

  timer_def2(timer, group, func, data)
    Defines and initializes a timer, with the clock callback semantics.

  timer_def2_priority(timer, group, func, data, timer_prio)
    Same as timer_def2, but with an option to defnie the timer as a priority timer also.

  timer_def_osal(timer,group,sigs_obj,sigs,func,data)
    Defines and initializes a timer. Provision of using a signal object for notification.

  timer_def_osal_priority(timer,group,sigs_obj,sigs,func,data,timer_prio)
    Same as timer_def_osal(), with added option to specify the timer priority.

  timer_reg(timer,func,data,time_ms,reload_ms)
    Complimentary function to timer_def2. Sets the timer as per the passed params.
    
  timer_set(timer, ticks, reload, unit)
    Sets an inactive timer to expire after a given period of time, or changes
    an active timer to expire after a given period of time.
    Optionally, specifies the timer to repeatly expire with a given period.

  timer_set_64(timer, ticks, reload, unit)
    Same as timer_set, but this API also supports the 64 bit timetick values @ 19.2 Mhz as an input.

  timer_set_absolute(timer,time)
    Sets an inactive timer to expire at the given time, or changes an active timer to expire at that time.
  
  timer_get(timer, unit)
    Get number of ticks before timer expires

  timer_get_64(timer, unit)
    Same as timer_get(), but with support for 64 bit timetick values @ 19.2 Mhz.

  timer_get_params(timer,timer_attrib_struct_ptr)
    Fills up the passed timer attribute structure with timer parameters.

  timer_set_params(timer,to_set,timer_attrib_struct_ptr)
    Updates the timer parameter based on the passed arguments.

  timer_is_active(timer)
    Returns boolean TRUE / FALSE based on the timer active status.

  timer_expires_at_64(timer)
    Returns the 19.2 Mhz, 64 bit timetick val for the timer's expiry if it is active 

  timer_expires_at(timer)
    Returns the slow clock based timetick val for the timer's expiry if it is active

  timer_get_start_64(timer)
    Return the 19.2 Mhz based 64 bit timetick value when the timer was started.

  timer_get_start(timer)
    Return the slow clock based timetick value when the timer was started.

  timer_pause(timer)
    Suspends an active timer

  timer_resume(timer)
    Resumes a previously "paused" active timer.

  timer_clr(timer, unit)
    Stops an active timer and return the remaining slow clock based timeticks before expiry.

  timer_clr_64(timer, unit)
    Stops an active timer and return the remaining 19.2 Mhz based 64 bit timeticks before expiry.

  timer_stop(timer,unit,rem_time)
    Stops an active timer and returns the remaining ticks before expiry in the passed argument.

  timer_undef(timer)
    Stops an active timer and also deallocates any internal memory reserved for the timer.
      
  timer_clr_task_timers(tcb)
    Stops all active timers which belong to the specified task.

  timer_clr_osal_thread_timers(thread_id)
    Stops all active timers which belong to the specified thread.
   
  timer_group_enable(timer_group)
    Enables a timer group.  Timers in the timer group which expired during the
    diabled period will have their expiry processing executed.

  timer_group_disable(timer_group)
    Disables a timer group.  Timers in the timer group will continue to count
    down, but they will not expire.

  timer_group_set_deferrable(pgroup,condition)
    Sets the passed group property as deferrable or non-deferrable



INITIALIZATION AND SEQUENCING REQUIREMENTS

    timer_init() must be called once before any other timer functions.

    timer_def() must be called, or the TIMER_DEF( ) macro used, for each
      timer, before other timer functions are used with that timer.

    A valid sclk estimate is needed before timers can be set.


Copyright (c) 2003 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/timer.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/30/13   abh     New API additions and adding error return types to the existing ones, and some changes.
04/26/11   din     Fixed compiler warnings.
03/11/11   ab      Removed compiler warnings.
03/10/11   ab      Fix a corner case when a small expiry timer is inserted 
                   immediately after a large expiry timer.
11/12/10   din     Added function to get time to next occuring non-deferrable 
                   timer expiry.
08/25/10   din     Removed call to timetick_get_sclk_offset in timer_expires_in.
04/27/10   din     Fixed compiler warnings.
12/18/09   din     Adding Timer List debugging feature.
12/10/09   din     Timer expiry now caters to changing sclk_delta
09/30/09   sri     Enhanced QXDM strings on timer_group NULL
05/22/09   din     Added missing refernce of zero in timer_defer_match_interrupt 
05/22/09   din     Moved implementation of timer_set_next_interrupt to 
                   timetick_set_next_interrupt.
05/22/09   din     Removed FEATURE_TIMER_TASK. Removed Function timer_isr.
01/16/09   pbi     Moved MIN_REARM_SCLK definition to timetick.c
01/07/09   pbi     sclk_delta removed from timers_type
01/07/09   pbi     Removed timer_set_sclk_offset
03/25/09   din     In timer_expire(), cached callback functions from timer 
                   structure before first callback is made.
03/04/09   din     timer_defer_match_interrupt returns zero when expiry of first 
                   non deferrable timers is in past.
02/23/09   din     Consoldation of time.
01/21/09   din     Forced timer_undefer_match_interrupt() to update MATCH_VAL
                   register. Updated timer_process_active_timers() to prevent
                   overflow in timer expiry past the active timer list zero.
01/16/09   din     Added missing reset of last_set_time in timer_restart().
01/13/09   din     Removed timer_delay_expiry_excluding_groups() and
                   timer_undelay_expiry().
01/12/09   din     Added deferrable attribute to timer_group. Also added 
                   lightweight API's to enable/ disable timer groups. API's
                   timer_defer_match_interrupt(), timer_undefer_match_interrupt
                   & timer_group_set_deferrable().
01/05/09   din     Consolidation of time sources. Added 
                   FEATURE_TIME_UMTS_TARGETS. timer_set_next_interrupt was
           made uniform acroos different targets. Also Ensured that 
           timer_get() doesnot return 0.
12/23/08   tev     Remove timer_v.h inclusion.  No longer required now that
                   timer_v.h is obsolete.
12/15/08   tev     Explicitly include newly created private header file
                   timer_v.h
11/20/08   din     Always set set_time when match_value is updated in 
                   timer_set_next_interrupt(). Every attempt to set is recorded 
                   in last_set_time.
11/14/08   tev     Amended control for timer_restart to
                   defined(FEATURE_TIME_POWER_COLLAPSE) &&
                   !defined(FEATURE_TIME_REMOVE_TIMER_RESTART)
11/10/08   tev     Tentatively restored timer_restart under control of
                   FEATURE_TIME_POWER_COLLAPSE symbol definition.
10/17/08   din     Fixed initialization of do_reprogram_isr
10/16/08   din     Added lightweight mechanism to enable and disable timer groups 
                   during sleep. For use by the sleep subsystem only! APIs are
                   timer_delay_expiry_excluding_groups(), and timer_undelay_expiry()
                   (renamed timer_restart().) 
09/19/08   trc     timer_bin_find_first_exceeding() returns NULL upon failure
06/10/08   trc     Upon re-enabling a timer group, only nudge the active timer
                   list zero point as far back as the earliest expired timer
11/13/07   jhs     Removed timer_always_on_group.
08/31/07   jhs     Support for AMSS RESTART
05/29/07   jhs     Added in support to rewrite MATCH when writing match over a
                   tick boundary if FEATURE_TIME_REQUIRES_ODD_WRITE is defined.
04/19/07   gfr     Support for timer tracing.
01/12/07   trc     Increase MIN_REARM_SCLK from 6 to 9 slow clock cycles
12/04/06   trc     Bump up the # of timer bins from 64 to 128
10/11/06   trc     Reset match_value and set_value when sclk_delta is set
                   coming out of power collapse since they are no longer valid.
08/19/06   gb      Changed FEATURE_POWER_COLLAPSE to FEATURE_TIME_POWER_COLLAPSE. Made
                   changes for modem power collapse code.
08/15/06   jhs     Clear out match_value and set_value when sclk_delta is set
                   because they are no longer valid.
04/10/06   ajn     Prevent rounding nudge from creating a negative runtime.
04/07/06   gfr     Remove unneeded header file (msm_drv.h)
03/01/06   kap     Only unbin when timer->bin field is non-null
02/22/06   ajn     Add offset btw modem & MPM sclk counter for power collapse
02/10/06   kap/ajn Added Timer Optimization Changes (double linking/binning)
12/20/05   ejv     Added T_QSC60X0.
11/03/05   ajn     Zero-initialize the timer.cache fields in timer_define()
10/27/05   ajn     Always set set_time in timer_set_next_interrupt().
08/12/05   jhs     Moved apps processor to its own caching assembly function
                   for writing to MATCH_VAL.
08/05/05   ajn     MATCH_VAL written from caching assembly function.
07/29/05   ajn     Repeat MATCH_VAL writes until the write latches.
07/22/05   ajn     Don't write new MATCH_VAL until last write has latched.
07/06/05   ajn     Added timer_set_modem_app_sclk_offset (7500 APPS)
06/24/05   ajn     Include partial 7500 support
06/09/05   ajn     Corrected non-timer_task initialization.
05/19/05   ajn     Timer op's don't signal task or call process_active_timers.
05/09/05   ajn     Don't write to TIMETICK_MATCH_VAL if value is unchanged.
                   Fixed O(N^2) list insertion in timer_group_enable().
05/06/05   ajn     Cache time/reload values to avoid recalculating if unchanged
03/29/05   ajn     Replaced timer_ptr with timer_ptr_type, for L4 project.
03/16/05   ajn     Update next timer expiry interrupt w/o processing timers
                   when timers are removed from the active list.
03/16/05   ajn     Add ms rounding to following expiry in repeating timers.
12/06/04   lcc     In timer_process_active_timers, added reading of timetick
                   count to make sure it's non-zero before existing.  This is
                   needed for QUASar's timetick hw problem.
10/20/04   ajn     Moved timer expiry out of ISR context into a Timer task.
09/28/04   ajn     Catch use of timers w/o call to timer_define().
09/27/04   ajn     Force timer in the NULL timer group into an actual group.
08/30/04   ajn     Added "get next interrupt" and "interrupt early" functions
                   for TCXO shutdown.
07/06/04   ajn     timer_define() ERR's (not ERR_FATAL's) on active timer.
06/23/04   ajn     Corrected re-entrancy when processing active timers.
01/29/04   ajn     Corrected possible NULL pointer dereference.
01/09/04   ajn     Optimized the expiry-time advance of auto-reloading timers.
11/14/03   ajn     Added sclk timestamps for profiling timer_isr latency.
11/11/03   ajn     Added checks to detect timer_def( ) calls on active timers
10/03/03   ajn     Call added to process_active_timers() in group_disable, to
                   ensure next active timer's interrupt occurs at proper time.
08/11/03   ajn     Added timer_pause/timer_resume for rex_set_timer replacement
08/08/03   ajn     Updated for coding standards
08/06/03   ajn     Added timer_def2(), for clk_reg() replacement
07/23/03   ajn     Moved _{to,from}_sclk functions to timetick.c
07/17/03   ajn     File created.

=============================================================================*/



/*=============================================================================


  *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE ***

  
  All comparisons in this module are done with respect to a moving origin,
  or "zero" point, to handle wrap-around of the 32-bit slow clock counter.
  
  For example, the conditional statement "if ( now >= expiry )" is wrong,
  since "now" could be 0xFFFFFF00, and "expiry" could be "0x00000100", ...
  "0x200" counts in the "future".
  
  The correct comparison would be "if ( now - zero >= expiry - zero )".
  This compares the current time (measured from some point in the past) to
  the expiry time (measured with respect to the same point in the past).
  If the "zero" origin was "0xFFFFFE00", with the above values, "now - zero"
  would be "0x100" and "expiry - zero" would be "0x300", making the entire
  expression FALSE; the expiry point has not been reached.

  If the expression "now - zero >= expiry - zero" returns TRUE, then it is
  safe to subtract "now - expiry" to determine how much time has passed since
  the expiry point.  If "now - zero >= expiry - zero" returns FALSE, the
  reverse subtraction is valid: "expiry - now" indicates how much time
  remains until the expiry point.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "ats_v.h"
#include "timer.h"
#include "err.h"
#include "assert.h"
#include "stdint.h"
#include "qurt_signal2.h"
#include "DALSys.h"
#include "rlc.h"

#ifndef FEATURE_TIMER_USE_QURT_SYSCLOCK
#include "DDIInterruptController.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/
#ifdef FEATURE_TIME_QDSP6
#include "qurt.h"
#endif /*FEATURE_TIME_QDSP6*/

#if defined(FEATURE_TIMER_APQ8064)
#define TRAMP_SLP_TICK_ISR TRAMP_GP_TIMER_IRQ
#endif

#include <stringl/stringl.h>
#include "timer_fn_map.h"
#include "rcevt.h"
#include "rcinit.h"
#include "queue.h"

#include "timer_slaves.h"
#include "DDITimetick.h"
#include "ats_log_v.h"

#include "npa.h"
#include "ClockDefs.h"
#include "VCSDefs.h"

/*timer debug variables can be mainlined based on profiling studies
 done. The latency introduced due to these features are less than a 
 tick on an average */
#define FEATURE_ENABLE_TIMER_DEBUG_VARIABLES

/*as of now this feature is disabled by default. The debug team can either
 uncomment this line or define this feature in .builds file*/
/*#define FEATURE_DEBUG_OFFENDING_TIMERS_ISOLATION*/

/*we still do not want to mainline FEATURE_TIMER_LIST_DEBUG 
  since it has not been profiled as of yet */
/*#define FEATURE_TIMER_LIST_DEBUG*/

/*=============================================================================

                             CONFIGURATION

=============================================================================*/


/*-----------------------------------------------------------------------------
 Select edge-triggered timer interrupt, if supported
-----------------------------------------------------------------------------*/

#if defined( T_MSM7500 ) || defined( T_QSC60X0 ) || defined (FEATURE_TIMER_APQ8064)


  #define TIMETICK_INT_EDGE_TRIGGERED

  /* All interrupts are already edge-triggered */
  #define TIMETICK_INT_EDGE_TRIGGERED_INIT() /* No-op */


#elif defined( T_MSM6700 ) || defined( T_MSM6275 )


  #define TIMETICK_INT_EDGE_TRIGGERED

  #define TIMETICK_INT_EDGE_TRIGGERED_INIT() \
    HWIO_INT_DET_REG_1_OUTM( \
      HWIO_INT_DET_REG_1_SLEEP_TIME_TICK_INT_BMSK, \
      HWIO_INT_DET_REG_1_SLEEP_TIME_TICK_INT_BMSK )


#elif defined( T_MSM6550 )


  #define TIMETICK_INT_EDGE_TRIGGERED

  /*lint -emacro(570,TIMETICK_INT_EDGE_TRIGGERED_INIT) */
  #define TIMETICK_INT_EDGE_TRIGGERED_INIT() \
    HWIO_INT_DETECT_CTL_0_OUTM( \
      HWIO_INT_DETECT_CTL_0_SLP_XTAL_TIMETICK_INT_BMSK, \
      HWIO_INT_DETECT_CTL_0_SLP_XTAL_TIMETICK_INT_BMSK )

#endif /* T_MSMxxxx */


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

#define TIMER_MAX_BACKOFF		 (1920) //100us in xo ticks


/*-----------------------------------------------------------------------------
  Temporary data for Badger.
-----------------------------------------------------------------------------*/
#define TIMER_FREQ_VALUE          19200000

/*-----------------------------------------------------------------------------
  Signals used by the timer task.
-----------------------------------------------------------------------------*/

/* Signal for a timer expiring */
#define TIMER_EXPIRY_SIG          0x00000001

/* Signal for a change in the active timer list */
#define TIMER_CHANGE_SIG          0x00000002

/* Signals which must cause the active timer list to be processed */
#define TIMER_PROCESS_SIGS        (TIMER_EXPIRY_SIG | TIMER_CHANGE_SIG)

/*-----------------------------------------------------------------------------
  Timer Bin Configuration
   - Modify these values to improve binning performance
-----------------------------------------------------------------------------*/

/* Number of bins available. This must be large enough to accomodate all the
   timers in the system, or an err_fatal will be reached */ 
#define TIMER_BIN_NUM             128 

/* The range (in timeticks) of each timer bin 
    - This must be (2^n - 1) to work (for bitwise optimizations)
    - 8191 = 0.25 sec bins */
#define TIMER_BIN_RANGE           TIMER_FREQ_VALUE/4
#define MAGIC_INITIALIZER         0xDEADBEEF
#define MAGIC_INITIALIZER_64      0xDEADBEEFD00DFEEDuLL

/* Factor to divide 19.2MHz clock into 32kHz */
#define XO_TO_SCLK_CONV_FACTOR 586

/* This range is a result of the following calculation...
In general we can say that the oscillator has a +/- 100% swing in freq
Therefore lower bound on Latency(Ticks/ms) = 2*f/1000 
If 5ms is max then TIMER_SCLK_LATENCY = (2*f/1000)*5 = f/100 
*/
#define TIMER_SCLK_LATENCY        TIMER_FREQ_VALUE/100

#ifdef FEATURE_OS_MEMORY_PROTECTION
extern byte * A11S_CSR_BASE;
#endif


/** Pointer to timer list structure */
typedef struct timer_list_struct  *timer_list_ptr;
/** Pointer to timer bin structure */
typedef struct timer_bin_struct   *timer_bin_ptr;

/** Timer cache structure type */

typedef struct
{
  /** Original set duration */
  time_timetick_type                   duration;

  /** Converted set duration */
  time_timetick_type                   duration_sclk;

  /** Original reload increment */
  time_timetick_type                   reload;

  /** Converted reload increment */
  time_timetick_type                   reload_sclk;

  /** Time units */
  timer_unit_type                 unit;
}
timer_cache_type;

/** Timer Queue flags with magic number */

typedef enum timer_internal_queue
{
  TIMER_IN_FREE_Q = 0x0BABABAB,
  TIMER_NOT_IN_FREE_Q = 0x0CACACAC 
} timer_internal_q;

/** Internal Timer struture referenced by external timer structure declared in 
timer.h through handle */
typedef struct timer_struct_internal
{
  /* Link for the next queue */
  q_link_type                     link;
  
  /* Index of timer_buffer array */
  int32                           index;
  
  /* Group to which timer is a member */
  timer_group_ptr                 group;

  /** Task to signal when timer expires, or task to use for APC */
  rex_tcb_type                    *tcb;

  /** Thread ID of the client using the timer */
  time_osal_thread_id			  thread_id;

  /** Signal(s) to set in task, when timer expires */
  rex_sigs_type                   sigs;

  /** APC (Asynchronous Procedure Call) function to call when timer expires */
  timer_t1_cb_type                func1;

  /** Clock callback style function to call when timer expires */
  timer_t2_cb_type                func2;

  /** Data for APC function, when timer expires */
  timer_cb_data_type              data;

  /** Callback type for the timer */
  timer_notify_type               notify_type;  

  /** The list the timer is on.  "list" will contain:
       "&active_timers" if the timer is active;
       "&timer->group->list" if the timer is active, but in a disabled group;
       "NULL" if the timer is not active. */
  timer_list_ptr                  list;

  /** Slow clock tick count timer expires at (list != NULL),
     or remaining ticks until expiry if timer is paused (list == NULL) */
  time_timetick_type                   expiry;

  /** Reload offset when timer expires (0 = no reload at expiry) */
  time_timetick_type                   reload;

  /** Slow clock tick count value when timer was set (started) */
  time_timetick_type                   start;

  /** Slow clock delta count value when timer was set (started) */
  uint32                          sclk_delta;

  /** Durations and reload times, original units and converted to sclks */
  timer_cache_type                cache;

  /** Pointer to the next timer in the list (list != NULL) */
  timer_ptr_internal_type         next;

  /** Pointer to the previous timer in the list (list != NULL) */
  timer_ptr_internal_type         prev;

  /** Pointer to the bin for this timer */
  timer_bin_ptr                   bin;

  /* Enum depicting if timer_internal_structure is in queue */
  timer_internal_q                internal_q;          
  
  /* Backward pointer to external timer */
  timer_ptr_type                  timer_ext;

  /*to store the assignment of slave task*/
  uint8                          slave_task_assigned;

  /*to store timer priority*/
  timer_priority_enum_type       timer_priority;
}
timer_internal_type;


/*-----------------------------------------------------------------------------
  Timer Bin structure type
    Represents a single bin used for fast insertion into the timer list
-----------------------------------------------------------------------------*/

typedef struct timer_bin_struct
{
  /* First timer in the bin */
  timer_ptr_internal_type         first;

  /* Last timer in the bin */
  timer_ptr_internal_type         last;

  /* Earliest expiry time in the bin */
  time_timetick_type                   lower;

  /* Latest expiry time in the bin */
  time_timetick_type                   upper;

  /* Number of timers in the bin */
  int                             count;

  /* Next bin pointer */
  timer_bin_ptr                   next_bin;

  /* Previous bin pointer */
  timer_bin_ptr                   prev_bin;
}
timer_bin_type;


/*-----------------------------------------------------------------------------
  Timer bin list type
-----------------------------------------------------------------------------*/

typedef struct timer_bin_list_struct
{
  /* First timer bin in list */
  timer_bin_ptr                   first;

  /* Last timer bin in list */
  timer_bin_ptr                   last;

  /* Number of timer bins */
  int                             count; 
}
timer_bin_list_type, *timer_bin_list_ptr;


/*-----------------------------------------------------------------------------
  Pool of timer bins
-----------------------------------------------------------------------------*/

timer_bin_type              timer_bin_pool[ TIMER_BIN_NUM ];

/*-----------------------------------------------------------------------------
  Private timer data ...
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Faux timer group, used to hold list of active timers, as well as a list
     of timer groups.
     Active timers will never "belong" to this group, ie timer->group will
     never be &timers.active, since this would cause them to forget their
     true timer group. */
  timer_group_type                active;

  /*to hold p0 active timers*/
  timer_group_type                p0_active;

  /* Slow clock timestamp of last write to SLEEP_XTAL_TIMETICK_MATCH */
  time_timetick_type                   set_time;

  /* Slow clock timestamp of last attempt to write SLEEP_XTAL_TIMETICK_MATCH */
  time_timetick_type                   last_set_time;

  /* Next timer expiration value */
  time_timetick_type                   set_value;

  /* Last timer expiration value */
  
  time_timetick_type					set_value_last;

  /* Last value written to SLEEP_XTAL_TIMETICK_MATCH */
  time_timetick_type                   match_value;

  /* Slow clock timestamp when timer_isr is called */
  time_timetick_type                   isr_time;

  /* Slow clock timestamp when timer_isr is called */
  time_timetick_type                   prev_learned_isr_time;

  /* Flag to indicate if timers_process_active_timers() is executing */
  boolean                         processing;

  /* Flag to indicate if timer_isr can be updated */
  boolean                         do_reprogram_isr;

  /* Active timer bin list */
  timer_bin_list_type             bins;

  /* Free timer bin list */
  timer_bin_list_type             bins_free;

  /* Pointer to the first non-deferrable timer */
  timer_ptr_internal_type         first_non_deferrable;

  /* DAL Device Handle */
  DalDeviceHandle*                dal_int_handle;

  /* Backoff time */
  uint32						  backoff;

}
timers_type;


static timers_type                timers =
{
  /* Faux timer group contain active timers & list of timer groups */
  {
    /* Active timers */
    {
      /* Slow clock "zero" base count for active timers list */
      0,

      /* Linked list of active timers */
      NULL
    },

    /* Disabled flag for the Faux Active Timer group.
       This group is always enabled.  When timers are disabled, they are moved
       off of this list */
    FALSE,

    /* Deferrable Flag */
    FALSE,

    /* Linked list of timer groups which have been disabled at some time. */
    &timers.active
  },

  /* Faux timer group contain p0 active timers & list of timer groups */
  {
    /* Active timers */
    {
      /* Slow clock "zero" base count for active timers list */
      0,

      /* Linked list of p0 active timers */
      NULL
    },

    /* Disabled flag for the Faux Active Timer group.
       This group is always enabled.  When timers are disabled, they are moved
       off of this list */
    FALSE,

    /* Deferrable Flag */
    FALSE,

    /* Linked list of timer groups which have been disabled at some time. */
    &timers.p0_active
  },

  /* Timestamp when call to _set_next_interrupt updates match_value */
  0,

  /* Timestamp of last attempt to update match value */
  0,

  /* Value passed to _set_next_interrupt */
  0,

  /* Last timer expiration value */
  
  0,

  /* Value written to match count */
  0,

  /* Timestamp when timer isr occurred at */
  0,

    /* Previous learned timestamp when timer isr occurred at */
  0,

  /* timers_process_active_timers is not executing */
  FALSE,

  /* Allow timer_isr to be set */
  TRUE,

  /* Active timer bin list */
  {
    /* First bin in active bin list */
    NULL,

    /* Last bin in active bin list */
    NULL,

    /* Number of bins in list */
    0
  },

  /* Free timer bin list */
  {
    /* First bin in free bin list */
    NULL,

    /* Last bin in free bin list */
    NULL,

    /* Number of bins in list */
    0
  },

  /* First Non-Deferrable Timer Pointer */
/* Timer Internal Pointer */
  NULL,

  /* Dal Device Handle */
  NULL,
  
  /* Backoff time */
  0

};

/* Max elements in the timer array. */
#define MAX_TIMER_BUFFER_NUMBER   256
#define MIN_TIMER_BUFFER_NUMBER    -1

/* Invalid Handle */
#define TIMER_BUFFER_INVALID_HANDLE   0xDEADDEAD
#define TIMER_HANDLE_CONSTANT 0xC3C3

inline uint32 timer_index_to_handle(uint32 timer_idx)
{
   uint32 handle;
   handle = ((timer_idx & 0xFFFF) ^ TIMER_HANDLE_CONSTANT)  << 16 | (timer_idx & 0xFFFF);
   return handle;
}

#define INDEX_TO_HANDLE(index)   (timer_index_to_handle(index))

inline boolean timer_handle_to_index(uint32 handle, uint32 *timer_idx)
{
   uint16 temp, tid;

   if(timer_idx == NULL)
      return FALSE;
   
   temp = (handle >> 16) & 0xFFFF;
   tid  = handle & 0xFFFF;
   
   if((tid ^ TIMER_HANDLE_CONSTANT) != temp)
      return FALSE;
	  
   *timer_idx = tid;
   return TRUE;
}

#define HANDLE_TO_INDEX(handle,index_ptr)   (timer_handle_to_index(handle,index_ptr))

/* Buffer of timer_internal_type */
static timer_internal_type timer_buffer[MAX_TIMER_BUFFER_NUMBER];

/* Queue to store free timer internal strcuture */
static q_type                     timer_free_q;

/* Indicated if timer_free_q in initialized */ 
static uint8 timer_q_init_status = 0;
static uint32 timer_while_cnt = 0;

static DalDeviceHandle *hTimerHandle = NULL;

static rex_tcb_type *timer_tcb_ptr = NULL;

static int timer_defer_undefer_reference_count =0;

volatile uint32 timer_jtag_checks_disable = 0;

/* Notification functions for indicating non-defer timer expiry changes */
typedef struct
{
  timer_notify_type         notify_type;
  time_osal_notify_obj_ptr  notify_obj;
  time_osal_notify_data     notify_mask;
} timer_non_defer_notification_client_type;

#define TIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT 4
static timer_non_defer_notification_client_type timer_non_defer_expiry_notify_clients[TIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT];
static uint32 timer_non_defer_expiry_notify_clients_cnt = 0;

/*-----------------------------------------------------------------------------
  The NULL timer group, for timers not in any other group
-----------------------------------------------------------------------------*/

/* Timers declared to be in the NULL group are reparented to this group */
timer_group_type                  timer_null_group;

/* Global Non-deferrable group for various clients use */
timer_group_type  				  timer_non_defer_group;

timer_group_type                  timer_prio0_group;
/*-----------------------------------------------------------------------------
  Timer trace data
-----------------------------------------------------------------------------*/

/* Stores number of timers present currently in  the active list */
static uint32 timer_active_timers_count = 0;

/* Stores number of timers present in timer_buffer*/
static uint32 timer_count_buffer_allocated = 0;

/*Stores number of paused timers*/
static uint32 timer_count_paused_timers = 0;

/* Stores the static function pointers for timer dependencies */
static timer_fn_map_fp_type timer_fn_map;

#define TIMER_UNDEFINED_DURATION 0xFFFFFFFFFFFFFFFFuLL


static void timer_active_list_check(void);

/**
Post initialization routine for timer
*/
void timer_post_init(void);


/*For ARM9*/
#ifndef FEATURE_TIME_QDSP6
#define ATS_INTLOCK() INTLOCK()
#define ATS_INTFREE() INTFREE()

#define ATS_DECLARE_ISAV(X) dword X = 0
#define ATS_INTLOCK_SAV(x) INTLOCK_SAV(x)
#define ATS_INTFREE_SAV(x) INTFREE_SAV(x)      

#define TIMER_PC_MASK_MUTEX_LOCK() INTLOCK()
#define TIMER_PC_MASK_MUTEX_UNLOCK() INTFREE()
#define TIMER_OS_EARLY_TOL 0

#define TIMER_MAX_EXPIRY 0xFFFFFF00000000uLL

#else /*else for Q6*/

#define TIMER_QURT_SIG_MASK 0x2

#define TIMER_SLEEP_CB_SIG_MASK 0x80000000

/*registration id obtained for timer signal registered with QURT. This
 id will be used by timetick to program the match values for timers*/
static int timer_qurt_sig_registration_id = -1;

/*Timer signal registered with QURT. QURT will set this signal to
 TIMER_QURT_SIG_MASK when the match value == timetick value*/
qurt_anysignal_t        timer_q6_signal;

/* Pointer to the mutex object for protecting critical-section of timer 
   code*/
static qurt_mutex_t timer_qemu_mutex;
static qurt_mutex_t timer_pc_mask_mutex;

/*function protect the critical section of timer code*/
void timer_pimutex_init(void);
void timer_pc_mutex_init(void);
/*MACRO for locking/unlocking the pimutex*/
#define ATS_INTLOCK()  \
  qurt_pimutex_lock(&timer_qemu_mutex); \
  
#define ATS_INTFREE()  \
  qurt_pimutex_unlock(&timer_qemu_mutex); \

#define ATS_DECLARE_ISAV(X)

#define ATS_INTLOCK_SAV(X) \
  qurt_pimutex_lock(&timer_qemu_mutex); \
  
#define ATS_INTFREE_SAV(X)   \
  qurt_pimutex_unlock(&timer_qemu_mutex); \

#define TIMER_PC_MASK_MUTEX_LOCK()  \
  qurt_pimutex_lock(&timer_pc_mask_mutex); \

#define TIMER_PC_MASK_MUTEX_UNLOCK()  \
  qurt_pimutex_unlock(&timer_pc_mask_mutex); \

#define TIMER_OS_EARLY_TOL TIMER_MAX_BACKOFF

#define TIMER_MAX_EXPIRY QURT_TIMER_MAX_DURATION_TICKS

#endif /*FEATURE_TIME_QDSP6*/

/* Dalsys Macros */
#define timer_dalsys_sig_init(h, obj) \
{ \
  if(DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT, \
                           h, \
                           obj)) \
      ERR_FATAL("timer dalsys sig obj failed", 0, 0, 0); \
\
} \

#define timer_dalsys_sig_set(h) \
   DALSYS_EventCtrl((DALSYSEventHandle)(*(DALSYSEventHandle *)h), DALSYS_EVENT_CTRL_TRIGGER)

#define timer_dalsys_sig_wait(h) \
   DALSYS_EventWait(h);

#define timer_dalsys_sig_reset(h) \
   DALSYS_EventCtrl((DALSYSEventHandle)(*(DALSYSEventHandle *)h), DALSYS_EVENT_CTRL_RESET)

/*These are timer debug variables that were useful to catch many 
 bugs. As of now they are featurized, so save space*/
#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES


#endif /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES*/


/*These values are set high to avoid the annoying F3 messages
 and an error fatal in case cb processing time is high. 
 1 sec = 32768 ticks. 
 Therefore, 32 ticks = 1 msec 
 */
volatile static uint64 timer_processing_time_warning_threshold = 19200;

#ifndef TIMER_ERROR_FATAL_THRESHOLD_ZEROED_OUT
volatile static uint64 timer_processing_time_err_fatal_threshold=38400000;
#else
volatile static uint64 timer_processing_time_err_fatal_threshold=0;
#endif

#ifdef FEATURE_TIMER_USE_QURT_SYSCLOCK
/*this function will be called in Q6 for updating the timer
 variables*/
static void timer_update_isr_time
(
  void
);
#endif /* FEATURE_TIMER_USE_QURT_SYSCLOCK */

static void timer_get_timetick_handle(void);

static void timer_init_qurt_signal(void);

static boolean timer_slave_processing_enabled = FALSE; 

static int timer_after_pc_mask[5]={0,0,0,0,0};
#define TIMER_PC_MASK_INDEX 0
#define TIMER_PC_DEFER_UNDEFER_CNT_INDEX 4
static int timer_cnt_cpu_max=0;
static int timer_cnt_cpu_down=0;

static rlc_handle_ptr rlc_handle = NULL;
static rlc_external_attribute_type rlc_attribute_learn;
static rlc_external_attribute_type rlc_attribute_correct;
/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

static void timer_set_backoff(uint32 cluster)
{	uint32 num=0;
	num=rlc_get_cluster_location(rlc_attribute_correct, cluster, 0);
	ATS_INTLOCK();
	if(num > TIMER_MAX_BACKOFF)
	{
		timers.backoff = TIMER_MAX_BACKOFF;
	}
	else
	{
		timers.backoff = num;
	}
	ATS_INTFREE();
    //ATS_ERR_1("ATS: New backoff: %d\n", timers.backoff);
}


void timer_reset_check_pc_mask_if_set(unsigned int index, unsigned int decrease_count)
{

  TIMER_PC_MASK_MUTEX_LOCK();
 
  if( timer_after_pc_mask[index] > 0)
  {
    timer_after_pc_mask[index]= timer_after_pc_mask[index]-decrease_count;
    if( (timer_after_pc_mask[0] <= 0) &&
        (timer_after_pc_mask[1] <= 0) &&
        (timer_after_pc_mask[2] <= 0) &&
        (timer_after_pc_mask[3] <= 0) )
    {
      // Release the cpu, expected to be executed once per powercollapse.
      if( timer_fn_map.timer_npa_clk_client )
      { /* client/resource exists so use can make MAX requests to the clock */
        if(timer_fn_map.timer_npa_complete_request)
        {  
          timer_fn_map.timer_npa_complete_request(timer_fn_map.timer_npa_clk_client);
        }
        timer_cnt_cpu_down++;
      }else
      {
        ATS_ERR("NPA Client is null for Timer to bring down the cpu");
      }

    }
  }
  TIMER_PC_MASK_MUTEX_UNLOCK();
}

static void timer_reset_check_pc_mask(unsigned int index)
{

  TIMER_PC_MASK_MUTEX_LOCK();
  timer_after_pc_mask[index]--;
  if( (timer_after_pc_mask[0] <= 0) &&
      (timer_after_pc_mask[1] <= 0) &&
      (timer_after_pc_mask[2] <= 0) &&
      (timer_after_pc_mask[3] <= 0) )
  {
    // Release the cpu, expected to be executed once per powercollapse.
    if( timer_fn_map.timer_npa_clk_client )
    { /* client/resource exists so use can make MAX requests to the clock */
      if( timer_fn_map.timer_npa_complete_request )
      {
        timer_fn_map.timer_npa_complete_request(timer_fn_map.timer_npa_clk_client);
      }
      timer_cnt_cpu_down++;
    }else
    {
      ATS_ERR("NPA Client is null for Timer to bring down the cpu");
    }

  }
  
  TIMER_PC_MASK_MUTEX_UNLOCK();
}

void timer_clk_cpu_callback( void *context,
                             unsigned int event_type, 
                             void *data,       
                             unsigned int data_size)
{
  npa_query_type queryResult;
  uint32 voltage_mode = VCS_CORNER_LOW_MINUS;

  if (NULL != timer_fn_map.timer_npa_clk_client)
  { /* client already created */
    return;
  }

  for (;voltage_mode < VCS_CORNER_NUM_OF_CORNERS; voltage_mode++) 
  {
    if(NPA_QUERY_SUCCESS == npa_query_by_name("/clk/cpu",
  	                                  CLOCK_NPA_QUERY_CPU_MAX_KHZ_AT_CORNER + voltage_mode,
  	                                  &queryResult) )
  	{
	  /* Update the time_fn_map structure with the updated SVS MAX freq */
      timer_fn_map.timer_npa_max_state = queryResult.data.value;  
      break;
    }
      
  }


  /* reached the max and still could not find a valid voltage mode */
  if ( VCS_CORNER_NUM_OF_CORNERS == voltage_mode )
  {
  	  ATS_ERR("NPA failed to get the MAX frequncy in SVS mode");
  	}
  else
    {
    /*only if a valid voltage mode is found then try to create a client*/
  if( timer_fn_map.timer_npa_create_sync_client )
  {
    timer_fn_map.timer_npa_clk_client = 
        timer_fn_map.timer_npa_create_sync_client( "/clk/cpu", 
                                                   "timer_clk_client",
                                                   timer_fn_map.timer_npa_client_required,
                                                   0,
                                                   NULL );
  }

  if (NULL == timer_fn_map.timer_npa_clk_client)
    { 
      /* client creation failed */
    ATS_ERR("NPA failed creation of timer_clk_client for Timer");
  }

  }/*end if-else*/
}

void timer_register_npa_client(void)
{
  if( timer_fn_map.timer_npa_resource_available_cb )
  {
    timer_fn_map.timer_npa_resource_available_cb( "/clk/cpu",
                                                  timer_clk_cpu_callback,
                                                  NULL );
  }
}


/*=============================================================================

FUNCTION TIMER_GET_INTERNAL_TIMER

DESCRIPTION
  This function returns pointer to internal timer.

DEPENDENCIES
  None.

RETURN VALUE
  Returns relevant pointer to internal timer structure. 
  If handle is invalid returns NULL.

SIDE EFFECTS
  This function should be called inside an intlock or a mutex.

=============================================================================*/

static timer_ptr_internal_type timer_get_internal_timer
(
  timer_ptr_type   timer
)
{
  int32 temp_index;
  timer_ptr_internal_type temp_int = NULL;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( (timer->timer_handle == 0) || 
       (timer->timer_handle == TIMER_BUFFER_INVALID_HANDLE) )
	{
	  return NULL;
	}

  if ( FALSE == HANDLE_TO_INDEX((uint32)timer->timer_handle, (uint32 *)&temp_index) )
  {
	  return NULL;
  }

  if ( temp_index > MIN_TIMER_BUFFER_NUMBER
       && temp_index < MAX_TIMER_BUFFER_NUMBER )
  {
    temp_int = &(timer_buffer[temp_index]);    
  }
  return temp_int;

} /* timer_get_internal_timer */


/*=============================================================================

FUNCTION TIMER_CONV_TO_SCLK_FREQ

DESCRIPTION
  This function returns 32kHz timetick value based on 19MHz timetick value.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 32kHz timetick value. 

SIDE EFFECTS
  None.

=============================================================================*/

static inline timetick_type timer_conv_to_sclk_freq(uint64 time_in)
{
  return (timetick_type)(time_in/XO_TO_SCLK_CONV_FACTOR);
} /* timer_conv_to_sclk_freq */


/*=============================================================================

FUNCTION TIMER_CONV_TO_XO_FREQ

DESCRIPTION
  This function returns 19MHz timetick value based on 32kHz timetick value.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 19MHz timetick value. 

SIDE EFFECTS
  None.

=============================================================================*/

static inline uint64 timer_conv_to_xo_freq(timetick_type time_in)
{
  return ((uint64)time_in)*((uint64)XO_TO_SCLK_CONV_FACTOR);
} /* timer_conv_to_xo_freq */


/*=============================================================================

FUNCTION TIMER_NOTIFY_FIRST_NON_DEFER_TIMER_EXPIRY_CHANGE

DESCRIPTION
  Sets the signal to clients' sig objs to notify the changed first non-deferrable timer 
  expiry

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/

static void timer_notify_first_non_defer_timer_expiry_change(void)
{
    int i;
    timer_notify_type         notify_type;
    time_osal_notify_obj_ptr  notify_obj;
    time_osal_notify_data     notify_mask;

    for(i=0; i<timer_non_defer_expiry_notify_clients_cnt; i++)
    {
    
      notify_type = timer_non_defer_expiry_notify_clients[i].notify_type;
      notify_obj = timer_non_defer_expiry_notify_clients[i].notify_obj;
      notify_mask = timer_non_defer_expiry_notify_clients[i].notify_mask;
      
      if(notify_obj == NULL)
         ERR_FATAL("Notify_obj NULL means corruption happened; i=%d", i, 0, 0);
      
      /* If there are signals to be set, and a task / signal object to set the signals to,
     then set those signals in that task's task control block or the passed signal object */

      if ( notify_type & TIMER_NATIVE_OS_SIGNAL_TYPE )
       {
         #ifdef FEATURE_TIME_QDSP6
         qurt_anysignal_set((qurt_anysignal_t *)notify_obj, notify_mask);
         #else
          ERR_FATAL("Signal type cannot be NATIVE OS type ; timer = 0x%x",timer->timer_ext,0,0);
         #endif /* FEATURE_TIME_QDSP6 */
       }
      else if ( notify_type & TIMER_REX_TCB_SIGNAL_TYPE )
      {
        #ifdef FEATURE_TIMER_REX_IS_AVAILABLE
          (void) rex_set_sigs( notify_obj, notify_mask );
        #else
          ERR_FATAL("Signal type cannot be rex type ; timer = 0x%x",timer->timer_ext,0,0);
        #endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
      }
      else if ( notify_type & TIMER_NATIVE_OS_SIGNAL2_TYPE )
      {
      #ifdef FEATURE_TIME_QDSP6
            qurt_signal2_set((qurt_signal2_t *)notify_obj, notify_mask);
      #else
             ERR_FATAL("Signal type cannot be NATIVE OS type ; timer = 0x%x",timer->timer_ext,0,0);
      #endif /* FEATURE_TIME_QDSP6 */
      }
      else if ( notify_type & TIMER_DALSYS_EVENT_SIGNAL_TYPE )
      {
          timer_dalsys_sig_set(notify_obj);
      }
      else if (notify_type & ~TIMER_NOTIFICATION_CHECK_MASK)
      {
         ERR_FATAL("Invalid Notify type 0x%x, may be corruption",notify_type,0,0);
      }
    } /* for() */

    return;
} /* timer_notify_first_non_defer_timer_expiry_change */


/*=============================================================================

FUNCTION TIMER_UPDATE_FIRST_ACTIVE_NON_DEFERRABLE_TIMER

DESCRIPTION
  This function returns the first non-deferrable timer on the active
  timer list.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the pointer to the first non-deferrable internal timer.
  If there are no non-deferrable timers on the active list, the return NULL. 

SIDE EFFECTS
  None.

=============================================================================*/

static void timer_update_first_active_non_deferrable_timer(void)
{
  timer_ptr_internal_type ptimer;
  timer_ptr_internal_type ptimer_p0 = timers.p0_active.list.first;
  time_timetick_type p1_expiry = TIMER_MAX_EXPIRY;
  time_timetick_type p0_expiry = TIMER_MAX_EXPIRY;
  time_timetick_type old_expiry = TIMER_MAX_EXPIRY;
  time_timetick_type new_expiry = TIMER_MAX_EXPIRY;

  if(timers.first_non_deferrable)
     old_expiry = timers.first_non_deferrable->expiry;
  
  if (ptimer_p0)
  {
    p0_expiry = ptimer_p0->expiry;
  }

  for( ptimer = timers.active.list.first; ptimer != NULL; ptimer = ptimer->next )
  {
    /* If timer belongs to a group, which is non-deferrable */
    if (ptimer->group->deferrable == FALSE )
    {
      /* Found the first non-deferrable timer on the active list */
      p1_expiry = ptimer->expiry;
      break;
    }
  }

  /* If there was a non-deferrable timer on the active list, then check the expiry value */
  if (p0_expiry == TIMER_MAX_EXPIRY &&
      p1_expiry == TIMER_MAX_EXPIRY)
  {
    /* There are no active non-deferrable timers */
    timers.first_non_deferrable = NULL;
  }
  else if (p1_expiry < p0_expiry)
  {
    /* First non-deferrable timer is on the active list */
    timers.first_non_deferrable = ptimer;
    new_expiry = p1_expiry;
  }
  else
  {
    /* First non-deferrable timer is on the p0 active list */
    timers.first_non_deferrable = ptimer_p0;
    new_expiry = p0_expiry;
  }
  
  if(old_expiry != new_expiry)
     timer_notify_first_non_defer_timer_expiry_change();
  
} /* timer_update_first_active_non_deferrable_timer */


/*=============================================================================

FUNCTION TIMER_TASK_ISR

DESCRIPTION
  This function detects a timer interrupt, and forwards it to the timer task
  for processing.

DEPENDENCIES
  Must only be called from tramp.

RETURN VALUE
  NULL pointer (No return value).

SIDE EFFECTS
  Uninstalls the timer ISR.
  Signals the timer task to process the expiring timer, which can have
  numerous side effects, depending on the expiring timer.

=============================================================================*/
static const void * timer_task_isr( void* input )
{
  /* Timer sclk time-stamp values */
  #ifdef TIMER_DEBUG_LATE_TIMER_ISR
  time_timetick_type                   set_time, set_value;

  /* Latency in ISR handling */
  time_timetick_type                   latency_sclk;
  #endif /*TIMER_DEBUG_LATE_TIMER_ISR*/

  time_timetick_type                   now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Disable interrupt since QTMR irq remains high */
  /* We need to disable the interrupt before we signal in order to avoid race cond'n */
  DalTimetick_Enable(hTimerHandle,0);

  #ifdef FEATURE_TIME_QDSP6
  /* Signal the timer task of the timer interrupt event */
  (void) qurt_anysignal_set(&timer_q6_signal, TIMER_QURT_SIG_MASK);
  #else /*FEATURE_TIME_QDSP6*/
  /* Signal the timer task of the timer interrupt event */
  (void) rex_set_sigs( timer_tcb_ptr, TIMER_EXPIRY_SIG );
  #endif /*FEATURE_TIME_QDSP6*/

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  ATS_INTLOCK( );

  /* Record when timer isr actually was handled */
  timers.isr_time = now;

  /* Capture timer set values, for messaging outside of ATS_INTLOCK. */
  #ifdef TIMER_DEBUG_LATE_TIMER_ISR
  set_time  = timers.set_time;
  set_value = timers.set_value;
  #endif /*TIMER_DEBUG_LATE_TIMER_ISR*/

  /* Set last set value */
  timers.set_value_last = timers.match_value;
  
  TIMER_TRACE_TS(ISR, now);
  ATS_INTFREE();

  #ifdef TIMER_DEBUG_LATE_TIMER_ISR
  if ( now - set_time >= set_value - set_time)
  {
    /* Determine the difference between when the interrupt was programmed to
       occur at, and when the interrupt was actually handled. */
    latency_sclk = now - set_value;

    if ( latency_sclk > TIMER_SCLK_LATENCY )
    {
      /* Timer interrupt was handled over btw 5 (38MHz) to 10ms (19MHz) late.
         (Timers may expire much later than 5-10ms, if they are in a disabled
         group, or if multiple timers expire due to the same interrupt.
         This is only a measure of the timer isr latency.) */

      ATS_MSG_ERROR("Late Timer ISR: ST=%d SV=%d IT-SV=%d",
                set_time, set_value, latency_sclk);
    }
  }
  else
  {
    ATS_MSG_HIGH("Early Timer ISR: ST=%d SV=%d SV-IT=%d",
             set_time, set_value, set_value - now);
  }
  #endif /*TIMER_DEBUG_LATE_TIMER_ISR*/

  return NULL;
} /* timer_task_isr */


/*=============================================================================

FUNCTION TIMER_SET_NEXT_INTERRUPT

DESCRIPTION
  Program the SLEEP_XTAL_TIMETICK to generate an interrupt at the given value

DEPENDENCIES
  Internal timer_ function.
  Must be called from ATS_INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt when slow clock counter reaches given value

=============================================================================*/

static void timer_set_next_interrupt
(
  /* Slow clock count at which next interrupt will occur */
  time_timetick_type                   match_count,

  /* Current slow clock count */
  time_timetick_type                   ticks_now,

  /* If TRUE, forces MATCH_VAL register to be updated */
  boolean                         force,

  uint8                           caller,

  timer_ptr_type                  head_timer
)
{                               

  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Don't re-write the same value to the MATCH_VAL register. 
    Only if the timer_undefer_reference_count is zero, it means
    that NPA is done with its job and we can start programming the interrupt.*/
  if ((force == TRUE || ( match_count != timers.set_value  &&  
         match_count != timers.match_value ))
       && timer_defer_undefer_reference_count == 0)       
  {

    timer_dbg_log_match_val( match_count, head_timer, ticks_now, caller );

    DalTimetick_SetNextInterrupt64(hTimerHandle,
                                   (DalTimetickTime64Type)match_count - (DalTimetickTime64Type)timers.backoff,
                                   (DalTimetickTime64Type)ticks_now,
                                   &timers.match_value);

    /* Featurize since this function is called in case of sysclock and without */  
    #ifndef FEATURE_TIMER_USE_QURT_SYSCLOCK
    /* Re-enable interrupts */
    DalTimetick_Enable(hTimerHandle,1);
    #endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/

    /* Save a trace packet with the match value */
    TIMER_TRACE_MV_TS(MATCH_VALUE_PROG, ticks_now, timers.match_value);     
    
    /* Record when this value was actually written */
    timers.set_time    = ticks_now ;

    /* Record when next timer should expire at */
    timers.set_value = match_count;
  }
  
  /* Record when this match value was logically written, even if we don't
     bother to write the value if it is unchanged. */
  timers.last_set_time = ticks_now ;

} /* timer_set_next_interrupt */


/*=============================================================================

FUNCTION TIMER_DETERMINE_NEXT_TIMER_EXPIRY

DESCRIPTION
  To determine which earliest timer from the two timer list.
  This will require us to iterate through the two list to check
  the earlier of the two timers and return that timer.

DEPENDENCIES
  This function should be called from critical section.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
time_timetick_type timer_determine_next_timer_expiry
(
  timer_ptr_type *head_timer
)
{
  /*initialize the next_timer_expiry to max value*/
  time_timetick_type first = TIMER_MAX_EXPIRY;
    
  if(timers.p0_active.list.first != NULL) 
  {
    first = timers.p0_active.list.first->expiry;
    *head_timer = timers.p0_active.list.first->timer_ext;
  }

  if ( timers.active.list.first != NULL )
  {
    if(timers.active.list.first->expiry < first) 
    {
      first = timers.active.list.first->expiry;
      *head_timer = timers.active.list.first->timer_ext;
    }   
  }
  return first;
}


/*=============================================================================

FUNCTION TIMER_UPDATE_TIMER_INTERRUPT

DESCRIPTION
  If the head of the timer list is modified, update the timer match value.
  Will cause an interrupt to be generated immediately if there is an
  expired timer.

DEPENDENCIES
  Internal timer_ function.
  Must be called from ATS_INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt when slow clock counter reaches the first timer's expiry point

=============================================================================*/

static void timer_update_timer_interrupt
(
  uint8 caller
)
{
  /* Current slow clock count */
  time_timetick_type                   now = 0;

  /* Time of first expiring timer */
  time_timetick_type                   first=TIMER_MAX_EXPIRY;
  timer_ptr_type                       head_timer=NULL;

  /* Minimum re-arm provided by DalTimetick call */
  DalTimetickTime32Type                minReArm;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* NOTE: This function has many early returns. */

  /* Skip updating the interrupts if not allowed */
  if ( FALSE == timers.do_reprogram_isr )
  {
    return;
  }

  /* If a timer is being altered inside "timer_process_active_timers" ... */
  if ( timers.processing )
  {
    /* ... don't bother updating the timer match value!  Caller will. */
    return;
  }



  /* The following two tests deal with timers.set_value, which is the time
     of the Already Programmed timer interrupt, which may have no relation to
     the first timer on the active timer list. */


  /* Get the current time */
  DalTimetick_GetTimetick64(hTimerHandle, &now);


  /* If the timer interrupt has already occurred ... */
  if ( timers.set_value - timers.set_time <= now - timers.set_time )
  {
    /* ... let the Timer ISR/Task update the next timer match value */
    return;
  }


  /* If the timer interrupt is just a little in the future ... */
  DalTimetick_GetMinRearmSclk(hTimerHandle,&minReArm);
  if ( timers.set_value - now <= minReArm )
  {
    /* ... we can't stop it from been generated.  Don't try to. */
    return;
  }

  first = timer_determine_next_timer_expiry(&head_timer);

  /* If the first expiring timer matches the timer.set_value, ... */
  if ( first == timers.set_value )
  {
    /* ... then it is already properly programmed in TIMETICK_MATCH */
    return;
  }

  /* If the first expiring timer is still in the past ... */
  if ( first < now  )
  {
    /* Set the timer for "as soon as possible" (eg, "now") */
    first = now;
  }


  /* Set the next interrupt match value (unless it is unchanged) */
  timer_set_next_interrupt( first, now, FALSE, caller,head_timer );

} /* timer_update_timer_interrupt */


/*=============================================================================

                           TIMER BIN FUNCTIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIMER_BIN_LIST_INSERT_BETWEEN

DESCRIPTION
  Inserts a bin into the specified list between the two specified bin positions 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_list_insert_between
(
  /* Bin list to insert bin into */
  timer_bin_list_ptr              list, 

  /* Bin to insert in bin list */
  timer_bin_ptr                   bin,

  /* Which node bin should be inserted after */
  timer_bin_ptr                   ptr1,

  /* Which node bin should be inserted before */
  timer_bin_ptr                   ptr2
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update our bin's prev/next ptrs to point at correct bin structs */
  bin->prev_bin = ptr1; 
  bin->next_bin = ptr2;


  /* Update surrounding prev/next ptrs (if necessary) to point to our 
     newly inserted bin */

  if( ptr1 != NULL )
  {
    ptr1->next_bin = bin;
  }
  else
  {
    list->first = bin; /* we have a new start of list, update first ptr */
  } 

  if( ptr2 != NULL )
  {
    ptr2->prev_bin = bin;
  }
  else
  {
    list->last = bin; /* we have a new end of list, update last ptr */
  }

  list->count++;

} /* timer_bin_list_insert_between */



/*=============================================================================

FUNCTION TIMER_BIN_LIST_REMOVE

DESCRIPTION
  Removes a bin from the specified list

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The popped timer_bin_type

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_list_remove
(
  /* Bin list to removed bin from */
  timer_bin_list_ptr              list, 

  /* Bin to remove from bin list */
  timer_bin_ptr                   bin
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bin->prev_bin != NULL )
  {
    bin->prev_bin->next_bin = bin->next_bin;
  }
  else
  {
    /* This was the head, so update first ptr */
    list->first = bin->next_bin;
  }

  if( bin->next_bin != NULL )
  {
    bin->next_bin->prev_bin = bin->prev_bin;
  }
  else
  {
    /* This was the tail, so updated last ptr */
    list->last = bin->prev_bin;
  }

  /* Update list's count */
  list->count--;

} /* timer_bin_list_remove */



/*=============================================================================

FUNCTION TIMER_BIN_LIST_POP

DESCRIPTION
  Pops a bin off the head of the specified list and returns it 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The popped timer_bin_type

SIDE EFFECTS
  None

=============================================================================*/

static timer_bin_ptr timer_bin_list_pop
(
  /* Bin list to remove the head bin from */
  timer_bin_list_ptr              list
) 
{
  /* Bin at head of bin list */
  timer_bin_ptr                   bin = list->first;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bin != NULL )
  {
    timer_bin_list_remove( list, bin );
  }

  return bin;

} /* timer_bin_list_pop */



/*=============================================================================

FUNCTION TIMER_BIN_LIST_PUSH

DESCRIPTION
  Pushes a bin onto the head of the specified list 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_list_push
(
  /* Bin list to push bin onto */
  timer_bin_list_ptr              list, 

  /* Bin to push onto bin list */
  timer_bin_ptr                   bin
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  timer_bin_list_insert_between( list, bin, NULL, list->first );

} /* timer_bin_list_push */



/*=============================================================================

FUNCTION TIMER_BIN_CREATE_BETWEEN

DESCRIPTION
  Takes a new bin from free bin pool and adds it between the specified bins
  on the active bin list 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The created timer_bin_type

SIDE EFFECTS
  None

=============================================================================*/

static timer_bin_ptr timer_bin_create_between
(
  /* Bin to create new bin after */
  timer_bin_ptr                   bin1, 

  /* Bin to create new bin before */
  timer_bin_ptr                   bin2,

  /* Timer which will exist in the bin (used to define bin range) */
  timer_ptr_internal_type         timer
) 
{ 
  /* Newly created bin */
  timer_bin_ptr                   new_bin;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Ensure we have a free bin in the pool */
  ASSERT( timers.bins_free.count > 0 );

  /* Pop it off */
  new_bin = timer_bin_list_pop( &timers.bins_free );

  /* Initialize the bin's info */
  new_bin->first  = NULL;
  new_bin->last   = NULL;
  new_bin->count  = 0;

  /* Determine the range of this new bin */
  new_bin->lower = timer->expiry & ~(TIMER_BIN_RANGE); 
  new_bin->upper = new_bin->lower  |  (TIMER_BIN_RANGE); 

  /* finally, compare against prev and next bins to ensure no overlap */

  if( bin1 != NULL )
  {
    /* is the bottom portion of the bin overlapping? */
    if( bin1->upper >= new_bin->lower )
    {
      /* yes, use the upper bound of the prev bin as our upper bound */
      new_bin->lower = bin1->upper + 1;
    }
  }
  if( bin2 != NULL )
  {
    /* is the top portion of the bin overlapping? */
    if( bin2->lower <= new_bin->upper )
    {
      /* yes, use the lower bound of the next bin as our upper bound */
      new_bin->upper = bin2->lower - 1;
    }
  }
        
  timer_bin_list_insert_between( &timers.bins, new_bin, bin1, bin2 );  

  return new_bin;

} /* timer_bin_create_between */



/*=============================================================================

FUNCTION TIMER_BIN_FIND_CREATE

DESCRIPTION
  Searches the active bin list for a bin corresponding to the specified timer  
  If none exists, create it and link it into the bin list

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The timer_bin_type whose range the timer fits into  

SIDE EFFECTS
  None

=============================================================================*/

static timer_bin_ptr timer_bin_find_create
( 
  /* Timer for which a bin is needed */
  timer_ptr_internal_type        timer
)
{
  /* Pointer to a bin.  Used for searching the list of active timer bins */
  timer_bin_ptr                   bin;

  /* Timer's expiry. */
  time_timetick_type                   expiry;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* NOTE: This function has many early returns. */


  ASSERT( timer->list != NULL );

  bin    = timers.bins.first;
  expiry = timer->expiry; 

  while( bin != NULL )
  {
    /* Is this bin able to hold this timer? */

    if ( expiry >= bin->lower  &&  expiry <= bin->upper )
    {
      /* Timer fits in this bin, and it doesn't straddle zero, return it */
      return bin;
    }


    /* Should we create a new bin before this bin? */

    if( expiry < bin->lower )
    {
      /* If it fits between this bin and the prev, or if the prev is NULL
         then create one */

      if( bin->prev_bin == NULL  ||  expiry > bin->prev_bin->upper )
      {
        return timer_bin_create_between( bin->prev_bin, bin, timer );
      }
    }

    /* Doesnt fit in this bin, or before this bin, so iterate to next */
    bin = bin->next_bin;

  } /* while( bin != NULL ) */


  /* If we've gotten to the end, create a bin after the last bin */
  return timer_bin_create_between( timers.bins.last, NULL, timer );

} /* timer_bin_find_create */



/*=============================================================================

FUNCTION TIMER_BIN_FIND_FIRST_EXCEEDING

DESCRIPTION
  Returns the first timer on the bin list exceeding the specified 
  timer's range. If none are found, NULL is returned 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The timer meeting the description's criteria.  

SIDE EFFECTS
  None

=============================================================================*/

static timer_ptr_internal_type timer_bin_find_first_exceeding
(
  /* Timer used to determine bin range */
  timer_ptr_internal_type         timer
)
{
  /* First timer in bins */
  timer_ptr_internal_type         first;

  /* Timer bin under consideration */
  timer_bin_ptr                   bin;

  /* Expiry of timer under consideration */
  time_timetick_type                   expiry = timer->expiry;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* NOTE: This function has many early returns. */


  /* Search the bins until we find one with a timer exceeding this timer's
     expiry */

  /* Skip passed all the unbinned expired timers at the head, if any */

  /*For priority timers, do not insert the timer into the 
  bin. Just return NULL*/
  if(TIMER_PRIORITY_0 == timer->timer_priority) 
  {
    return NULL;
  }
  else
  {
    first   = timers.active.list.first;
  }

  while( first != NULL )
  {
    if( first->bin != NULL )
    {
      break;
    }
    first = first->next;
  }

  /* Start with the bin associated with the first on the active list */
  if( first == NULL )
  {
    return NULL;
  }

  for( bin = first->bin;  bin != NULL;  bin = bin->next_bin )
  {
    /* Does this timer fit before the bin's upper bound? */

    if( expiry <= bin->upper )
    {
      /* return the first pointer */ 
      return bin->first;
    }
  }

  /* now continue at the beginning of the bin list and
     progress until we hit the bin of the first timer on the active list */
  for( bin = timers.bins.first; bin != first->bin; bin = bin->next_bin )
  {
    /* Does this timer fit before the bin's upper bound? */

    if( expiry  <= bin->upper )
    {
      /* return the first pointer */ 
      return bin->first;
    }
  }
 
  /* If we've gotten here, return NULL */
  return NULL;

} /* timer_bin_find_first_exceeding */



/*=============================================================================

FUNCTION TIMER_BIN_INSERT

DESCRIPTION
  Inserts the specified timer into the bin.
  This (potentially) involves updating the bin's first/last pointer.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_insert
(
  /* Bin to insert timer in */
  timer_bin_ptr                   bin,

  /* Timer to insert in bin */
  timer_ptr_internal_type         timer 
)
{
  /* Lowest expiry time in first bin */
  time_timetick_type                   bin_lowest;

  /* Highest expiry time in last bin */
  time_timetick_type                   bin_highest;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Since it is known that a bin does not straddle a counter loop,
     these compares can be done without a zero reference */


  /* Calculate the lowest timer expiry in the bin */

  if( bin->first != NULL )
  {
    bin_lowest  = bin->first->expiry;

    /* do we need to update the first pointer? */
    if( timer->expiry < bin_lowest )
    {
      /* We have a new first pointer */

      /* Note: if the expiries are equal, the first pointer remains the same
         to match the behavior of the timer list */ 
      bin->first = timer;
    } 

  }
  else
  {
    /* If no first pointer, update first pointer with new timer */
    bin->first = timer;
  } 

  /* Calculate the highest timer expiry in the bin */
  if( bin->last != NULL )
  {
    bin_highest = bin->last->expiry;

    /* Do we need to update the last pointer? */
    if( timer->expiry >= bin_highest )
    {
      /* We have a new last pointer */

      /* Note: if the expiries are equal, the last pointer is updated 
         to match the behavior of the timer list */ 
      bin->last = timer;
    }

  }
  else
  {
    /* If no last pointer, update last pointer with new timer */
    bin->last = timer;
  }

  /* Record the bin in the timer struct */
  timer->bin = bin; 

  /* Increment the bin's count */
  bin->count++;

} /* timer_bin_insert */



/*=============================================================================

FUNCTION TIMER_BIN_REMOVE

DESCRIPTION
  Remove the specified timer from the bin.
  This (potentially) involves updating the bin's first/last pointer.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_remove
(
  /* Timer to remove from bin list */
  timer_ptr_internal_type         timer
)
{
  /* Bin timer belongs to */
  timer_bin_ptr                   bin = timer->bin;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( bin != NULL );


  /* Do we now have an empty bin? */

  if( bin->count == 1 )
  {
    /* empty bin */
    bin->first = NULL;
    bin->last  = NULL;
  }
  else if( bin->count > 1 )
  {
    /* Check if we have a new bin first */
    if( timer == bin->first )
    { 
      bin->first = timer->next;
    }

    /* Check if we have a new bin last */ 
    if( timer == bin->last )
    {
      bin->last = timer->prev;
    }
  }

  /* Unbin the timer */ 
  timer->bin = NULL;

  /* Decrement the bin's count */
  bin->count--;  

  if( bin->count == 0 )
  {
    /* If this bin is now empty, then remove it from the active bin list
       and return it to the free list */
    timer_bin_list_remove( &timers.bins, bin );
    timer_bin_list_push( &timers.bins_free, bin);
  }

} /* timer_bin_remove */



/*=============================================================================

FUNCTION TIMER_INSERT_BETWEEN

DESCRIPTION
  Places 'timer' before 'ptr1' and after 'ptr2' in 'list'.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void timer_insert_between
(
  /* List to insert timer into */
  timer_list_ptr                  list,

  /* Timer to insert into list */
  timer_ptr_internal_type         timer, 

  /* Timer to insert after */
  timer_ptr_internal_type         ptr1,

  /* Timer to insert before */
  timer_ptr_internal_type         ptr2
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update our timer's prev/next ptrs to point at correct timer structs */

  timer->prev = ptr1; 
  timer->next = ptr2;


  /* Update surrounding prev/next ptrs (if necessary) to point to our 
     newly inserted timer */

  if( ptr1 != NULL )
  {
    ptr1->next = timer;
  }
  else
  {
    list->first = timer; /* We have a new start of list, update first ptr */
  } 

  if( ptr2 != NULL )
  {
    ptr2->prev = timer;
  }
  else
  {
    list->last = timer; /* We have a new end of list, update last ptr */
  }
 
} /* timer_insert_between */



/*=============================================================================

FUNCTION TIMER_INSERT

DESCRIPTION
  Inserts a timer into the sorted list of timers.
  Timers are sorted according to increasing expiration times

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_insert
(
  /* List timer is to be inserted into */
  timer_list_ptr                  list,

  /* Timer to be inserted into list of active timers */
  timer_ptr_internal_type         timer
)
{
  /* Pointer to a timer. Used for walking list of timers */
  timer_ptr_internal_type         ptr;

  /* Pointer to a timer. Determines where on the list to start looking */
  timer_ptr_internal_type         start=NULL;
  
  /* Current slow clock count */
  time_timetick_type                   now = 0;
  
  /* bin timetick bounds */
  time_timetick_type                   bin_lower, bin_upper;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  DalTimetick_GetTimetick64(hTimerHandle, &now);

  /* First set the list field of the timer */
  timer->list = list;

  /* Update last_start in ext_timer */
  DalTimetick_GetTimetick64(hTimerHandle, &timer->timer_ext->last_start);

  /* if this timer is to be inserted on the active list 
   For priority list, as of now there will be no binning*/
  if( timer->list == &timers.active.list)
  {

    /* If this timer is going on the active list we can use binning 
       to help us cut down on the list search time */

     /* Obtain the first timer from the bin exceeding this timer's expiry */
    start = timer_bin_find_first_exceeding( timer ); 

    /* Increment the number of timers */
    timer_active_timers_count++; 
  }
  else /*it can be disabled or p0 timer*/
  {
    /* If this is not an active timer, do not optimize the search,
       just search from the start of the list */
    start = list->first;

    /*increment the # of active timers if its a priority list*/
    if(timer->list == &timers.p0_active.list) 
    {
      timer_active_timers_count++;
    }
  }

  /*do  not attempt to find bin for priority timers*/
  if(start != NULL && start->bin != NULL)
  {
     /*Debug*/
     if(TIMER_PRIORITY_0 == timer->timer_priority) 
     {
       ERR_FATAL("1- Priority timers should not have been binned",0,0,0);
     }
     /*end debug*/

     bin_lower = start->bin->lower;
     bin_upper = start->bin->upper;
     
     /* Ignore the bin's first timer to insert new timer into timers list
        if the 'now is in between bin's upper and lower boundary' and first  
        timer of the bin is before now. In such case, Search from start of
        timers list to insert new timer. */
     if(now > bin_lower && now < bin_upper && start->expiry < now) 
        start = NULL;
  }
  
  /* Initialize the start position */
  if( start != NULL )
  {
    ptr = start;
  }
  else
  {
    ptr = list->first;
  }

  /* Search for appropriate list location to insert timer */
  while ( ptr != NULL &&  ptr->expiry <= timer->expiry )
  {
    ptr = ptr->next;
  }

  /* Insert the timer into the list */
  timer_insert_between( list, timer, ptr ? ptr->prev : list->last, ptr ); 

  /* Only bin this timer if we are inserting into the active list 
   Do not insert the priority timer into the bin*/
  if( timer->list == &timers.active.list)
  {
    /* Insert thie timer into the appropriate timer list bin */
    timer_bin_insert( timer_bin_find_create(timer), timer );
  }

  /* Update the first non-deferrable timer */
  if (timer->group->deferrable == 0 &&
      (timers.first_non_deferrable == NULL ||
       timers.first_non_deferrable->expiry > timer->expiry))
  {
    /* Set this timer as the first non-deferrable
       (regardless of if it's p0 or not) */
    timers.first_non_deferrable = timer;
    
    /* Notify the clients who are interested in non deferrable timer change */
    timer_notify_first_non_defer_timer_expiry_change();
  }

  /* Throw an error fatal if timer list is corrupted */
  timer_active_list_check();

  timer_dbg_log_timer_set( timer->timer_ext, timer, now, timer->expiry, rex_self() );

} /* timer_insert */



/*=============================================================================

FUNCTION TIMER_INSERT_LIST

DESCRIPTION
  Inserts a sorted list of timers into the sorted list of timers.
  Timers are sorted according to increasing expiration times

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_insert_list
(
  /* List timers are to be inserted into */
  timer_list_ptr                  dst_list,

  /* Linked list of timers to be inserted into the first list */
  timer_ptr_internal_type         src_list
)
{
  /* A timer from the second list, to be inserted in the first */
  timer_ptr_internal_type         timer;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Repeat for all timers on the source list */
  while ( src_list != NULL )
  {
    /* Retrieve a timer from the start of the source list */
    timer    = src_list;
    src_list = src_list->next;

    timer_insert( dst_list, timer );
  }

} /* timer_insert_list */



/*=============================================================================

FUNCTION TIMER_CONCATENATE_LISTS

DESCRIPTION
  Concatenates the src_list and the dst_list with the combined list taking on
  the identity of the dst_list.  src_list is cleared.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_concatenate_lists
(
  /* Linked list of timers to be inserted into the first list */
  timer_list_ptr                  src_list,

  /* List timers are to be inserted into */
  timer_list_ptr                  dst_list
)
{
  /* timer iteration pointer */
  timer_ptr_internal_type         timer;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( src_list != NULL );

  /* Check if the source list is empty */
  if( src_list->first == NULL )
  {
    return;
  }

  /* Stitch the two lists together */

  /* Is the destination list empty? */
  if( dst_list->first != NULL )
  { 
    /* No, so update the head's previous pointer */
    dst_list->first->prev = src_list->last;

    /* Update the source list's last next pointer */
    src_list->last->next = dst_list->first; 

  }
  else
  {
    /* Yes, so update the destination list's last pointer */
    dst_list->last = src_list->last;
  }

  /* Update the destination lists's first pointer */
  dst_list->first = src_list->first;
 
  /* Now, for each of the src list items, update their structures */
  timer = src_list->first;
  while( timer != NULL )
  {
    /* Update the list membership */
    timer->list = dst_list;

    /* If this is the last of the src list, we are done */
    if( timer == src_list->last )
    {
      break;
    }

    /* Proceed to next timer */
    timer = timer->next; 
  }

  /* Finally update the source list */
  src_list->first = NULL;
  src_list->last  = NULL;
    
} /* timer_concatenate_lists */



/*=============================================================================

FUNCTION TIMER_UNLINK

DESCRIPTION
  Simply unlinks a timer from its current list of timers.
  Updates the associated list first/last ptrs

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_unlink
(
  /* Timer to remove from its current list */
  timer_ptr_internal_type        timer
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( timer->prev != NULL )
  {
    timer->prev->next = timer->next;
  }
  else
  {
    /* This was the head, so update first ptr */
    timer->list->first = timer->next;
  }

  if( timer->next != NULL )
  {
    timer->next->prev = timer->prev;
  }
  else
  {
    /* This was the tail, so updated last ptr */
    timer->list->last = timer->prev;
  }

} /* timer_unlink */

/*=============================================================================

FUNCTION TIMER_REMOVE_INTERNAL_TIMER_FROM_Q

DESCRIPTION
  1. Puts the timer back in free q
  2. Sets the external timer handle to INVALID handle
  
DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_remove_internal_timer_from_q
(
  timer_ptr_internal_type timer  
)
{
   /* Cache Address of external timer */
  timer_ptr_type                 ext_timer;

  /* Temporary index */
  int32                          temp_index;

   if ( timer->internal_q == TIMER_IN_FREE_Q )
    ERR_FATAL ( " Removing timer_buffer which is not in the list, rextcb=0x%x timer_ext=0x%x",
                timer->tcb,timer->timer_ext,0);
      
   /* Get location of external timer */
   ext_timer = timer->timer_ext;

   if ( ext_timer != NULL )
   {
     ext_timer->timer_handle = TIMER_BUFFER_INVALID_HANDLE ;
   }
   else
   {
     ERR_FATAL ( " Timer must have an external timer, rextcb=0x%x timer_ext=0x%x timer_ptr=0x%x",
                timer->tcb,timer->timer_ext,timer);
   }

   /* Save index value in a teemp var */
   temp_index = timer->index;

   /* Clear expired timer buffer */
   memset( timer, 0, sizeof( timer_internal_type ));

   /* Restore timer index*/
   timer->index = temp_index ;

   timer->internal_q = TIMER_IN_FREE_Q;
  
   /* Link timer_buffer element back to timer_free_q */
   (void) q_link( timer, 
             &(timer->link) );

   /* Enqueue node in timer_free_q */
   q_put( &timer_free_q, &(timer->link) );

   timer_count_buffer_allocated = MAX_TIMER_BUFFER_NUMBER-q_cnt(&timer_free_q);
}

/*=============================================================================

FUNCTION TIMER_REMOVE

DESCRIPTION
  Removes a timer from the list of timers.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_remove
(
  /* Timer to be removed from list of active timers */
  timer_ptr_internal_type         timer,
  
  boolean                          remove_from_q
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( timer == NULL || timer->list == NULL )
  {
    return;
  }

  if( timer->list == &timers.active.list 
      || timer->list == &timers.p0_active.list )
  {
    /* Decrement the number of timers if timer belongs to active list */
    timer_active_timers_count--; 
  }

  /* if this timer is to be removed from the active list */
  if( timer->bin != NULL )
  {

    /*debug*/
    if(TIMER_PRIORITY_0 == timer->timer_priority) 
    {
      ERR_FATAL("2- Priority timers should not have been binned",0,0,0);
    }
    /*end debug*/

    /* Unbin this timer */
    timer_bin_remove( timer );
  }


  /* Fix up links/list around this node */

  if ( timer->prev )
  {
    timer->prev->next = timer->next;
  }
  else
  {
    timer->list->first = timer->next;
  }

  if ( timer->next )
  {
    timer->next->prev = timer->prev;
  }                                 
  else
  {
    timer->list->last = timer->prev;
  }


  timer->next = NULL;
  timer->prev = NULL;
  timer->list = NULL;

  /* Donot remove if timer is to be resumed */
  if(remove_from_q == TRUE)
  {
    timer_remove_internal_timer_from_q(timer);
  } /* remove_from_q == TRUE */

  /* Update the first non-deferrable timer */
  if (timers.first_non_deferrable == timer)
  {
    /* Since the first non-deferrable timer was removed,
       we need to get the next non-deferrable timer */
    timer_update_first_active_non_deferrable_timer();
  }

  /* Throw an error fatal if timer list is corrupted */
  timer_active_list_check();

} /* timer_remove */



/*=============================================================================

FUNCTION TIMER_REMOVE_HEAD

DESCRIPTION
  Removes the head of the specified timer list.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_remove_head
(
  /* List to chop head off of */
  timer_list_ptr                  list
)
{
  /* Head of the timer list */
  timer_ptr_internal_type         head = list->first;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( head->internal_q == TIMER_IN_FREE_Q )
    ERR_FATAL ( " Removing timer_buffer which is not in the list, rextcb=0x%x timer_ext=0x%x timer_ptr=0x%x",
                head->tcb, head->timer_ext, head);

  if( list == &timers.active.list
      || list == &timers.p0_active.list )
  {
    /* Decrement the number of timers if timer belongs to active list */
    timer_active_timers_count--; 
  }

  /* New front of list */
  list->first = head->next;

  /* Fix up links/list around this node */
  if( list->first != NULL )
  {
    /* Update next node's prev ptr */
    list->first->prev = NULL;
  }
  else
  {
    /* Empty list */
    list->last = NULL;
  }


  /* NULL out deleted node's link fields */
  head->next = NULL;
  head->list = NULL;

  /* Update the first non-deferrable timer */
  if (timers.first_non_deferrable == head)
  {
    /* Since the first non-deferrable timer was removed,
       we need to get the next non-deferrable timer */
    timer_update_first_active_non_deferrable_timer();
  }

  /* Throw an error fatal if timer list is corrupted */
  timer_active_list_check();

} /* timer_remove_head */



/*=============================================================================

FUNCTION TIMER_APPEND

DESCRIPTION
  Appends the list with a timer
  The appended timer's list member is updated

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_append
(
  /* List to add timer to */
  timer_list_ptr                  list,

  /* Timer to add to list */
  timer_ptr_internal_type         timer
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update timer's links */
  timer->prev = list->last;
  timer->next = NULL;

  /* Update the old tail to point to this new timer */
  if( list->last != NULL )
  {
    list->last->next = timer;
  }
  else
  {
    /* Empty list, have a new head */
    list->first = timer;
  } 
  
  /* Always have a new tail */ 
  list->last = timer;
 
  /* We have a new list for timer */ 
  timer->list = list;

} /* timer_append */



/*=============================================================================

FUNCTION TIMER_IN_LIST

DESCRIPTION
  Returns whether the timer actually exists in the specified list

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static boolean timer_in_list
(
  /* Timer under test */
  timer_ptr_internal_type         timer,

  /* List timer may be a member of */
  timer_list_ptr                  list
)
{
  /* Used to walk timer list */
  timer_ptr_internal_type         ptr; 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Do the search */
  for( ptr = list->first; ptr != NULL; ptr = ptr->next )
  {
    if( ptr == timer )
    {
      /* Timer is in list */
      return TRUE;
    }
  }

  /* Timer was not in list */
  return FALSE;

} /* timer_in_list */



/*=============================================================================

FUNCTION TIMER_EXPIRE

DESCRIPTION
  Processes an expiring timer, according to the timer's definition.

DEPENDENCIES
  Internal timer_ function()
  Must be called from inside ATS_INTLOCK & TASKLOCK (or interrupt) contexts

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

static void timer_expire
(
  /* Expiring timer to be processed */
  timer_ptr_internal_type         timer,

  /* Duration timer ran for. */
  time_timetick_type                   run_time
)
{
  /* Flag indicating if timer has been properly processed */
  boolean                         processed = FALSE;

  /* Duration timer ran for, in milliseconds */
  time_timetick_type                   run_time_ms = 0;

  /* Duration timer ran for, in milliseconds */
  time_timetick_type                   run_time_sclk;

  /* Timer calllback type */
  timer_notify_type               notify_type;

  /* Timer 'type 1' calllback function */
  timer_t1_cb_type                cb1;

  /* Timer 'type 2' calllback function */
  timer_t2_cb_type                cb2;

  /*to logs when the cb processing starts*/
  time_timetick_type cb_processing_started = 0;

  /*to log when the cb processing ends*/
  time_timetick_type cb_processing_ended = 0;

  /*to log the processing time*/
  time_timetick_type cb_processing_time = MAGIC_INITIALIZER_64;
  
  /*fn address*/
  uint32  fn_address_of_timer = 0;
  
  /* Last set value of timer */
  uint32 setlastval = 0;

  /* Last set value of timer */
  uint32 curr_latency = 0;

  /*fn callback type*/
  rlc_error_type rlc_error=RLC_SUCCESS;
  
  #ifdef FEATURE_TIMER_SLAVE1
  timer_slave_post_cmd_params_type slave_cmd;
  memset(&slave_cmd, 0, sizeof(timer_slave_post_cmd_params_type));
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Cache information from timer structure since it could get deleted any
  time after the first callback is made */
  notify_type = timer->notify_type;
  cb1 = timer->func1;
  cb2 = timer->func2;

  /* If a conversion from sclk to milliseconds is necessary, perform it now. */
  if ( notify_type & TIMER_FUNC2_CB_TYPE )
  {
    /* Compute time (in milliseconds) timer has been running for.
       This may be larger than expected if the timer expires during sleep,
       or otherwise has its expiry processing delayed */
    DalTimetick_CvtFromTimetick64(hTimerHandle,run_time,T_MSEC,&run_time_ms);

    /* If a timer is a reloading timer, the millisecond conversion will have
       a rounding error.  We can adjust the start time to account for the
       residual.  This is done before signalling the timer expiry, since
       the timer expiry might alter the timer data */

    if ( timer->list != NULL )
    {
      /* Timer is still on the list.  It is a reloading timer. */

      /* Difference btw run_time & run_time_sclk is rounding error (+/-.5ms) */
	  DalTimetick_CvtToTimetick64(hTimerHandle,run_time_ms,T_MSEC,&run_time_sclk);

      /* Nudge start time, so the next expiry includes this rounding error */
      if ( run_time >= run_time_sclk )
      {
        /* Rounded down: move start backwards, so next duration is longer */
        timer->start -= run_time - run_time_sclk;
      }
      else if ( timer->expiry - timer->start > run_time_sclk - run_time )
      {
        /* Rounded up: move start forward, so next duration is shorter */
        timer->start += run_time_sclk - run_time;
      }
      else
      {
        /* Rounded up, but next expiry is too close for rounding nudging. */
        timer->start = timer->expiry - 1;
      }
    }
  }

  /*log the time when processing started*/
  DalTimetick_GetTimetick64(hTimerHandle, &cb_processing_started);

  /* If there are signals to be set, and a task / signal object to set the signals to,
     then set those signals in that task's task control block or the passed signal object */

  if ( notify_type & TIMER_NATIVE_OS_SIGNAL_TYPE )
   {
     #ifdef FEATURE_TIME_QDSP6
     qurt_anysignal_set((qurt_anysignal_t *)timer->tcb, (time_osal_notify_data)timer->sigs);
	 #else
	  ERR_FATAL("Signal type cannot be NATIVE OS type ; timer = 0x%x",timer->timer_ext,0,0);
	 #endif /* FEATURE_TIME_QDSP6 */
   }
  else if ( notify_type & TIMER_REX_TCB_SIGNAL_TYPE )
  {
    #ifdef FEATURE_TIMER_REX_IS_AVAILABLE
      (void) rex_set_sigs( timer->tcb, timer->sigs );
	#else
	  ERR_FATAL("Signal type cannot be rex type ; timer = 0x%x",timer->timer_ext,0,0);
	#endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
  }
  else if ( notify_type & TIMER_NATIVE_OS_SIGNAL2_TYPE )
  {
  #ifdef FEATURE_TIME_QDSP6
		qurt_signal2_set((qurt_signal2_t *)timer->tcb, (time_osal_notify_data)timer->sigs);
  #else
		 ERR_FATAL("Signal type cannot be NATIVE OS type ; timer = 0x%x",timer->timer_ext,0,0);
  #endif /* FEATURE_TIME_QDSP6 */
  }
  else if (notify_type & TIMER_DALSYS_EVENT_SIGNAL_TYPE)
  {
      timer_dalsys_sig_set(timer->tcb);
  }
  
  /* If the timer was defined with a type-1 style function callback,
     call the function, or post the function as an asynchronous call */
  if ( notify_type & TIMER_FUNC1_CB_TYPE )
  {
    fn_address_of_timer = (uint32)cb1;
    
    if ( !processed )
    {
      /*queue it to slave task only if it's not priority timer*/
      if(TIMER_PRIORITY_0 != timer->timer_priority)
      { 
        /* Can't queue APC -- call it ourselves */
        #ifdef FEATURE_TIMER_SLAVE1
        slave_cmd.timer_ext = timer->timer_ext;
        slave_cmd.cb_type = TIMER_SLAVE_CB_TYPE1;
        slave_cmd.cb1 = cb1;
        slave_cmd.data = timer->data;
        slave_cmd.timer_slave_task_num = timer->slave_task_assigned;      
        if (1==timer_after_pc_mask[TIMER_PC_MASK_INDEX]) 
        {                                                
           TIMER_PC_MASK_MUTEX_LOCK();                    
           timer_after_pc_mask[slave_cmd.timer_slave_task_num+1]++;  
           TIMER_PC_MASK_MUTEX_UNLOCK();               
        }                                                  
        timer_slave_post_cmd(&slave_cmd);
        #else
        cb1( timer->data );
        #endif
      }
      else
      {
        cb1(timer->data);
      }
    }

    
  } /* timer->func1 != NULL */
  
  /* If the timer was defined with a type-2 style function callback,
     (clock callback style), call the function with the time (in
     milliseconds) between timer start and now. */
  if ( notify_type & TIMER_FUNC2_CB_TYPE )
  {
    fn_address_of_timer = (uint32)cb2;

    if(TIMER_PRIORITY_0 != timer->timer_priority) 
    {
    /* Call the callback function */
    #ifdef FEATURE_TIMER_SLAVE1
    slave_cmd.timer_ext = timer->timer_ext;
    slave_cmd.cb_type = TIMER_SLAVE_CB_TYPE2;
    slave_cmd.cb2 = cb2;
    slave_cmd.run_time = (time_timetick_type)run_time_ms;
    slave_cmd.data = timer->data;
    slave_cmd.timer_slave_task_num = timer->slave_task_assigned;
    if (1==timer_after_pc_mask[TIMER_PC_MASK_INDEX]) 
    {                                                
       TIMER_PC_MASK_MUTEX_LOCK();                  
       timer_after_pc_mask[slave_cmd.timer_slave_task_num+1]++;  
       TIMER_PC_MASK_MUTEX_UNLOCK();                
    }                                                  
    timer_slave_post_cmd(&slave_cmd);
    #else
    cb2( (time_timetick_type) run_time_ms, timer->data);
    #endif
  }
    else
    {
      cb2( (int32) run_time_ms, timer->data);
    }
  }

 /* Check if this timers has no notification types or if it is invalid */
 if (notify_type == TIMER_NO_NOTIFY_TYPE)
  	{
  	  // ATS_MSG_HIGH("Timer = 0x%x expired; External timer = 0x%x",timer,timer->timer_ext,0);
  	}
 else if (notify_type & ~TIMER_NOTIFICATION_CHECK_MASK)
  	{
  	  ERR_FATAL("Timer = 0x%x : INVALID NOTIFICATION TYPE = %d",timer,timer->notify_type,0);
  	}

  /*log the cb ended time*/
  DalTimetick_GetTimetick64(hTimerHandle, &cb_processing_ended);
  
  /*to take care of the wrap around case*/
  if(cb_processing_ended >= cb_processing_started)
  {
    cb_processing_time = cb_processing_ended - cb_processing_started;       
  }
  else /*if it is a wrap around case*/
  {  
    cb_processing_time = 0xFFFFFFFFFFFFFFuLL - 
                        (cb_processing_started - cb_processing_ended)+1;
  }

  /*generate a warning or error fatal out if the processing time > threshold*/
  if(cb_processing_time > timer_processing_time_warning_threshold)
  {
    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    ATS_MSG_ERROR("Processing of callback fn = 0x%x took 0x%x ticks, threshold = 0x%x ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_processing_time_warning_threshold);          
    #endif
    
    timer_dbg_log_timer_isolation( cb_processing_started,
                                   cb_processing_time,
                                   timer->data,
                                   timer->timer_ext,
                                   fn_address_of_timer,
                                   (uint8)timer->notify_type );
  }

  if(timer_processing_time_err_fatal_threshold != 0 && 
     cb_processing_time > timer_processing_time_err_fatal_threshold)
  {
    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    ERR_FATAL("Processing of callback fn = 0x%x took 0x%x ticks, threshold = 0x%x ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_processing_time_err_fatal_threshold);          
    #endif
    
    timer_dbg_log_timer_isolation( cb_processing_started,
                                   cb_processing_time,
                                   timer->data,
                                   timer->timer_ext,
                                   fn_address_of_timer,
                                   (uint8)timer->notify_type );
     }

  if(timers.prev_learned_isr_time != timers.isr_time)
  {
    ATS_INTLOCK();
	setlastval = timers.set_value_last;
	timers.prev_learned_isr_time = timers.isr_time;
	ATS_INTFREE();
	
	curr_latency = ( cb_processing_started - setlastval);
	
    if ( ( cb_processing_started >= setlastval ) &&
         ( ( timers.backoff == 0 ) ||
		   ( curr_latency > ( timers.backoff + ( timers.backoff / 8 ) ) ) ||
		   ( curr_latency < (timers.backoff - (timers.backoff / 8 ) ) ) ) )
    {
      rlc_error = rlc_learn_data(rlc_attribute_learn, RLC_LEARN_ASYNC_MODE | RLC_LEARN_SYNC_MODE, 1, cb_processing_started - setlastval); 
    
      if(rlc_error != RLC_SUCCESS && rlc_error != RLC_FULL && rlc_error != RLC_LEARNING)
      {
        ATS_ERR_1("Error returned from RLC! Error: 0x%x\n", rlc_error);
      }
    }
  }
	 
} /* timer_expire */

/*=============================================================================

FUNCTION TIMER_DETERMIN_TIMER_EXPIRY

DESCRIPTION
  Sets the timer expiry flag and determines whetehr the callback fn is to 
  be called or not

DEPENDENCIES
  Internal timer_ function()
  Must be called from inside ATS_INTLOCK & TASKLOCK (or interrupt) contexts

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static boolean timer_determine_timer_expiry
(
  timer_ptr_internal_type timer,
  time_timetick_type      timetick_now
)
{
  boolean expire_timer = FALSE;



  if ( timer->timer_ext != NULL )
  	{
     	/*Check for the integrity of the external timer state*/
     	if(0 == timer->timer_ext->timer_state
     	   || timer->timer_ext->timer_state < TIMER_DEF_FLAG
     	   || timer->timer_ext->timer_state > TIMER_RESUMED_FLAG)	 
     	{
     	  ERR_FATAL("External timer structure corrupted, state=0x%x timer_ext=0x%x",
     				  timer->timer_ext->timer_state,timer->timer_ext,0);
     	}
        timer->timer_ext->last_expiry = timetick_now;
  	}
  else
  	{
        ERR_FATAL (" Timer must have an ext timer, rextcb=0x%x timer_ext=0x%x timer_ptr=0x%x",
                timer->tcb,timer->timer_ext,timer);
  	}
  
  /*Expire the timer only if the timer state is
    resumed or set*/
  if(TIMER_RESUMED_FLAG == timer->timer_ext->timer_state
     || TIMER_SET_FLAG == timer->timer_ext->timer_state )
  {
    timer->timer_ext->timer_state = TIMER_EXPIRED_FLAG;
    expire_timer = TRUE;
  }

  return expire_timer;  
}

static timer_ptr_internal_type timer_next_timer_to_expire
(
  time_timetick_type ticks_now
)
{
  time_timetick_type tolerance = ticks_now + TIMER_OS_EARLY_TOL;

  /*first check the p0 list*/
  if(timers.p0_active.list.first
     && (timers.p0_active.list.first->expiry <= tolerance)) 
  {
    return timers.p0_active.list.first;
  }
  else if(timers.active.list.first
     && (timers.active.list.first->expiry <= tolerance)) 
  {
    return timers.active.list.first;
  }
  else
  {
    return NULL; 
  }
}


/*=============================================================================

FUNCTION TIMER_PROCESS_ACTIVE_TIMERS

DESCRIPTION
  Process timers, decrementing elapsed time from each one.

DEPENDENCIES
  Internal timer_ function()
  Must be called from inside ATS_INTLOCK & TASKLOCK (or interrupt) contexts, to
  prevent a large delay between reading the current timetick count, and
  writing the new timetick match value.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

static void timer_process_active_timers( void )
{
  /* Current slow clock count */
  time_timetick_type                   ticks_now = 0;

  /* Timer being processed */
  timer_ptr_internal_type         timer;

  /* Length of time the timer ran for */
  time_timetick_type                   run_time;

  /* Minimum advance required for reloading timer */
  time_timetick_type                   min_advance;

  /* Interrupt locking status */
  ATS_DECLARE_ISAV(isave);

  /* Temporary value to compute the new expiry point */
  time_timetick_type                   new_expiry;

  /* Temporary index */
  int32                          temp_index;

  /*to determine whether timer is to be expired or not*/
  boolean                        expire_timer = FALSE;

  struct timer_struct_internal          timer_temp_internal;

  time_timetick_type             next_timer_expiry =TIMER_MAX_EXPIRY;

  timer_ptr_type                 head_timer=NULL;

  timer_list_type*               current_list = NULL;

  uint32                         logged_tcb_or_cb;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&timer_temp_internal,0,sizeof(timer_temp_internal));

  /* Lock interrupts while testing & manipulating the active timer list */
  ATS_INTLOCK_SAV( isave );

  /* Record the fact that timers are being processed, to prevent re-entry
     into this function, and unnecessary modification of TIMETICK_MATCH. */
  timers.processing = TRUE;

  /* Get current slow clock count */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  /* Save a trace packet */
  TIMER_TRACE_TS(PROCESS_START,ticks_now);


  /* Check to determine if the timer at head of the active timer list has
     past its expiry point.  If it has, remove it, expire the timer, and
     repeat with the new timer at the head of active timer list.
       
     Note: a timer's expiry can change timers.active.list.zero, so no local
     variable should hold a calculated value involving it from one iteration
     to the next. */

  timer_while_cnt = 0;

  
   
  while ( NULL != (timer = timer_next_timer_to_expire(ticks_now)))          
  {
	/* Check if the external timer also points to this internal timer.
	To catch double internal timer cases */ 
	if (timer != (timer->timer_ext ? timer_get_internal_timer(timer->timer_ext) : NULL))
		ERR_FATAL("Incorrect int_timer=0x%x for the ext_timer=0x%x; tcb=0x%x",
		           timer,timer->timer_ext,timer->tcb);
  
    timer_while_cnt++;

    /*populate the current list*/
    current_list = timer->list;
 
    if( timer->bin != NULL )
    {
      /*debug*/
      if(TIMER_PRIORITY_0 == timer->timer_priority) 
      {
        ERR_FATAL("3- Priority timers should not have been binned",0,0,0);
      }

      /* Unbin this timer */
      timer_bin_remove( timer );
    }

    if( timer->tcb != NULL && timer->sigs != 0 )
    {
      logged_tcb_or_cb = (uint32)timer->tcb;
    }
    else if( timer->func1 )
    {
      logged_tcb_or_cb = (uint32)timer->func1;
    }
    else
    {
      logged_tcb_or_cb = (uint32)timer->func2;
    }

    timer_dbg_log_timer_exp( timer_while_cnt,
                             timer->timer_ext,
                             timer,
                             ticks_now,
                             timer->expiry,
                             timer->start,
                             logged_tcb_or_cb,
                             timer->slave_task_assigned,
                             (uint8)timer->notify_type );
    
    /* Donot check while debugging through jtag. Need to make timer_jtag_checks_disable as 1 */
    if(!timer_jtag_checks_disable)
        if(timer->group == &timer_prio0_group)
           if(ticks_now > timer->expiry)
           {
              if((ticks_now - timer->expiry) > (3*19200))
                  ERR_FATAL("priority timer 0x%x got delayed by %d msecs", timer, (ticks_now - timer->expiry)/19200, 0);           
           }

    /* Remove expiring timer from active timer list */
      timer_remove_head( current_list );
     
    /* Compute length of time the timer ran for.  This may be longer than
       programmed due to sleep or other delays in expiry processing. */
    run_time = ticks_now - timer->start;

    /* Check to see if the timer should be expired or not */
	expire_timer = timer_determine_timer_expiry(timer, ticks_now);

    /* Reactivate timer, if required */
    if ( timer->reload > 0 )
    {
      /* Determine how late the timer expired; this is the minimum
         amount the timer must be advanced by for the next expiry. */
      min_advance = ticks_now - timer->expiry;
  
      new_expiry = timer->expiry + timer->reload;
      
      if(new_expiry  <= ticks_now)
      {
        /* Temporary value to compute the new expiry point */
        new_expiry = timer->expiry;
        
        /* Timer expired 1 or more reload period ago.  This can happen if
           the timer belongs to a timer group which gets disabled, such as
           the default timer groups during sleep. */

        /* Round min_advance up to the next multiple of reload periods. */
        min_advance += timer->reload - min_advance % timer->reload;

        /* Add the rounded-up minimum advance to the timer expiry */
        new_expiry += min_advance;
      }

        /* Check to make sure that the new expiry point is further in the future
           than the old one.  This prevents the cases where overflow in the
           calculation could occur or wrap around past the active timer list
           zero. */
        
        if (new_expiry > timer->expiry )
        {
          /* New expiry point is further in the future than the old one, use it */
          timer->expiry = new_expiry;
        }
        else
        {
          /* The recurring timer’s new expiry point is at a point so far in the 
             future that it exceeds timer dynamic range.  Because of this, move  
             the expiry to just fit the available range */

          /* Move the expiry point as far out as possible */
          timer->expiry = TIMER_MAX_EXPIRY;
      
          //ATS_MSG_ERROR("Timer expiry set to max value. Timer may never expire !",0,0,0 );
        }

      /* Record the new start time for the next expiry */
      timer->start = ticks_now;

      /* Update sclk_delta for the timer when timer is set */
      DalTimetick_GetOffset(hTimerHandle,&timer->sclk_delta);

      /* Insert timer back in active list. */
      timer_insert( current_list, timer );

      memscpy(&timer_temp_internal,sizeof(timer_temp_internal),timer,sizeof(timer_temp_internal)); 

       if ( NULL != timer->timer_ext )
       {
        timer->timer_ext->timer_state = TIMER_SET_FLAG; 
       }


      /* We've finished manipulating the active timer list.  Unlock
       interrupts for the duration of the timer's expiry processing */
       ATS_INTFREE_SAV( isave );

      /* Expire timer */
      if(expire_timer) 
      {
         /* Expire timer */
        timer_expire( &timer_temp_internal, run_time ); 
      }     

    }
    else
    {

    /* Mark the timer_handle as invalid for the external timer */
    timer->timer_ext->timer_handle = TIMER_BUFFER_INVALID_HANDLE ;

      memscpy(&timer_temp_internal,sizeof(timer_temp_internal),timer,sizeof(timer_temp_internal));

      /* Save index value in a teemp var */
      temp_index = timer->index;

      /* Clear expired timer buffer */
      memset( timer, 0, sizeof( timer_internal_type ));

      /* Restore timer index*/
      timer->index = temp_index ;

      timer->internal_q = TIMER_IN_FREE_Q;
  
      /* Link timer_buffer element back to timer_free_q */
      (void) q_link( timer, 
             &(timer->link) );
    
      /* Enqueue node in timer_free_q */
      q_put( &timer_free_q, &(timer->link) );

      timer_count_buffer_allocated = MAX_TIMER_BUFFER_NUMBER-q_cnt(&timer_free_q);

      /* We've finished manipulating the active timer list.  Unlock
       interrupts for the duration of the timer's expiry processing */
      ATS_INTFREE_SAV( isave );

      if(expire_timer) 
      {
        /* Expire timer */
        timer_expire( &timer_temp_internal, run_time );
      }

    }

    /* NOTE: The above call may run user code, which can do anything ...
       including installing and uninstall other timers.  This can cause
       the value of timers.active.list.zero to change at this point.
       We must use the new value in the next iteration, so any local
       variable which holds computations involving that value must
       be considered invalid after the above call.  */


    /* Lock interrupts in preparation of the next iteration of the loop,
       which tests & manipulates the active timer list */
    ATS_INTLOCK_SAV( isave );

    /* Get current slow clock count */
    DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  } /* while timers on timer.active.list are expiring */
  
  

  /* Timers that expire at and before "ticks_now" have been processed.
     Set interrupt for when next timer expires. */

  next_timer_expiry = timer_determine_next_timer_expiry(&head_timer);

    /* The first timer on the active list is the timer next to expire */
  timer_set_next_interrupt( next_timer_expiry,ticks_now, TRUE, 
                            TIMER_MVS_TIMER_PROCESSED, 
                            head_timer );



  /* Timer processing has completed */
  timers.processing = FALSE;

  /* Save a trace packet */
  TIMER_TRACE_TS(PROCESS_END, ticks_now);
  
  
  /* We've finished manipulating the active timer list.  */
  ATS_INTFREE_SAV( isave );

  // Resetting and checking should happen in a mutex
  TIMER_PC_MASK_MUTEX_LOCK();
  if(timer_after_pc_mask[TIMER_PC_MASK_INDEX] == 1)
  {
    timer_reset_check_pc_mask(TIMER_PC_MASK_INDEX);
  }
  TIMER_PC_MASK_MUTEX_UNLOCK();

  #if defined(T_QUASAR) && !defined(T_QUASAR_X)
  /* Because of a hardware problem with QUASar implementation of this
     timer, do read on the timer count until it's not zero...
     The problem is that for some reason, the SLEEP_XTAL_TIMETICK_COUNT
     will read zero for a while (maybe a 10-20 us) if you read it right
     after the ..._VAL is written to.  This is an easy workaround, instead
     having to check for the actual condition of ..._VAL being written
     (_VAL is written in timer_set_next_interrupt above). */
  while ( MSM_R( SLEEP_XTAL_TIMETICK_COUNT ) == 0 )
  {
    /* Do nothing */
  }
  #endif /* T_QUASAR */

} /* timer_process_active_timers */



/*=============================================================================

FUNCTION TIMER_TASK

DESCRIPTION
  Initializes the Timer Task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_task
(
  /* Unused parameter - required to match rex_task_func_type prototype */
  dword                           ignored    /*lint -esym(715,ignored) */
)
{
  /* Signals returned from rex wait procedure */
  rex_sigs_type                   sigs;

  /* Stores possible result from registering ISR if we own the Interrupt */
  #ifndef FEATURE_TIMER_USE_QURT_SYSCLOCK
  int                             eRes;
  #endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/

  /* Install the timer expiry ISR */
  /* was DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER  if we own the interrupt */
  #ifndef FEATURE_TIMER_USE_QURT_SYSCLOCK
  if (timer_fn_map.timer_register_isr)
  {
    eRes = timer_fn_map.timer_register_isr(timers.dal_int_handle,
                                           timer_fn_map.timer_interrupt_id,
                                           (DALISR)timer_task_isr,
                                           NULL,
                                           timer_fn_map.timer_interrupt_flag);
    if ( eRes != DAL_SUCCESS )
    {
      ERR_FATAL("Cannot register ISR",0,0,0);
    }
  }
  #endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/

#ifndef FEATURE_TIME_QDSP6
  /* Signal the task in order to prime the timer processing mechanism */
  (void) rex_set_sigs( timer_tcb_ptr, TIMER_CHANGE_SIG );
#else
   /* Signal the task in order to prime the timer processing mechanism */
   (void) qurt_anysignal_set(&timer_q6_signal, TIMER_QURT_SIG_MASK);
#endif

  rcinit_handshake_startup();

  for(;;)
  {

#ifndef FEATURE_TIME_QDSP6
    /* Wait for some task signal */
    sigs = rex_wait( TIMER_PROCESS_SIGS );
#else
    sigs = qurt_anysignal_wait (&timer_q6_signal, TIMER_QURT_SIG_MASK);
#endif

#ifndef FEATURE_TIME_QDSP6
    if ( sigs & TIMER_PROCESS_SIGS )
    {
      /* Clear the timer expiry signal */
      (void) rex_clr_sigs( timer_tcb_ptr, TIMER_PROCESS_SIGS );
#else
    if (sigs == TIMER_QURT_SIG_MASK) 
    {
      /*record the time at which the qurt signal came*/
      #ifdef FEATURE_TIMER_USE_QURT_SYSCLOCK
      timer_update_isr_time();
      #endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/
       
      /* Clear the timer expiry signal */
      qurt_anysignal_clear (&timer_q6_signal, TIMER_QURT_SIG_MASK);
#endif

      /* Process expiring timer(s) */
      timer_process_active_timers();
    }
  }
} /* timer_task */


/* If FEATURE_TIMER_TASK is defined, then the time tick interrupt is
** serviced in the task context.  The timer task should be started
** before enabling the interrupt so that the task is running when an
** interrupt occurs.
** */

void timer_task_init(void) /* Time services */
{
  RCINIT_INFO handle = rcinit_lookup("timer");

  if (RCINIT_NULL != handle)
  {
    rcinit_initfn_spawn_task(handle, timer_task);
  }
  
} /* timer_task_init */


/*=============================================================================

FUNCTION TIMER_INIT                                                    REGIONAL

DESCRIPTION
  Initializes the 32kHz slow clock based Timer Subsytem

DEPENDENCIES
  Must only be called from time_init()

RETURN VALUE
  None

SIDE EFFECTS
  May install an ISR for the 32-bit slow clock match register

=============================================================================*/

void timer_init( void )
{
  /* Current slow clock count */
  time_timetick_type              ticks_now = 0;

  /* Loop index for timer bin initialization */
  int                             i;

  /* Timer bin */
  timer_bin_ptr                   bin;

  /* RLC starting values */

  rlc_error_type rlc_error = RLC_FAILED;

  #ifndef FEATURE_TIMER_USE_QURT_SYSCLOCK
  /* Dal Result */
  DALResult                       eRes;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  eRes = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &timers.dal_int_handle);
  if( eRes != DAL_SUCCESS || timers.dal_int_handle == NULL )
  {
    /* Error out, we should not start up timer task if we cannot set interrupts through DAL */
    ERR_FATAL("timer_init: Could not obtain a Dal Interrupt Handle",0,0,0);
  }
  #endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/

  timer_pimutex_init();
  timer_pc_mutex_init();

  timer_fn_map_initialize_fp(&timer_fn_map);

  /*sequence dependency - slave cmd queues have to be initialized 
   before timer_init*/
  #ifdef FEATURE_TIMER_SLAVE1
  /*the timer slave1 command queue has to be initializer here, before 
   starting the timer task*/
  timer_slave_cmd_queue_init();
  #endif

  /*initialize the signal that will be used for signalling expiry of
   the smallest timer when match value == timetick value*/
  timer_get_timetick_handle();

  #ifdef FEATURE_TIME_QDSP6
  /* Use function map here */
  timer_init_qurt_signal();
  #endif /*FEATURE_TIME_QDSP6*/

  /* Get current slow clock count */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_INIT, NULL, ticks_now, NULL);

  /* Initialize the timer_bin_pool and free list */

  for(i = 0; i < TIMER_BIN_NUM; i++)
  {
    bin = &timer_bin_pool[i]; 

    /* init the bin's data */
    bin->first    = NULL;
    bin->last     = NULL;
    bin->lower    = 0;
    bin->upper    = 0;
    bin->count    = 0;

    /* push it on the free list */
    timer_bin_list_push( &timers.bins_free, bin);
  }

  /* Initialize the list of active timers to contain no timers */
  timers.active.list.first = NULL;
  timers.active.list.last  = NULL;
  timers.p0_active.list.first = NULL;
  timers.p0_active.list.last  = NULL;  

  /* Initialize the first non-deferrable timer to NULL */
  timers.first_non_deferrable = NULL;

  /* Initialize timers.set_value, set_time values & last_set_time for tests 
     in set_next_interrupt() and update_timer_interrupt */
  timers.set_value         = ticks_now - 2;
  timers.set_time          = ticks_now - 1;
  timers.last_set_time     = ticks_now - 1;

#ifndef FEATURE_TIME_QDSP6

  #ifdef TIMETICK_INT_EDGE_TRIGGERED
  
  /* Setup edge-sensitive interrupt, instead of having the interrupt
     asserted for 30us */
  TIMETICK_INT_EDGE_TRIGGERED_INIT();

  #endif /* TIMETICK_INT_EDGE_TRIGGERED */

#endif

  #ifdef FEATURE_TIMER_SLAVE1
  timer_slave_processing_enabled = TRUE;
  #endif
  
  /* Register rlc handle */
  rlc_error = rlc_register_handle(&rlc_handle, "Timer");
  if(rlc_error != RLC_SUCCESS)
  {
    ATS_ERR_1("There was an error registering rlc handle! Error: 0x%x\n", rlc_error);
    //Sleep(5000);
    return;
  }
  else
  {
	ATS_MSG("Added handle successfully!\n");
  }
	
  
  rlc_error = rlc_add_learning_attribute(rlc_handle,
                                                  RLC_LEARNING_CLUSTERING,
                                                  64,
                                                  1,
                                                  2,
                                                  &rlc_attribute_learn
                                                  );
  if(rlc_error != RLC_SUCCESS)
  {

    ATS_ERR_1("There was an error adding learning attribute! Error: 0x%x\n", rlc_error);
    return;
  }
  else
  {
	ATS_MSG("Learning attribute added successfully!\n");
  }
  
  rlc_error = rlc_add_correcting_attribute_sc(rlc_handle, RLC_CORR_SET_VAR_BC, timer_set_backoff, rlc_attribute_learn, &rlc_attribute_correct);
  if(rlc_error != RLC_SUCCESS)
  {
    ATS_ERR_1("There was an error adding correction attribute! Error: 0x%x\n", rlc_error);
    return;
  }
  else
  {
	ATS_ERR("Correction attribute added successfully!\n");
  }

} /* timer_init */


/*=============================================================================

FUNCTION TIMER_GROUP_DISABLE

DESCRIPTION
  Disables a timer group.  Timers in the timer group will continue to count
  down, but they will not expire until the group is enabled.

DEPENDENCIES
  Timer group must not be left disabled for more than 2^32 slow clock counts.

RETURN VALUE
   TE_INVALID_PARAMETERS  If passed timer_group pointer is NULL
   TE_INVALID_GROUP_HANDLE	If the group asked to be disabled is time_prio0_group
   TE_SUCCESS  If the operation completed successfully

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

timer_error_type timer_group_disable
(
  /* Timer group to be disabled */
  timer_group_ptr                 timer_group
)
{
  /* Pointer to timers, for walking through lists of timers */
  timer_ptr_internal_type          ptr;
  time_timetick_type               now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( timer_group == NULL )
  	{
  	  ASSERT(0);
	  return TE_INVALID_PARAMETERS;
  	}


  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);
  TIMER_API_LOG(TIMER_API_LOG_TIMER_GROUP_DISABLE,NULL,now,NULL);

  /*if timer_priority0_group, return as disabling of 
   timer_prio0_group is not allowed*/
  if(timer_group == &timer_prio0_group) 
  {
    ATS_INTFREE();
    return TE_INVALID_GROUP_HANDLE;
  }

  /* Disable the timer group, if it is not already disabled */

  if ( !timer_group->disabled )
  {
    /* Mark timer-group as disabled, and record zero-basis */
    timer_group->disabled  = TRUE;

    /* Move given group's timers from active timer list to group timer list */
    ptr = timers.active.list.first;

    while (ptr != NULL)
    {
      /* Remember the next element on the active list, we are going
         to nuke it */
      timer_ptr_internal_type next = ptr->next;

      if ( ptr->group == timer_group )
      {
        /* This is a timer in the given timer group */

        /* if it's binned, remove it from its bin */
        if( ptr->bin != NULL ) 
        {
            timer_bin_remove( ptr );
        }
       
        /* Unlink timer from active list */
        timer_unlink( ptr );
       
        /* Once the timer has been removed from the active
         timer list, we need to decrement the num of
         timers in the active list*/
        /* Decrement the number of timers  */
        timer_active_timers_count--; 
       
        /* Append to group timer list */
        timer_append( &timer_group->list, ptr );
      }

        /* Walk to next timer node */
      ptr = next;

    }

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_GROUP_DISABLED);
  }

  /* Update the first non-deferrable timer in the active list */
  timer_update_first_active_non_deferrable_timer();

  ATS_INTFREE();
  return TE_SUCCESS;

} /* timer_group_disable */



/*=============================================================================

FUNCTION TIMER_GROUP_ENABLE

DESCRIPTION
  Enables a timer group.  Timers in the timer group which expired during the
  diabled period will have their expiry processing executed.

DEPENDENCIES
  Timer group must not be left disabled for more than 2^32 slow clock counts.

RETURN VALUE
  TE_INVALID_PARAMETERS  If the passed timer_group pointer is invalid
  TE_SUCCESS  If the operation completed successfully

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

timer_error_type timer_group_enable
(
  /* Timer group to be enabled */
  timer_group_ptr                 timer_group
)
{
  /* Current slow clock count */
  time_timetick_type                   ticks_now = 0;

  /* Pointer to a timer in the list of timers */
  timer_ptr_internal_type         timer;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (timer_group == NULL)
  	{
  	  ASSERT(0);
	  return TE_INVALID_PARAMETERS;
  	}

  ATS_INTLOCK();

  /* Get current slow clock count */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_GROUP_ENABLE,NULL,ticks_now,NULL);

  /* Enable the timer group, if it is disabled */

  if ( timer_group->disabled )
  {
    /* Walk through timers in timer group list.
       Stop when we get to a timer after the head of the active list */

    timer = timer_group->list.first;

    while( timer != NULL )
    { 
      if( timer->expiry >= ticks_now )
      {
        break; 
      }
      /* Increment the number of expired timers  */
      timer_active_timers_count++; 
      timer = timer->next;
    }

    /* Break timer list into two parts at this point */
    if( timer != NULL )
    {
      timer_group->list.last = timer->prev;

      if( timer->prev == NULL )
      {
        timer_group->list.first = NULL;
      }
      else
      {
        timer->prev->next = NULL;
      }

      timer->prev = NULL;
    }
        
    /* Expired timers are in list pointed to by "timer_group->list" */
    /* Non-expired timers are in linked list pointed to by "timer" */

    /* Updates timer_num in timer_insert for non-expired timers */
    /* Move non-expired timers into active timer list */
    timer_insert_list( &timers.active.list, timer );

    /* If there are any expired timers ... */
    if ( timer_group->list.first != NULL ) 
    {

      /* Move them to the front of the special expired list */
      timer_concatenate_lists( &timer_group->list, &timers.active.list ); 
 
    }

    /* Mark timer-group as enabled */
    timer_group->disabled = FALSE;

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_GROUP_ENABLED);
  }

  ATS_INTFREE();

  return TE_SUCCESS;

} /* timer_group_enable */


/*=============================================================================

FUNCTION TIMER_GROUP_SET_DEFERRABLE

DESCRIPTION
  Set timer group deferrable status.Timer groups default to being non-deferrable.

DEPENDENCIES
  None

RETURN VALUE
   TE_INVALID_PARAMETERS  If passed group pointer is NULL
   TE_INVALID_GROUP_HANDLE	If the operation is asked on a group for which it is not allowed, i.e.
												- cannot set timer_null_group to non-deferrable
												- cannot set timer_non_defer_group to deferrable
   TE_SUCCESS : If the operation completed successfully

SIDE EFFECTS
  None.

=============================================================================*/

timer_error_type timer_group_set_deferrable( timer_group_ptr pgroup, boolean condition )
{
  timer_error_type        status;
  time_timetick_type      now = 0;
  /* Set the property of this timer group to indicate whether or not it's 
  deferrable, based on the argument passed in*/
    
  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_GROUP_SET_DEFERRABLE,NULL,now,NULL);
  
  if ( pgroup != NULL )
  {
    if (pgroup == &timer_null_group && condition == FALSE)
    	{
    	 status = TE_INVALID_GROUP_HANDLE;
    	}
	else if (pgroup == &timer_non_defer_group && condition == TRUE)
		{
		 status = TE_INVALID_GROUP_HANDLE;
		}
	else
		{
		 pgroup->deferrable = condition;

		 /* Update the first non-deferrable timer in the active list */
		 timer_update_first_active_non_deferrable_timer();

		 status = TE_SUCCESS;
		}
  }
  else
  {
	status = TE_INVALID_PARAMETERS;  
  }
  ATS_INTFREE();
  return status; 

} /* timer_group_set_deferrable */

/*=============================================================================

FUNCTION TIMER_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY

DESCRIPTION
  Gets the first non-deferrable timer's expiry value.

DEPENDENCIES
  None.

RETURN VALUE
  Absolute expiry of first occuring non-deferrable timer.
  Returns TIMER_MAX_EXPIRY in case there is no non-deferrable timer present.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_get_first_non_deferrable_timer_expiry( void )
{
  /* Expiry for the 1st non-deferrable timer */
  time_timetick_type              future_expiry = TIMER_MAX_EXPIRY;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  //TIMER_API_LOG(TIMER_API_LOG_TIMER_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY,NULL,ticks_now,NULL);

  /* Return the expiry of the first non-deferrable timer */
  if (timers.first_non_deferrable != NULL)
  {
    future_expiry = timers.first_non_deferrable->expiry;
  }
 
  ATS_INTFREE();

  return future_expiry;

} /* timer_get_first_non_deferrable_timer_expiry */


/*=============================================================================

FUNCTION TIMER_GET_TIME_TILL_FIRST_NON_DEFERRABLE_TIMER

DESCRIPTION
  Gets time based on 32kHz clock to the next occurring non-deferrable  
  timer expiry value. 
  @NOTE : THIS API IS NOT BEING USED & WILL GET DEPRICATED

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of sclk's until the next timer expiry in a timer group which is 
  non-deferrable.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_get_time_till_first_non_deferrable_timer( void )
{
  return timer_conv_to_sclk_freq(timer_get_time_till_first_non_deferrable_timer_64());
} /* timer_get_time_till_first_non_deferrable_timer */


/*=============================================================================

FUNCTION TIMER_GET_TIME_TILL_FIRST_NON_DEFERRABLE_TIMER_64

DESCRIPTION
  Gets time based on 19MHz clock to the next occurring non-deferrable  
  timer expiry value. 

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of 19MHz ticks until the next timer expiry in a timer group which is 
  non-deferrable.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_get_time_till_first_non_deferrable_timer_64( void )
{
  /* Current slow clock count */
  time_timetick_type              ticks_now = 0;
  time_timetick_type              future_expiry;
  time_timetick_type              tt_ret;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  /* Get the current time */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_GET_TIME_TILL_FIRST_NON_DEFERRABLE_TIMER,NULL,ticks_now,NULL);

  /* Get the expiry of first non-deferrable active timer */
  future_expiry = timer_get_first_non_deferrable_timer_expiry();

  /* If future_expiry is in past */
  if (future_expiry  < ticks_now )
  {  
    tt_ret = 0;
  }
  else
  {  
    /* Return the difference between now and the above future expiry */
    tt_ret = (future_expiry - ticks_now);
  }

  ATS_INTFREE();

  return tt_ret;

} /* timer_get_time_till_first_non_deferrable_timer_64 */
     


/*=============================================================================

FUNCTION TIMER_DEFER_MATCH_INTERRUPT

DESCRIPTION
  Sets timer match interrupt to the next occurring non-deferrable timer expiry 
  value. Complimentary function is timer_undefer_match_interrupt(). 

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of sclk's in 32kHz domain until the next timer expiry in a timer  
  group which is non-deferrable.

SIDE EFFECTS
  Adjusts match value and prevents any subsequent timer calls from updating the
  match value, until timer_undefer_match_interrupt() is called.

=============================================================================*/

timetick_type timer_defer_match_interrupt( void )
{
  return timer_conv_to_sclk_freq(timer_defer_match_interrupt_64());
} /* timer_defer_match_interrupt */


/*=============================================================================

FUNCTION TIMER_DEFER_MATCH_INTERRUPT_64

DESCRIPTION
  Sets timer match interrupt to the next occurring non-deferrable timer expiry 
  value. Complimentary function is timer_undefer_match_interrupt(). 

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of ticks in 19MHz domain until the next timer expiry in a timer  
  group which is non-deferrable.

SIDE EFFECTS
  Adjusts match value and prevents any subsequent timer calls from updating the
  match value, until timer_undefer_match_interrupt() is called.

=============================================================================*/

time_timetick_type timer_defer_match_interrupt_64( void )
{
  /* Current slow clock count */
  time_timetick_type                   ticks_now = 0;
  time_timetick_type                   future_expiry;
  timer_ptr_type                  head_timer = NULL;
  ATS_DECLARE_ISAV(isave);


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  TIMER_PC_MASK_MUTEX_LOCK();
  timer_after_pc_mask[TIMER_PC_DEFER_UNDEFER_CNT_INDEX]++; 
  TIMER_PC_MASK_MUTEX_UNLOCK();


  ATS_INTLOCK_SAV(isave);

  /* Get the current time */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);
  
  timer_dbg_log_timer_defer_undefer( ticks_now, timer_defer_undefer_reference_count + 1, TIMER_MVS_DEFER_MATCH_INT, 0 );

  TIMER_API_LOG(TIMER_API_LOG_TIMER_DEFER_MATCH_INTERRUPT,NULL,ticks_now,NULL);

  /* Assume the worst - furthest point in future ~119 years */
  future_expiry = TIMER_MAX_EXPIRY;

  /* Return the expiry of the first non-deferrable timer */
  if (timers.first_non_deferrable != NULL)
  {
    future_expiry = timers.first_non_deferrable->expiry;
    head_timer = timers.first_non_deferrable->timer_ext;
  }

  /* If future_expiry is in past */
  if (future_expiry < ticks_now )
  {
    timer_defer_undefer_reference_count++;
    ATS_INTFREE_SAV(isave);
    return 0;
  }
  
  /* Set the timer interrupt to occur at the proper point in the future*/
  timer_set_next_interrupt( TIMER_MAX_EXPIRY, ticks_now, FALSE,TIMER_MVS_DEFER_MATCH_INT,head_timer );

  /*increase the reference count here AFTER calling the timer_set_next_interrupt()
   so that the first time around we program the timer interrupt. The decision to bypass 
   programming of the timer interrupt, the next time, is made inside 
   timer_set_next_interrupt() */
  timer_defer_undefer_reference_count++;

  /* From this point on timer interrupt  won’t be adjusted */
  timers.do_reprogram_isr = FALSE;

  ATS_INTFREE_SAV(isave);

  /* Return the difference between now and the above future expiry */
  return (future_expiry - ticks_now);

} /* timer_defer_match_interrupt_64 */


/*=============================================================================

FUNCTION TIMER_UNDEFER_MATCH_INTERRUPT

DESCRIPTION
  Set the timer ISR to fire at the earliest expiring timer in the active
  list.  Meant to serve as a the complimentary function to
  timer_defer_match_interrupt()

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  None

SIDE EFFECTS
  Adjusts the match value.

=============================================================================*/
void timer_undefer_match_interrupt( void )
{
  /* Current slow clock count */
  time_timetick_type                   now = 0;

  /* temp Timetick */
  time_timetick_type                   tmp_ticks = 0;

  /* Time of first expiring timer */
  time_timetick_type              first = TIMER_MAX_EXPIRY;

  timer_ptr_type                  head_timer=NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Log timetick here also, before any operation is performed */
  DalTimetick_GetTimetick64(hTimerHandle, &tmp_ticks);

  TIMER_PC_MASK_MUTEX_LOCK();
  timer_after_pc_mask[TIMER_PC_MASK_INDEX]=1;

  /*timer_defer_undefer_reference_count is never suppose to go below 0 */
  if(timer_after_pc_mask[TIMER_PC_DEFER_UNDEFER_CNT_INDEX] == 0 ) 
  {
    ERR_FATAL("timer_undefer called more than timer defer = 0x%x",
              timer_after_pc_mask[TIMER_PC_DEFER_UNDEFER_CNT_INDEX],0,0);
  }

  if(timer_after_pc_mask[TIMER_PC_DEFER_UNDEFER_CNT_INDEX] > 0 ) 
  {
    if(--timer_after_pc_mask[TIMER_PC_DEFER_UNDEFER_CNT_INDEX] == 0) 
    {
      // SET THE CPU TO MAX USING NPA CLIENT
      if( timer_fn_map.timer_npa_clk_client )
      { /* client/resource exists so use can make MAX requests to the clock */
        timer_fn_map.timer_npa_issue_suppressible_request( timer_fn_map.timer_npa_clk_client,
                                                       timer_fn_map.timer_npa_max_state );
    timer_cnt_cpu_max++;
  }else
  {
    ATS_MSG_ERROR("NPA Client is null for Timer to bring up the cpu",0,0,0);
      }
    }
  }
  TIMER_PC_MASK_MUTEX_UNLOCK();

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_UNDEFER_MATCH_INTERRUPT,NULL,now,NULL);

  timer_defer_undefer_reference_count--;

  timer_dbg_log_timer_defer_undefer( now, timer_defer_undefer_reference_count, TIMER_MVS_UNDEFER_MATCH_INT, tmp_ticks );

  /*timer_defer_undefer_reference_count is never suppose to go below 0 */
  if(timer_defer_undefer_reference_count < 0 ) 
  {
    ERR_FATAL("timer_defer and undefer calls do not match, ref_cnt=0x%x",
              timer_defer_undefer_reference_count,0,0);
  }

  /*we can return safely here since we will not be programming 
   the timer interrupt anyways.*/
  if(timer_defer_undefer_reference_count > 0)
  {
    ATS_INTFREE();
    return;
  }

  first = timer_determine_next_timer_expiry(&head_timer);

  /* If the first expiring timer is in the past ... */
  if ( first < now  )
  {
    /* Set the timer for "as soon as possible" (eg, "now") */
    first = now;
  }
   
  /* Set the next interrupt match value. The function is called after power 
     collapse and the hardware might not contain right values . Hence force 
     MATCH_VAL register to be updated. */
  timer_set_next_interrupt( first, now, TRUE,TIMER_MVS_UNDEFER_MATCH_INT, head_timer );

  /* Allow timer isr programming again */
  timers.do_reprogram_isr = TRUE;

  ATS_INTFREE();

  } /* timer_undefer_match_interrupt */


/*=============================================================================

FUNCTION TIMER_RESTART

DESCRIPTION
 This function is called after waking up. The timetick counter values 
 are readjusted to sync up with the base timetick counter when waking up from
 power collapse. The match value has to be adjusted accordingly.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Adjusts the match value


=============================================================================*/

void timer_restart(void)
{
  time_timetick_type                now = 0;

  DalTimetick_GetTimetick64(hTimerHandle, &now);
  TIMER_API_LOG(TIMER_API_LOG_TIMER_RESTART,NULL,now,NULL);
  /* Calling timer_undefer_match_interrupt */
  timer_undefer_match_interrupt();
}

/*=============================================================================

FUNCTION TIMER_DEFINE

DESCRIPTION
  Defines a timer structure, initializing members to NULL, and associates the
  timer with the given group.

DEPENDENCIES
  Timer must not already be defined and on a timer list (active).

RETURN VALUE
  None

SIDE EFFECTS
  May link timer group into linked list of timer groups

=============================================================================*/

static timer_error_type timer_define
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group
)
{
  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  
  if (timer == NULL){
    ASSERT(0);
	return TE_INVALID_PARAMETERS;
	}
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*if the timer is of priority 0, then it needs to be 
   associated with a static timer group that will be 
   guaranteed to be non-deferrable*/
  if(TIMER_PRIORITY_0 == timer->timer_priority) 
  {
    group = &timer_prio0_group;
  }
  else
  {
    /* Reparent timers in the NULL group to be in the timer_null_group */
    if ( group == NULL )
    {
      group = &timer_null_group;
    }
  }

  /* If this timer group is not yet in the linked list of timer groups,
     add it to the list. */

  if ( group->next == NULL )
  {
    group->next = timers.active.next;
    timers.active.next = group;
  }


#define TIMER_CHECK_FOR_DEFINE_OF_ACTIVE_TIMER
#ifdef TIMER_CHECK_FOR_DEFINE_OF_ACTIVE_TIMER

  /* Check if timer is active (already on the active timer list, or a
   deactived timer group list).  If it is, we need to remove it,
   or when the structure is re-initialized, the timer linked lists
   will be corrupted. */

  timer_int = timer_get_internal_timer(timer);

  /*If the timer is found in timer_buffer and the timer state
   is PAUSED, then return with an error message that we cannot
   define a paused timer*/
  if(timer_int 
     && TIMER_PAUSED_FLAG == timer->timer_state) 
  {
     ATS_MSG_ERROR("timer_def(%x,%x) called for paused timer (list %x) call clear or resume",
            timer, group, timer_int->list);
     return TE_INVALID_STATE_FOR_OP;
  }
  
  if ( timer_int != NULL && timer_int->list != NULL 
       && timer_int->internal_q == TIMER_NOT_IN_FREE_Q 
       && timer_int->group == timer->group
       && timer_int->sigs  == timer->sigs
       && timer_int->func1 == timer->func1
       && timer_int->func2 == timer->func2
       && timer_int->tcb   == timer->tcb )
  {
    /* Timer might be active ... or it might be declared as a stack
     variable, and contain garbage values in all of the fields.
     Search for the list it claims to be a member of in the
     linked list of timer groups. */

    timer_group_ptr  grp = &timers.active;

    do
    {
      /* Is the timer a member of the "grp" group? */
      if ( timer_int->list == &grp->list )
      {
         /* Yes - timer claims to be a member of an existing timer group */

        ATS_MSG_ERROR("timer_def(%x,%x) called for active timer (list %x)",
            timer, group, timer_int->list);

        /* Find timer in the groups's list */
        if( timer_in_list( timer_int, &grp->list ) )
        {
          /* Deactivate timer before initializing timer structure */          
          ERR_FATAL("timer_def(%x,%x) called for active timer (list %x)",
            timer, group, timer_int->list);
        }
      }

      /* Check next group */
      grp = grp->next;

    } while ( grp!= NULL && grp != &timers.active );

  }
 
  timer->timer_handle    = TIMER_BUFFER_INVALID_HANDLE;

#endif /* TIMER_CHECK_FOR_DEFINE_OF_ACTIVE_TIMER */

  /* Initialize timer structure */
  timer->group = group;

  #ifdef FEATURE_TIMER_SLAVE1
  /*assign the slave task here*/
  if ( timer->timer_priority != TIMER_PRIORITY_0 )
  	{
  	 timer->slave_task_assignment = timer_assign_slave_task_to_timer();
  	}
  else
  	{
  	 timer->slave_task_assignment = 0xFF;
  	}
  #endif

  timer->last_expiry = 0;
  timer->last_start = 0;
  timer->timer_state = TIMER_DEF_FLAG;

  return TE_SUCCESS;

} /* timer_define */


/*=============================================================================

FUNCTION TIMER_INIT_BUFFER_QUEUE

DESCRIPTION
  Iniitialize buffer queue where free timer buffers are kept. 

DEPENDENCIES
  Must be called from timer_def or timer_def2 before the first timer is 
  defined. It is not called from timer_init as some timers are defined 
  before call to timer_init.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_init_buffer_queue(void)
{
  /* local Iterator */
  uint32 i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Initialize the timer_free_q */
  q_init( &timer_free_q );
  
  for ( i = 0 ; i < MAX_TIMER_BUFFER_NUMBER ; i++ )
  {
    timer_buffer[i].internal_q = TIMER_IN_FREE_Q;
    timer_buffer[i].index = i;
  
    /* Link the handles to the timer_free_q */
    q_link( &timer_buffer[i], &timer_buffer[i].link );
    
    /* Enqueue handles */
    q_put( &timer_free_q, &timer_buffer[i].link );

  }
}


boolean timer_is_notify_type_valid(timer_notify_type notify_type)
{
  if(notify_type != TIMER_NATIVE_OS_SIGNAL_TYPE &&
     notify_type != TIMER_NATIVE_OS_SIGNAL2_TYPE &&
   	 notify_type != TIMER_REX_TCB_SIGNAL_TYPE &&
   	 notify_type != TIMER_DALSYS_EVENT_SIGNAL_TYPE &&
  	 notify_type != TIMER_FUNC1_CB_TYPE &&
  	 notify_type != TIMER_FUNC2_CB_TYPE &&
  	 notify_type != TIMER_NO_NOTIFY_TYPE)
     return FALSE;
  else
     return TRUE;
}
/*=============================================================================

FUNCTION timer_def_osal

DESCRIPTION
  Defines and initializes a timer. Signal and function notifications are 
  exclusive. If both are provided at the same time, error will be returned.

  The timer expires when:\n
  - sigs is non-NULL. The signals are set to the task given by tcb.
  - func is non-NULL. The callback function with the argument data
    is called from the timer tasks.

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS  Operation completed successfully
  TE_INVALID_PARAMETERS  Passed params are incorrect for this API
  TE_INVALID_STATE_FOR_OP  If the timer is in paused state


SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_def_osal
(
  /* Timer to define */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL = the "NULL" timer group) */
  timer_group_ptr                 group,

  /*ats timer callback type*/
  timer_notify_type               cb_type,

  /* Task to signal and/or task to queue APC call for */
  time_osal_notify_obj_ptr        sigs_func_addr,
  
  /* Task signals to set to the tcb task when timer expires */
  time_osal_notify_data           sigs_mask_data
)
{
  return timer_def_osal_priority( timer, group, cb_type, sigs_func_addr, sigs_mask_data, TIMER_PRIORITY_1);
} /* timer_drv_def_osal */


/*=============================================================================

FUNCTION timer_def_osal_priority

DESCRIPTION
  Defines and initializes a timer. Signal and function notifications are 
  exclusive. If both are provided at the same time, error will be returned.

  The timer expires when:\n
  - sigs is non-NULL. The signals are set to the task given by tcb.
  - func is non-NULL. The callback function with the argument data
    is called from the timer tasks.

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS  Operation completed successfully
  TE_INVALID_PARAMETERS  Passed params are incorrect for this API
  TE_INVALID_STATE_FOR_OP  If the timer is in paused state

  

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_def_osal_priority
(
  /* Timer to define */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL = the "NULL" timer group) */
  timer_group_ptr                 group,

  /*ats timer callback type*/
  timer_notify_type               cb_type,

  /* Task to signal and/or task to queue APC call for */
  time_osal_notify_obj_ptr        sigs_func_addr,
  
  /* Task signals to set to the tcb task when timer expires */
  time_osal_notify_data           sigs_mask_data,

  /* timer priority information */
  timer_priority_enum_type        timer_prio  
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  time_timetick_type			  now = 0;
  
  timer_error_type status = TE_SUCCESS;
  
  /* Input Validation */
  if(timer == NULL ||
     timer_prio > TIMER_MAX_PRIORITY
	 )
    return TE_INVALID_PARAMETERS;

  if(!timer_is_notify_type_valid(cb_type))
    return TE_INVALID_PARAMETERS;

  DalTimetick_GetTimetick64(hTimerHandle, &now);  

  ATS_INTLOCK();
  
  TIMER_API_LOG(TIMER_API_LOG_TIMER_DEF, timer, now, NULL);
  
  if ( sigs_mask_data != 0  &&  sigs_func_addr == NULL )
  {
    /* Todo: Can print msgs for error conditions from timer apis? */
    //ERR_FATAL("Can't set signals to NULL futex. T=%x S=%x", timer, sigs, 0);    
    ATS_INTFREE();
    return TE_INVALID_PARAMETERS;
  }

  /* Initialize the timer buffer if it is not initialized */
  if(!timer_q_init_status)
  {
    timer_init_buffer_queue();
    timer_q_init_status = 1;
  }
  
  /* Put the timer priority information */
  timer->timer_priority = timer_prio;    
  
  /* Initialize new timer structure */
  status = timer_define( timer, group );

  if (status != TE_SUCCESS)
  	{
  	  ATS_INTFREE();
  	  return status;
  	}

  timer->notify_type = cb_type;
  
  /* Assign the below members of the external client timer
     structure only after timer_define has been completed */
  if (cb_type == TIMER_NATIVE_OS_SIGNAL_TYPE ||
  	  cb_type == TIMER_REX_TCB_SIGNAL_TYPE ||
  	  cb_type == TIMER_NATIVE_OS_SIGNAL2_TYPE ||
  	  cb_type == TIMER_DALSYS_EVENT_SIGNAL_TYPE)
  	{
  	  timer->tcb           = (rex_tcb_type *)sigs_func_addr;
	  timer->sigs          = (rex_sigs_type)sigs_mask_data;
	  timer->func1	       = (timer_t1_cb_type)NULL;
	  timer->func2	       = (timer_t2_cb_type)NULL;
	  timer->data	       = (timer_cb_data_type)0;
  	}
  else if (cb_type == TIMER_FUNC1_CB_TYPE)
    {
	  timer->tcb		 = (rex_tcb_type *)NULL;
      timer->sigs 		 = (rex_sigs_type)0;
	  timer->func1		 = (timer_t1_cb_type)sigs_func_addr;
	  timer->func2		 = (timer_t2_cb_type)NULL;
	  timer->data 		 = (timer_cb_data_type)sigs_mask_data;
    }
  else if (cb_type == TIMER_FUNC2_CB_TYPE)
    {
	  timer->tcb		 = (rex_tcb_type *)NULL;
      timer->sigs 		 = (rex_sigs_type)0;
	  timer->func1		 = (timer_t1_cb_type)NULL;
	  timer->func2		 = (timer_t2_cb_type)sigs_func_addr;
	  timer->data 		 = (timer_cb_data_type)sigs_mask_data;
    }	
  else
    {
	  timer->tcb		 = (rex_tcb_type *)NULL;
      timer->sigs 		 = (rex_sigs_type)0;
	  timer->func1		 = (timer_t1_cb_type)NULL;
	  timer->func2		 = (timer_t2_cb_type)NULL;
	  timer->data 		 = (timer_cb_data_type)0;
	  timer->notify_type = TIMER_NO_NOTIFY_TYPE;
    }

  ATS_INTFREE();
  return status;
} /* timer_drv_def_osal */



/*=============================================================================

FUNCTION TIMER_DEF

DESCRIPTION
  Defines and initializes a timer.

  When the timer expires:
    if sigs is non-null, those signals are set to the task given by tcb;
    if func is non-null, then
       if tcb is null, then func(data) is called from interrupt context,
       otherwise, func(data) is queued as an Asynchronous Procedure Call (APC)

  Timers may also be staticly declared via the TIMER_DEF() macro

DEPENDENCIES
  None

RETURN VALUE
  TE_INVALID_STATE_FOR_OP  If the timer is in paused state
  TE_SUCCESS  If the operation completed successfully

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_def
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group,

  /* Task to signal and/or task to queue APC call for */
  rex_tcb_type                    *tcb,

  /* Task signals to set to the tcb task when timer expires */
  rex_sigs_type                   sigs,

  /* APC (tcb!=NULL) or callback (tcb==NULL) to queue/call at timer expiry */
  timer_t1_cb_type                func,

  /* Arbitrary data for APC or callback */
  timer_cb_data_type              data
)
{  
  /*call the new timer define api with priority 1*/
  return timer_def_priority(timer, group, tcb, sigs, func, data, TIMER_PRIORITY_1);
}
/*=============================================================================

FUNCTION TIMER_DEF_PRIORITY
  
DESCRIPTION
  Defines and initializes a timer with specified priority
  When the timer expires:
    if sigs is non-null, those signals are set to the task given by tcb;
    if func is non-null, then
       if tcb is null, then func(data) is called from interrupt context,
       otherwise, func(data) is queued as an Asynchronous Procedure Call (APC)

  Timers may also be staticly declared via the TIMER_DEF() macro

DEPENDENCIES
  None

RETURN VALUE
  TE_INVALID_STATE_FOR_OP  If the timer is in paused state
  TE_SUCCESS  If the operation completed successfully


SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_def_priority
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group,

  /* Task to signal and/or task to queue APC call for */
  rex_tcb_type                    *tcb,

  /* Task signals to set to the tcb task when timer expires */
  rex_sigs_type                   sigs,

  /* APC (tcb!=NULL) or callback (tcb==NULL) to queue/call at timer expiry */
  timer_t1_cb_type                func,

  /* Arbitrary data for APC or callback */
  timer_cb_data_type              data,

  timer_priority_enum_type        timer_prio
)
{  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  timer_error_type                status = TE_SUCCESS;
  
  time_timetick_type              now = 0;

  if( timer == NULL ){
    ASSERT(0);
	return TE_INVALID_PARAMETERS;
	}
	
  if( timer_prio > TIMER_PRIORITY_1 ){
	return TE_INVALID_PARAMETERS;
	}	
	

  if ( sigs != 0  &&  tcb == NULL )
  {
    ERR_FATAL("Can't set signals to NULL task. T=%x S=%x", timer, sigs, 0);
  }  

  if (sigs != 0 && func != NULL)
  	{
  	ATS_MSG_HIGH("Signals 0x%x and Func 0x%x both are defined for timer 0x%x", sigs, func, timer);
    timer_dbg_log_multi_not( tcb, timer );
  	}
  
  DalTimetick_GetTimetick64(hTimerHandle, &now);

  ATS_INTLOCK();
  
  TIMER_API_LOG(TIMER_API_LOG_TIMER_DEF,timer,now,NULL);

  if(rex_self() == timer_tcb_ptr){
    ATS_ERR("Timer def called from Timer context.");
  }

  /* Initialize the timer buffer if it is not initialized */
  if(!timer_q_init_status)
  {
    timer_init_buffer_queue();
    timer_q_init_status = 1;
  }

  /* Put the timer priority information */
  timer->timer_priority = timer_prio;    
  
  /* Initialize new timer structure */
  status = timer_define( timer, group );

  if (status != TE_SUCCESS)
  	{
  	 ATS_INTFREE();
  	 return status;
  	}

  /* Assign the below members of the external client timer
     structure only after timer_define has been completed */
  timer->tcb	  = tcb;
  timer->sigs	  = sigs; 	 
  timer->func1	  = func;
  timer->func2	   = NULL;
  timer->data	  = data;
  timer->notify_type = 0;

  /* Update the notification type */
  if (sigs == 0 && func == NULL)	 
  {
   timer->notify_type = TIMER_NO_NOTIFY_TYPE;
  }
  else
  {
   if(sigs!=0)
  	 timer->notify_type |= TIMER_REX_TCB_SIGNAL_TYPE;
   if(func!=NULL)
  	 timer->notify_type |= TIMER_FUNC1_CB_TYPE;
  }



  ATS_INTFREE();
  return TE_SUCCESS;

} /* timer_def */



/*=============================================================================

FUNCTION TIMER_DEF2

DESCRIPTION
  Defines a 'clock callback' style timer.

  When the timer expires:
    if func(time_ms, data) is called from interrupt context,

  Timers may also be staticly declared via the TIMER_DEF2() macro

DEPENDENCIES
  None

RETURN VALUE
  TE_INVALID_STATE_FOR_OP  If the timer is in paused state
  TE_SUCCESS  If the operation completed successfully


SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_def2
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group
)
{
  return timer_def2_priority(timer, group, TIMER_PRIORITY_1);
}

/*=============================================================================

FUNCTION TIMER_DEF2_PRIORITY

DESCRIPTION
  Defines a 'clock callback' style timer and with an ability to set
  the timer priority.

  When the timer expires:
    if func(time_ms, data) is called from interrupt context,

  Timers may also be staticly declared via the TIMER_DEF2() macro

DEPENDENCIES
  None

RETURN VALUE
  TE_INVALID_STATE_FOR_OP  If the timer is in paused state
  TE_SUCCESS  If the operation completed successfully

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_def2_priority
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group,

  timer_priority_enum_type        timer_prio
)
{
  time_timetick_type              now = 0;
  timer_error_type                status = TE_SUCCESS;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(timer == NULL)
  	{
     ASSERT(0);
	 return TE_INVALID_PARAMETERS;
  	}


  DalTimetick_GetTimetick64(hTimerHandle, &now);

  /* Initialize the timer buffer if it is not initialized */
  if( !timer_q_init_status )
  {
    timer_init_buffer_queue();
    timer_q_init_status = 1;
  }

  ATS_INTLOCK();

  TIMER_API_LOG(TIMER_API_LOG_TIMER_DEF2,timer,now,NULL);
  
  timer->timer_priority = timer_prio;

  /* Initialize new timer structure */
  status = timer_define( timer, group );

  ATS_INTFREE();
  return status;

  /* timer_reg() will fill in the timer members "func2" and "data" */


} /* timer_def2 */



/*=============================================================================

FUNCTION TIMER_REG

DESCRIPTION
  Initializes a 'clock callback' style timer.

  When the timer expires:
    if func(time_ms, data) is called from interrupt context,

DEPENDENCIES
  Timer must have been defined using timer_def2() or TIMER_DEF2()

RETURN VALUE
  TE_INVALID_STATE_FOR_OP  If the timer is not in the right state (defined,cleared,set,etc..)
  TE_INVALID_PARAMETERS  Passed params are incorrect for this API
  TE_SUCCESS


SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_reg
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Function to call at timer expiry */
  timer_t2_cb_type                func,

  /* Arbitrary data for func(time_ms, data) */
  timer_cb_data_type              data,

  /* Time (in ms) until first timer expiry */
  timetick_type                   time_ms,

  /* Period (in ms) between repeated expiries (0 = not periodic) */
  timetick_type                   reload_ms
)
{
  time_timetick_type              now = 0;
  timer_error_type                status = TE_SUCCESS ;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(timer == NULL)
  	{
     ASSERT(0);
	 return TE_INVALID_PARAMETERS;
  	}  

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  /* Lock before updating the timer structure */
  ATS_INTLOCK();

  TIMER_API_LOG(TIMER_API_LOG_TIMER_REG,timer,now,NULL);

  /* Copy function & function data to timer structure */
  timer->func2    = func;
  timer->func1    = NULL;
  timer->data     = data;
  timer->tcb      = NULL;
  timer->sigs     = (rex_sigs_type ) 0;
  timer->notify_type = TIMER_FUNC2_CB_TYPE;

  /* Start the timer */
  status = timer_set(timer, time_ms, reload_ms, T_MSEC );

  /* Release the lock */
  ATS_INTFREE();

  return status;


} /* timer_reg */



/*=============================================================================

FUNCTION TIMER_SET

DESCRIPTION
  Sets an inactive timer to expire after a given period of time, or changes
  an active timer to expire after a given period of time.

  Optionally, specifies the timer to repeatly expire with a given period.
  If units are ticks, then ticks are supplied in 32kHz domain.

DEPENDENCIES
  None

RETURN VALUE
  TE_INVALID_STATE_FOR_OP  If the timer is not in the right state (defined,cleared,set,etc..)
  TE_INVALID_PARAMETERS  Passed params are incorrect for this API
  TE_SUCCESS


SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/
timer_error_type timer_set
(
  timer_ptr_type                  timer,
  timetick_type                   time,
  timetick_type                   reload,
  timer_unit_type             unit
)
{
  /* If the unit type is in legacy ticks, we will convert to new timetick */
  if( unit == T_TICK || unit == T_NONE )
  {
    return timer_set_64(timer,
  			   timer_conv_to_xo_freq(time),
  			   timer_conv_to_xo_freq(reload),
  			   unit);
  }
  else
  {
    return timer_set_64(timer,
  			   time,
  			   reload,
  			   unit);
  }
} /* timer_set */

/*=============================================================================

FUNCTION TIMER_SET_GET_CMD_BUF

DESCRIPTION
  Gets a command buffer for a free timer and fills it out with timer info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static inline timer_error_type timer_set_get_cmd_buf
(
  timer_ptr_type                  timer,
  timer_ptr_internal_type         *free_timer_in
)
{
#ifdef FEATURE_TIMER_DETECT_MEM_LEAKS 
  uint32 iter = 0;
#endif /*FEATURE_TIMER_DETECT_MEM_LEAKS*/
  int32  temp_index;
  timer_ptr_internal_type free_timer = NULL;
  free_timer = timer_get_internal_timer( timer );
  if (free_timer != NULL &&
      free_timer->timer_ext != timer)
  {
    ERR_FATAL("External timer/handle mismatch! TimerInternal:0x%x timer:0x%x",free_timer,timer,0);
  }

  if(free_timer 
     && TIMER_PAUSED_FLAG == timer->timer_state) 
  {
    //ATS_MSG_ERROR("Timer 0x%x is a PAUSED timer that will reset",timer,0,0);    
    timer_remove_internal_timer_from_q(free_timer);
    timer_count_paused_timers--;
  }

  /* Remove timer from timer list, if any */
  if ( free_timer != NULL ) 
  {
    if ( free_timer->internal_q == TIMER_NOT_IN_FREE_Q )
    {
      timer_remove(free_timer, TRUE);
    }
  }

#ifdef FEATURE_TIMER_DETECT_MEM_LEAKS 

  /*iterate through timer_buffer to see if we have an active timer
   with an external memory and different handle. This is an error*/
  for(iter = 0; iter < MAX_TIMER_BUFFER_NUMBER ; iter++ ) 
  {
    if(timer_buffer[iter].timer_ext == timer 
       && timer_buffer[iter].internal_q == TIMER_NOT_IN_FREE_Q
      ) 
    {
      ERR_FATAL("Trying to set the same timer with different handle mem=0x%x, handle=0x%x",timer, iter+1,0);
    }
  }

#endif


  /* Get a free timer from Queue */
  free_timer = (timer_ptr_internal_type) q_get( &timer_free_q );

  if(free_timer == NULL)
	 {
	  ASSERT(0);
	  // ERR_FATAL("Free timer depleted",0,0,0);
	  return TE_INVALID_PARAMETERS;
	 }	

  /*increase the count of timers in timer_buffer*/
  timer_count_buffer_allocated = MAX_TIMER_BUFFER_NUMBER-q_cnt(&timer_free_q);

  temp_index = free_timer->index; 
  timer->timer_handle = INDEX_TO_HANDLE( (uint32)temp_index ); 
  timer->timer_state = TIMER_SET_FLAG;

  memset( free_timer, 0, sizeof( timer_internal_type ));

  free_timer->index = temp_index;

  free_timer->internal_q = TIMER_NOT_IN_FREE_Q;  
  /* Most fields are 0's or NULL's */

  if ( timer->group == NULL )
  {   
    timer->group = &timer_null_group;
    timer->timer_priority = TIMER_PRIORITY_1;
  }

  /* Copy data to Internal timer structure */


  /* Update the thread ID for the caller */
  free_timer->thread_id = TIME_OSAL_GET_THREAD_ID();

  /* Copy the remaining information from external timer to the internal timer */
  free_timer->group               = timer->group;
  free_timer->slave_task_assigned = timer->slave_task_assignment;
  free_timer->timer_priority      = timer->timer_priority;
  free_timer->notify_type         = timer->notify_type;
  free_timer->tcb                 = timer->tcb;
  free_timer->sigs                = timer->sigs;
  free_timer->func1               = timer->func1;
  free_timer->func2               = timer->func2;
  free_timer->data                = timer->data;
  free_timer->timer_ext = timer;  

  *free_timer_in = free_timer;

  return TE_SUCCESS;

} /*timer_set_get_cmd_buf*/


/*=============================================================================

FUNCTION TIMER_SET_64

DESCRIPTION
  Sets an inactive timer to expire after a given period of time, or changes
  an active timer to expire after a given period of time.

  Optionally, specifies the timer to repeatly expire with a given period.
  If units are ticks, then ticks are supplied in 19MHz domain.

DEPENDENCIES
  None

RETURN VALUE
  TE_INVALID_STATE_FOR_OP  If the timer is not in the right state (defined,cleared,set,etc..)
  TE_INVALID_PARAMETERS  Passed params are incorrect for this API
  TE_SUCCESS


SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

timer_error_type timer_set_64
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Time (in units below) until first timer expiry */
  time_timetick_type                   time,

  /* Period (in units) between repeated expiries (0 = not periodic) */
  time_timetick_type                   reload,

  /* Unit to measure "ticks" in. */
  timer_unit_type                 unit
)
{
   /* Local internal timer ptr */
   timer_ptr_internal_type       free_timer;

   /* Temp Index */
   time_timetick_type           now = 0;
   
   timer_error_type             status;

   #ifdef FEATURE_TIMER_DETECT_MEM_LEAKS
   int                           iter=0;
   #endif /*FEATURE_TIMER_DETECT_MEM_LEAKS*/

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(timer == NULL)
	 {
	  ASSERT(0);
	  return TE_INVALID_PARAMETERS;
	 }	

  if ( timer->group == NULL )
  {
    rex_tcb_type *tcb_ptr = rex_self();
#ifdef FEATURE_REX_OPAQUE_TCB_APIS
    char rex_task_name[REX_TASK_NAME_LEN+1];
    rex_get_task_name(tcb_ptr, rex_task_name, sizeof(rex_task_name), NULL);
#endif
#ifdef BUILD_BOOT_CHAIN
    #ifdef FEATURE_REX_OPAQUE_TCB_APIS
    ATS_MSG_HIGH("[timer_set] : timer_def was not called for timer %x, task %s",
             timer, rex_task_name, 0);    
    #else
    ATS_MSG_HIGH("[timer_set] : timer_def was not called for timer %x, task %s",
             timer, tcb_ptr->task_name, 0);
    #endif
#else
    #ifdef FEATURE_REX_OPAQUE_TCB_APIS
    ATS_MSG_SPRINTF_2("[timer_set] : timer_def was not called for timer %x, task %s", timer, 
                  rex_task_name);
    #else
    ATS_MSG_SPRINTF_2("[timer_set] : timer_def was not called for timer %x, task %s", timer, 
                  tcb_ptr->task_name);
    #endif
#endif
    
  }

  ATS_INTLOCK();

  /* Check if the current timer state is incorrect & return accordingly */
  if ( timer->timer_state < TIMER_DEF_FLAG ||
  	   timer->timer_state > TIMER_RESUMED_FLAG )
  	{
  	  ATS_ERR_2("Timer 0x%x not in correct state 0x%x to be passed to timer_set()", timer, timer->timer_state);
      ATS_INTFREE();
 	  return TE_INVALID_STATE_FOR_OP;
  	}

  DalTimetick_GetTimetick64(hTimerHandle, &now);
  TIMER_API_LOG(TIMER_API_LOG_TIMER_SET,timer,now,timer_get_internal_timer(timer));

  status = timer_set_get_cmd_buf(timer,&free_timer);
  if (status != TE_SUCCESS)
   {
     ATS_INTFREE();
	 return status;
   }

  /* Defensive programming. Clearing the signal that will
   be set when the timer expires.*/
  if ( free_timer->notify_type & TIMER_NATIVE_OS_SIGNAL_TYPE )
	  {
	    #ifdef FEATURE_TIME_QDSP6
		qurt_anysignal_clear( (qurt_anysignal_t *)free_timer->tcb, (time_osal_notify_data)free_timer->sigs );
		#else
          ERR_FATAL("Signal cannot be of NATIVE OS type. timer: 0x%x", timer, 0, 0);
		#endif /* FEATURE_TIME_QDSP6 */
	  }
  else if ( free_timer->notify_type & TIMER_REX_TCB_SIGNAL_TYPE )
  	{
  	#ifdef FEATURE_TIMER_REX_IS_AVAILABLE
       (void) rex_clr_sigs( free_timer->tcb, free_timer->sigs );
	#else
	   ERR_FATAL("Signal cannot be of rex type. timer: 0x%x", timer, 0, 0);
	#endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
  	}
  else if ( free_timer->notify_type & TIMER_NATIVE_OS_SIGNAL2_TYPE )
	  {
	    #ifdef FEATURE_TIME_QDSP6
		  qurt_signal2_clear( (qurt_signal2_t *)free_timer->tcb, (time_osal_notify_data)free_timer->sigs );
		#else
          ERR_FATAL("Signal cannot be of NATIVE OS type. timer: 0x%x", timer, 0, 0);
		#endif /* FEATURE_TIME_QDSP6 */
	  }
  else if (free_timer->notify_type & TIMER_DALSYS_EVENT_SIGNAL_TYPE)
     {
        timer_dalsys_sig_reset(free_timer->tcb);
     }

    /* ... convert given duration into slow clocks, and save in cache */
  free_timer->cache.duration      = time;

  /* Convert the passed unit to XO ticks */
  DalTimetick_CvtToTimetick64(hTimerHandle,time,unit,&free_timer->cache.duration_sclk);

  /* Retrieve converted time duration from cache */
  time = free_timer->cache.duration_sclk;

      /* Convert and cache given reload interval into slow clocks */
  free_timer->cache.reload      = reload;
  DalTimetick_CvtToTimetick64(hTimerHandle,reload,unit,&free_timer->cache.reload_sclk);


    /* Retrieve converted reload interval from cache */
  reload = free_timer->cache.reload_sclk;

  /* Ensure cached unit value is correct */
  free_timer->cache.unit = unit;

  /* Determine when timer should expire, and set reload */

  DalTimetick_GetTimetick64(hTimerHandle, &free_timer->start);
  free_timer->expiry = free_timer->start + time;
  free_timer->reload = reload;
  DalTimetick_GetOffset(hTimerHandle,&free_timer->sclk_delta);

  /* Insert timer in either active timer list, or group timer list if it
     belongs to a disabled timer group */

  if ( free_timer->group->disabled )
  {
    /* Timer belongs to a disabled group - add it to that group's list */
    timer_insert( &free_timer->group->list, free_timer );
  }
  else
  {
    if(TIMER_PRIORITY_0 == free_timer->timer_priority) 
    {
      /* Timer is not a member of a disabled group - add to active list */
      timer_insert( &timers.p0_active.list, free_timer);
    }
    else
    {
    /* Timer is not a member of a disabled group - add to active list */
    timer_insert( &timers.active.list, free_timer );
    }

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_TIMER_SET);
  }

  if ( timer->timer_handle == TIMER_BUFFER_INVALID_HANDLE )
    ERR_FATAL (" Wrong timer buffer handle, timer_ptr=0x%x rextcb=0x%x" , timer,timer->tcb,0);

  ATS_INTFREE();

  return TE_SUCCESS;
} /* timer_set_64 */


/*=============================================================================

FUNCTION TIMER_SET_ABSOLUTE

DESCRIPTION
  Sets an inactive timer to expire at a given time, or changes
  an active timer to expire at a given time.

DEPENDENCIES
  None

RETURN VALUE
  TE_INVALID_STATE_FOR_OP  If the timer is not in the right state (defined,cleared,set,etc..)
  TE_INVALID_PARAMETERS  Passed params are incorrect for this API
  TE_SUCCESS


SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

timer_error_type timer_set_absolute
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Time (in XOs) until first timer expiry */
  time_timetick_type              time

)
{
  /* Local internal timer ptr */
  timer_ptr_internal_type       free_timer = NULL;
  time_timetick_type            preintlock_val,postintlock_val;
  
  timer_error_type              status ;

  #ifdef FEATURE_TIMER_DETECT_MEM_LEAKS
  int                           iter=0;
  #endif /*FEATURE_TIMER_DETECT_MEM_LEAKS*/

 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 if (timer == NULL)
 	{
	 ASSERT(0);
	 return TE_INVALID_PARAMETERS;
 	}

 if ( timer->group == NULL )
 {
   rex_tcb_type *tcb_ptr = rex_self();
#ifdef FEATURE_REX_OPAQUE_TCB_APIS
   char rex_task_name[REX_TASK_NAME_LEN+1];
   rex_get_task_name(tcb_ptr, rex_task_name, sizeof(rex_task_name), NULL);
#endif

#ifdef BUILD_BOOT_CHAIN
   #ifdef FEATURE_REX_OPAQUE_TCB_APIS
   ATS_MSG_HIGH("[timer_set] : timer_def was not called for timer %x, task %s",
            timer, rex_task_name, 0);    
   #else
   ATS_MSG_HIGH("[timer_set] : timer_def was not called for timer %x, task %s",
            timer, tcb_ptr->task_name, 0);
   #endif
#else
   #ifdef FEATURE_REX_OPAQUE_TCB_APIS
   ATS_MSG_SPRINTF_2("[timer_set] : timer_def was not called for timer %x, task %s", timer, 
                 rex_task_name);
   #else
   ATS_MSG_SPRINTF_2("[timer_set] : timer_def was not called for timer %x, task %s", timer, 
                 tcb_ptr->task_name);
   #endif
#endif

 }

 /* Check if the current timer state is incorrect & return accordingly */
 if ( timer->timer_state < TIMER_DEF_FLAG ||
	  timer->timer_state > TIMER_RESUMED_FLAG )
   {
	 ATS_ERR_2("Timer 0x%x not in correct state to be passed to timer_set()",timer,timer->timer_state);
   }

 /* Specific to set_hard function... check to see if we pass expiry when acq lock */
 DalTimetick_GetTimetick64(hTimerHandle, &preintlock_val);

 ATS_INTLOCK();

 DalTimetick_GetTimetick64(hTimerHandle, &postintlock_val);

 TIMER_API_LOG(TIMER_API_LOG_TIMER_SET,timer,postintlock_val,timer_get_internal_timer(timer));

 status = timer_set_get_cmd_buf(timer,&free_timer);
 if (status != TE_SUCCESS)
  {
    ATS_INTFREE();
	return status;
  }
  
  /* Defensive programming. Clearing the signal that will
  be set when the timer expires.*/
  if ( free_timer->notify_type & TIMER_REX_TCB_SIGNAL_TYPE )
   {
   #ifdef FEATURE_TIMER_REX_IS_AVAILABLE
   (void) rex_clr_sigs( free_timer->tcb, free_timer->sigs );
   #else
   ERR_FATAL("Signal cannot be of rex type. timer: 0x%x", timer, 0, 0);
   #endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
   }
  else if ( free_timer->notify_type & TIMER_NATIVE_OS_SIGNAL_TYPE )
   {
   #ifdef FEATURE_TIME_QDSP6
    qurt_anysignal_clear( (qurt_anysignal_t *)free_timer->tcb, (time_osal_notify_data)free_timer->sigs );
   #else
     ERR_FATAL("Signal cannot be of NATIVE OS type. timer: 0x%x", timer, 0, 0);	
   #endif /* FEATURE_TIME_QDSP6 */
   }
  else if ( free_timer->notify_type & TIMER_NATIVE_OS_SIGNAL2_TYPE )
   {
   #ifdef FEATURE_TIME_QDSP6
    qurt_signal2_clear( (qurt_signal2_t *)free_timer->tcb, (time_osal_notify_data)free_timer->sigs );
   #else
     ERR_FATAL("Signal cannot be of NATIVE OS type. timer: 0x%x", timer, 0, 0);	
   #endif /* FEATURE_TIME_QDSP6 */
   } 
  else if (free_timer->notify_type & TIMER_DALSYS_EVENT_SIGNAL_TYPE)
     {
        timer_dalsys_sig_reset(free_timer->tcb);
     }   
  
  if( postintlock_val - preintlock_val  > time - preintlock_val )
  {
    /* We will want to expire this timer right away */
    /* ... convert given duration into slow clocks, and save in cache */
    free_timer->cache.duration      = 0;
    free_timer->cache.duration_sclk = 0;
    
    /* No reload allowed for this */
    free_timer->cache.reload      = 0;
    free_timer->cache.reload_sclk = 0;
    
    /* Ensure cached unit value is correct */
    free_timer->cache.unit = T_TICK;
    
    /* Determine when timer should expire, and set reload */
    
    free_timer->start  = postintlock_val;
    free_timer->expiry = postintlock_val;
    free_timer->reload = 0;
    DalTimetick_GetOffset(hTimerHandle,&free_timer->sclk_delta);
  }
  else
  {  
   /* ... convert given duration into slow clocks, and save in cache */
   free_timer->cache.duration      = time - postintlock_val;
   free_timer->cache.duration_sclk = free_timer->cache.duration;
  
       /* No reload allowed for this */
   free_timer->cache.reload      = 0;
   free_timer->cache.reload_sclk = 0; /* Is this okay? */
  
   /* Ensure cached unit value is correct */
   free_timer->cache.unit = T_TICK;
  
   /* Determine when timer should expire, and set reload */
  
   free_timer->start  = postintlock_val;
   free_timer->expiry = time;
   free_timer->reload = 0;
   DalTimetick_GetOffset(hTimerHandle,&free_timer->sclk_delta);
  }
  
    /* Insert timer in either active timer list, or group timer list if it
     belongs to a disabled timer group */
  
  if ( free_timer->group->disabled )
  {
    /* Timer belongs to a disabled group - add it to that group's list */
    timer_insert( &free_timer->group->list, free_timer );
  }
  else
  {
    if(TIMER_PRIORITY_0 == free_timer->timer_priority) 
    {
      /* Timer is not a member of a disabled group - add to active list */
      timer_insert( &timers.p0_active.list, free_timer);
    }
    else
    {
      /* Timer is not a member of a disabled group - add to active list */
      timer_insert( &timers.active.list, free_timer );
    }
  
    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_TIMER_SET);
  }
  
  if ( timer->timer_handle == TIMER_BUFFER_INVALID_HANDLE )
    ERR_FATAL (" Wrong timer buffer handle, timer_ptr=0x%x rextcb=0x%x" , timer,timer->tcb,0);

 ATS_INTFREE();
 
  return TE_SUCCESS;
}


/*=============================================================================

FUNCTION TIMER_EXPIRES_IN

DESCRIPTION
  Get number of ticks before timer expires

DEPENDENCIES
  Timer must be active (on a timer list)
  Internal function, called from ATS_INTLOCK context.

RETURN VALUE
  Number of ticks before timer expires.
  Zero is returned for inactive timers, and timers that have expired or
  are expiring.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_expires_in
(
  /* Timer to get */
  timer_ptr_internal_type         timer
)
{
  /* Current slow clock tick count */
  time_timetick_type                   now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current slow clock count value */

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  /* Determine if the timer has expired or not */

  if ( timer->expiry >=  now  )
  {
    /* Timer has not expired - compute time till expiry. */

    return ( timer->expiry - now );
  }
  else
  {
    /* Timer has expired - return zero */

    return 0;
  }

} /* timer_expires_in */



/*=============================================================================

FUNCTION TIMER_GET

DESCRIPTION
  Get number of ticks in 32kHz domain before timer expires

DEPENDENCIES
  None

RETURN VALUE
  Number of ticks in 32kHz domain before timer expires.
  Zero is returned for inactive timers, and timers that have expired or
  are expiring. Fractional value which might truncate to zero 
  depending on unit passed as parameter is rounded to 1.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_get
(
  timer_ptr_type                  timer,
  timer_unit_type                 unit
)
{
  if( unit == T_TICK || unit == T_NONE )
  {
    return timer_conv_to_sclk_freq(timer_get_64(timer,unit));
  }
  else
  {
    return (timetick_type)timer_get_64(timer,unit);
  }
} /* timer_get */


/*=============================================================================

FUNCTION TIMER_GET_64

DESCRIPTION
  Get number of ticks in 19MHz domain before timer expires

DEPENDENCIES
  None

RETURN VALUE
  Number of ticks in 19MHz domain before timer expires.
  Zero is returned for inactive timers, and timers that have expired or
  are expiring. Fractional value which might truncate to zero 
  depending on unit passed as parameter is rounded to 1.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_get_64
(
  /* Timer to get */
  timer_ptr_type                  timer,

  /* Unit to measure "ticks before timer expires" in. */
  timer_unit_type             unit
)
{
  /* Ticks until timer expires */
  time_timetick_type                   ticks;

  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  ATS_DECLARE_ISAV(isave);	
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(timer == NULL)
	 {
	  ASSERT(0);
	  return 0;
	 }	
  
  ATS_INTLOCK_SAV(isave);

  timer_int = timer_get_internal_timer ( timer );
  DalTimetick_GetTimetick64(hTimerHandle, &ticks);
  /* This API LOG should be before any possible early exit of the function */
  TIMER_API_LOG(TIMER_API_LOG_TIMER_GET,timer,ticks,timer_int);

  if ( timer_int == NULL )
  {  
    ATS_INTFREE_SAV(isave);
    return 0;
  }

  if ( timer_int->list != NULL )
  {
    /* Timer is active. Ensure non-zero tick count */

    ticks = timer_expires_in( timer_int );
    if ( ticks == 0 )
    {
      ticks = 1;
    }

  }
  else
  {
    /* Timer is not active */

    ticks = 0;
  }

  ATS_INTFREE_SAV(isave);

  /* If timer is active then ensure that return value is non-zero */
  if ( ticks != 0 )
  {
    DalTimetick_CvtFromTimetick64(hTimerHandle,ticks,unit,&ticks);
    if ( ticks == 0 )
    {
      ticks = 1;
    }
  }

  return ticks;

} /* timer_get_64 */


/*=============================================================================

FUNCTION TIMER_GET_PARAMS

DESCRIPTION
  Updates the passed structure as per the timer parameters.

DEPENDENCIES
   None

RETURN VALUE
  TE_SUCCESS if success
  TE_INVALID_PARAMETERS if passed pointers are NULL


SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_get_params
(
  timer_ptr_type                    timer,
  timer_attrib_struct_type *        timer_attrib_struct_ptr
)
{
  /* return error if the passed timer pointer is NULL */
  if (timer==NULL || timer_attrib_struct_ptr == NULL)
	  return TE_INVALID_PARAMETERS;
  
  ATS_INTLOCK();
  /* Update the passed timer attribute structure members */
  timer_attrib_struct_ptr->tcb_sigs_obj      = (void *)timer->tcb;
  timer_attrib_struct_ptr->sigs              = (uint32)timer->sigs;
  timer_attrib_struct_ptr->func1             = timer->func1;
  timer_attrib_struct_ptr->func2             = timer->func2;
  timer_attrib_struct_ptr->data              = timer->data;
  timer_attrib_struct_ptr->notify_type       = timer->notify_type;
  timer_attrib_struct_ptr->timer_state       = timer->timer_state;

  ATS_INTFREE();
  return TE_SUCCESS;
} /* timer_get_params */




/*=============================================================================

FUNCTION TIMER_SET_PARAMS

DESCRIPTION
  Updates the timer as per the passed params.

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS if success
  TE_INVALID_PARAMETERS if passed pointers are NULL


SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_set_params
(
  timer_ptr_type                    timer,
  timer_attrib_set_type             to_set,
  timer_attrib_struct_type *        timer_attrib_struct_ptr
)
{
  timer_state_struct_type  tmp_timer_state;
  time_timetick_type       ticks = 0;
  timer_ptr_internal_type  int_timer = NULL;

  /* return error if the passed timer pointer is NULL */
  if (timer==NULL || timer_attrib_struct_ptr == NULL)
	  return TE_INVALID_PARAMETERS;
  
  ATS_INTLOCK();
  /* Update the timer notification attributes */
  if (to_set == TIMER_UPDATE_NOTIFICATION_PARAMS)
  	{
  	  timer->notify_type |= timer_attrib_struct_ptr->notify_type;
	  
  	  if ( (timer_attrib_struct_ptr->notify_type & TIMER_DALSYS_EVENT_SIGNAL_TYPE) ||
           (timer_attrib_struct_ptr->notify_type & TIMER_NATIVE_OS_SIGNAL_TYPE) ||
           (timer_attrib_struct_ptr->notify_type & TIMER_NATIVE_OS_SIGNAL2_TYPE) ||
	  	   (timer_attrib_struct_ptr->notify_type & TIMER_REX_TCB_SIGNAL_TYPE) )
  	  	{
  	  	  timer->tcb   = (rex_tcb_type *)timer_attrib_struct_ptr->tcb_sigs_obj;
		  timer->sigs  = (rex_sigs_type)timer_attrib_struct_ptr->sigs;
  	  	}

	  if (timer_attrib_struct_ptr->notify_type & TIMER_FUNC1_CB_TYPE)
	  	{
          timer->func1  = timer_attrib_struct_ptr->func1;
		  timer->data  = timer_attrib_struct_ptr->data;
	  	}

	  if (timer_attrib_struct_ptr->notify_type & TIMER_FUNC2_CB_TYPE)
	  	{
          timer->func2= timer_attrib_struct_ptr->func2;
		  timer->data  = timer_attrib_struct_ptr->data;
	  	}

	  if (timer_attrib_struct_ptr->notify_type == TIMER_NO_NOTIFY_TYPE)
	  	{
  	  	  timer->tcb   = (rex_tcb_type *)NULL;
     	  timer->sigs  = (rex_sigs_type)0;
	      timer->func1  = (timer_t1_cb_type)NULL;
	      timer->func2  = (timer_t2_cb_type)NULL;
	      timer->data  = (timer_cb_data_type)0;
	  	}

 	}
  else 
  	{
	ATS_INTFREE();
  	return TE_INVALID_PARAMETERS;
  	}

  /* update the data for the corresponding internal timer as well */
  int_timer = timer_get_internal_timer(timer);

  /* Check if there is any internal timer for this external timer as well */
  if (int_timer != NULL)
  	{
     int_timer->notify_type = timer->notify_type;
	 int_timer->tcb           = timer->tcb;
	 int_timer->sigs          = timer->sigs;
	 int_timer->func1         = timer->func1;
	 int_timer->func2         = timer->func2;
	 int_timer->data          = timer->data;
  	}

  /* Get the timer's current state, for logging purpose */
  tmp_timer_state = timer->timer_state ;

  /* Log whenever somebody updates the timer structure */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks);
  timer_dbg_log_set_param ( timer, ticks, tmp_timer_state, timer_attrib_struct_ptr->notify_type );

  ATS_INTFREE();

  ATS_MSG_HIGH("timer_set_params called : timer = 0x%x; notification_type = 0x%x ; timetick = 0x%llx",
  	        timer,
  	        timer_attrib_struct_ptr->notify_type,
  	        ticks);


  return TE_SUCCESS;
} /* timer_set_params */


/*=============================================================================

FUNCTION TIMER_CLR

DESCRIPTION
  Stops an active timer

DEPENDENCIES
  None

RETURN VALUE
  Number of ticks, in 32kHz domain, remaining before expiry.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_clr
(
  timer_ptr_type                  timer,
  timer_unit_type                 unit
)
{
  if( unit == T_TICK || unit == T_NONE )
  {
    return timer_conv_to_sclk_freq(timer_clr_64(timer,unit));
  }
  else
  {
    return (timetick_type)timer_clr_64(timer,unit);
  }  
  
} /* timer_clr */


/*=============================================================================

FUNCTION TIMER_CLR_64

DESCRIPTION
  Stops an active timer

DEPENDENCIES
  None

RETURN VALUE
  Number of ticks, in 19MHz domain, remaining before expiry.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_clr_64
(
  /* Timer to stop */
  timer_ptr_type                  timer,

  /* Unit to measure "ticks before expiry" in.  Use T_NONE if not required */
  timer_unit_type             unit
)
{

time_timetick_type	rem_time = 0;

 (void) timer_stop(timer,unit,&rem_time);

return rem_time;

} /* timer_clr_64 */


/*=============================================================================

FUNCTION timer_undef

DESCRIPTION
  Deletes an active timer

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS if success
  TE_INVALID_PARAMETERS if timer  pointer passed is invalid
  TE_TIMER_ALREADY_IN_SAME_STATE if timer is already deleted

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_undef
(
  timer_ptr_type                  timer
)
{
  time_timetick_type        ticks = 0;
 
  /* Validate the input */
  if (timer == NULL)
  	return TE_INVALID_PARAMETERS;

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &ticks);
  
  TIMER_API_LOG(TIMER_API_LOG_TIMER_UNDEF,timer,ticks,timer_get_internal_timer(timer));

  if(timer->timer_state < TIMER_DEF_FLAG || timer->timer_state > TIMER_RESUMED_FLAG)
  {
     ATS_INTFREE();
     return TE_TIMER_ALREADY_IN_SAME_STATE;
  }
  
  /* We have already checked the input params, so no need to recheck again
        for the return status of timer_stop, as we will be doing memset on the 
        external timer in any case, whether it is inactive / already in same state. */
  (void)timer_stop(timer, T_NONE, NULL);
  
  /* Zero out the timer */
  memset( timer, 0, sizeof(timer_type));
  
  timer->timer_state = TIMER_UNDEF_FLAG;
  
  ATS_INTFREE();
  return TE_SUCCESS;

} /* timer_undef */


/*=============================================================================

FUNCTION timer_stop

DESCRIPTION
  Stops an active timer

DEPENDENCIES
  None

RETURN VALUE
  TE_INVALID_PARAMETERS
  TE_TIMER_ALREADY_IN_SAME_STATE
  TE_SUCCESS

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_stop
(
  timer_ptr_type				   timer,
  timer_unit_type	           unit,
  time_timetick_type			   *rem_time
)
{

  /* Ticks until timer expires */
  time_timetick_type              ticks = 0, call_time = 0;

  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;

  timer_group_ptr                 local_timer_group=NULL;

  timer_error_type                status = TE_SUCCESS;

  timer_state_struct_type         old_state;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_DECLARE_ISAV(isave);
 
  ATS_INTLOCK_SAV(isave);

  DalTimetick_GetTimetick64(hTimerHandle, &ticks);

  call_time = ticks;

  /* Get the corresponding internal timer */
  timer_int = timer_get_internal_timer ( timer );  

  old_state = timer->timer_state;
  
  TIMER_API_LOG(TIMER_API_LOG_TIMER_CLR,timer,ticks,timer_int);
  if ( timer_int == NULL )
  { 
    timer_dbg_log_timer_clr( timer, timer_int, call_time, TE_TIMER_ALREADY_IN_SAME_STATE, old_state, rex_self() );
	
    ATS_INTFREE_SAV(isave);
    return TE_TIMER_ALREADY_IN_SAME_STATE;
  }

  local_timer_group = timer_int->group;
  if(NULL == timer_int->group) 
  {
    timer_int->group = &timer_null_group;
  } 

  timer->last_expiry = 0;
  
  if ( timer_int->list != NULL )
  {
    /* Timer is active - record remaining time and remove timer */

    ticks = timer_expires_in( timer_int );
    timer_remove( timer_int, TRUE );
	status = TE_SUCCESS;
	
    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_TIMER_CLEARED);
  }
  /*if the timer was paused, then it was removed from the active list
   or the disabled list, but it was not returned back to the free 
   queue. Hence, if a paused timer was cleared, return it back
   to the free queue*/
  else if(timer->timer_state == TIMER_PAUSED_FLAG) 
  {
    timer_remove_internal_timer_from_q(timer_int);
	ticks = 0;
	status = TE_SUCCESS;
    timer_count_paused_timers--;
  }
  else
  {
    /* Timer was not active */
    ticks = 0;
	status = TE_TIMER_ALREADY_IN_SAME_STATE;
  }

  timer->timer_state = TIMER_CLEARED_FLAG;

  if ( local_timer_group == NULL )
  {
    rex_tcb_type *tcb_ptr = rex_self();
#ifdef FEATURE_REX_OPAQUE_TCB_APIS
    char rex_task_name[REX_TASK_NAME_LEN+1];
    rex_get_task_name(tcb_ptr, rex_task_name, sizeof(rex_task_name), NULL);
#endif
#ifdef BUILD_BOOT_CHAIN
    #ifdef FEATURE_REX_OPAQUE_TCB_APIS
    ATS_MSG_HIGH("[timer_clr] : timer_def was not called for timer %x, task %s",
             timer, rex_task_name, 0);
    #else
    ATS_MSG_HIGH("[timer_clr] : timer_def was not called for timer %x, task %s",
             timer, tcb_ptr->task_name, 0);
    #endif
#else
    #ifdef FEATURE_REX_OPAQUE_TCB_APIS
    ATS_MSG_SPRINTF_2("[timer_clr] : timer_def was not called for timer %x, task %s", timer, 
                  rex_task_name);   
    #else
    ATS_MSG_SPRINTF_2("[timer_clr] : timer_def was not called for timer %x, task %s", timer, 
                  tcb_ptr->task_name);
    #endif
#endif
  }

  if (rem_time != NULL)
  	{
	 DalTimetick_CvtFromTimetick64(hTimerHandle,ticks,unit,rem_time);
  
     /* Return at least 1 as the remaining time if the timer was cleared */
     if (status == TE_SUCCESS && *rem_time == 0)
     	*rem_time = 1;
  	}

  /* Timer clr logging */
  timer_dbg_log_timer_clr( timer, timer_int, call_time, status, old_state, rex_self() );
  
  ATS_INTFREE_SAV(isave);
  return status;
}


/*=============================================================================

FUNCTION TIMER_PAUSE

DESCRIPTION
  Pauses an active timer.

DEPENDENCIES
  Should only be called for a timer that is running.

RETURN VALUE
  TE_INVALID_TIMER_HANDLE  If the timer is not active
  TE_SUCCESS  If the operation was success


SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_pause
(
  /* Timer to pause */
  timer_ptr_type                  timer
)
{
  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  ATS_DECLARE_ISAV(isave);
   time_timetick_type             now = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(timer == NULL)
	 {
	  ASSERT(0);
	  return TE_INVALID_PARAMETERS;
	 }	

  if ( timer->group == NULL )
  {
    rex_tcb_type *tcb_ptr = rex_self();
#ifdef FEATURE_REX_OPAQUE_TCB_APIS
    char rex_task_name[REX_TASK_NAME_LEN+1];
    rex_get_task_name(tcb_ptr, rex_task_name, sizeof(rex_task_name), NULL);
#endif
#ifdef BUILD_BOOT_CHAIN
  #ifdef FEATURE_REX_OPAQUE_TCB_APIS
    ATS_MSG_HIGH("[timer_pause] : timer_def was not called for timer %x, task %s",
             timer, rex_task_name, 0);
  #else 
    ATS_MSG_HIGH("[timer_pause] : timer_def was not called for timer %x, task %s",
             timer, tcb_ptr->task_name, 0);
  #endif
#else
  #ifdef FEATURE_REX_OPAQUE_TCB_APIS
    ATS_MSG_SPRINTF_2("[timer_pause] : timer_def was not called for timer %x, task %s", timer, 
                  rex_task_name);
  #else
    ATS_MSG_SPRINTF_2("[timer_pause] : timer_def was not called for timer %x, task %s", timer, 
                  tcb_ptr->task_name);
  #endif
#endif
    
  }
  

  ATS_INTLOCK_SAV(isave);

  timer_int = timer_get_internal_timer ( timer );
  if (timer_int != NULL &&
      timer_int->timer_ext != timer)
  {
    ERR_FATAL("External timer/handle mismatch! InternalTimer:0x%x timer:0x%x",timer_int,timer,0);
  }
  DalTimetick_GetTimetick64(hTimerHandle, &now);
  TIMER_API_LOG(TIMER_API_LOG_TIMER_PAUSE,timer,now,timer_int);
  if ( timer_int == NULL )
  {  
    ATS_INTFREE_SAV(isave);
    return TE_INVALID_TIMER_HANDLE;
  }  
  
  if(NULL == timer->group) 
  {
     timer->group= &timer_null_group;
     timer->timer_priority = TIMER_PRIORITY_1;
  }

  if ( timer_int->list != NULL )
  {
    /* Timer is active - remove it, and record remaining time */

    timer_int->expiry = timer_expires_in( timer_int );
    
    /* Update sclk_delta */
    DalTimetick_GetOffset(hTimerHandle,&timer_int->sclk_delta);

    timer_remove( timer_int, FALSE );

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt( TIMER_MVS_TIMER_PAUSED);

    timer_count_paused_timers++;
  }

  timer->timer_state = TIMER_PAUSED_FLAG;
  ATS_INTFREE_SAV(isave);

  return TE_SUCCESS;
} /* timer_pause */



/*=============================================================================

FUNCTION TIMER_RESUME

DESCRIPTION
  Resumes a previously paused timer.

  If a timer was paused 5 seconds before its expiry, and 30 seconds later
  timer_resume( ) is called, the timer will expire 5 seconds after that point.

DEPENDENCIES
  Must only be called for a timer that has been paused.

RETURN VALUE
TE_INVAID_TIMER_HANDLE - If passed timer does not have any internal timer (it is not paused)
TE_SUCCESS - If the operation was success                                

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

timer_error_type timer_resume
(
  /* Timer to resume */
  timer_ptr_type                  timer
)
{
  /* Ticks until timer expires */
  time_timetick_type                   start;
  time_timetick_type              now = 0;

  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  ATS_DECLARE_ISAV(isave);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(timer == NULL)
	 {
	  ASSERT(0);
	  return TE_INVALID_PARAMETERS;
	 }	


  if ( timer->group == NULL )
  {
    rex_tcb_type *tcb_ptr = rex_self();
#ifdef FEATURE_REX_OPAQUE_TCB_APIS
    char rex_task_name[REX_TASK_NAME_LEN+1];
    rex_get_task_name(tcb_ptr, rex_task_name, sizeof(rex_task_name), NULL);
#endif
#ifdef BUILD_BOOT_CHAIN
  #ifdef FEATURE_REX_OPAQUE_TCB_APIS
    ATS_MSG_HIGH("[timer_resume] : timer_def was not called for timer %x, task %s",
             timer, rex_task_name, 0);
  #else
    ATS_MSG_HIGH("[timer_resume] : timer_def was not called for timer %x, task %s",
             timer, tcb_ptr->task_name, 0);
  #endif
#else
  #ifdef FEATURE_REX_OPAQUE_TCB_APIS
    ATS_MSG_SPRINTF_2("[timer_resume] : timer_def was not called for timer %x, task %s", timer, 
                  rex_task_name);
  #else
    ATS_MSG_SPRINTF_2("[timer_resume] : timer_def was not called for timer %x, task %s", timer, 
                  tcb_ptr->task_name);
  #endif
#endif
    
  }
  
  ATS_INTLOCK_SAV(isave);

  timer_int = timer_get_internal_timer ( timer );
  DalTimetick_GetTimetick64(hTimerHandle, &now);
  TIMER_API_LOG(TIMER_API_LOG_TIMER_RESUME,timer,now,timer_int);
  if ( timer_int == NULL )
  {  
    ATS_INTFREE_SAV(isave);
    return TE_INVALID_TIMER_HANDLE;
  }  

  if(NULL == timer->group) 
  {
    timer->group = &timer_null_group;
    timer->timer_priority = TIMER_PRIORITY_1;
  }

  if ( timer_int->list == NULL )
  {
    /* Determine when timer should expire */

    DalTimetick_GetTimetick64(hTimerHandle, &start);
    timer_int->expiry += start;

    /* Update sclk_delta */
    DalTimetick_GetOffset(hTimerHandle,&timer_int->sclk_delta);

    /* Insert timer in either active timer list, or group timer list if it
       belongs to a disabled timer group */

    if ( timer_int->group->disabled )
    {
      /* Timer belongs to a disabled group - add it to that group's list */
      timer_insert( &timer_int->group->list, timer_int );
    }
    else
    {
      if(TIMER_PRIORITY_0 == timer_int->timer_priority) 
      {
        /* Timer is not a member of a disabled group - add to active list */
        timer_insert( &timers.p0_active.list, timer_int);
      }
      else
      {
      /* Timer is not a member of a disabled group - add to active list */
      timer_insert( &timers.active.list, timer_int);
      }

      /* Active timer list has changed - ensure next timer event is correct */
      timer_update_timer_interrupt(TIMER_MVS_TIMER_RESUMED);
    }

    timer_count_paused_timers--;
  }
  timer->timer_state = TIMER_RESUMED_FLAG;

  ATS_INTFREE_SAV(isave);

  return TE_SUCCESS;

} /* timer_resume */

/*=============================================================================

FUNCTION TIMER_CLR_OSAL_THREAD_TIMERS

DESCRIPTION
  Stops active timers belonging to the given task thread

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_clr_osal_thread_timers
(
  /* Timers belonging to this task will be cleared (stopped). */
  time_osal_thread_id                    thread_id
)
{
  /* Pointer to a timer group.  Used for walking list of timer groups */
  timer_group_ptr                 group;

  /* Pointer to a timer.  Used for walking list of timers */
  timer_ptr_internal_type         timer;

  /* Pointer to the next timer.  Used for walking list of timers */
  timer_ptr_internal_type         next;

  /* Timetick value for logging */
  time_timetick_type              now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_CLR_TASK_TIMERS,NULL,now,NULL);

  /* Walk list of timer groups, starting with the faux active group */

  group = &timers.active;

  do
  {
    /* Walk the list of timers in this group, and
       remove timers which belong to given task */

    for( timer = group->list.first;  timer != NULL;  timer = next)
    {
      /* Save next timer in list, in case this timer's next field in NULL'd */
      next = timer->next;

      if ( timer->thread_id == thread_id)
      {
        if ( timer->timer_ext != NULL )
		 {
          timer->timer_ext->last_expiry = 0;
		  timer->timer_ext->timer_state = TIMER_CLEARED_FLAG;
		 }
        else
          {
           ATS_INTFREE();
           ERR_FATAL (" Timer must have an ext timer, timer_ptr=0x%x timer_ext=0x%x",
                    timer,timer->timer_ext,0);
		   return;
          }

        /* Remove the timer from the list */
        timer_remove( timer, TRUE );
      }
    }

    /* Determine the next timer group in the chain ... stopping when we
       return to the start of the list with the faux timer group */

    group = group->next;
  }
  while ( group != &timers.active );


  /* Active timer list may have changed - ensure next timer event is correct */
  timer_update_timer_interrupt(TIMER_MVS_TIMER_CLEAR_TASK_TIMERS );


  ATS_INTFREE();

  return;

} /* timer_clr_osal_thread_timers */



/*=============================================================================

FUNCTION TIMER_CLR_TASK_TIMERS

DESCRIPTION
  Stops active timers belonging to the given task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_clr_task_timers
(
  /* Timers belonging to this task will be cleared (stopped). */
  rex_tcb_type                    *tcb
)
{
  /* Pointer to a timer group.  Used for walking list of timer groups */
  timer_group_ptr                 group;

  /* Pointer to a timer.  Used for walking list of timers */
  timer_ptr_internal_type         timer;

  /* Pointer to the next timer.  Used for walking list of timers */
  timer_ptr_internal_type         next;

  /* Timetick value for logging */
  time_timetick_type              now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_CLR_TASK_TIMERS,NULL,now,NULL);

  /* Walk list of timer groups, starting with the faux active group */

  group = &timers.active;

  do
  {
    /* Walk the list of timers in this group, and
       remove timers which belong to given task */

    for( timer = group->list.first;  timer != NULL;  timer = next)
    {
      /* Save next timer in list, in case this timer's next field in NULL'd */
      next = timer->next;

      if ( timer->tcb == tcb)
      {
        if ( timer->timer_ext != NULL )
		 {
          timer->timer_ext->last_expiry = 0;
		  timer->timer_ext->timer_state = TIMER_CLEARED_FLAG;
		 }
        else
        {
          ATS_INTFREE();
          ERR_FATAL (" Timer must have an ext timer, timer_ptr=0x%x timer_ext=0x%x",
                    timer,timer->timer_ext,0);
		  return;
        }

        /* Remove the timer from the list */
        timer_remove( timer, TRUE );
      }
    }

    /* Determine the next timer group in the chain ... stopping when we
       return to the start of the list with the faux timer group */

    group = group->next;
  }
  while ( group != &timers.active );


  /* Active timer list may have changed - ensure next timer event is correct */
  timer_update_timer_interrupt(TIMER_MVS_TIMER_CLEAR_TASK_TIMERS );


  ATS_INTFREE();

  return;

} /* timer_clr_task_timers */



/*=============================================================================

FUNCTION TIMER_GET_SCLK_TILL_EXPIRY

DESCRIPTION
  Returns the number of sclks, in 32kHz domain, until the expiry of
  next timer to expire.

DEPENDENCIES
  This function is provided for the exclusive use of SLEEP, for TCXO shutdown.
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of sclk's, in 32kHz domain, until the next timer expiry.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_get_sclk_till_expiry( void )
{
  return timer_conv_to_sclk_freq(timer_get_sclk_till_expiry_64());
} /* timer_get_sclk_till_expiry */


/*=============================================================================

FUNCTION TIMER_GET_SCLK_TILL_EXPIRY_64

DESCRIPTION
  Returns the number of ticks, in 19MHz domain, until the expiry of
  next timer to expire.

DEPENDENCIES
  This function is provided for the exclusive use of SLEEP, for TCXO shutdown.
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of ticks, in 19MHz domain, until the next timer expiry.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_get_sclk_till_expiry_64( void )
{
  /* Current slow clock count */
  time_timetick_type                   ticks_now = 0;
  time_timetick_type                   tt_ret;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  /* Get the current time */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_GET_SCLK_TILL_EXPIRY,NULL,ticks_now,NULL);

  if ( timers.set_value - timers.set_time > ticks_now - timers.set_time )
  {  
    /* Compute & return the time until the timer is set to expire */
    tt_ret = timers.set_value - ticks_now;
  }
  else
  {  
    /* The next timer interrupt is already pending! */
    tt_ret = 0;
  }

  ATS_INTFREE();

  return tt_ret;
} /* timer_get_sclk_till_expiry_64 */



/*=============================================================================

FUNCTION TIMER_SET_WAKEUP

DESCRIPTION
  This function sets the timer interrupt to expire in advance of the
  the next expiring timer's expiry point, to allow for TCXO to turn back on.
  This function works in the 32kHz domain.

DEPENDENCIES
  This function is provided for the exclusive use of SLEEP, for TCXO shutdown.
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  TE_SUCCESS  If the operation completed successfully

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_set_wakeup
(
  timetick_type                        wakeup_sclks
)
{
  return timer_set_wakeup_64(timer_conv_to_xo_freq(wakeup_sclks));
} /* timer_set_wakeup */


/*=============================================================================

FUNCTION TIMER_SET_WAKEUP_64

DESCRIPTION
  This function sets the timer interrupt to expire in advance of the
  the next expiring timer's expiry point, to allow for TCXO to turn back on.
  This function works in the 19MHz domain.

DEPENDENCIES
  This function is provided for the exclusive use of SLEEP, for TCXO shutdown.
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  TE_SUCCESS always currently

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_set_wakeup_64
(
  /* Wakeup the CPU in the given number of sclk's. */
  time_timetick_type                   wakeup_sclks
)
{
  /* Current slow clock count */
  time_timetick_type                   ticks_now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current time2 */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_SET_WAKEUP,NULL,ticks_now,NULL); 
  /* Program the timer */
  timer_set_next_interrupt( ticks_now + wakeup_sclks, ticks_now, FALSE, 
                            TIMER_MVS_SET_WAKEUP, NULL );

  return TE_SUCCESS;

}

/* timer_set_wakeup_64 */


/*=============================================================================

FUNCTION TIMER_ACTIVE_LIST_CHECK

DESCRIPTION
  Throws an error fatat if timer list is corrupted

DEPENDENCIES
  FEATURE_TIMER_TRACE and FEATURE_TIMER_LIST_DEBUG should be turned on.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_active_list_check(void)
{
  
  #if defined( FEATURE_TIMER_LIST_DEBUG )
  
  /* Pointer to traverse timer list */
  timer_ptr_internal_type          ptimer1, ptimer2;

  /* Number of timers in the list starting from first timer */
  uint32 forward_traversal_count = 0;
  
  /* Number of timers in the list starting from last timer */
  uint32 reverse_traversal_count = 0;

  /* First & last timer respectively in the active list */
  timer_ptr_internal_type          first, last;

  /* timetick for logging this functions entry & exit */
  static time_timetick_type        timer_list_check_entry = 0;
  static time_timetick_type        timer_list_check_last_exit = 0;  

  boolean timer_top_of_list = TRUE;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DalTimetick_GetTimetick64(hTimerHandle, &timer_list_check_entry);

  /* Return if active list is NULL */
  if ( timers.active.list.first == NULL )
    return;
  
  /* Spin thru the active timer list */
  ptimer1 = timers.active.list.first->next;
  
  if( ptimer1 != NULL )
    ptimer2 = ptimer1->next;
  else
    ptimer2 = NULL;

  /* Checks for Cirular loop in timers' list 
     Start with 2 pointers ( ptimer1  and ptimer2 ). 
     Advance ptimers1 with one node and ptimer2 with two nodes
     At any point if ptimer1 == ptimer2 then you have a loop, 
     otherwise ptimer2 will reach NULL in n/2 iterations. */
  
  while( ptimer1 != NULL && ptimer2 != NULL )
  {
    
    if (ptimer1 == ptimer2 )
    {
      ERR_FATAL(" Timer List Corrupted, ptimer1=0x%x ", ptimer1,0,0 );
    }

    ptimer1 = ptimer1->next;
    ptimer2 = ptimer2->next;

    if ( ptimer2 !=NULL )
    {
      ptimer2 = ptimer2->next; 
    }
    else
    {
      break;
    }
  }
  
  first      = timers.active.list.first;
  last       = timers.active.list.last;

  while( first != NULL )
  {
     /* To catch the timers that have not been expired though the expiry time
        has been crossed. A timer is expired if expiry - start of that timer is
        less than now - start. But if such a timer still exists in timers list,
        it means it been fired and thus need to error fatal */
    if( timers.processing == FALSE &&
		(first->expiry < timer_list_check_entry) )
    {
       if(timer_top_of_list == FALSE)
       {
           ERR_FATAL(" timer active list corrupted due to timers not being expired, timer=0x%x ",first,0,0); 
       }
    }
    else
    {
       /* Starting timers that have crossed expiry are over */
       timer_top_of_list = FALSE;
    }

  /* Check if the links of the doubly linked list are proper starting from the
     first timer and also count the number of timer */
    if( first->next != NULL )
    {
      if( first != first->next->prev )
      {
         ERR_FATAL(" timer active list corrupted, ptr1=0x%x ptr1->next->prev=0x%x",first,first->next->prev,0); 
      }

      /* Checking for expiries of the timers to be in sorted order for some corruption issues */
      if(first->expiry > first->next->expiry)
      {
         ERR_FATAL("Timer active list corrupted due to small timer being inserted after large timer, timer=0x%x",first,0,0); 
      }
    }
    first = first->next;
    forward_traversal_count++;
  }

  /* Count the number of timers in the list starting from last timer */
  while( last != NULL )
  {
    last = last->prev;
    reverse_traversal_count++;
  }

  /* List is corrupted if first_num is not same as last_num or is 
     not same as actual number of timers supposed to be present 
     at given point of time */

  if (forward_traversal_count != reverse_traversal_count)
    ERR_FATAL(" timer active list corrupted, fwd_traversal=0x%x reverse_traversal=0x%x",
              forward_traversal_count,reverse_traversal_count,0 );

  timer_list_check_last_exit = timer_list_check_entry ;

  #endif /* FEATURE_TIMER_LIST_DEBUG */

} /*  timer_active_list_check */

#ifdef FEATURE_TIME_QDSP6
/*=============================================================================

FUNCTION TIMER_PIMUTEX_INIT

DESCRIPTION
  This function initializes the pimutex that will be used for protecting
  the crit-section portion of timer code.

DEPENDENCIES
  This function is applicable only for QURT.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timer_pimutex_init(void)
  {
  qurt_pimutex_init ( &timer_qemu_mutex );
  }

void timer_pc_mutex_init(void)
  {
  qurt_pimutex_init ( &timer_pc_mask_mutex );
}
#endif

/*=============================================================================

FUNCTION TIMER_GET_QURT_SIG_REGISTRATION_ID

DESCRIPTION
  This function returns the registration id that will be used to signal the 
  expiry of the first timer.

DEPENDENCIES
  This function is applicable only for QURT.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
int timer_get_qurt_sig_registration_id
(
  void
)
{
  time_timetick_type          now = 0;

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_GET_QURT_SIG_REGISTRATION_ID,NULL,now,NULL);
#ifdef FEATURE_TIME_QDSP6
  return timer_qurt_sig_registration_id;
#else
  return TIMER_MAX_EXPIRY; 
#endif
}


/*=============================================================================

FUNCTION TIMER_INIT_QURT_SIGNAL

DESCRIPTION
  This function initializes QURT signal related to timer expiry. When
  the match value == timetick value, QURT will set this signal. 

  This function also initializes the mutex that will be used in crit-section
  portions of timer code.

DEPENDENCIES
  This function is applicable only for QURT.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
#ifdef FEATURE_TIME_QDSP6
static void timer_init_qurt_signal
(
  void
)
{
  #ifdef FEATURE_TIMER_USE_QURT_SYSCLOCK
  time_timetick_type            now = 0;
  #endif

  /*initialize the signal mask that we want to wait on*/
  qurt_anysignal_init (&timer_q6_signal);


  #ifdef FEATURE_TIMER_USE_QURT_SYSCLOCK
  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_INIT_QURT_SIGNAL,NULL,now,NULL);

  timer_qurt_sig_registration_id = 
        qurt_sysclock_register (&timer_q6_signal, TIMER_QURT_SIG_MASK);

  if (timer_qurt_sig_registration_id < 0) 
  {
    ERR_FATAL("sclk registration failed, id=0x%i",timer_qurt_sig_registration_id,0,0);
  }

  /*pass on the qurt registration id to the timetick layer*/
  DalTimetick_UpdateBlastSigId(hTimerHandle,(uint32)timer_qurt_sig_registration_id);
  #endif
}
#endif

#ifdef FEATURE_TIMER_USE_QURT_SYSCLOCK
/*=============================================================================

FUNCTION TIMER_UPDATE_ISR_TIME

DESCRIPTION
  This function is called in QURT, when the timer signal is set. In Q6, 
  there is no timer interrupt, but for backward compatibility, the variables
  timer.isr_time, etc. need to be populated.

DEPENDENCIES
  This function is applicable only for QURT.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timer_update_isr_time
(
  void
)
{
  /* Timer sclk time-stamp values */
  time_timetick_type                   set_time, set_value;

  /* Latency in ISR handling */
  time_timetick_type                   latency_sclk;
  time_timetick_type                   now = 0;

  ATS_INTLOCK( );
  /* Record when timer isr actually was handled */
  DalTimetick_GetTimetick64(hTimerHandle, &now);
  timers.isr_time = now;

  TIMER_API_LOG(TIMER_API_LOG_TIMER_UPDATE_ISR_TIMER,NULL,now,NULL);  

  /* Capture timer set values, for messaging outside of ATS_INTLOCK. */
  set_time  = timers.set_time;
  set_value = timers.set_value;

  ATS_INTFREE();


  if ( now - set_time >= set_value - set_time)
  {
    /* Determine the difference between when the interrupt was programmed to
       occur at, and when the interrupt was actually handled. */
    latency_sclk = now - set_value;

    if ( latency_sclk > TIMER_SCLK_LATENCY )
    {
      /* Timer interrupt was handled over btw 5 (60kHz) to 10ms (30kHz) late.
         (Timers may expire much later than 5-10ms, if they are in a disabled
         group, or if multiple timers expire due to the same interrupt.
         This is only a measure of the timer isr latency.) */

      ATS_MSG_ERROR("Late Timer ISR: ST=%d SV=%d IT-SV=%d",
                set_time, set_value, latency_sclk);
    }
  }
  else
  {
#ifndef FEATURE_TIME_QDSP6
    ATS_MSG_HIGH("Early Timer ISR: ST=%d SV=%d SV-IT=%d",
             set_time, set_value, set_value - now);
#endif
  }

  /* Save a trace packet */
  TIMER_TRACE(ISR);
}
#endif /* FEATURE_TIMER_USE_QURT_SYSCLOCK */

/*=============================================================================

FUNCTION TIMER_IS_ACTIVE

DESCRIPTION
 This is function which queries whether a timer is active or not.        
 A timer which has been set to expire at a certain time - even if the   
 group it is a member of is disabled - is considered to be active.      
 A timer which is explicitly paused is not active, until it has been    
 resumed.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if the timer is active.FALSE if the timer is not active.

SIDE EFFECTS
  None

=============================================================================*/
boolean timer_is_active
(
  timer_ptr_type  timer
)
{
  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  boolean                         timer_active=FALSE;
  time_timetick_type              now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(timer == NULL)
	 {
	  ASSERT(0);
	  return FALSE;
	 }	


  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG(TIMER_API_LOG_TIMER_IS_ACTIVE,timer,now,timer_get_internal_timer(timer));
   
  timer_int = timer_get_internal_timer ( timer );

  if(timer_int
     && timer_int->list != NULL)
  {
    timer_active = TRUE;
  }
  ATS_INTFREE();
  
  return timer_active;

} /* timer_is_active */

/*=============================================================================

FUNCTION TIMER_EXPIRES_AT

DESCRIPTION
 This returns the sclk counter value when the timer    
 will expire at.See: timetick_get()  

DEPENDENCIES
  Timer must be running for the returned value to be meaningful.

RETURN VALUE
  timetick_type - sclk counter value corresponding to timer expiry point.

SIDE EFFECTS
  None

=============================================================================*/
timetick_type timer_expires_at
(
  timer_ptr_type  timer
)
{
  return timer_conv_to_sclk_freq(timer_expires_at_64(timer));;
} /* timer_expires_at */


/*=============================================================================

FUNCTION TIMER_EXPIRES_AT_64

DESCRIPTION
 This returns the sclk counter value when the timer    
 will expire at. See: DalTimetick_GetTimetick64()  

DEPENDENCIES
  Timer must be running for the returned value to be meaningful.

RETURN VALUE
  time_timetick_type - XO counter value corresponding to timer expiry point.

SIDE EFFECTS
  None

=============================================================================*/
time_timetick_type timer_expires_at_64
(
  timer_ptr_type  timer
)
{
   /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  time_timetick_type                   expires_in=0;
  time_timetick_type              now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(timer == NULL)
	 {
	  ASSERT(0);
	  return 0;
	 }	


  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  timer_int = timer_get_internal_timer ( timer );

  TIMER_API_LOG(TIMER_API_LOG_TIMER_EXPIRES_AT,timer,now,timer_int);

  if(timer_int) 
  {
    expires_in = timer_int->expiry;
  }
  ATS_INTFREE();
  
  return expires_in ;
  
} /* timer_expires_at_64 */


/*=============================================================================

FUNCTION TIMER_GET_START

DESCRIPTION
 This returns the sclk counter value in the 32kHz domain when the timer    
 was set  

DEPENDENCIES
  Timer must be running for the returned value to be meaningful.

RETURN VALUE
  timetick_type - sclk counter value corresponding to timer start point.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_get_start
(
  timer_ptr_type  timer
)
{
  return timer_conv_to_sclk_freq(timer_get_start_64(timer));
}


/*=============================================================================

FUNCTION TIMER_GET_START_64

DESCRIPTION
 This returns the sclk counter value in the 19MHz domain when the timer    
 was set  

DEPENDENCIES
  Timer must be running for the returned value to be meaningful.

RETURN VALUE
  time_timetick_type - TCXO counter value corresponding to timer start point.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_get_start_64
(
  timer_ptr_type  timer
)
{
   /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int;
  time_timetick_type                   timer_start=0;
  time_timetick_type              now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(timer == NULL)
	 {
	  ASSERT(0);
	  return 0;
	 }	


  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  timer_int = timer_get_internal_timer ( timer );

  TIMER_API_LOG(TIMER_API_LOG_TIMER_GET_START,timer,now,timer_int);

  if(timer_int) 
  {
    timer_start = timer_int->start;
  }

  ATS_INTFREE();
  
  return timer_start;

} /* timer_get_start_64 */

/*=============================================================================

FUNCTION TIMER_GET_ACTIVE_TIMER_NUM

DESCRIPTION
 This returns the number of timers in 
 active list. These does NOT include 
 paused or disabled timers

DEPENDENCIES
  None

RETURN VALUE
  uint32 number of active timers

SIDE EFFECTS
  None

=============================================================================*/

uint32 timer_get_active_timer_num
(
  void
)
{
  return timer_active_timers_count;
}

/*=============================================================================

FUNCTION TIMER_GET_PAUSED_TIMER_NUM

DESCRIPTION
 This returns the number of paused timers 

DEPENDENCIES
  None

RETURN VALUE
  uint32 number of paused timers

SIDE EFFECTS
  None

=============================================================================*/
uint32 timer_get_paused_timer_num
(
  void
)
{
  return timer_count_paused_timers;
}

/*=============================================================================

FUNCTION TIMER_GET_TIMER_FREE_Q_CNT

DESCRIPTION
 This returns the timers allocated 
 in timer_buffer

DEPENDENCIES
  None

RETURN VALUE
  uint32 number of allocated timers in timer_buffer

SIDE EFFECTS
  None

=============================================================================*/
uint32 timer_get_timer_free_q_cnt
(
  void
)
{
  return timer_count_buffer_allocated;
}

/*=============================================================================

FUNCTION TIMER_GET_TIMETICK_HANDLE

DESCRIPTION
 This function is called from timer_init to pass timetick handle to timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void timer_get_timetick_handle(void)
{

#ifdef FEATURE_TIMER_SLAVE1
  /* We want slave task to return the handle since slave needs the ptr too */
  timer_slaves_get_timetick_handle((void**)&hTimerHandle);
#else /* FEATURE_TIMER_SLAVE1 */
  DalTimetick_Attach("SystemTimer", &hTimerHandle);
  ASSERT(hTimerHandle != NULL);
#endif /* FEATURE_TIMER_SLAVE1 */

  /* Enable system timer */
  DalTimetick_Enable(hTimerHandle,1);
  DalTimetick_InitTimetick64(hTimerHandle);

  /* Rex TCB pointer needed for task synchronization */
  timer_tcb_ptr = rcinit_lookup_rextask("timer");
}


/*=============================================================================

FUNCTION TIMER_POST_INIT

DESCRIPTION
 This function is called from time_init to pass timetick handle to timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timer_post_init(void)
{
  /*at this point, the dog task has started and hence we can register the slave tasks with
   the dog*/
  #ifdef FEATURE_DOG
  #ifdef FEATURE_TIMER_SLAVE1
  timer_slave_register_dog(TIMER_SLAVE_TASK1);
  #endif

  #ifdef FEATURE_TIMER_SLAVE2
  timer_slave_register_dog(TIMER_SLAVE_TASK2);
  #endif

  #ifdef FEATURE_TIMER_SLAVE3
  timer_slave_register_dog(TIMER_SLAVE_TASK3);
  #endif
  #endif /* FEATURE_DOG */

  timer_register_npa_client();
}

/*=============================================================================

FUNCTION TIMER_SLEEP_CB

DESCRIPTION
 Signals the task waiting on the task_sleep_timer by timer_sleep function.
 Data will be signal object for Qurt Kernel while task pointer on Rex kernel

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timer_sleep_cb
(
timer_cb_data_type      data
)
{

  #ifdef FEATURE_TIME_QDSP6
  
   /* Signal the task in order to resume the sleeping task */
   (void) qurt_anysignal_set((qurt_anysignal_t*)data, TIMER_SLEEP_CB_SIG_MASK);
  
  #else 
  
  /* Use rex_resume_task in case of non-q6 kernel */
   rex_resume_task((rex_tcb_type*)data);
  
  #endif /*FEATURE_TIME_QDSP6*/     
  
   return;
}

/*=============================================================================

FUNCTION TIMER_SLEEP

DESCRIPTION
 Suspends the client task to get suspended for the specified time.
 If non_deferrable option is True, task will surely resumed at specified time
 If non_deferrable option is False, and if Proc goes into sleep at the time of
 specified timeout, task will resume only after Proc wakesup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timer_sleep
(
  time_timetick_type              timeout,
  timer_unit_type             unit,
  boolean                         non_deferrable
)
{
  /* rex tcb pointer to be used in case of rex kernel & logging */
  rex_tcb_type            *client_tcb;

  #ifdef FEATURE_TIME_QDSP6
  /* Signal object for timer callback notification */
  qurt_anysignal_t        timer_sleep_cb_sig_obj;
  #endif /*FEATURE_TIME_QDSP6*/  
  
  /* Timer for the client to handle the sleeping */
  timer_type		      task_sleep_timer;
  
  /* Data to send to the timer callback */
  timer_cb_data_type      cb_data;
   
  /* memset the external timer memory to zero */
  memset(&task_sleep_timer, 0, sizeof(timer_type));

  /* Get the tcb pointer to be used for the rex_resume/suspend calls & logging */
  client_tcb=rex_self();
  
  
  #ifdef FEATURE_TIME_QDSP6  
  
  /* Initialize the qurt signal object */
  qurt_anysignal_init (&timer_sleep_cb_sig_obj);
  cb_data=(timer_cb_data_type)&timer_sleep_cb_sig_obj;
  
  #else /*FEATURE_TIME_QDSP6*/
  
  /* Copy rex tcb pointer in the variable to be passed as timer callback data */
  cb_data=(timer_cb_data_type)client_tcb;
  
  #endif /*FEATURE_TIME_QDSP6*/ 
  
  if (non_deferrable != TRUE)
    {
	  /* Create a deferrable timer to wake up the calling task */
	  timer_def( &task_sleep_timer, 
	             NULL, /* Null Timer Group */
				 NULL, 
				 (rex_sigs_type)0, 
				 timer_sleep_cb, 
				 cb_data 
			   );
	}
  else
    {
	  /* Create a non-deferrable timer for the clinet */
	  timer_def( &task_sleep_timer, 
	             &timer_non_defer_group, 
				 NULL, 
				 (rex_sigs_type)0, 
				 timer_sleep_cb, 
				 cb_data 
			   );
	}
	
  /* Set the timer to expire after the specified duration */
    (void)timer_set_64(&task_sleep_timer,
                 timeout,
                 0,
                 unit);				 

  
  ATS_MSG_HIGH("TIMER_SLEEP: Entering timer_sleep, tcb = 0x%x; timeout=%d; unit=%d",
            client_tcb,
			(uint32)timeout,
			(uint32)unit);
				 
  #ifdef FEATURE_TIME_QDSP6 
  
  /* Wait on the Qurt signal object */
  qurt_anysignal_wait (&timer_sleep_cb_sig_obj, TIMER_SLEEP_CB_SIG_MASK);

  /* Clear the signal */
  qurt_anysignal_clear (&timer_sleep_cb_sig_obj, TIMER_SLEEP_CB_SIG_MASK);

  #else /*FEATURE_TIME_QDSP6*/
   /* For rex kernels */
   	/* Keep in a loop & susend the task until the timer is actually expired */
	do
	{
	   rex_suspend_task(client_tcb);
	}
	while((uint32)timer_get(&timer_s,T_MSEC))!=0);
	
  #endif /*FEATURE_TIME_QDSP6*/

  ATS_MSG_HIGH("TIMER_SLEEP: Exiting from timer_sleep, tcb = 0x%x; timeout=%d; unit=%d",
            client_tcb,
			(uint32)timeout,
			(uint32)unit);
 
  return;
} /* timer_sleep */


/*=============================================================================

FUNCTION TIMER_REGISTER_FOR_NON_DEFER_EXPIRY_CHANGE

DESCRIPTION
Registers for first non-deferrable timer expiry change notification.
Only signal type notifications are allowed, for others, error will be 
returned.

DEPENDENCIES
  Input objects should be initialized by client

RETURN VALUE
TE_INVALID_PARAMETERS   If passed sig obj addr is NULL or invalid notify type
TE_SUCCESS :            If the operation completed successfully

SIDE EFFECTS
  None

=============================================================================*/
timer_error_type timer_register_for_non_defer_expiry_change
(
  timer_notify_type        notify_type,
  time_osal_notify_obj_ptr notify_obj,
  time_osal_notify_data    notify_mask
)
{
   int idx;
   
   if(!timer_is_notify_type_valid(notify_type))
      return TE_INVALID_PARAMETERS;
   
   /* This registration is allowed only for signal type notifications */
   if(notify_type == TIMER_FUNC1_CB_TYPE || 
      notify_type == TIMER_FUNC2_CB_TYPE ||
      notify_type == TIMER_NO_NOTIFY_TYPE)   
      return TE_INVALID_PARAMETERS;

   if(notify_obj == NULL)   
      return TE_INVALID_PARAMETERS;
      
   ATS_INTLOCK();
   
   if(timer_non_defer_expiry_notify_clients_cnt == TIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT)
   {
      ATS_INTFREE();
      return TE_FAIL;
   }

   idx = timer_non_defer_expiry_notify_clients_cnt++;
   timer_non_defer_expiry_notify_clients[idx].notify_type = notify_type;
   timer_non_defer_expiry_notify_clients[idx].notify_obj = notify_obj;
   timer_non_defer_expiry_notify_clients[idx].notify_mask = notify_mask;
   
   ATS_INTFREE();
   
   return TE_SUCCESS;  
} /* timer_register_for_non_defer_expiry_change */