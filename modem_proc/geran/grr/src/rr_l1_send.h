#ifndef RR_L1_SEND_H
#define RR_L1_SEND_H

/** @file */

/*============================================================================
  @file rr_l1_send.h

  This is the header file for rr_l1_send.c
  It contains functions to send messages to the GL1 task.

                Copyright (c) 2002-2014 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_l1_send.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_gprs_defs.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  RR_L1_IDLE_STATUS_NOT_IDLE,
  RR_L1_IDLE_STATUS_EARLY_CAMP_IDLE,
  RR_L1_IDLE_STATUS_GSM_IDLE,
  RR_L1_IDLE_STATUS_GPRS_IDLE,
  RR_L1_IDLE_STATUS_MAX
} rr_l1_idle_status_e;

typedef struct
{
  uint32 arfcn;
  uint8 rat;
} specific_update_freq_T;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern void rr_send_mph_decode_bcch_list_abort_req(
  const gas_id_t gas_id
);

extern void rr_send_mph_decode_bcch_list_req(
  rr_l1_idle_freq_list_T *frequency_list,
  boolean                 immediate_search,
  boolean                 si3_only,
  const gas_id_t          gas_id
);

extern void rr_send_mph_power_scan_req(
  rr_l1_pscan_results_T   *pscan_results_buffer_ptr,
  rr_l1_band_bitmap_T     requested_bands,
  rr_search_mode_T        search_mode,
  const gas_id_t          gas_id
);

extern void rr_send_mph_power_scan_abort_req(
  const gas_id_t gas_id
);

extern void rr_send_mph_frequency_redefinition_req(
  channel_information_T*  p_channel_information,
  word                    starting_time,
  const gas_id_t          gas_id
);

extern void rr_send_mph_change_page_mode_req(
  paging_mode_T  paging_mode,
  const gas_id_t gas_id
);

extern void rr_send_mph_immed_assign_req(
  boolean                 DTX_indicator,
  byte                    timing_advance,
  boolean                 starting_time_valid,
  word                    starting_time,
  boolean                 before_parameters_valid,
  channel_information_T*  p_channel_info_before,
  channel_information_T*  p_channel_info_after,
  const gas_id_t          gas_id
);

extern void rr_send_mph_reconnect_channel_req(
  const gas_id_t gas_id
);

extern void rr_send_mph_select_specific_channel(
  ARFCN_T        specific_channel,
  BSIC_T           BSIC,
  const gas_id_t gas_id
);

extern void rr_send_mph_channel_release_req(
  boolean                 valid_bcch_carrier,
  ARFCN_T                 serving_bcch_carrier,
  BSIC_T                  BSIC,
  const gas_id_t          gas_id
);

extern void rr_send_mph_start_idle_req(
  layer1_sys_info_T*      p_layer1_sys_info,
  boolean                 imsi_valid,
  word                    imsi_mod_1000,
  boolean                 decode_bcch,
  const gas_id_t          gas_id
);

extern void rr_send_mph_read_sys_info_req(
  rr_l1_si_type_T si_required,
  const gas_id_t  gas_id
);

extern void rr_send_mph_channel_mode_modify_req(
  channel_mode_T          channel_mode,
  channel_type_T          channel_type,
  byte                    subchannel,
  multirate_config_T*     multirate_config_ptr,
  const gas_id_t          gas_id
);

extern void rr_send_mph_channel_assign_req(
  assignment_details_T*   assignment_details_ptr,
  multirate_config_T*     multirate_config_ptr,
  const gas_id_t          gas_id
);

extern void rr_send_mph_stop_handover_access_req(
  byte           timing_advance,
  const gas_id_t gas_id
);

extern void rr_send_mph_handover_req(
  handover_details_T*     handover_details,
  multirate_config_T*     multirate_config_ptr,
  const gas_id_t          gas_id
);

extern void rr_send_mph_set_ciphering_req(
  boolean                 cipher_flag,
  cipher_algorithm_T      cipher_algorithm,
  byte*                   cipher_key,
  byte*                   cipher_key_128,
  const gas_id_t          gas_id
);

extern void rr_send_mph_dedicated_update_req(
  inter_task_BCCH_allocation_T *BCCH_allocation,
  boolean                       DTX_indicator,
  byte                          PWRC,
  byte                          NCC_permitted,
  byte                          multiband_reporting,
  const gas_id_t                gas_id
);

extern void rr_send_mph_idle_update_req(
  layer1_sys_info_T*      p_layer1_sys_info,
  boolean                 imsi_valid,
  word                    imsi_mod_1000,
  const gas_id_t          gas_id
);

extern void rr_send_mph_early_camping_req(
 control_channel_description_T* control_channel_description,
 boolean                        imsi_valid,
 word                           imsi_mod_1000,
 paging_mode_T                  paging_mode,
 const gas_id_t                 gas_id
);

extern void rr_send_mph_abort_ra_req(
  const gas_id_t gas_id
);

/**
  @brief Sends MPH_START_GSM_MODE_REQ to L1.

  @param x2g_resel_in_progress TRUE if an X2G reselection is in progress, FALSE
         otherwise.

  @param x2g_cco_in_progress TRUE if an X2G CCO is in progress, FALSE
         otherwise.
*/
extern void rr_send_mph_start_gsm_mode_req(
  boolean        x2g_resel_in_progress,
  boolean        x2g_cco_in_progress,
  const gas_id_t gas_id
);

