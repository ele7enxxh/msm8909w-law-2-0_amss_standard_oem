/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Passive Location Listener

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZATPASSIVELOCATIONLISTENER_H__
#define __IZATPASSIVELOCATIONLISTENER_H__

#include <map>
#include <comdef.h>
#include <IzatTypes.h>
#include <IzatLocation.h>
#include <IOSLocationListener.h>

namespace izat_manager {
class IzatLocation;

using namespace izat_manager;

class IzatPassiveLocationListener
    : public IOSLocationListener {
public:

    static void startPassiveLocationListener(const std::string clientName,
                                             const IzatLocationListenerMask listensTo,
                                             bool intermediateFixAccepted,
                                             IIzatManager * izatManager,
                                             IPCMessagingProxy *iPCMessagingProxy);
    static void stopAllPassiveLocationListers(IIzatManager * izatManager);

    // override IOSLocationListener
    IzatLocationListenerMask listensTo() const;
    void onLocationChanged(const IzatLocation *location, const IzatLocationStatus status);
    void onStatusChanged(const IzatProviderStatus status);

private:

    typedef std::map<const std::string, IzatPassiveLocationListener *> IzatListenerMapType;
    static IzatListenerMapType sObjects;

    const std::string mClientName;
    const std::string mIpcName;
    bool mIntermediateFixAccepted;
    const IzatLocationListenerMask mLocationListenerMask;
    IPCMessagingProxy *mIPCMessagingProxy;

    IzatPassiveLocationListener(const std::string clientName,
                                const IzatLocationListenerMask listensTo,
                                bool intermediateFixAccepted,
                                IIzatManager * izatManager,
                                IPCMessagingProxy *iPCMessagingProxy);
    ~IzatPassiveLocationListener();
};

} // namespace izat_manager
#endif // #ifndef __IZATPASSIVELOCATIONLISTENER_H__

