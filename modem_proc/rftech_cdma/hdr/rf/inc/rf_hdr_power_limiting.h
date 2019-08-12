#ifndef RF_HDR_POWER_LIMITING_H
#define RF_HDR_POWER_LIMITING_H

/*!
  @file
  rf_hdr_power_limiting.h

  @details
  This file exports the APIs needed to operate and update hdr power limiting
  module. Note that data structures are common between 1x and HDR, hence are
  defined in the rf_cdma_power_limiting.h file
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/inc/rf_hdr_power_limiting.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/15/13   APU     Added changes for Enhanced MCDO coonection setup.
01/30/13   APU     Brought over Enhanced PA backoff code from Nikel.
08/24/12   cd      CDMA EPT Temp and HDET feature support
03/21/12   APU     Added rf_hdr_get_max_tx_power_for_band() API 
02/16/12   APU     Changed signature of rf_hdr_calculate_max_tx_power_limit()to
                   return boolean for success and failure
02/16/12   APU     Changed signature of rf_hdr_calculate_max_tx_power_limit()
02/07/12   cd      Changed the get lowest max power limit function to be 
                   a public API 
02/07/12   APU     Added NV support for MC-HDR power limiting.
01/27/12   hdz     Removed rf_hdr_plim_update_temp and 
                           rf_hdr_plim_update_temp_mdsp 
01/19/12   cd      Added linearizer limit configuration API to PLIM module
12/08/11   hdz     Added rf_hdr_plim_update_temp()
11/14/11   Saul    Thermal Backoff. Made implementation more modular.
11/11/11   Saul    Thermal Backoff Support.
11/10/11   Saul    SV Limiting. Initial support.
10/12/11   bmg     Added DSI SAR control to target max power limit
09/06/11   shb     Added rf_hdr_plim_get_err_adj_max_limit()
08/30/11   shb     Initial version - based on rf_1x_power_limiting.h

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfc_cdma.h"
#include "rf_cdma_power_limiting.h"
#include "rfm_hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

boolean
rf_hdr_plim_set_max_pwr_per_modified_ccp ( 
   rfm_device_enum_type device , 
   const rfm_cdma_chan_type* const band_chan_array,
   const uint8 num_carriers  , int16* max_power_limit_selected_dB10 ) ;

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_get_max_tx_power_for_band ( rfm_device_enum_type device , int16* max_power_limit_in_band_dB10 ) ;

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_calculate_max_tx_power_limit ( rfm_device_enum_type device , 
                                      rf_cdma_plim_data_type* plim_data,
                                      const rfc_cdma_logical_device_params_type* logical_dev ,
                                      int16 max_power[] , rfm_cdma_chan_type override_channels[] ) ;

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_plim_start
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rf_cdma_plim_data_type* plim_data,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_plim_stop
(
  rfm_device_enum_type device,
  rf_cdma_plim_data_type* plim_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_plim_update_tx_err_from_hdet
(
  rfm_device_enum_type device,
  int16 tx_err_db10
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_plim_update_sar_state
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_plim_update_sv_limit
(
  rfm_device_enum_type device,
  int16 max_power_limit_dbm10
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_plim_enable_override
(
  rfm_device_enum_type device,
  int16 min_plim_dbm10,
  int16 max_plim_dbm10
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_plim_disable_override
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_plim_get_err_adj_max_limit
(
  rfm_device_enum_type device,
  int16* max_plim_err_adj_dbm10
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_plim_update_thermal_backoff_limit
( 
  rfm_device_enum_type device, 
  rfm_hdr_thermal_state_type thermal_backoff_state
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_plim_update_temp_common
(
  rf_cdma_plim_data_type* plim_data,
  rfm_device_enum_type device,
  const rf_cdma_config_type* rf_cdma_config_data_ptr,
  const rfc_cdma_logical_device_params_type* logical_dev
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_plim_update_lin_limit
(
  rfm_device_enum_type device,
  int16 tx_lin_limit_dBm10
);

/*----------------------------------------------------------------------------*/
int16
rf_hdr_plim_get_lowest_max_power_limit
(
  rf_cdma_max_power_limits_type* max_power_limits
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_plim_update_lim_vs_temp_vs_freq
(
  rfm_device_enum_type device,
  rf_cdma_plim_data_type* plim_data,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  uint16 last_valid_therm_read
);

/*----------------------------------------------------------------------------*/
boolean 
rf_hdr_plim_get_funneling_metrics
(
  rfm_device_enum_type device ,
  rfm_hdr_per_carrier_funneling_bias_type* funnel_bias_array_ptr
) ;

#ifdef __cplusplus
}
#endif

#endif  /* RF_HDR_POWER_LIMITING_H */
