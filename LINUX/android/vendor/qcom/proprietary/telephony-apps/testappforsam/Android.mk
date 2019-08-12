LOCAL_PATH:= $(call my-dir)
ifeq (0,1)
ifdef TARGET_USES_QCOM_BSP
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under,src)

LOCAL_JAVA_LIBRARIES := sammanagerlibrary
LOCAL_JAVA_LIBRARIES += saminterfacelibrary

LOCAL_STATIC_JAVA_LIBRARIES := \
    android-support-design \
    android-support-v4 \
    android-support-v7-appcompat

LOCAL_CERTIFICATE := platform

LOCAL_RESOURCE_DIR = \
        $(LOCAL_PATH)/res \
        frameworks/support/v7/appcompat/res \
        frameworks/support/design/res

LOCAL_AAPT_FLAGS := \
        --auto-add-overlay \
        --extra-packages android.support.design \
        --extra-packages android.support.v7.appcompat

LOCAL_PROGUARD_FLAG_FILES := proguard.flags

LOCAL_PACKAGE_NAME := TestAppForSAM

include $(BUILD_PACKAGE)
endif
endif
