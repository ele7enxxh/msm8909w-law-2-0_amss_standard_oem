/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#define LOG_TAG "IzatProviderEngine_jni"
#define LOG_NDEBUG 0

#include <string>
#include <jni.h>
#include <JNIHelp.h>
#include <AndroidRuntime.h>
#include <utils/Log.h>
#include <log_util.h>
#include <IzatTypes.h>
#include <IzatLocation.h>
#include <FusedLocationProviderGlue.h>
#include <NetworkLocationProviderGlue.h>
#include <IzatRequest.h>
#include <map>

using namespace android;
using namespace std;
using namespace izat_manager;

static jobject callBackJavaObj = NULL;

static jmethodID method_onLocationChanged;
static jmethodID method_onStatusChanged;

// Provider status lookup table
static const int providerStatusLookUpTable [] = {
    IZAT_PROVIDER_OUT_OF_SERVICE,
    IZAT_PROVIDER_TEMPORARILY_UNAVAILABLE,
    IZAT_PROVIDER_AVAILABLE,
    IZAT_PROVIDER_GNSS_STATUS_NONE,
    IZAT_PROVIDER_GNSS_STATUS_SESSION_BEGIN,
    IZAT_PROVIDER_GNSS_STATUS_SESSION_END,
    IZAT_PROVIDER_GNSS_STATUS_ENGINE_ON,
    IZAT_PROVIDER_GNSS_STATUS_ENGINE_OFF
};


static const char * FUSED = "fused";
static const char * NETWORK = "network";
static const char * GPS = "gps";
// Whenever a provider type is added or deleted from IzatTypes.h
// this array must be modified to reflect the changes.
// The names here are from location manager
static map<IzatLocationStreamType, const char *> providerNameLookUpTable = {
    { IZAT_STREAM_FUSED, FUSED },
    { IZAT_STREAM_NETWORK, NETWORK },
    { IZAT_STREAM_GNSS, GPS }
};


static const int ACC_FINE  = 100;
static const int ACC_BLOCK = 102;
static const int ACC_CITY  = 104;

static const int PWR_HIGH  = 203;
static const int PWR_LOW   = 201;
static const int PWR_NONE  = 200;

// Helpers

static void createIzatRequest
(
    JNIEnv *env,
    jobject obj,
    IzatRequest * request,
    jstring providerName,
    jint numUpdates,
    jlong interval,
    jfloat smallestDisplacement,
    jint accuracy
)
{
    if (request == NULL) return;

    const char * rc = env->GetStringUTFChars (providerName, NULL);

    string pn (rc);

    IzatLocationStreamType providerType =
        (0 == pn.compare (NETWORK))
        ? (IZAT_STREAM_NETWORK)
        : (IZAT_STREAM_FUSED);

    IzatHorizontalAccuracy izatAcc;

    if ( (accuracy == ACC_FINE) || (accuracy == PWR_HIGH) ) {
        izatAcc = IZAT_HORIZONTAL_FINE;
    }
    else if ( (accuracy == ACC_BLOCK) || (accuracy == PWR_LOW) ) {
        izatAcc = IZAT_HORIZONTAL_BLOCK;
    }
    else {
        izatAcc = IZAT_HORIZONTAL_NONE;
    }

    request->setProvider (providerType);
    request->setNumUpdates (numUpdates);
    request->setInterval (interval);
    request->setDistance (smallestDisplacement);
    request->setHorizontalAccuracy (izatAcc);

    env->ReleaseStringUTFChars (providerName, rc);
}

// Java to native calls
static void onJavaClassLoad (JNIEnv* env, jclass clazz) {

    ALOGD ("%s : %s", "Entering ", __func__);

    method_onLocationChanged =
    env->GetMethodID
    (
        clazz,
        "onLocationChanged",
        "(Ljava/lang/String;JJDDZDZFZFZF)V"
    );

    method_onStatusChanged =
    env->GetMethodID
    (
        clazz,
        "onStatusChanged",
        "(Ljava/lang/String;I)V"
    );
}

