/*============================================================================
  FILE: sns_em.c

  This file contains the Sensors Event Manager implementation

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/evmgr/src/sns_em.c#1 $  */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-02-01  mb   sns_em_cancel_timer works to all timers, not just one-shot.
  2015-05-12  dk   Moved sns_em_convert_sec_in_q16_to_localtick to sns_uimg_em
  2015-04-09  pn   Added sns_em_set_system_time_offset()
  2014-09-17  ps   Remove AVS component dependency
  2014-06-15  sc   Disable uTimer on Virtio due to platform bug
  2014-06-11  vy   Further refactoring for uImage
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

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "fixed_point.h"
#include "sns_em.h"
#include "sns_init.h"
#include "sns_memmgr.h"

#include "sns_common.h"
#include "sns_osa.h"

#include "sns_em_priv.h"

extern OS_EVENT *g_em_mutex;

/*---------------------------------------------------------------------------
* Definitions and Types
* -------------------------------------------------------------------------*/


/* -----------------------------------------------------------------------
** Static variables
** ----------------------------------------------------------------------- */
static timer_group_type sSensorTimerGroup;


/*---------------------------------------------------------------------------
* Function prototypes
* -------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
* Function definitions
* -------------------------------------------------------------------------*/

/*=====================================================================================
  FUNCTION:  sns_em_init
=====================================================================================*/
sns_err_code_e sns_em_init(void)
{

  uint8_t os_err;

  // using priority of 10 for now
  g_em_mutex = sns_os_mutex_create(10, &os_err);
  if(os_err != OS_ERR_NONE)
  {
    SNS_EM_PRINTF1(ERROR, "sns_os_mutex_create failed %d", os_err);
    return SNS_ERR_FAILED;
  }


  sns_init_done();

  return SNS_SUCCESS;
}


/*=====================================================================================
  FUNCTION:  sns_em_create_timer_obj
=====================================================================================*/
sns_err_code_e sns_em_create_timer_obj( void (*timer_cb)(void *),
                                        void *timer_cb_arg,
                                        sns_em_timer_type_e timer_category,
                                        sns_em_timer_obj_t* timer_obj_ptr )
{
  uint8_t mutex_err;
  timer_error_type err;


  sns_os_mutex_pend(g_em_mutex, 0, &mutex_err);

  sns_em_tmr_s *new_tmr = sns_heap_malloc(sizeof(sns_em_tmr_s));

  if ( new_tmr == NULL )
  {
    SNS_EM_PRINTF0(ERROR, "ATS timer create fail");
    sns_os_mutex_post(g_em_mutex);
    return SNS_ERR_NOMEM;
  }
  else
  {
    SNS_OS_MEMZERO(new_tmr, sizeof(sns_em_tmr_s));
  }

  new_tmr->tmr_type = timer_category;
  new_tmr->pTimer = &new_tmr->sTimer;
  new_tmr->pTimerGroup = &sSensorTimerGroup;

  *timer_obj_ptr = (sns_em_timer_obj_t)new_tmr;

  timer_group_set_deferrable(new_tmr->pTimerGroup, FALSE);

  err = timer_def_osal((*timer_obj_ptr)->pTimer, (*timer_obj_ptr)->pTimerGroup, TIMER_FUNC1_CB_TYPE, (timer_t1_cb_type)timer_cb, (time_osal_notify_data)timer_cb_arg);
  if( err != TE_SUCCESS )
  {
    SNS_EM_PRINTF1(ERROR, "create ats timer  failed with status %d", err);
    sns_os_mutex_post(g_em_mutex);
    return SNS_ERR_FAILED;
  }

  (*timer_obj_ptr)->bTimerDefined = 1;

  sns_os_mutex_post(g_em_mutex);
  return SNS_SUCCESS;
}

