/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

#ifndef __IZAT_MANAGER_IZATCONTEXT_H__
#define __IZAT_MANAGER_IZATCONTEXT_H__

#include <MsgTask.h>
#include "IPCMessagingProxy.h"

namespace izat_manager
{
class IOSFramework;
class IDataItemSubscription;
class IPCMessagingProxy;
class OSObserver;

struct s_IzatContext {
    IOSFramework *mOSFrameworkObj;
    IDataItemSubscription *mSubscriptionObj;
    IPCMessagingProxy *mIPCMessagingProxyObj;
    OSObserver *mOSObserverObj;
    MsgTask *mMsgTask;

   inline s_IzatContext() : mOSFrameworkObj(NULL), mSubscriptionObj(NULL),
        mIPCMessagingProxyObj(NULL), mOSObserverObj(NULL), mMsgTask(NULL) {
    }
};

}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IZATCONTEXT_H__

