/*============================================================================
   Copyright (c) 2012 - 2015 Qualcomm Technologies, Inc. All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.

   This file defines the media/module/master controller's interface with the
   DSPS modules. The functionalities od this module include:

   1. Control communication with the sensor module
   2. Process data received from the sensors

============================================================================*/
#include "dsps_hw.h"
#ifdef FEATURE_GYRO_DSPS
#include <sensor1.h>
#include <sns_smgr_api_v01.h>
#include <sns_sam_gyroint_v01.h>
#include <sns_time_api_v02.h>
#include <sns_reg_api_v02.h>

void dsps_sensor1_callback(intptr_t *data, sensor1_msg_header_s *msg_hdr,
  sensor1_msg_type_e msg_type, void *msg_ptr);
#endif


/** dsps_get_gyro_samples:
 *    @data: gyro data from sensor
 *    @dsps_output: memory to store the gyro data
 *
 * This function transfers gyro data from sensor to camera.
 **/
#ifdef FEATURE_GYRO_DSPS
void dsps_get_gyro_samples(void *data, dsps_cb_data_t *cb_data)
{
  uint8_t i, idx;
  sns_sam_gyroint_report_ind_msg_v01 *sensor_data =
      (sns_sam_gyroint_report_ind_msg_v01 *) data;

  idx = 0;
  if (sensor_data->frame_info_valid) {
    uint32_t sample_len;

    sample_len = MIN(sensor_data->frame_info.sample_len,
      SNS_SAM_GYROINT_MAX_BUFSIZE_V01);
    /* Verify size match between sensor and camera callback API */
    if (sizeof(cb_data->u.gyro.sample) <
      sizeof(sensor_data->frame_info.sample)) {
      CDBG_ERROR("error: sensor sample, bigger than camera API");
      cb_data->u.gyro.sample_len = 0;
      return;
    }
    cb_data->u.gyro.sample_len = sample_len;

    for (i = 0; i < sample_len; i++) {
      cb_data->u.gyro.sample[idx].timestamp =
        sensor_data->frame_info.sample[i].timestamp;
      cb_data->u.gyro.sample[idx].value[0] =
        sensor_data->frame_info.sample[i].value[0];
      cb_data->u.gyro.sample[idx].value[1] =
        sensor_data->frame_info.sample[i].value[1];
      cb_data->u.gyro.sample[idx].value[2] =
        sensor_data->frame_info.sample[i].value[2];
      idx++;
    }

    if (sensor_data->frame_info2_valid)
    {
      sample_len = MIN(sensor_data->frame_info2.sample_len,
        SNS_SAM_GYROINT_MAX_BUFSIZE_V01);
      for (i = 0; i < sample_len; i++) {
        cb_data->u.gyro.sample[idx].timestamp =
          sensor_data->frame_info2.sample[i].timestamp;
        cb_data->u.gyro.sample[idx].value[0] =
          sensor_data->frame_info2.sample[i].value[0];
        cb_data->u.gyro.sample[idx].value[1] =
          sensor_data->frame_info2.sample[i].value[1];
        cb_data->u.gyro.sample[idx].value[2] =
          sensor_data->frame_info2.sample[i].value[2];
        idx++;
      }
      cb_data->u.gyro.sample_len += sample_len;
    }
  }
}
#endif

/*===========================================================================
 * FUNCTION      dsps_close
 *
 * DESCRIPTION   Close a connection with the sensor framework
 *==========================================================================*/
int dsps_close(sensor1_config_t *dsps_config)
{
  if (dsps_config == NULL)
    return -1;
#ifdef FEATURE_GYRO_DSPS
  if (sensor1_close(dsps_config->handle) != SENSOR1_SUCCESS)
    return -1;
#else
  return -1;
#endif
  return 0;
}


/*===========================================================================
 * FUNCTION      dsps_disconnect
 *
 * DESCRIPTION   Deregister an mctl client with the DSPS Thread
 *=========================================================================*/
