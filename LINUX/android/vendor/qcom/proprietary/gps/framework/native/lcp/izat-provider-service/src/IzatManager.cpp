/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Izat Manager

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <list>
#include <gps_extended_c.h>
#include "IzatManager.h"
#include "OSObserver.h"
#include "ComboNetworkProvider.h"
#include "OSNPProxy.h"
#include "IOSFramework.h"
#include "loc_cfg.h"
#include "Utils.h"
#include "IDataItem.h"
#include "DataItemId.h"
#include "IDataItemCopier.h"
#include "DataItemConcreteTypes.h"
#include "DataItemsFactory.h"
// #include "LCICandidatesReporter.h"

#include "IOSLocationListener.h"
#include "IzatPassiveLocationListener.h"
#include "Wiper.h"

#define LOG_NDDEBUG 0
#define LOG_TAG "IzatSvc_IzatManager"

namespace izat_manager
{
#ifdef __ANDROID__
const std::string IzatManager::mConfFile("/etc/gps.conf");
#else
const std::string IzatManager::mConfFile("gps.conf");
#endif

using namespace std;

static uint8 DEBUG_LEVEL = 0xff;

static loc_param_s_type gpsConfParamTable [] =
{
    {"DEBUG_LEVEL", &DEBUG_LEVEL, NULL, 'n'},
};

IzatManager* IzatManager::mIzatManager = NULL;

UlpEngineCallbacks IzatManager::mUlpEngineCallbacks = {
    sizeof(UlpEngineCallbacks),
    location_callback,
    gnss_status_callback,
    create_thread_callback
};

UlpNetworkLocationCallbacks IzatManager::mUlpNetworkLocationCallbacks = {
    ulp_network_location_request_cb
};

UlpPhoneContextCallbacks IzatManager::mUlpPhoneContextCallbacks = {
    ulp_request_phone_context_cb
};

IIzatManager* getIzatManager(IOSFramework *pOSFrameworkObj) {
   // for now keep the log levels read to be the first thing
    UTIL_READ_CONF_DEFAULT(IzatManager::mConfFile.c_str());

    ENTRY_LOG();

    return IzatManager::getInstance(pOSFrameworkObj);
}

int destroyIzatManager() {
    ENTRY_LOG();
    IzatManager::destroyInstance();
    return 0;
}

IzatManager* IzatManager::getInstance(IOSFramework* pOSFrameworkObj) {
    int result = -1;

    ENTRY_LOG();

    do {
        // already initialized
        BREAK_IF_NON_ZERO(0, mIzatManager);
        BREAK_IF_ZERO(2, pOSFrameworkObj);

        mIzatManager = new (std::nothrow) IzatManager();
        BREAK_IF_ZERO(3, mIzatManager);
        BREAK_IF_NON_ZERO(4, mIzatManager->init(pOSFrameworkObj))

        result = 0;
    } while(0);

    if (result != 0) {
        delete mIzatManager;
         mIzatManager = NULL;
    }

    EXIT_LOG_WITH_ERROR("%d", result);
    return mIzatManager;
}

int IzatManager::destroyInstance()
{
    if (NULL != mIzatManager) {
        delete mIzatManager;
        mIzatManager = NULL;
    }

    return 0;
}

IzatManager::IzatManager()
:mSelfName("IZAT-MANAGER")
,mFLPProviderEnabled(false)
,mNLPProviderEnabled(false)
,mComboNetworkProvider(NULL)
,mOSNetworkProvider(NULL)
,mCurrentNetworkProvider(NULL)
,mUlpEngineInterface(NULL)
,mUlpNetworkInterface(NULL)
,mUlpPhoneContextInterface(NULL)
,mUlpStarted(false)
,mHandlingGnssFix(false)
,mXtraSubscriber(NULL){
}

IzatManager::~IzatManager() {
    deinit();
}

int IzatManager::init(IOSFramework* pOSFrameworkObj) {
    int result = -1;

    ENTRY_LOG();

    do {
      // hold the pointer to framework object received in the s_IzatContext object
      mIzatContext.mOSFrameworkObj = pOSFrameworkObj;

      // hold the pointer the framework subscription object in the s_IzatContext.
      mIzatContext.mSubscriptionObj =
          mIzatContext.mOSFrameworkObj->getDataItemSubscriptionObj();
      BREAK_IF_ZERO(1, mIzatContext.mSubscriptionObj);

      // initialize the IzatContext MsgTask member
      mIzatContext.mMsgTask = (MsgTask*)LocDualContext::
          getLocFgContext(NULL,
                          mIzatContext.mOSFrameworkObj->getTaskSetupJob(),
                          "IZAT-MANAGER-THREAD",
                          false)->getMsgTask();
      BREAK_IF_ZERO(2, mIzatContext.mMsgTask);

      // initialize and register with the IPCMessagingProxy
      IPCMessagingProxy * ipcMessagingProxy =
          IPCMessagingProxy::getInstance();
      BREAK_IF_ZERO(3, ipcMessagingProxy);
      mIzatContext.mIPCMessagingProxyObj = ipcMessagingProxy;

      BREAK_IF_NON_ZERO(4,
          mIzatContext.mIPCMessagingProxyObj->registerResponseObj(mSelfName.c_str(), this));

      // initialize the OSObserver
      OSObserver * osObserver = OSObserver::getInstance(&mIzatContext);
      BREAK_IF_ZERO(5, osObserver);
      mIzatContext.mOSObserverObj = osObserver;

      // create a ComboNetworkProvider here and subscribe to it for location requests.
      mComboNetworkProvider = ComboNetworkProvider::getInstance(&mIzatContext);
      if (NULL != mComboNetworkProvider) {
          mComboNetworkProvider->subscribe(this);
      } else {
          LOC_LOGI("Combo Network provider is not available");
      }

      mOSNetworkProvider = OSNPProxy::createInstance(&mIzatContext);
      if (NULL != mOSNetworkProvider) {
          mCurrentNetworkProvider = mOSNetworkProvider;
          mOSNetworkProvider->subscribe(this);
          mOSNetworkProvider->enable();
      } else {
          LOC_LOGI("OS Network provider is not available");
      }

      // initialize the Wiper
      mWiper = Wiper::getInstance(&mIzatContext);
      BREAK_IF_ZERO(6, mWiper);

      // initialize libulp
      BREAK_IF_NON_ZERO(7, ulp_init(&mUlpEngineCallbacks,
                                    &mUlpNetworkLocationCallbacks,
                                    &mUlpPhoneContextCallbacks));

      // cache the ULP interfaces
      mUlpEngineInterface =
        static_cast<const UlpEngineInterface*>(ulp_get_extension
                                               (ULP_ENGINE_INTERFACE));
      BREAK_IF_ZERO(8, mUlpEngineInterface);

      mUlpNetworkInterface =
        static_cast<const UlpNetworkInterface*>(ulp_get_extension
                                                (ULP_NETWORK_INTERFACE));
      BREAK_IF_ZERO(9, mUlpNetworkInterface);

      mUlpPhoneContextInterface =
        static_cast<const UlpPhoneContextInterface*>(ulp_get_extension
                                                     (ULP_PHONE_CONTEXT_INTERFACE));
      BREAK_IF_ZERO(10, mUlpPhoneContextInterface);

      // send message to OSObserver to start listening for device events
      mIzatContext.mMsgTask->sendMsg(
          new (nothrow) deviceEventRequestMsg(IzatManager::mIzatManager));
      mIzatManager->mIzatContext.mMsgTask->sendMsg(
          new (nothrow)phoneContextRequestMsg(IzatManager::mIzatManager));

      // XTRA 2.0
      LOC_LOGD("[XTRA2] init XTRA subscriber");
      mXtraSubscriber = new (nothrow)XtraSubscriber(&mIzatContext);

      result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

int IzatManager::deinit() {
    int result = -1;

    ENTRY_LOG();

    do {
        // XTRA 2.0
        LOC_LOGD("[XTRA2] deinit XTRA subscriber");
        delete mXtraSubscriber;
        mXtraSubscriber = NULL;

        IzatPassiveLocationListener::stopAllPassiveLocationListers(this);

        IPCMessagingProxy::destroyInstance();

        ComboNetworkProvider::destroyInstance();
        mComboNetworkProvider = NULL;

        delete mOSNetworkProvider;
        mOSNetworkProvider = NULL;

        OSObserver::destroyInstance();

        mCurrentNetworkProvider = NULL;
        mNLPProviderEnabled = false;

        Wiper::destroyInstance();
        mWiper = NULL;

        ((MsgTask*)mIzatContext.mMsgTask)->destroy();

        memset(&mIzatContext, 0, sizeof(s_IzatContext));
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR(%d, result);
    return result;
}

//IIzatManager overrides
int32 IzatManager::subscribeListener(IOSLocationListener* osLocationListener) {
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, osLocationListener);
        subscribeListenerMsg *msg =
            new (nothrow) subscribeListenerMsg(this, osLocationListener);
        BREAK_IF_ZERO(2, msg);
        mIzatContext.mMsgTask->sendMsg(msg);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR(%d, result);
    return result;
}

int32 IzatManager::unsubscribeListener(IOSLocationListener* osLocationListener) {
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, osLocationListener);
        unsubscribeListenerMsg *msg =
            new (nothrow) unsubscribeListenerMsg(this, osLocationListener);
        BREAK_IF_ZERO(2, msg);
        mIzatContext.mMsgTask->sendMsg(msg);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR(%d, result);
    return result;
}

int32 IzatManager::addRequest(const IzatRequest* request) {
    ENTRY_LOG();
    int result = updateCriteria(request, true);
    if (request) {
        LOC_LOGI ("LocTech-Label :: IZATMANAGER :: Add Request In");
        LOC_LOGI
        (
            "LocTech-Value :: Provider: %d, Num Updates: %d, TTFF: %lld, Interval: %lld, Displacement: %f, Horizontal Accuracy: %f, Altitude Accuracy: %d, Bearing Accuracy: %d",
             request->getProvider (),
             request->getNumUpdates (),
             request->getTimeForFirstFix (),
             request->getInterval (),
             request->getDistance (),
             request->getHorizontalAccuracy (),
             request->getAltitudeAccuracy (),
             request->getBearingAccuracy ()
        );
    }
    EXIT_LOG_WITH_ERROR(%d, result);
    return result;
}

int32 IzatManager::removeRequest(const IzatRequest* request) {
    ENTRY_LOG();
    int result = updateCriteria(request, false);
    if (request) {
        LOC_LOGI ("LocTech-Label :: IZATMANAGER :: Remove Request In");
        LOC_LOGI
        (
            "LocTech-Value :: Provider: %d, Num Updates: %d, TTFF: %lld, Interval: %lld, Displacement: %f, Horizontal Accuracy: %f, Altitude Accuracy: %d, Bearing Accuracy: %d",
             request->getProvider (),
             request->getNumUpdates (),
             request->getTimeForFirstFix (),
             request->getInterval (),
             request->getDistance (),
             request->getHorizontalAccuracy (),
             request->getAltitudeAccuracy (),
             request->getBearingAccuracy ()
        );
    }
    EXIT_LOG_WITH_ERROR(%d, result);
    return result;
}

int32 IzatManager::enableProvider(IzatRequest::IzatProviderType provider) {
    ENTRY_LOG();
    mIzatContext.mMsgTask->sendMsg(new (nothrow)enableProviderMsg(this, provider));
    EXIT_LOG_WITH_ERROR("%d", 0);

    return 0;
}

int32 IzatManager::disableProvider(IzatRequest::IzatProviderType provider) {
    ENTRY_LOG();
    mIzatContext.mMsgTask->sendMsg(new (nothrow)disableProviderMsg(this, provider));
    EXIT_LOG_WITH_ERROR("%d", 0);

    return 0;
}

// ILocationResponse overrides
void IzatManager::reportLocation(const LocationReport* report,
    const ILocationProvider* providerSrc)
{
    int result = -1;
    ENTRY_LOG();
    do {
        BREAK_IF_ZERO(1, report);
        BREAK_IF_ZERO(2, report->isValid());

        // Report location to Wiper
        mWiper->reportLocation(report, providerSrc);

        UlpNetworkPositionReport networkPos;
        networkPos.valid_flag = ULP_NETWORK_POSITION_REPORT_HAS_POSITION;
        networkPos.position.latitude = report->mLatitude;
        networkPos.position.longitude = report->mLongitude;
        networkPos.position.HEPE = report->mHorizontalAccuracy;

        if (report->mHasUtcTimestampInMsec) {
            networkPos.valid_flag |= ULP_NETWORK_POSITION_REPORT_HAS_FIX_TIME;
            networkPos.fix_time = report->mUtcTimestampInMsec;
        }

        networkPos.position.pos_source = ULP_NETWORK_POSITION_SRC_UNKNOWN;
        if (report->mHasPositionSource) {
            if (WIFI == report->mPositionSource) {
                networkPos.position.pos_source = ULP_NETWORK_POSITION_SRC_WIFI;
            } else if (CELLID == report->mPositionSource ||
                       ENH_CELLID == report->mPositionSource) {
                networkPos.position.pos_source = ULP_NETWORK_POSITION_SRC_CELL;
            }
        }

        BREAK_IF_NON_ZERO(2,
            mUlpNetworkInterface->ulp_send_network_position(&networkPos));
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::reportError(const LocationError* error,
    const ILocationProvider* providerSrc)
{
    UNUSED(error);
    UNUSED(providerSrc);

    // ULP does not support error reporting
}

// IIPCMessagingResponse overrides
void IzatManager::handleMsg(InPostcard * const in_card)
{
    int result = -1;
    ENTRY_LOG();
    do {
        BREAK_IF_ZERO(1, in_card);
        const char* from = NULL;
        const char* req = NULL;

        BREAK_IF_NON_ZERO(2, in_card->getString("FROM", &from));
        BREAK_IF_NON_ZERO(3, in_card->getString("REQ", &req));

        if (strcmp(from, "XT-CS") == 0) {
            if (strcmp(req, "START-GNSS-FIX") == 0) {
                mIzatContext.mMsgTask->sendMsg(
                    new (nothrow)startStopFixMsg(this, true));
            } else if (strcmp(req, "STOP-GNSS-FIX") == 0) {
                mIzatContext.mMsgTask->sendMsg(
                    new (nothrow)startStopFixMsg(this, false));
            }
        }
        if (strcmp(req, "PASSIVE-LOCATION") == 0) {
            IzatLocationListenerMask listensto = 0x0;
            bool intermediateFixAccepted = false;
            BREAK_IF_NON_ZERO(4, in_card->getUInt16("LISTENS-TO", listensto));
            BREAK_IF_NON_ZERO(5, in_card->getBoolDefault("INTERMEDIATE-FIX-ACCEPTED", intermediateFixAccepted));
            if (0x0 != listensto) {
                mIzatContext.mMsgTask->sendMsg(
                    new(nothrow) startPassiveLocationListenerMsg(
                        this, from, listensto, intermediateFixAccepted));
            }
        }
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::handleStartGnssFix() {
    if (!mHandlingGnssFix) {
        if (!mUlpStarted)
        {
            mUlpEngineInterface->start();
            mUlpStarted = true;
        }

        UlpLocationCriteria crit;
        memset(&crit, 0, sizeof(UlpLocationCriteria));

        crit.valid_mask = ULP_CRITERIA_HAS_ACTION |
            ULP_CRITERIA_HAS_PROVIDER_SOURCE |
            ULP_CRITERIA_HAS_RECURRENCE_TYPE;
        crit.action = ULP_ADD_CRITERIA;
        crit.provider_source = ULP_PROVIDER_SOURCE_GNSS;
        crit.recurrence_type = ULP_LOC_RECURRENCE_PERIODIC;
        mUlpEngineInterface->update_criteria(crit);

        mHandlingGnssFix = true;
    }
}

void IzatManager::handleStopGnssFix() {
    if (mHandlingGnssFix) {
        UlpLocationCriteria crit;
        memset(&crit, 0, sizeof(UlpLocationCriteria));

        crit.valid_mask = ULP_CRITERIA_HAS_ACTION |
            ULP_CRITERIA_HAS_PROVIDER_SOURCE |
            ULP_CRITERIA_HAS_RECURRENCE_TYPE;
        crit.action = ULP_REMOVE_CRITERIA;
        crit.provider_source = ULP_PROVIDER_SOURCE_GNSS;
        crit.recurrence_type = ULP_LOC_RECURRENCE_PERIODIC;
        mUlpEngineInterface->update_criteria(crit);

        mHandlingGnssFix = false;
    }
}

// IDataItemObserver overrides
void IzatManager::getName(std::string & name) {
    name = mSelfName;
}

void IzatManager::notify(const std::list<IDataItem *> & dlist) {
    ENTRY_LOG();
    mIzatContext.mMsgTask->sendMsg(
        new (nothrow)phoneContextResponseMsg(mIzatManager, dlist));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void IzatManager::ulp_network_location_request_cb (UlpNetworkRequestPos* req) {
    ENTRY_LOG();
    int result = -1;
    do {
        BREAK_IF_ZERO(1, req);
        mIzatManager->mIzatContext.mMsgTask->sendMsg(
            new (nothrow)networkLocationRequestMsg(mIzatManager, req));
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::ulp_request_phone_context_cb(UlpPhoneContextRequest* req) {
    UNUSED(req);
    mIzatManager->mIzatContext.mMsgTask->sendMsg(
        new (nothrow)phoneContextRequestMsg(mIzatManager));
}

void IzatManager::printUlpLocation (UlpLocation * ulpLocation, enum loc_sess_status status) {
    if (ulpLocation) {
        LOC_LOGD
        (
            "LocTech-Value :: Latitude: %f, Longitude: %f, Altitude: %f, Speed: %f, Bearing: %f,"
            "Accuracy: %f, UTC Time: %lld, Position Source: %x, Location Session Status %d",
            ulpLocation->gpsLocation.latitude,
            ulpLocation->gpsLocation.longitude,
            ulpLocation->gpsLocation.speed,
            ulpLocation->gpsLocation.bearing,
            ulpLocation->gpsLocation.accuracy,
            ulpLocation->gpsLocation.timestamp,
            ulpLocation->position_source,
            status
        );
    }
}
void IzatManager::location_callback (UlpLocation* location, enum loc_sess_status status) {
    ENTRY_LOG();
    int result = -1;
    ENTRY_LOG ();
    do {
        LOC_LOGD ("LocTech-Label :: IZATMANAGER :: Position Report In");

        BREAK_IF_ZERO(1, mIzatManager)
        BREAK_IF_ZERO(2, location);

        mIzatManager->printUlpLocation (location, status);
        mIzatManager->mIzatContext.mMsgTask->sendMsg(
            new (nothrow)locationCallbackMsg(mIzatManager, location, (IzatLocationStatus)status));
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::gnss_status_callback (const uint16 status) {
    ENTRY_LOG();
    int result = -1;
    ENTRY_LOG ();
    do {
        LOC_LOGD ("LocTech-Label :: IZATMANAGER :: GNSS status Report In");
        LOC_LOGD ("LocTech-Value :: GNSS Status (NONE-0, SESSION BEGIN-1, SESSION END-2, ENGINE ON-3, ENGINE OFF-4: %d", status);

        BREAK_IF_ZERO(1, mIzatManager)

        mIzatManager->mIzatContext.mMsgTask->sendMsg(
            new (nothrow)gnssStatusCallbackMsg(mIzatManager, status));
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
}

UlpLocationCriteria IzatManager::createUlpCritera(const IzatRequest *req, bool add) {
    UlpLocationCriteria res;
    memset(&res, 0, sizeof(UlpLocationCriteria));

    res.valid_mask = ULP_CRITERIA_HAS_ACTION |
        ULP_CRITERIA_HAS_PROVIDER_SOURCE |
        ULP_CRITERIA_HAS_RECURRENCE_TYPE |
        ULP_CRITERIA_HAS_PREFERRED_RESPONSE_TIME |
        ULP_CRITERIA_HAS_MIN_INTERVAL |
        ULP_CRITERIA_HAS_MIN_DISTANCE |
        ULP_CRITERIA_HAS_PREFERRED_HORIZONTAL_ACCURACY |
        ULP_CRITERIA_HAS_PREFERRED_ALTITUDE_ACCURACY |
        ULP_CRITERIA_HAS_PREFERRED_BEARING_ACCURACY;

    res.action = add ? ULP_ADD_CRITERIA : ULP_REMOVE_CRITERIA;
    switch(req->getProvider()) {
        case IZAT_STREAM_GNSS:
            res.provider_source = ULP_PROVIDER_SOURCE_GNSS;
            break;
        case IZAT_STREAM_FUSED:
            res.provider_source = ULP_PROVIDER_SOURCE_HYBRID;
            break;
        case IZAT_STREAM_NETWORK:
            res.provider_source = ULP_PROVIDER_SOURCE_NLP;
            break;
        default:
            res.valid_mask &= ~ULP_CRITERIA_HAS_PROVIDER_SOURCE;
    }

    if (req->getNumUpdates() == 1) {
        res.recurrence_type = ULP_LOC_RECURRENCE_SINGLE;
    } else {
        res.recurrence_type = ULP_LOC_RECURRENCE_PERIODIC;
    }

    res.preferred_response_time = req->getTimeForFirstFix();
    res.min_interval = capTouint32(req->getInterval());
    res.min_distance = req->getDistance();

    switch(req->getHorizontalAccuracy()) {
        case IZAT_HORIZONTAL_FINE:
            res.preferred_horizontal_accuracy = ULP_HORZ_ACCURACY_HIGH;
            break;
        case IZAT_HORIZONTAL_BLOCK:
            res.preferred_horizontal_accuracy = ULP_HORZ_ACCURACY_LOW;
            break;
        case IZAT_HORIZONTAL_CITY:
            res.preferred_horizontal_accuracy = ULP_HORZ_ACCURACY_LOW;
            break;
        case IZAT_HORIZONTAL_NONE:
            res.preferred_horizontal_accuracy = ULP_HORZ_ACCURACY_DONT_CARE;
            break;
        default:
            res.valid_mask &= ~ULP_CRITERIA_HAS_PREFERRED_HORIZONTAL_ACCURACY;
    }

    switch(req->getAltitudeAccuracy()) {
        case IZAT_OTHER_HIGH:
            res.preferred_altitude_accuracy = ULP_ACCURACY_HIGH;
            break;
        case IZAT_OTHER_LOW:
            res.preferred_altitude_accuracy = ULP_ACCURACY_LOW;
            break;
        case IZAT_OTHER_NONE:
            res.preferred_altitude_accuracy = ULP_ACCURACY_DONT_CARE;
            break;
        default:
            res.valid_mask &= ~ULP_CRITERIA_HAS_PREFERRED_ALTITUDE_ACCURACY;
    }

    switch(req->getBearingAccuracy()) {
        case IZAT_OTHER_HIGH:
            res.preferred_bearing_accuracy = ULP_ACCURACY_HIGH;
            break;
        case IZAT_OTHER_LOW:
            res.preferred_bearing_accuracy = ULP_ACCURACY_LOW;
            break;
        case IZAT_OTHER_NONE:
            res.preferred_bearing_accuracy = ULP_ACCURACY_DONT_CARE;
            break;
        default:
            res.valid_mask &= ~ULP_CRITERIA_HAS_PREFERRED_BEARING_ACCURACY;
    }

    return res;
}

IzatLocation IzatManager::izatLocationFromUlpLocation(const UlpLocation *ulpLoc) {
    IzatLocation res;
    memset(&res, 0, sizeof(res));

    if (NULL != ulpLoc) {
        res.mUtcTimestampInMsec = ulpLoc->gpsLocation.timestamp;
        res.mHasUtcTimestampInMsec = true;

        if (ulpLoc->gpsLocation.flags & GPS_LOCATION_HAS_LAT_LONG) {
            res.mHasLatitude = true;
            res.mLatitude = ulpLoc->gpsLocation.latitude;
            res.mHasLongitude = true;
            res.mLongitude = ulpLoc->gpsLocation.longitude;
        }

        if (ulpLoc->gpsLocation.flags & GPS_LOCATION_HAS_ALTITUDE) {
            res.mHasAltitudeWrtEllipsoid = true;
            res.mAltitudeWrtEllipsoid = ulpLoc->gpsLocation.altitude;
        }

        if (ulpLoc->gpsLocation.flags & GPS_LOCATION_HAS_SPEED) {
            res.mHasSpeed = true;
            res.mSpeed = ulpLoc->gpsLocation.speed;
        }

        if (ulpLoc->gpsLocation.flags & GPS_LOCATION_HAS_BEARING) {
            res.mHasBearing = true;
            res.mBearing = ulpLoc->gpsLocation.bearing;
        }

        if (ulpLoc->gpsLocation.flags & GPS_LOCATION_HAS_ACCURACY) {
            res.mHasHorizontalAccuracy = true;
            res.mHorizontalAccuracy = ulpLoc->gpsLocation.accuracy;
        }
    }

    return res;
}

int32 IzatManager::updateCriteria(const IzatRequest* request, bool add) {
    int result = -1;

    do {
        BREAK_IF_ZERO(1, request);

        UlpLocationCriteria ulpCriteria = createUlpCritera(request, add);
        updateCriteriaMsg *msg =
            new (std::nothrow) updateCriteriaMsg(this, ulpCriteria);
        BREAK_IF_ZERO(2, msg);
        mIzatContext.mMsgTask->sendMsg(msg);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR(%d, result);
    return result;
}

void IzatManager::networkLocationRequestMsg::proc() const {
    int result = -1;
    ENTRY_LOG();

    do {
        // Network provider not available. Most likely OSNP since IzatManager
        // won't init without Combo provider
        BREAK_IF_ZERO(1, mIzatManager->mCurrentNetworkProvider);

        ILocationProvider::LocationRequest locRequest;
        memset(&locRequest, 0, sizeof(ILocationProvider::LocationRequest));

        switch (mRequest.request_type)
        {
            case ULP_NETWORK_POS_START_PERIODIC_REQUEST:
                locRequest.action = ILocationProvider::LocationRequestAction_Start;
                locRequest.intervalInMsec = mRequest.interval_ms;
                if (0 == mRequest.interval_ms) {
                    locRequest.action = ILocationProvider::LocationRequestAction_SingleShot;
                }
                break;
            case ULP_NETWORK_POS_STOP_REQUEST:
                locRequest.action = ILocationProvider::LocationRequestAction_Stop;
                break;
            default:
                LOC_LOGW("Unexpected request type = %d", mRequest.request_type);
                result = 2;
                break;
        }

        BREAK_IF_ZERO(result, (result == -1));

        BREAK_IF_NON_ZERO(3,
            mIzatManager->mCurrentNetworkProvider->setRequest(&locRequest));

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::locationCallbackMsg::proc() const {
    int result = -1;
    ENTRY_LOG ();
    do {
        TListenerCol::const_iterator it = mIzatManager->mListeners.begin();
        while (it != mIzatManager->mListeners.end()) {
            IzatLocationListenerMask streamType = (*it)->listensTo();
            if ((mSource & ULP_LOCATION_IS_FROM_HYBRID &&
                 streamType & IZAT_STREAM_FUSED) ||
                (mSource & ULP_LOCATION_IS_FROM_NLP &&
                 streamType & IZAT_STREAM_NETWORK) ||
                (mSource & ULP_LOCATION_IS_FROM_GNSS &&
                 streamType & IZAT_STREAM_GNSS)) {
                (*it)->onLocationChanged(&mLocation, mStatus);
            }
            ++it;
        }

        result = 0;
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void IzatManager::gnssStatusCallbackMsg::proc() const {
    int result = -1;
    ENTRY_LOG ();
    do {
        TListenerCol::const_iterator it = mIzatManager->mListeners.begin();
        while (it != mIzatManager->mListeners.end()) {
            IzatLocationListenerMask streamType = (*it)->listensTo();
            if ( (streamType&IZAT_STREAM_FUSED) || (streamType&IZAT_STREAM_GNSS) ) {
                (*it)->onStatusChanged (mStatus);
            }
            ++it;
        }
        result = 0;
    } while (0);
    EXIT_LOG_WITH_ERROR ("%d", result);
}

void IzatManager::phoneContextRequestMsg::proc() const {
    int result = -1;

    ENTRY_LOG();

    do
    {
        BREAK_IF_ZERO(1, mIzatManager->mIzatContext.mOSObserverObj);

        std::list<DataItemId> dataItemIdList;
        DataItemId dataItemId[] = {
            ENH_DATA_ITEM_ID,
            WIFIHARDWARESTATE_DATA_ITEM_ID,
            GPSSTATE_DATA_ITEM_ID,
            NLPSTATUS_DATA_ITEM_ID,
            ASSISTED_GPS_DATA_ITEM_ID};
        dataItemIdList.assign(dataItemId, dataItemId + 5);

        mIzatManager->mIzatContext.mOSObserverObj->subscribe(dataItemIdList, mIzatManager);
        result = 0;
   } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

IzatManager::phoneContextResponseMsg::phoneContextResponseMsg(IzatManager *mgrObj,
        const std::list<IDataItem *> & dataItemList)
: mIzatManager(mgrObj) {
    int result = -1;
    do {
        std::list<IDataItem *>::const_iterator it = dataItemList.begin();
        for (; it != dataItemList.end(); it++) {
            IDataItem *updatedDataItem = *it;

            IDataItem * dataitem = DataItemsFactory::createNewDataItem(updatedDataItem->getId());
            BREAK_IF_ZERO(2, dataitem);
            // Copy the contents of the data item
            dataitem->getCopier()->copy(updatedDataItem);

            mDataItemList.push_back(dataitem);
            result = 0;
        }
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}


void IzatManager::phoneContextResponseMsg::proc() const {
    ENTRY_LOG();

    UlpPhoneContextSettings settings;
    memset(&settings, 0, sizeof(UlpPhoneContextSettings));

    // the assumption here is that the first response to phone context request message
    // will contain all the settings requested for.

    std::list<IDataItem *>::const_iterator it = mDataItemList.begin();
    for (; it != mDataItemList.end(); it++) {
        IDataItem* dataItem = *it;
        switch (dataItem->getId()) {
            case ENH_DATA_ITEM_ID: {
                ENHDataItem *enh = reinterpret_cast<ENHDataItem*>(dataItem);
                settings.context_type |= ULP_PHONE_CONTEXT_ENH_LOCATION_SERVICES_SETTING;
                settings.is_enh_location_services_enabled = enh->mEnabled;
                break;
            }
            case WIFIHARDWARESTATE_DATA_ITEM_ID: {
                WifiHardwareStateDataItem *wifiState =
                    reinterpret_cast<WifiHardwareStateDataItem*>(dataItem);
                settings.context_type |= ULP_PHONE_CONTEXT_WIFI_SETTING;
                settings.is_wifi_setting_enabled = wifiState->mEnabled;
                break;
            }
            case GPSSTATE_DATA_ITEM_ID: {
                GPSStateDataItem *gpsState = reinterpret_cast<GPSStateDataItem*>(dataItem);
                settings.context_type |= ULP_PHONE_CONTEXT_GPS_SETTING;
                settings.is_gps_enabled = gpsState->mEnabled;
                break;
            }
            case NLPSTATUS_DATA_ITEM_ID: {
                NLPStatusDataItem *nlpState = reinterpret_cast<NLPStatusDataItem*>(dataItem);
                settings.context_type |= ULP_PHONE_CONTEXT_NETWORK_POSITION_SETTING;
                settings.is_network_position_available = nlpState->mEnabled;
                break;
            }
            case ASSISTED_GPS_DATA_ITEM_ID: {
                AssistedGpsDataItem *agpsState =
                    reinterpret_cast<AssistedGpsDataItem*>(dataItem);
                settings.context_type |= ULP_PHONE_CONTEXT_AGPS_SETTING;
                settings.is_agps_enabled = agpsState->mEnabled;
                break;
            }
            case SCREEN_STATE_DATA_ITEM_ID: {
                ScreenStateDataItem *screenState =
                    reinterpret_cast<ScreenStateDataItem*>(dataItem);
                if (screenState->mState) {
                    mIzatManager->mUlpEngineInterface->system_update(ULP_LOC_SCREEN_ON);
                }
                break;
            }
            case POWER_CONNECTED_STATE_DATA_ITEM_ID: {
                PowerConnectStateDataItem *powerState =
                    reinterpret_cast<PowerConnectStateDataItem*> (dataItem);
                if (powerState->mState) {
                    mIzatManager->mUlpEngineInterface->system_update(ULP_LOC_POWER_CONNECTED);
                } else {
                    mIzatManager->mUlpEngineInterface->system_update(ULP_LOC_POWER_DISCONNECTED);
                }
                break;
            }
            case TIMEZONE_CHANGE_DATA_ITEM_ID: {
                mIzatManager->mUlpEngineInterface->system_update(ULP_LOC_TIMEZONE_CHANGE);
                break;
            }
            default: {
                break;
            }
        }
    }

    if (settings.context_type != 0) {
        mIzatManager->mUlpPhoneContextInterface->ulp_phone_context_settings_update(&settings);

        // Also update the ZPP trigger condition
        mIzatManager->mUlpEngineInterface->system_update(ULP_LOC_PHONE_CONTEXT_UPDATE);
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

IzatManager::phoneContextResponseMsg::~phoneContextResponseMsg() {
    std::list<IDataItem *>::const_iterator it = mDataItemList.begin();
    for (; it != mDataItemList.end(); it++) {
        delete *it;
    }

    mDataItemList.clear();
}

void IzatManager::deviceEventRequestMsg::proc() const {
    int result = -1;

    ENTRY_LOG();

    do
    {
        BREAK_IF_ZERO(1, mIzatManager->mIzatContext.mOSObserverObj);

        std::list<DataItemId> dataItemIdList;
        DataItemId dataItemId[] = {
            SCREEN_STATE_DATA_ITEM_ID,
            POWER_CONNECTED_STATE_DATA_ITEM_ID,
            TIMEZONE_CHANGE_DATA_ITEM_ID};
        dataItemIdList.assign(dataItemId, dataItemId + 3);

        mIzatManager->mIzatContext.mOSObserverObj->subscribe(dataItemIdList, mIzatManager);
        result = 0;
   } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::subscribeListenerMsg::proc() const {
    int result = -1;

    ENTRY_LOG();

    do {
        pair<TListenerCol::iterator, bool> res =
            mIzatManager->mListeners.insert(mListener);
        BREAK_IF_ZERO(1, res.second); // listener was already in the collection
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::unsubscribeListenerMsg::proc() const {
    int result = -1;

    ENTRY_LOG();

    do {
        int found = mIzatManager->mListeners.erase(mListener);
        BREAK_IF_ZERO(1, found); // listener was not present in the collection
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::updateCriteriaMsg::proc() const {
    int result = -1;

    ENTRY_LOG();

    do {
        if (!mIzatManager->mUlpStarted)
        {
            BREAK_IF_NON_ZERO(3, mIzatManager->mUlpEngineInterface->start());
            mIzatManager->mUlpStarted = true;
        }

        BREAK_IF_NON_ZERO(2,
                mIzatManager->mUlpEngineInterface->update_criteria(mCriteria));
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void IzatManager::enableProviderMsg::proc() const {
    ENTRY_LOG();
    if (IZAT_STREAM_NETWORK == mProvider && !mIzatManager->mNLPProviderEnabled) {
        mIzatManager->mNLPProviderEnabled = true;

        if (NULL != mIzatManager->mOSNetworkProvider) {
            mIzatManager->mOSNetworkProvider->disable();
        }
        if (NULL != mIzatManager->mComboNetworkProvider) {
            mIzatManager->mCurrentNetworkProvider = mIzatManager->mComboNetworkProvider;
            mIzatManager->mComboNetworkProvider->enable();
        }
    } else if (IZAT_STREAM_FUSED == mProvider) {
        mIzatManager->mFLPProviderEnabled = true;
    }

    LOC_LOGD("NetworkLocationProvider Enabled = %d, FusedLocationProvider Enabled = %d",
            mIzatManager->mNLPProviderEnabled, mIzatManager->mFLPProviderEnabled);

    EXIT_LOG_WITH_ERROR(%d, 0);
}

void IzatManager::disableProviderMsg::proc() const {
    ENTRY_LOG();

    if (IZAT_STREAM_NETWORK == mProvider && mIzatManager->mNLPProviderEnabled) {
        mIzatManager->mNLPProviderEnabled = false;

        if (NULL != mIzatManager->mComboNetworkProvider) {
            mIzatManager->mComboNetworkProvider->disable();
        }
        if (NULL != mIzatManager->mOSNetworkProvider) {
            mIzatManager->mCurrentNetworkProvider = mIzatManager->mOSNetworkProvider;
            mIzatManager->mOSNetworkProvider->enable();
        }
    } else if (IZAT_STREAM_FUSED == mProvider) {
        mIzatManager->mFLPProviderEnabled = false;
    }

    LOC_LOGD("NetworkLocationProvider Enabled = %d, FusedLocationProvider Enabled = %d",
            mIzatManager->mNLPProviderEnabled, mIzatManager->mFLPProviderEnabled);

    EXIT_LOG_WITH_ERROR(%d, 0);
}

void IzatManager::startStopFixMsg::proc() const {
    ENTRY_LOG();

    if (mStart)
        mIzatManager->handleStartGnssFix();
    else
        mIzatManager->handleStopGnssFix();

    EXIT_LOG_WITH_ERROR(%d, 0);
}

void IzatManager::startPassiveLocationListenerMsg::proc() const {
    ENTRY_LOG();

    IzatPassiveLocationListener::startPassiveLocationListener(
      mClientName, mListensTo,mIntermediateFixAccepted, mIzatManager, mIzatManager->mIzatContext.mIPCMessagingProxyObj);

    EXIT_LOG_WITH_ERROR(%d, 0);
}

} // namespace izat_manager
