/*============================================================================
  @file sensor_calibrate.cpp

  Sensor calibration source file

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

#include <stdio.h>
#include <linux/limits.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sensors.h>
#include "sensor_calibrate.h"
#include "sensor_interface.h"
#ifdef DSPS
#include "SSC_Calibrate.h"
#else
#define SYSFS_SENSORS "/sys/class/sensors/"
#include <sensors_extension.h>
#include <sensors_XML.h>
#endif

#define CAL_BUF_LENGTH        33 /*one cal_result and separator length*/
#define MAX_CAL_ELEMENTS 16 /* The maximum calibration elements supported */
#define MAX_STATIC_CAL_NUMBER 3

#ifdef DSPS
static void sensor_test_type(struct sensor_t *sensor, struct sensor_cal_cmd_t *cmd)
{
    int i;
    if(cmd != NULL && cmd->test_type == SNS_SMGR_TEST_DEFAULT)
        switch (sensor->type) {
        case SENSOR_TYPE_ACCELEROMETER:
            for (i = 0; i < ARRAY_SIZE(accel_sensor); i++) {
                if (strcmp(sensor->name, accel_sensor[i].name) == 0) {
                    cmd->test_type = accel_sensor[i].test_type;
                    break;
                }
            }
            break;
        case SENSOR_TYPE_PROXIMITY:
            for (i = 0; i < ARRAY_SIZE(pro_sensor); i++) {
                if (strcmp(sensor->name, pro_sensor[i].name) == 0) {
                    cmd->test_type = pro_sensor[i].test_type;
                    break;
                }
            }
            break;
        case SENSOR_TYPE_LIGHT:
            for (i = 0; i < ARRAY_SIZE(als_sensor); i++) {
                if (strcmp(sensor->name, als_sensor[i].name) == 0) {
                    cmd->test_type = als_sensor[i].test_type;
                    break;
                }
            }
            break;
        case SENSOR_TYPE_GYROSCOPE:
            for (i = 0; i < ARRAY_SIZE(gyro_sensor); i++) {
                if (strcmp(sensor->name, gyro_sensor[i].name) == 0) {
                    cmd->test_type = gyro_sensor[i].test_type;
                    break;
                }
            }
            break;
        case SENSOR_TYPE_GEOMAGNETIC_FIELD:
        default:
            cmd->test_type = SNS_SMGR_TEST_SELF_V01;
            break;
        }

}
static int sensor1_calibrate(struct sensor_t *sensor, struct sensor_cal_cmd_t *cmd)
{
    int sensor_id, data_type = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
    int err;

    switch(sensor->type) {
    case SENSOR_TYPE_ACCELEROMETER:
        sensor_id = SNS_SMGR_ID_ACCEL_V01;
        break;
    case SENSOR_TYPE_GYROSCOPE:
        sensor_id = SNS_SMGR_ID_GYRO_V01;
        break;
    case SENSOR_TYPE_LIGHT:
        cmd->data_type = SNS_SMGR_DATA_TYPE_SECONDARY_V01;
    case SENSOR_TYPE_PROXIMITY:
        sensor_id = SNS_SMGR_ID_PROX_LIGHT_V01;
        break;
    case SENSOR_TYPE_GEOMAGNETIC_FIELD:
        sensor_id = SNS_SMGR_ID_MAG_V01;
        break;
    default:
        return -1;
        break;
    }

    sensor_test_type(sensor, cmd);
    if (cmd->test_type == -1 || cmd->test_type == SNS_SMGR_TEST_DEFAULT) {
       LOGE("sensor %s unsupport cliabration function", sensor->name);
       return -1;
    }

    ssc_calibrate sensor_calibrate;
    err = sensor_calibrate.calibrate(sensor_id, cmd->test_type, cmd->data_type);

    return err;
}
#else
static int native_calibrate(char *path, const struct cal_cmd_t* cal_cmd,
        struct cal_result_t* cal_result)
{
    int fd;
    char buf[MAX_STATIC_CAL_NUMBER * CAL_BUF_LENGTH];
    char *token, *strsaveptr;
    int i, err;
    off_t offset;
    char *ptr;

