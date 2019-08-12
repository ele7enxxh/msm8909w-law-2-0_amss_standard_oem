#ifndef CUSTMSUITE_H
#define CUSTMSUITE_H
/*===========================================================================

DESCRIPTION
  Configuration for Mobile Suite.

  Copyright (c) 2002  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/build/cust/custmsuite.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/27/06   gfr     Remove Teleca browser
09/25/02   lz      Initial version.
===========================================================================*/

/*---------------------------------------------------------------------------
                         Wireless Messaging Services
---------------------------------------------------------------------------*/

/* Main feature flag
*/
#define FEATURE_SMS_LISTENER      /* SMS Listener app */

/* Used by MobileSuite to determine if size_t is defined on the platform
 * it is ported to. 
*/
#define _SIZE_T_DEFINED

#endif /* CUSTMSUITE_H */
