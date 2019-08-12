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

# Comment line below to avoid linking with dynamic linker
export LIBDL_TEST := yes
export NO_RELOC := yes

ifndef TARGET
TARGET := MMv4
endif

ifndef Q6VERSION
Q6VERSION := v4
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
Q6_TOOLS_ROOT :=  /prj/dsp/qdsp6/austin/builds/hexbuild/allplat/hexframe/20390/Linux64/install/BT_201112092059_lnx64/
endif
export Q6_TOOLS_ROOT
endif
endif

cur_dir := $(shell echo "$(CURDIR)" | sed 's/^\/cygdrive\/\([a-z]\)\/\(.*\)/\1:\/\2/')

ifdef LIBDL_TEST
LINKER_SCRIPT_FLAGS := -T ./ld.sc
WHOLE_AR_FLAG := --whole-archive
NO_WHOLE_AR_FLAG := --no-whole-archive
SO_FLAGS:= -G0 -m$(Q6VERSION) -fpic -Wall -shared -nostartfiles -nostdlib -shared -g $(INCLUDES)
#LDFLAGS := $(WHOLE_AR_FLAG) $(Q6_TOOLS_ROOT)/qc/lib/$(Q6VERSION)/G0/libdl.a \
#		   	--end-group -Bdynamic --dynamic-linker= -E ./lib/dl.so --build-id=md5
LDFLAGS := $(WHOLE_AR_FLAG) ./libdl.a \
		   	--end-group -Bdynamic --dynamic-linker= -E ./lib/dl.so --build-id=md5

SO_SRC_DIR := .
SO_TARG_DIR:= .
SO_SRCS_MOD1 := $(SO_SRC_DIR)/module_1.c
SO_SRCS_MOD2 := $(SO_SRC_DIR)/module_2.c
SO_SRCS_MOD3 := $(SO_SRC_DIR)/module_3.c

MOD1_SO := module_1.so
MOD2_SO := module_2.so
MOD3_SO := module_3.so

else
WHOLE_AR_FLAG :=
NO_WHOLE_AR_FLAG :=
LDFLAGS :=  --end-group
endif

ifndef BUILD_DIR
BUILD_DIR := $(cur_dir)/build
endif

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
Q6SS_TIMER_CFG_CYGWIN := "L2timer.dll 32768 0xab000000 3 0xab010000"
Q6SS_INT_CFG_CYGWIN := "l2vic.dll 32 0xab010000"
else
#Cygwin & v5
Q6SS_TIMER_CFG_CYGWIN := "L2timer.dll 19200000 0xab020000 3 0xab010000"
Q6SS_INT_CFG_CYGWIN := "l2vic.dll 32 0xab010000"
endif
########## CYGWIN END ##################

########## Linux BEGIN #################
QURT_MODEL_LINUX := $(Q6_RTOS_ROOT)/debugger/lnx64/qurt_model.so
ifeq (v4, $(Q6VERSION))
#Linux & v4
Q6SS_TIMER_CFG_LINUX := "$(Q6_TOOLS_ROOT)/qc/lib/iss/L2timer.so 32768 0xab000000 3 0xab010000"
Q6SS_INT_CFG_LINUX := "$(Q6_TOOLS_ROOT)/qc/lib/iss/l2vic.so 32 0xab010000"
else
#Linux & v5
Q6SS_TIMER_CFG_LINUX := "$(Q6_TOOLS_ROOT)/qc/lib/iss/qtimer.so --csr_base=0xab000000 --irq_p=3 --freq=19200000 --cnttid=1"
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

INCLUDES :=-I. -I$(Q6_RTOS_ROOT)/include 

