/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
 *
 * File Name         : sns_dd_lsm6ds3_selftest.c
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
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "sns_dd_inertial_test.h"

extern sns_ddf_driver_if_s sns_dd_lsm6ds3_if;
extern sns_ddf_status_e sns_dd_lsm6ds3_collect_testdata(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    int16_t*    data,
    uint32_t*   err,
    uint8_t     num_samples);

extern sns_ddf_odr_t lsm6ds3_odr[STM_LSM6DS3_ODR_NUM];
extern stm_lsm6ds3_fifo_odr lsm6ds3_fifo_odr_reg_values[STM_LSM6DS3_ODR_NUM];
/**
 * @brief Callback function to report inertial SW self test and bias calibration results.
 *
 * @param[in]  handle    Handle to a driver instance.
 * @param[in]  sensor    Sensor type.
 * @param[in]  status    Inertial SW self test result.
 * @param[in]  err       Device error code.
 * @param[in]  biases    Sensor biases after offset calibration.
 *
 * @return No return;
 */
static void sns_dd_lsm6ds3_SWtest_notify(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_status_e ret_status,
    uint32_t err,
    q16_t* biases)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  q16_t* biases_ptr = NULL;
  uint8_t i;
  uint32_t sub_dev_inx;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_ddf_powerstate_e powerstate  = SNS_DDF_POWERSTATE_ACTIVE;
  //search for sensor in sub devices
  if((sensor != SNS_DDF_SENSOR_ACCEL) &&
      (sensor != SNS_DDF_SENSOR_GYRO)) {
    return;
  }
  status = lsm6ds3_search_subdev_by_sensortype(
      state,
      sensor,
      &sub_dev_inx);
  if(status != SNS_DDF_SUCCESS)
  {
    LSM6DS3_MSG_1(HIGH, "sns_dd_parse_and_send_gyr_fifo_data:lsm6ds3_search_subdev_by_sensortype status==%u ",status );
    return ;
  }

  LSM6DS3_MSG_3(HIGH, "LSM6DS3, self-test notify begin: Sensor(%d), status(%d), err(%d)",
      sensor, ret_status, err);
  sns_ddf_smgr_notify_test_complete(
      state->sub_dev[sub_dev_inx].smgr_handle,
      sensor,
      ret_status, err);

  //Disable sensor and self test
  sns_dd_gyr_lsm6ds3_set_datarate(dd_handle, -1, false);
  powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
  sns_dd_lsm6ds3_set_attr(dd_handle, sensor, SNS_DDF_ATTRIB_POWER_STATE, &powerstate);
  if((ret_status == SNS_DDF_SUCCESS) &&
      (err == SNS_DD_ITEST_SUCCESS)) {
    if(sensor == SNS_DDF_SENSOR_ACCEL) {
      biases_ptr = state->xl_biases;
    }
    else if(sensor == SNS_DDF_SENSOR_GYRO ) {
      biases_ptr = state->g_biases;
    }
    for(i = 0; i < STM_LSM6DS3_AXES_PER_SENSOR; i++)
    {
      biases_ptr[i] = biases[i];
    }

    LSM6DS3_MSG_3(HIGH, "LSM6DS3, self-test notify bias: biasX(%d), biasY(%d), biasZ(%d)",
        biases_ptr[0], biases_ptr[1], biases_ptr[2]);
    LSM6DS3_MSG_3(HIGH, "LSM6DS3, self-test notify event: sensor(%d), SNS_DDF_EVENT_BIAS_READY(%d), status(%d)",
        sensor, SNS_DDF_EVENT_BIAS_READY, ret_status);

    sns_ddf_smgr_notify_event(
        state->sub_dev[sub_dev_inx].smgr_handle,
        sensor,
        SNS_DDF_EVENT_BIAS_READY);
  }

  LSM6DS3_MSG_3(HIGH, "LSM6DS3, self-test notify exit: Sensor(%d), status(%d), err(%d)",
      sensor, ret_status, err);
}

