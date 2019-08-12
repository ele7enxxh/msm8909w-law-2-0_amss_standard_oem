/*==============================================================================

    S E N S O R S   A C C E L E R O M E T E R    D R I V E R

DESCRIPTION

   Impelements the driver for the ADXL350 accelerometer driver
   Code is based on ADXL350_DS_PrA_v5_23_Sept_2010.pdf data sheet.

Copyright (c) 2010 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_accel_adxl350.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-07-24  vh   Eliminated compiler warnings
  2013-01-09  sd   Removed  SNS_DDF_ASSERT
  2012-07-04  jh   Resolved Klocwork error
  2011-08-28  ag   Added place holder for self test ptr
  2011-07-11  br   changed the location of "#endif" for ADXL_CHECK_DATA_READY
  2011-06-11  br   Changed to honor SENSOR__ALL too in addition to SENSOR_GYRO.
  2011-05-25  br   undefined ADXL_CHECK_DATA_READY for making into a sync driver which assums data is ready always
  2011-04-20  yk   Removed ASSERT macro. Cleaned up edit history.
  2011-04-05  as   Cleanup unused old code for USE_CC41.
  2011-04-04  as   Allow to disable checking data-ready (ADXL_CHECK_DATA_READY).
  2011-03-08  as   Flush stale samples (if any) when enter active state.
  2011-02-22  ky   ADXL calibration
  2011-02-03  br   Modified lines for calculating resolutions
  2011-02-28  sd   Multipliy Accel raw data with right multiplier
  2011-01-26  br   Fixed index in initialization for device_range
  2011-01-20  sd   Changed the  X Y Z orientation
  2011-01-20  br   Updated/Added attributes: RANGE, RESOLUTION_ADC
  2010-12-13  as   Draft
==============================================================================*/

/*============================================================================

                                INCLUDE FILES

============================================================================*/
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd.h"

#include "sns_dd_accel_adxl350_priv.h"


extern sns_ddf_status_e sns_dd_adxl350_get_data(
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s*  data[]);


/*=======================================================================

                    FUNCTION PROTOTYPES

========================================================================*/
sns_ddf_status_e adxl350_log_data(
   sns_ddf_sensor_data_s *data_ptr);

/*=======================================================================

                   INTERNAL FUNCTION PROTOTYPES

========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_init(
    sns_ddf_handle_t*        dd_handle_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e*        sensors[],
    uint32_t*                num_sensors);

static sns_ddf_status_e sns_dd_adxl350_set_attr(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value);

static sns_ddf_status_e sns_dd_adxl350_get_attr(
    sns_ddf_handle_t       dd_handle,
    sns_ddf_sensor_e       sensor,
    sns_ddf_attribute_e    attrib,
    sns_ddf_memhandler_s*  memhandler,
    void**                 value,
    uint32_t*              num_elems);

static void sns_dd_adxl350_handle_timer(
    sns_ddf_handle_t dd_handle,
    void* arg);

static void sns_dd_adxl350_interrupt_handler(
    sns_ddf_handle_t dd_handle,
    uint32_t irq,
    sns_ddf_time_t timestamp);

static sns_ddf_status_e sns_dd_adxl350_reset(sns_ddf_handle_t dd_handle);

/*=======================================================================

                   STRUCTURE DEFINITIONS

========================================================================*/
sns_ddf_driver_if_s sns_accel_adxl350_driver_fn_list =
{
    &sns_dd_adxl350_init,
    &sns_dd_adxl350_get_data,
    &sns_dd_adxl350_set_attr,
    &sns_dd_adxl350_get_attr,
    &sns_dd_adxl350_handle_timer,
    &sns_dd_adxl350_interrupt_handler,
    &sns_dd_adxl350_reset,
    NULL /* self test */
};


/*============================================================================

  Local Data Definitions

 ============================================================================*/

/**
 * Driver state.
 * This variable is minly for debug,
 * since the device registers are accesd via I2C.
 */
static adxl350_drv_t *adxl350_drv_dbg = NULL;

/*
 * Supported frequencies in HZ.
*/
static sns_ddf_lowpass_freq_t adxl350_freq_hz[ADXL350_NUM_DATA_RATE] =
{ 6, 12, 25, 50, 100, 200, 400, 800, 1600, 3200};

/*
 * Look-up table to convert index 0..9 to HW config 0x06..0x0F.
*/
static uint8_t  adxl350_rate_config[ADXL350_NUM_DATA_RATE] =
{
    /* ADXL350_DATA_RATE_6_25_HZ */ 0x06,
    /* ADXL350_DATA_RATE_12_5_HZ */ 0x07,
    /* ADXL350_DATA_RATE_25_HZ   */ 0x08,
    /* ADXL350_DATA_RATE_50_HZ   */ 0x09,
    /* ADXL350_DATA_RATE_100_HZ  */ 0x0A,
    /* ADXL350_DATA_RATE_200_HZ  */ 0x0B,
    /* ADXL350_DATA_RATE_400_HZ  */ 0x0C,
    /* ADXL350_DATA_RATE_800_HZ  */ 0x0D,
    /* ADXL350_DATA_RATE_1600_HZ */ 0x0E,
    /* ADXL350_DATA_RATE_3200_HZ */ 0x0F,
};

/*
 * scale factor for FULL resolution.
 * Maintain 3.9 mg/LSB on all ranges.
 */
