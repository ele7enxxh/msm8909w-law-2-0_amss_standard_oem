/* gyro_port.c
 *
 * Copyright (c) 2013 - 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
*/

#include "mct_port.h"
#include "mct_stream.h"
#include "modules.h"
#include "stats_module.h"
#include "stats_event.h"
#include "dsps_hw_interface.h"
#include "dsps_hw.h"
#include "aec.h"
#include "camera_dbg.h"
#include "sensor_intf.h"
#include "sensor_laser.h"

#define Q8              (0x00000100)
#define BUF_SIZE        (4)
#ifndef MAX
#define MAX(a,b)        (((a) > (b)) ? (a) : (b))
#endif
//#define GYRO_LOGS_ENABLE
#ifdef GYRO_LOGS_ENABLE
#define CDBG CDBG_ERROR
#define CDBG_LOW CDBG_ERROR
#define CDBG_HIGH CDBG_ERROR
#endif

#define USE_LIN_ACCEL_IN_PLACE_OF_GYRO 0


/* Default gravity vector report period is at 30fps*/
#define GRAVITY_VECTOR_DEF_REPORT_PERIOD (0.033 * (1 << 16))
#define GYRO_SAMPLE_RATE_AF 66
#define GYRO_SAMPLE_RATE_EIS 200
#define MIN_SAMPLE_LEN_THRESHOLD 1

typedef enum {
  ACCELEROMETER_TYPE        = (0x1 << 0),
  GYRO_TYPE                 = (0x1 << 1),
  MAGNETOMETER_TYPE         = (0x1 << 2),
  GRAVITY_TYPE              = (0x1 << 3),
  LINEAR_ACCELERATION_TYPE  = (0x1 << 4),
} gyro_port_sensor_type;

typedef enum {
  GYRO_PORT_STATE_CREATED = 1,
  GYRO_PORT_STATE_RESERVED,
  GYRO_PORT_STATE_LINKED,
  GYRO_PORT_STATE_UNLINKED,
  GYRO_PORT_STATE_UNRESERVED
} gyro_port_state_t;


/** _gyro_port_private_t:
 *    @reserved_id: session id | stream id
 *    @state: state of gyro port
 *    @sof: start of frame timestamp in microseconds.
 *    @frame_time: time when the last line of the sensor is read in
 *                 microseconds.  Does not include vertical blanking time
 *    @last_time: end time of the last gyro sample interval in microseconds
 *    @exposure_time: exposure time in seconds
 *    @gyro_stats: MCT_EVENT_MODULE_STATS_GYRO_STATS event body
 *    @dsps_handle: DSPS handle for access to gyro data
 *    @rs_interval_offset: maximum rolling shutter gyro interval offset
 *    @s3d_interval_offset: maximum shake 3D gyro interval offset
 *    @gyro_sample_rate: gyro sample rate in Hz
 *    @is_type: IS type (DIS, EIS 1.0, etc.)
 *    @and_sens_handle: Android native sensor interface handle
 *    @sensor_list: List all availables sensors asigning one bit per sensor; type: gyro_port_sensor_type
 *
 *  This structure defines gyro port's private variables
 **/
typedef struct _gyro_port_private {
  unsigned int reserved_id;
  gyro_port_state_t state;
  unsigned int frame_id[BUF_SIZE];
  unsigned long long sof[BUF_SIZE];
  unsigned int sof_event_identity;
  unsigned long long frame_time;
  unsigned long long last_time;
  float exposure_time[BUF_SIZE];
  mct_event_gyro_stats_t gyro_stats;
  void *dsps_handle;
  int64_t rs_interval_offset;
  int64_t s3d_interval_offset;
  uint32_t gyro_sample_rate;
  cam_is_type_t is_type;
  void *and_sens_handle;
  unsigned int sensor_list;
  void *laser_sns_hndl;
  unsigned int current_sof_id;
  enum camb_position_t sensor_position;
  boolean stream_on;
} gyro_port_private_t;


/** sensor_laser_event_callback
 *    @port: gyro port
 *
 *  Callback received from laser sensor module.
 **/
static void sensor_laser_event_callback(
  void *port,
  sensor_laser_data_cb_t cb_event_data)
{
  if (port == NULL){
    CDBG_ERROR("%s: Port null",__func__);
    return;
  }
  gyro_port_private_t *private = ((mct_port_t *)port)->port_private;
  mct_event_t sensor_event;
  tof_update_t tof_data;

  if (private == NULL || (private->stream_on == FALSE)) {
    CDBG("%s: gyro port object null or stream is off",__func__);
    return;
  }

  CDBG("%s :Event:\n tv_sec: %f\n tv_usec: %f\n "
    "distance: %f mm\n distance: %f cm\n error_code: %f\n"
    "signalRate_mcps: %f\n rtnAmbRate(kcps): %f\n rtnConvTime: %f\n"
    "dmax_sq: %f\n",__func__,
    cb_event_data.tv_sec, cb_event_data.tv_usec, cb_event_data.range_mm,
    cb_event_data.range_cm, cb_event_data.error_status,
    cb_event_data.signal_rate, cb_event_data.rtn_amb_rate,
    cb_event_data.rtn_conv_time, cb_event_data.dmax_sq);

  /* Fill laser data structure */
  tof_data.frame_id = private->current_sof_id;
  tof_data.valid_data = 1;
  tof_data.timestamp.tv_sec = cb_event_data.tv_sec;
  tof_data.timestamp.tv_usec = cb_event_data.tv_usec;
  tof_data.range_mm = cb_event_data.range_mm;
  tof_data.signalRate_mcps = cb_event_data.signal_rate;
  tof_data.errorStatus = cb_event_data.error_status;
  tof_data.DMaxSq = cb_event_data.dmax_sq;
  tof_data.rtnAmbRate = cb_event_data.rtn_amb_rate;
  tof_data.rtnConvTime = cb_event_data.rtn_conv_time;

  if (tof_data.range_mm == 0) {
    tof_data.range_mm = 765;
  }

  /* Create event to be sent */
  CDBG("%s:Sending TOF_UPDATE to other module!",__func__);
  sensor_event.type = MCT_EVENT_MODULE_EVENT;
  sensor_event.identity = private->reserved_id;
  sensor_event.direction = MCT_EVENT_UPSTREAM;
  sensor_event.u.module_event.type = MCT_EVENT_MODULE_TOF_UPDATE;
  sensor_event.u.module_event.module_event_data = &tof_data;
  mct_port_send_event_to_peer(port, &sensor_event);
}

/** gyro_port_use_android_iface:
 *
 *  Returns true if gyro port is configured to use Android Sensor API
 **/
inline static boolean gyro_port_use_android_iface(void)
{
  boolean rc = FALSE;
  STATS_GYRO_USE_ANDROID_API(rc);
  return rc;
}

/** gyro_port_handle_sof_event:
 *    @port: gyro port
 *    @event: event object
 *    @private: gyro port private variables
 *
 * This function handles the SOF event.
 **/