extern void rr_send_mph_stop_gsm_mode_req(
  rr_l1_gsm_stop_reason_T gsm_stop_reason,
  const gas_id_t          gas_id
);

extern void rr_send_mph_device_mode_ind(
  const geran_device_mode_cause_t mode_cause,
  const gas_id_t gas_id
);

extern void rr_send_mph_set_priority_ind(
  const geran_priority_t l1_priority,
  const boolean          new_procedure,
  const gas_id_t gas_id
);

#ifdef FEATURE_LTE
/**
  @brief Check whether LTE neighbour list in the given
  MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ EARFCN contains the specific EARFCN.

*/
extern boolean rr_l1_dedicated_wcdma_cell_update_list_req_contains_earfcn(
  mph_dedicated_wcdma_cell_update_list_req_T * cell_update_list_req,
  lte_earfcn_t earfcn
);
#endif /* FEATURE_LTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

/**
  @brief Builds and sends MPH_WCDMA_CELL_UPDATE_LIST_REQ to GERAN L1.

  @param scell Pointer to the serving cell system information database.  If
  NULL an empty message will be sent to GERAN L1, effectively turning off any
  IRAT measurements in L1.
*/
extern void rr_send_mph_wcdma_cell_update_list_req(
  gprs_scell_info_t * scell,
  const gas_id_t      gas_id
);

extern void rr_send_mph_specific_irat_freq_update_req(
  uint32 arfcn,
  rr_rat_e rat,
  uint32 resel_timer,
  specific_update_freq_T *freq_list,
  byte freq_list_length,
  const gas_id_t gas_id
);

extern void rr_send_mph_ignore_wcdma_neighbors_req(
  word uarfcn,
  word scrambling_code,
  boolean ignore_flag,
  boolean threshold_valid,
  uint16 s_qualmin,
  uint16 s_rxlevmin,
  const gas_id_t gas_id
);

extern void rr_send_mph_ue_mode_change_req(
  boolean gsm_only,
  const gas_id_t gas_id
);

/**
  @brief Sends MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ to L1.
*/
extern void rr_send_mph_dedicated_wcdma_cell_update_list_req(
  utran_idle_neighbor_list_T *wcdma_idle_neighbor_list,
  wcdma_ded_neighbor_list_T *wcdma_ded_neighbor_list,
  rr_dedicated_mode_params_t * dedicated_mode_params, 
  const gas_id_t gas_id
);
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

extern void rr_send_mph_reset_req(
  const gas_id_t gas_id
);

#if defined (FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_GSM_TDS)

extern void rr_send_mph_interrat_list_search_time_req(
  rr_l1_list_srch_time_req_command_T command,
  rr_rat_e rat,
  const gas_id_t gas_id
);

extern void rr_send_mph_suspend_gsm_mode_req(
  const gas_id_t gas_id
);

extern void rr_send_mph_resume_gsm_mode_req(
  const gas_id_t gas_id
);

#endif /* FEATURE_GTOW_BACKGROUND_PLMN_SEARCH || FEATURE_GSM_TDS*/

extern void rr_send_mph_start_gsm_mode_for_bplmn_req(
  uint32 timer_val,
  const gas_id_t gas_id
);

extern void rr_send_mph_resume_gsm_mode_for_bplmn_req(
  uint32 timer_val,
  const gas_id_t gas_id
);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
extern void rr_send_mph_l1_ds_abort_ind(
  const rr_ds_abort_type_T      abort_type,
  const rr_l1_message_id_enum_T pending_msg,
  const boolean                 run_to_completion,
  const gas_id_t                gas_id
);

extern void rr_send_mph_dsc_threshold_ind(
  const int8 percent_of_max,
  const l2_channel_type_T l2_channel_type,
  const rr_l1_epd_preference_t epd_pref,
  const gas_id_t gas_id
);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

extern void rr_l1_send_mph_rat_change_ind(
  const gas_id_t gas_id
);

/**
  @brief Sends MPH_ENABLE_SLEEP_IND to GL1.
*/
extern void rr_l1_send_mph_enable_sleep_ind(
  const gas_id_t gas_id
);

#ifdef FEATURE_LTE

/**
  @brief Check whether the MPH_WCDMA_CELL_UPDATE_LIST_REQ contains
         and EARFCN.

  Use to avoid adding duplicate EARFCNs to the signal sent to L1.

  @param cell_update_list_req The signal to search.
  @param earfcn The EARFCN to search for.
*/
extern boolean rr_l1_wcdma_cell_update_list_req_contains_earfcn(
  mph_wcdma_cell_update_list_req_T * cell_update_list_req,
  lte_earfcn_t earfcn
);

