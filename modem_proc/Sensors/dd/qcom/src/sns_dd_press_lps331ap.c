/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
*
* File Name         : sns_dd_press_lps331ap.c
* Authors           : Jianjian Huo
                      Sebastien MARSANNE
                      Jasdeep Hundal
* Version           : V 1.0.2
* Date              : 05/16/2012
* Description       : LPS331AP pressure sensor driver source file
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
* 1.0.2   | 05/16/2012    | Changed zT1sq,zT2sq,zT3sq to signed integer.
* 1.0.0   | 04/27/2012    | Cleaned unused functions.
* 0.9.8   | 04/23/2012    | Removed 64 bit integer math library.
* 0.9.6   | 04/19/2012    | Added quadratic temperature compensation.
* 0.9.2   | 03/29/2012    | Converted all floating point numbers to fixed point.
* 0.9.1   | 09/06/2011    | Added ATTRIB_RESOLUTION_ADC.
* 0.9.0   | 05/24/2011    | Created.
*******************************************************************************/

#include "sns_dd_press_lps331ap.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include <string.h>

#include "sns_debug_str.h"
#include "sns_debug_api.h"

#define STM_LPS331AP_NUM_MEAS 2
#define STM_LPS331AP_PRESS_MEAS 0
#define STM_LPS331AP_TEMP_MEAS 1
#define AUTO_INCREMENT 0x80
#define STM_LPS331AP_NUM_BYTES_PRESS 3
#define STM_LPS331AP_NUM_BYTES_TEMP 2

#define STM_LPS331AP_NUM_SENSORS 2

#define LPS331_DEBUG
#ifdef LPS331_DEBUG
#define LPS331_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "LPS331 - " msg)
#define LPS331_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "LPS331 - " msg,p1)
#define LPS331_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "LPS331 - " msg,p1,p2)
#define LPS331_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "LPS331 - " msg,p1,p2,p3)
#else
#define LPS331_MSG_0(level,msg)
#define LPS331_MSG_1(level,msg,p1)
#define LPS331_MSG_2(level,msg,p1,p2)
#define LPS331_MSG_3(level,msg,p1,p2,p3)
#endif

/**
 * Pressure sensor LPS331AP Driver State Information Structure
 */
typedef struct {
    /**< Handle used with sns_ddf_notify_data. */
    sns_ddf_handle_t smgr_hndl;

    /**< Handle used to access the I2C bus. */
    sns_ddf_handle_t port_handle;

    /**< Current sensor sampling frequency. */
    stm_lps331ap_odr data_rate;

    /**< Current power state: ACTIVE or LOWPOWER */
    sns_ddf_powerstate_e power_state;

    /**< Temperature compensation coefficients.  */
    struct stm_lps331ap_coeff comp_coeff;

    /**< flag for sensor data out of bound. */
    //uint8_t out_of_bound_flag;
} sns_dd_press_lps331ap_state_t;

/**
 * Pressure sensor LPS331AP sampling frequencies array for getting attribute
 * frequencies are rounded to integer values, such as using 13Hz to replace 12.5Hz.
 */
static sns_ddf_lowpass_freq_t lps331ap_freqs[STM_LPS331AP_ODR_NUM] = {
	// define FX_QFACTOR		   (16)
	// define FX_CONV(a,q1,q2)	   (((q2)>(q1))?(a)<<((q2)-(q1)):(a)>>((q1)-(q2)))
	// define FX_CONV_Q16(a,q1)    (FX_CONV(a,q1,FX_QFACTOR))
	// Avoiding "shift count is negative" warning by not using FX_CONV macro

    //FX_CONV_Q16((int32_t)1,0),
    (q16_t)(1 << 16),
    //FX_CONV_Q16((int32_t)7,0),
    (q16_t)(7 << 16),
    FX_FLTTOFIX_Q16(12.5),
    //FX_CONV_Q16((int32_t)25,0)};
    (q16_t)(25 << 16)};

/**
 * Pressure sensor LPS331AP sampling frequencies array for setting attribute
 */
static stm_lps331ap_odr lps331ap_odr_setting[STM_LPS331AP_ODR_NUM] = {
            STM_LPS331AP_ODR_1_1,
            STM_LPS331AP_ODR_7_1,
            STM_LPS331AP_ODR_12p5_1,
            STM_LPS331AP_ODR_25_1};