static void onJavaInstanceInit (JNIEnv *env, jobject obj) {

    ALOGD ("%s : %s", "Entering ", __func__);

    // cache the Java callback object
    if (!callBackJavaObj)
        callBackJavaObj = env->NewGlobalRef (obj);
}

static void onJavaInstanceDeinit (JNIEnv *env, jobject obj) {

    ALOGD ("%s : %s", "Entering ", __func__);

    NetworkLocationProviderGlue :: destroyInstance ();
    FusedLocationProviderGlue :: destroyInstance ();

    // clear Global References if any
    if (callBackJavaObj) {
        env->DeleteGlobalRef (callBackJavaObj);
    }
    callBackJavaObj = NULL;
}

static void onEnable (JNIEnv * env, jobject obj, jstring providerName) {
    ALOGD ("%s : %s", "Entering ", __func__);
    const char * rc = env->GetStringUTFChars (providerName, NULL);
    string pn (rc);
    ALOGD ("onEnable called by %s",pn.c_str ());

#ifndef ON_TARGET_UNIT_TEST

    if (0 == pn.compare (NETWORK)) {

        NetworkLocationProviderGlue * p =
            NetworkLocationProviderGlue :: getInstance ();
        p->onEnable ();
    }
    else {
        FusedLocationProviderGlue * p =
            FusedLocationProviderGlue :: getInstance ();
        p->onEnable ();
    }

#endif
    env->ReleaseStringUTFChars (providerName, rc);
}

static void onDisable (JNIEnv *env, jobject obj, jstring providerName) {
    const char * rc = env->GetStringUTFChars (providerName, NULL);
    string pn (rc);
    ALOGD ("onDisable called by %s", pn.c_str ());

#ifndef ON_TARGET_UNIT_TEST

    if (0 == pn.compare (NETWORK)) {
        NetworkLocationProviderGlue * p =
            NetworkLocationProviderGlue :: getInstance ();
        p->onDisable ();
    }
    else {
        FusedLocationProviderGlue * p =
            FusedLocationProviderGlue :: getInstance ();
        p->onDisable ();
    }
#endif
    env->ReleaseStringUTFChars (providerName, rc);
}


static void onAddRequest
(
    JNIEnv * env,
    jobject obj,
    jstring providerName,
    jint numUpdates,
    jlong interval,
    jfloat smallestDisplacement,
    jint accuracy
)
{
    IzatRequest request;
    createIzatRequest
    (
        env,
        obj,
        &request,
        providerName,
        numUpdates,
        interval,
        smallestDisplacement,
        accuracy
    );


    const char * rc = env->GetStringUTFChars (providerName, NULL);

    string pn (rc);

    ALOGD ("onAddRequest called by %s", pn.c_str ());
    ALOGD ("Provider Type: %d", request.getProvider ());
    ALOGD ("Num Updates : %d", request.getNumUpdates ());
    ALOGD ("Interval : %d", request.getInterval ());
    ALOGD ("Displacement : %f", request.getDistance ());
    ALOGD ("Accuracy : %d", request.getHorizontalAccuracy ());

#ifndef ON_TARGET_UNIT_TEST

    if (0 == pn.compare(NETWORK)) {
        NetworkLocationProviderGlue * p =
            NetworkLocationProviderGlue :: getInstance ();
        p->onAddRequest (&request);
    }
    else {
        FusedLocationProviderGlue * p =
            FusedLocationProviderGlue :: getInstance ();
        p->onAddRequest (&request);
    }

#endif
    env->ReleaseStringUTFChars (providerName, rc);
}

