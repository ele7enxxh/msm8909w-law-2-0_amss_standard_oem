LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# ---------------------------------------------------------------------------------
#               Common definitons
# ---------------------------------------------------------------------------------
include $(BUILD_COPY_HEADERS)

libvpp-omx-def += -g -O3
libvpp-omx-def += -Werror -std=c++11
#libvpp-omx-def += -Wno-parentheses


include $(CLEAR_VARS)

# Common Includes
libvpp-omx-inc          := $(LOCAL_PATH)/inc
libvpp-omx-inc          += $(TOP)/hardware/qcom/media/mm-video-v4l2/vidc/common/inc
libvpp-omx-inc          += $(TOP)/hardware/qcom/media/mm-core/inc
libvpp-omx-inc          += $(TOP)/hardware/qcom/media/mm-core/src/common
libvpp-omx-inc          += $(TOP)/vendor/qcom/proprietary/vpp/inc/
#DRM include - Interface which loads the DRM library
libvpp-omx-inc          += $(TARGET_OUT_HEADERS)/qcom/display
libvpp-omx-inc          += $(TARGET_OUT_HEADERS)/adreno
libvpp-omx-inc          += $(TOP)/frameworks/native/include/media/openmax
libvpp-omx-inc          += $(TOP)/frameworks/native/include/media/hardware
libvpp-omx-inc          += $(TOP)/frameworks/av/include/media/stagefright
libvpp-omx-inc          += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

libvpp-omx-inc += $(TOP)/hardware/qcom/media/libstagefrighthw

# Common Dependencies
libvpp-omx-add-dep := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr


# ---------------------------------------------------------------------------------
#           Make the Shared library (libOmxVdec)
# ---------------------------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE                    := libOmxVpp
LOCAL_MODULE_TAGS               := optional
LOCAL_CFLAGS                    := $(libvpp-omx-def)
LOCAL_C_INCLUDES                += $(libvpp-omx-inc)
LOCAL_ADDITIONAL_DEPENDENCIES   := $(libvpp-omx-add-dep)

LOCAL_PRELINK_MODULE    := false
LOCAL_SHARED_LIBRARIES  := liblog libutils libbinder libcutils libdl libmm-omxcore

LOCAL_SHARED_LIBRARIES  += libqdMetaData
LOCAL_SHARED_LIBRARIES  += libvpplibrary

LOCAL_SRC_FILES         := src/vpp_omx_component.cpp
LOCAL_SRC_FILES         += src/vpp_buffer_manager.cpp

include $(BUILD_SHARED_LIBRARY)


# ---------------------------------------------------------------------------------
#                END
# ---------------------------------------------------------------------------------
