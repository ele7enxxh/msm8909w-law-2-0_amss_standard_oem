/**
 * @file sns_dd_mpu6050_ext.c
 * @brief MPU-6xxx driver, supporting gyro, accel, DMP and temperature.
 *
 * Copyright (c) 2010-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mpu6050_ext.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-07-09 AH   Fix compiler warnings
  2013-08-23 vh   Eliminated Klocwork warnings
  2013-07-17 vh   Eliminated compiler warnings
  2013-06-21 pn   Added support for SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST
  2013-05-22 lka  Added support for both MPU6050 and MPU6500.
                  Added support for DMP.
                  Renamed functions with 'mpu6xxx' prefix.
  2013-05-01 pn   Indentation cleanup
  2013-05-01 dc   Revert the delay added in DD init as the delay is handled
                  in a generic way in SMGR.
  2013-04-25 dc   Added an initial delay of 10ms needed to init MPU6500.
  2013-04-23 br   Moved initializing variables from init() to reset_device()
  2013-04-03 pn   Added support for MPU6500
  2013-03-18 vh   Added error check for functions
  2012-09-13 vh   Eliminated compiler warnings
  2012-05-16 br   fixed bug for getting ODR value
  2012-03-13 sd   Added cycle mode support
  2012-03-05 sd   Added DRI support
  2012-02-29 sd   Added FEATURE_MPU6050_INT_PIN_CONFIG  to disable MPU3050 int
  2012-01-31 ag   Validate I2C communication during gyro init
  2011-12-22 sd   Modified comment
  2011-12-07 sd   Adjust the sensitivity according to the product revision
  2011-11-16 yk   Added support for axis mapping.
  2011-11-14 br   Changed max freq of temp from 10 to 200
  2011-10-10 ag   Initialized state variable before using
  2011-10-09 ag   Added two different smgr handles for accel and gyro
  2011-10-06 yk   Updated power info to be per-sensor. Fixed accel range attrib
  2011-10-06 yk   Corrected init to return the proper handle for the 2nd instance
  2011-04-04 yk   Added support for per-sensor reset and power state changes.
  2010-08-26 yk   Initial revision
==============================================================================*/

#include "sns_dd_mpu6050_priv.h"

#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_util.h"
#ifdef FEATURE_MPU6050_INT_PIN_CONFIG
#include "sns_dd_gyro_priv.h"
#endif

#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_log_types.h"
#include "sns_log_api.h"

// Macro for printing out NV param error messages
#define MPU6050_MSG_NV_PARAM_ERROR(param,value,upper_lim,lower_lim)   MPU6050_MSG_3(ERROR, "Invalid " param " value of %u. The upper limit is %u and the lower limit is %d (inclusive)", value, upper_lim, lower_lim)

extern volatile boolean EnableI2C;

extern sns_ddf_driver_if_s sns_dd_mpu6050_if;

static mpu6xxx_head_s mpu6xxx_head = {
  NULL,
  {"Qualcomm MPU6XXX Driver", 1}
};

// Accelerometer range values
static sns_ddf_range_s sns_dd_mpu6050_accel_range[] =
{
   { FX_FLTTOFIX_Q16(-2 * G),  FX_FLTTOFIX_Q16(2 * G)  },
   { FX_FLTTOFIX_Q16(-4 * G),  FX_FLTTOFIX_Q16(4 * G)  },
   { FX_FLTTOFIX_Q16(-8 * G),  FX_FLTTOFIX_Q16(8 * G)  },
   { FX_FLTTOFIX_Q16(-16 * G), FX_FLTTOFIX_Q16(16 * G) }
};

// Gyroscope range values
static sns_ddf_range_s sns_dd_mpu6050_gyro_range[] =
{
   { MPU6050_DPS_TO_RPSQ16(-250),  MPU6050_DPS_TO_RPSQ16(250)  },
   { MPU6050_DPS_TO_RPSQ16(-500),  MPU6050_DPS_TO_RPSQ16(500)  },
   { MPU6050_DPS_TO_RPSQ16(-1000), MPU6050_DPS_TO_RPSQ16(1000) },
   { MPU6050_DPS_TO_RPSQ16(-2000), MPU6050_DPS_TO_RPSQ16(2000) }
};

// Temperature range values
static sns_ddf_range_s sns_dd_mpu6050_temp_range =
{
    FX_FLTTOFIX_Q16(-40.0), FX_FLTTOFIX_Q16(85.0)
};

// Gyroscope low pass filter bandwidth values for MPU6050
q16_t sns_dd_mpu6050_gyro_lpf[] =
{
   FX_FLTTOFIX_Q16(5),
   FX_FLTTOFIX_Q16(10),
   FX_FLTTOFIX_Q16(20),
   FX_FLTTOFIX_Q16(42),
   FX_FLTTOFIX_Q16(98),
   FX_FLTTOFIX_Q16(188),
   FX_FLTTOFIX_Q16(256),
};

// Accelerometer low pass filter bandwidth values for MPU6050
static q16_t sns_dd_mpu6050_accel_lpf[] =
{
   FX_FLTTOFIX_Q16(5),
   FX_FLTTOFIX_Q16(10),
   FX_FLTTOFIX_Q16(21),
   FX_FLTTOFIX_Q16(44),
   FX_FLTTOFIX_Q16(94),
   FX_FLTTOFIX_Q16(184),
   FX_FLTTOFIX_Q16(260)
};

// Gyroscope low pass filter bandwidth values for MPU6500
q16_t sns_dd_mpu6500_gyro_lpf[] =
{
   FX_FLTTOFIX_Q16(5),
   FX_FLTTOFIX_Q16(10),
   FX_FLTTOFIX_Q16(20),
   FX_FLTTOFIX_Q16(41),
   FX_FLTTOFIX_Q16(92),
   FX_FLTTOFIX_Q16(184),
   FX_FLTTOFIX_Q16(250),
   FX_FLTTOFIX_Q16(3600)
};

// Accelerometer low pass filter bandwidth values for MPU6500
static q16_t sns_dd_mpu6500_accel_lpf[] =
{
   FX_FLTTOFIX_Q16(5),
   FX_FLTTOFIX_Q16(10),
   FX_FLTTOFIX_Q16(20),
   FX_FLTTOFIX_Q16(41),
   FX_FLTTOFIX_Q16(92),
   FX_FLTTOFIX_Q16(184),
   FX_FLTTOFIX_Q16(460)
};

// Supported sensor types
static sns_ddf_sensor_e sns_dd_mpu6xxx_sensors[] =
{
   SNS_DDF_SENSOR_ACCEL,
   SNS_DDF_SENSOR_GYRO,
   SNS_DDF_SENSOR_TEMP,
   SNS_DDF_SENSOR_DOUBLETAP
};

static sns_ddf_sensor_e mpu6050_first_sensor[] =
{
  SNS_DDF_SENSOR_ACCEL
};

/**
 * @brief Adjust the sensitivity according to the product revision
 * @
 */
