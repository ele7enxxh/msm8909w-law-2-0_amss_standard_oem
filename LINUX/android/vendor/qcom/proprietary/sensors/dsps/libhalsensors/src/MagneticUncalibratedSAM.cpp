/*============================================================================
  @file MagneticUncalibratedSAM.cpp

  @brief
  MagneticUncalibratedSAM class implementation.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#include "TimeSyncService.h"
#include "MagneticUncalibratedSAM.h"
#include "SensorsContext.h"

/*============================================================================
  MagneticUncalibratedSAM Constructor
============================================================================*/
MagneticUncalibratedSAM::MagneticUncalibratedSAM(int handle)
    :SAMSensor(handle)
{
    if(getAttribOK() == true) {
        trigger_mode = SENSOR_MODE_CONT;
        svc_num = SNS_SAM_MAG_CAL_SVC_ID_V01;
        HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
        SensorsContext *context = SensorsContext::getInstance();
        Sensor *mSensor = context->getSensor(HANDLE_MAGNETIC_FIELD);
        if(HANDLE_MAGNETIC_FIELD_UNCALIBRATED_SAM_WAKE_UP == handle) {
            mSensor = context->getSensor(HANDLE_MAGNETIC_FIELD_WAKE_UP);
            bWakeUp = true;
        }
        if (mSensor != NULL) {
            /* Add calibrated mag through SAM, if we have a mag device and we are
             * supporting calibration through the SAM
             */
            setName(mSensor->getName());
            strlcat(name, " Uncalibrated", SNS_MAX_SENSOR_NAME_SIZE);
            setVendor(mSensor->getVendor());
            setVersion(mSensor->getVersion());
            setType(SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED);
            if (bWakeUp == false) {
                setFlags(SENSOR_FLAG_CONTINUOUS_MODE);
            } else {
                strlcat(name," -Wakeup",SNS_MAX_SENSOR_NAME_SIZE);
                setFlags(SENSOR_FLAG_CONTINUOUS_MODE|SENSOR_FLAG_WAKE_UP);
            }
            setMaxRange(mSensor->getMaxRange());
            setResolution(mSensor->getResolution());
            setPower(mSensor->getPower());
            setMaxFreq(mSensor->getMaxFreq());
            setMinFreq(mSensor->getMinFreq());
            setMaxBufferedSamples(0);
        }
        else {
            HAL_LOG_ERROR("The mSensor handle %d is NULL!", handle);
        }
    }
}

/*============================================================================
  MagneticUncalibratedSAM Destructor
============================================================================*/
MagneticUncalibratedSAM::~MagneticUncalibratedSAM()
{

}

