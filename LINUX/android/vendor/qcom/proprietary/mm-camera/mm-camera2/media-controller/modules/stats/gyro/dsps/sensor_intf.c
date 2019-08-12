/* sensor_intf.c
 *
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
*/
#include "sensor_intf.h"
#ifdef _ANDROID_
#include "sensor_native.h"
#endif


#ifdef _ANDROID_
//#define AND_SENSOR_LOGS_ENABLE
#ifdef AND_SENSOR_LOGS_ENABLE
#define CDBG CDBG_ERROR
#define CDBG_LOW CDBG_ERROR
#define CDBG_HIGH CDBG_ERROR
#endif
/*===========================================================================
 * FUNCTION      sensor_enable_req
 *    @sn_obj: handler of native interface
 *    @sensor: sensor type
 *
 * DESCRIPTION : Request to enable native sensor
 *=========================================================================*/
static sensor_return_type sensor_enable_req(sensor_native_type *sn_obj,
  sensor_type sensor)
{
  int rc = SENSOR_RET_SUCCESS;
  ASensorRef android_sensor = NULL;
  int32_t sample_rate = 0;
  boolean *enabled_success = NULL;
  switch (sensor) {
  case SENSOR_TYPE_GYRO: {
    if (sn_obj->gyro.present) {
      CDBG("%s:Enable Gyro sensor and set default rate!",__func__);
      android_sensor = sn_obj->gyro.sensor;
      sample_rate = sn_obj->gyro.sample_rate;
      enabled_success = &sn_obj->gyro.enabled;
    } else {
      CDBG_ERROR("%s:Try to enable gyro but not present",__func__);
      rc = SENSOR_RET_UNKNOWN;
    }
  }
    break;
  case SENSOR_TYPE_GRAVITY: {
    if (sn_obj->gravity.present) {
      CDBG("%s:Enable gravity sensor and set default rate!",__func__);
      android_sensor = sn_obj->gravity.sensor;
      sample_rate = sn_obj->gravity.sample_rate;
      enabled_success = &sn_obj->gravity.enabled;
    } else {
      CDBG_ERROR("%s:Try to enable gravity but not present",__func__);
      rc = SENSOR_RET_UNKNOWN;
    }
  }
    break;
  case SENSOR_TYPE_ACCELEROMETER: {
    if (sn_obj->accel.present) {
      CDBG("%s:Enable Accel sensor and set default rate!",__func__);
      android_sensor = sn_obj->accel.sensor;
      sample_rate = sn_obj->accel.sample_rate;
      enabled_success = &sn_obj->accel.enabled;
    } else {
      CDBG_ERROR("%s:Try to enable accelerometer but not present",__func__);
      rc = SENSOR_RET_UNKNOWN;
    }
  }
    break;
  case SENSOR_TYPE_LINEAR_ACCELERATION: {
    if (sn_obj->lin_accel.present) {
      CDBG("%s:Enable linear acceleration sensor and set default rate!",__func__);
      android_sensor = sn_obj->lin_accel.sensor;
      sample_rate = sn_obj->lin_accel.sample_rate;
      enabled_success = &sn_obj->lin_accel.enabled;
    } else {
      CDBG_ERROR("%s:Try to enable linear acceleration but not present",__func__);
      rc = SENSOR_RET_UNKNOWN;
    }
  }
    break;
  default:
    CDBG_ERROR("%s:Unkown sensor type: %d",__func__, sensor);
    rc = SENSOR_RET_BAD_PARAMS;
    break;
  }

  /* Calls to android sensor interface to perform operations */
  if (rc == SENSOR_RET_SUCCESS) {
    if (enabled_success) {
      /* enable the sensor only after eventqueue is created*/
      pthread_mutex_lock(&(sn_obj->cb_mutex));
      if(sn_obj->event_queue) {
        rc = ASensorEventQueue_enableSensor(sn_obj->event_queue, android_sensor);
      } else {
        rc = -1;
      }
      pthread_mutex_unlock(&(sn_obj->cb_mutex));
      if (rc < 0) {
        CDBG_ERROR("%s:Fail to enable sensor",__func__);
      } else {
        pthread_mutex_lock(&(sn_obj->cb_mutex));
        *enabled_success = TRUE;
        pthread_mutex_unlock(&(sn_obj->cb_mutex));
        rc = ASensorEventQueue_setEventRate(sn_obj->event_queue, android_sensor,
          sample_rate);
        if (rc < 0) {
          CDBG_ERROR("%s:Fail to set event rate",__func__);
        }
      }
    } else {
      CDBG_ERROR("%s:Fail to init param enabled_success",__func__);
      rc = SENSOR_RET_BAD_PARAMS;
    }
  }

  if (rc < 0)
    rc = SENSOR_RET_FAILURE;

  return rc;
}

