/*============================================================================
  FILE: sns_em.c

  This file contains the Sensors Event Manager implementation

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/evmgr/src/sns_uimg_em.c#1 $  */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-02-01  mb   sns_em_cancel_utimer works to all timers, not just one-shot.
  2015-07-09  ps   Fix compiler warnings
  2015-05-12  dk   Moved sns_em_convert_sec_in_q16_to_localtick to sns_uimg_em
  2015-04-09  pn   Added sns_em_get_system_time()
  2015-01-13  hw   Fix a bug in getting 64 bits timestamp function
  2014-06-15  sc   Disable uTimer on Virtio due to platform bug
  2014-06-11   vy  Further refactoring for uImage
  2014-05-24  jhh  Add featurization for micro timer
  2014-03-11  jhh  Add NULL pointer check before using it.
  2014-01-23  jhh  Use ATS timer instead of qurt timer
  2013-10-03  sc   Use qurt_timer_creaet_sig2() that is required for the new
                   qurt_elite timer structure.
  2013-05-07  sc   Fix klocwork errors; check for NULL pointers.
  2013-02-10  sc   - Round up when converting usec to tick
                   - Ensure duration is in allowable range when using qurt timer
  2013-01-10  sd   return SNS_ERR_FAILED if register timer fails
  2012-12-14  pn   Implemented sns_em_cancel_timer()
  2012-12-14  jhh  Add new function qurt_elite_non_deferrable_timer_periodic_start to
                   support non-deferrable periodic timer
  2012-11-15  ps   Change heap ID from QURT_ELITE_HEAP_DEFAULT to sns_heap_id
  2012-11-02  vh   Change the task create to sns_os_task_create_ext for SNS_DSPS_BUILD
  2012-10-19  ag   Remove debug messages
  2012-10-03  sc   Scale timestamp back down to 32768Hz tick.
  2012-10-02  jhh  Add new funciton qurt_elite_timer_create_nondeferrable to use
                   non-deferrable timer until qurt_elite support is available
  2012-09-12  sc   Fix sns_em_convert_sec_in_q16_to_localtick to get right tick
  2012-08-30  sc   Use mutex to protect g_em_channel when signal is added to it
  2012-08-26  sc   Clear common signal after the EM thread receives it
  2012-08-23  ag   Fix timestamps; handle 19.2MHz QTimer
  2012-07-02  sc   Added a common signal to update the timer signals to wait for;
                   this approach eliminates polling within EM thread
                   Reduced EM thread stack size
  2012-01-16  sd   Time Stamp change for DSPS using MPM time tick
  2011-11-14  jhh  Updated SNS_OS_MALLOC to meet new API
  2011-04-27  br   Disabled the long periodic timer
  2011-02-28  br   chaned SNS_OS_TIME_US when SNS_DSPS_PROFILE_ON is defined
  2011-02-23  br   Calling uCOS timer hook when SNS_DSPS_PROFILE_ON is defined
  2010-12-21  hm   Backing out the timer workaround
  2010-12-15  hm   Workaround for triggering the timer interrupt if it has expired
  2010-12-03  pg   Added __SNS_MODULE__ to be used by OI mem mgr in debug builds.
  2010-11-30  ad   Added function to convert time in seconds in Q16 format
                   to time in local ticks
  2010-11-16  pg   Workaround for periodic timer not working after unregister
                   and re-register.
  2010-09-14  hm   Implemented get next pending timer and cancel all timers
  2010-08-31  jtl  Calling sns_init_done() when init is done.
  2010-08-24  hm   Added support for periodic and one-shot timers
  2010-08-13  hm   Initial version

============================================================================*/

#define __SNS_MODULE__ SNS_EM

// #define USES_ISLAND

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "sns_em.h"
#include "sns_common.h"
#include "sns_osa.h"

#include "sns_em_priv.h"

OS_EVENT *g_em_mutex;

/*---------------------------------------------------------------------------
* Definitions and Types
* -------------------------------------------------------------------------*/


/* -----------------------------------------------------------------------
** Static variables
** ----------------------------------------------------------------------- */
uint64_t sns_em_system_time_offset = 0;


/*---------------------------------------------------------------------------
* Function prototypes
* -------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
* Function definitions
* -------------------------------------------------------------------------*/

/*=====================================================================================
  FUNCTION:  sns_em_cancel_utimer
=====================================================================================*/
sns_err_code_e sns_em_cancel_utimer( sns_em_timer_obj_t timer_obj )
{
#if (!defined(SNS_USES_ISLAND) || defined(VIRTIO_BRINGUP))
  return(sns_em_cancel_timer(timer_obj));
#else
  sns_em_tmr_s* temp = timer_obj;
  utimer_clr_64(temp->pTimer, UT_USEC );
  return(SNS_SUCCESS);
#endif
}

