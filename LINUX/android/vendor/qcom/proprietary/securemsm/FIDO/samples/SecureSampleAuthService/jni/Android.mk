# ---------------------------------------------------------------------------------
#                 Common definitions
# ---------------------------------------------------------------------------------
LOCAL_PATH            := $(call my-dir)
SECUREMSM_SHIP_PATH   := $(QCPATH)/securemsm
AUTH_SHIP             := $(SECUREMSM_SHIP_PATH)/authentication/include

COMMON_INCLUDES     :=  $(SSE_PATH)/common/include \
                        $(BOARD_KERNEL_HEADER_DIR) \
                        $(AUTH_SHIP) \
                        $(SECUREMSM_SHIP_PATH)/sse/common/daemon \
                        $(SECUREMSM_SHIP_PATH)/QSEEComAPI \

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libSecureSampleAuthJNI
#FIXME: Disabled for Feldspar bringup
LOCAL_CLANG := false
LOCAL_CFLAGS := -g -fdiagnostics-show-option -Wno-format \
                -Wno-missing-braces -Wno-missing-field-initializers \
                -std=gnu++0x -fpermissive -Wno-unused-parameter

LOCAL_PRELINK_MODULE := false

LOCAL_C_INCLUDES += $(COMMON_INCLUDES)

# Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := SecureSampleAuthImpl_interface.cpp
LOCAL_SHARED_LIBRARIES += \
        libbinder \
        libandroid_runtime \
        liblog \
        libutils \
        libSecureSampleAuthClient

LOCAL_PROPRIETARY_MODULE := true

LOCAL_MODULE_OWNER := qti
include $(BUILD_SHARED_LIBRARY)

