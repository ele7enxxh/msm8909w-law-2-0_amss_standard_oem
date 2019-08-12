/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


#define LOG_TAG "OSNPGlue_jni"
#define LOG_NDEBUG 0

#include <string>
#include <jni.h>
#include <JNIHelp.h>
#include <AndroidRuntime.h>
#include <utils/Log.h>
#include <log_util.h>
#include <IzatTypes.h>
#include <IzatRequest.h>
#include <IzatLocation.h>
#include <OSNPGlue.h>

using namespace std;
using namespace android;
using namespace izat_manager;

static jobject callBackJavaObj = NULL;

static jmethodID method_onInit;
static jmethodID method_onDestroy;
static jmethodID method_onSetRequest;
static jmethodID method_onStopRequest;

// Whenever a provider type is added or deleted from IzatTypes.h
// this array must be modified to reflect the changes.
// The names here are from location manager
static char * providerNameLookUpTable [] = {"fused", "network", "gps"};

// IzatTypes to LocationRequest horizontal accuracy to quality mapping table
static int qualityLookUpTable [] = {100, 102, 104, 201};

// Provider status lookup
static IzatProviderStatus providerStatusLookUpTable [] =
{
    IZAT_PROVIDER_OUT_OF_SERVICE,
    IZAT_PROVIDER_TEMPORARILY_UNAVAILABLE,
    IZAT_PROVIDER_AVAILABLE
};


// Called during class load time
static void onJavaClassLoad (JNIEnv* env, jclass clazz) {

    ALOGV ("%s : %s", "Entering ", __func__);

    method_onInit =
    env->GetMethodID
    (
        clazz,
        "onInit",
        "()Z"
    );

    method_onDestroy =
    env->GetMethodID
    (
        clazz,
        "onDestroy",
        "()V"
    );

    method_onSetRequest =
    env->GetMethodID
    (
        clazz,
        "onSetRequest",
        "(Ljava/lang/String;JFII)V"
    );

    method_onStopRequest =
    env->GetMethodID
    (
        clazz,
        "onStopRequest",
        "()V"
    );
}

// Called when java object is instantiated
static void onJavaInstanceInit (JNIEnv *env, jobject obj) {

    ALOGV ("%s : %s", "Entering ", __func__);

    // cache the Java callback object
    if (!callBackJavaObj)
        callBackJavaObj = env->NewGlobalRef (obj);
}

// Called when java object is destroyed
static void onJavaInstanceDeinit (JNIEnv *env, jobject obj) {

    ALOGV ("%s : %s", "Entering ", __func__);

    // clear Global References if any
    env->DeleteGlobalRef (callBackJavaObj);

    callBackJavaObj = NULL;
}

// java to native calls
static void onJavaLocationChanged
(
    JNIEnv * env,
    jobject obj,
    jboolean hasTime,
    jlong utcTime,
    jboolean hasElapsedRealTimeNanos,
    jlong elapsedRealTimeNanos,
    jboolean hasLatitude,
    jdouble latitude,
    jboolean hasLongitude,
    jdouble longitude,
    jboolean hasAccuracy,
    jfloat accuracy,
    jboolean hasAltitude,
    jdouble altitude,
    jboolean hasBearing,
    jfloat bearing,
    jboolean hasSpeed,
    jfloat speed
) {
    ALOGV ("%s : %s", "Entering ", __func__);

    OSNPGlue * osnpGlue = OSNPGlue :: getInstance ();

    if (osnpGlue) {
        IzatLocation izatLocation;

        izatLocation.mHasUtcTimestampInMsec = hasTime;
        izatLocation.mHasElapsedRealTimeInNanoSecs = hasElapsedRealTimeNanos;
        izatLocation.mHasLatitude = hasLatitude;
        izatLocation.mHasLongitude = hasLongitude;
        izatLocation.mHasHorizontalAccuracy = hasAccuracy;
        izatLocation.mHasAltitudeWrtEllipsoid = hasAltitude;
        izatLocation.mHasBearing = hasBearing;
        izatLocation.mHasSpeed = hasSpeed;

        izatLocation.mUtcTimestampInMsec = utcTime;
        izatLocation.mElapsedRealTimeInNanoSecs = elapsedRealTimeNanos;
        izatLocation.mLatitude = latitude;
        izatLocation.mLongitude = longitude;
        izatLocation.mHorizontalAccuracy = accuracy;
        izatLocation.mAltitudeWrtEllipsoid = altitude;
        izatLocation.mBearing = bearing;
        izatLocation.mSpeed = speed;

        osnpGlue->onLocationChanged (&izatLocation);
    }
}

static void onJavaStatusChanged (JNIEnv * env, jobject obj, jint status) {

    ALOGV ("%s : %s", "Entering ", __func__);

    OSNPGlue * osnpGlue = OSNPGlue :: getInstance ();
    if (osnpGlue) {
        osnpGlue->onStatusChanged (providerStatusLookUpTable [status]);
    }
}


// native to java calls
bool onInitJNI () {

    ALOGV ("%s : %s", "Entering ", __func__);

    JNIEnv* env = android::AndroidRuntime::getJNIEnv ();

    if (env == 0) return false;

    if (callBackJavaObj) {
        return env->CallBooleanMethod (callBackJavaObj, method_onInit);
    }
    return false;
}

void onDestroyJNI () {
    ALOGV ("%s : %s", "Entering ", __func__);

    JNIEnv* env = android::AndroidRuntime::getJNIEnv ();

    if (env == 0) return;

    if (callBackJavaObj) {
        env->CallVoidMethod
        (
            callBackJavaObj,
            method_onDestroy
        );
    }
}

void onSetRequestJNI (IzatRequest * request) {
    ALOGV ("%s : %s", "Entering ", __func__);

    if (request == 0) return;

    JNIEnv* env = android::AndroidRuntime::getJNIEnv ();

    if (env == 0) return;

    jstring javaProviderName =
    env->NewStringUTF
    (
        providerNameLookUpTable [static_cast <int> (request->getProvider () >> 1)]
    );

    jint quality =
    qualityLookUpTable [static_cast <int> (request->getHorizontalAccuracy ())];

    jlong interval = request->getInterval ();

    jint numUpdates = request->getNumUpdates ();

    jfloat smallestDisplacement = request->getDistance ();

    if (callBackJavaObj) {
        env->CallVoidMethod
        (
            callBackJavaObj,
            method_onSetRequest,
            javaProviderName,
            interval,
            smallestDisplacement,
            quality,
            numUpdates
        );
    }
    env->DeleteLocalRef (javaProviderName);
}

void onStopRequestJNI () {
    ALOGV ("%s : %s", "Entering ", __func__);

    JNIEnv* env = android::AndroidRuntime::getJNIEnv ();

    if (env == 0) return;

    if (callBackJavaObj) {
        env->CallVoidMethod
        (
            callBackJavaObj,
            method_onStopRequest
        );
    }
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"nativeOnClassLoad", "()V", (void *) onJavaClassLoad},
    {"nativeOnInstanceInit", "()V", (void *) onJavaInstanceInit},
    {"nativeOnInstanceDeinit", "()V", (void *) onJavaInstanceDeinit},
    {"nativeOnLocationChanged", "(ZJZJZDZDZFZDZFZF)V", (void *) onJavaLocationChanged},
    {"nativeOnStatusChanged", "(I)V", (void *) onJavaStatusChanged}
};


int register_OSNPGlue (JNIEnv* env) {
    return jniRegisterNativeMethods
    (
        env,
        "com/qualcomm/location/osnp/GNPProxy",
        sMethods, NELEM (sMethods)
    );
}

