/*============================================================================
  @file TimeSyncService.cpp

  @brief
  TimeSyncService class implementation.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#define __STDC_FORMAT_MACROS
#define __STDC_LIMIT_MACROS
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include <utils/SystemClock.h>

#include "TimeSyncService.h"
#include "Utility.h"
#include "Recovery.h"
#define TICK1_GTR_TICK2(t1,t2) (((t1) != (t2)) && ((t2==0) || (INT32_MAX >= (((uint32_t)t1) - ((uint32_t)t2)))))

TimeSyncService* TimeSyncService::self = NULL;

/*===========================================================================
                   PREPROCESSOR DEFINTIONS
===========================================================================*/
#define MAX_CLOCK_DIFF_NS 10000
#define MAX_CLOCK_DIFF_ATTEMPTS 5

/*============================================================================
  FUNCTION getTimeSyncService
    Get the TimeSyncService static object;
============================================================================*/
TimeSyncService* TimeSyncService::getTimeSyncService()
{
    if (NULL == self)
        self = new TimeSyncService;
    return self;
}
/*============================================================================
  FUNCTION getCb
    Get the TimeSyncService sensor1 handle;
============================================================================*/
hal_sensor1_cb_t* TimeSyncService::getCb()
{
    return sensor1_cb;
}

/*===========================================================================
  FUNCTION:  isTimeServiceStarted
    Return value
        bool : whether started or not yet
===========================================================================*/
bool TimeSyncService::isTimeServiceStarted()
{
    if (enabled)
        return true;

    return false;
}

/*============================================================================
  TimeSyncService Constructor
============================================================================*/
TimeSyncService::TimeSyncService()
    :enabled(0),
    timestamp_offset_apps_dsps(0),
    dsps_ts_last(0),
    dsps_rollover_cnt(0),
    dsps_rollover_cnt_rcv(0),
    boot_ts_last_rollover(0)
{
    int i;
    pthread_mutexattr_t attr;
    sensor1_error_e error;

    for(i = 0; i < MAX_NUM_SENSORS; i++) {
        timestamp_last_sent[i] = 0;
    }
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    sensor1_cb = new hal_sensor1_cb_t;
    if(sensor1_cb == NULL ) {
        HAL_LOG_ERROR("%s:sensor1_cb could not be allocated!", __FUNCTION__);
    }
    pthread_mutex_init(&sensor1_cb->cb_mutex, &attr);
    pthread_cond_init(&sensor1_cb->cb_cond, NULL);
    pthread_mutexattr_destroy(&attr);

    /* open the sensor1 connection for time_service */
    error = sensor1_open(&sensor1_cb->sensor1_handle, &time_service_sensor1_cb, (intptr_t)this);
    if(SENSOR1_SUCCESS != error) {
        HAL_LOG_ERROR("%s:sensor1 open failed for time_service!", __FUNCTION__);
    }

    HAL_LOG_VERBOSE("Sensor1 opened for time_service");
}

/*============================================================================
  TimeSyncService Destructor
============================================================================*/
TimeSyncService::~TimeSyncService()
{
    HAL_LOG_INFO("%s: closing time_service sensor1...", __FUNCTION__);
    sensor1_close(sensor1_cb->sensor1_handle);
    delete sensor1_cb;
    pthread_mutex_destroy(&sensor1_cb->cb_mutex);
    pthread_cond_destroy(&sensor1_cb->cb_cond);
}

