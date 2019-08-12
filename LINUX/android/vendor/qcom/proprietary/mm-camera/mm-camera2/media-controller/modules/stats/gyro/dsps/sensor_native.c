/* sensor_native.c
 *
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
*/
#ifdef _ANDROID_
#include <pthread.h>
#include <utils/Timers.h>
#include "sensor_native.h"
#include "sensor_service_util.h"

/*keep in sync with hardware/sensor.h */

#define ASENSOR_TYPE_GRAVITY 9
#define ASENSOR_TYPE_LINEAR_ACCELERATION 10
#define STATS_UNUSED(x) (void)(x)

//#define AND_SENSOR_LOGS_ENABLE
#ifdef AND_SENSOR_LOGS_ENABLE
#define CDBG CDBG_ERROR
#define CDBG_LOW CDBG_ERROR
#define CDBG_HIGH CDBG_ERROR
#endif


sensor_native_type g_snobj;


/*===========================================================================
 * FUNCTION   timestamp_correction
 * timestamp correction b/w gettimeofday() and systemTime().
 * Correction is required because gyro port requests samples
 * using gettimeofday() timestamps but android sensor API uses systemTime()
 *=========================================================================*/
static void timestamp_correction(int64_t *tc)
{
  struct timeval tv;
  *tc = 0;
  gettimeofday(&tv, NULL);
  int64_t sys_time = systemTime(SYSTEM_TIME_MONOTONIC)/1000;
  int64_t get_time = ((int64_t)tv.tv_sec * 1000000) + tv.tv_usec;
  *tc = sys_time - get_time;
  CDBG("%s:sys_time : %lld - gettime: %lld = time_correction: %lld",
    __func__,sys_time, get_time, *tc);
}

static int wait_loop(void *data)
{
  sensor_native_type *sn_obj = (sensor_native_type *)data;
  do {
  /*sleep it for long time to avoid unneccessary wake up */
    sleep(2000);
  } while (sn_obj->first_threadflag == TRUE);
  return 0;
}


/*===========================================================================
 * FUNCTION      sensor_native_gyro_start_search_forward
 *    @handle:Handler
 *    @start_pos:Start position
 *
 * DESCRIPTION : Search forward in the buffer for starting sample
 *=========================================================================*/
static sensor_return_type sensor_native_gyro_start_search_forward(
  void *handle,
  int start_pos)
{
  sensor_native_type *sn_obj = (sensor_native_type *)handle;
  CDBG("%s:search forward for start pos",__func__);
  do {
    if (start_pos > GYRO_BUFFER_SIZE - 1) {
      start_pos = 0;
    }
    if (sn_obj->sof_info.t_start > sn_obj->cirq_buffer[start_pos].timestamp) {
      start_pos++;
    } else {
      sn_obj->start_idx = start_pos;
      return SENSOR_RET_SUCCESS;
    }
  } while (start_pos != sn_obj->end_idx);
  return SENSOR_RET_FAILURE;
}

/*===========================================================================
 * FUNCTION      sensor_native_gyro_start_search_backward
 *    @handle:Handler
 *    @start_pos: start position
 *
 * DESCRIPTION: Search backward in the buffer for starting sample
 *=========================================================================*/
static sensor_return_type sensor_native_gyro_start_search_backward(
  void *handle,
  int start_pos)
{
  sensor_native_type *sn_obj = (sensor_native_type *)handle;
  CDBG("%s:search backward for start pos",__func__);
  do{
    if (start_pos < 0) {
      start_pos = GYRO_BUFFER_SIZE - 1;
    }
    if (sn_obj->sof_info.t_start < sn_obj->cirq_buffer[start_pos].timestamp) {
      start_pos--;
    } else {
      sn_obj->start_idx = start_pos;
      return SENSOR_RET_SUCCESS;
    }
  } while (start_pos != sn_obj->end_idx);
  return SENSOR_RET_FAILURE;
}

/*===========================================================================
 * FUNCTION      sensor_native_get_thread_status
 *    @p: Handler
 *
 * DESCRIPTION: Returns the current status of the sensor thread
 *=========================================================================*/
static boolean sensor_native_get_thread_status(void *p)
{
  sensor_native_type *sn_obj = (sensor_native_type *)p;
  sensor_native_thread_status_type status = sn_obj->thread_status;
  boolean ret_val = TRUE;

  if (status & SENSOR_NATIVE_THREAD_ERROR) {
    CDBG("Thread is in error state, ret false");
    ret_val = FALSE;
  } else if (status & SENSOR_NATIVE_THREAD_OFF) {
    CDBG("Thread state is OFF, return false");
    ret_val = FALSE;
  } else if (status & SENSOR_NATIVE_THREAD_ON) {
    CDBG("Thread state is active, ret true");
    ret_val = TRUE;
  } else if (status & SENSOR_NATIVE_THREAD_SUSPEND) {
    CDBG("Thread is in suspend state, ret false");
    ret_val = FALSE;
  } else {
    CDBG_ERROR("Invalid_thread status");
    ret_val = FALSE;
  }

  CDBG("%s:Sensor Thread status: %d ret_val: %d",__func__, status, ret_val);
  return ret_val;
}

