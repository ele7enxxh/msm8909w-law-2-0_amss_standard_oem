#ifndef RF_CDMA_RX_AGC_H
#define RF_CDMA_RX_AGC_H

/*!
  @file
  rf_cdma_rx_agc.h

  @details
  This file exports the definitions and declartions pertaining to RxAGC.
*/

/*==============================================================================

  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_rx_agc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/11/14   spa     Add tech data ptr to rxagc data struct/Jammer detect support
10/09/13   spa     Add rf mode param to configure RxAGC data 
04/11/12   aro     Removed unused RxAGC freeze and unfeeze functions
04/11/12   aro     Documentation Update
02/06/12   hdz     Added rf_cdma_update_rx_agc for reloading rx agc in calls
12/20/10   aro     Added CDMA Freeze/Unfreeze functions
12/09/10   aro     Moved code to LNA specific file
12/08/10   aro     Fixing preprocessor Macros
12/06/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"
#include "lm_types.h"
#include "rfm_device_types.h"
#include "rfm_mode_types.h"
#include "rfm_cdma_band_types.h"
#include "rfm_gain_state_types.h"
#include "rf_cdma_state_types.h"
#include "rfm_cdma_power_mode_types.h"
#include "rf_cdma_mdsp_types.h"
#include "rflm_rxagc_api.h"
#include "rfc_cdma.h"

/*----------------------------------------------------------------------------*/
typedef struct
{

  cell_id_t cell_id;

  /*! Void pointer to the tech specific scrathc area assigned by the common 
    RxAGC module, this is to be cast to the cdma specific data structure at
    MDSP layer. The data is used for jammer detection and other Rx related
    operations */
  void* rxagc_tech_data;
} rf_cdma_rxagc_data_t;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_update_rx_agc
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_configure_rx_agc_data
(
  const rfm_mode_enum_type rf_mode,
  const rfm_device_enum_type device,
  const rfm_cdma_band_class_type band,
  const lm_handle_type rx_handle,
  const rf_cdma_carrier_mode_type carrier_mode,
  const uint32 bw_khz,
  const rfm_cdma_power_mode_type power_mode,
  rf_cdma_mdsp_rx_agc_config_type* rxagc_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_mc_configure_stop_rx_agc_data
(
  const rfm_device_enum_type device,
  const lm_handle_type rx_handle
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_configure_jd_thresh
(
  const rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  const rfm_cdma_band_class_type band
);

#endif  /* RF_CDMA_RX_AGC_H */

