/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_ENROLL_COMMIT_H__
#define __QVOP_COMMAND_ENROLL_COMMIT_H__

#include "QvopCommandService.h"
#include "logfile.h"

/**
 * Command for VoicePrint Enrollment completion. Temlate should be saved.
 */
class QvopCommandEnrollCommit : public QvopCommandService {
public:
	QvopCommandEnrollCommit(sp<IQvopCallback> const& callback) :
		QvopCommandService(callback)
	{ }

protected:
	virtual void execute(IQvopCallback* callback) {

		QvopStatus enrollStatus = QvopEnrollCommit();
		if (QVOP_SUCCESS != enrollStatus) {
			logfile_print("QvopEnrollCommit failed");
		}

		logfile_print("ret=%d", enrollStatus);

		QvopStatus res = QvopSessionEnd();
		if (QVOP_SUCCESS != res) {
			logfile_print("QvopSessionEnd failed");
		}

		logfile_print("ret=%d", res);

		callback->onEnrollStatus(enrollStatus);
	}


private:


};

#endif  // __QVOP_COMMAND_ENROLL_COMMIT_H__



