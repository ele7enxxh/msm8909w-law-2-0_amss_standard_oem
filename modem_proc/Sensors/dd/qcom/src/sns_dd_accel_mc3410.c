/*==============================================================================

    S E N S O R S   ACCELEROMETER  D R I V E R

DESCRIPTION

   Implements the driver for the MC3410 accelerometer Sensor

Copyright (c) 2013-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_mc3410.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
07/22/14     MW      Eliminated compiler warning
03/12/14     MW      Fixed KW issue
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
#include "sns_ddf_signal.h"
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include <string.h>
#include "sns_memmgr.h"
#include "sns_debug_str.h"
#include "sns_ddf_signal.h"
#include "sns_dd_inertial_test.h"

/*------------------------------------------------------
  Access to externalized functions.
------------------------------------------------------*/
extern sns_ddf_status_e sns_dd_acc_mc3410_init(
    sns_ddf_handle_t* dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors);

extern sns_ddf_status_e sns_dd_acc_mc3410_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems);

static sns_ddf_status_e sns_dd_acc_mc3410_set_range(
    sns_ddf_handle_t dd_handle);

static sns_ddf_status_e sns_dd_acc_mc3410_set_datarate(
    sns_ddf_handle_t dd_handle);

static sns_ddf_status_e sns_dd_acc_mc3410_config_motion_int(
    sns_ddf_handle_t dd_handle,
    bool                enable);
	
sns_ddf_driver_if_s sns_dd_acc_mc3410_if;

/**
 * Accelerometer MC3410 ranges array for setting attribute
 */
mc3410_range mc3410_ranges[MC3410_RANGE_NUM]={
    MC3410_RANGE_2G,
    MC3410_RANGE_4G,
    MC3410_RANGE_8G,};

/**
 * Accelerometer MC3410 sampling frequencies array for getting attribute
 */
sns_ddf_lowpass_freq_t mc3410_freqs[MC3410_ODR_NUM] = {
    FX_FLTTOFIX_Q16(16),
    FX_FLTTOFIX_Q16(32),
    FX_FLTTOFIX_Q16(64),
    FX_FLTTOFIX_Q16(128),
    FX_FLTTOFIX_Q16(256),
    FX_FLTTOFIX_Q16(512),
    FX_FLTTOFIX_Q16(1024)
};

/**
 * Accelerometer MC3410 sampling frequencies array for attribute(ODR)
 */
const sns_ddf_odr_t mc3410_odr_array[MC3410_ODR_NUM] = {
    16,
    32,
    64,
    128,
    256,
    512,
    1024};

/**
 * Accelerometer MC3410 sampling frequencies array for setting attribute
 */
mc3410_odr mc3410_odr_setting[MC3410_ODR_NUM] = {
    MC3410_ODR16,
    MC3410_ODR32,
    MC3410_ODR64,
    MC3410_ODR128,
    MC3410_ODR256,
    MC3410_ODR512,
    MC3410_ODR1024};

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_reset

DESCRIPTION  Resets the driver and device so they return to the state
                    they were in after init() was called.
============================================================================*/
sns_ddf_status_e sns_dd_acc_mc3410_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_acc_mc3410_state_t* state = (sns_dd_acc_mc3410_state_t *)dd_handle;
    sns_ddf_status_e status;
    uint8_t i;

    for (i = 0; i < MC3410_NUM_AXES; i++)
        {
           state->bias[i] = 0;
		   state->data_cache_old[i] = 0;
		}
	state->out_of_bound_flag = 0;
	memset(state->data_cache, 0, MC3410_NUM_AXES * sizeof(q16_t));
	state->e_i = 1;
    state->range = MC3410_RANGE_2G;
    state->sstvt_adj = MC3410_SSTVT_2G;
    state->data_rate = MC3410_ODR32;
    state->acc_cur_rate = 32;
    state->power_state = SNS_DDF_POWERSTATE_LOWPOWER;
    state->motion_detect_enabled = false;
    state->int_enable = false;

    status = sns_dd_acc_mc3410_set_range(dd_handle);
    if(status != SNS_DDF_SUCCESS)
        return status;

    status = sns_dd_acc_mc3410_set_datarate(dd_handle);
    if(status != SNS_DDF_SUCCESS)
        return status;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_probe