/*===========================================================================
  FUNCTION:  timeServiceStart
    Start the time_service service
===========================================================================*/
int TimeSyncService::timeServiceStart()
{
    sensor1_msg_header_s msgHdr;
    sns_time_timestamp_req_msg_v02 *msg_ptr = NULL;
    int err = -1;
    sensor1_error_e error;

    pthread_mutex_lock(&sensor1_cb->cb_mutex);
    /* if the time_service is not started, start the time_service */
    if (0 == enabled) {
        error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                sizeof(sns_time_timestamp_req_msg_v02), (void**)&msg_ptr );
        if (SENSOR1_SUCCESS != error) {
            HAL_LOG_ERROR("%s: sensor1_alloc_msg_buf returned(get) %d",
                       __FUNCTION__, error);
            err = -1;
        }
        else {
            msgHdr.service_number = SNS_TIME2_SVC_ID_V01;
            msgHdr.msg_id = SNS_TIME_TIMESTAMP_REQ_V02;
            msgHdr.msg_size = sizeof(sns_time_timestamp_req_msg_v02);
            msgHdr.txn_id = 1;
            msg_ptr->reg_report_valid = true;
            msg_ptr->reg_report = true;

            sensor1_cb->is_resp_arrived = false;
            error = sensor1_write(sensor1_cb->sensor1_handle, &msgHdr, msg_ptr);
            if (SENSOR1_SUCCESS != error) {
                HAL_LOG_ERROR("%s: sensor1_write returned %d", __FUNCTION__, error);
                sensor1_free_msg_buf(sensor1_cb->sensor1_handle, msg_ptr);
                err = -1;
            }
            else if (false == Utility::waitForResponse(TIME_OUT_MS,
                                                    &sensor1_cb->cb_mutex,
                                                    &sensor1_cb->cb_cond,
                                                    &sensor1_cb->is_resp_arrived)) {
                HAL_LOG_ERROR("%s: ERROR: No response from request %d",
                        __FUNCTION__, SNS_TIME_TIMESTAMP_REQ_V02);
                err = -1;
            }
        }
    }
    enabled = 1;
    pthread_mutex_unlock(&sensor1_cb->cb_mutex);
    return err;
}

/*===========================================================================
  FUNCTION:  timeServiceStop
    Stop the time_service service
===========================================================================*/
int TimeSyncService::timeServiceStop()
{
    sensor1_msg_header_s msgHdr;
    sns_common_cancel_req_msg_v01 *msg_ptr = NULL;
    int err = -1;
    sensor1_error_e error;

    pthread_mutex_lock(&sensor1_cb->cb_mutex);
    /* if the time_service is started, stop the time_service */
    if (1 == enabled) {
        error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                sizeof(sns_common_cancel_req_msg_v01), (void**)&msg_ptr );
        if (SENSOR1_SUCCESS != error) {
            HAL_LOG_ERROR("%s: sensor1_alloc_msg_buf returned(get) %d",
                       __FUNCTION__, error);
            err = -1;
        }
        else {
            HAL_LOG_DEBUG("send stop time_service command");
            msgHdr.service_number = SNS_TIME2_SVC_ID_V01;
            msgHdr.msg_id = SNS_TIME_CANCEL_REQ_V02;
            msgHdr.msg_size = sizeof(sns_common_cancel_req_msg_v01);
            msgHdr.txn_id = 1;
            sensor1_cb->is_resp_arrived = false;
            error = sensor1_write(sensor1_cb->sensor1_handle, &msgHdr, msg_ptr);
            if (SENSOR1_SUCCESS != error) {
                HAL_LOG_ERROR("%s: sensor1_write returned %d", __FUNCTION__, error);
                sensor1_free_msg_buf(sensor1_cb->sensor1_handle, msg_ptr);
                err = -1;
            }
            else if (false == Utility::waitForResponse(TIME_OUT_MS,
                                                    &sensor1_cb->cb_mutex,
                                                    &sensor1_cb->cb_cond,
                                                    &sensor1_cb->is_resp_arrived)) {
                HAL_LOG_ERROR("%s: ERROR: No response from request %d",
                        __FUNCTION__, SNS_TIME_CANCEL_REQ_V02);
                err = -1;
            }
        }
    }
    enabled = 0;
    pthread_mutex_unlock(&sensor1_cb->cb_mutex);
    return err;
}

