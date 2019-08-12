/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
 *
 * File Name         : l3gd20_ddf_drv.c
 * Authors           : Jianjian Huo
 * Version           : V 2.2.4
 * Date              : 04/15/2014
 * Description       : L3GD20 gyroscope driver source file
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
 *******************************************************************************/

#ifndef __L3GD20__
#define __L3GD20__

#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd_inertial_test.h"
#include "sns_ddf_signal.h"
#include "fixed_point.h"


#define TICKS_PER_SEC      32768

/**
 * Gyroscope L3GD20 Full Scales in register setting.
 */
typedef enum
{
  STM_L3GD20_RANGE_250DPS   =  0x00,  /*corresponding value in register setting*/
  STM_L3GD20_RANGE_500DPS   =  0x10,
  STM_L3GD20_RANGE_2000DPS  =  0x20
} stm_l3gd20_range;

#define STM_L3GD20_NUM_AXES      3
#define STM_L3GD20_RANGE_NUM     3
#define STM_L3GD20_MAX_RANGE     STM_L3GD20_RANGE_2000DPS

/**
 * Gyroscope L3GD20 min and max of ranges, converted to Q16.
 */
#define STM_L3GD20_RANGE_250_MIN    FX_FLTTOFIX_Q16(-250 * PI /180)
#define STM_L3GD20_RANGE_250_MAX    FX_FLTTOFIX_Q16(250 * PI /180)
#define STM_L3GD20_RANGE_500_MIN    FX_FLTTOFIX_Q16(-500 * PI /180)
#define STM_L3GD20_RANGE_500_MAX    FX_FLTTOFIX_Q16(500 * PI /180)
#define STM_L3GD20_RANGE_2000_MIN   FX_FLTTOFIX_Q16(-2000 * PI /180)
#define STM_L3GD20_RANGE_2000_MAX   FX_FLTTOFIX_Q16(2000 * PI /180)

/**
 * Gyroscope L3GD20 sensitivity
 */
/* value of sensitivity at 250dps, converted to (rad/s)*1000 */
#define STM_L3GD20_SSTVT_250  FX_FLTTOFIX_Q16(8.75*PI/180)
/* value of sensitivity at 500dps, converted to rad/s */
#define STM_L3GD20_SSTVT_500  FX_FLTTOFIX_Q16(17.50*PI/180)
/* value of sensitivity at 2000dps, converted to rad/s */
#define STM_L3GD20_SSTVT_2000 FX_FLTTOFIX_Q16(70.00*PI/180)

/**
 * Gyroscope L3GD20 output data rate
 */
typedef enum
{
  STM_L3GD20_ODR95   =		0x00|0x30,	/* 95Hz output data rate */
  STM_L3GD20_ODR190  =		0x40|0x30,	/* 190Hz output data rate */
  STM_L3GD20_ODR380  =		0x80|0x30,	/* 380Hz output data rate */
  STM_L3GD20_ODR760  =		0xC0|0x30 	/* 760Hz output data rate */
} stm_l3gd20_odr;

/**
 * Gyroscope L3GD20 Driver State Information Structure
 */
typedef struct {
  /**< Handle used with sns_ddf_notify_data. */
  sns_ddf_handle_t smgr_hndl;

  /**< Handle used to access the I2C bus. */
  sns_ddf_handle_t port_handle;

  /**< Timer structure. */
  sns_ddf_timer_s timer;

  /**< Current range selection. */
  stm_l3gd20_range range;

  /**< Current gyro sampling frequency. */
  sns_ddf_odr_t gyr_cur_rate;

  /**< desired gyro sensor sampling frequency. */
  sns_ddf_odr_t gyr_desired_rate;

  /**< the time when new ODR is settled, for polling mode */
  sns_ddf_time_t   odr_settled_ts;

  /**< Current sensitivity for raw data conversion. */
  q16_t gyr_sstvt;

  /**< Current power state: ACTIVE or LOWPOWER */
  sns_ddf_powerstate_e power_state;

  /**< Device-to-phone axis mapping. */
  sns_ddf_axes_map_s axes_map;

  /** GPIO number used for interrupts */
  uint32_t gpio_num;

  /** L3GD20 zero-g biases after inertial SW self test and calibration */
  q16_t biases[STM_L3GD20_NUM_AXES];

  /** L3GD20 fifo data */
  sns_ddf_sensor_data_s fifo_data;

  /** L3GD20 fifo raw data buffer */
  uint8_t *fifo_raw_buffer;

  /** FIFO enabled or not*/
  bool fifo_enabled;

  /** FIFO and INT enabled or not*/
  bool fifo_int_enabled;

  /** FIFO watermark level*/
  uint16_t fifo_wmk;

  /** last FIFO watermark interrupt timestamp*/
  sns_ddf_time_t last_timestamp;
} sns_dd_gyr_l3gd20_state_t;

