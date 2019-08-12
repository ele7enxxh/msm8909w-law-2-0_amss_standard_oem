/*==============================================================================
 Copyright (c) 2014, AKM
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
     3. Neither the name of the AKM nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
==============================================================================*/

/*==============================================================================

 Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mag_fifo_akm099xx.c#1 $

when         who     what, where, why
----------   ---     -----------------------------------------------------------
12/01/15     JTL     Add specific float casting for average sampling period calc
11/19/15     JTL     Update average sampling period calculations
11/14/15     MW      Polling jitter failure fix
10/09/15     PN      Use avg sample period for timestamping
10/01/15     MW      max_odr 50Hz for 9911/9915 in polling
08/25/15     AKM     Add AK09913/AK09916 support.
05/18/15     AH      Fix timestamp issue
03/31/15     AKM     Updated copyright message
11/13/14     MW      Fixed timestamps in trigger_fifo_data
11/12/14     MW      Updated self-test
09/12/14     AKM     Add AK09914/AK09915
08/16/13     AKM     Combile AK09912/AK09911.
02/28/13     AKM     AK09912
09/13/12     vh      Eliminated compiler warnings
08/23/12     ag      Fix crash on 8974 target
02/02/12     ad      Send error report to sensor manager with valid sensor id
01/06/12     SD      changed max rate from 60hz to 50hz
12/12/11     SD      added sns_ddf_axes_map_init
10/12/11     AG      Don't release timer when init fails
10/12/11     RY      Inverted x-axis polarity
10/11/11     br      deleted line which releases resources when init fail(we need the resource for get_attr()..)
10/06/11     RY      Updated polarity for FLUID
09/15/11     RY      Modified for AK8963
09/01/11     AG      Implement changes from code review
08/28/11     AG      Added self test functionality
06/23/11     dc      removed debug flag for external release
06/04/11     br      Changed to honor SENSOR__ALL too in addition to SENSOR_MAG. Changed log msg level
03/24/11     OF      Adjust sensitivity values to the measurement data read out
02/27/11     OF      Add Device ID and Device Info, read data in one 8 byte read
01/18/11     OF      Split mag driver to AK8963 & AK8973 separte files, splited from sns_dd_mag.c version#8
04/08/10     RY      Added axis conversion for FLUID and FFA/SURF.
==============================================================================*/

/*============================================================================
INCLUDE FILES
============================================================================*/

#include "sns_dd_mag_fifo_akm099xx_priv.h"
#include "sns_dd_mag_fifo_akm099xx_tlim.h"

/*============================================================================
STATIC VARIABLE DEFINITIONS
============================================================================*/

/*============================================================================
STATIC FUNCTION PROTOTYPES
============================================================================*/
static sns_ddf_status_e sns_mgt_akm_check_device(
    sns_ddf_handle_t port_handle,
    uint8_t          device_id[]
);

static sns_ddf_status_e sns_mgt_akm_read_asa(
  sns_dd_mag_akm099xx_state_s *state,
  uint8_t                     asa[]
);

static sns_ddf_status_e sns_mgt_akm_get_sens_adj(
  sns_dd_mag_akm099xx_state_s *state
);

static sns_ddf_status_e sns_mgt_akm_test_threshold(
  uint16_t testno,
  int16_t  testdata,
  int16_t  lolimit,
  int16_t  hilimit,
  uint32_t *err
);

static sns_ddf_status_e sns_mgt_akm_test_self_sw(
  sns_dd_mag_akm099xx_state_s *state,
  uint32_t                    *err
);

static sns_ddf_status_e sns_mgt_akm_config_powerstate(
  sns_dd_mag_akm099xx_state_s *state,
  sns_ddf_powerstate_e        powerstate
);

static void sns_mgt_akm_clear_data_cache(
  sns_dd_mag_akm099xx_state_s *state
);

/*==============================================================================
  STATIC FUNCTION:   sns_mgt_akm_check_device
==============================================================================*/
/*!
  @brief Check device id if the device is supported one or not.

  @param[in]  port_handle  Handle to a communication port.
  @param[out] *device_id   ptr to uint8_t array which size must be larger than 2.
                           When the function succeeded, device information
                           (i.e. WIA1, WIA2) is stored to this variable.

  @return
   The error code definition within the DDF
   SNS_DDF_SUCCESS on success;
==============================================================================*/
static sns_ddf_status_e sns_mgt_akm_check_device(
  sns_ddf_handle_t port_handle,
  uint8_t          device_id[])
{
  sns_ddf_status_e status;
  uint8_t          out;

  /* Read Device ID, to make sure device is working properly */
  if ((status = sns_ddf_read_port(
           port_handle,
           AKM_MAG_REG_ADDR_WIA1_099XX,
           device_id,
           DDF_READ_TWO_BYTES,
           &out)) != SNS_DDF_SUCCESS) {
    MAG_AKM_DEBUG_MSG_1(HIGH, "Failed to read device id. status = %d.", status);
    return status;
  }

  MAG_AKM_DEBUG_MSG_2(MED, " Id = 0x%02X,0x%02X.", device_id[0], device_id[1]);

  /* Check WIA1 */
  if (device_id[0] != AKM_MAG_COMPANY_ID) {
    MAG_AKM_DEBUG_MSG_1(HIGH, "Unknown company id (0x%02X).", device_id[0]);
    return SNS_DDF_EDEVICE;
  }

  if ((device_id[1] != AKM_MAG09916_DEVICE_ID) &&
      (device_id[1] != AKM_MAG09915_DEVICE_ID) &&
      (device_id[1] != AKM_MAG09914_DEVICE_ID) &&
      (device_id[1] != AKM_MAG09913_DEVICE_ID) &&
      (device_id[1] != AKM_MAG09912_DEVICE_ID) &&
      (device_id[1] != AKM_MAG09911_DEVICE_ID)) {
    MAG_AKM_DEBUG_MSG_1(HIGH, "Unknown device id (0x%02X).", device_id[1]);
    return SNS_DDF_EDEVICE;
  }

  return SNS_DDF_SUCCESS;
}

/*==============================================================================
  STATIC FUNCTION:   sns_mgt_akm_read_asa
==============================================================================*/
/*!
  @brief Read sensitivity adjustment value from Fuse ROM.

  @detail
  - Though this function changes device operation mode, device power state
    is not changed internally.

  @param[in]  *state   ptr to the driver state structure
  @param[out] *asa     ptr to uint8_t array which size must be larger than
                       AKM_MAG099XX_NUM_SENSITIVITY.

  @return
   The error code definition within the DDF
   SNS_DDF_SUCCESS on success;
==============================================================================*/
static sns_ddf_status_e sns_mgt_akm_read_asa(
  sns_dd_mag_akm099xx_state_s *state,
  uint8_t                     asa[])
{
  sns_ddf_status_e status;
  sns_ddf_status_e status_final;
  uint8_t          write_buffer, out;

  /* Input sanity check */
  if ((state == NULL) || (asa == NULL)) {
    return SNS_DDF_EINVALID_PARAM;
  }

  /* Set Fuse ROM access mode */
  write_buffer = AKM_MAG_FUSE_ROM_ACCESS_MODE_099XX;
  if ((status = sns_ddf_write_port(
           state->port_handle,
           AKM_MAG_REG_ADDR_CNTL2_099XX,
           &write_buffer,
           DDF_WRITE_ONE_BYTE,
           &out)) != SNS_DDF_SUCCESS) {
    MAG_AKM_DEBUG_MSG_1(HIGH, "Writing CNTL2 failed. Status = %d.", status);
    return status;
  }

  /* Read the data */
  if ((status = sns_ddf_read_port(
          state->port_handle,
          AKM_MAG_REG_ADDR_X_SEN_ADJUST_099XX,
          asa,
          AKM_MAG099XX_NUM_SENSITIVITY,
          &out)) != SNS_DDF_SUCCESS) {
    MAG_AKM_DEBUG_MSG_1(HIGH, "Reading ASA* data failed. Status = %d.", status);
    /* When operation is failed, the device must be set to powerdown mode.
     * Therefore, don't exit this function before set to the mode.*/
  }

  /* Set power-down mode */
  write_buffer = AKM_MAG_PWR_DOWN_MODE_099XX;
  if ((status_final = sns_ddf_write_port(
           state->port_handle,
           AKM_MAG_REG_ADDR_CNTL2_099XX,
           &write_buffer,
           DDF_WRITE_ONE_BYTE,
           &out)) != SNS_DDF_SUCCESS) {
    MAG_AKM_DEBUG_MSG_1(HIGH, "Writing CNTL2 failed. Status = %d.", status);
    return status_final;
  }

  MAG_AKM_DEBUG_MSG_3(MED, " ASA = 0x%02X,0x%02X,0x%02X.", asa[0], asa[1], asa[2]);
  return status;
}

