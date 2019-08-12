/** 
 * @file sns_dd_inertial_test.c
 *  
 * Copyright (c) 2011-2013 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_inertial_test.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2013-09-17 vy   Added support for uImage
  2013-06-07 ps   swap order of checking high bias and zero variance 
  2013-05-09 vh   Getting multiple data for ACCEL's GYRO_CAL_MOT_STATE_MOVING state
  2012-05-25 pn   Prints out sample data in hex.
  2012-03-28 ag   Re-order validation sequence
  2012-02-15 ag   Adjust accel Z axis by G
  2011-12-07 ag   Fix the sign of bias to be what SMGR expects
  2011-11-16 sc   Change timer type to one-shot to prevent multiple release
  2011-10-10 ag   Check absolute value of bias; debug messages
  2011-09-30 yk   Initial revision
==============================================================================*/

#include "sns_dd_inertial_test.h"

#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "gyro_cal.h"

#include "sns_debug_str.h"
#include "sns_debug_api.h"

#define SNS_ACCEL_SELF_TEST_LOOP 3

#define SMGR_DATA_PRINTF(level, p1, p2, p3)  \
   SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_ASYNC_DATA_VALUE, p1, p2, p3)

typedef struct
{
    sns_ddf_sensor_e      sensor;
    sns_ddf_handle_t      dd_handle;
    sns_ddf_driver_if_s*  dd_if;
    sns_ddf_status_e      status;
    sns_ddf_timer_s       timer;
    sns_ddf_memhandler_s  memhandler;
    gyro_cal_state_s*     algo_state;
    gyro_cal_input_s      algo_input;
    gyro_cal_output_s     algo_output;
    sns_dd_inertial_test_config_s  config;
    sns_dd_inertial_test_notify_t  dd_notify;

} sns_dd_inertial_test_s;


static bool is_inertial_sensor(sns_ddf_sensor_e sensor)
{
    return (sensor == SNS_DDF_SENSOR_ACCEL) ||
           (sensor == SNS_DDF_SENSOR_GYRO);
}


static void sns_dd_inertial_test_notify_result(
    sns_dd_inertial_test_s* state, uint32_t err)
{
    uint8_t i;
    int32_t bias[GYRO_CAL_COL];

    /* change the sign of the bias since SMGR expects it this way.
       SMGR_CALIBRATE is defined as ((in)+(bias))*/
    for(i = 0; i < GYRO_CAL_COL; i++)
        bias[i] = -(state->algo_output.bias[i]);

    state->dd_notify(
        state->dd_handle,
        state->sensor,
        state->status,
        err,
        bias);
}


static void sns_dd_inertial_test_cleanup(sns_dd_inertial_test_s* state)
{
    sns_ddf_timer_release(state->timer);
    sns_ddf_mfree(state->algo_state);
    sns_ddf_mfree(state);
}


static void sns_dd_inertial_test_complete(
    sns_dd_inertial_test_s* state, uint32_t err)
{
    sns_dd_inertial_test_notify_result(state, err);
    sns_dd_inertial_test_cleanup(state);
}


static void sns_dd_inertial_test_get_data(sns_dd_inertial_test_s* state)
{
    sns_ddf_sensor_data_s* data;
    uint8_t i;
    bool is_uimg_refac = sns_ddf_smgr_get_uimg_refac(state->dd_handle);
    
    state->status = state->dd_if->get_data(
        state->dd_handle, &state->sensor, 1, &state->memhandler, &data);

    if(state->status != SNS_DDF_SUCCESS)
        return;

    if(data->num_samples != GYRO_CAL_COL) {
        state->status = SNS_DDF_EFAIL;
        return;
    }

    for(i = 0; i < GYRO_CAL_COL; i++)
        state->algo_input.data[i] = data->samples[i].sample;

    if(state->sensor == SNS_DDF_SENSOR_ACCEL) {
        /* Z axis for accel needs to be adjusted by 9.8 or G. */
        state->algo_input.data[GYRO_CAL_COL-1] += FX_FLTTOFIX_Q16(G); 
    }

    if ( is_uimg_refac )
      sns_ddf_memhandler_free_ex(&state->memhandler, NULL);
    else sns_ddf_memhandler_free(&state->memhandler);

}

static void sns_dd_inertial_test_log_result(sns_dd_inertial_test_s* state)
{
    SMGR_DATA_PRINTF(LOW,
                     state->algo_state->variance[0],
                     state->algo_state->variance[1],
                     state->algo_state->variance[2]);

    SMGR_DATA_PRINTF(LOW,
                     state->algo_output.bias[0],
                     state->algo_output.bias[1],
                     state->algo_output.bias[2]);
}

