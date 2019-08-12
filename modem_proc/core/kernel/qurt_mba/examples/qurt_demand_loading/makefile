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

ifndef TARGET
TARGET := modemSWv4
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

ifndef BUILD_DIR
BUILD_DIR := $(cur_dir)/build
endif
BUILD_DIR_REPLACE := $(subst /,\/, $(BUILD_DIR))

override Q6_RTOS_ROOT := "$(Q6_RTOS_ROOT)/$(TARGET)"

override Q6VERSION := $(shell cat $(Q6_RTOS_ROOT)/scripts/Input/build_params.txt |sed -n 's/Q6VERSION=\(.*\)/\1/p')

#Set up PATH for access DLLs
ifeq (cygwin, $(OSTYPE))
PATH := $(shell cygpath -u "$(Q6_TOOLS_ROOT)/qc/lib/iss"):$(shell cygpath -u "$(Q6_TOOLS_ROOT)/qc/lib/iss/$(Q6VERSION)"):$(PATH)
endif

RTOS_LIBS_ROOT := $(Q6_RTOS_ROOT)/lib

LIB_DINKUMWARE_ROOT := "$(Q6_TOOLS_ROOT)"/dinkumware/lib/$(Q6VERSION)
LIB_DSP_ROOT := "$(Q6_TOOLS_ROOT)"/qc/lib/$(Q6VERSION)

CC := "$(Q6_TOOLS_ROOT)"/gnu/bin/qdsp6-gcc
LD := "$(Q6_TOOLS_ROOT)"/gnu/bin/qdsp6-ld
SIM := "$(Q6_TOOLS_ROOT)"/qc/bin/qdsp6-sim
OBJDUMP := "$(Q6_TOOLS_ROOT)"/gnu/bin/qdsp6-objdump

########## CYGWIN BEGIN ################
QURT_MODEL_CYGWIN := $(Q6_RTOS_ROOT)/debugger/cygwin/qurt_model.dll
ifeq (v4, $(Q6VERSION))
#Cygwin & v4
Q6SS_TIMER_CFG_CYGWIN := "L2timer.dll 32768 0xa0080000 3 0xa0090000"
Q6SS_INT_CFG_CYGWIN := "l2vic.dll 32 0xa0090000"
else
#Cygwin & v5
Q6SS_TIMER_CFG_CYGWIN := "L2timer.dll 19200000 0xab020000 3 0xab010000"
Q6SS_INT_CFG_CYGWIN := "l2vic.dll 32 0xa0020000"
endif
########## CYGWIN END ##################

########## Linux BEGIN #################
QURT_MODEL_LINUX := $(Q6_RTOS_ROOT)/debugger/lnx64/qurt_model.so
ifeq (v4, $(Q6VERSION))
#Linux & v4
Q6SS_TIMER_CFG_LINUX := "$(Q6_TOOLS_ROOT)/qc/lib/iss/L2timer.so 32768 0xa0080000 3 0xa0090000"
Q6SS_INT_CFG_LINUX := "$(Q6_TOOLS_ROOT)/qc/lib/iss/l2vic.so 32 0xa0090000"
else
#Linux & v5
Q6SS_TIMER_CFG_LINUX := "$(Q6_TOOLS_ROOT)/qc/lib/iss/L2timer.so 19200000 0xab080000 3 0xab090000"
Q6SS_INT_CFG_LINUX := "$(Q6_TOOLS_ROOT)/qc/lib/iss/l2vic.so 32 0xa0090000"
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

OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(wildcard *.c))

CFLAGS := -m${Q6VERSION} -g -O2

INCLUDES :=-I. -Iinclude -I$(Q6_RTOS_ROOT)/include -I$(Q6_RTOS_ROOT)/include/qube

LDFLAGS_PREFIX := -m$(Q6VERSION) -T$(BUILD_DIR)/a.linker -g \
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
                        $(RTOS_LIBS_ROOT)/libposix.a \
                        $(LIB_DSP_ROOT)/libqdsp.a \
                        $(BUILD_DIR)/cust_config.o \

LDFLAGS := $(LIB_DINKUMWARE_ROOT)/fini.o --end-group

all: build run

#v4
ifeq (v4, $(Q6VERSION))
CONFIG_XML := "qdsp6_demand.xml"
else
#v5
CONFIG_XML := "qdsp6_demand_v5.xml"
endif

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

build: mkdirs $(OBJS)
	sed 's/build_dir_to_replace/${BUILD_DIR_REPLACE}/g' a.linker > $(BUILD_DIR)/a.linker
	python $(Q6_RTOS_ROOT)/scripts/qurt_config.py update -T "$(Q6_TOOLS_ROOT)" -o $(BUILD_DIR)/cust_config.o $(CONFIG_XML) 
	$(AR) cru $(BUILD_DIR)/app.lib $(OBJS)
	$(LD) $(LDFLAGS_PREFIX) $(BUILD_DIR)/app.lib $(LDFLAGS) -Map $(BUILD_DIR)/bootimg.map -o $(BUILD_DIR)/bootimg.pbn
	$(OBJDUMP) -dClx $(BUILD_DIR)/bootimg.pbn > $(BUILD_DIR)/bootimg.dump

run:
	@echo $(QURT_MODEL) > $(BUILD_DIR)/osam.cfg
	@echo $(Q6SS_TIMER_CFG) > $(BUILD_DIR)/q6ss.cfg
	@echo $(Q6SS_INT_CFG) >> $(BUILD_DIR)/q6ss.cfg
ifeq (v4, $(Q6VERSION))
	-cp t32/t32sim.cfg $(BUILD_DIR)/
	cat t32/t32sim.cmm | sed -e 's;HexagonV4C;$(T32_SYS_CPU);' -e 's;v4c;$(T32_MCD_ARCH);' >> $(BUILD_DIR)/t32sim.cmm
else
	-cp t32/t32sim.cfg $(BUILD_DIR)/
	cat t32/t32sim.cmm | sed -e 's;HexagonV5L;$(T32_SYS_CPU);' -e 's;v5l;$(T32_MCD_ARCH);' >> $(BUILD_DIR)/t32sim.cmm
endif
	@echo $(Q6SS_TIMER_CFG_CYGWIN) > $(BUILD_DIR)/t32sim_sirc.cfg
	@echo $(Q6SS_INT_CFG_CYGWIN) >> $(BUILD_DIR)/t32sim_sirc.cfg
	-cp $(Q6_RTOS_ROOT)/debugger/T32/*_model.* $(BUILD_DIR)
	$(SIM) -S $(BUILD_DIR)/stats.txt --simulated_returnval $(BUILD_DIR)/bootimg.pbn --rtos $(BUILD_DIR)/osam.cfg --symfile $(BUILD_DIR)/bootimg.pbn --cosim_file $(BUILD_DIR)/q6ss.cfg

mkdirs:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: build
