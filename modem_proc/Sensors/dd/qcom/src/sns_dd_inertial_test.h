/** 
 * @file sns_dd_inertial_test.h
 *  
 * @brief Self test for inertial sensors. 
 *  
 * Drivers make use of this object to run a variance test using configurable 
 * parameters. The test runs in the context of the caller's task, and makes use 
 * of a recurring timer to poll the driver for data at the requested frequency. 
 * However, drivers need not manage this test, they simply dispatch it and 
 * return. When the test completes the driver's callback function is called 
 * with the result. 
 *  
 * NB: This test only works with drivers that return data synchronously.
 * 
 * Copyright (c) 2011-2012, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_inertial_test.h#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2015-03-26 ps   Updated license header
  2012-05-25 ag   Report I2C error in inertial sensor test
  2012-03-28 ag   Update data types to match cal algo definitions
  2011-09-30 yk   Initial revision
==============================================================================*/

#ifndef SNS_DD_INERTIAL_TEST__H
#define SNS_DD_INERTIAL_TEST__H

#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"


/**
 * Error codes indicating reasons for test failures.
 */
typedef enum
{
    SNS_DD_ITEST_SUCCESS         = 0,
    SNS_DD_ITEST_GET_DATA_FAILED = 1,
    SNS_DD_ITEST_ZERO_VARIANCE   = 2,
    SNS_DD_ITEST_HIGH_VARIANCE   = 3,
    SNS_DD_ITEST_HIGH_BIAS       = 4,
    SNS_DD_ITEST_I2C_ERR         = 5
} sns_dd_inertial_test_err_e;


/**
 * Test configuration values.
 */
typedef struct
{
    int32_t  sample_rate;
    int32_t  num_samples;   //< Must be a power of 2
    int64_t  max_variance;
    q16_t    bias_thresholds[3];

} sns_dd_inertial_test_config_s;


/**
 * Callback function that notifies the driver when the test has completed. 
 *  
 * @param handle  Handle to the driver running this test. 
 * @param sensor  Sensor used for the test. 
 * @param status  Test success status. 
 * @param err     Error code to indicate the test state if it failed. 
 * @param biases  Average biases for each of the axes.
 */
typedef void (*sns_dd_inertial_test_notify_t)(
    sns_ddf_handle_t  handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_status_e  status,
    uint32_t          err,
    q16_t             biases[]);


/**
 * Run a self test on an inertial sensor driver.
 * 
 * @param[in] sensor     Sensor to poll for data. Must be inertial.
 * @param[in] dd_handle  Driver instance used to run this test 
 * @param[in] dd_if      Driver interface for the driver running the test
 * @param[in] config     Configuration parameters for this test 
 * @param[in] notify_fn  Callback function called when the test completes 
 * 
 * @return Success if the test was dispatched or an error code otherwise. The 
 *         actual test result will be give via the callback function, if the
 *         test was dispatched. 
 */
sns_ddf_status_e sns_dd_inertial_test_run(
    sns_ddf_sensor_e                sensor,
    sns_ddf_handle_t                dd_handle,
    sns_ddf_driver_if_s*            dd_if,
    sns_dd_inertial_test_config_s*  config,
    sns_dd_inertial_test_notify_t   notify_fn);

#endif

