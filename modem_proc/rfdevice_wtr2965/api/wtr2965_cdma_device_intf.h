#ifndef WTR2965_CDMA_DEVICE_INTF_H
#define WTR2965_CDMA_DEVICE_INTF_H
/*! 
  @file
  wtr2965_cdma_device_intf.h
 
  @brief
  Contains the interface for RFC to WTR2965 CDMA driver

  @details
  Contains function prototypes to create WTR2965 CDMA devices and any other
  direct configuration from RFC like setting band/ports, selecting TX LUTs etc

  This header is wtr2965 specific and hence must be included by WTR2965 RF cards 
  only.

  This file will have to be included by RFC to communicate with WTR2965 and must 
  be shipped.

  @addtogroup WTR2965_CDMA
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

===============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_wtr2965/api/wtr2965_cdma_device_intf.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/07/14   zhw     Compiler fix for non-CDMA configuration
11/20/13   rp      Moved cdma get device to wtr2965_cdma.h to support class based 
                   implementation for Common device driver.
10/31/13   shb     Update create_device API to accept rfdevice_rxtx_common_class 
                   ptr instead of wtr2965_rxtx_common_class ptr
09/23/13   shb     Moved wtr2965_cdma_rx/tx_set_band_port from 
                   wtr2965_cdma_device_intf.h to wtr2965_cdma.h
09/17/13   dps     Modified wtr2965_cdma_get_device for CDMA C++ objects
08/01/12   shb     Added return status for wtr2965_cdma_rx/tx_set_band_port
10/06/11   shb     Increased # of supported CDMA devices to 2 for SV cards
07/17/11   shb     Added APIs to set WTR2965 PRx/TX band port mapping via RFC
07/14/11   shb     Initial Revision

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X

#include "rfdevice_rxtx_common_class.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "rfdevice_cdma_interface.h"
#include "rfdevice_cmn_intf.h"

/*! Max number of devices supported by WTR2965 */
#define WTR2965_MAX_CDMA_DEVICES 2

/*----------------------------------------------------------------------------*/
boolean
wtr2965_cdma_create_device
(
  rfdevice_rxtx_common_class* common
);

/*! @} */

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_CDMA1X */

#endif /* WTR2965_CDMA_DEVICE_INTF_H */
