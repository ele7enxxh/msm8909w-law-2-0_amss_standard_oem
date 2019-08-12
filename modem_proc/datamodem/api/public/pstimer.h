#ifndef PS_TIMERS_H
#define PS_TIMERS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              P S T I M E R . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**
   @file pstimer.h
   @brief This is the source file for managing the API timers. The implementation is
  a standard delta list.
 */

/*
INITIALIZATION AND SEQUENCING REQUIREMENTS
  ps_timer_init() MUST be called before any other timer functions are
  called.

EXTERNALIZED FUNCTIONS
  Initialization functions:
    -  ps_timer_tick_init(tick_timer_cb_type *timer_cb_ptr): Initialize
       the Tick Timer that is used to tick other API timers.

  API Functions to be called by applications using Timer:
    1) ps_timer_alloc():      Function to allocate a timer from free pool.
    2) ps_timer_start():      Start the timer using milliseconds
    3) ps_timer_start_secs(): Start the timer using seconds
    4) ps_timer_cancel():     Cancel the timer
    5) ps_timer_free() :      Free the timer
    6) ps_timer_is_running:   Returns if the timer is running or not.

  PS Task to Timer API interface functions:
    1) ps_timer_start_tick: Starts the tick timer
    2) ps_timer_cancel_tick: Cancels the tick timer.
    3) ps_timer_handler: Function to be executed every time the tick
       timer expires.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the utils group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
Copyright (c) 2000-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/pstimer.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
11/28/11    sun    To fix documentation issue 
08/28/11    SCD    (Tech Pubs) Applied new Doxygen grouping markup.
12/01/10    SCD    (Tech Pubs) Edited Doxygen markup and comments.
12/01/10    sa     Modified the comments according to doxygen specific 
                   semantics.
01/11/10    ss     Lint warning fixes.  
02/19/09    ar     Added PS_TIMER_FREE()
12/14/08    pp     Common Modem Interface: Public/Private API split.
05/10/06    mct    Now using a random cookie value for timers.
04/22/06    mct    Reworked entire PS timer library to use new timer.h API.
                   Removed all old pstimer tick functionality. Implemented
                   callback mechanism to support extended timers through
                   REX timers.
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
10/26/04    ifk    Replaced reassembly timer with fraghdl timers
10/25/04    msr    Increase number of timers per TCB from 4 to 5 to include
                   persist timer.
06/16/04    mvl    changed the timers to use an int64 rather than an int32 to
                   support IPv6 prefix lifetimes.  Also added
                   ps_timer_start_secs() macro allowing timers to be started
                   in seconds (rather than milli seconds.
04/30/04    usb    Added timers for IP reassembly, include file cleanup.
04/28/04    mct    Increased # of sockets timers to account for extra TCBs.
06/11/03    jd     Added ps_timer_remaining to check time left on timer
05/04/03    ss     Increased PS_TIMER_SOCKET_TIMERS to account for timer
                   needed to support SO_LINGER socket option
11/06/02    ifk    Included dssdns.h and incremented PS timers for DNS.
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
09/10/02    mvl    fixed number of timers used for PPP.
08/13/02    mvl    Fixed MM featurization.
08/04/02    mvl    Simplified MM featurization.
08/01/02    usb    Merged in changes for multimode ps under FEATURE_DATA_MM
07/12/02    jd     Added pvcs header tag to edit history
06/06/02    jd     Keep track of elapsed ms instead of ticks, since timer
                   might expire much later than one tick (50ms) as seen in
                   the case of dormancy/mobile ip reregistration.
03/13/02    pjb    There are 4 timers per TCB now not 3.
01/25/02    ss     Increased the maximum timers to 8 to account for the timer
                   used by DNS
07/27/01    mvl    Added 4 extra timers for LCP and IPCP: 2 for each iface
06/07/01    jd     Modified the time argument to ps_start_timer() to a sint31
                   so that longer times can be specified.
05/18/01  mvl/na   Changed the way the max timers is defined to more cleanly
                   support multiple features.
02/06/01    snn    Modified the code to use the clock services instead of
                   maintaining the timer. This ensures that the timer is
                   more accurate. (specially since PS task has very low
                   priority).
11/27/00    na     Cleanup. Moved timer_entry_type to pstimer.c
11/02/00    snn    Kept PS_TIMER_MAX in #define for FEATURE_DS_SOCKETS
                   Initialized PS_TIMER_FREE to zero. This allows us to
                   memset the timer structure.
                   Deleted unwanted defines from ps_timer_error_type
09/11/00   snn/na  created module
===========================================================================*/

