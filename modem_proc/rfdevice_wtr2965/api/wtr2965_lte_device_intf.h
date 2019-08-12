/*!
  @file wtr2965_lte_api.h

  @brief
  Exports prototypes for WTR2965 device interface maping functions.

  @addtogroup WTR2965_LTE
  @{
*/

/*============================================================================== 
   
  Copyright (c) 2011-2015 Qualcomm Technologies Incorporated. All Rights Reserved 
   
  Qualcomm Proprietary 
   
  Export of this technology or software is regulated by the U.S. Government. 
  Diversion contrary to U.S. law prohibited. 
   
  All ideas, data and information contained in or disclosed by 
  this document are confidential and proprietary information of 
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved. 
  By accepting this material the recipient agrees that this material 
  and the information contained therein are held in confidence and in 
  trust and will not be used, copied, reproduced in whole or in part, 
  nor its contents revealed in any manner to others without the express 
  written permission of Qualcomm Technologies Incorporated. 
   
==============================================================================*/ 

/*==============================================================================
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 
 
$DateTime: 2016/12/13 08:00:04 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_wtr2965/api/wtr2965_lte_device_intf.h#1 $ 
   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
11/20/13   rp      Changes to support class based implementation for Common 
                   device driver.
10/31/13   shb     Update create_device API to accept rfdevice_rxtx_common_class 
                   ptr instead of wtr2965_rxtx_common_class ptr
10/10/13   rp      Class based implementation of LTE device driver.
04/26/13   tks     Add support for 3rd and 4th receive path instance
11/20/12   shb     Fixed doxygen warning
08/02/12   shb     Moved wtr2965_lte_set_rx/tx_port() from 
                   wtr2965_lte_api.h to wtr2965_lte.h
07/13/12   vss     Add support for LTE instance 1 
07/12/12   sr      Add conditional extern 'C'  
04/03/12   shb     Deleted unused APIs: rfdevice_lte_wtr2965_<rx/tx>_config
07/18/11   aca     Interface Update- WTR 1605 development
07/07/11   aca     Multi device support
07/05/11   aca     Initial version.
==============================================================================*/ 

#ifndef WTR2965_LTE_API_H
#define WTR2965_LTE_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================== 
 
                                 INCLUDE FILES
 
==============================================================================*/

#include "rfcom.h"
#include "rfdevice_lte_interface.h"
#include "rfdevice_cmn_intf.h"

#include "rfdevice_rxtx_common_class.h"


/*============================================================================== 
 
                                DATA DEFINITIONS
 
==============================================================================*/
/*! Max number of devices supported by WTR2965 */
#define WTR2965_MAX_LTE_DEVICES 2


/*============================================================================== 
 
                             FUNCTION DECLARATIONS
 
==============================================================================*/

boolean
wtr2965_lte_create_device
(
  rfdevice_rxtx_common_class* common
);


#ifdef __cplusplus
}
#endif

/*! @} */

#endif  /* wtr2965_API_H */
