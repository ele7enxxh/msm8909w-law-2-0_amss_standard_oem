/*============================================================================
  @file IOD.h

  @brief
  IOD class definition.

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#ifndef WEAR_SUPPORT
#ifdef FEATURE_SNS_IOD

#ifndef ANDROID_IOD_SENSOR_H
#define ANDROID_IOD_SENSOR_H

#include "SAMSensor.h"
#include "sns_sam_iod_v01.h"
#include "SnsOSLocationListener.h"

using namespace izat_manager;

/*============================================================================
 * Class IOD
=============================================================================*/

class IOD : public SAMSensor {
protected:
    IIzatManager *pIzatManager;
    IOSFramework *osFramework;
    SnsOSLocationListener osLocationListener;
    IIzatManager::IzatProviderType providerType = IZAT_STREAM_NETWORK;
    IzatRequest *pIzatRequest;

public:
    IOD(int handle);
    ~IOD();
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
  FUNCTION:  setLocData
    Process the response from Location Listener
    Parameters
        @mlat : Latitude
        @mlong : Longitude
===========================================================================*/
    void setLocData(double mlat, double mlong);
};

#endif
#endif /* FEATURE_SNS_IOD */
#endif /* WEAR_SUPPORT */
