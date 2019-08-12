/*=============================================================================

                              SBL1 Main Control

GENERAL DESCRIPTION
  This file contains the main control for SBL1 execution. Since this implements
  the main control functionality, it may be dependent on all modules of the
  system.

Copyright 2014-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
04/01/15     aus     Initialize DDR heap before device programmer execution
02/25/15     tj      Set build type based on compile flags
02/11/15     sc      Changes to zero initialize usb related structures
12/17/14     sve	 Code changes for merging firehose DeviceProgrammer into SBL
12/05/14	 sve	 Removed sbl1_hw_init call from sbl1_main_ctl and placed it in qsee_preprocs 
10/30/14     aus     Remove added local clobber holes after reading from flash
07/22/14     sk      changes for 8909
07/14/14     sk      Enabled external abort in SBL1
07/01/14     sk      Added sbl_error_handler function and BOOT_CRASH_DUMP_ZONE region
07/01/14     yps     Add platform informatino into sbl1 log
06/20/14     ck      Modified and renamed sbl1_cleanse_qsee_region to sbl1_cleanse_security_regions
06/03/14     ck      Added flash statistic entry when deinitializing boot logger
05/28/14     ck      Added PBL timestamp milestones to sbl1_boot_logger_init
05/14/14     ck      Added sbl1_cleanse_qsee_region
04/11/14     ck      Added SMEM and OCIMEM to boot clobber memory protection
04/08/14     ck      Set mmu_pagetable_size in bl_shared_data after moving PT's to DDR
04/08/14     ck      Added call to boot_l2_page_table_init in setup of initial memory map
04/08/14     ck      Added logic to copy page table size from PBL into bl_shared_data
04/02/14     ck      Updated use of boot_sbl_qsee_interface as it changed per Bear BAS 2.0
04/01/14     ck      Updated sbl1_retrieve_shared_info_from_pbl as Bear PBL now uses common secboot
03/25/14     jz      Ported placeholder for sbl_save_regs
03/21/14     ck      Added support to load SEC partition
03/18/14     ck      Updated boot_hw_reset calls as they now take a reset type parameter
03/14/14     ck      Ported fix to move boot_busywait_init before boot_ logger initialization
03/06/14     ck      Added sbl1_populate_initial_mem_map to sbl1_main_ctl to get execution
                     in RPM Code RAM early
01/08/14     ck      Changed SEGMENT #2 references to SYSTEM_DEBUG
01/03/14     ck      Added hole in boot_clobber memory region for DDR training partition.
01/03/14     ck      Added DDR and Segment #2 region to boot_clobber memory region.
12/16/13     ck      Removed sbl1_qfprom_test as TZ runs after SBL in Bear family.
12/13/13     ck      Removed boot_rollback_update_img_version call to update SBL version.
12/09/13     ck      Removed sbl1_update_sbl1_rollback_fuse and sbl1_update_all_rollback_fuses
                     as all fuse blowing is done after SBL in QSEE for Bear family
12/09/13     ck      Removed sbl1_update_sbl1_rollback_fuse as all fuse blowing is done after SBL in QSEE
11/21/13     ck      Removed sbl1_signal_tz_sbl_done
10/22/13     ck      Removed call to DISABLE_REMAPPER because it is not needed in 8916
10/17/13     ck      Removed sbl1_rpm_sync() because in Bear family RPM is
                     not executed until after SBL1 has run.
10/17/13     ck      Removed sbl1_check_l2_parity_error() as this was used in
                     Krait processors.  8916 is A53.
10/17/13     ck      Removed sbl1_update_shared_imem_base() as the alternative
                     IMEM location was a 8974v1 thing.  Not needed in 8916.
10/17/13     ck      Removed sbl1_pagetable_dep_enable() as the stack and
                     pagetable area has changed.  The entire MMU initialization
                     needs to be overhauled and will be before 8916 release.
08/06/13     aus     Added support for new secboot_verified_info structure
07/10/13     aus     Added placeholder for sbl_error_handler
06/26/13     aus     Remove sbl1_dead_battery_charge
06/13/13     dh      Update sbl1_load_ddr_training_data_from_partition to take buffer
                     address as argument
04/04/13     dh      Move boot_DALSYS_InitMod to early sbl1_main_ctl
04/03/13     dh      Use boot_dload_is_dload_mode_set instead of uefi dump cookie
                     to determin if device is in dload mode
04/11/13     jz      Disable remapper
04/03/13     dh      Add boot_init_stack_chk_canary in the beginning of sbl1_main_ctl
03/28/13     dhaval  set hash algorithm to be used during elf hashing in SBL
03/26/13     dh      Rename sbl1_relocate_page_table to sbl1_relocate_page_table_to_ddr
03/21/13     dh      Call sbl1_move_boot_log_to_ddr inside sbl1_relocate_page_table
                     to ensure we don't log in ddr before training is done
03/14/13     jz      Use mmu_set_dacr to configure DACR for client access
03/05/13     dh      Use mmu_l1_pagetable_base in sbl_shared_data as a common
                     pointer to page table base
02/28/13     jz      Moved page table settings out to boot_mmu_table.h
02/27/13     dh      If DDR training is not required OR we are in dload mode 
                     relocate page table to ddr and map all the regions we need
02/15/13     jz      Fixed bootup issue after TZ execution due to missing page table
02/14/13     kedara  Added sbl1_update_all_rollback_fuses.
02/13/13     dh      Add boot_secboot_ftbl_init
02/07/13     dh      Refactor sbl1 page table api
01/28/13     dh      Map sbl1 code section as read only 
12/06/12     dh      Change SHARED_IMEM_TPM_HASH_REGION_BASE to SHARED_IMEM_TPM_HASH_REGION_OFFSET
12/05/12     dh      if BOOT_ENTER_PBL_DLOAD_ON_SBL_ERROR is defined do not enter dload mode
12/04/12     dh      Move shared imem to OCIMEM for 8974v2
12/03/12     dh      Remove sbl1_rollback_version_update, add sbl1_update_sbl1_rollback_fuse
11/27/12     dh      Add sbl1_rpm_sync
11/19/12     dh      Store sbl1's hash info passed by pbl to shared imem
11/28/12     dh      Add roll back version protection logic for sbl1
11/16/12     dh      Check for L2 cache parity error in beginning of sbl1, if error
                     exists set dload cookie and reset after pmic device is configured
                     Move boot_shared_imem_init in beginning of sbl1_main_ctl
10/24/12     dh      Add boot_internal_heap and boot_external_heap, call
                     boot_DALSYS_HeapInit to initialize DAL heaps
10/09/12     dh      Add sbl1_debug_mode_enter to enable cookie based debugging
09/21/12     dh      Add boot_clock_debug_init as the first thing in sbl1_main_ctl
09/13/12     dh      Set boot logger's reference time to 0 
                     since mpm sleep clock resets after sys.u
08/30/12     dh      Relocate boot log to DDR once DDR is up
08/20/12     dh      Add sbl1_update_shared_imem_base to move shared imem base
                     to rpm msg ram for 8974v1
08/16/12     dh      Move sbl1_retrieve_shared_info_from_pbl in front of sbl1_hw_init
07/19/12     dh      Add sbl1_qfprom_test to call qfprom test framework
07/16/12     dh      Call sbl1_hw_init in early sbl1_main_ctl
06/07/12     dh      Zero initialize DLOAD buffer region
05/30/12     dh      Update boot logger to use rpm pbl start time as reference time
05/29/12     dh      Add sbl1_tlmm_init
05/24/12     dh      combine sbl1_ddr_data_init and sbl1_relocate_page_table into
                     sbl1_post_ddr_data_init
05/08/12     dh      Add sbl1_dead_battery_charge
05/03/12     dh      Remove hw_smem_init, it's integrated in 
                     DalPlatformInfo_CfgFromMem now
04/18/12     dh      passing appsbl entry address in tz milestone call as tz will
                     jump to appsbl.
03/15/12     dh      Added sbl1_populate_initial_mem_map and sbl1_relocate_page_table
12/01/11     dh      Added sbl1_rollback_version_update and sbl1_signal_tz_sbl_done.
08/26/11     dh      Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include <stdio.h>
#include BOOT_PBL_H
#include "boot_sbl_if.h"
#include "boot_logger.h"
#include "boot_target.h"
#include "boot_dload.h"
#include "boot_util.h"
#include "boot_cache_mmu.h"
#include "boot_config.h"
#include "boot_ram_init.h"
#include "boot_shared_imem_cookie.h"
#include "boot_ddr_info.h"
#include "boot_config_data.h"
#include "boot_clobber_prot.h"
#include "boot_authenticator.h"
#include "boot_extern_dal_interface.h"
#include "boot_extern_platforminfo_interface.h"
#include "boot_extern_clk_interface.h"
#include "boot_extern_tlmm_interface.h"
#include "boot_extern_ddr_interface.h"
#include "boot_extern_deviceprogrammer_lite_interface.h"
#include "boot_extern_hotplug_interface.h"
#include "boot_extern_efs_interface.h"
#include "boot_extern_usb_interface.h"
#include "boot_extern_busywait_interface.h"
#include "boot_extern_seccfg_interface.h"
#include "sbl1_target.h"
#include "boot_page_table_armv7.h"
#include "boot_mmu_table.h"
#include "sbl1_hw.h"
#include "boot_flash_dev_if.h"
#include "boot_sdcc.h"
#include "sbl1_mc.h"
#include "boothw_target.h"
#include "boot_loader.h"
#include "boot_rollback_version.h"
#include "boot_authenticator.h"
#include "boot_hash.h"
#include "boot_msm.h"
#include "boot_sbl_qsee_interface.h"
#include "tzbsp_fuseprov_chipset.h"
#include "boot_statistics.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/*External Abort mask bit */
#define CPSR_EA_BIT     (1<<8)

