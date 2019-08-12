#/******************************************************************************
#*@file Android.mk
#*brief Rules for compiling the source files
#*******************************************************************************/

LOCAL_PATH:= $(call my-dir)

#  Build the interface files into a library
# for clients to use.

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := debug
LOCAL_MODULE := radioconfiginterfacelibrary

LOCAL_SRC_FILES := src/com/qualcomm/qti/radioconfiginterface/IRadioConfig.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/ILtedProvisioningListener.aidl
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/RadioConfigIntegerList.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/RadioConfigStringList.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/RadioConfigItem.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/RadioConfigParcelable.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/LtedConfig.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/LtedCategory.java

include $(BUILD_JAVA_LIBRARY)


# ==========================================================================
#  Build the interface files into a library
# for clients to use.

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := debug
LOCAL_MODULE := radioconfiglibrary
LOCAL_JAVA_LIBRARIES := radioconfiginterfacelibrary

LOCAL_SRC_FILES := src/com/qualcomm/qti/radioconfiginterface/RadioConfig.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/RadioConfigItem.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/LtedConfig.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/LtedCategory.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/LtedProvisioningListener.java

include $(BUILD_JAVA_LIBRARY)

# ==========================================================================

# Build Library with nano-pb generated .c/.h files from proto
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= src/radio_config_interface.proto

LOCAL_C_INCLUDES += external/nanopb-c/

LOCAL_C_FILES :=

LOCAL_PROTOC_OPTIMIZE_TYPE := nanopb-c-enable_malloc

LOCAL_MODULE:= libril-qc-radioconfig

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

radioconfig_generated_dir := $(call generated-sources-dir-for,SHARED_LIBRARIES,$(LOCAL_MODULE),,)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(radioconfig_generated_dir)/proto/$(LOCAL_PATH)/src

include $(BUILD_SHARED_LIBRARY)

# ==========================================================================

# Build the service
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := debug
src_proto := src
LOCAL_SRC_FILES := src/com/qualcomm/qti/radioconfiginterface/RadioConfigService.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/RadioConfigSocket.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/RadioConfigMsgPacker.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/RadioConfigSyncRequest.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/RadioConfigBootReceiver.java
LOCAL_SRC_FILES += src/com/qualcomm/qti/radioconfiginterface/LtedAuthManager.java
LOCAL_SRC_FILES += $(call all-proto-files-under, $(src_proto))

LOCAL_PROTOC_OPTIMIZE_TYPE := micro

LOCAL_PACKAGE_NAME := radioconfig
LOCAL_CERTIFICATE := platform
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_JAVA_LIBRARIES := radioconfiglibrary
LOCAL_JAVA_LIBRARIES += radioconfiginterfacelibrary

include $(BUILD_PACKAGE)

# ==========================================================================

include $(CLEAR_VARS)

LOCAL_MODULE := radioconfig.xml
LOCAL_MODULE_TAGS := debug

LOCAL_MODULE_CLASS := ETC

#this will install the file in /system/etc/permissions
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

# ==========================================================================

include $(CLEAR_VARS)

LOCAL_MODULE := radioconfiginterface.xml
LOCAL_MODULE_TAGS := debug

LOCAL_MODULE_CLASS := ETC

#this will install the file in /system/etc/permissions
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)
