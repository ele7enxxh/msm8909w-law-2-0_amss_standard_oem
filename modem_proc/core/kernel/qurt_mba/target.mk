#=============================================================================
#
#                 target.mk -- H E A D E R  F I L E
#
#GENERAL DESCRIPTION
#   makefile target
#   
#EXTERNAL FUNCTIONS
#   none
#
#INITIALIZATION AND SEQUENCING REQUIREMENTS
#   None.
#
#      Copyright (c) 2009
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
#$Header: //source/qcom/qct/core/kernel/blast/main/latest/target.mk#15 $ 
#$DateTime: 2011/03/14 21:13:03 $ $Author: opham $
#
#when       who     what, where, why
#--------   ---     ------------------------------------------------------------
#03/03/11   op      Add header file
#=============================================================================*/

RELEASE_TARGETS = modemFWv4 modemSWv4 MMv4 modemv5 MMv5 simv5 ADSPv5MP

# Default Q6 tools roots for different build environments and targets
DEFAULT_Q6_TOOLS_ROOT_LINUX_V4=/pkg/qct/software/hexagon/releases/tools/3.1.04
DEFAULT_Q6_TOOLS_ROOT_WIN_V4=C:\Qualcomm\HEXAGON_Tools\3.1.04
DEFAULT_Q6_TOOLS_ROOT_LINUX_V5=/pkg/qct/software/hexagon/releases/tools/5.0.04
DEFAULT_Q6_TOOLS_ROOT_WIN_V5=C:\Qualcomm\HEXAGON_Tools\5.0.04

# XXX Note: 
# The idea behind these definitions is to have override definitions that
# doesn't show TARGET name.  Somehow, I couldn't get this working.
#
# Say, target.mk has the definitions has below
#simv2_Q6VERSION =v2
#simv2_CONFIG_DIR=sim
#simv2_TOOLS_VER=1.1
#
# And have the following common definitions in Makefile
#$(TARGET): override Q6VERSION := $$($$@_Q6VERSION)
#$(TARGET): override CONFIG_DIR := $(QURT_TOP)/config/$$($$@_CONFIG_DIR)
#$(TARGET): override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/$$($$@_CONFIG_DIR))
#ifeq ($(Q6_TOOLS_ROOT),$(empty))
#ifneq ($(findstring Linux,$(shell uname)),$(empty))
#$(TARGET): override Q6_TOOLS_ROOT := /pkg/qct/software/hexagon/releases/tools/$$($$@_TOOLS_VER)
#else
#ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
#$(TTARGE): override Q6_TOOLS_ROOT := "C:\Qualcomm\HEXAGON_Tools\$$($$@_TOOLS_VER)"
#else
#$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
#endif
#endif
#endif
#

modemFWv4: override Q6VERSION := v4
modemFWv4: override CONFIG_DIR := $(QURT_TOP)/config/modemFWv4
modemFWv4: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/modemFWv4)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
modemFWv4: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V4)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
modemFWv4: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V4)
else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif

modemSWv4: override Q6VERSION := v4
modemSWv4: override CONFIG_DIR := $(QURT_TOP)/config/modemSWv4
modemSWv4: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/modemSWv4)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
modemSWv4: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V4)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
modemSWv4: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V4)
else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif

modemv5: override Q6VERSION := v5
modemv5: override CONFIG_DIR := $(QURT_TOP)/config/modemv5
modemv5: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/modemv5)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
modemv5: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V5)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
modemv5: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V5)
else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif

MMv4: override Q6VERSION := v4
MMv4: override CONFIG_DIR := $(QURT_TOP)/config/MMv4
MMv4: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/MMv4)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
MMv4: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V4)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
MMv4: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V4)
else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif

MMv5: override Q6VERSION := v5
MMv5: override CONFIG_DIR := $(QURT_TOP)/config/MMv5
MMv5: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/MMv5)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
MMv5: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V5)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
MMv5: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V5)
else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif

ADSPv5MP: override Q6VERSION := v5
ADSPv5MP: override CONFIG_DIR := $(QURT_TOP)/config/ADSPv5MP
ADSPv5MP: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/ADSPv5MP)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
ADSPv5MP: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V5)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
ADSPv5MP: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V5)
else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif


simv4: override Q6VERSION := v4
simv4: override CONFIG_DIR := $(QURT_TOP)/config/sim
simv4: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/sim)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
simv4: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V4)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
simv4: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V4)
else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif

simv5: override Q6VERSION := v5
simv5: override CONFIG_DIR := $(QURT_TOP)/config/sim
simv5: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/sim)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
simv5: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V5)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
simv5: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V5)
else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif

simv4_bitmask: override Q6VERSION := v4
simv4_bitmask: override CONFIG_DIR := $(QURT_TOP)/config/sim_bitmask
simv4_bitmask: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/sim_bitmask)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
simv4_bitmask: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V4)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
simv4_bitmask: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V4)
else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif

simv5_bitmask: override Q6VERSION := v5
simv5_bitmask: override CONFIG_DIR := $(QURT_TOP)/config/sim_bitmask
simv5_bitmask: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/sim_bitmask)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
simv5_bitmask: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V5)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
simv5_bitmask: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V5)
else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif

mpqurt: override Q6VERSION := v5
mpqurt: override CONFIG_DIR := $(QURT_TOP)/config/mpqurt
mpqurt: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/mpqurt)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
mpqurt: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V5)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
mpqurt: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V5)
else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif

staticv4: override Q6VERSION := v4
staticv4: override CONFIG_DIR := $(QURT_TOP)/config/static
staticv4: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/static)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
staticv4: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V4)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
staticv4: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V4)

else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif

staticv5: override Q6VERSION := v5
staticv5: override CONFIG_DIR := $(QURT_TOP)/config/static
staticv5: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/static)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
staticv5: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V5)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
staticv5: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V5)

else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif

MBAv5: override Q6VERSION := v5
MBAv5: override CONFIG_DIR := $(QURT_TOP)/config/MBAv5
MBAv5: override BUILD_CONFIG_FLAGS := $(shell python kernel/scripts/build_flags.py config/MBAv5)
ifeq ($(Q6_TOOLS_ROOT),$(empty))
ifneq ($(findstring Linux,$(shell uname)),$(empty))
MBAv5: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_LINUX_V5)
else
ifneq ($(findstring CYGWIN,$(shell uname)),$(empty))
MBAv5: override Q6_TOOLS_ROOT := $(DEFAULT_Q6_TOOLS_ROOT_WIN_V5)

else
$(error Q6_TOOLS_ROOT not defined and UNKNOWN Platform !!!)
endif
endif
endif
