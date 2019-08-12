#ifndef _HSU_BUS_MODE_NOTIFIER_H_
#define _HSU_BUS_MODE_NOTIFIER_H_

/*=============================================================================

    High Speed USB suspend/resume notifier
        H E A D E R   F I L E
=============================================================================*/ 
/**
@file hsu_bus_mode_notifier.h

Defines the suspend/resume notifier. It allows registering for
suspend and resume events from the USB stack.

Externalized functions:
- hsu_register_bus_mode_notif_cb
- hsu_unregister_bus_mode_notif_cb
- hsu_bus_mode_changed
*/
/*=============================================================================
  
							  EDIT HISTORY FOR MODULE
	
	This section contains comments describing changes made to the module.
	Notice that changes are listed in reverse chronological order.
	
	$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_bus_mode_notifier.h#1 $
	$DateTime: 2016/06/17 14:31:11 $
	  
	when	  who	  what, where, why
	--------  ---	  ---------------------------------------------------------
    06/24/10   lo     (Tech Pubs) Edited/added Doxygen comments and markup.
	  
=============================================================================*/
  
/*=============================================================================
  Copyright (c) 2007-2010 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/
#include "comdef.h"

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
/** HS-USB bus mode.
*/
typedef enum 
{
  HSU_BUS_MODE_SUSPENDED,    /**< &nbsp; */
  HSU_BUS_MODE_RESUMED,      /**< &nbsp; */
  HSU_BUS_MODE_UNCONFIGURED, /**< &nbsp; */
  HSU_BUS_MODE_CONFIGURED,   /**< &nbsp; */
  HSU_BUS_MODE_DISCONNECTED, /**< &nbsp; */
  HSU_BUS_MODE_CONNECTED     /**< &nbsp; */
} hsu_bus_mode_enum;

/** Definition of a callback to be invoked when suspend or resume occurs.
    This callback gets two parameters:
    - new_mode -- New HS-USB bus mode.
    - app_data -- Application-specific data.
*/
typedef void (*hsu_bus_mode_notifier_cb)(hsu_bus_mode_enum new_mode, void* app_data);

/** HS-USB bus mode callback information structure.
*/
typedef struct
{
  hsu_bus_mode_notifier_cb   bus_mode_cb;
  /**< Callback to be invoked when suspend or resume occurs. */

  void*                           app_data;
  /**< Application-specific data. */

} hsu_bus_mode_cb_info_struct;

/*=============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_register_bus_mode_notif_cb
=============================================================================*/
/**
Registers a callback function to be invoked when the suspend/resume state of 
the HS-USB stack is changed.

@param cb_info [IN] -- HS-USB bus mode callback information structure.

@dependencies
None.

@sideeffects
None.

@return
TRUE -- Registration successful.\n
FALSE -- Registration failed.
*/
boolean hsu_register_bus_mode_notif_cb
(
  hsu_bus_mode_cb_info_struct cb_info
);

/*=============================================================================
FUNCTION   hsu_unregister_bus_mode_notif_cb
=============================================================================*/
/**
Unregister a suspend/resume callback function.

@param cb_info [IN] -- HS-USB bus mode callback information structure.

@dependencies
hsu_register_bus_mode_notif_cb must have been called with the same 
callback function.

@sideeffects
None.

@return
None.
*/
void hsu_unregister_bus_mode_notif_cb
(
  hsu_bus_mode_cb_info_struct cb_info
);

/*=============================================================================
FUNCTION    hsu_bus_mode_changed
=============================================================================*/
/**
Called when a change in bus mode occurs (either when the bus is suspended or
resumed).

@param core_index [IN] -- HS-USB core index.
@param new_mode   [IN] -- New HS-USB bus mode.

@dependencies
None.

@sideeffects
All registered callbacks are invoked.

@return
None.
*/
void hsu_bus_mode_changed
(
  uint32 core_index,
  hsu_bus_mode_enum new_mode
);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_BUS_MODE_NOTIFIER_H_ */
