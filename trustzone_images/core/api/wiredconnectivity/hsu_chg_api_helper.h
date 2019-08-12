#ifndef _HSU_CHG_API_HELPER_H_
#define _HSU_CHG_API_HELPER_H_

/*=============================================================================

Charging Module API Helper to Interface with the High Speed USB
H E A D E R   F I L E
=============================================================================*/
/**
@file hsu_chg_api_helper.h

Contains the API definitions for responding to HS-USB state changes 
(e.g., connect, disconnect, suspend, and resume) needed to allow battery 
charging via HS-USB.

Externalized functions:
- hsu_chg_api_helper_suspended
- hsu_chg_api_helper_resumed
- hsu_chg_api_helper_is_resumed

*/
/*=============================================================================
Copyright (c) 2007-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_chg_api_helper.h#1 $
$DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/23/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
05/25/07 esh  Initial version

=============================================================================*/

/*=============================================================================

INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h" /* For boolean */

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
Forward Declarations
=============================================================================*/

/*=============================================================================
Constants
=============================================================================*/

/** According to the USB 2.0 Specification, section 7.2.1, a device must draw
    no more than one unit load (100 mA) upon powerup prior to being configured.
*/
#define HSU_CHG_API_HELPER_POWER_UNCONFIGURED (100) 

/*=============================================================================
Macros
=============================================================================*/

/*=============================================================================
Typedefs
=============================================================================*/

/*=============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_chg_api_helper_suspended
=============================================================================*/
/**
Notifies the charging module when a device has been suspended.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_chg_api_helper_suspended
(
  void
);

/*=============================================================================
FUNCTION hsu_chg_api_helper_resumed
=============================================================================*/
/**
Notifies the charging module when a device has been resumed.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_chg_api_helper_resumed
(
  void
);

/*=============================================================================
FUNCTION hsu_chg_api_helper_is_resumed
=============================================================================*/
/**
Returns whether or not the device is resumed.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- Device was issued a USB Resume.\n
FALSE -- Device was not issued a USB Resume.

*/
boolean hsu_chg_api_helper_is_resumed
(
  void
);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_CHG_API_HELPER_H_ */
