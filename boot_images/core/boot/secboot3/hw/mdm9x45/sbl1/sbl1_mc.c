/*=============================================================================

                              SBL1 Main Control

GENERAL DESCRIPTION
  This file contains the main control for SBL1 execution. Since this implements
  the main control functionality, it may be dependent on all modules of the
  system.

Copyright 2010-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/mdm9x45/sbl1/sbl1_mc.c#1 $ 
$DateTime: 2015/09/01 00:30:35 $ 
$Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
01/08/15     jz      Moved clearing of TCSR_RESET_DEBUG to inside boot_hw_reset
01/05/15     jz      Clear TCSR_RESET_DEBUG_SW_ENTRY EN bit before warm reset
10/23/14     jz      Added qfprom test support in SBL
10/16/14     jz      Updated to scatterload the OEM code region to DDR
09/17/14     jz      Removed rpm_cookie_sync
09/08/14     jz      Added support for fast USB enumeration
09/05/14     jz      Added DEP for L2 region not used by SBL as non-exec
09/04/14     jz      Update page table for SBL L2 region DEP at runtime
07/16/14     jz      Fixed getting image entry point from qsee interface
06/20/14     ck      Modified and renamed sbl1_cleanse_qsee_region to sbl1_cleanse_security_regions
06/06/14     ck      Updated NO_FLASH conditionals to use booting_from_storage_media
06/03/14     ck      Added flash statistic entry when deinitializing boot logger
05/28/14     ck      Added PBL timestamp milestones to sbl1_boot_logger_init
05/14/14     ck      Added sbl1_cleanse_qsee_region
04/22/14     jz      Ported support PCIe coredump
04/14/14     jz      Updated to support Badger like boot flows in Bear for MDM
04/08/14     ck      Set mmu_pagetable_size in bl_shared_data after moving PT's to DDR
04/08/14     ck      Added call to boot_l2_page_table_init in setup of initial memory map
04/08/14     ck      Added logic to copy page table size from PBL into bl_shared_data
04/02/14     ck      Updated use of boot_sbl_qsee_interface as it changed per Bear BAS 2.0
04/01/14     ck      Updated sbl1_retrieve_shared_info_from_pbl as Bear PBL now uses common secboot
03/26/14     jz      Ported changes to save the registers early in the error handler
03/18/14     ck      Updated boot_hw_reset calls as they now take a reset type parameter
02/24/14     jz      Updated for Bear support
11/12/13     kedara  Support SBL crash dumps.
11/07/13     kedara  Added boot_set_hash_update_mode
10/25/13     kedara  Remove Null pointer check for apps image since 0 is apps
                     entry address
09/04/13     kedara  Update cache settings for arm a7.
08/23/13     kedara  Update sbl1_update_shared_imem_base
08/06/13     aus     Added support for new secboot_verified_info structure 
07/10/13     aus     Added support to save the registers and flush the L2 TCM to DDR
                     on SBL errors
06/19/13     kedara  Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
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
#include "boot_extern_tlmm_interface.h"
#include "boot_extern_ddr_interface.h"
#include "boot_extern_hotplug_interface.h"
#include "boot_extern_efs_interface.h"
#include "boot_extern_clk_interface.h"
#include "boot_extern_busywait_interface.h"
#include "boot_extern_seccfg_interface.h"
#include "sbl1_target.h"
#include "boot_fastcall_tz.h"
#include "boot_page_table_armv7.h"
#include "boot_mmu_table.h"
#include "sbl1_hw.h"
#include "boot_qfprom_test.h"
#include "boot_loader.h"
#include "boot_rollback_version.h"
#include "boot_authenticator.h"
#include "boot_hash.h"
#include "sbl1_mc.h"
#include "boothw_target.h"
#include "boot_dload_debug.h"
#include "boot_extern_pcie_interface.h"
#include "boot_statistics.h"
#include "boot_extern_usb_interface.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Macros to round up and down a value to be 4KB aligned */
#define ROUND_UP_TO_4KB_ALIGNED(VALUE)   ( ((VALUE) == ((VALUE) & ~0xFFF)) ? (VALUE) : (((VALUE) & ~0xFFF) + 0x1000) )
#define ROUND_DOWN_TO_4KB_ALIGNED(VALUE) ( ((VALUE) == ((VALUE) & ~0xFFF)) ? (VALUE) : ((VALUE) & ~0xFFF) )

