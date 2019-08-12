/******************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

  @file   FPSTracker.h
  @brief  Android performance FPS Tracker Interface
*******************************************************************************/

#ifndef ANDROID_FPSTRACKER_H
#define ANDROID_FPSTRACKER_H

#undef ATRACE_TAG
#define ATRACE_TAG ATRACE_TAG_ALWAYS

#include <utils/Timers.h>

#define QC_DEBUG_ERRORS 1
#ifdef QC_DEBUG_ERRORS
#define QLOGE(...)    ALOGE(__VA_ARGS__)
#else
#define QLOGE(...)
#endif

#if QC_DEBUG
#define QLOGW(...)    ALOGW(__VA_ARGS__)
#define QLOGI(...)    ALOGI(__VA_ARGS__)
#define QLOGV(...)    ALOGV(__VA_ARGS__)
#define QCLOGE(...)   ALOGE(__VA_ARGS__)
#else
#define QLOGW(...)
#define QLOGI(...)
#define QLOGV(...)
#define QCLOGE(...)
#endif


#ifdef __cplusplus
extern "C" {
#endif

class FpsTracker {
private:
    static const int NUM_FRAMES_TO_TRACK = 4;
    //protect both the below vars
    nsecs_t mCommitTimeNs[NUM_FRAMES_TO_TRACK];
    unsigned int mCurrFrame;

    pthread_mutex_t mMutex;

public:
    FpsTracker();
    ~FpsTracker();

    static const int NUM_FRAMES_TO_CALC_FPS = 3;
    void SetFrameUpdate(); //need to call once per frame commit
    float GetFps(unsigned int num_frames);
};

#ifdef __cplusplus
}
#endif
#endif // ANDROID_FPSTRACKER_H

