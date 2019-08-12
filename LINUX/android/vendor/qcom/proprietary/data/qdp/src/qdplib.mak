# -*- Mode: text -*-
#==============================================================================
# FILE: qdplib.mak
#
# SERVICE: 
#
# DESCRIPTION:
#
# Copyright 2012 Qualcomm Technologies, Inc. All rights reserved.
#==============================================================================

default: all

RELPATHTOROOT = ../../../../../..

TARGET = libqdp
MAKEFILETYPE = shared
MAKEFILENAME = qdplib.mak

C_SRC =\
   qdp.c\
   qdp_platform.c

NEEDINC=\
   $(QCPATH)/data/qdp/inc\
   $(QCPATH)/qmi/inc\
   $(QCPATH)/qmi/platform\
   $(QCPATH)/data/dsutils/inc\
   $(QCPATH)/common/inc\
   $(QCPATH)/diag/include\

NEEDLIB =\

CFLAGS += -m32
CFLAGS += -D_GNU_SOURCE # required for sighandler_t
CFLAGS += -DFEATURE_TARGET_GENERIC_x86

LDFLAGS += -m32

BTBUILD=$(QCPATH)/test/ril/btbuild
include $(RELPATHTOROOT)/$(BTBUILD)/bt.driver.mki

-include ./${MAKEFILENAME}.depend.mki

#======================================================================
# Add Mannual dependencies and makefile overrides here
#======================================================================

#EOF
