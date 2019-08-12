ifdef TARGET_USES_QCOM_BSP
ifeq (0,1)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := debug

LOCAL_PROGUARD_ENABLED := disabled

LOCAL_SRC_FILES := $(call all-subdir-java-files)

LOCAL_JAVA_LIBRARIES := telephony-common

LOCAL_PACKAGE_NAME := PrimaryCardController

LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)

endif
endif
