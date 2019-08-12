/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
 *
 * File Name         : sns_dd_lsm6ds3_esp.c
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
//sns_ddf_driver_if_s sns_dd_lsm6ds3_esp_if;
#if LSM6DS3_ENABLE_ESP
extern sns_ddf_odr_t lsm6ds3_odr[STM_LSM6DS3_ODR_NUM];
#define STM_LSM6DS3_ESP_RANGE_MIN 0
#define STM_LSM6DS3_ESP_RANGE_MAX 1
#define STM_LSM6DS3_ESP_RANGE_NUM 1
#define STM_LSM6DS3_FF_DUR 6
#define STM_LSM6DS3_FF_THS 3 //321mg check table 28 AN4650
extern sns_ddf_driver_if_s sns_dd_lsm6ds3_if;

/**
 * @brief Config lsm6ds3 according to NV_PARAMS.
 *
 *
 * @param[in]  nv_params     pointer to nv_params data structure.
 * @param[out] esp_config      pointer to esp_config state structure.
 *
 * @return void.
 */
static void sns_dd_lsm6ds3_config_esp(
    sns_ddf_nv_params_s* nv_params,
    stm_lsm6ds3_esp_config *esp_config)
{
  bool set_default = true;
  stm_lsm6ds3_nv_params *lsm6ds3_params;

  LSM6DS3_MSG_1(HIGH, "Entering sns_dd_acc_lsm6ds3_config_sm ", 0);
  if((nv_params)&&(nv_params->data)&&(nv_params->data_len >= STM_LSM6DS3_NVPARAMS_DATALEN))
  {
    lsm6ds3_params = (stm_lsm6ds3_nv_params *)((uint8_t *)nv_params->data + STM_LSM6DS3_NVPARAMS_OFFSET);

    if((STM_LSM6DS3_MAG_NUM1 == lsm6ds3_params->magic_num[0])
        && (STM_LSM6DS3_MAG_NUM2 == lsm6ds3_params->magic_num[1])
        && (STM_LSM6DS3_MAG_NUM3 == lsm6ds3_params->magic_num[2])
        && (STM_LSM6DS3_MAG_NUM4 == lsm6ds3_params->magic_num[3]))
    {
      //NV_PARAMS for SM1 and SM2 is valid.
      set_default =  false;

      esp_config->md_thresh = lsm6ds3_params->md_thresh;
      esp_config->md_dur = lsm6ds3_params->md_dur;
      esp_config->md_ODR = lsm6ds3_params->md_ODR;
      esp_config->dbt_thresh = lsm6ds3_params->dbt_thresh;
      esp_config->dbt_max_time_gap = lsm6ds3_params->dbt_max_time_gap;
      esp_config->dbt_quiet_time = lsm6ds3_params->dbt_quiet_time;
      esp_config->dbt_shock_time = lsm6ds3_params->dbt_shock_time;
      return;
    }
  }

  //set default config to SM1 and SM2
  if(set_default)
  {
    LSM6DS3_MSG_1(HIGH, "setting default configurations ", 0);
    esp_config->md_thresh = STM_LSM6DS3_MD_THRESH_2;
    esp_config->md_dur = STM_LSM6DS3_MD_DUR_2;
    esp_config->md_ODR = STM_LSM6DS3_MD_ODR_2;
    esp_config->dbt_thresh = STM_LSM6DS3_DBT_THRESH;
    esp_config->dbt_max_time_gap = STM_LSM6DS3_DBT_MAX_TIME_GAP;
    esp_config->dbt_quiet_time = STM_LSM6DS3_DBT_QUIET_TIME;
    esp_config->dbt_shock_time = STM_LSM6DS3_DBT_SHOCK_TIME;
  }

  return;
}

