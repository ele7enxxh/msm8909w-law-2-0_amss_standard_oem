#-------------------------------------------------------------------------------
#
# Copyright (c) QUALCOMM Inc. 2009
#
# $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/bsp/build/msm7x30_lib.mak#1 $ 
# $DateTime: 2016/06/17 14:31:11 $
#
#-------------------------------------------------------------------------------
# when       who     what, where, why
# --------   ---     --------------------------------------------------------
# 06/02/09   cjb     Created
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Path to ARM 1.2 tools
#-------------------------------------------------------------------------------
ARMHOME := $(subst \,/,$(ARMHOME))

ifeq ($(USES_STRIP),yes)
	USES_NO_DEBUG = yes
endif

#-------------------------------------------------------------------------------
# Create TARGET_NAME based on build type
#-------------------------------------------------------------------------------
ifeq ($(IMAGE), APPS_PROC)
        IMAGE_TAG = A
        USES_APPS_PROC=yes
endif
ifeq ($(IMAGE), MODEM_PROC)
        IMAGE_TAG = M
        USES_MODEM_PROC=yes
endif

ifeq ($(USES_COMBINED_IMAGE), yes)
        USES_APPS_PROC=yes
        USES_MODEM_PROC=yes
endif

TARGET_NAME := M7X30$(BUILD)$(VERSION)

#-------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------
include dmss_flags.min
include dmss_7X30_flags.min
include incpaths.min
include armtools.min
-include amss_lint_flags.min

#-------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------
corebsplibs: corebsplibs_begin $(TARGETDIR)/exist corebsp_deps corebsplib corebsplibs_complete

corebsplibs_begin :
	@echo ------------------------------------------------------------------------
	@echo CoreBSP Libraries - Start
	@echo ------------------------------------------------------------------------

corebsplibs_complete :
	@echo ------------------------------------------------------------------------
	@echo CoreBSP Libraries - Complete
	@echo ------------------------------------------------------------------------

#===============================================================================
#                              HAL LIBRARIES
#===============================================================================

ifeq ($(USES_HAL), yes)
  HALROOT=   $(SRCROOT)/hal
  include    $(HALROOT)/HAL.min
endif

#===============================================================================
#                              APPS PROC ONLY LIBRARIES
#===============================================================================

ifeq ($(USES_APPS_PROC), yes)
ifeq ($(USES_CORE_SERVICES_ONLY), yes)

else # USES_CORE_SERVICES_ONLY

endif   # USES_CORE_SERVICES_ONLY
endif   # end of ifeq ($(USES_APPS_PROC), yes)

#===============================================================================
#                              MODEM PROC ONLY LIBRARIES
#===============================================================================
ifeq ($(USES_SENSORS),yes)
  include $(SENSORS)/src/lib_sensors.min
endif

ifeq ($(USES_MODEM_PROC), yes)
ifeq ($(USES_CORE_SERVICES_ONLY), yes)

else # USES_CORE_SERVICES_ONLY

# ------------------------
# 76XX specific libraries
# ------------------------

ifneq ($(TARGET_ID), 7500)
ifeq ($(USES_DKPROV), yes)
    include $(SME_DRMCOM_DRM_SRC)/lib_drmcom_drm.min
endif

endif    # end of 76XX lib
endif   # else USER_CORE_SERVICES_ONLY
endif   # end of ifeq ($(USES_MODEM_PROC), yes)

#===============================================================================
#                              COMMON LIBRARIES
#===============================================================================

#++scons: include $(HW_SRC)/hw_lib.min

#-------------------------------------------------------------------------------
# Message Levels
#    These operate on object groups - so these instructions must appear after
#    all the object groups are named
#-------------------------------------------------------------------------------
include dmss_msg_config.min

#-------------------------------------------------------------------------------
# Dependency Generation
#    Names the location of the dependency file and options to the dependency
#    generator.
#-------------------------------------------------------------------------------
DEPFILE := $(LIBDIR)/corebsp_libs.dep
DEPOPTS := -XS ../../apps/brew/ -XS ../../apps/ui -XS ../../modem -XS ../../multimedia -d ../.. -f $(DEPFILE)
-include $(DEPFILE)

OBJDIR = $(LIBDIR)

#-------------------------------------------------------------------------------
# Build Rules
#-------------------------------------------------------------------------------
include $(COREBSP_BUILD)/dmss_rules.min
