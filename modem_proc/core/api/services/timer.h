#ifndef REX_H
/* Circular #include: "rex.h" includes "timer.h", which includes "rex.h".
   Ensure first half of "rex.h" is included before "timer.h" processed. */
#include "rex.h"
#endif /* REX_H */

#ifndef TIMER_H
#define TIMER_H

/**
@file timer.h
@brief
Implements time-keeping functions using the slow clock.

Timer groups are a way of enabling and disabling groups of timers at the same time. This is
 mainly used by the sleep subsystem to disable unwanted timers during sleep. For this reason, one
 should be careful which group their timer is set in, to help conserve power. 
 
When timer's are active, they live on the active list. When deactivated, they are moved to their appropriate timer group.
When a timer group is re-enabled, they are spooled back onto the active list. Timers that were pending expire once,
 while recurring ones get bumped to the future.

<table>
 <tr>
   <td><center><b>Slow Clock Frequency</b></center></td>
   <td><center><b>Granularity</b></center></td>
   <td><center><b>Timer Range</b></center></td>
 </tr>
 <tr>
   <td><center>30.000kHz (lower limit)</center></td>
   <td><center>33.3 us</center></td>
   <td><center>39.7 hrs</center></td>
 </tr>
 <tr>
   <td><center>32.768kHz (nominal freq)</center></td>
   <td><center>30.5 us</center></td>
   <td><center>36.4 hrs</center></td>
 </tr>
 <tr>
   <td><center>60.000kHz (upper limit)</center></td>
   <td><center>16.6 us</center></td>
   <td><center>19.8 hrs</center></td>
 </tr>
</table>

INITIALIZATION AND SEQUENCING REQUIREMENTS

timer_init() must be called once before any other timer functions.

timer_def() must be called, for each timer, 
before other timer functions are used with that timer.

A valid sclk estimate is needed before timers can be set.

*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_services group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2003-2013 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
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

    timer_def() must be called for each timer, 
    before other timer functions are used with that timer.

    A valid sclk estimate is needed before timers can be set.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/timer.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
11/12/10   din     Added function to get time to next occuring non-deferrable 
                   timer expiry.
10/27/10   EBR     Doxygenated File.
12/10/09   din     Added sclk_delta to timer_type. 
11/25/09   din     Added timer_[un]defer_match_interrupt()
05/22/09   din     Removed FEATURE_TIMER_TASK
01/07/09   pbi     Removed timer_set_sclk_offset
02/23/09   din     Consoldation of time.
01/13/09   din     Removed timer_delay_expiry_excluding_groups() and
                   timer_undelay_expiry().
01/12/09   din     Added deferrable attribute to timer_group. Also added 
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
#include "time_osal.h"


#ifdef __cplusplus
/* Avoid name mangling for any C++ customers */
extern "C" {
#endif

/** @addtogroup time_services
@{ */

/*==============================================================================

                           DATA DECLARATIONS

=============================================================================*/

/** @brief Timer structure enumerated type for error detection. 
      @note The error types contained below are a superset of all the 
                error types available on timer-services on other targets,
                thus all of them should not be taken for granted to be 
                available for all the API's where they are returned.
                Please consult the concerned API's comments for the
                particular specifics regarding which error types to expect.
*/
typedef enum
{
  TE_SUCCESS = 0,
  TE_FAIL,
  /* Need to make "timer_client" as dependency if some module is calling
     into timer creation apis before timer module gets initialized */
  TE_TIMER_MODULE_NOT_INITIALIZED, /* Error returned if timer creation apis are called before timer system is initialized */
  TE_HANDLE_IN_USE,              /* Error returned if timer is being defined upon a valid timer */
  TE_INVALID_TIMER_HANDLE,       /* Error returned if timer is invalid */
  TE_INVALID_GROUP_HANDLE,       /* Error returned if group is invalid */
  TE_INVALID_PARAMETERS,         /* Error returned if input parameters for an api are invalid */
  TE_INVALID_DURATION,           /* Error returned if timer is invalid duration */
  TE_INVALID_STATE_FOR_OP,       /* Error returned if timer is in invalid state for requested operation */
  TE_MALLOC_FAILED,              /* Error returned if free client timers are over */
  TE_NO_FREE_GROUP,              /* Error returned if free client timers are over */
  TE_NO_FREE_CLIENT_TIMER,       /* Error returned if free client timers are over */
  TE_NO_FREE_INTERNAL_TIMER,     /* Error returned if free internal timers are over */
  TE_TIMER_NOT_ACTIVE,           /* Error returned if timer is not active but an operation expects it */
  TE_TIMER_ALREADY_IN_SAME_STATE,/* Error returned if timer is already in state where an operation is being tried */
  TE_MAX = 0xFFFFFFFF  
}
timer_error_type;

/*@brief Notification Callback type 
   @note  The notification are expected to be exclusive for any timer, i.e. for any given timer
              it is expected to have only a single notification type, it can be either a callback
              type notification or signalling based notification or no notification at all, but both type
              of notification at the same time is not expected.
*/
typedef enum
{
  TIMER_NO_NOTIFY_TYPE = 0x0,           /* Use this if, no notification is required */
  TIMER_NATIVE_OS_SIGNAL_TYPE = 0x1,    /* In Qurt, required for qurt_anysignal_t object */
  TIMER_NATIVE_OS_SIGNAL2_TYPE = 0x10,  /* In Qurt, required for qurt_signal2_t object */
  TIMER_REX_TCB_SIGNAL_TYPE = 0x2,      /* For Rex only, rex_tcb_type is required */
  TIMER_DALSYS_EVENT_SIGNAL_TYPE = 0x20,/* For Dalsys Event objects, DALSYSEventHandle* obj addr is required */
  TIMER_FUNC1_CB_TYPE = 0x4,            /* timer_t1_cb_type type of function pointer is expected */
  TIMER_FUNC2_CB_TYPE = 0x8,            /* timer_t2_cb_type type of function pointer is expected */
  TIMER_INVALID_NOTIFY_TYPE
}
timer_notify_type;

#define TIMER_NOTIFICATION_CHECK_MASK  \
	(TIMER_NATIVE_OS_SIGNAL_TYPE |  \
	TIMER_NATIVE_OS_SIGNAL2_TYPE |     \
	TIMER_REX_TCB_SIGNAL_TYPE |     \
	TIMER_DALSYS_EVENT_SIGNAL_TYPE |     \
	TIMER_FUNC1_CB_TYPE |              \
	TIMER_FUNC2_CB_TYPE)



/** Timer 'type 1' callback arbitrary data type.
*/
typedef unsigned long             timer_cb_data_type;

/** Timer 'type 1' callback function.
*/
typedef void (*timer_t1_cb_type)
(
  timer_cb_data_type              data  
    /**< Arbitrary data given to timer_def( ) to pass to this callback 
         function. */
);

/** Timer 'type 2' callback function (for clock callback style interface)
*/
typedef void (*timer_t2_cb_type)
(
  int32                           time_ms, 
    /**< Time elapsed from timer setting (start). */
  timer_cb_data_type              data     
    /**< Arbitrary data given to timer_def2( ) to pass to this callback 
         function. */
);

/** @brief Timer structure pointer types.
*/
typedef struct timer_group_struct *timer_group_ptr;
/** @brief Pointer to the timer list structure. 
*/
typedef struct timer_struct_internal *timer_ptr_internal_type;
/** Pointer to timer structure */
typedef struct timer_struct       *timer_ptr_type;

/** @brief Timer data type for 64-bit data. 
*/
typedef uint64 time_timetick_type;

#ifndef FEATURE_L4
/* The L4 project has issues where timer_ptr is used other than as a typedef.
   For other projects, temporarily define timer_ptr a migration aid. */
//#define timer_ptr                 timer_ptr_type
#endif /* !FEATURE_L4 */


/** @brief Timer list structure type. Values in this structure are for private 
use by timer.c only. */
typedef struct timer_list_struct
{  
  timer_ptr_internal_type         first; /**< List of timers. */

  timer_ptr_internal_type         last;  /**< End of the timer list. */
}
timer_list_type;


/** @brief Timer group structure type. Values in this structure are for private 
use by timer.c only. */
typedef struct timer_group_struct
{
  timer_list_type                 list;       /**< Timers in the disabled timer 
                                                   group. */
  boolean                         disabled;   /**< Disabled flag for the timer 
                                                   group. */
  boolean                         deferrable; /**< Deferrable flag for the 
                                                   timer group. Default = FALSE. */
  timer_group_ptr                 next;       /**< Link to the next timer group. */
}
timer_group_type;


/** @brief Timer state structure type.
      @note The consecutive incremental value sequence
                for the below enums should not be disturbed.
*/
typedef enum timer_state_struct
{
  TIMER_UNDEF_FLAG = 0x0FEDCBA8,
  TIMER_DEF_FLAG,
  TIMER_SET_FLAG,
  TIMER_EXPIRED_FLAG,
  TIMER_CLEARED_FLAG,
  TIMER_PAUSED_FLAG,
  TIMER_RESUMED_FLAG,
} timer_state_struct_type;

/** @brief Timer Priority Type. */
typedef enum timer_priority_enum
{
  TIMER_PRIORITY_0,
  TIMER_PRIORITY_1,
} timer_priority_enum_type;

#define TIMER_MAX_PRIORITY   TIMER_PRIORITY_1


/** @brief Timer structure type. Values in this structure are for private use 
by timer.c only.*/
typedef struct timer_struct
{
  /* Group to which timer is a member */
  timer_group_ptr                 group; 
    /**< Group to which the timer is a member. */
  rex_tcb_type                    *tcb;  
    /**< Task to signal when the timer expires, or the task to use for the APC 
         function. */
  rex_sigs_type                   sigs;  
    /**< Signal(s) to set in the task when the timer expires. */
  timer_t1_cb_type                func1; 
    /**< Asynchronous Procedure Call (APC) function to call when the timer 
         expires. */
  timer_t2_cb_type                func2; 
    /**< Clock callback style function to call when the timer expires. */
  timer_cb_data_type              data;

  timer_notify_type               notify_type; 
    /**< Timer notification type. */

  int32                           timer_handle;
  /**< Handle to the internal timer. */

    /**< Data for the APC function when the timer expires. */
  time_timetick_type                   last_expiry; 
    /**< Last time timer expired, initialized to 0, updated only in 
         timer_expire, Remains zero if timer is cleared. */
  time_timetick_type                   last_start; 
    /**< Slow-clock tick count value when the timer was set (started).
         Updated only in timer_insert */
  timer_state_struct_type         timer_state; 
    /**< State of the current external timer
         Updated only in timer_insert */
  uint8                            slave_task_assignment;

   /**< Timer priority p0 or p1 timers */
  timer_priority_enum_type          timer_priority;
} 
timer_type;

/** @brief Timer attrib structure type. The values in this structure 
is to be used by the clients of timer-services to update/get these. */
typedef struct timer_attrib_structure
{
void *                   tcb_sigs_obj; /* Container for the TCB pointer or signal object address */
uint32       			 sigs;          /* Container for the signal mask value  */
timer_t1_cb_type		 func1;         /* func1 pointer */
timer_t2_cb_type		 func2;         /* func2 pointer */
timer_cb_data_type		 data;          /* value of the data used for func1 or func2 */
timer_notify_type        notify_type;   /* This will be OR'ed value of the notification type */
timer_state_struct_type  timer_state;   /* The current state which the timer is in */
} timer_attrib_struct_type;

/** @brief Timer attrib enum types. These values are for setting params 
using timer_set_params API */
typedef enum timer_attrib_set_enum
{
TIMER_UPDATE_NOTIFICATION_PARAMS = 0xBEE00EEB,       /* To get/set specific notification */
} timer_attrib_set_type;




/* The NULL timer group, for timers not in any other group */

/** Timers declared to be in the NULL group are placed into the NULL timer list. */
extern timer_group_type           timer_null_group;

/* Global Non-deferrable group for various clients use */
extern timer_group_type           timer_non_defer_group;

/*==============================================================================

                        REGIONAL FUNCTION DECLARATIONS

=============================================================================*/


/** 
Initializes the Timer subsytem.

@return
None.
 
@dependencies  
None.
 
@sideeffects 
This function may install an ISR for the 32-bit slow-clock match register.
*/
void timer_init( void );


/** 
Initializes the Timer task.

@param[in] ignored Unused parameter required to match rex_task_func_type 
                   prototype.

@return
None.

@dependencies  
None.
*/
void timer_task
(
  dword                           ignored
);



/*==============================================================================

                            FUNCTION DECLARATIONS

=============================================================================*/


/**
Enables a timer group. Timers in the timer group that expired during the
diabled period have their expiry processing executed.

@param[in] timer_group Timer group to be enabled.

@return
TE_INVALID_PARAMETERS  If the passed timer_group pointer is invalid
TE_SUCCESS  If the operation completed successfully

@dependencies  
None.

@sideeffects 
This function may generate signals to a task, which can cause task switches. \n
This function may queue an asynchronous procedure call.
*/
timer_error_type timer_group_enable
(
  timer_group_ptr                 timer_group
);


/**
Disables a timer group. Timers in the timer group continue to count
down, but they do not expire.

@param[in] timer_group Timer group to be disabled.

@return
TE_INVALID_PARAMETERS  If passed timer_group pointer is NULL
TE_INVALID_GROUP_HANDLE  If the group asked to be disabled is time_prio0_group
TE_SUCCESS  If the operation completed successfully
 
@dependencies
None.
*/
timer_error_type timer_group_disable
(
  timer_group_ptr                 timer_group
);


/**
Sets the timer group deferrable status. 

@note1hang Timer groups default to being non-deferrable.

@param[in] pgroup    Timer group to be undeferred.
@param[in] condition Boolean value: \n
                     TRUE -- Deferrable. \n
                     FALSE -- Otherwise.
@return
TE_INVALID_PARAMETERS  If passed group pointer is NULL
TE_INVALID_GROUP_HANDLE  If the operation is asked on a group for which it is not allowed, i.e.
                                             - cannot set timer_null_group to non-deferrable
                                             - cannot set timer_non_defer_group to deferrable
TE_SUCCESS : If the operation completed successfully
 
@dependencies  
None.
*/
timer_error_type timer_group_set_deferrable
( 
  timer_group_ptr pgroup, 
  boolean condition 
);


/**
Gets the first non-deferrable timer's expiry value.


@return
Absolute expiry of first occuring non-deferrable timer.
Returns TIMER_MAX_EXPIRY in case there is no non-deferrable timer present.

@dependencies  
None. 
*/


time_timetick_type timer_get_first_non_deferrable_timer_expiry( void );

/**
Registers for first non-deferrable timer expiry change notification.
Only signal type notifications are allowed, for others, error will be 
returned.

@param[in] notify_type       notification type from timer_notify_type. Only signal notify type
@param[in] notify_obj        notify object address that should be set on expiry change. Only signal obj addr.
@param[in] notify_mask       mask to be set to the signal obj addr provided

@dependencies Input objects should be initialized by client

@return
TE_INVALID_PARAMETERS   If passed sig obj addr is NULL or invalid notify type
TE_SUCCESS :            If the operation completed successfully
 
@dependencies  
None.
*/
timer_error_type timer_register_for_non_defer_expiry_change
(
  timer_notify_type        notify_type,
  time_osal_notify_obj_ptr notify_sig_obj,
  time_osal_notify_data    notify_sig_mask
);

/**
Gets time to the next occurring non-deferrable timer expiry value.                                                                       

@return
Number of sclk's in 32kHz domain until the next timer expiry in 
 a timer group which is non-deferrable.

@dependencies  
Must be called with interrupts locked. 
*/


timetick_type timer_get_time_till_first_non_deferrable_timer( void ) ;


/**
Gets time to the next occurring non-deferrable timer expiry value.                                                                       

@return
Number of ticks in 19MHz domain until the next timer expiry in a
 timer group which is non-deferrable.

@dependencies  
Must be called with interrupts locked. 
*/


time_timetick_type timer_get_time_till_first_non_deferrable_timer_64( void ) ;

/**  
Sets timer match interrupt to the next occurring non-deferrable timer expiry
value. Complementary function is timer_undefer_match_interrupt().

@return
timetick_type -- Number of slow clocks in 32kHz domain until the
next timer expiry in a timer group that is non-deferrable. 

@dependencies  
This function must be called with interrupts locked.
 

@sideeffects 
Adjusts the match value and prevents any subsequent timer calls from
updating the match value until timer_undefer_match_interrupt() is called.
*/
timetick_type timer_defer_match_interrupt( void ) ;

/**  
Sets timer match interrupt to the next occurring non-deferrable timer expiry
value. Complementary function is 
timer_undefer_match_interrupt_64(). 

@return
time_timetick_type -- Number of ticks in 19MHz domain 
until the next timer expiry in a timer group that is 
non-deferrable. 

@dependencies  
This function must be called with interrupts locked.
 

@sideeffects 
Adjusts the match value and prevents any subsequent timer calls from
updating the match value until timer_undefer_match_interrupt() is called.
*/
time_timetick_type timer_defer_match_interrupt_64( void ) ;

/** 
Sets the timer ISR to fire at the earliest expiring timer in the active list.
This function is meant to serve as the complementary function to 
timer_defer_match_interrupt().

@dependencies  
This function must be called with interrupts locked.

@sideeffects 
This function adjusts the match value.
*/
void timer_undefer_match_interrupt( void );

/**
Defines and initializes a timer.
                                  
The timer expires when:\n
- sigs is non-NULL. The signals are set to the task given by tcb.
- func is non-NULL and tcb is NULL. The callback function with the 
  argument data is called from the interrupt context. Otherwise, the callback 
  function is queued as an APC.

@param[in] timer Timer to set.
@param[in] group Group timer belongs to (NULL=the "NULL" timer group).
@param[in] tcb   Task to signal and/or the task for which to queue the APC call.
@param[in] sigs  Task signals to set to the tcb task when the timer expires.
@param[in] func  APC (tcb!=NULL) or callback (tcb==NULL) to queue/call at timer expiry.
@param[in] data  Arbitrary data for the APC or callback.

@return
TE_INVALID_STATE_FOR_OP  If the timer is in paused state
TE_SUCCESS  If the operation completed successfully

@dependencies  
None.
*/
timer_error_type timer_def
(
  timer_ptr_type                  timer,
  timer_group_ptr                 group,
  rex_tcb_type                    *tcb,
  rex_sigs_type                   sigs,
  timer_t1_cb_type                func,
  timer_cb_data_type              data
);

/** 
Api to define timer with specifying priority

@param[in] timer Timer to set.
@param[in] group Group timer belongs to (NULL=the "NULL" timer group).
@param[in] tcb   Task to signal and/or the task for which to queue the APC call.
@param[in] sigs  Task signals to set to the tcb task when the timer expires.
@param[in] func  APC (tcb!=NULL) or callback (tcb==NULL) to queue/call at timer expiry.
@param[in] data  Arbitrary data for the APC or callback.
@param[in] timer_prio  Priority of the timer.

@return
TE_INVALID_STATE_FOR_OP  If the timer is in paused state
TE_SUCCESS  If the operation completed successfully

@dependencies  
Not be called before allocating timer memory

@sideeffects 
None
*/
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
);

