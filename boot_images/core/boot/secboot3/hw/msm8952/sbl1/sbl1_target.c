/*=============================================================================

                               SBL1 Target

GENERAL DESCRIPTION
  Target-specific initialization and interface implementation.
  
Copyright 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/msm8952/sbl1/sbl1_target.c#1 $

when       who      what, where, why
--------   ---      ------------------------------------------------------------
10/31/11   dh       Add sbl1_dload_entry.
09/29/11   dh       Initial revision
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_dload.h"
#include "boot_sahara.h"
#include "boot_cache_mmu.h"
#include "boot_mc.h"
#include "boot_sbl_shared.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
extern boot_procedure_func_type sbl1_pre_dload_procs[];

/* Shared data between SBL */
extern struct bl_shared_data_type bl_shared_data;

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
  /* Get Sahara interface */
  struct boot_sahara_interface* sbl_sahara_interface = sbl_sahara_get_interface();
  BL_VERIFY(sbl_sahara_interface != NULL, BL_ERR_NULL_PTR);
  
  /* Set Sahara mode to memory debug */
  sbl_sahara_interface->sahara_mode = SAHARA_MODE_MEMORY_DEBUG;
  
  /* Flush the cache before calling into sahara so that all data is flushed to memory */  
  mmu_flush_cache();
  
  /* Enter Sahara */
  boot_sahara_entry(sbl_sahara_interface);
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