extern boot_configuration_table_entry sbl1_config_table[];
extern uint8 fota_partition_name[];

/* Shared data between SBL*/
bl_shared_data_type bl_shared_data;

static boot_sbl_if_shared_info_type sbl1_shared_data;


/* SBL1 RAM Data Initialization structure */
static boot_ram_init_data_type sbl1_ram_init_data =
{
  (uint8*)&Load$$SBL1_DATA_RW$$Base,   /*load_rw_base*/
  (uint8*)&Image$$SBL1_DATA_RW$$Base,  /*image_rw_base*/
  &Image$$SBL1_DATA_RW$$Length,        /*image_rw_len*/
  (uint8*)&Image$$SBL1_DATA_ZI$$Base,  /*image_zi_base*/
  &Image$$SBL1_DATA_ZI$$ZI$$Length     /*image_zi_len*/
};

static boot_scatterload_data_type sbl1_oem_ddr_region =
{
  (uint8*)&Load$$SBL1_OEM_CODE_DDR$$Base,   /*load_rw_base*/
  (uint8*)&Image$$SBL1_OEM_CODE_DDR$$Base,  /*image_rw_base*/
  &Image$$SBL1_OEM_CODE_DDR$$Length        /*image_rw_len*/
};

/* SBL1 DDR ZI Initialization structure */
static boot_ram_init_data_type sbl1_ram_init_data_ddr =
{
  NULL,   //load_rw_base;
  NULL,  //image_rw_base;
  0,        //image_rw_len;
  (uint8*)&Image$$SBL1_DDR_ZI$$Base,  //image_zi_base;
  &Image$$SBL1_DDR_ZI$$ZI$$Length     //image_zi_len;
};