/** 
Api for timer_def2 with specifying priority

@param[in] timer Timer to set.
@param[in] group Group timer belongs to (NULL=the "NULL" timer group).
@param[in] timer_prio  Priority of the timer.

@return
TE_INVALID_STATE_FOR_OP  If the timer is in paused state
TE_SUCCESS  If the operation completed successfully

@dependencies  
Not be called before allocating timer memory 

@sideeffects 
None
*/
timer_error_type timer_def2_priority
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group,

  timer_priority_enum_type        timer_prio
);

/**
Defines a clock callback-style timer. When the timer expires, the callback 
function with the arguments time_ms and data is called from the interrupt context. 

@param[in] timer Timer to set.
@param[in] group Group timer belongs to (NULL=the NULL timer group).
  
@return
TE_INVALID_STATE_FOR_OP  If the timer is in paused state
TE_SUCCESS  If the operation completed successfully

@dependencies  
The timer must be initialized with timer_reg().
*/
timer_error_type timer_def2
(
  timer_ptr_type                  timer,
  timer_group_ptr                 group
);

/**
Defines and initializes a timer. Signals to be initialized are native kernel signal type objects.
Signal and function notifications are exclusive. If both are provided at the same time,
error will be returned.
                                  
The timer expires when:\n
- sigs is non-NULL. The signals are set to sigs_obj.
- func is non-NULL. The callback function with the argument data
  is called from the timer tasks.

@note1hang
- A defined timer shouldn't be re-defined. TE_HANDLE_IN_USE will be returned in such case
- Signal or function notifications are "Exclusive".
- If Signal is used, then it should be properly initialized before passing it to the API
- Client should call timer_undef() once a timer is not required

@param[in] timer Timer to set.
@param[in] group Group timer belongs to (NULL=the "NULL" timer group). 
@param[in] cb_type Refer to comments on timer_notify_type
@param[in] sigs_func_ptr   signal or function ptr depending on cb_type
@param[in] sigs_mask_data  signal mask for signal obj or data for callback
                  function that needs to be returned
@return
  timer_error_type: Possible return values are
  TE_SUCCESS  Operation completed successfully
  TE_INVALID_PARAMETERS  Passed params are incorrect for this API
  TE_INVALID_STATE_FOR_OP  If the timer is in paused state

@dependencies  
If reusing a previously defined timer, call timer_undef() before calling timer_def.
*/
timer_error_type timer_def_osal
(
  timer_ptr_type                  timer,
  timer_group_ptr                 group,
  timer_notify_type				  cb_type,
  time_osal_notify_obj_ptr		  sigs_func_addr,
  time_osal_notify_data			  sigs_mask_data
);

