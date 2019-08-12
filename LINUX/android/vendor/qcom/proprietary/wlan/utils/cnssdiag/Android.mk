LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := cnss_diag
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES := $(TARGET_OUT_HEADERS)/diag/include
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../uapi/linux
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../target/inc
LOCAL_SHARED_LIBRARIES := libc libcutils libdiag libhardware_legacy
LOCAL_SRC_FILES := cld-fwlog-netlink.c parser.c nan-parser.c cld-diag-parser.c
LOCAL_CFLAGS += -DCONFIG_ANDROID_LOG
LOCAL_CFLAGS += -DANDROID -Wall -Werror
ifeq ($(TARGET_USES_QCOM_BSP),true)
LOCAL_CFLAGS += -DQCOM_BSP
endif
LOCAL_LDLIBS += -llog
include $(BUILD_EXECUTABLE)
