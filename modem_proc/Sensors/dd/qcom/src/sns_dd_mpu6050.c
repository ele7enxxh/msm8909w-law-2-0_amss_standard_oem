/*
 * @file sns_dd_mpu6050.c
 * @brief MPU-6xxx driver, supporting gyro, accel, DMP and temperature.
 *
 * Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mpu6050.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-07-09 AH   Fix compiler warnings
  2014-07-22 MW   Eliminate compiler warning
  2013-08-15 vh   Eliminated Klocwork warnings
  2013-08-09 pn   Uses configured range index
  2013-07-31 vh   Eliminated Klocwork warnings
  2013-07-17 vh   Eliminated compiler warnings
  2013-06-13 pn   Added wait loop to reset_device()
  2013-05-22 lka  Added support for both MPU6050 and MPU6500.
                  Added support for DMP.
                  Added additional notes on MD/DMP concurrency.
                  Fixed indentations.
  2013-04-25 br   Fixed issue of cycle mode getting stuck in enabled state.
  2013-04-23 br   Initialized related variables when the device is resetting
  2013-04-22 br   Set SMPRT_DIV register only when needed; Disables MD with temp/gyro clients
  2013-04-18 br   Disables MD when there is a gyro client
  2013-04-05 rs   Added MD and DRI support for MPU6500
  2013-04-03 pn   Added support for MPU6500
  2013-03-18 vh   Added error check for functions
  2013-03-11 pn   Logs correct timestamp for DRI mode
  2013-02-19 jtl  Added probe function
  2012-12-26 pn   Updated message macros
  2012-12-05 lka  Modified setup sequence of motion detect interrupt
  2012-09-13 vh   Eliminated compiler warnings
  2012-08-27 sc   Update with dummy data when EnableI2C is false
  2012-06-27 sd   Fixed Klocwork error/warning
  2012-06-21 sd   Fixed cycle mode was not enabled correctly.
                  disable cycle mode when change power state
  2012-06-20 sd   Do not set ODR if it is the same as previous
  2012-06-13 br   Cleared cycle mode and set LPF at any condition for normal mode
  2012-05-25 pn   Moved self-test related materials to a separate file.
  2012-05-16 br   setting registers related ODR only when ODR is changed.
  2012-04-30 br   Increased motion detect sensitivity by clearing DLPF configuration
  2012-04-18 br   Implemented MD feature with the cyclic mode
  2012-04-13 br   Fixed not to overflow SMPRT_DIV value which is one byte length.
  2012-03-30 sd   Moved changing accel ODR when enabling/disabling MD from SMGR to DD.
  2012-03-29 ks   Added HW Motion Detect Interrupt HPF Filter setting
  2012-03-28 ag   Fix delay in reset function; update self test thresholds; set range only
                  for requested sensor in self test routine
  2012-03-22 sd   Added temperature support
  2012-03-21 br   enabled SNS_LOG_CONVERTED_SENSOR_DATA log packet for DRI
  2012-03-16 sd   clear required odr when set sensor to low power
                  changes int cfg to level high
  2012-03-13 sd   Added cycle mode support
  2012-01-17 yk   Added support for DRI
  2012-02-29 sd   Added FEATURE_MPU6050_INT_PIN_CONFIG to disable MPU3050 int
  2012-02-28 br   Updated reset() so as to reset whole device when accel and gyro is in SLEEP
  2012-02-22 ag   Move accel Z axis bias adjustment to inertial test, update test
                  thresholds. Return correct error code during self test for
                  unsupported sensors and during reset upon errors
  2012-02-10 sd   Adde MD support for MPU6050
  2012-02-02 yk   set MPU6050_REG_PWR_MGMT_2 for making standby mode
  2012-02-01 sd   Change the default gyro_range to 2 during initial reset
  2011-12-07 ag   Adjust accel Z axis bias; Add comments
  2011-12-07 sd   changed the default sensitivity for rev D
  2011-12-01 sd   Added delay after MPU6050 reset during selftest
                  Changed the bias threshold for MPU6050 selftest
  2011-11-08 yk   Added support for axis mapping.
  2011-11-09 sd   Removed 20 ms delay in reset function, SMGR handles it using timer
  2011-10-21 yk   Added data logging at each data retrieval.
  2011-10-10 ag   Added comment
  2011-10-09 ag   Fixes for self test
  2011-10-07 br   Fixed per-sensor power enabling
  2011-10-06 yk   Reversed order of gyro sensitivity.
  2011-10-06 yk   Fixed polarity for gyro on 8960.
  2011-10-06 yk   Fixed polarity for accel on 8960.
  2011-10-05 br   Made POWERSTATE_ACTIVE before making configuration
  2011-10-05 yk   Updated sensitivity values and corrected power state bug.
  2011-10-04 yk   Added support for per-sensor reset and power state changes.
  2010-08-26 yk   Initial revision
==============================================================================*/

/**
 * !!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT NOTES !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * TODO:
 * From previous discussions that we have had with InvenSense engineers, on-chip
 * MD should be concurrent with the DMP. However, they mentioned that they could
 * not think of any use case in which both features would be concurrently active
 * (i.e. running at the same time). They said that MD is for low power use cases
 * while the DMP is for higher power use cases. They also mentioned that if you
 * would like to use both, it would make more sense to either:
 * 1. Just run the DMP without MD because any taps detected suggests the start
 *    of some motion (in which case we would stream the accel output to our own
 *    MD algorithm on the ADSP)
 * or
 * 2. Run MD and upon detecting motion, enable the DMP to begin tap gesture
 *    detection.
 *
 * Currently the driver DOES NOT ALLOW motion detect to be enabled while the DMP
 * is running. (On the other hand, the DMP can be enabled while motion-detect is
 * running. I have not tested how this will affect motion-detect.)
 *
 */

#include "sns_dd_mpu6050_priv.h"

#include <stdbool.h>
#include <stdio.h>
#include "fixed_point.h"
#include "sns_memmgr.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd_inertial_test.h"
#include "sns_ddf_signal.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"


extern volatile boolean EnableI2C;

extern q16_t sns_dd_mpu6050_gyro_lpf[7];

// Forward declaration of driver interface functions.
sns_ddf_driver_if_s sns_dd_mpu6xxx_if;

// Gyroscope sensitivity values, in LSB/deg/s. In order to maximize resolution
// we shift the value to take advantage of as many fractional bits as possible.
// We then un-shift after calculating the data values to be returned. These must
// correlate directly with the range setting in sns_dd_mpu6050_gyro_range[].
const q16_t sns_dd_mpu6050_gyro_sensitivity[] =
{
    MPU6050_DPS_TO_RPSQ16(1.0 / 131.0) << 9,
    MPU6050_DPS_TO_RPSQ16(1.0 / 65.5)  << 9,
    MPU6050_DPS_TO_RPSQ16(1.0 / 32.8)  << 9,
    MPU6050_DPS_TO_RPSQ16(1.0 / 16.4)  << 9
};

// Accelerometer sensitivity values, in G/LSB. These must correlate directly
// with the range setting in sns_dd_mpu6050_accel_range[].
q16_t sns_dd_mpu6050_accel_sensitivity[] =
{
    FX_FLTTOFIX_Q16(G / (8192 * 2)),
    FX_FLTTOFIX_Q16(G / (4096 * 2)),
    FX_FLTTOFIX_Q16(G / (2048 * 2)),
    FX_FLTTOFIX_Q16(G / (1024 * 2))
};


// Register configuration values that correlate to sns_dd_mpu6050_gyro_range[]
uint8_t sns_dd_mpu6050_gyro_range_reg[] =
{
    FS_SEL_250DPS,
    FS_SEL_500DPS,
    FS_SEL_1000DPS,
    FS_SEL_2000DPS
};

// Register configuration values that correlate to sns_dd_mpu6050_accel_range[]
uint8_t sns_dd_mpu6050_accel_range_reg[] =
{
    AFS_SEL_2G,
    AFS_SEL_4G,
    AFS_SEL_8G,
    AFS_SEL_16G
};

// Register configuration values that correlate to sns_dd_mpu6050_accel_lpf[]
static const uint8_t sns_dd_mpu6050_accel_lpf_reg[] =
{
    DLPF_CFG_ACCEL_5HZ,
    DLPF_CFG_ACCEL_10HZ,
    DLPF_CFG_ACCEL_21HZ,
    DLPF_CFG_ACCEL_44HZ,
    DLPF_CFG_ACCEL_94HZ,
    DLPF_CFG_ACCEL_184HZ,
    DLPF_CFG_ACCEL_260HZ
};

// Register configuration values that correlate to sns_dd_mpu6050_gyro_lpf[]
static const uint8_t sns_dd_mpu6050_gyro_lpf_reg[] =
{
    DLPF_CFG_GYRO_5HZ,
    DLPF_CFG_GYRO_10HZ,
    DLPF_CFG_GYRO_20HZ,
    DLPF_CFG_GYRO_42HZ,
    DLPF_CFG_GYRO_98HZ,
    DLPF_CFG_GYRO_188HZ,
    DLPF_CFG_GYRO_256HZ
};

// Register configuration values that correlate to sns_dd_mpu6500_accel_lpf[]
#if 0  // Unused variable
static const uint8_t sns_dd_mpu6500_accel_lpf_reg[] =
{
    MPU6500_DLPF_CFG_GYRO_3600HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_5HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_10HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_20HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_41HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_92HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_184HZ,
    MPU6500_ACCEL_DLPFCFG_NORMAL_460HZ
};

// Register configuration values that correlate to sns_dd_mpu6050_gyro_lpf[]
static const uint8_t sns_dd_mpu6500_gyro_lpf_reg[] =
{
    MPU6500_DLPF_CFG_GYRO_5HZ,
    MPU6500_DLPF_CFG_GYRO_10HZ,
    MPU6500_DLPF_CFG_GYRO_20HZ,
    MPU6500_DLPF_CFG_GYRO_41HZ,
    MPU6500_DLPF_CFG_GYRO_92HZ,
    MPU6500_DLPF_CFG_GYRO_184HZ,
    MPU6500_DLPF_CFG_GYRO_250HZ
};
#endif

// Temperature offset, in LSB/degC.
static const int16_t sns_dd_mpu6050_temp_offset_lsb = -521;

// Temperature at which temperature offset value occurs.
static const q16_t sns_dd_mpu6050_temp_offset_deg = FX_FLTTOFIX_Q16(35.0);

// Temperature sensor sensitivity (untrimmed), in LSB/degC.
const q16_t sns_dd_mpu6050_temp_sensitivity = FX_FLTTOFIX_Q16(1.0 / 340.0);

/**
 * @brief Finds the device instance corresponding to the specified ddf_handle
 */
sns_dd_mpu6xxx_s* sns_dd_mpu6xxx_get_device(sns_ddf_handle_t dd_handle)
{
  return (sns_dd_mpu6xxx_s*) ((uint32_t) dd_handle & (0xFFFFFFFF - MPU6XXX_STATE_BYTE_ALIGNMENT + 1));  // Zero-out the lower two bits of dd_handle
}

/**
 * @brief Retrieves the specified sensor type in the sub-device
 */
mpu6xxx_sensor_s* sns_dd_mpu6xxx_get_sensor(
    sns_ddf_handle_t dd_handle, sns_ddf_sensor_e sensor)
{
  // Get the appropriate sub-device
  mpu6xxx_sub_dev_s* sub_dev = sns_dd_mpu6xxx_get_sub_dev(dd_handle);
  if (NULL == sub_dev)
  {
    MPU6050_MSG_0(ERROR, "mpu6xxx_get_sensor: Received a NULL sub-device pointer.");
    return NULL;
  }

  // Find the sensor within the sub-device
  uint32_t j;
  for (j = 0; j < sub_dev->num_sensors; j++)
  {
    if (sub_dev->sensors[j].sensor_type == sensor)
      return &(sub_dev->sensors[j]);
  }


  // If the sensor is not found, return NULL
  MPU6050_MSG_1(ERROR, "mpu6xxx_get_sensor could not find a sensor of type %d", sensor);
  return NULL;
}

/**
 * @brief Searches for the specified sensor type in the entire device instance
 *
 * It will return the first instance of the specified sensor type that it finds.
 *
 * This is mostly used to check concurrencies.
 *
 * @param[in]   state     Pointer to the device-instance where the search will take place.
 * @param[in]   sensor    The sensor type that is being searched for.
 * @param[out]  sub_dev   (Optional) Pointer to the sub-device that the sensor was found in.
 *
 * @return      A pointer to the desired sensor structure
 */
mpu6xxx_sensor_s* sns_dd_mpu6xxx_search_for_sensor(
    sns_dd_mpu6xxx_s* state, sns_ddf_sensor_e sensor, mpu6xxx_sub_dev_s** sub_dev_out)
{
  mpu6xxx_sub_dev_s* sub_dev;

  if (NULL == state)
    return NULL;

  // Find the sensor within the entire device instance
  uint8_t i;
  uint32_t j;
  for (i = 0; i < state->sub_device_cnt; i++)
  {
    sub_dev = &state->sub_devices[i];
    for (j = 0; j < sub_dev->num_sensors; j++)
    {
      if (sub_dev->sensors[j].sensor_type == sensor)
      {
        if (sub_dev_out != NULL)
          *sub_dev_out = sub_dev;
        return &(sub_dev->sensors[j]);
      }
    }
  }

  // If the sensor is not found, return NULL
  if (sub_dev_out != NULL)
    *sub_dev_out = NULL;
  return NULL;
}

/**
 * @brief Retrieves the sub-device associated with the ddf_handle
 */
mpu6xxx_sub_dev_s* sns_dd_mpu6xxx_get_sub_dev(sns_ddf_handle_t dd_handle)
{
  uint8_t sub_dev_index = (uint8_t) ((uint32_t) dd_handle & (MPU6XXX_STATE_BYTE_ALIGNMENT - 1));
  sns_dd_mpu6xxx_s* state = sns_dd_mpu6xxx_get_device(dd_handle);
  /* Return with a null-pointer if the device instance is null or if the sub-
     device index is out-of-bounds */
  if (NULL == state)
    return NULL;
  if (sub_dev_index >= state->sub_device_cnt)
  {
    MPU6050_MSG_2(ERROR, "mpu6xxx_get_sub_dev: Desired sub-device index, %u, is invalid. (It is greater than or equal to the current sub-device count %u)", sub_dev_index, state->sub_device_cnt);
    return NULL;
  }
  return &(state->sub_devices[sub_dev_index]);
}

/**
 * @brief Retrieves the sub-device index associated with the ddf_handle
 *
 * @return The sub-device index if a corresponding sub-device is found.
 *         Otherwise it returns 0xFF to signal that the sub-device was not found.
 */
uint8_t sns_dd_mpu6xxx_get_sub_dev_idx(sns_ddf_handle_t dd_handle)
{
  uint8_t sub_dev_index = (uint8_t) ((uint32_t) dd_handle & (MPU6XXX_STATE_BYTE_ALIGNMENT - 1));
  sns_dd_mpu6xxx_s* state = sns_dd_mpu6xxx_get_device(dd_handle);
  /* Return with a null-pointer if the device instance is null or if the sub-
     device index is out-of-bounds */
  if (NULL == state)
    return 0xFF;
  if (sub_dev_index >= state->sub_device_cnt)
  {
    MPU6050_MSG_2(ERROR, "mpu6xxx_get_sub_dev_idx: Desired sub-device index, %u, is invalid. (It is greater than or equal to the current sub-device count %u)", sub_dev_index, state->sub_device_cnt);
    return 0xFF;
  }
  return sub_dev_index;
}

/**
 * @brief       Determines if the DMP is active (i.e. the desired ODR for DOUBLETAP is not zero).
 *
 * @param[in]   state     Pointer to the device-instance.
 *
 * @return      TRUE if the DMP is active, FALSE otherwise.
 */
bool sns_dd_mpu6xxx_is_dmp_active(sns_dd_mpu6xxx_s* state)
{
  mpu6xxx_sensor_s* doubletap_sensor;

  // TODO: extend the search for sensor to any new sensor type
  doubletap_sensor = sns_dd_mpu6xxx_search_for_sensor(state, SNS_DDF_SENSOR_DOUBLETAP, NULL);
  if(NULL != doubletap_sensor)
  {
    if (doubletap_sensor->desired_odr != 0)
      return true;
  }

  return false;
}

/**
 * @brief Searches through all sensors (excluding DOUBLETAP) within the device
 *        instance to find the highest ODR.
 *
 * @param[in]   state     Pointer to the device-instance.
 *
 * @return      The maximum desired ODR.
 */