int dsps_disconnect(void * sensor_config)
{
  int rc = 0;
  sensor1_config_t * dsps_config = (sensor1_config_t *)sensor_config;

  if (dsps_close(dsps_config) < 0) {
    CDBG_ERROR("%s Error in closing sensor connection", __func__);
    rc = -1;
  }
  pthread_mutex_destroy(&(dsps_config->callback_mutex));
  pthread_cond_destroy(&(dsps_config->callback_condvar));
  pthread_mutex_destroy(&(dsps_config->thread_mutex));
  pthread_cond_destroy(&(dsps_config->thread_condvar));

  return rc;
}

/*===========================================================================
 * FUNCTION      dsps_open
 *
 * DESCRIPTION   Open a new connection with the sensor framework
 *==========================================================================*/
int dsps_open(void *sensor_config)
{
  sensor1_config_t *dsps_config = (sensor1_config_t *)sensor_config;
#ifdef FEATURE_GYRO_DSPS
  /* Open sensor1 port */
  CDBG("try to sensor1_open()");
  if (sensor1_open(&dsps_config->handle,
      (sensor1_notify_data_cb_t)&dsps_sensor1_callback,
      (intptr_t)dsps_config) == SENSOR1_SUCCESS)
    return 0;
#endif
  return -1;
}

/*===========================================================================
 * FUNCTION      dsps_set_expiry_time
 *
 * DESCRIPTION   Set the expiry time for timed wait by adding timeout
 *               value to current time.
 *==========================================================================*/
void dsps_set_expiry_time(struct timespec *expiry_time)
{
  struct timeval current_time;

  gettimeofday(&current_time, NULL);
  expiry_time->tv_sec = current_time.tv_sec;
  expiry_time->tv_nsec = current_time.tv_usec * USEC_TO_NSEC;
  expiry_time->tv_sec += SENSOR_TIME_OUT * MSEC_TO_SEC;
  expiry_time->tv_sec += (expiry_time->tv_nsec + (SENSOR_TIME_OUT % SEC_TO_MSEC)
    * MSEC_TO_NSEC) / SEC_TO_NSEC;
  expiry_time->tv_nsec += (SENSOR_TIME_OUT % SEC_TO_MSEC) * MSEC_TO_NSEC;
  expiry_time->tv_nsec %= SEC_TO_NSEC;
}

/*===========================================================================
 * FUNCTION      dsps_wait_for_response
 *
 * DESCRIPTION   Wait for response from sensor until timer expires.
 *               Condition variable here is signaled from
 *               dsps_process_response()
 *==========================================================================*/
int dsps_wait_for_response(sensor1_config_t *dsps_config)
{
  int ret;
  struct timespec expiry_time;

  dsps_set_expiry_time(&expiry_time);

  pthread_mutex_lock(&(dsps_config->callback_mutex));

  /* Check if callback has already arrived */
  if (dsps_config->callback_arrived == 1) {
    CDBG("%s: Callback received before wait\n", __func__);
    ret = 0;
    goto end;
  }

  /* Timed wait for callback */
  ret = pthread_cond_timedwait(&(dsps_config->callback_condvar),
    &(dsps_config->callback_mutex), &expiry_time);

  if ((!ret) && (dsps_config->callback_arrived == 0)) {
    CDBG_ERROR("Error! Timed wait returned without callback.\n");
    ret = -1;
  }

end:
  pthread_mutex_unlock(&(dsps_config->callback_mutex));
  return ret;
}

/*===========================================================================
 * FUNCTION      dsps_prepare_req_header
 *
 * DESCRIPTION   Prepare header for a request message
 *==========================================================================*/
