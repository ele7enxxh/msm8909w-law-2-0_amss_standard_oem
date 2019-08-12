/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
 *
 * File Name         : lis3dsh_ddf_drv.h
 * Authors           : Jianjian(Samuel) Huo
 * Version           : V 3.6.1
 * Date              : 03/13/2014
 * Description       : LIS3DSH Accelerometer driver header file
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

/*******************************************************************************
* Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
*******************************************************************************/

/*=============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  06/05/2014  vy   Refactored for uImage
  11/18/2013  pn   Replaced BW timer with simple timestamp
  10/25/2013  vh   Synced with STM's version number
  10/22/2013  vh   Added Qualcomm Copyright
=============================================================================*/

#ifndef __LIS3DSH__
#define __LIS3DSH__

#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_util.h"
#include "driver_access_framework.h"
#include <stdbool.h>

typedef uint32_t dd_handle_align_t;

//#define LIS3DSH_DEBUG

#ifdef LIS3DSH_DEBUG
#define LIS3DSH_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "LIS3DSH - " msg)
#define LIS3DSH_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "LIS3DSH - " msg,p1)
#define LIS3DSH_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "LIS3DSH - " msg,p1,p2)
#define LIS3DSH_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "LIS3DSH - " msg,p1,p2,p3)
#else //LIS3DSH_DEBUG
#define LIS3DSH_MSG_0(level,msg)
#define LIS3DSH_MSG_1(level,msg,p1)
#define LIS3DSH_MSG_2(level,msg,p1,p2)
#define LIS3DSH_MSG_3(level,msg,p1,p2,p3)
#endif //LIS3DSH_DEBUG

/**
 * Accelerometer LIS3DSH Full Scales in register setting.
 */
typedef enum
{
  STM_LIS3DSH_RANGE_2G   = 0x00,  /*corresponding value in register setting*/
  STM_LIS3DSH_RANGE_4G   = 0x08,
  STM_LIS3DSH_RANGE_6G   = 0x10,
  STM_LIS3DSH_RANGE_8G   = 0x18,
  STM_LIS3DSH_RANGE_16G  = 0x20
} stm_lis3dsh_range;

#define STM_LIS3DSH_NUM_AXES      3
#define STM_LIS3DSH_RANGE_NUM     5
#define STM_LIS3DSH_MAX_RANGE     STM_LIS3DSH_RANGE_16G

#define STM_LIS3DSH_DBT_RANGE_MIN    SNS_DDF_TAP_DIR_NONE
#define STM_LIS3DSH_DBT_RANGE_MAX    (SNS_DDF_TAP_DIR_LAST - 1)
#define STM_LIS3DSH_DBT_RANGE_NUM 1


#define LIS3DSH_MAX_SENSOR_TYPES  2
#define LIS3DSH_MAX_SUB_DEV       2
#define LIS3DSH_DBT_ODR           2
//#define DD_HANDLE_ALIGN_32BITS    0xFFFFFFFFFFFFFFFC
#define DD_HANDLE_ALIGN_32BITS    -4

#define DD_HANDLE_ALIGN_BITS      DD_HANDLE_ALIGN_32BITS
#define DD_HANDLE_ALIGN_64        DD_HANDLE_ALIGN_BITS

/**
 * Accelerometer LIS3DSH ranges, converted to Q16.
 */
#define STM_LIS3DSH_RANGE_2G_MIN    FX_FLTTOFIX_Q16(-2*G)
#define STM_LIS3DSH_RANGE_2G_MAX    FX_FLTTOFIX_Q16(2*G)
#define STM_LIS3DSH_RANGE_4G_MIN    FX_FLTTOFIX_Q16(-4*G)
#define STM_LIS3DSH_RANGE_4G_MAX    FX_FLTTOFIX_Q16(4*G)
#define STM_LIS3DSH_RANGE_6G_MIN    FX_FLTTOFIX_Q16(-6*G)
#define STM_LIS3DSH_RANGE_6G_MAX    FX_FLTTOFIX_Q16(6*G)
#define STM_LIS3DSH_RANGE_8G_MIN    FX_FLTTOFIX_Q16(-8*G)
#define STM_LIS3DSH_RANGE_8G_MAX    FX_FLTTOFIX_Q16(8*G)
#define STM_LIS3DSH_RANGE_16G_MIN   FX_FLTTOFIX_Q16(-16*G)
#define STM_LIS3DSH_RANGE_16G_MAX   FX_FLTTOFIX_Q16(16*G)