/*=====================================================================================
  FUNCTION:  sns_em_create_utimer_obj
=====================================================================================*/
sns_err_code_e sns_em_create_utimer_obj( void (*timer_cb)(void *),
                                         void *timer_cb_arg,
                                         sns_em_timer_type_e timer_category,
                                         sns_em_timer_obj_t* timer_obj_ptr )
{
#if (!defined(SNS_USES_ISLAND) || defined(VIRTIO_BRINGUP))
  return(sns_em_create_timer_obj(timer_cb,
                                 timer_cb_arg,
                                 timer_category,
                                 timer_obj_ptr));
#else
  /* Ensure that we set the boolean flag indicating that the timer is uTimer */
  utimer_error_type err;

  sns_em_tmr_s *new_tmr = SNS_OS_U_MALLOC(SNS_DBG_MOD_EM, sizeof(sns_em_tmr_s));

  if ( new_tmr == NULL )
  {
    SNS_EM_PRINTF0(ERROR, "micro timer create fail");
    return SNS_ERR_NOMEM;
  }
  else
  {
    SNS_OS_MEMZERO(new_tmr, sizeof(sns_em_tmr_s));
  }


  new_tmr->pTimer = &new_tmr->sTimer;
  new_tmr->tmr_type = timer_category;
  *timer_obj_ptr = (sns_em_timer_obj_t)new_tmr;

  err = utimer_def_osal((utimer_ptr_type)((*timer_obj_ptr)->pTimer),
                        UTIMER_FUNC1_CB_TYPE,
                        (timer_t1_cb_type)timer_cb,
                        (time_osal_notify_data)timer_cb_arg);

  if( err != UTE_SUCCESS )
  {
    SNS_EM_PRINTF1(ERROR, "create micro timer  failed with status %d", err);
    return SNS_ERR_FAILED;
  }

  (*timer_obj_ptr)->bTimerDefined = 1;
  (*timer_obj_ptr)->bTimerIsUTimer = 1;

  return SNS_SUCCESS;
#endif
}


