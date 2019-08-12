#-------------------------------------------------------------------------------
#
# Copyright (c) QUALCOMM Inc. 2009
#
# $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/bsp/build/msm7x30_apps.mak#1 $ 
# $DateTime: 2016/06/17 14:31:11 $ 
#
#-------------------------------------------------------------------------------
# when       who     what, where, why
# --------   ---     --------------------------------------------------------
# 06/02/09   cjb     Created

all : corebsp_scons corebsp 
#-------------------------------------------------------------------------------
# Path to ARM 1.2 tools
#-------------------------------------------------------------------------------
ARMHOME := $(subst \,/,$(ARMHOME))

#-------------------------------------------------------------------------------
# Flag this as an apps-only build
#-------------------------------------------------------------------------------
PROC = MULTI_PROC
IMAGE  = APPS_PROC
USES_APPS_PROC=yes
IMAGE_TAG  = A
TARGET_NAME := M8660$(BUILD)$(VERSION)

#-------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------
include dmss_flags.min
include dmss_7X30_flags.min
include incpaths.min
include armtools.min
-include amss_lint_flags.min
include $(COREBSP_BUILD)/dmss_objects.min
include $(COREBSP_BUILD)/corebsp_setup.min


#-------------------------------------------------------------------------------
# Object groups that are specific to this target
#-------------------------------------------------------------------------------
# INSERT OBJECTS MACROS HERE
# Build the full object list for this target

ifeq ($(USES_CORE_SERVICES_ONLY),yes)

   # The full object list for this core services build
   SCONS_COREBSP_OBJECTS = \
               $(REX_OBJS) \
               $(ANSIC_OBJS) \
               $(BIO_OBJS) \
               $(BOOT_OBJS) \
               $(CLK_OBJS) \
               $(DMOV_OBJS) \
               $(DMOV_MSM_OBJS) \
               $(FLASH_OBJS) \
               $(FS_OBJS) \
               $(HW_OBJS) \
               $(SIO_OBJS) \
               $(SLEEP_APPS_OBJS) \
               $(TASK_OBJS) \
               $(TIME_OBJS) \
               $(TRAMP_APPS_OBJS) \
               $(UTILS_OBJS) \
               $(TLMM_OBJS) \
               $(RDEVMAP_OBJS) \
               $(UART_OBJS)

   else # USES_CORE_SERVICES_ONLY
   SCONS_COREBSP_OBJECTS = \
               $(TASK_OBJS) \
               $(REX_OBJS) \
               $(BIO_OBJS) \
               $(BOOT_OBJS) \
               $(DMOV_OBJS) \
               $(CLK_OBJS) \
               $(CLKREGIM_OBJS) \
               $(DIAG_OBJS) \
               $(DMOV_MSM_OBJS) \
               $(FLASH_OBJS) \
               $(FS_OBJS) \
               $(HS_OBJS) \
               $(HS_SURF_OBJS) \
               $(HS_BREW_OBJS) \
               $(HW_OBJS) \
               $(KPD_OBJS) \
               $(REXL4_OBJS) \
               $(RTE_OBJS) \
               $(SIO_OBJS) \
               $(SLEEP_APPS_OBJS) \
               $(TIME_OBJS) \
               $(TRAMP_APPS_OBJS) \
               $(USB_OBJS) \
               $(UTILS_OBJS) \
               $(TLMM_OBJS) \
               $(TOUCH_OBJS) \
               $(RDEVMAP_OBJS) \
               $(UART_OBJS)

	COREBSP_OBJECTS += $(HSU_OBJS)
		
ifeq ($(USES_SECSERVICES_TESTAPP), yes)
      COREBSP_OBJECTS += $(SECSERVICES_TESTAPP_OBJS)
endif

ifeq ($(USES_CONTENTEVTMGRTEST), yes)
       COREBSP_OBJECTS += $(CONTENTEVTMGRTEST_OBJS)
endif

ifeq ($(USES_ICONTENTHANDLERTEST), yes)
       COREBSP_OBJECTS += $(ICONTENTHANDLERTEST_OBJS)