/**
 * Accelerometer LIS3DSH sensitivity for each range.
 */
typedef enum
{
  STM_LIS3DSH_SSTVT_2G  = 61,   /* in the unit of micro-g/digit */
  STM_LIS3DSH_SSTVT_4G  = 122,
  STM_LIS3DSH_SSTVT_6G  = 183,
  STM_LIS3DSH_SSTVT_8G  = 244,
  STM_LIS3DSH_SSTVT_16G = 732
}stm_lis3dsh_sstvt;

/**
 * Accelerometer LIS3DSH output data rate in register setting
 */
typedef enum
{
  STM_LIS3DSH_ODR_OFF   = 0x00,
  STM_LIS3DSH_ODR3      = 0x10,  /* 3.125 Hz output data rate */
  STM_LIS3DSH_ODR6      = 0x20,  /* 6.25 Hz output data rate */
  STM_LIS3DSH_ODR12p5   = 0x30,  /* 12.5 Hz output data rate */
  STM_LIS3DSH_ODR25     = 0x40,  /* 25 Hz output data rate */
  STM_LIS3DSH_ODR50     = 0x50,  /* 50 Hz output data rate */
  STM_LIS3DSH_ODR100    = 0x60,  /* 100Hz output data rate */
  STM_LIS3DSH_ODR400    = 0x70,  /* 400Hz output data rate */
  STM_LIS3DSH_ODR800    = 0x80,  /* 800Hz output data rate */
  STM_LIS3DSH_ODR1600   = 0x90   /* 1600 Hz output data rate */
} stm_lis3dsh_odr;

/**
 * Accelerometer LIS3DSH filter bandwidth in register setting
 */
typedef enum
{
  STM_LIS3DSH_BW50      = 0xC0,  /* 50Hz bandwidth */
  STM_LIS3DSH_BW200     = 0x80,  /* 200 Hz bandwidth */
  STM_LIS3DSH_BW400     = 0x40,  /* 400 Hz bandwidth */
  STM_LIS3DSH_BW800     = 0x00   /* 800 Hz bandwidth */
} stm_lis3dsh_bw;

/**
 * Accelerometer LIS3DSH Driver, device instance chain Structure
 */
typedef struct
{
  sns_ddf_dev_list_t next;       /* the next device instance */
  sns_ddf_driver_info_s dd_info; /* the driver info */
}stm_lis3dsh_head_s;

/**
 * Accelerometer LIS3DSH Driver, sub device Structure
 */
typedef struct
{
  uint32_t num_sensors;

  sns_ddf_sensor_e sensors[LIS3DSH_MAX_SENSOR_TYPES];

  /**< sensor power state. */
  sns_ddf_powerstate_e powerstate;

  /**< Handle used with sns_ddf_notify_data. */
  sns_ddf_handle_t smgr_handle;
}stm_lis3dsh_sub_dev_s;

/**
 * Accelerometer LIS3DSH Driver, NV_PARAMS Structure
 */
