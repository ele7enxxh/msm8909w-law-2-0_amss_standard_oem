ifneq ($(TARGET_USES_SSC),true)
ifneq ($(filter msm8960 msm8610 msm8916 msm8909,$(TARGET_BOARD_PLATFORM)),)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES:= \
    sendevent.cpp \
    file_parser.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils libhardware

LOCAL_MODULE:= test-sensorloopback

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
endif
endif
