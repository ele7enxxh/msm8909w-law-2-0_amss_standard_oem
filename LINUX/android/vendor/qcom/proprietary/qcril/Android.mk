_TARGET_PD_SERVICE_ENABLED := ${TARGET_PD_SERVICE_ENABLED}
TARGET_PD_SERVICE_ENABLED :=
QCRIL_BOARD_PLATFORM_LIST := msm7627_surf
QCRIL_BOARD_PLATFORM_LIST += msm7627a
QCRIL_BOARD_PLATFORM_LIST += msm7630_surf
QCRIL_BOARD_PLATFORM_LIST += msm7630_fusion
QCRIL_BOARD_PLATFORM_LIST += msm8660
QCRIL_BOARD_PLATFORM_LIST += msm8960
QCRIL_BOARD_PLATFORM_LIST += msm8974
QCRIL_BOARD_PLATFORM_LIST += msm8226
QCRIL_BOARD_PLATFORM_LIST += msm8610
QCRIL_BOARD_PLATFORM_LIST += apq8084
QCRIL_BOARD_PLATFORM_LIST += msm8916
QCRIL_BOARD_PLATFORM_LIST += msm8994
QCRIL_BOARD_PLATFORM_LIST += msm8992
QCRIL_BOARD_PLATFORM_LIST += msm8909
QCRIL_BOARD_PLATFORM_LIST += msm8952
QCRIL_BOARD_PLATFORM_LIST += msm8996
QCRIL_BOARD_PLATFORM_LIST += msm8937
QCRIL_BOARD_PLATFORM_LIST += msm8953
QCRIL_BOARD_PLATFORM_LIST += msmcobalt

QCRIL_DIR := $(call my-dir)

ifeq ($(call is-board-platform-in-list,msm7627_6x $(QCRIL_BOARD_PLATFORM_LIST)),true)
  include $(QCRIL_DIR)/qcrilhook_oem/Android.mk
endif

include $(QCRIL_DIR)/qcril_database/Android.mk

ifeq ($(call is-board-platform-in-list,$(QCRIL_BOARD_PLATFORM_LIST)),true)

  ifdef DSDA_BUILD_SECOND_RIL
    ## Compile a 2nd library for some DSDA targets
    QCRIL_DSDA_INSTANCE=2
    include $(QCRIL_DIR)/qcril_qmi/qcril_qmi.mk

    QCRIL_DSDA_INSTANCE=1
  endif

  include $(QCRIL_DIR)/qcril_qmi/qcril_qmi.mk

endif

include $(QCRIL_DIR)/settingsd/Android.mk

include $(QCRIL_DIR)/common/uim/remote_simlock/Android.mk

TARGET_PD_SERVICE_ENABLED := ${_TARGET_PD_SERVICE_ENABLED}

include $(QCRIL_DIR)/common/uim/rsu/Android.mk