/*===========================================================================
 * FUNCTION      sensor_native_gyro_port_notify
 *    @handle:Handler
 *
 * DESCRIPTION :  Copy the requested gyro samples to the callback data struct and notify
 *                          Gyro port
 *=========================================================================*/
static sensor_return_type sensor_native_gyro_port_notify(void *handle)
{
  sensor_native_type *sn_obj = (sensor_native_type *)handle;
  sensor_cb_struct_type cb_event_data;
  sensor_gyro_buffer_data_type *gyro_data = NULL;
  int start_pos = sn_obj->start_idx;
  int end_pos = sn_obj->end_idx;
  int sample_len = 0;
  int idx = 0;
  /* check for number of samples available for frame
     maximum allowed samples are in range */
  sample_len = (start_pos < end_pos) ? (end_pos-start_pos + 1) :
    (GYRO_BUFFER_SIZE - start_pos + end_pos + 1);
  if (sample_len > STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE) {
    CDBG_ERROR("%s:Sample length crossed the maximum allowed",__func__);
    return SENSOR_RET_FAILURE;
  }

  /* Prepare data for CB */
  cb_event_data.type = SENSOR_DATA;
  cb_event_data.u.sensor_data.type = SENSOR_TYPE_GYRO;
  cb_event_data.u.sensor_data.timestamp = 0; /* not used */
  /* Getting data from cirq_buf */
  gyro_data = &cb_event_data.u.sensor_data.u.gyro;
  gyro_data->seqnum = sn_obj->sof_info.id;
  gyro_data->sample_len = sample_len;
  idx = 0;
  while (idx < sample_len) {
    gyro_data->sample[idx].timestamp =
      sn_obj->cirq_buffer[start_pos].timestamp;
    gyro_data->sample[idx].value[0]=
      sn_obj->cirq_buffer[start_pos].u.gyro.x;
    gyro_data->sample[idx].value[1]=
      sn_obj->cirq_buffer[start_pos].u.gyro.y;
    gyro_data->sample[idx].value[2]=
      sn_obj->cirq_buffer[start_pos].u.gyro.z ;
    start_pos++;
    idx++;
    if (start_pos > GYRO_BUFFER_SIZE - 1) {
      start_pos = start_pos % GYRO_BUFFER_SIZE;
    }
  }

  pthread_mutex_lock(&(sn_obj->cb_mutex));
  if (sn_obj->gyro.enabled) {
    sn_obj->data_cb(sn_obj->cb_hndl, cb_event_data);
  }
  pthread_mutex_unlock(&(sn_obj->cb_mutex));
  return SENSOR_RET_SUCCESS;
}

/*===========================================================================
 * FUNCTION      sensor_native_process_request
 *    @handle:Sensor handler object
 *
 * DESCRIPTION : Process the Gyro port request
 *=========================================================================*/
static sensor_return_type sensor_native_process_request(void *handle)
{
  sensor_native_type *sn_obj = (sensor_native_type *)handle;
  int end_pos = sn_obj->buf_pos - 1;
  int start_pos = sn_obj->pend_idx;
  unsigned long long start_ts = sn_obj->pend_ts;
  /* search for the end timestamp of gyro request in buffer */
  do {
    if (end_pos < 0) {
      end_pos = GYRO_BUFFER_SIZE - 1;
    }
    if (sn_obj->cirq_buffer[end_pos].timestamp < sn_obj->sof_info.t_end) {
      sn_obj->end_idx = end_pos;
      /* search for start position */
      if (sn_obj->sof_info.t_start == start_ts) {
        sn_obj->start_idx = start_pos;
      } else if (sn_obj->sof_info.t_start >
        sn_obj->cirq_buffer[start_pos].timestamp) {
        /* search forward */
        sensor_native_gyro_start_search_forward(handle,++start_pos);
      } else {
        /* search backward */
        sensor_native_gyro_start_search_backward(handle,--start_pos);
      }
        /* Requested data ready; Copy into circular queue */
      CDBG("%s:copying data to circular queue: start:%d end:%d",__func__,
        sn_obj->start_idx,sn_obj->end_idx);
      return sensor_native_gyro_port_notify(handle);
    }
    else{
      end_pos--;
    }
    }  while (end_pos != sn_obj->pend_idx);
  return SENSOR_RET_FAILURE;
}

/*===========================================================================
 * FUNCTION      sensor_native_enqueue
 *    @handle: data pointer of sensor object
 *    @sensor_data_type: sensor data
 *
 * DESCRIPTION   Store the received samples in a circular buffer
 *=========================================================================*/