static void gyro_port_handle_sof_event(mct_port_t *port, mct_event_t *event,
  gyro_port_private_t *private)
{
  mct_bus_msg_isp_sof_t *sof_event =
    (mct_bus_msg_isp_sof_t *)event->u.module_event.module_event_data;
  mct_event_t fps_event;
  unsigned long fps;
  unsigned long long sof, eof;
  unsigned long long t_start; /* Start Time (microsec) */
  unsigned long long t_end;   /* End Time (microsec) */
  struct timeval tv;
  int i;

  if (gettimeofday(&tv, NULL) == 0) {
    CDBG("%s, time = %llu, fid = %u, eid = %u", __func__,
      (((int64_t)tv.tv_sec * 1000000) + tv.tv_usec), sof_event->frame_id,
      event->identity);
  }

  /* Get FPS */
  fps_event.type = MCT_EVENT_MODULE_EVENT;
  fps_event.identity = event->identity;
  fps_event.direction = MCT_EVENT_UPSTREAM;
  fps_event.u.module_event.type = MCT_EVENT_MODULE_3A_GET_CUR_FPS;
  fps_event.u.module_event.module_event_data = &fps;
  mct_port_send_event_to_peer(port, &fps_event);
  fps /= Q8;

  i = sof_event->frame_id % BUF_SIZE;
  private->frame_id[i] = sof_event->frame_id;
  private->sof_event_identity = event->identity;
  private->current_sof_id = sof_event->frame_id;
  /* Calculate time interval */
  sof = (unsigned long long)sof_event->timestamp.tv_sec * SEC_TO_USEC
      + (unsigned long long)sof_event->timestamp.tv_usec;
  eof = sof + private->frame_time;
  t_start = private->last_time ? private->last_time : sof - 15000;
  /* Below is the gyro interval request end time for EIS 2.0 */
  t_end  = MAX(eof + private->rs_interval_offset,
   (sof + eof) / 2 + private->s3d_interval_offset);
  /* Below is the gyro interval request end time for EIS 1.0 */
  /* dsps_set.data.t_end = (sof + eof) / 2 -
    (long)((double)private->exposure_time[i] * SEC_TO_USEC / 2) + 5000; */
  private->last_time = t_end;
  private->sof[i] = sof;
  CDBG("%s: sof = %llu, frame_time = %llu, exp/2 = %f, fps = %lu",
    __func__, sof, private->frame_time,
    (double)private->exposure_time[i] * SEC_TO_USEC / 2, fps);

  CDBG("%s:Post for gyro interval %llu, %llu, %llu",__func__,
    t_start, t_end, t_end - t_start);

  /* Request gyro samples from t_start to t_end */
  if (private->dsps_handle) {
    dsps_set_data_t dsps_set;
    dsps_set.msg_type = DSPS_GET_REPORT;
    dsps_set.sensor_type = DSPS_DATA_TYPE_GYRO;
    dsps_set.u.gyro.data.id = sof_event->frame_id;
    dsps_set.u.gyro.data.t_start = t_start;
    dsps_set.u.gyro.data.t_end = t_end;
    dsps_proc_set_params(private->dsps_handle, &dsps_set);
  } else if (private->and_sens_handle &&
    (private->sensor_list & GYRO_TYPE)) {
#ifdef IS_ENABLE
    sensor_set_data_type sensor_set;
    sensor_set.msg_type = SENSOR_SET_DATA_CONFIG;
    sensor_set.sensor = SENSOR_TYPE_GYRO;
    sensor_set.u.data_range.id = sof_event->frame_id;
    sensor_set.u.data_range.t_start = t_start;
    sensor_set.u.data_range.t_end = t_end;
    sensor_set_params(private->and_sens_handle, sensor_set);
#endif
  }
}

/** dsps_port_callback_gyro
 *    @port: gyro port
 *    @frame_id: frame_id (LSB)
 *
 *  This is the callback function given to the DSPS layer.  It gets called when
 *  DSPS layer receives gyro samples from sensors.
 **/
static void dsps_port_callback_gyro(void *port, dsps_cb_data_gyro_t *input)
{
  gyro_port_private_t *private = ((mct_port_t *)port)->port_private;
  unsigned int i;

  CDBG("%s: frame id = %u", __func__, input->seq_no);

  i = input->seq_no % BUF_SIZE;
  if ((private->frame_id[i] & 0x0FF) == input->seq_no) {
    mct_event_gyro_stats_t gyro_stats;
    mct_event_t gyro_stats_event;
    unsigned int j;

    gyro_stats.is_gyro_data.sof = private->sof[i];
    gyro_stats.is_gyro_data.frame_time = private->frame_time;
    gyro_stats.is_gyro_data.exposure_time = private->exposure_time[i];

    gyro_stats.is_gyro_data.frame_id = private->frame_id[i];

    /* Verify that DSPS API struct sizes match size used by gyro port */
    if (STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE < input->sample_len) {
      CDBG_ERROR("%s: error: sample_len mismatch",__func__);
      return;
    }
    gyro_stats.is_gyro_data.sample_len = input->sample_len;
    for (j = 0; j < input->sample_len; j++) {
      gyro_stats.is_gyro_data.sample[j] = input->sample[j];
      CDBG("%s: gyro sample[%d], %llu, %d, %d, %d", __func__, j,
        gyro_stats.is_gyro_data.sample[j].timestamp,
        gyro_stats.is_gyro_data.sample[j].value[0],
        gyro_stats.is_gyro_data.sample[j].value[1],
        gyro_stats.is_gyro_data.sample[j].value[2]);
    }

    if (input->sample_len > MIN_SAMPLE_LEN_THRESHOLD) {
      gyro_stats.q16_angle[0] = input->sample[1].value[0];
      gyro_stats.q16_angle[1] = input->sample[1].value[1];
      gyro_stats.q16_angle[2] = input->sample[1].value[2];
      gyro_stats.ts = input->sample[1].timestamp;
    }

    gyro_stats_event.type = MCT_EVENT_MODULE_EVENT;
    gyro_stats_event.identity = private->sof_event_identity;
    gyro_stats_event.direction = MCT_EVENT_UPSTREAM;
    gyro_stats_event.u.module_event.type = MCT_EVENT_MODULE_STATS_GYRO_STATS;
    gyro_stats_event.u.module_event.module_event_data = &gyro_stats;
    mct_port_send_event_to_peer(port, &gyro_stats_event);
  } else {
    CDBG_ERROR("%s: gyro_port queue overflow", __func__);
  }
}

/** dsps_port_callback_gravity
 *    @port: gyro port
 *    @frame_id: frame_id (LSB)
 *
 *  This is the callback function given to the DSPS layer.  It gets called when
 *  DSPS layer receives gravity samples from sensors.
 **/
