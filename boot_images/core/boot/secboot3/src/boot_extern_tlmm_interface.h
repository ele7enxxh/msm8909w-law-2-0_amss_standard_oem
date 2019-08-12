#ifndef BOOT_EXTERN_TLMM_INTERFACE_H
#define BOOT_EXTERN_TLMM_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN TLMM DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external tlmm drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011,2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_tlmm_interface.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/12   kpa     Change boot_gpio_init return type to True.
10/19/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

  FUNCTION GPIO_INIT

  DESCRIPTION
        This function initializes the GPIOs for the TLMM configuration
        specified at boot time.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    It uses the stack

===========================================================================*/
#ifdef FEATURE_BOOT_EXTERN_TLMM_COMPLETED
  boolean boot_gpio_init(void);
#else
  static inline boolean boot_gpio_init(void)
  {
    return TRUE;
  }
#endif

#endif /* BOOT_EXTERN_TLMM_INTERFACE_H */
