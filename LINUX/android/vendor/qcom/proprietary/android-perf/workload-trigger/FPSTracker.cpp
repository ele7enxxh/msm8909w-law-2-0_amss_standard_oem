/******************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

  @file   FPSTracker.cpp
  @brief  Android performance FPS Tracker
*******************************************************************************/

#include <cutils/log.h>
#include <cutils/properties.h>
#include "FPSTracker.h"

#undef LOG_TAG
#define LOG_TAG "FDCC-FPSTRACKER"

#define SEC_TO_NSEC (1000000000)

FpsTracker::FpsTracker() {
    for(int i=0; i<NUM_FRAMES_TO_TRACK; i++) {
        mCommitTimeNs[i] = 0;
    }
    pthread_mutex_init(&mMutex, 0);
    mCurrFrame = 0;
}

FpsTracker::~FpsTracker() {
    pthread_mutex_destroy(&mMutex);
}

/* Setting frame commit time with current time
   This call comes with every frame submit */
void FpsTracker::SetFrameUpdate() {
    nsecs_t commit_time = systemTime();

    pthread_mutex_lock(&mMutex);
    mCommitTimeNs[mCurrFrame] = commit_time;
    mCurrFrame = (mCurrFrame + 1) % NUM_FRAMES_TO_TRACK;
    pthread_mutex_unlock(&mMutex);
}

float FpsTracker::GetFps(unsigned int numFrames) {
    float fps = 0.;

    if (numFrames == 0 || numFrames > NUM_FRAMES_TO_TRACK) {
        QLOGE("Invalid no. of input frames");
        return fps;
    }

    pthread_mutex_lock(&mMutex);
    int endFrame = mCurrFrame - 1;
    pthread_mutex_unlock(&mMutex);
    // mCurrFrame can be 0
    if (endFrame < 0) {
        endFrame = NUM_FRAMES_TO_TRACK - 1;
    }
    int startFrame = endFrame - numFrames;
    if (startFrame < 0) {
        //Frames across the circular buffer
        startFrame += NUM_FRAMES_TO_TRACK;
    }
    pthread_mutex_lock(&mMutex);
    float timeTakenForNumFrames = ((float)mCommitTimeNs[endFrame] - (float)mCommitTimeNs[startFrame])/SEC_TO_NSEC;
    pthread_mutex_unlock(&mMutex);
    if (timeTakenForNumFrames > 0) {
        fps = numFrames / timeTakenForNumFrames;
    }
    QLOGI("Current FPS=%f", fps);
    return fps;
}

