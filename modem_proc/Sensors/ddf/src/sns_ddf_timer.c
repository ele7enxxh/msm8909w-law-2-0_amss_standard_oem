/**
 * @file sns_ddf_timer.c 
 *  
 * @brief Provides timer service to drivers.
 *
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/ddf/src/sns_ddf_timer.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-11-03 pn   Refactored from sns_ddf_util.c and sns_ddf_uimg_util.c
==============================================================================*/

#include "sns_ddf_util.h"
#include "sns_ddf_util_priv.h"
#include "sns_smgr_ddf_if.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SNS_DDF_TIMER_MAX_CNT  (SNS_SMGR_NUM_SENSORS_DEFINED+1)

//#define SNS_DDF_TIMER_DEBUG /* uncomment to enable debug code */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct sns_ddf_timer_obj_s
{
  sns_ddf_driver_if_s*  dd_interface; /**< Functions of the calling driver. */
  sns_ddf_handle_t      dd_handle;    /**< Instance of the calling driver. */
  void*                 arg;          /**< Argument for the handler fn. */
  sns_em_timer_obj_t    em_timer;     /**< Timer service object. */
  sns_ddf_time_t        sched;        /**< scheduled to expired */
  sns_ddf_time_t        expired;      /**< actual expiration */
  sns_ddf_time_t        dispatched;   /**< time when dispatched to driver */
  uint8_t               q_idx;        /**< index into timer_q */
  bool                  is_periodic;
} sns_ddf_timer_obj_s;

typedef struct 
{
  unsigned int          pending_flags;
  sns_ddf_timer_obj_s*  timer_ptr[SNS_DDF_TIMER_MAX_CNT];
} sns_ddf_timer_queue_s;

/*----------------------------------------------------------------------------
 *  Local Variables
 * -------------------------------------------------------------------------*/
SNS_DDF_UIMAGE_DATA static sns_ddf_timer_queue_s sns_ddf_timer_q = {
  .pending_flags = false, 
  .timer_ptr = {0} };

/*----------------------------------------------------------------------------
 *  Private/Helper Functions
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:   sns_ddf_timer_obj_match

===========================================================================*/
/*!
  @brief Finds the timer queue entry for the given timer

  @param[i]  timer - the timer handle passed in from device driver
 
  @return   the matching timer if found in queue
 */
/*=========================================================================*/
SNS_DDF_UIMAGE_CODE sns_ddf_timer_obj_s* sns_ddf_timer_obj_match(sns_ddf_timer_s timer)
{
  sns_ddf_timer_obj_s* timer_ptr = (sns_ddf_timer_obj_s*)timer;
  if ( timer_ptr == NULL )
  {
    timer_ptr = NULL;
  }
  else if ( timer_ptr->q_idx >= ARR_SIZE(sns_ddf_timer_q.timer_ptr) || 
            timer_ptr != sns_ddf_timer_q.timer_ptr[timer_ptr->q_idx] )
  {
    /* timer may still be valid even if q_idx is corrupted */
    uint32_t i;
    for ( i=0; i<ARR_SIZE(sns_ddf_timer_q.timer_ptr); i++ )
    {
      if (timer_ptr == sns_ddf_timer_q.timer_ptr[i] )
      {
        MSG_3(MSG_SSID_SNS, DBG_ERROR_PRIO, "obj_match - timer 0x%x idx %u changed to %u", 
              timer, timer_ptr->q_idx, i);
        timer_ptr->q_idx = i;
        break;
      }
    }
    if ( timer_ptr->q_idx >= ARR_SIZE(sns_ddf_timer_q.timer_ptr) || 
         timer_ptr != sns_ddf_timer_q.timer_ptr[timer_ptr->q_idx] )
    {
      timer_ptr = NULL; /* truly invalid */
    }
  }
  return timer_ptr;
}

/*===========================================================================

  FUNCTION:   sns_ddf_timer_queue_add

===========================================================================*/
/*!
  @brief Adds the given timer to the timer queue

  @param[i]  timer_ptr - the timer to be added to timer queue
 
  @return   queue index of the given timer
 */