static void sns_dd_lsm6ds3_handle_gyr_selftest_timeout(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e  status = SNS_DDF_EFAIL;
  uint8_t rw_buffer[2] = {0};
  uint32_t error = 0;
  int16_t OUT_NOST[STM_LSM6DS3_AXES_PER_SENSOR] = {0};
  int16_t OUT_ST[STM_LSM6DS3_AXES_PER_SENSOR] = {0};
  int32_t OUTX_NOST,OUTY_NOST,OUTZ_NOST;
  int32_t OUTX_ST,OUTY_ST,OUTZ_ST;
  uint32_t sub_dev_inx;
  sns_ddf_powerstate_e powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
  sns_ddf_sensor_e sensor = SNS_DDF_SENSOR_GYRO;

  //search for gyro sensor in sub devices
  status = lsm6ds3_search_subdev_by_sensortype(
      state,
      sensor,
      &sub_dev_inx);
  if(status != SNS_DDF_SUCCESS)
  {
    LSM6DS3_MSG_1(HIGH, "gyr_selftest_timeout:lsm6ds3_search_subdev_by_sensortype status==%u ",status );
    return ;
  }
  if(state->timer == NULL)
  {
    sns_ddf_smgr_notify_test_complete(
        state->sub_dev[sub_dev_inx].smgr_handle,
        sensor, status, error);

    return;
  }
  else
  {
    sns_ddf_timer_release(state->timer);
    state->timer = NULL;
  }

  //Collect data
  memset(OUT_NOST, 0x0, sizeof(int16_t)*STM_LSM6DS3_AXES_PER_SENSOR);
  status = sns_dd_lsm6ds3_collect_testdata(dd_handle, sensor, OUT_NOST, &error, 6);
  if(status != SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 HW self test collect test data failed status %d %d %d",status, 0,0);

    powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    sns_dd_lsm6ds3_set_attr(dd_handle, sensor, SNS_DDF_ATTRIB_POWER_STATE, &powerstate);
    sns_ddf_smgr_notify_test_complete(
        state->sub_dev[sub_dev_inx].smgr_handle,
        sensor, status, error);
    return;
  }

  //Enable self test
  rw_buffer[0] = 0x04;
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_CTRL5,
      rw_buffer,
      1,
      0x00);
  if(status != SNS_DDF_SUCCESS) {

    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 HW self test write port failed status %d %d %d",status, 0,0);

    powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    sns_dd_lsm6ds3_set_attr(dd_handle, sensor, SNS_DDF_ATTRIB_POWER_STATE, &powerstate);
    sns_ddf_smgr_notify_test_complete(
        state->sub_dev[sub_dev_inx].smgr_handle,
        sensor, status, error);
    return;
  }

  // wait 60 ms.
  sns_ddf_delay(60000);

  //Collect data
  memset(OUT_ST, 0x0, sizeof(int16_t)*STM_LSM6DS3_AXES_PER_SENSOR);
  status = sns_dd_lsm6ds3_collect_testdata(dd_handle, sensor, OUT_ST, &error, 6);
  if(status != SNS_DDF_SUCCESS){
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 HW self test collect test data failed status %d %d %d",status, 0,0);

    powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    sns_dd_lsm6ds3_set_attr(dd_handle, sensor, SNS_DDF_ATTRIB_POWER_STATE, &powerstate);
    sns_ddf_smgr_notify_test_complete(
        state->sub_dev[sub_dev_inx].smgr_handle,
        sensor, SNS_DDF_EBUS, error);
    return ;
  }

  //Check self test limits
  OUTX_NOST = OUT_NOST[0] * 70;
  OUTY_NOST = OUT_NOST[1] * 70;
  OUTZ_NOST = OUT_NOST[2] * 70;
  OUTX_ST = OUT_ST[0] * 70;
  OUTY_ST = OUT_ST[1] * 70;
  OUTZ_ST = OUT_ST[2] * 70;
  if((STM_LSM6DS3_2000DPS_ST_MIN <= abs(OUTX_ST-OUTX_NOST))
      &&(abs(OUTX_ST-OUTX_NOST) <= STM_LSM6DS3_2000DPS_ST_MAX)
      &&(STM_LSM6DS3_2000DPS_ST_MIN <= abs(OUTY_ST-OUTY_NOST))
      &&(abs(OUTY_ST-OUTY_NOST)<= STM_LSM6DS3_2000DPS_ST_MAX)
      &&(STM_LSM6DS3_2000DPS_ST_MIN <= abs(OUTZ_ST-OUTZ_NOST))
      &&(abs(OUTZ_ST-OUTZ_NOST)<= STM_LSM6DS3_2000DPS_ST_MAX))
    status = SNS_DDF_SUCCESS;
  else
    status = SNS_DDF_EFAIL;

  rw_buffer[0] = 0x0;
  sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_CTRL5,
      rw_buffer,
      1,
      0x00);

  //Disable sensor and self test
  sns_dd_gyr_lsm6ds3_set_datarate(dd_handle, -1, false);

  powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
  sns_dd_lsm6ds3_set_attr(dd_handle, sensor, SNS_DDF_ATTRIB_POWER_STATE, &powerstate);
  rw_buffer[0] = 0x0
    | (0<<7)           // BOOT bit
    | (0<<6)           // BDU bit
    | (0<<5)           // H_LACTIVE bit
    | (0<<4)           // PP_OD bit
    | (0<<3)           // SIM bit
    | (1<<2)           // IF_INC
    | (0<<1)           // BLE
    | 0;               // SW_RESET
  sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_CTRL3,
      rw_buffer,
      1,
      0xFF);

  LSM6DS3_MSG_2(HIGH, "LSM6DS3 gyro_selftest timeout returning status(%d) err(0x%x) ",status, error);

  sns_ddf_smgr_notify_test_complete(
      state->sub_dev[sub_dev_inx].smgr_handle,
      sensor, status, error);

}
/**
 * @brief Check XLDA/GDA in STATUS register, return data ready flag.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[out] data_flag      data ready flag to set.
 *                            true:  a new set of data is available.
 *                            false: a new set of data is not yet available.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_lsm6ds3_is_dataready(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    bool *data_flag)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;
  uint8_t status_bit = 0;

  if(sensor == SNS_DDF_SENSOR_ACCEL){
    status_bit = 0x01;

  } else if(sensor == SNS_DDF_SENSOR_GYRO){
    status_bit = 0x02;

  } else {
    return SNS_DDF_EINVALID_PARAM;
  }
  // read status register
  status = sns_ddf_read_port(
      state->port_handle,
      STM_LSM6DS3_REG_STATUS,
      &rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(rw_bytes != 1)
    return SNS_DDF_EBUS;

  if((rw_buffer & status_bit) > 0)
    *data_flag = true;
  else
    *data_flag = false;

  return SNS_DDF_SUCCESS;
}
/**
 * @brief Collects sample data for lsm6ds3 self test.
 *
 * @param[in]  dd_handle     Handle to a driver instance.
 * @param[in] sensor         sensor to test
 * @param[in] num_samples    num of samples to collect
 * @param[out] data          Output data buffer to be filled in, should be set 0 before called.
 * @param[out] err           err no if any error 
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
#define STM_POOL_CNT    4
#define MAX_WAIT_COUNT  5 //total max wait time 50ms
sns_ddf_status_e sns_dd_lsm6ds3_collect_testdata(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    int16_t*    data,
    uint32_t*   err,
    uint8_t     num_samples)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status;
  uint8_t rw_buffer[STM_LSM6DS3_BYTES_PER_SAMPLE] = { 0 };
  int32_t average_data[STM_LSM6DS3_AXES_PER_SENSOR] = {0};
  int16_t pool_data[STM_POOL_CNT][STM_LSM6DS3_AXES_PER_SENSOR] = {{0}};
  uint8_t rw_bytes = 0;
  bool data_rdy = false;
  uint8_t wait_time_count = 0;
  uint8_t idx_i;
  uint8_t idx_pool = 0, cnt_pool = 0, i_pool = 0, j_pool = 0;
  uint8_t temp = 0;
  uint8_t out_reg = 0x0;

  *err = 0;
  if(sensor == SNS_DDF_SENSOR_ACCEL){
    out_reg = STM_LSM6DS3_REG_OUT_X_L_XL;

  } else if(sensor == SNS_DDF_SENSOR_GYRO){
    out_reg = STM_LSM6DS3_REG_OUT_X_L_G;

  } else {
    return SNS_DDF_EINVALID_PARAM;
  }
  //Collect data
  for(idx_i=0;idx_i<num_samples;idx_i++)
  {
    wait_time_count = 0;
    do{
      status = sns_dd_lsm6ds3_is_dataready(dd_handle, sensor, &data_rdy);
      if(status != SNS_DDF_SUCCESS) {
        return status;
      }
      if(data_rdy == false) {
        sns_ddf_delay(10000);
        if(++wait_time_count > MAX_WAIT_COUNT){
          LSM6DS3_MSG_E_3(ERROR, "Data not ready for 50ms ", 0, 0, 0);

          return SNS_DDF_EDEVICE;
        }
      }
    }while(data_rdy == false);

    status = sns_ddf_read_port(
        state->port_handle,
        out_reg,
        rw_buffer,
        STM_LSM6DS3_BYTES_PER_SAMPLE,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != STM_LSM6DS3_BYTES_PER_SAMPLE)
      return SNS_DDF_EBUS;

    if(0==idx_i)
      continue;

    pool_data[idx_pool][0] = (int16_t) (((rw_buffer[1]) << 8) | rw_buffer[0]);
    pool_data[idx_pool][1] = (int16_t) (((rw_buffer[3]) << 8) | rw_buffer[2]);
    pool_data[idx_pool][2] = (int16_t) (((rw_buffer[5]) << 8) | rw_buffer[4]);

    cnt_pool++;
    //idx_pool = ++idx_pool % STM_POOL_CNT;
    temp = ++idx_pool % STM_POOL_CNT;
    idx_pool = temp;
    if (cnt_pool >= STM_POOL_CNT){
      // check consecutive data values are the same
      for (i_pool=1,j_pool=1;i_pool<STM_POOL_CNT;i_pool++){
        if ((pool_data[0][0] == pool_data[i_pool][0])
            && (pool_data[0][1] == pool_data[i_pool][1])
            && (pool_data[0][2] == pool_data[i_pool][2]))
          j_pool++;
        else
          break;
      }
      if (j_pool >= STM_POOL_CNT){
        // four samples with same values are detected
        *err = 1;
        return SNS_DDF_EFAIL;
      }
    }

    average_data[0] += (int16_t) (((rw_buffer[1]) << 8) | rw_buffer[0]);
    average_data[1] += (int16_t) (((rw_buffer[3]) << 8) | rw_buffer[2]);
    average_data[2] += (int16_t) (((rw_buffer[5]) << 8) | rw_buffer[4]);
  }

  for(idx_i=0;idx_i<STM_LSM6DS3_AXES_PER_SENSOR;idx_i++)
    data[idx_i] = average_data[idx_i] / (num_samples - 1); //ignored first sample

  LSM6DS3_MSG_3(MED, "avg data: x = %d, y = %d, z = %d", data[0], data[1], data[2]);

  return SNS_DDF_SUCCESS;
}
void sns_dd_lsm6ds3_handle_selftest_timeout(
    sns_ddf_handle_t dd_handle,
    void* arg)
{
  stm_lsm6ds3_selftest_t* test_info = (stm_lsm6ds3_selftest_t *)arg;
  if(test_info != NULL) {
    if(test_info->test == SNS_DDF_TEST_SELF_HW)
      sns_dd_lsm6ds3_handle_gyr_selftest_timeout(dd_handle);
  }
}
/**
 * @brief Runs lsm6ds3 inertial software self test and bias calibration.
 *
 * @param[in]  dd_handle   Handle to a driver instance.
 * @param[in]  sensor      Sensor type.
 *
 * @return SNS_DDF_PENDING if this SW self test is prepared to be performed later; SNS_DDF_EFAIL
 *         if SW self test fails. Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS,
 *         SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lsm6ds3_SW_selftest(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor
    )
{
  sns_ddf_status_e status;
  sns_dd_inertial_test_config_s test_config;

  LSM6DS3_MSG_3(HIGH, "LSM6DS3 sw self test sensor (%d), (%d), (%d)",sensor, 0, 0);
  test_config.sample_rate = 104;
  test_config.num_samples = 64;
  test_config.max_variance = FX_FLTTOFIX_Q16(383);                                       // max variance
  
  test_config.bias_thresholds[0] = FX_FLTTOFIX_Q16(STM_LSM6DS3_ACC_SWST_MAX_BIAS1 * G/1000); // x-axis max bias
  test_config.bias_thresholds[1] = FX_FLTTOFIX_Q16(STM_LSM6DS3_ACC_SWST_MAX_BIAS1 * G/1000); // y-axis max bias
  test_config.bias_thresholds[2] = FX_FLTTOFIX_Q16(STM_LSM6DS3_ACC_SWST_MAX_BIAS2 * G/1000); // z-axis max bias

  status = sns_dd_acc_lsm6ds3_set_range(dd_handle, 1);
  if(SNS_DDF_SUCCESS != status) {
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 sw self test sensor (%d) set range failed (%d),(%d)",sensor, 0, 0);
    return status;
  }

  status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle, STM_LSM6DS3_ODR104);
  if(SNS_DDF_SUCCESS != status) {
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 sw self test sensor (%d) set datarate failed (%d),(%d)",sensor, 0, 0);
    return status;
  }

  // wait 3 ODR.
  sns_ddf_delay(30000);

  status = sns_dd_inertial_test_run(
      sensor,
      dd_handle,
      &sns_dd_lsm6ds3_if,
      &test_config,
      &sns_dd_lsm6ds3_SWtest_notify);

  if(SNS_DDF_SUCCESS != status) {
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 sw self test sensor (%d) inertial test run failed (%d),(%d)",sensor, 0, 0);
    return status;
  }

  LSM6DS3_MSG_3(HIGH, "LSM6DS3 sw self test sensor (%d) success - returning SNS_DDF_PENDING (%d),(%d)",sensor, 0, 0);
  return SNS_DDF_PENDING;
}


/**
 * @brief Runs lsm6ds3 inertial software self test and bias calibration.
 *
 * @param[in]  dd_handle   Handle to a driver instance.
 * @param[in]  sensor      Sensor type.
 *
 * @return SNS_DDF_PENDING if this SW self test is prepared to be performed later; SNS_DDF_EFAIL
 *         if SW self test fails. Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS,
 *         SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_dd_gyr_lsm6ds3_SW_selftest(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor
    )
{
  sns_ddf_status_e status;
  sns_dd_inertial_test_config_s test_config;


  LSM6DS3_MSG_3(HIGH, "LSM6DS3 sw self test sensor (%d), (%d), (%d)",sensor, 0, 0);
  test_config.sample_rate = 104;
  test_config.num_samples = 64;
  test_config.max_variance = FX_FLTTOFIX_Q16(64);                                       // max variance
  
  test_config.bias_thresholds[0] = FX_FLTTOFIX_Q16(STM_LSM6DS3_GYR_SWST_MAX_BIAS * PI/180); // x-axis max bias
  test_config.bias_thresholds[1] = FX_FLTTOFIX_Q16(STM_LSM6DS3_GYR_SWST_MAX_BIAS * PI/180); // y-axis max bias
  test_config.bias_thresholds[2] = FX_FLTTOFIX_Q16(STM_LSM6DS3_GYR_SWST_MAX_BIAS * PI/180); // z-axis max bias


  status = sns_dd_gyr_lsm6ds3_set_range(dd_handle, 3);
  if(SNS_DDF_SUCCESS != status) {
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 sw self test sensor (%d) set range failed (%d),(%d)",sensor, 0, 0);
    return status;
  }

  status = sns_dd_gyr_lsm6ds3_set_datarate(dd_handle, STM_LSM6DS3_ODR104, false);
  if(SNS_DDF_SUCCESS != status) {
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 sw self test sensor (%d) set datarate failed (%d),(%d)",sensor, 0, 0);
    return status;
  }

  // wait 80ms
  sns_ddf_delay(80000);

  status = sns_dd_inertial_test_run(
      sensor,
      dd_handle,
      &sns_dd_lsm6ds3_if,
      &test_config,
      &sns_dd_lsm6ds3_SWtest_notify);

  if(SNS_DDF_SUCCESS != status) {
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 sw self test sensor (%d) inertial test run failed (%d),(%d)",sensor, 0, 0);
    return status;
  }

  LSM6DS3_MSG_3(HIGH, "LSM6DS3 sw self test sensor (%d) success - returning SNS_DDF_PENDING (%d),(%d)",sensor, 0, 0);
  return SNS_DDF_PENDING;
}



/**
 * @brief Runs lsm6ds3 factory hardware self test and returns the test result.
 *        There is no need to call init before self test, while haveing init called
 *        before it won't be harmful though.
 *        After self test is done, lsm6ds3 driver and device will return to the state
 *        they were in after device is just powered on(need to call init and enable
 *        before getting any data).
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  oem_flag      For OEM specific HW selftst
 * @param[out] err          update err with error code apart from return status.
 *
 * @return SNS_DDF_SUCCESS if this HW self test was done successfully; SNS_DDF_EFAIL
 *         if HW self test fails. Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS,
 *         SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lsm6ds3_HW_selftest(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e sensor,
    uint32_t*         err)
{
  sns_ddf_status_e status;
  uint8_t rw_buffer[8] = {0};
  int16_t OUT_NOST[STM_LSM6DS3_AXES_PER_SENSOR];
  int16_t OUT_ST[STM_LSM6DS3_AXES_PER_SENSOR];
  int32_t OUTX_NOST,OUTY_NOST,OUTZ_NOST;
  int32_t OUTX_ST,OUTY_ST,OUTZ_ST;

  *err = 0;
  LSM6DS3_MSG_3(HIGH, "LSM6DS3 HW self test starting..(%d) (%d),(%d)",0,0,0);

  //Register address automatically incremented during a multiple byte access
  rw_buffer[0] = 0x44;
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_CTRL3,
      rw_buffer,
      1,
      0xFF);
  if(status != SNS_DDF_SUCCESS)
    return status;

  //Initialize accel and turn on sensor enable x/y/x axis
  //BDU =1 FS=2G ODR=50Hz
  //disable gyro sensor

  status = sns_dd_acc_lsm6ds3_set_range(dd_handle, 0);
  if(SNS_DDF_SUCCESS != status) {
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 HW self test sensor (%d) set range failed (%d),(%d)",sensor, 0, 0);
    return status;
  }
  status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle, STM_LSM6DS3_ODR52);
  if(SNS_DDF_SUCCESS != status) {
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 HW self test sensor (%d) set datarate failed (%d),(%d)",sensor, 0, 0);
    return status;
  }

  // wait 200 ms.
  sns_ddf_delay(200000);

  //Collect data
  memset(OUT_NOST, 0x0, sizeof(int16_t)*STM_LSM6DS3_AXES_PER_SENSOR);
  status = sns_dd_lsm6ds3_collect_testdata(dd_handle, sensor, OUT_NOST, err, 6);
  if(status != SNS_DDF_SUCCESS){
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 HW self test collect test data failed status %d %d %d",status, 0,0);
    return status;
  }

  //Enable self test
  rw_buffer[0] = 0x01;
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_CTRL5,
      rw_buffer,
      1,
      0x00);
  if(status != SNS_DDF_SUCCESS)
    return status;

  // wait 200 ms.
  sns_ddf_delay(200000);

  //Collect data
  memset(OUT_ST, 0x0, sizeof(int16_t)*STM_LSM6DS3_AXES_PER_SENSOR);
  status = sns_dd_lsm6ds3_collect_testdata(dd_handle, sensor, OUT_ST, err, 6);
  if(status != SNS_DDF_SUCCESS){
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 HW self test collect test data failed status %d %d %d",status, 0,0);
    return status;
  }

  //Check self test limits
  OUTX_NOST = ((OUT_NOST[0] * STM_LSM6DS3_ACC_SSTVT_2G) / 1000);
  OUTY_NOST = ((OUT_NOST[1] * STM_LSM6DS3_ACC_SSTVT_2G) / 1000);
  OUTZ_NOST = ((OUT_NOST[2] * STM_LSM6DS3_ACC_SSTVT_2G) / 1000);
  OUTX_ST = ((OUT_ST[0] * STM_LSM6DS3_ACC_SSTVT_2G) / 1000);
  OUTY_ST = ((OUT_ST[1] * STM_LSM6DS3_ACC_SSTVT_2G) / 1000);
  OUTZ_ST = ((OUT_ST[2] * STM_LSM6DS3_ACC_SSTVT_2G) / 1000);
  if((STM_LSM6DS3_ST_2G_MIN <= abs(OUTX_ST-OUTX_NOST))
      &&(abs(OUTX_ST-OUTX_NOST) <= STM_LSM6DS3_ST_2G_MAX)
      &&(STM_LSM6DS3_ST_2G_MIN <= abs(OUTY_ST-OUTY_NOST))
      &&(abs(OUTY_ST-OUTY_NOST)<= STM_LSM6DS3_ST_2G_MAX)
      &&(STM_LSM6DS3_ST_2G_MIN <= abs(OUTZ_ST-OUTZ_NOST))
      &&(abs(OUTZ_ST-OUTZ_NOST)<= STM_LSM6DS3_ST_2G_MAX))
    status = SNS_DDF_SUCCESS;
  else
    status = SNS_DDF_EFAIL;


  //Disable sensor and self test
  rw_buffer[0] = 0x0;
  sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_CTRL5,
      rw_buffer,
      1,
      0x00);

  sns_dd_acc_lsm6ds3_set_datarate(dd_handle, -1);
  rw_buffer[0] = 0x0
    | (0<<7)           // BOOT bit
    | (0<<6)           // BDU bit
    | (0<<5)           // H_LACTIVE bit
    | (0<<4)           // PP_OD bit
    | (0<<3)           // SIM bit
    | (1<<2)           // IF_INC
    | (0<<1)           // BLE
    | 0;               // SW_RESET
  sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_CTRL3,
      rw_buffer,
      1,
      0xFF);
  LSM6DS3_MSG_3(HIGH, "LSM6DS3 HW self test returning status(%d) err(0x%x) rsvd(%d)",status, *err, 0);
  return status;
}

/**
 * @brief Runs lsm6ds3 factory hardware self test and returns the test result.
 *        There is no need to call init before self test, while haveing init called
 *        before it won't be harmful though.
 *        After self test is done, lsm6ds3 driver and device will return to the state
 *        they were in after device is just powered on(need to call init and enable
 *        before getting any data).
 *
 * @param[in]  dd_handle    Handle to a driver instance.
 * @param[in]  sensor       sensor to test
 * @param[out] err          update err with error code apart from return status.
 *
 * @return SNS_DDF_SUCCESS if this HW self test was done successfully; SNS_DDF_EFAIL
 *         if HW self test fails. Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS,
 *         SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_dd_gyr_lsm6ds3_HW_selftest(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e sensor,
    uint32_t*         err)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status;
  uint8_t rw_buffer[8] = {0};

  *err = 0;
  LSM6DS3_MSG_3(HIGH, "LSM6DS3 HW self test starting..(%d) (%d),(%d)",0,0,0);

  //verify all the parameters
  if(sensor != SNS_DDF_SENSOR_GYRO)
    return SNS_DDF_EINVALID_PARAM;

  //Register address automatically incremented during a multiple byte access
  rw_buffer[0] = 0x44;
  status = sns_dd_lsm6ds3_write_reg(
      dd_handle,
      STM_LSM6DS3_REG_CTRL3,
      rw_buffer,
      1,
      0xFF);
  if(status != SNS_DDF_SUCCESS)
    return status;

  //Initialize gyro and turn on sensor enable x/y/x axis
  //BDU =1 FS=2000dps ODR=208Hz
  //disable accel
  status = sns_dd_gyr_lsm6ds3_set_range(dd_handle, 3);
  if(SNS_DDF_SUCCESS != status) {
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 HW self test sensor (%d) set range failed (%d),(%d)",sensor, 0, 0);
    return status;
  }
  status = sns_dd_gyr_lsm6ds3_set_datarate(dd_handle, STM_LSM6DS3_ODR208, false);
  if(SNS_DDF_SUCCESS != status) {
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 HW self test sensor (%d) set datarate failed (%d),(%d)",sensor, 0, 0);
    return status;
  }

  //start a timer of 800ms
  //static sns_ddf_timer_s timer;
  //add arg if needed instead of NULL
  sns_ddf_timer_init(&state->timer, dd_handle, &sns_dd_lsm6ds3_if, &state->selftest_info, false);

  sns_ddf_timer_start(state->timer, 800000u);

  return SNS_DDF_PENDING;
}

/**
 * @brief Runs lsm6ds3 self test and returns the test result.
 *        Supports SNS_DDF_TEST_SELF_HW and SNS_DDF_TEST_SELF_SW.
 *        SNS_DDF_TEST_SELF_HW is the hardware self test provided by LSM6DS3.
 *        SNS_DDF_TEST_SELF_SW is the inertial software self test and bias calibration.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm6ds3_run_test(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_test_e    test,
    uint32_t*         err)
{
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_bytes = 0;
  uint8_t rw_buffer = 0;
  *err = 0;
  sns_ddf_powerstate_e powerstate  = SNS_DDF_POWERSTATE_ACTIVE;
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);

  LSM6DS3_MSG_3(HIGH, "LSM6DS3 run test sensor (%d) test (%d) %d", sensor, test,0);
  // putting 10ms delay since sns_dd_acc_lsm6ds3_run_test() is called just after turning on VDD
  sns_ddf_delay(10000);

  state->selftest_info.test = test;
  state->selftest_info.sensor = sensor;
  if ((sensor != SNS_DDF_SENSOR_ACCEL) &&
      (sensor != SNS_DDF_SENSOR_GYRO) &&
      (sensor != SNS_DDF_SENSOR__ALL))
  {
    status = SNS_DDF_EINVALID_PARAM;
    LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 run test returning error - status %d %d %d", status, 0, 0);
    return status;
  }

  //If accel is streaming, return EDEVICE_BUSY.
  if((state->acc_desired_rate_idx >= 0))
    return SNS_DDF_EDEVICE_BUSY;

  if((state->gyr_desired_rate_idx >= 0))
    return SNS_DDF_EDEVICE_BUSY;

  if(test == SNS_DDF_TEST_SELF)
  {
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LSM6DS3_REG_WHO_AM_I,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_E_1(ERROR, "read_port failed- status (%d)",status);
      return status;
    }
    if((rw_bytes!=1) ||
        (rw_buffer != STM_LSM6DS3_WHOAMI_VALUE)) {
      LSM6DS3_MSG_E_3(ERROR, "read_port failed (bytes count or WHOAMI failed)-\
          rw_bytes (%d) rw_buffer (0x%x) WHOAMI (0x%x)",rw_bytes, rw_buffer, STM_LSM6DS3_WHOAMI_VALUE);
      return SNS_DDF_EFAIL;
    }
  } else {
    if((test != SNS_DDF_TEST_SELF_SW) &&
        (test != SNS_DDF_TEST_SELF_HW) &&
        (test != SNS_DDF_TEST_OEM))
      return SNS_DDF_EINVALID_TEST;

    //DD init function should be called before SW self test with accel sensor initialized.
    status = sns_dd_lsm6ds3_reset(dd_handle);
    if(SNS_DDF_SUCCESS != status) {
      LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 run test sensor (%d) reset failed (%d) (%d)",sensor, 0, 0);
      return status;
    }

    powerstate = SNS_DDF_POWERSTATE_ACTIVE;
    status = sns_dd_lsm6ds3_set_attr(dd_handle, sensor, SNS_DDF_ATTRIB_POWER_STATE, &powerstate);
    if(SNS_DDF_SUCCESS != status) {
      LSM6DS3_MSG_E_3(ERROR, "LSM6DS3 run test sensor (%d) set power failed (%d),(%d)",sensor, 0, 0);
      return status;
    }

    if(test == SNS_DDF_TEST_SELF_SW)
    {
      status =  (sensor == SNS_DDF_SENSOR_ACCEL) ?
        sns_dd_acc_lsm6ds3_SW_selftest(dd_handle, sensor) :
        sns_dd_gyr_lsm6ds3_SW_selftest(dd_handle, sensor) ;
    }
    else if(test == SNS_DDF_TEST_SELF_HW)
    {
      status =  (sensor == SNS_DDF_SENSOR_ACCEL) ?
        sns_dd_acc_lsm6ds3_HW_selftest(dd_handle, sensor, err) :
        sns_dd_gyr_lsm6ds3_HW_selftest(dd_handle, sensor, err) ;

    }
    else if (test == SNS_DDF_TEST_OEM)
    {
      status =  (sensor == SNS_DDF_SENSOR_ACCEL) ?
        sns_dd_acc_lsm6ds3_SW_selftest(dd_handle, sensor) :
        sns_dd_gyr_lsm6ds3_SW_selftest(dd_handle, sensor) ;
    }
    if(status != SNS_DDF_PENDING) {
      powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
      sns_dd_lsm6ds3_set_attr(dd_handle, sensor, SNS_DDF_ATTRIB_POWER_STATE, &powerstate);
    }
  }
  LSM6DS3_MSG_3(HIGH, "LSM6DS3, self-test result: Sensor(%d), test_type(%d), status(%d)",
      sensor, test, status);
  LSM6DS3_MSG_3(HIGH, "LSM6DS3, self-test result: err(0x%x), rsvd(%d), rsvd(%d)",
      *err, 0, 0);

  return status;
}