/**
  @brief Adds an entry to the given Frequency/Priority list.

  @param dst The list to modify.
  @param rat The RAT of the frequency to be added.
  @param arfcn The ARFCN of the frequency to be added.
  @param priority The priority of the frequency to be added, relative to
         GERAN priority.
*/
extern void rr_irat_frequency_priority_list_add(
  rr_l1_irat_frequency_priority_list_t * dst,
  rr_l1_irat_rat_e                       rat,
  uint32                                 arfcn,
  uint8                                  priority,
  const gas_id_t                         gas_id //tlx
);


/*!
  @param priority_and_threshold_information_available TRUE if priority and
         threshold information are available and the priority-based cell
         reselection algorithm should be used, FALSE otherwise.
*/
extern void rr_wcdma_cell_list_add(
  wcdma_cell_list_T *                    cell_list,
  rr_utran_cell_T                        cell,
  boolean                                priority_and_threshold_information_available,
  rr_l1_irat_frequency_priority_list_t * frequency_priority_list,
  const gas_id_t                         gas_id
);

#endif /* FEATURE_LTE */

extern void rr_send_mph_start_gprs_idle_mode_req(
  ccch_pccch_params_t*,
  const gas_id_t gas_id
);

extern void rr_send_mph_gprs_l1param_update_req(
  ccch_pccch_params_t*,
  const gas_id_t gas_id
);

extern void rr_send_mph_nc_measurements_req(
  inter_task_BCCH_allocation_T*,
  const gas_id_t gas_id
);

extern void rr_send_mph_stop_nc_measurements_req(
  inter_task_BCCH_allocation_T*,
  const gas_id_t gas_id
);

extern boolean rr_send_mph_gprs_surround_update_req(
  gprs_surround_data_t*,
  boolean,
  bcch_ncell_info_t*,
  const gas_id_t gas_id
);

extern void rr_send_mph_gprs_reselection_req(
  gprs_surround_data_t *,
  boolean sync_flag,
  const gas_id_t gas_id
);

extern void rr_send_mph_stop_surround_update_req(
  const gas_id_t gas_id
);

extern void rr_send_mph_start_mm_non_drx_ind(
  const gas_id_t gas_id
);

extern void rr_send_mph_stop_mm_non_drx_ind(
  const gas_id_t gas_id
);

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
extern void rr_gprs_send_mph_wcdma_cell_update_list_req(
  const gas_id_t gas_id
);
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

#ifdef FEATURE_GSM_DTM
extern void rr_send_mph_dtm_channel_release_req(
  boolean        valid_bcch_carrier,
  ARFCN_T        serving_bcch_carrier,
  BSIC_T         BSIC,
  boolean        ps_release,
  const gas_id_t gas_id
);
#ifdef FEATURE_GSM_EDTM
extern void rr_send_mph_dtm_l1param_update_req(
  gprs_scell_info_t *,
  const gas_id_t gas_id
);
#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
extern void rr_l1_send_mph_ded_wcdma_cell_upd_list_req_for_pseudo_fr(
  tdsrr_ho_pseudo_fast_return_type *pseudo_fr_ptr,
  const gas_id_t gas_id
);
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

#ifdef FEATURE_MODEM_CONFIG_REFRESH
extern void rr_l1_send_mph_nv_refresh_req(const gas_id_t gas_id);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

extern void rr_l1_send_mph_deep_sleep_ind(const gas_id_t gas_id);

#ifdef FEATURE_GSM_SCE_REDUCED_NCELL_LIST

extern void rr_l1_send_reduced_ncell_meas_ind(
  boolean reduced_ncell_list,
  const gas_id_t gas_id
);

#endif /* FEATURE_GSM_SCE_REDUCED_NCELL_LIST*/

#ifdef FEATURE_IDLE_DRX_SCALING
extern void rr_l1_send_mph_update_idle_drx_ind(
  const byte     bs_pa_mfrms,
  const boolean  extended_bs_pa_mfrms_valid,
  const byte     extended_bs_pa_mfrms,
  const boolean  skip_idle_mode_meas,
  const gas_id_t gas_id
);
#endif /* FEATURE_IDLE_DRX_SCALING */

/**
  @brief Returns the current L1 idle status
*/
extern rr_l1_idle_status_e rr_l1_send_get_l1_idle_status(const gas_id_t gas_id);

/*!
 * \brief Returns the last timing advance info given to L1.
 * 
 * \param timing_advance_ptr 
 * \param gas_id 
 * 
 * \return boolean - TRUE if the timing advance value is valid, FALSE otherwise
 */
extern boolean rr_l1_send_get_current_timing_advance_info(byte *timing_advance_ptr, const gas_id_t gas_id);

/**
  @brief Initialisation function.
*/ 
extern void rr_l1_send_init(const gas_id_t gas_id);

/**
  @brief Initialisation function. Should be called once at RR task start
*/ 
extern void rr_l1_send_task_start_init(void);

extern void rr_send_mph_xmsi_update_ind(const gas_id_t gas_id);

#endif /* RR_L1_SEND_H */

/* EOF */