sns_ddf_status_e sns_dd_mpu6xxx_sensitivity_adjust(sns_dd_mpu6xxx_s* state)
{
  uint8_t buffer = 0;
  uint8_t bytes_read, product_rev=0;
  sns_ddf_status_e status;

  status = sns_ddf_read_port(
        state->port_handle,
        MPU6050_REG_PRODUCT_ID,
        &buffer,
        1,
        &bytes_read);
  if(status != SNS_DDF_SUCCESS)
  {
     return status;
  }
  product_rev = buffer & PRODUCT_REV_MASK;
  if( (state->device_id == DEVICE_ID_MPU6500) ||
      ((product_rev == REV_C_1) || (product_rev == REV_C_2)) )
  {
     /* revision C*/
     uint8_t range_indx =0, max_indx=0;
     max_indx = (uint8_t)(MPU6050_TABLE_SIZE(sns_dd_mpu6050_accel_range));
     for(range_indx=0; range_indx<max_indx;  range_indx++ )
     {
        sns_dd_mpu6050_accel_sensitivity[range_indx]= sns_dd_mpu6050_accel_sensitivity[range_indx]*2;
     }
  }

  sns_ddf_write_port_byte(
        state->port_handle,
        MPU6050_REG_INT_PIN_CFG,
        INT_LEVEL_HIGH | LATCH_INT_EN_PULSE | INT_RD_CLEAR_ON_ANY_REG,
        &status);

  return status;
}


/**
 * @brief Validates device ID
 *
 * @param[in] state  Ptr to the driver structure
 *
 * @return Success status of this operation.
 */
sns_ddf_status_e sns_dd_mpu6050_validate_device_id(
   sns_dd_mpu6xxx_s* state)
{
    uint8_t buffer = 0;
    uint8_t bytes_read;
    sns_ddf_status_e status;
    sns_dd_mpu6xxx_device_id_e dev_id;

    if (EnableI2C == FALSE)
        return SNS_DDF_SUCCESS;

    status = sns_ddf_read_port(
        state->port_handle,
        MPU6XXX_REG_WHO_AM_I,
        &buffer,
        1,
        &bytes_read);

    if(status != SNS_DDF_SUCCESS)
       return status;

    dev_id = (sns_dd_mpu6xxx_device_id_e) buffer;
    if (dev_id == DEVICE_ID_MPU6050)
    {
      state->device_id = DEVICE_ID_MPU6050;
    }
    else if (dev_id == DEVICE_ID_MPU6500)
    {
      state->device_id = DEVICE_ID_MPU6500;
    }
    // TODO: ADD SUPPORT FOR NEW DEVICE ID HERE
    else
    {
      state->device_id = DEVICE_ID_INVALID;
      return SNS_DDF_EDEVICE;
    }

    return SNS_DDF_SUCCESS;
}

/**
 * @brief Writes to an advanced hardware (DMP) register
 *
 * Burst write data to the DMP with sizes of up to 256 bytes at a time.
 *
 * @param[in]  state          Pointer to the MPU6500 struct
 * @param[in]  address        Sixteen-bit destination address
 * @param[in]  value          Pointer to an array of values to write to @address
 * @param[in]  size           The number of elements in @value
 *
 * @return Success if @value is correctly written to @address
 */
sns_ddf_status_e sns_dd_mpu6xxx_write_dmp_register(sns_dd_mpu6xxx_s * state,
    uint16_t address, const uint8_t * value, uint16_t size)
{
  uint16_t n = 0;
  uint16_t curr_address;
  uint8_t address_bytes[2];
  uint8_t bytes_written, burst_size;
  uint16_t burst_limit;
  uint8_t * burst_buffer;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  MPU6050_MSG_3(LOW, "mpu6xxx write dmp register @ addr: %x  1st value: %x  size: %d", address, *value, size);

#ifdef MPU6050_DEBUG
  uint8_t burst_buffer_READ_BACK[MPU6500_MAX_BURST_WRITE_SIZE];
  uint8_t burst_buffer_READ_BACK_1byte = 0;
  uint8_t bytes_read;
  uint8_t buffer_iter;
#endif

  while (n < size)
  {
    // Pointer to the beginning of the next set of data
    burst_buffer = (uint8_t *) (value + n);
    /* Burst-write the address to DMP_CTRL_1 and DMP_CTRL_2 */
    curr_address = address + n;
    address_bytes[0] = MPU6XXX_HIGH_BYTE(curr_address);
    address_bytes[1] = MPU6XXX_LOW_BYTE(curr_address);
    status = sns_ddf_write_port(
      state->port_handle,
      MPU6500_REG_DMP_CTRL_1,
      address_bytes,
      2,
      &bytes_written);
    if (status != SNS_DDF_SUCCESS)
      return status;

    /* Burst-write a bank (up to 256 bytes) of data to DMP_CTRL_3 */

    // Determine the max allowed burst-size for this particular write-cycle
    // Note: The MPU6xxx is limited in its burst-write size by the
    //       boundaries of its memory banks (size of 256)
    burst_limit = MPU6500_MAX_BURST_WRITE_SIZE - (curr_address & 0xFF);
    // If burst_limit is 0x100, then limit it to 0xFF (the maximum write-size defined by sns_ddf_write_port)
    if (burst_limit > 0xFF)
      burst_limit = 0xFF; // Set to the maximum number of bytes allowed for a burst write

    if ((n + burst_limit) > size)
    {
      burst_size = (uint8_t) size - n;
    }
    else
    {
      burst_size = (uint8_t) burst_limit;
    }

    /* Write the value(s) to the DMP_CTRL_3 */
    // Burst-write @burst_size bytes starting at the address @burst_buffer
    status = sns_ddf_write_port(
      state->port_handle,
      MPU6500_REG_DMP_CTRL_3,
      burst_buffer,
      burst_size,
      &bytes_written);
    if (status != SNS_DDF_SUCCESS)
      return status;

#ifdef MPU6050_DEBUG
    /* DMP Read-Back for Verification */

    // Re-write the starting address
    status = sns_ddf_write_port(
      state->port_handle,
      MPU6500_REG_DMP_CTRL_1,
      address_bytes,
      2,
      &bytes_read);
    if (status != SNS_DDF_SUCCESS)
      return status;

    // Burst-read the portion of the DMP image that was just written
    status = sns_ddf_read_port(
      state->port_handle,
      MPU6500_REG_DMP_CTRL_3,
      burst_buffer_READ_BACK, //&burst_buffer_READ_BACK_1byte,
      burst_size,
      &bytes_read);
    if (status != SNS_DDF_SUCCESS)
      return status;

    // Iterate through what was read-back and compare it to what was supposed to be written
    for (buffer_iter = 0; buffer_iter < burst_size; buffer_iter++)
    {
      // If there is a mismatch, set status to FAILED
      if (burst_buffer_READ_BACK[buffer_iter] != burst_buffer[buffer_iter])
      {
        MPU6050_MSG_3(ERROR, "DMP register read-back ERROR at %d, desired: %x, read-back: %x", n + buffer_iter,burst_buffer[buffer_iter],burst_buffer_READ_BACK[buffer_iter]);
        status = SNS_DDF_EFAIL;
      }
    }
#endif

    n += bytes_written;  // @bytes_written should equal @burst_size
  }

  return status;
}


/**
 * @brief Loads the DMP firmware with the given byte-array (firmware image)
 *
 *  "Starting with the byte[0] of data of the firmware image, perform the
 *  following for each byte[N] of data:
 *  - Write the value (N >> 8) to DMP_CTRL_1 (0x6D)
 *  - Write the value (N & 0xFF) to the DMP_CTRL_2 (0x6E)
 *  - Write the Nth byte of data to DMP_CTRL_3 (0x6F)
 *  Repeat the steps above for each byte of the DMP firmware image."
 *
 *  For more information regarding burst-writing see below.
 *  Source: Section 7.7 of the MPU-6500 Register Map and Definitions
 *
 * @param[in]  state          Pointer to the MPU6XXX device driver
 * @param[in]  image          Pointer to the DMP image byte-array
 * @param[in]  image_size     The number of elements in @image
 *
 * @return Success if @a dd_handle_ptr was allocated and the driver was
 *         configured properly. Otherwise a specific error code is returned.
 */
