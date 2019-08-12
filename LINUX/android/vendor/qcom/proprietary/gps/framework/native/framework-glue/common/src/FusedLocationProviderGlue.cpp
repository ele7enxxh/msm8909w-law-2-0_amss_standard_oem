/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <comdef.h>
#include <IzatTypes.h>
#include <IzatRequest.h>
#include <IzatLocation.h>
#include <FusedLocationProviderGlue.h>
#include <IzatProviderEngine_jni.h>
#include <IIzatManager.h>

using namespace izat_manager;

FusedLocationProviderGlue * FusedLocationProviderGlue :: mInstance = NULL;

FusedLocationProviderGlue * FusedLocationProviderGlue :: getInstance () {

    if (!mInstance) {
        mInstance = new FusedLocationProviderGlue ();
    }

    return mInstance;
}

void FusedLocationProviderGlue :: destroyInstance () {
    if (mInstance) {
        mInstance->mIzatManager->disableProvider
        (
            mInstance->mLocationStreamType
        );
        mInstance->mIzatManager->unsubscribeListener (mInstance);
    }
}

// ctor
FusedLocationProviderGlue :: FusedLocationProviderGlue ()
: LocationProviderGlue (IZAT_STREAM_FUSED) {}

// dtor
FusedLocationProviderGlue :: ~FusedLocationProviderGlue () {}

//overrides
void FusedLocationProviderGlue :: onEnable () {
    LocationProviderGlue :: onEnable ();
}

void FusedLocationProviderGlue :: onDisable () {
    LocationProviderGlue :: onDisable ();
}

void FusedLocationProviderGlue :: onAddRequest (IzatRequest * request) {
    LocationProviderGlue :: onAddRequest (request);
}

void FusedLocationProviderGlue :: onRemoveRequest (IzatRequest * request) {
    LocationProviderGlue :: onRemoveRequest (request);
}

IzatProviderStatus FusedLocationProviderGlue :: getStatus () {
    return LocationProviderGlue :: getStatus ();
}

int64 FusedLocationProviderGlue :: getStatusUpdateTime () {
    return LocationProviderGlue :: getStatusUpdateTime ();
}

IzatLocationListenerMask FusedLocationProviderGlue :: listensTo () const {
    return LocationProviderGlue :: listensTo ();
}

void FusedLocationProviderGlue :: onLocationChanged
(
    const IzatLocation * location,
    const IzatLocationStatus status
)
{
    LocationProviderGlue :: onLocationChanged (location, status);
}

void FusedLocationProviderGlue :: onStatusChanged
(
    const IzatProviderStatus status
)
{
    LocationProviderGlue :: onStatusChanged (status);
}


