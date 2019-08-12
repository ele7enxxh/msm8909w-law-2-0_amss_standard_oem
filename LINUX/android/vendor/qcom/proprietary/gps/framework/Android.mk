#/******************************************************************************
#*@file Android.mk
#* brief Rules to create an invisible package which declares the permissions
#* for accessing location api
#*******************************************************************************/
LOCAL_PATH := $(call my-dir)

ifneq ($(LW_FEATURE_SET),true)
include $(CLEAR_VARS)

#this tag means this package will be available for all eng builds
#note if the OEM decides to include this permission and group assignment in the final build, the
#tag should be changed to 'user'
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, java)

LOCAL_SRC_FILES += \
    java/com/android/location/XT/IXTSrvCb.aidl \
    java/com/android/location/XT/IXTSrv.aidl \
    java/com/qualcomm/location/izat/IIzatService.aidl \
    java/com/qualcomm/location/izat/flp/ILocationCallback.aidl \
    java/com/qualcomm/location/izat/flp/IMaxPowerAllocatedCallback.aidl \
    java/com/qualcomm/location/izat/flp/IFlpService.aidl \
    java/com/qualcomm/location/izat/flp/ITestService.aidl \
    java/com/qualcomm/location/izat/geofence/IGeofenceService.aidl \
    java/com/qualcomm/location/izat/geofence/IGeofenceCallback.aidl

LOCAL_JAVA_LIBRARIES := telephony-common \
                        com.android.location.provider

LOCAL_STATIC_JAVA_LIBRARIES := android-support-v4

LOCAL_PACKAGE_NAME := com.qualcomm.location
LOCAL_PRIVILEGED_MODULE := true
#LOCAL_PROGUARD_FLAG_FILES := proguard.flags
LOCAL_PROGUARD_ENABLED := disabled

LOCAL_CERTIFICATE := platform

include $(BUILD_PACKAGE)
endif # LW_FEATURE_SET

######## Copy permissions for com.qualcomm.location
include $(CLEAR_VARS)

LOCAL_MODULE := com.qualcomm.location.xml

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

ifneq ($(TARGET_NO_RPC),true)
    LOCAL_SRC_FILES := rpc/$(LOCAL_MODULE)
else
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
endif

include $(BUILD_PREBUILT)

ifneq ($(LW_FEATURE_SET), true)
include $(addsuffix /Android.mk, $(addprefix $(LOCAL_PATH)/,native))
endif #LW_FEATURE_SET
