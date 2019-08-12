
/*============================================================================
  @SSC_Calibrate.cpp

  Sensor calibration for ssc source file

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/
#include <SSC_Calibrate.h>

/*============================================================================
  Useful macros
  ==========================================================================*/
#define FX_QFACTOR           (16)
#define FX_FLTTOFIX(f,q)     ((int32_t)( (f)*(1<<(q))+(((f)>(0.0))?(0.5):(-0.5))))
#define FX_FLTTOFIX_Q16(d)   (FX_FLTTOFIX(d,FX_QFACTOR))

void cal_sensor1_cb(intptr_t data, sensor1_msg_header_s *msg_hdr,
                sensor1_msg_type_e msg_type, void *msg_ptr);

ssc_calibrate::ssc_calibrate()
{
    sensor_cal = new sensor_cal_t;
    sensor_cal->resp_result = -1;
    sensor_cal->is_cb_arrived = false;
    sensor_cal->sensor1_handle = NULL;
    pthread_mutex_init(&sensor_cal->cb_mutex, NULL);
    pthread_cond_init(&sensor_cal->cb_cond, NULL);
    mTxn_id = 0;
}

ssc_calibrate::~ssc_calibrate() {

}

int ssc_calibrate::sensor_by_type(int type) {
    return type;
}

int ssc_calibrate::calibrate(int sensor_id, int test_type, int data_type) {
    int err;

    mSensor_id = sensor_id;
    mTest_type = test_type;
    mData_type = data_type;
    err = req_sensor_cal();

    LOGD( "%s: sensor calibrate returned %d", __FUNCTION__, err);
    return err;
}

user_cal_error_e ssc_calibrate::close_sensor1()
{
    sensor1_error_e error;
    mTxn_id++;

    error = sensor1_close(sensor_cal->sensor1_handle);
    if( SENSOR1_SUCCESS != error )
    {
        LOGE( "%s: sensor1_close returned %d", __FUNCTION__, error );
        return USER_CAL_SENSOR1;
    }
    return USER_CAL_SUCCESS;
}

int ssc_calibrate:: req_sensor_cal() {

    sensor1_msg_header_s msgHdr;
    sns_smgr_single_sensor_test_req_msg_v01 *msgPtr;
    sensor1_error_e sensor1_error;
    struct timespec ts; // timestamp to use with timed_wait
    int resp_result;    // Result contained in response
    int result_status;  // Result of timed_wait

    sensor1_error = sensor1_open(&sensor_cal->sensor1_handle, &cal_sensor1_cb, (intptr_t)this);
    if( SENSOR1_SUCCESS != sensor1_error ) {
        LOGE( "%s: sensor1_open returned %d", __FUNCTION__, sensor1_error );
        return USER_CAL_SENSOR1;
    }

    sensor1_error = sensor1_alloc_msg_buf( sensor_cal->sensor1_handle,
            sizeof(sns_smgr_single_sensor_test_req_msg_v01),(void**)&msgPtr );
    if( SENSOR1_SUCCESS != sensor1_error )
    {
        LOGE( "%s: sensor1_alloc_msg_buf returned %d",
                __FUNCTION__, sensor1_error );
        close_sensor1();
        return USER_CAL_SENSOR1;
    }

    msgHdr.service_number = SNS_SMGR_SVC_ID_V01;
    msgHdr.msg_id = SNS_SMGR_SINGLE_SENSOR_TEST_REQ_V01;
    msgHdr.msg_size = sizeof(sns_smgr_single_sensor_test_req_msg_v01);
    msgHdr.txn_id = mTxn_id;

    /* Create the message */
    msgPtr->SensorID = mSensor_id;
    msgPtr->DataType = mData_type;
    msgPtr->TestType = (sns_smgr_test_type_e_v01)mTest_type;
    msgPtr->SaveToRegistry_valid = true;
    msgPtr->SaveToRegistry = true;
    msgPtr->ApplyCalNow_valid = true;
    msgPtr->ApplyCalNow = true;

    LOGD( "%s: message id = %d", __FUNCTION__, msgPtr->SensorID);
    pthread_mutex_lock( &sensor_cal->cb_mutex );
    sensor1_error = sensor1_write( sensor_cal->sensor1_handle,
            &msgHdr, msgPtr );
    if( SENSOR1_SUCCESS != sensor1_error )
    {
        LOGE( "%s: sensor1_write returned %d", __FUNCTION__, sensor1_error );
        sensor1_free_msg_buf(sensor_cal->sensor1_handle, msgPtr);
        close_sensor1();
        pthread_mutex_unlock( &sensor_cal->cb_mutex );
        return USER_CAL_SENSOR1;
    }

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 10;

    /* wait for response */
    while( 1 != sensor_cal->is_cb_arrived )
    {
        result_status = pthread_cond_timedwait( &sensor_cal->cb_cond,
                &sensor_cal->cb_mutex, &ts );
        if( ETIMEDOUT == result_status )
        {
            LOGE( "%s: Request timed-out", __FUNCTION__ );
            sensor_cal->resp_result = USER_CAL_TIMEOUT;
            break;
        }
    }

    sensor_cal->is_cb_arrived = 0;
    resp_result = sensor_cal->resp_result;
    close_sensor1();

    LOGD( "%s: sensor returned %d", __FUNCTION__, resp_result);
    pthread_mutex_unlock( &sensor_cal->cb_mutex );
    return resp_result;
}

