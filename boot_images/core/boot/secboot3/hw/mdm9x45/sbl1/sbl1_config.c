/**
 * @file sbl1_config.c
 * @brief
 * Source file for 9x45 SBL1 bootloader configuration
 *
 * DEFINED VARIABLES:
 * sbl1_config_table - Boot configuration entries for SBL1
 * load_qsee_pre_procs - SBL1 procedures run prior QSEE loading
 * load_qsee_post_procs - SBL1 procedures run post QSEE loading
 * qsee_exec_func - SBL1 function to execute QSEE
 * load_rpm_pre_procs - SBL1 procedures run prior RPM loading
 * load_rpm_post_procs - SBL1 procedures run post RPM loading
 * load_apps_post_procs - SBL1 procedures run post APPSBL/APPS loading
 * apps_jump_func - SBL1 function to jump to APPS
 * appsbl_load_cancel - SBL1 function to cancel APPSBL loading 
 * load_mba_pre_procs - SBL1 procedures run before MBA loading
 * load_mba_post_procs - SBL1 procedures run post MBA loading
 * load_mpss_pre_procs  - SBL1 procedures run before q6/Modem image loading
 * load_mpss_post_procs  - SBL1 procedures run post q6/Modem image loading
 */

/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/25/14   jz      Update the clobber protection regions for MBA after MBA image is loaded 
10/27/14   jz      Removed sbl1_hw_set_boot_done_gpio (not needed for 9x45), cleaned ups
10/23/14   jz      Added qfprom test support in SBL
10/17/14   jz      Moved boot_secure_watchdog_init to before TZ is loaded
10/09/14   jz      Added reconstruct_loaded_mba_image in MBA post_proc
09/17/14   jz      Removed sbl1_rpm_sync
09/08/14   jz      Added support for fast USB enumeration
08/08/14   jz      Added support for eMMC boot
08/04/14   jz      Replaced BOOT_PRE_SILICON with FEATURE_RUMI_BOOT for skipping pmic call on RUMI
07/31/14   jz      Added support for MBA image loading that requires image loaded as one blob
07/24/14   jz      Cleaned up obsoleted partitions
06/30/14   jz      Moved sbl1_hw_deinit after sbl1_boot_logger_deinit
06/20/14   ck      Modified sbl1_cleanse_qsee_region and boot_dload_dump_qsee
                   for QHEE and renamed to sbl1_cleanse_security_regions and boot_dload_dump_security_regions
06/13/14   jz      Fixed appsbl_load_cancel to handle APPSBL in ELF
06/13/14   jz      Make TZ readonly and executable before execution, non-executable after
06/06/14   jz      Keep QSEE partition name '0:TZ', remove QHEE, move flash_init after ddr_init
06/06/14   ck      Updated NO_FLASH conditionals to use booting_from_storage_media
06/02/14   jz      Cleaned up qsee_exec_func
05/14/14   ck      Added calls to sbl1_cleanse_qsee_region and boot_dload_dump_qsee
04/15/14   ck      Moved boot_dload_check before loading of QSEE as debug info is in SBL 
04/10/14   jz      Updated boot flows and no QHEE for 9x45/MDM, no LPASS
03/26/14   jz      Updated and ported fix for rollback protection for non-MSA authentication.
02/24/14   jz      Updated for Bear support
11/20/13   kedara  Update wdt_load_cancel.
11/15/13   kedara  Support SDI (debug) image as an elf.
11/12/13   kedara  Added set_lpass_mpss_partitions.
10/25/13   kedara  Remove Null pointer check for apps image since 0 is apps
                   entry address
09/27/13   dhaval  fixed conditional loading of SDI, LE-boot flow updates,
                   update APPSBL partition name as 0:aboot
08/26/13   kedara  load rpm image before debug image.
07/18/13   dh      remove boot_ram_dumps_to_sd_card
06/29/13   kedara  Branch from 9x25 sbl1_config, make changes for 9x35 nand
                   & flashless boot.

============================================================================
                     Copyright 2012 - 2014 Qualcomm Technologies Incorporated.
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
#include "boot_flash_dev_nand.h"
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
#include "boot_rmb.h"
#include "boot_dload_debug.h"
#include "boot_fastcall_tz.h"
#include "boot_util.h"
#include "miprogressive.h"
#include "boot_extern_efs_interface.h"
#include "boot_elf_header.h"
#include "boot_secure_watchdog.h"
#include "sys_debug.h"
#include "boot_sahara.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define NAND_ERASE_VAL 0xFFFFFFFF

static uint8 data[PARTITION_ID_MAX_SIZE];
static struct fs_stat item_stat_buf;