#ifdef FEATURE_GYRO_DSPS
static void dsps_prepare_req_header_gyro(sensor1_msg_header_s *req_hdr,
    sensor1_req_data_t *msg_data)
{
  /* Prepare Message Header */
  switch (msg_data->msg_type) {
    case DSPS_ENABLE_REQ:
      req_hdr->service_number = SNS_SAM_GYROINT_SVC_ID_V01;
      req_hdr->msg_id = SNS_SAM_GYROINT_ENABLE_REQ_V01;
      req_hdr->msg_size = sizeof(sns_sam_gyroint_enable_req_msg_v01);
      req_hdr->txn_id = 0;
      break;
    case DSPS_DISABLE_REQ:
      req_hdr->service_number = SNS_SAM_GYROINT_SVC_ID_V01;
      req_hdr->msg_id = SNS_SAM_GYROINT_DISABLE_REQ_V01;
      req_hdr->msg_size = sizeof(sns_sam_gyroint_disable_req_msg_v01);
      req_hdr->txn_id = 0;
      break;
    case DSPS_GET_REPORT:
      req_hdr->service_number = SNS_SAM_GYROINT_SVC_ID_V01;
      req_hdr->msg_id = SNS_SAM_GYROINT_GET_REPORT_REQ_V01;
      req_hdr->msg_size = sizeof(sns_sam_gyroint_get_report_req_msg_v01);
      req_hdr->txn_id = msg_data->u.gyro.seqnum;
      break;
    default:
      CDBG_ERROR("%s Invalid type", __func__);
  }
}
#endif


/*===========================================================================
 * FUNCTION      dsps_prepare_req_header
 *
 * DESCRIPTION   Prepare header for a request message
 *==========================================================================*/
#ifdef FEATURE_GYRO_DSPS
static void dsps_prepare_req_header_gravity(sensor1_msg_header_s *req_hdr,
    sensor1_req_data_t *msg_data)
{
  /* Prepare Message Header */
  switch (msg_data->msg_type) {
    case DSPS_ENABLE_REQ:
      req_hdr->service_number = SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01;
      req_hdr->msg_id = SNS_SAM_GRAVITY_ENABLE_REQ_V01;
      req_hdr->msg_size = sizeof(sns_sam_gravity_enable_req_msg_v01);
      req_hdr->txn_id = 0;
      break;
    case DSPS_DISABLE_REQ:
      req_hdr->service_number = SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01;
      req_hdr->msg_id = SNS_SAM_GRAVITY_DISABLE_REQ_V01;
      req_hdr->msg_size = sizeof(sns_sam_gravity_disable_req_msg_v01);
      req_hdr->txn_id = 0;
      break;
    case DSPS_GET_REPORT:
      req_hdr->service_number = SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01;
      req_hdr->msg_id = SNS_SAM_GRAVITY_GET_REPORT_REQ_V01;
      req_hdr->msg_size = sizeof(sns_sam_gravity_get_report_req_msg_v01);
      req_hdr->txn_id = 0;
      break;
    default:
      CDBG_ERROR("%s Invalid type", __func__);
  }
}
#endif

/*===========================================================================
 * FUNCTION      dsps_prepare_req_header
 *
 * DESCRIPTION   Prepare header for a request message
 *==========================================================================*/
#ifdef FEATURE_GYRO_DSPS
static void dsps_prepare_req_header(sensor1_msg_header_s *req_hdr,
    sensor1_req_data_t *msg_data)
{
  int rc = 0;
  switch (msg_data->sensor_type) {
  case DSPS_DATA_TYPE_GYRO:
    dsps_prepare_req_header_gyro(req_hdr, msg_data);
    break;
  case DSPS_DATA_TYPE_GRAVITY_VECTOR:
    dsps_prepare_req_header_gravity(req_hdr, msg_data);
    break;
  default:
    CDBG_ERROR("%s: Sensor type %d not supported yet!", __func__,
      msg_data->sensor_type);
    rc = -1;
    break;
  }
}
#endif


/*===========================================================================
 * FUNCTION      dsps_prepare_req_msg_gyro
 *
 * DESCRIPTION   Prepare body of a request message
 *==========================================================================*/
