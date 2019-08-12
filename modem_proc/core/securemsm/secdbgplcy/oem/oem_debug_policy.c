/*===========================================================================

                     S E C U R I T Y    S E R V I C E S

                S E C U R E  B O O T   X.5 0 9  P A R S E R

                               M O D U L E

FILE:  debug_policy.c

DESCRIPTION:
  Debug policy implementation

EXTERNALIZED FUNCTIONS


Copyright (c) 2013-2014 by Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================*/

/*=========================================================================

                          EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secdbgplcy/oem/oem_debug_policy.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when         who                what, where, why
--------   ----               ------------------------------------------- 
05/05/14   st                  Initial version
===========================================================================*/


/*==========================================================================

           Include Files for Module

==========================================================================*/
#include <comdef.h>

/* Serial Number Flag */
//#define SKIP_SERIAL_NUMBER_CHECK 1

boolean is_sec_dbg_skip_serial_number(void)
{

#ifdef SKIP_SERIAL_NUMBER_CHECK
  return TRUE;
#else
 return FALSE;
#endif

}
