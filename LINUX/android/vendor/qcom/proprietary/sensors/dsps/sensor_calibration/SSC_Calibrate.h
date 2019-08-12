/*============================================================================
  @SSC_Calibrate.h

  Sensor calibration for ssc source file

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/
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

typedef enum user_cal_error_e {
    USER_CAL_SUCCESS = 0,  /* No error */
    USER_CAL_TIMEOUT = -1, /* Response error - timeout */
    USER_CAL_SENSOR1 = -2, /* Sensor1 Error */
    USER_CAL_BIAS    = -3, /* Error using bias */
    USER_CAL_REG     = -4 /* Error writing to registry */
} user_cal_error_e;

typedef struct {
    sensor1_handle_s *sensor1_handle;
    bool is_cb_arrived; /* If a RESP has been received yet */
    pthread_mutex_t cb_mutex; /* Wait for RESP */
    pthread_cond_t cb_cond; /* Wait for RESP */
    int resp_result;  /* Result of sensor cal message */
}sensor_cal_t;

class ssc_calibrate {
    sensor_cal_t *sensor_cal;
    uint32_t mTxn_id;  /* transaction id as per sensor1_msg_header_s */
    int mSensor_id;
    int mData_type;
    int mTest_type;
    int sensor_by_type(int type);
    int req_sensor_cal();
    int signal_response(int result);
    user_cal_error_e close_sensor1();
public:
    ssc_calibrate();
    ~ssc_calibrate();
    int calibrate(int sensor_id, int test_type, int data_type);
    int get_txn_id();
    sensor_cal_t* get_cal();
};