static sensor_return_type sensor_native_enqueue(
  void *handle,
  sensor_data_type sensor_data)
{
  sensor_native_type *sn_obj = (sensor_native_type *)handle;
  if (sn_obj->buf_pos > GYRO_BUFFER_SIZE - 1) {
    sn_obj->buf_pos = 0;
  }
  int buf_pos =sn_obj->buf_pos;
  CDBG("%s:Storing sample in circular buffer:pos:%d",__func__, buf_pos);
  sn_obj->cirq_buffer[buf_pos].timestamp = sensor_data.timestamp;
  /*orienation changes in android frmaework. To make
     it in complaince with dsps; X <--> Y; Z --> -Z */
  sn_obj->cirq_buffer[buf_pos].u.gyro.x = (int)(sensor_data.u.gyro.y * (1 << 16));
  sn_obj->cirq_buffer[buf_pos].u.gyro.y = (int)(sensor_data.u.gyro.x * (1 << 16));
  sn_obj->cirq_buffer[buf_pos].u.gyro.z = -(int)(sensor_data.u.gyro.z * (1 << 16));

  /*  check for a request */
  if (sn_obj->sof_info.t_end != 0) {
    /* check wheteher the requested samples are available */
    if (sensor_data.timestamp >= sn_obj->sof_info.t_end) {
      /*Lock the update of SoF request from gyro_port*/
      pthread_mutex_lock(&(sn_obj->sof_req_mutex));
      /* The requested samples are available in buffer ;
         Copy them in to a circular queue and notify gyro port*/
      CDBG("%s:Requested samples are available in buffer",__func__);
      if (sensor_native_process_request(handle) != SENSOR_RET_SUCCESS) {
        CDBG_ERROR("%s:Gyro request was not processed",__func__);
      }
      sn_obj->pend_ts = sn_obj->sof_info.t_end;
      sn_obj->pend_idx = sn_obj->end_idx;
      sn_obj->pstart_idx = sn_obj->start_idx;
      /* set previous frame request processed */
      sn_obj->prev_request_proc = TRUE;
      /* Reset untill a request comes*/
      sn_obj->sof_info.t_end = 0;
      /* check if there is any new request and update the timestamps */
      if (sn_obj->sof_request) {
        sn_obj->sof_info.id = sn_obj->sof_info_next.id;
        sn_obj->sof_info.t_start = sn_obj->sof_info_next.t_start;
        sn_obj->sof_info.t_end = sn_obj->sof_info_next.t_end;
        sn_obj->sof_request = FALSE;
        sn_obj->prev_request_proc = FALSE;
      }
      pthread_mutex_unlock(&(sn_obj->sof_req_mutex));
     }
   }
     ++sn_obj->buf_pos;
  return SENSOR_RET_SUCCESS;
}

/*===========================================================================
 * FUNCTION      sensor_native_event_handler
 *    @fd: file descriptor
 *    @events: Bitmask of the poll events
 *    @data: data pointer for handler
 *
 * DESCRIPTION   looper callback function called by looper thread
 *                        if there are any events available from the sensor
 *=========================================================================*/
