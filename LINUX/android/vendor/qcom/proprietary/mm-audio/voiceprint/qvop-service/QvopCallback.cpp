/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "QvopCallback.h"

#include <utils/Log.h>
#include <stdint.h>
#include <sys/types.h>
#include <vector>
#include <string>

namespace android {

class BpQvopCallback : public BpInterface<IQvopCallback> {
public:
  BpQvopCallback(const sp<IBinder>& impl)
    : BpInterface<IQvopCallback>(impl) {
  }

  // error
  virtual void onError(int32_t error) {
    logfile_print("%s: entry\n", QVOP_FN);
    Parcel data, reply;
    data.writeInterfaceToken(IQvopCallback::getInterfaceDescriptor());
    data.writeInt32(error);
    remote()->transact(QVOP_RES_ERROR, data, &reply);
  }

  // On a match or match any successful search
  virtual void onMatchFound(const char* user_id, const char* identifier, int32_t index, double sentence_score, double ver_score, float spoofScore) {
    logfile_print("%s: entry\n", QVOP_FN);
    Parcel data, reply;
    data.writeInterfaceToken(IQvopCallback::getInterfaceDescriptor());
    data.writeCString(user_id);
    data.writeCString(identifier);
    data.writeInt32(index);
    data.writeDouble(sentence_score);
    data.writeDouble(ver_score);
    data.writeFloat(spoofScore);
    remote()->transact(QVOP_RES_MATCH_FOUND, data, &reply);
  }

  // On a match or match any failed
  virtual void onMatchNotFound(int32_t error, int32_t index, double sentence_score, double ver_score, float spoofScore)  {
    logfile_print("%s: entry\n", QVOP_FN);
    Parcel data, reply;
    data.writeInterfaceToken(IQvopCallback::getInterfaceDescriptor());
    data.writeInt32(error);
    data.writeInt32(index);
    data.writeDouble(sentence_score);
    data.writeDouble(ver_score);
    data.writeFloat(spoofScore);
    remote()->transact(QVOP_RES_MATCH_NOT_FOUND, data, &reply);
  }

  // Enrollment was successful
  virtual void onEnrollStatus(int32_t status) {
    logfile_print("%s: entry\n", QVOP_FN);
    Parcel data, reply;
    data.writeInterfaceToken(IQvopCallback::getInterfaceDescriptor());
    data.writeInt32(status);
    remote()->transact(QVOP_RES_ENROLL, data, &reply);
  }


  // Delete Status - user/identifier, depending on request
  virtual void onDeleteStatus(int32_t status, const char* user_id, const char* identifier) {
    logfile_print("%s: entry\n", QVOP_FN);
    Parcel data, reply;
    data.writeInterfaceToken(IQvopCallback::getInterfaceDescriptor());
    data.writeInt32(status);
    data.writeCString(user_id);
    data.writeCString(identifier);
    remote()->transact(QVOP_RES_DELETE_ID, data, &reply);
  }

  // Delete All status - identifier
  virtual void onDeleteAllStatus(int32_t status) {
    logfile_print("%s: entry\n", QVOP_FN);
    Parcel data, reply;
    data.writeInterfaceToken(IQvopCallback::getInterfaceDescriptor());
    data.writeInt32(status);
    remote()->transact(QVOP_RES_DELETE_ALL, data, &reply);
  }

  // Version info
  virtual void onVersionInfo(long major, long minor) {
    logfile_print("%s: entry\n", QVOP_FN);
    Parcel data, reply;
    data.writeInterfaceToken(IQvopCallback::getInterfaceDescriptor());
    data.writeInt64(major);
    data.writeInt64(minor);
    remote()->transact(QVOP_RES_VERSION, data, &reply);
  }

  // Config setting success
  virtual void onConfigStatus(int32_t status) {
    logfile_print("%s: entry\n", QVOP_FN);
    Parcel data, reply;
    data.writeInterfaceToken(IQvopCallback::getInterfaceDescriptor());
    data.writeInt32(status);
    remote()->transact(QVOP_RES_CONFIG, data, &reply);
  }

