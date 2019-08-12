/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  OS Network Provider Proxy module

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#define LOG_NDDEBUG 0
#define LOG_TAG "IzatSvc_OSNPProxy"


#include "OSNPProxy.h"
#include "IzatDefines.h"
#include "LocationReport.h"
#include "IzatRequest.h"
#include "IOSLocationProvider.h"
#include "IOSFramework.h"
#include <log_util.h>

using namespace izat_manager;


OSNPProxy* OSNPProxy::createInstance(const struct s_IzatContext* ctx) {
    ENTRY_LOG();

    int result = -1;
    OSNPProxy* pOSNPProxy = NULL;

    do {
        BREAK_IF_ZERO(1, ctx);
        BREAK_IF_ZERO(2, ctx->mMsgTask);
        BREAK_IF_ZERO(3, ctx->mOSFrameworkObj);

        IOSLocationProvider *pOSNPProvider = ctx->mOSFrameworkObj->getOSNP();
        BREAK_IF_ZERO(4, pOSNPProvider);

        pOSNPProxy = new (std::nothrow) OSNPProxy(ctx, pOSNPProvider);
        BREAK_IF_ZERO(5, pOSNPProxy);
        result = 0;
    } while(0);

    if (result != 0) {
        delete pOSNPProxy;
        pOSNPProxy = NULL;
    }

    EXIT_LOG_WITH_ERROR("%d", result);
    return pOSNPProxy;
}

OSNPProxy::OSNPProxy(const struct s_IzatContext* ctx, IOSLocationProvider *pOSNPProvider)
: mIzatContext(ctx)
, mClient(NULL)
, mOSNPProvider(pOSNPProvider)
, mEnabled(false) {
};

OSNPProxy::~OSNPProxy() {
    if (mEnabled)
    {
        mOSNPProvider->stopRequest();
        mOSNPProvider->unsubscribe(this);
        mEnabled = false;
    }
}

// IIzatLocationListener overrides
void OSNPProxy::onLocationChanged(const IzatLocation* location, const IzatLocationStatus status) {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, status == IZAT_LOCATION_STATUS_FINAL)
        BREAK_IF_ZERO(2, location);

        onLocationChangedMsg *msg = new
            (nothrow) onLocationChangedMsg(this, location);
        BREAK_IF_ZERO(3, msg);
        mIzatContext->mMsgTask->sendMsg(msg);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void OSNPProxy::onStatusChanged(IzatProviderStatus status) {
    ENTRY_LOG();
    LOC_LOGD("OSNPProxy status: %d", status);
    EXIT_LOG_WITH_ERROR("%d", 0);
}

// ILocationProvider overrides
int OSNPProxy::setRequest(const LocationRequest* request) {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, request);

        onSetRequestMsg *msg = new (nothrow) onSetRequestMsg(this, request);
        BREAK_IF_ZERO(2, msg);
        mIzatContext->mMsgTask->sendMsg(msg);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

void OSNPProxy::subscribe(const ILocationResponse* respObject) {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, respObject);
        subscribeMsg *msg = new (nothrow) subscribeMsg(this,
            const_cast<ILocationResponse*>(respObject));
        BREAK_IF_ZERO(2, msg);
        mIzatContext->mMsgTask->sendMsg(msg);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void OSNPProxy::unsubscribe(const ILocationResponse* respObject) {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, respObject);
        unsubscribeMsg *msg = new (nothrow) unsubscribeMsg(this,
            const_cast<ILocationResponse*>(respObject));

        BREAK_IF_ZERO(2, msg);
        mIzatContext->mMsgTask->sendMsg(msg);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void OSNPProxy::enable() {
    ENTRY_LOG();
    mIzatContext->mMsgTask->sendMsg(new (nothrow) enableMsg(this));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void OSNPProxy::disable() {
    ENTRY_LOG();
    mIzatContext->mMsgTask->sendMsg(new (nothrow) disableMsg(this));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

// Async messages
void OSNPProxy::onLocationChangedMsg::proc() const {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, mOSNPProxy->mEnabled); // Disabled
        BREAK_IF_ZERO(2, mOSNPProxy->mClient); // No subscription active

        mOSNPProxy->mClient->reportLocation(&mLocation, mOSNPProxy);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
};

void OSNPProxy::onSetRequestMsg::proc() const {
    ENTRY_LOG();

    int result = -1;
    IzatRequest *izatReq = NULL;
    do {
        BREAK_IF_ZERO(1, mOSNPProxy->mEnabled); // Disabled

        if ((LocationRequestAction_Start == mRequest.action) ||
            (LocationRequestAction_SingleShot == mRequest.action)) {
            izatReq = IzatRequest::createProviderRequest(
                        IZAT_STREAM_NETWORK,
                        mRequest.intervalInMsec, // timeBetweenFixes
                        0.0f, // distanceBetweenFixes
                        (LocationRequestAction_SingleShot == mRequest.action) // singleShot
                        );
            BREAK_IF_ZERO(2, izatReq);
            BREAK_IF_NON_ZERO(3, mOSNPProxy->mOSNPProvider->setRequest(izatReq));
        } else if (LocationRequestAction_Stop == mRequest.action) {
            mOSNPProxy->mOSNPProvider->stopRequest();
        } else {
            LOC_LOGW("Unexpected request action: %d", mRequest.action);
        }
        result = 0;
    } while(0);

    delete izatReq;
    izatReq = NULL;

    EXIT_LOG_WITH_ERROR("%d", result);
};

void OSNPProxy::subscribeMsg::proc() const {
    ENTRY_LOG();
    mOSNPProxy->mClient = mLocResponse;
    EXIT_LOG_WITH_ERROR("%d", 0);
};

void OSNPProxy::unsubscribeMsg::proc() const {
    ENTRY_LOG();
    mOSNPProxy->mClient = NULL;
    EXIT_LOG_WITH_ERROR("%d", 0);
};

void OSNPProxy::enableMsg::proc() const {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_NON_ZERO(1, mOSNPProxy->mEnabled);
        BREAK_IF_NON_ZERO(2, mOSNPProxy->mOSNPProvider->subscribe(mOSNPProxy))
        mOSNPProxy->mEnabled = true;
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
};

void OSNPProxy::disableMsg::proc() const {
    ENTRY_LOG();

    int result = -1;
    do {
        BREAK_IF_ZERO(1, mOSNPProxy->mEnabled);
        mOSNPProxy->mOSNPProvider->stopRequest();
        BREAK_IF_NON_ZERO(2, mOSNPProxy->mOSNPProvider->unsubscribe(mOSNPProxy))
        mOSNPProxy->mEnabled = false;
        result = -1;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
};
