/*=============================================================================
  @file sns_smgr_main.c

  This file implements SMGR task loop

*******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_main.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-03-08  pn   Changed MD state change request method
  2016-02-11  pn   Cleans up WAKE_SIG/SLEEP_SIG processing
  2016-01-26  pn   RH is no longer a child thread of SMGR
  2015-09-24  pn   Code cleanup
  2015-09-22  pn   Self-scheduling streaming sensors are monitored for activities
  2015-08-24  bd   Use only one depot for each sensor
  2015-08-21  bd   Removed SNS_SMGR_DEPOT_COPY_SIG
  2015-07-06  hw   Adding sampling latency measurement support
  2015-06-29  agk  Removed power rail check in handle_ap_wake
  2015-04-09  pn   Replaced calls to sns_em_get_timestamp() with sns_ddf_get_timestamp()
  2015-02-24  pn   Updated QDSS events
  2014-11-19  pn   Sets event_done_tick when processing MD requests
  2014-09-25  pn   Added support for copying uImage depot to bigImage depot
  2014-09-14  vy   Moved app sleep and wake sig processing to BigImage
  2014-09-08  pn   Moved PM interface functions to PM_IF module
  2014-09-03  jhh  Added client name on pm client
  2014-07-31  pn   Enabled power control; removed obsolete code
  2014-07-22  VY   Fixed compiler warnings
  2014-07-09  vy   Clean up during bringup
  2014-06-16  pn   sns_smgr_hw_init() returns when last registry response is received
  2014-06-11  ps   add Power rail voting, fix hang at sns_smgr_hw_init
  2014-06-11  vy   Further refactored to support uImage  
  2014-05-24  vy   Introduced utimers
  2014-05-09  vy   Refactored for uImage and PM support
  2014-04-24  pn   Initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stringl.h>

#include "sensor1.h"
#include "sns_em.h"
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_init.h"
#include "sns_profiling.h"
#include "sns_rh_util.h"
#include "sns_smgr.h"
#include "sns_smgr_hw.h"
#include "sns_smgr_main.h"
#include "sns_smgr_mr.h"
#include "sns_smgr_pm_if.h"
#include "sns_smgr_reg.h"
#include "sns_smgr_util.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define __SNS_MODULE__ SNS_SMGR

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Global Variables
 * -------------------------------------------------------------------------*/
extern sns_smgr_s              sns_smgr;

#ifdef SNS_QDSP_SIM
extern sns_dri_sim_s           sns_dri_sim;
#endif

/*----------------------------------------------------------------------------
 * Function prototypes
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*===========================================================================

  FUNCTION:   sns_smgr_process_msg

===========================================================================*/
/*!
  @brief This function processes the given incoming message for SMGR
  @param   none
  @return  none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_msg(void)
{
  sns_smgr_mr_msg_s* msg_ptr;

  sns_smgr_update_last_tick();

  while ( NULL != (msg_ptr = sns_smgr_mr_get_msg()) )
  {
    sns_smgr_mr_header_s* hdr_ptr = &msg_ptr->header;
    SNS_SMGR_PRINTF3(
      MED, "process_msg - svc=%d msg=0x%x now=%u",
      hdr_ptr->svc_num, hdr_ptr->msg_id, sns_smgr.last_tick);

    if ( SNS_REG2_SVC_ID_V01 == hdr_ptr->svc_num )
    {
      sns_smgr_process_reg_resp_msg(msg_ptr);
    }
    else
    {
      SNS_SMGR_PRINTF0(ERROR, "process_msg - unexpected message");
    }
  }
}
 
/*===========================================================================

  FUNCTION:   sns_smgr_ltcy_measure_check(void)

===========================================================================*/
/*!
  @brief Check whether latency measurement is enabled or not.

  @detial
  @param[i] none
  @return  none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_ltcy_measure_check(void)
{
  uint8_t i;

  sns_latency.ltcy_measure_enabled = false;
  for ( i=0; i<ARR_SIZE(sns_smgr.sensor); i++ )
  {
    if( sns_smgr.sensor[i].const_ptr->flags & SNS_REG_SSI_FLAG_LTCY_ENABLE )
    {
      sns_latency.ltcy_measure_enabled = true;
      break;
    }
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_handle_app_wake_sig

===========================================================================*/
/*!
  @brief Handles APP_WAKE signal

  @detail
  @param  none
  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_handle_app_wake_sig(void)
{
  uint8_t i;

  SNS_SMGR_PRINTF2(
    HIGH, "WAKE_SIG curr state(%u) change count(%u)",
    sns_smgr.app_is_awake, sns_smgr.app_state_toggle_count);

  if ( !sns_smgr.app_is_awake )
  {
    sns_smgr.app_is_awake = true;
    sns_smgr.app_state_toggle_count++;

    for ( i=0; i<ARR_SIZE(sns_smgr.sensor); i++ )
    {
      if ( sns_smgr.sensor[i].sensor_state == SENSOR_STATE_OFF )
      {
        sns_smgr.sensor[i].event_done_tick = sns_ddf_get_timestamp();
        sns_smgr_schedule_sensor_event(1);
        break; /* one is enough */
      }
    }
    
    sns_smgr_vote_mips_bw_reqd();
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_handle_app_sleep_sig

===========================================================================*/
/*!
  @brief Handles APP_SLEEP signal

  @detail
  @param  none
  @return none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_handle_app_sleep_sig(void)
{
  SNS_SMGR_PRINTF2(
    HIGH, "SLEEP_SIG curr state(%u), change count(%u)",
    sns_smgr.app_is_awake, sns_smgr.app_state_toggle_count);
  
  if ( sns_smgr.app_is_awake )
  {
    sns_smgr.app_is_awake = false;
    sns_smgr.app_state_toggle_count++;
    sns_smgr_vote_mips_bw_reqd();
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_hw_init

===========================================================================*/
/*!
  @brief Waits for registry responses that carry HW config info and applies
         the HW config before returning

  @param[i] none
  @return  none
 */
