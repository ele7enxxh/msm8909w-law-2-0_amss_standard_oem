/**
 * @file sbl1_config.c
 * @brief
 * Source file for 8909 SBL1 bootloader configuration
 *
 * DEFINED VARIABLES:
 * sbl1_config_table   - Boot configuration entries for SBL1
 * load_qsee_pre_procs - SBL1 procedures run prior to QSEE loading
 * load_qsee_post_procs - SBL1 procedures run post QSEE loading
 */

/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
        
when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/19/14   xl      Add sbl1_ddr_set_params_enhance, enhance set
12/19/14   sc      Enabled whitelist region check
12/17/14   sve	   Code changes for merging firehose DeviceProgrammer into SBL
12/05/14   sve	   Placed sbl1_hw_init after CDT initialization in qsee_preprocs
09/30/14   sk      Cache settings in qsee_jump_func before jumping to Qsee entry point
09/18/14   lm      code clean-up and calling boot_flash_init based on feature flag (SDCC_BOOT) 
08/05/14   sk      8909 SBL NAND changes
07/22/14   sk      Changes for 8909 EMMC
06/20/14   ck      Modified sbl1_cleanse_qsee_region and boot_dload_dump_qsee
                   for QHEE and renamed to sbl1_cleanse_security_regions and boot_dload_dump_security_regions
05/14/14   ck      Added calls to sbl1_cleanse_qsee_region and boot_dload_dump_qsee
04/15/14   ck      Moved boot_dload_check before loading of QSEE as debug info is in SBL
04/10/14   ck      Added call to APT library
03/24/14   ck      Utilizing new boot_sbl_qsee_interface_get_* functions
03/21/14   ck      Added support to load SEC partition
03/18/14   ck      Updated boot_hw_reset calls as they now take a reset type parameter
03/06/14   ck      Moved sbl1_populate_initial_mem_map to sbl1_main_ctl
03/05/14   ck      Enabling AUTH for loaded images
02/28/14   ck      Moved sbl1_hw_deinit as it MUST be after sbl1_boot_logger_deinit
02/26/14   ck      Added logic to mark QSEE page table has Executable right before jumping
02/12/14   ck      Added AARCH64_ENABLE fuse check
02/10/14   ck      sbl1_tlmm_init needs to be after SMEM initialization
02/05/14   ck      Added 64 bit logic to qsee_jump_func
01/28/14   kpa     Update dload check, smem api locations
01/09/14   ck      Added logic to qsee_jump_func to store QSEE entry point and elf type
01/08/14   ck      Added boot_secure_watchdog_init call
12/18/13   ck      Removed call to boot_dload_dump_tz
12/16/13   ck      Removed sbl1_qfprom_test as TZ runs after SBL in Bear family.
12/09/13   ck      Removed call to sbl1_update_sbl1_rollback_fuse and
                   sbl1_update_all_rollback_fuses as this is done in QSEE for Bear family
12/06/13   ck      Replaced boot_pbl_is_auth_enabled call with boot_is_auth_enabled
10/24/13   ck      Bear boot flow is very linear now, everything initializes
                   before the first image is loaded.
10/17/13   ck      Removed unused functions.  Memory map change.
08/13/13   aus     Pass the TZ secboot verified info structure to TZ image
08/08/13   jz      Fixed the issue in WDT load when the partition doesn't exist
07/29/13   aus     Support for loading ELF SDI image
06/26/13   aus     Remove sbl1_dead_battery_charge
05/08/13   yp      Call boot_pm_vib_off to stop vibrate for 8X26 and 8x10.
04/02/13   dh      Use boot_dload_is_dload_mode_set instead of uefi dump cookie
                   to determin if device is in dload mode
03/11/13   kedara  boot config entry to control xpu configurations per image.
03/05/13   dh      Use mmu_l1_pagetable_base in sbl_shared_data as a common
                   pointer to page table base
