/********************************* (C) COPYRIGHT 2016 STMicroelectronics ********************************
 *
 * File Name         : sns_dd_lsm6ds3.c
 * Authors           : Karimuddin Sayed
 * Version           : V 4.19.0
 * Date              : 8/24/2016
 * Description       : LSM6DS3 driver source file
 *
 *********************************************************************************************************
 * Copyright (c) 2016, STMicroelectronics.
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
 *********************************************************************************************************
 * REVISON HISTORY
 *
 * VERSION | DATE          | DESCRIPTION
 *
 * 1.0.0   | 9/10/2014     | DDF device driver initial version
 * 1.1.0   | 9/19/2014     | Added HW self test and SW self test.
 * 1.2.0   | 9/26/2014     | Passed compilation and bug fixes.
 * 1.2.2   | 9/26/2014     | Fixed one crash bug, one accel set_power bug.
 * 1.2.3   | 10/15/2014    | uimage support added
 * 3.0.1   | 12/08/2014    | FIFO inital version
 * 3.1.0   | 01/23/2015    | moving head pointer to uimg file, to fix crash
 * 3.2.0   | 01/29/2015    | Fixed timestamp issue.
 * 3.2.1   | 02/10/2015    | update fifo params, after powering down sensors
 * 3.2.2   | 02/20/2015    | log packet support, temp sensor support in pooling mode
 * 3.2.3   | 02/26/2015    | log uImage packets, update turn on time
 * 3.2.4   | 03/04/2015    | temp sensor support in fifo
 * 3.2.5   | 04/10/2015    | step detector support
 * 3.2.6   | 04/20/2015    | update smd and step event output data
 * 3.2.7   | 06/23/2015    | timestamp gap, gyro sleep
 * 3.2.9   | 07/28/2015    | added oem selftest 
 * 3.2.10  | 08/04/2015    | fix duplicated data when turn on
 * 3.2.11  | 08/05/2015    | 3.2.10 not fixed duplicated data when turn on,
 *                         | correct fix added
 * 3.2.13  | 09/02/2015    | code optimization - moveout some functions from uimg
 * 3.5.1   | 09/30/2015    | Added Accel always on feature
 * 3.6.1   | 10/01/2015    | CTS jitter failure fixes and time gap fixes
 * 3.7.1   | 10/07/2015    | Klockwork fixes
 * 3.8.1   | 10/08/2015    | change wmk without closing sensor
 * 3.9.0   | 10/14/2015    | fix timestamp when flush or first pass
 * 3.10.0  | 10/20/2015    | successive samples with same timestamp
 * 3.11.0  | 10/23/2015    | Enabled TILT detection feature
 * 3.12.0  | 11/06/2015    | MD not sensitive enough, used HPF instead of Slope
 * 3.13.0  | 11/13/2015    | handle_irq optimizations, reducing register reads
 *                         | removing unused code
 *                         | setting 416Hz as Max odr
 *                         | freeing fifo memory in disable fifo
 * 3.14.0  | 11/19/2015    | gyro stream issue, when acc enabled first in fifo
 * 3.15.0  | 11/24/2015    | Enable D6D event sensor, used OEM_SENSOR2
 *                         | optimize disable_esp function
 * 3.16.0  | 12/02/2015    | collect data updated to check more than 2 sampels
 *                         | set range updated to check initial range
 * 3.17.0  | 12/08/2015    | Added 2 sample time settling time for MD HPF
 *                         | set acc odr for esp, when gyro fifo enabled/disabled
 * 3.18.0  | 12/17/2015    | update cal_sample_time in case of flush
 * 3.19.0  | 12/21/2015    | using 64 bit timestamp comparison instead of 32 bit
 *                         | Max odr reset to 208 as per QCOM request
 * 3.20.0  | 12/24/2015    | align read bytes based on active sensors also
 *                         | Calculate timestamp in case ADSP delayed calling irq_handle
 * 3.21.0  | 12/27/2015    | check fifo_count before reading fifo along fifo_empty_flag
 * 3.22.0  | 12/31/2015    | replace 1 arg logs with 2/3 arguments
 *                         | Adjust timestamp logic when end time drifting away from cur time
 * 3.23.0  | 1/12/2016     | Fixing z-axis data becoming zero.
 *                         | Fixing timestamp gaps when flush happens
 * 3.24.0  | 1/12/2016     | fix streaming issue with selftest enabled, BDU disable
 * 3.25.0  | 1/19/2016     | Update timestamp logic, to fix missing samples
 * 4.0.0   | 2/1/2016      | Enable 13Hz suppport and Temp sensor support for gyro
 * 4.1.0   | 2/5/2016      | updating reset function for gyro outof bound and 
 *                         | fixing frequency sweep issue
 * 4.2.0   | 2/10/2016     | temp sensor streaming with multiple clients
 * 4.3.0   | 2/16/2016     | High Gyro z-axis bias, overwriting some bits of 0x63 reg
 * 4.4.0   | 2/16/2016     | ADSP crash fix with multiple clients of temp sensor
 * 4.5.0   | 2/16/2016     | Accel low power mode when not streaming & temp sensor closing
 * 4.6.0   | 2/24/2016     | Adding tilt tuning and temp sensor attribute updates
 * 4.7.0   | 3/1/2016      | compilation flag for esp features
 * 4.8.0   | 3/11/2016     | update delay in reset to 1 ms (max allowed 1ms)
 * 4.9.0   | 3/29/2016     | fix, if more than 4 invalid samples while streaming
 * 4.10.0  | 4/07/2016     | fix, use of active_sensors and buf read if greater than size of raw_buffer
 * 4.11.0  | 4/11/2016     | add double tap on INT2.
 * 4.12.0  | 4/27/2016     | adding  freefall support
 * 4.13.0  | 6/29/2016     | updating MD handling-first handle fifo, then send md event to smgr at the end
 * 4.14.0  | 7/1/2016      | check fifo status before powering down and resetting, update the logic.
 * 4.15.0  | 7/13/2016     | update overflow/fifo full logic, reset fifo when overflow/fifo full happens
 * 4.17.0  | 8/2/2016      | Based on 4.15.0. Use sns_dd_lsm6ds3_get_state(), sns_dd_lsm6ds3_register_signal().
 *                         | Set BDU=1, I2C_DISABLE=1.
 * 4.18.0  | 8/23/2016     | Enable accel normal/LP mode when streaming data. Enable accel LPF2
 * 4.19.0  | 8/24/2016     | Apply timestamp jitter smoothing patch.
 *********************************************************************************************************/

#include "sns_dd_lsm6ds3.h"

extern sns_ddf_driver_if_s sns_dd_lsm6ds3_if;
#if LSM6DS3_ENABLE_ESP
extern sns_ddf_driver_if_s sns_dd_lsm6ds3_esp_if;
#endif

extern void sns_dd_lsm6ds3_handle_timer(
   sns_ddf_handle_t dd_handle,
    void* arg);
extern void sns_dd_lsm6ds3_config_md(
    sns_ddf_nv_params_s* nv_params,
    stm_lsm6ds3_esp_config *esp_config);

/**
 * Accelerometer LSM6DS3_ACC ranges array for setting attribute
 */
stm_lsm6ds3_acc_range lsm6ds3_acc_ranges[STM_LSM6DS3_ACC_RANGE_NUM]={
  STM_LSM6DS3_ACC_RANGE_2G,
  STM_LSM6DS3_ACC_RANGE_4G,
  STM_LSM6DS3_ACC_RANGE_8G,
  STM_LSM6DS3_ACC_RANGE_16G
};

/**
 * Accelerometer LSM6DS3 sampling frequencies array for getting attribute
 */
sns_ddf_lowpass_freq_t lsm6ds3_freqs[STM_LSM6DS3_ODR_NUM] = {
/*
  FX_CONV_Q16((int32_t)13,0),
  FX_CONV_Q16((int32_t)26,0),
  FX_CONV_Q16((int32_t)52,0),
  FX_CONV_Q16((int32_t)104,0),
  FX_CONV_Q16((int32_t)208,0)
  */
  (q16_t) (6<<16),
  (q16_t) (13<<16),
  (q16_t) (26<<16),
  (q16_t) (52<<16),
  (q16_t) (104<<16),
  (q16_t) (208<<16)
};

/**
 * Magnetometer LSM6DS3_GYR ranges array for setting attribute
 */
stm_lsm6ds3_gyr_range lsm6ds3_gyr_ranges[STM_LSM6DS3_GYR_RANGE_NUM]={
  STM_LSM6DS3_GYR_RANGE_245DPS,
  STM_LSM6DS3_GYR_RANGE_500DPS,
  STM_LSM6DS3_GYR_RANGE_1000DPS,
  STM_LSM6DS3_GYR_RANGE_2000DPS
};

extern sns_ddf_odr_t lsm6ds3_odr[STM_LSM6DS3_ODR_NUM];
extern stm_lsm6ds3_acc_odr lsm6ds3_acc_odr_reg_values[STM_LSM6DS3_ODR_NUM];
extern stm_lsm6ds3_gyr_odr lsm6ds3_gyr_odr_reg_values[STM_LSM6DS3_ODR_NUM];
extern stm_lsm6ds3_fifo_odr lsm6ds3_fifo_odr_reg_values[STM_LSM6DS3_ODR_NUM];

extern stm_lsm6ds3_head_s lsm6ds3_dd_head;
static uint8_t stm_lsm6ds3_axis_dflt[3] = {1, 2, 3};


/*===========================================================================

FUNCTION:   dd_find_free_sub_dev_slot

===========================================================================*/
/*!
  @brief Find a free slot for a sub-dd

  @detail

  @param[i] my_dev_ptr - the poiter to the my_dev instance

  @return
  The index for the first free slot of the sub-dd
  -1 : When there is no free slot for a sub-dd
  */
/*=========================================================================*/
static int32_t dd_find_free_sub_dev_slot (sns_dd_lsm6ds3_state_t *my_dev_ptr)
{
  uint32_t i;
  for (i =0; i < LSM6DS3_MAX_SUB_DEV; i++)
  {
    if ( 0 == my_dev_ptr->sub_dev[i].num_sensors )
    {
      return i;
    }
  }
  return -1;
}
/*===========================================================================

FUNCTION:   dd_fill_sub_dev_slot

===========================================================================*/
/*!
  @brief  Fill the sub-dd slot with the sensor types from the input parameters

  @detail

  @param[o] sub_dev_slot_ptr - the poiter to the my_dev instance
  @param[i] num_sensors - the number of sensor types
  @param[i] sensors - the list of sensor types

  @return the number of sensor types that the device suspport
  */
/*=========================================================================*/
static uint32_t dd_fill_sub_dev_slot (stm_lsm6ds3_sub_dev_s *sub_dev_slot_ptr, uint32_t num_sensors, sns_ddf_sensor_e sensors[])
{
  uint32_t my_num_sensors = 0, i;
  for (i =0; i < num_sensors; i++)
  {
    /*
     * Note: both ACCEL and GYR are Primary Data Types, by definition.
     */
    if(LSM6DS3_IS_ESP_SENSOR(sensors[i]))
        sub_dev_slot_ptr->sensors[my_num_sensors++] = sensors[i];
    else {
      LSM6DS3_MSG_2(HIGH, "fill_sub_dev i (%d) sensor %d",i, sensors[i]);
      switch(sensors[i]) {
        case SNS_DDF_SENSOR_ACCEL:
        case SNS_DDF_SENSOR_GYRO:
          sub_dev_slot_ptr->sensors[my_num_sensors++] = sensors[i];
          sub_dev_slot_ptr->sensors[my_num_sensors++] = SNS_DDF_SENSOR_TEMP;
          break;
        default:
          break;
      }
    }
  }

  sub_dev_slot_ptr->num_sensors = my_num_sensors;
  return my_num_sensors;
}

