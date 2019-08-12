/**
 * @file sns_dd_mpu6515_selftest.c
 * @brief Test implementation for MPU-6xxx driver
 *
 * Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mpu6515_selftest.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2013-07-12  sd   each sensor use its own axe map
  2013-07-11 pn   Re-enabled HW selftest; removed SW selftest
  2013-06-20 lka  Initial version

==============================================================================*/

#include "sns_dd_mpu6515_priv.h"

#include <stdbool.h>
#include <stdio.h>
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_util.h"
#include "sns_dd_inertial_test.h"
#include "sns_memmgr.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"

#define MPU6XXX_ST_DEBUG
#ifdef MPU6XXX_ST_DEBUG
#ifndef PRIMITIVE_DEBUG_MSG
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define MPU_ST_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, msg)
#define MPU_ST_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1)
#define MPU_ST_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1,p2)
#define MPU_ST_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1,p2,p3)
#define MPU_ST_DATA_PRINTF(level,msg,p1,p2,p3)  MPU_ST_MSG_3(level,msg,p1,p2,p3)
#else
#define MPU_ST_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define MPU_ST_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define MPU_ST_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define MPU_ST_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#define MPU_ST_DATA_PRINTF(level,msg,p1,p2,p3)  \
   SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_ASYNC_DATA_VALUE,p1,p2,p3)
#define MED MEDIUM
#endif
#else
#define MPU_ST_MSG_0(level,msg)
#define MPU_ST_MSG_1(level,msg,p1)
#define MPU_ST_MSG_2(level,msg,p1,p2)
#define MPU_ST_MSG_3(level,msg,p1,p2,p3)
#define MPU_ST_DATA_PRINTF(level,msg,p1,p2,p3)
#endif

#define MPU6515_USECS_BETWEEN_COLLECTS  2000  // 500Hz sampling rate
#define MPU6515_TEST_SAMPLES_TO_COLLECT   200

typedef enum
{
  MPU6XXX_HW_ST_STATE_ST_DISABLED,
  MPU6XXX_HW_ST_STATE_ST_ENABLED,
  MPU6XXX_HW_ST_NUM_STATES
} mpu6515_hw_st_state_e;

typedef enum
{
  SNS_DD_TEST_SUCCESS         = 0,
  SNS_DD_TEST_I2C_ERR         = 1,
  SNS_DD_TEST_FAIL            = 2
} sns_dd_test_err_e;

typedef struct
{
  int32_t  lower_limit;
  int32_t  upper_limit;
} mpu6515_hw_test_specs_s;

typedef struct
{
  q16_t sample_sum;
  q16_t sample_avg[MPU6XXX_HW_ST_NUM_STATES];
} sns_dd_st_data_s;

struct mpu6515_hw_st_s
{
  mpu6515_sensor_s*            sensor;
  mpu6515_sub_dev_s*           sub_dev;
  sns_ddf_handle_t             smgr_handle;
  sns_ddf_timer_s              timer;
  mpu6515_hw_st_state_e        st_state;
  sns_ddf_memhandler_s         memhandler;
  uint16_t                     num_samples;
  uint16_t                     sample_count;
  int16_t                      read_attempts;
  sns_dd_st_data_s             samples[MPU6XXX_NUM_INERTIAL_AXES];
  uint8_t                      st_reg_vals[MPU6XXX_NUM_INERTIAL_AXES];
  uint8_t                      st_reg_addr[MPU6XXX_NUM_INERTIAL_AXES];
  uint8_t                      range_idx;
  uint32_t                     usec_between_collects;
  q16_t                        str[MPU6XXX_NUM_INERTIAL_AXES];
  q16_t                        sensitivity;
  const q16_t*                 data_cache_ptr;
  sns_ddf_axes_map_s*          axes_map_ptr; // should really be const
  const mpu6515_hw_test_specs_s* test_specs_ptr;
};