/**
Defines and initializes a timer. Signals to be initialized are native kernel signal type objects.
Signal and function notifications are exclusive. If both are provided at the same time,
error will be returned.
                                  
The timer expires when:\n
- sigs is non-NULL. The signals are set to sigs_obj.
- func is non-NULL. The callback function with the argument data
  is called from the timer tasks.

@note1hang
- A defined timer shouldn't be re-defined. TE_HANDLE_IN_USE will be returned in such case
- Signal or function notifications are "Exclusive".
- If Signal is used, then it should be properly initialized before passing it to the API
- Client should call timer_undef() once a timer is not required

@param[in] timer Timer to set.
@param[in] group Group timer belongs to (NULL=the "NULL" timer group). 
@param[in] cb_type Refer to comments on timer_notify_type
@param[in] sigs_func_ptr   signal or function ptr depending on cb_type
@param[in] sigs_mask_data  signal mask for signal obj or data for callback
                  function that needs to be returned
@param[in] timer_prio  Priority of the timer.

@return
  timer_error_type: Possible return values are
  TE_SUCCESS  Operation completed successfully
  TE_INVALID_PARAMETERS  Passed params are incorrect for this API
  TE_INVALID_STATE_FOR_OP  If the timer is in paused state
  
@dependencies  
If reusing a previously defined timer, call timer_undef() before calling timer_def.
*/
timer_error_type timer_def_osal_priority
(
  timer_ptr_type                  timer,
  timer_group_ptr                 group,
  timer_notify_type				  cb_type,
  time_osal_notify_obj_ptr		  sigs_func_addr,
  time_osal_notify_data			  sigs_mask_data,
  timer_priority_enum_type        timer_prio
);

