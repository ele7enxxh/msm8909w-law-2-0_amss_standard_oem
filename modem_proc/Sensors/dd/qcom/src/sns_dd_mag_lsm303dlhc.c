/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
*
* File Name         : lsm303dlhc_ddf_drv.c
* Authors           : Jianjian Huo
* Version           : V 1.0.0
* Date              : 12/03/2012
* Description       : LSM303DLHC magnetometer driver source file
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
* VERSION | DATE          | DESCRIPTION
*
* 2.0.0   | 08/12/2015    | Fix shift count is negative warnings in few places
* 1.0.0   | 12/03/2012    | Reduced default ODR to 30Hz;
                            Added WHO_AM_I reading during init.
* 0.9.6   | 05/16/2012    | Converted all floating point operations to fixed point.
* 0.9.1   | 05/05/2011    | Used a header file to configure axis and polarity.
* 0.9.0   | 04/27/2011    | Created.
*******************************************************************************/

#include "sns_dd_mag_lsm303dlhc.h"
#include "sns_dd_stm_config.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include <string.h>

#ifndef DDK
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#endif

#define STM_LSM303DLHC_NUM_AXES 3
#define STM_LSM303DLHC_NUM_READ_BYTES 6
#define AUTO_INCREMENT    0x80

/**
 * Magnetometer LSM303DLHC Driver State Information Structure
 */
typedef struct {
    /**< Handle used with sns_ddf_notify_data. */
    sns_ddf_handle_t smgr_hndl; 

    /**< Handle used to access the I2C bus. */
    sns_ddf_handle_t port_handle; 

    /**< Current range selection. */
    stm_lsm303dlhc_mag_range range; 

    /**< Current sampling frequency. */
    stm_lsm303dlhc_mag_odr data_rate; 

    /**< Current sensitivity for raw data conversion. */
    stm_lsm303dlhc_mag_sstvt mag_sstvt;

    /**< Current power state: ACTIVE or LOWPOWER */
    sns_ddf_powerstate_e power_state; 

    /**< flag for sensor data out of bound. */
    //uint8_t out_of_bound_flag;
} sns_dd_mag_lsm303dlhc_state_t;

/**
 * Magnetometer LSM303DLHC ranges array for setting attribute
 */
static stm_lsm303dlhc_mag_range lsm303dlhc_ranges[STM_LSM303DLHC_MAG_RANGE_NUM]={
            STM_LSM303DLHC_MAG_RANGE_1_3G,
            STM_LSM303DLHC_MAG_RANGE_1_9G,
            STM_LSM303DLHC_MAG_RANGE_2_5G,
            STM_LSM303DLHC_MAG_RANGE_4_0G,
            STM_LSM303DLHC_MAG_RANGE_4_7G,
            STM_LSM303DLHC_MAG_RANGE_5_6G,
            STM_LSM303DLHC_MAG_RANGE_8_1G};

/**
 * Magnetometer LSM303DLHC sampling frequencies array for getting attribute
 * frequencies are rounded to interger, such as using 2Hz to replace 1.5Hz.
 */
static sns_ddf_lowpass_freq_t lsm303dlhc_freqs[STM_LSM303DLHC_MAG_ODR_NUM] = {
            FX_FLTTOFIX_Q16(0.4), 
            FX_FLTTOFIX_Q16(0.8),
            FX_FLTTOFIX_Q16(1.5),
            FX_FLTTOFIX_Q16(3.8),
            FX_FLTTOFIX_Q16(7.5),

			// define FX_QFACTOR		   (16)
			// define FX_CONV(a,q1,q2)	   (((q2)>(q1))?(a)<<((q2)-(q1)):(a)>>((q1)-(q2)))
			// define FX_CONV_Q16(a,q1)    (FX_CONV(a,q1,FX_QFACTOR))
			// Avoiding "shift count is negative" warning by not using FX_CONV macro
            //FX_CONV_Q16((int32_t)15,0),
            (q16_t)(15 << 16),
            FX_FLTTOFIX_Q16(37.5),
            //FX_CONV_Q16((int32_t)110,0)};
            (q16_t)(110 << 16)};
            
/**
 * Magnetometer LSM303DLHC sampling frequencies array for setting attribute
 */         
