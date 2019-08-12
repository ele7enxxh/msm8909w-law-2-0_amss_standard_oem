/*============================================================================
  @file DeviceOrientation.cpp

  @brief
  DeviceOrientation class implementation.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#include <cutils/properties.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include "TimeSyncService.h"
#include "DeviceOrientation.h"

/*============================================================================
  DeviceOrientation Constructor
============================================================================*/
DeviceOrientation::DeviceOrientation(int handle)
    :SAMSensor(handle),
     dev_ori_enabled(false)
{
    if(getAttribOK() == true) {
        svc_num = SNS_SAM_DEV_ORI_SVC_ID_V01;
        trigger_mode = SENSOR_MODE_EVENT;
        HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
        setVendor("QTI");
        setType(SENSOR_TYPE_DEVICE_ORIENTATION);
        setMaxRange(1);
        setResolution(1);

        if(handle == HANDLE_DEVICE_ORIENTATION_WAKE_UP) {
            bWakeUp = true;
            setName("DeviceOrientation -Wakeup");
            setFlags(SENSOR_FLAG_ON_CHANGE_MODE|SENSOR_FLAG_WAKE_UP);
        } else if(handle == HANDLE_DEVICE_ORIENTATION) {
            bWakeUp = false;
            setName("DeviceOrientation");
            setFlags(SENSOR_FLAG_ON_CHANGE_MODE);
        }

        /* Send Algo Attributes Request */
        sendAlgoAttribReq();
    }
}

/*============================================================================
  DeviceOrientation Destructor
============================================================================*/
DeviceOrientation::~DeviceOrientation()
{

}

/*============================================================================
  enable
============================================================================*/
int DeviceOrientation::enable(int en)
{
    int err;
    sensor1_error_e error;
    sensor1_msg_header_s req_hdr;
    sns_sam_dev_ori_enable_req_msg_v01 *sam_req;

    if (enabled == en) {
        HAL_LOG_INFO("DeviceOrientation is already enabled/disabled %d", enabled);
        return 0;
    }

    /* store the en value */
    enabled = en;
    HAL_LOG_DEBUG("%s:DeviceOrientation handle=%d, bWakeUp=%d", __FUNCTION__, handle, bWakeUp);

    if (en) {
        pthread_mutex_lock(&sensor1_cb->cb_mutex);
        error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                                 sizeof(sns_sam_dev_ori_enable_req_msg_v01),
                                 (void**)&sam_req );
        if (SENSOR1_SUCCESS != error) {
            HAL_LOG_ERROR("%s:DeviceOrientation sensor1_alloc_msg_buf error:%d", __FUNCTION__, error);
            pthread_mutex_unlock(&sensor1_cb->cb_mutex);
            enabled = 0;
            return -1;
        }

        req_hdr.service_number = svc_num;
        req_hdr.msg_id = SNS_SAM_DEV_ORI_ENABLE_REQ_V01;
        req_hdr.msg_size = sizeof(sns_sam_dev_ori_enable_req_msg_v01);
        req_hdr.txn_id = 0;

        /* set default behavior for indications during suspend */
        sam_req->notify_suspend_valid = true;
        sam_req->notify_suspend.proc_type = SNS_PROC_APPS_V01;
        sam_req->notify_suspend.send_indications_during_suspend = bWakeUp;

        /* Send Enable Request */
        err = sendEnableReq(&req_hdr, (void *)sam_req);
        if (err) {
            HAL_LOG_ERROR("DeviceOrientation send the SAM sensor Enable message failed!");
            pthread_mutex_unlock(&sensor1_cb->cb_mutex);
            enabled = 0;
            return -1;
        }
        HAL_LOG_DEBUG("%s: DeviceOrientation Received response: %d", __FUNCTION__, sensor1_cb->error);
        pthread_mutex_unlock(&sensor1_cb->cb_mutex);

        /* Get the last report data after enabling the sensor */
        err = sendGetReportReq();
        if (err) {
            HAL_LOG_ERROR("DeviceOrientation get the last sensor data return error!");
        }
    } else {
        /* Disable sensor */
        HAL_LOG_DEBUG("%s: DeviceOrientation Disabling sensor handle=%d", __FUNCTION__, handle);
        dev_ori_enabled = false;
        sendCancel();
    }
    return 0;
}

