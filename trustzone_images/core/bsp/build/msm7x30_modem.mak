#-------------------------------------------------------------------------------
#
# Copyright (c) QUALCOMM Inc. 2006
#
# $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/bsp/build/msm7x30_modem.mak#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
#
#-------------------------------------------------------------------------------
# when       who     what, where, why
# --------   ---     --------------------------------------------------------
# 06/17/08   drh     Added BASE_PATHS to DEPOPT to correct dependency gen error
# 02/27/07   gfr     Added keypad remote apis.
# 04/18/06   hg      Initial revision. Ported from dmss7200modem.mak
#-------------------------------------------------------------------------------

all : corebsp_scons corebsp

#-------------------------------------------------------------------------------
# Path to ARM 1.2 tools
#-------------------------------------------------------------------------------
ARMHOME := $(subst \,/,$(ARMHOME))

#-------------------------------------------------------------------------------
# Boot security type
#-------------------------------------------------------------------------------
ifeq ($(USES_SECBOOT),yes)
   SEC_MODE = sec
else
   SEC_MODE = nonsec
endif

#-------------------------------------------------------------------------------
# Flag this as a modem-only build
#-------------------------------------------------------------------------------
PROC = MULTI_PROC
IMAGE = MODEM_PROC
USES_MODEM_PROC=yes
IMAGE_TAG = M
TARGET_NAME := $(IMAGE_TAG)7X30$(BUILD)$(VERSION)

#-------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------
include dmss_flags.min
include dmss_7X30_flags.min
include incpaths.min
include armtools.min
-include amss_lint_flags.min
include $(COREBSP_BUILD)/dmss_objects.min
#include boot_targets_$(SEC_MODE).min

#-------------------------------------------------------------------------------
# Object groups that are specific to this target
#-------------------------------------------------------------------------------
# INSERT OBJECTS MACROS HERE
# Build the full object list for this target
ifeq ($(USES_CORE_SERVICES_ONLY),yes)

 
         SCONS_COREBSP_OBJECTS = \
                 $(REX_OBJS) \
 		$(ANSIC_OBJS) \
                 $(BIO_OBJS) \
                 $(BOOT_OBJS) \
                 $(CLK_OBJS) \
                 $(DEM_OBJS) \
 		$(DLOAD_OBJS) \
 		$(DMOV_OBJS) \
 		$(DMOV_MSM_OBJS) \
                 $(FLASH_OBJS) \
                 $(FS_OBJS) \
 		$(HW_OBJS) \
                 $(SIO_OBJS) \
                 $(SLEEP_OBJS) \
                 $(TASK_OBJS) \
 		$(TIME_OBJS) \
                 $(TLMM_OBJS) \
                 $(TRAMP_MODEM_OBJS) \
                 $(USB_CORE_OBJS) \
                 $(UTILS_OBJS) \
                 $(RDEVMAP_OBJS) \
                 $(UART_OBJS)
		
else
SCONS_COREBSP_OBJECTS = \
 		$(ADC_OBJS) \
                 $(ANSIC_OBJS) \
 		$(BIO_OBJS) \
                 $(BOOT_OBJS) \
                 $(CLK_OBJS) \
                 $(DEM_OBJS) \
                 $(DIAG_OBJS) \
                 $(DLOAD_OBJS) \
 		$(DMOV_MSM_OBJS) \
                 $(DMOV_OBJS) \
                 $(DSM_OBJS) \
                 $(FLASH_OBJS) \
                 $(EFUSE_OBJS) \
                 $(FS_OBJS) \
 		$(HS_SURF_OBJS) \
                 $(HSU_OBJS) \
                 $(HW_OBJS) \
                 $(MAO_OBJS) \
                 $(MSMSUPPORT_OBJS) \
                 $(NVIM_OBJS) \
                 $(OEM_RAPI_OBJS) \
                 $(REX_OBJS) \
                 $(RTE_OBJS) \
                 $(SIO_OBJS) \
                 $(SLEEP_OBJS) \
                 $(TASK_OBJS) \
                 $(TIME_OBJS) \
                 $(TLMM_OBJS) \
                 $(TMC_OBJS) \
                 $(TRAMP_MODEM_OBJS) \
                 $(UTILS_OBJS) \
                 $(RDEVMAP_OBJS) \
                 $(UART_OBJS)
		

#-------------------------------------------------------------------------------
# Target specific objects
#-------------------------------------------------------------------------------

# 7500 specific objects
  ifeq ($(TARGET_ID), 7500)  #Need to change to 7500 DNN    
    SCONS_COREBSP_OBJECTS += \
                 $(CLKRGM_OBJS) \
                 $(KPD_OBJS)

  endif
    # TARGET_ID =7500

