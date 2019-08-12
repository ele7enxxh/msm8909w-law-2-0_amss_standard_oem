/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_MATCHANY_H__
#define __QVOP_COMMAND_MATCHANY_H__

#include "QvopCommandService.h"
#include "logfile.h"


/**
 * Command for VoicePrint MatchAny calls.
 */
class QvopCommandMatchAny : public QvopCommandService {
public:
  QvopCommandMatchAny(sp<IQvopCallback> const& callback, const char* user_id, const char * keyphrase) :
    QvopCommandService(callback)
  {
    if ( user_id != NULL ) {
      logfile_print("user_id=%s", user_id);
      int length = strlen(user_id);
      strlcpy(match_params.user_id, user_id, length);
    }
    if ( keyphrase != NULL ) {
      logfile_print("keyphrase=%s", keyphrase);
      int length = strlen(keyphrase);
      strlcpy(match_params.keyphrase, keyphrase, length);
    }
  }

protected:
  virtual void execute(IQvopCallback* callback) {

  }

private:

  struct qvop_match_params match_params;

};

#endif  // __QVOP_COMMAND_MATCHANY_H__



