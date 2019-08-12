/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
*
* File Name         : sns_dd_press_lps331ap.h
* Authors           : Jianjian Huo
                      Sebastien MARSANNE
                      Jasdeep Hundal 
* Version           : V 1.0.2
* Date              : 05/16/2012
* Description       : LPS331AP pressure sensor driver header file
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
*******************************************************************************/

#ifndef __LPS331AP__
#define __LPS331AP__

#include "fixed_point.h"

#define STM_LPS331AP_PRESS_RANGE_MIN    FX_CONV_Q16(260,0)
#define STM_LPS331AP_PRESS_RANGE_MAX    FX_CONV_Q16(1260,0)
#define STM_LPS331AP_TEMP_RANGE_MIN     FX_CONV_Q16(0,0)
#define STM_LPS331AP_TEMP_RANGE_MAX     FX_CONV_Q16(80,0)

#define STM_LPS331AP_MAX_RES_PRESS      FX_FLTTOFIX_Q16(0.02)
#define STM_LPS331AP_MAX_RES_TEMP       FX_FLTTOFIX_Q16(0.002)

/**
 * Pressure sensor LPS331AP output data rate
 * * This is bits [ODR2:ODR0] ([6:4]) in register CTRL_REG1(20h)
 * * The values at the end of name indicate pressure output rate first, temperature output rate second
 */
typedef enum 
{
    //STM_LPS331AP_ODR_0_0 =                0x00,    /*Pressure: one shot, Temperature: one shot*/
    STM_LPS331AP_ODR_1_1 =             0x01,  /*Pressure: 1Hz, Temperature: 1Hz*/
    STM_LPS331AP_ODR_7_1 =             0x02,  /*Pressure: 7Hz, Temperature: 1Hz*/
    STM_LPS331AP_ODR_12p5_1 =          0x03,  /*Pressure: 12.5Hz, Temperature: 1Hz*/
    STM_LPS331AP_ODR_25_1 =            0x04,  /*Pressure: 25Hz, Temperature: 1Hz*/
    STM_LPS331AP_ODR_7_7 =             0x05,  /*Pressure: 7Hz, Temperature: 7Hz*/
    STM_LPS331AP_ODR_12p5_12p5 =       0x06,  /*Pressure: 12.5Hz, Temperature: 12.5Hz*/
    STM_LPS331AP_ODR_25_25 =           0x07,  /*Pressure: 25Hz, Temperature: 25Hz*/
} stm_lps331ap_odr;

#define STM_LPS331AP_ODR_NUM      4
#define STM_LPS331AP_MAX_ODR      STM_LPS331AP_ODR_25_1

/**

 * Pressure sensor LPS331AP interrupt
 * * This is bits [INT1_S2:INT1_S0] ([2:0]) in register CTRL_REG3(22h)
 * * Used for programming what triggers interrups
 * * Shift to the left 3 bits to use for second interrupt
 */
typedef enum 
{
    STM_LPS331AP_INT_GND =             0x00,  /*No trigger, output is always ground*/
    STM_LPS331AP_INT_P_H =             0x01,  /*Pressure high*/
    STM_LPS331AP_INT_P_L =             0x02,  /*Pressure low*/
    STM_LPS331AP_INT_P_HL =            0x03,  /*Pressure high or low*/
    STM_LPS331AP_INT_DRDY =            0x04,  /*Data ready*/
} stm_lps331ap_int;
/**
 * Pressure sensor LPS331AP compensation coefficients
 */
struct stm_lps331ap_coeff 
{
    int32_t coeff_a;
    int32_t coeff_b;
    int32_t coeff_c;
    int32_t coeff_d;
    int32_t coeff_e1;
    int32_t coeff_e2;
};

/**
 * Pressure sensor LPS331AP compensation data package
 */
struct stm_lps331ap_comp_pkg
{
    uint16_t nf_TCV1;
    uint16_t nf_TCV2;
    uint16_t nf_TCS1;
    uint16_t nf_TCS2;
    uint16_t nf_DGain;
    int32_t  zT1;
    int32_t  zT2;
    int32_t  zT3;
    int32_t  zT2_T3;
    int32_t  zT1_T3;
    int32_t  zT1_T2;
    int32_t  zT1sq;
    int32_t  zT2sq;
    int32_t  zT3sq;
};

/**
 * Pressure sensor LPS331AP I2C address
 */
#define STM_LPS331AP_I2C_ADDR         0x5D

/**
 * Pressure sensor LPS331AP register addresses
 */
#define STM_LPS331AP_REF_P_XL             0x08
#define STM_LPS331AP_REF_P_L              0x09
#define STM_LPS331AP_REF_P_H              0x0A
#define STM_LPS331AP_WHO_AM_I             0x0F
#define STM_LPS331AP_RES_CONF             0x10
#define STM_LPS331AP_CTRL_REG1            0x20
#define STM_LPS331AP_CTRL_REG2            0x21
#define STM_LPS331AP_CTRL_REG3            0x22
#define STM_LPS331AP_INT_CFG_REG          0x23
#define STM_LPS331AP_INT_SOURCE_REG       0x24
#define STM_LPS331AP_THS_P_LOW_REG        0x25
#define STM_LPS331AP_THS_P_HIGH_REG       0x26
#define STM_LPS331AP_STATUS_REG           0x27
#define STM_LPS331AP_PRESS_OUT_XL         0x28
#define STM_LPS331AP_PRESS_OUT_L          0x29
#define STM_LPS331AP_PRESS_OUT_H          0x2A
#define STM_LPS331AP_TEMP_OUT_L           0x2B
#define STM_LPS331AP_TEMP_OUT_H           0x2C
#define STM_LPS331AP_AMP_CTRL             0x2C
#define STM_LPS331AP_DELTA_PRESS_XL       0x3C
#define STM_LPS331AP_DELTA_PRESS_L        0x3D
#define STM_LPS331AP_DELTA_PRESS_H        0x3E


#endif  /* __LPS331AP__ */
