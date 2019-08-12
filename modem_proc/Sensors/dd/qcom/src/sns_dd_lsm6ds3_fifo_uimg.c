/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
 *
 * File Name         : sns_ddf_lsm6ds3_fifo_uimg.c
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
#define ROUND(a,b) (((a)/(b)) + ((((a) % (b)) > ((b)/2)) ? 1 : 0))

extern sns_ddf_odr_t lsm6ds3_odr[STM_LSM6DS3_ODR_NUM];
extern stm_lsm6ds3_fifo_odr lsm6ds3_fifo_odr_reg_values[STM_LSM6DS3_ODR_NUM];
/**
 * @brief To enable LSM6DS3 FIFO interrupt.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_lsm6ds3_enable_fifo_int(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t rw_buffer = 0;

  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  LSM6DS3_MSG_2(HIGH, "lsm6ds3_enable_fifo_int ENTER: NOW time (%u) cur_wmk (%d)",sns_ddf_get_timestamp(), state->fifo_info.cur_wmk);
  LSM6DS3_MSG_3(HIGH, "sns_dd_lsm6ds3_enable_fifo_int: sensor (%d) fifo_info.fifo_enabled (%d) int_enabled (%d)",
      sensor, state->fifo_info.fifo_enabled ,state->fifo_info.fifo_int_enabled);

  if(state->fifo_info.fifo_enabled == 0) {
    LSM6DS3_DATA_MSG_0(HIGH, "sns_dd_lsm6ds3_enable_fifo_int: FIFO not enabled - returing wihtout enabling interrupt");
    return SNS_DDF_SUCCESS;
  }
  if(state->fifo_info.cur_wmk > 0) {
    if(state->fifo_info.fifo_int_enabled) {
      if((sensor == SNS_DDF_SENSOR_ACCEL) && (state->fifo_info.fifo_enabled & 0x01))
        state->fifo_info.fifo_int_enabled |= 0x01;
      else if((sensor == SNS_DDF_SENSOR_GYRO) && (state->fifo_info.fifo_enabled & 0x02))
        state->fifo_info.fifo_int_enabled |= 0x02;
    } else {
      if(((sensor == SNS_DDF_SENSOR_ACCEL) && (state->fifo_info.fifo_enabled & 0x01)) ||
          ((sensor == SNS_DDF_SENSOR_GYRO) && (state->fifo_info.fifo_enabled & 0x02))) {
        // Configure lsm6ds3 FIFO control registers
        rw_buffer = 0x0
          | (0<<7)		// INT1 pedometer
          | (0<<6)		// INT1 sign_motion
          | (0<<5)		// INT1 FIFO_FULL flag
          | (1<<4)		// INT1 FIFO_OVR flag
          | (1<<3)		// INT1 FIFO_FTH flag
          | (0<<2)		// INT1 BOOT flag
          | (0<<1)		// INT1 DRDY_G flag
          | 0;				// INT1 DRDY_XL flag
        status = sns_dd_lsm6ds3_write_reg(
            dd_handle,
            STM_LSM6DS3_REG_INT1_CTRL,
            &rw_buffer,
            1,
            STM_LSM6DS3_FIFO_INT_MASK);
        if(status != SNS_DDF_SUCCESS) {
          LSM6DS3_MSG_E_1(ERROR, "write reg failed . status=%u", status);
          return status;
        }
        if((sensor == SNS_DDF_SENSOR_ACCEL) && (state->fifo_info.fifo_enabled & 0x01))
          state->fifo_info.fifo_int_enabled |= 0x01;
        else if((sensor == SNS_DDF_SENSOR_GYRO) && (state->fifo_info.fifo_enabled & 0x02))
          state->fifo_info.fifo_int_enabled |= 0x02;
      }
    }
  } else {
    LSM6DS3_DATA_MSG_0(HIGH, "sns_dd_lsm6ds3_enable_fifo_int: cur_wmk is invalid");
    return SNS_DDF_EINVALID_PARAM;
  }

  LSM6DS3_MSG_3(HIGH, "lsm6ds3_enable_fifo_int EXIT: NOW time (%u) cur_wmk (%d) int_enabled (%d)",
      sns_ddf_get_timestamp(), state->fifo_info.cur_wmk, state->fifo_info.fifo_int_enabled);

  return SNS_DDF_SUCCESS;
}

/**
 * @brief To disable LSM6DS3 FIFO interrupt.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_lsm6ds3_disable_fifo_int(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);

  LSM6DS3_MSG_3(HIGH, "sns_dd_lsm6ds3_disable_fifo_int: sensor (%d) fifo_info.fifo_enabled (%d) fifo_info.fifo_int_enabled (%d) \
      ", sensor, state->fifo_info.fifo_enabled, state->fifo_info.fifo_int_enabled);
  LSM6DS3_MSG_2(HIGH, "lsm6ds3_disable_fifo_int ENTER: NOW time (%u) cur_wmk (%d)",sns_ddf_get_timestamp(), state->fifo_info.cur_wmk);

  if(!state->fifo_info.fifo_int_enabled) {
    LSM6DS3_DATA_MSG_0(HIGH, "sns_dd_lsm6ds3_disable_fifo_int: fifo int not enabled returning");
    return SNS_DDF_SUCCESS;
  }
  state->fifo_info.fifo_int_enabled &= (sensor == SNS_DDF_SENSOR_ACCEL) ? (~0x01) : (~0x02);
  if(state->fifo_info.cur_wmk > 0) {
    if(((sensor == SNS_DDF_SENSOR_ACCEL) && (state->fifo_info.fifo_enabled & 0x02)) ||
        ((sensor == SNS_DDF_SENSOR_GYRO) && (state->fifo_info.fifo_enabled & 0x01))) {
      LSM6DS3_DATA_MSG_3(HIGH, "Disable_fifo_int: Another sensor fifo active sensor (0x%x) fifo_enabled (0x%x) fifo_int_enabled",
          sensor, state->fifo_info.fifo_enabled,state->fifo_info.fifo_int_enabled);
      return SNS_DDF_SUCCESS;
    }
    LSM6DS3_DATA_MSG_0(HIGH, "sns_dd_lsm6ds3_disable_fifo_int: Disabling FIFO interrupt");
    // Configure FIFO control registers
    rw_buffer = 0x0
      | (0<<7)					 // INT1 pedometer
      | (0<<6)					 // INT1 sign_motion
      | (0<<5)					 // INT1 FIFO_FULL flag
      | (0<<4)					 // INT1 FIFO_OVR flag
      | (0<<3)					 // INT1 FIFO_FTH flag
      | (0<<2)					 // INT1 BOOT flag
      | (0<<1)					 // INT1 DRDY_G flag
      | 0;					     // INT1 DRDY_XL flag
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_INT1_CTRL,
        &rw_buffer,
        1,
        STM_LSM6DS3_FIFO_INT_MASK);

    if(status != SNS_DDF_SUCCESS)
      return status;
  } else {
    LSM6DS3_DATA_MSG_0(HIGH, "sns_dd_lsm6ds3_disable_fifo_int: cur_wmk is invalid ");
    return SNS_DDF_EINVALID_PARAM;
  }

  LSM6DS3_MSG_3(HIGH, "lsm6ds3_disable_fifo_int EXIT: sensor (%d) NOW time (%u) fifo_int (%d)",sensor, sns_ddf_get_timestamp(), state->fifo_info.fifo_int_enabled);
  return SNS_DDF_SUCCESS;
}
void sns_dd_lsm6ds3_clear_wmk(sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint8_t rw_buffer = 0;
  sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_FIFO_CTRL1,
      &rw_buffer,
      1,
      0xFF);
  sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_FIFO_CTRL2,
      &rw_buffer,
      1,
      STM_LSM6DS3_FIFO_WTM_CTRL2_MASK);
  state->fifo_info.cur_wmk = 0;
}

sns_ddf_status_e sns_dd_lsm6ds3_stop_fifo_streaming(
    sns_ddf_handle_t  dd_handle,
    uint8_t fifo_status,
    bool flush)
{
  //flags 
  //0x1 acc restarted as gyro is starting first time
  //0x2 gyr restarted as acc is starting first time
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint8_t rw_buffer = 0x00;

  if(flush == true)
    sns_dd_lsm6ds3_flush_fifo(dd_handle, 0, true);

  if(fifo_status) {
    sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_FIFO_CTRL5,
        &rw_buffer,
        1,
        0xFF);
    state->fifo_info.fifo_rate_idx = -1;
  }

  if (state->acc_cur_rate_idx > STM_LSM6DS3_ODR0) {
    sns_dd_acc_lsm6ds3_set_datarate(dd_handle,
        -1);
  }

  if(state->gyr_cur_rate_idx > STM_LSM6DS3_ODR0) {
    sns_dd_gyr_lsm6ds3_set_datarate(dd_handle,
        -1, true);
  }
  return SNS_DDF_SUCCESS;
}
sns_ddf_status_e sns_dd_lsm6ds3_start_fifo_streaming(
    sns_ddf_handle_t  dd_handle,
    int16_t rate_idx,
    int16_t flags)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint8_t rw_buffer = 0x00;
  int8_t new_rate_idx = -1;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  //TODO - If gyro is enabled, run acc with same odr

  //start streaming,stream mode
  rw_buffer = 0x06 | lsm6ds3_fifo_odr_reg_values[rate_idx] ;
  sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_FIFO_CTRL5,
      &rw_buffer,
      1,
      0xFF);

  state->fifo_info.fifo_rate_idx = rate_idx;
  if ((state->acc_desired_rate_idx >= 0) ||
      (state->fifo_info.acc_fifo_alive == true)) {
    status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle,
        rate_idx);
    if(SNS_DDF_SUCCESS != status) {
      LSM6DS3_MSG_E_2(ERROR, "Failed status (%u) odr_idx (%d)", status, rate_idx);
      return status;
    }
  } else {
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
  }
  if (state->gyr_desired_rate_idx >= 0) {
    status = sns_dd_gyr_lsm6ds3_set_datarate(dd_handle,
        rate_idx, false);

    if(SNS_DDF_SUCCESS != status) {
      LSM6DS3_MSG_E_2(ERROR, "Failed status (%u) odr_idx (%d)", status, rate_idx);
      return status;
    }
  }
  return SNS_DDF_SUCCESS;
}

static void sns_dd_backup_valid_sample(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e sensor)
{

  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint16_t idx = 0;
  if((sensor == SNS_DDF_SENSOR_ACCEL) && 
  (state->fifo_info.acc_data.status == SNS_DDF_SUCCESS)){
    //backup last sample
    if((state->fifo_info.acc_sample_cnt) &&
        (state->fifo_info.acc_sample_cnt < (STM_LSM6DS3_MAX_FIFO_READ/12))) {
      state->data_intp.acc_last_sample_status = true;
      idx = state->fifo_info.acc_sample_cnt - 1;
      state->data_intp.acc_last_sample[0] = state->fifo_info.acc_data.samples[0 + idx*3].sample;
      state->data_intp.acc_last_sample[1] = state->fifo_info.acc_data.samples[1 + idx*3].sample;
      state->data_intp.acc_last_sample[2] = state->fifo_info.acc_data.samples[2 + idx*3].sample;
    }
  }  else if((sensor == SNS_DDF_SENSOR_GYRO) && 
  (state->fifo_info.gyr_data.status == SNS_DDF_SUCCESS)){
    if((state->fifo_info.gyr_sample_cnt) &&
        (state->fifo_info.gyr_sample_cnt < (STM_LSM6DS3_MAX_FIFO_READ/12))) {
      state->data_intp.gyr_last_sample_status = true;
      idx = state->fifo_info.gyr_sample_cnt - 1;
      state->data_intp.gyr_last_sample[0] = state->fifo_info.gyr_data.samples[0 + idx*3].sample;
      state->data_intp.gyr_last_sample[1] = state->fifo_info.gyr_data.samples[1 + idx*3].sample;
      state->data_intp.gyr_last_sample[2] = state->fifo_info.gyr_data.samples[2 + idx*3].sample;
    }
  }

}
static sns_ddf_status_e sns_dd_lsm6ds3_intp(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_sensor_sample_s* data_ptr,
    uint16_t valid_samples,
    uint16_t invalid_samples)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  q16_t* last_sample = NULL;
  q16_t valid_first_sample[3] = {0};
  int i = 0;
  bool last_sample_status = false;

  if(sensor == SNS_DDF_SENSOR_ACCEL) {
    last_sample = state->data_intp.acc_last_sample;
    last_sample_status = state->data_intp.acc_last_sample_status;
  }
  else {
    last_sample = state->data_intp.gyr_last_sample;
    last_sample_status = state->data_intp.gyr_last_sample_status;
  }
  if(last_sample_status == false)
    return SNS_DDF_EINVALID_PARAM;

  if(valid_samples) {
    valid_first_sample[0] = (data_ptr+(invalid_samples*3 + 0))->sample;
    valid_first_sample[1] = (data_ptr+(invalid_samples*3 + 1))->sample;
    valid_first_sample[2] = (data_ptr+(invalid_samples*3 + 2))->sample;

    for(i=0; i<invalid_samples; i++) {
      (data_ptr+(0 + i*3))->sample = FX_DIV_Q16((valid_first_sample[0] - last_sample[0]), FX_CONV_Q16((int32_t)(invalid_samples+1), 0)) + last_sample[0];
      (data_ptr+(1 + i*3))->sample = FX_DIV_Q16((valid_first_sample[1] - last_sample[1]), FX_CONV_Q16((int32_t)(invalid_samples+1), 0)) + last_sample[1];
      (data_ptr+(2 + i*3))->sample = FX_DIV_Q16((valid_first_sample[2] - last_sample[2]), FX_CONV_Q16((int32_t)(invalid_samples+1), 0)) + last_sample[2];
    }
  } else {
    for(i=0;i<invalid_samples;i++) {
      (data_ptr+(0 + i*3))->sample = last_sample[0];
      (data_ptr+(1 + i*3))->sample = last_sample[1];
      (data_ptr+(2 + i*3))->sample = last_sample[2];
    }
  }
  return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e sns_dd_lsm6ds3_send_data_smgr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_sensor_data_s* data,
    uint32_t len)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint32_t sub_dev_inx = 0;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t t_fifo_status = 0, t_idx=0;
  sns_ddf_sensor_e p_sensor = 0;
  uint8_t i = 0;

  if(sensor == SNS_DDF_SENSOR_TEMP) {
    t_fifo_status = (state->fifo_info.fifo_enabled & 0x0C) >> 2; //moving bits to first
    while(t_fifo_status) {
      if(i == 0) {
        p_sensor = SNS_DDF_SENSOR_ACCEL;
        t_idx = 0x0F;
      } else {
        p_sensor = SNS_DDF_SENSOR_GYRO;
        t_idx = 0xF0;
      }
      LSM6DS3_DATA_MSG_3(HIGH, "sns_dd_temp_fifo_data:fifo_enabled (0x%x) temp rate (%d) p_sensor (%d)", 
          state->fifo_info.fifo_enabled, (state->temp_cur_rate & t_idx), p_sensor);

      if(state->temp_cur_rate & t_idx) {
        status = lsm6ds3_search_subdev_by_sensortype(
            state,
            p_sensor,
            &sub_dev_inx);
        status = sns_ddf_smgr_notify_data(
            state->sub_dev[sub_dev_inx].smgr_handle,
            data,
            len);
        if(status != SNS_DDF_SUCCESS)
          return status;
#if STM_LSM6DS3_LOGDATA
        sns_dd_lsm6ds3_log_data(data, true);
#endif
      }
      t_fifo_status >>= 1;
      i++;
    }
  } else {
    status = lsm6ds3_search_subdev_by_sensortype(
        state,
        sensor,
        &sub_dev_inx);
    if(status != SNS_DDF_SUCCESS)
      return status;
    status = sns_ddf_smgr_notify_data(
        state->sub_dev[sub_dev_inx].smgr_handle,
        data,
        len);
    if(status != SNS_DDF_SUCCESS)
      return status;

#if STM_LSM6DS3_LOGDATA
    sns_dd_lsm6ds3_log_data(data, true);
#endif

  }
  return status;
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
static sns_ddf_status_e sns_dd_send_fifo_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_time_t    start_time,
    sns_ddf_time_t    end_time)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  stm_lsm6ds3_fifo_info_s* fifo_info = &state->fifo_info;
  sns_ddf_sensor_data_s fifo_data;
  uint16_t num_samples = 0, total_samples = 0;
  uint16_t num_invalid_samples;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_ddf_time_t    last_timestamp = 0, odr_settled_ts = 0;
  sns_ddf_sensor_data_s* sensor_data = NULL;
  int8_t cur_rate_idx = -1;
  bool* intp_status;

  LSM6DS3_DATA_MSG_3(HIGH,
      "send_fifo_data: sensor (%d) start_time (%u) end_time (%u)", sensor, start_time, end_time);

  fifo_data.sensor = sensor;
  fifo_data.status = SNS_DDF_SUCCESS;
  fifo_data.end_timestamp = end_time;
  fifo_data.timestamp = start_time;

  if(sensor == SNS_DDF_SENSOR_TEMP) {
    fifo_data.samples = fifo_info->temp_data.samples;
    fifo_data.num_samples = 1 * fifo_info->temp_sample_cnt;

    status = sns_dd_lsm6ds3_send_data_smgr(
        dd_handle,
        sensor,
        &fifo_data,
        1);
    return status;

  }

  if(sensor == SNS_DDF_SENSOR_ACCEL) {
    num_samples = total_samples = fifo_info->acc_sample_cnt;
    sensor_data = &fifo_info->acc_data;
    odr_settled_ts = state->acc_odr_settled_ts;
    intp_status = &state->data_intp.acc_intp_status;
    cur_rate_idx = state->acc_cur_rate_idx;
  } else {
    num_samples = total_samples = fifo_info->gyr_sample_cnt;
    sensor_data = &fifo_info->gyr_data;
    odr_settled_ts = state->gyr_odr_settled_ts;
    intp_status = &state->data_intp.gyr_intp_status;
    cur_rate_idx = state->gyr_cur_rate_idx;
  }

  if(cur_rate_idx < 0) {
    LSM6DS3_MSG_E_1(ERROR, "sns_dd_send_fifo_data:invalid rate_idx (%d) ", cur_rate_idx);
    return SNS_DDF_EINVALID_PARAM;
  }

  fifo_data.samples = sensor_data->samples;

  LSM6DS3_DATA_MSG_3(HIGH, "send_fifo_data: start_timestamp=%u, end_timestamp=%u odr_settled_ts=%u",
      fifo_data.timestamp, fifo_data.end_timestamp, odr_settled_ts);

  if ( (int64_t)(odr_settled_ts - fifo_data.timestamp) <= 0 )
  {
    /* all samples in FIFO produced after ODR change settled */
    num_invalid_samples = 0;
  }
  else if ( (int64_t)(odr_settled_ts - fifo_data.end_timestamp) > 0 )
  {
    /* all samples in FIFO produced before ODR change settled */
    num_invalid_samples = num_samples;
  }
  else /* some samples in FIFO produced before ODR change settled */
  {
    //fifo_info.last_timestamp could be zero or not reliable. Use input parameters of this function.
    uint64_t time_interval = (end_time - start_time)/(num_samples-1);
    uint32_t st_odr = sns_ddf_convert_usec_to_tick ( 1000000 / lsm6ds3_odr[cur_rate_idx]);
    num_invalid_samples =
      ((odr_settled_ts - fifo_data.timestamp) / st_odr) + 1;
    if(num_invalid_samples > num_samples)
      num_invalid_samples = num_samples;
    fifo_data.end_timestamp =
      start_time + (time_interval * (num_invalid_samples-1));
    //set temporary fifo_info.last_timestamp within this function.
    last_timestamp = fifo_data.end_timestamp;
  }

  if ( num_invalid_samples > 0 )
  {
    LSM6DS3_MSG_3(HIGH,
        "send_fifo_data: #T_samples=%d #invalid=%d intp_status %d", num_samples, num_invalid_samples, *intp_status);
    fifo_data.status      = SNS_DDF_EINVALID_DATA;
    fifo_data.num_samples = STM_LSM6DS3_AXES_PER_SENSOR * num_invalid_samples;
    if(*intp_status == true) {
        status = sns_dd_lsm6ds3_intp(dd_handle, sensor, sensor_data->samples, (num_samples-num_invalid_samples), num_invalid_samples);
        if(status == SNS_DDF_SUCCESS) {

          sns_ddf_time_t est_st_odr = (sns_ddf_convert_usec_to_tick(1000000 / lsm6ds3_odr[cur_rate_idx]));
          uint8_t i = 0;
          if((int64_t)(start_time - state->fifo_info.last_timestamp) > (1.8 * est_st_odr)) {
            fifo_data.timestamp -= est_st_odr;
            fifo_data.num_samples += STM_LSM6DS3_AXES_PER_SENSOR;
            for(i=0;i<STM_LSM6DS3_AXES_PER_SENSOR;i++)
              sensor_data->samples[i + num_samples * STM_LSM6DS3_AXES_PER_SENSOR] = 
                sensor_data->samples[i + (num_samples -1) * STM_LSM6DS3_AXES_PER_SENSOR];
          }

          fifo_data.status  = SNS_DDF_SUCCESS;
          fifo_data.end_timestamp = end_time;
          if(num_samples - num_invalid_samples) {
            fifo_data.num_samples += STM_LSM6DS3_AXES_PER_SENSOR * (num_samples-num_invalid_samples);
          }
          num_samples = 0;
        } else {
          *intp_status = false;
          /* prepares to send remaining samples */
          num_samples -= num_invalid_samples;
        }
    } else {
        /* prepares to send remaining samples */
        num_samples -= num_invalid_samples;
    }
    status = sns_dd_lsm6ds3_send_data_smgr(
        dd_handle,
        sensor,
        &fifo_data,
        1);
    if(status != SNS_DDF_SUCCESS)
      return status;

    LSM6DS3_MSG_3(HIGH,
        "send_fifo_data: #sensor=%d #samples=%d #invalid=%d",
        sensor, total_samples, num_invalid_samples);
    LSM6DS3_MSG_3(HIGH,
        "send_fifo_data: start_ts=%u end_ts=%u status=%u",
        fifo_data.timestamp, fifo_data.end_timestamp, fifo_data.status);

    if ( num_samples > 0 )
    {
      fifo_data.end_timestamp = end_time;
      fifo_data.timestamp     = last_timestamp +
        (end_time - last_timestamp)/num_samples;
      fifo_data.samples = &sensor_data->samples[num_invalid_samples * STM_LSM6DS3_AXES_PER_SENSOR];
      LSM6DS3_MSG_3(HIGH,
          "send_fifo_data: #valid=%d ts=%u end_ts=%u",
          num_samples, fifo_data.timestamp, fifo_data.end_timestamp);
    }
  }
  if ( num_samples > 0 )
  {
    *intp_status = false;
    fifo_data.status = SNS_DDF_SUCCESS;
    fifo_data.num_samples = STM_LSM6DS3_AXES_PER_SENSOR * num_samples;
    status = sns_dd_lsm6ds3_send_data_smgr(
        dd_handle,
        sensor,
        &fifo_data,
        1);
    if(status != SNS_DDF_SUCCESS)
      return status;
    LSM6DS3_DATA_MSG_3(HIGH,
        "send_fifo_data: #valid=%d ts=%u end_ts=%u",
        num_samples, fifo_data.timestamp, fifo_data.end_timestamp);
    sensor_data->status = fifo_data.status;
    sns_dd_backup_valid_sample(dd_handle, sensor);
  }
  return SNS_DDF_SUCCESS;
}

