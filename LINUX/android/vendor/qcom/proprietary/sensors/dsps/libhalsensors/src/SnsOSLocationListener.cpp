/*============================================================================
  @file SnsOSLocationListener.cpp

  @brief
  SnsOSLocationListener class implementation.

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#ifndef WEAR_SUPPORT
#ifdef FEATURE_SNS_IOD

#include "SnsOSLocationListener.h"
#include "IOD.h"

/*============================================================================
  FUNCTION: onLocationChanged
============================================================================*/
void SnsOSLocationListener::onLocationChanged (const IzatLocation * location, const IzatLocationStatus status) {
    UNREFERENCED_PARAMETER(status);
    HAL_LOG_DEBUG("%s: Location changed! latitude: %f longitude: %f",
                  __FUNCTION__, location->mLatitude, location->mLongitude);
    IOD iod(HANDLE_IOD);
    iod.setLocData(location->mLatitude, location->mLongitude);
}

/*============================================================================
  FUNCTION: onStatusChanged
============================================================================*/
void SnsOSLocationListener::onStatusChanged (const IzatProviderStatus status) {
    UNREFERENCED_PARAMETER(status);
    HAL_LOG_INFO("%s: Status changed!", __FUNCTION__);
}

/*============================================================================
  FUNCTION: listensTo
============================================================================*/
IzatLocationListenerMask SnsOSLocationListener::listensTo () const {
    HAL_LOG_INFO("%s: Returning Location Listener bit mask", __FUNCTION__);
    return (IZAT_STREAM_FUSED | IZAT_STREAM_NETWORK | IZAT_STREAM_GNSS);
}

#endif /* FEATURE_SNS_IOD */
#endif /* WEAR_SUPPORT */
