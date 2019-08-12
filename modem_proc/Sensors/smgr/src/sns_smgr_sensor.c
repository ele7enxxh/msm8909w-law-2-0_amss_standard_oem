/*=============================================================================
  @file sns_smgr_sensor.c

  This file contains the logic for managing sensor data sampling in SMGR

*******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_sensor.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-04-25  pn   Resets requested_odr when rebooting sensor
  2016-04-04  pn   Distinguishes between requested ODR and current ODR
  2016-03-11  pn   Handles FIFOs delivered in chunks
  2016-03-08  pn   Handles signals that result in multi state transition
  2016-02-05  pn   Replaced call to reschedule_reports() with smgr_update_assoc_reports()
  2016-01-27  pn   Takes into account that Primary sensor may not always be present
  2016-01-25  sc   Do not reset first_valid_sample_received for event sensor
  2016-01-13  sc   Stop processing calibration request if accuracy is 0
  2016-01-13  pn   Must also update odr_change_tick for sensors not supporting ODR attribute
  2016-01-07  pn   Reboots sensor after its heartbeat stops
  2015-12-30  pn   Added missing steps for ODR_CHANGED event handling
  2015-12-15  pn   Must clear first_valid_sample_received when turning off sensor
  2015-10-20  pn   Makes sure request queue is processed
  2015-10-15  pn   - Fixed handling of ODR_CHANGED event
                   - Turns off self-sched before changing ODRs only for DRI sensors
  2015-09-30  pn   Enabled controls of sensors in FAILED state
  2015-09-24  pn   Code cleanup
  2015-09-22  pn   sched_link is also used by self-sched sensors
  2015-08-25  tc   Prevent a sensor from going to LOWPOWER while DAF is active
  2015-08-24  bd   Adopted only uimg depot or big image depot for a sensor
  2015-08-20  pn   Must update power state before resetting sensor
  2015-08-19  pn   Simplified powerrails control
  2015-07-09  ps   Fix compiler warnings
  2015-07-02  pn   Turns off self-sched before changing ODRs for DRI sensors
  2015-04-29  agk  Power rail changes
  2015-06-05  pn   Added SMGR_SENSOR_FLAGS_FIFO_WM_CHANGED_B
  2015-05-21  pn   Gives RH unlimited time to consume FIFO data from Flush request
  2015-04-21  pn   Removed references to deprecated field new_sample_idx
  2015-04-09  pn   Added support for 56-bit timestamp
  2015-03-20  pn   Optimized retrieval of latest sample timestamp
  2015-03-05  pn   Reconfigures only READY sensors after receiving valid samples
  2015-02-26  bd   Turn off sensor regardless ODR setting results
  2015-02-24  pn   Updated QDSS events
  2015-02-17  pn   (Re)schedules reports only after receiving valid samples
  2015-01-02  pn   Finishes configuring sensor in MD state
  2014-12-30  pn   - Does not apply LPF_DURATION for sensor with zero idle-to-ready
                   - Sets powering-up timer after, not before, turning on powerrail
                   - Handles ITEM_DEL in MD state
                   - Value of event_done_tick is no longer the deciding factor
                   of whether to process sensor event
  2014-12-17  pn   Hastens first polling instance only if it is more than half
                   sampling interval away
  2014-12-05  pn   - Keeps track of DD powerstate
                   - Handles ITEM_DEL in IDLE state
  2014-12-01  pn   Fixed handling requests in IDLE state 
  2014-11-15  pn   Updates sensor_status's last_status_ts
  2014-11-11  pn   - Refactored smgr_process_sensor_event()
                   - Fixed handling of MD related events and state
  2014-10-23  pn   Updates sensor power state after disabling MD
  2014-10-21  vy   Only set sensor ODR attr to 0 before disabling its interrupt
  2014-10-17  pn   Clears sensor ODR before disabling its interrupt
  2014-10-16  vy   Updated EWUR api to handle interrupt latency est
  2014-09-25  pn   Manages both depots of each sensor
  2014-09-16  vy   Updated uImage support for self-test
  2014-09-16  MW   Allow driver self-test in failed state
  2014-09-14  vy   Used new uImage supported DDF api to free memhandler
  2014-09-11  pn   Protects all shared sensor state variables, not just client DB
  2014-09-08  pn   - Wakeup rate estimation is updated when sampling rate changes
                   - Cleans up individual sensor type when it has no clients left
  2014-09-03  pn   Optimize sensor state change from idle to configuring to ready
  2014-08-27  pn   Updates power when all sensors are in steady state
  2014-08-26  pn   Votes for LPM when only Accel is in use
  2014-08-23  vy   Skip process calibration for non-existent cal data
  2014-08-05  pn   Added handler for FIFO_FLUSH and ODR_CHANGED signals from RH task
  2014-07-31  pn   Enabled power control; removed obsolete code
  2014-07-21  pn   Prevents corrupting client DB when ODR is changed
  2014-07-14  VY   Fixed compiler warnings
  2014-07-21  pn   Prevents corrupting client DB when turning off sensors
  2014-07-09  vy   Clean-up, removed temp hacks
  2014-06-16  pn   Used updated DDF sensor's client_status structure
  2014-06-05  vy   Added a workaround to avoid normal timer to run it on Virtio
  2014-05-28  tc   SubHz Sampling
  2014-05-23  pn   Fixed false alarm on self-test that returned result immediately
  2014-05-23  pn   Updated state machine to handle requests forwarded from RH
  2014-05-12  tc   Converted frequencies to fixed point, Q16. This is to
                   accommodate frequencies less than 1. Q16 fixed point is
                   used, instead of floating point, because the Q6 does not
                   have hardware-support for floating point division.
  2014-05-15  MW   Enhanced SNS_DDF_ATTRIB_LOWPASS handling
  2014-05-13  vy   Refactored for uImage
  2014-05-07  pn   Added support for MD deregistration
  2014-04-23  pn   Initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_memmgr.h"
#include "sns_profiling.h"
#include "sns_rh_sol.h"
#include "sns_rh_util.h"
#include "sns_smgr_main.h"
#include "sns_smgr_depot.h"
#include "sns_smgr_ddf_if.h"
#include "sns_smgr_hw.h"
#include "sns_smgr_util.h"
#include "sns_smgr_pm_if.h"

#if defined(SNS_QDSP_SIM)
#include "sns_qdsp_playback_utils.h"
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
//#define SNS_SMGR_SAMPLING_DEBUG
#ifdef SNS_SMGR_SAMPLING_DEBUG
#define SNS_SMGR_SAMPLING_DBG0 SNS_SMGR_PRINTF0
#define SNS_SMGR_SAMPLING_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_SAMPLING_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_SAMPLING_DBG3 SNS_SMGR_PRINTF3
#define SNS_SMGR_SAMPLING_DBG4 SNS_SMGR_PRINTF4
#else
#define SNS_SMGR_SAMPLING_DBG0(level,msg)
#define SNS_SMGR_SAMPLING_DBG1(level,msg,p1)
#define SNS_SMGR_SAMPLING_DBG2(level,msg,p1,p2)
#define SNS_SMGR_SAMPLING_DBG3(level,msg,p1,p2,p3)
#define SNS_SMGR_SAMPLING_DBG4(level,msg,p1,p2,p3,p4)
#endif

/*----------------------------------------------------------------------------
 *  External functions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
#ifdef SNS_QDSP_SIM
extern sns_dri_sim_s sns_dri_sim;
#endif

/*----------------------------------------------------------------------------
 *  Local Functions
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:   smgr_sensor_type_read_state_to_idle

===========================================================================*/
/*!
  @brief Sets the read state for all types to idle

  @Detail

  @param[i] sensor_ptr - the sensor leader structure

  @return
    NONE
*/
/*=========================================================================*/
SMGR_STATIC void smgr_sensor_type_read_state_to_idle(sns_smgr_sensor_s* sensor_ptr)
{
  uint32_t i;
  for ( i = 0; i < ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
  {
    if ( sensor_ptr->ddf_sensor_ptr[i] != NULL )
    {
      sensor_ptr->ddf_sensor_ptr[i]->sensor_type_state = SENSORTYPE_STATE_IDLE;
    }
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_update_device_sampling_factor

===========================================================================*/
/*!
  @details For sensors in DRI mode, if no High Performance clients request CIC
  filtering, computes the factor between ODR and max requested rate.
  The samples deposited into buffer will have the sampling rate that is
  ODR/device_sampling_factor.

  @param[i] ddf_sensor_ptr  ddf sensor

  @return
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_update_device_sampling_factor(
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  ddf_sensor_ptr->device_sampling_factor = 1;

  if ( !sns_smgr_ddf_sensor_is_event_sensor(ddf_sensor_ptr) && 
       SMGR_SENSOR_IS_SELF_SCHED(ddf_sensor_ptr->sensor_ptr) )
  {
    sns_smgr_client_stat_s* client_stat_ptr = &ddf_sensor_ptr->client_stat;
    q16_t max_rate = client_stat_ptr->max_requested_freq_hz;
    ddf_sensor_ptr->device_sampling_factor = 1;

    if ( SMGR_BIT_TEST(client_stat_ptr->flags, 
                       (RH_RPT_ITEM_FLAGS_ACCURATE_TS_B | 
                        RH_RPT_ITEM_FLAGS_DECIM_FILTER_B)) )
    {
      max_rate = DDF_ODRTOFIX_Q16(ddf_sensor_ptr->current_odr);
    }
    max_rate = MAX(1, max_rate);
    ddf_sensor_ptr->device_sampling_factor =
      MAX(1, DDF_ODRTOFIX_Q16(ddf_sensor_ptr->current_odr/max_rate));
  }

  SNS_SMGR_PRINTF2(
    MED, "update_dev_sampling_factor - odr=%d factor=%d",
    ddf_sensor_ptr->current_odr, ddf_sensor_ptr->device_sampling_factor);
}


/*===========================================================================

  FUNCTION:   sns_smgr_config_odr

===========================================================================*/
/*!
  @brief configure ODR(Output Data Rate) for all sensor types of the sensor

  @Detail

  @param[i] sensor_ptr - the sensor leader structure

  @return
    NONE
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_config_odr(sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_ddf_status_e    ddf_status;
  sns_smgr_sensor_s*  sensor_ptr = ddf_sensor_ptr->sensor_ptr;
  uint32_t            odr = 0;
  sns_ddf_sensor_e    sensor_type = SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type);
  
  SNS_ASSERT(sensor_ptr->odr_attr_supported);

  if ( ddf_sensor_ptr->client_stat.max_requested_freq_hz > 0 )
  {
    odr = sns_smgr_choose_odr(sensor_ptr);
  }

  SNS_SMGR_PRINTF4(
    MED, "config_odr - ddf_sensor=%d req=%u curr=%u new=%u",
    SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type),
    ddf_sensor_ptr->requested_odr, ddf_sensor_ptr->current_odr, odr);

  if ( odr != ddf_sensor_ptr->requested_odr )
  {
    if ( SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) && 
         !SMGR_SENSOR_FIFO_IS_ENABLE(sensor_ptr) )
    {
      sns_smgr_dd_enable_sched_data(sensor_ptr, sensor_type, false);

      /* Forces FIFO watermark recalculation */
      sensor_ptr->fifo_cfg.current_watermark = 0;
    }

    ddf_status = sns_smgr_set_attr(sensor_ptr, sensor_type, SNS_DDF_ATTRIB_ODR, &odr);
    if ( SNS_DDF_SUCCESS == ddf_status )
    {
      ddf_sensor_ptr->requested_odr = odr;
      if ( SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) && 
           !SMGR_SENSOR_FIFO_IS_ENABLE(sensor_ptr) )
      {
        if ( odr > 0 )
        {
          sns_smgr_update_ewur( FX_CONV_Q16(odr,0), FX_CONV_Q16(ddf_sensor_ptr->current_odr,0), 
                                sensor_ptr->is_uimg_refac, 
                                !sns_smgr_ddf_sensor_is_event_sensor(ddf_sensor_ptr) );

          /* trigger self schedule */
          sns_smgr_dd_enable_sched_data(sensor_ptr, sensor_type, true);
        }
        else
        { 
          sns_smgr_update_ewur( 0, FX_CONV_Q16(ddf_sensor_ptr->current_odr,0), 
                                sensor_ptr->is_uimg_refac, 
                                !sns_smgr_ddf_sensor_is_event_sensor(ddf_sensor_ptr) );

          /* stops streaming */
          sns_smgr_dd_enable_sched_data(sensor_ptr, sensor_type, false);
        }
      }
    }
    else if ( SNS_DDF_EBUS != ddf_status || SNS_DDF_EDEVICE != ddf_status )
    {
      /* should not fail */
      ERR_FATAL("config_odr - sensor=%u odr=%u status=%u",
                sensor_type, odr, ddf_status);
    }
  }
}