static void sns_dd_fill_fifo_data(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e sensor,
    uint8_t* raw_buf_ptr)
{

  uint16_t idx = 0;
  uint16_t i= 0;
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  q16_t sae_d[STM_LSM6DS3_AXES_PER_SENSOR];
  sns_ddf_sensor_sample_s* data_samples = NULL;

  sns_dd_lsm6ds3_convert_raw_data(dd_handle, sensor, raw_buf_ptr, sae_d);
  sns_ddf_map_axes(&state->axes_map, sae_d);

  if(sensor == SNS_DDF_SENSOR_ACCEL) {
    data_samples = state->fifo_info.acc_data.samples;
    idx = state->fifo_info.acc_sample_cnt;
    state->fifo_info.acc_sample_cnt++;
  } else if(sensor == SNS_DDF_SENSOR_GYRO) {
    data_samples = state->fifo_info.gyr_data.samples;
    idx = state->fifo_info.gyr_sample_cnt;
    state->fifo_info.gyr_sample_cnt++;
  } else if(sensor == SNS_DDF_SENSOR_TEMP) {
    data_samples = state->fifo_info.temp_data.samples;
    idx = state->fifo_info.temp_sample_cnt;
    state->fifo_info.temp_sample_cnt++;
  }
  if(data_samples != NULL) {
    for(i=0; i<STM_LSM6DS3_AXES_PER_SENSOR; i++) {
      data_samples[i+idx*3].sample = sae_d[i];
    }
  }

}

