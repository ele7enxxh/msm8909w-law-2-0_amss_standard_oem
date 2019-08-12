/******************** (C) COPYRIGHT 2014 STMicroelectronics ********************
*
* File Name         : sns_dd_press_lps25h.c
* Authors           : Jianjian Huo
*                   : Wei Wang
* Version           : V 1.0.9
* Date              : 09/30/2014
* Description       : LPS25H pressure sensor driver source file
*
********************************************************************************
* Copyright (c) 2014, STMicroelectronics.
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
* 1.0.9   | 09/30/14      | Improve accuracy (Wei Wang)
* 1.0.8   | 09/17/14      | Updated memhandler api in probe fn (VY)
* 1.0.7   | 08/24/14      | Used new DDF apis to allocate/free mem supported in uImage (VY)
* 1.0.6   | 07/27/2014    | Refactored for uImage. (VY)
* 1.0.5   | 05/09/2014    | Updated SNS_DDF_TEST_SELF implementation. (M. Wahegaonkar)
* 1.0.4   | 04/10/2014    | Added timer handling for self-test function. (M. Wahegaonkar)
* 1.0.3   | 04/09/2014    | Add 25Hz.
*         |               | Remove sns_dd_press_disable_FIFO().
*         |               | Update sns_dd_press_lps25h_set_datarate(). (W. Wang)
* 1.0.2   | 04/08/2014    | Added data log packet support, probe function. (M. Wahegaonkar)
* 1.0.1   | 04/08/2014    | Implement sns_dd_press_lps25h_run_test().
*         |               | sns_dd_press_lps25h_get_data(): add default to switch{}.
*         |               | sns_dd_press_lps25h_get_attr() > case SNS_DDF_ATTRIB_RESOLUTION: correct. (ww)
* 1.0.0   | 11/01/2012    | Improved reading accuracy by set sensor's internal sampling rate
*         |               | fixed at 12.5Hz with FIFO average mode.
* 0.9.0   | 10/22/2012    | Created.
*******************************************************************************/

/*==============================================================================
Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "sns_dd_press_lps25h.h"


/* Extern dreclaration of function pointer list */
extern sns_ddf_driver_if_s sns_dd_press_lps25h_if;

/**
 * Pressure sensor LPS25H low pass frequencies array for getting attribute
 */
static sns_ddf_lowpass_freq_t lps25h_freqs[STM_LPS25H_ODR_NUM] = {
            FX_FLTTOFIX_Q16(0.5),
            FX_FLTTOFIX_Q16(3.5),
            FX_FLTTOFIX_Q16(6.25),
            FX_FLTTOFIX_Q16(12.5)
};

/**
 * Pressure sensor LPS25H sampling frequencies array for setting attribute
 */
static stm_lps25h_odr lps25h_odr_setting[STM_LPS25H_ODR_NUM] = {
            STM_LPS25H_ODR_1_1,
            STM_LPS25H_ODR_7_7,
            STM_LPS25H_ODR_12p5_12p5,
            STM_LPS25H_ODR_25_25
};