sns_ddf_status_e sns_dd_lsm6ds3_esp_reset(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint32_t sub_dev_idx = (dd_handle_align_t) dd_handle & (~DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t i=0;
  bool reset = false;
  uint8_t rw_buffer;

  switch(state->sub_dev[sub_dev_idx].sensors[0]) {

    case SNS_DDF_SENSOR_DOUBLETAP:
      state->esp_info.esp_enabled &= ~(STM_LSM6DS3_DBT_IDX);
      break;
    case SNS_DDF_SENSOR_SMD:
      state->esp_info.esp_enabled &= ~(STM_LSM6DS3_SMD_IDX);
      break;
    case SNS_DDF_SENSOR_STEP_EVENT:
      state->esp_info.esp_enabled &= ~(STM_LSM6DS3_STE_IDX);
      break;
    case SNS_DDF_SENSOR_TILT_EVENT:
      state->esp_info.esp_enabled &= ~(STM_LSM6DS3_TILT_IDX);
      break;
    case SNS_DDF_SENSOR_D6D_EVENT:
      state->esp_info.esp_enabled &= ~(STM_LSM6DS3_D6D_IDX);
      break;
    case SNS_DDF_SENSOR_STEP_COUNT:
      state->esp_info.esp_enabled &= ~(STM_LSM6DS3_STC_IDX);
      break;
    case SNS_DDF_SENSOR_FREEFALL_EVENT:
      state->esp_info.esp_enabled &= ~(STM_LSM6DS3_FFE_IDX);
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
  if(reset == true) {
    //Tilt settings
    rw_buffer = 0x80;
    status = sns_dd_lsm6ds3_write_reg(dd_handle, 0x01, &rw_buffer, 1, 0xFF);
    if(status != SNS_DDF_SUCCESS) return status;
    rw_buffer = 0x70;
    status = sns_dd_lsm6ds3_write_reg(dd_handle, 0x10, &rw_buffer, 1, 0xFF);
    if(status != SNS_DDF_SUCCESS) return status;
    rw_buffer = 0x6D;
    status = sns_dd_lsm6ds3_write_reg(dd_handle, 0x11, &rw_buffer, 1, 0xFF);
    if(status != SNS_DDF_SUCCESS) return status;
    rw_buffer = 0x0;
    status = sns_dd_lsm6ds3_write_reg(dd_handle, 0x12, &rw_buffer, 1, 0xFF);
    if(status != SNS_DDF_SUCCESS) return status;
    rw_buffer = 0x00;
    status = sns_dd_lsm6ds3_write_reg(dd_handle, 0x01, &rw_buffer, 1, 0xFF);
    if(status != SNS_DDF_SUCCESS) return status;
    state->esp_info.esp_enabled = 0;
  }

  return status;
}

sns_ddf_status_e sns_dd_lsm6ds3_esp_init(
    sns_ddf_handle_t* dd_handle,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  //TODO if any esp sensor active, shutdown
  state->esp_info.esp_enabled = 0;
  sns_dd_lsm6ds3_config_esp(nv_params, &state->esp_info.esp_config);
  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a SMD enable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_enable_smd(
    sns_ddf_handle_t dd_handle,
    uint32_t value)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_buffer = 0;
  uint8_t mask = 0;

  if(!(state->esp_info.esp_enabled & STM_LSM6DS3_SMD_IDX)) {
    LSM6DS3_MSG_0(LOW, "enable_smd get called");

    rw_buffer = 0x01;
    mask = 0x01;
    if(!(state->esp_info.esp_enabled & 
        (STM_LSM6DS3_STE_IDX | 
         STM_LSM6DS3_STC_IDX | 
         STM_LSM6DS3_TILT_IDX))) {

      rw_buffer |= 0x04;
      mask |= 0x04;
    }

    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_CTRL10,
        &rw_buffer,
        1,
        mask);
    if(status != SNS_DDF_SUCCESS)
      return status;

    state->esp_info.esp_enabled |= (STM_LSM6DS3_SMD_IDX);
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a SMD disable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_disable_smd(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_buffer = 0;
  uint8_t mask = 0;

  LSM6DS3_MSG_0(LOW, "disable_smd get called");
  if(state->esp_info.esp_enabled & STM_LSM6DS3_SMD_IDX) {

    mask = 0x01;
    if(!(state->esp_info.esp_enabled & 
          (STM_LSM6DS3_STE_IDX | 
           STM_LSM6DS3_STC_IDX | 
           STM_LSM6DS3_TILT_IDX)))
      mask |= 0x04;

    //disable SMD HW function
    rw_buffer = 0x0;
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_CTRL10,
        &rw_buffer,
        1,
        mask);
    if(status != SNS_DDF_SUCCESS)
      return status;

    state->esp_info.esp_enabled &= ~(STM_LSM6DS3_SMD_IDX);
  }
  return SNS_DDF_SUCCESS;
}
/**
 * @brief Performs a TILT EVENT enable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_enable_tilt(
    sns_ddf_handle_t dd_handle,
    uint32_t value)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_buffer = 0;

  if(!(state->esp_info.esp_enabled & STM_LSM6DS3_TILT_IDX)) {
    LSM6DS3_MSG_0(LOW, "enable_tilt get called");

    //Register configuration for STEP EVENT
    if(!(state->esp_info.esp_enabled & 
          (STM_LSM6DS3_STE_IDX | 
           STM_LSM6DS3_STC_IDX | 
           STM_LSM6DS3_SMD_IDX))) {
      rw_buffer = 0x04;
      status = sns_dd_lsm6ds3_write_reg(
          dd_handle,
          STM_LSM6DS3_REG_CTRL10,
          &rw_buffer,
          1,
          0x04);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }
    /* 
NOTE: bit 5 enables tilt, bit 0 places the interrupt
in latch mode. If the interrupt is not in latch mode,
reads of func_src do not appear to clear func_src.
*/
    rw_buffer = 0x21;
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_TAP_CFG,
        &rw_buffer,
        1,
        0x21);
    if(status != SNS_DDF_SUCCESS)
      return status;

    state->esp_info.esp_enabled |= (STM_LSM6DS3_TILT_IDX);
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a Tilt EVENT disable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_disable_tilt(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_buffer = 0;

  LSM6DS3_MSG_0(LOW, "disable_tilt get called");
  if(state->esp_info.esp_enabled & STM_LSM6DS3_TILT_IDX) {

    //disable TILT EVENT int & TILT EN
    rw_buffer = 0x0;
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_TAP_CFG,
        &rw_buffer,
        1,
        0x20);
    if(status != SNS_DDF_SUCCESS)
      return status;

    if(!(state->esp_info.esp_enabled & 
          (STM_LSM6DS3_STE_IDX | 
           STM_LSM6DS3_STC_IDX | 
           STM_LSM6DS3_SMD_IDX ))) {
      //disable FUNC_EN HW function
      rw_buffer = 0x0;
      status = sns_dd_lsm6ds3_write_reg(
          dd_handle,
          STM_LSM6DS3_REG_CTRL10,
          &rw_buffer,
          1,
          0x04);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }
    state->esp_info.esp_enabled &= ~(STM_LSM6DS3_TILT_IDX);
  }
  return SNS_DDF_SUCCESS;
}
/**
 * @brief Performs a D6D EVENT enable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_enable_d6d(
    sns_ddf_handle_t dd_handle,
    uint32_t value)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_buffer = 0;

  if(!(state->esp_info.esp_enabled & STM_LSM6DS3_D6D_IDX)) {
    LSM6DS3_MSG_0(LOW, "enable_d6d get called");

    //Register configuration for STEP EVENT
    rw_buffer = 0x40; //60 degrees
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_TAP_THS_6D,
        &rw_buffer,
        1,
        0x60);
    if(status != SNS_DDF_SUCCESS)
      return status;

    rw_buffer = 0x10;//enable LPF2
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_TAP_CFG,
        &rw_buffer,
        1,
        0x10);
    if(status != SNS_DDF_SUCCESS)
      return status;

    rw_buffer = 0x01; //Apply LPF2 on 6D
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_CTRL8_XL,
        &rw_buffer,
        1,
        0x01);
    if(status != SNS_DDF_SUCCESS)
      return status;

    state->esp_info.esp_enabled |= (STM_LSM6DS3_D6D_IDX);
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a d6d EVENT disable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_disable_d6d(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_buffer = 0;

  if(state->esp_info.esp_enabled & STM_LSM6DS3_D6D_IDX) {
    LSM6DS3_MSG_0(LOW, "disable_d6d get called");

    //disable D6D EVENT int & D6D EN
    rw_buffer = 0x0;
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_TAP_CFG,
        &rw_buffer,
        1,
        0x10);
    if(status != SNS_DDF_SUCCESS)
      return status;

    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_CTRL8_XL,
        &rw_buffer,
        1,
        0x01);
    if(status != SNS_DDF_SUCCESS)
      return status;

    state->esp_info.esp_enabled &= ~(STM_LSM6DS3_D6D_IDX);
  }
  return SNS_DDF_SUCCESS;
}
/**
 * @brief Performs a STEP EVENT enable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_enable_ste(
    sns_ddf_handle_t dd_handle,
    uint32_t value)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_buffer = 0;

  if(!(state->esp_info.esp_enabled & STM_LSM6DS3_STE_IDX)) {
    LSM6DS3_MSG_0(LOW, "enable_ste get called");

    if(!(state->esp_info.esp_enabled & 
          (STM_LSM6DS3_STC_IDX | 
           STM_LSM6DS3_SMD_IDX | 
           STM_LSM6DS3_TILT_IDX))) {

      //Register configuration for STEP EVENT
      rw_buffer = 0x04;
      status = sns_dd_lsm6ds3_write_reg(
          dd_handle,
          STM_LSM6DS3_REG_CTRL10,
          &rw_buffer,
          1,
          0x04);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }

    if(!(state->esp_info.esp_enabled & 
          STM_LSM6DS3_STC_IDX)) {
      rw_buffer = 0x40;
      status = sns_dd_lsm6ds3_write_reg(
          dd_handle,
          STM_LSM6DS3_REG_TAP_CFG,
          &rw_buffer,
          1,
          0x40);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }

    state->esp_info.esp_enabled |= (STM_LSM6DS3_STE_IDX);
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a STEP EVENT disable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_disable_ste(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_buffer = 0;

  if(state->esp_info.esp_enabled & STM_LSM6DS3_STE_IDX) {
    LSM6DS3_MSG_0(LOW, "disable_ste get called");

    //disable STEP EVENT int
    if(!(state->esp_info.esp_enabled & 
          STM_LSM6DS3_STC_IDX)) {
      rw_buffer = 0x0;
      status = sns_dd_lsm6ds3_write_reg(
          dd_handle,
          STM_LSM6DS3_REG_TAP_CFG,
          &rw_buffer,
          1,
          0x40);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }

  if(!(state->esp_info.esp_enabled & 
          (STM_LSM6DS3_STC_IDX | 
           STM_LSM6DS3_SMD_IDX | 
           STM_LSM6DS3_TILT_IDX))) {
    //disable FUNC_EN HW function
    rw_buffer = 0x0;
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_CTRL10,
        &rw_buffer,
        1,
        0x04);
    if(status != SNS_DDF_SUCCESS)
      return status;
  }

    state->esp_info.esp_enabled |= ~(STM_LSM6DS3_STE_IDX);
  }
  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a STEP COUNT enable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_enable_stc(
    sns_ddf_handle_t dd_handle,
    uint32_t value)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_buffer = 0;
  uint8_t mask = 0;

  if(!(state->esp_info.esp_enabled & STM_LSM6DS3_STC_IDX)) {
    LSM6DS3_MSG_0(LOW, "enable_stc get called");

      rw_buffer = 0x02;
      mask = 0x02;
    if(!(state->esp_info.esp_enabled & 
          (STM_LSM6DS3_STE_IDX | 
           STM_LSM6DS3_SMD_IDX | 
           STM_LSM6DS3_TILT_IDX))) {
      rw_buffer |= 0x04;
      mask |= 0x04;
    }

    //Register configuration for STEP EVENT
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_CTRL10,
        &rw_buffer,
        1,
        mask);
    if(status != SNS_DDF_SUCCESS)
      return status;

    if(!(state->esp_info.esp_enabled & 
          STM_LSM6DS3_STE_IDX)) {
      rw_buffer = 0x40;
      status = sns_dd_lsm6ds3_write_reg(
          dd_handle,
          STM_LSM6DS3_REG_TAP_CFG,
          &rw_buffer,
          1,
          0x40);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }

    //PEDO_RST not reset to 0, manually resetting
    rw_buffer = 0x00;
    mask = 0x02;
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_CTRL10,
        &rw_buffer,
        1,
        mask);
    if(status != SNS_DDF_SUCCESS)
      return status;

    state->esp_info.esp_enabled |= (STM_LSM6DS3_STC_IDX);
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a STEP COUNT disable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm6ds3_disable_stc(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t rw_buffer = 0;

  if(state->esp_info.esp_enabled & STM_LSM6DS3_STC_IDX) {
    LSM6DS3_MSG_0(LOW, "disable_stc get called");

    //disable PEDO_EN bit
    if(!(state->esp_info.esp_enabled & 
          STM_LSM6DS3_STE_IDX)) {
      rw_buffer = 0x0;
      status = sns_dd_lsm6ds3_write_reg(
          dd_handle,
          STM_LSM6DS3_REG_TAP_CFG,
          &rw_buffer,
          1,
          0x40);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }

    if(!(state->esp_info.esp_enabled & 
          (STM_LSM6DS3_STE_IDX | 
           STM_LSM6DS3_SMD_IDX | 
           STM_LSM6DS3_TILT_IDX))) {

      //disable FUNC_EN HW function
      rw_buffer = 0x0;
      status = sns_dd_lsm6ds3_write_reg(
          dd_handle,
          STM_LSM6DS3_REG_CTRL10,
          &rw_buffer,
          1,
          0x04);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }

    state->esp_info.esp_enabled &= ~(STM_LSM6DS3_STC_IDX);
  }
  return SNS_DDF_SUCCESS;
}
/**
 * @brief Performs a double tap enable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_lsm6ds3_enable_dbt(
    sns_ddf_handle_t dd_handle, 
    uint32_t value)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t write_buffer = 0;

  if(!(state->esp_info.esp_enabled & STM_LSM6DS3_DBT_IDX)) {
    LSM6DS3_MSG_1(HIGH, "enable_dbt dd_handle (%u)", dd_handle);

    LSM6DS3_MSG_2(HIGH, "acc_curr_rate_idx=%u dbt_odr %u", state->acc_cur_rate_idx, STM_LSM6DS3_MIN_DBT_ODR);

    //enable double tap detection on Z axis
    write_buffer = 0x02;
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_TAP_CFG,
        &write_buffer,
        1,
        0x0E);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_1(ERROR, "write_port failed status (%d) ", status);
      return status;
    }
    write_buffer = state->esp_info.esp_config.dbt_thresh; //click threshold
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_TAP_THS_6D,
        &write_buffer,
        1,
        0x1F);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_1(ERROR, "write_port failed status (%d) ", status);
      return status;
    }

    write_buffer = (state->esp_info.esp_config.dbt_max_time_gap << 4 ) |//click time limit
      (state->esp_info.esp_config.dbt_quiet_time << 2) |  //click time_latency
      (state->esp_info.esp_config.dbt_shock_time); //click time_window

    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_TAP_DUR,
        &write_buffer,
        1,
        0xFF);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_1(ERROR, "write_port failed status (%d) ", status);
      return status;
    }

    //enable double tap bit
    write_buffer = 0x80;
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_WAKE_THS,
        &write_buffer,
        1,
        0x80);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_1(ERROR, "write_port failed status (%d) ", status);
      return status;
    }
    state->esp_info.esp_enabled |= (STM_LSM6DS3_DBT_IDX);
  }
  return status;
}
/**
 *@brief Turns off double tap virtual sensor, handles driver state and concurrencies.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 **/