static q16_t adxl350_scale_full_res = FX_FLTTOFIX_Q16(16 * G / 4095);


/*###########################################################################*/
/*                    Internal Utilities APIs                                */
/*###########################################################################*/

#ifdef ADXL350_DEBUG
/* Release by debugger to continue debug */
static volatile int adxl350_debug_hold_flag = 1;
static char adxl350_debug_msg[0x20] = {0};
static void adxl350_err_msg(char *msg)
{
    int32 i;
    for (i = 0; (i < sizeof(adxl350_debug_msg)) && (*msg != '\0'); i++)
    {
        adxl350_debug_msg[i] = *msg++;
    }
    adxl350_debug_msg[i] = '\0';

    while (adxl350_debug_hold_flag)
    {
    }
}
#endif /* ADXL350_DEBUG */

/*===========================================================================

  FUNCTION:   find_first_bit

===========================================================================*/
/*!
  @brief Find first bit set to 1, from LSB to MSB.

  @detail
  - It is used for shifting a value to its place on the mask.

  @param[in] mask: bitmask of relenat bit for the field.

  @return first bit number.

  $TODO:
*/
/*=========================================================================*/
static uint8_t find_first_bit(uint8_t mask)
{
    int i;

    for (i = 0; i < BITS_PER_BYTE; i++)
    {
        if (mask & (1<<i))
        {
            break; /* found */
        }
    }

    return i; /* Error */
}

/*===========================================================================

  FUNCTION:   adxl350_read_modify_write_reg

===========================================================================*/
/*!
  @brief Read-Modify-Write to sensor register

  @detail
  - Write to a specific field mask of the sensor register.

  @param[in] drv: Pointer to the driver structure.
  @param[in] reg: register address.
  @param[in] mask: bitmask of the relevant field.
  @param[in] val: field value.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e adxl350_read_modify_write_reg(adxl350_drv_t *drv,
                                          uint8_t reg,
                                          uint8_t mask,
                                          uint8_t val)
{
    sns_ddf_status_e status;
    uint8_t buf = 0;
    uint8_t read_count = 0;
    uint8_t write_count = 0;
    uint8_t shift = 0;

    if (mask == 0)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    if (mask != ENTIRE_REG_MASK)
    {
        shift = find_first_bit(mask);

        status = sns_ddf_read_port(drv->i2c_handle, reg, &buf, ONE_BYTE,
                                   &read_count);

        if (status != SNS_DDF_SUCCESS)
        {
            return status;
        }

        buf &= ~mask;
        buf |= ((val<<shift) & mask);
    }
    else
    {
        buf = val;
    }

    status = sns_ddf_write_port(drv->i2c_handle, reg, &buf, ONE_BYTE,
                                &write_count);

    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_check_device_id

===========================================================================*/
/*!
  @brief Verify the device id

  @detail

  @param[in] drv: Pointer to the driver structure.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_check_device_id(adxl350_drv_t *drv)
{
    sns_ddf_status_e status;
    uint8_t dev_id = 0;
    uint8_t read_count = 0;

    status = sns_ddf_read_port(drv->i2c_handle,
                               ADXL350_REG_DEVICE_ID,
                               &dev_id, ONE_BYTE, &read_count);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    if (
        (dev_id != ADXL350_DEVICE_ID) &&
        (dev_id != ADXL346_DEVICE_ID)
        )
    {
        return SNS_DDF_EDEVICE;
    }

    drv->dev_id = dev_id;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_set_range

===========================================================================*/
/*!
  @brief Sets the ADXL350 acclerometer range.

  @detail
  - The range can be one of the following. ±2g/±4g/±8g/±16g

  @param[in] drv: Pointer to the driver structure.
  @param[in] range: range selected.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_set_range(adxl350_drv_t *drv,
                                                 adxl350_range_e range)
{
    sns_ddf_status_e status;

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_DATA_FORMAT,
                               ADXL350_RANGE_MASK,
                               range);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    drv->range = range;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_set_resolution

===========================================================================*/
/*!
  @brief Sets the ADXL350 acclerometer resolution.

  @detail
  - full resolution - maintaine scale factor of 3.9 mg/LSB for all ranges,
    resolution is 10/11/12/13 bits for ±2g/±4g/±8g/±16g range.
  - fixed resolution - 10 bits for all ranges,
    scale factor vary from 3.9 mg/LSB to 15.6 mg/LSB.

  @param[in] drv: Pointer to the driver structure.
  @param[in] resolution: resolution selected.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_set_resolution(
    adxl350_drv_t *drv,
    adxl350_resolution_e resolution)
{
    sns_ddf_status_e status;

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_DATA_FORMAT,
                               ADXL350_FULL_RES,
                               resolution);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    drv->resolution = resolution;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_set_data_rate

===========================================================================*/
/*!
  @brief Sets the ADXL350 acclerometer rate.

  @detail
  - The data rate. range from 6.25 HZ to 3200 HZ.

  @param[in] drv: Pointer to the driver structure.
  @param[in] data_rate: selected data_rate.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_set_data_rate(
    adxl350_drv_t *drv,
    adxl350_data_rate_e data_rate)
{
    sns_ddf_status_e status;
    uint8_t val = adxl350_rate_config[data_rate];

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_RATE_AND_POWER_MODE,
                               ADXL350_RATE_MASK,
                               val);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    drv->data_rate = data_rate;
    drv->retry_delay_usec = 1000 * 1000 / adxl350_freq_hz[drv->data_rate];

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_set_power_mode

===========================================================================*/
/*!
  @brief Sets the ADXL350 acclerometer power mode.

  @detail
  - Set the sensor in power saving mode or active mode.

  @param[in] drv: Pointer to the driver structure.
  @param[in] power_mode: mode selected.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_set_power_mode(
    adxl350_drv_t *drv,
    adxl350_power_mode_e power_mode)
{
    sns_ddf_status_e status;

    /* Only 2 power modes are currently supported by DDF. */
    if  ((power_mode != ADXL350_POWER_MODE_STANDBY) &&
         (power_mode != ADXL350_POWER_MODE_MEASURE))

    {
        return SNS_DDF_EINVALID_PARAM;
    }

    /* Power mode is controlled by 2 registers */
    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_RATE_AND_POWER_MODE,
                               ADXL350_LOW_POWER,
                               0); /* Disabled */

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_POWER_SAVE,
                               ADXL350_POWER_MODE_MASK,
                               power_mode);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    drv->power_mode = power_mode;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_set_fifo_mode

