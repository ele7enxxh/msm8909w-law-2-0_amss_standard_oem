ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES :=  $(call all-java-files-under, src/com/qualcomm/qti/sensors/ui)

LOCAL_PACKAGE_NAME := ArSensorTest
LOCAL_CERTIFICATE := platform

LOCAL_PROGUARD_FLAG_FILES := proguard.flags
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti

include $(BUILD_PACKAGE)
endif