/*===========================================================================

FUNCTION:   dd_find_same_dev

===========================================================================*/
/*!
  @brief  Find the device instance which if corespond to the input port configuration

  @detail

  @param[i] port_config_ptr - the port configuration

  @return the my_dev instance
  */
/*=========================================================================*/
static sns_dd_lsm6ds3_state_t *dd_find_same_port_dev (sns_ddf_port_config_s *port_config_ptr)
{
  sns_dd_lsm6ds3_state_t *cur_dd_dev_ptr = (sns_dd_lsm6ds3_state_t *)lsm6ds3_dd_head.next;

  while (cur_dd_dev_ptr)
  {
    if (true == sns_ddf_is_same_port_id(port_config_ptr, &cur_dd_dev_ptr->port_id))
    {
      return cur_dd_dev_ptr;
    }
    else
    {
      cur_dd_dev_ptr = cur_dd_dev_ptr->next;
    }
  }
  return NULL;
}


/**
 * @brief To update odr of lsm6ds3 based on cur accel and gyro rate
 *
 * @param[in]  state          - Handle to a driver instance.
 * @param[in]  new_rate_idx       - if new_rate_idx = -1 device is resetting
 *
 * @return SNS_DDF_SUCCESS if set appropriate rate otherwise return SNS_DDF_EINVALID_PARAM.
 */

sns_ddf_status_e sns_dd_lsm6ds3_update_rate(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    int8_t new_rate_idx)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_dd_lsm6ds3_state_t* state;
  int8_t a_cur_odr_idx = -1;
  int8_t g_cur_odr_idx = -1;
  int8_t odr_idx = -1;
  int8_t update_rate_idx;
  int8_t cur_rate_idx;

  //enters in this function only when fifo is enabled and new_rate_idx >= 0
  //doesn't enter when sensor is powering down
  //dd_handle would always be a pointer to stat->sub_dev[sub_dev_idx]
  state = sns_dd_lsm6ds3_get_state(dd_handle);

  cur_rate_idx = state->fifo_info.fifo_rate_idx;
  a_cur_odr_idx = state->acc_cur_rate_idx;
  g_cur_odr_idx = state->gyr_cur_rate_idx;

  if(((sensor != SNS_DDF_SENSOR_ACCEL) && 
        (sensor != SNS_DDF_SENSOR_GYRO)) ||
      (new_rate_idx < 0)) {
    LSM6DS3_MSG_E_2(ERROR, "sensor (%d) new_rate (%d) invalid parameters", sensor, new_rate_idx);
    return SNS_DDF_EINVALID_PARAM;
  }

  if(sensor == SNS_DDF_SENSOR_ACCEL) {
    update_rate_idx = MAX(new_rate_idx, state->gyr_desired_rate_idx);
    odr_idx = a_cur_odr_idx;
  }
  else {
    if(LSM6DS3_IS_ESP_ACTIVE(state->esp_info))
      update_rate_idx = MAX(a_cur_odr_idx, new_rate_idx);
    else
      update_rate_idx = MAX(state->acc_desired_rate_idx, new_rate_idx);
    odr_idx = g_cur_odr_idx;
  }

  LSM6DS3_MSG_3(LOW, "update_rate new_rate_idx (%d) update_rate_idx (%d) cur_fifo_rate_idx (%d)",new_rate_idx, update_rate_idx, cur_rate_idx);
  if(update_rate_idx < 0) {
    return SNS_DDF_SUCCESS;
  }

  if((update_rate_idx != cur_rate_idx) ||
      (odr_idx != cur_rate_idx)) {
    LSM6DS3_DATA_MSG_2(LOW, "update accel, gyro and fifo rate - update_rate_idx (%d) cur_rate_idx (%d)", update_rate_idx, cur_rate_idx);

    sns_dd_lsm6ds3_stop_fifo_streaming(dd_handle, state->fifo_info.fifo_enabled, true);
    sns_dd_lsm6ds3_start_fifo_streaming(dd_handle, update_rate_idx, 0x00);
    state->fifo_info.avg_sample_time = 0;

    if(sensor == SNS_DDF_SENSOR_ACCEL) {
      //update desired rate before sending event
      if(state->gyr_desired_rate_idx >= 0)
        state->data_intp.gyr_intp_status = true;

      if(g_cur_odr_idx != state->gyr_cur_rate_idx) {
        sns_dd_lsm6ds3_send_event_smgr(dd_handle,
            SNS_DDF_SENSOR_GYRO,
            SNS_DDF_EVENT_ODR_CHANGED);
      }
    } else {
      if(state->acc_desired_rate_idx >= 0) {
        state->data_intp.acc_intp_status = true;
        if(a_cur_odr_idx != state->acc_cur_rate_idx) {
          sns_dd_lsm6ds3_send_event_smgr(dd_handle,
              SNS_DDF_SENSOR_ACCEL,
              SNS_DDF_EVENT_ODR_CHANGED);
        }
      }
    }
  }
  return status;
}


/**
 * @brief To check if the other sub device is in active state
 *
 * @param[in]  sub_dev_inx    - the sub device index
 * @param[in]  state               - Handle to a driver instance.
 *
 * @return true - one of the other sub device is in active state.
 */
