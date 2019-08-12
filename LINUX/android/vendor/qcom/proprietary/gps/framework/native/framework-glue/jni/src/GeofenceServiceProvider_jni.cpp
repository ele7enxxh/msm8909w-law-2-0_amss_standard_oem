/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the license at
 *
 *      http://www.apache.org/license/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
 */

#define LOG_TAG "GeofenceSvcProvider_jni"
#define LOG_NDEBUG 0
#define LOCATION_CLASS_NAME "android/location/Location"

#include "JNIHelp.h"
#include "jni.h"
#include "hardware/gps.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/Log.h"
#include "GeoFencer.h"
#include "GpsGeofenceCb.h"

using namespace android;

static GeoFencer* sGeoFencer = NULL;
static jobject sCallbacksObj = NULL;

static int32_t GEOFENCE_REQUEST_TYPE_ADD = 1;
static int32_t GEOFENCE_REQUEST_TYPE_REMOVE = 2;
static int32_t GEOFENCE_REQUEST_TYPE_PAUSE = 3;
static int32_t GEOFENCE_REQUEST_TYPE_RESUME = 4;

static jmethodID method_reportGeofenceTransition = NULL;
static jmethodID method_reportGeofenceRequestStatus = NULL;
static jmethodID method_reportGeofenceEngineStatus = NULL;

static void checkAndClearExceptionFromCallback(JNIEnv* env, const char* methodName) {
    if (env->ExceptionCheck()) {
        ALOGE("An exception was thrown by callback '%s'.", methodName);
        LOGE_EX(env);
        env->ExceptionClear();
    }
}

static void TranslateToObject(const GpsLocation* location, jobject& locationObject) {
    JNIEnv* env = AndroidRuntime::getJNIEnv();
    jclass locationClass = env->FindClass(LOCATION_CLASS_NAME);
    jmethodID locationCtor = env->GetMethodID(locationClass,
                                              "<init>",
                                              "(Ljava/lang/String;)V");
    locationObject = env->NewObject(locationClass, locationCtor, NULL);
    jint flags = location->flags;

    if (flags & GPS_LOCATION_HAS_LAT_LONG) {
        jmethodID setLatitude = env->GetMethodID(locationClass,
                                                 "setLatitude",
                                                 "(D)V");
        env->CallVoidMethod(locationObject, setLatitude, location->latitude);

        jmethodID setLongitude = env->GetMethodID(locationClass,
                                                  "setLongitude",
                                                  "(D)V");
        env->CallVoidMethod(locationObject, setLongitude, location->longitude);

        jmethodID setTime = env->GetMethodID(locationClass,
                                             "setTime",
                                             "(J)V");
        env->CallVoidMethod(locationObject, setTime, location->timestamp);
    }

    if (flags & GPS_LOCATION_HAS_ALTITUDE) {
        jmethodID setAltitude = env->GetMethodID(locationClass,
                                                 "setAltitude",
                                                 "(D)V");
        env->CallVoidMethod(locationObject, setAltitude, location->altitude);
    }

    if (flags & GPS_LOCATION_HAS_SPEED) {
        jmethodID setSpeed = env->GetMethodID(locationClass,
                                              "setSpeed",
                                              "(F)V");
        env->CallVoidMethod(locationObject, setSpeed, location->speed);
    }

    if (flags & GPS_LOCATION_HAS_BEARING) {
        jmethodID setBearing = env->GetMethodID(locationClass,
                                                "setBearing",
                                                "(F)V");
        env->CallVoidMethod(locationObject, setBearing, location->bearing);
    }

    if (flags & GPS_LOCATION_HAS_ACCURACY) {
        jmethodID setAccuracy = env->GetMethodID(locationClass,
                                                 "setAccuracy",
                                                 "(F)V");
        env->CallVoidMethod(locationObject, setAccuracy, location->accuracy);
    }
    env->DeleteLocalRef(locationClass);
}

