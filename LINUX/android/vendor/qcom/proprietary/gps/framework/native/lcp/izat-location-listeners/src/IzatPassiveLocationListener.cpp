/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <IzatDefines.h>
#include <IzatManager.h>
#include <IzatPassiveLocationListener.h>
#include <Utils.h>

#define LOG_NDDEBUG 0
#define LOG_TAG "IzatSvc_PassiveLocListener"

using namespace izat_manager;
using namespace std;

IzatPassiveLocationListener::IzatListenerMapType IzatPassiveLocationListener::sObjects;

void IzatPassiveLocationListener :: startPassiveLocationListener(
    const std::string clientName,
    const IzatLocationListenerMask listensTo,
    bool intermediateFixAccepted,
    IIzatManager *izatManager,
    IPCMessagingProxy *iPCMessagingProxy) {
    int result = -1;
    do {
        if (NULL == sObjects[clientName]) {
            sObjects[clientName] =
                new(nothrow) IzatPassiveLocationListener(clientName, listensTo, intermediateFixAccepted,
                                                         izatManager, iPCMessagingProxy);
            BREAK_IF_ZERO(1, sObjects[clientName]);
            result = 0;
        }
    } while (0);
    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatPassiveLocationListener::stopAllPassiveLocationListers(IIzatManager *izatManager) {
    for (IzatListenerMapType::iterator it = sObjects.begin(); it != sObjects.end(); it++) {
        if(0 != izatManager->unsubscribeListener(it->second)) {
            LOC_LOGI("Failed to unsubcribe location listener for %s", it->first.c_str());
        }
        delete it->second;
    }
}


// ctor
IzatPassiveLocationListener :: IzatPassiveLocationListener(const std::string clientName,
                                                           IzatLocationListenerMask listensTo,
                                                           bool intermediateFixAccepted,
                                                           IIzatManager *izatManager,
                                                           IPCMessagingProxy *iPCMessagingProxy)
    : mClientName(clientName)
    , mIpcName(clientName + "-LOC-LISTENER")
    , mIntermediateFixAccepted(intermediateFixAccepted)
    , mLocationListenerMask(listensTo)
    , mIPCMessagingProxy(iPCMessagingProxy) {
    int result = -1;

    do {
        BREAK_IF_ZERO(1, izatManager);
        izatManager->subscribeListener(this);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

// dtor
IzatPassiveLocationListener :: ~IzatPassiveLocationListener() { }

//overrides
IzatLocationListenerMask IzatPassiveLocationListener :: listensTo() const {
    return mLocationListenerMask;
}

void IzatPassiveLocationListener :: onLocationChanged (const IzatLocation *location,
                                                          const IzatLocationStatus status)
{
    ENTRY_LOG();

    OutPostcard *card = NULL;
    int result = 0;
    do {
        if (status == IZAT_LOCATION_STATUS_INTERMEDIATE) {
          BREAK_IF_ZERO(1, mIntermediateFixAccepted);
        }

        BREAK_IF_ZERO(2, location);
        if (!location->isValid()) {
            result = 3;
            break;
        }
        BREAK_IF_ZERO(4, mIPCMessagingProxy);
        // send out the location update
        card = OutPostcard::createInstance();
        BREAK_IF_ZERO(5, card);
        BREAK_IF_NON_ZERO(6, card->init());
        BREAK_IF_NON_ZERO(7, card->addString("TO", mClientName.c_str()));
        BREAK_IF_NON_ZERO(8, card->addString("FROM", mIpcName.c_str()));
        BREAK_IF_NON_ZERO(9, card->addString("INFO", "LOCATION-UPDATE"));
        BREAK_IF_NON_ZERO(10, card->addDouble("LATITUDE", location->mLatitude));
        BREAK_IF_NON_ZERO(11, card->addDouble("LONGITUDE", location->mLongitude));
        BREAK_IF_NON_ZERO(12, card->addFloat("HORIZONTAL-ACCURACY", location->mHorizontalAccuracy));
        BREAK_IF_NON_ZERO(13, card->finalize());
        BREAK_IF_NON_ZERO(14, mIPCMessagingProxy->sendMsg(card, mClientName.c_str()));
    } while(0);

    delete card;

    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatPassiveLocationListener :: onStatusChanged
(
    const IzatProviderStatus
)
{
    // do nothing for now
}