int sensor_native_event_handler(int fd, int events, void* data)
{
  STATS_UNUSED(fd);
  STATS_UNUSED(events);
  ASensorEvent event;
  sensor_native_type *sn_obj = (sensor_native_type *)data;
  sensor_cb_struct_type cb_event_data;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int64_t recv_time = ((int64_t)tv.tv_sec * 1000000) + tv.tv_usec;
  /* get the time stamp correction factor for every ten events */
  if (sn_obj->tc_counter % 10 == 0) {
    timestamp_correction(&sn_obj->time_correction);
  }
  sn_obj->tc_counter++;
  CDBG("%s:Sensor Event handler!",__func__);
  if (events == ALOOPER_EVENT_INPUT) {
    /* Get events from the queue one by one */
    while (ASensorEventQueue_getEvents(sn_obj->event_queue, &event, 1) > 0) {
      switch (event.type) {
        case ASENSOR_TYPE_ACCELEROMETER: {
          event.timestamp = event.timestamp/1000 - sn_obj->time_correction;
          CDBG("%s:Event type - Accelerometer!",__func__);
          CDBG("%s:Accel: evt_timestamp:%lld ns recv_timestamp: %lld ns"
            "x:%f y:%f z:%f",__func__,
            event.timestamp, recv_time,
            event.acceleration.x, event.acceleration.y,
            event.acceleration.z);
          /* callback the gyro port notifying about the accelerometer */
          cb_event_data.type = SENSOR_DATA;
          cb_event_data.u.sensor_data.type = SENSOR_TYPE_ACCELEROMETER;
          cb_event_data.u.sensor_data.timestamp = event.timestamp;
          cb_event_data.u.sensor_data.u.accel.x = event.acceleration.x;
          cb_event_data.u.sensor_data.u.accel.y = event.acceleration.y;
          cb_event_data.u.sensor_data.u.accel.z = event.acceleration.z;
          pthread_mutex_lock(&(sn_obj->cb_mutex));
          if (sn_obj->accel.enabled) {
            sn_obj->data_cb(sn_obj->cb_hndl, cb_event_data);
          }
          pthread_mutex_unlock(&(sn_obj->cb_mutex));
        }
        break;
        case ASENSOR_TYPE_GYROSCOPE: {
          sensor_data_type sensor_data;
          /* Time correction to make it in complaince with dsps */
          event.timestamp = event.timestamp / 1000 - sn_obj->time_correction;
          CDBG("%s: Event type - Gyroscope!",__func__);
          CDBG("%s:Gyro: evt_timestamp:%lld us recv_timestamp: %lld us diff:"
            "%lld x:%f y:%f z:%f",__func__,
            event.timestamp, recv_time,
            (recv_time - event.timestamp),
            event.uncalibrated_gyro.x_uncalib,
            event.uncalibrated_gyro.y_uncalib,
            event.uncalibrated_gyro.z_uncalib);
#ifdef IS_ENABLE
          sensor_data.type = SENSOR_TYPE_GYRO;
          sensor_data.timestamp = event.timestamp;
          sensor_data.u.gyro.x = event.uncalibrated_gyro.x_uncalib;
          sensor_data.u.gyro.y = event.uncalibrated_gyro.y_uncalib;
          sensor_data.u.gyro.z = event.uncalibrated_gyro.z_uncalib;
          CDBG("%s:start:%llu end:%llu",__func__,
            sn_obj->sof_info.t_start,
            sn_obj->sof_info.t_end);
          /* Return all data once per frame */
          sensor_native_enqueue(sn_obj, sensor_data);
#else
          /* callback the gyro port notifying about the Gyro */
          cb_event_data.type = SENSOR_DATA;
          cb_event_data.u.sensor_data.type = SENSOR_TYPE_GYRO;
          cb_event_data.u.sensor_data.timestamp = event.timestamp;
          cb_event_data.u.sensor_data.u.gyro.x = event.uncalibrated_gyro.x_uncalib;
          cb_event_data.u.sensor_data.u.gyro.y = event.uncalibrated_gyro.y_uncalib;
          cb_event_data.u.sensor_data.u.gyro.z = event.uncalibrated_gyro.z_uncalib;

          /* callback the gyro port notifying about the individaual Gyro sample*/
          pthread_mutex_lock(&(sn_obj->cb_mutex));
          if (sn_obj->gyro.enabled) {
            sn_obj->data_cb(sn_obj->cb_hndl, cb_event_data);
          }
          pthread_mutex_unlock(&(sn_obj->cb_mutex));
#endif
        }
        break;

        case ASENSOR_TYPE_GRAVITY: {
          event.timestamp = (event.timestamp / 1000) - sn_obj->time_correction;
          CDBG("%s:Event type - Gravity!",__func__);
          CDBG("%s:Gravity: evt_timestamp:%lld ns recv_timestamp: %lld ns"
            "x:%f y:%f z:%f",__func__,
             event.timestamp, recv_time,
             event.acceleration.x, event.acceleration.y,
             event.acceleration.z);
          cb_event_data.type = SENSOR_DATA;
          cb_event_data.u.sensor_data.type = SENSOR_TYPE_GRAVITY;
          cb_event_data.u.sensor_data.timestamp = event.timestamp;
          cb_event_data.u.sensor_data.u.gravity.x = event.acceleration.x;
          cb_event_data.u.sensor_data.u.gravity.y = event.acceleration.y;
          cb_event_data.u.sensor_data.u.gravity.z = event.acceleration.z;
          /* callback the gyro port notifying about the gravity*/
          pthread_mutex_lock(&(sn_obj->cb_mutex));
          if (sn_obj->gravity.enabled) {
            sn_obj->data_cb(sn_obj->cb_hndl, cb_event_data);
          }
          pthread_mutex_unlock(&(sn_obj->cb_mutex));
        }
        break;
        case ASENSOR_TYPE_LINEAR_ACCELERATION: {
          event.timestamp = (event.timestamp / 1000) - sn_obj->time_correction;
          CDBG("%s:Event type - Linear accel!",__func__);
          CDBG("%s:Linear accel: evt_timestamp:%lld ns recv_timestamp: %lld ns"
            "x:%f y:%f z:%f",__func__,
             event.timestamp, recv_time,
             event.acceleration.x, event.acceleration.y,
             event.acceleration.z);
          cb_event_data.type = SENSOR_DATA;
          cb_event_data.u.sensor_data.type = SENSOR_TYPE_LINEAR_ACCELERATION;
          cb_event_data.u.sensor_data.timestamp = event.timestamp;
          cb_event_data.u.sensor_data.u.lin_accel.x = event.acceleration.x;
          cb_event_data.u.sensor_data.u.lin_accel.y = event.acceleration.y;
          cb_event_data.u.sensor_data.u.lin_accel.z = event.acceleration.z;
          /* callback the gyro port notifying about the gravity*/
          pthread_mutex_lock(&(sn_obj->cb_mutex));
          if (sn_obj->lin_accel.enabled) {
            sn_obj->data_cb(sn_obj->cb_hndl, cb_event_data);
          }
          pthread_mutex_unlock(&(sn_obj->cb_mutex));
        }
          break;
      default:
        CDBG("%s:Event for sensor(type: %d) not interested.",__func__, event.type);
        break;
      }
    }
  } else {
    CDBG_ERROR("Error in sensor manager service, kill the thread" );
    sn_obj->thread_status |= SENSOR_NATIVE_THREAD_ERROR;
    sn_obj->first_threadflag = FALSE;
    if (sn_obj->looper) {
      CDBG("%s: Waking up looper thread!", __func__);
      ALooper_wake(sn_obj->looper);
    }
    CDBG("%s: Waiting for thread to join!", __func__);
    pthread_join(sn_obj->thread_id, NULL);
    CDBG("%s : thread joined successfully", __func__);
  }

  /* return 1 to continue receiving callbacks or
     0 to unregister callback from the looper */
  return 1;
}