/*===========================================================================
 * FUNCTION      sensor_disable_req
 *    @sn_obj: handler of native interface
 *    @sensor: sensor type
 *
 * DESCRIPTION : Request to disable native sensor
 *=========================================================================*/
static sensor_return_type sensor_disable_req(sensor_native_type *sn_obj,
  sensor_type sensor)
{
  int rc = SENSOR_RET_SUCCESS;
  switch (sensor) {
  case SENSOR_TYPE_GYRO: {
    if (sn_obj->gyro.enabled) {
      CDBG("%s:Disable Gyro Sensor!",__func__);
      pthread_mutex_lock(&(sn_obj->cb_mutex));
      sn_obj->gyro.enabled = FALSE;
      pthread_mutex_unlock(&(sn_obj->cb_mutex));
      if(sn_obj->event_queue) {
        rc = ASensorEventQueue_disableSensor(sn_obj->event_queue,
          sn_obj->gyro.sensor);
      } else {
        CDBG_ERROR("Sensor event queue was already destroyed");
        rc = SENSOR_RET_BAD_PARAMS;
      }
    }
  }
  break;
  case SENSOR_TYPE_GRAVITY: {
    if (sn_obj->gravity.enabled) {
      CDBG("%s:Disable Gravity Sensor!",__func__);
      pthread_mutex_lock(&(sn_obj->cb_mutex));
      sn_obj->gravity.enabled = FALSE;
      pthread_mutex_unlock(&(sn_obj->cb_mutex));
      if(sn_obj->event_queue) {
        rc = ASensorEventQueue_disableSensor(sn_obj->event_queue,
          sn_obj->gravity.sensor);
      } else {
        CDBG_ERROR("Sensor event queue was already destroyed");
        rc = SENSOR_RET_BAD_PARAMS;
      }
    }
  }
  break;
  case SENSOR_TYPE_ACCELEROMETER: {
    if (sn_obj->accel.enabled) {
      CDBG("%s:Disable Acceleroemter Sensor!",__func__);
      pthread_mutex_lock(&(sn_obj->cb_mutex));
      sn_obj->accel.enabled = FALSE;
      pthread_mutex_unlock(&(sn_obj->cb_mutex));
      if(sn_obj->event_queue) {
        rc = ASensorEventQueue_disableSensor(sn_obj->event_queue,
         sn_obj->accel.sensor);
      } else {
        CDBG_ERROR("Sensor event queue was already destroyed");
        rc = SENSOR_RET_BAD_PARAMS;
      }
    }
  }
  break;
  case SENSOR_TYPE_LINEAR_ACCELERATION: {
    if (sn_obj->lin_accel.enabled) {
      CDBG("%s:Disable Linear accel Sensor!",__func__);
      pthread_mutex_lock(&(sn_obj->cb_mutex));
      sn_obj->lin_accel.enabled = FALSE;
      pthread_mutex_unlock(&(sn_obj->cb_mutex));
      if(sn_obj->event_queue) {
        rc = ASensorEventQueue_disableSensor(sn_obj->event_queue,
          sn_obj->lin_accel.sensor);
      } else {
        CDBG_ERROR("Sensor event queue was already destroyed");
        rc = SENSOR_RET_BAD_PARAMS;
      }
    }
  }
  break;
  default:
    CDBG_ERROR("%s:Unkown sensor type: %d",__func__, sensor);
    rc = SENSOR_RET_BAD_PARAMS;
  break;
  }

  if (rc < 0)
    rc = SENSOR_RET_FAILURE;

  return rc;
}
#endif /* _ANDROID_ */


