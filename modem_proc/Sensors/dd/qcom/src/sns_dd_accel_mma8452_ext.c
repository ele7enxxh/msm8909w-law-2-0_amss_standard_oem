/*  Date: 2013/08/05 10:00:00
 *  Revision: 1.0
 */

/*==============================================================================

    S E N S O R S   ACCELEROMETER  D R I V E R

DESCRIPTION

   Implements the driver for the MMA8452 accelerometer Sensor

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_mma8452_ext.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
08/09/13     yh      Initial version of driver for mma8452
==============================================================================*/

#include "sns_dd_accel_mma8452.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_debug_str.h"

extern sns_ddf_status_e sns_dd_acc_mma8452_reset(sns_ddf_handle_t dd_handle);
extern mma8452_range mma8452_ranges[];
extern sns_ddf_lowpass_freq_t mma8452_freqs[];
extern mma8452_odr mma8452_odr_setting[];
extern const sns_ddf_odr_t mma8452_odr_array[];

/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_init

DESCRIPTION  Initializes the MMA8452 driver and sets up sensor.
============================================================================*/
sns_ddf_status_e sns_dd_acc_mma8452_init(
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
    sns_dd_acc_mma8452_state_t* state;
    static sns_ddf_sensor_e my_sensors[] = {
        SNS_DDF_SENSOR_ACCEL};

    /* Allocate a driver instance.*/
    status = sns_ddf_malloc((void**)&state, sizeof(sns_dd_acc_mma8452_state_t));
    if(status != SNS_DDF_SUCCESS)
    {
        return SNS_DDF_ENOMEM;
    }
    state->smgr_hndl = smgr_handle;
    state->gpio_num = device_info->first_gpio;
    state->motion_detect_enabled = false;

    /* Open communication port to the device.*/
    status = sns_ddf_open_port(&state->port_handle, &device_info->port_config);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_mfree(state);
        return status;
    }

    /* wait sensor powering up to be ready by delaying 5 ms.*/
    sns_ddf_delay(5000);

    /* Resets the mma8452*/
    status = sns_dd_acc_mma8452_reset(state);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state);
        return status;
    }

    sns_ddf_axes_map_init(
        &state->axes_map, ((nv_params != NULL) ? nv_params->data : NULL));

    /* Fill out output parameters.*/
    *num_sensors = 1;
    *sensors = my_sensors;
    *dd_handle_ptr = state;

    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_get_attr

DESCRIPTION  Retrieves the value of an attribute for MMA8452.
============================================================================*/
sns_ddf_status_e sns_dd_acc_mma8452_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems)
{
    sns_dd_acc_mma8452_state_t *state = dd_handle;

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
            power->active_current = 165;
            power->lowpower_current = 6;
            *value = power;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s* ranges = sns_ddf_memhandler_malloc(
                memhandler, MMA8452_RANGE_NUM * sizeof(sns_ddf_range_s));
            if(NULL == ranges)
                return SNS_DDF_ENOMEM;

            ranges[0].min = MMA8452_RANGE_2G_MIN;
            ranges[0].max = MMA8452_RANGE_2G_MAX;
            ranges[1].min = MMA8452_RANGE_4G_MIN;
            ranges[1].max = MMA8452_RANGE_4G_MAX;
            ranges[2].min = MMA8452_RANGE_8G_MIN;
            ranges[2].max = MMA8452_RANGE_8G_MAX;

            *num_elems = MMA8452_RANGE_NUM;
            *value = ranges;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t *res = sns_ddf_memhandler_malloc(
                memhandler ,sizeof(sns_ddf_resolution_t));
            if(NULL == res)
                return SNS_DDF_ENOMEM;

            *res = FX_FLTTOFIX_Q16(2*state->sstvt_adj * G / 4096);
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
            *value = mma8452_freqs;
            *num_elems = MMA8452_ODR_NUM;

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
            sns_ddf_delays_s *mma8452_delays = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_delays_s));
            if(NULL == mma8452_delays)
                return SNS_DDF_ENOMEM;

            mma8452_delays->time_to_active = 5;
            mma8452_delays->time_to_data = 1;
            *value = mma8452_delays;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s *info = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_driver_info_s));
            if(NULL == info)
                return SNS_DDF_ENOMEM;

            info->name = "MMA8452";
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
            info->vendor = "Freescale";
            info->model = "MMA8452";
            info->version = 1;
            *value = info;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }
        default:
            return SNS_DDF_EINVALID_ATTR;
    }
}

