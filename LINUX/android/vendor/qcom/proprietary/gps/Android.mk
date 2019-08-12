ifneq ($(BUILD_TINY_ANDROID),true)
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)

LOCAL_PATH := $(call my-dir)

# This property is used by GPS HAL and Wiper to cooperate with QC NLP
# note: this property must be aligned with persist.loc.nlp_name
# 1: QC Network Location Provider is in use. Note persist.loc.nlp_name must be set.
# 0: otherwise. Note persist.loc.nlp_name must be cleared/commented out.
ADDITIONAL_BUILD_PROPERTIES += persist.gps.qc_nlp_in_use=1

# package name of QC NLP, if so chosen in persist.gps.qc_nlp_in_use
# note: this property must be aligned with persist.gps.qc_nlp_in_use,
#       for LocationManagerService.java is controlled by this property only
# note: the length limit for value is 92 characters
ADDITIONAL_BUILD_PROPERTIES += persist.loc.nlp_name=com.qualcomm.location

# This property will decouple the "WiFi & Mobile Network Location" from AGPS database setting(Settings.Global.ASSISTED_GPS_ENABLED)
# 1: AGPS operation is controlled by Settings.Global.ASSISTED_GPS_ENABLED.
#      Recommended for all OEMs who don't use Google SUPL servers.
# 0: AGPS operation is controlled by "WiFi & Mobile Network Location" setting on Android UI
#      Recommended for everybody who use Google SUPL servers.
ADDITIONAL_BUILD_PROPERTIES += ro.gps.agps_provider=1

# Select which RPC lib to link to
LOC_API_USE_LOCAL_RPC:=0
LOC_API_USE_QCOM_AUTO_RPC:=1

# Target-specific makefile
GPS_BUILD_DIR:=$(LOCAL_PATH)/build
include $(GPS_BUILD_DIR)/target_specific_features.mk

ifneq ($(LOC_UNSUPPORTED_TARGET),true)
ifeq ($(LW_FEATURE_SET),true)
DIR_LIST := $(LOCAL_PATH)/etc/
DIR_LIST += $(LOCAL_PATH)/framework/
DIR_LIST += $(LOCAL_PATH)/framework/native/core/
DIR_LIST += $(LOCAL_PATH)/location_hal_test/
include $(addsuffix Android.mk, $(DIR_LIST))
else ifeq ($(BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET),true)
DIR_LIST := $(LOCAL_PATH)/etc/
DIR_LIST += $(LOCAL_PATH)/framework/native/core/
DIR_LIST += $(LOCAL_PATH)/framework/native/lcp/data-items/
DIR_LIST += $(LOCAL_PATH)/launcher/
DIR_LIST += $(LOCAL_PATH)/location_hal_test/
DIR_LIST += $(LOCAL_PATH)/slim/
include $(addsuffix Android.mk, $(DIR_LIST))
else
#include $(call all-subdir-makefiles)
include $(LOCAL_PATH)/*/Android.mk
endif

endif # LOC_UNSUPPORTED_TARGET

endif # BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
endif # BUILD_TINY_ANDROID