/*=========================================================================*/
#ifndef ADSP_STANDALONE
SMGR_STATIC void sns_smgr_hw_init(void)
{
  OS_FLAGS sig_mask = SNS_SMGR_QMI_MSG_SIG;

  sns_os_sigs_add(sns_smgr.sig_grp, sig_mask);
  sns_hw_bd_config();
  sns_smgr_dd_init();

  while ( sns_smgr.all_init_state != SENSOR_ALL_INIT_DONE )
  {
    uint8_t  err;
    OS_FLAGS sig_flags;
    sig_flags = sns_os_sigs_pend(sns_smgr.sig_grp, sig_mask,
                                 OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);
    SNS_SMGR_PRINTF1(LOW, "hw_init - sig_flags=0x%x", sig_flags);

    if ( sig_flags & SNS_SMGR_QMI_MSG_SIG )
    {
      sns_smgr_process_msg();
    }
    sns_smgr_dd_init();
  }

  //check the registry flags if latency measurement enabled
  sns_smgr_ltcy_measure_check();
}
#else
void sns_smgr_hw_init(void)
{
  sns_hw_bd_config();
  sns_smgr_dd_init();
  sns_smgr_ltcy_measure_check();
}
#endif

/*===========================================================================

  FUNCTION:   smgr_init

===========================================================================*/
/*!
  @brief initialize some flags and variables in sns_smgr

  @detail

  @param[i] none
  @return   none
 */
/*=========================================================================*/
SMGR_STATIC void smgr_init(void)
{
  uint8_t err;

  /* --------------------------------------------------------------------- */
  SNS_OS_MEMZERO(&sns_smgr, sizeof(sns_smgr_s));
  sns_smgr.app_is_awake = true;
  sns_q_init(&sns_smgr.sched_que);
  sns_q_init(&sns_smgr.heartbeat_monitor_que);

  /* Init signal events */
  sns_smgr.sig_grp = sns_os_sigs_create ( 0, &err);
  SNS_ASSERT(sns_smgr.sig_grp != NULL);

  sns_em_create_utimer_obj(&sns_smgr_timer_cb, &sns_smgr.tmr_obj, 
                           SNS_EM_TIMER_TYPE_ONESHOT, &sns_smgr.tmr_obj);
  SNS_ASSERT(sns_smgr.tmr_obj != NULL);

  sns_em_create_utimer_obj(&sns_smgr_timer_cb, &sns_smgr.sensor_ev_tmr_obj, 
                           SNS_EM_TIMER_TYPE_ONESHOT, &sns_smgr.sensor_ev_tmr_obj);
  SNS_ASSERT(sns_smgr.sensor_ev_tmr_obj != NULL);

  sns_em_create_utimer_obj(&sns_smgr_timer_cb, &sns_smgr.heartbeat_tmr_obj, 
                           SNS_EM_TIMER_TYPE_ONESHOT, &sns_smgr.heartbeat_tmr_obj);
  SNS_ASSERT(sns_smgr.heartbeat_tmr_obj != NULL);

  /* Initialize interface with PM */
  sns_smgr_pm_stats_init();
}


/*===========================================================================

  FUNCTION:   sns_smgr_init_timer_cb

===========================================================================*/
/*!
  @brief Sensor init timer call back

  @param[i]  cb_arg - unused callback argument
  @return   none
 */
/*=========================================================================*/
void sns_smgr_init_timer_cb(void *cb_arg)
{
  sns_smgr_signal_me(SNS_SMGR_INIT_TIMER_SIG);
}



/*===========================================================================

  FUNCTION:   sns_smgr_task

===========================================================================*/
/*!
  @brief SMGRT task

  @detail This is created by sns_smgr_init, which is called during sns_init

  @param[i]  p_arg
  @return   none
 */
/*=========================================================================*/
void sns_smgr_task(void* p_arg)
{
  
  /* --------------------------------------------------------------------- */
  /* Start initializing SMGR modules */
  smgr_init();
  sns_smgr_sensor_init();

  /* Message Router init only returns after successfully connecting to services */
  sns_smgr_mr_init(sns_smgr.sig_grp);

  /* HW init only returns after all device drivers are initialized */
  sns_smgr_hw_init();

  /* Inform init code that SMGR init is done */
  sns_init_done();

  /* Run the main task loop, no return */
  sns_smgr_task_main();
}


/*===========================================================================

  FUNCTION:   sns_smgr_handle_bigimage_signals

===========================================================================*/
/*!
  @brief Handle big image events and messages

  @detail 

  @param[i]  signal
  @return   none
 */
/*=========================================================================*/
void sns_smgr_handle_bigimage_signals(OS_FLAGS sig_flags)
{
  if ( sig_flags & SNS_SMGR_SENSOR_STATE_SIG )
  {
    smgr_process_sensor_event();
  }
  if ( sig_flags & SNS_SMGR_PWR_RAIL_SIG )
  {
    sns_hw_send_powerrail_msg_tmr_cb_proc();
  }
  if (sig_flags & SNS_SMGR_QMI_MSG_SIG)
  {
    sns_smgr_process_msg();
  }
  if (sig_flags & SNS_SMGR_VOTE_MIPS_BW)
  {
    sns_smgr_vote_mips_bw_reqd();
  }
  if ( sig_flags & SNS_SMGR_APP_WAKE_SIG )
  {
    sns_smgr_handle_app_wake_sig();
  }
  if ( sig_flags & SNS_SMGR_APP_SLEEP_SIG )
  {
    sns_smgr_handle_app_sleep_sig();
  }
}