#define STM_L3GD20_ODR_NUM      4
#define STM_L3GD20_MAX_ODR      760
#define STM_L3GD20_MAX_FIFO     32

/**
 * Gyroscope L3GD20 data bit length
 */
#define STM_GYR_SENSOR_BIT_LEN          16

/**
 * Gyroscope L3GD20 register address
 */
#define STM_L3GD20_WHO_AM_I     0x0F    /* WHO AM I register */
#define STM_L3GD20_CTRL_REG1    0x20    /* Configuration register 1 */
#define STM_L3GD20_CTRL_REG2    0x21    /* Configuration register 2 */
#define STM_L3GD20_CTRL_REG3    0x22    /* Configuration register 3 */
#define STM_L3GD20_CTRL_REG4    0x23    /* Configuration register 4 */
#define STM_L3GD20_CTRL_REG5    0x24    /* Configuration register 5 */
#define STM_L3GD20_REFERENCE    0x25
#define STM_L3GD20_OUT_TEMP     0x26
#define STM_L3GD20_STATUS_REG   0x27
#define STM_L3GD20_OUT_X_L      0x28
#define STM_L3GD20_OUT_X_H      0x29
#define STM_L3GD20_OUT_Y_L      0x2A
#define STM_L3GD20_OUT_Y_H      0x2B
#define STM_L3GD20_OUT_Z_L      0x2C
#define STM_L3GD20_OUT_Z_H      0x2D
#define STM_L3GD20_FIFO_MODE    0x2E
#define STM_L3GD20_FIFO_SRC     0x2F
#define STM_L3GD20_INT1_CFG     0x30
#define STM_L3GD20_INT1_SRC     0x31
#define STM_L3GD20_INT1_THS_XH  0x32
#define STM_L3GD20_INT1_THS_XL  0x33
#define STM_L3GD20_INT1_THS_YH  0x34
#define STM_L3GD20_INT1_THS_YL  0x35
#define STM_L3GD20_INT1_THS_ZH  0x36
#define STM_L3GD20_INT1_THS_ZL  0x37
#define STM_L3GD20_INT1_DRT     0x38

/**
 * Gyroscope L3GD20 self test settings
 */
#define STM_L3GD20_ST_MIN       175*1000  //unit milli-dps.
#define STM_L3GD20_ST_MAX       875*1000

/**
 * Gyroscope L3GD20 SW inertial self test settings
 */
#define STM_L3GD20_SWST_MAX_BIAS   70     //Unit: dps

/**
 * Gyroscope L3GD20 driver LOG PACKAGE support
 */
#define STM_L3GD20_LOGDATA        1

/**
 * Gyroscope L3GD20 AXIS and BIAS error codes
 */

#define AXIS_FAIL_X         0x10
#define AXIS_FAIL_Y         0x20
#define AXIS_FAIL_Z         0x40
#define AXIS_FAIL_XY        0x30
#define AXIS_FAIL_XZ        0x50
#define AXIS_FAIL_YZ        0x60
#define AXIS_FAIL_XYZ       0x70
#define AXIS_SPECIAL_ERROR  0x80

#define BIAS_FAIL_X         0x01
#define BIAS_FAIL_Y         0x02
#define BIAS_FAIL_Z         0x04
#define BIAS_FAIL_XY        0x03
#define BIAS_FAIL_XZ        0x05
#define BIAS_FAIL_YZ        0x06
#define BIAS_FAIL_XYZ       0x07
#define BIAS_SPECIAL_ERROR  0x08

#endif  /* __L3GD20__ */
