/*!
  @file
  rflm_api_fbrx_fw.h

  @brief
  RF LM API definitions for FBRx for use by FW
  
  @detail

  
  @todo


*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/fw/rflm_api_fbrx_fw.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/10/14    ka     Add API to determine if handle is valid for operation
07/10/14    aa     Move threshold_gain & scale_tx to power dependent params
02/21/14    aa     Added predet index to pwr dep params
02/06/14    ka     Added rx scale to pwr dep params
01/24/14    ka     Restore temporary fields for fw compilation
01/22/14    ka     Updated results type, added delay interface
12/20/13    ka     Add support for LTE TxAGC parameters
12/17/13    ka     Added additional parameters
12/16/13    ka     Temporarily revert version number to fix crash. API updates.
12/13/13    ka     Updated for latest uK APIs
11/14/13    ka     Updated for latest uK APIs
11/12/13    ka     Updated for latest uK APIs
10/30/13    ka     Updated for latest uK APIs
10/02/13   hdz     Added fbrx  api version number
09/05/13    ka     Remove unused header 
08/15/13    ka     Initial version
==============================================================================*/

#ifndef RFLM_API_FBRX_FW_H
#define RFLM_API_FBRX_FW_H

/*! @brief Version of the rflm fbrx module.
*/
#define RFLM_API_FBRX_MAJOR_VERSION            1
#define RFLM_API_FBRX_MINOR_VERSION            3
#define RFLM_API_FBRX_VERSION    \
              ((RFLM_API_FBRX_MAJOR_VERSION << 16) | RFLM_API_FBRX_MINOR_VERSION )


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/
#define RFLM_FBRX_RX_FILTER_TAPS_SIZE 24
#define RFLM_FBRX_TX_FILTER_TAPS_SIZE 5
#define RFLM_FBRX_AMAM_SIZE 4
#define RFLM_FBRX_PCOEFF_SIZE 4
#define RFLM_FBRX_DPD_K_SIZE 3
#define RFLM_FBRX_LTE_MAX_RUNS_PER_SF 3
#define RFLM_FBRX_XCORR_LOG_SIZE 32

typedef enum
{
  RFLM_FBRX_1X0_V1 = 0,
  RFLM_FBRX_1X0_V2 = 1,
  RFLM_FBRX_1X1    = 2,
  RFLM_FBRX_2X2    = 3,
  RFLM_FBRX_3X3    = 4,
} rflm_fbrx_algorithm_t;

typedef enum
{
  RFLM_FBRX_RSB_BEFORE_LOFT = 0,
  RFLM_FBRX_LOFT_BEFORE_RSB = 1,
} rflm_fbrx_algorithm_order_t;

typedef enum
{
  RFLM_FBRX_1_STAGE = 0,
  RFLM_FBRX_2_STAGE = 1,
} rflm_fbrx_upsample_stage_t;

typedef enum
{
  RFLM_FBRX_FFT_256  = 0,
  RFLM_FBRX_FFT_512  = 1,
  RFLM_FBRX_FFT_1024 = 2,
} rflm_fbrx_fft_size_t;

typedef enum
{
  RFLM_FBRX_ENV_SCALE = 0,
  RFLM_FBRX_IQ_SCALE  = 1,
} rflm_fbrx_es_iq_t;

typedef enum
{
  RFLM_FBRX_REAL_FILTER    = 0,
  RFLM_FBRX_COMPLEX_FILTER = 1,
} rflm_fbrx_rx_filter_mode_t;

