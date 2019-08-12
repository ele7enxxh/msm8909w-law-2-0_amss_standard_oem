/*============================================================================
  @file PersistentMotionDetector.cpp

  @brief
  PersistentMotionDetector class implementation.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#include <cutils/properties.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "TimeSyncService.h"
#include "PersistentMotionDetector.h"

PersistentMotionDetector::PersistentMotionDetector(int handle):
  SAMSensor(handle)
{
  monitorMode = SNS_SAM_PMD_MODE_BOTH_V01;
  monitorPeriod = PMD_DEFAULT_MONITOR_PERIOD;
  oneShotMode = false;
  sensorType = SENSOR_TYPE_PMD;
  sensorHandle = handle;

  if (getAttribOK() == true) {
    svc_num = SNS_SAM_PMD_SVC_ID_V01;
    trigger_mode = SENSOR_MODE_TRIG;
    bWakeUp = true;
    HAL_LOG_INFO("%s: handle:%d", __func__, handle);
    setName("Persistent Motion Detector");
    setVendor("QTI");
    setType(sensorType);
    setFlags(SENSOR_FLAG_WAKE_UP | SENSOR_FLAG_ON_CHANGE_MODE);
    setMaxRange(1);
    setResolution(1);

    /* Send Algo Attributes Request */
    sendAlgoAttribReq();
  }
}

int PersistentMotionDetector::enable(int en)
{
  int err;
  sensor1_error_e error;
  sensor1_msg_header_s req_hdr;
  sns_sam_pmd_enable_req_msg_v01* sam_req;

  if (enabled == en) {
    HAL_LOG_INFO("PMD is already enabled/disabled %d", enabled);
    return 0;
  }

  /* store the en value */
  enabled = en;
  HAL_LOG_DEBUG("%s: handle=%d", __func__, handle);

  if (en) {
    pthread_mutex_lock(&sensor1_cb->cb_mutex);
    error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                                  sizeof(sns_sam_pmd_enable_req_msg_v01),
                                  (void**)&sam_req);
    if (SENSOR1_SUCCESS != error) {
      HAL_LOG_ERROR("%s:sensor1_alloc_msg_buf error:%d", __func__, error);
      pthread_mutex_unlock(&sensor1_cb->cb_mutex);
      enabled = 0;
      return -1;
    }

    req_hdr.service_number = svc_num;
    req_hdr.msg_id = SNS_SAM_PMD_ENABLE_REQ_V01;
    req_hdr.msg_size = sizeof(sns_sam_pmd_enable_req_msg_v01);
    req_hdr.txn_id = 0;

    /* set default behavior for indications during suspend */
    sam_req->notify_suspend_valid = true;
    sam_req->notify_suspend.proc_type = SNS_PROC_APPS_V01;
    sam_req->notify_suspend.send_indications_during_suspend = true;

    sam_req->mode = monitorMode;
    sam_req->monitor_period = monitorPeriod;

    /* Send Enable Request */
    err = sendEnableReq(&req_hdr, (void*)sam_req);
    if (err) {
      HAL_LOG_ERROR("send the SAM sensor Enable message failed!");
      pthread_mutex_unlock(&sensor1_cb->cb_mutex);
      enabled = 0;
      return -1;
    }

    HAL_LOG_DEBUG("%s: Received response: %d", __func__, sensor1_cb->error);
    pthread_mutex_unlock(&sensor1_cb->cb_mutex);
  } else {
    /* Disable sensor */
    HAL_LOG_DEBUG("%s: Disabling sensor handle=%d", __func__, handle);
    sendCancel();
  }
  return 0;
}