/*==========================================================================
                      DEFINE PARTITION IDs
===========================================================================*/
#ifdef FEATURE_FLASHLESS_BOOT
uint8 rpm_partition_name[PARTITION_ID_MAX_SIZE] = {RPM_IMG};
uint8 apps_partition_name[PARTITION_ID_MAX_SIZE] = {APPS_IMG};
uint8 appsbl_partition_name[PARTITION_ID_MAX_SIZE] = {APPSBL_IMG};
uint8 q6_partition_name[PARTITION_ID_MAX_SIZE] = {DSP1_IMG};
uint8 qsee_partition_name[PARTITION_ID_MAX_SIZE] = {TZ_IMG};
uint8 mba_partition_name[PARTITION_ID_MAX_SIZE] = {MBA_IMG};
uint8 acdb_partition_name[PARTITION_ID_MAX_SIZE] = {ACDB_IMG};
#else
uint8 rpm_partition_name[PARTITION_ID_MAX_SIZE] = {"0:RPM"};
uint8 apps_partition_name[PARTITION_ID_MAX_SIZE] = {"0:APPS"};
uint8 appsbl_partition_name[PARTITION_ID_MAX_SIZE] = {"0:aboot"};
uint8 q6_partition_name[PARTITION_ID_MAX_SIZE] = {"0:QDSP"};
uint8 qsee_partition_name[PARTITION_ID_MAX_SIZE] = {"0:TZ"};
uint8 mba_partition_name[PARTITION_ID_MAX_SIZE] = {"0:MBA"};
uint8 acdb_partition_name[PARTITION_ID_MAX_SIZE] = {"0:ACDB"};
#endif
uint8 efs1_partition_name[PARTITION_ID_MAX_SIZE] = {RAMFS1_IMG};
uint8 efs2_partition_name[PARTITION_ID_MAX_SIZE] = {RAMFS2_IMG};
uint8 efs3_partition_name[PARTITION_ID_MAX_SIZE] = {BACKUP_RAMFS_IMG};
uint8 fota_partition_name[PARTITION_ID_MAX_SIZE] = {"0:FOTA"};

/* The following image partition IDs are in the form of GPT partition GUID (for eMMC boot) */
extern uint8 qsee_partition_id[];
extern uint8 rpm_partition_id[];
extern uint8 appsbl_partition_id[];

/*==========================================================================
                      DEFINE BOOT LOGGER STR
===========================================================================*/
#define QSEE_BOOT_LOG_STR "QSEE Image Loaded, Delta"
#define RPM_BOOT_LOG_STR "RPM Image Loaded, Delta"
#define MBA_BOOT_LOG_STR "MBA Image Loaded, Delta"
#define Q6_BOOT_LOG_STR "Q6 Image Loaded, Delta"
#define APPS_BOOT_LOG_STR "APPS Image Loaded, Delta"
#define APPSBL_BOOT_LOG_STR "APPSBL Image Loaded, Delta"
#define EFS1_BOOT_LOG_STR "EFS1 Image Loaded, Delta"
#define EFS2_BOOT_LOG_STR "EFS2 Image Loaded, Delta"
#define EFS3_BOOT_LOG_STR "EFS3 Image Loaded, Delta"
#define ACDB_BOOT_LOG_STR "ACDB Image Loaded, Delta"

/*==========================================================================
                      DEFINE WHITELIST TABLES
===========================================================================*/
static whitelist_region_type sbl1_table[] = {SBL1_WHITELIST_REGION_TABLE};
static whitelist_region_type efs1_table[] = {EFS1_WHITELIST_REGION_TABLE};
static whitelist_region_type efs2_table[] = {EFS2_WHITELIST_REGION_TABLE};
static whitelist_region_type efs3_table[] = {EFS3_WHITELIST_REGION_TABLE};
static whitelist_region_type mba_table[]  = {MBA_WHITELIST_REGION_TABLE};

boot_configuration_table_entry sbl1_config_table[];

boot_boolean is_mpss_load_cancelled = FALSE;

/* SYSTEM_DEBUG_QSEE_INFO is memory specified in the scatter file for SBL
   and system debug to share information about where QSEE was loaded and
   if it's a 32 bit or 64 bit image. */
extern uint32 Image$$SYSTEM_DEBUG_QSEE_INFO$$Base;

/*==========================================================================
                     PRE/POST PROCEDURE DEFINITIONS 
===========================================================================*/

/*===========================================================================
**  Function :  sbl1_efs1_init
** ==========================================================================
*/
/*!
* 
* @brief
*  This function performs necessary configs pertaining efs1 image
*  prior to loading the image
*
* @param[in] 
*  None
*        
* @par Dependencies
*  Called before loading efs1 image and *After* boot_dload_check
* 
*/
static void sbl1_efs1_init(void)
{
  /* Zero out efs1 address space. Required to clear out remnants of data,
    in DDR, that may have got retained across a previous load */ 
  qmemset((void* )(EFS1_RAM_BASE), 0, EFS1_RAM_SIZE);   
}

/*===========================================================================
**  Function :  sbl1_efs2_init
** ==========================================================================
*/
/*!
* 
* @brief
*  This function performs necessary configs pertaining efs2 image
*  prior to loading the image
*
* @param[in] 
*  None
*        
* @par Dependencies
*  Called before loading efs2 image and *After* boot_dload_check
* 
*/
static void sbl1_efs2_init(void)
{
  /* Zero out efs2 address space. Required to clear out remnants of data,
    in DDR, that may have got retained across a previous load */ 
  qmemset((void* )(EFS2_RAM_BASE), 0, EFS2_RAM_SIZE);   
}

/*===========================================================================
**  Function :  sbl1_efs3_init
** ==========================================================================
*/
/*!
* 
* @brief
*  This function performs necessary configs pertaining efs3 image
*  prior to loading the image
*
* @param[in] 
*  None
*        
* @par Dependencies
*  Called before loading efs3 image and *After* boot_dload_check
* 
*/
static void sbl1_efs3_init(void)
{
  /* Zero out efs3 address space. Required to clear out remnants of data,
    in DDR, that may have got retained across a previous load */ 
  qmemset((void* )(EFS3_RAM_BASE), 0, EFS3_RAM_SIZE);   
}

