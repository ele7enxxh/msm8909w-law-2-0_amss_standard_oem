/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
*
* File Name         : sns_dd_lsm303d.h
* Authors           : Samuel(Jianjian) Huo
*                   : Karimuddin sayed 
* Version           : V 2.3.2
* Date              : 10/31/2013
* Description       : LSM303D driver header file
*
********************************************************************************
* Copyright (c) 2013, STMicroelectronics.
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

#ifndef __LSM303D__
#define __LSM303D__

#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_util.h"
#include <stdbool.h>

#define LSM303D_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "LSM303D - " msg)
#define LSM303D_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "LSM303D - " msg,p1)
#define LSM303D_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "LSM303D - " msg,p1,p2)
#define LSM303D_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "LSM303D - " msg,p1,p2,p3)

/**
 * Magnetometer LSM303D Full Scales in register setting.
 */
typedef enum 
{
    STM_LSM303D_MAG_RANGE_2G   = 0x00,  /*corresponding value in register setting*/
    STM_LSM303D_MAG_RANGE_4G   = 0x20,
    STM_LSM303D_MAG_RANGE_8G   = 0x40,  
    STM_LSM303D_MAG_RANGE_12G  = 0x60,
} stm_lsm303d_mag_range;

#define STM_LSM303D_MAG_RANGE_NUM 4 
#define STM_LSM303D_MAG_MAX_RANGE STM_LSM303D_MAG_RANGE_12G

/**
 * Magnetometer LSM303D min and max of ranges, converted to Q16.
 */
#define STM_LSM303D_MAG_RANGE_2G_MIN    FX_FLTTOFIX_Q16(-2.0)
#define STM_LSM303D_MAG_RANGE_2G_MAX    FX_FLTTOFIX_Q16(2.0)
#define STM_LSM303D_MAG_RANGE_4G_MIN    FX_FLTTOFIX_Q16(-4.0)
#define STM_LSM303D_MAG_RANGE_4G_MAX    FX_FLTTOFIX_Q16(4.0)
#define STM_LSM303D_MAG_RANGE_8G_MIN    FX_FLTTOFIX_Q16(-8.0)
#define STM_LSM303D_MAG_RANGE_8G_MAX    FX_FLTTOFIX_Q16(8.0)
#define STM_LSM303D_MAG_RANGE_12G_MIN   FX_FLTTOFIX_Q16(-12.0)
#define STM_LSM303D_MAG_RANGE_12G_MAX   FX_FLTTOFIX_Q16(12.0)

/**
 * Magnetometer LSM303D X-Y sensitivity, unit is uGauss.
 */
typedef enum
{
    STM_LSM303D_MAG_SSTVT_2G   = 80,   /* sensitivity at 2G */
    STM_LSM303D_MAG_SSTVT_4G   = 160,  /* sensitivity at 4G */
    STM_LSM303D_MAG_SSTVT_8G   = 320,  /* sensitivity at 6G */
    STM_LSM303D_MAG_SSTVT_12G  = 479,  /* sensitivity at 8G */
}stm_lsm303d_mag_sstvt;

/**
 * Magnetometer LSM303D output data rate
 */
typedef enum 
{
    STM_LSM303D_MAG_ODR3_125 =        0x00,       /* 3.125Hz output data rate */
    STM_LSM303D_MAG_ODR6_25  =        0x04,       /* 6.25Hz output data rate */
    STM_LSM303D_MAG_ODR12_5  =        0x08,       /* 12.5Hz output data rate */
    STM_LSM303D_MAG_ODR25    =        0x0C,       /* 25Hz output data rate */
    STM_LSM303D_MAG_ODR50    =        0x10,       /* 50Hz output data rate */
    STM_LSM303D_MAG_ODR100   =        0x14,       /* 1000Hz output data rate */
} stm_lsm303d_mag_odr;


#define STM_LSM303D_MAG_ODR_NUM     6
#define STM_LSM303D_MAG_MAX_ODR     100

/**
 * Accelerometer LSM303D_ACC Full Scales in register setting.
 */