/*===========================================================================
  FUNCTION:  processResp
===========================================================================*/
void DeviceOrientation::processResp(sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    const sns_common_resp_s_v01*  crsp_ptr = (sns_common_resp_s_v01 *)msg_ptr;
    bool error = false;

    HAL_LOG_INFO("%s: DeviceOrientation handle:%d %d", __FUNCTION__, handle, msg_hdr->msg_id);

    if (crsp_ptr->sns_result_t != 0 &&
        msg_hdr->msg_id != SNS_SAM_DEV_ORI_CANCEL_RESP_V01) {
        HAL_LOG_ERROR("%s: Msg %i; Result: %u, Error: %u", __FUNCTION__,
                    msg_hdr->msg_id, crsp_ptr->sns_result_t, crsp_ptr->sns_err_t);
        error = true;
    }

    if(true != error) {
        switch (msg_hdr->msg_id) {
        case SNS_SAM_DEV_ORI_ENABLE_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_DEV_ORI_ENABLE_RESP_V01", __FUNCTION__);
            instance_id = ((sns_sam_dev_ori_enable_resp_msg_v01 *)msg_ptr)->instance_id;
            break;
        case SNS_SAM_DEV_ORI_GET_REPORT_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_DEV_ORI_GET_REPORT_RESP_V01", __FUNCTION__);
            processInd(msg_hdr, msg_ptr);
            break;
        case SNS_SAM_DEV_ORI_CANCEL_RESP_V01:
        case SNS_SAM_DEV_ORI_DISABLE_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_DEV_ORI_CANCEL/DISABLE_RESP_V01", __FUNCTION__);
            /* Reset instance ID */
            instance_id = 0xFF;
            break;
        case SNS_SAM_DEV_ORI_VERSION_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_DEV_ORI_VERSION_RESP_V01", __FUNCTION__);
            processAlgoAttribResp(msg_hdr, msg_ptr);
            break;
        case SNS_SAM_DEV_ORI_GET_ATTRIBUTES_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_DEV_ORI_GET_ATTRIBUTES_RESP_V01", __FUNCTION__);
            processAlgoAttribResp(msg_hdr, msg_ptr);
            break;
        default:
            HAL_LOG_ERROR("%s: Unknown msg id: %d", __FUNCTION__, msg_hdr->msg_id );
            return;
        }
    }

    if (msg_hdr->txn_id != TXN_ID_NO_RESP_SIGNALLED) {
        pthread_mutex_lock(&sensor1_cb->cb_mutex);
        Utility::signalResponse(error, sensor1_cb);
        pthread_mutex_unlock(&sensor1_cb->cb_mutex);
    }
}

