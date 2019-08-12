/**
 * @file sns_dd_mpu6xxx_selftest.c
 * @brief Test implementation for MPU-6xxx driver
 *
 * Copyright (c) 2012-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mpu6050_selftest.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-08-12 AH   Fix shift count is negative warnings in few places
  2013-07-17 vh   Eliminated compiler warnings
  2013-05-22 lka  Renamed MPU function pointer.
                  Integrated new DD for MPU6500.
  2013-05-16 vh   Increased the ACCEL bias threshold values
  2013-05-09 vh   Changed self test sampling rate
  2013-05-01 pn   Added criteria C evaluation for MPU6500 Accel
  2013-04-03 pn   Added support for MPU6500
  2013-03-18 vh   Added error check for functions
  2012-12-26 pn   Updated message macros
  2012-06-11 pn   For HW selftest sensor under test now streams at 100Hz
  2012-06-05 ag   Use inertial test error code for bus error
  2012-05-25 ag   Check for bus error
  2012-05-25 pn   Initial revision
==============================================================================*/

#include "sns_dd_mpu6050_priv.h"

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


#define MPU6050_USECS_BETWEEN_COLLECTS 10000  // 100Hz sampling rate
#define MPU6500_USECS_BETWEEN_COLLECTS  2000  // 500Hz sampling rate

#define MPU6050_NUM_TEST_SAMPLES_ACCEL     20
#define MPU6050_NUM_TEST_SAMPLES_GYRO      60
#define MPU6500_TEST_SAMPLES_TO_COLLECT   200

typedef enum
{
  MPU6XXX_HW_ST_STATE_ST_DISABLED,
  MPU6XXX_HW_ST_STATE_ST_ENABLED,
  MPU6XXX_HW_ST_NUM_STATES
} sns_dd_mpu6xxx_hw_st_state_e;

typedef enum
{
  SNS_DD_TEST_SUCCESS         = 0,
  SNS_DD_TEST_I2C_ERR         = 1,
  SNS_DD_TEST_FAIL            = 2
} sns_dd_test_err_e;

typedef struct
{
  int64_t  max_variance;
  q16_t    bias_thresholds;
} sns_dd_mpu6050_inertial_test_specs_s;

typedef struct
{
  int32_t  lower_limit;
  int32_t  upper_limit;
} sns_dd_mpu6xxx_hw_test_specs_s;

typedef struct
{
  q16_t sample_sum;
  q16_t sample_avg[MPU6XXX_HW_ST_NUM_STATES];
} sns_dd_st_data_s;

struct sns_dd_mpu6xxx_hw_st_s
{
  mpu6xxx_sensor_s*            sensor;
  mpu6xxx_sub_dev_s*           sub_dev;
  sns_ddf_sensor_e             sensor_type;
  sns_ddf_handle_t             smgr_handle;
  sns_ddf_timer_s              timer;
  sns_dd_mpu6xxx_hw_st_state_e st_state;
  sns_ddf_memhandler_s         memhandler;
  uint16_t                     num_samples;
  uint16_t                     sample_count;
  int16_t                      read_attempts;
  sns_dd_st_data_s             samples[MPU6XXX_NUM_INERTIAL_AXES];
  uint8_t                      st_reg_vals[MPU6XXX_NUM_INERTIAL_AXES+1];
  uint8_t                      st_reg_addr[MPU6XXX_NUM_INERTIAL_AXES+1];
  uint8_t                      range_idx;
  uint32_t                     usec_between_collects;
  q16_t                        str[MPU6XXX_NUM_INERTIAL_AXES];
  q16_t*                       bias_ptr;
  const q16_t*                 sensivity_ptr;
  const q16_t*                 data_cache_ptr;
  sns_ddf_axes_map_s*          axes_map_ptr; // should really be const
  const sns_dd_mpu6xxx_hw_test_specs_s* test_specs_ptr;
};

/*
 *   code1(axis) = 5-bit value ranging from 0 to 31
 *   code2(axis) = (code1(axis) - 1) / (2^5 - 2)
 *   FT[axis] = 0 if code1(axis) is 0
 *   FT[axis] = 4096 * 0.34 * ((0.92/0.34)^code2(axis)) if code1(axis) is not 0
 */
const float mpu6050_accel_ft[] =
{
  0.0,
  1392.64000000,
  1439.62427794,
  1488.19369086,
  1538.40171734,
  1590.30364022,
  1643.95660742,
  1699.41969491,
  1756.75397174,
  1816.02256727,
  1877.29074070,
  2006.09794076,
  2073.77879384,
  2143.74303387,
  2216.06769677,
  2290.83241744,
  2368.11951749,
  2448.01409585,
  2530.60412248,
  2615.98053523,
  2704.23733997,
  2795.47171411,
  2889.78411358,
  2987.27838344,
  3088.06187225,
  3192.24555023,
  3299.94413148,
  3411.27620026,
  3526.36434157,
  3645.33527616,
  3768.32000000
};

