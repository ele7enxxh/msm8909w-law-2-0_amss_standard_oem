/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_GETVERSION_H__
#define __QVOP_COMMAND_GETVERSION_H__

#include "QvopCommandService.h"
#include "logfile.h"

/**
 * Command to retrieve VoicePrint algorithm version.
 */
class QvopCommandGetVersion : public QvopCommandService {
public:
  QvopCommandGetVersion(sp<IQvopCallback> const& callback) :
    QvopCommandService(callback)
  { }

protected:
  virtual void execute(IQvopCallback* callback) {
    logfile_print("%s: entry.\n", QVOP_FN);

    QvopStatus res = tryInitSession();
    if (QVOP_SUCCESS != res) {
      logfile_print("QVOPQvopSessionInit failed");
      callback->onError(res);
    }

    logfile_print("%s: TZ loaded.\n", QVOP_FN);

    res = QvopGetVersion(&version_info);

    if ( res == QVOP_SUCCESS) {
      logfile_print("QvopGetVersion success res=%d, major=%d, minor=%d",
                    res, version_info.major, version_info.minor);
      callback->onVersionInfo(static_cast<long>(version_info.major),
                          static_cast<long>(version_info.minor));
    } else  {
      logfile_print("QvopGetVersion failed res=%d", res);
    }
  }


private:
  struct qvop_version_info version_info;
};

#endif  // __QVOP_COMMAND_GETVERSION_H__



