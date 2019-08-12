/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Boot Dload Debug Target File

GENERAL DESCRIPTION
  This module defines the target specific memory debug information to be sent
  to host.
  Please make sure that dump region file name follows the 8.3 file name format!

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/msm8952/boot_dload_debug_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/14   mva     Added PMIC_RTC region to RAM dump
04/29/14   ck      Adding zero_init attribute on small variables that must be in ZI region
04/02/14   ck      Added RPM MSG RAM as a restricted region
03/17/14   ck      Added call to dload_mem_debug_zero_peripheral_memory in retail unlocked scenario
01/10/14   ck      Added support for retail unlock dumps
06/13/13   dh      Add dload_add_ddr_training_data to dump ddr training data
06/06/13   dh      Call sbl1_hw_get_reset_status to read the backed up copy of
                   gcc reset status

02/11/13   dh      Move dload_mem_debug_supported to common code
01/28/13   dh      Add DDR as the last region in memory debug table
09/20/12   dh      Add Pmic power on reason and reset status to ram dump
07/17/12   dh      Remove reset status region
05/23/11   dh      call dload_mem_debug_len_init in dload_mem_debug_target_init to
                   set memory region table to be fixed length, required by sahara ram dump
04/05/11   dh      do not include customer.h
09/29/11   dh      Initial revision

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_dload_debug.h"
#include "boot_ddr_info.h"
#include "boot_target.h"
#include "boot_shared_imem_cookie.h"
#include "boot_extern_pmic_interface.h"
#include "boot_msm.h"


extern uint32 sbl1_hw_get_reset_status(void);
extern void sbl1_load_ddr_training_data_from_partition(uint8*, uint32);
extern uint32 sbl1_hw_get_pmictimer_status(void);

#ifdef FEATURE_DLOAD_MEM_DEBUG


/*===========================================================================

**  Function :  dload_add_ddr_training_data_info

** ==========================================================================
*/
/*!
* 
* @brief
*   Add the ddr training data region, ddr training data is loaded from ddr 
*   training data partition
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
static void dload_add_ddr_training_data(void)
{
  /* Buffer to store ddr training data */
  static uint8 ddr_training_data_dump[SCL_DDR_TRAINING_DATA_BUF_SIZE];
  
  /* Load SCL_DDR_TRAINING_DATA_BUF_SIZE of data from the training partition*/
  sbl1_load_ddr_training_data_from_partition(ddr_training_data_dump,
                                             SCL_DDR_TRAINING_DATA_BUF_SIZE);
  
  dload_debug_add_region(OPTIONAL_DEF_SAVE,
                          (uint32)ddr_training_data_dump,
                          SCL_DDR_TRAINING_DATA_BUF_SIZE,
                          "DDR Training Data", "DDR_DATA.BIN");  


}



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
  // Dump variable must be declared as static and have zero_init attribute to
  // be added to boot DDR region.  USB controller can not access locked 
  // L2/TCM region  
  __attribute__((section("BOOT_UNCACHED_DDR_ZI_ZONE"), zero_init))
    static uint64 pmic_power_on;
  
  /* Get power on reason for pmic device 0 */
  boot_pm_dev_get_power_on_reason(0, &pmic_power_on);

  dload_debug_add_region(OPTIONAL_DEF_SAVE,
                          (uint32)(&pmic_power_on),
                          (uint32) sizeof(pmic_power_on),
                          "Pmic PON stat", "PMIC_PON.BIN");
}


/*===========================================================================

**  Function :  dload_add_reset_status

** ==========================================================================
*/
/*!
* 
* @brief
*   Add the Reset Status register
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
static void dload_add_reset_status(void)
{
  // Dump variable must be declared as static and have zero_init attribute to
  // be added to boot DDR region.  USB controller can not access locked 
  // L2/TCM region  
  __attribute__((section("BOOT_UNCACHED_DDR_ZI_ZONE"), zero_init))
    static uint32 reset_status_register;
  
  reset_status_register = sbl1_hw_get_reset_status();
  dload_debug_add_region(OPTIONAL_DEF_SAVE,
                          (uint32)(&reset_status_register),
                          (uint32) sizeof(reset_status_register),
                          "Reset Status Region", "RST_STAT.BIN");
}

/*===========================================================================

**  Function :  dload_add_pmic_rtc_value

** ==========================================================================
*/
/*!
* 
* @brief
*   Store the PMIC RTC value in DDR
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
static void dload_add_pmic_rtc_value(void)
{
  __attribute__((section("BOOT_UNCACHED_DDR_ZI_ZONE"), zero_init))
  static uint32 pmic_rtc_value;
  
  pmic_rtc_value = sbl1_hw_get_pmictimer_status();
  dload_debug_add_region(OPTIONAL_DEF_SAVE,
                          (uint32)(&pmic_rtc_value),
                          (uint32) sizeof(pmic_rtc_value),
                          "Pmic RTCstat", "PMIC_RTC.BIN");

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


  /* Set memory region table to be fixed length, required by sahara*/
  dload_mem_debug_len_init();


  /* Check if device is retail unlocked. Do not dump internal memories 
     in retail unlock scenario.  Also zero peripheral memory so it does not
     appear in DDR dump. */
  if (dload_mem_debug_is_device_retail_unlocked()) 
  {
    dump_regions[0].region_base = NULL;
    dump_regions[0].region_size = NULL;
    dump_regions[0].desc = NULL;
    dump_regions[0].filename = NULL;

    dload_mem_debug_zero_peripheral_memory();
  }
  

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

  /* Add RPM MSG RAM as a restricted region as USB controller cannot access it */
  dload_debug_add_restricted_region(SCL_RPM_MSG_RAM_BASE,
                                    SCL_RPM_MSG_RAM_SIZE);

  /* Add Pmic power on reason*/
  dload_add_pmic_info();  

  /* Add reset status */ 
  dload_add_reset_status();  
  
  /* Add PMIC RTC value */
  dload_add_pmic_rtc_value();
  
  /* Add ddr training data */
  dload_add_ddr_training_data();
  
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
