/*!
  @file
  rflte_msm.h

  @brief
  Provides LTE MSM interface functions.

  @details
  The function interface is defined in this file. This encompases functions
  for all the MSMs.

*/

/*===========================================================================

  Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/hal/lte/inc/rflte_msm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/14   svi     Update WB with respect to Alt Path
11/06/14   as      Added support for extended EARFCN for LTE
10/02/14   kab     API parameter change to support Single Rx
08/20/14   gvn     MSM Alternate path change
06/19/14   bar     Updated RFLTE_MSM_NUM_NOTCH_PER_CHAIN for DPM
06/13/14   svi     Notch Application based on Tx AFC - Stubbed Now for DPM
06/11/14   bar     Added rflte_msm_set_rxagc_disable() to support DPM IQ capture
06/10/14   php     Add API to set the DAC setpoint
02/04/14   npi     Support for applying dynamic notches and hooks for updating 
                   notch thresholds
01/28/14   npi     Support for configuring/apply/reset LTE dedicated notches
08/12/13   bsh     FTM LTE Fixed Vector Tx
07/25/13   kab     Common Notch Filter Implementation
07/24/13   gvn     RSB Cal updates on online mode, iqmc update and IRAT
07/19/13   php     Merge TxLO API parameter to fix compile error
06/26/13   gvn     Write RSB Cal params to NV and other RSB cal updates
06/26/13   kab     Dedicated Notch Filter Feature implementation
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
04/18/13   vss     Added support for Dynamic SCELL switch
07/05/13   cri     Update sync opcode calculation
06/26/13   gvn     Write RSB Cal params to NV and other RSB cal updates
06/26/13   kab     Dedicated Notch Filter Feature implementation
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
04/18/13   vss     Added support for Dynamic SCELL switch
07/03/13   gvn     Merging iqmc code from Dime to resolve compiler error
03/27/13   vss     Add support for TxLO cal
03/12/13   sn      Added rflte_msm_rxfe_adc_wb_index_dynamic_update_xpt
03/06/13   php     Remove FTM mode field for RxFE settings
02/22/13   id      Support for new MLine computation using RMS dBFs
02/08/13   cri     Program the wmore clipper in tx override
01/23/12   php     Add FTM mode field for Rxfe static settings
01/03/13   cri     Updates to support new TxAGC override interface 
11/26/12   cri     Added Tx Override and Sync opcodes
11/20/12   php     Add FTM check for NBEE config
11/12/12   php     Add new APIs for supporting NBEE
10/15/12   pl      Update IRAT MSM interface 
10/09/12   cri     LTE DPD IQ capture MSGR support and bug fixes
09/05/12   cri     Add ET support for LM selection through DPD Config NV
06/14/12   php     Added support for picking the right RxLM state
06/07/12   pl      Update MSM interface for CA support
06/07/12   pl      Remove all rfdevice calls from MSM to MC
06/07/12   as      Added BBRX fuse based compensation
02/16/12   pl      Remove offline DRX processing from RFSW. 
01/17/12   pl      Support Notch Filter
10/12/11   pl      Support Offline DRX
10/04/11   whc     Remove deprecated SMPS Bias code       
09/08/11   pl/vb   Support for DAC Cal and TxLM dynamic updates
06/23/11   pl      remove obsolete fucntions
06/23/11   pl      remove obsolete table
05/06/11   tnt     Add support for PDM_0 program using for APT_0
05/03/11   pl      Remove Unused Functions
03/15/11   pl      Remove unused functions
03/08/11   whc     Parked Mode SMPS support
03/02/11   pl      Temporarily disable NikeL autogen
02/22/11 whc/can   Initial APT support.
02/14/11   whc     Fix for TDD Crash in PLF 110
02/08/11   pl      Add TxLM support
01/24/11   whc     Use SMPS Overdrive in FTM mode
01/19/11   pl      Add RxLm support
01/19/11   pl      retire legacy RxFE support 
12/21/10   whc     FTM Command for SMPS Override
12/21/10   whc     Basic APT\GST support
11/12/10   jyu     Added support to program RxFE for L2L interfreq 
10/07/10   whc     Added rflte_msm_temp_read_disable
07/07/10   pl      Added function to set fixed BW-dependent RxFE Resampling setting 
05/26/10   qma     Added RX I/Q mismatch correction
05/19/10   can     Added rflte_msm_temp_read_setup to register for Temp Comp ISR.
05/04/10   qma     Added RxFE disable/enable support
04/16/10   tnt     Adding support for run-time band change for PA_ON_SRC
04/06/10   can     Addition of TX related MSM functions.
03/12/10   tnt     Adding code to support GRFC for RF signals
08/03/09   qma     Initial Check-in
===========================================================================*/
#ifndef RFLTE_MSM_H
#define RFLTE_MSM_H

