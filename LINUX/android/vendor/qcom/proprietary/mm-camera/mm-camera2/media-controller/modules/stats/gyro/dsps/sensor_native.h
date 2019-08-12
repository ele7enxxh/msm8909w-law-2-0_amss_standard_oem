
/* sensor_native.h
 *
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
*/
#ifndef __SENSOR_NATIVE_H__
#define __SENSOR_NATIVE_H__

#include <stdbool.h>
#include <stdlib.h>
//#include "stats_debug.h"
#include "sensor_intf.h"


typedef enum _sensor_native_thread_status
{
  SENSOR_NATIVE_THREAD_ON      =  (1<<0),
  SENSOR_NATIVE_THREAD_OFF     =  (1<<1),
  SENSOR_NATIVE_THREAD_SUSPEND =  (1<<2),
  SENSOR_NATIVE_THREAD_ERROR   =  (1<<3),
} sensor_native_thread_status_type;

typedef struct _sensor_native_sensor_info
{
  boolean present;
  boolean enabled;
  ASensorRef sensor;
  int32_t min_delay;
  int32_t sample_rate;
} sensor_native_sensor_info_type;

typedef struct _sensor_native
{
  boolean initialized;
  pthread_t thread_id;
  ASensorEventQueue* event_queue;
  ASensorManager* smgr;
  ALooper *looper;
  sensor_native_sensor_info_type accel;
  sensor_native_sensor_info_type gyro;
  sensor_native_sensor_info_type gravity;
  sensor_native_sensor_info_type lin_accel;
  sensor_native_thread_status_type thread_status;
  void* cb_hndl;
  sensor_data_cb data_cb;
  uint8_t seqnum;
  sensor_data_range_type sof_info;
  sensor_data_range_type sof_info_next;
  boolean sof_request;
  pthread_mutex_t sof_req_mutex;
  pthread_mutex_t cb_mutex;
  sensor_data_type cirq_buffer[GYRO_BUFFER_SIZE];
  boolean prev_request_proc; /* If previous request is processed */
  int16_t buf_pos;
  int16_t start_idx; /* Gyro request start timestamp index */
  int16_t end_idx;  /* Gyro request end timestamp index */
  int16_t pstart_idx; /* previous frame's start index */
  int16_t pend_idx; /* previous frame's end index */
  unsigned long long pend_ts; /*Last request end timestamp */
  int64_t time_correction;
  uint32_t tc_counter;
  boolean first_threadflag;
} sensor_native_type;


int sensor_native_event_handler(int fd, int events, void* data);

sensor_return_type sensor_native_init(
  void **handle,
  void *cb_hndl,
  sensor_data_cb cb);

sensor_return_type sensor_native_deinit(void *handle);

#endif /* __SENSOR_NATIVE_H__*/