sns_ddf_status_e sns_dd_lsm6ds3_disable_dbt(
        sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status;
  uint8_t write_buffer = 0;

  if(state->esp_info.esp_enabled & STM_LSM6DS3_DBT_IDX) {
    LSM6DS3_MSG_1(HIGH,"disable_dbt handle (%u)", dd_handle);
    //disable dbt sensor.

    write_buffer = 0x00;
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_TAP_CFG,
        &write_buffer,
        1,
        0x0E);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_1(ERROR, "write_port failed status (%d) ", status);
      return status;
    }
    //write 0x3A register only disable dbt interrupt
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_WAKE_THS,
        &write_buffer,
        1,
        0x80);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_1(ERROR, "write_port failed status (%d) ", status);
      return status;
    }
    state->esp_info.esp_enabled &= ~(STM_LSM6DS3_DBT_IDX);
  }
  return SNS_DDF_SUCCESS;
}
/**
 * @brief Performs a free fall enable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_lsm6ds3_enable_ffe(
    sns_ddf_handle_t dd_handle, 
    uint32_t value)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t write_buffer = 0;
  uint8_t ff_dur = 0;

  if(!(state->esp_info.esp_enabled & STM_LSM6DS3_FFE_IDX)) {
    LSM6DS3_MSG_1(HIGH, "enable_ffe dd_handle (%u)", dd_handle);

    LSM6DS3_MSG_2(HIGH, "acc_curr_rate_idx=%u esp_odr %u", state->acc_cur_rate_idx, STM_LSM6DS3_MIN_ESP_ODR);

    //ff duaration 5 bits FF_DUR[5:0]
    ff_dur = (STM_LSM6DS3_FF_DUR & 0x3F);

    //write FF_DUR[5] 
    write_buffer = ((ff_dur << 2) & 0x80);
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_WAKE_DUR,
        &write_buffer,
        1,
        0x80);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_1(ERROR, "write_port failed status (%d) ", status);
      return status;
    }

    write_buffer = ((ff_dur << 3) & 0xF8) | (STM_LSM6DS3_FF_THS & 0x07);
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_FREE_FALL,
        &write_buffer,
        1,
        0xFF);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_1(ERROR, "write_port failed status (%d) ", status);
      return status;
    }
    state->esp_info.esp_enabled |= (STM_LSM6DS3_FFE_IDX);
  }
  return status;
}
/**
 *@brief Turns off free fall sensor
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 **/
sns_ddf_status_e sns_dd_lsm6ds3_disable_ffe(
        sns_ddf_handle_t dd_handle)
{
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status;
  uint8_t write_buffer = 0;

  if(state->esp_info.esp_enabled & STM_LSM6DS3_FFE_IDX) {
    LSM6DS3_MSG_1(HIGH,"disable_ffe handle (%u)", dd_handle);

    write_buffer = 0x0;
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        STM_LSM6DS3_REG_FREE_FALL,
        &write_buffer,
        1,
        0xFF);
    if(status != SNS_DDF_SUCCESS) {
      LSM6DS3_MSG_1(ERROR, "write_port failed status (%d) ", status);
      return status;
    }
    state->esp_info.esp_enabled &= ~(STM_LSM6DS3_DBT_IDX);
  }
  return SNS_DDF_SUCCESS;
}
/**
 * @brief enables esp feature
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  sensor      sensor to enable.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */

