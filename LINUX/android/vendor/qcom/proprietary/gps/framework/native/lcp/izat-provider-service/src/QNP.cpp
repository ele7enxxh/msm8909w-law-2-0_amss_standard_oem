/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Qualcomm Network Provider module

  Copyright (c) 2015 -2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

The MIT License (MIT)

Copyright (c) 2015 Chris Veness

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
=============================================================================*/


#define LOG_NDDEBUG 0
#define LOG_TAG "IzatSvc_QNP"

#include "QNP.h"
#include "loc_cfg.h"
#include "Utils.h"

#include <math.h>

namespace izat_manager
{

ILocationProvider* QNP::mQNP = NULL;

void QNP::enableQNPMsg::proc() const
{
    int result = -1;

    ENTRY_LOG();

    do {
        // If the current state is TRACKING_STATE_IDLE || TRACKING_STATE_TBF ||
        // TRACKING_STATE_WAIT_FOR_WIFI_FIX || TRACKING_STATE_WAIT_FOR_WWAN_FIX ||
        // TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX
        // then do not do anything. Could be just a repeated enable call.
        if (TRACKING_STATE_DISABLED == mQNP->mTrackingState) {

            mQNP->mTrackingState = TRACKING_STATE_IDLE;

            if (mQNP->mGtpWifiProvider != NULL) {
                mQNP->mGtpWifiProvider->enable();
            }

            if (mQNP->mGtpWwanProvider != NULL) {
                mQNP->mGtpWwanProvider->enable();
            }

            if (mQNP->mZppProvider != NULL) {
                mQNP->mZppProvider->enable();
            }
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void QNP::disableQNPMsg::proc() const
{
    ENTRY_LOG();
    int result = -1;

    do {
        // On-going request / timer must be stopped
        mQNP->stopTrackingSession();

        // Just change the state to disabled and exit.
        // If location comes after changing the state to disabled, location will be dropped.
        mQNP->mTrackingState = TRACKING_STATE_DISABLED;

        if (mQNP->mGtpWifiProvider != NULL) {
            mQNP->mGtpWifiProvider->disable();
        }

        if (mQNP->mGtpWwanProvider != NULL) {
            mQNP->mGtpWwanProvider->disable();
        }

        if (mQNP->mZppProvider != NULL) {
            mQNP->mZppProvider->disable();
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}


void QNP::requestLocationMsg::proc() const
{
    ENTRY_LOG();

    LOC_LOGD("Tracking State = %d", mQNP->mTrackingState);

    if (TRACKING_STATE_DISABLED == mQNP->mTrackingState) {
        LOC_LOGD("QNP is disabled, ignore");
    } else {

        // save single-shot fix information
        mQNP->mSingleShotFixRequested =
            (LocationRequestAction_SingleShot == mLocRequest.action);

        // start tracking request
        if ((LocationRequestAction_Start == mLocRequest.action) ||
            (LocationRequestAction_SingleShot == mLocRequest.action)) {
            if ((TRACKING_STATE_IDLE == mQNP->mTrackingState) ||
                (TRACKING_STATE_TBF == mQNP->mTrackingState)) {
                LOC_LOGD("In IDLE/TBF state, start tracking");
                mQNP->startTrackingSession();
            } else {
                LOC_LOGD("In the middle of position request, continue to track.");
            }
        } else if (LocationRequestAction_Stop == mLocRequest.action) {
            mQNP->stopTrackingSession();
        } else {
            LOC_LOGE("Unhandled LocationRequest type");
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::setIntervalMsg::proc() const
{
    ENTRY_LOG();

    // only if current TBF changes

    // Note: a setIntervalMsg is always accompanied with a setRequest msg
    // If state TRACKING_STATE_IDLE or TRACKING_STATE_TBF, this TBF will take affect
    // immediately.
    if (mQNP->mCurrentTimeBetweenFixInMsec != mIntervalInMsec) {
        mQNP->mCurrentTimeBetweenFixInMsec = mIntervalInMsec;
        if (mQNP->mCurrentTimeBetweenFixInMsec < mQNP->mMinTimeBeweenTrackingFixInMsec) {
            mQNP->mCurrentTimeBetweenFixInMsec = mQNP->mMinTimeBeweenTrackingFixInMsec;
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::tbfTimerMsg::proc() const
{
    ENTRY_LOG();

    int result = -1;
    do {
        LOC_LOGD("QNP Tracking state = %d", mQNP->mTrackingState);
        BREAK_IF_ZERO(1, TRACKING_STATE_TBF == mQNP->mTrackingState);

        // cache the time this request is being made
         GetTimeSinceBoot(mQNP->mTimestampLastTrackingRequestInNanoSec);

        // send out the fix requests
        mQNP->requestFixes();

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

QNP::QNP(const struct s_IzatContext *izatContext,
           ILocationProvider* pGTPWifiProvider, ILocationProvider* pGTPWwanProvider) :
    LocationProvider(izatContext),
#ifdef __ANDROID__
    mNLPConfFile("/etc/xtwifi.conf"),
    mIzatConfFile("/etc/izat.conf"),
#else
    mNLPConfFile("xtwifi.conf"),
    mIzatConfFile("izat.conf"),
#endif
    mGtpWifiProvider(pGTPWifiProvider), mGtpWwanProvider(pGTPWwanProvider),
    mZppProvider(NULL),
    mTrackingState(TRACKING_STATE_DISABLED),
    mCurrentTimeBetweenFixInMsec(0), mTimestampLastTrackingRequestInNanoSec(0),
    mTBFTimer(this), mFixReportedInCurrentSession(false), mSingleShotFixRequested(false),
    mApWwanEnabled(false), mWaitForWifiFinalFix(false), mRequestId(0),
    mMinTimeBeweenTrackingFixInMsec(2000), mMaxAcuracyThresholdToFilterNlpPosition(25000),
    mMaxZppReportableHepe(10), mMaxZppWifiConsistencyCheckInMeters(200),
    mCountOfWifiFixInconsistentWithZPP(0), mCountOfDroppedFixesDueToLargeRadius(0),
    mCountOfDroppedSingleApWifiFix(90),
    mMinTimeToReportStatsCollectedInSec(3600), mTimeForSavedPositionToBeValidInMinutes(10)
{
    readConfig();

    // initialize the statistic collection start time to current time
    GetTimeSinceBoot(mCollectionStartTimestampInNanoSec);

    if (mGtpWifiProvider != NULL) {
        mGtpWifiProvider->subscribe(this);
    }

    if (mGtpWwanProvider != NULL) {
        mGtpWwanProvider->subscribe(this);
    }

    // If GTP AP WWAN is enabled then we do not want to trigger ZPP fix
    if (false == mApWwanEnabled) {
        mZppProvider = ZppProxy::getInstance(izatContext);
        if (mZppProvider == NULL) {
            LOC_LOGE("Failed to create Zpp provider");
        } else {
            mZppProvider->subscribe(this);
        }
    }
}


QNP::~QNP()
{
    GtpWifiProxy::destroyInstance();
    mGtpWifiProvider = NULL;

    GtpWWanProxy::destroyInstance();
    mGtpWwanProvider = NULL;

    ZppProxy::destroyInstance();
    mZppProvider = NULL;
}

ILocationProvider* QNP::getInstance(const struct s_IzatContext* izatContext)
{
    int result = -1;
    ILocationProvider *pGtpWiFiProvider = NULL;
    ILocationProvider *pGtpWwanProvider = NULL;

    do {
        // QNP object is already created
        BREAK_IF_NON_ZERO(0, mQNP);
        BREAK_IF_ZERO(1, izatContext);

        pGtpWiFiProvider = GtpWifiProxy::getInstance(izatContext);
        pGtpWwanProvider = GtpWWanProxy::getInstance(izatContext);

        // QNP exists only if atleast one of the izat features is enabled
        BREAK_IF_ZERO(2,
            ((pGtpWiFiProvider != NULL) || (pGtpWwanProvider != NULL)));

        mQNP = new (nothrow) QNP(izatContext,
            pGtpWiFiProvider, pGtpWwanProvider);
        BREAK_IF_ZERO(4, mQNP);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR(%d, result);
    return mQNP;
}

int QNP::destroyInstance()
{
    ENTRY_LOG();

    delete mQNP;
    mQNP = NULL;

    EXIT_LOG_WITH_ERROR("%d", 0);
    return 0;
}

  // ILocationProvider overrides
int QNP::setRequest(const LocationRequest *request)
{
    ENTRY_LOG();
    int result = -1;

    do {
        BREAK_IF_ZERO(1, request);

        LOC_LOGD ("LocTech-Label :: QNP :: Position Request In");
        LOC_LOGD ("LocTech-Value :: Action (Single Shot = 0, Start = 1, Stop = 2) : %d", request->action);
        LOC_LOGD ("LocTech-Value :: Interval In milliseconds %u", request->intervalInMsec);
        mIzatContext->mMsgTask->sendMsg(new (nothrow) setIntervalMsg(this, request->intervalInMsec));
        mIzatContext->mMsgTask->sendMsg(new (nothrow) requestLocationMsg(this, request));

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

void QNP::enable()
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow) enableQNPMsg(this));
}

void QNP::disable()
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow) disableQNPMsg(this));
}

// ILocationResponse overrides

// reportLocation gets called in the context of the MsgTask thread
void QNP::reportLocation(const LocationReport *locReport,
    const ILocationProvider *providerSrc )
{
    ENTRY_LOG();

    int result = -1;
    do {
      LOC_LOGD("QNP Tracking State = %d", mTrackingState);

      if ((TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX != mTrackingState) &&
          (TRACKING_STATE_WAIT_FOR_WIFI_FIX != mTrackingState) &&
          (TRACKING_STATE_WAIT_FOR_WWAN_FIX != mTrackingState)) {
        result = 1;
        break;
      }
      LOC_LOGD ("LocTech-Label :: QNP :: Position Report In");

      string locationReport;
      locReport->stringify (locationReport);
      LOC_LOGD ("LocTech-Value :: Position Report: %s", locationReport.c_str());

      if (providerSrc == mZppProvider) {
          LOC_LOGD ("LocTech-Value :: Provider Source: ZPP");
          const ZppProxy::ZppMeasurementReport *zppReport =
                      static_cast<const ZppProxy::ZppMeasurementReport*> (locReport);
          mZppLocReport = *zppReport;
          handleReportFromZpp();
      } else if (providerSrc == mGtpWwanProvider) {
          LOC_LOGD ("LocTech-Value :: Provider Source: GTP WWAN");
          mWwanLocReport = *locReport;
          handleReportFromWwan();
      } else if (providerSrc == mGtpWifiProvider) {
          LOC_LOGD ("LocTech-Value :: Provider Source: GTP WIFI");
        const GtpWifiProxy::WifiMeasurementReport *wifiReport =
            static_cast<const GtpWifiProxy::WifiMeasurementReport*> (locReport);
          mWifiLocReport = *wifiReport;
          handleReportFromWifi();
      }

      result = 0;
    } while(0);

   EXIT_LOG_WITH_ERROR("%d", result);
}

// reportError gets called in the context of the MsgTask thread
void QNP::reportError(const LocationError *locError,
    const ILocationProvider *providerSrc)
{
    ENTRY_LOG();

    int result = -1;
    do {
      LOC_LOGD("QNP Tracking State = %d", mTrackingState);

      if ((TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX != mTrackingState) &&
          (TRACKING_STATE_WAIT_FOR_WIFI_FIX != mTrackingState) &&
          (TRACKING_STATE_WAIT_FOR_WWAN_FIX != mTrackingState)) {
          result = 1;
          break;
      }

      LOC_LOGD ("LocTech-Label :: QNP :: Error Report In");

      if (providerSrc == mGtpWwanProvider) {
          LOC_LOGD ("LocTech-Value :: Provider Source: GTPWWAN");
          mWwanLocError = *locError;
          handleReportFromWwan();
      } else if (providerSrc == mGtpWifiProvider) {
          LOC_LOGD ("LocTech-Value :: Provider Source: GTPWIFI");
          mWifiLocError = *locError;
          handleReportFromWifi();
      } else if (providerSrc == mZppProvider) {
          LOC_LOGD ("LocTech-Value :: Provider Source: ZPP");
          mZppLocError = *locError;
          handleReportFromZpp();
      }
      string errorReport;
      locError->stringify (errorReport);
      LOC_LOGD ("LocTech-Value :: Error Report: %s", errorReport.c_str ());

      result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void QNP::readConfig()
{
    static char conf_gtp_cell_mode[LOC_MAX_PARAM_STRING];
    static char conf_gtp_cell_proc[LOC_MAX_PARAM_STRING];

    static loc_param_s_type xtwifi_conf_param_table[] = {
        {
           "TIME_MSEC_NLP_MIN_TIME_BETWEEN_FIX", &mMinTimeBeweenTrackingFixInMsec,
           NULL, 'n'
        },
        {  "LARGE_ACCURACY_THRESHOLD_TO_FILTER_NLP_POSITION", &mMaxAcuracyThresholdToFilterNlpPosition,
            NULL, 'n'
        },
        {  "HEPE_MARGIN_METER_REPORT_ZPP_LOCATION", &mMaxZppReportableHepe,
            NULL, 'n'
        },
        {  "TIME_SEC_MIN_AGE_REPORTING_STATS_COLLECTED", &mMinTimeToReportStatsCollectedInSec,
            NULL, 'n'
        },
        {  "CONSISTENCY_CHECK_ZPP_WIFI_DISTANCE_MAX_THRESHOLD_METERS", &mMaxZppWifiConsistencyCheckInMeters,
            NULL , 'n'
        },
        {  "TIME_MINUTE_INVALIDATE_LAST_KNOWN_POS", &mTimeForSavedPositionToBeValidInMinutes,
            NULL , 'n'
        }
    };

    ENTRY_LOG();

    UTIL_READ_CONF(mNLPConfFile.c_str(), xtwifi_conf_param_table);

    LOC_LOGD("TIME_MSEC_NLP_MIN_TIME_BETWEEN_FIX = %d \
        LARGE_ACCURACY_THRESHOLD_TO_FILTER_NLP_POSITION = %d \
        HEPE_MARGIN_METER_REPORT_ZPP_LOCATION = %d \
        TIME_SEC_MIN_AGE_REPORTING_STATS_COLLECTED = %d \
        CONSISTENCY_CHECK_ZPP_WIFI_DISTANCE_MAX_THRESHOLD_METERS = %d \
        TIME_MINUTE_INVALIDATE_LAST_KNOWN_POS = %d",
        mMinTimeBeweenTrackingFixInMsec, mMaxAcuracyThresholdToFilterNlpPosition,
        mMaxZppReportableHepe, mMinTimeToReportStatsCollectedInSec,
        mMaxZppWifiConsistencyCheckInMeters,
        mTimeForSavedPositionToBeValidInMinutes);

    static loc_param_s_type izat_conf_param_table[] = {
        {"GTP_CELL",      &conf_gtp_cell_mode, NULL, 's'},
        {"GTP_CELL_PROC", &conf_gtp_cell_proc, NULL, 's'}
    };

    UTIL_READ_CONF(mIzatConfFile.c_str(), izat_conf_param_table);

    LOC_LOGD("GTP cell set in izat.conf, proc: %s, mode: %s",
     conf_gtp_cell_proc, conf_gtp_cell_mode);

    if (strncmp(conf_gtp_cell_mode, "BASIC", sizeof (conf_gtp_cell_mode)) == 0) {
        if (strncmp(conf_gtp_cell_proc, "AP", sizeof (conf_gtp_cell_proc)) == 0) {
            mApWwanEnabled = true;
            LOC_LOGE("GTP Cell AP enabled");
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::startTrackingSession()
{
    int result = -1;
    ENTRY_LOG();

    do {
        if ((TRACKING_STATE_IDLE == mTrackingState) ||
            (TRACKING_STATE_TBF == mTrackingState)) {
            // stop tbf timer and start session immediately

            mTBFTimer.stop();

            BREAK_IF_NON_ZERO(1, GetTimeSinceBoot(mTimestampLastTrackingRequestInNanoSec));

            requestFixes();
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void QNP::stopTrackingSession()
{
    int result = -1;
    ENTRY_LOG();

    do {
        // Nothing to do if the provider state is IDLE anyways
        BREAK_IF_NON_ZERO(1, TRACKING_STATE_IDLE == mTrackingState);

        if (TRACKING_STATE_TBF == mTrackingState) {
            mTBFTimer.stop();
        }

        stopFixes();

        // change state to IDLE
        mTrackingState = TRACKING_STATE_IDLE;

        // reset single-shot fix session status.
        mSingleShotFixRequested = false;

        // reset request count
        mRequestId = 0;
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void QNP::resetSession()
{
    ENTRY_LOG();

    if (mSingleShotFixRequested) {
        stopTrackingSession();
    } else {
        resetTBFTimer();
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::resetTBFTimer()
{
    ENTRY_LOG();

    int result = -1;

    do {
        mTBFTimer.stop();

        // send stop request to other providers in its list
        stopFixes();

        // Calculate the wait time before triggering next session
        uint32_t timeToNextFixInMsec = 0;
        int64 nowInNanoSec = 0;
        BREAK_IF_NON_ZERO(1, GetTimeSinceBoot(nowInNanoSec));

        if ((mTimestampLastTrackingRequestInNanoSec <= 0) || (mTimestampLastTrackingRequestInNanoSec > nowInNanoSec)) {
            // <= 0: we have not started any fix in this tracking session
            // > now: system time adjustment?
            // retry immediately
            timeToNextFixInMsec = 0;
        } else if (mFixReportedInCurrentSession) {
            timeToNextFixInMsec = mCurrentTimeBetweenFixInMsec;
        } else {
            // we haven't reported anything in the last fix session, delay for awhile and restart
            // we do not want to squeeze the system too much by setting shorter TBF here
            // as it contributes to harmless race condition with coarse position injection
            // and potentially too fast wifi scan rate
            timeToNextFixInMsec = mCurrentTimeBetweenFixInMsec;
        }

        if (0 == timeToNextFixInMsec) {
            mIzatContext->mMsgTask->sendMsg(new (nothrow)tbfTimerMsg(this));
        } else {
            mTBFTimer.start(timeToNextFixInMsec, false);
        }

        mTrackingState = TRACKING_STATE_TBF;

        result = 0;
    }while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void QNP::Timer::timeOutCallback()
{
    ENTRY_LOG();
    mClient->mIzatContext->mMsgTask->sendMsg(new (nothrow)tbfTimerMsg(mClient));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::requestFixes()
{
    ENTRY_LOG();

    // reset any previous position /error reports
    mLocationReport.reset();
    mWifiLocReport.reset();
    mWwanLocReport.reset();
    mZppLocReport.reset();
    mWifiLocError.reset();
    mWwanLocError.reset();
    mZppLocError.reset();
    mFixReportedInCurrentSession = false;
    mWaitForWifiFinalFix = false;

    LocationRequest locRequest;
    memset(&locRequest, 0, sizeof(LocationRequest));
    locRequest.action = LocationRequestAction_Start;
    locRequest.intervalInMsec = 0;

    int64 nowInNanoSec = 0;
    int result = GetTimeSinceBoot(nowInNanoSec);
    if (result == 0) {
        int64 ageOfCollectedDataInNanoSec = nowInNanoSec - mCollectionStartTimestampInNanoSec;
        if (mMinTimeToReportStatsCollectedInSec <= (convertNanoSecsToSecs(ageOfCollectedDataInNanoSec))) {
             LOC_LOGI("metrics.lbs", "qclbs:data:wifi_fix_inconsistent_with_zpp=%d ;CT;1," \
                      "fix_dropped_too_large_radius=%d ;CT;1," \
                      "dropped_wifi_fix_due_to_single_AP=%d ;CT;1:NR",
                      mCountOfWifiFixInconsistentWithZPP, mCountOfDroppedFixesDueToLargeRadius,
                      mCountOfDroppedSingleApWifiFix);
             mCollectionStartTimestampInNanoSec = nowInNanoSec;
             mCountOfDroppedFixesDueToLargeRadius = 0;
             mCountOfDroppedFixesDueToLargeRadius = 0;
             mCountOfDroppedSingleApWifiFix = 0;
        }
    }

    mRequestId++;
    if (mRequestId > mMaxRequestId) {
        mRequestId = mMinRequestId;
    }

    // Automation -log - Please do not touch
    LOC_LOGD("fireGTPFixSession: Fire a position request to GTP-AP %d", mRequestId);


    if (mGtpWifiProvider != NULL) {
        mGtpWifiProvider->setRequest(&locRequest);
        mTrackingState = TRACKING_STATE_WAIT_FOR_WIFI_FIX;
    } else {
        if (mZppProvider != NULL) {
            mTrackingState = TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX;
            mZppProvider->setRequest(&locRequest);
        }

        if (mGtpWwanProvider != NULL) {
            if (mTrackingState < TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX) {
                mTrackingState = TRACKING_STATE_WAIT_FOR_WWAN_FIX;
            }
            mGtpWwanProvider->setRequest(&locRequest);
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::stopFixes()
{
    ENTRY_LOG();

    LocationRequest locRequest;
    memset(&locRequest, 0, sizeof(LocationRequest));
    locRequest.action = LocationRequestAction_Stop;
    locRequest.intervalInMsec = -1;

    if (mGtpWwanProvider != NULL) {
        mGtpWwanProvider->setRequest(&locRequest);
    }

    if (mGtpWifiProvider != NULL) {
        mGtpWifiProvider->setRequest(&locRequest);
    }

    if (mZppProvider != NULL) {
        mZppProvider->setRequest(&locRequest);
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}


bool QNP:: zppHasGpsContent()
{
    LOC_LOGD("Source technology mask = %d %d",
        mZppLocReport.mHasSourceTechMask, mZppLocReport.mSourceTechMask);

      if (mZppLocReport.mHasSourceTechMask &&
          (mZppLocReport.mSourceTechMask & LOC_POS_TECH_MASK_SATELLITE) == true) {
          return true;
      }

      return false;
}

void QNP::handleReportFromZpp()
{
    ENTRY_LOG();
    LOC_LOGD("QNP Tracking State = %d", mTrackingState);
    compareAndReportWifiFix();
    if (true == mFixReportedInCurrentSession) {
        resetSession();
    } else {
        consolidateAllAndWaitForNextState();
    }
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::handleReportFromWwan()
{
    ENTRY_LOG();
    int result = -1;
    LOC_LOGD("QNP Tracking State = %d", mTrackingState);

    if (mWwanLocReport.isValid()) {
        mLastSavedWwanLocReport = mWwanLocReport;

        mLastSavedWwanLocReport.mHasPropagatedFix = true;
        mLastSavedWwanLocReport.mIsPropagatedFix = true;
    } else if (mLastSavedWwanLocReport.isValid()) {
        if (shouldApplyPuncGrowthModel(mLastSavedWwanLocReport)) {
            LOC_LOGD("Apply PUNC growth for WWAN fix");
            mWwanLocReport = mLastSavedWwanLocReport;
            increasePUNC(mWwanLocReport);
        }
    }

    // if trackingstate is TRACKING_STATE_WAIT_FOR_WWAN_FIX, this means
    // 1. Either we do not have wait for WIFI / ZPP fix based on configuration or
    // 2. Have recieved Zpp and Wifi fix and have been unable to report either fixes
    // hence now we have been waiting for wwan fix.
    if (mTrackingState == TRACKING_STATE_WAIT_FOR_WWAN_FIX) {
        compareAndReportWifiFix();
        if (true == mFixReportedInCurrentSession) {
            resetSession();
        } else {
            consolidateAllAndWaitForNextState();
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::handleReportFromWifi()
{
    ENTRY_LOG();
    LOC_LOGD("QNP Tracking State = %d", mTrackingState);

    LocationRequest locRequest;
    memset(&locRequest, 0, sizeof(LocationRequest));
    locRequest.action = LocationRequestAction_Start;
    locRequest.intervalInMsec = 0;

    if (mWifiLocReport.isValid()) {
        mLastSavedWifiLocReport = mWifiLocReport;

        mLastSavedWifiLocReport.mHasPropagatedFix = true;
        mLastSavedWifiLocReport.mIsPropagatedFix = true;
    } else if (mLastSavedWifiLocReport.isValid()) {
        if (shouldApplyPuncGrowthModel(mLastSavedWifiLocReport)) {
            LOC_LOGD("Apply PUNC growth for WIFI fix");
            mWifiLocReport = mLastSavedWifiLocReport;
            increasePUNC(mWifiLocReport);
        }
    }

    // Request for Zpp and WWAN fix only after GTP-WIFI engine returns either with an error or a fix.
    if (TRACKING_STATE_WAIT_FOR_WIFI_FIX == mTrackingState) {
        if (mZppProvider != NULL) {
            mTrackingState = TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX;
            mZppProvider->setRequest(&locRequest);
        }

        if (mGtpWwanProvider != NULL) {
            if (mTrackingState < TRACKING_STATE_WAIT_FOR_BEST_AVAILABLE_FIX) {
                mTrackingState = TRACKING_STATE_WAIT_FOR_WWAN_FIX;
            }
            mGtpWwanProvider->setRequest(&locRequest);
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void QNP::consolidateAllAndWaitForNextState()
{
    ENTRY_LOG();

    int result = -1;
    bool reportLocation = false;

    QNPTrackingState previousState = mTrackingState;

    do {

        if (mGtpWifiProvider != NULL) {
            if (((mWifiLocReport.isValid() == false ) && (mWifiLocError.isValid() == false)) ||
                ((mWifiLocReport.isValid()) && (mWifiLocReport.mIsIntermediateFix))) {
               // wifi has not reported a fix or error yet, wait for it. OR
               // wifi has only reported a intermediate fix, hence wait for final fix
               mTrackingState = TRACKING_STATE_WAIT_FOR_WIFI_FIX;

               // this could be a preliminary fix that has failed consistency check, reset it anyways.
               if ((mWifiLocReport.isValid()) && (mWifiLocReport.mIsIntermediateFix)) {
                   LOC_LOGD("Continue to wait for WIFI final fix");
                   mWifiLocReport.reset();
                   mLastSavedWifiLocReport.reset();
                   mWaitForWifiFinalFix = true;
               } else {
                     LOC_LOGD("Continue to wait for WIFI fix");
               }
               break;
            }
        }

        // Wifi has already reported a final fix and still we are unable to report the wifi fix.
        // check if Zpp fix can be reported anyways.
        if ((mZppLocReport.isValid())&&
            (mZppLocReport.mHorizontalAccuracy < mMaxAcuracyThresholdToFilterNlpPosition) &&
            (!zppHasGpsContent())) {
            mLocationReport = mZppLocReport;
            reportLocation = true;
            break;
        }

        if (mGtpWwanProvider != NULL) {
            if ((mWwanLocReport.isValid() == false) && (mWwanLocError.isValid() == false)) {
                // wwan has not reported still, wait for it.
                LOC_LOGD("Continue to wait for WWAN fix");
                mTrackingState = TRACKING_STATE_WAIT_FOR_WWAN_FIX;
                break;
            } else if (mWwanLocReport.isValid()) {
                mLocationReport = mWwanLocReport;
                reportLocation = true;
                break;
            }
        }
    } while(0);

    if (reportLocation) {
        if (mLocationReport.mHorizontalAccuracy > mMaxAcuracyThresholdToFilterNlpPosition) {
            LOC_LOGD("Dropping final fix from src = %d Accuracy = %f AccuracyThreshold = %d",
                mLocationReport.mPositionSource, mLocationReport.mHorizontalAccuracy,
                mMaxAcuracyThresholdToFilterNlpPosition);
            mFixReportedInCurrentSession = false;
            mCountOfDroppedFixesDueToLargeRadius += 1;
        } else {
            if (mLocationReport.mHasPositionSource) {
                LOC_LOGD ("LocTech-Label :: QNP :: Position Report Out");
                if (CELLID == mLocationReport.mPositionSource) {
                    increasePUNC(mLocationReport);
                    LOC_LOGD ("LocTech-Value :: Position source: WWAN");

                    // Automation -log - Please do not touch
                    LOC_LOGD ("Reporting WWAN fix (%f, %f), unc %f time (ms) %lld",
                        mLocationReport.mLatitude, mLocationReport.mLongitude,
                        mLocationReport.mHorizontalAccuracy, mLocationReport.mUtcTimestampInMsec);
                } else if (OTHER == mLocationReport.mPositionSource) {
                    increasePUNC(mLocationReport);
                    LOC_LOGD ("LocTech-Value :: Position source: ZPP");

                    // Automation -log - Please do not touch
                    LOC_LOGD ("Reporting ZPP fix (%f, %f), unc %f time (ms) %lld",
                        mLocationReport.mLatitude, mLocationReport.mLongitude,
                        mLocationReport.mHorizontalAccuracy, mLocationReport.mUtcTimestampInMsec);

                }
            }
            broadcastLocation(&mLocationReport);
            mFixReportedInCurrentSession = true;
        }
    }

    if ((mFixReportedInCurrentSession) ||
        ((previousState == mTrackingState) && (!mWaitForWifiFinalFix))) {
        // resetSesssion if
        // 1. fix is reported or
        // 2. there is no tracking state change because there are no more provider
        //     reports to wait for.
        // 3. there is no tracking state change because the preliminary wifi fix might have failed
        //     consistency check and we remain in TRACKING_STATE_WAIT_FOR_WIFI_FIX state
        resetSession();
    }
    result = 0;

    EXIT_LOG_WITH_ERROR("%d", 0);
}

// this function only checks if wifi fix can be reported or not based on its comparison with Zpp /Wwan fix
void QNP::compareAndReportWifiFix()
{
    ENTRY_LOG();
    int result = -1;
    bool reportWifi = false;
    LocationReport locReportToCompareWith;

    // PUNC scaling from from 99% confidence to 68% confidence is (1.07/2.14) = 0.5
    const double ScaleFactorPuncConf = 0.5;

    do {
        // if APWWAN is enabled, then ZPP is not requested for, hence cannot be used for comparison
        if (mApWwanEnabled) {
            locReportToCompareWith = mWwanLocReport;
        } else {
            locReportToCompareWith = mZppLocReport;
        }

        if ((mWifiLocReport.isValid()) && (locReportToCompareWith.isValid())) {

            // grow uncertainity circle of Zpp fix based on its accuracy and timestamp first
            increasePUNC(locReportToCompareWith);
            // grow the Zpp circle to accomodate the wifi fix
            float increasedPUNC = getPUNCIncrease(mWifiLocReport);
            float accuracy_99 = increasedPUNC +
                (locReportToCompareWith.mHorizontalAccuracy / ScaleFactorPuncConf);
            float accuracy_68 = accuracy_99 * ScaleFactorPuncConf;

            double distance = getGreatCircleDistanceMeter(&mWifiLocReport, &locReportToCompareWith);

            // Automation -log - Please do not touch
            LOC_LOGD("Distance between ZPP and XTWiFi fixes: %f m, "
                       "while accuracy of ZPP fix is %f m (99%%), "
                       "accuracy of ZPP fix is %f m (68%%)", distance,
                       accuracy_99, accuracy_68);

            // 1. Wifi fix falls into the uncertainity circle of wwan/zpp fix
            // 2. Distance of wifi fix frm zpp/wwan fix is lesser than mMaxZppWifiConsistencyCheckInMeters
            if ((distance < accuracy_99) || (distance < mMaxZppWifiConsistencyCheckInMeters)) {

                // Accuracy of wifi fix must be lesser than WWAN (To account for cases where punc of wifi fix
                // keeps growing and crosses that of wwan). This needs to be checked only for cases where ZPP fix is
                // not being requested for which is if APWWAN is being used and there is no ALE on those targets.
                if ((mApWwanEnabled) &&
                    (mWifiLocReport.mHorizontalAccuracy > locReportToCompareWith.mHorizontalAccuracy)) {
                    LOC_LOGD("Horizontal Accuracy of WIFI report (%f) is worse than that of "
                        "Cell fix (%f), hence dropping the WIFI fix and reporting Cell fix instead",
                        mWifiLocReport.mHorizontalAccuracy,
                        locReportToCompareWith.mHorizontalAccuracy);
                } else {
                    mLocationReport = mWifiLocReport;
                }
            }
        } else if (mWifiLocReport.isValid()) {
            mLocationReport = mWifiLocReport;
        }

        if (mLocationReport.isValid() == false) {
            mFixReportedInCurrentSession = false;
            break;
        }

        if ((mLocationReport.mHasPositionSource) &&
            (WIFI == mLocationReport.mPositionSource)) {
            // check if the Wifi position being reported was got using 1 AP
            // if yes, then we cannot trust this position as much

            if ((mWifiLocReport.mHasNumberOfApsUsedForFix) && (mWifiLocReport.mNumberOfApsUsedForFix == 1)) {
                // check if we had a valid location report to do consistency check against
                if (locReportToCompareWith.isValid() == false) {
                    LOC_LOGD("Dropping Wifi fix because cache hit is 1 and \
                        no ZPP / WWAN fix available for comparison");
                    mFixReportedInCurrentSession = false;
                    mCountOfDroppedSingleApWifiFix += 1;
                    break;
                }

                // Report the 1 AP Wifi fix with the accuracy of the ZPP  /WWAN fix
                if (mWifiLocReport.mHorizontalAccuracy < locReportToCompareWith.mHorizontalAccuracy) {
                    mLocationReport.mHorizontalAccuracy = locReportToCompareWith.mHorizontalAccuracy;
                }
            }
        }

        if (mLocationReport.mHorizontalAccuracy > mMaxAcuracyThresholdToFilterNlpPosition) {
            LOC_LOGD("Dropping final fix from src = %d Accuracy = %f AccuracyThreshold = %d",
                mLocationReport.mPositionSource, mLocationReport.mHorizontalAccuracy,
                mMaxAcuracyThresholdToFilterNlpPosition);
            mFixReportedInCurrentSession = false;
            mCountOfDroppedFixesDueToLargeRadius += 1;
            break;
        }

        // Automation - log. Please do not touch.
        if (mLocationReport.mHasPositionSource) {
          if (WIFI == mLocationReport.mPositionSource) {
            LOC_LOGD ("LocTech-Label :: QNP :: WiFi Position Out");

            // Automation -log - Please do not touch
            LOC_LOGD ("Reporting XTWiFi fix (%f, %f), unc %f time (ms) %lld",
                      mLocationReport.mLatitude, mLocationReport.mLongitude,
                      mLocationReport.mHorizontalAccuracy, mLocationReport.mUtcTimestampInMsec);
          }
        }

        result = 0;
        mFixReportedInCurrentSession = true;
    }while(0);

    if (mFixReportedInCurrentSession) {
        broadcastLocation(&mLocationReport);
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

double QNP::getGreatCircleDistanceMeter(const LocationReport *loc1, const LocationReport *loc2)
{
    const double CONVERT_TO_RADIANS  = 3.14 / 180;
    const double earth_radius_meter = 6367435.68;
    double lat1, lon1, lat2, lon2;

    // calculation taken from http://www.movable-type.co.uk/scripts/latlong.html
    lat1 = loc1->mLatitude * CONVERT_TO_RADIANS;
    lat2 = loc2->mLatitude * CONVERT_TO_RADIANS;
    lon1 = loc1->mLongitude * CONVERT_TO_RADIANS;
    lon2 = loc2->mLongitude * CONVERT_TO_RADIANS;

    double lat_diff = lat1 - lat2;
    double lon_diff = lon1 - lon2;
    double sin_half_lat_diff = sin(lat_diff / 2);
    double sin_half_lon_diff = sin(lon_diff / 2);
    double h = sin_half_lat_diff * sin_half_lat_diff
               + sin_half_lon_diff * sin_half_lon_diff * cos(lat1) * cos(lat2);
    double m = sqrt(h);
    double m1 = sqrt(1 - h);

    double c = 2 * atan2(m, m1);
    LOC_LOGD("lat1 = %f, lat2 =%f, long1 = %f, long2 = %f, lat_diff = %f "
                  "long_diff = %f, sin_half_lat_diff = %f "
                  "sin_half_long_diff = %f, h = %f, m = %f, m1 = %f, c = %f",
                  lat1, lat2, lon1, lon2, lat_diff, lon_diff, sin_half_lat_diff,
                  sin_half_lon_diff, h, m, m1, c);
    return earth_radius_meter * c;
}

void QNP::increasePUNC(LocationReport & locReport)
{
    int result = -1;

    // Assuming an increase in punc by travelling at 56 miles / hour
    const double puncIncreaseMetersPerSecond = 25;

    // PUNC scaling from from 99% confidence to 68% confidence is (1.07/2.14) = 0.5
    const double ScaleFactorPuncConf = 0.5;
    struct timespec time_info_current;

    do {
        int64 currentTimeInMilliSec;
        BREAK_IF_ZERO(1, locReport.isValid());
        BREAK_IF_ZERO(2, locReport.mHasUtcTimestampInMsec);
        BREAK_IF_NON_ZERO(3, clock_gettime(CLOCK_REALTIME,&time_info_current));
        currentTimeInMilliSec = (time_info_current.tv_sec)*1e3 +
                           (time_info_current.tv_nsec)/1e6;

        int64 ageOfFixInMilliSec = currentTimeInMilliSec - locReport.mUtcTimestampInMsec;
        int ageOfFixInSec = ageOfFixInMilliSec / 1000;

        LOC_LOGD("currentTime = %lld UtcTimeInMsec = %lld",
            currentTimeInMilliSec, locReport.mUtcTimestampInMsec);

        if (ageOfFixInSec > 0) {
            float puncIncreaseInMeters = ageOfFixInSec * puncIncreaseMetersPerSecond;
            LOC_LOGD("Increase punc by %f for %d secs", puncIncreaseInMeters, ageOfFixInSec);

            float accuracy_99 = puncIncreaseInMeters +
                (locReport.mHorizontalAccuracy / ScaleFactorPuncConf);
            float accuracy_68 = accuracy_99 * ScaleFactorPuncConf;

            LOC_LOGD("Previous punc (68%%) is %f, adjusted for punc growth to %f (68%%)",
                locReport.mHorizontalAccuracy, accuracy_68);

            locReport.mHorizontalAccuracy = accuracy_68;
            locReport.mUtcTimestampInMsec = currentTimeInMilliSec;
        }

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}



float QNP::getPUNCIncrease(const LocationReport & locReport)
{
    int result = -1;
    float puncIncreaseInMeters = 0;

    // Assuming an increase in punc by travelling at 56 miles / hour
    const double puncIncreaseMetersPerSecond = 25;
    struct timespec time_info_current;

    do {
        int64 currentTimeInMilliSec;
        BREAK_IF_ZERO(1, locReport.isValid());
        BREAK_IF_ZERO(2, locReport.mHasUtcTimestampInMsec);
        BREAK_IF_NON_ZERO(3, clock_gettime(CLOCK_REALTIME,&time_info_current));
        currentTimeInMilliSec = (time_info_current.tv_sec)*1e3 +
                           (time_info_current.tv_nsec)/1e6;

        int64 ageOfFixInMilliSec = currentTimeInMilliSec - locReport.mUtcTimestampInMsec;
        int ageOfFixInSec = ageOfFixInMilliSec / 1000;

        LOC_LOGD("currentTime = %lld UtcTimeInMsec = %lld",
            currentTimeInMilliSec, locReport.mUtcTimestampInMsec);

        if (ageOfFixInSec > 0) {
            puncIncreaseInMeters = ageOfFixInSec * puncIncreaseMetersPerSecond;
            LOC_LOGD("Increase punc by %f for %d secs", puncIncreaseInMeters, ageOfFixInSec);
        }

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return puncIncreaseInMeters;
}


bool QNP::shouldApplyPuncGrowthModel(LocationReport & savedLocReport)
{
    int result = -1;
    bool applyIncreasePUNC = false;

    do {
        // if AP Cell is enabled then most probably there is no ALE/ZPP, hence we need to propagate WIFI
        // and WWAN fixes to replicate the behavior. If Modem Cell is enabled then ZPP fixes are also available
        // and there is no need for QNP to do saved fix propagation.
        BREAK_IF_ZERO(0, mApWwanEnabled);

        BREAK_IF_ZERO(0, mTimeForSavedPositionToBeValidInMinutes);
        BREAK_IF_ZERO(0, savedLocReport.mHasElapsedRealTimeInNanoSecs);

        int64 puncInvalidateTimeNanoSec =
            convertSecsToNanoSecs(mTimeForSavedPositionToBeValidInMinutes * 60);

        int64 nowInNanoSec = 0;
        BREAK_IF_NON_ZERO(2, GetTimeSinceBoot(nowInNanoSec));


        LOC_LOGD("Compare with saved postion now = %lld" \
                     " mHasElapsedRealTimeInNanoSecs = %d mElapsedRealTimeInNanoSecs = %lld" \
                     " Invalidate time = %lld",
                     nowInNanoSec, savedLocReport.mHasElapsedRealTimeInNanoSecs,
                     savedLocReport.mElapsedRealTimeInNanoSecs, puncInvalidateTimeNanoSec);
        if ((nowInNanoSec < savedLocReport.mElapsedRealTimeInNanoSecs) ||
            ((nowInNanoSec - savedLocReport.mElapsedRealTimeInNanoSecs) > puncInvalidateTimeNanoSec)) {
            savedLocReport.reset();
        } else {
            applyIncreasePUNC = true;
        }

       result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return applyIncreasePUNC;
}
}
