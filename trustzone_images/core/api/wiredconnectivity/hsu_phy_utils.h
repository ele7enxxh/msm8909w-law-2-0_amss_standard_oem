#ifndef _HSU_PHY_UTILS_H_
#define _HSU_PHY_UTILS_H_

/*=============================================================================

     High Speed USB PHY Utilities
        H E A D E R   F I L E
=============================================================================*/
/**
@file hsu_phy_utils.h

Defines utilities functions for the PHY.

Externalized functions:
- hsu_phy_utils_spoof_disconnect
- hsu_phy_utils_spoof_connect
*/

/*=============================================================================    
Copyright (c) 2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
  
$Header: 
$DateTime: 
    
when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/11/10 sm  Exposing spoof-disconnect/connect functions.
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

/*=============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*===========================================================================
FUNCTION    hsu_phy_utils_spoof_disconnect
=============================================================================*/
/**
This function causes the device to disconnect from host, 
while USB cable can still be attached.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- Operation successful.\n
FALSE -- Operation was not successful.
*/
boolean hsu_phy_utils_spoof_disconnect(void);

/*===========================================================================
FUNCTION hsu_phy_utils_spoof_connect
=============================================================================*/
/**
This function causes the device to connect to the host, 
after calling the hsu_phy_utils_spoof_disconnect() function.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- Operation successful.\n
FALSE -- Operation was not successful.
*/
boolean hsu_phy_utils_spoof_connect(void);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_PHY_UTILS_H_ */
