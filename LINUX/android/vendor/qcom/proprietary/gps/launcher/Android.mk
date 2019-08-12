LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := loc_launcher

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libgps.utils

LOCAL_STATIC_LIBRARIES := \
  libloc_base_util \

LOCAL_SRC_FILES += \
    process_launcher.c \
    mq_server/mq_server.cpp \
    mq_server/mq_client_info.cpp \
    mq_server/mq_server_launcher.cpp

LOCAL_CFLAGS += \
    -fno-short-enums \
    -D_ANDROID_

## Includes
LOCAL_C_INCLUDES := \
    $(TARGET_OUT_HEADERS)/gps.utils \
    $(TARGET_OUT_HEADERS)/libloc/base_util \
    $(TARGET_OUT_HEADERS)/libloc \
    mq_server

include $(BUILD_EXECUTABLE)
