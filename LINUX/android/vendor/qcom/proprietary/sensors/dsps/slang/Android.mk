ifneq ($(call is-board-platform-in-list,msm8909),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
commonSources  :=
commonIncludes := $(TARGET_OUT_HEADERS)/common/inc
commonIncludes += $(TARGET_OUT_HEADERS)/qmi/inc
commonIncludes += $(TARGET_OUT_HEADERS)/sensors/inc

LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc

LOCAL_MODULE:=slang
LOCAL_CLANG := true
LOCAL_MODULE_OWNER := qti

LOCAL_SHARED_LIBRARIES += \
    libsensor1

LOCAL_SRC_FILES += \
    src/array.c \
    src/builtin.c \
    src/namespace.c \
    src/alu.c \
    src/slnew_req.c \
    src/slcore.c \
    src/literals.c \
    src/parse.c \
    src/qmgr.c  \
    src/amd_service.c \
    src/smgr_service.c \
    src/reg_service.c \
    src/slmalloc.c \
    src/slfile.c \
    src/slprintf.c \
    src/diagnostics.c \
    src/time_service.c \
    src/grav_service.c \
    src/sam_common.c \
    src/grv_service.c \
    src/rotv_service.c \
    src/ornt_service.c \
    src/ped_service.c \
    src/slget.c

LOCAL_CFLAGS += -D_GNU_SOURCE -DSNS_LA
LOCAL_CFLAGS += -Wno-missing-field-initializers -O0

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
endif
