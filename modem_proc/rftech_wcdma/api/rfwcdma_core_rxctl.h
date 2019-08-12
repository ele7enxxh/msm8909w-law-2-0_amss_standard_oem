#ifndef RFWCDMA_CORE_RXCTL_H
#define RFWCDMA_CORE_RXCTL_H
/*!
  @file
  rfwcdma_core_rxctl.h

  @brief
  Provides WCDMA Temperature Compensation functionality

  @details


*/

/*===========================================================================

Copyright (c) 2008-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_core_rxctl.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/14   rmb     Added Dynamic spur mitigation feature related changes.
09/26/14   kai     Added support for WCDMA Rx V2 NVs
08/21/14   rmb     Add alternate support in the WCDMA MSM layer.
08/11/14   rmb     Add alternate path support for W IRAT scenario. 
06/23/14   zhh     implement ASM sleep during retune or any ifreq/irat 
06/06/14   dw      Added rfwcdma_core_rxctl_update_alt_path
06/05/14   dw      Added rfwcdma_core_rxctl_disable_sleep_asm()
06/06/14   ac      NBR support
04/18/14   rmb     Add IRAT meas enum info to config_band and config_chan APIs.
03/13/14   rmb     Pass the IRAT flag to config chan and band API.
02/27/14   rmb     Add fix to tune to channel for multi-carrier scenario.
02/13/14   dw      [1]Change AGC command function to support multiple device
                   [2]Code cleanup
01/02/14   dw      Bolt cleanup on legacy mdsp function calls
09/10/13   dw      Remove DBDC featurization and code clean up. 
09/09/13   dw      Core changes for new Bolt MDSP module - new API to config Rx band/chan
08/23/13   vs      Added support for 3 carrier
08/16/13   ndb     Clean-up the SPI based antenna tuner support
07/26/13   ac      compiler warning fix for dime 2.0
07/19/13   ac      DBDC bringup related and mdsp cleanup change
07/08/13   sma     Modify support for rsb comp with temp change
05/03/13   tks     Added support for rsb comp with temp change
02/06/13   rmb     Added support for wakeup optimization.
12/11/12   dw      Add support for DTx and DRx/LPM.
05/08/12   tks     Add support for CPC Drx  
11/04/11   gh      Add ant_tuner state check
10/17/11   gh      Add support for ant_tuner
10/13/11   swb     Added API for freezing Rx AGC accumulator 
06/23/11   dw      Add rfwcdma_core_rxctl_enable_dual_carrier
06/13/11   dw      Remove hard-coded zero db digital gain
02/22/11   dw      Include RxLM buffer index in AGC cmd
02/09/11   dw      Add support for WCDMA AGC cmd control
09/10/10   dw      Fix wrong Z_ZERO_DB_DIGITAL_GAIN for Genesis
02/11/10   ckl     Changed Zero dB digital gain.
03/16/10   kguo    Merged from scmm_final_merge branch 
01/10/10   kguo    Merged mdm9k dev branch to main\latest 
12/02/09   ckl     Removed NV related warning.
11/18/09   ac      CGAGC parameter update
08/13/09   jhe     freq comp change
07/07/09   ckl     Fixed Z_ZERO_DB_DIGITAL_GAIN value.
03/15/09   ckl     Deleted rfnv.h
11/06/08   av      Initial version.

============================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "rfcom.h"
#include "rfnv_wcdma.h"
#include "rfwcdma_mdsp_types.h"
#include "rfcommon_concurrency_manager.h"
#include "rfdevice_wcdma_type_defs.h"
#include "rfwcdma_msm.h"

#if 0
typedef enum
{
  RF_UARFCN_UL,
  RF_UARFCN_DL
} rfwcdma_core_rxctl_uarfcn_ul_dl_type;

typedef struct
{
  uint16    rxAgcVal_CmInit_0;
  uint16    rxAgcVal_CmInit_1;
  uint16    rxAgcVal_CmInit_2;
  uint16    rxAgcVal_CmInit_3;
  uint16    rxAgcVal_CmInit_4;
  uint16    rxAgcVal_CmInit_5;
  uint32    Z_cmLnaFall_1;
  uint32    Z_cmLnaFall_2;
  uint32    Z_cmLnaRise_1;
  uint32    Z_cmLnaRise_2;
  uint32    Z_cmLnaRise_3;
} rfwcdma_core_rxctl_cgagc_param_type;

typedef struct
{
  int16    rxagcmin;
  int16    rxagcmin2;
  int16    rxagcmin3;
  int16    rxagcmin4;
  int16    rxagcmax;
} rfwcdma_core_rxctl_agc_limit_params_type;

typedef enum
{
  RFWCDMA_CORE_AGC_CAR_0,
  RFWCDMA_CORE_AGC_CAR_1,
  RFWCDMA_CORE_AGC_CAR_BOTH,
  RFWCDMA_CORE_AGC_CAR_INVALID
} rfwcdma_core_rxctl_agc_car_type;

typedef enum
{
  RFWCDMA_CORE_AGC_CHAIN_0,
  RFWCDMA_CORE_AGC_CHAIN_1,
  RFWCDMA_CORE_AGC_CHAIN_BOTH,
  RFWCDMA_CORE_AGC_CHAIN_INVALID
} rfwcdma_core_rxctl_agc_chain_type;

typedef enum {
  RFWCDMA_CORE_ANT_TUNER_STATE_DISABLED,
  RFWCDMA_CORE_ANT_TUNER_STATE_ENABLED,
  RFWCDMA_CORE_ANT_TUNER_STATE_INVALID
}rfwcdma_core_rxctl_ant_tuner_state_type;
#endif
/*--------------------------------------------------------------------------*/
typedef enum {
  RFWCDMA_CORE_ASM_TUNER_DIS_ALL_BANDS = 0,
  RFWCDMA_CORE_ASM_TUNER_DIS_IND_BAND,
  RFWCDMA_CORE_ASM_TUNER_NO_DISABLE_OPS
}rfwcdma_core_rxctl_dis_first_ind_type;

