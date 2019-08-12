/*===========================================================================

         S E N S O R S    A C C E L E R O M E T E R    D R I V E R

DESCRIPTION

  ADXL350 Accelerometer Sensor Registers address and thier format.


Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_adxl350_priv.h#1 $


when           who     what, where, why
----------     ---     ------------------------------------------------------
12/13/10     AS      Draft
==========================================================================*/

#ifndef _ADXL350_H_
#define _ADXL350_H_

/*
 * ADXL350 Accelerometer Sensor Registers address
 */

#define ADXL350_REG_DEVICE_ID                   0x00

#define ADXL350_REG_TAP_THRESHOLD               0x1D
#define ADXL350_REG_OFFSET_X                    0x1E
#define ADXL350_REG_OFFSET_Y                    0x1F
#define ADXL350_REG_OFFSET_Z                    0x20
#define ADXL350_REG_TAP_DURATION                0x21
#define ADXL350_REG_TAP_LATENCY                 0x22
#define ADXL350_REG_TAP_WINDOW                  0x23
#define ADXL350_REG_ACTIVITY_THRESHOLD          0x24
#define ADXL350_REG_INACTIVITY_THRESHOLD        0x25
#define ADXL350_REG_INACTIVITY_TIME             0x26
#define ADXL350_REG_ACTIVITY_INACTIVITY_CONTROL 0x27
#define ADXL350_REG_FREE_FALL_THRESHOLD         0x28
#define ADXL350_REG_FREE_FALL_TIME              0x29
#define ADXL350_REG_TAP_CONTROL                 0x2A
#define ADXL350_REG_TAP_STATUS                  0x2B
#define ADXL350_REG_RATE_AND_POWER_MODE         0x2C
#define ADXL350_REG_POWER_SAVE                  0x2D
#define ADXL350_REG_INTERRUPT_ENABLE            0x2E
#define ADXL350_REG_INTERRUPT_MAP               0x2F
#define ADXL350_REG_INTERRUPT_SOURCE            0x30
#define ADXL350_REG_DATA_FORMAT                 0x31
#define ADXL350_REG_DATA_X0                     0x32
#define ADXL350_REG_DATA_X1                     0x33
#define ADXL350_REG_DATA_Y0                     0x34
#define ADXL350_REG_DATA_Y1                     0x35
#define ADXL350_REG_DATA_Z0                     0x36
#define ADXL350_REG_DATA_Z1                     0x37
#define ADXL350_REG_FIFO_CONTROL                0x38
#define ADXL350_REG_FIFO_STATUS                 0x39

/*
 * ADXL350 Accelerometer Sensor Registers format
 */

/* ADXL350_REG_DEVICE_ID */
#define ADXL350_DEVICE_ID               0xE5
#define ADXL346_DEVICE_ID               0xE6

/* ADXL350_REG_ACTIVITY_INACTIVITY_CONTROL */
#define ADXL350_INACT_Z_ENABLE          0x01
#define ADXL350_INACT_Y_ENABLE          0x02
#define ADXL350_INACT_X_ENABLE          0x04
#define ADXL350_INACT_ACDC              0x08
#define ADXL350_ACT_Z_ENABLE            0x10
#define ADXL350_ACT_Y_ENABLE            0x20
#define ADXL350_ACT_X_ENABLE            0x40
#define ADXL350_ACT_ACDC                0x80

/* ADXL350_REG_TAP_CONTROL */
#define ADXL350_TAP_Z_ENABLE            0x01
#define ADXL350_TAP_Y_ENABLE            0x02
#define ADXL350_TAP_X_ENABLE            0x04
#define ADXL350_SUPRESS_DOUBLE_TAP      0x08

/* ADXL350_REG_TAP_STATUS */
#define ADXL350_TAP_Z_SOURCE            0x01
#define ADXL350_TAP_Y_SOURCE            0x02
#define ADXL350_TAP_X_SOURCE            0x04
#define ADXL350_ASLEEP                  0x08
#define ADXL350_ACT_Z_SOURCE            0x10
#define ADXL350_ACT_Y_SOURCE            0x20
#define ADXL350_ACT_X_SOURCE            0x40