/*===========================================================================
 * FUNCTION      sensor_init
 *    @handle: Handler
 *    @cb_handle:Callback function of client
 *    @cb: Callback data fnction
 *
 * DESCRIPTION : initializaties the sensor interface
 *=========================================================================*/
sensor_return_type sensor_intf_init(
  void **handle,
  void *cb_hndl,
  sensor_data_cb cb)
{
  sensor_return_type rc = SENSOR_RET_SUCCESS;
#ifdef _ANDROID_
  /* just call native interface */
  rc = sensor_native_init(handle, cb_hndl, cb);
#else
  /* Handle compilation warnings */
  STATS_UNUSED(handle);
  STATS_UNUSED(cb_hndl);
  STATS_UNUSED(cb);
  rc = SENSOR_RET_FAILURE;
#endif
  return rc;
}

/*===========================================================================
 * FUNCTION      sensor_deinit
 *    @handle: Handler
 *
 * DESCRIPTION : Deinitializaties the sensor interface
 *=========================================================================*/
sensor_return_type sensor_intf_deinit(void *handle)
{
#ifdef _ANDROID_
  sensor_native_deinit(handle);
#else
  /* Handle compilation warnings */
  STATS_UNUSED(handle);
#endif
  return SENSOR_RET_SUCCESS;
}

/*===========================================================================
 * FUNCTION      sensor_set_params
 *    @handle: Handler
 *    @set_data : set/req data info
 *
 * DESCRIPTION : sets/config the sensor data.
 *=========================================================================*/
sensor_return_type sensor_set_params(
  void *handle, sensor_set_data_type set_data)
{
  sensor_return_type rc = SENSOR_RET_SUCCESS;
#ifdef _ANDROID_
  sensor_native_type *sn_obj = (sensor_native_type *)handle;
  switch (set_data.msg_type) {
  case SENSOR_SET_DATA_CONFIG:
    if (set_data.sensor == SENSOR_TYPE_GYRO) {
      sensor_data_range_type data_range = set_data.u.data_range;
      pthread_mutex_lock(&(sn_obj->sof_req_mutex));
      if (sn_obj->prev_request_proc) {
        CDBG("%s:sof_event :%d",__func__, data_range.id);
        sn_obj->sof_info.id = data_range.id;
        sn_obj->sof_info.t_start = data_range.t_start;
        sn_obj->sof_info.t_end = data_range.t_end;
        sn_obj->prev_request_proc = FALSE ;
      } else {
        CDBG("%s:New sof_event :%d",__func__, data_range.id);
        sn_obj->sof_request = TRUE;
        sn_obj->sof_info_next.id = data_range.id;
        sn_obj->sof_info_next.t_start = data_range.t_start;
        sn_obj->sof_info_next.t_end = data_range.t_end;
      }
      pthread_mutex_unlock(&(sn_obj->sof_req_mutex));
      rc = SENSOR_RET_SUCCESS;
    }
    break;
  case SENSOR_ENABLE_REQ:
    rc = sensor_enable_req(sn_obj, set_data.sensor);
    break;
  case SENSOR_DISABLE_REQ:
    rc = sensor_disable_req(sn_obj, set_data.sensor);
    break;
  default:
    CDBG_ERROR("%s:Error not handle msg_type: %d",__func__, set_data.msg_type);
    rc = SENSOR_RET_BAD_PARAMS;
    break;
  }
#else
  /* Handle compilation warnings */
  STATS_UNUSED(handle);
  STATS_UNUSED(set_data);
  rc = SENSOR_RET_FAILURE;
#endif /* _ANDROID_ */
  return rc;
}
