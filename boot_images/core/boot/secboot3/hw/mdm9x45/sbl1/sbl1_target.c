/*=============================================================================

                               SBL1 Target

GENERAL DESCRIPTION
  Target-specific initialization and interface implementation.
  
Copyright 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/mdm9x45/sbl1/sbl1_target.c#1 $

when       who      what, where, why
--------   ---      ------------------------------------------------------------
01/28/15   jz       Updated to use SECBOOT_FLASH_PRG_SW_TYPE for secboot support via QPST
11/03/14   jz       Updated to support HOSTDL in ELF format
06/06/14   ck       Updated NO_FLASH conditionals to use booting_from_storage_media
06/19/13   kedara   Initial version.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_dload.h"
#include "boot_sahara.h"
#include "boot_cache_mmu.h"
#include "boot_mc.h"
#include "boot_sbl_if.h"
#include "miheader.h"
#include "mibib.h"
#include "boot_loader.h"
#include "boot_util.h"
#include "boot_page_table_armv7.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
extern boot_procedure_func_type sbl1_pre_dload_procs[];

/* Shared data between SBL */
extern struct bl_shared_data_type bl_shared_data;

#pragma arm section zidata = "BOOT_DDR_ZI_DATA_ZONE"
  mi_boot_image_header_type dload_header_ptr;
#pragma arm section zidata

/*===========================================================================

**  Function :  sbl_dload_entry

** ==========================================================================
*/
/*!
* 
* @brief
*   This function pointer is defined in each SBL* Bootloader to handle SBL-specific
*   requirements to enter a download routine. It is initialized to 
*   boot_dload_transition_pbl_forced_dload by default.
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
void (*sbl_dload_entry)(void) = boot_dload_transition_pbl_forced_dload; 


/*===========================================================================

**  Function :  boot_dload_transition_enter_sahara

** ==========================================================================
*/
/*!
* 
* @brief
*   This function is implemented in each SBL to enter sahara dload mode
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
void boot_dload_transition_enter_sahara(void)
{
  void  (* jump_point)(void) = NULL ;  /* Hostdl entry point */
  static boolean dload_set_cookie_ran_once = FALSE;
  boot_boolean result;

  struct mem_block hostdl_mem_block = 
  {
    /* Base address updated later. Size assumed to be max 2MB */
    NULL, NULL, 0x200000>>10,
    MMU_L2_SECTION_MAPPING, MMU_L2_PAGETABLE_MEM_READ_WRITE, 
    MMU_L2_PAGETABLE_DEVICE_SHARED, MMU_L2_PAGETABLE_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN
  };

  /* Get Sahara interface */
  struct boot_sahara_interface* sbl_sahara_interface = sbl_sahara_get_interface();
  BL_VERIFY(sbl_sahara_interface != NULL, BL_ERR_NULL_PTR);

  if(booting_from_storage_media())
  {  
    /*Support Hostdl only for nand boot */
    
    /* The dload magic numbers will be cleared by this time 
    Set them again so as so retain the memory dump mode in
    sbl_sahara_entry. Set it only once to avoid multiple
    ramdumps in case we end up here again. 
    */
    if(dload_set_cookie_ran_once == FALSE)
    {
      boot_dload_set_cookie();
      dload_set_cookie_ran_once = TRUE;
    }

    /* Prepare for receiving hostdl image */
    sbl_sahara_interface->sahara_shared_data->image_rx_complete = FALSE;
  
    /* Enter sahara to take ram dumps. Load Hostdl image if sent by host*/
    sbl_sahara_entry(&dload_header_ptr, 
                     (uint32) HOSTDL_IMG, 
                     SAHARA_IMAGE_TYPE_ELF,
                     (uint32)SECBOOT_FLASH_PRG_SW_TYPE,
                     TRUE,  /* Auth Flag */
                     FALSE, /* Jump Flag not set to retain ram dump mode */
                     &bl_shared_data);  
  
    if (sbl_sahara_interface->sahara_shared_data->image_rx_complete == TRUE)
    {
      /* Mark hostdl region as executable */
      hostdl_mem_block.p_base = boot_sahara_get_elf_entry_address();

      /*Align on 1 MB boundary */
      hostdl_mem_block.p_base = ((hostdl_mem_block.p_base >> 20)<<20);
      hostdl_mem_block.v_base = hostdl_mem_block.p_base ;

      BL_VERIFY(( (bl_shared_data.sbl_shared_data != NULL) &&
        ((bl_shared_data.sbl_shared_data)->mmu_l1_pagetable_base != NULL)), 
        BL_ERR_NULL_PTR_PASSED );

      result = boot_mmu_page_table_map_single_mem_block
      ((bl_shared_data.sbl_shared_data)->mmu_l1_pagetable_base, &hostdl_mem_block); 
  
      BL_VERIFY(result, BL_ERR_SBL);      
      
      jump_point = (void (*)(void))(boot_sahara_get_elf_entry_address());

      /*Jump to Hostdl image */
      jump_point();
    }
  }
  else
  {
    /* Dload in Flashless boot use case */
  
    /* Set Sahara mode to memory debug */
    sbl_sahara_interface->sahara_mode = SAHARA_MODE_MEMORY_DEBUG;
  
    /* Flush the cache before calling into sahara so that all data is flushed to memory */  
    mmu_flush_cache();
  
    /* Enter Sahara */
    boot_sahara_entry(sbl_sahara_interface);
  }
  
}


/*===========================================================================

**  Function :  sbl_dload_target_entry

** ==========================================================================
*/
/*!
* 
* @brief
*   This function is defined in each SBL Bootloader to handle SBL-specific
*   requirements to enter a download routine
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
void sbl1_dload_entry ()
{
  static uint32 dload_entry_count = 0;

  dload_entry_count++; 

  /* Only execute the pre-dload procedures the first time we try to enter
   * dload in case there is an error within these procedures. */
  if( dload_entry_count == 1 && &bl_shared_data != NULL )
  {
    /* Entering dload routine for the first time */
    boot_do_procedures( &bl_shared_data, sbl1_pre_dload_procs );
  }
  
  /* Enter boot Sahara */
  boot_dload_transition_enter_sahara();
  
}/* sbl_dload_entry() */
