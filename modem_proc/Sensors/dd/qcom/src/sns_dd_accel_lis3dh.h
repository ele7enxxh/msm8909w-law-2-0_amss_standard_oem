/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
*
* File Name         : lis3dh_ddf_drv.h
* Authors           : Jianjian Huo (samuel.huo@st.com)
* Version           : V 0.9.2
* Date              : 05/19/2011
* Description       : LIS3DH Accelerometer driver header file
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

#ifndef __LIS3DH__
#define __LIS3DH__

#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_util.h"

/**
 * Accelerometer LIS3DH Full Scales in register setting.
 */
typedef enum 
{
    STM_LIS3DH_RANGE_2G   = 0x00,  /*corresponding value in register setting*/
    STM_LIS3DH_RANGE_4G   = 0x10,
    STM_LIS3DH_RANGE_8G   = 0x20,  
    STM_LIS3DH_RANGE_16G  = 0x30
} stm_lis3dh_range;

#define STM_LIS3DH_NUM_AXES   3
#define STM_LIS3DH_RANGE_NUM  4
#define STM_LIS3DH_MAX_RANGE  STM_LIS3DH_RANGE_16G

/**
 * Accelerometer LIS3DH ranges, converted to Q16.
 */
#define STM_LIS3DH_RANGE_2G_MIN    FX_FLTTOFIX_Q16(-2*G)
#define STM_LIS3DH_RANGE_2G_MAX    FX_FLTTOFIX_Q16(2*G)
#define STM_LIS3DH_RANGE_4G_MIN    FX_FLTTOFIX_Q16(-4*G)
#define STM_LIS3DH_RANGE_4G_MAX    FX_FLTTOFIX_Q16(4*G)
#define STM_LIS3DH_RANGE_8G_MIN    FX_FLTTOFIX_Q16(-8*G)
#define STM_LIS3DH_RANGE_8G_MAX    FX_FLTTOFIX_Q16(8*G)
#define STM_LIS3DH_RANGE_16G_MIN   FX_FLTTOFIX_Q16(-16*G)
#define STM_LIS3DH_RANGE_16G_MAX   FX_FLTTOFIX_Q16(16*G)

/**
 * Accelerometer LIS3DH sensitivity for each range.
 */
 typedef enum 
{
    STM_LIS3DH_SSTVT_2G  = 1,
    STM_LIS3DH_SSTVT_4G  = 2,
    STM_LIS3DH_SSTVT_8G  = 4,  
    STM_LIS3DH_SSTVT_16G = 12
}stm_lis3dh_sstvt;

/**
 * Accelerometer LIS3DH output data rate in register setting
 */
 typedef enum 
{
    STM_LIS3DH_ODR_OFF   = 0x00,
    STM_LIS3DH_ODR1      = 0x10,  /* 1Hz output data rate */
    STM_LIS3DH_ODR10     = 0x20,  /* 10Hz output data rate */
    STM_LIS3DH_ODR25     = 0x30,  /* 25Hz output data rate */
    STM_LIS3DH_ODR50     = 0x40,  /* 50Hz output data rate */
    STM_LIS3DH_ODR100    = 0x50,  /* 100Hz output data rate */
    STM_LIS3DH_ODR200    = 0x60,  /* 200Hz output data rate */
    STM_LIS3DH_ODR400    = 0x70,  /* 400Hz output data rate */
    STM_LIS3DH_ODR1250   = 0x90   /* 1250Hz output data rate */
} stm_lis3dh_odr;


/**
 * Accelerometer LIS3DH Driver State Information Structure
 */
typedef struct {
    /**< Handle used with sns_ddf_notify_data. */
    sns_ddf_handle_t smgr_hndl; 

    /**< Handle used to access the I2C bus. */
    sns_ddf_handle_t port_handle; 

    /**< Current range selection.*/
    stm_lis3dh_range range; 

    /**< Current sampling frequency. */
    stm_lis3dh_odr data_rate; 

    /**< Previous sampling frequency, used when enters and exits LOW_POWER mode. */
    stm_lis3dh_odr prvs_rate; 

    /**< Current sensitivity. */
    stm_lis3dh_sstvt sstvt_adj;

    /**< Current power state: ACTIVE or LOWPOWER */
    sns_ddf_powerstate_e power_state; 

    /**< bias setting. */
    q16_t bias[STM_LIS3DH_NUM_AXES];
    
    /**< data cache for storing sensor data. */
    q16_t data_cache[STM_LIS3DH_NUM_AXES];

    /**< flag for sensor data out of bound. */
    uint8_t out_of_bound_flag;

    /**< Device-to-phone axis mapping. */
    sns_ddf_axes_map_s axes_map;

    /** GPIO number used for interrupts */
    uint32_t gpio_num;

} sns_dd_acc_lis3dh_state_t;


#define STM_LIS3DH_ODR_NUM         8
#define STM_LIS3DH_MAX_ODR         STM_LIS3DH_ODR1250

/**
 * Accelerometer LIS3DH I2C address
 */
#define STM_LIS3DH_I2C_ADDR        0x19
#define STM_LIS3DH_I2C_ADDR_W      (STM_LIS3DH_I2C_ADDR<<1)     //0x3a
#define STM_LIS3DH_I2C_ADDR_R      (STM_LIS3DH_I2C_ADDR_W+1)    //0x3b

/**
 * Accelerometer LIS3DH register address
 */
#define STM_LIS3DH_WHO_AM_I_A      0x0f
#define STM_LIS3DH_TEMP_REG_A      0x1f
#define STM_LIS3DH_CTRL_REG1_A     0x20
#define STM_LIS3DH_CTRL_REG2_A     0x21
#define STM_LIS3DH_CTRL_REG3_A     0x22
#define STM_LIS3DH_CTRL_REG4_A     0x23
#define STM_LIS3DH_CTRL_REG5_A     0x24
#define STM_LIS3DH_CTRL_REG6_A     0x25
#define STM_LIS3DH_REFERENCE       0x26
#define STM_LIS3DH_STATUS_REG_A    0x27
#define STM_LIS3DH_OUT_X_L_A       0x28
#define STM_LIS3DH_OUT_X_H_A       0x29
#define STM_LIS3DH_OUT_Y_L_A       0x2A
#define STM_LIS3DH_OUT_Y_H_A       0x2B
#define STM_LIS3DH_OUT_Z_L_A       0x2C
#define STM_LIS3DH_OUT_Z_H_A       0x2D
#define STM_LIS3DH_FIFO_CTRL_A     0x2E
#define STM_LIS3DH_INT1_CFG        0x30
#define STM_LIS3DH_INT1_SRC        0x31
#define STM_LIS3DH_INT1_THS        0x32
#define STM_LIS3DH_INT1_DURATION   0x33

/**
 * LIS3DH resolution
 */
#define STM_LIS3DH_TEMP_RESLT      FX_FLTTOFIX_Q16(0.01)

/**
 * Accelerometer LIS3DH orientation settings, to comply with SAE coordinates
 */
#define STM_LIS3DH_SDD_ACC_X       0
#define STM_LIS3DH_SDD_ACC_Y       1
#define STM_LIS3DH_SDD_ACC_Z       2

#endif  /* __LIS3DH__ */
