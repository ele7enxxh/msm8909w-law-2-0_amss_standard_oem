ifeq ($(call is-board-platform-in-list,msm8660 msm8960 msm8974 msm8226 msm8610 copper apq8084 msm8994 msm8992 msm8996 msm8952 msm8937 thorium titanium),true)
ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
#                 Common definitons
# ---------------------------------------------------------------------------------

libOmxWmaDec-def := -g -O3
libOmxWmaDec-def += -DQC_MODIFIED
libOmxWmaDec-def += -D_ANDROID_
libOmxWmaDec-def += -D_ENABLE_QC_MSG_LOG_
libOmxWmaDec-def += -DVERBOSE
libOmxWmaDec-def += -D_DEBUG
libOmxWmaDec-def += -DAUDIOV2
libOmxWmaDec-def += -Wconversion
ifeq ($(call is-board-platform-in-list,msm8610 apq8084 msm8996 msm8952 msm8937 thorium titanium),true)
libOmxWmaDec-def += -DQCOM_AUDIO_USE_SYSTEM_HEAP_ID
endif
ifeq ($(strip $(AUDIO_FEATURE_ENABLED_NT_PAUSE_TIMEOUT)),true)
libOmxWmaDec-def += -DNT_PAUSE_TIMEOUT_ENABLED
endif

# ---------------------------------------------------------------------------------
#             Make the Shared library (libOmxWmaDec)
# ---------------------------------------------------------------------------------

include $(CLEAR_VARS)

libOmxWmaDec-inc        := $(LOCAL_PATH)/inc
libOmxWmaDec-inc        += $(TARGET_OUT_HEADERS)/mm-core/omxcore
libOmxWmaDec-inc        += $(TARGET_OUT_HEADERS)/common/inc
libOmxWmaDec-inc        += $(BOARD_KERNEL_HEADER_DIR)

LOCAL_ADDITIONAL_DEPENDENCIES := $(BOARD_KERNEL_HEADER_DEPENDENCIES)

LOCAL_MODULE            := libOmxWmaDec
LOCAL_MODULE_TAGS       := optional
LOCAL_CFLAGS            := $(libOmxWmaDec-def)
LOCAL_C_INCLUDES        := $(libOmxWmaDec-inc)
LOCAL_PRELINK_MODULE    := false
LOCAL_SHARED_LIBRARIES  := libutils liblog

LOCAL_SRC_FILES         := src/adec_svr.c
LOCAL_SRC_FILES         += src/omx_wma_adec.cpp

LOCAL_MODULE_OWNER := qcom
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

# ---------------------------------------------------------------------------------
#             Make the apps-test (mm-adec-omxwma-test)
# ---------------------------------------------------------------------------------

include $(CLEAR_VARS)

mm-wma-dec-test-inc     := $(LOCAL_PATH)/inc
mm-wma-dec-test-inc     += $(LOCAL_PATH)/test
mm-wma-dec-test-inc     += $(TARGET_OUT_HEADERS)/mm-audio/audio-alsa
mm-wma-dec-test-inc     += $(TARGET_OUT_HEADERS)/mm-core/omxcore
mm-wma-dec-test-inc     += $(TARGET_OUT_HEADERS)/common/inc
mm-wma-dec-test-inc     += $(BOARD_KERNEL_HEADER_DIR)

LOCAL_MODULE            := mm-adec-omxwma-test
LOCAL_MODULE_TAGS       := optional
LOCAL_CFLAGS            := $(libOmxWmaDec-def)
LOCAL_C_INCLUDES        := $(mm-wma-dec-test-inc)
LOCAL_PRELINK_MODULE    := false
LOCAL_SHARED_LIBRARIES  := libmm-omxcore
LOCAL_SHARED_LIBRARIES  += libOmxWmaDec
LOCAL_SHARED_LIBRARIES  += libaudioalsa

LOCAL_SRC_FILES         := test/omx_wma_dec_test.c

LOCAL_ADDITIONAL_DEPENDENCIES := $(BOARD_KERNEL_HEADER_DEPENDENCIES)
include $(BUILD_EXECUTABLE)

endif #BUILD_TINY_ANDROID
endif #TARGET_BOARD_PLATFORM
# ---------------------------------------------------------------------------------
#                     END
# ---------------------------------------------------------------------------------

