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
* VERSION | DATE          | DESCRIPTION
*         | 06/19/2012    | (vh) Added run time switch for I2C code
*         | 05/02/2012    | Added sns_dd_acc_lis3dh_handle_int to sns_dd_acc_lis3dh_if
*                         | use sns_dd_acc_lis3dh_disable_int return status for disabling MD status
*         | 03/05/2012    | Changed interrupt APIs
* 1.0.1   | 09/15/2011    | Removed delay in reset/set ODR/set range functions
* 1.0.0   | 08/29/2011    | Switch motion detection interrupt to INT pin 2.
*         | 08/23/2011    | Updated LPF frequency array to comply with DDF spec
*         | 08/13/2011    | Changed the unit of LOWPASS to q16_t
*         | 07/15/2011    | Added Driver level logpacket
*         | 06/17/2011    | Fixed Z-axis data conversion issue
*         | 05/25/2011    | Changed to adapt SENSOR__ALL too in addition to SENSOR_ACCEL
* 0.9.3   | 05/24/2011    | Disabled interrupt latch bit.
* 0.9.2   | 05/20/2011    | Merged bug fixes from STM and renumbered versions.
* 0.9.1   | 05/05/2011    | Used a header file to configure axis and polarity.
* 0.9.0   | 04/27/2011    | Created with motion detection support.
*******************************************************************************/
#include "sns_dd_accel_lis3dh.h"
#include "sns_dd_stm_config.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_signal.h"
//Include files required to support accel log packet
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include <string.h>
#include "sns_memmgr.h"

#define STM_LIS3DH_NUM_READ_BYTES  6
#define AUTO_INCREMENT             0x80

/*------------------------------------------------------
Extern Variables
------------------------------------------------------*/
extern volatile boolean EnableI2C;
extern sns_ddf_driver_if_s sns_dd_acc_lis3dh_if;

/*------------------------------------------------------
  Access to externalized functions.
------------------------------------------------------*/

extern sns_ddf_status_e sns_dd_acc_lis3dh_init(
    sns_ddf_handle_t* dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors);

extern sns_ddf_status_e sns_dd_acc_lis3dh_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems);

extern sns_ddf_status_e sns_dd_acc_lis3dh_disable_int(
    sns_ddf_handle_t dd_handle);

/*------------------------------------------------------
  Function prototypes
------------------------------------------------------*/
sns_ddf_status_e sns_dd_acc_lis3dh_reset(sns_ddf_handle_t dd_handle);

sns_ddf_status_e sns_dd_acc_lis3dh_probe(
  sns_ddf_device_access_s*  device_info,
  sns_ddf_memhandler_s*     memhandler,
  uint32_t*                 num_sensors,
  sns_ddf_sensor_e**        sensors );

sns_ddf_status_e sns_dd_acc_lis3dh_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value);


static sns_ddf_status_e sns_dd_acc_lis3dh_set_datarate(
  sns_ddf_handle_t dd_handle,
  stm_lis3dh_odr data_rate);


static sns_ddf_status_e sns_dd_acc_lis3dh_set_power(
  sns_ddf_handle_t dd_handle,
  sns_ddf_powerstate_e power_state);

static sns_ddf_status_e sns_dd_acc_lis3dh_set_range(
  sns_ddf_handle_t dd_handle,
  stm_lis3dh_range range);


static sns_ddf_status_e sns_dd_acc_lis3dh_enable_int(
    sns_ddf_handle_t dd_handle);

 
 
/*------------------------------------------------------
  Structures
------------------------------------------------------*/
/**
 * Accelerometer LIS3DH ranges array for setting attribute
 */
stm_lis3dh_range lis3dh_ranges[STM_LIS3DH_RANGE_NUM]={
            STM_LIS3DH_RANGE_2G,
            STM_LIS3DH_RANGE_4G,
            STM_LIS3DH_RANGE_8G,
            STM_LIS3DH_RANGE_16G};

/**
 * Accelerometer LIS3DH sampling frequencies array for getting attribute
 */
sns_ddf_lowpass_freq_t lis3dh_freqs[STM_LIS3DH_ODR_NUM] = {
            FX_FLTTOFIX_Q16(0.5),
            FX_FLTTOFIX_Q16(5),
            FX_FLTTOFIX_Q16(12.5),
            FX_FLTTOFIX_Q16(25),
            FX_FLTTOFIX_Q16(50),
            FX_FLTTOFIX_Q16(100),
            FX_FLTTOFIX_Q16(200),
            FX_FLTTOFIX_Q16(625)
};

/**
 * Accelerometer LIS3DH sampling frequencies array for setting attribute
 */