/*===========================================================================
 * FUNCTION      sensor_native_thread_handler
 *    @arg: Arguments
 *
 * DESCRIPTION :  Sensor thread handler function
 *                          - Create sensor manager instance
 *                          - Look for available sensors to verify presence
 *                          - Create ALooper and ASensor event queue
 *                          - Disable sensors at ALooper exit
 *=========================================================================*/
static void* sensor_native_thread_handler(void *arg)
{
  sensor_native_type *sn_obj = (sensor_native_type *)arg;
  sensor_cb_struct_type cb_event_data;
  sensor_list_type sensor_list;
  sensor_list.num_of_sensors = 0;

  CDBG("%s:Started sensor thread handler!",__func__);
  CDBG("%s:Acquiring instance of sensor manager",__func__);
  /* Create instance of sensor manager */
  sn_obj->smgr = sensor_service_get_object();

  /* Get default Gyro sensor */
  CDBG("%s:Get Default Gyro sensor!",__func__);
  sn_obj->gyro.sensor =
    ASensorManager_getDefaultSensor(sn_obj->smgr, ASENSOR_TYPE_GYROSCOPE);
  if (!sn_obj->gyro.sensor) {
    CDBG("%s:Gyro not found!",__func__);
    sn_obj->gyro.present = FALSE;
  } else {
    CDBG("%s:Gyro Sensor: %s found. Vendor: %s!",__func__,
      ASensor_getName(sn_obj->gyro.sensor),
      ASensor_getVendor(sn_obj->gyro.sensor));
    /* record gyro available */
    sn_obj->gyro.present = TRUE;
    sensor_list.available_sensors[sensor_list.num_of_sensors] =
      SENSOR_TYPE_GYRO;
    sensor_list.num_of_sensors++;

    /* get minimum delay allowed between events in us. A value of zero
       means event is reported only when available */
    sn_obj->gyro.min_delay = ASensor_getMinDelay(sn_obj->gyro.sensor);
    sn_obj->gyro.sample_rate =
      (sn_obj->gyro.min_delay < DEFAULT_RATE_IN_US) ?
      DEFAULT_RATE_IN_US : sn_obj->gyro.min_delay;
    CDBG("%s:Gyro - min_delay: %d sample_rate: %d!",__func__,
      sn_obj->gyro.min_delay, sn_obj->gyro.sample_rate);
  }

  /* Get default accelerometer sensor */
  sn_obj->accel.sensor =
    ASensorManager_getDefaultSensor(sn_obj->smgr, ASENSOR_TYPE_ACCELEROMETER);
  if (!sn_obj->accel.sensor) {
    CDBG_ERROR("%s:Accelerometer not found!",__func__);
    sn_obj->accel.present = FALSE;
  } else {
    CDBG("%s:Accelerometer Sensor: %s found. Vendor: %s!",__func__,
      ASensor_getName(sn_obj->accel.sensor),
      ASensor_getVendor(sn_obj->accel.sensor));
    /* Enable it if accelerometer data is needed */
    sn_obj->accel.present = TRUE;
    sensor_list.available_sensors[sensor_list.num_of_sensors] =
      SENSOR_TYPE_ACCELEROMETER;
    sensor_list.num_of_sensors++;

    sn_obj->accel.min_delay =
      ASensor_getMinDelay(sn_obj->accel.sensor);
    sn_obj->accel.sample_rate =
      (sn_obj->accel.min_delay < DEFAULT_RATE_IN_US_ACCEL) ?
      DEFAULT_RATE_IN_US_ACCEL : sn_obj->accel.min_delay;
    CDBG("%s:Accelerometer - min_delay: %d sample_rate: %d!",__func__,
      sn_obj->accel.min_delay, sn_obj->accel.sample_rate);
  }

  /* Get default gravity sensor */
  sn_obj->gravity.sensor =
    ASensorManager_getDefaultSensor(sn_obj->smgr, ASENSOR_TYPE_GRAVITY);
  if (!sn_obj->gravity.sensor) {
    CDBG_ERROR("%s:Gravity sensor not found!",__func__);
    sn_obj->gravity.present = FALSE;
  } else {
    CDBG("%s:Gravity Sensor: %s found. Vendor: %s!",__func__,
      ASensor_getName(sn_obj->gravity.sensor),
      ASensor_getVendor(sn_obj->gravity.sensor));
    /* Enable it if gravity data is needed */
    sn_obj->gravity.present = TRUE;
    sensor_list.available_sensors[sensor_list.num_of_sensors] =
      SENSOR_TYPE_GRAVITY;
    sensor_list.num_of_sensors++;

    sn_obj->gravity.min_delay =
      ASensor_getMinDelay(sn_obj->gravity.sensor);
    sn_obj->gravity.sample_rate =
      (sn_obj->gravity.min_delay < DEFAULT_RATE_IN_US_ACCEL) ?
      DEFAULT_RATE_IN_US_ACCEL : sn_obj->gravity.min_delay;
    CDBG("%s:Gravity - min_delay: %d sample_rate: %d!",__func__,
      sn_obj->gravity.min_delay, sn_obj->gravity.sample_rate);
  }
  /* Get default linear accelerometer sensor */
  sn_obj->lin_accel.sensor = ASensorManager_getDefaultSensor(sn_obj->smgr,
      ASENSOR_TYPE_LINEAR_ACCELERATION);
  if (!sn_obj->lin_accel.sensor) {
    CDBG_ERROR("%s:Linear Acceleration not found!",__func__);
    sn_obj->lin_accel.present = FALSE;
  } else {
    CDBG("%s:Linear Accelerometer Sensor: %s found. Vendor: %s!",__func__,
      ASensor_getName(sn_obj->lin_accel.sensor),
      ASensor_getVendor(sn_obj->lin_accel.sensor));
    /* Enable it if accelerometer data is needed */
    sn_obj->lin_accel.present = TRUE;
    sensor_list.available_sensors[sensor_list.num_of_sensors] =
      SENSOR_TYPE_LINEAR_ACCELERATION;
    sensor_list.num_of_sensors++;

    sn_obj->lin_accel.min_delay =
      ASensor_getMinDelay(sn_obj->lin_accel.sensor);
    sn_obj->lin_accel.sample_rate =
      (sn_obj->lin_accel.min_delay < DEFAULT_RATE_IN_US_ACCEL) ?
      DEFAULT_RATE_IN_US_ACCEL : sn_obj->lin_accel.min_delay;
    CDBG("%s:Linear Accelerometer - min_delay: %d sample_rate: %d!",
      __func__,sn_obj->lin_accel.min_delay, sn_obj->lin_accel.sample_rate);
  }

  /* Send list of available sensors */
  cb_event_data.type = SENSOR_LIST;
  cb_event_data.u.sensor_list = sensor_list;
  pthread_mutex_lock(&(sn_obj->cb_mutex));
  sn_obj->data_cb(sn_obj->cb_hndl, cb_event_data);

  /* Create looper thread */
  sn_obj->looper = ALooper_forThread();
  if(!sn_obj->looper) {
    CDBG("%s:Creating new looper thread!",__func__);
    sn_obj->looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
  }

  CDBG("%s:Create new event queue!",__func__);
  sn_obj->event_queue = ASensorManager_createEventQueue(
    sn_obj->smgr,
    sn_obj->looper,
    0/*LOOPER_POLL_CALLBACK*/,
    sensor_native_event_handler,
    (void *)sn_obj);
  pthread_mutex_unlock(&(sn_obj->cb_mutex));
  /* sensors will be enabled from gyro port only after event queue*/

  do {
    /* poll the looper.
       If the timeout is zero, returns immediately without blocking.
       If negative, waits indefinitely until an event appears.
        Time out : 100 ms */

    CDBG("%s:Polling the looper!",__func__);
    int ident = ALooper_pollOnce(100, NULL, NULL, NULL);
    CDBG("%s:Looper thread returned: %d",__func__,ident);
    boolean exit_loop = FALSE;
    switch (ident) {
      case ALOOPER_POLL_WAKE:
        CDBG("%s:Poll was awoken using wake before timeout.",__func__);
        exit_loop = FALSE;
        break;
      case ALOOPER_POLL_CALLBACK:
        CDBG("%s:One or more callbacks executed",__func__);
        exit_loop = FALSE;
        break;
      case ALOOPER_POLL_TIMEOUT:
        CDBG("%s:Timeout expired!",__func__);
        exit_loop = FALSE;
        break;
      case ALOOPER_POLL_ERROR:
        CDBG_ERROR("Error during poll.");
        exit_loop = FALSE;
        sn_obj->thread_status |= SENSOR_NATIVE_THREAD_ERROR;
        break;
      default:
        CDBG_ERROR("%s:Ident retured by poll: %d",__func__, ident);
        exit_loop = FALSE;
        break;
    }

    if (exit_loop) {
      CDBG("%s:Breaking out of do-while loop.",__func__);
      break;
    }
  } while (sensor_native_get_thread_status(sn_obj));

  /* DO not kill the first thread because of sensormanager issue */
  /* Issue link : https://code.google.com/p/android/issues/detail?id=80212 */
  if (sn_obj->first_threadflag == TRUE) {
    wait_loop(sn_obj);
  }

  /* Exiting the thread - do necessary stuff */
  /* Sensors will be disabled in deinit*/
  /*no need to call alooper_wake in deinit when crtl is here*/
  pthread_mutex_lock(&(sn_obj->cb_mutex));

  /* Destroy event queue */
  CDBG("%s:Destroy event queue!",__func__);
  ASensorManager_destroyEventQueue(sn_obj->smgr, sn_obj->event_queue);

#ifndef UPGRADE_SENSOR_MANAGER
  /* Delete service manager object */
  sensor_service_destroy(sn_obj->smgr);
#endif
  sn_obj->looper = NULL;

  pthread_mutex_unlock(&(sn_obj->cb_mutex));
  CDBG_LOW("Destroyed sensor service");

  /* Free the object if signaled by deinit*/
  if(sn_obj->thread_status & SENSOR_NATIVE_THREAD_OFF) {
    /* Free the object as camera session closed*/
    CDBG("camera session close, exit the thread; free obj");
    pthread_mutex_destroy(&sn_obj->cb_mutex);
    free(sn_obj);
  } else {
    /* This is error case, deinit will free the object */
    CDBG_ERROR("Error in sensor service manager, free in deinit");
  }

  return NULL;
}

