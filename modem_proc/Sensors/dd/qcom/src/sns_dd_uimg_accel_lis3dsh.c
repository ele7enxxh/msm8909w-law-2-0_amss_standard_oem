/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
 *
 * File Name         : lis3dsh_ddf_drv.c
 * Authors           : Jianjian(Samuel) Huo
 *                   : karimuddin Sayed
 * Version           : V 3.6.2
 * Date              : 03/13/2014
 * Description       : LIS3DSH Accelerometer driver source file
 *
 ********************************************************************************
 * Copyright (c) 2013, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the STMicroelectronics nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 * REVISON HISTORY
 *
 * VERSION | DATE          | DESCRIPTION
 * 1.0.0   | 06/05/2014    | Created, Refactored for uImage
*******************************************************************************/

/*******************************************************************************
* Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
*******************************************************************************/

/*=============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  07/20/2014  VY   Disabled logging in uImage
  06/11/2014  vy   Refactored for uImage
  03/24/2014  tc   Added support for the Driver Access Framework
  01/12/2014  pn   Flushes FIFO when FIFO interrupt is missed
  11/18/2013  pn   - BW timer replaced with simple timestamp
                   - Marks invalid FIFO data produced before new ODR is settled
  10/25/2013  vh   Synced with STM's version number
  10/22/2013  vh   Added Qualcomm Copyright
=============================================================================*/

#include "sns_dd_accel_lis3dsh.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd_inertial_test.h"
#include "sns_ddf_signal.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define CONFIG_OEM

#if STM_LIS3DSH_LOGDATA
//Include files required to support accel log packet
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#endif

#define STM_LIS3DSH_NUM_READ_BYTES  6
#define AUTO_INCREMENT             0x80

sns_ddf_driver_if_s sns_dd_acc_lis3dsh_if;

/*------------------------------------------------------
  Access to externalized functions.
  ------------------------------------------------------*/

extern sns_ddf_status_e sns_dd_acc_lis3dsh_init(
    sns_ddf_handle_t* dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors);

extern sns_ddf_status_e sns_dd_acc_lis3dsh_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems);

extern sns_ddf_status_e sns_dd_acc_lis3dsh_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value);

extern sns_ddf_status_e sns_dd_acc_lis3dsh_reset(
    sns_ddf_handle_t dd_handle);

extern sns_ddf_status_e sns_dd_acc_lis3dsh_run_test(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_test_e    test,
    uint32_t*         err);

extern sns_ddf_status_e sns_dd_acc_lis3dsh_process_daf_req(
    sns_ddf_handle_t       dd_handle,
    uint32_t               req_id,
    const void*            req_msg,
    uint32_t               req_size,
    sns_ddf_memhandler_s*  memhandler,
    void**                 resp_msg,
    uint32_t*              resp_size,
    const uint8_t*         trans_id_ptr,
    void*                  conn_handle);

extern void sns_dd_acc_lis3dsh_cancel_daf_trans(
     sns_ddf_handle_t       dd_handle,
     void*                  conn_handle);	

extern volatile boolean EnableI2C; 
	 
static sns_ddf_status_e sns_dd_acc_lis3dsh_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data);

static void sns_dd_acc_lis3dsh_handle_irq(
    sns_ddf_handle_t  handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp);

static sns_ddf_status_e sns_dd_acc_lis3dsh_enable_sched(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable);

static sns_ddf_status_e sns_dd_acc_lis3dsh_trigger_fifo(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    uint16_t num_samples,
    bool trigger_now);

static sns_ddf_status_e sns_dd_acc_lis3dsh_send_dbt_event(
    sns_ddf_handle_t dd_handle);	

#if STM_LIS3DSH_LOGDATA
static void sns_dd_acc_lis3dshaccel_log_data(sns_ddf_sensor_data_s *accel_data_ptr);

static void sns_dd_acc_lis3dshaccel_log_fifo(sns_ddf_sensor_data_s *accel_data_ptr);
#endif
	
static sns_ddf_status_e sns_dd_acc_lis3dsh_enable_fifo_int(
    sns_ddf_handle_t dd_handle);

static sns_ddf_status_e sns_dd_acc_lis3dsh_disable_fifo_int(
    sns_ddf_handle_t dd_handle);

static void sns_dd_acc_lis3dsh_catch_missed_interrupt(
  sns_dd_acc_lis3dsh_state_t* state);

static sns_ddf_status_e sns_dd_accel_lis3dsh_parse_and_send_fifo_data(
   sns_dd_acc_lis3dsh_state_t* state,
   sns_ddf_handle_t            smgr_handle,
   const uint8_t*              raw_buf_ptr,
   uint16_t                    num_samples);

sns_ddf_status_e lis3dsh_search_subdev_by_sensortype (
    sns_dd_acc_lis3dsh_state_t* state,
    sns_ddf_sensor_e sensor,
    uint32_t *sub_dev_inx);

sns_ddf_status_e sns_dd_acc_lis3dsh_send_fifo_data(
    sns_ddf_handle_t dd_handle,
    uint16_t num_samples,
    sns_ddf_time_t    timestamp);

uint8_t sns_dd_acc_lis3dsh_get_fifo_sample_count(
  sns_dd_acc_lis3dsh_state_t* state);

sns_ddf_status_e sns_dd_acc_lis3dsh_update_onereg(
    sns_ddf_handle_t dd_handle,
    uint8_t reg_addr,
    uint8_t mask,
    uint8_t new_value);
  
sns_ddf_status_e sns_dd_acc_lis3dsh_enable_dbt_int(
    sns_ddf_handle_t dd_handle);
	
sns_ddf_status_e sns_dd_acc_lis3dsh_disable_dbt(
    sns_ddf_handle_t dd_handle);
  
/**
 * LIS3DSH device driver interface.
 */
