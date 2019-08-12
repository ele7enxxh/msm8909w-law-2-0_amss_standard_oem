/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Fused Location Provider Glue

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __FUSEDLOCATIONPROVIDERGLUE_H__
#define __FUSEDLOCATIONPROVIDERGLUE_H__

#include <comdef.h>
#include <LocationProviderGlue.h>

using namespace izat_manager;

class FusedLocationProviderGlue
    : public LocationProviderGlue {
public:

    static FusedLocationProviderGlue * getInstance ();
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
    FusedLocationProviderGlue ();
    ~FusedLocationProviderGlue ();

    static FusedLocationProviderGlue * mInstance;
};

#endif // #ifndef __FUSEDLOCATIONPROVIDERGLUE_H__