endif

ifeq ($(USES_DRM20), yes)
       COREBSP_OBJECTS += $(IXOMADRMCRYPTOALGORITHMGROUP_OBJS)
endif

ifeq ($(USES_SEC_CLNT), yes)
       COREBSP_OBJECTS += $(SEC_OEM_OBJS)
endif


   #----------------------------------------------------------------------------
   # Object List Modifiers
   #    Alter the object list based on the requested features
   #----------------------------------------------------------------------------
  ifeq ($(USES_L4),yes)
       SCONS_COREBSP_OBJECTS := $(filter-out $(DLOAD_OBJS), $(SCONS_COREBSP_OBJECTS))
       SCONS_COREBSP_OBJECTS := $(filter-out $(REX_OBJS), $(SCONS_COREBSP_OBJECTS))
       # SCONS_COREBSP_OBJECTS := $(filter-out $(BOOT_OBJS), $(SCONS_COREBSP_OBJECTS))
  endif  # USES_L4

  ifeq ($(USES_DEM),yes)
       SCONS_COREBSP_OBJECTS += $(DEM_APPS_OBJS)
  endif

endif    # End of USES_CORE_SERVICES_ONLY 

ifeq ($(USES_EXPORT_HSU_APP_APIS),yes)
 COREBSP_OBJECTS += $(HSU_APP_APIS_SERVER_OBJS)
endif

ifeq ($(USES_EXPORT_HSU_MDM_APIS),yes)
 COREBSP_OBJECTS += $(HSU_MDM_APIS_CLIENT_OBJS)
endif

#-------------------------------------------------------------------------------
# Add modules for exported interfaces
#-------------------------------------------------------------------------------

ifeq ($(USES_EXPORT_PMEM_REMOTE),yes)
   SCONS_COREBSP_OBJECTS += $(PMEM_REMOTE_CLIENT_OBJS)
endif

ifeq ($(USES_EXPORT_JOYST),yes)
  SCONS_COREBSP_OBJECTS += $(JOYST_CLIENT_OBJS)
endif

ifeq ($(USES_EXPORT_VJOY),yes)
  SCONS_COREBSP_OBJECTS += $(VJOY_SERVER_OBJS)
endif

ifeq ($(USES_EXPORT_JOYSTC),yes)
  SCONS_COREBSP_OBJECTS += $(JOYSTC_SERVER_OBJS)
endif

ifeq ($(USES_EXPORT_I2C),yes)
  SCONS_COREBSP_OBJECTS += $(I2C_CLIENT_OBJS)
endif

ifeq ($(USES_EXPORT_TIME_REMOTE_MTOA), yes)
  SCONS_COREBSP_OBJECTS += $(TIME_REMOTE_MTOA_SERVER_OBJS)
endif

ifeq ($(USES_EXPORT_TIME_REMOTE_ATOM), yes)
  SCONS_COREBSP_OBJECTS += $(TIME_REMOTE_ATOM_CLIENT_OBJS)
endif

ifeq ($(USES_EXPORT_NV),yes)
   SCONS_COREBSP_OBJECTS += $(NV_CLIENT_OBJS)
else
   ifneq ($(USES_CORE_SERVICES_ONLY),yes)
       SCONS_COREBSP_OBJECTS += $(NVIM_OBJS)
   endif
endif # end USES_EXPORT_NV

ifeq ($(USES_EXPORT_SECUTIL),yes)
  COREBSP_OBJECTS += $(SECUTIL_CLIENT_OBJS)
endif

ifeq ($(USES_EXPORT_SMD_PORT_MGR),yes)
  SCONS_COREBSP_OBJECTS += $(SMD_PORT_MGR_CLIENT_OBJS)
endif

ifeq ($(USES_EXPORT_SMD_BRIDGE),yes)   
  SCONS_COREBSP_OBJECTS += $(SMD_BRIDGE_APPS_OBJS)
endif

