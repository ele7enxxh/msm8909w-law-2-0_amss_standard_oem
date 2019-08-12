#ifndef __HSU_CHG_WALLCHG_
#define __HSU_CHG_WALLCHG_

/*=============================================================================

USB Wall Charger Detection API
    H E A D E R   F I L E
=============================================================================*/
/**
@file hsu_chg_wallchg.h

Contains the API definitions for detecting the presence of a USB wall
charger.

Externalized functions:
- hsu_chg_wallchg_init
- hsu_chg_wallchg_detect_wall_charger
- hsu_chg_wallchg_cancel_detection

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

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_chg_wallchg.h#1 $
$DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/24/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
12/03/07 esh  Initial version

=============================================================================*/
/*=============================================================================

INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
Typedefs
=============================================================================*/
/** Callback to be called on wall charger detection.
*/
typedef void (*hsu_chg_wallchg_detected_notif_fn)(boolean);

/*=============================================================================
  Constants and Macros
=============================================================================*/
/** HS-USB core index used for charging.\ No multiple cores supported.
*/
#define HSU_CHG_CORE_IDX (0)

/*=============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_chg_wallchg_init
=============================================================================*/
/**
Calls any initializations needed to enable USB wall charger detection. Must be
called prior to using any of the other APIs.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_chg_wallchg_init(void);

/*===========================================================================
FUNCTION    hsu_chg_wallchg_detect_wall_charger
===========================================================================*/
/**
Requests this module to detect the presence of a USB wall charger. Should be
called upon connection to an A-device.
A callback will be invoked once this module has an answer as to whether or not
a USB wall charger was detected.

@param detection_cb [IN] -- Callback to call on detection of a USB wall charger.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_chg_wallchg_detect_wall_charger(
  hsu_chg_wallchg_detected_notif_fn detection_cb);

/*=============================================================================
FUNCTION    hsu_chg_wallchg_cancel_detection
=============================================================================*/
/**
Requests this module to abort the current request to detect presence of a USB
wall charger. This usually occurs upon disconnection from an A-device. 

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_chg_wallchg_cancel_detection(void);

/*=============================================================================
FUNCTION    hsu_chg_wallchg_turn_on_dp_pullup
=============================================================================*/
/**
Sets the Run/Stop bit on the USBCMD ULPI register. Essentially, this enables
the HS-USB link to turn on the D+ pull-up.

@param turn_on [IN] -- If D+ pull-up should be turned on.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_chg_wallchg_turn_on_dp_pullup(boolean turn_on);

/*=============================================================================
FUNCTION    hsu_chg_wallchg_is_dp_pullup_on
=============================================================================*/
/**
Checks if the D+ pull-up is turned on.

This function inintally checks if the Run/Stop bit is set. If the Run/Stop bit
is set, the D+ will be pulled high when the USB is connected.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The Run/Stop bit is set, meaning D+ pull-up is turned on.\n
FALSE -- The Rnt/Stop bit is not set, meaning D+ pull-up is not turned on.
*/
boolean hsu_chg_wallchg_is_dp_pullup_on(void);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* __HSU_CHG_WALLCHG_ */
