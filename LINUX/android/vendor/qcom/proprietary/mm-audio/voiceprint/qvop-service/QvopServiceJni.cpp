/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "QVOP"

#include <android/log.h>
#include <utils/Log.h>
#include <binder/IServiceManager.h>
#include "android_runtime/AndroidRuntime.h"

#include "jni.h"
#include "IQvopService.h"
#include "QvopCallback.h"
#include <cutils/properties.h>
#include <string>
#include <vector>

using namespace android;

#include "QvopJniHelper.h"

/**
 * The JNI binder callback.
 */
class QvopJniCallback : public BnQvopCallback {
public:

  QvopJniCallback() :
    mListenerCallback(0)
  {
    //  ALOGI("%s: QvopJniCallback()", QVOP_FN);
  }

  virtual ~QvopJniCallback() {
    // ALOGI("%s: ~QvopJniCallback()", QVOP_FN);
    setListeningCallback(0);
  }

  void setListeningCallback(jobject callback) {
    //  ALOGI("%s: setListeningCallback", QVOP_FN);

    JNIEnv* env = android::AndroidRuntime::getJNIEnv();

    // remove existing callback
    if (mListenerCallback != 0) {
      env->DeleteGlobalRef(mListenerCallback);
      mListenerCallback = 0;
    }

    // Setup Java Callbacks
    if (callback != 0) {
      mListenerCallback = env->NewGlobalRef(callback);
      jclass cls = env->GetObjectClass(mListenerCallback);

      mListenerOnResult = env->GetMethodID(cls, "onResult", "(I)V");
      if ( NULL == mListenerOnResult ) {
        ALOGE("mListenerOnResult is NULL. Method may not exist.");
      }

      mListenerOnError = env->GetMethodID(cls, "onError", "(I)V");
      if ( NULL == mListenerOnError ) {
        ALOGE("mListenerOnError is NULL. Method may not exist.");
      }

      mListenerOnMatchStatus = env->GetMethodID(cls, "onMatchStatus", "(ILjava/lang/String;Ljava/lang/String;IDDF)V");
      if ( NULL == mListenerOnMatchStatus ) {
        ALOGE("mListenerOnMatchStatus is NULL. Method may not exist.");
      }

      mListenerOnEnrollStatus = env->GetMethodID(cls, "onEnrollStatus", "(I)V");
      if ( NULL == mListenerOnEnrollStatus ) {
        ALOGE("mListenerOnEnrollStatus is NULL. Method may not exist.");
      }

      mListenerOnDeleteStatus = env->GetMethodID(cls, "onDeleteStatus", "(ILjava/lang/String;Ljava/lang/String;)V");
      if ( NULL == mListenerOnDeleteStatus ) {
        ALOGE("mListenerOnDeleteStatus is NULL. Method may not exist.");
      }

      mListenerOnVersionInfo = env->GetMethodID(cls, "onVersionInfo", "(JJ)V");
      if ( NULL == mListenerOnVersionInfo ) {
        ALOGE("mListenerOnVersionInfo is NULL. Method may not exist.");
      }

      mListenerOnConfigUpdateStatus = env->GetMethodID(cls, "onConfigUpdateStatus", "(I)V");
      if ( NULL == mListenerOnConfigUpdateStatus ) {
        ALOGE("mListenerOnConfigUpdateStatus is NULL. Method may not exist.");
      }

      mListenerOnRenameStatus = env->GetMethodID(cls, "onRenameStatus", "(I)V");
      if ( NULL == mListenerOnRenameStatus ) {
        ALOGE("mListenerOnRenameStatus is NULL. Method may not exist.");
      }

    }
  }
  virtual void onResult(int32_t result) {
    ALOGI("%s: onResult %d", QVOP_FN, result);

    if (mListenerCallback != 0 && mListenerOnResult != NULL) {
      JNIEnv* env = android::AndroidRuntime::getJNIEnv();
      env->CallVoidMethod(mListenerCallback, mListenerOnResult, result);
    }

  }