/*=========================================================================*/
static uint32_t sns_ddf_timer_queue_add(sns_ddf_timer_obj_s* timer_ptr)
{
  uint32_t i;
  for ( i=0; i<ARR_SIZE(sns_ddf_timer_q.timer_ptr); i++ )
  {
    if (sns_ddf_timer_q.timer_ptr[i] == NULL)
    {
      sns_ddf_timer_q.timer_ptr[i] = timer_ptr;
      timer_ptr->q_idx             = i;
      break;
    }
  }
  if ( i >= ARR_SIZE(sns_ddf_timer_q.timer_ptr) )
  {
    MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, "que_add - queue full!!!");
  }
  return i;
}

/*===========================================================================

  FUNCTION:   sns_ddf_timer_queue_del

===========================================================================*/
/*!
  @brief Removes the given timer from the timer queue

  @param[i]  timer - timer to be removed from queue
 
  @return    the timer matching the given index
 */
/*=========================================================================*/
static sns_ddf_timer_obj_s* sns_ddf_timer_queue_del(sns_ddf_timer_s timer)
{
  sns_ddf_timer_obj_s* timer_ptr = sns_ddf_timer_obj_match(timer);

  if ( timer_ptr != NULL )
  {
    sns_ddf_timer_q.timer_ptr[timer_ptr->q_idx] = NULL;
  }
  else
  {
    MSG_1(MSG_SSID_SNS, DBG_ERROR_PRIO, "que_del - timer 0x%x not found!!!", timer);
  }
  return timer_ptr;
}

/*----------------------------------------------------------------------------
 *  Public Functions
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:   sns_ddf_timer_init

===========================================================================*/
/*!
  @brief See sns_ddf_util.h
 */
/*=========================================================================*/
sns_ddf_status_e sns_ddf_timer_init(
    sns_ddf_timer_s*      dest_timer_ptr,
    sns_ddf_handle_t      dd_handle,
    sns_ddf_driver_if_s*  dd_interface,
    void*                 arg,
    bool                  periodic)
{
  sns_err_code_e       status = SNS_SUCCESS;
  sns_ddf_timer_obj_s* timer_ptr = NULL;
  bool                 in_uimg = sns_ddf_smgr_get_uimg_refac(dd_handle);
  uint32_t             idx;

  if ( dest_timer_ptr == NULL || dd_handle == NULL || 
       dd_interface == NULL || dd_interface->handle_timer == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if ( in_uimg )
  {
    timer_ptr = SNS_OS_ANY_MALLOC(SNS_DBG_MOD_DSPS_DDF, sizeof(sns_ddf_timer_obj_s));
  }
  else
  {
    timer_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_DDF, sizeof(sns_ddf_timer_obj_s));
  }
  if ( timer_ptr == NULL )
  {
    return SNS_DDF_ENOMEM;
  }

  if ( (idx = sns_ddf_timer_queue_add(timer_ptr)) >= ARR_SIZE(sns_ddf_timer_q.timer_ptr) )
  {
    if ( in_uimg )
    {
      SNS_OS_ANY_FREE(timer_ptr);
    }
    else
    {
      SNS_OS_FREE(timer_ptr);
    }
    return SNS_DDF_ENOMEM;
  }

  timer_ptr->dd_handle    = dd_handle;
  timer_ptr->dd_interface = dd_interface;
  timer_ptr->arg          = arg;
  timer_ptr->is_periodic  = periodic;

  /* Create the timer with SMGR's callback function since it is responsible
   * for dispatching the driver's actual callback at its discretion. */
  if ( in_uimg )
  {
    status = sns_em_create_utimer_obj(
        sns_ddf_smgr_notify_timer,
        (void*)idx,
        periodic ? SNS_EM_TIMER_TYPE_PERIODIC : SNS_EM_TIMER_TYPE_ONESHOT,
        &timer_ptr->em_timer);
  }

  /* Create a bigImage timer if the driver is not in uImage OR if
   * the system is unable to create a uImage timer. */
  if ( NULL == timer_ptr->em_timer )
  {
    status = sns_em_create_timer_obj(
        sns_ddf_smgr_notify_timer,
        (void*)idx,
        periodic ? SNS_EM_TIMER_TYPE_PERIODIC : SNS_EM_TIMER_TYPE_ONESHOT,
        &timer_ptr->em_timer);
  }

  if ( status != SNS_SUCCESS )
  {
    sns_ddf_timer_queue_del(timer_ptr);
    if ( in_uimg )
    {
      SNS_OS_ANY_FREE(timer_ptr->em_timer);
      SNS_OS_ANY_FREE(timer_ptr);
    }
    else
    {
      SNS_OS_FREE(timer_ptr->em_timer);
      SNS_OS_FREE(timer_ptr);
    }
    return SNS_DDF_EFAIL;
  }

  *dest_timer_ptr = timer_ptr;
  return SNS_DDF_SUCCESS; 
}

