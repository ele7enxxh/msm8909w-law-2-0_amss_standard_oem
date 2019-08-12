/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#ifndef __IZAT_MANAGER_WIPER_H__
#define __IZAT_MANAGER_WIPER_H__

#include "MsgTask.h"
#include "IFreeWifiScanObserver.h"
#include "ILocationResponse.h"
#include "IDataItemObserver.h"
#include "IIPCMessagingRequest.h"
#include "LBSAdapter.h"
#include "FreeWifiScanObserver.h"
#include <base_util/vector.h>
#include "IzatContext.h"
#include "OSObserver.h"
#include "lowi_response.h"
#include "LocationReport.h"
#include "DataItemConcreteTypes.h"
#include "LocTimer.h"

namespace qc_loc_fw {
class InPostcard;
class LOWIScanMeasurement;
class LOWIAsyncDiscoveryScanResultResponse;
}

namespace izat_manager
{

using namespace std;
using namespace loc_core;
using namespace izat_manager;

class Wiper :
    public IFreeWifiScanObserver,
    public ILocationResponse,
    public IDataItemObserver {

public:

    // Singleton methods
    static Wiper * getInstance(const struct s_IzatContext * izatContex);
    static void destroyInstance();

    // set feature registration bits and initialize/update LBSAdapter with event mask
    void setFeatureRegistrationBits();

    // IFreeWifiScanObserver overrides
    virtual void notify(const LOWIAsyncDiscoveryScanResultResponse * response);

    // ILocationResponse, overrides
    virtual void reportLocation(const LocationReport *location_report, const ILocationProvider *providerSrc = NULL);
    virtual void reportError(const LocationError *location_error, const ILocationProvider *providerSrc = NULL);

    // IDataItemObserver overrides
    virtual void getName (string & name);
    virtual void notify(const std::list<IDataItem *> & dlist);

    // static function call for messages received from LBSAdapter
    static void handleNetworkLocationRequest(WifiRequestType type);
    static void handleWifiApDataRequest();
    static void handleSsrInform();
    static void handleTimeZoneInfoRequest();

private:
    class Timer : public LocTimer {
        Wiper* mClient;
    public:
        inline Timer(Wiper* client) :
            LocTimer(), mClient(client) {}
        virtual void timeOutCallback();
    };
    friend class Timer;

    typedef enum WiperFeatureFlag {
        FEATURE_ODCPI_MASK = 0x1000,
        FEATURE_FREE_WIFI_SCAN_INJECT_MASK = 0x2000,
        FEATURE_SUPL_WIFI_MASK = 0x4000,
        FEATURE_WIFI_SUPPLICANT_INFO_MASK = 0x8000
    } WiperFeatureFlag;

    typedef enum ActiveNlpSession {
        NO_ACTIVE_NLP_SESSION,
        HIGH_FREQ_ACTIVE_NLP_SESSION,
        LOW_FREQ_ACTIVE_NLP_SESSION,
        EMERGENCY_ACTIVE_NLP_SESSION
    } ActiveNlpSession;

    // Data members
    static Wiper                  * mInstance;
    const struct s_IzatContext    * mIzatContext;
    LBSAdapter                    * mLbsAdapter;
    FreeWifiScanObserver          * mWifiScanObserver;
    int                             mWiperFlag;
    int                             mWifiWaitTimeoutSelect;
    ActiveNlpSession                mActiveNlpSession;
    bool                            mIsWifiScanInSession;
    WifiLocation                    mWifiLocation;
    WifiSupplicantStatusDataItem::WifiSupplicantState    mLatestSupplicantState;

    static const int HIGH_FREQ_PERIOD = 3500; //3.5 seconds
    static const int LOW_FREQ_PERIOD = 5000; //5 seconds
    static const int EMERGENCY_FREQ_PERIOD = 2000; // 2 seconds
    Timer                           mEmergencyODCPITimer;

    Wiper(const struct s_IzatContext * izatContext,
        unsigned int wiperFlag, int wifi_wait_timeout_select);
    ~Wiper();

    static void stringify (CoarsePositionInfo & cpi, string & valueStr);
    static void stringify (WifiSupplicantInfo & wsi, string & valueStr);
    static void stringify (WifiApInfo & wai, string & valueStr);
    static void stringify (WifiApSsidInfo & ws, string & valueStr);
    static void stringify (WifiLocation & wl, string & valueStr, bool printEmbedded);

    struct networkLocationRequestMsg : public LocMsg {
        Wiper *mWiperObj;
        WifiRequestType mRequestType;

        inline networkLocationRequestMsg(Wiper *wiperObj, WifiRequestType requestType) :
            mWiperObj(wiperObj), mRequestType(requestType) {
        }

        inline ~networkLocationRequestMsg() {}
        virtual void proc() const;
    };

    struct wifiScanNotificationMsg : public LocMsg {
        Wiper *mWiperObj;
        qc_loc_fw::vector<LOWIScanMeasurement *> mLowiScanMeasurements;

        wifiScanNotificationMsg(Wiper *wiperObj, qc_loc_fw::vector<LOWIScanMeasurement *> lowiScanMeasurements);

        ~wifiScanNotificationMsg();
        virtual void proc() const;
    };

    struct injectNetworkLocationMsg : public LocMsg {
        Wiper *mWiperObj;
        const LocationReport mLocReport;

        inline injectNetworkLocationMsg(Wiper *wiperObj, LocationReport locReport) :
            mWiperObj(wiperObj), mLocReport(locReport) {
        }

        inline ~injectNetworkLocationMsg() {}
        virtual void proc() const;
    };

    struct wifiScanRequestMsg : public LocMsg {
        Wiper *mWiperObj;

        inline wifiScanRequestMsg(Wiper *wiperObj) :
            mWiperObj(wiperObj) {
        }

        inline ~wifiScanRequestMsg() {}
        virtual void proc() const;
    };

    struct handleOsObserverUpdateMsg : public LocMsg {
        Wiper *mWiperObj;
        std::list <IDataItem *> mDataItemList;

        handleOsObserverUpdateMsg(Wiper *wiperObj,
            const std::list<IDataItem *> & dataItemList);

        ~handleOsObserverUpdateMsg();
        virtual void proc() const;
    };

    struct handleSsrMsg : public LocMsg {
        Wiper *mWiperObj;

        inline handleSsrMsg(Wiper *wiperObj) :
            mWiperObj(wiperObj) {
        }

        inline ~handleSsrMsg() {}
        virtual void proc() const;
    };

    struct wiperSubscribeInit : public LocMsg {
        Wiper *mWiperObj;
        int mWiperFlag;

        wiperSubscribeInit(Wiper *wiperObj, int wiperFlag) :
            mWiperObj(wiperObj), mWiperFlag(wiperFlag) {
        }

        inline ~wiperSubscribeInit() {}
        virtual void proc() const;
    };

    struct lbsShutDownMsg : public LocMsg {
        ContextBase* mContext;
        inline lbsShutDownMsg(ContextBase* context) :
            LocMsg(), mContext(context) {}
        inline ~lbsShutDownMsg() {
            ((MsgTask*)mContext->getMsgTask())->destroy();
            delete mContext;
        }
        void proc() const;
    };

    struct handleTimeZoneInfoRequestMsg : public LocMsg {
        Wiper *mWiperObj;

        inline handleTimeZoneInfoRequestMsg(Wiper *wiperObj) :
            mWiperObj(wiperObj) {
        }

        inline ~handleTimeZoneInfoRequestMsg() {}
        virtual void proc() const;
    };

    struct killOdcpiMsg : public LocMsg {
        Wiper* mWiperObj;

        inline killOdcpiMsg(Wiper *pWiperObj) : mWiperObj(pWiperObj) {}
        virtual void proc() const;
        inline ~killOdcpiMsg() {}
    };

};

class LBSShutDownAdapter : public LBSAdapterBase {
public:
    inline LBSShutDownAdapter(ContextBase* context) :
        LBSAdapterBase(0, context) {}
};


} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_WIPER_H__