/**
Initializes a clock callback style timer.

@param[in] timer     Timer to set.
@param[in] func      Function to call at timer expiry.
@param[in] data      Arbitrary data for the callback function with the 
                     arguments time_ms and data.
@param[in] time_ms   Time in milliseconds until first timer expiry.
@param[in] reload_ms Period in milliseconds between repeated expiries (0=not periodic).

@return
error return values
TE_INVALID_STATE_FOR_OP  If the timer is not in the right state (defined,cleared,set,etc..)
TE_INVALID_PARAMETERS  Passed params are incorrect for this API
TE_SUCCESS
  
@dependencies
The timer must be initialized with timer_def2().
*/
timer_error_type timer_reg
(
  timer_ptr_type                  timer,
  timer_t2_cb_type                func,
  timer_cb_data_type              data,
  timetick_type                   time_ms,
  timetick_type                   reload_ms
);


/**
Sets an inactive timer to expire after a given period of time, or changes an
active timer to expire after a given period of time. Optionally, specifies
the timer to repeatly expire with a given period.  If the timer 
is to be set in units of ticks, it will be done in the 32kHz 
domain. 

@note1hang
 - This API is not able to handle operations based on the 19.2 Mhz based 64 bit timetick value.

@param[in] timer  Timer to set.
@param[in] time   Time in units specified until first timer 
      expiry.
@param[in] reload Period in units specified between repeated 
      expiries (0=not periodic).
@param[in] unit   Unit to use to measure ticks.

@return
error return values
TE_INVALID_STATE_FOR_OP  If the timer is not in the right state (defined,cleared,set,etc..)
TE_INVALID_PARAMETERS  Passed params are incorrect for this API
TE_SUCCESS
  
@dependencies  
None.

@sideeffects 
This function may generate signals to a task, which can cause task switches. \n
This function may queue an APC.
*/
timer_error_type timer_set
(
  timer_ptr_type                  timer,
  timetick_type                   time,
  timetick_type                   reload,
  timer_unit_type             unit
);


