#############################################################
#
# VoicePrintService
#
#############################################################
LOCAL_PATH:= $(call my-dir)

# Set the following flag to false for production releases
# or remove the flag.
QVOP_DEBUG = true

include $(CLEAR_VARS)

LOCAL_JAVA_LIBRARIES := framework
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(call all-java-files-under, src/com/qualcomm/qti/biometrics/voiceprint/service)
LOCAL_SRC_FILES += \
    src/com/qualcomm/qti/biometrics/voiceprint/service/IVoicePrintService.aidl \
    src/com/qualcomm/qti/biometrics/voiceprint/service/IVoicePrintServiceCallback.aidl \
#
# Add debug files here
#
ifeq ($(QVOP_DEBUG), true)
#  LOCAL_SRC_FILES += $(call all-java-files-under, src/com/qualcomm/qti/biometrics/voiceprint/servicedebug)
 endif

 LOCAL_PACKAGE_NAME:= VoicePrintService
 LOCAL_SDK_VERSION := current
 LOCAL_CERTIFICATE := platform
 LOCAL_MODULE_OWNER := qti
 LOCAL_PROGUARD_ENABLED := disabled

 include $(BUILD_PACKAGE)
