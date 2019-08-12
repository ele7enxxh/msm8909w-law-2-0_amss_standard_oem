/*==========================================================================
@file AR_jni.cpp

@brief
JNI Native code containing several functions to be called from a java class.
Run Activity Recognition Hardware tests.

Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
==========================================================================*/

#include <utils/Log.h>
#include <jni.h>
#include <JNIHelp.h>
#include <dlfcn.h>
#include <string.h>
#include <AR_jni.h>
#include "hardware/activity_recognition.h"
#include <inttypes.h>
#include "cutils/properties.h"
#define DEBUG_TAG "ARHAL-JNI"

#if defined(__LP64__)
# define AR_HAL_LIB_PATH "/system/vendor/lib64/hw/"
#else
# define AR_HAL_LIB_PATH "/system/vendor/lib/hw/"
#endif
#define AR_HAL_LIB_NAME AR_HAL_LIB_PATH"activity_recognition." BOARD_PLATFORM ".so"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 Variable Definitions
 ==========================================================================*/
activity_recognition_callback_procs_t callbackStruct;
activity_recognition_device_t *arDevice;
activity_recognition_module_t *hal_sym;
int num_supported_activities;
void *hal_lib;
char **list = NULL;

typedef struct {
    jboolean initialized;
    JavaVM* vm;
    jclass javaClass;
    JNIEnv* env;
    jmethodID callbackMethodId;
    jobject object;
} CachedData;

/*==========================================================
 Static Variable Definitions
===========================================================*/
 const char* const AR_ClassName =
                "com/qualcomm/qti/sensors/core/sensortest/AR";
 const char* const notifyARCallback_Sig = "(IIJ)V";
 CachedData nativeCachedData;
//============================================================
static int registerMethods() {
    jclass javaClassLocalRef = nativeCachedData.env->FindClass(AR_ClassName);
    if (javaClassLocalRef == NULL) {
        ALOGE("%s : Can't find class %s", __FUNCTION__, AR_ClassName);
        return JNI_FALSE;
    } else {
        ALOGD("%s :found class %s notifyARCallback_Sig = %s", __FUNCTION__, AR_ClassName, notifyARCallback_Sig);
    }
    nativeCachedData.javaClass =
        reinterpret_cast<jclass>(nativeCachedData.env->NewGlobalRef(javaClassLocalRef));
    ALOGD("%s : found class %s notifyARCallback_Sig = %s ", __FUNCTION__, AR_ClassName, notifyARCallback_Sig);
    nativeCachedData.callbackMethodId = nativeCachedData.env->GetMethodID(
        nativeCachedData.javaClass, "arCallback", notifyARCallback_Sig);
    if (nativeCachedData.callbackMethodId == NULL) {
        ALOGE("%s : Can't find method arCallback", __FUNCTION__);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/*
 * This is called by the VM when the shared library is first loaded.
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    // Save vm. Needed for async call into Java layer
    nativeCachedData.initialized = false;
    nativeCachedData.vm = vm;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("%s : GetEnv failed", __FUNCTION__);
        return JNI_ERR;
    }
    nativeCachedData.env = env;
    if (registerMethods() != JNI_TRUE) {
        ALOGE("%s : native registration failed", __FUNCTION__);
        return JNI_ERR;
    }
    nativeCachedData.initialized = true;
    return JNI_VERSION_1_4;
}
//=============================================================

static void activity_callback(
    const struct activity_recognition_callback_procs* procs,
    const activity_event_t* events, int count) {

    ALOGD(" Activity callback called with count: %d", count);
    if (!nativeCachedData.initialized) {
        ALOGE(" Cached Data is not initialized: can't invoke JAVA callback.");
        return;
    } else {
        ALOGD(" Cached Data initialized: preparing invoke JAVA callback.");
    }
    int getEnvResult = (nativeCachedData.vm)->GetEnv(
        (void**) &(nativeCachedData.env), JNI_VERSION_1_4);
        if (getEnvResult == JNI_EDETACHED) {
            // Since this function is called in the CMC/TILT library thread, it needs to first attach to the Android VM
            // and get the JNIEnv
            int thrAttachResult = nativeCachedData.vm->AttachCurrentThread(
                &(nativeCachedData.env), NULL);
            if (thrAttachResult < 0 || nativeCachedData.env == NULL) {
                ALOGE("%s : Can't get jEnv even after AttachCurrentThread",__FUNCTION__);
                return;
            } else {
                ALOGD("%s : Got jEnv after AttachCurrentThread", __FUNCTION__);
            }
        } else if (getEnvResult != JNI_OK) {
            ALOGE("%s : GetEnv failed. result=%d", __FUNCTION__, getEnvResult);
            return;
	}

    for (int i = 0; i < count; i++) {
         ALOGD(" Processing event with id: %d ", i);
         if (events[i].event_type == ACTIVITY_EVENT_EXIT || events[i].event_type == ACTIVITY_EVENT_ENTER) {
        /*
         * Temporary fix to prevent crash where values received are like state: 2, activity: -1284831432, ts: -5518198490269022376
         */
         if (events[i].activity >= 0 && events[i].activity < num_supported_activities) {
              if (events[i].timestamp > 0) {
                  if (nativeCachedData.env) {
                      nativeCachedData.env->CallVoidMethod(
                        nativeCachedData.object, nativeCachedData.callbackMethodId,
                          events[i].event_type, events[i].activity,
                          events[i].timestamp);
                 }
              }
          }
       }
    }
    nativeCachedData.vm->DetachCurrentThread();
}
//===================================================================