/*==============================================================================

  STATIC FUNCTION:   sns_mgt_akm_get_sens_adj

==============================================================================*/
/*!
  @brief Gets the sensitivity adjusment values of the AKM099XX magnetometer

  @detail
  - Get the axis sensitivity adjusment values from the fuse ROM, store it in
    the driver state structure in floating point.
  - It's assumed to be called only while this driver is initialized.
    Therefore, power state is not changed internally.
  - When this function success, the right value is set in senso_adj_flt
    which is a member of sns_dd_mag_akm099xx_state_s struct.

  @param[in/out] state: Ptr to the driver structure

  @return
   The error code definition within the DDF
   SNS_DDF_SUCCESS on success;

*/
/*============================================================================*/
static sns_ddf_status_e sns_mgt_akm_get_sens_adj(
  sns_dd_mag_akm099xx_state_s *state)
{
  uint8_t          asa[AKM_MAG099XX_NUM_SENSITIVITY];
  sns_ddf_status_e status;
  int              i;

  /* Input sanity check */
  if (state == NULL) {
    return SNS_DDF_EINVALID_PARAM;
  }

#ifdef SNSD_MAG_AKM099XX_DEBUG_FUNC
  MAG_AKM_DEBUG_MSG_1(LOW, "sens_adj: type=%d", state->fuse_type);
#endif

  /* If the device does not have FUSE ROM, we don't need to access it. */
  /* AK09913, AK09915, AK09916: set default value, then return immediately. */
  if (state->fuse_type == SNSD_AKM_FUSE_NONE) {
    for (i = 0; i < AKM_MAG099XX_NUM_SENSITIVITY; ++i) {
      state->sens_adj_flt[i] = 1.0f;
    }
    return SNS_DDF_SUCCESS;
  }

  /* Read Fuse ROM */
  if ((status = sns_mgt_akm_read_asa(state, asa)) != SNS_DDF_SUCCESS) {
    return status;
  }

  if (state->fuse_type == SNSD_AKM_FUSE_TYPEA) {
    /* Sensitivity adjusment is calculated acording to
     * AK09912 datasheet version MS1547-E-00 section 8.3.13.
     */
    for (i = 0; i < AKM_MAG099XX_NUM_SENSITIVITY; ++i) {
      state->sens_adj_flt[i] = (asa[i] / 256.0f) + 0.5f;
    }
  } else if (state->fuse_type == SNSD_AKM_FUSE_TYPEB) {
    /* Sensitivity adjustment is calculated by the formula in
     * AK09914 datasheet preliminary_E01 section 11.3.13
     */
    for (i = 0; i < AKM_MAG099XX_NUM_SENSITIVITY; ++i) {
      state->sens_adj_flt[i] = (asa[i] / 256.0f) + 1.0f;
    }
  } else if (state->fuse_type == SNSD_AKM_FUSE_TYPEC) {
    /* Sensitivity adjusment is calculated acording to
     * AKM09911 datasheet version MS1526-E-00 section 8.3.11 */
    for (i = 0; i < AKM_MAG099XX_NUM_SENSITIVITY; ++i) {
      state->sens_adj_flt[i] = (asa[i] / 128.0f) + 1.0f;
    }
  } else {
    /* Never happens. */
    return SNS_DDF_EDEVICE;
  }

  return status;
}

/*==============================================================================
  STATIC FUNCTION:   sns_mgt_akm_test_threshold
==============================================================================*/
static sns_ddf_status_e sns_mgt_akm_test_threshold(
  uint16_t testno,
  int16_t  testdata,
  int16_t  lolimit,
  int16_t  hilimit,
  uint32_t *err)
{
  /* Input sanity check */
  if (err == NULL) {
    return SNS_DDF_EINVALID_PARAM;
  }

  if ((lolimit <= testdata) && (testdata <= hilimit)) {
    return SNS_DDF_SUCCESS;
  } else {
    *err = (uint32_t)((((uint32_t)testno) << 16) | ((uint16_t)testdata));
    return SNS_DDF_EFAIL;
  }
}

#define AKM_FST(no, data, lo, hi, err) \
  if (sns_mgt_akm_test_threshold((no), (data), (lo), (hi), (err)) \
      != SNS_DDF_SUCCESS) { goto TEST_SEQUENCE_FAILED; }

/*==============================================================================
  STATIC FUNCTION:   sns_mgt_akm_test_self_sw
==============================================================================*/
/*!
  @brief Implementation of SNS_DDF_TEST_SELF_SW.

  @param[in]  *state   ptr to the driver state structure
  @param[out] *out     ptr to u32 variable. When return value is not SUCCESS,
                       further error code is stored to this value.

  @return
   The error code definition within the DDF
   SNS_DDF_SUCCESS on success;
==============================================================================*/
static sns_ddf_status_e sns_mgt_akm_test_self_sw(
    sns_dd_mag_akm099xx_state_s *state,
    uint32_t                    *err)
{
  uint8_t          write_buffer, out;
  uint8_t          asa[AKM_MAG099XX_NUM_SENSITIVITY];
  uint8_t          akm_read_data[AKM_MAG099XX_NUM_DATA_BYTES];
  int16_t          xval_i16, yval_i16, zval_i16;
  sns_ddf_status_e status;

  /* Input sanity check */
  if ((state == NULL) || (err == NULL)) {
    return SNS_DDF_EINVALID_PARAM;
  }

#ifdef SNSD_MAG_AKM099XX_DEBUG_FUNC
  MAG_AKM_DEBUG_MSG_0(LOW, "test_self_sw:");
#endif

  /* Initialize error code. */
  *err = 0;

  /* Reset device */
  write_buffer = AKM_MAG_SOFT_RESET;

  if ((status = sns_ddf_write_port(
           state->port_handle,
           AKM_MAG_REG_ADDR_CNTL3_099XX,
           &write_buffer,
           DDF_WRITE_ONE_BYTE,
           &out)) != SNS_DDF_SUCCESS) {
    MAG_AKM_DEBUG_MSG_1(HIGH, "Device reset failed. Status = %d.", status);
    *err = ((TLIMIT_NO_RESET) << 16);
    goto TEST_SEQUENCE_FAILED;
  }

  /* After POR, wait for boot up the device (refer PORT in datasheet). */
  /* Wait for RESET operation completion. */
  sns_ddf_delay(USEC_TIME_TO_ACTIVE);

  /* STEP 1 ================================================================*/
  /* If the device has FUSE ROM, test the sensitivity value */
  if (state->fuse_type != SNSD_AKM_FUSE_NONE) {
    if ((status = sns_mgt_akm_read_asa(state, asa)) != SNS_DDF_SUCCESS) {
      *err = ((TLIMIT_NO_READ_ASA) << 16);
      goto TEST_SEQUENCE_FAILED;
    }
    AKM_FST(TLIMIT_NO_ASAX, asa[0], TLIMIT_LO_ASAX, TLIMIT_HI_ASAX, err);
    AKM_FST(TLIMIT_NO_ASAY, asa[1], TLIMIT_LO_ASAY, TLIMIT_HI_ASAY, err);
    AKM_FST(TLIMIT_NO_ASAZ, asa[2], TLIMIT_LO_ASAZ, TLIMIT_HI_ASAZ, err);
  }

  /* STEP 2 ================================================================*/
  /* Start self test */
  write_buffer = AKM_MAG_SELF_TEST_MODE_099XX;

  if ((status = sns_ddf_write_port(
           state->port_handle,
           AKM_MAG_REG_ADDR_CNTL2_099XX,
           &write_buffer,
           DDF_WRITE_ONE_BYTE,
           &out)) != SNS_DDF_SUCCESS) {
    MAG_AKM_DEBUG_MSG_1(HIGH, "Device reset failed. Status = %d.", status);
    *err = ((TLIMIT_NO_SET_SELFTEST) << 16);
    goto TEST_SEQUENCE_FAILED;
  }

  /* Since self_test is performed not in sensor streaming time,
   * sns_ddf_delay() is used instead of using timer for reducing code complexity.
   * The handle_timer() is in the uImage, self_test shouldn't use timer.
   */

  /* To ensure that menasurement is finished, wait for double as typical. */
  sns_ddf_delay(USEC_TIME_FOR_MEASUREMENT * 2);

  /* Read ST1, 7 data registers, ST2 in a single 9-byte I2C transaction. */
  /* ST1 value should be 0, because this is second time to read out. */
  if (sns_ddf_read_port(
        state->port_handle,
        AKM_MAG_REG_ADDR_STATUS1_099XX,
        akm_read_data,
        AKM_MAG099XX_NUM_DATA_BYTES,
        &out) != SNS_DDF_SUCCESS) {
    *err = ((TLIMIT_NO_READ_DATA) << 16);
    goto TEST_SEQUENCE_FAILED;
  }

  /* raw data in 16 bits */
  xval_i16 = (int16_t)(((uint16_t)akm_read_data[1]) | ((uint16_t)akm_read_data[2] << 8));
  yval_i16 = (int16_t)(((uint16_t)akm_read_data[3]) | ((uint16_t)akm_read_data[4] << 8));
  zval_i16 = (int16_t)(((uint16_t)akm_read_data[5]) | ((uint16_t)akm_read_data[6] << 8));
  /* Adjust sensitivity */
  xval_i16 = (int16_t)(xval_i16 * state->sens_adj_flt[0]);
  yval_i16 = (int16_t)(yval_i16 * state->sens_adj_flt[1]);
  zval_i16 = (int16_t)(zval_i16 * state->sens_adj_flt[2]);

  MAG_AKM_DEBUG_MSG_3(MED, "test_val=%6d, %6d, %6d", xval_i16, yval_i16, zval_i16);

  /* Check read value */
  if (state->device_select == AKM_MAG09916_DEVICE_ID) {
    AKM_FST(TLIMIT_NO_SLF_RVHX, xval_i16, TLIMIT_LO_SLF_RVHX_09916, TLIMIT_HI_SLF_RVHX_09916, err);
    AKM_FST(TLIMIT_NO_SLF_RVHY, yval_i16, TLIMIT_LO_SLF_RVHY_09916, TLIMIT_HI_SLF_RVHY_09916, err);
    AKM_FST(TLIMIT_NO_SLF_RVHZ, zval_i16, TLIMIT_LO_SLF_RVHZ_09916, TLIMIT_HI_SLF_RVHZ_09916, err);
  } else if (state->device_select == AKM_MAG09915_DEVICE_ID) {
    AKM_FST(TLIMIT_NO_SLF_RVHX, xval_i16, TLIMIT_LO_SLF_RVHX_09915, TLIMIT_HI_SLF_RVHX_09915, err);
    AKM_FST(TLIMIT_NO_SLF_RVHY, yval_i16, TLIMIT_LO_SLF_RVHY_09915, TLIMIT_HI_SLF_RVHY_09915, err);
    AKM_FST(TLIMIT_NO_SLF_RVHZ, zval_i16, TLIMIT_LO_SLF_RVHZ_09915, TLIMIT_HI_SLF_RVHZ_09915, err);
  } else if (state->device_select == AKM_MAG09914_DEVICE_ID) {
    AKM_FST(TLIMIT_NO_SLF_RVHX, xval_i16, TLIMIT_LO_SLF_RVHX_09914, TLIMIT_HI_SLF_RVHX_09914, err);
    AKM_FST(TLIMIT_NO_SLF_RVHY, yval_i16, TLIMIT_LO_SLF_RVHY_09914, TLIMIT_HI_SLF_RVHY_09914, err);
    AKM_FST(TLIMIT_NO_SLF_RVHZ, zval_i16, TLIMIT_LO_SLF_RVHZ_09914, TLIMIT_HI_SLF_RVHZ_09914, err);
  } else if (state->device_select == AKM_MAG09913_DEVICE_ID) {
    AKM_FST(TLIMIT_NO_SLF_RVHX, xval_i16, TLIMIT_LO_SLF_RVHX_09913, TLIMIT_HI_SLF_RVHX_09913, err);
    AKM_FST(TLIMIT_NO_SLF_RVHY, yval_i16, TLIMIT_LO_SLF_RVHY_09913, TLIMIT_HI_SLF_RVHY_09913, err);
    AKM_FST(TLIMIT_NO_SLF_RVHZ, zval_i16, TLIMIT_LO_SLF_RVHZ_09913, TLIMIT_HI_SLF_RVHZ_09913, err);
  } else if (state->device_select == AKM_MAG09912_DEVICE_ID) {
    AKM_FST(TLIMIT_NO_SLF_RVHX, xval_i16, TLIMIT_LO_SLF_RVHX_09912, TLIMIT_HI_SLF_RVHX_09912, err);
    AKM_FST(TLIMIT_NO_SLF_RVHY, yval_i16, TLIMIT_LO_SLF_RVHY_09912, TLIMIT_HI_SLF_RVHY_09912, err);
    AKM_FST(TLIMIT_NO_SLF_RVHZ, zval_i16, TLIMIT_LO_SLF_RVHZ_09912, TLIMIT_HI_SLF_RVHZ_09912, err);
  } else if (state->device_select == AKM_MAG09911_DEVICE_ID) {
    AKM_FST(TLIMIT_NO_SLF_RVHX, xval_i16, TLIMIT_LO_SLF_RVHX_09911, TLIMIT_HI_SLF_RVHX_09911, err);
    AKM_FST(TLIMIT_NO_SLF_RVHY, yval_i16, TLIMIT_LO_SLF_RVHY_09911, TLIMIT_HI_SLF_RVHY_09911, err);
    AKM_FST(TLIMIT_NO_SLF_RVHZ, zval_i16, TLIMIT_LO_SLF_RVHZ_09911, TLIMIT_HI_SLF_RVHZ_09911, err);
  } else {
    *err = ((TLIMIT_NO_INVALID_ID) << 16);
    goto TEST_SEQUENCE_FAILED;
  }

  AKM_FST(TLIMIT_NO_SLF_ST2, (akm_read_data[8] & TLIMIT_ST2_MASK),
          TLIMIT_LO_SLF_ST2, TLIMIT_HI_SLF_ST2, err);

TEST_SEQUENCE_FAILED:
  if (*err == 0) {
    return SNS_DDF_SUCCESS;
  } else {
    return SNS_DDF_EFAIL;
  }
}

