LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES += \
        vendor/qcom/proprietary/android-perf

LOCAL_SRC_FILES := \
        UserspaceCtlServiceMain.cpp \
        FPSTracker.cpp \
        UserspaceCtl.cpp \
        UserspaceCtlService.cpp \
        PerflockHandler.cpp

LOCAL_SHARED_LIBRARIES := \
        libutils \
        libcutils \
        libqti-userspacectl \
        libbinder

LOCAL_CFLAGS += \
        -Wall \
        -DHAVE_PTHREADS \
        -DQC_DEBUG=0

LOCAL_MODULE := perffeedbackd
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)


#Adding header in out folder
include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO   := perffeedback
LOCAL_COPY_HEADERS      := IUserspaceCtl.h

include $(BUILD_COPY_HEADERS)


include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
        UserspaceCtlClient.cpp

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        libutils \
        libdl \
        libbinder

LOCAL_CFLAGS += \
        -Wall \
        -DHAVE_PTHREADS \
        -DQC_DEBUG=0

LOCAL_MODULE := libqti-userspacectl
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := qti
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
