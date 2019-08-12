/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
       D A T A   S E R V I C E S   A P I  T I M E R

GENERAL DESCRIPTION
  This is the source file for managing the PS timer API. The implementation
  utilizes the Platform specific timer APIs.

  PLEASE NOTE THAT THE FUNCTIONS CAN BE CALLED ONLY FROM A SINGLE TASK
  CONTEXT, CURRENTLY IT IS ONLY PS TASK CONTEXT.


  TERMINOLOGY USED:
    API Timers: The timers that are given to the application.

  STEPS FOR USING THE TIMERS:
    To use the timers do the following steps:
        1. Obtain a timer by calling ps_timer_alloc(). If the timer is
           available in the free pool, then a timer handle is returned, else
           it returns PS_TIMER_FAILURE. Pass the callback function that
           should be called on the expiry of the timer. After the timer
           expires, the callback function is called from the PS task
           context.

        2. After getting a valid timer handle, start the timer by calling
           ps_timer_start(). The time should be given in milli seconds.

        3. If you would like to cancel a particular timer, then use the
           function ps_timer_cancel(). Note that the function does not
           free it, the application still owns the handle.

        4. If you are done with using the timer, call ps_timer_free and
           it will return the timer to the free pool.

        5. If you would like to check if a particular timer is running, then
           use the function, ps_timer_is_running(). This returns TRUE if the
           timer is running.


  State Transitions of the Timer:

        (Starting State)------(function to be called)--------(Ending State)
        FREE -------------------ps_timer_alloc-------------------> STOP

        STOP -------------------ps_timer_start-------------------> RUN
                                with non-zero timer value

        RUN --------------------ps_timer_start ------------------> RUN
                                with non-zero timer value

        RUN --------------------ps_timer_cancel------------------> STOP

        STOP--------------------ps_timer_free--------------------> FREE




RELATIONSHIP BETWEEN PS TASK AND THE TIMER API:
    1. Timer Initializations:
          The API timers are initialized by calling ps_timer_init in
          the PS task.
    2. PS TASK SHOULD BE INITIALIZED BEFORE ANY APPLICATION CALLS THIS TIMER API.
          This is required as PS must also initialize the ps timer array.

RELATIONSHIP BETWEEN PS TASK AND TIMER SERVICES:
   1. The PS task registers the client's timer with REX timer services. When
      the timer expires PS task's timer handler callback will determine if
      the client's callback should be called. If it determines the time
      initially set by the client was longer than the max supported REX
      timer length it will restart another timer for the client automatically.
   2. PS task receives an Asynchronous Procedure Call (APC) from timer services
      in the PS context once the timer has expired.
   3. The maximum supported REX timer length is determined by calling the slow
      clock API. This function determines the number of slow clocks based on
      the current slow clock frequency. PS uses a value of 0xFD000000 of the
      maximum value. This prevents an overflow from occuring when the clock
      frequency changes. Drift of .1% can result in 86 secs over 24 hours if
      the mobile does not have access to network time. Using this percentage
      helps to reduce that drift.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  ps_timer_init() MUST be called before any of the other functions can
  be called and have predictable behaviour.

EXTERNALIZED FUNCTIONS
  Initialization functions:
    1) ps_timer_init(void): Initialize the api timers.

  API Functions to be called by applications using Timer:
    1) ps_timer_alloc():   Function to allocate a timer from free pool.
    2) ps_timer_start():      Start the timer using milliseconds
    3) ps_timer_cancel():     Cancel the timer
    4) ps_timer_free() :      Free the timer
    5) ps_timer_is_running:   Returns if the timer is running or not.


Copyright (c) 2000-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/pstimer.c_v   1.2   19 Feb 2003 16:17:42   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/pstimer.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/30/11    su     Correcting documentation
09/20/11    bvd    Added free floating changes to pstimer
10/18/11    ss     Reducing highly recurring F3.
03/31/11    ss     Avoid posting PS command while timer free or cancel is in
                   progress.
02/17/11    ss     Cleaning up of Globals for thread safety in Q6 Free
                   Floating environment.
02/14/11    ss     Do not crash in case the timer callback is called after
                   the timer has been freed.
02/09/10    sa     Adding two more timer states START and CANCEL to avoid
                   race condition between timer_expiry and timer_cancel.
01/12/10    hm     Seperate rex timer functionality into another module.
08/16/09    pp     Use timer critical sections for mutual exclusion instead
                   of global PS critical section.
                   Using REX_DIS[EN]ABLE_PREMPTION construct to resolve
                   race condition between timer_expiry and timer_cancel.