/*
 *   code1(axis) = 5-bit value ranging from 0 to 31
 *   code2(axis) = code1(axis) - 1
 *   FT[axis] = 0 if code1(axis) is 0
 *   FT[axis] =  25 * 131 * (1.046^code2(axis)) if axis is X or Z and code1(axis) is not 0
 *   FT[axis] = -25 * 131 * (1.046^code2(axis)) if axis is Y
 */
const float mpu6050_gyro_ft[] =
{
  0.0,
  3275.00000000,
  3425.65000000,
  3583.22990000,
  3748.05847540,
  3920.46916527,
  4100.81074687,
  4289.44804123,
  4486.76265112,
  4693.15373307,
  4909.03880480,
  5371.05790095,
  5618.12656439,
  5876.56038635,
  6146.88216413,
  6429.63874368,
  6725.40212589,
  7034.77062368,
  7358.37007237,
  7696.85509569,
  8050.91043010,
  8421.25230988,
  8808.62991613,
  9213.82689228,
  9637.66292932,
  10080.99542407,
  10544.72121358,
  11029.77838940,
  11537.14819532,
  12067.85701230,
  12622.97843487
};

/*
 * ST_OTP Table
 * ST_OTP = round((2620/2^FS) * 1.01^(ST_code-1)) (lsb)
 * ST_code obtained from OTP register and used to index this array
 */