/*===========================================================================

  FUNCTION:   smgr_find_slow_or_same_scheduler

===========================================================================*/
/*!
  @brief Find the scheduler that has slow freq(i.e. bigger interval) or the same interval.

  @param[i] interval - The timetick of the scheduling period

  @return
   A pointer to the scheduler block which is slower or the same frequency,
   or returns NULL when there is nothing
*/
/*=========================================================================*/
SMGR_STATIC smgr_sched_block_s *smgr_find_slow_or_same_scheduler (uint32_t interval)
{
  smgr_sched_block_s* found_ptr = NULL;
  smgr_sched_block_s* sched_blk_ptr;
  SMGR_FOR_EACH_Q_ITEM(&sns_smgr.sched_que, sched_blk_ptr, sched_link)
  {
    if ( sched_blk_ptr->sched_intvl >= interval )
    {
      found_ptr = sched_blk_ptr;
      break;
    }
  }
  return found_ptr;
}


/*===========================================================================

  FUNCTION:   sns_smgr_register_into_scheduler

===========================================================================*/
/*!
  @brief Insert given sensor type into a schedule block.

  @param[i] ddf_sensor_ptr - sensor type representing a sensor/data type pair

  @return
   true Successfully registerd into a scheduler
   false Failed to register into a scheduler because of:
   - Memory allocation failed for the new scheduler
   - etc
*/
/*=========================================================================*/
SMGR_STATIC bool sns_smgr_register_into_scheduler(sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  uint32_t sched_intvl;
  smgr_sched_block_s* existing_sched_ptr;
  sns_q_s* q_ptr;

  if ( SMGR_SENSOR_IS_SELF_SCHED(ddf_sensor_ptr->sensor_ptr) || 
       (q_ptr = SMGR_QUE_HEADER_FROM_LINK(&ddf_sensor_ptr->sched_link)) != NULL )
  {
    return false;
  }

  sched_intvl = ddf_sensor_ptr->depot_data_interval;
  existing_sched_ptr = smgr_find_slow_or_same_scheduler(sched_intvl);

  SNS_SMGR_PRINTF3(
    HIGH, "reg_into_scheduler - ddf_sensor=%d depot_rate=0x%X intvl=%d",
    SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr,
                     ddf_sensor_ptr->data_type),
    ddf_sensor_ptr->depot_data_rate, (unsigned)sched_intvl);

  if ( (NULL != existing_sched_ptr) &&
       (existing_sched_ptr->sched_intvl == sched_intvl) )
  {
    sns_q_put(&existing_sched_ptr->ddf_sensor_queue, &ddf_sensor_ptr->sched_link);
    SNS_SMGR_PRINTF2(
      HIGH, "reg_into_scheduler - existing - next_tick=%u q_cnt=%d",
      existing_sched_ptr->next_tick, sns_q_cnt(&existing_sched_ptr->ddf_sensor_queue));
  }
  else
  {
    smgr_sched_block_s* new_sched_ptr = SMGR_ANY_ALLOC_STRUCT(smgr_sched_block_s);

    if ( NULL != new_sched_ptr )
    {
      SNS_OS_MEMZERO(new_sched_ptr, sizeof(smgr_sched_block_s));

      sns_q_link(new_sched_ptr, &new_sched_ptr->sched_link);  /* init the link field */
      sns_q_init(&new_sched_ptr->ddf_sensor_queue);           /* init the queue */

      new_sched_ptr->sampling_rate = ddf_sensor_ptr->depot_data_rate;
      new_sched_ptr->sched_intvl   = sched_intvl;
      new_sched_ptr->next_tick  = 
        (sns_smgr.last_tick + sched_intvl)/sched_intvl * sched_intvl;
      if ( (new_sched_ptr->next_tick - sns_smgr.last_tick) > (sched_intvl >> 1) )
      {
        new_sched_ptr->next_tick = sns_smgr.last_tick;
      }
      SNS_SMGR_PRINTF2(
        HIGH, "reg_into_scheduler - new - next_tick=%u now=%u",
        new_sched_ptr->next_tick, sns_smgr.last_tick);
      sns_q_put(&new_sched_ptr->ddf_sensor_queue, &ddf_sensor_ptr->sched_link);

      if ( NULL == existing_sched_ptr )
      {
        sns_q_put(&sns_smgr.sched_que, &new_sched_ptr->sched_link);
      }
      else
      {
        /* Ascending order by sched_intvl(i.e. Descending order by freq) */
        sns_q_insert(&new_sched_ptr->sched_link, &existing_sched_ptr->sched_link);
      }

      sns_smgr_update_ewur( new_sched_ptr->sampling_rate, 0, 
                            ddf_sensor_ptr->sensor_ptr->is_uimg_refac, false );
      sns_smgr_update_maxsensors();
      
      /* schedule a plan cycle */
      sns_smgr.sched_tick = sns_smgr.last_tick - SMGR_SCHEDULE_GRP_TOGETHER_TICK;

      sns_profiling_log_qdss(SNS_SMGR_POLLING_ENTER, 1, false);
      sns_smgr_signal_me(SNS_SMGR_SENSOR_READ_SIG);
    }
    else
    {
      SNS_SMGR_PRINTF2(
        ERROR, "reg_into_scheduler - alloc %d bytes failed for intvl %u",
        sizeof(smgr_sched_block_s), sched_intvl);
      return false;
    }
  }
  SNS_SMGR_PRINTF1(MED, "reg_into_scheduler - num schedules=%d",
                   sns_q_cnt(&sns_smgr.sched_que));
  return true;
}


/*===========================================================================

  FUNCTION:   sns_smgr_deregister_from_scheduler

===========================================================================*/
/*!
  @brief Removes given sensor from scheduler's queue.

  @param[i] ddf_sensor_ptr - the sensor to be removed from scheduler

  @return
   none
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_deregister_from_scheduler(
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_q_s* q_ptr = SMGR_QUE_HEADER_FROM_LINK(&ddf_sensor_ptr->sched_link);
  if ( !SMGR_SENSOR_IS_SELF_SCHED(ddf_sensor_ptr->sensor_ptr) && q_ptr != NULL )
  {
    smgr_sched_block_s* sched_ptr =
      SMGR_GET_ENTRY(q_ptr, smgr_sched_block_s, ddf_sensor_queue);

    SNS_SMGR_PRINTF3(
      HIGH, "dereg_from_scheduler - ddf_sensor=%u intvl=%u next=%u",
      SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr,
                       ddf_sensor_ptr->data_type),
      sched_ptr->sched_intvl, sched_ptr->next_tick);

    sns_q_delete(&ddf_sensor_ptr->sched_link); /* removed from schedule block */
    if ( sns_q_cnt(q_ptr) == 0 ) /* last sensor removed from queue? */
    {
      sns_smgr_update_ewur( 0, sched_ptr->sampling_rate, 
                            ddf_sensor_ptr->sensor_ptr->is_uimg_refac, false );
      sns_smgr_update_maxsensors();
      
      sns_q_delete(&sched_ptr->sched_link); /* removed from schedule queue */
      SNS_OS_ANY_FREE(sched_ptr);
    }
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_set_lpf

