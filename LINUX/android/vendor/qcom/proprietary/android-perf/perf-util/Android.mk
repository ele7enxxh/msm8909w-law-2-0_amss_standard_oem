LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    XmlParser.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libdl \
    libutils \
    libxml2

LOCAL_C_INCLUDES := external/libxml2/include \
                    external/icu/icu4c/source/common

LOCAL_CFLAGS += \
    -Wall \
    -DQC_DEBUG=0

LOCAL_MODULE := libqti-util
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