/*==============================================================================
  STATIC FUNCTION:   sns_mgt_akm_config_powerstate
==============================================================================*/
/*!
  @brief Routine used to set the power state when set as an attrib by
  SMGR that calls the appropriate power setting routines based on the
  device type requested

  @detail
  When power is turned ON, AK099XX is in power-down mode. When any other mode
  is selected the AK099XX transits to the specified mode and starts operation.
  When operation finishes the AK099XX turns back to power-down mode.
  Therefore there isn't much difference between SNS_DDF_POWERSTATE_LOWPOWER and
  SNS_DDF_POWERSTATE_ACTIVE modes. When the former is set, we need to cancel
  any measurement in progress.

  @param[in/out] state: Ptr to the driver structure
  @param[in] *mode: Power state change requested with an power attrib set
  routine

  @return
   The error code definition within the DDF
   SNS_DDF_SUCCESS on success;

*/
/*============================================================================*/
static sns_ddf_status_e sns_mgt_akm_config_powerstate(
  sns_dd_mag_akm099xx_state_s *state,
  sns_ddf_powerstate_e        powerstate)
{
  /* Input sanity check */
  if (state == NULL) {
    return SNS_DDF_EINVALID_PARAM;
  }

  if (powerstate == state->curr_powerstate) {
    return SNS_DDF_SUCCESS;
  }

#ifdef SNSD_MAG_AKM099XX_DEBUG_FUNC
  MAG_AKM_DEBUG_MSG_1(LOW, "config_powerstate: state=%d", powerstate);
#endif

  if (powerstate == SNS_DDF_POWERSTATE_LOWPOWER) {
    /* Lowpower is requested */
    if (state->curr_powerstate == SNS_DDF_POWERSTATE_ACTIVE) {
      /* If current state is not IDLE, it means that
       * timer may working, but its not sure...
       */
      sns_ddf_timer_cancel(state->sns_dd_tmr_obj);
    }

    if (sns_mgt_akm_set_operation_mode_099xx(
          state,
          AKM_MAG_PWR_DOWN_MODE_099XX) != SNS_DDF_SUCCESS) {
      return (SNS_DDF_EDEVICE);
    }

    state->curr_powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
  } else if (powerstate == SNS_DDF_POWERSTATE_ACTIVE) {
    /* Actually, AKM device is always ready to sample data
     * while power is supplied. So this command is ignored currently.
     * When device actually starts measurement, i.e. set to other than
     * powerdown mode, then change the powerstate internally.
     */
    ;
  } else {
    return SNS_DDF_EINVALID_ATTR;
  }

  return SNS_DDF_SUCCESS;
}

/*==============================================================================
  STATIC FUNCTION:   sns_mgt_akm_clear_data_cache
==============================================================================*/
/*!
  @brief
  @detail

  @param[in/out] state: Ptr to the driver structure

  @return
*/
/*============================================================================*/
static void sns_mgt_akm_clear_data_cache(
  sns_dd_mag_akm099xx_state_s *state)
{
  uint16_t total_num = AKM_MAG099XX_NUM_DATATYPES * AKM_MAG099XX_AVERAGING;
  int      i;

  /* input sanity check */
  if (state == NULL) {
    return;
  }

#ifdef SNSD_MAG_AKM099XX_DEBUG_FUNC
  MAG_AKM_DEBUG_MSG_0(LOW, "clear data cache");
#endif

  /* put 0 */
  for (i = 0; i < total_num; i++) {
    state->data_cache[i] = 0;
  }
  /* clear num */
  state->num_cached = 0;
}

