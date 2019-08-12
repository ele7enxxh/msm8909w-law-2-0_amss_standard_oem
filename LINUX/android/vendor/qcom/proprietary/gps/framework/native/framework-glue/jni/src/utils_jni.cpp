/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#include "utils_jni.h"

#include <android_runtime/AndroidRuntime.h>
#include <log_util.h>

#include "utils/Log.h"
#include "IzatDefines.h"

JNIEnv *sCallbackEnv = NULL;

using namespace android;

int vmAssociator()
{
    ENTRY_LOG();

    int result = 0;

    do {
       if (sCallbackEnv != NULL) {
            result = 0;
            break;
       }

       JavaVM* javaVm = AndroidRuntime::getJavaVM();

       JavaVMAttachArgs args = {
            JNI_VERSION_1_6,
            "IzatSvc Callback Thread",
            NULL
       };

       jint attachResult = javaVm->AttachCurrentThread(&sCallbackEnv,
            &args);
       if (attachResult != 0) {
            LOC_LOGE("Callback thread attachment error %d", attachResult);
            result = 1;
            break;
       }

       // TODO: make the associator from MsgTask send events to associate
       // and disassociate.
        LOC_LOGV("Callback thread attached to VM %p", sCallbackEnv)
        result = 0;
    } while(0);

    EXIT_LOG("%d", result);
    return result;
}

JNIEnv* getCallbackEnv()
{
    return sCallbackEnv;
}

bool isValidCallbackThread()
{
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    if ((sCallbackEnv == NULL) || (sCallbackEnv != env)) {
        return false;
    }

    return true;
}

void checkAndClearExceptionsFromCallback(JNIEnv *env, const char* methodName)
{
    if (env->ExceptionCheck()) {
        LOC_LOGE("An exception was thrown from callback '%s'", methodName);
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}
