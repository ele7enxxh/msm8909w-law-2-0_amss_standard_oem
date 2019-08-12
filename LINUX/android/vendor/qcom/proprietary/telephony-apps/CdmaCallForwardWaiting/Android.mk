ifdef TARGET_USES_QCOM_BSP

 LOCAL_PATH:= $(call my-dir)
 include $(CLEAR_VARS)
 LOCAL_MODULE_TAGS := optional eng

 res_dirs := res

 LOCAL_SRC_FILES := $(call all-subdir-java-files)
 LOCAL_RESOURCE_DIR := $(addprefix $(LOCAL_PATH)/, $(res_dirs))

 LOCAL_JAVA_LIBRARIES := telephony-common

 LOCAL_PACKAGE_NAME := CdmaCallOptions

 LOCAL_CERTIFICATE := platform

 include $(BUILD_PACKAGE)

endif
