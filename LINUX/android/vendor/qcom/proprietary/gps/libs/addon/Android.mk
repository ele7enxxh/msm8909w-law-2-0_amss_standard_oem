#/******************************************************************************
#*@file Android.mk
#* brief Rules to create an package which exposes the location api
#*******************************************************************************/
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:= com.qti.location.sdk

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, java)

LOCAL_SRC_FILES += \
    java/com/qualcomm/location/izat/IIzatService.aidl \
    java/com/qualcomm/location/izat/flp/IFlpService.aidl \
    java/com/qualcomm/location/izat/flp/ITestService.aidl \
    java/com/qualcomm/location/izat/flp/ILocationCallback.aidl \
    java/com/qualcomm/location/izat/flp/IMaxPowerAllocatedCallback.aidl \
    java/com/qualcomm/location/izat/geofence/IGeofenceService.aidl \
    java/com/qualcomm/location/izat/geofence/IGeofenceCallback.aidl

LOCAL_PROGUARD_FLAG_FILES := proguard.flags

LOCAL_MODULE_OWNER := qti

include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE:= com.qti.location.sdk.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(call all-makefiles-under,$(LOCAL_PATH))
