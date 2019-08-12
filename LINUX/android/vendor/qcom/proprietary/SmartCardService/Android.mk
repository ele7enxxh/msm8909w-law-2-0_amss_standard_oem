ifeq ($(strip $(TARGET_ENABLE_PROPRIETARY_SMARTCARD_SERVICE)),true)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

define all-java-files-but-ese-term
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "*.java" -and -not -name ".*" -and -not -name "eSETerminal.java") \
 )
endef

define all-java-files-oma-only
$(patsubst ./%,%, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "*.java" -and -not -name ".*" -and -not -name "eSETerminal.java"
          -and -not -name "NfcIntentsService.java") \
 )
endef

ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
ifeq ($(strip $(TARGET_USES_QPAY)),true)
LOCAL_SRC_FILES := $(call all-java-files-under, src)
else
LOCAL_SRC_FILES := $(call all-java-files-but-ese-term, src)
endif
else
LOCAL_SRC_FILES := $(call all-java-files-oma-only, src)
endif

LOCAL_AIDL_INCLUDES := $(LOCAL_PATH)/common/src/

LOCAL_PACKAGE_NAME := SmartcardService
LOCAL_CERTIFICATE := platform
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := qti
LOCAL_MODULE_TAGS := optional

LOCAL_JAVA_LIBRARIES := org.simalliance.openmobileapi
ifeq ($(strip $(TARGET_USES_QPAY)),true)
LOCAL_JAVA_LIBRARIES += com.android.qti.qpay
endif
ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)
LOCAL_JAVA_LIBRARIES += com.nxp.nfc.nq com.android.nfc_extras
endif

LOCAL_PROGUARD_ENABLED := disabled

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))
endif