/*===========================================================================
  FUNCTION:  processInd
===========================================================================*/
void DeviceOrientation::processInd(sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    bool                             error = false;
    uint32_t                         timestamp = 0;
    sensors_event_t                  sensor_data;

    HAL_LOG_DEBUG("%s: DeviceOrientation handle:%d", __FUNCTION__, handle);
    memset(&sensor_data, 0, sizeof(sensors_event_t));
    switch(msg_hdr->msg_id) {
        case SNS_SAM_DEV_ORI_REPORT_IND_V01:
        {
            sns_sam_dev_ori_report_ind_msg_v01*  rpt_ind_ptr =
                                     (sns_sam_dev_ori_report_ind_msg_v01*) msg_ptr;
            HAL_LOG_DEBUG("%s: SNS_SAM_DEV_ORI_REPORT_IND_V01", __FUNCTION__);
            sensor_data.data[0] = rpt_ind_ptr->dev_ori_state;
            timestamp = rpt_ind_ptr->timestamp;
            break;
        }
        case SNS_SAM_DEV_ORI_GET_REPORT_RESP_V01:
        {
            sns_sam_dev_ori_get_report_resp_msg_v01*  rpt_resp_ptr =
                                     (sns_sam_dev_ori_get_report_resp_msg_v01*) msg_ptr;
            if (rpt_resp_ptr->timestamp_valid && rpt_resp_ptr->dev_ori_state_valid) {
                sensor_data.data[0] = rpt_resp_ptr->dev_ori_state;
                timestamp = rpt_resp_ptr->timestamp;
            } else {
                HAL_LOG_ERROR("%s: DeviceOrientation invalid report resp, timestamp_valid=%d, state_valid=%d",
                    __FUNCTION__,
                    rpt_resp_ptr->timestamp_valid,
                    rpt_resp_ptr->dev_ori_state_valid);
                error = true;
            }
            break;
        }
        case SNS_SAM_DEV_ORI_ERROR_IND_V01:
        {
            HAL_LOG_ERROR("%s: SNS_SAM_DEV_ORI_ERROR_IND_V01", __FUNCTION__);
            error = true;
            break;
        }
        default:
        {
            HAL_LOG_ERROR( "%s: Unknown message ID = %d", __FUNCTION__, msg_hdr->msg_id );
            error = true;
            break;
        }
    }

    /* No error */
    if (error == false) {
        /* Unknown state is not defined in LA */
        if (sensor_data.data[0] == SNS_SAM_DEV_ORI_UNKNOWN_V01) {
        #ifdef DEV_ORI_HANDLE_UNKNOWN_EVENT
            sensor_data.data[0] = SNS_SAM_DEV_ORI_X_UP_V01;
            HAL_LOG_DEBUG( "%s: DeviceOrientation: Orientation Unknown, set do default orientation 'Portrait'.",__FUNCTION__);
        #else
            return;
        #endif
        }
        sensor_data.type = SENSOR_TYPE_DEVICE_ORIENTATION;
        if(bWakeUp == false) {
            sensor_data.sensor = HANDLE_DEVICE_ORIENTATION;
        } else {
            sensor_data.sensor = HANDLE_DEVICE_ORIENTATION_WAKE_UP;
        }
        sensor_data.version = sizeof(sensors_event_t);
        sensor_data.timestamp = time_service->timestampCalc(
                (uint64_t)timestamp, sensor_data.sensor);

        /* Droping duplicates and report accurate expected count of samples to clients */
        if (dev_ori_enabled == true && !bComplySampleRate(dev_ori_last_event.timestamp, sensor_data.timestamp)) {
            HAL_LOG_DEBUG("%s: sensor(%s): Drop Duplicate Sample, HAL TS Diff:%" PRId64,
                                            __FUNCTION__, Utility::SensorTypeToSensorString(getType()),
                                            (sensor_data.timestamp - dev_ori_last_event.timestamp));
            return;
        }
        if (dev_ori_enabled == false) {
            dev_ori_enabled = true;
        }
        HAL_LOG_VERBOSE( "%s: DeviceOrientation: Orientation: %f wakeup: %d SAM TS: %u HAL TS:%" PRId64 "elapsedRealtimeNano:%" PRId64 "",
                        __FUNCTION__, sensor_data.data[0], bWakeUp, timestamp, sensor_data.timestamp,
                        android::elapsedRealtimeNano());
        /* backup the last sensor event */
        memcpy(&dev_ori_last_event, &sensor_data, sizeof(sensors_event_t));
        pthread_mutex_lock(&data_cb->data_mutex);
        if (Utility::insertQueue(&sensor_data, bWakeUp, data_cb)) {
            Utility::signalInd(data_cb);
        }
        pthread_mutex_unlock(&data_cb->data_mutex);
    }
}

/*===========================================================================
  FUNCTION:  sendGetReportReq
    Get the last report data after enabling the sensor.
===========================================================================*/
int DeviceOrientation::sendGetReportReq(void)
{
    sensor1_error_e error;
    sensor1_msg_header_s req_hdr;
    sns_sam_dev_ori_get_report_req_msg_v01 *sam_req;

    HAL_LOG_INFO("%s: DeviceOrientation handle:%d", __FUNCTION__, handle);
    error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                                  sizeof(sns_sam_dev_ori_get_report_req_msg_v01),
                                  (void**)&sam_req);
    if (SENSOR1_SUCCESS != error) {
        HAL_LOG_ERROR( "%s: sensor1_alloc_msg_buf() error: %d", __FUNCTION__, error);
        return -1;
    }
    req_hdr.service_number = svc_num;
    req_hdr.msg_id = SNS_SAM_DEV_ORI_GET_REPORT_REQ_V01;
    req_hdr.msg_size = sizeof(sns_sam_dev_ori_get_report_req_msg_v01);
    req_hdr.txn_id = 0;
    sam_req->instance_id = instance_id;
    /* Send Request */
    sensor1_cb->error = false;

    error = sensor1_write(sensor1_cb->sensor1_handle, &req_hdr, sam_req);
    if (error != SENSOR1_SUCCESS) {
        /* free the message buffer */
        sensor1_free_msg_buf(sensor1_cb->sensor1_handle, sam_req);
        HAL_LOG_ERROR( "%s: DeviceOrientation sensor1_write() error: %d", __FUNCTION__, error);
        return -1;
    }
    return sensor1_cb->error ? -1 : 0;
}
