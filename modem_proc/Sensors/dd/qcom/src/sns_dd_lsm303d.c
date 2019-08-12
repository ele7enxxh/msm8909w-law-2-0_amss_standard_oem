/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
*
* File Name         : sns_dd_lsm303d.c
* Authors           : Samuel(Jianjian) Huo
*                   : Karimuddin sayed 
* Version           : V 2.3.4
* Date              : 10/31/2013
* Description       : LSM303D driver source file
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
*
* 2.3.4   | 07/22/2014    | Eliminated compiler warning.
* 2.3.3   | 03/12/2014    | Fixed KW issues.
* 2.3.2   | 10/31/2013    | Fixed HW self test out of range issue, changed ALIGN_32BITS value.
* 2.3.0   | 10/30/2013    | Fixed state variable compilation issues,
*                           Fixed data unit conversion issues in get_data and HW self test.
* 2.2.0   | 10/29/2013    | Added accel HW and SW self test,
*                           Added log data to report sensor log packet.
* 2.0.0   | 10/21/2013    | Converted driver to DDF combo device driver.
* 0.9.6   | 05/16/2012    | Converted all floating point operations to fixed point.
* 0.9.1   | 05/05/2011    | Used a header file to configure axis and polarity.
* 0.9.0   | 04/27/2011    | Created.
*******************************************************************************/
/*============================================================================
 Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential
 ============================================================================*/

#include "sns_dd_lsm303d.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_signal.h"
//Include files required to support log packet
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_dd_inertial_test.h"
#include <stdbool.h>
#include <string.h>

#define STM_LSM303D_NUM_AXES 3
#define STM_LSM303D_NUM_READ_BYTES 6
#define AUTO_INCREMENT    0x80 //to read multiple bytes MSB should be 1

sns_ddf_driver_if_s sns_dd_lsm303d_if;

/**
 * Accelerometer LSM303D_ACC ranges array for setting attribute
 */
stm_lsm303d_acc_range lsm303d_acc_ranges[STM_LSM303D_ACC_RANGE_NUM]={
            STM_LSM303D_ACC_RANGE_2G,
            STM_LSM303D_ACC_RANGE_4G,
            STM_LSM303D_ACC_RANGE_6G,
            STM_LSM303D_ACC_RANGE_8G,
            STM_LSM303D_ACC_RANGE_16G};

/**
 * Accelerometer LSM303D_ACC sampling frequencies array for getting attribute
 */
sns_ddf_lowpass_freq_t lsm303d_acc_freqs[STM_LSM303D_ACC_ODR_NUM] = {
            FX_FLTTOFIX_Q16(1.5625),
            FX_FLTTOFIX_Q16(3.125),
            FX_FLTTOFIX_Q16(6.25),
            FX_FLTTOFIX_Q16(12.5),
            FX_FLTTOFIX_Q16(25),
            FX_FLTTOFIX_Q16(50),
            FX_FLTTOFIX_Q16(100),
            FX_FLTTOFIX_Q16(200),
            FX_FLTTOFIX_Q16(400),
            FX_FLTTOFIX_Q16(800)
};

/**
 * Accelerometer LSM303D sampling frequencies array for attribute(ODR)
 */
sns_ddf_odr_t lsm303d_acc_odr[STM_LSM303D_ACC_ODR_NUM] = {
           3,
           6,
           12,
           25,
           50,
           100,
           200,
           400,
           800,
           1600
};

/**
 * Accelerometer LSM303D_ACC sampling frequencies array for setting attribute
 */
stm_lsm303d_acc_odr lsm303d_acc_odr_setting[STM_LSM303D_ACC_ODR_NUM] = {
            STM_LSM303D_ACC_ODR3_125,
            STM_LSM303D_ACC_ODR6_25,
            STM_LSM303D_ACC_ODR12_5,
            STM_LSM303D_ACC_ODR25,
            STM_LSM303D_ACC_ODR50,
            STM_LSM303D_ACC_ODR100,
            STM_LSM303D_ACC_ODR200,
            STM_LSM303D_ACC_ODR400,
            STM_LSM303D_ACC_ODR800,
            STM_LSM303D_ACC_ODR1600 
};

/**
 * Magnetometer LSM303D_MAG ranges array for setting attribute
 */
stm_lsm303d_mag_range lsm303d_mag_ranges[STM_LSM303D_MAG_RANGE_NUM]={
            STM_LSM303D_MAG_RANGE_2G,
            STM_LSM303D_MAG_RANGE_4G,
            STM_LSM303D_MAG_RANGE_8G,
            STM_LSM303D_MAG_RANGE_12G
};

/**
 * Magnetometer LSM303D_MAG sampling frequencies array for getting attribute
 */
sns_ddf_lowpass_freq_t lsm303d_mag_freqs[STM_LSM303D_MAG_ODR_NUM] = {
            FX_FLTTOFIX_Q16(1.5625),
            FX_FLTTOFIX_Q16(3.125),
            FX_FLTTOFIX_Q16(6.25),
            FX_FLTTOFIX_Q16(12.5),
            FX_FLTTOFIX_Q16(25),
            FX_FLTTOFIX_Q16(50)
};

/**
 * Accelerometer LSM303D sampling frequencies array for attribute(ODR)
 */
sns_ddf_odr_t lsm303d_mag_odr[STM_LSM303D_MAG_ODR_NUM] = {
           3,
           6,
           12,
           25,
           50,
           100
};
/**
 * Magnetometer LSM303D_MAG sampling frequencies array for setting attribute
 */
stm_lsm303d_mag_odr lsm303d_mag_odr_setting[STM_LSM303D_MAG_ODR_NUM] = {
    STM_LSM303D_MAG_ODR3_125,
    STM_LSM303D_MAG_ODR6_25,
    STM_LSM303D_MAG_ODR12_5,
    STM_LSM303D_MAG_ODR25,
    STM_LSM303D_MAG_ODR50,
    STM_LSM303D_MAG_ODR100,
};

