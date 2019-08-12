/*==============================================================================

    S E N S O R S   ACCELEROMETER  D R I V E R

DESCRIPTION

   Implements the driver for the MC3410 accelerometer Sensor

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_mc3410_priv.h#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
11/29/13     yh      Initial version of driver for mc3410
==============================================================================*/

#ifndef __MC3410__
#define __MC3410__

#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_util.h"

/**
 * Accelerometer MC3410 ranges, converted to Q16.
 */
#define MC3410_RANGE_2G_MIN    FX_FLTTOFIX_Q16(-2*G)
#define MC3410_RANGE_2G_MAX    FX_FLTTOFIX_Q16(2*G)
#define MC3410_RANGE_4G_MIN    FX_FLTTOFIX_Q16(-4*G)
#define MC3410_RANGE_4G_MAX    FX_FLTTOFIX_Q16(4*G)
#define MC3410_RANGE_8G_MIN    FX_FLTTOFIX_Q16(-8*G)
#define MC3410_RANGE_8G_MAX    FX_FLTTOFIX_Q16(8*G)

/**
 * Accelerometer MC3410 parameter config
 */
#define MC3410_NUM_AXES          3
#define MC3410_RANGE_NUM         3
#define MC3410_NUM_READ_BYTES    6
#define MC3410_ODR_NUM           7
#define MC3410_LOWPW_C           3
#define MC3410_BIT_LEN           10
#define MC3410_ACTIVE_C          150
#define MC3410_MAX_FREQ          256
#define MC3410_MAX_ODR           1024
#define MC3410                   0xA8
#define MC3410_DATAR_MASK        0x8F
#define MC3410_RANGE_MASK        0xF3
#define MC3410_BIT_MASK          0x03
#define MC3410_STDBY_MASK        0x03
#define MC3410_INTEN_MASK        0x10
#define MC3410_INTCON_MASK       0x80
#define MC3410_ACTIVE_MODE       0x41
#define MC3410_STANDBY_MODE      0x43
#define MC3410_MAX_RANGE         0x02
#define MC3410_MAXODR_INDEX      0x06
#define MC3410_SHAKE_INT         0xE0
#define MC3410_ADJACENT_SK       0x03
#define MC3410_SK_THRESHOLD		 0x00

/**
 *  mc3410_register_table
 */
#define MC3410_TILT              0x03
#define MC3410_OPSTAT            0x04
#define MC3410_INTEN             0x06
#define MC3410_MODE              0x07
#define MC3410_TAPEN             0x09
#define MC3410_TAPP              0x0A
#define MC3410_DROP              0x0B
#define MC3410_SHDB              0x0C
#define MC3410_XOUT_EX_L         0x0D
#define MC3410_XOUT_EX_H         0x0E
#define MC3410_YOUT_EX_L         0x0F
#define MC3410_YOUT_EX_H         0x10
#define MC3410_ZOUT_EX_L         0x11
#define MC3410_ZOUT_EX_H         0x12
#define MC3410_CHIP_ID           0x18
#define MC3410_OUTCFG            0x20
#define MC3410_XOFFL             0x21
#define MC3410_XOFFH             0x22
#define MC3410_YOFFL             0x23
#define MC3410_YOFFH             0x24
#define MC3410_ZOFFL             0x25
#define MC3410_ZOFFH             0x26
#define MC3410_XGAIN             0x27
#define MC3410_YGAIN             0x28
#define MC3410_ZGAIN             0x29
#define MC3410_SHAKE_TH          0x2B
#define MC3410_UD_Z_TH           0x2C
#define MC3410_UD_X_TH           0x2D
#define MC3410_RL_Z_TH           0x2E
#define MC3410_RL_Y_TH           0x2F
#define MC3410_FB_Z_TH           0x30
#define MC3410_DROP_TH           0x31
#define MC3410_TAP_TH            0x32
#define MC3410_PCODE             0x3B

/**
 * Accelerometer MC3410 Full Scales in register setting.
 */
typedef enum
{
    MC3410_RANGE_2G   = 0x00,  /*corresponding value in register setting*/
    MC3410_RANGE_4G   = 0x01,
    MC3410_RANGE_8G   = 0x02,
} mc3410_range;

/**
 * Accelerometer MC3410 sensitivity for each range.
 */
 typedef enum
{
    MC3410_SSTVT_2G  = 2,
    MC3410_SSTVT_4G  = 4,
    MC3410_SSTVT_8G  = 8,
}mc3410_sstvt;

/**
 * Accelerometer MC3410 output data rate in register setting
 */
 typedef enum
{
    MC3410_ODR16     = 0x06,  /* 6.25Hz output data rate */
    MC3410_ODR32     = 0x05,  /* 12.5Hz output data rate */
    MC3410_ODR64     = 0x04,  /* 50Hz output data rate */
    MC3410_ODR128    = 0x03,  /* 100Hz output data rate */
    MC3410_ODR256    = 0x02,  /* 200Hz output data rate */
    MC3410_ODR512    = 0x01,  /* 400Hz output data rate */
    MC3410_ODR1024   = 0x00   /* 800Hz output data rate */
} mc3410_odr;

/**
 * Accelerometer MC3410 Driver State Information Structure
 */
typedef struct {
    /**< Handle used with sns_ddf_notify_data. */
    sns_ddf_handle_t smgr_hndl;

    /**< Handle used to access the I2C bus. */
    sns_ddf_handle_t port_handle;

    /**< Current range selection.*/
    mc3410_range range;

    /**< Current sampling frequency. */
    sns_ddf_odr_t acc_cur_rate;

    mc3410_odr data_rate;

    bool motion_detect_enabled;
    bool int_enable;

    /**< Current sensitivity. */
    mc3410_sstvt sstvt_adj;

    /**< Current power state: ACTIVE or LOWPOWER */
    sns_ddf_powerstate_e power_state;

    /**< bias setting. */
    q16_t bias[MC3410_NUM_AXES];

    /**< data cache for storing sensor data. */
    q16_t data_cache[MC3410_NUM_AXES];

    /**< flag for sensor data out of bound. */
    uint8_t out_of_bound_flag;
	uint8_t e_i;
	int16_t data_cache_old[MC3410_NUM_AXES];
    /**< Device-to-phone axis mapping. */
    sns_ddf_axes_map_s axes_map;

    /** GPIO number used for interrupts */
    uint32_t gpio_num;

} sns_dd_acc_mc3410_state_t;

#endif  /* __MC3410__ */
