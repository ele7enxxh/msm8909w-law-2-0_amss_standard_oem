ifeq ("disable_build_true","") #(Disable this project)
ifneq (, $(filter aarch64 arm64 arm, $(TARGET_ARCH)))
    $(info TODOAArch64: $(LOCAL_PATH)/Android.mk: Enable build support for 64 bit)
else
>>>>>>> 0371fda... Disable compilation of all projects except ims  in telephony-apps
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := debug

LOCAL_PROGUARD_ENABLED := disabled

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_JAVA_LIBRARIES := telephony-common

LOCAL_PACKAGE_NAME := GsmTuneAway

LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)
endif
endif #(Disable this project)
