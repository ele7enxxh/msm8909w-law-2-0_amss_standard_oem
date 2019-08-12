/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "BpQSEEConnectorService.h"
#include "BnQSEEConnectorService.h"

namespace android {
  const char* IQSEEConnectorService::getServiceName() {
    return EXPAND_AND_QUOTE(QSEE_CONNECTOR_SERVICE_NAME);
  }

  IMPLEMENT_META_INTERFACE(QSEEConnectorService, IQSEEConnectorService::getServiceName());

  const char* IQSEEConnectorServiceCb::getServiceName() {
    return EXPAND_AND_QUOTE(QSEE_CONNECTOR_SERVICE_NAME) EXPAND_AND_QUOTE(Cb);
  }

  IMPLEMENT_META_INTERFACE(QSEEConnectorServiceCb, IQSEEConnectorServiceCb::getServiceName());
}; // namespace android
