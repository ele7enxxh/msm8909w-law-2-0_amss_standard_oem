#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling the source files
#*******************************************************************************/

LOCAL_PATH:= $(call my-dir)

src_java := src
src_aidl := src

# ==========================================================================
# Build The Library
# ==========================================================================

include $(CLEAR_VARS)

LOCAL_MODULE := qti-vzw-ims-internal
LOCAL_MODULE_TAGS := optional

LOCAL_JAVA_LIBRARIES := voip-common

LOCAL_CERTIFICATE := platform
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

LOCAL_PROGUARD_ENABLED := disabled

LOCAL_AIDL_INCLUDES := $(call all-Iaidl-files-under, $(src_aidl))
LOCAL_SRC_FILES := $(call all-java-files-under, $(src_java_ims)) \
        $(LOCAL_AIDL_INCLUDES)

LOCAL_AIDL_INCLUDES += frameworks/native/aidl/gui

include $(BUILD_JAVA_LIBRARY)


# ==========================================================================
# Install The Library
# ==========================================================================

include $(CLEAR_VARS)

LOCAL_MODULE := qti-vzw-ims-internal.xml
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

# ==========================================================================
# Build Subdirectories
# ==========================================================================

include $(call all-makefiles-under,$(LOCAL_PATH))