extern boot_configuration_table_entry sbl1_config_table[];

/* DDR parameter partition ID */
extern uint8 ddr_params_partition_id[];

/* SEC partition ID */
extern uint8 sec_partition_id[];

/* Shared data between SBL*/
bl_shared_data_type bl_shared_data;

static boot_sbl_if_shared_info_type sbl1_shared_data;

/* SBL1 DDR ZI Initialization structure */
static boot_ram_init_data_type sbl1_ram_init_data_ddr =
{
  NULL,   //load_rw_base;
  NULL,  //image_rw_base;
  0,        //image_rw_len;
  (uint8*)&Image$$SBL1_DDR_ZI$$Base,  //image_zi_base;
  &Image$$SBL1_DDR_ZI$$ZI$$Length     //image_zi_len;
};


/* SBL1 DLOAD ZI Initialization structure */
static boot_ram_init_data_type sbl1_ram_init_data_dload =
{
  NULL,   //load_rw_base;
  NULL,  //image_rw_base;
  0,        //image_rw_len;
  (uint8*)&Image$$SBL1_DLOAD_ZI$$Base,  //image_zi_base;
  &Image$$SBL1_DLOAD_ZI$$ZI$$Length     //image_zi_len;
};


/* USB ZI Initialization structure */
static boot_ram_init_data_type sbl1_usb_init_data =
{
  NULL,   //load_rw_base;
  NULL,  //image_rw_base;
  0,        //image_rw_len;
  (uint8*)&Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$Base,  //image_zi_base;
  &Image$$SBL1_DEVICEPROGRAMMER_USB_ZI$$ZI$$Length     //image_zi_len;
};

/* Boot Logger specific variables */
static uint32 sbl_start_time = 0;
static boot_log_init_data boot_log_data =
{
  (void *)SBL1_LOG_BUF_START,
  SBL1_LOG_BUF_SIZE,
  (void *)SBL1_LOG_META_INFO_START,
  SBL1_LOG_META_INFO_SIZE,
  NULL
};

/* Roll back image table */
extern struct boot_version_rollback_img_set boot_rollback_version_img_set_table[];
extern uint32 boot_rollback_version_img_set_num;

/* Internal heap memory, inside the SBL1 image*/
#pragma arm section zidata = "BOOT_INTERNAL_HEAP"
static byte boot_internal_heap[BOOT_INTERNAL_HEAP_SIZE];
#pragma arm section zidata 

/* External heap memory, inside DDR */
#pragma arm section zidata = "BOOT_EXTERNAL_HEAP"
byte boot_external_heap[BOOT_EXTERNAL_HEAP_SIZE];
#pragma arm section zidata 

