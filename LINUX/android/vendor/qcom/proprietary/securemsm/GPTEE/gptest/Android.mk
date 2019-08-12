LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(BOARD_KERNEL_HEADER_DIR) \
                    $(LOCAL_PATH)/../../QSEEComAPI \
                    $(LOCAL_PATH)/../../GPTEE/inc \
                    $(TARGET_OUT_HEADERS)/common/inc
LOCAL_ADDITIONAL_DEPENDENCIES := $(BOARD_KERNEL_HEADER_DEPENDENCIES)

LOCAL_SHARED_LIBRARIES := \
        libc \
        libcutils \
        libutils \
        libQSEEComAPI \
        libGPTEE

LOCAL_MODULE := gptest
LOCAL_SRC_FILES := gptest.c
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := $(QSEECOM_CFLAGS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qcom
LOCAL_32_BIT_ONLY := true
include $(BUILD_EXECUTABLE)
