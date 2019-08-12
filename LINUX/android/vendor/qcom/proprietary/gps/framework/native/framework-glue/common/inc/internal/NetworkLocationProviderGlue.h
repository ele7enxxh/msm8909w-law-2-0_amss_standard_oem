/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Network Location Provider Glue

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

#ifndef __NETWORKLOCATIONPROVIDERGLUE_H__
#define __NETWORKLOCATIONPROVIDERGLUE_H__

#include <comdef.h>
#include <LocationProviderGlue.h>

using namespace izat_manager;

class NetworkLocationProviderGlue
    : public LocationProviderGlue {
public:

    static NetworkLocationProviderGlue * getInstance ();
    static void destroyInstance ();

    //overrides
    void setIzatManagerInstance (IIzatManager * izatManager);

    void onEnable ();
    void onDisable ();
    void onAddRequest (IzatRequest * request);
    void onRemoveRequest (IzatRequest * request);
    IzatProviderStatus getStatus ();
    int64 getStatusUpdateTime ();
    IzatLocationListenerMask listensTo () const;
    void onLocationChanged (const IzatLocation * location, const IzatLocationStatus status);
    void onStatusChanged (const IzatProviderStatus status);

private:
    NetworkLocationProviderGlue ();
    ~NetworkLocationProviderGlue ();

    static NetworkLocationProviderGlue * mInstance;
};

#endif // #ifndef __NETWORKLOCATIONPROVIDERGLUE_H__