/*
 * ST_OTP Table
 * ST_OTP = round((2620/2^FS) * 1.01^(ST_code-1)) (lsb)
 * ST_code obtained from OTP register and used to index this array
 */
static const uint16_t mpu6515_st_otp[] = {
   2620,  2646,  2673,  2699,  2726,  2754,  2781,  2809,  2837,  2865,
   2923,  2952,  2982,  3012,  3042,  3072,  3103,  3134,  3165,  3197,
   3229,  3261,  3294,  3327,  3360,  3394,  3428,  3462,  3496,  3531,
   3567,  3602,  3638,  3675,  3711,  3749,  3786,  3824,  3862,  3901,
   3940,  3979,  4019,  4059,  4100,  4141,  4182,  4224,  4266,  4309,
   4352,  4396,  4440,  4484,  4529,  4574,  4620,  4666,  4713,  4760,
   4807,  4855,  4904,  4953,  5003,  5053,  5103,  5154,  5206,  5258,
   5310,  5363,  5417,  5471,  5526,  5581,  5637,  5693,  5750,  5808,
   5866,  5925,  5984,  6044,  6104,  6165,  6227,  6289,  6352,  6415,
   6480,  6544,  6610,  6676,  6743,  6810,  6878,  6947,  7016,  7087,
   7157,  7229,  7301,  7374,  7448,  7523,  7598,  7674,  7751,  7828,
   7906,  7985,  8065,  8146,  8227,  8310,  8393,  8477,  8561,  8647,
   8733,  8821,  8909,  8998,  9088,  9179,  9271,  9363,  9457,  9552,
   9647,  9744,  9841,  9940, 10039, 10139, 10241, 10343, 10447, 10551,
  10657, 10763, 10871, 10979, 11089, 11200, 11312, 11425, 11539, 11655,
  11771, 11889, 12008, 12128, 12249, 12372, 12496, 12621, 12747, 12874,
  13003, 13133, 13264, 13397, 13531, 13666, 13803, 13941, 14080, 14221,
  14363, 14507, 14652, 14799, 14947, 15096, 15247, 15399, 15553, 15709,
  15866, 16025, 16185, 16347, 16510, 16675, 16842, 17011, 17181, 17353,
  17526, 17701, 17878, 18057, 18238, 18420, 18604, 18790, 18978, 19168,
  19360, 19553, 19749, 19946, 20146, 20347, 20551, 20756, 20964, 21173,
  21385, 21599, 21815, 22033, 22253, 22476, 22701, 22928, 23157, 23389,
  23622, 23859, 24097, 24338, 24582, 24827, 25076, 25326, 25580, 25836,
  26094, 26355, 26618, 26885, 27153, 27425, 27699, 27976, 28256, 28538,
  28824, 29112, 29403, 29697, 29994, 30294, 30597, 30903, 31212, 31524,
  31839, 32158, 32479, 32804
};

/* MPU6500 HW self-test limits */
const mpu6515_hw_test_specs_s mpu6515_accel_str_limits =
{
  FX_FLTTOFIX_Q16(0.225*G),   // min, in G
  FX_FLTTOFIX_Q16(0.675*G)    // max, in G
};
const mpu6515_hw_test_specs_s mpu6515_gyro_str_limits =
{
  MPU6050_DPS_TO_RPSQ16(60),  // min, in radians/sec
  MPU6050_DPS_TO_RPSQ16(250)  // max, in radians/sec,
                              //      arbitrary as no max is specified in specs
};

const mpu6515_hw_test_specs_s mpu6515_accel_ratio_limits =
{
  FX_FLTTOFIX_Q16(0.0), // min, in %
  FX_FLTTOFIX_Q16(0.5)  // max, in %
};

const mpu6515_hw_test_specs_s mpu6515_gyro_ratio_limits =
{
  FX_FLTTOFIX_Q16(0.5), // min, in %
  FX_FLTTOFIX_Q16(5.0)  // max, arbitrary as no max is specified in specs
};