ifeq ($(USES_EXPORT_DOG_KEEPALIVE),yes)
    SCONS_COREBSP_OBJECTS += $(DOG_KEEPALIVE_SERVER_OBJS)
endif


ifeq ($(USES_EXPORT_BUS_PERF),yes)
    SCONS_COREBSP_OBJECTS += $(BUS_PERF_CLIENT_OBJS)
endif

ifeq ($(USES_EXPORT_BUS_MON),yes)
   SCONS_COREBSP_OBJECTS += $(BUS_MON_REMOTE_SERVER_OBJS)
   SCONS_COREBSP_OBJECTS += $(BUS_MON_REMOTE_CLIENT_OBJS)
endif

ifeq ($(USES_EXPORT_DIAG),yes)
  SCONS_COREBSP_OBJECTS += $(DIAG_CLIENT_OBJS)
endif

ifeq ($(USES_EXPORT_KEYPAD),yes)
   SCONS_COREBSP_OBJECTS += $(KEYPAD_SERVER_OBJS)
endif

#----------------------------------------------------------------------------
# Add modules for HAL
#----------------------------------------------------------------------------

ifeq ($(USES_HAL), yes)
  SCONS_COREBSP_OBJECTS += $(HAL_OBJS)
endif

#----------------------------------------------------------------------------
# Add modules for DAL
#----------------------------------------------------------------------------

ifeq ($(USES_DAL), yes)
	SCONS_COREBSP_OBJECTS += $(DAL_OBJS)
endif

#----------------------------------------------------------------------------

#++cjb - TODO
ifneq ($(USES_THINUI),yes)
ifeq ($(USES_BREW_USB_HID), yes)
    OBJECTS += $(EXT_USB_HID_OBJS)
endif
endif

#----------------------------------------------------------------------------
# Add modules for Buses
#----------------------------------------------------------------------------

SCONS_COREBSP_OBJECTS += $(BUSES_OBJS)

#-------------------------------------------------------------------------------
# Add modules for standalone version
#-------------------------------------------------------------------------------
ifeq ($(USES_STANDALONE),yes)
###  SCONS_COREBSP_OBJECTS += $(USB_OBJS)
endif

#-------------------------------------------------------------------------------
# UMTS data objects - only included if building Data-On-Apps
#-------------------------------------------------------------------------------
ifeq ($(USES_L4LINUX),yes)
       SCONS_COREBSP_OBJECTS += \
                 $(QDIAG_OBJS)
endif


#-------------------------------------------------------------------------------
# Object List Modifiers
#    Alter the object list based on the requested features
#-------------------------------------------------------------------------------

ifeq ($(USES_SDCC),yes)
       SCONS_COREBSP_OBJECTS += $(SDCC_OBJS)
endif

ifeq ($(USES_RTE),yes)
       SCONS_COREBSP_OBJECTS += $(RTE_OBJS)
endif 

#ifeq ($(USES_TIME),yes)
#        SCONS_COREBSP_OBJECTS += $(TIME_OBJS)
#endif 

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
endif 


ifeq ($(USES_EFS2),yes)
       SCONS_COREBSP_OBJECTS += $(EFS_OBJS)
endif

ifeq ($(USES_HFAT),yes) 
       SCONS_COREBSP_OBJECTS += $(HFAT_OBJS) 
endif

ifeq ($(USES_ERR_SERVICES),yes)
       SCONS_COREBSP_OBJECTS += $(ERR_OBJS)
endif

#++cjb - TODO
ifeq ($(USES_PBMTASK),yes)
        SCONS_COREBSP_OBJECTS += $(PBM_OBJS)
endif

ifeq ($(USES_JOYST),yes)
        SCONS_COREBSP_OBJECTS += $(VJOY_OBJS) \
                  $(JOYSTC_OBJS) \
                  $(EXT_IJOY_OBJS)
endif

ifeq ($(USES_BUS_MON),yes)
       SCONS_COREBSP_OBJECTS += $(BUSMON_OBJS)
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
DEPFILE := $(TARGETDIR)/$(TARGETDIR)_corebsp_apps.dep
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

