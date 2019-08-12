# -*- Mode: text -*-
#==============================================================================
# FILE: qdilib.mak
#
# SERVICE: 
#
# DESCRIPTION:
#
# Copyright 2012 Qualcomm Technologies, Inc. All rights reserved.
#==============================================================================

default: all

RELPATHTOROOT=../../../../../..

TARGET = libqdi
MAKEFILETYPE = shared
MAKEFILENAME = qdilib.mak

C_SRC =\
   qdi.c\
   qdi_netlink.c\

NEEDINC=\
   $(QCPATH)/data/qdi/inc\
   $(QCPATH)/diag/include\
   $(QCPATH)/qmi/inc\
   $(QCPATH)/qmi/platform\
   $(QCPATH)/data/dsutils/inc\
   $(QCPATH)/data/dsi_netctrl/inc\
   $(QCPATH)/common/inc\

NEEDLIB =\
   $(QCPATH)/qmi/libqmi\
   $(QCPATH)/diag/src/libdiag\
   $(QCPATH)/data/dsutils/src/libdsutils\

CFLAGS += -m32
CFLAGS += -DDSI_NETCTRL_OFFTARGET
CFLAGS += -DFEATURE_DSI_TEST
CFLAGS += -Dstrlcpy=strncpy -Dstrlcat=strncat
CFLAGS += -DQDI_OFFTARGET
CFLAGS += -DFEATURE_DATA_LOG_QXDM

LDFLAGS += -m32

BTBUILD=$(QCPATH)/test/ril/btbuild
include $(RELPATHTOROOT)/$(BTBUILD)/bt.driver.mki

-include ./${MAKEFILENAME}.depend.mki

#======================================================================
# Add Mannual dependencies and makefile overrides here
#======================================================================

#EOF
