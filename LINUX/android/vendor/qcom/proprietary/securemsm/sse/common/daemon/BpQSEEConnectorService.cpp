/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <android/log.h>
#include <binder/Parcel.h>
#include "BpQSEEConnectorService.h"

#undef LOG_TAG
#define LOG_TAG EXPAND_AND_QUOTE(QSEE_CONNECTOR_SERVICE_NAME) EXPAND_AND_QUOTE(-client)

namespace android {

  status_t BpQSEEConnectorService::load(const sp<IQSEEConnectorServiceCb> &notifier, uint32_t size, uint32_t &handle) {
    Parcel data, reply;
    status_t rv = NO_ERROR;
    data.writeInterfaceToken(BpQSEEConnectorService::getInterfaceDescriptor());
    data.writeStrongBinder(asBinder(notifier));
    data.writeInt32(size);

    // binder call
    rv = remote()->transact(LOAD, data, &reply);
    if (rv != NO_ERROR) {
      ALOGE("Couldn't contact remote: %d", rv);
      return rv;
    }
    int32_t err = reply.readExceptionCode();
    if (err < 0) {
      ALOGE("remote exception: %d", err);
      return err;
    }
    handle = reply.readInt32();
    return rv;
  }

  status_t BpQSEEConnectorService::unload(uint32_t handle) {
    Parcel data, reply;
    status_t rv = NO_ERROR;
    data.writeInterfaceToken(BpQSEEConnectorService::getInterfaceDescriptor());
    data.writeInt32(handle);

    // binder call
    rv = remote()->transact(UNLOAD, data, &reply);
    if (rv != NO_ERROR) {
      ALOGE("Couldn't contact remote: %d", rv);
      return rv;
    }
    int32_t err = reply.readExceptionCode();
    if (err < 0) {
      ALOGE("remote exception: %d", err);
      return err;
    }
    return rv;
  }

  status_t BpQSEEConnectorService::sendCommand(uint32_t handle, uint8_t const * const cmd, uint32_t cmdLen, uint8_t * const rsp, uint32_t rspLen) {
    Parcel data, reply;
    status_t rv = NO_ERROR;
    do {
      if ((!cmd) || (!rsp) || (cmdLen == 0) || (rspLen == 0)) {
        ALOGE("NULL params");
        rv = BAD_VALUE;
        break;
      }
      data.writeInterfaceToken(BpQSEEConnectorService::getInterfaceDescriptor());
      data.writeInt32(handle);
      data.writeInt32(cmdLen);
      data.writeInt32(rspLen);
      data.write(cmd, cmdLen);
      // binder call
      rv = remote()->transact(SEND_COMMAND, data, &reply);
      if (rv != NO_ERROR) {
        ALOGE("Couldn't contact remote: %d", rv);
        break;
      }
      int32_t err = reply.readExceptionCode();
      if (err < 0) {
        ALOGE("remote exception: %d", err);
        rv = err;
        break;
      }
      reply.read(rsp, rspLen);
      ALOGD("Response read: %u bytes", rspLen);
    } while (0);
    return rv;
  }

  BpQSEEConnectorService::BpQSEEConnectorService(const sp<IBinder>& impl) : BpInterface<IQSEEConnectorService>(impl){}

  status_t BnQSEEConnectorServiceCb::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
  {
    status_t ret = UNKNOWN_ERROR;

    switch(code) {

      case NOTIFY_CALLBACK: {
          CHECK_INTERFACE(IQSEEConnectorServiceCb, data, reply);
          int32_t event = data.readInt32();
          notifyCallback(event);
          reply->writeNoException();
        }
        break;

      default:
        ALOGD("Unknown binder command ID: %d", code);
        return BBinder::onTransact(code, data, reply, flags);
    }
    return ret;
  }

}; // namespace android
