LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#QTI SW FLAC Decoder
ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(strip $(AUDIO_FEATURE_ENABLED_EXTN_FLAC_DECODER)),true)

LOCAL_SRC_FILES := \
        SoftFlacDecoder.cpp

LOCAL_C_INCLUDES := \
        frameworks/av/media/libstagefright/include \
        frameworks/native/include/media/openmax \
        $(TARGET_OUT_HEADERS)/mm-audio/audio-flac \
        $(TOP)/hardware/qcom/media/mm-core/inc

LOCAL_CFLAGS := -Werror

LOCAL_SHARED_LIBRARIES := \
        libstagefright libstagefright_omx libstagefright_foundation libutils liblog libdl

LOCAL_MODULE := libstagefright_soft_flacdec

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_OWNER := qti

include $(BUILD_SHARED_LIBRARY)

endif
endif