/*===========================================================================

FUNCTION:   sns_dd_akm099xx_init

===========================================================================*/
/**
* @brief Initializes the driver and sets up devices.
*
* @param[out] dd_handle_ptr  Pointer that this function must malloc and
*                            populate. This is a handle to the driver
*                            instance that will be passed in to all other
*                            functions. NB: Do not use @a memhandler to
*                            allocate this memory.
* @param[in]  smgr_handle    Handle used to identify this driver when it
*                            calls into Sensors Manager functions.
* @param[in]  nv_params      NV parameters retrieved for the driver.
* @param[in]  device_info    Access info for physical devices controlled by
*                            this driver. Used to configure the bus
*                            and talk to the devices.
* @param[in]  num_devices    Number of elements in @a device_info.
* @param[in]  memhandler     Memory handler used to dynamically allocate
*                            output parameters, if applicable. NB: Do not
*                            use memhandler to allocate memory for
*                            @a dd_handle_ptr.
* @param[in/out] sensors     List of supported sensors, allocated,
*                            populated, and returned by this function.
* @param[in/out] num_sensors Number of elements in @a sensors.
*
* @return Success if @a dd_handle_ptr was allocated and the driver was
*         configured properly. Otherwise a specific error code is returned.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm099xx_init(
    sns_ddf_handle_t*        dd_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e*        sensors[],
    uint32_t*                num_sensors)
{
  sns_dd_mag_akm099xx_state_s *state;
  sns_ddf_status_e            status;
  uint8_t                     device_id[2];
  static sns_ddf_sensor_e     sns_dd_mag_sensor_types[] = { SNS_DDF_SENSOR_MAG };

#ifdef AKM_MAG_ENABLE_UIMG
  /* Update smgr sensor data for the driver to indicate uImage support */
  sns_ddf_smgr_set_uimg_refac(smgr_handle);
#endif

  MAG_AKM_DEBUG_MSG_0(LOW, "Entering init().");

  /* Input sanity check */
  if (num_sensors == NULL) {
    return SNS_DDF_EINVALID_PARAM;
  }

  if ((status = sns_ddf_malloc_ex(
         (void **)&state,
         sizeof(sns_dd_mag_akm099xx_state_s),
         smgr_handle)) != SNS_DDF_SUCCESS) {
    MAG_AKM_DEBUG_MSG_0(HIGH, "init: Failed to allocate memory space.");
    goto errorCleanUp_0;
  }

  memset(state, 0, sizeof(sns_dd_mag_akm099xx_state_s));
  state->sampling_intvl_factor = 1;
  
  *dd_ptr = state;

  if ((status = sns_ddf_open_port(
           &(state->port_handle),
           &(device_info->port_config))) != SNS_DDF_SUCCESS) {
    MAG_AKM_DEBUG_MSG_0(HIGH, "init: Failed to open port.");
    goto errorCleanUp_1;
  }

  /* Initialize members which is not initialized after reset function. */
  state->smgr_handle = smgr_handle;
  state->gpio1       = device_info->first_gpio;

  /* Init timer object */
  if ((status = sns_ddf_timer_init(
           &state->sns_dd_tmr_obj,
           (sns_ddf_handle_t)state,
           &AKM099xx_FIFO_DD_IF,
           state,
           false)) != SNS_DDF_SUCCESS) {
    MAG_AKM_DEBUG_MSG_0(HIGH, "init: Failed to initialize timer.");
    goto errorCleanUp_2;
  }

  /* Fill out supported sensor info */
  *sensors       = sns_dd_mag_sensor_types;
  *num_sensors   = AKM_MAG099XX_NUM_SUPPORTED_SENSORS;

  /* Axes mapping parameter can be get from NV paraemter. */
  sns_ddf_axes_map_init(&state->axes_map, ((nv_params != NULL) ? nv_params->data : NULL));

  /* Read Device ID, to make sure device is working properly */
  if ((status = sns_mgt_akm_check_device(state->port_handle, device_id)) != SNS_DDF_SUCCESS) {
    goto errorCleanUp_3;
  }

  /* Check WIA2, then set device parameters. */
  state->device_select = device_id[1];

  if (state->device_select == AKM_MAG09916_DEVICE_ID) {
    state->fuse_type = SNSD_AKM_FUSE_NONE;
    state->mag_scale_flt = AKM_MAG09916_SENSITIVITY;
    state->has_drdy = false;
    state->has_nsf = false;
    state->extended_odr = false;
    state->device_fifo_size = AKM_MAG09916_FIFO_SIZE;
  } else if (state->device_select == AKM_MAG09915_DEVICE_ID) {
    state->fuse_type = SNSD_AKM_FUSE_NONE;
    state->mag_scale_flt = AKM_MAG09915_SENSITIVITY;
    state->has_drdy = true;
    state->has_nsf = true;
    state->extended_odr = false;
    state->device_fifo_size = AKM_MAG09915_FIFO_SIZE;
  } else if (state->device_select == AKM_MAG09914_DEVICE_ID) {
    state->fuse_type = SNSD_AKM_FUSE_TYPEB;
    state->mag_scale_flt = AKM_MAG09914_SENSITIVITY;
    state->has_drdy = true;
    state->has_nsf = true;
    state->extended_odr = false;
    state->device_fifo_size = AKM_MAG09914_FIFO_SIZE;
  } else if (state->device_select == AKM_MAG09913_DEVICE_ID) {
    state->fuse_type = SNSD_AKM_FUSE_NONE;
    state->mag_scale_flt = AKM_MAG09913_SENSITIVITY;
    state->has_drdy = false;
    state->has_nsf = false;
    state->extended_odr = false;
    state->device_fifo_size = AKM_MAG09913_FIFO_SIZE;
  } else if (state->device_select == AKM_MAG09912_DEVICE_ID) {
    state->fuse_type = SNSD_AKM_FUSE_TYPEA;
    state->mag_scale_flt = AKM_MAG09912_SENSITIVITY;
    state->has_drdy = true;
    state->has_nsf = true;
    state->extended_odr = false;
    state->device_fifo_size = AKM_MAG09912_FIFO_SIZE;
  } else if (state->device_select == AKM_MAG09911_DEVICE_ID) {
    state->fuse_type = SNSD_AKM_FUSE_TYPEC;
    state->mag_scale_flt = AKM_MAG09911_SENSITIVITY;
    state->has_drdy = false;
    state->has_nsf = false;
    state->extended_odr = false;
    state->device_fifo_size = AKM_MAG09911_FIFO_SIZE;
  } else {
    /* device_id is already checked in sns_mgt_akm_check_device. */
    /* So, program never reach here. */
    status = SNS_DDF_EDEVICE;
    MAG_AKM_DEBUG_MSG_1(HIGH, "Cannot determine device parameter (0x%02X).", state->device_select);
    goto errorCleanUp_3;
  }

  /* Read sensitivity data */
  /* fuse_type must be specified before get_sensitivity function is called. */
  if ((status = sns_mgt_akm_get_sens_adj(state)) != SNS_DDF_SUCCESS) {
    goto errorCleanUp_3;
  }

  /* Perform reset */
  sns_dd_akm099xx_reset(state);

  /* clear data_cache */
  sns_mgt_akm_clear_data_cache(state);

  MAG_AKM_DEBUG_MSG_0(LOW, "init finished.");

  return SNS_DDF_SUCCESS;

errorCleanUp_3:
  sns_ddf_timer_release(state->sns_dd_tmr_obj);
errorCleanUp_2:
  sns_ddf_close_port(state->port_handle);
errorCleanUp_1:
  sns_ddf_mfree_ex((void *)state, smgr_handle);
errorCleanUp_0:
  /* Clear output parameter */
  *dd_ptr      = NULL;
  *sensors     = NULL;
  *num_sensors = 0;

  return status;
}

/*===========================================================================

FUNCTION:   sns_dd_akm099xx_reset

===========================================================================*/
/**
* @brief Resets the driver and device so they return to the state they were
*        in after init() was called.
*
* @param[in] dd_handle  Handle to a driver instance.
*
* @return Success if the driver was able to reset its state and the device.
*         Otherwise a specific error code is returned.
*
* $TODO: If SPI interface is used, set I2CDIS register after soft reset.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm099xx_reset(sns_ddf_handle_t dd_handle)
{
  sns_ddf_status_e            status;
  sns_dd_mag_akm099xx_state_s *state = (sns_dd_mag_akm099xx_state_s *)dd_handle;
  uint8_t                     write_buffer, out;

  /* Input sanity check */
  if (state == NULL) {
    return SNS_DDF_EINVALID_PARAM;
  }

  /* Stop timer interrupt before reset the device. */
  sns_ddf_timer_cancel(state->sns_dd_tmr_obj);

  /* Reset device */
  write_buffer = AKM_MAG_SOFT_RESET;

  if ((status = sns_ddf_write_port(
           state->port_handle,
           AKM_MAG_REG_ADDR_CNTL3_099XX,
           &write_buffer,
           DDF_WRITE_ONE_BYTE,
           &out)) != SNS_DDF_SUCCESS) {
    MAG_AKM_DEBUG_MSG_1(HIGH, "Writing RST bit failed. status=%d.", status);
    return status;
  }

  /* After POR, wait for boot up the device (refer PORT). */
  sns_ddf_delay(USEC_TIME_TO_ACTIVE);

  /* Reset driver state (in order of struct declaration) */
  state->curr_powerstate  = SNS_DDF_POWERSTATE_LOWPOWER;
  state->curr_mode        = AKM_MAG_PWR_DOWN_MODE_099XX;
  state->curr_odr         = AKM_MAG099XX_SUPPORTED_ODR_POWER_DOWN;
  state->retry            = 0;
  state->water_mark       = 0;

