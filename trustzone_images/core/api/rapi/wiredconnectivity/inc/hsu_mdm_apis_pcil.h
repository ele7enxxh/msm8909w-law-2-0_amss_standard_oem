 /*==============================================================================

High Speed USB Module PCIL Remote APIs on Modem processor 

GENERAL DESCRIPTION
This module defines PCIL APIs for High Speed USB APIs that are implemented on the modem 
processor. 

EXTERNALIZED FUNCTIONS
hsu_api_init_phy_pcil

INITALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when      who     what, where, why
--------  ---     ------------------------------------------------------------
04/01/09  me  Initial version
==============================================================================*/
#ifndef HSU_MDM_APIS_PCIL_H
#define HSU_MDM_APIS_PCIL_H

/*===========================================================================

FUNCTION      HSU_API_INIT_PHY_PCIL

DESCRIPTION
The function performs HSUSB PHY intialization.

DEPENDENCIES
FEATURE_HS_USB must be defined. 

RETURN VALUE
The function allways returns TRUE.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_api_init_phy_pcil(uint32* ignored_param_a, uint32* ignored_param_b);


/*===========================================================================

FUNCTION      HSU_PIN_CONFIG_PCIL

DESCRIPTION
PCIL function that does usb pin configuration.
This pcil function is a wrapper for HWIO_USB_PIN_CONFIG_OUTM.

DEPENDENCIES
FEATURE_HS_USB must be defined. 

RETURN VALUE
returns always TRUE.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_pin_config_pcil
(
 uint32 *mask, 
 uint32 *value
);


/*===========================================================================

FUNCTION      HSU_PIN_SEL_PCIL

DESCRIPTION
PCIL function that does usb pin selection.
This pcil function is a wrapper for HWIO_USB_PIN_SEL_OUTM.

DEPENDENCIES
FEATURE_HS_USB must be defined. 

RETURN VALUE
returns always TRUE.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_pin_sel_pcil
(
 uint32 *mask,
 uint32 *value
);


/*===========================================================================

FUNCTION      HSU_REGISTER_PCIL_CMDS

DESCRIPTION
Registers the functions in hsu_mdm_apis_pcil.c with proc_comm.

DEPENDENCIES
FEATURE_HS_USB must be defined. 

RETURN VALUE
TRUE if all functions were successfully registered.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_register_pcil_cmds(void);

#endif /* HSU_MDM_APIS_PCIL_H */