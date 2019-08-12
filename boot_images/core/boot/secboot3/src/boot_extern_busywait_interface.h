#ifndef BOOT_EXTERN_BUSYWAIT_INTERFACE_H
#define BOOT_EXTERN_BUSYWAIT_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN BUSYWAIT DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external busy wait drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_busywait_interface.h#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/12   kpa     Make boot_busywait_init return DAL_SUCCESS  
09/08/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "busywait.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_busywait_init

** ==========================================================================
*/
/*!
 *This function initializes the busywait functionality..
 */
#ifdef FEATURE_BOOT_EXTERN_BUSYWAIT_COMPLETED
  DALResult boot_busywait_init(void);
#else
  static inline DALResult boot_busywait_init(void)
  {
    return DAL_SUCCESS;
  }
#endif

/*===========================================================================

**  Function :  boot_busywait

** ==========================================================================
*/
/*!
  * This function pauses the execution of a thread for a specified time.
 */
#ifdef FEATURE_BOOT_EXTERN_BUSYWAIT_COMPLETED
  void boot_busywait(uint32 pause_time_us);
#else
  static inline void boot_busywait(uint32 pause_time_us)
  {
  }
#endif


#endif /* BOOT_EXTERN_SMEM_INTERFACE_H */

