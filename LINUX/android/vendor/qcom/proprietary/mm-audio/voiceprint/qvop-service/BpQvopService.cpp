/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "logfile.h"
#include "BpQvopService.h"

#include <map>
#include <string>


namespace android {

// map binder transaction codes to their string representation
static const std::pair<int32_t, std::string> binderCodeData[] = {
  std::make_pair(QVOP_REQ_ENROLL, "QVOP_REQ_ENROLL"),
  std::make_pair(QVOP_REQ_MATCH, "QVOP_REQ_MATCH"),
  std::make_pair(QVOP_REQ_MATCH_ANY, "QVOP_REQ_MATCH_ANY"),
  std::make_pair(QVOP_REQ_DELETE_USER, "QVOP_REQ_DELETE_USER"),
  std::make_pair(QVOP_REQ_DELETE_ALL, "QVOP_REQ_DELETE_ALL"),
  std::make_pair(QVOP_REQ_CANCEL, "QVOP_REQ_CANCEL"),
  std::make_pair(QVOP_REQ_CONFIG, "QVOP_REQ_CONFIG"),
  std::make_pair(QVOP_REQ_ADD_FRAME, "QVOP_REQ_ADD_FRAME"),
  std::make_pair(QVOP_REQ_ADD_FRAME_WITH_ID, "QVOP_REQ_ADD_FRAME_WITH_ID"),
  std::make_pair(QVOP_REQ_DELETE_USER_WITH_ID, "QVOP_REQ_DELETE_USER_WITH_ID"),
  std::make_pair(QVOP_REQ_MATCH_ANY_SECURE, "QVOP_REQ_MATCH_ANY_SECURE"),
  std::make_pair(QVOP_REQ_MATCH_ANY_KEYPHRASE, "QVOP_REQ_MATCH_ANY_KEYPHRASE"),
  std::make_pair(QVOP_REQ_MATCH_SECURE, "QVOP_REQ_MATCH_SECURE"),
  std::make_pair(QVOP_REQ_MATCH_WITH_ID, "QVOP_REQ_MATCH_WITH_ID"),
  std::make_pair(QVOP_REQ_START_AUDIO_CAPTURE, "QVOP_REQ_START_AUDIO_CAPTURE"),
  std::make_pair(QVOP_REQ_STOP_AUDIO_CAPTURE, "QVOP_REQ_STOP_AUDIO_CAPTURE"),
  std::make_pair(QVOP_REQ_ENROLL_CAPTURE_START, "QVOP_REQ_ENROLL_CAPTURE_START"),
  std::make_pair(QVOP_REQ_ENROLL_CAPTURE_COMPLETE, "QVOP_REQ_ENROLL_CAPTURE_COMPLETE"),
  std::make_pair(QVOP_REQ_ENROLL_COMMIT, "QVOP_REQ_ENROLL_COMMIT"),
  std::make_pair(QVOP_REQ_MATCH_CAPTURE_COMPLETE, "QVOP_REQ_MATCH_CAPTURE_COMPLETE"),
  std::make_pair(QVOP_REQ_RENAME, "QVOP_REQ_RENAME"),
  std::make_pair(QVOP_REQ_SET_THRESHOLD, "QVOP_REQ_SET_THRESHOLD")
};


static const std::map<uint32_t, std::string> binderCodes(binderCodeData,
    binderCodeData + sizeof binderCodeData / sizeof *binderCodeData);

QvopStatus BpQvopService::cancel() {
  logfile_print("%s: entry\n", QVOP_FN);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());

  // binder call
  binderTransact(QVOP_REQ_CANCEL, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::getVersion(sp<IQvopCallback> const& callback) {
  logfile_print("%s: entry\n", QVOP_FN);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));

