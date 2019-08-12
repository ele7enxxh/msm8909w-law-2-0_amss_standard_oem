ifeq ($(strip $(TARGET_USES_NQ_NFC)),true)

LOCAL_PATH := $(call my-dir)

## NQ1x
include $(CLEAR_VARS)
LOCAL_MODULE             := libpn547_fw.so
LOCAL_MODULE_TAGS        := optional
LOCAL_MODULE_CLASS       := ETC
LOCAL_MODULE_PATH        := $(TARGET_OUT)/vendor/firmware
LOCAL_SRC_FILES_arm      := pn547c2/32-bit/$(LOCAL_MODULE)
LOCAL_SRC_FILES_arm64    := pn547c2/64-bit/$(LOCAL_MODULE)
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER       := nxp
LOCAL_MODULE_TARGET_ARCH := arm arm64
include $(BUILD_PREBUILT)

## NQ2x
include $(CLEAR_VARS)
LOCAL_MODULE             := libpn548ad_fw.so
LOCAL_MODULE_TAGS        := optional
LOCAL_MODULE_CLASS       := ETC
LOCAL_MODULE_PATH        := $(TARGET_OUT)/vendor/firmware
LOCAL_SRC_FILES_arm      := pn548ad/32-bit/$(LOCAL_MODULE)
LOCAL_SRC_FILES_arm64    := pn548ad/64-bit/$(LOCAL_MODULE)
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER       := nxp
LOCAL_MODULE_TARGET_ARCH := arm arm64
include $(BUILD_PREBUILT)

## NQ31x
include $(CLEAR_VARS)
LOCAL_MODULE             := libpn551_fw.so
LOCAL_MODULE_TAGS        := optional
LOCAL_MODULE_CLASS       := ETC
LOCAL_MODULE_PATH        := $(TARGET_OUT)/vendor/firmware
LOCAL_SRC_FILES_arm      := pn551/32-bit/$(LOCAL_MODULE)
LOCAL_SRC_FILES_arm64    := pn551/64-bit/$(LOCAL_MODULE)
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER       := nxp
LOCAL_MODULE_TARGET_ARCH := arm arm64
include $(BUILD_PREBUILT)

## NQ33x
include $(CLEAR_VARS)
LOCAL_MODULE             := libpn553_fw.so
LOCAL_MODULE_TAGS        := optional
LOCAL_MODULE_CLASS       := ETC
LOCAL_MODULE_PATH        := $(TARGET_OUT)/vendor/firmware
LOCAL_SRC_FILES_arm      := pn553/32-bit/$(LOCAL_MODULE)
LOCAL_SRC_FILES_arm64    := pn553/64-bit/$(LOCAL_MODULE)
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER       := nxp
LOCAL_MODULE_TARGET_ARCH := arm arm64
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := libnfc-mtp_default.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
#need to change source, once Nq330 changes are merged
LOCAL_MODULE       := libnfc-mtp_rf1.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := libnfc-mtp_rf2.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := libnfc-qrd_default.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE       := libnfc-qrd_rf1.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
#need to change source, once Nq330 changes are merged
LOCAL_MODULE       := libnfc-qrd_rf2.conf
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH  := $(TARGET_OUT_ETC)
LOCAL_SRC_FILES    := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

endif
