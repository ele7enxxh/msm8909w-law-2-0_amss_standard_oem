ifneq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)
ifneq ($(TARGET_USES_AOSP), true)
include $(call all-subdir-makefiles)
endif
endif
