
#ifndef WTR4905_GNSS_DEVICE_INTF_H
#define WTR4905_GNSS_DEVICE_INTF_H
/*! 
  @file
  wtr4905_gnss_device_intf.h
 
  @brief
  Contains the interface for RFC to WTR4905 GNSS driver

  @details
  Contains function prototypes to create WTR4905 GNSS devices

  This header is wtr4905 specific and hence must be included by WTR4905 RF cards 
  only.

  This file will have to be included by RFC to communicate with WTR4905 and must 
  be shipped.

  @addtogroup WTR4905_GNSS
  @{
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_wtr4905/api/wtr4905_gnss_device_intf.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/07/13   shb/dbz Added support for GNSS to operate on WTR4905 common instance 1
07/19/11   dbz     Initial Revision

==============================================================================*/

#include "rfdevice_cmn_intf.h"

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Initialize GNSS device pointers.

   @details

   @param path: rf device path.

   @retval None.
*/
void rfdevice_wtr4905_gnss_config(rfgnss_path_enum_type rfgnss_path);

/*----------------------------------------------------------------------------*/
extern boolean
rfdevice_wtr4905_gnss_set_instance_id
(
  uint8 instance_id
);

/*! @} */

#endif /* WTR4905_GNSS_DEVICE_INTF_H */