/* Global variable indicating if L2 error has happened */
uint32 L2_parity_error_in_sbl = FALSE;

/*DLOAD flag for SBL1 to enter PBL error handler*/
#ifdef BOOT_ENTER_PBL_DLOAD_ON_SBL_ERROR
  static boot_boolean edload_flag = TRUE;
#else
  static boot_boolean edload_flag = FALSE;
#endif

#pragma arm section zidata = "BOOT_CRASH_DUMP_ZONE"
static boot_crash_dump_type boot_crash_dump_data;
uint32* boot_crash_dump_regs_ptr = (uint32 *)(&boot_crash_dump_data.regs);
#pragma arm section zidata

static boolean is_ddr_initialized = FALSE;

/* Flag incidates if page table has been moved to DDR or not */
boot_boolean page_table_is_in_ddr = FALSE;

/* Secboot shared data structure */
secboot_verified_info_type sbl_verified_info;

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/


static void sbl1_debug_mode_enter()
{
  volatile uint32 *boot_debug_cookie_ptr = (uint32 *)SBL1_DEBUG_COOKIE_ADDR;
	
  /* Loop here to wait for jtag attach if cookie value matches*/
  while(*boot_debug_cookie_ptr == SBL1_DEBUG_COOKIE_VAL);
}


/*===========================================================================
**  Function :  sbl1_retrieve_shared_info_from_pbl
** ==========================================================================
*/
/*!
* 
* @brief
*   This function retreieves shared data from the PBL
* 
* @param[in] pbl_shared Pointer to shared data
*        
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_retrieve_shared_info_from_pbl(boot_pbl_shared_data_type *pbl_shared)
{
  static boot_pbl_shared_data_type sbl1_pbl_shared_data;
  
  BL_VERIFY( ((pbl_shared != NULL) && 
              (pbl_shared->secboot_shared_data != NULL)), BL_ERR_NULL_PTR );
  
  /* Copy the PBL shared data structure to SBL1 address space. */
  qmemcpy(&sbl1_pbl_shared_data,
          pbl_shared,
          sizeof(sbl1_pbl_shared_data));

  sbl1_shared_data.pbl_shared_data = &sbl1_pbl_shared_data;

  bl_shared_data.sbl_shared_data = &sbl1_shared_data;
  
  /* Set sdcc device info to NULL so it will be populated in sdcc init*/
  bl_shared_data.sbl_shared_data->sdcc_boot_device_info = NULL;


  /* Set the L1 pagetable base address and total pagetable size. */  
  bl_shared_data.sbl_shared_data->mmu_l1_pagetable_base = 
                             (uint32 *)pbl_shared->pbl_page_tbl_base;

  bl_shared_data.sbl_shared_data->mmu_pagetable_size =
    pbl_shared->pbl_page_tbl_size;
  

  /* Intialize the PBL accessor routines, pbl accessor will make its own copy */
  boot_pbl_interface_init( sbl1_shared_data.pbl_shared_data );
  
  /* Copy PBL verified info into SBL verified info */
  qmemcpy(&sbl_verified_info,
          &pbl_shared->secboot_shared_data->pbl_verified_info, 
          sizeof(sbl_verified_info));

#if (defined(SHARED_IMEM_TPM_HASH_REGION_SIZE) && (defined(SHARED_IMEM_TPM_HASH_REGION_OFFSET)))
  
  /* Clear the tpm hash imem */
  qmemset((void *)(SHARED_IMEM_TPM_HASH_REGION_OFFSET + (uint32)boot_shared_imem_cookie_ptr), 
          0x0,
          SHARED_IMEM_TPM_HASH_REGION_SIZE);
  
  /* 
    Initialize Tpm hash pointer to base of memory location
    allocated to store all hashes.
  */
  bl_shared_data.sbl_shared_data->next_avail_tpm_hash_blk_ptr = 
         (uint32*)(SHARED_IMEM_TPM_HASH_REGION_OFFSET + (uint32)boot_shared_imem_cookie_ptr);
  
  /* 
    First uint32 refers to number of hashes. Initialize it to 0. The value
    will be incremented every time a hash (sha 256) is stored 
  */
  *(bl_shared_data.sbl_shared_data->next_avail_tpm_hash_blk_ptr) = 0;
  
  /* Make ptr point to start of Hashes */  
  bl_shared_data.sbl_shared_data->next_avail_tpm_hash_blk_ptr++;
  
#endif

  /* Set build type based on compile flags */
  bl_shared_data.build_type = SBL_BUILD_SBL;
#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_LITE_COMPLETED
  bl_shared_data.build_type = SBL_BUILD_DEVICEPROGRAMMER_LITE;
#endif
#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_DDR_COMPLETED
  bl_shared_data.build_type = SBL_BUILD_DEVICEPROGRAMMER_DDR;
#endif
} /* sbl1_retrieve_shared_info_from_pbl */


/*===========================================================================
**  Function :  sbl1_boot_logger_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initialize the boot logger, must be called as the first 
*   function in main control function. 
*
* @param[in] boot_log_data boot logger's initialization data
* @param[in] pbl_shared    pbl shared data
*        
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_boot_logger_init
(
  boot_log_init_data *boot_log_data,
  boot_pbl_shared_data_type *pbl_shared
)
{
  /*initialize boot logger*/
  boot_log_init(boot_log_data);
  /* Write PBL timestamp milestones into beginning of log */
  boot_pbl_log_milestones(pbl_shared);

  /* Set the reference time to 0 as the start of boot*/
  boot_log_set_ref_time(0);

  /* Add SBL start entry using stored time from beginning of sbl1_main_ctl */
  boot_log_message_raw("SBL1, Start",
                       sbl_start_time,
                       LOG_MSG_TYPE_BOOT,
                       NULL);

}/* sbl1_boot_logger_init */