sns_ddf_status_e sns_dd_mpu6xxx_load_dmp_firmware(sns_dd_mpu6xxx_s * state,
    const unsigned char * image, uint16_t image_size)
{
  sns_ddf_status_e status;
  // Burst-write the DMP image
  MPU6050_MSG_2(LOW, "%u Loading DMP firmware...  size: %d", 22222222, image_size);
  status = sns_dd_mpu6xxx_write_dmp_register(state, 0, (uint8_t*) image, image_size);
  if (SNS_DDF_SUCCESS == status)
  {
    MPU6050_MSG_2(LOW, "%u Loaded %d bytes of the MPU6XXX firmware.", 22222223, image_size);
  }
  return status;
}

#ifdef FEATURE_MPU6050_INT_PIN_CONFIG

#define PART_NUM_OFFSET 4
#define MPU3050_PART_NUM 3
#define MPU3050_I2C_ADDR 0x68
#define MPU3050_I2C_ADDR_MASK 0x6E
/**
 * @brief disable MPU3050 INT since it shares int pin with MPU6050
 * @
 */
sns_ddf_status_e sns_dd_mpu3050_int_disable()
{
  sns_ddf_status_e status;
  uint8_t data, out, read_count;
  sns_dd_gyro3050_state_s* ptr;
  sns_ddf_i2c_config_s    i2c_config;
  sns_ddf_device_access_s device;

  device.device_select = 0;
  device.port_config.bus = SNS_DDF_BUS_I2C;
  device.port_config.bus_config.i2c  = &i2c_config;
  i2c_config.addr_type = SNS_DDF_I2C_ADDR_7BIT;
  i2c_config.bus_acq_timeout = -1;
  i2c_config.bus_freq = 400;
  i2c_config.dev_type = SNS_DDF_I2C_DEVICE_REGADDR;
  i2c_config.read_opt = SNS_DDF_I2C_START_BEFORE_RD;
  i2c_config.slave_addr = MPU3050_I2C_ADDR;
  i2c_config.xfer_timeout = -1;

  status = sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_gyro3050_state_s));
  if (status != SNS_DDF_SUCCESS)
  {
    return status;
  }
  status = sns_ddf_open_port(&(ptr->port_handle),
                              &(device.port_config));

  if (status != SNS_DDF_SUCCESS)
  {
    sns_ddf_mfree(ptr);
    return status;
  }

  status = sns_ddf_read_port(ptr->port_handle,
                                   GYRO_3050_REG_PRODUCT_ID,
                                   &data, 1, &read_count);
  if( (data>>PART_NUM_OFFSET) != MPU3050_PART_NUM)
  {
     /* no MPU 3050, return */
     sns_ddf_close_port(ptr->port_handle);
     sns_ddf_mfree(ptr);
     return SNS_DDF_SUCCESS;
  }

  status = sns_ddf_read_port(ptr->port_handle,
                                   GYRO_3050_REG_I2C_ADDR,
                                   &data, 1, &read_count);
  if ( (data & MPU3050_I2C_ADDR_MASK) != MPU3050_I2C_ADDR )
  {
     /* no MPU 3050, return */
     sns_ddf_close_port(ptr->port_handle);
     sns_ddf_mfree(ptr);
     return SNS_DDF_SUCCESS;
  }

  /* configure interrupt */
  data =  GYRO_3050_BIT_ACTL_LOW | GYRO_3050_BIT_OPEN_DRAIN ;

  status = sns_ddf_write_port(ptr->port_handle,
                               GYRO_3050_REG_INT_CFG,
                               &data,
                               1,
                               &out);

  sns_ddf_close_port(ptr->port_handle);
  sns_ddf_mfree(ptr);
  if (status != SNS_DDF_SUCCESS)
  {
     return status;
  }

  return SNS_DDF_SUCCESS;
}
#endif

/**
 *
 */
sns_dd_mpu6xxx_s* mpu6xxx_find_same_port_dev (sns_ddf_port_config_s *port_config_ptr)
{
  sns_dd_mpu6xxx_s* cur_dd_dev_ptr = (sns_dd_mpu6xxx_s*) mpu6xxx_head.next;

  while (cur_dd_dev_ptr)
  {
    if (true == sns_ddf_is_same_port_id(port_config_ptr, &cur_dd_dev_ptr->port_id))
    {
      return cur_dd_dev_ptr;
    }
    else
    {
      cur_dd_dev_ptr = cur_dd_dev_ptr->next;
    }
  }
  return NULL;
}

/**
 *
 */
uint32_t mpu6xxx_fill_sub_dev_slot(
    sns_dd_mpu6xxx_s*       state,
    mpu6xxx_sub_dev_s*      sub_dev_slot,
    sns_ddf_handle_t        smgr_handle,
    uint32_t                gpio,
    uint32_t                num_sensors,
    sns_ddf_sensor_e        sensors[],
    sns_ddf_sensor_e*       sensors_init[])
{
  uint32_t num_sensors_filled = 0, i;

  // Check for null-pointers
  if ((NULL == state) || (NULL == sub_dev_slot) || (NULL == sensors_init))
    return 0;

  // Check bounds
  if (num_sensors > MPU6XXX_MAX_NUM_SENSORS)
    return 0;

  for (i = 0; i < num_sensors; i++)
  {
    // If the device is an MPU6050, only support accel, gyro, and temp
    if (((sensors[i] == SNS_DDF_SENSOR_ACCEL) || (sensors[i] == SNS_DDF_SENSOR_GYRO) || (sensors[i] == SNS_DDF_SENSOR_TEMP))
        && (state->device_id == DEVICE_ID_MPU6050))
    {
      MPU6050_MSG_2(LOW, "mpu6xxx_fill_sub_dev_slot: adding sensor type %d under SMGR handle 0x%X", sensors[i], smgr_handle);
      sub_dev_slot->sensors[num_sensors_filled++].sensor_type = sensors[i];
      (*sensors_init)[i] = sensors[i];
    }
    // Else, if the device is an MPU6500, only support accel, gyro, temp, and doubletap
    else if (((sensors[i] == SNS_DDF_SENSOR_ACCEL) || (sensors[i] == SNS_DDF_SENSOR_GYRO) || (sensors[i] == SNS_DDF_SENSOR_TEMP) || (sensors[i] == SNS_DDF_SENSOR_DOUBLETAP))
        && (state->device_id == DEVICE_ID_MPU6500))
    {
      MPU6050_MSG_2(LOW, "mpu6xxx_fill_sub_dev_slot: adding sensor type %d under SMGR handle 0x%X", sensors[i], smgr_handle);
      sub_dev_slot->sensors[num_sensors_filled++].sensor_type = sensors[i];
      (*sensors_init)[i] = sensors[i];
    }
    /*// TODO: ADD SUPPORT (both device_id and sensor_types)
    else if

     */
  }
  sub_dev_slot->smgr_handle = smgr_handle;
  sub_dev_slot->int_gpio = gpio;
  sub_dev_slot->num_sensors = num_sensors_filled;
  sub_dev_slot->powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
  return sub_dev_slot->num_sensors;
}

/**
 * @brief Validate and initialize the tap settings
 */
