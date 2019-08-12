ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH := $(call my-dir)

DATAITEMS_HEADERS := \
    inc/DataItemConcreteTypeDefaultValues.h \
    inc/DataItemConcreteTypeFieldNames.h \
    inc/DataItemId.h \
    inc/DataItemsFactory.h \
    inc/IDataItem.h \
    inc/DataItemBase.h \
    inc/DataItemConcreteTypes.h \
    inc/IDataItemSerializer.h \
    inc/DataItemSerializerBase.h \
    inc/DataItemSerializerConcreteTypes.h \
    inc/IDataItemDeSerializer.h \
    inc/DataItemDeSerializerBase.h \
    inc/DataItemDeSerializerConcreteTypes.h \
    inc/IDataItemCopier.h \
    inc/DataItemCopierBase.h \
    inc/DataItemCopierConcreteTypes.h \
    inc/IDataItemStringifier.h \
    inc/DataItemStringifierBase.h \
    inc/DataItemStringifierConcreteTypes.h \
    inc/loc_ril.h

DATAITEMS_SRCS := \
    src/DataItemsFactory.cpp \
    src/DataItemBase.cpp \
    src/DataItemConcreteTypes.cpp \
    src/DataItemSerializerBase.cpp \
    src/DataItemSerializerConcreteTypes.cpp \
    src/DataItemDeSerializerBase.cpp \
    src/DataItemDeSerializerConcreteTypes.cpp \
    src/DataItemCopierBase.cpp \
    src/DataItemCopierConcreteTypes.cpp \
    src/DataItemStringifierBase.cpp \
    src/DataItemStringifierConcreteTypes.cpp

LOCAL_COPY_HEADERS := $(DATAITEMS_HEADERS)
LOCAL_COPY_HEADERS_TO := libdataitems
include $(BUILD_COPY_HEADERS)

#####################################################################

# Build libdataitems

include $(CLEAR_VARS)

LOCAL_MODULE := libdataitems
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

LOCAL_CFLAGS += \
    -D_ANDROID_

LOCAL_C_INCLUDES := \
    $(TOP)/system/core/include \
    $(LOCAL_PATH)/inc/internal \
    $(LOCAL_PATH)/inc \
    $(TARGET_OUT_HEADERS)/common/inc \
    $(TARGET_OUT_HEADERS)/liblocationservice/lcp/inc \
    $(TARGET_OUT_HEADERS)/libloc \
    $(TARGET_OUT_HEADERS)/libloc/base_util \
    $(TARGET_OUT_HEADERS)/liblbs_core \
    $(TARGET_OUT_HEADERS)/gps.utils

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libgps.utils \
    liblog

LOCAL_STATIC_LIBRARIES := \
    libloc_base_util

LOCAL_SRC_FILES := \
    $(DATAITEMS_SRCS)

LOCAL_MODULE_OWNER := qti

include $(BUILD_SHARED_LIBRARY)

#####################################################################
endif