#include "comdef.h"
#include "rfd_grfc.h"
#include "DDITlmm.h"
#include "rfcom.h"
#include "rflte_mc.h"
#include "lm_types.h"
#include "rfdevice_cmn_intf.h"
#include "rfcommon_core_xpt.h"
#include "rfdevice_lte_type_defs.h"
#include "rxlm_intf.h"

#define RF_SIGNAL_MAX   17
#define RF_PA_MAX       5

#define RFLTE_MSM_PA_SMPS_MAX_HW_LIMIT_PDM_VAL 1023
#define RFLTE_MSM_PA_SMPS_PARKED_MODE_PDM_VAL  780

#define RFLTE_MSM_PA_SMPS_BYPASS_PDM_VAL       1023
#define RFLTE_MSM_NUM_NOTCH_PER_CHAIN 3

typedef struct
{ 
  DALGpioSignalType tlmm_signal;                            /*!< signal connects to TLMM */
  rfd_rt_grfc_out_type grfc_out;                            /*!< grfc_out of the RFD RT  */
  rfd_rt_input_grfc_engine_type grfc_engine;                /*!< grfc_engine             */
  rfd_rf_control_signal_type    rf_signal;                  /*!< rf control signal       */
  rfd_internal_grfc_engine_rf_enum_type internal_engine;    /*!< rf internal engine      */
} rflte_msm_grfc_mapping_type;

typedef struct
{
  uint32  dl_freq;
  int32   rsb_a;
  int32   rsb_b;
} rflte_msm_rxfe_misc_data_struct;

typedef struct
{
  rfdevice_dac_iref_val iref_val;
  boolean spectral_inversion;
} rflte_msm_tx_misc_data_struct;

#ifndef FEATURE_BOLT_MODEM
/* Making use of below structure for all common notches, 
   since structure for all common structures are similar.
   If this fact doesn't hold true, suitable modifications are needed.*/
typedef struct
{
  uint32 rxfe_cmnnotch_filt_cfg;
  uint32 rxfe_cmnnotch_filtn_cfg0[RFLTE_MSM_NUM_NOTCH_PER_CHAIN];
  uint32 rxfe_cmnnotch_filtn_cfg1[RFLTE_MSM_NUM_NOTCH_PER_CHAIN];
}common_notch_group_struct;


#endif

typedef enum 
{
  RFLTE_MSM_DAC_TO_LTE_WAVEFORM,
  RFLTE_MSM_DAC_TO_CW_TONE
} rflte_msm_dac_type;

/*----------------------------------------------------------------------------*/
void rflte_msm_get_regs (rfcom_device_enum_type device,
                         rfcom_lte_earfcn_type rx_chan, 
                         rfcom_lte_bw_type bw,
                         lm_handle_type rxlm_buffer_idx,
                         rflte_msm_rxfe_misc_data_struct* misc_data_ptr,
                         uint8 alt_path);

/*--------------------------------------------------------------------------*/
void rflte_msm_set_pa_smps_override_enable(boolean enable);

/*--------------------------------------------------------------------------*/
boolean rflte_msm_get_pa_smps_override_enable(void);

/*--------------------------------------------------------------------------*/
void rflte_msm_rxfe_init_static_settings(uint32 rxlm_buf_index, 
                                         rfcom_device_enum_type device, 
                                         rfcom_lte_band_type band, 
                                         rflte_mc_carrier_bw_spacing_s_type bw_info,
                                         xpt_cfg_type xpt_cfg);