static const uint16_t mpu6500_st_otp[] = {
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

/* MPU6050 HW self-test limits */
const sns_dd_mpu6xxx_hw_test_specs_s mpu6050_accel_str_limits =
{
  FX_FLTTOFIX_Q16(0.300*G),   // min, in G
  FX_FLTTOFIX_Q16(0.950*G)    // max, in G
};
const sns_dd_mpu6xxx_hw_test_specs_s mpu6050_gyro_str_limits =
{
  MPU6050_DPS_TO_RPSQ16( 10), // min, in radians/sec
  MPU6050_DPS_TO_RPSQ16(105)  // max, in radians/sec
};
const sns_dd_mpu6xxx_hw_test_specs_s mpu6050_ft_pct_change_limits =
{
  FX_FLTTOFIX_Q16(-14), // min, in %
  FX_FLTTOFIX_Q16( 14)  // max, in %
};

/* MPU6500 HW self-test limits */
const sns_dd_mpu6xxx_hw_test_specs_s mpu6500_accel_str_limits =
{
  FX_FLTTOFIX_Q16(0.225*G),   // min, in G
  FX_FLTTOFIX_Q16(0.675*G)    // max, in G
};
const sns_dd_mpu6xxx_hw_test_specs_s mpu6500_gyro_str_limits =
{
  MPU6050_DPS_TO_RPSQ16(60),  // min, in radians/sec
  MPU6050_DPS_TO_RPSQ16(250)  // max, in radians/sec,
                              //      arbitrary as no max is specified in specs
};

const sns_dd_mpu6xxx_hw_test_specs_s mpu6500_accel_ratio_limits =
{
  FX_FLTTOFIX_Q16(0.0), // min, in %
  FX_FLTTOFIX_Q16(0.5)  // max, in %
};

const sns_dd_mpu6xxx_hw_test_specs_s mpu6500_gyro_ratio_limits =
{
  FX_FLTTOFIX_Q16(0.5), // min, in %
  FX_FLTTOFIX_Q16(5.0)  // max, arbitrary as no max is specified in specs
};

const q16_t mpu6500_accel_offset_limit = FX_FLTTOFIX_Q16(0.5*G);

const q16_t mpu6500_gyro_offset_limit = MPU6050_DPS_TO_RPSQ16(20);

/* MPU6050 SW self-test specs */
const sns_dd_mpu6050_inertial_test_specs_s accel_test_specs =
{
  // define FX_CONV(a,q1,q2)	   (((q2)>(q1))?(a)<<((q2)-(q1)):(a)>>((q1)-(q2)))
  // Avoiding "shift count is negative" warning by not using FX_CONV macro
  //  (int64_t)(FX_CONV(FX_FLTTOFIX_Q16(0.000944),16,32)), // max variance
  (int64_t)(FX_FLTTOFIX_Q16(0.000944) << 16), // max variance
  FX_FLTTOFIX_Q16(0.18*G),                            // bias threshold
};
const sns_dd_mpu6050_inertial_test_specs_s gyro_test_specs =
{
  // define FX_CONV(a,q1,q2)		 (((q2)>(q1))?(a)<<((q2)-(q1)):(a)>>((q1)-(q2)))
  // Avoiding "shift count is negative" warning by not using FX_CONV macro
  // (int64_t)(FX_CONV(FX_FLTTOFIX_Q16(0.00436),16,32)),  // max variance
  (int64_t)(FX_FLTTOFIX_Q16(0.00436) << 16),  // max variance
  FX_FLTTOFIX_Q16(0.52359),                            // bias threshold
};

extern sns_ddf_driver_if_s sns_dd_mpu6xxx_if;

/*
*******************************************************************************
*/
/**
 * @brief Releases memory allocated for HW self-test.
 *
 */
static void sns_dd_mpu6050_HW_test_cleanup(sns_dd_mpu6xxx_s* state)
{
  if(state->hw_st_ptr != NULL)
  {
    // clean up
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
static void sns_dd_mpu6xxx_compute_str(
  sns_dd_mpu6xxx_hw_st_s* st_ptr,
  boolean                 convert)
{
  uint8_t i;
  for(i=0; i<ARR_SIZE(st_ptr->samples); i++)
  {
    sns_dd_st_data_s* st_data_ptr = &st_ptr->samples[i];
    st_ptr->str[i] = (st_data_ptr->sample_avg[MPU6XXX_HW_ST_STATE_ST_ENABLED] -
                      st_data_ptr->sample_avg[MPU6XXX_HW_ST_STATE_ST_DISABLED]);
    if (convert)
    {
      if ( st_ptr->sensor->sensor_type == SNS_DDF_SENSOR_ACCEL )
      {
        /* converts back to mGee */
        st_ptr->str[i] = FX_DIV_Q16(st_ptr->str[i], *st_ptr->sensivity_ptr);
      }
      else
      {
        /* converts back to dps */
        st_ptr->str[i] = FX_DIV_Q16(st_ptr->str[i], (*st_ptr->sensivity_ptr >> 9));
      }
    }
  }
  MPU6050_DATA_PRINTF(LOW, "STR    0x%08x 0x%08x 0x%08x",
                      st_ptr->str[X], st_ptr->str[Y], st_ptr->str[Z]);
}

/**
 * @brief Evaluates HW self-test data using Factory Trim info.
 *
 * @details
 *
 * The self-test response (STR) is defined as follows:
 *   STR = Sensor output with self-test enabled -
 *         Sensor output without self-test enabled
 *
 * Using Self-Test Relative Method, this STR is used to determine whether the
 * part has passed or failed self-test by finding
 * the change from factory trim (FT) of the self-test response as follows:
 *   FT delta = (STR - FT) / FT
 *
 * FT values are computed as follows:
 * For Accel:
 *   code1(axis) = ((MPU6050_REG_SELF_TEST_axis & 0xE0) >> 3) |
 *                 ((MPU6050_REG_SELF_TEST_ACCEL >> SELF_TEST_ACCEL_axis) && 0x03)
 *   code2(axis) = (code1(axis) - 1) / (2^5 - 2)
 *   FT[axis] = 0 if code1(axis) is 0
 *   FT[axis] = 4096 * 0.34 * ((0.92/0.34)^code2(axis)) if code1(axis) is not 0
 *
 * For Gyro:
 *   code1(axis) = (MPU6050_REG_SELF_TEST_axis & 0x1F)
 *   code2(axis) = code1(axis) - 1
 *   FT[axis] = 0 if code1(axis) is 0
 *   FT[axis] =  25 * 131 * (1.046^code2(axis)) if axis is X or Z
 *   FT[axis] = -25 * 131 * (1.046^code2(axis)) if axis is Y
 *
 * When the value of the FT delta is within the min/max limits of the
 * product specification (PS), the part has passed self test.
 *
 * Per Invensense, this limit is -14% to 14%.
 */
static sns_ddf_status_e sns_dd_mpu6050_hw_st_eval_fine(
  sns_dd_mpu6xxx_hw_st_s* st_ptr
)
{
  uint8_t code[MPU6XXX_NUM_INERTIAL_AXES];
  q16_t   q16ft[MPU6XXX_NUM_INERTIAL_AXES];
  q16_t   q16ft_pct_change[MPU6XXX_NUM_INERTIAL_AXES];
  uint8_t axes_validated = 0;
  uint8_t i;

  if(st_ptr->sensor_type == SNS_DDF_SENSOR_ACCEL)
  {
    code[X] =
      CODE_ACCEL(st_ptr->st_reg_vals[X]) | SELF_TEST_ACCEL_X(st_ptr->st_reg_vals[3]);
    code[Y] =
      CODE_ACCEL(st_ptr->st_reg_vals[Y]) | SELF_TEST_ACCEL_Y(st_ptr->st_reg_vals[3]);
    code[Z] =
      CODE_ACCEL(st_ptr->st_reg_vals[Z]) | SELF_TEST_ACCEL_Z(st_ptr->st_reg_vals[3]);

    for(i=0; i<ARR_SIZE(st_ptr->samples); i++)
    {
      q16ft[i] = FX_FLTTOFIX_Q16(mpu6050_accel_ft[code[i]]);
    }
  }
  else /* Gyro */
  {
    for(i=0; i<ARR_SIZE(code); i++)
    {
      code[i]  = CODE_GYRO(st_ptr->st_reg_vals[i]);
      q16ft[i] = FX_FLTTOFIX_Q16(mpu6050_gyro_ft[code[i]]);
    }
  }
  MPU6050_DATA_PRINTF(MEDIUM, "Code 0x%08x 0x%08x 0x%08x",
                      code[X], code[Y], code[Z]);
  MPU6050_DATA_PRINTF(MEDIUM, "FT   0x%08x 0x%08x 0x%08x",
                      q16ft[X], q16ft[Y], q16ft[Z]);

  for(i=0; i<ARR_SIZE(q16ft_pct_change); i++)
  {
    if(q16ft[i] == 0)
    {
      q16ft_pct_change[i] = 0;
    }
    else
    {
      q16ft_pct_change[i] = st_ptr->str[i] - q16ft[i];
      q16ft_pct_change[i] = FX_DIV_Q16(q16ft_pct_change[i], q16ft[i]);
    }
    if( (q16ft_pct_change[i] >= mpu6050_ft_pct_change_limits.lower_limit) &&
        (q16ft_pct_change[i] <= mpu6050_ft_pct_change_limits.upper_limit) )
    {
      axes_validated++;
    }
    else
    {
      MPU6050_MSG_2(ERROR, "eval fine i=%d delta=%d",
                    i, q16ft_pct_change[i]);
    }
  }
  if ( axes_validated == MPU6XXX_NUM_INERTIAL_AXES )
  {
    MPU6050_MSG_0(HIGH, "MPU6050 HW Selftest passed criteria A");
  }
  return (axes_validated == MPU6XXX_NUM_INERTIAL_AXES) ?
    SNS_DDF_SUCCESS : SNS_DDF_EFAIL;
}

static sns_ddf_status_e sns_dd_mpu6500_hw_st_criteria_c(
  sns_dd_mpu6xxx_hw_st_s* st_ptr
)
{
  uint8_t axes_validated = 0;
  uint8_t i;

  q16_t offset_limit = (st_ptr->sensor->sensor_type == SNS_DDF_SENSOR_ACCEL) ?
    mpu6500_accel_offset_limit : mpu6500_gyro_offset_limit;

  for ( i=0; i<ARR_SIZE(st_ptr->samples); i++ )
  {
    q16_t offset = st_ptr->samples[i].sample_avg[MPU6XXX_HW_ST_STATE_ST_DISABLED];

    if ( (st_ptr->sensor->sensor_type == SNS_DDF_SENSOR_ACCEL) && (i == st_ptr->axes_map_ptr->indx_z) )
    {
      MPU6050_MSG_2(LOW, "criteria_c - offset=0x%08x sign=%d",
                    offset, st_ptr->axes_map_ptr->sign_z);
      offset *= st_ptr->axes_map_ptr->sign_z;
      offset += FX_FLTTOFIX_Q16(G);
    }

    MPU6050_MSG_3(LOW, "criteria_c - i=%d |offset|=0x%08x limit=0x%08x",
                  i, FX_ABS(offset), offset_limit);
    if ( FX_ABS(offset) <= offset_limit )
    {
      axes_validated++;
    }
    else
    {
      MPU6050_MSG_1(ERROR, "Criteria C fails i=%d", i);
    }
  }
  if ( axes_validated == MPU6XXX_NUM_INERTIAL_AXES )
  {
    MPU6050_MSG_0(HIGH, "MPU6500 HW Selftest passed criteria C!");
  }
  return (axes_validated == MPU6XXX_NUM_INERTIAL_AXES) ?
    SNS_DDF_SUCCESS : SNS_DDF_EFAIL;
}

/**
 * @brief Evaluates HW self-test data using Factory Trim info for MPU6500
 *
 */
static sns_ddf_status_e sns_dd_mpu6500_hw_st_eval_fine(
  sns_dd_mpu6xxx_hw_st_s* st_ptr
)
{
  const sns_dd_mpu6xxx_hw_test_specs_s* ratio_limits_ptr;
  q16_t q16st_ratio[MPU6XXX_NUM_INERTIAL_AXES];
  q16_t q16st_otp[MPU6XXX_NUM_INERTIAL_AXES];
  const uint16_t* st_otp_ptr;
  uint8_t axes_validated = 0;
  uint8_t i;

  ratio_limits_ptr = (st_ptr->sensor->sensor_type == SNS_DDF_SENSOR_ACCEL) ?
    &mpu6500_accel_ratio_limits : &mpu6500_gyro_ratio_limits;
  st_otp_ptr = mpu6500_st_otp;
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
      MPU6050_MSG_3(ERROR, "Sensor %d failed criteria A - i=%d ratio=0x%x",
                    st_ptr->sensor, i, q16st_ratio[i]);
    }
  }
  MPU6050_DATA_PRINTF(LOW, "ST_OTP 0x%08x 0x%08x 0x%08x",
                      q16st_otp[X], q16st_otp[Y], q16st_otp[Z]);
  MPU6050_DATA_PRINTF(LOW, "Ratio  0x%08x 0x%08x 0x%08x",
                      q16st_ratio[X], q16st_ratio[Y], q16st_ratio[Z]);

  if ( axes_validated == MPU6XXX_NUM_INERTIAL_AXES )
  {
    MPU6050_MSG_0(HIGH, "MPU6500 HW Selftest passed criteria A!");
  }
  return (axes_validated == MPU6XXX_NUM_INERTIAL_AXES) ?
    SNS_DDF_SUCCESS : SNS_DDF_EFAIL;
}

/**
 * @brief Evaluates HW self-test data without Factory Trim info.
*/
static sns_ddf_status_e sns_dd_mpu6xxx_hw_st_eval_coarse(
  sns_dd_mpu6xxx_hw_st_s* st_ptr
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
      MPU6050_MSG_3(ERROR, "eval_coarse i=%d low=0x%x hi=0x%x",
                    i, st_ptr->test_specs_ptr->lower_limit,
                    st_ptr->test_specs_ptr->upper_limit);
    }
  }
  if ( axes_validated == MPU6XXX_NUM_INERTIAL_AXES )
  {
    MPU6050_MSG_0(HIGH, "MPU6500 HW Selftest passed criteria B!");
  }
  return (axes_validated == MPU6XXX_NUM_INERTIAL_AXES) ?
    SNS_DDF_SUCCESS : SNS_DDF_EFAIL;
}