#ifdef FEATURE_GYRO_DSPS
static void *dsps_prepare_req_msg_gyro(sensor1_config_t *dsps_config,
  sensor1_req_data_t *msg_data)
{
  int size = 0;
  sensor1_error_e error;
  void *req_msg = NULL;

  CDBG("%s: Prepare Request message of type : %d", __func__,
    msg_data->msg_type);

  switch (msg_data->msg_type) {
  case DSPS_ENABLE_REQ: {
    sns_sam_gyroint_enable_req_msg_v01 *enable_req_msg;
    size = sizeof(sns_sam_gyroint_enable_req_msg_v01);
    error = sensor1_alloc_msg_buf(dsps_config->handle, size, &req_msg);
    if (error != SENSOR1_SUCCESS) {
      CDBG_ERROR("%s: DSPS_ENABLE_REQ: Error allocating buffer %d\n", __func__,
        error);
      return NULL;
    }
    enable_req_msg = (sns_sam_gyroint_enable_req_msg_v01 *)req_msg;
    enable_req_msg->sample_rate = msg_data->u.gyro.gyro_sample_rate << 16;
    enable_req_msg->extra_sample = 1;
    enable_req_msg->enable_angle = 0;
    enable_req_msg->enable_sample = 1;
  }
    break;

  case DSPS_DISABLE_REQ: {
    sns_sam_gyroint_disable_req_msg_v01 *disable_req_msg;
    size = sizeof(sns_sam_gyroint_disable_req_msg_v01);
    error = sensor1_alloc_msg_buf(dsps_config->handle, size, &req_msg);
    if (error != SENSOR1_SUCCESS) {
      CDBG_ERROR("%s: DSPS_DISABLE_REQ: Error allocating buffer %d\n",
        __func__, error);
      return NULL;
    }
   disable_req_msg = (sns_sam_gyroint_disable_req_msg_v01 *)req_msg;
   disable_req_msg->instance_id = dsps_config->instance_id;
  }
    break;

  case DSPS_GET_REPORT: {
    sns_sam_gyroint_get_report_req_msg_v01 *get_report_req_msg;
    size = sizeof(sns_sam_gyroint_get_report_req_msg_v01);
    error = sensor1_alloc_msg_buf(dsps_config->handle, size, &req_msg);
    if (error != SENSOR1_SUCCESS) {
      CDBG_ERROR("%s: DSPS_GET_REPORT: Error allocating buffer %d\n", __func__,
        error);
      return NULL;
    }
    get_report_req_msg = (sns_sam_gyroint_get_report_req_msg_v01 *)req_msg;
    get_report_req_msg->instance_id = dsps_config->instance_id;
    get_report_req_msg->seqnum = msg_data->u.gyro.seqnum;
    /* Use microseconds */
    get_report_req_msg->t_start = msg_data->u.gyro.t_start;
    get_report_req_msg->t_end = msg_data->u.gyro.t_end;
    get_report_req_msg->frame_info_valid = 0;
  }
    break;

  default:
    CDBG_ERROR("%s Invalid type", __func__);
    return NULL;
  }

  return req_msg;
} /* dsps_prepare_req_msg_gyro */
#endif

/*===========================================================================
 * FUNCTION      dsps_prepare_req_msg_gravity
 *
 * DESCRIPTION   Prepare body of a request message
 *==========================================================================*/
