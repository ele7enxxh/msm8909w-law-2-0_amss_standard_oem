/*============================================================================
  @file SAMSensor.cpp

  @brief
  SAMSensor class implementation.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#include "SAMSensor.h"
#include "Recovery.h"
#include "Utility.h"
#include "TimeSyncService.h"

/*============================================================================
  SAMSensor Constructor
============================================================================*/
SAMSensor::SAMSensor(int handle)
    :Sensor(handle),
     instance_id(0),
     batch_rate(0),
     batch_support(0)
{
    sensor1_error_e err;
    time_service = TimeSyncService::getTimeSyncService();
    data_cb = Utility::getDataCb();

    sensor1_cb = new hal_sensor1_cb_t;
    if (sensor1_cb == NULL) {
        HAL_LOG_ERROR("%s:sensor(%s) malloc sensor1_cb error!",
                __FUNCTION__,Utility::SensorTypeToSensorString(getType()));
        return;
    }
    memset(sensor1_cb, 0, sizeof(*sensor1_cb));
    sensor1_cb->error = false;
    sensor1_cb->is_resp_arrived = false;
    /* init the mutex and cond for sensor1_handle */
    pthread_mutex_init(&(sensor1_cb->cb_mutex), NULL);
    pthread_cond_init(&(sensor1_cb->cb_cond), NULL);

    /* open sensor1 connection for SAM Sensor */
    err = sensor1_open(&sensor1_cb->sensor1_handle, &SAMSensor_sensor1_cb, (intptr_t)this);
    if (SENSOR1_SUCCESS != err) {
        HAL_LOG_ERROR("%s: sensor1 open failed for %s!", __FUNCTION__,
                      Utility::SensorTypeToSensorString(getType()));
        setAttribOK(false);
    } else {
        setAttribOK(true);
        report_rate = (int)lroundf(calcSampleRate(0));
    }
    HAL_LOG_INFO("%s: sensor(%s) is_attrib_ok=%d", __FUNCTION__,
                 Utility::SensorTypeToSensorString(getType()), getAttribOK());

    /* Disable periodic requests for QFusion sensors by default */
    property_get(HAL_PROP_QFUSION_USE_REPORT_PERIOD, qfusion_use_report_period, "false");
}

/*============================================================================
  SAMSensor Destructor
============================================================================*/
SAMSensor::~SAMSensor()
{
    HAL_LOG_INFO("%s: Sensor(%s) handle %d", __FUNCTION__,
                 Utility::SensorTypeToSensorString(getType()), handle);

    /* free up resources */
    if(sensor1_cb != NULL) {
        HAL_LOG_WARN("%s: closing sensor1 connection for sensor %s handle %d",
                     __FUNCTION__, Utility::SensorTypeToSensorString(getType()), handle);
        /* close sensor1 */
        sensor1_close(sensor1_cb->sensor1_handle);

        /* clean up mutex and cond var  */
        pthread_mutex_destroy(&sensor1_cb->cb_mutex );
        pthread_cond_destroy(&sensor1_cb->cb_cond );

        /* free up memory */
        delete sensor1_cb;
    }
}

/*===========================================================================
  setSensorInfo
===========================================================================*/
void SAMSensor::setSensorInfo()
{
    HAL_LOG_INFO("%s:sensor(%s) handle:%d", __FUNCTION__,
                 Utility::SensorTypeToSensorString(getType()), handle);
}

hal_sensor1_cb_t* SAMSensor::getSensor1Cb()
{
    return sensor1_cb;
}