sns_ddf_driver_if_s sns_dd_press_lps331ap_if;

/**
 * Macro definitions for pressure compensation coefficients calculation
 */
#define T1_0  (int16_t)-15600 //480*(10-42.5)
#define T1_25 (int16_t)-8400  //480*(25-42.5)
#define T2_0  0               //480*(42.5-42.5)
#define T3_0  (int16_t)13200  //480*(70-42.5)
#define nf_P0 1000            // mbar
#define ROUND_UP    0x80

/**
 * Constant viarables for pressure compensation coefficients calculation
 */
static const int16_t nf_s4_to_300xs32[16] = {

    0*300,  // 0x00
    1*300,  // 0x01
    2*300,  // 0x02
    3*300,  // 0x03
    4*300,
    5*300,
    6*300,
    7*300,  // 0x07
    -8*300, // 0x08 [2400]
    -7*300, // 0x09
    -6*300, // 0x0A
    -5*300, // 0x0B
    -4*300, // 0x0C
    -3*300, // 0x0D
    -2*300, // 0x0E 1101
    -1*300  // 0x0F 1111
};

/**
 * @brief convert s32 into u32 and a s8 (+1 or -1) to use unsigned fractional divide and have more range
 *
 * @param[in]   s32           signed 32 bits number
 * @param[out]  ret_u32       unsigned 32 bits number
 * @param[out]  ret_sign      sign flag
 *
 * @return NULL;
 */
void s32_to_u32(int32_t s32, uint32_t* ret_u32, int8_t* ret_sign)
{
    if(s32<0)
    {
        *ret_u32 = -s32;
        *ret_sign = -1;
    }else
    {
        *ret_u32 = s32;
        *ret_sign = 1;
    }
}

/**
 * @brief 32 bit fractional division, very useful for complex or accurate calculations
 *
 * @param[in]    dwNum       numerator (32 bits)
 * @param[in]    dwDen       denominator (32 bits)
 * @param[out]  dwQuo       the result of the LDIV fractional division
 * @param[out]  dwRem       the remainder of the LDIV fractional division
 * @param[in]    bNbPostShiftLeft
 *
 * @return NULL;
 */
void LDIV (uint32_t dwNum, uint32_t dwDen,
             uint32_t* dwQuo, uint32_t* dwRem,
             uint8_t bNbPostShiftLeft)
{
    uint8_t RoundUp = bNbPostShiftLeft & ROUND_UP;
    bNbPostShiftLeft &= ~ROUND_UP;
    // result is 32 bit int and 32 bit frac, to keep the resolution, use the remainder
    *dwQuo = dwNum / dwDen;
    // dwRem = dwNum % dwDen;
    *dwRem = dwNum - (*dwQuo)*dwDen;

    while(bNbPostShiftLeft)
    {
        *dwQuo <<= 1;
        *dwRem <<=1;
        if(*dwRem >= dwDen)
        {
            *dwQuo |= 1;
            *dwRem = *dwRem - dwDen;
        }

        bNbPostShiftLeft--;
    };

    // Need to check roundup, should discard the remainer if quo is roundedup
    if(RoundUp) {
        if((*dwRem << 1) >= dwDen)
        {
            (*dwQuo)++;
        }
    }
}

/**
 * @brief 32 bit signed fractional division
 *
 * @param[in]    Num_s32     signed 32 bits numerator
 * @param[in]    Den_s32      signed 32 bits denominator
 * @param[in]    shiftleft       shiftleft
 *
 * @return int32_t;
 */
int32_t SDIV(int32_t Num_s32, int32_t Den_s32, uint8_t shiftleft)
{
    int32_t result;
    uint32_t Num_u32, Den_u32;
    int8_t Num_sign, Den_sign;
    uint32_t dwQuo, dwRem;

    s32_to_u32(Num_s32, &Num_u32, &Num_sign);
    s32_to_u32(Den_s32, &Den_u32, &Den_sign);

    LDIV(Num_u32, Den_u32, &dwQuo, &dwRem, shiftleft | ROUND_UP);
    result = ((int32_t)dwQuo)*(Num_sign*Den_sign);

    return result;
}