static int32_t sns_dd_read_and_split(
    sns_ddf_handle_t  dd_handle, 
    uint16_t* pattern_pos,
    uint16_t fifo_count,
    uint8_t* ex_buf,
    uint16_t ex_count)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint16_t buffer_pos = 0;
  uint16_t p_pos = 0;
  uint8_t read_count = 0;
  uint8_t offset = 0;
  uint16_t bytes_to_read = fifo_count;
  uint16_t idx = 0;
  uint16_t buf_idx = 0;

  LSM6DS3_DATA_MSG_3(HIGH, "fifo_count %d pattern_pos %d ex_count %d",fifo_count, *pattern_pos, ex_count);

  if(fifo_count == 0)
    return 0;
  if(state->fifo_info.raw_buffer == NULL) {
    LSM6DS3_MSG_E_0(ERROR, "send_fifo_data: fifo buffer NULL");
    return -1;
  }

  //read pattern pos before reading fifo data
  uint16_t pattern_sample = *pattern_pos / STM_LSM6DS3_AXES_PER_SENSOR; //converting word pos to sample pos

  //reset the parameters
  state->fifo_info.acc_sample_cnt = 0;
  state->fifo_info.gyr_sample_cnt = 0;
  //reset index
  idx = 0;
  //calculate offset
  switch (*pattern_pos % STM_LSM6DS3_AXES_PER_SENSOR) {

    LSM6DS3_DATA_MSG_2(HIGH, "fifo_info.ex_pos %d fifo_info.ex_count %d",state->fifo_info.ex_pos, state->fifo_info.ex_count);
    //data not at sample boundary
    case 1:
    if((state->fifo_info.ex_count) &&
        (*pattern_pos == (state->fifo_info.ex_pos+1))) {
      state->fifo_info.raw_buffer[0] = state->fifo_info.ex_buf[0];
      state->fifo_info.raw_buffer[1] = state->fifo_info.ex_buf[1];
      offset = 0;
      buf_idx = 2;
      state->fifo_info.ex_count = 0;
    } else {
      if(state->fifo_info.active_sensors > 1) {
        offset = 4 + (1 - pattern_sample) * STM_LSM6DS3_BYTES_PER_SAMPLE;
      }
      else
        offset = 4 ;
    }
    break;
    case 2:
    if(state->fifo_info.active_sensors > 1)
      offset = 2 + (1 - pattern_sample) * STM_LSM6DS3_BYTES_PER_SAMPLE;
    else
      offset = 2 ;
    break;
    default:
    offset = 0;
    break;
  }

  if(offset > 0) {

    LSM6DS3_DATA_MSG_3(HIGH, "pattern_pos %d pattern_sample %d fifo_count %d",*pattern_pos, pattern_sample, fifo_count);
    bytes_to_read = ALIGN_UP((fifo_count - offset ), (STM_LSM6DS3_BYTES_PER_SAMPLE * state->fifo_info.active_sensors)) + offset;
    pattern_sample = 0;
    LSM6DS3_DATA_MSG_2(HIGH, "offset %d bytes_to_read %d", offset, bytes_to_read);
  }
  if((!(bytes_to_read / (STM_LSM6DS3_BYTES_PER_SAMPLE * state->fifo_info.active_sensors))) && (offset > 0)) {
    LSM6DS3_MSG_E_0(HIGH, "ERROR*** not enough bytes in fifo to make a sample");
    sns_ddf_read_port(
        state->port_handle,
        STM_LSM6DS3_REG_FIFO_DATA_OUT_L,
        state->fifo_info.raw_buffer,
        (offset-ex_count),
        &read_count);
    *pattern_pos = pattern_sample * STM_LSM6DS3_AXES_PER_SENSOR;
    return offset;
  }

  for(idx = 0; idx < ex_count; idx++, buf_idx++)
    state->fifo_info.raw_buffer[buf_idx] = ex_buf[idx];

  bytes_to_read -= ex_count;


  bytes_to_read = ALIGN_UP((bytes_to_read + buf_idx), (STM_LSM6DS3_BYTES_PER_SAMPLE * state->fifo_info.active_sensors)) + offset;
  if(bytes_to_read == 0) {
    LSM6DS3_MSG_1(HIGH, "handle_irq: bytes_to_read (%d) not even one sample", bytes_to_read);
    return bytes_to_read;
  }

  if(bytes_to_read > STM_LSM6DS3_MAX_FIFO_READ)
    bytes_to_read = STM_LSM6DS3_MAX_FIFO_READ;

  bytes_to_read -= buf_idx;

  LSM6DS3_DATA_MSG_2(HIGH, "sns_dd_parse_and_split: buff_idx %d bytes_to_read %d",buf_idx, bytes_to_read);
  status = sns_ddf_read_port(
      state->port_handle,
      STM_LSM6DS3_REG_FIFO_DATA_OUT_L,
      &state->fifo_info.raw_buffer[buf_idx],
      bytes_to_read,
      &read_count);

  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_1(ERROR, "read_port fails status 0x%x ",status);
    return -1;
  }
  if(read_count != bytes_to_read)
  {
    LSM6DS3_MSG_E_2(ERROR, "send_fifo_data: raw data reading fails, num_bytes_to_read=%u, actual_read_bytes=%u",
        bytes_to_read, read_count);
    return -1;
  }

  uint8_t* raw_buf_ptr = state->fifo_info.raw_buffer;

  LSM6DS3_DATA_MSG_3(HIGH, "sns_dd_parse_and_split read bytes %d pattern_pos %d active_sensors %d",read_count, *pattern_pos, state->fifo_info.active_sensors);
  if(state->fifo_info.active_sensors > 1) {
    p_pos = pattern_sample;
    for(buffer_pos = 0; buffer_pos < ((read_count+buf_idx-offset)/6); buffer_pos++, p_pos++)
    {
      if(p_pos == state->fifo_info.pattern_samples_cnt)
        p_pos = 0;
#if PACKET_DBG
      LSM6DS3_DATA_MSG_2(HIGH, "sns_dd_parse_and_split: p_pos %d buffer_pos %d",p_pos, buffer_pos);
#endif
      //convert the raw data in read_buffer to X/Y/Z axis sensor data
      if(state->fifo_info.pattern_array[p_pos] == 0x01) {
        sns_dd_fill_fifo_data(dd_handle, SNS_DDF_SENSOR_ACCEL, &raw_buf_ptr[offset + buffer_pos*6]);
      } else if(state->fifo_info.pattern_array[p_pos] == 0x02) {
        sns_dd_fill_fifo_data(dd_handle, SNS_DDF_SENSOR_GYRO, &raw_buf_ptr[offset + buffer_pos*6]);
      }
    }
  } else if(state->fifo_info.active_sensors == 1) {
    if(state->fifo_info.fifo_enabled & 0x01) {
      for(buffer_pos = 0; buffer_pos < ((read_count+buf_idx-offset)/6); buffer_pos++ ) {
        sns_dd_fill_fifo_data(dd_handle, SNS_DDF_SENSOR_ACCEL, &raw_buf_ptr[offset + buffer_pos*6]);
      }
    } else if(state->fifo_info.fifo_enabled & 0x02) {
      for(buffer_pos = 0; buffer_pos < ((read_count+buf_idx-offset)/6); buffer_pos++ ) {
        sns_dd_fill_fifo_data(dd_handle, SNS_DDF_SENSOR_GYRO, &raw_buf_ptr[offset + buffer_pos*6]);
      }
    }
  }
  *pattern_pos = 0;
  return (read_count+ex_count);
}
static void sns_dd_lsm6ds3_timestamp_synthesis(
    sns_ddf_handle_t dd_handle,
    uint32_t total_sps,
    sns_ddf_time_t*  c_sample_time,
    sns_ddf_time_t use_time,
    bool flush)