sns_ddf_driver_if_s sns_dd_acc_lis3dsh_if =
{
  &sns_dd_acc_lis3dsh_init,
  &sns_dd_acc_lis3dsh_get_data,
  &sns_dd_acc_lis3dsh_set_attr,
  &sns_dd_acc_lis3dsh_get_attr,
  NULL,  // handle_timer
  &sns_dd_acc_lis3dsh_handle_irq,
  &sns_dd_acc_lis3dsh_reset,
  &sns_dd_acc_lis3dsh_run_test,
  &sns_dd_acc_lis3dsh_enable_sched,
  NULL,  // probe
  &sns_dd_acc_lis3dsh_trigger_fifo,
  .process_daf_req = &sns_dd_acc_lis3dsh_process_daf_req,
  .cancel_daf_trans = &sns_dd_acc_lis3dsh_cancel_daf_trans
};


/**
 * @brief Called in response to an motion detection interrupt for this driver.
 *
 * @param[in]  dd_handle    Handle to a driver instance.
 * @param[in]  gpio_num     GPIO number that generated the interrupt.
 * @param[in]  timestamp    Time at which ISR is called
 *
 * @return none
 * */

static void sns_dd_acc_lis3dsh_handle_irq(
    sns_ddf_handle_t  handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp)
{
  sns_dd_acc_lis3dsh_state_t* state=NULL;
  sns_ddf_status_e status;
  uint8_t rw_buffer[2] = {0, 0};
  uint8_t rw_buffer_temp = 0;
  uint8_t rw_bytes = 0;
  uint32_t sub_dev_inx = 0;
  uint8_t total_in_fifo = 0;
  //flag for the while loop to guarantee no MD/DBT interrupt get dropped during fifo interrupt.
  bool run_again = true;
  //how many times fifo_read happens
  uint8_t fifo_run_cnt = 0;
  sns_ddf_time_t entry_time;

  if(handle==NULL)
  {
    LIS3DSH_MSG_0(HIGH, "handle_irq: Received Null Pointer.");
    return;
  }
  entry_time = sns_ddf_get_timestamp();

  LIS3DSH_MSG_1(HIGH, "handle_irq: CPU IRQ time=%u", timestamp);
  LIS3DSH_MSG_1(HIGH, "handle_irq: DD ISR start time=%u", entry_time);

  state = (sns_dd_acc_lis3dsh_state_t*)((dd_handle_align_t)handle & DD_HANDLE_ALIGN_BITS);

  if (gpio_num != state->gpio_num)
    return;

  status = lis3dsh_search_subdev_by_sensortype(
      state,
      SNS_DDF_SENSOR_ACCEL,
      &sub_dev_inx);
  if(status != SNS_DDF_SUCCESS)
    return;

  while(run_again)
  {
    run_again = false;

    if(fifo_run_cnt > 0)
    {
         timestamp = sns_ddf_get_timestamp();
    }

    // verify source of interrupts by reading INT1_SRC and FIFO_SRC.
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LIS3DSH_INT_STAT,
        rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return;
    if(rw_bytes != 1)
      return;

    status = sns_ddf_read_port(
        state->port_handle,
        STM_LIS3DSH_FIFO_SRC,
        rw_buffer+1,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return;
    if(rw_bytes != 1)
      return;

    LIS3DSH_MSG_2(HIGH, "handle_irq: INT1_SRC=%u, FIFO_SRC=%u", rw_buffer[0], rw_buffer[1]);

    //FIFO overflow interrupt handling.
    if((rw_buffer[1] & 0x40) && (fifo_run_cnt==0))
    {
      LIS3DSH_MSG_2(HIGH, "handle_irq: to send overflow event, watermark=%u, timestamp=%u",
          state->fifo_wmk, timestamp);

      // send a FIFO overflow event to SMGR
      status = sns_ddf_smgr_notify_event(
          state->sub_dev[sub_dev_inx].smgr_handle,
          SNS_DDF_SENSOR_ACCEL,
          SNS_DDF_EVENT_FIFO_OVERFLOW);
      if(status != SNS_DDF_SUCCESS)
        return;

      fifo_run_cnt ++;
      run_again = true;
    }

    //FIFO watermark interrupt(without overflow flag) handling
    if((0x0 == (rw_buffer[1] & 0x40)) && (rw_buffer[1] & 0x80))
    {
      total_in_fifo = rw_buffer[1] & 0x1F;
      LIS3DSH_MSG_1(HIGH, "handle_irq: WM INT happenes. total_in_fifo=%u", total_in_fifo);

      //if there is no data in FIFO.
      if(0 == total_in_fifo)
        return;

      // send a FIFO Watermark event to SMGR
      status = sns_ddf_smgr_notify_event(
          state->sub_dev[sub_dev_inx].smgr_handle,
          SNS_DDF_SENSOR_ACCEL,
          SNS_DDF_EVENT_FIFO_WM_INT);
      if(status != SNS_DDF_SUCCESS)
        return;

      // if more samples than fifo_wmk, recalcuate the end timestamp of last sample.
      if((total_in_fifo > state->fifo_wmk) && (!fifo_run_cnt))
      {
        sns_ddf_time_t interrupt_timestamp = timestamp;
        timestamp = state->last_timestamp +
                    ((timestamp - state->last_timestamp)/state->fifo_wmk) * total_in_fifo;

        if(timestamp > entry_time)
          timestamp = entry_time;

        MSG_3(MSG_SSID_SNS, DBG_HIGH_PRIO, 
              "handle_irq - #in_fifo=%u interrupt_ts=%u recomputed_ts=%u", 
              total_in_fifo, interrupt_timestamp, timestamp);
      }

      LIS3DSH_MSG_2(HIGH, "handle_irq: to call send_fifo_data, watermark=%u, real_timestamp=%u",
          state->fifo_wmk, timestamp);

      // send all data samples in FIFO to SMGR.
      status = sns_dd_acc_lis3dsh_send_fifo_data(state,
          total_in_fifo,
          timestamp);
      if(status != SNS_DDF_SUCCESS)
        return;

      fifo_run_cnt ++;
      run_again = true;
    }

    //double tap interrupt handling.
    if((rw_buffer[0] & 0x04))
    {
      status = sns_dd_acc_lis3dsh_send_dbt_event(state);
      if(status != SNS_DDF_SUCCESS)
        return;

      //run_again = true;
    }

    //motion detection interrupt handling.
    if((rw_buffer[0] & 0x08))
    {
      //read OUTS1 register to reset the interrupt
      sns_ddf_read_port(
          state->port_handle,
          STM_LIS3DSH_OUTS1,
          &rw_buffer_temp,
          1,
          &rw_bytes);

      // motion detected, send an event to SMGR
      status = sns_ddf_smgr_notify_event(
          state->sub_dev[sub_dev_inx].smgr_handle,
          SNS_DDF_SENSOR_ACCEL,
          SNS_DDF_EVENT_MOTION);
      if(status != SNS_DDF_SUCCESS)
        return;

      //run_again = true;
    }

    //special case, FIFO DRI interrupt handling for watermark 1.
    if((rw_buffer[0] & 0x01) && (state->fifo_enabled)
      && (state->fifo_int_enabled) && (1==state->fifo_wmk))
    {
      // send a FIFO Watermark event to SMGR
      status = sns_ddf_smgr_notify_event(
          state->sub_dev[sub_dev_inx].smgr_handle,
          SNS_DDF_SENSOR_ACCEL,
          SNS_DDF_EVENT_FIFO_WM_INT);
      if(status != SNS_DDF_SUCCESS)
        return;

      LIS3DSH_MSG_2(HIGH, "handle_irq: to call send_fifo_data, watermark=%u, timestamp=%u",
          state->fifo_wmk, timestamp);

      status = sns_dd_acc_lis3dsh_send_fifo_data(state,
          state->fifo_wmk,
          timestamp);
      if(status != SNS_DDF_SUCCESS)
        return;

      //run_again = true;
    }

  }
/*
  //test only start
  status = sns_ddf_read_port(
      state->port_handle,
      STM_LIS3DSH_INT_STAT,
      rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return;
  if(rw_bytes != 1)
    return;

  status = sns_ddf_read_port(
      state->port_handle,
      STM_LIS3DSH_FIFO_SRC,
      rw_buffer+1,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return;
  if(rw_bytes != 1)
    return;

	LIS3DSH_MSG_2(HIGH, "handle_irq END: INT1_SRC=%u, FIFO_SRC=%u", rw_buffer[0], rw_buffer[1]);
  //test only end

  LIS3DSH_MSG_1(HIGH, "handle_irq: DD ISR end time=%u", sns_ddf_get_timestamp());
 */
}


/**
 * @brief Retrieves a single set of sensor data from lis3dsh.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data)
{
  uint8_t read_buffer[STM_LIS3DSH_NUM_READ_BYTES] = { 0,0,0,0,0,0 };
  uint8_t read_count = 0;
  sns_ddf_time_t timestamp;
  sns_ddf_status_e status;
  sns_ddf_sensor_data_s *data_ptr;
  sns_dd_acc_lis3dsh_state_t *state;
  // sensor coordinate x,y,z axis raw hardware data
  int16_t hw_d[STM_LIS3DSH_NUM_AXES] = { 0, 0, 0 };
  // SAE coordinate x,y,z axis data
  q16_t sae_d[STM_LIS3DSH_NUM_AXES] = {0, 0, 0 };

  state = (sns_dd_acc_lis3dsh_state_t*)((uint32_t)dd_handle & DD_HANDLE_ALIGN_BITS);

  // This is a synchronous driver, so try to read data now.
  status = sns_ddf_read_port(
      state->port_handle,
      AUTO_INCREMENT | STM_LIS3DSH_OUT_X_L_A,
      read_buffer,
      STM_LIS3DSH_NUM_READ_BYTES,
      &read_count);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(read_count != STM_LIS3DSH_NUM_READ_BYTES)
    return SNS_DDF_EBUS;

  // get current time stamp
  timestamp = sns_ddf_get_timestamp();

  //convert the raw data in read_buffer to X/Y/Z axis sensor data
  hw_d[0] = (int16_t) (((read_buffer[1]) << 8) | read_buffer[0]);
  hw_d[1] = (int16_t) (((read_buffer[3]) << 8) | read_buffer[2]);
  hw_d[2] = (int16_t) (((read_buffer[5]) << 8) | read_buffer[4]);

  //adjust sensor data per current sensitivity, and convert to Q16
  sae_d[0] = FX_FLTTOFIX_Q16(hw_d[0] * state->sstvt_adj * G/1000000);
  sae_d[1] = FX_FLTTOFIX_Q16(hw_d[1] * state->sstvt_adj * G/1000000);
  sae_d[2] = FX_FLTTOFIX_Q16(hw_d[2] * state->sstvt_adj * G/1000000);

  //map the sensor data to the phone's coordinates
  sns_ddf_map_axes(&state->axes_map, sae_d);

  //allocate memory for sns_ddf_sensor_data_s data structure
  data_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_data_s));
  if(NULL == data_ptr)
  {
    return SNS_DDF_ENOMEM;
  }
  *data = data_ptr;

  data_ptr->sensor = sensors[0];
  data_ptr->status = SNS_DDF_SUCCESS;
  data_ptr->timestamp = timestamp;

  //allocate memory for data samples.
  data_ptr->samples = sns_ddf_memhandler_malloc(memhandler, STM_LIS3DSH_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
  if(NULL == data_ptr->samples)
  {
    return SNS_DDF_ENOMEM;
  }

  //Fill out sensor output data and status
  //Axis and polarity conversion are configured in the header file.
  data_ptr->samples[0].sample = sae_d[0];
  data_ptr->samples[1].sample = sae_d[1];
  data_ptr->samples[2].sample = sae_d[2];
  data_ptr->num_samples = STM_LIS3DSH_NUM_AXES;

  // If the timer is still pending, invalidate the data
  if((int32_t)(state->odr_settled_ts - data_ptr->timestamp) > 0)
  {
    LIS3DSH_MSG_1(HIGH, "LIS3DSH - Invalidating sample at time: %u", data_ptr->timestamp);
    // Note: the status values for each samples[i] is UNUSED
    data_ptr->samples[0].status = SNS_DDF_EINVALID_DATA;
    data_ptr->samples[1].status = SNS_DDF_EINVALID_DATA;
    data_ptr->samples[2].status = SNS_DDF_EINVALID_DATA;
    data_ptr->status = SNS_DDF_EINVALID_DATA;
  }
  else
  {
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->samples[2].status = SNS_DDF_SUCCESS;
  }

  if (EnableI2C == false)
  {
     static uint32 dummyLis3dshData = 1;
     data_ptr->samples[0].sample = dummyLis3dshData++;
     data_ptr->samples[1].sample = dummyLis3dshData++;
     data_ptr->samples[2].sample = dummyLis3dshData++;
  }

  LIS3DSH_MSG_3(HIGH, "LIS3DSH time=%u \tget_data=s0(%d), s1(%d)",
      data_ptr->timestamp, data_ptr->samples[0].sample, data_ptr->samples[1].sample);
  /*
     LIS3DSH_MSG_3(HIGH, "LIS3DSH get_data=s0(%d), s1(%d), s2(%d)",
     data_ptr->samples[0].sample, data_ptr->samples[1].sample,
     data_ptr->samples[2].sample);
     */
#if STM_LIS3DSH_LOGDATA
  sns_dd_acc_lis3dshaccel_log_data(data_ptr);
#endif

  return SNS_DDF_SUCCESS;
}