void sns_dd_mpu6500_tap_settings_init(
    sns_dd_mpu6500_tap_settings_s*    tap_settings,
    const sns_dd_mpu6xxx_nv_params_s* nv_params)
{
  if (tap_settings == NULL)
      return;
  // Preset everything to default
  tap_settings->shake_reject_thr = MPU6500_DEFAULT_SHAKE_REJECT_THR;
  tap_settings->shake_reject_timeout_thr = MPU6500_DEFAULT_SHAKE_REJECT_TIMEOUT_THR;
  tap_settings->shake_reject_time_thr = MPU6500_DEFAULT_SHAKE_REJECT_TIME_THR;
  tap_settings->multi_tap_thr = MPU6500_DEFAULT_MULTI_TAP_THR;
  tap_settings->tap_time_thr = MPU6500_DEFAULT_TAP_TIME_THR;
  tap_settings->tap_thr_x_1 = MPU6500_DEFAULT_TAP_THR_X_1;
  tap_settings->tap_thr_x_2 = MPU6500_DEFAULT_TAP_THR_X_2;
  tap_settings->tap_thr_y_1 = MPU6500_DEFAULT_TAP_THR_Y_1;
  tap_settings->tap_thr_y_2 = MPU6500_DEFAULT_TAP_THR_Y_2;
  tap_settings->tap_thr_z_1 = MPU6500_DEFAULT_TAP_THR_Z_1;
  tap_settings->tap_thr_z_2 = MPU6500_DEFAULT_TAP_THR_Z_2;
  tap_settings->multi_tap_num = MPU6500_DEFAULT_MULTI_TAP_NUM;

  // If no nv_params are specified, then keep the default
  if (nv_params == NULL)
  {
    MPU6050_MSG_0(LOW, "No NV params specified, using defaults...");
    MPU6050_MSG_1(LOW, "    Shake Reject Threshold is set to %d dps.", tap_settings->shake_reject_thr);
    MPU6050_MSG_1(LOW, "    Shake Reject Timeout Threshold is set to %u ms.", tap_settings->shake_reject_timeout_thr);
    MPU6050_MSG_1(LOW, "    Shake Reject Time Threshold is set to %u ms.", tap_settings->shake_reject_time_thr);
    MPU6050_MSG_1(LOW, "    Multi-Tap Time Threshold is set to %u ms.", tap_settings->multi_tap_thr);
    MPU6050_MSG_1(LOW, "    Tap Time Threshold is set to %u ms.", tap_settings->tap_time_thr);
    MPU6050_MSG_1(LOW, "    Tap Threshold 1 X-Axis is set to %u.", tap_settings->tap_thr_x_1);
    MPU6050_MSG_1(LOW, "    Tap Threshold 2 X-Axis is set to %u.", tap_settings->tap_thr_x_2);
    MPU6050_MSG_1(LOW, "    Tap Threshold 1 Y-Axis is set to %u.", tap_settings->tap_thr_y_1);
    MPU6050_MSG_1(LOW, "    Tap Threshold 2 Y-Axis is set to %u.", tap_settings->tap_thr_y_2);
    MPU6050_MSG_1(LOW, "    Tap Threshold 1 Z-Axis is set to %u.", tap_settings->tap_thr_z_1);
    MPU6050_MSG_1(LOW, "    Tap Threshold 2 Z-Axis is set to %u.", tap_settings->tap_thr_z_2);
    MPU6050_MSG_1(LOW, "    Multi-Tap Number is set to %u consecutive taps.", tap_settings->multi_tap_num);
    return;
  }

  MPU6050_MSG_2(LOW, "Checking the registry magic number (0x%X)... found 0x%X", MPU6500_TAP_SETTINGS_MAGIC_NUM, nv_params->magic_num);
  // Check to see if the nv_params contains tap-settings by validating the magic number
  if (MPU6500_TAP_SETTINGS_MAGIC_NUM == nv_params->magic_num)
  {
    MPU6050_MSG_0(LOW, "The registry magic number is correct. Loading NV params...");
    // Validate each parameter. If it is within the bounds, then use that value instead of the default value
    /* Shake Reject Threshold */
    if ((nv_params->shake_reject_thr <= MPU6500_UPPER_LIMIT_SHAKE_REJECT_THR) &&
        (nv_params->shake_reject_thr >= MPU6500_LOWER_LIMIT_SHAKE_REJECT_THR))
    {
      tap_settings->shake_reject_thr = nv_params->shake_reject_thr;
    }
    else
    {
      //MPU6050_MSG_3(ERROR, "Invalid Shake Reject Threshold value of %d. The upper limit is %d and the lower limit is %d (inclusive)",
      //    nv_params->shake_reject_thr, MPU6500_UPPER_LIMIT_SHAKE_REJECT_THR, MPU6500_LOWER_LIMIT_SHAKE_REJECT_THR);
      MPU6050_MSG_NV_PARAM_ERROR("Shake Reject Threshold", nv_params->shake_reject_thr, MPU6500_UPPER_LIMIT_SHAKE_REJECT_THR, MPU6500_LOWER_LIMIT_SHAKE_REJECT_THR);
    }
    MPU6050_MSG_1(LOW, "    Shake Reject Threshold is set to %d dps.", tap_settings->shake_reject_thr);
    /* Shake Reject Timeout Threshold */
    if (nv_params->shake_reject_timeout_thr <= ((MPU6500_UPPER_LIMIT_SHAKE_REJECT_TIMEOUT_THR)))
    {
      tap_settings->shake_reject_timeout_thr = nv_params->shake_reject_timeout_thr;
    }
    else
    {
      MPU6050_MSG_NV_PARAM_ERROR("Shake Reject Timeout Threshold", nv_params->shake_reject_timeout_thr, MPU6500_UPPER_LIMIT_SHAKE_REJECT_TIMEOUT_THR, MPU6500_LOWER_LIMIT_SHAKE_REJECT_TIMEOUT_THR);
    }
    MPU6050_MSG_1(LOW, "    Shake Reject Timeout Threshold is set to %u ms.", tap_settings->shake_reject_timeout_thr);
    /* Shake Reject Time Threshold */
    if (nv_params->shake_reject_time_thr <= ((MPU6500_UPPER_LIMIT_SHAKE_REJECT_TIME_THR)))
    {
      tap_settings->shake_reject_time_thr = nv_params->shake_reject_time_thr;
    }
    else
    {
      MPU6050_MSG_NV_PARAM_ERROR("Shake Reject Time Threshold", nv_params->shake_reject_time_thr, MPU6500_UPPER_LIMIT_SHAKE_REJECT_TIME_THR, MPU6500_LOWER_LIMIT_SHAKE_REJECT_TIME_THR);
    }
    MPU6050_MSG_1(LOW, "    Shake Reject Time Threshold is set to %u ms.", tap_settings->shake_reject_time_thr);
    /* Multi-Tap Time Threshold */
    if (nv_params->multi_tap_thr <= ((MPU6500_UPPER_LIMIT_MULTI_TAP_THR)))
    {
      tap_settings->multi_tap_thr = nv_params->multi_tap_thr;
    }
    else
    {
      MPU6050_MSG_NV_PARAM_ERROR("Multi-Tap Time Threshold", nv_params->multi_tap_thr, MPU6500_UPPER_LIMIT_MULTI_TAP_THR, MPU6500_LOWER_LIMIT_MULTI_TAP_THR);
    }
    MPU6050_MSG_1(LOW, "    Multi-Tap Time Threshold is set to %u ms.", tap_settings->multi_tap_thr);
    /* Tap Time Threshold */
    if (nv_params->tap_time_thr <= ((MPU6500_UPPER_LIMIT_TAP_TIME_THR)))
    {
      tap_settings->tap_time_thr = nv_params->tap_time_thr;
    }
    else
    {
      MPU6050_MSG_NV_PARAM_ERROR("Tap Time Threshold", nv_params->tap_time_thr, MPU6500_UPPER_LIMIT_TAP_TIME_THR, MPU6500_LOWER_LIMIT_TAP_TIME_THR);
    }
    MPU6050_MSG_1(LOW, "    Tap Time Threshold is set to %u ms.", tap_settings->tap_time_thr);
    /* Tap Threshold X-Axis */
//    if ((nv_params->tap_thr_x <= MPU6500_UPPER_LIMIT_TAP_THR_X) &&
//        (nv_params->tap_thr_x >= MPU6500_LOWER_LIMIT_TAP_THR_X))
//    {
//      tap_settings->tap_thr_x = nv_params->tap_thr_x;
//    }
//    else
//    {
//      MPU6050_MSG_NV_PARAM_ERROR("Tap Threshold X-Axis", nv_params->tap_thr_x, MPU6500_UPPER_LIMIT_TAP_THR_X, MPU6500_LOWER_LIMIT_TAP_THR_X);
//    }
    tap_settings->tap_thr_x_1 = nv_params->tap_thr_x_1;
    tap_settings->tap_thr_x_2 = nv_params->tap_thr_x_2;
    MPU6050_MSG_1(LOW, "    Tap Threshold 1 X-Axis is set to %u.", tap_settings->tap_thr_x_1);
    MPU6050_MSG_1(LOW, "    Tap Threshold 2 X-Axis is set to %u.", tap_settings->tap_thr_x_2);

    /* Tap Threshold Y-Axis */
//    if ((nv_params->tap_thr_y <= MPU6500_UPPER_LIMIT_TAP_THR_Y) &&
//        (nv_params->tap_thr_y >= MPU6500_LOWER_LIMIT_TAP_THR_Y))
//    {
//      tap_settings->tap_thr_y = nv_params->tap_thr_y;
//    }
//    else
//    {
//      MPU6050_MSG_NV_PARAM_ERROR("Tap Threshold Y-Axis", nv_params->tap_thr_y, MPU6500_UPPER_LIMIT_TAP_THR_Y, MPU6500_LOWER_LIMIT_TAP_THR_Y);
//    }

    tap_settings->tap_thr_y_1 = nv_params->tap_thr_y_1;
    tap_settings->tap_thr_y_2 = nv_params->tap_thr_y_2;
    MPU6050_MSG_1(LOW, "    Tap Threshold 1 Y-Axis is set to %u.", tap_settings->tap_thr_y_1);
    MPU6050_MSG_1(LOW, "    Tap Threshold 2 Y-Axis is set to %u.", tap_settings->tap_thr_y_2);

    /* Tap Threshold Z-Axis */
//    if ((nv_params->tap_thr_z <= MPU6500_UPPER_LIMIT_TAP_THR_Z) &&
//        (nv_params->tap_thr_z >= MPU6500_LOWER_LIMIT_TAP_THR_Z))
//    {
//      tap_settings->tap_thr_z = nv_params->tap_thr_z;
//    }
//    else
//    {
//      MPU6050_MSG_NV_PARAM_ERROR("Tap Threshold Z-Axis", nv_params->tap_thr_z, MPU6500_UPPER_LIMIT_TAP_THR_Z, MPU6500_LOWER_LIMIT_TAP_THR_Z);
//    }

    tap_settings->tap_thr_z_1 = nv_params->tap_thr_z_1;
    tap_settings->tap_thr_z_2 = nv_params->tap_thr_z_2;
    MPU6050_MSG_1(LOW, "    Tap Threshold 1 Z-Axis is set to %u.", tap_settings->tap_thr_z_1);
    MPU6050_MSG_1(LOW, "    Tap Threshold 2 Z-Axis is set to %u.", tap_settings->tap_thr_z_2);

    /* Multi-Tap Number */
    if ((nv_params->multi_tap_num <= MPU6500_UPPER_LIMIT_MULTI_TAP_NUM) &&
        (nv_params->multi_tap_num >= MPU6500_LOWER_LIMIT_MULTI_TAP_NUM))
    {
      tap_settings->multi_tap_num = nv_params->multi_tap_num;
    }
    else
    {
      MPU6050_MSG_NV_PARAM_ERROR("Multi-Tap Number", nv_params->multi_tap_num, MPU6500_UPPER_LIMIT_MULTI_TAP_NUM, MPU6500_LOWER_LIMIT_MULTI_TAP_NUM);
    }
    MPU6050_MSG_1(LOW, "    Multi-Tap Number is set to %u consecutive taps.", tap_settings->multi_tap_num);
  }
  else
  {
    MPU6050_MSG_0(LOW, "Incorrect registry magic number specified, using defaults...");
    MPU6050_MSG_1(LOW, "    Shake Reject Threshold is set to %d dps.", tap_settings->shake_reject_thr);
    MPU6050_MSG_1(LOW, "    Shake Reject Timeout Threshold is set to %u ms.", tap_settings->shake_reject_timeout_thr);
    MPU6050_MSG_1(LOW, "    Shake Reject Time Threshold is set to %u ms.", tap_settings->shake_reject_time_thr);
    MPU6050_MSG_1(LOW, "    Multi-Tap Time Threshold is set to %u ms.", tap_settings->multi_tap_thr);
    MPU6050_MSG_1(LOW, "    Tap Time Threshold is set to %u ms.", tap_settings->tap_time_thr);
    MPU6050_MSG_1(LOW, "    Tap Threshold 1 X-Axis is set to %u.", tap_settings->tap_thr_x_1);
    MPU6050_MSG_1(LOW, "    Tap Threshold 2 X-Axis is set to %u.", tap_settings->tap_thr_x_2);
    MPU6050_MSG_1(LOW, "    Tap Threshold 1 Y-Axis is set to %u.", tap_settings->tap_thr_y_1);
    MPU6050_MSG_1(LOW, "    Tap Threshold 2 Y-Axis is set to %u.", tap_settings->tap_thr_y_2);
    MPU6050_MSG_1(LOW, "    Tap Threshold 1 Z-Axis is set to %u.", tap_settings->tap_thr_z_1);
    MPU6050_MSG_1(LOW, "    Tap Threshold 2 Z-Axis is set to %u.", tap_settings->tap_thr_z_2);
    MPU6050_MSG_1(LOW, "    Multi-Tap Number is set to %u consecutive taps.", tap_settings->multi_tap_num);
    return;
  }
}

