
LOCAL_PATH:=$(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    location_hal_test.cpp

LOCAL_CFLAGS:= \
    -DDEBUG \
    -D_ANDROID_ \

LOCAL_C_INCLUDES += \
        $(TARGET_OUT_HEADERS)/gps.utils

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libgps.utils \
    libhardware \
    libandroid_runtime

LOCAL_PRELINK_MODULE:=false

ifdef PLATFORM_JACK_MIN_SDK_VERSION
LOCAL_CFLAGS += -DGNSS_CALLBACKS_PRESENT
endif

LOCAL_MODULE:=location_hal_test
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