/**
 * @brief Implement enable_sched_data() DDF API.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_enable_sched(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable)
{
//  sns_ddf_status_e status;
  sns_dd_acc_lis3dsh_state_t *state;

  state = (sns_dd_acc_lis3dsh_state_t*)((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);

  LIS3DSH_MSG_2(HIGH, "enable_sched_data: sensor=%u, enable=%u", sensor, enable);

  //if(SNS_DDF_SENSOR_DOUBLETAP != sensor)
  //  return SNS_DDF_EINVALID_PARAM;

  //each called function will handle concurrencies when to enable/disable FIFO int or dbt int.

  switch(sensor)
  {
    case SNS_DDF_SENSOR_DOUBLETAP:
      {
        if(enable)
        {
          return sns_dd_acc_lis3dsh_enable_dbt_int(state);

        }else{

          return sns_dd_acc_lis3dsh_disable_dbt(state);
        }
        break;
      }

    case SNS_DDF_SENSOR_ACCEL:
      {
        if(enable)
        {
          return sns_dd_acc_lis3dsh_enable_fifo_int(state);

        }else{

          return sns_dd_acc_lis3dsh_disable_fifo_int(state);
        }

        break;
      }
    default:
      return SNS_DDF_EINVALID_ATTR;
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Implement trigger_fifo_data API.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_trigger_fifo(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    uint16_t num_samples,
    bool trigger_now)
{
  sns_dd_acc_lis3dsh_state_t *state;
  uint8_t total_in_fifo = 0;
  uint8_t total_to_read = 0;
  sns_ddf_time_t time;

  state = (sns_dd_acc_lis3dsh_state_t*)((uint32_t)dd_handle & DD_HANDLE_ALIGN_BITS);

  LIS3DSH_MSG_3(HIGH, "trigger_fifo: #samples=%u now=%u sensor=%u",
                num_samples, trigger_now, sensor);

  if((SNS_DDF_SENSOR_ACCEL != sensor) || (num_samples > STM_LIS3DSH_MAX_FIFO))
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if(!trigger_now || !state->fifo_enabled)
  {
    return SNS_DDF_SUCCESS;
  }

  total_in_fifo = sns_dd_acc_lis3dsh_get_fifo_sample_count(state);
  if(0 == total_in_fifo)
  {
    return SNS_DDF_SUCCESS;
  }

  time = sns_ddf_get_timestamp();

  if(0 == num_samples)
  {
    total_to_read = total_in_fifo;
  }
  else
  {
    total_to_read = MIN(num_samples, total_in_fifo);
    total_to_read = MIN(total_to_read, state->fifo_wmk);

    //rare case: fifo has more data samples than what SMGR wants,
    //then recalculate end_timestamp
    if(total_to_read < total_in_fifo)
    {
      time = state->last_timestamp +
        total_to_read * (time - state->last_timestamp)/total_in_fifo;
    }
  }

  LIS3DSH_MSG_2(HIGH, "trigger_fifo: total_to_read=%u time=%u",
        total_to_read, time);

  return sns_dd_acc_lis3dsh_send_fifo_data(state, total_to_read, time);
}

/**
 * @brief To search which sub device has a specific sensor.
 *
 * @param[in]  state               - Handle to a driver instance.
 * @param[in]  sensor            - the sensor type.
 * @param[out]  sub_dev_inx  - the index of sub_dev which contains this sensor.
 *
 * @return SNS_DDF_SUCCESS if this sensor is found. Otherwise return SNS_DDF_EFAIL.
 */