===========================================================================*/
/*!
  @brief Sets the ADXL350 acclerometer FIFO mode.

  @detail
  - FIFO Modes:
  ADXL350_FIFO_MODE_BYPASS  - FIFO not in use.
    Read one sample from DATAX/DATAY/DATAZ registers.
  ADXL350_FIFO_MODE_FIFO    - Up to 32 samples are collected,
    until FIFO is full.
  ADXL350_FIFO_MODE_STREAM  - Up to 32 samples are collected,
    Old samples are overwritten.
  ADXL350_FIFO_MODE_TRIGGER - Up to 32 samples are collected,
    Old samples are held, new samples are added until the FIFO is full.

  @param[in] drv: Pointer to the driver structure.
  @param[in] fifo_mode: collecting samples mode.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_set_fifo_mode(
    adxl350_drv_t *drv,
    adxl350_fifo_mode_e fifo_mode)
{
    sns_ddf_status_e status;
    uint8_t num_samples;

    /*
     * Note: Undesirable operation may occur if a value of 0 is used for the
     * samples bits when trigger mode is used.
     */
    if ( fifo_mode == ADXL350_FIFO_MODE_BYPASS)
    {
        num_samples = 0;
    }
    else
    {
        num_samples = ADXL350_MAX_SAMPLES;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_FIFO_CONTROL,
                               ADXL350_SAMPLES_NUM_MASK,
                               num_samples);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_FIFO_CONTROL,
                               ADXL350_FIFO_MODE_MASK,
                               fifo_mode);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    drv->fifo_mode = fifo_mode;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_set_offset

===========================================================================*/
/*!
  @brief Sets the ADXL350 acclerometer X/Y/Z offset.

  @detail
  - Sensor calibration for X/Y/Z bias.

  @param[in] drv: Pointer to the driver structure.
  @param[in] offset_x: X Offset.
  @param[in] offset_y: Y Offset.
  @param[in] offset_z: Z Offset.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_set_offset(
    adxl350_drv_t *drv,
    int8_t offset_x,
    int8_t offset_y,
    int8_t offset_z)
{
    sns_ddf_status_e status;

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_OFFSET_X,
                               ENTIRE_REG_MASK,
                               offset_x);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_OFFSET_Y,
                               ENTIRE_REG_MASK,
                               offset_y);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_OFFSET_Z,
                               ENTIRE_REG_MASK,
                               offset_z);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    drv->offset[0] = offset_x;
    drv->offset[1] = offset_y;
    drv->offset[2] = offset_z;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_enable_interrupts

===========================================================================*/
/*!
  @brief Enable interrupts.

  @detail
  - There are 2 output lines INT1 and INT2.
    Each line can be configured to required events:
      1. overrun
      2. watermark
      3. free_fall
      4. inactivity
      5. activity
      6. double_tap
      7. single_tap
      8. data_ready

  @param[in] drv: Pointer to the driver structure.
  @param[in] intr_id: interrupt line ID.
  @param[in] intr_mask: bitmask of the required events. See ADXL350_INT_***.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_enable_interrupts(
    adxl350_drv_t *drv,
    adxl350_intr_id_e intr_id,
    uint8_t intr_mask)
{
    sns_ddf_status_e status;
    uint8_t val = 0;

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_INTERRUPT_ENABLE,
                               ADXL350_INT_ALL_MASK,
                               intr_mask);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    if (intr_id == ADXL350_INTR1)
    {
        val = 0x00; /* INTR1 */
    }
    else
    {
        val = 0xFF; /* INTR2 */
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_INTERRUPT_MAP,
                               intr_mask,
                               val);


    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    drv->intr_enable[intr_id] |= intr_mask;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_disable_interrupts

