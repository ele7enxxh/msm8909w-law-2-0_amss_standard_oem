/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             I P C   T I M E R   M A N A G E R   S E R V I C E
                             
GENERAL DESCRIPTION
  CCLTM timer services provide coarse timing for REX applications.
  These services are intended to provide "fast" timer services for
  potentially large numbers of timers which expire relatively
  infrequently.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/kernel/rte/tsvc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12-May-04  rwh     Added prototype for tsvc_tick()
03/10/13   et      Created module.

===========================================================================*/


/*===================================================================
   INCLUDES AND VARIABLE DEFINITIONS
===================================================================*/

/*-------------------------------------------------------------------
   Include Files
-------------------------------------------------------------------*/
#include "comdef.h"

/*-------------------------------------------------------------------
   Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
   Type Declarations
-------------------------------------------------------------------*/
typedef uint32 tsvc_timer_duration_type;
typedef uint32 tsvc_timer_handle_type;

/*-------------------------------------------------------------------
   Global Constant Declarations
-------------------------------------------------------------------*/


/*===================================================================
   FUNCTION DEFINITIONS
===================================================================*/

/*===========================================================================
FUNCTION TSVC_CANCEL_TIMER()

DESCRIPTION                                                                */
/** Cancel the timer identified by the handle.  The message is returned to
    the caller.  A timer can be only canceled by the thread that requested it.

@param handle   Handle for the timer to cancel

@return {\bf IPC message} - The message pointer that was given when the timer
                            was requested.
                            NULL if the timer was not running.

*/
/*=========================================================================*/
union ipc_msg_type* tsvc_timer_cancel
(
   tsvc_timer_handle_type timer_handle
);

/*===========================================================================
FUNCTION TSVC_CANCEL_PURGE()

DESCRIPTION                                                                */
/** Same as tsvc_timer_cancel(), but in addition this function will remove
    the timer from the incoming thread queue.

@param handle   Handle for the timer to cancel

@return {\bf IPC message} - The message pointer that was given when the timer
                            was requested.
                            NULL if the timer was not neither running nor
                            in the thread incoming queue.

*/
/*=========================================================================*/
union ipc_msg_type* tsvc_timer_purge
(
   tsvc_timer_handle_type timer_handle
);

/*===========================================================================
FUNCTION TSVC_TIMER_REQUEST()
DESCRIPTION                                                                */
/** Start a new timer for duration_ms milliseconds. Upon expiration the
    specified message is posted into the calling thread message queue.

@param duration_ms  Timer duration in milliseconds
@param msg_ptr      Message to post when the timer expires

@return {\bf A handle identifying the timer.}
*/
/*=========================================================================*/
tsvc_timer_handle_type tsvc_timer_request
(
   tsvc_timer_duration_type duration_ms,
   union ipc_msg_type**     msg_ptr
);

/*===========================================================================
FUNCTION TSVC_TIMER_IS_RUNNING()

DESCRIPTION                                                                */
/** Finds out if a given timer is running

@param handle   Handle for the timer to cancel

@return {\bf TRUE if the timer is running}
*/
/*=========================================================================*/
boolean tsvc_timer_is_running
(
   tsvc_timer_handle_type handle
);

/*===========================================================================
FUNCTION TSVC_TIMER_TIME_LEFT()

DESCRIPTION                                                                */
/** Return the time left in of a running timer

@param handle   Handle for the timer to cancel

@return {\bf The time left in milliseconds}
*/
/*=========================================================================*/
tsvc_timer_duration_type tsvc_timer_time_left
(
   tsvc_timer_handle_type handle
);

/*===================================================================
FUNCTION:  
   TSVC_INIT
DESCRIPTION:
   This is the main function of the Timer Manager thread.  It is
   responsible for receiving signals for timer pools and then 
   calling the tick function for the appropriate timer pool.
RETURN VALUE:
   None.
SIDE EFFECTS:
   None.
===================================================================*/
void tsvc_init(
   void
);         
/*===================================================================
FUNCTION:  
   TSVC_TICK
DESCRIPTION:
   This helping operation increments the tick count and determines
   if any callback functions need to be called.
RETURN VALUE:
   None.
SIDE EFFECTS:
   See Description.
===================================================================*/
void tsvc_tick(    
  rex_timer_cnt_type p_ticks 
);