sns_ddf_status_e lis3dsh_search_subdev_by_sensortype (
    sns_dd_acc_lis3dsh_state_t* state,
    sns_ddf_sensor_e sensor,
    uint32_t *sub_dev_inx)
{
  uint32_t i, j;
  stm_lis3dsh_sub_dev_s *sub_dev_slot_ptr;

  for (i =0; i < LIS3DSH_MAX_SUB_DEV; i++)
  {
    sub_dev_slot_ptr = &state->sub_dev[i];
    for (j =0; j < sub_dev_slot_ptr->num_sensors; j++)
    {
      if(sensor == sub_dev_slot_ptr->sensors[j])
      {
        *sub_dev_inx = i;
        return SNS_DDF_SUCCESS;
      }
    }
  }

  return SNS_DDF_EFAIL;
}

/**
 * @brief To send FIFO data to SMGR, numbers of sample up to watermark level.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  num_samples Num of data samples in FIFO to read.
 * @param[in]  timestamp      The end timestamp.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_send_fifo_data(
    sns_ddf_handle_t dd_handle,
    uint16_t num_samples,
    sns_ddf_time_t    timestamp)
{
  sns_dd_acc_lis3dsh_state_t* state = 
    (sns_dd_acc_lis3dsh_state_t*)((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t read_count = 0;
  uint16_t num_invalid_samples;
  uint32_t sub_dev_inx = 0;
  uint8_t* raw_buf_ptr = state->fifo_raw_buffer;

  // read all FIFO data up to num_samples.
  status = sns_ddf_read_port(
      state->port_handle,
      AUTO_INCREMENT | STM_LIS3DSH_OUT_X_L_A,
      state->fifo_raw_buffer,
      STM_LIS3DSH_NUM_READ_BYTES * num_samples,
      &read_count);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(read_count != STM_LIS3DSH_NUM_READ_BYTES * num_samples)
  {
    LIS3DSH_MSG_2(HIGH, "send_fifo_data: raw data reading fails, num_samples_to_read=%u, actual_read_bytes=%u",
        num_samples, read_count);
    return SNS_DDF_EBUS;
  }

  /*if(1 == state->fifo_wmk)
    {
    sns_ddf_read_port(
    state->port_handle,
    AUTO_INCREMENT | STM_LIS3DSH_OUT_X_L_A,
    (uint8_t *)hw_d,
    STM_LIS3DSH_NUM_READ_BYTES,
    &read_count);
    }*/

  //search for accel sensor in sub devices
  status = lis3dsh_search_subdev_by_sensortype(
      state,
      SNS_DDF_SENSOR_ACCEL,
      &sub_dev_inx);
  if(status != SNS_DDF_SUCCESS)
  {
     LIS3DSH_MSG_1(HIGH, "sns_dd_acc_lis3dsh_send_fifo_data:lis3dsh_search_subdev_by_sensortype status==%u ",status );
    return status;
   }
   
  // fill out fifo_data structure
  if(0 == state->last_timestamp)
  {
    LIS3DSH_MSG_0(HIGH, "sns_dd_acc_lis3dsh_send_fifo_data:last_timestamp==0 ");
    return SNS_DDF_EFAIL;
  }
  
  state->fifo_data.sensor = SNS_DDF_SENSOR_ACCEL;
  state->fifo_data.end_timestamp = timestamp;
  state->fifo_data.timestamp = state->last_timestamp +
    (timestamp - state->last_timestamp)/num_samples;

  LIS3DSH_MSG_2(HIGH, "send_fifo_data: start_timestamp=%u, end_timestamp=%u",
      state->fifo_data.timestamp, state->fifo_data.end_timestamp);

  if ( (int32_t)(state->odr_settled_ts - state->fifo_data.timestamp) <= 0 )
  {
    /* all samples in FIFO produced after ODR change settled */
    num_invalid_samples = 0;
  }
  else if ( (int32_t)(state->odr_settled_ts - state->fifo_data.end_timestamp) > 0 )
  {
    /* all samples in FIFO produced before ODR change settled */
    num_invalid_samples = num_samples;
  }
  else /* some samples in FIFO produced before ODR change settled */
  {
    uint32_t time_interval = state->fifo_data.timestamp - state->last_timestamp;
    num_invalid_samples = 
      ((state->odr_settled_ts - state->fifo_data.timestamp) / time_interval) + 1;
    state->fifo_data.end_timestamp = 
      state->last_timestamp + (time_interval * num_invalid_samples);
    state->last_timestamp = state->fifo_data.end_timestamp;
  }

  if ( num_invalid_samples > 0 )
  {
    state->fifo_data.status      = SNS_DDF_EINVALID_DATA;
    state->fifo_data.num_samples = STM_LIS3DSH_NUM_AXES * num_invalid_samples;
    status = sns_dd_accel_lis3dsh_parse_and_send_fifo_data(
       state, state->sub_dev[sub_dev_inx].smgr_handle, raw_buf_ptr, num_invalid_samples);
    MSG_3(MSG_SSID_SNS, DBG_HIGH_PRIO, 
          "send_fifo_data: #samples=%d #invalid=%d end_ts=%u", 
          num_samples, num_invalid_samples, state->fifo_data.end_timestamp);

    /* prepares to send remaining samples */
    num_samples -= num_invalid_samples;
    if ( num_samples > 0 )
    {
      raw_buf_ptr = &state->fifo_raw_buffer[state->fifo_data.num_samples<<1];
      state->fifo_data.end_timestamp = timestamp;
      state->fifo_data.timestamp     = state->last_timestamp +
        (timestamp - state->last_timestamp)/num_samples;
      MSG_3(MSG_SSID_SNS, DBG_HIGH_PRIO, 
            "send_fifo_data: #valid=%d ts=%u end_ts=%u", 
            num_samples, state->fifo_data.timestamp, state->fifo_data.end_timestamp);
    }
  }
  if ( num_samples > 0 )
  {
    state->fifo_data.status = SNS_DDF_SUCCESS;
    state->fifo_data.num_samples = STM_LIS3DSH_NUM_AXES * num_samples;
    status |= sns_dd_accel_lis3dsh_parse_and_send_fifo_data(
       state, state->sub_dev[sub_dev_inx].smgr_handle, raw_buf_ptr, num_samples);
  }
  state->last_timestamp = timestamp;

  return status;
}

