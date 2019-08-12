#ifndef RR_NV_H
#define RR_NV_H

/*! \file rr_nv.h
 
  This is the header file for rr_nv.c.
  This module contains access functions for NV data.
 
                Copyright (c) 2011-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_nv.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "mm_rr.h"
#include "rr_defs.h"
#include "sys.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  /* GERAN Information */
  uint8 geran_priority;
  uint8 thresh_priority_search;
  uint8 thresh_gsm_low;
  uint8 h_prio;
  uint8 t_reselection;
} rr_geran_def_data_t;

typedef struct
{
  /* UTRAN information */
  uint8 priority;
  uint8 thresh_high;
  uint8 thresh_low;
  uint8 qrxlevmin;

} rr_utran_def_data_t;

typedef struct
{    
  /* LTE Information */
  uint8 priority;
  uint8 meas_bandwidth;
  uint8 thresh_high;
  uint8 thresh_low;
  uint8 qrxlevmin;  

} rr_lte_def_data_t;
    
typedef struct
{
  boolean pseudo_si2q_def_data_valid;
  rr_geran_def_data_t rr_geran_def_data;
  rr_utran_def_data_t rr_utran_def_data;
  rr_lte_def_data_t  rr_lte_def_data;
  
} rr_efs_pseudo_si2q_def_data_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns the value of SPLIT_PAGE_CYCLE stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint16 
 */
