/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#define MAIN_C
#include <android/log.h>
#include <QSEEConnectorService.h>
#include <stdlib.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <string>
#include <unistd.h>

#include <IQSEEConnectorService.cpp>
#include <BpQSEEConnectorService.cpp>

using namespace android;

#undef LOG_TAG
#define LOG_TAG "qsee_daemon_client_test: "

#define ENTER ALOGD("%s+",__func__)
#define EXIT  ALOGD("%s-",__func__)
#define EXITV(a) ALOGD("%s-: %x", __func__, (a)); return (a)

// Helper function to get a hold of the service.
sp<IQSEEConnectorService> getServ(std::string const &name) {
  ENTER;
  sp<IQSEEConnectorService> proxy = 0;
  do {
    sp<IServiceManager> sm = defaultServiceManager();
    if (sm == 0) {
      ALOGD("NULL sm");
      break;
    }
    sp<IBinder> binder = sm->getService(String16(name.c_str()));
    if (binder == 0) {
      ALOGD("NULL binder for service %s", name.c_str());
      break;
    }
    proxy = interface_cast<IQSEEConnectorService>(binder);
    if (proxy == 0) {
      ALOGD("NULL proxy");
      break;
    }
  } while (0);
  ALOGD("Returning proxy: %s", proxy->getServiceName());
  EXIT;
  return proxy;
}

void test(sp<IQSEEConnectorService> &proxy) {
  ENTER;
  status_t rv = OK;
  uint32_t cmdLen = 0x89C0;
  uint32_t rspLen = 0x10140;
  uint32_t size = cmdLen + rspLen;
  uint32_t handle = 0;
  uint8_t cmd[cmdLen] = {0};
  uint8_t rsp[rspLen] = {0};
  uint32_t *cmdPtr = (uint32_t*)cmd;
  uint32_t *rspPtr = (uint32_t*)rsp;
  cmdPtr[0] = 1; // cmdId
  do {
    sp<QSEEConnectorServiceCb> cb = new QSEEConnectorServiceCb();

    if (cb == NULL) break;

    rv = proxy->load(cb, size, handle);
    ALOGD("load: rv=%d, size=%u, handle=%x", rv, size, handle);
    if (rv != 0) break;

    rv = proxy->sendCommand(handle, cmd, cmdLen, rsp, rspLen);
    ALOGD("sendCommand: rv=%d, cmdId=%x, rspId=%x, retCode=%x, version=%x", rv, cmdPtr[0], rspPtr[0], rspPtr[1], rspPtr[2]);
    if (rv != 0) break;

    sleep(60);
    rv = proxy->unload(handle);
    ALOGD("unload: rv = %d", rv);
  } while (0);
  EXIT;
}

int main(int argc, char **argv) {

  ENTER;
  ALOGD("We're the client, connecting to service %s", IQSEEConnectorService::getServiceName());

  sp<IQSEEConnectorService> proxy = getServ(IQSEEConnectorService::getServiceName());
  if (proxy != 0) {
    test(proxy);
  } else {
    ALOGE("Null proxy");
  }

  EXITV(0);
}
