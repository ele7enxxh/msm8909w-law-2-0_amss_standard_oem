/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Network Location Provider Glue

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

#ifndef __LOCATIONPROVIDERGLUE_H__
#define __LOCATIONPROVIDERGLUE_H__

#include <comdef.h>
#include <IzatTypes.h>
#include <IOSLocationListener.h>

namespace izat_manager {
class IzatRequest;
class IzatLocation;
class IIzatManager;
}

using namespace izat_manager;

class LocationProviderGlue
    : public IOSLocationListener {
public:

    void onEnable ();
    void onDisable ();
    void onAddRequest (IzatRequest * request);
    void onRemoveRequest (IzatRequest * request);
    IzatProviderStatus getStatus ();
    int64 getStatusUpdateTime ();

    // override IOSLocationListener
    IzatLocationListenerMask listensTo () const;
    void onLocationChanged (const IzatLocation * location, const IzatLocationStatus status);
    void onStatusChanged (const IzatProviderStatus status);

protected:
    LocationProviderGlue (IzatLocationStreamType streamType);
    ~LocationProviderGlue ();

    IzatLocationStreamType mLocationStreamType;
    IIzatManager * mIzatManager;
    IzatProviderStatus mStatus;
};

#endif // #ifndef __LOCATIONPROVIDERGLUE_H__