#ifdef FEATURE_GYRO_DSPS
static void *dsps_prepare_req_msg_gravity(sensor1_config_t *dsps_config,
  sensor1_req_data_t *msg_data)
{
  int size = 0;
  int rc = 0;
  sensor1_error_e error;
  void *req_msg = NULL;

  CDBG("%s: Prepare Request message of type : %d", __func__,
    msg_data->msg_type);

  switch (msg_data->msg_type) {
  case DSPS_ENABLE_REQ: {
    sns_sam_gravity_enable_req_msg_v01 *enable_req_msg;
    size = sizeof(sns_sam_gravity_enable_req_msg_v01);
    error = sensor1_alloc_msg_buf(dsps_config->handle, size, &req_msg);
    if (error != SENSOR1_SUCCESS) {
      CDBG_ERROR("%s: DSPS_ENABLE_REQ: Error allocating buffer %d\n", __func__,
        error);
      return NULL;
    }

    /* Gravity vector enable request message has following fields:
     * Mandatory -
     * report_period: ouput rate - units of seconds, Q16. 0 to report
     *   at sampling rate.
     * Optional -
     * sample_rate_valid: true if sample rate to be passed.
     * sample_rate: in Hz, Q16. If less than report rate, set to report rate
     * notify_suspend_valid: true if notify_suspend is being passed.
     * notify_suspend: send indicaiton for the request when the processor
     *   is in suspend state.
     **/
    enable_req_msg = (sns_sam_gravity_enable_req_msg_v01 *)req_msg;
    enable_req_msg->report_period = msg_data->u.gravity.report_period;
    if (msg_data->u.gravity.sample_rate_valid) {
      enable_req_msg->sample_rate_valid = TRUE;
      enable_req_msg->sample_rate = msg_data->u.gravity.sample_rate;
    }
  }
    break;

  case DSPS_DISABLE_REQ: {
    sns_sam_gravity_disable_req_msg_v01 *disable_req_msg;
    size = sizeof(sns_sam_gravity_disable_req_msg_v01);
    error = sensor1_alloc_msg_buf(dsps_config->handle, size, &req_msg);
    if (error != SENSOR1_SUCCESS) {
      CDBG_ERROR("%s: DSPS_DISABLE_REQ: Error allocating buffer %d\n",
        __func__, error);
      return NULL;
    }
    disable_req_msg = (sns_sam_gravity_disable_req_msg_v01 *)req_msg;
    disable_req_msg->instance_id = dsps_config->instance_id_gravity;
  }
    break;

  case DSPS_GET_REPORT: {
    sns_sam_gravity_get_report_req_msg_v01 *get_report_req_msg;
    size = sizeof(sns_sam_gravity_get_report_req_msg_v01);
    error = sensor1_alloc_msg_buf(dsps_config->handle, size, &req_msg);
    if (error != SENSOR1_SUCCESS) {
      CDBG_ERROR("%s: DSPS_GET_REPORT: Error allocating buffer %d\n", __func__,
        error);
      return NULL;
    }

    get_report_req_msg = (sns_sam_gravity_get_report_req_msg_v01 *)req_msg;
    get_report_req_msg->instance_id = dsps_config->instance_id_gravity;
  }
    break;

  default:
    CDBG_ERROR("%s Invalid type", __func__);
    return NULL;
  }

  return req_msg;
} /* dsps_prepare_req_msg_gravity */
#endif

/*===========================================================================
 * FUNCTION      dsps_prepare_req_msg
 *
 * DESCRIPTION   Prepare body of a request message
 *==========================================================================*/
#ifdef FEATURE_GYRO_DSPS
void *dsps_prepare_req_msg(sensor1_config_t *dsps_config,
    sensor1_req_data_t *msg_data)
{
  int size = 0;
  void *req_msg = NULL;

  switch (msg_data->sensor_type) {
  case DSPS_DATA_TYPE_GYRO:
    req_msg = dsps_prepare_req_msg_gyro(dsps_config, msg_data);
    break;
  case DSPS_DATA_TYPE_GRAVITY_VECTOR:
    req_msg = dsps_prepare_req_msg_gravity(dsps_config, msg_data);
    break;
  default:
    CDBG_ERROR("%s: Sensor type %d not supported yet!", __func__,
      msg_data->sensor_type);
    break;
  }

  return req_msg;
} /* dsps_prepare_req_msg_gravity */
#endif


/*===========================================================================
 * FUNCTION      dsps_send_request
 *
 * DESCRIPTION   Send a request message to the sensor framework.
 *               Typically used for adding and deleting reports
 *==========================================================================*/