/* SBL1 DDR DLOAD ZI Initialization structure */
static boot_ram_init_data_type sbl1_ram_init_data_dload =
{
  NULL,   //load_rw_base;
  NULL,  //image_rw_base;
  0,        //image_rw_len;
  (uint8*)&Image$$SBL1_DLOAD_ZI$$Base,  //image_zi_base;
  &Image$$SBL1_DLOAD_ZI$$ZI$$Length     //image_zi_len;
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
static byte boot_external_heap[BOOT_EXTERNAL_HEAP_SIZE];
#pragma arm section zidata 

#pragma arm section zidata = "BOOT_CRASH_DUMP_ZONE"
static boot_crash_dump_type boot_crash_dump_data;
uint32* boot_crash_dump_regs_ptr = (uint32 *)(&boot_crash_dump_data.regs);
#pragma arm section zidata

static boolean is_ddr_initialized = FALSE;

static boolean qsee_is_ready = FALSE;

/* Secboot shared data structure */
secboot_verified_info_type sbl_verified_info;

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================

**  Function :  sbl1_debug_mode_enter

** ==========================================================================
*/
/*!
* 
* @brief
*   Enters debug mode 
* 
* @param[in] 
*  None
* 
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static volatile uint32 *enter_fastdebug_ptr = (uint32*) BOOT_DEBUG_COOKIE_ADDR;
void sbl1_debug_mode_enter (void)
{
  while( BOOT_DEBUG_COOKIE == (uint32)(*enter_fastdebug_ptr));
  
  /* Reset cookie value */
  *enter_fastdebug_ptr = 0x0;
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
  qmemcpy( &sbl1_pbl_shared_data, pbl_shared,
            sizeof(sbl1_pbl_shared_data) );

  sbl1_shared_data.pbl_shared_data = &sbl1_pbl_shared_data;
  
  bl_shared_data.sbl_shared_data = &sbl1_shared_data;
  
  /*set sdcc device info to NULL so it will be populated in sdcc init*/
  bl_shared_data.sbl_shared_data->sdcc_boot_device_info = NULL;

  
  /* Set the L1 pagetable base address and total pagetable size. */
  bl_shared_data.sbl_shared_data->mmu_l1_pagetable_base = 
                             (uint32 *)pbl_shared->pbl_page_tbl_base;

  bl_shared_data.sbl_shared_data->mmu_pagetable_size =
    pbl_shared->pbl_page_tbl_size;

  
  /* Intialize the PBL accessor routines, pbl accessor will make
   * its own copy */
  boot_pbl_interface_init( sbl1_shared_data.pbl_shared_data );
 
  /* Copy PBL verified info into SBL verified info */
  qmemcpy(&sbl_verified_info,
          &pbl_shared->secboot_shared_data->pbl_verified_info, 
          sizeof(sbl_verified_info));

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

  /*set the reference time to 0 as the start of boot*/
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
* @param[in] None
*        
* @par Dependencies
*   Called after sbl1_boot_logger_init()
*   
*/
void sbl1_boot_logger_deinit( void )
{
  boot_log_message("SBL1, End");

  boot_log_delta_time(sbl_start_time,
                      "SBL1, Delta");
  
  /* Add flash entry to boot log. */
  boot_statistics_log_flash();

  /* Add DDR frequency to boot log. */
  boot_log_ddr_frequency();

  /* Add Apps0 proc frequency to boot log. */
  boot_log_apps_frequency(0);

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
    mmu_flush_cache();
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
  
  /* Zero out the SBL1 DDR Page table region */
  qmemset((void *)SCL_SBL1_DDR_PAGE_TABLE_BASE, 0x0, SCL_SBL1_DDR_PAGE_TABLE_SIZE);
  
  /* Zero out the SBL1 DLOAD DDR ZI */
  boot_ram_init(&sbl1_ram_init_data_dload);
  
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
static void sbl1_update_ddr_page_table(bl_shared_data_type *bl_shared_data)
{
  uint32 ddr_index = 0;
  boot_boolean result = FALSE;
  sbl_if_shared_ddr_device_info_type *ddr_shared_info = NULL;
  sbl_if_shared_ddr_info_type *ddr_info = NULL;
  
  /* DDR memory region, read-write, non-executable, inner cachable,
    outer non cached. 
  */
  struct mem_block sbl_ddr_mem_block = 
  {
    0, 0, 0,
    MMU_L1_SECTION_MAPPING, MMU_L1_PAGETABLE_MEM_READ_WRITE, 
    MMU_L1_PAGETABLE_INNER_WBWA_OUTER_NONCACHE, MMU_L1_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN
  };
  
  /* Populate the available DDR size info from DDR driver */
  sbl1_update_ddr_info(bl_shared_data);  
  ddr_shared_info = boot_get_ddr_info();
  
  /*Add all DDR region into the new page table as cacheble*/
  for(; ddr_index < ddr_shared_info->noofddr; ddr_index++)
  {
    ddr_info = &ddr_shared_info->ddr_info[ddr_index];
    
    sbl_ddr_mem_block.p_base = ddr_info->cs_addr;
    sbl_ddr_mem_block.v_base = ddr_info->cs_addr;

    /* ddr_info->ramsize is in MB */
    sbl_ddr_mem_block.size_in_kbytes = ddr_info->ramsize << 10;
    
    result = boot_mmu_page_table_map_single_mem_block(mmu_l1_pagetable,
                                                      &sbl_ddr_mem_block);
    BL_VERIFY(result , BL_ERR_SBL);                                         
  } 
  /* Add the addtional memory map to new page table */
  result =
    boot_mmu_page_table_map_mem_block_list(mmu_l1_pagetable,
                                           sbl1_addtional_memory_map);
  BL_VERIFY(result , BL_ERR_SBL);  
}


/*===========================================================================
**  Function :  sbl1_relocate_page_table
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will initialize sbl1 DDR zi and
*   relocate page table from imem to DDR and map all avaliable
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
static void sbl1_relocate_page_table(bl_shared_data_type *bl_shared_data)
{
  boot_boolean result = FALSE;
  
  /* Copy existing L1 and L2 page table from IMEM to DDR */
  result = boot_mmu_copy_page_table((uint32 *)
                                    bl_shared_data->sbl_shared_data->
                                    pbl_shared_data->pbl_page_tbl_base,
                                    mmu_l1_pagetable,
                                    mmu_l2_pagetable,
                                    MMU_NUM_L2_PT);
  BL_VERIFY(result , BL_ERR_SBL);
  
  /*Add all necessary mappings to the DDR page table*/
  sbl1_update_ddr_page_table(bl_shared_data);
  
  /* Relocate the page table base register, new page table will take effect after this call */
  mmu_relocate_page_table_base(mmu_l1_pagetable);

  /* Update the global page table base pointer and size. */
  bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base = mmu_l1_pagetable;
  bl_shared_data->sbl_shared_data->mmu_pagetable_size = SCL_SBL1_DDR_PAGE_TABLE_SIZE;

}

/*===========================================================================
**  Function :  sbl1_scatterload_oem_code_region
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will scatterload OEM code region from L2 TCM to DDR and mark
*   the region as read-only, executable and cacheable.
* 
* @param[in] None
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
static void sbl1_scatterload_oem_code_region()
{
  boot_boolean result;
  
  struct mem_block oem_ddr_mem_block = 
  {
    SCL_SBL1_DDR_OEM_BASE, SCL_SBL1_DDR_OEM_BASE, SBL_OEM_CODE_DDR_SIZE>>10,
    MMU_L2_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_ONLY, 
    MMU_L2_PAGETABLE_INNER_WBWA_OUTER_NONCACHE, 
    MMU_L2_PAGETABLE_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN
  };

  /* Initialize the region to 0x0 */
  qmemset((void *)SCL_SBL1_DDR_OEM_BASE, 0x0, SBL_OEM_CODE_DDR_SIZE);

  /* Load OEM code region */
  boot_scatterload_region(&sbl1_oem_ddr_region);

  /* Map the region as read only and executable */
  result = boot_mmu_page_table_map_single_mem_block(mmu_l1_pagetable,
	 											&oem_ddr_mem_block); 
  BL_VERIFY(result , BL_ERR_SBL);
}