{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_time_t cal_end=0, cal_st = 0;

  sns_ddf_time_t est_st_odr = (sns_ddf_convert_usec_to_tick(1000000 / lsm6ds3_odr[state->fifo_info.fifo_rate_idx])) ;
  sns_ddf_time_t est_st_s = ROUND((use_time - state->fifo_info.last_timestamp), total_sps);
  int64_t drift_time = 0;

  //if((state->fifo_info.first_pass == true) &&
  if((flush == true) ||
       (total_sps > state->fifo_info.cur_wmk))
  {
    if(state->fifo_info.avg_sample_time == 0) {
      cal_st = est_st_odr;
      //cal_st = MIN(est_st_s, est_st_odr);
    } else {
      cal_st = state->fifo_info.avg_sample_time;
    }
    cal_end = state->fifo_info.last_timestamp + cal_st * total_sps;
    if(cal_end > use_time)
      cal_st = est_st_s;
    else {
      drift_time = (int64_t) (use_time - cal_end);
      if(drift_time > (0.9 * cal_st)) {
        // if drift time more than 70% 
        //still consider avg_st is accurate, so add 50% of diff
        cal_st += ROUND((uint64_t) (drift_time - (cal_st * 0.9)), total_sps);
      }
    }
    LSM6DS3_MSG_3(HIGH, "time_synthesis: avg_st (%d) use_time (%u) cal_end (%u) ", state->fifo_info.avg_sample_time, 
        use_time, cal_end);
    LSM6DS3_MSG_3(HIGH, "time_synthesis: c_st (%d) e_st_s (%d) est_st_odr (%d)",cal_st, est_st_s, est_st_odr);

  } else if((flush != true) &&
      (total_sps <= state->fifo_info.cur_wmk)) {

    if((use_time > state->fifo_info.last_timestamp)&&
        (0 != state->fifo_info.last_timestamp))
    {
      //use state->fifo_info.last_timestamp as reliable source
      cal_st = est_st_s;
      LSM6DS3_DATA_MSG_2(HIGH, "est_st_odr (%u) cal_st (%u)", est_st_odr, cal_st);
    }
    else
    {
      //applies if timestamp < fifo_info.last_timestamp or last_timestmap is zero
      //use timestamp as reliable source.
      cal_st = est_st_odr;
      LSM6DS3_DATA_MSG_2(HIGH, "est_st_odr (%u) cal_st (%u)", est_st_odr, cal_st);
    }
  }
  *c_sample_time = cal_st;
  LSM6DS3_DATA_MSG_2(HIGH, "Finished synthesis - flush %d cal_st (%u) ", flush, *c_sample_time);
}