/*===========================================================================

  FUNCTION:   sns_ddf_timer_start

===========================================================================*/
/*!
  @brief See sns_ddf_util.h
 */
/*=========================================================================*/
SNS_DDF_UIMAGE_CODE 
sns_ddf_status_e sns_ddf_timer_start(sns_ddf_timer_s timer, uint32_t usec)
{
  sns_err_code_e status = SNS_ERR_NOTSUPPORTED;

#ifndef SNS_QDSP_SIM
  uint32_t offset = sns_em_convert_usec_to_localtick(usec);
  sns_ddf_timer_obj_s* timer_ptr = sns_ddf_timer_obj_match(timer);
  
  if ( timer_ptr != NULL )
  {
    timer_ptr->sched = sns_ddf_get_timestamp() + offset;
    if ( sns_em_is_utimer(timer_ptr->em_timer) )
    {
      status = sns_em_register_utimer(timer_ptr->em_timer, offset);
    }
    else
    {
      sns_smgr_enter_bigimage();
      status = sns_em_register_timer(timer_ptr->em_timer, offset);
      sns_smgr_exit_bigimage();
    }
#ifdef SNS_DDF_TIMER_DEBUG
    DDF_MSG_3(LOW, "start - idx=%u offset=%u per=%u", 
           timer_ptr->q_idx, offset, timer_ptr->is_periodic);
#endif
  }
  else
  {
    DDF_MSG_1(ERROR, "start - timer 0x%x not found", timer);
    status = SNS_ERR_BAD_PARM;
  }
#endif

  return (status == SNS_SUCCESS) ? SNS_DDF_SUCCESS : SNS_DDF_EFAIL;
}

/*===========================================================================

  FUNCTION:   sns_ddf_timer_cancel

===========================================================================*/
/*!
  @brief See sns_ddf_util.h
 */
/*=========================================================================*/
SNS_DDF_UIMAGE_CODE sns_ddf_status_e sns_ddf_timer_cancel(sns_ddf_timer_s timer)
{
  sns_err_code_e status = SNS_ERR_NOTSUPPORTED;

#ifndef SNS_QDSP_SIM
  sns_ddf_timer_obj_s* timer_ptr = sns_ddf_timer_obj_match(timer);

  if ( timer_ptr != NULL )
  {
    if ( sns_em_is_utimer(timer_ptr->em_timer) )
    {
      status = sns_em_cancel_utimer(timer_ptr->em_timer);
    }
    else
    {
      sns_smgr_enter_bigimage();
      status = sns_em_cancel_timer(timer_ptr->em_timer);
      sns_smgr_exit_bigimage();
    }
    qurt_atomic_clear_bit(&sns_ddf_timer_q.pending_flags, timer_ptr->q_idx);
#ifdef SNS_DDF_TIMER_DEBUG
    DDF_MSG_2(LOW, "cancel - idx=%u per=%u", 
           timer_ptr->q_idx, timer_ptr->is_periodic);
#endif
  }
  else
  {
    DDF_MSG_1(ERROR, "cancel - timer 0x%x not found", timer);
    status = SNS_ERR_BAD_PARM;
  }
#endif

  return (status == SNS_SUCCESS) ? SNS_DDF_SUCCESS : SNS_DDF_EFAIL;
}

/*===========================================================================

  FUNCTION:   sns_ddf_timer_release

===========================================================================*/
/*!
  @brief See sns_ddf_util.h
 */
