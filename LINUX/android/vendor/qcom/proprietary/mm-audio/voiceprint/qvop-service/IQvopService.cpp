/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "BpQvopService.h"

namespace android {

const char* IQvopService::getServiceName() {
  return "android.apps.IQvopService";
}

IMPLEMENT_META_INTERFACE(QvopService, IQvopService::getServiceName());

}; // namespace android