/*===========================================================================
**  Function :  sbl1_acdb_init
** ==========================================================================
*/
/*!
* 
* @brief
*  This function performs necessary configs pertaining acdb image
*  prior to loading the image
*
* @param[in] 
*  None
*        
* @par Dependencies
*  Called before loading acdb image and *After* boot_dload_check
* 
*/
static void sbl1_acdb_init(void)
{
  /* Zero out acdb address space. Required to clear out remnants of data,
    in DDR, that may have got retained across a previous load */ 
  qmemset((void* )(SCL_ACDB_BOOT_BASE), 0, SCL_ACDB_BOOT_SIZE);   
}

/*===========================================================================
**  Function :  sbl1_set_config_table_image_auth_status
** ==========================================================================
*/
/*!
* 
* @brief
*  This function updates the authentication flag for an image in
*  boot config table
*
* @param[in] img_sec_type Image type to locate desired image in config table
*            auth_status  Boolean status to be set.
*            rollback_status Boolean status as to whether rollback protection
*                            is to be enabled
*
*        
* @par Dependencies
*  None
*
* @retval
*   TRUE  if image entry found in config table else FALSE.
* 
*/
static boolean sbl1_set_config_table_image_auth_status
( 
  secboot_sw_type img_sec_type,
  boolean auth_status,
  boolean rollback_status  
)
{
  uint32 index = 0;
  boolean img_entry_exists = FALSE;
  
  /* Iterate over config table to find image entry and
     set authentication flag status
  */
  while (sbl1_config_table[index].host_img_id!= NULL)
  {
    if (sbl1_config_table[index].target_img_sec_type 
         == img_sec_type)
    {
      sbl1_config_table[index].auth = auth_status;
      sbl1_config_table[index].enable_rollback_protection = rollback_status;
      img_entry_exists = TRUE;
      break;
    }
    index++;
  }
  return img_entry_exists;
}

/*===========================================================================
**  Function :  sbl1_mba_msa_configure
** ==========================================================================
*/
/*!
* 
* @brief
*  This function configures the boot config entry flags for mba image
*  depending upon the MSA feature enable status in hw
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*  Called before loading mba image
* 
*/
static void sbl1_mba_msa_configure( bl_shared_data_type *bl_shared_data )
{
  /* If SSA feature is disabled sbl1 will authenticate MBA image*/
  if (FALSE == boot_is_msa_enabled_in_hw())
  {
    /* Iterate over config table to find MBA entry and
       turn on authentication
    */
    BL_VERIFY( 
      TRUE == sbl1_set_config_table_image_auth_status(
                SECBOOT_MBA_SW_TYPE,
                TRUE, 
                TRUE),
      BL_ERR_SUBSYS_BOOT_FAIL);
  }
}

/*===========================================================================
**  Function :  sbl1_mpss_msa_configure
** ==========================================================================
*/
/*!
* 
* @brief
*  This function configures the boot config entry flags for mpss image
*  depending upon the MSA feature enable status in hw
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*  Called before loading mpss image
* 
*/
static void sbl1_mpss_msa_configure( bl_shared_data_type *bl_shared_data )
{
  /* If MSA feature is disabled sbl1 will authenticate MPSS image*/
  if( FALSE == boot_is_msa_enabled_in_hw())
  {
    /* Iterate over config table to find MPSS entry and
       turn on authentication
    */
    BL_VERIFY( 
      TRUE == sbl1_set_config_table_image_auth_status(
                SECBOOT_AMSS_HASH_TABLE_SW_TYPE,
                TRUE,
                TRUE),
      BL_ERR_SUBSYS_BOOT_FAIL);
  }
}


static void set_mpss_partitions(bl_shared_data_type *bl_shared_data)
{
  int result=0;

  /* Check the MPSS partition only in case of flash based boot. Flashless
  configuration always has MPSS */
  if (booting_from_storage_media())
  {
    result = boot_efs_stat(MPSS_EFS_PATH, &item_stat_buf);
    if ((result == 0) &&
        (item_stat_buf.st_size != 0) &&
        (item_stat_buf.st_size <= FLASH_PART_NAME_LENGTH)
        )
    {  
      result = boot_efs_get(MPSS_EFS_PATH,data,item_stat_buf.st_size);
      if (result > 0 && result <= sizeof(q6_partition_name))
      {
        /* If we reached here, we could open the file and read the string.
          Copy this into partition name which will be used to open the 
          partition.
         */
        qmemset((void* )q6_partition_name, 0, sizeof(q6_partition_name));
        qmemcpy(q6_partition_name, data, result);
      }
    }
  }

}

/*===========================================================================
**  Function :  sbl1_setup_partition_ids
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

/*===========================================================================
**  Function :  reconstruct_loaded_mba_image
** ==========================================================================
*/
/*!
* 
* @brief
*  This function reconstructs the MBA image in one blob as laid out in the flash
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*  Called after MBA image is loaded and before loading mpss image
* 
*/
static void reconstruct_loaded_mba_image(bl_shared_data_type *bl_shared_data)
{
  /* Since MPSS image will be loaded after MBA image is loaded by modem PBL to Q6/TCM,
   * the MPSS region in DDR can be used for loading the MBA image then reconstructed in MBA region
   * in one blob as laid out in the flash, then its base address will be passed to modem PBL.
   */
  if (booting_from_storage_media())
  {
    boot_elf_reconstruct_elf_image(MBA_RAM_BASE, MBA_RAM_SIZE);
  }
  else
  {
    boot_sahara_reconstruct_elf_image(MBA_RAM_BASE, MBA_RAM_SIZE);
  }

  /* After the MBA image is reconstructed, update the clobber protection region 
   * since the MPSS region was used for MBA loading (portion of MPSS region was in whitelist) 
   * so needs to be removed
   */
  boot_clobber_remove_global_protection_regions((void*)mba_table[0].start_addr, mba_table[0].size);
  boot_clobber_add_global_protection_region((void*)MBA_RAM_BASE, MBA_RAM_SIZE);
}
	

