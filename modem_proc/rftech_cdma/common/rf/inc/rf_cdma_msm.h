#ifndef RF_CDMA_MSM_H
#define RF_CDMA_MSM_H

/*!
  @file
  rf_cdma_msm.h
 
  @brief
  RF Driver's Common CDMA Modem HAL.

  @details
  This file includes definitions and prototypes used by the CDMA MOdem HAL.
 
*/

/*==============================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_msm.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/17/15   vr      Added dynamic txr_iq_dac setting update support
12/15/14   vc      Added tx_spectral_inversion to rf_cdma_msm_dynamic_txlm_cfg_type
10/07/14   vr      ftm iq capture changes
07/24/14   zhw     Fix wrong retune type in get WB reading API
06/09/14   spa     Notch filter support for bolt
05/05/14   fh      Added device_id to rf_cdma_msm_configure_txlm inf
04/21/14   JJ      Moved get current rtc time api to ftm_cdma_msm.c
04/09/14   JJ      Moved get current rtc time api from ftm_1x_msm.c
03/26/14   spa     Added APIs to Query TxLM and RxLM for TxFE and RxFE delays
02/25/14   fh      Dimepm get DAC sampling rate update
03/03/14   JJ      Added APIs to query ACQ_DURATION and combiner interrupt 
                   update duration 
01/15/13   spa     Added API to freeze/unfreeze WBDC loop
12/12/13   spa     Add API to query for sigma delta max level
12/09/13   JJ      Added API to query dac_rate 
10/22/13   spa     Added API to query for DVGA gain offsets
10/08/13   spa     Added API to query for NB indices
07/19/13   vr      Changes to incorporate device dependennt ICIFIR filter settings
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
04/10/13   spa     Updated notch filter settings structure
04/10/13   spa     Updated rf_cdma_msm_conv_recos_to_reg_vals API
03/25/13   Saul    CDMA. Abstract TxLM XPT mode used in MC in HAL layer.
02/22/13   id      Support for new MLine computation using RMS dBFs
02/15/13   APU     Removed get RXLM and TXLM APIs.         
12/18/12   sty     Added band info in arg list for rf_cdma_msm_configure_txlm
12/06/12   APU     Changed the parameter name for clarity. 
12/06/12   APU     Replaced LM chains with Modem chains. 
10/02/12   spa     Added Tx modem chain info to dynamic TxLM config
08/30/12   bmg     Added sample logging
08/21/12   APU     Added support to get modem_chain from rfc_device_info
08/03/12   Saul    CDMA. RX/TXLM sample capture support.
07/12/12   cri     Added EPT TxLM selection
06/15/12   adk     Corrected BB spur mitigation for WTR1605
06/13/12   aro     Updated RxLM interface to indicate, if dynamic config is to 
                   be applied or not + if ET configuration is to be applied
06/06/12   as      Added BBRX fuse based compensation
03/23/12   adk     Updated rf_cdma_msm_configure_txlm() to write DAC LUT to TxLM
03/19/12   aro     Added interface to query Rx Notch update mask from HAL
03/19/12   aro     Added interface to query TXC gain update mask from HAL
03/14/12   vrb     Configure RxLM API requires band of operation
03/05/12   adk     Undid param update made in rf_cdma_msm_configure_txlm()
02/27/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan 
02/23/12   sty     Renamed API to rf_cdma_msm_get_params_for_nv_notches()                     
02/22/12   adk     Updated rf_cdma_msm_configure_txlm() to write DAC LUT to TxLM
02/17/12   sty     Made notch_filt_settings into an array  
                   Arg changes to rf_cdma_msm_conv_recos_to_reg_vals()
02/06/12   sty     Added prototype for rf_cdma_msm_get_notch_params()
01/20/12   sty     Added prototype for rf_cdma_msm_update_xo_params()
01/19/12   Saul    MCDO. Configure TxLM in multi-carrier mode.
01/19/12   Saul    MCDO. Added ICIFIR dynamic configuration.
01/19/12   aro     Updated all HAL interfaces to use RxLM/TxLm handle rather
                   than logical device
01/04/12   aro     Added interface to perform conversion of LNA phase offset
                   from degrees to modem specific HW units
01/03/12   aro     Added static configuration structure as argument to
                   configure TxLM interface
01/03/12   aro     Created TxLM static configuration structure
01/03/12   aro     Interface to query TxLM chain for the given TxLM buffer
12/29/11   aro     Interface to query RxLM chain for thr given RxLM buffer
12/28/11   shb     Converted IQMC coefficients to int32 from uint32
12/21/11   sty     var name change
12/20/11   sty     Added prototype - rf_cdma_msm_update_notch_settings
12/20/11   sty     Added prototype for rf_cdma_msm_update_notch_flt_settings()
12/19/11   sty     Added new API - rf_cdma_msm_init_dyn_settings()
12/10/11   sty     Deleted flag from rf_cdma_msm_notch_filter_settings_type 
12/10/11   sty     Added place holder for notch filter settings in dynamic 
                   settings struct - rf_cdma_msm_dynamic_rxlm_cfg_type
12/02/11   cd      Functionality to retrieve default TxLM IQ gain 
09/27/11   aro     Updated TxLM configure function to pass dynamic configuration
                   structure
09/27/11   aro     Deleted unused TxLM activate function
09/27/11   aro     Added test function to set TXC DP IQ Gain
09/08/11   vb/aro  TxLM Dyamic updates for DAC Cal
06/14/11   Saul    Now configuring CSR freq offsets.
06/10/11   Saul    Now configuring carriers based on mask.
06/09/11   Saul    RxLM updates for multi-carrier driver.
05/21/11   aro     Merged Rx Spectral inversion into configure Rxlm function
05/20/11   aro     Fixed Rx Spectral Inversion Bug
05/18/11   aro     Added interface to configure spectral inversion
05/13/11   sty     Removed online_flag from rf_cdma_msm_configure_txlm
04/25/11   sty     Added cal_state as arg in rf_cdma_msm_configure_txlm
04/22/11   aro     Removed ADC clock manager Init
04/21/11   aro     Added ADC Clock Manager to HAL
04/17/11   sty     Added mode arg to rf_cdma_msm_configure_rxlm()
04/12/11   aro     Deleted activate and de-activate RXLM functions
03/31/11   aro     Moved IQ Capture and RxF Debug interfaces to FTM HAL
02/16/11   aro     Added support to enable RxLM
02/15/11   aro     Changed the Get Cal Comp interface to take LM buffer
02/15/11   aro     Moved set DVGA and LNA offset interface to MDSP
02/14/11   sty     Deleted rf_1x_enable_modulator
02/07/11   aro     Added TxLM Configure/Activate function
02/07/11   aro     Added RxLM Activate function
02/07/11   aro     Added RXLM Configuration Function
02/03/11   sty     Added rf_1x_enable_modulator
02/03/11   aro     Added interface to apply Compensation value
02/01/11   sty     Renamed rf_1x_enable_modulator to 
                   rf_1x_enable_modulator_and_tx and changed its return type
01/31/11   aro     Added Set LNA Offset and DVGA Offset Interfaces
01/31/11   bmg     Added rf_1x_enable_modulator()
01/10/11   aro     Removed extern rfm_mode[]
01/07/11   aro     Added RXF Debug function
12/20/10   aro     Initial version

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfm_device_types.h"
#include "rfm_cdma_carrier_types.h"
#include "rfm_cdma_power_mode_types.h"
#include "rfm_cdma_band_types.h"
#include "rf_cdma_state_types.h"
#include "rfm_mode_types.h"
#include "rf_cdma_lna.h"
#include "sys.h"
#include "txlm_intf.h"
#include "rxlm_intf.h"
#include "rfdevice_cmn_intf.h"
#include "rfdevice_cdma_interface.h"
#include "rf_cdma_wb_rxf.h"
#include "rfcommon_core_xpt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure containing the RXLM static settings
*/
typedef struct
{ 
  uint32 adc_clock_freq;
  /*!< ADC clock frequency on which Rx front end is to be operated on */

  rxlm_adc_type rxlm_adc;
  /*!< ADC block used for RxLM Static programming */

  uint32 bw_khz;
  /*!< Bandwidth to use. */

  boolean config_as_et;
  /*!< Flag indicating, if the static configuration is to be done as ET  */

} rf_cdma_msm_static_rxlm_cfg_type;

