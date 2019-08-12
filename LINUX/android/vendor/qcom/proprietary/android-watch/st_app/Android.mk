LOCAL_PATH := $(call my-dir)

ifeq ($(strip $(BOARD_SUPPORTS_SOUND_TRIGGER)),true)

#=============================================
#  Listen App using SoundTrigger
#=============================================
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_PROGUARD_FLAG_FILES := proguard.flags

LOCAL_SRC_FILES := $(call all-java-files-under, src)


# Build App to execute in 32-bit process
LOCAL_JNI_SHARED_LIBRARIES := liblistenjni

LOCAL_STATIC_JAVA_LIBRARIES := Listen \
	android-support-v4 \
	android-support-v7-appcompat

LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res \
	frameworks/support/v7/appcompat/res

LOCAL_AAPT_FLAGS := --auto-add-overlay \
	--extra-packages android.support.v7.appcompat
LOCAL_MULTILIB := 32

LOCAL_CERTIFICATE := platform

LOCAL_PACKAGE_NAME := STApp

#LOCAL_PRIVILEGED_MODULE := true

#LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))

endif