02/28/13   kedara  call boot_set_tz_execute_status to indicate tz ready.
02/27/13   dh      Don't lower krait frequency prior to ddr training 
                   if we are in uefi dump mode 
                   Remove sbl1_relocate_page_table from dload path.
                   it will be called in sbl1_post_ddr_init if dload mode is set
02/27/13   dh      Call sbl1_hw_platform_pre_ddr before rpm is loaded to share 
                   platform id to rpm, call sbl1_hw_platform_smem to store 
                   platform info to smem
02/20/13   dh      Move all qfprom api after rpm xpu sync
02/15/13   dh      Add boot_dload_dump_tz in pre tz functions
02/14/13   kedara  Added sbl1_update_all_rollback_fuses in load_appsbl_post_procs
02/12/13   dh      Call boot_clock_set_L2_perf_level and boot_clock_set_cpu_perf_level
                   to Put krait to SVS mode before rpm is released
02/07/13   dh      Move SDI loading after RPM, delay page table relocation until DDR training
01/24/13   jz      Updated memory region names
01/24/13   dh      Pass secboot_shared_data to SDI as we pass it to TZ
12/12/12   jz      Do not load SDI when in dload mode
12/03/12   dh      Remove sbl1_rollback_version_update
11/27/12   dh      Add sbl1_rpm_sync in load_appsbl_post_procs
11/28/12   dh      Add a field in config table for Roll back protection feature
                   Blow sbl1 version roll back protection fuse as soon as tz is initialized
11/16/12   dh      Check for L2 cache parity error in appsbl_jump_func, if error
                   exists set dload cookie and reset
                   Move sbl1_boot_logger_deinit inside appsbl_jump_func
           Move boot_shared_imem_init before sbl1_hw_init in sbl1_main_ctl
10/23/12   dh      Map RPM code ram and data ram as shared device to prevent XPU violation
                   before signal rpm fw to go
10/17/12   jz      Turn on TZ elf image support
10/08/12   jz      Turn on RPM elf image support
09/26/12   dh      Move sbl1_hw_init_secondary after sbl1_post_ddr_data_init
           since pmic api in hw_init_secondary uses DDR heap
09/19/12   dh      Only load debug image if its image header is valid, this will support
                   optional debug image flashing
08/20/12   kedara  Added flag to support loading elf in chunks (ssa)
08/01/12   dh      Add sbl1_hw_deinit
07/23/12   dh      Add sbl1_wait_for_ddr_training 
07/19/12   dh      Add sbl1_qfprom_test
07/16/12   dh      Move sbl1_hw_init to sbl1_main_ctl
06/14/12   kedara  Initialize whitelist config table entry to NULL
05/29/12   dh      Move sbl1_hw_init_secondary right after ddr init
                   Add sbl1_tlmm_init after boot_smem_init
05/24/12   dh      combine sbl1_ddr_data_init and sbl1_relocate_page_table into
                   sbl1_post_ddr_data_init
05/24/12   dh      Delay rpm jump to after smem init since rpm uses smem
05/08/12   dh      Add sbl1_dead_battery_charge
04/18/12   dh      passing appsbl entry address in tz milestone call as tz will
                   jump to appsbl.Mark TZ as non-executable and read only after
                   we executed it
03/22/12   dh      Load the wdog debug image(WDT) after tz
03/14/12   dh      relocate page table to ddr once ddr is up
02/03/12   dh      Skip tz execution for pre silicon build
12/01/11   dh      Add sbl1_rollback_version_update and sbl1_signal_tz_sbl_done.
11/30/11   dh      Add sbl1_ddr_data_init
10/31/11   dh      Add dload function list
10/18/11   dh      Initial Creation 

============================================================================
                     Copyright 2014 Qualcomm Technologies Incorporated.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================
                             INCLUDE FILES
