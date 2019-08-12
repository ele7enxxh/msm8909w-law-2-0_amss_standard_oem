/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
*
* File Name         : sns_dd_gyro_l3g4200d.c
* Authors           : Jianjian Huo (samuel.huo@st.com)
* Version           : V 0.9.3
* Date              : 07/08/2011
* Description       : L3G4200D gyroscope driver source file
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
* 0.9.3   | 07/08/2011    | FIFO read enabled.
* 0.9.2   | 07/07/2011    | Created based on L3GD20 driver.
*******************************************************************************/


#include "sns_dd_gyro_l3g4200d.h"
#include "sns_dd_stm_config.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include <string.h>

#define STM_L3G4200D_NUM_AXES 3
#define STM_L3G4200D_NUM_READ_BYTES 6
#define AUTO_INCREMENT    0x80

/**
 * Gyroscope L3G4200D Driver State Information Structure
 */
typedef struct {
    /**< Handle used with sns_ddf_notify_data. */
    sns_ddf_handle_t smgr_hndl; 

    /**< Handle used to access the I2C bus. */
    sns_ddf_handle_t port_handle; 

    /**< Current range selection. */
    stm_l3g4200d_range range; 

    /**< Current sampling frequency. */
    stm_l3g4200d_odr data_rate; 

    /**< Current sensitivity for raw data conversion. */
    stm_l3g4200d_sstvt gyr_sstvt;

    /**< Current power state: ACTIVE or LOWPOWER */
    sns_ddf_powerstate_e power_state; 

    /**< bias setting. */
    q16_t bias[STM_L3G4200D_NUM_AXES];
    
    /**< data cache for storing sensor data. */
    q16_t data_cache[STM_L3G4200D_NUM_AXES];

    /**< flag for sensor data out of bound. */
    uint8_t out_of_bound_flag;
} sns_dd_gyr_l3g4200d_state_t;

/**
 * Gyroscope L3G4200D ranges array for setting attribute
 */
static stm_l3g4200d_range l3g4200d_ranges[STM_L3G4200D_RANGE_NUM]={
            STM_L3G4200D_RANGE_250DPS,
            STM_L3G4200D_RANGE_500DPS,
            STM_L3G4200D_RANGE_2000DPS};

/**
 * Gyroscope L3G4200D sampling frequencies array for getting attribute
 */
static sns_ddf_lowpass_freq_t l3g4200d_freqs[STM_L3G4200D_ODR_NUM] = {
            100, 200, 400, 800};
            
/**
 * Gyroscope L3G4200D sampling frequencies array for setting attribute
 */         
static stm_l3g4200d_odr l3g4200d_odr_setting[STM_L3G4200D_ODR_NUM] = {
            STM_L3G4200D_ODR100, 
            STM_L3G4200D_ODR200, 
            STM_L3G4200D_ODR400, 
            STM_L3G4200D_ODR800};

