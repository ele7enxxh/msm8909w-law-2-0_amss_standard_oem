ifdef TARGET_USES_QCOM_BSP
LOCAL_PATH := $(call my-dir)
########################
include $(CLEAR_VARS)

LOCAL_MODULE := QtiFeedback

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := APPS

LOCAL_MODULE_PATH := $(TARGET_OUT_APPS)

LOCAL_SRC_FILES := $(LOCAL_MODULE).apk

LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)

LOCAL_CERTIFICATE := platform

LOCAL_DEX_PREOPT := nostripping

include $(BUILD_PREBUILT)
########################

include $(CLEAR_VARS)
LOCAL_MODULE := libpoll_cpu_freq
ifeq ($(TARGET_ARCH),arm64)
  LOCAL_SRC_FILES := arm64/$(LOCAL_MODULE).so
else
  LOCAL_SRC_FILES := arm32/$(LOCAL_MODULE).so
endif
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
include $(BUILD_PREBUILT)
########################
include $(CLEAR_VARS)

LOCAL_MODULE := q_interface_permissions.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)
endif
