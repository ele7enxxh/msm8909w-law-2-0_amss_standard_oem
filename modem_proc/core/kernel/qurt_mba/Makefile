#=============================================================================
#
#                 Makefile -- H E A D E R  F I L E
#
#GENERAL DESCRIPTION
#   makefile
#   
#EXTERNAL FUNCTIONS
#   none
#
#INITIALIZATION AND SEQUENCING REQUIREMENTS
#   None.
#
#      Copyright (c) 2009 - 2011
#                    by QUALCOMM Incorporated.  All Rights Reserved.
#
#=============================================================================*/

#=============================================================================
#
#                        EDIT HISTORY FOR MODULE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
#
#$Header: //source/qcom/qct/core/kernel/qurt/main/latest/Makefile#15 $ 
#$DateTime: 2011/03/14 21:13:03 $ $Author: opham $
#
#when       who     what, where, why
#--------   ---     ------------------------------------------------------------
#03/03/11   op      Add header file
#=============================================================================*/

empty :=
BUILD_CONFIG_FLAGS :=$(empty)
QURT_TOP := $(shell echo "$(CURDIR)" | sed 's/^\/cygdrive\/\([a-z]\)\/\(.*\)/\1:\/\2/')

ifndef BUILD_DIR
BUILD_DIR := $(QURT_TOP)/build
endif

ifndef INSTALL_DIR
INSTALL_DIR := $(QURT_TOP)/install
endif

#Include Target definitions
include target.mk

# If TARGET is specified, build the TARGET only. Otherwise, build all TARGETS
ifndef TARGET
TARGETS := $(RELEASE_TARGETS)
else
TARGETS := $(TARGET)
endif

RELEASE_OBJS := kernel/libqurtkernel.a \
                libs/qurt/libqurt.a  \
                libs/qube/libqube_compat.a \
                libs/posix/libposix.a \
                libs/timer/libtimer.a \
                libs/c/fs/libqurtcfs.a \
                libs/c/sys/crt0.o \
                libs/c/sys/crt1.o

TARGET_KERNEL := $(patsubst %, %_kernel, $(TARGETS))
TARGET_LIBS := $(patsubst %, %_libs, $(TARGETS))
TARGET_INSTALL := $(patsubst %, %_install, $(TARGETS))
TARGET_CLEAN := $(patsubst %, %_clean, $(TARGETS))


all: mkdirs $(TARGETS)

install: clean all

export QURT_TOP
export Q6VERSION
export CONFIG_DIR
export BUILD_CONFIG_FLAGS
export Q6_TOOLS_ROOT

.SECONDEXPANSION:

$(TARGETS): $$@_kernel $$@_libs $$@_install

$(TARGET_KERNEL):
	make -C kernel BUILD_DIR=$(BUILD_DIR)/$(patsubst %_kernel,%,$@)

$(TARGET_LIBS):
	make -C libs BUILD_DIR=$(BUILD_DIR)/$(patsubst %_libs,%,$@)

