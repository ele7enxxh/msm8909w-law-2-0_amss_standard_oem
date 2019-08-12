LOCAL_PATH := $(call my-dir)

ifeq ($(call is-board-platform-in-list,msm8660 copper msm8974 msm8226 apq8084 msm8962 msm8994 msm8996 msm8952 msm8937 msm8953),true)
BUILD_DSPS:=true
endif

ifeq ($(TARGET_USES_SSC),true)
  BUILD_DSPS:=true
endif

include $(CLEAR_VARS)
LOCAL_COPY_HEADERS_TO := sensors/inc
LOCAL_COPY_HEADERS := sensor_interface.h

commonSources  :=
commonIncludes := $(TARGET_OUT_HEADERS)/common/inc
commonIncludes += $(TARGET_OUT_HEADERS)/qmi/inc
commonIncludes += $(TARGET_OUT_HEADERS)/sensors/inc

LOCAL_C_INCLUDES := $(commonIncludes)

LOCAL_C_INCLUDES += hardware/libhardware/include/hardware
LOCAL_C_INCLUDES += external/libxml2/include\
                external/icu/icu4c/source/common
LOCAL_MODULE := sensor_calibrate
ifeq ($(BUILD_DSPS),true)
LOCAL_CFLAGS += -DDSPS
LOCAL_SRC_FILES := \
                sensor_calibrate.cpp\
                SSC_Calibrate.cpp
else
LOCAL_SRC_FILES := \
                sensor_calibrate.cpp\
                sensors_XML.cpp
endif
LOCAL_SHARED_LIBRARIES := liblog libcutils libdl libxml2 libutils

ifeq ($(BUILD_DSPS),true)
LOCAL_SHARED_LIBRARIES += libsensor1
endif

LOCAL_MODULE_TAGS := optional

ifdef TARGET_2ND_ARCH
LOCAL_MODULE_PATH_32 := $(TARGET_OUT_VENDOR)/lib
LOCAL_MODULE_PATH_64 := $(TARGET_OUT_VENDOR)/lib64
else
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)
endif

include $(BUILD_SHARED_LIBRARY)
