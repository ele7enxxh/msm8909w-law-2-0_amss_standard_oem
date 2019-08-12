#ifndef TZAPPS_TARGET_COMMON_H
#define TZAPPS_TARGET_COMMON_H

/*===========================================================================

                    T r u s t z o n e   T a r g e t
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the TZAPPS
  that is target-specific

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2013 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/tzdrm/tzcommon/common/chipset/msm8909/inc/tzapps_target.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/15/13   cz      Increased the Widevine HEAP size
12/18/12   cz      Removed drmprov
12/18/12   rk      Fixed playdyn issue (CR# 433618)
10/16/12   ib      Remove the tz apps code base and region size definitions since TZ applications are now re-locatable.
08/29/12   rk      Changed tzapp code base and playready segment sizes
05/30/12   cz      Initial version
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "customer.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
  Define the destination address and size of PLAYREADY.
---------------------------------------------------------------------------*/
#define PLAYREADY_STACK_SIZE    0x10000
#define PLAYREADY_HEAP_SIZE     0xC9000

/*---------------------------------------------------------------------------
  Define the destination address and size of  WIDEVINE.
---------------------------------------------------------------------------*/
#define WIDEVINE_STACK_SIZE     0x1000
#define WIDEVINE_HEAP_SIZE      0x15000

/*---------------------------------------------------------------------------
  Define the destination address and size of  ISDBTMM.
---------------------------------------------------------------------------*/
#define ISDBTMM_STACK_SIZE      0x1000
#define ISDBTMM_HEAP_SIZE       0x20000


#endif  /* TZAPPS_TARGET_H */
