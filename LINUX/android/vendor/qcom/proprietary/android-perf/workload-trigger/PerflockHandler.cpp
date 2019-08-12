/******************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

  @file   PerflockHandler.cpp
  @brief  Module to load perflock library and use perflock
*******************************************************************************/

#include "PerflockHandler.h"
#include <cutils/properties.h>

#undef LOG_TAG
#define LOG_TAG "FDCC-PERFLOCK"
#define MSEC_TO_NSEC 1000000

PerflockHandler::PerflockHandler() {
    mDlHandle = NULL;
    mReqHandle = -1;
    mPerflockAcq = NULL;
    mPerflockRel = NULL;
    mDuration = 0;
    mAcqTS = 0;
    OpenPerflock();
}

PerflockHandler::~PerflockHandler() {
    ClosePerflock();
}


void PerflockHandler::OpenPerflock(void) {
    const char *rc = NULL;
    char buf[PROPERTY_VALUE_MAX];

    /* Retrieve name of vendor extension library */
    if (property_get("ro.vendor.extension_library", buf, NULL) <= 0) {
        QLOGE("%s perflock path not found\n", __func__);
        return;
    }

    /* Sanity check - ensure */
    buf[PROPERTY_VALUE_MAX-1] = '\0';
    if (strstr(buf, "/") != NULL) {
        QLOGE("%sInvalid perf lock extn lib\n", __func__);
        return;
    }

    /** Clear error and load lib. */
    dlerror();
    mDlHandle = dlopen(buf, RTLD_NOW | RTLD_LOCAL);
    if (NULL == mDlHandle) {
        QLOGE("%s Failed to (dl)open perf lock\n", __func__);
        return;
    }

    mPerflockAcq = (PerflockAcquire)dlsym(mDlHandle, "perf_lock_acq");
    if ((rc = dlerror()) != NULL) {
        QLOGE("%s Failed to get perf_lock_acq\n", __func__);
        dlclose(mDlHandle);
        mDlHandle = NULL;
        return;
    }

    mPerflockRel = (PerflockRelease)dlsym(mDlHandle, "perf_lock_rel");
    if ((rc = dlerror()) != NULL) {
        QLOGE("%s Failed to get perf_lock_acq\n", __func__);
        dlclose(mDlHandle);
        mDlHandle = NULL;
        return;
    }
}

void PerflockHandler::ClosePerflock(void) {
    if (mDlHandle) {
        dlclose(mDlHandle);
        mDlHandle = NULL;
    }
}

void PerflockHandler::AcquirePerflock(int duration, int opts[], int numOptArgs) {
    nsecs_t nowTime = systemTime();
    bool bReq = false;

    if (mReqHandle < 0) {
        //new request
        bReq = true;
        mAcqTS = nowTime;
        mDuration = duration * MSEC_TO_NSEC;
    } else {
        //already exists
        if ((nowTime - mAcqTS) > mDuration) {
            //duration has elapsed since last perf_lock, so reset lock again
            mReqHandle = -1;
            mDuration = duration * MSEC_TO_NSEC;
            mAcqTS = nowTime;
            bReq = true;
        }
    }

    if (bReq && (mPerflockAcq != NULL)) {
        mReqHandle = mPerflockAcq(mReqHandle, duration, opts, numOptArgs);
        QLOGI("Perf lock acquired handle=%d duration=%d", mReqHandle, duration);
    }
}

void PerflockHandler::ReleasePerflock(void) {
    if ((mPerflockRel != NULL) && (mReqHandle >= 0)) {
        mPerflockRel(mReqHandle);
        QLOGI("Perf lock released handle=%d", mReqHandle);
        mReqHandle = -1; //reset perflock request handle
        mDuration = 0;
        mAcqTS = 0;
    }
}