11/28/08    pp     Use of PS_ENTER_CRIT_SECTION/UNLOCK macros for better task(s)
                   /interrupt(s) syncronization.
10/24/08    pp     Fixed compiler warning observed in OFFTARGET build.
10/01/08    kcr    Incorrect use of REX_DISABLE_PREMPTION(). Scoping rules corrected.
07/27/06    mct    Fixed window between timer expiration and timer callback
                   execution.
06/14/06    msr    Removed critical section in ps_timer_handler_cb()
06/01/06    mct    Removed critical section around client callback.
05/10/06    mct    Now using a random cookie value for timers.
05/02/06    mct    L4 APCs occur in DPC context. Added cmd to ensure all
                   timer client callbacks occur in PS context.
04/22/06    mct    Reworked entire PS timer library to use new timer.h API.
                   Removed all old pstimer tick functionality. Implemented
                   callback mechanism to support extended timers through
                   REX timers.
02/22/06    msr    Using single critical section
01/31/06    rt     Modified TASKLOCK's.
10/21/05    rt     Added TASKLOCK's.
05/18/05    ssh    Fixed lint errors.
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
09/10/04    ks/msr added the value of ps_ms_elapsed_cntr to time_left in
                   ps_timer_start() to avoid early expiration. Moved the
                   start of the tick timer to later in the function.
06/16/04    mvl    changed the timers to use an int64 rather than an int32 to
                   support IPv6 prefix lifetimes.
04/30/04    usb    Assert if we exceed max limit on num required timers.
06/11/03    jd     Added ps_timer_remaining to check time left on timer
02/19/03    jd     Save timer # for MSG_ display in timer handler callback
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/02/02    usb    Merged in changes for multimode ps support under
                   FEATURE_DATA_MM
06/06/02    jd     Keep track of elapsed ms instead of ticks, since timer
                   might expire much later than one tick (50ms) as seen in
                   the case of dormancy/mobile ip reregistration.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
08/29/01    jd     timer handler was not calling timer callback until 1+
                   ticks after timer expired. fixed. Added rev. header field.
08/08/01    jd     modified ps_timer_handler to handle multiple timers
                   expiring on the same PS_TIMER_TICK_SIG
06/07/01    jd     Modified the time argument to ps_start_timer() to a sint31
                   so that longer times can be specified.
02/06/01    snn    Modified the code to use the clock services instead of
                   maintaining the timer. This ensures that the timer is
                   more accurate. (specially since PS task has very low
                   priority).
11/27/00   na      Minor cleanup.
11/11/00   snn     Added new callback functions for PS task interface.
                   Removed errno in the functions because it is never used.
                   Modified ps_timer_tick_init for better decoupling of PS
                     task and timer api module.
8/20/00    snn/na  Created this module.
===========================================================================*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                          INCLUDE FILES FOR MODULE

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "comdef.h"
#include "customer.h"

#include "ps_svc.h"
#include "ps_crit_sect.h"
#include "pstimer.h"
#include "pstimer_config.h"
#include "ps_platform_timer.h"
#include "ds_Utils_DebugMsg.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                        LOCAL DECLARATIONS FOR MODULE

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  This is for the wrap-around value of the cookie. Need to define locally
  to reduce dependency on outside files.
---------------------------------------------------------------------------*/
#define PS_TIMER_UINT16_MAX 0xFFFF

/*---------------------------------------------------------------------------
  Added a critical section for ps timers as there is no need to block other
  critical sections while performing timer functions.
---------------------------------------------------------------------------*/
static ps_crit_sect_type ps_timer_crit_section;