typedef struct
{ 
  /* vector of 64 AMAM coefs (16 unsigned bits each) */
  uint16 matrix_predist_amam[RFLM_FBRX_AMAM_SIZE];
  /* Threshold for TA |Xcor| uniformity (Test 1: small RB case) */
  uint16 threshold_xcorr_1;
  /* Threshold for  detecting tau=1/2 (Test 2: T.O. ~= 1/2) */
  uint16 threshold_xcorr_2;
  /* 0: Recalculate correlation values 1 :Use the previous Correlation values */
  boolean use_previous_corr_en;
  /* 0: Recalculate estimation values 1 :Use the previous estimation values */
  boolean use_previous_est_en;
  /* Sc16t time domain taps for RX filter */
  uint32 rx_filter_taps[RFLM_FBRX_RX_FILTER_TAPS_SIZE];
  /* only for uplink CA */
  int16 tx_up_filter_taps[RFLM_FBRX_TX_FILTER_TAPS_SIZE];
  /* TxFE index:  0 = TxFE-0, 1 = TxFE-1 */
  uint8  txfe_index;
  /* Enable Carrier Aggregation , 0: Disable, 1 : Enable */
  boolean ca_enable;
  /* only for uplink CA */
  rflm_fbrx_upsample_stage_t ca0_num_upsample_stages;
  /* only for uplink CA */
  rflm_fbrx_upsample_stage_t ca1_num_upsample_stages;
  /* Number of Time Alignment Cross Correlations . Can be only 4,8,16,32 */
  uint8 num_corr_lags;
  /* One-sided sample padding applied to Tx Data , Max 16 */
  uint8 num_tx_extension_samples;
  /* 0 or 1 sample */
  uint8 lag_spacing;
  /* 0: liner interp, 1: frequency domain interpolation */
  uint8 time_align_mode;
  /* Number of samples to be read from DTR-IB */
  uint16 num_tx_in_samples_c0;
  uint16 num_tx_in_samples_c1;
  /* Number of sampled to be read from RxFE output */
  uint16 num_rx_in_samples;
  /* systems to provide values */
  uint16 num_rx_samples;
  /* Padding to Apply to FBRx data for FBRx filter transient response */
  uint8 num_rx_extension_samples;
  /* Group delay of FBRx filter */
  uint8 rx_filter_group_delay;
  /* 0: use predetermined Tau , 1 : Estimate new Tau */
  boolean estimate_tau_en;
  /* 0: Do not apply Tau,  1: Apply Tau */
  boolean apply_tau_en;
  /* Value to scale previous correlation settings. Only used if use_previous_en = TRUE */
  int16 scaling_corr;
  /* Value to scale previous estimate. Only used if use_previous_en = TRUE */
  int16 scaling_est;
  /* Select processing type : 3x3 , 2x2 , 1x1 , 1x0 */
  rflm_fbrx_algorithm_t processing_type;
  /* @todo add one for which correction to apply */
  /* 3x3 subtype: 1=LOFT before RSB, 0 = RSB before LOFT */
  rflm_fbrx_algorithm_order_t subtype_3x3;
  /* Threshold for GAIN Calculation */
  uint16 threshold_gain;
  /* Threshold for RSB Calculation */
  uint16 threshold_rsb;
  /* Threshold for LOFT Calculation */
  uint16 threshold_loft;
  /* fft size sel */
  rflm_fbrx_fft_size_t fft_size_sel;
  /* num_rx_taps */
  uint16 num_rx_taps;
  /* tx_data_len */
  uint16 tx_data_len;
  //predet_tau
  uint16 predet_tau;
  /* txfe_update */
  boolean txfe_update;
  /* scale_rx_en */
  boolean scale_rx_en;
  /* scale_rx 0x7FFF */
  uint16 scale_rx;
  /* scale_tx_en */
  boolean scale_tx_en;
  /* scale_tx 0x7FFF */
  uint16 scale_tx;
  /* dtr_ib_offset */
  int16 dtr_ib_offset_0;
  /* dtr_ib_offset */
  int16 dtr_ib_offset_1;
  /* rx_filter_mode */
  rflm_fbrx_rx_filter_mode_t rx_filter_mode;
  /* @todo in SS but possibly not in API, need to confirm */
  boolean use_uk;
  /* shift in Tx samples to be applied if time-alignment is disabled */
  uint16 predet_index;
  /* use current results for next result */
  boolean use_current_params;
  /* DPD vector param */
  int8 dpd_pcoeff_0[RFLM_FBRX_PCOEFF_SIZE];
  /* DPD vector param */
  int8 dpd_pcoeff_1[RFLM_FBRX_PCOEFF_SIZE];
  int8 dpd_k[RFLM_FBRX_DPD_K_SIZE];
  /* DPD scalar param */
  int16  dpd_src2_value;
  int8   dpd_bias;
  uint16 dpd_uconst;
  uint8  dpd_pwlin_mode;
  int8   dpd_scale;
  /* correlation IIR scale params */
  int16 scale_corr_alpha;
  int16 scale_corr_beta;
  /* estimation IIR scale params */
  int16 est_corr_alpha;
  int16 est_corr_beta;
} rflm_fbrx_static_param_t;

typedef struct
{
  /* Parameter used for equaling (1tau, tau) TA interpolator gain droop */
  int16 gain_comp;
  /* Previous Gain compensation. Needed if use_previous_en = TRUE */
  uint32 previous_gain;
  /* Previous LOFT compensation. Needed if use_previous_en = TRUE */
  uint32 previous_loft;
  /* Previous Residual Side Band estimate. Needed if use_previous_en = TRUE */
  uint32 previous_rsb;
  /* Previous gain imbalance estimate. Needed if use_previous_en = TRUE */
  uint32 previous_gain_imbalance;
  /* Previous phase imbalance estimate. Needed if use_previous_en = TRUE */
  uint32 previous_phase_imbalance;
  /* obsolete, will be removed once removed from fw */
  uint32 gain_droop_center;
  /* obsolete, will be removed once removed from fw */
  uint32 gain_droop_bw;
} rflm_fbrx_dynamic_param_t;

