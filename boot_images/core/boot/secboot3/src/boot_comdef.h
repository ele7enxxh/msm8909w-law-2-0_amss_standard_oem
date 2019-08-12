#ifndef BOOT_COMDEF_H
#define BOOT_COMDEF_H

/*===========================================================================

                 Boot Common Definition Header File

GENERAL DESCRIPTION
  This header file contains common declarations and definitions used
  throughout the boot software.
   
Copyright 2004-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_comdef.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/10   as      Removed definitions as they are defined in comdef.h
12/04/09   as      Added boot_boolean in !BUILD_BOOT_CHAIN 
11/20/09   as      Add boot_boolean as uint32 boolean data type
07/29/09   dk      Added #define _BOOLEAN_DEFINED to avoid mismatch with 
                   other comdef file inclusions.
01/07/08   MJS     Merged 6xxx and 7xxx versions.
04/11/05   SJD     Changed to only define LOCAL if it isn't already defined.
08/25/04   SJD     Initial revision.

===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* boot_boolean data type used for boolean variables shared with pbl*/
typedef  unsigned int       boot_boolean;     /* uint32 Boolean value type. */

#endif /* BOOT_COMDEF_H */
