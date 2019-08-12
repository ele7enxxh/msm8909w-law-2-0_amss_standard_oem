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
 * Copyright (C) 2013 The Android Open Source Project
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

#define LOG_TAG "FlpSvcProvider_jni"
#define LOG_NDEBUG 0
#define LOCATION_CLASS_NAME "android/location/Location"

#include "JNIHelp.h"
#include "jni.h"
#include "FlpLocationClient.h"
#include "hardware/gps.h"
#include "hardware_legacy/power.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/Log.h"

using namespace android;

static jobject sCallbacksObj = NULL;
static FlpLocationClient* sFlpLocationClient = NULL;
static jmethodID sOnLocationReport = NULL;
static jmethodID sOnMaxPowerAllocatedChanged = NULL;
static JNIEnv *sCallbackEnv = NULL;

static inline void CheckExceptions(JNIEnv* env, const char* methodName) {
    if(!env->ExceptionCheck()) {
        return;
    }
    ALOGE("An exception was thrown by '%s'.", methodName);
    LOGE_EX(env);
    env->ExceptionClear();
}

static bool IsValidCallbackThread() {
    JNIEnv* env = AndroidRuntime::getJNIEnv();
    if(sCallbackEnv == NULL){
        ALOGE("sCallbackEnv == NULL");
        return false;
    } else if (sCallbackEnv != env) {
        ALOGE("CallbackThread check fail: env=%p, expected=%p", env, sCallbackEnv);
        return false;
    }
    return true;
}

static void TranslateToObject(const FlpExtLocation* location, jobject& locationObject) {
    jclass locationClass = sCallbackEnv->FindClass(LOCATION_CLASS_NAME);
    jmethodID locationCtor = sCallbackEnv->GetMethodID(locationClass,
                                                       "<init>",
                                                       "(Ljava/lang/String;)V");
    // the provider is set in the upper JVM layer
    locationObject = sCallbackEnv->NewObject(locationClass, locationCtor, NULL);
    jint flags = location->flags;

    // set the valid information in the object
    if (flags & FLP_EXTENDED_LOCATION_HAS_LAT_LONG) {
        jmethodID setLatitude = sCallbackEnv->GetMethodID(locationClass,
                                                          "setLatitude",
                                                          "(D)V");
        sCallbackEnv->CallVoidMethod(locationObject, setLatitude, location->latitude);

        jmethodID setLongitude = sCallbackEnv->GetMethodID(locationClass,
                                                           "setLongitude",
                                                           "(D)V");
        sCallbackEnv->CallVoidMethod( locationObject, setLongitude, location->longitude);

        jmethodID setTime = sCallbackEnv->GetMethodID(locationClass,
                                                      "setTime",
                                                      "(J)V");
        sCallbackEnv->CallVoidMethod(locationObject, setTime, location->timestamp);
    }

    if (flags & FLP_EXTENDED_LOCATION_HAS_ALTITUDE) {
        jmethodID setAltitude = sCallbackEnv->GetMethodID(locationClass,
                                                          "setAltitude",
                                                          "(D)V");
        sCallbackEnv->CallVoidMethod(locationObject, setAltitude, location->altitude);
    }

    if (flags & FLP_EXTENDED_LOCATION_HAS_SPEED) {
        jmethodID setSpeed = sCallbackEnv->GetMethodID(locationClass,
                                                       "setSpeed",
                                                       "(F)V");
        sCallbackEnv->CallVoidMethod(locationObject, setSpeed, location->speed);
    }

    if (flags & FLP_EXTENDED_LOCATION_HAS_BEARING) {
        jmethodID setBearing = sCallbackEnv->GetMethodID(locationClass,
                                                         "setBearing",
                                                         "(F)V");
        sCallbackEnv->CallVoidMethod(locationObject, setBearing, location->bearing);
    }

    if (flags & FLP_EXTENDED_LOCATION_HAS_ACCURACY) {
        jmethodID setAccuracy = sCallbackEnv->GetMethodID(locationClass,
                                                          "setAccuracy",
                                                          "(F)V");
        sCallbackEnv->CallVoidMethod(locationObject, setAccuracy, location->accuracy);
    }
    sCallbackEnv->DeleteLocalRef(locationClass);
}

static void TranslateToObjectArray(int32_t locationsCount,
                                   FlpExtLocation** locations,
                                   jobjectArray& locationsArray) {

    jclass locationClass = sCallbackEnv->FindClass(LOCATION_CLASS_NAME);
    locationsArray = sCallbackEnv->NewObjectArray(locationsCount,
                                                  locationClass,
                                                  NULL);
    for (int i = 0; i < locationsCount; ++i) {
        jobject locationObject = NULL;
        TranslateToObject(locations[i], locationObject);
        sCallbackEnv->SetObjectArrayElement(locationsArray, i, locationObject);
        sCallbackEnv->DeleteLocalRef(locationObject);
    }
    sCallbackEnv->DeleteLocalRef(locationClass);
}

