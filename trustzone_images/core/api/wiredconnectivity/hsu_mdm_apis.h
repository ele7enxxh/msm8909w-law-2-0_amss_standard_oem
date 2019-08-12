#ifndef _HSU_MDM_APIS_H_
#define _HSU_MDM_APIS_H_

/*=============================================================================

     High Speed USB Module Remote APIs on Modem processor 
      H E A D E R   F I L E
=============================================================================*/
/**
@file hsu_mdm_apis.h
  
High-Speed USB APIs that are implemented on the modem processor.

The externalized functions in this module are:
    - hsu_api_init_phy()
    - hsu_api_vbus_powerup()
    - hsu_api_vbus_shutdown()
    - hsu_api_update_product_id()
    - hsu_api_update_serial_number()
    - hsu_api_update_is_serial_number_null()
    - hsu_api_init_chg()
    - hsu_api_chg_is_connected()
    - hsu_api_enable_pmic_otg_comparators()
    - hsu_api_disable_pmic_otg_comparators()
    - hsu_api_init_uicc()
    - hsu_api_start_uicc()
    - hsu_api_disconnect_pulldowns_uicc()
    - hsu_api_update_vendor_id()
*/
/*=============================================================================
  Copyright (c) 2007 - 2010 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_mdm_apis.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  06/25/10  lo 	(Tech Pubs) Additional edits to Dogygen comments for USB
                API Reference Guide Rev. B.
  06/02/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
  02/08/07  ke  Initial version
  05/02/07  me  Add functions to update the product id and the serial number
    
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

/*=============================================================================
  Typedefs
=============================================================================*/

/*=============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_api_init_phy
=============================================================================*/
/**
Starts the USB PHY initialization sequence specific to each target.

@dependencies
FEATURE_HS_USB must be defined.
  
@sideeffects
None.

@return
None.
*/
void hsu_api_init_phy(void);
/*~ FUNCTION hsu_api_init_phy */

/*=============================================================================
FUNCTION    hsu_api_init_uicc
=============================================================================*/
/**
Initializes the USB UICC core and GPIOs.

@dependencies
FEATURE_HS_USB_UICC must be defined.

@sideeffects
None.

@return
None.
*/
void hsu_api_init_uicc(void);
/*~ FUNCTION hsu_api_init_uicc */

/*=============================================================================
FUNCTION    hsu_api_start_uicc
=============================================================================*/
/**
Starts the USB UICC core and GPIOs.

@dependencies
FEATURE_HS_USB_UICC must be defined.

@sideeffects
None.

@return
None.
*/
void hsu_api_start_uicc(void);
/*~ FUNCTION hsu_api_start_uicc */

/*=============================================================================
FUNCTION    hsu_api_polldown_uicc
=============================================================================*/
/**
Pulls down the D+ and D- of the USB UICC GPIOs.

@dependencies
FEATURE_HS_USB_UICC must be defined.

@sideeffects
None.

@return
None.
*/
void hsu_api_polldown_uicc(void);
/*~ FUNCTION hsu_api_polldown_uicc */

/*=============================================================================
FUNCTION    hsu_api_vbus_powerup
=============================================================================*/
/**
Turns on the 5V power supply on the VBUS pin when the HS-USB stack enters
Host mode.

@dependencies
FEATURE_HS_USB must be defined.

@sideeffects
None.

@return
None.
*/
void hsu_api_vbus_powerup(void);
/*~ FUNCTION hsu_api_vbus_powerup */

/*=============================================================================
FUNCTION    hsu_api_vbus_shutdown
=============================================================================*/
/**
Turns off the 5V power supply on the VBUS pin when the HS-USB stack 
exits Host mode.

@dependencies
  FEATURE_HS_USB must be defined. 

@sideeffects
  None.

@return
  None.
*/
void hsu_api_vbus_shutdown(void);
/*~ FUNCTION hsu_api_vbus_shutdown */

/*=============================================================================
FUNCTION    hsu_api_update_product_id
=============================================================================*/
/**
Updates the ARM9&tm; &nbsp; product ID variable.

@param product_id [IN] -- The USB product ID.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_api_update_product_id(unsigned short product_id);
/*~ FUNCTION hsu_api_update_product_id */
/*~  PARAM IN product_id UINT 16*/