// TODO: EVENTUALLY ADD SUPPORT, for now we will just hard code settings

/**
 * @brief Parses the nv_params and fills in appropriate values for the axes_map
 *        and the tap_settings (if applicable)
 */
sns_ddf_status_e sns_dd_mpu6xxx_parse_nv_params(
    sns_ddf_nv_params_s*              entire_nv_params,
    sns_ddf_axes_map_s*               axes_map,
    sns_dd_mpu6500_tap_settings_s*    tap_settings)
{
  // Check for null-pointers
  if ((NULL == axes_map) || (NULL == tap_settings))
    return SNS_DDF_EINVALID_PARAM;

  // If there are no nv_params handed in, then set all to default values
  if (NULL == entire_nv_params)
  {
    MPU6050_MSG_0(LOW, "No NV params found. Using defaults.");
    sns_ddf_axes_map_init(axes_map, NULL);
    sns_dd_mpu6500_tap_settings_init(tap_settings, NULL);
  }
  // Else, parse the nv_params and validate each entry
  else
  {
    MPU6050_MSG_2(LOW, "Parsing NV params with group %u and id %u", entire_nv_params->nvitem_grp, entire_nv_params->nvitem_id);
    const sns_dd_mpu6xxx_nv_params_s* nv_params = (sns_dd_mpu6xxx_nv_params_s*) entire_nv_params->data;
    // Initialize the axes-map
    sns_ddf_axes_map_init(axes_map, entire_nv_params->data);
    sns_dd_mpu6500_tap_settings_init(tap_settings, nv_params);
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Initializes the driver and sets up devices.
 * @see sns_ddf_driver_if_s.init()
 */
sns_ddf_status_e sns_dd_mpu6xxx_init(
    sns_ddf_handle_t*       dd_handle_ptr,
    sns_ddf_handle_t        smgr_handle,
    sns_ddf_nv_params_s*    nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t                num_devices,  // TODO: PROPERLY IMPLEMENT THIS PARAMETER (Number of elements in @a device_info. )
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_e*       sensors[],
    uint32_t*               num_sensors)
{
    sns_ddf_status_e status;
    sns_dd_mpu6xxx_s* state;
    sns_ddf_sensor_e* sensors_init;

    // Suppress compiler warning.
    (void)sns_dd_mpu6xxx_sensors;

    MPU6050_MSG_3(LOW, "%d Initializing... number of sensors %u  first sensor %u", 11111111, *num_sensors, (*sensors)[0]);

    if(dd_handle_ptr == NULL || num_sensors == NULL || sensors == NULL || memhandler == NULL)
    {
      MPU6050_MSG_0(ERROR, "mpu6xxx_init: Null Pointer argument found.");
      return SNS_DDF_EINVALID_PARAM;
    }

    // If the MPU6xxx device instance does not exist, then initialize it
    state = mpu6xxx_find_same_port_dev(&device_info->port_config);
    if ( NULL == state )
    {
      MPU6050_MSG_2(LOW, "Initializing the device driver for the first time at I2C addr: 0x%X  bus instance: 0x%X", device_info->port_config.bus_config.i2c->slave_addr, device_info->port_config.bus_instance);
      // Driver being initialized for the first time.
      status = sns_ddf_malloc((void**)&state, sizeof(sns_dd_mpu6xxx_s) + (MPU6XXX_STATE_BYTE_ALIGNMENT - 4) /* 4 byte alignment is guaranteed */);
      if(status != SNS_DDF_SUCCESS)
        return status;
      memset(state, 0, sizeof(sns_dd_mpu6xxx_s));

      state = (sns_dd_mpu6xxx_s *) (((uint32_t) state + (MPU6XXX_STATE_BYTE_ALIGNMENT - 4)) / MPU6XXX_STATE_BYTE_ALIGNMENT * MPU6XXX_STATE_BYTE_ALIGNMENT);

      // Put this device instance at the front of the list (all inserts into
      // the list are placed at the front)
      state->next = NULL;
      mpu6xxx_head.next = state;
      sns_ddf_get_port_id(&device_info->port_config, &state->port_id);

      status = sns_ddf_malloc((void**)&(state->sensor_data.samples),sizeof(sns_ddf_sensor_sample_s)*(MPU6XXX_MAX_NUM_AXES+1)); /* +1 for temperature */
      if(status != SNS_DDF_SUCCESS)
        return status;

      // Open the communication port
      status = sns_ddf_open_port(&state->port_handle, &device_info->port_config);
      if(status != SNS_DDF_SUCCESS)
      {
        sns_ddf_mfree(state->sensor_data.samples);
        sns_ddf_mfree(state);
        return status;
      }

      // Find out if it is an MPU6050 or MPU6500 connected
      if ((status = sns_dd_mpu6050_validate_device_id(state)) != SNS_DDF_SUCCESS )
      {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state->sensor_data.samples);
        sns_ddf_mfree(state);
        return status;
      }

      state->sub_device_cnt = 0;

      sensors_init = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_e) * MPU6XXX_MAX_NUM_SENSORS);

      // Check to see if valid input is given for num_sensors and sensors.
      // If not, then it is using the older version of SMGR (without using probe & SSI)
      if (0 == *num_sensors && SNS_DDF_SENSOR__NONE == *sensors)
      {
        *num_sensors = 1;
        *sensors = mpu6050_first_sensor;
      }

      // Fill out the first sub-device slot
      uint32_t num_sensors_init = mpu6xxx_fill_sub_dev_slot(
          state,
          &state->sub_devices[0],
          smgr_handle,
          device_info->first_gpio,
          *num_sensors,
          *sensors,
          &sensors_init);
      if (num_sensors_init == 0)
      {
        MPU6050_MSG_0(ERROR, "mpu6xxx_init - Unable to initialize any sensors.");
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state->sensor_data.samples);
        sns_ddf_mfree(state);
        return SNS_DDF_EFAIL;
      }
      // Assign the dd_handle_ptr
      (*dd_handle_ptr) = (sns_ddf_handle_t) ((uint32_t) state | state->sub_device_cnt);
      state->sub_device_cnt++;

      sns_dd_mpu6xxx_parse_nv_params(nv_params, &(state->axes_map), &(state->tap_settings));

      // Reset the device
      status = sns_dd_mpu6xxx_reset(*dd_handle_ptr);
      if(status != SNS_DDF_SUCCESS)
      {
        sns_ddf_close_port(state->port_handle);
        sns_ddf_mfree(state->sensor_data.samples);
        sns_ddf_mfree(state);
        return status;
      }

      // Set the output and return
      (*num_sensors) = num_sensors_init;
      (*sensors) = sensors_init;
      MPU6050_MSG_3(LOW, "%d Finished initializing %u sensor(s). First sensor %u", 11111112, *num_sensors, (*sensors)[0]);
      return SNS_DDF_SUCCESS;
    }
    // Else, the device instance already exists. Put @sensors in a new sub-device slot.
    else
    {
      // If there is an empty slot
      if (state->sub_device_cnt < MPU6XXX_MAX_SUB_DEV)
      {
        sns_ddf_sensor_e* sensors_init = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_e) * MPU6XXX_MAX_NUM_SENSORS);
        // Fill out the next sub-device slot
        uint32_t num_sensors_init = mpu6xxx_fill_sub_dev_slot(
            state,
            &state->sub_devices[state->sub_device_cnt],
            smgr_handle,
            device_info->first_gpio,
            *num_sensors,
            *sensors,
            &sensors_init);
        if (num_sensors_init == 0)
        {
          MPU6050_MSG_0(ERROR, "mpu6xxx_init - Unable to initialize any sensors.");
          return SNS_DDF_EFAIL;
        }

        // Assign the dd_handle_ptr
        (*dd_handle_ptr) = (sns_ddf_handle_t) ((uint32_t) state | state->sub_device_cnt);

        // Reset the sub-device
        status = sns_dd_mpu6xxx_reset(*dd_handle_ptr);
        if (status != SNS_DDF_SUCCESS)
          return status;

        (*num_sensors) = num_sensors_init;
        (*sensors) = sensors_init;
        MPU6050_MSG_3(LOW, "%d Finished initializing %u sensor(s). First sensor %u", 11111112, *num_sensors, (*sensors)[0]);
        state->sub_device_cnt++;
        if(3 == state->sub_device_cnt)
        {
          MPU6050_MSG_0(LOW, "MPU6XXX has been fully initialized!");
        }
        return SNS_DDF_SUCCESS;
      }
      // Else
      else
      {
        MPU6050_MSG_0(ERROR, "mpu6xxx_init - Out of sub-device slots.");
        return SNS_DDF_ENOMEM;
      }
    }
    return SNS_DDF_SUCCESS;
}