int SAMSensor::batch(int flags, int64_t period_ns, int64_t timeout)
{
    float report_rate_f = 0;
    uint32_t report_rate_Q16 = 0;
    float sample_rate = 0;
    bool do_buffering = false;

    HAL_LOG_INFO("%s:sensor(%s) handle:%d flags:0x%x period_ns:%" PRId64" timeout:%" PRId64,
        __FUNCTION__, Utility::SensorTypeToSensorString(getType()), handle, flags, period_ns, timeout);

    if (period_ns < 0) {
        HAL_LOG_ERROR("Error in %s: argument ns < 0", __FUNCTION__);
        return -1;
    }

    sample_rate = calcSampleRate((uint64_t)period_ns);
    if (timeout == 0) {
        report_rate_f = 0;
        do_buffering = false;
    } else {
        report_rate_f = NSEC_TO_HZ(timeout);
        do_buffering = true;
    }

    /* The batch/report rate can never be greater than the sample rate */
    if (report_rate_f > sample_rate) {
        report_rate_f = sample_rate;
        HAL_LOG_DEBUG("Report rate has been adjusted to: %fHz", report_rate_f);
    }
    report_rate_Q16 = FX_FLTTOFIX_Q16(report_rate_f);
    HAL_LOG_DEBUG( "%s:sensor(%s) sample_rate=%fHz report_rate_f=%fHz curr sample rate:%f cur rpt rate:%u max:%f min:%f",
            __FUNCTION__, Utility::SensorTypeToSensorString(getType()),sample_rate, report_rate_f, freq, report_rate, max_freq, min_freq);

    if (sample_rate == freq && report_rate_Q16 == batch_rate && do_buffering == batching ) {
        HAL_LOG_INFO("%s: current sample rate, report rate & buffering are equal to requested (%f,%d,%d)",
            __FUNCTION__, sample_rate, report_rate_Q16, do_buffering);
    } else {
        freq = sample_rate;
        /* convert from Hz to sec in Q16 to support subHz/periodic requests */
        if (!strncmp("true", qfusion_use_report_period, 4) || freq < min_sample_freq) {
            report_rate = (int)lroundf(HZ_TO_SEC(freq) * UNIT_Q16);
        }
        else {
            report_rate = (int)lroundf(freq);
        }
        if ( batch_support == true ) {
            batch_rate = report_rate_Q16;
            batching = do_buffering;
            if ( enabled ) {
                enable(0);
                enable(1);
            }
        }
    }
    return 0;
}

int SAMSensor::flush()
{
    flush_requested++;
    HAL_LOG_INFO("%s:sensor(%s) handle:%d", __FUNCTION__,
                    Utility::SensorTypeToSensorString(getType()),handle);
    if (!enabled) {
        flush_requested--;
        HAL_LOG_DEBUG("%s: handle %d is inactive", __FUNCTION__, handle);
        return -EINVAL;
    }
    else if (batching) {
        HAL_LOG_DEBUG("%s: handle %d batch_support %d", __FUNCTION__, handle, batch_support);
        if (batch_support == true) {
            if (sendBatchReq()) {
                HAL_LOG_ERROR("%s: sendBatchReq failed!", Utility::SensorTypeToSensorString(getType()));
                return -1;
            }
        }
        else {
            /* SAM Sensors which doesn't have the batch support inject flush event to Android */
            pthread_mutex_lock(&data_cb->data_mutex);
            flushSendCmplt();
            pthread_mutex_unlock(&data_cb->data_mutex);
        }
    }
    else {
        HAL_LOG_DEBUG("%s: handle %d is not batching", __FUNCTION__, handle);
        pthread_mutex_lock(&data_cb->data_mutex);
        flushSendCmplt();
        pthread_mutex_unlock(&data_cb->data_mutex);
    }
    return 0;
}

/*===========================================================================
  FUNCTION:  sendBatchReq
===========================================================================*/
int SAMSensor::sendBatchReq()
{
    HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
    return 0;
}

/*============================================================================
  sendEnableReq
============================================================================*/
int SAMSensor::sendEnableReq(sensor1_msg_header_s *req_hdr, void *sam_req)
{
    sensor1_error_e error;
    HAL_LOG_DEBUG("%s:sensor(%s) Sending enable to svc no:%d", __FUNCTION__,
                                Utility::SensorTypeToSensorString(getType()), svc_num);

    /* Send Request */
    sensor1_cb->error = false;
    sensor1_cb->is_resp_arrived = false;

    if(SENSOR1_SUCCESS != (error = sensor1_write(sensor1_cb->sensor1_handle, req_hdr, sam_req))) {
        /* free the message buffer */
        sensor1_free_msg_buf(sensor1_cb->sensor1_handle, sam_req);
        HAL_LOG_ERROR("%s: sensor1_write() error: %d", __FUNCTION__, error);
        sensor1_cb->error = true;
        return -1;
    }
    /* waiting for response */
    if (Utility::waitForResponse(TIME_OUT_MS,
                             &sensor1_cb->cb_mutex,
                             &sensor1_cb->cb_cond,
                             &sensor1_cb->is_resp_arrived) == false) {
        HAL_LOG_ERROR("%s: ERROR: No response from enable request",
                           __FUNCTION__);
        sensor1_cb->error = true;
        return -1;
    }
    return 0;
}

