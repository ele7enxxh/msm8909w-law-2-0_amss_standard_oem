/*  Date: 2013/08/05 10:00:00
 *  Revision: 1.0
 */

/*==============================================================================

    S E N S O R S   ACCELEROMETER  D R I V E R

DESCRIPTION

   Implements the driver for the MMA8452 accelerometer Sensor

==============================================================================*/

/*​---------------------------------------​--------------------------------------
 * Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------​------------------------------------*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_mma8452.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
07/09/15     AH      Fix compiler warnings
03/12/14     MW      Fixed KW issues
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
Extern Variables
------------------------------------------------------*/
extern volatile boolean EnableI2C;

/*------------------------------------------------------
  Access to externalized functions.
------------------------------------------------------*/
extern sns_ddf_status_e sns_dd_acc_mma8452_init(
    sns_ddf_handle_t* dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors);

extern sns_ddf_status_e sns_dd_acc_mma8452_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems);

static sns_ddf_status_e sns_dd_acc_mma8452_set_range(
    sns_ddf_handle_t dd_handle);

static sns_ddf_status_e sns_dd_acc_mma8452_set_datarate(
    sns_ddf_handle_t dd_handle);

static sns_ddf_status_e sns_dd_acc_mma8452_config_motion_int(
    sns_ddf_handle_t dd_handle,
    bool                        enable);

sns_ddf_driver_if_s sns_dd_acc_mma8452_if;


/**
 * Accelerometer MMA8452 ranges array for setting attribute
 */
mma8452_range mma8452_ranges[MMA8452_RANGE_NUM]={
    MMA8452_RANGE_2G,
    MMA8452_RANGE_4G,
    MMA8452_RANGE_8G,};

/**
 * Accelerometer MMA8452 sampling frequencies array for getting attribute
 */
sns_ddf_lowpass_freq_t mma8452_freqs[MMA8452_ODR_NUM] = {
    FX_FLTTOFIX_Q16(1.65),
    FX_FLTTOFIX_Q16(6.25),
    FX_FLTTOFIX_Q16(12.5),
    FX_FLTTOFIX_Q16(50),
    FX_FLTTOFIX_Q16(100),
    FX_FLTTOFIX_Q16(200),
    FX_FLTTOFIX_Q16(400),
    FX_FLTTOFIX_Q16(800)
};


/**
 * Accelerometer MMA8452 sampling frequencies array for attribute(ODR)
 */
const sns_ddf_odr_t mma8452_odr_array[MMA8452_ODR_NUM] = {
    1,
    6,
    12,
    50,
    100,
    200,
    400,
    800};


/**
 * Accelerometer MMA8452 sampling frequencies array for setting attribute
 */
mma8452_odr mma8452_odr_setting[MMA8452_ODR_NUM] = {
    MMA8452_ODR1_65,
    MMA8452_ODR6_25,
    MMA8452_ODR12_5,
    MMA8452_ODR50,
    MMA8452_ODR100,
    MMA8452_ODR200,
    MMA8452_ODR400,
    MMA8452_ODR800};


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_writer_register

