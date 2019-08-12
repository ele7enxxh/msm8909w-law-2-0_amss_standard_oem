/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_MATCH_GET_RESULT_H__
#define __QVOP_COMMAND_MATCH_GET_RESULT_H__

#include "QvopCommandService.h"
#include "logfile.h"

/**
 * Command for VoicePrint Match Get Result calls.
 */
using namespace android;

class QvopCommandMatchGetResult : public QvopCommandService {
public:
	QvopCommandMatchGetResult(sp<IQvopCallback> const& callback) :
		QvopCommandService(callback) {
	}

protected:
	virtual void execute(IQvopCallback* callback) {
		logfile_print("%s: execute\n", QVOP_FN);

		// Send EOS flag to start processing
		struct qvop_pcm_packet pcmPkt;
		pcmPkt.eos_flag = QVOP_PCM_EOS;
		logfile_print("Sending EOS!");

		QvopStatus res = QvopSendPCM(&pcmPkt);
		if ( QVOP_PCM_DATA_REQUEST == res ) {
			logfile_print("Why are you asking for more? QVOP_PCM_DATA_REQUEST");
		}
		logfile_print("ret=%d", res);

		// Get the results from the process.
		struct qvop_match_results match_results;
		QvopStatus matchStatus = QvopMatchGetResult(&match_results);
		logfile_print("QvopMatchGetResult query");
		if ( matchStatus == QVOP_SUCCESS || matchStatus == QVOP_MATCH_SUCCESS) {
			logfile_print("onMatchFound: id=%d, index=%d, sentence_score=%lf, verification_score=%lf, spoofScore=%f",
						  match_results.resp_id,
			              match_results.index,
			              match_results.sentence_score,
			              match_results.verification_score,
			              match_results.as_fSvmScore);
			callback->onMatchFound(match_results.user_id, match_results.keyphrase_id,
			                       match_results.index, match_results.sentence_score,
			                       match_results.verification_score,
			                       match_results.as_fSvmScore);


		} else {
			logfile_print("onMatchNotFound: id=%d, index=%d, sentence_score=%lf, verification_score=%lf, spoofScore=%f",
				      match_results.resp_id,
			              match_results.index,
			              match_results.sentence_score,
			              match_results.verification_score,
			              match_results.as_fSvmScore);
			callback->onMatchNotFound(matchStatus,
			                          match_results.index,
			                          match_results.sentence_score,
			                          match_results.verification_score,
			                          match_results.as_fSvmScore);

		}

		logfile_print("ret=%d", matchStatus);

		res = QvopSessionEnd();
		if (QVOP_SUCCESS != res) {
			logfile_print("QvopSessionEnd failed");
		} else {
			logfile_print("QvopSessionEnd success");
		}
		logfile_print("ret=%d", res);


		logfile_print("%s: exit.\n", QVOP_FN);

	}


private:

};

#endif  // __QVOP_COMMAND_MATCH_GET_RESULT_H__



