/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

  Not a Contribution, Apache license notifications and
  license are retained for attribution purposes only.
=============================================================================*/

/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (c) 2011,2012, The Linux Foundation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "LocSvc_jni"
#define LOG_NDEBUG 0

#include "JNIHelp.h"
#include "jni.h"
#include "hardware/hardware.h"
#include "hardware/gps.h"
#include "hardware_legacy/power.h"
#include "utils/Log.h"
#include "utils/misc.h"
#include "android_runtime/AndroidRuntime.h"
#include "loc_extended.h"

#include <string.h>
#include <pthread.h>

static jmethodID method_reportAGpsStatus;
static jmethodID method_setEngineCapabilities;
static jmethodID method_reportNiNotification;

static jobject mCallbacksObj = NULL;

#define WAKE_LOCK_NAME  "LocationService"

static void checkAndClearExceptionFromCallback(JNIEnv* env, const char* methodName) {
    if (env->ExceptionCheck()) {
        ALOGE("An exception was thrown by callback '%s'.", methodName);
        env->ExceptionClear();
    }
}

static void set_capabilities_callback(uint32_t capabilities)
{
    ALOGD("set_capabilities_callback: %ld\n", (long ) capabilities);
    JNIEnv* env = android::AndroidRuntime::getJNIEnv();
    env->CallVoidMethod(mCallbacksObj, method_setEngineCapabilities, capabilities);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);
}

static void acquire_wakelock_callback()
{
    acquire_wake_lock(PARTIAL_WAKE_LOCK, WAKE_LOCK_NAME);
}

static void release_wakelock_callback()
{
    release_wake_lock(WAKE_LOCK_NAME);
}

static pthread_t create_thread_callback(const char* name, void (*start)(void *), void* arg)
{
    return (pthread_t)android::AndroidRuntime::createJavaThread(name, start, arg);
}

GpsExtCallbacks sGpsExtCallbacks = {
    sizeof(GpsExtCallbacks),
    set_capabilities_callback,
    acquire_wakelock_callback,
    release_wakelock_callback,
    create_thread_callback,
    NULL,
};

static void agps_status_extended_callback(AGpsExtStatus* agps_status)
{
    JNIEnv* env = android::AndroidRuntime::getJNIEnv();

    env->CallVoidMethod(mCallbacksObj,
                        method_reportAGpsStatus,
                        agps_status->type,
                        agps_status->status);

    checkAndClearExceptionFromCallback(env, __FUNCTION__);
}

AGpsExtCallbacks sAGpsExtCallbacks = {
    agps_status_extended_callback,
    create_thread_callback,
};

static void gps_ni_notify(GpsNiNotification *notification)
{
    ALOGD("gps_ni_notify\n");
    JNIEnv* env = android::AndroidRuntime::getJNIEnv();
    jstring requestor_id = env->NewStringUTF(notification->requestor_id);
    jstring text = env->NewStringUTF(notification->text);
    jstring extras = env->NewStringUTF(notification->extras);

    if (requestor_id && text && extras) {
        env->CallVoidMethod(mCallbacksObj, method_reportNiNotification,
            notification->notification_id, notification->ni_type,
            notification->notify_flags, notification->timeout,
            notification->default_response, requestor_id, text,
            notification->requestor_id_encoding,
            notification->text_encoding, extras);
    } else {
        ALOGE("out of memory in gps_ni_notify\n");
    }

    if (requestor_id)
        env->DeleteLocalRef(requestor_id);
    if (text)
        env->DeleteLocalRef(text);
    if (extras)
        env->DeleteLocalRef(extras);
    checkAndClearExceptionFromCallback(env, __FUNCTION__);
}

GpsNiExtCallbacks sGpsNiExtCallbacks = {
    gps_ni_notify,
    create_thread_callback,
};

static void LocationService_class_init_native(JNIEnv* env, jclass clazz) {
    int err;
    hw_module_t* module;

    method_reportAGpsStatus = env->GetMethodID(clazz, "reportAGpsStatus", "(II)V");
    method_setEngineCapabilities = env->GetMethodID(clazz, "setEngineCapabilities", "(I)V");
    method_reportNiNotification = env->GetMethodID(clazz, "reportNiNotification",
            "(IIIIILjava/lang/String;Ljava/lang/String;IILjava/lang/String;)V");
}

static jboolean LocationService_init(JNIEnv* env, jobject obj)
{
    // this must be set before calling into the HAL library
    if (!mCallbacksObj)
        mCallbacksObj = env->NewGlobalRef(obj);

    loc_extended_init(&sGpsExtCallbacks);
    loc_extended_agps_init(&sAGpsExtCallbacks);
    loc_extended_ni_init(&sGpsNiExtCallbacks);

    return true;
}

static void LocationService_cleanup(JNIEnv* env, jobject obj)
{
    loc_extended_cleanup();
}

static void LocationService_agps_data_conn_open(JNIEnv* env, jobject obj,
        jint agpsType, jstring apn, jint bearerType)
{
    if (apn == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }
    const char *apnStr = env->GetStringUTFChars(apn, NULL);
    loc_extended_agps_open(agpsType, apnStr, bearerType);
    env->ReleaseStringUTFChars(apn, apnStr);
}

static void LocationService_agps_data_conn_closed(JNIEnv* env, jobject obj,
        jint agpsType)
{
    loc_extended_agps_closed(agpsType);
}

static void LocationService_agps_data_conn_failed(JNIEnv* env, jobject obj,
        jint agpsType)
{
    loc_extended_agps_open_failed(agpsType);
}

static void LocationService_send_ni_response(JNIEnv* env, jobject obj,
      jint notifId, jint response)
{
    loc_extended_ni_respond(notifId, response);
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"class_init_native", "()V", (void *)LocationService_class_init_native},
    {"native_init", "()Z", (void*)LocationService_init},
    {"native_cleanup", "()V", (void*)LocationService_cleanup},
    {"native_agps_data_conn_open", "(ILjava/lang/String;I)V", (void*)LocationService_agps_data_conn_open},
    {"native_agps_data_conn_closed", "(I)V", (void*)LocationService_agps_data_conn_closed},
    {"native_agps_data_conn_failed", "(I)V", (void*)LocationService_agps_data_conn_failed},
    {"native_send_ni_response", "(II)V", (void*)LocationService_send_ni_response},
};

int register_LocationService(JNIEnv* env)
{
    return jniRegisterNativeMethods(env, "com/qualcomm/location/LocationService", sMethods, NELEM(sMethods));
}


extern int register_OsFramework(JNIEnv* env);
extern int register_Subscription(JNIEnv* env);
extern int register_RilInfoMonitor(JNIEnv* env);
extern int register_OSNPGlue (JNIEnv* env);
extern int register_IzatProviderEngine (JNIEnv* env);
extern int register_FlpServiceProvider(JNIEnv* env);
extern int register_GeofenceServiceProvider(JNIEnv* env);

extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("GetEnv failed!");
        return result;
    }
    ALOG_ASSERT(env, "Could not retrieve the env!");

    ALOGD ("LocationService_jni: JNI_OnLoad");

    register_OsFramework(env);
    register_Subscription(env);
    register_LocationService(env);
    register_RilInfoMonitor(env);
    register_OSNPGlue (env);
    register_IzatProviderEngine (env);
    register_FlpServiceProvider(env);
    register_GeofenceServiceProvider(env);

    return JNI_VERSION_1_4;
}