/**
 * @brief Builds the list of supported ODRs up to MAX_FREQ for
 *        the given sensor.
 */
sns_ddf_status_e sns_dd_mpu6xxx_build_odr_list(
   sns_dd_mpu6xxx_s*     state,
   mpu6xxx_sensor_s*     sensor,
   sns_ddf_memhandler_s* memhandler,
   void**                value,
   uint32_t*             num_elems)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  const sns_ddf_odr_t  max_odr = 200;
  sns_ddf_odr_t* odr_list = sns_ddf_memhandler_malloc(
     memhandler, sizeof(sns_ddf_odr_t)*max_odr);
  uint16_t num_odrs = 0;
  sns_ddf_odr_t curr_odr = 0, next_odr = 0;
  uint8_t  smplrt_div = 0xFF;

  if (odr_list == NULL)
  {
    return SNS_DDF_ENOMEM;
  }

  switch ( sensor->sensor_type )
  {
  case SNS_DDF_SENSOR_ACCEL:
    odr_list[num_odrs++] = 1;
    if (MPU6050_CYCLE_MODE_LIMIT_HZ >= 5)
    {
      odr_list[num_odrs++] = 5;
    }
    if (MPU6050_CYCLE_MODE_LIMIT_HZ >= 10)
    {
      odr_list[num_odrs++] = 10;
    }
    if (MPU6050_CYCLE_MODE_LIMIT_HZ >= 40)
    {
      odr_list[num_odrs++] = 40;
    }
    curr_odr = odr_list[num_odrs-1];
    /* falls through */
  case SNS_DDF_SENSOR_GYRO:
  case SNS_DDF_SENSOR_TEMP:
    for ( ; num_odrs<=max_odr; num_odrs++ )
    {
      while ( (smplrt_div !=0) && ((next_odr = 1000 / smplrt_div) <= curr_odr) )
      {
        smplrt_div--;
      }
      if ( next_odr > curr_odr )
      {
        curr_odr = odr_list[num_odrs] = next_odr;
      }
      else
      {
        break;
      }
    }
    break;
  case SNS_DDF_SENSOR_DOUBLETAP:
    for ( num_odrs=0; num_odrs<MPU6500_DOUBLETAP_MAX_ODR; num_odrs++ )
    {
      odr_list[num_odrs] = num_odrs+1;
    }
    break;
  default:
    status = SNS_DDF_EINVALID_PARAM;
  }
  *value = odr_list;
  *num_elems = num_odrs;
  return status;
}