static void dsps_port_callback_gravity(void *port, dsps_cb_data_gravity_t *input)
{
  gyro_port_private_t *private = ((mct_port_t *)port)->port_private;
  mct_event_gravity_vector_update_t data;
  mct_event_t stats_event;


  CDBG("%s: Input data: Gravity (%f, %f, %f) Lin_Accel (%f, %f, %f)"
    "Accuracy: %d", __func__, input->gravity[0], input->gravity[1],
    input->gravity[2], input->lin_accel[0], input->lin_accel[1],
    input->lin_accel[2], input->accuracy);

  memcpy(&data.gravity, input->gravity, sizeof(float) * 3);
  memcpy(&data.lin_accel, input->lin_accel, sizeof(float) *3);
  data.accuracy = input->accuracy;

  stats_event.type = MCT_EVENT_MODULE_EVENT;
  stats_event.identity = private->sof_event_identity;
  stats_event.direction = MCT_EVENT_UPSTREAM;
  stats_event.u.module_event.type = MCT_EVENT_MODULE_GRAVITY_VECTOR_UPDATE;
  stats_event.u.module_event.module_event_data = &data;
  mct_port_send_event_to_peer(port, &stats_event);
}

/** dsps_port_callback
 *    @port: dsps port
 *    @cb_data: callback data
 *
 *  This is the callback function given to the DSPS layer.  It gets called when
 *  DSPS layer receives sensor data from motion sensors.
 **/
static void dsps_port_callback(void *port, dsps_cb_data_t *cb_data)
{
  switch (cb_data->type) {
  case DSPS_DATA_TYPE_GYRO: {
    CDBG("%s: DSPS Callback - data type GYRO!", __func__);
    dsps_port_callback_gyro(port, &cb_data->u.gyro);
  }
    break;
  case DSPS_DATA_TYPE_GRAVITY_VECTOR: {
    CDBG("%s: DSPS Callback - data type GRAVITY!", __func__);
    dsps_port_callback_gravity(port, &cb_data->u.gravity);
  }
    break;
  default:
    CDBG_ERROR("%s: DSPS Callback - data type (%d) not handled!", __func__,
      cb_data->type);
    break;
  }
}

/** sensor_event_cb_sensor_data
 *    @port: gyro port
 *    @sensor_data: Information with the type of data and data itself
 *
 *  This is a helper to the sensor_event_callback to hanlde sensor data callback
 **/
static void sensor_event_cb_sensor_data(void *port,
  sensor_callback_data_type* sensor_data)
{
  gyro_port_private_t *private = ((mct_port_t *)port)->port_private;
  switch (sensor_data->type) {
    case SENSOR_TYPE_GYRO: {
#ifdef IS_ENABLE
      CDBG("%s:SENSOR_TYPE_GYRO",__func__);
      mct_event_t gyro_stats_event;
      mct_event_gyro_stats_t gyro_stats;
      sensor_gyro_buffer_data_type *gyro_data = NULL;
      unsigned int i, j;
      uint8_t frame_id;
      gyro_data = &sensor_data->u.gyro;
      frame_id = gyro_data->seqnum;
      i = frame_id % BUF_SIZE;
      CDBG("%s:Received gyro samples",__func__);
      if((private->frame_id[i] & 0x0FF) == frame_id) {
        gyro_stats.is_gyro_data.sof = private->sof[i];
        gyro_stats.is_gyro_data.frame_time = private->frame_time;
        gyro_stats.is_gyro_data.exposure_time = private->exposure_time[i];

        gyro_stats.is_gyro_data.frame_id = private->frame_id[i];

        if (STATS_GYRO_MAX_SAMPLE_BUFFER_SIZE < gyro_data->sample_len) {
          CDBG_ERROR("%s:error: size mismatch",__func__);
          return;
        }
        gyro_stats.is_gyro_data.sample_len = gyro_data->sample_len;
        /* Fill data used mainly by IS */
        for (j = 0; j < gyro_data->sample_len; j++) {
          gyro_stats.is_gyro_data.sample[j] = gyro_data->sample[j];
          CDBG("%s:gyro sample[%d], %llu, %d, %d, %d",__func__, j,
          gyro_stats.is_gyro_data.sample[j].timestamp,
          gyro_stats.is_gyro_data.sample[j].value[0],
          gyro_stats.is_gyro_data.sample[j].value[1],
          gyro_stats.is_gyro_data.sample[j].value[2]);
        }
        /* Fill data used mainly by AF */
        if (gyro_data->sample_len > MIN_SAMPLE_LEN_THRESHOLD) {
          gyro_stats.q16_angle[0] = gyro_data->sample[1].value[0];
          gyro_stats.q16_angle[1] = gyro_data->sample[1].value[1];
          gyro_stats.q16_angle[2] = gyro_data->sample[1].value[2];
          gyro_stats.ts = gyro_data->sample[1].timestamp;
        }
        gyro_stats_event.type = MCT_EVENT_MODULE_EVENT;
        gyro_stats_event.identity = private->reserved_id ;
        gyro_stats_event.direction = MCT_EVENT_UPSTREAM;
        gyro_stats_event.u.module_event.type = MCT_EVENT_MODULE_STATS_GYRO_STATS;
        gyro_stats_event.u.module_event.module_event_data = &gyro_stats;
        mct_port_send_event_to_peer(port, &gyro_stats_event);
      } else {
        CDBG_ERROR("%s: gyro_port queue overflow", __func__);
      }
#else
        mct_event_t gyro_stats_event;
        mct_event_gyro_stats_t gyro_stats;
        CDBG("%s: Sensor Callback - data type GYRO!", __func__);
        CDBG("%s: timestamp:%lld us x:%f y:%f z:%f", __func__,
          sensor_data->timestamp, sensor_data->u.gyro.x, sensor_data->u.gyro.y,
          sensor_data->u.gyro.z);

          /*Af port expects data in q16 format and orientation also changes */
          gyro_stats.q16_angle[0] = sensor_data->u.gyro.y * (1 << 16);
          gyro_stats.q16_angle[1] = sensor_data->u.gyro.x* (1 << 16);
          gyro_stats.q16_angle[2] = -sensor_data->u.gyro.z * (1 << 16);
          gyro_stats.ts = sensor_data->timestamp;

          gyro_stats_event.type = MCT_EVENT_MODULE_EVENT;
          gyro_stats_event.identity = private->reserved_id ;
          gyro_stats_event.direction = MCT_EVENT_UPSTREAM;
          gyro_stats_event.u.module_event.type = MCT_EVENT_MODULE_STATS_GYRO_STATS_AF;
          gyro_stats_event.u.module_event.module_event_data = &gyro_stats;
          mct_port_send_event_to_peer(port, &gyro_stats_event);
#endif
      break;
    }
    case SENSOR_TYPE_GRAVITY: {
      mct_event_gravity_vector_update_t data;
      mct_event_t stats_event;
      CDBG("%s:Input data: Gravity (%f, %f, %f)",__func__,
        sensor_data->u.gravity.x,
        sensor_data->u.gravity.y,
        sensor_data->u.gravity.z);

      memset(&data, 0, sizeof(mct_event_gravity_vector_update_t));
     /* As algo is tuned with that of DSPS data; change the axes in
      * accordance  with that of DSPS. x<-->Y  ;  Z -> -Z */
      data.gravity[0] = sensor_data->u.gravity.y;
      data.gravity[1] = sensor_data->u.gravity.x;
      data.gravity[2] = -sensor_data->u.gravity.z;

      stats_event.type = MCT_EVENT_MODULE_EVENT;
      stats_event.identity = private->sof_event_identity;
      stats_event.direction = MCT_EVENT_UPSTREAM;
      stats_event.u.module_event.type = MCT_EVENT_MODULE_GRAVITY_VECTOR_UPDATE;
      stats_event.u.module_event.module_event_data = &data;
      mct_port_send_event_to_peer(port, &stats_event);
    }
      break;
    case SENSOR_TYPE_ACCELEROMETER: {
      /* TODO: Need to create an appropraite event for accelerometer
       * based on requirement*/
    }
      break;
    case SENSOR_TYPE_LINEAR_ACCELERATION: {
      /* TODO: Need to handle linear accelerometer based on requirement */
#if USE_LIN_ACCEL_IN_PLACE_OF_GYRO
        mct_event_t gyro_stats_event;
        mct_event_gyro_stats_t gyro_stats;
        CDBG("%s: Sensor Callback - data type lin_accel!", __func__);
        CDBG("%s: timestamp:%lld us x:%f y:%f z:%f", __func__,
          sensor_data->timestamp, sensor_data->u.lin_accel.x,
          sensor_data->u.lin_accel.y,
          sensor_data->u.lin_accel.z);

          /*Af port expects data in q16 format and orientation also changes */
          gyro_stats.q16_angle[0] = sensor_data->u.lin_accel.y * (1 << 16);
          gyro_stats.q16_angle[1] = sensor_data->u.lin_accel.x* (1 << 16);
          gyro_stats.q16_angle[2] = -sensor_data->u.lin_accel.z * (1 << 16);
          gyro_stats.ts = sensor_data->timestamp;

          gyro_stats_event.type = MCT_EVENT_MODULE_EVENT;
          gyro_stats_event.identity = private->reserved_id ;
          gyro_stats_event.direction = MCT_EVENT_UPSTREAM;
          gyro_stats_event.u.module_event.type = MCT_EVENT_MODULE_STATS_GYRO_STATS_AF;
          gyro_stats_event.u.module_event.module_event_data = &gyro_stats;
          mct_port_send_event_to_peer(port, &gyro_stats_event);
#endif
    }
      break;
    default:
      CDBG_ERROR("%s:Sensor Callback - data type (%d) not handled!",__func__,
        sensor_data->type);
      break;
  }
}
/** sensor_event_callback
 *    @port: gyro port
 *    @type: Sensor type like gyroscope or Accelerometer
 *    @frame_id: frame_id (LSB)
 *
 *  This is the callback function given to the Android sensor interface layer.
 *  It gets called when android interface layer receives sensor samples from sensors.
 **/
