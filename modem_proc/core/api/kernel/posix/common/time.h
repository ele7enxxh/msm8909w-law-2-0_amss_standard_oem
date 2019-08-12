#ifndef _POSIX_TIME_H_
#define _POSIX_TIME_H_

/*==========================================================================
 * FILE:         time.h
 *
 * SERVICES:     POSIX Timer API interface
 *
 * DESCRIPTION:  POSIX Timer API interface based upon POSIX 1003.1-2004
 *
 *               Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *==========================================================================*/


#include <sys/types.h>

typedef int              clockid_t; /* ignored */
#define _CLOCKID_T

typedef struct timer_i * timer_t; /* handle to internal struct */
#define _TIMER_T

#if !defined(CLOCK_REALTIME)
# define CLOCK_REALTIME 0
#endif

#if !defined(CLOCK_MONOTONIC)
# define CLOCK_MONOTONIC 1
#endif


#ifndef _TIMESPEC_T
struct timespec
{
    time_t tv_sec;   /* Seconds.     */
    long   tv_nsec;  /* Nanoseconds. */
};
#define _TIMESPEC_T
#endif

struct timeval
{
    time_t tv_sec;   /* Seconds.     */
    long   tv_usec;  /* microseconds. */
};

struct itimerspec
{
    struct timespec it_interval;  /* Timer period.     */
    struct timespec it_value;     /* Timer expiration. */
};

/* have to move #include here to solve circular include problems between time.h and signal.h */
#include <signal.h>

/* Timer functions */

/** \details
 * POSIX timers can be either of two types: a one-shot type or a periodic 
 * type.
 *
 * A one-shot is an armed timer that is set to an expiration time relative 
 * to either a current time or an absolute time. The timer expires once and 
 * is disarmed. 
 *
 * A periodic timer is armed with an initial expiration time and a repetition 
 * interval. Every time the interval timer 
 * expires, the timer is reloaded with the repetition interval. The timer 
 * is then rearmed. 
 */

/** \defgroup timer POSIX Timer API */

/** \ingroup timer */
/** @{ */

/** Create a POSIX timer. 
 * Please refer to POSIX standard for details.
 * @param clockid [in] ignored in this implementation
 * @param evp     [in] if non-NULL, points to a sigevent structure. This 
 * structure, allocated by the application, defines the asynchronous 
 * notification to occur when the timer expires. If the evp argument is 
 * NULL, the effect is as if the evp argument pointed to a sigevent 
 * structure with the sigev_notify member having the value SIGEV_SIGNAL, 
 * the sigev_signo having a default signal number (SIGALRM), and the 
 * sigev_value member having the value of the timer ID.
 */
int timer_create(clockid_t clockid, struct sigevent *restrict evp,
                 timer_t *restrict timerid);

/** Delete a POSIX timer. 
 * Please refer to POSIX standard for details.
 */                 
int timer_delete(timer_t timerid);

/** Get the time remaining on a POSIX timer. 
 * Please refer to POSIX standard for details.
 */                 
int timer_gettime(timer_t timerid, struct itimerspec *value);


/** Set the time remaining on a POSIX timer. 
 * Please refer to POSIX standard for details.
 * @param flags [in] ignored in this implementation
 */                 
int timer_settime(timer_t timerid, int flags,
                  const struct itimerspec *restrict value,
                  struct itimerspec *restrict ovalue);
/** @} */

#endif /* _POSIX_TIME_H_ */
