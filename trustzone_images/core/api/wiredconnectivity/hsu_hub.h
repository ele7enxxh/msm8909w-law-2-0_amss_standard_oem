#ifndef _HSU_HUB_H_
#define _HSU_HUB_H_

/*=============================================================================

                    HS-USB API to HUB Module Header file

=============================================================================*/
/**
@file hsu_hub.h

API definitions for hub interfacing with HS-USB.

There are no initialization or sequencing requirements.

The externalized functions in this module are:
- hsu_hub_enable()
- hsu_hub_disable()
*/
/*=============================================================================
Copyright (c) 2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_hub.h#1 $
$DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/25/10 lo   (Tech Pubs) Edited/added Doxygen comments and markup.
03/11/10 jch  Initial version

=============================================================================*/

/*=============================================================================
INCLUDE FILES FOR MODULE
=============================================================================*/

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
Forward Declarations
=============================================================================*/

/*=============================================================================
Constants
=============================================================================*/

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
FUNCTION    hsu_hub_enable
=============================================================================*/
/**
Enables the USB hub and switches that connect two or more chipsets' USB
ports.\ The device becomes a USB compound device.

This function should not be called for targets that do not have the hub.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_hub_enable(void);

/*==============================================================================
FUNCTION    hsu_hub_disable
==============================================================================*/
/**
Disables the USB hub and switches that connect two or more chipsets' USB
ports.\ Only the master chipset's USB port is routed to the USB connector.

This function should not be called for a target that does not have the hub.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_hub_disable(void);

/** @} */ /* end_group hsusb_qc_api */

#endif /* HSU_HUB_H */
