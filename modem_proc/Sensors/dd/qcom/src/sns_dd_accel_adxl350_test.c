/*==============================================================================

    S E N S O R S   A C C E L E R O M E T E R    D R I V E R    T E S T E R

DESCRIPTION

   ADXL350 On Target Unit-Test.

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_adxl350_test.c#1 $


when         who     what, where, why
----------   ---    -----------------------------------------------------------
12/16/10     as     Draft 
04/04/11     as     Add ADXL Unit-Test under ifdef CONFIG_ADXL_UNIT_TEST.
==============================================================================*/

#ifdef CONFIG_ADXL_UNIT_TEST

#include <stdlib.h>
#include <stdio.h>
#include "sns_ddf_driver_if.h"
#include "sns_ddf_attrib.h"
#include "fixed_point.h"
#include "sns_memmgr.h"

#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd.h"

#define PR_DBG(x...)

extern sns_ddf_driver_if_s sns_accel_adxl350_driver_fn_list;

/*
 * Check getting attributes.
 */
sns_ddf_status_e adxl350_get_all_attrib(sns_ddf_handle_t dd_handle,
                                       sns_ddf_memhandler_s memhandler)
{
    sns_ddf_status_e status;
    uint32_t num_elements = 0;
    sns_ddf_power_info_s *power_info = NULL;
    sns_ddf_range_s *range = NULL;
    sns_ddf_resolution_t *resolution = NULL;
    sns_ddf_lowpass_freq_t *freq = NULL;
    sns_ddf_delays_s *delay = NULL;
    sns_ddf_driver_info_s *driver_info = NULL;
    sns_ddf_device_info_s *device_info = NULL;
    int i;

    status = sns_accel_adxl350_driver_fn_list.get_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_POWER_INFO,
                                                         &memhandler,
                                                         (void **)&power_info,
                                                         &num_elements);

    if (status)
    {
        return status;
    }

    for (i = 0; i < num_elements; i++)
    {
        PR_DBG("ADXL350: active_current[i] = %d.\n",
               power_info->active_current);
    }

    status = sns_accel_adxl350_driver_fn_list.get_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_RANGE,
                                                         &memhandler,
                                                         (void **)&range,
                                                         &num_elements);
    if (status)
    {
        return status;
    }

    for (i = 0; i < num_elements; i++)
    {
        PR_DBG("ADXL350: range[i]: min=%d, max=%d.\n",
               range[i].min, range[i].max);
    }

    status = sns_accel_adxl350_driver_fn_list.get_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_RESOLUTION,
                                                         &memhandler,
                                                         (void **)&resolution,
                                                         &num_elements);
    if (status)
    {
        return status;
    }

    for (i = 0; i < num_elements; i++)
    {
        PR_DBG("ADXL350: resolution[i] = %d.\n", resolution[i]);
    }


    status = sns_accel_adxl350_driver_fn_list.get_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_LOWPASS,
                                                         &memhandler,
                                                         (void **)&freq,
                                                         &num_elements);
    if (status)
    {
        return status;
    }

    for (i = 0; i < num_elements; i++)
    {
        PR_DBG("ADXL350: low pass freq[i] = %d HZ.\n", freq[i]);
    }


    status = sns_accel_adxl350_driver_fn_list.get_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_DELAYS,
                                                         &memhandler,
                                                         (void **)&delay,
                                                         &num_elements);
    if (status)
    {
        return status;
    }

    for (i = 0; i < num_elements; i++)
    {
        PR_DBG("ADXL350: delay time_to_active= %d usec.\n",
               delay->time_to_active);
        PR_DBG("ADXL350: delay time_to_data= %d usec.\n",
               delay->time_to_data);
    }

    status = sns_accel_adxl350_driver_fn_list.get_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_DRIVER_INFO,
                                                         &memhandler,
                                                         (void **)&driver_info,
                                                         &num_elements);
    if (status)
    {
        return status;
    }

    for (i = 0; i < num_elements; i++)
    {
        PR_DBG("ADXL350: driver_info name=%s,version=%s.\n",
               driver_info->name, driver_info->version);
    }

    status = sns_accel_adxl350_driver_fn_list.get_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_DEVICE_INFO,
                                                         &memhandler,
                                                         (void **)&device_info,
                                                         &num_elements);
    if (status)
    {
        return status;
    }

    for (i = 0; i < num_elements; i++)
    {
        PR_DBG("ADXL350: device_info name=%s,vendor=%s,model=%s,version=%s.\n",
               device_info->name,
               device_info->vendor,
               device_info->model,
               device_info->version);

    }

    return SNS_DDF_SUCCESS;
}

/*
 * Check setting attributes.
 */