#ifdef FEATURE_GYRO_DSPS
int dsps_send_request(void *sensor_config,
  void *req_data, int wait)
{
  sensor1_error_e error;
  sensor1_msg_header_s req_hdr;
  void *req_msg = NULL;
  sensor1_config_t *dsps_config = (sensor1_config_t *)sensor_config;
  sensor1_req_data_t *msg_data = (sensor1_req_data_t *)req_data;


  req_msg = dsps_prepare_req_msg(dsps_config, msg_data);
  if (req_msg == NULL) {
    CDBG_ERROR("%s: Error preparing request message!", __func__);
    return -1;
  }

  dsps_prepare_req_header(&req_hdr, msg_data);

  dsps_config->error = 0;
  dsps_config->callback_arrived = 0;

  error = sensor1_write(dsps_config->handle, &req_hdr, req_msg);
  if (error != SENSOR1_SUCCESS) {
    CDBG_ERROR("%s: Error writing request message\n", __func__);
    sensor1_free_msg_buf(dsps_config->handle, req_msg);
    return -1;
  }

  if (wait) {
    /* Wait for a response */
    if (dsps_wait_for_response(dsps_config) != 0) {
      CDBG_ERROR("%s: Request response timed out\n", __func__);
      return -1;
    }

    if (dsps_config->error)
      return -1;
  }

  return 0;
}
#endif

/*===========================================================================
 * FUNCTION      dsps_handle_broken_pipe
 *
 * DESCRIPTION   Handle error condition of broken pipe with the sensor
 *               framework
 *==========================================================================*/
void dsps_handle_broken_pipe(sensor1_config_t *dsps_config)
{
  CDBG_ERROR("%s: Broken Pipe Exception\n", __func__);
  pthread_mutex_lock(&(dsps_config->thread_mutex));
  dsps_config->status = DSPS_BROKEN_PIPE;
  pthread_mutex_unlock(&(dsps_config->thread_mutex));
  pthread_cond_signal(&(dsps_config->thread_condvar));
}

/*===========================================================================
 * FUNCTION      dsps_process_repsonse_gyro
 *
 * DESCRIPTION   Process response received from sensor framework.
 *               A response message is in response to a message sent to the
 *               sensor framework. Signal waiting condition variable in
 *               dsps_wait_for_response()
 *=========================================================================*/
#ifdef FEATURE_GYRO_DSPS
static void dsps_process_response_gyro(sensor1_config_t *dsps_config,
    sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
  sns_sam_gyroint_enable_resp_msg_v01 *enable_resp_msg;
  sns_sam_gyroint_disable_resp_msg_v01 *disable_resp_msg;
  sns_sam_gyroint_get_report_resp_msg_v01 *get_report_resp_msg;

  switch (msg_hdr->msg_id) {
    case SNS_SAM_GYROINT_ENABLE_RESP_V01:
      enable_resp_msg = (sns_sam_gyroint_enable_resp_msg_v01*)msg_ptr;
      if (enable_resp_msg->instance_id_valid)
        dsps_config->instance_id = enable_resp_msg->instance_id;
      break;
    case SNS_SAM_GYROINT_DISABLE_RESP_V01:
      disable_resp_msg = (sns_sam_gyroint_disable_resp_msg_v01*)msg_ptr;
      if (disable_resp_msg->instance_id_valid)
        dsps_config->instance_id = INVALID_INSTANCE_ID;
      break;
    case SNS_SAM_GYROINT_GET_REPORT_RESP_V01:
      get_report_resp_msg = (sns_sam_gyroint_get_report_resp_msg_v01 *)msg_ptr;
      if (get_report_resp_msg->resp.sns_result_t == SNS_RESULT_SUCCESS_V01) {
        CDBG("%s: Report Request Accepted\n", __func__);
      } else {
        CDBG_ERROR("%s: Report Request Denied\n", __func__);
      }
      break;
    default:
      CDBG_ERROR("%s Response not valid",__func__);
      break;
  }
  pthread_mutex_lock(&(dsps_config->callback_mutex));
  dsps_config->callback_arrived = 1;
  pthread_mutex_unlock(&(dsps_config->callback_mutex));
  pthread_cond_signal(&(dsps_config->callback_condvar));
} /* dsps_process_response_gyro */
#endif

/*===========================================================================
 * FUNCTION      dsps_process_response_gravity
 *
 * DESCRIPTION   Process response received from sensor framework.
 *               A response message is in response to a message sent to the
 *               sensor framework. Signal waiting condition variable in
 *               dsps_wait_for_response()
 *=========================================================================*/
