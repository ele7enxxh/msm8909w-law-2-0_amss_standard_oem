/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#include "BnQvopService.h"
#include "logfile.h"

#include <binder/IPCThreadState.h>

namespace android {

status_t BnQvopService::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags) {
  //logfile_print("%s: entry\n", QVOP_FN);

  // security check, only system processes may call this
  int const callingUid = IPCThreadState::self()->getCallingUid();
  int const myUid = getuid();
  if (callingUid != myUid && callingUid != 0) {
    logfile_print("%s: denied access attempt by uid %d (%d)\n", QVOP_FN, callingUid, myUid);
    return PERMISSION_DENIED;
  }

  status_t ret = UNKNOWN_ERROR;

  // Handle request
  switch (code) {
  case QVOP_REQ_ENROLL:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    char const* user = data.readCString();
    char const* id = data.readCString();
    char const* phrase = data.readCString();
    status_t res = enroll(callback, user, id, phrase);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_SET_THRESHOLD:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    char const* user = data.readCString();
    char const* id = data.readCString();
    int32_t threshold = data.readInt32();
    status_t res = setThreshold(callback, user, id, threshold);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_RENAME:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    char const* user = data.readCString();
    char const* old_id = data.readCString();
    char const* new_id = data.readCString();
    status_t res = rename(callback, user, old_id, new_id);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_MATCH:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    char const* user = data.readCString();

    status_t res = match(callback, user);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_MATCH_WITH_ID:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    char const* user = data.readCString();
    char const* id = data.readCString();
    status_t res = matchWithId(callback, user, id);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_MATCH_SECURE:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    char const* user = data.readCString();
    status_t res = matchSecure(callback, user);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_MATCH_ANY:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    char const* user = data.readCString();
    status_t res = matchAny(callback, user);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_MATCH_ANY_KEYPHRASE:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    char const* user = data.readCString();
    char const* phrase = data.readCString();
    status_t res = matchAnyKeyPhrase(callback, user, phrase);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_MATCH_ANY_SECURE:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    char const* user = data.readCString();
    status_t res = matchAnySecure(callback, user);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_CANCEL:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    status_t res = cancel();
    reply->writeInt32(res);
  }
  break;

  case QVOP_REQ_DELETE_USER_WITH_ID:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    char const* user = data.readCString();
    char const* id = data.readCString();
    status_t res = deleteWithId(callback, user, id);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_DELETE_USER:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    char const* user = data.readCString();
    status_t res = deleteUser(callback, user);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_DELETE_ALL:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    char const* user = data.readCString();
    status_t res = deleteAll(callback, user);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_ADD_FRAME:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    int64_t timestamp = data.readInt64();
    int32_t len = data.readInt32();
    int size = sizeof(int16_t) * len;

    status_t res = processFrame(callback, timestamp, len, (int16_t const*)data.readInplace(size));
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_ADD_FRAME_WITH_ID:
  {
    //logfile_print("QVOP_REQ_ADD_FRAME_WITH_ID");
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    int64_t timestamp = data.readInt64();
    int64_t frameId = data.readInt64();
    int32_t len = data.readInt32();
    int size = sizeof(int16_t) * len;
    int16_t const* buffer = (int16_t const*) data.readInplace(size);

#ifdef QVOP_DEBUG
    //logfile_print("FrameId: %d", frameId);
    //logfile_print("Frame[0]: %d", buffer[0]);
    //logfile_print("Frame[1]: %d", buffer[1]);
    //logfile_print("Frame[%d]: %d", len, buffer[len - 1]);
#endif

    status_t res = processFrameWithId(callback, timestamp, frameId, len, buffer);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_START_AUDIO_CAPTURE:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    int32_t sampleRate = data.readInt32();
    int32_t numberOfChannels = data.readInt32();
    int32_t audioFormat = data.readInt32();

    status_t res = onStartAudioCapture(callback, sampleRate, numberOfChannels, audioFormat);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_STOP_AUDIO_CAPTURE:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());

    status_t res = onStopAudioCapture(callback);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_ENROLL_CAPTURE_START:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());

    status_t res = enrollCaptureStart(callback);
    reply->writeInt32(res);
  }
  break;

  case QVOP_REQ_ENROLL_CAPTURE_COMPLETE:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());

    status_t res = enrollCaptureComplete(callback);
    reply->writeInt32(res);
  }
  break;

  case QVOP_REQ_ENROLL_COMMIT:
  {
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());

    status_t res = enrollCommit(callback);
    reply->writeInt32(res);
  }
  break;
  case QVOP_REQ_MATCH_CAPTURE_COMPLETE:
  {
    //logfile_print("Received QVOP_REQ_MATCH_CAPTURE_COMPLETE");
    CHECK_INTERFACE(IQvopService, data, reply);
    sp<IQvopCallback> callback = interface_cast<IQvopCallback>(data.readStrongBinder());
    status_t res = matchCaptureComplete(callback);
    reply->writeInt32(res);
  }
  break;
  default:
    logfile_print("%s: default hit! Unknown Binder command ID:%d - returning...\n", QVOP_FN, code);
    return BBinder::onTransact(code, data, reply, flags);
  }
  //logfile_print("%s: returning %d\n", QVOP_FN, ret);
  return NO_ERROR;
}
}; // namespace android
