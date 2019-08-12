/******************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

  @file   IUserspaceCtl.h
  @brief  Interface for client/server binder service
*******************************************************************************/

#ifndef ANDROID_IUSERSPACECTL_H
#define ANDROID_IUSERSPACECTL_H

#include <binder/Parcel.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>

#define USERSPACE_CTL_SERVICE "android.perf.UserspaceCtlService"

namespace userspaceCtl {

// Interface - Shared by server and client
class IUserspaceCtl : public android::IInterface {
    public:
        enum {
            DISP_HAL = android::IBinder::FIRST_CALL_TRANSACTION,
            VID_HAL,
            AUD_HAL
        };
        // Sends a user-provided value to the service
        virtual void Dispatch(int32_t data) = 0;

        DECLARE_META_INTERFACE(UserspaceCtl);
};

android::sp<IUserspaceCtl> getUserspaceCtlService();

}; // namespace userspaceCtl
#endif // ANDROID_IUSERSPACECTL_H

