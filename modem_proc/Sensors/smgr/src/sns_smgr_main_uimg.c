/*=============================================================================
  @file sns_smgr_main_uimg.c

  This file implements the portion of SMGR task that executes in microImage

*******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_main_uimg.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-03-08  pn   Removed ENABLE_MD and DISABLE_MD signal bits
  2016-01-07  pn   Reboots sensor after its heartbeat stops
  2015-11-03  pn   Moved DDF timer handling to DDF
  2015-10-13  jtl  Added heartbeat check utility function
  2015-10-12  pn   Heartbeat monitor to exit uIImage before setting sensor state
  2015-09-30  pn   Decommissions a sensor when it misses many consecutive interrupts
  2015-09-24  pn   Code cleanup
  2015-09-22  pn   Self-scheduling streaming sensors are monitored for activities
  2015-6-29   agk  Added power rail per sensor
  2015-02-24  pn   Updated QDSS events
  2015-01-22  BD   Changed the way of putting functions in uImage to use sections
  2014-09-25  pn   Handled SNS_SMGR_DEPOT_COPY_SIG
  2014-09-14  vy   Moved app sleep and wake sig processing to BigImage
  2014-09-08  pn   Moved PM interface functions to PM_IF module
  2014-09-03  vy   Fixed KW error
  2014-08-25  vy   Enabled QDSS logging
  2014-08-18  pn   Removed KW error
  2014-07-31  pn   Replaced smgr_power_state_type_e with sns_pm_pwr_rail_e
  2014-07-14  VY   Fixed compiler warnings
  2014-07-09  vy   Cleanup during bringup
  2014-06-05  vy   Further refactored to support uImage  
  2014-05-24  vy   Removed uimage transition hysteresis timer
  2014-05-13  vy   Refactored for uImage with PM support
  2014-05-07  pn   Added support for MD deregistration
  2014-04-24  pn   Initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_em.h"
#include "sns_pm.h"
#include "sns_profiling.h"
#include "sns_rh_util.h"
#include "sns_smgr.h"
#include "sns_smgr_ddf_priv.h"
#include "sns_smgr_hw.h"
#include "sns_smgr_main.h"
#include "sns_smgr_pm_if.h"
#include "sns_smgr_util.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define __SNS_MODULE__ SNS_SMGR_UIMG

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Global Variables
 * -------------------------------------------------------------------------*/
SNS_SMGR_UIMAGE_DATA sns_smgr_s              sns_smgr;
SNS_SMGR_UIMAGE_DATA OS_STK                  sns_smgr_task_stack[SNS_MODULE_STK_SIZE_DSPS_SMGR];
SNS_SMGR_UIMAGE_DATA sns_profiling_latency_s sns_latency;

