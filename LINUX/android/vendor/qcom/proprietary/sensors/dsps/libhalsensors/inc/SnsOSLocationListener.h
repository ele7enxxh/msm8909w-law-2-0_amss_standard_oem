/*============================================================================
  @file SnsOSLocationListener.h

  @brief
  SnsOSLocationListener class definition.

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#ifndef WEAR_SUPPORT
#ifdef FEATURE_SNS_IOD

#ifndef SNS_OS_LOCATION_LISTENER_H
#define SNS_OS_LOCATION_LISTENER_H

#include "IIzatManager.h"
#include "IzatLocation.h"
#include "IOSLocationListener.h"
#include "SAMSensor.h"
#include <list>
#include <unistd.h>

using namespace izat_manager;

class SnsOSLocationListener : public IOSLocationListener {
public:

/*===========================================================================
  FUNCTION:  onLocationChanged
    Invoked when location changes
    Parameters
        @location : Pointer to an instance of IzatLocation
===========================================================================*/
    void onLocationChanged (const IzatLocation * location, const IzatLocationStatus status);
/*===========================================================================
  FUNCTION:  onStatusChanged
    Invoked when status changes
    Parameters
        @status : Pointer to an instance of IzatProviderStatus
===========================================================================*/
    void onStatusChanged (const IzatProviderStatus status);
/*===========================================================================
  FUNCTION:  listensTo
    Gets bit masks of location streams that this listener listens to
    Return value
        @IzatLocationListenerMask : Location listener bit mask
===========================================================================*/
    IzatLocationListenerMask listensTo () const;

};

#endif

#endif /* FEATURE_SNS_IOD */
#endif /* WEAR_SUPPORT */