uint32_t sns_dd_mpu6xxx_find_max_desired_odr(sns_dd_mpu6xxx_s* state)
{
  uint8_t i;
  uint32_t j;
  uint32_t max_odr = 0;

  if (state == NULL)
    return 0;

  for (i = 0; i < state->sub_device_cnt; i++)
  {
    for (j = 0; j < state->sub_devices[i].num_sensors; j++)
    {
      // Do not take DOUBLETAP's desired ODR into account
      if ((max_odr < state->sub_devices[i].sensors[j].desired_odr) &&
          (state->sub_devices[i].sensors[j].sensor_type != SNS_DDF_SENSOR_DOUBLETAP))
        max_odr = state->sub_devices[i].sensors[j].desired_odr;
    }
  }

  return max_odr;
}

/** Returns the actual gyro output rate for a specific LPF setting. */
static uint16_t sns_dd_mpu6050_get_gyro_output_rate(uint8_t lpf_index)
{
    // See MPU6050 Register Map, section 4.3.
    return (lpf_index == MPU6050_TABLE_SIZE(sns_dd_mpu6050_gyro_lpf) - 1) ?
        8000 : 1000;
}

static sns_ddf_status_e sns_dd_mpu6xxx_set_cycle_mode(
    sns_dd_mpu6xxx_s* state, bool enable );

/**
 * @brief Sets the power state of the device
 *
 * @param[in] state  Ptr to the driver structure
 * @param[in] mode   Ptr to the variable of sns_ddf_powerstate_e type
 *
 * @return Success status of this operation.
 */
static sns_ddf_status_e sns_dd_mpu6xxx_set_device_powerstate(
   sns_dd_mpu6xxx_s* state, sns_ddf_powerstate_e powerstate)
{
  uint8_t buffer = 0;
  uint8_t bytes_read;
  sns_ddf_status_e status;
  bool device_active;

  status = sns_ddf_read_port(state->port_handle,
      MPU6050_REG_PWR_MGMT_1, &buffer, 1, &bytes_read);
  if(status != SNS_DDF_SUCCESS)
     return status;

  MPU6050_MSG_3(HIGH, "set_dev_power %d buffer=%d powerstate=%d",
                11111, buffer, powerstate);

  // No need to perform an expensive I2C operation if we're already at the
  // right power state.
  device_active = ((buffer & (1 << SLEEP)) == SLEEP_DISABLE);
  if((powerstate == SNS_DDF_POWERSTATE_ACTIVE && device_active) ||
     (powerstate == SNS_DDF_POWERSTATE_LOWPOWER && !device_active))
      return SNS_DDF_SUCCESS;

  // Hold all Advanced-?Features under reset
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_USER_CTRL, 0x0C, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;

  buffer ^= (1 << SLEEP);
  status = sns_ddf_write_port(
      state->port_handle,
      MPU6050_REG_PWR_MGMT_1,
      &buffer,
      1,
      &bytes_read);

  MPU6050_MSG_3(HIGH, "set_dev_power %d buffer=%d dev_active=%d",
                22222, buffer, device_active);

    return status;
}

/**
 *
 */
static bool sns_dd_mpu6xxx_sensors_active(sns_dd_mpu6xxx_s* state)
{
  // Check for null-pointers
  if (NULL == state)
    return FALSE;

  // Iterate through each sub-device to see if it is active or not
  uint8_t i;
  for (i = 0; i < state->sub_device_cnt; i++)
  {
    if (state->sub_devices[i].powerstate == SNS_DDF_POWERSTATE_ACTIVE)
      return TRUE;
  }
  // No devices found active, return false
  return FALSE;
}

/**
 * Convert the device instance's axes map into an orientation matrix (mounting matrix)
 * that the DMP can understand.
 */
