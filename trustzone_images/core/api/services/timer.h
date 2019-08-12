#ifndef REX_H
/* Circular #include: "rex.h" includes "timer.h", which includes "rex.h".
   Ensure first half of "rex.h" is included before "timer.h" processed. */
#include "rex.h"
#endif /* REX_H */

#ifndef TIMER_H
#define TIMER_H
/*=============================================================================

                  T I M E R   S E R V I C E   S U B S Y S T E M

                     P U B L I C   A P I S   A N D   D A T A

GENERAL DESCRIPTION
  Implements time-keeping functions using the slow clock.

  Slow Clock Frequency          Granularity     Timer Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


REGIONAL FUNCTIONS

  timer_init()
    Initializes the timer subsytem.  Call only from time_init( ).

  timer_set_modem_app_sclk_offset(delta)
    Sets the modem/app sclk counter difference.  Call only from timetick.c


EXTERNALIZED FUNCTIONS

  timer_def(timer, group, tcb, sigs, func, data)
    Defines and initializes a timer.
    Timers may also be staticly declared via the TIMER_DEF() macro

  timer_def2(timer, group, func, data)
    Defines and initializes a timer, with the clock callback semantics.
    Timers may also be staticly declared via the TIMER_DEF2() macro

  timer_set(timer, ticks, reload, unit)
    Sets an inactive timer to expire after a given period of time, or changes
    an active timer to expire after a given period of time.
    Optionally, specifies the timer to repeatly expire with a given period.

  timer_get(timer, unit)
    Get number of ticks before timer expires

  timer_pause(timer)
    Suspends an active timer

  timer_resume(timer)
    Resumes a previously "paused" active timer.

  timer_clr(timer, unit)
    Stops an active timer

  timer_clr_task_timers(tcb)
    Stops all active timers which belong to the specified task.

  timer_group_enable(timer_group)
    Enables a timer group.  Timers in the timer group which expired during the
    diabled period will have their expiry processing executed.

  timer_group_disable(timer_group)
    Disables a timer group.  Timers in the timer group will continue to count
    down, but they will not expire.


INITIALIZATION AND SEQUENCING REQUIREMENTS

    timer_init() must be called once before any other timer functions.

    timer_def() must be called, or the TIMER_DEF( ) macro used, for each
      timer, before other timer functions are used with that timer.

    A valid sclk estimate is needed before timers can be set.


Copyright (c) 2003 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/timer.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/10/09   din     Added sclk_delta to timer_type. 
11/25/09   din     Added timer_[un]defer_match_interrupt()
05/22/09   din     Removed FEATURE_TIMER_TASK
01/07/09   pbi     Removed timer_set_sclk_offset
02/23/09   din     Consoldation of time.
01/13/09   din     Removed timer_delay_expiry_excluding_groups() and
                   timer_undelay_expiry().
01/12/09   din	   Added deferrable attribute to timer_group. Also added 
                   lightweight API's to enable/ disable timer groups. API's are
                   timer_defer_match_interrupt(), timer_undefer_match_interrupt()
                   & timer_group_set_deferrable().
12/23/08   tev     Restore timer_null_group to public timer.h.  Eliminate
                   need for timer_v.h.
12/15/08   tev     Removed timer_null_group to private timer_v.h header file.
11/14/08   tev     Amended control for timer_restart to
                   defined(FEATURE_TIME_POWER_COLLAPSE) &&
                   !defined(FEATURE_TIME_REMOVE_TIMER_RESTART)
11/10/08   tev     Tentatively restored timer_restart under control of
                   FEATURE_TIME_POWER_COLLAPSE symbol definition.
10/16/08    din    Added lightweight mechanism to enable and disable timer groups 
                   during sleep. For use by the sleep subsystem only! APIs are
                   timer_delay_expiry_excluding_groups(), and timer_undelay_expiry()
                   (renamed timer_restart().)  
11/13/07   jhs     Removed timer_always_on_group.
08/31/07   jhs     Support for AMSS RESTART
12/04/06   trc     extern "C" {} our API to avoid name mangling for C++ clients
08/19/06   gb      Changed FEATURE_POWER_COLLAPSE to FEATURE_TIME_POWER_COLLAPSE
02/22/06   ajn     SCLK offset now applies for both modem & apps processors.
02/10/06   kap/ajn Added Timer Optimization Changes (double linking/binning)
11/01/05   ajn     Added cache initialization to TIMER_DEF/TIMER_DEF2 macros.
10/10/05   ajn     Added TIMER_EXPIRES_AT() macro
07/06/05   ajn     Added TIMER_IS_ACTIVE() macro
07/06/05   ajn     Added timer_set_modem_app_sclk_offset (7500 APPS)
05/06/05   ajn     Cache time/reload values to avoid recalculating if unchanged
03/29/05   ajn     Replaced timer_ptr with timer_ptr_type, for L4 project.
10/20/04   ajn     Added a timer task, for timer expiry
09/28/04   ajn     Added "timer_null_group" for timers declared in NULL group
08/30/04   ajn     Added "get next interrupt" and "interrupt early" functions
                   for TCXO shutdown.
08/11/03   ajn     Added timer_pause/timer_resume for rex_set_timer replacement
08/08/03   ajn     Updated for coding standards
08/06/03   ajn     Added timer_def2(), for clk_reg() replacement
07/23/03   ajn     Moved _{to,from}_sclk functions to timetick.h
07/17/03   ajn     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "timetick.h"

/**
@mainpage Time Services CDMA
Implements timer functions*/


