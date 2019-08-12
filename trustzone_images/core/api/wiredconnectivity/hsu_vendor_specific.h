#ifndef _HSU_VENDOR_SPECIFIC_H_
#define _HSU_VENDOR_SPECIFIC_H_

/*=============================================================================

        High Speed USB Vendor Specific command handler
                  H E A D E R   F I L E
=============================================================================*/
 
/**
@file hsu_vendor_specific.h

Defines the function that handles vendor-specific commands received from the host.

The hsu_vendor_cmd_in_cb() function is externalized.

There are no initialization and sequencing requirements.
*/

/*=============================================================================
  Copyright (c) 2008-2010 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_vendor_specific.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
    
  when     who      what, where, why
  -------- ---    ---------------------------------------------------------
  06/25/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
  05/27/08  tl    Initial version
      
=============================================================================*/

#include "jtypes.h"


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
/** Codes for vendor-specific messages.
*/
enum hsu_vendor_requests_t
{
  HSU_VENDOR_SPECIFIC_ENABLE_BYTE_PAD = 0x81
       /**< -- Enable byte padding (this is a workaround for the 1_2 byte
               error). */
};

/** Describes the strings that are sent in the device/configuration descriptor.

These definitions are used as indices of the hsu_vendor_specific_str_idx_arr 
(defined in hsu_vendor_specific.c). The hsu_vendor_specific_str_idx_arr  is
used to determine in which index the specific string (e.g., HSU_STR_IDX_PRODUCT
-> product name) is saved.

This index is sent to the host at enumeration (e.g., if 
hsu_vendor_specific_str_idx_arr[HSU_STR_IDX_PRODUCT] = 1, then iProduct = 0x01).
 */
typedef enum 
{ 
  HSU_STR_IDX_PRODUCT = 0,   /**< -- Index string for the product name. */
  HSU_STR_IDX_MANUFACTURER,  /**< -- Index string for the product
                                     manufacturer. */
  HSU_STR_IDX_SERIAL_NUMBER, /**< -- Index string for the serial number. */
  HSU_STR_IDX_CONFIGURATION, /**< -- Index string for the product
                                     configuration. */
  HSU_STR_IDX_LAST,          /**< -- Index string for the size of the
                                     enumerator. */
  HSU_STR_IDX_NOT_SET = 0xFF /**< -- Index string has not been assigned as it
                                     marks the last value. */
}hsu_str_idx_desc_type;


/*=============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_vendor_cmd_in_cb        
=============================================================================*/
/** 
Callback for a vendor-specific (IN) command that is received from the host.
    
@param bRequest           [IN] -- Request code.
@param wValue             [IN] -- Request value.
@param wIndex             [IN] -- Request index.
@param *resp_buf          [OUT] -- Response buffer.
@param resp_buf_size      [IN] -- Response buffer size.
@param *act_resp_buf_size [OUT] -- Actual response buffer size.

@dependencies
None.

@sideeffects
None.

@return
Zero (0) for success, or an error code
*/
jresult_t hsu_vendor_cmd_in_cb(juint8_t bRequest, juint16_t wValue,
    juint16_t wIndex, juint8_t *resp_buf, juint16_t resp_buf_size, 
    juint16_t *act_resp_buf_size);

/*==============================================================================
FUNCTION     hsu_vendor_specific_is_idx_occupied     
==============================================================================*/
/** 
Checks if a specified index is occupied by another string.

For more information, see the documentation for @latexonly\hyperlink{hsu__vendor__
specific_8h_cfc92c9eea99ee96b46f91c76c7d4ca2}{hsu\_\-str\_\-idx\_\-desc\_\-type}
@endlatexonly.

@param idx_to_check [IN] -- Index to check whether it is occupied or not.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The specified index is already occupied.\n
FALSE -- The specified index is not occupied.
*/
boolean hsu_vendor_specific_is_idx_occupied(juint8_t idx_to_check);

/*==============================================================================
FUNCTION     hsu_vendor_specific_get_str_idx     
==============================================================================*/
/** 
Returns the index that was reserved for the specified string (from the
device/configuration string descriptors).

For more information, see the documentation for @latexonly\hyperlink{hsu__vendor__
specific_8h_cfc92c9eea99ee96b46f91c76c7d4ca2}{hsu\_\-str\_\-idx\_\-desc\_\-type}
@endlatexonly.

@param str_desc [IN] -- Index of the string descriptor.

@dependencies
None.

@sideeffects
None.

@return
Index of the specified string or HSU_STR_IDX_NOT_SET.
*/
juint8_t hsu_vendor_specific_get_str_idx(hsu_str_idx_desc_type str_desc);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_VENDOR_SPECIFIC_H_ */