/*============================================================================
  sendCancel
============================================================================*/
void SAMSensor::sendCancel()
{
  sensor1_error_e error;
  sensor1_msg_header_s req_hdr;
  sns_common_cancel_req_msg_v01 *cancel_msg;

  HAL_LOG_DEBUG( "%s:sensor(%s) Sending cancel to svc no:%d",
            __FUNCTION__,Utility::SensorTypeToSensorString(getType()), svc_num );

  pthread_mutex_lock(&sensor1_cb->cb_mutex);
  error = sensor1_alloc_msg_buf( sensor1_cb->sensor1_handle,
                                 sizeof(sns_common_cancel_req_msg_v01),
                                 (void**)&cancel_msg );

  if(SENSOR1_SUCCESS != error) {
    HAL_LOG_ERROR("%s:sensor(%s) sensor1_alloc_msg_buf() error: %d",
            __FUNCTION__,Utility::SensorTypeToSensorString(getType()), error);
  }
  else
  {
      req_hdr.service_number = svc_num;
      req_hdr.msg_id = 0; /* Message ID for Cancel Request is 0 for all services */
      req_hdr.msg_size = sizeof( sns_common_cancel_req_msg_v01 );
      req_hdr.txn_id = 0;

      sensor1_cb->error = false;
      sensor1_cb->is_resp_arrived = false;

      if(SENSOR1_SUCCESS !=
          (error = sensor1_write(sensor1_cb->sensor1_handle,
          &req_hdr, cancel_msg))) {
          sensor1_free_msg_buf( sensor1_cb->sensor1_handle, cancel_msg );
          HAL_LOG_ERROR( "%s:sensor(%s) sensor1_write() error: %d", __FUNCTION__,
                                Utility::SensorTypeToSensorString(getType()),error );
      }

      /* waiting for response */
      if (Utility::waitForResponse(TIME_OUT_MS,
                               &sensor1_cb->cb_mutex,
                               &sensor1_cb->cb_cond,
                               &sensor1_cb->is_resp_arrived) == false) {
          HAL_LOG_ERROR("%s: No response from cancel request for sensor: %s",
                        __FUNCTION__, Utility::SensorTypeToSensorString(getType()));
          pthread_mutex_unlock(&sensor1_cb->cb_mutex);
          return;
      }
      HAL_LOG_DEBUG("%s: Received cancel response for sensor: %s",
                    __FUNCTION__, Utility::SensorTypeToSensorString(getType()));
  }
  pthread_mutex_unlock(&sensor1_cb->cb_mutex);
}

/*===========================================================================
  FUNCTION:  sendAlgoAttribReq
===========================================================================*/
bool SAMSensor::sendAlgoAttribReq()
{
    sensor1_error_e error;
    sensor1_msg_header_s req_hdr;
    sns_sam_get_algo_attrib_req_msg_v01 *sam_req;

    HAL_LOG_INFO( "%s:sensor(%s) svc no:%d handle:%d", __FUNCTION__,
                                    Utility::SensorTypeToSensorString(getType()),svc_num, handle );
    pthread_mutex_lock(&sensor1_cb->cb_mutex);
    error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                                  sizeof(sns_sam_get_algo_attrib_req_msg_v01),
                                  (void**)&sam_req);
    if( SENSOR1_SUCCESS != error ) {
        HAL_LOG_ERROR( "%s: sensor1_alloc_msg_buf() error: %d", __FUNCTION__, error );
        pthread_mutex_unlock(&sensor1_cb->cb_mutex);
        return false;
    }

    req_hdr.service_number = svc_num;
    req_hdr.msg_id = 0x0024; /* Message ID for Attributes Request is 0x0024 for all services */
    req_hdr.msg_size = sizeof(sns_sam_get_algo_attrib_req_msg_v01);
    req_hdr.txn_id = 0;

    /* Send Request */
    sensor1_cb->error = false;
    if( (error = sensor1_write( sensor1_cb->sensor1_handle, &req_hdr,
                                sam_req )) != SENSOR1_SUCCESS ) {
        /* free the message buffer */
        sensor1_free_msg_buf( sensor1_cb->sensor1_handle, sam_req );
        HAL_LOG_ERROR( "%s: sensor1_write() error: %d", __FUNCTION__, error );
        pthread_mutex_unlock(&sensor1_cb->cb_mutex);
        return false;
    }

    /* waiting for response */
    if (Utility::waitForResponse(TIME_OUT_MS,
                               &sensor1_cb->cb_mutex,
                               &sensor1_cb->cb_cond,
                               &sensor1_cb->is_resp_arrived) == false) {
        HAL_LOG_ERROR( "%s: ERROR: No response from the request", __FUNCTION__ );
        pthread_mutex_unlock(&sensor1_cb->cb_mutex);
        return false;
    }

    HAL_LOG_DEBUG( "%s: Received Response: %d", __FUNCTION__, sensor1_cb->error );
    pthread_mutex_unlock(&sensor1_cb->cb_mutex);

    return true;
}

