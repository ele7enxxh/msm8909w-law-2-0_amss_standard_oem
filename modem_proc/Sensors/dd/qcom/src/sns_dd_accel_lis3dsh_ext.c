/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
 *
 * File Name         : lis3dsh_ddf_drv.c
 * Authors           : Jianjian(Samuel) Huo
 * Version           : V 3.6.1
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
 *******************************************************************************/

/*******************************************************************************
* Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
*******************************************************************************/

/*=============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  11/18/2013  pn   Removed BW timer
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
#include "string.h"
#include <stdbool.h>

extern sns_ddf_status_e sns_dd_acc_lis3dsh_reset(sns_ddf_handle_t dd_handle);
extern sns_ddf_driver_if_s sns_dd_acc_lis3dsh_if;
extern stm_lis3dsh_range lis3dsh_ranges[];
extern sns_ddf_lowpass_freq_t lis3dsh_freqs[];
extern stm_lis3dsh_odr lis3dsh_odr_setting[];
extern volatile boolean EnableI2C;

static stm_lis3dsh_head_s lis3dsh_dd_head = {
  NULL,
  {"STMICRO_LIS3DSH", 2}
};
static uint8_t stm_lis3dsh_axis_dflt[3] = {1, 2, 3};

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
static uint32_t dd_find_free_sub_dev_slot (sns_dd_acc_lis3dsh_state_t *my_dev_ptr)
{
  uint32_t i;
  for (i =0; i < LIS3DSH_MAX_SUB_DEV; i++)
  {
    if ( 0 == my_dev_ptr->sub_dev[i].num_sensors )
    {
      return i;
    }
  }
  return 0xffffffff;
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
static uint32_t dd_fill_sub_dev_slot (stm_lis3dsh_sub_dev_s *sub_dev_slot_ptr, uint32_t num_sensors, sns_ddf_sensor_e sensors[])
{
  uint32_t my_num_sensors = 0, i;
  for (i =0; i < num_sensors; i++)
  {
    /*
     * Note: both ACCEL and DOUBLETAP are Primary Data Types, by definition.
     */
    if ((SNS_DDF_SENSOR_ACCEL==sensors[i])||(SNS_DDF_SENSOR_DOUBLETAP==sensors[i]))
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
static sns_dd_acc_lis3dsh_state_t *dd_find_same_port_dev (sns_ddf_port_config_s *port_config_ptr)
{
  sns_dd_acc_lis3dsh_state_t *cur_dd_dev_ptr = (sns_dd_acc_lis3dsh_state_t *)lis3dsh_dd_head.next;

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
 * @brief Config lis3dsh SM1 and SM2 according to NV_PARAMS.
 *
 *
 * @param[in]  nv_params     pointer to nv_params data structure.
 * @param[out] sm_config      pointer to sm_config state structure.
 *
 * @return void.
 */
static void sns_dd_acc_lis3dsh_config_sm(
    sns_ddf_nv_params_s* nv_params,
    stm_lis3dsh_sm_config *sm_config)
{
  bool set_default = true;
  stm_lis3dsh_nv_params *lis3dsh_params;

  if((nv_params)&&(nv_params->data)&&(nv_params->data_len >= STM_LIS3DSH_NVPARAMS_DATALEN))
  {
    lis3dsh_params = (stm_lis3dsh_nv_params *)((uint8_t *)nv_params->data + STM_LIS3DSH_NVPARAMS_OFFSET);

    if((STM_LIS3DSH_MAG_NUM1 == lis3dsh_params->magic_num[0])
        && (STM_LIS3DSH_MAG_NUM2 == lis3dsh_params->magic_num[1])
        && (STM_LIS3DSH_MAG_NUM3 == lis3dsh_params->magic_num[2])
        && (STM_LIS3DSH_MAG_NUM4 == lis3dsh_params->magic_num[3]))
    {
      //NV_PARAMS for SM1 and SM2 is valid.
      set_default =  false;
      //This driver has two sets of MD settings. One for when DBT is on, another for when DBT is off.
      //The setting in sm_config is only used for MD when DBT is on.
      sm_config->md_thresh = lis3dsh_params->md_thresh;
      sm_config->consistency_cnt = lis3dsh_params->consistency_cnt;
      sm_config->md_ODR = lis3dsh_params->md_ODR;
      sm_config->dbt_thresh_1 = lis3dsh_params->dbt_thresh_1;
      sm_config->dbt_thresh_2 = lis3dsh_params->dbt_thresh_2;
      sm_config->dbt_tim1 = lis3dsh_params->dbt_tim1;
      sm_config->dbt_tim2 = lis3dsh_params->dbt_tim2;
      sm_config->dbt_tim3 = lis3dsh_params->dbt_tim3;
      sm_config->dbt_tim4 = lis3dsh_params->dbt_tim4;

      return;
    }
  }

  //set default config to SM1 and SM2
  if(set_default)
  {
    //This driver has two sets of MD settings. One for when DBT is on, another for when DBT is off.
    //The setting in sm_config is only used for MD when DBT is on.
    sm_config->md_thresh = STM_LIS3DSH_MD_THRESH_2;
    sm_config->consistency_cnt = STM_LIS3DSH_CONSTANT_CNT_2;
    sm_config->md_ODR = STM_LIS3DSH_MD_ODR_2;
    sm_config->dbt_thresh_1 = STM_LIS3DSH_DBT_THRESH_1;
    sm_config->dbt_thresh_2 = STM_LIS3DSH_DBT_THRESH_2;
    sm_config->dbt_tim1 = STM_LIS3DSH_DBT_TIM1;
    sm_config->dbt_tim2 = STM_LIS3DSH_DBT_TIM2;
    sm_config->dbt_tim3 = STM_LIS3DSH_DBT_TIM3;
    sm_config->dbt_tim4 = STM_LIS3DSH_DBT_TIM4;
  }

  return;
}

/**
 * @brief Initializes the lis3dsh driver and sets up sensor.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_init(
    sns_ddf_handle_t* dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_dd_acc_lis3dsh_state_t* state;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;
  uint32_t my_num_sensors = 0;

  /* check if this init() is a call for an existing device */
  state = dd_find_same_port_dev(&device_info->port_config);

  if ( NULL == state)
  {
    /* Allocate a device instance and initialize it */
    status = sns_ddf_malloc((void**)&state, sizeof(sns_dd_acc_lis3dsh_state_t));
    if(status != SNS_DDF_SUCCESS)
      return SNS_DDF_ENOMEM;
    memset(state, 0, sizeof(sns_dd_acc_lis3dsh_state_t));

    /* link this dev instance to the front of the list */
    state->next = lis3dsh_dd_head.next;
    lis3dsh_dd_head.next = state;
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

    // wait sensor powering up to be ready by delaying 10 ms.
    //sns_ddf_delay(10000);

    // confirm WHO_AM_I register
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LIS3DSH_WHO_AM_I,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(EnableI2C == true) {
       if((rw_bytes!=1) ||
          !((rw_buffer==STM_LIS3DSH_WHOAMI)||(rw_buffer==STM_LSM330_WHOAMI)))
       return SNS_DDF_EFAIL;
    }

    // Resets the lis3dsh
    status = sns_dd_acc_lis3dsh_reset(state);
    if(status != SNS_DDF_SUCCESS)
    {
      sns_ddf_close_port(state->port_handle);
      sns_ddf_mfree(state);
      return status;
    }

    //copy axis config from NV_PARAMS
    sns_ddf_axes_map_init(&state->axes_map,
        ((nv_params != NULL) ? nv_params->data : stm_lis3dsh_axis_dflt));

    //copy SM configs from NV_PARAMS
    sns_dd_acc_lis3dsh_config_sm(nv_params, &state->sm_config);

    // Fill out output and driver parameters.
    state->sub_dev[0].smgr_handle = smgr_handle;
    state->sub_dev[0].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    state->gpio_num = device_info->first_gpio;
    *dd_handle_ptr = state;

    //allocate memory for double tap data sample and fifo data sample.
    //state->dbt_data.samples = sns_ddf_memhandler_malloc(memhandler, STM_LIS3DSH_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
    status = sns_ddf_malloc((void**)&(state->dbt_data.samples),
        STM_LIS3DSH_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
    if(status != SNS_DDF_SUCCESS)
      return SNS_DDF_ENOMEM;

    memset(state->dbt_data.samples, 0, STM_LIS3DSH_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));

    state->fifo_data.samples = NULL;
    state->fifo_raw_buffer = NULL;

  }
  else /* case the device instance is already there */
  {
    uint32_t sub_dev_idx;
    sns_ddf_handle_t dd_handle;

    sub_dev_idx = dd_find_free_sub_dev_slot(state);
    if ( 0xffffffff == sub_dev_idx )
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

      ///            dd_handle = (sns_ddf_handle_t)((uint32_t)state & sub_dev_idx);
      dd_handle = (sns_ddf_handle_t)((dd_handle_align_t)state | (dd_handle_align_t)sub_dev_idx);
      *dd_handle_ptr = dd_handle;
      state->sub_dev[sub_dev_idx].smgr_handle = smgr_handle;
      state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    }
  }

  return status;
}