static void sensor_event_callback(
  void *port,
  sensor_cb_struct_type cb_event_data)
{
  gyro_port_private_t *private = ((mct_port_t *)port)->port_private;
  switch (cb_event_data.type) {
  case SENSOR_LIST: {
    int i = 0;
    cb_event_data.u.sensor_list;
    sensor_type *sensor = (sensor_type *)&cb_event_data.u.sensor_list.available_sensors;
    for (i = 0; i < cb_event_data.u.sensor_list.num_of_sensors; i++) {
      if (sensor[i] == SENSOR_TYPE_ACCELEROMETER) {
        private->sensor_list |= ACCELEROMETER_TYPE;
        CDBG("%s:Available: ACCELEROMETER_TYPE",__func__);
      } else if (sensor[i] == SENSOR_TYPE_GYRO) {
        private->sensor_list |= GYRO_TYPE;
        CDBG("%s:Available: GYRO_TYPE",__func__);
      } else if (sensor[i] == SENSOR_TYPE_GRAVITY) {
        private->sensor_list |= GRAVITY_TYPE;
        CDBG("%s:Available: GRAVITY_TYPE",__func__);
      } else if (sensor[i] == SENSOR_TYPE_LINEAR_ACCELERATION) {
        private->sensor_list |= LINEAR_ACCELERATION_TYPE;
        CDBG("%s:Available: LINEAR_ACCELERATION_TYPE",__func__);
      } else {
        CDBG("%s:Sensor not needed",__func__);
      }
    }
    CDBG("%s:Sensor list, num of sensors: %d, sensors: 0x%x",__func__,
      cb_event_data.u.sensor_list.num_of_sensors,
      private->sensor_list);
  }
    break;
  case SENSOR_DATA: {
    sensor_event_cb_sensor_data(port, &cb_event_data.u.sensor_data);
  }
    break;
  default:
    CDBG_ERROR("%s:Not valid cb type",__func__);
    break;
  }
}

/** gyro_port_init_sensors:
     *    @port: gyro port
     *
     * Initialize motion sensor framework.
     *
     *  Returns TRUE on success.
     **/

boolean gyro_port_init_sensors(mct_port_t *port)
{
  gyro_port_private_t *private = (gyro_port_private_t *)port->port_private;
  boolean rc = TRUE;
  /* Init sensor interface */
  if (!gyro_port_use_android_iface()){
    CDBG_HIGH("%s:Init DSPS intf",__func__);
    private->dsps_handle = dsps_proc_init(port, dsps_port_callback);
    if (private->dsps_handle) {
      rc = TRUE;
    } else {
      rc = FALSE;
      CDBG_ERROR("%s:DSPS fail to init",__func__);
    }
  }
  if (private->dsps_handle == NULL) {
    /* DSPS not available, try using Android sensor interface */
    /* Initialization done here to make sure sensors start before requesting
       to enable at dsps_port_enable_disable_request() */
    CDBG_HIGH("%s:Init Android sensor intf",__func__);
    rc = sensor_intf_init(&private->and_sens_handle, port, sensor_event_callback);
    if ((private->and_sens_handle == NULL) || (SENSOR_RET_SUCCESS != rc)) {
      CDBG_ERROR("%s:Error initializing Native Sensor!",__func__);
      rc = FALSE;
    } else {
      rc = TRUE;
    }
  }


  return rc;
}


/** gyro_port_deinit_sensors
 *    @private: gyro port private struct
 *
 *  First disable then deinit sensors modules (DSPS and Android sensors API)
 **/