/*=============================================================================
FUNCTION    hsu_api_update_serial_number
=============================================================================*/
/**
Updates the ARM9 serial number variable.

@param *serial_number [IN] -- The USB serial number string.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_api_update_serial_number(const char* serial_number);
/*~ FUNCTION hsu_api_update_serial_number */
/*~  PARAM IN serial_number STRING*/

/*=============================================================================
FUNCTION    hsu_api_update_is_serial_number_null
=============================================================================*/
/**
Updates the ARM9 as to whether or not the serial number is NULL.

@param is_serial_number_null [IN] -- TRUE if the serial number is NULL.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_api_update_is_serial_number_null(boolean is_serial_number_null);
/*~ FUNCTION hsu_api_update_is_serial_number_null */
/*~  PARAM IN is_serial_number_null UINT 8*/


/*=============================================================================
FUNCTION    hsu_api_notif_charger_not_avail
=============================================================================*/
/**
Notifies the charger module that it can no longer draw current.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_api_notif_charger_not_avail(void);
/*~ FUNCTION hsu_api_notif_charger_not_avail */

/*=============================================================================
FUNCTION    hsu_api_notif_charger_is_avail
=============================================================================*/
/**
  Notifies the charger module that it can start drawing current.

@param amount_current_to_draw [IN] -- Specifies the amount of current (in mA)
                                      the charger module can draw.

@dependencies
  None.

@sideeffects
  None.

@return
  None.
*/
void hsu_api_notif_charger_is_avail(uint32 amount_current_to_draw);
/*~ FUNCTION hsu_api_notif_charger_is_avail */
/*~  PARAM IN amount_current_to_draw UINT32*/

/*=============================================================================
FUNCTION    hsu_api_notif_charger_disconnected
=============================================================================*/
/**
Notifies the charger module that it is disconnected from the host PC.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_api_notif_charger_disconnected(void);
/*~ FUNCTION hsu_api_notif_charger_disconnected */

/*=============================================================================
FUNCTION    hsu_api_notif_charger_connected
=============================================================================*/
/**
Notifies the charger software module whether it is connected to a Host PC
or to a dedicated wall charger.

@param is_wall_charger [IN] -- TRUE if connected to a wall charger;
                               FALSE if the charging is done from a host PC.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_api_notif_charger_connected(boolean is_wall_charger);
/*~ FUNCTION hsu_api_notif_charger_connected */
/*~  PARAM IN is_wall_charger UINT 8*/

/*=============================================================================
FUNCTION    hsu_api_init_chg
=============================================================================*/
/**
Initializes the hsu_chg module.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_api_init_chg(void);
/*~ FUNCTION hsu_api_init_chg */

/*=============================================================================
FUNCTION    hsu_api_chg_is_connected
=============================================================================*/
/**
Returns whether the charger module is connected.

@dependencies
None.

@sideeffects
None.

@return
TRUE -- The charger is connected. \n
FALSE -- The charger is not connected.
*/
boolean hsu_api_chg_is_connected(void);
/*~ FUNCTION hsu_api_chg_is_connected */

/*=============================================================================
FUNCTION    hsu_api_usb_reset_rework_installed
=============================================================================*/
/**
Checks whether the USB reset rework is installed.\ The rework is needed to
properly exit TCXO shutdown upon receiving a USB Reset.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The rework is installed. \n
FALSE -- The rework is not installed.
*/
boolean hsu_api_usb_reset_rework_installed(void);
/*~ FUNCTION hsu_api_usb_reset_rework_installed */

/*=============================================================================
FUNCTION    hsu_api_enable_pmic_ulpidata0_irq_handler 
=============================================================================*/
/**
Configures PMIC MPPxx to interrupt when ULPI Data(0)[D+] changes, since this
line is not monitored by the MSM&tm;&nbsp; during TCXO shutdown (sleep).\ This
work-around is required to support USB Reset (D+ and D- both LOW).

@dependencies
None.

@sideeffects
Registers an ISR to be invoked upon change in input value to MPP15.

@return
None.
*/
void hsu_api_enable_pmic_ulpidata0_irq_handler(void);
/*~ FUNCTION hsu_api_enable_pmic_ulpidata0_irq_handler */

