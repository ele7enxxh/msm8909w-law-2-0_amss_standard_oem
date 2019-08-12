#ifndef HSU_USB_UICC_H
#define HSU_USB_UICC_H

/*==============================================================================

                  H S U _ U S B _ U I C C . H
==============================================================================*/

/**
@file hsu_usb_uicc.h

Defines the HSUSB UICC function that is used from outside the HSUSB.

The externalized function is hsu_uicc_usb_shutdown().

There are no initialization and sequencing requirements.
*/

/*============================================================================
Copyright (c) 2008-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
==============================================================================*/


/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when      who     what, where, why
--------  ---     --------------------------------------------------------------
06/23/10  sw      (Tech Pubs) Edited/added Doxygen comments and markup.
05/15/08  msh     Initial version

==============================================================================*/


/*==============================================================================

INCLUDE FILES

==============================================================================*/


/** @ingroup hsusb_qc_api 
    @{
*/

/*=============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/
/*=============================================================================
Typedefs
=============================================================================*/

/*=============================================================================
Constants and Macros
=============================================================================*/

/*=============================================================================

FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
FUNCTION    hsu_uicc_usb_shutdown
=============================================================================*/
/**
Allows the UIM to shut down the UICC when enumeration fails or when the UICC 
connect fails.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_uicc_usb_shutdown(void);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* HSU_USB_UICC_H */
