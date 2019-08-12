/*  Date: 2013/08/05 10:00:00
 *  Revision: 1.0
 */

/*==============================================================================

    S E N S O R S   ACCELEROMETER  D R I V E R

DESCRIPTION

   Implements the driver for the MMA8452 accelerometer Sensor

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_mma8452.h#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
08/09/13     yh      Initial version of driver for mma8452
==============================================================================*/

#ifndef __MMA8452__
#define __MMA8452__

#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_util.h"


/**
 * Accelerometer MMA8452 Full Scales in register setting.
 */
typedef enum
{
    MMA8452_RANGE_2G   = 0x00,  /*corresponding value in register setting*/
    MMA8452_RANGE_4G   = 0x01,
    MMA8452_RANGE_8G   = 0x02,
} mma8452_range;

#define MMA8452_NUM_AXES   3
#define MMA8452_RANGE_NUM  3
#define MMA8452_MAX_RANGE  MMA8452_RANGE_8G

/**
 * Accelerometer MMA8452 ranges, converted to Q16.
 */
#define MMA8452_RANGE_2G_MIN    FX_FLTTOFIX_Q16(-2*G)
#define MMA8452_RANGE_2G_MAX    FX_FLTTOFIX_Q16(2*G)
#define MMA8452_RANGE_4G_MIN    FX_FLTTOFIX_Q16(-4*G)
#define MMA8452_RANGE_4G_MAX    FX_FLTTOFIX_Q16(4*G)
#define MMA8452_RANGE_8G_MIN    FX_FLTTOFIX_Q16(-8*G)
#define MMA8452_RANGE_8G_MAX    FX_FLTTOFIX_Q16(8*G)


/**
 * Accelerometer MMA8452 sensitivity for each range.
 */
 typedef enum
{
    MMA8452_SSTVT_2G  = 2,
    MMA8452_SSTVT_4G  = 4,
    MMA8452_SSTVT_8G  = 8,
}mma8452_sstvt;

/**
 * Accelerometer MMA8452 output data rate in register setting
 */
 typedef enum
{
    MMA8452_ODR1_65     = 0x07,  /* 1.65Hz output data rate */
    MMA8452_ODR6_25     = 0x06,  /* 6.25Hz output data rate */
    MMA8452_ODR12_5     = 0x05,  /* 12.5Hz output data rate */
    MMA8452_ODR50      = 0x04,  /* 50Hz output data rate */
    MMA8452_ODR100    = 0x03,  /* 100Hz output data rate */
    MMA8452_ODR200    = 0x02,  /* 200Hz output data rate */
    MMA8452_ODR400    = 0x01,  /* 400Hz output data rate */
    MMA8452_ODR800    = 0x00   /* 800Hz output data rate */
} mma8452_odr;


/**
 * Accelerometer MMA8452 Driver State Information Structure
 */
typedef struct {
    /**< Handle used with sns_ddf_notify_data. */
    sns_ddf_handle_t smgr_hndl;

    /**< Handle used to access the I2C bus. */
    sns_ddf_handle_t port_handle;

    /**< Current range selection.*/
    mma8452_range range;

    /**< Current sampling frequency. */
    sns_ddf_odr_t acc_cur_rate;

    mma8452_odr data_rate;

    bool motion_detect_enabled;
    bool int_enable;

    /**< Current sensitivity. */
    mma8452_sstvt sstvt_adj;

    /**< Current power state: ACTIVE or LOWPOWER */
    sns_ddf_powerstate_e power_state;

    /**< bias setting. */
    q16_t bias[MMA8452_NUM_AXES];

    /**< data cache for storing sensor data. */
    q16_t data_cache[MMA8452_NUM_AXES];

    /**< flag for sensor data out of bound. */
    uint8_t out_of_bound_flag;

    /**< Device-to-phone axis mapping. */
    sns_ddf_axes_map_s axes_map;

    /** GPIO number used for interrupts */
    uint32_t gpio_num;

} sns_dd_acc_mma8452_state_t;


#define MMA8452_NUM_READ_BYTES  6
#define MMA8452_ODR_NUM         8
#define MMA8452_MAX_ODR         800
#define MMA8452_MAX_ODR_RIGVAL  MMA8452_ODR1_65


/**
 * Accelerometer MMA8452 register address
 */
#define  MMA8452_STATUS        0x00
#define  MMA8452_OUT_X_MSB        0x01
#define  MMA8452_OUT_X_LSB        0x02   
#define  MMA8452_OUT_Y_MSB        0x03
#define  MMA8452_OUT_Y_LSB        0x04
#define  MMA8452_OUT_Z_MSB        0x05
#define  MMA8452_OUT_Z_LSB        0x06

#define  MMA8452_F_SETUP        0x09
#define  MMA8452_TRIG_CFG        0x0A
#define  MMA8452_SYSMOD        0x0B
#define  MMA8452_INT_SOURCE        0x0C
#define  MMA8452_WHO_AM_I        0x0D
#define  MMA8452_XYZ_DATA_CFG        0x0E
#define  MMA8452_HP_FILTER_CUTOFF        0x0F

#define  MMA8452_PL_STATUS        0x10
#define  MMA8452_PL_CFG        0x11
#define  MMA8452_PL_COUNT        0x12
#define  MMA8452_PL_BF_ZCOMP        0x13
#define  MMA8452_P_L_THS_REG        0x14

#define  MMA8452_FF_MT_CFG        0x15
#define  MMA8452_FF_MT_SRC        0x16
#define  MMA8452_FF_MT_THS        0x17
#define  MMA8452_FF_MT_COUNT        0x18

#define  MMA8452_TRANSIENT_CFG        0x1D
#define  MMA8452_TRANSIENT_SRC        0x1E
#define  MMA8452_TRANSIENT_THS        0x1F
#define  MMA8452_TRANSIENT_COUNT        0x20

#define  MMA8452_PULSE_CFG        0x21
#define  MMA8452_PULSE_SRC        0x22
#define  MMA8452_PULSE_THSX        0x23
#define  MMA8452_PULSE_THSY        0x24
#define  MMA8452_PULSE_THSZ        0x25
#define  MMA8452_PULSE_TMLT        0x26
#define  MMA8452_PULSE_LTCY        0x27
#define  MMA8452_PULSE_WIND        0x28

#define  MMA8452_ASLP_COUNT        0x29
#define  MMA8452_CTRL_REG1        0x2A
#define  MMA8452_CTRL_REG2        0x2B
#define  MMA8452_CTRL_REG3        0x2C
#define  MMA8452_CTRL_REG4        0x2D
#define  MMA8452_CTRL_REG5        0x2E

#define  MMA8452_OFF_X        0x2F
#define  MMA8452_OFF_Y        0x30
#define  MMA8452_OFF_Z        0x31

#define  MMA8452_REG_END        0x32


/**
 * Accelerometer MMA8452 register config
 */
#define CHIP_ID_MMA8452        0x2A
#define MMA8452_STANDBY        0xfe
#define MMA8452_ACTIVE        0x01
#define MMA8452_ODR_CFG        0xC7
#define MMA8452_RANGE_CFG        0xFC
#define MMA8452_SRC_DRDY        0x01
#define MMA8452_SRC_TRANS        0x20
#define MMA8452_EA        0x40
#define MMA8452_TRANSIENT_ENABLE        0x1E
#define MMA8452_TRANSIENT_THRESHOLD        0x01
#define MMA8452_TRANSIENT_INT        0x20

#endif  /* __MMA8452__ */
