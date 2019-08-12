ifeq ($(strip $(BOARD_SUPPORTS_SOUND_TRIGGER)),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := smwrapper.c
LOCAL_C_INCLUDES += \
        system/core/include

LOCAL_SHARED_LIBRARIES := \
        libcutils

LOCAL_MODULE := libsmwrapper
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional
LOCAL_32_BIT_ONLY := true
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

endif