===========================================================================*/
/*!
  @brief Configures the LPF of sensor device.

  @param[in] sensor_ptr: Ptr to the data structure for a specific device driver
  @param[in] bw:  bandwidth, Hz, must be a legitimate value for the device,

  @return
    SNS_DDF_SUCCESS if LPF was set, error code if not
*/
/*=========================================================================*/
SMGR_STATIC sns_ddf_status_e sns_smgr_set_lpf(
  sns_smgr_sensor_s* sensor_ptr,
  q16_t bw)
{
  sns_ddf_status_e  ddf_status = SNS_DDF_EFAIL;
  sns_ddf_sensor_e sensor_type = SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr);
  uint32_t idx;

  if ( sensor_type == SNS_DDF_SENSOR__NONE )
  {
    sensor_type = SMGR_SENSOR_TYPE_SECONDARY(sensor_ptr);
  }
  
  for ( idx=0; idx<sensor_ptr->num_lpf; idx++ )
  {
    if ( bw <= sensor_ptr->lpf_table[idx] )
    {
      ddf_status = sns_smgr_set_attr(sensor_ptr,
                                     sensor_type,
                                     SNS_DDF_ATTRIB_LOWPASS,
                                     &idx);
      break;
    }
  }
  if ( idx == sensor_ptr->num_lpf )
  {
    /* BW not found in table */
    SNS_SMGR_PRINTF2(ERROR, "set_lpf - sensor=%d num_lpf=%d",
                     SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->num_lpf);
  }
  return ddf_status;
}


/*===========================================================================

  FUNCTION:   sns_smgr_config_lpf

===========================================================================*/
/*!
  @brief Selects and sets the LPF for the given sensor.

  @detail This function is called only for sensors that do not support ODR
          attribute (or are not self-scheduling?)

  @param[i] sensor_ptr - sensor structure

  @return
   lpf bandwidth
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_config_lpf(sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  uint32_t odr;
  q16_t    lpf;
  SNS_ASSERT(0 != ddf_sensor_ptr->sensor_ptr->num_lpf);

  odr = sns_smgr_choose_odr(ddf_sensor_ptr->sensor_ptr);

  SNS_SMGR_PRINTF2(
    LOW, "config_lpf - curr_odr=%u new_odr=%u", ddf_sensor_ptr->current_odr, odr);

  if ( odr != ddf_sensor_ptr->current_odr )
  {
    lpf = FX_FLTTOFIX_Q16(odr) >> 1; /* BW = odr/2 */
    if ( (odr > 0) &&
         (sns_smgr_set_lpf(ddf_sensor_ptr->sensor_ptr, lpf) == SNS_DDF_SUCCESS) )
    {
      ddf_sensor_ptr->requested_odr = ddf_sensor_ptr->current_odr = odr;
    }
    else
    {
      ddf_sensor_ptr->requested_odr = ddf_sensor_ptr->current_odr = 0;
    }
    SNS_SMGR_PRINTF3(
      LOW, "config_lpf - ddf_sensor=%d odr=%u max_req_freq=0x%X",
      SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type),
      ddf_sensor_ptr->current_odr, ddf_sensor_ptr->client_stat.max_requested_freq_hz);
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_clear_depots

===========================================================================*/
/*!
  @brief    Clears all the sample depots for the given DDF sensor

  @param[i] ddf_sensor_ptr

  @return
    NONE
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_clear_depots(sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_os_mutex_pend(ddf_sensor_ptr->depot_mutex_ptr, 0, NULL);
  ddf_sensor_ptr->latest_sample_ts = 0;
  if ( ddf_sensor_ptr->bimg_depot_ptr != NULL )
  {
    sns_smgr_depot_clear(ddf_sensor_ptr->bimg_depot_ptr, 0);
  }
  if ( ddf_sensor_ptr->uimg_depot_ptr != NULL )
  {
    sns_smgr_depot_clear(ddf_sensor_ptr->uimg_depot_ptr, 0);
  }
  sns_os_mutex_post(ddf_sensor_ptr->depot_mutex_ptr);
}


/*===========================================================================

  FUNCTION:   sns_smgr_update_sampling_setting

===========================================================================*/
/*!
  @brief    Configures ODR or LPF of a sensor

  @param[i] ddf_sensor_ptr

  @return
    NONE
*/
/*=========================================================================*/
void sns_smgr_update_sampling_setting(sns_smgr_ddf_sensor_s*  ddf_sensor_ptr)
{
  sns_smgr_sensor_s* sensor_ptr = ddf_sensor_ptr->sensor_ptr;
  sns_smgr_client_stat_s* cs_ptr = &ddf_sensor_ptr->client_stat;
  sns_smgr_client_modifier_s* cli_mod_ptr = &cs_ptr->client_modifier;
  /* ---------------------------------------------------------------------- */

  SNS_SMGR_PRINTF4(
    MED, "update_sampling - mod orig: sensor=%u odr=%u depot=0x%X sampling=0x%X",
    SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type),
    cli_mod_ptr->odr, cli_mod_ptr->depot_data_rate, cli_mod_ptr->max_req_sampling_rate);
  SNS_SMGR_PRINTF4(
    MED, "update_sampling - ddf val:  sensor=%u odr=%u depot=0x%X sampling=0x%X",
    SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type),
    ddf_sensor_ptr->current_odr, ddf_sensor_ptr->depot_data_rate, 
    cs_ptr->max_requested_freq_hz);

  sns_os_mutex_pend(ddf_sensor_ptr->sensor_ptr->mutex_ptr, 0, NULL);

  if ( (cli_mod_ptr->odr != ddf_sensor_ptr->current_odr) ||
       (cli_mod_ptr->depot_data_rate != ddf_sensor_ptr->depot_data_rate) ||
       ((cli_mod_ptr->max_req_sampling_rate != cs_ptr->max_requested_freq_hz) &&
        !SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr)) )
  {
    q16_t depot_data_rate = ddf_sensor_ptr->depot_data_rate;
    if ( cli_mod_ptr->odr != ddf_sensor_ptr->current_odr )
    {
      uint32_t watermark = ddf_sensor_ptr->sensor_ptr->fifo_cfg.current_watermark;
      ddf_sensor_ptr->data_poll_ts = 0;
      ddf_sensor_ptr->num_expected_samples = 1;
      ddf_sensor_ptr->num_received_samples = 0;
      if ( !sns_smgr_ddf_sensor_is_event_sensor(ddf_sensor_ptr) )
      {
        ddf_sensor_ptr->first_valid_sample_received = false;
      }
      if ( watermark > 0 && SMGR_SENSOR_FIFO_IS_SUPPORTED(sensor_ptr) && 
           SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) )
      {
        sns_smgr_update_ewur(FX_CONV_Q16(ddf_sensor_ptr->current_odr/watermark,0), 
                             FX_CONV_Q16(cli_mod_ptr->odr/watermark,0),
                             sensor_ptr->is_uimg_refac, true );
      }
    }

    sns_smgr_deregister_from_scheduler(ddf_sensor_ptr);
    if ( cs_ptr->max_requested_freq_hz > 0)
    {
      sns_smgr_update_device_sampling_factor(ddf_sensor_ptr);
      ddf_sensor_ptr->depot_data_rate =
        sns_smgr_compute_depot_data_rate(ddf_sensor_ptr);
      if ( ddf_sensor_ptr->depot_data_rate > 0 )
      {
        ddf_sensor_ptr->depot_data_interval =
          sns_smgr_get_sched_intval(ddf_sensor_ptr->depot_data_rate);
        ddf_sensor_ptr->fifo_interval = ddf_sensor_ptr->depot_data_interval;
      }
      sns_smgr_register_into_scheduler(ddf_sensor_ptr);
    }
    else
    {
      ddf_sensor_ptr->depot_data_rate = 0;
    }

    sns_rh_sol_update_items_info(ddf_sensor_ptr, false);
    if ( (depot_data_rate != ddf_sensor_ptr->depot_data_rate) &&
         !sns_smgr_is_event_sensor(sensor_ptr, ddf_sensor_ptr->data_type) )
    {
      sns_smgr_clear_depots(ddf_sensor_ptr);
    }
    cs_ptr->sensor_status.last_status_ts = sns_smgr.last_tick;
    sns_rh_sol_update_sensor_status(ddf_sensor_ptr);

    cli_mod_ptr->odr                   = ddf_sensor_ptr->current_odr;
    cli_mod_ptr->depot_data_rate       = ddf_sensor_ptr->depot_data_rate;
    cli_mod_ptr->max_req_sampling_rate = cs_ptr->max_requested_freq_hz;
    cli_mod_ptr->last_updated          = sns_smgr.last_tick;
  }
  else if ( ddf_sensor_ptr->current_odr != 0 )
  {
    sns_rh_sol_update_items_info(ddf_sensor_ptr, true);
  }

  sns_os_mutex_post(ddf_sensor_ptr->sensor_ptr->mutex_ptr);
}