const q16_t mpu6515_accel_offset_limit = FX_FLTTOFIX_Q16(0.5*G);

const q16_t mpu6515_gyro_offset_limit = MPU6050_DPS_TO_RPSQ16(20);

extern sns_ddf_driver_if_s sns_dd_mpu6515_if;

/*
*******************************************************************************
*/
/**
 * @brief Releases memory allocated for HW self-test.
 *
 */
static void mpu6515_HW_test_cleanup(mpu6515_s* state)
{
  if(state->hw_st_ptr != NULL)
  {
    if(state->hw_st_ptr->timer != NULL)
    {
      sns_ddf_timer_release(state->hw_st_ptr->timer);
      state->hw_st_ptr->timer = NULL;
    }
    sns_ddf_mfree(state->hw_st_ptr);
    state->hw_st_ptr = NULL;
  }
}

/**
 * @brief Computes the self test response
 *
 *   STR = Sensor output with self-test enabled -
 *         Sensor output without self-test enabled
*/
static void mpu6515_compute_str(mpu6515_hw_st_s* st_ptr, boolean convert)
{
  uint8_t i;
  for(i=0; i<ARR_SIZE(st_ptr->samples); i++)
  {
    sns_dd_st_data_s* st_data_ptr = &st_ptr->samples[i];
    st_ptr->str[i] = (st_data_ptr->sample_avg[MPU6XXX_HW_ST_STATE_ST_ENABLED] -
                      st_data_ptr->sample_avg[MPU6XXX_HW_ST_STATE_ST_DISABLED]);
    if (convert)
    {
      /* converts back to mGee for ACCEL and to dps for GYRO */
      st_ptr->str[i] = FX_DIV_Q16(st_ptr->str[i], st_ptr->sensitivity);
    }
  }
  MPU_ST_DATA_PRINTF(LOW, "STR    0x%08x 0x%08x 0x%08x",
                     st_ptr->str[X], st_ptr->str[Y], st_ptr->str[Z]);
}

static sns_ddf_status_e mpu6515_hw_st_criteria_c(mpu6515_hw_st_s* st_ptr)
{
  uint8_t axes_validated = 0;
  uint8_t i;

  q16_t offset_limit = (st_ptr->sensor->sensor_type == SNS_DDF_SENSOR_ACCEL) ?
    mpu6515_accel_offset_limit : mpu6515_gyro_offset_limit;

  for ( i=0; i<ARR_SIZE(st_ptr->samples); i++ )
  {
    q16_t offset = st_ptr->samples[i].sample_avg[MPU6XXX_HW_ST_STATE_ST_DISABLED];

    if ( (st_ptr->sensor->sensor_type == SNS_DDF_SENSOR_ACCEL) && 
         (i == st_ptr->axes_map_ptr->indx_z) )
    {
      MPU_ST_MSG_2(LOW, "criteria_c - offset=0x%08x sign=%d",
                   offset, st_ptr->axes_map_ptr->sign_z);
      offset *= st_ptr->axes_map_ptr->sign_z;
      offset += FX_FLTTOFIX_Q16(G);
    }

    MPU_ST_MSG_3(LOW, "criteria_c - i=%d |offset|=0x%08x limit=0x%08x",
                 i, FX_ABS(offset), offset_limit);
    if ( FX_ABS(offset) <= offset_limit )
    {
      axes_validated++;
    }
    else
    {
      MPU_ST_MSG_1(ERROR, "Criteria C fails i=%d", i);
    }
  }
  if ( axes_validated == MPU6XXX_NUM_INERTIAL_AXES )
  {
    MPU_ST_MSG_0(HIGH, "MPU6500 HW Selftest passed criteria C!");
  }
  return (axes_validated == MPU6XXX_NUM_INERTIAL_AXES) ?
    SNS_DDF_SUCCESS : SNS_DDF_EFAIL;
}

/**
 * @brief Evaluates HW self-test data using Factory Trim info for MPU6500
 *
 */