/*=========================================================================*/
SNS_DDF_UIMAGE_CODE sns_ddf_status_e sns_ddf_timer_release(sns_ddf_timer_s timer)
{
  sns_err_code_e status = SNS_ERR_NOTSUPPORTED;
#ifndef SNS_QDSP_SIM
  sns_ddf_timer_obj_s* timer_ptr = sns_ddf_timer_queue_del(timer);

  sns_smgr_enter_bigimage();
  if ( timer_ptr != NULL )
  {
    status = SNS_SUCCESS;
    qurt_atomic_clear_bit(&sns_ddf_timer_q.pending_flags, timer_ptr->q_idx);
    if ( sns_em_is_utimer(timer_ptr->em_timer) )
    {
      sns_em_delete_utimer_obj(timer_ptr->em_timer);
      SNS_OS_ANY_FREE(timer_ptr);
    }
    else
    {
      sns_em_delete_timer_obj(timer_ptr->em_timer);
      SNS_OS_FREE(timer_ptr);
    }
  }
  else
  {
    DDF_MSG_1(ERROR, "release - timer 0x%x not found", timer);
    status = SNS_ERR_BAD_PARM;
  }
  sns_smgr_exit_bigimage();
#endif

  return (status == SNS_SUCCESS) ? SNS_DDF_SUCCESS : SNS_DDF_EFAIL;
}

/*===========================================================================

  FUNCTION:   sns_ddf_timer_post

===========================================================================*/
/*!
  @brief See sns_smgr_ddf_priv.h
 */
/*=========================================================================*/
SNS_DDF_UIMAGE_CODE void sns_ddf_timer_post(void* timer_id)
{
#ifndef SNS_QDSP_SIM
  uint32_t idx = (uint32_t)timer_id;
  if ( idx < ARR_SIZE(sns_ddf_timer_q.timer_ptr) && 
       sns_ddf_timer_q.timer_ptr[idx] != NULL )
  {
    sns_ddf_timer_obj_s* timer_ptr = sns_ddf_timer_q.timer_ptr[idx];
    qurt_atomic_set_bit(&sns_ddf_timer_q.pending_flags, idx);

    timer_ptr->expired = sns_em_get_system_time();

#ifdef SNS_DDF_TIMER_DEBUG
    if ( !timer_ptr->is_periodic && (timer_ptr->expired - timer_ptr->sched) > 15 )
    {
      DDF_MSG_3(ERROR, "idx=%u sched=%u expired=%u",
             idx, timer_ptr->sched, timer_ptr->expired);
    }
#endif
  }
#endif
}

/*===========================================================================

  FUNCTION:   sns_ddf_timer_dispatch

===========================================================================*/
/*!
  @brief See sns_smgr_ddf_priv.h
 */
/*=========================================================================*/
SNS_DDF_UIMAGE_CODE void sns_ddf_timer_dispatch(void)
{
#ifndef SNS_QDSP_SIM
  uint32_t flags;

  while (0 != (flags = sns_ddf_timer_q.pending_flags))
  {
    uint32_t idx = get_bit_position(flags);
    qurt_atomic_clear_bit(&sns_ddf_timer_q.pending_flags, idx);

    if ( idx < ARR_SIZE(sns_ddf_timer_q.timer_ptr) && 
         sns_ddf_timer_q.timer_ptr[idx] != NULL )
    {
      sns_ddf_timer_obj_s* timer_ptr = sns_ddf_timer_q.timer_ptr[idx];
      timer_ptr->dispatched = sns_ddf_get_timestamp();

      sns_profiling_log_qdss(SNS_DDF_TIMER_DISPATCH_ENTER, 0);
      if( !sns_em_is_utimer(timer_ptr->em_timer) )
      {
        sns_smgr_enter_bigimage();
      }
      timer_ptr->dd_interface->handle_timer(timer_ptr->dd_handle, timer_ptr->arg);
      if( !sns_em_is_utimer(timer_ptr->em_timer))
      {
        sns_smgr_exit_bigimage();
      }
      sns_profiling_log_qdss(SNS_DDF_TIMER_DISPATCH_EXIT, 0);

#ifdef SNS_DDF_TIMER_DEBUG
      {
        uint32_t took = sns_ddf_get_timestamp() - timer_ptr->dispatched;
        if ( timer_ptr->dispatched - timer_ptr->expired > 15 )
        {
          DDF_MSG_3(ERROR, "idx=%u expired=%u dispatched=%u",
                 idx, timer_ptr->expired, timer_ptr->dispatched);
        }
        if ( took > 15 )
        {
          DDF_MSG_2(ERROR, "idx=%u handle_timer took %u ticks",
                 idx, took);
        }
      }
#endif
    }
  }
#endif
}

