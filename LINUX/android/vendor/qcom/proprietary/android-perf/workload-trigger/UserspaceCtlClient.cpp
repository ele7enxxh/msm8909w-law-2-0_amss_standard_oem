/******************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------

  @file   UserspaceCtlClient.cpp
  @brief  Client side support for feedback driven core control
*******************************************************************************/

#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>
#include <utils/Trace.h>
#include <cutils/properties.h>
#include "UserspaceCtl.h"
#include "UserspaceCtlClient.h"

#undef LOG_TAG
#define LOG_TAG "FDCC-CTL-CLIENT"

using namespace android;

namespace userspaceCtl {

static pthread_mutex_t getInstanceMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t binderMutex = PTHREAD_MUTEX_INITIALIZER;

// Client
UserspaceCtlClient* UserspaceCtlClient::sUserspaceCtlClient = NULL;

void BpUserspaceCtl::Dispatch(int32_t data) {
    QLOGI("Client dispatching the data %d", data);
    Parcel outData, reply;
    outData.writeInterfaceToken(IUserspaceCtl::getInterfaceDescriptor());
    outData.writeInt32(data);
    remote()->transact(DISP_HAL, outData, &reply, IBinder::FLAG_ONEWAY);
}

IMPLEMENT_META_INTERFACE(UserspaceCtl, USERSPACE_CTL_SERVICE);

UserspaceCtlClient *UserspaceCtlClient::getInstance() {
    pthread_mutex_lock(&getInstanceMutex);
    if (!sUserspaceCtlClient) {
        sUserspaceCtlClient = new UserspaceCtlClient();
    }
    pthread_mutex_unlock(&getInstanceMutex);
    return sUserspaceCtlClient;
}

void UserspaceCtlClient::SetBinder(sp<IUserspaceCtl> UserspaceCtlBinder) {
    pthread_mutex_lock(&binderMutex);
    mUserspaceCtlBinder = UserspaceCtlBinder;
    pthread_mutex_unlock(&binderMutex);
}

sp<IUserspaceCtl> UserspaceCtlClient::GetBinder() {
    return mUserspaceCtlBinder;
}

UserspaceCtlClient::UserspaceCtlClient() {
    char buf[PROPERTY_VALUE_MAX];
    workloadTriggerEnabled = false;

    //check for feature enablement
    if (property_get("ro.qti.userspacectl", buf, NULL) <= 0) {
        QLOGE("%s feedback core_ctl property not found\n", __func__);
        return;
    }
    if (!strncmp(buf, "true", strlen("true"))) {
        workloadTriggerEnabled = true;
        deathRecipient = new ClientDeathRecipient();
    }
    QLOGI("Support for feedback driven core_ctl present: %s", (workloadTriggerEnabled ? "true" : "false"));
}

UserspaceCtlClient::~UserspaceCtlClient() {
}

//Function to notify binder death
void ClientDeathRecipient::binderDied(const android::wp<IBinder>& who __attribute__((unused))) {
    UserspaceCtlClient *client = UserspaceCtlClient::getInstance();
    if (client != NULL) {
        client->SetBinder(NULL);
    }
}

//Function to notify daemon death
ClientDeathRecipient::~ClientDeathRecipient() {
    UserspaceCtlClient *client = UserspaceCtlClient::getInstance();
    if (client != NULL) {
        client->SetBinder(NULL);
    }
}

// Helper function to check and get userspace ctl service.
sp<IUserspaceCtl> getUserspaceCtlService() {
    sp<IUserspaceCtl> UserspaceCtlBinder;
    UserspaceCtlClient *client = UserspaceCtlClient::getInstance();

    if (NULL == client) {
        QLOGI("Error: Did not get client instance");
        return NULL;
    }

    if (!client->workloadTriggerEnabled) {
        //FDCC is not enabled
        QLOGI("FDCC is not enabled");
        return NULL;
    }

    if ((client->GetBinder() != NULL)) {
        return client->GetBinder();
    }

    sp<IServiceManager> sm = defaultServiceManager();
    if (sm == NULL) {
        QLOGE("Didn't get service manager instance");
        return NULL;
    }
    sp<IBinder> binder = sm->checkService(String16(USERSPACE_CTL_SERVICE));
    if (binder == NULL) {
        QLOGI("Didn't get binder instance");
        return NULL;
    }
    UserspaceCtlBinder = interface_cast<IUserspaceCtl>(binder);

    binder->linkToDeath(client->deathRecipient);

    client->SetBinder(UserspaceCtlBinder);
    return UserspaceCtlBinder;
}

}; // namespace userspaceCtl

