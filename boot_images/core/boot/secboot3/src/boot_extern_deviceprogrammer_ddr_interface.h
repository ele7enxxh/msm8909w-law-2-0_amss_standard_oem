#ifndef BOOT_EXTERN_DEVICEPROGRAMMER_INTERFACE_H
#define BOOT_EXTERN_DEVICEPROGRAMMER_INTERFACE_H

/*===========================================================================

                    BOOT EXTERN DEVICEPROGRAMMER DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for deviceprogrammer main function

INITIALIZATION AND SEQUENCING REQUIREMENTS
  DDR must be initialized
  
Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_deviceprogrammer_ddr_interface.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/14   sb      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_sbl_if.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_deviceprogrammer_ddr_main

** ==========================================================================
*/
/*!
 * Run deviceprogrammer after ddr has been initialized.
 *
 * @param bl_shared_data
 *    The shared bootloader information.
 *
 * @return
 *    This function never returns.
 *
*****************************************************************************/
#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_DDR_COMPLETED
  void boot_hand_control_to_deviceprogrammer_ddr_main
  (
    bl_shared_data_type *bl_shared_data
  ) __attribute((noreturn));

#else
  static inline void boot_hand_control_to_deviceprogrammer_ddr_main
  (
    bl_shared_data_type *bl_shared_data
  )
  {
    //do nothing
  }
#endif

#endif  /* BOOT_EXTERN_DEVICEPROGRAMMER_INTERFACE_H */