/**
 * @brief Update only one part of one register in L3G4200D
 *
 * @param[in]  dd_handle   Handle to a driver instance.
 * @param[in]  reg_addr    The address of L3G4200D register to be updated
 * @param[in]  mask        The mask of this register to specify which part to be updated
 * @param[in]  new_value   The new content for the specified part of this register
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_gyr_l3g4200d_update_onereg(
  sns_ddf_handle_t dd_handle, 
  uint8_t reg_addr,
  uint8_t mask,
  uint8_t new_value)
{
    sns_dd_gyr_l3g4200d_state_t* state = dd_handle;
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
 * @brief Check FIFO is empty or not.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[out] empty_flag     The bool flag to be set, set true when FIFO is empty.
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_gyr_l3g4200d_is_fifo_empty(
  sns_ddf_handle_t dd_handle, 
  bool* empty_flag)
{
    sns_dd_gyr_l3g4200d_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;

    // read FIFO_SRC_REG register
    status = sns_ddf_read_port(
        state->port_handle,
        STM_L3G4200D_FIFO_SRC,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    // check if FIFO is empty
    *empty_flag = (rw_buffer & 0x20)>0 ? true : false;

    return SNS_DDF_SUCCESS;
}

/** 
  * @brief Resets the driver and device so they return to the state they were 
  *        in after init() was called.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_gyr_l3g4200d_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_gyr_l3g4200d_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer[5] = { 0 };
    uint8_t bytes_written = 0;

    // Reset driver state.
    memset(state->bias, 0, STM_L3G4200D_NUM_AXES * sizeof(q16_t));
    state->out_of_bound_flag = 0;
    memset(state->data_cache, 0, STM_L3G4200D_NUM_AXES * sizeof(q16_t));

    // Configure gyroscope control register CTRL_REG1
    write_buffer[0] = 0x0
                   | (3<<6)         // DR1-DR0: ODR selection, 0-100Hz; 1-200; 2-400Hz; 3-800Hz;
                   | (2<<4)         // BW1-BW0: bandwidth selection
                   | (0<<3)         // PD bit: power down mode
                   | (1<<2)         // Zen bit
                   | (1<<1)         // Xen bit
                   | 1;             // Yen bit
                                
    // Configure gyroscope control register CTRL_REG2
    write_buffer[1] = 0x0
                   | (0<<4)         // HPM1-HPM0: High Pass filter Mode Selection.
                   | 0;             // HPCF3-HPCF0: Cut Off frequency selection  
                                
    // Configure gyroscope control register CTRL_REG3
    write_buffer[2] = 0x0;

    // Configure gyroscope control register CTRL_REG4
    write_buffer[3] = 0x0
                   | (1<<7)         // BDU: block data update
                   | (3<<4);        // FS1-FS0: Full Scale selection.0-250dps;1-500dps;2-2000dps;3-2000dps
                   
    // Configure gyroscope control register CTRL_REG5
    write_buffer[4] = 0x0
                   | (1<<6)         // FIFO_EN bit 
                   | (0<<4);        // HPen bit 
    
    status = sns_ddf_write_port(
        state->port_handle,
        AUTO_INCREMENT | STM_L3G4200D_CTRL_REG1,
        write_buffer,
        5,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 5)
        return SNS_DDF_EBUS;

    // Configure gyroscope FIFO control register FIFO_CTRL_REG
    write_buffer[0] = 0x0
                   | (2<<5)		//FIFO mode: 0-bypass; 1-fifo; 2-stream; 3-stream to fifo; 4-bypass to stream
                   | 0,			//Watermark level

    status = sns_ddf_write_port(
        state->port_handle,
        STM_L3G4200D_FIFO_CTRL,
        write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;
	
    // Let new values settle in by waiting 5 ms.
    sns_ddf_delay(5000);
				   
    state->range = STM_L3G4200D_RANGE_2000DPS;
    state->gyr_sstvt = STM_L3G4200D_SSTVT_2000;
    state->data_rate = STM_L3G4200D_ODR800;
    state->power_state = SNS_DDF_POWERSTATE_LOWPOWER;
    
    return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set data sampling rate operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  data_rate      Data sampling rate settings for L3G4200D
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_gyr_l3g4200d_set_datarate(
  sns_ddf_handle_t dd_handle, 
  stm_l3g4200d_odr data_rate)
{
    sns_dd_gyr_l3g4200d_state_t* state = dd_handle;
    sns_ddf_status_e status;

    // Configure ODR in CTRL_REG1
    status = sns_dd_gyr_l3g4200d_update_onereg(
        dd_handle, 
        STM_L3G4200D_CTRL_REG1, 
        0xF0, 
        (uint8_t)data_rate);
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }

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
static sns_ddf_status_e sns_dd_gyr_l3g4200d_set_power(
  sns_ddf_handle_t dd_handle, 
  sns_ddf_powerstate_e power_state)
{
    sns_dd_gyr_l3g4200d_state_t* state = dd_handle;
    sns_ddf_status_e status;

    if((SNS_DDF_POWERSTATE_LOWPOWER == power_state)
        &&(SNS_DDF_POWERSTATE_ACTIVE == state->power_state))
    {
        // Configure CTRL_REG1 to enter "power down mode".
        status = sns_dd_gyr_l3g4200d_update_onereg(
            dd_handle, 
            STM_L3G4200D_CTRL_REG1, 
            0x08, 
            0x0);
        if(status != SNS_DDF_SUCCESS)
        {
            return status;
        }

        state->power_state=SNS_DDF_POWERSTATE_LOWPOWER;
                                
    }
    else if((SNS_DDF_POWERSTATE_ACTIVE == power_state)
        &&(SNS_DDF_POWERSTATE_LOWPOWER == state->power_state))
    {   
        // Configure CTRL_REG1 to enter "Normal Mode".
        status = sns_dd_gyr_l3g4200d_update_onereg(
            dd_handle, 
            STM_L3G4200D_CTRL_REG1, 
            0x08, 
            0x08);
        if(status != SNS_DDF_SUCCESS)
        {
            return status;
        }

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
 * @param[in]  range          Dynamic range settings for L3G4200D
 * 
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_gyr_l3g4200d_set_range(
  sns_ddf_handle_t dd_handle, 
  stm_l3g4200d_range range)
{
    sns_dd_gyr_l3g4200d_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;
    stm_l3g4200d_sstvt sstvt;

    //set sensor sensitivity for data conversion
    switch (range) {
        case STM_L3G4200D_RANGE_250DPS:
            sstvt = STM_L3G4200D_SSTVT_250;
            break;
        case STM_L3G4200D_RANGE_500DPS:
            sstvt = STM_L3G4200D_SSTVT_500;
            break;
        case STM_L3G4200D_RANGE_2000DPS:
            sstvt = STM_L3G4200D_SSTVT_2000;
            break;
        default:
            return SNS_DDF_EINVALID_PARAM;  
    }
    
    // Configure CTRL_REG4
    write_buffer = ((uint8_t)range)|(1<<7); 
    status = sns_ddf_write_port(
        state->port_handle,
        STM_L3G4200D_CTRL_REG4,
        &write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    state->range = range;
    state->gyr_sstvt = sstvt;

    // Let dynamic range value settle in by waiting 5 ms.
    sns_ddf_delay(5000);

    return SNS_DDF_SUCCESS;
}

/** 
 * @brief Initializes the l3g4200d driver and sets up sensor. 
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_gyr_l3g4200d_init(
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
    sns_dd_gyr_l3g4200d_state_t* state;
    static sns_ddf_sensor_e my_sensors[] = {
        SNS_DDF_SENSOR_GYRO};
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    
    // Allocate a driver instance.
    status = sns_ddf_malloc((void**)&state, sizeof(sns_dd_gyr_l3g4200d_state_t));
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

    // wait sensor powering up to be ready by delaying 5 ms.
    sns_ddf_delay(5000);

    // confirm WHO_AM_I register
    status = sns_ddf_read_port(
        state->port_handle,
        STM_L3G4200D_WHO_AM_I,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if((rw_bytes!=1)||(rw_buffer!=0xd3))
        return SNS_DDF_EFAIL;
    
    // Resets the l3g4200d
    status = sns_dd_gyr_l3g4200d_reset(state);
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
 * @brief Retrieves a single set of angular rate data from l3g4200d.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_gyr_l3g4200d_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data)
{
    uint8_t read_buffer[STM_L3G4200D_NUM_READ_BYTES] = { 0 };
    uint8_t read_count = 0;
    sns_ddf_time_t timestamp;
    sns_ddf_status_e status;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_gyr_l3g4200d_state_t *state = dd_handle;
    float sample_data;
    // sensor coordinate x,y,z axis raw hardware data
    int16_t hw_d[STM_L3G4200D_NUM_AXES] = { 0 };
    // SAE coordinate x,y,z axis data
    int16_t sae_d[STM_L3G4200D_NUM_AXES] = { 0 };
    bool fifo_empty = true;
	
    //if current power mode is LOWPOWER , return error.
    if((SNS_DDF_POWERSTATE_LOWPOWER == state->power_state)||(num_sensors != 1))
        return SNS_DDF_EDEVICE;

    //Loop should only execute only once, since current ODR=800Hz. 
	while(true == fifo_empty)
	{
	    status = sns_dd_gyr_l3g4200d_is_fifo_empty(dd_handle, &fifo_empty);
		if(status != SNS_DDF_SUCCESS)
            return status;
	}

	// This is a synchronous driver, so read data now.
	// Loop exit only when FIFO is empty.
	while(false == fifo_empty)
	{
	    status = sns_ddf_read_port(
	        state->port_handle,
	        AUTO_INCREMENT | STM_L3G4200D_OUT_X_L,
	        read_buffer,
	        STM_L3G4200D_NUM_READ_BYTES,
	        &read_count);
	    if(status != SNS_DDF_SUCCESS)
	        return status;
	    if(read_count != STM_L3G4200D_NUM_READ_BYTES)
	        return SNS_DDF_EBUS;

	    status = sns_dd_gyr_l3g4200d_is_fifo_empty(dd_handle, &fifo_empty);
		if(status != SNS_DDF_SUCCESS)
            return status;
	}
	
    // get current time stamp
    timestamp = sns_ddf_get_timestamp();
    
    //convert the raw data in read_buffer to X/Y/Z axis sensor data
    hw_d[0] = (int16_t) (((read_buffer[0]) << 8) | read_buffer[1]);
    hw_d[1] = (int16_t) (((read_buffer[2]) << 8) | read_buffer[3]);
    hw_d[2] = (int16_t) (((read_buffer[4]) << 8) | read_buffer[5]);

    //convert the sensor data to SAE coordinate
    sae_d[0] = (1 == STM_GYR_POLARITY_X)? hw_d[STM_GYR_AXIS_MAP_X] : -hw_d[STM_GYR_AXIS_MAP_X];
    sae_d[1] = (1 == STM_GYR_POLARITY_Y)? hw_d[STM_GYR_AXIS_MAP_Y] : -hw_d[STM_GYR_AXIS_MAP_Y];
    sae_d[2] = (1 == STM_GYR_POLARITY_Y)? hw_d[STM_GYR_AXIS_MAP_Z] : -hw_d[STM_GYR_AXIS_MAP_Z];
    
    state->out_of_bound_flag = 0;

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
                            STM_L3G4200D_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
    if(NULL == data_ptr->samples)
        return SNS_DDF_ENOMEM;

    //Fill out sensor output data and status. 
    //Axis and polarity conversion are configured in the header file.
    sample_data = (sae_d[0] * ((float)state->gyr_sstvt / 100) / 1000) * PI /180;
    data_ptr->samples[0].sample = FX_FLTTOFIX_Q16(sample_data);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    sample_data = (sae_d[1] * ((float)state->gyr_sstvt / 100) / 1000) * PI /180;
    data_ptr->samples[1].sample = FX_FLTTOFIX_Q16(sample_data);
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    sample_data = (sae_d[2] * ((float)state->gyr_sstvt / 100) / 1000) * PI /180;
    data_ptr->samples[2].sample = FX_FLTTOFIX_Q16(sample_data);
    data_ptr->samples[2].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples = STM_L3G4200D_NUM_AXES;
    
    return SNS_DDF_SUCCESS;
}

/** 
 * @brief Retrieves the value of an attribute for a l3g4200d sensor.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_gyr_l3g4200d_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems)
{
    sns_dd_gyr_l3g4200d_state_t *state = dd_handle;
    
    if(sensor != SNS_DDF_SENSOR_GYRO)
        return SNS_DDF_EINVALID_PARAM;
    
    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_INFO:
        {
            sns_ddf_power_info_s *power;
            power = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s));
            if(NULL == power)
                return SNS_DDF_ENOMEM;

            //current consumption, unit uA
            power->active_current = 6100;
            power->lowpower_current = 5;

            *value = power;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *ranges;
            ranges = sns_ddf_memhandler_malloc(memhandler, 
                         STM_L3G4200D_RANGE_NUM * sizeof(sns_ddf_range_s));
            if(NULL == ranges)
                return SNS_DDF_ENOMEM;

            ranges[0].min = STM_L3G4200D_RANGE_250_MIN;
            ranges[0].max = STM_L3G4200D_RANGE_250_MAX;
            ranges[1].min = STM_L3G4200D_RANGE_500_MIN;
            ranges[1].max = STM_L3G4200D_RANGE_500_MAX;
            ranges[2].min = STM_L3G4200D_RANGE_2000_MIN;
            ranges[2].max = STM_L3G4200D_RANGE_2000_MAX;

            *num_elems = STM_L3G4200D_RANGE_NUM;
            *value = ranges;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t *res;
            res = sns_ddf_memhandler_malloc(memhandler ,sizeof(sns_ddf_resolution_t));
            if(NULL == res)
                return SNS_DDF_ENOMEM;

            *res = FX_FLTTOFIX_Q16((state->gyr_sstvt/1000) * PI /180);
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
          device_res->bit_len = STM_GYR_L3G4200D_SENSOR_BIT_LEN;
          device_res->max_freq = 800;

          return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            *value = l3g4200d_freqs;
            *num_elems = STM_L3G4200D_ODR_NUM;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s *l3g4200d_delays;
            l3g4200d_delays = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_delays_s));
            if(NULL == l3g4200d_delays)
                return SNS_DDF_ENOMEM;

            l3g4200d_delays->time_to_active = 5;
            l3g4200d_delays->time_to_data = 1;
            *value = l3g4200d_delays;
            *num_elems = 1;
            
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s *info;
            info = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_driver_info_s));
            if(NULL == info)
                return SNS_DDF_ENOMEM;

            info->name = "STM L3G4200D Gyroscope Driver";
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

            info->name = "Gyroscope";
            info->vendor = "STMicroelectronics";
            info->model = "L3G4200D";
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
 * @brief Sets a l3g4200d sensor attribute to a specific value.
 *  
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_gyr_l3g4200d_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value)
{
    if(sensor != SNS_DDF_SENSOR_GYRO)
        return SNS_DDF_EINVALID_PARAM;
    
    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_STATE:
        {
            sns_ddf_powerstate_e* power_state = value;
            
            return sns_dd_gyr_l3g4200d_set_power(dd_handle, *power_state);
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            uint32_t* range_idx = value;
            
            stm_l3g4200d_range range = l3g4200d_ranges[*range_idx];
            if(range > STM_L3G4200D_MAX_RANGE)
            {
                return SNS_DDF_EINVALID_PARAM;
            }
            
            return sns_dd_gyr_l3g4200d_set_range(dd_handle, range);
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            uint32_t* freqs_idx = value;
            
            stm_l3g4200d_odr freqs = l3g4200d_odr_setting[*freqs_idx];
            if(freqs > STM_L3G4200D_MAX_ODR)
            {
                return SNS_DDF_EINVALID_PARAM;
            }
            
            return sns_dd_gyr_l3g4200d_set_datarate(dd_handle, freqs);
        }

        default:
            return SNS_DDF_EINVALID_ATTR;
    }
}

/** 
 * L3G4200D Gyroscope device driver interface.
 */
sns_ddf_driver_if_s sns_dd_gyr_l3g4200d_if =
{
    &sns_dd_gyr_l3g4200d_init,
    &sns_dd_gyr_l3g4200d_get_data,
    &sns_dd_gyr_l3g4200d_set_attr,
    &sns_dd_gyr_l3g4200d_get_attr,
    NULL,   // handle timer
    NULL,   // handle irq
    &sns_dd_gyr_l3g4200d_reset
};