typedef enum 
{
    STM_LSM303D_ACC_RANGE_2G   = 0x00,  /*corresponding value in register setting*/
    STM_LSM303D_ACC_RANGE_4G   = 0x08,
    STM_LSM303D_ACC_RANGE_6G   = 0x10,    
    STM_LSM303D_ACC_RANGE_8G   = 0x18,  
    STM_LSM303D_ACC_RANGE_16G  = 0x20
} stm_lsm303d_acc_range;

#define STM_LSM303D_ACC_NUM_AXES   3
#define STM_LSM303D_ACC_RANGE_NUM  5
#define STM_LSM303D_ACC_MAX_RANGE  STM_LSM303D_ACC_RANGE_16G

/**
 * Accelerometer LSM303D_ACC ranges, converted to Q16.
 */
#define STM_LSM303D_ACC_RANGE_2G_MIN    FX_FLTTOFIX_Q16(-2*G)
#define STM_LSM303D_ACC_RANGE_2G_MAX    FX_FLTTOFIX_Q16(2*G)
#define STM_LSM303D_ACC_RANGE_4G_MIN    FX_FLTTOFIX_Q16(-4*G)
#define STM_LSM303D_ACC_RANGE_4G_MAX    FX_FLTTOFIX_Q16(4*G)
#define STM_LSM303D_ACC_RANGE_6G_MIN    FX_FLTTOFIX_Q16(-6*G)
#define STM_LSM303D_ACC_RANGE_6G_MAX    FX_FLTTOFIX_Q16(6*G)
#define STM_LSM303D_ACC_RANGE_8G_MIN    FX_FLTTOFIX_Q16(-8*G)
#define STM_LSM303D_ACC_RANGE_8G_MAX    FX_FLTTOFIX_Q16(8*G)
#define STM_LSM303D_ACC_RANGE_16G_MIN   FX_FLTTOFIX_Q16(-16*G)
#define STM_LSM303D_ACC_RANGE_16G_MAX   FX_FLTTOFIX_Q16(16*G)

/**
 * Accelerometer LSM303D_ACC sensitivity for each range.
 */
 typedef enum 
{
    STM_LSM303D_ACC_SSTVT_2G  = 61,   /* in the unit of micro-g/digit */
    STM_LSM303D_ACC_SSTVT_4G  = 122,
    STM_LSM303D_ACC_SSTVT_6G  = 183,    
    STM_LSM303D_ACC_SSTVT_8G  = 244,  
    STM_LSM303D_ACC_SSTVT_16G = 732
}stm_lsm303d_acc_sstvt;

/**
 * Accelerometer LSM303D_ACC output data rate in register setting
 */
 typedef enum 
{
    STM_LSM303D_ACC_ODR_OFF   = 0x00,
    STM_LSM303D_ACC_ODR3_125  = 0x10,  /* 3.125 Hz output data rate */
    STM_LSM303D_ACC_ODR6_25   = 0x20,  /* 6.25 Hz output data rate */
    STM_LSM303D_ACC_ODR12_5   = 0x30,  /* 12.5 Hz output data rate */
    STM_LSM303D_ACC_ODR25     = 0x40,  /* 25 Hz output data rate */
    STM_LSM303D_ACC_ODR50     = 0x50,  /* 50 Hz output data rate */
    STM_LSM303D_ACC_ODR100    = 0x60,  /* 100Hz output data rate */
    STM_LSM303D_ACC_ODR200    = 0x70,  /* 100Hz output data rate */
    STM_LSM303D_ACC_ODR400    = 0x80,  /* 400Hz output data rate */
    STM_LSM303D_ACC_ODR800    = 0x90,  /* 800Hz output data rate */    
    STM_LSM303D_ACC_ODR1600   = 0xA0   /* 1600 Hz output data rate */
} stm_lsm303d_acc_odr;

/**
 * Accelerometer LSM303D ACC filter bandwidth in register setting
 */
typedef enum
{
  STM_LSM303D_ACC_BW50      = 0xC0,  /* 50 Hz bandwidth */
  STM_LSM303D_ACC_BW200     = 0x40,  /* 194 Hz bandwidth */
  STM_LSM303D_ACC_BW400     = 0x80,  /* 362 Hz bandwidth */
  STM_LSM303D_ACC_BW800     = 0x00   /* 773 Hz bandwidth */
} stm_lsm303d_acc_bw;

#define STM_LSM303D_ACC_ODR_NUM         10
#define STM_LSM303D_ACC_MAX_ODR         1600 