/* group: utils */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

/** @addtogroup datatypes
  @{
*/

/** Timer callback function. The callback function must have the 
  following signature: @code void function(int); @endcode
*/
typedef void(timer_cb_type)(void *);
/** @} */ /* end_addtogroup datatypes */

/**
  Errors passed to the APIs. The values are 0 and -1 only. Do not add more 
  values.
*/
typedef enum
{
  PS_TIMER_FAILURE = 0,                  /**< Timer operation failed. */
  PS_TIMER_SUCCESS                 = 1,  /**< Timer operation succeeded. */
  /** @cond
  */
  PS_TIMER_ERROR_TYPE_FORCE_32_BIT = 0x7FFFFFFF /**< Internal use only. */
  /** @endcond */
} ps_timer_error_type;

/** Protocol service timer handle. */
typedef uint32 ps_timer_handle_type;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup constants
  @{
*/
/** Protocol service timer NULL handle. */
#define PS_TIMER_INVALID_HANDLE ((ps_timer_handle_type)0)
/** @} */ /* end_addtogroup constants */

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/** @addtogroup macros_utils
  @{
*/
/*===========================================================================
MACRO PS_TIMER_FREE_HANDLE()
===========================================================================*/
/**
  Frees a timer from the delta list. This function also clears the handle.

  @param[in] handle  Handle to the timer.

  @return
  The handle is reset to NULL.

  @dependencies
  None.
 */
#define PS_TIMER_FREE_HANDLE(handle)                                     \
  if (PS_TIMER_INVALID_HANDLE != handle)                                 \
  {                                                                      \
    (void) ps_timer_free( handle);                                      \
    handle = PS_TIMER_INVALID_HANDLE;                                    \
  }
/** @} */ /* end_addtogroup macros_utils */
/*===========================================================================
FUNCTION PS_TIMER_ALLOC()
===========================================================================*/
/** @ingroup ps_timer_alloc
  Allocates a timer. Before any task uses the timer, it must first acquire a 
  timer by calling alloc_timer(). This function returns a timer handle to the 
  caller and can be used in the future to access this timer until it is freed.

  Every timer must have a valid callback associated with it. The timer takes 
  the function callback as an argument. The callback function must have the 
  following signature: @code void fname(void *); @endcode

  @param[in] callback  Callback associated with the timer.
  @param[in] cb_param  Parameter to the callback.

  @return
  Value between 1 and PS_TIMER_MAX -- Timer allocated successfully. \n
  PS_TIMER_INVALID_HANDLE          -- Timer allocation failed.

  @dependencies
  After the timer is used, FREE_TIMER must be called to return the timer back 
  to the free pool.
  @newpage
*/
extern ps_timer_handle_type ps_timer_alloc
(
  void (* callback) (void *),
  void *cb_param
);

/*===========================================================================
FUNCTION PS_TIMER_START()
===========================================================================*/
/** @ingroup ps_timer_start
  Starts a timer. This function takes as arguments a timer handle, the time in 
  milliseconds for the timer, and a pointer to the error number.

  @datatypes
  #ps_timer_handle_type

  @vertspace
  @param[in] handle  Handle to the timer.
  @param[in] time    Time in milliseconds.

  @return
  PS_TIMER_FAILURE -- Error occurred while setting the timer. \n
  PS_TIMER_SUCCESS -- Timer is set successfully.

  @dependencies
  None.

  @sideeffects
  If the timer is the first timer in the list, a PS timer is started that goes 
  off every PS_TIMER_TICK_INTERVAL in duration.\n
  @vertspace
  If time_left is set to zero, the timer is stopped (i.e., removed from the 
  list).
  @newpage
*/
extern ps_timer_error_type ps_timer_start
(
  ps_timer_handle_type handle,
  int64 time
);

/** @addtogroup macros_utils
  @{
*/
/*===========================================================================
FUNCTION PS_TIMER_START_SECS()
===========================================================================*/
/** 
  Starts the PS timers.

  @param[in] handle  Handle to the timer.
  @param[in] time_s  Time in seconds for the timer.

  @return
  PS_TIMER_FAILURE -- Error occurred while setting the timer.\n
  PS_TIMER_SUCCESS -- Timer is set successfully.

  @dependencies
  None.

  @sideeffects
  If the timer is the first timer in the list, a PS timer is started that goes 
  off every PS_TIMER_TICK_INTERVAL in duration. \n
  @vertspace
  If time_left is set to zero, the timer is stopped (i.e., removed from the 
  list).
 */
#define ps_timer_start_secs( handle,  time_s )  \
  ps_timer_start( (handle), ((time_s)*1000))
/** @} */ /* end_addtogroup macros_utils */
/*===========================================================================
FUNCTION PS_TIMER_CANCEL()
===========================================================================*/
/** @ingroup ps_timer_cancel
  Cancels the timer.

  @datatypes
  #ps_timer_handle_type

  @vertspace
  @param[in] handle   Handle to the timer.

  @return
  TIMER_FAILURE    -- Error ocurred while canceling the timer.\n
  PS_TIMER_SUCCESS -- Timer is canceled successfully.

  @dependencies
  None.
  @newpage
*/
extern ps_timer_error_type ps_timer_cancel
(
  ps_timer_handle_type handle
);

/*===========================================================================
FUNCTION PS_TIMER_FREE()
===========================================================================*/
/** @ingroup ps_timer_free
  Frees a timer from the delta list.

  @datatypes
  #ps_timer_handle_type

  @vertspace
  @param[in] handle  Handle to the timer.

  @return
  PS_TIMER_FAILURE -- Timer cannot be freed.\n
  PS_TIMER_SUCCESS -- Timer freed successfully.

  @dependencies
  This function cannot free a timer twice. If the same timer is freed twice, it 
  may be acting on another timer. This function returns an error if a request is 
  received to free a timer that is already free.
  @newpage
*/
extern ps_timer_error_type ps_timer_free
(
  ps_timer_handle_type handle
);

/*===========================================================================
FUNCTION PS_TIMER_IS_RUNNING()
===========================================================================*/
/** @ingroup ps_timer_is_running
  Checks whether a timer is running.

  This function is used to check whether a timer is running before starting 
  the timer again, as required with TCP delayed acknowledgement timers.

  FALSE is returned when a non-valid handle is passed because the timer is 
  not running.

  @datatypes
  #ps_timer_handle_type

  @vertspace
  @param[in] handle  Handle to the timer.

  @return
  TRUE  -- Timer is running.\n
  FALSE -- Timer is not running.

  @dependencies
  None.
  @newpage
 */
extern boolean ps_timer_is_running
(
  ps_timer_handle_type handle
);

/*===========================================================================
FUNCTION PS_TIMER_REMAINING()
===========================================================================*/
/** @ingroup ps_timer_remaining
  Checks the time remaining on a running timer.

  @datatypes
  #ps_timer_handle_type

  @vertspace
  @param[in] handle   Handle to the timer.

  @return
  Time left in milliseconds -- Timer is running.\n 
  0                         -- Timer is stopped.\n 
  -1                        -- Timer does not exist. 

  @dependencies
  None.
  @newpage
*/
extern int64 ps_timer_remaining
(
  ps_timer_handle_type handle
);



#endif /* PS_TIMERS_H */
