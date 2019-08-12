/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_MATCH_H__
#define __QVOP_COMMAND_MATCH_H__

#include "QvopCommandService.h"
#include "logfile.h"

/**
 * Command for VoicePrint Match calls.
 */
using namespace android;

class QvopCommandMatch : public QvopCommandService {
public:
	QvopCommandMatch(sp<IQvopCallback> const& callback, const char* user_id, const char * identifier) :
		QvopCommandService(callback) {

		if ( user_id != NULL ) {
			logfile_print("user_id=%s", user_id);
			//int length = strlen(user_id);
			strlcpy(match_params.user_id, user_id, QVOP_MAX_ENROLLEE_ID_LEN);
		} else {
                match_params.user_id[0] = '\0';
        }
		if ( identifier != NULL ) {
			logfile_print("identifier=%s", identifier);
			//int length = strlen(identifier);
			strlcpy(match_params.keyphrase_id, identifier, QVOP_MAX_PHRASEID_LEN);
		} else {
                match_params.keyphrase_id[0] = '\0';
        }

	}

protected:
	virtual void execute(IQvopCallback* callback) {
		logfile_print("%s: execute\n", QVOP_FN);

		QvopStatus res = tryInitSession();
		if (QVOP_SUCCESS != res) {
			logfile_print("QVOPQvopSessionInit failed");
			callback->onError(res);
			return;
		}

		logfile_print("%s: TZ loaded.\n", QVOP_FN);

		res = QvopMatchBegin(&match_params);

		if ( res == QVOP_SUCCESS || res == QVOP_PCM_DATA_REQUEST) {
			logfile_print("QvopMatchBegin initiated successfully res=%d", res);
		}  else  {
			logfile_print("QvopMatchBegin failed res=%d", res);
			callback->onError(res);
		}
	}


private:
	struct qvop_match_params match_params;

};

#endif  // __QVOP_COMMAND_MATCH_H__



