#ifndef RFLM_NLIC_UTIL_H
#define RFLM_NLIC_UTIL_H

/*!
  @file
  rflm_nlic_util.h

  @brief
  Export the API necessary for calling modules to get the NLIC dynamic settings
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/nlic/inc/rflm_nlic_util.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/03/32   cri     Initial check in

==============================================================================*/

/*TO BE DELETED*/

#include "rflm_nlic_settings_ag.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 0
/*----------------------------------------------------------------------------*/
boolean
rflm_nlic_get_dynamic_settings
(
  uint32 tx_aggr_freq, 
  uint32 rx_vict_freq, 
  uint32 ul_bw,
  uint32 dl_bw, 
  uint8 num_rb,
  rflm_nlic_data_type *nlic_data, 
  int32 rssi,
  int32 tx_power
);
#endif 

#ifdef __cplusplus
}
#endif

#endif /*RFLM_NLIC_UTIL_H*/