/**
 * @brief To send a double tap event data to SMGR.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_send_dbt_event(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;
  uint32_t sub_dev_inx = 0;

  //read OUTS2 register to reset the interrupt
  status = sns_ddf_read_port(
      state->port_handle,
      STM_LIS3DSH_OUTS2,
      &rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return status;

  status = lis3dsh_search_subdev_by_sensortype(
      state,
      SNS_DDF_SENSOR_DOUBLETAP,
      &sub_dev_inx);
  if(status != SNS_DDF_SUCCESS)
    return status;

  LIS3DSH_MSG_2(HIGH, "handle_irq: doubletap: rw_buffer=%u, status=%u", rw_buffer, status);

  state->dbt_data.sensor = SNS_DDF_SENSOR_DOUBLETAP;
  state->dbt_data.status = SNS_DDF_SUCCESS;
  state->dbt_data.timestamp = sns_ddf_get_timestamp();

  //Fill out sensor output data and status
  state->dbt_data.samples[0].sample = SNS_DDF_TAP_DIR_Z;
  state->dbt_data.samples[0].status = SNS_DDF_SUCCESS;
  state->dbt_data.samples[1].sample = 0;
  state->dbt_data.samples[1].status = SNS_DDF_SUCCESS;
  state->dbt_data.samples[2].sample = 0;
  state->dbt_data.samples[2].status = SNS_DDF_SUCCESS;
  state->dbt_data.num_samples = STM_LIS3DSH_NUM_AXES;

  // double tap detected, send an event to SMGR
  status = sns_ddf_smgr_notify_data(
      state->sub_dev[sub_dev_inx].smgr_handle,
      &state->dbt_data,
      1);
  if(status != SNS_DDF_SUCCESS)
    return status;

  return SNS_DDF_SUCCESS;
}

#if STM_LIS3DSH_LOGDATA
/*===========================================================================
FUNCTION:   sns_dd_acc_lis3dshaccel_log_data
===========================================================================*/
/*!
  @brief log accel sensor data

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] accel_data_ptr: ptr to the driver data
  */
