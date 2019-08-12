/*============================================================================
  @file PickUpGesture.cpp

  @brief
  PickUpGesture class implementation.

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#include <cutils/properties.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "TimeSyncService.h"
#include "PickUpGesture.h"
#include "SensorsContext.h"

/*============================================================================
  PickUpGesture Constructor
============================================================================*/
PickUpGesture::PickUpGesture(int handle)
    :SAMSensor(handle)
{
    if(getAttribOK() == true) {
        svc_num = SNS_SAM_DPC2_SVC_ID_V01;
        trigger_mode = SENSOR_MODE_TRIG;
        bWakeUp = true;
        HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);

        SensorsContext *context = SensorsContext::getInstance();
        Sensor *mSensor = context->getSensor(HANDLE_ACCELERATION);
        Sensor *mSensorProx = context->getSensor(HANDLE_PROXIMITY);

        setName("Pick Up Gesture");
        setVendor("QTI");
        setType(SENSOR_TYPE_PICK_UP_GESTURE);
        setFlags(SENSOR_FLAG_ONE_SHOT_MODE|SENSOR_FLAG_WAKE_UP);
        setMaxRange(1);
        setResolution(1);
        setVersion(1);
        setMaxFreq(1);
        setMinFreq(0);
        setMaxBufferedSamples(0);

        if (mSensor != NULL && mSensorProx != NULL) {
            setPower(mSensor->getPower() + mSensorProx->getPower());
        }
    }
}

/*============================================================================
  PickUpGesture Destructor
============================================================================*/
PickUpGesture::~PickUpGesture()
{

}

/*============================================================================
  enable
============================================================================*/
int PickUpGesture::enable(int en)
{
    int err;
    sensor1_error_e error;
    sensor1_msg_header_s req_hdr;
    sns_sam_dpc_enable_req_msg_v02 *sam_req;

    if (enabled == en) {
        HAL_LOG_INFO("Pick-Up-Gesture is already enabled/disabled %d", enabled);
        return 0;
    }

    /* store the en value */
    enabled = en;
    HAL_LOG_DEBUG("%s: handle=%d", __FUNCTION__, handle);

    if (en) {
        pthread_mutex_lock(&sensor1_cb->cb_mutex);
        error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                                 sizeof(sns_sam_dpc_enable_req_msg_v02),
                                 (void**)&sam_req );
        if (SENSOR1_SUCCESS != error) {
            HAL_LOG_ERROR("%s:sensor1_alloc_msg_buf error:%d", __FUNCTION__, error);
            pthread_mutex_unlock(&sensor1_cb->cb_mutex);
            enabled = 0;
            return -1;
        }

        req_hdr.service_number = svc_num;
        req_hdr.msg_id = SNS_SAM_DPC_ENABLE_REQ_V02;
        req_hdr.msg_size = sizeof(sns_sam_dpc_enable_req_msg_v02);
        req_hdr.txn_id = 0;

        /* set default behavior for indications during suspend */
        sam_req->notify_suspend_valid = true;
        sam_req->notify_suspend.proc_type = SNS_PROC_APPS_V01;
        sam_req->notify_suspend.send_indications_during_suspend = true;

        /* Send Enable Request */
        err = sendEnableReq(&req_hdr, (void *)sam_req);
        if (err) {
            HAL_LOG_ERROR("send the SAM sensor Enable message failed!");
            pthread_mutex_unlock(&sensor1_cb->cb_mutex);
            enabled = 0;
            return -1;
        }

        HAL_LOG_DEBUG("%s: Received response: %d", __FUNCTION__, sensor1_cb->error);
        pthread_mutex_unlock(&sensor1_cb->cb_mutex);
    } else {
        /* Disable sensor */
        HAL_LOG_DEBUG("%s: Disabling sensor handle=%d", __FUNCTION__, handle);
        sendCancel();
    }
    return 0;
}

