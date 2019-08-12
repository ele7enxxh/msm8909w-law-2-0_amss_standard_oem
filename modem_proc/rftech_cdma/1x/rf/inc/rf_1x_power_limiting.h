#ifndef RF_1X_POWER_LIMITING_H
#define RF_1X_POWER_LIMITING_H

/*!
  @file
  rf_1x_power_limiting.h

  @details
  This file exports the APIs needed to operate and update 1x power limiting
  module. Note that data structures are common between 1x and HDR, hence are
  defined in the rf_cdma_power_limiting.h file
*/

/*==============================================================================

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_power_limiting.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/06/12   hdz     Added rf_1x_plim_update_external_limit()
07/24/12   hdz     Changed rf_1x_plim_get_lowest_max_power_limit() interface
                   to support tx_pwr_limit override
02/02/12   cd      Changed the get lowest max power limit function to be 
                   a public API 
01/27/12   hdz     Removed rf_1x_plim_update_temp and 
                           rf_1x_plim_update_temp_mdsp 
01/19/12   cd      Added linearizer limit configuration API to PLIM module
12/08/11   hdz     Added rf_1x_plim_update_temp()
10/12/11   bmg     Added DSI SAR control to target max power limit
09/06/11   shb     Added rf_1x_plim_get_err_adj_max_limit()
08/29/11   shb     Added rf_1x_plim_enable/disable_override
08/18/11   shb     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfc_cdma.h"
#include "rf_cdma_power_limiting.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean
rf_1x_plim_start
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rf_cdma_plim_data_type* plim_data,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_plim_stop
(
  rfm_device_enum_type device,
  rf_cdma_plim_data_type* plim_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_plim_update_tx_err_from_hdet
(
  rfm_device_enum_type device,
  int16 tx_err_db10
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_plim_update_sar_state
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_plim_enable_override
(
  rfm_device_enum_type device,
  int16 min_plim_dbm10,
  int16 max_plim_dbm10
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_plim_disable_override
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_plim_get_err_adj_max_limit
(
  rfm_device_enum_type device,
  int16* max_plim_err_adj_dbm10
);

/*----------------------------------------------------------------------------*/
void
rf_1x_plim_update_temp_common
(
  rf_cdma_plim_data_type* plim_data,
  rfm_device_enum_type device,
  const rf_cdma_config_type* rf_cdma_config_data_ptr,
  const rfc_cdma_logical_device_params_type* logical_dev
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_plim_update_lin_limit
(
  rfm_device_enum_type device,
  int16 tx_lin_limit_db640
);

/*----------------------------------------------------------------------------*/
int16
rf_1x_plim_get_lowest_max_power_limit
(
  rf_cdma_max_power_limits_type* max_power_limits,
  rfm_device_enum_type device 
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_plim_update_external_limit
(
  rfm_device_enum_type device,
  boolean tx_pwr_override, 
  int16 tx_pwr_limit_dbm
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_plim_update_lim_vs_temp_vs_freq
(
  rfm_device_enum_type device,
  rf_cdma_plim_data_type* plim_data,
  rfm_cdma_band_class_type band,
  uint16 channel,
  uint16 last_valid_therm_read
);

#ifdef __cplusplus
}
#endif

#endif  /* RF_1X_POWER_LIMITING_H */
