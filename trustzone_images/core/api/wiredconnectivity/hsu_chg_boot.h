#ifndef _HSU_CHG_BOOT_H_
#define _HSU_CHG_BOOT_H_
/*=============================================================================

                    HS-USB API to BOOT CHG Module Header file
=============================================================================*/
/**
@file hsu_chg_boot.h

Contains the functions that will handle the scenario where the phone boots from
the USB port and finds that the battery is too low to assist in the boot
process.

The phone will attempt to charge the battery first before continuing
to boot. The API definitions are for boot to allow interfacing with HS-USB
regarding the USB charging in boot.

A makeonly flag is required for this file to compile:\n
hsu_chg_boot makeonly (adds object files for this file)

@par Dead Battery Charging vs. Fast Dead Battery Charging
Dead Battery Charging:\n
When the battery is too low to boot up and the USB PC Host cable is 
connected, the device will draw less than 100 mA to charge the battery.\n
@par
Fast Dead Battery Charging:\n
When the battery is too low to boot up and the USB PC Host cable is 
connected, the device will enumerate as an HID device to draw up to 500 mA
to charge the battery.\n
The following flags must be defined:
- FEATURE_HS_USB_FAST_CHARGE_VIA_HID -- Enables the feature in *.c file.
- hsu_fast_charge makeonly -- Adds object files for HID.
        
Externalized functions:\n
hsu_chg_boot
*/
/*=============================================================================
Copyright (c) 2009-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_chg_boot.h#1 $
$DateTime: 2016/06/17 14:31:11 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/24/10 lo   (Tech Pubs) Edited/added Doxygen comments and markup.
04/17/09 jc   Initial version

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

/*=============================================================================
Macros
=============================================================================*/
/** Minimum threshold for weak battery (in mV).
*/
#define HSU_CHG_BOOT_VBATT_WEAK_THRESH_MIN              (3200)

/** Maximum threshold for weak battery (in mV).
*/
#define HSU_CHG_BOOT_VBATT_WEAK_THRESH_MAX              (3700)

/** Device current consumption should be less than the defined maximum (in mA).
*/
#define HSU_CHG_BOOT_I_DEV_MAX                          (200)

/*==============================================================================
Typedefs
==============================================================================*/
/** Charging boot error types. 
*/
typedef enum
{
  HSU_CHG_BOOT_ERR_NONE,
  /**< -- No error. */
  HSU_CHG_BOOT_ERR_PARAM_PTR_NULL,
  /**< -- Parameter pointer is NULL. */
  HSU_CHG_BOOT_ERR_PARAM_OUT_OF_RANGE_VBATT_WEAK,
  /**< -- Weak battery threshold parameter is out of range (3200 to 3700V). */
  HSU_CHG_BOOT_ERR_PARAM_OUT_OF_RANGE_DEV_I_CONSUM,
  /**< -- Device current consumption parameter is out of range (0 to 200 mA). */
  HSU_CHG_BOOT_ERR_PARAM_CB_NULL_POWER_DOWN,
  /**< -- Callback function power down is not set. */
  HSU_CHG_BOOT_ERR_USB_POLL_FAIL,
  /**< -- USB polling returned with error. */
  HSU_CHG_BOOT_ERR_USB_DISCON_CHG_FAIL,
  /**< -- USB failed to disconnect and charge. */
} hsu_chg_boot_err_type;

/** Charging boot callbacks.  
*/
typedef struct
{
  void (*power_down)(void);
  /**< Mandatory; to be called on power down. */
  void (*disp_off)(void);
  /**< Optional; to be called when the display is off. */
  void (*disp_chg_screen)(void);
  /**< Optional; to be called after enumeration. */
  void (*disp_chg_fail_screen)(void);
  /**< Optional; to be called after cancelling enumeration. */
} hsu_chg_boot_cb_list_type;

/** Charging boot parameter type.
*/
typedef struct
{
  int32 vbatt_weak_thresh;
  /**< Weak battery threshold. */
    int32 device_i_consumption;
  /**< Device power consumption during Fast Dead Battery Charging. */
    boolean requires_vbus_vchg_rework;
  /**< Flag to indicate that the device requires USB charging hardware rework
       of shorting Vbus and Vchg. */
    hsu_chg_boot_cb_list_type cb_list;
  /**< Registered callback list. */
} hsu_chg_boot_param_type;

/*=============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_chg_boot
=============================================================================*/
/**
Implements the core algorithm that handles the special case where the phone
boots from USB and finds the battery is too low. The core algorithm will
attempt to charge the battery before boot-up continues.

@param *param_ptr [IN] -- Device charging information.

@dependencies
HS-USB CLK should be running above 60 MHz. This function should only be called
from boot-code.

@sideeffects
None.

@return
Returns an error code when this condition exists.
*/
hsu_chg_boot_err_type 
hsu_chg_boot(hsu_chg_boot_param_type *param_ptr);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_CHG_BOOT_H_ */