extern uint16 rr_get_nv_split_page_cycle(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Returns the value of NON_DRX_TIMER.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_non_drx_timer(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Returns the value of MULTISLOT_CLASS stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_multislot_class(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Returns the value of ANITE_GCF_FLAG stored in NV. 
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
extern boolean rr_get_nv_anite_gcf_flag(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Indicates if EGPRS is enabled in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
extern boolean rr_get_nv_egprs_enabled(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Returns the value of EGPRS_MULTISLOT_CLASS stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_egprs_multislot_class(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Returns TRUE if the NV parameter EGPRS_8PSK_POWER_CLASS is non-zero; FALSE if zero.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
extern boolean rr_get_nv_egprs_8psk_ul_enabled(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Returns the EGPRS 8PSK power capability.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_egprs_8psk_power_capability(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Indicates if GERAN Feature Pack 1 is enabled in NV
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
extern boolean rr_get_nv_geran_feature_pack_1_enabled(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Returns the current mask from NV that indicates support for inter-RAT NACC.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 - 0x01 indicates W2G, 0x02 indicates G2W, 0x03 indicates both
 */
extern uint8 rr_get_nv_interrat_nacc_support(const sys_modem_as_id_e_type as_id);

#ifdef FEATURE_GSM_DTM

/*!
 * \brief Indicates if DTM is enabled in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
extern boolean rr_get_nv_dtm_enabled(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Returns the value of DTM_EGPRS_MULTISLOT_SUBCLASS stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_dtm_egprs_multislot_subclass(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Returns the value of DTM_MULTISLOT_CLASS, derived from DTM_EGPRS_MULTISLOT_SUBCLASS.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_dtm_multislot_class(const sys_modem_as_id_e_type as_id);

#ifdef FEATURE_GSM_EDTM
/*!
 * \brief Indicates if EDTM is enabled in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled; FALSE otherwise
 */
extern boolean rr_get_nv_edtm_enabled(const sys_modem_as_id_e_type as_id);
#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

/*!
 * \brief Returns the value of GEA_SUPPORTED stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_gea_supported(const sys_modem_as_id_e_type as_id);

#ifdef FEATURE_GSM_GPRS_MSC33

/*!
 * \brief Returns the value of HMSC_GPRS_CODED_MS_CLASS stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_hmsc_gprs_coded_ms_class(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Returns the value of HMSC_EGPRS_CODED_MS_CLASS stored in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return uint8 
 */
extern uint8 rr_get_nv_hmsc_egprs_coded_ms_class(const sys_modem_as_id_e_type as_id);

#endif /* FEATURE_GSM_GPRS_MCS33 */

#ifdef FEATURE_VAMOS
/*!
 * \brief Indicates if VAMOS is enabled in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE is VAMOS is enabled in NV (VAMOS-I or VAMOS-II); FALSE otherwise
 */
extern boolean rr_get_nv_vamos_enabled(const sys_modem_as_id_e_type as_id);
#endif /* FEATURE_VAMOS */

/*!
 * \brief Indicates if Fast SI Acq during CSFB is enabled in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return TRUE if Fast SI Acq during CSFB is enabled, FALSE otherwise.
 */
extern boolean rr_get_nv_fast_si_acq_during_csfb_enabled(const sys_modem_as_id_e_type as_id);

extern boolean rr_nv_x2g_fast_si_acq_feature_is_enabled(const sys_modem_as_id_e_type as_id);
extern boolean rr_nv_x2g_fast_si_acq_is_plmn_match(sys_plmn_id_s_type plmn, const sys_modem_as_id_e_type as_id);

/*!
 * \brief Inidicates if Fast SI Acq when CS-only is enabled in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_fast_si_acq_when_cs_only_is_enabled(const sys_modem_as_id_e_type as_id);

#ifdef FEATURE_GSM_GPRS_READ_CCCH_IN_XFER 
/*!
 * \brief Indicates if the supplied PLMN is Read PCH during transfer is enabled in NV.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_read_pch_during_transfer_is_plmn_match(sys_plmn_id_s_type plmn, const gas_id_t gas_id);

#endif /*FEATURE_GSM_GPRS_READ_CCCH_IN_XFER */
/*!
 * \brief Indicates if the supplied PLMN is present in the white-list for C1 relaxation info.
 * 
 * \return boolean - TRUE if present, FALSE otherwise
 */
extern boolean rr_nv_c1_relaxation_is_plmn_match(sys_plmn_id_s_type plmn, const gas_id_t gas_id);

/*!
 * \brief Returns the value of the power-scan re-use timer (0=disabled)
 * 
 * \return rex_timer_cnt_type - timer value in milliseconds
 */
extern rex_timer_cnt_type rr_nv_get_pscan_results_reuse_time_millisecs(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Returns the value of the plmn_search_freq_pruning_enabled.
 * 
 * \return TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_get_plmn_search_freq_pruning_enabled(const sys_modem_as_id_e_type as_id);

#ifdef FEATURE_SGLTE
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
/*!
 * \brief Indicates if SGLTE Connected Mode Reselection feature is enabled in NV.
 * 
 * \return boolean - TRUE if SGLTE SGLTE Connected Mode Reselection is enabled, FALSE otherwise
 */
extern boolean rr_get_nv_sglte_g2x_conn_mode_resel_enabled(const sys_modem_as_id_e_type as_id);
#endif /* FEATURE_SGLTE_G2X_CONN_MODE_RESEL */
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
/**
  @brief Returns the Fast G2L Using Pseudo SI2Q Enabled NV value

  @return boolean
*/
extern boolean rr_is_efs_nv_fast_g2l_using_pseudo_si2q_enabled(const sys_modem_as_id_e_type as_id);

extern boolean rr_is_efs_nv_fast_g2l_using_pseudo_si2q_data_valid(const sys_modem_as_id_e_type as_id);

extern rr_efs_pseudo_si2q_def_data_t* rr_get_efs_nv_fast_g2l_pseudo_si2q_data(const sys_modem_as_id_e_type as_id);

#endif /*FEATURE_FAST_G2L_PSEUDO_SI2Q*/


/*!
 * \brief Returns the additional C2 hysteresis values.
 * 
 * \return TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_get_add_crh_enabled(
  int8 *add_crh_ptr,
  int8 *add_crh_scell_thresh_db_ptr,
  const sys_modem_as_id_e_type as_id
);

/*!
 * \brief Indicates if the sys info cache functionality is enabled.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_sys_info_cache_is_enabled(const sys_modem_as_id_e_type as_id);

/*!
 * \brief Indicates if the iterative updating during SI acq feature is enabled.
 *        Note: Feature is not disabled for a Type Approval SIM.
 * 
 * \param as_id (in) - AS-ID (Note: Not GAS-ID)
 * 
 * \return boolean - TRUE if enabled, FALSE otherwise
 */
extern boolean rr_nv_iterative_si_acq_is_enabled(const sys_modem_as_id_e_type as_id);

#ifdef FEATURE_GSM_BAND_AVOIDANCE

extern boolean rr_is_band_avoidance_enabled(const sys_modem_as_id_e_type as_id);

/**
  @brief Returns the threshold for band avoidance ncells as 
         rxlev (0.63)
*/
extern uint8 rr_get_band_avoid_ncell_threshold(const sys_modem_as_id_e_type as_id);

/**
  @brief Returns the max number of meas repts that can be sent 
  when doing band avoidance. 
*/
extern uint8 rr_get_band_avoid_max_meas_rpts(const sys_modem_as_id_e_type as_id);

#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

#ifdef FEATURE_LTE

extern boolean rr_nv_get_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(const sys_modem_as_id_e_type as_id);
extern boolean rr_nv_get_g2l_blind_redir_after_csfb_enabled(const sys_modem_as_id_e_type as_id);
extern boolean rr_nv_get_g2l_blind_redir_after_srvcc_enabled(const sys_modem_as_id_e_type as_id);

#ifdef FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB

/**
  @brief get the csfb fcch snr offset for csfb cell selection

  @return the value of csfb_fcch_snr_offset
*/ 
extern uint16 rr_nv_get_csfb_fcch_snr_offset(const sys_modem_as_id_e_type as_id);

/**
  @brief get the csfb rssi offset for csfb cell selection

  @return the value of csfb_rssi_offset
*/ 
extern uint16 rr_nv_get_csfb_rssi_offset(const sys_modem_as_id_e_type as_id);

#endif /* FEATURE_SNR_BASED_CELL_SEL_FOR_CSFB */

#endif /* FEATURE_LTE */

extern boolean rr_nv_get_g2w_measurements_enabled(sys_modem_as_id_e_type as_id);
extern boolean rr_nv_get_g2t_measurements_enabled(sys_modem_as_id_e_type as_id);
extern boolean rr_nv_get_g2l_measurements_enabled(sys_modem_as_id_e_type as_id);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*!
 * \brief Returns the value of the serving cell resync timer
 * 
 * \return uint32 - timer value in milliseconds
 */
extern uint32 rr_nv_get_scell_resync_time_millisecs(const sys_modem_as_id_e_type as_id);

extern uint32 rr_nv_get_multisim_si_refresh_duration_milliseconds(sys_modem_as_id_e_type as_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#ifdef FEATURE_IDLE_DRX_SCALING
extern uint32 rr_nv_get_idle_drx_scaling_suspend_time_millisecs(const sys_modem_as_id_e_type as_id);
#endif /* FEATURE_IDLE_DRX_SCALING */

#ifdef FEATURE_GERAN_HST_OPT
/*!
 * \brief Indicates if High Speed Train Optimization can be used with the supplied PLMN.
 * 
 * \param plmn (in), gas_id (in)
 * 
 * \return boolean - TRUE if PLMN matches, FALSE otherwise.
 */
extern boolean rr_nv_hst_opt_is_plmn_match(sys_plmn_id_s_type plmn, const gas_id_t gas_id);
#endif /*FEATURE_GERAN_HST_OPT*/

/*!
 * \brief Called when the NV data is received from NAS (RR_GMM_GPRS_NV_PARAMS_IND).
 * 
 * \param nv_params_ind_ptr 
 */
extern void rr_nv_process_nv_params_ind(rr_gmm_gprs_nv_params_ind_t *nv_params_ind_ptr);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*!
 * \brief Called when the NV refresh ind received from MCFG.
 *  
 * Re-reads carrier specific EFS/NV items. 
 * 
 * \param gas_id - GAS ID
 */
extern void rr_nv_process_nv_refresh_ind(const gas_id_t gas_id);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
#ifdef FEATURE_LTE
extern boolean rr_nv_depriotize_gsm_cell_is_enabled(const gas_id_t gas_id);
#endif /*FEATURE_LTE*/

#ifdef PERLUTF
#error code not present
#endif // PERLUTF

extern boolean rr_get_fake_bs_detection_enabled(const gas_id_t gas_id);

#endif /* #ifndef RR_NV_H */

/* EOF */