static void gyro_port_deinit_sensors(gyro_port_private_t *private)
{
  if (private->dsps_handle) {
    dsps_set_data_t dsps_set_data;
    CDBG_HIGH("%s: DSPS Deinit", __func__);
    /* Disable Gyro */
    dsps_set_data.msg_type = DSPS_DISABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_GYRO;
    dsps_proc_set_params(private->dsps_handle, &dsps_set_data);

    /* Disable Gravity Vector */
    dsps_set_data.msg_type = DSPS_DISABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_GRAVITY_VECTOR;
    dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    dsps_proc_deinit(private->dsps_handle);
    private->dsps_handle = NULL;
  }

  if (private->and_sens_handle) {
    CDBG_HIGH("%s: Android sensor API Deinit", __func__);
    sensor_set_data_type sensor_set;
    sensor_set.msg_type = SENSOR_DISABLE_REQ;

    if (private->sensor_list & GYRO_TYPE) {
      sensor_set.sensor = SENSOR_TYPE_GYRO;
      sensor_set_params(private->and_sens_handle, sensor_set);
    }
    if (private->sensor_list & GRAVITY_TYPE) {
      sensor_set.sensor = SENSOR_TYPE_GRAVITY;
      sensor_set_params(private->and_sens_handle, sensor_set);
    }
    if (private->sensor_list & ACCELEROMETER_TYPE) {
      sensor_set.sensor = SENSOR_TYPE_ACCELEROMETER;
      sensor_set_params(private->and_sens_handle, sensor_set);
    }
    if (private->sensor_list & MAGNETOMETER_TYPE) {
      sensor_set.sensor = SENSOR_TYPE_MAGNETOMETER;
      sensor_set_params(private->and_sens_handle, sensor_set);
    }
    if (private->sensor_list & LINEAR_ACCELERATION_TYPE) {
      sensor_set.sensor = SENSOR_TYPE_LINEAR_ACCELERATION;
      sensor_set_params(private->and_sens_handle, sensor_set);
    }
  }

  return;
}
/** dsps_port_handle_set_is_enable
 *    @port: gyro port
 *    @is_enable: IS enable indication
 *
 * This function adjusts the gyro sampling rate based on use case to save
 * power.  When gyro-based IS is not active, gyro sampling rate is set low.
 * When gyro-based IS is active, the gyro sampling rate is set high.  Changing
 * the sampling rate requires a restart of gyro services.  General worse case
 * delay may be up to 40 ms.  However, this function executes during camera to
 * camcorder mode transitions (vice versa) and during the enabling/disabling of
 * IS from the camcorder menu which results in a restarting of the camera
 * pipeline.  In these contexts, the delay should not be too noticeable.
 *
 * If gyro restart to change sampling rate fails, leave gravity vector running.
**/
int dsps_port_handle_set_is_enable(gyro_port_private_t *private,
  int32_t is_enable)
{
  int rc = 0;
  uint32_t new_gyro_sample_rate = 0;

  if (private->dsps_handle == NULL) {
    /* DSPS services failed earlier at init or enable, return "no error" */
    return rc;
  }

  if (is_enable) {
    /* Change gyro sample rate only for IS technologies that require gyro */
    if (private->gyro_sample_rate == GYRO_SAMPLE_RATE_AF &&
        (private->is_type == IS_TYPE_EIS_1_0 ||
         private->is_type == IS_TYPE_EIS_2_0)) {
      new_gyro_sample_rate = GYRO_SAMPLE_RATE_EIS;
    }
  } else {
    if (private->gyro_sample_rate == GYRO_SAMPLE_RATE_EIS) {
      new_gyro_sample_rate = GYRO_SAMPLE_RATE_AF;
    }
  }

  if (new_gyro_sample_rate != 0) {
    dsps_set_data_t dsps_set_data;

    dsps_set_data.msg_type = DSPS_DISABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_GYRO;
    dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    if (rc == 0) {
      dsps_set_data.msg_type = DSPS_ENABLE_REQ;
      dsps_set_data.sensor_type = DSPS_DATA_TYPE_GYRO;
      dsps_set_data.u.gyro.gyro_sample_rate = new_gyro_sample_rate;
      rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
      if (rc == 0) {
        private->gyro_sample_rate = new_gyro_sample_rate;
        CDBG_HIGH("%s: Set gyro sample rate to %u Hz", __func__, new_gyro_sample_rate);
      } else {
        CDBG_ERROR("%s: Gyro enable req failed", __func__);
      }
    } else {
      CDBG_ERROR("%s: Gyro disable req failed", __func__);
    }
  }
  return rc;
}


/** gyro_port_enable_disable_sensor_request
 *    @port: gyro port
 *    @enable: indicates whether to enable or disable android sensor or DSPS services
 *
 *  At the start, gyro and gravity vector enablements must both both succeed.
 *  succeed.  Otherwise, DSPS or android sensors are deinited.
**/
void gyro_port_enable_disable_sensor_request(mct_port_t *port, boolean enable)
{
  int rc = 0;
  gyro_port_private_t *private;
  private = (gyro_port_private_t *)(port->port_private);
  dsps_set_data_t dsps_set_data;

  if (enable == TRUE) {
    if (private->dsps_handle != NULL) {
      CDBG_HIGH("%s:Enable dsps sensors",__func__);
      /* Enable gyro */
      dsps_set_data.msg_type = DSPS_ENABLE_REQ;
      dsps_set_data.sensor_type = DSPS_DATA_TYPE_GYRO;
      dsps_set_data.u.gyro.gyro_sample_rate = private->gyro_sample_rate;
      rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
      if (!rc) {
        CDBG_HIGH("%s: Set gyro sample rate to %u Hz", __func__,
          private->gyro_sample_rate);

        /* Enable gravity vector */
        memset(&dsps_set_data, 0, sizeof(dsps_set_data));
        dsps_set_data.msg_type = DSPS_ENABLE_REQ;
        dsps_set_data.sensor_type = DSPS_DATA_TYPE_GRAVITY_VECTOR;
        dsps_set_data.u.gravity.report_period =
          GRAVITY_VECTOR_DEF_REPORT_PERIOD;
        rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
        if (rc) {
          CDBG_ERROR("%s: Gravity enable req failed", __func__);
          gyro_port_deinit_sensors(private);
        }
      } else {
        CDBG_ERROR("%s: Gyro enable req failed", __func__);
        gyro_port_deinit_sensors(private);
      }
    } else if (private->and_sens_handle != NULL) {
      /* Not initializing Android sensor thread here because it takes time to start, and sensors are not prove
         by the time we try to enable them here */
      CDBG_HIGH("%s:Enable Android API sensors",__func__);
      if (private->sensor_list & GYRO_TYPE) {
        /* Enable Gyro sensor */
        sensor_set_data_type sensor_set;
        sensor_set.msg_type = SENSOR_ENABLE_REQ;
        sensor_set.sensor = SENSOR_TYPE_GYRO;
        sensor_set.u.sample_rate = DEFAULT_RATE_IN_US;
        rc = sensor_set_params(private->and_sens_handle, sensor_set);
        if (rc) {
          CDBG_ERROR("%s:Error to enable gyro",__func__);
          gyro_port_deinit_sensors(private);
        }
      /* TODO: Enabling gravity sensor here is blocking CTS for camera open,
                 as gravity enabling is taking around 300 ms. Need to handle it */
//        if (rc == SENSOR_RET_SUCCESS) {
//          if (private->sensor_list & GRAVITY_TYPE) {
//            /* Enable Gravity sensor */
//            sensor_set.msg_type = SENSOR_ENABLE_REQ;
//            sensor_set.sensor = SENSOR_TYPE_GRAVITY;
//            sensor_set.u.sample_rate = DEFAULT_RATE_IN_US;
//            rc = sensor_set_params(private->and_sens_handle, sensor_set);
//            if (rc != SENSOR_RET_SUCCESS) {
//              CDBG_ERROR("%s:Error to enable gravity",__func__);
//              gyro_port_deinit_sensors(private);
//            }
//          }
//        }
      } else {
      /* Gyro sensor is not supported; can use lin accel in place of gyro*/
#if USE_LIN_ACCEL_IN_PLACE_OF_GYRO
        if (private->sensor_list & LINEAR_ACCELERATION_TYPE) {
          /* Enable lin. accel sensor */
          sensor_set_data_type sensor_set;
          sensor_set.msg_type = SENSOR_ENABLE_REQ;
          sensor_set.sensor = SENSOR_TYPE_LINEAR_ACCELERATION;
          sensor_set.u.sample_rate = DEFAULT_RATE_IN_US;
          rc = sensor_set_params(private->and_sens_handle, sensor_set);
          if (rc != SENSOR_RET_SUCCESS) {
            CDBG_ERROR("%s:Error to Lin accel",__func__);
            gyro_port_deinit_sensors(private);
          }
        }
#endif
      }
    } else {
      CDBG_ERROR("%s:No handler to enable!",__func__);
    }

     /*###### INIT and Enable Laser sensor ######*/
     /* Note: Check if this is back camera. Currently we only support a single */
     /*back sensor for laser AF. For front we do not initialize laser sensor */
    if ((NULL == private->laser_sns_hndl) &&
      (BACK_CAMERA_B == private->sensor_position)){
      sensor_laser_ret_type ret;
      private->stream_on = FALSE;
      CDBG_HIGH("%s:Init Laser Sensor Interface!",__func__);
      ret = sensor_laser_init(&private->laser_sns_hndl, port,
        sensor_laser_event_callback);
      if (SENSOR_LASER_RET_SUCCESS != ret) {
        CDBG_ERROR("%s:Failure initializing laser sensor. Error: %d",__func__, ret);
      }
    }
  } else { /* enable == FALSE */
    CDBG_HIGH("%s: Deinit all sensors", __func__);
    gyro_port_deinit_sensors(private);
  }
}

