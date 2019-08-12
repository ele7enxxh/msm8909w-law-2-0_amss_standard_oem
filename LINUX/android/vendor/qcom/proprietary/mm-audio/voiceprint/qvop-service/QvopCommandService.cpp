/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "QvopCommandService.h"
#include "logfile.h"

#include <cutils/properties.h>


QvopCommandService::QvopCommandService(sp<IQvopCallback> const& callback) :
  mCallback(callback),
  mComplete(false),
  mCanceled(false)

{
  logfile_print("%s: entry", QVOP_FN);
}

QvopCommandService::~QvopCommandService()
{
  logfile_print("%s: entry", QVOP_FN);
  // release callback strong pointer
  if ( mCallback != NULL ) {
    sp<IBinder> binder = QvopAsBinder(mCallback);
    if ( binder != NULL ) {
      binder->unlinkToDeath(this);
    }
  }
}

bool QvopCommandService::isCanceled() const {
  return mCanceled;
}

QvopStatus QvopCommandService::tryInitSession() {

  QvopStatus res = QvopSessionInit();

  if ( res == QVOP_SUCCESS ) {
    return res;
  }

  // pton TODO Subhash will return session in progress.
  logfile_print("Session init failed. Try closing the session first.");
  res = QvopSessionEnd();

  if (QVOP_SUCCESS != res) {
    logfile_print("QvopSessionEnd failed");
  }

  // Retry opening session
  res = QvopSessionInit();

  return res;
}

QvopStatus QvopCommandService::tryCloseSession() {
  QvopStatus res = QvopSessionEnd();
  if (QVOP_SUCCESS != res) {
    logfile_print("QvopSessionEnd failed");
  }

  logfile_print("ret=%d", res);

  return res;
}

void QvopCommandService::execute() {
  logfile_print("%s: entry", QVOP_FN);
  if (mComplete) {
    logfile_print("%s: attempt to rerun thread after completion\n", QVOP_FN);
    return;
  }

  // register callback death notification
  if ( mCallback != NULL ) {
    sp<IBinder> binder = QvopAsBinder(mCallback);
    if ( binder != NULL ) {
      binder->linkToDeath(this);
    }
  }


  // Execute target command
  execute(mCallback.get());

  mCallback.clear();

  mComplete = true;
  logfile_print("%s: exit", QVOP_FN);
}

void QvopCommandService::binderDied(const wp<IBinder>& who) {
  logfile_print("%s: callback binder died, cancel\n", QVOP_FN);
  mCanceled = true;

  if ( mCallback != NULL ) {
    sp<IBinder> binder = QvopAsBinder(mCallback);
    if ( binder != NULL ) {
      binder->unlinkToDeath(this);
    }
  }
}



