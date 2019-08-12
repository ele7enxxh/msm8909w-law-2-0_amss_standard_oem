#ifndef _HSU_EVENT_NOTIFIER_H_
#define _HSU_EVENT_NOTIFIER_H_


/*=============================================================================

    High Speed USB generic event notifier
        H E A D E R   F I L E
=============================================================================*/
/**
@file hsu_event_notifier.h

Generic event notifier that allows clients outside HS-USB
to register for events from the HS-USB.

Externalized functions:
- hsu_register_event_notif_cb()
- hsu_unregister_event_notif_cb()
- hsu_event_raise()
*/
/*=============================================================================
  
							  EDIT HISTORY FOR MODULE
	
	This section contains comments describing changes made to the module.
	Notice that changes are listed in reverse chronological order.
	
	$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_event_notifier.h#1 $
	$DateTime: 2016/06/17 14:31:11 $
	  
	when	  who	  what, where, why
	--------  ---	  ---------------------------------------------------------
    06/25/10   LO     (Tech Pubs) Edited/added Doxygen comments and markup.
	  
=============================================================================*/

/*=============================================================================
  Copyright (c) 2009 by Qualcomm Technologies Incorporated.
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

/** Describes an event.
*/
typedef enum 
{
  HSU_EVENT_INIT_COMPLETED = 0,  /**< -- Indicates HS-USB initialization
                                         is complete. */
  HSU_EVENT_UICC_ENUM_FAILED,    /**< -- Indicates if UICC enumeration has
								         failed. */
  HSU_EVENT_MAX                  /**< -- Number of possible events. */
} hsu_event_notif_enum;

/** HS-USB event notifier callback.
*/
typedef void (*hsu_event_notif_cb)(void);

/** Structure for defining a callback and an event.
*/
typedef struct
{
  hsu_event_notif_cb   event_cb;  /**< The callback to be invoked when
                                       the event occurs */
  hsu_event_notif_enum event;     /**< The event. */
} hsu_event_notif_cb_info_struct;

/*=============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_register_event_notif_cb
=============================================================================*/
/**
Registers a callback function to be invoked when the specified event is
raised.

@param cb_info [IN] -- Structure that specifies the callback function and
                       event.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- Registration was successful.\n
FALSE -- Registration failed.
*/
boolean hsu_register_event_notif_cb
(
  hsu_event_notif_cb_info_struct cb_info
);

/*===========================================================================
FUNCTION    hsu_unregister_event_notif_cb
===========================================================================*/
/**
Unregisters a callback function.\ The callback will no longer be invoked
when the event is raised.

@param cb_info [IN] -- Structure that specifies the callback function and
                       event.

@dependencies
hsu_register_event_notif_cb() must have been called with the same
callback function.

@sideeffects
None.

@return
None.
*/
void hsu_unregister_event_notif_cb
(
  hsu_event_notif_cb_info_struct cb_info
);

/*=============================================================================
FUNCTION    hsu_event_notif_raise
=============================================================================*/
/**
Calls all the callbacks registered for a certain HS-USB event when it is raised.

@param event [IN] -- HS-USB event.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_event_notif_raise
(
  hsu_event_notif_enum event
);

/** @} */ /* end_group hsusb_qc_api */

#endif /* _HSU_EVENT_NOTIFIER_H_ */