static void sns_dd_lsm6ds3_fifo_reset(sns_ddf_handle_t  dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  int8_t cur_rate_idx = state->fifo_info.fifo_rate_idx;
  int8_t fifo_int = state->fifo_info.fifo_int_enabled;

  if(fifo_int & 0x01)
    sns_dd_lsm6ds3_disable_fifo_int(dd_handle, SNS_DDF_SENSOR_ACCEL);
  if(fifo_int & 0x02)
    sns_dd_lsm6ds3_disable_fifo_int(dd_handle, SNS_DDF_SENSOR_GYRO);

  sns_dd_lsm6ds3_stop_fifo_streaming(dd_handle, state->fifo_info.fifo_enabled, false);
  sns_dd_lsm6ds3_start_fifo_streaming(dd_handle, cur_rate_idx, 0x00);
  state->fifo_info.last_timestamp = sns_ddf_get_timestamp();
  if(state->acc_cur_rate_idx >= 0)
    state->data_intp.acc_intp_status = true;
  if(state->gyr_cur_rate_idx >= 0)
    state->data_intp.gyr_intp_status = true;
  if(fifo_int & 0x01)
    sns_dd_lsm6ds3_enable_fifo_int(dd_handle, SNS_DDF_SENSOR_ACCEL);
  if(fifo_int & 0x02)
    sns_dd_lsm6ds3_enable_fifo_int(dd_handle, SNS_DDF_SENSOR_GYRO);
}



