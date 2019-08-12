#ifndef RF_TDSCDMA_MSM_H
#define RF_TDSCDMA_MSM_H
/*!
  @file
  rf_tdscdma_msm.h

  @brief
  Provides TDSCDMA MSM interface functions.

  @details
  The function interface is defined in this file. This encompases functions
  for all the MSMs.

*/

/*=============================================================================

Copyright (c) 2011 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/hal/tdscdma/inc/rf_tdscdma_msm.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/17/15   whz     Create API to push AutoPin config to DM
08/05/13   dej     Moved to Bolt DTR\RxLM types
07/22/13   kb      [XPT] Added API to return sampling rate.
05/28/13   ra      Added multiple API 
02/14/13   jyu     Added API to check if RX_ON/RF_ON is controlled by SSBI 
12/30/12   jyu     Added support for run-time select RX_ON control by SSBI or GRFC 
10/11/12   dej     Rename LM typedef ag header files 
08/20/12   jyu     Added support to set TX_ON 
08/08/12   jyu     Integrated with rfdevice_pa
07/31/12   qzh     Rfdevice interface change to use rfm_device instead of path
07/28/12   jyu     Fixed errors during merge 
07/10/12   kb/jyu  Remove MODEM specific setting from RF TDSCDMA MC/MEAS
06/05/12   jyu     Fixed compliation error for RxLM/TxLM
04/07/12   ycl     Added API to set DAC0_UPDATE GRFC for FTM.
08/23/11   jyu     Added API to allow to initialize TDS stmr in FTM mode 
08/09/11   jyu     Added support to control GRFCs before FW is ready 
07/08/11   jhe     Initial version.

============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rfnv_tdscdma.h"
#include "ftm.h"
#include "rf_tdscdma_msg.h"
#include "rfdevice_cmn_intf.h"
#include "lm_types.h"

//TODO
#define RF_TDSCDMA_ARM_TX_GENERAL_CTL_FTM      0x004
#define RF_TDSCDMA_ARM_TX_GENERAL_CTL_FTM_EUL  0x204
#define RF_TDSCDMA_ARM_TX_GENERAL_CTL_ONLINE   0x284



/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_update_static_rxlm_buffer(rfa_tdscdma_rxlm_mode_t mode,
                                              lm_handle_type rxlm_buf_idx);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_update_dynamic_rxlm_buffer
(
  rfm_device_enum_type device,
                                               uint16 channel,
  lm_handle_type rxlm_buf_idx,
  uint8* wb_adc_idx
);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_update_rxlm_buffer
(
  rfm_device_enum_type device,
                                       uint16 channel,
                                       rfa_tdscdma_rxlm_mode_t mode,
  lm_handle_type rxlm_buf_idx,
  uint8* wb_adc_idx
);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_update_txlm_buffer(lm_handle_type txlm_buf_idx,rfcom_tdscdma_band_type band);

/*--------------------------------------------------------------------------*/
uint16 rf_tdscdma_msm_get_tx_agc_adj( void );

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_enable_tx_apt(rfcom_tdscdma_band_type band);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_init_lna_ctrl(void);

/*--------------------------------------------------------------------------*/
uint8 rf_tdscdma_msm_is_pa_on(void);

/*--------------------------------------------------------------------------*/
uint8 rf_tdscdma_msm_is_tx_on(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_set_pa_range(rfm_device_enum_type device, rfcom_tdscdma_band_type band, uint8 pa_range);

/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_msm_is_tx_agc_in_override(void);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_set_pa_bypass(boolean state);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_set_smps_pa_bias_val(uint16 smps_bias_val);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_set_smps_pa_bias_override(boolean flag);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_set_tx_adj_pdm_delay(int16 delay);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_config_tx_modulator(void);

#if 0
/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_get_cm_delta_rxf(
                              rfcom_tdscdma_band_type band,
                              WfwCmRfRxfDeltaRegisterStruct *rxf_buf 
                            );
#endif

/*--------------------------------------------------------------------------*/
uint32 *rf_tdscdma_msm_iq_capture (ftm_config_iq_log_type *msg_ptr);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_set_cw(boolean on_off,int32 freq_offset);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_set_rx_grfcs(rfcom_tdscdma_band_type band);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_set_tx_grfcs(rfcom_tdscdma_band_type band);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_set_tx_on(rfm_device_enum_type device, rfcom_tdscdma_band_type band, boolean tx_on);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_set_pa_on(rfm_device_enum_type device, rfcom_tdscdma_band_type band, boolean pa_on);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_set_rx_on(rfm_device_enum_type device, rfcom_tdscdma_band_type band, boolean rx_on);

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_set_dac_update( void );

/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_stmr_init(void);

/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_msm_compare_is_same_rxlm_mode(rfa_tdscdma_rxlm_mode_t rxlm_mode_1, rfa_tdscdma_rxlm_mode_t rxlm_mode_2);

/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_msm_is_rx_on_ssbi(rfm_device_enum_type  device, rfcom_tdscdma_band_type band);

/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_msm_is_rf_on_ssbi(rfm_device_enum_type  device, rfcom_tdscdma_band_type band);
/*--------------------------------------------------------------------------*/
void rf_tdscdma_msm_clear_iqmc_rxlm( rfm_device_enum_type device,uint16 channel,lm_handle_type rxlm_buf_idx);
/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_msm_update_dynamic_rxlm_buffer_xpt(uint32 rxlm_buf_idx, rfcom_device_enum_type device_fb, rfcom_tdscdma_band_type band);
/*--------------------------------------------------------------------------*/
uint32 rf_tdscdma_msm_get_default_txc_gain(lm_handle_type txlm_handle);
/*--------------------------------------------------------------------------*/
uint32 rf_tdscdma_msm_calc_iqmc_setting( rfm_device_enum_type device,uint16 channel,lm_handle_type rxlm_buf_idx);
/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_msm_set_xpt_state_rxlm(uint32 rxlm_buf_idx);

/*--------------------------------------------------------------------------*/
uint32 rf_tdscdma_msm_get_samp_rate(lm_handle_type rx_lm_buf_index);

/*--------------------------------------------------------------------------*/
boolean rf_tdscdma_msm_autopin_push_capture_params(lm_handle_type txlm_handle,
                                                   rfdevice_rxtx_common_class *cmn_device_ptr,
                                                   uint16 band,
                                                   uint16 chan);

#endif /*RF_TDSCDMA_MSM_H*/