void PersistentMotionDetector::processResp(sensor1_msg_header_s* msg_hdr,
                                           void* msg_ptr)
{
  sns_sam_pmd_enable_resp_msg_v01* crsp_ptr =
    (sns_sam_pmd_enable_resp_msg_v01*)msg_ptr;
  bool error = false;

  HAL_LOG_INFO("%s: handle:%d", __func__, handle);
  if (crsp_ptr->resp.sns_result_t != 0 &&
      msg_hdr->msg_id != SNS_SAM_PMD_CANCEL_RESP_V01) {
    HAL_LOG_ERROR("%s: Msg %i; Result: %u, Error: %u", __func__,
                  msg_hdr->msg_id, crsp_ptr->resp.sns_result_t,
                  crsp_ptr->resp.sns_err_t);
  }

  if (true != error) {
    switch (msg_hdr->msg_id) {
      case SNS_SAM_PMD_ENABLE_RESP_V01:
        HAL_LOG_DEBUG("%s: Received SNS_SAM_PMD_ENABLE_RESP_V01", __func__);
        instance_id = ((sns_sam_pmd_enable_resp_msg_v01*)msg_ptr)->instance_id;
        break;
      case SNS_SAM_PMD_DISABLE_RESP_V01:
      case SNS_SAM_PMD_CANCEL_RESP_V01:
        HAL_LOG_DEBUG("%s: Received SNS_SAM_PMD_DISABLE/CANCEL_RESP_V01",
                       __func__);
        /* Reset instance ID */
        instance_id = 0xFF;
        if (msg_hdr->txn_id == TXN_ID_NO_RESP_SIGNALLED) {
          /* This disable response is because PMD was "auto-disabled" due to receiving
           * an PMD indication. Don't signal a response here, since the HAL isn't
           * expecting one for this case */
          HAL_LOG_VERBOSE("%s: PMD disable response. PMD auto-disabled due to indication",
                          __func__);
          return;
        }
        HAL_LOG_VERBOSE("%s: PMD disable response. "
                        "PMD disabled due to HAL command",
                        __func__);
        break;
      case SNS_SAM_PMD_GET_ATTRIBUTES_RESP_V01:
        HAL_LOG_DEBUG("%s: Received SNS_SAM_PMD_GET_ATTRIBUTES_RESP_V01",
                      __func__);
        processAlgoAttribResp(msg_hdr, msg_ptr);
        break;
      default:
        HAL_LOG_ERROR("%s: Unknown msg id: %d", __func__, msg_hdr->msg_id);
        return;
    }
  }

  if (msg_hdr->txn_id != TXN_ID_NO_RESP_SIGNALLED) {
    pthread_mutex_lock(&sensor1_cb->cb_mutex);
    Utility::signalResponse(error, sensor1_cb);
    pthread_mutex_unlock(&sensor1_cb->cb_mutex);
  }
}

void PersistentMotionDetector::processInd(sensor1_msg_header_s* msg_hdr,
                                          void* msg_ptr)
{
  sns_sam_pmd_report_ind_msg_v01*  sam_pmd_rpt_ptr =
    (sns_sam_pmd_report_ind_msg_v01*)msg_ptr;
  bool error = false;
  uint32_t timestamp = 0;
  sensors_event_t sensor_data;
  float rest_value;
  float motion_value;
  memset(&sensor_data, 0, sizeof(sensors_event_t));

  HAL_LOG_DEBUG("%s: PMD", __func__);
  switch (msg_hdr->msg_id) {
    case SNS_SAM_PMD_REPORT_IND_V01:
      HAL_LOG_DEBUG("%s: SNS_SAM_PMD_REPORT_IND_V01", __func__);
      sensor_data.type = sensorType;
      sensor_data.sensor = sensorHandle;
      timestamp = sam_pmd_rpt_ptr->timestamp;

      HAL_LOG_VERBOSE("%s: sensor %d, state %d", __func__,
                      sensor_data.type,
                      sam_pmd_rpt_ptr->state);
      rest_value = 1.0f;
      motion_value = (sensor_data.type == SENSOR_TYPE_PMD) ? 2.0f : 1.0f;

      switch (sam_pmd_rpt_ptr->state) {
        case SNS_SAM_PMD_STATE_REST_V01:
          sensor_data.data[0] = rest_value;
          break;
        case SNS_SAM_PMD_STATE_MOTION_V01:
          sensor_data.data[0] = motion_value;
          break;
        default:
          error = true;
          HAL_LOG_DEBUG("%s: Invalid pmd state %d", __func__,
                        sam_pmd_rpt_ptr->state);
          break;
      }
      break;
    case SNS_SAM_PMD_ERROR_IND_V01:
      HAL_LOG_ERROR("%s: SNS_SAM_PMD_ERROR_IND_V01", __func__);
      error = true;
      break;
    default:
      HAL_LOG_ERROR("%s: Unknown message ID = %d", __func__,
                    msg_hdr->msg_id);
      error = true;
      break;
  }

  /* No error */
  if (error == false) {
    sensor_data.version = sizeof(sensors_event_t);
    sensor_data.timestamp = time_service->timestampCalc(
      (uint64_t)timestamp, sensor_data.sensor);

    HAL_LOG_VERBOSE("%s: PMD: motion %d SAM TS: %lld "
                    "elapsedRealtimeNano:%lld", __func__,
                    sam_pmd_rpt_ptr->state, (long long) sensor_data.timestamp,
                    (long long) android::elapsedRealtimeNano());

    pthread_mutex_lock(&data_cb->data_mutex);
    if (Utility::insertQueue(&sensor_data, bWakeUp, data_cb)) {
      Utility::signalInd(data_cb);
    }
    pthread_mutex_unlock(&data_cb->data_mutex);
    /* Deactivate PMD if operated in one-shot-mode */
    if (oneShotMode) {
      deactivate();
    }
  } else {
    /* Release wakelock if held */
    if (getFlags() & SENSOR_FLAG_WAKE_UP) {
      Utility::release_sensor_wake_lock(data_cb, __func__);
    }
  }
}