//MAX delay between interrupt generated to interrupt handler called
#define ADSP_MAX_INTERRUPT_DELAY (sns_ddf_convert_usec_to_tick(1000))
static sns_ddf_status_e sns_dd_lsm6ds3_handle_fifo_data(
    sns_ddf_handle_t  dd_handle,
    uint8_t* read_buf,
    sns_ddf_time_t timestamp,
    sns_ddf_time_t cur_time,
    bool flush)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint16_t read_bytes_remain = 0, bytes_to_read = 0, total_samples_per_sensor = 0, total_in_fifo = 0;
  int64_t drift_time = 0, adsp_int_latency = 0, cal_st = 0, st_jitter = 0;
  uint16_t read_bytes = 0;
  uint32_t read_samples = 0;
  //flag for the while loop to guarantee no MD/DBT interrupt get dropped during fifo interrupt.
  sns_ddf_time_t end_time=0, start_time=0, cal_sample_time = 0, use_time = 0;
  uint8_t* ex_buf = NULL;
  uint8_t ex_count = 0;
  uint8_t countH, countL;

  if(NULL == state->fifo_info.raw_buffer) {
    LSM6DS3_MSG_E_0(ERROR, "No raw buffer FIFO already disabled");
    return SNS_DDF_EFAIL;
  }
  //initialize end time, helps if any errors
  end_time = state->fifo_info.last_timestamp;

  countH = read_buf[1] & 0x0F;
  countL = read_buf[0] & 0xFF;

  total_in_fifo = (((countH << 8) & 0xFF00) | countL) * 2; //converting to bytes
  LSM6DS3_DATA_MSG_3(HIGH, "countL=(%x) countH=(%x) fifo_count = (%d) bytes", countL, countH, total_in_fifo);

  if(read_buf[1] & STM_LSM6DS3_FIFO_FULL_STATUS_MASK) {
    total_in_fifo = (STM_LSM6DS3_MAX_FIFO - state->fifo_info.active_sensors) * STM_LSM6DS3_BYTES_PER_SAMPLE;
    LSM6DS3_MSG_2(HIGH, "FIFO is getting FULL - reading slow status_reg (0x%x) updated total_in_fifo (%d)", read_buf[1], total_in_fifo);
    if(state->fifo_info.first_pass == false) {
      LSM6DS3_MSG_1(HIGH, "FIFO FULL status - fifo resetting fifo_enabled (%d)",state->fifo_info.fifo_enabled);
      sns_dd_lsm6ds3_fifo_reset(dd_handle);
      return SNS_DDF_EFAIL;
    }
  }

  uint16_t pattern_pos = ((((read_buf[3] & STM_LSM6DS3_FIFO_PATTERN_STATUS4) << 8 ) & 0xFF00) | 
      (read_buf[2] & STM_LSM6DS3_FIFO_PATTERN_STATUS3));

  LSM6DS3_DATA_MSG_3(HIGH, "pattern_pos (%d) ex_count %d fifo_enabled (%d)", pattern_pos, state->fifo_info.ex_count, state->fifo_info.fifo_enabled);
  if (pattern_pos % STM_LSM6DS3_AXES_PER_SENSOR) {
    if(state->fifo_info.ex_count) {
      if((pattern_pos == 1) &&
          (state->fifo_info.ex_pos == 0))
        total_in_fifo += state->fifo_info.ex_count;
      else
        state->fifo_info.ex_count = 0;
    }
  }

  total_samples_per_sensor =  total_in_fifo / (STM_LSM6DS3_BYTES_PER_SAMPLE * state->fifo_info.active_sensors);
  if(!total_samples_per_sensor) {
    LSM6DS3_DATA_MSG_3(HIGH, "handle_fifo_data: total_in_fifo %d total_sps %d a_sensors %d", 
        total_in_fifo, total_samples_per_sensor, state->fifo_info.active_sensors);
    if(flush == true) {
      state->fifo_info.ex_buf[0] = read_buf[4];
      state->fifo_info.ex_buf[1] = read_buf[5];
      state->fifo_info.ex_count = 2;
      state->fifo_info.ex_pos = pattern_pos;
    } else
      state->fifo_info.ex_count = 0;
    return SNS_DDF_EFAIL;
  }

  // send a FIFO Watermark event to SMGR
  if(flush == true) {
    LSM6DS3_MSG_3(HIGH, "cur_time (%u) last_timestamp (%u) total_in_fifo (%d) bytes", cur_time, state->fifo_info.last_timestamp, total_in_fifo);
    state->fifo_info.first_pass = true;
    if(total_in_fifo % STM_LSM6DS3_BYTES_PER_SAMPLE) {
      //reset the fifo
      LSM6DS3_MSG_E_2(HIGH, "Resetting - FIFO bytes (%d) not multiple of 6 - pattern (%d)", total_in_fifo, pattern_pos);
      sns_dd_lsm6ds3_fifo_reset(dd_handle);
      return SNS_DDF_EFAIL;
    }
    ex_buf = &read_buf[4];
    ex_count = 2;
  } else if(state->fifo_info.first_pass == true) {
      if(total_samples_per_sensor > 1)
          state->fifo_info.avg_sample_time = 0;//reset here, recalculate
  }

  //initialize read_count_bytes
  read_bytes_remain = total_in_fifo; //total fifo bytes

  LSM6DS3_DATA_MSG_3(HIGH, "handle_fifo_data: total_in_fifo=%u bytes active sensors %d total_samples %d per sensor", 
      total_in_fifo, state->fifo_info.active_sensors, total_samples_per_sensor);

  adsp_int_latency = (int64_t) (cur_time - timestamp);
  cal_sample_time = sns_ddf_convert_usec_to_tick(1000000 / lsm6ds3_odr[state->fifo_info.fifo_rate_idx]);

  //only one sample per sensor
  if(total_samples_per_sensor == 1) {


    bytes_to_read = ALIGN_UP(read_bytes_remain, (STM_LSM6DS3_BYTES_PER_SAMPLE * state->fifo_info.active_sensors));
    if(bytes_to_read == 0) {
      LSM6DS3_DATA_MSG_2(HIGH, "handle_fifo_data: read_bytes_remain (%d) bytes_to_read (%d) not even one sample", read_bytes_remain, bytes_to_read);
      return SNS_DDF_EFAIL;
    }
    read_bytes = sns_dd_read_and_split(dd_handle, &pattern_pos, bytes_to_read, ex_buf, ex_count);
    if(read_bytes >= STM_LSM6DS3_BYTES_PER_SAMPLE) {
      //use last-timestamp, flush or not first pass
      if((flush == true) ||
          (state->fifo_info.first_pass == false)) {

        use_time = cur_time;

        if(state->fifo_info.avg_sample_time != 0) {
          cal_sample_time = state->fifo_info.avg_sample_time;
        }
        end_time = state->fifo_info.last_timestamp + cal_sample_time;
        if(end_time > cur_time)
          end_time = cur_time;
        else {
          drift_time = cur_time - end_time;
          if(drift_time > (0.9 * cal_sample_time)) // if drift time more than 70%
            //still consider avg_st is accurate, so add 50% of diff
            end_time += (drift_time - (cal_sample_time * 0.9));
        }
      } else {

        if(state->fifo_info.last_timestamp)
          cal_st = timestamp - state->fifo_info.last_timestamp;
        else
          cal_st = cal_sample_time;
        LSM6DS3_DATA_MSG_3(HIGH, "timestamp (%u) last_ts (%u) cal_st (%u)", timestamp, state->fifo_info.last_timestamp, cal_st);
        if((cal_st < 1.25 * cal_sample_time) && (cal_st > 0.75 * cal_sample_time)) {
          if(!state->fifo_info.avg_sample_time)
            state->fifo_info.avg_sample_time = cal_sample_time;
          st_jitter = cal_st - state->fifo_info.avg_sample_time;
          cal_st = state->fifo_info.avg_sample_time + st_jitter/2;
          state->fifo_info.avg_sample_time = (state->fifo_info.avg_sample_time + cal_st)/2;
        }

        //adjust timestamp - sometimes ADSP delaying calling interrupt handler
        //check if irq timestamp is way off cur time
        if(adsp_int_latency > ADSP_MAX_INTERRUPT_DELAY) {
          end_time = state->fifo_info.last_timestamp + cal_sample_time;
          use_time = cur_time;
        }
        else {
          if(state->fifo_info.last_timestamp)
            end_time = state->fifo_info.last_timestamp + cal_st;
          else
            end_time = timestamp;
          use_time = timestamp;
        }
        if(end_time > cur_time)
          end_time = cur_time;		
      }
      LSM6DS3_DATA_MSG_3(HIGH, "cur_ts %u ts %u end_ts %u", cur_time, start_time, end_time);	  
      if((state->fifo_info.fifo_enabled & 0x01) &&
          (state->fifo_info.acc_sample_cnt)) {
        sns_dd_send_fifo_data(dd_handle,
            SNS_DDF_SENSOR_ACCEL,
            end_time,
            end_time);
      }
      if((state->fifo_info.fifo_enabled & 0x02) &&
          (state->fifo_info.gyr_sample_cnt)) {
        sns_dd_send_fifo_data(dd_handle,
            SNS_DDF_SENSOR_GYRO,
            end_time,
            end_time);
      }
    }
    read_bytes_remain -= read_bytes;
    state->fifo_info.first_pass = false;

    //update last timestamp
    state->fifo_info.last_timestamp = end_time;

  } else {

    if((flush == true) ||
        (state->fifo_info.first_pass != true) ||
        ((total_samples_per_sensor > state->fifo_info.cur_wmk) &&
         (state->fifo_info.cur_wmk == 1)))
      use_time = cur_time;
    else if((total_samples_per_sensor > state->fifo_info.cur_wmk) &&
        (state->fifo_info.cur_wmk > 1)) {
      //read only wmk samples if cur_wmk > 1
      read_bytes_remain =  state->fifo_info.cur_wmk * (STM_LSM6DS3_BYTES_PER_SAMPLE * state->fifo_info.active_sensors);
      use_time = timestamp;
    } else
      use_time = timestamp;


    do {
      //LIMIT - the max no of bytes to read at once is limited to 120 words (40 samples)
      //convert total samples (6 bytest) to bytes 
      if(read_bytes_remain  > (STM_LSM6DS3_MAX_FIFO_READ/(3 - state->fifo_info.active_sensors))) {
        //limit total_in (in samples)
        bytes_to_read = STM_LSM6DS3_MAX_FIFO_READ/(3 - state->fifo_info.active_sensors);
      } else
        bytes_to_read = read_bytes_remain;

      LSM6DS3_DATA_MSG_3(HIGH, "handle_fifo_data: read_bytes_remain %d active sensors %d bytes_to_read (%d)", 
          read_bytes_remain, state->fifo_info.active_sensors, bytes_to_read);
      bytes_to_read = ALIGN_UP(bytes_to_read, (STM_LSM6DS3_BYTES_PER_SAMPLE * state->fifo_info.active_sensors));
      if(bytes_to_read == 0) {
        LSM6DS3_DATA_MSG_2(HIGH, "handle_fifo_data: a_sensors (%d) bytes_to_read (%d) not even one sample", state->fifo_info.active_sensors, bytes_to_read);
        break;
      }
      total_samples_per_sensor = (read_bytes_remain / STM_LSM6DS3_BYTES_PER_SAMPLE) / state->fifo_info.active_sensors;
      sns_dd_lsm6ds3_timestamp_synthesis(dd_handle, total_samples_per_sensor, &cal_sample_time, use_time, flush);

      if((total_samples_per_sensor <= state->fifo_info.cur_wmk) &&
          (flush != true)) {
        state->fifo_info.avg_sample_time += (cal_sample_time * total_samples_per_sensor) ;
        read_samples += total_samples_per_sensor;
      }
      read_bytes = sns_dd_read_and_split(dd_handle, &pattern_pos, bytes_to_read, ex_buf, ex_count);
      LSM6DS3_DATA_MSG_3(HIGH, "handle_fifo_data: active sensors %d acc_sample_cnt (%d) gyr_sample_cnt (%d)", 
          state->fifo_info.active_sensors, state->fifo_info.acc_sample_cnt, state->fifo_info.gyr_sample_cnt) ;
      if(read_bytes >= (STM_LSM6DS3_BYTES_PER_SAMPLE * state->fifo_info.active_sensors)) {

        //calculate start_time and cal_end for three different cases.
        if((state->fifo_info.fifo_enabled & 0x01) &&
            (state->fifo_info.acc_sample_cnt)){
          if(state->fifo_info.last_timestamp)
            start_time = state->fifo_info.last_timestamp + cal_sample_time;
          else
            start_time = use_time - cal_sample_time * (state->fifo_info.acc_sample_cnt - 1);

          end_time = start_time + cal_sample_time * (state->fifo_info.acc_sample_cnt - 1);
          if(end_time > use_time)
            end_time = use_time;
          sns_dd_send_fifo_data(dd_handle,
              SNS_DDF_SENSOR_ACCEL,
              start_time,
              end_time);
        }
        if((state->fifo_info.fifo_enabled & 0x02) &&
            (state->fifo_info.gyr_sample_cnt)) {
          if(state->fifo_info.last_timestamp)
            start_time = state->fifo_info.last_timestamp + cal_sample_time;
          else
            start_time = use_time - cal_sample_time * (state->fifo_info.gyr_sample_cnt - 1);

          end_time = start_time + cal_sample_time * (state->fifo_info.gyr_sample_cnt - 1);
          if(end_time > use_time)
            end_time = use_time;
          sns_dd_send_fifo_data(dd_handle,
              SNS_DDF_SENSOR_GYRO,
              start_time,
              end_time);

        }
      }
      state->fifo_info.first_pass = false;
      //reset ex_count
      ex_count = 0;
      //update fifo_info.last_timestamp
      //state->fifo_info.last_timestamp = end_time;
      read_bytes_remain -= read_bytes;
      //update last timestamp
      state->fifo_info.last_timestamp = end_time;
      LSM6DS3_DATA_MSG_3(HIGH, "handle_fifo_data: read_bytes_remain=%d read_bytes %d last_timestamp %u",read_bytes_remain, read_bytes, state->fifo_info.last_timestamp);
    }while (read_bytes_remain > 0);

    if((flush != true) && (read_samples))
      state->fifo_info.avg_sample_time /= read_samples;
  }

  if((state->fifo_info.fifo_enabled & 0x0C) && 
      (state->fifo_info.temp_sample_cnt)) {
    sns_dd_send_fifo_data(dd_handle,
        SNS_DDF_SENSOR_TEMP,
        state->fifo_info.temp_data.timestamp,
        end_time);
    state->fifo_info.temp_sample_cnt = 0;
  }

  if(total_in_fifo >= (STM_LSM6DS3_BYTES_PER_SAMPLE * state->fifo_info.active_sensors)) {
    //resetting end_time based on cur_time
    drift_time = (int64_t) (use_time - end_time);
    if ((drift_time > 0) && (drift_time > cal_sample_time)) {
      end_time += ALIGN_UP(drift_time, cal_sample_time);
      LSM6DS3_MSG_3(HIGH, "handle_fifo_data: adjusting drift_time %u cur_time %u cal_sample_time %d", drift_time, use_time, cal_sample_time);
    }
  }
  state->fifo_info.last_timestamp = end_time;
  return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_lsm6ds3_flush_fifo(
    sns_ddf_handle_t dd_handle,
    uint32_t num_samples,
    bool fifo_resetting)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t rw_buffer[10] = {0};
  uint8_t rw_bytes = 0;
  sns_ddf_time_t cur_time = 0;
  LSM6DS3_MSG_2(HIGH, "Flush_fifo: fifo_resetting %d fifo_enabled 0x%x", fifo_resetting, state->fifo_info.fifo_enabled);
  do {
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LSM6DS3_REG_SENSOR_HUB10,
        rw_buffer,
        9,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status ;
    if(rw_bytes != 9)
      return SNS_DDF_EBUS;

    cur_time = sns_ddf_get_timestamp();
    LSM6DS3_DATA_MSG_2(HIGH, "Flush_fifo: fifo_resetting %d cur_time %u", fifo_resetting, cur_time);
    status = sns_dd_lsm6ds3_handle_fifo_data(dd_handle, &rw_buffer[3], cur_time, cur_time, true);
  } while((fifo_resetting == true) && (status == SNS_DDF_SUCCESS));
  return SNS_DDF_SUCCESS;
}

