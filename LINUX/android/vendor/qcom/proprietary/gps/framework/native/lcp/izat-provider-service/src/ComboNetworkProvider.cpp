/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
GENERAL DESCRIPTION
  Combo Network Provider module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#define LOG_NDDEBUG 0
#define LOG_TAG "IzatSvc_ComboNetworkProvider"

#include "ComboNetworkProvider.h"
#include "IOSFramework.h"
#include "IOSFrameworkCleaner.h"
#include "QNP.h"
#include "OSNPProxy.h"
#include "loc_cfg.h"

#include "IDataItem.h"
#include "DataItemId.h"
#include "IDataItemCopier.h"
#include "DataItemConcreteTypes.h"
#include "DataItemsFactory.h"

namespace izat_manager
{

ILocationProvider* ComboNetworkProvider::mComboNetworkProvider = NULL;

void ComboNetworkProvider::enableComboMsg::proc() const
{
    ENTRY_LOG();

    if (COMBO_STATE_DISABLED == mCombo->mProviderState) {
        mCombo->mProviderState = COMBO_STATE_IDLE;

        if (mCombo->mQNP != NULL) {
            mCombo->mQNP->enable();
        }

        if (mCombo->mOSNP != NULL) {
            mCombo->mOSNP->enable();
        }
    }

    LOC_LOGD ("LocTech-Label :: COMBO :: ComboNetworkProviderState");
    LOC_LOGD ("LocTech-Value :: Combo State: %d", mCombo->mProviderState);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::disableComboMsg::proc() const
{
    ENTRY_LOG();

    if ((COMBO_STATE_IDLE == mCombo->mProviderState) ||
         (COMBO_STATE_TRACKING == mCombo->mProviderState)) {
        // On-going request / timer must be stopped
        LocationRequest locRequest = {ILocationProvider::LocationRequestAction_Stop, 0};
        mCombo->stopTrackingSession(&locRequest);

        // Just change the state to disabled and exit.
        // If location comes after changing the state to disabled, location will be dropped.
        mCombo->mProviderState = COMBO_STATE_DISABLED;

        if (mCombo->mQNP != NULL) {
            mCombo->mQNP->disable();
        }

        if (mCombo->mOSNP != NULL) {
            mCombo->mOSNP->disable();
        }
    }

    LOC_LOGD ("LocTech-Label :: COMBO :: ComboNetworkProviderState");
    LOC_LOGD ("LocTech-Value :: Combo State: %d", mCombo->mProviderState);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::requestLocationMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_NON_ZERO(1, COMBO_STATE_DISABLED == mCombo->mProviderState);

        if ((LocationRequestAction_SingleShot == mLocRequest.action) ||
            (LocationRequestAction_Start == mLocRequest.action)) {
            // Save current request just in case EULA changes
            mCombo->mCurrentRequest = mLocRequest;

            if (COMBO_STATE_IDLE == mCombo->mProviderState) {
                LOC_LOGD("In IDLE state, start tracking");
                mCombo->startTrackingSession(&mLocRequest);
                mCombo->mProviderState = COMBO_STATE_TRACKING;
            } else {
                LOC_LOGD("In the middle of position request, update tracking session if required.");
                mCombo->updateTrackingSession(&mLocRequest);
            }
        } else if (LocationRequestAction_Stop == mLocRequest.action) {
            mCombo->stopTrackingSession(&mLocRequest);
            mCombo->mProviderState = COMBO_STATE_IDLE;
        } else {
            LOC_LOGE("Unhandled LocationRequest type.");
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::toleranceTimerMsg::proc() const
{
    ENTRY_LOG();

    int result = -1;
    do {
         BREAK_IF_ZERO(1, COMBO_STATE_TRACKING == mCombo->mProviderState);

        if ((mCombo->mQNPLocReport.isValid() == false) &&
            (mCombo->mFrameworkLocReport.isValid() == false)) {
            mCombo->mToleranceExpired = true;
            break;
        } else if (mCombo->mAwaitingFirstFix) {

            if (mCombo->mQNPLocReport.isValid()) {
                LOC_LOGD("Reporting QNP location, as Framework NLP did not report within %d ms",
                    mCombo->mToleranceTimeFirstInMsec);
                mCombo->reportBestLocation(mCombo->mQNPLocReport);
                mCombo->mOSNPPreferred = false;
            } else if (mCombo->mFrameworkLocReport.isValid()) {
                LOC_LOGD("Reporting Framework NLP location, as QNP did not report within %d ms",
                    mCombo->mToleranceTimeFirstInMsec);
                mCombo->reportBestLocation(mCombo->mFrameworkLocReport);
                mCombo->mOSNPPreferred = true;
            }

            mCombo->increamentAdaptiveCounter();
        } else if (mCombo->mOSNPPreferred) {
            if ((mCombo->mQNPLocReport.isValid() == false) &&
                (mCombo->mFrameworkLocReport.isValid())) {
                mCombo->increamentAdaptiveCounter();
            } else if ((mCombo->mQNPLocReport.isValid()) &&
                (mCombo->mFrameworkLocReport.isValid() == false)) {
                mCombo->decreamentAdaptiveCounter();
            }
        } else {
            if ((mCombo->mQNPLocReport.isValid()) &&
                (mCombo->mFrameworkLocReport.isValid() == false)) {
                mCombo->increamentAdaptiveCounter();
            } else if ((mCombo->mQNPLocReport.isValid() == false) &&
                (mCombo->mFrameworkLocReport.isValid())) {
                mCombo->decreamentAdaptiveCounter();
            }
        }

        // reset the next combo state variables for next interval
        mCombo->resetForNextInterval(
            capTouint32(mCombo->mToleranceTimeAfterInMsec + (int64) mCombo->mCurrentTimeBetweenFixInMsec));
        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::reportLocationMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, COMBO_STATE_TRACKING == mCombo->mProviderState);
        mCombo->reportBestLocation(mLocationReport);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}


void ComboNetworkProvider::handleQNPLocReportMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, COMBO_STATE_TRACKING == mCombo->mProviderState);

        if (mCombo->mAwaitingFirstFix) {
            if (mCombo->mFrameworkLocReport.isValid()) {
                mCombo->consolidateFirstFixesAndReport();
            } else if (mCombo->mToleranceExpired) {
                LOC_LOGD("Reporting QNP location, as Framework NLP did not report within %d ms",
                    mCombo->mToleranceTimeFirstInMsec);

                mCombo->reportBestLocation(mCombo->mQNPLocReport);
                mCombo->mOSNPPreferred = false;
            } // else the QNP fix is anyways saved, and we
            // continue to wait for Framework Nlp fix
        } else if (!mCombo->mOSNPPreferred) {
            LOC_LOGD("Reporting QNP location");
            mCombo->reportBestLocation(mCombo->mQNPLocReport);

            if (mCombo->mToleranceExpired) {
                mCombo->increamentAdaptiveCounter();
            }
        } else {
            LOC_LOGD("Dropping non-preferred QNP location.");
            if (mCombo->mToleranceExpired) {
                mCombo->decreamentAdaptiveCounter();
            }
        }

        mCombo->adjustAdaptiveCounter();

        if (((mCombo->mFrameworkLocReport.isValid()) &&
            (mCombo->mQNPLocReport.isValid())) || mCombo->mToleranceExpired) {

            mCombo->resetForNextInterval(
                capTouint32(mCombo->mToleranceTimeAfterInMsec + (int64)mCombo->mCurrentTimeBetweenFixInMsec));
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::handleFrameworkLocReportMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, COMBO_STATE_TRACKING == mCombo->mProviderState);

        if (mCombo->mAwaitingFirstFix) {
            if (mCombo->mQNPLocReport.isValid()) {
                mCombo->consolidateFirstFixesAndReport();
            } else if (mCombo->mToleranceExpired) {
                LOC_LOGD("Reporting Framework NLP location, \
                    as QNP did not report within %d ms", mCombo->mToleranceTimeFirstInMsec);

                mCombo->reportBestLocation(mCombo->mFrameworkLocReport);
                mCombo->mOSNPPreferred = true;
            } else if (!mCombo->mEULAAccepted) {
                LOC_LOGD("Reporting Framework NLP location as EULA is not accepted");
                mCombo->reportBestLocation(mCombo->mFrameworkLocReport);
                mCombo->mOSNPPreferred = true;
            } // else the Framework Nlp fix is anyways saved, and we
            // continue to wait for QNP fix
        } else if (mCombo->mOSNPPreferred) {
            LOC_LOGD("Reporting Framework NLP location");

            mCombo->reportBestLocation(mCombo->mFrameworkLocReport);
            if (mCombo->mToleranceExpired) {
                mCombo->increamentAdaptiveCounter();
            }
        } else {
            LOC_LOGD("Dropping non-preferred Framework NLP location.");
            if (mCombo->mToleranceExpired) {
                mCombo->decreamentAdaptiveCounter();
            }
        }

        mCombo->adjustAdaptiveCounter();

        if (((mCombo->mFrameworkLocReport.isValid()) &&
            (mCombo->mQNPLocReport.isValid())) || mCombo->mToleranceExpired) {

            mCombo->resetForNextInterval(
                capTouint32(mCombo->mToleranceTimeAfterInMsec + (int64) mCombo->mCurrentTimeBetweenFixInMsec));
        }
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::subscribeEULAMsg::proc() const
{
    int result = -1;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, mCombo != NULL);

        std::list<DataItemId> dataItemIdList(1, ENH_DATA_ITEM_ID);
        mCombo->mIzatContext->mOSObserverObj->subscribe(dataItemIdList, mCombo);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

ComboNetworkProvider::handleOsObserverUpdateMsg::
            handleOsObserverUpdateMsg(ComboNetworkProvider *pCombo,
            const std::list<IDataItem *> & dataItemList) : mCombo(pCombo)
{
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
        }
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::handleOsObserverUpdateMsg::proc() const
{
    ENTRY_LOG();
    int result = -1;

        do {
            std::list<IDataItem *>::const_iterator it = mDataItemList.begin();
            for (; it != mDataItemList.end(); it++) {
                IDataItem* dataItem = *it;
                switch (dataItem->getId())
                {
                    case ENH_DATA_ITEM_ID:
                    {
                        ENHDataItem *enh = reinterpret_cast<ENHDataItem*>(dataItem);
                        mCombo->handleEULAUpdate(enh->mEnabled);
                        result = 0;
                    }
                    break;

                    default:
                    break;
                }
            }
        } while(0);
        EXIT_LOG_WITH_ERROR("%d", result);
}

ComboNetworkProvider::handleOsObserverUpdateMsg::~handleOsObserverUpdateMsg()
{
    std::list<IDataItem *>::const_iterator it = mDataItemList.begin();
    for (; it != mDataItemList.end(); it++)
    {
        delete *it;
    }
}

ComboNetworkProvider::ComboNetworkProvider(const struct s_IzatContext *izatContext,
        ILocationProvider* pQNP, ILocationProvider* pOSNP,
        NlpMode mode, unsigned int toleranceTimeFirstInMsec,
        unsigned int toleranceTimeAfterInMsec, unsigned int nlpThreshold,
        unsigned int accuracyMultiple,
        unsigned int nlpComboModeUsesQnpWithNoEulaConsent) :
    LocationProvider(izatContext),mQNP(pQNP), mOSNP(pOSNP),
    mNlpMode(mode), mProviderState(COMBO_STATE_DISABLED), mAwaitingFirstFix(false),
    mOSNPPreferred(false), mAdaptiveCounter(0), mToleranceExpired(false),
    mCurrentTimeBetweenFixInMsec(0), mToleranceTimeFirstInMsec(toleranceTimeFirstInMsec),
    mToleranceTimeAfterInMsec(toleranceTimeAfterInMsec), mNlpThreshold(nlpThreshold),
    mAccuracyMultiple(accuracyMultiple), mToleranceTimer(this),
    mNlpComboModeUsesQnpWithNoEulaConsent(nlpComboModeUsesQnpWithNoEulaConsent!=0),
    mEULAAccepted(false)
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow)subscribeEULAMsg(this));