LDFLAGS_PREFIX := -m$(Q6VERSION) -g $(LINKER_SCRIPT_FLAGS) \
			-nostdlib --section-start .start=0x1e000000\
			--start-group \
                        $(LIB_DINKUMWARE_ROOT)/init.o \
                        $(RTOS_LIBS_ROOT)/crt0.o \
       $(WHOLE_AR_FLAG) $(LIB_DINKUMWARE_ROOT)/libc.a \
       $(WHOLE_AR_FLAG) $(LIB_DINKUMWARE_ROOT)/libqcc.a \
       $(NO_WHOLE_AR_FLAG) $(RTOS_LIBS_ROOT)/libqurt.a \
       $(WHOLE_AR_FLAG) $(RTOS_LIBS_ROOT)/libqurtcfs.a \
       $(WHOLE_AR_FLAG) $(RTOS_LIBS_ROOT)/libqurtkernel.a \
       $(WHOLE_AR_FLAG) $(RTOS_LIBS_ROOT)/libqube_compat.a \
       $(WHOLE_AR_FLAG) $(RTOS_LIBS_ROOT)/libtimer.a \
       $(WHOLE_AR_FLAG) $(RTOS_LIBS_ROOT)/libposix.a \
       $(WHOLE_AR_FLAG) $(LIB_DSP_ROOT)/libqdsp.a \
                        $(BUILD_DIR)/cust_config.o

LDFLAGS :=  $(LIB_DINKUMWARE_ROOT)/fini.o $(LDFLAGS)

all: so build run

build:
	mkdir -p $(BUILD_DIR) 
	python $(Q6_RTOS_ROOT)/scripts/qurt_config.py update -T "$(Q6_TOOLS_ROOT)" -o $(BUILD_DIR)/cust_config.o qdsp6_qurt.xml
	$(CC) -c $(CFLAGS) main.c $(INCLUDES) -o $(BUILD_DIR)/app1.o
	$(LD) $(LDFLAGS_PREFIX) $(BUILD_DIR)/app1.o $(LDFLAGS) -Map $(BUILD_DIR)/bootimg_org.map -o $(BUILD_DIR)/bootimg.pbn
ifndef NO_RELOC
	cp $(BUILD_DIR)/bootimg.pbn $(BUILD_DIR)/bootimg_org.pbn
	./qdsp6-relocate $(BUILD_DIR)/bootimg.pbn +0x10000000
endif
	$(OBJDUMP) -dClx $(BUILD_DIR)/bootimg.pbn > $(BUILD_DIR)/bootimg.dump

run:
	@echo $(QURT_MODEL) > $(BUILD_DIR)/osam.cfg
	@echo $(Q6SS_TIMER_CFG) > $(BUILD_DIR)/q6ss.cfg
	@echo $(Q6SS_INT_CFG) >> $(BUILD_DIR)/q6ss.cfg
	-cp t32/t32sim.cfg $(BUILD_DIR)/
ifeq (v4, $(Q6VERSION))
	cat t32/t32sim.cmm | sed -e 's;HexagonV4C;$(T32_SYS_CPU);' -e 's;v4c;$(T32_MCD_ARCH);' >> $(BUILD_DIR)/t32sim.cmm
else
	cat t32/t32sim.cmm | sed -e 's;HexagonV5L;$(T32_SYS_CPU);' -e 's;v5l;$(T32_MCD_ARCH);' >> $(BUILD_DIR)/t32sim.cmm
endif
	@echo $(Q6SS_TIMER_CFG_CYGWIN) > $(BUILD_DIR)/t32sim_sirc.cfg
	@echo $(Q6SS_INT_CFG_CYGWIN) >> $(BUILD_DIR)/t32sim_sirc.cfg
	-cp $(Q6_RTOS_ROOT)/debugger/T32/*_model.* $(BUILD_DIR)
	$(SIM) -S $(BUILD_DIR)/stats.txt --simulated_returnval $(BUILD_DIR)/bootimg.pbn --rtos $(BUILD_DIR)/osam.cfg --symfile $(BUILD_DIR)/bootimg.pbn --cosim_file $(BUILD_DIR)/q6ss.cfg

so: $(SO_TARG_DIR) $(MOD1_SO) $(MOD3_SO) $(MOD2_SO)

$(MOD1_SO): $(SO_SRCS_MOD1)
	$(CC) $(SO_FLAGS) $^ -o $@

$(MOD2_SO): $(SO_SRCS_MOD2)
	$(CC) $(SO_FLAGS) $^ -o $@

$(MOD3_SO): $(SO_SRCS_MOD3)
	$(CC) $(SO_FLAGS) $^ -o $@

clean:
	rm -rf build
	rm -f $(MOD1_SO)
	rm -f $(MOD2_SO)
	rm -f $(MOD3_SO)

.PHONY: build
