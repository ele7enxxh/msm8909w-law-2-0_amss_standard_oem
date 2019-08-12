ifneq ($(BUILD_TINY_ANDROID),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := liblbs_core

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
    LBSAdapterBase.cpp \
    LBSApiBase.cpp \
    LBSProxy.cpp \
    LocApiProxy.cpp

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libizat_core \
    libloc_core \
    libgps.utils \
    libcrypto \
    libdl \
    libsqlite

LOCAL_STATIC_LIBRARIES := \
    libloc_base_util

LOCAL_C_INCLUDES := \
    $(TARGET_OUT_HEADERS)/gps.utils \
    $(TARGET_OUT_HEADERS)/libloc_core \
    $(TARGET_OUT_HEADERS)/libizat_core \
    $(TARGET_OUT_HEADERS)/liblocationservice/lcp/inc \
    $(TARGET_OUT_HEADERS)/common/inc \
    $(TARGET_OUT_HEADERS)/libflp \
    $(TOP)/external/connectivity/stlport/stlport

ifneq ($(TARGET_NO_RPC),true)
LOCAL_SRC_FILES += \
    LBSApiRpc.cpp

LOCAL_SHARED_LIBRARIES += \
    libloc_api-rpc-qc

LOCAL_C_INCLUDES += \
    $(TARGET_OUT_HEADERS)/loc_api/rpcgen/inc \
    $(TARGET_OUT_HEADERS)/libcommondefs/rpcgen/inc \
    $(TARGET_OUT_HEADERS)/librpc \
    $(TARGET_OUT_HEADERS)/libloc_api-rpc-qc/rpc_inc

LOCAL_CFLAGS+= \
    -D_HAS_LOC_RPC_
endif #TARGET_NO_RPC


ifeq ($(GPS_LOC_API_V02_ENABLED),true)
LOCAL_SRC_FILES += \
    LBSApiV02.cpp

LOCAL_SHARED_LIBRARIES += \
    libloc_api_v02

LOCAL_C_INCLUDES += \
    $(TARGET_OUT_HEADERS)/qmi/inc \
    $(TARGET_OUT_HEADERS)/qmi-framework/inc \
    $(TARGET_OUT_HEADERS)/libloc_api_v02 \
    $(TARGET_OUT_HEADERS)/libloc_ds_api

LOCAL_CFLAGS+= \
    -D_HAS_LOC_V02_

ifneq ($(FEATURE_MODEM_POWER_VOTE),false)
$(call print-vars, $(TARGET_DEVICE))
LOCAL_SHARED_LIBRARIES += \
    libmdmdetect \
    libperipheral_client

LOCAL_C_INCLUDES += \
    $(TARGET_OUT_HEADERS)/libmdmdetect/inc \
    $(TARGET_OUT_HEADERS)/libperipheralclient/inc
LOCAL_CFLAGS += \
    -DMODEM_POWER_VOTE
endif

endif #GPS_LOC_API_V02_ENABLED

LOCAL_COPY_HEADERS_TO:= liblbs_core/

LOCAL_COPY_HEADERS := \
    LBSAdapterBase.h \
    LBSApiBase.h \
    LBSApiV02.h \
    LBSProxy.h \
    LBSApiRpc.h \
    geofence.h \
    LocApiProxy.h \
    SlimSensor.h \
    WiperData.h

LOCAL_CFLAGS+=$(GPS_FEATURES) \
    -D_ANDROID_ \
    -DON_TARGET_TEST

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

endif # not BUILD_TINY_ANDROID