/*==========================================================================

                  FUNCTIONS TO EXECUTE BEFORE TZ LOADING

===========================================================================*/
boot_procedure_func_type load_qsee_pre_procs[] = 
{

  /*----------------------------------------------------------------------
   Save reset register logs
  ----------------------------------------------------------------------*/
  boot_save_reset_register_log,
  
  /*----------------------------------------------------------------------
   Update the partition IDs to be used for GUID based boot targets
  ----------------------------------------------------------------------*/  
  sbl1_update_partition_ids,
  
  /*-----------------------------------------------------------------------
   Copy the configure data table from eeprom
   NOTE: the eeprom CDT does the eeprom_init, so no dependency on flash_init
  -----------------------------------------------------------------------*/
  boot_config_data_table_init,
  
  /*-----------------------------------------------------------------------
   Store platform id
  -----------------------------------------------------------------------*/
  sbl1_hw_platform_pre_ddr,
  
  /*-----------------------------------------------------------------------
   Configure ddr parameters based on eeprom CDT table data.
  -----------------------------------------------------------------------*/
  sbl1_ddr_set_params,
  
  /*----------------------------------------------------------------------
   Initialize DDR 
  ----------------------------------------------------------------------*/
  (boot_procedure_func_type)sbl1_ddr_init,

  /*----------------------------------------------------------------------
   Initialize SBL1 DDR ZI region, relocate page table to DDR
  ----------------------------------------------------------------------*/
  sbl1_post_ddr_init, 

  sbl1_hw_init_secondary,  
  
  /*----------------------------------------------------------------------
   Initialize modules post DDR init
  ----------------------------------------------------------------------*/  
  sbl1_post_ddr_hw_init,
  
  /*----------------------------------------------------------------------
   Initialize the flash device
   NOTE: NAND flash init depends on DDR to be initialized for using the heap
  ----------------------------------------------------------------------*/
  boot_flash_init,
  
#ifdef FEATURE_QUSB_FAST_ENUMERATION
  /*----------------------------------------------------------------------
   Do fast USB enumeration
  ----------------------------------------------------------------------*/ 
  sbl1_qusb_al_do_fast_enum,
#endif
   
  /*----------------------------------------------------------------------
    EFS Initialize for EFS-Lite
  ----------------------------------------------------------------------*/  
  (boot_procedure_func_type)boot_efs_read_only_initialize,

  /*----------------------------------------------------------------------
    Zero out QSEE and QHEE region if needed.  This MUST be done before
    boot_dload_dump_security_regions executes for security reasons.
  ----------------------------------------------------------------------*/
  sbl1_cleanse_security_regions,

  /* Backup QSEE and QHEE region for ramdumps taken after SBL has executed */
  boot_dload_dump_security_regions,
  
  /* Initialization of secure watchdog reset */
  boot_secure_watchdog_init,

  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};


/*==========================================================================

                  FUNCTIONS TO EXECUTE AFTER TZ LOADING

===========================================================================*/
boot_procedure_func_type load_qsee_post_procs[] = 
{  
  /*----------------------------------------------------------------------
   Check to see if DLOAD mode needs to be entered
  ----------------------------------------------------------------------*/
  boot_dload_check,
  
  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};


/*==========================================================================

                  FUNCTIONS TO EXECUTE BEFORE RPM LOADING

===========================================================================*/
boot_procedure_func_type load_rpm_pre_procs[] = 
{
  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};

/*==========================================================================

                  FUNCTIONS TO EXECUTE AFTER RPM LOADING

===========================================================================*/
boot_procedure_func_type load_rpm_post_procs[] = 
{
  /*----------------------------------------------------------------------
   Set the memory barrier pointer to shared memory
  ----------------------------------------------------------------------*/
  boot_cache_set_memory_barrier,

  /*----------------------------------------------------------------------
   Put SMEM in debug state such that smem_alloc() calls will return NULL. 
   The state is changed back to normal once smem_boot_init() is called.
   This call has to be made after setting the memory barrier.
  ----------------------------------------------------------------------*/
  boot_smem_debug_init,  
    
  /*----------------------------------------------------------------------
   Initialize shared memory after dload to preserve logs
  ----------------------------------------------------------------------*/
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
   Initialize GPIO for low power configuration
  ----------------------------------------------------------------------*/
  sbl1_tlmm_init,

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

  /*-----------------------------------------------------------------------
   Load SSD partition 
  ------------------------------------------------------------------------*/
  boot_load_ssd_partition,
  
  /*----------------------------------------------------------------------
   Bring RPM processor out of reset. 
  ----------------------------------------------------------------------*/
   sbl1_release_rpm_proc,

  /*----------------------------------------------------------------------
   Enable JTAG to RPM processor
  ----------------------------------------------------------------------*/
  sbl1_hw_jtag_enable_rpm,  

  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};


/*==========================================================================

                  FUNCTIONS TO EXECUTE BEFORE EFS1 LOADING

===========================================================================*/
boot_procedure_func_type load_efs1_pre_procs[] = 
{
  /*----------------------------------------------------------------------
   Zero out efs1 address space
  ----------------------------------------------------------------------*/
  (boot_procedure_func_type)sbl1_efs1_init,
  
  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};

/*==========================================================================

                  FUNCTIONS TO EXECUTE BEFORE EFS2 LOADING

===========================================================================*/
boot_procedure_func_type load_efs2_pre_procs[] = 
{
  /*----------------------------------------------------------------------
   Zero out efs2 address space
  ----------------------------------------------------------------------*/
  (boot_procedure_func_type)sbl1_efs2_init,

  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};