sns_ddf_status_e sns_dd_lsm6ds3_control_esp(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    uint32_t value)
{
  sns_ddf_status_e status = SNS_DDF_EINVALID_PARAM;
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint32_t sub_dev_idx = (dd_handle_align_t) dd_handle & (~DD_HANDLE_ALIGN_BITS);
  int8_t rate_idx = -1;
  uint8_t rw_bytes = 0;
  uint8_t rw_buffer = 0;
  int8_t a_cur_odr_idx = state->acc_cur_rate_idx;
  //special case if sensor is sensor _all
  //use sub device and primary device
  if(sensor == SNS_DDF_SENSOR__ALL) {
    sensor = state->sub_dev[sub_dev_idx].sensors[0];
  }

  LSM6DS3_MSG_3(HIGH, "control_esp: sensor=%u value=%d sub_dev_idx (%d)", sensor, value, sub_dev_idx);

  //read FUNC_SRC register to reset the interrupt
  //if((state->int_mode == INT_MODE_LATCH) &&
    if(LSM6DS3_IS_ESP_ACTIVE(state->esp_info)) {
    sns_ddf_read_port(
        state->port_handle,
        STM_LSM6DS3_REG_FUNC_SRC,
        &rw_buffer,
        1,
        &rw_bytes);
  }

  switch(sensor)
  {
    case SNS_DDF_SENSOR_DOUBLETAP:
      status = (value) ? sns_dd_lsm6ds3_enable_dbt(dd_handle, value) :
                      sns_dd_lsm6ds3_disable_dbt(dd_handle);
      break;
    case SNS_DDF_SENSOR_SMD:
      status = (value) ? sns_dd_lsm6ds3_enable_smd(dd_handle, value) :
                      sns_dd_lsm6ds3_disable_smd(dd_handle);
      break;

    case SNS_DDF_SENSOR_TILT_EVENT:
      status = (value) ? sns_dd_lsm6ds3_enable_tilt(dd_handle, value) :
                      sns_dd_lsm6ds3_disable_tilt(dd_handle);
      break;

    case SNS_DDF_SENSOR_D6D_EVENT:
      status = (value) ? sns_dd_lsm6ds3_enable_d6d(dd_handle, value) :
                      sns_dd_lsm6ds3_disable_d6d(dd_handle);
      break;

    case SNS_DDF_SENSOR_STEP_EVENT:
      status = (value) ? sns_dd_lsm6ds3_enable_ste(dd_handle, value) :
                      sns_dd_lsm6ds3_disable_ste(dd_handle);
      break;
    case SNS_DDF_SENSOR_STEP_COUNT:
      status = (value) ? sns_dd_lsm6ds3_enable_stc(dd_handle, value) :
                      sns_dd_lsm6ds3_disable_stc(dd_handle);
      break;
    case SNS_DDF_SENSOR_FREEFALL_EVENT:
      status = (value) ? sns_dd_lsm6ds3_enable_ffe(dd_handle, value) :
                      sns_dd_lsm6ds3_disable_ffe(dd_handle);
      break;
    default:
      status = SNS_DDF_EINVALID_PARAM;
  }
  if(status == SNS_DDF_SUCCESS) {
    LSM6DS3_MSG_3(HIGH, "acc_fifo_alive (%d) a_cur_odr_idx (%d) fifo_enabled (0x%x)", state->fifo_info.acc_fifo_alive, a_cur_odr_idx, state->fifo_info.fifo_enabled);
    if(value) {
      rate_idx = LSM6DS3_ESP_RATE(state->esp_info);
      if(rate_idx > a_cur_odr_idx) {
        if(state->fifo_info.fifo_enabled) {
          status = sns_dd_lsm6ds3_update_rate(dd_handle, SNS_DDF_SENSOR_ACCEL, rate_idx);
          if(state->acc_desired_rate_idx >= 0) {
            state->data_intp.acc_intp_status = true;
            sns_dd_lsm6ds3_send_event_smgr(dd_handle,
                SNS_DDF_SENSOR_ACCEL,
                SNS_DDF_EVENT_ODR_CHANGED);
          }
        }
        else
          status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle, rate_idx);
      }
    } else {
      if ((!sns_dd_acc_lsm6ds3_is_active_other_sub_dev(state, sub_dev_idx)) &&
          (state->md_enabled == false) &&
          (state->fifo_info.acc_fifo_alive == false)) {
        status = sns_dd_acc_lsm6ds3_set_datarate(dd_handle, -1);
        if(status != SNS_DDF_SUCCESS)
          return status;
      }
    }
  }
  return status;
}

