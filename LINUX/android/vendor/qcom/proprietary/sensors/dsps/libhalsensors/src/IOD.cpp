/*============================================================================
  @file IOD.cpp

  @brief
  IOD class implementation.

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#ifndef WEAR_SUPPORT
#ifdef FEATURE_SNS_IOD

#include <cutils/properties.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include "TimeSyncService.h"
#include "IOD.h"

double latitude = 0;
double longitude = 0;

/*============================================================================
  IOD Constructor
============================================================================*/
IOD::IOD(int handle)
    :SAMSensor(handle)
{
    if(getAttribOK() == true) {
        svc_num = SNS_SAM_IOD_SVC_ID_V01;
        trigger_mode = SENSOR_MODE_EVENT;
        HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
        setName("IOD");
        setVendor("QTI");
        setType(SENSOR_TYPE_IOD);
        setFlags(SENSOR_FLAG_ON_CHANGE_MODE);
        setMaxRange(1);
        setResolution(1);

        /* Send Algo Attributes Request */
        sendAlgoAttribReq();
    }
}

/*============================================================================
  IOD Destructor
============================================================================*/
IOD::~IOD()
{
}

/*============================================================================
  setLocData
============================================================================*/
void IOD::setLocData(double mlat, double mlong)
{
    HAL_LOG_DEBUG("%s: Received Location Data mlat: %f mlong: %f",
                  __FUNCTION__, mlat, mlong);
    latitude = mlat;
    longitude = mlong;
    Utility::signalResponse(false, sensor1_cb);
}

