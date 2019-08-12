/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "QVOP"

#include <android/log.h>
#include <utils/Log.h>
#include "android_runtime/AndroidRuntime.h"

#include "jni.h"
#include <cutils/properties.h>
#include <string>
#include <vector>

using namespace android;

/**
 * Create a Java string array
 */
static jobjectArray NewStringArray(JNIEnv* env, jsize length) {
  jclass StringClass = env->FindClass("java/lang/String");
  return env->NewObjectArray(length, StringClass, NULL);
}

static bool ValidateArray(JNIEnv* env, jarray dst, jsize pos) {
  jsize length = env->GetArrayLength(dst);
  if (length <= pos) {
    ALOGE("invalid array length %d", length);
    return false;
  }
  return true;
}

/**
 * Sets a value in an int array
 */
static void SetJavaInt(JNIEnv* env, jintArray dst, jsize pos, jint src) {
  if (!ValidateArray(env, dst, pos)) {
    return;
  }
  env->SetIntArrayRegion(dst, pos, 1, &src);
}

/**
 * Sets a value in a long array
 */
static void SetJavaLong(JNIEnv* env, jlongArray dst, jsize pos, jlong src) {
  if (!ValidateArray(env, dst, pos)) {
    return;
  }
  env->SetLongArrayRegion(dst, pos, 1, &src);
}

/**
 * Sets a value in an object array.
 */
static void SetJavaObject(JNIEnv* env, jobjectArray dst, jsize pos, jobject src) {
  if (!ValidateArray(env, dst, pos)) {
    return;
  }
  env->SetObjectArrayElement(dst, pos, src);
}

/**
 * Sets a value in a string array.
 */
static void SetJavaString(JNIEnv* env, jobjectArray dst, jsize pos, char const src[], size_t len) {
  // make sure the string is terminated
  std::string str(src, len);
  SetJavaObject(env, dst, pos, env->NewStringUTF(str.c_str()));
}

/**
 * Get a Java string with automatic release
 */
class GetJavaString {
public:
  GetJavaString(JNIEnv* jniEnv, jstring str) :
    env(jniEnv),
    javaString(str),
    nativeString(env->GetStringUTFChars(str, 0))
  { }

  ~GetJavaString() {
    env->ReleaseStringUTFChars(javaString, nativeString);
  }

  char const* get() {
    return nativeString;
  }

  operator char const*() {
    return nativeString;
  }

private:
  JNIEnv* env;
  jstring javaString;
  char const* nativeString;
};




