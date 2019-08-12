#ifndef BOOT_ARM_V7_H
#define BOOT_ARM_V7_H


/*=============================================================================

                    Boot arm v7 function header File

GENERAL DESCRIPTION
  This header file contains the public interface for arm v7 functions
          
Copyright 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_armv7.h#1 $    

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/11   dh      Initial Creation.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_comdef.h"


/*===========================================================================

**  Function :  boot_check_sec_mode

** ==========================================================================
*/
/*!
* 
* @brief
*   This function checks the processor is in secure mode or not
* 
* @par Dependencies
*   None
*
* @retval
*   TRUE if processor is in secure mode
*   FALSE if processor is in non secure mode
* 
* @par Side Effects
*   None
* 
*/
boot_boolean boot_check_sec_mode(void);

        
#endif /* BOOT_ARM_V7_H */