void sns_dd_lsm6ds3_handle_fifo_int(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_time_t    timestamp,
    bool* fifo_retry)
{
  sns_ddf_status_e status;
  sns_ddf_time_t    cur_time;
  uint8_t rw_buffer[4] = {0};
  uint8_t rw_bytes = 0;
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);

  //check if fifo is enabled and int is enabled
  //if not, force disable fifo and interrupt
  if((!state->fifo_info.fifo_enabled) ||
    (!state->fifo_info.fifo_int_enabled) ||
    (!state->fifo_info.active_sensors)) {

    //disable fifo
    rw_buffer[0] = 0x0;
    sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_FIFO_CTRL5,
        rw_buffer,
        1,
        0xFF);
    state->fifo_info.fifo_rate_idx = -1;

    sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_INT1_CTRL,
        rw_buffer,
        1,
        STM_LSM6DS3_FIFO_INT_MASK);
    sns_dd_lsm6ds3_clear_wmk(dd_handle);
    //TODO - cleare decimation
    *fifo_retry = false;
    return;
  }

  // verify source of interrupts by reading FIFO_SRC.
  status = sns_ddf_read_port(
      state->port_handle,
      STM_LSM6DS3_REG_FIFO_STATUS1,
      rw_buffer,
      4,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return;
  if(rw_bytes != 4)
    return;

  cur_time = sns_ddf_get_timestamp();
  LSM6DS3_DATA_MSG_4(HIGH, "FS0(0x%x) FS1=(0x%x) FS2=(0x%x) FS3=(0x%x) ", rw_buffer[0], rw_buffer[1], rw_buffer[2], rw_buffer[3]);

  //FIFO overflow interrupt handling.
  if(rw_buffer[1] & STM_LSM6DS3_FIFO_OVR_STATUS_MASK) {
    LSM6DS3_MSG_1(HIGH, "handle_fifo_int: Overflow - fifo resetting fifo_enabled (%d)",state->fifo_info.fifo_enabled);
    sns_dd_lsm6ds3_fifo_reset(dd_handle);

    state->fifo_info.first_pass = false;
    *fifo_retry = false;
  }

  //FIFO watermark interrupt(without overflow flag) handling
  if((0x0 == (rw_buffer[1] & STM_LSM6DS3_FIFO_OVR_STATUS_MASK)) && 
      (rw_buffer[1] & STM_LSM6DS3_FIFO_WTM_STATUS_MASK))
  {

    if(rw_buffer[1] & STM_LSM6DS3_FIFO_EMPTY_STATUS_MASK) {
      LSM6DS3_DATA_MSG_0(LOW, "fifo empty flag set");
      *fifo_retry = false;
    } else {
      if((state->acc_desired_rate_idx >=0) && 
          (state->fifo_info.fifo_enabled & 0x01)){
        status =sns_dd_lsm6ds3_send_event_smgr(dd_handle,
            SNS_DDF_SENSOR_ACCEL,
            SNS_DDF_EVENT_FIFO_WM_INT);
        if(status != SNS_DDF_SUCCESS)
          return ;
      }
      if((state->gyr_desired_rate_idx >=0) && 
          (state->fifo_info.fifo_enabled & 0x02)){
        status =sns_dd_lsm6ds3_send_event_smgr(dd_handle,
            SNS_DDF_SENSOR_GYRO,
            SNS_DDF_EVENT_FIFO_WM_INT);
        if(status != SNS_DDF_SUCCESS)
          return ;
      }
      status = sns_dd_lsm6ds3_handle_fifo_data(dd_handle, rw_buffer, timestamp, cur_time, false);
        if(status == SNS_DDF_SUCCESS)
          *fifo_retry = true;
        else
          *fifo_retry = false;
    }
  }

}

