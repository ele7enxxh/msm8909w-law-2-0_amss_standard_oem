LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_JAVA_LIBRARIES += qwcontextualmodelib

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res

LOCAL_AAPT_FLAGS := --auto-add-overlay

LOCAL_PACKAGE_NAME := watchhome

LOCAL_OVERRIDES_PACKAGES := Launcher2 Launcher3 Home

LOCAL_PRIVILEGED_MODULE := true

LOCAL_CERTIFICATE := platform

LOCAL_PROGUARD_ENABLED := obfuscation

LOCAL_PROGUARD_FLAG_FILES := proguard.flags

include $(BUILD_PACKAGE)
