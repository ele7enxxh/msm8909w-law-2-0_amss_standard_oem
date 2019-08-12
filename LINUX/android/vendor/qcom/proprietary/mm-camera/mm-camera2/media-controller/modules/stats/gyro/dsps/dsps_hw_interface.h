/*============================================================================
   Copyright (c) 2012 - 2015 Qualcomm Technologies, Inc. All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.

   This file defines the media/module/master controller's interface with the
   DSPS modules.

============================================================================*/

#ifndef _DSPS_HW_INTERFACE_H_
#define _DSPS_HW_INTERFACE_H_

#include "time.h"
#include "stats_event.h"


typedef enum {
  DSPS_ENABLE_REQ,
  DSPS_DISABLE_REQ,
  DSPS_GET_REPORT
} dsps_req_msg_type_t;

typedef enum _dsps_data_type {
  DSPS_DATA_TYPE_GYRO,
  DSPS_DATA_TYPE_GRAVITY_VECTOR,
  DSPS_DATA_TYPE_ACCELEROMETER,
  DSPS_DATA_TYPE_ROTATION_VECTOR,
} dsps_data_type;

typedef struct {
  uint8_t id;
  uint64_t t_start; /* Start Time (microsec) */
  uint64_t t_end;   /* End Time (microsec) */
} dsps_data_t;

typedef struct {
  uint32_t gyro_sample_rate;
  dsps_data_t data;
} dsps_set_data_gyro_t;

typedef struct {
  uint32_t report_period;
  uint8_t sample_rate_valid;
  uint32_t sample_rate;
} dsps_set_data_gravity_t;

typedef struct {
  dsps_data_type sensor_type;
  dsps_req_msg_type_t msg_type;
  union {
    dsps_set_data_gravity_t gravity;
    dsps_set_data_gyro_t gyro;
  } u;
} dsps_set_data_t;

typedef struct {
  float gravity[3];
  float lin_accel[3];
  uint8_t accuracy;
}dsps_cb_data_gravity_t;

typedef struct {
  uint8_t seq_no;
  uint32_t sample_len;  /**< Must be set to # of elements in sample */
  gyro_samples_t sample[STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE];
} dsps_cb_data_gyro_t;

typedef struct {
  dsps_data_type type;
  union {
    dsps_cb_data_gyro_t gyro;
    dsps_cb_data_gravity_t gravity;
  } u;
} dsps_cb_data_t;

typedef void (*dsps_callback_func)(void *port, dsps_cb_data_t *cb_data);

/* Function Prototypes */
void* dsps_proc_init(void *port, dsps_callback_func dsps_callback);
void dsps_proc_deinit(void *dsps_client_handle);
int dsps_proc_set_params(void *dsps_client_handle, dsps_set_data_t *data);

#endif /* _DSPS_HW_INTERFACE_H_ */
