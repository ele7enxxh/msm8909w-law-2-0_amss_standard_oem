ifeq ($(TARGET_BUILD_JAVA_SUPPORT_LEVEL),platform)

LOCAL_PATH := $(call my-dir)
ifeq (1,1)
include $(CLEAR_VARS)

LOCAL_AIDL_INCLUDES := $(LOCAL_PATH)/src/java
LOCAL_SRC_FILES := $(call all-java-files-under, src/java) \
	$(call all-Iaidl-files-under, src/java) \
	$(call all-logtags-files-under, src/java)

LOCAL_JAVA_LIBRARIES := voip-common ims-common telephony-common telephony-ext

LOCAL_STATIC_JAVA_LIBRARIES := qcrilhook-static

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := qti-telephony-common

include $(BUILD_JAVA_LIBRARY)

# ==========================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := qti_telephony_common.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
#
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

# ==========================================================================

endif
endif # JAVA platform
