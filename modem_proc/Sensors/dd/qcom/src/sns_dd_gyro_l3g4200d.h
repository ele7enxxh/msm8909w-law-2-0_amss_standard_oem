/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
*
* File Name         : sns_dd_gyro_l3g4200d.h
* Authors           : Jianjian Huo (samuel.huo@st.com)
* Version           : V 0.9.2
* Date              : 07/07/2011
* Description       : L3G4200D gyroscope driver header file
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

#ifndef __L3G4200D__
#define __L3G4200D__

#include "fixed_point.h"

/**
 * Gyroscope L3G4200D Full Scales in register setting.
 */
typedef enum 
{
    STM_L3G4200D_RANGE_250DPS   =  0x00,  /*corresponding value in register setting*/
    STM_L3G4200D_RANGE_500DPS   =  0x10,
    STM_L3G4200D_RANGE_2000DPS  =  0x20
} stm_l3g4200d_range;

#define STM_L3G4200D_RANGE_NUM     3
#define STM_L3G4200D_MAX_RANGE     STM_L3G4200D_RANGE_2000DPS

/**
 * Gyroscope L3G4200D min and max of ranges, converted to Q16.
 */
#define STM_L3G4200D_RANGE_250_MIN    FX_FLTTOFIX_Q16(-250)
#define STM_L3G4200D_RANGE_250_MAX    FX_FLTTOFIX_Q16(250)
#define STM_L3G4200D_RANGE_500_MIN    FX_FLTTOFIX_Q16(-500)
#define STM_L3G4200D_RANGE_500_MAX    FX_FLTTOFIX_Q16(500)
#define STM_L3G4200D_RANGE_2000_MIN   FX_FLTTOFIX_Q16(-2000)
#define STM_L3G4200D_RANGE_2000_MAX   FX_FLTTOFIX_Q16(2000)

/**
 * Gyroscope L3G4200D sensitivity
 */
typedef enum
{
    STM_L3G4200D_SSTVT_250  =     875,  /* value of sensitivity*100 at 250dps */
    STM_L3G4200D_SSTVT_500  =     1750, /* value of sensitivity*100 at 500dps */
    STM_L3G4200D_SSTVT_2000 =     7000  /* value of sensitivity*100 at 2000dps */
}stm_l3g4200d_sstvt;

/**
 * Gyroscope L3G4200D output data rate
 */
typedef enum 
{
    STM_L3G4200D_ODR100  =        0x00|0x00,  /* 100Hz output data rate */
    STM_L3G4200D_ODR200  =        0x40|0x10,  /* 200Hz output data rate */
    STM_L3G4200D_ODR400  =        0x80|0x00,  /* 400Hz output data rate */
    STM_L3G4200D_ODR800  =        0xC0|0x00   /* 800Hz output data rate */
} stm_l3g4200d_odr;

#define STM_L3G4200D_ODR_NUM      4
#define STM_L3G4200D_MAX_ODR      STM_L3G4200D_ODR800

/**
 * Gyroscope L3G4200D data bit length
 */
#define STM_GYR_L3G4200D_SENSOR_BIT_LEN          16

/**
 * Gyroscope L3G4200D register address
 */
#define STM_L3G4200D_WHO_AM_I     0x0F    /* WHO AM I register */
#define STM_L3G4200D_CTRL_REG1    0x20    /* Configuration register 1 */
#define STM_L3G4200D_CTRL_REG2    0x21    /* Configuration register 2 */
#define STM_L3G4200D_CTRL_REG3    0x22    /* Configuration register 3 */
#define STM_L3G4200D_CTRL_REG4    0x23    /* Configuration register 4 */
#define STM_L3G4200D_CTRL_REG5    0x24    /* Configuration register 5 */
#define STM_L3G4200D_REFERENCE    0x25
#define STM_L3G4200D_OUT_TEMP     0x26
#define STM_L3G4200D_STATUS_REG   0x27
#define STM_L3G4200D_OUT_X_L      0x28
#define STM_L3G4200D_OUT_X_H      0x29
#define STM_L3G4200D_OUT_Y_L      0x2A
#define STM_L3G4200D_OUT_Y_H      0x2B
#define STM_L3G4200D_OUT_Z_L      0x2C
#define STM_L3G4200D_OUT_Z_H      0x2D
#define STM_L3G4200D_FIFO_CTRL    0x2E
#define STM_L3G4200D_FIFO_SRC     0x2F
#define STM_L3G4200D_INT1_CFG     0x30
#define STM_L3G4200D_INT1_SRC     0x31

#endif  /* __L3G4200D__ */