/*---------------------------------------------------------------------------
  States of the timer
  FREE:  Timer is available in the pool and can be used. This should be
         initialized to zero, because we use memset to zero initalize the
         ps_timer_list and so, FREE should equal zero.
  STOP:  Timer has been allocated but not running (stopped/never started)
  RUN:   Timer has been started.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_TIMER_FREE   = 0,
  PS_TIMER_STOP   = 1,
  PS_TIMER_RUN    = 2
} ps_timer_state_type;

/*---------------------------------------------------------------------------
  The timer structure has the following fields:
    timer_callback_fcn:
                The function pointer points to the function that will be
                executed when the timer expires.  If this is NULL, no
                callback is called when the timer expires.  It will be
                intialized to NULL during initialization and freeing the
                timer.

    timer_callback_param:
                This contains the parameter to be passed to the callback
                function.

    timer:      This is the rex timer structure that contains the timer
                allocation information, callback, time, etc.

    timer_val:  The time set by the client for the timer.

    extended_time_left:
                This is the remaining time left on the timer that could
                not originally be registered with rex timers. This field
                is only used when timers are defined that go beyond the
                capability(size) of rex timers. This value is decremented
                when subsequent timers are registered until all the time has
                expired.

    timer_state: This holds the state of the timer. If the timer is defined
                but not being used, it is in PS_TIMER_STOP state. If it is
                not yet defined, it is in PS_TIMER_FREE state. If the timer is
                started it is in PS_TIMER_RUN state. If it is cancelled
                it is in PS_TIMER_STOP state.
---------------------------------------------------------------------------*/
typedef struct time_list
{
  void                *timer;                /* Timer structure.  Defined
                                                in platform specific layer */
  int64                timer_val;            /* Time set by client in ms   */
  int64                extended_time_left;   /* For timers longer than
                                                platform specific timer API
                                                supports remaining time is
                                                stored here in ms          */
  timer_cb_type       *timer_callback_fcn;   /* callback function pointer  */
  void                *timer_callback_param; /* callback function parameter*/
  uint16               timer_cookie;         /* Random timer cookie        */
  ps_timer_state_type  timer_state;          /* contains timers state      */
  boolean              cback_pending;        /* Is there a callback pending?*/
} ps_timer_entry_type;

/*---------------------------------------------------------------------------
  This list contains all the API timers.
---------------------------------------------------------------------------*/
static ps_timer_entry_type ps_timer_list[PS_TIMER_MAX];

/*---------------------------------------------------------------------------
  Maximum supported time in ms that Platform (REX/WM) timers API supports.
---------------------------------------------------------------------------*/
static uint32 ps_const_max_timer_supported;


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         LOCAL FUNCTION DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_TIMERI_HANDLE_TO_TIMER()

DESCRIPTION
  This function converts the handle passed into the address of the timer.
  Checks if the handle passed is valid or not. If it is not valid, it returns
  FALSE; If it is valid, it returns TRUE.

DEPENDENCIES
  This should be called only after a valid timer handle is obtained by
  calling ps_timer_alloc_timer

PARAMETERS
  handle:           The assigned ps_timer handle.

RETURN VALUE

  NULL: if timer handle is not valid
  address:  If passed handle is valid.

SIDE EFFECTS
  None