typedef struct
{
  boolean needs_update;

  uint32 rxfe_wbw_notch_filt01_enable_cfg;

  uint32 rxfe_wbw_notch_filt23_enable_cfg;

  uint32 rxfe_wbw_notch_filtn_cfg0[ RF_MAX_NUM_NOTCH_FILTERS_AVAIL];

  uint32 rxfe_wbw_notch_filtn_cfg1[ RF_MAX_NUM_NOTCH_FILTERS_AVAIL];

  uint8  num_notch_filters_used;

} rf_cdma_notch_flt_settings_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure containing the RXLM dynamic settings
*/
typedef struct
{
  boolean apply_dynamic_cfg;
  /*!< Flag indicating, if the dynamic configuration needs to be applied 
  or not */

  int32 iqmc_coeff_in;
  /*!< Signed in-phase coefficient for IQ mismatch compensation. This
  is 32Q15 format */

  int32 iqmc_coeff_quad;
  /*!< Signed quadrature-phase coefficient for IQ mismatch compensation. 
  This is 32Q15 format */

  rfm_cdma_carrier_en_mask_type carrier_mask;
  /*!< Represents which carriers to enable/disable */

  int32 freq_offset_hz[RFM_CDMA_CARRIER_NUM];
  /*!< Contains frequency offset (relative to tuned chan) in Hertz */

  /*! The number of active carriers.*/
  uint32 num_carriers;

  rf_cdma_notch_flt_settings_type notch_filt_settings[RFM_CDMA_CARRIER_NUM];
  /*!< Contains register settings to configure notches */

  uint32 modem_chain ;
  /*!< The modem chain for given device and band. */

} rf_cdma_msm_dynamic_rxlm_cfg_type;

