ifeq ($(call is-board-platform-in-list,msm8660 copper msm8974 msm8226 apq8084 msm8962 msm8994 msm8992 msm8996 msm8952 msm8937 msm8953),true)
  BUILD_DSPS:=true
endif

ifeq ($(TARGET_USES_SSC),true)
  BUILD_DSPS:=true
endif

ifeq ($(BUILD_DSPS),true)
  include $(call all-subdir-makefiles)
else
  ifneq ($(call is-board-platform-in-list,msm8960),true)
    LOCAL_PATH := $(call my-dir)
    include $(LOCAL_PATH)/sensor_calibration/Android.mk
  endif
endif