/**
 * @brief Update only one part of one register in LPS25H
 *
 * @param[in]  dd_handle   Handle to a driver instance.
 * @param[in]  reg_addr    The address of LPS25H register to be updated
 * @param[in]  mask        The mask of this register to specify which part to be updated
 * @param[in]  new_value   The new content for the specified part of this register
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_press_update_onereg(
  sns_ddf_handle_t dd_handle,
  uint8_t reg_addr,
  uint8_t mask,
  uint8_t new_value)
{
    sns_dd_press_lps25h_state_t* state = dd_handle;
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
 * @brief Set ODR related registers in LPS25H
 *
 * @param[in]  dd_handle   Handle to a driver instance.
 * @param[in]  reg_addr    The address of LPS25H register to be updated
 * @param[in]  mask        The mask of this register to specify which part to be updated
 * @param[in]  new_value   The new content for the specified part of this register
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_press_set_ODR_regs(
  sns_ddf_handle_t dd_handle,
  uint8_t reg_RES,
  uint8_t reg_ODR,
  uint8_t reg_FIFO)
{
    sns_dd_press_lps25h_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t bytes_written = 0;

    status = sns_dd_press_update_onereg(
        dd_handle,
        STM_LPS25H_RES_ALLOW,
        (uint8_t)0x80,
        (uint8_t)0x80);
    if(status != SNS_DDF_SUCCESS)
        return status;

    // Configure pressure sensor resolution (register RES_CONF(10h))
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LPS25H_RES_CONF,
        &reg_RES,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    // Configure power mode/data rate (register CTRL_REG1(20h))
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LPS25H_CTRL_REG1,
        &reg_ODR,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    // set FIFO in median filter mode.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LPS25H_FIFO_CTRL,
        &reg_FIFO,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    return SNS_DDF_SUCCESS;
}

/**
 * @brief Resets the driver and device so they return to the state they were
 *        in after init() was called.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_press_lps25h_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_press_lps25h_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer[3] = {0, 0, 0};
    uint8_t bytes_written = 0;

    // Reset driver state.
    // power down device in order to set control registers.
    write_buffer[0] = 0x0;
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LPS25H_CTRL_REG1,
        write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    // wait sensor to power down by delaying 1 ms.
    sns_ddf_delay(1000);

    // set data rate to 12.5Hz
    write_buffer[0] = 0x0f;
    write_buffer[1] = 0x0
                   | (0<<7)                        // PD (power down control, 0 for power-down, 1 for active)
                   | (STM_LPS25H_ODR_25_25<<4) // ODR2:0 (Bits [6:4]) - data rate
                   | (0<<3)                        // DIFF_EN (interrupt enable)
                   | (1<<2)                        // BDU (block data update)
                   | (0<<1)                        // RESET_AZ
                   | (0);                          // SIM (SPI Serial Interface Mode selection)
    write_buffer[2] = 0x0
                   | (0x06<<5)                     // F_MODE2:0
                   | 0x1F;                         // WTM_POINT4:0
    status = sns_dd_press_set_ODR_regs(dd_handle,
                                       write_buffer[0],
                                       write_buffer[1],
                                       write_buffer[2]);
    if(status != SNS_DDF_SUCCESS)
        return status;

    status = sns_dd_press_enable_FIFO(dd_handle);
    if(status != SNS_DDF_SUCCESS)
        return status;
    state->FIFO_enable = 1;

    state->self_test_enable = 0;

    state->data_rate = STM_LPS25H_ODR_25_25;
    state->power_state = SNS_DDF_POWERSTATE_LOWPOWER;

    return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set data sampling rate operation
 *        -- Must set power to low before doing this
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  data_rate       Data sampling rate settings for LPS25H
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_press_lps25h_set_datarate(
  sns_ddf_handle_t dd_handle,
  stm_lps25h_odr data_rate)
{
    sns_dd_press_lps25h_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t ctrl_reg1;
    uint8_t rw_bytes;
    if(state->data_rate != data_rate)
    {
      if (state->power_state==SNS_DDF_POWERSTATE_LOWPOWER)
      {
        status = sns_ddf_read_port(state->port_handle, STM_LPS25H_CTRL_REG1, &ctrl_reg1, 1, &rw_bytes);
        if (status!=SNS_DDF_SUCCESS) return status;
        if(rw_bytes!=1) return SNS_DDF_EBUS;
        ctrl_reg1=(ctrl_reg1&0x8F)|(data_rate<<4);
        status = sns_ddf_write_port(state->port_handle, STM_LPS25H_CTRL_REG1, &ctrl_reg1, 1, &rw_bytes);
        if(status!=SNS_DDF_SUCCESS) return status;
        if(rw_bytes!=1) return SNS_DDF_EBUS;
      }
      else
      {
        // read CTRL_REG1
        status = sns_ddf_read_port(state->port_handle, STM_LPS25H_CTRL_REG1, &ctrl_reg1, 1, &rw_bytes);
        if (status!=SNS_DDF_SUCCESS) return status;
        if(rw_bytes!=1) return SNS_DDF_EBUS;
        // set new ODR and power down
        ctrl_reg1=(ctrl_reg1&0x0F)|(data_rate<<4);
        // write CTRL_REG1
        status = sns_ddf_write_port(state->port_handle, STM_LPS25H_CTRL_REG1, &ctrl_reg1, 1, &rw_bytes);
        if(status!=SNS_DDF_SUCCESS) return status;
        if(rw_bytes!=1) return SNS_DDF_EBUS;
        // delay 100us
        sns_ddf_delay(100);
        // set power on, and write CTRL_REG1
        ctrl_reg1|=0x80;
        status = sns_ddf_write_port(state->port_handle, STM_LPS25H_CTRL_REG1, &ctrl_reg1, 1, &rw_bytes);
        if(status!=SNS_DDF_SUCCESS) return status;
        if(rw_bytes!=1) return SNS_DDF_EBUS;
      }
        state->data_rate= data_rate;
    }
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
static sns_ddf_status_e sns_dd_press_lps25h_set_power(
  sns_ddf_handle_t dd_handle,
  sns_ddf_powerstate_e power_state)
{
    sns_dd_press_lps25h_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer[1] = {0};
    uint8_t bytes_written = 0;

    if((SNS_DDF_POWERSTATE_LOWPOWER == power_state)
        &&(SNS_DDF_POWERSTATE_ACTIVE == state->power_state))
    {
        // Configure CTRL_REG1 to power-down mode.
        write_buffer[0] = 0x0
                       | (0<<7);                      // PD (0 for power-down)

        status = sns_ddf_write_port(
            state->port_handle,
            STM_LPS25H_CTRL_REG1,
            write_buffer,
            1,
            &bytes_written);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(bytes_written != 1)
            return SNS_DDF_EBUS;

        // disable FIFO.
        /*if(1 == state->FIFO_enable)
        {
            status = sns_dd_press_disable_FIFO(dd_handle);
            if(status != SNS_DDF_SUCCESS)
                return status;
            state->FIFO_enable = 0;
        }*/

        state->power_state=SNS_DDF_POWERSTATE_LOWPOWER;

    }
    else if((SNS_DDF_POWERSTATE_ACTIVE == power_state)
        &&(SNS_DDF_POWERSTATE_LOWPOWER == state->power_state))
    {
        // Configure CTRL_REG1 to active mode.
        write_buffer[0] = 0x0
                       | (1<<7)                        // PD (1 for active)
                       | (STM_LPS25H_ODR_25_25<<4)     // ODR2:0 (Bits [6:4]) - data rate
                       | (0<<3)                        // DIFF_EN (interrupt enable)
                       | (1<<2)                        // BDU (block data update)
                       | (0<<1)                        // RESET_AZ
                       | (0);                          // SIM (SPI Serial Interface Mode selection)

        status = sns_ddf_write_port(
            state->port_handle,
            STM_LPS25H_CTRL_REG1,
            write_buffer,
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
 * @brief Initializes the lps25h driver and sets up sensor.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_press_lps25h_init(
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
    sns_dd_press_lps25h_state_t* state;
    static sns_ddf_sensor_e my_sensors[] = {
        SNS_DDF_SENSOR_PRESSURE, SNS_DDF_SENSOR_TEMP};
    uint8_t read_buffer[1] = {0};
    uint8_t read_count = 0;

    /* Update smgr sensor data for the driver to indicate uImage support        */
    //sns_ddf_smgr_set_uimg_refac(smgr_handle);

    // Allocate a driver instance.
    status = sns_ddf_malloc_ex((void**)&state, sizeof(sns_dd_press_lps25h_state_t), smgr_handle);
    if(status != SNS_DDF_SUCCESS)
        return SNS_DDF_ENOMEM;

    memset(state, 0x0, sizeof(sns_dd_press_lps25h_state_t));
    state->smgr_hndl = smgr_handle;

     // Open communication port to the device.
    status = sns_ddf_open_port(&state->port_handle, &device_info->port_config);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_mfree_ex(state, smgr_handle);
        return status;
    }

    // wait sensor powering up to be ready by delaying 5 ms.
    sns_ddf_delay(5000);

    // Check WhoAmIRegister to make sure this is the correct driver
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LPS25H_WHO_AM_I,
        read_buffer,
        1,
        &read_count);
    if (status != SNS_DDF_SUCCESS)
    {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree_ex(state, smgr_handle);
        return status;
    }
    if (read_count != 1)
    {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree_ex(state, smgr_handle);
        return SNS_DDF_EBUS;
    }
#ifndef VIRTIO_BRINGUP
    if (read_buffer[0] != STM_LPS25H_WHO_AM_I_VALUE)
    {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree_ex(state, smgr_handle);
        return SNS_DDF_EFAIL;
    }
#endif

    // Resets the LPS25H
    status = sns_dd_press_lps25h_reset(state);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree_ex(state, smgr_handle);
        return status;
    }

    status = sns_ddf_timer_init(&state->timer_obj,
              state,
              &sns_dd_press_lps25h_if,
              state, //always return dd_handle_ptr
              false); // not periodic
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree_ex(state, smgr_handle);
        return status;
    }

    // Fill out output parameters.
    *num_sensors = 2;
    *sensors = my_sensors;
    *dd_handle_ptr = state;

    return SNS_DDF_SUCCESS;
}