===========================================================================*/
static ps_timer_entry_type *ps_timeri_handle_to_timer
(
  ps_timer_handle_type  timer_handle
)
{
  ps_timer_entry_type     *ps_timer;  /* Ptr to the ps_timer entry         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Confirm the timer handle's cookie is valid and derive the timer index.
  -------------------------------------------------------------------------*/
  if ((timer_handle & PS_TIMER_HANDLE_MASK) >= PS_TIMER_MAX)
  {
    LOG_MSG_INFO2_1("ps_timeri_handle_to_timer(): Invalid timer 0x%x",
                    timer_handle);
    return NULL;
  }

  ps_timer = (ps_timer_list + (timer_handle & PS_TIMER_HANDLE_MASK));

  /*-------------------------------------------------------------------------
    Confirm that the timer is not FREE. This confirms that a valid timer
    is being used.
  -------------------------------------------------------------------------*/
  if (ps_timer->timer_state == PS_TIMER_FREE)
  {
    LOG_MSG_INFO2_1("ps_timeri_handle_to_timer(): Timer 0x%x is freed",
                    timer_handle);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Confirm that the cookie is valid.
  -------------------------------------------------------------------------*/
  if (ps_timer->timer_cookie != ((timer_handle & 0xFFFF0000) >> 16))
  {
    LOG_MSG_INFO2_2("ps_timeri_handle_to_timer(): Invalid timer 0x%x, "
                    "cookie 0x%x mismatch",
                    timer_handle, ps_timer->timer_cookie);
    return NULL;
  }

  return ps_timer;

} /* ps_timeri_handle_to_timer */

/*===========================================================================
FUNCTION PS_TIMERI_CLIENT_CMD_CB()

DESCRIPTION
  This function is called in PS context to execute the client timer callbacks.

  If the timer was originally set for a longer timer than rex supports, start
  the timer again using the following logic:

    If if the value extended_time_left is > ps_const_max_timer_supported

    new_timer_value = ps_const_max_timer_supported

    Set new timer.

    extended_time_left = extended_time_left - ps_const_max_timer_supported

  This logic will repeat until their time has "officially" expired. The
  timer handler will then call the client's registered callback function.

PARAMETERS
  cmd:               The PS_TIMER_CALLBACK_CMD called from ps_task().
  user_data_ptr:     The assigned ps_timer handle.

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_timeri_client_cmd_cb
(
  ps_cmd_enum_type   cmd,
  void              *user_data_ptr
)
{
  ps_timer_entry_type    *ps_timer;
  timer_cb_type          *cb_fcn   = NULL;
  void                   *cb_param = NULL;
  uint32                  temp_timer_val;
  ps_timer_handle_type    timer_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(cmd == PS_TIMER_CALLBACK_CMD);

  timer_handle = (ps_timer_handle_type) user_data_ptr;

  PS_ENTER_CRIT_SECTION(&ps_timer_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Check if the handle passed is valid and obtain the pointer to the timer.
    -----------------------------------------------------------------------*/
    if ((ps_timer = ps_timeri_handle_to_timer(timer_handle)) == NULL)
    {
      LOG_MSG_ERROR_1("ps_timeri_client_cmd_cb(): Couldn't find timer 0x%x",
                      timer_handle);
      break;
    }

    if (FALSE == ps_timer->cback_pending)
    {
      LOG_MSG_INFO2_1("ps_timeri_client_cmd_cb(): Not calling callback for "
                      "timer 0x%x as timer was restarted/cancelled",
                      timer_handle);
      break;
    }

    /*-----------------------------------------------------------------------
      If the client set a long timer restart the timer with the remaining time
      and updated the time left. Otherwise if there is no time left call the
      client's callback
    -----------------------------------------------------------------------*/
    if (ps_timer->extended_time_left > 0)
    {
      if (ps_timer->extended_time_left > ps_const_max_timer_supported)
      {
        LOG_MSG_INFO1_2("ps_timeri_handler_cb(): "
                        "Timer %d, remaining time : %lld",
                        (ps_timer-ps_timer_list),
                        ps_timer->extended_time_left);

        temp_timer_val                = ps_const_max_timer_supported;
        ps_timer->extended_time_left -= ps_const_max_timer_supported;
      }
      else
      {
        temp_timer_val               = (uint32) ps_timer->extended_time_left;
        ps_timer->extended_time_left = 0;
      }

      ps_platform_timer_set( ps_timer->timer, temp_timer_val);
    }
    else
    {
      cb_fcn                 = ps_timer->timer_callback_fcn;
      cb_param               = ps_timer->timer_callback_param;
      ps_timer->cback_pending = FALSE;
    }
  } while (0);

  PS_LEAVE_CRIT_SECTION(&ps_timer_crit_section);

  if (cb_fcn != NULL )
  {
    LOG_MSG_INFO2_1("ps_timeri_client_cmd_cb(): "
                    "Timer %d expired - calling callback",
                    (ps_timer - ps_timer_list));
    cb_fcn( cb_param);
  }

  return;
} /* ps_timeri_client_cmd_cb() */


/*===========================================================================
FUNCTION PS_TIMERI_HANDLER_CB()

DESCRIPTION


DEPENDENCIES
  This occurs in INTERRUPT CONTEXT

PARAMETERS
  user_data:        The assigned ps_timer handle.

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to PS to call the client's callback if their timer has
  officially expired.
===========================================================================*/
static void ps_timeri_handler_cb
(
  void *    user_data
)
{
  ps_timer_entry_type     *ps_timer;
  ps_timer_handle_type     timer_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  timer_handle = (ps_timer_handle_type) user_data;

  PS_ENTER_CRIT_SECTION(&ps_timer_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Check if the handle passed is valid and obtain the pointer to the timer.
      We should never get an invalid handle from timer services API.
    -----------------------------------------------------------------------*/
    if ((ps_timer = ps_timeri_handle_to_timer(timer_handle)) == NULL)
    {
      LOG_MSG_ERROR_1("ps_timeri_handler_cb(): Couldn't find timer 0x%x",
                      timer_handle);
      break;
    }

    if (PS_TIMER_STOP == ps_timer->timer_state)
    {
      LOG_MSG_INFO1_1("ps_timeri_client_cmd_cb(): Ignoring callback for "
                      "timer 0x%x in PS_TIMER_STOP state", timer_handle);
      break;
    }

    /*----------------------------------------------------------------------
      Set cback_pending to TRUE to account for cmds executed on the timer
      prior to the callback executing.
    ----------------------------------------------------------------------*/
    ps_timer->cback_pending = TRUE;

    /*----------------------------------------------------------------------
      Send a cmd to PS task to process the client callbacks in PS context.
      Protected within timer critical section to avoid posting command while
      timer cancel or timer free is in progress.
    ----------------------------------------------------------------------*/
    ps_send_cmd(PS_TIMER_CALLBACK_CMD, user_data);
  } while (0);

  PS_LEAVE_CRIT_SECTION(&ps_timer_crit_section);

} /* ps_timeri_handler_cb() */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                       REGIONAL DEFINITIONS FOR MODULE

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PS_TIMER_INIT()

