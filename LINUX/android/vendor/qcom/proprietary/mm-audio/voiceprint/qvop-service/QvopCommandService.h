/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __QVOP_COMMAND_SERVICE_H__
#define __QVOP_COMMAND_SERVICE_H__

#include "IQvopService.h"
#include <binder/IBinder.h>

using namespace android;

/**
 * Abstract base class for service commands which handles these
 * common functions:
 *
 *  - Managing the callback lifetime
 *  - Registering and handling callback death notification
 *  - Keeping track of any cancellation request
 *
 */
class QvopCommandService : public android::IBinder::DeathRecipient {
public:
  /**
   * Constructor
   *
   * @param callback Callback, 0 if none
   */
  QvopCommandService(sp<IQvopCallback> const& callback);
  virtual ~QvopCommandService();

  // common fn then forwards execution to the derived class
  void execute();


protected:
  /**
   * Execute the target. Override in the derived class.
   *
   * @param callback Callback, 0 if none
   */
  virtual void execute(IQvopCallback* callback) = 0;

  /**
   * Check for cancel request
   *
   * @return bool True if thread cancellation has been requested.
   */
  bool isCanceled() const;


  // Try opening a TZ session
  QvopStatus tryInitSession();

  // Try closing a TZ session
  QvopStatus tryCloseSession();

  // Binder callback
  sp<IQvopCallback> mCallback;

private:

  // implementation of DeathRecipient
  virtual void binderDied(const wp<IBinder>& who);

  // flags
  bool mComplete;
  volatile bool mCanceled;


};


#endif  // __QVOP_COMMAND_SERVICE_H__

