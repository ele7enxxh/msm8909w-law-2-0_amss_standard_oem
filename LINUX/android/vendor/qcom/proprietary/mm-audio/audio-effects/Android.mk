ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)
AUDIO_EFFECTS_ROOT := $(call my-dir)
include $(call all-subdir-makefiles)
endif