    if (mQNP != NULL) {
        mQNP->subscribe(this);
    }

    if (mOSNP != NULL) {
        mOSNP->subscribe(this);
    }
}

ComboNetworkProvider::~ComboNetworkProvider()
{
    if (mQNP != NULL) {
        QNP::destroyInstance();
        mQNP = NULL;
    }

    if (mOSNP != NULL) {
        delete mOSNP;
        mOSNP = NULL;
    }
}

ILocationProvider* ComboNetworkProvider::getInstance(const struct s_IzatContext* izatContext)
{
    int result = 0;

    // minimum accpetable values
    const unsigned int toleranceTimeFirstMinInMsec = 100;
    const unsigned int toleranceTimeAfterMinInMsec = 100;
    const unsigned int nlpThresholdMin = 1;
    const unsigned int nlpAccuracyMultipleMin = 1;

    // default values if not specified in the izat.conf.
    const unsigned int toleranceTimeFirstDefaultInMsec = 2000;
    const unsigned int toleranceTimeAfterDefaultInMsec = 20000;
    const unsigned int nlpThresholdDefault = 3;
    const unsigned int accuracyMultipleDefault = 2;
    const NlpMode nlpModeDefault = NLP_MODE_NONE;
    const unsigned int nlpComboModeUsesQNPWithNoEULAConsentDefault = 1;

    unsigned int toleranceTimeFirstInMsec = toleranceTimeFirstDefaultInMsec;
    unsigned int toleranceTimeAfterInMsec = toleranceTimeAfterDefaultInMsec;
    unsigned int nlpThreshold = nlpThresholdDefault;
    unsigned int accuracyMultiple = accuracyMultipleDefault;
    NlpMode nlpMode = nlpModeDefault;
    unsigned int nlpComboModeUsesQnpWithNoEulaConsent =
                            nlpComboModeUsesQNPWithNoEULAConsentDefault;

    ILocationProvider* pQNP = NULL;
    ILocationProvider* pOSNP = NULL;

    static loc_param_s_type izat_conf_param_table[] = {
       {"NLP_MODE", &nlpMode, NULL, 'n'},
       {"NLP_TOLERANCE_TIME_FIRST", &toleranceTimeFirstInMsec, NULL, 'n'},
       {"NLP_TOLERANCE_TIME_AFTER", &toleranceTimeAfterInMsec, NULL, 'n'},
       {"NLP_THRESHOLD", &nlpThreshold, NULL, 'n'},
       {"NLP_ACCURACY_MULTIPLE", &accuracyMultiple, NULL, 'n'},
       {"NLP_COMBO_MODE_USES_QNP_WITH_NO_EULA_CONSENT",
        &nlpComboModeUsesQnpWithNoEulaConsent, NULL, 'n'},
    };

    ENTRY_LOG();

    do {

        // Combo already intialized
        BREAK_IF_NON_ZERO(0, mComboNetworkProvider);
        BREAK_IF_ZERO(1, izatContext);

        // Read the default configurations from the izat.conf first
        UTIL_READ_CONF(mIzatConfFile.c_str(), izat_conf_param_table);

        if (nlpMode > NLP_MODE_COMBO || nlpMode < NLP_MODE_NONE) {
            nlpMode = NLP_MODE_NONE;
        }

        // Check for minimum toleranceTimeFirst
        toleranceTimeFirstInMsec =
            (toleranceTimeFirstInMsec < toleranceTimeFirstMinInMsec ?
            toleranceTimeFirstMinInMsec : toleranceTimeFirstInMsec);

        // Check for minimum toleranceTimeAfter
        toleranceTimeAfterInMsec =
            (toleranceTimeAfterInMsec < toleranceTimeAfterMinInMsec ?
            toleranceTimeAfterMinInMsec: toleranceTimeAfterInMsec);

        // Check for minimum threshold set for nlp selection
        nlpThreshold = (nlpThreshold < nlpThresholdMin ?
            nlpThresholdMin: nlpThreshold);

        // Check for minimum accuracy factory to be used to compare against.
        accuracyMultiple = (accuracyMultiple < nlpAccuracyMultipleMin ?
            nlpAccuracyMultipleMin : accuracyMultiple);

        // Decide on the final mode
        if (NLP_MODE_QNP_ONLY == nlpMode) {
            pQNP = QNP::getInstance(izatContext);
            if (pQNP == NULL) {
                pOSNP = OSNPProxy::createInstance(izatContext);
                if (pOSNP != NULL) {
                    nlpMode = NLP_MODE_GNP_ONLY;
                } else {
                    nlpMode = NLP_MODE_NONE;
                }
            }
        }
        else if (NLP_MODE_GNP_ONLY == nlpMode) {
            pOSNP = OSNPProxy::createInstance(izatContext);
            if (pOSNP == NULL) {
                pQNP = QNP::getInstance(izatContext);
                if (pQNP != NULL) {
                    nlpMode = NLP_MODE_QNP_ONLY;
                } else {
                    nlpMode = NLP_MODE_NONE;
                }
            }
        }
        else if ((NLP_MODE_COMBO == nlpMode) || (NLP_MODE_NONE == nlpMode)) {
            pQNP = QNP::getInstance(izatContext);
            pOSNP = OSNPProxy::createInstance(izatContext);

            if ((pQNP == NULL) || (pOSNP == NULL)) {
                if (pQNP != NULL) {
                    nlpMode = NLP_MODE_QNP_ONLY;
                } else if (pOSNP != NULL) {
                    nlpMode = NLP_MODE_GNP_ONLY;
                } else {
                    nlpMode = NLP_MODE_NONE;
                }
            } else {
                nlpMode = NLP_MODE_COMBO;
            }
        }

        // this should not happen, but just in case
        BREAK_IF_NON_ZERO(3, NLP_MODE_NONE == nlpMode);

        LOC_LOGD ("LocTech-Label :: COMBO :: Combo Mode");
        LOC_LOGD ("LocTech-Value :: NLP_MODE = %d NLP_TOLERANCE_TIME_FIRST (ms) = %d NLP_TOLERANCE_TIME_AFTER (ms)= %d \
            NLP_THRESHOLD = %d NLP_ACCURACY_MULTIPLE = %d NLP_COMBO_MODE_USES_QNP_WITH_NO_EULA_CONSENT = %d",
            nlpMode, toleranceTimeFirstInMsec, toleranceTimeAfterInMsec,
            nlpThreshold, accuracyMultiple, nlpComboModeUsesQnpWithNoEulaConsent);

        mComboNetworkProvider = new (nothrow) ComboNetworkProvider(izatContext, pQNP, pOSNP,
            nlpMode, toleranceTimeFirstInMsec, toleranceTimeAfterInMsec,
            nlpThreshold, accuracyMultiple, nlpComboModeUsesQnpWithNoEulaConsent);

        BREAK_IF_ZERO(4, mComboNetworkProvider);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return mComboNetworkProvider;
}

int ComboNetworkProvider::destroyInstance()
{
    ENTRY_LOG();

    delete mComboNetworkProvider;
    mComboNetworkProvider = NULL;

    EXIT_LOG_WITH_ERROR("%d", 0);
    return 0;
}

// ILocationRequest overrides
int ComboNetworkProvider::setRequest(const LocationRequest *request)
{
    ENTRY_LOG();
    int result = 0;

    do {
        BREAK_IF_ZERO(1, request);
        LOC_LOGD ("LocTech-Label :: COMBO :: Position Request In");
        LOC_LOGD ("LocTech-Value :: Action (Single Shot = 0, Start = 1, Stop = 2): %d", request->action);
        LOC_LOGD ("LocTech-Value :: Interval In milliseconds: %u", request->intervalInMsec);
        mIzatContext->mMsgTask->sendMsg(new (nothrow)requestLocationMsg(this, request));
        result = 0;

    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

void ComboNetworkProvider::enable()
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow) enableComboMsg(this));
}

