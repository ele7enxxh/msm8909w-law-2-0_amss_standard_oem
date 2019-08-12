#ifndef _HSU_APP_APIS_H_
#define _HSU_APP_APIS_H_

/*=============================================================================

High Speed USB Module Remote APIs on the Application processor
      H E A D E R   F I L E

=============================================================================*/

/**
@file hsu_app_apis.h
  
Defines High-Speed USB APIs that are implemented on the 
application processor.  

The function hsu_api_get_product_id is an externalized function.

The function hsu_config_selector_init() should be called before any other
function in this file is called.
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
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_app_apis.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  06/23/10  lo  (Tech Pubs) Additional edits for Rev. B of USB API Ref. Guide.
  06/02/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
  02/06/07  ke  Initial version
    
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

/*============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
============================================================================*/

/*============================================================================
FUNCTION      hsu_api_get_product_id
=============================================================================*/
/**
Returns the product ID of the current HS-USB device composition.

@dependencies
FEATURE_HS_USB must be defined. 
Must be called after hsu_config_selector_init() was called.

@sideeffects
None.

@return
The product ID in use.
*/
unsigned short hsu_api_get_product_id(void);
/*~ FUNCTION hsu_api_get_product_id */


/*=============================================================================
FUNCTION      hsu_api_chg_force_resume
=============================================================================*/
/**
This function has been deprecated and should not be used.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_api_chg_force_resume(void);
/*~ FUNCTION hsu_api_chg_force_resume */

/*=============================================================================
FUNCTION      hsu_api_exit_lpm
=============================================================================*/
/**
Enables the HS-USB core running on the ARM9&tm; &nbsp; processor to issue an
LPM exit.

This function enables the use of the PMIC OTG comparators instead of the
integrated PHY comparators.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_api_exit_lpm(void);
/*~ FUNCTION hsu_api_exit_lpm */

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_APP_APIS_H_ */
