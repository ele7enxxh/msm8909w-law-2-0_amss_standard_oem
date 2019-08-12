/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANDROID_IQVOPSERVICE_H
#define ANDROID_IQVOPSERVICE_H

#include <binder/IInterface.h>
#include <string>

#include "logfile.h"

extern "C" {
#include "qvop_qsee_interface.h"

    namespace android {

    // IDs for each QVOP command. These are based on the Binder FIRST_CALL_TRANSACTION.
    // These are shared between the service and the client.
    enum QvopBinderCodes {
        QVOP_REQ_ENROLL = IBinder::FIRST_CALL_TRANSACTION,
        QVOP_REQ_MATCH,
        QVOP_REQ_MATCH_ANY,
        QVOP_REQ_DELETE_USER,
        QVOP_REQ_DELETE_ALL,
        QVOP_REQ_CANCEL,
        QVOP_REQ_CONFIG,
        QVOP_REQ_VERSION,
        QVOP_REQ_ADD_FRAME,
        QVOP_REQ_ADD_FRAME_WITH_ID,
        QVOP_REQ_DELETE_USER_WITH_ID,
        QVOP_REQ_MATCH_ANY_SECURE,
        QVOP_REQ_MATCH_ANY_KEYPHRASE,
        QVOP_REQ_MATCH_SECURE,
        QVOP_REQ_MATCH_WITH_ID,
        QVOP_REQ_START_AUDIO_CAPTURE,
        QVOP_REQ_STOP_AUDIO_CAPTURE,
        QVOP_REQ_ENROLL_CAPTURE_START,
        QVOP_REQ_ENROLL_CAPTURE_COMPLETE,
        QVOP_REQ_ENROLL_COMMIT,
        QVOP_REQ_MATCH_CAPTURE_COMPLETE,
        QVOP_REQ_RENAME,
        QVOP_REQ_SET_THRESHOLD,
    };

    // response IDs
    enum QvopCallbackCodes {
        QVOP_RES_ERROR = IBinder::FIRST_CALL_TRANSACTION,
        QVOP_RES_ENROLL,
        QVOP_RES_MATCH_FOUND,
        QVOP_RES_MATCH_NOT_FOUND,
        QVOP_RES_DELETE_USER,
        QVOP_RES_DELETE_ID,
        QVOP_RES_DELETE_ALL,
        QVOP_RES_CONFIG,
        QVOP_RES_VERSION,
        QVOP_RES_RENAME,
        QVOP_RES_SET_THRESHOLD,
    };

    class IQvopCallback : public IInterface {
    public:
        DECLARE_META_INTERFACE(QvopCallback);

        // On any error
        virtual void onError(int32_t error) = 0;

        // On a match or match any successful search
        virtual void onMatchFound(const char* user_id, const char* identifier, int32_t index, double sentence_score, double ver_score, float spoofScore) = 0;

        // On a match or match any failed
        virtual void onMatchNotFound(int32_t error, int32_t index, double sentence_score, double ver_score, float spoofScore) = 0;

        // Enrollment was successful
        virtual void onEnrollStatus(int32_t status) = 0;

        // Delete status - user/identifier, depends on request
        virtual void onDeleteStatus(int32_t status, const char* user_id, const char* identifier) = 0;

        // Delete all voiceprint templates status
        virtual void onDeleteAllStatus(int32_t status) = 0;

        // Version info
        virtual void onVersionInfo(long major, long minor) = 0;

        // Config setting success
        virtual void onConfigStatus(int32_t status) = 0;

        // Rename successful
        virtual void onRenameStatus(int32_t status) = 0;

    };

    class IQvopService : public IInterface {
    public:
        DECLARE_META_INTERFACE(QvopService);
        static const char* getServiceName();

        // Interface functions
        virtual QvopStatus getVersion(sp<IQvopCallback> const& callback) = 0;
        virtual QvopStatus enroll(sp<IQvopCallback> const& callback, const char* user_id, const char* identifier, const char* phrase) = 0;
        virtual QvopStatus match(sp<IQvopCallback> const& callback, const char* user_id) = 0;
        virtual QvopStatus matchWithId(sp<IQvopCallback> const& callback, const char* user_id, const char* identifier) = 0;
        virtual QvopStatus matchSecure(sp<IQvopCallback> const& callback, const char* user_id) = 0;
        virtual QvopStatus matchAny(sp<IQvopCallback> const& callback, const char* user_id) = 0;
        virtual QvopStatus matchAnyKeyPhrase(sp<IQvopCallback> const& callback, const char* user_id, const char* phrase) = 0;
        virtual QvopStatus matchAnySecure(sp<IQvopCallback> const& callback, const char* user_id) = 0;
        virtual QvopStatus deleteUser(sp<IQvopCallback> const& callback, const char* user_id) = 0;
        virtual QvopStatus deleteWithId(sp<IQvopCallback> const& callback, const char* user_id, const char* identifier) = 0;
        virtual QvopStatus deleteAll(sp<IQvopCallback> const& callback, const char* user_id) = 0;
        virtual QvopStatus processFrame(sp<IQvopCallback> const& callback, int64_t timestamp, int32_t len, int16_t const* buffer) = 0;
        virtual QvopStatus processFrameWithId(sp<IQvopCallback> const& callback, int64_t timestamp, int64_t id, int32_t len, int16_t const* buffer) = 0;
        virtual QvopStatus cancel() = 0;
        virtual QvopStatus onStartAudioCapture(sp<IQvopCallback> const& callback, int32_t sampleRate, int32_t numberOfChannels, int32_t audioFormat) = 0;
        virtual QvopStatus onStopAudioCapture(sp<IQvopCallback> const& callback) = 0;
        virtual QvopStatus enrollCaptureStart(sp<IQvopCallback> const& callback) = 0;
        virtual QvopStatus enrollCaptureComplete(sp<IQvopCallback> const& callback) = 0;
        virtual QvopStatus enrollCommit(sp<IQvopCallback> const& callback) = 0;
        virtual QvopStatus matchCaptureComplete(sp<IQvopCallback> const& callback) = 0;
        virtual QvopStatus rename(sp<IQvopCallback> const& callback, const char* user_id, const char* old_id, const char* new_id) = 0;
        virtual QvopStatus setThreshold(sp<IQvopCallback> const& callback, const char* user_id, const char* identifier, int32_t threshold) = 0;
    };

    }; // namespace android

} // extern C

namespace android {
    // backwards compatibility for L to M asBinder change
    template<sp<IBinder> (IInterface::* const T)()> sp<IBinder> QvopAsBinderCall(sp<IInterface> const& inter) {
        return inter == 0 ? 0 : (*inter.*T)();
    }

    template<sp<IBinder> (*T)(sp<IInterface> const&)> sp<IBinder> QvopAsBinderCall(sp<IInterface> const& inter) {
        return T(inter);
    }

    inline sp<IBinder> QvopAsBinder(sp<IInterface> const& inter) {
        return QvopAsBinderCall<&IInterface::asBinder>(inter);
    }
}
#endif // ANDROID_IQVOPSERVICE_H