/* Header for class com_qualcomm_qti_sensors_core_sensortest_AR */

/*
 * Class:     com_qualcomm_qti_sensors_core_sensortest_AR
 * Method:    nativeClassInit
 * Signature: ()V
 */

JNIEXPORT void JNICALL Java_com_qualcomm_qti_sensors_core_sensortest_AR_nativeClassInit
  (JNIEnv *env, jclass j) {

    hal_lib = dlopen(AR_HAL_LIB_NAME, RTLD_LAZY);
    if (hal_lib == NULL) {
        ALOGE("dlopen of sensor HAL lib (%s) failed\n", AR_HAL_LIB_NAME);
        return ;
    }
    dlerror(); // Clear pending errors.
    hal_sym = (activity_recognition_module_t*) dlsym(hal_lib, HAL_MODULE_INFO_SYM_AS_STR);
    if (dlerror() != NULL || hal_sym == NULL) {
        ALOGE( " dlsym(%s) failed", HAL_MODULE_INFO_SYM_AS_STR);
        return ;
    }

    if (0 != hal_sym->common.methods->open(NULL,
        ACTIVITY_RECOGNITION_HARDWARE_INTERFACE, (hw_device_t**) &arDevice)) {
        ALOGE( " AR Hal open failure");
        return ;
    }

    num_supported_activities = hal_sym->get_supported_activities_list(NULL, ((char const* const**)&list));
    ALOGD(" Number of activities supported is %d", num_supported_activities);
    if (list != NULL ) {
        for (int i = 0; i < num_supported_activities; i++) {
             ALOGE(" list %d  %s ", i, list[i]);
        }
    }
    //Set callback function
    callbackStruct.activity_callback = activity_callback;
    ALOGD( " Attempting to register callback");
    //if the function pointers are not null, register callback
    if (arDevice->register_activity_callback == NULL) {
        return ;
    } else {
        arDevice->register_activity_callback(arDevice, &callbackStruct);
        ALOGD("Successfully registered callback");
    }
    return ;
}

/*
 * Class:     com_qualcomm_qti_sensors_core_sensortest_AR
 * Method:    nativeEnableActivityEvent
 * Signature: (IIJ)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_sensors_core_sensortest_AR_nativeEnableActivityEvent
  (JNIEnv *env, jobject o, jint ar_handle, jint ar_event, jlong ar_timestamp) {
  return arDevice->enable_activity_event(arDevice, ar_handle, ar_event, ar_timestamp);
}

/*
 * Class:     com_qualcomm_qti_sensors_core_sensortest_AR
 * Method:    nativeDisableActivityEvent
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_sensors_core_sensortest_AR_nativeDisableActivityEvent
  (JNIEnv *env, jobject o, jint ar_handle, jint ar_event) {
  return arDevice->disable_activity_event(arDevice, ar_handle, ar_event);
}

/*
 * Class:     com_qualcomm_qti_sensors_core_sensortest_AR
 * Method:    nativeInitialize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_sensors_core_sensortest_AR_nativeInitialize
  (JNIEnv *env, jobject thisObj) {
   nativeCachedData.object= nativeCachedData.env->NewGlobalRef(thisObj);
}

#ifdef __cplusplus
}
#endif