stm_lis3dh_odr lis3dh_odr_setting[STM_LIS3DH_ODR_NUM] = {
            STM_LIS3DH_ODR1,
            STM_LIS3DH_ODR10,
            STM_LIS3DH_ODR25,
            STM_LIS3DH_ODR50,
            STM_LIS3DH_ODR100,
            STM_LIS3DH_ODR200,
            STM_LIS3DH_ODR400,
            STM_LIS3DH_ODR1250};


/*------------------------------------------------------
  Functions
------------------------------------------------------*/
/**
  * @brief Resets the driver and device so they return to the state they were
  *        in after init() was called.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_acc_lis3dh_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_acc_lis3dh_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer[6] = { 0 };
    uint8_t rw_bytes = 0;
    /** Reset values for the 6 control registers. */
    static const uint8_t control_regs[6] = {
      /* CTRL_REG1 */
      0x0
      | (0<<7)           // ODR3 bit
      | (0<<6)           // ODR2 bit
      | (0<<5)           // ODR1 bit
      | (0<<4)           // ODR0 bit
      | (0<<3)           // Low power mode bit
      | (1<<2)           // Z axis enable bit
      | (1<<1)           // Y axis enable bit
      | 1,               // X axis enable bit
      /* CTRL_REG2 */
      0x0
      | (0<<6)           // HPM1-HPM0 high pass filter mode selection
      | (0<<4)           // High pass filter cut off frequency
      | (0<<3)           // FDS: Filtered data selection
      | 1,               // HPIS1: High pass filter for AOI function on interrupt 1 enable bit.
      /* CTRL_REG3 */
      0
      | (0<<6),          // I1_AOI1: Interrupt generator 1 on INT1 pin.
      /* CTRL_REG4 */
      0x0
      | (1<<7)           // Block data update bit
      | (0<<6)           // Big/Little endian data selection bit
      | (0<<5)           // FS1 bit
      | (0<<4)           // FS0 bit
      | (1<<3)           // High resolution output mode bit
      | (0<<2)           // ST1 bit
      | (0<<1)           // ST0 bit
      | 0,               // SIM bit
      /* CTRL_REG5 */
      0x0
      | (0<<6)           // FIFO_EN bit
      | (1<<3),          // LIR_INT1 bit
      /* CTRL_REG6 */
      0x0
      | (0<<7)           // I2_CLICKen bit
      | (0<<6)           // I2_INT1 bit
      | (0<<5)           // always 0
      | (0<<4)           // BOOT_I2 bit
      | (0<<3)           // always 0
      | (0<<1)           // H_LACTIVE bit
    };

    // Reset driver state.
    memset(state->bias, 0, STM_LIS3DH_NUM_AXES * sizeof(q16_t));
    state->out_of_bound_flag = 0;
    memset(state->data_cache, 0, STM_LIS3DH_NUM_AXES * sizeof(q16_t));

    // Configure temperature control register
    rw_buffer[0] = 0x0
             | (1<<7)          // ADC_PD#: 0-ADC disabled; 1-ADC enabled.
             | (0<<6)          // 0-Temperature sensor disabled; 1-enabled.
             | 0;

    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DH_TEMP_REG_A,
        rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    // Configure accelerometer control register REG1-REG6
    SNS_OS_MEMCOPY(rw_buffer, control_regs, 6*sizeof(uint8_t));

    status = sns_ddf_write_port(
        state->port_handle,
        AUTO_INCREMENT | STM_LIS3DH_CTRL_REG1_A,
        rw_buffer,
        6,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 6)
        return SNS_DDF_EBUS;

    // Configure accelerometer FIFO register
    rw_buffer[0]  = 0x0
             | (0<<6)            // FM[1:0]: 0-bypass;1-FIFO;2-stream;3-tirgger
             | (0<<5)            // Trigger mode:
             | 0;                // FTH[4:0]

    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DH_FIFO_CTRL_A,
        rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    state->range = STM_LIS3DH_RANGE_2G;
    state->sstvt_adj = STM_LIS3DH_SSTVT_2G;
    state->data_rate = STM_LIS3DH_ODR_OFF;
    state->prvs_rate = STM_LIS3DH_ODR50;
    state->power_state = SNS_DDF_POWERSTATE_LOWPOWER;

    return SNS_DDF_SUCCESS;
}

