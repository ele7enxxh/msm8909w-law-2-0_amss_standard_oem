#ifndef SBL1_TARGET_H
#define SBL1_TARGET_H

/*===========================================================================

                             SBL1 Target Header File

GENERAL DESCRIPTION
  Target-specific initialization and interface APIs.
  
Copyright 2010-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/mdm9x45/sbl1/sbl1_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/13   kedara  Initial version.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_target.h"

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
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
void sbl1_dload_entry(void);


#endif  /* SBL1_TARGET_H */