/**
Sets an inactive timer to expire after a given period of time, or changes an
active timer to expire after a given period of time. Optionally, specifies
the timer to repeatly expire with a given period.  If the timer 
is to be set in units of ticks, it will be done in the 19MHz 
domain. 

@param[in] timer  Timer to set.
@param[in] time   Time in units specified until first timer 
      expiry.
@param[in] reload Period in units specified between repeated 
      expiries (0=not periodic).
@param[in] unit   Unit to use to measure ticks.

@return
TE_INVALID_STATE_FOR_OP  If the timer is not in the right state (defined,cleared,set,etc..)
TE_INVALID_PARAMETERS  Passed params are incorrect for this API
TE_SUCCESS
  
@dependencies  
None.

@sideeffects 
This function may generate signals to a task, which can cause task switches. \n
This function may queue an APC.
*/
timer_error_type timer_set_64
(
  timer_ptr_type                  timer,
  time_timetick_type              time,
  time_timetick_type              reload,
  timer_unit_type             unit
);


/**
Sets an inactive timer to expire after a given time, or changes 
an active timer to expire after a given time. 

@param[in] timer  Timer to set.
@param[in] time   Time in XOs specified until first timer 
      expiry.

@return
TE_INVALID_STATE_FOR_OP  If the timer is not in the right state (defined,cleared,set,etc..)
TE_INVALID_PARAMETERS  Passed params are incorrect for this API
TE_SUCCESS
  
@dependencies  
None.

@sideeffects 
This function may generate signals to a task, which can cause task switches. \n
This function may queue an APC.
*/
timer_error_type timer_set_absolute
(
  timer_ptr_type                  timer,
  time_timetick_type              time
);


