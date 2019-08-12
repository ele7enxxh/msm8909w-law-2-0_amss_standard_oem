/*============================================================================
  @file Thresh.h

  @brief
  Thresh class definition.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#ifndef ANDROID_THRESH_SENSOR_H
#define ANDROID_THRESH_SENSOR_H

#include "SAMSensor.h"
#include "sns_sam_sensor_thresh_v01.h"

/*============================================================================
 * Class Thresh
=============================================================================*/

class Thresh : public SAMSensor {

protected:
    /* The last event of thresh sensor */
    sensors_event_t thresh_last_event;
    /* To check Thresh is already enabled or NOT */
    bool thresh_enabled;

public:
    Thresh(int handle);
    ~Thresh();
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
  FUNCTION:  sendBatchReq
===========================================================================*/
    int sendBatchReq();
};


#endif