/* Address registers */
#define STM_LSM303D_REG_WHOAMI    (0x0F)  /** Who am i address register */
#define STM_LSM303D_REG_CNTRL0    (0x1F)  /** CNTRL0 address register */
#define STM_LSM303D_REG_CNTRL1    (0x20)  /** CNTRL1 address register */
#define STM_LSM303D_REG_CNTRL2    (0x21)  /** CNTRL2 address register */
#define STM_LSM303D_REG_CNTRL3    (0x22)  /** CNTRL3 address register */
#define STM_LSM303D_REG_CNTRL4    (0x23)  /** CNTRL4 address register */
#define STM_LSM303D_REG_CNTRL5    (0x24)  /** CNTRL5 address register */
#define STM_LSM303D_REG_CNTRL6    (0x25)  /** CNTRL6 address register */
#define STM_LSM303D_REG_CNTRL7    (0x26)  /** CNTRL7 address register */
#define STM_LSM303D_REG_STATUS_A  (0x27)  /** accel STATUS address register */
#define STM_LSM303D_REG_OUT_X_L_A  (0x28)  /** Acc. data low address register */
#define STM_LSM303D_REG_OUT_X_L_M (0x08)  /** Mag. data low address register */
#define STM_LSM303D_REG_TEMP_DATA  (0x05)  /** Temp. data low address register */
       
#define STM_LSM303D_REG_GEN_MAG    (0x12)  /** INT_CTRL_REG_M address register */
#define STM_LSM303D_INT_SRC_REG_M  (0x13)  /** INT_SRC_REG_M address register */
#define STM_LSM303D_REG_GEN_MAG_THR    (0x14)  /** INT_THS_L_M address register */
#define STM_LSM303D_MIG_THRESHOLD_H    (0x15)  /** INT_THS_H_M address register */
#define STM_LSM303D_REG_GEN1_AXIS  (0x30)  /** INT_GEN1_REG address register */
#define STM_LSM303D_INT_GEN1_SRC   (0x31)  /** INT_GEN1_SRC address register */
#define STM_LSM303D_REG_GEN1_THR   (0x32)  /** INT_GEN1_THS address register */
#define STM_LSM303D_REG_GEN1_DUR   (0x33)  /** INT_GEN1_DUR address register */
#define STM_LSM303D_REG_GEN2_AXIS  (0x34)  /** INT_GEN2_REG address register */
#define STM_LSM303D_INT_GEN2_SRC   (0x35)  /** INT_GEN2_SRC address register */
#define STM_LSM303D_REG_GEN2_THR   (0x36)  /** INT_GEN2_THS address register */
#define STM_LSM303D_REG_GEN2_DUR   (0x37)  /** INT_GEN2_DUR address register */
        

/* Default values loaded in probe function */
#define STM_LSM303D_WHOAMI_VALUE        (0x49)  /** Who Am I default value */
#define STM_LSM303D_REG_DEF_CNTRL0      (0x00)  /** CNTRL0 default value */
#define STM_LSM303D_REG_DEF_CNTRL1      (0x07)  /** CNTRL1 default value */
#define STM_LSM303D_REG_DEF_CNTRL2      (0x00)  /** CNTRL2 default value */
#define STM_LSM303D_REG_DEF_CNTRL3      (0x00)  /** CNTRL3 default value */
#define STM_LSM303D_REG_DEF_CNTRL4      (0x00)  /** CNTRL4 default value */
#define STM_LSM303D_REG_DEF_CNTRL5      (0x18)  /** CNTRL5 default value */
#define STM_LSM303D_REG_DEF_CNTRL6      (0x20)  /** CNTRL6 default value */
#define STM_LSM303D_REG_DEF_CNTRL7      (0x02)  /** CNTRL7 default value */
        