sns_ddf_status_e sns_dd_mpu6500_setup_orientation(sns_dd_mpu6xxx_s* state)
{
  sns_ddf_status_e status;
  sns_ddf_axes_map_s axes_map = state->axes_map;
//  uint8_t i;
  uint8_t gyro_axes[3];
  uint8_t accel_axes[3];
  uint8_t gyro_sign[3] = {0x36, 0x56, 0x76};
  uint8_t accel_sign[3] = {0x26, 0x46, 0x66};

  const uint8_t gyro_axes_values[3] = {0x4C, 0xCD, 0x6C};
  const uint8_t accel_axes_values[3] = {0x0C, 0xC9, 0x2C};
//  const uint8_t gyro_sign_values[3] = {0x36, 0x56, 0x76};
//  const uint8_t accel_sign_values[3] = {0x26, 0x46, 0x66};

  // MAP THE AXES MAP TO THE ORIENTATION MATRIX
  gyro_axes[0] = gyro_axes_values[axes_map.indx_x];
  gyro_axes[1] = gyro_axes_values[axes_map.indx_y];
  gyro_axes[2] = gyro_axes_values[axes_map.indx_z];

  accel_axes[0] = accel_axes_values[axes_map.indx_x];
  accel_axes[1] = accel_axes_values[axes_map.indx_y];
  accel_axes[2] = accel_axes_values[axes_map.indx_z];

  if (axes_map.sign_x < 0)
  {
    gyro_sign[0] = gyro_sign[0] | 1;
    accel_sign[0] = accel_sign[0] | 1;
  }
  if (axes_map.sign_y < 0)
  {
    gyro_sign[1] = gyro_sign[1] | 1;
    accel_sign[1] = accel_sign[1] | 1;
  }
  if (axes_map.sign_z < 0)
  {
    gyro_sign[2] = gyro_sign[2] | 1;
    accel_sign[2] = accel_sign[2] | 1;
  }

  if ((status = sns_dd_mpu6xxx_write_dmp_register(state,
    1062 /* MPU6500_REG_DMP_GYRO_MOUNT_MATRIX_CONFIG */, gyro_axes, 3)) != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_3(LOW, "setup orientation: wrote 0x%X 0x%X 0x%X to gyro axes", gyro_axes[0], gyro_axes[1], gyro_axes[2]);
  if ((status = sns_dd_mpu6xxx_write_dmp_register(state,
    1066 /* MPU6500_REG_DMP_GYRO_MOUNT_MATRIX_CONFIG */, accel_axes, 3)) != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_3(LOW, "setup orientation: wrote 0x%X 0x%X 0x%X to accel axes", accel_axes[0], accel_axes[1], accel_axes[2]);
  if ((status = sns_dd_mpu6xxx_write_dmp_register(state,
    1088 /* MPU6500_REG_DMP_GYRO_MOUNT_MATRIX_CONFIG */, gyro_sign, 3)) != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_3(LOW, "setup orientation: wrote 0x%X 0x%X 0x%X to gyro sign", gyro_sign[0], gyro_sign[1], gyro_sign[2]);
  if ((status = sns_dd_mpu6xxx_write_dmp_register(state,
    1073 /* MPU6500_REG_DMP_GYRO_MOUNT_MATRIX_CONFIG */, accel_sign, 3)) != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_3(LOW, "setup orientation: wrote 0x%X 0x%X 0x%X to accel sign", accel_sign[0], accel_sign[1], accel_sign[2]);

  return status;
}

/**
 * @brief Performs all of the required steps to setup the DMP for DOUBLETAP
 *
 * !!Important!! You can only flash the DMP image ONCE before writing the DMP start address (FW_START)
 *
 * THIS FUNCTION SHOULD ONLY BE CALLED BY sns_dd_mpu6xxx_reset_device (hardware reset)!!
 */
sns_ddf_status_e sns_dd_mpu6500_setup_dmp(sns_dd_mpu6xxx_s* state)
{
  sns_ddf_status_e status;
  uint8_t buffer[4];
//  q16_t threshold;
//  uint16_t dmp_threshold_value;
//  uint8_t fifo_rate_div_en[12] = {0xFE,0xF2,0xAB,0xC4,0xAA,0xF1,0xDF,0xDF,0xBB,0xAF,0xDF,0xDF};
//  uint8_t bytes_written;

  MPU6050_MSG_0(MEDIUM, "Setting up the DMP...");

  // Load DMP Firmware first
  if ((status = sns_dd_mpu6xxx_load_dmp_firmware(state, dmp_image.image, SNS_DDF_MAX_ESPIMAGE_SIZE /* dmp_image.size */)) != SNS_DDF_SUCCESS)
    return status;

  // Load the Orientation Matrix using the Axes Map
  status = sns_dd_mpu6500_setup_orientation(state);
  if (status != SNS_DDF_SUCCESS)
  {
    MPU6050_MSG_0(ERROR, "Unsuccessful setup of orientation.");
  }

  // TODO: add support for the newest sensors

  buffer[0] = DMP_ORIENT_TAP_ENABLE;
  if ((status = sns_dd_mpu6xxx_write_dmp_register(state,
    2742 /* MPU6500_REG_DMP_ORIENT_TAP_EN */, buffer, 1)) != SNS_DDF_SUCCESS)
    return status;

  buffer[0] = DMP_TAP_ENABLE;
  if ((status = sns_dd_mpu6xxx_write_dmp_register(state,
      2224 /* CHANGED 4/10: MPU6500_REG_DMP_TAP_EN */, buffer, 1)) != SNS_DDF_SUCCESS)
    return status;

  // Load the Tap Thresholds
  buffer[0] = MPU6XXX_HIGH_BYTE(state->tap_settings.tap_thr_x_2);
  buffer[1] = MPU6XXX_LOW_BYTE(state->tap_settings.tap_thr_x_2);
  status = sns_dd_mpu6xxx_write_dmp_register(state, MPU6500_REG_DMP_TAP_THR_X_1, buffer, 2);
  if (status != SNS_DDF_SUCCESS)
    return status;

  buffer[0] = MPU6XXX_HIGH_BYTE(state->tap_settings.tap_thr_x_1);
  buffer[1] = MPU6XXX_LOW_BYTE(state->tap_settings.tap_thr_x_1);
  status = sns_dd_mpu6xxx_write_dmp_register(state, MPU6500_REG_DMP_TAP_THR_X_3, buffer, 2);
  if (status != SNS_DDF_SUCCESS)
    return status;

  buffer[0] = MPU6XXX_HIGH_BYTE(state->tap_settings.tap_thr_y_2);
  buffer[1] = MPU6XXX_LOW_BYTE(state->tap_settings.tap_thr_y_2);
  status = sns_dd_mpu6xxx_write_dmp_register(state, MPU6500_REG_DMP_TAP_THR_Y_1, buffer, 2);
  if (status != SNS_DDF_SUCCESS)
    return status;
  buffer[0] = MPU6XXX_HIGH_BYTE(state->tap_settings.tap_thr_y_1);
  buffer[1] = MPU6XXX_LOW_BYTE(state->tap_settings.tap_thr_y_1);
  status = sns_dd_mpu6xxx_write_dmp_register(state, MPU6500_REG_DMP_TAP_THR_Y_3, buffer, 2);
  if (status != SNS_DDF_SUCCESS)
    return status;

  buffer[0] = MPU6XXX_HIGH_BYTE(state->tap_settings.tap_thr_z_2);
  buffer[1] = MPU6XXX_LOW_BYTE(state->tap_settings.tap_thr_z_2);
  status = sns_dd_mpu6xxx_write_dmp_register(state, MPU6500_REG_DMP_TAP_THR_Z_1, buffer, 2);
  if (status != SNS_DDF_SUCCESS)
    return status;
  buffer[0] = MPU6XXX_HIGH_BYTE(state->tap_settings.tap_thr_z_1);
  buffer[1] = MPU6XXX_LOW_BYTE(state->tap_settings.tap_thr_z_1);
  status = sns_dd_mpu6xxx_write_dmp_register(state, MPU6500_REG_DMP_TAP_THR_Z_3, buffer, 2);
  if (status != SNS_DDF_SUCCESS)
    return status;

  // Enable Tap Gestures on all axes.
  buffer[0] = DMP_TAP_ALL_AXES_ENABLE;
  if ((status = sns_dd_mpu6xxx_write_dmp_register(state,
    MPU6500_REG_DMP_TAP_AXES_EN, buffer, 1)) != SNS_DDF_SUCCESS)
  return status;

  // Set the Multi-Tap Number
  buffer[0] = state->tap_settings.multi_tap_num - 1;
  status = sns_dd_mpu6xxx_write_dmp_register(state, MPU6500_REG_DMP_MULTI_TAP_SET, buffer, 1);
  if (status != SNS_DDF_SUCCESS)
    return status;

  // Load the rest of the settings
  uint16_t t_16;
  // Load the Tap Time Threshold
  // T = x / 5
  t_16 = state->tap_settings.tap_time_thr / 5;
  buffer[0] = MPU6XXX_HIGH_BYTE(t_16);
  buffer[1] = MPU6XXX_LOW_BYTE(t_16);
  status = sns_dd_mpu6xxx_write_dmp_register(state, MPU6500_REG_DMP_TAP_TIME_THR_1, buffer, 2);
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Load the Multi-Tap Time Threshold
  // T = x / 5
  t_16 = state->tap_settings.multi_tap_thr / 5;
  buffer[0] = MPU6XXX_HIGH_BYTE(t_16);
  buffer[1] = MPU6XXX_LOW_BYTE(t_16);
  status = sns_dd_mpu6xxx_write_dmp_register(state, MPU6500_REG_DMP_MULTI_TAP_THR_1, buffer, 2);
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Load the Shake Reject Threshold
  // T = ROUND_TO_32_BIT_INTEGER(x * 46850.825)
  uint32_t T_32 = 9370165; //state->tap_settings.shake_reject_thr * 46850; // This an integer approximation
  buffer[0] = (T_32 >> 24) & 0xFF;
  buffer[1] = (T_32 >> 16) & 0xFF;
  buffer[2] = (T_32 >> 8) & 0xFF;
  buffer[3] = T_32 & 0xFF;
  status = sns_dd_mpu6xxx_write_dmp_register(state, MPU6500_REG_DMP_SHAKE_REJECT_THR_1, buffer, 4);
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Load the Shake Reject Time Threshold
  // T = x / 5
  t_16 = state->tap_settings.shake_reject_time_thr / 5;
  buffer[0] = MPU6XXX_HIGH_BYTE(t_16);
  buffer[1] = MPU6XXX_LOW_BYTE(t_16);
  status = sns_dd_mpu6xxx_write_dmp_register(state, MPU6500_REG_DMP_SHAKE_REJECT_TIME_THR_1, buffer, 2);
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Load the Shake Reject Timeout Threshold
  // T = x / 5
  t_16 = state->tap_settings.shake_reject_timeout_thr / 5;
  buffer[0] = MPU6XXX_HIGH_BYTE(t_16);
  buffer[1] = MPU6XXX_LOW_BYTE(t_16);
  status = sns_dd_mpu6xxx_write_dmp_register(state, MPU6500_REG_DMP_SHAKE_REJECT_TIMEOUT_THR_1, buffer, 2);
  if (status != SNS_DDF_SUCCESS)
    return status;

  return SNS_DDF_SUCCESS;
}

/**
 *
 */
sns_ddf_status_e sns_dd_mpu6xxx_set_powerstate(
   sns_dd_mpu6xxx_s*    state,
   uint8_t              sub_dev_idx,
   sns_ddf_powerstate_e powerstate /*, boolean use_i2c*/) //
{
  uint8_t buffer = 0;
  uint8_t bytes_written, bytes_read;
  uint32_t i;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  mpu6xxx_sensor_s * sensor;

  MPU6050_MSG_2(HIGH, "mpu6xxx_set_powerstate  sub_dev: %d  powerstate: %d", sub_dev_idx, powerstate);

  // Check for null-pointers or invalid sub-device indices
  if (NULL == state || 0xFF == sub_dev_idx)
    return SNS_DDF_EINVALID_PARAM;

  mpu6xxx_sub_dev_s * sub_dev = &(state->sub_devices[sub_dev_idx]);
  if (NULL == sub_dev)
    return SNS_DDF_EINVALID_PARAM;

  // Check to see if we need to bring the device out of its slumber.
  if(!sns_dd_mpu6xxx_sensors_active(state) &&
     powerstate == SNS_DDF_POWERSTATE_ACTIVE)
  {
    status = sns_dd_mpu6xxx_set_device_powerstate(state, SNS_DDF_POWERSTATE_ACTIVE);

    if(status != SNS_DDF_SUCCESS)
       return status;
  }

  for (i = 0; i < sub_dev->num_sensors; i++)
  {
    sensor = &sub_dev->sensors[i];

    /* Change the value of the register as needed */
    switch(sensor->sensor_type)
    {
      case SNS_DDF_SENSOR_ACCEL:
      {
        /* Read the current PWR_MGMT_2 register */
        status = sns_ddf_read_port(
          state->port_handle,
          MPU6050_REG_PWR_MGMT_2,
          &buffer,
          1,
          &bytes_read);
        if(status != SNS_DDF_SUCCESS)
            return status;

        if(powerstate == SNS_DDF_POWERSTATE_ACTIVE)
        {
          buffer = ~MPU6050_ACCEL_STBY_ENABLE & buffer;
        }
        else
        {
          buffer = MPU6050_ACCEL_STBY_ENABLE | buffer;
          //sensor->desired_odr = 0;
          sensor->odr = 0;
          sensor->int_enabled = false;
          if (state->device_id == DEVICE_ID_MPU6500)
          {
            // MPU6500 accel defaults to 2g FSR
            status = sns_dd_mpu6xxx_set_config_register(state, sensor, 0, 0, FALSE);
            if (status != SNS_DDF_SUCCESS)
              return status;
            MPU6050_MSG_0(MEDIUM, "Accel FSR is set to 2g.");
          }
          else
          {
            // MPU6050 accel defaults to 2g FSR
            status = sns_dd_mpu6xxx_set_config_register(state, sensor, sensor->range, 0, FALSE);
            if (status != SNS_DDF_SUCCESS)
              return status;
            MPU6050_MSG_0(MEDIUM, "Accel FSR is set to 2g.");
                //const uint8_t default_lowpass_idx     = 4; // Gyro 98Hz, Accel 94Hz
                //const uint8_t default_gyro_range_idx  = 2; // 1000 deg/s
                //const uint8_t default_accel_range_idx = 1; // +/- 4G
          }

          sensor->num_axes = 3;
          sensor->biases[0] = 0;
          sensor->biases[1] = 0;
          sensor->biases[2] = 0;
          sensor->cur_bandwidth = 0;

          state->md_enabled = false;
          state->cycle_mode = false;
          status = sns_dd_mpu6xxx_set_odr(state, sub_dev, sensor, 0);

          if(status != SNS_DDF_SUCCESS)
             return status;
        }

        if(!sns_dd_mpu6xxx_is_dmp_active(state))
        {
          /* Write the new value to the device */
          status = sns_ddf_write_port(
              state->port_handle,
              MPU6050_REG_PWR_MGMT_2,
              &buffer,
              1,
              &bytes_written);
          if(status != SNS_DDF_SUCCESS)
              return status;
        }
        else
        {
          MPU6050_MSG_0(HIGH, "ACCESS TO PWR_MGMT_2 FORBIDDEN (DMP ACTIVE)");
          return SNS_DDF_SUCCESS;  // still need to return SUCCESS: this is
                                   // a limitation of DMP in MPU6500, not a generic error.
                                   // Alternative: add a new DDF return code to
                                   //   accommodate this. This requires SMGR to process
                                   //   this new return code correctly.
        }

        break;
      }

      case SNS_DDF_SENSOR_GYRO:
      {
        /* Read the current PWR_MGMT_2 register */
        status = sns_ddf_read_port(
          state->port_handle, MPU6050_REG_PWR_MGMT_2, &buffer, 1, &bytes_read);
        if(status != SNS_DDF_SUCCESS)
            return status;

        if(powerstate == SNS_DDF_POWERSTATE_ACTIVE)
        {
          /* needed to set MPU6050_REG_GYRO_CONFIG register */
          status = sns_dd_mpu6xxx_set_cycle_mode(state, false);

          if(status != SNS_DDF_SUCCESS)
             return status;
          buffer = ~MPU6050_GYRO_STBY_ENABLE & buffer;
        }
        else
        {
          buffer = MPU6050_GYRO_STBY_ENABLE | buffer;
          //sensor->desired_odr = 0;

          if (state->device_id == DEVICE_ID_MPU6500)
          {
            // MPU6500 gyro defaults to 2000dps FSR
            status = sns_dd_mpu6xxx_set_config_register(state, sensor, 3, 0, FALSE);
            if (status != SNS_DDF_SUCCESS)
              return status;
            MPU6050_MSG_0(MEDIUM, "Gyro FSR is set to 2000dps.");
          }
          else
          {
            // MPU6500 gyro defaults to 1000dps FSR
            status = sns_dd_mpu6xxx_set_config_register(state, sensor, sensor->range, 0, FALSE);
            if (status != SNS_DDF_SUCCESS)
              return status;
                //const uint8_t default_lowpass_idx     = 4; // Gyro 98Hz, Accel 94Hz
                //const uint8_t default_gyro_range_idx  = 2; // 1000 deg/s
                //const uint8_t default_accel_range_idx = 1; // +/- 4G
            MPU6050_MSG_0(MEDIUM, "Gyro FSR is set to 1000dps.");
          }

          status = sns_dd_mpu6xxx_set_odr(state, sub_dev, sensor, 0);
          if(status != SNS_DDF_SUCCESS)
             return status;
        }

        if(!sns_dd_mpu6xxx_is_dmp_active(state))
        {
          /* Write the new value to the device */
          status = sns_ddf_write_port(
              state->port_handle, MPU6050_REG_PWR_MGMT_2, &buffer, 1, &bytes_written);
          if(status != SNS_DDF_SUCCESS)
              return status;
        }
        else
        {
          MPU6050_MSG_0(HIGH,"ACCESS TO PWR_MGMT_2 FORBIDDEN (DMP ACTIVE)");
          return SNS_DDF_SUCCESS;  // still need to return SUCCESS: this is
                                   // a limitation of DMP in MPU6500, not a generic error.
        }
        break;
      }

      case SNS_DDF_SENSOR_TEMP:
      {
        if(powerstate == SNS_DDF_POWERSTATE_ACTIVE)
        {
          status = sns_ddf_read_port(
              state->port_handle, MPU6050_REG_PWR_MGMT_1, &buffer, 1, &bytes_read);
          if(status != SNS_DDF_SUCCESS)
              return status;

          // Set TEMP_DIS (aka PD_PTAT) to 0
          buffer = buffer & ~(TEMP_DIS_DISABLE);

          if (!sns_dd_mpu6xxx_is_dmp_active(state))
          {
            status = sns_ddf_write_port(
                state->port_handle,
                MPU6050_REG_PWR_MGMT_1,
                &buffer,
                1,
                &bytes_written);
            if(status != SNS_DDF_SUCCESS)
                return status;
          }
          else
          {
            MPU6050_MSG_0(HIGH,"ACCESS TO PWR_MGMT_1 FORBIDDEN (DMP ACTIVE)");
            return SNS_DDF_SUCCESS;  // still need to return SUCCESS: this is
                                     // a limitation of DMP in MPU6500, not a generic error.
          }
        }
        else
        {
          sensor->int_enabled = false;
          // Range, Biases, and Current Bandwidth are not supported for the TEMP sensor
          sensor->num_axes = 1;
          status = sns_dd_mpu6xxx_set_odr(state, sub_dev, sensor, 0);
          if(status != SNS_DDF_SUCCESS)
             return status;
        }
        break;
      }

      // TODO: maybe add some support for the new sensor types
      case SNS_DDF_SENSOR_DOUBLETAP:
      {
        /* Read the current USER_CTRL */
        status = sns_ddf_read_port(
          state->port_handle,
          MPU6500_REG_USER_CTRL,
          &buffer,
          1,
          &bytes_read);
        if(status != SNS_DDF_SUCCESS)
          return status;

        if(powerstate == SNS_DDF_POWERSTATE_ACTIVE)
        {
          // Not much to be done here. We currently flash the DMP image shortly after a device reset (see sns_dd_mpu6xxx_set_device_powerstate).

          MPU6050_MSG_0(MEDIUM, "mpu6xxx_set_powerstate nothing to be done for DOUBLETAP, wait for set_odr");
        }
        else
        {
          sensor->desired_odr = 0;
          sensor->int_enabled = false;
          // Range, Biases, and Current Bandwidth are not supported for the DOUBLETAP sensor
          sensor->num_axes = 1;

          status = sns_dd_mpu6xxx_set_odr(state, sub_dev, sensor, 0);
          if(status != SNS_DDF_SUCCESS)
             return status;
        }
        break;
      }

      default:
        return SNS_DDF_EINVALID_PARAM;
    }
  }

  /* Set the sub-device's power-state after dealing with all of its internal sensors */
  sub_dev->powerstate = powerstate;

  /* Check if we can bring the device down to save power */
  if(!sns_dd_mpu6xxx_sensors_active(state))
  {
      status = sns_dd_mpu6xxx_set_device_powerstate(state, SNS_DDF_POWERSTATE_LOWPOWER);
  }
  return status;
}

sns_ddf_status_e sns_dd_mpu6xxx_enable_interrupt(
  sns_dd_mpu6xxx_s*           state,
  mpu6xxx_sub_dev_s*          sub_dev,
  mpu6xxx_sensor_s*           sensor,
  sns_dd_mpu6050_interrupt_t  interrupt)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t int_enable_reg_value = 0;
  uint8_t buffer = 0, bytes_read;

  if (NULL == state || NULL == sub_dev || NULL == sensor)
    return SNS_DDF_EINVALID_PARAM;

  MPU6050_MSG_1(HIGH, "mpu6xxx_enable_interrupt for sensor %d", sensor->sensor_type);

  // Configure interrupt pin
  sns_ddf_write_port_byte(
      state->port_handle,
      MPU6050_REG_INT_PIN_CFG,
      INT_LEVEL_HIGH | LATCH_INT_EN_PULSE | INT_RD_CLEAR_ON_STATUS_REG,
      &status);

  if(status != SNS_DDF_SUCCESS)
     return status;

  // Read in the current enabled interrupts
  status = sns_ddf_read_port(
      state->port_handle,
      MPU6050_REG_INT_ENABLE,
      &int_enable_reg_value, 1,
      &bytes_read);
  if(status != SNS_DDF_SUCCESS)
     return status;

  // Configure interrupt specific parameters
  switch(interrupt)
  {
  case MPU6050_INT_MOTION_DETECT:
  {
    // Verify that the sensor is an ACCEL
    if (SNS_DDF_SENSOR_ACCEL != sensor->sensor_type)
    {
      MPU6050_MSG_1(ERROR, "mpu6xxx_enable_interrupt: Cannot enable motion-detect for sensor type %u (must be ACCEL!)", sensor->sensor_type);
      return SNS_DDF_EINVALID_PARAM;
    }

    // If the device already has the MD interrupt enabled, then no more work needs to be done.
    if ((int_enable_reg_value & MOT_EN_ENABLE) && state->md_enabled)
    {
      MPU6050_MSG_0(LOW, "mpu6xxx_enable_interrupt: Motion Detect interrupt already enabled");
      return status;
    }

    // Disables cycle mode
    status = sns_dd_mpu6xxx_set_cycle_mode(state, false);
    if(status != SNS_DDF_SUCCESS)
       return status;

    // Reads MPU6050_REG_PWR_MGMT_2 and disables accel standby mode
    status = sns_ddf_read_port(
                state->port_handle,
                MPU6050_REG_PWR_MGMT_2,
                &buffer, 1,
                &bytes_read);
    if(status != SNS_DDF_SUCCESS)
    {
       return status;
    }

    // If ACCEL is in standby, bring it out of standby
    if ((buffer & MPU6050_ACCEL_STBY_ENABLE) != MPU6050_ACCEL_STBY_ENABLE)
    {
      buffer &= ~(MPU6050_ACCEL_STBY_ENABLE);

      // Only allow modifications to the PWR_MGMT set of registers when the DMP is inactive
      if (!sns_dd_mpu6xxx_is_dmp_active(state))
      {
        sns_ddf_write_port_byte(
            state->port_handle,
            MPU6050_REG_PWR_MGMT_2,
            buffer,
            &status);
        if(status != SNS_DDF_SUCCESS)
           return status;
      }
      else
      {
        MPU6050_MSG_0(HIGH,"ACCESS TO PWR_MGMT_2 FORBIDDEN (DMP ACTIVE)");
        return SNS_DDF_SUCCESS; //SNS_DDF_EINVALID_PARAM;
      }
    }

    // Clears HPF
    if (state->device_id == DEVICE_ID_MPU6500)
    {
      // If the device is an MPU6500, keep the FSR at 2g
      status = sns_dd_mpu6xxx_set_config_register(state, sensor, 0,
                                         ACCEL_HPF_None, FALSE);
    }
    else
    {
      status = sns_dd_mpu6xxx_set_config_register(state, sensor, sensor->range,
                                         ACCEL_HPF_None, FALSE);
    }
    if(status != SNS_DDF_SUCCESS)
       return status;

    // Sets LPF
    status = sns_dd_mpu6xxx_set_lowpass(state, 6);   // 260Hz
    if(status != SNS_DDF_SUCCESS)
       return status;

    // Enable the interrupt(s)
    int_enable_reg_value |= MOT_EN_ENABLE;

    break;
  }

  case MPU6050_INT_DATA_READY:
  {
    MPU6050_MSG_0(HIGH, "mpu6xxx_enable_interrupt ENABLING DRI");
    sensor->int_enabled = TRUE;

    // If the device already has DRI enabled, then no more work needs to be done.
    if (int_enable_reg_value & DATA_RDY_EN_ENABLE)
      return status;

    int_enable_reg_value |= DATA_RDY_EN_ENABLE;

    break;
  }
  case MPU6500_INT_DMP:
  {
    MPU6050_MSG_0(HIGH, "mpu6xxx_enable_interrupt ENABLING DMP INTERRUPT");

    if (state->device_id == DEVICE_ID_MPU6500)
    {
      // No need to do any setup here, DMP setup is taken care of in sns_dd_mpu6500_setup_dmp.
      int_enable_reg_value |= DMP_DATA_RDY_EN_ENABLE;
    }
    else
    {
      MPU6050_MSG_1(ERROR, "mpu6xxx_enable_interrupt: DMP interrupt is only supported for the MPU6500 (this device has the ID of 0x%X)", state->device_id);
      return SNS_DDF_EINVALID_PARAM;
    }
    break;
  }

  // TODO: add support for any newly added interrupts

  default:
      return SNS_DDF_EINVALID_PARAM;
  }

  // Register to receive interrupt notifications
  status = sns_ddf_signal_register(
      sub_dev->int_gpio,
      state,
      &sns_dd_mpu6xxx_if,
      SNS_DDF_SIGNAL_IRQ_RISING);
  if(status != SNS_DDF_SUCCESS)
     return status;

  // Enable interrupt
  sns_ddf_write_port_byte(
      state->port_handle,
      MPU6050_REG_INT_ENABLE,
      int_enable_reg_value,
      &status);
  if(status != SNS_DDF_SUCCESS)
     return status;

  MPU6050_MSG_3(HIGH, "enable_int %d int=%d reg_val=0x%X",
                9916, interrupt, int_enable_reg_value);

  if(interrupt == MPU6050_INT_MOTION_DETECT)
  {
    const uint8_t threshold_6500 = 8; // 1 lsb = 4 milli-g
    const uint8_t threshold_6050 = 1;  // 1 lsb == 32 milli-g
    const uint8_t duration = 1;   // == 1 millisec

    if (state->device_id == DEVICE_ID_MPU6050)
    {
      // Set duration
      sns_ddf_write_port_byte(
          state->port_handle, MPU6050_REG_MOT_DUR, duration, &status);
      if(status != SNS_DDF_SUCCESS)
         return status;

      // Set threshold (Forcing internal state of MPU6050 to REST)
      sns_ddf_write_port_byte(
          state->port_handle, MPU6050_REG_MOT_THR, 255, &status);
      if(status != SNS_DDF_SUCCESS)
         return status;

      // Intended delay : 1 msec (1000 usec)
      sns_ddf_delay(1000);

      // Set threshold (desired value)
      sns_ddf_write_port_byte(
          state->port_handle, MPU6050_REG_MOT_THR, threshold_6050, &status);
      if(status != SNS_DDF_SUCCESS)
         return status;
    }
    // TODO: maybe add support? motion-detect does not seem to be high priority
    else if (state->device_id == DEVICE_ID_MPU6500)
    {
      sns_ddf_write_port_byte(
          state->port_handle, MPU6500_REG_MOT_DETECT_CTR,
          WOM_ACCEL_INTEL_ENABLE_MPU6050_SAMPLE_MODE, &status);
      //TO 0X69 reg, enable w o motion with mpu6050 mode -
      //every sample compared to original sample


      // Set threshold (Forcing internal state of MPU6500 to REST)
      sns_ddf_write_port_byte(
          state->port_handle, MPU6500_REG_MOT_THR, 255, &status);
      if(status != SNS_DDF_SUCCESS)
        return status;

      // Intended delay : 1 msec (1000 usec)
      sns_ddf_delay(1000);

      // Set threshold (desired value)
      sns_ddf_write_port_byte(
          state->port_handle, MPU6050_REG_MOT_THR, threshold_6500, &status);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }

    // Configures HPF
    if (state->device_id == DEVICE_ID_MPU6500)
    {
      // If the device is an MPU6500, keep the FSR at 2g
      status = sns_dd_mpu6xxx_set_config_register(state, sensor, 0,
          ACCEL_HPF_0_63HZ, FALSE);
    }
    else
    {
      status = sns_dd_mpu6xxx_set_config_register(state, sensor, sensor->range,
          ACCEL_HPF_0_63HZ, FALSE);
    }

    state->md_enabled = TRUE;
  }

  return status;
}

sns_ddf_status_e sns_dd_mpu6xxx_disable_interrupt(
    sns_dd_mpu6xxx_s*           state,
    mpu6xxx_sub_dev_s*          sub_dev,
    mpu6xxx_sensor_s*           sensor,
    sns_dd_mpu6050_interrupt_t  interrupt)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t int_disable_reg_value = 0;
  uint8_t bytes_read;

  uint8_t i;
  uint32_t j;
//  bool dereg_pin = true;
  mpu6xxx_sub_dev_s* sub_dev_iter;
  mpu6xxx_sensor_s* sensor_iter;

  /* read the current MPU6050_REG_INT_ENABLE */
  status = sns_ddf_read_port(
    state->port_handle,
    MPU6050_REG_INT_ENABLE,
    &int_disable_reg_value,
    1,
    &bytes_read);

  if(status != SNS_DDF_SUCCESS)
     return status;

  switch(interrupt)
  {
    case MPU6050_INT_DATA_READY:
      // Disable the DRI flag for this particular sensor
      sensor->int_enabled = FALSE;

      /* If other sensors (except DOUBLETAP) are in DRI-mode, then keep DRI
         enabled for the entire device */
      for (i = 0; i < state->sub_device_cnt; i++)
      {
        sub_dev_iter = &state->sub_devices[i];
        for (j = 0; j < sub_dev_iter->num_sensors; j++)
        {
          sensor_iter = &sub_dev_iter->sensors[j];
          /* If the sensor is in DRI-mode and is not a DOUBLETAP sensor, then
           * exit out of the function. */
          if ((sensor_iter->int_enabled) &&
              (sensor_iter->sensor_type != SNS_DDF_SENSOR_DOUBLETAP))
            return status;
        }
      }
      int_disable_reg_value &= ~DATA_RDY_EN_ENABLE;
      break;

    case MPU6050_INT_MOTION_DETECT:
      int_disable_reg_value &= ~MOT_EN_ENABLE;
      state->md_enabled = FALSE;

      // If the device already has the MD interrupt disabled, then no more work needs to be done.
      if (int_disable_reg_value & DMP_DATA_RDY_EN_ENABLE)
        return SNS_DDF_SUCCESS;

      break;

    case MPU6500_INT_DMP:
      // No need to worry about disabling other portions of the DMP,
      // that is taken care of in sns_dd_mpu6xxx_set_powerstate
      int_disable_reg_value &= ~DMP_DATA_RDY_EN_ENABLE;
      break;

    // TODO: maybe add support for newly added interrupts
    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  sns_ddf_write_port_byte(
     state->port_handle, MPU6050_REG_INT_ENABLE, int_disable_reg_value, &status);
  if(status != SNS_DDF_SUCCESS)
     return status;

  MPU6050_MSG_3(HIGH, "disable_int %d int=%d reg_val=%d",
                9916, interrupt, int_disable_reg_value);

#if 0
  // Determines if interrupt pin can be de-registered.
  for(i = 0; i < state->sub_device_cnt; i++)
  {
    sub_dev_iter = &state->sub_devices[i];
    for(j = 0; j < sub_dev_iter->num_sensors && dereg_pin; j++)
    {
      sensor_iter = &sub_dev_iter->sensors[j];
      if( sensor_iter->int_enabled ||
         (sensor_iter->sensor_type == SNS_DDF_SENSOR_DOUBLETAP) )
      {
        dereg_pin = false;
        break;
      }
    }
  }

  if(dereg_pin)
  {
     status = sns_ddf_signal_deregister(sub_devices->int_gpio);
  }
#endif

  return status;
}


// Note: This function is called by the SMGR and helps it route IRQ flags.
sns_ddf_status_e sns_dd_mpu6xxx_enable_sched_data(
    sns_ddf_handle_t  handle,
    sns_ddf_sensor_e  sensor_type,
    bool              enable)
{
  sns_dd_mpu6xxx_s* state = sns_dd_mpu6xxx_get_device(handle);
  mpu6xxx_sub_dev_s* sub_dev = sns_dd_mpu6xxx_get_sub_dev(handle);
  mpu6xxx_sensor_s* sensor;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t i;
  uint8_t buffer, bytes_read;

  MPU6050_MSG_3(HIGH, "mpu6xxx_enable_sched_data for sensor %d  dd_handle: 0x%x, true? %d", sensor_type, handle, enable);

  if (NULL == state || NULL == sub_dev)
    return SNS_DDF_EINVALID_PARAM;

  status = sns_ddf_read_port(
      state->port_handle,
      MPU6050_REG_INT_ENABLE,
      &buffer,1,
      &bytes_read);
  if(status != SNS_DDF_SUCCESS)
     return status;
  MPU6050_MSG_2(MEDIUM, "mpu6xxx enable sched data read 0x%x from the int enable register 0x%x", buffer, MPU6050_REG_INT_ENABLE);
  status = sns_ddf_read_port(
      state->port_handle,
      MPU6050_REG_INT_STATUS,
      &buffer,1,
      &bytes_read);
  if(status != SNS_DDF_SUCCESS)
     return status;
  MPU6050_MSG_2(MEDIUM, "mpu6xxx enable sched data read 0x%x from the int status register 0x%x", buffer, MPU6050_REG_INT_STATUS);

  // Do nothing if the sensor type is NONE
  if(SNS_DDF_SENSOR__NONE == sensor_type)
  {
    MPU6050_MSG_0(HIGH, "mpu6xxx_enable_sched_data nothing to do for sensor type NONE");
    return SNS_DDF_SUCCESS;
  }
  // The function will call itself specifying which sensors to use
  if(SNS_DDF_SENSOR__ALL == sensor_type)
  {
    for (i = 0; i < sub_dev->num_sensors; i++)
    {
      status = sns_dd_mpu6xxx_enable_sched_data(handle, sub_dev->sensors[i].sensor_type, enable);
      if(status != SNS_DDF_SUCCESS)
         return status;
    }
    return status;
  }

  sensor = sns_dd_mpu6xxx_get_sensor(handle, sensor_type);
  /* If the sensor type is not found in the sub-device associated with the
     handle, then report an error */
  if (NULL == sensor)
  {
      return SNS_DDF_EINVALID_PARAM;
  }

  // Note: DRI currently does not affect the DOUBLETAP in MPU6500 since it is self-scheduled
  if(SNS_DDF_SENSOR_DOUBLETAP == sensor_type)
  {
    if(enable)
      sensor->int_enabled = true;
    else
      sensor->int_enabled = false;
    return SNS_DDF_SUCCESS;
  }

  if(enable)
  {
    /* Don't enable DRI interrupt if it is in motion detection mode */
    if ( (SNS_DDF_SENSOR_ACCEL != sensor_type) || (FALSE == state->md_enabled) )
    {
      status = sns_dd_mpu6xxx_enable_interrupt(state, sub_dev, sensor, MPU6050_INT_DATA_READY);
      if(status != SNS_DDF_SUCCESS)
         return status;
    }
    MPU6050_MSG_0(HIGH, "mpu6xxx_enable_sched_data: MD and DRI are NOT concurrent");
  }
  else
  {
    status = sns_dd_mpu6xxx_disable_interrupt(state, sub_dev, sensor, MPU6050_INT_DATA_READY);
    if(status != SNS_DDF_SUCCESS)
       return status;
  }

  return status;
}


/**
 * @brief Configure the low pass filter bandwidth
 *
 * NB: The accel and gyro LPF are configured together and therefore modifying
 * one always modifies the other as well. See MPU-6000/MPU-6050 Register Map and
 * Descriptions, section 4.3 for details.
 *
 * @param[in] state      Ptr to the driver structure
 * @param[in] lpf_index  Index to low pass filter bandwidth value
 *
 * @return Success status of this operation.
 */
sns_ddf_status_e sns_dd_mpu6xxx_set_lowpass(
   sns_dd_mpu6xxx_s* state, uint8_t lpf_index)
{
    sns_ddf_status_e status;
    uint8_t bytes_written;
    uint8_t buffer;

    // Since accel and gyro LPFs are linked, it doesn't matter if we use the
    // gyro table or accel table. Suppress compiler warning re: an unused
    // variable. We keep this definition around for reference.
    (void)sns_dd_mpu6050_gyro_lpf_reg;

    // Do not set the lowpass while DOUBLETAP is running
    if (sns_dd_mpu6xxx_is_dmp_active(state))
    {
      MPU6050_MSG_1(ERROR, "mpu6xxx_set_lowpass: Cannot set the lowpass for the device instance 0x%X while the DMP is active", state);
      return SNS_DDF_SUCCESS; //return SNS_DDF_EINVALID_PARAM;
    }

    if(lpf_index >= MPU6050_TABLE_SIZE(sns_dd_mpu6050_accel_lpf_reg))
       return SNS_DDF_EINVALID_PARAM;

    buffer = sns_dd_mpu6050_accel_lpf_reg[lpf_index];

    status = sns_ddf_write_port(
        state->port_handle, MPU6050_REG_CONFIG, &buffer, 1, &bytes_written);
    if(status != SNS_DDF_SUCCESS)
        return status;


    state->lpf_index = lpf_index;
    return status;
}

/**
 * @brief sns_dd_mpu6xxx_set_cycle_mode
 *
 * @param[in] state      Ptr to the driver structure
 * @param[in] enable     true to enable, false to disable
 *
 * @return Success status of this operation.
 */
static sns_ddf_status_e sns_dd_mpu6xxx_set_cycle_mode(
    sns_dd_mpu6xxx_s* state, bool enable )
{
  uint8_t buffer = 0;
  uint8_t bytes_written, bytes_read;
  sns_ddf_status_e status;

  MPU6050_MSG_1(HIGH, "mpu6xxx_set_cycle_mode enabling? %d", enable);

  // Check for null-pointers
  if (NULL == state)
    return SNS_DDF_EINVALID_PARAM;

  if (sns_dd_mpu6xxx_is_dmp_active(state))
  {
    MPU6050_MSG_0(HIGH,"ACCESS TO PWR_MGMT_1 AND CONFIG FORBIDDEN (DMP ACTIVE)");
    return SNS_DDF_SUCCESS;
  }

  if (state->cycle_mode == enable )
  {
      return SNS_DDF_SUCCESS;
  }


  if( enable )
  {
      /* clear DLPF for cyclic mode. It is required */
      buffer = 0;
      status = sns_ddf_write_port(
        state->port_handle,
        MPU6050_REG_CONFIG,
        &buffer,
        1,
        &bytes_written);

      if(status != SNS_DDF_SUCCESS)
          return status;

      // Zero-out the current bandwidth for all of the sensors
      uint8_t i;
      uint32_t j;
      for (i = 0; i < state->sub_device_cnt; i++)
      {
        mpu6xxx_sub_dev_s * sub_device = &(state->sub_devices[i]);
        for (j = 0; j < sub_device->num_sensors; j++)
        {
          sub_device->sensors[j].cur_bandwidth = 0;
        }
      }

  }

  /* read the current MPU6050_REG_PWR_MGMT_1 */
  status = sns_ddf_read_port(
      state->port_handle,
      MPU6050_REG_PWR_MGMT_1,
      &buffer,
      1,
      &bytes_read);

  if(status != SNS_DDF_SUCCESS)
      return status;

  if( enable )
  {
      buffer |= CYCLE_ENABLE;
      buffer &= ~SLEEP_ENABLE;
  }
  else
  {
      buffer &= ~CYCLE_ENABLE;
      buffer &= ~SLEEP_ENABLE;
  }
  status = sns_ddf_write_port(
        state->port_handle,
        MPU6050_REG_PWR_MGMT_1,
        &buffer,
        1,
        &bytes_written);

  MPU6050_MSG_3(HIGH, "set_cycle_mode %d enable=%d buffer=%d",
                9903, enable, buffer);
  if( status == SNS_DDF_SUCCESS)
  {
      state->cycle_mode = enable;
  }
  return status;
}

/**
 * @brief sns_dd_mpu6500_set_lp_accel_odr
 *
 * @param[in] state      Ptr to the driver structure
 * @param[in] lp_accel_odr  wake up control for wake up frequency
 *
 * @return Success status of this operation.
 */
static sns_ddf_status_e sns_dd_mpu6500_set_lp_accel_odr(
    sns_dd_mpu6xxx_s* state, uint8_t lp_accel_odr  )
{
  uint8_t buffer;
  uint8_t bytes_written;
  sns_ddf_status_e status;

  buffer = lp_accel_odr; /*//assign the input frequency to REG 1E*/
  status = sns_ddf_write_port(
    state->port_handle,
    MPU6500_REG_LP_ACCEL_ODR,
    &buffer,
    1,
    &bytes_written);

  MPU6050_MSG_1(LOW, "mpu6500_set_lp_accel_odr: Set LP ACCEL ODR to %u", lp_accel_odr);

  return status;
}

/**
 * @brief sns_dd_mpu6xxx_set_wakeup_control
 *
 * @param[in] state      Ptr to the driver structure
 * @param[in] wkup_ctrl  wake up control for wake up frequency
 *
 * @return Success status of this operation.
 */
static sns_ddf_status_e sns_dd_mpu6xxx_set_wakeup_control(
    sns_dd_mpu6xxx_s* state, uint8_t wkup_ctrl  )
{
  uint8_t buffer = 0;
  uint8_t bytes_written, bytes_read;
  sns_ddf_status_e status;

  if (sns_dd_mpu6xxx_is_dmp_active(state))
  {
    MPU6050_MSG_0(HIGH,"ACCESS TO PWR_MGMT_2 FORBIDDEN (DMP ACTIVE)");
    return SNS_DDF_SUCCESS; //SNS_DDF_EINVALID_PARAM;
  }

  /* read the current MPU6050_REG_PWR_MGMT_2 */
    status = sns_ddf_read_port(
        state->port_handle,
        MPU6050_REG_PWR_MGMT_2,
        &buffer,
        1,
        &bytes_read);

    if(status != SNS_DDF_SUCCESS)
        return status;

    buffer = (buffer & ~LP_WAKE_CTRL_FREQ_40HZ) | wkup_ctrl;
    MPU6050_MSG_3(HIGH, "set_wakeup %d ctrl=%d buffer=%d", 9900, wkup_ctrl, buffer);

    status = sns_ddf_write_port(
        state->port_handle,
        MPU6050_REG_PWR_MGMT_2,
        &buffer,
        1,
        &bytes_written);

    return status;
}

sns_ddf_status_e sns_dd_mpu6xxx_enable_dmp (sns_dd_mpu6xxx_s* state, mpu6xxx_sub_dev_s* sub_dev,
    mpu6xxx_sensor_s* sensor)
{
  sns_ddf_status_e status;
  uint8_t buffer[2];
  uint8_t bytes_written;

  // Hold all Advanced-Features under reset
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_USER_CTRL, 0x0C, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Delay 50 milliseconds
  sns_ddf_delay(50000);

  /* MPU Setup for Enabling DMP (see Chapter 7 of MPU-6500 Register Map) */
  // Configure Power Management Registers
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_PWR_MGMT_1, 0x00, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_PWR_MGMT_2, 0x00, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Configure Gyroscope Parameters
  sns_ddf_write_port_byte(state->port_handle, MPU6500_REG_CONFIG, 0x03, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_0(MEDIUM, "Digital Low-Pass Frequency is set to 42Hz.");
  sns_ddf_write_port_byte(state->port_handle, MPU6500_REG_GYRO_CONFIG, 0x18, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_0(MEDIUM, "Gyro FSR is set to 2000dps.");
  // Configure Accelerometer Parameters
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_ACCEL_CONFIG, 0x00, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_0(MEDIUM, "Accel FSR is set to 2g.");
  sns_ddf_write_port_byte(state->port_handle, MPU6500_REG_ACCEL_CONFIG_2, 0x40, &status);  // Set the FIFO size to 1K
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Configure FIFO and Interrupts
  sns_ddf_write_port_byte(state->port_handle, MPU6500_REG_FIFO_EN, 0x00, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  sns_ddf_write_port_byte(state->port_handle, MPU6500_REG_INT_ENABLE, 0x00, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Configure Sensor Sample Rate - Sets the ACCEL, GYRO, TEMP ODR to 200Hz
  sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_SMPRT_DIV, 0x04, &status);
  if (status != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_0(MEDIUM, "Sensor Sample Rate is set to 200 Hz.");

  // Load firmware start value (DMP_START_ADDRESS) to start the DMP
  buffer[0] = MPU6XXX_HIGH_BYTE(DMP_START_ADDRESS);
  buffer[1] = MPU6XXX_LOW_BYTE(DMP_START_ADDRESS);
  status = sns_ddf_write_port(
    state->port_handle,
    MPU6XXX_REG_FW_START_ADDR,
    buffer,
    2,
    &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_0(MEDIUM, "mpu6xxx_set_odr: loaded DMP start address");

  /* Enable DMP and FIFO
   * See Chapter 9 of MPU-6500 Register Map and Descriptions */
  buffer[0] = DMP_EN_ENABLED | FIFO_EN_ENABLED;
  status = sns_ddf_write_port(
    state->port_handle,
    MPU6500_REG_USER_CTRL,
    buffer,
    1,
    &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_0(MEDIUM, "DMP is now running.");

  // Enable the DMP interrupt
  MPU6050_MSG_0(MEDIUM, "Enabling the DMP interrupt...");
  status = sns_dd_mpu6xxx_enable_interrupt(state, sub_dev, sensor, MPU6500_INT_DMP);
  if(status != SNS_DDF_SUCCESS)
    return status;
  MPU6050_MSG_0(MEDIUM, "DMP interrupt enabled.");

  return status;
}

sns_ddf_status_e sns_dd_mpu6xxx_set_dmp_odr (
    sns_dd_mpu6xxx_s* state, mpu6xxx_sub_dev_s* sub_dev,
    mpu6xxx_sensor_s* sensor, uint32_t odr)
{
  sns_ddf_status_e status;
  uint16_t n;
  uint8_t n_byte_array[2];
  uint8_t bytes_written;

  // Deactivate DOUBLETAP output
  if (odr == 0)
  {
    status = sns_dd_mpu6xxx_disable_interrupt(state, sub_dev, sensor, MPU6500_INT_DMP);
    if (status != SNS_DDF_SUCCESS)
      return status;

    // Zero out the ODR
    sensor->odr = 0;

    // Put the FIFO and DMP into reset
    n_byte_array[0] = FIFO_RST_RESET | DMP_RST_RESET;
    status = sns_ddf_write_port(
      state->port_handle,
      MPU6500_REG_USER_CTRL,
      n_byte_array,
      1,
      &bytes_written);
    if(status != SNS_DDF_SUCCESS)
      return status;

    return status;
  }

  /* Doubletap_odr = 200 / (n + 1)
   * where N is the FIFO rate divider (0x216 and 0x217).
   *
   * n = floor(200 / Doubletap_odr) - 1
   * This will give a true ODR equivalent or higher to the desired odr.
   */
  n = (MPU6500_DOUBLETAP_SMPLRT / odr) - 1;
  n_byte_array[0] = MPU6XXX_HIGH_BYTE(n);
  n_byte_array[1] = MPU6XXX_LOW_BYTE(n);

  sensor->odr = MPU6500_DOUBLETAP_SMPLRT / (n + 1);

  MPU6050_MSG_1(HIGH, "mpu6xxx set dmp odr - setting FIFO divider rate to %d", n);
  status = sns_dd_mpu6xxx_write_dmp_register(state,
      MPU6500_REG_DMP_FIFO_RATE_DIV_H, n_byte_array, 2);
  if (status != SNS_DDF_SUCCESS)
    return status;
  // Enable FIFO Rate Divider - THIS IS REQUIRED!!!
  status = sns_dd_mpu6xxx_write_dmp_register(state, 0xAC1, sns_dd_mpu6500_fifo_rate_div_en, 12);
  if (status != SNS_DDF_SUCCESS)
    return status;

  return status;
}

#if 0
/**
 * @brief mpu6050_is_equal_reg_val
 *
 * @param[in] state     A pointer to the driver structure
 * @param[in] reg_addr  The register address
 * @param[in] val       The input value to compare
 *
 * @return true  when the register value is the same as input value.
 *         false when the register value is not the same as input value.
 */
static uint8_t mpu6050_is_equal_reg_val (sns_dd_mpu6xxx_s* state, uint8_t reg_addr, uint8_t val)
{
  uint8_t buffer;
  uint8_t bytes_read;
  sns_ddf_status_e status;
  status = sns_ddf_read_port(
      state->port_handle,
      reg_addr,
      &buffer,
      1,
      &bytes_read);
  if(status == SNS_DDF_SUCCESS && (buffer == val))
  {
    return true;
  }
  else
  {
    return false;
  }
}
#endif

sns_ddf_status_e sns_dd_mpu6xxx_set_odr(
    sns_dd_mpu6xxx_s* state, mpu6xxx_sub_dev_s* sub_dev,
    mpu6xxx_sensor_s* sensor, uint32_t odr)
{
  q16_t           req_bandwidth;
  const q16_t*    bandwidth_table = sns_dd_mpu6050_gyro_lpf;
  const uint32_t  bandwidth_table_size = MPU6050_TABLE_SIZE(sns_dd_mpu6050_gyro_lpf);

  q16_t     actual_bandwidth = 0;
  uint32_t  actual_odr, req_odr_max;
  uint8_t   lpf_index;
  uint16_t  gyro_output_rate;
  uint16_t  smprt_div;

//  uint8_t bytes_written, bytes_read;
//  uint8_t buffer[2];

  sns_ddf_sensor_e sensor_type = sensor->sensor_type;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  uint32_t /*accel_desired_odr,*/accel_odr;
  uint32_t gyro_desired_odr, gyro_odr;
  uint32_t temp_desired_odr, temp_odr;

  mpu6xxx_sub_dev_s* accel_sub_dev;
  mpu6xxx_sub_dev_s* gyro_sub_dev;
  mpu6xxx_sub_dev_s* temp_sub_dev;

  mpu6xxx_sensor_s* accel_sensor = sns_dd_mpu6xxx_search_for_sensor(state, SNS_DDF_SENSOR_ACCEL, &accel_sub_dev);
  mpu6xxx_sensor_s* gyro_sensor = sns_dd_mpu6xxx_search_for_sensor(state, SNS_DDF_SENSOR_GYRO, &gyro_sub_dev);
  mpu6xxx_sensor_s* temp_sensor = sns_dd_mpu6xxx_search_for_sensor(state, SNS_DDF_SENSOR_TEMP, &temp_sub_dev);

  MPU6050_MSG_2(HIGH, "mpu6xxx set odr  desired_odr: %d  sensor: %d", odr, sensor->sensor_type);

  // Set the desired odr before filling in local accel and gyro data
  sensor->desired_odr = odr;

  // Obtain pointers to the ACCEL, GYRO, and TEMP sensors within the device
  if (NULL == accel_sensor)
  {
    // Do not consider the ACCEL
    /*accel_desired_odr = 0;*/
    accel_odr = 0;
  }
  else
  {
    /*accel_desired_odr = accel_sensor->desired_odr;*/
    accel_odr = accel_sensor->odr;
  }

  if (NULL == gyro_sensor)
  {
    // Do not consider the GYRO
    gyro_desired_odr = 0;
    gyro_odr = 0;
  }
  else
  {
    gyro_desired_odr = gyro_sensor->desired_odr;
    gyro_odr = gyro_sensor->odr;
  }

  if (NULL == temp_sensor)
  {
    // Do not consider the TEMP
    temp_desired_odr = 0;
    temp_odr = 0;
  }
  else
  {
    temp_desired_odr = temp_sensor->desired_odr;
    temp_odr = temp_sensor->odr;
  }

  // TODO: need to use DOUBLETAP to control DMP (instead of the other new sensor types)

  /* DOUBLETAP ODR Procedures */
  if (SNS_DDF_SENSOR_DOUBLETAP == sensor_type)
  {
    MPU6050_MSG_0(HIGH, "setting odr for DOUBLETAP");

    // Max ODR, as physically limited by the device, is 200 Hz
    if (odr > MPU6500_DOUBLETAP_MAX_ODR)
    {
      sensor->desired_odr = MPU6500_DOUBLETAP_MAX_ODR;
    }

    if (sensor->desired_odr != 0)
    {
      MPU6050_MSG_2(HIGH, "odr_changed event? accel_sensor=%u, gyro_sensor=%u", accel_sensor, gyro_sensor);

      /* Set the accel and gyro odr's to 200Hz (if possible) */
      if (accel_sensor != NULL)
      {
        accel_sensor->odr = MPU6500_DOUBLETAP_SMPLRT;

        MPU6050_MSG_1(HIGH, "accel desired_odr=%u", accel_sensor->desired_odr);
        if (accel_sensor->desired_odr != 0)
        {
          // Notify the SMGR of an ODR Changed Event
          sns_ddf_smgr_notify_event(accel_sub_dev->smgr_handle,
              SNS_DDF_SENSOR_ACCEL,
              SNS_DDF_EVENT_ODR_CHANGED);
        }
      }
      if (gyro_sensor != NULL)
      {
        gyro_sensor->odr = MPU6500_DOUBLETAP_SMPLRT;

        MPU6050_MSG_1(HIGH, "gyro desired_odr=%u", gyro_sensor->desired_odr);
        if (gyro_sensor->desired_odr != 0)
        {
          // Notify the SMGR of an ODR Changed Event
          sns_ddf_smgr_notify_event(gyro_sub_dev->smgr_handle,
              SNS_DDF_SENSOR_GYRO,
              SNS_DDF_EVENT_ODR_CHANGED);
        }
      }

      // Set the ODR for the DMP
      status = sns_dd_mpu6xxx_set_dmp_odr(state, sub_dev, sensor, odr);
      if (status != SNS_DDF_SUCCESS)
        return status;

      // Enable the DMP
      status = sns_dd_mpu6xxx_enable_dmp(state, sub_dev, sensor);
      return status;
    }
    else
    {
      MPU6050_MSG_0(MEDIUM, "Disabling the DMP interrupt...");

      status = sns_dd_mpu6xxx_set_dmp_odr(state, sub_dev, sensor, odr);
      if (status != SNS_DDF_SUCCESS)
        return status;

      MPU6050_MSG_0(MEDIUM, "DMP interrupt disabled.");

      sensor->odr = odr;
    }

    return status;
  }

  /* ACCEL, GYRO, and TEMP ODR Procedures */
  // Find the max ODR
  req_odr_max = sns_dd_mpu6xxx_find_max_desired_odr(state);

  MPU6050_MSG_2(HIGH, "set_odr req_odr_max=%d odr=%d", req_odr_max, odr);

  // Configure ODR if DOUBLETAP is not active. Otherwise the SMPRT_DIV should be controlled by the DMP.
  if (sns_dd_mpu6xxx_is_dmp_active(state))
  {
    MPU6050_MSG_2(HIGH, "mpu6xxx_set_odr: Cannot set the odr of sensor type %d to %d Hz because the DMP is running.", sensor->sensor_type, sensor->desired_odr);
    sensor->odr = 200;
    MPU6050_MSG_0(HIGH, "The sensor sample rate remains at 200 Hz.");
    return SNS_DDF_SUCCESS;
  }

  // If all sensors (except DOUBLETAP) have a desired odr of 0 AND motion-detect is not enabled
  // Then zero-out all (except DOUBLETAP) odr's
  if (0 == req_odr_max && !(state->md_enabled))
  {
    MPU6050_MSG_0(LOW, "mpu6xxx clearing own odr");
    sensor->odr = 0;
  }
  // If motion-detect is enabled, setup low-power settings
  else if ( (0 == gyro_desired_odr) &&
            (0 == temp_desired_odr) &&
            (state->md_enabled) )
  {
    MPU6050_MSG_0(LOW, "mpu6xxx_set_odr setup low-power settings");
    if (state->device_id == DEVICE_ID_MPU6050)
    {
      status = sns_dd_mpu6xxx_set_wakeup_control(state, LP_WAKE_CTRL_FREQ_20HZ);
      if(status != SNS_DDF_SUCCESS)
          return status;
      accel_odr = 20;
    }

    else// if (state->device_id == DEVICE_ID_MPU6500)
    {
      status =  sns_dd_mpu6500_set_lp_accel_odr(state, MPU6500_LP_ACCEL_ODR_FREQ_62_50HZ);
      if(status != SNS_DDF_SUCCESS)
          return status;
      accel_odr = 63; // 63 = ROUND(62.50)
    }

    gyro_odr = 0;
    temp_odr = 0;
    // Push the proper odr values back into the right places
    if (accel_sensor != NULL)
    {
      accel_sensor->odr = accel_odr;
    }
    if (gyro_sensor != NULL)
    {
      gyro_sensor->odr = gyro_odr;  // Set to zero for now... TODO: CONSIDER SYNCING GYRO_ODR AND ACCEL_ODR
    }
    if (temp_sensor != NULL)
    {
      temp_sensor->odr = temp_odr;
    }
    MPU6050_MSG_0(LOW, "Attempting to enable Cycle-Mode...");
    status=sns_dd_mpu6xxx_set_cycle_mode(state, true);
    if(status != SNS_DDF_SUCCESS)
        return status;
  }
  // TODO: MAYBE WE CAN ADJUST THIS SO ACCEL IS PUT INTO LOW-POWER MODE WHEN ITS desired_odr <= 5
  else if ( (state->device_id == DEVICE_ID_MPU6050) &&
            (0 == temp_desired_odr) &&
            (0 == gyro_desired_odr) &&
            (req_odr_max <= MPU6050_CYCLE_MODE_LIMIT_HZ) )
  {
    gyro_odr = 0;

    if ( req_odr_max <= 1)
    {
      status = sns_dd_mpu6xxx_set_wakeup_control(state, LP_WAKE_CTRL_FREQ_1_25HZ);
      accel_odr = 1;
    }
    else if( req_odr_max <= 5 )
    {
      status = sns_dd_mpu6xxx_set_wakeup_control(state, LP_WAKE_CTRL_FREQ_5HZ);
      accel_odr = 5;
    }
    else if (req_odr_max <= 10 )
    {
      status = sns_dd_mpu6xxx_set_wakeup_control(state, LP_WAKE_CTRL_FREQ_20HZ);
      accel_odr = 10;
    }
    else if (req_odr_max <= 40 )
    {
      status = sns_dd_mpu6xxx_set_wakeup_control(state, LP_WAKE_CTRL_FREQ_40HZ);
      accel_odr = 40;
    }

    if (status == SNS_DDF_SUCCESS)
    {
      // Push the proper odr values back into the right places
      if (accel_sensor != NULL)
      {
        accel_sensor->odr = accel_odr;
      }
      if (gyro_sensor != NULL)
      {
        gyro_sensor->odr = gyro_odr;  // Set to zero for now... TODO: CONSIDER SYNCING GYRO_ODR AND ACCEL_ODR
      }
      if (temp_sensor != NULL)
      {
        temp_sensor->odr = temp_odr;
      }
      MPU6050_MSG_0(LOW, "Attempting to enable Cycle-Mode...");
      status=sns_dd_mpu6xxx_set_cycle_mode(state, true);
      if (SNS_DDF_SUCCESS != status)
        return status;
//      sensor->odr = accel_odr;  // This is primarily for the TEMP sensor
    }
    else
    {
      return status;
    }
  }
  else if ( (state->device_id == DEVICE_ID_MPU6500) &&
            (0 == gyro_desired_odr) &&
            (0 == temp_desired_odr) &&
            (req_odr_max <= MPU6050_CYCLE_MODE_LIMIT_HZ) )
  {
    if ( req_odr_max <= 1)
    {
      status = sns_dd_mpu6500_set_lp_accel_odr(state, MPU6500_LP_ACCEL_ODR_FREQ_1_95HZ);
      accel_odr = 1;
    }
    else if( req_odr_max <= 5 )
    {
      status = sns_dd_mpu6500_set_lp_accel_odr(state, MPU6500_LP_ACCEL_ODR_FREQ_7_81HZ);
      accel_odr = 5;
    }
    else if (req_odr_max <= 10 )
    {
      status = sns_dd_mpu6500_set_lp_accel_odr(state, MPU6500_LP_ACCEL_ODR_FREQ_15_63HZ);
      accel_odr = 10;
    }
    else if (req_odr_max <= 40 )
    {
      status = sns_dd_mpu6500_set_lp_accel_odr(state, MPU6500_LP_ACCEL_ODR_FREQ_62_50HZ);
      accel_odr = 40;
    }
    if (status == SNS_DDF_SUCCESS)
    {
      // Push the proper odr values back into the right places
      if (accel_sensor != NULL)
      {
        accel_sensor->odr = accel_odr;
      }
      if (gyro_sensor != NULL)
      {
        gyro_sensor->odr = gyro_odr;  // Set to zero for now...
      }
      if (temp_sensor != NULL)
      {
        temp_sensor->odr = temp_odr;
      }
      MPU6050_MSG_0(LOW, "Attempting to enable Cycle-Mode...");
      status=sns_dd_mpu6xxx_set_cycle_mode(state, true);
      if (SNS_DDF_SUCCESS != status)
        return status;
    }
    else
    {
      return status;
    }
  }
  else
  {
    req_bandwidth = FX_FLTTOFIX_Q16(req_odr_max / 2);

    // Find a bandwidth that's the same or next higher than the one requested.
    for(lpf_index = 0; lpf_index < bandwidth_table_size; lpf_index++)
    {
        if(bandwidth_table[lpf_index] >= req_bandwidth)
        {
            actual_bandwidth = bandwidth_table[lpf_index];
            break;
        }
    }

    // No bandwidth to match the requested ODR.
    if(actual_bandwidth == 0)
    {
      MPU6050_MSG_1(HIGH, "sns_dd_mpu6xxx_set_odr cannot find a bandwidth to match the requested odr: %d", odr);
      return SNS_DDF_EINVALID_PARAM;
    }

    status = sns_dd_mpu6xxx_set_cycle_mode(state, false);      /* make sure no cycle mode is used */
    if(status != SNS_DDF_SUCCESS)
        return status;

    // Calculate SMPRT_DIV (see MPU6050 Register Map, Section 4.2):
    // Sample Rate = Gyro output rate / (1 + SMPRT_DIV)
    gyro_output_rate = sns_dd_mpu6050_get_gyro_output_rate(lpf_index);
    smprt_div = (gyro_output_rate / req_odr_max);
    /* adjust the divisor to possible max as the register value is just byte length */
    if (smprt_div > 0x100)
    {
      smprt_div = 0x100;
    }
    actual_odr = gyro_output_rate / smprt_div;
    // Set LPF
    status = sns_dd_mpu6xxx_set_lowpass(state, lpf_index);
    if(status != SNS_DDF_SUCCESS)
        return status;

    if(sensor->cur_bandwidth != actual_bandwidth)
    {
      // Set LPF
      status = sns_dd_mpu6xxx_set_lowpass(state, lpf_index);
      if(status != SNS_DDF_SUCCESS)
         return status;
      sensor->cur_bandwidth = actual_bandwidth;
    }

    if( sensor->odr != actual_odr)
    {
      sns_ddf_write_port_byte(
        state->port_handle, MPU6050_REG_SMPRT_DIV, smprt_div-1, &status);
      if(status != SNS_DDF_SUCCESS)
        return status;

      // Record the sensor's actual ODR
      sensor->odr = actual_odr;

      MPU6050_MSG_2(MEDIUM, "Sensor type, %u, is now set to an ODR of %u Hz.", sensor->sensor_type, actual_odr);
    }

  }
  return status;
}


/**
 * @brief Configure the given sensor.
 *
 * @param[in] state           Ptr to the device instance
 * @param[in] sensor          Sensor to configure
 * @param[in] range_index     Index in the range table for the sensor
 * @param[in] accel_hpf       Accel high-pass filter; ignored for Gyro
 * @param[in] enable_selftest TRUE to enable HW self-test
 *
 * @return Success status of this operation.
 */
sns_ddf_status_e sns_dd_mpu6xxx_set_config_register(
   sns_dd_mpu6xxx_s* state,
   mpu6xxx_sensor_s* sensor,
   uint8_t           range_index,
   uint8_t           accel_hpf,
   boolean           enable_selftest)
{
    uint8_t  reg_addr;
    uint8_t  reg_data;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
//    mpu6xxx_sensor_s* doubletap_sensor;

    // Check for null-pointers
    if (NULL == state || NULL == sensor)
    {
      MPU6050_MSG_2(ERROR, "mpu6xxx_set_config_register: Null parameter (state = 0x%X) (sensor = 0x%X).", state, sensor);
      return SNS_DDF_EINVALID_PARAM;
    }

    // Do not set the config register while DOUBLETAP is running
    if (sns_dd_mpu6xxx_is_dmp_active(state))
    {
      MPU6050_MSG_0(HIGH,"ACCESS TO CONFIG FORBIDDEN (DMP ACTIVE)");
      return SNS_DDF_SUCCESS; //SNS_DDF_EINVALID_PARAM;
    }

    sns_ddf_sensor_e sensor_type = sensor->sensor_type;
    uint8_t* range_idx_ptr = &sensor->range;

    MPU6050_MSG_3(HIGH, "mpu6xxx_set_config_reg sensor_type: %d  range: %d  selftest? %d", sensor_type, range_index, enable_selftest);

    // If this device is an MPU-6500, only allow the following settings
    if (state->device_id == DEVICE_ID_MPU6500)
    {
      // ACCEL FSR must be 2g
      if (sensor_type == SNS_DDF_SENSOR_ACCEL && range_index != 0)
      {
        MPU6050_MSG_1(ERROR, "mpu6xxx_set_config_register: ACCEL range index %u is currently unsupported for MPU6500. (Must be 0)", range_index);
        return SNS_DDF_EINVALID_PARAM;
      }

      // GYRO FSR must be 2000dps
      if (sensor_type == SNS_DDF_SENSOR_GYRO && range_index != 3)
      {
        MPU6050_MSG_1(ERROR, "mpu6xxx_set_config_register: GYRO range index %u is currently unsupported for MPU6500. (Must be 3)", range_index);
        return SNS_DDF_EINVALID_PARAM;
      }
    }

    switch(sensor_type)
    {
    case SNS_DDF_SENSOR_ACCEL:
        if(range_index >= MPU6050_TABLE_SIZE(sns_dd_mpu6050_accel_range_reg))
        {
          MPU6050_MSG_2(ERROR, "mpu6xxx_set_config_register: ACCEL range index %u is out of bounds. (Must be smaller than %u)",range_index, MPU6050_TABLE_SIZE(sns_dd_mpu6050_accel_range_reg));
          return SNS_DDF_EINVALID_PARAM;
        }
        reg_addr  = MPU6050_REG_ACCEL_CONFIG;
        reg_data  = sns_dd_mpu6050_accel_range_reg[range_index] | accel_hpf;
        reg_data |= (enable_selftest == FALSE) ?
          MPU6050_ACCEL_ST_DISABLE : MPU6050_ACCEL_ST_ENABLE;
        break;

    case SNS_DDF_SENSOR_GYRO:
        if(range_index >= MPU6050_TABLE_SIZE(sns_dd_mpu6050_gyro_range_reg))
        {
          MPU6050_MSG_2(ERROR, "mpu6xxx_set_config_register: GYRO range index %u is out of bounds. (Must be smaller than %u)",range_index, MPU6050_TABLE_SIZE(sns_dd_mpu6050_gyro_range_reg));
          return SNS_DDF_EINVALID_PARAM;
        }
        reg_addr  = MPU6050_REG_GYRO_CONFIG;
        reg_data  = sns_dd_mpu6050_gyro_range_reg[range_index];
        reg_data |= (enable_selftest == FALSE) ?
          MPU6050_GYRO_ST_DISABLE : MPU6050_GYRO_ST_ENABLE;
        break;

    default:
        MPU6050_MSG_1(ERROR, "mpu6xxx_set_config_register: Unsupported sensor type %u", sensor_type);
        return SNS_DDF_EINVALID_PARAM;
    }

    sns_ddf_write_port_byte(state->port_handle, reg_addr, reg_data, &status);
    if(status == SNS_DDF_SUCCESS)
    {
       // Save the range so we can apply it when retrieving data.
       *range_idx_ptr = range_index;
    }

    return status;
}


// Set @exception to NULL if you do not want the function to use the exception
static bool sns_dd_mpu6xxx_is_ready_for_reset(
    sns_dd_mpu6xxx_s* state, sns_ddf_handle_t exception)
{
  uint8_t i;

  // Check for null-pointers
  if (state == NULL)
    return false;

  uint8_t sub_dev_exception_idx = sns_dd_mpu6xxx_get_sub_dev_idx(exception);
  /* Note: there is no need to check for a valid sub-device exception index.
   * The faulty exception index will essentially exclude the check for an exception.
   */

  // Check each sub-device, unless if it is the exception
  for (i = 0; i < state->sub_device_cnt; i++)
  {
    if ((i != sub_dev_exception_idx) && (state->sub_devices[i].powerstate == SNS_DDF_POWERSTATE_ACTIVE))
    {
      MPU6050_MSG_1(HIGH, "mpu6xxx is NOT ready for reset, sub dev %d is ACTIVE", i);
      return false;
    }
  }

  MPU6050_MSG_0(HIGH, "mpu6xxx is ready for reset");
  return true;
}

/**
 * @brief Resets all sensors that have the same dd_handle (i.e. within the same sub-device)
 */
sns_ddf_status_e sns_dd_mpu6xxx_reset_sub_dev(
    sns_dd_mpu6xxx_s* state, uint8_t sub_dev_idx)
{
  sns_ddf_status_e status;

  MPU6050_MSG_1(HIGH, "mpu6xxx resetting sub device %d", sub_dev_idx);

  // Check for null-pointers, out-of-bound indices, and other invalid parameters
  if (NULL == state)
    return SNS_DDF_EINVALID_PARAM;
  if (sub_dev_idx >= state->sub_device_cnt)
    return SNS_DDF_EINVALID_PARAM;

//  mpu6xxx_sub_dev_s* sub_dev = &state->sub_devices[sub_dev_idx];

  // Reset the entire sub-device (and all sensors contained within)
  status = sns_dd_mpu6xxx_set_powerstate(state, sub_dev_idx, SNS_DDF_POWERSTATE_LOWPOWER);
  if (status != SNS_DDF_SUCCESS)
    return status;

  return status;
}


/**
 * @brief Reset the whole device
 *
 * @param[in] state        Ptr to the driver structure
 *
 * @return  status of this operation.
 */
sns_ddf_status_e sns_dd_mpu6xxx_reset_device(sns_dd_mpu6xxx_s* state)
{
  sns_ddf_status_e status;
  uint8_t i2c_buff, bytes_written;
  mpu6xxx_sub_dev_s* sub_dev;

  uint8_t i, j;
  uint32_t  reset_wait_loop;

  MPU6050_MSG_0(LOW, "Performing a hardware reset of the device...");

  // Check for null-pointers
  if (NULL == state)
    return SNS_DDF_EINVALID_PARAM;

  /* Hardware-reset the device */
  i2c_buff = DEVICE_RESET_ENABLE;
  status = sns_ddf_write_port(
      state->port_handle,
      MPU6050_REG_PWR_MGMT_1,
      &i2c_buff,
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
      return status;

  /* Software-reset all of the register fields */
  state->cycle_mode = false;
  state->md_enabled = false;
  state->lpf_index = DLPF_CFG_GYRO_256HZ; // DLPF disabled

  // Wait for the device to come back up
  for (reset_wait_loop = 0; reset_wait_loop < DD_MPU6050_RESET_WAIT_LOOP_MAX; reset_wait_loop++)
  {
    uint8_t bytes_read;
    /* read MPU6050_REG_PWR_MGMT_1 to check if reset have been done */
    status = sns_ddf_read_port(state->port_handle,
                            MPU6050_REG_PWR_MGMT_1,
                            &i2c_buff,
                            1,
                            &bytes_read);
    if((state->device_id != DEVICE_ID_MPU6050) &&
       (state->device_id != DEVICE_ID_MPU6500))
    {
      sns_dd_mpu6050_validate_device_id(state);
    }
    if ( (SNS_DDF_SUCCESS == status) &&
         (((state->device_id == DEVICE_ID_MPU6050) &&  (i2c_buff & SLEEP_ENABLE)) ||
          ((state->device_id == DEVICE_ID_MPU6500) && !(i2c_buff & DEVICE_RESET_ENABLE))) )
    {
      break;
    }
    sns_ddf_delay(DD_MPU6050_RESET_WAIT_ONCE_US);
  }
  MPU6050_MSG_3(HIGH, "reset_dev - id=0x%x reg_val=0x%x status=%d",
                state->device_id, i2c_buff, status);

  if (DD_MPU6050_RESET_WAIT_LOOP_MAX == reset_wait_loop)
  {
    return SNS_DDF_EDEVICE;
  }

  /* Reset all sub-devices within the device */
  for (i = 0; i < state->sub_device_cnt; i++)
  {
    status = sns_dd_mpu6xxx_reset_sub_dev(state, i);
    if(status != SNS_DDF_SUCCESS)
        return status;
    sub_dev = &state->sub_devices[i];
    // If the sub-device uses the DMP (i.e. DOUBLETAP), then setup the DMP
    // Check to see if there is a DOUBLETAP sensor in the sub-device
    for (j = 0; j < sub_dev->num_sensors; j++)
    {
      sub_dev->sensors[j].desired_odr = 0;
      sub_dev->sensors[j].odr = 0;
      if (sub_dev->sensors[j].sensor_type == SNS_DDF_SENSOR_DOUBLETAP)
      {
        // Put the device into an active powerstate so we can write values to the DMP
        status = sns_dd_mpu6xxx_set_device_powerstate(state, SNS_DDF_POWERSTATE_ACTIVE);
        if (status != SNS_DDF_SUCCESS)
          return status;

        // Hold all advanced features (FIFO and DMP) under reset
        sns_ddf_write_port_byte(state->port_handle, MPU6050_REG_USER_CTRL, 0x0C, &status);
        if (status != SNS_DDF_SUCCESS)
          return status;

        // Flash the DMP image
        status = sns_dd_mpu6500_setup_dmp(state);
        if (status != SNS_DDF_SUCCESS)
          return status;
      }
    }

  }

  MPU6050_MSG_0(MEDIUM, "Device reset complete! Software and Hardware have both been reset.");

  return status;
}


/**
 * @brief Sets the device to its default operational state
 * @see sns_ddf_driver_if_s.reset()
*/
sns_ddf_status_e sns_dd_mpu6xxx_reset(sns_ddf_handle_t dd_handle)
{
  sns_dd_mpu6xxx_s* state = sns_dd_mpu6xxx_get_device(dd_handle);

  MPU6050_MSG_0(LOW, "Attempting to reset the device...");

  // Validate the state to make sure it is not a null-pointer
  if (NULL == state)
    return SNS_DDF_EINVALID_PARAM;

  // If all sensors are inactive, then reset the entire device
  if (sns_dd_mpu6xxx_is_ready_for_reset(state, dd_handle))
  {
    return sns_dd_mpu6xxx_reset_device(state);
  }
  // Else, software-reset the sub-device associated with @dd_handle
  else
  {
    MPU6050_MSG_0(MEDIUM, "The device is currently active so a software reset of internal structures will be performed.");
    uint8_t sub_dev_idx = sns_dd_mpu6xxx_get_sub_dev_idx(dd_handle);
    return sns_dd_mpu6xxx_reset_sub_dev(state, sub_dev_idx);
  }

}


/**
 * @brief Sets an attribute of the gyroscope
 * @see sns_ddf_driver_if_s.set_attrib()
 */
sns_ddf_status_e sns_dd_mpu6xxx_set_attr(
//static sns_ddf_status_e sns_dd_mpu6xxx_set_attr(
   sns_ddf_handle_t    dd_handle,
   sns_ddf_sensor_e    sensor_type,
   sns_ddf_attribute_e attrib,
   void*               value)
{
  sns_dd_mpu6xxx_s* state = sns_dd_mpu6xxx_get_device(dd_handle);
  mpu6xxx_sub_dev_s * sub_device = sns_dd_mpu6xxx_get_sub_dev(dd_handle);
  uint8_t sub_dev_idx = sns_dd_mpu6xxx_get_sub_dev_idx(dd_handle);
  mpu6xxx_sensor_s * sensor;
  sns_ddf_status_e status;
  uint8_t n;

  if (dd_handle == NULL || value == NULL || state == NULL || sub_device == NULL)
     return SNS_DDF_EINVALID_PARAM;

  // If the requested sensor type is NONE, then do nothing
  if (sensor_type == SNS_DDF_SENSOR__NONE)
  {
    MPU6050_MSG_0(LOW, "mpu6xxx_set_attr no actions available for sensor type NONE");
    return SNS_DDF_SUCCESS;
  }
  /* If the requested sensor type is ALL, then iterate through all of the
   * sub-device's sensors and call this function with a specific sensor type */
  if (sensor_type == SNS_DDF_SENSOR__ALL)
  {
    for (n = 0; n < sub_device->num_sensors; n++)
    {
      status = sns_dd_mpu6xxx_set_attr(dd_handle, sub_device->sensors[n].sensor_type, attrib, value);
      if (status != SNS_DDF_SUCCESS)
        return status;
    }
    return SNS_DDF_SUCCESS;
  }

  sensor = sns_dd_mpu6xxx_get_sensor(dd_handle, sensor_type);
  /* If the sensor type is not found in the sub-device associated with the
     handle, then report an error */
  if (NULL == sensor)
  {
    MPU6050_MSG_2(ERROR, "mpu6xxx_set_attr: Cannot find the sensor type %u within the dd_handle 0x%X", sensor_type, dd_handle);
    return SNS_DDF_EINVALID_PARAM;
  }

  MPU6050_MSG_3(HIGH, "mpu6xxx_set_attr dd_handle: %x  sensor_type: %d  attrib: %d", dd_handle, sensor_type, attrib);

  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
    {
      sns_ddf_powerstate_e* powerstate = value;
      return sns_dd_mpu6xxx_set_powerstate(state, sub_dev_idx, *powerstate);
    }

    case SNS_DDF_ATTRIB_RANGE:
    {
      uint8_t* range_index = value;
      if(sensor_type == SNS_DDF_SENSOR_TEMP || sensor_type == SNS_DDF_SENSOR_DOUBLETAP)
        return SNS_DDF_EINVALID_PARAM;

      // This attribute is not supported for the MPU6500 (due to DMP concurrency)
      if (state->device_id == DEVICE_ID_MPU6500)
      {
        MPU6050_MSG_0(ERROR, "set_attrib: range is not supported for MPU6500");
        return SNS_DDF_EINVALID_PARAM;
      }

      return sns_dd_mpu6xxx_set_config_register(state, sensor, *range_index,
                                                ACCEL_HPF_0_63HZ, FALSE);
    }

    // Note: SMGR should check the value of this after setting it.
    case SNS_DDF_ATTRIB_LOWPASS:  // TODO: ADD SUPPORT FOR LOWPASS
    {
      uint8_t* lowpass_index = value;
      if(sensor_type == SNS_DDF_SENSOR_TEMP)
        return SNS_DDF_EINVALID_PARAM;

      if (state->device_id == DEVICE_ID_MPU6050)
      {
        return sns_dd_mpu6xxx_set_lowpass(state, *lowpass_index);
      }
      else
      {
        // This attribute is not supported for the MPU6500 (due to DMP concurrency)
        MPU6050_MSG_0(ERROR, "set_attr lowpass is not supported.");
        return SNS_DDF_EINVALID_PARAM;
      }
    }

    case SNS_DDF_ATTRIB_ODR:
    {
      sns_ddf_odr_t* odr = value;
      if ( (SNS_DDF_SENSOR_ACCEL == sensor_type) && (state->md_enabled) )
      {
        MPU6050_MSG_3(HIGH, "set_attr %d odr=%d md_en=%d",
                      9912, *odr, state->md_enabled);
        state->cnt.set_odr_err++;
        return SNS_DDF_EFAIL;
      }
      else
      {
        return sns_dd_mpu6xxx_set_odr(state, sub_device, sensor, *odr);
      }
    }

    case SNS_DDF_ATTRIB_MOTION_DETECT:
    {
      MPU6050_MSG_0(HIGH, "set_attr MOTION-DETECT.");
      sns_ddf_status_e status = SNS_DDF_EFAIL;
      bool * flag = value;
      sensor = sns_dd_mpu6xxx_get_sensor(dd_handle, SNS_DDF_SENSOR_ACCEL);

      MPU6050_MSG_3(HIGH, "set_attr %d md_en=%d flag=%d",
                    9909, state->md_enabled, *flag);
      if (NULL == sensor)
      {
        MPU6050_MSG_1(ERROR, "mpu6xxx_set_attr: Cannot enable/disable Motion-Detect without initializing ACCEL for driver dd_handle 0x%X", dd_handle);
        return SNS_DDF_EINVALID_PARAM;
      }

      if ( *flag == state->md_enabled )     /* is it a redundant request */
      {
        state->cnt.md_dup_warning++;
        MPU6050_MSG_1(HIGH, "mpu6xxx_set_attr: Redundant request to enable/disable Motion-Detect. (flag = %u)", *flag);
        return SNS_DDF_SUCCESS;
      }
      else if(*flag)
      {
        if (sns_dd_mpu6xxx_is_dmp_active(state))
        {
          MPU6050_MSG_1(ERROR, "mpu6xxx_set_attr: Cannot enable Motion-Detect while the DMP is active for driver dd_handle 0x%X", dd_handle);
          return SNS_DDF_EINVALID_PARAM;
        }

        MPU6050_MSG_0(MEDIUM, "Enabling Motion-Detect...");
        if ( sensor->desired_odr )   /* Accel shall be activated for MOTION_DETECT */
        {
          status = sns_dd_mpu6xxx_enable_interrupt(state, sub_device, sensor, MPU6050_INT_MOTION_DETECT);
          if( SNS_DDF_SUCCESS == status )
          {
            state->md_enabled = TRUE;     /* this flag shall be set before calling mpu6050_set_odr() */
            state->cnt.md_state++;
            status = sns_dd_mpu6xxx_set_odr(state, sub_device, sensor, sensor->desired_odr);
            MPU6050_MSG_0(MEDIUM, "Motion-Detect Enabled!");
          }
          else
          {
            MPU6050_MSG_1(ERROR, "mpu6xxx_set_attr: Unable to enable MOTION-DETECT interrupt for device driver instance 0x%X", state);
            state->cnt.md_err++;
          }
          /* enable motion detection */
        }
        else
        {
          MPU6050_MSG_0(ERROR, "mpu6xxx_set_attr: Cannot set Motion-Detect without activating ACCEL first.");
          state->cnt.md_err++;
        }
        return status;
      }
      else
      {
        MPU6050_MSG_0(MEDIUM, "Disabling Motion-Detect...");
        /* disable motion detection */
        status = sns_dd_mpu6xxx_disable_interrupt( state, sub_device, sensor, MPU6050_INT_MOTION_DETECT);
        if( SNS_DDF_SUCCESS == status )
        {
          state->md_enabled = FALSE;
          MPU6050_MSG_3(HIGH, "set_attr %d status=%d md_state=%d",
                        9911, status, state->cnt.md_state);
          /* recover the previous freq */
          status = sns_dd_mpu6xxx_set_odr(state, sub_device, sensor, sensor->desired_odr);
          MPU6050_MSG_0(MEDIUM, "Motion-Detect Disabled.");
        }
      }
      return status;
    }

    case SNS_DDF_ATTRIB_IO_REGISTER:
    {
      sns_ddf_io_register_s* io_reg_in = (sns_ddf_io_register_s *) value;
      uint8_t bytes_written;
      sns_ddf_status_e status;

      // Address sizes are limited to uint16_t for the MPU6XXX
      if ((uint32_t)(io_reg_in->address_ptr) > 0xFFFF)
      {
        MPU6050_MSG_1(HIGH, "mpu6xxx_set_attr: invalid address %a", (io_reg_in->address_ptr));
        return SNS_DDF_EINVALID_PARAM;
      }
      // sns_ddf_write_port is limited to writing 0xFF bytes
      if (io_reg_in->length > 0xFF)
      {
        MPU6050_MSG_1(HIGH, "mpu6xxx_set_attr: length too long, %d", io_reg_in->length);
        return SNS_DDF_EINVALID_PARAM;
      }

      status = sns_ddf_write_port(
          state->port_handle,
          (uint16_t) ((uint32_t)io_reg_in->address_ptr),
          (uint8_t*) io_reg_in->data,
          (uint8_t) io_reg_in->length,
          &bytes_written);

      MPU6050_MSG_2(MEDIUM, "mpu6xxx_set_attr: (IO_REGISTER) Wrote to address 0x%X  length %d", io_reg_in->address_ptr, io_reg_in->length);
      return status;
    }
    default:
      return SNS_DDF_EINVALID_PARAM;
  }
}

static void sns_dd_mpu6xxx_log_data(
    sns_dd_mpu6xxx_s* state,
    sns_ddf_sensor_e  sensor)
{
  sns_err_code_e status;
  sns_log_sensor_data_pkt_s* log;
  uint8_t data_cache_index = 0;
  uint8_t i;
  uint8_t index;

  status = sns_logpkt_malloc(
      SNS_LOG_CONVERTED_SENSOR_DATA,
      sizeof(sns_log_sensor_data_pkt_s),
      (void**)&log);

  if(status != SNS_SUCCESS || log == NULL)
  {
      // Can't allocate log packet. Ignore silently.
      return;
  }

  log->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
  log->sensor_id = sensor;
  log->vendor_id = SNS_DDF_VENDOR_INVENSENSE;
  log->timestamp = state->data_cache_time;

  switch(sensor)
  {
     case SNS_DDF_SENSOR_ACCEL:
          data_cache_index = MPU6050_CACHE_ACCELX;
          log->num_data_types = MPU6XXX_NUM_INERTIAL_AXES;
          break;

     case SNS_DDF_SENSOR_GYRO:
          data_cache_index = MPU6050_CACHE_GYROX;
          log->num_data_types = MPU6XXX_NUM_INERTIAL_AXES;
          break;

     case SNS_DDF_SENSOR_TEMP:
          data_cache_index = MPU6050_CACHE_TEMP;
          log->num_data_types = 1;
          break;

     case SNS_DDF_SENSOR_DOUBLETAP:
          data_cache_index = MPU6500_CACHE_DOUBLETAP;
          log->num_data_types = 1;
          break;

     default:
          log->num_data_types = 0;
          break;
  }

  for(i = 0, index = data_cache_index;
         (i < log->num_data_types) &&
         (index < ARR_SIZE(state->data_cache)); i++, index++)
  {
    log->data[i] = state->data_cache[index];
  }

  // Commit and free the log packet.
  sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log);
}

/**
 * param[in]    state
 * param[in]    length
 * param[out]   packet      Pointer to a uint8_t array with a size of @length
 * param[out]   bytes_read  Pointer to the location where the number of bytes read should be placed
 */
sns_ddf_status_e sns_dd_mpu6xxx_get_packet_from_fifo(
    sns_dd_mpu6xxx_s* state, uint8_t length, uint8_t* packet, uint8_t* bytes_read)
{
  sns_ddf_status_e status;
  uint8_t buffer[2] = {0};
  uint8_t packet_buffer[length];
  uint8_t read_count;
//  uint16_t i;
  uint16_t fifo_count = 0;

  // Burst-read the FIFO count
  status = sns_ddf_read_port(
              state->port_handle,
              MPU6XXX_REG_FIFO_COUNTH,
              buffer,
              2,
              &read_count);
  if (status != SNS_DDF_SUCCESS)
    return status;

  // Merge the high byte and the low byte of the FIFO_COUNT
  fifo_count = (((uint16_t) buffer[0]) << 8) | (buffer[1]);

  MPU6050_MSG_1(LOW, "mpu6xxx_get_packet_from_fifo: FIFO COUNT is %d", fifo_count);

  if (length > fifo_count)
  {
    MPU6050_MSG_2(ERROR, "mpu6xxx_get_packet_from_fifo: Unable to read %d bytes from the FIFO, it exceeds the current FIFO count of %d.", length, fifo_count);
    return SNS_DDF_EINVALID_PARAM;
  }

  SNS_OS_MEMSET(packet_buffer, 0, length);

  // TODO: remove the while loop beneath
  /* Flush the FIFO by multiples of @length. This has the effect of reading as
   * many packets of data as we can from the FIFO. If there are more, then just
   * return the last packet read. The primary reason behind this strategy is to
   * help prevent FIFO overflow. */
  while(fifo_count >= length)
  {
    // Burst-read from the FIFO register.
    status = sns_ddf_read_port(
        state->port_handle, MPU6XXX_REG_FIFO_R_W, packet_buffer, length, bytes_read);
    if(status != SNS_DDF_SUCCESS)
        return status;

    fifo_count = fifo_count - length;
  }
  // Return the last packet read.
  SNS_OS_MEMCOPY(packet, packet_buffer, length);

  return status;
}

/**
 * @brief Gets all sensor data from the device and stores it in the local cache.
 *
 * @param[in] state: Pointer to the device-instance structure
 * @param[in] map_axes: Map to the phone's axes if TRUE. Otherwise just cache raw data.
 *
 * @return Success status of this operation.
 */
sns_ddf_status_e sns_dd_mpu6xxx_get_data_from_device(
   sns_dd_mpu6xxx_s* state,
   bool map_axes)
{
  uint8_t i;
  sns_ddf_status_e status;
  uint8_t bytes_read;
  int16_t device_data[MPU6050_CACHE__SIZE] = {0};
  uint8_t* buffer = (uint8_t*)device_data;
  q16_t sensitivity;
  mpu6xxx_sensor_s* sensor;
  mpu6xxx_sub_dev_s* sub_dev;

  // Read the data
  status = sns_ddf_read_port(
      state->port_handle,
      MPU6050_REG_ACCEL_XOUT_H,
      buffer,
      MPU6500_CACHE_DOUBLETAP * 2,  // Read only the accel, temp, and gyro
      &bytes_read);

  if (status != SNS_DDF_SUCCESS)
     return status;

  state->data_cache_time = sns_ddf_get_timestamp();

  // Combine high and low order bytes
  device_data[MPU6050_CACHE_ACCELX] = buffer[0] << 8  | buffer[1];
  device_data[MPU6050_CACHE_ACCELY] = buffer[2] << 8  | buffer[3];
  device_data[MPU6050_CACHE_ACCELZ] = buffer[4] << 8  | buffer[5];
  device_data[MPU6050_CACHE_TEMP]   = buffer[6] << 8  | buffer[7];
  device_data[MPU6050_CACHE_GYROX]  = buffer[8] << 8  | buffer[9];
  device_data[MPU6050_CACHE_GYROY]  = buffer[10] << 8 | buffer[11];
  device_data[MPU6050_CACHE_GYROZ]  = buffer[12] << 8 | buffer[13];

  // Check for 16 bit signed data out of bounds
  for (i = 0; i < MPU6050_CACHE__SIZE; i++)
  {
      state->data_status[i] =
          (device_data[i] == ((0x8000)) || device_data[i] == ((0x7FFF))) ?
          SNS_DDF_EDATA_BOUND : SNS_DDF_SUCCESS;
  }

  // Report accel data in mG (Q16 format)
  sensor = sns_dd_mpu6xxx_search_for_sensor(state, SNS_DDF_SENSOR_ACCEL, &sub_dev);
  if (sensor != NULL)
  {
    sensitivity = sns_dd_mpu6050_accel_sensitivity[sensor->range];
    state->data_cache[MPU6050_CACHE_ACCELX] = device_data[MPU6050_CACHE_ACCELX] * sensitivity;
    state->data_cache[MPU6050_CACHE_ACCELY] = device_data[MPU6050_CACHE_ACCELY] * sensitivity;
    state->data_cache[MPU6050_CACHE_ACCELZ] = device_data[MPU6050_CACHE_ACCELZ] * sensitivity;
    if (map_axes)
      sns_ddf_map_axes(&state->axes_map, &state->data_cache[MPU6050_CACHE_ACCELX]);
  }

  // Report gyro data in radians/sec (Q16 format)
  sensor = sns_dd_mpu6xxx_search_for_sensor(state, SNS_DDF_SENSOR_GYRO, &sub_dev);
  if (sensor != NULL)
  {
    sensitivity = sns_dd_mpu6050_gyro_sensitivity[sensor->range];
    state->data_cache[MPU6050_CACHE_GYROX] = (device_data[MPU6050_CACHE_GYROX] * sensitivity) >> 9;
    state->data_cache[MPU6050_CACHE_GYROY] = (device_data[MPU6050_CACHE_GYROY] * sensitivity) >> 9;
    state->data_cache[MPU6050_CACHE_GYROZ] = (device_data[MPU6050_CACHE_GYROZ] * sensitivity) >> 9;
    if (map_axes)
      sns_ddf_map_axes(&state->axes_map, &state->data_cache[MPU6050_CACHE_GYROX]);
  }

  // Report temperature data in deg C (Q16 format)
  state->data_cache[MPU6050_CACHE_TEMP] =
      (device_data[MPU6050_CACHE_TEMP] - sns_dd_mpu6050_temp_offset_lsb) *
      sns_dd_mpu6050_temp_sensitivity + sns_dd_mpu6050_temp_offset_deg;

  return SNS_DDF_SUCCESS;
}


/**
 * @brief Called by the SMGR to get instantaneous data
 * @see sns_ddf_driver_if_s.get_data()
 */
sns_ddf_status_e sns_dd_mpu6xxx_get_data(
   sns_ddf_handle_t       dd_handle,
   sns_ddf_sensor_e       sensors[],
   uint32_t               num_sensors,
   sns_ddf_memhandler_s*  memhandler,
   sns_ddf_sensor_data_s* data[])
{
  uint8_t i, j;
  sns_ddf_status_e status;
  sns_ddf_sensor_data_s* data_iter;
  sns_dd_mpu6xxx_s* state = sns_dd_mpu6xxx_get_device(dd_handle);
//  mpu6xxx_sub_dev_s* sub_dev = sns_dd_mpu6xxx_get_sub_dev(dd_handle);
  uint8_t data_cache_idx = 0;
  uint8_t index;

  MPU6050_MSG_3(HIGH, "mpu6xxx_get_data for %d sensors. first sensor is: %d  dd_handle: 0x%x", num_sensors, sensors[0], dd_handle);

  if(state == NULL || sensors == NULL || memhandler == NULL || data == NULL)
      return SNS_DDF_EINVALID_PARAM;

  for(i = 0; i < num_sensors; i++)
  {
     if(sensors[i] != SNS_DDF_SENSOR_ACCEL &&
        sensors[i] != SNS_DDF_SENSOR_GYRO &&
        sensors[i] != SNS_DDF_SENSOR_TEMP &&
        sensors[i] != SNS_DDF_SENSOR_DOUBLETAP)
        return SNS_DDF_EINVALID_PARAM;
  }

  status = sns_dd_mpu6xxx_get_data_from_device(state, TRUE);
  if (status != SNS_DDF_SUCCESS)
     return status;

  *data = sns_ddf_memhandler_malloc(
      memhandler, num_sensors * sizeof(sns_ddf_sensor_data_s));

  if(*data == NULL)
      return SNS_DDF_ENOMEM;

  for(i = 0; i < num_sensors; i++)
  {
     data_iter = &((*data)[i]);

     data_iter->sensor = sensors[i];
     data_iter->timestamp = state->data_cache_time;
     data_iter->status = SNS_DDF_SUCCESS;

     switch(sensors[i])
     {
       case SNS_DDF_SENSOR_ACCEL:
            data_iter->num_samples = 3;
            data_cache_idx = MPU6050_CACHE_ACCELX;
            break;
       case SNS_DDF_SENSOR_GYRO:
            data_iter->num_samples = 3;
            data_cache_idx = MPU6050_CACHE_GYROX;
            break;
       case SNS_DDF_SENSOR_TEMP:
            data_iter->num_samples = 1;
            data_cache_idx = MPU6050_CACHE_TEMP;
            break;
       case SNS_DDF_SENSOR_DOUBLETAP:
            data_iter->num_samples = 1;
            data_cache_idx = MPU6500_CACHE_DOUBLETAP;
            break;
       default:
            return SNS_DDF_EINVALID_PARAM;
     }

     data_iter->samples = sns_ddf_memhandler_malloc(
         memhandler, sizeof(sns_ddf_sensor_sample_s) * data_iter->num_samples);

     if (data_iter->samples == NULL)
         return SNS_DDF_ENOMEM;

     if (EnableI2C == FALSE)
     {
         static uint32_t dummyData = 1;
         for (j = 0, index = data_cache_idx;
                 (j < data_iter->num_samples) &&
                 (index < ARR_SIZE(state->data_cache)) &&
                 (index < ARR_SIZE(state->data_status));
                      j++, index++)
         {
             state->data_cache[index] = dummyData++;
             state->data_status[index] = SNS_DDF_SUCCESS;
         }
     }

     for (j = 0, index = data_cache_idx;
                 (j < data_iter->num_samples) &&
                 (index < ARR_SIZE(state->data_status)) &&
                 (index < ARR_SIZE(state->data_cache));
                      j++, index++)
     {
         data_iter->samples[j].sample = state->data_cache[index];
         data_iter->samples[j].status = state->data_status[index];
     }

     sns_dd_mpu6xxx_log_data(state, sensors[i]);
  }

  return SNS_DDF_SUCCESS;
}


/**
 * @brief Called in response to an interrupt for this driver.
 * @see sns_ddf_driver_if_s.handle_irq()
 */
static void sns_dd_mpu6xxx_handle_irq(
        sns_ddf_handle_t  dd_handle,
        uint32_t          gpio_num,
        sns_ddf_time_t    timestamp)
{
  sns_dd_mpu6xxx_s* state = sns_dd_mpu6xxx_get_device(dd_handle);
  mpu6xxx_sub_dev_s* sub_dev = sns_dd_mpu6xxx_get_sub_dev(dd_handle);
  mpu6xxx_sensor_s* sensor;
  sns_ddf_sensor_e sensor_type;
  sns_ddf_status_e status;
  uint8_t buffer = 0;
  uint8_t buffer_4b[4];
  uint8_t read_count;

  MPU6050_MSG_2(LOW, "mpu6xxx_handle_irq dd_handle: %x  gpio_num: %d", dd_handle, gpio_num);

  if (NULL == state || NULL == sub_dev)
  {
    return;
  }

  // Check which interrupts occurred.
  status = sns_ddf_read_port(
      state->port_handle, MPU6050_REG_INT_STATUS, &buffer, 1, &read_count);
  if(status != SNS_DDF_SUCCESS)
      return;

  // Motion detection
  if((buffer & MOT_EN_ENABLE) && state->md_enabled)
  {
    MPU6050_MSG_0(HIGH, "mpu6xxx_handle_irq: MOTION DETECT INT detected");
    sensor = sns_dd_mpu6xxx_search_for_sensor(state, SNS_DDF_SENSOR_ACCEL, &sub_dev);
    if (NULL == sensor)
    {
      MPU6050_MSG_1(ERROR, "mpu6xxx_handle_irq: Cannot find the ACCEL sensor in state 0x%X", state);
      return;
    }

    if ( state->md_enabled )
    {
      status = sns_dd_mpu6xxx_disable_interrupt(state, sub_dev, sensor, MPU6050_INT_MOTION_DETECT);
      if(status != SNS_DDF_SUCCESS)
          return;

      sns_ddf_smgr_notify_event(
                sub_dev->smgr_handle,
                SNS_DDF_SENSOR_ACCEL,
                SNS_DDF_EVENT_MOTION);
      MPU6050_MSG_0(HIGH, "mpu6xxx_handle_irq: Notified SMGR of a MOTION DETECT INT");
      /* make accel stream */
      state->md_enabled = FALSE;     /* this flag shall be cleared before calling mpu6050_set_odr() */
      status = sns_dd_mpu6xxx_set_odr(state, sub_dev, sensor, sensor->desired_odr);
      if(status != SNS_DDF_SUCCESS)
          return;
    }
  }

  // Data ready
  if(buffer & DATA_RDY_INT_ENABLED)
  {
    uint8_t i;
    uint32_t j;
    uint8_t k;
    sns_ddf_handle_t smgr_handle;
    sns_ddf_status_e  ddf_status;
    ddf_status = sns_dd_mpu6xxx_get_data_from_device(state, TRUE);

    if(ddf_status != SNS_DDF_SUCCESS)
        return;

    state->data_cache_time = timestamp;

    // Iterate through all sub-devices
    for(i = 0; i < state->sub_device_cnt; i++)
    {
      // Iterate through each sub-device's sensors
      sub_dev = &state->sub_devices[i];
      for (j = 0; j < sub_dev->num_sensors; j++)
      {
        smgr_handle = sub_dev->smgr_handle;
        sensor = &sub_dev->sensors[j];
        // If the sensor has DRI enabled, then cache the data before notifying the SMGR
        if (sensor->int_enabled)
        {
          sensor_type = sensor->sensor_type;
          MPU6050_MSG_1(HIGH, "mpu6xxx_handle_irq DATA READY INT for sensor_type %d", sensor_type);
          switch(sensor_type)
          {
          case SNS_DDF_SENSOR_ACCEL:
          {
            state->sensor_data.status = ddf_status;
            state->sensor_data.sensor = SNS_DDF_SENSOR_ACCEL;
            state->sensor_data.num_samples = 3;
            state->sensor_data.timestamp = timestamp; //TS from the INT TS;
            for(k = 0; k < state->sensor_data.num_samples; k++)
            {
              state->sensor_data.samples[k].sample = state->data_cache[MPU6050_CACHE_ACCELX + k];
              state->sensor_data.samples[k].status = state->data_status[MPU6050_CACHE_ACCELX + k];
            }
            sns_dd_mpu6xxx_log_data(state, SNS_DDF_SENSOR_ACCEL);
            sns_ddf_smgr_notify_data(smgr_handle, &state->sensor_data, 1);
            break;
          }
          case SNS_DDF_SENSOR_GYRO:
          {
            state->sensor_data.status = ddf_status;
            state->sensor_data.sensor = SNS_DDF_SENSOR_GYRO;
            state->sensor_data.num_samples = 3;
            state->sensor_data.timestamp = timestamp; //TS from the INT TS;
            for(k = 0; k <state->sensor_data.num_samples; k++)
            {
              state->sensor_data.samples[k].sample = state->data_cache[MPU6050_CACHE_GYROX + k];
              state->sensor_data.samples[k].status = state->data_status[MPU6050_CACHE_GYROX + k];
            }
            sns_dd_mpu6xxx_log_data(state, SNS_DDF_SENSOR_GYRO);
            sns_ddf_smgr_notify_data(smgr_handle, &state->sensor_data, 1);
            break;
          }
          case SNS_DDF_SENSOR_TEMP:
          {
            state->sensor_data.status = ddf_status;
            state->sensor_data.sensor = SNS_DDF_SENSOR_TEMP;
            state->sensor_data.num_samples = 1;
            state->sensor_data.timestamp = timestamp; //TS from the INT TS;
            state->sensor_data.samples[0].sample = state->data_cache[MPU6050_CACHE_TEMP];
            state->sensor_data.samples[0].status = state->data_status[MPU6050_CACHE_TEMP];
            sns_dd_mpu6xxx_log_data(state, SNS_DDF_SENSOR_TEMP);
            sns_ddf_smgr_notify_data(smgr_handle, &state->sensor_data, 1);
            break;
          }
          default:
            break;
          }
        }
      }
    }
  }

  // DMP data ready
  if(buffer & DMP_DATA_RDY_INT_ENABLED)
  {
    uint8_t tap_number = 0;
    uint8_t tap_direction = 0;

    MPU6050_MSG_0(LOW, "mpu6xxx_handle_irq DMP DATA READY INT");

    // Find the sensor data structure for the DOUBLETAP sensor
    sensor = sns_dd_mpu6xxx_search_for_sensor(state, SNS_DDF_SENSOR_DOUBLETAP, &sub_dev);
    if (NULL == sensor)
      return;

    /* Only Tap-Gestures should be enabled in the DMP and only the DMP should
       be writing to the FIFO. Hence, we can expect only one 4-byte Gesture
       Word to be in the FIFO. */
    // Read packets (in increments of 4 bytes) from the FIFO register.
    status = sns_dd_mpu6xxx_get_packet_from_fifo(state, 4, buffer_4b, &read_count);
    if (status != SNS_DDF_SUCCESS)
    {
      MPU6050_MSG_0(ERROR, "mpu6xxx_handle_irq: Unable to read a packet from FIFO.");
      return;
    }

    // TODO: ADD PARSING FOR ALL OF THE PACKET TYPES. THIS WILL REQUIRE REWORKING THIS FUNCTION.
    //       WE WILL NEED TO ADD A DEDICATED FUNCTION THAT PARSES PACKETS BASED OFF OF EITHER THE
    //       2-BYTE HEADER INCLUDED IN THE PACKET OR BASED OFF OF A STATE THAT WE KEEP TRACK OF
    //       THAT SHOWS WHAT IS IN THE PACKET.
    // Parse the DMP data
    if (buffer_4b[1] & TAP_EVENT_MASK)
    {
      // Number of taps detected = TAP_NUMBER + 1
      tap_number = 1 + (buffer_4b[3] & TAP_NUMBER_MASK);
      tap_direction = (buffer_4b[3] & TAP_DIRECTION_MASK) >> TAP_DIRECTION;
      MPU6050_MSG_2(MEDIUM, "Tap Event! %d tap(s) detected in the %d direction.", tap_number, tap_direction);
    }

    // Cache the DOUBLETAP data
    if (tap_number >= 2)
    {
//        MPU6050_MSG_2(MEDIUM, "Doubletap Detected!! tap_num: %d  tap_dir: %d", tap_number, tap_direction);
      state->data_cache[MPU6500_CACHE_DOUBLETAP] = tap_direction;
      state->data_status[MPU6500_CACHE_DOUBLETAP] = status;
      // Notify the SMGR of a doubletap event
      state->sensor_data.status = status;
      state->sensor_data.sensor = SNS_DDF_SENSOR_DOUBLETAP;
      state->sensor_data.num_samples = 1;
      state->sensor_data.timestamp = timestamp; //TS from the INT TS;
      state->sensor_data.samples[0].sample = state->data_cache[MPU6500_CACHE_DOUBLETAP];
      state->sensor_data.samples[0].status = state->data_status[MPU6500_CACHE_DOUBLETAP];
      state->sensor_data.samples[1].sample = 0;
      state->sensor_data.samples[1].status = SNS_DDF_SUCCESS;
      state->sensor_data.samples[2].sample = 0;
      state->sensor_data.samples[2].status = SNS_DDF_SUCCESS;

      sns_dd_mpu6xxx_log_data(state, SNS_DDF_SENSOR_DOUBLETAP);
      status = sns_ddf_smgr_notify_data(sub_dev->smgr_handle, &state->sensor_data, 1);
      if (status != SNS_DDF_SUCCESS)
        return;
    }
    else
    {
      state->data_cache[MPU6500_CACHE_DOUBLETAP] = 0; // No DOUBLETAP detected
    }
  }
}

/**
 * @brief Called when the timer set by this driver has expired. This must be
 *        the callback function submitted when initializing a timer.
 *
 * @see sns_ddf_driver_if_s.handle_timer()
 */
static void sns_dd_mpu6xxx_handle_timer(sns_ddf_handle_t dd_handle, void* arg)
{
  sns_dd_mpu6xxx_s* state = sns_dd_mpu6xxx_get_device(dd_handle);
  if (NULL == state)
    return;

  if(state->hw_st_ptr != NULL)
  {
    sns_dd_mpu6050_handle_st_timer(state);
  }
}


sns_ddf_status_e sns_dd_mpu6xxx_probe(
  sns_ddf_device_access_s*  device_info,
  sns_ddf_memhandler_s*     memhandler,
  uint32_t*                 num_sensors,
  sns_ddf_sensor_e**        sensors )
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  sns_dd_mpu6xxx_device_id_e  dev_id;
  uint8_t i2c_buff = 0;
  uint8_t bytes_read;
  *num_sensors = 0;
  *sensors = NULL;

  status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  MPU6050_MSG_2(LOW, "mpu6xxx_probe: Probing the device at I2C addr: 0x%X  bus instance: 0x%X", device_info->port_config.bus_config.i2c->slave_addr, device_info->port_config.bus_instance);

  status = sns_ddf_read_port( port_handle,
                              MPU6050_REG_PWR_MGMT_1,
                              &i2c_buff,
                              1,
                              &bytes_read );

  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  if( i2c_buff != SLEEP_ENABLE )
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }
  // Validate the WHO_AM_I register to see which device is being probed
  status = sns_ddf_read_port( port_handle,
                              MPU6050_REG_WHO_AM_I,
                              &i2c_buff,
                              1,
                              &bytes_read );
  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }

  dev_id = (sns_dd_mpu6xxx_device_id_e) i2c_buff;
  if ( dev_id == DEVICE_ID_MPU6050 )
  {
    MPU6050_MSG_0(LOW, "MPU6050 found!");
    /* PWR MGMT and WHOAMI registers are correct. This is an MPU 6050 */
    *num_sensors = 3;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
                                          sizeof(sns_ddf_sensor_e) * *num_sensors );
    if( *sensors != NULL )
    {
      (*sensors)[0] = SNS_DDF_SENSOR_ACCEL;
      (*sensors)[1] = SNS_DDF_SENSOR_GYRO;
      (*sensors)[2] = SNS_DDF_SENSOR_TEMP;
      status = SNS_DDF_SUCCESS;
    } else {
      status = SNS_DDF_ENOMEM;
    }
    sns_ddf_close_port( port_handle );
    return status;
  }
  else if ( dev_id == DEVICE_ID_MPU6500 )
  {
    MPU6050_MSG_0(LOW, "MPU6500 found!!");
    /* PWR MGMT and WHOAMI registers are correct. This is an MPU6500 */
    *num_sensors = 5;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
                                          sizeof(sns_ddf_sensor_e) * *num_sensors );
    if( *sensors != NULL )
    {
      (*sensors)[0] = SNS_DDF_SENSOR_ACCEL;
      (*sensors)[1] = SNS_DDF_SENSOR_GYRO;
      (*sensors)[2] = SNS_DDF_SENSOR_TEMP;
      (*sensors)[3] = SNS_DDF_SENSOR_DOUBLETAP;
      status = SNS_DDF_SUCCESS;
    } else {
      status = SNS_DDF_ENOMEM;
    }
    sns_ddf_close_port( port_handle );
    return status;
  }
  else
  {
    /* Unknown device ID. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }

}

// Function driver API
sns_ddf_driver_if_s sns_dd_mpu6xxx_if =
{
  .init = &sns_dd_mpu6xxx_init,
  .get_data = &sns_dd_mpu6xxx_get_data,
  .set_attrib = &sns_dd_mpu6xxx_set_attr,
  .get_attrib = &sns_dd_mpu6xxx_get_attr,
  .handle_timer = &sns_dd_mpu6xxx_handle_timer,
  .handle_irq = &sns_dd_mpu6xxx_handle_irq,
  .reset = &sns_dd_mpu6xxx_reset,
  .run_test = &sns_dd_mpu6xxx_run_test,
  .enable_sched_data = &sns_dd_mpu6xxx_enable_sched_data,
  .probe = &sns_dd_mpu6xxx_probe
};

