/*============================================================================
  @file StepCounter.cpp

  @brief
  StepCounter class implementation.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#include <cutils/properties.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "TimeSyncService.h"
#include "StepCounter.h"

/*============================================================================
  StepCounter Constructor
============================================================================*/
StepCounter::StepCounter(int handle)
    :SAMSensor(handle),
    step_counter_running_total(0),
    step_counter_running_instance(0),
    step_counter_current_instance(0),
    is_batch_manage_timer_created(false),
    is_report_timer_created(false)
{
    if(getAttribOK() == true) {
        batch_support = true;
        svc_num = SNS_SAM_PED_SVC_ID_V01;
        trigger_mode = SENSOR_MODE_EVENT;
        HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
        setName("Step Counter");
        setVendor("QTI");
        setType(SENSOR_TYPE_STEP_COUNTER);

        if(handle == HANDLE_SAM_STEP_COUNTER_WAKE_UP) {
            bWakeUp = true;
            setName("Step Counter -Wakeup");
            setFlags(SENSOR_FLAG_ON_CHANGE_MODE|SENSOR_FLAG_WAKE_UP);
        } else if(handle == HANDLE_SAM_STEP_COUNTER) {
            bWakeUp = false;
            setFlags(SENSOR_FLAG_ON_CHANGE_MODE);
        }

        setMaxRange(1);
        setResolution(1);

        /* Send Algo Attributes Request */
        sendAlgoAttribReq();
    }
}

/*============================================================================
  StepCounter Destructor
============================================================================*/
StepCounter::~StepCounter()
{

}

/*===========================================================================
  setSensorInfo
===========================================================================*/
void StepCounter::setSensorInfo()
{
    HAL_LOG_INFO("%s: handle=%d", __FUNCTION__, handle);
}

/*===========================================================================
  manageBatch
  If the activity is stopped, switch to async mode
===========================================================================*/
void  StepCounter::manageBatch( sigval_t val )
{
    struct itimerspec timer;
    float batch_rate_hz = 0;
    int batch_period = 0;
    StepCounter *sc_handle = (StepCounter *)val.sival_ptr;

    if ( sc_handle->enabled ) {
        batch_rate_hz = FX_FIXTOFLT_Q16( sc_handle->batch_rate );
        if( batch_rate_hz != 0 ) {
            batch_period = (int)lroundf(1.0/batch_rate_hz);
        }
        clock_gettime( CLOCK_BOOTTIME, &sc_handle->ts_poll );
        if( batch_period > 0 ) {
            timer.it_value.tv_sec = sc_handle->ts_poll.tv_sec + batch_period;
            timer.it_value.tv_nsec = sc_handle->ts_poll.tv_nsec;
            timer.it_interval.tv_sec = 0;
            timer.it_interval.tv_nsec = 0;
            timer_settime( sc_handle->batch_manage_timer, TIMER_ABSTIME, &timer, NULL );
        }

        if ( (batch_period > 0) && (sc_handle->ts_event.tv_sec != 0) &&
            (sc_handle->step_counter_running_instance == sc_handle->prev_stepcount) &&
            ((sc_handle->ts_poll.tv_sec - sc_handle->ts_event.tv_sec ) > batch_period))  {
            sc_handle->ts_event.tv_sec = 0;
            /* Switch to Async mode */
            sc_handle->asyncmode = true;
            sc_handle->sendBatchReq();
        }
        else {
            sc_handle->prev_stepcount = sc_handle->step_counter_running_instance;
        }
    }
}

/*============================================================================
  initBatchManageTimer
============================================================================*/
int StepCounter::initBatchManageTimer(void)
{
    struct sigevent sigev;
    struct itimerspec timer;
    int ret = 0;

    prev_stepcount = 0;
    memset (&sigev, 0, sizeof (struct sigevent));
    sigev.sigev_value.sival_int = 0;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_notify_attributes = NULL;
    sigev.sigev_value.sival_ptr = (void*) this;
    sigev.sigev_notify_function = manageBatch;
    if( timer_create (CLOCK_BOOTTIME, &sigev, &batch_manage_timer)) {
        HAL_LOG_ERROR("%s:timer_create failed", __FUNCTION__);
        ret= -1;
    } else {
        is_batch_manage_timer_created = true;
        clock_gettime( CLOCK_BOOTTIME, &ts_event );
        timer.it_value.tv_sec = ts_event.tv_sec;
        timer.it_value.tv_nsec = ts_event.tv_nsec;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_nsec = 0;
        timer_settime( batch_manage_timer, TIMER_ABSTIME, &timer, NULL );
    }
    return ret;
}