/*==========================================================================

                  FUNCTIONS TO EXECUTE BEFORE EFS3 LOADING

===========================================================================*/
boot_procedure_func_type load_efs3_pre_procs[] = 
{
  /*----------------------------------------------------------------------
   Zero out efs3 address space
  ----------------------------------------------------------------------*/
  (boot_procedure_func_type)sbl1_efs3_init,
  
  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};

/*==========================================================================

                  FUNCTIONS TO EXECUTE AFTER APPSBL/APPS LOADING

===========================================================================*/
boot_procedure_func_type load_apps_post_procs[] = 
{    
  /*-----------------------------------------------------------------------
   Calls efs cookie handling api to perform efs backup/restore
  -----------------------------------------------------------------------*/  
  sbl1_efs_handle_cookies,

  /*----------------------------------------------------------------------
   Blow sbl1 version roll back protection fuse after rpm sync
  ----------------------------------------------------------------------*/  
  sbl1_update_sbl1_rollback_fuse,

 /*----------------------------------------------------------------------
   Update version number rollback protection fuses for all images
  ----------------------------------------------------------------------*/
  (boot_procedure_func_type) sbl1_update_all_rollback_fuses,
  
  /*----------------------------------------------------------------------
   Invokes qfprom test framework
  ----------------------------------------------------------------------*/  
  sbl1_qfprom_test,
  
  /*----------------------------------------------------------------------
   Enable JTAG to APPS processor
  ----------------------------------------------------------------------*/  
  sbl1_hw_jtag_enable_apps,
  
  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};


/*==========================================================================

                  FUNCTIONS TO EXECUTE BEFORE ACDB LOADING

===========================================================================*/
boot_procedure_func_type load_acdb_pre_procs[] = 
{
  /*----------------------------------------------------------------------
   Zero out acdb address space
  ----------------------------------------------------------------------*/
  (boot_procedure_func_type)sbl1_acdb_init,
  
  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};

/*==========================================================================

                  FUNCTIONS TO EXECUTE BEFORE MBA LOADING

===========================================================================*/
boot_procedure_func_type load_mba_pre_procs[] = 
{
  /*------------------------------------------------------------------
   Configure Image auth and MSA enable flags. 
  ------------------------------------------------------------------*/
  sbl1_mba_msa_configure,
  
  /*------------------------------------------------------------------
   Configure partitions for MPSS
  ------------------------------------------------------------------*/
  set_mpss_partitions,
  
  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};

/*==========================================================================

                  FUNCTIONS TO EXECUTE AFTER MBA LOADING

===========================================================================*/
boot_procedure_func_type load_mba_post_procs[] = 
{
  /*------------------------------------------------------------------
   Reconstruct the loaded ELF image as a blob in DDR as laid out in flash
  ------------------------------------------------------------------*/
  reconstruct_loaded_mba_image,

  /*------------------------------------------------------------------
   Bring modem processor out of reset. 
  ------------------------------------------------------------------*/
  sbl1_release_modem_proc,
  
  /*----------------------------------------------------------------------
   Enable JTAG to Modem processor
  ----------------------------------------------------------------------*/  
  sbl1_hw_jtag_enable_modem,
  
  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};

/*==========================================================================

                  FUNCTIONS TO EXECUTE BEFORE Q6 /MODEM LOADING
  
===========================================================================*/
boot_procedure_func_type load_mpss_pre_procs[] = 
{
  /*------------------------------------------------------------------
   Configure Image auth and MSA enable flags. 
  ------------------------------------------------------------------*/
  sbl1_mpss_msa_configure,

  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
  NULL 
};

/*==========================================================================

                  FUNCTIONS TO EXECUTE AFTER Q6 /MODEM LOADING
  
===========================================================================*/
boot_procedure_func_type load_mpss_post_procs[] = 
{
  /*----------------------------------------------------------------------
   Last entry in the table.
  ----------------------------------------------------------------------*/
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

  /*-----------------------------------------------------------------------
   * NULL pointer indicates the end of the list
   *-----------------------------------------------------------------------*/
  NULL
};


/*==========================================================================
                     CONFIG FUNCTION DEFINITIONS 
===========================================================================*/
/* Executing QSEE from SBL1 (flashless case) */ 
static void qsee_exec_func(bl_shared_data_type *bl_shared_data )
{
  boot_boolean result = TRUE;
  sys_debug_qsee_info_type * qsee_info =
    (sys_debug_qsee_info_type *)&Image$$SYSTEM_DEBUG_QSEE_INFO$$Base;

  struct mem_block qsee_mem_block = 
  {
    SCL_QSEE_CODE_BASE, SCL_QSEE_CODE_BASE, SCL_QSEE_TOTAL_SIZE>>10,
    MMU_L2_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_DEVICE_SHARED, MMU_L2_PAGETABLE_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN
  };

  /* Update QSEE memory region as executable and read write before executing it */
  result = boot_mmu_page_table_map_single_mem_block
              (bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base, &qsee_mem_block); 
  
  BL_VERIFY(result, BL_ERR_SBL);

  boot_log_message("QSEE Execution, Start");
  boot_log_start_timer();
  
  /* Search image entries in QSEE interface for QSEE and get entry point.
     The entry point and elf type of QSEE must be stored so if an abnormal
     reset occurs the system debug image knows how to get to QSEE.  A small
     section has been carved out in the system debug image memory for this
     data. */
  qsee_info->entry_point =
    boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_QSEE_SW_TYPE);
  /* Verify entry point is not null */
  BL_VERIFY(qsee_info->entry_point != NULL,
            BL_ERR_NULL_PTR);
  
  /* Flush the cache before entering QSEE */
  mmu_flush_cache();
  
  /* "Exit" bootloader and enter QSEE context */
  ((void (*)())(uint32)(qsee_info->entry_point))
    (&(bl_shared_data->sbl_qsee_interface)); 

  /* Update QSEE memory region as read only and non executable after executing it */
  qsee_mem_block.access = MMU_L2_PAGETABLE_MEM_READ_ONLY;
  qsee_mem_block.executable = MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION;
  result = boot_mmu_page_table_map_single_mem_block
              (bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base, &qsee_mem_block); 
  
  BL_VERIFY(result, BL_ERR_SBL);
  
  /*Set flag to indicate QSEE is ready */
  boot_set_qsee_execute_status(TRUE);

  boot_log_stop_timer("QSEE Execution, Delta");

}