/*=====================================================================================
  FUNCTION:  sns_em_register_utimer
=====================================================================================*/
sns_err_code_e sns_em_register_utimer64( sns_em_timer_obj_t  timer_obj,
                                         uint64_t            delta_tick_time )
{
#if (!defined(SNS_USES_ISLAND) || defined(VIRTIO_BRINGUP))
  return(sns_em_register_timer(timer_obj, delta_tick_time));
#else
  sns_em_tmr_s *temp;
  uint32_t usec;
  utimer_error_type err;
  uint8_t rv = SNS_ERR_FAILED;

  if(timer_obj == NULL)
  {
    SNS_EM_UPRINTF0(ERROR, "timer_obj is NULL");
    return rv;
  }

  temp = timer_obj;

  usec = sns_em_convert_dspstick_to_usec(delta_tick_time);

  //ATS timer's minimal duration is the same as Qurt timers'
  if (usec < QURT_TIMER_MIN_DURATION)
      usec = QURT_TIMER_MIN_DURATION;

  if(temp->bTimerDefined == 1)
  {
    if(temp->tmr_type == SNS_EM_TIMER_TYPE_ONESHOT )
    {
      err = utimer_set_64(temp->pTimer, (time_timetick_type)usec, 0, UT_USEC);
      if ( err != 0 )
      {
        SNS_EM_UPRINTF1(ERROR, "start oneshot ats timer failed with status %d", err);
        rv = SNS_ERR_FAILED;
      }
      else
      {
        rv = (SNS_SUCCESS);
      }
    }
    else if(temp->tmr_type == SNS_EM_TIMER_TYPE_PERIODIC )
    {
      err = utimer_set_64(temp->pTimer, (time_timetick_type)usec, (time_timetick_type)usec, UT_USEC);
      if ( err != 0 )
      {
        SNS_EM_UPRINTF1(ERROR, "start periodic timer failed with status %d", err);
        rv = SNS_ERR_FAILED;
      }
      else
      {
        rv = (SNS_SUCCESS);
      }
    }
  }
  else
  {
    SNS_EM_UPRINTF0(ERROR, "Timer not defined - trying to start timer that's not defined");
  }

  return rv;

#endif
}

/*=====================================================================================
  FUNCTION:  sns_em_register_utimer
=====================================================================================*/
sns_err_code_e sns_em_register_utimer( sns_em_timer_obj_t  timer_obj,
                                       uint32_t            delta_tick_time )
{
  return sns_em_register_utimer64( timer_obj, (uint64_t)delta_tick_time );
}


/*=====================================================================================
  FUNCTION:  sns_em_get_system_time
=====================================================================================*/
uint64_t sns_em_get_system_time( void )
{
#if (!defined(SNS_USES_ISLAND) || defined(VIRTIO_BRINGUP))
  uint64_t raw_tick = qurt_sysclock_get_hw_ticks();
#else
  uint64_t raw_tick = uTimetick_Get();
#endif
  /* convert 19.2MHz tick to 32768Hz */
  return (((raw_tick << 4) / 9375) + sns_em_system_time_offset);
}


/*=====================================================================================
  FUNCTION:  sns_em_get_timestamp
=====================================================================================*/
uint32_t sns_em_get_timestamp( void )
{
  return ((uint32_t)sns_em_get_system_time());
}


/*=====================================================================================
  FUNCTION:  sns_em_get_timestamp64
=====================================================================================*/
sns_err_code_e
sns_em_get_timestamp64( uint64_t *timestamp_p )
{
  sns_err_code_e result = SNS_ERR_BAD_PARM;
  if ( timestamp_p != NULL )
  {
    *timestamp_p = sns_em_get_system_time();
    result = SNS_SUCCESS;
  }
  return result;
}


/*=====================================================================================
  FUNCTION:  sns_em_convert_usec_to_localtick
=====================================================================================*/
uint32_t sns_em_convert_usec_to_localtick( uint32_t usec )
{
  return (sns_em_convert_usec_to_dspstick(usec) );
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_usec_to_dspstick64
=====================================================================================*/
uint64_t sns_em_convert_usec_to_dspstick64( uint64_t usec )
{
  int64_t dspstick;
  /* round up the result */
  dspstick = (usec * DSPS_SLEEP_CLK + USEC_PER_SEC - 1) / USEC_PER_SEC;
  return dspstick;
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_dspstick_to_usec64
=====================================================================================*/
uint64_t sns_em_convert_dspstick_to_usec64( uint64_t dsps_tick )
{
  int64_t usec;
  usec = (dsps_tick * USEC_PER_SEC) / DSPS_SLEEP_CLK;
  return usec;
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_usec_to_localtick64
=====================================================================================*/
uint64_t sns_em_convert_usec_to_localtick64( uint64_t usec )
{
  return (sns_em_convert_usec_to_dspstick64(usec) );
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_usec_to_dspstick
=====================================================================================*/
uint32_t sns_em_convert_usec_to_dspstick( uint32_t usec )
{
  int64_t dspstick;
  /* round up the result */
  dspstick = ((int64_t)usec * DSPS_SLEEP_CLK + USEC_PER_SEC - 1)/USEC_PER_SEC;
  return (uint32_t)dspstick;
}
/*=====================================================================================
  FUNCTION:  sns_em_convert_dspstick_to_usec
=====================================================================================*/
uint32_t sns_em_convert_dspstick_to_usec( uint32_t dsps_tick )
{
  int64_t usec;
  usec = ((int64_t)dsps_tick * USEC_PER_SEC)/DSPS_SLEEP_CLK;
  return (uint32_t)usec;
}
/*=====================================================================================
  FUNCTION:  sns_em_is_utimer
=====================================================================================*/
bool sns_em_is_utimer( const sns_em_timer_obj_t timer )
{
#if (!defined(SNS_USES_ISLAND) || defined(VIRTIO_BRINGUP))
  return(FALSE);
#else
  return(timer->bTimerIsUTimer);
#endif
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_sec_in_q16_to_localtick
=====================================================================================*/
uint32_t sns_em_convert_sec_in_q16_to_localtick( uint32_t time_sec_q16 )
{
  uint32_t repPerioduSec =
    (uint32_t)(FX_CONV(((uint64_t)time_sec_q16 * 1000000), FX_QFACTOR, 0));

  return (sns_em_convert_usec_to_dspstick(repPerioduSec));
}