static sns_ddf_status_e mpu6515_hw_st_eval_fine(mpu6515_hw_st_s* st_ptr)
{
  const mpu6515_hw_test_specs_s* ratio_limits_ptr;
  q16_t q16st_ratio[MPU6XXX_NUM_INERTIAL_AXES];
  q16_t q16st_otp[MPU6XXX_NUM_INERTIAL_AXES];
  const uint16_t* st_otp_ptr;
  uint8_t axes_validated = 0;
  uint8_t i;

  ratio_limits_ptr = (st_ptr->sensor->sensor_type == SNS_DDF_SENSOR_ACCEL) ?
    &mpu6515_accel_ratio_limits : &mpu6515_gyro_ratio_limits;
  st_otp_ptr = mpu6515_st_otp;
  for(i=0; i<ARR_SIZE(q16st_otp); i++)
  {
    q16st_otp[i]   = FX_FLTTOFIX_Q16(st_otp_ptr[st_ptr->st_reg_vals[i]-1]);
    q16st_ratio[i] = FX_DIV_Q16(st_ptr->str[i], q16st_otp[i]);
    if (st_ptr->sensor->sensor_type == SNS_DDF_SENSOR_ACCEL)
    {
      q16st_ratio[i] = FX_ABS(q16st_ratio[i] - FX_ONE_Q16);
    }
    if ( (q16st_ratio[i] > ratio_limits_ptr->lower_limit) &&
         (q16st_ratio[i] < ratio_limits_ptr->upper_limit) )
    {
      axes_validated++;
    }
    else
    {
      MPU_ST_MSG_3(ERROR, "Sensor %d failed criteria A - i=%d ratio=0x%x",
                   st_ptr->sensor, i, q16st_ratio[i]);
    }
  }
  MPU_ST_DATA_PRINTF(LOW, "ST_OTP 0x%08x 0x%08x 0x%08x",
                     q16st_otp[X], q16st_otp[Y], q16st_otp[Z]);
  MPU_ST_DATA_PRINTF(LOW, "Ratio  0x%08x 0x%08x 0x%08x",
                     q16st_ratio[X], q16st_ratio[Y], q16st_ratio[Z]);

  if ( axes_validated == MPU6XXX_NUM_INERTIAL_AXES )
  {
    MPU_ST_MSG_0(HIGH, "MPU6500 HW Selftest passed criteria A!");
  }
  return (axes_validated == MPU6XXX_NUM_INERTIAL_AXES) ?
    SNS_DDF_SUCCESS : SNS_DDF_EFAIL;
}

/**
 * @brief Evaluates HW self-test data without Factory Trim info.
*/
static sns_ddf_status_e mpu6515_hw_st_eval_coarse(
  mpu6515_hw_st_s* st_ptr
)
{
  uint8_t axes_validated = 0;
  uint8_t i;
  for(i=0; i<ARR_SIZE(st_ptr->samples); i++)
  {
    if( (FX_ABS(st_ptr->str[i]) >= st_ptr->test_specs_ptr->lower_limit) &&
        (FX_ABS(st_ptr->str[i]) <= st_ptr->test_specs_ptr->upper_limit) )
    {
      axes_validated++;
    }
    else
    {
      MPU_ST_MSG_3(ERROR, "eval_coarse i=%d low=0x%x hi=0x%x",
                   i, st_ptr->test_specs_ptr->lower_limit,
                   st_ptr->test_specs_ptr->upper_limit);
    }
  }
  if ( axes_validated == MPU6XXX_NUM_INERTIAL_AXES )
  {
    MPU_ST_MSG_0(HIGH, "MPU6500 HW Selftest passed criteria B!");
  }
  return (axes_validated == MPU6XXX_NUM_INERTIAL_AXES) ?
    SNS_DDF_SUCCESS : SNS_DDF_EFAIL;
}