sns_ddf_status_e adxl350_set_all_attrib(sns_ddf_handle_t dd_handle)
{
    sns_ddf_status_e status;
    sns_ddf_powerstate_e power_state = 0;
    sns_ddf_resolution_t resolution = 0;
    uint32_t range_index = 0;
    uint32_t freq_index = 0;

    power_state = SNS_DDF_POWERSTATE_ACTIVE;
    status = sns_accel_adxl350_driver_fn_list.set_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_POWER_STATE,
                                                         &power_state);

    if (status)
    {
        return status;
    }

    power_state = SNS_DDF_POWERSTATE_LOWPOWER;
    status = sns_accel_adxl350_driver_fn_list.set_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_POWER_STATE,
                                                         &power_state);

    if (status)
    {
        return status;
    }

    power_state = SNS_DDF_POWERSTATE_ACTIVE;
    status = sns_accel_adxl350_driver_fn_list.set_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_POWER_STATE,
                                                         &power_state);

    if (status)
    {
        return status;
    }


    resolution = 10; /* Fix-resolution */
    status = sns_accel_adxl350_driver_fn_list.set_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_RESOLUTION,
                                                         &resolution);

    if (status)
    {
        return status;
    }

    resolution = 13; /* Full-resolution */
    status = sns_accel_adxl350_driver_fn_list.set_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_RESOLUTION,
                                                         &resolution);

    if (status)
    {
        return status;
    }

    range_index = 0; /* ±2g */
    status = sns_accel_adxl350_driver_fn_list.set_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_RANGE,
                                                         &range_index);

    if (status)
    {
        return status;
    }

    range_index = 3; /* ±16g */
    status = sns_accel_adxl350_driver_fn_list.set_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_RANGE,
                                                         &range_index);

    if (status)
    {
        return status;
    }



    freq_index = 0; /* 6.25 HZ */
    status = sns_accel_adxl350_driver_fn_list.set_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_LOWPASS,
                                                         &freq_index);

    if (status)
    {
        return status;
    }

    freq_index = 9; /* 3200 HZ */
    status = sns_accel_adxl350_driver_fn_list.set_attrib(dd_handle,
                                                         SNS_DDF_SENSOR_ACCEL,
                                                         SNS_DDF_ATTRIB_LOWPASS,
                                                         &freq_index);

    if (status)
    {
        return status;
    }

    return SNS_DDF_SUCCESS;
}

/*
 * Test main function.
 */
void adxl350_on_target_test_main(void)
{
    sns_ddf_device_access_s       device_info;
    sns_ddf_i2c_config_s          i2c_config;
    uint32_t num_sensors = 0;
    sns_ddf_sensor_e *sensors;
    sns_ddf_handle_t dd_handle = NULL;
    sns_ddf_handle_t smgr_handle = NULL;
    sns_ddf_nv_params_s *nv_params = NULL;
    sns_ddf_memhandler_s memhandler;
    sns_ddf_sensor_data_s  *data;

    sns_ddf_status_e status;
    int i;

    /* Device Info */
    device_info.device_select = 0;
    device_info.port_config.bus             = SNS_DDF_BUS_I2C;
    device_info.port_config.bus_config.i2c  = &i2c_config;

    /* I2C Info */
    i2c_config.addr_type            = SNS_DDF_I2C_ADDR_7BIT;
    i2c_config.bus_acq_timeout      = -1;
    i2c_config.xfer_timeout         = -1;
    i2c_config.bus_freq             = 400;
    i2c_config.dev_type             = SNS_DDF_I2C_DEVICE_REGADDR;
    i2c_config.read_opt             = SNS_DDF_I2C_START_BEFORE_RD;
    i2c_config.slave_addr           = 0xA6/2;

    /* get memory handler */
    sns_ddf_memhandler_init(&memhandler);

    status = sns_accel_adxl350_driver_fn_list.init(
                                                  &dd_handle,
                                                  smgr_handle,
                                                  nv_params,
                                                  &device_info,
                                                  1,
                                                  &memhandler,
                                                  &sensors,
                                                  &num_sensors);

    if (status != SNS_DDF_SUCCESS)
    {
        PR_DBG("error initializing\n", status);
        return;
    }

    PR_DBG("num_sensors = %d.\n", num_sensors);
    PR_DBG("sensor type is %d.\n", *sensors);

    for (i = 0; i < 3 ; i++)
    {
        status = sns_accel_adxl350_driver_fn_list.get_data(
                                                          dd_handle,
                                                          sensors,
                                                          num_sensors,
                                                          &memhandler,
                                                          &data);
    }

    status = adxl350_get_all_attrib(dd_handle, memhandler);

    if (status != SNS_DDF_SUCCESS)
    {
        PR_DBG("error get attrib.\n", status);
        return;
    }

    status = adxl350_set_all_attrib(dd_handle);

    if (status != SNS_DDF_SUCCESS)
    {
        PR_DBG("error set attrib.\n", status);
        return;
    }

    while (1)
    {
    }
}

#endif // CONFIG_ADXL_UNIT_TEST