/**
 * @brief Retrieves the value of an attribute for a lps25h sensor.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_press_lps25h_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems)
{
    sns_dd_press_lps25h_state_t *state = dd_handle;
    LPS25H_MSG_1(HIGH, "sns_dd_press_lps25h_get_attr attrib=%d",attrib);

    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_INFO:
        {
            sns_ddf_power_info_s *power;
            power = sns_ddf_memhandler_malloc_ex(memhandler, 
                       sizeof(sns_ddf_power_info_s), state->smgr_hndl);
            if(NULL == power)
                return SNS_DDF_ENOMEM;

            //current consumption, unit uA
            power->active_current = 25;
            power->lowpower_current = 1;

            *value = power;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *range;
            range = sns_ddf_memhandler_malloc_ex(memhandler, 
                       sizeof(sns_ddf_range_s), state->smgr_hndl);
            if(NULL == range)
                return SNS_DDF_ENOMEM;

            if(sensor == SNS_DDF_SENSOR_PRESSURE)
            {
                range->min = STM_LPS25H_PRESS_RANGE_MIN;
                range->max = STM_LPS25H_PRESS_RANGE_MAX;
            }
            else
            {
                range->min = STM_LPS25H_TEMP_RANGE_MIN;
                range->max = STM_LPS25H_TEMP_RANGE_MAX;
            }

            *num_elems = 1;
            *value = range;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t *res;
            res = sns_ddf_memhandler_malloc_ex(memhandler ,
                     sizeof(sns_ddf_resolution_t), state->smgr_hndl);
            if(NULL == res)
                return SNS_DDF_ENOMEM;

            if(sensor == SNS_DDF_SENSOR_PRESSURE)
                *res = STM_LPS25H_MAX_RES_PRESS;
            else
                *res = STM_LPS25H_MAX_RES_TEMP;

            *value = res;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            sns_ddf_resolution_adc_s *res = sns_ddf_memhandler_malloc_ex(
                memhandler ,sizeof(sns_ddf_resolution_adc_s), state->smgr_hndl);
            if(NULL == res)
                return SNS_DDF_ENOMEM;

            res->bit_len = (sensor == SNS_DDF_SENSOR_PRESSURE) ? 24 : 16;
            res->max_freq = STM_LPS25H_MAX_ODR_HZ;
            *value = res;
            *num_elems = 1;
            LPS25H_MSG_2(MED, "SNS_DDF_ATTRIB_RESOLUTION_ADC bit_len=%d max_freq=%d",res->bit_len, res->max_freq);
            return SNS_DDF_SUCCESS;

        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            *value = lps25h_freqs;
            *num_elems = STM_LPS25H_ODR_NUM;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s *lps25h_delays;
            lps25h_delays = sns_ddf_memhandler_malloc_ex(memhandler, 
                               sizeof(sns_ddf_delays_s), state->smgr_hndl);
            if(NULL == lps25h_delays)
                return SNS_DDF_ENOMEM;

            lps25h_delays->time_to_active = 5;
            lps25h_delays->time_to_data = 1;
            *value = lps25h_delays;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s *info;
            info = sns_ddf_memhandler_malloc_ex(memhandler,
                      sizeof(sns_ddf_driver_info_s), state->smgr_hndl);
            if(NULL == info)
                return SNS_DDF_ENOMEM;

            info->name = "STM LPS25H Pressure Sensor Driver";
            info->version = 1;
            *value = info;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DEVICE_INFO:
        {
            sns_ddf_device_info_s *info;
            info = sns_ddf_memhandler_malloc_ex(memhandler, 
                       sizeof(sns_ddf_device_info_s), state->smgr_hndl);
            if(NULL == info)
                return SNS_DDF_ENOMEM;

            info->name = "Pressure sensor";
            info->vendor = "STMicroelectronics";
            info->model = "LPS25H";
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
 * @brief Sets a lps25h sensor attribute to a specific value.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_press_lps25h_set_attr(
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
            LPS25H_MSG_1(MED, "New Power State=%d",*power_state);
            return sns_dd_press_lps25h_set_power(dd_handle, *power_state);
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            int odr_idx = *(int*)value;
            if (odr_idx>=STM_LPS25H_ODR_NUM) return SNS_DDF_EINVALID_PARAM;
            LPS25H_MSG_1(MED, "New ODR setting=%d", lps25h_odr_setting[odr_idx]);
            return sns_dd_press_lps25h_set_datarate(dd_handle, lps25h_odr_setting[odr_idx]);
        }

        default:
            return SNS_DDF_EINVALID_ATTR;
    }
}

/**
 * @brief Runs a factory test case.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_press_lps25h_run_test
(
  sns_ddf_handle_t    dd_handle,
  sns_ddf_sensor_e    sensor_type,
  sns_ddf_test_e      test,
  uint32_t*           err
)
{
    sns_dd_press_lps25h_state_t* state = dd_handle;
    sns_ddf_status_e status;

    LPS25H_MSG_1(MED, "RunTest: %d.", test);

    if ((sensor_type!=SNS_DDF_SENSOR_PRESSURE) && (sensor_type!=SNS_DDF_SENSOR__ALL)) return SNS_DDF_EINVALID_PARAM;
    if (SNS_DDF_POWERSTATE_ACTIVE == state->power_state) return SNS_DDF_EDEVICE_BUSY;
    if ((test!=SNS_DDF_TEST_SELF_SW)&&(test!=SNS_DDF_TEST_SELF)) return SNS_DDF_EINVALID_TEST;

    if(test == SNS_DDF_TEST_SELF)
    {
        uint8_t i2c_buff = 0;
        uint8_t bytes_read;

        status = sns_ddf_read_port(
            state->port_handle,
            STM_LPS25H_WHO_AM_I,
            &i2c_buff,
            1,
            &bytes_read);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(bytes_read != 1)
            return SNS_DDF_EBUS;
        if(i2c_buff != STM_LPS25H_WHO_AM_I_VALUE)
            return SNS_DDF_EFAIL;

        return SNS_DDF_SUCCESS;

    }
    else // SNS_DDF_TEST_SELF_SW
    {

        state->self_test_enable = 1;

        // Start a timer for few ms here
        status = sns_ddf_timer_start(state->timer_obj, 10000);
        if(status != SNS_DDF_SUCCESS)
        {
            return status;
        }

        // Return pending
        return SNS_DDF_PENDING;
    }
}

/**
 * @brief Probe LPS25H.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_press_lps25h_probe(
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

  /* Read & Verify Device ID */
  status = sns_ddf_read_port( port_handle,
                              STM_LPS25H_WHO_AM_I,
                              &i2c_buff,
                              1,
                              &bytes_read );

  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  if( i2c_buff != STM_LPS25H_WHO_AM_I_VALUE )
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }

  /* Registers are correct. This is probably a LPS25H */
  *num_sensors = 2;
  *sensors = sns_ddf_memhandler_malloc( memhandler,
                 sizeof(sns_ddf_sensor_e) * *num_sensors );
  if( *sensors != NULL )
  {
    (*sensors)[0] = SNS_DDF_SENSOR_PRESSURE;
    (*sensors)[1] = SNS_DDF_SENSOR_TEMP;
    status = SNS_DDF_SUCCESS;
  } else {
    status = SNS_DDF_ENOMEM;
  }
  sns_ddf_close_port( port_handle );
  return status;
}