/**
 * @brief Reads the self-test registers.
 *
 * @details Considered failed if all registers hold zeroes.
 */
static sns_ddf_status_e mpu6515_read_st_reg(
  sns_ddf_handle_t port,
  mpu6515_hw_st_s* st_ptr
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t reg_bits = 0;
  uint8_t bytes_read = 0;
  uint8_t i;

  for( i=0;
       (i < ARR_SIZE(st_ptr->st_reg_addr)) && (st_ptr->st_reg_addr[i] != 0xFF) &&
       (status == SNS_DDF_SUCCESS);
       i++ )
  {
    status = sns_ddf_read_port(port,
                               st_ptr->st_reg_addr[i],
                               &st_ptr->st_reg_vals[i],
                               1,
                               &bytes_read);
    reg_bits |= st_ptr->st_reg_vals[i];
  }
  MPU_ST_DATA_PRINTF(LOW, "ST REGs 0x%02x 0x%02x 0x%02x", st_ptr->st_reg_vals[X],
                     st_ptr->st_reg_vals[Y], st_ptr->st_reg_vals[Z]);
  if(reg_bits == 0)
  {
    status = SNS_DDF_EFAIL;
  }
  return status;
}

/**
 * @brief Extracts biases from HW test data
 *
 * @details
 */
static void mpu6515_hw_st_extract_bias(mpu6515_hw_st_s* st_ptr)
{
  uint8_t i;
  for ( i=0; i<ARR_SIZE(st_ptr->samples); i++ )
  {
    st_ptr->sensor->biases[i] =
      st_ptr->samples[i].sample_avg[MPU6XXX_HW_ST_STATE_ST_DISABLED];
  }
  sns_ddf_map_axes(st_ptr->axes_map_ptr, st_ptr->sensor->biases);

  if (st_ptr->sensor->sensor_type == SNS_DDF_SENSOR_ACCEL)
  {
    /* Z axis for accel needs to be adjusted by G. */
    st_ptr->sensor->biases[Z] += FX_FLTTOFIX_Q16(G);
  }
  for ( i=0; i<ARR_SIZE(st_ptr->samples); i++ )
  {
    /* change the sign of the bias since SMGR expects it this way. */
    st_ptr->sensor->biases[i] *= (-1);
  }
  sns_ddf_smgr_notify_event(st_ptr->smgr_handle, st_ptr->sensor->sensor_type,
                            SNS_DDF_EVENT_BIAS_READY);
}

/**
 * @brief Wraps up HW self-test.
 *
 * @details
 * Enough samples collected with and without self-test enabled.  Depending on
 * whether self-test registers hold non-zeros values or not, different
 * evaluation method is used.
 *
 * @see sns_ddf_notify_test_complete()
 */
static void mpu6515_hw_st_completes(mpu6515_s* state)
{
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  mpu6515_hw_st_s* st_ptr = state->hw_st_ptr;

  if ( st_ptr->sample_count >= st_ptr->num_samples )
  {
    status = mpu6515_read_st_reg(state->port_handle, st_ptr);
    mpu6515_compute_str(st_ptr, (status == SNS_DDF_SUCCESS) ? TRUE : FALSE);
    if(status == SNS_DDF_SUCCESS)
    {
      status = mpu6515_hw_st_eval_fine(st_ptr);
    }
    else
    {
      status = mpu6515_hw_st_eval_coarse(st_ptr);
    }
    if (status == SNS_DDF_SUCCESS)
    {
      status = mpu6515_hw_st_criteria_c(st_ptr);
    }
  }
  /* else, test was aborted */

  sns_ddf_smgr_notify_test_complete(st_ptr->smgr_handle, 
                                    st_ptr->sensor->sensor_type, 
                                    status,
                                    ((status == SNS_DDF_SUCCESS) ?
                                      SNS_DD_TEST_SUCCESS: SNS_DD_TEST_FAIL) );
  if ( status == SNS_DDF_SUCCESS )
  {
    mpu6515_hw_st_extract_bias(st_ptr);
  }
  mpu6515_HW_test_cleanup(state);
}