/*============================================================================
  enable
============================================================================*/
int IOD::enable(int en)
{
    int err;
    sensor1_error_e error;
    sensor1_msg_header_s req_hdr;
    sns_sam_iod_enable_req_msg_v01 *sam_req;
    struct timeval tv;
    uint64_t sunrise_ts = 0, sunset_ts = 0;

    int64 tbf = 1000;
    float dist = 0;
    bool singleShot = true;

    if (enabled == en) {
        HAL_LOG_INFO("IOD is already enabled/disabled %d", enabled);
        return 0;
    }

    /* store the en value */
    enabled = en;
    HAL_LOG_DEBUG("%s: handle=%d", __FUNCTION__, handle);

    if (en) {
        pthread_mutex_lock(&sensor1_cb->cb_mutex);
        error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                                 sizeof(sns_sam_iod_enable_req_msg_v01),
                                 (void**)&sam_req );
        if (SENSOR1_SUCCESS != error) {
            HAL_LOG_ERROR("%s:sensor1_alloc_msg_buf error:%d", __FUNCTION__, error);
            pthread_mutex_unlock(&sensor1_cb->cb_mutex);
            enabled = 0;
            return -1;
        }

        req_hdr.service_number = svc_num;
        req_hdr.msg_id = SNS_SAM_IOD_ENABLE_REQ_V01;
        req_hdr.msg_size = sizeof(sns_sam_iod_enable_req_msg_v01);
        req_hdr.txn_id = 0;

        /* Operate in asynchronous mode */
        sam_req->report_period = 0;

        /* Request for Location data */
        pIzatManager = getIzatManager(osFramework);
        if ( pIzatManager != NULL ) {
            pIzatManager->subscribeListener(&osLocationListener);
            HAL_LOG_DEBUG("%s: Location subscribeListener done!", __FUNCTION__);
        } else {
            HAL_LOG_ERROR("%s: pIzatManager is NULL, can't get Location data, Exiting!", __FUNCTION__);
            pthread_mutex_unlock(&sensor1_cb->cb_mutex);
            enabled = 0;
            return -1;
        }

        HAL_LOG_VERBOSE("%s: Enable provider...", __FUNCTION__);
        pIzatManager->enableProvider(providerType);

        HAL_LOG_VERBOSE("%s: Adding Provider based location request...", __FUNCTION__);
        pIzatRequest = IzatRequest::createProviderRequest(providerType, tbf, dist, singleShot);
        pIzatManager->addRequest(pIzatRequest);

        /* waiting for response */
        sensor1_cb->error = false;
        sensor1_cb->is_resp_arrived = false;
        if (Utility::waitForResponse(TIME_OUT_MS_LOC_SER,
                                 &sensor1_cb->cb_mutex,
                                 &sensor1_cb->cb_cond,
                                 &sensor1_cb->is_resp_arrived) == false) {
            HAL_LOG_ERROR("%s: EXPECTED ERROR: No response from Location Listener!",
                          __FUNCTION__);
        }
        HAL_LOG_VERBOSE("%s: Removing location request...", __FUNCTION__);
        pIzatManager->removeRequest (pIzatRequest);
        HAL_LOG_VERBOSE("%s: Disabling provide...", __FUNCTION__);
        pIzatManager->disableProvider (providerType);
        HAL_LOG_VERBOSE("%s: Unsubscribing Listener...", __FUNCTION__);
        pIzatManager->unsubscribeListener (&osLocationListener);

        HAL_LOG_DEBUG("%s: latitude: %f longitude: %f",
                      __FUNCTION__, latitude, longitude);

        gettimeofday(&tv, NULL);
        Utility::getSunriseSunsetTimes(latitude, longitude, sunrise_ts, sunset_ts);
        HAL_LOG_DEBUG("%s: ts:%ld sunrise_ts:%" PRId64 " sunset_ts:%" PRId64 "",
                      __FUNCTION__, tv.tv_sec, sunrise_ts, sunset_ts);

        /* UTC timestamp in secs from Unix epoch */
        sam_req->timeofday = tv.tv_sec;
        /* UTC sunset/sunrise timestamps in secs from Unix epoch */
        sam_req->sunset_sunrise_ts.sunset_ts = sunset_ts;
        sam_req->sunset_sunrise_ts.sunrise_ts = sunrise_ts;

        /* set default behavior for indications during suspend */
        sam_req->notify_suspend_valid = true;
        sam_req->notify_suspend.proc_type = SNS_PROC_APPS_V01;
        sam_req->notify_suspend.send_indications_during_suspend = false;

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
  FUNCTION:  processResp
===========================================================================*/
void IOD::processResp(sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    const sns_common_resp_s_v01*  crsp_ptr = (sns_common_resp_s_v01 *)msg_ptr;
    bool                          error = false;

    HAL_LOG_INFO("%s: handle:%d %d", __FUNCTION__, handle, msg_hdr->msg_id);
    if (crsp_ptr->sns_result_t != 0 &&
        msg_hdr->msg_id != SNS_SAM_IOD_CANCEL_RESP_V01) {
        HAL_LOG_ERROR("%s: Msg %i; Result: %u, Error: %u", __FUNCTION__,
                    msg_hdr->msg_id, crsp_ptr->sns_result_t, crsp_ptr->sns_err_t);
        error = true;
    }

    if(true != error) {
        switch (msg_hdr->msg_id) {
        case SNS_SAM_IOD_ENABLE_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_IOD_ENABLE_RESP_V01", __FUNCTION__);
            instance_id = ((sns_sam_iod_enable_resp_msg_v01 *)msg_ptr)->instance_id;
            break;
        case SNS_SAM_IOD_CANCEL_RESP_V01:
        case SNS_SAM_IOD_DISABLE_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_IOD_CANCEL/DISABLE_RESP_V01", __FUNCTION__);
            /* Reset instance ID */
            instance_id = 0xFF;
            break;
        case SNS_SAM_IOD_GET_ATTRIBUTES_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_IOD_GET_ATTRIBUTES_RESP_V01", __FUNCTION__);
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
void IOD::processInd(sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    sns_sam_iod_report_ind_msg_v01*  sam_iod_rpt_ptr =
                                     (sns_sam_iod_report_ind_msg_v01*) msg_ptr;
    bool                             error = false;
    uint32_t                         timestamp;
    sensors_event_t                  sensor_data;

    HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
    memset(&sensor_data, 0, sizeof(sensors_event_t));
    switch(msg_hdr->msg_id) {
        case SNS_SAM_IOD_REPORT_IND_V01:
            HAL_LOG_VERBOSE("%s: Output Class: %d Unknown Reason: 0x0%x Confidence: %f ts: %u",
                          __FUNCTION__, sam_iod_rpt_ptr->report_data.output_class,
                          (unsigned int)sam_iod_rpt_ptr->report_data.reason_unknown,
                          sam_iod_rpt_ptr->report_data.confidence, sam_iod_rpt_ptr->timestamp);
            sensor_data.type = SENSOR_TYPE_IOD;
            sensor_data.sensor = HANDLE_IOD;
            sensor_data.data[0] = sam_iod_rpt_ptr->report_data.output_class;
            timestamp = sam_iod_rpt_ptr->timestamp;
            break;
        case SNS_SAM_IOD_ERROR_IND_V01:
            HAL_LOG_ERROR("%s: SNS_SAM_IOD_ERROR_IND_V01", __FUNCTION__);
            error = true;
            break;
        default:
            HAL_LOG_ERROR("%s: Unknown message ID = %d", __FUNCTION__, msg_hdr->msg_id);
            error = true;
            break;
    }

    /* No error */
    if (error == false) {
        sensor_data.version = sizeof(sensors_event_t);
        sensor_data.timestamp = time_service->timestampCalc(
                (uint64_t)timestamp, sensor_data.sensor);

        HAL_LOG_DEBUG("%s: IOD: Output Class: %f SAM TS: %u HAL TS:%" PRId64 " elapsedRealtimeNano:%" PRId64 "",
                      __FUNCTION__, sensor_data.data[0], timestamp, sensor_data.timestamp,
                      android::elapsedRealtimeNano());
        pthread_mutex_lock(&data_cb->data_mutex);
        if (Utility::insertQueue(&sensor_data, bWakeUp, data_cb)) {
            Utility::signalInd(data_cb);
        }
        pthread_mutex_unlock(&data_cb->data_mutex);
    }
}

#endif /* FEATURE_SNS_IOD */
#endif /* WEAR_SUPPORT */