static void LocationCallback(int32_t locationsCount,
                             FlpExtLocation** locations,
                             LocReportType reportTrigger) {
    ALOGD("FlpService_jni LocationCallback()");
    if(!IsValidCallbackThread()) {
        return;
    }
    if(locationsCount == 0 || locations == NULL) {
        ALOGE("Invalid LocationCallback. Count: %d, Locations: %p",
              locationsCount, locations);
        return;
    }
    jobjectArray locationsArray = NULL;
    TranslateToObjectArray(locationsCount, locations, locationsArray);

    sCallbackEnv->CallVoidMethod(
        sCallbacksObj,
        sOnLocationReport,
        locationsArray,
        reportTrigger
        );
    CheckExceptions(sCallbackEnv, __FUNCTION__);

    if(locationsArray != NULL) {
        sCallbackEnv->DeleteLocalRef(locationsArray);
    }
}

static void MaxPowerAllocatedCallback(double power_mW) {
    ALOGD("FlpService_jni MaxPowerAllocatedCallback()");
    if(!IsValidCallbackThread()) {
        return;
    }

    sCallbackEnv->CallVoidMethod(
        sCallbacksObj,
        sOnMaxPowerAllocatedChanged,
        power_mW);
    CheckExceptions(sCallbackEnv, __FUNCTION__);
}

static void AcquireWakelock() {
    acquire_wake_lock(PARTIAL_WAKE_LOCK, "FlpService");
}

static void ReleaseWakelock() {
    release_wake_lock("FlpService");
}

static int SetThreadEvent(FlpExtThreadEvent event) {
    ALOGD("FlpService jni SetThreadEvent");
    JavaVM* javaVm = AndroidRuntime::getJavaVM();
    switch(event) {
        case ASSOCIATE_JVM_EXT:
        {
            if(sCallbackEnv != NULL) {
                ALOGE( "Attempted to associate callback in '%s'."
                       " Callback already associated.",
                        __FUNCTION__);
                return FLP_ERROR;
            }

            JavaVMAttachArgs args = {
                JNI_VERSION_1_6,
                "FLP Service Callback Thread",
                NULL
            };

            jint attachResult = javaVm->AttachCurrentThread(&sCallbackEnv, &args);
            if (attachResult != 0) {
                ALOGE("Callback thread attachment error: %d", attachResult);
                return FLP_ERROR;
            }

            ALOGV("Callback thread attached: %p", sCallbackEnv);
            break;
       }
       case DISASSOCIATE_JVM_EXT:
       {
            if (!IsValidCallbackThread()) {
                ALOGE( "Attempted to disassociate an unknown callback thread : '%s'.",
                       __FUNCTION__ );
                return FLP_ERROR;
            }

            if (javaVm->DetachCurrentThread() != 0) {
              return FLP_ERROR;
            }

            sCallbackEnv = NULL;
            break;
       }
       default:
            ALOGE("Invalid FlpExtThreadEvent request %d", event);
            return FLP_ERROR;
    }
    return FLP_SUCCESS;
}

FlpExtCallbacks sFlpCallbacks = {
    sizeof(FlpExtCallbacks),
    LocationCallback,
    AcquireWakelock,
    ReleaseWakelock,
    SetThreadEvent,
    MaxPowerAllocatedCallback
};

static void FlpServiceProvider_class_init(JNIEnv* env, jclass clazz)
{
    ALOGD("FlpServiceProvider_class_init");
    if (sFlpLocationClient == NULL) {
        sFlpLocationClient = FlpLocationClient::createInstance();
    }
    // get references to the Java methods
    sOnLocationReport = env->GetMethodID(clazz,
                                         "onLocationReport",
                                         "([Landroid/location/Location;I)V");
    sOnMaxPowerAllocatedChanged = env->GetMethodID(clazz,
                                                   "onMaxPowerAllocatedChanged",
                                                  "(D)V");
}

static jint FlpServiceProvider_init(JNIEnv* env, jobject obj)
{
    ALOGD("FlpServiceProvider_init");
    jint result = FLP_ERROR;
    if (sCallbacksObj == NULL) {
        sCallbacksObj = env->NewGlobalRef(obj);
    }
    if (sFlpLocationClient) {
        result = sFlpLocationClient->flp_init(&sFlpCallbacks);
    } else {
        ALOGE("sFlpLocationClient is null.");
    }
    return result;
}

static jint FlpServiceProvider_get_all_supported_features(JNIEnv* env, jobject obj)
{
    ALOGD("FlpServiceProvider_get_all_supported_features");
    jint result = FLP_ERROR;
    if (sFlpLocationClient) {
        result = sFlpLocationClient->flp_get_all_supported_features();
    } else {
        ALOGE("sFlpLocationClient is null.");
    }
    return result;
}

