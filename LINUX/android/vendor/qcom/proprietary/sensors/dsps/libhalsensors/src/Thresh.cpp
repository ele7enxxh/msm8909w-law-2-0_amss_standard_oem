/*============================================================================
  @file Thresh.cpp

  @brief
  Thresh class implementation.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#include <cutils/properties.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include "TimeSyncService.h"
#include "Thresh.h"
#include "SensorsContext.h"

/*============================================================================
  Thresh Constructor
============================================================================*/
Thresh::Thresh(int handle)
    :SAMSensor(handle),
     thresh_enabled(false)
{
    if(getAttribOK() == true) {
        sensor1_error_e err;
        SensorsContext *context = SensorsContext::getInstance();
        Sensor *mSensor = NULL;

        svc_num = SNS_SAM_SENSOR_THRESH_SVC_ID_V01;
        trigger_mode = SENSOR_MODE_EVENT;
         batch_support = true;

        HAL_LOG_INFO("%s: handle=%d", __FUNCTION__, handle);

        if(HANDLE_THRESH_NON_WAKE_UP == handle) {
            mSensor = context->getSensor(HANDLE_PROXIMITY_NON_WAKE_UP);
            bWakeUp = false;
        } else if (HANDLE_THRESH == handle) {
            mSensor = context->getSensor(HANDLE_PROXIMITY);
            bWakeUp = true;
        }

        if (mSensor != NULL) {
            setType(mSensor->getType());
            setFlags(mSensor->getFlags());
            setVendor(mSensor->getVendor());
            setName(mSensor->getName());
            setMaxRange(mSensor->getMaxRange());
            setResolution(mSensor->getResolution());
            setPower(mSensor->getPower());
            setMaxFreq(mSensor->getMaxFreq());
            setMinFreq(mSensor->getMinFreq());
        }
        else {
            HAL_LOG_ERROR("The mSensor handle %d is NULL!", handle);
        }

        /* Send Algo Attributes Request */
        sendAlgoAttribReq();
    }
}

/*============================================================================
  Thresh Destructor
============================================================================*/
Thresh::~Thresh()
{

}

/*============================================================================
  enable
============================================================================*/
int Thresh::enable(int en)
{
    int err;
    sensor1_error_e error;
    sensor1_msg_header_s req_hdr;
    sns_sam_sensor_thresh_enable_req_msg_v01 *sam_req;

    if (enabled == en) {
        HAL_LOG_INFO("THRESH is already enabled/disabled %d", enabled);
        return 0;
    }

    /* store the en value */
    enabled = en;
    HAL_LOG_DEBUG("%s: handle=%d", __FUNCTION__, handle);

    if (en) {
        pthread_mutex_lock(&sensor1_cb->cb_mutex);
        error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                                 sizeof(sns_sam_sensor_thresh_enable_req_msg_v01),
                                 (void**)&sam_req );
        if (SENSOR1_SUCCESS != error) {
            HAL_LOG_ERROR("%s:sensor1_alloc_msg_buf error:%d", __FUNCTION__, error);
            pthread_mutex_unlock(&sensor1_cb->cb_mutex);
            enabled = 0;
            return -1;
        }

        req_hdr.service_number = svc_num;
        req_hdr.msg_id = SNS_SAM_SENSOR_THRESH_ENABLE_REQ_V01;
        req_hdr.msg_size = sizeof(sns_sam_sensor_thresh_enable_req_msg_v01);
        req_hdr.txn_id = 0;

        /* Report Request */
        sam_req->sensor_id = SNS_SMGR_ID_PROX_LIGHT_V01;
        sam_req->data_type = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
        sam_req->sample_rate = (5 << 16); /* ignore report_rate, use 5Hz */
        sam_req->threshold[0] = 0xFFFF;   /* threshold for binary states in Q16 */
        sam_req->threshold[1] = sam_req->threshold[2] = 0;  /* for completeness */

        /* set default behavior for indications during suspend */
        sam_req->notify_suspend_valid = true;
        sam_req->notify_suspend.proc_type = SNS_PROC_APPS_V01;
        sam_req->notify_suspend.send_indications_during_suspend = bWakeUp;

        /* Send Enable Request */
        err = sendEnableReq(&req_hdr, (void *)sam_req);
        if (err) {
            HAL_LOG_ERROR("send the SAM sensor Enable message failed!");
            pthread_mutex_unlock(&sensor1_cb->cb_mutex);
            enabled = 0;
            return -1;
        }

        HAL_LOG_DEBUG("%s: Received response: %d", __FUNCTION__, sensor1_cb->error );

        /* Send batch request */
        if( batching ) {
            err = sendBatchReq();
            if (err) {
                HAL_LOG_ERROR("%s: sendBatchReq failed!", Utility::SensorTypeToSensorString(getType()));
                sendCancel();
                pthread_mutex_unlock(&sensor1_cb->cb_mutex);
                enabled = 0;
                return -1;
            }
        }
        pthread_mutex_unlock(&sensor1_cb->cb_mutex);
    } else {
        /* Disable sensor */
        HAL_LOG_DEBUG("%s: Disabling sensor handle=%d", __FUNCTION__, handle);
        sendCancel();
        thresh_enabled = false;
    }
    return 0;
}

