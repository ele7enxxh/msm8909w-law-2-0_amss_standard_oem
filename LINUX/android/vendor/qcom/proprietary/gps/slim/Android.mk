# SLIM Daemon make file

# If the platform doesn't have QMI-LOC, SLIM daemon is not needed.
ifeq ($(GPS_LOC_API_V02_ENABLED),true)

LOCAL_PATH := $(call my-dir)
LOCTECH_SLIM_ROOT := $(LOCAL_PATH)

# Feature triggers:
# NO_GSIFF_DSPS           If not defined, enables Sensor1 provider by default
# GPS_LOC_API_V02_ENABLED If true, enables QMI-LOC client by default
ifndef NO_GSIFF_DSPS
ifndef FEATURE_LOCTECH_SLIM_DSPS
FEATURE_LOCTECH_SLIM_DSPS := true
endif
endif
ifeq ($(GPS_LOC_API_V02_ENABLED),true)
ifndef FEATURE_LOCTECH_SLIM_QMILOC
FEATURE_LOCTECH_SLIM_QMILOC := true
endif
endif
ifndef FEATURE_LOCTECH_SLIM_NDK
FEATURE_LOCTECH_SLIM_NDK := true
endif
# MP data source is enabled by default
ifndef FEATURE_LOCTECH_QMISLIM_PROVIDER_MP
FEATURE_LOCTECH_QMISLIM_PROVIDER_MP := false
endif
# Test data source is disabled by default
ifndef FEATURE_LOCTECH_QMISLIM_PROVIDER_TEST
FEATURE_LOCTECH_QMISLIM_PROVIDER_TEST := false
endif

# Vehicle network/test setup
# FEATURE_LOCTECH_SLIM_VNW := true

# slim_daemon
# FEATURE_LOCTECH_SLIM_NDK    := true  Enables Android Sensor provider
# FEATURE_LOCTECH_SLIM_VNW    := true  Enables Vehicle Network provider
# FEATURE_LOCTECH_SLIM_DSPS   := true  Enables Sensor1 provider
# FEATURE_LOCTECH_SLIM_QMILOC := true  Enables QMI-LOC client support

include $(CLEAR_VARS)

LOCAL_MODULE := slim_daemon
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/apss/listener \
  $(LOCAL_PATH)/apss/daemon/ \
  $(LOCAL_PATH)/provider/common \
  $(TARGET_OUT_HEADERS)/common/inc \
  $(TARGET_OUT_HEADERS)/qmi-framework/inc \
  $(TARGET_OUT_HEADERS)/gps.utils \
  $(TARGET_OUT_HEADERS)/libslimcommon \
  $(TARGET_OUT_HEADERS)/liblocationservice/lcp/inc \
  $(TARGET_OUT_HEADERS)/diag/include \
  $(TARGET_OUT_HEADERS)/libflp

LOCAL_SRC_FILES:= \
  apss/daemon/SlimDaemonManager.cpp \
  apss/listener/ClientListener.cpp \
  provider/common/ProviderBase.cpp \
  provider/common/MultiplexingProvider.cpp \
  provider/common/SlimProviderConf.cpp \
  provider/common/KalmanFilter.cpp
LOCAL_SHARED_LIBRARIES := \
  libgps.utils \
  liblog \
  libdiag \
  libqmi_csi \
  libqmi_cci \
  libqmi_common_so \
  libqmi_encdec
LOCAL_STATIC_LIBRARIES := \
  libslimcommon \
  libslimutils
LOCAL_CFLAGS += \
  $(GPS_FEATURES) \
  -DFEATURE_LOCTECH_SLIM_NOHANDLES \
  -DLOG_TAG=\"slim\" \
  -fvisibility=hidden \
  -DSLIM_API=""
LOCAL_CPPFLAGS += \
  -D__STDC_FORMAT_MACROS \
  -D__STDINT_LIMITS \
  -D__STDINT_MACROS
# Android NDK Provider Support
ifeq ($(FEATURE_LOCTECH_SLIM_NDK),true)
LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/provider/ndk
LOCAL_SRC_FILES += \
  provider/ndk/SlimNDKProvider.cpp
LOCAL_SHARED_LIBRARIES += \
  libutils \
  libgui \
  libbinder
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_SLIM_NDK -DFEATURE_LOCTECH_NEW_NDK_API

endif # FEATURE_LOCTECH_SLIM_NDK

# QMI-SLIM Provider Support
ifeq ($(FEATURE_LOCTECH_QMISLIM_PROVIDER_MP),true)
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_QMISLIM_PROVIDER_MP
endif
ifeq ($(FEATURE_LOCTECH_QMISLIM_PROVIDER_TEST),true)
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_QMISLIM_PROVIDER_TEST
endif
ifneq ($(FEATURE_LOCTECH_QMISLIM_PROVIDER_MP)$(FEATURE_LOCTECH_QMISLIM_PROVIDER_TEST),)
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_QMISLIM_PROVIDER
endif
# Sensor1 Provider Support
ifeq ($(FEATURE_LOCTECH_SLIM_DSPS),true)
LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/provider/sensor1 \
  $(TARGET_OUT_HEADERS)/sensors/inc
LOCAL_SRC_FILES += \
  provider/sensor1/SlimSensor1Provider.cpp
LOCAL_SHARED_LIBRARIES += libsensor1
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_SLIM_DSPS
endif
# Vehicle Network Provider Support
ifeq ($(FEATURE_LOCTECH_SLIM_VNW),true)
LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/provider/vnw
LOCAL_SRC_FILES += \
  provider/vnw/VehicleNetworkProvider.cpp
LOCAL_SHARED_LIBRARIES +=
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_SLIM_VNW
endif
# QMI-LOC Modem Client Support
ifeq ($(FEATURE_LOCTECH_SLIM_QMILOC),true)
LOCAL_C_INCLUDES += \
  $(TARGET_OUT_HEADERS)/libloc_api_v02 \
  $(TARGET_OUT_HEADERS)/libloc_core \
  $(TARGET_OUT_HEADERS)/liblbs_core
LOCAL_SHARED_LIBRARIES += \
  libloc_api_v02 \
  libloc_core \
  liblbs_core
LOCAL_SRC_FILES += \
  apss/listener/QLClientListener.cpp
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_SLIM_QMILOC
endif

ifeq ($(TARGET_BUILD_VARIANT),$(filter $(TARGET_BUILD_VARIANT),userdebug eng))
LOCAL_CFLAGS           += -DFEATURE_LOCTECH_SLIM_DEBUG
endif

include $(BUILD_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))

LOCTECH_SLIM_ROOT :=

endif

