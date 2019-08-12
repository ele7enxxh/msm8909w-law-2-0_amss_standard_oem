#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling the source files
#*******************************************************************************/

LOCAL_PATH:= $(call my-dir)

# Build the app
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-subdir-java-files)


LOCAL_PACKAGE_NAME := radioconfigtest
LOCAL_CERTIFICATE := platform
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_JAVA_LIBRARIES := radioconfiglibrary
LOCAL_JAVA_LIBRARIES += radioconfiginterfacelibrary

include $(BUILD_PACKAGE)
