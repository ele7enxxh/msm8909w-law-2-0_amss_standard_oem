/*============================================================================
  @file DevicePositionClassifier.h

  @brief
  DevicePositionClassifier class definition.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#ifndef ANDROID_DPC_SENSOR_H
#define ANDROID_DPC_SENSOR_H

#include "SAMSensor.h"
#include "sns_sam_dpc_v02.h"

/*============================================================================
 * Class DevicePositionClassifier
=============================================================================*/

class DevicePositionClassifier : public SAMSensor {

public:
    DevicePositionClassifier(int handle);
    ~DevicePositionClassifier();
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
};

#endif
