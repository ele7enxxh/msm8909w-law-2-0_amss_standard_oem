/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
*/

#pragma once

#include <binder/IInterface.h>
#include <stdint.h>
#include <string>
#include <utility>

#define QUOTE(x) #x
#define EXPAND_AND_QUOTE(str) QUOTE(str)

namespace android {

  class IQSEEConnectorServiceCb: public IInterface {
    public:
      static const char* getServiceName();

      enum {
        NOTIFY_CALLBACK = IBinder::FIRST_CALL_TRANSACTION,
      };

      virtual void notifyCallback(int32_t event) = 0;

      DECLARE_META_INTERFACE(QSEEConnectorServiceCb);
  };

  class IQSEEConnectorService : public IInterface {
    public:
      static const char* getServiceName();

      enum {
        LOAD = IBinder::FIRST_CALL_TRANSACTION,
        UNLOAD,
        SEND_COMMAND,
      };

      virtual status_t load(const sp<IQSEEConnectorServiceCb> &notifier, uint32_t size, uint32_t &handle) = 0;
      virtual status_t unload(uint32_t handle) = 0;
      virtual status_t sendCommand(uint32_t handle, uint8_t const * const cmd, uint32_t cmdLen, uint8_t * const rsp, uint32_t rspLen) = 0;

      DECLARE_META_INTERFACE(QSEEConnectorService);
  };
}; // namespace android