/*----------------------------------------------------------------------------
 * Function prototypes
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:   sns_smgr_dd_service

===========================================================================*/
/*!
  @brief This function Enter the device driver framework for each queued DD
         timer expiration or IRQ handling

  @detail

  @param[i]  none
  @return   none
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE SMGR_STATIC void sns_smgr_dd_service(void)
{
  sns_hw_set_qup_clk(true);

#ifndef SNS_QDSP_SIM  
  sns_ddf_timer_dispatch();
#else
  {
    uint8_t err = 0; 
    sns_os_sigs_post(sns_dri_sim.sns_dri_sig_grp, SNS_DRI_SIMULATE, OS_FLAG_SET, &err);
  }
#endif //SNS_QDSP_SIM  
  sns_ddf_signal_dispatch();
  
  sns_hw_set_qup_clk(false);
  sns_profiling_log_qdss(SNS_SMGR_DD_SERVICE_EXIT, 0);
}

/*===========================================================================

  FUNCTION:   sns_smgr_timer_cb

===========================================================================*/
/*!
  @brief timer call back. Entered on interrupt

  @detail

  @param[i]  cb_arg  callback argument
  @return   none
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE void sns_smgr_timer_cb(void *cb_arg)
{
  if ( (sns_em_timer_obj_t*)cb_arg == &sns_smgr.tmr_obj )
  {
    sns_profiling_log_qdss(SNS_SMGR_POLLING_ENTER, 1, true);
    if( sns_latency.ltcy_measure_enabled )
    {
      /* latency measurement under polling mode */
      sns_latency.polling_cb_ts = sns_em_get_timestamp();
    }
    sns_smgr_signal_me(SNS_SMGR_SENSOR_READ_SIG);
  }
  else if ( (sns_em_timer_obj_t*)cb_arg == &sns_smgr.sensor_ev_tmr_obj )
  {
    sns_smgr_signal_me(SNS_SMGR_SENSOR_STATE_SIG);
  }
  else if ( (sns_em_timer_obj_t*)cb_arg == &sns_smgr.heartbeat_tmr_obj )
  {
    sns_smgr_signal_me(SNS_SMGR_HEARTBEAT_SIG);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_check_heartbeat

===========================================================================*/
/*!
  @brief Checks heartbeats of self-sched streaming sensors
  @param    none
  @return   none
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE SMGR_STATIC void sns_smgr_check_heartbeat(void)
{
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr;
  sns_smgr_ddf_sensor_s* ddf_sensor_ptrs[SNS_SMGR_NUM_SENSORS_DEFINED];
  uint32_t num_sensors = 0;
  uint32_t offset = SMGR_MAX_TICKS;

  SMGR_FOR_EACH_Q_ITEM(&sns_smgr.heartbeat_monitor_que, ddf_sensor_ptr, sched_link)
  {
    if ( !sns_smgr_sensor_is_ok(ddf_sensor_ptr) )
    {
      ddf_sensor_ptrs[num_sensors++] = ddf_sensor_ptr;
    }
    else
    {
      offset = MIN(offset, ddf_sensor_ptr->heartbeat_interval);
    }
  }
  
  if ( num_sensors > 0 )
  {
    uint32_t i = num_sensors;
    sns_pm_img_mode_e curr_mode = sns_smgr_get_curr_image_vote();
    sns_smgr_vote_image_mode(SNS_IMG_MODE_BIG);
    
    for ( i=0; i<num_sensors; i++ )
    {
      sns_smgr_sensor_s* sensor_ptr = ddf_sensor_ptrs[i]->sensor_ptr;
      SNS_SMGR_PRINTF5(
        ERROR, "heartattack - sensor=%u intvl=%u odr_changed=%u latest_sample_ts=%u now=%u",
        SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptrs[i]->data_type), 
        ddf_sensor_ptrs[i]->heartbeat_interval, sensor_ptr->odr_change_tick, 
        ddf_sensor_ptrs[i]->latest_sample_ts, sns_smgr.last_tick);
      
      if ( sensor_ptr->num_reboots < SMGR_MAX_REBOOTS )
      {
        sns_smgr_reboot_sensor(ddf_sensor_ptrs[i]);
      }
      else
      {
        SNS_SMGR_PRINTF1(
           ERROR, "heartattack - failed reset %u times", sensor_ptr->num_reboots);
        sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_FAILED);
        sns_rh_signal_me(SNS_RH_SENSOR_HEARTBEAT_SIG);
      }
    }
    sns_smgr_vote_image_mode(curr_mode);
  }
  if ( offset != SMGR_MAX_TICKS )
  {
    sns_em_register_utimer(sns_smgr.heartbeat_tmr_obj, offset);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_sensor_ev_timer_cb

===========================================================================*/
/*!
  @brief Sensor event timer call back

  @param[i]  cb_arg - unused callback argument
  @return   none
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE void sns_smgr_sensor_ev_timer_cb(void *cb_arg)
{
  sns_smgr_signal_me(SNS_SMGR_SENSOR_STATE_SIG);
}


/*===========================================================================

  FUNCTION:   sns_smgr_task_main

===========================================================================*/
/*!
  @brief SMGRT task

  @detail

  @param[i]  none
  @return   none
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE void sns_smgr_task_main(void)
{
  uint8_t   err;
  OS_FLAGS  sig_flags;
  /* Union of uImage and BigImage signals */
  OS_FLAGS  OS_FLAGS_sig_type =
     SNS_SMGR_SENSOR_READ_SIG     |
     SNS_SMGR_DD_SERVICE_SIG      |
     SNS_SMGR_SENSOR_STATE_SIG    |
     SNS_SMGR_HEARTBEAT_SIG       |
     SNS_SMGR_PWR_RAIL_SIG        |
     SNS_SMGR_VOTE_MIPS_BW        |
     SNS_SMGR_APP_WAKE_SIG        |
     SNS_SMGR_APP_SLEEP_SIG       |
     SNS_SMGR_QMI_MSG_SIG; 

  /* Only BigImage signals */
  OS_FLAGS  OS_FLAGS_sig_type_big =
     SNS_SMGR_SENSOR_STATE_SIG  |
     SNS_SMGR_PWR_RAIL_SIG      |
     SNS_SMGR_VOTE_MIPS_BW      |
     SNS_SMGR_APP_WAKE_SIG      |
     SNS_SMGR_APP_SLEEP_SIG     |
     SNS_SMGR_QMI_MSG_SIG;

  (void)sns_os_sigs_add(sns_smgr.sig_grp, OS_FLAGS_sig_type);
  
  sns_smgr_vote_image_mode(SNS_IMG_MODE_NOCLIENT);

  /* ========================= MAIN TASK LOOP ===============================*/
  while ( 1 )
  {
    //sns_profiling_log_timestamp((uint64_t) SNS_SMGR_LATENCY_PROFILE_END);
    sns_profiling_log_qdss(SNS_SMGR_EXIT, 0);

    sig_flags = sns_os_sigs_pend(sns_smgr.sig_grp, OS_FLAGS_sig_type,
                                 OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);

    sns_profiling_log_qdss(SNS_SMGR_ENTER, 1, sig_flags);
    //sns_profiling_log_timestamp((uint64_t) SNS_SMGR_LATENCY_PROFILE_START);

    sns_smgr_update_last_tick();
    
#ifndef SNS_PLAYBACK_SKIP_SMGR
    if ( sig_flags & SNS_SMGR_SENSOR_READ_SIG )
    {
      sns_smgr_sampling_cycle();
      sns_profiling_log_qdss(SNS_SMGR_POLLING_EXIT, 0);
    }
#endif
    if ( sig_flags & SNS_SMGR_DD_SERVICE_SIG )
    {
      sns_smgr_dd_service();
    }
    if ( sig_flags & SNS_SMGR_HEARTBEAT_SIG )
    {
      sns_smgr_check_heartbeat();
    }
    if ( sig_flags & OS_FLAGS_sig_type_big )
    {
      sns_smgr_enter_bigimage();
      sns_smgr_handle_bigimage_signals(sig_flags);
      sns_smgr_exit_bigimage();
    }
    if ( sig_flags & (~OS_FLAGS_sig_type))
    {
      SNS_SMGR_PRINTF1(ERROR, "unknown signal(s) 0x%x", 
                       sig_flags & ~OS_FLAGS_sig_type);
    }
    sig_flags = 0;
  } /* while */
}