# 76XX specific objects
    SCONS_COREBSP_OBJECTS += \
 	        $(CLKREGIM_OBJS) \
                 $(REXL4_OBJS)

    ifeq ($(USES_WPLT),yes)
        # 76XX WPLT specific objects
        COREBSP_OBJECTS += \
 		$(USB_OBJS)

    else
        # 76XX dual-processor specific obejcts
        SCONS_COREBSP_OBJECTS += \
                 $(KPD_OBJS)

    endif
    # end of USES_WPLT



endif 
# End of USES_CORE_SERVICES_ONL 

#-------------------------------------------------------------------------------
# Add modules for the exported interfaces
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Object List Modifiers
#    Alter the object list based on the requested features
#-------------------------------------------------------------------------------
 ifeq ($(USES_L4),yes)
         SCONS_COREBSP_OBJECTS := $(filter-out $(DLOAD_OBJS), $(SCONS_COREBSP_OBJECTS))
         SCONS_COREBSP_OBJECTS := $(filter-out $(REX_OBJS), $(SCONS_COREBSP_OBJECTS))
         # SCONS_COREBSP_OBJECTS := $(filter-out $(BOOT_OBJS), $(SCONS_COREBSP_OBJECTS))
 endif

# common data objects always included

 ifeq ($(USES_STANDALONE_MODEM),yes)
   COREBSP_OBJECTS += $(USB_OBJS)           
 endif

ifeq ($(USES_EXPORT_JOYST),yes)
   SCONS_COREBSP_OBJECTS += $(JOYST_SERVER_OBJS)
endif

ifeq ($(USES_EXPORT_VJOY),yes)
   SCONS_COREBSP_OBJECTS += $(VJOY_CLIENT_OBJS)
endif

ifeq ($(USES_EXPORT_JOYSTC),yes)
   SCONS_COREBSP_OBJECTS += $(JOYSTC_CLIENT_OBJS)
endif

 ifeq ($(USES_EXPORT_I2C),yes)
    SCONS_COREBSP_OBJECTS += $(I2C_SERVER_OBJS)
 endif

 ifeq ($(USES_EXPORT_TIME_REMOTE_MTOA), yes)
    SCONS_COREBSP_OBJECTS += $(TIME_REMOTE_MTOA_CLIENT_OBJS)
 endif

 ifeq ($(USES_EXPORT_TIME_REMOTE_ATOM), yes)
    SCONS_COREBSP_OBJECTS += $(TIME_REMOTE_ATOM_SERVER_OBJS)
 endif

 ifeq ($(USES_EXPORT_NV),yes)
     SCONS_COREBSP_OBJECTS += $(NV_SERVER_OBJS)
 endif

#++cjb - TODO 
ifeq ($(USES_EXPORT_SECUTIL),yes)
   COREBSP_OBJECTS += $(SECUTIL_SERVER_OBJS)
endif

 ifeq ($(USES_EXPORT_SMD_PORT_MGR),yes)
    SCONS_COREBSP_OBJECTS += $(SMD_PORT_MGR_SERVER_OBJS)
 endif

 ifeq ($(USES_EXPORT_SMD_BRIDGE),yes)
    SCONS_COREBSP_OBJECTS += $(SMD_BRIDGE_MODEM_OBJS)
 endif

 ifeq ($(USES_EXPORT_OEM_RAPI),yes)
    SCONS_COREBSP_OBJECTS += $(OEM_RAPI_SERVER_OBJS)
 endif

 ifeq ($(USES_EXPORT_DOG_KEEPALIVE),yes)
      SCONS_COREBSP_OBJECTS += $(DOG_KEEPALIVE_CLIENT_OBJS)
 endif

 ifeq ($(USES_EXPORT_BUS_PERF),yes)
         SCONS_COREBSP_OBJECTS += $(BUS_PERF_SERVER_OBJS)
 endif

 ifeq ($(USES_EXPORT_BUS_MON),yes)
     SCONS_COREBSP_OBJECTS += $(BUS_MON_REMOTE_SERVER_OBJS)
     SCONS_COREBSP_OBJECTS += $(BUS_MON_REMOTE_CLIENT_OBJS)
 endif

 ifeq ($(USES_EXPORT_DIAG),yes)
    SCONS_COREBSP_OBJECTS += $(DIAG_SERVER_OBJS)
 endif

 ifeq ($(USES_EXPORT_KEYPAD),yes)
     SCONS_COREBSP_OBJECTS += $(KEYPAD_CLIENT_OBJS)
 endif

 ifeq ($(USES_EXPORT_HSU_APP_APIS),yes)
   COREBSP_OBJECTS += $(HSU_APP_APIS_CLIENT_OBJS)
 endif

 ifeq ($(USES_EXPORT_HSU_MDM_APIS),yes)
   COREBSP_OBJECTS += $(HSU_MDM_APIS_SERVER_OBJS)
 endif

 ifeq ($(USES_EXPORT_ISENSE),yes)
   COREBSP_OBJECTS += $(ISENSE_SERVER_OBJS)
 endif

