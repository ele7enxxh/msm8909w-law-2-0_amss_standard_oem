/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
*
* File Name         : lsm303dlhc_ddf_drv.h
* Authors           : Jianjian Huo
* Version           : V 1.0.0
* Date              : 12/03/2012
* Description       : LSM303DLHC magnetometer driver header file
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

#ifndef __LSM303DLHC__
#define __LSM303DLHC__

#include "fixed_point.h"

/**
 * Magnetometer LSM303DLHC Full Scales in register setting.
 */
typedef enum 
{
    STM_LSM303DLHC_MAG_RANGE_1_3G   = 0x20,  /*corresponding value in register setting*/
    STM_LSM303DLHC_MAG_RANGE_1_9G   = 0x40,
    STM_LSM303DLHC_MAG_RANGE_2_5G   = 0x60,  
    STM_LSM303DLHC_MAG_RANGE_4_0G   = 0x80,
    STM_LSM303DLHC_MAG_RANGE_4_7G   = 0xA0,
    STM_LSM303DLHC_MAG_RANGE_5_6G   = 0xC0,
    STM_LSM303DLHC_MAG_RANGE_8_1G   = 0xE0
} stm_lsm303dlhc_mag_range;

#define STM_LSM303DLHC_MAG_RANGE_NUM 7
#define STM_LSM303DLHC_MAG_MAX_RANGE STM_LSM303DLHC_MAG_RANGE_8_1G

/**
 * Magnetometer LSM303DLHC min and max of ranges, converted to Q16.
 */
#define STM_LSM303DLHC_MAG_RANGE_1_3G_MIN    FX_FLTTOFIX_Q16(-1.3)
#define STM_LSM303DLHC_MAG_RANGE_1_3G_MAX    FX_FLTTOFIX_Q16(1.3)
#define STM_LSM303DLHC_MAG_RANGE_1_9G_MIN    FX_FLTTOFIX_Q16(-1.9)
#define STM_LSM303DLHC_MAG_RANGE_1_9G_MAX    FX_FLTTOFIX_Q16(1.9)
#define STM_LSM303DLHC_MAG_RANGE_2_5G_MIN    FX_FLTTOFIX_Q16(-2.5)
#define STM_LSM303DLHC_MAG_RANGE_2_5G_MAX    FX_FLTTOFIX_Q16(2.5)
#define STM_LSM303DLHC_MAG_RANGE_4_0G_MIN    FX_FLTTOFIX_Q16(-4.0)
#define STM_LSM303DLHC_MAG_RANGE_4_0G_MAX    FX_FLTTOFIX_Q16(4.0)
#define STM_LSM303DLHC_MAG_RANGE_4_7G_MIN    FX_FLTTOFIX_Q16(-4.7)
#define STM_LSM303DLHC_MAG_RANGE_4_7G_MAX    FX_FLTTOFIX_Q16(4.7)
#define STM_LSM303DLHC_MAG_RANGE_5_6G_MIN    FX_FLTTOFIX_Q16(-5.6)
#define STM_LSM303DLHC_MAG_RANGE_5_6G_MAX    FX_FLTTOFIX_Q16(5.6)
#define STM_LSM303DLHC_MAG_RANGE_8_1G_MIN    FX_FLTTOFIX_Q16(-8.1)
#define STM_LSM303DLHC_MAG_RANGE_8_1G_MAX    FX_FLTTOFIX_Q16(8.1)

/**
 * Magnetometer LSM303DLHC X-Y sensitivity
 */
typedef enum
{
    STM_LSM303DLHC_XY_SENSITIVITY_1_3   = 1100,  /* XY sensitivity at 1.3G */
    STM_LSM303DLHC_XY_SENSITIVITY_1_9   = 855,  /* XY sensitivity at 1.9G */
    STM_LSM303DLHC_XY_SENSITIVITY_2_5   = 670,  /* XY sensitivity at 2.5G */
    STM_LSM303DLHC_XY_SENSITIVITY_4_0   = 450,  /* XY sensitivity at 4.0G */
    STM_LSM303DLHC_XY_SENSITIVITY_4_7   = 400,  /* XY sensitivity at 4.7G */
    STM_LSM303DLHC_XY_SENSITIVITY_5_6   = 330,  /* XY sensitivity at 5.6G */
    STM_LSM303DLHC_XY_SENSITIVITY_8_1   = 230   /* XY sensitivity at 8.1G */
}stm_lsm303dlhc_xy_sstvt;
/**
 * Magnetometer LSM303DLHC Z sensitivity
 */
typedef enum
{ 
    STM_LSM303DLHC_Z_SENSITIVITY_1_3    = 980,  /* Z sensitivity at 1.3G */
    STM_LSM303DLHC_Z_SENSITIVITY_1_9    = 760,  /* Z sensitivity at 1.9G */
    STM_LSM303DLHC_Z_SENSITIVITY_2_5    = 600,  /* Z sensitivity at 2.5G */
    STM_LSM303DLHC_Z_SENSITIVITY_4_0    = 400,  /* Z sensitivity at 4.0G */
    STM_LSM303DLHC_Z_SENSITIVITY_4_7    = 355,  /* Z sensitivity at 4.7G */
    STM_LSM303DLHC_Z_SENSITIVITY_5_6    = 295,  /* Z sensitivity at 5.6G */
    STM_LSM303DLHC_Z_SENSITIVITY_8_1    = 205   /* Z sensitivity at 8.1G */
}stm_lsm303dlhc_z_sstvt;

/**
 * Magnetometer LSM303DLHC sensitivity data stucture.
 */
typedef struct
{
    stm_lsm303dlhc_xy_sstvt  xy;
    stm_lsm303dlhc_z_sstvt   z;
}stm_lsm303dlhc_mag_sstvt;

/**
 * Magnetometer LSM303DLHC output data rate
 */
typedef enum 
{
    STM_LSM303DLHC_MAG_ODR_75  =        0x00,       /* 0.75Hz output data rate */
    STM_LSM303DLHC_MAG_ODR1_5  =        0x04,       /* 1.5Hz output data rate */
    STM_LSM303DLHC_MAG_ODR3_0  =        0x08,       /* 3Hz output data rate */
    STM_LSM303DLHC_MAG_ODR7_5  =        0x0C,       /* 7.5Hz output data rate */
    STM_LSM303DLHC_MAG_ODR15   =        0x10,       /* 15Hz output data rate */
    STM_LSM303DLHC_MAG_ODR30   =        0x14,       /* 30Hz output data rate */
    STM_LSM303DLHC_MAG_ODR75   =        0x18,       /* 75Hz output data rate */
    STM_LSM303DLHC_MAG_ODR220  =        0x1C        /* 220 Hz output data rate */
} stm_lsm303dlhc_mag_odr;

#define STM_LSM303DLHC_MAG_ODR_NUM      8
#define STM_LSM303DLHC_MAG_MAX_ODR      STM_LSM303DLHC_MAG_ODR220

/**
 * Magnetometer LSM303DLHC data bit length
 */
#define STM_MAG_SENSOR_BIT_LEN          16

/**
 * Magnetometer LSM303DLHC register address
 */
#define STM_LSM303DLHC_CTRL_CRA_REG     0x00    /* Configuration register A */
#define STM_LSM303DLHC_CTRL_CRB_REG     0x01    /* Configuration register B */
#define STM_LSM303DLHC_MR_REG           0x02    /* Mode register */
#define STM_LSM303DLHC_OUT_X_M          0x03    /* Output register start address*/
#define STM_LSM303DLHC_IR_REG_M         0x0A    /* IR registers start address*/

#endif  /* __LSM303DLHC__ */