/**
 * @brief Reads the self-test registers.
 *
 * @details Considered failed if all registers hold zeroes.
 */
static sns_ddf_status_e sns_dd_mpu6050_read_st_reg(
  sns_ddf_handle_t port,
  sns_dd_mpu6xxx_hw_st_s* st_ptr
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
  MPU6050_DATA_PRINTF(LOW, "ST REGs 0x%02x 0x%02x 0x%02x", st_ptr->st_reg_vals[X],
                      st_ptr->st_reg_vals[Y], st_ptr->st_reg_vals[Z]);
  if ( st_ptr->st_reg_addr[3] != 0xFF)
  {
    MPU6050_DATA_PRINTF(LOW, "ST REGs 0x%02x 0x%02x 0x%02x",
                        st_ptr->st_reg_vals[3], 0, 0);
  }
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
static void sns_dd_mpu6050_hw_st_extract_bias(sns_dd_mpu6xxx_hw_st_s* st_ptr)
{
  uint8_t i;
  for ( i=0; i<ARR_SIZE(st_ptr->samples); i++ )
  {
    st_ptr->bias_ptr[i] =
      st_ptr->samples[i].sample_avg[MPU6XXX_HW_ST_STATE_ST_DISABLED];
  }
  sns_ddf_map_axes(st_ptr->axes_map_ptr, st_ptr->bias_ptr);

  if (st_ptr->sensor->sensor_type == SNS_DDF_SENSOR_ACCEL)
  {
    /* Z axis for accel needs to be adjusted by G. */
    st_ptr->bias_ptr[Z] += FX_FLTTOFIX_Q16(G);
  }
  for ( i=0; i<ARR_SIZE(st_ptr->samples); i++ )
  {
    /* change the sign of the bias since SMGR expects it this way. */
    st_ptr->bias_ptr[i] *= (-1);
  }
  sns_ddf_smgr_notify_event(st_ptr->smgr_handle, st_ptr->sensor_type,
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
static void sns_dd_mpu6050_hw_st_completes(sns_dd_mpu6xxx_s* state)
{
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  sns_dd_mpu6xxx_hw_st_s* st_ptr = state->hw_st_ptr;

  if ( st_ptr->sample_count >= st_ptr->num_samples )
  {
    status = sns_dd_mpu6050_read_st_reg(state->port_handle, st_ptr);
    sns_dd_mpu6xxx_compute_str(st_ptr, (status == SNS_DDF_SUCCESS) ? TRUE : FALSE);
    if(status == SNS_DDF_SUCCESS)
    {
      if ( state->device_id == DEVICE_ID_MPU6050 )
      {
        status = sns_dd_mpu6050_hw_st_eval_fine(st_ptr);
      }
      else
      {
        status = sns_dd_mpu6500_hw_st_eval_fine(st_ptr);
      }
    }
    else
    {
      status = sns_dd_mpu6xxx_hw_st_eval_coarse(st_ptr);
    }
    if ((status == SNS_DDF_SUCCESS) && (state->device_id == DEVICE_ID_MPU6500))
    {
      status = sns_dd_mpu6500_hw_st_criteria_c(st_ptr);
    }
  }
  /* else, test was aborted */

  sns_ddf_smgr_notify_test_complete(st_ptr->smgr_handle, st_ptr->sensor_type, status,
                                    ((status == SNS_DDF_SUCCESS) ?
                                      SNS_DD_TEST_SUCCESS: SNS_DD_TEST_FAIL) );
  if ( status == SNS_DDF_SUCCESS )
  {
    sns_dd_mpu6050_hw_st_extract_bias(st_ptr);
  }
  sns_dd_mpu6050_HW_test_cleanup(state);
}


/**
 * @brief Called when the timer set by this driver has expired.
 *
 */
void sns_dd_mpu6050_handle_st_timer(sns_dd_mpu6xxx_s* state)
{
  sns_dd_mpu6xxx_hw_st_s* st_ptr = state->hw_st_ptr;
  sns_ddf_status_e status;
  uint8_t i;

  if(st_ptr->read_attempts >= 0)
  {
    sns_ddf_timer_start(st_ptr->timer, st_ptr->usec_between_collects);
    status = sns_dd_mpu6xxx_get_data_from_device(state, FALSE);
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
      MPU6050_MSG_3(ERROR, "handle_st_timer - attempts=%d samples=%d status=%d",
                    st_ptr->read_attempts, st_ptr->sample_count, status);
      st_ptr->read_attempts = -1;
      st_ptr->st_state = MPU6XXX_HW_ST_NUM_STATES;
    }
  }
  if(st_ptr->sample_count >= st_ptr->num_samples)
  {
    MPU6050_MSG_1(LOW, "handle_st_timer - samples=%d", st_ptr->sample_count);
    MPU6050_DATA_PRINTF(MEDIUM, "Sum     0x%08x 0x%08x 0x%08x",
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
    MPU6050_DATA_PRINTF(MEDIUM, "Average 0x%08x 0x%08x 0x%08x",
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
    MPU6050_MSG_1(MEDIUM, "handle_st_timer state=%d", st_ptr->st_state);
    switch(st_ptr->st_state)
    {
    case MPU6XXX_HW_ST_STATE_ST_DISABLED:
      st_ptr->sample_count  = 0;
      sns_dd_mpu6xxx_set_config_register(state, st_ptr->sensor, st_ptr->range_idx, //sns_dd_mpu6xxx_set_config_register(state, st_ptr->sensor, st_ptr->range_idx,
                                         ACCEL_HPF_None, FALSE);
      sns_ddf_timer_start(st_ptr->timer, MPU6050_IDLE_TO_READY_TIME);
      break;
    case MPU6XXX_HW_ST_STATE_ST_ENABLED:
      st_ptr->sample_count  = 0;
      sns_dd_mpu6xxx_set_config_register(state, st_ptr->sensor, st_ptr->range_idx, //sns_dd_mpu6xxx_set_config_register(state, st_ptr->sensor, st_ptr->range_idx,
                                         ACCEL_HPF_None, TRUE);
      sns_ddf_timer_start(st_ptr->timer, MPU6050_IDLE_TO_READY_TIME);
      break;
    default:
      sns_dd_mpu6050_hw_st_completes(state);
    }
  }
}


/**
 * @brief Sets up and runs HW self-test
 *
 */
static sns_ddf_status_e sns_dd_mpu6050_run_HW_self_test(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor_type,
    uint32_t*         err
)
{
  sns_dd_mpu6xxx_s* state = sns_dd_mpu6xxx_get_device(dd_handle);
  mpu6xxx_sub_dev_s* sub_device = sns_dd_mpu6xxx_get_sub_dev(dd_handle);
  uint8_t sub_dev_idx = sns_dd_mpu6xxx_get_sub_dev_idx(dd_handle);
  mpu6xxx_sensor_s* sensor = sns_dd_mpu6xxx_get_sensor(dd_handle, sensor_type);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  if (state == NULL || sub_device == NULL || sensor == NULL)
  {
    MPU6050_MSG_3(ERROR, "mpu6050_run_HW_self_test: One of the following was found to be NULL... state 0x%X  sub_device 0x%X  sensor 0x%X", state, sub_device, sensor);
    return SNS_DDF_EINVALID_PARAM;
  }

  status = sns_ddf_malloc((void**)&(state->hw_st_ptr),
                          sizeof(sns_dd_mpu6xxx_hw_st_s));
  if(status == SNS_DDF_SUCCESS)
  {
    SNS_OS_MEMZERO(state->hw_st_ptr, sizeof(sns_dd_mpu6xxx_hw_st_s));
    status = sns_ddf_timer_init(&state->hw_st_ptr->timer,
                                (sns_ddf_handle_t)state,
                                &sns_dd_mpu6xxx_if,
                                NULL,
                                FALSE);
  }
  if(status == SNS_DDF_SUCCESS)
  {
    sns_dd_mpu6xxx_hw_st_s* st_ptr = state->hw_st_ptr;

    if ( state->device_id == DEVICE_ID_MPU6050 )
    {
      if ( sensor_type == SNS_DDF_SENSOR_ACCEL )
      {
        st_ptr->range_idx       = 2; // +/-8G
        st_ptr->test_specs_ptr  = &mpu6050_accel_str_limits;
        st_ptr->num_samples     = MPU6050_NUM_TEST_SAMPLES_ACCEL;
      }
      else
      {
        st_ptr->range_idx       = 0; // +/-250dps
        st_ptr->test_specs_ptr  = &mpu6050_gyro_str_limits;
        st_ptr->num_samples     = MPU6050_NUM_TEST_SAMPLES_GYRO;
      }
      st_ptr->st_reg_addr[X]    = MPU6050_REG_SELF_TEST_X;
      st_ptr->st_reg_addr[Y]    = MPU6050_REG_SELF_TEST_Y;
      st_ptr->st_reg_addr[Z]    = MPU6050_REG_SELF_TEST_Z;
      st_ptr->st_reg_addr[3]    = MPU6050_REG_SELF_TEST_ACCEL;
    }
    else /* has got to be MPU6500 */
    {
      st_ptr->range_idx         = 0; // +/-2G for Accel and +/-250dps for Gyro
      st_ptr->num_samples       = MPU6500_TEST_SAMPLES_TO_COLLECT;
      if ( sensor_type == SNS_DDF_SENSOR_ACCEL )
      {
        st_ptr->test_specs_ptr  = &mpu6500_accel_str_limits;
        st_ptr->st_reg_addr[X]  = MPU6500_REG_SELF_TEST_AX;
        st_ptr->st_reg_addr[Y]  = MPU6500_REG_SELF_TEST_AY;
        st_ptr->st_reg_addr[Z]  = MPU6500_REG_SELF_TEST_AZ;
      }
      else
      {
        st_ptr->test_specs_ptr  = &mpu6500_gyro_str_limits;
        st_ptr->st_reg_addr[X]  = MPU6500_REG_SELF_TEST_GX;
        st_ptr->st_reg_addr[Y]  = MPU6500_REG_SELF_TEST_GY;
        st_ptr->st_reg_addr[Z]  = MPU6500_REG_SELF_TEST_GZ;
      }
      st_ptr->st_reg_addr[3]    = 0xFF;
    }
    if ( sensor_type == SNS_DDF_SENSOR_ACCEL )
    {
      st_ptr->smgr_handle       = sub_device->smgr_handle;
      st_ptr->data_cache_ptr    = &state->data_cache[MPU6050_CACHE_ACCELX];
      st_ptr->bias_ptr          = sensor->biases;
      st_ptr->sensivity_ptr     = &sns_dd_mpu6050_accel_sensitivity[st_ptr->range_idx];
      st_ptr->axes_map_ptr      = &state->axes_map;
    }
    else
    {
      st_ptr->smgr_handle       = sub_device->smgr_handle;
      st_ptr->data_cache_ptr    = &state->data_cache[MPU6050_CACHE_GYROX];
      st_ptr->bias_ptr          = sensor->biases;
      st_ptr->sensivity_ptr     = &sns_dd_mpu6050_gyro_sensitivity[st_ptr->range_idx];
      st_ptr->axes_map_ptr      = &state->axes_map;
    }
    st_ptr->usec_between_collects = (state->device_id == DEVICE_ID_MPU6050) ?
      MPU6050_USECS_BETWEEN_COLLECTS : MPU6500_USECS_BETWEEN_COLLECTS;
    st_ptr->sensor_type          = sensor_type;
    st_ptr->sensor          = sensor;
    st_ptr->sub_dev              = sub_device;
    st_ptr->read_attempts   = -1;
    sns_ddf_memhandler_init(&st_ptr->memhandler);
    MPU6050_MSG_2(LOW, "HW_self_test - usec=%d samples=%d",
                  st_ptr->usec_between_collects, st_ptr->num_samples);

    status = sns_dd_mpu6xxx_reset(dd_handle);
    MPU6050_MSG_3(MEDIUM, "HW_self_test - sensor_type=%d dev=0x%x reset=%d",
                  sensor_type, state->device_id, status);
    if(status == SNS_DDF_SUCCESS)
    {
      status = sns_dd_mpu6xxx_set_powerstate(state, sub_dev_idx, SNS_DDF_POWERSTATE_ACTIVE);
    }
    if(status == SNS_DDF_SUCCESS)
    {
      status = sns_dd_mpu6xxx_set_lowpass(state,
                                          (state->device_id == DEVICE_ID_MPU6050) ?
                                          3 : //44 Hz
                                          2); //94 Hz
    }
    if(status == SNS_DDF_SUCCESS)
    {
      status = sns_ddf_timer_start(st_ptr->timer, 100);
    }
  }
  MPU6050_MSG_2(MEDIUM, "HW_self_test hw_st_ptr=0x%x status=%d",
                state->hw_st_ptr, status);
  if(status != SNS_DDF_SUCCESS)
  {
    sns_dd_mpu6050_HW_test_cleanup(state);
  }
  return status;
}


/*---------------------------------------------------------------------------*/
/**
 * @brief Called when the inertial test has accumulated enough
 *        samples. This function notifies SMGR of results. In
 *        addition, when the test passes, the calculated bias is
 *        reported to SMGR to be saved in the registry.
 *
 * @see sns_ddf_notify_test_complete()
 * @see sns_ddf_smgr_notify_event()
 */
static void sns_dd_mpu6050_test_notify(
  sns_ddf_handle_t  handle,
  sns_ddf_sensor_e  sensor_type,
  sns_ddf_status_e  status,
  uint32_t          err,
  q16_t*            biases)
{
  mpu6xxx_sub_dev_s* sub_device = sns_dd_mpu6xxx_get_sub_dev(handle);
  mpu6xxx_sensor_s* sensor = sns_dd_mpu6xxx_get_sensor(handle, sensor_type);
  sns_ddf_handle_t smgr_handle;
  q16_t* biases_ptr;
  uint8_t i;

  MPU6050_MSG_3(LOW, "mpu6050_test_notify: Notifying SMGR of MPU6XXX test (sensor_type %u  status %u  error %u)", sensor_type, status, err);

  /* Note - the parameter handle doesn't get used in this unique driver */

  if (sub_device == NULL || sensor == NULL)
    return;

  smgr_handle = sub_device->smgr_handle;

  sns_ddf_smgr_notify_test_complete(smgr_handle, sensor_type, status, err);

  if ( status == SNS_DDF_SUCCESS )
  {
    biases_ptr = sensor->biases;

    for(i = 0; i < MPU6XXX_NUM_INERTIAL_AXES; i++)
      biases_ptr[i] = biases[i];

    sns_ddf_smgr_notify_event(smgr_handle, sensor_type, SNS_DDF_EVENT_BIAS_READY);
  }
}


/**
 * @brief Sets up and runs Inertial test.
 *
 */
static sns_ddf_status_e sns_dd_mpu6050_run_inertial_test(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor_type,
    uint32_t*         err
)
{
  sns_dd_inertial_test_config_s test_config;
  sns_dd_mpu6xxx_s* state = sns_dd_mpu6xxx_get_device(dd_handle);
  uint8_t sub_dev_idx = sns_dd_mpu6xxx_get_sub_dev_idx(dd_handle);
  mpu6xxx_sensor_s* sensor = sns_dd_mpu6xxx_get_sensor(dd_handle, sensor_type);
  sns_ddf_status_e status;
  uint8_t range_idx, i;
  const sns_dd_mpu6050_inertial_test_specs_s* test_spec_ptr =
    (sensor_type == SNS_DDF_SENSOR_ACCEL) ? &accel_test_specs : &gyro_test_specs;

MPU6050_MSG_3(FATAL, "invalid param: %u, %u, %u", state, sensor, sensor_type);
  if (state == NULL || sensor == NULL)
    return SNS_DDF_EINVALID_PARAM;

  test_config.sample_rate  = (sensor_type == SNS_DDF_SENSOR_ACCEL) ? 200 : 100;
  test_config.num_samples  = 64;
  test_config.max_variance = test_spec_ptr->max_variance;
  for(i=0; i<ARR_SIZE(test_config.bias_thresholds); i++)
  {
    test_config.bias_thresholds[i] = test_spec_ptr->bias_thresholds;
  }

  status = sns_dd_mpu6xxx_reset(dd_handle);
  MPU6050_MSG_2(MEDIUM, "run_inertial_test sensor=%d status=%d", sensor_type, status);

  if(status == SNS_DDF_SUCCESS)
  {
    status = sns_dd_mpu6xxx_set_powerstate(state, sub_dev_idx, SNS_DDF_POWERSTATE_ACTIVE);
  }
  if(status == SNS_DDF_SUCCESS)
  {
    status = sns_dd_mpu6xxx_set_lowpass(state, 2);
  }
  if(status == SNS_DDF_SUCCESS)
  {
    range_idx = (sensor_type == SNS_DDF_SENSOR_ACCEL) ? 1 : 2;
    status = sns_dd_mpu6xxx_set_config_register(state, sensor, range_idx,
                                                ACCEL_HPF_0_63HZ, FALSE);
  }
  if(status == SNS_DDF_SUCCESS)
  {
    /* need to wait idle to ready time */
    sns_ddf_delay(MPU6050_IDLE_TO_READY_TIME);

    status = sns_dd_inertial_test_run(sensor_type,
                                      dd_handle,
                                      &sns_dd_mpu6xxx_if,
                                      &test_config,
                                      &sns_dd_mpu6050_test_notify);
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
sns_ddf_status_e sns_dd_mpu6xxx_run_test(
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
    else if(test == SNS_DDF_TEST_SELF)
    {
      status = sns_dd_mpu6050_run_inertial_test(dd_handle, sensor, err);
      if(status == SNS_DDF_EBUS)
      {
         *err = SNS_DD_ITEST_I2C_ERR;
      }
    }
    else if(test == SNS_DDF_TEST_SELF_HW)
    {
      status = sns_dd_mpu6050_run_HW_self_test(dd_handle, sensor, err);
      if(status == SNS_DDF_EBUS)
      {
         *err = SNS_DD_ITEST_I2C_ERR;
      }
    }
    else
    {
      status = SNS_DDF_EINVALID_TEST;
    }
    MPU6050_MSG_3(HIGH, "run_test sensor=%d test=%d status=%d", sensor, test, status);
    return (status == SNS_DDF_SUCCESS) ? SNS_DDF_PENDING : status;
}

