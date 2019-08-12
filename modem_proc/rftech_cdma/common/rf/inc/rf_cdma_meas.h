#ifndef RF_CDMA_MEAS_H
#define RF_CDMA_MEAS_H

/*!
  @file
  rf_cdma_mc.h
 
  @brief
  RF Driver's Common CDMA MC interface file.

  @details
  This file exports the definitions and declartions to be used by Common CDMA
  MC Layer.
*/

/*==============================================================================

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_meas.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/27/15   rs      Added NV control support for FR25072
11/23/15   rs      Add band as argument to wtr power down API
11/23/15   rs      Power down WTR in case of abort 
07/05/13   spa     Added rf_cdma_meas_cleanup_ccs_events 
10/05/12   sty     Initial version

==============================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_meas_cleanup_ccs_events 
( 
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void rf_cdma_mc_meas_init
(
  void 
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_meas_power_down_wtr_for_abort
( 
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band
);
#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_MC_H */