/*===========================================================================
  FUNCTION:  getTimeSyncServiceStatus
    Get the time_service status
    Return valuses
        1 : time_service is started
        0 : time_service is stopped
===========================================================================*/
int TimeSyncService::getTimeSyncServiceStatus()
{
    return enabled;
}

/*===========================================================================
  FUNCTION:  processTimeResp
    Process response message from the time sync service.
    Parameters
        @msg_hdr : sensor1 message header
        @msg_ptr : sensor1 message pointer
===========================================================================*/
void TimeSyncService::processTimeResp(sensor1_msg_header_s const *msg_hdr,
        sns_time_timestamp_resp_msg_v02 const *msg_ptr)
{
    UNREFERENCED_PARAMETER(msg_hdr);
    pthread_mutex_lock(&sensor1_cb->cb_mutex);
    if (0 == msg_ptr->resp.sns_result_t) {
        if(true == msg_ptr->dsps_rollover_cnt_valid) {
            rolloverCntIs(msg_ptr->dsps_rollover_cnt);
        }
        if (true == msg_ptr->timestamp_dsps_valid &&
            true == msg_ptr->timestamp_apps_boottime_valid) {
                tsOffsetIs(msg_ptr->timestamp_dsps, msg_ptr->timestamp_apps_boottime);
        }
        else if(true == msg_ptr->error_code_valid) {
            HAL_LOG_ERROR("%s: Error in RESP: %i", __FUNCTION__, msg_ptr->error_code);
        }
        else {
            HAL_LOG_ERROR("%s: Unknown error in RESP. DSPS ts valid: %i; APPS: %i APPS boottime: %i",
                     __FUNCTION__, msg_ptr->timestamp_dsps_valid, msg_ptr->timestamp_apps_valid,
                     msg_ptr->timestamp_apps_boottime_valid);
        }
    }
    else {
        HAL_LOG_ERROR("%s: Received 'Failed' in response result", __FUNCTION__);
    }

    sensor1_cb->is_resp_arrived = true;
    pthread_cond_signal(&sensor1_cb->cb_cond);
    pthread_mutex_unlock(&sensor1_cb->cb_mutex);
    return;
}

/*===========================================================================
  FUNCTION:  processCancelResp
    Process Cancer response to the time_service service.
===========================================================================*/
void TimeSyncService::processCancelResp()
{
        pthread_mutex_lock(&sensor1_cb->cb_mutex);
        sensor1_cb->is_resp_arrived = true;
        pthread_cond_signal(&sensor1_cb->cb_cond);
        pthread_mutex_unlock(&sensor1_cb->cb_mutex);
}

/*===========================================================================
  FUNCTION:  processTimeInd
    Process indication message from the time sync service.
    Parameters
        @msg_hdr : sensor1 message header
        @msg_ptr : sensor1 message pointer
===========================================================================*/
void TimeSyncService::processTimeInd(sensor1_msg_header_s const *msg_hdr,
    sns_time_timestamp_ind_msg_v02 const *msg_ptr)
{
    UNREFERENCED_PARAMETER(msg_hdr);
    rolloverCntIs(msg_ptr->dsps_rollover_cnt);
    if (msg_ptr->timestamp_apps_boottime_valid) {
        tsOffsetIs(msg_ptr->timestamp_dsps, msg_ptr->timestamp_apps_boottime);
    }
}

/*===========================================================================
  FUNCTION:  rolloverCntIs
    Update the rollover count
    parameters
        @rollover_cnt : the rollover count in the ssc side
===========================================================================*/
void TimeSyncService::rolloverCntIs(uint32_t rollover_cnt)
{
    pthread_mutex_lock(&sensor1_cb->cb_mutex);
    if (0 == dsps_rollover_cnt && 0 == dsps_ts_last) {
        dsps_rollover_cnt = rollover_cnt;
    }
    dsps_rollover_cnt_rcv = rollover_cnt;
    pthread_mutex_unlock(&sensor1_cb->cb_mutex);
}