/**
 * @brief Called when the timer set by this driver has expired.
 *
 */
void mpu6515_handle_st_timer(mpu6515_s* state)
{
  mpu6515_hw_st_s* st_ptr = state->hw_st_ptr;
  sns_ddf_status_e status;
  uint8_t i;

  if(st_ptr->read_attempts >= 0)
  {
    sns_ddf_timer_start(st_ptr->timer, st_ptr->usec_between_collects);
    status = mpu6515_get_data_from_device(state, FALSE);
    st_ptr->read_attempts++;

    if( status == SNS_DDF_SUCCESS )
    {
      st_ptr->sample_count++;
      for(i=0; i<ARR_SIZE(st_ptr->samples); i++)
      {
        st_ptr->samples[i].sample_sum += st_ptr->data_cache_ptr[i];
      }
    }
    else if ( st_ptr->read_attempts >= (st_ptr->num_samples << 1) )
    {
      /* has tried twice the number of planned readings; bail */
      MPU_ST_MSG_3(ERROR, "handle_st_timer - attempts=%d samples=%d status=%d",
                   st_ptr->read_attempts, st_ptr->sample_count, status);
      st_ptr->read_attempts = -1;
      st_ptr->st_state = MPU6XXX_HW_ST_NUM_STATES;
    }
  }
  if(st_ptr->sample_count >= st_ptr->num_samples)
  {
    MPU_ST_MSG_1(LOW, "handle_st_timer - samples=%d", st_ptr->sample_count);
    MPU_ST_DATA_PRINTF(MEDIUM, "Sum     0x%08x 0x%08x 0x%08x",
                       st_ptr->samples[X].sample_sum,
                       st_ptr->samples[Y].sample_sum,
                       st_ptr->samples[Z].sample_sum);
    for(i=0; i<ARR_SIZE(st_ptr->samples); i++)
    {
      st_ptr->samples[i].sample_avg[st_ptr->st_state] =
        FX_DIV_Q16(st_ptr->samples[i].sample_sum,
                   FX_FLTTOFIX_Q16(st_ptr->num_samples));
      st_ptr->samples[i].sample_sum = 0;
    }
    MPU_ST_DATA_PRINTF(MEDIUM, "Average 0x%08x 0x%08x 0x%08x",
                       st_ptr->samples[X].sample_avg[st_ptr->st_state],
                       st_ptr->samples[Y].sample_avg[st_ptr->st_state],
                       st_ptr->samples[Z].sample_avg[st_ptr->st_state]);
    //prepare for next state of the test
    st_ptr->read_attempts = -1;
    st_ptr->st_state++;
  }
  if(st_ptr->read_attempts == -1)
  {
    st_ptr->read_attempts = 0;
    sns_ddf_timer_cancel(st_ptr->timer);
    MPU_ST_MSG_1(MEDIUM, "handle_st_timer state=%d", st_ptr->st_state);
    switch(st_ptr->st_state)
    {
    case MPU6XXX_HW_ST_STATE_ST_DISABLED:
      st_ptr->sample_count  = 0;
      mpu6515_set_config_register(state, st_ptr->sensor, st_ptr->range_idx, //mpu6515_set_config_register(state, st_ptr->sensor, st_ptr->range_idx,
                                         ACCEL_HPF_None, FALSE);
      sns_ddf_timer_start(st_ptr->timer, MPU6050_IDLE_TO_READY_TIME);
      break;
    case MPU6XXX_HW_ST_STATE_ST_ENABLED:
      st_ptr->sample_count  = 0;
      mpu6515_set_config_register(state, st_ptr->sensor, st_ptr->range_idx, //mpu6515_set_config_register(state, st_ptr->sensor, st_ptr->range_idx,
                                         ACCEL_HPF_None, TRUE);
      sns_ddf_timer_start(st_ptr->timer, MPU6050_IDLE_TO_READY_TIME);
      break;
    default:
      mpu6515_hw_st_completes(state);
    }
  }
}

