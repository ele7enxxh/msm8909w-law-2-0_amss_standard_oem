#ifndef RF_CDMA_SSMA_H
#define RF_CDMA_SSMA_H

/*!
  @file
  rf_cdma_ssma.h
 
  @brief
  This file contains all the interfaces needed for SSMA

  @details
  This file exports the definitions and declartions to be used by 1x and HDR MC 
  layers
*/ 

/*==============================================================================

  Copyright (c) 2011, 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_ssma.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/30/12   zhaow   Updated rf_cdma_init_ssma_recos() 
03/29/12   sty     Added new API rf_cdma_get_spurs_to_suppress()
                   Changed args to rf_cdma_get_ssma_recos()
03/01/12   sty     Renamed rf_cdma_perform_ssma_operations to rf_cdma_get_ssma_recos
                   Added rf_cdma_enable_ssma_debug_msgs()
02/17/12   sty     Added RxLM handle for rf_cdma_perform_ssma_operations()
01/20/12   sty     Changed arg-list in rf_cdma_update_bb_ssma_recos()
01/05/12   sty     Added prototype for rf_cdma_update_ssma_flag()
01/05/12   sty     Added prototype for rf_cdma_update_rf_ssma_recos()
12/23/11   sty     Added prototype for rf_cdma_update_ssma_recos()
                   Changed arg-list for rf_cdma_perform_ssma_operations()
12/21/11   sty     deleted unused rf_cdma_apply_rx_reco()
12/20/11   sty     Changed return type of rf_cdma_perform_ssma_operations()
12/19/11   sty     deleted prototype rf_cdma_update_rf_bb_recos() 
12/15/11   sty     [1] updated arg-list in rf_cdma_get_rx_reco()
                   [2] added new API rf_cdma_perform_ssma_operations
12/14/11   sty     rf_cdma_init_ssma_recos() changed arg-type
12/13/11   sty     Changed var name from rfdevice_1x_rx_recommendation_type to 
                   rfdevice_cdma_rx_recommendation_type
12/10/11   sty     Deleted unsed APIs and added rf_cdma_update_rf_bb_recos
12/06/11   sty     Added rf_cdma_init_ssma_ptr() - may need to call this API in
                   cases where recos need to be nullified dynamically
11/16/11   sty     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfdevice_cdma_interface.h"
#include "rf_cdma_data.h"
#include "rf_cdma_msm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_init_ssma_recos
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_get_ssma_recos
(
  const rfm_device_enum_type device,
  const lm_handle_type curr_rxlm_handle,
  rfm_cdma_band_class_type const band,
  rfm_cdma_chan_type const chan,
  const rfm_device_enum_type paired_device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_update_bb_ssma_recos
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_update_rf_ssma_recos
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_update_ssma_flag
(
  boolean enable_ssma
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_enable_ssma_debug_msgs
(
  rfdevice_cdma_rx_reco_type *ssma_data,
  boolean debug_en
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_get_spurs_to_suppress
(
  rfm_device_enum_type device,
  lm_handle_type rxlm_handle,
  rfm_cdma_band_class_type rf_band,
  rfm_cdma_chan_type rf_chan,
  uint8 num_active_carriers,
  rf_cdma_msm_dynamic_rxlm_cfg_type *const dyn_settings
);

#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_SSMA_H */