===========================================================================*/
/*!
  @brief Disable interrupts.

  @detail

  @param[in] drv: Pointer to the driver structure.
  @param[in] bitmask: bitmask of the required events. See ADXL350_INT_***.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_disable_interrupts(
    adxl350_drv_t *drv,
    adxl350_intr_id_e intr_id,
    uint8_t bitmask)
{
    sns_ddf_status_e status;

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_INTERRUPT_ENABLE,
                               bitmask,
                               0x00);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    drv->intr_enable[intr_id] &= ~bitmask;

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_set_self_test

===========================================================================*/
/*!
  @brief Enable/Disable Self-Test mode.

  @detail

  @param[in] drv: Pointer to the driver structure.
  @param[in] enable: 1=enbale,0=disable.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_set_self_test(adxl350_drv_t *drv,
                                                     int enable)
{
    sns_ddf_status_e status;

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_DATA_FORMAT,
                               ADXL350_SELF_TEST,
                               enable);

    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_config_tap_detection

===========================================================================*/
/*!
  @brief Config single-tap/double-tap detection.

  @detail

  @param[in] drv: Pointer to the driver structure.
  @param[in] threshold:
  @param[in] duartion:
  @param[in] latency:
  @param[in] window:
  @param[in] supress_double_tap: Only single-tap detection.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_config_tap_detection(
    adxl350_drv_t *drv,
    uint8_t threshold,
    uint8_t duartion,
    uint8_t latency,
    uint8_t window,
    int supress_double_tap)
{
    sns_ddf_status_e status;
    uint8_t axes_mask = ADXL350_TAP_X_ENABLE |
                        ADXL350_TAP_Y_ENABLE |
                        ADXL350_TAP_Z_ENABLE;


    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_TAP_THRESHOLD,
                               ENTIRE_REG_MASK,
                               threshold);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_TAP_DURATION,
                               ENTIRE_REG_MASK,
                               duartion);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_TAP_LATENCY,
                               ENTIRE_REG_MASK,
                               latency);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_TAP_WINDOW,
                               ENTIRE_REG_MASK,
                               window);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_TAP_CONTROL,
                               axes_mask,
                               ENABLE_ALL);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_TAP_CONTROL,
                               ADXL350_SUPRESS_DOUBLE_TAP,
                               supress_double_tap);

    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_config_free_fall_detection

===========================================================================*/
/*!
  @brief Config free fall detection

  @detail

  @param[in] drv: Pointer to the driver structure.
  @param[in] threshold:
  @param[in] time:

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_config_free_fall_detection(
    adxl350_drv_t *drv,
    uint8_t threshold,
    uint8_t time)
{
    sns_ddf_status_e status;

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_FREE_FALL_THRESHOLD,
                               ENTIRE_REG_MASK,
                               threshold);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_FREE_FALL_TIME,
                               ENTIRE_REG_MASK,
                               time);

    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_config_activity_detection

===========================================================================*/
/*!
  @brief Config activity detection

  @detail

  @param[in] drv: Pointer to the driver structure.
  @param[in] threshold:

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_config_activity_detection(
    adxl350_drv_t *drv,
    uint8_t threshold)
{
    sns_ddf_status_e status;
    uint8_t axes_mask = ADXL350_TAP_X_ENABLE |
                        ADXL350_TAP_Y_ENABLE |
                        ADXL350_TAP_Z_ENABLE;


    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_ACTIVITY_THRESHOLD,
                               ENTIRE_REG_MASK,
                               threshold);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_ACTIVITY_INACTIVITY_CONTROL,
                               (axes_mask<<4),
                               ENABLE_ALL);

    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_config_inactivity_detection

===========================================================================*/
/*!
  @brief Config inactivity detection

  @detail

  @param[in] drv: Pointer to the driver structure.
  @param[in] threshold:
  @param[in] time:

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_config_inactivity_detection(
    adxl350_drv_t *drv,
    uint8_t threshold,
    uint8_t time)
{
    sns_ddf_status_e status;
    uint8_t axes_mask = ADXL350_TAP_X_ENABLE |
                        ADXL350_TAP_Y_ENABLE |
                        ADXL350_TAP_Z_ENABLE;


    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_INACTIVITY_THRESHOLD,
                               ENTIRE_REG_MASK,
                               threshold);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_INACTIVITY_TIME,
                               ENTIRE_REG_MASK,
                               time);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_ACTIVITY_INACTIVITY_CONTROL,
                               axes_mask,
                               ENABLE_ALL);

    return status;
}

/*===========================================================================

  FUNCTION:   adxl350_log_data

===========================================================================*/
/*!
  @brief log data

  @detail

  @param[in] data_ptr: pointer to the data structure.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
sns_ddf_status_e adxl350_log_data(sns_ddf_sensor_data_s *data_ptr)
{
    sns_err_code_e status;
    sns_log_sensor_data_pkt_s* log_ptr = NULL;
    int num_samples = 0;
    int i;

    /* Allocate log packet */
    status = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                               sizeof(sns_log_sensor_data_pkt_s),
                               (void**)&log_ptr);

    if (status != SNS_SUCCESS)
        return SNS_DDF_ENOMEM;

    log_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_ptr->sensor_id = SNS_DDF_SENSOR_ACCEL;
    log_ptr->vendor_id = SNS_DDF_VENDOR_ADI;

    /* Timestamp the log with sample time */
    log_ptr->timestamp = data_ptr->timestamp;

    /* Log the sensor data */
    num_samples = MIN(data_ptr->num_samples,
                      SNS_LOG_MAX_DATA_TYPES_PER_SENSOR);
    num_samples = (num_samples / ADXL350_NUM_AXES) * ADXL350_NUM_AXES;

    log_ptr->num_data_types = num_samples;

    for (i = 0; i < num_samples; i++)
    {
        log_ptr->data[i]  = data_ptr->samples[i].sample;
    }

    /* Commit log (also frees up the log packet memory) */
    status = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                               log_ptr);
    if (status)
    {
        return SNS_DDF_EFAIL;
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_flush_stale_samples

===========================================================================*/
/*!
  @brief Flush stale samples.

  @detail   when exist from standby to measurement mode
            there might be stale samples left.

  @param[in] drv: Pointer to the driver structure.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_flush_stale_samples(adxl350_drv_t *drv)
{
    sns_ddf_status_e status;

    uint8_t intr_status = 0; /* updated regaldless if interrupts are enabled */
    uint8_t read_count = 0;
    int16_t data_raw[ADXL350_NUM_AXES];

    do
    {
        /* Check if data ready */
        status = sns_ddf_read_port(drv->i2c_handle,
                                   ADXL350_REG_INTERRUPT_SOURCE,
                                   &intr_status, ONE_BYTE, &read_count);

        if (status != SNS_DDF_SUCCESS)
        {
            ADXL350_ERR_MSG("fail to read intr_status");
            return status;
        }

        if (intr_status & ADXL350_INT_DATA_READY)
        {
            /* Read all samples in one burst , FIFO in bypass mode. */
            status = sns_ddf_read_port(drv->i2c_handle,
                                       ADXL350_REG_DATA_X0,
                                       (uint8_t*) data_raw, sizeof(data_raw),
                                       &read_count);

            if (status != SNS_DDF_SUCCESS)
            {
                ADXL350_ERR_MSG("fail to read samples");
                return status;
            }
        }

    } while (intr_status & ADXL350_INT_DATA_READY);

    return SNS_DDF_SUCCESS;
}