/**
 * @brief Calculate pressure compensation coefficients based on factory calibration.
 *
 * @param[in]  comp_pkg      data structure holding factory calibration data
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return NULL;
 */
static void lps331ap_calc_coeff(
    struct stm_lps331ap_comp_pkg *comp_pkg,
    sns_ddf_handle_t dd_handle)
{
    int32_t Num_s32, Den_s32;
    int32_t nf_P1,nf_P2, Den_saved;
    struct stm_lps331ap_coeff *coeff;

    coeff = &(((sns_dd_press_lps331ap_state_t *)dd_handle)->comp_coeff);

    //algorithm step 1
    Num_s32 = 15*(32*(int32_t)comp_pkg->nf_TCV2+15*(int32_t)comp_pkg->nf_TCS2)
              *(comp_pkg->zT2_T3);
    Den_s32 = comp_pkg->nf_DGain + (uint32_t)32768;
    nf_P2 = SDIV(Num_s32, Den_s32, 8);

    Num_s32 = 15*(32*(int32_t)comp_pkg->nf_TCV1+15*(int32_t)comp_pkg->nf_TCS1)
              *(comp_pkg->zT1_T2);
    nf_P1 = nf_P2 + SDIV(Num_s32, Den_s32, 8);

    //algorithm step 2
    Den_saved = Den_s32 = comp_pkg->zT1*comp_pkg->zT1*(comp_pkg->zT2_T3)
                          - comp_pkg->zT2*comp_pkg->zT2*(comp_pkg->zT1_T3)
                          + comp_pkg->zT3*comp_pkg->zT3*(comp_pkg->zT1_T2);

    Num_s32 = (nf_P1*comp_pkg->zT2_T3)-(nf_P2*comp_pkg->zT1_T3);
    Den_s32 = 225;

    Num_s32 = SDIV(Num_s32, Den_s32, 8);
    Den_s32 = Den_saved;
    coeff->coeff_a = SDIV(Num_s32, Den_s32, 20);

    Num_s32 = -(nf_P1/256)*(comp_pkg->zT2sq-comp_pkg->zT3sq)
              +(nf_P2/256)*(comp_pkg->zT1sq-comp_pkg->zT3sq);
    Num_s32 /= 15;

    coeff->coeff_b = SDIV(Num_s32, Den_s32, 14);

    Num_s32 = (nf_P1/256)*((comp_pkg->zT3*comp_pkg->zT2sq-comp_pkg->zT3sq*comp_pkg->zT2)/64)
              -(nf_P2/256)*((comp_pkg->zT3*comp_pkg->zT1sq-comp_pkg->zT3sq*comp_pkg->zT1)/64);

    coeff->coeff_c = SDIV(Num_s32, Den_s32, 8+6);

    //algorithm step 3
    // denominator first
    Den_s32 = comp_pkg->zT2_T3;
    // calculate e2
    Num_s32 = nf_P2;
    coeff->coeff_e2 = SDIV(Num_s32, Den_s32, 8);

    // calculate d2
    coeff->coeff_d = - (coeff->coeff_e2 * comp_pkg->zT3)/256;

    // calculate e1
    Num_s32 = (nf_P1 - coeff->coeff_d);
    Den_s32 = comp_pkg->zT1*60;
    coeff->coeff_e1 = SDIV(Num_s32, Den_s32, 8);
}


/**
 * @brief Initializes the compensation algorithm, needs to be called once during driver initialization.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return sns_ddf_status_e.
 */