/*===========================================================================
  reportLatestEvent
  Report latest count if it has not been reported.
===========================================================================*/
void StepCounter::reportLatestEvent( sigval_t val )
{
    StepCounter *sc_handle = (StepCounter *)val.sival_ptr;
    hal_sam_sample_t *curr_sample = &sc_handle->latest_sample;

    HAL_LOG_VERBOSE("%s: step_counter_current_instance=%" PRIu64" current sample data=%f",
        __FUNCTION__ , sc_handle->step_counter_current_instance, curr_sample->data[0]);

    if ((sc_handle->step_counter_current_instance != curr_sample->data[0]) &&
        sc_handle->is_report_timer_created) {
        sc_handle->reportSingleEvent(curr_sample);
    }
}


/*============================================================================
  initReportTimer
============================================================================*/
int StepCounter::initReportTimer(float report_delay_sec)
{
    struct sigevent sigev;
    struct itimerspec timer;
    int ret = 0;

    HAL_LOG_VERBOSE("%s: reportDelay=%f second, ceil=%u" ,
                            __FUNCTION__, report_delay_sec,
                            (unsigned int)ceil(report_delay_sec));

    prev_stepcount = 0;
    memset (&sigev, 0, sizeof (struct sigevent));
    sigev.sigev_value.sival_int = 0;
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_notify_attributes = NULL;
    sigev.sigev_value.sival_ptr = (void*) this;
    sigev.sigev_notify_function = reportLatestEvent;
    if( timer_create (CLOCK_BOOTTIME, &sigev, &report_timer)) {
        HAL_LOG_ERROR("%s:report timer create failed", __FUNCTION__);
        ret= -1;
    } else {
        is_report_timer_created = true;
        clock_gettime( CLOCK_BOOTTIME, &ts_event );
        /* Round upward to second to minimize resource usage */
        timer.it_value.tv_sec = ts_event.tv_sec + (unsigned int)ceil(report_delay_sec);
        timer.it_value.tv_nsec = ts_event.tv_nsec;
        timer.it_interval.tv_sec = ceil(report_delay_sec);
        timer.it_interval.tv_nsec = 0;
        timer_settime( report_timer, TIMER_ABSTIME, &timer, NULL );
    }
    return ret;
}