/*=============================================================================
FUNCTION    hsu_api_disable_pmic_ulpidata0_irq_handler
=============================================================================*/
/**
Disables the PMIC MPPxx interrupt.

This function is called when USB is no longer in LPM, hence ULPI Data(0)[D+]
does not need to be monitored by the PMIC.

@dependencies
None.

@sideeffects
Unregisters the ISR from the corresponding enable function.

@return
None.
*/
void hsu_api_disable_pmic_ulpidata0_irq_handler(void);
/*~ FUNCTION hsu_api_disable_pmic_ulpidata0_irq_handler */

/*=============================================================================
FUNCTION    hsu_dload_set_usb_imax
=============================================================================*/
/**
Sets the power limit register to the maximum current when called from the
bootloader.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_dload_set_usb_imax(void);
/*~ FUNCTION hsu_dload_set_usb_imax */

/*=============================================================================
FUNCTION    hsu_dload_enable_disable_core
=============================================================================*/
/**
Enables or disables the HS-USB PHY when called from the bootloader.

@param enable [IN] -- If TRUE, enables the core. If FALSE, disables the core.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_dload_enable_disable_core(boolean enable);
/*~ FUNCTION hsu_dload_enable_disable_core */
/*~ PARAM IN enable UINT 8*/

/*=============================================================================
FUNCTION    hsu_dload_is_wall_charger
=============================================================================*/
/**
Determines whether a wall charger is connected when called from the
bootloader.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- A wall charger is connected. \n
FALSE -- A wall charger is not connected.
*/
boolean hsu_dload_is_wall_charger(void);
/*~ FUNCTION hsu_dload_is_wall_charger */

/*=============================================================================
FUNCTION    hsu_dload_sess_is_valid
=============================================================================*/
/**
Determines whether an HS USB session is valid when called from the
bootloader.

@dependencies
pm_sbl_init() must be called before calling this function.

@sideeffects
None.

@return
TRUE  -- The HS-USB session is valid. \n
FALSE -- The HS-USB session is invalid.
*/
boolean hsu_dload_sess_is_valid(void);
/*~ FUNCTION hsu_dload_sess_is_valid */

/*=============================================================================
FUNCTION    hsu_api_enable_pmic_otg_comparators
=============================================================================*/
/**
Turns on the PMIC OTG comparators.\ This is for detecting USB cable
connection events during LPM.

The goal of this function is to replace the HS-USB PHY's comparators with
the PMIC OTG comparators in targets that use an integrated HS-USB PHY. This
is because the PMIC OTG comparators are more energy efficient than the PHY's
comparators (the PHY's comparators consume ~80 uA, while the PMIC OTG
comparators consume only ~1.6 uA).

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The PMIC OTG comparators were turned on. \n
FALSE -- The PMIC OTG comparators were not turned on.
*/
boolean hsu_api_enable_pmic_otg_comparators(void);
/*~ FUNCTION hsu_api_enable_pmic_otg_comparators */

/*=============================================================================
FUNCTION    hsu_api_disable_pmic_otg_comparators
=============================================================================*/
/**
Turns off the PMIC OTG comparators.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The PMIC OTG comparators were turned off. \n
FALSE -- The PMIC OTG comparators were not turned off.

@sa
hsu_api_enable_pmic_otg_comparators() for more details on PMIC comparitors.
*/
boolean hsu_api_disable_pmic_otg_comparators(void);
/*~ FUNCTION hsu_api_disable_pmic_otg_comparators */

/*=============================================================================
FUNCTION    hsu_api_disconnect_polldowns_uicc
=============================================================================*/
/**
Disconnects the pull-downs on the D+ and D- of the USB UICC GPIOs.

@dependencies
FEATURE_HS_USB_UICC must be defined.

@sideeffects
None.

@return
None.
*/
void hsu_api_disconnect_pulldowns_uicc(void);
/*~ FUNCTION hsu_api_disconnect_pulldowns_uicc */

/*=============================================================================
FUNCTION    hsu_api_update_vendor_id
=============================================================================*/
/**
Updates the vendor id variable.

@param vendor_id [IN] -- The USB vendor ID.

@dependencies
for GOBI only. 

@sideeffects
None.

@return
None.
*/
void hsu_api_update_vendor_id(unsigned short vendor_id);
/*~ FUNCTION hsu_api_update_vendor_id */

/** @} */ /* end_group hsusb_qc_api */

#endif /* _HSU_MDM_APIS_H_ */
