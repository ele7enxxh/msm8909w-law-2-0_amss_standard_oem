/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_DELETE_H__
#define __QVOP_COMMAND_DELETE_H__

#include "QvopCommandService.h"
#include "logfile.h"

/**
 * Command for VoicePrint Delete calls.
 */
class QvopCommandDelete : public QvopCommandService {
public:
  QvopCommandDelete(sp<IQvopCallback> const& callback, const char* user = 0, const char * identifier = 0, bool all = false) :
    QvopCommandService(callback),
    deleteAll(false)
  {
    deleteAll = all;

    if ( user != 0 ) {
      strlcpy(user_id, user, QVOP_MAX_ENROLLEE_ID_LEN);
    }

    if ( identifier != 0 ) {
      strlcpy(keyphrase_id, identifier, QVOP_MAX_PHRASEID_LEN);
    }
  }

protected:
  virtual void execute(IQvopCallback* callback) {
    logfile_print("%s: QvopCommandDelete: Running\n", QVOP_FN);
    QvopStatus res = tryInitSession();
    if (QVOP_SUCCESS != res) {
      logfile_print("QVOPQvopSessionInit failed");
      callback->onError(res);
    }

    if ( deleteAll ) {
      logfile_print("Attempt to delete all");
      res = QvopDeleteAllTemplates();
      logfile_print("QvopDeleteAllTemplates res=%d", res);
      callback->onDeleteAllStatus(res);
    } else {
      logfile_print("Attempt to delete %s=", keyphrase_id);
      res = QvopDeleteTemplate(user_id, keyphrase_id);
      logfile_print("QvopDeleteTemplate res=%d", res);
      callback->onDeleteStatus(res, user_id, keyphrase_id);
    }

    res = tryCloseSession();

  }


private:
  bool deleteAll;
  char user_id[QVOP_MAX_ENROLLEE_ID_LEN];
  char keyphrase_id[QVOP_MAX_PHRASEID_LEN];

};

#endif  // __QVOP_COMMAND_DELETE_H__