/*--------------------------------------------------------------------------*/
void rfwcdma_core_rxctl_enable_rxagc
(
  uint32 multi_carrier_idx,
  rfwcdma_mdsp_agc_action_type cmd_action[RFM_DEVICE_4],
  uint32 rxlm_buf_idx[RFM_DEVICE_4],
  boolean rf_warmup_en,
  boolean nbr_en_flag,
  uint8 nbr_rxlm_buf_idx
);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_rxctl_set_rx_freeze_accum
(
  rfcom_device_enum_type device,
  boolean freeze 
);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_rxctl_temp_val_update
(
  int16 vga_gain_offset_vs_temp_val,
  int16 c1_vga_gain_offset_vs_temp_val
);
/*--------------------------------------------------------------------------*/
void rfwcdma_core_rxctl_get_ant_mask
(
  rfm_device_enum_type rfm_device, 
  uint32* ant_mask
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_rxctl_update_rsb_coeff(uint8 scaled_therm); 

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_rxctl_config_band
(
  uint32 rxlm_handle,
  rfm_device_enum_type rfm_device,
  rfcom_wcdma_band_type band,
  uint32 multi_carrier_idx,
  rfwcdma_mdsp_rx_irat_meas_type meas_scenario
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_rxctl_config_chan
(
  uint32 rxlm_handle,
  rfm_device_enum_type rfm_device,
  rfcom_wcdma_band_type band,
  uint16 channel[RFCOM_MAX_CARRIERS],
  uint32 multi_carrier_idx,
  rfwcdma_mdsp_rx_irat_meas_type meas_scenario
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_rxctl_disable_sleep_asm
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rf_buffer_intf *script,
  rf_device_execution_type execution_type,
  int16 script_timing,
  rfwcdma_core_rxctl_dis_first_ind_type disable_ind
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_rxctl_update_alt_path
(  
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  boolean is_irat_neighbour,
  uint32 *alt_path
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_rxctl_enable_asm
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rf_buffer_intf *script,
  rf_device_execution_type execution_type,
  int16 script_timing
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_rxctl_enable_tuner
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  uint16 chan_num,
  rf_device_execution_type execution_type,
  rf_buffer_intf *script
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_rxctl_disable_tuner
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rf_device_execution_type execution_type,
  rf_buffer_intf *script,
  int16 script_timing,
  rfwcdma_core_rxctl_dis_first_ind_type disable_ind
);
/*----------------------------------------------------------------------------*/
void rfwcdma_core_rxctl_update_dynamic_notch_cb
(
 rfdevice_wcdma_spur_param_type* dyn_notch_filter_settings
);
/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_rxctl_send_dynamic_notch_rxlm_cmd
(
 rfm_device_enum_type device,
 uint32 rxlm_handle,
 const rfwcdma_msm_dyn_notch_cmd_param_type* notch_params
);
/*--------------------------------------------------------------------------*/
rfdevice_wcdma_spur_param_type* rfwcdma_core_rxctl_get_dynamic_notch_cb_data
(
 rfm_device_enum_type device
);
/*--------------------------------------------------------------------------*/
void rfwcdma_core_rxctl_reset_dynamic_notch_cb_data();
/*--------------------------------------------------------------------------*/
#endif /* RFWCDMA_CORE_TEMP_COMP_H */
