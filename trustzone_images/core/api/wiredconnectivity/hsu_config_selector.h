#ifndef _HSU_CONFIG_SELECTOR_H_
#define _HSU_CONFIG_SELECTOR_H_

/*=============================================================================

High Speed USB configuration Selector Module        
          H E A D E R   F I L E

=============================================================================*/
/**
@file hsu_config_selector.h

Initializes the High-Speed USB stack.

Externalized functions:
- hsu_config_selector_init()
- hsu_get_num_of_net_instances()
- hsu_conf_sel_get_rm_wakeup_delay()
- hsu_conf_sel_diag_is_on_legacy_port()
- hsu_conf_sel_uim_first_inst_class()
- hsu_conf_sel_use_pmic_otg_comparators()
- hsu_conf_sel_reconnect_on_reset_during_suspend()
- hsu_conf_sel_number_of_sdcc_luns()
- hsu_conf_sel_disable_sleep_voting()
- hsu_config_selector_is_standard_ecm_supported()
- hsu_config_selector_is_standard_acm_supported()
- hsu_conf_sel_is_hsusb_initialized()
- hsu_conf_sel_update_bmp_composition()
- hsu_conf_sel_get_bmp_device_info()
- hsu_conf_sel_hs_main_core_enabled()
- hsu_conf_sel_hsic_core_enabled()
- hsu_conf_sel_hide_ftl()

hsu_config_selector_init() should be called after DSM pools are initialized,
and before the creation of tasks that use USB, such as DIAG, Modem, RmNET, 
etc.
*/

/*=============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_config_selector.h#1 $
$DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/25/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
09/12/06  ke  Initial version

=============================================================================*/
/*=============================================================================
Copyright (c) 2007-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

#include "nv_items.h"
#include "rdevmap.h"

#include "uw_args.h"
#include "hsu_conf_sel_types.h"

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

/** BrewMP device data structure.
*/
typedef struct hsu_conf_sel_brewmp_device_data_struct 
{
  boolean               is_initialized;
    /**< TRUE if the BrewMP device data structure is initialized. */
  dev_info_t            device_info;
    /**< Device information. */
  config_param_t        configuration_info;
    /** Configuration information. */
  uint8                 num_of_rdm_dev_in_comp;
    /**< Number of RDM devices used. */
  rdm_device_enum_type  rdm_devices[HSU_NUM_OF_RDM_USB_PORTS];
    /**< Array of RDM devices. */
} hsu_conf_sel_brewmp_device_data_type;


/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================
FUNCTION    hsu_config_selector_init
==============================================================================*/
/**
Initializes the High-Speed USB stack according to the default composition;
called from the application's processor TMC.

@dependencies
Should be called after DSM pools are initialized, and before the creation of 
tasks that use USB, such as DIAG, Modem, RmNET, etc.

@sideeffects
Various tasks related to the HS-USB stack are created (HSU_AL_MS_TASK, 
HSU_AL_TASK, and several tasks that are created by Jungo).

@return
None.
*/
void hsu_config_selector_init(void);

/*===========================================================================
FUNCTION    hsu_get_num_of_net_instances
==============================================================================*/
/**
Returns the number of network (ECM) FD instances included in
the current composition for the primary core.

@dependencies
FEATURE_HS_USB must be defined. 
Must be called after hsu_config_selector_init().

@sideeffects
None.

@return
The number of network (ECM) FD instances included in the current
composition.
*/
uint8 hsu_get_num_of_net_instances(void);