/*===========================================================================

  FUNCTION:   sns_smgr_update_odr

===========================================================================*/
/*!
  @brief    Retrieves sensor ODR and updates DDF sensor with the value.

  @param[i] ddf_sensor_ptr  : pointer to DDF sensor

  @return
    NONE
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_update_odr(sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  uint32_t len;
  sns_ddf_odr_t* odr_ptr;
  sns_ddf_status_e  ddf_status;
  sns_smgr_sensor_s* sensor_ptr = ddf_sensor_ptr->sensor_ptr;

  if( sensor_ptr->odr_attr_supported )
  {
    sns_ddf_sensor_e sensor = SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type);
    ddf_status = sns_smgr_get_attr(
                      sensor_ptr,
                      sensor,
                      SNS_DDF_ATTRIB_ODR,
                      (void**)&odr_ptr,
                      &len);
    if ( ddf_status == SNS_DDF_SUCCESS )
    {
      SNS_SMGR_PRINTF4(
        LOW, "update_odr - sensor=%u req=%u old=%u new=%u",
        sensor, ddf_sensor_ptr->requested_odr, ddf_sensor_ptr->current_odr, 
        *(sns_ddf_odr_t *)odr_ptr);

      if ( *(sns_ddf_odr_t *)odr_ptr != ddf_sensor_ptr->current_odr )
      {
        ddf_sensor_ptr->current_odr = (*(sns_ddf_odr_t *)odr_ptr);
        sensor_ptr->odr_change_tick = sns_smgr.last_tick;
      }
    }
    sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_schedule_next_event

===========================================================================*/
/*!
  @brief insert configuring event

  @Detail

  @param[i] sensor_ptr - the sensor leader structure

  @return
    NONE
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_schedule_next_event(sns_smgr_sensor_s* sensor_ptr)
{
  uint32_t lpf_dur = 0;
  if ( sensor_ptr->num_lpf > 0 && sensor_ptr->const_ptr->idle_to_ready_time > 0 )
  {
    lpf_dur = SMGR_LPF_DURATION(smgr_sensor_type_max_odr(sensor_ptr));
  }
  if ( SENSOR_STATE_IDLE == sensor_ptr->sensor_state )
  {
    lpf_dur = MAX(lpf_dur, sensor_ptr->const_ptr->idle_to_ready_time);
  }
  if ( lpf_dur > 0 )
  {
    sensor_ptr->event_done_tick = sensor_ptr->odr_change_tick + lpf_dur;
  }

  SNS_SMGR_PRINTF4(
    MED, "sched_next_ev - sensor=%u state=%u i2r=%u lpf_dur=%u",
    SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state, 
    sensor_ptr->const_ptr->idle_to_ready_time, lpf_dur);
  SNS_SMGR_PRINTF3(
    MED, "sched_next_ev - odr_tick=%u done_tick=%u time_now=%u",
    sensor_ptr->odr_change_tick, sensor_ptr->event_done_tick, sns_smgr.last_tick);
}


/*===========================================================================

  FUNCTION:   sns_smgr_config_odr_lpf

===========================================================================*/
/*!
  @brief configure ODR or LPF

  @Detail set appropriate odr/LPF and reconfigure CIC configuration

  @param[i] sensor_ptr - the sensor leader structure

  @return
    NONE
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_config_odr_lpf(sns_smgr_sensor_s* sensor_ptr)
{
  uint8_t i;
  sns_ddf_odr_t max_odr = 0;

  SNS_SMGR_PRINTF3(
    HIGH, "config_odr_lpf - sensor=%d state=%d num_lpf=%d",
    SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state, sensor_ptr->num_lpf);
  for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
  {
    sns_ddf_odr_t prev_odr;
    sns_smgr_ddf_sensor_s* ddf_sensor_ptr;
    if ( sensor_ptr->ddf_sensor_ptr[i] == NULL )
    {
      continue;
    }
    ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[i];
    prev_odr = ddf_sensor_ptr->current_odr;

    if( sensor_ptr->odr_attr_supported )
    {
      sns_smgr_config_odr(ddf_sensor_ptr);
    }
    else if( 0 != sensor_ptr->num_lpf)
    {
      sns_smgr_config_lpf(ddf_sensor_ptr);
    }
    else
    {
      ddf_sensor_ptr->requested_odr = ddf_sensor_ptr->current_odr = 
        DDF_FIXTOODR_CEIL_Q16(ddf_sensor_ptr->client_stat.max_requested_freq_hz);
    }

    if ( prev_odr != ddf_sensor_ptr->current_odr )
    {
      sensor_ptr->odr_change_tick = sns_smgr.last_tick;
    }
  }

  for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
  {
    sns_smgr_ddf_sensor_s* ddf_sensor_ptr;
    if ( sensor_ptr->ddf_sensor_ptr[i] == NULL )
    {
      continue;
    }
    ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[i];
    sns_smgr_update_odr(ddf_sensor_ptr);
    sns_smgr_update_sampling_setting(ddf_sensor_ptr);
    max_odr = MAX(max_odr, ddf_sensor_ptr->current_odr);
  }
  
  /* schedule state change event if ODR is changed */
  if ( sensor_ptr->odr_change_tick == sns_smgr.last_tick )
  {
    sns_smgr_schedule_next_event(sensor_ptr);
  }

  if ( max_odr > 0 )
  {
    sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_CONFIGURING);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_odr_changed_event