/*--------------------------------------------------------------------------*/
void rflte_msm_rxfe_init_dynamic_settings(uint32 rxlm_buf_index, 
                                          rfcom_device_enum_type device, 
                                          rfcom_lte_band_type band, 
                                          rfcom_lte_bw_type bw, 
                                          rflte_msm_rxfe_misc_data_struct* misc_data_ptr,
                                          rfdevice_lte_spur_mitigation_type *spur_mitigation_params,
                                          uint32 carrier_idx,
                                          uint8  alt_path);

/*--------------------------------------------------------------------------*/
void rflte_msm_rxfe_adc_wb_index_dymanic_update( uint32 rxlm_buf_index, 
                                                 rfcom_device_enum_type device,
                                                 rfcom_lte_band_type band,
                                                 rxlm_chain_type rxlm_chain);

/*--------------------------------------------------------------------------*/
void rflte_msm_rxfe_notch_filter_dynamic_update(uint32 carrier_idx, uint32 rxlm_buf_index, 
                                                 rfcom_lte_bw_type bw,
                                                 rfcom_device_enum_type device,
                                                rflte_msm_rxfe_misc_data_struct* misc_data_ptr,
                                                rfdevice_lte_spur_mitigation_type *spur_mitigation_params);	
/*--------------------------------------------------------------------------*/
void rflte_msm_rxfe_cmn_notch_filter_deallocate(rfcom_device_enum_type device, rfcom_lte_band_type band);

/*--------------------------------------------------------------------------*/
void rflte_msm_rxfe_adc_wb_index_dynamic_update_xpt( uint32 rxlm_buf_index, 
                                                 rfcom_device_enum_type device,
                                                 rfcom_lte_band_type band);                    
/*--------------------------------------------------------------------------*/
void rflte_msm_tx_init_static_settings(uint32 txlm_buf_index, 
                                       rfcom_device_enum_type device, 
                                       rfcom_lte_band_type band, 
                                       rfcom_lte_bw_type bw,
                                       xpt_cfg_type xpt_cfg);

/*--------------------------------------------------------------------------*/
void rflte_msm_tx_init_dynamic_settings(uint32 txlm_buf_index, 
                                        rfcom_device_enum_type device, 
                                        rfcom_lte_band_type band, 
                                        rfcom_lte_bw_type bw, 
                                        rflte_msm_tx_misc_data_struct *misc_data_ptr);
/*--------------------------------------------------------------------------*/
int32
rflte_msm_get_bbrx_msp_iq_mismatch_gain
(
  lm_handle_type rxlm_handle,
  rfcom_lte_bw_type bw
);
/*--------------------------------------------------------------------------*/
void rflte_msm_rxfe_nbee_config_update(uint32 rxlm_buf_index, 
                                       rfcom_lte_bw_type bw);
/*--------------------------------------------------------------------------*/
int16 rflte_msm_get_nbee_rxagc_val(rfm_device_enum_type device, 
                                   uint8 lna_range, 
                                   rfcom_lte_band_type rf_band,
                                   rfcom_lte_bw_type bw,
                                   uint32 carrier_idx );

/*--------------------------------------------------------------------------*/
uint32 rflte_msm_get_current_sf_time(uint8 modem_chain, rfcom_lte_bw_type bw);
/*--------------------------------------------------------------------------*/
uint32 rflte_msm_get_sync_offset(uint8 modem_chain, rfcom_lte_bw_type bw);
/*--------------------------------------------------------------------------*/
uint32 rflte_msm_get_max_txc_sample_count(uint8 modem_chain, rfcom_lte_bw_type bw);
/*--------------------------------------------------------------------------*/
void rflte_msm_verify_modem_settings_latched(uint8 modem_chain);

/*--------------------------------------------------------------------------*/
void rflte_msm_tx_disable_dpd(void);

/*--------------------------------------------------------------------------*/
void rflte_msm_tx_enable_dpd(void);

/*--------------------------------------------------------------------------*/
uint32
rflte_msm_get_default_txc_gain
(
  uint32 txlm_buf_index
);

/*--------------------------------------------------------------------------*/
void rflte_msm_set_wmore_clipper( uint32 wmore_clip_en,
                                  uint32 wmore_clip_thr1,
                                  uint32 wmore_clip_thr2 );
