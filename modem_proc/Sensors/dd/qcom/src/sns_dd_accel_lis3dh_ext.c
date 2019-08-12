/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
*
* File Name         : lis3dh_ddf_drv.c
* Authors           : Jianjian Huo (samuel.huo@st.com)
* Version           : V 1.0.0
* Date              : 08/29/2011
* Description       : LIS3DH Accelerometer driver source file
*
********************************************************************************
* Copyright (c) 2011, STMicroelectronics.
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
* VERSION | DATE       | DESCRIPTION
* 0.4      |03/06/2012 | added gpio_num during init
* 0.3     | 08/31/2011 | changed time_to_active and time_to_data values
* 0.2     | 06/09/2011 | Added support for SNS_DDF_ATTRIB_RESOLUTION_ADC
* 0.1     | 05/06/2011 | Created.
*******************************************************************************/

#include "sns_dd_accel_lis3dh.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"


extern sns_ddf_status_e sns_dd_acc_lis3dh_reset(sns_ddf_handle_t dd_handle);
extern stm_lis3dh_range lis3dh_ranges[];
extern sns_ddf_lowpass_freq_t lis3dh_freqs[];
extern stm_lis3dh_odr lis3dh_odr_setting[];

/** 
 * @brief Initializes the lis3dh driver and sets up sensor. 
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_acc_lis3dh_init(
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
    sns_dd_acc_lis3dh_state_t* state;
    static sns_ddf_sensor_e my_sensors[] = {
        SNS_DDF_SENSOR_ACCEL};

    // Allocate a driver instance.
    status = sns_ddf_malloc((void**)&state, sizeof(sns_dd_acc_lis3dh_state_t));
    if(status != SNS_DDF_SUCCESS)
    {
        return SNS_DDF_ENOMEM;
    }
    state->smgr_hndl = smgr_handle;
    state->gpio_num = device_info->first_gpio;

    // Open communication port to the device.
    status = sns_ddf_open_port(&state->port_handle, &device_info->port_config); 
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_mfree(state);
        return status;
    }

    // wait sensor powering up to be ready by delaying 5 ms.
    sns_ddf_delay(5000);

    // Resets the lis3dh
    status = sns_dd_acc_lis3dh_reset(state);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state);
        return status;
    }

    sns_ddf_axes_map_init(
        &state->axes_map, ((nv_params != NULL) ? nv_params->data : NULL));

    // Fill out output parameters.
    *num_sensors = 1;
    *sensors = my_sensors;
    *dd_handle_ptr = state;

    return SNS_DDF_SUCCESS;
}


/** 
 * @brief Retrieves the value of an attribute for a lis3dh sensor.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_acc_lis3dh_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems)
{
    sns_dd_acc_lis3dh_state_t *state = dd_handle;
    
    if(sensor != SNS_DDF_SENSOR_ACCEL)
        return SNS_DDF_EINVALID_PARAM;
    
    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_INFO:
        {
            sns_ddf_power_info_s* power = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_power_info_s));
            if(NULL == power)
                return SNS_DDF_ENOMEM;

            //current consumption, unit uA
            power->active_current = 11;
            power->lowpower_current = 1;
            *value = power;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s* ranges = sns_ddf_memhandler_malloc(
                memhandler, STM_LIS3DH_RANGE_NUM * sizeof(sns_ddf_range_s));
            if(NULL == ranges)
                return SNS_DDF_ENOMEM;
            
            ranges[0].min = STM_LIS3DH_RANGE_2G_MIN;
            ranges[0].max = STM_LIS3DH_RANGE_2G_MAX;
            ranges[1].min = STM_LIS3DH_RANGE_4G_MIN;
            ranges[1].max = STM_LIS3DH_RANGE_4G_MAX;
            ranges[2].min = STM_LIS3DH_RANGE_8G_MIN;
            ranges[2].max = STM_LIS3DH_RANGE_8G_MAX;
            ranges[3].min = STM_LIS3DH_RANGE_16G_MIN;
            ranges[3].max = STM_LIS3DH_RANGE_16G_MAX;

            *num_elems = STM_LIS3DH_RANGE_NUM;
            *value = ranges;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t *res = sns_ddf_memhandler_malloc(
                memhandler ,sizeof(sns_ddf_resolution_t));
            if(NULL == res)
                return SNS_DDF_ENOMEM;

            *res = FX_FLTTOFIX_Q16(state->sstvt_adj * G / 1000);
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
            
            res->bit_len = 12;
            res->max_freq = 200;
            *value = res;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;

        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            *value = lis3dh_freqs;
            *num_elems = STM_LIS3DH_ODR_NUM;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s *lis3dh_delays = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_delays_s));
            if(NULL == lis3dh_delays)
                return SNS_DDF_ENOMEM;

            lis3dh_delays->time_to_active = 5;
            lis3dh_delays->time_to_data = 1;
            *value = lis3dh_delays;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s *info = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_driver_info_s));
            if(NULL == info)
                return SNS_DDF_ENOMEM;

            info->name = "STMicroelectronics LIS3DH Driver";
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
            
            info->name = "Accelerometer";
            info->vendor = "STMicroelectronics";
            info->model = "LIS3DH";
            info->version = 1;
            *value = info;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }
        default:
            return SNS_DDF_EINVALID_ATTR;
    }
}