/*=========================================================================*/
static void sns_dd_acc_lis3dshaccel_log_data(sns_ddf_sensor_data_s *accel_data_ptr)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;

  if( sns_ddf_smgr_is_in_uimage() )
    return;
      
  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
      sizeof(sns_log_sensor_data_pkt_s),
      (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_ACCEL;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_STMICRO;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = accel_data_ptr->timestamp;

    //Log the sensor data
    log_struct_ptr->num_data_types = STM_LIS3DSH_NUM_AXES;
    log_struct_ptr->data[0]  = accel_data_ptr->samples[0].sample;
    log_struct_ptr->data[1]  = accel_data_ptr->samples[1].sample;
    log_struct_ptr->data[2]  = accel_data_ptr->samples[2].sample;

    //Commit log (also frees up the log packet memory)
    (void) sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
        log_struct_ptr);
  }
}
#endif

/**
 * @brief Reads FIFO_SRC register to extract the FIFO sample count
 *
 * @param[in]  state      The driver instance
 *
 * @return number of samples in FIFO
 */
uint8_t sns_dd_acc_lis3dsh_get_fifo_sample_count(
  sns_dd_acc_lis3dsh_state_t* state)
{
  uint8_t fifo_count;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;
  sns_ddf_status_e status;
  
  if(1 == state->fifo_wmk)
  {
    fifo_count = 1;
  }
  else
  {
    status  = sns_ddf_read_port(state->port_handle,
                                STM_LIS3DSH_FIFO_SRC,
                                &rw_buffer,
                                1,
                                &rw_bytes);
    if((status != SNS_DDF_SUCCESS) || (rw_bytes != 1))
    {
      fifo_count = 0;
    }
    else
    {
      fifo_count = rw_buffer & 0x1F;

      // if it's overflow, add one more sample.
      if(rw_buffer & 0x40)
      {
        fifo_count++;
      }
    }
  }
  return fifo_count;
}