/* Conditionally cancel RPM loading in SBL1 */
static boot_boolean rpm_load_cancel(bl_shared_data_type *bl_shared_data)
{
  if (booting_from_storage_media())
  {
    boot_boolean is_auth_enabled = FALSE;
    bl_error_type status = boot_is_auth_enabled(&is_auth_enabled);
    BL_VERIFY((status == BL_ERR_NONE ), BL_ERR_IMG_SECURITY_FAIL); 
	
    /* Do not load RPM if we are in DLOAD mode and auth is disabled. 
    * This is to preserve RPM code ram for the memory debug tools */
    return (boot_boolean)(boot_dload_is_dload_mode_set() == TRUE && 
                          is_auth_enabled == FALSE);
  }
  else
  {
    /* Load RPM in case of Flashless boot as the correct mode would 
    be detected by Sahara and we can continue boot-up after dump
    collection */
    return (boot_boolean)FALSE;
  }
}

/* Conditionally cancel MBA loading in SBL1 */
boot_boolean mba_load_cancel(bl_shared_data_type *bl_shared_data)
{
  boot_boolean cancel = FALSE;  
  is_mpss_load_cancelled = FALSE;
  
  /* Check the mpss partitions only in case of flash based boot. Flashless
	 configuration always has mpss loading */
  if (booting_from_storage_media())
  {
    /* Do not load mpss if FOTA recovery cookie is present */
    cancel = boot_check_fota_partition(fota_partition_name);
  }
  
  if(cancel == TRUE)
  {
    is_mpss_load_cancelled = TRUE;
  }
  
  return cancel;
}


/* Conditionally cancel modem/Q6 loading in SBL1 */
boot_boolean mpss_load_cancel(bl_shared_data_type *bl_shared_data)
{
  boot_boolean cancel = FALSE;  
  
  /* Check the mpss partitions only in case of flash based boot. Flashless
    configuration always has mpss loading 
  */
  if (booting_from_storage_media())
  {
    /* Do not load mpss if FOTA recovery cookie is present */
    cancel = boot_check_fota_partition(fota_partition_name);
  }
  
  return cancel;
}

/* Conditionally cancel APPSBL loading in SBL1 */
boot_boolean appsbl_load_cancel(bl_shared_data_type *bl_shared_data)
{
  boot_flash_trans_if_type *trans_if;    
  boot_boolean cancel = TRUE;
  uint32 index = 0;

  /* Check the appsbl partition only in case of flash based boot. Flashless
     configuration does not have appsbl */
  if (booting_from_storage_media())
  {
    /* Check the APPSBL partition existance */
    boot_flash_configure_target_image(appsbl_partition_name);
    trans_if = boot_flash_dev_open_image(GEN_IMG);
    if (trans_if != NULL)
    {
      boot_flash_dev_close_image( &trans_if );
	  
      /* Validate the ELF header and load the image only if the header is valid */
      if (boot_elf_verify_image_header(appsbl_partition_name, NULL))
      {
        /* Partition is flashed */
        cancel = FALSE;
      }
    }
  }

  /* stub out load_apps_post_procs if appsbl absent */
  if (cancel)
  {
    /* Iterate over config table to find appsbl entry */
    while (sbl1_config_table[index].host_img_id!= NULL)
    {
      if(sbl1_config_table[index].target_img_sec_type 
          == SECBOOT_APPSBL_SW_TYPE)
      {
        sbl1_config_table[index].post_procs = NULL;
        break;
      }
      index++;
    }
  }

  return cancel;
}


/* Conditionally cancel EFS/ACDB loading in SBL1 */
boot_boolean efs_load_cancel(bl_shared_data_type *bl_shared_data)
{
  boot_boolean cancel = TRUE;

  /* In case of Fusion flashless boot, we need to load the EFS image */
  if (!booting_from_storage_media())
  {  
    cancel = FALSE;
  }

  return cancel;
}

/* Conditionally cancel ACDB loading in SBL1 */
boot_boolean acdb_load_cancel(bl_shared_data_type *bl_shared_data)
{
  boot_flash_trans_if_type *trans_if;    
  boot_boolean cancel = TRUE;
  mi_boot_image_header_type acdb_header;
  
  /* We load the ACDB image if the partition and image exists*/
  if (booting_from_storage_media())
  {
    boot_flash_configure_target_image(acdb_partition_name);
    trans_if = boot_flash_dev_open_image(GEN_IMG);
    if (trans_if != NULL)
    {
      boot_flash_dev_close_image( &trans_if );
      /* We check the header of ACDB, only load ACDB if header is valid.
       if image is not flashed in partition header will have
       all NAND_ERASE_VAL. Or it may also be forced to have 0 val.
      */
      boot_load_image_header(GEN_IMG, &acdb_header);
      if (acdb_header.image_dest_ptr != NULL && 
          acdb_header.image_size != 0 &&
          acdb_header.image_dest_ptr != (uint8 *)NAND_ERASE_VAL && 
          acdb_header.image_size != NAND_ERASE_VAL)
      {
        cancel = FALSE;
      }
    }
  }
  else
  {
    cancel = FALSE;
  }

  return cancel;
}