/*===========================================================================
 * FUNCTION      sensor_native_init
 *    @handle: :Handler
 *    @cb_hndl:Pointer for callback
 *    @cb: data callback
 *
 * DESCRIPTION: Initialization of sensor thread
 *=========================================================================*/
sensor_return_type sensor_native_init(
  void **handle,
  void *cb_hndl,
  sensor_data_cb cb)
{
  sensor_native_type  *sn_obj = NULL;
  CDBG("%s:status mul_inst: %d",__func__, g_snobj.first_threadflag);
  /*if first thread already exists; go for dynamic objects */
  if (g_snobj.first_threadflag) {
    /* Local object (not first instance) */
    CDBG("%s:Alloc for internal structure, mul_inst: %d",__func__,
      g_snobj.first_threadflag);
    sn_obj = calloc(1, sizeof(sensor_native_type));
    if (sn_obj == NULL) {
      CDBG_ERROR("%s:Failure allocating memory!",__func__);
      return SENSOR_RET_NO_MEMMORY;
    }
    CDBG("%s:Alloc for internal structure: %p, mul_inst: %d",
      __func__,sn_obj, g_snobj.first_threadflag);
    sn_obj->data_cb = cb;
    sn_obj->cb_hndl = cb_hndl;
    sn_obj->first_threadflag = FALSE;
    /* Time correction between getTimeOfday and systemTime */
    timestamp_correction(&sn_obj->time_correction);
    if (sn_obj->initialized != TRUE) {
      sn_obj->thread_status |= SENSOR_NATIVE_THREAD_ON;
      /* For first frame no previous frame, hence true*/
      sn_obj->prev_request_proc = TRUE;
      /* Initialize thread and mutexes */
      pthread_mutex_init(&sn_obj->sof_req_mutex, NULL);
      pthread_mutex_init(&sn_obj->cb_mutex, NULL);
      pthread_create(&sn_obj->thread_id, NULL, sensor_native_thread_handler,
        (void *)sn_obj);
      CDBG("%s:thread id :%lu",__func__, sn_obj->thread_id);
      pthread_setname_np(sn_obj->thread_id, "GA2");
      sn_obj->initialized = TRUE;
      pthread_detach(sn_obj->thread_id);
    }
  } else {
    /* First time: use global object g_snobj */
    /* TODO: Remove this else part if google resolves the issue with SM
     * NOTE : The below created thread will not be killed during the entire
     *           target UP time; Consumes memory as we are not clearing up
     *           Need to remove this else part if google resolves the issue.
   Issue link : https://code.google.com/p/android/issues/detail?id=80212
     */
    CDBG("%s:Using g_snobj: %p, mul_inst: %d",
      __func__,&g_snobj, g_snobj.first_threadflag);
    sn_obj = &g_snobj;
    /*Only first thread enters into else condition; set the flag*/
    sn_obj->first_threadflag = TRUE;

    /* Init native circular buffer */
    memset(&sn_obj->sof_info, 0, sizeof(sensor_data_range_type));
    memset(&sn_obj->cirq_buffer, 0, GYRO_BUFFER_SIZE * sizeof(sensor_data_type));
    sn_obj->buf_pos = 0;
    sn_obj->start_idx = 0;
    sn_obj->end_idx = 0;
    sn_obj->pstart_idx = 0;
    sn_obj->pend_idx = 0;
    sn_obj->pend_ts = 0;

    /* For first frame no previous frame; hence true */
    sn_obj->prev_request_proc = TRUE;

    sn_obj->data_cb = cb;
    sn_obj->cb_hndl = cb_hndl;
    /* Time correction between getTimeOfday and systemTime */
    timestamp_correction(&sn_obj->time_correction);
    if (sn_obj->initialized != TRUE) {
      sn_obj->thread_status |= SENSOR_NATIVE_THREAD_ON;
      /* Initialize thread and mutexes */
      pthread_mutex_init(&sn_obj->sof_req_mutex, NULL);
      pthread_mutex_init(&sn_obj->cb_mutex, NULL);
      pthread_create(&sn_obj->thread_id, NULL, sensor_native_thread_handler,
        (void *)sn_obj);
      CDBG("%s:thread id :%lu",__func__, sn_obj->thread_id);
      pthread_setname_np(sn_obj->thread_id, "GA1");
      sn_obj->initialized = TRUE;
      pthread_detach(sn_obj->thread_id);
    }
  }
  *handle = (void *)sn_obj;
  return SENSOR_RET_SUCCESS;
}

