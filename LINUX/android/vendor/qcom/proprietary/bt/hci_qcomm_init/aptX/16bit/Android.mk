# version of main bluedroid source
APTX_BLUEDROID_VERSION := 7.0.0
# version of aptX encoder library
APTX_ENCODER_VERSION := 1.0.0-rel
LOCAL_PATH := $(call my-dir)
LOCAL_DIR_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# uncomment all the statements below after copying
# encoder library in this folder

#LOCAL_MODULE := libaptX-$(APTX_ENCODER_VERSION)-Android21-ARMv7A
#LOCAL_32_BIT_ONLY := true
#LOCAL_SRC_FILES := libaptX-$(APTX_ENCODER_VERSION)-Android21-ARMv7A.so
#LOCAL_MODULE_SUFFIX := .so
#LOCAL_MODULE_CLASS := SHARED_LIBRARIES
#LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
#LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE_OWNER := qti
#LOCAL_EXPORT_C_INCLUDE_DIRS  := $(LOCAL_PATH)/include
#LOCAL_PROPRIETARY_MODULE := true

#include $(BUILD_PREBUILT)