===========================================================================*/
#include "boot_mc.h"
#include "boot_config.h"
#include "boot_flash_target.h"
#include "boot_flash_dev_if.h"
#include "boot_authenticator.h"
#include "boot_config_data.h"
#include "boot_dload.h"
#include "boot_loader.h"
#include "boot_ram_init.h"
#include "boot_logger.h"
#include "boot_page_table_armv7.h"
#include "boot_cache_mmu.h"
#include "boot_smem.h"
#include "boot_ram_partition.h"
#include "boot_sd_ramdump.h"
#include "sbl1_hw.h"
#include "sbl1_mc.h"
#include "boot_dload.h"
#include "boot_dload_debug.h"
#include "boothw_target.h"
#include "boot_rollback_version.h"
#include "boot_extern_clk_interface.h"
#include "boot_extern_ddr_interface.h"
#include "boot_shared_imem_cookie.h"
#include "boot_raw_partition_ramdump.h"
#include "boot_extern_pmic_interface.h"
#include "boot_extern_deviceprogrammer_ddr_interface.h"
#include "boot_extern_apt_interface.h"
#include "boot_elf_header.h"
#include "boot_secure_watchdog.h"
#include "sys_debug.h"
#include "boot_flash_dev_nand.h"
#include "boot_util.h"

#include "miprogressive.h"
#include "boot_extern_efs_interface.h"


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*==========================================================================
                      DEFINE WHITELIST TABLES
===========================================================================*/
static whitelist_region_type sbl1_table[] = {SBL1_WHITELIST_REGION_TABLE};



/*==========================================================================
                      DEFINE PARTITION IDs
===========================================================================*/
uint8 rpm_partition_name[PARTITION_ID_MAX_SIZE] = {"0:RPM"};
uint8 appsbl_partition_name[PARTITION_ID_MAX_SIZE] = {"0:aboot"};
uint8 qsee_partition_name[PARTITION_ID_MAX_SIZE] = {"0:TZ"};
/* The following image partition IDs are in the form of GPT partition GUID (for eMMC boot) */
extern uint8 qsee_partition_id[];
extern uint8 rpm_partition_id[];
extern uint8 appsbl_partition_id[];

/*==========================================================================
                      DEFINE BOOT LOGGER STR
===========================================================================*/
#define QSEE_BOOT_LOG_STR "QSEE Image Loaded, Delta"
#define RPM_BOOT_LOG_STR "RPM Image Loaded, Delta"
#define APPSBL_BOOT_LOG_STR "APPSBL Image Loaded, Delta"

boot_configuration_table_entry sbl1_config_table[];


/* SYSTEM_DEBUG_QSEE_INFO is memory specified in the scatter file for SBL
   and system debug to share information about where QSEE was loaded and
   if it's a 32 bit or 64 bit image. */
extern uint32 Image$$SYSTEM_DEBUG_QSEE_INFO$$Base;