sensor_cal_t* ssc_calibrate::get_cal()
{
    return sensor_cal;
}

int ssc_calibrate::get_txn_id()
{
    return mTxn_id;
}

int signal_response(int result, sensor_cal_t *sensor_cal) {
    pthread_mutex_lock( &sensor_cal->cb_mutex );
    sensor_cal->resp_result = result;
    sensor_cal->is_cb_arrived = true;
    pthread_cond_signal( &sensor_cal->cb_cond );
    pthread_mutex_unlock( &sensor_cal->cb_mutex );

    return 0;
}

void cal_sensor1_cb(intptr_t data, sensor1_msg_header_s *msg_hdr,
                sensor1_msg_type_e msg_type, void *msg_ptr) {
    ssc_calibrate *ssc = (ssc_calibrate *)data;
    sensor_cal_t *sensor_cal = ssc->get_cal();
    if (NULL == msg_hdr) {
        LOGD( "%s: received NULL msg_hdr_ptr. type: %i",
            __FUNCTION__, msg_type );
        return ;
    }

    if (SENSOR1_MSG_TYPE_RESP == msg_type) {
        sns_smgr_single_sensor_test_resp_msg_v01 *msgPtr =
            (sns_smgr_single_sensor_test_resp_msg_v01*) msg_ptr;
        if( msg_hdr->txn_id != ssc->get_txn_id() )
        {
           LOGE( "%s: Wrong transaction ID: %i, %i",
            __FUNCTION__, msg_hdr->txn_id, ssc->get_txn_id() );
        }
        else if (msgPtr->Resp.sns_result_t == 0)
        {
            if (msgPtr->TestStatus == 0)
                signal_response( msgPtr->TestStatus, sensor_cal);
            else if (msgPtr->TestStatus != 1)
                signal_response( -msgPtr->TestStatus - 10, sensor_cal);
        }
    }
    else if (SENSOR1_MSG_TYPE_IND == msg_type) {
        sns_smgr_single_sensor_test_ind_msg_v01 *msgPtr =
            (sns_smgr_single_sensor_test_ind_msg_v01 *) msg_ptr;
        if (msgPtr->TestResult == 0)
            signal_response( msgPtr->TestResult, sensor_cal);
        else if (msgPtr->ErrorCode_valid == true)
            signal_response( msgPtr->ErrorCode, sensor_cal);
        else
            signal_response( -msgPtr->TestResult, sensor_cal);
    }
    else
    {
      LOGW( "%s: Unsupported message.  type: %i; svc_id: %i; msg_id: %i",
            __FUNCTION__, msg_type, msg_hdr->service_number,
            msg_hdr->msg_id );
    }

    if( NULL != msg_ptr ) {
      sensor1_free_msg_buf( *((sensor1_handle_s**)data), msg_ptr );
    }
}