/*==============================================================================
FUNCTION    hsu_conf_sel_get_rm_wakeup_delay
==============================================================================*/
/**
Returns the delay (in milliseconds) used for performing an
explicit remote wakeup.

@dependencies
None.

@sideeffects
None.

@return
The remote wakeup delay (in milliseconds).
*/
uint32 hsu_conf_sel_get_rm_wakeup_delay(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_diag_is_on_legacy_port
=============================================================================*/
/**
Reads the NV item that determines whether DIAG should run over full-speed or
high-speed USB and saves it in the variable hsu_diag_on_legacy_usb_port.

@note This function is relevant only if both FS-legacy and HS-OTG cores are
      supported in the image. Otherwise, DIAG is always routed from the primary
	  core.

@dependencies
hsu_config_selector_init must be called before this function.

@sideeffects
None.

@return
TRUE  -- DIAG should run over full-speed USB port.\n
FALSE -- DIAG should run over high-speed USB port.
*/
boolean hsu_conf_sel_diag_is_on_legacy_port(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_uim_first_inst_class
=============================================================================*/
/**
Returns the operating mode of the Universal Integrated Circuit Card.

@dependencies
hsu_config_selector_init must be called before this function.

@sideeffects
None.

@return
UICC operating mode.
*/
nv_uim_first_inst_class_type hsu_conf_sel_uim_first_inst_class(void);
  
/*=============================================================================
FUNCTION    hsu_conf_sel_use_pmic_otg_comparators
=============================================================================*/
/**
Returns whether or not to use Power Management IC OTG comparators, based on the
value of the NV item hs_usb_use_pmic_otg_comparators.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- PMIC comparators are used during LPM.\n
FALSE -- PMIC comparators are not used during LPM.
*/
boolean hsu_conf_sel_use_pmic_otg_comparators(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_reconnect_on_reset_during_suspend
=============================================================================*/
/**
Returns whether or not to use the workaround for the issue with the USB bus
reset being on during TCXO shutdown, based on the value of the NV item
hs_usb_reconnect_on_reset_during_suspend.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The workaround is used.\n
FALSE -- The workaround is not used.
*/
boolean hsu_conf_sel_reconnect_on_reset_during_suspend(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_number_of_sdcc_luns
=============================================================================*/
/**
Returns the number of SDCC/MMC LUNs that are enabled.

@dependencies
None.

@sideeffects
None.

@return
The number of SDCC/MMC LUNs that are enabled.
*/
uint8 hsu_conf_sel_number_of_sdcc_luns (void);

/*=============================================================================
FUNCTION    hsu_config_selector_is_standard_ecm_supported
=============================================================================*/
/**
Returns TRUE if the current composition is one that supports standard ECM mode.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The current composition supports standard ECM mode.\n
FALSE -- The current composition does not support standard ECM mode.
*/
boolean hsu_config_selector_is_standard_ecm_supported(void);

/*=============================================================================
FUNCTION    hsu_config_selector_is_standard_acm_supported
=============================================================================*/
/**
Returns TRUE if the current composition is one that supports standard ACM mode.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The current composition supports standard ACM mode.\n
FALSE -- The current composition does not support standard ACM mode.
*/
boolean hsu_config_selector_is_standard_acm_supported(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_disable_sleep_voting
=============================================================================*/
/**
Returns whether or not to disable voting for sleep when entering LPM, based on
the value of the NV item hs_usb_disable_sleep_voting.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The value of the hs_usb_disable_sleep_voting NV item is TRUE.\n
FALSE -- The value of the hs_usb_disable_sleep_voting NV item is FALSE, 
         or the NV item could not be read.
*/
boolean hsu_conf_sel_disable_sleep_voting(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_hsic_core_enabled
=============================================================================*/
/**

This function returns whether the High-Speed Inter-Chip is enabled at runtime.

@dependencies
None.

@sideeffects
None.

@return
TRUE -- If the value of the hs_usb hsic_enabled NV item is TRUE, or no
        HS-USB OTG core is defined.\n
FALSE -- NV item could not be read or the HSIC core not enabled.
*/
boolean hsu_conf_sel_hsic_core_enabled (void);

/*=============================================================================
FUNCTION    hsu_conf_sel_hide_ftl
=============================================================================*/
/**

This function returns whether FTL should be hidden at runtime.

@dependencies
None.

@sideeffects
None.

@return
TRUE -- When value of the hs_usb hide_ftl NV/EFS item is TRUE
FALSE -- NV item could not be read or when hide_ftl is FALSE
*/
boolean hsu_conf_sel_hide_ftl (void);

/*=============================================================================
FUNCTION    hsu_conf_sel_hs_main_core_enabled
=============================================================================*/
/**

This function returns whether the HS-USB OTG core is enabled at runtime.

@dependencies
None.

@sideeffects
None.

@return
TRUE -- If the value of the hs_usb hsic_enabled NV item is FALSE, or no HSIC
        core is defined.\n
FALSE -- If the value of the hsic_enabled NV item is FALSE.

*/
boolean hsu_conf_sel_hs_main_core_enabled (void);


/*=============================================================================
FUNCTION    hsu_conf_sel_is_hsusb_initialized
=============================================================================*/
/**
Returns whether hsu_config_selector_init() completed.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- hsu_config_selector_init() completed.\n
FALSE -- hsu_config_selector_init() did not complete.
*/
boolean hsu_conf_sel_is_hsusb_initialized(void);

/*=============================================================================
FUNCTION    hsu_conf_sel_update_bmp_composition
=============================================================================*/
/**
Defines the details of the BrewMP device composition and starts an
enumeration as a BrewMP device.

@param *device_info            [IN] -- Device information.
@param *configuration_info     [IN] -- Configuration information.
@param num_of_rdm_dev_in_comp  [IN] -- Number of RDM devices used.
@param rdm_devices             [IN] -- Array of RDM devices used.

@dependencies
The BrewMP composition must be active.

@sideeffects
None.

@return
TRUE  -- The composition was updated successfully.\n
FALSE -- The composition was not updated successfully.
*/
boolean hsu_conf_sel_update_bmp_composition(
  dev_info_t*           device_info,
  config_param_t*       configuration_info,
  uint8                 num_of_rdm_dev_in_comp,
  rdm_device_enum_type  rdm_devices[HSU_NUM_OF_RDM_USB_PORTS]
);

/*=============================================================================
FUNCTION    hsu_conf_sel_get_bmp_device_info
=============================================================================*/
/**
Retrieves the BrewMP device information as provided by the BrewMP layer upon
the call to hsu_config_selector_update_bmp_composition().

@dependencies
None.

@sideeffects
None.

@return
A pointer to brewmp_device_data_struct.
*/
hsu_conf_sel_brewmp_device_data_type* hsu_conf_sel_get_bmp_device_info(void);

/** @} */ /* end_group hsusb_qc_api */

#endif /* _HSU_CONFIG_SELECTOR_H_ */
