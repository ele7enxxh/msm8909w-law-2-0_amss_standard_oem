/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//////////////////////////////////////////////////////////////////////////////////////
//
// This file contains calls that are made through Binder and on to TrustZone. The functions
// here are called by the Binder BnQvopService::OnTransact function and will do the
// interface calls. The tz interface is responsible for marshaling data in/out of TZ.

#include <stdio.h>

#include <map>

#include "QvopService.h"
#include "QvopCommandGetVersion.h"
#include "QvopCommandProcessFrame.h"
#include "QvopCommandMatch.h"
#include "QvopCommandMatchGetResult.h"
#include "QvopCommandEnroll.h"
#include "QvopCommandEnrollTrainStart.h"
#include "QvopCommandEnrollTrainComplete.h"
#include "QvopCommandEnrollCommit.h"
#include "QvopCommandMatchAny.h"
#include "QvopCommandDelete.h"
#include "QvopCommandRename.h"


namespace android {


// map binder transaction codes to their string representation
static const std::pair<int32_t, std::string> tzResponseCode[] = {
  std::make_pair(QVOP_SUCCESS, "QVOP_SUCCESS"),
  std::make_pair(QVOP_FAILURE, "QVOP_FAILURE"),
  std::make_pair(QVOP_TIMEOUT, "QVOP_TIMEOUT"),
  std::make_pair(QVOP_CANCEL, "QVOP_CANCEL"),
  std::make_pair(QVOP_STOPPED, "QVOP_STOPPED"),
  std::make_pair(QVOP_PCM_DATA_REQUEST, "QVOP_PCM_DATA_REQUEST"),
  std::make_pair(QVOP_PCM_REWIND, "QVOP_PCM_REWIND"),
  std::make_pair(QVOP_ENROLLMENT_COMPLETE, "QVOP_ENROLLMENT_COMPLETE"),
  std::make_pair(QVOP_TZ_CANCEL_SUCCESS, "QVOP_TZ_CANCEL_SUCCESS"),
  std::make_pair(QVOP_ENROLLMENT_FAILED, "QVOP_ENROLLMENT_FAILED"),
  std::make_pair(QVOP_ENROLLMENT_NO_SPACE, "QVOP_ENROLLMENT_NO_SPACE"),
  std::make_pair(QVOP_ENROLLEE_NOT_FOUND, "QVOP_ENROLLEE_NOT_FOUND"),
  std::make_pair(QVOP_MATCH_SUCCESS, "QVOP_MATCH_SUCCESS"),
  std::make_pair(QVOP_MATCH_COMPLETE, "QVOP_MATCH_COMPLETE"),
  std::make_pair(QVOP_MATCH_FAILURE, "QVOP_MATCH_FAILURE"),
  std::make_pair(QVOP_ERR_UNKNOWN_COMMAND, "QVOP_ERR_UNKNOWN_COMMAND"),
  std::make_pair(QVOP_ERR_NOT_IMPLEMENTED, "QVOP_ERR_NOT_IMPLEMENTED"),
  std::make_pair(QVOP_ERR_NOT_INITIALIZED, "QVOP_ERR_NOT_INITIALIZED"),
  std::make_pair(QVOP_ERR_BAD_PARAM, "QVOP_ERR_BAD_PARAM"),
  std::make_pair(QVOP_ERR_MALLOC, "QVOP_ERR_MALLOC"),
  std::make_pair(QVOP_ERR_FREE, "QVOP_ERR_FREE"),
  std::make_pair(QVOP_ENROLLMENT_SECURE_UI_ERR, "QVOP_ENROLLMENT_SECURE_UI_ERR"),


};

static const std::map<uint32_t, std::string> binderCodes(tzResponseCode,
    tzResponseCode + sizeof tzResponseCode / sizeof *tzResponseCode);

QvopService::QvopService() :
  mIsShutdown(false)
{
  logfile_print("%s:QvopService constructed\n", QVOP_FN);

}


QvopService::~QvopService() {

}

QvopStatus QvopService::getVersion(sp<IQvopCallback> const& callback) {
  logfile_print("%s: entry.\n", QVOP_FN);
  return execute(new QvopCommandGetVersion(callback));
}



QvopStatus QvopService::enroll(sp<IQvopCallback> const& callback,
                               const char* user_id,
                               const char* identifier,
                               const char* keyphrase) {
  logfile_print("%s: entry user=%s, id=%s, phrase=%s.\n", QVOP_FN, user_id, identifier, keyphrase);

  return execute(new QvopCommandEnroll(callback, user_id, identifier, keyphrase));

}

QvopStatus QvopService::setThreshold(sp<IQvopCallback> const& callback,
                               const char* user_id,
                               const char* identifier,
                               int32_t setThreshold) {
  logfile_print("%s: entry user=%s, id=%s, setThreshold=%d.\n", QVOP_FN, user_id, identifier, setThreshold);

  return execute(new QvopCommandSetThreshold(callback, user_id, identifier, setThreshold));

}

QvopStatus QvopService::rename(sp<IQvopCallback> const& callback,
                               const char* user_id,
                               const char* old_id,
                               const char* new_id) {
  logfile_print("%s: entry user=%s, old_id=%s, new_id=%s.\n", QVOP_FN, user_id, old_id, new_id);

  return execute(new QvopCommandRename(callback, user_id, old_id, new_id));

}


QvopStatus QvopService::enrollCaptureStart(sp<IQvopCallback> const& callback) {
  logfile_print("%s: entry.\n", QVOP_FN);

  return QVOP_SUCCESS;
}


QvopStatus QvopService::enrollCaptureComplete(sp<IQvopCallback> const & callback) {
  logfile_print("%s: entry.\n", QVOP_FN);

  return execute(new QvopCommandEnrollTrainComplete(callback));
}

QvopStatus QvopService::enrollCommit(sp<IQvopCallback> const & callback) {
  logfile_print("%s: entry.\n", QVOP_FN);

  return execute(new QvopCommandEnrollCommit(callback));
}

QvopStatus QvopService::matchCaptureComplete(sp<IQvopCallback> const & callback) {
  logfile_print("%s: entry.\n", QVOP_FN);

  return execute(new QvopCommandMatchGetResult(callback));
}

QvopStatus QvopService::match(sp<IQvopCallback> const & callback, const char* user_id) {

  logfile_print("%s: entry.\n", QVOP_FN);

  return matchWithId(callback, user_id, 0);
}

QvopStatus QvopService::matchWithId(sp<IQvopCallback> const & callback, const char* user_id, const char* identifier) {
  logfile_print("%s: entry.\n", QVOP_FN);


  return execute(new QvopCommandMatch(callback, user_id, identifier));
}

QvopStatus QvopService::matchSecure(sp<IQvopCallback> const & callback, const char* user_id) {
  logfile_print("%s: entry.\n", QVOP_FN);
  // TODO PTON
  return execute(new QvopCommandMatch(callback, user_id, 0));

}

QvopStatus QvopService::matchAny(sp<IQvopCallback> const & callback, const char* user_id) {
  logfile_print("%s: entry.\n", QVOP_FN);
  return execute(new QvopCommandMatchAny(callback, user_id, 0));

}

QvopStatus QvopService::matchAnyKeyPhrase(sp<IQvopCallback> const & callback, const char* user_id, const char* phrase) {
  logfile_print("%s: entry.\n", QVOP_FN);
  // TODO PTON
  return execute(new QvopCommandMatchAny(callback, user_id, 0));

}

QvopStatus QvopService::matchAnySecure(sp<IQvopCallback> const & callback, const char* user_id) {
  logfile_print("%s: entry.\n", QVOP_FN);
  // TODO PTON
  return execute(new QvopCommandMatchAny(callback, user_id, 0));

}

QvopStatus QvopService::deleteUser(sp<IQvopCallback> const & callback, const char* user_id) {
  logfile_print("%s: entry.\n", QVOP_FN);
  return execute(new QvopCommandDelete(callback, user_id));

}

QvopStatus QvopService::deleteWithId(sp<IQvopCallback> const & callback, const char* user_id, const char* identifier) {
  logfile_print("%s: entry.\n", QVOP_FN);
  return execute(new QvopCommandDelete(callback, user_id, identifier, false));

}

QvopStatus QvopService::deleteAll(sp<IQvopCallback> const & callback, const char* user_id) {
  logfile_print("%s: entry.\n", QVOP_FN);
  return execute(new QvopCommandDelete(callback, user_id, 0, true));

}


QvopStatus QvopService::processFrame(sp<IQvopCallback> const & callback, int64_t timestamp, int32_t bufferlen, int16_t const * buffer) {
  logfile_print("%s: entry bufferlen=%d\n", QVOP_FN, bufferlen);

  return execute( new QvopCommandProcessFrame(callback, timestamp, 0, bufferlen, buffer));
}

QvopStatus QvopService::processFrameWithId(sp<IQvopCallback> const & callback, int64_t timestamp, int64_t frameId, int32_t bufferlen, int16_t const * buffer) {
  logfile_print("%s: entry bufferlen=%d, frameId=%d\n", QVOP_FN, bufferlen, frameId);

  return execute( new QvopCommandProcessFrame(callback, timestamp, frameId, bufferlen, buffer));
}

QvopStatus QvopService::onStartAudioCapture(sp<IQvopCallback> const & callback, int32_t sampleRate, int32_t numberOfChannels, int32_t audioFormat) {

  logfile_print("%s: entry \n", QVOP_FN);

  return QVOP_SUCCESS;
}

QvopStatus QvopService::onStopAudioCapture(sp<IQvopCallback> const & callback) {
  logfile_print("%s: entry \n", QVOP_FN);

  return QVOP_SUCCESS;
}

QvopStatus QvopService::execute(sp<QvopCommandService> const & target) {
  logfile_print("%s: entry.\n", QVOP_FN);
  if (mIsShutdown) {
    logfile_print("%s: called when shutdown\n", QVOP_FN);
    return QVOP_FAILURE;
  }

  target->execute();

  logfile_print("%s: exit.\n", QVOP_FN);
  return QVOP_SUCCESS;

}

QvopStatus QvopService::cancel() {
  logfile_print("%s: entry.\n", QVOP_FN);

  QvopStatus res = QvopSessionEnd();
  if (QVOP_SUCCESS != res) {
    logfile_print("QvopSessionEnd failed");
  } else {
    logfile_print("QvopSessionEnd success");
  }
  logfile_print("ret=%d", res);

  return QVOP_SUCCESS;
}

status_t QvopService::onTransact(uint32_t code, const Parcel & data, Parcel * reply, uint32_t flags) {
  //logfile_print("%s: entry...// returning\n", QVOP_FN);
  return BnQvopService::onTransact(code, data, reply, flags);
}


sp<QvopService> QvopService::instantiate() {
  logfile_print("%s: getServiceName()=%s\n", QVOP_FN, IQvopService::getServiceName());

  // create and init service
  sp<QvopService> qvopService(new QvopService());

  // register service
  sp<IServiceManager> sm(defaultServiceManager());
  sm->addService(String16(IQvopService::getServiceName()), qvopService, false);

  return qvopService;
}

}; // namespace android