/**
 * @brief Resets device before running selftest
 *
 */
sns_ddf_status_e mpu6515_st_reset_device(mpu6515_s* state)
{
  sns_ddf_status_e status;
  uint8_t i2c_buff, bytes_written;
  uint32_t  reset_wait_loop;

  MPU6050_MSG_0(LOW, "Performing a hardware reset of the device...");

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
    if(state->device_id != DEVICE_ID_MPU6515)
    {
      mpu6515_validate_device_id(state);
    }
    if ( (SNS_DDF_SUCCESS == status) && 
         ((state->device_id == DEVICE_ID_MPU6515) && !(i2c_buff & DEVICE_RESET_ENABLE)) )
    {
      break;
    }
    sns_ddf_delay(DD_MPU6050_RESET_WAIT_ONCE_US);
  }
  MPU_ST_MSG_3(HIGH, "st_reset_dev - id=0x%x val=0x%x status=%d",
               state->device_id, i2c_buff, status);

  if (DD_MPU6050_RESET_WAIT_LOOP_MAX == reset_wait_loop)
  {
    return SNS_DDF_EDEVICE;
  }
  return mpu6515_set_device_powerstate(state, SNS_DDF_POWERSTATE_ACTIVE);
}

/**
 * @brief Sets up and runs HW self-test
 *
 */
