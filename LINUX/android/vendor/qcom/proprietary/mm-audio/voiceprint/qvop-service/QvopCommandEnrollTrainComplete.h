/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_ENROLL_CAPTURE_COMPLETE_H__
#define __QVOP_COMMAND_ENROLL_CAPTURE_COMPLETE_H__

#include "QvopCommandService.h"
#include "logfile.h"
/**
 * Command for VoicePrint training session complete. Model should be processed.
 */
class QvopCommandEnrollTrainComplete : public QvopCommandService {
public:
	QvopCommandEnrollTrainComplete(sp<IQvopCallback> const& callback) :
		QvopCommandService(callback)
	{ }

protected:
	virtual void execute(IQvopCallback* callback) {
		struct qvop_pcm_packet pcmPkt;
		pcmPkt.eos_flag = QVOP_PCM_EOS;

		// Workaround
		// Put empty data in there since qvop_qsee is failing with the flag only.
		int16_t buffer[1] = { 0 };
		pcmPkt.num_pcm_samples = 1;
		memcpy(pcmPkt.pcm_data, buffer, 1);

		logfile_print("Sending EOS!");
		QvopStatus res = QvopSendPCM(&pcmPkt);
		if ( QVOP_PCM_DATA_REQUEST == res ) {
			logfile_print("Why are you asking for more? QVOP_PCM_DATA_REQUEST");
		}
		logfile_print("ret=%d", res);

		// handle error
		if ( res ==  QVOP_ENROLLMENT_COMPLETE ||
		        res == QVOP_SUCCESS ) {
			logfile_print("Capture is successful.");
			callback->onEnrollStatus(QVOP_SUCCESS);
		} else {
			logfile_print("Capture has failed.");
			callback->onEnrollStatus(res);
		}
	}

private:

};

#endif  // __QVOP_COMMAND_ENROLL_CAPTURE_COMPLETE_H__



