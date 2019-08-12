#ifndef WTR2965_TDSCDMA_DEVICE_INTF_H
#define WTR2965_TDSCDMA_DEVICE_INTF_H
/*! 
  @file
  wtr2965_tdscdma_device_intf.h
 
  @brief
  Contains the interface for RFC to WTR2965 TDSCDMA driver

  @details
  Contains function prototypes to create WTR2965 TDSCDMA devices and any other
  direct configuration from RFC like setting band/ports, selecting TX LUTs etc

  This header is wtr2965 specific and hence must be included by WTR2965 RF cards 
  only.  This file will have to be included by RFC to communicate with WTR2965
  and must be shipped.

  @addtogroup WTR2965_TDSCDMA
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

===============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/25/15   bm      Increased max num TDSCDMA instances to 2
11/20/13   rp      Changes to support class based implementation for Common 
                   device driver.
11/08/13   rp      Update create_device API to accept rfdevice_rxtx_common_class 
                   ptr instead of wtr2965_rxtx_common_class ptr
11/08/13   rp      Class based implementation for TDSCDMA device driver.
04/26/13   tks     Added support for rx2 and rx3 receive chains
08/02/11   jyu     Initial Revision

==============================================================================*/

#include "rfdevice_rxtx_common_class.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rfdevice_tdscdma_intf.h"
#include "rfdevice_cmn_intf.h"
#include "rfcom.h" 

/*! Max number of devices supported by WTR2965 */
#define WTR2965_MAX_TDSCDMA_DEVICES 2

/*----------------------------------------------------------------------------*/
boolean
wtr2965_tdscdma_create_device
(
  rfdevice_rxtx_common_class* common
);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* WTR2965_TDSCDMA_DEVICE_INTF_H */