bool sns_dd_acc_lsm6ds3_is_active_other_sub_dev (
    sns_dd_lsm6ds3_state_t* state, 
    uint32_t sub_dev_inx)
{
  uint32_t i, j;
  bool     return_val = false;
  stm_lsm6ds3_sub_dev_s *sub_dev_slot_ptr = &state->sub_dev[0];

  for (i =0; i < LSM6DS3_MAX_SUB_DEV; i++)
  {
    if (i == sub_dev_inx) continue;
    for (j =0; (i < LSM6DS3_MAX_SUB_DEV) && 
        (j < sub_dev_slot_ptr[i].num_sensors) &&
        (j < ARR_SIZE(sub_dev_slot_ptr[i].sensors)); j++)
    {

      if(LSM6DS3_IS_ESP_SENSOR(sub_dev_slot_ptr[i].sensors[j]))
        return_val = LSM6DS3_IS_ESP_ACTIVE(state->esp_info);
      else if(sub_dev_slot_ptr[i].sensors[j] == SNS_DDF_SENSOR_ACCEL) {
        if (state->acc_desired_rate_idx >= 0) 
          return_val = true;
      }
      if(return_val == true) {
        LSM6DS3_MSG_3(LOW, "Active  subdev_idx (%d) s_dev_idx(%d) sensor (%d)",i, j, sub_dev_slot_ptr[i].sensors[j]);
        i = LSM6DS3_MAX_SUB_DEV;
      }
    }
  }
  return return_val;
}
/**
 * @brief Find the matched internal ODR for desired ODR.

 *
 * @param[in]  desired_rate      New desired ODR.
 * @param[out] new_rate          Matched internal ODR.
 * @param[out] new_index         The index of matched internal ODR in lsm6ds3_odr.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e  sns_dd_lsm6ds3_match_odr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_odr_t desired_rate,
    sns_ddf_odr_t *new_rate,
    int8_t *new_index)
{
  uint8_t idx;
  *new_index = -1;
  sns_ddf_odr_t* odr_list = NULL;

  if((0 == desired_rate) || (desired_rate > STM_LSM6DS3_MAX_ODR)) {
    LSM6DS3_MSG_E_2(ERROR, "ODR MATCH failed desired_rate (%d) STM_LSM6DS3_MAX_ODR (%d)",desired_rate, STM_LSM6DS3_MAX_ODR);
    return SNS_DDF_EINVALID_PARAM;
  }

  odr_list = lsm6ds3_odr;

  for(idx=0; idx<STM_LSM6DS3_ODR_NUM; idx++)
  {
    if(desired_rate <= odr_list[idx])
      break;
  }

  if (idx >= STM_LSM6DS3_ODR_NUM)
    return SNS_DDF_EINVALID_PARAM;

  *new_rate = odr_list[idx];
  *new_index = idx;

  return SNS_DDF_SUCCESS;
}
/**
 * @brief Performs a motion detection enable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_lsm6ds3_enable_md(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_buffer = 0;
  int8_t new_rate_index;
  sns_ddf_odr_t new_rate;
  uint8_t dur_set = 0;
  uint8_t thresh_set = 0;

  LSM6DS3_MSG_1(LOW, "enable_md dd_handle (%u)", dd_handle);
  //Register configuration for wake-up application
  //config ODR and bandwidth
  //only config ODR when double tap sensor is off. ODR when dbt is on is always 1250hz.

  if(!(state->esp_info.esp_enabled & STM_LSM6DS3_DBT_IDX))
  {
    if(state->fifo_info.acc_fifo_alive == false) {
      status =  sns_dd_lsm6ds3_match_odr(dd_handle, 
          STM_LSM6DS3_MD_ODR,
          &new_rate,
          &new_rate_index);
      if(SNS_DDF_SUCCESS != status) {
        LSM6DS3_MSG_E_1(ERROR, "match_odr failed status (%d)", status);
        return status;
      }
      //update rate 
      status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle, new_rate_index);
      if(SNS_DDF_SUCCESS != status) {
        LSM6DS3_MSG_E_1(ERROR, "set_acc_datarate failed status (%d)", status);
        return status;
      }
    }
    thresh_set = STM_LSM6DS3_MD_THRESH;
    dur_set =  STM_LSM6DS3_MD_DUR;
  }
  else
  {
    // default, double-tap enabled
    thresh_set = state->esp_info.esp_config.md_thresh;
    dur_set = state->esp_info.esp_config.md_dur;
  }

  LSM6DS3_MSG_3(HIGH, "esp_enabled (%d) md_thresh (%d) md_odr (%d)", state->esp_info.esp_enabled, thresh_set, dur_set);
  rw_buffer = thresh_set;
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_WAKE_THS,
      &rw_buffer,
      1,
      0x3F);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write reg failed  status=%u", status);
    return status;
  }
  //rw_buffer = STM_LSM6DS3_CONSTANT_CNT;
  rw_buffer = dur_set;
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_WAKE_DUR,
      &rw_buffer,
      1,
      0x60);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write reg failed  status=%u", status);
    return status;
  }

  rw_buffer = 0
    | (0<<7)            // INT1_INACT_STATE
    | (0<<6)            // INT1_SINGLE_TAP
    | (1<<5)            // INT1_WU
    | (0<<4)            // INT1_FF
    | (0<<3)            // INT1_DOUBLE_TAP
    | (0<<2)            // INT1_6D
    | (0<<1)            // INT1_TILT
    | 0;                // INT1_TIMER
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_MD1_CFG,
      &rw_buffer,
      1,
      0x20);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write reg failed  status=%u", status);
    return status;
  }
  state->md_enabled = true;
  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a motion detection disable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_lsm6ds3_disable_md(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_buffer[1] = {0};

  LSM6DS3_MSG_0(LOW, "disable_md dd_handle ");
  if(state->md_enabled) {
    rw_buffer[0] = 0
      | (0<<7)            // INT1_INACT_STATE
      | (0<<6)            // INT1_SINGLE_TAP
      | (0<<5)            // INT1_WU
      | (0<<4)            // INT1_FF
      | (0<<3)            // INT1_DOUBLE_TAP
      | (0<<2)            // INT1_6D
      | (0<<1)            // INT1_TILT
      | 0;                // INT1_TIMER
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_MD1_CFG,
        rw_buffer,
        1,
        0x20);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "write reg failed  status=%u", status);
      return status;
    }

    rw_buffer[0] = 0x3F;
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_WAKE_THS,
        rw_buffer,
        1,
        0x3F);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "write reg failed  status=%u", status);
      return status;
    }

    if((state->acc_desired_rate_idx < 0) &&
        (state->fifo_info.acc_fifo_alive == false) &&
        (!LSM6DS3_IS_ESP_ACTIVE(state->esp_info))) {
      status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle, -1);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }
    LSM6DS3_MSG_1(HIGH, "disable_md: status=%d", status);
    state->md_enabled = false;
  }
  return SNS_DDF_SUCCESS;

}
sns_ddf_status_e sns_dd_lsm6ds3_temp_set_rate(sns_ddf_handle_t dd_handle, uint8_t value)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint32_t sub_dev_idx = (dd_handle_align_t) dd_handle & (~DD_HANDLE_ALIGN_BITS);

  sns_ddf_sensor_e p_sensor= state->sub_dev[sub_dev_idx].sensors[0];
  uint8_t idx = 0, t_idx = 0;

  if(p_sensor == SNS_DDF_SENSOR_ACCEL) {
    idx = 0x01;
    t_idx = 0x0F;
  } else if(p_sensor == SNS_DDF_SENSOR_GYRO) {
    idx = 0x02;
    t_idx = 0xF0;
  }

  if(value) {
    //set_odr
    //check fifo is enabled for p_sensor and temp odr is set for p_sensor
    if((state->temp_cur_rate & t_idx) &&
        (state->fifo_info.fifo_enabled & idx)) {
      if(state->timer == NULL) {
        if((NULL == state->fifo_info.temp_data.samples))
          sns_dd_lsm6ds3_alloc(dd_handle, SNS_DDF_SENSOR_TEMP);
        sns_ddf_timer_init(&state->timer, dd_handle, &sns_dd_lsm6ds3_if, NULL, true);
        sns_ddf_timer_start(state->timer, 1000000u);
        LSM6DS3_MSG_2(HIGH, "Temp timer started: temp_cur_rate (%d) ", state->temp_cur_rate, state->fifo_info.fifo_enabled);
      }
      if(((p_sensor == SNS_DDF_SENSOR_ACCEL) &&
            (state->acc_desired_rate_idx >= 0)) ||
          ((p_sensor == SNS_DDF_SENSOR_GYRO) &&
           (state->gyr_desired_rate_idx >= 0)))

        state->fifo_info.fifo_enabled |= (idx << 2);
    }
  } else {
    if(!state->temp_cur_rate) {
      if(state->timer != NULL) {
        LSM6DS3_MSG_2(HIGH, "Temp timer cancelled: temp_cur_rate (%d) fifo_enabled (0x%x)", state->temp_cur_rate, state->fifo_info.fifo_enabled);
        sns_ddf_timer_cancel(state->timer);
        sns_ddf_timer_release(state->timer);
        state->timer = NULL;
        state->fifo_info.fifo_enabled &= (~0x0C);
        if((NULL != state->fifo_info.temp_data.samples))
          sns_dd_lsm6ds3_free(dd_handle, SNS_DDF_SENSOR_TEMP);
      }
    }
    if(state->timer != NULL) {
      //fifo mode
      state->fifo_info.fifo_enabled &= ~(idx << 2);

    }
  }

  return SNS_DDF_SUCCESS;
}
/**
 * @brief Performs a set accelerometer data sampling rate operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  data_rate       Data sampling rate settings for LSM6DS3
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lsm6ds3_attrib_set_datarate(
    sns_ddf_handle_t dd_handle,
    uint32_t sub_dev_idx,
    sns_ddf_odr_t data_rate)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  int8_t new_rate_idx, esp_rate_idx;
  sns_ddf_odr_t new_rate;
  LSM6DS3_MSG_2(LOW, "acc_attrib_setrate data_rate (%d) fifo_enabled (%d) ", data_rate, state->fifo_info.fifo_enabled);

  //set different data rate
  if (data_rate > 0)
  {
    status =  sns_dd_lsm6ds3_match_odr(dd_handle,
        data_rate,
        &new_rate,
        &new_rate_idx);

    if(SNS_DDF_SUCCESS != status)
      return SNS_DDF_EINVALID_PARAM;

    LSM6DS3_DATA_MSG_3(LOW, "acc_attrib_setrate new_rate_idx (%d) acc_desired_rate_idx (%d) gyr_desired_rate_idx (%d) ",
        new_rate_idx, state->acc_desired_rate_idx, state->gyr_desired_rate_idx);

    //do not update with max rate, useful when gyro is disabled
    state->acc_desired_rate_idx = new_rate_idx;

    if(LSM6DS3_IS_ESP_ACTIVE(state->esp_info)) {
      esp_rate_idx = LSM6DS3_ESP_RATE(state->esp_info);
      if(esp_rate_idx > new_rate_idx)
        new_rate_idx = esp_rate_idx;
    }

    if(state->fifo_info.fifo_enabled) {
      status =  sns_dd_lsm6ds3_update_rate(dd_handle, SNS_DDF_SENSOR_ACCEL, new_rate_idx);
      if(SNS_DDF_SUCCESS != status) {
        return status;
      }
      if(state->gyr_cur_rate_idx >= 0)
        state->data_intp.gyr_intp_status = true;
    } else if(!(state->fifo_info.fifo_enabled)) {
      status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle,
          new_rate_idx);
      if(SNS_DDF_SUCCESS != status)
        return status;
    }

    return status;
  } else {
    state->acc_desired_rate_idx = -1;
    //data rate is less than zero
    //turn off accel
    if(!(state->fifo_info.fifo_enabled & 0x01) &&
        (state->fifo_info.acc_fifo_alive == false)) {

      if(LSM6DS3_IS_ESP_ACTIVE(state->esp_info)) {
        new_rate_idx = LSM6DS3_ESP_RATE(state->esp_info);
        LSM6DS3_DATA_MSG_3(HIGH, "new_rate(%d) new_rate_idx (%d) esp_en (0x%x) ", 
            (new_rate_idx >= 0) ? lsm6ds3_odr[new_rate_idx]:-1, new_rate_idx, state->esp_info.esp_enabled);
        //update rate 
        status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle, new_rate_idx);

        if(SNS_DDF_SUCCESS != status) {
          LSM6DS3_MSG_E_1(ERROR, "set_acc_datarate failed status (%d)", status);
          return status;
        }
      }
    }
  }
  return status;
}

/**
 * @brief Performs a set gyroscope data sampling rate operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  data_rate       Data sampling rate settings for LSM6DS3
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_gyr_lsm6ds3_attrib_set_datarate(
    sns_ddf_handle_t dd_handle,
    uint32_t sub_dev_idx,
    sns_ddf_odr_t data_rate)
{

  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  sns_ddf_odr_t new_gyr_rate;
  int8_t new_gyr_rate_index;

  LSM6DS3_MSG_2(LOW, "gyr_attrib_setrate data_rate (%d) fifo_enabled (%d)",data_rate, state->fifo_info.fifo_enabled);
  if (data_rate > 0)
  {
    status =  sns_dd_lsm6ds3_match_odr(dd_handle,
        data_rate,
        &new_gyr_rate,
        &new_gyr_rate_index);
    if(SNS_DDF_SUCCESS != status)
      return SNS_DDF_EINVALID_PARAM;

    LSM6DS3_DATA_MSG_3(LOW, "gyr_attrib_setrate new_gyr_rate_index (%d) acc_desired_rate_idx (%d) gyr_desired_rate_idx (%d) ",
        new_gyr_rate_index, state->acc_desired_rate_idx, state->gyr_desired_rate_idx);

    state->gyr_desired_rate_idx = new_gyr_rate_index;
    if(state->fifo_info.fifo_enabled) {
      status =  sns_dd_lsm6ds3_update_rate(dd_handle, SNS_DDF_SENSOR_GYRO, new_gyr_rate_index);
      if(SNS_DDF_SUCCESS != status) {
        return status;
      }
      if(state->acc_cur_rate_idx >= 0)
        state->data_intp.acc_intp_status = true;
    } else {
      status = sns_dd_gyr_lsm6ds3_set_datarate(dd_handle,
          new_gyr_rate_index, false);
    }

    state->gyr_desired_rate_idx = new_gyr_rate_index;
    return status;
  } else {
    //data rate is less than zero
    //turn off gyro
    LSM6DS3_DATA_MSG_2(LOW, "gyr_attrib_setrate data_rate (%d) fifo_enabled (%d) ",data_rate, state->fifo_info.fifo_enabled);
    if(state->fifo_info.fifo_enabled) {
      if(!(state->fifo_info.fifo_enabled & 0x02)) {
        sns_dd_gyr_lsm6ds3_set_datarate(dd_handle,
            -1, true);

      }
      state->gyr_desired_rate_idx = -1;
      return status;
    } else {
      sns_dd_gyr_lsm6ds3_set_datarate(dd_handle,
          -1, true);

      state->gyr_desired_rate_idx = -1;
      return status;
    }
  }
}
/**
 * @brief Resets the driver and device so they return to the state they were
 *        in after init() was called.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm6ds3_reset(sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint32_t sub_dev_idx = (dd_handle_align_t) dd_handle & (~DD_HANDLE_ALIGN_BITS);
  stm_lsm6ds3_sub_dev_s *sub_dev_slot_ptr;
  uint32_t i = 0;
  uint8_t rw_buffer[5] = {0};
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  bool reset = true;

  if ( sub_dev_idx < ARR_SIZE(state->sub_dev) )
  {
    sub_dev_slot_ptr = &state->sub_dev[sub_dev_idx];
  }
  else
  {
    return SNS_DDF_EFAIL;
  }

  LSM6DS3_MSG_2(LOW, "Reset - called for subdev_idx (%d) sensor (%d)", sub_dev_idx, sub_dev_slot_ptr->sensors[0]);

  switch (sub_dev_slot_ptr->sensors[0])
  {
    case SNS_DDF_SENSOR_ACCEL:
      if(sub_dev_slot_ptr->powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
        sns_dd_acc_lsm6ds3_set_power(dd_handle, SNS_DDF_POWERSTATE_LOWPOWER);
      break;
    case SNS_DDF_SENSOR_GYRO:
      if(sub_dev_slot_ptr->powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
        sns_dd_gyr_lsm6ds3_set_power(dd_handle, SNS_DDF_POWERSTATE_LOWPOWER);
        LSM6DS3_MSG_3(MED, "ScaleFactorComp=%d,%d,%d", state->gyr_sstvt_comp[0], state->gyr_sstvt_comp[1], state->gyr_sstvt_comp[2]);
      break;
    default:
      break;
  }

  for (i =0; i < LSM6DS3_MAX_SUB_DEV; i++)
  {
    if((state->sub_dev[i].powerstate != SNS_DDF_POWERSTATE_LOWPOWER) && 
        (state->sub_dev[i].num_sensors)) {
      LSM6DS3_MSG_3(LOW, "Reset - sensor (%d) sub_dev (%d) power_state (%d)", state->sub_dev[i].sensors[0], i, state->sub_dev[i].powerstate);
      reset = false;
    }
  }

  if(reset != true) {
    /*TODO check if the resetting sensor is active
     * if active reset the variables */
    if(LSM6DS3_IS_ESP_SENSOR(sub_dev_slot_ptr->sensors[0])) {
      if(state->esp_info.esp_if)
        state->esp_info.esp_if->reset(dd_handle);
    }
    return SNS_DDF_SUCCESS;
  }

  //set FIFO CTRL to defaults
  rw_buffer[0] = 0x0;
  rw_buffer[1] = 0x0;
  rw_buffer[2] = 0x0;
  rw_buffer[3] = 0x0;
  rw_buffer[4] = 0x0;

  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_FIFO_CTRL1,
      rw_buffer,
      5,
      0xFF);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write_reg failed- status (%d)",status);
    return status;
  }

  rw_buffer[0] = 0x0;
  rw_buffer[1] = 0x0;

  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_INT1_CTRL,
      rw_buffer,
      2,
      0xFF);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write_reg failed- status (%d)",status);
    return status;
  }

  //async read
  if(state->bus_id == SNS_DDF_BUS_SPI) {
    rw_buffer[0] = 0x30;
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_FIFO_CTRL2,
        rw_buffer,
        1,
        0x30);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "write reg failed  status=%u", status);
      return status;
    }
    //disable I2C in case other SPI device affect LSM6DS3.
    rw_buffer[0] = 0x04;
    status = sns_dd_lsm6ds3_write_reg(dd_handle, STM_LSM6DS3_REG_CTRL4, rw_buffer, 1, 0xFF);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "write_reg failed- status (%d)",status);
      return status;
    }
  }

  // Configure control register 3
  rw_buffer[0] = 0x0
    | (0<<7)           // BOOT bit
    | (1<<6)           // BDU bit
    | (0<<5)           // H_LACTIVE bit
    | (0<<4)           // PP_OD bit
    | (0<<3)           // SIM bit
    | (1<<2)           // IF_INC
    | (0<<1)           // BLE
    | 0;               // SW_RESET
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_CTRL3,
      rw_buffer,
      1,
      0xFF);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write_reg failed- status (%d)",status);
    return status;
  }

  state->acc_cur_rate_idx = -1;

  if(state->acc_range_idx < 0) {
    status = sns_dd_acc_lsm6ds3_set_range(dd_handle, 2);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "set range failed - status (%d)",status);
      return status;
    }
  }
  if(state->acc_lp_mode) {
    rw_buffer[0] = 0
      | (0<<7)	//TRIG_EN (Gyro edge-sensitive trigger)
      | (0<<6)	//LVLen (Gyro level-sensitive trigger)
      | (0<<5)  //LVL2_EN (Gyro level-sensitive latched)
      | (1<<4); //XL_HM_MODE: 0-high-performance enabled; 1-disabled
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_CTRL6_G,
        rw_buffer,
        1,
        0x10);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_1(HIGH,"set acc performance failed - status (%d)",status);
      return status;
    }
  }
  //workaround enable HPF for XL here
  //initialize with high threshold
  rw_buffer[0] = 0x3F;
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_WAKE_THS,
      rw_buffer,
      1,
      0x3F);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write reg failed  status=%u", status);
    return status;
  }

  if (state->acc_lp_mode) {
    rw_buffer[0] = 0
      | (1<<7)            // LPF2_XL_EN: refer to figure 5.
      | (2<<5)            // HPCF_XL[1:0]: 0-SP=ODR/4; 1-HP=ODR/100; 2-ODR/9; 3-ODR/400; 0-LPF2=ODR/50; 1-ODR/100; 2-ODR/9; 3-ODR/400.
      | (1<<2)            // HP_SLOPE_XL_EN:
      | 0;                // LOW_PASS_ON_6D:
  } else {
    rw_buffer[0] = 0
      | (0<<7)            // LPF2_XL_EN: refer to figure 5.
      | (2<<5)            // HPCF_XL[1:0]: 0-SP=ODR/4; 1-HP=ODR/100; 2-ODR/9; 3-ODR/400; 0-LPF2=ODR/50; 1-ODR/100; 2-ODR/9; 3-ODR/400.
      | (0<<2)            // HP_SLOPE_XL_EN:
      | 0;                // LOW_PASS_ON_6D:
  }
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_CTRL8_XL,
      rw_buffer,
      1,
      0xFF);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write reg failed  status=%u", status);
    return status;
  }

  sns_dd_acc_lsm6ds3_set_datarate(dd_handle,
      STM_LSM6DS3_ODR6660);

  rw_buffer[0] = 0
    | (0<<7)            // TIMER_EN:
    | (0<<6)            // PEDO_EN:
    | (0<<5)            // TILT_EN:
    | (1<<4)            // SLOPE_FDS: refer to figure 5.
    | (0<<3)            // TAP_X_EN:
    | (0<<2)            // TAP_Y_EN:
    | (0<<1)            // TAP_Z_EN:
    | 0;                // LIR: 0-interrupt not latched; 1-latched.
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_TAP_CFG,
      rw_buffer,
      1,
      0x10);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write reg failed  status=%u", status);
    return status;
  }

  //settling time for slope/Hp filter, 2 sample time
  sns_ddf_delay(1000);
  status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle, -1);
  if(SNS_DDF_SUCCESS != status)
    return status;

  status = sns_dd_gyr_lsm6ds3_set_datarate(dd_handle, -1, false);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write_reg failed- status (%d)",status);
    return status;
  }

  status = sns_dd_gyr_lsm6ds3_set_range(dd_handle, 3);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "set range failed - status (%d)",status);
    return status;
  }

  //ZRL shift 
  rw_buffer[0] = 0x40;
  status = sns_dd_lsm6ds3_write_reg(dd_handle, 0x00, rw_buffer, 1, 0xFF);
  if(status != SNS_DDF_SUCCESS) return status;
  rw_buffer[0] = 0x10;
  status = sns_dd_lsm6ds3_write_reg(dd_handle, 0x63, rw_buffer, 1, 0x10);
  if(status != SNS_DDF_SUCCESS) return status;
  rw_buffer[0] = 0x80;
  status = sns_dd_lsm6ds3_write_reg(dd_handle, 0x00, rw_buffer, 1, 0xFF);
  if(status != SNS_DDF_SUCCESS) return status;
  rw_buffer[0] = 0x02;
  status = sns_dd_lsm6ds3_write_reg(dd_handle, 0x02, rw_buffer, 1, 0xFF);
  if(status != SNS_DDF_SUCCESS) return status;
  rw_buffer[0] = 0x00;
  status = sns_dd_lsm6ds3_write_reg(dd_handle, 0x00, rw_buffer, 1, 0xFF);
  if(status != SNS_DDF_SUCCESS) return status;

  state->md_enabled = false;
  state->acc_desired_rate_idx = -1;

  state->fifo_info.acc_sample_cnt = 0;
  state->data_intp.acc_intp_status = false;
  state->fifo_info.acc_fifo_alive = false;
  state->data_intp.acc_last_sample_status = false;

  state->gyr_desired_rate_idx = -1;
  state->fifo_info.gyr_sample_cnt = 0;
  state->data_intp.gyr_intp_status = false;
  state->data_intp.gyr_last_sample_status = false;

  state->temp_cur_rate = 0;

  //update fifo parameters
  state->fifo_info.fifo_enabled = 0;
  state->fifo_info.fifo_int_enabled = 0;
  state->fifo_info.active_sensors = 0;
  state->fifo_info.acc_wmk = 0;
  state->fifo_info.gyr_wmk = 0;
  //only reset last_timestamp after reset or after fifo disabled.
  state->fifo_info.last_timestamp = 0;
  state->fifo_info.avg_sample_time = 0;
  if(state->esp_info.esp_if)
    state->esp_info.esp_if->reset(dd_handle);

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Initializes the lsm6ds3 driver and sets up sensor.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm6ds3_init(
    sns_ddf_handle_t* dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors)
{
  sns_ddf_status_e status;
  sns_dd_lsm6ds3_state_t* state;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;
  uint32_t my_num_sensors = 0;
  int32_t sub_dev_idx = 0;
  sns_ddf_time_t cur_time = 0;
  sns_ddf_handle_t dd_handle = NULL;

#if UIMAGE_SUPPORT
  /* Update smgr sensor data for the driver to indicate uImage support */
  sns_ddf_smgr_set_uimg_refac(smgr_handle);

#endif

  /* check if this init() is a call for an existing device */
  state = dd_find_same_port_dev(&device_info->port_config);

  LSM6DS3_MSG_2(HIGH, "INIT num_sensors (%d) sensor %d",*num_sensors, *sensors[0]);
  if ( NULL == state)
  {
    LSM6DS3_MSG_0(HIGH, "INIT state NULL");
    /* Allocate a device instance and initialize it */
    status = sns_dd_lsm6ds3_malloc((void**)&state, sizeof(sns_dd_lsm6ds3_state_t), smgr_handle);
    if(status != SNS_DDF_SUCCESS)
      return SNS_DDF_ENOMEM;
    memset(state, 0, sizeof(sns_dd_lsm6ds3_state_t));
    //get sub dev from state
    sub_dev_idx = (dd_handle_align_t)state & (~DD_HANDLE_ALIGN_BITS);
    //state->sub_dev[sub_dev_idx].state_ptr = state;
    //*dd_handle_ptr = &state->sub_dev[sub_dev_idx];

    state->next = lsm6ds3_dd_head.next;
    lsm6ds3_dd_head.next = state;
    dd_handle = (sns_ddf_handle_t )state;
    *dd_handle_ptr = state;
    LSM6DS3_MSG_1(HIGH, "state (%p) ", state);
    LSM6DS3_MSG_3(HIGH, "dd_handle (%p) sub_dev_addr (%p) sub_dev (%d)",dd_handle, &state->sub_dev[sub_dev_idx], sub_dev_idx);
    //*dd_handle_ptr = dd_handle;
    //state->sub_dev[sub_dev_idx].sub_dev_idx = sub_dev_idx;
    sns_ddf_get_port_id(&device_info->port_config,
        &state->port_id);

    /* this init() call is for first sub_dev */
    my_num_sensors = dd_fill_sub_dev_slot(&state->sub_dev[sub_dev_idx],
        *num_sensors,
        *sensors);
    *num_sensors = my_num_sensors;
    *sensors = state->sub_dev[sub_dev_idx].sensors;
    LSM6DS3_MSG_2(HIGH, "INIT num_sensors (%d) sensor %d",*num_sensors, *sensors[0]);
    if ( 0 == my_num_sensors ) {
      sns_dd_lsm6ds3_mfree(state, smgr_handle);
      lsm6ds3_dd_head.next = NULL;
      return SNS_DDF_EINVALID_DATA;
    }

    // Open communication port to the device.
    status = sns_ddf_open_port(&state->port_handle,
        &device_info->port_config);
    if(status != SNS_DDF_SUCCESS)
    {
      LSM6DS3_MSG_1(HIGH, "INIT status %u open port failed", status);
      sns_dd_lsm6ds3_mfree(state, smgr_handle);
      lsm6ds3_dd_head.next = NULL;
      return status;
    }

    LSM6DS3_MSG_0(HIGH, "INIT Open port success");
    // confirm WHO_AM_I register
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LSM6DS3_REG_WHO_AM_I,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "read_port failed- status (%d)",status);
      sns_ddf_close_port(state->port_handle);
      sns_dd_lsm6ds3_mfree(state, smgr_handle);
      lsm6ds3_dd_head.next = NULL;
      return status;
    }
    if((rw_bytes!=1) ||
        (rw_buffer != STM_LSM6DS3_WHOAMI_VALUE)) {
      LSM6DS3_MSG_E_3(ERROR, "read_port failed (bytes count or WHOAMI failed)-\
          rw_bytes (%d) rw_buffer (0x%x) WHOAMI (0x%x)",rw_bytes, rw_buffer, STM_LSM6DS3_WHOAMI_VALUE);
      sns_ddf_close_port(state->port_handle);
      sns_dd_lsm6ds3_mfree(state, smgr_handle);
      lsm6ds3_dd_head.next = NULL;
      return SNS_DDF_EFAIL;
    }

    LSM6DS3_MSG_1(HIGH, "INIT WHO AM I success value (0x%x)", rw_buffer);

    state->axes_mapped = false;
    if((*sensors[0] == SNS_DDF_SENSOR_ACCEL) ||
        (*sensors[0] == SNS_DDF_SENSOR_GYRO)) {
      //copy axis config from NV_PARAMS
      sns_ddf_axes_map_init(&state->axes_map,
          ((nv_params != NULL) ? nv_params->data : stm_lsm6ds3_axis_dflt));
      state->axes_mapped = true;
    }

    // Fill out output and driver parameters.
    state->bus_id = device_info->port_config.bus;
    state->sub_dev[sub_dev_idx].smgr_handle = smgr_handle;
    state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    state->gpio_num = device_info->first_gpio;
    state->gpio2_num = device_info->second_gpio;
    state->acc_desired_rate_idx = -1;
    state->gyr_desired_rate_idx = -1;
    state->timer = NULL;
    state->int_registered = false;
    state->fifo_info.acc_data.samples = NULL;
    state->fifo_info.temp_data.samples = NULL;
    state->fifo_info.gyr_data.samples = NULL;
    state->fifo_info.raw_buffer = NULL;
    state->fifo_info.fifo_enabled = 0;
    state->fifo_info.fifo_int_enabled = 0;
    state->fifo_info.temp_sample_cnt = 0;
    state->fifo_info.acc_wmk = 0;
    state->fifo_info.gyr_wmk = 0;
    //initializing fifo pattern array, acc and gyr odr are same
    //TODO need to change if acc and gyr odr are different
    state->fifo_info.pattern_samples_cnt = 2;//only acc and gyr
    //gyr is 2 and acc is 1
    state->fifo_info.pattern_array[0] = 2;
    state->fifo_info.pattern_array[1] = 1;
    state->fifo_info.active_sensors = 0;
    state->fifo_info.fifo_rate_idx = -1;
    state->acc_range_idx = -1;
    state->gyr_range_idx = -1;
    state->acc_cur_rate_idx = -1;
    state->gyr_cur_rate_idx = -1;
    state->temp_cur_rate = 0;
    state->gyr_sleep = false;
    state->esp_info.esp_enabled = 0;
