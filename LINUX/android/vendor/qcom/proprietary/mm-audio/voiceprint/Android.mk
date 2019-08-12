ifeq ($(call is-vendor-board-platform,QCOM),true)
ifeq ($(call is-board-platform-in-list,msm8994 msm8996),true)
MY_PATH := $(call my-dir)

include $(MY_PATH)/qvop-service/Android.mk
include $(MY_PATH)/VoicePrintService/Android.mk
include $(MY_PATH)/VoicePrintSDK/Android.mk
include $(MY_PATH)/VoicePrintTest/Android.mk
include $(MY_PATH)/apps/Android.mk

endif
endif

