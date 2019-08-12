# -*- Mode: text -*-
#==============================================================================
# FILE: qmilib.mak
#
# SERVICE: 
#
# DESCRIPTION:
#
# Copyright 2012 Qualcomm Technologies, Inc. All rights reserved.
#==============================================================================

default: all

RELPATHTOROOT = ../../../..

TARGET = libqmi
MAKEFILETYPE = shared
MAKEFILENAME = qmilib.mak

C_SRC =\
   src/qmi.c\
   src/qmi_atcop_srvc.c\
   src/qmi_eap_srvc.c\
   src/qmi_nas_srvc.c\
   src/qmi_qos_srvc.c\
   src/qmi_service.c\
   src/qmi_wds_srvc.c\
   src/qmi_wds_utils.c\
   src/qmi_errors.c\
   src/qmi_client.c ## realy is qcci_legacy, but just put here for now

NEEDINC=\
   $(QCPATH)/qmi/inc\
   $(QCPATH)/qmi/src\
   $(QCPATH)/qmi/platform\
   $(QCPATH)/qmi/proxy\
   $(QCPATH)/qmi/core/lib/inc\
   $(QCPATH)/qmi/services\
   $(QCPATH)/common/inc\
   $(QCPATH)/data/dsutils/inc\
   $(QCPATH)/diag/include\

NEEDLIB =\
   $(QCPATH)/qmi/services/libqmiservices\
   $(QCPATH)/qmi/qmi_client_qmux/libqmi_client_qmux\

CFLAGS += -m32
CFLAGS += -DFEATURE_DATA_LOG_QXDM
CFLAGS += -DFEATURE_RILTEST
CFLAGS += -include string.h

CFLAGS += -DFEATURE_QMI_ANDROID

LDFLAGS += -m32

BTBUILD=$(QCPATH)/test/ril/btbuild
include $(RELPATHTOROOT)/$(BTBUILD)/bt.driver.mki

-include ./${MAKEFILENAME}.depend.mki

#======================================================================
# Add Mannual dependencies and makefile overrides here
#======================================================================

#EOF