/**
Gets the number of units before the timer expires. If the unit 
is ticks, it will return value in 32kHz domain. 

@param[in] timer Timer to get.
@param[in] unit  Unit to use to measure the ticks before the timer expires.

@note1hang
 - This API is not able to handle operations based on the 19.2 Mhz based 64 bit timetick value.

@return
Number of units before timer expires. \n
@note1hang \n
- Zero is returned for inactive timers and timers that have expired or are
expiring. 
- Fractional values that might truncate to zero, depending on the unit
passed as a parameter, are rounded to 1.
  
@dependencies  
None.
*/
timetick_type timer_get
(
  timer_ptr_type                  timer,
  timer_unit_type             unit
);

/**
Gets the number of units before the timer expires. If the unit 
is ticks, it will return value in 19MHz domain. 

@param[in] timer Timer to get.
@param[in] unit  Unit to use to measure the ticks before the timer expires.

@return
Number of units before timer expires. \n
@note1hang \n
- Zero is returned for inactive timers and timers that have expired or are
expiring. 
- Fractional values that might truncate to zero, depending on the unit
passed as a parameter, are rounded to 1.
  
@dependencies  
None.
*/
time_timetick_type timer_get_64
(
  timer_ptr_type                  timer,
  timer_unit_type             unit
);

/**
Updates the passed structure as per the timer parameters.

@note1hang
If the timer is having multiple notifications, then the notify_type
from the passed attrib structure will be an OR'ed value of the notification
enums.

@param[in] timer Timer, which is asked for.
@param[out] timer_attrib_struct_ptr Pointer to structure which will be updated

@return
  TE_SUCCESS if success
  TE_INVALID_PARAMETERS if passed pointers are NULL
  
@dependencies
  None
*/

timer_error_type timer_get_params
(
  timer_ptr_type				timer,
  timer_attrib_struct_type *    timer_attrib_struct_ptr
);


/**
Updates the timer as per the passed params.

@note1hang
If the update is for the notifications of the timer, then proper
value should be put in notify_type from the timer_attrib_struct_type.
The value is meant to be OR'ed with required timer_notify_type enums 
if there are multiple notifications to be updated.

@param[in] timer Timer, for which the params need to be updated
@param[in] to_set Enum for what to update
@param[in] timer_attrib_struct_ptr Pointer to structure containing the update

@return
  TE_SUCCESS if success
  TE_INVALID_PARAMETERS if passed pointers are NULL
  
@dependencies
  None
*/
timer_error_type timer_set_params
(
  timer_ptr_type				timer,
  timer_attrib_set_type         to_set,
  timer_attrib_struct_type *    timer_attrib_struct_ptr
);


