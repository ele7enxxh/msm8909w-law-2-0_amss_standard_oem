ifeq ($(call is-board-platform-in-list, msm8952),true)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
                   InstallKeybox.cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc                \
                    $(LOCAL_PATH)/../../inc          \
                    $(TARGET_OUT_HEADERS)/common/inc \
                    $(TOP)/system/keymaster/include \
                    $(TOP)/vendor/qcom/proprietary/securemsm/QSEEComAPI \
                    $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
                    $(TOP)/external/boringssl/include \
                    $(TOP)/hardware/libhardware/include/hardware

LOCAL_SHARED_LIBRARIES := \
                          liblog                \
                          libutils              \
                          libz                  \
                          libdl                 \
                          libcutils             \
                          liblog                \
                          libQSEEComAPI         \
                          libhardware           \
                          libcrypto             \
                          libkeymasterprovision \

LOCAL_MODULE := KmInstallKeybox
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_OWNER := qti
include $(BUILD_EXECUTABLE)

endif # is-board-platform-in-list
