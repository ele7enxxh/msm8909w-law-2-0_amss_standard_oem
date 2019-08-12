#ifndef RF_CDMA_TXAGC_H
#define RF_CDMA_TXAGC_H

/*!
  @file
  rf_cdma_txagc.h

  @details
  This file exports the definitions and declartions to be used Tx linearizers
  core.
*/

/*==============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_tx_agc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/10/14   zhw     Dynamic Predictive Cliping support
08/01/14   cd      Add support for FTM AGC logging with FED and common tx AGC 
                   functionality
04/16/14   cd      Cleanup unused MDSP interfaces and Tx AGC functionality
11/08/13   cc      1x logging enhancement
01/08/14   cd      Provide xPT type to RFLM per PA state 
12/13/13   sty     deleted unused rf_cdma_tx_agc_get_template_id
11/14/13   cd      Added generic API to query CDMA Tx device pointers
10/16/13   cc      Add new ET logging for FTM AGC logs 
09/27/13   cd      Hook up common load linearizer interface to CDMA 
09/26/13   cd      Added functionality to load to RFLM through common TxAGC 
04/17/13   zhw     Support for variable transaction in QPOET/PA TxAGC templates
04/12/13   cd      Remove max PA state functionality
04/02/13   cd      Allow skipping of PA states when computing max valid PA 
                   state. Provide new API for valid/invalid PA state
03/28/13   zhw     Add support for getting apt value from linearizer
03/20/13   cd      XPT Temp comp support
12/11/12   Saul    CDMA. TX AGC XPT Dev. Get max PA state based on lin NV.
12/05/12   spa     Added RF mode to load linearizer API
11/29/12   zhw     PA API refactor and make pa_script API static
10/29/12   zhw     Support for query device_config from RFC and device driver
10/23/12   zhw     Remove hardcoding in get_template_id()
10/02/12   zhw     Support for PA state setting for SSBI/RFFE PA
09/26/12   zhw     Get Template ID API based on RFM device
08/24/12   cd      CDMA EPT Temp and HDET feature support
07/11/12   cri     Added EPT support for CDMA
07/09/12   cd      Temp comp functionality can use both first and second 
                   sweep linearizers for compensation
02/06/12   hdz     Added rf_cdma_update_tx_agc() for reloading tx agc in calls 
12/20/11   aro     Added inteface to query the TxLUT index for given PA state
                   and FW Table index
11/07/11   hdz     Added rf_cdma_tx_temp_comp()
11/07/11   hdz     Included "rfc_cdma.h"
11/03/11   cd      Provide interface to return max PA state for a given band 
10/18/11   cd      Removed old linearizer functionality
08/16/11   cd      Functionality to compute FW linearizer tables from new 
                   multi-linearizer internal data structures 
07/20/11   sty     Changed arg-list for rf_cdma_load_tx_linearizers
07/07/11   sty     Changed return type for rf_cdma_load_tx_linearizers
05/10/11   cd      Remove old NV table pointer usage
03/25/11   sty     Changes to Header label
03/24/11   sty     Initial Release

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_cdma_band_types.h"
#include "rf_cdma_mdsp_types.h"
#include "rfc_cdma.h"
#include "rfm_gain_state_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_tx_agc_load_linearizer
(
  rfm_mode_enum_type const mode,
  rfm_device_enum_type const device,
  rfm_cdma_band_class_type const rf_band,
  uint16 rf_channel,
  tx_linearizer_waveform_type waveform_type,
  rf_cdma_mdsp_pa_stm_config_type *pa_stm_data,
  lm_handle_type txlm_handle
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_tx_agc_pa_state_is_valid
(
  rfm_cdma_band_class_type const band,
  rfm_cdma_chan_type const channel,
  tx_linearizer_waveform_type tx_waveform,
  const tx_band_cal_type *tx_band_cal_data,
  rfm_pa_gain_state_type pa_state
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_tx_temp_comp
(
  tx_linearizer_table_type *tx_lin_data_fb_swp,
  tx_linearizer_table_data_type *tx_lin_data_apt_swp,
  tx_linearizer_table_data_type *tx_lin_data_after,
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  uint16 channel,
  rfm_pa_gain_state_type pa_state,
  tx_linearizer_waveform_type waveform_type ,
  boolean skip_pout_comp
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_update_tx_agc
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_tx_agc_config
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  lm_handle_type txlm_handle
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_tx_agc_get_device_info
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfcommon_core_txlin_agc_device_info *device_info
);

/*----------------------------------------------------------------------------*/
int16 
rf_cdma_tx_agc_get_pin_comp_val
(
  rfm_device_enum_type device,  
  uint8 pa_state
);

/*----------------------------------------------------------------------------*/
int16 
rf_cdma_tx_agc_get_pout_comp_val
(
  rfm_device_enum_type device,
  uint8 pa_state,
  uint16 fw_rgi
);

/*----------------------------------------------------------------------------*/
uint16
rf_cdma_tx_agc_get_dpd_index
(
  rfm_device_enum_type device,
  rfm_pa_gain_state_type pa_state,
  uint16 fw_rgi
);

/*----------------------------------------------------------------------------*/
uint16 
rf_cdma_txagc_get_pred_clip_thresh
(
  rfm_cdma_band_class_type const rf_band,
  rf_cdma_mdsp_pa_stm_config_type* pa_stm_data,
  rf_cdma_mdsp_xpt_config_type* xpt_config_params
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_tx_agc_autopin_update
(
  rfm_device_enum_type device,
  int16 pin_comp_val,
  uint8 pa_index
);

#ifdef __cplusplus
}
#endif

#endif  /* RF_CDMA_TXAGC_H */

