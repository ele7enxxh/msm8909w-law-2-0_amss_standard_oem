/*==============================================================================

    S E N S O R S   ACCELEROMETER  D R I V E R

DESCRIPTION

   Implements the driver for the MC3410 accelerometer Sensor

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_mc3410_ext.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
11/29/13     yh      Initial version of driver for mc3410
==============================================================================*/

#include "sns_dd_accel_mc3410_priv.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_debug_str.h"
#include "sns_ddf_signal.h"

extern sns_ddf_driver_if_s sns_dd_acc_mc3410_if;
extern sns_ddf_status_e sns_dd_acc_mc3410_reset(sns_ddf_handle_t dd_handle);
extern mc3410_range mc3410_ranges[];
extern sns_ddf_lowpass_freq_t mc3410_freqs[];
extern mc3410_odr mc3410_odr_setting[];
extern const sns_ddf_odr_t mc3410_odr_array[];

static sns_ddf_sensor_e my_sensors[] =
    {
        SNS_DDF_SENSOR_ACCEL
    };

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_init

DESCRIPTION  Initializes the MC3410 driver and sets up sensor.
============================================================================*/
sns_ddf_status_e sns_dd_acc_mc3410_init(
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
    sns_dd_acc_mc3410_state_t* state;

    /* Allocate a driver instance.*/
    status = sns_ddf_malloc((void**)&state, sizeof(sns_dd_acc_mc3410_state_t));
    if(status != SNS_DDF_SUCCESS)
    {
        return SNS_DDF_ENOMEM;
    }
    state->smgr_hndl = smgr_handle;
    state->gpio_num = device_info->first_gpio;
    state->motion_detect_enabled = false;

    /* Open communication port to the device.*/
    status = sns_ddf_open_port(&(state->port_handle), &(device_info->port_config));
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_mfree(state);
        return status;
    }

    *num_sensors = 1;
    *sensors = my_sensors;
    *dd_handle_ptr = (sns_ddf_handle_t)state;

    /* Resets the mc3410*/
    status = sns_dd_acc_mc3410_reset(state);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state);
        return status;
    }

    sns_ddf_axes_map_init(
        &state->axes_map, ((nv_params != NULL) ? nv_params->data : NULL));

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_get_attr

DESCRIPTION  Retrieves the value of an attribute for MC3410.
============================================================================*/
sns_ddf_status_e sns_dd_acc_mc3410_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems)
{
    sns_dd_acc_mc3410_state_t *state = dd_handle;

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

            /*current consumption, unit uA*/
            power->active_current = MC3410_ACTIVE_C;
            power->lowpower_current = MC3410_LOWPW_C;
            *value = power;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s* ranges = sns_ddf_memhandler_malloc(
                memhandler, MC3410_RANGE_NUM * sizeof(sns_ddf_range_s));
            if(NULL == ranges)
                return SNS_DDF_ENOMEM;

            ranges[0].min = MC3410_RANGE_2G_MIN;
            ranges[0].max = MC3410_RANGE_2G_MAX;
            ranges[1].min = MC3410_RANGE_4G_MIN;
            ranges[1].max = MC3410_RANGE_4G_MAX;
            ranges[2].min = MC3410_RANGE_8G_MIN;
            ranges[2].max = MC3410_RANGE_8G_MAX;

            *num_elems = MC3410_RANGE_NUM;
            *value = ranges;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            sns_ddf_resolution_adc_s *res = sns_ddf_memhandler_malloc(
                memhandler ,sizeof(sns_ddf_resolution_adc_s));
            if(NULL == res)
                return SNS_DDF_ENOMEM;

            res->bit_len = MC3410_BIT_LEN;
            res->max_freq = MC3410_MAX_FREQ;
            *value = res;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t *res = sns_ddf_memhandler_malloc(
                memhandler ,sizeof(sns_ddf_resolution_t));
            if(NULL == res)
                return SNS_DDF_ENOMEM;

            *res = FX_FLTTOFIX_Q16(2*state->sstvt_adj * G / 1024);
            *value = res;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            *value = mc3410_freqs;
            *num_elems = MC3410_ODR_NUM;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_ODR:
        {
            sns_ddf_odr_t *res = sns_ddf_memhandler_malloc(
                memhandler ,sizeof(sns_ddf_odr_t));
            if(NULL == res){
                return SNS_DDF_ENOMEM;
            }

            /*always return the current accel ODR to SMGR.*/
            if((SNS_DDF_SENSOR_ACCEL == sensor)||(SNS_DDF_SENSOR__ALL == sensor)){
                *res = state->acc_cur_rate;
            }
            else {
                return SNS_DDF_EINVALID_PARAM;
            }

            *value = res;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s *mc3410_delays = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_delays_s));
            if(NULL == mc3410_delays)
                return SNS_DDF_ENOMEM;

            mc3410_delays->time_to_active = 5;
            mc3410_delays->time_to_data = 1;
            *value = mc3410_delays;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s *info = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_driver_info_s));
            if(NULL == info)
                return SNS_DDF_ENOMEM;

            info->name = "MC3410";
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
            info->vendor = "mCUBE";
            info->model = "MC3410";
            info->version = 1;
            *value = info;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }
        default:
            return SNS_DDF_EINVALID_ATTR;
    }
}