/* Jump to APPS from SBL1 */
void apps_jump_func(bl_shared_data_type *bl_shared_data )
{
  /* Write the OVERRIDE register */
  sbl1_hw_write_override_register();
  
  /* Flush cache before end of boot */
  mmu_flush_cache();

  /* Check if modem image was successfully loaded */
  sbl1_check_modem_status();

  /* De-Initialize the boot logger */
  sbl1_boot_logger_deinit();

  /* Deinit hardware.  This must be done AFTER sbl1_boot_logger_deinit() */  
  sbl1_hw_deinit();

  /* Call into tz to signal SBL1 is done , TZ will jump to APPSBL/APPS */
  sbl1_signal_qsee_sbl_done(bl_shared_data);
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
    TRUE,                       /* exec */
    FALSE,                      /* jump */
    qsee_exec_func,             /* exec_func */
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
    load_rpm_pre_procs,         /* pre_procs */ 
    load_rpm_post_procs,        /* post_procs */
    rpm_load_cancel,            /* load_cancel */
    rpm_partition_name,         /* target_img_partition_id */
    RPM_BOOT_LOG_STR,           /* target_img_str */
    sbl1_table,                 /* whitelist table */
    FALSE,                      /* boot_ssa_enabled  */
    TRUE,                       /* enable_rollback_protection */
    FALSE,                      /* enable_xpu */
    0x0                         /* xpu_proc_id*/
  },

  /* SBL1 -> EFS1 */
  {
    SBL1_IMG,                           /* host_img_id */
    CONFIG_IMG_QC,                      /* host_img_type */
    GEN_IMG,                            /* target_img_id */
    CONFIG_IMG_QC,                      /* target_img_type */
    SECBOOT_MAX_SW_TYPE,                /* target_img_sec_type */ 
    TRUE,                               /* load */
    FALSE,                              /* auth */
    FALSE,                              /* exec */
    FALSE,                              /* jump */
    NULL,                               /* exec_func */
    NULL,                               /* jump_func */
    load_efs1_pre_procs,                /* pre_procs */ 
    NULL,                               /* post_procs */
    efs_load_cancel,                    /* load_cancel */
    efs1_partition_name,                /* target_img_partition_id */
    EFS1_BOOT_LOG_STR,                  /* target_img_str */	
    efs1_table,                         /* whitelist table */
    FALSE,                              /* boot_ssa_enabled */
    FALSE,                              /* enable_rollback_protection */
    FALSE,                              /* enable_xpu */
    0x0                                 /* xpu_proc_id*/
  },

  /* SBL1 -> EFS2 */
  {
    SBL1_IMG,                           /* host_img_id */
    CONFIG_IMG_QC,                      /* host_img_type */
    GEN_IMG,                            /* target_img_id */
    CONFIG_IMG_QC,                      /* target_img_type */
    SECBOOT_MAX_SW_TYPE,                /* target_img_sec_type */ 
    TRUE,                               /* load */
    FALSE,                              /* auth */
    FALSE,                              /* exec */
    FALSE,                              /* jump */
    NULL,                               /* exec_func */
    NULL,                               /* jump_func */
    load_efs2_pre_procs,                /* pre_procs */ 
    NULL,                               /* post_procs */
    efs_load_cancel,                    /* load_cancel */
    efs2_partition_name,                /* target_img_partition_id */
    EFS2_BOOT_LOG_STR,                  /* target_img_str */
    efs2_table,                         /* whitelist table */
    FALSE,                              /* boot_ssa_enabled */
    FALSE,                              /* enable_rollback_protection */
    FALSE,                              /* enable_xpu */
    0x0                                 /* xpu_proc_id*/
  }, 
  
  /* SBL1 -> EFS3 */
  {
    SBL1_IMG,                           /* host_img_id */
    CONFIG_IMG_QC,                      /* host_img_type */
    GEN_IMG,                            /* target_img_id */
    CONFIG_IMG_QC,                      /* target_img_type */
    SECBOOT_MAX_SW_TYPE,                /* target_img_sec_type */ 
    TRUE,                               /* load */
    FALSE,                              /* auth */
    FALSE,                              /* exec */
    FALSE,                              /* jump */
    NULL,                               /* exec_func */
    NULL,                               /* jump_func */
    load_efs3_pre_procs,                /* pre_procs */ 
    NULL,                               /* post_procs */
    efs_load_cancel,                    /* load_cancel */
    efs3_partition_name,                /* target_img_partition_id */
    EFS3_BOOT_LOG_STR,                  /* target_img_str */
    efs3_table,                         /* whitelist table */
    FALSE,                              /* boot_ssa_enabled */
    FALSE,                              /* enable_rollback_protection*/
    FALSE,                              /* enable_xpu */
    0x0                                 /* xpu_proc_id*/
  },   

  /* SBL1 -> APPSBL */
  {
    SBL1_IMG,                          /* host_img_id */
    CONFIG_IMG_QC,                     /* host_img_type */
    GEN_IMG,                           /* target_img_id */
    CONFIG_IMG_ELF,                    /* target_img_type */
    SECBOOT_APPSBL_SW_TYPE,            /* target_img_sec_type */ 
    TRUE,                              /* load */
    TRUE,                              /* auth */
    FALSE,                             /* exec */
    TRUE,                              /* jump */
    NULL,                              /* exec_func */
    apps_jump_func,                    /* jump_func */
    NULL,                              /* pre_procs */ 
    load_apps_post_procs,                              /* post_procs */
    appsbl_load_cancel,                              /* load_cancel */
    appsbl_partition_name,             /* target_img_partition_id */
    APPSBL_BOOT_LOG_STR,               /* target_img_str */
    sbl1_table,                        /* whitelist table */
    FALSE,                             /* boot_ssa_enabled */
    TRUE,                              /* enable_rollback_protection */
    FALSE,                             /* enable_xpu */
    0x0                                /* xpu_proc_id*/
  },                                  
                                    
  
  /* SBL1 -> ACDB */
  {
    SBL1_IMG,                           /* host_img_id */
    CONFIG_IMG_QC,                      /* host_img_type */
    GEN_IMG,                            /* target_img_id */
    CONFIG_IMG_QC,                      /* target_img_type */
    SECBOOT_MAX_SW_TYPE,                /* target_img_sec_type */ 
    TRUE,                               /* load */
    FALSE,                              /* auth */
    FALSE,                              /* exec */
    FALSE,                              /* jump */
    NULL,                               /* exec_func */
    NULL,                               /* jump_func */
    load_acdb_pre_procs,                /* pre_procs */ 
    NULL,                               /* post_procs */
    acdb_load_cancel,                   /* load_cancel */
    acdb_partition_name,                /* target_img_partition_id */
    ACDB_BOOT_LOG_STR,                  /* target_img_str */
    sbl1_table,                         /* whitelist table */
    FALSE,                              /* boot_ssa_enabled */
    FALSE,                              /* enable_rollback_protection */
    FALSE,                              /* enable_xpu */
    0x0                                 /* xpu_proc_id*/
  },

  /* SBL1 -> MBA */
  {
    SBL1_IMG,                   /* host_img_id */
    CONFIG_IMG_QC,              /* host_img_type */
    GEN_IMG,                    /* target_img_id */
    CONFIG_IMG_ELF,             /* target_img_type */
    SECBOOT_MBA_SW_TYPE,        /* target_img_sec_type */
    TRUE,                       /* load */
    FALSE,                      /* auth */
    FALSE,                      /* exec */
    FALSE,                      /* jump */
    NULL,                       /* exec_func */
    NULL,                       /* jump_func */
    load_mba_pre_procs,         /* pre_procs */ 
    load_mba_post_procs,        /* post_procs */
    mba_load_cancel,            /* load_cancel */
    mba_partition_name,         /* target_img_partition_id */
    MBA_BOOT_LOG_STR,           /* target_img_str */
    mba_table,                  /* whitelist table */
    FALSE,                      /* boot_ssa_enabled */
    FALSE,                      /* enable_rollback_protection*/
    FALSE,                      /* enable_xpu */
    0x0                         /* xpu_proc_id*/
  },  

  /* SBL1 -> Q6 /Modem */
  {
    SBL1_IMG,                           /* host_img_id */
    CONFIG_IMG_QC,                      /* host_img_type */
    GEN_IMG,                            /* target_img_id */
    CONFIG_IMG_ELF,                     /* target_img_type */
    SECBOOT_AMSS_HASH_TABLE_SW_TYPE,    /* target_img_sec_type */ 
    TRUE,                               /* load */
    FALSE,                              /* auth */
    FALSE,                              /* exec */
    FALSE,                              /* jump */
    NULL,                               /* exec_func */
    NULL,                               /* jump_func */
    load_mpss_pre_procs,                /* pre_procs */ 
    load_mpss_post_procs,               /* post_procs */
    mpss_load_cancel,                   /* load_cancel */
    q6_partition_name,                  /* target_img_partition_id */
    Q6_BOOT_LOG_STR,                    /* target_img_str */	
    sbl1_table,                         /* whitelist table */
    TRUE,                               /* boot_ssa_enabled */
    FALSE,                              /* enable_rollback_protection*/
    FALSE,                              /* enable_xpu */
    0x0                                 /* xpu_proc_id*/
  },  

  /* SBL1 -> APPS */                  
  {
    SBL1_IMG,                          /* host_img_id */
    CONFIG_IMG_QC,                     /* host_img_type */
    GEN_IMG,                           /* target_img_id */
    CONFIG_IMG_ELF,                    /* target_img_type */
    SECBOOT_APPS_HASH_TABLE_TYPE,      /* target_img_sec_type */ 
    TRUE,                              /* load */
    TRUE,                              /* auth */
    FALSE,                             /* exec */
    TRUE,                              /* jump */
    NULL,                              /* exec_func */
    apps_jump_func,                    /* jump_func */
    NULL,                              /* pre_procs */ 
    load_apps_post_procs,                              /* post_procs */
    NULL,                              /* load_cancel */
    apps_partition_name,               /* target_img_partition_id */
    APPS_BOOT_LOG_STR,                 /* target_img_str */	
    sbl1_table,                        /* whitelist table */
    FALSE,                             /* boot_ssa_enabled */
    FALSE,                             /* enable_rollback_protection*/
    FALSE,                             /* enable_xpu */
    0x0                                /* xpu_proc_id*/
  },

  /* LAST ENTRY */
  {
    NONE_IMG,
  }
};