void ComboNetworkProvider::disable()
{
    mIzatContext->mMsgTask->sendMsg(new (nothrow) disableComboMsg(this));
}

// ILocationResponse overrides
/**
* This method can be re-entrant with QNP reporting in context of the
* MsgTask thread and OSNP reporting in context of its own thread.
**/
void ComboNetworkProvider::reportLocation(const LocationReport *report,
    const ILocationProvider *providerSrc)
{
    ENTRY_LOG();

    LOC_LOGD ("LocTech-Label :: COMBO :: Position Report In");

    if (providerSrc == mQNP) {
        LOC_LOGD ("LocTech-Value :: Provider Source: QNP");
    } else if (providerSrc == mOSNP) {
        LOC_LOGD ("LocTech-Value :: Provider Source: OSNP");
    }

    string locationReport;
    report->stringify (locationReport);
    LOC_LOGD ("LocTech-Value :: Location Report: %s", locationReport.c_str());

    if (mNlpMode < NLP_MODE_COMBO) {
        mIzatContext->mMsgTask->sendMsg(new (nothrow) reportLocationMsg(this, report));
    } else {
        if (providerSrc == mQNP) {
            mQNPLocReport = *report;
            mIzatContext->mMsgTask->sendMsg(new (nothrow) handleQNPLocReportMsg(this));
        } else if (providerSrc == mOSNP) {
            mFrameworkLocReport = *report;
            mIzatContext->mMsgTask->sendMsg(new (nothrow) handleFrameworkLocReportMsg(this));
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::reportError(const LocationError* error, const ILocationProvider* providerSrc)
{
    UNUSED(error);
    UNUSED(providerSrc);
}

void ComboNetworkProvider::notify(const std::list<IDataItem *> & dlist)
{
    ENTRY_LOG();
    mIzatContext->mMsgTask->sendMsg(new (nothrow) handleOsObserverUpdateMsg(this, dlist));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::startTrackingSession(const LocationRequest *request)
{
    ENTRY_LOG();

    mAwaitingFirstFix = true;
    mAdaptiveCounter = 0;

    // record the new interval
    mCurrentTimeBetweenFixInMsec = request->intervalInMsec;

    if (NLP_MODE_COMBO == mNlpMode) {
        // QNP is only used if EULA is accepted
        // or forced via NLP_COMBO_MODE_USES_QNP_WITH_NO_EULA_CONSENT
        if (mNlpComboModeUsesQnpWithNoEulaConsent || mEULAAccepted) {
            mQNP->setRequest(request);
        }
        mOSNP->setRequest(request);

        // reset these members for every tolerance timer interval
        resetForNextInterval(mToleranceTimeFirstInMsec);
    } else {
        if (NLP_MODE_QNP_ONLY == mNlpMode) {
            mQNPLocReport.reset();
            mQNP->setRequest(request);
        } else if (NLP_MODE_GNP_ONLY == mNlpMode) {
            mFrameworkLocReport.reset();
            mOSNP->setRequest(request);
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::updateTrackingSession(const LocationRequest *request)
{
    ENTRY_LOG();

    mCurrentTimeBetweenFixInMsec = request->intervalInMsec;

    if (NLP_MODE_COMBO == mNlpMode) {
        // QNP is only used if EULA is accepted
        // or forced via NLP_COMBO_MODE_USES_QNP_WITH_NO_EULA_CONSENT
        if (mNlpComboModeUsesQnpWithNoEulaConsent || mEULAAccepted) {
            mQNP->setRequest(request);
        }
        mOSNP->setRequest(request);

        // 1. If we are waiting on first fix from an ongoing session then we just
        //     continue to do that.
        // 2. If we already have preferred providers then we want to go back to choosing
        //     preferred providers again and setting a tolerance of 5 sec.
        //     This is to avoid too much delay to get first fix to this new client
        //     incase the already selected preferred provider does not return a fix immediately.
        if (false == mAwaitingFirstFix) {
            mAwaitingFirstFix = true;
            mAdaptiveCounter = 0;

            LOC_LOGD("Reset tolerance timer to get first fix for session update.");
            resetForNextInterval(mToleranceTimeFirstInMsec);
        }
    } else {
        if (NLP_MODE_QNP_ONLY == mNlpMode) {
            mQNP->setRequest(request);
        } else if (NLP_MODE_GNP_ONLY == mNlpMode) {
            mOSNP->setRequest(request);
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}


void ComboNetworkProvider::stopTrackingSession(const LocationRequest *request)
{
    ENTRY_LOG();

    if (NLP_MODE_COMBO == mNlpMode) {
        mQNP->setRequest(request);
        mOSNP->setRequest(request);

        mToleranceTimer.stop();
    } else {
        if (NLP_MODE_QNP_ONLY == mNlpMode) {
            mQNP->setRequest(request);
        } else if (NLP_MODE_GNP_ONLY == mNlpMode) {
            mOSNP->setRequest(request);
        }
    }

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::increamentAdaptiveCounter()
{
    ++mAdaptiveCounter;
    if (mAdaptiveCounter > mNlpThreshold) {
        mAdaptiveCounter = mNlpThreshold;
    }

    LOC_LOGD("+Adaptive Counter = %d", mAdaptiveCounter);
}

void ComboNetworkProvider::decreamentAdaptiveCounter()
{
    int thresholdMin = -1 * mNlpThreshold;

    --mAdaptiveCounter;
    if (mAdaptiveCounter <= thresholdMin) {
        mOSNPPreferred = !mOSNPPreferred;
        mAdaptiveCounter = 0;

        if (mOSNPPreferred) {
            LOC_LOGD("Preferred change to Framework NLP");
        } else {
            LOC_LOGD("Preferred change to QNP");
        }
    }

    LOC_LOGD("-Adaptive Counter = %d", mAdaptiveCounter);
}

void ComboNetworkProvider::resetForNextInterval(uint32_t interval)
{
    // reset these variables for every tolerance timer interval

    mToleranceExpired = false;

    // reset all position or error reports
    mQNPLocReport.reset();
    mFrameworkLocReport.reset();
    mQNPErrorReport.reset();
    mFrameworkErrorReport.reset();

    mToleranceTimer.stop();
    mToleranceTimer.start(interval, this);

    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::consolidateFirstFixesAndReport()
{
    if (mFrameworkLocReport.isValid() && mQNPLocReport.isValid()) {
        if (mFrameworkLocReport.mHorizontalAccuracy < mQNPLocReport.mHorizontalAccuracy) {
            mOSNPPreferred = true;

            LOC_LOGD("Reporting Framework NLP location, as %f <= %f",
                mFrameworkLocReport.mHorizontalAccuracy, mQNPLocReport.mHorizontalAccuracy);
            reportBestLocation(mFrameworkLocReport);
        } else {
            mOSNPPreferred = false;

            LOC_LOGD("Reporting QNP location, as %f <= %f",
                mQNPLocReport.mHorizontalAccuracy ,mFrameworkLocReport.mHorizontalAccuracy);
            reportBestLocation(mQNPLocReport);
        }
    }
}

void ComboNetworkProvider::adjustAdaptiveCounter()
{
    if (mFrameworkLocReport.isValid() && mQNPLocReport.isValid()) {
        if (mFrameworkLocReport.mHorizontalAccuracy >=
            mAccuracyMultiple * mQNPLocReport.mHorizontalAccuracy) {

            LOC_LOGD("Framework NLP has much worse accuracy than QNP (%f vs %f)",
                mFrameworkLocReport.mHorizontalAccuracy, mQNPLocReport.mHorizontalAccuracy);

            if (mOSNPPreferred) {
                decreamentAdaptiveCounter();
            } else {
                increamentAdaptiveCounter();
            }
        } else if (mQNPLocReport.mHorizontalAccuracy >=
            mAccuracyMultiple * mFrameworkLocReport.mHorizontalAccuracy) {

            LOC_LOGD("QNP has much worse accuracy than Framework NLP (%f vs %f)",
                mQNPLocReport.mHorizontalAccuracy, mFrameworkLocReport.mHorizontalAccuracy);

            if (mOSNPPreferred) {
                increamentAdaptiveCounter();
            } else {
                decreamentAdaptiveCounter();
            }
        }
    }
}

void ComboNetworkProvider::reportBestLocation(const LocationReport & report)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(2, report.isValid());

        LOC_LOGD("Best location ready for broadcast");

        broadcastLocation(&report);
        mAwaitingFirstFix = false;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void ComboNetworkProvider::Timer::timeOutCallback()
{
    ENTRY_LOG();
    mClient->mIzatContext->mMsgTask->sendMsg(new (nothrow) toleranceTimerMsg(mClient));
    EXIT_LOG_WITH_ERROR("%d", 0);
}

void ComboNetworkProvider::handleEULAUpdate(bool eulaAccepted)
{
    ENTRY_LOG();
    if (eulaAccepted != mEULAAccepted) {
        LOC_LOGD("EULA changed to: %d", eulaAccepted);
        mEULAAccepted = eulaAccepted;

        if ((mNlpMode == NLP_MODE_COMBO) &&
            (mNlpComboModeUsesQnpWithNoEulaConsent == false) &&
            (mProviderState == COMBO_STATE_TRACKING))
        {
            if (mEULAAccepted == true) {
                mQNP->setRequest(&mCurrentRequest);
                LOC_LOGD("QNP is requested after EULA change");
            } else {
                LocationRequest stopRequest = {ILocationProvider::LocationRequestAction_Stop, 0};
                mQNP->setRequest(&stopRequest);
                LOC_LOGD("QNP request canceled after EULA change");
            }
        }
    }
    EXIT_LOG_WITH_ERROR("%d", 0);
}

}
