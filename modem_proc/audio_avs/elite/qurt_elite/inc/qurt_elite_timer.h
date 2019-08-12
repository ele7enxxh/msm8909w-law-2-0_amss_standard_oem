/*========================================================================
Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */
/**
@file qurt_elite_timer.h

@brief This file contains utilities for timers. One-shot, period and sleep
timers are provided.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_timer.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo     (Tech Pubs) Edited doxygen comments and markup.
03/08/10   brs     Edited for doxygen-to-latex process.
02/04/10   mwc     Created file.
========================================================================== */

#ifndef QURT_ELITE_TIMER_H
#define QURT_ELITE_TIMER_H

#include "qurt_elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

/** @addtogroup qurt_elite_timer
@{ */

/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/
#define ATS_TIMER_MAX_DURATION qurt_sysclock_max_duration()

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/****************************************************************************
** Timers
*****************************************************************************/

/** Timer types.
*/
typedef enum {
    QURT_ELITE_TIMER_ONESHOT_DURATION=0, /**< Duration-based one-shot timer. */
    QURT_ELITE_TIMER_ONESHOT_ABSOLUTE,   /**< Absolute one-shot timer. */
    QURT_ELITE_TIMER_PERIODIC            /**< Periodic timer. */
} qurt_elite_timer_duration_t;

/** Timer clock sources.
*/
typedef enum {
    QURT_ELITE_TIMER_USER      /**< User general purpose timer. */
} qurt_elite_timer_src_t;

/** Timer expiration handlers.
*/
typedef enum {
    QURT_ELITE_TIMER_NONE = 0,    /**< No handler; just start the timer. */
    QURT_ELITE_TIMER_SIGNAL,      /**< Signal generated to the calling thread. */
    QURT_ELITE_TIMER_CALLBACK     /**< Callback to a specified function. Not
                                       implemented.*/
} qurt_elite_timer_hdlr_t;

/** QuRT_Elite timer structure.
*/
typedef struct {
    unsigned int qTimerObj;             /**< Timer object. */ 
    unsigned int uTimerType;            /**< Timer type; one of
                                             qurt_elite_timer_duration_t. */ 
    unsigned int      timer_sigmask;    /**< Signal mask of the timer signal to
                                             be set when the timer expires. */ 
    qurt_elite_channel_t*     pChannel; /**< Pointer to the channel upon which
                                             to wait for the timer signal.   */ 
    boolean  istimerCreated;            /**< Flag that indicates whether the
                                             timer is created. */
    uint64_t duration;                  /**< Duration of timer in usec.
                                            "duration" for duration/periodic timer,
                                            "Expiry - start" for expiry timer. */
    boolean is_deferrable_group;        /**< whether timer is
                                             deferrable/non-deferrable */
}qurt_elite_timer_t;

/** QuRT_Elite timer attributes structure.
*   Set by client and passed into qurt_elite_timer_create/create2
*/
typedef struct {
    boolean is_deferrable_group;
} qurt_elite_timer_attr_t;

/**
  Deletes an existing timer.

  @datatypes
  qurt_elite_timer_t

  @param[in] pTimer   Pointer to the timer to be deleted.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer object must have been created using qurt_elite_timer_create().
  @newpage
*/
int qurt_elite_timer_delete(qurt_elite_timer_t *pTimer);

/**
  Gets the duration of the specified timer. This function is currently
  not implemented.

  @datatypes
  qurt_elite_timer_t

  @param[in] pTimer   Pointer to the timer to be queried.

  @return
  Duration of the timer, in microseconds.

  @dependencies
  The timer object must have been created using qurt_elite_timer_create().
  @newpage
*/
unsigned long long qurt_elite_timer_get_duration(qurt_elite_timer_t *pTimer);

/**
  Creates a timer. The caller must allocate the memory for the timer strucure
  and pass the pointer to this function.

  After this function, call the appropriate start functions based on
  the type of timer.

  This function creates timer in default deferrable timer group

  @datatypes
  qurt_elite_timer_t \n
  #qurt_elite_timer_duration_t \n
  #qurt_elite_timer_src_t \n
  qurt_elite_signal_t

  @param[in] pTimer      Pointer to the QuRT_Elite timer object.
  @param[in] timerType   One of the following:
                          - #QURT_ELITE_TIMER_ONESHOT_DURATION
                          - #QURT_ELITE_TIMER_PERIODIC
                          - #QURT_ELITE_TIMER_ONESHOT_ABSOLUTE @tablebulletend
  @param[in] clockSource  One of the following:
                          \n
                          @note1 Only QURT_ELITE_TIMER_USER is currently
                                 implemented.
                          - #QURT_ELITE_TIMER_USER
                          - #QURT_ELITE_TIMER_AVS (even when not used) @tablebulletend
  @param[in] pSignal      Pointer to the signal to be generated when the timer
                          expires.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  This function must be called before arming the timer. @newpage
*/
int qurt_elite_timer_create(qurt_elite_timer_t *pTimer, qurt_elite_timer_duration_t timerType, qurt_elite_timer_src_t clockSource,
                   qurt_elite_signal_t *pSignal);