/*===========================================================================

  FUNCTION:   sns_smgr_app_state_change

===========================================================================*/
/*!
  @brief Called by Power module when AP state changes

  @detail
  @param[i] app_is_asleep - TRUE if AP entered sleep state, FALSE wake state
  @return   none
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE void sns_smgr_app_state_change(bool app_is_asleep)
{
  uint8_t  err;
  uint32_t set_bit = app_is_asleep ? SNS_SMGR_APP_SLEEP_SIG : SNS_SMGR_APP_WAKE_SIG;
  uint32_t clr_bit = app_is_asleep ? SNS_SMGR_APP_WAKE_SIG  : SNS_SMGR_APP_SLEEP_SIG;
  sns_os_sigs_post ( sns_smgr.sig_grp, clr_bit, OS_FLAG_CLR, &err );
  sns_os_sigs_post ( sns_smgr.sig_grp, set_bit, OS_FLAG_SET, &err );
}


/*===========================================================================

  FUNCTION:   sns_smgr_signal_me

===========================================================================*/
/*!
  @brief Posts a signal to SMGR task

  @detail
  @param[i] sig - the signal to post
  @return   none
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE void sns_smgr_signal_me(uint32_t sig)
{
  uint8_t os_err;
  sns_os_sigs_post(sns_smgr.sig_grp, sig, OS_FLAG_SET, &os_err);
}


/*===========================================================================

  FUNCTION:   sns_smgr_find_sensor

===========================================================================*/
/*!
  @brief Finds the associated sensor structure given a sensor ID.

  @detail
  @param[i] sensor_id - the sensor ID passed in via Sensor1 API
  @return
   sensor structure, or NULL if not found
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE sns_smgr_sensor_s* sns_smgr_find_sensor(smgr_sensor_id_e sensor_id)
{
  sns_smgr_sensor_s* sensor_ptr = NULL;
  uint8_t i;
  for ( i=0; i<ARR_SIZE(sns_smgr.sensor) && sensor_ptr==NULL; i++ )
  {
    if ( SMGR_SENSOR_ID(&sns_smgr.sensor[i]) == sensor_id )
    {
      sensor_ptr = &sns_smgr.sensor[i];
    }
  }
  return sensor_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_init

===========================================================================*/
/*!
  @brief This function starts smgr task

  @detail

  @param   none
  @return  return SNS_SUCCESS always
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE sns_err_code_e sns_smgr_init(void)
{
  sns_os_task_create_ext(sns_smgr_task, NULL,
                         (OS_STK *)&sns_smgr_task_stack[SNS_MODULE_STK_SIZE_DSPS_SMGR-1],
                         SNS_MODULE_PRI_DSPS_SMGR,
                         SNS_MODULE_PRI_DSPS_SMGR,
                         (OS_STK *)&sns_smgr_task_stack[0],
                         SNS_MODULE_STK_SIZE_DSPS_SMGR,
                         (void *)0,
                         OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR |
                         OS_TASK_OPT_ISLAND,
                         (uint8_t *)"SNS_SMGR");
  return SNS_SUCCESS;
}