/*###########################################################################
  *                    DDF APIs
 ###########################################################################*/

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_reset

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were
         in after init() was called.

  @detail
  - resets the offset to zero & range to 2G which
    is stored in the driver drv structure

  @param[in] drv: pointer to the driver structure

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO: Self-Test.
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_reset(sns_ddf_handle_t dd_handle)
{
    sns_ddf_status_e status;
    adxl350_drv_t *drv = (adxl350_drv_t *)dd_handle;

    status = sns_dd_adxl350_disable_interrupts(drv,
                                               ADXL350_INTR1,
                                               ADXL350_INT_ALL_MASK);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = sns_dd_adxl350_set_range(drv, ADXL350_RANGE_16G);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    /* Setting Full resolution maintain 3.9 mg/LSB. */
    status = sns_dd_adxl350_set_resolution(drv, ADXL350_FULL_RES_13_BIT);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = sns_dd_adxl350_set_data_rate(drv, ADXL350_DATA_RATE_3200_HZ);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = sns_dd_adxl350_set_power_mode(drv, ADXL350_POWER_MODE_MEASURE);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    status = sns_dd_adxl350_set_fifo_mode(drv, ADXL350_FIFO_MODE_BYPASS);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    /* Note: This should be changed once NV items are supported. */
    status = sns_dd_adxl350_set_offset(drv, 0, 0, 0);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    /* Set right justify with sign extension. */
    status = adxl350_read_modify_write_reg(drv,
                               ADXL350_REG_DATA_FORMAT,
                               ADXL350_LEFT_JUSTIFY,
                               0);


    return SNS_DDF_SUCCESS;

}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_init