===========================================================================*/
/*!
  @brief Checks if ODR for given sensor was changed by DD
 
  @details RH task was notified of ODR changed event, flushed the samples
           and forwarded the notification to SMGR task.  Sample buffer can now
           be cleared in preparation for samples at new ODR.
 
  @param    sensor_ptr
 
  @return   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_odr_changed_event(sns_smgr_sensor_s* sensor_ptr)
{
  uint8_t i;
  sns_ddf_odr_t max_odr = 0;

  if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ODR_CHANGED_B) )
  {
    SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ODR_CHANGED_B);

    for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
    {
      sns_smgr_ddf_sensor_s*  ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[i];

      if ( ddf_sensor_ptr != NULL &&
           SMGR_BIT_TEST(ddf_sensor_ptr->flags, SMGR_SENSOR_FLAGS_ODR_CHANGED_B) )
      {
        sns_ddf_odr_t old_odr = ddf_sensor_ptr->current_odr;
        sns_smgr_update_odr(ddf_sensor_ptr);
        if ( old_odr != ddf_sensor_ptr->current_odr )
        {
          sns_smgr_update_sampling_setting(ddf_sensor_ptr);
        }
        max_odr = MAX(max_odr, ddf_sensor_ptr->current_odr);
        SMGR_BIT_CLEAR(ddf_sensor_ptr->flags, SMGR_SENSOR_FLAGS_ODR_CHANGED_B);
      }
    }
    
    /* schedule state change event if ODR is changed */
    if ( sensor_ptr->odr_change_tick == sns_smgr.last_tick )
    {
      sns_smgr_schedule_next_event(sensor_ptr);
      if ( max_odr > 0 )
      {
        sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_CONFIGURING);
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_reset_sensor

===========================================================================*/
/*!
  @brief Resets the given sensor.

  @Detail

  @param[i] sensor_ptr: Ptr to the data structure for a specific device driver

  @return
   NONE
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_reset_sensor(sns_smgr_sensor_s* sensor_ptr)
{
  sns_ddf_status_e        reset_status = sns_smgr_dd_reset(sensor_ptr);
  sns_smgr_sensor_state_e next_state   = SENSOR_STATE_FAILED;

  if ( SNS_DDF_SUCCESS == reset_status )
  {
    uint32_t i;
    for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
    {
      if ( sensor_ptr->ddf_sensor_ptr[i] != NULL )
      {
        sensor_ptr->ddf_sensor_ptr[i]->current_odr = 0;
        sensor_ptr->ddf_sensor_ptr[i]->data_poll_ts = 0;
        sensor_ptr->ddf_sensor_ptr[i]->latest_sample_ts = 0;
        sensor_ptr->ddf_sensor_ptr[i]->first_valid_sample_received = false;
      }
    }
    if ( SMGR_SENSOR_FIFO_IS_ENABLE(sensor_ptr) )
    {
      sensor_ptr->fifo_cfg.current_watermark = 0;
      sensor_ptr->fifo_cfg.desired_watermark = 0;
    }
    sensor_ptr->odr_change_tick              = 0;
    next_state = SENSOR_STATE_IDLE;
  }
  sns_smgr_set_sensor_state(sensor_ptr, next_state);
}

/*===========================================================================

  FUNCTION:   sns_smgr_update_md_attribute

===========================================================================*/
/*!
  @brief Enables/disables MD interrupt

  @Detail

  @param[i] sensor_ptr - Accel sensor
  @param[i] enable - true to enable, false to disable

  @return
   NONE
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_update_md_attribute(
  sns_smgr_sensor_s* sensor_ptr,
  bool               enable)
{
  sns_ddf_status_e driver_status;
  uint32_t value = (uint32_t)enable;
  driver_status = sns_smgr_set_attr(sensor_ptr, 
                                    SNS_DDF_SENSOR_ACCEL,
                                    SNS_DDF_ATTRIB_MOTION_DETECT,
                                    &value);
  if ( SNS_DDF_SUCCESS == driver_status )
  {
    if ( enable )
    {
      sns_smgr.md_is_enabled = true;
      sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_MD);
      sns_rh_signal_me(SNS_RH_MD_INT_ENABLED_SIG);
    }
    else
    {
      sns_smgr.md_is_enabled = false;
      sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_IDLE);
      sns_rh_signal_me(SNS_RH_MD_INT_DISABLED_SIG);

      if ( smgr_get_max_requested_freq(sensor_ptr) == 0 )
      {
        sns_smgr_set_power_attr(sensor_ptr, SNS_DDF_POWERSTATE_LOWPOWER);
      }
    }
    SNS_SMGR_PRINTF1(HIGH, "update_md - enabled=%u", sns_smgr.md_is_enabled);
  }
  else
  {
    SNS_SMGR_PRINTF1(HIGH, "update_md - failed %u, sending MD int disabled sig", driver_status);
    sns_rh_signal_me(SNS_RH_MD_INT_DISABLED_SIG);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_powering_up_event

===========================================================================*/
/*!
  @brief Handles SENSOR_EVENT_POWERING_UP event for given sensor

  @Detail
    POWERING_UP event is only expected when sensor is in OFF state.

  @param none

  @return none
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_powering_up_event(sns_smgr_sensor_s* in_sensor_ptr)
{
  SNS_SMGR_PRINTF3(
    HIGH, "powering_up_ev - sensor=%u vdd=%u vddio=%u", 
    SMGR_SENSOR_ID(in_sensor_ptr), in_sensor_ptr->const_ptr->vdd_rail,
    in_sensor_ptr->const_ptr->vddio_rail);

  if ( in_sensor_ptr->sensor_state == SENSOR_STATE_OFF &&
       (in_sensor_ptr->flags != 0 || 
        sns_q_cnt(&in_sensor_ptr->request_queue.protected_q) > 0) )
  {
    uint8_t i;

    sns_smgr_update_power_state(in_sensor_ptr);
    sns_smgr_update_last_tick();

    for ( i=0; i<ARR_SIZE(sns_smgr.sensor); i++ )
    {
      sns_smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[i];
      if ( sensor_ptr->sensor_state == SENSOR_STATE_OFF &&
           sensor_ptr->vdd_rail_ptr->powerrail_state > SNS_PWR_RAIL_OFF )
      {
        sensor_ptr->powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
        sensor_ptr->event_done_tick = 
          sns_smgr.last_tick + sensor_ptr->const_ptr->off_to_idle_time;
        sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_POWERING_UP);
      }
    }
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_process_no_sample_event

===========================================================================*/
/*!
  @brief Handles SENSOR_EVENT_NO_SAMPLE event for given sensor

  @Detail
    NO_SAMPLE event is only expected when sensor is not in OFF state.
    1. Clears all ODR values
    2. Disable self scheduling if necessary
    3. Update sensor state to IDLE
    4. Set device to Low power state

  @param[i] ddf_sensor_ptr - the sensor leader structure

  @return none
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_no_sample_event(
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_smgr_deregister_from_scheduler(ddf_sensor_ptr);
  SNS_OS_MEMZERO(&ddf_sensor_ptr->client_stat.client_modifier, 
                 sizeof(ddf_sensor_ptr->client_stat.client_modifier));
  ddf_sensor_ptr->requested_odr = 0;
  ddf_sensor_ptr->current_odr = 0;
  ddf_sensor_ptr->dri_count = 0;
  ddf_sensor_ptr->device_sampling_factor = 1;
  ddf_sensor_ptr->depot_data_rate = 0;
  ddf_sensor_ptr->data_poll_ts = 0;
  ddf_sensor_ptr->sensor_type_state = SENSORTYPE_STATE_IDLE;
  ddf_sensor_ptr->first_valid_sample_received = false;
  sns_smgr_clear_depots(ddf_sensor_ptr);

  ddf_sensor_ptr->client_stat.sensor_status.last_status_ts = sns_smgr.last_tick;
  sns_rh_sol_update_sensor_status(ddf_sensor_ptr);
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_config_filter_done_event

===========================================================================*/
/*!
  @brief Handles CONFIG_FILTER_DONE event for given sensor

  @Detail
    CONFIG_FILTER_DONE event is only expected when sensor is CONFIGURING state.
    1. Update sensor state to READY

  @param[i] sensor_ptr - the sensor leader structure

  @return
    none
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_config_filter_done_event(
  sns_smgr_sensor_s* sensor_ptr)
{
  uint8_t i;

  sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_READY);
  if ( SMGR_BIT_CLEAR_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_FIFO_WM_CHANGED_B) &&
       SMGR_SENSOR_FIFO_IS_SUPPORTED(sensor_ptr) )
  {
    bool force_wm1 = !SMGR_PRIMARY_SENSOR(sensor_ptr)->first_valid_sample_received;
    sns_smgr_fifo_configure(sensor_ptr, force_wm1);
  }
  SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_FIFO_WM_CHANGED_B);

  for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
  {
    sns_smgr_ddf_sensor_s* ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[i];
    if ( ddf_sensor_ptr != NULL && ddf_sensor_ptr->first_valid_sample_received )
    {
      if ( SMGR_SENSOR_FIFO_IS_ENABLE(sensor_ptr) && i == SNS_SMGR_DATA_TYPE_PRIMARY_V01 )
      {
        ddf_sensor_ptr->num_expected_samples = sensor_ptr->fifo_cfg.current_watermark;
      }
      else
      {
        ddf_sensor_ptr->num_expected_samples = 1;
      }
      ddf_sensor_ptr->fifo_interval = 
        ddf_sensor_ptr->depot_data_interval * ddf_sensor_ptr->num_expected_samples;

      SNS_SMGR_PRINTF2(
        MED, "filter_done - ddf_sensor=%u fifo_intvl=%u",
        SMGR_SENSOR_TYPE(sensor_ptr, i), ddf_sensor_ptr->fifo_interval);
      
      sns_rh_sol_smgr_update_assoc_reports(ddf_sensor_ptr);
    }
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_run_self_test

===========================================================================*/
/*!
  @brief This function self-tests a sensor and fills out result in response
    message body.

  @detail

  @param[o] req_ptr   request pointer
  @param[o] resp_ptr  response pointer
  @return  device-specific test error code
 */
/*=========================================================================*/
SMGR_STATIC sns_ddf_status_e sns_smgr_run_self_test(void)
{
  sns_ddf_status_e status;
  sns_smgr_sensor_s* sensor_ptr = sns_smgr.self_test.ddf_sensor_ptr->sensor_ptr;

  SNS_SMGR_PRINTF3(
    HIGH, "run_self_test - ddf_sensor=%d test=%d state=%d",
    SMGR_SENSOR_TYPE(sensor_ptr, sns_smgr.self_test.ddf_sensor_ptr->data_type),
    sns_smgr.self_test.self_test_req->TestType, sns_smgr.self_test.pre_test_state);

  sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_TESTING);
  sns_smgr.self_test.result.test_specific_error = 0;
  if ( (status = sns_smgr_dd_run_test(sensor_ptr)) != SNS_DDF_PENDING )
  {
    sns_smgr_set_sensor_state(sensor_ptr, sns_smgr.self_test.pre_test_state);
    sns_smgr.self_test.self_test_req = NULL;
    sns_smgr.self_test.result.result = (status == SNS_DDF_SUCCESS) ? 
      SNS_SMGR_TEST_RESULT_PASS_V01 : SNS_SMGR_TEST_RESULT_FAIL_V01;
    sns_smgr_update_stay_in_bigimage();
    sns_rh_signal_me(SNS_RH_SELF_TEST_DONE_SIG);
    sns_smgr_update_power_state(sensor_ptr);
  }
  return status;
}


/*===========================================================================
  FUNCTION:   sns_smgr_process_calibration_req
===========================================================================*/
SMGR_STATIC sns_ddf_status_e sns_smgr_process_calibration_req(
  sns_smgr_request_response_s*  smgr_req_resp_ptr)
{
  uint8_t i;
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr = smgr_req_resp_ptr->ddf_sensor_ptr;
  sns_smgr_all_cal_s* all_cal_ptr = ddf_sensor_ptr->all_cal_ptr;
  const sns_smgr_sensor_cal_req_msg_v01* req_ptr = 
    smgr_req_resp_ptr->req.calibration_ptr;
  sns_smgr_cal_s* cal_db_ptr;
  /* -------------------------------------------------------------------- */

  SNS_SMGR_PRINTF3(
    HIGH, "proc_cal - sensor=%u dynamic=%u matrix=%u", 
    req_ptr->SensorId, (req_ptr->usage == SNS_SMGR_CAL_DYNAMIC_V01),
    req_ptr->CompensationMatrix_valid);
  SNS_SMGR_PRINTF3(
    HIGH, "proc_cal - bias = 0x%08x 0x%08x 0x%08x", 
    req_ptr->ZeroBias[0], req_ptr->ZeroBias[1], req_ptr->ZeroBias[2]);

  if ( all_cal_ptr == NULL )
  {
    SNS_SMGR_PRINTF1(
      MED, "proc_cal - sensor=%u - No cal needed", req_ptr->SensorId);
    SNS_OS_FREE(smgr_req_resp_ptr);
    return SNS_DDF_SUCCESS;
  }

  if ( (req_ptr->CalibrationAccuracy_valid) && 
       (req_ptr->CalibrationAccuracy == 0) )
  {
    all_cal_ptr->full_cal.calibration_accuracy = 0;
    cal_db_ptr = (SNS_SMGR_CAL_FACTORY_V01 == req_ptr->usage) ?
        &all_cal_ptr->factory_cal : &all_cal_ptr->auto_cal;
    cal_db_ptr->calibration_accuracy = 0;
    SNS_SMGR_PRINTF1(
      MED, "proc_cal - sensor=%u - unreliable accuracy", req_ptr->SensorId);
    SNS_OS_FREE(smgr_req_resp_ptr);
    return SNS_DDF_SUCCESS;
  }
  
  if ( SNS_SMGR_CAL_FACTORY_V01 == req_ptr->usage )
  {
    /* Nullify all autocal data */
    all_cal_ptr->auto_cal.used           = false;
    sns_smgr_load_default_cal(&all_cal_ptr->auto_cal);

    /* set logging flag to be logged */
    all_cal_ptr->factory_cal.used        = true;
    all_cal_ptr->factory_cal.need_to_log = true;
    cal_db_ptr = &all_cal_ptr->factory_cal;
  }
  else  /* SNS_SMGR_CAL_DYNAMIC_V01 */
  {
    all_cal_ptr->auto_cal.used           = true;
    all_cal_ptr->auto_cal.need_to_log    = true;
    cal_db_ptr = &all_cal_ptr->auto_cal;
  }

  all_cal_ptr->full_cal.used             = true;
  all_cal_ptr->full_cal.need_to_log      = true;
  all_cal_ptr->full_cal.calibration_accuracy = 
    cal_db_ptr->calibration_accuracy         = 
    req_ptr->CalibrationAccuracy_valid ? req_ptr->CalibrationAccuracy : 0;

  SNS_SMGR_PRINTF1(
    HIGH, "proc_cal - accuracy(%u)", (unsigned)cal_db_ptr->calibration_accuracy);

  for ( i=0; i<req_ptr->ZeroBias_len; i++ )
  {
    cal_db_ptr->zero_bias[i] = req_ptr->ZeroBias[i];
    all_cal_ptr->full_cal.zero_bias[i] = 
      all_cal_ptr->factory_cal.zero_bias[i] +
      all_cal_ptr->auto_cal.zero_bias[i];
  }
  for ( i=0; i<req_ptr->ScaleFactor_len; i++ )
  {
    cal_db_ptr->scale_factor[i] = req_ptr->ScaleFactor[i];
    all_cal_ptr->full_cal.scale_factor[i] = req_ptr->ScaleFactor[i];
  }
  if ( req_ptr->CompensationMatrix_valid )
  {
    cal_db_ptr->compensation_matrix_valid = true;
    for ( i=0; i<ARR_SIZE(cal_db_ptr->compensation_matrix); i++)
    {
      cal_db_ptr->compensation_matrix[i] = req_ptr->CompensationMatrix[i];
      SNS_SMGR_PRINTF2(
        ERROR, "proc_cal - Matrix[%d] = %d", 
        i, (int)cal_db_ptr->compensation_matrix[i]);
    }
  }
  sns_smgr_set_attr(
    ddf_sensor_ptr->sensor_ptr,
    SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type),
    SNS_DDF_ATTRIB_BIAS, cal_db_ptr->zero_bias);
  
  sns_rh_put_next_response(smgr_req_resp_ptr); // RH task will free resp_ptr
  
  return SNS_DDF_SUCCESS;
}


