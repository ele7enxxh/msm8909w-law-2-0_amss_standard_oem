#=#====#====#====#====#====#====#====#====#====#====#====#====#====#====#====#
#
#        Location Service module - jni glue
#
# GENERAL DESCRIPTION
#   JNI Glue location service module makefile
#
#=============================================================================
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := liblocationservice_glue
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := jni/src/LocationService_jni.cpp
LOCAL_SRC_FILES += jni/src/RilInfoMonitor_jni.cpp
LOCAL_SRC_FILES += jni/src/OSFramework_jni.cpp
LOCAL_SRC_FILES += jni/src/utils_jni.cpp
LOCAL_SRC_FILES += jni/src/Subscription_jni.cpp
LOCAL_SRC_FILES += jni/src/IzatProviderEngine_jni.cpp
LOCAL_SRC_FILES += jni/src/OSNPGlue_jni.cpp
LOCAL_SRC_FILES += jni/src/FlpServiceProvider_jni.cpp
LOCAL_SRC_FILES += jni/src/GeofenceServiceProvider_jni.cpp
LOCAL_SRC_FILES += common/src/RawCommandInjector.cpp
LOCAL_SRC_FILES += common/src/OSFrameworkCleaner.cpp
LOCAL_SRC_FILES += common/src/FusedLocationProviderGlue.cpp
LOCAL_SRC_FILES += common/src/NetworkLocationProviderGlue.cpp
LOCAL_SRC_FILES += common/src/LocationProviderGlue.cpp
LOCAL_SRC_FILES += common/src/OSNPGlue.cpp
LOCAL_SRC_FILES += common/src/Subscription.cpp
LOCAL_SRC_FILES += common/src/OSFramework.cpp

LOCAL_SHARED_LIBRARIES := liblocationservice
LOCAL_SHARED_LIBRARIES += libulp2
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libandroid_runtime
LOCAL_SHARED_LIBRARIES += libnativehelper
LOCAL_SHARED_LIBRARIES += libloc_core
LOCAL_SHARED_LIBRARIES += libgps.utils
LOCAL_SHARED_LIBRARIES += libhardware_legacy
LOCAL_SHARED_LIBRARIES += liblbs_core
LOCAL_SHARED_LIBRARIES += libdataitems
LOCAL_SHARED_LIBRARIES += libgeofence
LOCAL_SHARED_LIBRARIES += libflp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/common/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/jni/inc
LOCAL_C_INCLUDES += $(TOP)/frameworks/base/include/android_runtime
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/gps.utils
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libloc_core
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/liblbs_core
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/liblocationservice
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/liblocationservice/lcp/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libdataitems
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/gtp_client
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TOP)/external/connectivity/stlport/stlport
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libflp
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libizat_core
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libgeofence

LOCAL_CFLAGS += $(GPS_FEATURES)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := liblocationservice/jni/inc
LOCAL_COPY_HEADERS := jni/inc/IzatProviderEngine_jni.h
include $(BUILD_COPY_HEADERS)