static void onRemoveRequest
(
    JNIEnv *env,
    jobject obj,
    jstring providerName,
    jint numUpdates,
    jlong interval,
    jfloat smallestDisplacement,
    jint accuracy
)
{
    IzatRequest request;
    createIzatRequest
    (
        env,
        obj,
        &request,
        providerName,
        numUpdates,
        interval,
        smallestDisplacement,
        accuracy
    );

    const char * rc = env->GetStringUTFChars (providerName, NULL);
    string pn (rc);

    ALOGD ("onRemoveRequest called by %s", pn.c_str ());
    ALOGD ("Provider Type: %d", request.getProvider ());
    ALOGD ("Num Updates : %d", request.getNumUpdates ());
    ALOGD ("Interval : %d", request.getInterval ());
    ALOGD ("Displacement : %f", request.getDistance ());
    ALOGD ("Accuracy : %d", request.getHorizontalAccuracy ());

#ifndef ON_TARGET_UNIT_TEST

    if (0 == pn.compare(NETWORK)) {
        NetworkLocationProviderGlue * p =
            NetworkLocationProviderGlue :: getInstance ();
        p->onRemoveRequest (&request);
    }
    else {
        FusedLocationProviderGlue * p =
            FusedLocationProviderGlue :: getInstance ();
        p->onRemoveRequest (&request);
    }
#endif
    env->ReleaseStringUTFChars (providerName, rc);
}


// native to java calls
void onLocationChangedJNI
(
    const IzatLocation * location,
    IzatLocationStreamType type
)
{
    JNIEnv* env = android::AndroidRuntime::getJNIEnv ();

    if (env == 0) return;

    if (location == NULL) return;

    if (!const_cast<IzatLocation *> (location)->isValid ()) return;

    jlong utcTime = location->mUtcTimestampInMsec;
    jlong elapsedRealTimeNanos = location->mElapsedRealTimeInNanoSecs;
    jdouble latitude = location->mLatitude;
    jdouble longitude = location->mLongitude;
    jboolean hasAltitude = location->mHasAltitudeWrtEllipsoid;
    jdouble altitude = location->mAltitudeWrtEllipsoid;
    jboolean hasSpeed = location->mHasSpeed;
    jfloat speed = location->mSpeed;
    jboolean hasBearing = location->mHasBearing;
    jfloat bearing = location->mBearing;
    jboolean hasAccuracy = location->mHasHorizontalAccuracy;
    jfloat accuracy = location->mHorizontalAccuracy;

    jstring javaProviderName =
    env->NewStringUTF
    (
        providerNameLookUpTable [type]
    );

    if ( (callBackJavaObj) && (method_onLocationChanged) ) {
        env->CallVoidMethod
        (
            callBackJavaObj,
            method_onLocationChanged,
            javaProviderName,
            utcTime,
            elapsedRealTimeNanos,
            latitude,
            longitude,
            hasAltitude,
            altitude,
            hasSpeed,
            speed,
            hasBearing,
            bearing,
            hasAccuracy,
            accuracy
        );
    }
    env->DeleteLocalRef (javaProviderName);
}

void onStatusChangedJNI
(
    const IzatProviderStatus status,
    IzatLocationStreamType type
)
{
    JNIEnv* env = android::AndroidRuntime::getJNIEnv ();

    if (env == 0) return;

    jstring javaProviderName =
    env->NewStringUTF
    (
        providerNameLookUpTable [type]
    );

    ALOGD ("onStatusChangedJNI :: status : %d", status);

    if ( (callBackJavaObj) && (method_onStatusChanged) ) {
        env->CallVoidMethod
        (
            callBackJavaObj,
            method_onStatusChanged,
            javaProviderName,
            providerStatusLookUpTable [static_cast <int> (status)]
        );
    }

    env->DeleteLocalRef (javaProviderName);
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"nativeOnClassLoad", "()V", (void *) onJavaClassLoad},
    {"nativeOnInstanceInit", "()V", (void *) onJavaInstanceInit},
    {"nativeOnInstanceDeinit", "()V", (void *) onJavaInstanceDeinit},
    {"nativeOnEnableProvider","(Ljava/lang/String;)V", (void *) onEnable},
    {"nativeOnDisableProvider","(Ljava/lang/String;)V", (void *) onDisable},
    {"nativeOnAddRequest","(Ljava/lang/String;IJFI)V", (void *) onAddRequest},
    {"nativeOnRemoveRequest", "(Ljava/lang/String;IJFI)V", (void *) onRemoveRequest}
};


int register_IzatProviderEngine (JNIEnv* env) {

    ALOGD ("register_IzatProviderEngine");
    return jniRegisterNativeMethods
    (
        env,
        "com/qualcomm/location/izatserviceprovider/provider/IzatProviderEngine",
        sMethods, NELEM (sMethods)
    );
}