/*===========================================================================
**  Function :  sbl1_boot_logger_deinit
** ==========================================================================
*/
/*!
* 
* @brief
*   This function stops the boot logger and dumps the log information. Should
*   be called right before exiting the bootloader 
*
*        
* @par Dependencies
*   Called after sbl1_boot_logger_init()
*   
*/
void sbl1_boot_logger_deinit()
{
  boot_log_message("SBL1, End");
  boot_log_delta_time(sbl_start_time,
                      "SBL1, Delta");
  /* Add flash entry to boot log. */
  boot_statistics_log_flash();

  /* Add DDR frequency to boot log. */
  boot_log_ddr_frequency();
  boot_log_dump(&boot_log_data);
  
  /*Check if the boot debug scripts instructs to loop
    forever. It would instruct to do so, to enable collecting
    bootup times at end of boot process.
  
    etb_buf_addr is reused as a memory location for infinite loop
    magic number. etb_buf_addr is set by HLOS which does not 
    run in this use case. Hence it can be safely used. 
    boot_shared_imem_cookie_ptr should be set via boot_ram_init.c 
    in sbl2 by now. 
  */
  if(boot_shared_imem_cookie_ptr != NULL)
  {
    boot_log_forever_loop_check(boot_shared_imem_cookie_ptr->etb_buf_addr);
  }
}/* sbl1_boot_logger_deinit */


/*===========================================================================
**  Function :  sbl1_hw_platform_pre_ddr
** ==========================================================================
*/
/*!
* 
* @brief
*     	Parse cdt table to get start of sw-platform id CDB.
*       Call platform-id API and pass pointer to start of sw-platform id CDB.
*       Call DalPlatformInfo_CDTConfigPreDDR to share platform info to rpm
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void sbl1_hw_platform_pre_ddr(bl_shared_data_type* bl_shared_data)
{
  uint8* platform_id_cdb_ptr = NULL;
  uint32 platform_id_len = 0;
  DalDeviceHandle *phPlatform;
  DALResult eResult;
  char str[100];
  struct cdt_info *cdt_info_ptr = (struct cdt_info *)
                          bl_shared_data->sbl_shared_data->config_data_table_info;
  /*get a pointer to platform id data from configuration data table*/
  platform_id_cdb_ptr = 
              boot_get_config_data_block(cdt_info_ptr->cdt_ptr,
                                         CONFIG_DATA_BLOCK_INDEX_V1_PLATFORM_ID,
                                         &platform_id_len);  
   
  if(platform_id_cdb_ptr != NULL)
  {
    qmemset(str,0,100);
    snprintf(str, 100, "CDT version:%d,Platform ID:%d,Major ID:%d,Minor ID:%d,Subtype:%d",
                     platform_id_cdb_ptr[0],
                     platform_id_cdb_ptr[1],
                     platform_id_cdb_ptr[2],
                     platform_id_cdb_ptr[3],
                     platform_id_cdb_ptr[4]);
    boot_log_message(str);
    eResult = boot_DAL_DeviceAttachEx(NULL,
                                      DALDEVICEID_PLATFORMINFO,
                                      DALPLATFORMINFO_INTERFACE_VERSION,
                                      &phPlatform);
                                 
    if (eResult == DAL_SUCCESS) 
    {
      /*call the following API to store the platform id*/
      boot_DalPlatformInfo_CDTConfigPreDDR(phPlatform, platform_id_cdb_ptr);
      boot_DAL_DeviceDetach(phPlatform);
    }
  }
  
}/* sbl1_hw_platform_pre_ddr() */


/*===========================================================================
**  Function :  sbl1_hw_platform_smem
** ==========================================================================
*/
/*!
* 
* @brief
*     	Parse cdt table to get start of sw-platform id CDB.
*       Call platform-id API and pass pointer to start of sw-platform id CDB.
*       Call hw_init_smem to store platform id to SMEM
* 
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void sbl1_hw_platform_smem(bl_shared_data_type* bl_shared_data)
{
  uint8* platform_id_cdb_ptr = NULL;
  uint32 platform_id_len = 0;
  DalDeviceHandle *phPlatform;
  DALResult eResult;
  struct cdt_info *cdt_info_ptr = (struct cdt_info *)
                          bl_shared_data->sbl_shared_data->config_data_table_info;
  /*get a pointer to platform id data from configuration data table*/
  platform_id_cdb_ptr = 
              boot_get_config_data_block(cdt_info_ptr->cdt_ptr,
                                         CONFIG_DATA_BLOCK_INDEX_V1_PLATFORM_ID,
                                         &platform_id_len);  
   
  if(platform_id_cdb_ptr != NULL)
  {
    eResult = boot_DAL_DeviceAttachEx(NULL,
                                      DALDEVICEID_PLATFORMINFO,
                                      DALPLATFORMINFO_INTERFACE_VERSION,
                                      &phPlatform);
                                 
    if (eResult == DAL_SUCCESS) 
    {
      /*call the following API to store the platform id to DAL and SMEM*/
      boot_DalPlatformInfo_CDTConfigPostDDR(phPlatform, platform_id_cdb_ptr);
      boot_DAL_DeviceDetach(phPlatform);
    }
  }
  
}/* sbl1_hw_platform_smem() */