static void sns_dd_inertial_test_handle_timer(sns_ddf_handle_t handle, void* arg)
{
    sns_dd_inertial_test_s* state = handle;
    uint8_t i;

    sns_dd_inertial_test_get_data(state);

    if(state->status != SNS_DDF_SUCCESS) {
        sns_dd_inertial_test_complete(state, SNS_DD_ITEST_GET_DATA_FAILED);
        return;
    }

    gyro_cal_scm_update_sns(state->algo_state, &state->algo_input, &state->algo_output);

    sns_dd_inertial_test_log_result(state);

    if(state->algo_output.motionState == GYRO_CAL_MOT_STATE_UNKNOWN) {
        // Algorithm hasn't accumulated enough samples to complete the test.
        // Let the timer run again so we can collect more samples.
        sns_ddf_timer_start(state->timer, 1000000u / state->config.sample_rate);
        return;
    } else if(state->algo_output.motionState == GYRO_CAL_MOT_STATE_MOVING) {
        static uint8 accel_test_loop = 0;
        if((state->sensor == SNS_DDF_SENSOR_ACCEL) &&
           (accel_test_loop < SNS_ACCEL_SELF_TEST_LOOP)) {
            accel_test_loop++;
            sns_ddf_timer_start(state->timer, 1000000u / state->config.sample_rate);
        } else {
            accel_test_loop = 0;
            state->status = SNS_DDF_EFAIL;
            sns_dd_inertial_test_complete(state, SNS_DD_ITEST_HIGH_VARIANCE);
        }
    } else {
        for(i = 0; i < GYRO_CAL_COL; i++) {
            if(FX_ABS(state->algo_output.bias[i]) > state->config.bias_thresholds[i]) {
                state->status = SNS_DDF_EFAIL;
                sns_dd_inertial_test_complete(state, SNS_DD_ITEST_HIGH_BIAS);
                return;
            }
        }

        for(i = 0; i < GYRO_CAL_COL; i++) {
            if(state->algo_state->variance[i] == 0) {
                state->status = SNS_DDF_EFAIL;
                sns_dd_inertial_test_complete(state, SNS_DD_ITEST_ZERO_VARIANCE);
                return;
            }
        }

        state->status = SNS_DDF_SUCCESS;
        sns_dd_inertial_test_complete(state, SNS_DD_ITEST_SUCCESS);
    }
}


static void sns_dd_inertial_test_init(
    sns_dd_inertial_test_s*        state,
    sns_ddf_sensor_e               sensor,
    sns_ddf_handle_t               dd_handle,
    sns_ddf_driver_if_s*           dd_if,
    sns_dd_inertial_test_notify_t  notify_fn)
{
    // In order to make use of a timer, we create a driver interface that only
    // has a timer handling function.
    static sns_ddf_driver_if_s test_if;
    test_if.handle_timer = sns_dd_inertial_test_handle_timer;

    if(!is_inertial_sensor(sensor)) {
        state->status = SNS_DDF_EINVALID_PARAM;
        return;
    }

    state->dd_handle = dd_handle;
    state->dd_if = dd_if;
    state->sensor = sensor;
    state->dd_notify = notify_fn;

    sns_ddf_memhandler_init(&state->memhandler);
    state->status = sns_ddf_timer_init(&state->timer, state, &test_if, NULL, false);
}


static void sns_dd_inertial_test_config(
    sns_dd_inertial_test_s* state,
    sns_dd_inertial_test_config_s* config)
{
    uint32_t algo_size;
    gyro_cal_config_s algo_config;

    if(state->status != SNS_DDF_SUCCESS)
        return;

    algo_config.numSamples = config->num_samples;
    algo_config.sampleRate = config->sample_rate;
    algo_config.varianceThold = config->max_variance;
    state->config = *config;

    algo_size = gyro_cal_scm_mem_req_sns(&algo_config);
    if(algo_size == 0) {
        state->status = SNS_DDF_EINVALID_PARAM;
        return;
    }

    state->status = sns_ddf_malloc((void**)&state->algo_state, algo_size);
    if(state->status != SNS_DDF_SUCCESS)
        return;

    gyro_cal_scm_state_reset_sns(&algo_config, state->algo_state);
}


static void sns_dd_inertial_test_dispatch(sns_dd_inertial_test_s* state)
{
    if(state->status != SNS_DDF_SUCCESS)
        return;

    /// @todo According to the DDF spec, drivers are allowed to block for 100ms
    /// at a time. Therefore, we can optimize by letting tests that run at >10Hz
    /// poll for data using busy-wait until this threshold has been met, and
    /// only then dispatch the timer.
    sns_ddf_timer_start(state->timer, 1000000u / state->config.sample_rate);
}


sns_ddf_status_e sns_dd_inertial_test_run(
    sns_ddf_sensor_e                sensor,
    sns_ddf_handle_t                dd_handle,
    sns_ddf_driver_if_s*            dd_if,
    sns_dd_inertial_test_config_s*  config,
    sns_dd_inertial_test_notify_t   notify_fn)
{
    sns_dd_inertial_test_s* test;
    sns_ddf_status_e malloc_status;

    malloc_status = sns_ddf_malloc((void**)&test, sizeof(sns_dd_inertial_test_s));
    if(malloc_status != SNS_DDF_SUCCESS)
        return malloc_status;

    sns_dd_inertial_test_init(test, sensor, dd_handle, dd_if, notify_fn);
    sns_dd_inertial_test_config(test, config);
    sns_dd_inertial_test_dispatch(test);

    return test->status;
}


