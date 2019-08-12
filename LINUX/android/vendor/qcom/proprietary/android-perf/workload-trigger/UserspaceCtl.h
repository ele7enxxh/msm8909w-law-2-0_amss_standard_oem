/******************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

  @file   UserspaceCtl.h
  @brief  Userspace control Interface
*******************************************************************************/

#ifndef ANDROID_USERSPACECTL_H
#define ANDROID_USERSPACECTL_H

#include <pthread.h>
#include <sys/un.h>
#include "IUserspaceCtl.h"
#include "FPSTracker.h"
#include "PerflockHandler.h"

namespace userspaceCtlCore {

class UserspaceCtl {
    private:
        static UserspaceCtl mUserspaceCtl;
        FpsTracker mFpsTracker;
        PerflockHandler mPerflockHandler;
        int mDirtyRegionPercent;

        pthread_mutex_t mMutex;
        pthread_cond_t mCond;

        UserspaceCtl();
        UserspaceCtl(UserspaceCtl const& rh);
        UserspaceCtl& operator=(UserspaceCtl const& rh);

    public:
        ~UserspaceCtl();
        void Feed(int data);
        void Run();
        void Wait(int timeInMs = 0);
        static UserspaceCtl& getInstance();
};

} //namespace userspaceCtlCore
#endif // ANDROID_USERSPACECTL_H

