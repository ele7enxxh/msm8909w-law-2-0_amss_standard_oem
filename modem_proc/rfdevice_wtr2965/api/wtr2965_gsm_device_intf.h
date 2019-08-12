#ifndef WTR2965_GSM_DEVICE_INTF_H
#define WTR2965_GSM_DEVICE_INTF_H
/*! 
  @file
  wtr2965_gsm_device_intf.h
 
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

  Copyright (c) 2011 - 2015 by Qualcomm Technologies, Inc. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_wtr2965/api/wtr2965_gsm_device_intf.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/18/14   tws     Increase WTR2965_MAX_GSM_DEVICES to 2 for CA RFC
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

/*! Max number of devices supported by WTR2965 */
#define WTR2965_MAX_GSM_DEVICES 2

/*----------------------------------------------------------------------------*/
boolean
wtr2965_gsm_create_device
(
  rfdevice_rxtx_common_class* common_device
);

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* WTR2965_CDMA_DEVICE_INTF_H */
