/*===========================================================================

                    BOOT EXTERN DEVICEPROGRAMMER DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for deviceprogrammer main function

INITIALIZATION AND SEQUENCING REQUIREMENTS

  
Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_deviceprogrammer_lite_interface.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who    what, where, why
--------   ---    ----------------------------------------------------------
12/17/14   sve    Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_extern_deviceprogrammer_lite_interface.h"
#include "boot_sbl_if.h"
#include "deviceprogrammer_entry_initialize.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_deviceprogrammer_lite_main

** ==========================================================================
*/
/*!
 * Run deviceprogrammer before ddr has been initialized.
 *
 * @param pbl_shared_data
 *    The shared bootloader information.
 *
 * @return
 *    This function never returns.
 *
*****************************************************************************/
#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_LITE_COMPLETED
void boot_hand_control_to_deviceprogrammer_lite_main
(
 boot_pbl_shared_data_type *pbl_shared_data
)
{
  deviceprogrammer_entry(pbl_shared_data);
  for (;;);
}
#endif
