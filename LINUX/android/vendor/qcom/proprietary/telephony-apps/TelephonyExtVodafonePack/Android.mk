ifdef TARGET_USES_QCOM_BSP
LOCAL_PATH:= $(call my-dir)

ifeq (0,1)
include $(CLEAR_VARS)
LOCAL_JAVA_LIBRARIES := qcrilhook
LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_PACKAGE_NAME := TelephonyExtVodafonePack
LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true

include $(BUILD_PACKAGE)
endif
endif