#define STM_LIS3DSH_NVPARAMS_DATALEN     136
#define STM_LIS3DSH_NVPARAMS_OFFSET      64
#define STM_LIS3DSH_MAG_NUM1             0x01
#define STM_LIS3DSH_MAG_NUM2             0xDB
#define STM_LIS3DSH_MAG_NUM3             0xBA
#define STM_LIS3DSH_MAG_NUM4             0xAB
typedef PACK(struct)
{
  /**< Magic number string for LIS3DSH */
  uint8_t  magic_num[4];
  /**< motion detection threshold. 0x0: 31.25 mg,  0x1: 62.5 mg*/
  uint8_t  md_thresh;
  /**< Num_of_samples to check. 0x0: 1 sample, 0x1 or more: multiple samples.*/
  uint8_t  consistency_cnt;
  /**< motion detection ODR. 50 for 50Hz. */
  uint16_t md_ODR;
  /**< MD reserved. */
  uint32_t md_rsvd[6];
  /**< double tap threshold.   0x0: 31.25 mg,  0x1: 62.5 mg*/
  uint8_t  dbt_thresh_1;
  /**< double tap threshold reserved. */
  uint8_t  dbt_thresh_2;
  /**< double tap state machine timer1. unit 2.5 ms. 0x1: 2.5 ms */
  uint16_t dbt_tim1;
  /**< double tap state machine timer2. unit 2.5 ms. 0x1: 2.5 ms */
  uint16_t dbt_tim2;
  /**< double tap state machine timer3. unit 2.5 ms. 0x1: 2.5 ms */
  uint8_t  dbt_tim3;
  /**< double tap state machine timer4. unit 2.5 ms. 0x1: 2.5 ms */
  uint8_t  dbt_tim4;
  /**< double tap reserved. */
  uint32_t dbt_rsvd[5];
}stm_lis3dsh_nv_params;

/**
 * Accelerometer LIS3DSH Driver, SM1 and SM2 configuration Structure
 */
typedef struct
{
  /**< motion detection threshold.  0x0: 31.25 mg,  0x1: 62.5 mg*/
  uint8_t  md_thresh;
  /**< Num_of_samples to check. 0x0: 1 sample, 0x1 or more: multiple samples.*/
  uint8_t  consistency_cnt;
  /**< motion detection ODR. 50 for 50Hz. */
  uint16_t md_ODR;
  /**< double tap threshold.  0x0: 31.25 mg,  0x1: 62.5 mg */
  uint8_t  dbt_thresh_1;
  /**< double tap threshold.  0x0: 31.25 mg,  0x1: 62.5 mg */
  uint8_t  dbt_thresh_2;
  /**< double tap state machine timer1. unit 2.5 ms. 0x1: 2.5 ms */
  uint16_t dbt_tim1;
  /**< double tap state machine timer2. unit 2.5 ms. 0x1: 2.5 ms */
  uint16_t dbt_tim2;
  /**< double tap state machine timer3. unit 2.5 ms. 0x1: 2.5 ms */
  uint8_t  dbt_tim3;
  /**< double tap state machine timer4. unit 2.5 ms. 0x1: 2.5 ms */
  uint8_t  dbt_tim4;
}stm_lis3dsh_sm_config;

/**
 * Accelerometer LIS3DSH Driver, SM1 and SM2 configuration default value
 */
//Default setting when DBT is off.
#define STM_LIS3DSH_MD_THRESH          0x0    //31.25 mg.
#define STM_LIS3DSH_CONSTANT_CNT       0x0    // 1 sample
#define STM_LIS3DSH_MD_ODR             10     //10 Hz
//Default setting when DBT is on.
//The new setting through registry will overwrite this one.
#define STM_LIS3DSH_MD_THRESH_2        0x1    //62.5 mg.
#define STM_LIS3DSH_CONSTANT_CNT_2     0x0    // 1 sample
#define STM_LIS3DSH_MD_ODR_2           400    //400 Hz

#define STM_LIS3DSH_DBT_THRESH_1       0x11   //562.5 mg
#define STM_LIS3DSH_DBT_THRESH_2       0x0F   //468 mg
#define STM_LIS3DSH_DBT_TIM1           0x08   //20 ms
#define STM_LIS3DSH_DBT_TIM2           0x10   //40 ms
#define STM_LIS3DSH_DBT_TIM3           0x1C   //70 ms
#define STM_LIS3DSH_DBT_TIM4           0x58   //220 ms


/**
 *
 */
