/******************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

  @file   UserspaceCtlServiceMain.cpp
  @brief  main function to start perffeedbackd daemon
*******************************************************************************/

#include <fcntl.h>
#include <sys/types.h>
#include <binder/Parcel.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <utils/RefBase.h>
#include <cutils/properties.h>
#include "UserspaceCtl.h"
#include "UserspaceCtlService.h"



using namespace android;
using namespace userspaceCtl;

int main(void) {
    char buf[PROPERTY_VALUE_MAX];
    bool workloadTriggerEnabled = false;

    //check for feature enablement
    if (property_get("ro.qti.userspacectl", buf, NULL) <= 0) {
        QLOGE("%s feedback core_ctl property not found\n", __func__);
        return 0;
    }

    if (!strncmp(buf, "true", strlen("true"))) {
        workloadTriggerEnabled = true;
    }

    if (workloadTriggerEnabled) {
        UserspaceCtlService& service = UserspaceCtlService::getInstance();
        defaultServiceManager()->addService(String16(USERSPACE_CTL_SERVICE), &service);
        ProcessState::self()->startThreadPool();
        service.Run();
        IPCThreadState::self()->joinThreadPool();
    }
    return 0;
}