static void geofence_service_transition_callback(int32_t geofence_id,
                                                 GpsLocation* location,
                                                 int32_t transition,
                                                 GpsUtcTime timestamp)
{
    JNIEnv* env = AndroidRuntime::getJNIEnv();
    jobject locationObject = NULL;
    if (location != NULL) {
        TranslateToObject(location, locationObject);
    }

    env->CallVoidMethod(sCallbacksObj,
                        method_reportGeofenceTransition,
                        geofence_id,
                        transition,
                        locationObject);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);
};

static void geofence_service_status_callback(int32_t status,
                                             GpsLocation* last_location)
{
    JNIEnv* env = AndroidRuntime::getJNIEnv();
    ALOGD("geofence_service_status_callback: %d\n", status);
    jobject locationObject = NULL;
    if (last_location != NULL) {
        TranslateToObject(last_location, locationObject);
    }

    env->CallVoidMethod(sCallbacksObj,
                        method_reportGeofenceEngineStatus,
                        status,
                        locationObject);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);
}

static void geofence_service_add_callback(int32_t geofence_id,
                                          int32_t status)
{
    JNIEnv* env = AndroidRuntime::getJNIEnv();
    ALOGD("geofence_service_add_callback: %d\n", status);
    env->CallVoidMethod(sCallbacksObj,
                        method_reportGeofenceRequestStatus,
                        GEOFENCE_REQUEST_TYPE_ADD,
                        geofence_id,
                        status);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);
};

static void geofence_service_remove_callback(int32_t geofence_id,
                                             int32_t status)
{
    JNIEnv* env = AndroidRuntime::getJNIEnv();
    ALOGD("geofence_service_remove_callback: %d\n", status);
    env->CallVoidMethod(sCallbacksObj,
                        method_reportGeofenceRequestStatus,
                        GEOFENCE_REQUEST_TYPE_REMOVE,
                        geofence_id,
                        status);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);
};

static void geofence_service_resume_callback(int32_t geofence_id,
                                             int32_t status)
{
    JNIEnv* env = AndroidRuntime::getJNIEnv();
    ALOGD("geofence_service_resume_callback: %d\n", status);
    env->CallVoidMethod(sCallbacksObj,
                        method_reportGeofenceRequestStatus,
                        GEOFENCE_REQUEST_TYPE_RESUME,
                        geofence_id,
                        status);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);
};

static void geofence_service_pause_callback(int32_t geofence_id,
                                            int32_t status)
{
    JNIEnv* env = AndroidRuntime::getJNIEnv();
    ALOGD("geofence_service_pause_callback: %d\n", status);
    env->CallVoidMethod(sCallbacksObj,
                        method_reportGeofenceRequestStatus,
                        GEOFENCE_REQUEST_TYPE_PAUSE,
                        geofence_id,
                        status);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);
};

static pthread_t create_thread_callback(const char* name, void (*start)(void *), void* arg)
{
    return (pthread_t)AndroidRuntime::createJavaThread(name, start, arg);
}

GpsGeofenceCallbacks sGpsGeofenceCallbacks = {
    geofence_service_transition_callback,
    geofence_service_status_callback,
    geofence_service_add_callback,
    geofence_service_remove_callback,
    geofence_service_pause_callback,
    geofence_service_resume_callback,
    create_thread_callback,
};

static void GeofenceServiceProvider_class_init(JNIEnv* env, jclass clazz)
{
    ALOGD("GeofenceServiceProvider_class_init");
    // get references to the Java methods
    method_reportGeofenceTransition = env->GetMethodID(clazz,
                                            "reportGeofenceTransition",
                                            "(IILandroid/location/Location;)V");

    method_reportGeofenceRequestStatus = env->GetMethodID(clazz,
                                              "reportGeofenceRequestStatus",
                                              "(III)V");

    method_reportGeofenceEngineStatus = env->GetMethodID(clazz,
                                             "reportGeofenceStatus",
                                             "(ILandroid/location/Location;)V");
}

static void GeofenceServiceProvider_init(JNIEnv* env, jobject obj)
{
    ALOGD("GeofenceServiceProvider_init");
    if (sCallbacksObj == NULL) {
        sCallbacksObj = env->NewGlobalRef(obj);
    }
    if (sGeoFencer == NULL) {
        GpsGeofenceCb* gpsCallBacks = new GpsGeofenceCb(&sGpsGeofenceCallbacks);
        sGeoFencer = new GeoFencer(gpsCallBacks, create_thread_callback);
    }
}

