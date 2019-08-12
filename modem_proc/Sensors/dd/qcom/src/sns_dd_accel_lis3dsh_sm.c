/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
 *
 * File Name         : sns_dd_accel_lis3dsh_sm.c
 * Authors           : Jianjian(Samuel) Huo
 * Version           : V 3.6.1
 * Date              : 03/13/2014
 * Description       : LIS3DSH Accelerometer driver source file, state machines
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
 *******************************************************************************/

#include "sns_dd_accel_lis3dsh.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_signal.h"
#include "string.h"
#include <stdbool.h>

extern sns_ddf_status_e sns_dd_acc_lis3dsh_update_onereg(
    sns_ddf_handle_t dd_handle,
    uint8_t reg_addr,
    uint8_t mask,
    uint8_t new_value);
extern sns_ddf_status_e  sns_dd_acc_lis3dsh_match_odr(
    sns_ddf_odr_t desired_rate,
    sns_ddf_odr_t *new_rate,
    uint32_t *new_index);
extern sns_ddf_status_e sns_dd_acc_lis3dsh_set_datarate(
    sns_ddf_handle_t dd_handle,
    stm_lis3dsh_odr data_rate);
extern const stm_lis3dsh_odr lis3dsh_odr_setting[STM_LIS3DSH_ODR_NUM];
extern sns_ddf_driver_if_s sns_dd_acc_lis3dsh_if;

extern sns_ddf_status_e sns_dd_acc_lis3dsh_enable_dbt_int(
    sns_ddf_handle_t dd_handle);
extern sns_ddf_status_e sns_dd_acc_lis3dsh_disable_dbt(
    sns_ddf_handle_t dd_handle);
	
#define NUM_OF_SM_STEPS   16
#define NUM_OF_SM_CONFIG  13

static uint8_t sm2_steps[NUM_OF_SM_STEPS] = {
  0x05,  //SM2 step 1 at reg address 0x60
  0x17,  //SM2 step 2
  0x02,  //SM2 step 3
  0x63,  //SM2 step 4
  0x45,  //SM2 step 5
  0x17,  //SM2 step 6
  0x02,  //SM2 step 7
  0x63,  //SM2 step 8
  0x11,  //SM2 step 9
  0x00,  //SM2 step 10
  0x00,  //SM2 step 11
  0x00,  //SM2 step 12
  0x00,  //SM2 step 12
  0x00,  //SM2 step 13
  0x00,  //SM2 step 14
  0x00,  //SM2 step 15
};
//TIM1 - timer for checking data to go below threshold - 20msec
//TIM2 - Timer for setting the data after tapping - 40msec
//TIM3 - silence period within which no tap should happen - 70msec
//TIM4 - Maximum time within which second tap should happen - 150msec(Max - TIM3-TIM2)
//THRS1 - Threshold to test the tapping - 570mg
//THRS2 - Threshold to detect in silence period - 250mg

static uint8_t sm2_configs[NUM_OF_SM_CONFIG] = {
  0x58,  //TIM4_2 at reg address    0x70
  0x1C,  //TIM3_2 at reg address    0x71
  0x18,  //TIM2_2_L at reg address 0x72
  0x00,  //TIM2_2_H at reg address 0x73
  0x08,  //TIM1_2_L at reg address 0x74
  0x00,  //TIM1_2_H at reg address 0x75
  0x0F,  //THRS2_2 at reg address  0x76
  0x11,  //THRS1_2 at reg address  0x77
  0x00,  //DES2 at reg address       0x78
  0x08,  //MASK2_B at reg address  0x79
  0x08,  //MASK2_A at reg address  0x7A
  0x11,  //SETT2 at reg address      0x7B
  0x00   //PR2 at reg address         0x7C
};