/**
Queries whether a timer is active. A timer that has been set to expire at a 
certain time is considered to be active even if the group it is a member of is 
disabled. A timer that is explicitly paused is not active until it has been 
resumed.

@param[in] timer Timer to be queried.

@return
TRUE  -- Timer is active. \n
FALSE -- Timer is not active.

@dependencies  
None.
*/
boolean timer_is_active
(
  timer_ptr_type  timer
);
#define TIMER_IS_ACTIVE(timer)  ( timer_is_active(timer) )


/**
Returns the XO counter value when the timer expires.

@param[in] timer Timer to be queried.

@return
XO counter value corresponding to the timer expiry point.
  
@dependencies  
The timer must be running for the returned value to be meaningful.

@sa
DalTimetick_GetTimetick64()
*/
time_timetick_type timer_expires_at_64
(
  timer_ptr_type  timer
);

/**
Returns the slow-clock counter value when the timer expires.

@param[in] timer Timer to be queried.

@return
Slow-clock counter value corresponding to the timer expiry point.
  
@dependencies  
The timer must be running for the returned value to be meaningful.

@sa
timetick_get()
*/
timetick_type timer_expires_at
(
  timer_ptr_type  timer
);

#define TIMER_EXPIRES_AT_64(timer)  ( timer_expires_at_64(timer) )
#define TIMER_EXPIRES_AT(timer)  ( timer_expires_at(timer) )

/**
Returns the slow-clock counter value in the 32kHz domain when 
the timer started. 

@param[in] timer Timer to be queried.

@return
Slow-clock counter value in the 32kHz domain corresponding to 
the timer start point. 
  
@dependencies  
The timer must be running for the returned value to be meaningful.

@sa
timetick_get()
*/
timetick_type timer_get_start
(
  timer_ptr_type  timer
);

/**
Returns the slow-clock counter value in the 19MHz domain when 
the timer started. 

@param[in] timer Timer to be queried.

@return
Slow-clock counter value in the 19MHz domain corresponding to 
the timer start point. 
  
@dependencies  
The timer must be running for the returned value to be meaningful.

@sa
timetick_get()
*/
time_timetick_type timer_get_start_64
(
  timer_ptr_type  timer
);


/**
Stops an active timer.  If units are in ticks, then it will 
return the value in the 32kHz domain. 

@param[in] timer Timer to be stopped.
@param[in] unit  Unit to use to measure the ticks before expiry.
                 Use T_NONE if the unit measurement is not required.

@note1hang
 - This API is not able to handle operations based on the 19.2 Mhz based 64 bit timetick value.
                   
@return
Number of units remaining before expiry.
  
@dependencies  
None.
*/
timetick_type timer_clr
(
  timer_ptr_type                  timer,
  timer_unit_type             unit
);

/**
Stops an active timer.  If units are in ticks, then it will 
return the value in the 19MHz domain. 

@param[in] timer Timer to be stopped.
@param[in] unit  Unit to use to measure the ticks before expiry.
                 Use T_NONE if the unit measurement is not required.
                   
@return
Number of units remaining before expiry.
  
@dependencies  
None.
*/
time_timetick_type timer_clr_64
(
  timer_ptr_type                  timer,
  timer_unit_type             unit
);

/**
Same as timer_clr which stops an active timer and also with an error return.

@note1hang
 - This API will return TE_TIMER_ALREADY_IN_SAME_STATE in case timer
   is already in the asked state. This is not an error case & should be
   carefully handled.
   
@param[in] timer Timer to be stopped.
@param[in] unit  Unit to use to measure the ticks before expiry.
@param[in] rem_time  Amount of time remaining before expiry. 
                 Time will be in specified unit. This can be NULL.
                 If the unit is T_TICK, ticks will be from XO clk
                   
@return
Status of the operation. Possible return values are:
TE_INVALID_PARAMETERS
TE_TIMER_ALREADY_IN_SAME_STATE -- If timer is already expired.

TE_SUCCESS
  
@dependencies  
None.

@Note
If the timer_stop api is called at the moment of timer being expired
there can be chances that timer gets expired first and then timer_stop
will simply return 0 as timer got expired. Therefore, it is always
advised to check if there is sufficient time(ex: 1msec or few ticks)
before calling this api. timer_get() can be used to check how much
time is still left for the expiry.
*/
timer_error_type timer_stop
(
  timer_ptr_type                 timer,
  timer_unit_type            unit,
  time_timetick_type             *rem_time
);

/**
Removes timer from active list and deallocates all the timer memory
for the given timer handle.

@note1hang
 - This API will return TE_TIMER_ALREADY_IN_SAME_STATE in case timer
   is already in the asked state. This is not an error case & should be
   carefully handled.

@param[in] timer Timer to be stopped.

@return
  TE_SUCCESS if success
  TE_INVALID_PARAMETERS if timer  pointer passed is invalid
  TE_TIMER_ALREADY_IN_SAME_STATE if timer is already deleted
  

@dependencies
  None
*/