/*===========================================================================
  FUNCTION:  sendBatchReq
===========================================================================*/
int Thresh::sendBatchReq()
{
    sensor1_error_e  error;
    sensor1_msg_header_s req_hdr;
    sns_sam_sensor_thresh_batch_req_msg_v01 *sam_req;
    float batch_rate_in_hz;

    HAL_LOG_DEBUG( "%s: batching: %d, batch_rate=%f (Hz)",
                   __FUNCTION__, batching, FX_FIXTOFLT_Q16( batch_rate ));

    error = sensor1_alloc_msg_buf( sensor1_cb->sensor1_handle,
                                   sizeof(sns_sam_sensor_thresh_batch_req_msg_v01),
                                   (void**)&sam_req );
    if( SENSOR1_SUCCESS != error ) {
        HAL_LOG_ERROR( "%s: sensor1_alloc_msg_buf() error: %d", __FUNCTION__, error );
      return -1;
    }
    req_hdr.service_number = svc_num;
    req_hdr.msg_id = SNS_SAM_SENSOR_THRESH_BATCH_REQ_V01;
    req_hdr.msg_size = sizeof( sns_sam_sensor_thresh_batch_req_msg_v01 );
    req_hdr.txn_id = TXN_ID_NO_RESP_SIGNALLED;

    sam_req->instance_id = instance_id;

    /* convert batch rate from Hz in Q16 to batch period in seconds in Q16 */
    batch_rate_in_hz = FX_FIXTOFLT_Q16( batch_rate );
    sam_req->batch_period = batching ?
        FX_FLTTOFIX_Q16( 1.0 / batch_rate_in_hz ) :
        0 ;  /* Rate of 0 means disabled */

    /* Send Request */
    if( (error = sensor1_write( sensor1_cb->sensor1_handle, &req_hdr,
                                sam_req )) != SENSOR1_SUCCESS )
    {
        /* free the message buffer */
        sensor1_free_msg_buf( sensor1_cb->sensor1_handle, sam_req );
        HAL_LOG_ERROR( "%s: sensor1_write() error: %d", __FUNCTION__, error );
        return -1;
    }

    return sensor1_cb->error ? -1 : 0;
}