/*===========================================================================
  FUNCTION:   sns_smgr_process_driver_access_req
===========================================================================*/
SMGR_STATIC sns_ddf_status_e sns_smgr_process_driver_access_req(
  sns_smgr_sensor_s*            sensor_ptr,
  sns_smgr_request_response_s*  smgr_req_resp_ptr)
{
  sns_ddf_status_e status;
  void*    dd_resp_ptr = NULL;
  uint32_t dd_resp_len = 0;
  sns_smgr_driver_access_req_msg_v01* req_ptr = 
    smgr_req_resp_ptr->req.driver_access_ptr;
  sns_smgr_driver_access_resp_msg_v01* resp_ptr =
    smgr_req_resp_ptr->resp.driver_access_ptr;
  /* -------------------------------------------------------------------- */

  status = sns_smgr_dd_driver_access(sensor_ptr, req_ptr, 
                                     &dd_resp_ptr, &dd_resp_len,
                                     smgr_req_resp_ptr->conn_handle);

  resp_ptr->ResponseStatus_valid = true;
  switch ( status )
  {
  case SNS_DDF_SUCCESS:
  {
    resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_STATUS_SUCCESS_V01;

    if ( (dd_resp_len != 0) && (dd_resp_ptr != NULL) )
    {
      resp_ptr->ResponseMsg_valid = true;
      resp_ptr->ResponseMsg_len = 
        SNS_OS_MEMSCPY(resp_ptr->ResponseMsg, sizeof(resp_ptr->ResponseMsg), 
                       dd_resp_ptr, dd_resp_len);
      if ( resp_ptr->ResponseMsg_len < dd_resp_len )
      {
        SNS_SMGR_PRINTF2(
          ERROR, "Response msg size of %d is truncated to %d", 
          dd_resp_len, resp_ptr->ResponseMsg_len);
      }
    }
    break;
  }
  case SNS_DDF_PENDING:
  {
    resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_PENDING_V01;
    break;
  }
  case SNS_DDF_EINVALID_PARAM:
  {
    resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_INVALID_PARAM_V01;
    break;
  }
  case SNS_DDF_EINVALID_DAF_REQ:
  {
    resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_INVALID_REQ_V01;
    break;
  }
  default:
  {
    resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_DD_FAILURE_V01;
    break;
  }
  }
  sns_ddf_memhandler_free_ex(&sensor_ptr->memhandler, sensor_ptr);

  sns_rh_put_next_response(smgr_req_resp_ptr); // RH task will free resp_ptr

  return status;
}


/*===========================================================================
  FUNCTION:   sns_smgr_process_driver_access_cancel_req
===========================================================================*/
SMGR_STATIC void sns_smgr_process_driver_access_cancel_req(
  sns_smgr_sensor_s*            sensor_ptr,
  sns_smgr_request_response_s*  smgr_req_resp_ptr)
{
  sns_smgr_dd_driver_access_cancel(sensor_ptr, smgr_req_resp_ptr->conn_handle);
  /* no response is expected */
  SNS_OS_FREE(smgr_req_resp_ptr);
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_request

===========================================================================*/
/*!
  @brief Processes request forwarded by Request Handler task

  @param    none
  @return   none
 */
/*=========================================================================*/
SMGR_STATIC sns_ddf_status_e sns_smgr_process_request(
  sns_smgr_sensor_s*           sensor_ptr,
  sns_smgr_request_response_s* req_resp_ptr)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  SNS_SMGR_PRINTF2(
     HIGH, "process_request - svc_num=%u msg_id=%u", 
     req_resp_ptr->svc_num, req_resp_ptr->msg_id);

  if ( (req_resp_ptr->svc_num == SNS_SMGR_SVC_ID_V01) &&
       (req_resp_ptr->msg_id  == SNS_SMGR_SINGLE_SENSOR_TEST_REQ_V01) )
  {
    if ( req_resp_ptr->ddf_sensor_ptr    != NULL && 
         req_resp_ptr->req.self_test_ptr != NULL )
    {
      sns_smgr.self_test.self_test_req  = req_resp_ptr->req.self_test_ptr;
      sns_smgr.self_test.ddf_sensor_ptr = req_resp_ptr->ddf_sensor_ptr;
      sns_smgr.self_test.pre_test_state = sensor_ptr->sensor_state;
      if ( sensor_ptr->sensor_state == SENSOR_STATE_FAILED )
      {
        sensor_ptr->sensor_state = SENSOR_STATE_OFF;
      }
      SNS_OS_FREE(req_resp_ptr);
      sns_smgr_update_stay_in_bigimage();
      status = sns_smgr_run_self_test();
    }
    else
    {
      SNS_SMGR_PRINTF2(
        HIGH, "process_request - bad selftest params sensor=0x%x test=0x%x",
        req_resp_ptr->ddf_sensor_ptr, req_resp_ptr->req.self_test_ptr);
    }
  }
  else
  if ( (req_resp_ptr->svc_num == SNS_SMGR_SVC_ID_V01) &&
       (req_resp_ptr->msg_id  == SNS_SMGR_CAL_REQ_V01) )
  {
    status = sns_smgr_process_calibration_req(req_resp_ptr);
  }
  else
  if ( (req_resp_ptr->svc_num == SNS_SMGR_RESTRICTED_SVC_ID_V01) &&
       (req_resp_ptr->msg_id  == SNS_SMGR_DRIVER_ACCESS_REQ_V01) )
  {
    status = sns_smgr_process_driver_access_req(sensor_ptr, req_resp_ptr);
  }
  else
  if ( (req_resp_ptr->svc_num == SNS_SMGR_RESTRICTED_SVC_ID_V01) &&
       (req_resp_ptr->msg_id  == SNS_SMGR_RESTRICTED_CANCEL_REQ_V01) )
  {
    sns_smgr_process_driver_access_cancel_req(sensor_ptr, req_resp_ptr);
  }
  else
  {
    SNS_SMGR_PRINTF0(ERROR, "process_request - dropping unexpected request");
    SNS_OS_FREE(req_resp_ptr);
  }
  return status;
}

/*===========================================================================

  FUNCTION:   sns_smgr_check_request_queue

===========================================================================*/
/*!
  @brief Checks the request queue of the given sensor for requests from RH task

  @param    sensor_ptr 
  @return   none
 */
/*=========================================================================*/
SMGR_STATIC bool sns_smgr_check_request_queue(sns_smgr_sensor_s* sensor_ptr)
{
  bool req_present = false;
  sns_smgr_request_response_s* req_resp_ptr;
  while ( (req_resp_ptr = sns_smgr_get_next_request(sensor_ptr)) != NULL )
  {
    req_present = true;
    if ( sns_smgr_process_request(sensor_ptr, req_resp_ptr) == SNS_DDF_PENDING )
    {
      break; /* will process next request after this one completes */
    }
  }
  return req_present;
}

/*===========================================================================

  FUNCTION:   sns_smgr_turn_off_sensor

===========================================================================*/
/*!
  @brief Turns off the given sensor if it has no clients left.

  @param[io] sensor_ptr - the sensor to turn off

  @return
    true if the sensor is turned off
*/
/*=========================================================================*/
SMGR_STATIC bool sns_smgr_turn_off_sensor(sns_smgr_sensor_s* sensor_ptr)
{
  bool     sensor_turned_off = false;
  uint8_t  d;
  q16_t    max_freq = 0;
  uint32_t zero_odr = 0;

  SNS_SMGR_PRINTF3(
    HIGH, "turn_off_sensor - enter - sensor=%u state=%u flags=0x%x",
    SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state, sensor_ptr->flags);

  for ( d=0; d<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); d++ )
  {
    sns_smgr_ddf_sensor_s* ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[d];
    if ( ddf_sensor_ptr != NULL )
    {
      max_freq = MAX(max_freq,
                     ddf_sensor_ptr->client_stat.max_requested_freq_hz);

      if ( ddf_sensor_ptr->client_stat.max_requested_freq_hz == 0 &&
           ddf_sensor_ptr->current_odr > 0 )
      {
        sns_ddf_sensor_e sensor_type = 
          SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type);
        
        if ( ddf_sensor_ptr->sensor_ptr->odr_attr_supported )
        {
          sns_smgr_set_attr(
            ddf_sensor_ptr->sensor_ptr, sensor_type, SNS_DDF_ATTRIB_ODR, &zero_odr);
        }

        if ( SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) )
        {
          if ( !SMGR_SENSOR_FIFO_IS_SUPPORTED(sensor_ptr) ||
               !SMGR_SENSOR_FIFO_CAN_BE_USED(sensor_ptr) )
          {
            sns_smgr_update_ewur( 0, FX_CONV_Q16(ddf_sensor_ptr->current_odr,0), 
                                  sensor_ptr->is_uimg_refac, 
                                  !sns_smgr_ddf_sensor_is_event_sensor(ddf_sensor_ptr) );
            sns_smgr_dd_enable_sched_data(sensor_ptr, sensor_type, false);
          }
          else
          {
            sns_smgr_fifo_configure(sensor_ptr, false);
          }
        }
        else
        {
          sns_smgr_update_ewur( 0, FX_CONV_Q16(ddf_sensor_ptr->current_odr,0), 
                                sensor_ptr->is_uimg_refac, false );
        }
        sns_smgr_process_no_sample_event(ddf_sensor_ptr);
      }
    }
  }

  if ( max_freq == 0 )
  {
    if ( sensor_ptr->sensor_state != SENSOR_STATE_FAILED )
    {
      sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_IDLE);
    }

    // Prevent the sensor from going to LOWPOWER while a DAF request is active
    // Once the DAF request finishes, the DAF_INACTIVE event will bring the sensor to LOWPOWER
    if ( !sensor_ptr->daf_active )
    {
      sns_smgr_set_power_attr(sensor_ptr, SNS_DDF_POWERSTATE_LOWPOWER);
      sensor_turned_off = true;
    }
  }

  SNS_SMGR_PRINTF3(
    HIGH, "turn_off_sensor - exit  - sensor=%d state=%u flags=0x%x",
    SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state, sensor_ptr->flags);

  return sensor_turned_off;
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_wake_up_event

