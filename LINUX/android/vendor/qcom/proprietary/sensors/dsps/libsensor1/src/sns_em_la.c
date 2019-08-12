/*============================================================================
  @file sns_em_la.c

  @brief
  This implements the Event Manager (EM) for the Linux Android platform.
  It manages timers, and converting time units between apps/dsps ticks and
  local apps time.

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

/*============================================================================

  INCLUDE FILES

  ============================================================================*/
#include "fixed_point.h"
#include "sns_common.h"
#include "sns_em.h"
#include "sns_memmgr.h"
#include "sns_osa.h"

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

/*============================================================================
  Preprocessor Definitions and Constants
  ============================================================================*/

#define SNS_TS_CLOCK CLOCK_REALTIME
#define SNS_TIMER_CLOCK CLOCK_MONOTONIC

#define USEC_PER_SEC 1000000L
#define NSEC_PER_SEC 1000000000L

#define SEC_TO_USEC(sec)   ((sec)*USEC_PER_SEC)
#define NSEC_TO_USEC(nsec) ((nsec)/1000L)

#define USEC_TO_NSEC(usec) ((usec)*1000L)
#define USEC_TO_SEC(usec)  ((usec)/USEC_PER_SEC)

#define DSPS_HZ 32768LL

/*============================================================================
  Type Declarations
  ============================================================================*/

/*============================================================================
  Static Function Definitions and Documentation
  ============================================================================*/

/*===========================================================================

  FUNCTION:   add_usec_to_ts

  ===========================================================================*/
/*
  @brief Adds microsecnd count to a timespec

  @param[i] t1_ptr: pointer to timer1
  @param[i] usec: Number of microseconds to add to t1

  @return
  Normalized t1 + usec

  @sideeffects
  none
*/
/*=========================================================================*/
static struct timespec add_usec_to_ts( const struct timespec *t1_ptr,
                                       uint32_t usec )
{
  struct timespec rv;

  rv.tv_sec = t1_ptr->tv_sec;

  /* Compute number of seconds in the usec value provided
   * to avoid rollover issues
   */
  rv.tv_sec += usec/USEC_PER_SEC;
  usec = usec % USEC_PER_SEC;
  rv.tv_nsec = t1_ptr->tv_nsec + USEC_TO_NSEC(usec);

  /* Check if the nano seconds component is greater than 1 second
   * If it is more than a second then the setitimer functions will
   * fail
   */
  if (rv.tv_nsec > (NSEC_PER_SEC-1))
  {
    rv.tv_nsec = rv.tv_nsec - 1000000000;
    rv.tv_sec = rv.tv_sec + 1;
  }
  return rv;
}

/*============================================================================
  Externalized Function Definitions
  ===========================================================================*/

/*===========================================================================

  FUNCTION:   sns_em_get_timestamp

  ===========================================================================*/
uint32_t sns_em_get_timestamp(void)
{
  struct timespec ts;

  if( 0 == clock_gettime( SNS_TS_CLOCK, &ts ) ) {
    return( SEC_TO_USEC((uint32_t)ts.tv_sec) + NSEC_TO_USEC(ts.tv_nsec) );
  } else {
    return 0;
  }
}

/*===========================================================================

  FUNCTION:   sns_em_get_timestamp64

  ===========================================================================*/
sns_err_code_e
sns_em_get_timestamp64( uint64_t* timestamp_p )
{
  struct timespec ts;
  uint64_t time;

  if( NULL == timestamp_p || 0 != clock_gettime( SNS_TS_CLOCK, &ts ) ) {
    return SNS_ERR_FAILED;
  }

  time = ( SEC_TO_USEC((uint64_t)ts.tv_sec) +
           NSEC_TO_USEC((uint64_t)ts.tv_nsec) );

  SNS_OS_MEMCOPY( timestamp_p, &time, sizeof(uint64_t) );

  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_em_convert_dspstick_to_usec

  ===========================================================================*/
uint32_t sns_em_convert_dspstick_to_usec (uint32_t dsps_tick)
{
  int64_t rv; // return value

  rv = ((int64_t)dsps_tick * USEC_PER_SEC)/DSPS_HZ;

  return (uint32_t)rv;
}

/*===========================================================================

  FUNCTION:   sns_em_convert_usec_to_dspstick

  ===========================================================================*/
uint32_t sns_em_convert_usec_to_dspstick (uint32_t usec_time)
{
  int64_t rv; // Return value

  rv = ((int64_t)usec_time * DSPS_HZ)/USEC_PER_SEC;

  return (uint32_t)rv;
}

/*===========================================================================

  FUNCTION:   sns_em_convert_dspstick_to_appstime

  ===========================================================================*/
uint32_t sns_em_convert_dspstick_to_appstime (uint32_t dsps_tick);

/*===========================================================================

  FUNCTION:   sns_em_convert_localtick_to_usec

  ===========================================================================*/
uint32_t sns_em_convert_localtick_to_usec( uint32_t local_tick )
{
  return local_tick;
}

/*===========================================================================

  FUNCTION:   sns_em_convert_usec_to_localtick

  ===========================================================================*/
uint32_t sns_em_convert_usec_to_localtick( uint32_t usec_time )
{
  return usec_time;
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_sec_in_q16_to_localtick
=====================================================================================*/
uint32_t sns_em_convert_sec_in_q16_to_localtick( uint32_t time_sec_q16 )
{
  static const uint64_t usecPerSecQ16 = FX_CONV_Q16(((uint64_t)USEC_PER_SEC), 0);
  uint64_t repPeriodQ32 = (uint64_t)time_sec_q16 * usecPerSecQ16;
  uint32_t repPerioduSec =
    (uint32_t)(FX_CONV(repPeriodQ32, (FX_QFACTOR + FX_QFACTOR), 0));

  return repPerioduSec;
}