/*============================================================================
  enable
============================================================================*/
int StepCounter::enable(int en)
{
    int err;
    sensor1_error_e error;
    sensor1_msg_header_s req_hdr;
    sns_sam_ped_enable_req_msg_v01 *sam_req;
    step_counter_current_instance = -1;

    if (enabled == en) {
        HAL_LOG_INFO("StepCounter is already enabled/disabled %d", enabled);
        return 0;
    }

    /* store the en value */
    enabled = en;
    HAL_LOG_DEBUG("%s: handle=%d, freq=%f report_rate=%d batch_rate=%u \
                    batched=%d wakeup %d",__FUNCTION__, handle, freq,
                    report_rate, batch_rate, batching, bWakeUp);

    if (en) {
        pthread_mutex_lock(&sensor1_cb->cb_mutex);
        error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                                 sizeof(sns_sam_ped_enable_req_msg_v01),
                                 (void**)&sam_req );
        if (SENSOR1_SUCCESS != error) {
            HAL_LOG_ERROR("%s:sensor1_alloc_msg_buf error:%d", __FUNCTION__, error);
            pthread_mutex_unlock(&sensor1_cb->cb_mutex);
            enabled = 0;
            return -1;
        }

        req_hdr.service_number = svc_num;
        req_hdr.msg_id = SNS_SAM_PED_ENABLE_REQ_V01;
        req_hdr.msg_size = sizeof(sns_sam_ped_enable_req_msg_v01);
        req_hdr.txn_id = 0;

        /* asynchronous reporting */
        sam_req->report_period = 0;

        /* set default behaviour for indications during suspend */
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
        HAL_LOG_DEBUG("%s: Received Response: %d",  __FUNCTION__, sensor1_cb->error);
        pthread_mutex_unlock(&sensor1_cb->cb_mutex);

        /* Get the last report data after enabling the sensor */
        err = sendGetReportReq(false);
        if (err) {
            HAL_LOG_ERROR("Get the last sensor data return error!");
        }

        /* Send batch request */
        if( batching ) {
            asyncmode = false;
            if ( sendBatchReq() ) {
                HAL_LOG_ERROR("%s: SendBatchReq failed", __FUNCTION__);
                sendCancel();
                enabled = 0;
                return -1;
            }
            if ( initBatchManageTimer() ) {
                sendCancel();
                HAL_LOG_ERROR("%s: Timer init failed", __FUNCTION__);
                enabled = 0;
                return -1;
            }
        }

        if(initReportTimer(HZ_TO_SEC(freq))) {
            HAL_LOG_ERROR("%s: initReportTimer failed", __FUNCTION__);
            sendCancel();
            if(is_batch_manage_timer_created) {
                timer_delete(batch_manage_timer);
            }
            enabled = 0;
            return -1;
        }
    } else {
        /* Disable sensor */
        if( is_report_timer_created ) {
            if (timer_delete (report_timer) < 0) {
                HAL_LOG_ERROR(" %s:report timer delete failed", __FUNCTION__);
            } else {
                is_report_timer_created = false;
            }
        }
        /*Get the samples present*/
        pthread_mutex_lock(&sensor1_cb->cb_mutex);
        /* Get the last report data before disabling the sensor */
            HAL_LOG_DEBUG("sendGetReportReq before cancel!");
        err = sendGetReportReq(true);
        if (err) {
            HAL_LOG_ERROR("sendGetReportReq reported error during cancel!");
        }

        /* waiting for response */
        if (Utility::waitForResponse(TIME_OUT_MS,
                                 &sensor1_cb->cb_mutex,
                                 &sensor1_cb->cb_cond,
                                 &sensor1_cb->is_resp_arrived) == false) {
            HAL_LOG_ERROR("%s: ERROR: No response from sendGetReportReq request",
                               __FUNCTION__);
            sensor1_cb->error = true;
        }
        /*No need to return as main purpose is to disable the sensor , mark the error in log
                 and move forward it can lead some steps missing but disable has to happen*/
        pthread_mutex_unlock(&sensor1_cb->cb_mutex);

        HAL_LOG_DEBUG("%s: Disabling sensor handle=%d", __FUNCTION__, handle);
        sendCancel();
        if( is_batch_manage_timer_created ) {
            if (timer_delete (batch_manage_timer) < 0) {
                HAL_LOG_ERROR(" %s:batch manage timer delete fail", __FUNCTION__);
            } else {
                is_batch_manage_timer_created = false;
            }
        }
    };
    return 0;
}