/* ADXL350_REG_RATE_AND_POWER_MODE */
#define ADXL350_RATE_MASK               0x0F
#define ADXL350_LOW_POWER               0x10

/* ADXL350_REG_POWER_SAVE */
#define ADXL350_SLEEP_FREQ_MASK         0x03

#define ADXL350_POWER_MODE_MASK         0x0C
#define ADXL350_SLEEP                   0x04
#define ADXL350_MEASURE                 0x08 /* 0 = standby mode */
#define ADXL350_AUTO_SLEEP              0x10

#define ADXL350_LINK                    0x20

/* ADXL350_REG_INTERRUPT_ENABLE */
#define ADXL350_INT_ALL_MASK            0xFF

#define ADXL350_INT_OVERRUN             0x01
#define ADXL350_INT_WATERMARK           0x02
#define ADXL350_INT_FREE_FALL           0x04
#define ADXL350_INT_INACTIVITY          0x08
#define ADXL350_INT_ACTIVITY            0x10
#define ADXL350_INT_DOUBLE_TAP          0x20
#define ADXL350_INT_SINGLE_TAP          0x40
#define ADXL350_INT_DATA_READY          0x80

/* ADXL350_REG_DATA_FORMAT */
#define ADXL350_RANGE_MASK              0x03
#define ADXL350_LEFT_JUSTIFY            0x04
#define ADXL350_FULL_RES                0x08
#define ADXL350_INT_INVERT              0x20 /* 0=Active-Hight, 1=Active-Low */
#define ADXL350_SPI                     0x40
#define ADXL350_SELF_TEST               0x80

/* ADXL350_REG_FIFO_CONTROL */
/*
 * Number of samples needed for trigger a watermark interrupt.
 * Depends on the FIFO mode.
 */
#define ADXL350_SAMPLES_NUM_MASK        0x1F

#define ADXL350_TRIGGER_INT_NUM         0x20 /* 0=INT1, 1=INT2 */
#define ADXL350_FIFO_MODE_MASK          0xC0

/* ADXL350_REG_FIFO_STATUS */
#define ADXL350_ENTRIES_NUM_MASK        0x3F
#define ADXL350_FIFO_TRIG               0x80

#include "fixed_point.h"    /* FX_FLTTOFIX_Q16() */

/* Include files required to support accel log packet */
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"

#ifdef CONFIG_USE_ADXL_CALIB
#include "sns_dd_accel_adxl_calib.h"
#endif

/*=======================================================================

                  Preprocessor Definitions and Constants

========================================================================*/
//#define ADXL350_DEBUG 1
//#define ADXL_CHECK_DATA_READY 1

/*
 * Avoid compiler inlining functions,
 * allowing the debugger to break on the function name.
 */
#ifdef ADXL350_DEBUG
    #define static
#endif

#ifdef ADXL350_DEBUG
    #define ADXL350_ERR_MSG(msg) adxl350_err_msg(msg)
#else
    #define ADXL350_ERR_MSG(msg)
#endif

#ifndef ARRAY_SIZE
    #define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif

#ifndef MIN
    #define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef G
    #define G (9.80665) /* accel due to gravity at sea level */
#endif

#define DATA_READY_MAX_RETRY   2

#define BITS_PER_BYTE   8
#define ONE_BYTE 1
#define ENTIRE_REG_MASK 0xFF
#define ENABLE_ALL 0xFF
#define DISABLE_ALL 0x00

#define ADXL350_NUM_AXES     3
#define ADXL350_FIFO_SIZE   0x20
#define ADXL350_MAX_SAMPLES 0x1F

/**
 * I2C slave address.
 */
#define ADXL350_I2C_ADDR      (0x53)
/**
 *  Alternative I2C Address selected via the device pin.
 */
#define ADXL350_I2C_ALT_ADDR  (0x1D)

/*=======================================================================

                   INTERNAL ENUMS

========================================================================*/
/**
 * Accelerometer measurement range.
 */
typedef enum
{
    ADXL350_RANGE_2G  = 0,
    ADXL350_RANGE_4G  = 1,
    ADXL350_RANGE_8G  = 2,
    ADXL350_RANGE_16G = 3,
    ADXL350_NUM_RANGE
} adxl350_range_e ;

