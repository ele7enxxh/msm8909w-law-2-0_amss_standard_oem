# SLIM Daemon make file

LOCAL_PATH := $(call my-dir)

# libslimclient
# Client library for using SLIM daemon services on processor
# Interface types: C/C++
# Published headers:
# libslimclient/slim_client_api.h   - C interface header
# libslimclient/slim_client_types.h - C/C++ types header
# libslimclient/SlimClient.h        - C++ interface header
include $(CLEAR_VARS)
LOCAL_MODULE := libslimclient
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES:= \
  src/QmiSlimClient.cpp \
  src/QmiSlimMonitor.cpp \
  src/SlimClientApi.cpp \
  src/SlimClientBase.cpp \
  src/SlimProxy.cpp \
  src/SlimProxyConnection.cpp
LOCAL_STATIC_LIBRARIES := \
  libslimcommon \
  libslimutils
LOCAL_SHARED_LIBRARIES := \
  libutils \
  libgps.utils \
  liblog \
  libqmi_cci \
  libqmi_common_so \
  libqmi_encdec \
  libdiag
LOCAL_C_INCLUDES += \
  $(TARGET_OUT_HEADERS)/common/inc \
  $(TARGET_OUT_HEADERS)/qmi-framework/inc \
  $(TARGET_OUT_HEADERS)/diag/include \
  $(TARGET_OUT_HEADERS)/gps.utils \
  $(TARGET_OUT_HEADERS)/libslimcommon \
  $(LOCAL_PATH)/inc \
  $(LOCAL_PATH)/src
# Install additional non-MP headers
LOCAL_COPY_HEADERS_TO:= libslimclient
LOCAL_COPY_HEADERS := \
  inc/SlimClientBase.h
LOCAL_CFLAGS += \
  $(GPS_FEATURES) \
  -DFEATURE_LOCTECH_SLIM_NOHANDLES \
  -fvisibility=hidden \
  -DSLIM_API="__attribute__ ((visibility (\"default\")))"
LOCAL_CPPFLAGS += \
  -D__STDC_FORMAT_MACROS \
  -D__STDC_CONSTANT_MACROS \
  -D__STDC_LIMIT_MACROS
LOCAL_EXPORT_C_INCLUDE_DIRS := \
  $(LOCAL_PATH)/inc \
  $(LOCAL_PATH)/src
include $(BUILD_SHARED_LIBRARY)