DESCRIPTION
  This function initializes the timer data structures.
    timer_callback_fcn:   Initialize to NULL
    timer_callback_param: Initialize to NULL
    extended_time_left    Initialize to 0;
    timer_val:            Initialize to 0;
    timer_state:          Initialize to PS_TIMER_FREE

  This function also initializes the variable ps_const_max_timer_supported to
  the max time that timers can support without significant drift.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the ps_timer_list structure contents.

===========================================================================*/
void ps_timer_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the ps_timer_crit_section
  -------------------------------------------------------------------------*/
  PS_INIT_CRIT_SECTION(&ps_timer_crit_section);

  PS_ENTER_CRIT_SECTION(&ps_timer_crit_section);

  /*-------------------------------------------------------------------------
    Initialize the timer fields to default values.

    All the pointers are initialized to NULL.
    The timer state should be made as FREE, since it can be used.
    The Time value is made as zero.
  -------------------------------------------------------------------------*/
  memset( ps_timer_list, 0, sizeof(ps_timer_list));

  ps_const_max_timer_supported = ps_platform_timer_get_max_supported_timer();

  LOG_MSG_INFO1_1("ps_timer_init(): Max timer value supported is %lld ms",
                  ps_const_max_timer_supported);

  /*-------------------------------------------------------------------------
    Set the PS handler function for PS timer expirations.
  -------------------------------------------------------------------------*/
  (void) ps_set_cmd_handler(PS_TIMER_CALLBACK_CMD, ps_timeri_client_cmd_cb);

  PS_LEAVE_CRIT_SECTION(&ps_timer_crit_section);

} /* ps_timer_init() */


/*===========================================================================
FUNCTION PS_TIMER_DEINIT()

DESCRIPTION
  This function cleansup the timer data structures.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the ps_timer_list structure contents.

===========================================================================*/
void ps_timer_deinit
(
  void
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_DESTROY_CRIT_SECTION(&ps_timer_crit_section);

}


