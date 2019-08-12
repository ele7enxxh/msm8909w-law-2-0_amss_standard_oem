#ifndef WTR4905_GSM_DEVICE_INTF_H
#define WTR4905_GSM_DEVICE_INTF_H
/*! 
  @file
  wtr4905_gsm_device_intf.h
 
  @brief
  Contains the interface for RFC to WTR4905 CDMA driver

  @details
  Contains function prototypes to create WTR4905 CDMA devices and any other
  direct configuration from RFC like setting band/ports, selecting TX LUTs etc

  This header is wtr4905 specific and hence must be included by WTR4905 RF cards 
  only.

  This file will have to be included by RFC to communicate with WTR4905 and must 
  be shipped.

  @addtogroup WTR4905_CDMA
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 by Qualcomm Technologies, Inc. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_wtr4905/api/wtr4905_gsm_device_intf.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/06/15   sd      Increased Max number of GSM devices to 2
12/19/13   rp      Removed additional header files.
11/15/13  rp/dps   Class based implementation for GSM device driver.
07/11/12   sr      Added extern "C" conditional compile.
09/27/11   vrb     Push config info from RFC instead of querying from WTR lib
07/17/11   vrb     Added API to create GSM device

==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "rfdevice_rxtx_common_class.h"

/*! Max number of devices supported by WTR4905 */
#define WTR4905_MAX_GSM_DEVICES 2

/*----------------------------------------------------------------------------*/
boolean
wtr4905_gsm_create_device
(
  rfdevice_rxtx_common_class* common_device
);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* WTR4905_CDMA_DEVICE_INTF_H */