/*===========================================================================
**  Function :  sbl1_update_ddr_info
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will get ddr information from ddr driver and put it in 
*   sbl_shared_data. 
* 
* @param[in] bl_shared_data Pointer to shared data
*        
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_update_ddr_info(bl_shared_data_type *bl_shared_data)
{
  static sbl_if_shared_ddr_device_info_type ddr_info;
  ddr_size_info ddr_available = boot_ddr_get_size();
  ddr_size_partition ddr_partition_info = boot_ddr_get_partition();

  boot_share_extended_ddr_info(&ddr_info, &ddr_available, &ddr_partition_info);
  bl_shared_data->sbl_shared_data->ddr_shared_info = &ddr_info;
  boot_set_ddr_info(bl_shared_data);
}


/*===========================================================================
**  Function :  sbl1_update_ddr_page_table
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will map all avaliable DDR memory and addtional necessary mappings
*   to the page table in DDR
* 
* @param[in] bl_shared_data Pointer to shared data
*  
* @param[in] mmu_l1_page_table_ptr Pointer to the base of L1 page table that we want
*                                  to update
*
* @par Dependencies
*   Must be called after sbl1_ddr_zi_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_update_ddr_page_table
(
  bl_shared_data_type *bl_shared_data, 
  uint32 *mmu_l1_page_table_ptr
)
{
  uint32 ddr_index = 0;
  boot_boolean result = FALSE;
  sbl_if_shared_ddr_device_info_type *ddr_shared_info = NULL;
  sbl_if_shared_ddr_info_type *ddr_info = NULL;


  /* Memory block to descript DDR memory */
  struct mem_block *sbl_ddr_mem_block = &sbl1_ddr_mem_block;
  
  /* Populate the available DDR size info from DDR driver */
  sbl1_update_ddr_info(bl_shared_data);  
  ddr_shared_info = boot_get_ddr_info();

  /*Add all DDR region into the new page table as cacheble*/
  for(; ddr_index < ddr_shared_info->noofddr; ddr_index++)
  {
    ddr_info = &ddr_shared_info->ddr_info[ddr_index];
    
    sbl_ddr_mem_block->p_base = ddr_info->cs_addr;
    sbl_ddr_mem_block->v_base = ddr_info->cs_addr;

    /* ddr_info->ramsize is in MB */
    sbl_ddr_mem_block->size_in_kbytes = ddr_info->ramsize << 10;
    
    result = boot_mmu_page_table_map_single_mem_block
             (mmu_l1_page_table_ptr, sbl_ddr_mem_block);
    
    BL_VERIFY(result , BL_ERR_SBL);                                         
  } 

  /* Add the addtional memory map to new page table */
  result = boot_mmu_page_table_map_mem_block_list
           (mmu_l1_page_table_ptr, sbl1_addtional_memory_map);
    
  BL_VERIFY(result , BL_ERR_SBL);  
}

/*===========================================================================
**  Function :  sbl1_move_boot_log_to_ddr
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will Move existing boot log in sbl1 image region to DDR
*
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_move_boot_log_to_ddr()
{
  /* Relocate boot logger buffer to DDR and continue to save log in DDR */
  boot_log_relocate((void *)SCL_SBL1_DDR_BOOT_LOG_META_INFO_BASE,
                    (void *)SCL_SBL1_DDR_BOOT_LOG_BUF_BASE,
                    SCL_SBL1_DDR_BOOT_LOG_BUF_SIZE,  
                    (void *)SBL1_LOG_META_INFO_START,
                    (void *)SBL1_LOG_BUF_START,
                    SBL1_LOG_BUF_SIZE);
  
  boot_log_data.log_buffer_start = (void *)SCL_SBL1_DDR_BOOT_LOG_BUF_BASE;
  boot_log_data.log_buffer_size = SCL_SBL1_DDR_BOOT_LOG_BUF_SIZE;
  boot_log_data.meta_info_start = (void *)SCL_SBL1_DDR_BOOT_LOG_META_INFO_BASE;

}


/*===========================================================================
**  Function :  sbl1_ddr_zi_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will zero initialize SBL1's ZI and Page Table in DDR
*   sbl_shared_data. 
* 
* @param[in] bl_shared_data Pointer to shared data
*        
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void sbl1_ddr_zi_init()
{
  /* Zero out the SBL1 DDR ZI */
  boot_ram_init(&sbl1_ram_init_data_ddr);
    
  /* Zero out the SBL1 DLOAD ZI */
  boot_ram_init(&sbl1_ram_init_data_dload);
  
  /* Zero out the USB ZI */
  boot_ram_init(&sbl1_usb_init_data);
}


/*===========================================================================
**  Function :  sbl1_relocate_page_table_to_ddr
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will relocate page table from imem to DDR and map all avaliable
*   DDR memory to page table
* 
* @param[in] bl_shared_data Pointer to shared data
*  
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_relocate_page_table_to_ddr(bl_shared_data_type *bl_shared_data)
{
  if (page_table_is_in_ddr == FALSE)
  {
    boot_boolean result = FALSE;
    
    /* Firt Zero out the SBL1 DDR Page table region */
    qmemset((void *)SCL_SBL1_DDR_PAGE_TABLE_BASE, 0x0, SCL_SBL1_DDR_PAGE_TABLE_SIZE);
    
    /* Copy existing L1 and L2 page table from IMEM to DDR */
    result = boot_mmu_copy_page_table(bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base,
                                      mmu_l1_pagetable,
                                      mmu_l2_pagetable,
                                      MMU_NUM_L2_PT);
    BL_VERIFY(result , BL_ERR_SBL);
    
    /*Add all necessary mappings to the DDR page table*/
    sbl1_update_ddr_page_table(bl_shared_data, mmu_l1_pagetable);
    
    /* Relocate the page table base register, new page table will take effect after this call */
    mmu_relocate_page_table_base(mmu_l1_pagetable);
    
    /* Update the global page table base pointer and size. */
    bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base = mmu_l1_pagetable;
    bl_shared_data->sbl_shared_data->mmu_pagetable_size = SCL_SBL1_DDR_PAGE_TABLE_SIZE;
    page_table_is_in_ddr = TRUE;
    
    /* Add the DDR heap to DAL */
    boot_DALSYS_HeapInit(boot_external_heap, BOOT_EXTERNAL_HEAP_SIZE, TRUE);

    /* Move boot log to DDR */    
    sbl1_move_boot_log_to_ddr();

  }
}


/*===========================================================================
**  Function :  sbl1_post_ddr_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will initialize the data section in DDR and relocate boot log to DDR
*   After execution of this function SBL1 will enter Sahara in stead of PBL
*   emergency dload mode in case of error
*
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_post_ddr_init(bl_shared_data_type *bl_shared_data)
{
  /* Initialize DDR ZI region */
  sbl1_ddr_zi_init();

  /* If DDR training is not required OR we are in dload mode 
     relocate page table to ddr and map all the regions we need */
  if (boot_ddr_params_is_training_required() == FALSE ||
      boot_dload_is_dload_mode_set() == TRUE)
  {
    sbl1_relocate_page_table_to_ddr(bl_shared_data);
  }
  

  
  if (edload_flag != TRUE)
  {
    /* Update the dload entry to sbl1 sahara dload entry function */
    sbl_dload_entry = sbl1_dload_entry;
  }	
  
  /* Set the variable to TRUE to indicate DDR is initialized */
  is_ddr_initialized = TRUE;
}

