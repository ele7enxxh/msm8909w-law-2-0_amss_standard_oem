LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

DEVICE_COMMON_PATH := device/qcom/common

ifeq ($(TARGET_PRODUCT),msm8909w)
    DEVICE_COMMON_PATH := $(BOARD_COMMON_DIR)
endif

LOCAL_SRC_FILES := \
        ActiveRequestList.cpp \
	ResourceQueues.cpp \
	OptsHandler.cpp \
        RestoreHandler.cpp \
	OptsData.cpp \
	ResourceInfo.cpp \
	Request.cpp \
	Target.cpp \
	TargetInit.cpp \
        MpctlUtils.cpp \
        mp-ctl.cpp

LOCAL_SHARED_LIBRARIES := libcutils libthermalclient libutils

LOCAL_C_INCLUDES += \
        $(DEVICE_COMMON_PATH)/power $(DEVICE_COMMON_PATH)/thermal-engine

LOCAL_CFLAGS += \
        -DSERVER \
        -DCLIENT \
	-DPERFD \
        -g0 \
        -Wall \
        -DOEM_PROFILE_RESTORE_ON_FAIL=0 \
	#-DQC_DEBUG=1

ifeq ($(call is-android-codename,JELLY_BEAN),true)
    LOCAL_CFLAGS += -DANDROID_JELLYBEAN=1

    LOCAL_C_INCLUDES += \
    $(TARGET_OUT_HEADERS)/common/inc
endif

ifneq ($(filter msm8996 msm8994 msm8992,$(TARGET_BOARD_PLATFORM)),)
    LOCAL_CFLAGS += -DPROFILE_MANAGER
endif

LOCAL_LDLIBS := -llibthermalclient

LOCAL_MODULE := libqti-perfd
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
        ActiveRequestList.cpp \
	ResourceQueues.cpp \
	OptsHandler.cpp \
        RestoreHandler.cpp \
	OptsData.cpp \
	ResourceInfo.cpp \
	Request.cpp \
	Target.cpp \
	TargetInit.cpp \
        MpctlUtils.cpp \
        mp-ctl.cpp

LOCAL_SHARED_LIBRARIES := libcutils libthermalclient libutils

LOCAL_C_INCLUDES += \
        $(DEVICE_COMMON_PATH)/power $(DEVICE_COMMON_PATH)/thermal-engine

LOCAL_CFLAGS += \
        -DCLIENT \
        -g0 \
        -Wall \
        -DOEM_PROFILE_RESTORE_ON_FAIL=0 \
        #-DQC_DEBUG=1

ifeq ($(call is-android-codename,JELLY_BEAN),true)
    LOCAL_CFLAGS += -DANDROID_JELLYBEAN=1
    LOCAL_C_INCLUDES += \
    $(TARGET_OUT_HEADERS)/common/inc
endif

LOCAL_MODULE := libqti-perfd-client
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_OWNER := qti
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := mp-ctl-d.c

LOCAL_SHARED_LIBRARIES := libcutils libthermalclient libutils

LOCAL_CFLAGS += \
        -DPERFD \

LOCAL_MODULE := perfd

LOCAL_MODULE_TAGS := optional

LOCAL_STATIC_LIBRARIES += libqti-perfd

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := client.c

LOCAL_MODULE := libqti-perfd-client

LOCAL_MODULE_TAGS := optional

LOCAL_STATIC_LIBRARIES += libqti-perfd-client liblog

LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