#----------------------------------------------------------------------------
# Add modules for HAL
#----------------------------------------------------------------------------

ifeq ($(USES_HAL), yes)
  SCONS_COREBSP_OBJECTS += $(HAL_OBJS)
endif

 #----------------------------------------------------------------------------
 # Add modules for Buses
 #----------------------------------------------------------------------------
 
 SCONS_COREBSP_OBJECTS += $(BUSES_OBJS)

#-------------------------------------------------------------------------------
# Object List Modifiers
#    Alter the object list based on the requested features
#-------------------------------------------------------------------------------

 ifeq ($(USES_ERR_SERVICES),yes)
         SCONS_COREBSP_OBJECTS += $(ERR_OBJS)
 endif

 ifeq ($(USES_SDCC),yes)
 #   SCONS_COREBSP_OBJECTS += $(SDCC_OBJS)
 endif

 ifeq ($(USES_EXPORT_SDCC_CPRM),yes)
   SCONS_COREBSP_OBJECTS += $(SDCC_CPRM_CLIENT_OBJS)
 endif


 ifeq ($(USES_TIME),yes)
         SCONS_COREBSP_OBJECTS += $(TIME_OBJS)
 endif

 ifeq ($(USES_SMEM),yes)
 SCONS_COREBSP_OBJECTS += $(SMEM_OBJS)
 SCONS_COREBSP_OBJECTS += $(SMEM_LOG_OBJS)
 ifeq ($(USES_SMD),yes)
 SCONS_COREBSP_OBJECTS += $(SMD_OBJS)
 endif
 endif

 ifeq ($(USES_ONCRPC),yes)
         SCONS_COREBSP_OBJECTS += $(ONCRPC_OBJS)
       SCONS_COREBSP_OBJECTS += $(PING_MDM_OBJS)
 ifeq ($(USES_ONCRPC_ROUTER),yes)
   SCONS_COREBSP_OBJECTS += $(ONCRPC_ROUTER_OBJS)
 endif
 ifeq ($(USES_OEM_RAPI),yes)
      COREBSP_OBJECTS += $(OEM_RAPI_OBJS)
 endif
 endif

 ifeq ($(USES_EFS2),yes)
         SCONS_COREBSP_OBJECTS += $(EFS_OBJS)
 endif

 ifeq ($(USES_ERR_SERVICES),yes)
         SCONS_COREBSP_OBJECTS += $(ERR_OBJS)
 endif

 ifeq ($(USES_JOYST),yes)
   SCONS_COREBSP_OBJECTS += $(JOYST_OBJS)
 endif

 ifeq ($(USES_BUS_PERF),yes)
         SCONS_COREBSP_OBJECTS += $(BUSPERF_OBJS)
 endif

 ifeq ($(USES_BUS_MON),yes)
         SCONS_COREBSP_OBJECTS += $(BUSMON_OBJS)
 endif

#++cjb - TODO
ifeq ($(USES_SEC_CLNT), yes)
        COREBSP_OBJECTS += $(SEC_OEM_OBJS)
endif

ifeq ($(USES_SENSORS),yes)
        COREBSP_OBJECTS += $(OEMSENSORPORT_OBJS)
        COREBSP_OBJECTS += $(SENSORS_OBJS)
        COREBSP_OBJECTS += $(SENSORS_API_OBJS)
        COREBSP_OBJECTS += $(SNSDRV_OBJS)
endif

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
DEPFILE := $(TARGETDIR)/$(TARGETDIR)_corebsp_modem.dep
DEPOPTS := $(COREBSP_BASE_INCLUDES) $(COREBSP_XALL_INCLUDES) -f $(DEPFILE) -bid $(BUILD)
ifneq ($(MEM_USAGE),yes)
-include $(DEPFILE)
endif

OBJDIR = $(TARGETDIR)

#-------------------------------------------------------------------------------
# Build Rules
#-------------------------------------------------------------------------------
ifeq ($(USES_THINUI),yes)
vpath %.c $(TARGETDIR)
endif
include $(COREBSP_BUILD)/dmss_rules.min
-include amss_lint_rules.min