/**
 * @brief Performs a motion detection enable
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_enable_md(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t*)
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_64);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t write_buffer[3] = {0, 0, 0};
  uint8_t bytes_written = 0;
  uint8_t vector_filter_coeff[4] = {STM_LIS3DSH_VFLIT_COEF1, STM_LIS3DSH_VFLIT_COEF2,
    STM_LIS3DSH_VFLIT_COEF3, STM_LIS3DSH_VFLIT_COEF4};
  sns_ddf_odr_t new_rate;
  uint32_t new_rate_index;
  uint8_t consist_cnt_set = 0;
  uint8_t thresh_set = 0;

  //read OUTS1 register to reset the SM1 interrupt
  sns_ddf_read_port(
      state->port_handle,
      STM_LIS3DSH_OUTS1,
      &write_buffer[0],
      1,
      &bytes_written);

  //Register configuration for wake-up application
  //config ODR and bandwidth
  //only config ODR when double tap sensor is off. ODR when dbt is on is always 400hz.
  if(0 == state->dbt_desired_rate)
  {
    status =  sns_dd_acc_lis3dsh_match_odr(STM_LIS3DSH_MD_ODR,
        &new_rate,
        &new_rate_index);
    if(SNS_DDF_SUCCESS != status)
      return status;

    status = sns_dd_acc_lis3dsh_set_datarate(state,
        lis3dsh_odr_setting[new_rate_index]);
    if(SNS_DDF_SUCCESS != status)
      return status;

    state->acc_cur_rate = new_rate;
    thresh_set = STM_LIS3DSH_MD_THRESH;
    consist_cnt_set =  STM_LIS3DSH_CONSTANT_CNT;
  }
  else
  {
    // default, double-tap enabled

    thresh_set = state->sm_config.md_thresh;
    consist_cnt_set = state->sm_config.consistency_cnt;
  }

  //config motion detection threshold to SM1.
  write_buffer[0] = thresh_set;
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_THRS1_STM1,
      &write_buffer[0],
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 1)
    return SNS_DDF_EBUS;

  write_buffer[0] = 0x02;  //MASK1_B enable +-V
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_MASKB_1,
      &write_buffer[0],
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 1)
    return SNS_DDF_EBUS;

  write_buffer[0] = 0x02; //MASK1_A enable +-V
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_MASKA_1,
      &write_buffer[0],
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 1)
    return SNS_DDF_EBUS;

  write_buffer[0] = 0x01; //SETT1 SITR  = 1
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_SETT1,
      &write_buffer[0],
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 1)
    return SNS_DDF_EBUS;

  if(0x0 == consist_cnt_set)
  {
    //only check 1 sample to go beyond threshold.
    write_buffer[0] = 0x00;  //set timer3 0
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_TIM3_1,
        &write_buffer[0],
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(bytes_written != 1)
      return SNS_DDF_EBUS;

    write_buffer[0] = 0x05;  //reset/next - 0x05
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_STM1_X,
        &write_buffer[0],
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(bytes_written != 1)
      return SNS_DDF_EBUS;
  }else{
    // check multiple samples to go beyond threshold.
    write_buffer[0] = consist_cnt_set - 1;  //set timer3
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_TIM3_1,
        &write_buffer[0],
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(bytes_written != 1)
      return SNS_DDF_EBUS;

    write_buffer[0] = 0x73;  //reset/next - 0x73
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_STM1_X,
        &write_buffer[0],
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(bytes_written != 1)
      return SNS_DDF_EBUS;
  }

  write_buffer[0] = 0x11; //cont
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_STM1_X + 1,
      &write_buffer[0],
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 1)
    return SNS_DDF_EBUS;

  //writing filter coefficients
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_VCF_BASE,
      &vector_filter_coeff[0],
      4,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 4)
    return SNS_DDF_EBUS;

  write_buffer[0] = 0x01;
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_SM1,
      &write_buffer[0],
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 1)
    return SNS_DDF_EBUS;

  write_buffer[0] = 0x68; //enable INT1 and IEA, pulse
  write_buffer[0] |= STM_LIS3DSH_VFLIT_MASK; //enable vector filter
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

  if((0 == state->dbt_desired_rate) && (false == state->md_enabled) 
                                    && (false == state->fifo_int_enabled))
  {
    status = sns_ddf_signal_register(
        state->gpio_num,
        state,
        &sns_dd_acc_lis3dsh_if,
        SNS_DDF_SIGNAL_IRQ_RISING);
    if(status != SNS_DDF_SUCCESS)
      return status;

    state->int_registered = true;
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
sns_ddf_status_e sns_dd_acc_lis3dsh_disable_md(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t*)
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_64);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t write_buffer[4] = {0, 0, 0, 0};
  uint8_t bytes_written = 0;

  // clear the MD interrupt
  //Register Address: Value
  //0x21:      0x00
  write_buffer[0] = 0x00;

  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_SM1,
      &write_buffer[0],
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 1)
    return SNS_DDF_EBUS;


  if((0 == state->dbt_desired_rate) && (false == state->fifo_int_enabled)
                                    && (true == state->int_registered))
  {
    sns_ddf_signal_deregister(state->gpio_num);
    state->int_registered = false;
    state->acc_cur_rate = 0;
  }

  state->md_enabled = false;

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
sns_ddf_status_e sns_dd_acc_lis3dsh_enable_dbt(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t*)
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_64);
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  uint8_t write_buffer[3] = {0, 0, 0};
  uint8_t bytes_written = 0;

  //get proper config value from state structure
  sm2_configs[7] = state->sm_config.dbt_thresh_1;
  sm2_configs[6] = state->sm_config.dbt_thresh_2;
  sm2_configs[5] = (uint8_t)(state->sm_config.dbt_tim1 >> 8);
  sm2_configs[4] = (uint8_t)(state->sm_config.dbt_tim1 & 0x00FF);
  sm2_configs[3] = (uint8_t)(state->sm_config.dbt_tim2 >> 8);
  sm2_configs[2] = (uint8_t)(state->sm_config.dbt_tim2 & 0x00FF);
  sm2_configs[1] = state->sm_config.dbt_tim3;
  sm2_configs[0] = state->sm_config.dbt_tim4;

  //read OUTS1 register to reset the interrupt
  sns_ddf_read_port(
      state->port_handle,
      STM_LIS3DSH_OUTS2,
      &write_buffer[0],
      1,
      &bytes_written);

  //Register configuration for wake-up application
  //set ODR as 400Hz
  write_buffer[0] = 0x77;
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_ODR,
      &write_buffer[0],
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 1)
    return SNS_DDF_EBUS;

  //set Full Scale +-4g, bandwidth 200Hz.
  write_buffer[0] = 0x88;
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_FS,
      &write_buffer[0],
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 1)
    return SNS_DDF_EBUS;

  //set SM2 configuration registers, from 0x70 to 0x7C.
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_TIM4_2,
      sm2_configs,
      NUM_OF_SM_CONFIG,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != NUM_OF_SM_CONFIG)
    return SNS_DDF_EBUS;

  //set SM2 step registers, from 0x60 to 0x6F.
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_STM2_X,
      sm2_steps,
      NUM_OF_SM_STEPS,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != NUM_OF_SM_STEPS)
    return SNS_DDF_EBUS;

  //enable SM2
  /*write_buffer[0] = 0x01;
    status = sns_ddf_write_port(
    state->port_handle,
    STM_LIS3DSH_CTRL_SM2,
    &write_buffer[0],
    1,
    &bytes_written);
    if(status != SNS_DDF_SUCCESS)
    return status;
    if(bytes_written != 1)
    return SNS_DDF_EBUS;*/

  return status;
}