static stm_lsm303dlhc_mag_odr lsm303dlhc_odr_setting[STM_LSM303DLHC_MAG_ODR_NUM] = {
            STM_LSM303DLHC_MAG_ODR_75, 
            STM_LSM303DLHC_MAG_ODR1_5, 
            STM_LSM303DLHC_MAG_ODR3_0, 
            STM_LSM303DLHC_MAG_ODR7_5, 
            STM_LSM303DLHC_MAG_ODR15, 
            STM_LSM303DLHC_MAG_ODR30, 
            STM_LSM303DLHC_MAG_ODR75, 
            STM_LSM303DLHC_MAG_ODR220};
    
/** 
  * @brief Resets the driver and device so they return to the state they were 
  *        in after init() was called.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_mag_lsm303dlhc_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_mag_lsm303dlhc_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer[3];
    uint8_t bytes_written = 0;

    // Reset driver state.
    //state->out_of_bound_flag = 0;

    // Configure magnetometer control register CRA_REG_M
    write_buffer[0] = 0x0
                   | (1<<7)         // MSB must be 1
                   | (1<<4)         // DO2 bit
                   | (0<<3)         // DO1 bit
                   | (1<<2)         // DO0 bit.  ODR:30Hz
                   | (0<<1)         // MS1 bit
                   | 0;             // MS0 bit      
                                
    // Configure magnetometer control register CRB_REG_M
    write_buffer[1] = 0x0
                   | (1<<7)         // GN2 bit
                   | (0<<6)         // GN1 bit
                   | (1<<5)         // GN0 bit  Gain: 4.7Gauss
                   | 0;               
                                
    // Configure magnetometer control register MR_REG_M
    write_buffer[2] = 0x0
                   | (1<<1)         // MD1 bit
                   | 0;             // MD0 bit  Mode: sleep mode.
    
    status = sns_ddf_write_port(
        state->port_handle,
        AUTO_INCREMENT | STM_LSM303DLHC_CTRL_CRA_REG,
        write_buffer,
        3,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 3)
        return SNS_DDF_EBUS;
    
    // Let calibration values settle in by waiting 5 ms.
    sns_ddf_delay(5000);

    state->range = STM_LSM303DLHC_MAG_RANGE_4_7G;
    state->mag_sstvt.xy = STM_LSM303DLHC_XY_SENSITIVITY_4_7;
    state->mag_sstvt.z = STM_LSM303DLHC_Z_SENSITIVITY_4_7;
    state->data_rate = STM_LSM303DLHC_MAG_ODR30;
    state->power_state = SNS_DDF_POWERSTATE_LOWPOWER;
    
    return SNS_DDF_SUCCESS;
}

/**
  * @brief Probes for an LSM303DLHC part.
  *
  * Refer to sns_ddf_driver_if.h for definition.
  */
sns_ddf_status_e sns_dd_mag_lsm303dlhc_probe(
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors )
{
    sns_ddf_status_e status;
    sns_ddf_handle_t port_handle;
    uint8_t i2c_buff;
    uint8_t bytes_read;
    *num_sensors = 0;
    *sensors = NULL;

    status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    // Check WhoAmIRegister to make sure this is the correct driver
    status = sns_ddf_read_port(
        port_handle,
        STM_LSM303DLHC_IR_REG_M,
        &i2c_buff,
        1,
        &bytes_read);
    if(status != SNS_DDF_SUCCESS || bytes_read != 1)
    {
        sns_ddf_close_port( port_handle );
        return status;
    }
    if(i2c_buff != 0x48)
    {
        /* Incorrect value. Return now with nothing detected */
        sns_ddf_close_port( port_handle );
        return SNS_DDF_SUCCESS;
    }

    // WHO_AM_I register is correct. This is probably an LSM303DLHC
    *num_sensors = 1;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
                                          sizeof(sns_ddf_sensor_e) * *num_sensors );
    if( *sensors != NULL ) 
    {
        (*sensors)[0] = SNS_DDF_SENSOR_MAG;
        status = SNS_DDF_SUCCESS;
    } else {
        status = SNS_DDF_ENOMEM;
    }
    sns_ddf_close_port( port_handle );
    return status;
}