/** dsps_port_enable_gravity_vector:
 *    @port: dsps port
 *    @input: input parameter
 *
 *  This function enables/disables gravity vector.
 *
 *  Returns TRUE on success.
 **/
int dsps_port_enable_gravity_vector(void *port, void *input)
{
  int rc = 0;
  mct_event_gravity_vector_enable_t *param =
    (mct_event_gravity_vector_enable_t *)input;
  gyro_port_private_t *private = ((mct_port_t *)port)->port_private;
  dsps_set_data_t dsps_set_data;

  if (private->dsps_handle == NULL) {
    CDBG_ERROR("%s: DSPS not initialized!", __func__);
    return -1;
  }

  if (param->enable) { /* enable gravity vector */
    memset(&dsps_set_data, 0, sizeof(dsps_set_data));
    dsps_set_data.msg_type = DSPS_ENABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_GRAVITY_VECTOR;
    dsps_set_data.u.gravity.report_period = param->report_period;
    dsps_set_data.u.gravity.sample_rate_valid =
      param->sample_rate_valid;
    dsps_set_data.u.gravity.sample_rate = param->sample_rate;
    rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    if (rc) {
      CDBG_ERROR("%s: DSPS enable req failed", __func__);
    }
  } else { /* disable gravity vector */
    memset(&dsps_set_data, 0, sizeof(dsps_set_data));
    dsps_set_data.msg_type = DSPS_DISABLE_REQ;
    dsps_set_data.sensor_type = DSPS_DATA_TYPE_GRAVITY_VECTOR;
    rc = dsps_proc_set_params(private->dsps_handle, &dsps_set_data);
    if (rc) {
      CDBG_ERROR("%s: DSPS enable req failed", __func__);
    }
  }

  return rc;
} /* dsps_port_enable_gravity_vector */

/** gyro_port_event:
 *    @port: gyro port
 *    @event: event
 *
 *  This function handles events for the gyro port.
 *
 *  Returns TRUE on success.
 **/