DESCRIPTION  Probe function for MC3410
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mc3410_probe(
    sns_ddf_device_access_s*  device_info,
    sns_ddf_memhandler_s*     memhandler,
    uint32_t*                 num_sensors,
    sns_ddf_sensor_e**        sensors )
{
    sns_ddf_status_e status;
    sns_ddf_handle_t port_handle;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    status = SNS_DDF_SUCCESS;
    *num_sensors = 0;
    *sensors = NULL;
	
    status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    /* Read & Verify Device ID */
    status = sns_ddf_read_port( port_handle,
        MC3410_PCODE,
        &rw_buffer,
        1,
        &rw_bytes );

    if(status != SNS_DDF_SUCCESS || rw_bytes != 1)
    {
        sns_ddf_close_port( port_handle );
        return status;
    }

    if (rw_buffer != MC3410)
    {
        sns_ddf_close_port( port_handle );
        return status;
    }

    /* This probe is successful */
    *num_sensors = 1;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
            sizeof(sns_ddf_sensor_e) * *num_sensors );
    if( *sensors != NULL )
    {
        (*sensors)[0] = SNS_DDF_SENSOR_ACCEL;
        status = SNS_DDF_SUCCESS;
    }
    else
    {
        status = SNS_DDF_ENOMEM;
    }

    sns_ddf_close_port( port_handle );
    return status;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_set_datarate