/**
 * @brief Performs a set data sampling rate operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  data_rate       Data sampling rate settings for LSM303DLHC
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_mag_lsm303dlhc_set_datarate(
  sns_ddf_handle_t dd_handle, 
  stm_lsm303dlhc_mag_odr data_rate)
{
    sns_dd_mag_lsm303dlhc_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;

    // Configure ODR in CRA_REG_M
    write_buffer = (uint8_t)data_rate; 
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LSM303DLHC_CTRL_CRA_REG,
        &write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    state->data_rate= data_rate;

    // Let ODR value settle in by waiting 5 ms.
    sns_ddf_delay(5000);

    return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set power state operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  power_state    Power state to be set.
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_mag_lsm303dlhc_set_power(
  sns_ddf_handle_t dd_handle, 
  sns_ddf_powerstate_e power_state)
{
    sns_dd_mag_lsm303dlhc_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;

    if((SNS_DDF_POWERSTATE_LOWPOWER == power_state)
        &&(SNS_DDF_POWERSTATE_ACTIVE == state->power_state))
    {
        // Configure MR_REG_M to enter "sleep mode".
        write_buffer =  0x0
                    | (1<<1)       // MD1 bit
                    | 0;           // MD0 bit 

        status = sns_ddf_write_port(
            state->port_handle,
            STM_LSM303DLHC_MR_REG,
            &write_buffer,
            1,
            &bytes_written);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(bytes_written != 1)
            return SNS_DDF_EBUS;

        state->power_state=SNS_DDF_POWERSTATE_LOWPOWER;
                                
    }
    else if((SNS_DDF_POWERSTATE_ACTIVE == power_state)
        &&(SNS_DDF_POWERSTATE_LOWPOWER == state->power_state))
    {   
        // Configure MR_REG_M to enter "Continuous-Conversion Mode".
        write_buffer =  0x0
                     | (0<<1)       // MD1 bit
                     | 0;           // MD0 bit 

        status = sns_ddf_write_port(
            state->port_handle,
            STM_LSM303DLHC_MR_REG,
            &write_buffer,
            1,
            &bytes_written);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(bytes_written != 1)
            return SNS_DDF_EBUS;

        // Let register value settle in by waiting 5 ms.
        sns_ddf_delay(5000);

        state->power_state= SNS_DDF_POWERSTATE_ACTIVE;
    }

    return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set dynamic range operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  range          Dynamic range settings for LSM303DLHC
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_mag_lsm303dlhc_set_range(
  sns_ddf_handle_t dd_handle, 
  stm_lsm303dlhc_mag_range range)
{
    sns_dd_mag_lsm303dlhc_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;
    stm_lsm303dlhc_mag_sstvt sstvt;

    //set sensor sensitivity for data conversion
    switch (range) {
        case STM_LSM303DLHC_MAG_RANGE_1_3G:
            sstvt.xy = STM_LSM303DLHC_XY_SENSITIVITY_1_3;
            sstvt.z = STM_LSM303DLHC_Z_SENSITIVITY_1_3;
            break;
        case STM_LSM303DLHC_MAG_RANGE_1_9G:
            sstvt.xy = STM_LSM303DLHC_XY_SENSITIVITY_1_9;
            sstvt.z = STM_LSM303DLHC_Z_SENSITIVITY_1_9;
            break;
        case STM_LSM303DLHC_MAG_RANGE_2_5G:
            sstvt.xy = STM_LSM303DLHC_XY_SENSITIVITY_2_5;
            sstvt.z = STM_LSM303DLHC_Z_SENSITIVITY_2_5;
            break;
        case STM_LSM303DLHC_MAG_RANGE_4_0G:
            sstvt.xy = STM_LSM303DLHC_XY_SENSITIVITY_4_0;
            sstvt.z = STM_LSM303DLHC_Z_SENSITIVITY_4_0;
            break;
        case STM_LSM303DLHC_MAG_RANGE_4_7G:
            sstvt.xy = STM_LSM303DLHC_XY_SENSITIVITY_4_7;
            sstvt.z = STM_LSM303DLHC_Z_SENSITIVITY_4_7;
            break;
        case STM_LSM303DLHC_MAG_RANGE_5_6G:
            sstvt.xy = STM_LSM303DLHC_XY_SENSITIVITY_5_6;
            sstvt.z = STM_LSM303DLHC_Z_SENSITIVITY_5_6;
            break;
        case STM_LSM303DLHC_MAG_RANGE_8_1G:
            sstvt.xy = STM_LSM303DLHC_XY_SENSITIVITY_8_1;
            sstvt.z = STM_LSM303DLHC_Z_SENSITIVITY_8_1;
            break;
        default:
            return SNS_DDF_EINVALID_PARAM;  
    }
    
    // Configure CRB_REG_M
    write_buffer = (uint8_t)range; 
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LSM303DLHC_CTRL_CRB_REG,
        &write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    state->range = range;
    state->mag_sstvt= sstvt;

    // Let dynamic range value settle in by waiting 5 ms.
    sns_ddf_delay(5000);

    return SNS_DDF_SUCCESS;
}

/** 
 * @brief Initializes the lsm303dlhc driver and sets up sensor. 
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_mag_lsm303dlhc_init(
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
    sns_dd_mag_lsm303dlhc_state_t* state;
    static sns_ddf_sensor_e my_sensors[] = {
        SNS_DDF_SENSOR_MAG};
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;       
    
    // Allocate a driver instance.
    status = sns_ddf_malloc((void**)&state, sizeof(sns_dd_mag_lsm303dlhc_state_t));
    if(status != SNS_DDF_SUCCESS)
    {
        return SNS_DDF_ENOMEM;
    }
    state->smgr_hndl = smgr_handle;

     // Open communication port to the device.
    status = sns_ddf_open_port(&state->port_handle, &device_info->port_config); 
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_mfree(state);
        return status;
    }

    // wait sensor powering up to be ready by delaying 20 ms.
    sns_ddf_delay(20000);

    // confirm WHO_AM_I register
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LSM303DLHC_IR_REG_M,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status; 
    if((rw_bytes!=1)||(rw_buffer!=0x48))
        return SNS_DDF_EFAIL;

    // Resets the lsm303dlhc
    status = sns_dd_mag_lsm303dlhc_reset(state);
    if(status != SNS_DDF_SUCCESS)   
    {   
        sns_ddf_close_port(state->port_handle);       
        sns_ddf_mfree(state);     
        return status;  
    }   

    // Fill out output parameters.
    *num_sensors = 1;
    *sensors = my_sensors;
    *dd_handle_ptr = state;

    return SNS_DDF_SUCCESS;
}

/** 
 * @brief Retrieves a single set of magnetic sensor data from lsm303dlhc.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_mag_lsm303dlhc_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data)
{
    uint8_t read_buffer[STM_LSM303DLHC_NUM_READ_BYTES];
    uint8_t read_count = 0;
    sns_ddf_time_t timestamp;
    sns_ddf_status_e status;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_mag_lsm303dlhc_state_t *state = dd_handle;
    // sensor coordinate x,y,z axis raw hardware data
    int16_t hw_d[STM_LSM303DLHC_NUM_AXES] = { 0 };
    // SAE coordinate x,y,z axis data
    int16_t sae_d[STM_LSM303DLHC_NUM_AXES] = { 0 };

    //if current power mode is LOWPOWER , return error.
    if(SNS_DDF_POWERSTATE_LOWPOWER == state->power_state)
        return SNS_DDF_EDEVICE;
    
    // This is a synchronous driver, so try to read data now.
    status = sns_ddf_read_port(
        state->port_handle,
        AUTO_INCREMENT | STM_LSM303DLHC_OUT_X_M,
        read_buffer,
        STM_LSM303DLHC_NUM_READ_BYTES,
        &read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(read_count != STM_LSM303DLHC_NUM_READ_BYTES)
        return SNS_DDF_EBUS;    

    // get current time stamp
    timestamp = sns_ddf_get_timestamp();
    
    //convert the raw data in read_buffer to X/Y/Z axis sensor data
    hw_d[0] = (int16_t) (((read_buffer[0]) << 8) | read_buffer[1]);
    hw_d[1] = (int16_t) (((read_buffer[2]) << 8) | read_buffer[3]);
    hw_d[2] = (int16_t) (((read_buffer[4]) << 8) | read_buffer[5]);

    //print out raw data if need debugging.
    /*SNS_PRINTF_STRING_ID_MEDIUM_3(SNS_DBG_MOD_DSPS_SMGR, 
                                  DBG_SMGR_GENERIC_STRING3, 
                                  hw_d[0], hw_d[1], hw_d[2]);*/

    //convert the sensor data to SAE coordinate
    sae_d[0] = (1 == STM_MAG_POLARITY_X)? hw_d[STM_MAG_AXIS_MAP_X] : -hw_d[STM_MAG_AXIS_MAP_X];
    sae_d[1] = (1 == STM_MAG_POLARITY_Y)? hw_d[STM_MAG_AXIS_MAP_Y] : -hw_d[STM_MAG_AXIS_MAP_Y];
    sae_d[2] = (1 == STM_MAG_POLARITY_Y)? hw_d[STM_MAG_AXIS_MAP_Z] : -hw_d[STM_MAG_AXIS_MAP_Z];
    
    //state->out_of_bound_flag = 0;

    //allocate memory for sns_ddf_sensor_data_s data structure
    data_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_data_s));
    if(NULL == data_ptr)
        return SNS_DDF_ENOMEM;

    *data = data_ptr;

    data_ptr->sensor = sensors[0];
    data_ptr->status = SNS_DDF_SUCCESS;
    data_ptr->timestamp = timestamp;

    //allocate memory for data samples.
    data_ptr->samples = sns_ddf_memhandler_malloc(memhandler, 
                            STM_LSM303DLHC_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
    if(NULL == data_ptr->samples)
        return SNS_DDF_ENOMEM;

    //Fill out sensor output data and status. 
    //Axis and polarity conversion are configured in the header file.
    data_ptr->samples[0].sample = FX_DIV_Q16(FX_CONV_Q16((int32_t)sae_d[0],0),
                                             FX_CONV_Q16((int32_t)(state->mag_sstvt.xy),0));
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = FX_DIV_Q16(FX_CONV_Q16((int32_t)sae_d[1],0),
                                             FX_CONV_Q16((int32_t)(state->mag_sstvt.xy),0));
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->samples[2].sample = FX_DIV_Q16(FX_CONV_Q16((int32_t)sae_d[2],0),
                                             FX_CONV_Q16((int32_t)(state->mag_sstvt.z),0));
    data_ptr->samples[2].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples = STM_LSM303DLHC_NUM_AXES;
    
    return SNS_DDF_SUCCESS;
}