typedef struct
{
  /* fbrx expected gain based on external cal */
  uint32 gain_fbrx;
  /* Threshold for Total LS Error */
  uint16 threshold_ls;
  /* TXFE gain calibration factor */
  uint32 gain_txfe;
  /* 0: Disabled,  1: Enabled. Depends on PA state */
  boolean dpd_enabled;
  /* Current Gain compensation */
  uint32 current_gain;
  /* Current LOFT compensation */
  uint32 current_loft;
  /* Current Residual Side Band estimate */
  uint32 current_rsb;
  /* Current gain imbalance estimate */
  uint32 current_gain_imbalance;
  /* Current phase imbalance estimate */
  uint32 current_phase_imbalance;
  /* Input bias to be applied to format conversion from FBRx buffer */
  uint8 input_bias;
  /* DC compensation for Q channel */
  uint32 wbdc_q;
  /* DC compensation for I channel */
  uint32 wbdc_i;
  /* Select envelope scaling or IQ scaling */
  rflm_fbrx_es_iq_t es_iq_sel;
  /* Rx scale factor */
  uint16 scale_rx;
  /* shift in Tx samples to be applied if time-alignment is disabled*/
  uint16 predet_idx;  
  /* Threshold for GAIN Calculation */
  uint16 thresh_gain_high;
  /* Threshold for GAIN Calculation */
  uint16 thresh_gain_low;
  /* scale_tx 0x7FFF */
  uint16 scale_tx;

} rflm_fbrx_pwr_dependent_param_t;

/*==============================================================================

  FBRx API input/output structure definitions
  
==============================================================================*/

/*==============================================================================
  For rflm_fbrx_get_static_settings
==============================================================================*/
/*! @brief  Input parameters for rflm_fbrx_get_static_settings API */
typedef struct
{
  uint8 reserved;  ///< placeholder. no input for now
} rflm_fbrx_get_static_settings_in_t;


/*! @brief  Output parameters for rflm_fbrx_get_static_settings API */
typedef struct
{
  rflm_fbrx_static_param_t static_settings; ///< static uK parameters
} rflm_fbrx_get_static_settings_out_t;

/*==============================================================================
  For rflm_fbrx_update_results
==============================================================================*/
/*! @brief  Input parameters for rflm_fbrx_update_results API per block */
typedef struct
{
  rflm_tech_id_t tech_id;
  int16          gain_est;
  uint16         next_gain;
  uint32         loft; /* complex */
  uint32         next_loft;
  uint32         rsb;  /* complex */
  uint32         next_rsb;
  int16          gain_imbalance;
  int16          phase_imbalance;
  uint32         cgain_est;
  uint32         model_a;
  uint32         model_b;
  uint32         model_c;
  uint16         xcorr_log[RFLM_FBRX_XCORR_LOG_SIZE];
  uint32         ls_err;
  int32          rx_debug_sample_byte_offset; /* Byte offset of DEBUG RX samples
                                              from MODEM_LMEM_RAM_START_ADDR */
  int32          tx_debug_sample_byte_offset; /* Byte offset of DEBUG RX samples
                                              from MODEM_LMEM_RAM_START_ADDR */
  uint32         fbrx_start_time;
} rflm_fbrx_proc_results_t;

/*! @brief  Input parameters for rflm_fbrx_update_results API */
typedef struct
{
  uint8 num_valid_results;
  rflm_fbrx_proc_results_t results[RFLM_FBRX_LTE_MAX_RUNS_PER_SF];
  /* The following are obsolete in this location. Remove them once fw is updated */
  int16          gain_error;
  uint32         loft; /* complex */
  uint32         rsb;  /* complex */
  int16          gain_imbalance;
  int16          phase_imbalance;
} rflm_fbrx_update_results_in_t;

/*! @brief  Output parameters for rflm_fbrx_update_results API */
typedef struct
{
  uint8 reserved; /* placeholder */
} rflm_fbrx_update_results_out_t;

#ifdef __cplusplus
extern "C" {
#endif


/*==============================================================================
  FBRx FW API's prototypes
==============================================================================*/

/*----------------------------------------------------------------------------*/
extern rflm_err_t rflm_fbrx_get_static_settings( rflm_handle_tx_t handle,
                                                 rflm_fbrx_get_static_settings_in_t  *in,
                                                 rflm_fbrx_get_static_settings_out_t *out );

/*----------------------------------------------------------------------------*/
extern rflm_err_t rflm_fbrx_update_results( rflm_handle_tx_t handle,
                                            rflm_fbrx_update_results_in_t  *in,
                                            rflm_fbrx_update_results_out_t *out );

/*----------------------------------------------------------------------------*/
extern uint32 rflm_fbrx_get_rx_tx_delay ( rflm_handle_tx_t handle );

/*----------------------------------------------------------------------------*/
extern boolean rflm_fbrx_is_handle_valid_for_operation( rflm_handle_tx_t tx_handle );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_API_FBRX_FW_H */
