/*===========================================================================

                    BOOT EXTERN BAM DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external api to initialize BAM interface
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_bam_interface.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/14/12   kedara   Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_extern_bam_interface.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/**
 * @brief Initialize the bam driver
 *
 * This function initializes the bam driver data structures. 
 * Clients of bam driver are not expected to call this function
 * directly. 
 *
 * @return Status of the operation.
 *
 */
bam_status_type boot_bam_drv_init(void)
{
  return bam_drv_init();
}
