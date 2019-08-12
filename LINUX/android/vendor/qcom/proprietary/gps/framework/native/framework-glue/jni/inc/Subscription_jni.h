/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_SUBSCRIPTION_JNI_H__
#define __IZAT_MANAGER_SUBSCRIPTION_JNI_H__

#include <DataItemId.h>
#include <list>

using namespace std;
using namespace izat_manager;

void update_subscribeJNI(const std::list<DataItemId> & l, bool subscribe);
void requestDataJNI(const std::list<DataItemId> & l);
void unsubscribeAllJNI();

#endif // #ifndef __IZAT_MANAGER_SUBSCRIPTION_JNI_H__
