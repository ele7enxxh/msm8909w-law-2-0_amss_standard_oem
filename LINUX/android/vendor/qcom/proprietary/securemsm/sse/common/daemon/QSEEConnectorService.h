/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <map>

#include <binder/Parcel.h>
#include <binder/BinderService.h>
#include "BnQSEEConnectorService.h"
#include "BpQSEEConnectorService.h"

namespace android {
  class QSEEConnectorService: public BinderService<QSEEConnectorService>,  public BnQSEEConnectorService
  {
    friend class BinderService<QSEEConnectorService>;

    private:

      class ClientDiedNotifier: public IBinder::DeathRecipient {
        public:
          ClientDiedNotifier(uint32_t handle, void *service) {
            mHandle = handle;
            mService = service;
          }
        protected:
          // Invoked by binder when client crash. Implemented by Server
          virtual void binderDied(const wp<IBinder> &who);

        private:
          uint32_t mHandle;
          // QSEEConnectorService instance
          void *mService;
      };

      void* l_QSEEComHandle;
      uint32_t l_size;
      uint8_t *buffer;
      uint32_t l_handle;
      uint64_t counter;
      sp<IBinder> l_serviceCb;
      sp<QSEEConnectorService::ClientDiedNotifier> l_deathNotifier;
      pthread_rwlock_t lock;

    public:
      QSEEConnectorService():l_QSEEComHandle(NULL),l_size(0),buffer(NULL),l_handle(0),counter(0),l_serviceCb(NULL),l_deathNotifier(NULL) {
        pthread_rwlock_init(&lock, NULL);
      };
      virtual ~QSEEConnectorService() {};
      virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);

      virtual status_t load(const sp<IQSEEConnectorServiceCb> &notifier, uint32_t size, uint32_t &handle);
      virtual status_t unload(uint32_t handle);
      virtual status_t sendCommand(uint32_t handle, uint8_t const * const cmd, uint32_t cmdLen, uint8_t * const rsp, uint32_t rspLen);

  };

  class QSEEConnectorServiceCb: public BnQSEEConnectorServiceCb {
    public:
      virtual void notifyCallback(int32_t event){};
  };
}; // namespace android