static boolean gyro_port_event(mct_port_t *port, mct_event_t *event)
{
  boolean rc = TRUE;
  gyro_port_private_t *private;

  /* sanity check */
  if (!port || !event)
    return FALSE;

  private = (gyro_port_private_t *)(port->port_private);
  if (!private)
    return FALSE;

  /* sanity check: ensure event is meant for port with same identity*/
  if ((private->reserved_id & 0xFFFF0000) != (event->identity & 0xFFFF0000))
  {
    return FALSE;
  }

  switch (MCT_EVENT_DIRECTION(event)) {
  case MCT_EVENT_DOWNSTREAM: {
    switch (event->type) {
    case MCT_EVENT_CONTROL_CMD: {
      mct_event_control_t *control = &event->u.ctrl_event;

      CDBG("%s: Control event type %d", __func__, control->type);
      switch (control->type) {
      case MCT_EVENT_CONTROL_STREAMON:
        private->last_time = 0;
        break;

      case MCT_EVENT_CONTROL_STREAMOFF:
        break;

      case MCT_EVENT_CONTROL_SET_PARM: {
        stats_set_params_type *stats_parm = control->control_event_data;

        if (stats_parm->param_type == STATS_SET_IS_PARAM &&
          stats_parm->u.is_param.type == IS_SET_PARAM_IS_ENABLE) {
          rc = dsps_port_handle_set_is_enable(private,
            stats_parm->u.is_param.u.is_enable);
        } else if (stats_parm->param_type == STATS_SET_COMMON_PARAM &&
          stats_parm->u.common_param.type == COMMON_SET_PARAM_STREAM_ON_OFF) {
          stats_common_set_parameter_t *common_param =
             &(stats_parm->u.common_param);

          CDBG_LOW("%s COMMON_SET_PARAM_STREAM_ON_OFF %d", __func__,
            common_param->u.stream_on);

          if (common_param->u.stream_on) {
            /* Enable sensors during first stream on  */
            /*disable during last stream off*/
            CDBG_LOW("Enable Sensors!");
            gyro_port_enable_disable_sensor_request(port, TRUE);
            private->stream_on = TRUE;
          } else {
            CDBG_LOW("Disable Sensors!");
            gyro_port_enable_disable_sensor_request(port, FALSE);
            private->stream_on = FALSE;
          }
        }
      }
        break;

      default:
        break;
      }
    } /* case MCT_EVENT_CONTROL_CMD */
      break;

    case MCT_EVENT_MODULE_EVENT: {
      mct_event_module_t *mod_event = &event->u.module_event;

      CDBG("%s: Module event type %d", __func__, mod_event->type);
      switch (mod_event->type) {
      case MCT_EVENT_MODULE_START_STOP_STATS_THREADS: {
        uint8_t *start_flag = (uint8_t*)(mod_event->module_event_data);
        CDBG_HIGH("%s  MCT_EVENT_MODULE_START_STOP_STATS_THREADS start_flag: %d",
        __func__,*start_flag);
        if (*start_flag) {
          /* Initialize sensor framework for the first time */
          /*Deinit happens during gyro_port deinit */
            if ((NULL == private->dsps_handle) &&
              (NULL == private->and_sens_handle)) {
              CDBG_HIGH("Initializing Motion Sensor Framework!!!");
              gyro_port_init_sensors(port);
            }
        }
      }
        break;

      case MCT_EVENT_MODULE_SOF_NOTIFY:{
        gyro_port_handle_sof_event(port, event, private);
      }
        break;

      case MCT_EVENT_MODULE_STATS_AEC_UPDATE: {
        aec_update_t *aec_update_data;
        int i;

        aec_update_data = (aec_update_t *)mod_event->module_event_data;
        i = aec_update_data->frame_id % BUF_SIZE;
        CDBG("%s: fid = %u, exp time = %.6f", __func__,
          aec_update_data->frame_id, aec_update_data->exp_time);
        private->exposure_time[i] = aec_update_data->exp_time;
      }
        break;

      case MCT_EVENT_MODULE_SET_STREAM_CONFIG: {
        sensor_out_info_t *sensor_info;

        CDBG("%s: Got MCT_EVENT_MODULE_SET_STREAM_CONFIG", __func__);
        sensor_info = (sensor_out_info_t *)mod_event->module_event_data;
        private->frame_time = (unsigned long long)
          ((sensor_info->ll_pck * sensor_info->dim_output.height) /
           (sensor_info->vt_pixel_clk / SEC_TO_USEC));
        private->sensor_position = sensor_info->position;
        CDBG_HIGH("%s:frame_time: %llu sensor_position: %d",__func__,
          private->frame_time, private->sensor_position);
      }
        break;

      case MCT_EVENT_MODULE_SET_CHROMATIX_PTR: {
        /* Get values from chromatix structure when IS parameters are available
           with 305 header.  For now, hard-code with default values */
        private->rs_interval_offset = 10000;
        private->s3d_interval_offset = 10000;
      }
        break;

      case MCT_EVENT_MODULE_GRAVITY_VECTOR_ENABLE: {
        CDBG("%s: Got MCT_EVENT_MODULE_GRAVITY_VECTOR_ENABLE", __func__);
        rc = dsps_port_enable_gravity_vector(port,
          mod_event->module_event_data);
      }
        break;
      default:
        break;
      }
    } /* case MCT_EVENT_MODULE_EVENT */
      break;

    default:
      break;
    } /* switch (event->type) */

  } /* case MCT_EVENT_TYPE_DOWNSTREAM */
      break;

  default:
    break;
  } /* switch (MCT_EVENT_DIRECTION(event)) */

  return rc;
}


/** gyro_port_ext_link:
 *    @identity: session id | stream id
 *    @port: gyro port
 *    @peer: For gyro sink port, peer is most likely stats port
 *
 *  Sets gyro port's external peer port.
 *
 *  Returns TRUE on success.
 **/
static boolean gyro_port_ext_link(unsigned int identity,
  mct_port_t *port, mct_port_t *peer)
{
  boolean rc = FALSE;
  gyro_port_private_t *private;
  mct_event_t event;

  CDBG("%s: Enter", __func__);
  if (strcmp(MCT_OBJECT_NAME(port), "gyro_sink"))
    return FALSE;

  private = (gyro_port_private_t *)port->port_private;
  if (!private)
    return FALSE;

  MCT_OBJECT_LOCK(port);
  switch (private->state) {
  case GYRO_PORT_STATE_RESERVED:
  case GYRO_PORT_STATE_UNLINKED:
    if ((private->reserved_id & 0xFFFF0000) != (identity & 0xFFFF0000)) {
      break;
    }
  case GYRO_PORT_STATE_CREATED:
    /* If gyro module requires a thread, indicate here. */
    rc = TRUE;
    break;

  case GYRO_PORT_STATE_LINKED:
    if ((private->reserved_id & 0xFFFF0000) == (identity & 0xFFFF0000)) {
      rc = TRUE;
    }
    break;

  default:
    break;
  }

  if (rc == TRUE) {
    /* If gyro module requires a thread and the port state above warrants one,
       create the thread here */
    private->state = GYRO_PORT_STATE_LINKED;
    MCT_PORT_PEER(port) = peer;
    MCT_OBJECT_REFCOUNT(port) += 1;
  }

  MCT_OBJECT_UNLOCK(port);
  CDBG("%s: rc=%d", __func__, rc);
  return rc;
}


/** gyro_port_unlink:
 *  @identity: session id | stream id
 *  @port: gyro port
 *  @peer: gyro port's peer port (probably stats port)
 *
 *  This funtion unlinks the gyro port from its peer.
 **/
static void gyro_port_unlink(unsigned int identity,
  mct_port_t *port, mct_port_t *peer)
{
  gyro_port_private_t *private;

  CDBG_HIGH("%s: Enter", __func__);
  if (!port || !peer || MCT_PORT_PEER(port) != peer)
    return;

  private = (gyro_port_private_t *)port->port_private;
  if (!private)
    return;

  MCT_OBJECT_LOCK(port);
  if (private->state == GYRO_PORT_STATE_LINKED &&
      (private->reserved_id & 0xFFFF0000) == (identity & 0xFFFF0000)) {
    MCT_OBJECT_REFCOUNT(port) -= 1;
    if (!MCT_OBJECT_REFCOUNT(port))
      private->state = GYRO_PORT_STATE_UNLINKED;
  }
  MCT_OBJECT_UNLOCK(port);

  CDBG_HIGH("%s: Exit", __func__);
  return;
}


/** gyro_port_set_caps:
 *    @port: port object whose caps are to be set
 *    @caps: this port's capability.
 *
 *  Function overwrites a ports capability.
 *
 *  Returns TRUE if it is valid source port.
 **/
static boolean gyro_port_set_caps(mct_port_t *port,
  mct_port_caps_t *caps)
{
  CDBG("%s: Enter", __func__);
  if (strcmp(MCT_PORT_NAME(port), "gyro_sink"))
    return FALSE;

  port->caps = *caps;
  return TRUE;
}


/** gyro_port_check_caps_reserve:
 *    @port: this interface module's port
 *    @peer_caps: the capability of peer port which wants to match
 *                interface port
 *    @stream_info: stream information
 *
 *  Returns TRUE on success.
 **/