===========================================================================*/
/*!
  @brief Handles SENSOR_EVENT_WAKE_UP event for given sensor

  @Detail
    WAKE_UP event is only expected when sensor is in IDLE state.
    1. Initializes Read state for all data type
    2. Set device to Active power state
    3. Set device range
    4. Configures ODR
    5. Schedule sampling
    6. Update sensor state to CONFIGURING

  @param[i] sensor_ptr - the sensor leader structure

  @return
    true if event was processed
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_wake_up_event(sns_smgr_sensor_s* sensor_ptr)
{
  sns_ddf_status_e     driver_status;

  sns_smgr_update_power_state(sensor_ptr);

  smgr_sensor_type_read_state_to_idle(sensor_ptr);

  driver_status = sns_smgr_set_power_attr(sensor_ptr, SNS_DDF_POWERSTATE_ACTIVE);
  if ( SNS_DDF_SUCCESS == driver_status )
  {
    if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_ADD_B) )
    {
      uint32_t range_set = (uint32_t)sensor_ptr->const_ptr->sensitivity_default;
      SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_ADD_B);
      SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_DEL_B);
      sns_smgr_set_attr(sensor_ptr,
                        SMGR_SENSOR_TYPE(sensor_ptr,
                                         sensor_ptr->const_ptr->range_sensor),
                        SNS_DDF_ATTRIB_RANGE,
                        &range_set);
      sns_smgr_config_odr_lpf(sensor_ptr);
    }
    if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ENABLE_MD_B) )
    {
      SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ENABLE_MD_B);
      if ( sensor_ptr->sensor_state != SENSOR_STATE_CONFIGURING )
      {
        sns_smgr_update_md_attribute(sensor_ptr, true);
      }
      /* else, ignores MD request as sensor has a streaming client */
    }
    if ( sensor_ptr->sensor_state == SENSOR_STATE_IDLE &&
         !sensor_ptr->daf_active )
    {
      /* not in MD state, and not configuring to stream, and DAF not active */
      sns_smgr_turn_off_sensor(sensor_ptr);
    }
  }
  else
  {
    sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_FAILED);
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_process_flush_fifo_event

===========================================================================*/
/*!
  @brief Checks if RH requested FIFO to be flushed

  @param    sensor_ptr
 
  @return   true if RH task was signaled
*/
/*=========================================================================*/
SMGR_STATIC bool sns_smgr_process_flush_fifo_event(sns_smgr_sensor_s* sensor_ptr)
{
  uint8_t i, num_reports = 0;
  sns_rh_rpt_spec_s*  rpt_ptrs[SNS_SMGR_MAX_REPORT_CNT];

  if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_FLUSH_FIFO_B) )
  {
    for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
    {
      if ( sensor_ptr->ddf_sensor_ptr[i] != NULL )
      {
        sns_rh_rpt_item_s* item_ptr = 
          sensor_ptr->ddf_sensor_ptr[i]->client_stat.rpt_item_ptr;
        while ( item_ptr != NULL )
        {
          if ( item_ptr->parent_report_ptr->state == RH_RPT_STATE_FIFO_FLUSH_PENDING )
          {
            rpt_ptrs[num_reports++] = item_ptr->parent_report_ptr;
          }
          item_ptr = item_ptr->next_item_ptr;
        }
      }
    }
    SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_FLUSH_FIFO_B);
  }

  if ( num_reports > 0 )
  {
    sns_smgr_fifo_flush(sensor_ptr, false);

    for ( i=0; i<num_reports; i++ )
    {
      rpt_ptrs[i]->state = RH_RPT_STATE_FIFO_FLUSH_READY;
    }

    sns_rh_signal_me(SNS_RH_FIFO_FLUSHED_SIG);
    SNS_SMGR_PRINTF4(
      HIGH, "flush_fifo_event - sensor=%u #rpts=%u done_tick=%u flags=0x%x",
      SMGR_SENSOR_ID(sensor_ptr), num_reports, sensor_ptr->event_done_tick, 
      sensor_ptr->flags);
  }

  return (num_reports > 0) ? true : false;
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_client_update_events