/*===========================================================================
**  Function :  sbl_error_handler
** ==========================================================================
*/
/*!
* 
* @brief
*   This saves registers and fills up the information
*   in the boot crash information for simulator tool
*
* @param[in] None
*        
* @par Dependencies
*  none
*   
*/
void sbl_error_handler(void)
{
 /* Only if DDR is initialized */
   if (is_ddr_initialized)
   {
      /* Initialize the boot crash dump data type */
      boot_crash_dump_data.crash_dump_cookie = BOOT_CRASH_DUMP_COOKIE;
      boot_crash_dump_data.version = BOOT_CRASH_DUMP_VERSION;

      /* SBL DDR region */
      boot_crash_dump_data.sbl_ddr_addr = SCL_SBL1_DDR_DATA_BASE;
      boot_crash_dump_data.sbl_ddr_size = SCL_SBL1_DDR_DATA_SIZE;
      boot_crash_dump_data.sbl_ddr_dump_addr = 
           SCL_SBL1_CRASH_DUMP_DATA_BASE + SCL_SBL1_CRASH_DUMP_DATA_SIZE;

      /* SBL L2 region */
      boot_crash_dump_data.sbl_l2_addr = SCL_BOOT_L2_LOCK_BASE;
      boot_crash_dump_data.sbl_l2_size = SCL_BOOT_L2_LOCK_SIZE;
      boot_crash_dump_data.sbl_l2_dump_addr = 
         boot_crash_dump_data.sbl_ddr_dump_addr + boot_crash_dump_data.sbl_ddr_size;

	  /* MSM does not have PCIe region so write 0x0 */
	  boot_crash_dump_data.sbl_pcie_coredump_addr = 0x0;
	  boot_crash_dump_data.sbl_pcie_coredump_size = 0x0;
	  
	  /* Flush everything to DDR */
      qmemcpy((void *)boot_crash_dump_data.sbl_ddr_dump_addr, 
              (const void *)boot_crash_dump_data.sbl_ddr_addr,
              boot_crash_dump_data.sbl_ddr_size);		 
		 
      qmemcpy((void *)boot_crash_dump_data.sbl_l2_dump_addr, 
              (const void *)boot_crash_dump_data.sbl_l2_addr,
              boot_crash_dump_data.sbl_l2_size);
	  
	  /* Flush the cache */
      mmu_flush_cache();
   }   
}

/*===========================================================================
**  Function :  sbl1_post_ddr_training_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will initialize the data section in DDR, populate ddr size information
*   then relocate page table from IMEM to DDR and map all memory regions required by SBL1 
*   in page table.
*
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_post_ddr_training_init(bl_shared_data_type *bl_shared_data)
{
  if (boot_ddr_params_is_training_required() == TRUE)
  {
    /* Reinitialize DDR ZI region because DDR training may corrupt DDR*/
    sbl1_ddr_zi_init();
  
    /* Relocate page table to DDR */
    sbl1_relocate_page_table_to_ddr(bl_shared_data);
  }
}


/*===========================================================================
**  Function :  sbl1_load_ddr_training_data_from_partition
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will read the ddr training data from ddr parition
*
* @par Dependencies
*   Must be called before sbl1_ddr_init
*   
* @retval
*   pointer to ddr training data
* 
* @par Side Effects
*   None
* 
*/
void sbl1_load_ddr_training_data_from_partition
(
  uint8* ddr_training_data_buf,
  uint32 ddr_training_data_size
)
{
  boot_boolean success = FALSE;
  boot_flash_trans_if_type *trans_if = NULL;
  
  /* Verify ddr training data size is valid */
  BL_VERIFY((ddr_training_data_size != 0) && 
            (ddr_training_data_size <= SCL_DDR_TRAINING_DATA_BUF_SIZE),
            BL_ERR_SBL);
            
  boot_flash_configure_target_image(ddr_params_partition_id);

  trans_if = boot_flash_dev_open_image(GEN_IMG);

  BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR );
   
  /* Allow the address range of the ddr_training_data_buf to be written to. */
  boot_clobber_add_local_hole(boot_flash_trans_get_clobber_tbl_ptr(trans_if),
                              ddr_training_data_buf,
                              ddr_training_data_size);

  /* Read the ddr training data out, store it in the buffer */
  success = boot_flash_trans_read(trans_if,
                                  ddr_training_data_buf,
                                  0,
                                  ddr_training_data_size);

  /* Remove the local hole */                                  
  boot_clobber_remove_local_hole(boot_flash_trans_get_clobber_tbl_ptr(trans_if),
                                 ddr_training_data_buf,
                                 ddr_training_data_size);
							  
  BL_VERIFY(success, BL_ERR_SBL);
  
  /* close parition */
  boot_flash_dev_close_image(&trans_if);
}


/*===========================================================================
**  Function :  sbl1_load_sec_partition
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will read the sec partition and load it to a defined memory 
*   region
*
* @par Dependencies
*   Must be called after TZ is loaded
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_load_sec_partition ( bl_shared_data_type *bl_shared_data )
{
  boot_boolean success = FALSE;
  boot_flash_trans_if_type *trans_if = NULL;
  uint32 secdat_buffer_location = 0;
  char bootlog_buffer[BOOT_LOG_TEMP_BUFFER_SIZE];
  uint32 bytes_read = 0;
  /* Reset the flash statistics byte counter so the number of bytes read
     from flash can be read later. */
  boot_statistics_reset_flash_byte_counter();

  boot_log_message("Image Load, Start");
  boot_log_start_timer();


  /* Search image entries in QSEE interface for QSEE and get elf type. 
     The SEC partition gets loaded to a different address depending on if
     QSEE is 32 or 64 bit. */
  switch(boot_sbl_qsee_interface_get_eident(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_QSEE_SW_TYPE))
  {
  case ELF_CLASS_32:
    secdat_buffer_location = SCL_QSEE_CODE_BASE +
      TZBSP_SECDAT_BUFFER_OFFSET_LOC_32BIT;
    break;
    
  case ELF_CLASS_64:
    secdat_buffer_location = SCL_QSEE_CODE_BASE +
      TZBSP_SECDAT_BUFFER_OFFSET_LOC_64BIT;
    break;

  /* If ELF_CLASS is invalid then secdat_buffer_location will remain zero
     and error in check below. */
  }  

  boot_flash_configure_target_image(sec_partition_id);

  trans_if = boot_flash_dev_open_image(GEN_IMG);

  /* Partition is present, then load it to memory */
  if (trans_if != NULL) 
  {
     /* Verify destination address and size */
     BL_VERIFY(TZBSP_SECDAT_BUFFER_SIZE &&
               secdat_buffer_location,
               BL_ERR_SBL);   

     /* Read the SEC data to the destination address */
     success = boot_flash_trans_read(trans_if,
                                    (void *)secdat_buffer_location,
                                     0,
                                     TZBSP_SECDAT_BUFFER_SIZE);
    
     BL_VERIFY(success, BL_ERR_SBL);

     /* Close parition */
     boot_flash_dev_close_image(&trans_if);
  }
  /* Retreive number of bytes read from flash via boot statistics. */
  bytes_read = boot_statistics_get_flash_byte_counter();

  /* Format bytes read value into string. */
  snprintf(bootlog_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "(%d Bytes)",
           bytes_read);

  boot_log_stop_timer_optional_data("SEC Image Loaded, Delta",
                                    bootlog_buffer);
}