/*===========================================================================
  FUNCTION:  processResp
===========================================================================*/
void Thresh::processResp(sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    const sns_common_resp_s_v01*  crsp_ptr = (sns_common_resp_s_v01 *)msg_ptr;
    bool                          error = false;

    HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
#if 0
    if (crsp_ptr->sns_result_t != 0) {
        HAL_LOG_ERROR("%s: Msg %i; Result: %u, Error: %u", __FUNCTION__,
                    msg_hdr->msg_id, crsp_ptr->sns_result_t, crsp_ptr->sns_err_t);
        error = true;
    }
#endif

    if(true != error ) {
        switch (msg_hdr->msg_id) {
        case SNS_SAM_SENSOR_THRESH_ENABLE_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_SENSOR_THRESH_ENABLE_RESP_V01", __FUNCTION__);
            instance_id = ((sns_sam_sensor_thresh_enable_resp_msg_v01 *)msg_ptr)->instance_id;
            break;
        case SNS_SAM_SENSOR_THRESH_DISABLE_RESP_V01:
        case SNS_SAM_SENSOR_THRESH_CANCEL_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_SENSOR_THRESH_DISABLE/CANCEL_RESP_V01", __FUNCTION__);
            /* Reset instance ID */
            instance_id = 0xFF;
            break;
        case SNS_SAM_SENSOR_THRESH_GET_ATTRIBUTES_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_SENSOR_THRESH_GET_ATTRIBUTES_RESP_V01", __FUNCTION__);
            processAlgoAttribResp(msg_hdr, msg_ptr);
            break;
        case SNS_SAM_SENSOR_THRESH_BATCH_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_SENSOR_THRESH_BATCH_RESP_V01", __FUNCTION__);
            if( ((sns_sam_sensor_thresh_batch_resp_msg_v01*)msg_ptr)->max_batch_size_valid )
            {
                int max_samples = ((sns_sam_sensor_thresh_batch_resp_msg_v01*)msg_ptr)->max_batch_size;
                max_buffered_samples = max_samples;
            }
            pthread_mutex_lock(&data_cb->data_mutex);
            flushSendCmplt();
            pthread_mutex_unlock(&data_cb->data_mutex);
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
  FUNCTION:  processInd
===========================================================================*/
void Thresh::processInd(sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    hal_sam_sample_t *sample_list = NULL;
    hal_sam_sample_t *curr_sample = NULL;
    sensors_event_t la_sample;
    uint32_t i = 0;
    uint32_t count = 0;
    SensorsContext *context = SensorsContext::getInstance();
    Sensor *mSensor = context->getSensor(HANDLE_THRESH);

    if(bWakeUp == false) mSensor = context->getSensor(HANDLE_THRESH_NON_WAKE_UP);
    HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
    if (SNS_SAM_SENSOR_THRESH_REPORT_IND_V01 == msg_hdr->msg_id) {
        HAL_LOG_DEBUG("%s: SNS_SAM_SENSOR_THRESH_REPORT_IND_V01", __FUNCTION__);
        sns_sam_sensor_thresh_report_ind_msg_v01* sam_ind =
            (sns_sam_sensor_thresh_report_ind_msg_v01*)msg_ptr;

        sample_list = (hal_sam_sample_t *)malloc(sizeof(hal_sam_sample_t));
        if (NULL == sample_list) {
            HAL_LOG_ERROR( "%s: Malloc error", __FUNCTION__ );
        } else {
            count = 1;
            sample_list->data[0] = sam_ind->sample_value[0];
            sample_list->data[1] = sam_ind->sample_value[1];
            sample_list->data[2] = sam_ind->sample_value[2];
            sample_list->timestamp = sam_ind->timestamp;
        }
    }
    else if(SNS_SAM_SENSOR_THRESH_BATCH_IND_V01 == msg_hdr->msg_id) {
        HAL_LOG_DEBUG("%s: SNS_SAM_SENSOR_THRESH_BATCH_IND_V01", __FUNCTION__);
        sns_sam_sensor_thresh_batch_ind_msg_v01* sam_ind =
            (sns_sam_sensor_thresh_batch_ind_msg_v01*)msg_ptr;

        sample_list = (hal_sam_sample_t *)malloc(sam_ind->items_len * sizeof(hal_sam_sample_t));
        if (NULL == sample_list) {
            HAL_LOG_ERROR( "%s: Malloc error", __FUNCTION__ );
        }
        else {
            curr_sample = sample_list;
            count = sam_ind->items_len;

            for(i = 0; i < sam_ind->items_len; i++) {
                curr_sample->data[0] = sam_ind->items[ i ].sample_value[0];
                curr_sample->data[1] = sam_ind->items[ i ].sample_value[1];
                curr_sample->data[2] = sam_ind->items[ i ].sample_value[2];
                curr_sample->timestamp = sam_ind->items[ i ].timestamp;
                curr_sample++;
            }
        }
    }
    else {
        HAL_LOG_ERROR("%s: Unknown message ID = %d", __FUNCTION__, msg_hdr->msg_id);
    }

    if(count == 0) {
        pthread_mutex_lock(&data_cb->data_mutex);
        /* Release wakelock if held */
        if (getFlags() & SENSOR_FLAG_WAKE_UP) {
            Utility::release_sensor_wake_lock(data_cb, __FUNCTION__);
        }
        pthread_mutex_unlock(&data_cb->data_mutex);
    }

    for (i = 0; i < count; i++) {
        HAL_LOG_DEBUG("%s: handle %d, count=%d", __FUNCTION__, handle, count);
        curr_sample = &sample_list[i];
        la_sample.type = SENSOR_TYPE_PROXIMITY;

        if(bWakeUp == false) {
            la_sample.sensor = HANDLE_THRESH_NON_WAKE_UP;
            HAL_LOG_VERBOSE("%s:sensor %s ",__FUNCTION__,
                        Utility::SensorTypeToSensorString(getType()));
        } else {
            la_sample.sensor = HANDLE_THRESH;
            HAL_LOG_VERBOSE("%s:sensor %s (wake_up)",__FUNCTION__,
                            Utility::SensorTypeToSensorString(getType()));
        }

#ifndef HAL_SUPPORT_DISTANCE
        if (0 == curr_sample->data[0] * UNIT_CONVERT_Q16) {
            /* far */
            if (mSensor != NULL)
                la_sample.distance = mSensor->getMaxRange();
            else
                la_sample.distance = (float)(curr_sample->data[1]) * UNIT_CONVERT_PROXIMITY;
        }
        else {
            /* near */
            la_sample.distance = 0;
        }
#else
        la_sample.distance = (float)(curr_sample->data[1]) * UNIT_CONVERT_PROXIMITY;
#endif

        la_sample.version = sizeof(sensors_event_t);
        la_sample.timestamp = time_service->timestampCalc(
                        (uint64_t)curr_sample->timestamp, la_sample.sensor);
        /* Droping duplicates and report accurate expected count of samples to clients */
        if (thresh_enabled == true && ((!bComplySampleRate(thresh_last_event.timestamp, la_sample.timestamp)) ||
                (la_sample.distance == thresh_last_event.distance))) {
            HAL_LOG_DEBUG("%s: sensor(%s): Drop Duplicate Sample, HAL TS Diff:%" PRId64,
                                            __FUNCTION__, Utility::SensorTypeToSensorString(getType()),
                                            (la_sample.timestamp - thresh_last_event.timestamp));
            free(sample_list);
            return;
        }

        if (thresh_enabled == false) {
            thresh_enabled = true;
        }

        HAL_LOG_VERBOSE( "%s: prox data: %f %f %f SAM TS: %u HAL TS:%" PRId64 " elapsedRealtimeNano:%" PRId64 "",
                         __FUNCTION__, curr_sample->data[0], curr_sample->data[1], la_sample.distance,
                         curr_sample->timestamp, la_sample.timestamp, android::elapsedRealtimeNano() );
        /* backup the last sensor event */
        memcpy(&thresh_last_event, &la_sample, sizeof(sensors_event_t));

        pthread_mutex_lock(&data_cb->data_mutex);
        if (Utility::insertQueue(&la_sample, bWakeUp, data_cb)) {
            Utility::signalInd(data_cb);
        }
        pthread_mutex_unlock(&data_cb->data_mutex);
    }
    free(sample_list);
}
