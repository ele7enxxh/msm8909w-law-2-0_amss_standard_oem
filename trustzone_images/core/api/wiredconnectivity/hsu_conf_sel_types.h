#ifndef _HSU_CONF_SEL_TYPES_H_
#define _HSU_CONF_SEL_TYPES_H_

/*=============================================================================

     High Speed USB configuration Selector Module - Type Definitions
        I N T E R N A L  H E A D E R   F I L E

=============================================================================*/
/**
@file hsu_conf_sel_types.h

Type definitions used by the various configuration selector files.

These definitions include enumeration types of possible FDs, serial numbers, etc.

*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_conf_sel_types.h#1 $
  $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  06/24/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
  06/29/07  bt  Added MTP support
  04/22/07  me  Adding constants for ESN/IMEI based serial number support
  09/12/06  ke  Initial version

=============================================================================*/

/*=============================================================================
Copyright (c) 2007-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

#include "nv_items.h"

/** @ingroup hsusb_qc_api
    @{
*/
/*=============================================================================
  Forward Declarations
=============================================================================*/

/*=============================================================================
  Constants
=============================================================================*/

/** Maximum length for the serial number.
*/
#define HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN (255-2)/2 + 1

/** Maximum serial number length as per the USB Mass Storage Class
    Specification.
    The IMEI is structured from 6 digits for Type Approval Code (TAC), two
    digits for Final Assembly Code (FAC), six digits for Serial Number, and
    one check digit, for a total of 15 digits.

    When reading the serial number from the NV, there are two redundant bytes
    -- one for bcd_len and one ID digit that is being ignored. This is why the
    size of (NV_UE_IMEI_SIZE-1)*2 = 15 is kept.
*/
#define HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE (NV_UE_IMEI_SIZE-1)*2

/** A constant serial number to be used when IMEI/ESN is not available.
*/
#define HSU_CFG_SELECTOR_DEFAULT_SERIAL_NUMBER "1234567890ABCDEF"

/** Minimum length allowed for the serial number according to the USB Mass
    Storage Class Specification.
*/
#define HSU_CFG_SELECTOR_SERIAL_NUMBER_MIN_LEN 12

/*=============================================================================
  Macros
=============================================================================*/

/*=============================================================================
  Typedefs
=============================================================================*/

/* Enumeration of the various FDs supported by the HS-USB device stack.
    Used as the index for the num_of_fd_instances_in_comp array in the type
    hsu_conf_sel_comp_info_type. */
enum
{
  HSU_CFG_SEL_MS_FD = 0,
  HSU_CFG_SEL_OBEX_FD,
  HSU_CFG_SEL_ACM_FD,
  HSU_CFG_SEL_ECM_FD,
  HSU_CFG_SEL_SICD_FD,
  HSU_CFG_SEL_MTP_FD,
  HSU_CFG_SEL_HID_FD,
  HSU_CFG_SEL_LOOPBACK_FD,
  HSU_CFG_SEL_AUDIO_FD,
  HSU_CFG_SEL_NUM_OF_FD_TYPES /* this has to be the last in the list! */
};

/** Describes for each composition if it has standard or proprietary drivers,
    or if its drivers are supported by all operating systems.
*/
typedef enum{
  HSU_PROPRIETARY_CMP_ONLY = 0,  /**< -- Only proprietary PC drivers supported. */
  HSU_STANDARD_CMP_ONLY,         /**< -- Only standard PC drivers supported. */
  HSU_SUPP_ALL_OSES_CMP          /**< -- Drivers are supported by all PC OSs. */
}hsu_conf_sel_supported_drv_by_comp_enum;

/** Number of RDM enum values that represent USB ports. Currently there are
five:
- RDM_USB_MDM_DEV
- RDM_USB_MMC_DEV
- RDM_USB_DIAG_DEV
- RDM_USB_NMEA_DEV
- RDM_USB_NET_WWAN_DEV
 */
#define HSU_NUM_OF_RDM_USB_PORTS 5

/** @} */ /* end_group hsusb_qc_api */

#endif /* _HSU_CONF_SEL_TYPES_H_ */
