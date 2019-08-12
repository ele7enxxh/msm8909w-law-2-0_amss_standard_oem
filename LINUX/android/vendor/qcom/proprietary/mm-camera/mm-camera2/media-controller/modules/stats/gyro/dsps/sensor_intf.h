/* sensor_intf.h
 *
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
*/
#ifndef __SENSOR_INTF_H__
#define __SENSOR_INTF_H__
#include "camera_dbg.h"
#include "mtype.h"
#include "stats_event.h"
#ifdef _ANDROID_
#include <android/sensor.h>
#include <android/looper.h>
#endif
#include <utils/Log.h>
#include <cutils/properties.h>
//#define IS_ENABLE

#define GYRO_BUFFER_SIZE 100
#ifdef IS_ENABLE
#define DEFAULT_RATE_IN_US 5000
#else
#define DEFAULT_RATE_IN_US 20000
#endif

#define DEFAULT_RATE_IN_US_ACCEL 20000
#define MAX_Q_SIZE          5


#undef STATS_GYRO_USE_ANDROID_API
#ifdef _ANDROID_

#define STATS_GYRO_USE_ANDROID_API(use_android) {\
                                  char prop[PROPERTY_VALUE_MAX];\
                                  property_get("persist.camera.gyro.android", prop, "0");\
                                  use_android = (boolean)atoi(prop);};
#else
#define STATS_GYRO_USE_ANDROID_API(use_android) {use_android = 0;}
#endif





typedef enum _sensor_return {
  SENSOR_RET_SUCCESS,
  SENSOR_RET_FAILURE,
  SENSOR_RET_BAD_PARAMS,
  SENSOR_RET_NO_MEMMORY,
  SENSOR_RET_UNKNOWN,
} sensor_return_type;


typedef enum _sensor {
  SENSOR_TYPE_ACCELEROMETER,
  SENSOR_TYPE_GYRO,
  SENSOR_TYPE_MAGNETOMETER,
  SENSOR_TYPE_GRAVITY,
  SENSOR_TYPE_LINEAR_ACCELERATION,
  SENSOR_TYPE_MAX_AVAILABLE,
} sensor_type;


typedef struct _sensor_data_gyro {
  float x;
  float y;
  float z;
} sensor_data_gyro_type;

typedef struct _sensor_data_accel {
  float x;
  float y;
  float z;
} sensor_data_accel_type;

typedef struct _sensor_data {
  sensor_type type;
  uint64_t timestamp;
  union {
    sensor_data_gyro_type gyro;
    sensor_data_accel_type accel;
    sensor_data_accel_type gravity;
    sensor_data_accel_type lin_accel;
  } u;
} sensor_data_type;

typedef struct {
  uint8_t seqnum;
  uint32_t sample_len;  /**< Must be set to # of elements in sample */
  gyro_samples_t sample[STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE];
} sensor_gyro_buffer_data_type;

typedef struct {
  sensor_type type;
  uint64_t timestamp;
  union {
#ifdef IS_ENABLE
    sensor_gyro_buffer_data_type gyro;
#else
    sensor_data_gyro_type gyro;
#endif
    sensor_data_accel_type accel;
    sensor_data_accel_type gravity;
    sensor_data_accel_type lin_accel;
  } u;
} sensor_callback_data_type;

typedef enum {
  SENSOR_ENABLE_REQ,
  SENSOR_DISABLE_REQ,
  SENSOR_SET_DATA_CONFIG, /* Setup time interval of data per request */
} sensor_set_msg_type;

typedef enum {
  SENSOR_GET_FRAME_DATA,
  SENSOR_GET_LAST_FRAME_DATA,
} sensor_get_msg_type;

typedef struct {
  uint8_t id;
  uint64_t t_start; /* Start Time (microsec) */
  uint64_t t_end;   /* End Time (microsec) */
} sensor_data_range_type;

typedef struct {
  sensor_type sensor;
  sensor_set_msg_type msg_type;
  union {
    int32_t sample_rate; /* SENSOR_ENABLE_REQ */
    sensor_data_range_type data_range; /* SENSOR_SET_DATA_CONFIG */
  } u;
} sensor_set_data_type;

typedef struct {
  int num_of_sensors;
  sensor_type available_sensors[SENSOR_TYPE_MAX_AVAILABLE];
} sensor_list_type;

typedef enum {
  SENSOR_LIST,
  SENSOR_DATA,
} sensor_cb_type;

typedef struct {
  sensor_cb_type type;
  union {
    sensor_list_type sensor_list;
    sensor_callback_data_type sensor_data;
  } u;
} sensor_cb_struct_type;

typedef void (*sensor_data_cb)(
  void *p,
  sensor_cb_struct_type cb_event_data);

sensor_return_type sensor_intf_init(
  void **handle,
  void *cb_hndl,
  sensor_data_cb cb);

sensor_return_type sensor_intf_deinit(void *handle);

sensor_return_type sensor_set_params(
  void *handle, sensor_set_data_type sensor_set);

#endif /* __SENSOR_INTF_H__*/