===========================================================================*/
/*!
  @brief Initializes the 3 axis Bosch accelerometer ADXL350
              determines the device to use

  @detail
  - Allocates memory for driver drv structure.
  Opens the device port by calling sns_ddf_open_port routine
  Calls sns_dd_adxl350_reset routine

  @param[out] dd_handle_ptr  Pointer that this function must malloc and
                             populate. This is a handle to the driver
                             instance that will be passed in to all
                             other functions.
  @param[in]  smgr_handle    Handle used to identify this driver when it
                             calls into Sensors Manager functions.
  @param[in]  nv_params      NV parameters retrieved for the driver.
  @param[in]  device_info    Information describing each of the physical
                             devices controlled by this driver. This is
                             used to configure the bus and talk to the
                             device.
  @param[in]  memhandler     Memory handler used to dynamically allocate
                             output parameters, if applicable. NB: Do
                             not use memhandler to allocate memory for
                             @a dd_handle_ptr.
  @param[in]  num_devices    Length of @a device_info.
  @param[out] sensors        List of supported sensors, populated and
                              returned by this function.
  @param[out] num_sensors    Length of @a sensors.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_init(
    sns_ddf_handle_t*        dd_handle_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e*        sensors[],
    uint32_t*                num_sensors)
{
    sns_ddf_status_e status;
    adxl350_drv_t *drv;

    status = sns_ddf_malloc((void **)&drv, sizeof(adxl350_drv_t));

    if (status != SNS_DDF_SUCCESS)
    {
        return SNS_DDF_ENOMEM;
    }
    memset(drv, 0, sizeof(adxl350_drv_t));

    drv->smgr_handle = smgr_handle;

    status = sns_ddf_open_port(&(drv->i2c_handle),
                               &(device_info->port_config));

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    /* Fill out supported sensor info */
    *num_sensors = 1;
    *sensors[0] = SNS_DDF_SENSOR_ACCEL;
    *dd_handle_ptr = (sns_ddf_handle_t)drv;
    adxl350_drv_dbg = drv; /* debug */

    /* Verify access to the device */
    status = sns_dd_adxl350_check_device_id(drv);

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    /* Set the sensor to default setup */
    status = sns_dd_adxl350_reset(drv);

    if (status != SNS_DDF_SUCCESS)
    {

        return status;
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_set_attr

===========================================================================*/
/*!
  @brief Sets an attribute of the adxl350 accelerometer

  @detail
  - Called by SMGR to set certain device attributes that are
    programmable. Curently its the power mode and range.

  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib      Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO: DDF should add more attributes to support new features of the device.
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_adxl350_set_attr(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value_ptr)
{
    adxl350_drv_t *drv = (adxl350_drv_t *)dd_handle;
    sns_ddf_status_e status = SNS_DDF_EINVALID_PARAM;

    if ((sensor != SNS_DDF_SENSOR_ACCEL) && (sensor != SNS_DDF_SENSOR__ALL))
    {
        ADXL350_ERR_MSG("invalid sensor");
        return SNS_DDF_EINVALID_PARAM;
    }

    switch (attrib)
    {
    case SNS_DDF_ATTRIB_POWER_STATE:
        {
            sns_ddf_powerstate_e power_state = *((sns_ddf_powerstate_e*) value_ptr);

            switch (power_state)
            {
            case SNS_DDF_POWERSTATE_ACTIVE:
                if (drv->power_mode == ADXL350_POWER_MODE_STANDBY)
                {
                    /* Flush stale samples (if any) before taking new samples. */
                    status = sns_dd_adxl350_flush_stale_samples(drv);
                    if (status != SNS_DDF_SUCCESS)
                    {
                        return status;
                    }
                }

                status = sns_dd_adxl350_set_power_mode(drv,
                                                       ADXL350_POWER_MODE_MEASURE);
                break;
            case SNS_DDF_POWERSTATE_LOWPOWER:
                status = sns_dd_adxl350_set_power_mode(drv,
                                                       ADXL350_POWER_MODE_STANDBY);
                break;
            default:
                ADXL350_ERR_MSG("invalid power state");
                return SNS_DDF_EINVALID_PARAM;
            }
        }
        break;
    case SNS_DDF_ATTRIB_RANGE:
        /* Support only 16G range
        {
            adxl350_range_e range = *(adxl350_range_e*) value_ptr;

            if (range >= ADXL350_NUM_RANGE)
            {
                ADXL350_ERR_MSG("invalid range");
                return SNS_DDF_EINVALID_PARAM;
            }

            // Note: The value is an index and not in resoulution of G

            status = sns_dd_adxl350_set_range(drv, range);
        }
        */
        break;

    case SNS_DDF_ATTRIB_LOWPASS:
        {
            adxl350_data_rate_e data_rate = *(adxl350_data_rate_e*) value_ptr;

            if (data_rate >= ADXL350_NUM_DATA_RATE)
            {
                ADXL350_ERR_MSG("invalid bandwidth");
                return SNS_DDF_EINVALID_PARAM;
            }

            /* Note: The value is an index and not in resoulution of HZ. */

            status = sns_dd_adxl350_set_data_rate(drv, data_rate);
        }
        break;

    case SNS_DDF_ATTRIB_RESOLUTION:
        /* Support only FULL resolution
        {
            adxl350_resolution_e resolution = *(adxl350_resolution_e*) value_ptr;

            if (resolution >= ADXL350_NUM_RES)
            {
                ADXL350_ERR_MSG("invalid resolution");
                return SNS_DDF_EINVALID_PARAM;
            }

            // Note: The value is an index and not in resoulution of bits.

            status = sns_dd_adxl350_set_resolution(drv, resolution);
        }
        */
        break;

    default:
        ADXL350_ERR_MSG("invalid attrib");
        return SNS_DDF_EINVALID_ATTR;
    }

    return(status);
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_get_attr

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  - range and resolution info is from the device data sheet.

  @param[in]  dd_handle   Handle to a driver instance.
  @param[in] sensor       Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved.
  @param[in] memhandler  Memory handler used to dynamically allocate
                         output parameters, if applicable.
  @param[out] value      Pointer that this function will allocate or set
                         to the attribute's value.
  @param[out] num_elems  Number of elements in @a value.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO:
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_adxl350_get_attr(
    sns_ddf_handle_t       dd_handle,
    sns_ddf_sensor_e       sensor,
    sns_ddf_attribute_e    attrib,
    sns_ddf_memhandler_s*  memhandler,
    void**                 value,
    uint32_t*              num_elems)
{
    adxl350_drv_t *drv = (adxl350_drv_t *)dd_handle;

    if ((sensor != SNS_DDF_SENSOR_ACCEL) && (sensor != SNS_DDF_SENSOR__ALL))
    {
        ADXL350_ERR_MSG("invalid sensor");
        return SNS_DDF_EINVALID_PARAM;
    }

    switch (attrib)
    {
    case SNS_DDF_ATTRIB_POWER_INFO:
        {
            sns_ddf_power_info_s* power_attrib;
            power_attrib = sns_ddf_memhandler_malloc(memhandler,
                                                     sizeof(sns_ddf_power_info_s));
            if (power_attrib == NULL)
            {
                ADXL350_ERR_MSG("malloc failure power_attrib");
                return SNS_DDF_ENOMEM;
            }
            /* 145 uA in measurement mode rate > 100 HZ */
            power_attrib->active_current = 145;

            /* 1 uA in Sleep mode */
            power_attrib->lowpower_current = 1;
            *num_elems = 1;
            *value = power_attrib;
        }
        break;

    case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *device_ranges;

            device_ranges = sns_ddf_memhandler_malloc(memhandler,
                                                      ADXL350_NUM_RANGE*sizeof(sns_ddf_range_s));

            if (device_ranges == NULL)
            {
                ADXL350_ERR_MSG("malloc failure device_ranges");
                return SNS_DDF_ENOMEM;
            }

            *num_elems = 1; /* the range is fixed as ±16g */
            device_ranges[0].min = FX_FLTTOFIX_Q16(-16 * G);
            device_ranges[0].max = FX_FLTTOFIX_Q16(16 * G);

            *value = device_ranges;
        }
        break;

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            sns_ddf_resolution_adc_s* resp;
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
            {
                 return SNS_DDF_ENOMEM;
            }
            *num_elems = 1;
            resp->bit_len = (uint32_t) 13;
            resp->max_freq = 200;
            *value = resp;
        }
        break;

    case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t *device_res;
            int res_num = 1; /* Only Full resolution supported */

            device_res = sns_ddf_memhandler_malloc(memhandler,
                                                   res_num*sizeof(sns_ddf_resolution_t));

            if (device_res == NULL)
            {
                ADXL350_ERR_MSG("malloc failure device_res");
                return SNS_DDF_ENOMEM;
            }

            *num_elems = res_num;

            /* Full resolution - maximum 13-bit on ±16g range. */
            device_res[0] = adxl350_scale_full_res;

            *value = device_res;
        }
        break;

    case SNS_DDF_ATTRIB_LOWPASS:
        {
            uint8_t i;
            sns_ddf_lowpass_freq_t *freq_set;
            int num = ARRAY_SIZE(adxl350_freq_hz);

            freq_set = sns_ddf_memhandler_malloc(memhandler,
                                                 num*sizeof(sns_ddf_lowpass_freq_t));

            if (freq_set == NULL)
            {
                ADXL350_ERR_MSG("malloc failure freq_set");
                return SNS_DDF_ENOMEM;
            }

            for (i=0; i<num; i++)
            {
                freq_set[i] = adxl350_freq_hz[i];
            }

            *num_elems = num;
            *value = freq_set;
        }
        break;

    case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s *device_delay;
            device_delay = sns_ddf_memhandler_malloc(memhandler,
                                                     sizeof(sns_ddf_delays_s));
            if (device_delay == NULL)
            {
                ADXL350_ERR_MSG("malloc failure device_delay");
                return SNS_DDF_ENOMEM;
            }
            /*
             * Wakeup-to-ready time.
             * Turn-On and Wake-up times are 1.1 milisec + 1/Freq.
             * For 100 HZ it is 11.1 miliseconds.
             */
            device_delay->time_to_active = 1100; /* 1.1 milisec */
            /*
             * Command-to-data time.
             * Samples are updated according to the data rate.
             * If stream mode is used, then there are allways the latest samples.
             */
            device_delay->time_to_data = 0;

            *value = device_delay;
            *num_elems = 1;
        }
        break;

    case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s *driver_info;
            driver_info = sns_ddf_memhandler_malloc(memhandler,
                                                    sizeof(sns_ddf_driver_info_s));
            if (driver_info == NULL)
            {
                ADXL350_ERR_MSG("malloc failure driver_info");
                return SNS_DDF_ENOMEM;
            }

            if (drv->dev_id == ADXL350_DEVICE_ID)
            {
                driver_info->name = "ADXL350";
            }
            else if (drv->dev_id == ADXL346_DEVICE_ID)
            {
                driver_info->name = "ADXL346";
            }
            else
            {
                driver_info->name = "ADXL_INVALID";
            }
            driver_info->version = 1;

            *num_elems = 1;
            *value = driver_info;
        }
        break;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
        {
            sns_ddf_device_info_s *device_info;
            device_info = sns_ddf_memhandler_malloc(memhandler,
                                                    sizeof(sns_ddf_device_info_s));
            if (device_info == NULL)
            {
                ADXL350_ERR_MSG("malloc failure device_info");
                return SNS_DDF_ENOMEM;
            }

            if (drv->dev_id == ADXL350_DEVICE_ID)
            {
                device_info->model = "ADXL350";
                device_info->name = "Accelerometer";
                device_info->version = 350;
            }
            else if (drv->dev_id == ADXL346_DEVICE_ID)
            {
                device_info->model = "ADXL346";
                device_info->name = "Accelerometer";
                device_info->version = 346;
            }
            else
            {
                device_info->model = "ADXL_INVALID";
                device_info->name = "ADXL_INVALID";
                device_info->version = -1;
            }

            device_info->vendor = "Analog Devices";

            *value = device_info;
            *num_elems = 1;
        }
        break;

    default:
        ADXL350_ERR_MSG("invalid attrib");
        return SNS_DDF_EINVALID_ATTR;
    }

    return SNS_DDF_SUCCESS;
}


