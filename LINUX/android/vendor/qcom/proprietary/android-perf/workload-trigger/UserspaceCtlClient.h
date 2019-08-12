/******************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

  @file   UserspaceCtlClient.h
  @brief  FDCC Client Interface
*******************************************************************************/

#ifndef ANDROID_USERSPACECTLCLIENT_H
#define ANDROID_USERSPACECTLCLIENT_H

#include <binder/Parcel.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include "UserspaceCtl.h"

using namespace android;

namespace userspaceCtl {

class BpUserspaceCtl : public BpInterface<IUserspaceCtl> {
    public:
        BpUserspaceCtl(const sp<IBinder>& impl) : BpInterface<IUserspaceCtl>(impl) {
        }

        virtual void Dispatch(int32_t data);
};

class ClientDeathRecipient : public IBinder::DeathRecipient {
    public:
        virtual void binderDied(const android::wp<IBinder>& who);
        virtual ~ClientDeathRecipient();
};

class UserspaceCtlClient {
private:
    UserspaceCtlClient();
    UserspaceCtlClient(UserspaceCtlClient const& rh);
    UserspaceCtlClient& operator=(UserspaceCtlClient const& rh);
    static UserspaceCtlClient *sUserspaceCtlClient;
    sp<IUserspaceCtl> mUserspaceCtlBinder;

public:
    bool workloadTriggerEnabled;
    virtual ~UserspaceCtlClient();
    static UserspaceCtlClient *getInstance();
    sp<ClientDeathRecipient> deathRecipient;
    inline sp<IUserspaceCtl> GetBinder();
    inline void SetBinder(sp<IUserspaceCtl> UserspaceCtlBinder);
};

} // namespace userspaceCtl
#endif // ANDROID_USERSPACECTLCLIENT_H