typedef struct sns_dd_acc_lis3dsh_daf_reverb_v01_s
{
  uint32_t echo;
  uint32_t reverb_period;
} sns_dd_acc_lis3dsh_daf_reverb_v01_s;

/**
 * LIS3DSH Driver Access Item
 */
typedef struct sns_dd_acc_lis3dsh_daf_item
{
  uint32_t    request_id;
  bool        trans_id_valid;
  uint8_t     trans_id;
  void*       conn_handle;

  union
  {
    sns_dd_acc_lis3dsh_daf_reverb_v01_s reverb;

    // put other messages here
  };
} sns_dd_acc_lis3dsh_daf_item_s;

/**
 * Accelerometer LIS3DSH Driver State Information Structure
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
  stm_lis3dsh_sub_dev_s sub_dev[LIS3DSH_MAX_SUB_DEV];

  /**< Current accel sampling frequency. */
  sns_ddf_odr_t acc_cur_rate;

  /**< desired accel sensor sampling frequency. */
  sns_ddf_odr_t acc_desired_rate;

  /**< Current double tap sensor sampling frequency. */
  sns_ddf_odr_t dbt_cur_rate;

  /**< desired double tap sensor sampling frequency. */
  sns_ddf_odr_t dbt_desired_rate;

  /**< Current range selection.*/
  stm_lis3dsh_range range;

  /**< Current sensitivity. */
  stm_lis3dsh_sstvt sstvt_adj;

  /**< Device-to-phone axis mapping. */
  sns_ddf_axes_map_s axes_map;

  /** GPIO number used for interrupts */
  uint32_t gpio_num;

  /** INT GPIO registered or not.*/
  bool int_registered;

  /** LIS3DSH zero-g biases after inertial SW self test and calibration */
  q16_t biases[STM_LIS3DSH_NUM_AXES];

  /** LIS3DSH double tap data */
  sns_ddf_sensor_data_s dbt_data;

  /** LIS3DSH state machines 1 and 2 config data */
  stm_lis3dsh_sm_config sm_config;

  /** Motion detection enabled or not*/
  bool md_enabled;

  /** LIS3DSH fifo data */
  sns_ddf_sensor_data_s fifo_data;

  /** LIS3DSH fifo raw data buffer */
  uint8_t *fifo_raw_buffer;

  /** FIFO enabled or not*/
  bool fifo_enabled;

  /** FIFO and INT enabled or not*/
  bool fifo_int_enabled;

  /** FIFO watermark level*/
  uint16_t fifo_wmk;

  /** last FIFO watermark interrupt timestamp*/
  sns_ddf_time_t last_timestamp;

  /** the time when new ODR is settled */
  sns_ddf_time_t   odr_settled_ts;

  /** timer to generate reverb indications
   *  The reverb timer uses an EM timer instead of a DDF timer because this
   *  driver should be able to service the timer even when there's no power
   *  going to the device. If a DDF timer were used here, the timer could
   *  generate spurious interrupts. */
  sns_em_timer_obj_t  daf_reverb_timer;

  /** this keeps track of the DAF reverb request */
  sns_dd_acc_lis3dsh_daf_item_s  daf_reverb_item;

} sns_dd_acc_lis3dsh_state_t;


#define STM_LIS3DSH_ODR_NUM         9
#define STM_LIS3DSH_MAX_ODR         1600
#define STM_LIS3DSH_DBT_ODR         400
#define STM_LIS3DSH_MAX_FIFO        32

/**
 * Accelerometer LIS3DSH I2C address
 */
#define STM_LIS3DSH_I2C_ADDR        0x1E
#define STM_LIS3DSH_I2C_ADDR_W      (STM_LIS3DSH_I2C_ADDR<<1)
#define STM_LIS3DSH_I2C_ADDR_R      (STM_LIS3DSH_I2C_ADDR_W+1)

/**
 * Accelerometer LIS3DSH register address
 */
