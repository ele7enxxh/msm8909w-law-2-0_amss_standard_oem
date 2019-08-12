/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_UTILS_JNI_H__
#define __IZAT_MANAGER_UTILS_JNI_H__

#include <jni.h>

int vmAssociator();
JNIEnv* getCallbackEnv();
bool isValidCallbackThread();
void checkAndClearExceptionsFromCallback(JNIEnv *env, const char* methodName);

#endif // #ifndef __IZAT_MANAGER_UTILS_JNI_H__
