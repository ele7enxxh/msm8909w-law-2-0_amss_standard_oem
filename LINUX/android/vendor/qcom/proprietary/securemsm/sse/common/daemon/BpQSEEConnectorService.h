/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <binder/Parcel.h>
#include "IQSEEConnectorService.h"

// this is the client
namespace android {
  class BpQSEEConnectorService: public BpInterface<IQSEEConnectorService> {
    public:
      BpQSEEConnectorService(const sp<IBinder>& impl);

      virtual status_t load(const sp<IQSEEConnectorServiceCb> &notifier, uint32_t size, uint32_t &handle);
      virtual status_t unload(uint32_t handle);
      virtual status_t sendCommand(uint32_t handle, uint8_t const * const cmd, uint32_t cmdLen, uint8_t * const rsp, uint32_t rspLen);
  };

  class BnQSEEConnectorServiceCb: public BnInterface<IQSEEConnectorServiceCb> {
    public:
      virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);
  };
}; // namespace android