  // binder call
  binderTransact(QVOP_REQ_VERSION, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::enroll(sp<IQvopCallback> const& callback, const char* user_id, const char* identifier, const char* phrase) {
  logfile_print("%s: entry user=%s,id=%s,phrase=%s\n", QVOP_FN, user_id, identifier, phrase);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeCString(user_id);
  data.writeCString(identifier);
  data.writeCString(phrase);

  // binder call
  binderTransact(QVOP_REQ_ENROLL, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::setThreshold(sp<IQvopCallback> const& callback, const char* user_id, const char* identifier, int32_t threshold) {
  logfile_print("%s: entry user=%s,id=%s,threshold=%d\n", QVOP_FN, user_id, identifier, threshold);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeCString(user_id);
  data.writeCString(identifier);
  data.writeInt32(threshold);

  // binder call
  binderTransact(QVOP_REQ_SET_THRESHOLD, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}


QvopStatus BpQvopService::rename(sp<IQvopCallback> const& callback, const char* user_id, const char* old_id, const char* new_id) {
  logfile_print("%s: entry user=%s,old_id=%s,new_id=%s\n", QVOP_FN, user_id, old_id, new_id);
  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeCString(user_id);
  data.writeCString(old_id);
  data.writeCString(new_id);

  // binder call
  binderTransact(QVOP_REQ_RENAME, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}


QvopStatus BpQvopService::match(sp<IQvopCallback> const& callback, const char* user_id) {
  logfile_print("%s: entry user=%s\n", QVOP_FN, user_id);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeCString(user_id);

  // binder call
  binderTransact(QVOP_REQ_MATCH, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::matchWithId(sp<IQvopCallback> const& callback, const char* user_id, const char* identifier) {
  logfile_print("%s: entry user=%s,id=%s\n", QVOP_FN, user_id, identifier);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeCString(user_id);
  data.writeCString(identifier);

  // binder call
  binderTransact(QVOP_REQ_MATCH_WITH_ID, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::matchSecure(sp<IQvopCallback> const& callback, const char* user_id) {
  logfile_print("%s: entry user=%s\n", QVOP_FN, user_id);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeCString(user_id);

  // binder call
  binderTransact(QVOP_REQ_MATCH_SECURE, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::matchAny(sp<IQvopCallback> const& callback, const char* user_id) {
  logfile_print("%s: entry user=%s\n", QVOP_FN, user_id);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeCString(user_id);

  // binder call
  binderTransact(QVOP_REQ_MATCH_ANY, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::matchAnyKeyPhrase(sp<IQvopCallback> const& callback, const char* user_id, const char* phrase)
{
  logfile_print("%s: entry user=%s,phrase=%s\n", QVOP_FN, user_id, phrase);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeCString(user_id);
  data.writeCString(phrase);

  // binder call
  binderTransact(QVOP_REQ_MATCH_ANY_KEYPHRASE, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::matchAnySecure(sp<IQvopCallback> const& callback, const char* user_id)
{
  logfile_print("%s: entry user=%s\n", QVOP_FN, user_id);
  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeCString(user_id);

  // binder call
  binderTransact(QVOP_REQ_MATCH_ANY_SECURE, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::deleteUser(sp<IQvopCallback> const& callback, const char* user_id) {
  logfile_print("%s: entry user=%s\n", QVOP_FN, user_id);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeCString(user_id);

  // binder call
  binderTransact(QVOP_REQ_DELETE_USER, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::deleteWithId(sp<IQvopCallback> const& callback, const char* user_id, const char* identifier) {
  logfile_print("%s: entry user=%s,id=%s\n", QVOP_FN, user_id, identifier);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeCString(user_id);
  data.writeCString(identifier);

  // binder call

  // todo check return value of binderTransact and handle for all pton
  binderTransact(QVOP_REQ_DELETE_USER_WITH_ID, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::deleteAll(sp<IQvopCallback> const& callback, const char* user_id) {
  logfile_print("%s: entry %s\n", QVOP_FN, user_id);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeCString(user_id);

  // binder call
  binderTransact(QVOP_REQ_DELETE_ALL, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::processFrame(sp<IQvopCallback> const& callback, int64_t timestamp, int32_t bufferlen, int16_t const* buffer) {
  logfile_print("%s: entry, timestamp=%d, len=%d\n", QVOP_FN, timestamp, bufferlen);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeInt64(timestamp);
  data.writeInt32(bufferlen);
  //#ifdef QVOP_DEBUG
// logfile_print("Frame[0]: %d", buffer[0]);
// logfile_print("Frame[1]: %d", buffer[1]);
// logfile_print("Frame[%d]: %d", bufferlen, buffer[bufferlen - 1]);
  //#endif
  size_t size = sizeof(int16_t) * bufferlen;
  void * buf = data.writeInplace(size);

  if ( buf != NULL && size > 0) {
    memcpy(buf, buffer, size);
  } else {
    return QVOP_ERR_BAD_PARAM;
  }

  // binder call
  binderTransact(QVOP_REQ_ADD_FRAME, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::processFrameWithId(sp<IQvopCallback> const& callback, int64_t timestamp, int64_t frameId, int32_t bufferlen, int16_t const* buffer) {
  logfile_print("%s: entry, timestamp=%d, len=%d, frameId=%d\n", QVOP_FN, timestamp, bufferlen, frameId);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeInt64(timestamp);
  data.writeInt64(frameId);
  data.writeInt32(bufferlen);

#ifdef QVOP_DEBUG
// logfile_print("FrameId: %d", frameId);
// logfile_print("Frame[0]: %d", buffer[0]);
// logfile_print("Frame[1]: %d", buffer[1]);
// logfile_print("Frame[%d]: %d", bufferlen, buffer[bufferlen - 1]);
#endif
  size_t size = sizeof(int16_t) * bufferlen;
  void * buf = data.writeInplace(size);

  if ( buf != NULL && size > 0 ) {
    memcpy(buf, buffer, size);
  } else {
    return QVOP_ERR_BAD_PARAM;
  }

  // binder call
  binderTransact(QVOP_REQ_ADD_FRAME_WITH_ID, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());


  return ret;
}

QvopStatus BpQvopService::onStartAudioCapture(sp<IQvopCallback> const& callback, int32_t sampleRate, int32_t numberOfChannels, int32_t audioFormat) {
  logfile_print("%s: entry, sampleRate=%d, numberOfChannels=%d, audioFormat=%d\n", QVOP_FN, sampleRate, numberOfChannels, audioFormat);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  data.writeInt32(sampleRate);
  data.writeInt32(numberOfChannels);
  data.writeInt32(audioFormat);

  // binder call
  binderTransact(QVOP_REQ_START_AUDIO_CAPTURE, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}

QvopStatus BpQvopService::onStopAudioCapture(sp<IQvopCallback> const& callback) {
  logfile_print("%s: entry\n", QVOP_FN);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));


  // binder call
  binderTransact(QVOP_REQ_STOP_AUDIO_CAPTURE, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  return ret;
}


QvopStatus BpQvopService::enrollCaptureStart(sp<IQvopCallback> const& callback) {
  logfile_print("%s: entry\n", QVOP_FN);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));


  // binder call
  binderTransact(QVOP_REQ_ENROLL_CAPTURE_START, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());
  return ret;

}

QvopStatus BpQvopService::enrollCaptureComplete(sp<IQvopCallback> const& callback) {
  logfile_print("%s: entry\n", QVOP_FN);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));



  // binder call
  binderTransact(QVOP_REQ_ENROLL_CAPTURE_COMPLETE, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());
  return ret;
}

QvopStatus BpQvopService::enrollCommit(sp<IQvopCallback> const& callback) {
  logfile_print("%s: entry\n", QVOP_FN);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));


  // binder call
  binderTransact(QVOP_REQ_ENROLL_COMMIT, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());
  return ret;
}

QvopStatus BpQvopService::matchCaptureComplete(sp<IQvopCallback> const& callback) {
  logfile_print("%s: entry\n", QVOP_FN);

  Parcel data, reply;
  data.writeInterfaceToken(BpQvopService::getInterfaceDescriptor());
  data.writeStrongBinder(QvopAsBinder(callback));
  binderTransact(QVOP_REQ_MATCH_CAPTURE_COMPLETE, data, &reply);
  QvopStatus ret = static_cast<QvopStatus>(reply.readInt32());

  logfile_print("%s: exit\n", QVOP_FN);
  return ret;
}

BpQvopService::BpQvopService(const sp<IBinder>& impl) : BpInterface<IQvopService>(impl) {
  logfile_print("%s: entry...returning impl\n", QVOP_FN);
}

std::string BpQvopService::getBinderCode(int32_t code) {
  std::map<uint32_t, std::string>::const_iterator it = binderCodes.find(code);
  if (it != binderCodes.end()) {
    return it->second;
  } else {

    // unknown code, return its numeric value
    char codeStr[40];
    snprintf(codeStr, sizeof codeStr, "BINDER_CODE_%d", code);
    return codeStr;
  }
}

status_t BpQvopService::binderTransact(
  uint32_t code,
  const Parcel& data,
  Parcel* reply,
  uint32_t flags) {

  std::string msg = "BP-" + getBinderCode(code);
  QVOPTRACE_BEGIN(msg.c_str());
  status_t ret = remote()->transact(code, data, reply, flags);
  QVOPTRACE_END();

  return ret;
}


}; // namespace android