#if ACC_LOWPOWER_MODE
    state->acc_lp_mode = true;
#else
    state->acc_lp_mode = false;
#endif
#if LSM6DS3_ENABLE_ESP
    state->esp_info.esp_if = &sns_dd_lsm6ds3_esp_if;
#else
    state->esp_info.esp_if = NULL;
#endif
    if(LSM6DS3_IS_ESP_SENSOR(*sensors[0])) {
      LSM6DS3_MSG_0(HIGH, "INIT sensor is ESP SENSOR");
      if(state->esp_info.esp_if == NULL) {
        sns_ddf_close_port(state->port_handle);
        sns_dd_lsm6ds3_mfree(state, smgr_handle);
        lsm6ds3_dd_head.next = NULL;
        status = SNS_DDF_EINVALID_PARAM;
        LSM6DS3_MSG_E_2(ERROR, "ESP not enabled - Init failed sensor (%d) status (%d)",*sensors[0], status);
        return status;
      }
    }
    //sw reset
    rw_buffer = 0x01; //set SW_RESET
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_CTRL3,
        &rw_buffer,
        1,
        0x01);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "write_reg failed- status (%d)",status);
      sns_ddf_close_port(state->port_handle);
      sns_dd_lsm6ds3_mfree(state, smgr_handle);
      lsm6ds3_dd_head.next = NULL;
      return status;
    }
    cur_time = sns_ddf_get_timestamp();
    do {
      if(sns_ddf_get_timestamp() > (cur_time + sns_ddf_convert_usec_to_tick(10000))) {
        LSM6DS3_MSG_E_3(ERROR, "cur_time (%u) now (%u) delay (%u) ",
            cur_time, sns_ddf_get_timestamp(), sns_ddf_convert_usec_to_tick(10000));
        status = SNS_DDF_EFAIL;
      } else {
        //1ms wait
        sns_ddf_delay(1000);
        status = sns_ddf_read_port(
            state->port_handle,
            STM_LSM6DS3_REG_CTRL3,
            &rw_buffer,
            1,
            &rw_bytes);
        if(status != SNS_DDF_SUCCESS) {
          LSM6DS3_MSG_E_1(ERROR, "read_port failed- status (%d)",status);
        }
        if(rw_bytes != 1) {
          LSM6DS3_MSG_E_1(ERROR, "read_port failed-  read_count  does not match - read_count (%d)",rw_bytes);
          status = SNS_DDF_EBUS;
        }
      }
      if(status != SNS_DDF_SUCCESS) {
        sns_ddf_close_port(state->port_handle);
        sns_dd_lsm6ds3_mfree(state, smgr_handle);
        lsm6ds3_dd_head.next = NULL;
        return status;
      }
    } while((rw_buffer & 0x01));

    LSM6DS3_MSG_0(HIGH, "INIT sensor HW reset success");
    status = sns_dd_lsm6ds3_reset(dd_handle);
    if(status != SNS_DDF_SUCCESS)
    {
      LSM6DS3_MSG_E_1(ERROR, "reset failed  status=%u", status);
      sns_ddf_close_port(state->port_handle);
      sns_dd_lsm6ds3_mfree(state, smgr_handle);
      lsm6ds3_dd_head.next = NULL;
      return status;
    }
    LSM6DS3_MSG_0(HIGH, "INIT sensor SW reset success");
    if(state->esp_info.esp_if)
      state->esp_info.esp_if->init(dd_handle_ptr,
          smgr_handle,
          nv_params,
          device_info,
          num_devices,
          memhandler,
          sensors,
          num_sensors);

    //only reset last_timestamp after reset or after fifo disabled.
    state->fifo_info.last_timestamp = 0;
    state->fifo_info.avg_sample_time = 0;

    status =  sns_ddf_signal_register(
        state->gpio_num,
        state,
        &sns_dd_lsm6ds3_if,
        SNS_DDF_SIGNAL_IRQ_RISING);

    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_0(HIGH, "INIT sensor signal register failed");
      sns_ddf_close_port(state->port_handle);
      sns_dd_lsm6ds3_mfree(state, smgr_handle);
      lsm6ds3_dd_head.next = NULL;
      return status;
    }

    //update the head pointer if everything is successful
    /* link this dev instance to the front of the list */
    LSM6DS3_MSG_2(HIGH, "INIT Success sub_dev_idx (%d) sensor %d",sub_dev_idx, *sensors[0]);


  }
  else /* case the device instance is already there */
  {
    LSM6DS3_MSG_0(HIGH, "INIT state not NULL");

    sub_dev_idx = dd_find_free_sub_dev_slot(state);
    if ( -1 == sub_dev_idx )
    {
      LSM6DS3_MSG_0(ERROR, "no free sub dev slot");
      return SNS_DDF_ENOMEM;
    }
    else
    {
      my_num_sensors = dd_fill_sub_dev_slot(&state->sub_dev[sub_dev_idx],
          *num_sensors,
          *sensors);

      // Fill out output parameters.
      *num_sensors = my_num_sensors;
      *sensors = state->sub_dev[sub_dev_idx].sensors;

      if ( 0 == my_num_sensors )
        return SNS_DDF_EINVALID_DATA;
      if(LSM6DS3_IS_ESP_SENSOR(*sensors[0])) {
        if(state->esp_info.esp_if == NULL) {
          status = SNS_DDF_EINVALID_PARAM;
          LSM6DS3_MSG_E_2(ERROR, "ESP not enabled - Init failed sensor (%d) status (%d)",*sensors[0], status);
          return status;
        }
      }
      dd_handle = (sns_ddf_handle_t)(((dd_handle_align_t)state & DD_HANDLE_ALIGN_BITS) | (dd_handle_align_t)sub_dev_idx);
      //dd_handle = (sns_ddf_handle_t)(&state->sub_dev[sub_dev_idx]);
      LSM6DS3_MSG_3(HIGH, "dd_handle (%p) sub_dev_addr (%p) sub_dev (%d)",dd_handle, &state->sub_dev[sub_dev_idx], sub_dev_idx);
      *dd_handle_ptr = dd_handle;
      state->sub_dev[sub_dev_idx].smgr_handle = smgr_handle;
      state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;


      if (state->axes_mapped != true) {
        if((*sensors[0] == SNS_DDF_SENSOR_ACCEL) ||
            (*sensors[0] == SNS_DDF_SENSOR_GYRO)) {
          //copy axis config from NV_PARAMS
          sns_ddf_axes_map_init(&state->axes_map,
              ((nv_params != NULL) ? nv_params->data : stm_lsm6ds3_axis_dflt));
          state->axes_mapped = true;
        }
      }
    }
    LSM6DS3_MSG_2(HIGH, "INIT Success sub_dev_idx (%d) sensor %d",sub_dev_idx, *sensors[0]);
  }

  if (*sensors[0] == SNS_DDF_SENSOR_GYRO) {
    state->gyr_sstvt_comp[0] = device_info[0].device_select & 0x0F;
    if (state->gyr_sstvt_comp[0] & 0x08)
      state->gyr_sstvt_comp[0] |= 0xF0;
    state->gyr_sstvt_comp[1] = (int8_t)(device_info[0].device_select&0xF0) >> 4;
  } else if (*sensors[0] == SNS_DDF_SENSOR_ACCEL) {
	  state->gyr_sstvt_comp[2] = device_info[0].device_select & 0x0F;
	  if (state->gyr_sstvt_comp[2] & 0x08)
      state->gyr_sstvt_comp[2] |= 0xF0;
  }
  LSM6DS3_MSG_3(HIGH, "INIT state (%p) dd_handle_ptr (%p) sub_dev_idx %d ",state, *dd_handle_ptr, sub_dev_idx);
  return SNS_DDF_SUCCESS;
}

