##################################################################
# Copyright (c) $Date: 2009/08/04 21:39:57 $ QUALCOMM INCORPORATED.
# All Rights Reserved.
# Modified by QUALCOMM INCORPORATED on $Date: 2009/08/04 21:39:57 $
##################################################################

SHELL = /bin/bash

empty :=

# Make sure Environment variables are set up
ifndef Q6_RTOS_ROOT
$(error Q6_RTOS_ROOT not defined, please set to QURT install directory)
endif

ifneq ($(TARGET), mpqurt) 
$(warning TARGET not defined or not equal to mpqurt, please rebuild RTOS with TARGET=mpqurt)
endif

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
Q6_TOOLS_ROOT := /pkg/qct/software/hexagon/releases/tools/5.0.01
export Q6_TOOLS_ROOT
endif
endif

cur_dir := $(shell echo "$(CURDIR)" | sed 's/^\/cygdrive\/\([a-z]\)\/\(.*\)/\1:\/\2/')

SIM := "$(Q6_TOOLS_ROOT)"/qc/bin/qdsp6-sim

ifndef BUILD_DIR
BUILD_DIR := $(cur_dir)/build
endif

TEMP_Q6_RTOS_ROOT := "$(Q6_RTOS_ROOT)/$(TARGET)"

override Q6VERSION := $(shell cat $(TEMP_Q6_RTOS_ROOT)/scripts/Input/build_params.txt |sed -n 's/Q6VERSION=\(.*\)/\1/p')

export BUILD_DIR
export TARGET
export Q6_RTOS_ROOT
export Q6VERSION


#Set up PATH for access DLLs
ifeq (cygwin, $(OSTYPE))
PATH := $(shell cygpath -u "$(Q6_TOOLS_ROOT)/qc/lib/iss/$(Q6VERSION)"):$(PATH)
endif


ifeq (cygwin, $(OSTYPE))
Q6SS_TIMER_CFG := "$(Q6_TOOLS_ROOT)\qc\lib\iss\L2timer.dll 19200000 0xab020000 3 0xab010000"
QURT_MODEL := $(TEMP_Q6_RTOS_ROOT)/debugger/cygwin/qurt_model.dll
Q6SS_INT_CFG := "$(Q6_TOOLS_ROOT)\qc\lib\iss\l2vic.dll 32 0xab010000"
########## CYGWIN END ##################
else
########## Linux BEGIN #################
Q6SS_TIMER_CFG := "$(Q6_TOOLS_ROOT)/qc/lib/iss/qtimer.so --csr_base=0xab000000 --irq_p=3 --freq=19200000 --cnttid=1"
QURT_MODEL := $(TEMP_Q6_RTOS_ROOT)/debugger/lnx64/qurt_model.so
Q6SS_INT_CFG := "$(Q6_TOOLS_ROOT)/qc/lib/iss/l2vic.so 32 0xab010000"
########## Linux END ###################
endif

all: build run

build:
	mkdir -p $(BUILD_DIR) 
	make -C qurt
	make -C app1
	make -C app2
	[ -x $(TEMP_Q6_RTOS_ROOT)/scripts/qdsp6-image-build ] || chmod +x $(TEMP_Q6_RTOS_ROOT)/scripts/qdsp6-image-build
	python $(TEMP_Q6_RTOS_ROOT)/scripts/qurt-image-build.py $(BUILD_DIR)/qurt.elf $(BUILD_DIR)/app1.elf $(BUILD_DIR)/app2.elf -o $(BUILD_DIR)/bootimg.pbn -p 0x1e000000

run:
	@echo $(QURT_MODEL) > $(BUILD_DIR)/osam.cfg
	@echo $(Q6SS_TIMER_CFG) > $(BUILD_DIR)/q6ss.cfg
	@echo $(Q6SS_INT_CFG) >> $(BUILD_DIR)/q6ss.cfg
	$(SIM) -S $(BUILD_DIR)/stats.txt --simulated_returnval $(BUILD_DIR)/bootimg.pbn --rtos $(BUILD_DIR)/osam.cfg --symfile $(BUILD_DIR)/qurt.elf --cosim_file $(BUILD_DIR)/q6ss.cfg

clean:
	rm -rf build

.PHONY: build
