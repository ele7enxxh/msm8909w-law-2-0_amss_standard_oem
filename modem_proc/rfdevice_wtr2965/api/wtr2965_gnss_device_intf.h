
#ifndef WTR2965_GNSS_DEVICE_INTF_H
#define WTR2965_GNSS_DEVICE_INTF_H
/*! 
  @file
  wtr2965_gnss_device_intf.h
 
  @brief
  Contains the interface for RFC to WTR2965 GNSS driver

  @details
  Contains function prototypes to create WTR2965 GNSS devices

  This header is wtr2965 specific and hence must be included by WTR2965 RF cards 
  only.

  This file will have to be included by RFC to communicate with WTR2965 and must 
  be shipped.

  @addtogroup WTR2965_GNSS
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_wtr2965/api/wtr2965_gnss_device_intf.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/07/13   shb/dbz Added support for GNSS to operate on WTR2965 common instance 1
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
void rfdevice_wtr2965_gnss_config(rfgnss_path_enum_type rfgnss_path);

/*----------------------------------------------------------------------------*/
extern boolean
rfdevice_wtr2965_gnss_set_instance_id
(
  uint8 instance_id
);

/*! @} */

#endif /* WTR2965_GNSS_DEVICE_INTF_H */
