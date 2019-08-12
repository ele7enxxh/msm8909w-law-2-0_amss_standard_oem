/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <comdef.h>
#include <IzatTypes.h>
#include <IzatRequest.h>
#include <IzatLocation.h>
#include <NetworkLocationProviderGlue.h>
#include <IzatProviderEngine_jni.h>
#include <IIzatManager.h>

using namespace izat_manager;

NetworkLocationProviderGlue * NetworkLocationProviderGlue :: mInstance = NULL;

NetworkLocationProviderGlue * NetworkLocationProviderGlue :: getInstance () {

    if (!mInstance) {
        mInstance = new NetworkLocationProviderGlue ();
    }

    return mInstance;
}

void NetworkLocationProviderGlue :: destroyInstance () {
    if (mInstance) {
        mInstance->mIzatManager->disableProvider
        (
            mInstance->mLocationStreamType
        );
        mInstance->mIzatManager->unsubscribeListener (mInstance);
    }
}

// ctor
NetworkLocationProviderGlue :: NetworkLocationProviderGlue ()
: LocationProviderGlue (IZAT_STREAM_NETWORK) {}

// dtor
NetworkLocationProviderGlue :: ~NetworkLocationProviderGlue () {}

//overrides
void NetworkLocationProviderGlue :: onEnable () {
    LocationProviderGlue :: onEnable ();
}

void NetworkLocationProviderGlue :: onDisable () {
    LocationProviderGlue :: onDisable ();
}

void NetworkLocationProviderGlue :: onAddRequest (IzatRequest * request) {
    LocationProviderGlue :: onAddRequest (request);
}

void NetworkLocationProviderGlue :: onRemoveRequest (IzatRequest * request) {
    LocationProviderGlue :: onRemoveRequest (request);
}

IzatProviderStatus NetworkLocationProviderGlue :: getStatus () {
    return LocationProviderGlue :: getStatus ();
}

int64 NetworkLocationProviderGlue :: getStatusUpdateTime () {
    return LocationProviderGlue :: getStatusUpdateTime ();
}

IzatLocationListenerMask NetworkLocationProviderGlue :: listensTo () const {
    return LocationProviderGlue :: listensTo ();
}

void NetworkLocationProviderGlue :: onLocationChanged
(
    const IzatLocation * location,
    const IzatLocationStatus status
)
{
    LocationProviderGlue :: onLocationChanged (location, status);
}

void NetworkLocationProviderGlue :: onStatusChanged
(
    const IzatProviderStatus status
)
{
    LocationProviderGlue :: onStatusChanged (status);
}

