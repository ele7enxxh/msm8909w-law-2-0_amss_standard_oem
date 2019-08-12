/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Izat Manager

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef __IZAT_MANAGER_IZATMANAGER_H__
#define __IZAT_MANAGER_IZATMANAGER_H__

#include <set>
#include <string>
#include <comdef.h>
#include "IIzatManager.h"
#include "IIPCMessagingResponse.h"
#include "ILocationResponse.h"
#include "IDataItemObserver.h"
#include "ulp_service.h"
#include "IzatLocation.h"
#include "IzatContext.h"
#include "XtraSubscriber.h"

namespace izat_manager
{

class LocationReport;
class LocationError;
class IPCMessagingProxy;


class IzatManager :
    public IIzatManager,
    public IIPCMessagingResponse,
    public ILocationResponse,
    public IDataItemObserver {
public:
    static IzatManager* getInstance(IOSFramework* pOSFrameworkObj);
    static int destroyInstance();
    virtual ~IzatManager();

    // IIzatManager overrides
    virtual int32 subscribeListener(IOSLocationListener* osLocationListener);
    virtual int32 unsubscribeListener(IOSLocationListener* osLocationListener);
    virtual int32 addRequest(const IzatRequest* request);
    virtual int32 removeRequest(const IzatRequest* request);
    virtual int32 enableProvider(const IzatRequest::IzatProviderType provider);
    virtual int32 disableProvider(const IzatRequest::IzatProviderType provider);

    // ILocationResponse overrides
    virtual void reportLocation(const LocationReport* report, const ILocationProvider* providerSrc = NULL);
    virtual void reportError(const LocationError* error, const ILocationProvider* providerSrc = NULL);

    // IIPCMessagingResponse overrides
    virtual void handleMsg(InPostcard* const in_card);

    // IDataItemObserver overrides
    virtual void getName (std :: string & name);
    virtual void notify(const std::list<IDataItem *>& dlist);

    static const std::string mConfFile;

private:
    // Internal Methods
    IzatManager();
    int init(IOSFramework *pOSFrameworkObj);
    int deinit();

    int32 updateCriteria(const IzatRequest* request, bool add);
    static UlpLocationCriteria createUlpCritera(const IzatRequest *req, bool add);
    static IzatLocation izatLocationFromUlpLocation(const UlpLocation *ulpLoc);

    // Functions to handle START-GNSS-FIX/STOP-GNSS-FIX coming from XT-CS
    void handleStartGnssFix();
    void handleStopGnssFix();

    void handleNLPSettingChange(bool nlpSettingsEnabled);

    void printUlpLocation (UlpLocation * ulpLocation, enum loc_sess_status status);

    // Data Types
    typedef set<IOSLocationListener*> TListenerCol;

    // Class instance
    static IzatManager* mIzatManager;

    // Data members
    const std::string mSelfName;

    s_IzatContext mIzatContext;
    TListenerCol mListeners;
    bool mFLPProviderEnabled;
    bool mNLPProviderEnabled;
    ILocationResponse *mWiper;
    bool mHandlingGnssFix;

    // Network provider stuff
    ILocationProvider* mComboNetworkProvider;
    ILocationProvider* mOSNetworkProvider;
    ILocationProvider* mCurrentNetworkProvider;

    // ULP Interfaces
    const UlpEngineInterface* mUlpEngineInterface;
    const UlpNetworkInterface* mUlpNetworkInterface;
    const UlpPhoneContextInterface* mUlpPhoneContextInterface;
    bool mUlpStarted;

    // ULP Callbacks
    static UlpEngineCallbacks mUlpEngineCallbacks;
    static UlpNetworkLocationCallbacks mUlpNetworkLocationCallbacks;
    static UlpPhoneContextCallbacks mUlpPhoneContextCallbacks;
    static void ulp_network_location_request_cb (UlpNetworkRequestPos* req);
    static void ulp_request_phone_context_cb(UlpPhoneContextRequest *req);
    static void location_callback(UlpLocation* location, enum loc_sess_status status);
    static void gnss_status_callback (const uint16 status);

    // Async messages
    struct networkLocationRequestMsg : public LocMsg {
        IzatManager *mIzatManager;
        UlpNetworkRequestPos mRequest;

        inline networkLocationRequestMsg(IzatManager *mgrObj,
            const UlpNetworkRequestPos *req)
        :mIzatManager(mgrObj) { mRequest = *req; }

        virtual void proc() const;
    };

    struct locationCallbackMsg : public LocMsg {
        IzatManager *mIzatManager;
        IzatLocation mLocation;
        uint16_t mSource;
        IzatLocationStatus mStatus;

        inline locationCallbackMsg(IzatManager *mgrObj, const UlpLocation *loc, const IzatLocationStatus status)
        :mIzatManager(mgrObj)
        {
            mLocation = izatLocationFromUlpLocation(loc);
            mSource = loc->position_source;
            mStatus = status;
        }

        virtual void proc() const;
    };

    struct gnssStatusCallbackMsg : public LocMsg {
        IzatManager * mIzatManager;
        IzatProviderStatus mStatus;
        inline gnssStatusCallbackMsg (IzatManager * mgrObj, const uint16 status)
        : mIzatManager (mgrObj)
        {
            int lookup [] = {
                IZAT_PROVIDER_GNSS_STATUS_NONE,
                IZAT_PROVIDER_GNSS_STATUS_SESSION_BEGIN,
                IZAT_PROVIDER_GNSS_STATUS_SESSION_END,
                IZAT_PROVIDER_GNSS_STATUS_ENGINE_ON,
                IZAT_PROVIDER_GNSS_STATUS_ENGINE_OFF
            };
            if (status > 4) {
                mStatus = IZAT_PROVIDER_GNSS_STATUS_NONE;
            } else {
                mStatus = static_cast<IzatProviderStatus> (lookup [status]);
            }
        }
        virtual void proc () const;
    };

    struct phoneContextRequestMsg : public LocMsg {
        IzatManager *mIzatManager;

        inline phoneContextRequestMsg(IzatManager *mgrObj)
        : mIzatManager(mgrObj) {}

        virtual void proc() const;
    };

    struct phoneContextResponseMsg : public LocMsg {
        std::list <IDataItem *> mDataItemList;
        IzatManager *mIzatManager;

        phoneContextResponseMsg(IzatManager *mgrObj,
            const std::list<IDataItem *> & dataItemList);

        virtual void proc() const;
        virtual ~phoneContextResponseMsg();
    };

    struct deviceEventRequestMsg : public LocMsg {
        IzatManager *mIzatManager;

        inline deviceEventRequestMsg(IzatManager *mgrObj)
        :mIzatManager(mgrObj) {}

        virtual void proc() const;
    };

    struct subscribeListenerMsg : public LocMsg {
        IzatManager* mIzatManager;
        IOSLocationListener* mListener;

        inline subscribeListenerMsg(IzatManager *mgrObj,
            IOSLocationListener* listener)
        :mIzatManager(mgrObj), mListener(listener) {}

        virtual void proc() const;
    };

    struct unsubscribeListenerMsg : public LocMsg {
        IzatManager* mIzatManager;
        IOSLocationListener* mListener;

        inline unsubscribeListenerMsg(IzatManager *mgrObj,
            IOSLocationListener* listener)
        :mIzatManager(mgrObj), mListener(listener) {}

        virtual void proc() const;
    };

    struct updateCriteriaMsg : public LocMsg {
        IzatManager* mIzatManager;
        UlpLocationCriteria mCriteria;

        inline updateCriteriaMsg(IzatManager *mgrObj, const UlpLocationCriteria& crit)
        :mIzatManager(mgrObj) {
            memcpy(&mCriteria, &crit, sizeof(UlpLocationCriteria));
        }

        virtual void proc() const;
    };

    struct enableProviderMsg : public LocMsg {
        IzatManager* mIzatManager;
        IzatRequest::IzatProviderType mProvider;

        inline enableProviderMsg(IzatManager *mgrObj, IzatRequest::IzatProviderType prov)
        :mIzatManager(mgrObj)
        ,mProvider(prov) {}

        virtual void proc() const;
    };

    struct disableProviderMsg : public LocMsg {
        IzatManager* mIzatManager;
        IzatRequest::IzatProviderType mProvider;

        inline disableProviderMsg(IzatManager *mgrObj, IzatRequest::IzatProviderType prov)
        :mIzatManager(mgrObj)
        ,mProvider(prov) {}

        virtual void proc() const;
    };

    struct startStopFixMsg : public LocMsg {
        IzatManager* mIzatManager;
        bool mStart;

        inline startStopFixMsg(IzatManager *mgrObj, bool start)
        :mIzatManager(mgrObj)
        ,mStart(start) {}

        virtual void proc() const;
    };

    struct startPassiveLocationListenerMsg : public LocMsg {
        IzatManager *mIzatManager;
        std::string mClientName;
        IzatLocationListenerMask mListensTo;
        bool mIntermediateFixAccepted;

        inline startPassiveLocationListenerMsg(IzatManager *mgrObj,
                                               const char *clientName,
                                               IzatLocationListenerMask listensTo,
                                               bool intermediateFixAccepted)
            : mIzatManager(mgrObj)
            , mClientName(clientName)
            , mListensTo(listensTo)
            , mIntermediateFixAccepted(intermediateFixAccepted)
            { }

        virtual void proc() const;
    };

    // XTRA 2.0
    XtraSubscriber* mXtraSubscriber;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IZATMANAGER_H__