/*=====================================================================================
  FUNCTION:  sns_em_cancel_timer
=====================================================================================*/
sns_err_code_e sns_em_cancel_timer( sns_em_timer_obj_t timer_obj )
{
  uint8_t mutex_err;

  sns_os_mutex_pend(g_em_mutex, 0, &mutex_err);

  sns_em_tmr_s* temp = timer_obj;
  timer_clr_64(temp->pTimer, T_USEC );

  sns_os_mutex_post(g_em_mutex);

  return SNS_SUCCESS;
}
/*=====================================================================================
  FUNCTION:  sns_em_register_timer64
=====================================================================================*/
sns_err_code_e sns_em_register_timer64( sns_em_timer_obj_t  timer_obj,
                                        uint64_t            delta_tick_time )
{
  uint8_t mutex_err;

  sns_em_tmr_s *temp;
  uint64_t usec;
  timer_error_type err;
  uint8_t rv = SNS_ERR_FAILED;


  sns_os_mutex_pend(g_em_mutex, 0, &mutex_err);

  if(timer_obj == NULL)
  {
    SNS_EM_PRINTF0(ERROR, "timer_obj is NULL");
    sns_os_mutex_post(g_em_mutex);
    return rv;
  }

  temp = timer_obj;

  usec = sns_em_convert_dspstick_to_usec64(delta_tick_time);

  //ATS timer's minimal duration is the same as Qurt timers'
  if (usec < QURT_TIMER_MIN_DURATION)
      usec = QURT_TIMER_MIN_DURATION;

  if(temp->bTimerDefined == 1)
  {
    if(temp->tmr_type == SNS_EM_TIMER_TYPE_ONESHOT )
    {
      err = timer_set_64(temp->pTimer, (time_timetick_type)usec, 0, T_USEC);
      if ( err != 0 )
      {
        SNS_EM_PRINTF1(ERROR, "start oneshot ats timer failed with status %d", err);
        rv = SNS_ERR_FAILED;
      }
      else
      {
        rv = (SNS_SUCCESS);
      }
    }
    else if(temp->tmr_type == SNS_EM_TIMER_TYPE_PERIODIC )
    {
      err = timer_set_64(temp->pTimer, (time_timetick_type)usec, (time_timetick_type)usec, T_USEC);
      if ( err != 0 )
      {
        SNS_EM_PRINTF1(ERROR, "start periodic timer failed with status %d", err);
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
    SNS_EM_PRINTF0(ERROR, "Timer not defined - trynig to start timer that's not defined");
  }

  sns_os_mutex_post(g_em_mutex);

  return rv;
}

/*=====================================================================================
  FUNCTION:  sns_em_register_timer
=====================================================================================*/
sns_err_code_e sns_em_register_timer( sns_em_timer_obj_t  timer_obj,
                                      uint32_t            delta_tick_time )
{
  return sns_em_register_timer64( timer_obj, (uint64_t)delta_tick_time );
}



/*=====================================================================================
  FUNCTION:  sns_em_delete_timer
=====================================================================================*/
sns_err_code_e sns_em_delete_timer_obj( sns_em_timer_obj_t timer_obj )
{
  uint8_t mutex_err;
  sns_em_tmr_s *temp;
  uint8_t rv;

  sns_os_mutex_pend(g_em_mutex, 0, &mutex_err);

  if(timer_obj != NULL)
  {
    temp = timer_obj;
    temp->bTimerDefined = 0;
    if(timer_undef(temp->pTimer) != TE_SUCCESS)
    {
      SNS_EM_PRINTF0(ERROR, "Timer_undef failed");
    }
    sns_heap_free(temp);

    rv = SNS_SUCCESS;
  }
  else
  {
    rv = SNS_ERR_FAILED;
  }

  sns_os_mutex_post(g_em_mutex);

  return rv;
}

/*=====================================================================================
  FUNCTION:  sns_em_delete_utimer
=====================================================================================*/
sns_err_code_e sns_em_delete_utimer_obj( sns_em_timer_obj_t timer_obj )
{
#if (!defined(SNS_USES_ISLAND) || defined(VIRTIO_BRINGUP))
  return(sns_em_delete_timer_obj(timer_obj));
#else
  sns_em_tmr_s *temp;

  uint8_t rv;

  if(timer_obj != NULL)
  {
    temp = timer_obj;
    temp->bTimerDefined = 0;
    if(utimer_undef(temp->pTimer) != UTE_SUCCESS)
    {
      SNS_EM_PRINTF0(ERROR, "Timer_undef failed");
    }
    SNS_OS_U_FREE(temp);

    rv = SNS_SUCCESS;
  }
  else
  {
    rv = SNS_ERR_FAILED;
  }

  return rv;

#endif
}

/*=====================================================================================
  FUNCTION:  sns_em_cancel_all_timers
=====================================================================================*/
sns_err_code_e sns_em_cancel_all_timers ( void )
{

  return (SNS_ERR_FAILED);

}


/*=====================================================================================
  FUNCTION:  sns_em_check_timer_pending
=====================================================================================*/
uint32_t sns_em_check_timer_pending( void )
{
  return 0;
}

/*=========================================================================
  FUNCTION:  sns_em_timer_get_remaining_time
  =========================================================================*/
/*!
  @brief Gets the timer remaining duration in microseconds

  @param[i] timer: timer
  @param[o] remaining: remaining duration in microseconds

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_em_timer_get_remaining_time(
   const sns_em_timer_obj_t timer, uint32_t *remaining_time)
{
   uint8_t mutex_err;
   sns_em_tmr_s *temp;

   sns_os_mutex_pend(g_em_mutex, 0, &mutex_err);

   if(timer != NULL)
   {
     temp = timer;
     *remaining_time = timer_get_64( temp->pTimer, T_USEC );
   }

   sns_os_mutex_post(g_em_mutex);

   return SNS_SUCCESS;

}

/*=====================================================================================
  FUNCTION:  sns_em_sleep
=====================================================================================*/
void sns_em_sleep( uint32_t time_us )
{
  timer_sleep(time_us, T_USEC, TRUE);
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_localtick_to_usec
=====================================================================================*/
uint32_t sns_em_convert_localtick_to_usec( uint32_t dsps_tick )
{
  return (sns_em_convert_dspstick_to_usec(dsps_tick));
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_localtick_to_usec64
=====================================================================================*/
uint64_t sns_em_convert_localtick_to_usec64( uint64_t dsps_tick )
{
  return (sns_em_convert_dspstick_to_usec64(dsps_tick));
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_dspstick_to_appstime
=====================================================================================*/
uint32_t sns_em_convert_dspstick_to_appstime( uint32_t dsps_tick )
{
  return 0;
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_dspstick_to_gpstime
=====================================================================================*/
uint32_t sns_em_convert_dspstick_to_gpstime( uint32_t dsps_tick )
{
  return 0;
}

/*=====================================================================================
  FUNCTION:  sns_em_set_system_time_offset
=====================================================================================*/
void sns_em_set_system_time_offset( uint64_t offset )
{
  sns_em_system_time_offset = offset;
}