static sns_ddf_status_e lps331ap_init_comp(sns_ddf_handle_t dd_handle)
{
    struct stm_lps331ap_comp_pkg comp_pkg;
    sns_dd_press_lps331ap_state_t* state = dd_handle;
    uint8_t read_count = 0;
    sns_ddf_status_e status;

    uint8_t compens_data[11];
    uint32_t reg;
    int16_t T1d,T2d,T3d;
    int32_t nf_T1,nf_T2,nf_T3;

    status = sns_ddf_read_port(
        state->port_handle,
        AUTO_INCREMENT | 0x30,
        compens_data,
        11,
        &read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(read_count != 11)
        return SNS_DDF_EBUS;

    reg =  compens_data[1] >> (8-2);
    reg |= ((uint16_t)compens_data[2]) << 2;
    reg |= ((uint16_t)compens_data[3] & 0x03) << 10;
    comp_pkg.nf_TCV1 = reg;

    reg =  compens_data[3] >> (8-6);
    reg |= ((uint16_t)compens_data[4] & 0x3F) << 6;
    comp_pkg.nf_TCV2 = reg;

    reg =  compens_data[6] >> (8-6);
    reg |= ((uint16_t)compens_data[7] & 0x0F) << 6;
    comp_pkg.nf_TCS1 = reg;

    reg =  compens_data[7] >> (8-4);
    reg |= ((uint16_t)compens_data[8] & 0x3F) << 4;
    comp_pkg.nf_TCS2 = reg;

    status = sns_ddf_read_port(
        state->port_handle,
        AUTO_INCREMENT | 0x18,
        compens_data,
        3,
        &read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(read_count != 3)
        return SNS_DDF_EBUS;

    reg = compens_data[0]>>(8-6);
    reg |= ((uint16_t)compens_data[1]) << 6;
    reg |= ((uint32_t)(compens_data[2]&0x0F))<< 14;
    comp_pkg.nf_DGain = reg;

    status = sns_ddf_read_port(
        state->port_handle,
        0x3B,
        compens_data,
        1,
        &read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(read_count != 1)
        return SNS_DDF_EBUS;

    status = sns_ddf_read_port(
        state->port_handle,
        0x3F,
        compens_data+1,
        1,
        &read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(read_count != 1)
        return SNS_DDF_EBUS;

    status = sns_ddf_read_port(
        state->port_handle,
        0x1E,
        compens_data+2,
        1,
        &read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(read_count != 1)
        return SNS_DDF_EBUS;

    T1d = nf_s4_to_300xs32[(compens_data[0]&0xF0)>>4];
    T2d = nf_s4_to_300xs32[(compens_data[1]&0xF0)>>4];
    T3d = nf_s4_to_300xs32[compens_data[1]&0x0F];

    nf_T1 = T1d + T1_0;
    if(compens_data[2] & 0x80)
        nf_T1 = T1d + T1_25;
    nf_T2 = T2d + T2_0;
    nf_T3 = T3d + T3_0;

    comp_pkg.zT1 = nf_T1/60;
    comp_pkg.zT2 = nf_T2/60;
    comp_pkg.zT3 = nf_T3/60;
    comp_pkg.zT2_T3 = comp_pkg.zT2-comp_pkg.zT3;
    comp_pkg.zT1_T3 = comp_pkg.zT1-comp_pkg.zT3;
    comp_pkg.zT1_T2 = comp_pkg.zT1-comp_pkg.zT2;
    comp_pkg.zT1sq=comp_pkg.zT1*comp_pkg.zT1;
    comp_pkg.zT2sq=comp_pkg.zT2*comp_pkg.zT2;
    comp_pkg.zT3sq=comp_pkg.zT3*comp_pkg.zT3;

    lps331ap_calc_coeff(&comp_pkg, dd_handle);

    return SNS_DDF_SUCCESS;
}

/**
 * @brief Convert raw pressure reading to compensated pressure data.
 *
 * @param[in]  temperature      temperature output as from device (raw value, SENSITIVITY_T not applied yet
 *                                        and before to apply TEMPERATURE_OFFSET);
 * @param[in]  devicePressureOutput    pressure output as from device (raw LSB value,SENSITIVITY_P not applied yet);
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return pComp pressure corrected by Quadratic Algorithm, SENSITYVITY_P needs to be applied
 *         to get it expressed as mbar.
 */
static uint32_t lps331ap_calc_comp_press(
    int32_t temperature,
    uint32_t devicePressureOutput,
    sns_ddf_handle_t dd_handle)
{
    int32_t d, Num_s32;
    uint32_t Num_u32;
    int8_t Num_sign;
    int32_t Quadratic_delta = 0;
    struct stm_lps331ap_coeff *coeff;
    unsigned long long big_temp;

    coeff = &(((sns_dd_press_lps331ap_state_t *)dd_handle)->comp_coeff);

    d = temperature*temperature;
    Num_s32 = coeff->coeff_a;
    s32_to_u32(Num_s32, &Num_u32, &Num_sign);
    big_temp = (unsigned long long)d * (unsigned long long)Num_u32;
    d = big_temp>>32;
    d = d * Num_sign;
    Quadratic_delta += d;

    if(temperature<0)
    {
        d = (temperature*(coeff->coeff_b-coeff->coeff_e1))>>8;
    }
    else
    {
        d = (temperature*(coeff->coeff_b-coeff->coeff_e2/60))>>8;
    }
    Quadratic_delta += d;

    d = coeff->coeff_c - coeff->coeff_d;
    Quadratic_delta += d;

    return ((int32_t)devicePressureOutput + (int32_t)Quadratic_delta);
}

/**
  * @brief Resets the driver and device so they return to the state they were
  *        in after init() was called.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_press_lps331ap_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_press_lps331ap_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer[1] = { 0 };
    uint8_t bytes_written = 0;

    // Reset driver state.
    //state->out_of_bound_flag = 0;
    //memset(state->data_cache, 0, STM_LPS331AP_NUM_MEAS * sizeof(q16_t));

    // power down device in order to set control registers.
    write_buffer[0] = 0x0;
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LPS331AP_CTRL_REG1,
        write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    // wait sensor to power down by delaying 1 ms.
    sns_ddf_delay(1000);

    // Configure pressure sensor resolution (register RES_CONF(10h))
    write_buffer[0] = 0x0
                   | (0<<7)        // RFU
                   | (0x7<<4)      // AVGT2:0 (Bits [6:4]) - temperature resolution
                   | (0x9<<0);     // AVGP3:0 (bits [3:0]) - pressure resolution

    status = sns_ddf_write_port(
        state->port_handle,
        STM_LPS331AP_RES_CONF,
        write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    // Configure power mode/data rate (register CTRL_REG1(20h))
    write_buffer[0] = 0x0
                   | (0<<7)                        // PD (power down control, 0 for power-down, 1 for active)
                   | (STM_LPS331AP_ODR_25_25<<4)   // ODR2:0 (Bits [6:4]) - data rate
                   | (0<<3)                        // DIFF_EN (interrupt enable)
                   | (1<<2)                        // BDU (block data update)
                   | (0<<1)                        // DELTA_EN
                   | (0);                          // SIM (SPI Serial Interface Mode selection)
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LPS331AP_CTRL_REG1,
        write_buffer,
        1,
        &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(bytes_written != 1)
        return SNS_DDF_EBUS;

    state->data_rate = STM_LPS331AP_ODR_25_25;
    state->power_state = SNS_DDF_POWERSTATE_LOWPOWER;

    return SNS_DDF_SUCCESS;
}

/**
  * @brief Probes for an LPS331AP part.
  *
  * Refer to sns_ddf_driver_if.h for definition.
  */
static sns_ddf_status_e sns_dd_press_lps331ap_probe(
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
        STM_LPS331AP_WHO_AM_I,
        &i2c_buff,
        1,
        &bytes_read);
    if(status != SNS_DDF_SUCCESS || bytes_read != 1)
    {
        sns_ddf_close_port( port_handle );
        return status;
    }
    if(i2c_buff != 0xBB)
    {
        /* Incorrect value. Return now with nothing detected */
        sns_ddf_close_port( port_handle );
        return SNS_DDF_SUCCESS;
    }

    // WHO_AM_I register is correct. This is probably an LPS331AP
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

/**
 * @brief Performs a set data sampling rate operation
 *        -- Must set power to low before doing this
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  data_rate       Data sampling rate settings for LPS331AP
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_press_lps331ap_set_datarate(
  sns_ddf_handle_t dd_handle,
  stm_lps331ap_odr data_rate)
{
    sns_dd_press_lps331ap_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t write_buffer[1] = {0};
    uint8_t bytes_written = 0;

    if(state->data_rate != data_rate)
    {
        // turn the device off if current power state is ACTIVE
        if (state->power_state == SNS_DDF_POWERSTATE_ACTIVE)
        {
            write_buffer[0] = 0x0
                       | (0<<7);                      // PD (0 for power-down)

            status = sns_ddf_write_port(
                state->port_handle,
                STM_LPS331AP_CTRL_REG1,
                write_buffer,
                1,
                &bytes_written);
            if(status != SNS_DDF_SUCCESS)
                return status;
            if(bytes_written != 1)
                return SNS_DDF_EBUS;

            // wait sensor to power down by delaying 1 ms.
            sns_ddf_delay(1000);
        }

        // Configure new data rate (register CTRL_REG1(20h))
        write_buffer[0] = 0x0
                       | (0<<7)              // PD (power down control, 0 for power-down, 1 for active)
                       | (data_rate<<4)     // ODR2:0 (Bits [6:4]) - data rate
                       | (0<<3)             // DIFF_EN (interrupt enable)
                       | (1<<2)             // BDU (block data update)
                       | (0<<1)             // DELTA_EN
                       | (0);               // SIM (SPI Serial Interface Mode selection)

        status = sns_ddf_write_port(
            state->port_handle,
            STM_LPS331AP_CTRL_REG1,
            write_buffer,
            1,
            &bytes_written);
        if(status != SNS_DDF_SUCCESS)
            return status;
        if(bytes_written != 1)
            return SNS_DDF_EBUS;

        // Power the device back up if that was the state in which it entered this function
        if (state->power_state == SNS_DDF_POWERSTATE_ACTIVE)
        {
            write_buffer[0] = 0x0
                       | (1<<7)          // PD (power down control, 0 for power-down, 1 for active)
                       | (data_rate<<4)  // ODR2:0 (Bits [6:4]) - data rate
                       | (0<<3)          // DIFF_EN (interrupt enable)
                       | (1<<2)          // BDU (block data update)
                       | (0<<1)          // DELTA_EN
                       | (0);            // SIM (SPI Serial Interface Mode selection)


            status = sns_ddf_write_port(
                state->port_handle,
                STM_LPS331AP_CTRL_REG1,
                write_buffer,
                1,
                &bytes_written);
            if(status != SNS_DDF_SUCCESS)
                return status;
            if(bytes_written != 1)
                return SNS_DDF_EBUS;

            sns_ddf_delay(5000);
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
static sns_ddf_status_e sns_dd_press_lps331ap_set_power(
  sns_ddf_handle_t dd_handle,
  sns_ddf_powerstate_e power_state)
{
    sns_dd_press_lps331ap_state_t* state = dd_handle;
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
            STM_LPS331AP_CTRL_REG1,
            write_buffer,
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
        // Configure CTRL_REG1 to active mode.
        write_buffer[0] = 0x0
                       | (1<<7)                      // PD (1 for active)
                       | ((state->data_rate)<<4)     // ODR2:0 (Bits [6:4]) - data rate
                       | (0<<3)                      // DIFF_EN (interrupt enable)
                       | (1<<2)                      // BDU (block data update)
                       | (0<<1)                      // DELTA_EN
                       | (0);                        // SIM (SPI Serial Interface Mode selection)

        status = sns_ddf_write_port(
            state->port_handle,
            STM_LPS331AP_CTRL_REG1,
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
 * @brief Initializes the lps331ap driver and sets up sensor.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_press_lps331ap_init(
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
    sns_dd_press_lps331ap_state_t* state;
    static sns_ddf_sensor_e my_sensors[] = {
        SNS_DDF_SENSOR_PRESSURE, SNS_DDF_SENSOR_TEMP};
    uint8_t read_buffer[1] = {0};
    uint8_t read_count = 0;

    // Allocate a driver instance.
    status = sns_ddf_malloc((void**)&state, sizeof(sns_dd_press_lps331ap_state_t));
    if(status != SNS_DDF_SUCCESS)
        return SNS_DDF_ENOMEM;

    memset(state, sizeof(sns_dd_press_lps331ap_state_t), 0x0);
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

    // Check WhoAmIRegister to make sure this is the correct driver
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LPS331AP_WHO_AM_I,
        read_buffer,
        1,
        &read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(read_count != 1)
        return SNS_DDF_EBUS;
    if(read_buffer[0] != 0xBB)
        return SNS_DDF_EFAIL;

    // Resets the LPS331AP
    status = sns_dd_press_lps331ap_reset(state);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state);
        return status;
    }

    // Initialize the compensation algorithm
    status = lps331ap_init_comp(state);
    if(status != SNS_DDF_SUCCESS)
    {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state);
        return status;
    }

    // Fill out output parameters.
    *num_sensors = STM_LPS331AP_NUM_SENSORS;
    *sensors = my_sensors;
    *dd_handle_ptr = state;

    return SNS_DDF_SUCCESS;
}

/**
 * @brief Retrieves a single set of pressure and/or temperature data from lps331ap
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_press_lps331ap_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data)
{
    uint8_t i;
    uint8_t read_buffer[5];
    uint8_t read_count = 0;
    q16_t sample;
    uint32_t pressure;
    int16_t temperature;
    sns_ddf_time_t timestamp;
    sns_ddf_status_e status;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_press_lps331ap_state_t *state = dd_handle;

    //if current power mode is LOWPOWER , return error.
    if((SNS_DDF_POWERSTATE_LOWPOWER == state->power_state)||(num_sensors > 2))
        return SNS_DDF_EDEVICE;

    //allocate memory for sns_ddf_sensor_data_s data structure
    data_ptr = sns_ddf_memhandler_malloc(memhandler, num_sensors * sizeof(sns_ddf_sensor_data_s));
    if(NULL == data_ptr)
        return SNS_DDF_ENOMEM;

    *data = data_ptr;

    for(i = 0; i < num_sensors; i++)
    {
        // get current time stamp
        timestamp = sns_ddf_get_timestamp();

        // This is a synchronous driver, so try to read data now.
        switch(sensors[i])
        {
        case SNS_DDF_SENSOR_PRESSURE:
            status = sns_ddf_read_port(
                state->port_handle,
                AUTO_INCREMENT | STM_LPS331AP_PRESS_OUT_XL,
                read_buffer,
                5,
                &read_count);
            // check if the read was a success
            if(status != SNS_DDF_SUCCESS)
                return status;
            if(read_count != 5)
                return SNS_DDF_EBUS;
            break;

        case SNS_DDF_SENSOR_TEMP:
            status = sns_ddf_read_port(
                state->port_handle,
                AUTO_INCREMENT | STM_LPS331AP_TEMP_OUT_L,
                read_buffer,
                STM_LPS331AP_NUM_BYTES_TEMP,
                &read_count);
            // check if the read was a success
            if(status != SNS_DDF_SUCCESS)
                return status;
            if(read_count != STM_LPS331AP_NUM_BYTES_TEMP)
                return SNS_DDF_EBUS;
            break;
        default:
            return SNS_DDF_EINVALID_PARAM;
        }

        data_ptr[i].sensor = sensors[i];
        data_ptr[i].status = SNS_DDF_SUCCESS;
        data_ptr[i].timestamp = timestamp;

        //allocate memory for data samples.
        data_ptr[i].samples = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_sample_s));
        if(NULL == data_ptr[i].samples)
            return SNS_DDF_ENOMEM;

        if(sensors[i] == SNS_DDF_SENSOR_PRESSURE)
        {
            temperature = (int16_t) ((uint16_t)(read_buffer[4] << 8) | (uint16_t)(read_buffer[3]));
            pressure = (uint32_t)((uint32_t)read_buffer[0]
                                   | (uint32_t)((read_buffer[1])<<8)
                                   | (uint32_t)((read_buffer[2])<<16));

            // Apply temperature compensation, get quadratic pressure
            pressure = lps331ap_calc_comp_press(temperature, pressure, dd_handle);

            //convert pressure LSB data to Q16 and milli-bar
            sample = pressure <<4;
        }else{
            temperature = (int32_t) ((read_buffer[1] << 8) | (read_buffer[0]));
            sample = FX_FLTTOFIX_Q16(42.5) + FX_DIV_Q16(FX_CONV_Q16(temperature,0), FX_CONV_Q16((int32_t)480,0));
        }

        data_ptr[i].samples->sample = sample;
        data_ptr[i].samples->status = SNS_DDF_SUCCESS;
        data_ptr[i].num_samples = 1;
    }

    return SNS_DDF_SUCCESS;
}

/**
 * @brief Retrieves the value of an attribute for a lps331ap sensor.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_press_lps331ap_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems)
{
    //sns_dd_press_lps331ap_state_t *state = dd_handle;

    switch(attrib)
    {
        case SNS_DDF_ATTRIB_POWER_INFO:
        {
            sns_ddf_power_info_s *power;
            power = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s));
            if(NULL == power)
                return SNS_DDF_ENOMEM;

            //current consumption, unit uA
            power->active_current = 30;
            power->lowpower_current = 1;

            *value = power;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *range;
            range = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_range_s));
            if(NULL == range)
                return SNS_DDF_ENOMEM;

            if(sensor == SNS_DDF_SENSOR_PRESSURE)
            {
                range->min = STM_LPS331AP_PRESS_RANGE_MIN;
                range->max = STM_LPS331AP_PRESS_RANGE_MAX;
            }
            else
            {
                range->min = STM_LPS331AP_TEMP_RANGE_MIN;
                range->max = STM_LPS331AP_TEMP_RANGE_MAX;
            }

            *num_elems = 1;
            *value = range;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t *res;
            res = sns_ddf_memhandler_malloc(memhandler ,sizeof(sns_ddf_resolution_t));
            if(NULL == res)
                return SNS_DDF_ENOMEM;

            if(sensor == SNS_DDF_SENSOR_TEMP)
                *res = STM_LPS331AP_MAX_RES_TEMP;
            else
                *res = STM_LPS331AP_MAX_RES_PRESS;

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

            res->bit_len = (sensor == SNS_DDF_SENSOR_PRESSURE) ? 24 : 16;
            // TODO: Support temperature up to 25 Hz
            res->max_freq = (sensor == SNS_DDF_SENSOR_PRESSURE) ? 25 : 1;
            *value = res;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;

        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            *value = lps331ap_freqs;
            *num_elems = STM_LPS331AP_ODR_NUM;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s *lps331ap_delays;
            lps331ap_delays = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_delays_s));
            if(NULL == lps331ap_delays)
                return SNS_DDF_ENOMEM;

            lps331ap_delays->time_to_active = 5;
            lps331ap_delays->time_to_data = 1;
            *value = lps331ap_delays;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s *info;
            info = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_driver_info_s));
            if(NULL == info)
                return SNS_DDF_ENOMEM;

            info->name = "STM LPS331AP Pressure Sensor Driver";
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

            info->name = "Pressure sensor";
            info->vendor = "STMicroelectronics";
            info->model = "LPS331AP";
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
 * @brief Sets a lps331ap sensor attribute to a specific value.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_press_lps331ap_set_attr(
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

            return sns_dd_press_lps331ap_set_power(dd_handle, *power_state);
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            uint32_t* freqs_idx = value;

            if(*freqs_idx >= ARR_SIZE(lps331ap_odr_setting))
            {
              LPS331_MSG_2(ERROR, "sensor = %d, freqs = %d", sensor, *freqs_idx);
              return SNS_DDF_EINVALID_PARAM;
            }

            stm_lps331ap_odr freqs = lps331ap_odr_setting[*freqs_idx];
            if(freqs > STM_LPS331AP_MAX_ODR)
            {
                LPS331_MSG_3(ERROR, "sensor = %d, freqs = %d, *freqs_idx = %d",
                             sensor, freqs, *freqs_idx);
                return SNS_DDF_EINVALID_PARAM;
            }

            return sns_dd_press_lps331ap_set_datarate(dd_handle, freqs);
        }

        default:
            return SNS_DDF_EINVALID_ATTR;
    }
}

/**
 * LPS331AP pressure sensor device driver interface.
 */
sns_ddf_driver_if_s sns_dd_press_lps331ap_if =
{
    .init = &sns_dd_press_lps331ap_init,
    .get_data = &sns_dd_press_lps331ap_get_data,
    .set_attrib = &sns_dd_press_lps331ap_set_attr,
    .get_attrib = &sns_dd_press_lps331ap_get_attr,
    .handle_timer = NULL, // handle_timer
    .handle_irq = NULL, // handle_irq
    .reset = &sns_dd_press_lps331ap_reset,
    .run_test = NULL, // run_test
    .enable_sched_data = NULL, // enable_sched_data
    .probe = &sns_dd_press_lps331ap_probe,
};