static int FlpServiceProvider_start_flp_session(JNIEnv* env, jobject object,
                                                jint id,
                                                jint flags,
                                                jlong period_ns,
                                                jint distance_ms)
{
    ALOGD("FlpServiceProvider_start_flp_session");
    jint result = FLP_ERROR;
    if (sFlpLocationClient) {
        FlpExtBatchOptions options;
        memset (&options, 0, sizeof(FlpExtBatchOptions));
        options.max_power_allocation_mW = FLP_SESSION_POWER_LEVEL_HIGH;
        options.sources_to_use = FLP_SESSION_TECH_SOURCE_BIT_GNSS;
        options.flags = flags;
        options.period_ns = period_ns;
        options.distance_ms = distance_ms;

        result = sFlpLocationClient->flp_start_session(id, &options);
    } else {
        ALOGE("sFlpLocationClient is null.");
    }
    return result;
}

static int FlpServiceProvider_update_flp_session(JNIEnv* env, jobject object,
                                                 jint id,
                                                 jint flags,
                                                 jlong period_ns,
                                                 jint distance_ms)
{
    ALOGD("FlpServiceProvider_update_flp_session");
    jint result = FLP_ERROR;
    if (sFlpLocationClient) {
        FlpExtBatchOptions options;
        memset (&options, 0, sizeof(FlpExtBatchOptions));
        options.max_power_allocation_mW = FLP_SESSION_POWER_LEVEL_HIGH;
        options.sources_to_use = FLP_SESSION_TECH_SOURCE_BIT_GNSS;
        options.flags = flags;
        options.period_ns = period_ns;
        options.distance_ms = distance_ms;

        result = sFlpLocationClient->flp_update_session(id, &options);
    } else {
        ALOGE("sFlpLocationClient is null.");
    }
    return result;
}

static int FlpServiceProvider_stop_flp_session(JNIEnv* env, jobject object, jint id)
{
    ALOGD("FlpServiceProvider_stop_flp_session");
    jint result = FLP_ERROR;
    if (sFlpLocationClient) {
        result = sFlpLocationClient->flp_stop_session(id);
    } else {
        ALOGE("sFlpLocationClient is null.");
    }
    return result;
}

static int FlpServiceProvider_get_all_locations(JNIEnv* env, jobject object)
{
    ALOGD("FlpServiceProvider_get_all_locations");
    jint result = FLP_ERROR;
    if (sFlpLocationClient) {
        sFlpLocationClient->flp_pull_all_locations();
        result = FLP_SUCCESS;
    } else {
        ALOGE("sFlpLocationClient is null.");
    }
    return result;
}

static void FlpServiceProvider_delete_aiding_data(JNIEnv* env, jobject object,
                                                  jlong flags)
{
    ALOGD("FlpServiceProvider_delete_aiding_data");
    if (sFlpLocationClient) {
        sFlpLocationClient->delete_aiding_data(flags);
    } else {
        ALOGE("sFlpLocationClient is null.");
    }
}

static double FlpServiceProvider_get_max_power_allocated_in_mw(JNIEnv* env,
                                                               jobject object)
{
    ALOGD("FlpServiceProvider_get_max_power_allocated_in_mw");
    double res = -1.0;
    if (sFlpLocationClient) {
        res = sFlpLocationClient->flp_get_max_power_allocated_in_mW();
    } else {
        ALOGE("sFlpLocationClient is null.");
    }
    return res;
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"native_flp_class_init",
     "()V",
     reinterpret_cast<void*>(FlpServiceProvider_class_init)},
    {"native_flp_init",
     "()I",
     reinterpret_cast<void*>(FlpServiceProvider_init)},
    {"native_flp_get_all_supported_features",
     "()I",
     reinterpret_cast<void*>(FlpServiceProvider_get_all_supported_features)},
    {"native_flp_get_all_locations",
     "()I",
     reinterpret_cast<void*>(FlpServiceProvider_get_all_locations)},
    {"native_flp_stop_session",
     "(I)I",
     reinterpret_cast<void*>(FlpServiceProvider_stop_flp_session)},
    {"native_flp_start_session",
     "(IIJI)I",
     reinterpret_cast<void*>(FlpServiceProvider_start_flp_session)},
    {"native_flp_update_session",
     "(IIJI)I",
     reinterpret_cast<void*>(FlpServiceProvider_update_flp_session)},
    {"native_flp_delete_aiding_data",
     "(J)V",
     reinterpret_cast<void*>(FlpServiceProvider_delete_aiding_data)},
    {"native_flp_get_max_power_allocated_in_mw",
     "()D",
     reinterpret_cast<void*>(FlpServiceProvider_get_max_power_allocated_in_mw)},
};

int register_FlpServiceProvider(JNIEnv* env)
{
    ALOGD("register_FlpServiceProvider");
    return jniRegisterNativeMethods(env,
                                    "com/qualcomm/location/izat/flp/FlpServiceProvider",
                                    sMethods,
                                    NELEM(sMethods));
}
