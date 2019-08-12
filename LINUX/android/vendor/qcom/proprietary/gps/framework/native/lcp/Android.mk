#=#====#====#====#====#====#====#====#====#====#====#====#====#====#====#====#
#
#        Location Service module - common
#
# GENERAL DESCRIPTION
#   Common location service module makefile
#
#=============================================================================
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := liblocationservice
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := ../adapter/LBSAdapter.cpp
LOCAL_SRC_FILES += ../service/loc_extended.cpp
LOCAL_SRC_FILES += observers/common/src/ClientIndex.cpp
LOCAL_SRC_FILES += observers/common/src/DataItemIndex.cpp
LOCAL_SRC_FILES += observers/common/src/IndexFactory.cpp
LOCAL_SRC_FILES += observers/IPCClient/src/IPCClient.cpp
LOCAL_SRC_FILES += observers/IPCHandler/src/IPCHandler.cpp
LOCAL_SRC_FILES += observers/OSObserver/src/OSObserver.cpp
LOCAL_SRC_FILES += observers/FreeWifiScanObserver/src/FreeWifiScanObserver.cpp
LOCAL_SRC_FILES += izat-provider-service/src/LocationProvider.cpp
LOCAL_SRC_FILES += izat-provider-service/src/IzatManager.cpp
LOCAL_SRC_FILES += izat-provider-service/src/ComboNetworkProvider.cpp
LOCAL_SRC_FILES += izat-provider-service/src/QNP.cpp
LOCAL_SRC_FILES += izat-provider-service/src/GTPWiFiProxy.cpp
LOCAL_SRC_FILES += izat-provider-service/src/GTPWWanProxy.cpp
LOCAL_SRC_FILES += izat-provider-service/src/ZppProxy.cpp
LOCAL_SRC_FILES += izat-provider-service/src/OSNPProxy.cpp
LOCAL_SRC_FILES += izat-location-listeners/src/IzatPassiveLocationListener.cpp
LOCAL_SRC_FILES += izat-location-listeners/src/XtraSubscriber.cpp
LOCAL_SRC_FILES += wiper/src/Wiper.cpp
LOCAL_SRC_FILES += ipc-messaging-proxy/src/IPCMessagingProxy.cpp
LOCAL_SRC_FILES += utils/src/Utils.cpp

LOCAL_SHARED_LIBRARIES := libulp2
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libloc_core
LOCAL_SHARED_LIBRARIES += libloc_eng
LOCAL_SHARED_LIBRARIES += libgps.utils
LOCAL_SHARED_LIBRARIES += liblbs_core
LOCAL_SHARED_LIBRARIES += libdataitems
LOCAL_SHARED_LIBRARIES += liblowi_client
LOCAL_SHARED_LIBRARIES += libizat_core

LOCAL_STATIC_LIBRARIES := libloc_mq_client
LOCAL_STATIC_LIBRARIES += libloc_base_util

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../service
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../adapter
LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/common/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/common/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/IPCHandler/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/IPCHandler/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/IPCClient/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/IPCClient/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/OSObserver/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/OSObserver/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/FreeWifiScanObserver/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/observers/FreeWifiScanObserver/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/izat-provider-service/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/izat-provider-service/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/izat-location-listeners/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/izat-location-listeners/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ipc-messaging-proxy/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ipc-messaging-proxy/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utils/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/utils/inc/internal
LOCAL_C_INCLUDES += $(LOCAL_PATH)/wiper/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/wiper/inc/internal
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/gps.utils
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libloc_eng
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libloc_api_v02
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qmi/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libloc_core
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/liblbs_core
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libloc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libloc/base_util
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libdataitems
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/liblowi_client/inc
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/liblowi_client
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/common/inc
LOCAL_C_INCLUDES += $(TOP)/external/connectivity/stlport/stlport
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libflp
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/libizat_core

LOCAL_CFLAGS += $(GPS_FEATURES)
LOCAL_CFLAGS += -D_ANDROID_
LOCAL_CFLAGS += -DON_TARGET_TEST

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := liblocationservice
LOCAL_COPY_HEADERS := ../service/ulp_service.h
LOCAL_COPY_HEADERS += ../service/loc_extended.h
LOCAL_COPY_HEADERS += ../adapter/LBSAdapter.h
LOCAL_COPY_HEADERS += ../geofence/GeoFencerProxyMsgs.h
include $(BUILD_COPY_HEADERS)

include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := liblocationservice/lcp/inc
LOCAL_COPY_HEADERS := inc/IIzatManager.h
LOCAL_COPY_HEADERS += inc/IDataItemSubscription.h
LOCAL_COPY_HEADERS += inc/IDataItemQuery.h
LOCAL_COPY_HEADERS += inc/IDataItemObserver.h
LOCAL_COPY_HEADERS += inc/IOSFramework.h
LOCAL_COPY_HEADERS += inc/IzatLocation.h
LOCAL_COPY_HEADERS += inc/IOSLocationProvider.h
LOCAL_COPY_HEADERS += inc/IIzatLocationListener.h
LOCAL_COPY_HEADERS += inc/IOSLocationListener.h
LOCAL_COPY_HEADERS += inc/IzatTypes.h
LOCAL_COPY_HEADERS += inc/IzatRequest.h
LOCAL_COPY_HEADERS += inc/IRawCommandInjector.h
LOCAL_COPY_HEADERS += inc/IOSFrameworkCleaner.h
LOCAL_COPY_HEADERS += utils/inc/IzatDefines.h
include $(BUILD_COPY_HEADERS)

include $(addsuffix /Android.mk,$(addprefix $(LOCAL_PATH)/, data-items))