#ifdef FEATURE_GYRO_DSPS
static void dsps_process_response_gravity(sensor1_config_t *dsps_config,
    sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
  sns_sam_gravity_enable_resp_msg_v01 *enable_resp_msg;
  sns_sam_gravity_disable_resp_msg_v01 *disable_resp_msg;
  sns_sam_gravity_get_report_resp_msg_v01 *get_report_resp_msg;

  switch (msg_hdr->msg_id) {
    case SNS_SAM_GRAVITY_ENABLE_RESP_V01:
      enable_resp_msg = (sns_sam_gravity_enable_resp_msg_v01*)msg_ptr;
      if (enable_resp_msg->resp.sns_result_t == SNS_RESULT_SUCCESS_V01) {
        if (enable_resp_msg->instance_id_valid) {
          dsps_config->instance_id_gravity = enable_resp_msg->instance_id;
          CDBG("%s: Ensable Response Instance ID received: %d",
            __func__, dsps_config->instance_id_gravity);
        }
      } else {
        CDBG_ERROR("%s: Enable Request failed (err: %d)", __func__,
          enable_resp_msg->resp.sns_err_t);
      }
      break;
    case SNS_SAM_GRAVITY_DISABLE_RESP_V01:
      disable_resp_msg = (sns_sam_gravity_disable_resp_msg_v01*)msg_ptr;
      if (disable_resp_msg->resp.sns_result_t == SNS_RESULT_SUCCESS_V01) {
        if (disable_resp_msg->instance_id_valid)
          dsps_config->instance_id_gravity = INVALID_INSTANCE_ID;
        CDBG("%s: Disable Response Instance ID received: %d",
          __func__, dsps_config->instance_id_gravity);
      } else {
        CDBG_ERROR("%s: Disable Request failed (err: %d)", __func__,
          disable_resp_msg->resp.sns_err_t);
      }
      break;
    case SNS_SAM_GRAVITY_GET_REPORT_RESP_V01:
      get_report_resp_msg = (sns_sam_gravity_get_report_resp_msg_v01 *)msg_ptr;
      if (get_report_resp_msg->resp.sns_result_t == SNS_RESULT_SUCCESS_V01) {
        CDBG("%s: Report Request Accepted\n", __func__);
      } else {
        CDBG_ERROR("%s: Report Request Denied\n", __func__);
      }
      break;
    default:
      CDBG_ERROR("%s Response not valid",__func__);
      break;
  }
  pthread_mutex_lock(&(dsps_config->callback_mutex));
  dsps_config->callback_arrived = 1;
  pthread_mutex_unlock(&(dsps_config->callback_mutex));
  pthread_cond_signal(&(dsps_config->callback_condvar));
} /* dsps_process_response_gravity */
#endif

/*===========================================================================
 * FUNCTION      dsps_process_indication_gyro
 *
 * DESCRIPTION   Process indication received from sensor framework.
 *=========================================================================*/
#ifdef FEATURE_GYRO_DSPS
static void dsps_process_indication_gyro(sensor1_config_t *dsps_config,
    sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
  sns_sam_gyroint_report_ind_msg_v01 *indication =
      (sns_sam_gyroint_report_ind_msg_v01 *) msg_ptr;

  switch(msg_hdr->msg_id) {
    case SNS_SAM_GYROINT_REPORT_IND_V01:
      if (!indication->frame_info_valid) {
        CDBG_ERROR("%s: Invalid Indication Frame\n", __func__);
        break;
      }

      if (indication->frame_info_valid) {
        dsps_cb_data_t cb_data;
		cb_data.type = DSPS_DATA_TYPE_GYRO;
		dsps_get_gyro_samples(indication, &cb_data);
		cb_data.u.gyro.seq_no = indication->seqnum;
        dsps_config->dsps_callback(dsps_config->port, &cb_data);
      }
      else
        CDBG_ERROR("%s: Invalid Frame Info\n", __func__);
      break;
    default:
      CDBG_ERROR("%s: Invalid Indication ID\n", __func__);
      break;
  }
} /* dsps_process_response_gyro */
#endif

/*===========================================================================
 * FUNCTION      dsps_process_indication_gravity
 *
 * DESCRIPTION   Process indication received from sensor framework.
 *=========================================================================*/