    if (cal_cmd == NULL || cal_result == NULL) {
        LOGE("invalid calibration parameters");
        return -EINVAL;
    }

    fd = open(path, O_RDWR);
    if (fd < 0) {
        LOGE("open %s failed.(%s)", path, strerror(errno));
        return -1;
    }
    snprintf(buf, sizeof(buf), "%d", CMD_CAL(cal_cmd->axis, cal_cmd->apply_now));
    err = write(fd, buf, strlen(buf)+1);
    if (err <= 0) {
        LOGE("write %s failed.(%s)", path, strerror(errno));
        return -1;
    }

    offset = lseek(fd, 0, SEEK_SET);
    memset(buf, 0, sizeof(buf));

    err = read(fd, buf, sizeof(buf)-1);
    if (err < 0) {
        LOGE("read %s failed.(%s)", path, strerror(errno));
        close(fd);
        return err;
    }

    ptr = buf;
    for (i = 0; i < MAX_STATIC_CAL_NUMBER; i++, ptr = NULL) {
        token = strtok_r(ptr, ",", &strsaveptr);
        if(token == NULL)
            break;
        LOGE("token:%s", token);
        if(strlen(token) > CAL_BUF_LENGTH - 1) {
            LOGE("token %s is too long", token);
            close(fd);
            return -1;
        }
        cal_result->offset[i] = strtol(token, NULL, 0);
    }
    close(fd);

    return 0;
}


static int sensor_calibrate(struct sensor_t *sensor)
{
    int nNodes;
    struct dirent **namelist;
    char path[PATH_MAX];
    struct cal_cmd_t cal_cmd;
    struct cal_result_t result;
    char *file;
    int fd;
    char buf[2];
    int len;
    int err;

    if (sensor == NULL) {
        LOGE("Can only take one argument");
        return -1;
    }

    nNodes = scandir(SYSFS_SENSORS, &namelist, 0, alphasort);
    if (nNodes < 0) {
        LOGE("scan %s failed.(%s)", SYSFS_SENSORS, strerror(errno));
        return -1;
    }

    strcpy(path, SYSFS_SENSORS);
    strcat(path, sensor->name);
    strcat(path, "/");
    file = &path[strlen(path)];
    strcat(path, "type");
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        LOGE("open %s failed(%s)", path, strerror(errno));
        return -1;
    }

    len = read(fd, buf, sizeof(buf));
    if (len != sizeof(buf)) {
        LOGE("read %s failed.(%s)", path, strerror(errno));
        return -1;
    }

    close(fd);

    if (buf[0] == '0' + SENSOR_TYPE_PROXIMITY) {
        cal_cmd.axis = AXIS_BIAS;
    } else {
        cal_cmd.axis = AXIS_XYZ;
    }

    strcpy(file, "/calibrate");
    cal_cmd.save = 1;
    cal_cmd.apply_now = 1;

    if (native_calibrate(path, &cal_cmd, &result)) {
        LOGE("calibrate failed.");
        return -1;
    }

    LOGW("result:%d %d %d", result.offset[0], result.offset[1], result.offset[2]);

    sensors_XML& sensor_XML(sensors_XML :: getInstance());

    err = sensor_XML.write_sensors_params(sensor, &result, CAL_STATIC);
    if (err < 0) {
        LOGE("write calibrate %s sensor error", sensor->name);
        return err;
    }

    return 0;
}
#endif

int calibrate(struct sensor_t *sensor, struct sensor_cal_cmd_t *cmd) {
    int err;

#ifdef DSPS
    err = sensor1_calibrate(sensor, cmd);
#else
    err = sensor_calibrate(sensor);
#endif
    return err;
}

static struct sensor_cal_methods_t cal_methods = {
    .calibrate = calibrate,
};

struct sensor_cal_module_t SENSOR_CAL_MODULE_INFO = {
    .tag = SENSOR_CAL_MODULE_TAG,
    .id = "sensor_cal_module_qualcomm",
    .version = SENSOR_CAL_MODULE_VERSION,
    .vendor = "qualcomm",
    .dso = NULL,
    .methods = &cal_methods,
    .reserved = {0},
};

