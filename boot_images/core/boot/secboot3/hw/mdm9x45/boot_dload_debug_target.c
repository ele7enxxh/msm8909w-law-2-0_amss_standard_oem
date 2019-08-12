/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Boot Dload Debug Target File

GENERAL DESCRIPTION
  This module defines the target specific memory debug information to be sent
  to host.
  Please make sure that dump region file name follows the 8.3 file name format!

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 2010-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/mdm9x45/boot_dload_debug_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   kedara  Added PMIC_PON.BIN region to ramdumps, Moved reset_status_register 
                   and pon_status to BOOT_UNCACHED_DDR_ZI_ZONE
10/28/13   kedara  Added boot_dload_clock_init
06/19/13   kedara  Initial revision

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_dload_debug.h"
#include "boot_ddr_info.h"
#include "boot_target.h"
#include "boot_shared_imem_cookie.h"
#include "sbl1_hw.h"
#include "boot_extern_clk_interface.h"
#include "boot_extern_pmic_interface.h"

#ifdef FEATURE_DLOAD_MEM_DEBUG

/* Declare rst_status and pon_status in BOOT_UNCACHED_DDR_ZI_ZONE */
__attribute__((section("BOOT_UNCACHED_DDR_ZI_ZONE"), zero_init)) static uint64 pmic_power_on; 
__attribute__((section("BOOT_UNCACHED_DDR_ZI_ZONE"), zero_init)) static uint32 reset_status_register;


/*===========================================================================

**  Function :  dload_add_pmic_info

** ==========================================================================
*/
/*!
* 
* @brief
*   Add the Reset Status register region
*   
* @par Dependencies
*   This api Must be called after clk_regime_apps_init_boot 
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
static void dload_add_pmic_info(void)
{
  /* Get power on reason for pmic device 0 */
  boot_pm_dev_get_power_on_reason(0, &pmic_power_on);

  dload_debug_add_region(OPTIONAL_DEF_SAVE,
                          (uint32)(&pmic_power_on),
                          (uint32) sizeof(pmic_power_on),
                          "Pmic PON stat", "PMIC_PON.BIN");
}

/*===========================================================================

**  Function :  dload_add_reset_status_region

** ==========================================================================
*/
/*!
* 
* @brief
*   Add the Reset Status register region
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
static void dload_add_reset_status_region(void)
{
  reset_status_register = sbl1_hw_get_reset_status();
  dload_debug_add_region(OPTIONAL_DEF_SAVE,
                          (uint32)(&reset_status_register),
                          (uint32) sizeof(reset_status_register),
                          "Reset Status Region", "RST_STAT.BIN");
}


/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_TARGET_INIT

DESCRIPTION
  This function initializes clocks as needed for this target, as well as sets
  up the memory regions to be used.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/                                                                          
void dload_mem_debug_target_init(void)
{

  uint32 start_addr =0, start_addr_cs1 =0;
  uint32 size =0, size_cs1 =0;
  uint32 index =0, index_cs1 =0;
  sbl_if_shared_ddr_device_info_type *available_ddr;

  struct memory_region 
  { 
      uint32 region_base;
      uint32 region_size;
      char* desc;
      char* filename;
  };
  
  struct memory_region dump_regions[] = {MEMORY_REGION_TABLE};

  /*Enable Clocks needed to access memory regions */
  boot_dload_clock_init();

  /* Set memory region table to be fixed length, required by sahara*/
  dload_mem_debug_len_init();


  /*
  ********************************WARNING**************************************
 
  Please make sure all dump region file names follow the 8.3 file name format!
 
  *****************************************************************************
  */

  index = 0;

  /* RAM-DUMP table defined in .builds file */
  while ( dump_regions[index].region_base != NULL ) 
  {
      dload_debug_add_region(OPTIONAL_DEF_SAVE, 
                             dump_regions[index].region_base,
                             dump_regions[index].region_size,
                             dump_regions[index].desc,
                             dump_regions[index].filename
                             );
      index++;
  }  

 /* Add Pmic power on reason*/
  dload_add_pmic_info(); 

  /* Reset Status register  */
  dload_add_reset_status_region();
    
  /* Add DDR region */
  index = 0;
  available_ddr = boot_get_ddr_info();
  while(index < available_ddr->noofddr)
  {
     start_addr = available_ddr->ddr_info[index].cs_addr;
     size = available_ddr->ddr_info[index].ramsize << CONVERT_TO_MB_SHIFT;

      /* Define EBI1 CS0 Memory Region */
    if(start_addr == DDR_MEM_BASE)
    {
      dload_debug_add_region(OPTIONAL_DEF_SAVE, start_addr, size, 
                     " DDR CS0 Memory", "DDRCS0.BIN");
      /*Check if EBI1 CS1 exists */
      index_cs1 =0;
      while(index_cs1 < available_ddr->noofddr)
      {
        start_addr_cs1 = available_ddr->ddr_info[index_cs1].cs_addr;	  
        if(start_addr_cs1 == (start_addr + size))
        {  
        size_cs1 = available_ddr->ddr_info[index_cs1].ramsize << CONVERT_TO_MB_SHIFT;
        /*Add EBI1 CS1 Region*/        
        dload_debug_add_region(OPTIONAL_DEF_SAVE, start_addr_cs1, size_cs1, 
                             " DDR CS1 Memory ", "DDRCS1.BIN");
        }     
        index_cs1++;          
      }       
    }    
   index++;
  } 
}


#endif /*FEATURE_DLOAD_MEM_DEBUG*/