/*===========================================================================
FUNCTION PS_TIMER_ALLOC

DESCRIPTION
  This function allocates a timer. Before any task uses the timer, it should
  first acquire a timer by calling alloc_timer. This function returns a timer
  handle to the caller. This timer handle can be used in the future to access
  this timer (until it is freed).

  Every timer may have a callback associated with it.  So, it takes the
  function callback as an argument.  The callback function must have the
  following signature: void fname(void *);  If NULL is specified as the
  callback parameter, no function is called.

DEPENDENCIES
  After the timer is used, call FREE_TIMER. This will return the timer back
  to the free pool.

PARAMETERS
  callback  :The callback function to call when the timer expires.
  cb_param  :The parameter to pass back in the client callback.

RETURN VALUE
   Success: A non-zero handle
   Failure: PS_TIMER_INVALID_HANDLE

SIDE EFFECTS
  None
===========================================================================*/
ps_timer_handle_type ps_timer_alloc
(
  void (* callback) (void *),
  void *cb_param
)
{
  /*-------------------------------------------------------------------------
    timer_handle returns the handle of the free timer.
  -------------------------------------------------------------------------*/
  ps_timer_handle_type timer_handle = PS_TIMER_INVALID_HANDLE;

  /*-------------------------------------------------------------------------
    looping index
  -------------------------------------------------------------------------*/
  uint32 loop;

  /*-------------------------------------------------------------------------
    index is used to allocate the timer handle. It stores the value of the
    of the index allocated previously. Hence, its made as static. This allows
    application to allocate different indices every time a new timer is
    requested. Even if a timer is allocated and freed alternatively, we
    ensure that different handles are returned.
  -------------------------------------------------------------------------*/
  static uint16  index  = 0;
  static uint16  cookie = 1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&ps_timer_crit_section);

  /*-------------------------------------------------------------------------
    Find out if there are timers available.

    If there are no free timers available then return PS_TIMER_FAILURE

    Note that we use loop and index to obtain the timer handle. This is
    useful because, it ensures that we always allocate different timer
    handles when more than one free timer exists.
  -------------------------------------------------------------------------*/
  for (loop = 0; loop < PS_TIMER_MAX; loop++ )
  {
    /*-----------------------------------------------------------------------
      If the index/cookie reaches its maximum, then wrap around to make it
      initial value.
    -----------------------------------------------------------------------*/
    index++;
    if (index >= PS_TIMER_MAX)
    {
      index = 0;
    }

    if (cookie == PS_TIMER_UINT16_MAX)
    {
      cookie = 1;
    }
    else
    {
      cookie++;
    }

    /*---------------------------------------------------------------------
      Find out if any of the timers are in free pool. If yes, then copy
      the index into handle of that timer and set the timer cookie.
    ---------------------------------------------------------------------*/
    if (ps_timer_list[index].timer_state == PS_TIMER_FREE)
    {
      timer_handle = (index | (cookie << 16));
      ASSERT( timer_handle != PS_TIMER_INVALID_HANDLE);

      ps_timer_list[index].timer_state          = PS_TIMER_STOP;
      ps_timer_list[index].extended_time_left   = 0;
      ps_timer_list[index].timer_callback_fcn   = callback;
      ps_timer_list[index].timer_callback_param = cb_param;
      ps_timer_list[index].timer_cookie         = cookie;

      ps_platform_timer_def( &(ps_timer_list[index].timer),
                             /*lint -e(611) */
                             (void *) ps_timeri_handler_cb,
                             (void *) timer_handle);
      break;
    }
  }

  PS_LEAVE_CRIT_SECTION(&ps_timer_crit_section);

  LOG_MSG_INFO2_1("ps_timer_alloc(): Allocated timer 0x%x", timer_handle);
  return timer_handle;

} /* ps_timer_alloc() */


/*===========================================================================
FUNCTION PS_TIMER_START()

DESCRIPTION
  This function takes a timer handle, the time in milli-seconds for the timer
  and a pointer to error number as arguments.

DEPENDENCIES
  If this function is called for the first time, then we start the timer
  tick. Since, timer tick is initialized in PS task context, the PS task
  should already be initialized before calling this function. One simple way
  to confirm this is, to check the priority of the PS task and the task
  calling this function. If PS Task priority is higher than calling task,
  then we are safe.

PARAMETERS
  handle:           The assigned ps_timer handle.
  timer_len:        The length of the timer(in ms) to set.

RETURN VALUE
  PS_TIMER_FAILURE: if there is an error setting the timer.
  PS_TIMER_SUCCESS: if the setting is success.

SIDE EFFECTS
  If the timer is the first timer in the list, then a PS timer is started
  which goes off every PS_TIMER_TICK_INTERVAL duration.
  If the the time_left is set to zero, the timer will be stopped i.e. removed
  from the list.
===========================================================================*/
ps_timer_error_type ps_timer_start
(
  ps_timer_handle_type timer_handle,
  int64                timer_len
)
{
  ps_timer_entry_type    *ps_timer;
  ps_timer_error_type     ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_2("ps_timer_start(): Timer 0x%x, timeout %lld ms",
                  timer_handle, timer_len);

  if (timer_len <= 0)
  {
    LOG_MSG_ERROR_2("ps_timer_start(): Invalid timeout %lld, Timer 0x%x",
                    timer_len, timer_handle);
    return PS_TIMER_FAILURE;
  }

  PS_ENTER_CRIT_SECTION(&ps_timer_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Check if the handle passed is valid and obtain the valid timer address.
    -----------------------------------------------------------------------*/
    if ((ps_timer = ps_timeri_handle_to_timer(timer_handle)) == NULL)
    {
      LOG_MSG_ERROR_1("ps_timer_start(): Couldn't find timer 0x%x",
                      timer_handle);
      ret_val = PS_TIMER_FAILURE;
      break;
    }

    /*-----------------------------------------------------------------------
      Ensure that if there was already a callback pending, but applications
      are restarting the same timer anyway, we cancel the pending callback.
      This can potentially happen if applications start and stop very short
      timers quickly.
    -----------------------------------------------------------------------*/
    if (ps_timer->cback_pending == TRUE)
    {
      LOG_MSG_INFO1_1("ps_timer_start(): Timer 0x%x, removing old callback that "
                      "hasn't been processed yet", timer_handle);
      ps_timer->cback_pending = FALSE;
    }

    ps_timer->timer_val = timer_len;

    if (timer_len > ps_const_max_timer_supported)
    {
      ps_timer->extended_time_left = timer_len - ps_const_max_timer_supported;
      timer_len                    = ps_const_max_timer_supported;
    }
    else
    {
      ps_timer->extended_time_left = 0;
    }

    /*-----------------------------------------------------------------------
      Set the timer
    -----------------------------------------------------------------------*/
    ps_platform_timer_set( ps_timer->timer, (uint32) timer_len );

    ps_timer->timer_state = PS_TIMER_RUN;
    ret_val               = PS_TIMER_SUCCESS;

  } while( 0);

  PS_LEAVE_CRIT_SECTION(&ps_timer_crit_section);

  return ret_val;
} /* ps_timer_start() */


