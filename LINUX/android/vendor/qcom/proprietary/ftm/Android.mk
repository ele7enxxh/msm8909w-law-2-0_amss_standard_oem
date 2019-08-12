ifeq ($(call is-vendor-board-platform,QCOM),true)

# Build only if board has BT/FM/WLAN
ifeq ($(findstring true, $(BOARD_HAVE_QCOM_FM) $(BOARD_HAVE_BLUETOOTH) $(BOARD_HAS_ATH_WLAN_AR6320)),true)

LOCAL_PATH:= $(call my-dir)

BDROID_DIR:= system/bt
QCOM_DIR := $(BOARD_BT_HAL)/libbt-vendor

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(TARGET_OUT_HEADERS)/diag/include \
LOCAL_C_INCLUDES += $(QCPATH)/diag/src \
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc \
LOCAL_C_INCLUDES += $(QCPATH)/bt/hci_qcomm_init \
LOCAL_C_INCLUDES += $(BDROID_DIR)/hci/include \
LOCAL_C_INCLUDES += $(QCOM_DIR)/include \


LOCAL_CFLAGS:= \
              -DANDROID \
              -DDEBUG

LOCAL_CFLAGS += -include bionic/libc/include/sys/socket.h
LOCAL_CFLAGS += -include bionic/libc/include/netinet/in.h

LOCAL_CFLAGS +=  -DCONFIG_FTM_BT
ifeq ($(BOARD_HAVE_QCOM_FM),true)
LOCAL_CFLAGS +=  -DCONFIG_FTM_FM
endif
LOCAL_CFLAGS +=  -DCONFIG_FTM_ANT
LOCAL_CFLAGS +=  -DCONFIG_FTM_NFC

ifeq ($(BOARD_HAVE_BLUETOOTH_BLUEZ), true)
    LOCAL_CFLAGS += -DHAS_BLUEZ_BUILDCFG
endif # BOARD_HAVE_BLUETOOTH_BLUEZ

ifeq ($(BOARD_HAS_QCA_BT_ROME), true)
LOCAL_CFLAGS += -DBT_SOC_TYPE_ROME
endif

LOCAL_SRC_FILES:= ftm_main.c ftm_bt.c ftm_bt_power_pfal_linux.c ftm_bt_hci_pfal_linux.c ftm_bt_persist.cpp
LOCAL_SRC_FILES += ftm_nfc.c
ifeq ($(BOARD_HAVE_QCOM_FM),true)
LOCAL_SRC_FILES += ftm_fm.c ftm_fm_pfal_linux.c
endif
LOCAL_SRC_FILES += ftm_ant.c

ifeq ($(findstring true, $(BOARD_HAS_ATH_WLAN) $(BOARD_HAS_ATH_WLAN_AR6320)),true)
LOCAL_CFLAGS += -DBOARD_HAS_ATH_WLAN_AR6320
LOCAL_CFLAGS +=  -DCONFIG_FTM_WLAN
LOCAL_CFLAGS +=  -DCONFIG_FTM_WLAN_AUTOLOAD
LOCAL_STATIC_LIBRARIES += libtcmd
LOCAL_SHARED_LIBRARIES += libnl
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libtcmd
LOCAL_SRC_FILES  += ftm_wlan.c
endif

LOCAL_SHARED_LIBRARIES += libdl
LOCAL_MODULE:= ftmdaemon
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES  += libdiag
LOCAL_SHARED_LIBRARIES  += libbtnv libcutils libhardware

# By default NV persist gets used
LOCAL_CFLAGS += -DBT_NV_SUPPORT

LDFLAGS += -ldl

include $(BUILD_EXECUTABLE)
include $(call all-makefiles-under,$(LOCAL_PATH))

endif # filter
endif # is-vendor-board-platform