/*===========================================================================
  FUNCTION:   processResp
===========================================================================*/
void PickUpGesture::processResp(sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    sns_sam_dpc_enable_resp_msg_v02* crsp_ptr = (sns_sam_dpc_enable_resp_msg_v02*) msg_ptr;
    bool                          error = false;

    HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
    if (crsp_ptr->resp.sns_result_t != 0 &&
        msg_hdr->msg_id != SNS_SAM_DPC_CANCEL_RESP_V02) {
        HAL_LOG_ERROR("%s: Msg %i; Result: %u, Error: %u", __FUNCTION__,
                    msg_hdr->msg_id, crsp_ptr->resp.sns_result_t, crsp_ptr->resp.sns_err_t);

        /* Disregard DPC failures when algo is already disabled; avoid delete-retry cycles */
        if(SNS_SAM_DPC_DISABLE_RESP_V02 == msg_hdr->msg_id &&
            SENSOR1_ENOTALLOWED == crsp_ptr->resp.sns_err_t)
        {
            error = false;
        }
    }

    if(true != error ) {
        switch (msg_hdr->msg_id) {
        case SNS_SAM_DPC_ENABLE_RESP_V02:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_DPC_ENABLE_RESP_V02", __FUNCTION__);
            instance_id = ((sns_sam_dpc_enable_resp_msg_v02 *)msg_ptr)->instance_id;
            break;
        case SNS_SAM_DPC_DISABLE_RESP_V02:
        case SNS_SAM_DPC_CANCEL_RESP_V02:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_DPC_DISABLE/CANCEL_RESP_V02", __FUNCTION__);
            /* Reset instance ID */
            instance_id = 0xFF;
            if( msg_hdr->txn_id == TXN_ID_NO_RESP_SIGNALLED )
            {
              /* This disable response is because DPC was "auto-disabled" due to receiving
               * an DPC indication. Don't signal a response here, since the HAL isn't
               * expecting one for this case */
              HAL_LOG_VERBOSE("%s: DPC disable response. DPC auto-disabled due to indication",
                              __FUNCTION__ );
              return;
            }
            HAL_LOG_VERBOSE("%s: DPC disable response. DPC disabled due to HAL command",
                            __FUNCTION__ );
            break;
        case SNS_SAM_DPC_GET_ATTRIBUTES_RESP_V02:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_DPC_GET_ATTRIBUTES_RESP_V02", __FUNCTION__);
            processAlgoAttribResp(msg_hdr, msg_ptr);
            break;
        default:
            HAL_LOG_ERROR("%s: Unknown msg id: %d", __FUNCTION__, msg_hdr->msg_id);
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
  FUNCTION:   processInd
===========================================================================*/
void PickUpGesture::processInd(sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    sns_sam_dpc_report_ind_msg_v02*  sam_dpc_rpt_ptr =
                                     (sns_sam_dpc_report_ind_msg_v02*) msg_ptr;
    bool                             error = false;
    uint32_t                         timestamp = 0;
    sensors_event_t                  sensor_data;

    HAL_LOG_DEBUG("%s: Pick-Up-Gesture", __FUNCTION__);
    switch( msg_hdr->msg_id ) {
        case SNS_SAM_DPC_REPORT_IND_V02:
            HAL_LOG_DEBUG("%s: SNS_SAM_DPC_REPORT_IND_V02", __FUNCTION__);
            sensor_data.type = SENSOR_TYPE_PICK_UP_GESTURE;
            sensor_data.sensor = HANDLE_PICK_UP_GESTURE;
            if (sam_dpc_rpt_ptr->dpc_state == 0)
            {
                sensor_data.data[0] = 1;
                HAL_LOG_VERBOSE("%s: sensor %d, dpc state %f", __FUNCTION__,
                                sensor_data.type, sensor_data.data[0]);
                timestamp = sam_dpc_rpt_ptr->timestamp;
            }
            else
            {
                error = true;
                HAL_LOG_DEBUG("%s: Invalid dpc state %d",__FUNCTION__,
                               sam_dpc_rpt_ptr->dpc_state);
            }
            break;
        case SNS_SAM_DPC_ERROR_IND_V02:
            HAL_LOG_ERROR("%s: SNS_SAM_DPC_ERROR_IND_V02", __FUNCTION__);
            error = true;
            break;
        default:
            HAL_LOG_ERROR("%s: Unknown message ID = %d", __FUNCTION__, msg_hdr->msg_id);
            error = true;
            break;
    }

    /* No error */
    if (error == false) {
        /* Deactivate DPC since pick-up-gesture is one-shot sensor */
        deactivateDpc();
        sensor_data.version = sizeof(sensors_event_t);
        sensor_data.timestamp = time_service->timestampCalc(
                (uint64_t)timestamp, sensor_data.sensor);

        HAL_LOG_VERBOSE("%s: PUG: state: %f SAM TS: %u HAL TS:%" PRId64 " elapsedRealtimeNano:%" PRId64 "",
                        __FUNCTION__, sensor_data.data[0], timestamp, sensor_data.timestamp,
                        android::elapsedRealtimeNano());
        pthread_mutex_lock(&data_cb->data_mutex);
        if (Utility::insertQueue(&sensor_data, bWakeUp, data_cb)) {
            Utility::signalInd(data_cb);
        }
        pthread_mutex_unlock(&data_cb->data_mutex);
    }
    else {
        pthread_mutex_lock(&data_cb->data_mutex);
        /* Release wakelock if held */
        if (getFlags() & SENSOR_FLAG_WAKE_UP) {
            Utility::release_sensor_wake_lock(data_cb, __FUNCTION__);
        }
        pthread_mutex_unlock(&data_cb->data_mutex);
    }
}

void PickUpGesture::deactivateDpc()
{
    sensor1_error_e       error;
    sensor1_msg_header_s  req_hdr;
    sns_sam_dpc_disable_req_msg_v02*  sam_req;

    HAL_LOG_DEBUG("%s", __FUNCTION__ );
    pthread_mutex_lock( &sensor1_cb->cb_mutex );
    /* Disable DPC */
    enabled = 0;
    /* send request to disable algo */
    error = sensor1_alloc_msg_buf( sensor1_cb->sensor1_handle,
                                   sizeof(sns_sam_dpc_disable_req_msg_v02),
                                   (void**)&sam_req );
    if(SENSOR1_SUCCESS == error) {
        /* Message header */
        req_hdr.service_number = SNS_SAM_DPC2_SVC_ID_V01;
        req_hdr.msg_id = SNS_SAM_DPC_DISABLE_REQ_V02;
        req_hdr.msg_size = sizeof( sns_sam_dpc_disable_req_msg_v02 );
        req_hdr.txn_id = TXN_ID_NO_RESP_SIGNALLED;
        sam_req->instance_id = instance_id;

        HAL_LOG_VERBOSE("%s: Sending DPC disable request. Instance ID %d",
                    __FUNCTION__, sam_req->instance_id );
        /* Send Request */
        sensor1_cb->error = false;
        if( (error = sensor1_write( sensor1_cb->sensor1_handle, &req_hdr,
                                    sam_req )) != SENSOR1_SUCCESS ) {
            sensor1_free_msg_buf( sensor1_cb->sensor1_handle, sam_req );
            HAL_LOG_ERROR("%s: sensor1_write() error: %u", __FUNCTION__, error );
        }

        /*
         *  waiting for response - 200ms
         *  This is need to give enough time for SAM/Algo to clean up after
         *  sending disable request.
         *  This call will fail as above disable request gets sent as part
         *  of indication call back and response for the request will be blocked.
         */
        if (Utility::waitForResponse(TIME_OUT_MS_DPC_IND,
                                   &sensor1_cb->cb_mutex,
                                   &sensor1_cb->cb_cond,
                                   &sensor1_cb->is_resp_arrived) == false) {
            HAL_LOG_VERBOSE("%s: No response from the request as expected", __FUNCTION__);
        }
    }
    else {
        HAL_LOG_ERROR("%s: failed to allocated disable msg", __FUNCTION__);
    }
    pthread_mutex_unlock( &sensor1_cb->cb_mutex );
}