/**
 * @brief Retrieves the value of an attribute for a sensor.
 * @see sns_ddf_driver_if_s.get_attrib()
 */
sns_ddf_status_e sns_dd_mpu6xxx_get_attr(
   sns_ddf_handle_t      dd_handle,
   sns_ddf_sensor_e      sensor_type,
   sns_ddf_attribute_e   attrib,
   sns_ddf_memhandler_s* memhandler,
   void**                value,
   uint32_t*             num_elems)
{
  sns_dd_mpu6xxx_s* state = sns_dd_mpu6xxx_get_device(dd_handle);
  mpu6xxx_sensor_s* sensor = sns_dd_mpu6xxx_get_sensor(dd_handle, sensor_type);

  MPU6050_MSG_3(HIGH, "mpu6xxx_Get_attr dd_handle: 0x%X  sensor_type: %d  attrib: %d", dd_handle, sensor_type, attrib);
  MPU6050_MSG_3(HIGH, "mpu6xxx_Get_attr state: 0x%X  sensor: 0x%X  value: 0x%X", state, sensor, value);

    if((dd_handle == NULL) || (value == NULL) || (state == NULL) || (sensor == NULL))
    {
      MPU6050_MSG_0(ERROR, "mpu6xxx_get_attr: NULL parameter handed in.");
      return SNS_DDF_EINVALID_PARAM;
    }

    switch(attrib)
    {
       case SNS_DDF_ATTRIB_POWER_INFO:
       {
         MPU6050_MSG_0(LOW, "Getting attribute POWER INFO");
         sns_ddf_power_info_s* power_attrib = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_power_info_s));
          if (power_attrib == NULL)
             return SNS_DDF_ENOMEM;

          switch(sensor_type)
          {
            case SNS_DDF_SENSOR_ACCEL:
            {
              if (state->device_id == DEVICE_ID_MPU6050)
              {
                power_attrib->active_current = 500;
                power_attrib->lowpower_current = 5;
              }
              else if (state->device_id == DEVICE_ID_MPU6500)
              {
                power_attrib->active_current = 450;
                power_attrib->lowpower_current = 5;
                MPU6050_MSG_2(LOW, "MPU6500-Accel active current=%d Accel low power current=%d",
                              power_attrib->active_current, power_attrib->lowpower_current);
              }
              break;
            }
            case SNS_DDF_SENSOR_GYRO:
            {
              if (state->device_id == DEVICE_ID_MPU6050)
              {
                power_attrib->active_current = 3600;
                power_attrib->lowpower_current = 5;
              }
              else if (state->device_id == DEVICE_ID_MPU6500)
              {
                power_attrib->active_current = 3200;
                power_attrib->lowpower_current = 6;
                MPU6050_MSG_2(LOW, "MPU6500-Gyro active current=%d low power current=%d",
                              power_attrib->active_current, power_attrib->lowpower_current);
              }
              break;
            }
            case SNS_DDF_SENSOR_TEMP:
              power_attrib->active_current = 500;
              power_attrib->lowpower_current = 5;
              break;
            case SNS_DDF_SENSOR_DOUBLETAP:
              // TODO: Obtain current values for both (these estimates are not accurate)
              power_attrib->active_current = 500;
              power_attrib->lowpower_current = 5;
              break;
            default:
              return SNS_DDF_EINVALID_PARAM;
          }

          *value = power_attrib;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_RANGE:
       {
         MPU6050_MSG_0(LOW, "Getting attribute RANGE");
         switch(sensor_type)
           {
           case SNS_DDF_SENSOR_GYRO:
               *value = sns_dd_mpu6050_gyro_range;
               *num_elems = MPU6050_TABLE_SIZE(sns_dd_mpu6050_gyro_range);
               break;

           case SNS_DDF_SENSOR_ACCEL:
               *value = sns_dd_mpu6050_accel_range;
               *num_elems = MPU6050_TABLE_SIZE(sns_dd_mpu6050_accel_range);
               break;

           case SNS_DDF_SENSOR_TEMP:
               *value = &sns_dd_mpu6050_temp_range;
               *num_elems = 1;
               break;

           default:
             return SNS_DDF_EINVALID_PARAM;
           }

           return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_LOWPASS:
       {
         MPU6050_MSG_0(LOW, "Getting attribute LOWPASS");
         switch(sensor_type)
           {
           case SNS_DDF_SENSOR_GYRO:
             if (state->device_id == DEVICE_ID_MPU6050)
             {
               *value = &sns_dd_mpu6050_gyro_lpf;
               *num_elems = MPU6050_TABLE_SIZE(sns_dd_mpu6050_gyro_lpf);
             }
             else if (state->device_id == DEVICE_ID_MPU6500)
             {
               *value = &sns_dd_mpu6500_gyro_lpf;
               *num_elems = MPU6050_TABLE_SIZE(sns_dd_mpu6500_gyro_lpf);
             }
             break;

           case SNS_DDF_SENSOR_ACCEL:
             if (state->device_id == DEVICE_ID_MPU6050)
             {
               *value = &sns_dd_mpu6050_accel_lpf;
               *num_elems = MPU6050_TABLE_SIZE(sns_dd_mpu6050_accel_lpf);
             }
             else if (state->device_id == DEVICE_ID_MPU6500)
             {
               *value = &sns_dd_mpu6500_accel_lpf;
               *num_elems = MPU6050_TABLE_SIZE(sns_dd_mpu6500_accel_lpf);
             }
             break;

           default:
               return SNS_DDF_EINVALID_PARAM;
           }

           return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_ODR:
       {
          uint32_t *odr_ptr;
          odr_ptr = &sensor->desired_odr;

          MPU6050_MSG_0(LOW, "Getting attribute ODR");

          if (0 != *odr_ptr)
          {
            odr_ptr = &sensor->odr;    /* returns current odr */
          }
          MPU6050_MSG_2(HIGH, "mpu6xxx_get_attr sensor_type: %d  current odr: %d", sensor_type, *odr_ptr);
          *value = odr_ptr;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }
       case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
       {
         return sns_dd_mpu6xxx_build_odr_list(state, sensor, memhandler, value, num_elems);
       }

       case SNS_DDF_ATTRIB_BIAS:
       {
         MPU6050_MSG_0(LOW, "Getting attribute BIAS");
         switch(sensor_type)
           {
             case SNS_DDF_SENSOR_ACCEL:
               *value = &sensor->biases;
               *num_elems = MPU6050_TABLE_SIZE(sensor->biases);
               break;
             case SNS_DDF_SENSOR_GYRO:
               *value = &sensor->biases;
               *num_elems = MPU6050_TABLE_SIZE(sensor->biases);
               break;
             default:
               return SNS_DDF_EINVALID_PARAM;
           }

           return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_DRIVER_INFO:
       {
         MPU6050_MSG_0(LOW, "Getting attribute DRIVER INFO");
         *value = &mpu6xxx_head.dd_info;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_DEVICE_INFO:
       {
         MPU6050_MSG_0(LOW, "Getting attribute DEVICE INFO");
         sns_ddf_device_info_s* device_info = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_device_info_s));
          if(device_info == NULL)
             return SNS_DDF_ENOMEM;

          device_info->vendor = "InvenSense";

          if (state->device_id == DEVICE_ID_MPU6050)
          {
            device_info->model = "MPU6050";
            device_info->name = "Accelerometer, Gyroscope, Temperature";
          }
          else if (state->device_id == DEVICE_ID_MPU6500)
          {
            device_info->model = "MPU6500";
            device_info->name = "Accelerometer, Gyroscope, Temperature, Double-Tap";
          }
          // TODO: ADD SUPPORT FOR MPU6515 AND THE SUPPORTED SENSOR TYPES
          else
          {
            return SNS_DDF_EFAIL;
          }

          device_info->version = 1;

          *value = device_info;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_DELAYS:
       {
         MPU6050_MSG_0(LOW, "Getting attribute DELAYS");

         sns_ddf_delays_s *delays = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_delays_s));
          if (delays == NULL)
             return SNS_DDF_ENOMEM;

          delays->time_to_active = 30000;
          delays->time_to_data = 500;

          *value = delays;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_RESOLUTION_ADC:
       {
         MPU6050_MSG_0(LOW, "Getting attribute RESOLUTION ADC");

         sns_ddf_resolution_adc_s *device_res = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_resolution_adc_s));
          if(device_res == NULL)
             return SNS_DDF_ENOMEM;

          device_res->bit_len = 16;
          device_res->max_freq = 200;
          *value = device_res;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_RESOLUTION:
       {
         MPU6050_MSG_1(LOW, "Getting attribute RESOLUTION attrib: %u", SNS_DDF_ATTRIB_RESOLUTION);

         sns_ddf_resolution_t* device_res = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_resolution_t));
          if(device_res == NULL)
             return SNS_DDF_ENOMEM;

          switch(sensor_type)
          {
          case SNS_DDF_SENSOR_GYRO:
              *device_res = sns_dd_mpu6050_gyro_sensitivity[sensor->range] >> 9;
              break;

          case SNS_DDF_SENSOR_ACCEL:
              *device_res = sns_dd_mpu6050_accel_sensitivity[sensor->range];
              break;

          case SNS_DDF_SENSOR_TEMP:
              *device_res = sns_dd_mpu6050_temp_sensitivity;
              break;

          // The resolution of the DOUBLETAP sensor is defined as 1
          case SNS_DDF_SENSOR_DOUBLETAP:
              *device_res = 1;
              break;

          default:
              return SNS_DDF_EINVALID_PARAM;
          }

          *value = device_res;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_IO_REGISTER:
       {
         sns_ddf_io_register_s* io_reg_in = (sns_ddf_io_register_s *) (*value);
         uint8_t bytes_read;
         sns_ddf_status_e status;

         MPU6050_MSG_1(LOW, "Getting attribute IO_REGISTER attrib: %u", SNS_DDF_ATTRIB_IO_REGISTER);

         // Address sizes are limited to uint16_t for the MPU6XXX
         if ((uint32_t)(io_reg_in->address_ptr) > 0xFFFF)
         {
           MPU6050_MSG_1(HIGH, "mpu6xxx_set_attr: invalid address 0x%X", (io_reg_in->address_ptr));
           return SNS_DDF_EINVALID_PARAM;
         }
         // sns_ddf_read_port is limited to reading 0xFF bytes
         if ((uint16_t)io_reg_in->length > 0xFF)
         {
           MPU6050_MSG_1(HIGH, "mpu6xxx_set_attr: length too long, %d", io_reg_in->length);
           return SNS_DDF_EINVALID_PARAM;
         }

         MPU6050_MSG_2(LOW, "mpu6xxx_get_attr: (IO_REGISTER) Reading %d bytes from register 0x%X", io_reg_in->length, io_reg_in->address_ptr);

         // Allocate memory for the output data structure and for the data to be read from the register(s)
         sns_ddf_io_register_s* io_reg_out = sns_ddf_memhandler_malloc(
             memhandler, sizeof(sns_ddf_io_register_s) + (sizeof(uint8_t) * (io_reg_in->length - 1)));
         if (io_reg_out == NULL)
           return SNS_DDF_ENOMEM;

         status = sns_ddf_read_port(
             state->port_handle,
             (uint16_t) ((uint32_t)io_reg_in->address_ptr),
             &(io_reg_out->data[0]),
             (uint8_t) io_reg_in->length,
             &bytes_read);
         if (status != SNS_DDF_SUCCESS)
           return status;

         // Fill out the information in the output io_register data structure
         io_reg_out->address_ptr = io_reg_in->address_ptr;
         io_reg_out->addr_fixed = io_reg_in->addr_fixed;
         io_reg_out->data_width = io_reg_in->data_width;
         io_reg_out->length = (uint32_t) bytes_read;
         // Return the io_register data structure by reference
         *value = io_reg_out;
         *num_elems = 1;

         return SNS_DDF_SUCCESS;
       }

       default:
         MPU6050_MSG_1(ERROR, "mpu6xxx_get_attr: Invalid attribute %u", attrib);
         return SNS_DDF_EINVALID_PARAM;
    }
}
