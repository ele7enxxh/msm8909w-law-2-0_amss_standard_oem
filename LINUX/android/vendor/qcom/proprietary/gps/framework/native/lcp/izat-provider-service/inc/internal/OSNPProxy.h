/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  OS Network Provider Proxy module

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#ifndef __IZAT_MANAGER_OSNPPROXY_H__
#define __IZAT_MANAGER_OSNPPROXY_H__

#include "MsgTask.h"
#include "IIzatLocationListener.h"
#include "ILocationProvider.h"
#include "IzatContext.h"
#include "LocationReport.h"

namespace izat_manager {
class IzatLocation;
class ILocationResponse;
class IOSLocationProvider;
class IzatRequest;

/**
 * OSNPProxy may receive calls from different threads, hence methods may
 * be re-entrant
 * It does not support MULTI CLIENT. Every client must create its own instance
 */
class OSNPProxy :
public IIzatLocationListener,
public ILocationProvider {

public:
    static OSNPProxy* createInstance(const struct s_IzatContext* ctx);
    virtual ~OSNPProxy();

    // IIzatLocationListener overrides
    virtual void onLocationChanged(const IzatLocation* location, const IzatLocationStatus status);
    virtual void onStatusChanged(IzatProviderStatus status);

    // ILocationProvider overrides
    virtual int setRequest(const LocationRequest* request);
    /**
     * OSNPProxy is MONO client, hence last respObject provided will be
     * used to report locations
     */
    virtual void subscribe(const ILocationResponse* respObject);
    /**
     * unsubscribe() does not check if provided passObject matches the object
     * provided in subscribe() call
     */
    virtual void unsubscribe(const ILocationResponse* respObject);
    virtual void enable();
    virtual void disable();

private:
    // Internal methods
    OSNPProxy(const struct s_IzatContext* ctx, IOSLocationProvider *pOSNPProvider);

    // Data members
    const s_IzatContext *mIzatContext;
    ILocationResponse* mClient;
    IOSLocationProvider *mOSNPProvider;
    bool mEnabled;

    // Async messages
    struct onLocationChangedMsg : public LocMsg {
        OSNPProxy* mOSNPProxy;
        LocationReport mLocation;

        onLocationChangedMsg(OSNPProxy* pOSNPProxy, const IzatLocation* loc)
        : mOSNPProxy(pOSNPProxy), mLocation(*loc)
        {
          mLocation.mHasProviderSource = true;
          mLocation.mProviderSource = LocationReport::PROVIDER_SOURCE_NETWORK;
          mLocation.mHasProcessorSource = true;
          mLocation.mProcessorSource = LocationReport::ProcessorSrc_AP;
        }
        virtual void proc() const;
    };

    struct onSetRequestMsg : public LocMsg {
        OSNPProxy* mOSNPProxy;
        LocationRequest mRequest;

        onSetRequestMsg(OSNPProxy* pOSNPProxy, const LocationRequest* request)
        : mOSNPProxy(pOSNPProxy), mRequest(*request) {}
        virtual void proc() const;
    };

    struct subscribeMsg : public LocMsg {
        OSNPProxy* mOSNPProxy;
        ILocationResponse* mLocResponse;

        subscribeMsg(OSNPProxy* pOSNPProxy, ILocationResponse* locRes)
        : mOSNPProxy(pOSNPProxy), mLocResponse(locRes) {}
        virtual void proc() const;
    };

    struct unsubscribeMsg : public LocMsg {
        OSNPProxy* mOSNPProxy;
         ILocationResponse* mLocResponse;

        unsubscribeMsg(OSNPProxy* pOSNPProxy, ILocationResponse* locRes)
        : mOSNPProxy(pOSNPProxy), mLocResponse(locRes) {}
        virtual void proc() const;
    };

    struct enableMsg : public LocMsg {
        OSNPProxy* mOSNPProxy;

        enableMsg(OSNPProxy* pOSNPProxy)
        : mOSNPProxy(pOSNPProxy) {}
        virtual void proc() const;
    };

    struct disableMsg : public LocMsg {
        OSNPProxy* mOSNPProxy;

        disableMsg(OSNPProxy* pOSNPProxy)
        : mOSNPProxy(pOSNPProxy) {}
        virtual void proc() const;
    };


};

} // izat_manager

#endif // #ifndef __IZAT_MANAGER_OSNPPROXY_H__
