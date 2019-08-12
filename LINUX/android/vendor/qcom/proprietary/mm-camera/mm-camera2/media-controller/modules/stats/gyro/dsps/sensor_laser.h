/* sensor_laser.h
 *
 * Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
*/

#ifndef __SENSOR_LASER_H__

#define HAL_LIB_NAME_DEF "/system/lib64/hw/sensors.hal.tof.so"
#define HAL_LIB_NAME_32 "/system/lib/hw/sensors.hal.tof.so"


#define SENSOR_TYPE_TIME_OF_FLIGHT 30
#define NUMBER_OF_EVENTS_POLLED 1

/**
 * Callback data to be sent after reading
 */
typedef struct {
  float range_mm;
  float range_cm;
  float signal_rate;
  float error_status;
  float rtn_amb_rate;
  float rtn_conv_time;
  float tv_sec;
  float tv_usec;
  float dmax_sq;
} sensor_laser_data_cb_t;


/**
 * Data callback to provide laser output
 **/
typedef void (*sensor_laser_data_cb)(void *p, sensor_laser_data_cb_t cb_event_data);

/**
 * Error values returned by SENSOR Laser interface
 */
typedef enum _sensor_laser_ret {
  SENSOR_LASER_RET_SUCCESS,
  SENSOR_LASER_RET_FAILURE,
  SENSOR_LASER_RET_BAD_PARAMS,
  SENSOR_LASER_RET_NO_MEMMORY,
  SENSOR_LASER_RET_HW_NOT_FOUND,
  SENSOR_LASER_RET_SYM_NOT_FOUND,
  SENSOR_LASER_RET_OPEN_FAILURE,
  SENSOR_LASER_RET_FAILURE_ENABLE,
  SENSOR_LASER_RET_UNKNOWN,
} sensor_laser_ret_type;

/**
 * SENSOR Laser thread status.
 */
typedef enum _sensor_laser_thread_status
{
  SENSOR_LASER_THREAD_ON,
  SENSOR_LASER_THREAD_OFF,
  SENSOR_LASER_THREAD_SUSPEND,
} sensor_laser_thread_status_type;



/**
 * Internal SENSOR Laser object.
 */
typedef struct _sensor_laser_obj
{
  pthread_t thread_id;
  void* cb_hndl;
  sensor_laser_data_cb data_cb;
  sensor_laser_thread_status_type thread_status;
  void *lib_handle;
  struct sensors_module_t *module;
  struct sensors_poll_device_t *device;
  struct sensor_t const *sensor_list;
  int sensor_handle;

} sensor_laser_obj_t;


sensor_laser_ret_type sensor_laser_init(
  void **handle,
  void *cb_hndl,
  sensor_laser_data_cb cb);

sensor_laser_ret_type sensor_laser_deinit(void *handle);

#endif

