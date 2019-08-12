/*============================================================================
  @file QHeart.h

  @brief
  QHeart class definition.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
#ifndef ANDROID_QHEART_SENSOR_H
#define ANDROID_QHEART_SENSOR_H

#include "SAMSensor.h"
#include "sns_sam_qheart_v01.h"

/*============================================================================
 * Class QHeart
=============================================================================*/

class QHeart : public SAMSensor {
protected:
    /* The last event of QHeart sensor */
    sensors_event_t qheart_last_event;
    /* To check qheart is already enabled or NOT */
    bool qheart_enabled;

public:
    QHeart(int handle);
    ~QHeart();
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
