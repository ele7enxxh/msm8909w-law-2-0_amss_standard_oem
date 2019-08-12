/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "QSEEConnectorService.h"
#include "QSEEComAPI.h"

#undef LOG_TAG
#define LOG_TAG EXPAND_AND_QUOTE(QSEE_CONNECTOR_SERVICE_NAME) EXPAND_AND_QUOTE(-daemon)

#define LOCKW() \
      rv = pthread_rwlock_wrlock(&lock); \
      if (rv) { \
        ALOGE("Error W-locking rwlock: %d (%s)", rv, strerror(rv)); \
        break; \
      } \
      locked = 1

#define LOCKR() \
      rv = pthread_rwlock_rdlock(&lock); \
      if (rv) { \
        ALOGE("Error R-locking rwlock: %d (%s)", rv, strerror(rv)); \
        break; \
      } \
      locked = 1

#define UNLOCK() \
      if (locked) { \
        pthread_rwlock_unlock(&lock); \
        locked = 0; \
      }


namespace android {

  status_t QSEEConnectorService::load(const sp<IQSEEConnectorServiceCb> &notifier, uint32_t size, uint32_t &handle) {
    status_t ret = BAD_VALUE;
    int rv = 0;
    int locked = 0;
    ALOGD("load");
    do {
      LOCKW();
      if (counter > 0) {
        ALOGD("Service already in use");
        counter++;
        handle = l_handle;
        ret = NO_ERROR;
        break;
      }

      rv = QSEECom_start_app((struct QSEECom_handle**)&l_QSEEComHandle, EXPAND_AND_QUOTE(SECURE_APP_PATH), EXPAND_AND_QUOTE(SECURE_APP_NAME), size);
      if (rv) {
        ALOGE("Loading app [%s] failed\n", EXPAND_AND_QUOTE(SECURE_APP_NAME));
        size = 0;
        buffer = NULL;
        ret = BAD_VALUE;
      } else {
        ALOGD("Loading app [%s] succeeded\n", EXPAND_AND_QUOTE(SECURE_APP_NAME));
        buffer = ((struct QSEECom_handle*)l_QSEEComHandle)->ion_sbuffer;
        l_handle = rand();
        handle = l_handle;
        l_size = size;
        l_serviceCb = asBinder(notifier);
        l_deathNotifier = new ClientDiedNotifier(handle, this);
        counter = 1;
        if (l_serviceCb != 0) {
          l_serviceCb->linkToDeath(l_deathNotifier);
        } else {
          ALOGE("linkToDeath failed");
        }
        ret = NO_ERROR;
      }
    } while (0);
    UNLOCK();
    return ret;
  }

  status_t QSEEConnectorService::unload(uint32_t handle) {
    status_t ret = BAD_VALUE;
    int rv = 0;
    int locked = 0;
    ALOGD("unload");
    do {
      LOCKW();
      if ((l_handle == 0) || (counter == 0)) {
        ALOGE("Service not in use");
        ret = BAD_VALUE;
        break;
      }

      if (l_handle != handle) {
        ALOGE("Wrong handle");
        ret = BAD_VALUE;
        break;
      }

      if (l_QSEEComHandle == NULL) {
        ALOGW("App [%s] not loaded", EXPAND_AND_QUOTE(SECURE_APP_NAME));
        break;
      }

      if (counter > 1)  {
        ALOGD("Decrement counter");
        counter--;
        ret = NO_ERROR;
        break;
      }

      rv = QSEECom_shutdown_app((struct QSEECom_handle**)&l_QSEEComHandle);
      if (rv) {
        ALOGE("Failed to unload app [%s]: %d (%s)\n", EXPAND_AND_QUOTE(SECURE_APP_NAME), rv, strerror(errno));
        ret = BAD_VALUE;
      } else {
        l_QSEEComHandle = NULL;
        buffer = NULL;
        l_handle = 0;
        l_size = 0;
        counter = 0;
        if (l_serviceCb != 0) {
          l_serviceCb->unlinkToDeath(l_deathNotifier);
        }
        l_deathNotifier = NULL;
        ret = NO_ERROR;
      }
    } while (0);
    UNLOCK();
    return ret;
  }

  status_t QSEEConnectorService::sendCommand(uint32_t handle, uint8_t const * const cmd, uint32_t cmdLen, uint8_t * const rsp, uint32_t rspLen) {
    status_t ret = BAD_VALUE;
    int rv = 0;
    int locked = 0;
    ALOGD("sendCommand");
    do {
      uint8_t *cmdIon = NULL;
      uint8_t *rspIon = NULL;
      int rv = 0;
      LOCKR();
      if (l_handle == 0) {
        ALOGE("Service not in use");
        ret = BAD_VALUE;
        break;
      }
      if (l_handle != handle) {
        ALOGE("Wrong handle");
        ret = BAD_VALUE;
        break;
      }
      if (cmdLen + rspLen > l_size) {
        ALOGE("Bad cmdLen/rspLen: %u + %u > %u", cmdLen, rspLen, l_size);
        ret = BAD_VALUE;
        break;
      }
      if (cmdLen & QSEECOM_ALIGN_MASK) {
        ALOGE("Command buffer not aligned: %u", cmdLen);
        ret = BAD_VALUE;
        break;
      }
      if (rspLen & QSEECOM_ALIGN_MASK) {
        ALOGE("Response buffer not aligned: %u", rspLen);
        ret = BAD_VALUE;
        break;
      }
      cmdIon = buffer;
      memcpy(cmdIon, cmd, cmdLen);
      rspIon = buffer + cmdLen;

      QSEECom_set_bandwidth((struct QSEECom_handle*)l_QSEEComHandle, 1);

      rv = QSEECom_send_cmd((struct QSEECom_handle*)l_QSEEComHandle, cmdIon, cmdLen, rspIon, rspLen);

      QSEECom_set_bandwidth((struct QSEECom_handle*)l_QSEEComHandle, 0);

      if (rv) {
        ALOGE("Error in QSEECom_send_cmd: %d (%s)", rv, strerror(errno));
        ret = BAD_VALUE;
        break;
      }
      memcpy(rsp, rspIon, rspLen);
      ret = NO_ERROR;
    } while (0);
    UNLOCK();
    return ret;
  }

  status_t QSEEConnectorService::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
  {
    return BnQSEEConnectorService::onTransact(code, data, reply, flags);
  }

  void QSEEConnectorService::ClientDiedNotifier::binderDied(const wp<IBinder> &who) {
    const wp<IBinder> unused = who;
    QSEEConnectorService *service = reinterpret_cast <QSEEConnectorService *>(mService);

    ALOGW("Client died");

    service->unload(mHandle);
}

}; // namespace android