/**
 * @brief Implement trigger_fifo_data API.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm6ds3_trigger_fifo(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    uint16_t num_samples,
    bool trigger_now)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  LSM6DS3_MSG_3(HIGH, "trigger_fifo: get called, sensor (%d) num_samples=%u, cur_time=%u",
      sensor, num_samples, sns_ddf_get_timestamp());

  if(!((SNS_DDF_SENSOR_ACCEL == sensor) ||
        (SNS_DDF_SENSOR_GYRO == sensor) ||
        (num_samples < STM_LSM6DS3_MAX_FIFO)))
  {
    LSM6DS3_MSG_E_2(HIGH, "trigger_fifo: SNS_DDF_EINVALID_PARAM , sensor=%u num_samples %d", sensor, num_samples);
    return SNS_DDF_EINVALID_PARAM;
  }

  if(!trigger_now || !state->fifo_info.fifo_enabled)
  {
    LSM6DS3_MSG_E_2(HIGH, "trigger_fifo: returning .. trigger_now (%d) fifo_info.fifo_enabled (%d)", trigger_now, state->fifo_info.fifo_enabled);
    return SNS_DDF_SUCCESS;
  }

  status = sns_dd_lsm6ds3_flush_fifo(dd_handle, num_samples, false);
  //reset the intp variables ... flushing
  //state->data_intp.acc_intp_status = false;
  //state->data_intp.gyr_intp_status = false;
  return status;
}