/*==========================================================================
                     PRE/POST PROCEDURE DEFINITIONS 
===========================================================================*/
/*===========================================================================
**  Function :  sbl1_update_partition_ids
** ==========================================================================
*/
/*!
* 
* @brief
*  This function updates the image partiton id with the corresponding 
*  partition GUIDs for boot targets that are GUID based (eMMC/SD/UFS)
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*  None
* 
*/
static void sbl1_update_partition_ids(bl_shared_data_type *bl_shared_data)
{
  boot_flash_type boot_device;
  BL_VERIFY( bl_shared_data != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Get the boot device type */
  boot_device = boot_pbl_get_flash_type();

  if ((boot_device == MMC_FLASH)  || (boot_device == SDC_FLASH)  || (boot_device == UFS_FLASH) )
  {
    qmemcpy(qsee_partition_name, qsee_partition_id, sizeof(struct hotplug_guid));
    qmemcpy(rpm_partition_name, rpm_partition_id, sizeof(struct hotplug_guid));
    qmemcpy(appsbl_partition_name, appsbl_partition_id, sizeof(struct hotplug_guid));
  }
}

/*==========================================================================
                  FUNCTIONS TO EXECUTE BEFORE TZ LOADING
===========================================================================*/
boot_procedure_func_type load_qsee_pre_procs[] = 
{
  /* Save reset register logs */
  boot_save_reset_register_log,

  /*----------------------------------------------------------------------
   Update the partition IDs to be used for GUID based boot targets
  ----------------------------------------------------------------------*/  
  sbl1_update_partition_ids,

#ifdef FEATURE_BOOT_SDCC_BOOT  
  /* Initialize the flash device */
  boot_flash_init,
#endif  
  /* Copy the configure data table from eeprom */
  boot_config_data_table_init,
  
  /* Store platform id */
  sbl1_hw_platform_pre_ddr,
  
  (boot_procedure_func_type)sbl1_hw_init, 
  
  /* Configure ddr parameters based on eeprom CDT table data. */
  sbl1_ddr_set_params,
  
  /* Initialize DDR */
  (boot_procedure_func_type)sbl1_ddr_init,

  /*----------------------------------------------------------------------
   In SBL builds the function below is stubbed out (does nothing).
   When building Deviceprogrammer the function below never returns.
   The function below becomes active by defining the feature
   FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_DDR_COMPLETED.
  ----------------------------------------------------------------------*/
  boot_hand_control_to_deviceprogrammer_ddr_main,    
  
  /*Set ddr parameters in CDB2&CDB3, enhance set*/
  sbl1_ddr_set_params_enhance,
  
  /* Initialize SBL1 DDR ZI region, relocate boot log to DDR */   
  sbl1_post_ddr_init,
  
  sbl1_hw_init_secondary,
 
  /* DDR training */
  (boot_procedure_func_type)sbl1_wait_for_ddr_training,
  
  /* Initialize SBL1 DDR ZI region, relocate page table to DDR */
  sbl1_post_ddr_training_init, 
  
  /*----------------------------------------------------------------------
   Initialize modules post DDR init
  ----------------------------------------------------------------------*/  
#ifndef FEATURE_BOOT_SDCC_BOOT  
  /*----------------------------------------------------------------------
   Initialize the flash device
   NOTE: NAND flash init depends on DDR to be initialized for using the heap
  ----------------------------------------------------------------------*/
  boot_flash_init,
#endif  
  /* Zero out QSEE and QHEE region if needed.  This MUST be done before
     boot_dload_dump_security_regions executes for security reasons. */
  sbl1_cleanse_security_regions,

  /* Backup QSEE and QHEE region for ramdumps taken after SBL has executed */
  boot_dload_dump_security_regions,

  /* Check to see if DLOAD mode needs to be entered */
  boot_dload_check,

  /* Last entry in the table. */
  NULL 
};

/*==========================================================================
   FUNCTIONS TO EXECUTE AFTER QSEE LOADING
===========================================================================*/
boot_procedure_func_type load_qsee_post_procs[] = 
{
  /* Enable the secure watchdog
     This is done after boot_dload_check that way if we are in the final stage
     of an abnormal reset boot_dload_check will finalize the stage. */
  boot_secure_watchdog_init,
  
  /* Load SEC partition if it exists.  This must be done after QSEE is
     loaded as the partition is loaded into a QSEE buffer. */
  sbl1_load_sec_partition,  

  /* Set the memory barrier pointer to shared memory */
  boot_cache_set_memory_barrier,

  /*----------------------------------------------------------------------
   Put SMEM in debug state such that smem_alloc() calls will return NULL. 
   The state is changed back to normal once smem_boot_init() is called.
   This call has to be made after setting the memory barrier.
  ----------------------------------------------------------------------*/
  boot_smem_debug_init,  
    
  /* Initialize shared memory after dload to preserve logs */
  boot_smem_init,

#if !defined(FEATURE_RUMI_BOOT)
  /* Stub out for rumi build. pmic api  pm_get_power_on_status gets 
     called from below api to get power on reason */ 
  /*----------------------------------------------------------------------
   Store Power on Status in SMEM. 
   Needs to be done after PMIC and SMEM initialization
  ----------------------------------------------------------------------*/
  boot_smem_store_pon_status,
#endif  
  /*----------------------------------------------------------------------
   Store Partition Table in SMEM. 
  ----------------------------------------------------------------------*/  
  boot_flash_dev_nand_store_partition_table,  

  /*----------------------------------------------------------------------
   Store the platform id to smem
  ----------------------------------------------------------------------*/
  sbl1_hw_platform_smem,
   
  /*----------------------------------------------------------------------
   Get shared data out of the flash device module
  ----------------------------------------------------------------------*/
  boot_share_flash_data,
  
  /*----------------------------------------------------------------------
   populate the ram partition table
  ----------------------------------------------------------------------*/
  boot_populate_ram_partition_table,

  /*----------------------------------------------------------------------
   Initialize GPIO for low power configuration
  ----------------------------------------------------------------------*/
  sbl1_tlmm_init,
  
  /*-----------------------------------------------------------------------
   Calls efs cookie handling api to perform efs backup/restore
  -----------------------------------------------------------------------*/  
  sbl1_efs_handle_cookies,
    
  /*-----------------------------------------------------------------------
   APT Security Test
   ----------------------------------------------------------------------*/
  (boot_procedure_func_type)boot_apt_test,

  /* Last entry in the table. */
  NULL 
};

/*==========================================================================
        List of SBL1 procedures to execute prior to dload entry
===========================================================================*/
boot_procedure_func_type sbl1_pre_dload_procs[] =
{
  /*-----------------------------------------------------------------------
   * Initialization functions for dload. This has to be the first function 
   * called before Dload entry
   *----------------------------------------------------------------------*/
  boot_init_for_dload,
  
  /*-----------------------------------------------------------------------
   * Setup clocks for ram dump
   *----------------------------------------------------------------------*/
  sbl1_hw_dload_init,
  
#ifdef FEATURE_BOOT_SDCC_BOOT 
  /*-----------------------------------------------------------------------
   * Ram dump to eMMC raw partition, this function will reset device 
   * after successful dump collection if cookie is set 
   *----------------------------------------------------------------------*/
  (boot_procedure_func_type) boot_ram_dump_to_raw_parition,
  
#endif  
#ifdef FEATURE_BOOT_RAMDUMPS_TO_SD_CARD   
  /*----------------------------------------------------------------------
   * Take the Ramdumps to SD card if  cookie file is
   *   present in SD card 
   *---------------------------------------------------------------------*/
  boot_ram_dumps_to_sd_card,
#endif /*FEATURE_BOOT_RAMDUMPS_TO_SD_CARD*/  

  /*-----------------------------------------------------------------------
   * NULL pointer indicates the end of the list
   *-----------------------------------------------------------------------*/
  NULL
};

/*==========================================================================
                     CONFIG FUNCTION DEFINITIONS 
===========================================================================*/

/* Executing QSEE from SBL1 */ 
static void qsee_jump_func(bl_shared_data_type *bl_shared_data )
{
  boot_boolean result = FALSE;
  struct mem_block qsee_mem_block;
  sys_debug_qsee_info_type * qsee_info =
    (sys_debug_qsee_info_type *)&Image$$SYSTEM_DEBUG_QSEE_INFO$$Base;


  qsee_info->entry_point = NULL;
  qsee_info->elf_class = NULL;

  boot_log_message("QSEE Execution, Start");
  boot_log_start_timer();
  
  /* Flush the cache before entering QSEE */
  mmu_flush_cache();

  boot_log_stop_timer("QSEE Execution, Delta");

  boot_pm_vib_off();
  
  /* De-Initialize the boot logger */
  sbl1_boot_logger_deinit();
  
  /* Deinit hardware.  This must be done AFTER sbl1_boot_logger_deinit() */  
  sbl1_hw_deinit();


  if(L2_parity_error_in_sbl)
  {
    boot_dload_set_cookie();
    boot_hw_reset(BOOT_WARM_RESET_TYPE);
  }  


  /* Search image entries in QSEE interface for QSEE and get entry point.
     The entry point and elf type of QSEE must be stored so if an abnormal
     reset occurs the system debug image knows how to get to QSEE.  A small
     section has been carved out in the system debug image memory for this
     data. */
  qsee_info->entry_point =
    boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_QSEE_SW_TYPE);
  qsee_info->elf_class =
    boot_sbl_qsee_interface_get_eident(&(bl_shared_data->sbl_qsee_interface),
                                       SECBOOT_QSEE_SW_TYPE);

  /* Verify entry point is not null */
  BL_VERIFY(qsee_info->entry_point != NULL,
            BL_ERR_NULL_PTR);
  
  /* Verify QSEE ELF class is valid */
  BL_VERIFY((qsee_info->elf_class == ELF_CLASS_64) ||
            (qsee_info->elf_class == ELF_CLASS_32),
            BL_ERR_ELF_CLASS_INVALID);


  /* Mark QSEE page table as executable */
  qsee_mem_block.p_base = SCL_QSEE_CODE_BASE;
  qsee_mem_block.v_base = SCL_QSEE_CODE_BASE;
  qsee_mem_block.size_in_kbytes = SCL_QSEE_TOTAL_SIZE >> 10;
  qsee_mem_block.memory_mapping = MMU_L2_SECTION_MAPPING;
  qsee_mem_block.access = MMU_L2_PAGETABLE_MEM_READ_WRITE;
  qsee_mem_block.cachetype = MMU_L2_PAGETABLE_MEM_NON_CACHEABLE;
  qsee_mem_block.executable = MMU_L2_PAGETABLE_EXECUTABLE_REGION;
  qsee_mem_block.domain = MMU_PAGETABLE_DEF_DOMAIN;

  result =
    boot_mmu_page_table_map_single_mem_block(bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base,
                                             &qsee_mem_block);
  BL_VERIFY(result,
            BL_ERR_SBL);


  /* "Exit" bootloader and enter QSEE context.
     If QSEE is 64 bit then generate a warm reset switching processor
     mode to AARCH64.  If not then just just a function pointer. */
  if (qsee_info->elf_class == ELF_CLASS_64)
  {
    /* In Bear family all entry points must be 32 bits.  Even if the image is 
       a 64 bit elf.  This is because on Bear the boot remapper is used to set
       the entry point when the processor is switched to AARCH64.  
       The remapper only supports 32 bits.
       Verify the entry point is valid. */
    BL_VERIFY(qsee_info->entry_point <= 0xFFFFFFFF,
              BL_ERR_VALUE_TOO_LARGE);

    /* Check fuse to make sure AARCH64 is enabled */
    BL_VERIFY(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_INM(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_BMSK) > 0,
              BL_ERR_AARCH64_NOT_ENABLED);
 
    boot_switch_to_aarch64((uint32)qsee_info->entry_point,
                           (uint32)&(bl_shared_data->sbl_qsee_interface));
  }
  else
  {
    ((void (*)())(uint32)(qsee_info->entry_point))
      (&(bl_shared_data->sbl_qsee_interface)); 
  }

  return;
}