/*============================================================================
  sendBatchReq
============================================================================*/
int StepCounter::sendBatchReq()
{
    sensor1_error_e  error;
    sensor1_msg_header_s req_hdr;
    sns_sam_ped_batch_req_msg_v01 *sam_req;
    float batch_rate_in_hz;

    HAL_LOG_DEBUG( "%s: batching: %d, batch_rate=%f (Hz)",
                   __FUNCTION__, batching, FX_FIXTOFLT_Q16( batch_rate ));
    ts_event.tv_sec = 0;

    error = sensor1_alloc_msg_buf( sensor1_cb->sensor1_handle,
                                   sizeof(sns_sam_ped_batch_req_msg_v01),
                                   (void**)&sam_req );
    if( SENSOR1_SUCCESS != error ) {
        HAL_LOG_ERROR( "%s: sensor1_alloc_msg_buf() error: %d", __FUNCTION__, error );
      return -1;
    }
    req_hdr.service_number = svc_num;
    req_hdr.msg_id = SNS_SAM_PED_BATCH_REQ_V01;
    req_hdr.msg_size = sizeof( sns_sam_ped_batch_req_msg_v01 );
    req_hdr.txn_id = TXN_ID_NO_RESP_SIGNALLED;

    sam_req->instance_id = instance_id;

    sam_req->req_type_valid = true;
    sam_req->req_type = bWakeUp ? SNS_BATCH_WAKE_UPON_FIFO_FULL_V01: SNS_BATCH_NO_WAKE_UP_V01;

    /* convert batch rate from Hz in Q16 to batch period in seconds in Q16 */
    if (true == asyncmode) {
        sam_req->batch_period = 0 ;  /* Rate of 0 means disabled */
        asyncmode = false;
    }
    else {
        batch_rate_in_hz = FX_FIXTOFLT_Q16( batch_rate );
        sam_req->batch_period = batching ?
            FX_FLTTOFIX_Q16( 1.0 / batch_rate_in_hz ) :
            0 ;  /* Rate of 0 means disabled */
    }

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
void StepCounter::processResp(sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    const sns_common_resp_s_v01*  crsp_ptr = (sns_common_resp_s_v01 *)msg_ptr;
    bool                          error = false;

    HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
    if (crsp_ptr->sns_result_t != 0) {
        HAL_LOG_ERROR("%s: Msg %i; Result: %u, Error: %u", __FUNCTION__,
                    msg_hdr->msg_id, crsp_ptr->sns_result_t, crsp_ptr->sns_err_t);
        error = true;

        if (msg_hdr->msg_id == SNS_SAM_PED_BATCH_RESP_V01 &&
            (crsp_ptr->sns_err_t == SENSOR1_EBAD_SVC_ID ||
            crsp_ptr->sns_err_t == SENSOR1_EUNKNOWN)) {
            /* Proceed if batching is not supported */
            error = false;
        }
    }

    if (SNS_SAM_PED_CANCEL_RESP_V01 == msg_hdr->msg_id ||
        SNS_SAM_PED_DISABLE_RESP_V01 == msg_hdr->msg_id) {
        /* Note, it's possible that we continue to receive/process indications after this.
        * However, they will update the running_instance, and should not be lost in
        * subsequent iterations */
        step_counter_running_total = step_counter_running_instance;
    }

    if(true != error ) {
        switch (msg_hdr->msg_id) {
        case SNS_SAM_PED_ENABLE_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_PED_ENABLE_RESP_V01", __FUNCTION__);
            instance_id = ((sns_sam_ped_enable_resp_msg_v01 *)msg_ptr)->instance_id;
            break;
        case SNS_SAM_PED_GET_REPORT_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_PED_GET_REPORT_RESP_V01", __FUNCTION__);
            processInd(msg_hdr, msg_ptr);
            break;
        case SNS_SAM_PED_CANCEL_RESP_V01:
        case SNS_SAM_PED_DISABLE_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_PED_CANCEL/DISABLE_RESP_V01", __FUNCTION__);
            /* Reset instance ID */
            instance_id = 0xFF;
            break;
        case SNS_SAM_PED_GET_ATTRIBUTES_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_PED_GET_ATTRIBUTES_RESP_V01", __FUNCTION__);
            processAlgoAttribResp(msg_hdr, msg_ptr);
            break;
        case SNS_SAM_PED_BATCH_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_PED_BATCH_RESP_V01", __FUNCTION__);
            if( ((sns_sam_ped_batch_resp_msg_v01*)msg_ptr)->max_batch_size_valid )
            {
                int max_samples = ((sns_sam_ped_batch_resp_msg_v01*)msg_ptr)->max_batch_size;
                max_buffered_samples = max_samples;
            }
            pthread_mutex_lock(&data_cb->data_mutex);
            flushSendCmplt();
            pthread_mutex_unlock(&data_cb->data_mutex);
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
  FUNCTION:  reportSingleEvent
===========================================================================*/
void StepCounter::reportSingleEvent(hal_sam_sample_t *curr_sample)
{
    sensors_event_t la_sample;
    uint64_t steps = 0;

    /* As we update all sensors associated with an algo when SAM sends a response,
         * step counter needs a special handling to avoid spurious events generated
         * from step detector or pedometer when either of them are registered/de-registered.
         */
    step_counter_current_instance = curr_sample->data[0];
    steps = step_counter_running_total + step_counter_current_instance;
    step_counter_running_instance = steps;
    la_sample.type = SENSOR_TYPE_STEP_COUNTER;

    if(bWakeUp == false) {
        la_sample.sensor = HANDLE_SAM_STEP_COUNTER;
        HAL_LOG_VERBOSE("%s:sensor %s ",__FUNCTION__,
                    Utility::SensorTypeToSensorString(getType()));
    } else {
        la_sample.sensor = HANDLE_SAM_STEP_COUNTER_WAKE_UP;
        HAL_LOG_VERBOSE("%s:sensor %s (wake_up)",__FUNCTION__,
                        Utility::SensorTypeToSensorString(getType()));
    }
    la_sample.u64.step_counter = steps;
    la_sample.version = sizeof(sensors_event_t);
    if( step_counter_current_instance == 0) {
    /* Step count didn't change.
     * Use present time to make sure timestamp is larger than previous event timestamp.
     */
        la_sample.timestamp = android::elapsedRealtimeNano();
    } else {
        la_sample.timestamp = time_service->timestampCalc((uint64_t)curr_sample->timestamp, la_sample.sensor);
    }

    HAL_LOG_VERBOSE("%s: STEP COUNTER: steps %" PRIu64", running_total %" PRIu64" SAM TS: %u HAL TS:%" PRId64 " elapsedRealtimeNano:%" PRId64 "",
                    __FUNCTION__,
                    steps,
                    step_counter_running_total,
                    curr_sample->timestamp,
                    la_sample.timestamp,
                    android::elapsedRealtimeNano());

    pthread_mutex_lock(&data_cb->data_mutex);
    if (Utility::insertQueue(&la_sample, bWakeUp, data_cb)) {
        Utility::signalInd(data_cb);
    }
    pthread_mutex_unlock(&data_cb->data_mutex);
}


/*===========================================================================
  FUNCTION:  processInd
===========================================================================*/
void StepCounter::processInd(sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    hal_sam_sample_t *sample_list = NULL;
    hal_sam_sample_t *curr_sample;
    uint32_t i = 0;
    uint32_t count = 0;

    HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
    if (SNS_SAM_PED_REPORT_IND_V01 == msg_hdr->msg_id) {
        HAL_LOG_DEBUG("%s: SNS_SAM_PED_REPORT_IND_V01", __FUNCTION__);
        if( batching ) {
            /* Switch from Async state to batching mode*/
            if (sendBatchReq()) {
                HAL_LOG_ERROR("%s: sendBatchReq failed!", Utility::SensorTypeToSensorString(getType()));
            }
        }
        sns_sam_ped_report_ind_msg_v01* sam_ind =
            (sns_sam_ped_report_ind_msg_v01*)msg_ptr;

        sample_list = (hal_sam_sample_t *)malloc(sizeof(hal_sam_sample_t));
        if (NULL == sample_list) {
            HAL_LOG_ERROR( "%s: Malloc error", __FUNCTION__ );
        } else {
            count = 1;
            sample_list->data[0] = sam_ind->report_data.step_count;
            sample_list->data[1] = sam_ind->report_data.step_rate;
            sample_list->data[2] = sam_ind->report_data.step_confidence;
            sample_list->data[3] = sam_ind->report_data.step_event;
            sample_list->data[4] = sam_ind->report_data.step_count_error;
            sample_list->accuracy = 0;
            sample_list->timestamp = sam_ind->timestamp;
        }
    }
    else if (SNS_SAM_PED_GET_REPORT_RESP_V01 == msg_hdr->msg_id) {
        HAL_LOG_DEBUG("%s: SNS_SAM_PED_GET_REPORT_RESP_V01", __FUNCTION__);
        sns_sam_ped_get_report_resp_msg_v01 *sam_ind =
            (sns_sam_ped_get_report_resp_msg_v01 *)msg_ptr;

        if (sam_ind->timestamp_valid && sam_ind->report_data_valid) {
            sample_list = (hal_sam_sample_t *)malloc( sizeof(hal_sam_sample_t) );
            if (NULL == sample_list) {
                HAL_LOG_ERROR( "%s: Malloc error", __FUNCTION__ );
            } else {
                count = 1;
                sample_list->data[0] = sam_ind->report_data.step_count;
                sample_list->data[1] = sam_ind->report_data.step_rate;
                sample_list->data[2] = sam_ind->report_data.step_confidence;
                sample_list->data[3] = 0; /* We don't want to generate extra step detector events */
                sample_list->data[4] = sam_ind->report_data.step_count_error;
                sample_list->accuracy = 0;
                sample_list->timestamp = sam_ind->timestamp;
            }
        }
        else {
            HAL_LOG_WARN("%s: Received report with invalid data", __FUNCTION__);
        }
    }
    else if(SNS_SAM_PED_BATCH_IND_V01 == msg_hdr->msg_id) {
        HAL_LOG_DEBUG("%s: SNS_SAM_PED_BATCH_IND_V01", __FUNCTION__);
        sns_sam_ped_batch_ind_msg_v01* sam_ind =
            (sns_sam_ped_batch_ind_msg_v01*)msg_ptr;

        sample_list = (hal_sam_sample_t *)malloc(sam_ind->items_len * sizeof(hal_sam_sample_t));
        if (NULL == sample_list) {
            HAL_LOG_ERROR( "%s: Malloc error", __FUNCTION__ );
        }
        else {
            curr_sample = sample_list;

            clock_gettime( CLOCK_BOOTTIME, &ts_event );
            ts_poll.tv_sec = ts_event.tv_sec;
            for(i = 0; i < sam_ind->items_len; i++) {
                curr_sample->data[0] = sam_ind->items[ i ].report.step_count;
                curr_sample->data[1] = sam_ind->items[ i ].report.step_rate;
                curr_sample->data[2] = sam_ind->items[ i ].report.step_confidence;
                curr_sample->data[3] = sam_ind->items[ i ].report.step_event;
                curr_sample->data[4] = sam_ind->items[ i ].report.step_count_error;
                curr_sample->accuracy = 0;
                curr_sample->timestamp = sam_ind->items[ i ].timestamp;
            }
            /* Report only last event*/
            count = 1;
            curr_sample++;
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

        memcpy(&latest_sample, curr_sample, sizeof(hal_sam_sample_t));

        /* Report first event after sensor enabled and update count change if GetReport is called */
        if ((step_counter_running_instance == 0) ||
            (SNS_SAM_PED_GET_REPORT_RESP_V01 == msg_hdr->msg_id &&
            step_counter_current_instance != curr_sample->data[0])) {
            reportSingleEvent(curr_sample);
        }
    }
    free(sample_list);
}

/*===========================================================================
  FUNCTION:  sendGetReportReq
    Get the last report data after enabling the sensor.
===========================================================================*/
int StepCounter::sendGetReportReq(bool bRespRequired)
{
    sensor1_error_e error;
    sensor1_msg_header_s req_hdr;
    sns_sam_ped_get_report_req_msg_v01 *sam_req;

    HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
    error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                                  sizeof(sns_sam_ped_get_report_req_msg_v01),
                                  (void**)&sam_req);
    if (SENSOR1_SUCCESS != error) {
        HAL_LOG_ERROR( "%s: sensor1_alloc_msg_buf() error: %d", __FUNCTION__, error );
        return -1;
    }
    req_hdr.service_number = svc_num;
    req_hdr.msg_id = SNS_SAM_PED_GET_REPORT_REQ_V01;
    req_hdr.msg_size = sizeof(sns_sam_ped_get_report_req_msg_v01);
    req_hdr.txn_id = (bRespRequired==true)?0:TXN_ID_NO_RESP_SIGNALLED;
    sam_req->instance_id = instance_id;
    /* Send Request */
    sensor1_cb->error = false;

    error = sensor1_write(sensor1_cb->sensor1_handle, &req_hdr, sam_req);
    if (error != SENSOR1_SUCCESS) {
        /* free the message buffer */
        sensor1_free_msg_buf(sensor1_cb->sensor1_handle, sam_req);
        HAL_LOG_ERROR( "%s: sensor1_write() error: %d", __FUNCTION__, error );
        return -1;
    }
    return sensor1_cb->error ? -1 : 0;
}