/**
 * @brief To enable LIS3DSH FIFO interrupt.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_enable_fifo_int(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;

  if(false == state->fifo_enabled)
    return SNS_DDF_SUCCESS;

  if(state->fifo_enabled && state->fifo_int_enabled)
    return SNS_DDF_SUCCESS;

  if(state->fifo_wmk > 1)
  {
    // Configure accelerometer FIFO control registers
    rw_buffer = 0x0
      | (0<<7)           // force reboot
      | (1<<6)           // FIFO enable
      | (0<<5)           // Enable FIFO Watermark level use
      | (1<<4)           // ADD_INC bit
      | (0<<3)           // Enable FIFO Empty indication on int1.
      | (1<<2)           // FIFO Watermark interrupt on int1.
      | (1<<1)           // FIFO overrun interrupt on int1.
      | 0;               // BOOT interrupt on int2.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_CTRL_FIFO,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;

  }

  state->fifo_int_enabled = true;
  //state->last_timestamp = sns_ddf_get_timestamp();

  LIS3DSH_MSG_2(HIGH, "enable_fifo_int: md_enabled=%u, dbt_desired_rate=%u",
      state->md_enabled, state->dbt_desired_rate);

  if((false == state->md_enabled) && (0 == state->dbt_desired_rate))
  {
    rw_buffer = 0x68; //enable INT1, interrupt active high, signal pulsed.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_CTRL_INT,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;

    status =  sns_ddf_signal_register(
        state->gpio_num,
        state,
        &sns_dd_acc_lis3dsh_if,
        SNS_DDF_SIGNAL_IRQ_HIGH);

    state->int_registered = true;

    LIS3DSH_MSG_2(HIGH, "fifo int enabled: gpio_num=%u, status=%u",
        state->gpio_num, status);

    if(status != SNS_DDF_SUCCESS)
      return status;

  }
  
  //flush FIFO if FIFO interrupt is missed during int enable.  
  sns_dd_acc_lis3dsh_catch_missed_interrupt(state);

  //special case, watermark is 1, enable DRI instead of FIFO.
  if(1 == state->fifo_wmk)
  {
    rw_buffer = 0x80;
    status = sns_dd_acc_lis3dsh_update_onereg(
        dd_handle,
        STM_LIS3DSH_CTRL_INT,
        0x80,
        rw_buffer);
    if(status != SNS_DDF_SUCCESS)
      return status;
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief To disable LIS3DSH FIFO interrupt.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_disable_fifo_int(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;

  if((false == state->fifo_enabled) || (false == state->fifo_int_enabled))
    return SNS_DDF_SUCCESS;

  if(state->fifo_wmk > 1)
  {
    // Configure accelerometer FIFO control registers
    rw_buffer = 0x0
      | (0<<7)           // force reboot
      | (1<<6)           // FIFO enable
      | (0<<5)           // Enable FIFO Watermark level use
      | (1<<4)           // ADD_INC bit
      | (0<<3)           // Enable FIFO Empty indication on int1.
      | (0<<2)           // FIFO Watermark interrupt on int1.
      | (0<<1)           // FIFO overrun interrupt on int1.
      | 0;               // BOOT interrupt on int2.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_CTRL_FIFO,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;
  }
  else
  {
    //special case, if watermark is 1, clear DRI INT.
    rw_buffer = 0x00;
    status = sns_dd_acc_lis3dsh_update_onereg(
        dd_handle,
        STM_LIS3DSH_CTRL_INT,
        0x80,
        rw_buffer);
    if(status != SNS_DDF_SUCCESS)
      return status;
  }

  state->fifo_int_enabled = false;

  LIS3DSH_MSG_2(HIGH, "disable_fifo_int: md_enabled=%u, dbt_desired_rate=%u",
      state->md_enabled, state->dbt_desired_rate);

  if((false == state->md_enabled) && (0 == state->dbt_desired_rate) 
                                  && (true == state->int_registered))
  {
    //Don't disable INT register for FIFO INT.
    /*rw_buffer = 0x00; //disable INT1
      status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_INT,
      &rw_buffer,
      1,
      &rw_bytes);
      if(status != SNS_DDF_SUCCESS)
      return status;
      if(rw_bytes != 1)
      return SNS_DDF_EBUS;*/

    status = sns_ddf_signal_deregister(state->gpio_num);
    if(SNS_DDF_SUCCESS != status)
      return status;
    state->int_registered = false;
  }

  return SNS_DDF_SUCCESS;
}


/**
 * @brief Triggers sending FIFO data if FIFO interrupt was missed
 *
 * @param[in]  state      Handle to a driver instance.
 *
 * @return none
 */
static void sns_dd_acc_lis3dsh_catch_missed_interrupt(
  sns_dd_acc_lis3dsh_state_t* state)
{
  uint8_t fifo_count = 0; 
 
  //do nothing if wmk == 1
  if(state->fifo_wmk == 1)
    return;

  fifo_count = sns_dd_acc_lis3dsh_get_fifo_sample_count(state);

  //1.use (fifo_wmk -1) instead of fifo_wmk, in case fifo int is high
  //when fifo_count < fifo_wmk, possible leftover issue from trigger_fifo
  //2.use while loop to make sure the condition finally become false,
  //in case new samples go into fifo while fifo reading.
  while (fifo_count >= (state->fifo_wmk - 1))
  {
    MSG_2(MSG_SSID_SNS, DBG_MED_PRIO, "missed_int - count(%d) wm(%d)", 
          fifo_count, state->fifo_wmk);
    sns_dd_acc_lis3dsh_send_fifo_data(state, fifo_count, sns_ddf_get_timestamp());

    fifo_count = sns_dd_acc_lis3dsh_get_fifo_sample_count(state);
  }
}


/*===========================================================================
FUNCTION:   sns_dd_accel_lis3dsh_parse_and_send_fifo_data
===========================================================================*/
/*!
  @brief Parses FIFO data and sends to SMGR
  */
/*=========================================================================*/
static sns_ddf_status_e sns_dd_accel_lis3dsh_parse_and_send_fifo_data(
   sns_dd_acc_lis3dsh_state_t* state,
   sns_ddf_handle_t            smgr_handle,
   const uint8_t*              raw_buf_ptr,
   uint16_t                    num_samples
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t idx;
  // sensor coordinate x,y,z axis raw data sample
  static int16_t hw_d[STM_LIS3DSH_NUM_AXES];
  // SAE coordinate x,y,z axis data sample
  static q16_t sae_d[STM_LIS3DSH_NUM_AXES];

  for(idx=0; idx<num_samples; idx++)
  {
    //convert the raw data in read_buffer to X/Y/Z axis sensor data
    hw_d[0] = (int16_t) (((raw_buf_ptr[1 + idx*6]) << 8) | raw_buf_ptr[0 + idx*6]);
    hw_d[1] = (int16_t) (((raw_buf_ptr[3 + idx*6]) << 8) | raw_buf_ptr[2 + idx*6]);
    hw_d[2] = (int16_t) (((raw_buf_ptr[5 + idx*6]) << 8) | raw_buf_ptr[4 + idx*6]);

    //adjust sensor data per current sensitivity, and convert to Q16
    sae_d[0] = FX_FLTTOFIX_Q16(hw_d[0] * state->sstvt_adj * G/1000000);
    sae_d[1] = FX_FLTTOFIX_Q16(hw_d[1] * state->sstvt_adj * G/1000000);
    sae_d[2] = FX_FLTTOFIX_Q16(hw_d[2] * state->sstvt_adj * G/1000000);

    //map the sensor data to the phone's coordinates
    sns_ddf_map_axes(&state->axes_map, sae_d);

    //Fill out sensor output data and status
    state->fifo_data.samples[0 + idx*3].sample = sae_d[0];
    state->fifo_data.samples[0 + idx*3].status = state->fifo_data.status;
    state->fifo_data.samples[1 + idx*3].sample = sae_d[1];
    state->fifo_data.samples[1 + idx*3].status = state->fifo_data.status;
    state->fifo_data.samples[2 + idx*3].sample = sae_d[2];
    state->fifo_data.samples[2 + idx*3].status = state->fifo_data.status;

    /*LIS3DSH_MSG_3(HIGH, "LIS3DSH fifo_data=s0(%d), s1(%d), s2(%d)",
      FX_FIXTOFLT_Q16(sae_d[0]),
      FX_FIXTOFLT_Q16(sae_d[1]),
      FX_FIXTOFLT_Q16(sae_d[2]));*/
  }

  state->fifo_data.num_samples = STM_LIS3DSH_NUM_AXES * num_samples;

  // send fifo data to SMGR
  status = sns_ddf_smgr_notify_data(smgr_handle, &state->fifo_data, 1);
  if(status != SNS_DDF_SUCCESS)
    return status;

  LIS3DSH_MSG_1(HIGH, "parse_and_send: sent data successfully. fifo_data.num_samples=%u",
      state->fifo_data.num_samples);

  LIS3DSH_MSG_1(HIGH, "parse_and_send: end time=%u", sns_ddf_get_timestamp());

#if STM_LIS3DSH_LOGDATA
  sns_dd_acc_lis3dshaccel_log_fifo(&state->fifo_data);
#endif

  return SNS_DDF_SUCCESS;
}

