#ifndef WTR4905_COMMON_DEVICE_INTF_H
#define WTR4905_COMMON_DEVICE_INTF_H
/*! 
  @file
  wtr4905_common_device_intf.h
 
  @brief
  Contains the interface for RFC to WTR4905 Common driver

  @details
  Contains function prototypes to create WTR4905 common devices and any other
  direct configuration from RFC.

  This header is wtr4905 specific and hence must be included by WTR4905 RF cards 
  only.

  This file will have to be included by RFC to communicate with WTR4905 and must 
  be shipped.

  @addtogroup WTR4905_COMMON
  @{
*/

/*==============================================================================

  Copyright (c) 2011-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_wtr4905/api/wtr4905_common_device_intf.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/24/14   zhw     WTR4905 HDET support
11/20/13   rp      Changes to support class based implementation for Common 
                   device driver.
09/24/13   rp      Cpp support.
08/12/13   tks     Added support for wxe & trx rffe initialization 
06/10/13   tks     Added support for rffe interface control 
05/01/13   shb     Pass efs_file_path string to legacy device factory - used by 
                   WTR4905/WFR1620
03/07/13  bmg/shb  Added "destructor"
02/24/12   shb     Added wtr4905_common_do_internal_device_cal() to do all 
                   internal cal (RC tuner + HDET autocal) for WTR4905
02/14/12   shb     Added argument to send internal device cal data during device
                   creation
10/06/11   shb     Increased # of supported common devices to 2 for SV cards
07/14/11   shb     Initial Revision

==============================================================================*/

#include "rfdevice_cmn_intf.h"
#include "rfc_common.h"
#include "rfdevice_rxtx_common_class.h"


#ifdef __cplusplus
extern "C" {
#endif


/*! Max number of Common devices supported by WTR4905 */
#define WTR4905_MAX_DEVICES 2

/*----------------------------------------------------------------------------*/
rfdevice_rxtx_common_class*
wtr4905_common_create_device
(
  rfc_device_cfg_info_type wtr4905_rffe_info,
  rfdevice_cmn_int_dev_cal_data_type cal_data,
  const char* efs_file_path
);

boolean wtr4905_common_create_wxe_device
(
  rfdevice_rxtx_common_class* instance,
  rfc_device_cfg_info_type* cfg
);

/*----------------------------------------------------------------------------*/
rfdevice_class*
wtr4905_common_create_hdet_device
(
  rfdevice_rxtx_common_class* common_device
);

#ifdef __cplusplus
}
#endif


/*! @} */

#endif /* WTR4905_COMMON_DEVICE_INTF_H */
