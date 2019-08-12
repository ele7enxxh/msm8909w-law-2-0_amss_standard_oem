/*=============================================================================

                                 boot secure watchdog

GENERAL DESCRIPTION
  This file contains the function interface for the secure watchdog
  feature in Bear family chipsets.

Copyright  2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
when       who        what, where, why
--------   ---        ---------------------------------------------------------
08/04/14   moh        Removing code to ensure SW_ENTRY is not being set in SBL 
06/11/14   jz         Fixed boot_is_auth_enabled to check return status in the caller
04/25/14   ck         Added logic to only enable SDI on non-production devices
12/27/13   ck         Initial implimentation 
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_secure_watchdog.h"
#include "boot_authenticator.h"
#include "boot_extern_seccfg_interface.h"


/*============================================================================ 
                          PUBLIC FUNCTION DECLARATIONS
============================================================================*/


/*===========================================================================

**  Function :  boot_enable_secure_watchdog

** ==========================================================================
*/
/*!
* 
* @brief
*   Enables secure watchdog feature in Bear family chipsets.
*     Clears
*       RESET_ACCESS_FIRST_PASS
*       RESET_DEBUG_FIRST_PASS
*
*   Sets RESET_DEBUG_SW_ENTRY register to SBL Segment #2 (sys_debug) entry point
*
* @param[in]
*   bl_shared_data * - Pointer to SBL shared data structure
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
void boot_secure_watchdog_init(bl_shared_data_type * bl_shared_data)
{
  boot_boolean is_auth_enabled = FALSE;
  bl_error_type status = boot_is_auth_enabled(&is_auth_enabled);
  BL_VERIFY((status == BL_ERR_NONE), BL_ERR_IMG_SECURITY_FAIL); 

  /* Clear RESET_ACCESS_FIRST_PASS and RESET_DEBUG_FIRST_PASS */
  HWIO_GCC_RESET_DEBUG_OUTM(HWIO_GCC_RESET_DEBUG_RESET_ACCESS_FIRST_PASS_BMSK,
                            0);
  HWIO_GCC_RESET_DEBUG_OUTM(HWIO_GCC_RESET_DEBUG_RESET_DEBUG_FIRST_PASS_BMSK,
                            0);


  /* Set RESET_DEBUG_SW_ENTRY register with System Debug entry point */
  HWIO_TCSR_RESET_DEBUG_SW_ENTRY_OUT(SCL_SYSTEM_DEBUG_BASE &
                                     ~RESET_DEBUG_SW_ENTRY_ENABLE);

}