$(TARGET_INSTALL):
	rm -Rf $(INSTALL_DIR)/$(patsubst %_install,%,$@)
	@echo "### Installing $(patsubst %_install,%,$@)/debugger ###"
	mkdir -p $(INSTALL_DIR)/$(patsubst %_install,%,$@)/debugger
	mkdir -p $(INSTALL_DIR)/$(patsubst %_install,%,$@)/debugger
	cp -a osam/bin/T32/$(Q6VERSION) $(INSTALL_DIR)/$(patsubst %_install,%,$@)/debugger/T32
	cp -a osam/bin/Win/$(Q6VERSION) $(INSTALL_DIR)/$(patsubst %_install,%,$@)/debugger/Win
	chmod +rx $(INSTALL_DIR)/$(patsubst %_install,%,$@)/debugger/Win/*
	cp -a osam/bin/lnx32/$(Q6VERSION) $(INSTALL_DIR)/$(patsubst %_install,%,$@)/debugger/lnx32
	cp -a osam/bin/lnx64/$(Q6VERSION) $(INSTALL_DIR)/$(patsubst %_install,%,$@)/debugger/lnx64
	@echo "### Installing $(patsubst %_install,%,$@)/include ###"
	mkdir -p $(INSTALL_DIR)/$(patsubst %_install,%,$@)/include/arch
	mkdir -p $(INSTALL_DIR)/$(patsubst %_install,%,$@)/include/posix
	mkdir -p $(INSTALL_DIR)/$(patsubst %_install,%,$@)/include/qube

	#cp -a libs/atomic_ops/*.h $(INSTALL_DIR)/$(patsubst %_install,%,$@)/include
	#cp -a libs/atomic_ops/arch/*.h $(INSTALL_DIR)/$(patsubst %_install,%,$@)/include/arch
	cp -a libs/qurt/include/public/*.h $(INSTALL_DIR)/$(patsubst %_install,%,$@)/include
	cp -a libs/posix/include/public/* $(INSTALL_DIR)/$(patsubst %_install,%,$@)/include/posix
	cp -a libs/qube/include/public/*.h $(INSTALL_DIR)/$(patsubst %_install,%,$@)/include/qube
	cp -a libs/timer/include/public/*.h $(INSTALL_DIR)/$(patsubst %_install,%,$@)/include

	@echo "### Installing $(patsubst %_install,%,$@)/scripts ###"
	cp -a kernel/scripts $(INSTALL_DIR)/$(patsubst %_install,%,$@)
	@echo "Q6VERSION=$(Q6VERSION)" >> $(INSTALL_DIR)/$(patsubst %_install,%,$@)/scripts/Input/build_params.txt
	@echo "BUILD_CONFIG_FLAGS=$(BUILD_CONFIG_FLAGS)" >> $(INSTALL_DIR)/$(patsubst %_install,%,$@)/scripts/Input/build_params.txt
	@echo "Q6_TOOLS_ROOT=$(Q6_TOOLS_ROOT)" >> $(INSTALL_DIR)/$(patsubst %_install,%,$@)/scripts/Input/build_params.txt
	@echo "### Installing $(patsubst %_install,%,$@)/lib ###"
	mkdir -p $(INSTALL_DIR)/$(patsubst %_install,%,$@)/lib
	cp -a $(patsubst %, $(BUILD_DIR)/$(patsubst %_install,%,$@)/%, $(RELEASE_OBJS)) $(INSTALL_DIR)/$(patsubst %_install,%,$@)/lib
	
	@echo "### Prepending cust_config.c ###"
	grep -i "THREAD_CONTEXT_TOTALSIZE" $(BUILD_DIR)/$(patsubst %_install,%,$@)/kernel/context_asm_offsets.h > $(BUILD_DIR)/$(patsubst %_install,%,$@)/kernel/tmpfile
	grep -i "QURT_CONFIG_MAX_HTHREAD" $(BUILD_DIR)/$(patsubst %_install,%,$@)/kernel/context_asm_offsets.h >> $(BUILD_DIR)/$(patsubst %_install,%,$@)/kernel/tmpfile
	cat $(INSTALL_DIR)/$(patsubst %_install,%,$@)/scripts/Input/cust_config_template.c >> $(BUILD_DIR)/$(patsubst %_install,%,$@)/kernel/tmpfile
	mv -f $(BUILD_DIR)/$(patsubst %_install,%,$@)/kernel/tmpfile $(INSTALL_DIR)/$(patsubst %_install,%,$@)/scripts/Input/cust_config.c

mkdirs:
	$(foreach target,$(TARGET),$(if $(wildcard $(BUILD_DIR)/$(target)),,mkdir -p $(BUILD_DIR)/$(target)))
	$(foreach target,$(TARGET),$(if $(wildcard $(INSTALL_DIR)/$(target)),,mkdir -p $(INSTALL_DIR)/$(target)))

$(TARGET_CLEAN):
	@echo "### Clean TARGET $(patsubst %_clean,%,$@) ###"
	rm -Rf $(BUILD_DIR)/$(patsubst %_clean,%,$@)
	rm -Rf $(INSTALL_DIR)/$(patsubst %_clean,%,$@)

clean: $(TARGET_CLEAN)
	-rmdir $(BUILD_DIR)
	-rmdir $(INSTALL_DIR)

cleanall:
	-rm -Rf $(BUILD_DIR)
	-rm -Rf $(INSTALL_DIR)

.PHONY: install dummy
