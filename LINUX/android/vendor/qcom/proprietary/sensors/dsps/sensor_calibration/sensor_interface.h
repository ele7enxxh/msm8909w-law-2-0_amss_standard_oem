/*============================================================================
  @file sensor_calibrate.h

  Gyroscope calibration algorithm source file

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/
#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H
#include <sys/cdefs.h>

#define SENSOR_CAL_MODULE_TAG   0x12373948
#define SENSOR_CAL_MODULE_VERSION 1
#define SENSOR_CAL_MODULE_INFO   sensorcali
#define SENSOR_CAL_MODULE_INFO_STR   "sensorcali"

/* Default test just define here. If will be updted if
   test mode changed in vim sns_smgr_common_v01.h
 */
#define SNS_SMGR_TEST_DEFAULT (0x1000)

struct sensor_cal_cmd_t {
    int test_type;
    int data_type;
    int reserved[6];
};

struct sensor_cal_methods_t {
    int (*calibrate) (struct sensor_t *sensor, struct sensor_cal_cmd_t *cmd);
};

struct sensor_cal_module_t {
    /* Tag of the module */
    uint32_t        tag;
    /* Id of the module */
    char            *id;
    /* Version of the calibration module */
    uint32_t        version;
    /* Vendor of the calibration lib */
    char            *vendor;
    /* Point to the handle of this module */
    void            *dso;
    /* Callbacks of the calibration lib provided */
    struct sensor_cal_methods_t     *methods;
    /* The compatible sensors list for this library */
    int             reserved[6];
};

#endif
