/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file host_hid_app.h

Human Interface Device (HID) Class Driver Interface between the OS (via its 
porting layer) and the HID Class Driver Application.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/host_hid_app.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/21/10  sw    (Tech Pubs) - edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _HOST_HID_APP
#define _HOST_HID_APP

#include <uw_host_hid_common.h> /* Common HID Class Driver API */


/** @ingroup hsusb_jungo_api 
@{ */

/** Application device context.
*/
typedef void *hid_app_dev_ctx_t;

/** Private porting layer data.
*/
typedef void *os_input_ctx_t;

/** HID input device types. 
*/
typedef enum
{
    HID_UNDEF = 0,   /**< &nbsp; */
    HID_KEYBOARD,    /**< &nbsp; */
    HID_MOUSE,       /**< &nbsp; */
    HID_GAMEPAD,     /**< &nbsp; */
    HID_JOYSTICK,    /**< &nbsp; */
    HID_XBOX,        /**< &nbsp; */
    HID_BT_KEYBOARD, /**< &nbsp; */
    HID_BT_MOUSE,    /**< &nbsp; */
    HID_BT_GAMEPAD,  /**< &nbsp; */
    HID_BT_JOYSTICK  /**< &nbsp; */
} input_device_type_t;

#define IS_BT_HID_DEV(dev) ( (dev == HID_BT_KEYBOARD) \
                             || (dev == HID_BT_MOUSE) \
                             || (dev == HID_BT_GAMEPAD) \
                             || (dev == HID_BT_JOYSTICK) )

/********************************************************
 * OS Porting Layer -> HID Class Driver Application API *
 ********************************************************/

/** OS porting layer HID class driver callbacks structure.
*/
typedef struct
{
    void (*unregister_input_device)(os_input_ctx_t os_ctx);
	    /**< Unregisters a HID input device.\n
             Requirement: Mandatory. */

    void (*report_event)(os_input_ctx_t os_ctx, juint8_t keycode,
        juint8_t event, usage_att_t *att);
        /**< Report a device's key/button event to the OS.\n
             Requirement: Mandatory */
	
    void (*report_relative_data)(os_input_ctx_t os_ctx, juint8_t data_type,
        jint32_t data, usage_att_t *att);
	    /**< Reports a device's relative data to the OS.\n
             Requirement: Mandatory if the device's report descriptor includes 
			 a boolean data state value (e.g., On/Off or Pressed/Released). 
			 Such devices include keyboard keys, mouse buttons, etc. */

		
    void (*report_absolute_data)(os_input_ctx_t os_ctx, juint8_t data_type, 
        juint32_t data, usage_att_t *att);
	    /**< Reports a device's absolute data to the OS.\n
             Requirement: Mandatory if the device's report descriptor includes 
			 a boolean data state value (e.g., On/Off or Pressed/Released).
			 Such devices include keyboard keys, mouse buttons, etc. */

} os_input_ops_t;

/** @} */ /* end_group hsusb_jungo_api */

#endif