  virtual void onError(int32_t error) {
    ALOGI("%s: onError %d", QVOP_FN, error);

    if (mListenerCallback != 0 && mListenerOnError != NULL) {
      JNIEnv* env = android::AndroidRuntime::getJNIEnv();
      env->CallVoidMethod(mListenerCallback, mListenerOnError, error);
    }
  }

  // On a match or match any successful search
  virtual void onMatchFound(const char* user_id, const char* identifier,
                            int32_t index, double sentence_score, double user_score, float spoofScore) {
    ALOGI("%s: onMatchFound user=%s, id=%s, index=%d, sentence_score=%lf, user_score=%lf, spoofScore=%f", QVOP_FN,
          user_id, identifier, index, sentence_score, user_score, spoofScore);

    if (mListenerCallback != 0 && mListenerOnMatchStatus != NULL) {
      JNIEnv* env = android::AndroidRuntime::getJNIEnv();
      jstring userStr = env->NewStringUTF(user_id);
      jstring identifierStr = env->NewStringUTF(identifier);
      env->CallVoidMethod(mListenerCallback,
                          mListenerOnMatchStatus,
                          QVOP_SUCCESS,
                          userStr,
                          identifierStr,
                          index,
                          sentence_score,
                          user_score,
                          spoofScore);
      env->DeleteLocalRef(userStr);
      env->DeleteLocalRef(identifierStr);
    }
  }

  // On a match or match any failed
  virtual void onMatchNotFound(int32_t error, int32_t index, double sentence_score, double user_score, float spoofScore)  {
    ALOGI("%s: onMatchNotFound, error=%d, index=%d, sentence_score=%lf, user_score=%lf, spoofScore=%f", QVOP_FN, error,
          index, sentence_score, user_score, spoofScore);

    if (mListenerCallback != 0 && mListenerOnMatchStatus != NULL) {
      JNIEnv* env = android::AndroidRuntime::getJNIEnv();
      env->CallVoidMethod(mListenerCallback,
                          mListenerOnMatchStatus,
                          error,
                          NULL,
                          NULL,
                          index,
                          sentence_score,
                          user_score,
                          spoofScore);
    }
  }


  // Enrollment status
  virtual void onEnrollStatus(int32_t status) {
    ALOGI("%s: status=%d", QVOP_FN, status);

    if (mListenerCallback != 0 && mListenerOnEnrollStatus != NULL) {
      JNIEnv* env = android::AndroidRuntime::getJNIEnv();
      env->CallVoidMethod(mListenerCallback, mListenerOnEnrollStatus, status);
    }
  }


  // Delete Successful - identifier
  virtual void onDeleteStatus(int32_t status, const char* user_id, const char* identifier) {
    ALOGI("%s: onDeleteSuccess user=%s, id=%s", QVOP_FN, user_id, identifier);

    if (mListenerCallback != 0 && mListenerOnDeleteStatus != NULL) {
      JNIEnv* env = android::AndroidRuntime::getJNIEnv();
      jstring userStr = env->NewStringUTF(user_id);
      jstring identifierStr = env->NewStringUTF(identifier);
      env->CallVoidMethod(mListenerCallback, mListenerOnDeleteStatus, status, userStr, identifierStr);
      env->DeleteLocalRef(userStr);
      env->DeleteLocalRef(identifierStr);
    }
  }

  // Delete Successful - identifier
  virtual void onDeleteAllStatus(int32_t status) {
    ALOGI("%s: onDeleteAllSuccess", QVOP_FN);

    if (mListenerCallback != 0 && mListenerOnDeleteStatus != NULL) {
      JNIEnv* env = android::AndroidRuntime::getJNIEnv();
      env->CallVoidMethod(mListenerCallback, mListenerOnDeleteStatus, status, NULL, NULL);
    }
  }



  // Version info
  virtual void onVersionInfo(long major, long minor) {
    ALOGI("%s: onVersionInfo", QVOP_FN);
    if (mListenerCallback != 0 && mListenerOnVersionInfo != NULL) {
      JNIEnv* env = android::AndroidRuntime::getJNIEnv();

      env->CallVoidMethod(mListenerCallback, mListenerOnVersionInfo, major, minor);

    }
  }


