# -*- Mode: text -*-
#==============================================================================
# FILE: qmuxd_app.mak
#
# SERVICE: 
#
# DESCRIPTION:
#
# Copyright 2012 Qualcomm Technologies, Inc. All rights reserved.
#==============================================================================

default: all

RELPATHTOROOT=../../../../..

TARGET = qmuxd
MAKEFILETYPE = app
MAKEFILENAME = qmuxd_app.mak

C_SRC =\
   ../platform/linux_qmi_qmux_if_server.c\
   ../platform/qmi_platform_qmux_io.c\
   ../platform/qmi_platform.c\
   ../src/qmi_qmux.c\
   ../src/qmi_util.c\

NEEDINC=\
   $(QCPATH)/qmi/inc\
   $(QCPATH)/qmi/src\
   $(QCPATH)/qmi/platform\
   $(QCPATH)/qmi/core/lib/inc\
   $(QCPATH)/common/inc\
   $(QCPATH)/diag/include\
   system/core/include/cutils\

NEEDLIB =\
   system/core/liblog/liblog\
   system/core/libcutils/libcutils\
   frameworks/native/libs/utils/libutils\
   $(QCPATH)/data/dsutils/src/libdsutils\
   $(QCPATH)/diag/src/libdiag\
   $(QCPATH)/test/ril/strllib/libstrl\

LIBS = -lz

CFLAGS += -m32
CFLAGS += -DFEATURE_RILTEST
CFLAGS += -DFEATURE_DATA_LOG_QXDM
CFLAGS += -include string.h
CFLAGS += -D_XOPEN_SOURCE=500 ## pthreads recursive mutex

CFLAGS += -DFEATURE_QMI_ANDROID

LDFLAGS += -m32

BTBUILD=$(QCPATH)/test/ril/btbuild
include $(RELPATHTOROOT)/$(BTBUILD)/bt.driver.mki

-include ./${MAKEFILENAME}.depend.mki

#======================================================================
# Add Mannual dependencies and makefile overrides here
#======================================================================

#
#ifneq (, $(filter msm7630_fusion, $(QCOM_TARGET_PRODUCT)))
#   CFLAGS += -DFEATURE_QMI_FUSION
#   CFLAGS += -DFEATURE_WAIT_FOR_MODEM_HACK
#endif
#
#ifneq (, $(filter msm8660_surf msm8960, $(QCOM_TARGET_PRODUCT)))
#   CFLAGS += -DFEATURE_WAIT_FOR_MODEM_HACK
#endif

#EOF