/*===========================================================================
 * FUNCTION      sensor_native_deinit
 *    @handle:Handler
 *
 * DESCRIPTION : Deinitilize sensor thread
 *=========================================================================*/

sensor_return_type sensor_native_deinit(void *handle)
{
  sensor_native_type *sn_obj = (sensor_native_type *)handle;
  CDBG("%s:Deinitialize native sensor!",__func__);
  if (sn_obj->gyro.sensor && sn_obj->gyro.enabled) {
    CDBG("%s:Disable Gyro Sensor!",__func__);
    sn_obj->gyro.enabled = FALSE;
    ASensorEventQueue_disableSensor(sn_obj->event_queue,
      sn_obj->gyro.sensor);
  }

  if (sn_obj->accel.sensor && sn_obj->accel.enabled) {
    CDBG("%s:Disable Accelerometer Sensor!",__func__);
    sn_obj->accel.enabled = FALSE;
    ASensorEventQueue_disableSensor(sn_obj->event_queue,
      sn_obj->accel.sensor);
  }

  if (sn_obj->gravity.sensor && sn_obj->gravity.enabled) {
    CDBG("%s:Disable Gravity Sensor!",__func__);
    sn_obj->accel.enabled = FALSE;
    ASensorEventQueue_disableSensor(sn_obj->event_queue,
      sn_obj->gravity.sensor);
    sn_obj->gravity.enabled = FALSE;
  }

  if (sn_obj->lin_accel.sensor && sn_obj->lin_accel.enabled) {
    CDBG("%s:Disable Linear accel Sensor!",__func__);
    sn_obj->lin_accel.enabled = FALSE;
    ASensorEventQueue_disableSensor(sn_obj->event_queue,
      sn_obj->lin_accel.sensor);
  }

  /* Deinit mutexes and wait for thread to exit */

  /* wake up looper thread so it can exit */
  /* First time donot kill the thread */
  /* TODO:Remove this condition if google resolves the issue and make sure all *
   *               created threads are joined here after google resolves the issue*/
  /*   Issue link : https://code.google.com/p/android/issues/detail?id=80212 */
  sn_obj->initialized = FALSE;
  CDBG_LOW(" is it second or next thread ?: %d", !sn_obj->first_threadflag);
  pthread_mutex_destroy(&sn_obj->sof_req_mutex);

  /*Signal GA thread to exit and free the object */
  if(!(sn_obj->thread_status & SENSOR_NATIVE_THREAD_ERROR))
  {
     sn_obj->thread_status |= SENSOR_NATIVE_THREAD_OFF;
  } else {
    CDBG_ERROR("Error in sensorservice manager, free the object");
    pthread_mutex_lock(&(sn_obj->cb_mutex));
    if (sn_obj->looper) {
      CDBG_LOW("Waking up looper thread!");
      ALooper_wake(sn_obj->looper);
    }
    pthread_mutex_unlock(&(sn_obj->cb_mutex));
    /* first thread uses a global object; donot clear first object */
    if (!sn_obj->first_threadflag) {
      CDBG_LOW("Waiting for thread to join!");
      pthread_join(sn_obj->thread_id, NULL);
      pthread_mutex_destroy(&sn_obj->cb_mutex);
      free(sn_obj);
    } else {
      CDBG_LOW("Keeping thread alive and exit");
    }
    CDBG_LOW(" first thread ? : %d", sn_obj->first_threadflag);
  }
  return SENSOR_RET_SUCCESS;
}
#endif /* _ANDROID_ */