static stm_lsm303d_head_s lsm303d_dd_head = {
  NULL,
  {"STMICRO_LSM303D", 2}
};
static uint8_t stm_lsm303d_axis_dflt[3] = {1, 2, 3};

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
static uint32_t dd_find_free_sub_dev_slot (sns_dd_lsm303d_state_t *my_dev_ptr)
{
  uint32_t i;
  for (i =0; i < LSM303D_MAX_SUB_DEV; i++)
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
static uint32_t dd_fill_sub_dev_slot (stm_lsm303d_sub_dev_s *sub_dev_slot_ptr, uint32_t num_sensors, sns_ddf_sensor_e sensors[])
{
  uint32_t my_num_sensors = 0, i;
  for (i =0; i < num_sensors; i++)
  {
    /*
     * Note: both ACCEL and MAG are Primary Data Types, by definition.
     */
    if ((SNS_DDF_SENSOR_ACCEL==sensors[i])||(SNS_DDF_SENSOR_MAG==sensors[i]))
      /* to check if this device support the sensor type */
    {
      sub_dev_slot_ptr->sensors[my_num_sensors++] = sensors[i];
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
static sns_dd_lsm303d_state_t *dd_find_same_port_dev (sns_ddf_port_config_s *port_config_ptr)
{
  sns_dd_lsm303d_state_t *cur_dd_dev_ptr = (sns_dd_lsm303d_state_t *)lsm303d_dd_head.next;

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
 * @brief Update only one part of one register in LSM303D
 *
 * @param[in]  dd_handle   Handle to a driver instance.
 * @param[in]  reg_addr    The address of LSM303D register to be updated
 * @param[in]  mask        The mask of this register to specify which part to be updated
 * @param[in]  new_value   The new content for the specified part of this register
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_lsm303d_update_onereg(
    sns_ddf_handle_t dd_handle,
    uint8_t reg_addr,
    uint8_t mask,
    uint8_t new_value)
{
  sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
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
 * @brief Find the matched internal acc ODR for desired ODR.

 *
 * @param[in]  desired_rate      New desired ODR.
 * @param[out] new_rate          Matched internal ODR.
 * @param[out] new_index         The index of matched internal ODR in lsm303d_odr.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e  sns_dd_acc_lsm303d_match_odr(
    sns_ddf_odr_t desired_rate,
    sns_ddf_odr_t *new_rate,
    uint32_t *new_index)
{
  uint8_t idx;

  if((0 == desired_rate) || (desired_rate > STM_LSM303D_ACC_MAX_ODR))
    return SNS_DDF_EINVALID_PARAM;

  for(idx=0; idx<STM_LSM303D_ACC_ODR_NUM; idx++)
  {
    if(desired_rate <= lsm303d_acc_odr[idx])
      break;
  }

  if(idx >= STM_LSM303D_ACC_ODR_NUM)
  {
    // if desired_rate > max supported ODR then use
    // max supported ODR
    idx = (STM_LSM303D_ACC_ODR_NUM-1);
  }

  *new_rate = lsm303d_acc_odr[idx];
  *new_index = idx;

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Find the matched internal mag ODR for desired ODR.

 *
 * @param[in]  desired_rate      New desired ODR.
 * @param[out] new_rate          Matched internal ODR.
 * @param[out] new_index         The index of matched internal ODR in lsm303d_odr.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e  sns_dd_mag_lsm303d_match_odr(
    sns_ddf_odr_t desired_rate,
    sns_ddf_odr_t *new_rate,
    uint32_t *new_index)
{
  uint8_t idx;

  if((0 == desired_rate) || (desired_rate > STM_LSM303D_MAG_MAX_ODR))
    return SNS_DDF_EINVALID_PARAM;

  for(idx=0; idx<STM_LSM303D_MAG_ODR_NUM; idx++)
  {
    if(desired_rate <= lsm303d_mag_odr[idx])
      break;
  }

  if(idx >= STM_LSM303D_MAG_ODR_NUM)
  {
    // if desired_rate > max supported ODR then use
    // max supported ODR
    idx = (STM_LSM303D_MAG_ODR_NUM-1);
  }

  *new_rate = lsm303d_mag_odr[idx];
  *new_index = idx;

  return SNS_DDF_SUCCESS;
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
static sns_ddf_status_e lsm303d_search_subdev_by_sensortype (
    sns_dd_lsm303d_state_t* state,
    sns_ddf_sensor_e sensor,
    uint32_t *sub_dev_inx)
{
  uint32_t i, j;
  stm_lsm303d_sub_dev_s *sub_dev_slot_ptr;

  for (i =0; i < LSM303D_MAX_SUB_DEV; i++)
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
 * @brief Check ZYXDA in STATUS register, return data ready flag.
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
static sns_ddf_status_e sns_dd_acc_lsm303d_is_dataready(
    sns_ddf_handle_t dd_handle,
    bool *data_flag)
{
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;

    // read status register
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LSM303D_REG_STATUS_A,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    if((rw_buffer&0x08) > 0)
        *data_flag = true;
    else
        *data_flag = false;
    
    return SNS_DDF_SUCCESS;
}

 /**
 * @brief Collects sample data for lsm303d self test.
 *
 * @param[in]  dd_handle     Handle to a driver instance.
 * @param[out] data           Output data buffer to be filled in, should be set 0 before called.
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lsm303d_collect_testdata(
    sns_ddf_handle_t dd_handle,
    int16_t *data)
{
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    sns_ddf_status_e status;
    uint8_t rw_buffer[STM_LSM303D_NUM_READ_BYTES] = {0, 0, 0};
    int32_t average_data[STM_LSM303D_NUM_AXES] = {0, 0, 0};
    uint8_t rw_bytes = 0;
    bool data_rdy = false;
    uint8_t idx_i;

    //Collect data
    for(idx_i=0;idx_i<6;idx_i++)
    {
        do{
            status = sns_dd_acc_lsm303d_is_dataready(dd_handle,&data_rdy);
            if(status != SNS_DDF_SUCCESS)
                return status;
        }while(data_rdy == false);

        status = sns_ddf_read_port(
            state->port_handle,
            AUTO_INCREMENT | STM_LSM303D_REG_OUT_X_L_A,
            rw_buffer,
            STM_LSM303D_NUM_READ_BYTES,
            &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(rw_bytes != STM_LSM303D_NUM_READ_BYTES)
            return SNS_DDF_EBUS;    
      
        //discard the very first one.
        if(0==idx_i)
            continue;

        average_data[0] += (int16_t) (((rw_buffer[1]) << 8) | rw_buffer[0]);
        average_data[1] += (int16_t) (((rw_buffer[3]) << 8) | rw_buffer[2]);
        average_data[2] += (int16_t) (((rw_buffer[5]) << 8) | rw_buffer[4]);
    }
    
    for(idx_i=0;idx_i<STM_LSM303D_NUM_AXES;idx_i++)
        data[idx_i] = average_data[idx_i] / 5; 

    return SNS_DDF_SUCCESS;
}

/** 
  * @brief Resets the driver and device so they return to the state they were 
  *        in after init() was called.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm303d_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    sns_ddf_status_e status;
    uint8_t rw_buffer[3];
    uint8_t rw_bytes = 0;
    uint32_t sub_dev_idx;
    stm_lsm303d_sub_dev_s *sub_dev_slot_ptr;

    sub_dev_idx = (uint32_t)dd_handle & (~(uint32_t)DD_HANDLE_ALIGN_BITS);
    if(sub_dev_idx >= LSM303D_MAX_SUB_DEV)
    {
      return SNS_DDF_EINVALID_PARAM;
    }
    sub_dev_slot_ptr = &(state->sub_dev[sub_dev_idx]);

    if(SNS_DDF_SENSOR_ACCEL == sub_dev_slot_ptr->sensors[0])
    {
      // Configure accelerometer control registers
      rw_buffer[0] = 0x0
               | (0<<7)           // ODR3 bit
               | (0<<6)           // ODR2 bit
               | (0<<5)           // ODR1 bit
               | (0<<4)           // ODR0 bit
               | (1<<3)           // BDU bit
               | (1<<2)           // Z axis enable bit
               | (1<<1)           // Y axis enable bit
               | 1;               // X axis enable bit
      status = sns_ddf_write_port(
          state->port_handle,
          STM_LSM303D_REG_CNTRL1,
          rw_buffer,
          1,
          &rw_bytes);
      if(status != SNS_DDF_SUCCESS)
          return status;
      if(rw_bytes != 1)
          return SNS_DDF_EBUS;    
          
      rw_buffer[0] = 0x0
               | (0<<7)           // BW2 bit
               | (0<<6)           // BW1 bit
               | (0<<5)           // FSCALE2 bit
               | (0<<4)           // FSCALE1 bit
               | (0<<3)           // FSCALE0 bit
               | (0<<2)           // 0
               | (0<<1)           // AST bit selftest
               | 0;               // SIM bit
      status = sns_ddf_write_port(
          state->port_handle,
          STM_LSM303D_REG_CNTRL2,
          rw_buffer,
          1,
          &rw_bytes);
      if(status != SNS_DDF_SUCCESS)
          return status;
      if(rw_bytes != 1)
          return SNS_DDF_EBUS;                 

      state->acc_range = STM_LSM303D_ACC_RANGE_2G;
      state->acc_sstvt_adj = STM_LSM303D_ACC_SSTVT_2G;
      state->acc_cur_rate = 0;
      state->acc_desired_rate = 0;
    }

    if(SNS_DDF_SENSOR_MAG == sub_dev_slot_ptr->sensors[0])
    {
        //Configure magnetometer control register CTRL5
        rw_buffer[0] = 0x0 
        | (0<<7)         // Disable Temp sensor TEMP_EN bit
        | (1<<6)         // M_RES1 bit
        | (1<<5)         // M_RES0 bit 
        | (0<<4)         // ODR2 bit
        | (1<<3)         // ODR1 bit
        | (1<<2)         // ODR0 bit 25Hz 
        | (0<<1)         // LIR1 bit
        | 0;             // LIR0 bit

        // Configure magnetometer control register CTRL6
        rw_buffer[1] = 0x0 
        | (0<<7)         // must be 0 
        | (1<<6)         // MFS1
        | (0<<5)         // MFS0 bit  8 Gauss
        | 0;

        // Configure acc and mag control register CTRL7 
        rw_buffer[2] = 0x0 
        | (0<<7)         // AHPM1 bit
        | (0<<6)         // AHPM0 bit acc Highpass filter - Normal mode
        | (0<<5)         // AFDS bit filtered acc data - internel filter bypassed
        | (0<<4)         // T_ONLY 
        | (0<<3)         // must be 0
        | (0<<2)         // mag lowpower mode
        | (1<<1)         // MD1 bit
        | 0;             // MD0 bit  Mode: sleep mode.

        status = sns_ddf_write_port(
                state->port_handle,
                AUTO_INCREMENT | STM_LSM303D_REG_CNTRL5,
                rw_buffer,
                3,  
                &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(rw_bytes!= 3)
            return SNS_DDF_EBUS;

        state->mag_range = STM_LSM303D_MAG_RANGE_8G;
        state->mag_sstvt_adj = STM_LSM303D_MAG_SSTVT_8G;
        state->mag_cur_rate = 0;
        state->mag_desired_rate = 0;
    }
   
    return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set accelerometer data sampling rate operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  data_rate       Data sampling rate settings for LSM303D
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lsm303d_set_datarate(
  sns_ddf_handle_t dd_handle, 
  stm_lsm303d_acc_odr data_rate)
{
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;
    
    // Configure ODR in CTRL_ODR register
    write_buffer = ((uint8_t)data_rate | 0x0F); 
    status = sns_ddf_write_port(
            state->port_handle,
            STM_LSM303D_REG_CNTRL1,
            &write_buffer,
            1,
            &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    // select proper bandwith for this ODR.
    if(data_rate < STM_LSM303D_ACC_ODR100)
      write_buffer = STM_LSM303D_ACC_BW50;
    else if(data_rate <= STM_LSM303D_ACC_ODR400)
      write_buffer = STM_LSM303D_ACC_BW200;
    else if(data_rate <= STM_LSM303D_ACC_ODR800)
      write_buffer = STM_LSM303D_ACC_BW400;
    else
      write_buffer = STM_LSM303D_ACC_BW800;

    // Configure Bandwidth along with ODR.
    status = sns_dd_lsm303d_update_onereg(
        dd_handle,
        STM_LSM303D_REG_CNTRL2,
        0xC0,
        write_buffer);
    if(status != SNS_DDF_SUCCESS)
      return status;

    // wait 10 ms.
    sns_ddf_delay(10000);

    return SNS_DDF_SUCCESS;
}
/**
 * @brief Performs a set magnetometer data sampling rate operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  data_rate       Data sampling rate settings for LSM303D
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_mag_lsm303d_set_datarate(
  sns_ddf_handle_t dd_handle, 
  stm_lsm303d_mag_odr data_rate)
{
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;
    uint8_t read_buffer = 0;
    uint8_t bytes_read = 0;
    
    //read CTRL5 register and mask the bits except ODR
    status = sns_ddf_read_port(
            state->port_handle,
            STM_LSM303D_REG_CNTRL5,
            &read_buffer,
            1,
            &bytes_read);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_read != 1)
        return SNS_DDF_EBUS;

    // Configure ODR in CTRL_ODR register
    write_buffer = (read_buffer & 0xE3)| data_rate; 
    status = sns_ddf_write_port(
            state->port_handle,
            STM_LSM303D_REG_CNTRL5,
            &write_buffer,
            1,
            &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    // wait 10 ms.
    sns_ddf_delay(10000);

    return SNS_DDF_SUCCESS;
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
static sns_ddf_status_e sns_dd_acc_lsm303d_set_power(
  sns_ddf_handle_t dd_handle,
  uint32_t sub_dev_idx,
  sns_ddf_powerstate_e power_state)
{
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;

    if(SNS_DDF_POWERSTATE_LOWPOWER == power_state)
    {
        // Configure ODR of control register 1 to enter power down mode.
        write_buffer = 0x0
                    | (0<<7)             // ODR3 bit
                    | (0<<6)             // ODR2 bit
                    | (0<<5)             // ODR1 bit
                    | (0<<4)             // ODR0 bit
                    | (0<<3)             // BDU bit
                    | (0<<2)             // Z axis enable bit
                    | (0<<1)             // Y axis enable bit
                    | 0;                 // X axis enable bit  

        status = sns_ddf_write_port(
            state->port_handle,
            STM_LSM303D_REG_CNTRL1,
            &write_buffer,
            1,
            &bytes_written);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(bytes_written != 1)
            return SNS_DDF_EBUS;

        state->acc_cur_rate = 0;
        state->acc_desired_rate = 0;
        state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    }
    else if(SNS_DDF_POWERSTATE_ACTIVE == power_state)
    {
        // call sns_dd_acc_lsm303d_set_datarate to enter ACTIVE mode 
        status = sns_dd_acc_lsm303d_set_datarate(dd_handle, 50);
        if(status != SNS_DDF_SUCCESS)
        {
            return status;
        }

        state->acc_cur_rate = 50;
        state->acc_desired_rate = 0;
        state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_ACTIVE;
    }

    return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set magnetometer power state operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  power_state    Power state to be set.
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_mag_lsm303d_set_power(
  sns_ddf_handle_t dd_handle,
  uint32_t sub_dev_idx,
  sns_ddf_powerstate_e power_state)
{
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;

    if(SNS_DDF_POWERSTATE_LOWPOWER == power_state)
    {
        // Configure MD0 and MD1 of control register 7 to enter power down mode.
        write_buffer = 0x0
                    | (1<<1)             // MD1 bit
                    | 0;                 // MD0 bit

        status = sns_ddf_write_port(
            state->port_handle,
            STM_LSM303D_REG_CNTRL7,
            &write_buffer,
            1,
            &bytes_written);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(bytes_written != 1)
            return SNS_DDF_EBUS;

        state->mag_cur_rate = 0;
        state->mag_desired_rate = 0;
        state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    }
    else if(SNS_DDF_POWERSTATE_ACTIVE == power_state)
    {
        // Configure MD0 and MD1 of control register 7 to enter continuous conversion mode.
        write_buffer = 0x0
                    | (0<<1)             // MD1 bit
                    | 0;                 // MD0 bit

        status = sns_ddf_write_port(
            state->port_handle,
            STM_LSM303D_REG_CNTRL7,
            &write_buffer,
            1,
            &bytes_written);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(bytes_written != 1)
            return SNS_DDF_EBUS;

        status = sns_dd_mag_lsm303d_set_datarate(dd_handle, 12);
        if(status != SNS_DDF_SUCCESS)
        {
            return status;
        }
        state->mag_cur_rate = 12;
        state->mag_desired_rate = 0;
        state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_ACTIVE;
    }

    return SNS_DDF_SUCCESS;
}
/**
 * @brief Performs a set dynamic range operation for accelerometer
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  range          Dynamic range settings for LSM303D
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lsm303d_set_range(
  sns_ddf_handle_t dd_handle, 
  stm_lsm303d_acc_range range)
{
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;
    stm_lsm303d_acc_sstvt sstvt;

    //set sensor sensitivity for data conversion
    switch (range) {
        case STM_LSM303D_ACC_RANGE_2G:
            sstvt = STM_LSM303D_ACC_SSTVT_2G;
            break;
        case STM_LSM303D_ACC_RANGE_4G:
            sstvt = STM_LSM303D_ACC_SSTVT_4G;
            break;
        case STM_LSM303D_ACC_RANGE_6G:
            sstvt = STM_LSM303D_ACC_SSTVT_6G;
            break;			
        case STM_LSM303D_ACC_RANGE_8G:
            sstvt = STM_LSM303D_ACC_SSTVT_8G;
            break;
        case STM_LSM303D_ACC_RANGE_16G:
            sstvt = STM_LSM303D_ACC_SSTVT_16G;
            break;
        default:
            return SNS_DDF_EINVALID_PARAM;
    }
    
    // Configure CTRL_FS register
    write_buffer = ((uint8_t)range)|0x00; 
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LSM303D_REG_CNTRL2,
        &write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    state->acc_range = range;
    state->acc_sstvt_adj = sstvt;

  
    return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set dynamic range operation for magnetometer 
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  range          Dynamic range settings for LSM303D
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_mag_lsm303d_set_range(
  sns_ddf_handle_t dd_handle, 
  stm_lsm303d_mag_range range)
{
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;
    stm_lsm303d_mag_sstvt sstvt;

    //set sensor sensitivity for data conversion
    switch (range) {
        case STM_LSM303D_MAG_RANGE_2G:
            sstvt = STM_LSM303D_MAG_SSTVT_2G;
            break;
        case STM_LSM303D_MAG_RANGE_4G:
            sstvt = STM_LSM303D_MAG_SSTVT_4G;
            break;
        case STM_LSM303D_MAG_RANGE_8G:
            sstvt = STM_LSM303D_MAG_SSTVT_8G;
            break;
        case STM_LSM303D_MAG_RANGE_12G:
            sstvt = STM_LSM303D_MAG_SSTVT_12G;
            break;
        default:
            return SNS_DDF_EINVALID_PARAM;
    }
    
    // Configure CTRL_FS register
    write_buffer = ((uint8_t)range)|0x00; 
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LSM303D_REG_CNTRL6,
        &write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    state->mag_range = range;
    state->mag_sstvt_adj = sstvt;

  
    return SNS_DDF_SUCCESS;
}
  
/*===========================================================================
  FUNCTION:   sns_dd_acc_lsm303daccel_log_data
===========================================================================*/
/*!
  @brief log accel sensor data
 
  @detail
  - Logs latest set of sensor data sampled from the sensor. 
 
  @param[in] accel_data_ptr: ptr to the driver data
*/
/*=========================================================================*/
void sns_dd_lsm303d_log_data(sns_ddf_sensor_data_s *accel_data_ptr)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;
  
  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                               sizeof(sns_log_sensor_data_pkt_s),
                               (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = accel_data_ptr->sensor;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_STMICRO;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = accel_data_ptr->timestamp;

    //Log the sensor data
    log_struct_ptr->num_data_types = STM_LSM303D_NUM_AXES;
    log_struct_ptr->data[0]  = accel_data_ptr->samples[0].sample;
    log_struct_ptr->data[1]  = accel_data_ptr->samples[1].sample;
    log_struct_ptr->data[2]  = accel_data_ptr->samples[2].sample;

    //Commit log (also frees up the log packet memory)
    (void) sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, 
                             log_struct_ptr);
  }
}  


/** 
 * @brief Retrieves a single set of sensor data from lsm303d.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_lsm303d_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data)
{
    uint8_t read_buffer[STM_LSM303D_NUM_READ_BYTES] = { 0 };
    uint8_t read_count = 0;
    sns_ddf_time_t timestamp;
    sns_ddf_status_e status;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    // sensor coordinate x,y,z axis raw hardware data
    int16_t hw_d[STM_LSM303D_NUM_AXES] = { 0 };
    // SAE coordinate x,y,z axis data
    q16_t sae_d[STM_LSM303D_NUM_AXES] = { 0 };
    int i;

    //allocate memory for sns_ddf_sensor_data_s data structure
    data_ptr = sns_ddf_memhandler_malloc(memhandler, num_sensors * sizeof(sns_ddf_sensor_data_s));
    if(NULL == data_ptr)
    {
        return SNS_DDF_ENOMEM;
    }
    *data = data_ptr;

    for(i = 0; i < num_sensors; i++) 
    {
        switch(sensors[i])
        {
            case SNS_DDF_SENSOR_ACCEL :
                // This is a synchronous driver, so try to read data now.
                status = sns_ddf_read_port(
                        state->port_handle,
                        AUTO_INCREMENT | STM_LSM303D_REG_OUT_X_L_A,
                        read_buffer,
                        STM_LSM303D_NUM_READ_BYTES,
                        &read_count);
                if(status != SNS_DDF_SUCCESS)
                    return status;
                if(read_count != STM_LSM303D_NUM_READ_BYTES)
                    return SNS_DDF_EBUS;

                break;
            case SNS_DDF_SENSOR_MAG:
                // This is a synchronous driver, so try to read data now.
                status = sns_ddf_read_port(
                        state->port_handle,
                        AUTO_INCREMENT | STM_LSM303D_REG_OUT_X_L_M,
                        read_buffer,
                        STM_LSM303D_NUM_READ_BYTES,
                        &read_count);
                if(status != SNS_DDF_SUCCESS)
                    return status;
                if(read_count != STM_LSM303D_NUM_READ_BYTES)
                    return SNS_DDF_EBUS;    
                break;
	        default:
	    	    return SNS_DDF_EBUS;
        }
        // get current time stamp
        timestamp = sns_ddf_get_timestamp();

        //convert the raw data in read_buffer to X/Y/Z axis sensor data
        hw_d[0] = (int16_t) (((read_buffer[1]) << 8) | read_buffer[0]);
        hw_d[1] = (int16_t) (((read_buffer[3]) << 8) | read_buffer[2]);
        hw_d[2] = (int16_t) (((read_buffer[5]) << 8) | read_buffer[4]);

        if (sensors[i] == SNS_DDF_SENSOR_ACCEL) 
        {
          //adjust sensor data per current sensitivity, and convert to Q16
          sae_d[0] = FX_FLTTOFIX_Q16(hw_d[0] * state->acc_sstvt_adj * G/1000000);
          sae_d[1] = FX_FLTTOFIX_Q16(hw_d[1] * state->acc_sstvt_adj * G/1000000);
          sae_d[2] = FX_FLTTOFIX_Q16(hw_d[2] * state->acc_sstvt_adj * G/1000000);
        }
        else if (sensors[i] == SNS_DDF_SENSOR_MAG) 
        {
          //adjust sensor data per current sensitivity, and convert to Q16
          sae_d[0] = FX_MUL_Q16(FX_DIV_Q16(FX_CONV_Q16((int32_t)(hw_d[0]), 0), FX_CONV_Q16(1000,0)),
			                    FX_DIV_Q16(FX_CONV_Q16((int32_t)(state->mag_sstvt_adj), 0), FX_CONV_Q16(1000,0)));
          sae_d[1] = FX_MUL_Q16(FX_DIV_Q16(FX_CONV_Q16((int32_t)(hw_d[1]), 0), FX_CONV_Q16(1000,0)),
			                    FX_DIV_Q16(FX_CONV_Q16((int32_t)(state->mag_sstvt_adj), 0), FX_CONV_Q16(1000,0)));
          sae_d[2] = FX_MUL_Q16(FX_DIV_Q16(FX_CONV_Q16((int32_t)(hw_d[2]), 0), FX_CONV_Q16(1000,0)),
			                    FX_DIV_Q16(FX_CONV_Q16((int32_t)(state->mag_sstvt_adj), 0), FX_CONV_Q16(1000,0)));
        }

        //map the sensor data to the phone's coordinates
        sns_ddf_map_axes(&state->axes_map, sae_d);

        data_ptr[i].sensor = sensors[i];
        data_ptr[i].status = SNS_DDF_SUCCESS;
        data_ptr[i].timestamp = timestamp;

        //allocate memory for data samples.
        data_ptr[i].samples = sns_ddf_memhandler_malloc(memhandler, STM_LSM303D_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
        if(NULL == data_ptr[i].samples)
        {
            return SNS_DDF_ENOMEM;
        }

        //Fill out sensor output data and status 
        //Axis and polarity conversion are configured in the header file.
        data_ptr[i].samples[0].sample = sae_d[0];
        data_ptr[i].samples[0].status = SNS_DDF_SUCCESS;
        data_ptr[i].samples[1].sample = sae_d[1];
        data_ptr[i].samples[1].status = SNS_DDF_SUCCESS;
        data_ptr[i].samples[2].sample = sae_d[2];
        data_ptr[i].samples[2].status = SNS_DDF_SUCCESS;
        data_ptr[i].num_samples = STM_LSM303D_NUM_AXES;

        sns_dd_lsm303d_log_data(data_ptr + i);
    }

    return SNS_DDF_SUCCESS;
}


/** 
 * @brief Sets a lsm303d sensor attribute to a specific value.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_lsm303d_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value)
{
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    sns_ddf_status_e status;
    uint32_t sub_dev_idx;

    if (dd_handle == NULL || value == NULL)
      return SNS_DDF_EINVALID_PARAM;

    sub_dev_idx = (uint32_t)dd_handle & (~(uint32_t)DD_HANDLE_ALIGN_BITS);

    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_STATE:
        {
            sns_ddf_powerstate_e* power_state = value;
            return (sensor == SNS_DDF_SENSOR_ACCEL) ? 
                sns_dd_acc_lsm303d_set_power(dd_handle, sub_dev_idx, *power_state) : 
                sns_dd_mag_lsm303d_set_power(dd_handle, sub_dev_idx, *power_state);
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            uint32_t* range_idx = value;
            
            return (sensor == SNS_DDF_SENSOR_ACCEL) ? 
                sns_dd_acc_lsm303d_set_range(dd_handle, lsm303d_acc_ranges[*range_idx]):
                sns_dd_mag_lsm303d_set_range(dd_handle, lsm303d_mag_ranges[*range_idx]);
        }

        case SNS_DDF_ATTRIB_ODR:
        {
            sns_ddf_odr_t new_rate;
            uint32_t new_rate_index;

            if(SNS_DDF_SENSOR_ACCEL == sensor)
            {
              if (*(sns_ddf_odr_t*)value > 0)
              {
                //find the matched internal accel ODR for desired ODR
                status =  sns_dd_acc_lsm303d_match_odr(*(sns_ddf_odr_t*)value,
                    &new_rate,
                    &new_rate_index);
                if(SNS_DDF_SUCCESS != status)
                  return SNS_DDF_EINVALID_PARAM;

                if(new_rate != state->acc_cur_rate) 
                {
                  LSM303D_MSG_3(MED, "LSM303D set_acc_datarate(%d), new_rate(%d), new_rate_index(%d)", *(sns_ddf_odr_t*)value, new_rate, new_rate_index);

                  if((100 > state->mag_desired_rate) || (100 == state->mag_desired_rate && new_rate >=100))
                  {
                    status = sns_dd_acc_lsm303d_set_datarate(state,
                        lsm303d_acc_odr_setting[new_rate_index]);
                    if(SNS_DDF_SUCCESS == status)
                    {
                      state->acc_cur_rate = new_rate;
                      state->acc_desired_rate= *(sns_ddf_odr_t*)value;
                      return status;
                    }
                    else
                    {
                      return status;
                    }
                  }else if(100 == state->mag_desired_rate && new_rate < 100 && 100!=state->acc_cur_rate)
                  {
                    new_rate_index = 5; //set as 100Hz
                    new_rate = 100;
                    status = sns_dd_acc_lsm303d_set_datarate(state,
                        lsm303d_acc_odr_setting[new_rate_index]);
                    if(SNS_DDF_SUCCESS == status)
                    {
                      state->acc_cur_rate = new_rate;
                      state->acc_desired_rate= *(sns_ddf_odr_t*)value;
                      return status;
                    }
                    else
                    {
                      return status;
                    }
                  }
                }
                else
                {
                  LSM303D_MSG_3(MED, "LSM303D  -- else --set_acc_datarate(%d), new_rate(%d), new_rate_index(%d)", *(sns_ddf_odr_t*)value, new_rate, new_rate_index);

                  state->acc_desired_rate = *(sns_ddf_odr_t*)value;
                  return SNS_DDF_SUCCESS;
                }
              }
              else
              {
                //turn off accel
                sns_dd_acc_lsm303d_set_power(dd_handle, sub_dev_idx, SNS_DDF_POWERSTATE_LOWPOWER);
                state->acc_cur_rate = *(sns_ddf_odr_t*)value;
                state->acc_desired_rate = *(sns_ddf_odr_t*)value;
              }
            }
            else if(SNS_DDF_SENSOR_MAG == sensor)
            {
              if (*(sns_ddf_odr_t*)value > 0)
              {
                //find the matched internal accel ODR for desired ODR
                status =  sns_dd_mag_lsm303d_match_odr(*(sns_ddf_odr_t*)value,
                    &new_rate,
                    &new_rate_index);
                if(SNS_DDF_SUCCESS != status)
                  return SNS_DDF_EINVALID_PARAM;

                if(new_rate != state->mag_cur_rate) 
                {
                  LSM303D_MSG_3(MED, "LSM303D set_mag_datarate(%d), new_rate(%d), new_rate_index(%d)", *(sns_ddf_odr_t*)value, new_rate, new_rate_index);

                  //reset accel ODR to 100Hz if needed. No need to notify SMGR, since DD is in polling mode.
                  if(100 == new_rate && state->acc_cur_rate < 100 && state->acc_cur_rate > 0)
                  {
                    new_rate_index = 5; //set accel as 100Hz
                    new_rate = 100;
                    status = sns_dd_acc_lsm303d_set_datarate(state,
                        lsm303d_acc_odr_setting[new_rate_index]);
                    if(SNS_DDF_SUCCESS == status)
                    {
                      state->acc_cur_rate = new_rate;
                      state->acc_desired_rate= *(sns_ddf_odr_t*)value;
                      return status;
                    }
                    else
                    {
                      return status;
                    }
                  }

                  //set mag ODR to new_rate
                  status = sns_dd_mag_lsm303d_set_datarate(state,
                      lsm303d_mag_odr_setting[new_rate_index]);
                  if(SNS_DDF_SUCCESS == status)
                  {
                    state->mag_cur_rate = new_rate;
                    state->mag_desired_rate= *(sns_ddf_odr_t*)value;
                    return status;
                  }
                  else
                  {
                    return status;
                  }
                }
                else
                {
                  LSM303D_MSG_3(MED, "LSM303D  -- else --set_acc_datarate(%d), new_rate(%d), new_rate_index(%d)", *(sns_ddf_odr_t*)value, new_rate, new_rate_index);

                  state->mag_desired_rate = *(sns_ddf_odr_t*)value;
                  return SNS_DDF_SUCCESS;
                }
              }
              else
              {
                //turn off mag
                sns_dd_mag_lsm303d_set_power(dd_handle, sub_dev_idx, SNS_DDF_POWERSTATE_LOWPOWER);
                state->mag_cur_rate = *(sns_ddf_odr_t*)value;
                state->mag_desired_rate = *(sns_ddf_odr_t*)value;
              }

            }
            else
            {
              return SNS_DDF_EINVALID_PARAM;
            }
            break;
        }

        default:
            return SNS_DDF_EINVALID_ATTR;
    }

    return SNS_DDF_SUCCESS;
}

/** 
 * @brief Retrieves the value of an attribute for a lsm303d sensor.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm303d_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems)
{
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    /*uint32_t sub_dev_idx;*/

    if (dd_handle == NULL || value == NULL)
      return SNS_DDF_EINVALID_PARAM;

    /*sub_dev_idx = (uint32_t)dd_handle & (~(uint32_t)DD_HANDLE_ALIGN_BITS);*/
    
    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_INFO:
        {
            sns_ddf_power_info_s* power = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_power_info_s));
            if(NULL == power)
                return SNS_DDF_ENOMEM;
            //current consumption, unit uA
            power->active_current = 300;
            power->lowpower_current = 1;
            *value = power;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s* ranges = NULL;
            if(sensor == SNS_DDF_SENSOR_ACCEL)
            {
                ranges = sns_ddf_memhandler_malloc(
                        memhandler, STM_LSM303D_ACC_RANGE_NUM * sizeof(sns_ddf_range_s));
                if(NULL == ranges)
                    return SNS_DDF_ENOMEM;
                ranges[0].min = STM_LSM303D_ACC_RANGE_2G_MIN;
                ranges[0].max = STM_LSM303D_ACC_RANGE_2G_MAX;
                ranges[1].min = STM_LSM303D_ACC_RANGE_4G_MIN;
                ranges[1].max = STM_LSM303D_ACC_RANGE_4G_MAX;
                ranges[2].min = STM_LSM303D_ACC_RANGE_6G_MIN;
                ranges[2].max = STM_LSM303D_ACC_RANGE_6G_MAX;			
                ranges[3].min = STM_LSM303D_ACC_RANGE_8G_MIN;
                ranges[3].max = STM_LSM303D_ACC_RANGE_8G_MAX;
                ranges[4].min = STM_LSM303D_ACC_RANGE_16G_MIN;
                ranges[4].max = STM_LSM303D_ACC_RANGE_16G_MAX;
                *num_elems = STM_LSM303D_ACC_RANGE_NUM;
            }
            else
            {
                ranges = sns_ddf_memhandler_malloc(
                        memhandler, STM_LSM303D_MAG_RANGE_NUM * sizeof(sns_ddf_range_s));
                if(NULL == ranges)
                    return SNS_DDF_ENOMEM;

                ranges[0].min = STM_LSM303D_MAG_RANGE_2G_MIN;
                ranges[0].max = STM_LSM303D_MAG_RANGE_2G_MAX;
                ranges[1].min = STM_LSM303D_MAG_RANGE_4G_MIN;
                ranges[1].max = STM_LSM303D_MAG_RANGE_4G_MAX;
                ranges[2].min = STM_LSM303D_MAG_RANGE_8G_MIN;
                ranges[2].max = STM_LSM303D_MAG_RANGE_8G_MAX;
                ranges[3].min = STM_LSM303D_MAG_RANGE_12G_MIN;
                ranges[3].max = STM_LSM303D_MAG_RANGE_12G_MAX;
                *num_elems = STM_LSM303D_MAG_RANGE_NUM;
            }
            *value = ranges;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t *res = sns_ddf_memhandler_malloc(
                memhandler ,sizeof(sns_ddf_resolution_t));
            if(NULL == res)
                return SNS_DDF_ENOMEM;

            if(sensor == SNS_DDF_SENSOR_ACCEL)
                *res = FX_FLTTOFIX_Q16(state->acc_sstvt_adj * G / 1000000);
            else
                *res = FX_DIV_Q16(FX_CONV_Q16((int32_t)1,0),FX_CONV_Q16((int32_t)(state->mag_sstvt_adj),0));

            *value = res;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            sns_ddf_resolution_adc_s *res = sns_ddf_memhandler_malloc(
                memhandler ,sizeof(sns_ddf_resolution_adc_s));
            if(NULL == res)
                return SNS_DDF_ENOMEM;
            
            if(sensor == SNS_DDF_SENSOR_ACCEL)
            {
                res->bit_len = 16;
                res->max_freq = 200;
            }
            else
            {
                res->bit_len = 16;
                res->max_freq = 100;
            }
            *value = res;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            if(sensor == SNS_DDF_SENSOR_ACCEL)
            {
              *value = lsm303d_acc_freqs;
              *num_elems = STM_LSM303D_ACC_ODR_NUM;
            }
            else
            {
              *value = lsm303d_mag_freqs;
              *num_elems = STM_LSM303D_MAG_ODR_NUM;
            }
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_ODR:
        {
          // for both
          sns_ddf_odr_t *res = sns_ddf_memhandler_malloc(
              memhandler ,sizeof(sns_ddf_odr_t));
          if(NULL == res)
            return SNS_DDF_ENOMEM;

          //always return the current ODR to SMGR.
          if(SNS_DDF_SENSOR_ACCEL == sensor)
            *res = state->acc_cur_rate;
          else if(SNS_DDF_SENSOR_MAG == sensor)
            *res = state->mag_cur_rate;
          else
            return SNS_DDF_EINVALID_PARAM;

          *value = res;
          *num_elems = 1;

          return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s *lsm303d_delays = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_delays_s));
            if(NULL == lsm303d_delays)
                return SNS_DDF_ENOMEM;

            if(sensor == SNS_DDF_SENSOR_ACCEL)
            {
                lsm303d_delays->time_to_active = 1;
                lsm303d_delays->time_to_data = 1;
            }
            else
            {
                lsm303d_delays->time_to_active = 5;
                lsm303d_delays->time_to_data = 1;
            }
            *value = lsm303d_delays;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s *info = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_driver_info_s));
            if(NULL == info)
                return SNS_DDF_ENOMEM;

            info->name = "STMicroelectronics LSM303D Driver";
            info->version = 1;
            *value = info;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DEVICE_INFO:
        {
            sns_ddf_device_info_s *info = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_device_info_s));
            if(NULL == info)
                return SNS_DDF_ENOMEM;
            
            
            if(sensor == SNS_DDF_SENSOR_ACCEL)
                info->name = "Accelerometer";
            else
                info->name = "Magnetometer";

            info->vendor = "STMicroelectronics";
            info->model = "LSM303D";
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
 * @brief Initializes the lsm303d driver and sets up sensor. 
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm303d_init(
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
  sns_dd_lsm303d_state_t* state;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;
  uint32_t my_num_sensors = 0;

  /* check if this init() is a call for an existing device */
  state = dd_find_same_port_dev(&device_info->port_config);

  if ( NULL == state)
  {
    /* Allocate a device instance and initialize it */
    status = sns_ddf_malloc((void**)&state, sizeof(sns_dd_lsm303d_state_t));
    if(status != SNS_DDF_SUCCESS)
      return SNS_DDF_ENOMEM;
    memset(state, 0, sizeof(sns_dd_lsm303d_state_t));

    /* link this dev instance to the front of the list */
    state->next = lsm303d_dd_head.next;
    lsm303d_dd_head.next = state;
    sns_ddf_get_port_id(&device_info->port_config,
        &state->port_id);

    /* this init() call is for first sub_dev */
    my_num_sensors = dd_fill_sub_dev_slot(&state->sub_dev[0],
        *num_sensors,
        *sensors);
    *num_sensors = my_num_sensors;
    *sensors = state->sub_dev[0].sensors;
    if ( 0 == my_num_sensors )
      return SNS_DDF_EINVALID_DATA;

    // Open communication port to the device.
    status = sns_ddf_open_port(&state->port_handle,
        &device_info->port_config);
    if(status != SNS_DDF_SUCCESS)
    {
      sns_ddf_mfree(state);
      return status;
    }

    // confirm WHO_AM_I register
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LSM303D_REG_WHOAMI,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if((rw_bytes!=1) || 
		!(rw_buffer==STM_LSM303D_WHOAMI_VALUE))
        return SNS_DDF_EFAIL;

    // Resets the lsm303d
    status = sns_dd_lsm303d_reset(state);
    if(status != SNS_DDF_SUCCESS)
    {
      sns_ddf_close_port(state->port_handle);
      sns_ddf_mfree(state);
      return status;
    }

    //copy axis config from NV_PARAMS
    sns_ddf_axes_map_init(&state->axes_map,
        ((nv_params != NULL) ? nv_params->data : stm_lsm303d_axis_dflt));

    // Fill out output and driver parameters.
    state->sub_dev[0].smgr_handle = smgr_handle;
    state->sub_dev[0].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    state->gpio_num = device_info->first_gpio;
    *dd_handle_ptr = state;

  }
  else /* case the device instance is already there */
  {
    uint32_t sub_dev_idx;
    sns_ddf_handle_t dd_handle;

    sub_dev_idx = dd_find_free_sub_dev_slot(state);
    if ( -1 == sub_dev_idx )
    {
      return SNS_DDF_ENOMEM;
    }
    else if( sub_dev_idx >= LSM303D_MAX_SUB_DEV)
    {
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

      // dd_handle = (sns_ddf_handle_t)((uint32_t)state & sub_dev_idx);
      dd_handle = (sns_ddf_handle_t)((uint32_t)state | (uint32_t)sub_dev_idx);
      *dd_handle_ptr = dd_handle;
      state->sub_dev[sub_dev_idx].smgr_handle = smgr_handle;
      state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
      
      // Resets the lsm303d
      status = sns_dd_lsm303d_reset(dd_handle);
      if(status != SNS_DDF_SUCCESS)
      {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state);
        return status;
      }
    }
  }

  return SNS_DDF_SUCCESS;
}

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
static void sns_dd_acc_lsm303d_SWtest_notify(
	sns_ddf_handle_t handle,
	sns_ddf_sensor_e sensor,
	sns_ddf_status_e status,
	uint32_t err,
	q16_t* biases)
{
   sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    q16_t* biases_ptr;
    uint8_t i;
    uint32_t sub_dev_inx = 0;

    status = lsm303d_search_subdev_by_sensortype(
        state,
        SNS_DDF_SENSOR_ACCEL,
        &sub_dev_inx);
    if(status != SNS_DDF_SUCCESS)
      return;

    sns_ddf_smgr_notify_test_complete(
                state->sub_dev[sub_dev_inx].smgr_handle,
		sensor,
		status, err);

    if(status == SNS_DDF_SUCCESS)
    {
	biases_ptr = state->biases;

	for(i = 0; i < STM_LSM303D_NUM_AXES; i++)
	    biases_ptr[i] = biases[i];

	sns_ddf_smgr_notify_event(
                state->sub_dev[sub_dev_inx].smgr_handle,
		sensor,
		SNS_DDF_EVENT_BIAS_READY);
    }

}

/** 
 * @brief Runs lsm303d inertial software self test and bias calibration.
 *
 * @param[in]  dd_handle   Handle to a driver instance.
 * @param[in]  sensor      Sensor type.
 * 
 * @return SNS_DDF_PENDING if this SW self test is prepared to be performed later; SNS_DDF_EFAIL
 *         if SW self test fails. Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, 
 *         SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lsm303d_SW_selftest(
        sns_ddf_handle_t  dd_handle,
        sns_ddf_sensor_e  sensor)
{
    //sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    sns_ddf_status_e status;
    sns_dd_inertial_test_config_s test_config;

    test_config.sample_rate = 15; 
    test_config.num_samples = 64; 
    test_config.max_variance = FX_FLTTOFIX_Q16(383);                                     // max variance
    test_config.bias_thresholds[0] = FX_FLTTOFIX_Q16(STM_LSM303D_SWST_MAX_BIAS * G/1000); // x-axis max bias
    test_config.bias_thresholds[1] = FX_FLTTOFIX_Q16(STM_LSM303D_SWST_MAX_BIAS * G/1000); // y-axis max bias
    test_config.bias_thresholds[2] = FX_FLTTOFIX_Q16(STM_LSM303D_SWST_MAX_BIAS * G/1000); // z-axis max bias

    status = sns_dd_acc_lsm303d_set_datarate(dd_handle, 100);
    if(SNS_DDF_SUCCESS != status)
	return status;

    // wait 3 ODR.
    sns_ddf_delay(30000);

    status = sns_dd_inertial_test_run(
		    sensor,
		    dd_handle,
		    &sns_dd_lsm303d_if,
		    &test_config,
		    &sns_dd_acc_lsm303d_SWtest_notify);

    if(SNS_DDF_SUCCESS != status)
	return status;

    return SNS_DDF_PENDING;
}

/** 
 * @brief Runs lsm303d factory hardware self test and returns the test result.
 *        There is no need to call init before self test, while haveing init called 
 *        before it won't be harmful though.
 *        After self test is done, lsm303d driver and device will return to the state
 *        they were in after device is just powered on(need to call init and enable 
 *        before getting any data).
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * 
 * @return SNS_DDF_SUCCESS if this HW self test was done successfully; SNS_DDF_EFAIL
 *         if HW self test fails. Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, 
 *         SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lsm303d_HW_selftest(
        sns_ddf_handle_t  dd_handle)
{
    sns_dd_lsm303d_state_t* state = (sns_dd_lsm303d_state_t*)(((uint32_t)dd_handle) & (uint32_t)DD_HANDLE_ALIGN_BITS);
    sns_ddf_status_e status;
    uint8_t rw_buffer[3] = { 0, 0, 0 };
    uint8_t rw_bytes = 0;
    int16_t OUT_NOST[STM_LSM303D_NUM_AXES];
    int16_t OUT_ST[STM_LSM303D_NUM_AXES];
    int32_t OUTX_NOST,OUTY_NOST,OUTZ_NOST;
    int32_t OUTX_ST,OUTY_ST,OUTZ_ST;

    //Initialize and turn on sensor, enable X/Y/Z axes.
    //set BUD=1, FS=2G, ODR = 50Hz.
    rw_buffer[0] = 0x0;
    rw_buffer[1] = 0x0;
    rw_buffer[2] = 0x0; 
    status = sns_ddf_write_port(
        state->port_handle,
        AUTO_INCREMENT | STM_LSM303D_REG_CNTRL2,
        rw_buffer,
        3,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
	{
		LSM303D_MSG_1(ERROR, "sns_dd_acc_lsm303d_HW_selftest error=%d", status);
        return status;
	}
    if(rw_bytes != 3)
	{
        LSM303D_MSG_1(ERROR, "sns_dd_acc_lsm303d_HW_selftest rw_bytes=%d", rw_bytes);
        return SNS_DDF_EBUS;
	}
    
    rw_buffer[0] = 0x5F;
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LSM303D_REG_CNTRL1,
        rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
    {
		LSM303D_MSG_1(ERROR, "sns_dd_acc_lsm303d_HW_selftest error=%d", status);
        return status;
	}
    if(rw_bytes != 1)
    {
        LSM303D_MSG_1(ERROR, "sns_dd_acc_lsm303d_HW_selftest rw_bytes=%d", rw_bytes);
        return SNS_DDF_EBUS;        
	}        

    // wait 100 ms.
    sns_ddf_delay(100000);
    
    //Collect data
    memset(OUT_NOST, 0x0, sizeof(int16_t)*STM_LSM303D_NUM_AXES);
    status = sns_dd_acc_lsm303d_collect_testdata(dd_handle, OUT_NOST);
    if(status != SNS_DDF_SUCCESS)
    {
		LSM303D_MSG_1(ERROR, "sns_dd_acc_lsm303d_HW_selftest error=%d", status);
        return status;
	}
        
    //Enable self test
    rw_buffer[0] = 0x02;
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LSM303D_REG_CNTRL2,
        rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
    {
		LSM303D_MSG_1(ERROR, "sns_dd_acc_lsm303d_HW_selftest error=%d", status);
        return status;
	}
    if(rw_bytes != 1)
    {
        LSM303D_MSG_1(ERROR, "sns_dd_acc_lsm303d_HW_selftest rw_bytes=%d", rw_bytes);
        return SNS_DDF_EBUS;  
	}

    // wait 100 ms.
    sns_ddf_delay(100000);
    
    //Collect data
    memset(OUT_ST, 0x0, sizeof(int16_t)*STM_LSM303D_NUM_AXES);
    status = sns_dd_acc_lsm303d_collect_testdata(dd_handle, OUT_ST);
    if(status != SNS_DDF_SUCCESS)
    {
		LSM303D_MSG_1(ERROR, "sns_dd_acc_lsm303d_HW_selftest error=%d", status);
        return status;
	}
    
    //Check self test limits
    OUTX_NOST = OUT_NOST[0] * STM_LSM303D_ACC_SSTVT_2G /1000;
    OUTY_NOST = OUT_NOST[1] * STM_LSM303D_ACC_SSTVT_2G /1000;
    OUTZ_NOST = OUT_NOST[2] * STM_LSM303D_ACC_SSTVT_2G /1000;
    OUTX_ST = OUT_ST[0] * STM_LSM303D_ACC_SSTVT_2G /1000;
    OUTY_ST = OUT_ST[1] * STM_LSM303D_ACC_SSTVT_2G /1000;
    OUTZ_ST = OUT_ST[2] * STM_LSM303D_ACC_SSTVT_2G /1000;
	LSM303D_MSG_3(MED, "Accel: OUTX_NOST=%d, OUTY_NOST=%d, OUTZ_NOST=%d", OUTX_NOST, OUTY_NOST,OUTZ_NOST);
	LSM303D_MSG_3(MED, "Accel: OUTX_ST=%d, OUTY_ST=%d, OUTZ_ST=%d", OUTX_ST, OUTY_ST,OUTZ_ST);
	LSM303D_MSG_3(MED, "Accel: OUTX_DIFF=%d, OUTY_DIFF=%d, OUTZ_DIFF=%d", abs(OUTX_ST-OUTX_NOST), abs(OUTY_ST-OUTY_NOST),abs(OUTZ_ST-OUTZ_NOST));
    if((STM_LSM303D_ST_2G_MIN <= abs(OUTX_ST-OUTX_NOST))
	&&(abs(OUTX_ST-OUTX_NOST) <= STM_LSM303D_ST_2G_MAX)
        &&(STM_LSM303D_ST_2G_MIN <= abs(OUTY_ST-OUTY_NOST))
        &&(abs(OUTY_ST-OUTY_NOST)<= STM_LSM303D_ST_2G_MAX)
        &&(STM_LSM303D_ST_2G_MIN <= abs(OUTZ_ST-OUTZ_NOST))
        &&(abs(OUTZ_ST-OUTZ_NOST)<= STM_LSM303D_ST_2G_MAX))
        status = SNS_DDF_SUCCESS;
    else
    {
        status = SNS_DDF_EFAIL;
		LSM303D_MSG_0(ERROR, "sns_dd_acc_lsm303d_HW_selftest OUT OF RANGE");
	}
    
    //Disable sensor and self test
    rw_buffer[0] = 0x0;
    sns_ddf_write_port(
        state->port_handle,
        STM_LSM303D_REG_CNTRL1,
        rw_buffer,
        1,
        &rw_bytes);
    sns_ddf_write_port(
        state->port_handle,
        STM_LSM303D_REG_CNTRL2,
        rw_buffer,
        1,
        &rw_bytes);

    return status;
}

/** 
 * @brief Runs lsm303d self test and returns the test result.
 *        Supports SNS_DDF_TEST_SELF_HW and SNS_DDF_TEST_SELF_SW.
 *        SNS_DDF_TEST_SELF_HW is the hardware self test provided by LSM303D.
 *        SNS_DDF_TEST_SELF_SW is the inertial software self test and bias calibration. 
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_lsm303d_run_test(
        sns_ddf_handle_t  dd_handle,
        sns_ddf_sensor_e  sensor,
        sns_ddf_test_e    test,
        uint32_t*         err)
{
    sns_ddf_status_e status;
    
    //verify all the parameters. LSM303D only provides accel self tests.
    if((sensor != SNS_DDF_SENSOR_ACCEL) && (sensor != SNS_DDF_SENSOR__ALL))
        return SNS_DDF_EINVALID_PARAM;

    switch(test)
    {
	case SNS_DDF_TEST_SELF_HW:
	    status = sns_dd_acc_lsm303d_HW_selftest(dd_handle);
	    break;
	case SNS_DDF_TEST_SELF_SW:
	    status = sns_dd_acc_lsm303d_SW_selftest(dd_handle, sensor);
	    break;
	default:
	    return SNS_DDF_EINVALID_TEST;
    }

    return status;
}


/** 
 * LSM303DLHC device driver interface.
 */
sns_ddf_driver_if_s sns_dd_lsm303d_if =
{
    &sns_dd_lsm303d_init,
    &sns_dd_lsm303d_get_data,
    &sns_dd_lsm303d_set_attr,
    &sns_dd_lsm303d_get_attr,
    NULL,   // handle timer
    NULL,   // handle irq
    &sns_dd_lsm303d_reset,
    &sns_dd_lsm303d_run_test
};