  // Config setting success
  virtual void onConfigStatus(int32_t status) {
    ALOGI("%s: onConfigSuccess", QVOP_FN);

    if (mListenerCallback != 0 && mListenerOnConfigUpdateStatus != NULL) {
      JNIEnv* env = android::AndroidRuntime::getJNIEnv();
      env->CallVoidMethod(mListenerCallback, mListenerOnConfigUpdateStatus, status);
    }
  }

  // Rename successful
  virtual void onRenameStatus(int32_t status) {
    ALOGI("%s: ", QVOP_FN);

    if (mListenerCallback != 0 && mListenerOnRenameStatus != NULL) {
      JNIEnv* env = android::AndroidRuntime::getJNIEnv();
      env->CallVoidMethod(mListenerCallback, mListenerOnRenameStatus, status);
    }
  }


private:

  // listener callbacks
  jobject mListenerCallback;
  jmethodID mListenerOnResult;
  jmethodID mListenerOnError;
  jmethodID mListenerOnMatchStatus;
  jmethodID mListenerOnEnrollStatus;
  jmethodID mListenerOnDeleteStatus;
  jmethodID mListenerOnConfigUpdateStatus;
  jmethodID mListenerOnRenameStatus;
  jmethodID mListenerOnVersionInfo;

};

/**
 * The JNI context
 */
struct QvopJniContext {
  sp<IQvopService> mService;

  QvopJniContext(JNIEnv* env, sp<IQvopService> const& service) :
    mService(service)
  {
    ALOGE("%s: ", QVOP_FN);
  }

  virtual ~QvopJniContext() {
    ALOGE("%s: ", QVOP_FN);
  }
};

/**
 * Get the context.
 */
static QvopJniContext* GetContext(jlong context) {
  // the context is a pointer to QvopJniContext
  return reinterpret_cast<QvopJniContext*>(context);
}

/**
 * Get the service interface from the context.
 */
static sp<IQvopService> const& GetService(jlong context) {
  return GetContext(context)->mService;
}