/**
 * @brief Retrieves the value of an attribute for a lsm6ds3 sensor.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm6ds3_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems)
{
  sns_dd_lsm6ds3_state_t *state;
  uint32_t sub_dev_idx;
  sns_ddf_sensor_e ddf_sensor_type;

  if (dd_handle == NULL || value == NULL) {
    LSM6DS3_MSG_E_0(ERROR, "dd_handle is NULL");
    return SNS_DDF_EINVALID_PARAM;
  }

  state = sns_dd_lsm6ds3_get_state(dd_handle);
  sub_dev_idx = (dd_handle_align_t) dd_handle & (~DD_HANDLE_ALIGN_BITS);
  ddf_sensor_type = state->sub_dev[sub_dev_idx].sensors[0]; // '0' --> Primary
  LSM6DS3_MSG_3(LOW, "GET_ATTRIB attrib (%d) sensor (%d) p_sensor (%d)",attrib, sensor, ddf_sensor_type);

  if((attrib != SNS_DDF_ATTRIB_IO_REGISTER) &&
      (attrib != SNS_DDF_ATTRIB_DRIVER_INFO)) {
    if(LSM6DS3_IS_ESP_SENSOR(ddf_sensor_type)) {
      if(state->esp_info.esp_if)
        return state->esp_info.esp_if->get_attrib(dd_handle, sensor, attrib, memhandler, value, num_elems);
      else
        return SNS_DDF_EINVALID_PARAM;
    }
  }

  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_INFO:
      {
        sns_ddf_power_info_s* power = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler, sizeof(sns_ddf_power_info_s), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == power)
          return SNS_DDF_ENOMEM;
        //current consumption, unit uA
        if(ddf_sensor_type == SNS_DDF_SENSOR_ACCEL) {
          power->active_current = 900;
          power->lowpower_current = 6;
        }
        else if(ddf_sensor_type == SNS_DDF_SENSOR_GYRO){
          power->active_current = 900;
          power->lowpower_current = 6;
        }
        else {
          return SNS_DDF_EINVALID_PARAM;
        }
        *value = power;
        *num_elems = 1;
        return SNS_DDF_SUCCESS;
      }

    case SNS_DDF_ATTRIB_RANGE:
      {
        sns_ddf_range_s* ranges = NULL;
        if(sensor == SNS_DDF_SENSOR_ACCEL)
        {
          ranges = sns_dd_lsm6ds3_memhandler_malloc(
              memhandler, STM_LSM6DS3_ACC_RANGE_NUM * sizeof(sns_ddf_range_s), state->sub_dev[sub_dev_idx].smgr_handle);
          if(NULL == ranges)
            return SNS_DDF_ENOMEM;
          ranges[0].min = STM_LSM6DS3_ACC_RANGE_2G_MIN;
          ranges[0].max = STM_LSM6DS3_ACC_RANGE_2G_MAX;
          ranges[1].min = STM_LSM6DS3_ACC_RANGE_4G_MIN;
          ranges[1].max = STM_LSM6DS3_ACC_RANGE_4G_MAX;
          ranges[2].min = STM_LSM6DS3_ACC_RANGE_8G_MIN;
          ranges[2].max = STM_LSM6DS3_ACC_RANGE_8G_MAX;
          ranges[3].min = STM_LSM6DS3_ACC_RANGE_16G_MIN;
          ranges[3].max = STM_LSM6DS3_ACC_RANGE_16G_MAX;
          *num_elems = STM_LSM6DS3_ACC_RANGE_NUM;
        }
        else if(sensor == SNS_DDF_SENSOR_GYRO)
        {
          ranges = sns_dd_lsm6ds3_memhandler_malloc(
              memhandler, STM_LSM6DS3_GYR_RANGE_NUM * sizeof(sns_ddf_range_s), state->sub_dev[sub_dev_idx].smgr_handle);
          if(NULL == ranges)
            return SNS_DDF_ENOMEM;

          ranges[0].min = STM_LSM6DS3_GYR_RANGE_245_MIN;
          ranges[0].max = STM_LSM6DS3_GYR_RANGE_245_MAX;
          ranges[1].min = STM_LSM6DS3_GYR_RANGE_500_MIN;
          ranges[1].max = STM_LSM6DS3_GYR_RANGE_500_MAX;
          ranges[2].min = STM_LSM6DS3_GYR_RANGE_1000_MIN;
          ranges[2].max = STM_LSM6DS3_GYR_RANGE_1000_MAX;
          ranges[3].min = STM_LSM6DS3_GYR_RANGE_2000_MIN;
          ranges[3].max = STM_LSM6DS3_GYR_RANGE_2000_MAX;
          *num_elems = STM_LSM6DS3_GYR_RANGE_NUM;
        }
        else if(sensor == SNS_DDF_SENSOR_TEMP)
        {
          ranges = sns_dd_lsm6ds3_memhandler_malloc(
              memhandler, sizeof(sns_ddf_range_s), state->sub_dev[sub_dev_idx].smgr_handle);
          if(NULL == ranges)
            return SNS_DDF_ENOMEM;
          ranges->min = FX_CONV_Q16((int32_t)-40,0);
          ranges->max = FX_CONV_Q16((int32_t)85,0);
          *num_elems = 1;
        }
        else
        {
          return SNS_DDF_EINVALID_PARAM;
        }
        *value = ranges;
        return SNS_DDF_SUCCESS;
      }

    case SNS_DDF_ATTRIB_RESOLUTION:
      {
        sns_ddf_resolution_t *res = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler, sizeof(sns_ddf_resolution_t), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == res)
          return SNS_DDF_ENOMEM;

        if(sensor == SNS_DDF_SENSOR_ACCEL)
          *res = FX_FLTTOFIX_Q16(state->acc_sstvt_adj[0] * G / 1000000);
        else if(sensor == SNS_DDF_SENSOR_GYRO)
          *res = FX_DIV_Q16(state->gyr_sstvt_adj[0], FX_CONV_Q16((int32_t)1000,0));
        else if(sensor == SNS_DDF_SENSOR_TEMP)
          *res = FX_DIV_Q16(FX_CONV_Q16((int32_t)1,0), FX_CONV_Q16((int32_t)16,0));
        else
          return SNS_DDF_EINVALID_PARAM;

        *value = res;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
      {
        sns_ddf_resolution_adc_s *res = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler ,sizeof(sns_ddf_resolution_adc_s), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == res)
          return SNS_DDF_ENOMEM;

        if((sensor == SNS_DDF_SENSOR_ACCEL) ||
            (sensor == SNS_DDF_SENSOR_GYRO))
        {
          res->bit_len = 16;
          res->max_freq = 100;
        }
        else if(sensor == SNS_DDF_SENSOR_TEMP)
        {
          res->bit_len = 16;
          res->max_freq = 1;
        }
        else
          return SNS_DDF_EINVALID_PARAM;
        *value = res;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }

    case SNS_DDF_ATTRIB_LOWPASS:
      {
        if((sensor == SNS_DDF_SENSOR_ACCEL) ||
        (sensor == SNS_DDF_SENSOR_GYRO))
        {
          *value = lsm6ds3_freqs;
          *num_elems = STM_LSM6DS3_ODR_NUM;
        }
         else
            return SNS_DDF_EINVALID_ATTR;


        return SNS_DDF_SUCCESS;
      }
    case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
      {
        if((sensor == SNS_DDF_SENSOR_ACCEL) ||
            (sensor == SNS_DDF_SENSOR_GYRO)) {

          *value = lsm6ds3_odr;
          *num_elems = STM_LSM6DS3_ODR208 + 1; //index
        } else
          return SNS_DDF_EINVALID_ATTR;

        return SNS_DDF_SUCCESS;
      }

    case SNS_DDF_ATTRIB_ODR:
      {
        //int8_t lsm6ds3_rate_index = -1;
        sns_ddf_odr_t *res = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler ,sizeof(sns_ddf_odr_t), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == res)
          return SNS_DDF_ENOMEM;

        if(sensor == SNS_DDF_SENSOR_ACCEL) {
          if(state->acc_desired_rate_idx >= 0) {
            *res = lsm6ds3_odr[state->acc_cur_rate_idx];
          } else
            *res = 0;
        } else if(sensor == SNS_DDF_SENSOR_GYRO) {
          if(state->gyr_desired_rate_idx >= 0) {
            *res = lsm6ds3_odr[state->gyr_cur_rate_idx];
          } else
            *res = 0;
        }
        else if(sensor == SNS_DDF_SENSOR_TEMP)
        {
          if(ddf_sensor_type == SNS_DDF_SENSOR_ACCEL)
            *res = (state->temp_cur_rate & 0x0F) ? 1 : 0;
          else if(ddf_sensor_type == SNS_DDF_SENSOR_GYRO)
            *res = ((state->temp_cur_rate & 0xF0) >> 4) ? 1 : 0;
        }
        else
          return SNS_DDF_EINVALID_PARAM;

        LSM6DS3_MSG_2(LOW, "Get_ATTRIB_ODR sensor(%d) odr (%d)",sensor, *res);

        *value = res;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }
    case SNS_DDF_ATTRIB_DELAYS:
      {
        sns_ddf_delays_s *lsm6ds3_delays = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler, sizeof(sns_ddf_delays_s), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == lsm6ds3_delays)
          return SNS_DDF_ENOMEM;

        if(sensor == SNS_DDF_SENSOR_ACCEL)
        {
          lsm6ds3_delays->time_to_active = 1;
          lsm6ds3_delays->time_to_data = 1;
        }
        else
        {
          lsm6ds3_delays->time_to_active = 1;
          lsm6ds3_delays->time_to_data = 1;
        }
        *value = lsm6ds3_delays;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }
   case SNS_DDF_ATTRIB_BIAS:
      {
        if(sensor == SNS_DDF_SENSOR_ACCEL)
        {
          // for accel
          *value = state->xl_biases;
          *num_elems = ARR_SIZE(state->xl_biases);

          return SNS_DDF_SUCCESS;
        }
        else if(sensor == SNS_DDF_SENSOR_GYRO)
        {
          // for accel
          *value = state->g_biases;
          *num_elems = ARR_SIZE(state->g_biases);

          return SNS_DDF_SUCCESS;
        }
        else
        {
          LSM6DS3_MSG_E_1(ERROR, " Invalid sensor (%d) for getting bias ", sensor);
          return SNS_DDF_EINVALID_ATTR;
        }
      }
     case SNS_DDF_ATTRIB_FIFO:
      {
        uint32_t s_dev_idx;
        sns_ddf_status_e status;
        sns_ddf_fifo_share_sensor_s* shared_sensors = NULL;

        sns_ddf_fifo_attrib_get_s *res = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler ,sizeof(sns_ddf_fifo_attrib_get_s), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == res)
          return SNS_DDF_ENOMEM;

        res->is_supported = true;
        res->is_sw_watermark = false;
        res->max_watermark = STM_LSM6DS3_MAX_FIFO;


        if(sensor == SNS_DDF_SENSOR_GYRO) {
          status = lsm6ds3_search_subdev_by_sensortype(
              state,
              SNS_DDF_SENSOR_ACCEL,
              &s_dev_idx);
          if(status != SNS_DDF_SUCCESS) {
            res->share_sensor_cnt = 0;
            res->share_sensors[0] = NULL;
          } else {
            res->share_sensor_cnt = 1;
            shared_sensors = sns_dd_lsm6ds3_memhandler_malloc(
                memhandler ,res->share_sensor_cnt * sizeof(sns_ddf_fifo_share_sensor_s), state->sub_dev[sub_dev_idx].smgr_handle);

            if(NULL == shared_sensors)
              return SNS_DDF_ENOMEM;
            shared_sensors[0].sensor =  SNS_DDF_SENSOR_ACCEL;
            shared_sensors[0].dd_handle = (sns_ddf_handle_t)((dd_handle_align_t)state |
                (dd_handle_align_t)s_dev_idx);
            res->share_sensors[0] = shared_sensors;
          }
        } else if(sensor == SNS_DDF_SENSOR_ACCEL) {
          status = lsm6ds3_search_subdev_by_sensortype(
              state,
              SNS_DDF_SENSOR_GYRO,
              &s_dev_idx);
          if(status != SNS_DDF_SUCCESS) {
            res->share_sensor_cnt = 0;
            res->share_sensors[0] = NULL;
          } else {
            res->share_sensor_cnt = 1;
            shared_sensors = sns_dd_lsm6ds3_memhandler_malloc(
                memhandler ,res->share_sensor_cnt * sizeof(sns_ddf_fifo_share_sensor_s), state->sub_dev[sub_dev_idx].smgr_handle);
            if(NULL == shared_sensors)
              return SNS_DDF_ENOMEM;

            shared_sensors[0].sensor =  SNS_DDF_SENSOR_GYRO;
            shared_sensors[0].dd_handle = (sns_ddf_handle_t)((dd_handle_align_t)state |
                (dd_handle_align_t)s_dev_idx);
            res->share_sensors[0] = shared_sensors;
          }
        }else if(sensor == SNS_DDF_SENSOR_TEMP) {
          res->share_sensor_cnt = 0;
          res->share_sensors[0] = NULL;
        }
        else
        {
          LSM6DS3_MSG_E_1(ERROR, " Invalid sensor (%d) ", sensor);
          return SNS_DDF_EINVALID_ATTR;
        }

        *value = res;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }
    case SNS_DDF_ATTRIB_ODR_TOLERANCE:
      {
        q16_t *res = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler ,sizeof(q16_t), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == res)
          return SNS_DDF_ENOMEM;
        if(sensor == SNS_DDF_SENSOR_ACCEL)
        {
          *res = FX_FLTTOFIX_Q16(0.05);
        }
        else if(sensor == SNS_DDF_SENSOR_GYRO)
        {
          *res = FX_FLTTOFIX_Q16(0.05);
        }
        else
        {
          LSM6DS3_MSG_E_1(ERROR, " Invalid sensor (%d) ", sensor);
          return SNS_DDF_EINVALID_ATTR;
        }
        *value = res;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }
      case SNS_DDF_ATTRIB_IO_REGISTER:
      {
        sns_ddf_io_register_s* io_reg_in = (sns_ddf_io_register_s *) (*value);
        uint8_t bytes_read;
        sns_ddf_status_e status;

        if ((uint32_t)(io_reg_in->address_ptr) > 0xFFFF)
        {
          LSM6DS3_MSG_E_1(ERROR, " Invalid address_ptr (0x%x)", io_reg_in->address_ptr);
          return SNS_DDF_EINVALID_PARAM;
        }
        if (io_reg_in->length > 0xFF)
        {
          LSM6DS3_MSG_E_1(ERROR, " Invalid length (%d)", io_reg_in->length);
          return SNS_DDF_EINVALID_PARAM;
        }

        // Allocate memory for the output data structure and for the data to be read from the register(s)
        sns_ddf_io_register_s* io_reg_out = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler, sizeof(sns_ddf_io_register_s) + (sizeof(uint8_t) * (io_reg_in->length - 1)),
          state->sub_dev[sub_dev_idx].smgr_handle);
        if (io_reg_out == NULL) {
          LSM6DS3_MSG_E_1(ERROR, " Malloc failed ", 0);
          return SNS_DDF_ENOMEM;
        }

        status = sns_ddf_read_port(
            state->port_handle,
            (uint16_t) ((uint32_t)io_reg_in->address_ptr),
            &(io_reg_out->data[0]),
            (uint8_t) io_reg_in->length,
            &bytes_read);
      if(status != SNS_DDF_SUCCESS) {
          LSM6DS3_MSG_E_1(ERROR, "read_port failed status (%d) ", status);
          return status;
        }
        if(bytes_read != (uint8_t) io_reg_in->length) {
          LSM6DS3_MSG_E_2(ERROR, "read_port failed bytes_read (%d) doesn't match \
              length (%d) ", bytes_read, (uint8_t) io_reg_in->length);
          return SNS_DDF_EBUS;
        }

        // Fill out the information in the output io_register data structure
        io_reg_out->address_ptr = io_reg_in->address_ptr;
        io_reg_out->addr_fixed = io_reg_in->addr_fixed;
        io_reg_out->data_width = io_reg_in->data_width;
        io_reg_out->length = (uint32_t) bytes_read;
        // Return the io_register data structure by reference
        *value = io_reg_out;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }
    case SNS_DDF_ATTRIB_DRIVER_INFO:
      {
        sns_ddf_driver_info_s *info = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler, sizeof(sns_ddf_driver_info_s), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == info)
          return SNS_DDF_ENOMEM;

        info->name = "STMicroelectronics LSM6DS3 Driver";
        info->version = 40190; //04.019.0
        *value = info;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }

    case SNS_DDF_ATTRIB_DEVICE_INFO:
      {
        sns_ddf_device_info_s *info = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler, sizeof(sns_ddf_device_info_s), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == info)
          return SNS_DDF_ENOMEM;

        if(ddf_sensor_type == SNS_DDF_SENSOR_ACCEL)
          info->name = "Accelerometer";
        else if(ddf_sensor_type == SNS_DDF_SENSOR_GYRO)
          info->name = "Gyroscope";
        else if(ddf_sensor_type == SNS_DDF_SENSOR_TEMP)
          info->name = "Thermometer";
        else
          return SNS_DDF_EINVALID_PARAM;

        info->vendor = "STMicroelectronics";
        info->model = "LSM6DS3";
        info->version = 1;
        *value = info;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }
    default:
      return SNS_DDF_EINVALID_ATTR;
  }
}


/**
 * @brief Performs a set accelerometer power state operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  power_state    Power state to be set.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lsm6ds3_set_power(
    sns_ddf_handle_t dd_handle,
    sns_ddf_powerstate_e power_state)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint32_t sub_dev_idx;
  lsm6ds3_search_subdev_by_sensortype(state, SNS_DDF_SENSOR_ACCEL, &sub_dev_idx);

  if(SNS_DDF_POWERSTATE_LOWPOWER == power_state)
  {
    LSM6DS3_MSG_3(LOW, "SET_ACC_POWER - -> LOWPOWER rate %d fifo_enabled %d fifo_int_enabled %d",
        state->acc_cur_rate_idx, state->fifo_info.fifo_enabled, state->fifo_info.fifo_int_enabled);
    // This can call at any stage, make sure to handle well
    if(state->md_enabled)
      sns_dd_lsm6ds3_disable_md(dd_handle);
    state->temp_cur_rate &= ~0x0F;
    if(state->fifo_info.fifo_enabled & 0x04)
      sns_dd_lsm6ds3_temp_set_rate(dd_handle, 0);
    if(state->acc_cur_rate_idx >= 0)
      sns_dd_acc_lsm6ds3_attrib_set_datarate(dd_handle, sub_dev_idx, 0);
    if(state->fifo_info.fifo_int_enabled & 0x01)
      sns_dd_lsm6ds3_disable_fifo_int(dd_handle, SNS_DDF_SENSOR_ACCEL);
    if(state->fifo_info.fifo_enabled & 0x01)
      sns_dd_lsm6ds3_disable_fifo(dd_handle, SNS_DDF_SENSOR_ACCEL);

    state->acc_desired_rate_idx = -1;

    //if gyro is enabled do not set fifo acc params
    //will be set when gyro disables
    if(!(state->fifo_info.fifo_enabled & 0x02)) {
      state->fifo_info.acc_sample_cnt = 0;
      state->data_intp.acc_intp_status = false;
      state->fifo_info.acc_fifo_alive = false;
      state->data_intp.acc_last_sample_status = false;
    }
    state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
  }
  else if(SNS_DDF_POWERSTATE_ACTIVE == power_state)
  {
    LSM6DS3_MSG_0(LOW, "SET_ACC_POWER - power_state to ACTIVE");
    sns_dd_lsm6ds3_reset(dd_handle);
    state->acc_desired_rate_idx = -1;
    state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_ACTIVE;
  }
  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set gyrocope power state operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  power_state    Power state to be set.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_gyr_lsm6ds3_set_power(
    sns_ddf_handle_t dd_handle,
    sns_ddf_powerstate_e power_state)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint32_t sub_dev_idx;

  lsm6ds3_search_subdev_by_sensortype(state, SNS_DDF_SENSOR_GYRO, &sub_dev_idx);

  if(SNS_DDF_POWERSTATE_LOWPOWER == power_state)
  {
    LSM6DS3_MSG_3(LOW, "SET_ACC_POWER - -> LOWPOWER rate %d fifo_enabled %d fifo_int_enabled %d",
        state->gyr_cur_rate_idx, state->fifo_info.fifo_enabled, state->fifo_info.fifo_int_enabled);
    //gyro sleep mode to Power down -- acc streaming no effect

    state->temp_cur_rate &= ~0xF0;
    if(state->fifo_info.fifo_enabled & 0x08)
      sns_dd_lsm6ds3_temp_set_rate(dd_handle, 0);
    if(state->gyr_cur_rate_idx >= 0)
      sns_dd_gyr_lsm6ds3_attrib_set_datarate(dd_handle, sub_dev_idx, 0);
    if(state->fifo_info.fifo_int_enabled & 0x02)
      sns_dd_lsm6ds3_disable_fifo_int(dd_handle, SNS_DDF_SENSOR_GYRO);
    if(state->fifo_info.fifo_enabled & 0x02)
      sns_dd_lsm6ds3_disable_fifo(dd_handle, SNS_DDF_SENSOR_GYRO);

    state->gyr_odr_settled_ts = 0;
    state->gyr_desired_rate_idx = -1;
    state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;

  }
  else if(SNS_DDF_POWERSTATE_ACTIVE == power_state)
  {
    LSM6DS3_MSG_0(LOW, "SET_GYR_POWER - power_state to ACTIVE");
    //set lowest odr
    //status = sns_dd_gyr_lsm6ds3_set_datarate(dd_handle, 0);
    //keeping device in sleep mode
    //status = sns_dd_gyr_lsm6ds3_set_datarate(dd_handle, -1);
    sns_dd_lsm6ds3_reset(dd_handle);
    state->gyr_desired_rate_idx = -1;
    state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_ACTIVE;
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set dynamic range operation for accelerometer
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  range          Dynamic range settings for LSM6DS3
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lsm6ds3_set_range(
    sns_ddf_handle_t dd_handle,
    int8_t range_idx)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status;
  uint8_t write_buffer = 0;
  uint8_t read_count = 0;
  stm_lsm6ds3_acc_sstvt sstvt;

  if((range_idx < 0) ||
    (range_idx > (STM_LSM6DS3_ACC_RANGE_NUM -1))) {
    LSM6DS3_MSG_E_1(ERROR, "SET_GYR_RANGE - invalid range_idx - (%d)",range_idx);
    return SNS_DDF_EINVALID_PARAM;
  }
  if(state->acc_range_idx >= 0) {
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LSM6DS3_REG_CTRL1_A,
        &write_buffer,
        1,
        &read_count);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "read_port failed- status (%d)",status);
      return status;
    }
    if(read_count != 1) {
      LSM6DS3_MSG_E_1(ERROR, "read_port failed-  read_count  does not match - read_count (%d)",read_count);
      return SNS_DDF_EBUS;
    }
    LSM6DS3_MSG_2(LOW, "SET_ACC_RANGE - set range_idx - (%d) ctrl1_a 0x%x",range_idx, write_buffer);
    if(lsm6ds3_acc_ranges[range_idx]  == (write_buffer & STM_LSM6DS3_FS_XL_MASK))
      return SNS_DDF_SUCCESS;
  }
  LSM6DS3_MSG_2(LOW, "SET_ACC_RANGE - set range_idx - (%d) reg_value (%x)",range_idx, lsm6ds3_acc_ranges[range_idx]);
  //set sensor sensitivity for data conversion
  switch (lsm6ds3_acc_ranges[range_idx]) {
    case STM_LSM6DS3_ACC_RANGE_2G:
      sstvt = STM_LSM6DS3_ACC_SSTVT_2G;
      break;
    case STM_LSM6DS3_ACC_RANGE_4G:
      sstvt = STM_LSM6DS3_ACC_SSTVT_4G;
      break;
    case STM_LSM6DS3_ACC_RANGE_8G:
      sstvt = STM_LSM6DS3_ACC_SSTVT_8G;
      break;
    case STM_LSM6DS3_ACC_RANGE_16G:
      sstvt = STM_LSM6DS3_ACC_SSTVT_16G;
      break;
    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  // Configure CTRL_FS register
  write_buffer = ((uint8_t)lsm6ds3_acc_ranges[range_idx]);
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_CTRL1_A,
      &write_buffer,
      1,
      STM_LSM6DS3_FS_XL_MASK);
  if(status != SNS_DDF_SUCCESS)
    return status;

  state->acc_range_idx = range_idx;
  state->acc_sstvt_adj[0] = sstvt;
  state->acc_sstvt_adj[1] = sstvt;
  state->acc_sstvt_adj[2] = sstvt;

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set dynamic range operation for gyroscope.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  range          Dynamic range settings for LSM6DS3
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_gyr_lsm6ds3_set_range(
    sns_ddf_handle_t dd_handle,
    int8_t range_idx)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status;
  uint8_t write_buffer = 0;
  q16_t sstvt;

  if((range_idx < 0) ||
    (range_idx > (STM_LSM6DS3_GYR_RANGE_NUM -1))) {
    LSM6DS3_MSG_E_1(ERROR, "SET_GYR_RANGE - invalid range_idx - (%d)",range_idx);
    return SNS_DDF_EINVALID_PARAM;
  }

  LSM6DS3_MSG_2(LOW, "SET_GYR_RANGE - set range_idx - (%d) reg_value (%x)",range_idx, lsm6ds3_gyr_ranges[range_idx]);
  //set sensor sensitivity for data conversion
  switch (lsm6ds3_gyr_ranges[range_idx]) {
    case STM_LSM6DS3_GYR_RANGE_245DPS:
      sstvt = STM_LSM6DS3_GYR_SSTVT_245DPS;
      break;
    case STM_LSM6DS3_GYR_RANGE_500DPS:
      sstvt = STM_LSM6DS3_GYR_SSTVT_500DPS;
      break;
    case STM_LSM6DS3_GYR_RANGE_1000DPS:
      sstvt = STM_LSM6DS3_GYR_SSTVT_1000DPS;
      break;
    case STM_LSM6DS3_GYR_RANGE_2000DPS:
      sstvt = STM_LSM6DS3_GYR_SSTVT_2000DPS;
      break;
    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  // Configure CTRL_FS register
  write_buffer = ((uint8_t)lsm6ds3_gyr_ranges[range_idx]);
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_CTRL2_G,
      &write_buffer,
      1,
      STM_LSM6DS3_FS_G_MASK);
  if(status != SNS_DDF_SUCCESS)
    return status;

  state->gyr_range_idx = range_idx;
  state->gyr_sstvt_adj[0] = sstvt + (sstvt*state->gyr_sstvt_comp[0])/100;
  state->gyr_sstvt_adj[1] = sstvt + (sstvt*state->gyr_sstvt_comp[1])/100;
  state->gyr_sstvt_adj[2] = sstvt + (sstvt*state->gyr_sstvt_comp[2])/100;

  return SNS_DDF_SUCCESS;
}






/**
 * @brief Sets a lsm6ds3 sensor attribute to a specific value.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm6ds3_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value)
{
  uint32_t sub_dev_idx;
  sns_ddf_status_e  status = SNS_DDF_SUCCESS;
  sns_ddf_sensor_e ddf_sensor_type;

  if (dd_handle == NULL || value == NULL) {
    LSM6DS3_MSG_E_0(ERROR, "dd_handle is NULL");
    return SNS_DDF_EINVALID_PARAM;
  }

  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sub_dev_idx = (dd_handle_align_t) dd_handle & (~DD_HANDLE_ALIGN_BITS);
  ddf_sensor_type = state->sub_dev[sub_dev_idx].sensors[0]; // '0' --> Primary

  LSM6DS3_MSG_3(LOW, "SET_ATTRIB attrib (%d) sensor (%d) p_sensor (%d)",attrib, sensor, ddf_sensor_type);
  if(attrib != SNS_DDF_ATTRIB_IO_REGISTER) {
    if(LSM6DS3_IS_ESP_SENSOR(ddf_sensor_type)) {
      if(state->esp_info.esp_if)
        return state->esp_info.esp_if->set_attrib(dd_handle, sensor, attrib, value);
      else
        return SNS_DDF_EINVALID_PARAM;
    }
  }

  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
      {
        sns_ddf_powerstate_e* power_state = value;

        if(ddf_sensor_type == SNS_DDF_SENSOR_ACCEL) {
          LSM6DS3_MSG_0(LOW, "SET_POWER - sensor type accel");
          switch(sensor) {
            case SNS_DDF_SENSOR__ALL:
              // TODO: need to include secondary (above) under this
              /* falling through... */
            case SNS_DDF_SENSOR_ACCEL: // primary
              status = sns_dd_acc_lsm6ds3_set_power(dd_handle, *power_state);
              break;
            default:
              status =  SNS_DDF_EINVALID_PARAM;
              break;
          }
        } else if(ddf_sensor_type == SNS_DDF_SENSOR_GYRO) {
          LSM6DS3_MSG_0(LOW, "SET_POWER - sensor type gyro");
          switch(sensor) {
            case SNS_DDF_SENSOR__ALL:
              // TODO: need to include secondary (above) under this
              /* falling through... */
            case SNS_DDF_SENSOR_GYRO: // primary
              status = sns_dd_gyr_lsm6ds3_set_power(dd_handle, *power_state);
              break;
            default:
              status = SNS_DDF_EINVALID_PARAM;
              break;
          }
        }  else {
          /* should never be here... */
          status = SNS_DDF_EINVALID_PARAM;
        }
        if(status == SNS_DDF_SUCCESS)
          state->sub_dev[sub_dev_idx].powerstate = *power_state;
        break;
      }
    case SNS_DDF_ATTRIB_RANGE:
      {
        uint32_t* range_idx = value;

         if(sensor == SNS_DDF_SENSOR_ACCEL)
         status = sns_dd_acc_lsm6ds3_set_range(dd_handle, *range_idx);
         else if(sensor == SNS_DDF_SENSOR_GYRO)
          status = sns_dd_gyr_lsm6ds3_set_range(dd_handle, *range_idx);
         else
          status = SNS_DDF_EINVALID_PARAM;
        break;
      }
    case SNS_DDF_ATTRIB_ODR:
      {
        sns_ddf_odr_t* rate = value;
        LSM6DS3_MSG_3(LOW, "set_odr attrib (%d) sensor (%d) value (%d) ",attrib, sensor, *rate);

        if(sensor == SNS_DDF_SENSOR_ACCEL)
          status = sns_dd_acc_lsm6ds3_attrib_set_datarate(dd_handle, sub_dev_idx, *rate);
        else if(sensor == SNS_DDF_SENSOR_GYRO)
          status =sns_dd_gyr_lsm6ds3_attrib_set_datarate(dd_handle, sub_dev_idx, *rate);
        else if(sensor == SNS_DDF_SENSOR_TEMP)
        {
            if(ddf_sensor_type == SNS_DDF_SENSOR_ACCEL)
              state->temp_cur_rate = (state->temp_cur_rate & 0xF0) | (*rate & 0x0F);
            else if(ddf_sensor_type == SNS_DDF_SENSOR_GYRO)
              state->temp_cur_rate = (state->temp_cur_rate & 0x0F) | ((*rate << 4) & 0xF0);

            sns_dd_lsm6ds3_temp_set_rate(dd_handle, *rate);
        }
        else
          status = SNS_DDF_EINVALID_PARAM;

        break;
      }
    case SNS_DDF_ATTRIB_MOTION_DETECT:
      {
        if(sensor == SNS_DDF_SENSOR_ACCEL) {
          sns_ddf_motion_detect_enable_t* flag = value;
          if(*flag)
          {
            //enable motion detection
            LSM6DS3_MSG_0(HIGH, "+++ enable MD +++ ");
            status = sns_dd_lsm6ds3_enable_md(dd_handle);
          }else{
            //disable motion detection
            LSM6DS3_MSG_0(HIGH, "--- disable MD --- ");
            status = sns_dd_lsm6ds3_disable_md(dd_handle);
          }
        }else
          status = SNS_DDF_EINVALID_PARAM;
        break;
      }
    case SNS_DDF_ATTRIB_FIFO:
      {
        sns_ddf_fifo_set_s* fifo = value;

        if(0 == fifo->watermark) {
          status = sns_dd_lsm6ds3_disable_fifo(dd_handle, sensor);
        }else if(fifo->watermark <= STM_LSM6DS3_MAX_FIFO) {
          //atleast acc should be up and running
          //if (state->acc_desired_rate_idx < 0)
          //  return SNS_DDF_EFAIL;

          status = sns_dd_lsm6ds3_enable_fifo(dd_handle, fifo->watermark, sensor);
        } else {
          status = SNS_DDF_EINVALID_ATTR;
        }

        break;
      }

    case SNS_DDF_ATTRIB_IO_REGISTER:
      {
        sns_ddf_io_register_s* io_reg_in = (sns_ddf_io_register_s *) value;

        if ((uint32_t)(io_reg_in->address_ptr) > 0xFFFF) {
          status =  SNS_DDF_EINVALID_PARAM;
        } else if (io_reg_in->length > 0xFF) {
          status = SNS_DDF_EINVALID_PARAM;
        } else {

          //sns_ddf_status_e status;
          status = sns_dd_lsm6ds3_write_reg(
              dd_handle,
              (uint16_t) ((uint32_t)io_reg_in->address_ptr),
              (uint8_t*) io_reg_in->data,
              (uint8_t) io_reg_in->length,
              0x00);
        }
        break;
      }

    default:
      status = SNS_DDF_EINVALID_ATTR;
  }

  return status;
}