/**
  Creates a timer. The caller must allocate the memory for the timer strucure
  and pass the pointer to this function.

  After this function, call the appropriate start functions based on
  the type of timer.

  This function creates a timer with attributes pTimerAttr passed in by
  caller.

  @datatypes
  qurt_elite_timer_t \n
  #qurt_elite_timer_duration_t \n
  #qurt_elite_timer_src_t \n
  qurt_elite_signal_t

  @param[in] pTimer      Pointer to the QuRT_Elite timer object.
  @param[in] timerType   One of the following:
                          - #QURT_ELITE_TIMER_ONESHOT_DURATION
                          - #QURT_ELITE_TIMER_PERIODIC
                          - #QURT_ELITE_TIMER_ONESHOT_ABSOLUTE @tablebulletend
  @param[in] pTimerAttr : timer attributes struct set by clients
  @param[in] clockSource  One of the following:
                          \n
                          @note1 Only QURT_ELITE_TIMER_USER is currently
                                 implemented.
                          - #QURT_ELITE_TIMER_USER
                          - #QURT_ELITE_TIMER_AVS (even when not used) @tablebulletend
  @param[in] pSignal      Pointer to the signal to be generated when the timer
                          expires.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  This function must be called before arming the timer. @newpage
*/
int qurt_elite_timer_create2(qurt_elite_timer_t *pTimer, qurt_elite_timer_duration_t timerType,
        qurt_elite_timer_attr_t *pTimerAttr, qurt_elite_timer_src_t clockSource, qurt_elite_signal_t *pSignal);

/**
  Creates a synchronous sleep timer. Control returns to the callee
  after the timer expires.

  @param[in] llMicrosec  Duration the timer will sleep (@ge 100 microseconds).
                         If this parameter is less than 100 microseconds, it
                         rounds up to 100 microseconds.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The object must have been created and initializedd before calling this
  function.
*/
int qurt_elite_timer_sleep(int64_t llMicrosec);

/**
  Gets the wall clock time.

  @return
  Returns the wall clock time in microseconds.

  @dependencies
  The object must have been created and initialized before calling this
  function.
*/
uint64_t qurt_elite_timer_get_time(void);

/**
  Gets the wall clock time in milliseconds.

  @return
  Returns the wall clock time in milliseconds.

  @dependencies
  The object must have been created and initialized before calling this
  function. @newpage
*/
uint64_t qurt_elite_timer_get_time_in_msec(void);


/**
  Restarts the absolute one-shot timer.

  @datatypes
  qurt_elite_timer_t
 
  @param[in] pTimer  Pointer to the QuRT_Elite timer object.
  @param[in] time    Absolute time of the timer.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must have been created using qurt_elite_timer_create().
*/
int qurt_elite_timer_oneshot_start_absolute(qurt_elite_timer_t *pTimer, int64_t time);


/**
  Restarts the duration-based one-shot timer.

  @datatypes
  qurt_elite_timer_t

  @param[in] pTimer    Pointer to the QuRT_Elite timer object.
  @param[in] duration  Duration of the timer, in microseconds.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must have been created using qurt_elite_timer_create(). @newpage
*/
int qurt_elite_timer_oneshot_start_duration(qurt_elite_timer_t *pTimer, int64_t duration);


/**
  Starts the periodic timer.

  @datatypes
  qurt_elite_timer_t

  @param[in] pTimer    Pointer to the QuRT_Elite timer object.
  @param[in] duration  Duration of the timer, in microseconds.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must have been created using qurt_elite_timer_create().
*/
int qurt_elite_timer_periodic_start(qurt_elite_timer_t *pTimer, int64_t duration);


/**
  Cancels the duration-based one-shot timer.

  @datatypes
  qurt_elite_timer_t

  @param[in] pTimer   Pointer to the QuRT_Elite timer object.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must have been created using qurt_elite_timer_create(). @newpage
*/
int qurt_elite_timer_oneshot_cancel(qurt_elite_timer_t *pTimer);

/** @} */ /* end_addtogroup qurt_elite_timer */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_TIMER_H