/*===========================================================================
**  Function :  sbl1_post_ddr_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will initialize the data section in DDR, populate ddr size information
*   then relocate page table from IMEM to DDR and map all memory regions required by SBL1 
*   in page table. After execution of this function SBL1 will enter Sahara in stead of PBL
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
 
  sbl1_ddr_zi_init();
  
  sbl1_relocate_page_table(bl_shared_data);
  
  /* Scatterload OEM Code region to DDR, if present */  
  if (sbl1_oem_ddr_region.image_region_len != 0)
  {
    sbl1_scatterload_oem_code_region();
  }

  /* Add the DDR heap to DAL */
  boot_DALSYS_HeapInit(boot_external_heap, BOOT_EXTERNAL_HEAP_SIZE, TRUE);
  
  /* Relocate boot logger buffer to DDR and continue to save log in DDR */
  boot_log_relocate((void *)SCL_DDR_BOOT_LOG_META_INFO_BASE,
                    (void *)SCL_DDR_BOOT_LOG_BUF_BASE,
                    SCL_DDR_BOOT_LOG_BUF_SIZE,  
                    (void *)SBL1_LOG_META_INFO_START,
                    (void *)SBL1_LOG_BUF_START,
                    SBL1_LOG_BUF_SIZE);
  
  boot_log_data.log_buffer_start = (void *)SCL_DDR_BOOT_LOG_BUF_BASE;
  boot_log_data.log_buffer_size = SCL_DDR_BOOT_LOG_BUF_SIZE;
  boot_log_data.meta_info_start = (void *)SCL_DDR_BOOT_LOG_META_INFO_BASE;
  
  
  /*Finally update the dload entry to sbl1 sahara dload entry function */
  sbl_dload_entry = sbl1_dload_entry;

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
*   This function updates the the boot crash information for simulator tool
*   and flushes the SBL regions to the allocated DDR region
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

      /* Update SBL DDR region meta data */
      boot_crash_dump_data.sbl_ddr_addr = SCL_SBL1_DDR_DATA_BASE;
      boot_crash_dump_data.sbl_ddr_size = SCL_SBL1_DDR_DATA_SIZE;
      boot_crash_dump_data.sbl_ddr_dump_addr = 
           SCL_SBL1_CRASH_DUMP_DATA_BASE + SCL_SBL1_CRASH_DUMP_DATA_SIZE;

      /* Update SBL L2 region meta data */
      boot_crash_dump_data.sbl_l2_addr = SCL_BOOT_L2_LOCK_BASE;
      boot_crash_dump_data.sbl_l2_size = SCL_BOOT_L2_LOCK_SIZE;
      boot_crash_dump_data.sbl_l2_dump_addr = 
         boot_crash_dump_data.sbl_ddr_dump_addr + boot_crash_dump_data.sbl_ddr_size;

      /* Flush PCIe core dump region 
        Note: Source location for PCIe dump data need not be stored as no need 
        to repopulate it later from dump location to source. A global variable
        in PCIe drivers keeps track of the data.

        Dumps only taken for PCIe flashless boot.
      */
      boot_crash_dump_data.sbl_pcie_coredump_addr = 
        boot_crash_dump_data.sbl_l2_dump_addr + boot_crash_dump_data.sbl_l2_size ;
  
      if (BOOT_PCIe_OPTION == sbl1_hw_get_boot_option())
      {
        boot_crash_dump_data.sbl_pcie_coredump_size = SCL_SBL_PCIE_COREDUMP_SIZE;
        boot_pcie_reg_dump((void *)boot_crash_dump_data.sbl_pcie_coredump_addr,
                            boot_crash_dump_data.sbl_pcie_coredump_size
                           );
      }
      else
      {
        boot_crash_dump_data.sbl_pcie_coredump_size = 0x0;
      }
      
      /* Ensure no more api get called beyond this point. So RW/ZI variables
         are consistent in dumps */
     
      /* Flush SBL DDR region */
      qmemcpy((void *)boot_crash_dump_data.sbl_ddr_dump_addr, 
              (const void *)boot_crash_dump_data.sbl_ddr_addr,
              boot_crash_dump_data.sbl_ddr_size);

      /* Flush SBL L2 region. */
      qmemcpy((void *)boot_crash_dump_data.sbl_l2_dump_addr, 
              (const void *)boot_crash_dump_data.sbl_l2_addr,
              boot_crash_dump_data.sbl_l2_size);
    
      /* Flush the cache */
      mmu_flush_cache();

      /* Check if QSEE has executed or if this is flashless boot */
      if (boot_get_qsee_execute_status() ||
          !booting_from_storage_media())
      {
        /* Set DLOAD cookies and reset the target so that we enter DLOAD mode 
           to get the RAM dumps */
        boot_dload_set_cookie();
        boot_hw_reset(BOOT_WARM_RESET_TYPE);
      }
   }
}

