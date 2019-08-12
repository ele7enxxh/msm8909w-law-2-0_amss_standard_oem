/******************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

  @file   UserspaceCtlService.h
  @brief  FDCC server side Interface
*******************************************************************************/

#ifndef ANDROID_USERSPACECTLSERVICE_H
#define ANDROID_USERSPACECTLSERVICE_H

#include <binder/Parcel.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include "UserspaceCtl.h"

using namespace android;

namespace userspaceCtl {

class BnUserspaceCtl : public BnInterface<IUserspaceCtl> {
    virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);
};

class UserspaceCtlService : public BnUserspaceCtl {
private:
    UserspaceCtlService();
    UserspaceCtlService(UserspaceCtlService const& rh);
    UserspaceCtlService& operator=(UserspaceCtlService const& rh);
    static UserspaceCtlService sUserspaceCtlService;

public:
    // TODO: Check for method scope
    virtual ~UserspaceCtlService();
    virtual void Dispatch(int32_t data);
    void Run();
    static UserspaceCtlService& getInstance();
};

} // namespace userspaceCtl
#endif // ANDROID_USERSPACECTLSERVICE_H

