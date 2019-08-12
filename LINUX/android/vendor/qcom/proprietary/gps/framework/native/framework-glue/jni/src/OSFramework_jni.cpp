/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#define LOG_TAG "OSFramework_jni"
#define LOG_NDEBUG 0

#include <jni.h>
#include <JNIHelp.h>
#include <android_runtime/AndroidRuntime.h>

#include "utils/Log.h"
#include "OSFramework.h"
#include "IzatDefines.h"

using namespace android;

static void classInit(JNIEnv* env, jclass clazz)
{
    ENTRY_LOG();
}

static void instanceInit(JNIEnv *env, jobject obj)
{
    ENTRY_LOG();
#ifndef ON_TARGET_UNIT_TEST
    (void)OSFramework::getOSFramework();
#endif
}

static void instanceDeinit(JNIEnv *env, jobject obj)
{
    ENTRY_LOG();

    OSFramework::destroyInstance();
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"native_framework_class_init", "()V", (void *)classInit},
    {"native_framework_init", "()V", (void *)instanceInit},
    {"native_framework_deinit", "()V", (void *)instanceDeinit},
};

int register_OsFramework(JNIEnv* env) {
    return jniRegisterNativeMethods(env, "com/qualcomm/location/izatserviceprovider/service/IzatServiceBase",
                                    sMethods, NELEM(sMethods));
}