/*===========================================================================
**  Function :  sbl1_save_ddr_training_data_to_partition
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will save ddr training data to ddr partition
* 
* @param[in] ddr_training_data_ptr Pointer to ddr training data
*
* @param[in] ddr_training_data_size size of ddr training data
*     
* @par Dependencies
*   Must be called after ddr_post_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_save_ddr_training_data_to_partition
(
  uint8* ddr_training_data_ptr,
  uint32 ddr_training_data_size
)
{
  boot_flash_trans_if_type *trans_if = NULL;
//  boot_boolean success = FALSE;
  
  /* Write DDR training data to storage device */
  boot_flash_configure_target_image(ddr_params_partition_id);

  trans_if = boot_flash_dev_open_image(GEN_IMG);

  BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR );

#if 0 //Need revisit
  success = dev_sdcc_write_bytes(ddr_training_data_ptr,
                                 0,
                                 ddr_training_data_size,
                                 GEN_IMG);

  BL_VERIFY(success, BL_ERR_SBL);
#endif
  /* close parition*/
  boot_flash_dev_close_image(&trans_if);
}


/*===========================================================================
**  Function :  sbl1_efs_handle_cookies
** ==========================================================================
*/
/*!
* 
* @brief
*   Calls efs cookie handling api to perform efs backup/restore action
* 
* @param[in] bl_shared_data Pointer to shared data
*   
* @par Dependencies
*   None
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void sbl1_efs_handle_cookies(bl_shared_data_type* bl_shared_data)
{
  boot_log_message("sbl1_efs_handle_cookies, Start");
  boot_log_start_timer();
  boot_efs_handle_cookies();
  boot_log_stop_timer("sbl1_efs_handle_cookies, Delta");
}/* sbl1_efs_handle_cookies */


/*===========================================================================
**  Function :  sbl1_populate_initial_mem_map
** ==========================================================================
*/
/*!
* 
* @brief
*   populate the existing pbl page table with SBL memory mapping
* 
* @param[in] bl_shared_data Pointer to shared data
*   
* @par Dependencies
*   None
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void sbl1_populate_initial_mem_map(bl_shared_data_type* bl_shared_data)
{
  boot_boolean result;
  
  /* First populate the initial memory map list */    
  BL_VERIFY(bl_shared_data->
              sbl_shared_data->
              pbl_shared_data->
              pbl_page_tbl_base != NULL,
            BL_ERR_NULL_PTR);
 

  /* Initialize L2 page table pointer and calculate number of available
     L2 entries left. */
  boot_l2_page_table_init(bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base,
                          bl_shared_data->sbl_shared_data->mmu_pagetable_size);


  result =
    boot_mmu_page_table_map_mem_block_list(bl_shared_data->
                                             sbl_shared_data->
                                             mmu_l1_pagetable_base,
                                           sbl_initial_memory_map);
    
  BL_VERIFY(result , BL_ERR_SBL);   
}

/*===========================================================================
**  Function :  sbl1_tlmm_init
** ==========================================================================
*/
/*!
* 
* @brief
*   SBL1 wrapper to Initialize Tlmm and gpio for low power config
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   must be called after boot_smem_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_tlmm_init(bl_shared_data_type *bl_shared_data)
{
  /* Initialize Tlmm and gpio for low power config ,must be done after smem init*/
  if (FALSE == boot_gpio_init())
  {
   BL_VERIFY(FALSE, BL_ERR_SBL);
  }  
}


/*===========================================================================
**  Function :  sbl1_init_qsee_interface
** ==========================================================================
*/
/*!
* 
* @brief
*   Zero out the SBL - QSEE interface and populate with SBL sec info.
*
* @param[in]
*   bl_shared_data           Pointer to shared data
*   sbl1_auth_verified_info  Pointer to auth data provided by PBL
*        
* @par Dependencies
*   None
*   
*/
void sbl1_init_sbl_qsee_interface(bl_shared_data_type *bl_shared_data,
                                  secboot_verified_info_type *sbl1_auth_verified_info)
{
  // Zero out the interface
  qmemset(&(bl_shared_data->sbl_qsee_interface),
          0x0,
          sizeof(boot_sbl_qsee_interface));

  // Assign the magic numbers and version
  bl_shared_data->sbl_qsee_interface.magic_1 = SBL_QSEE_MAGIC_NUMBER_1;
  bl_shared_data->sbl_qsee_interface.magic_2 = SBL_QSEE_MAGIC_NUMBER_2;
  bl_shared_data->sbl_qsee_interface.version = 1;  // First version
           
  /* Update QSEE interface with SBL auth information provided by PBL */
  qmemcpy(&(bl_shared_data->sbl_qsee_interface.boot_image_entry[0].image_verified_info),
          sbl1_auth_verified_info,
          sizeof(secboot_verified_info_type));

  /* Update SBL entry with information not passed by PBL */
  bl_shared_data->sbl_qsee_interface.boot_image_entry[0].image_id = SECBOOT_SBL_SW_TYPE;
  bl_shared_data->sbl_qsee_interface.boot_image_entry[0].entry_point = SCL_SBL1_VECTOR_BASE;
  bl_shared_data->sbl_qsee_interface.number_images++;
}