/**
@defgroup Time Services CDMA  - Public Interface
This section contains definitions for various public interface defined 
by this module.
*/

/** 
@ingroup Timer Functions
@file timer.h
This file contains public interface defined by this module
*/


#ifdef __cplusplus
/** Avoid name mangling for any C++ customers */
extern "C" {
#endif

/*==============================================================================

                           DATA DECLARATIONS

=============================================================================*/

/** Timer 'type 1' callback arbitrary data type*/
typedef unsigned long             timer_cb_data_type;

/** Timer 'type 1' callback function*/
typedef void (*timer_t1_cb_type)
(
  /** Arbitrary data given to timer_def( ) to pass to this callback function */
  timer_cb_data_type              data
);


/** Timer 'type 2' callback function (for clock callback style interface)*/
typedef void (*timer_t2_cb_type)
(
  /** Time elapsed from timer setting (start) */
  int32                           time_ms,

  /** Arbitrary data given to timer_def2( ) to pass to this callback function */
  timer_cb_data_type              data
);



/** Timer structure pointer types*/
typedef struct timer_group_struct *timer_group_ptr;
/** Pointer to timer list structure */
typedef struct timer_list_struct  *timer_list_ptr;
/** Pointer to timer structure */
typedef struct timer_struct       *timer_ptr_type;
/** Pointer to timer bin structure */
typedef struct timer_bin_struct   *timer_bin_ptr;

#ifndef FEATURE_L4
/** The L4 project has issues where timer_ptr is used other than as a typedef.
   For other projects, temporarily define timer_ptr a migration aid. */
//#define timer_ptr                 timer_ptr_type
#endif /** !FEATURE_L4 */


/** Timer list structure type
    Values in this structure are for private use by "timer.c" only.*/
typedef struct timer_list_struct
{
  /** Slow clock "zero" base count for timer list */
  timetick_type                   zero;

  /** List of timers */
  timer_ptr_type                  first;

  /** End of that list */
  timer_ptr_type                  last;
}
timer_list_type;


/** Timer group structure type
    Values in this structure are for private use by "timer.c" only.*/
typedef struct timer_group_struct
{
  /** Timers in disabled timer group */
  timer_list_type                 list;

  /** Disabled flag for the timer group */
  boolean                         disabled;

  /** Deferrable flag for the timer group, default value should be FALSE */
  boolean                         deferrable;
  
  /** Link to next timer group */
  timer_group_ptr                 next;
}
timer_group_type;


/** Timer cache structure type
    Values in this structure are for private use by "timer.c" only.*/

typedef struct
{
  /** Original set duration */
  timetick_type                   duration;

  /** Converted set duration */
  timetick_type                   duration_sclk;

  /** Original reload increment */
  timetick_type                   reload;

  /** Converted reload increment */
  timetick_type                   reload_sclk;

  /** Time units */
  timer_unit_type                 unit;
}
timer_cache_type;


/** Timer structure type
    Values in this structure are for private use by "timer.c" only.*/

typedef struct timer_struct
{
  /** Group to which timer is a member */
  timer_group_ptr                 group;

  /** Task to signal when timer expires, or task to use for APC */
  rex_tcb_type                    *tcb;

  /** Signal(s) to set in task, when timer expires */
  rex_sigs_type                   sigs;

  /** APC (Asynchronous Procedure Call) function to call when timer expires */
  timer_t1_cb_type                func1;

  /** Clock callback style function to call when timer expires */
  timer_t2_cb_type                func2;

  /** Data for APC function, when timer expires */
  timer_cb_data_type              data;

  /** The list the timer is on.  "list" will contain:
       "&active_timers" if the timer is active;
       "&timer->group->list" if the timer is active, but in a disabled group;
       "NULL" if the timer is not active. */
  timer_list_ptr                  list;

  /** Slow clock tick count timer expires at (list != NULL),
     or remaining ticks until expiry if timer is paused (list == NULL) */
  timetick_type                   expiry;

  /** Reload offset when timer expires (0 = no reload at expiry) */
  timetick_type                   reload;

  /** Slow clock tick count value when timer was set (started) */
  timetick_type                   start;

  /** Slow clock delta count value when timer was set (started) */
  timetick_type                   sclk_delta;

  /** Durations and reload times, original units and converted to sclks */
  timer_cache_type                cache;

  /** Pointer to the next timer in the list (list != NULL) */
  timer_ptr_type                  next;

  /** Pointer to the previous timer in the list (list != NULL) */
  timer_ptr_type                  prev;

  /** Pointer to the bin for this timer */
  timer_bin_ptr                   bin;
}
timer_type;

/** The NULL timer group, for timers not in any other group*/

/** Timers declared to be in the NULL group are reparented to this group */
extern timer_group_type           timer_null_group;


/** Static timer initialization - see timer_def(), timer_def2*/

/** TIMER_DEF - See timer_def*/
#define TIMER_DEF( group, task, sigs, func1, data ) \
{ group, task, sigs, func1, NULL, data, NULL, 0, 0, 0, 0, {0,0,0,0,T_NONE}, NULL, NULL, NULL }

/**TIMER_DEF2 - See timer_def2() */
#define TIMER_DEF2( group ) \
{ group, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, {0,0,0,0,T_NONE}, NULL, NULL, NULL }



/*==============================================================================

                        REGIONAL FUNCTION DECLARATIONS

=============================================================================*/


/** 
@ingroup Timer Functions
         
@brief  
 Initializes the Timer Subsytem                                        \n\n                                                                

@param[in]                                                               
 None                                                                   \n\n
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 May install an ISR for the 32-bit slow clock match register            \n\n

*/
void timer_init( void );


/** 
@ingroup Timer Functions 
         
@brief  
 Initializes the Timer Task                                             \n\n                                                                 

@param[in]                                                               
 dword - Unused parameter - required to match rex_task_func_type         \n
         prototype                                                       \n\n
 
@dependencies  
 None                                                                    \n\n  
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                                                   \n\n

*/
void timer_task
(
  dword                           ignored
);



/*==============================================================================

                            FUNCTION DECLARATIONS

=============================================================================*/


/** 
@ingroup Timer Functions 
         
@brief  
  Enables a timer group.  Timers in the timer group which expired during 
  the diabled period will have their expiry processing executed.         \n\n                                                                                                    \n\n

@param[in]                                                               
 timer_group_ptr - Timer group to be enabled                             \n                                                     
 
@dependencies  
 None                                                                    \n\n  
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 May generate signals to a task, which can cause task switches.         
 May queue an Asynchronous Procedure Call.                              \n\n

*/
void timer_group_enable
(
  timer_group_ptr                 timer_group
);


/** 
@ingroup Timer Functions 
         
@brief  
 Disables a timer group.  Timers in the timer group will continue to    
 count down, but they will not expire.                                  \n\n                                                                 

@param[in]                                                               
 timer_group_ptr - Timer group to be disabled                            \n\n
 
@dependencies  
 None                                                                    \n\n  
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                                                   \n\n

*/

void timer_group_disable
(
  timer_group_ptr                 timer_group
);


/** 
@ingroup Timer Functions 
         
@brief  
 Set timer group deferrable status.Timer groups default to being        
 non-deferrable.                                                        \n\n                 

@param[in] timer_group_ptr - Timer group to be undeferred                          
@param[in] boolean - Set parameter , TRUE, if deferrable FALSE otherwise  \n\n
 
@dependencies  
 None                                                                    \n\n  
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                                                   \n\n

*/

void timer_group_set_deferrable
( 
  timer_group_ptr pgroup, 
  boolean condition 
);


/** 
@ingroup Timer Functions 
         
@brief  
 Sets timer match interrupt to the next occurring non-deferrable timer  
 expiry value. Complimentary function is timer_undefer_match_interrupt()\n\n                                                                         

@param[in]                                                               
 None                                                                   \n\n 
 
@dependencies  
 Must be called from an INTLOCK'd context.                               \n\n  
 
@param[out]
 timetick_type - Number of sclk's until the next timer expiry in a timer\n 
 group which is non-deferrable.                                         \n\n
 
@sideffects 
 Adjusts match value and prevents any subsequent timer calls from        \n
 updating the match value,until timer_undefer_match_interrupt()is called.\n\n                                                                   

*/


timetick_type timer_defer_match_interrupt( void ) ;

/** 
@ingroup Timer Functions 
         
@brief  
 Set the timer ISR to fire at the earliest expiring timer in the active 
 list.  Meant to serve as a the complimentary function to               
 timer_defer_match_interrupt()                                          \n\n                 

@param[in]                                                               
 None                                                                    \n\n 
 
@dependencies  
 Must be called from an INTLOCK'd context.                               \n\n  
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 Adjusts the match value                                                 \n\n

*/
void timer_undefer_match_interrupt( void );

/** 
@ingroup Timer Functions 
         
@brief  
 Defines and initializes a timer.                                       
 When the timer expires:                                                
 if sigs is non-null, those signals are set to the task given by tcb; 
 if func is non-null, then if tcb is null, then func(data) is called  
 from interrupt context,otherwise, func(data) is queued as an         
 Asynchronous Procedure Call (APC)                                    
 Timers may also be staticly declared via the TIMER_DEF() macro          \n\n

@param[in] timer_ptr_type - Timer to set                                           
@param[in] timer_group_ptr -Group timer will belong to(NULL=the "NULL" timer group)
@param[in] rex_tcb_type - Task to signal and/or task to queue APC call for         
@param[in] rex_sigs_type - Task signals to set to the tcb task when timer expires  
@param[in] timer_t1_cb_type - APC (tcb!=NULL) or callback (tcb==NULL) to           
                    queue/call at timer expiry                           
@param[in] timer_cb_data_type - Arbitrary data for APC or callback                 \n\n

 
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                                                   \n\n

*/

void timer_def
(
  timer_ptr_type                  timer,
  timer_group_ptr                 group,
  rex_tcb_type                    *tcb,
  rex_sigs_type                   sigs,
  timer_t1_cb_type                func,
  timer_cb_data_type              data
);


/** 
@ingroup Timer Functions 
         
@brief  
 Defines a 'clock callback' style timer.                                 \n
 When the timer expires:                                                 
   func(time_ms, data) is called from interrupt context,                 
 Timers may also be staticly declared via the TIMER_DEF2() macro         \n\n

@param[in] timer_ptr_type - Timer to set                                           
@param[in] timer_group_ptr -Group timer will belong to(NULL=the "NULL" timer group)\n\n
  
@dependencies  
 Timer must be initialized with timer_reg                                \n\n 
 
@param[out]
  None                                                                   \n\n
 
@sideffects 
  None                                                                   \n\n

*/

void timer_def2
(
  timer_ptr_type                  timer,
  timer_group_ptr                 group
);



/** 
@ingroup Timer Functions 
         
@brief  
  Initializes a 'clock callback' style timer.                            \n\n

@param[in] timer_ptr_type - Timer to set                                           
@param[in] timer_t2_cb_type - Function to call at timer expiry                     
@param[in] timer_cb_data_type - Arbitrary data for func(time_ms, data)             
@param[in] timetick_type - Time (in ms) until first timer expiry                   
@param[in] timetick_type - Period (in ms) between repeated expiries(0=not periodic)\n\n
  
@dependencies  
 Timer must be initialized with timer_def2                               \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                                                   \n\n

*/

void timer_reg
(
  timer_ptr_type                  timer,
  timer_t2_cb_type                func,
  timer_cb_data_type              data,
  timetick_type                   time_ms,
  timetick_type                   reload_ms
);


/** 
@ingroup Timer Functions 
         
@brief  
 Sets an inactive timer to expire after a given period of time, or changes
 an active timer to expire after a given period of time.                  
 Optionally, specifies the timer to repeatly expire with a given period.\n\n

@param[in] timer_ptr_type - Timer to set                                          
@param[in] timetick_type - Time (in ms) until first timer expiry                   
@param[in] timetick_type - Period (in ms) between repeated expiries(0=not periodic)
@param[in] timer_unit_type - Unit to measure "ticks" in                            \n\n
  
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
  May generate signals to a task, which can cause task switches.         \n   
  May queue an Asynchronous Procedure Call.                              \n\n

*/

void timer_set
(
  timer_ptr_type                  timer,
  timetick_type                   time,
  timetick_type                   reload,
  timer_unit_type                 unit
);

/** 
@ingroup Timer Functions 
         
@brief  
 Get number of ticks before timer expires                               \n\n

                                                               
@param[in] timer_ptr_type - Timer to get                                    
@param[in] timer_unit_type - Unit to measure "ticks before timer expires" in.      \n\n
  
@dependencies  
 None                                                                    \n\n 
 
@param[out]
  timetick_type - Number of ticks before timer expires.                  \n
  Zero is returned for inactive timers, and timers that have expired or  
  are expiring. Fractional value which might truncate to zero            
  depending on unit passed as parameter is rounded to 1.                 \n\n                                                  
 
@sideffects 
  None                                                                   \n\n

*/

timetick_type timer_get
(
  timer_ptr_type                  timer,
  timer_unit_type                 unit
);


/** 
@ingroup Timer Functions 
         
@brief  
 This is a macro which queries whether a timer is active or not.        
 A timer which has been set to expire at a certain time - even if the   
 group it is a member of is disabled - is considered to be active.      
 A timer which is explicitly paused is not active, until it has been    
 resumed.                                                               \n\n

@param[in]                                                               
 timer_ptr_type - timer to be queried                                    \n\n
  
@dependencies  
 None                                                                    \n\n 
 
@param[out]
 boolean - TRUE if the timer is active.FALSE if the timer is not active.\n\n
 
@sideffects 
 None                                                                   \n\n

*/

#define TIMER_IS_ACTIVE(timer)  ( (timer)->list != NULL )


/** 
@ingroup Timer Functions 
         
@brief  
 This is a macro which returns the sclk counter value when the timer    
 will expire at.See: timetick_get()                                     \n\n

@param[in]                                                               
 timer_ptr_type - timer to be queried                                    \n\n
  
@dependencies  
 Timer must be running for the returned value to be meaningful.          \n\n 
 
@param[out]
 timetick_type - sclk counter value corresponding to timer expiry point.\n\n
 
@sideffects 
  None                                                                   \n\n

*/
#define TIMER_EXPIRES_AT(timer)  ( (timer)->expiry )


/** 
@ingroup Timer Functions 
         
@brief  
 Stops an active timer                                                  \n\n

@param[in] timer_ptr_type - timer to be stopped                                   
@param[in] timer_unit_type - Unit to measure "ticks before expiry" in.             
                   Use T_NONE if not required                            \n\n
  
@dependencies  
 None                                                                   \n\n
 
@param[out]
 timetick_type - Number of ticks remaining before expiry.               \n\n
 
@sideffects 
 None                                                                   \n\n

*/

timetick_type timer_clr
(
  timer_ptr_type                  timer,
  timer_unit_type                 unit
);


/** 
@ingroup Timer Functions 
         
@brief  
 Pauses an active timer                                                 \n\n

@param[in]                                                               
 timer_ptr_type - timer to be paused                                     \n\n
   
@dependencies  
 Should only be called for a timer that is running.                      \n\n 
 
@param[out]
  None                                                                   \n\n 
 
@sideffects 
 None                                                                   \n\n

*/

void timer_pause
(
  timer_ptr_type                  timer
);



/** 
@ingroup Timer Functions 
         
@brief  
 Resumes a previously paused timer.                                     
 If a timer was paused 5 seconds before its expiry, and 30 seconds later 
 timer_resume( ) is called, the timer will expire 5 seconds after that   
 point.                                                                  \n\n

@param[in]                                                               
 timer_ptr_type - timer to be resumed                                     \n\n
   
@dependencies  
 Must only be called for a timer that has been paused.                    \n\n 
 
@param[out]
 None                                                                   \n\n 
 
@sideffects 
 May generate signals to a task, which can cause task switches.         \n
 May queue an Asynchronous Procedure Call.                              \n\n

*/
void timer_resume
(
  timer_ptr_type                  timer
);



/** 
@ingroup Timer Functions 
         
@brief  
 Stops active timers belonging to the given task                       \n\n

@param[in]                                                               
 rex_tcb_type - Timers belonging to this task will be cleared (stopped).  \n\n
   
@dependencies  
 None                                                                     \n\n  
 
@param[out]
 None                                                                   \n\n 
 
@sideffects 
 None                                                                   \n\n

*/

void timer_clr_task_timers
(
  rex_tcb_type                    *tcb
);


/** 
@ingroup Timer Functions 
         
@brief  
 Returns the number of sclks until the expiry of next timer to expire. \n\n

@param[in]                                                               
 None                                                                   \n\n 
   
@dependencies  
 This function is provided for the exclusive use of SLEEP,for TCXO shutdown.\n
 Must be called from an INTLOCK'd context.                                \n\n  
 
@param[out]
 timetick_type - Number of sclk's until the next timer expiry.           \n\n 
 
@sideffects 
 None                                                                    \n\n

*/

timetick_type timer_get_sclk_till_expiry( void );


/** 
@ingroup Timer Functions 
         
@brief  
 This function sets the sclk timer interrupt to expire in advance of the  
 the next expiring timer's expiry point,to allow for TCXO to turn back on.\n\n

@param[in]                                                               
 timetick_type - Wakeup the CPU in the given number of sclk's.            \n\n
   
@dependencies  
 This function is provided for the exclusive use of SLEEP, for TXCO 
 shutdown. Must be called from INTLOCK'd context                         \n\n  
 
@param[out]
 None                                                                    \n\n 
 
@sideffects 
 None                                                                    \n\n

*/

void timer_set_wakeup
(
  timetick_type                   wakeup_sclks
);

/** 
@ingroup Timer Functions 
         
@brief  
 This function is called after waking up. The timetick counter values   
 are readjusted to sync up with the base timetick counter when waking up 
 from  power collapse. The match value has to be adjusted accordingly.   \n\n

@param[in]                                                               
 None                                                                   \n\n 
   
@dependencies  
 None                                                                   \n\n   
 
@param[out]
 None                                                                   \n\n 
 
@sideffects 
 Adjusts the match value                                                \n\n

*/
void timer_restart(void);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* TIMER_H */