/**
 * @brief enables esp feature
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  sensor      sensor to enable.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_lsm6ds3_esp_sched_data(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable)
{

  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t value = 0, reg = 0, mask = 0;
  uint8_t rw_bytes = 0;
  bool write = true;
  LSM6DS3_MSG_1(HIGH, "enable_esp_int: sensor=%u", sensor);

  sns_ddf_read_port(
      state->port_handle,
      STM_LSM6DS3_REG_FUNC_SRC,
      &value,
      1,
      &rw_bytes);

  switch(sensor)
  {
    case SNS_DDF_SENSOR_DOUBLETAP:

      LSM6DS3_MSG_1(HIGH, "enable_dbt_interrupt dd_handle (%u)", dd_handle);
      reg = STM_LSM6DS3_REG_MD2_CFG;
      value = (enable) ? 0x08 : 0x0;
      mask = 0x08;
      if (enable)
        sns_ddf_signal_register(state->gpio2_num, state, &sns_dd_lsm6ds3_if, SNS_DDF_SIGNAL_IRQ_RISING);
      else
        sns_ddf_signal_deregister(state->gpio2_num);

      break;

    case SNS_DDF_SENSOR_SMD:

      LSM6DS3_MSG_0(LOW, "enable_smd_int get called");

      reg = STM_LSM6DS3_REG_INT1_CTRL;
      value = (enable) ? 0x40 : 0x0;
      mask = 0x40;

      if(!(state->esp_info.esp_enabled & 
            (STM_LSM6DS3_STE_IDX | STM_LSM6DS3_STC_IDX))) {

        value |= (enable) ? (0x80) : 0x0;
        mask |= (0x80);
      }
      break;

    case SNS_DDF_SENSOR_TILT_EVENT:

      LSM6DS3_MSG_0(LOW, "enable_tilt_int get called");
      reg = STM_LSM6DS3_REG_MD1_CFG;
      value = 0x02;
      mask = 0x02;
      break;

    case SNS_DDF_SENSOR_D6D_EVENT:

      LSM6DS3_MSG_0(LOW, "enable_d6d_int get called");
      reg = STM_LSM6DS3_REG_MD1_CFG;
      value = 0x04;
      mask = 0x04;
      break;

    case SNS_DDF_SENSOR_STEP_EVENT:
      LSM6DS3_MSG_0(LOW, "enable_step_event int get called");
      reg = STM_LSM6DS3_REG_INT1_CTRL;

      if(state->esp_info.esp_enabled & 
          (STM_LSM6DS3_SMD_IDX | STM_LSM6DS3_STC_IDX)) 
        write = false;
      else {
        value = (enable) ? (0x80) : 0x0;
        mask = (0x80);
      }
      break;

    case SNS_DDF_SENSOR_STEP_COUNT:

      LSM6DS3_MSG_0(LOW, "enable_step_count int get called");
      reg = STM_LSM6DS3_REG_INT1_CTRL;
      if(state->esp_info.esp_enabled & 
          (STM_LSM6DS3_SMD_IDX | STM_LSM6DS3_STE_IDX)) 
        write = false;
      else {
        value = (enable) ? (0x80) : 0x0;
        mask = (0x80);
      }
      break;

    case SNS_DDF_SENSOR_FREEFALL_EVENT:

      LSM6DS3_MSG_0(LOW, "enable_freefall_event int get called");
      reg = STM_LSM6DS3_REG_MD1_CFG;
      value = (enable) ? (0x10) : 0x0;
      mask = 0x10;
      break;

    default:
      status = SNS_DDF_EINVALID_PARAM;
  }

  if((status  == SNS_DDF_SUCCESS) &&
      (write == true)) {
    status = sns_dd_lsm6ds3_write_reg(
        dd_handle,
        reg,
        &value,
        1,
        mask);
  }
  return status;
}

/**
 * @brief Retrieves the value of an attribute for a lsm6ds3 sensor.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm6ds3_esp_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems)
{
  if (dd_handle == NULL || value == NULL) {
    LSM6DS3_MSG_E_0(ERROR, "dd_handle is NULL");
    return SNS_DDF_EINVALID_PARAM;
  }
  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint32_t sub_dev_idx = (dd_handle_align_t) dd_handle & (~DD_HANDLE_ALIGN_BITS);
  sns_ddf_sensor_e p_sensor;

  p_sensor = state->sub_dev[sub_dev_idx].sensors[0]; // '0' --> Primary

  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_INFO:
      {
        sns_ddf_power_info_s* power = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler, sizeof(sns_ddf_power_info_s), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == power)
          return SNS_DDF_ENOMEM;
        //current consumption, unit uA
        power->active_current = 900;
        power->lowpower_current = 6;

        *value = power;
        *num_elems = 1;
        return SNS_DDF_SUCCESS;
      }

    case SNS_DDF_ATTRIB_RANGE:
      {
        sns_ddf_range_s* ranges = NULL;
        ranges = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler, STM_LSM6DS3_ESP_RANGE_NUM * sizeof(sns_ddf_range_s),state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == ranges)
          return SNS_DDF_ENOMEM;

        ranges[0].min = STM_LSM6DS3_ESP_RANGE_MIN;
        ranges[0].max = STM_LSM6DS3_ESP_RANGE_MAX;

        *num_elems = STM_LSM6DS3_ESP_RANGE_NUM;
        *value = ranges;
        return SNS_DDF_SUCCESS;
      }

    case SNS_DDF_ATTRIB_RESOLUTION:
      {
        sns_ddf_resolution_t *res = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler, sizeof(sns_ddf_resolution_t), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == res)
          return SNS_DDF_ENOMEM;

        *res = 1;
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

        res->bit_len = 16;
        res->max_freq = lsm6ds3_odr[STM_LSM6DS3_MIN_ESP_ODR];
        *value = res;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }
    case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
      {
        //TODO set odr list as one element of supporting odr
        sns_ddf_odr_t *res = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler ,sizeof(sns_ddf_odr_t), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == res)
          return SNS_DDF_ENOMEM;
        *res = 1;
        *value = res;
        *num_elems = 1;
        return SNS_DDF_SUCCESS;
      }
    case SNS_DDF_ATTRIB_ODR:
      {
        //int8_t lsm6ds3_rate_index = -1;
        sns_ddf_odr_t *res = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler ,sizeof(sns_ddf_odr_t), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == res)
          return SNS_DDF_ENOMEM;

        if(p_sensor == SNS_DDF_SENSOR_DOUBLETAP)
          *res = (state->esp_info.esp_enabled & STM_LSM6DS3_DBT_IDX) ? 1 : 0;
        else if(p_sensor == SNS_DDF_SENSOR_SMD)
          *res = (state->esp_info.esp_enabled & STM_LSM6DS3_SMD_IDX) ? 1 : 0;
        else if(p_sensor == SNS_DDF_SENSOR_TILT_EVENT)
          *res = (state->esp_info.esp_enabled & STM_LSM6DS3_TILT_IDX) ? 1 : 0;
        else if(p_sensor == SNS_DDF_SENSOR_D6D_EVENT)
          *res = (state->esp_info.esp_enabled & STM_LSM6DS3_D6D_IDX) ? 1 : 0;
        else if(p_sensor == SNS_DDF_SENSOR_STEP_EVENT)
          *res = (state->esp_info.esp_enabled & STM_LSM6DS3_STE_IDX) ? 1 : 0;
        else if(p_sensor == SNS_DDF_SENSOR_STEP_COUNT)
          *res = (state->esp_info.esp_enabled & STM_LSM6DS3_STC_IDX) ? 1 : 0;
        else if(p_sensor == SNS_DDF_SENSOR_FREEFALL_EVENT)
          *res = (state->esp_info.esp_enabled & STM_LSM6DS3_FFE_IDX) ? 1 : 0;
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


        lsm6ds3_delays->time_to_active = 1;
        lsm6ds3_delays->time_to_data = 1;
        *value = lsm6ds3_delays;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }

    case SNS_DDF_ATTRIB_DEVICE_INFO:
      {
        sns_ddf_device_info_s *info = sns_dd_lsm6ds3_memhandler_malloc(
            memhandler, sizeof(sns_ddf_device_info_s), state->sub_dev[sub_dev_idx].smgr_handle);
        if(NULL == info)
          return SNS_DDF_ENOMEM;

        if(p_sensor == SNS_DDF_SENSOR_DOUBLETAP)
          info->name = "Double-Tap";
        else if(p_sensor == SNS_DDF_SENSOR_SMD)
          info->name = "Significant Motion Detector";
        else if(p_sensor == SNS_DDF_SENSOR_TILT_EVENT)
          info->name = "Tilt Detector";
        else if(p_sensor == SNS_DDF_SENSOR_D6D_EVENT)
          info->name = "D6D Detector";
        else if(p_sensor == SNS_DDF_SENSOR_STEP_EVENT)
          info->name = "Step Detector";
        else if(p_sensor == SNS_DDF_SENSOR_STEP_COUNT)
          info->name = "Step Counter";
        else if(p_sensor == SNS_DDF_SENSOR_FREEFALL_EVENT)
          info->name = "Free Fall Detector";
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
 * @brief Sets a lsm6ds3 sensor attribute to a specific value.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm6ds3_esp_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value)
{
  sns_ddf_status_e  status = SNS_DDF_SUCCESS;
  sns_ddf_sensor_e ddf_sensor_type;

  if (dd_handle == NULL || value == NULL) {
    LSM6DS3_MSG_E_0(ERROR, "dd_handle is NULL");
    return SNS_DDF_EINVALID_PARAM;
  }

  sns_dd_lsm6ds3_state_t* state = sns_dd_lsm6ds3_get_state(dd_handle);
  uint32_t sub_dev_idx = (dd_handle_align_t) dd_handle & (~DD_HANDLE_ALIGN_BITS);
  ddf_sensor_type = state->sub_dev[sub_dev_idx].sensors[0]; // '0' --> Primary

  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
      {
        sns_ddf_powerstate_e* power_state = value;

        LSM6DS3_MSG_3(LOW, "SET_POWER - sensor (%d) sub_dev_idx (%d) value (%d)", sensor, sub_dev_idx, *power_state);
        if(*power_state == SNS_DDF_POWERSTATE_LOWPOWER)
          status = sns_dd_lsm6ds3_control_esp(dd_handle, ddf_sensor_type, 0);

        if(status == SNS_DDF_SUCCESS)
          state->sub_dev[sub_dev_idx].powerstate = *power_state;
        break;
      }
    case SNS_DDF_ATTRIB_ODR:
      {
        sns_ddf_odr_t* rate = value;
        LSM6DS3_MSG_3(LOW, "set_odr attrib (%d) sensor (%d) value (%d) ",attrib, sensor, *rate);

        status = sns_dd_lsm6ds3_control_esp(dd_handle, ddf_sensor_type, *rate);
        break;
      }

    default:
      status = SNS_DDF_EINVALID_ATTR;
  }

  return status;
}

/**
 * LSM6DS3LHC device driver interface.
 */
sns_ddf_driver_if_s sns_dd_lsm6ds3_esp_if =
{
  &sns_dd_lsm6ds3_esp_init,
  NULL,
  &sns_dd_lsm6ds3_esp_set_attr,
  &sns_dd_lsm6ds3_esp_get_attr,
  NULL,
  NULL,
  &sns_dd_lsm6ds3_esp_reset,
  NULL,
  &sns_dd_lsm6ds3_esp_sched_data,
  NULL,
  NULL
};
#endif // LSM6DS3_ENABLE_ESP