#define STM_LIS3DSH_TEMP_OUT        0x0C
#define STM_LIS3DSH_WHO_AM_I        0x0F
#define STM_LIS3DSH_INT_STAT        0x18
#define STM_LIS3DSH_VCF_BASE        0x1B
#define STM_LIS3DSH_CTRL_ODR        0x20
#define STM_LIS3DSH_CTRL_SM1        0x21
#define STM_LIS3DSH_CTRL_SM2        0x22
#define STM_LIS3DSH_CTRL_INT        0x23
#define STM_LIS3DSH_CTRL_FS         0x24
#define STM_LIS3DSH_CTRL_FIFO       0x25
#define STM_LIS3DSH_STATUS_REG      0x27
#define STM_LIS3DSH_OUT_X_L_A       0x28
#define STM_LIS3DSH_OUT_X_H_A       0x29
#define STM_LIS3DSH_OUT_Y_L_A       0x2A
#define STM_LIS3DSH_OUT_Y_H_A       0x2B
#define STM_LIS3DSH_OUT_Z_L_A       0x2C
#define STM_LIS3DSH_OUT_Z_H_A       0x2D
#define STM_LIS3DSH_FIFO_MODE       0x2E
#define STM_LIS3DSH_FIFO_SRC        0x2F
#define STM_LIS3DSH_STM1_X          0x40
#define STM_LIS3DSH_TIM4_1          0x50
#define STM_LIS3DSH_TIM3_1          0x51
#define STM_LIS3DSH_THRS2_STM1      0x56
#define STM_LIS3DSH_THRS1_STM1      0x57
#define STM_LIS3DSH_MASKB_1         0x59
#define STM_LIS3DSH_MASKA_1         0x5A
#define STM_LIS3DSH_SETT1           0x5B
#define STM_LIS3DSH_OUTS1           0x5F
#define STM_LIS3DSH_STM2_X          0x60
#define STM_LIS3DSH_TIM4_2          0x70
#define STM_LIS3DSH_OUTS2           0x7F

/**
 * LIS3DSH resolution
 */
#define STM_LIS3DSH_TEMP_RESLT      FX_FLTTOFIX_Q16(1)

/**
 * Accelerometer LIS3DSH WHO_AM_I register content
 */
#define STM_LIS3DSH_WHOAMI          0x3F
#define STM_LSM330_WHOAMI           0x40

/**
 * Accelerometer LIS3DSH driver LOG PACKAGE support
 */
#define STM_LIS3DSH_LOGDATA         1

/**
 * Accelerometer LIS3DSH SM1 vector filter coefficients
 */
#define STM_LIS3DSH_VFLIT_MASK      0x04
#define STM_LIS3DSH_VFLIT_COEF1     0x35
#define STM_LIS3DSH_VFLIT_COEF2     0x7F
#define STM_LIS3DSH_VFLIT_COEF3     0x7F
#define STM_LIS3DSH_VFLIT_COEF4     0x35

/**
 * Accelerometer LIS3DSH hardware self test settings
 */
#define STM_LIS3DSH_ST_2G_MIN       60      //Unit: mg
#define STM_LIS3DSH_ST_2G_MAX       1700    //Unit: mg

/**
 * Accelerometer LIS3DSH SW inertial self test settings
 */
#define STM_LIS3DSH_SWST_MAX_BIAS   200     //Unit: mg

/**
 * Accelerometer LIS3DSH AXIS and BIAS error codes
 */

#define AXIS_FAIL_X   0x10
#define AXIS_FAIL_Y   0x20
#define AXIS_FAIL_Z   0x40
#define AXIS_FAIL_XY  0x30
#define AXIS_FAIL_XZ  0x50
#define AXIS_FAIL_YZ  0x60
#define AXIS_FAIL_XYZ 0x70

#define BIAS_FAIL_X   0x01
#define BIAS_FAIL_Y   0x02
#define BIAS_FAIL_Z   0x04
#define BIAS_FAIL_XY  0x03
#define BIAS_FAIL_XZ  0x05
#define BIAS_FAIL_YZ  0x06
#define BIAS_FAIL_XYZ 0x07

#endif  /* __LIS3DSH__ */
