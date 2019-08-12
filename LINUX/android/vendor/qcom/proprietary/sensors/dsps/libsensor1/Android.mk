LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

commonSources  :=
commonIncludes += $(TARGET_OUT_HEADERS)/sensors/inc
commonIncludes += $(TARGET_OUT_HEADERS)/common/inc
commonIncludes += $(TARGET_OUT_HEADERS)/diag/include
commonIncludes += $(TARGET_OUT_HEADERS)/qmi-framework/inc

LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../api
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../sensordaemon/common/idl/inc

LOCAL_MODULE:=libsensor1

LOCAL_SHARED_LIBRARIES :=
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libqmi_common_so
LOCAL_SHARED_LIBRARIES += libqmi_cci
LOCAL_SHARED_LIBRARIES += libqmi_encdec
LOCAL_SHARED_LIBRARIES += libc libutils libcutils
LOCAL_SHARED_LIBRARIES += libdiag

LOCAL_SRC_FILES += src/libsensor1.c
LOCAL_SRC_FILES += src/libsensor1_la.c
LOCAL_SRC_FILES += src/libsensor1_rtb.c
LOCAL_SRC_FILES += src/libsensor1_log.c
LOCAL_SRC_FILES += src/sns_memmgr_native.c
LOCAL_SRC_FILES += src/sns_osa_linux.c
LOCAL_SRC_FILES += src/sns_em_la.c
LOCAL_SRC_FILES += \
  $(shell find $(LOCAL_PATH)/../sensordaemon/common/idl/src/* -name '*.c' | grep  'src/.*\.c' | sed s:^$(LOCAL_PATH)/::g )

LOCAL_CFLAGS += -D_GNU_SOURCE -DSNS_LA
LOCAL_CFLAGS += -Wno-missing-field-initializers -Werror

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