typedef enum
{
    ADXL350_DATA_RATE_6_25_HZ = 0, /* 6.25 HZ */
    ADXL350_DATA_RATE_12_5_HZ = 1,
    ADXL350_DATA_RATE_25_HZ   = 2,
    ADXL350_DATA_RATE_50_HZ   = 3,
    ADXL350_DATA_RATE_100_HZ  = 4,
    ADXL350_DATA_RATE_200_HZ  = 5,
    ADXL350_DATA_RATE_400_HZ  = 6,
    ADXL350_DATA_RATE_800_HZ  = 7,
    ADXL350_DATA_RATE_1600_HZ = 8,
    ADXL350_DATA_RATE_3200_HZ = 9,
    ADXL350_NUM_DATA_RATE
} adxl350_data_rate_e ;

typedef enum
{
    ADXL350_POWER_MODE_STANDBY    = 0, /* 0.1 uA */
    ADXL350_POWER_MODE_SLEEP      = 1,
    ADXL350_POWER_MODE_MEASURE    = 2, /* 40-145 uA */
} adxl350_power_mode_e ;

/**
 * Sampling rate when in sleep mode.
 */
typedef enum
{
    ADXL350_SLEEP_8_HZ = 0,
    ADXL350_SLEEP_4_HZ = 1,
    ADXL350_SLEEP_2_HZ = 2,
    ADXL350_SLEEP_1_HZ = 3
} adxl350_sleep_freq_e ;

/**
 * Data FIFO mode.
 */
typedef enum
{
    ADXL350_FIFO_MODE_BYPASS  = 0,
    ADXL350_FIFO_MODE_FIFO    = 1,
    ADXL350_FIFO_MODE_STREAM  = 2,
    ADXL350_FIFO_MODE_TRIGGER = 3
} adxl350_fifo_mode_e ;

/**
 * Interrupt pin ID
 */
typedef enum
{
    ADXL350_INTR1  = 0,
    ADXL350_INTR2  = 1,
    ADXL350_INTR_NUM
} adxl350_intr_id_e ;

/**
 * Number of bits per sample.
 */
typedef enum
{
    ADXL350_FIX_RES_10_BIT   = 0,
    ADXL350_FULL_RES_13_BIT  = 1,
    ADXL350_NUM_RES
} adxl350_resolution_e ;


/**
 *  Driver context.
 *  @i2c_handle - i2c handle used to access the I2C bus.
 *  @smgr_handle - sensor manager handle used to notify_data.
 *  @dev_id - Device Id as reported in the device hardware spec.
 *  @range - current range selection ±2g/±4g/±8g/±16g.
 *  @data_rate - current data rate selection 6.25 HZ .. 3200 HZ.
 *  @power_mode - normal/standby/sleep/auto-sleep.
 *  @sleep_freq - sampling rate in sleep mode 1 HZ .. 8HZ.
 *  @fifo_mode - collecting samples mode.
 *  @resolution - fixed (10 bits) or full (10 to 13 bit) resolution.
 *  @intr_enable - bitmask of interrupt enabled.
 *  @offset - Accelerometer offset, 2's compliment, 15.6 mg/LSB for ±2g range.
 *  @data_raw - last raw sampled data. 3.9 mg/LSB.
 *  @data_cache - last sample data reported.
 *  @retry_delay_usec - delay in usec in case data is not ready.
 */
typedef struct
{
    sns_ddf_handle_t     i2c_handle;
    sns_ddf_handle_t     smgr_handle;

    uint8_t              dev_id;
    adxl350_range_e      range;
    adxl350_data_rate_e  data_rate;
    adxl350_power_mode_e power_mode;
    adxl350_sleep_freq_e sleep_freq;
    adxl350_fifo_mode_e  fifo_mode;
    adxl350_resolution_e resolution;

    uint8_t              intr_enable[ADXL350_INTR_NUM];
    int8_t               offset[ADXL350_NUM_AXES];
    int16_t              data_raw[ADXL350_NUM_AXES];
    q16_t                data_cache[ADXL350_NUM_AXES];
    uint32_t             retry_delay_usec;
#ifdef ADXL350_DEBUG
    uint32_t samples_counter;
    uint32_t retry_counter;
#endif
} adxl350_drv_t;


#endif /* _ADXL350_H_ */