/*===========================================================================
FUNCTION PS_TIMER_CANCEL()

DESCRIPTION
  Removes the API Timer from the active list of timers.

DEPENDENCIES
  None

PARAMETERS
  handle:          The assigned ps_timer handle.

RETURN VALUE
 PS_TIMER_FAILURE: if there is an error cancelling the timer.
 PS_TIMER_SUCCESS: if the cancelling is success.

SIDE EFFECTS

===========================================================================*/
ps_timer_error_type ps_timer_cancel
(
  ps_timer_handle_type timer_handle
)
{
  ps_timer_entry_type    *ps_timer;
  ps_timer_error_type     ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1("ps_timer_cancel(): Timer 0x%x", timer_handle);

  PS_ENTER_CRIT_SECTION(&ps_timer_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Check if the handle passed is valid and obtain the valid timer address.
    -----------------------------------------------------------------------*/
    if ((ps_timer = ps_timeri_handle_to_timer(timer_handle)) == NULL)
    {
      LOG_MSG_ERROR_1("ps_timer_cancel(): Couldn't find timer 0x%x",
                      timer_handle);
      ret_val = PS_TIMER_FAILURE;
      break;
    }

    /*-----------------------------------------------------------------------
      Ensure that if there was already a callback pending, but applications
      are restarting the same timer anyway, we cancel the pending callback.
      This can potentially happen if applications start and stop very short
      timers quickly.
    -----------------------------------------------------------------------*/
    if (ps_timer->cback_pending == TRUE)
    {
      LOG_MSG_INFO1_1("ps_timer_cancel(): Timer 0x%x, removing old callback "
                      "that hasn't been processed yet", timer_handle);
      ps_timer->cback_pending = FALSE;
    }

    /*-----------------------------------------------------------------------
      Cancel the timer.
    -----------------------------------------------------------------------*/
    if (ps_platform_timer_is_active(ps_timer->timer) )
    {
      ps_platform_timer_clr( ps_timer->timer );
    }

    ps_timer->timer_state = PS_TIMER_STOP;
    ret_val               = PS_TIMER_SUCCESS;

  } while (0);

  PS_LEAVE_CRIT_SECTION(&ps_timer_crit_section);

  return ret_val;
} /* ps_timer_cancel() */


/*===========================================================================
FUNCTION PS_TIMER_FREE()

DESCRIPTION
  This function frees a timer from the delta list.

DEPENDENCIES
  Note that the application cannot free a timer twice. If a same timer is
  freed twice, it may be usurping on another timer. So, this application
  returns ERROR, if a free timer is again requested to be freed.

PARAMETERS
  handle:           The assigned ps_timer handle.

RETURN VALUE
  PS_TIMER_SUCCESS: If free'd.
  PS_TIMER_FAILURE: If cannot free

SIDE EFFECTS
  None
===========================================================================*/
ps_timer_error_type ps_timer_free
(
  ps_timer_handle_type timer_handle
)
{
  ps_timer_entry_type    *ps_timer;
  ps_timer_error_type     ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("ps_timer_free(): Timer 0x%x", timer_handle);

  PS_ENTER_CRIT_SECTION(&ps_timer_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Check if the handle passed is valid and obtain the pointer to the timer.
    -----------------------------------------------------------------------*/
    if ((ps_timer = ps_timeri_handle_to_timer(timer_handle)) == NULL)
    {
      LOG_MSG_ERROR_1("ps_timer_free(): Couldn't find timer 0x%x",
                      timer_handle);
      ret_val = PS_TIMER_FAILURE;
      break;
    }

    /*-----------------------------------------------------------------------
      Ensure that if there was already a callback pending, but applications
      are restarting the same timer anyway, we cancel the pending callback.
      This can potentially happen if applications start and stop very short
      timers quickly.
    -----------------------------------------------------------------------*/
    if (ps_timer->cback_pending == TRUE)
    {
      LOG_MSG_INFO1_1("ps_timer_free(): Timer 0x%x, removing old callback "
                      "that hasn't been processed yet", timer_handle);
      ps_timer->cback_pending = FALSE;
    }

    /*-----------------------------------------------------------------------
      Free the timer.
    -----------------------------------------------------------------------*/
    ps_platform_timer_free( (void **) &ps_timer->timer );

    /*-----------------------------------------------------------------------
      Make sure this timer is not attached to anything and update its state.
    -----------------------------------------------------------------------*/
    memset(ps_timer, 0, sizeof(ps_timer_entry_type));
    ret_val = PS_TIMER_SUCCESS;

  } while (0);

  PS_LEAVE_CRIT_SECTION(&ps_timer_crit_section);

  return ret_val;
} /* ps_timer_free() */