timer_error_type timer_undef
(
  /* Timer to stop */
  timer_ptr_type                  timer
);


/**
Pauses an active timer.

@note1hang
 - This API will return TE_TIMER_ALREADY_IN_SAME_STATE in case timer
   is already in the asked state. This is not an error case & should be
   carefully handled.

@param[in] timer Timer to be paused.
 
@return
TE_INVALID_TIMER_HANDLE  If the timer is not active
TE_SUCCESS  If the operation was success
TE_TIMER_ALREADY_IN_SAME_STATE - If timer is already in paused state.

@dependencies  
This function is only called for a timer that is running.
*/
timer_error_type timer_pause
(
  timer_ptr_type                  timer
);


/**
Resumes a previously paused timer. When a timer is paused 5 seconds before its
expiry and timer_resume( ) is called 30 seconds later, the timer expires
5 seconds after timer_resume( ) was called.

@note1hang
 - This API will return TE_TIMER_ALREADY_IN_SAME_STATE in case timer
   is already in the asked state. This is not an error case & should be
   carefully handled.

@param[in] timer Timer to be resumed.

@return
TE_INVAID_TIMER_HANDLE - If passed timer does not have any internal timer (it is not paused)
TE_SUCCESS - If the operation was success
TE_TIMER_ALREADY_IN_SAME_STATE - If the timer is already in resumed state.

@dependencies  
This function must only be called for a timer that has been paused.

@sideeffects 
This function may generate signals to a task, which can cause task switches. \n
This function may queue an APC.
*/
timer_error_type timer_resume
(
  timer_ptr_type                  timer
);

/**
Stops active timers belonging to the given task.

@param[in] Thread ID Timers belonging to this thread are cleared (stopped).

@return
None.

@dependencies  
None.
*/
void timer_clr_osal_thread_timers
(
  time_osal_thread_id                    thread_id
);

/**
Stops active timers belonging to the given task.

@param[in] tcb Timers belonging to this task are cleared (stopped).

@return
None.

@dependencies  
None.
*/
void timer_clr_task_timers
(
  rex_tcb_type                    *tcb
);


/**
Returns the number of slow clocks in 32kHz domain until the 
expiry of the next timer to expire. 

@return
Number of slow clocks in the 32kHz domain until the next timer 
expiry. 

@dependencies  
This function is for the exclusive use of SLEEP for TCXO shutdown. \n
This function must be called with interrupts locked.
*/
timetick_type timer_get_sclk_till_expiry( void );


/**
Returns the number of slow clocks in 19MHz domain until the 
expiry of the next timer to expire. 

@return
Number of slow clocks in the 19MHz domain until the next timer 
expiry. 

@dependencies  
This function is for the exclusive use of SLEEP for TCXO shutdown. \n
This function must be called with interrupts locked.
*/
time_timetick_type timer_get_sclk_till_expiry_64( void );


/**
Sets the slow-clock timer interrupt to expire in advance of the next expiring 
timer expiry point to allow for TCXO to turn on.

@param[in] wakeup_sclks Wakes up the CPU in the given number of 
      slow clocks in 32kHz domain.

@return
TE_SUCCESS  If the operation completed successfully

@dependencies  
This function is for the exclusive use of SLEEP for TXCO shutdown. \n
This function must be called with interrupts locked.
*/
timer_error_type timer_set_wakeup
(
  timetick_type                        wakeup_sclks
);


/**
Sets the slow-clock timer interrupt to expire in advance of the next expiring 
timer expiry point to allow for TCXO to turn on.

@param[in] wakeup_sclks Wakes up the CPU in the given number of 
      slow clocks in 19MHz domain.

@return
TE_SUCCESS  If the operation completed successfully

@dependencies  
This function is for the exclusive use of SLEEP for TXCO shutdown. \n
This function must be called with interrupts locked.
*/
timer_error_type timer_set_wakeup_64
(
  time_timetick_type                   wakeup_sclks
);

/**
Readjusts the timetick counter values to synchronize with the base timetick 
counter when waking up from a power collapse. The match value must be adjusted 
after a power collapse and this function performs that adjustment. This 
function is called after waking up. 

@return
None.

@dependencies  
None.

@sideeffects 
This function adjusts the match value.
*/
void timer_restart(void);

/**
 Suspends the client task to get suspended for the specified time.
 If non_deferrable option is True, task will surely resumed at specified time
 If non_deferrable option is False, and if Proc goes into sleep at the time of
 specified timeout, task will resume only after Proc wakesup.

@return
None.

@dependencies  
 This function should only be called from the clients task context only.

@sideeffects
None.
*/
void timer_sleep
(
  time_timetick_type              timeout,
  timer_unit_type             unit,
  boolean                         non_deferrable
);

#ifdef __cplusplus
} /* extern "C" { */
#endif

/** @} */ /* end_addtogroup time_services */

#endif /* TIMER_H */