DESCRIPTION  Set ODR for MC3410
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mc3410_set_datarate(
    sns_ddf_handle_t dd_handle)
{
    sns_dd_acc_mc3410_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    uint8_t read_value = 0;
    uint8_t mode1,mode_rbuf;

    status = sns_ddf_read_port(state->port_handle,
        MC3410_OUTCFG,
        &read_value,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;

    rw_buffer = (uint8_t)((uint8_t)(state->data_rate << 4)) | (read_value & MC3410_DATAR_MASK);
    rw_buffer = rw_buffer | MC3410_BIT_MASK;
	/*change to standby*/
    if ( (status = sns_ddf_read_port(state->port_handle,
                  MC3410_MODE,
                  &mode_rbuf,
                  1,
                  &rw_bytes)) != SNS_DDF_SUCCESS)
			{
				return status;
			}

    mode1 = mode_rbuf | MC3410_STDBY_MASK;

    if ( (status = sns_ddf_write_port(state->port_handle,
                  MC3410_MODE,
                  &mode1,
                  1,
                  &rw_bytes)) != SNS_DDF_SUCCESS)
			{
				return status;
			}

	/*write to sensor */
    if ( (status = sns_ddf_write_port(state->port_handle,
                   MC3410_OUTCFG,
                   &rw_buffer,
                   1,
                   &rw_bytes)) != SNS_DDF_SUCCESS)
			{
				return status;
			}

	/*change to original mode*/
    if ( (status = sns_ddf_write_port(state->port_handle,
                   MC3410_MODE,
                   &mode_rbuf,
                   1,
                   &rw_bytes)) != SNS_DDF_SUCCESS)
			{
				return status;
			}

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_set_power

DESCRIPTION  Set power state for MC3410
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mc3410_set_power(
    sns_ddf_handle_t dd_handle,
    sns_ddf_powerstate_e power_state)
{
    sns_dd_acc_mc3410_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;

    if((SNS_DDF_POWERSTATE_LOWPOWER == power_state)
        &&(SNS_DDF_POWERSTATE_ACTIVE == state->power_state))
    {
        /*STANDBY mode*/
        rw_buffer = MC3410_STANDBY_MODE;
        status = sns_ddf_write_port(
            state->port_handle,
            MC3410_MODE,
            &rw_buffer,
            1,
            &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
            return status;

        state->power_state = SNS_DDF_POWERSTATE_LOWPOWER;

    }
    else if((SNS_DDF_POWERSTATE_ACTIVE == power_state)
        &&(SNS_DDF_POWERSTATE_LOWPOWER == state->power_state))
    {
        /*ACTIVE mode*/
        rw_buffer = MC3410_ACTIVE_MODE;
        status = sns_ddf_write_port(
            state->port_handle,
            MC3410_MODE,
            &rw_buffer,
            1,
            &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
            return status;

        state->power_state= SNS_DDF_POWERSTATE_ACTIVE;
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_set_range

DESCRIPTION  Set range for MC3410
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mc3410_set_range(
    sns_ddf_handle_t dd_handle)
{
    sns_dd_acc_mc3410_state_t* state = dd_handle;
    sns_ddf_status_e status;
    mc3410_sstvt sstvt;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    uint8_t read_value = 0;
    uint8_t mode1,mode_rbuf;

    /*set sensor sensitivity for data conversion*/
    switch (state->range) {
        case MC3410_RANGE_2G:
            sstvt = MC3410_SSTVT_2G;
            break;
        case MC3410_RANGE_4G:
            sstvt = MC3410_SSTVT_4G;
            break;
        case MC3410_RANGE_8G:
            sstvt = MC3410_SSTVT_8G;
            break;
        default:
            return SNS_DDF_EINVALID_PARAM;
    }

    if ( (status = sns_ddf_read_port(state->port_handle,
                  MC3410_OUTCFG,
                  &read_value,
                  1,
                  &rw_bytes)) != SNS_DDF_SUCCESS)
			{
				return status;
			}

    rw_buffer = ((state->range << 2)|(read_value & MC3410_RANGE_MASK));
	rw_buffer = rw_buffer | MC3410_BIT_MASK;
	
	/*change to standby*/
    if ( (status = sns_ddf_read_port(state->port_handle,
                  MC3410_MODE,
                  &mode_rbuf,
                  1,
                  &rw_bytes)) != SNS_DDF_SUCCESS)
			{
				return status;
			}

    mode1 = mode_rbuf | MC3410_STDBY_MASK;

    if ( (status = sns_ddf_write_port(state->port_handle,
                  MC3410_MODE,
                  &mode1,
                  1,
                  &rw_bytes)) != SNS_DDF_SUCCESS)
			{
				return status;
			}

	/*write to sensor */
    if ( (status = sns_ddf_write_port(state->port_handle,
                   MC3410_OUTCFG,
                   &rw_buffer,
                   1,
                   &rw_bytes)) != SNS_DDF_SUCCESS)
			{
				return status;
			}

	/*change to original mode*/
    if ( (status = sns_ddf_write_port(state->port_handle,
                   MC3410_MODE,
                   &mode_rbuf,
                   1,
                   &rw_bytes)) != SNS_DDF_SUCCESS)
			{
				return status;
			}

    state->sstvt_adj = sstvt;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_enable_irq

DESCRIPTION  Enable interrupt mode for MC3410
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mc3410_enable_irq(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{
    sns_dd_acc_mc3410_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    uint8_t rw_value = 0;
    uint8_t mode1,mode_rbuf,mc3410_read_mode;

    if (sensor != SNS_DDF_SENSOR_ACCEL)
		{
			return SNS_DDF_EINVALID_PARAM;
		}
    else
    {
        if (state->int_enable == TRUE)
			{
				return SNS_DDF_SUCCESS;
			}
        else
        {
            state->int_enable = TRUE;

            status = sns_ddf_signal_register(state->gpio_num,
                     dd_handle,
                     &sns_dd_acc_mc3410_if,
                     SNS_DDF_SIGNAL_IRQ_FALLING);
            if( status != SNS_DDF_SUCCESS )
                  return status;

			/*change to standby*/
            if ( (status = sns_ddf_read_port(state->port_handle,
                            MC3410_MODE,
                            &mode_rbuf,
                            1,
                            &rw_bytes)) != SNS_DDF_SUCCESS)
					{
						return status;
					}

            mode1 = mode_rbuf | MC3410_STDBY_MASK;

            if ( (status = sns_ddf_write_port(state->port_handle,
                            MC3410_MODE,
                            &mode1,
                            1,
                            &rw_bytes)) != SNS_DDF_SUCCESS)
					{
						return status;
					}

			/*write to sensor*/
            if ( (status = sns_ddf_read_port(state->port_handle,
							MC3410_OUTCFG,
							&mc3410_read_mode,
							1,
							&rw_bytes)) != SNS_DDF_SUCCESS)
					{
						return status;
					}
        
			mc3410_read_mode = mc3410_read_mode | MC3410_INTCON_MASK;

            if ( (status = sns_ddf_write_port(state->port_handle,
							MC3410_OUTCFG,
							&mc3410_read_mode,
							1,
							&rw_bytes)) != SNS_DDF_SUCCESS)
					{
						return status;
					}

            if ( (status = sns_ddf_read_port(state->port_handle,
                            MC3410_INTEN,
                            &rw_value,
                            1,
                            &rw_bytes)) != SNS_DDF_SUCCESS)
					{
						return status;
					}
				
            rw_buffer = rw_value | MC3410_INTEN_MASK;
			
            if ( (status = sns_ddf_write_port(state->port_handle,
                            MC3410_INTEN,
                            &rw_buffer,
                            1,
                            &rw_bytes)) != SNS_DDF_SUCCESS)
					{
						return status;
					}

			/*change to original mode*/
            if ( (status = sns_ddf_write_port(state->port_handle,
                            MC3410_MODE,
                            &mode_rbuf,
                            1,
                            &rw_bytes)) != SNS_DDF_SUCCESS)
					{
						return status;
					}
        }
    }
    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_disable_irq

DESCRIPTION  Disable interrupt mode for MC3410
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mc3410_disable_irq(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{
    sns_dd_acc_mc3410_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    uint8_t rw_value = 0;
    uint8_t mode1,mode_rbuf;

    if (sensor != SNS_DDF_SENSOR_ACCEL)
		{
			return SNS_DDF_EINVALID_PARAM;
		}
    else
    {
        if (state->int_enable == false || state->motion_detect_enabled == true)
			{
				return SNS_DDF_SUCCESS;
			}
        else
        {
            state->int_enable = FALSE;

            status = sns_ddf_signal_deregister(state->gpio_num);
            if(status != SNS_DDF_SUCCESS)
                return status;

			/*change to standby*/
            if ( (status = sns_ddf_read_port(state->port_handle,
                            MC3410_MODE,
                            &mode_rbuf,
                            1,
                            &rw_bytes)) != SNS_DDF_SUCCESS)
					{
						return status;
					}

            mode1 = mode_rbuf | MC3410_STDBY_MASK;

            if ( (status = sns_ddf_write_port(state->port_handle,
                            MC3410_MODE,
                            &mode1,
                            1,
                            &rw_bytes)) != SNS_DDF_SUCCESS)
					{
						return status;
					}

			/*write to sensor*/
            if ( (status = sns_ddf_read_port(state->port_handle,
                            MC3410_INTEN,
                            &rw_value,
                            1,
                            &rw_bytes)) != SNS_DDF_SUCCESS)
					{
						return status;
					}
				
            rw_buffer = rw_value & ~(MC3410_INTEN_MASK);
			
            if ( (status = sns_ddf_write_port(state->port_handle,
                            MC3410_INTEN,
                            &rw_buffer,
                            1,
                            &rw_bytes)) != SNS_DDF_SUCCESS)
					{
						return status;
					}

			/*change to original mode*/
            if ( (status = sns_ddf_write_port(state->port_handle,
                            MC3410_MODE,
                            &mode_rbuf,
                            1,
                            &rw_bytes)) != SNS_DDF_SUCCESS)
					{
						return status;
					}
        }
    }
    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_enable_sched_data

DESCRIPTION  Implement enable_sched_data() DDF API.
============================================================================*/
sns_ddf_status_e sns_dd_acc_mc3410_enable_sched_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    bool enable)
{
    sns_dd_acc_mc3410_state_t* state = dd_handle;

    if(enable)
        return sns_dd_acc_mc3410_enable_irq(state, sensor);
    else
        return sns_dd_acc_mc3410_disable_irq(state, sensor);
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_log_data

DESCRIPTION  Logs latest set of sensor data sampled from the sensor.
============================================================================*/
void sns_dd_acc_mc3410_log_data(sns_ddf_sensor_data_s *accel_data_ptr)
{
    sns_err_code_e err_code;
    sns_log_sensor_data_pkt_s* log_struct_ptr;

    err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                               sizeof(sns_log_sensor_data_pkt_s),
                               (void**)&log_struct_ptr);

    if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
    {
        log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log_struct_ptr->sensor_id = SNS_DDF_SENSOR_ACCEL;
        log_struct_ptr->vendor_id = SNS_DDF_VENDOR_STMICRO;
        log_struct_ptr->timestamp = accel_data_ptr->timestamp;
        log_struct_ptr->num_data_types = MC3410_NUM_AXES;
        log_struct_ptr->data[0]  = accel_data_ptr->samples[0].sample;
        log_struct_ptr->data[1]  = accel_data_ptr->samples[1].sample;
        log_struct_ptr->data[2]  = accel_data_ptr->samples[2].sample;

        (void) sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                             log_struct_ptr);
    }
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_get_accel_data

DESCRIPTION  Get the datas from the sensor.
============================================================================*/
sns_ddf_status_e sns_dd_acc_mc3410_get_accel_data(sns_dd_acc_mc3410_state_t* state)
{
    uint8_t read_buffer[MC3410_NUM_READ_BYTES] = { 0 };
    uint8_t read_count = 0;
    sns_ddf_status_e status;
    int16_t hw_d[MC3410_NUM_AXES] = { 0 };

   if(SNS_DDF_POWERSTATE_LOWPOWER == state->power_state)
        return SNS_DDF_EDEVICE;

    status = sns_ddf_read_port(
			state->port_handle,
			MC3410_XOUT_EX_L,
			read_buffer,
			MC3410_NUM_READ_BYTES,
			&read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;

    hw_d[0] = (int16_t) (read_buffer[0] | (read_buffer[1] << 8));
    hw_d[1] = (int16_t) (read_buffer[2] | (read_buffer[3] << 8));
    hw_d[2] = (int16_t) (read_buffer[4] | (read_buffer[5] << 8));

    state->data_cache[0] = FX_FLTTOFIX_Q16(hw_d[0] * 2 * state->sstvt_adj * G/1024);
    state->data_cache[1] = FX_FLTTOFIX_Q16(hw_d[1] * 2 * state->sstvt_adj * G/1024);
    state->data_cache[2] = FX_FLTTOFIX_Q16(hw_d[2] * 2 * state->sstvt_adj * G/1024);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_report_data

DESCRIPTION  Report the datas when in irq mode.
============================================================================*/
sns_ddf_status_e sns_dd_acc_mc3410_report_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_time_t    timestamp)
{
    static sns_ddf_sensor_data_s   *sensor_data;
    sns_dd_acc_mc3410_state_t* state = dd_handle;
    sns_ddf_status_e status;

    status = sns_dd_acc_mc3410_get_accel_data(state);
    if(status != SNS_DDF_SUCCESS)
        return status;

    sns_ddf_map_axes(&state->axes_map, state->data_cache);

    if ((sns_ddf_malloc((void **)&sensor_data, sizeof(sns_ddf_sensor_data_s))) != SNS_DDF_SUCCESS )
        return SNS_DDF_EFAIL;

    sensor_data->sensor = SNS_DDF_SENSOR_ACCEL;
    sensor_data->status = SNS_DDF_SUCCESS;
    sensor_data->timestamp = timestamp;

    if((sns_ddf_malloc((void **)&sensor_data->samples, MC3410_NUM_AXES * sizeof(sns_ddf_sensor_sample_s))) != SNS_DDF_SUCCESS )
        return SNS_DDF_EFAIL;

    sensor_data->samples[0].sample = state->data_cache[0];
    sensor_data->samples[0].status = SNS_DDF_SUCCESS;
    sensor_data->samples[1].sample = state->data_cache[1];
    sensor_data->samples[1].status = SNS_DDF_SUCCESS;
    sensor_data->samples[2].sample = state->data_cache[2];
    sensor_data->samples[2].status = SNS_DDF_SUCCESS;
    sensor_data->num_samples = MC3410_NUM_AXES;

    sns_dd_acc_mc3410_log_data(sensor_data);

    status =  sns_ddf_smgr_notify_data(state->smgr_hndl, sensor_data, 1);
    if(status != SNS_DDF_SUCCESS)
        return status;

    sns_ddf_mfree(sensor_data->samples);
    sns_ddf_mfree(sensor_data);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_handle_irq

DESCRIPTION  Called in response to an motion detection interrupt for this driver.
============================================================================*/
void sns_dd_acc_mc3410_handle_irq(
    sns_ddf_handle_t  dd_handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp)
{
    sns_ddf_status_e status;
	uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
	
	sns_dd_acc_mc3410_state_t* state = dd_handle;
	
    status = sns_ddf_read_port(state->port_handle,
			MC3410_TILT,
			&rw_buffer,
			1,
			&rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return;

	if((rw_buffer >> 7)==0x01)
	{
	    sns_dd_acc_mc3410_config_motion_int(state,0);
        status = sns_ddf_signal_deregister(gpio_num);
        if(status != SNS_DDF_SUCCESS)
            return;

         sns_ddf_smgr_notify_event(state->smgr_hndl,
               SNS_DDF_SENSOR_ACCEL,
               SNS_DDF_EVENT_MOTION);
	}
	else
	{
		status = sns_dd_acc_mc3410_report_data(dd_handle, timestamp);
			if(status != SNS_DDF_SUCCESS)
				return;
	}
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_get_data

DESCRIPTION  Retrieves a single set of sensor data from mc3410.
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mc3410_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data)
{
    uint8_t read_buffer[MC3410_NUM_READ_BYTES] = { 0 };
    uint8_t read_count = 0;
	//uint8_t i;
    sns_ddf_time_t timestamp;
    sns_ddf_status_e status;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_acc_mc3410_state_t *state = dd_handle;
    int16_t hw_d[MC3410_NUM_AXES] = { 0 };
    q16_t sae_d[MC3410_NUM_AXES] = { 0 };
	//i = state->e_i;
	state->e_i = (state->e_i == 1)? 0 : 1;
    if((SNS_DDF_POWERSTATE_LOWPOWER == state->power_state)||(num_sensors!=1))
        return SNS_DDF_EDEVICE;

    status = sns_ddf_read_port(
			state->port_handle,
			MC3410_XOUT_EX_L,
			read_buffer,
			MC3410_NUM_READ_BYTES,
			&read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;

    timestamp = sns_ddf_get_timestamp();

    hw_d[0] = (int16_t) (read_buffer[0] | (read_buffer[1] << 8));
    hw_d[1] = (int16_t) (read_buffer[2] | (read_buffer[3] << 8));
    hw_d[2] = (int16_t) (read_buffer[4] | (read_buffer[5] << 8));

    sae_d[0] = FX_FLTTOFIX_Q16(hw_d[0] * 2 * state->sstvt_adj * G/1024);
    sae_d[1] = FX_FLTTOFIX_Q16(hw_d[1] * 2 * state->sstvt_adj * G/1024);
    sae_d[2] = FX_FLTTOFIX_Q16(hw_d[2] * 2 * state->sstvt_adj * G/1024);

    sns_ddf_map_axes(&state->axes_map, sae_d);

    state->out_of_bound_flag = 0;

    data_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_data_s));
    if(NULL == data_ptr)
    {
        return SNS_DDF_ENOMEM;
    }
    *data = data_ptr;

    data_ptr->sensor = sensors[0];
    data_ptr->status = SNS_DDF_SUCCESS;
    data_ptr->timestamp = timestamp;

    data_ptr->samples = sns_ddf_memhandler_malloc(memhandler, MC3410_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
    if(NULL == data_ptr->samples)
    {
       return SNS_DDF_ENOMEM;
    }

    data_ptr->samples[0].sample = sae_d[0];
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = sae_d[1];
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->samples[2].sample = sae_d[2];
    data_ptr->samples[2].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples = MC3410_NUM_AXES;

    sns_dd_acc_mc3410_log_data(data_ptr);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_match_odr

DESCRIPTION  Match the freqs of ODR.
============================================================================*/
sns_ddf_status_e  sns_dd_acc_mc3410_match_odr(
    sns_ddf_odr_t desired_rate,
    sns_ddf_odr_t *new_rate,
    uint32_t *new_index)
{
    uint8_t idx = 0;

    if((0 == desired_rate) || (desired_rate > MC3410_MAX_ODR))
        return SNS_DDF_EINVALID_PARAM;

    for(idx=0; idx < MC3410_ODR_NUM; idx++)
    {
        if(desired_rate <= mc3410_odr_array[idx])
            break;
    }

    if(idx >= MC3410_ODR_NUM)
    {
        // if desired_rate > max supported ODR
        // use max ODR
        idx = (MC3410_ODR_NUM - 1);
    }

    *new_rate = mc3410_odr_array[idx];
    *new_index = idx;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_config_motion_detect

DESCRIPTION  Configure the motion interrupt threshold
============================================================================*/

static sns_ddf_status_e sns_dd_acc_mc3410_config_motion_detect(
    sns_ddf_handle_t dd_handle
)
{
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
	uint8_t mode1,mode_rbuf;
    sns_dd_acc_mc3410_state_t* state = dd_handle;

    rw_buffer = MC3410_ADJACENT_SK;

	/*change to standby*/
    if ( (status = sns_ddf_read_port(state->port_handle,
                MC3410_MODE,
                &mode_rbuf,
                1,
                &rw_bytes)) != SNS_DDF_SUCCESS)
            {
                return status;
            }

    mode1 = mode_rbuf | MC3410_STDBY_MASK;

    if ( (status = sns_ddf_write_port(state->port_handle,
                MC3410_MODE,
                &mode1,
                1,
                &rw_bytes)) != SNS_DDF_SUCCESS)
            {
                return status;
            }
			
	/*write to sensor*/		
	if ( (status = sns_ddf_write_port(state->port_handle,
                MC3410_SHDB,
                &rw_buffer,
                1,
                &rw_bytes)) != SNS_DDF_SUCCESS)
            {
                return status;
            }	
			
	rw_buffer = MC3410_SK_THRESHOLD;
	
	if ( (status = sns_ddf_write_port(state->port_handle,
               MC3410_SHAKE_TH,
               &rw_buffer,
               1,
               &rw_bytes)) != SNS_DDF_SUCCESS)
            {
                return status;
            }
	/*change to original mode*/
    if ( (status = sns_ddf_write_port(state->port_handle,
                MC3410_MODE,
                &mode_rbuf,
                1,
                &rw_bytes)) != SNS_DDF_SUCCESS)
            {
                return status;
            }

    return  SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_config_motion_int

DESCRIPTION  Configure (enable/disable) the motion interrupt
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mc3410_config_motion_int(
    sns_ddf_handle_t dd_handle,
    bool                        enable
)
{
    sns_dd_acc_mc3410_state_t* state = dd_handle;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
	uint8_t mode1,mode_rbuf;
	
    status = sns_ddf_read_port(state->port_handle,
			  MC3410_INTEN,
			  &rw_buffer,
			  1,
			  &rw_bytes);
    if ( status == SNS_DDF_SUCCESS)
    {
        if (enable)
        {
            rw_buffer |= MC3410_SHAKE_INT;
        }
        else
        {
            rw_buffer &= ~(MC3410_SHAKE_INT);
        }
		/*change to standby*/
		if ( (status = sns_ddf_read_port(state->port_handle,
					MC3410_MODE,
					&mode_rbuf,
					1,
					&rw_bytes)) != SNS_DDF_SUCCESS)
            {
                return status;
            }

		mode1 = mode_rbuf | MC3410_STDBY_MASK;

		if ( (status = sns_ddf_write_port(state->port_handle,
					MC3410_MODE,
					&mode1,
					1,
					&rw_bytes)) != SNS_DDF_SUCCESS)
            {
                return status;
            }
			
		/*write to sensor*/		
		if ( (status = sns_ddf_write_port(state->port_handle,
					MC3410_INTEN,
					&rw_buffer,
					1,
					&rw_bytes)) != SNS_DDF_SUCCESS)
            {
                return status;
            }		
			
		/*change to original mode*/
		if ( (status = sns_ddf_write_port(state->port_handle,
					MC3410_MODE,
					&mode_rbuf,
					1,
					&rw_bytes)) != SNS_DDF_SUCCESS)
            {
                return status;
            }
    }

    state->motion_detect_enabled = enable;

   return status;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_set_attr

DESCRIPTION  Sets a mc3410 sensor attribute to a specific value.
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mc3410_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    sns_ddf_odr_t new_rate = 0;
    uint32_t new_rate_index = 0;
    sns_dd_acc_mc3410_state_t* state = dd_handle;

    if((sensor != SNS_DDF_SENSOR_ACCEL) && (sensor != SNS_DDF_SENSOR__ALL))
        return SNS_DDF_EINVALID_PARAM;

    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_STATE:
        {
            sns_ddf_powerstate_e* power_state = value;
            return sns_dd_acc_mc3410_set_power(dd_handle, *power_state);
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            uint32_t* range_idx = value;
            state->range = mc3410_ranges[*range_idx];
            if(state->range > MC3410_MAX_RANGE)
                return SNS_DDF_EINVALID_PARAM;

            return sns_dd_acc_mc3410_set_range(dd_handle);
        }
		
        case SNS_DDF_ATTRIB_ODR:
        {
            if(sensor == SNS_DDF_SENSOR_ACCEL)
            {
                status =  sns_dd_acc_mc3410_match_odr(*(sns_ddf_odr_t*)value,
                    &new_rate,
                    &new_rate_index);
            }
            if(status != SNS_DDF_SUCCESS)
                return status;

            if(new_rate != state->acc_cur_rate)
            {
                state->data_rate = mc3410_odr_setting[new_rate_index];
                status = sns_dd_acc_mc3410_set_datarate(dd_handle);
                if(SNS_DDF_SUCCESS == status)
                {
                    state->acc_cur_rate = new_rate;
                    return status;
                }
                else
                {
                    return status;
                }
            }
            else
            {
                return SNS_DDF_SUCCESS;
            }
        }
		
		case SNS_DDF_ATTRIB_LOWPASS:
        {
			if(*((uint8_t *)value) > MC3410_MAXODR_INDEX)
			{
				return SNS_DDF_EINVALID_ATTR;
			}
            state->data_rate = mc3410_odr_setting[*((uint8_t *)value)];
            return sns_dd_acc_mc3410_set_datarate(dd_handle);
        }

		case SNS_DDF_ATTRIB_MOTION_DETECT:
		{
            bool* enable = value;
            if(!sns_ddf_signal_irq_enabled())
            {
                return SNS_DDF_EINVALID_PARAM;
            }

            if(*enable)
            {
                status = sns_dd_acc_mc3410_config_motion_detect(dd_handle);
                if ( status  != SNS_DDF_SUCCESS)
                    return status;

                status = sns_dd_acc_mc3410_config_motion_int(dd_handle, 1);
                if ( status  != SNS_DDF_SUCCESS)
                    return status;

                return sns_ddf_signal_register(state->gpio_num,
                    dd_handle,
                    &sns_dd_acc_mc3410_if,
                    SNS_DDF_SIGNAL_IRQ_FALLING);
            }
            else
            {
                if(state->motion_detect_enabled)
                {
                    status = sns_dd_acc_mc3410_config_motion_int(dd_handle, 0);
                    if (status != SNS_DDF_SUCCESS)
                        return status;

                    return sns_ddf_signal_deregister(state->gpio_num);
                }
                else
                {
                    return SNS_DDF_SUCCESS;
                }
            }
        }
        default:
            return SNS_DDF_EINVALID_PARAM;
    }
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_test_notify

DESCRIPTION  Runs bias test as part of OEM selftest and returns the test result.
============================================================================*/
static void sns_dd_acc_mc3410_test_notify(
    sns_ddf_handle_t handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_status_e status,
    uint32_t err,
    q16_t* biases)
{
    static const uint8_t num_axes = 3;
    sns_dd_acc_mc3410_state_t *state = handle;
    q16_t* biases_ptr;
    uint8_t i;

    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
        sensor,
        status,err);
    if (status == SNS_DDF_SUCCESS)
    {
        biases_ptr = state->bias;

        for (i = 0; i < num_axes; i++)
        biases_ptr[i] = biases[i];

        sns_ddf_smgr_notify_event(handle,
            sensor,
            SNS_DDF_EVENT_BIAS_READY);
    }
}

/*===========================================================================
FUNCTION      sns_dd_acc_mc3410_test_notify

DESCRIPTION  Runs self test and returns the test result.
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mc3410_self_test(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_test_e test,
    uint32_t* err)
{
    sns_dd_acc_mc3410_state_t *state = dd_handle;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    sns_dd_inertial_test_config_s test_config;
    *err = 0;

    if(test == SNS_DDF_TEST_OEM)
    {
        test_config.sample_rate =  16;
        test_config.num_samples =  64;
        test_config.max_variance = (int64_t)(FX_CONV((FX_FLTTOFIX_Q16(0.3)),16,32));
        test_config.bias_thresholds[0] = FX_FLTTOFIX_Q16(0.2*G);
        test_config.bias_thresholds[1] = FX_FLTTOFIX_Q16(0.2*G);
        test_config.bias_thresholds[2] = FX_FLTTOFIX_Q16(0.26*G);

        sns_dd_acc_mc3410_reset(dd_handle);

        status = sns_dd_inertial_test_run(
                  sensor,
                  dd_handle,
                  &sns_dd_acc_mc3410_if,
                  &test_config,
                  &sns_dd_acc_mc3410_test_notify);

        if(status != SNS_DDF_SUCCESS)
            return status;
        return SNS_DDF_SUCCESS;
    }
    else if(test == SNS_DDF_TEST_SELF)
    {
        status = sns_ddf_read_port(state->port_handle, MC3410_CHIP_ID, &rw_buffer, 1,&rw_bytes);
        if( status != SNS_DDF_SUCCESS ) return status;
        status = sns_ddf_read_port(state->port_handle, MC3410_OPSTAT, &rw_buffer, 1,&rw_bytes);
        if( status != SNS_DDF_SUCCESS ) return status;
        status = sns_ddf_read_port(state->port_handle, MC3410_PCODE, &rw_buffer, 1,&rw_bytes);
        if( status != SNS_DDF_SUCCESS ) return status;
        status = sns_ddf_read_port(state->port_handle, MC3410_OUTCFG, &rw_buffer, 1,&rw_bytes);
        if( status != SNS_DDF_SUCCESS ) return status;
        status = sns_ddf_read_port(state->port_handle, MC3410_TILT, &rw_buffer, 1,&rw_bytes);
        if( status != SNS_DDF_SUCCESS ) return status;
        status = sns_ddf_read_port(state->port_handle, MC3410_INTEN, &rw_buffer, 1,&rw_bytes);
        if( status != SNS_DDF_SUCCESS ) return status;
    }
    else
    {
        return SNS_DDF_EINVALID_TEST;
    }
    return SNS_DDF_SUCCESS;
}

sns_ddf_driver_if_s sns_dd_acc_mc3410_if =
{
    .init = &sns_dd_acc_mc3410_init,
    .get_data = &sns_dd_acc_mc3410_get_data,
    .set_attrib = &sns_dd_acc_mc3410_set_attr,
    .get_attrib = &sns_dd_acc_mc3410_get_attr,
    .handle_timer = NULL,
    .handle_irq = &sns_dd_acc_mc3410_handle_irq,
    .reset = &sns_dd_acc_mc3410_reset,
    .run_test = &sns_dd_acc_mc3410_self_test,
    .enable_sched_data = &sns_dd_acc_mc3410_enable_sched_data,
    .probe = &sns_dd_acc_mc3410_probe
};