/**
 * @brief Probes for an LIS3DH part.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_acc_lis3dh_probe(
  sns_ddf_device_access_s*  device_info,
  sns_ddf_memhandler_s*     memhandler,
  uint32_t*                 num_sensors,
  sns_ddf_sensor_e**        sensors )
{

    sns_ddf_status_e status;
    sns_ddf_handle_t port_handle;
    uint8_t i2c_buff[6];
    uint8_t bytes_read;
    
    /** Power-on values for the 6 control registers. */
    static const uint8_t control_regs[6] = {
      /* CTRL_REG1 */
      0x0
      | (0<<7)           // ODR3 bit
      | (0<<6)           // ODR2 bit
      | (0<<5)           // ODR1 bit
      | (0<<4)           // ODR0 bit
      | (0<<3)           // Low power mode bit
      | (1<<2)           // Z axis enable bit
      | (1<<1)           // Y axis enable bit
      | 1,               // X axis enable bit
      /* CTRL_REG2 */
      0x0
      | (0<<6)           // HPM1-HPM0 high pass filter mode selection
      | (0<<4)           // High pass filter cut off frequency
      | (0<<3)           // FDS: Filtered data selection
      | 0,               // HPIS1: High pass filter for AOI function on interrupt 1 enable bit.
      /* CTRL_REG3 */
      0
      | (0<<6),          // I1_AOI1: Interrupt generator 1 on INT1 pin.
      /* CTRL_REG4 */
      0x0
      | (0<<7)           // Block data update bit
      | (0<<6)           // Big/Little endian data selection bit
      | (0<<5)           // FS1 bit
      | (0<<4)           // FS0 bit
      | (0<<3)           // High resolution output mode bit
      | (0<<2)           // ST1 bit
      | (0<<1)           // ST0 bit
      | 0,               // SIM bit
      /* CTRL_REG5 */
      0x0
      | (0<<6)           // FIFO_EN bit
      | (0<<3),          // LIR_INT1 bit
      /* CTRL_REG6 */
      0x0
      | (0<<7)           // I2_CLICKen bit
      | (0<<6)           // I2_INT1 bit
      | (0<<5)           // always 0
      | (0<<4)           // BOOT_I2 bit
      | (0<<3)           // always 0
      | (0<<1)           // H_LACTIVE bit
    };
    
    *num_sensors = 0;
    *sensors = NULL;

    status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
    if(status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    // Don't check WhoAmI Register
    // Unfortunately, the LSM303DLHC accel part doesn't support a
    // WhoAmI register accessible via the accel I2C address.

    // Check CTRL_REG1-6 to make sure this is the correct hardware
    status = sns_ddf_read_port(
        port_handle,
        AUTO_INCREMENT | STM_LIS3DH_CTRL_REG1_A,
        i2c_buff,
        6,
        &bytes_read);
    if(status != SNS_DDF_SUCCESS || bytes_read != 6)
    {
        sns_ddf_close_port( port_handle );
        return status;
    }
    if(0 != SNS_OS_MEMCMP(i2c_buff, control_regs, 6))
    {
        /* Incorrect value. Return now with nothing detected */
        sns_ddf_close_port( port_handle );
        return SNS_DDF_SUCCESS;
    }

    // CTRL_REG1-6 registers are correct. This is probably an LIS3DH or the
    // accel part of an LSM303DLHC.
    *num_sensors = 1;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
                                          sizeof(sns_ddf_sensor_e) * *num_sensors );
    if( *sensors != NULL ) 
    {
        (*sensors)[0] = SNS_DDF_SENSOR_ACCEL;
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
 * @param[in]  data_rate       Data sampling rate settings for LIS3DH
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dh_set_datarate(
  sns_ddf_handle_t dd_handle,
  stm_lis3dh_odr data_rate)
{
    sns_dd_acc_lis3dh_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;

    // Configure ODR in CTRL_REG1
    write_buffer = ((uint8_t)data_rate)|0x7;
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DH_CTRL_REG1_A,
        &write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    state->data_rate= data_rate;

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
static sns_ddf_status_e sns_dd_acc_lis3dh_set_power(
  sns_ddf_handle_t dd_handle,
  sns_ddf_powerstate_e power_state)
{
    sns_dd_acc_lis3dh_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;

    if((SNS_DDF_POWERSTATE_LOWPOWER == power_state)
        &&(SNS_DDF_POWERSTATE_ACTIVE == state->power_state))
    {
        // Configure ODR of control register 1 to enter power down mode.
        write_buffer = 0x0
                    | (0<<7)             // ODR3 bit
                    | (0<<6)             // ODR2 bit
                    | (0<<5)             // ODR1 bit
                    | (0<<4)             // ODR0 bit
                    | (0<<3)             // Low power mode bit
                    | (0<<2)             // Z axis enable bit
                    | (0<<1)             // Y axis enable bit
                    | 0;                 // X axis enable bit

        status = sns_ddf_write_port(
            state->port_handle,
            STM_LIS3DH_CTRL_REG1_A,
            &write_buffer,
            1,
            &bytes_written);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(bytes_written != 1)
            return SNS_DDF_EBUS;

        state->prvs_rate = state->data_rate;
        state->data_rate = STM_LIS3DH_ODR_OFF;
        state->power_state = SNS_DDF_POWERSTATE_LOWPOWER;

    }
    else if((SNS_DDF_POWERSTATE_ACTIVE == power_state)
        &&(SNS_DDF_POWERSTATE_LOWPOWER == state->power_state))
    {
        // call sns_dd_acc_lis3dh_set_datarate to enter ACTIVE mode
        // and recover sensor's previous active sampling rate.
        status = sns_dd_acc_lis3dh_set_datarate(dd_handle, state->prvs_rate);
        if(status != SNS_DDF_SUCCESS)
        {
            return status;
        }

        state->prvs_rate= STM_LIS3DH_ODR_OFF;
        state->power_state= SNS_DDF_POWERSTATE_ACTIVE;
    }

    return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set dynamic range operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  range          Dynamic range settings for LIS3DH
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dh_set_range(
  sns_ddf_handle_t dd_handle,
  stm_lis3dh_range range)
{
    sns_dd_acc_lis3dh_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer = 0;
    uint8_t bytes_written = 0;
    stm_lis3dh_sstvt sstvt;

    //set sensor sensitivity for data conversion
    switch (range) {
        case STM_LIS3DH_RANGE_2G:
            sstvt = STM_LIS3DH_SSTVT_2G;
            break;
        case STM_LIS3DH_RANGE_4G:
            sstvt = STM_LIS3DH_SSTVT_4G;
            break;
        case STM_LIS3DH_RANGE_8G:
            sstvt = STM_LIS3DH_SSTVT_8G;
            break;
        case STM_LIS3DH_RANGE_16G:
            sstvt = STM_LIS3DH_SSTVT_16G;
            break;
        default:
            return SNS_DDF_EINVALID_PARAM;
    }

    // Configure CTRL_REG4
    write_buffer = ((uint8_t)range)|(1<<3);
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DH_CTRL_REG4_A,
        &write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    state->range = range;
    state->sstvt_adj = sstvt;

  
    return SNS_DDF_SUCCESS;
}

 /**
 * @brief Enable accelerometer to generate motion detection interrupt.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dh_enable_int(
    sns_ddf_handle_t dd_handle)
{
    sns_dd_acc_lis3dh_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;

    // Enable AOI1 interrupt on INT1
    rw_buffer = 0
             | (1<<6);          // I2_INT1: Interrupt generator 1 on INT2 pin.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DH_CTRL_REG6_A,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    // Configure accelerometer Interrupt 1 threshold register, threshold = 48mg
    rw_buffer  = 0x3;
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DH_INT1_THS,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    // Configure accelerometer Interrupt 1 duration register, duration = 3
    rw_buffer  = 0x3;
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DH_INT1_DURATION,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    // Dummy read HP_FILTER_RESET
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LIS3DH_REFERENCE,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    // Configure accelerometer Interrupt 1 config register per desired wake-up event.
    rw_buffer  = 0x2A;
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DH_INT1_CFG,
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
 * @brief Sets a lis3dh sensor attribute to a specific value.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_acc_lis3dh_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value)
{
    sns_ddf_status_e status;

    if((sensor != SNS_DDF_SENSOR_ACCEL) && (sensor != SNS_DDF_SENSOR__ALL))
        return SNS_DDF_EINVALID_PARAM;

    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_STATE:
        {
            sns_ddf_powerstate_e* power_state = value;
            return sns_dd_acc_lis3dh_set_power(dd_handle, *power_state);
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            uint32_t* range_idx = value;
            stm_lis3dh_range range = lis3dh_ranges[*range_idx];
            if(range > STM_LIS3DH_MAX_RANGE)
                return SNS_DDF_EINVALID_PARAM;

            return sns_dd_acc_lis3dh_set_range(dd_handle, range);
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            uint32_t* freqs_idx = value;
            stm_lis3dh_odr freqs = lis3dh_odr_setting[*freqs_idx];
            if(freqs > STM_LIS3DH_MAX_ODR)
                return SNS_DDF_EINVALID_PARAM;

            return sns_dd_acc_lis3dh_set_datarate(dd_handle, freqs);
        }

        case SNS_DDF_ATTRIB_MOTION_DETECT:
        {
            sns_dd_acc_lis3dh_state_t* state = dd_handle;
            sns_ddf_motion_detect_enable_t* flag = value;

            if(!sns_ddf_signal_irq_enabled())
                return SNS_DDF_EFAIL;

            if(*flag)
            {
                //enable motion detection
                status = sns_dd_acc_lis3dh_enable_int(dd_handle);
                if (status!=SNS_DDF_SUCCESS)
                    return status;

                return sns_ddf_signal_register(
                            state->gpio_num,
                            state,
                            &sns_dd_acc_lis3dh_if,
                            SNS_DDF_SIGNAL_IRQ_RISING);
            }

            //disable motion detection
            status = sns_dd_acc_lis3dh_disable_int(dd_handle);
            sns_ddf_signal_deregister(state->gpio_num);
            return status;
        }

        default:
            return SNS_DDF_EINVALID_ATTR;
    }
}
