IMS_DIR := $(call my-dir)
include $(CLEAR_VARS)
ifeq ($(call is-board-platform-in-list,msm8953 msm8960 msm8974 msm8226 msm8994 apq8084 msm8916 msm8909 msm8952 msm8952_32 msm8952_64 msmcobalt msm8996 msm8992 msm8937 titanium msmfalcon msmskunk),true)
  ifeq ($(TARGET_USES_IMS),true)
    include $(IMS_DIR)/imscamera/Android.mk
    include $(IMS_DIR)/qmi/Android.mk
  else
    include $(IMS_DIR)/qmi/Android.mk
  endif
endif