#ifdef FEATURE_GYRO_DSPS
static void dsps_process_indication_gravity(sensor1_config_t *dsps_config,
  sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
  switch(msg_hdr->msg_id) {
  case SNS_SAM_GRAVITY_REPORT_IND_V01: {
    sns_sam_gravity_report_ind_msg_v01 *indication =
        (sns_sam_gravity_report_ind_msg_v01 *) msg_ptr;
    sns_sam_gravity_result_s_v01 *gravity_data =
      &indication->result;
    dsps_cb_data_t cb_data;

    cb_data.type = DSPS_DATA_TYPE_GRAVITY_VECTOR;
    memcpy(&cb_data.u.gravity.gravity, gravity_data->gravity,
      sizeof(float) * 3);
    memcpy(&cb_data.u.gravity.lin_accel, gravity_data->lin_accel,
      sizeof(float) *3);
    cb_data.u.gravity.accuracy = gravity_data->accuracy;
    dsps_config->dsps_callback(dsps_config->port, &cb_data);
  }
    break;
  default:
    CDBG_ERROR("%s: Invalid Indication ID\n", __func__);
    break;
  }
} /* dsps_process_indication_gravity */
#endif

/*===========================================================================
 * FUNCTION      dsps_sensor1_callback
 *
 * DESCRIPTION   Callback function to be registered with the sensor framework.
 *               This will be called in context of the sensor framework.
 *==========================================================================*/
#ifdef FEATURE_GYRO_DSPS
void dsps_sensor1_callback(intptr_t *data,
  sensor1_msg_header_s *msg_hdr,
  sensor1_msg_type_e msg_type,
  void *msg_ptr)
{
  sensor1_config_t * dsps_config =(sensor1_config_t *)data;
  sensor1_handle_s *handle = dsps_config->handle;

  switch (msg_type) {
    case SENSOR1_MSG_TYPE_RESP:
    if (msg_hdr->service_number == SNS_SMGR_SVC_ID_V01) {
      /* Ignore */
    } else if (msg_hdr->service_number == SNS_SAM_GYROINT_SVC_ID_V01) {
      CDBG("DSPS Response Received\n");
      dsps_process_response_gyro(dsps_config, msg_hdr, msg_ptr);
    } else if (msg_hdr->service_number == SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01) {
      CDBG("DSPS Gravity Vector Response Received\n");
      dsps_process_response_gravity(dsps_config, msg_hdr, msg_ptr);
    } else {
      CDBG_ERROR("%s: Response Msg Id %d not supported\n", __func__,
        msg_hdr->msg_id);
    }
      break;
    case SENSOR1_MSG_TYPE_IND:
      if (msg_hdr->service_number == SNS_SMGR_SVC_ID_V01) {
        /* Ignore */
    } else if (msg_hdr->service_number == SNS_SAM_GYROINT_SVC_ID_V01) {
      CDBG("%s: DSPS Gyro Indication Received\n", __func__);
      dsps_process_indication_gyro(dsps_config, msg_hdr, msg_ptr);
    } else if (msg_hdr->service_number == SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01) {
      CDBG("%s: DSPS Gravity Vector Indication Received!", __func__);
      dsps_process_indication_gravity(dsps_config, msg_hdr, msg_ptr);
    } else {
      CDBG_ERROR("%s Unexpected Indication Msg type received ",__func__);
    }
      break;
    case SENSOR1_MSG_TYPE_BROKEN_PIPE:
      dsps_handle_broken_pipe(dsps_config);
      break;
    default:
      CDBG_ERROR("%s: Invalid Message Type\n", __func__);
      break;
  }

  if (msg_ptr != NULL)
    sensor1_free_msg_buf(handle, msg_ptr);
}

void dump_time(const char *id)
{
  struct timespec t_now;
  clock_gettime( CLOCK_REALTIME, &t_now );
  CDBG_HIGH("%s, %s, time, %llu, (ms)", __FUNCTION__,id,
       (((int64_t)t_now.tv_sec * 1000) + t_now.tv_nsec/1000000));
}
#endif