#ifdef SNSD_MAG_AKM099XX_DEBUG_FUNC
  MAG_AKM_DEBUG_MSG_0(LOW, "reset done.");
#endif

  return status;
}

/*===========================================================================

FUNCTION:   sns_dd_akm099xx_set_attrib

===========================================================================*/
/**
* @brief Sets a sensor attribute to a specific value.
*
* @param[in] dd_handle  Handle to a driver instance.
* @param[in] sensor     Sensor for which this attribute is to be set. When
*                       addressing an attribute that refers to the driver
*                       this value is set to SNS_DDF_SENSOR__ALL.
* @param[in] attrib     Attribute to be set.
* @param[in] value      Value to set this attribute.
*
* @return Success if the value of the attribute was set properly. Otherwise
*         a specific error code is returned.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm099xx_set_attrib(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_dd_mag_akm099xx_state_s *state = (sns_dd_mag_akm099xx_state_s *)dd_handle;
  sns_ddf_powerstate_e powerstate;

  /* Input sanity check */
  if (state == NULL) {
    return SNS_DDF_EINVALID_PARAM;
  }

  if ((sensor != SNS_DDF_SENSOR_MAG) && (sensor != SNS_DDF_SENSOR__ALL)) {
    return SNS_DDF_EINVALID_PARAM;
  }

#ifdef SNSD_MAG_AKM099XX_DEBUG_FUNC
  MAG_AKM_DEBUG_MSG_2(LOW, "set_attr: sensor=%d, attr=%d", sensor, attrib);
#endif

  switch(attrib) {
    case SNS_DDF_ATTRIB_POWER_STATE:
      {
        powerstate = *(sns_ddf_powerstate_e *)value;
        status = sns_mgt_akm_config_powerstate(state, powerstate);
		/* resetting prev_ts stamp here for polling case. prev_ts stamp will
		   be populated in sns_mgt_akm_sample_sensor() when first polling cycle is complete. */
		if(state->gpio1 == 0xFFFF)
		{
		  state->time_prev_int = 0;
		}		
      }
      break;
    case SNS_DDF_ATTRIB_ODR:
      {
        /* ODR=0 can be used to disable sampling */
        sns_ddf_odr_t             desired_odr = *(sns_ddf_odr_t *)value;
        sns_ddf_odr_t             set_odr     = AKM_MAG099XX_SUPPORTED_ODR_POWER_DOWN;
        mag099xx_operation_mode_e mode        = 0;

        /* In case of HD22_v30, sns_ddf_odr_t is unsigned int */
        if (desired_odr == AKM_MAG099XX_SUPPORTED_ODR_POWER_DOWN) {
          mode    = AKM_MAG_PWR_DOWN_MODE_099XX;
          set_odr = AKM_MAG099XX_SUPPORTED_ODR_POWER_DOWN;
          if( state->curr_odr != set_odr )
          {
            state->sampling_intvl_factor =
              state->avg_sampling_intvl/(float)sns_ddf_convert_usec_to_tick(1000000 / state->curr_odr);
            MAG_AKM_DEBUG_MSG_2(MED, "Setting sampling factor %d.%03d",
                                (int)(state->sampling_intvl_factor),
                                (int)(state->sampling_intvl_factor*1000)%1000);
          }
        } else if (desired_odr <= AKM_MAG099XX_SUPPORTED_ODR_10HZ) {
          if ((state->extended_odr) && (desired_odr <= AKM_MAG099XX_SUPPORTED_ODR_1HZ)) {
            mode    = AKM_MAG_CONT_MEAS_MODE_6_099XX;
            set_odr = AKM_MAG099XX_SUPPORTED_ODR_1HZ;
          } else {
            mode    = AKM_MAG_CONT_MEAS_MODE_1_099XX;
            set_odr = AKM_MAG099XX_SUPPORTED_ODR_10HZ;
          }
        } else if (desired_odr <= AKM_MAG099XX_SUPPORTED_ODR_20HZ) {
		
		  /* In case of polling use 26Hz ODR. This is to address jitter failure in CTS. */
		  if(state->gpio1 == 0xFFFF)
		  {
            mode    = AKM_MAG_CONT_MEAS_MODE_3_099XX;
            set_odr = AKM_MAG099XX_SUPPORTED_ODR_26HZ;
		  }
		  else
		  {
          mode    = AKM_MAG_CONT_MEAS_MODE_2_099XX;
          set_odr = AKM_MAG099XX_SUPPORTED_ODR_20HZ;
		  }
        } else if (desired_odr <= AKM_MAG099XX_SUPPORTED_ODR_50HZ) {
          mode    = AKM_MAG_CONT_MEAS_MODE_3_099XX;
          set_odr = AKM_MAG099XX_SUPPORTED_ODR_50HZ;
        } else if (desired_odr <= AKM_MAG099XX_SUPPORTED_ODR_100HZ) {
          mode    = AKM_MAG_CONT_MEAS_MODE_4_099XX;
          set_odr = AKM_MAG099XX_SUPPORTED_ODR_100HZ;
        } else if ((state->extended_odr) && (desired_odr <= AKM_MAG099XX_SUPPORTED_ODR_200HZ)) {
          mode    = AKM_MAG_CONT_MEAS_MODE_5_099XX;
          set_odr = AKM_MAG099XX_SUPPORTED_ODR_200HZ;
        } else {
          /* When desired_odr exceeds the 100Hz even if device supports 100Hz at maximum
           * in this case, this function returns error.
           */
          MAG_AKM_DEBUG_MSG_1(HIGH, "Invalid ODR %d Hz was specified.", desired_odr);
          return SNS_DDF_EINVALID_ATTR;
        }

        /* avoid chattering */
        if (state->curr_odr == set_odr) {
          MAG_AKM_DEBUG_MSG_1(MED, "Skip because same odr (%d).", set_odr);
          break;
        }

        /* Flush FIFO before ODR change if FIFO is enabled */
        if (state->curr_odr > 0 && state->water_mark > 0) {
          sns_mgt_akm_sample_fifo_099xx(state, FLUSH_FIFO, sns_ddf_get_timestamp(), false);
        }

        /* Before set a new mode, the device must be transition to power-down mode.
         * If the device is already powerdown mode, this command is ignored by
         * the set_operation function. */
        if ((status = sns_mgt_akm_set_operation_mode_099xx(
                state,
                AKM_MAG_PWR_DOWN_MODE_099XX)) != SNS_DDF_SUCCESS) {
          MAG_AKM_DEBUG_MSG_0(HIGH, "Transition to power-down mode failed.");
          return status;
        }

        /* Transition to new mode */
        if ((status = sns_mgt_akm_set_operation_mode_099xx(state, mode)) != SNS_DDF_SUCCESS) {
          MAG_AKM_DEBUG_MSG_1(HIGH, "Transition to mode (%d) failed.", mode);
          return status;
        }

        /* Set internal parameter */
        state->curr_odr = set_odr;
        if( set_odr != 0 )
        {
          state->avg_sampling_intvl =
            (float)sns_ddf_convert_usec_to_tick(1000000 / state->curr_odr)
            * state->sampling_intvl_factor;
          MAG_AKM_DEBUG_MSG_1(MED, "Initial avg sampling intvl %u",
                              (int)(state->avg_sampling_intvl) );
        }

        /* Set powerstate according to current measurement mode */
        if (mode == AKM_MAG_PWR_DOWN_MODE_099XX) {
          state->curr_powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
          state->sample_delay = 0;
        } else {
          uint32_t delay_us;
          /* The mode is one of the continuous measurement */
          state->curr_powerstate = SNS_DDF_POWERSTATE_ACTIVE;
          /* set hardware measurement delay */
          if (state->extended_odr) {
            delay_us = USEC_TIME_FOR_DEVICE_DELAY_EXT;
          } else {
            delay_us = USEC_TIME_FOR_DEVICE_DELAY;
          }

          /* set averaging filter delay.
           * delay = (AVERAGING_NUM - 1) * (1000000 / ODR) / 2
           * DIV_CHECK: When mode is set to other than powerdown,
           *   odr must be positive (not 0) value.
           */
          delay_us += ((AKM_MAG099XX_AVERAGING - 1) * 500000 / set_odr);

          /* convert to tick */
          state->sample_delay = sns_ddf_convert_usec_to_tick(delay_us);

          /* set current time to previous watermark int. */
          //state->time_prev_int = sns_ddf_get_timestamp();

          /* clear data_cache */
          sns_mgt_akm_clear_data_cache(state);

          sns_ddf_smgr_notify_event(state->smgr_handle, SNS_DDF_SENSOR_MAG, SNS_DDF_EVENT_FILTER_DELAY_CHANGED);
        }
        MAG_AKM_DEBUG_MSG_3(MED, "set_odr: odr=%d, powerstate=%d, delay=%ld",
            state->curr_odr,
            state->curr_powerstate,
            state->sample_delay);
      }
      break;
    case SNS_DDF_ATTRIB_FIFO:
      {
        /* When SMGR change FIFO watermark (see section A.3 of reference manual)
         *   Set to WM>0 (i.e. FIFO on)
         *     1. enable_sched_data(false)
         *     2. trigger_fifo_data() to flush FIFO
         *     3. set_attrib() <- This function!
         *     4. enable_sched_data(true)
         *   Set to WM=0 (i.e. FIFO off)
         *     1. trigger_fifo_data() to flush FIFO
         *     2. enable_sched_data(false)
         *     3. set_attrib() <- This function!
         *     4. enable_sched_data(true)
         */
        mag099xx_operation_mode_e reserved_mode;
        uint8_t                   reserved_wm;
        sns_ddf_fifo_set_s        wm = *(sns_ddf_fifo_set_s *)value;

        /* Check if the device supports FIFO. But this should not be happened. */
        if (state->device_fifo_size == 0) {
          MAG_AKM_DEBUG_MSG_0(HIGH, "FIFO is not available on the current device.");
          return SNS_DDF_EINVALID_ATTR;
        }

        /* water mark should be within device_fifo_size */
        if (state->device_fifo_size < wm.watermark) {
          MAG_AKM_DEBUG_MSG_1(HIGH, "  Invalid WM was set. WM = %d.", wm.watermark);
          return SNS_DDF_EINVALID_ATTR;
        }

        /* avoid chattering */
        if (state->water_mark == wm.watermark) {
          MAG_AKM_DEBUG_MSG_1(MED, "Skip because same watermark (%d).", wm.watermark);
          break;
        }

        /* Reserve current mode */
        reserved_mode = state->curr_mode;
        reserved_wm   = state->water_mark;

        /* Transition to power-down mode once.
         * Because watermark bit can be changed only when the device is powerdown mode.
         */
        if ((status = sns_mgt_akm_set_operation_mode_099xx(
                state,
                AKM_MAG_PWR_DOWN_MODE_099XX)) != SNS_DDF_SUCCESS) {
          MAG_AKM_DEBUG_MSG_1(HIGH, "Transition to power-down mode failed. Status = %d.", status);
          return status;
        }

        /* set watermark */
        state->water_mark = wm.watermark;

        /* Transition to original mode. state->curr_mode is already overwritten by
         * previous sns_mgt_akm_set_operation_mode_099xx call, so use reserved parameter.
         * This set_operation function also sets watermark
         */
        if ((status = sns_mgt_akm_set_operation_mode_099xx(
                state,
                reserved_mode)) != SNS_DDF_SUCCESS) {
          MAG_AKM_DEBUG_MSG_1(HIGH, "Set to continuous mode failed. status = %d.", status);
          /* revert watermark */
          state->water_mark = reserved_wm;
          return status;
        }

        /* When FIFO function is turned on, update previous_timestamp */
        if (state->water_mark > 0 && reserved_wm == 0 && state->previous_water_mark == 0) {
          /* set current time to previous watermark int. */
          state->time_prev_int = sns_ddf_get_timestamp();
          state->time_prev_smgr_ev = state->time_prev_int;
          state->num_samples_since_prev_int = 0;
          MAG_AKM_DEBUG_MSG_2(MED, "fifo starts: prev_ts = %u, wm = %d",
              state->time_prev_int,
              state->water_mark);
        }
		state->previous_water_mark = state->water_mark;
        MAG_AKM_DEBUG_MSG_1(MED, "set_fifo: wm=%d", state->water_mark);
      }
      break;
    default:
      return SNS_DDF_EINVALID_ATTR;
  }

  return status;
}