/*===========================================================================
  FUNCTION:  processAlgoAttribResp
===========================================================================*/
bool SAMSensor::processAlgoAttribResp(sensor1_msg_header_s *msg_hdr, void *msg_ptr )
{
    int rv;
    const sns_sam_get_algo_attrib_resp_msg_v01*  cbrsp_ptr =
               (sns_sam_get_algo_attrib_resp_msg_v01*) msg_ptr;

    HAL_LOG_INFO("%s:sensor(%s) Received response %d for svc_num %d handle %d",
                   __FUNCTION__, Utility::SensorTypeToSensorString(getType()),
                   cbrsp_ptr->resp.sns_result_t, msg_hdr->service_number, handle);

    if(cbrsp_ptr->resp.sns_result_t == SNS_RESULT_SUCCESS_V01) {
        /* Increment version to make sure it is positive value */
        setVersion(cbrsp_ptr->algorithm_revision + 1);
        /* Exclude Thresh handles as Thresh Algo won't set below attributes */
        if(handle != HANDLE_THRESH && handle != HANDLE_THRESH_NON_WAKE_UP) {
            setPower((float)(cbrsp_ptr->power * UNIT_CONVERT_Q16));
            setMaxFreq((float)(cbrsp_ptr->max_report_rate * UNIT_CONVERT_Q16));
            setMinFreq((float)(cbrsp_ptr->min_report_rate * UNIT_CONVERT_Q16));
            setMaxSampleFreq((float)(cbrsp_ptr->max_sample_rate * UNIT_CONVERT_Q16));
            setMinSampleFreq((float)(cbrsp_ptr->min_sample_rate * UNIT_CONVERT_Q16));
        }
        setMaxBufferedSamples(cbrsp_ptr->max_batch_size);
        setAttribOK(true);
        if(cbrsp_ptr->reserved_batch_size_valid) {
            setResBufferedSamples(cbrsp_ptr->reserved_batch_size);
        }
        HAL_LOG_INFO("%s:sensor(%s) sensor1 Version:%d Power:%d MaxFreq:%d \
                     MinFreq:%d MaxSampleFreq:%d MinSampleFreq:%d MaxBuffSamples:%d ResBuffSamples:%d",
                     __FUNCTION__, Utility::SensorTypeToSensorString(getType()),
                     cbrsp_ptr->algorithm_revision, cbrsp_ptr->power, cbrsp_ptr->max_report_rate,
                     cbrsp_ptr->min_report_rate, cbrsp_ptr->max_sample_rate,
                     cbrsp_ptr->min_sample_rate, cbrsp_ptr->max_batch_size,
                     cbrsp_ptr->reserved_batch_size );
        HAL_LOG_DEBUG("%s:sensor(%s:wakeup:%d) HAL Version:%d Power:%f MaxFreq:%f MinFreq:%f \
                     MaxSampleFreq:%f MinSampleFreq:%f MaxBuffSamples:%d ResBuffSamples:%d",
                     __FUNCTION__, Utility::SensorTypeToSensorString(getType()), bWakeUp,
                     getVersion(), getPower(), getMaxFreq(), getMinFreq(),
                     getMaxSampleFreq(), getMinSampleFreq(), getMaxBufferedSamples(),
                     getResBufferedSamples());
        rv = true;
    } else {
        HAL_LOG_ERROR("%s: ERROR response from the request %d for handle:%d svc no:%d",
                       __FUNCTION__, cbrsp_ptr->resp.sns_err_t, handle, svc_num);
        setAttribOK(false);
        rv = false;
    }

    return rv;
}