/*============================================================================
  enable
============================================================================*/
int MagneticUncalibratedSAM::enable(int en)
{
    int err;
    sensor1_error_e error;
    sensor1_msg_header_s req_hdr;
    sns_sam_mag_cal_enable_req_msg_v01 *sam_req;

    if (enabled == en) {
        HAL_LOG_INFO("MAG CAL is already enabled/disabled %d", enabled);
        return 0;
    }

    /* store the en value */
    enabled = en;
    HAL_LOG_DEBUG("%s: handle=%d", __FUNCTION__, handle);

    if (en) {
        pthread_mutex_lock(&sensor1_cb->cb_mutex);
        error = sensor1_alloc_msg_buf(sensor1_cb->sensor1_handle,
                                 sizeof(sns_sam_mag_cal_enable_req_msg_v01),
                                 (void**)&sam_req );
        if (SENSOR1_SUCCESS != error) {
            HAL_LOG_ERROR("%s:sensor1_alloc_msg_buf error:%d", __FUNCTION__, error);
            pthread_mutex_unlock(&sensor1_cb->cb_mutex);
            enabled = 0;
            return -1;
        }

        req_hdr.service_number = svc_num;
        req_hdr.msg_id = SNS_SAM_MAG_CAL_ENABLE_REQ_V01;
        req_hdr.msg_size = sizeof(sns_sam_mag_cal_enable_req_msg_v01);
        req_hdr.txn_id = 0;

        sam_req->report_period = (uint32_t)(UNIT_Q16/report_rate);
        sam_req->sample_rate_valid = true;
        sam_req->sample_rate = (uint32_t) MAX(FREQ_NORMAL_HZ, report_rate) * UNIT_Q16;

        /* Send Enable Request */
        err = sendEnableReq(&req_hdr, (void *)sam_req);
        if (err) {
            HAL_LOG_ERROR("send the SAM sensor Enable message failed!");
            pthread_mutex_unlock(&sensor1_cb->cb_mutex);
            enabled = 0;
            return -1;
        }

        HAL_LOG_DEBUG("%s: Received response:%d", __FUNCTION__, sensor1_cb->error);
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
void MagneticUncalibratedSAM::processResp(sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    const sns_common_resp_s_v01*  crsp_ptr = (sns_common_resp_s_v01 *)msg_ptr;
    bool                          error = false;

    HAL_LOG_INFO("%s: handle:%d msg_id=%d", __FUNCTION__, handle, msg_hdr->msg_id);
    if( SENSOR1_ENOTALLOWED == crsp_ptr->sns_err_t )
    {
        HAL_LOG_DEBUG("%s: Algorithm instance ID not found by SAM", __FUNCTION__);
    }
    else if (crsp_ptr->sns_result_t != 0) {
        HAL_LOG_ERROR("%s: Msg %i; Result: %u, Error: %u", __FUNCTION__,
                    msg_hdr->msg_id, crsp_ptr->sns_result_t, crsp_ptr->sns_err_t);
        error = true;
    }

    if(true != error ) {
        switch (msg_hdr->msg_id) {
        case SNS_SAM_MAG_CAL_ENABLE_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_MAG_CAL_ENABLE_RESP_V01", __FUNCTION__);
            instance_id = ((sns_sam_mag_cal_enable_resp_msg_v01 *)msg_ptr)->instance_id;
            break;
        case SNS_SAM_MAG_CAL_DISABLE_RESP_V01:
        case SNS_SAM_MAG_CAL_CANCEL_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_MAG_CAL_DISABLE/CANCEL_RESP_V01", __FUNCTION__);
            /* Reset instance ID */
            instance_id = 0xFF;
            break;
        case SNS_SAM_MAG_CAL_VERSION_RESP_V01:
            HAL_LOG_DEBUG("%s: Received SNS_SAM_MAG_CAL_VERSION_RESP_V01", __FUNCTION__);
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
void MagneticUncalibratedSAM::processInd(sensor1_msg_header_s *msg_hdr, void *msg_ptr)
{
    sns_sam_mag_cal_report_ind_msg_v01*  sam_mag_cal_rpt_ptr =
                                     (sns_sam_mag_cal_report_ind_msg_v01*) msg_ptr;
    bool                             error = false;
    uint32_t                         timestamp = 0;
    sensors_event_t                  sensor_data;

    HAL_LOG_INFO("%s: handle:%d", __FUNCTION__, handle);
    switch (msg_hdr->msg_id) {
        case SNS_SAM_MAG_CAL_REPORT_IND_V01:
            HAL_LOG_DEBUG("%s: SNS_SAM_MAG_CAL_REPORT_IND_V01", __FUNCTION__);
            sensor_data.type = SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED;
            if(bWakeUp == false) {
                sensor_data.sensor = HANDLE_MAGNETIC_FIELD_UNCALIBRATED_SAM;
                HAL_LOG_VERBOSE("%s:sensor %s ",__FUNCTION__,
                            Utility::SensorTypeToSensorString(getType()));
            } else {
                sensor_data.sensor = HANDLE_MAGNETIC_FIELD_UNCALIBRATED_SAM_WAKE_UP;
                HAL_LOG_VERBOSE("%s:sensor %s (wake_up)",__FUNCTION__,
                            Utility::SensorTypeToSensorString(getType()));
            }
            /* Convert from SAE to Android co-ordinates and scale
              x' = y; y' = x; z' = -z;                                        */
            float x_uncalib, y_uncalib, z_uncalib;

            x_uncalib = (float)(sam_mag_cal_rpt_ptr->result.m[1] +
                sam_mag_cal_rpt_ptr->result.b[1]) * UNIT_CONVERT_MAGNETIC_FIELD;
            y_uncalib = (float)(sam_mag_cal_rpt_ptr->result.m[0] +
                sam_mag_cal_rpt_ptr->result.b[0]) * UNIT_CONVERT_MAGNETIC_FIELD;
            z_uncalib = (float)-(sam_mag_cal_rpt_ptr->result.m[2] +
                sam_mag_cal_rpt_ptr->result.b[2]) * UNIT_CONVERT_MAGNETIC_FIELD;

            sensor_data.uncalibrated_magnetic.x_uncalib = x_uncalib;
            sensor_data.uncalibrated_magnetic.y_uncalib = y_uncalib;
            sensor_data.uncalibrated_magnetic.z_uncalib = z_uncalib;

            sensor_data.uncalibrated_magnetic.x_bias =
                (float)(sam_mag_cal_rpt_ptr->result.b[1] * UNIT_CONVERT_MAGNETIC_FIELD);
            sensor_data.uncalibrated_magnetic.y_bias =
                (float)(sam_mag_cal_rpt_ptr->result.b[0] * UNIT_CONVERT_MAGNETIC_FIELD);
            sensor_data.uncalibrated_magnetic.z_bias =
                (float)(-sam_mag_cal_rpt_ptr->result.b[2] * UNIT_CONVERT_MAGNETIC_FIELD);

            mag_cal_cur_sample.sam_ts = sam_mag_cal_rpt_ptr->timestamp;

            sensor_data.magnetic.status = sam_mag_cal_rpt_ptr->result.accuracy;
            timestamp = sam_mag_cal_rpt_ptr->timestamp;
            break;
        case SNS_SAM_MAG_CAL_ERROR_IND_V01:
            HAL_LOG_ERROR("%s: SNS_SAM_MAG_CAL_ERROR_IND_V01", __FUNCTION__);
            error = true;
            break;
        default:
            HAL_LOG_ERROR("%s: Unknown message ID = %d", __FUNCTION__, msg_hdr->msg_id);
            error = true;
            break;
    }

    if (error == false) {
        sensor_data.version = sizeof(sensors_event_t);
        sensor_data.timestamp = time_service->timestampCalc(
                (uint64_t)timestamp, sensor_data.sensor);

        HAL_LOG_VERBOSE( "%s: SAM Uncal mag x:%f y:%f z:%f xb:%f yb:%f zb:%f ts:%u HAL TS:%" PRId64 " elapsedRealtimeNano:%" PRId64 "",
            __FUNCTION__, sensor_data.uncalibrated_magnetic.x_uncalib, sensor_data.uncalibrated_magnetic.y_uncalib,
            sensor_data.uncalibrated_magnetic.z_uncalib, sensor_data.uncalibrated_magnetic.x_bias,
            sensor_data.uncalibrated_magnetic.y_bias, sensor_data.uncalibrated_magnetic.z_bias, timestamp,
            sensor_data.timestamp, android::elapsedRealtimeNano() );

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