DESCRIPTION  Write the register of MMA8452
============================================================================*/
 sns_ddf_status_e sns_dd_acc_mma8452_writer_register(
    sns_ddf_handle_t  dd_handle,
    uint16_t          reg_addr,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          read_count)
{
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    uint8_t pw_status = 0;
    sns_dd_acc_mma8452_state_t* state = dd_handle;

    pw_status = state->power_state;
    /*standby*/
    if(pw_status != SNS_DDF_POWERSTATE_LOWPOWER)
    {
        status = sns_ddf_read_port(state->port_handle,
                MMA8452_CTRL_REG1,
                &rw_buffer,
                1,
                &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(rw_bytes != 1)
            return SNS_DDF_EBUS;

        rw_buffer &=  MMA8452_STANDBY;

        status = sns_ddf_write_port(state->port_handle,
                MMA8452_CTRL_REG1,
                &rw_buffer,
                1,
                &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(rw_bytes != 1)
            return SNS_DDF_EBUS;
    }

    /*write register*/
    status = sns_ddf_write_port(state->port_handle,
            reg_addr,
            buffer,
            bytes,
            read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    /*active*/
    if(pw_status != SNS_DDF_POWERSTATE_LOWPOWER)
    {
        status = sns_ddf_read_port(state->port_handle,
                MMA8452_CTRL_REG1,
                &rw_buffer,
                1,
                &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
                return status;
        if(rw_bytes != 1)
                return SNS_DDF_EBUS;

        rw_buffer |= MMA8452_ACTIVE;

        status = sns_ddf_write_port(state->port_handle,
                MMA8452_CTRL_REG1,
                &rw_buffer,
                1,
                &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(rw_bytes != 1)
            return SNS_DDF_EBUS;
    }
    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_reset

DESCRIPTION  Resets the driver and device so they return to the state
                    they were in after init() was called.
============================================================================*/
sns_ddf_status_e sns_dd_acc_mma8452_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_acc_mma8452_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer[5] = { 0 };
    uint8_t rw_bytes = 0;
    /** Reset values for the 5 control registers. */
    static const uint8_t control_regs[5] = {
      /* CTRL_REG1 */
      0x0
      | (0<<6)
      | (0<<5)
      | (0<<4)
      | (0<<3)
      | (0<<2)
      | (0<<1)
      | 1,
      /* CTRL_REG2 */
      0x0
      | (0<<7)
      | (0<<6)
      | (0<<3)
      | (0<<2)
      | 0,
      /* CTRL_REG3 */
      0x0
      | (0<<7)
      | (0<<6)
      | (0<<5)
      | (0<<4)
      | (0<<3)
      | (0<<2)
      | (0<<1)
      | 0,
      /* CTRL_REG4 */
      0x0
      | (0<<7)
      | (0<<6)
      | (0<<5)
      | (0<<4)
      | (0<<3)
      | (0<<2)
      | (0<<1)
      | 0,
      /* CTRL_REG5 */
      0x0
      | (0<<7)
      | (0<<6)
      | (0<<5)
      | (0<<4)
      | (0<<3)
      | (0<<2)
      | (0<<1)
      | 0
    };

    /*Reset driver state.*/
    memset(state->bias, 0, MMA8452_NUM_AXES * sizeof(q16_t));
    state->out_of_bound_flag = 0;
    memset(state->data_cache, 0, MMA8452_NUM_AXES * sizeof(q16_t));

    /*Configure accelerometer control register REG1-REG5*/
    SNS_OS_MEMCOPY(rw_buffer, control_regs, 5*sizeof(uint8_t));

    status = sns_ddf_write_port(
        state->port_handle,
        MMA8452_CTRL_REG1,
        rw_buffer,
        5,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 5)
        return SNS_DDF_EBUS;

    state->range = MMA8452_RANGE_2G;
    state->sstvt_adj = MMA8452_SSTVT_2G;
    state->data_rate = MMA8452_ODR50;
    state->acc_cur_rate = 50;
    state->power_state = SNS_DDF_POWERSTATE_ACTIVE;
    state->motion_detect_enabled = false;
    state->int_enable = false;

    status = sns_dd_acc_mma8452_set_range(dd_handle);
    if(status != SNS_DDF_SUCCESS)
        return status;

    status = sns_dd_acc_mma8452_set_datarate(dd_handle);
    if(status != SNS_DDF_SUCCESS)
        return status;

    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_probe

DESCRIPTION  Probe function for MMA8452
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mma8452_probe(
    sns_ddf_device_access_s*  device_info,
    sns_ddf_memhandler_s*     memhandler,
    uint32_t*                 num_sensors,
    sns_ddf_sensor_e**        sensors )
{
    /*
    * Initialize data before we begin probing for device existance.
    */
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
        MMA8452_WHO_AM_I,
        &rw_buffer,
        1,
        &rw_bytes );

    if(status != SNS_DDF_SUCCESS || rw_bytes != 1)
    {
        sns_ddf_close_port( port_handle );
        return status;
    }

    if (rw_buffer != CHIP_ID_MMA8452)
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
FUNCTION      sns_dd_acc_mma8452_set_datarate

DESCRIPTION  Set ODR for MMA8452
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mma8452_set_datarate(
    sns_ddf_handle_t dd_handle)
{
    sns_dd_acc_mma8452_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    uint8_t read_value = 0;

    /*Configure ODR in CTRL_REG1*/
    status = sns_ddf_read_port(state->port_handle,
        MMA8452_CTRL_REG1,
        &read_value,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    rw_buffer = (uint8_t)((uint8_t)(state->data_rate << 3)) | (read_value & MMA8452_ODR_CFG);

    status = sns_dd_acc_mma8452_writer_register(dd_handle,
        MMA8452_CTRL_REG1,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_set_power

DESCRIPTION  Set power state for MMA8452
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mma8452_set_power(
    sns_ddf_handle_t dd_handle,
    sns_ddf_powerstate_e power_state)
{
    sns_dd_acc_mma8452_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;

    if((SNS_DDF_POWERSTATE_LOWPOWER == power_state)
        &&(SNS_DDF_POWERSTATE_ACTIVE == state->power_state))
    {
        /*STANDBY mode*/
        rw_buffer = 0x0
            | (0<<6)
            | (0<<5)
            | (0<<4)
            | (0<<3)
            | (0<<2)
            | (0<<1)
            | 0,               /*selects between STANDBY mode and ACTIVE mode*/

        status = sns_ddf_write_port(
            state->port_handle,
            MMA8452_CTRL_REG1,
            &rw_buffer,
            1,
            &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(rw_bytes != 1)
            return SNS_DDF_EBUS;

        state->power_state = SNS_DDF_POWERSTATE_LOWPOWER;

    }
    else if((SNS_DDF_POWERSTATE_ACTIVE == power_state)
        &&(SNS_DDF_POWERSTATE_LOWPOWER == state->power_state))
    {
        /*ACTIVE mode*/
        rw_buffer = 0x0
            | (0<<6)
            | (0<<5)
            | (0<<4)
            | (0<<3)
            | (0<<2)
            | (0<<1)
            | 1,               /*selects between STANDBY mode and ACTIVE mode*/

        status = sns_ddf_write_port(
            state->port_handle,
            MMA8452_CTRL_REG1,
            &rw_buffer,
            1,
            &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(rw_bytes != 1)
            return SNS_DDF_EBUS;

        state->power_state= SNS_DDF_POWERSTATE_ACTIVE;
    }

    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_set_range

DESCRIPTION  Set range for MMA8452
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mma8452_set_range(
    sns_ddf_handle_t dd_handle)
{
    sns_dd_acc_mma8452_state_t* state = dd_handle;
    sns_ddf_status_e status;
    mma8452_sstvt sstvt;	
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    uint8_t read_value = 0;

    /*set sensor sensitivity for data conversion*/
    switch (state->range) {
        case MMA8452_RANGE_2G:
            sstvt = MMA8452_SSTVT_2G;
            break;
        case MMA8452_RANGE_4G:
            sstvt = MMA8452_SSTVT_4G;
            break;
        case MMA8452_RANGE_8G:
            sstvt = MMA8452_SSTVT_8G;
            break;
        default:
            return SNS_DDF_EINVALID_PARAM;
    }

    status = sns_ddf_read_port(state->port_handle,
        MMA8452_XYZ_DATA_CFG,
        &read_value,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    rw_buffer = (uint8_t)((state->range)|(read_value & MMA8452_RANGE_CFG));

    status = sns_dd_acc_mma8452_writer_register(dd_handle,
        MMA8452_XYZ_DATA_CFG,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    state->sstvt_adj = sstvt;

    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_enable_irq

DESCRIPTION  Enable interrupt mode for MMA8452
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mma8452_enable_irq(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{
    sns_dd_acc_mma8452_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;

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
            if(sns_ddf_signal_irq_enabled())
            {
                  /* IRQ's are enabled, register the irq*/
                 status = sns_ddf_signal_register(state->gpio_num,
                         dd_handle,
                         &sns_dd_acc_mma8452_if,
                         SNS_DDF_SIGNAL_IRQ_LOW);
                 if( status != SNS_DDF_SUCCESS )
                     return status;
            }
            else
                 return SNS_DDF_EFAIL;

        rw_buffer = 0
            | 1,               /*Enable INT_2 PIN.*/

        status = sns_dd_acc_mma8452_writer_register(dd_handle,
                MMA8452_CTRL_REG4,
                &rw_buffer,
                1,
                &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
                return status;
        if(rw_bytes != 1)
                return SNS_DDF_EBUS;
        }
    }
    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_disable_irq

DESCRIPTION  Disable interrupt mode for MMA8452
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mma8452_disable_irq(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor)
{
    sns_dd_acc_mma8452_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;

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

            rw_buffer = 0
                | 0,               /*Disable INT_2 PIN.*/

            status = sns_dd_acc_mma8452_writer_register(dd_handle,
                    MMA8452_CTRL_REG4,
                    &rw_buffer,
                    1,
                    &rw_bytes);
            if(status != SNS_DDF_SUCCESS)
                return status;
            if(rw_bytes != 1)
                return SNS_DDF_EBUS;
        }
    }
    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_enable_sched_data

DESCRIPTION  Implement enable_sched_data() DDF API.
============================================================================*/
sns_ddf_status_e sns_dd_acc_mma8452_enable_sched_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    bool enable)
{
    sns_dd_acc_mma8452_state_t* state = dd_handle;

    if(enable)
        return sns_dd_acc_mma8452_enable_irq(state, sensor);
    else
        return sns_dd_acc_mma8452_disable_irq(state, sensor);
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_log_data

DESCRIPTION  Logs latest set of sensor data sampled from the sensor.
============================================================================*/
void sns_dd_acc_mma8452_log_data(sns_ddf_sensor_data_s *accel_data_ptr)
{
    sns_err_code_e err_code;
    sns_log_sensor_data_pkt_s* log_struct_ptr;

    /*Allocate log packet*/
    err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                               sizeof(sns_log_sensor_data_pkt_s),
                               (void**)&log_struct_ptr);

    if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
    {
        log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log_struct_ptr->sensor_id = SNS_DDF_SENSOR_ACCEL;
        log_struct_ptr->vendor_id = SNS_DDF_VENDOR_STMICRO;

        /*Timestamp the log with sample time*/
        log_struct_ptr->timestamp = accel_data_ptr->timestamp;

        /*Log the sensor data*/
        log_struct_ptr->num_data_types = MMA8452_NUM_AXES;
        log_struct_ptr->data[0]  = accel_data_ptr->samples[0].sample;
        log_struct_ptr->data[1]  = accel_data_ptr->samples[1].sample;
        log_struct_ptr->data[2]  = accel_data_ptr->samples[2].sample;

        /*Commit log (also frees up the log packet memory)*/
        (void) sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                             log_struct_ptr);
    }
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_get_accel_data

DESCRIPTION  Get the datas from the sensor.
============================================================================*/
sns_ddf_status_e sns_dd_acc_mma8452_get_accel_data(sns_dd_acc_mma8452_state_t* state)
{
    uint8_t read_buffer[MMA8452_NUM_READ_BYTES] = { 0 };
    uint8_t read_count = 0;
    sns_ddf_status_e status;
    /* sensor coordinate x,y,z axis raw hardware data*/
    int16_t hw_d[MMA8452_NUM_AXES] = { 0 };

    /*if current power mode is LOWPOWER , return error.*/
   if(SNS_DDF_POWERSTATE_LOWPOWER == state->power_state)
        return SNS_DDF_EDEVICE;

    /* This is a synchronous driver, so try to read data now.*/
    status = sns_ddf_read_port(
        state->port_handle,
        MMA8452_OUT_X_MSB,
        read_buffer,
        MMA8452_NUM_READ_BYTES,
        &read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(read_count != MMA8452_NUM_READ_BYTES)
        return SNS_DDF_EBUS;

    /*convert the raw data in read_buffer to X/Y/Z axis sensor data*/
    hw_d[0] = (int16_t) (((read_buffer[0]) << 8) | read_buffer[1]);
    hw_d[1] = (int16_t) (((read_buffer[2]) << 8) | read_buffer[3]);
    hw_d[2] = (int16_t) (((read_buffer[4]) << 8) | read_buffer[5]);

    /*adjust sensor data per current sensitivity, and convert to Q16*/
    state->data_cache[0] = FX_FLTTOFIX_Q16((hw_d[0] >> 4) * 2 * state->sstvt_adj * G/4096);
    state->data_cache[1] = FX_FLTTOFIX_Q16((hw_d[1] >> 4) * 2 * state->sstvt_adj * G/4096);
    state->data_cache[2] = FX_FLTTOFIX_Q16((hw_d[2] >> 4) * 2 * state->sstvt_adj * G/4096);

    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_report_data

DESCRIPTION  Report the datas when in irq mode.
============================================================================*/
sns_ddf_status_e sns_dd_acc_mma8452_report_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_time_t    timestamp)
{
    static sns_ddf_sensor_data_s   *sensor_data;
    sns_dd_acc_mma8452_state_t* state = dd_handle;
    sns_ddf_status_e status;

    status = sns_dd_acc_mma8452_get_accel_data(state);
    if(status != SNS_DDF_SUCCESS)
        return status;

    sns_ddf_map_axes(&state->axes_map, state->data_cache);

    if ((sns_ddf_malloc((void **)&sensor_data, sizeof(sns_ddf_sensor_data_s))) != SNS_DDF_SUCCESS )
        /* Failed miserably. Can't even notify SMGR */
        return SNS_DDF_EFAIL;

    sensor_data->sensor = SNS_DDF_SENSOR_ACCEL;
    sensor_data->status = SNS_DDF_SUCCESS;
    sensor_data->timestamp = timestamp;

    if((sns_ddf_malloc((void **)&sensor_data->samples, MMA8452_NUM_AXES * sizeof(sns_ddf_sensor_sample_s))) != SNS_DDF_SUCCESS )
        return SNS_DDF_EFAIL;

    /*
    *Fill out sensor output data and status
    *Axis and polarity conversion are configured in the header file.
    */
    sensor_data->samples[0].sample = state->data_cache[0];
    sensor_data->samples[0].status = SNS_DDF_SUCCESS;
    sensor_data->samples[1].sample = state->data_cache[1];
    sensor_data->samples[1].status = SNS_DDF_SUCCESS;
    sensor_data->samples[2].sample = state->data_cache[2];
    sensor_data->samples[2].status = SNS_DDF_SUCCESS;
    sensor_data->num_samples = MMA8452_NUM_AXES;

    if (EnableI2C == FALSE)
    {
        static uint32 dummymma8452Data = 1;
        sensor_data->samples[0].sample = dummymma8452Data++;
        sensor_data->samples[1].sample = dummymma8452Data++;
        sensor_data->samples[2].sample = dummymma8452Data++;
    }

    sns_dd_acc_mma8452_log_data(sensor_data);

    sns_ddf_smgr_notify_data(state->smgr_hndl, sensor_data, 1);

    sns_ddf_mfree(sensor_data->samples);
    sns_ddf_mfree(sensor_data);

    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_handle_irq

DESCRIPTION  Called in response to an motion detection interrupt for this driver.
============================================================================*/
void sns_dd_acc_mma8452_handle_irq(
    sns_ddf_handle_t  dd_handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp)
{
    sns_dd_acc_mma8452_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    uint8_t read_buffer = 0;

    status = sns_ddf_read_port(state->port_handle,
        MMA8452_INT_SOURCE,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return;
    if(rw_bytes != 1)
        return;

    if(rw_buffer & MMA8452_SRC_DRDY)
    {
        status = sns_dd_acc_mma8452_report_data(dd_handle, timestamp);
        if(status != SNS_DDF_SUCCESS)
            return;
    }
    else if(rw_buffer & MMA8452_SRC_TRANS)
    {
        status = sns_ddf_read_port(state->port_handle,
            MMA8452_TRANSIENT_SRC,
            &read_buffer,
            1,
            &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
            return;
        if(rw_bytes != 1)
            return;

        if(read_buffer & MMA8452_EA)
        {
            sns_dd_acc_mma8452_config_motion_int(state,0);
            status = sns_ddf_signal_deregister(gpio_num);
            if(status != SNS_DDF_SUCCESS)
                return;

            sns_ddf_smgr_notify_event(state->smgr_hndl,
                SNS_DDF_SENSOR_ACCEL,
                SNS_DDF_EVENT_MOTION);
        }
    }
}

/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_get_data

DESCRIPTION  Retrieves a single set of sensor data from mma8452.
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mma8452_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data)
{
    uint8_t read_buffer[MMA8452_NUM_READ_BYTES] = { 0 };
    uint8_t read_count = 0;
    sns_ddf_time_t timestamp;
    sns_ddf_status_e status;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_acc_mma8452_state_t *state = dd_handle;
    /*sensor coordinate x,y,z axis raw hardware data*/
    int16_t hw_d[MMA8452_NUM_AXES] = { 0 };
    /*SAE coordinate x,y,z axis data*/
    q16_t sae_d[MMA8452_NUM_AXES] = { 0 };

    /*if current power mode is LOWPOWER , return error.*/
    if((SNS_DDF_POWERSTATE_LOWPOWER == state->power_state)||(num_sensors!=1))
        return SNS_DDF_EDEVICE;

    /*This is a synchronous driver, so try to read data now.*/
    status = sns_ddf_read_port(
        state->port_handle,
        MMA8452_OUT_X_MSB,
        read_buffer,
        MMA8452_NUM_READ_BYTES,
        &read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(read_count != MMA8452_NUM_READ_BYTES)
        return SNS_DDF_EBUS;

    /*get current time stamp*/
    timestamp = sns_ddf_get_timestamp();

    /*convert the raw data in read_buffer to X/Y/Z axis sensor data*/
    hw_d[0] = (int16_t) (((read_buffer[0]) << 8) | read_buffer[1]);
    hw_d[1] = (int16_t) (((read_buffer[2]) << 8) | read_buffer[3]);
    hw_d[2] = (int16_t) (((read_buffer[4]) << 8) | read_buffer[5]);

    /*adjust sensor data per current sensitivity, and convert to Q16*/
    sae_d[0] = FX_FLTTOFIX_Q16((hw_d[0] >> 4) * 2 * state->sstvt_adj * G/4096);
    sae_d[1] = FX_FLTTOFIX_Q16((hw_d[1] >> 4) * 2 * state->sstvt_adj * G/4096);
    sae_d[2] = FX_FLTTOFIX_Q16((hw_d[2] >> 4) * 2 * state->sstvt_adj * G/4096);

    /*map the sensor data to the phone's coordinates*/
    sns_ddf_map_axes(&state->axes_map, sae_d);

    state->out_of_bound_flag = 0;

    /*allocate memory for sns_ddf_sensor_data_s data structure*/
    data_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_data_s));
    if(NULL == data_ptr)
    {
        return SNS_DDF_ENOMEM;
    }
    *data = data_ptr;

    data_ptr->sensor = sensors[0];
    data_ptr->status = SNS_DDF_SUCCESS;
    data_ptr->timestamp = timestamp;

    /*allocate memory for data samples.*/
    data_ptr->samples = sns_ddf_memhandler_malloc(memhandler, MMA8452_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
    if(NULL == data_ptr->samples)
    {
       return SNS_DDF_ENOMEM;
    }

    /*
    *Fill out sensor output data and status
    *Axis and polarity conversion are configured in the header file.
    */
    data_ptr->samples[0].sample = sae_d[0];
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = sae_d[1];
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->samples[2].sample = sae_d[2];
    data_ptr->samples[2].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples = MMA8452_NUM_AXES;

    if (EnableI2C == FALSE)
    {
       static uint32 dummyLis3dhData = 1;
       data_ptr->samples[0].sample = dummyLis3dhData++;
       data_ptr->samples[1].sample = dummyLis3dhData++;
       data_ptr->samples[2].sample = dummyLis3dhData++;
    }

    sns_dd_acc_mma8452_log_data(data_ptr);

    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_match_odr

DESCRIPTION  Match the freqs of ODR.
============================================================================*/
sns_ddf_status_e  sns_dd_acc_mma8452_match_odr(
    sns_ddf_odr_t desired_rate,
    sns_ddf_odr_t *new_rate,
    uint32_t *new_index)
{
    uint8_t idx = 0;

    if((0 == desired_rate) || (desired_rate > MMA8452_MAX_ODR))
        return SNS_DDF_EINVALID_PARAM;

    for(idx=0; idx<MMA8452_ODR_NUM; idx++)
    {
        if(desired_rate <= mma8452_odr_array[idx])
            break;
    }

    if(idx >= MMA8452_ODR_NUM)
    {
        // if desired_rate > max supported rate
        // use max ODR        
        idx = (MMA8452_ODR_NUM - 1);
    }

    *new_rate = mma8452_odr_array[idx];
    *new_index = idx;

    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_config_motion_detect

DESCRIPTION  Configure the motion interrupt threshold
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mma8452_config_motion_detect(
    sns_ddf_handle_t dd_handle
)
{
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    sns_dd_acc_mma8452_state_t* state = dd_handle;

    rw_buffer = MMA8452_TRANSIENT_ENABLE;

    status = sns_dd_acc_mma8452_writer_register(dd_handle,
        MMA8452_TRANSIENT_CFG,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    rw_buffer = MMA8452_TRANSIENT_THRESHOLD;

    status = sns_dd_acc_mma8452_writer_register(state->port_handle,
        MMA8452_TRANSIENT_THS,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    return  SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_config_motion_int

DESCRIPTION  Configure (enable/disable) the motion interrupt
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mma8452_config_motion_int(
    sns_ddf_handle_t dd_handle,
    bool                        enable
)
{
    sns_dd_acc_mma8452_state_t* state = dd_handle;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;

    status = sns_ddf_read_port(state->port_handle,
        MMA8452_CTRL_REG4,
        &rw_buffer,
        1,
        &rw_bytes);

    if ( status == SNS_DDF_SUCCESS)
    {
        if (enable)
        {
            rw_buffer |= MMA8452_TRANSIENT_INT;
        }
        else
        {
            rw_buffer &= ~(MMA8452_TRANSIENT_INT);
        }

        status = sns_dd_acc_mma8452_writer_register(state->port_handle,
            MMA8452_CTRL_REG4,
            &rw_buffer,
            1,
            &rw_bytes);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(rw_bytes != 1)
            return SNS_DDF_EBUS;
    }

   if(status == SNS_DDF_SUCCESS)
   {
       state->motion_detect_enabled = enable;
   }

   return status;
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_set_attr

DESCRIPTION  Sets a mma8452 sensor attribute to a specific value.
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mma8452_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    sns_ddf_odr_t new_rate = 0;
    uint32_t new_rate_index = 0;
    sns_dd_acc_mma8452_state_t* state = dd_handle;

    if((sensor != SNS_DDF_SENSOR_ACCEL) && (sensor != SNS_DDF_SENSOR__ALL))
        return SNS_DDF_EINVALID_PARAM;

    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_STATE:
        {
            sns_ddf_powerstate_e* power_state = value;
            return sns_dd_acc_mma8452_set_power(dd_handle, *power_state);
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            uint32_t* range_idx = value;
            state->range = mma8452_ranges[*range_idx];
            if(state->range > MMA8452_MAX_RANGE)
                return SNS_DDF_EINVALID_PARAM;

            return sns_dd_acc_mma8452_set_range(dd_handle);
        }
		
        case SNS_DDF_ATTRIB_LOWPASS:
        {
            uint32_t* freqs_idx = value;
            mma8452_odr freqs = mma8452_odr_setting[*freqs_idx];
            if(freqs > ((MMA8452_MAX_ODR)))
                return SNS_DDF_EINVALID_PARAM;

            return sns_dd_acc_mma8452_set_datarate(dd_handle);
        }
		
        case SNS_DDF_ATTRIB_ODR:
        {
            if(sensor == SNS_DDF_SENSOR_ACCEL)
            {
                status =  sns_dd_acc_mma8452_match_odr(*(sns_ddf_odr_t*)value,
                    &new_rate,
                    &new_rate_index);
            }
            if(status != SNS_DDF_SUCCESS)
                return status;

            if(new_rate != state->acc_cur_rate)
            {
                state->data_rate = mma8452_odr_setting[new_rate_index];
                status = sns_dd_acc_mma8452_set_datarate(dd_handle);
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

        case SNS_DDF_ATTRIB_MOTION_DETECT:
        {
            bool* enable = value;
 
            if(!sns_ddf_signal_irq_enabled())
            {
                return SNS_DDF_EINVALID_PARAM;
            }

            if(*enable)
            {
                status = sns_dd_acc_mma8452_config_motion_detect(dd_handle);
                if ( status  != SNS_DDF_SUCCESS)
                    return status;

                status = sns_dd_acc_mma8452_config_motion_int(dd_handle, 1);
                if ( status  != SNS_DDF_SUCCESS)
                    return status;

                /*! Register to receive notification when the interrupt fires */
                return sns_ddf_signal_register(state->gpio_num,
                    dd_handle,
                    &sns_dd_acc_mma8452_if,
                    SNS_DDF_SIGNAL_IRQ_LOW);
            }
            else
            {
                if(state->motion_detect_enabled)
                {
                    status = sns_dd_acc_mma8452_config_motion_int(dd_handle, 0);
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
            return SNS_DDF_EINVALID_ATTR;
    }
}


/*===========================================================================
FUNCTION      sns_dd_acc_mma8452_test_notify

DESCRIPTION  Runs bias test as part of OEM selftest and returns the test result.
============================================================================*/
static void sns_dd_acc_mma8452_test_notify(
    sns_ddf_handle_t handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_status_e status,
    uint32_t err,
    q16_t* biases)
{
    static const uint8_t num_axes = 3;
    sns_dd_acc_mma8452_state_t *state = handle;
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
FUNCTION      sns_dd_acc_mma8452_test_notify

DESCRIPTION  Runs self test and returns the test result.
============================================================================*/
static sns_ddf_status_e sns_dd_acc_mma8452_self_test(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_test_e test,
    uint32_t* err)
{
    sns_dd_acc_mma8452_state_t *state = dd_handle;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    sns_dd_inertial_test_config_s test_config;
	
    *err = 0;

    if(test == SNS_DDF_TEST_OEM)
    {
        test_config.sample_rate =  20;
        test_config.num_samples =  64;
        test_config.max_variance = (int64_t)(FX_CONV((FX_FLTTOFIX_Q16(0.3)),16,32));
        test_config.bias_thresholds[0] = FX_FLTTOFIX_Q16(0.2*G);
        test_config.bias_thresholds[1] = FX_FLTTOFIX_Q16(0.2*G);
        test_config.bias_thresholds[2] = FX_FLTTOFIX_Q16(0.26*G);

        sns_dd_acc_mma8452_reset(dd_handle);

        status = sns_dd_inertial_test_run(
                  sensor,
                  dd_handle,
                  &sns_dd_acc_mma8452_if,
                  &test_config,
                  &sns_dd_acc_mma8452_test_notify);

        if(status != SNS_DDF_SUCCESS)
            return status;
        return SNS_DDF_SUCCESS;
    }
    else if(test == SNS_DDF_TEST_SELF)
    {
        status = sns_ddf_read_port(state->port_handle, MMA8452_WHO_AM_I, &rw_buffer, 1,&rw_bytes);
        if( status != SNS_DDF_SUCCESS ) return status;
        status = sns_ddf_read_port(state->port_handle, MMA8452_CTRL_REG1, &rw_buffer, 1,&rw_bytes);
        if( status != SNS_DDF_SUCCESS ) return status;
        status = sns_ddf_read_port(state->port_handle, MMA8452_CTRL_REG2, &rw_buffer, 1,&rw_bytes);
        if( status != SNS_DDF_SUCCESS ) return status;
        status = sns_ddf_read_port(state->port_handle, MMA8452_CTRL_REG3, &rw_buffer, 1,&rw_bytes);
        if( status != SNS_DDF_SUCCESS ) return status;
        status = sns_ddf_read_port(state->port_handle, MMA8452_CTRL_REG4, &rw_buffer, 1,&rw_bytes);
        if( status != SNS_DDF_SUCCESS ) return status;
        status = sns_ddf_read_port(state->port_handle, MMA8452_CTRL_REG5, &rw_buffer, 1,&rw_bytes);
        if( status != SNS_DDF_SUCCESS ) return status;
    }
    else
    {
        return SNS_DDF_EINVALID_TEST;
    }
    return SNS_DDF_SUCCESS;
}


/**
 * mma8452 device driver interface.
 */
sns_ddf_driver_if_s sns_dd_acc_mma8452_if =
{
    .init = &sns_dd_acc_mma8452_init,
    .get_data = &sns_dd_acc_mma8452_get_data,
    .set_attrib = &sns_dd_acc_mma8452_set_attr,
    .get_attrib = &sns_dd_acc_mma8452_get_attr,
    .handle_timer = NULL,
    .handle_irq = &sns_dd_acc_mma8452_handle_irq,
    .reset = &sns_dd_acc_mma8452_reset,
    .run_test = &sns_dd_acc_mma8452_self_test,
    .enable_sched_data = &sns_dd_acc_mma8452_enable_sched_data,
    .probe = &sns_dd_acc_mma8452_probe
};
