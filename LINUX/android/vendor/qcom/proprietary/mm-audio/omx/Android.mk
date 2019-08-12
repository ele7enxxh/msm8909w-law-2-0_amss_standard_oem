ifneq ($(TARGET_SUPPORTS_WEARABLES),true)
AUDIO_OMX := $(call my-dir)
include $(call all-subdir-makefiles)
endif