  // Rename successful
  virtual void onRenameStatus(int32_t status) {
    logfile_print("%s: entry\n", QVOP_FN);
    Parcel data, reply;
    data.writeInterfaceToken(IQvopCallback::getInterfaceDescriptor());
    data.writeInt32(status);
    remote()->transact(QVOP_RES_RENAME, data, &reply);
  }
};

status_t BnQvopCallback::onTransact(
  uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags) {
  switch (code) {
  case QVOP_RES_ERROR:
  {
    logfile_print("%s: %s\n", QVOP_FN, "QVOP_RES_ERROR");
    CHECK_INTERFACE(IQvopCallback, data, reply);
    int32_t error = data.readInt32();
    onError(error);
    return NO_ERROR;
  }
  break;
  case QVOP_RES_ENROLL:
  {
    logfile_print("%s: %s\n", QVOP_FN, "QVOP_RES_ENROLL");
    CHECK_INTERFACE(IQvopCallback, data, reply);
    int32_t status = data.readInt32();
    onEnrollStatus(status);
    return NO_ERROR;
  }
  break;
  case QVOP_RES_MATCH_FOUND:
  {
    logfile_print("%s: %s\n", QVOP_FN, "QVOP_RES_MATCH_FOUND");
    CHECK_INTERFACE(IQvopCallback, data, reply);
    char const* user = data.readCString();
    char const* id = data.readCString();
    int32_t index = data.readInt32();
    double sentence_score = data.readDouble();
    double verification_score = data.readDouble();
    float spoofScore = data.readFloat();

#ifdef QVOP_DEBUG
    logfile_print("%s: onMatchFound: index=%d, sentence_score=%lf, verification_score=%lf, spoofScore=%f",
                  QVOP_FN, index, sentence_score, verification_score, spoofScore);
    onMatchFound(user, id, index, sentence_score, verification_score, spoofScore);
#else
    onMatchFound(user, id, 0, 0, 0, 0);
#endif

    return NO_ERROR;
  }
  break;
  case QVOP_RES_MATCH_NOT_FOUND:
  {
    logfile_print("%s: %s\n", QVOP_FN, "QVOP_RES_MATCH_NOT_FOUND");
    CHECK_INTERFACE(IQvopCallback, data, reply);
    int32_t error = data.readInt32();
    int32_t index = data.readInt32();
    double sentence_score = data.readDouble();
    double verification_score = data.readDouble();
    float spoofScore = data.readFloat();



#ifdef QVOP_DEBUG
    logfile_print("onMatchNotFound: error=%d, index=%d, sentence_score=%lf, verification_score=%lf, spoofScore=%f",
                  error, index, sentence_score, verification_score, spoofScore);
    onMatchNotFound(error, index, sentence_score, verification_score, spoofScore);
#else
    onMatchNotFound(error, index, 0, 0, 0);
#endif

    return NO_ERROR;
  }
  break;
  case QVOP_RES_DELETE_USER:
  {
    logfile_print("%s: %s\n", QVOP_FN, "QVOP_RES_DELETE_USER");
    CHECK_INTERFACE(IQvopCallback, data, reply);
    int32_t status = data.readInt32();
    char const* user = data.readCString();
    onDeleteStatus(status, user, 0);
    return NO_ERROR;
  }
  break;
  case QVOP_RES_DELETE_ID:
  {
    logfile_print("%s: %s\n", QVOP_FN, "QVOP_RES_DELETE_ID");
    CHECK_INTERFACE(IQvopCallback, data, reply);
    int32_t status = data.readInt32();
    char const* user = data.readCString();
    char const* id = data.readCString();
    onDeleteStatus(status, user, id);
    return NO_ERROR;
  }
  break;
  case QVOP_RES_DELETE_ALL:
  {
    logfile_print("%s: %s\n", QVOP_FN, "QVOP_RES_DELETE_ALL");
    CHECK_INTERFACE(IQvopCallback, data, reply);
    int32_t status = data.readInt32();
    onDeleteAllStatus(status);
    return NO_ERROR;
  }
  break;
  case QVOP_RES_CONFIG:
  {
    logfile_print("%s: %s\n", QVOP_FN, "QVOP_RES_ERROR");
    CHECK_INTERFACE(IQvopCallback, data, reply);
    int32_t status = data.readInt32();
    onConfigStatus(status);
    return NO_ERROR;
  }
  break;
  case QVOP_RES_VERSION:
  {
    logfile_print("%s: %s\n", QVOP_FN, "QVOP_RES_ERROR");
    CHECK_INTERFACE(IQvopCallback, data, reply);
    int64_t major = data.readInt64();
    int64_t minor = data.readInt64();
    onVersionInfo(major, minor);
    return NO_ERROR;
  }
  break;
  case QVOP_RES_RENAME:
  {
    logfile_print("%s: %s\n", QVOP_FN, "QVOP_RES_ERROR");
    CHECK_INTERFACE(IQvopCallback, data, reply);
    int32_t status = data.readInt32();
    onRenameStatus(status);
    return NO_ERROR;
  }
  break;
  default:
    return BBinder::onTransact(code, data, reply, flags);
  }
}

IMPLEMENT_META_INTERFACE(QvopCallback, "android.apps.IQvopCallback");

}; // namespace android