/*===========================================================================

  FUNCTION:   sns_dd_adxl350_interrupt_handler

===========================================================================*/
/*!
  @brief ADXL350 interrupt handler

  @detail

  @param[in] drv: pointer to the driver structure
  @param[in] irq: irq number
  @param[in] timestamp: time stamp

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success.

  $TODO: Sensor interrupt line (MSM GPIO) is not routed to the SIC
         (the ARM7 Interrupt controller).
*/
/*=========================================================================*/
static void sns_dd_adxl350_interrupt_handler(
    sns_ddf_handle_t dd_handle,
    uint32_t irq,
    sns_ddf_time_t timestamp)
{
    ; /* Shouldn't happen */
}

/*===========================================================================

  FUNCTION:   sns_dd_adxl350_handle_timer

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired. This is
         the callback function submitted when initializing a timer.

  @detail
  This will be called within the context of the Sensors Manager task.
  It indicates that sensor data is ready

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] arg        The argument submitted when the timer was set.

  @return None
*/
/*=========================================================================*/
static void sns_dd_adxl350_handle_timer(sns_ddf_handle_t dd_handle, void* arg)
{
   ;/* Shouldn't happen */
}

/*
 * Dummy
 */
void avoid_warnings_for_unused_func(void)
{
    sns_dd_adxl350_enable_interrupts(NULL,ADXL350_INTR1,0);
    sns_dd_adxl350_set_self_test(NULL,0);
    sns_dd_adxl350_config_tap_detection(NULL,0,0,0,0,0);
    sns_dd_adxl350_config_free_fall_detection(NULL,0,0);
    sns_dd_adxl350_config_activity_detection(NULL,0);
    sns_dd_adxl350_config_inactivity_detection(NULL, 0, 0);
    adxl350_drv_dbg->data_cache[0] = 0;
}

