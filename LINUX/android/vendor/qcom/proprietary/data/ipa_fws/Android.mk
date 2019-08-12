TARGET_LIST := msmcobalt

ifeq ($(call is-board-platform-in-list,$(TARGET_LIST)),true)

# IPAv3.1 (8998) base addresses
DPS_ADDR := 0x01E5E000
HPS_ADDR := 0x01E5E080
GSI_ADDR := 0x01E08000
GSI_MEM_SIZE := 0x4000
SOC_HW_VERSION := 0x30020000

# For 8997 HW version
SOC_VERS := 0x3005

LOCAL_PATH := $(call my-dir)
SECIMAGE_BASE := vendor/qcom/proprietary/common/scripts/SecImage
SECIMAGE_CONFIG_FILE := config/integration/secimagev2.xml
PILSPLITTER_BASE := vendor/qcom/proprietary/common/scripts

# Assemble binaries/firmwares to a single ELF file
RESULT := $(shell python $(LOCAL_PATH)/elf_creator.py \
	--gsi_fw $(LOCAL_PATH)/fw_mhi_ipa.bin \
	--gsi_fw_address $(GSI_ADDR) \
	--gsi_fw_mem_size $(GSI_MEM_SIZE) \
	--hps $(LOCAL_PATH)/hps.bin \
	--hps_addr $(HPS_ADDR) \
	--dps $(LOCAL_PATH)/dps.bin \
	--dps_addr $(DPS_ADDR) \
	--outfile $(LOCAL_PATH)/ipa_fws.elf)

$(info Creating ipa ELF image OUT folders)
$(shell mkdir -p $(PRODUCT_OUT)/ipa)
$(shell mkdir -p $(PRODUCT_OUT)/ipa/signed)
$(shell mkdir -p $(PRODUCT_OUT)/ipa/unsigned)

$(info install unsigned ipa_fws.elf at $(PRODUCT_OUT)/ipa/unsigned)
$(shell cp -f $(LOCAL_PATH)/ipa_fws.elf $(PRODUCT_OUT)/ipa/unsigned)

# Sign the ELF file using SecImage tool
RESULT := $(shell SECIMAGE_LOCAL_DIR=$(SECIMAGE_BASE) USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN=$(USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN) \
	python $(SECIMAGE_BASE)/sectools_builder.py \
	-i $(LOCAL_PATH)/ipa_fws.elf \
	-g ipa_fw \
	-t $(LOCAL_PATH) \
	--install_base_dir=$(LOCAL_PATH) \
	--soc_hw_version=$(SOC_HW_VERSION) \
	--soc_vers=$(SOC_VERS) \
	--config $(SECIMAGE_BASE)/$(SECIMAGE_CONFIG_FILE) \
	> $(LOCAL_PATH)/secimage.log 2>&1)

$(info install signed ipa_fws.elf at $(PRODUCT_OUT)/ipa/signed)
$(shell cp -f $(LOCAL_PATH)/ipa_fws.elf $(PRODUCT_OUT)/ipa/signed)

# PIL split the output of the SecImage tool
RESULT := $(shell python $(PILSPLITTER_BASE)/pil-splitter.py \
	$(LOCAL_PATH)/ipa_fws.elf \
	$(LOCAL_PATH)/ipa_fws)

# Remove the SecImage tool intermediate output
RESULT := $(shell rm -rf $(LOCAL_PATH)/signed \
	$(LOCAL_PATH)/sign \
	$(LOCAL_PATH)/signed_encrypted \
	$(LOCAL_PATH)/sign_and_encrypt)

include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.elf
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.b00
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.b01
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.b02
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

# Only if the policy is defined, these files will be created
ifeq ($(USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN),1)
include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.b03
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.b04
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
endif # Signing policy check

include $(CLEAR_VARS)
LOCAL_MODULE := ipa_fws.mdt
LOCAL_MODULE_OWNER := ipa
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/firmware
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
endif # Target check
