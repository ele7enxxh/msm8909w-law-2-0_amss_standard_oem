/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QVOP_COMMAND_PROCESS_FRAME_H__
#define __QVOP_COMMAND_PROCESS_FRAME_H__

#include "QvopCommandService.h"
#include "QvopCommandMatchGetResult.h"
#include "logfile.h"

/**
 * Command for VoicePrint audio frame processing calls.
 */
using namespace android;

class QvopCommandProcessFrame : public QvopCommandService {
public:
	QvopCommandProcessFrame(sp<IQvopCallback> const& callback,
	                        int64_t timestamp,
	                        int64_t frameId,
	                        int32_t bufferlen,
	                        int16_t const * buffer) :
		QvopCommandService(callback) {
		logfile_print("%s: entry", QVOP_FN);

		// pton TODO remove timestamp
		// frameId to be used; will be added pcmPkt

#ifdef QVOP_DEBUG
		logfile_print("FrameId: %d", frameId);
		//logfile_print("Frame[0]: %d", buffer[0]);
		//logfile_print("Frame[1]: %d", buffer[1]);
		//logfile_print("Frame[%d]: %d", bufferlen, buffer[bufferlen - 1]);
#endif

		pcmPkt.num_pcm_samples = bufferlen;
		pcmPkt.eos_flag = QVOP_PCM_NO_EOS;
		pcmPkt.pcm_format = 16; // subhash rename this TODO enum?

		mCurrentIndex = frameId;

		size_t size = sizeof(int16_t) * bufferlen;
		memcpy(pcmPkt.pcm_data, buffer, size);
	}


protected:
	virtual void execute(IQvopCallback * callback) {

		logfile_print("%s: entry", QVOP_FN);


		QvopStatus res = QvopSendPCM(&pcmPkt);


		logfile_print("ret=%d", res);


		if (  QVOP_PCM_DATA_REQUEST == res ) {
			// More data requested. This is valid.
			// logfile_print("ret=%d [QVOP_PCM_DATA_REQUEST]", res);
		} else if (  QVOP_SUCCESS == res ) {
			// Okay to continue
		} else if ( QVOP_MATCH_COMPLETE == res || QVOP_MATCH_FAILURE == res ) {
			logfile_print("Found a match within this frame id=%d", mCurrentIndex);

			struct qvop_match_results match_results;
			QvopStatus matchStatus = QvopMatchGetResult(&match_results);
			logfile_print("QvopMatchGetResult query");
			if ( matchStatus == QVOP_SUCCESS || matchStatus == QVOP_MATCH_SUCCESS) {
				logfile_print("onMatchFound: id=%d, byteIndex=%d, sentence_score=%lf, verification_score=%lf, spoofScore=%f",
				              match_results.resp_id,
				              match_results.index,
				              match_results.sentence_score,
				              match_results.verification_score,
				              match_results.as_fSvmScore);
				callback->onMatchFound(match_results.user_id,
				                       match_results.keyphrase_id,
				                       match_results.index,
				                       match_results.sentence_score,
				                       match_results.verification_score,
				                       match_results.as_fSvmScore);


			} else {
				logfile_print("onMatchNotFound: id=%d, byteIndex=%d, sentence_score=%lf, verification_score=%lf, spoofScore=%f",
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



		} else {
			logfile_print("Error while processing frame.");
			callback->onError(res);
		}


		logfile_print("%s: exit", QVOP_FN);
	}


private:
	struct qvop_pcm_packet pcmPkt;
	int32_t mCurrentIndex;

};

#endif  // __QVOP_COMMAND_PROCESS_FRAME_H__



