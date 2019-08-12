LOCAL_PATH := $(call my-dir)

# common include paths
commonIncludes := $(TARGET_OUT_HEADERS)/common/inc
commonIncludes += $(TARGET_OUT_HEADERS)/qmi/inc
commonIncludes += $(TARGET_OUT_HEADERS)/sensors/inc

# sns_dsps_tc0001
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(commonIncludes)

LOCAL_MODULE := sns_dsps_tc0001
LOCAL_MODULE_OWNER := qti

LOCAL_SHARED_LIBRARIES := libsensor1

LOCAL_SRC_FILES := src/sns_dsps_tc0001.c

LOCAL_CFLAGS := -D_GNU_SOURCE

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

# sns_cm_conc_test
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(commonIncludes)

LOCAL_MODULE := sns_cm_conc_test
LOCAL_MODULE_OWNER := qti

LOCAL_SHARED_LIBRARIES := libsensor1

LOCAL_SRC_FILES := src/sns_cm_conc_test.c

LOCAL_CFLAGS := -D_GNU_SOURCE -DSNS_LA

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

# sns_cm_test
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(commonIncludes)

# Figure out if this getting built with the MSM kernel. If so, include
# the path to the sanitized headers in the target "out" dir.
ifeq ($(TARGET_COMPILE_WITH_MSM_KERNEL),true)
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
endif

LOCAL_MODULE := sns_cm_test
LOCAL_MODULE_OWNER := qti

LOCAL_SHARED_LIBRARIES := libutils
LOCAL_SHARED_LIBRARIES += libsensor1

LOCAL_SRC_FILES := src/sns_cm_test.cpp

LOCAL_CFLAGS := -D_GNU_SOURCE -DSNS_LA

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

# sns_regedit_ssi
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../sensordaemon/common/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../sensordaemon/common/util/mathtools/inc

LOCAL_MODULE := sns_regedit_ssi
LOCAL_MODULE_OWNER := qti

LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_SHARED_LIBRARIES += libsensor1

LOCAL_SRC_FILES := src/sns_regedit_ssi.c

LOCAL_CFLAGS := -D_GNU_SOURCE -DSNS_LA

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

# sns_ar_testapp
include $(CLEAR_VARS)
LOCAL_MODULE := sns_ar_testapp
LOCAL_MODULE_OWNER := qti

LOCAL_SHARED_LIBRARIES := libdl

LOCAL_SRC_FILES := src/sns_ar_testapp.c

LOCAL_CFLAGS := -D_GNU_SOURCE
LOCAL_CFLAGS += -DBOARD_PLATFORM=\"$(TARGET_BOARD_PLATFORM)\"

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

# sns_ping_test
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(commonIncludes)

LOCAL_MODULE := sns_ping_test
LOCAL_MODULE_OWNER := qti

LOCAL_SHARED_LIBRARIES := libsensor1

LOCAL_SRC_FILES := src/sns_ping_test.c

LOCAL_CFLAGS := -D_GNU_SOURCE -DSNS_LA

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

# sns_sam_test
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(commonIncludes)

LOCAL_MODULE := sns_sam_test
LOCAL_MODULE_OWNER := qti

LOCAL_SHARED_LIBRARIES := libsensor1

LOCAL_SRC_FILES := src/sns_sam_test.c

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

# sns_prox_test
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(commonIncludes)

LOCAL_MODULE := sns_prox_test
LOCAL_MODULE_OWNER := qti

LOCAL_SHARED_LIBRARIES := libsensor1

LOCAL_SRC_FILES := src/sns_prox_test.c

LOCAL_CFLAGS := -D_GNU_SOURCE -DSNS_LA

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

# sns_oem_test
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(commonIncludes)

LOCAL_MODULE := sns_oem_test
LOCAL_MODULE_OWNER := qti

LOCAL_SHARED_LIBRARIES := libsensor1

LOCAL_SRC_FILES := src/sns_oem_test.c

LOCAL_CFLAGS := -D_GNU_SOURCE -DSNS_LA

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

# sns_daf_test
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../api

LOCAL_MODULE := sns_daf_test
LOCAL_MODULE_OWNER := qti

LOCAL_SHARED_LIBRARIES := libsensor1

LOCAL_SRC_FILES := src/sns_daf_test.cpp

LOCAL_CFLAGS := -D_GNU_SOURCE -DSNS_LA

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

# test_smgr_buf
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../sensors/dsps/api
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc

LOCAL_MODULE := test_smgr_buf
LOCAL_MODULE_TAGS := optional eng
LOCAL_SHARED_LIBRARIES := libsensor1
LOCAL_PRELINK_MODULE := false
LOCAL_LDLIBS += -lm -llog

LOCAL_SRC_FILES := src/smgr_buf.c
include $(BUILD_EXECUTABLE)