/*===========================================================================

FUNCTION:   sns_dd_akm099xx_get_attrib

===========================================================================*/
/**
* @brief Retrieves the value of an attribute for a sensor.
*
* @param[in]  dd_handle   Handle to a driver instance.
* @param[in]  sensor      Sensor whose attribute is to be retrieved. When
*                         addressing an attribute that refers to the driver
*                         this value is set to SNS_DDF_SENSOR__ALL.
* @param[in]  attrib      Attribute to be retrieved.
* @param[in]  memhandler  Memory handler used to dynamically allocate
*                         output parameters, if applicable.
* @param[out] value       Pointer that this function will allocate or set
*                         to the attribute's value.
* @param[out] num_elems   Number of elements in @a value.
*
* @return Success if the attribute was retrieved and the buffer was
*         populated. Otherwise a specific error code is returned.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm099xx_get_attrib(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    sns_ddf_memhandler_s* memhandler,
    void**                value,
    uint32_t*             num_elems)
{
  sns_dd_mag_akm099xx_state_s *state = (sns_dd_mag_akm099xx_state_s *)dd_handle;

  /* Input sanity check */
  if (state == NULL) {
    return SNS_DDF_EINVALID_PARAM;
  }

  if (sensor != SNS_DDF_SENSOR_MAG) {
    return SNS_DDF_EINVALID_PARAM;
  }

#ifdef SNSD_MAG_AKM099XX_DEBUG_FUNC
    MAG_AKM_DEBUG_MSG_2(LOW, "get_attr: sensor=%d, attr=%d", sensor, attrib);
