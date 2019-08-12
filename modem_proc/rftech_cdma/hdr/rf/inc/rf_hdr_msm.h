#ifndef RF_HDR_MSM_H
#define RF_HDR_MSM_H

/*!
  @file
  rf_hdr_msm.h
 
  @brief
  RF Driver's HDR CDMA Modem HAL.

  @details
  This file includes definitions and prototypes used by the CDMA MOdem HAL.
 
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/inc/rf_hdr_msm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/03/14   sty     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_cdma_carrier_types.h"
#include "rfm_device_types.h"
#include "rf_cdma_msm.h" /* for rf_cdma_msm_sample_type */

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_msm_get_last_n_samples
(
  rfm_device_enum_type device,
  rfm_cdma_carrier_type carrier,
  unsigned short num_iq_samples,
  rf_cdma_msm_sample_type* buffer
);

#ifdef __cplusplus
}
#endif

#endif /* RF_HDR_MSM_H */

