/*=============================================================================
                           
                          Boot Device Singleton

GENERAL DESCRIPTION
  Singleton for the boot device interface.

Copyright 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_flash_dev_if.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/04/08   MJS     Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_flash_dev_if.h"

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/* Define the singleton */
BOOT_SINGLETON_DEFINE(boot_flash_dev_if);

