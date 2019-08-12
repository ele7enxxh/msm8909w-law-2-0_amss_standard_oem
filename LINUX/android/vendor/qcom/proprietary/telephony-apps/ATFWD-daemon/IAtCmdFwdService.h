/*===========================================================================

Copyright (c) 2015, Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef ANDROID_HARDWARE_IATCMDFWDSERVICE_H
#define ANDROID_HARDWARE_IATCMDFWDSERVICE_H

#include <binder/IInterface.h>
#include <utils/String16.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include "AtCmdFwd.h"


namespace android {

class IAtCmdFwdService : public IInterface
{
public:
    DECLARE_META_INTERFACE(AtCmdFwdService);

    virtual AtCmdResponse *processCommand(const AtCmd &cmd) = 0;
};

class DeathNotifier: public IBinder::DeathRecipient
{
public:
    DeathNotifier() {
    }

    virtual void binderDied(const wp<IBinder>& who);
};

}; // namespace android
#endif // ANDROID_HARDWARE_IATCMDFWDSERVICE_H