static void GeofenceServiceProvider_add_geofence(JNIEnv* env,
                                                 jobject obj,
                                                 jint geofence_id,
                                                 jdouble latitude,
                                                 jdouble longitude,
                                                 jdouble radius_meters,
                                                 jint transition_types,
                                                 jint notification_responsiveness_ms,
                                                 jint confidence,
                                                 jint dwell_time_s,
                                                 jint dwell_time_mask) {
    if (sGeoFencer) {
        sGeoFencer->addCommand(geofence_id,
                               latitude,
                               longitude,
                               radius_meters,
                               0, //last_transition
                               transition_types,
                               notification_responsiveness_ms,
                               0, //unknown_timer_ms
                               confidence, // confidence
                               dwell_time_s,
                               dwell_time_mask);
    } else {
        ALOGE("sGeoFencer is NULL in add_geofence");
    }
}

static void GeofenceServiceProvider_remove_geofence(JNIEnv* env,
                                                    jobject obj,
                                                    jint geofence_id) {
    if (sGeoFencer) {
        sGeoFencer->removeCommand(geofence_id);
    } else {
        ALOGE("sGeoFencer is NULL in remove_geofence");
    }
}

static void GeofenceServiceProvider_update_geofence(JNIEnv* env,
                                                    jobject obj,
                                                    jint geofence_id,
                                                    jint transition_type,
                                                    jint notification_responsiveness_ms) {
    if (sGeoFencer) {
        GeofenceExtOptions options;
        memset (&options, 0, sizeof(GeofenceExtOptions));
        options.monitor_transitions = transition_type;
        options.notification_responsivenes_ms = notification_responsiveness_ms;
        sGeoFencer->modifyCommand(geofence_id, &options);
    } else {
        ALOGE("sGeoFencer is NULL in update_geofence");
    }
}

static void GeofenceServiceProvider_pause_geofence(JNIEnv* env,
                                                   jobject obj,
                                                   jint geofence_id) {
    if (sGeoFencer) {
        sGeoFencer->pauseCommand(geofence_id);
    } else {
        ALOGE("sGeoFencer is NULL in remove_geofence");
    }
}

static void GeofenceServiceProvider_resume_geofence(JNIEnv* env,
                                                    jobject obj,
                                                    jint geofence_id,
                                                    jint transition_type) {
    if (sGeoFencer) {
        sGeoFencer->resumeCommand(geofence_id, transition_type);
    } else {
        ALOGE("sGeoFencer is NULL in remove_geofence");
    }
}

static JNINativeMethod sMethods[] = {
    /* name, signature, funcPtr */
    {"native_geofence_class_init",
     "()V",
     reinterpret_cast<void*>(GeofenceServiceProvider_class_init)},
    {"native_geofence_init",
     "()V",
     reinterpret_cast<void*>(GeofenceServiceProvider_init)},
    {"native_add_geofence",
     "(IDDDIIIII)V",
     reinterpret_cast<void*>(GeofenceServiceProvider_add_geofence)},
    {"native_remove_geofence",
     "(I)V",
     reinterpret_cast<void*>(GeofenceServiceProvider_remove_geofence)},
    {"native_update_geofence",
     "(III)V",
     reinterpret_cast<void*>(GeofenceServiceProvider_update_geofence)},
    {"native_pause_geofence",
     "(I)V",
     reinterpret_cast<void*>(GeofenceServiceProvider_pause_geofence)},
    {"native_resume_geofence",
     "(II)V",
     reinterpret_cast<void*>(GeofenceServiceProvider_resume_geofence)}
};

int register_GeofenceServiceProvider(JNIEnv* env)
{
    ALOGD("register_GeofenceServiceProvider");
    return jniRegisterNativeMethods(env,
                                    "com/qualcomm/location/izat/geofence/GeofenceServiceProvider",
                                    sMethods,
                                    NELEM(sMethods));
}