/*===========================================================================
  FUNCTION:  tsOffsetIs
    Update local time stamp offset state.
    parameters
        @timestamp_ssc : the time stamp in the SSC side
        @timestamp_ap: the times tamp in the AS side
===========================================================================*/
void TimeSyncService::tsOffsetIs(uint32_t timestamp_ssc, uint64_t timestamp_ap)
{
    uint64_t ssc_ns;
    uint64_t elapsed_realtime_ns;
    struct timespec ts_start, ts_end;
    uint64_t boottime_ns, boottime_start, boottime_end;
    int attempts = MAX_CLOCK_DIFF_ATTEMPTS;
    uint64_t boottime_diff;
    uint64_t boottime_diff_best = MAX_CLOCK_DIFF_NS;
    uint64_t boottime_start_best = 0, boottime_end_best =0;

    /* Figure out the difference between the CLOCK_BOOTTIME time and
     * elapsedRealtimeNano */
    do {
        clock_gettime( CLOCK_BOOTTIME, &ts_start );
        elapsed_realtime_ns = android::elapsedRealtimeNano();
        clock_gettime( CLOCK_BOOTTIME, &ts_end );
        boottime_start = ts_start.tv_sec * NSEC_PER_SEC + ts_start.tv_nsec;
        boottime_end = ts_end.tv_sec * NSEC_PER_SEC + ts_end.tv_nsec;
        boottime_diff = boottime_end - boottime_start;
        if (attempts == (int)MAX_CLOCK_DIFF_ATTEMPTS && boottime_diff >= boottime_diff_best) {
            boottime_start_best = boottime_start;
            boottime_end_best = boottime_end;
            boottime_diff_best = boottime_diff;
        }
        else if( boottime_diff < boottime_diff_best ) {
            boottime_start_best = boottime_start;
            boottime_end_best = boottime_end;
            boottime_diff_best = boottime_diff;
        }
        HAL_LOG_DEBUG("%s: boottime_diff: %" PRId64 " boottime_diff_best: %" PRId64 " boottime_start_best: %" PRId64 " boottime_end_best: %" PRId64 "",
                      __FUNCTION__, boottime_diff, boottime_diff_best, boottime_start_best, boottime_end_best);

    } while ((boottime_diff > MAX_CLOCK_DIFF_NS) && (--attempts > 0));

    boottime_ns = (boottime_start_best + boottime_end_best) / 2;
    ssc_ns = (uint64_t)(((uint64_t)timestamp_ssc * NSEC_PER_SEC) / DSPS_HZ);
    timestamp_offset_apps_dsps = timestamp_ap - ssc_ns - boottime_ns + elapsed_realtime_ns;

    HAL_LOG_DEBUG("%s: Apps: %" PRIu64"; DSPS: %u; Offset : %" PRId64 " (diff %" PRId64 " @%d)",
                  __FUNCTION__,  timestamp_ap, timestamp_ssc, timestamp_offset_apps_dsps,
                  boottime_diff_best, attempts);
}