===========================================================================*/
/*!
  @brief Checks if there's a change in sampling requests
 
  @param    sensor_ptr
 
  @return   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_client_update_events(sns_smgr_sensor_s* sensor_ptr)
{
  if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_ADD_B) ||
       SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_DEL_B) )
  {
    SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_ADD_B);
    SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_FIFO_WM_CHANGED_B);
    if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_DEL_B) )
    {
      SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_DEL_B);
      sns_smgr_turn_off_sensor(sensor_ptr);
    }
    if ( sensor_ptr->sensor_state != SENSOR_STATE_IDLE )
    {
      sns_smgr_config_odr_lpf(sensor_ptr);
    }
    SNS_SMGR_PRINTF3(
      HIGH, "client_update_events - sensor=%u state=%u flags=0x%x",
      SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state, sensor_ptr->flags);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_ready_state_events

===========================================================================*/
/*!
  @brief Handles sensor events when in READY/CONFIGURING state

  @param[io] sensor_ptr

  @return none
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_ready_state_events(sns_smgr_sensor_s* sensor_ptr)
{
  SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_DISABLE_MD_B);
  if ( !sns_smgr_process_flush_fifo_event(sensor_ptr) )
  {
    sns_smgr_process_odr_changed_event(sensor_ptr);
    sns_smgr_process_client_update_events(sensor_ptr);
    if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_FIRST_VALID_SAMPLE_B) ||
         SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_FIFO_WM_CHANGED_B) )
    {
      SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_FIRST_VALID_SAMPLE_B);
      if ( sensor_ptr->sensor_state == SENSOR_STATE_READY )
      {
        sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_CONFIGURING);
      }
    }
    if ( sensor_ptr->event_done_tick == 0 &&
         sensor_ptr->sensor_state == SENSOR_STATE_CONFIGURING )
    {
      sns_smgr_process_config_filter_done_event(sensor_ptr);
    }
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_process_idle_state_events

===========================================================================*/
/*!
  @brief Handles sensor events when in IDLE state

  @param[i] sensor_ptr - the sensor leader structure

  @return none
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_idle_state_events(sns_smgr_sensor_s* sensor_ptr)
{
  if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_ADD_B) ||
       SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ENABLE_MD_B) )
  {
    sns_smgr_process_wake_up_event(sensor_ptr);

    if ( sensor_ptr->event_done_tick == 0 &&
         sensor_ptr->sensor_state == SENSOR_STATE_CONFIGURING )
    {
      sns_smgr_process_config_filter_done_event(sensor_ptr);
    }
  }
  if ( sensor_ptr->flags != 0 )
  {
    SNS_SMGR_PRINTF1(
      ERROR, "idle_state_events - unexpected events=0x%x", sensor_ptr->flags);
    sensor_ptr->flags = 0;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_md_state_events

===========================================================================*/
/*!
  @brief Handles sensor events when in MD state

  @param[io] sensor_ptr

  @return none
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_md_state_events(sns_smgr_sensor_s* sensor_ptr)
{
  SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ENABLE_MD_B);
  if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_ADD_B) ||
       SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_DEL_B) )
  {
    SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_ADD_B);
    SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_DEL_B);
    if ( smgr_get_max_requested_freq(sensor_ptr) > 0 )
    {
      sns_smgr_update_md_attribute(sensor_ptr, false);
      sns_smgr_config_odr_lpf(sensor_ptr);
      if ( sensor_ptr->event_done_tick == 0 &&
           sensor_ptr->sensor_state == SENSOR_STATE_CONFIGURING )
      {
        sns_smgr_process_config_filter_done_event(sensor_ptr);
      }
    }
  }
  if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_DISABLE_MD_B) )
  {
    SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_DISABLE_MD_B);
    if ( sensor_ptr->sensor_state == SENSOR_STATE_MD )
    {
      sns_smgr_update_md_attribute(sensor_ptr, false);
    }
  }
  if ( sensor_ptr->flags != 0 )
  {
    SNS_SMGR_PRINTF1(
      ERROR, "md_state_events - unexpected events=0x%x", sensor_ptr->flags);
    sensor_ptr->flags = 0;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_testing_state_events

===========================================================================*/
/*!
  @brief Handles sensor events when in TESTING state

  @param[io] sensor_ptr

  @return none
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_testing_state_events(sns_smgr_sensor_s* sensor_ptr)
{
  if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_SELF_TEST_DONE_B) )
  {
    SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_SELF_TEST_DONE_B);
    sns_rh_signal_me(SNS_RH_SELF_TEST_DONE_SIG);
    sns_smgr_set_sensor_state(sensor_ptr, sns_smgr.self_test.pre_test_state);
    sns_smgr.self_test.self_test_req  = NULL;
    sns_smgr.self_test.ddf_sensor_ptr = NULL;
    sns_smgr_update_stay_in_bigimage();
    if ( sensor_ptr->sensor_state == SENSOR_STATE_IDLE )
    {
      sns_smgr_reset_sensor(sensor_ptr);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_failed_state_events

===========================================================================*/
/*!
  @brief Handles sensor events when in FAILED state

  @param[io] sensor_ptr

  @return none
*/
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_failed_state_events(sns_smgr_sensor_s* sensor_ptr)
{
  if ( sensor_ptr->flags != 0 )
  {
    sns_smgr_turn_off_sensor(sensor_ptr);
    sensor_ptr->flags = 0;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_individual_sensor_events

===========================================================================*/
/*!
  @brief Processes events for the given sensor
 
  @param    sensor_ptr
 
  @return   none
 */
/*=========================================================================*/
SMGR_STATIC void sns_smgr_process_individual_sensor_events(sns_smgr_sensor_s* sensor_ptr)
{
  SNS_SMGR_PRINTF4(
    MED, "sensor_event - enter - sensor=%2u state/flags=0x%05x #req=%u tick=%u",
    SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state<<16 |sensor_ptr->flags,
    sns_q_cnt(&sensor_ptr->request_queue.protected_q), sensor_ptr->event_done_tick);

  switch ( sensor_ptr->sensor_state )
  {
  case SENSOR_STATE_OFF:
    sns_smgr_process_powering_up_event(sensor_ptr);
    break;
  case SENSOR_STATE_POWERING_UP:
    if ( sensor_ptr->event_done_tick != 0 )
    {
      break; /* needs to wait off-to-idle duration */
    }
    sns_smgr_reset_sensor(sensor_ptr);
    if ( sensor_ptr->sensor_state != SENSOR_STATE_IDLE )
    {
      break; /* sensor fails to reset */
    }
    /* falling through */
  case SENSOR_STATE_IDLE:
    if ( sensor_ptr->event_done_tick == 0 )
    {
      sns_smgr_process_idle_state_events(sensor_ptr);
    }
    break;
  case SENSOR_STATE_CONFIGURING:
  case SENSOR_STATE_READY:
    sns_smgr_process_ready_state_events(sensor_ptr);
    break;
  case SENSOR_STATE_MD:
    sns_smgr_process_md_state_events(sensor_ptr);
    break;
  case SENSOR_STATE_TESTING:
    sns_smgr_process_testing_state_events(sensor_ptr);
    break;
  case SENSOR_STATE_FAILED:
    sns_smgr_process_failed_state_events(sensor_ptr);
    break;
  default:
    SNS_SMGR_PRINTF0(ERROR, "sensor_event - why am i here???");
    break;
  }

  if ( sensor_ptr->event_done_tick == 0 )
  {
    sns_smgr_check_request_queue(sensor_ptr);
  }

  SNS_SMGR_PRINTF4(
    MED, "sensor_event -  exit - sensor=%2u state/flags=0x%05x #req=%u tick=%u",
    SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state<<16 |sensor_ptr->flags,
    sns_q_cnt(&sensor_ptr->request_queue.protected_q), sensor_ptr->event_done_tick);

  if ( sensor_ptr->event_done_tick == 0 )
  {
    sns_smgr_update_power_state(sensor_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_reboot_sensor

===========================================================================*/
/*!
  @brief Reboots the given sensor whose heartbeat must have stopped

  @param[i] ddf_sensor_ptr - the sensor to be revived

  @return none
*/
/*=========================================================================*/
void sns_smgr_reboot_sensor(sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_smgr_sensor_s* sensor_ptr = ddf_sensor_ptr->sensor_ptr;
  sns_ddf_sensor_e sensor = SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type);

  if ( SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) )
  {
    sns_smgr_dd_enable_sched_data(sensor_ptr, sensor, false);
  }
  if ( sensor_ptr->odr_attr_supported )
  {
    ddf_sensor_ptr->requested_odr = 0;
    sns_smgr_set_attr(sensor_ptr, sensor, SNS_DDF_ATTRIB_ODR, 
                      (void*)&ddf_sensor_ptr->requested_odr);
  }

  if ( sensor_ptr->odr_change_tick != 0 )
  {
    sns_smgr_set_power_attr(sensor_ptr, SNS_DDF_POWERSTATE_LOWPOWER);
    sns_smgr_reset_sensor(sensor_ptr);
    sensor_ptr->num_reboots++;
  }
  if ( sensor_ptr->sensor_state == SENSOR_STATE_IDLE )
  {
    uint16_t action = 0;
    if ( smgr_get_max_requested_freq(sensor_ptr) > 0 )
    {
      action = SMGR_SENSOR_FLAGS_ITEM_ADD_B;
    }
    else
    {
      action = SMGR_SENSOR_FLAGS_ITEM_DEL_B;
    }
    sensor_ptr->flags = 0;
    sns_os_mutex_pend(sensor_ptr->mutex_ptr, 0, NULL);
    SMGR_BIT_SET(sensor_ptr->flags, action);
    sns_os_mutex_post(sensor_ptr->mutex_ptr);
    sns_smgr_signal_me(SNS_SMGR_SENSOR_STATE_SIG);
  }
  SNS_SMGR_PRINTF3(
     HIGH, "reboot_sensor - sensor=%u state=%u flags=0x%x",
     SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state, sensor_ptr->flags);
}

/*===========================================================================

  FUNCTION:   sns_smgr_schedule_sensor_event

===========================================================================*/
/*!
  @brief Schedules the next sensor event processing
 
  @details Sets timer for the given offset, or triggers sensor state machine 
           immediately if offset is too short for timer
           
  @param[i] tick_offset - wait time (in ticks)

  @return none
*/
/*=========================================================================*/
void sns_smgr_schedule_sensor_event(uint32_t tick_offset)
{
  if ( tick_offset <= SMGR_TOO_SHORT_TICKS_FOR_TIMER )
  {
    sns_smgr_signal_me(SNS_SMGR_SENSOR_STATE_SIG);
  }
  else
  {
    sns_err_code_e status = SNS_ERR_FAILED;
    uint8_t i;

    sns_em_cancel_utimer(sns_smgr.sensor_ev_tmr_obj);
    for ( i=0; i<100 && status != SNS_SUCCESS; i++ )
    {
      status = sns_em_register_utimer(sns_smgr.sensor_ev_tmr_obj, tick_offset);
    }
    if ( status == SNS_SUCCESS )
    {
      SNS_SMGR_PRINTF2(LOW, "sched_sensor_ev - offset=%u #tries=%u", tick_offset, i);
    }
    else
    {
      SNS_SMGR_PRINTF0(ERROR, "sched_sensor_ev - register_timer failed");
    }
  }
}

/*===========================================================================

  FUNCTION:   smgr_process_sensor_event

===========================================================================*/
/*!
  @brief Checks each sensor for event to process.

  @param none

  @return none
*/
/*=========================================================================*/
void smgr_process_sensor_event(void)
{
  uint8_t   i;
  uint8_t   event_count = 0;
  uint32_t  next_tick_offset = SMGR_MAX_TICKS;

  sns_smgr_set_stay_in_big_image(true);
  sns_smgr_update_last_tick();

  SNS_SMGR_PRINTF2(HIGH, "sensor_event - time now=%u/0x%x", 
                   sns_smgr.last_tick, sns_smgr.last_tick);

  for ( i=0; i<ARR_SIZE(sns_smgr.sensor); i++ )
  {
    sns_smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[i];
    if ( sensor_ptr->mutex_ptr != NULL )
    {
      bool times_up = false;

      sns_os_mutex_pend(sensor_ptr->mutex_ptr, 0, NULL);

      if ( sensor_ptr->event_done_tick != 0 &&
           TICK1_GEQ_TICK2(sns_smgr.last_tick, sensor_ptr->event_done_tick) )
      {
        times_up = true;
        sensor_ptr->event_done_tick = 0;
      }
      if ( times_up || (sensor_ptr->flags != 0) ||
           (sns_q_cnt(&sensor_ptr->request_queue.protected_q) > 0) )
      {
        sns_smgr_process_individual_sensor_events(sensor_ptr);
      }
      if ( sensor_ptr->event_done_tick != 0 )
      {
        SNS_SMGR_PRINTF3(
          LOW, "sensor_event - sensor=%u state=%u tick=%u",
          SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state, 
          sensor_ptr->event_done_tick);

        event_count++;
        next_tick_offset = 
          MIN(next_tick_offset, (sensor_ptr->event_done_tick - sns_smgr.last_tick));
      }
      else if ( sensor_ptr->flags != 0 )
      {
        if ( sensor_ptr->sensor_state == SENSOR_STATE_IDLE )
        {
          next_tick_offset = 1; /* triggers state machine to run again ASAP */
        }
        else
        {
          SNS_SMGR_PRINTF3(
            ERROR, "sensor_event - sensor=%u state=%u flags=0x%x",
            SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state, sensor_ptr->flags);
        }
      }

      sns_os_mutex_post(sensor_ptr->mutex_ptr);
    }
  }

  SNS_SMGR_PRINTF3(
    HIGH, "sensor_event - count=%d next_tick_offset=%u(0x%x)", 
    event_count, next_tick_offset, next_tick_offset);

  if ( next_tick_offset != SMGR_MAX_TICKS )
  {
    sns_smgr_schedule_sensor_event(next_tick_offset);
  }
  else
  {
    sns_smgr_set_stay_in_big_image(false);
  }
}

