LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# Logging Features. Enable only one at any time
#LOCAL_CFLAGS += -DFEATURE_DATA_LOG_SYSLOG
LOCAL_CFLAGS += -DFEATURE_DATA_LOG_ADB
#LOCAL_CFLAGS += -DFEATURE_DATA_LOG_QXDM
LOCAL_CFLAGS += -DDIAG_ENABLED

ifeq ($(WCNSS_FILTER_USES_SIBS),true)
LOCAL_CFLAGS += -DWCNSS_IBS_ENABLED
endif #WCNSS_FILTER_USES_SIBS

# Additional Logging
# Uncomment for VERBOSE level messages
#LOCAL_CFLAGS += -DLOG_NDEBUG=0

# Additional Logging
# Uncomment for DEBUG level messages
#LOCAL_CFLAGS += -DLOG_NDDEBUG=0

# Additional Logging
# Uncomment for INFO level messages
#LOCAL_CFLAGS += -DLOG_NIDEBUG=0

# Ignore HCI_RESET in filter code
# so that HCI_RESET happens only once
# regardless of the client
#LOCAL_CFLAGS += -DIGNORE_HCI_RESET

# MIMIC cmd TOUT at filter
# used to check the behavior of the system
# by mimicing the cmd tout at host
# this is only for debug purpose
# DON'T ENABLE IT FOR NORMAL BT OPERATIONS
#LOCAL_CFLAGS += -DDEBUG_MIMIC_CMD_TOUT

LOCAL_SRC_FILES:= \
           src/main.c \
           src/bt_qxdmlog.c

#LOCAL_CFLAGS += -Werror -Wall -Wextra

ifeq ($(WCNSS_FILTER_USES_SIBS),true)
LOCAL_SRC_FILES += src/wcnss_ibs.c
endif

ifneq (,$(filter userdebug eng,$(TARGET_BUILD_VARIANT)))
LOCAL_CFLAGS += -DPANIC_ON_SOC_CRASH
LOCAL_CFLAGS += -DDUMP_IPC_LOG
endif

LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/diag/include

LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libdiag
LOCAL_MODULE:= wcnss_filter

LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