/**
 * @brief Implement enable_sched_data() DDF API.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm6ds3_sched_data(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);

  LSM6DS3_MSG_2(HIGH, "enable_sched_data: sensor=%u, enable=%u", sensor, enable);

  //each called function will handle concurrencies when to enable/disable FIFO int.

  if(LSM6DS3_IS_ESP_SENSOR(sensor)) {
      if(state->esp_info.esp_if)
        state->esp_info.esp_if->enable_sched_data(dd_handle, sensor, enable);
  } else {
    if((sensor == SNS_DDF_SENSOR_GYRO) ||
        (sensor == SNS_DDF_SENSOR_ACCEL) ||
        (sensor == SNS_DDF_SENSOR_TEMP)) {
      status = (enable) ?
        sns_dd_lsm6ds3_enable_fifo_int(dd_handle, sensor) :
        sns_dd_lsm6ds3_disable_fifo_int(dd_handle, sensor);
    }
  }
  return status;
}

/**
* @brief Probes for an LSM6DS3 part.
*
* Refer to sns_ddf_driver_if.h for definition.
*/
sns_ddf_status_e sns_dd_lsm6ds3_probe(
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors )
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  uint8_t i2c_buff;
  uint8_t bytes_read;
  *num_sensors = 0;
  *sensors = NULL;

  status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  // Check WhoAmIRegister to make sure this is the correct driver
  status = sns_ddf_read_port(
      port_handle,
      STM_LSM6DS3_REG_WHO_AM_I,
      &i2c_buff,
      1,
      &bytes_read);
  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  if(i2c_buff != STM_LSM6DS3_WHOAMI_VALUE)
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }

  // WHO_AM_I register is correct. This is probably an LSM6DS3
  *num_sensors = 2;
  *sensors = sns_ddf_memhandler_malloc( memhandler,
                    sizeof(sns_ddf_sensor_e) * *num_sensors );
  if( *sensors != NULL )
  {
    (*sensors)[0] = SNS_DDF_SENSOR_ACCEL;
    (*sensors)[1] = SNS_DDF_SENSOR_GYRO;
    status = SNS_DDF_SUCCESS;
  } else {
    status = SNS_DDF_ENOMEM;
  }

  sns_ddf_close_port( port_handle );
  return status;
}

