#ifndef WTR4905_WCDMA_DEVICE_INTF_H
#define WTR4905_WCDMA_DEVICE_INTF_H
/*! 
  @file
  wtr4905_wcdma_device_intf.h
 
  @brief
  Contains the interface for RFC to WTR4905 WCDMA driver

  @details
  Contains function prototypes to create WTR4905 WCDMA devices and any other
  direct configuration from RFC like setting band/ports, selecting TX LUTs etc

  This header is wtr4905 specific and hence must be included by WTR4905 RF cards 
  only.  This file will have to be included by RFC to communicate with WTR4905
  and must be shipped.

  @addtogroup WTR4905_WCDMA
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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
11/20/13   rp      Changes to support class based implementation for Common 
                   device driver.
10/31/13   shb     Update create_device API to accept rfdevice_rxtx_common_class 
                   ptr instead of wtr4905_rxtx_common_class ptr
10/15/13   rp      WTR4905 WCDMA device driver modification to support C++ interface.
08/02/13   bm      Removed Rx set band port and Tx set band port interfaces
04/25/12   tks     Expanded device type structure to support rx2 and rx3 
11/27/12   tks     Updated interface functions to accept rfm_device as 
                   input parameter
09/28/12   swb     Add support for RFC virtual PA mapping
08/10/12   tks     Added support for dual WTR instance for W
08/02/12   shb     Moved wtr4905_wcdma_rx/tx_set_band_port back as they are used 
                   directly by DIME RFC 
08/02/12   shb     Moved wtr4905_wcdma_rx/tx_set_band_port from 
                   wtr4905_wcdma_devices.c to wtr4905_wcdma.c 
07/10/12   tks     Added interface functions to push band port information from 
                   RFC to device 
07/14/11   shb     Initial Revision

==============================================================================*/

#include "rfdevice_rxtx_common_class.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rfdevice_wcdma_intf.h"
#include "rfdevice_cmn_intf.h"
#include "rfcom.h" 

/*! Max number of devices supported by WTR4905 */
#define WTR4905_MAX_WCDMA_DEVICES 2

boolean
wtr4905_wcdma_create_device
(
  rfdevice_rxtx_common_class* common
);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* WTR4905_WCDMA_DEVICE_INTF_H */



