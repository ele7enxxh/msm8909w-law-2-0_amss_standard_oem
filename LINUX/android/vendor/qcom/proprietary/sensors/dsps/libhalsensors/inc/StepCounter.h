/*============================================================================
  @file StepCounter.h

  @brief
  StepCounter class definition.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#ifndef ANDROID_STEPCOUNTER_SENSOR_H
#define ANDROID_STEPCOUNTER_SENSOR_H

#include "SAMSensor.h"
#include "sns_sam_ped_v01.h"

/*============================================================================
 * Class StepCounter
=============================================================================*/

class StepCounter : public SAMSensor {
    uint64_t step_counter_running_total;
    uint64_t step_counter_running_instance;
    uint64_t step_counter_current_instance;
    bool is_batch_manage_timer_created;
    bool is_report_timer_created;
    struct timespec ts_event, ts_poll;
    uint64_t prev_stepcount;
    bool asyncmode;
    timer_t batch_manage_timer;
    timer_t report_timer;
/*===========================================================================
  FUNCTION:  sendGetReportReq
    Get the last report data after enabling the sensor.
===========================================================================*/
    int sendGetReportReq(bool bRespRequired);

protected:
    /* The last sample of step counter sensor */
    hal_sam_sample_t latest_sample;

public:
    StepCounter(int handle);
    ~StepCounter();
    int enable(int en);
/*===========================================================================
  FUNCTION:  processResp
    Process the response to the sensor1 SENSOR1_MSG_TYPE_RESP
    Parameters
        @msg_hdr : sensor1 message header
        @msg_ptr : sensor1 message data
===========================================================================*/
    void processResp(sensor1_msg_header_s *msg_hdr, void *msg_ptr);
/*===========================================================================
  FUNCTION:  processInd
    Process the response to the sensor1 SENSOR1_MSG_TYPE_IND
    Parameters
        @msg_hdr : sensor1 message header
        @msg_ptr : sensor1 message data
===========================================================================*/
    void processInd(sensor1_msg_header_s *msg_hdr, void *msg_ptr);
/*===========================================================================
  FUNCTION:  setSensorInfo
    Fill the sensor specific information.
===========================================================================*/
    void setSensorInfo();
/*===========================================================================
  FUNCTION:  sendBatchReq
  Send request to receive stepcounter info at periodic intervals
===========================================================================*/
    int sendBatchReq();
/*===========================================================================
  FUNCTION:  manageBatch
  Timer handler which allows switching to async mode when activity stopped
===========================================================================*/
    static void  manageBatch( sigval_t );
/*===========================================================================
  FUNCTION:  initBatchManageTimer
  Setup batch manage timer handler
===========================================================================*/
    int initBatchManageTimer(void);
/*===========================================================================
  FUNCTION:  reportLatestEvent
  Report latest count if it has not been reported.
===========================================================================*/
    static void  reportLatestEvent( sigval_t );
/*===========================================================================
  FUNCTION:  initTimer
  Setup event report timer handler
===========================================================================*/
    int initReportTimer(float report_delay_sec);
/*===========================================================================
  FUNCTION:  reportSingleEvent
  Report single sample to Android
===========================================================================*/
    void reportSingleEvent(hal_sam_sample_t *curr_sample);
};

#endif