/*===========================================================================

FUNCTION:   sns_dd_adxl350_calibrate

===========================================================================*/
/*!
@brief  Calibrate the AD ADXL accelerometer

@detail
1.  This function is the ADXL calibrate entry point.
2.  It converts the internal representation of the offset bias
to a more general, sensor-related data (sns_ddf_sensor_data_s)
3.  For further details see Calibration application-note

@attention Calibration should be performed while the module is positioned
    in a known orientation that is provided to this function, in order
    to compensate for the natural gravity acceleration.
@attention The module must remain stationary during calibration, or else
    calibration might fail.

@param[in] dd_handle          Handle to a driver instance.
@param[in] orientation        The orientation of the device.
indicates the axis on which the 1-g is acting
see also Table 1 in the Calibration application-note
(reference values for all orientations)
@param[in] max_tries          Maximal number of iterations of the calibration loop.
@param[in] write_to_eeprom_b  adxl has no eeprom, must be FALSE
@param[in] memhandler         Memory handler used to dynamically allocate
output parameters, if applicable.
@param[out] sensor_data       Sampled sensor data. The number of elements
must match @a num_sensors.

@return
SNS_DDF_SUCCESS         success
SNS_DDF_EFAIL           sensor calibration failed
SNS_DDF_ENOMEM          no memory

$TODO:
*/
/*=========================================================================*/

#ifdef CONFIG_USE_ADXL_CALIB
static sns_ddf_status_e sns_dd_adxl350_calibrate
(
 sns_ddf_handle_t           dd_handle,
 acc_orientation_e          orientation,
 uint32_t                   max_tries,
 boolean                    write_to_eeprom_b,
 sns_ddf_memhandler_s*      memhandler,
 sns_ddf_sensor_data_s**    sensor_data
 )
{
    adxl350_drv_t*  state = (adxl350_drv_t *)dd_handle;
    sns_ddf_status_e            stat;
    sns_ddf_sensor_data_s*      data_ptr;
    OffsetDataType              stOffset;

    if (sensor_data)
    {
        *sensor_data    = NULL;
    }

    if (TRUE == write_to_eeprom_b)
    {
        // no eeprom for adxl
        return SNS_DDF_EFAIL;
    }

    stat = sns_dd_adxl350_calibrate_internal(dd_handle,
        state->i2c_handle,
        orientation,
        max_tries,
        &stOffset);
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }

    state->offset[0] = (int8_t)(stOffset.i8X);
    state->offset[1] = (int8_t)(stOffset.i8Y);
    state->offset[2] = (int8_t)(stOffset.i8Z);

    /* raise bias offset data */
    if (sensor_data && memhandler)
    {
        if( (data_ptr = sns_ddf_memhandler_malloc(memhandler,
            sizeof(sns_ddf_sensor_data_s))) == NULL)
        {
            return SNS_DDF_ENOMEM;
        }

        *sensor_data = data_ptr;

        data_ptr->sensor = SNS_DDF_SENSOR_ACCEL;
        data_ptr->status = SNS_DDF_SUCCESS;
        data_ptr->timestamp = sns_ddf_get_timestamp();

        if( (data_ptr->samples = sns_ddf_memhandler_malloc(memhandler,
            ADXL350_NUM_AXES*sizeof(sns_ddf_sensor_sample_s))) == NULL)
        {
            return SNS_DDF_ENOMEM;
        }

        data_ptr->samples[0].sample = stOffset.i8X;
        data_ptr->samples[0].status = SNS_DDF_SUCCESS;

        data_ptr->samples[1].sample  = stOffset.i8Y;
        data_ptr->samples[1].status = SNS_DDF_SUCCESS;

        data_ptr->samples[2].sample  = stOffset.i8Z;
        data_ptr->samples[2].status = SNS_DDF_SUCCESS;

        data_ptr->num_samples = ADXL350_NUM_AXES;

    }

    return SNS_DDF_SUCCESS;
}
#endif // CONFIG_USE_ADXL_CALIB