#endif

  switch(attrib) {
  case SNS_DDF_ATTRIB_POWER_INFO:
    {
      sns_ddf_power_info_s* power_attrib;
      if ((*value = sns_ddf_memhandler_malloc_ex(
              memhandler,
              sizeof(sns_ddf_power_info_s) , state->smgr_handle)) == NULL) {
        return SNS_DDF_ENOMEM;
      }
      power_attrib  = (sns_ddf_power_info_s *)(*value);
      *num_elems    = 1;

      /* Value is unit of uA */
      if (state->device_select == AKM_MAG09916_DEVICE_ID) {
        power_attrib->active_current   = AKM_MAG09916_HI_PWR;
        power_attrib->lowpower_current = AKM_MAG09916_LO_PWR;
      } else if (state->device_select == AKM_MAG09915_DEVICE_ID) {
        power_attrib->active_current   = AKM_MAG09915_HI_PWR;
        power_attrib->lowpower_current = AKM_MAG09915_LO_PWR;
      } else if (state->device_select == AKM_MAG09914_DEVICE_ID) {
        power_attrib->active_current   = AKM_MAG09914_HI_PWR;
        power_attrib->lowpower_current = AKM_MAG09914_LO_PWR;
      } else if (state->device_select == AKM_MAG09913_DEVICE_ID) {
        power_attrib->active_current   = AKM_MAG09913_HI_PWR;
        power_attrib->lowpower_current = AKM_MAG09913_LO_PWR;
      } else if (state->device_select == AKM_MAG09912_DEVICE_ID) {
        power_attrib->active_current   = AKM_MAG09912_HI_PWR;
        power_attrib->lowpower_current = AKM_MAG09912_LO_PWR;
      } else if (state->device_select == AKM_MAG09911_DEVICE_ID) {
        power_attrib->active_current   = AKM_MAG09911_HI_PWR;
        power_attrib->lowpower_current = AKM_MAG09911_LO_PWR;
      } else {
        /* This shouldn't be happened. */
        return SNS_DDF_EFAIL;
      }

    }
    break;

  case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if ((*value = sns_ddf_memhandler_malloc_ex(
              memhandler,
              sizeof(sns_ddf_range_s) , state->smgr_handle)) == NULL) {
        return SNS_DDF_ENOMEM;
      }

      device_ranges  = (sns_ddf_range_s *)(*value);
      *num_elems     = 1;

      /* Value is unit of Gauss in Q16 format */
      if (state->device_select == AKM_MAG09916_DEVICE_ID) {
        device_ranges->min = AKM_MAG09916_MIN_RANGE;
        device_ranges->max = AKM_MAG09916_MAX_RANGE;
      } else if (state->device_select == AKM_MAG09915_DEVICE_ID) {
        device_ranges->min = AKM_MAG09915_MIN_RANGE;
        device_ranges->max = AKM_MAG09915_MAX_RANGE;
      } else if (state->device_select == AKM_MAG09914_DEVICE_ID) {
        device_ranges->min = AKM_MAG09914_MIN_RANGE;
        device_ranges->max = AKM_MAG09914_MAX_RANGE;
      } else if (state->device_select == AKM_MAG09913_DEVICE_ID) {
        device_ranges->min = AKM_MAG09913_MIN_RANGE;
        device_ranges->max = AKM_MAG09913_MAX_RANGE;
      } else if (state->device_select == AKM_MAG09912_DEVICE_ID) {
        device_ranges->min = AKM_MAG09912_MIN_RANGE;
        device_ranges->max = AKM_MAG09912_MAX_RANGE;
      } else if (state->device_select == AKM_MAG09911_DEVICE_ID) {
        device_ranges->min = AKM_MAG09911_MIN_RANGE;
        device_ranges->max = AKM_MAG09911_MAX_RANGE;
      } else {
        /* This shouldn't be happened. */
        return SNS_DDF_EFAIL;
      }
    }
    break;
  case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
     sns_ddf_resolution_adc_s *device_res_adc;
      if ((*value = sns_ddf_memhandler_malloc_ex(
              memhandler,
              sizeof(sns_ddf_resolution_adc_s) , state->smgr_handle)) == NULL) {
        return SNS_DDF_ENOMEM;
      }

      device_res_adc  = (sns_ddf_resolution_adc_s *)(*value);
      *num_elems      = 1;

      /* Value is integer */
      if (state->device_select == AKM_MAG09916_DEVICE_ID) {
        device_res_adc->bit_len  = AKM_MAG09916_RESOLUTION_ADC;
        device_res_adc->max_freq = AKM_MAG09916_SUPPORTED_ODR;
      } else if (state->device_select == AKM_MAG09915_DEVICE_ID) {
        device_res_adc->bit_len  = AKM_MAG09915_RESOLUTION_ADC;
        if(state->gpio1 == 0xFFFF)
        {
          /* Limit polling max_freq to 50Hz */
          device_res_adc->max_freq = AKM_MAG099XX_SUPPORTED_ODR_50HZ;
        }
        else
        {
          device_res_adc->max_freq = AKM_MAG099XX_SUPPORTED_ODR_50HZ;
        }
      } else if (state->device_select == AKM_MAG09914_DEVICE_ID) {
        device_res_adc->bit_len  = AKM_MAG09914_RESOLUTION_ADC;
        device_res_adc->max_freq = AKM_MAG09914_SUPPORTED_ODR;
      } else if (state->device_select == AKM_MAG09913_DEVICE_ID) {
        device_res_adc->bit_len  = AKM_MAG09913_RESOLUTION_ADC;
        device_res_adc->max_freq = AKM_MAG09913_SUPPORTED_ODR;
      } else if (state->device_select == AKM_MAG09912_DEVICE_ID) {
        device_res_adc->bit_len  = AKM_MAG09912_RESOLUTION_ADC;
        device_res_adc->max_freq = AKM_MAG09912_SUPPORTED_ODR;
      } else if (state->device_select == AKM_MAG09911_DEVICE_ID) {
        device_res_adc->bit_len  = AKM_MAG09911_RESOLUTION_ADC;
        if(state->gpio1 == 0xFFFF)
        {
          /* Limit polling max_freq to 50Hz */
          device_res_adc->max_freq = AKM_MAG099XX_SUPPORTED_ODR_50HZ;
        }
        else
        {
          device_res_adc->max_freq = AKM_MAG09911_SUPPORTED_ODR;
        }
      } else {
        /* This shouldn't be happened. */
        return SNS_DDF_EFAIL;
      }
    }
    break;
  case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t *device_res;
      if ((*value = sns_ddf_memhandler_malloc_ex(
              memhandler,
              sizeof(sns_ddf_resolution_t) , state->smgr_handle)) == NULL) {
        return SNS_DDF_ENOMEM;
      }
      /* Value is in Q16 in sensor unit */
      device_res  = (sns_ddf_resolution_t *)(*value);
      *num_elems  = 1;
      *device_res = FX_FLTTOFIX_Q16(state->mag_scale_flt);
    }
    break;
  case SNS_DDF_ATTRIB_DELAYS:
    {
      sns_ddf_delays_s *device_delay;
      if ((*value = sns_ddf_memhandler_malloc_ex(
              memhandler,
              sizeof(sns_ddf_delays_s) , state->smgr_handle)) == NULL) {
        return SNS_DDF_ENOMEM;
      }

      device_delay  = (sns_ddf_delays_s *)(*value);
      *num_elems    = 1;
      /* Value is in int in usec */
      device_delay->time_to_active  = USEC_TIME_TO_ACTIVE;

      if (state->extended_odr) {
        device_delay->time_to_data    = USEC_TIME_FOR_MEASUREMENT_EXT;
      } else {
        device_delay->time_to_data    = USEC_TIME_FOR_MEASUREMENT;
      }
    }
    break;
  case SNS_DDF_ATTRIB_DRIVER_INFO:
    {
      sns_ddf_driver_info_s *driver_info;
      if ((*value = sns_ddf_memhandler_malloc_ex(
              memhandler,
              sizeof(sns_ddf_driver_info_s) , state->smgr_handle)) == NULL) {
        return SNS_DDF_ENOMEM;
      }

      driver_info  = (sns_ddf_driver_info_s *)(*value);
      *num_elems   = 1;
      driver_info->name    = AKM_MAG_DRIVER_NAME;
      driver_info->version = AKM_MAG_DRIVER_VERSION;
    }
    break;

  case SNS_DDF_ATTRIB_DEVICE_INFO:
    {
      sns_ddf_device_info_s *device_info;
      if ((*value = sns_ddf_memhandler_malloc_ex(
              memhandler,
              sizeof(sns_ddf_device_info_s) , state->smgr_handle)) == NULL) {
        return SNS_DDF_ENOMEM;
      }

      device_info  = (sns_ddf_device_info_s *)(*value);
      *num_elems   = 1;
      if (state->device_select == AKM_MAG09916_DEVICE_ID) {
        device_info->model = "AK09916";
      } else if (state->device_select == AKM_MAG09915_DEVICE_ID) {
        device_info->model = "AK09915";
      } else if (state->device_select == AKM_MAG09914_DEVICE_ID) {
        device_info->model = "AK09914";
      } else if (state->device_select == AKM_MAG09913_DEVICE_ID) {
        device_info->model = "AK09913";
      } else if (state->device_select == AKM_MAG09912_DEVICE_ID) {
        device_info->model = "AK09912";
      } else if (state->device_select == AKM_MAG09911_DEVICE_ID) {
        device_info->model = "AK09911";
      } else {
        /* This should not be happened. */
        return SNS_DDF_EFAIL;
      }

      device_info->vendor  = AKM_MAG_DEVICE_VENDOR;
      device_info->name    = AKM_MAG_DEVICE_NAME;
      device_info->version = AKM_MAG_DEVICE_VERSION;

    }
    break;

  case SNS_DDF_ATTRIB_ODR:
    {
      sns_ddf_odr_t *curr_odr;
      if ((curr_odr = sns_ddf_memhandler_malloc_ex(
              memhandler,
              sizeof(sns_ddf_odr_t) , state->smgr_handle)) == NULL) {
        return SNS_DDF_ENOMEM;
      }
      *curr_odr = state->curr_odr;
      *value = curr_odr;
    }
    break;
  case SNS_DDF_ATTRIB_ODR_TOLERANCE:
    {
      sns_ddf_odr_tolerance_t *odr_tolerance;
      if ((odr_tolerance = sns_ddf_memhandler_malloc_ex(
              memhandler,
              sizeof(sns_ddf_odr_tolerance_t) , state->smgr_handle)) == NULL) {
        return SNS_DDF_ENOMEM;
      }
      *odr_tolerance = FX_FLTTOFIX_Q16(AKM_MAG099XX_ODR_TOLERANCE);
      *value = odr_tolerance;
    }
    break;
  case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
    {
      uint32_t *odr_list;
      uint32_t num_odr;

      if (state->extended_odr) {
        num_odr = AKM_MAG099XX_NUM_SUPPORTED_ODR_EXT;
      } else {
        num_odr = AKM_MAG099XX_NUM_SUPPORTED_ODR;
      }

      if ((odr_list = (uint32_t *)sns_ddf_memhandler_malloc_ex(
              memhandler,
              num_odr * sizeof(uint32_t) , state->smgr_handle)) == NULL) {
        return SNS_DDF_ENOMEM;
      }
      if (state->extended_odr) {
        odr_list[0] = AKM_MAG099XX_SUPPORTED_ODR_1HZ;
        odr_list[1] = AKM_MAG099XX_SUPPORTED_ODR_10HZ;
        odr_list[2] = AKM_MAG099XX_SUPPORTED_ODR_20HZ;
        odr_list[3] = AKM_MAG099XX_SUPPORTED_ODR_50HZ;
        odr_list[4] = AKM_MAG099XX_SUPPORTED_ODR_100HZ;
        odr_list[5] = AKM_MAG099XX_SUPPORTED_ODR_200HZ;
      } else {
        odr_list[0] = AKM_MAG099XX_SUPPORTED_ODR_10HZ;
        odr_list[1] = AKM_MAG099XX_SUPPORTED_ODR_20HZ;
        odr_list[2] = AKM_MAG099XX_SUPPORTED_ODR_50HZ;
        odr_list[3] = AKM_MAG099XX_SUPPORTED_ODR_100HZ;
      }
      *value = (uint32_t *)odr_list;
      *num_elems = num_odr;
    }
    break;
  case SNS_DDF_ATTRIB_FIFO:
    {
      sns_ddf_fifo_attrib_get_s *fifo_info;

      if ((fifo_info = (sns_ddf_fifo_attrib_get_s *)sns_ddf_memhandler_malloc_ex(
              memhandler,
              sizeof(sns_ddf_fifo_attrib_get_s) , state->smgr_handle)) == NULL) {
        return SNS_DDF_ENOMEM;
      }

      if (state->device_fifo_size != 0) {
        /* The device has FIFO */
        fifo_info->is_supported     = true;
        fifo_info->is_sw_watermark  = false;
        /* FIFO depth in number of sensor samples */
        fifo_info->max_watermark    = state->device_fifo_size;
        /* FIFO is not shared with other sensor */
        fifo_info->share_sensor_cnt = 0;
        fifo_info->share_sensors[0] = NULL;
      } else {
        /* The device does NOT have FIFO */
        fifo_info->is_supported     = false;
        fifo_info->is_sw_watermark  = false;
        fifo_info->max_watermark    = 0;
        /* FIFO is not shared with other sensor */
        fifo_info->share_sensor_cnt = 0;
        fifo_info->share_sensors[0] = NULL;
      }

      *value = fifo_info;
    }
    break;
  case SNS_DDF_ATTRIB_FILTER_DELAY:
    {
      sns_ddf_filter_delay_t *delay;
      if ((delay = sns_ddf_memhandler_malloc_ex(
              memhandler,
              sizeof(sns_ddf_filter_delay_t) , state->smgr_handle)) == NULL) {
        return SNS_DDF_ENOMEM;
      }
      //*delay = AKM_MAG099XX_FILTER_DELAY;
      *delay = (sns_ddf_filter_delay_t)state->sample_delay;
      *value = delay;
    }
    break;

  default:
    MAG_AKM_DEBUG_MSG_0(HIGH, "Undefined Parameter is requested.");
    return SNS_DDF_EINVALID_PARAM;
  }
  return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_akm099xx_run_test