/** 
 * @brief Retrieves the value of an attribute for a lsm303dlhc sensor.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_mag_lsm303dlhc_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems)
{
    sns_dd_mag_lsm303dlhc_state_t *state = dd_handle;
    
    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_INFO:
        {
            sns_ddf_power_info_s *power;
            power = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s));
            if(NULL == power)
                return SNS_DDF_ENOMEM;

            //current consumption, unit uA
            power->active_current = 119;
            power->lowpower_current = 24;

            *value = power;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *ranges;
            ranges = sns_ddf_memhandler_malloc(memhandler, 
                         STM_LSM303DLHC_MAG_RANGE_NUM * sizeof(sns_ddf_range_s));
            if(NULL == ranges)
                return SNS_DDF_ENOMEM;

            ranges[0].min = STM_LSM303DLHC_MAG_RANGE_1_3G_MIN;
            ranges[0].max = STM_LSM303DLHC_MAG_RANGE_1_3G_MAX;
            ranges[1].min = STM_LSM303DLHC_MAG_RANGE_1_9G_MIN;
            ranges[1].max = STM_LSM303DLHC_MAG_RANGE_1_9G_MAX;
            ranges[2].min = STM_LSM303DLHC_MAG_RANGE_2_5G_MIN;
            ranges[2].max = STM_LSM303DLHC_MAG_RANGE_2_5G_MAX;
            ranges[3].min = STM_LSM303DLHC_MAG_RANGE_4_0G_MIN;
            ranges[3].max = STM_LSM303DLHC_MAG_RANGE_4_0G_MAX;
            ranges[4].min = STM_LSM303DLHC_MAG_RANGE_4_7G_MIN;
            ranges[4].max = STM_LSM303DLHC_MAG_RANGE_4_7G_MAX;
            ranges[5].min = STM_LSM303DLHC_MAG_RANGE_5_6G_MIN;
            ranges[5].max = STM_LSM303DLHC_MAG_RANGE_5_6G_MAX;
            ranges[6].min = STM_LSM303DLHC_MAG_RANGE_8_1G_MIN;
            ranges[6].max = STM_LSM303DLHC_MAG_RANGE_8_1G_MAX;

            *num_elems = STM_LSM303DLHC_MAG_RANGE_NUM;
            *value = ranges;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t *res;
            res = sns_ddf_memhandler_malloc(memhandler ,sizeof(sns_ddf_resolution_t));
            if(NULL == res)
                return SNS_DDF_ENOMEM;

            *res = FX_DIV_Q16(FX_CONV_Q16((int32_t)1,0),FX_CONV_Q16((int32_t)(state->mag_sstvt.xy),0));
            *value = res;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
          sns_ddf_resolution_adc_s *device_res;
          if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_adc_s))) == NULL)
          {
            return SNS_DDF_ENOMEM;
          }
         
          device_res = *value;
    
          *num_elems = 1;
          device_res->bit_len = STM_MAG_SENSOR_BIT_LEN;
          device_res->max_freq = 50;

          return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            *value = lsm303dlhc_freqs;
            *num_elems = STM_LSM303DLHC_MAG_ODR_NUM;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s *lsm303dlhc_delays;
            lsm303dlhc_delays = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_delays_s));
            if(NULL == lsm303dlhc_delays)
                return SNS_DDF_ENOMEM;

            lsm303dlhc_delays->time_to_active = 5;
            lsm303dlhc_delays->time_to_data = 1;
            *value = lsm303dlhc_delays;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s *info;
            info = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_driver_info_s));
            if(NULL == info)
                return SNS_DDF_ENOMEM;

            info->name = "STM LSM303DLHC Magnetometer Driver";
            info->version = 1;
            *value = info;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DEVICE_INFO:
        {
            sns_ddf_device_info_s *info;
            info = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_device_info_s));
            if(NULL == info)
                return SNS_DDF_ENOMEM;

            info->name = "Magnetometer";
            info->vendor = "STMicroelectronics";
            info->model = "LSM303DLHC";
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
 * @brief Sets a lsm303dlhc sensor attribute to a specific value.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_mag_lsm303dlhc_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value)
{   
    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_STATE:
        {
            sns_ddf_powerstate_e* power_state = value;
            
            return sns_dd_mag_lsm303dlhc_set_power(dd_handle, *power_state);
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            uint32_t* range_idx = value;
            
            stm_lsm303dlhc_mag_range range = lsm303dlhc_ranges[*range_idx];
            if(range > STM_LSM303DLHC_MAG_MAX_RANGE)
                return SNS_DDF_EINVALID_PARAM;
            
            return sns_dd_mag_lsm303dlhc_set_range(dd_handle, range);
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            uint32_t* freqs_idx = value;
            
            stm_lsm303dlhc_mag_odr freqs = lsm303dlhc_odr_setting[*freqs_idx];
            if(freqs > STM_LSM303DLHC_MAG_MAX_ODR)
                return SNS_DDF_EINVALID_PARAM;
            
            return sns_dd_mag_lsm303dlhc_set_datarate(dd_handle, freqs);
        }

        default:
            return SNS_DDF_EINVALID_ATTR;
    }
}

/** 
 * LSM303DLHC Magnetometer device driver interface.
 */
sns_ddf_driver_if_s sns_dd_mag_lsm303dlhc_if =
{
    .init = &sns_dd_mag_lsm303dlhc_init,
    .get_data = &sns_dd_mag_lsm303dlhc_get_data,
    .set_attrib = &sns_dd_mag_lsm303dlhc_set_attr,
    .get_attrib = &sns_dd_mag_lsm303dlhc_get_attr,
    .handle_timer = NULL,
    .handle_irq = NULL,
    .reset = &sns_dd_mag_lsm303dlhc_reset,
    .run_test = NULL,
    .enable_sched_data = NULL,
    .probe = &sns_dd_mag_lsm303dlhc_probe
};

