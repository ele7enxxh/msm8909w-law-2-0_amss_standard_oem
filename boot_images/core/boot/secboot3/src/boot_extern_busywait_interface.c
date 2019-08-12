/*===========================================================================

                    BOOT EXTERN BUSYWAIT DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external busywait drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_extern_busywait_interface.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/29/12   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_extern_busywait_interface.h"
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
DALResult boot_busywait_init(void)
{
  return busywait_init();
}


/*===========================================================================

**  Function :  boot_busywait

** ==========================================================================
*/
/*!
  * This function pauses the execution of a thread for a specified time.
 */
void boot_busywait(uint32 pause_time_us)
{
  busywait(pause_time_us);
}