static boolean gyro_port_check_caps_reserve(mct_port_t *port, void *caps,
  void *stream_info)
{
  boolean rc = FALSE;
  mct_port_caps_t *port_caps;
  gyro_port_private_t *private;
  mct_stream_info_t *strm_info = (mct_stream_info_t *)stream_info;

  CDBG("%s: Enter", __func__);
  MCT_OBJECT_LOCK(port);
  if (!port || !caps || !strm_info ||
      strcmp(MCT_OBJECT_NAME(port), "gyro_sink")) {
    CDBG("%s: Exit unsucessful", __func__);
    goto reserve_done;
  }

  port_caps = (mct_port_caps_t *)caps;
  /* Change later to SOF? */
  if (port_caps->port_caps_type != MCT_PORT_CAPS_STATS) {
    rc = FALSE;
    goto reserve_done;
  }

  private = (gyro_port_private_t *)port->port_private;
  if (!private){
      rc = FALSE;
      goto reserve_done;
  }

  switch (private->state) {
  case GYRO_PORT_STATE_LINKED:
  if ((private->reserved_id & 0xFFFF0000) ==
      (strm_info->identity & 0xFFFF0000)) {
    rc = TRUE;
  }
  private->is_type = strm_info->is_type;
  break;

  case GYRO_PORT_STATE_CREATED:
  case GYRO_PORT_STATE_UNRESERVED:
    private->reserved_id = strm_info->identity;
    private->state = GYRO_PORT_STATE_RESERVED;
    rc = TRUE;
    break;

  case GYRO_PORT_STATE_RESERVED:
    if ((private->reserved_id & 0xFFFF0000) ==
        (strm_info->identity & 0xFFFF0000))
      rc = TRUE;
    break;

  default:
    break;
  }

reserve_done:
  MCT_OBJECT_UNLOCK(port);
  return rc;
}


/** gyro_port_check_caps_unreserve:
 *    @port: this port object to remove the session/stream
 *    @identity: session+stream identity
 *
 *  This function frees the identity from port's children list.
 *
 *  Returns FALSE if the identity does not exist.
 **/
static boolean gyro_port_check_caps_unreserve(mct_port_t *port,
  unsigned int identity)
{
  boolean rc = FALSE;
  gyro_port_private_t *private;

  if (!port || strcmp(MCT_OBJECT_NAME(port), "gyro_sink"))
    return FALSE;

  private = (gyro_port_private_t *)port->port_private;
  if (!private)
    return FALSE;

  if (private->state == GYRO_PORT_STATE_UNRESERVED)
    return TRUE;

  MCT_OBJECT_LOCK(port);
  if ((private->state == GYRO_PORT_STATE_UNLINKED ||
       private->state == GYRO_PORT_STATE_RESERVED) &&
      ((private->reserved_id & 0xFFFF0000) == (identity & 0xFFFF0000))) {

    if (!MCT_OBJECT_REFCOUNT(port)) {
      private->state = GYRO_PORT_STATE_UNRESERVED;
      private->reserved_id = (private->reserved_id & 0xFFFF0000);
    }
    rc = TRUE;
  }

unreserve_done:
  MCT_OBJECT_UNLOCK(port);
  return rc;
}


/** gyro_port_init:
 *    @port: gyro port
 *    @session_id: session id
 *    @dsps_handle: DSPS handle
 *
 *  This function initializes the gyro port's internal variables.
 *
 *  Returns TRUE on success.
 **/
boolean gyro_port_init(mct_port_t *port, unsigned int session_id)
{
  mct_port_caps_t caps;
  gyro_port_private_t *private;
  int rc = 0;
  CDBG_HIGH( "%s , Enter ",__func__);

  if (port == NULL || strcmp(MCT_OBJECT_NAME(port), "gyro_sink")) {
    return FALSE;
  }

  private = (void *)calloc(1, sizeof(gyro_port_private_t));
  if (!private) {
    return FALSE;
  }
  private->reserved_id = session_id;
  private->state = GYRO_PORT_STATE_CREATED;
  private->gyro_sample_rate = GYRO_SAMPLE_RATE_AF;
  port->port_private = private;
  port->direction = MCT_PORT_SINK;
  caps.port_caps_type = MCT_PORT_CAPS_STATS;
  caps.u.stats.flag = MCT_PORT_CAP_STATS_GYRO;

  mct_port_set_event_func(port, gyro_port_event);
  CDBG("%s: gyro_port_event addr = %p", __func__, gyro_port_event);
  /* Accept default int_link function */
  mct_port_set_ext_link_func(port, gyro_port_ext_link);
  mct_port_set_unlink_func(port, gyro_port_unlink);
  mct_port_set_set_caps_func(port, gyro_port_set_caps);
  mct_port_set_check_caps_reserve_func(port, gyro_port_check_caps_reserve);
  mct_port_set_check_caps_unreserve_func(port, gyro_port_check_caps_unreserve);

  if (port->set_caps) {
    port->set_caps(port, &caps);
  }
  rc = TRUE;
  CDBG_HIGH( "%s , EXIT ",__func__);

  return rc;
}


/** gyro_port_deinit:
 *    @port: gyro port
 *
 * This function frees the gyro port's memory.
 **/
void gyro_port_deinit(mct_port_t *port)
{
  CDBG_HIGH( "%s , Enter ",__func__);
  gyro_port_private_t *private;
  if (!port || strcmp(MCT_OBJECT_NAME(port), "gyro_sink")) {
    return;
  }
  CDBG_HIGH("%s:Deinit Gyro port",__func__);
  private = port->port_private;
  /*deinit Android sensorService here */
  if (private->and_sens_handle) {
    sensor_intf_deinit(private->and_sens_handle);
    CDBG("%s : Deinited Android sensor_Service", __func__);
    private->and_sens_handle = NULL;
  } else {
    CDBG_ERROR("%s: Error in android Deinit", __func__);
  }
  /* deinitialize the laser sensor */
  if (NULL != private->laser_sns_hndl) {
    CDBG_HIGH("%s:Deinit Laser Sensor Module!",__func__);
    sensor_laser_deinit(private->laser_sns_hndl);
    private->laser_sns_hndl = NULL;
  }

  if (port->port_private) {
    free(port->port_private);
    port->port_private = NULL;
  }
  CDBG_HIGH( "%s , EXIT ",__func__);
}


/** gyro_port_find_identity:
 *    @port: gyro port
 *    @identity: session id | stream id
 *
 * This function checks for the port with a given session.
 *
 * Returns TRUE if the port is found.
 **/
boolean gyro_port_find_identity(mct_port_t *port, unsigned int identity)
{
  gyro_port_private_t *private;

  if (!port || strcmp(MCT_OBJECT_NAME(port), "gyro_sink"))
    return TRUE;

  private = port->port_private;

  if (private) {
    return ((private->reserved_id & 0xFFFF0000) == (identity & 0xFFFF0000) ?
            TRUE : FALSE);
  }

  return FALSE;
}

