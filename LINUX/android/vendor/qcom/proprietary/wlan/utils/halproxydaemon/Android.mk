ifeq ($(BOARD_HAS_QCOM_WLAN), true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := hal_proxy_daemon
LOCAL_CLANG := true
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES :=
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += \
	$(call include-path-for, libhardware_legacy)/hardware_legacy \
	hardware/qcom/wlan/qcwcn/wifi_hal

LOCAL__CPP_EXTENSION := .cpp

LOCAL_SHARED_LIBRARIES :=

ifneq ($(wildcard external/libnl),)
LOCAL_SHARED_LIBRARIES += libnl
endif

LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libwifi-hal-qcom
LOCAL_SHARED_LIBRARIES += libhardware_legacy

LOCAL_SRC_FILES :=
LOCAL_SRC_FILES += src/halProxyDaemon.cpp
LOCAL_SRC_FILES += src/nan_test.cpp
LOCAL_SRC_FILES += src/llstats_test.cpp
LOCAL_SRC_FILES += src/gscan_test.cpp
LOCAL_SRC_FILES += src/rtt_test.cpp
LOCAL_SRC_FILES += src/tdls_test.cpp
LOCAL_SRC_FILES += src/wifihal_test.cpp
LOCAL_SRC_FILES += src/wificonfig_test.cpp
LOCAL_SRC_FILES += src/wifilogger_test.cpp
LOCAL_SRC_FILES += src/wifi_calling_test.cpp
LOCAL_SRC_FILES += src/packet_filtering.cpp
LOCAL_SRC_FILES += src/nd_offload.cpp

HAL_PROXY_DAEMON_GIT_VER := $(shell cd $(LOCAL_PATH)/ && git describe --always)
LOCAL_CFLAGS += -DHAL_PROXY_DAEMON_VER=\"$(HAL_PROXY_DAEMON_GIT_VER)\"
LOCAL_CFLAGS += -Wall -Werror

LOCAL_LDLIBS += -Lpthread

include $(BUILD_EXECUTABLE)

endif # Global WLAN enable check