===========================================================================*/
/**
* @brief Runs a factory test case.
*
* @param[in]  dd_handle  Handle to a driver instance.
* @param[in]  sensor     Sensor on which to run the test.
* @param[in]  test       Test case to run.
* @param[out] err        Optional driver-specific error code.
*
* @return One of the following error codes:
*     SNS_DDF_SUCCESS        - Test passed.
*     SNS_DDF_PENDING        - Test result will be sent as an event.
*     SNS_DDF_EDEVICE_BUSY   - Device is busy streaming, cannot run test.
*     SNS_DDF_EINVALID_TEST  - Test is not defined for this sensor.
*     SNS_DDF_EINVALID_PARAM - One of the parameters is invalid.
*     SNS_DDF_EFAIL          - Unknown error occurred.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm099xx_run_test(
    sns_ddf_handle_t    dd_handle,
    sns_ddf_sensor_e    sensor_type,
    sns_ddf_test_e      test,
    uint32_t*           err)
{
  sns_dd_mag_akm099xx_state_s *state = (sns_dd_mag_akm099xx_state_s *)dd_handle;
  sns_ddf_status_e            status = SNS_DDF_SUCCESS;

  /* Input sanity check */
  if (state == NULL || err == NULL) {
    return SNS_DDF_EINVALID_PARAM;
  }

  if (sensor_type != SNS_DDF_SENSOR_MAG) {
    return SNS_DDF_EINVALID_PARAM;
  }

#ifdef SNSD_MAG_AKM099XX_DEBUG_FUNC
  MAG_AKM_DEBUG_MSG_1(LOW, "self-test: test=%d", test);
#endif

  /* check power state */
  if (state->curr_powerstate == SNS_DDF_POWERSTATE_ACTIVE) {
    MAG_AKM_DEBUG_MSG_0(HIGH, "Transition to test mode is only from power down mode.");
    return SNS_DDF_EDEVICE_BUSY;
  }
  /* set power state */
  state->curr_powerstate = SNS_DDF_POWERSTATE_ACTIVE;
  /* set err code first */
  *err = 0;

  /* perform connectivity test */
  if (test == SNS_DDF_TEST_SELF) {
    uint8_t device_id[2] = {0, 0};

    status = sns_mgt_akm_check_device(state->port_handle, device_id);

    /* If id is not correct, set read id to error code. */
    if (status != SNS_DDF_SUCCESS) {
      *err = ((TLIMIT_NO_INVALID_ID) << 16);
      *err |= ((((uint16_t)device_id[1]) << 8) + device_id[0]);
    }
  } else if (test == SNS_DDF_TEST_SELF_SW) {
    status = sns_mgt_akm_test_self_sw(state, err);
  } else {
    /* According to driver reference manual section 4.6,
     * the retrun value must be SNS_DDF_EINVALID_TEST when the
     * test type is not supported.
     */
    status = SNS_DDF_EINVALID_TEST;
    MAG_AKM_DEBUG_MSG_1(HIGH, "Requested test (test = %d) is not supported.", test);
  }

  /* set power state */
  state->curr_powerstate = SNS_DDF_POWERSTATE_LOWPOWER;

  return status;
}

/*===========================================================================

FUNCTION:   sns_dd_akm099xx_probe

===========================================================================*/
/**
* @brief Probes for the device with a given configuration.

* @param[in]  dev_info    Access info for physical devices controlled by
*                         this driver. Used to determine if the device is
*                         physically present.
* @param[in]  memhandler  Memory handler used to dynamically allocate
*                         output parameters, if applicable.
* @param[out] num_sensors Number of sensors supported. 0 if none.
* @param[out] sensor_type Array of sensor types supported, with num_sensor
*                         elements. Allocated by this function.
*
* @return SNS_DDF_SUCCESS if the part was probed function completed, even
*         if no device was found (in which case num_sensors will be set to
*         0).
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm099xx_probe(
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors)
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  uint8_t          device_id[2];

  /* "num = 0" means Nothing detected. */
  *num_sensors = 0;
  *sensors     = NULL;
  /* No error code is reserved, but put something. */
  device_info->device_select = 0xFF;

  if ((status = sns_ddf_open_port(
          &port_handle,
          &(device_info->port_config))) != SNS_DDF_SUCCESS) {
    MAG_AKM_DEBUG_MSG_1(HIGH, "Port open failed. Status = %d.", status);
    return status;
  }

  /* Read Device ID, to make sure device is working properly */
  if ((status = sns_mgt_akm_check_device(port_handle, device_id)) != SNS_DDF_SUCCESS) {
    goto FINALIZE_PROBE;
  }

  /** 'status' should be SNS_DDF_SUCCESS at this line **/

  /* Put Device ID to 'device_info' struct for using in 'init' function */
  device_info->device_select = device_id[1];

  /* This is probably an AK099XX
   * In probe function, you can use memhandler_malloc, no memhandler_malloc_ex.
   * see section 11.2.5 of 80-N4080-1-T
   */
  *sensors = sns_ddf_memhandler_malloc(
      memhandler,
      sizeof(sns_ddf_sensor_e) * AKM_MAG099XX_NUM_SUPPORTED_SENSORS);

  if (*sensors == NULL) {
    status = SNS_DDF_ENOMEM;
    MAG_AKM_DEBUG_MSG_0(HIGH, "Couldn't allocate memory.");
    goto FINALIZE_PROBE;
  }

  /* set return value */
  (*sensors)[0] = SNS_DDF_SENSOR_MAG;
  *num_sensors  = AKM_MAG099XX_NUM_SUPPORTED_SENSORS;

  status        = SNS_DDF_SUCCESS;

FINALIZE_PROBE:
  sns_ddf_close_port(port_handle);
  return status;
}

/*===========================================================================

FUNCTION:   sns_dd_akm099xx_enable_sched_data

===========================================================================*/
/**
* @brief Begins device-scheduled sampling and enables notification via Data
*        Ready Interrupts (DRI).
*
* @param[in] handle  Handle to the driver's instance.
* @param[in] sensor  Sensor to be sampled.
* @param[in] enable  True to enable or false to disable data stream.
*
* @return SNS_DDF_SUCCESS if sensor was successfully configured and
*         internal sampling has commenced or ceased. Otherwise an
*         appropriate error code.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_akm099xx_enable_sched_data(
    sns_ddf_handle_t  handle,
    sns_ddf_sensor_e  sensor,
    bool              enable)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_dd_mag_akm099xx_state_s *state = (sns_dd_mag_akm099xx_state_s *)handle;

  /* Input sanity check */
  if (state == NULL) {
    return SNS_DDF_EINVALID_PARAM;
  }

  /* check parameter - sensor */
  if (sensor != SNS_DDF_SENSOR_MAG) {
    return SNS_DDF_EINVALID_PARAM;
  }

#ifdef SNSD_MAG_AKM099XX_DEBUG_FUNC
  MAG_AKM_DEBUG_MSG_2(
      LOW, "enable_sched_data: sensor=%d, enable=%d",
      sensor, (enable ? 1 : 0));
#endif

  /* Check if the platform supports interrupts. */
  if ( !sns_ddf_signal_irq_enabled() || (state->gpio1 == 0)) {
    return SNS_DDF_EFAIL;
  }

  /* If the device doesn't have DRI pin, it doesn't support sched_data. */
  if (!state->has_drdy) {
    MAG_AKM_DEBUG_MSG_0(HIGH, "This device does not have DRDY.");
    return SNS_DDF_EDEVICE;
  }

  /* interrupt signal line of sensor is always enabled. */
  if (enable) {
    /* Enables DRI */
    status = sns_ddf_signal_register(
        state->gpio1,
        state,
        &AKM099xx_FIFO_DD_IF,
        SNS_DDF_SIGNAL_IRQ_RISING);
  } else {
    /* Disables DRI */
    sns_ddf_signal_deregister(state->gpio1);
  }

  return status;
}
