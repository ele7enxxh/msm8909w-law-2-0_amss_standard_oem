ifeq ($(call is-vendor-board-platform,QCOM),true)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
#only compile on targets with libmdmdetect
ifneq ($(wildcard $(LOCAL_PATH)/../../mdm-helper/libmdmdetect),)
LOCAL_ADDITIONAL_DEPENDENCIES += $(BOARD_KERNEL_HEADER_DEPENDENCIES)
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc/ \
		    $(TARGET_OUT_HEADERS)/libmdmdetect/inc \
		    $(BOARD_KERNEL_HEADER_DIR)
LOCAL_SRC_FILES := ssr_setup.c
LOCAL_MODULE := ssr_setup
LOCAL_SHARED_LIBRARIES += libc libcutils libutils libmdmdetect
LOCAL_MODULE_TAG := optional
LOCAL_CFLAGS += -Wall
LOCAL_MODULE_OWNER := qcom
include $(BUILD_EXECUTABLE)
endif
endif