/*--------------------------------------------------------------------------*/
void rflte_msm_convert_20pcfl_to_16sc
(
   uint32 *rx_samples, 
   uint16 sample_size, 
   double *rx_samples_i_comp, 
   double *rx_samples_q_comp
);
/*--------------------------------------------------------------------------*/
void rflte_msm_compute_fbrx_dc(double *rx_samples_i_comp, 
                               double *rx_samples_q_comp,
                               uint16 sample_size,
                               double *dc_i_comp,
                               double *dc_q_comp);
/*--------------------------------------------------------------------------*/
void 
rflte_msm_perform_dc_cancellation
(
   double *rx_samples_i_comp, 
   double *rx_samples_q_comp,
   uint16 sample_size, 
   double fbrx_dc_i_comp,
   double fbrx_dc_q_comp
);

/*--------------------------------------------------------------------------*/
void 
rflte_msm_perform_rx_tx_alignment
(
 uint32 *tx_samples,
 uint16 tx_sample_size,
 double *rx_samples_i,
 double *rx_samples_q,
 uint16 rx_sample_size,
 uint16 search_center,
 uint16 search_width,
 double *tx_samples_i,
 double *tx_samples_q,
 double *tx_sum_i,
 double *tx_sum_q,
 double *rx_sum_i,
 double *rx_sum_q,
 double *rx_align_i_samples,
 double *rx_align_q_samples,
 double *dc_estimation_i,
 double *dc_estimation_q
);

/*--------------------------------------------------------------------------*/
double
rflte_msm_array_cross_product
(
 double*  tx_sample_comp,
 uint16 tx_start_index,
 uint16 tx_end_index,
 double*  rx_sample_comp,
 uint16 rx_start_index,
 uint16 rx_end_index
);

boolean 
rflte_msm_configure_dcoc_register_coefficients 
(
  uint32 txc_dp_dcoc_an
);

/*--------------------------------------------------------------------------*/
int32
rflte_msm_get_rms_dbfs_x100_val
(
  void
);

/*--------------------------------------------------------------------------*/
void 
rflte_msm_process_txlo_samples
(
 uint32 *tx_on_tx_samples,
 uint32 *tx_on_rx_samples, 
 uint32 *tx_off_tx_samples,
 uint32 *tx_off_rx_samples,
 uint16 sample_size
);

/*--------------------------------------------------------------------------*/
boolean 
rflte_msm_configure_iqmc_register_coefficients 
(
   uint32 value
);
/*--------------------------------------------------------------------------*/
boolean
rflte_msm_perform_txlo_search 
( 
   rfm_device_enum_type rfm_device_fb, 
   rfcom_lte_band_type band, 
   rfcom_lte_bw_type bw, 
   uint32 carrier_idx,
   uint32 *dcoc_adjust_result,
   uint8 search_resolution
);
/*--------------------------------------------------------------------------*/
uint32
rflte_msm_get_samp_rate 
(
   uint8 rx_lm_buf_index
);
/*--------------------------------------------------------------------------*/
void rflte_msm_update_iqmc_rxlm( rfm_device_enum_type device,
                                 rfcom_lte_earfcn_type channel,
                                 lm_handle_type rxlm_buf_idx,
                                 uint32 carrier_idx,
                                 rfcom_lte_bw_type rx_bw );

void rflte_msm_set_enc_to_fixed_vector(uint8 harq_idx, 
                                       uint32 *fixed_vector_sf_ptr, 
                                       uint32 *vector_length_ptr);
/*--------------------------------------------------------------------------*/
uint16 rflte_msm_get_dac_setpoint(void);


/*--------------------------------------------------------------------------*/
boolean rflte_msm_rxfe_program_notch_update ( uint32 carrier_idx,
                                              rfcom_device_enum_type device,
                                              uint64 dl_lo_rx_hz,
                                              boolean program_settings_to_msm,
                                              uint32 num_notches,
                                              uint32* spur_freq_list,
                                              uint32* spur_thresh,
                                              uint32* notch_depth,
                                              rflte_mc_notch_programmed_config_type notch_cfg );
/*--------------------------------------------------------------------------*/
void rflte_msm_set_rxagc_disable(void);

/*--------------------------------------------------------------------------*/
#endif /*RFLTE_MSM_H*/