/*----------------------------------------------------------------------------*/
/*! Control word for Rx and Tx block */
typedef uint32 rf_cdma_msm_ctrl_word_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure containing the TXLM static settings
*/
typedef struct
{ 
  txlm_dac_type txlm_dac;
  /*!< DAC block used for TxLM Static programming */

  xpt_cfg_type xpt;
  /*!< current xpt option being used (APT, EPT, ET) */

} rf_cdma_msm_static_txlm_cfg_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure containing the TXLM dynamic settings
*/
typedef struct
{
  rfdevice_dac_iref_val iref_val;
  /*!< DAV iREF value to be used */

  uint32 num_carriers;
  /*!< The number of active carriers.*/

  uint32 tx_modem_chain ;
  /*!< The modem chain for given device and band. */

  boolean tx_spectral_inversion;
  /*!< Flag to enable Tx spectral inversion */
} rf_cdma_msm_dynamic_txlm_cfg_type;

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_msm_configure_rxlm
(
  rfdevice_id_enum_type device_id,
  lm_handle_type rxlm_handle,
  rfm_mode_enum_type curr_mode,
  rfm_cdma_band_class_type band,
  rfm_cdma_power_mode_type power_mode,
  rf_cdma_msm_static_rxlm_cfg_type static_cfg,
  rf_cdma_msm_dynamic_rxlm_cfg_type dynamic_cfg
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_msm_configure_txlm
(
  rfdevice_id_enum_type device_id,
  lm_handle_type txlm_handle,
  rfm_mode_enum_type rf_mode,
  const rfm_device_enum_type device,
  const rfm_cdma_band_class_type band,
  rf_cdma_msm_static_txlm_cfg_type static_cfg,
  rf_cdma_msm_dynamic_txlm_cfg_type dynamic_cfg
);

/*----------------------------------------------------------------------------*/
xpt_cfg_type 
rf_cdma_msm_get_txlm_xpt_mode
( 
   boolean is_master_lm, 
   xpt_cfg_type xpt_cfg 
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_msm_set_txc_gain
(
  uint32 modem_chain,
  lm_handle_type txlm_handle,
  uint32 txc_gain
);

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_msm_get_default_txc_gain
(
  txlm_chain_type txlm_chain,
  lm_handle_type txlm_handle
);

/*----------------------------------------------------------------------------*/
rf_cdma_msm_ctrl_word_type
rf_cdma_msm_get_ctrl_word_txc_gain
(
  uint32 modem_chain
);

/*----------------------------------------------------------------------------*/
rf_cdma_msm_ctrl_word_type
rf_cdma_msm_get_ctrl_word_rx_notch
(
  lm_handle_type rxlm_handle
);

/*----------------------------------------------------------------------------*/
rf_cdma_msm_ctrl_word_type
rf_cdma_msm_get_ctrl_word_txr_iq_dac
(
  lm_handle_type txlm_handle
);

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_msm_get_adc_samp_freq
(
  lm_handle_type rxlm_handle,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_msm_get_cal_compensation
(
  lm_handle_type rxlm_handle
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_msm_init_dyn_settings
(
  rf_cdma_msm_dynamic_rxlm_cfg_type *const dyn_settings
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_msm_conv_recos_to_reg_vals
(
  lm_handle_type rxlm_handle,
  rfm_cdma_band_class_type curr_band,
  uint16 curr_chan,
  uint16 lo_tuned_to_chan,
  uint16 num_carriers,
  rfdevice_per_carrier_notch_reco_type const notch_settings,
  rf_cdma_notch_flt_settings_type *const notch_register_settings_ptr
);

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_msm_convert_lna_phase
(
  uint16 phase_offset_degree
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_msm_update_notch_settings
(
  lm_handle_type rxlm_handle,
  rf_cdma_notch_flt_settings_type *const new_notch_settings
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_msm_update_xo_params
(
  rfm_mode_enum_type rf_mode,
  lm_handle_type rxlm_handle,
  int32 freq_offset_hz[]
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_msm_get_params_for_nv_notches
(
  rxf_notch_filter_config_type* notch_flt_params
);

/*----------------------------------------------------------------------------*/
int32
rf_cdma_msm_get_bbrx_msp_iq_mismatch_gain
(
  lm_handle_type rxlm_handle,
  uint32 bw_khz
);

/*----------------------------------------------------------------------------*/
int32
rf_cdma_msm_get_rms_dbfs_x100_val
(
  void
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  A structure for storing CDMA Rx samples.

  @details
  Rx samples contain two values, one each for the I and Q channels of the
  receiver.

  Four bits of precision are provided for each component of the sample in
  the sample capture.  The value is stored as a 4-bit offset 2's complement
  integer.  The mapping for 4-bit offset 2's complement is:
  Raw value   Offset 2's complement value
        8   =  -7.5
        9   =  -6.5
        10  =  -5.5
        11  =  -4.5
        12  =  -3.5
        13  =  -2.5
        14  =  -1.5
        15  =  -0.5
        0   =   0.5
        1   =   1.5
        2   =   2.5
        3   =   3.5
        4   =   4.5
        5   =   5.5
        6   =   6.5
        7   =   7.5

  I and Q components are stored as bit-fields to pack one I+Q sample into
  a single byte.
*/
typedef struct rf_cdma_msm_sample_s
{
  /*!
    @brief
    Value of the in-phase component of the signal at this sample point.
  */
  signed char i;

  /*!
    @brief
    Value of the quadrature-phase component of the signal at this sample
    point.
  */
  signed char q;
} rf_cdma_msm_sample_type;

/*----------------------------------------------------------------------------*/
boolean
rf_1x_msm_get_last_n_samples
(
  rfm_device_enum_type device,
  unsigned short num_iq_samples,
  rf_cdma_msm_sample_type* buffer
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_msm_get_last_n_samples
(
  rfm_device_enum_type device,
  rfm_cdma_carrier_type carrier,
  unsigned short num_iq_samples,
  rf_cdma_msm_sample_type* buffer
);

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_msm_get_samp_rate 
(
   uint8 rx_lm_buf_index
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_msm_get_wb_nb_indices
( 
  lm_handle_type rxlm_handle,
  uint8 *wb_idx,
  uint8 *nb_indices 
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_msm_get_dvga_gain_offsets
( 
  lm_handle_type rxlm_handle,
  int32 *dvga_gain_offsets 
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_msm_get_dac_rate
(
  lm_handle_type txlm_handle,
  uint32 *dac_rate_hz
);

/*----------------------------------------------------------------------------*/
uint8
rf_cdma_msm_get_dac_bit_length
( 
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_msm_get_dac_sampling_rate
( 
  lm_handle_type txlm_handle,
  uint32 *dac_sampling_rate
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_msm_get_sigma_delta_max_level
( 
  void
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_msm_wbdc_accum_freeze_unfreeze
( 
  uint8 wb_idx,
  boolean start_imm 
);

/*----------------------------------------------------------------------------*/
void 
rf_cdma_msm_config_wb_power_estimator
(
  uint32 wb_idx,
  boolean always_armed,
  boolean start,
  boolean immediate,
  uint32 start_action_sample,
  uint32 stop_action_sample
);

/*----------------------------------------------------------------------------*/
uint32 
rf_cdma_msm_get_wb_accum_reading
(
  uint32 wb_idx
);

/*----------------------------------------------------------------------------*/
void 
rf_cdma_msm_clear_wb_accum
(
  uint32 wb_idx
);

/*----------------------------------------------------------------------------*/
void 
rf_cdma_msm_config_wb_pwr_estimate_mask
(
  uint32 wb_idx
);

/*----------------------------------------------------------------------------*/
uint16
rf_cdma_msm_get_combiner_interrupt_period
(
  void
);

/*----------------------------------------------------------------------------*/
uint16
rf_cdma_msm_get_1x_default_agc_acq_duration
(
  void
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_msm_get_txfe_delay
(
  lm_handle_type txlm_handle,
  int32 *txfe_delay_cx8
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_msm_get_rxfe_delay
(
  lm_handle_type rxlm_handle,
  int32 *rxfe_delay
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_msm_get_default_mcdo_scdo_gains
(
  txlm_chain_type txlm_chain ,
  lm_handle_type  txlm_handle ,
  int16*         scdo_iq_gain ,
  int16*         mcdo_iq_gain ,
  int16*            diff_dB10
) ;

/*----------------------------------------------------------------------------*/

boolean
rf_cdma_msm_update_txr_iq_dac_settings
(

rfm_device_enum_type device, 
rf_cdma_msm_dynamic_txlm_cfg_type dynamic_cfg, 
lm_handle_type txlm_handle);

/*----------------------------------------------------------------------------*/


#endif /* FEATURE_CDMA1X */
#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_MSM_H */

