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

using namespace android;

#undef LOG_TAG
#define LOG_TAG EXPAND_AND_QUOTE(QSEE_CONNECTOR_SERVICE_NAME) EXPAND_AND_QUOTE(-daemon)
#define ENTER ALOGD("%s+",__func__)
#define EXIT  ALOGD("%s-",__func__)
#define EXITV(a) ALOGD("%s-: %x", __func__, (a)); return (a)

int main(int argc, char **argv) {

  ALOGD("Starting service: %s", QSEEConnectorService::getServiceName());

  QSEEConnectorService::instantiate();
  android::ProcessState::self()->startThreadPool();
  ALOGD("QSEEConnector service is now ready");
  IPCThreadState::self()->joinThreadPool();
  ALOGD("QSEEConnector service thread joined");

  return 0;
}