#if STM_LIS3DSH_LOGDATA
/*===========================================================================
FUNCTION:   sns_dd_acc_lis3dshaccel_log_fifo
===========================================================================*/
/*!
  @brief log accel sensor data in fifo

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] accel_data_ptr: ptr to the driver data
  */
/*=========================================================================*/
static void sns_dd_acc_lis3dshaccel_log_fifo(sns_ddf_sensor_data_s *accel_data_ptr)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;
  uint16 idx =0 ;

  if( sns_ddf_smgr_is_in_uimage() )
    return;

  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
      sizeof(sns_log_sensor_data_pkt_s) + (accel_data_ptr->num_samples -1)*sizeof(int32_t),
      (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_ACCEL;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_STMICRO;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = accel_data_ptr->timestamp;
    log_struct_ptr->end_timestamp = accel_data_ptr->end_timestamp;

    log_struct_ptr->num_data_types = STM_LIS3DSH_NUM_AXES;
    log_struct_ptr->num_samples = accel_data_ptr->num_samples / STM_LIS3DSH_NUM_AXES;

    //Log the sensor fifo data
    log_struct_ptr->data[0]  = accel_data_ptr->samples[0].sample;
    log_struct_ptr->data[1]  = accel_data_ptr->samples[1].sample;
    log_struct_ptr->data[2]  = accel_data_ptr->samples[2].sample;

    for(idx=0; idx<accel_data_ptr->num_samples; idx++)
    {
      log_struct_ptr->samples[idx]  = accel_data_ptr->samples[idx].sample;
    }

    //Commit log (also frees up the log packet memory)
    (void) sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
        log_struct_ptr);
  }
}

#endif

/**
 * @brief Update only one part of one register in LIS3DSH
 *
 * @param[in]  dd_handle   Handle to a driver instance.
 * @param[in]  reg_addr    The address of LIS3DSH register to be updated
 * @param[in]  mask        The mask of this register to specify which part to be updated
 * @param[in]  new_value   The new content for the specified part of this register
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_update_onereg(
    sns_ddf_handle_t dd_handle,
    uint8_t reg_addr,
    uint8_t mask,
    uint8_t new_value)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;

  // read current value from this register
  status = sns_ddf_read_port(
      state->port_handle,
      reg_addr,
      &rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(rw_bytes != 1)
    return SNS_DDF_EBUS;

  // generate new value
  rw_buffer = (rw_buffer & (~mask)) | (new_value & mask);

  // write new value to this register
  status = sns_ddf_write_port(
      state->port_handle,
      reg_addr,
      &rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(rw_bytes != 1)
    return SNS_DDF_EBUS;

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Enable double tap interrupt.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_enable_dbt_int(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t*)
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_64);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t write_buffer[1] = {0};
  uint8_t bytes_written = 0;

  //read OUTS1 register to reset the interrupt flag
  sns_ddf_read_port(
      state->port_handle,
      STM_LIS3DSH_OUTS2,
      &write_buffer[0],
      1,
      &bytes_written);

  //enable SM2
  write_buffer[0] = 0x01;
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_SM2,
      &write_buffer[0],
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 1)
    return SNS_DDF_EBUS;

  if((false == state->md_enabled) && (false == state->fifo_int_enabled))
  {
    write_buffer[0] = 0x68; //enable INT1, interrupt active high, signal pulsed.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_CTRL_INT,
        &write_buffer[0],
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(bytes_written != 1)
      return SNS_DDF_EBUS;

    status =  sns_ddf_signal_register(
        state->gpio_num,
        state,
        &sns_dd_acc_lis3dsh_if,
        SNS_DDF_SIGNAL_IRQ_RISING);

    state->int_registered = true;

    return status;
  }


  return SNS_DDF_SUCCESS;
}


/**
 * @brief Performs a double tap disable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_disable_dbt(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t*)
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_64);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t write_buffer[1] = {0};
  uint8_t bytes_written = 0;

  // clear the MD interrupt
  //Register Address: Value
  //0x22:      0x00
  write_buffer[0] = 0x00;

  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_SM2,
      &write_buffer[0],
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 1)
    return SNS_DDF_EBUS;

  if((false == state->md_enabled) && (false == state->fifo_int_enabled)
                                  && (true == state->int_registered))
  {
    status = sns_ddf_signal_deregister(state->gpio_num);
    if(SNS_DDF_SUCCESS != status)
      return status;
    state->int_registered = false;
  }

  return SNS_DDF_SUCCESS;
}