/*===========================================================================
  FUNCTION:  timestampCalc
    Converts the DSPS clock ticks from a sensor sample to a LA timestamp (ns
    since epoch).  Adjusts return value based on dsps timestamp rollover
    and makes minor adjustments to ensure sensor samples are sent with
    ascending timestamps.
    Parameters
        @dsps_timestamp : the time stamp in the sensor event
        @sensor_handle : one type of sensor handle
    Return value
        int64_t : the calculated timestamp
===========================================================================*/
int64_t TimeSyncService::timestampCalc(uint64_t dsps_timestamp, int sensor_handle)
{
    int32_t rollover_diff;
    struct timespec current_time;
    int64_t current_boot_ts;
    int time_err;
    int64_t rv = timestamp_offset_apps_dsps +
                ((dsps_timestamp * NSEC_PER_SEC) / DSPS_HZ);

    pthread_mutex_lock(&sensor1_cb->cb_mutex);

    if (((dsps_timestamp < TS_ROLLOVER_THRESH) &&
         (UINT32_MAX - dsps_ts_last < TS_ROLLOVER_THRESH)) &&
        TICK1_GTR_TICK2(dsps_timestamp,dsps_ts_last)) {

        /* If a roll-over is predicted, check the boottime timestamp of the last
        * predicted roll-over against the current boottime timestamp. If the
        * difference between the two times is not greater than
        * BOOT_TS_ROLLOVER_THRESH, then there most likely was just jitter in the
        * incoming DSPS timestamps instead of a real clock roll-over event. */
        time_err = clock_gettime(CLOCK_BOOTTIME, &current_time);
        if (0 != time_err) {
            time_err = errno;
            HAL_LOG_ERROR("%s: Error with clock_gettime: %i", __FUNCTION__, time_err);
        }
        else {
            current_boot_ts = ((int64_t)current_time.tv_sec * 1000000000) +
                                                        current_time.tv_nsec;
            HAL_LOG_WARN("%s: potential TS rollover detected. \
                        DSPS TS: %" PRIu64", last DSPS: %u, boot \
                        TS: %" PRIi64", last boot: %" PRIi64"",
                        __FUNCTION__, dsps_timestamp, dsps_ts_last,
                        current_boot_ts, boot_ts_last_rollover);

            if ((current_boot_ts - boot_ts_last_rollover) > BOOT_TS_ROLLOVER_THRESH) {
                /* If a roll-over has likely occurred */
                dsps_rollover_cnt++;
                /* Record the boottime timestamp */
                boot_ts_last_rollover = current_boot_ts;

                HAL_LOG_WARN( "%s: TS rollover confirmed. cnt: %u, rcv: %u",
                        __FUNCTION__, dsps_rollover_cnt,
                        dsps_rollover_cnt_rcv );
            }
        }
    }

    /* If the # of rollovers determined by the HAL is different than in the
    * last message received from the time service, adjust the timestamp accordingly */
    rollover_diff = dsps_rollover_cnt - dsps_rollover_cnt_rcv;
    if ((0 < rollover_diff && dsps_timestamp < TS_ROLLOVER_THRESH) ||
        (0 > rollover_diff && dsps_timestamp > UINT32_MAX - TS_ROLLOVER_THRESH)) {
        HAL_LOG_WARN("%s: Adjusting timestamp for rollover: %" PRIu64", %i",
            __FUNCTION__, rv, rollover_diff );
        rv += (rollover_diff * UINT32_MAX) * NSEC_PER_SEC / DSPS_HZ;
    }

    /* Ensure sensor samples are sent to LA with increasing timestamps */
    if (timestamp_last_sent[sensor_handle] > rv &&
        llabs(timestamp_last_sent[sensor_handle] - rv) < TS_CORRECT_THRESH) {
        HAL_LOG_WARN("%s: Adjusting timestamp to maintain ascension: %" PRIu64", %" PRIu64,
            __FUNCTION__, rv, timestamp_last_sent[sensor_handle]);
        rv = timestamp_last_sent[sensor_handle] + 1;
    }

    if (TICK1_GTR_TICK2(dsps_timestamp,dsps_ts_last)) {
        dsps_ts_last = dsps_timestamp;
    }

    timestamp_last_sent[sensor_handle] = rv;
    pthread_mutex_unlock(&sensor1_cb->cb_mutex);

    return rv;
}