#define STM_LSM303D_REG_DEF_INT_CNTRL_MAG   (0x00)  /** INT_CTRL_REG_M default value */
#define STM_LSM303D_REG_DEF_INT_GEN1    (0x00)  /** INT_GEN1_REG default value */
#define STM_LSM303D_REG_DEF_INT_GEN2    (0x00)  /** INT_GEN2_REG default value */
#define STM_LSM303D_REG_DEF_IIG1_DURATION   (0x00)  /** INT_GEN1_DUR default value */
#define STM_LSM303D_REG_DEF_IIG2_DURATION   (0x00)  /** INT_GEN2_DUR default value */
#define STM_LSM303D_REG_DEF_IIG1_THRESHOLD  (0x00)  /** INT_GEN1_THS default value */
#define STM_LSM303D_REG_DEF_IIG2_THRESHOLD  (0x00)  /** INT_GEN2_THS default value */
#define STM_LSM303D_REG_DEF_MIG_THRESHOLD_L (0x00)  /** INT_THS_L_M default value */
#define STM_LSM303D_REG_DEF_MIG_THRESHOLD_H (0x00)  /** INT_THS_H_M default value */

#define STM_LSM303D_REG_DEF_ALL_ZEROS   (0x00)


/**
 * Magnetometer LSM303D data bit length
 */
#define STM_MAG_SENSOR_BIT_LEN          16

#define LSM303D_MAX_SENSOR_TYPES  2
#define LSM303D_MAX_SUB_DEV       2
#define DD_HANDLE_ALIGN_32BITS    -4
#define DD_HANDLE_ALIGN_BITS      DD_HANDLE_ALIGN_32BITS
#define DD_HANDLE_ALIGN_64        DD_HANDLE_ALIGN_BITS

/**
 * Accelerometer LSM303D Driver, device instance chain Structure
 */
typedef struct
{
  sns_ddf_dev_list_t next;       /* the next device instance */
  sns_ddf_driver_info_s dd_info; /* the driver info */
}stm_lsm303d_head_s;

/**
 * Accelerometer LSM303D Driver, sub device Structure
 */
typedef struct
{
  uint32_t num_sensors;

  sns_ddf_sensor_e sensors[LSM303D_MAX_SENSOR_TYPES];

  /**< sensor power state. */
  sns_ddf_powerstate_e powerstate;

  /**< Handle used with sns_ddf_notify_data. */
  sns_ddf_handle_t smgr_handle;
}stm_lsm303d_sub_dev_s;

/**
 * LSM303D Driver State Information Structure
 */
typedef struct {
    /**< pointer to next device int the device chain. */
    sns_ddf_dev_list_t next;

    /**< the identifier of this port. */
    sns_ddf_port_id_s port_id;

    /**< Handle used to access the I2C bus. */
    sns_ddf_handle_t port_handle; 

    /**< sub device count.*/
    uint32_t sub_dev_cnt;

    /**< supported sub device array.*/
    stm_lsm303d_sub_dev_s sub_dev[LSM303D_MAX_SUB_DEV];

    /**< Current accel sampling frequency. */
    sns_ddf_odr_t acc_cur_rate;

    /**< desired accel sensor sampling frequency. */
    sns_ddf_odr_t acc_desired_rate;

    /**< Current mag sensor sampling frequency. */
    sns_ddf_odr_t mag_cur_rate;

    /**< desired mag sensor sampling frequency. */
    sns_ddf_odr_t mag_desired_rate;

    /**< Current range selection.*/
    stm_lsm303d_acc_range acc_range; 

    /**< Current range selection.*/
    stm_lsm303d_mag_range mag_range; 
    
    /**< Current acc sensitivity. */
    stm_lsm303d_acc_sstvt acc_sstvt_adj;

    /**< Current mag sensitivity. */
    stm_lsm303d_mag_sstvt mag_sstvt_adj;
    
    /**< Device-to-phone axis mapping. */
    sns_ddf_axes_map_s axes_map;

    /** GPIO number used for interrupts */
    uint32_t gpio_num;

    /** LIS303D accel zero-g biases after inertial SW self test and calibration */
    q16_t biases[STM_LSM303D_ACC_NUM_AXES];

} sns_dd_lsm303d_state_t;

/**
 * Accelerometer LSM303D hardware self test settings
 */
#define STM_LSM303D_ST_2G_MIN       70      //Unit: mg
#define STM_LSM303D_ST_2G_MAX       1700    //Unit: mg

/**
 * Accelerometer LSM303D SW inertial self test settings
 */
#define STM_LSM303D_SWST_TPL_BIAS   40      //Unit: mg
#define STM_LSM303D_SWST_MAX_BIAS   170     //Unit: mg

#endif  /* __LSM303D__ */