/*===========================================================================
**  Function :  sbl1_cleanse_security_regions
** ==========================================================================
*/
/*!
* 
* @brief
*   Zero out the QSEE and QHEE DDR region for security if needed.
*
* @param[in]
*   bl_shared_data           Pointer to shared data
*        
* @par Dependencies
*   None
*   
*/
void sbl1_cleanse_security_regions(bl_shared_data_type * bl_shared_data)
{
  secboot_verified_info_type * sbl_secboot_verified_info = NULL;

  /* Locate the entry in the SBL - QSEE interface cooresponding to SBL and
     get the secboot verified info. */
  sbl_secboot_verified_info =
    boot_sbl_qsee_interface_get_secboot_info(&(bl_shared_data->sbl_qsee_interface),
                                             SECBOOT_SBL_SW_TYPE);
  BL_VERIFY(sbl_secboot_verified_info,
            BL_ERR_NULL_PTR);


  /* Zero out QSEE and QHEE  memory if needed */
  boot_qsee_cond_zero_peripheral_memory(sbl_secboot_verified_info);
}


/*===========================================================================
**  Function :  sbl1_main_ctl
** ==========================================================================
*/
/*!
* 
* @brief
*   The  Main Controller performs the following functions:
*       - Initializes ram
*       - And so on...
* 
* @param[in] pbl_shared Pointer to shared data
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   This function never returns.
* 
*/
void sbl1_main_ctl(boot_pbl_shared_data_type *pbl_shared)
{
  bl_error_type status = BL_ERR_OTHERS;
  /* Calculate the SBL start time for use during boot logger initialization. */
  sbl_start_time = CALCULATE_TIMESTAMP(HWIO_IN(TIMETICK_CLK));

#ifdef SBL1_DISABLE_D_CACHE
  mmu_flush_cache_and_disable();
  mmu_enable_instruction_cache();
#endif

  /* Enabling external abort */
  sbl1_external_abort_enable(CPSR_EA_BIT);
  
  /*Configure Domain access control register */
  mmu_set_dacr(DACR_ALL_DOMAIN_CLIENTS);

  /* Enable qdss workaround*/
  boot_clock_debug_init();
  
  /* Enter debug mode if debug cookie is set */
  sbl1_debug_mode_enter();
  
  /* Initialize the stack protection canary */
  boot_init_stack_chk_canary();
  
  /* Initialize boot shared imem */
  boot_shared_imem_init(&bl_shared_data);

  /* Initialize busywait module
     Note: This is required to be done before logger init due to uart driver
     dependency on busywait */
  BL_VERIFY(boot_busywait_init() == DAL_SUCCESS,BL_ERR_SBL);
  
  /* Initialize boot logger and start the log timer */
  sbl1_boot_logger_init(&boot_log_data,
                        pbl_shared);
  boot_log_set_meta_info(boot_log_data.meta_info_start);

  /* Retrieve info passed from PBL*/
  sbl1_retrieve_shared_info_from_pbl(pbl_shared);
  
  /* Initialize the QSEE interface */
  sbl1_init_sbl_qsee_interface(&bl_shared_data,
                               &sbl_verified_info);

  /* Initialize SBL memory map.
     Initializing early because drivers could be located in RPM Code RAM. */
  sbl1_populate_initial_mem_map(&bl_shared_data);

  /* Initialize the secboot functions */
  BL_VERIFY(boot_secboot_ftbl_init(&bl_shared_data) == BL_ERR_NONE,
            BL_ERR_IMG_SECURITY_FAIL);

  /* set hash algorithm */
  BL_VERIFY((status = boot_set_hash_algo(SBL_HASH_SHA256)) == BL_ERR_NONE,
            status);
  
  /* Initialize dal heap using internal memory */
  boot_DALSYS_HeapInit(boot_internal_heap,
                       BOOT_INTERNAL_HEAP_SIZE,
                       FALSE);
  
  /*Initialize DAL, needs to be called before modules that uses DAL */  
  boot_DALSYS_InitMod(NULL); 
  
  /* Store the sbl1 hash to shared imem */
  boot_store_tpm_hash_block(&bl_shared_data,
                            &sbl_verified_info);

   /* Initialize roll back protection image table */
  boot_rollback_init_img_set_table(&bl_shared_data,
                                   &boot_rollback_version_img_set_table[0],
                                   boot_rollback_version_img_set_num);
                                   
  /* Check L2 error flag, if it has been set then set dload cookie and reboot */
  if(L2_parity_error_in_sbl)
  {
    boot_dload_set_cookie();
    boot_hw_reset(BOOT_WARM_RESET_TYPE);
  }
  

  /*-----------------------------------------------------------------------
   Add the SBL1, System Debug, SMEM, and OCIMEM address ranges
   to the memory protection. 
  -----------------------------------------------------------------------*/
  boot_clobber_add_global_protection_region((void *)SBL1_CODE_BASE,
                                            SBL1_MAX_IMAGE_SIZE );

  boot_clobber_add_global_protection_region((void *)SCL_SBL1_DDR_DATA_BASE,
                                            SCL_SBL1_DDR_DATA_SIZE);

  boot_clobber_add_global_protection_region((void *)SCL_SYSTEM_DEBUG_BASE,
                                            SCL_SYSTEM_DEBUG_SIZE);

  boot_clobber_add_global_protection_region((void *)SCL_SHARED_RAM_BASE,
                                            SCL_SHARED_RAM_SIZE);

  boot_clobber_add_global_protection_region((void *)SCL_IMEM_BASE,
                                            SCL_IMEM_SIZE);

  /*----------------------------------------------------------------------
   In SBL builds the function below is stubbed out (does nothing).
   When building Deviceprogrammer the function below never returns.
   The function below becomes active by defining the feature
   FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_LITE_COMPLETED.
  ----------------------------------------------------------------------*/
  boot_hand_control_to_deviceprogrammer_lite_main (pbl_shared);
  
  /*-----------------------------------------------------------------------
    Process the target-dependent SBL1 procedures
  -----------------------------------------------------------------------*/
  boot_config_process_bl(&bl_shared_data,
                         SBL1_IMG,
                         sbl1_config_table);

} /* sbl1_main_ctl() */
