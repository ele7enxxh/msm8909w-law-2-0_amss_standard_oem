##################################################################
# Copyright (c) $Date: 2009/08/04 21:39:57 $ QUALCOMM INCORPORATED.
# All Rights Reserved.
# Modified by QUALCOMM INCORPORATED on $Date: 2009/08/04 21:39:57 $
##################################################################

SHELL = /bin/bash

empty :=

# Make sure Environment variables are set up
ifndef Q6_RTOS_SOURCE
$(error Q6_RTOS_SOURCE not defined, please set Q6_RTOS_SOURCE to QURT source directory)
endif

ifeq ($(Q6VERSION),$(empty))
Q6VERSION := v5
endif

include t32/t32_arch.mk

#Find OS Environment
ifeq (CYGWIN, $(findstring CYGWIN,$(shell uname)))
OSTYPE := cygwin
else
ifeq (Linux, $(findstring Linux,$(shell uname)))
ifeq (x86_64, $(shell arch))
OSTYPE := lnx64
else
$(error Supported OS environments - cygwin, lnx64)
endif
endif
endif

ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifeq (cygwin, $(OSTYPE))
$(error Q6_TOOLS_ROOT not defined)
else
ifneq ($(empty),$(findstring $(TARGET),"modemv5 MMv5 simv5"))
Q6_TOOLS_ROOT := /pkg/qct/software/hexagon/releases/tools/5.0.01
else
Q6_TOOLS_ROOT := /pkg/qct/software/hexagon/releases/tools/3.1.04
endif
export Q6_TOOLS_ROOT
endif
endif

cur_dir := $(shell echo "$(CURDIR)" | sed 's/^\/cygdrive\/\([a-z]\)\/\(.*\)/\1:\/\2/')

#Set up PATH for access DLLs
ifeq (cygwin, $(OSTYPE))
PATH := $(shell cygpath -u "$(Q6_TOOLS_ROOT)/qc/lib/iss"):$(shell cygpath -u "$(Q6_TOOLS_ROOT)/qc/lib/iss/$(Q6VERSION)"):$(PATH)
endif

ifndef BUILD_DIR
BUILD_DIR := $(cur_dir)/build
endif

override TARGET := fw$(Q6VERSION)
override Q6_RTOS_ROOT := $(BUILD_DIR)/qurt_install
Q6_RTOS_BUILD := $(BUILD_DIR)/qurt_build

RTOS_LIBS_ROOT := $(Q6_RTOS_ROOT)/$(TARGET)/lib

LIB_DINKUMWARE_ROOT := "$(Q6_TOOLS_ROOT)"/dinkumware/lib/$(Q6VERSION)
LIB_DSP_ROOT := "$(Q6_TOOLS_ROOT)"/qc/lib/$(Q6VERSION)

ifeq (y, $(G0))
LIB_DINKUMWARE_ROOT := $(LIB_DINKUMWARE_ROOT)/G0
endif

CC := "$(Q6_TOOLS_ROOT)"/gnu/bin/qdsp6-gcc
LD := "$(Q6_TOOLS_ROOT)"/gnu/bin/qdsp6-ld
SIM := "$(Q6_TOOLS_ROOT)"/qc/bin/qdsp6-sim
OBJDUMP := "$(Q6_TOOLS_ROOT)"/gnu/bin/qdsp6-objdump

########## CYGWIN BEGIN ################
QURT_MODEL_CYGWIN := $(Q6_RTOS_ROOT)/$(TARGET)/debugger/cygwin/qurt_model.dll
ifeq (v4, $(Q6VERSION))
#Cygwin & v4
Q6SS_TIMER_CFG_CYGWIN := "L2timer.dll 32768 0xab000000 3 0xab010000"
Q6SS_INT_CFG_CYGWIN := "l2vic.dll 32 0xab010000"
else
#Cygwin & 5
Q6SS_TIMER_CFG_CYGWIN := "L2timer.dll 19200000 0xab020000 3 0xab010000"
Q6SS_INT_CFG_CYGWIN := "l2vic.dll 32 0xab010000"
endif
########## CYGWIN END ##################

########## Linux BEGIN #################
QURT_MODEL_LINUX := $(Q6_RTOS_ROOT)/$(TARGET)/debugger/lnx64/qurt_model.so
ifeq (v4, $(Q6VERSION))
#Linux & v4
Q6SS_TIMER_CFG_LINUX := "$(Q6_TOOLS_ROOT)/qc/lib/iss/L2timer.so 32768 0xab000000 3 0xab010000"
Q6SS_INT_CFG_LINUX := "$(Q6_TOOLS_ROOT)/qc/lib/iss/l2vic.so 32 0xab010000"
else
#Linux &v5
Q6SS_TIMER_CFG_LINUX := "$(Q6_TOOLS_ROOT)/qc/lib/iss/L2timer.so 19200000 0xab020000 3 0xab010000"
Q6SS_INT_CFG_LINUX := "$(Q6_TOOLS_ROOT)/qc/lib/iss/l2vic.so 32 0xab010000"
endif
########## Linux END ###################

