/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
 *
 * File Name         : sns_ddf_lsm6ds3_fifo.c
 * Authors           : Karimuddin Sayed
 * Version           : V 4.19.0
 * Date              : 8/24/2016
 * Description       : LSM6DS3 driver source file
 *
 ********************************************************************************
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
 *******************************************************************************/

#include "sns_dd_lsm6ds3.h"
extern sns_ddf_driver_if_s sns_dd_lsm6ds3_if;

extern stm_lsm6ds3_fifo_odr lsm6ds3_fifo_odr_reg_values[STM_LSM6DS3_ODR_NUM];
extern stm_lsm6ds3_acc_odr lsm6ds3_acc_odr_reg_values[STM_LSM6DS3_ODR_NUM];
extern stm_lsm6ds3_gyr_odr lsm6ds3_gyr_odr_reg_values[STM_LSM6DS3_ODR_NUM];
extern stm_lsm6ds3_fifo_odr lsm6ds3_fifo_odr_reg_values[STM_LSM6DS3_ODR_NUM];
extern sns_ddf_odr_t lsm6ds3_odr[STM_LSM6DS3_ODR_NUM];

/**
 * @brief To select appropriate wmk when both sensors are active
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[out] wmk      calculated wmk
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_calculate_wmk(
    sns_ddf_handle_t dd_handle,
    uint16_t* wmk)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint16_t acc_cur_wmk = 0;
  uint16_t gyr_cur_wmk = 0;
  int16_t min_odr_idx = 0;
  int16_t max_odr_idx = 0;
  uint16_t min_odr = 0;
  uint16_t max_odr = 0;
  uint16_t min_odr_wmk = 0;
  uint16_t max_odr_wmk = 0;
  uint16_t cal_wmk = 0;
  if(state->acc_desired_rate_idx >= 0) 
    acc_cur_wmk = state->fifo_info.acc_wmk;
  if(state->gyr_desired_rate_idx >= 0)
    gyr_cur_wmk = state->fifo_info.gyr_wmk;
  if((acc_cur_wmk > 0) && (gyr_cur_wmk > 0)) {
    //both sensors are enabled
    if(state->acc_desired_rate_idx == state->gyr_desired_rate_idx) {
      cal_wmk = MIN(acc_cur_wmk, gyr_cur_wmk);
    } else {
      min_odr_idx = MIN(state->acc_desired_rate_idx,state->gyr_desired_rate_idx);
      max_odr_idx = MAX(state->acc_desired_rate_idx,state->gyr_desired_rate_idx);
      if(min_odr_idx == state->acc_desired_rate_idx) {
        min_odr_wmk = acc_cur_wmk;
        max_odr_wmk = gyr_cur_wmk;
      }
      else {
        min_odr_wmk = gyr_cur_wmk;
        max_odr_wmk = acc_cur_wmk;
      }

      min_odr = lsm6ds3_odr[min_odr_idx];
      max_odr = lsm6ds3_odr[max_odr_idx];
      cal_wmk = ((max_odr*min_odr_wmk) / min_odr);
      if(cal_wmk > max_odr_wmk)
        cal_wmk = max_odr_wmk;
    }
  } else {
    //only one sensor is active
    if(acc_cur_wmk > 0)
      cal_wmk = acc_cur_wmk;
    else if(gyr_cur_wmk > 0)
      cal_wmk = gyr_cur_wmk;
    else
      cal_wmk = 0;
  }
  *wmk = cal_wmk;
  return SNS_DDF_SUCCESS;
}

/**
 * @brief To set LSM6DS3 FIFO wmk
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in] watermark      Watermark level.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_set_fifo_wmk(
    sns_ddf_handle_t dd_handle,
    uint16_t watermark,
    sns_ddf_sensor_e sensor)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status;
  uint16_t wmk_words = 0;
  uint8_t wmkL = 0;
  uint8_t wmkH = 0;

  //convert samples to words
  if((state->acc_desired_rate_idx >= 0) && (state->fifo_info.fifo_enabled & 0x01))
    wmk_words = watermark * 3;
  else if(state->fifo_info.acc_fifo_alive == true)
    wmk_words = watermark * 3;
  if((state->gyr_desired_rate_idx >= 0) && (state->fifo_info.fifo_enabled & 0x02))
    wmk_words += (watermark * 3);

  //wmk_words -= 1; //starts from 0
  // Configure FIFO control registers to enable streaming mode
  wmkL = wmk_words & 0xFF;
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_FIFO_CTRL1,
      &wmkL,
      1,
      0x00);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write_reg failed status=%u", status);
    return SNS_DDF_EBUS;
  }

  wmkH = (wmk_words >> 8) & 0x0F;

  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_FIFO_CTRL2,
      &wmkH,
      1,
      STM_LSM6DS3_FIFO_WTM_CTRL2_MASK);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write reg failed  status=%u", status);
    return status;
  }
  state->fifo_info.cur_wmk = watermark;
  LSM6DS3_DATA_MSG_3(HIGH, "lsm6ds3_set_fifo_wmk: enabling fifo - setting ctrl register - wmk (%d) wmkL (%x) wmkH (%x)", wmk_words, wmkL, wmkH);

  return status;
}

static sns_ddf_status_e sns_dd_lsm6ds3_clear_decimation(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t rw_buffer;
  uint8_t read_count;

  status = sns_ddf_read_port(
      state->port_handle,
      STM_LSM6DS3_REG_FIFO_CTRL3,
      &rw_buffer,
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

  //update decimation register
  if((sensor == SNS_DDF_SENSOR_ACCEL) && (rw_buffer & 0x01)) {
    rw_buffer &= ~0x01; //accel not in FIFO
    sns_dd_lsm6ds3_free(dd_handle, SNS_DDF_SENSOR_ACCEL);
    state->fifo_info.active_sensors = 0;
    if(rw_buffer & 0x08)
     state->fifo_info.active_sensors = 1;
  } else if((sensor == SNS_DDF_SENSOR_GYRO) && (rw_buffer & 0x08)) {
    rw_buffer &= ~0x08; //gyro not in FIFO
    sns_dd_lsm6ds3_free(dd_handle, SNS_DDF_SENSOR_GYRO);
    state->fifo_info.active_sensors = 0;
    if(state->fifo_info.acc_fifo_alive == true) {
      rw_buffer &= ~0x01; //accel not in FIFO
      state->fifo_info.acc_fifo_alive = false;
      sns_dd_lsm6ds3_free(dd_handle, SNS_DDF_SENSOR_ACCEL);
    }
    if(rw_buffer & 0x01)
     state->fifo_info.active_sensors = 1;
  } else
    return status;
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_FIFO_CTRL3,
      &rw_buffer,
      1,
      0xFF);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "write reg failed . status=%u", status);
    return status;
  }

  LSM6DS3_DATA_MSG_2(HIGH, "lsm6ds3_clear_decimation: sensor (%d) active_sensors %d ", sensor, state->fifo_info.active_sensors);
  return status;
}

/**
 * @brief To set decimation register
 *
 * @param[in]  dd_handle    Handle to a driver instance.
 * @param[in]  sensor      sensor 
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_set_decimation(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t rw_buffer;
  uint8_t read_count;
#if 0
  max_odr_idx = MAX(state->acc_desired_rate_idx, state->gyr_desired_rate_idx);
  if(sensor == SNS_DDF_SENSOR_ACCEL) {
    cur_rate_idx = state->acc_desired_rate_idx;
  }
  else if(sensor == SNS_DDF_SENSOR_GYRO) {
    cur_rate_idx = state->gyr_desired_rate_idx;
  }
  dec_factor = lsm6ds3_odr[state->max_odr_idx]/lsm6ds3_odr[cur_rate_idx];
  switch(dec_factor) {
    case 1:
      reg_value = 0x01;
    case 2:
      reg_value = 0x02;
    case 3:
      reg_value = 0x03;
    case 4:
      reg_value = 0x04;
    case 8:
      reg_value = 0x05;
    case 16:
      reg_value = 0x06;
    case 32:
      reg_value = 0x07;
    default:
      reg_value = 0x00;
  }
  if(sensor == SNS_DDF_SENSOR_GYRO) 
    reg_value = (reg_value << 3);
#endif
  status = sns_ddf_read_port(
      state->port_handle,
      STM_LSM6DS3_REG_FIFO_CTRL3,
      &rw_buffer,
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
  if((sensor == SNS_DDF_SENSOR_ACCEL) && (!(rw_buffer & 0x01))) {
    rw_buffer |= 0x01;

    state->fifo_info.active_sensors = 1;
    if(rw_buffer & 0x08)
      state->fifo_info.active_sensors = 2;
  } else if((sensor == SNS_DDF_SENSOR_GYRO) && (!(rw_buffer & 0x08))) {
    rw_buffer |= 0x08;
    state->fifo_info.active_sensors = 1;
    if(rw_buffer & 0x01)
      state->fifo_info.active_sensors = 2;
  } else
    return status;

    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_FIFO_CTRL3,
        &rw_buffer,
        1,
      0xFF);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "write reg failed . status=%u", status);
      return status;
    }
  LSM6DS3_DATA_MSG_2(HIGH, "lsm6ds3_set_decimation: sensor (%d) active_sensors %d ", sensor, state->fifo_info.active_sensors);
  return status;
}
sns_ddf_status_e sns_dd_lsm6ds3_free(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{

  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint32_t sub_dev_idx = 0;

  status = lsm6ds3_search_subdev_by_sensortype(
      state,
      sensor,
      &sub_dev_idx);
  if(status != SNS_DDF_SUCCESS)
    return status;

  if((NULL != state->fifo_info.acc_data.samples) &&
      (sensor == SNS_DDF_SENSOR_ACCEL)) {
    status = sns_dd_lsm6ds3_mfree(state->fifo_info.acc_data.samples,
        state->sub_dev[sub_dev_idx].smgr_handle);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "free fifo_acc_data samples failed. status=%u", status);
      return status;
    }
    state->fifo_info.acc_data.samples = NULL;
  }

  if((NULL != state->fifo_info.temp_data.samples) &&
      (sensor == SNS_DDF_SENSOR_TEMP) &&
      (state->timer == NULL)) {
    status = sns_dd_lsm6ds3_mfree(state->fifo_info.temp_data.samples,
        state->sub_dev[sub_dev_idx].smgr_handle);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "free fifo_temp_data samples failed. status=%u", status);
      return status;
    }
    state->fifo_info.temp_data.samples = NULL;
  }

  //allocate fifo memory only once, as the mode is fifo
  if((NULL != state->fifo_info.gyr_data.samples) &&
      (sensor == SNS_DDF_SENSOR_GYRO)) {
    status = sns_dd_lsm6ds3_mfree(state->fifo_info.gyr_data.samples,
        state->sub_dev[sub_dev_idx].smgr_handle);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "free gyr_fifo_data samples failed. status=%u", status);
      return status;
    }

    state->fifo_info.gyr_data.samples = NULL;
  }

  //malloc memory for state->fifo_raw_buffer
  if((NULL != state->fifo_info.raw_buffer) &&
      (NULL == state->fifo_info.gyr_data.samples) &&
      (NULL == state->fifo_info.acc_data.samples))
  {

    if(sensor == SNS_DDF_SENSOR_GYRO) {
      uint32_t t_sub_dev_idx = sub_dev_idx;
      status = lsm6ds3_search_subdev_by_sensortype(
          state,
          SNS_DDF_SENSOR_ACCEL,
          &sub_dev_idx);
      if(status != SNS_DDF_SUCCESS) {
        // do not return here, only gyro is enabled in config
        LSM6DS3_MSG_E_1(ERROR, "ACC is not available", status);
        sub_dev_idx = t_sub_dev_idx;
      }
    }

    //allocate for accel and gyro
    //if gyro is not enabled we will not use those samples
    status = sns_dd_lsm6ds3_mfree(state->fifo_info.raw_buffer,
        state->sub_dev[sub_dev_idx].smgr_handle);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "free raw_buffer failed. status=%u", status);
      return status;
    }
    state->fifo_info.raw_buffer = NULL;
  }
  return status;
}
sns_ddf_status_e sns_dd_lsm6ds3_alloc(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint32_t sub_dev_idx = 0;

  status = lsm6ds3_search_subdev_by_sensortype(
        state,
        sensor,
        &sub_dev_idx);
    if(status != SNS_DDF_SUCCESS)
      return status;

    if((NULL == state->fifo_info.acc_data.samples) &&
        (sensor == SNS_DDF_SENSOR_ACCEL)) {
      status = sns_dd_lsm6ds3_malloc((void**)&(state->fifo_info.acc_data.samples),
          (STM_LSM6DS3_MAX_FIFO_READ / 6) * STM_LSM6DS3_AXES_PER_SENSOR * sizeof(sns_ddf_sensor_sample_s) ,state->sub_dev[sub_dev_idx].smgr_handle);

      if(status != SNS_DDF_SUCCESS) {
        LSM6DS3_MSG_E_1(ERROR, "malloc fifo_acc_data samples failed. status=%u", status);
        return status;
      }
    }

    if((NULL == state->fifo_info.temp_data.samples) &&
        (state->temp_cur_rate > 0) &&
        (sensor == SNS_DDF_SENSOR_TEMP)) {
      status = sns_dd_lsm6ds3_malloc((void**)&(state->fifo_info.temp_data.samples),
          (STM_LSM6DS3_MAX_TEMP_FIFO_DEPTH) * sizeof(sns_ddf_sensor_sample_s) ,state->sub_dev[sub_dev_idx].smgr_handle);
      if(status != SNS_DDF_SUCCESS) {
        LSM6DS3_MSG_E_1(ERROR, "malloc fifo_temp_data samples failed. status=%u", status);
        return status;
      }
    }

  //allocate fifo memory only once, as the mode is fifo
  if((NULL == state->fifo_info.gyr_data.samples) &&
    (sensor == SNS_DDF_SENSOR_GYRO)) {
    status = sns_dd_lsm6ds3_malloc((void**)&(state->fifo_info.gyr_data.samples),
        (STM_LSM6DS3_MAX_FIFO_READ / 6) * STM_LSM6DS3_AXES_PER_SENSOR * sizeof(sns_ddf_sensor_sample_s) ,state->sub_dev[sub_dev_idx].smgr_handle);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "malloc fifo_gyr_data samples failed. status=%u", status);
      return status;
    }
  }

  //malloc memory for state->fifo_raw_buffer
  if(NULL == state->fifo_info.raw_buffer) {
    //always try to allocate with acc sub dev
    if(sensor == SNS_DDF_SENSOR_GYRO) {
      uint32_t t_sub_dev_idx = sub_dev_idx;
      status = lsm6ds3_search_subdev_by_sensortype(
          state,
          SNS_DDF_SENSOR_ACCEL,
          &sub_dev_idx);
      if(status != SNS_DDF_SUCCESS) {
        // do not return here, only gyro might be working
        LSM6DS3_MSG_E_1(ERROR, "ACC is not available", status);
        sub_dev_idx = t_sub_dev_idx;
      }
    }
    //allocate for accel and gyro
    //if gyro is not enabled we will not use those samples
    status = sns_dd_lsm6ds3_malloc((void**)&(state->fifo_info.raw_buffer),
        STM_LSM6DS3_MAX_FIFO_READ * sizeof(uint8_t) ,state->sub_dev[sub_dev_idx].smgr_handle);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "malloc raw_buffer failed. status=%u", status);
      return status;
    }
  }
  return status;
}
/**
 * @brief To enable LSM6DS3 FIFO.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in] watermark      Watermark level.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_lsm6ds3_enable_fifo(
    sns_ddf_handle_t dd_handle,
    uint16_t watermark,
    sns_ddf_sensor_e sensor)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint16_t new_wmk = 0;
  uint8_t cur_fifo_status = 0;
  int8_t set_odr_idx = -1, esp_rate_idx = -1;
  bool power_down  = false;
  int8_t cur_acc_rate_idx = state->acc_cur_rate_idx;
  int8_t cur_gyr_rate_idx = state->gyr_cur_rate_idx;

  LSM6DS3_MSG_3(HIGH, "lsm6ds3_enable_fifo: get called. sensor (%d) watermark=%u fifo_info.fifo_enabled (%d)",
      sensor, watermark, state->fifo_info.fifo_enabled);
  LSM6DS3_MSG_3(HIGH, "lsm6ds3_enable_fifo ENTER: NOW time (%u) acc_cur_rate_idx (%d) cur_gyr_rate_idx (%d) ",
      sns_ddf_get_timestamp(), cur_acc_rate_idx, cur_gyr_rate_idx);

  status = sns_dd_lsm6ds3_alloc(dd_handle, sensor);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_2(ERROR, "lsm6ds3_enable_fifo: Memory alloc failed status %d sensor %d", status, sensor);
    return status;
  }
  cur_fifo_status = state->fifo_info.fifo_enabled;

  {
    if(cur_fifo_status) {
      //Do not move this code, fifo enabled is setting below
      sns_dd_lsm6ds3_flush_fifo(dd_handle, 0, true);
    }

    state->fifo_info.fifo_enabled |= (sensor == SNS_DDF_SENSOR_ACCEL) ? 0x01 : 0x02;
    //update_fifo_status =  state->fifo_info.fifo_enabled | (sensor == SNS_DDF_SENSOR_ACCEL) ? 0x01 : 0x02;
    // call set rate only after setting fifo_enabled
    if(state->temp_cur_rate)
      sns_dd_lsm6ds3_temp_set_rate(dd_handle, 1);

    if(sensor == SNS_DDF_SENSOR_ACCEL)
      state->fifo_info.acc_wmk = watermark;
    else if(sensor == SNS_DDF_SENSOR_GYRO)
      state->fifo_info.gyr_wmk = watermark;

    sns_dd_lsm6ds3_calculate_wmk(dd_handle, &new_wmk);

    LSM6DS3_DATA_MSG_2(HIGH, "lsm6ds3_enable_fifo: cur_fifo_enabled %d update_fifo_enabled %d", cur_fifo_status, state->fifo_info.fifo_enabled);
    LSM6DS3_DATA_MSG_2(HIGH, "lsm6ds3_enable_fifo: cur_wmk %d new_wmk %d", state->fifo_info.cur_wmk, new_wmk);
    //adding new sensor, changing wmk and odr are not allowed
    if((sensor == SNS_DDF_SENSOR_ACCEL) &&
        (state->fifo_info.acc_fifo_alive == true)) {
      state->fifo_info.acc_fifo_alive = false;
      if(state->fifo_info.cur_wmk != new_wmk) {
        //power_down = true;
        //lets try to set wmk without closing
        status = sns_dd_lsm6ds3_set_fifo_wmk(
            dd_handle,
            new_wmk,
            sensor);
        return status;
      }
      else
        power_down = false;

    } else if((cur_fifo_status == state->fifo_info.fifo_enabled) &&
        (state->fifo_info.cur_wmk != new_wmk)) {
      //fifo status same but wmk is changed

      status = sns_dd_lsm6ds3_set_fifo_wmk(
          dd_handle,
          new_wmk,
          sensor);
      return status;
    } else if(cur_fifo_status != state->fifo_info.fifo_enabled) 
    {
      power_down = true;
    }

    if(power_down == false) {
      LSM6DS3_DATA_MSG_0(HIGH, "lsm6ds3_enable_fifo: FIFO config is not changing .. returning");
      LSM6DS3_DATA_MSG_3(HIGH, "lsm6ds3_enable_fifo: sensor (%d) watermark=%u fifo_info.fifo_enabled (%d)",
          sensor, watermark, state->fifo_info.fifo_enabled);
      return status;
    }
    sns_dd_lsm6ds3_stop_fifo_streaming(dd_handle, cur_fifo_status, false);
    if(cur_acc_rate_idx > STM_LSM6DS3_ODR0)
      state->data_intp.acc_intp_status = true;
    if(cur_gyr_rate_idx > STM_LSM6DS3_ODR0)
      state->data_intp.gyr_intp_status = true;

    sns_dd_lsm6ds3_set_decimation(dd_handle, sensor);
    status = sns_dd_lsm6ds3_set_fifo_wmk(
        dd_handle,
        new_wmk,
        sensor);
    LSM6DS3_DATA_MSG_2(HIGH, "lsm6ds3_enable_fifo: acc_desired_rate_idx %d gyr_desired_rate_idx (%d)", state->acc_desired_rate_idx, state->gyr_desired_rate_idx);
    if((state->acc_desired_rate_idx >= 0) && 
        (state->gyr_desired_rate_idx >= 0))
      set_odr_idx = MAX(state->acc_desired_rate_idx, state->gyr_desired_rate_idx);
    else if(state->acc_desired_rate_idx >= 0)
      set_odr_idx = state->acc_desired_rate_idx;
    else if(state->gyr_desired_rate_idx >= 0)
      set_odr_idx = state->gyr_desired_rate_idx;
    else
      return SNS_DDF_EFAIL;

    if(LSM6DS3_IS_ESP_ACTIVE(state->esp_info)) {
      esp_rate_idx = LSM6DS3_ESP_RATE(state->esp_info);
      if(esp_rate_idx > set_odr_idx)
        set_odr_idx = esp_rate_idx;
    }

    sns_dd_lsm6ds3_start_fifo_streaming(dd_handle, set_odr_idx, 0);
    if(!cur_fifo_status && state->fifo_info.fifo_enabled)
      state->fifo_info.last_timestamp = sns_ddf_get_timestamp();

    if((state->acc_desired_rate_idx >= 0) &&
        (cur_acc_rate_idx >= 0) &&
        (cur_acc_rate_idx != state->acc_cur_rate_idx))
      sns_dd_lsm6ds3_send_event_smgr(dd_handle,
          SNS_DDF_SENSOR_ACCEL,
          SNS_DDF_EVENT_ODR_CHANGED);

    if ((state->gyr_desired_rate_idx >= 0) &&
        (cur_gyr_rate_idx >= 0) &&
        (cur_gyr_rate_idx != state->gyr_cur_rate_idx))
      sns_dd_lsm6ds3_send_event_smgr(dd_handle,
          SNS_DDF_SENSOR_GYRO,
          SNS_DDF_EVENT_ODR_CHANGED);

  }
  LSM6DS3_MSG_3(HIGH, "lsm6ds3_enable_fifo EXIT: watermark=%u fifo_info.fifo_enabled (%d) NOW_time (%u)",
          watermark, state->fifo_info.fifo_enabled, sns_ddf_get_timestamp());
  return SNS_DDF_SUCCESS;
}
/**
 * @brief To disable LSM6DS3 FIFO.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_lsm6ds3_disable_fifo(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status;
  uint16_t new_wmk = 0;
  int8_t update_odr_idx =  0;
  sns_ddf_sensor_e update_sensor = 0;
  int8_t new_rate_idx = -1;
  int8_t cur_acc_rate_idx = state->acc_cur_rate_idx;
  int8_t cur_gyr_rate_idx = state->gyr_cur_rate_idx;
  uint8_t cur_fifo_status = 0;

  LSM6DS3_MSG_2(HIGH, "lsm6ds3_disable_fifo ENTER: NOW time (%u) fifo_rate_idx (%d)",sns_ddf_get_timestamp(), state->fifo_info.fifo_rate_idx);
  LSM6DS3_MSG_3(HIGH, "lsm6ds3_disable_fifo: sensor (%d) fifo_info.fifo_enabled (%d) cur_wmk (%d)",
      sensor, state->fifo_info.fifo_enabled, state->fifo_info.cur_wmk);
  {
    if(((sensor == SNS_DDF_SENSOR_ACCEL) && (!(state->fifo_info.fifo_enabled & 0x01))) ||
        ((sensor == SNS_DDF_SENSOR_GYRO) && (!(state->fifo_info.fifo_enabled & 0x02)))) {
      //if requested sensor is not enabled, return from here
      LSM6DS3_DATA_MSG_2(HIGH, "lsm6ds3_disable_fifo: sensor (%d) fifo not enabled (%d)", sensor, state->fifo_info.fifo_enabled);
      return SNS_DDF_SUCCESS;
    }

    cur_fifo_status = state->fifo_info.fifo_enabled;

    if(sensor == SNS_DDF_SENSOR_ACCEL) {
      if(state->fifo_info.fifo_enabled & 0x02) {
        //gyro alive
        state->fifo_info.acc_fifo_alive = true;
        state->fifo_info.fifo_enabled &= (~0x01);
        sns_dd_lsm6ds3_temp_set_rate(dd_handle, 0);
        return SNS_DDF_SUCCESS;
      }
    }
    state->fifo_info.fifo_enabled &= (sensor == SNS_DDF_SENSOR_ACCEL) ? (~0x01) : (~0x02);
    sns_dd_lsm6ds3_temp_set_rate(dd_handle, 0);

    sns_dd_lsm6ds3_stop_fifo_streaming(dd_handle, cur_fifo_status, true);
    sns_dd_lsm6ds3_clear_decimation(dd_handle, sensor);

    if(state->fifo_info.fifo_enabled ) {
      LSM6DS3_DATA_MSG_0(HIGH, "lsm6ds3_disable_fifo: Another sensor fifo active");

      if(sensor == SNS_DDF_SENSOR_ACCEL) {
        state->data_intp.gyr_intp_status = true;
        new_wmk = state->fifo_info.gyr_wmk;
        update_sensor = SNS_DDF_SENSOR_GYRO;
        update_odr_idx = state->gyr_desired_rate_idx;
      } else if(sensor == SNS_DDF_SENSOR_GYRO) {
        state->data_intp.acc_intp_status = true;
        new_wmk = state->fifo_info.acc_wmk;
        update_sensor = SNS_DDF_SENSOR_ACCEL;
        update_odr_idx = state->acc_desired_rate_idx;
      }
      LSM6DS3_DATA_MSG_2(HIGH, "lsm6ds3_disable_fifo: new_wmk (%d) cur_wmk=%d",new_wmk, state->fifo_info.cur_wmk);
      status = sns_dd_lsm6ds3_set_fifo_wmk(
          dd_handle,
          new_wmk,
          update_sensor);
      sns_dd_lsm6ds3_start_fifo_streaming(dd_handle, update_odr_idx, 0x03);
      state->fifo_info.avg_sample_time = 0;
    } else {
      //clear wmk
      sns_dd_lsm6ds3_clear_wmk(dd_handle);
      //only reset fifo_info.last_timestamp after reset or after fifo disabled
      state->fifo_info.last_timestamp = 0;
      state->fifo_info.avg_sample_time = 0;
      if(sensor == SNS_DDF_SENSOR_ACCEL)
        state->fifo_info.acc_wmk = 0;
      else if(sensor == SNS_DDF_SENSOR_GYRO)
        state->fifo_info.gyr_wmk = 0;
    }
    if(sensor == SNS_DDF_SENSOR_ACCEL) {
      if(state->fifo_info.acc_fifo_alive == false) {
        if((state->acc_desired_rate_idx == -1) &&
            (LSM6DS3_IS_ESP_ACTIVE(state->esp_info))) {
          new_rate_idx = LSM6DS3_ESP_RATE(state->esp_info);
          LSM6DS3_DATA_MSG_3(HIGH, "new_rate(%d) new_rate_idx (%d) esp_en (0x%x) ", 
              (new_rate_idx >= 0) ? lsm6ds3_odr[new_rate_idx]:-1, new_rate_idx, state->esp_info.esp_enabled);
          status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle, new_rate_idx);
        } else {
          status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle,
              state->acc_desired_rate_idx);
        }
        if(SNS_DDF_SUCCESS != status) {
          LSM6DS3_MSG_E_2(ERROR, "setting acc odr - rate_idx (%d) rate (%d)", 0, lsm6ds3_acc_odr_reg_values[0]);
          return status;
        }
      }
      //this case applies only when gyro not in fifo
      //but gyro desired odr > 0
      if((!state->fifo_info.fifo_enabled) && 
          (state->gyr_desired_rate_idx >= STM_LSM6DS3_ODR26)) {
        status = sns_dd_gyr_lsm6ds3_set_datarate(dd_handle,
            state->gyr_desired_rate_idx, false);
      }

    }else if(sensor == SNS_DDF_SENSOR_GYRO) {
      status = sns_dd_gyr_lsm6ds3_set_datarate(dd_handle,
          state->gyr_desired_rate_idx, false);
      if(SNS_DDF_SUCCESS != status) {
        LSM6DS3_MSG_E_2(ERROR, "setting gyr odr - rate_idx (%d) rate (%d)", 0, lsm6ds3_gyr_odr_reg_values[0]);
        return status;
      }
      if(state->acc_desired_rate_idx < STM_LSM6DS3_ODR26) {
        if(LSM6DS3_IS_ESP_ACTIVE(state->esp_info))
          new_rate_idx = LSM6DS3_ESP_RATE(state->esp_info);
        else if(state->md_enabled == true)
          new_rate_idx = STM_LSM6DS3_ODR26;
        LSM6DS3_DATA_MSG_3(HIGH, "new_rate(%d) new_rate_idx (%d) esp_en (0x%x) ", 
            (new_rate_idx >= 0) ? lsm6ds3_odr[new_rate_idx]:-1, new_rate_idx, state->esp_info.esp_enabled);
        status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle, new_rate_idx);

        if(SNS_DDF_SUCCESS != status) {
          LSM6DS3_MSG_E_2(ERROR, "Failed status (%u) odr_idx (%d)", status, new_rate_idx);
          return status;
        }
      } else if(!state->fifo_info.fifo_enabled) {
        //case: fifo disabled
        //acc odr is set, but fifo not enabled
        status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle,
            state->acc_desired_rate_idx);
      }
    }
  }

  if(state->fifo_info.fifo_enabled) {
    if((state->fifo_info.fifo_enabled & 0x01) &&
        (state->acc_desired_rate_idx >= 0) &&
        (cur_acc_rate_idx != state->acc_cur_rate_idx))
      sns_dd_lsm6ds3_send_event_smgr(dd_handle,
          SNS_DDF_SENSOR_ACCEL,
          SNS_DDF_EVENT_ODR_CHANGED);

    if((state->fifo_info.fifo_enabled & 0x02) &&
        (state->gyr_desired_rate_idx >= 0) &&
        (cur_gyr_rate_idx != state->gyr_cur_rate_idx))
      sns_dd_lsm6ds3_send_event_smgr(dd_handle,
          SNS_DDF_SENSOR_GYRO,
          SNS_DDF_EVENT_ODR_CHANGED);
  }
  LSM6DS3_MSG_3(HIGH, "lsm6ds3_disable_fifo EXIT:  fifo_info.fifo_enabled (%d) cur_wmk (%d) Now_time (%u)",
      state->fifo_info.fifo_enabled, state->fifo_info.cur_wmk, sns_ddf_get_timestamp());

  return SNS_DDF_SUCCESS;
}