/* Conditionally cancel RPM loading in SBL1 */
static boot_boolean rpm_load_cancel(bl_shared_data_type *bl_shared_data)
{
  boot_boolean is_auth_enabled = FALSE;

  bl_error_type status = boot_is_auth_enabled(&is_auth_enabled);
  BL_VERIFY((status == BL_ERR_NONE), BL_ERR_IMG_SECURITY_FAIL); 

  /* Do not load RPM if we are in DLOAD mode and auth is disabled. 
   * This is to preserve RPM code ram for the memory debug tools */
  return (boot_boolean)(boot_dload_is_dload_mode_set() == TRUE && 
                        is_auth_enabled == FALSE);
}


/*==========================================================================
                      DEFINE TARGET BOOT CONFIG TABLE
===========================================================================*/
boot_configuration_table_entry sbl1_config_table[] = 
{

 /* SBL1 -> QSEE */
  {
    SBL1_IMG,                   /* host_img_id */
    CONFIG_IMG_QC,              /* host_img_type */
    GEN_IMG,                    /* target_img_id */
    CONFIG_IMG_ELF,             /* target_img_type */
    SECBOOT_QSEE_SW_TYPE,       /* target_img_sec_type */ 
    TRUE,                       /* load */
    TRUE,                       /* auth */
    FALSE,                      /* exec */
    FALSE,                      /* jump */
    NULL,                       /* exec_func */
    NULL,                       /* jump_func */
    load_qsee_pre_procs,        /* pre_procs */ 
    load_qsee_post_procs,       /* post_procs */
    NULL,                       /* load_cancel */
    qsee_partition_name,        /* target_img_partition_id */
    QSEE_BOOT_LOG_STR,          /* target_img_str */
    sbl1_table,                       /* whitelist table */
    FALSE,                      /* boot_ssa_enabled */
    TRUE,                       /* enable_rollback_protection*/
    FALSE,                      /* enable_xpu */
    0x0                         /* xpu_proc_id*/    
  },

  /* SBL1 -> RPM */
  {
    SBL1_IMG,                   /* host_img_id */
    CONFIG_IMG_QC,              /* host_img_type */
    GEN_IMG,                    /* target_img_id */
    CONFIG_IMG_ELF,             /* target_img_type */
    SECBOOT_RPM_FW_SW_TYPE,     /* target_img_sec_type */ 
    TRUE,                       /* load */
    TRUE,                       /* auth */
    FALSE,                      /* exec */
    FALSE,                      /* jump */
    NULL,                       /* exec_func */
    NULL,                       /* jump_func */
    NULL,                       /* pre_procs */ 
    NULL,                       /* post_procs */
    rpm_load_cancel,            /* load_cancel */
    rpm_partition_name,           /* target_img_partition_id */
    RPM_BOOT_LOG_STR,           /* target_img_str */
    sbl1_table,                       /* whitelist table */
    FALSE,                      /* boot_ssa_enabled  */
    TRUE,                       /* enable_rollback_protection */
    FALSE,                      /* enable_xpu */
    0x0                         /* xpu_proc_id*/
  },
  /* SBL1 -> APPSBL */
  {
    SBL1_IMG,                   /* host_img_id */
    CONFIG_IMG_QC,              /* host_img_type */
    GEN_IMG,                    /* target_img_id */
    CONFIG_IMG_ELF,             /* target_img_type */
    SECBOOT_APPSBL_SW_TYPE,     /* target_img_sec_type */ 
    TRUE,                       /* load */
    TRUE,                       /* auth */
    FALSE,                      /* exec */
    TRUE,                       /* jump */
    NULL,                       /* exec_func */
    qsee_jump_func,             /* jump_func */
    NULL,                       /* pre_procs */ 
    NULL,                       /* post_procs */
    NULL,                       /* load_cancel */
    appsbl_partition_name,        /* target_img_partition_id */
    APPSBL_BOOT_LOG_STR,        /* target_img_str */
    sbl1_table,                       /* whitelist table */
    FALSE,                      /* boot_ssa_enabled  */
    TRUE,                       /* enable_rollback_protection */
    FALSE,                      /* enable_xpu */
    0x0                         /* xpu_proc_id*/    
  },
  /* LAST ENTRY */
  {
    NONE_IMG,
  }

};