extern "C"
{

  /**
   * Public API
   */

  JNIEXPORT jlong JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_open(
    JNIEnv* env, jobject thiz) {
    ALOGI("nativeOpen");

    // connect to service
    sp<IQvopService> service;
    status_t res = getService(String16(IQvopService::getServiceName()), &service);
    if (res != NO_ERROR) {
      // service connect failed
      ALOGE("nativeConnect failed to connect to %s %d", IQvopService::getServiceName(), res);
      return 0;
    }

    ALOGE("nativeConnect success, connected to %s", IQvopService::getServiceName());

    // the context is a pointer to QvopJniContext
    QvopJniContext* context = new QvopJniContext(env, service);
    ALOGI("context=%p", context);

    return reinterpret_cast<jlong>(context);
  }

  JNIEXPORT void JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_close(
    JNIEnv* env, jobject thiz, jlong context) {
    ALOGI("nativeClose");

    delete GetContext(context);
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_shutdown(
    JNIEnv* env,
    jobject thiz,
    jlong context) {
    ALOGI("nativeShutdown");

    jint rc = NO_ERROR;

    // jint rc = GetService(context)->shutdown();
    if (rc != NO_ERROR) {
      ALOGE("nativeShutdown error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_enroll(
    JNIEnv* env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jstring user,
    jstring identifier,
    jstring phrase) {
    ALOGI("nativeEnroll");

    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->enroll(jnicb,
                                          GetJavaString(env, user),
                                          GetJavaString(env, identifier),
                                          GetJavaString(env, phrase ));

    if (rc != QVOP_SUCCESS) {
      ALOGE("nativeEnroll error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_setThreshold(
    JNIEnv* env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jstring user,
    jstring identifier,
    jint threshold) {
    ALOGI("nativeSetThreshold");

    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->setThreshold(jnicb,
              GetJavaString(env, user),
              GetJavaString(env, identifier),
              threshold);

    if (rc != QVOP_SUCCESS) {
      ALOGE("nativeSetThreshold error %d", rc);
    }

    return rc;
  }


  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_match(
    JNIEnv* env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jstring user) {
    ALOGI("nativeMatch");

    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);
    ALOGI("match call");
    jint rc = GetService(context)->match(jnicb,
                                         GetJavaString(env, user));

    ALOGI("match exit");
    if (rc != NO_ERROR) {
      ALOGE("nativeMatch error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_matchWithId(
    JNIEnv* env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jstring user,
    jstring identifier) {
    ALOGI("nativeMatchWithId");
    ALOGI("context=%d", context);
    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);
    ALOGI("matchWithId call");
    jint rc = GetService(context)->matchWithId(jnicb,
              GetJavaString(env, user),
              GetJavaString(env, identifier));

    ALOGI("matchWithId exit");
    if (rc != NO_ERROR) {
      ALOGE("nativeMatchWithId error %d", rc);
    }

    return rc;
  }


  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_matchSecure(
    JNIEnv* env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jstring user) {
    ALOGI("nativeMatchSecure");

    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->matchSecure(jnicb,
              GetJavaString(env, user));


    if (rc != NO_ERROR) {
      ALOGE("nativeMatchSecure error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_matchAny(
    JNIEnv* env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jstring user) {
    ALOGI("nativeMatchAny");

    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->matchAny(jnicb,
                                            GetJavaString(env, user));


    if (rc != NO_ERROR) {
      ALOGE("nativeMatchAny error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_matchAnyKeyPhrase(
    JNIEnv* env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jstring user,
    jstring phrase) {
    ALOGI("nativeMatchAnyKeyPhrase");

    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->matchAnyKeyPhrase(jnicb,
              GetJavaString(env, user),
              GetJavaString(env, phrase));

    if (rc != NO_ERROR) {
      ALOGE("nativeMatchAnyKeyPhrase error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_matchAnySecure(
    JNIEnv* env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jstring user) {
    ALOGI("nativeMatchAnySecure");

    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->matchAnySecure(jnicb,
              GetJavaString(env, user));


    if (rc != NO_ERROR) {
      ALOGE("nativeMatchAnySecure error %d", rc);
    }

    return rc;
  }


  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_cancel(
    JNIEnv* env,
    jobject thiz,
    jlong context) {
    ALOGI("nativeCancel");

    jint rc = GetService(context)->cancel();
    if (rc != NO_ERROR) {
      ALOGE("nativeCancel error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_delete(
    JNIEnv* env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jstring user) {
    ALOGI("nativeDelete");

    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    // remove all voiceprint templates for user
    jint rc = GetService(context)->deleteUser(jnicb,
              GetJavaString(env, user));

    if (rc != NO_ERROR) {
      ALOGE("nativeDelete error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_deleteWithId(
    JNIEnv* env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jstring user,
    jstring identifier) {
    ALOGI("nativeDeleteWithId");

    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    // remove all voiceprint templates for user
    jint rc = GetService(context)->deleteWithId(jnicb,
              GetJavaString(env, user),
              GetJavaString(env, identifier));

    if (rc != NO_ERROR) {
      ALOGE("nativeDeleteWithId error %d", rc);
    }

    return rc;
  }


  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_deleteAll(
    JNIEnv* env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jstring user) {
    ALOGI("nativeDeleteAll");

    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    // remove all voiceprint templates for user
    jint rc = GetService(context)->deleteAll(jnicb,
              GetJavaString(env, user));

    if (rc != NO_ERROR) {
      ALOGE("nativeDeleteAll error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_processFrame(
    JNIEnv *env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jlong timeStamp,
    jint bufferLen,
    jshortArray frame)
  {
    ALOGV("nativeProcessFrame bufferLen=" + bufferLen);
    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jshort* buffer = env->GetShortArrayElements(frame, 0);

#ifdef QVOP_DEBUG
    ALOGI("Frame[0]: %d", buffer[0]);
    ALOGI("Frame[1]: %d", buffer[1]);
    ALOGI("Frame[%d]: %d", bufferLen - 1, buffer[bufferLen - 1]);
#endif
    jint rc = GetService(context)->processFrame(jnicb,
              timeStamp, bufferLen, reinterpret_cast<int16_t const*>(buffer));

    env->ReleaseShortArrayElements(frame, buffer, JNI_ABORT /* no copy-back */);

    if (rc != NO_ERROR) {
      ALOGE("nativeProcessFrame error %d", rc);
    }

    return rc;
  }


  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_processFrameWithId(
    JNIEnv *env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jlong timeStamp,
    jlong frameId,
    jint bufferLen,
    jshortArray frame)
  {
    ALOGV("nativeProcessFrameWithId bufferLen=" + bufferLen);
    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);


    jshort* buffer = env->GetShortArrayElements(frame, 0);
#ifdef QVOP_DEBUG
    ALOGI("FrameId: %d", frameId);
    ALOGI("Frame[0]: %d", buffer[0]);
    ALOGI("Frame[1]: %d", buffer[1]);
    ALOGI("Frame[%d]: %d", bufferLen, buffer[bufferLen - 1]);
#endif

    jint rc = GetService(context)->processFrameWithId(jnicb,
              timeStamp, frameId, bufferLen,  reinterpret_cast<int16_t const*>(buffer));

    env->ReleaseShortArrayElements(frame, buffer, JNI_ABORT /* no copy-back */);

    if (rc != NO_ERROR) {
      ALOGE("nativeProcessFrameWithId error %d", rc);
    }

    return rc;
  }


  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_onStartAudioCapture(
    JNIEnv *env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jint sampleRate,
    jint numberOfChannels,
    jint audioFormat)
  {
    ALOGV("nativeOnStartAudioCapture");
    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->onStartAudioCapture(jnicb,
              sampleRate, numberOfChannels, audioFormat);

    if (rc != NO_ERROR) {
      ALOGE("nativeOnStartAudioCapture error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_onStopAudioCapture(
    JNIEnv *env,
    jobject thiz,
    jlong context,
    jobject receiver)
  {
    ALOGV("nativeOnStopAudioCapture");
    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->onStopAudioCapture(jnicb);

    if (rc != NO_ERROR) {
      ALOGE("nativeOnStopAudioCapture error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_enrollCaptureStart(
    JNIEnv *env,
    jobject thiz,
    jlong context,
    jobject receiver)
  {
    ALOGV("nativeEnrollCaptureStart");
    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->enrollCaptureStart(jnicb);

    if (rc != NO_ERROR) {
      ALOGE("nativeEnrollCaptureStart error %d", rc);
    }

    return rc;
  }


  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_enrollCaptureComplete(
    JNIEnv *env,
    jobject thiz,
    jlong context,
    jobject receiver)
  {
    ALOGV("nativeEnrollCaptureComplete");
    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->enrollCaptureComplete(jnicb);

    if (rc != NO_ERROR) {
      ALOGE("nativeEnrollCaptureComplete error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_enrollCommit(
    JNIEnv *env,
    jobject thiz,
    jlong context,
    jobject receiver)
  {
    ALOGV("nativeEnrollCommit");
    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->enrollCommit(jnicb);

    if (rc != NO_ERROR) {
      ALOGE("nativeEnrollCommit error %d", rc);
    }

    return rc;
  }

  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_matchCaptureComplete(
    JNIEnv *env,
    jobject thiz,
    jlong context,
    jobject receiver)
  {
    ALOGV("nativeMatchCaptureComplete");
    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->matchCaptureComplete(jnicb);

    if (rc != NO_ERROR) {
      ALOGE("nativeMatchCaptureComplete error %d", rc);
    }

    return rc;
  }


  JNIEXPORT jint JNICALL
  Java_com_qualcomm_qti_biometrics_voiceprint_service_Native_rename(
    JNIEnv *env,
    jobject thiz,
    jlong context,
    jobject receiver,
    jstring user,
    jstring old_id,
    jstring new_id
  )
  {
    ALOGV("nativeRename");
    sp<QvopJniCallback> jnicb = new QvopJniCallback();
    jnicb->setListeningCallback(receiver);

    jint rc = GetService(context)->rename(jnicb,
                                          GetJavaString(env, user),
                                          GetJavaString(env, old_id),
                                          GetJavaString(env, new_id));

    if (rc != NO_ERROR) {
      ALOGE("nativeRename error %d", rc);
    }

    return rc;
  }


#ifdef QVOP_DEBUG

// Add any debug APIs here

#endif


} // extern "C"