static sns_ddf_status_e mpu6515_run_HW_self_test(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor_type,
    uint32_t*         err
)
{
  mpu6515_s* state = mpu6515_get_device(dd_handle);
  mpu6515_sub_dev_s* sub_device = mpu6515_get_sub_dev(dd_handle);
  mpu6515_sensor_s* sensor = mpu6515_get_sensor(dd_handle, sensor_type);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  
  MPU6050_MSG_2(LOW, "HW_st - dd_handle=0x%x state=0x%x", dd_handle, state);

  if (state == NULL || sub_device == NULL || sensor == NULL)
  {
    MPU_ST_MSG_3(ERROR, "HW_st - state(0x%X) sub_dev(0x%X) sensor(0x%X)", 
                 state, sub_device, sensor);
    return SNS_DDF_EINVALID_PARAM;
  }
  if (mpu6515_is_dmp_active(state))
  {
    MPU_ST_MSG_0(ERROR, "HW_st - DMP is active");
    return SNS_DDF_EDEVICE_BUSY;
  }

  status = sns_ddf_malloc((void**)&(state->hw_st_ptr), sizeof(mpu6515_hw_st_s));
  if(status == SNS_DDF_SUCCESS)
  {
    SNS_OS_MEMZERO(state->hw_st_ptr, sizeof(mpu6515_hw_st_s));
    status = sns_ddf_timer_init(&state->hw_st_ptr->timer,
                                (sns_ddf_handle_t)state,
                                &sns_dd_mpu6515_if,
                                NULL,
                                FALSE);
  }
  if(status == SNS_DDF_SUCCESS)
  {
    mpu6515_hw_st_s* st_ptr = state->hw_st_ptr;

    st_ptr->range_idx         = 0; // +/-2G for Accel and +/-250dps for Gyro
    if ( sensor_type == SNS_DDF_SENSOR_ACCEL )
    {
      st_ptr->smgr_handle     = sub_device->smgr_handle;
      st_ptr->data_cache_ptr  = &state->data_cache[MPU6050_CACHE_ACCELX];
      st_ptr->sensitivity     = mpu6515_accel_sensitivity[st_ptr->range_idx];
      st_ptr->axes_map_ptr    = &sensor->axes_map;
      st_ptr->test_specs_ptr  = &mpu6515_accel_str_limits;
      st_ptr->st_reg_addr[X]  = MPU6500_REG_SELF_TEST_AX;
      st_ptr->st_reg_addr[Y]  = MPU6500_REG_SELF_TEST_AY;
      st_ptr->st_reg_addr[Z]  = MPU6500_REG_SELF_TEST_AZ;
    }
    else
    {
      st_ptr->smgr_handle     = sub_device->smgr_handle;
      st_ptr->data_cache_ptr  = &state->data_cache[MPU6050_CACHE_GYROX];
      st_ptr->sensitivity     = mpu6515_gyro_sensitivity[st_ptr->range_idx] >> 9;
      st_ptr->axes_map_ptr    = &sensor->axes_map;
      st_ptr->test_specs_ptr  = &mpu6515_gyro_str_limits;
      st_ptr->st_reg_addr[X]  = MPU6500_REG_SELF_TEST_GX;
      st_ptr->st_reg_addr[Y]  = MPU6500_REG_SELF_TEST_GY;
      st_ptr->st_reg_addr[Z]  = MPU6500_REG_SELF_TEST_GZ;
    }
    st_ptr->num_samples       = MPU6515_TEST_SAMPLES_TO_COLLECT;
    st_ptr->sensor            = sensor;
    st_ptr->sub_dev           = sub_device;
    st_ptr->read_attempts     = -1;
    st_ptr->usec_between_collects = MPU6515_USECS_BETWEEN_COLLECTS;
    sns_ddf_memhandler_init(&st_ptr->memhandler);
    MPU_ST_MSG_2(LOW, "HW_st - usec=%d samples=%d",
                 st_ptr->usec_between_collects, st_ptr->num_samples);

    status = mpu6515_st_reset_device(state);
    MPU_ST_MSG_3(MEDIUM, "HW_st - sensor_type=%d dev=0x%x reset=%d",
                 sensor_type, state->device_id, status);
    if(status == SNS_DDF_SUCCESS)
    {
      uint16_t config_reg = (sensor_type == SNS_DDF_SENSOR_ACCEL) ?
        MPU6500_REG_ACCEL_CONFIG_2 : MPU6050_REG_CONFIG;
      uint8_t  value = 2;
      uint8_t  bytes_written = 0;
      status = sns_ddf_write_port(state->port_handle, config_reg, &value, 
                                  1, &bytes_written);
      MPU_ST_MSG_3(MEDIUM, "HW_st - reg=0x%x val=0x%x status=%d",
                   config_reg, value, status);
    }
    if(status == SNS_DDF_SUCCESS)
    {
      status = sns_ddf_timer_start(st_ptr->timer, 100);
    }
  }
  MPU_ST_MSG_2(MEDIUM, "HW_st - hw_st_ptr=0x%x status=%d", state->hw_st_ptr, status);
  if(status != SNS_DDF_SUCCESS)
  {
    mpu6515_HW_test_cleanup(state);
  }
  return status;
}

/*
*******************************************************************************
*/
/**
 * @brief Called by SMGR to start sensor test
 *
 * @see sns_ddf_driver_if_s.run_test()
 */
sns_ddf_status_e mpu6515_run_test(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_test_e    test,
    uint32_t*         err)
{
    sns_ddf_status_e status;

    if ((sensor != SNS_DDF_SENSOR_ACCEL) && (sensor != SNS_DDF_SENSOR_GYRO))
    {
      status = SNS_DDF_EINVALID_PARAM;
    }
    else if(test == SNS_DDF_TEST_SELF_HW)
    {
      status = mpu6515_run_HW_self_test(dd_handle, sensor, err);
      if(status == SNS_DDF_EBUS)
      {
         *err = SNS_DD_ITEST_I2C_ERR;
      }
    }
    else
    {
      status = SNS_DDF_EINVALID_TEST;
    }
    MPU_ST_MSG_3(HIGH, "run_test - sensor=%d test=%d status=%d", sensor, test, status);
    return (status == SNS_DDF_SUCCESS) ? SNS_DDF_PENDING : status;
}

