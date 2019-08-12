/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANDROID_QVOPSERVICEBP_H
#define ANDROID_QVOPSERVICEBP_H

#include <binder/Parcel.h>
#include <string>
#include "IQvopService.h"


namespace android {

class BpQvopService : public BpInterface<IQvopService> {
public:
	BpQvopService(const sp<IBinder>& impl);

	virtual QvopStatus getVersion(sp<IQvopCallback> const& callback);
	virtual QvopStatus enroll(sp<IQvopCallback> const& callback, const char* user_id, const char* identifier, const char* phrase);
	virtual QvopStatus match(sp<IQvopCallback> const& callback, const char* user_id);
	virtual QvopStatus matchWithId(sp<IQvopCallback> const& callback, const char* user_id, const char* identifier);
	virtual QvopStatus matchSecure(sp<IQvopCallback> const& callback, const char* user_id);
	virtual QvopStatus matchAny(sp<IQvopCallback> const& callback, const char* user_id);
	virtual QvopStatus matchAnyKeyPhrase(sp<IQvopCallback> const& callback, const char* user_id, const char* phrase);
	virtual QvopStatus matchAnySecure(sp<IQvopCallback> const& callback, const char* user_id);
	virtual QvopStatus cancel();
	virtual QvopStatus deleteUser(sp<IQvopCallback> const& callback, const char* user_id);
	virtual QvopStatus deleteWithId(sp<IQvopCallback> const& callback, const char* user_id, const char* identifier);
	virtual QvopStatus deleteAll(sp<IQvopCallback> const& callback, const char* user_id);
	virtual QvopStatus processFrame(sp<IQvopCallback> const& callback, int64_t timestamp, int32_t len, int16_t const* buffer);
	virtual QvopStatus processFrameWithId(sp<IQvopCallback> const& callback, int64_t timestamp, int64_t frameId, int32_t len, int16_t const* buffer);
	virtual QvopStatus onStartAudioCapture(sp<IQvopCallback> const& callback, int32_t sampleRate, int32_t numberOfChannels, int32_t audioFormat);
	virtual QvopStatus onStopAudioCapture(sp<IQvopCallback> const& callback);
	virtual QvopStatus enrollCaptureStart(sp<IQvopCallback> const& callback);
	virtual QvopStatus enrollCaptureComplete(sp<IQvopCallback> const& callback);
	virtual QvopStatus enrollCommit(sp<IQvopCallback> const& callback);
	virtual QvopStatus matchCaptureComplete(sp<IQvopCallback> const& callback);
	virtual QvopStatus rename(sp<IQvopCallback> const& callback, const char* user_id, const char* old_id, const char* new_id);
	virtual QvopStatus setThreshold(sp<IQvopCallback> const& callback, const char* user_id, const char* identifier, int32_t threshold);
private:
	static std::string getBinderCode(int32_t id);
	status_t binderTransact(
	    uint32_t code,
	    const Parcel& data,
	    Parcel* reply,
	    uint32_t flags = 0);
};

}; // namespace android

#endif // ANDROID_QVOPSERVICEBP_H
