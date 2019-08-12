/*============================================================================

   Copyright (c) 2012 - 2015 Qualcomm Technologies, Inc. All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.

   This file defines the media/module/master controller's interface with the
   DSPS modules.

============================================================================*/
#include "dsps_hw.h"
#include "camera_dbg.h"


/** dsps_proc_init:
 *    @port: gyro port (opaque to dsps layer)
 *    @dsps_callback: callback to upper layer client
 *
 * This function creates an instance of DSPS (Discrete Sensors Processing
 * System) session and opens a channel to the DSPS.
 *
 * Returns DSPS client handle on success, NULL otherwise.
 **/
void *dsps_proc_init(void *port, dsps_callback_func dsps_callback)
{
  sensor1_config_t *dsps_config;

  CDBG("%s: E\n", __func__);
  dsps_config = (sensor1_config_t *)malloc(sizeof(sensor1_config_t));
  if (dsps_config == NULL) {
    CDBG_ERROR("%s:%d malloc error", __func__, __LINE__);
    return NULL;
  }

  memset((void *)dsps_config, 0, sizeof(sensor1_config_t));
  dsps_config->handle = (sensor1_handle_s *)-1;
  dsps_config->callback_arrived = 0;
  dsps_config->status = DSPS_RUNNING;
  dsps_config->error = 0;
  dsps_config->instance_id = -1;
  dsps_config->port = port;
  dsps_config->dsps_callback = dsps_callback;
  pthread_mutex_init(&(dsps_config->callback_mutex), NULL);
  pthread_cond_init(&(dsps_config->callback_condvar), NULL);
  pthread_mutex_init(&(dsps_config->thread_mutex), NULL);
  pthread_cond_init(&(dsps_config->thread_condvar), NULL);
  if (dsps_open((void *)dsps_config) != 0) {
    goto dsps_open_error;
  }
  CDBG("%s: X", __func__);
  return dsps_config;

dsps_open_error:
  CDBG_ERROR("%s: Failed to open sensor1 port", __func__);
  free(dsps_config);
  return NULL;
}


/** dsps_proc_deinit:
 *    @port: dsps_client_handle
 *
 * This function closes the DSPS channel and frees the session memory.
 **/
void dsps_proc_deinit(void *dsps_client_handle)
{
  sensor1_config_t *dsps_config = (sensor1_config_t *)dsps_client_handle;

  if (dsps_config == NULL) {
    CDBG_ERROR("%s: DSPS client handle is NULL", __func__);
    return;
  }

  dsps_disconnect((void *)dsps_config);
  free(dsps_config);
}


/*===========================================================================
 * FUNCTION    - dsps_proc_set_params_gyro -
 *
 * DESCRIPTION:
 *==========================================================================*/
int dsps_proc_set_params_gyro(void *dsps_client_handle, dsps_set_data_t *data)
{
  sensor1_config_t *dsps_config = (sensor1_config_t *)dsps_client_handle;

  if (dsps_config == NULL || dsps_config->status != DSPS_RUNNING) {
    CDBG("%s: DSPS is not running, deinitialized or not connected",
      __func__);
    return -1;
  }

  int rc = 0;
  sensor1_req_data_t msg_data;
  msg_data.msg_type = data->msg_type;
  msg_data.sensor_type = data->sensor_type;
  int wait = 0;

  CDBG("%s: msg_type %d", __func__, data->msg_type);
  switch (data->msg_type) {
    case DSPS_ENABLE_REQ:
      msg_data.u.gyro.gyro_sample_rate = data->u.gyro.gyro_sample_rate;
      wait = 1;
      break;

    case DSPS_DISABLE_REQ:
      wait = 1;
      break;

    case DSPS_GET_REPORT:
      msg_data.u.gyro.seqnum = data->u.gyro.data.id;
      msg_data.u.gyro.t_start = data->u.gyro.data.t_start;
      msg_data.u.gyro.t_end = data->u.gyro.data.t_end;
      break;

    default:
      CDBG_ERROR("%s Invalid param",__func__);
      return -1;
  }
#ifdef FEATURE_GYRO_DSPS
  if (dsps_send_request((void *)dsps_config, &msg_data, wait) < 0) {
    CDBG_ERROR("%s Error sending request", __func__);
    return -1;
  }
#endif
  return rc;
}  /* dsps_proc_set_params_gyro */

/*===========================================================================
 * FUNCTION    - dsps_proc_set_params_gravity -
 *
 * DESCRIPTION:
 *==========================================================================*/
int dsps_proc_set_params_gravity(void *dsps_client_handle, dsps_set_data_t *data)
{
  sensor1_config_t *dsps_config = (sensor1_config_t *)dsps_client_handle;

  if (dsps_config == NULL || dsps_config->status != DSPS_RUNNING) {
    CDBG("%s: DSPS is not running, deinitialized or not connected",
      __func__);
    return -1;
  }

  int rc = 0;
  sensor1_req_data_t msg_data;
  msg_data.sensor_type = data->sensor_type;
  msg_data.msg_type = data->msg_type;
  int wait = 0;

  CDBG("%s: msg_type %d", __func__, data->msg_type);
  switch (data->msg_type) {
  case DSPS_ENABLE_REQ: {
    msg_data.u.gravity.report_period = data->u.gravity.report_period;
    msg_data.u.gravity.sample_rate_valid = data->u.gravity.sample_rate_valid;
    msg_data.u.gravity.sample_rate = data->u.gravity.sample_rate;
    CDBG("%s: DSPS_ENABLE_REQ: repor_period: %u sample_rate_valid: %d "
      "sample_rate: %u",
      __func__, msg_data.u.gravity.report_period,
      msg_data.u.gravity.sample_rate_valid,
      msg_data.u.gravity.sample_rate);
    wait = 1;
  }
    break;

  case DSPS_DISABLE_REQ:
    wait = 1;
    break;

  case DSPS_GET_REPORT:
    wait = 1;
    break;

  default:
    CDBG_ERROR("%s Invalid param",__func__);
    return -1;
  }
#ifdef FEATURE_GYRO_DSPS
  if (dsps_send_request((void *)dsps_config, &msg_data, wait) < 0) {
    CDBG_ERROR("%s Error sending request", __func__);
    return -1;
  }
#endif
  return rc;
}  /* dsps_proc_set_params_gravity */

/*===========================================================================
 * FUNCTION    - dsps_proc_set_params -
 *
 * DESCRIPTION:
 *==========================================================================*/
int dsps_proc_set_params(void *dsps_client_handle, dsps_set_data_t *data)
{
  sensor1_config_t *dsps_config = (sensor1_config_t *)dsps_client_handle;
  int rc = 0;

  if (dsps_config == NULL || dsps_config->status != DSPS_RUNNING) {
    CDBG("%s: DSPS is not running, deinitialized or not connected",
      __func__);
    return -1;
  }

  CDBG("%s: DSPS Set Parameter for %d", __func__, data->sensor_type);

  switch (data->sensor_type) {
  case DSPS_DATA_TYPE_GYRO:
    rc = dsps_proc_set_params_gyro(dsps_client_handle, data);
    break;
  case DSPS_DATA_TYPE_GRAVITY_VECTOR:
    rc = dsps_proc_set_params_gravity(dsps_client_handle, data);
    break;
  default:
    CDBG_ERROR("%s: Sensor type %d not supported yet!", __func__,
      data->sensor_type);
    rc = -1;
    break;
  }

  return rc;
}  /* dsps_proc_set_params */
