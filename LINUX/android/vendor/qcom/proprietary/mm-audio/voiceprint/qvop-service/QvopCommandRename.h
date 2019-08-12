/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QFP_COMMAND_RENAME_H__
#define __QFP_COMMAND_RENAME_H__

#include "QvopCommandService.h"
#include "logfile.h"

/**
 * Command for VoicePrint Rename calls.
 */
class QvopCommandRename : public QvopCommandService {
public:
  QvopCommandRename(sp<IQvopCallback> const& callback,  const char* user_id, const char * old_id, const char * new_id) :
    QvopCommandService(callback)
  {
    if ( user_id != 0 ) {
      strlcpy(mUser, user_id, QVOP_MAX_ENROLLEE_ID_LEN);
    }
    if ( old_id != 0 ) {
      strlcpy(mOldId, old_id, QVOP_MAX_PHRASEID_LEN);
    }

    if ( new_id != 0 ) {
      strlcpy(mNewId, new_id, QVOP_MAX_PHRASEID_LEN);
    }
  }

protected:
  virtual void execute(IQvopCallback* callback) {

    QvopStatus res = tryInitSession();
    if (QVOP_SUCCESS != res) {
      logfile_print("QVOPQvopSessionInit failed");
      callback->onError(res);
      return;
    }

    logfile_print("%s: TZ loaded.\n", QVOP_FN);

    res = QvopRenameTemplate(mUser, mOldId, mNewId);
    logfile_print("QvopRenameTemplate res=%d", res);

    callback->onRenameStatus(res);

    res = tryCloseSession();
  }


private:

  char mUser[QVOP_MAX_ENROLLEE_ID_LEN];
  char mOldId[QVOP_MAX_PHRASEID_LEN];
  char mNewId[QVOP_MAX_PHRASEID_LEN];

};

class QvopCommandSetThreshold : public QvopCommandService {
public:
  QvopCommandSetThreshold(sp<IQvopCallback> const& callback,  const char* user_id, const char * identifier, int32_t threshold) :
    QvopCommandService(callback)
  {
    if ( user_id != 0 ) {
      strlcpy(param.user_id, user_id, QVOP_MAX_ENROLLEE_ID_LEN);
    }
    if ( identifier != 0 ) {
      strlcpy(param.keyphrase_id, identifier, QVOP_MAX_PHRASEID_LEN);
    }

    param.auth_level = threshold;
  }

protected:
  virtual void execute(IQvopCallback* callback) {

    QvopStatus res = tryInitSession();
    if (QVOP_SUCCESS != res) {
      logfile_print("QVOPQvopSessionInit failed");
      callback->onError(res);
    }

    logfile_print("%s: TZ loaded.\n", QVOP_FN);

    res = QvopConfigAuthParams(&param);
    logfile_print("QvopConfigAuthParams res=%d", res);

    res = tryCloseSession();
  }


private:
  struct qvop_config_auth_params param;


};


#endif  // __QFP_COMMAND_RENAME_H__