/*===========================================================================
  FUNCTION:  time_service_sensor1_cb
    Parameters
        @cb_data : callback data pointer
        @msg_hdr : sensor1 message header
        @msg_type : sensor1 message type
        @msg_ptr : sensor1 message pointer
===========================================================================*/
void time_service_sensor1_cb(intptr_t cb_data, sensor1_msg_header_s *msg_hdr,
        sensor1_msg_type_e msg_type, void *msg_ptr)
{
    TimeSyncService *time_service = (TimeSyncService *)cb_data;
    hal_sensor1_cb_t  *timesync_sensor1_cb = time_service->getCb();

    HAL_LOG_DEBUG("%s: msg_type %d", __FUNCTION__, msg_type );
    if (msg_hdr != NULL) {
        HAL_LOG_DEBUG("%s: Sn %d, msg Id %d, txn Id %d", __FUNCTION__,
            msg_hdr->service_number, msg_hdr->msg_id, msg_hdr->txn_id);
    }
    else if (msg_type != SENSOR1_MSG_TYPE_BROKEN_PIPE &&
            msg_type != SENSOR1_MSG_TYPE_RETRY_OPEN ) {
            HAL_LOG_ERROR("%s: Error - invalid msg type with NULL msg_hdr: %u",
                __FUNCTION__, msg_type);
            return;
    }

    if (SENSOR1_MSG_TYPE_RESP == msg_type &&
        SNS_TIME_TIMESTAMP_RESP_V02 == msg_hdr->msg_id) {
        time_service->processTimeResp(msg_hdr, (sns_time_timestamp_resp_msg_v02*)msg_ptr);
    }
    else if (SENSOR1_MSG_TYPE_RESP == msg_type &&
        SNS_TIME_CANCEL_RESP_V02 == msg_hdr->msg_id) {
        time_service->processCancelResp();
    }
    else if (SENSOR1_MSG_TYPE_IND == msg_type) {
        time_service->processTimeInd(msg_hdr, (sns_time_timestamp_ind_msg_v02*)msg_ptr);
    }
    else if ( SENSOR1_MSG_TYPE_RETRY_OPEN == msg_type) {
        HAL_LOG_WARN("%s: SENSOR1_MSG_TYPE_RETRY_OPEN", __FUNCTION__);
        if (timesync_sensor1_cb != NULL) {
            pthread_mutex_lock(&timesync_sensor1_cb->cb_mutex);
            Recovery::reInit(timesync_sensor1_cb, &time_service_sensor1_cb, cb_data);
            pthread_mutex_unlock(&timesync_sensor1_cb->cb_mutex);
        }
        else {
            HAL_LOG_WARN("%s: timesync_sensor1_cb is NULL for RETRY_OPEN",
                            __FUNCTION__);
        }
    }
    else if (SENSOR1_MSG_TYPE_BROKEN_PIPE == msg_type) {
        HAL_LOG_WARN("%s: SENSOR1_MSG_TYPE_BROKEN_PIPE", __FUNCTION__);
        if (timesync_sensor1_cb != NULL) {
            pthread_mutex_lock(&timesync_sensor1_cb->cb_mutex);
            Recovery::handleBrokenPipe(timesync_sensor1_cb, &time_service_sensor1_cb, cb_data);
            pthread_mutex_unlock(&timesync_sensor1_cb->cb_mutex);
            if(time_service->isTimeServiceStarted()) {
                HAL_LOG_WARN("%s: starting time sync as it is enabled", __FUNCTION__);
                time_service->timeServiceStart();
            }
        }
        else {
            HAL_LOG_WARN("%s: timesync_sensor1_cb is NULL for BROKEN_PIPE",
                            __FUNCTION__);
        }
    }
    else {
        HAL_LOG_WARN("%s: Received unknown message type %i, id %i",
                    __FUNCTION__, msg_type, msg_hdr->msg_id);
    }
    /* free the callback message buf */
    if (timesync_sensor1_cb != NULL) {
        if (NULL != msg_ptr && timesync_sensor1_cb->sensor1_handle) {
            sensor1_free_msg_buf(timesync_sensor1_cb->sensor1_handle, msg_ptr);
        }
    }
    else {
        HAL_LOG_WARN("%s: timesync_sensor1_cb is NULL", __FUNCTION__);
    }

    return;
}