/*===========================================================================
  FUNCTION:  SAMSensor_sensor1_cb
    Handle the sensor1 callback for the SAM sensors. Each SAM sensor has its own
    sensor1 handle but they share the same callbcak function.
    Parameters
    @cb_data : pointer of the callback data, SensorsContext is passed in
            this function
    @msg_hdr : sensor1 message header
    @msg_type : sensor1 message type, two major types are listed in the below:
            SENSOR1_MSG_TYPE_RESP
            SENSOR1_MSG_TYPE_IND
    @msg_ptr : sensor1 message pointer, do free this memory before return
===========================================================================*/
void SAMSensor_sensor1_cb (intptr_t cb_data,
                     sensor1_msg_header_s *msg_hdr,
                     sensor1_msg_type_e msg_type,
                     void *msg_ptr)
{
    SAMSensor *sam = (SAMSensor *)cb_data;
    hal_sensor1_cb_t *sensor1_cb = sam->getSensor1Cb();
    hal_data_cb_t *data_cb = Utility::getDataCb();

    if (msg_hdr != NULL) {
        HAL_LOG_VERBOSE("%s: msg_type %d, Sn %d, msg Id %d, txn Id %d", __FUNCTION__,
            msg_type, msg_hdr->service_number, msg_hdr->msg_id, msg_hdr->txn_id );
    }
    else {
        if (msg_type != SENSOR1_MSG_TYPE_BROKEN_PIPE &&
            msg_type != SENSOR1_MSG_TYPE_REQ &&
            msg_type != SENSOR1_MSG_TYPE_RETRY_OPEN ) {
            HAL_LOG_ERROR("%s: Error - invalid msg type with NULL msg_hdr: %u",
                __FUNCTION__, msg_type);
            return;
        }
        else {
            HAL_LOG_VERBOSE("%s: msg_type %d", __FUNCTION__, msg_type);
        }
    }

    switch(msg_type) {
    case SENSOR1_MSG_TYPE_RESP_INT_ERR:
        pthread_mutex_lock(&sensor1_cb->cb_mutex);
        Utility::signalResponse(true, sensor1_cb);
        pthread_mutex_unlock(&sensor1_cb->cb_mutex);
        break;
    case SENSOR1_MSG_TYPE_RESP:
        sam->processResp(msg_hdr, msg_ptr);
        break;
    case SENSOR1_MSG_TYPE_IND:
        pthread_mutex_lock(&data_cb->data_mutex);
        /* acquire wakelock to make sure system doesn't go into suspend
         * till data/indication is received by Android */
        if (sam->getFlags() & SENSOR_FLAG_WAKE_UP) {
            Utility::acquire_sensor_wake_lock(data_cb, __FUNCTION__);
        }
        pthread_mutex_unlock(&data_cb->data_mutex);
        sam->processInd(msg_hdr, msg_ptr);
        break;
    case SENSOR1_MSG_TYPE_BROKEN_PIPE:
        HAL_LOG_WARN("%s: SENSOR1_MSG_TYPE_BROKEN_PIPE", __FUNCTION__);
        if (sensor1_cb != NULL) {
            pthread_mutex_lock(&sensor1_cb->cb_mutex);
            Recovery::handleBrokenPipe(sensor1_cb, &SAMSensor_sensor1_cb, cb_data);
            pthread_mutex_unlock(&sensor1_cb->cb_mutex);
            if (sam->getAttribOK()) {
                if (sam->getEnabled()) {
                    /* Before enable the sensor, it is better to disable the
                       sensor to reset the enabled variable */
                    sam->enable(0);
                    /* Re-enable the sensor */
                    sam->enable(1);
                }
            }
        } else {
            HAL_LOG_ERROR("%s: sensor1_cb is NULL!", __FUNCTION__);
            return;
        }
        break;
    case SENSOR1_MSG_TYPE_RETRY_OPEN:
        HAL_LOG_WARN("%s: SENSOR1_MSG_TYPE_RETRY_OPEN", __FUNCTION__);
        if (sensor1_cb != NULL) {
            pthread_mutex_lock(&sensor1_cb->cb_mutex);
            Recovery::reInit(sensor1_cb, &SAMSensor_sensor1_cb, cb_data);
            pthread_mutex_unlock( &sensor1_cb->cb_mutex );
        } else {
            HAL_LOG_ERROR("%s: sensor1_cb is NULL!", __FUNCTION__);
            return;
        }
        break;
    case SENSOR1_MSG_TYPE_REQ:
    default:
        HAL_LOG_ERROR("%s: Error - invalid msg type in cb: %u", __FUNCTION__, msg_type);
        break;
    }

    pthread_mutex_lock(&sensor1_cb->cb_mutex);
    if (NULL != msg_ptr && sensor1_cb->sensor1_handle) {
        sensor1_free_msg_buf(sensor1_cb->sensor1_handle, msg_ptr);
    }
    pthread_mutex_unlock(&sensor1_cb->cb_mutex);
    return;
}
