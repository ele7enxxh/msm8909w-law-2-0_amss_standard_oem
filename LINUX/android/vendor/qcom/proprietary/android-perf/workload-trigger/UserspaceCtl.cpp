/******************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

  @file   UserspaceCtl.cpp
  @brief  Userspace control service for parsing recieved data and
          making core_ctl decision
*******************************************************************************/

#include <sys/stat.h>
#include <cutils/log.h>
#include <utils/Trace.h>
#include "mp-ctl/mp-ctl.h"
#include "UserspaceCtl.h"

#undef LOG_TAG
#define LOG_TAG "FDCC-CTL-MAIN"

#define FPS_THRESHOLD 50
#define DIRTY_REGION_THRESHOLD 50

namespace userspaceCtlCore {
UserspaceCtl UserspaceCtl::mUserspaceCtl;

UserspaceCtl::UserspaceCtl() : mFpsTracker(), mPerflockHandler() {
    pthread_mutex_init(&mMutex, 0);
    pthread_cond_init(&mCond, NULL);
}

UserspaceCtl::~UserspaceCtl() {
    pthread_mutex_destroy(&mMutex);
    pthread_cond_destroy(&mCond);
}

UserspaceCtl& UserspaceCtl::getInstance() {
    return mUserspaceCtl;
}

//Function to parse the input value
void UserspaceCtl::Feed(int data) {
    //  ------------------
    // | FU | DR | vid|fmt|
    //  ------------------
    //read frame, dirty region %, frame fmt

    mFpsTracker.SetFrameUpdate();

    pthread_mutex_lock(&mMutex);
    mDirtyRegionPercent = data;

    //wakeup userspaceCtl daemon
    pthread_cond_signal(&mCond);
    pthread_mutex_unlock(&mMutex);
}

//decision making algorithm
void UserspaceCtl::Run() {
    int dr = 0;

    do {
        pthread_mutex_lock(&mMutex);
        dr = mDirtyRegionPercent;
        pthread_mutex_unlock(&mMutex);

        QLOGI("Dirty region for current frame: %d%%", dr);
        if (mFpsTracker.GetFps(mFpsTracker.NUM_FRAMES_TO_CALC_FPS) > FPS_THRESHOLD && dr > DIRTY_REGION_THRESHOLD) {
            int perf_lock_opts[2] = {MPCTLV3_MAX_ONLINE_CPU_CLUSTER_BIG ,0x4};
            QLOGI("Acquiring perflock for setting max_cpus to 4");
            mPerflockHandler.AcquirePerflock(500, perf_lock_opts, 2);
        }
        else {
            QLOGI("Call for releasing perflock");
            mPerflockHandler.ReleasePerflock();
        }
        //pthread cond wait here
        Wait();
    } while (true);
}

void UserspaceCtl::Wait(int timeInMs) {
    struct timespec timeToWait;
    struct timeval now;
    uint32_t val;

    if (timeInMs > 0) {
        QLOGI("Mutex lock for timed wait: %dmsec", timeInMs);
        gettimeofday(&now,NULL);
        val = now.tv_usec+1000UL*timeInMs;

        if (val >= 1000000UL) {
            timeToWait.tv_sec = now.tv_sec + val / 1000000UL;
            timeToWait.tv_nsec = (val % 1000000UL) * 1000UL;
        } else {
            timeToWait.tv_sec = now.tv_sec;
            timeToWait.tv_nsec = val * 1000UL;
        }
        pthread_mutex_lock(&mMutex);
        pthread_cond_timedwait(&mCond, &mMutex, &timeToWait);
    }
    else {
        QLOGI("Mutex lock for conditional wait");
        pthread_mutex_lock(&mMutex);
        pthread_cond_wait(&mCond, &mMutex);
    }

    pthread_mutex_unlock(&mMutex);
}

} //namespace userspaceCtlCore