/*===========================================================================
FUNCTION PS_TIMER_IS_RUNNING()

DESCRIPTION
  This function can be used to check if a timer is running.

  Note that, even if a non valid handle is passed, this function returns
  FALSE, because that timer is not running.
  An example of where this function can be used is, in TCP delayed ack
  timers, always check if the timer is running before starting the timer
  again.

DEPENDENCIES
  None

PARAMETERS
  handle:           The assigned ps_timer handle.

RETURN VALUE
  TRUE:  If the Timer is running
  FALSE: If the Timer is not Running

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_timer_is_running
(
  ps_timer_handle_type timer_handle
)
{
  ps_timer_entry_type *ps_timer;
  boolean              is_timer_running;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1("ps_timer_is_running(): Timer 0x%x", timer_handle);

  PS_ENTER_CRIT_SECTION(&ps_timer_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Check if the handle passed is valid and obtain the pointer to the timer.
    -----------------------------------------------------------------------*/
    if ((ps_timer = ps_timeri_handle_to_timer(timer_handle)) == NULL)
    {
      LOG_MSG_ERROR_1("ps_timer_is_running(): Couldn't find timer 0x%x",
                      timer_handle);
      is_timer_running = FALSE;
      break;
    }

    is_timer_running = ps_platform_timer_is_active(ps_timer->timer);

    LOG_MSG_INFO2_2("ps_timer_is_running(): Timer 0x%x is_running %d",
                    timer_handle, is_timer_running);
  } while (0);

  PS_LEAVE_CRIT_SECTION(&ps_timer_crit_section);
  return is_timer_running;

} /* ps_timer_is_running() */

/*===========================================================================
FUNCTION PS_TIMER_REMAINING()

DESCRIPTION
  This function is used to check the time left on a running timer.

  Accurate to 1 ps timer tick (50ms)

DEPENDENCIES
  None

PARAMETERS
  handle:           The assigned ps_timer handle.

RETURN VALUE
  If timer is running, time left (in ms)
  If timer is stopped, 0
  If timer does not exist, -1

SIDE EFFECTS
  None
===========================================================================*/
int64 ps_timer_remaining
(
  ps_timer_handle_type timer_handle
)
{
  ps_timer_entry_type *ps_timer;
  int64                time_left;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1("ps_timer_remaining(): Timer 0x%x", timer_handle);

  PS_ENTER_CRIT_SECTION(&ps_timer_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Check if the handle passed is valid and obtain the pointer to the timer.
    -----------------------------------------------------------------------*/
    if ((ps_timer = ps_timeri_handle_to_timer(timer_handle)) == NULL)
    {
      LOG_MSG_ERROR_1("ps_timer_remaining(): Couldn't find timer 0x%x",
                      timer_handle);
      time_left = -1;
      break;
    }

    /*-----------------------------------------------------------------------
      Check the timer state to determine how much time is left.
    -----------------------------------------------------------------------*/
    if (ps_platform_timer_is_active (ps_timer->timer))
    {
      time_left = ps_platform_timer_get (ps_timer->timer);

      time_left += ps_timer->extended_time_left;
    }
    else
    {
      time_left = 0;
    }
  } while (0);

  PS_LEAVE_CRIT_SECTION(&ps_timer_crit_section);

  return time_left;
} /* ps_timer_remaining() */

