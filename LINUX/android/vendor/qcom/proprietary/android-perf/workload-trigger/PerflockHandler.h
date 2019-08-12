/******************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

  @file   PerflockHandler.h
  @brief  Perflock Handler Interface
*******************************************************************************/

#ifndef ANDROID_PERFLOCKHANDLER_H
#define ANDROID_PERFLOCKHANDLER_H

#include <string.h>
#include <dlfcn.h>
#include <utils/Log.h>
#include <cutils/atomic.h>
#include <utils/Timers.h>
#include "FPSTracker.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*PerflockAcquire)(int, int, int[], int);
typedef void (*PerflockRelease)(int);

class PerflockHandler {
private:
    void *mDlHandle;
    int mReqHandle;
    PerflockAcquire mPerflockAcq;
    PerflockRelease mPerflockRel;
    nsecs_t mAcqTS;
    nsecs_t mDuration;

public:
    PerflockHandler();
    ~PerflockHandler();

    void OpenPerflock(void);
    void ClosePerflock(void);
    void AcquirePerflock(int duration, int opts[], int numOptArgs);
    void ReleasePerflock(void);

};

#ifdef __cplusplus
}
#endif
#endif // ANDROID_PERFLOCKHANDLER_H

