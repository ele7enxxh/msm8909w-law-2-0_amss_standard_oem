LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_OWNER := qti

LOCAL_PROPRIETARY_MODULE := true

LOCAL_MODULE:= libmmi

LOCAL_SRC_FILES := util_comm.cpp \
                   util_string.cpp \
                   util_system.cpp \
                   util_ui.cpp \
                   textview.cpp \
                   button.cpp \
                   listview.cpp \
                   layout.cpp \
                   nv.cpp \
                   events.cpp \
                   xmlparse.cpp \
                   lang.cpp \
                   board.cpp \
                   mmi_log.cpp


LOCAL_C_INCLUDES := external/libcxx/include \
                    external/skia/include/core \
                    external/libxml2/include \
                    external/icu/icu4c/source/common \
                    $(QC_PROP_ROOT)/diag/include \
                    $(QC_PROP_ROOT)/diag/src/ \
                    $(TARGET_OUT_HEADERS)/common/inc

LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -Wall

ifeq ($(TARGET_SUPPORTS_WEARABLES),true)
LOCAL_CFLAGS += -DANDROID_WEARABLES
else
LOCAL_CFLAGS += -DANDROID
endif

LOCAL_SHARED_LIBRARIES := libcutils libdiag libft2 libutils libxml2 libicuuc libc libui libbinder libgui libskia libc++

LOCAL_C_INCLUDES += $(BOARD_KERNEL_HEADER_DIR)
ifeq ($(TARGET_COMPILE_WITH_MSM_KERNEL),true)
LOCAL_ADDITIONAL_DEPENDENCIES := $(BOARD_KERNEL_HEADER_DEPENDENCIES)
endif

include $(BUILD_SHARED_LIBRARY)
