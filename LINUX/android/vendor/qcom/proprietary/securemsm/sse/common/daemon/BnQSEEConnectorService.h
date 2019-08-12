/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <binder/Parcel.h>
#include "IQSEEConnectorService.h"

namespace android {
  class BnQSEEConnectorService: public BnInterface<IQSEEConnectorService> {

    public:
      virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);

  };

  class BpQSEEConnectorServiceCb: public BpInterface<IQSEEConnectorServiceCb> {
    public:
      BpQSEEConnectorServiceCb(const sp<IBinder>& impl);

      virtual void notifyCallback(int32_t event);
  };
}; // namespace android

