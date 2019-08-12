#ifndef _HSU_BOOT_H_
#define _HSU_BOOT_H_

/*=============================================================================

                    HS-USB API to BOOT Module Header file
=============================================================================*/
/**
@file hsu_boot.h

Contains the API definitions for Boot to allow interfacing with HS-USB.

Externalized functions:
- hsu_boot_start_stack
- hsu_boot_stop_stack
- hsu_boot_poll_stack
- hsu_boot_disconnect_from_host
*/
/*
Initialization and sequencing requirements:
None.
*/
/*=============================================================================
Copyright (c) 2007-20010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_boot.h#1 $
$DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/24/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
12/27/07 esh  Initial version

=============================================================================*/
/*=============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "comdef.h" /* For boolean */

/** @ingroup hsusb_qc_api
    @{
*/

/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
Constants
==============================================================================*/

/*==============================================================================
Macros
==============================================================================*/

/*==============================================================================
Typedefs
==============================================================================*/
/** Enumeration of boot poll results. 
*/
typedef enum
{
  HSU_BOOT_POLL_RESULT_ERROR = -1,             /**< &nbsp; */
  HSU_BOOT_POLL_RESULT_NO_NEW_EVENT,
  /**< -- No change in USB state resulted from polling. */
  HSU_BOOT_POLL_RESULT_DISCONNECTED,
  /**< -- USB cable disconnected. */
  HSU_BOOT_POLL_RESULT_CONNECTED_TO_WALL_CHG,
  /**< -- USB cable connected to wall charger. */
  HSU_BOOT_POLL_RESULT_CONNECTED_TO_HOST_PC,
  /**< -- USB cable connected to a host PC. */
  HSU_BOOT_POLL_RESULT_NOT_CONNECTED,
  /**< -- USB cable is not connected. */
  HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED,
  /**< -- Device is resumed, but not yet enumerated.\ When resuming after
          enumeration, the enumerated event is raised to the boot calling
          function. */
  HSU_BOOT_POLL_RESULT_ENUMERATED,
  /**< -- Device was successfully enumerated. */
  HSU_BOOT_POLL_RESULT_SUSPENDED,
  /**< -- Device is suspended. */
  HSU_BOOT_POLL_RESULT_MAX                     /**< &nbsp; */
} hsu_boot_poll_result;

/*=============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/
/*=============================================================================
FUNCTION    hsu_boot_start_stack
=============================================================================*/
/**
Initializes and starts the HS-USB stack under boot.

This function enumerates as an HID device, allowing the boot to consume 500 mA
from the host PC. The function returns after initializing the stack. Any
events, such as connection of a PC or a wall charger, or the completion of the
enumeration as an HID device, is reported using the	hsu_boot_poll_stack
function.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- Operation successful.\n
FALSE -- Operation failed.
*/
boolean hsu_boot_start_stack(void);

/*=============================================================================
FUNCTION    hsu_boot_stop_stack
=============================================================================*/
/**
Stops and tears down the HS-USB stack under BOOT. 

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_boot_stop_stack(void);

/*=============================================================================
FUNCTION    hsu_boot_poll_stack
=============================================================================*/
/**
Polls the HS-USB stack and returns the type of event that occurred as a 
hsu_boot_poll_result type.

@dependencies
hsu_boot_start_stack must be called before this function is called.

@sideeffects
None.

@return
hsu_boot_poll_result corresponding to the type of USB event that occurred.
If no new event occurred, HSU_BOOT_POLL_RESULT_NO_NEW_EVENT is returned.
*/
hsu_boot_poll_result hsu_boot_poll_stack(void);

/*=============================================================================
FUNCTION    hsu_boot_disconnect_from_host
=============================================================================*/
/**
Requests the HS-USB stack to detach from the host while keeping the USB stack
active, allowing the boot to consume 100 mA from the host PC.

This function is intended to be called by the boot when the host PC suspends
the device.

@dependencies
hsu_boot_start_stack must be called before this function is called.

@sideeffects
None.

@return
TRUE  -- Operation successful.\n
FALSE -- Operation failed.
*/
boolean hsu_boot_disconnect_from_host(void);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* HSU_BOOT_H */