/*===========================================================================
**  Function :  sbl1_signal_qsee_sbl_done
** ==========================================================================
*/
/*!
* 
* @brief
*   This api makes a smc call to qsee to signal end of sbl boot.
*   QSEE will jump to the address passed in the second argument.
*   This function will not return!
*
* @param[in] bl_shared_data Pointer to shared data
*        
* @par Dependencies
*  none
*   
*/
void sbl1_signal_qsee_sbl_done( bl_shared_data_type *bl_shared_data )
{
  boolean recovery_mode = FALSE;
  uint32 entry_point;
  
  /* Pass on recovery mode status to TZ for appropriate Nand Mpu configs */
  if(booting_from_storage_media())
  { 
    /* Not applicable for Flashless boot */
    recovery_mode = boot_check_fota_partition(fota_partition_name);
  }
  
  /* Flush the cache before we exit */
  mmu_flush_cache();

  /* Get the APPSBL or APPS image entry point to pass to TZ
    * Note: both APPSBL and APPS images share this function
    * but only one of them is loaded
    */
  entry_point = boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_APPSBL_SW_TYPE);
  if (!entry_point)
    entry_point = boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_APPS_HASH_TABLE_TYPE);
  
  boot_fastcall_tz_2arg(TZ_SBL_END_MILESTONE, entry_point, recovery_mode);
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
**  Function :  sbl1_pagetable_dep_enable
** ==========================================================================
*/
/*!
* 
* @brief
*   Configure the page table to enable DEP(Data Execution Prevention) in sbl1 
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
static void sbl1_pagetable_dep_enable(bl_shared_data_type* bl_shared_data)
{
  boot_boolean result;
  
  uint32 rw_base = (uint32)&Image$$SBL1_DATA_RW$$Base;
  
 /* Data Execution Prevention(DEP) for SBL RW region
     All SBL region except code should be marked as non executable */
  struct mem_block sbl_rw_dep_mem_block = 
  {    
    /*All SBL1 rw region should be read/write and non-executable*/
    0, 0, 0,
    MMU_L2_NS_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_MEM_NON_CACHEABLE, MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN
  };

  struct mem_block sbl_code_dep_mem_block = 
  {    
    /*All SBL1 code region should be read only and executable */
    0, 0, 0,
    MMU_L2_NS_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_ONLY, 
    MMU_L2_PAGETABLE_MEM_NON_CACHEABLE, MMU_L2_PAGETABLE_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN
  };

  sbl_rw_dep_mem_block.p_base = rw_base;
  sbl_rw_dep_mem_block.v_base = rw_base;
  
  /*The size we need to map as non executable is from RW base to end of L2 lock region*/
  sbl_rw_dep_mem_block.size_in_kbytes = 
    (SCL_BOOT_L2_LOCK_BASE + SCL_BOOT_L2_LOCK_SIZE - rw_base) >> 10;
    
  sbl_code_dep_mem_block.p_base = ROUND_DOWN_TO_4KB_ALIGNED(SCL_SBL1_CODE_BASE);
  sbl_code_dep_mem_block.v_base = ROUND_DOWN_TO_4KB_ALIGNED(SCL_SBL1_CODE_BASE);
  sbl_code_dep_mem_block.size_in_kbytes = ROUND_UP_TO_4KB_ALIGNED(rw_base - SCL_SBL1_CODE_BASE) >> 10;
  
  /* Now populate the sbl DEP memory map block */
  result =
    boot_mmu_page_table_map_single_mem_block((uint32 *)
                                             bl_shared_data->sbl_shared_data->
                                             pbl_shared_data->pbl_page_tbl_base,
                                             &sbl_rw_dep_mem_block);                                              
  BL_VERIFY(result , BL_ERR_SBL);
  
  result =
    boot_mmu_page_table_map_single_mem_block((uint32 *)
                                             bl_shared_data->sbl_shared_data->
                                             pbl_shared_data->pbl_page_tbl_base,
                                             &sbl_code_dep_mem_block);
  BL_VERIFY(result , BL_ERR_SBL);
  
  /* PBL shared data region which is from SBL1 stack base to 
     the beginning of SBL1 code region,
     should also be configured the same as SBL1 RW region*/
  sbl_rw_dep_mem_block.p_base = (uint32)SCL_BOOT_L2_LOCK_BASE;
  sbl_rw_dep_mem_block.v_base = (uint32)SCL_BOOT_L2_LOCK_BASE;
  sbl_rw_dep_mem_block.size_in_kbytes = 
    ROUND_DOWN_TO_4KB_ALIGNED(SCL_SBL1_CODE_BASE - SCL_BOOT_L2_LOCK_BASE)>>10;  
    
  result =
    boot_mmu_page_table_map_single_mem_block((uint32 *)
                                             bl_shared_data->sbl_shared_data->
                                             pbl_shared_data->pbl_page_tbl_base,
                                             &sbl_rw_dep_mem_block);
  BL_VERIFY(result , BL_ERR_SBL);           

}


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
static void sbl1_populate_initial_mem_map(bl_shared_data_type* bl_shared_data)
{
  boot_boolean result;
  
   /* First populate the initial memory map list */    
  BL_VERIFY(bl_shared_data->sbl_shared_data->
            pbl_shared_data->pbl_page_tbl_base != NULL, BL_ERR_NULL_PTR);

 
  /* Initialize L2 page table pointer and calculate number of available
     L2 entries left. */
  boot_l2_page_table_init(bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base,
                          bl_shared_data->sbl_shared_data->mmu_pagetable_size);


  result =
    boot_mmu_page_table_map_mem_block_list(
                                           bl_shared_data->sbl_shared_data->mmu_l1_pagetable_base,
                                           sbl_initial_memory_map);
  BL_VERIFY(result , BL_ERR_SBL); 
  
  /* Now populate the sbl DEP memory map block */
  sbl1_pagetable_dep_enable(bl_shared_data);
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
**  Function :  sbl1_qfprom_test
** ==========================================================================
*/
/*!
* 
* @brief
*   This api invokes qfprom test framework
*
* @param[in] bl_shared_data Pointer to shared data
*        
* @par Dependencies
*  Must be invoked before sbl1_signal_tz_sbl_done
*   
*/
void sbl1_qfprom_test( bl_shared_data_type *bl_shared_data )
{
  /*Invoke qfprom test framework */
  boot_qfprom_test();
}

/*===========================================================================
**  Function :  sbl1_update_all_rollback_fuses
** ==========================================================================
*/
/*!
* 
* @brief
*  Blow version rollback fuses for all images.
*
* @par Dependencies
*  Has to execute *after* sbl1_rpm_sync api. 
*   
*/
void sbl1_update_all_rollback_fuses()
{
 
 /* Blow version rollback fuses for all images after RPM
    is ready to service requests to configure pmic power
    rails
  */
  boot_config_update_all_fuse_versions(SBL1_IMG, sbl1_config_table);

}

/*===========================================================================
**  Function :  sbl1_update_sbl1_rollback_fuse
** ==========================================================================
*/
/*!
* 
* @brief
*   Update the sbl1 roll back protection version fuse
*
*        
* @par Dependencies
*  Must be called after TZ is initialized
*   
*/
void sbl1_update_sbl1_rollback_fuse(bl_shared_data_type *bl_shared_data)
{
  BL_VERIFY(boot_rollback_update_fuse_version(SECBOOT_SBL1_SW_TYPE)== BL_ERR_NONE, 
            BL_ERR_CODE_ROLLBACK_VERSION_BLOW_FAIL);
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
**  Function :  boot_set_qsee_execute_status
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will set the flag to indicate if QSEE image has executed.
*
* @param[in] status Indicates whether QSEE image has executed and is ready.
*        
* @par Dependencies
*   Called after QSEE image has been loaded and executed.
* 
*/
void boot_set_qsee_execute_status(boolean status)
{
  qsee_is_ready = status;
}

/*===========================================================================
**  Function :  boot_get_qsee_execute_status
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will get the flag to indicate if TZ image has executed.
*
* @param[in] None
*        
* @par Dependencies
* 
*/
boolean boot_get_qsee_execute_status(void)
{
  return qsee_is_ready;
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


  /* Zero out QSEE and QHEE memory if needed */
  boot_qsee_cond_zero_peripheral_memory(sbl_secboot_verified_info);
}

/*===========================================================================
**  Function :  sbl1_qusb_al_do_fast_enum
** ==========================================================================
*/
/*!
* 
* @brief
*   SBL1 wrapper to check for QUSB fast enumeration 
*
* @param[in]
*   bl_shared_data           Pointer to shared data
*        
* @par Dependencies
*   must be called after sbl1_ddr_data_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_qusb_al_do_fast_enum(bl_shared_data_type *bl_shared_data)
{
  boot_log_message("sbl1_qusb_al_do_fast_enum, Start");
  boot_log_start_timer();
  
  /* Do fast enumeration */
  boot_qusb_al_do_fast_enum(QUSB_MAX_SPEED_HIGH);
  boot_log_stop_timer("sbl1_qusb_al_do_fast_enum, Delta"); 

}/* sbl1_qusb_al_do_fast_enum */

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
  uint32 result;
  uint32 sbl1_cert_img_version;
  bl_error_type status = BL_ERR_OTHERS;
  
  /* Calculate the SBL start time for use during boot logger initialization. */
  sbl_start_time = CALCULATE_TIMESTAMP(HWIO_IN(TIMETICK_CLK));

#ifdef SBL1_DISABLE_D_CACHE
  mmu_flush_cache_and_disable();
  mmu_enable_instruction_cache();
#endif
  
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
  
  /*------------------------------------------------------------------
   Initialize the RW-ZI region according to scatter load file.
  ------------------------------------------------------------------*/
  result = boot_ram_init(&sbl1_ram_init_data);
  BL_VERIFY( result , BL_ERR_SBL );  

  /* Initialize busywait module
     Note: This is required to be done before logger init due to uart driver
     dependency on busywait */
  BL_VERIFY(boot_busywait_init() == DAL_SUCCESS,BL_ERR_SBL);
  
  sbl1_boot_logger_init(&boot_log_data, pbl_shared);
  boot_log_set_meta_info(boot_log_data.meta_info_start);

  /*-----------------------------------------------------------------------
    Retrieve info passed from PBL
  -----------------------------------------------------------------------*/
  sbl1_retrieve_shared_info_from_pbl(pbl_shared);
    
  /* Initialize the QSEE interface */
  sbl1_init_sbl_qsee_interface(&bl_shared_data,
                               &sbl_verified_info);

  /* Initialize SBL memory map.
     Initializing early because drivers could be located in RPM Code RAM. */
  sbl1_populate_initial_mem_map(&bl_shared_data);

  /* Initialize the secboot functions */
  BL_VERIFY(boot_secboot_ftbl_init(&bl_shared_data) == BL_ERR_NONE, BL_ERR_IMG_SECURITY_FAIL);

  /* set hash algorithm */
  BL_VERIFY( (status = boot_set_hash_algo(SBL_HASH_SHA256)) == BL_ERR_NONE, status);
  
  /*Set hash update api to be non blocking*/
  boot_set_hash_update_mode(NON_BLOCK_MODE_ENABLE);

  /* Initialize dal heap using internal memory */
  boot_DALSYS_HeapInit(boot_internal_heap, BOOT_INTERNAL_HEAP_SIZE, FALSE);
   
  /*Initialize DAL, needs to be called before modules that uses DAL */  
  boot_DALSYS_InitMod(NULL); 
  
   /* Initialize roll back protection image table */
  boot_rollback_init_img_set_table(&bl_shared_data,
                                   &boot_rollback_version_img_set_table[0],
                                   boot_rollback_version_img_set_num);
                                   
   /* Update sbl1 roll back image version */
  sbl1_cert_img_version = sbl_verified_info.sw_id >> 32;
  boot_rollback_update_img_version(SECBOOT_SBL1_SW_TYPE, sbl1_cert_img_version);

   
  /*------------------------------------------------------------------
   Call sbl1_hw_init as soon as possible to config pmic device
   so we can use PS_HOLD to reset
  ------------------------------------------------------------------*/  
  sbl1_hw_init();

  /*-----------------------------------------------------------------------
   Add the SBL1 address range to the memory protection 
  -----------------------------------------------------------------------*/
  boot_clobber_add_global_protection_region( (void*) SBL1_CODE_BASE,
                                             SBL1_MAX_IMAGE_SIZE );

  boot_clobber_add_global_protection_region((void *)SCL_SBL1_DDR_DATA_BASE,
                                            SCL_SBL1_DDR_DATA_SIZE);

  boot_clobber_add_global_protection_region((void *)SCL_SYSTEM_DEBUG_BASE,
                                            SCL_SYSTEM_DEBUG_SIZE);
                                             
  boot_clobber_add_global_protection_region((void *)SCL_SHARED_RAM_BASE,
                                            SCL_SHARED_RAM_SIZE);

  boot_clobber_add_global_protection_region((void *)SCL_IMEM_BASE,
                                            SCL_IMEM_SIZE);

  /*-----------------------------------------------------------------------
    Process the target-dependent SBL1 procedures
  -----------------------------------------------------------------------*/
  boot_config_process_bl(&bl_shared_data, SBL1_IMG, sbl1_config_table);

} /* sbl1_main_ctl() */


