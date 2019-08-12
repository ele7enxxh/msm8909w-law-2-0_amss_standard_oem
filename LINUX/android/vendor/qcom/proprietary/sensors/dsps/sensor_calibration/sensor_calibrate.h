/*============================================================================
  @file sensor_calibrate.h

  Gyroscope calibration algorithm source file

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/
#ifndef SENSOR_CALIBRATE_H
#define SENSOR_CALIBRATE_H
#include <utils/Singleton.h>
#include "sns_reg_api_v02.h"
#include "sns_smgr_api_v01.h"
#include "sensor1.h"
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <utils/Log.h>
#include <common_log.h>
#include <stdbool.h>

#ifdef DSPS
#include <sns_smgr_common_v01.h>

#define ARRAY_SIZE(x) (int)(sizeof(x) / sizeof((x)[0]))

struct sensor_name_to_test_type {
    const char *name;
    int test_type;
};

const struct sensor_name_to_test_type accel_sensor[] = {
    {"BMA2X2 Accelerometer/Temperature/Double-tap",
            SNS_SMGR_TEST_OEM_V01},
    {"BMI058 Accelerometer/Temperature/Double-tap",
            SNS_SMGR_TEST_OEM_V01},
    {"BMI055 Accelerometer/Temperature/Double-tap",
            SNS_SMGR_TEST_OEM_V01},
    {"KXCJK Accelerometer", SNS_SMGR_TEST_OEM_V01},
    {"BMA250 Accelerometer", SNS_SMGR_TEST_OEM_V01},
    {"BMA150 Accelerometer", -1}, /* -1 is not support test */
    {"kxtik Accelerometer", SNS_SMGR_TEST_OEM_V01},
    {"LSM303D Accelerometer", SNS_SMGR_TEST_SELF_SW_V01},
    {"MMA8452 Accelerometer", SNS_SMGR_TEST_OEM_V01},
    {"MC3410 Accelerometer", SNS_SMGR_TEST_OEM_V01},
    {"LIS3DSH Accelerometer", SNS_SMGR_TEST_OEM_V01},
    {"LIS3DH Accelerometer", -1},
    {"ADXL350 Accelerometer", -1},
    {"MPU6515 Accelerometer", SNS_SMGR_TEST_SELF_HW_V01},
    {"MPU6050 Accelerometer, Gyroscope, Temperature", SNS_SMGR_TEST_SELF_V01}
};

const struct sensor_name_to_test_type gyro_sensor[] = {
    {"BMI160 Gyroscope", SNS_SMGR_TEST_OEM_V01},
    {"BMI058 Gyroscope", SNS_SMGR_TEST_OEM_V01},
    {"BMI055 Gyroscope", SNS_SMGR_TEST_OEM_V01},
    {"BMG160 Gyroscope", SNS_SMGR_TEST_OEM_V01},
    {"L3G4200D Gyroscope", -1},
    {"L3GD20 Gyroscope", SNS_SMGR_TEST_OEM_V01},
    {"MPU3050 GYROSCOPE", SNS_SMGR_TEST_SELF_V01},
    {"EWTSAFMA Gyro", SNS_SMGR_TEST_SELF_SW_V01},
    {"EWTZNA Gyro", SNS_SMGR_TEST_SELF_SW_V01},
    {"MPU6515 Gyroscope", SNS_SMGR_TEST_SELF_HW_V01},
    {"MPU6050 Accelerometer, Gyroscope, Temperature", SNS_SMGR_TEST_SELF_V01}
};

const struct sensor_name_to_test_type pro_sensor[] = {
    {"Proximity", SNS_SMGR_TEST_SELF_V01},
    {"APDS-9960 Proximity", SNS_SMGR_TEST_OEM_V01},
    {"APDS-9950 Proximity & Ambient", SNS_SMGR_TEST_OEM_V01},
    {"APDS-9900/QPDS-T900 Proximity & Light", SNS_SMGR_TEST_OEM_V01},
    {"APDS-9930/QPDS-T930, Proximity & Light", SNS_SMGR_TEST_OEM_V01},
    {"TMG399X ALS_PRX", -1},
    {"TMD277x ALSPRX", -1},
    {"CT406 ALSPRX", -1},
    {"LTR55X ALSPRX", SNS_SMGR_TEST_SELF_V01},
    {"ISL29147 ALSPRX", SNS_SMGR_TEST_SELF_V01},
    {"ISL29044A ALSPRX", SNS_SMGR_TEST_SELF_V01},
    {"ISL29028 ALSPRX", -1},
    {"ISL29011 ALSPRX", -1},
    {"CM36283 ALSPRX", SNS_SMGR_TEST_SELF_V01},
    {"AP3216C ALSPRX", SNS_SMGR_TEST_SELF_V01},
    {"MAX44006 MAX44006 ALS", SNS_SMGR_TEST_OEM_V01}
};

const struct sensor_name_to_test_type als_sensor[] = {
    {"APDS-9960 Light", SNS_SMGR_TEST_OEM_V01},
    {"APDS-9950 Proximity & Ambient", SNS_SMGR_TEST_OEM_V01},
    {"APDS-9900/QPDS-T900 Proximity & Light", SNS_SMGR_TEST_OEM_V01},
    {"APDS-9930/QPDS-T930, Proximity & Light", SNS_SMGR_TEST_OEM_V01},
    {"TMG399X ALS_PRX", SNS_SMGR_TEST_OEM_V01},
    {"TMD277x, ALSPRX", SNS_SMGR_TEST_OEM_V01},
    {"CT406, ALSPRX", -1},
    {"LTR55X ALSPRX", SNS_SMGR_TEST_SELF_V01},
    {"ISL29147 ALSPRX", SNS_SMGR_TEST_SELF_V01},
    {"ISL29044A ALSPRX", SNS_SMGR_TEST_SELF_V01},
    {"ISL29028 ALSPRX", -1},
    {"ISL29033 ALS", SNS_SMGR_TEST_SELF_V01},
    {"ISL29011 ALSPRX", -1},
    {"CM36283 ALSPRX", SNS_SMGR_TEST_SELF_V01},
    {"AP3216C ALSPRX", SNS_SMGR_TEST_OEM_V01},
    {"BH1721FVC Ambient Light Sensor", -1},
    {"MAX44006 MAX44006 ALS", SNS_SMGR_TEST_OEM_V01},
    {"AL3320B ALS", -1}
};
#endif
#endif