/**
 * @brief Retrieves the value of an attribute for a lis3dsh sensor.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems)
{
  sns_dd_acc_lis3dsh_state_t *state;
//  uint32_t sub_dev_idx;

  if (dd_handle == NULL || value == NULL)
    return SNS_DDF_EINVALID_PARAM;

  state = (sns_dd_acc_lis3dsh_state_t*)((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);

  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_INFO:
      {
        sns_ddf_power_info_s* power = sns_ddf_memhandler_malloc(
            memhandler, sizeof(sns_ddf_power_info_s));
        if(NULL == power)
          return SNS_DDF_ENOMEM;

        //current consumption, unit uA
        power->active_current = 280;
        power->lowpower_current = 2;
        *value = power;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }

    case SNS_DDF_ATTRIB_RANGE:
      {
        if(sensor == SNS_DDF_SENSOR_ACCEL)
        {
          // For Accel only
          sns_ddf_range_s* ranges = sns_ddf_memhandler_malloc(
              memhandler, STM_LIS3DSH_RANGE_NUM * sizeof(sns_ddf_range_s));
          if(NULL == ranges)
            return SNS_DDF_ENOMEM;

          ranges[0].min = STM_LIS3DSH_RANGE_2G_MIN;
          ranges[0].max = STM_LIS3DSH_RANGE_2G_MAX;
          ranges[1].min = STM_LIS3DSH_RANGE_4G_MIN;
          ranges[1].max = STM_LIS3DSH_RANGE_4G_MAX;
          ranges[2].min = STM_LIS3DSH_RANGE_6G_MIN;
          ranges[2].max = STM_LIS3DSH_RANGE_6G_MAX;
          ranges[3].min = STM_LIS3DSH_RANGE_8G_MIN;
          ranges[3].max = STM_LIS3DSH_RANGE_8G_MAX;
          ranges[4].min = STM_LIS3DSH_RANGE_16G_MIN;
          ranges[4].max = STM_LIS3DSH_RANGE_16G_MAX;

          *num_elems = STM_LIS3DSH_RANGE_NUM;
          *value = ranges;
          return SNS_DDF_SUCCESS;
        }
        else
        {
          // For double tap only
          sns_ddf_range_s* ranges = sns_ddf_memhandler_malloc(
              memhandler, STM_LIS3DSH_DBT_RANGE_NUM * sizeof(sns_ddf_range_s));
          if(NULL == ranges)
            return SNS_DDF_ENOMEM;

          ranges[0].min = STM_LIS3DSH_DBT_RANGE_MIN;
          ranges[0].max = STM_LIS3DSH_DBT_RANGE_MAX;

          *num_elems = STM_LIS3DSH_DBT_RANGE_NUM;
          *value = ranges;
          return SNS_DDF_SUCCESS;
        }
      }

    case SNS_DDF_ATTRIB_RESOLUTION:
      {
        if(sensor == SNS_DDF_SENSOR_ACCEL)
        {
          // for ACCEL
          sns_ddf_resolution_t *res = sns_ddf_memhandler_malloc(
              memhandler ,sizeof(sns_ddf_resolution_t));
          if(NULL == res)
            return SNS_DDF_ENOMEM;

          *res = FX_FLTTOFIX_Q16(state->sstvt_adj * G / 1000000);
          *value = res;
          *num_elems = 1;

          return SNS_DDF_SUCCESS;
        }
        else
        {
          // for double tap
          sns_ddf_resolution_t *res = sns_ddf_memhandler_malloc(
              memhandler ,sizeof(sns_ddf_resolution_t));
          if(NULL == res)
            return SNS_DDF_ENOMEM;

          *res = 1;
          *value = res;
          *num_elems = 1;

          return SNS_DDF_SUCCESS;

        }
      }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
      {
        // For Accel and DT. When double Tap, return LIS3DSH_DBT_ODR.
        sns_ddf_resolution_adc_s *res = sns_ddf_memhandler_malloc(
            memhandler ,sizeof(sns_ddf_resolution_adc_s));
        if(NULL == res)
          return SNS_DDF_ENOMEM;

        res->bit_len = 16;
        res->max_freq = ((sensor == SNS_DDF_SENSOR_ACCEL) ? 200 : LIS3DSH_DBT_ODR);
        *value = res;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;

      }

    case SNS_DDF_ATTRIB_LOWPASS:
      {
        if(sensor == SNS_DDF_SENSOR_ACCEL)
        {
          // Accel
          *value = lis3dsh_freqs;
          *num_elems = STM_LIS3DSH_ODR_NUM;

          return SNS_DDF_SUCCESS;
        }
        else
        {
          return SNS_DDF_EINVALID_ATTR;
        }
      }

    case SNS_DDF_ATTRIB_ODR:
      {
        // for both
        sns_ddf_odr_t *res = sns_ddf_memhandler_malloc(
            memhandler ,sizeof(sns_ddf_odr_t));
        if(NULL == res)
          return SNS_DDF_ENOMEM;

        //always return the current accel ODR to SMGR.
        if((SNS_DDF_SENSOR_ACCEL == sensor)||(SNS_DDF_SENSOR__ALL == sensor))
          *res = state->acc_cur_rate;
        else if(SNS_DDF_SENSOR_DOUBLETAP == sensor)
          *res = state->dbt_cur_rate;
        else
          return SNS_DDF_EINVALID_PARAM;

        *value = res;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }
    case SNS_DDF_ATTRIB_DELAYS:
      {
        if(sensor == SNS_DDF_SENSOR_ACCEL)
        {
          // for Accel
          sns_ddf_delays_s *lis3dsh_delays = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_delays_s));
          if(NULL == lis3dsh_delays)
            return SNS_DDF_ENOMEM;

          lis3dsh_delays->time_to_active = 1;
          lis3dsh_delays->time_to_data = 1;
          *value = lis3dsh_delays;
          *num_elems = 1;

          return SNS_DDF_SUCCESS;
        }
        else
        {
          return SNS_DDF_EINVALID_ATTR;
        }
      }

    case SNS_DDF_ATTRIB_BIAS:
      {
        if(sensor == SNS_DDF_SENSOR_ACCEL)
        {
          // for accel
          *value = state->biases;
          *num_elems = STM_LIS3DSH_NUM_AXES;

          return SNS_DDF_SUCCESS;
        }
        else
        {
          return SNS_DDF_EINVALID_ATTR;
        }
      }

    case SNS_DDF_ATTRIB_FIFO:
      {
        if(sensor == SNS_DDF_SENSOR_ACCEL)
        {
          // for ACCEL
          sns_ddf_fifo_attrib_get_s *res = sns_ddf_memhandler_malloc(
              memhandler ,sizeof(sns_ddf_fifo_attrib_get_s));
          if(NULL == res)
            return SNS_DDF_ENOMEM;

          res->is_supported = true;
          res->is_sw_watermark = false;
          res->max_watermark = STM_LIS3DSH_MAX_FIFO;
          res->share_sensor_cnt = 0;
          res->share_sensors[0] =  NULL;
          *value = res;
          *num_elems = 1;

          return SNS_DDF_SUCCESS;
        }
        else
        {
          return SNS_DDF_EINVALID_ATTR;
        }
      }

    case SNS_DDF_ATTRIB_ODR_TOLERANCE:
      {
        if(sensor == SNS_DDF_SENSOR_ACCEL)
        {
          // for ACCEL
          q16_t *res = sns_ddf_memhandler_malloc(
              memhandler ,sizeof(q16_t));
          if(NULL == res)
            return SNS_DDF_ENOMEM;

          *res = FX_FLTTOFIX_Q16(0.05);
          *value = res;
          *num_elems = 1;

          return SNS_DDF_SUCCESS;
        }
        else
        {
          return SNS_DDF_EINVALID_ATTR;
        }
      }

    case SNS_DDF_ATTRIB_IO_REGISTER:
      {
        sns_ddf_io_register_s* io_reg_in = (sns_ddf_io_register_s *) (*value);
        uint8_t bytes_read;
        sns_ddf_status_e status;

        if ((uint32_t)(io_reg_in->address_ptr) > 0xFFFF)
        {
          return SNS_DDF_EINVALID_PARAM;
        }
        if (io_reg_in->length > 0xFF)
        {
          return SNS_DDF_EINVALID_PARAM;
        }

        // Allocate memory for the output data structure and for the data to be read from the register(s)
        sns_ddf_io_register_s* io_reg_out = sns_ddf_memhandler_malloc(
            memhandler, sizeof(sns_ddf_io_register_s) + (sizeof(uint8_t) * (io_reg_in->length - 1)));
        if (io_reg_out == NULL)
          return SNS_DDF_ENOMEM;

        status = sns_ddf_read_port(
            state->port_handle,
            (uint16_t) ((uint32_t)io_reg_in->address_ptr),
            &(io_reg_out->data[0]),
            (uint8_t) io_reg_in->length,
            &bytes_read);
        if (status != SNS_DDF_SUCCESS)
          return status;

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
        // for both
        sns_ddf_driver_info_s *info = sns_ddf_memhandler_malloc(
            memhandler, sizeof(sns_ddf_driver_info_s));
        if(NULL == info)
          return SNS_DDF_ENOMEM;

        info->name = "STMicroelectronics LIS3DSH Driver";
        info->version = 2;
        *value = info;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }

    case SNS_DDF_ATTRIB_DEVICE_INFO:
      {
        // for both, if DT, info->name = "Double-Tap";
        sns_ddf_device_info_s *info = sns_ddf_memhandler_malloc(
            memhandler, sizeof(sns_ddf_device_info_s));
        if(NULL == info)
          return SNS_DDF_ENOMEM;

        info->name = ((sensor == SNS_DDF_SENSOR_ACCEL) ? "Accelerometer" : "Double-Tap");
        info->vendor = "STMicroelectronics";
        info->model = "LIS3DSH";
        info->version = 1;
        *value = info;
        *num_elems = 1;

        return SNS_DDF_SUCCESS;
      }
    default:
      return SNS_DDF_EINVALID_ATTR;
  }
}