ifeq (cygwin, $(OSTYPE))
QURT_MODEL := $(QURT_MODEL_CYGWIN)
Q6SS_TIMER_CFG := $(Q6SS_TIMER_CFG_CYGWIN)
Q6SS_INT_CFG := $(Q6SS_INT_CFG_CYGWIN)
else
QURT_MODEL := $(QURT_MODEL_LINUX)
Q6SS_TIMER_CFG := $(Q6SS_TIMER_CFG_LINUX)
Q6SS_INT_CFG := $(Q6SS_INT_CFG_LINUX)
endif

CFLAGS := -m${Q6VERSION} -g -O2
ifeq (y, $(G0))
CFLAGS += -G0
endif

ifeq (v4, $(Q6VERSION))
LINKER_SCRIPT := a.linker.v4
else  # for v5 need new linker ???
LINKER_SCRIPT := a.linker
endif

INCLUDES :=-I. -I$(Q6_RTOS_ROOT)/$(TARGET)/include 

LDFLAGS_PREFIX := -m$(Q6VERSION) -T$(LINKER_SCRIPT) $(CFLAGS) -g \
			-nostdlib --section-start .start=0x1e000000\
			--start-group \
                        $(LIB_DINKUMWARE_ROOT)/init.o \
                        $(RTOS_LIBS_ROOT)/crt0.o \
                        $(LIB_DINKUMWARE_ROOT)/libc.a \
                        $(LIB_DINKUMWARE_ROOT)/libqcc.a \
                        $(RTOS_LIBS_ROOT)/libqurt.a \
                        $(RTOS_LIBS_ROOT)/libqurtcfs.a \
                        $(RTOS_LIBS_ROOT)/libqurtkernel.a \
                        $(RTOS_LIBS_ROOT)/libqube_compat.a \
                        $(RTOS_LIBS_ROOT)/libtimer.a \
                        $(LIB_DSP_ROOT)/libqdsp.a \
                        $(BUILD_DIR)/cust_config.o \

LDFLAGS := $(LIB_DINKUMWARE_ROOT)/fini.o --end-group

# QURT build flags
BUILD_CONFIG_FLAGS := $(shell python $(Q6_RTOS_SOURCE)/kernel/scripts/build_flags.py config)

all: build run

build: qurt_build app_build

ifeq (v4, $(Q6VERSION))
#v4
CONFIG_XML := "qdsp6_fw.xml"
else
#v5
CONFIG_XML := "qdsp6_fw_v5.xml"
endif

qurt_build:
	mkdir -p $(BUILD_DIR)
	cd "$(Q6_RTOS_SOURCE)"; make TARGET=$(TARGET) INSTALL_DIR=$(Q6_RTOS_ROOT) BUILD_DIR=$(Q6_RTOS_BUILD) Q6_TOOLS_ROOT="$(Q6_TOOLS_ROOT)" Q6VERSION=$(Q6VERSION) CONFIG_DIR=$(cur_dir)/config BUILD_CONFIG_FLAGS="$(BUILD_CONFIG_FLAGS)"

app_build:
	python $(Q6_RTOS_ROOT)/$(TARGET)/scripts/qurt_config.py update -T "$(Q6_TOOLS_ROOT)" -o $(BUILD_DIR)/cust_config.o $(CONFIG_XML)
	$(CC) -c $(CFLAGS) main.c $(INCLUDES) -o $(BUILD_DIR)/app1.o
	$(LD) $(LDFLAGS_PREFIX) $(BUILD_DIR)/app1.o $(LDFLAGS) -Map $(BUILD_DIR)/bootimg.map -o $(BUILD_DIR)/bootimg.pbn
	$(OBJDUMP) -dClx $(BUILD_DIR)/bootimg.pbn > $(BUILD_DIR)/bootimg.dump

run:
	-rm -f $(BUILD_DIR)/osam.cfg
	@echo $(QURT_MODEL) > $(BUILD_DIR)/osam.cfg
	@echo $(Q6SS_INT_CFG) >> $(BUILD_DIR)/q6ss.cfg
	-cp t32/t32sim.cfg $(BUILD_DIR)/
	@echo $(Q6SS_TIMER_CFG_CYGWIN) > $(BUILD_DIR)/t32sim_sirc.cfg
	@echo $(Q6SS_INT_CFG_CYGWIN) >> $(BUILD_DIR)/t32sim_sirc.cfg
	-cp $(Q6_RTOS_ROOT)/$(TARGET)/debugger/T32/*_model.* $(BUILD_DIR)

	$(SIM) -S $(BUILD_DIR)/stats.txt --simulated_returnval --tcm:lowaddr 0xd8000000 $(BUILD_DIR)/bootimg.pbn --rtos $(BUILD_DIR)/osam.cfg --symfile $(BUILD_DIR)/bootimg.pbn

clean:
	rm -rf $(BUILD_DIR)

.PHONY: build
