
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    aptXScheduler.c \
    aptXCaps.c

LOCAL_COPY_HEADERS    := aptXCaps.h
LOCAL_COPY_HEADERS    += aptXScheduler.h
LOCAL_COPY_HEADERS_TO := bt/hci_qcomm_init/aptX

BDROID_DIR := $(TOP)/system/bt

LOCAL_C_INCLUDES += \
    $(BDROID_DIR)/include \
    $(BDROID_DIR)/utils/include \
    $(BDROID_DIR)/btif/include \
    $(BDROID_DIR)/stack/include \
    $(LOCAL_PATH)/../16bit/include \
    $(BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR)

bdroid_CFLAGS += -DHAS_NO_BDROID_BUILDCFG

LOCAL_CFLAGS += -DBUILDCFG $(bdroid_CFLAGS)
LOCAL_CFLAGS += -Wno-implicit-function-declaration
LOCAL_MULTILIB := 32

LOCAL_CONLYFLAGS := -std=c99

LOCAL_SHARED_LIBRARIES := \
    liblog \
#    libaptX-1.0.0-rel-Android21-ARMv7A

LOCAL_MODULE := libaptXScheduler

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_EXPORT_C_INCLUDE_DIRS  := $(LOCAL_PATH)
LOCAL_PROPRIETARY_MODULE := true


include $(BUILD_SHARED_LIBRARY)

