/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_ENROLL_CAPTURE_H__
#define __QVOP_COMMAND_ENROLL_CAPTURE_H__

#include "QvopCommandService.h"
#include "logfile.h"

/**
 * Command for VoicePrint Enrollment data processing calls.
 */
class QvopCommandEnrollTrainStart : public QvopCommandService {
public:
	QvopCommandEnrollTrainStart(sp<IQvopCallback> const& callback) :
		QvopCommandService(callback)
	{ }

protected:
	virtual void execute(IQvopCallback* callback) {
		// Doesn't really do anything
	}

private:

};

#endif  // __QVOP_COMMAND_ENROLL_CAPTURE_H__