void PersistentMotionDetector::deactivate()
{
  sensor1_error_e error;
  sensor1_msg_header_s req_hdr;
  sns_sam_pmd_disable_req_msg_v01* sam_req;

  HAL_LOG_DEBUG("%s", __func__);
  pthread_mutex_lock(&sensor1_cb->cb_mutex);
  /* Disable PMD */
  enabled = 0;
  /* send request to disable algo */
  error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                                sizeof(sns_sam_pmd_disable_req_msg_v01),
                                (void**)&sam_req);
  if (SENSOR1_SUCCESS == error) {
    /* Message header */
    req_hdr.service_number = SNS_SAM_PMD_SVC_ID_V01;
    req_hdr.msg_id = SNS_SAM_PMD_DISABLE_REQ_V01;
    req_hdr.msg_size = sizeof(sns_sam_pmd_disable_req_msg_v01);
    req_hdr.txn_id = TXN_ID_NO_RESP_SIGNALLED;
    sam_req->instance_id = instance_id;

    HAL_LOG_VERBOSE("%s: Sending PMD disable request. Instance ID %d",
                    __func__, sam_req->instance_id);
    /* Send Request */
    sensor1_cb->error = false;
    if ((error = sensor1_write(sensor1_cb->sensor1_handle, &req_hdr,
                               sam_req)) != SENSOR1_SUCCESS) {
      sensor1_free_msg_buf(sensor1_cb->sensor1_handle, sam_req);
      HAL_LOG_ERROR("%s: sensor1_write() error: %u", __func__, error);
    }

    /*
     *  waiting for response - 200ms
     *  This is need to give enough time for SAM/Algo to clean up after
     *  sending disable request.
     *  This call will fail as above disable request gets sent as part
     *  of indication call back and response for the request will be blocked.
     */
    if (Utility::waitForResponse(TIME_OUT_MS_PMD_IND,
                                 &sensor1_cb->cb_mutex,
                                 &sensor1_cb->cb_cond,
                                 &sensor1_cb->is_resp_arrived) == false) {
      HAL_LOG_VERBOSE("%s: No response from the request as expected",
                      __func__);
    }
  } else {
    HAL_LOG_ERROR("%s: failed to allocated disable msg", __func__);
  }
  pthread_mutex_unlock(&sensor1_cb->cb_mutex);
}

AndroidStationaryDetector::AndroidStationaryDetector(int handle):
  PersistentMotionDetector(handle)
{
  oneShotMode = true;
  monitorMode = SNS_SAM_PMD_MODE_REST_V01;
  sensorType = SENSOR_TYPE_STATIONARY_DETECT;
  setName("Android Stationary Detector");
  setType(sensorType);
  setFlags(SENSOR_FLAG_ONE_SHOT_MODE | SENSOR_FLAG_WAKE_UP);
}

AndroidMotionDetector::AndroidMotionDetector(int handle):
  PersistentMotionDetector(handle)
{
  oneShotMode = true;
  monitorMode = SNS_SAM_PMD_MODE_MOTION_V01;
  sensorType = SENSOR_TYPE_MOTION_DETECT;
  setName("Android Motion Detector");
  setType(sensorType);
  setFlags(SENSOR_FLAG_ONE_SHOT_MODE | SENSOR_FLAG_WAKE_UP);
}
