#ifndef RR_CONTROL_H
#define RR_CONTROL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_control.h

GENERAL DESCRIPTION
   This module contains functions for the rr control function.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2002-2014 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_control.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/02   rmc     QXDM Cleanup

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "rr_defs.h"
#include "rr_task.h"
#include "rr_log.h"
#include "geran_multi_sim.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

#define BPLMN_INVALID_TRANS_ID 0xFF

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

typedef enum
{
  RR_X2G_RESEL_TYPE_UNDEFINED = -1,
  RR_X2G_RESEL_TYPE_W2G_RESELECTION,
  RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER,
  RR_X2G_RESEL_TYPE_L2G_RESELECTION,
  RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER,
  RR_X2G_RESEL_TYPE_T2G_RESELECTION,
  RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER, 
  RR_X2G_RESEL_TYPE_L2G_SGLTE_REDIRECTION,
  RR_X2G_RESEL_TYPE_T2G_SGLTE_REDIRECTION,
  RR_X2G_RESEL_TYPE_MAX
} rr_x2g_resel_type_e;

typedef enum
{
  RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED = -1,
  RR_X2G_RESEL_ABORT_CAUSE_NAS,
  RR_X2G_RESEL_ABORT_CAUSE_IRAT,
  RR_X2G_RESEL_ABORT_CAUSE_CCO_TIMER_EXPIRY,
  RR_X2G_RESEL_ABORT_CAUSE_CCO_ABORT,
  RR_X2G_RESEL_ABORT_CAUSE_MAX
} rr_x2g_resel_abort_cause_e;

typedef enum
{
  RR_X2G_RESELECTION_FAILURE,
  RR_X2G_REDIRECTION_FAILURE,
  RR_X2G_HANDOVER_FAILURE
} rr_proc_failure_type_t;

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

extern void rr_set_perform_cell_selection_after_plmn_list_construction(const gas_id_t gas_id);

extern boolean rr_is_control_state(const rr_state_T, const gas_id_t gas_id);
extern rr_state_T get_rr_state(const gas_id_t gas_id);
extern const char *rr_main_state_name(rr_state_T state);
extern const char *rr_main_current_state_name(const gas_id_t gas_id);

/* Used as a flag to send RR abort indication to MM when a cell reselection is forced
due to serving cell getting barred during connection establishment */
extern rr_log_reselection_trigger_T rr_log_reselection_trigger;

extern void rr_control_init(
  const gas_id_t gas_id
);

extern void rr_panic_reset_triggered_by_l1(
  const gas_id_t gas_id
);

extern void rr_save_msg_for_recovery_after_panic_reset(
  rr_cmd_bdy_type* message_ptr,
  const gas_id_t gas_id
);

/**
  @brief Determine if a message saved before a panic reset matches the
         specified message set.

  @param message_set The message set to check for.
  @return TRUE if the message saved before panic reset matches the specified
          message set, FALSE otherwise.
*/
extern boolean rr_verify_panic_reset_saved_message_set(
  uint8 message_set,
  const gas_id_t gas_id
);

/**
  @brief Determine if a message saved before a panic reset matches the
         specified message set and message id.

  @param message_set The message set to check for.
  @param message_id The message id to check for.
  @return TRUE if the message saved before panic reset matches the specified
          message set and message id, FALSE otherwise.
*/
extern boolean rr_verify_panic_reset_saved_msg(
  uint8 message_set,
  uint8 message_id,
  const gas_id_t gas_id
);

/**
  @brief Determines if there is a RR_EST_REQ (any type) currently saved

  @return TRUE if the saved message is RR_EST_REQ (any type), FALSE otherwise.
*/
extern boolean rr_saved_msg_is_rr_est_req(const gas_id_t gas_id);

/**
  @brief Determines if there is a RR_EST_REQ (Emergency Call) currently saved

  @return TRUE if the saved message is RR_EST_REQ (Emergency Call), FALSE otherwise.
*/
extern boolean rr_saved_msg_is_rr_est_req_emergency_call(const gas_id_t gas_id);

/**
  @brief Prepares an OTA message received in a DL_UNIT_DATA_IND message
  (unacknowledged mode) for parsing and determines if the message can be
  decoded or not.
 
  @param message_ptr Pointer to DL_UNIT_DATA_IND carrying the OTA message.
  @return TRUE if the message is syntactically valid, FALSE otherwise.
*/
extern boolean rr_prepare_ota_msg_for_parsing(
  dl_unit_data_ind_T * message_ptr,
  const gas_id_t gas_id
);

/**
  @brief Prepares an OTA message received in a DL_DATA_IND message
  (acknowledged mode) for parsing and determines if the message can be decoded
  or not.

  @param message_ptr Pointer to DL_DATA_IND carrying the OTA message.
  @param error_type_ptr [out] set to the error type value if the message is not OK.
  @return TRUE if the message is syntactically valid, FALSE otherwise.
*/
extern boolean rr_prepare_ota_msg_for_parsing_in_dedicated(
  dl_data_ind_T * message_ptr,
  RR_cause_T    * error_type_ptr,
  const gas_id_t gas_id
);

extern boolean rr_save_msg(rr_cmd_bdy_type *new_msg_ptr, const gas_id_t gas_id);
extern void rr_reject_msg(rr_cmd_bdy_type* msg_ptr, const gas_id_t gas_id);

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
extern boolean rr_save_msg_for_edtm(rr_cmd_bdy_type *, const gas_id_t gas_id);
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

extern void    rr_delete_saved_mac_ul_tbf_req(const gas_id_t gas_id);
extern boolean rr_in_dedicated(const gas_id_t gas_id);
extern boolean rr_in_connection_pending(const gas_id_t gas_id);
extern boolean rr_background_plmn_search_in_progress(const gas_id_t gas_id);
extern boolean rr_is_inactive(const gas_id_t gas_id);
extern boolean rr_is_acquiring_si_after_conn_rel(const gas_id_t gas_id);

extern rr_event_T rr_process_saved_message(rr_cmd_bdy_type **new_message_ptr_ptr, const gas_id_t gas_id);
extern rr_event_T rr_examine_saved_message(rr_cmd_bdy_type **new_message_ptr_ptr, const gas_id_t gas_id);
extern void rr_save_rr_est_req(rr_est_req_T *rr_est_req_ptr, const gas_id_t gas_id);
extern boolean rr_get_saved_msg(rr_cmd_bdy_type ** message_ptr_ptr, const gas_id_t gas_id);
extern boolean rr_can_process_resel_abort_req(const gas_id_t gas_id);
extern boolean rr_can_process_rr_abort_req(const gas_id_t gas_id);
extern boolean rr_can_process_saved_msg_ind(const gas_id_t gas_id);
extern boolean rr_can_process_rr_event_ind(const gas_id_t gas_id);
extern boolean rr_can_process_mm_message(const gas_id_t gas_id);
extern boolean rr_can_process_timer_expiry(rr_timer_id_e timer_id, const gas_id_t gas_id);
extern boolean rr_can_process_serving_meas_ind(const gas_id_t gas_id);
extern boolean rr_can_process_surround_meas_ind(const gas_id_t gas_id);
extern boolean rr_can_process_msc_change_ind(const gas_id_t gas_id);
extern boolean rr_can_process_sglte_x2g_req(const gas_id_t gas_id);
extern boolean rr_can_process_no_ps_access_req(const gas_id_t gas_id);
extern boolean rr_can_process_ps_access_req(const gas_id_t gas_id);

/**
  @brief Stores pending mode change data.
*/
extern void rr_pending_mode_change_data_store(
  rr_change_mode_ind_T * change_mode_ind,
  const gas_id_t gas_id
);

/**
  @brief Clears any pending mode change data.
*/
extern void rr_pending_mode_change_data_clear(const gas_id_t gas_id);

/**
  @brief Copies pending change mode data to PLMN Request Details store.
*/
extern void rr_pending_mode_change_data_action(const gas_id_t gas_id);


/**
  @brief Checks whether an X2G Cell Change Order procedure is in progress.

  @return TRUE if an X2G Cell Change Order is in progress, FALSE otherwise.
*/
extern boolean rr_x2g_cco_in_progress(const gas_id_t gas_id);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)

/**
  @brief Checks whether an X2G CCO with NACC procedure is in progress.

  @return TRUE if an X2G CCO with NACC procedure is in progress, FALSE otherwise.
*/
extern boolean rr_x2g_cco_with_nacc_in_progress(const gas_id_t gas_id);

/**
  @brief Checks whether RR_ACTIVATION_RSP message received during X2G CCO.

  @return TRUE if RR_ACTIVATION_RSP message received, FALSE otherwise.
*/
extern boolean rr_get_x2g_cco_act_rsp_rcvd(const gas_id_t gas_id);

/**
  @brief Sets the flag indicating whether RR_ACTIVATION_RSP message received during X2G CCO.
*/
extern void rr_set_x2g_cco_act_rsp_rcvd(boolean act_rsp_rcvd, const gas_id_t gas_id);

/**
  @brief Checks whether a CCO for CSFB is in progress.

  @return TRUE if a CCO for CSFB is in progress, FALSE, otherwise.
*/
extern boolean rr_x2g_cco_for_csfb_in_progress(const gas_id_t gas_id);

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

/**
  @brief Handles MPH_SURROUND_UPDATE_FAILURE_IND
 
  @Param: restart - to restart timer count when SU failure is not due to state change.

  @Param: resume - to resume sys info handling when SU failure is not due to state change.
 */
extern void rr_handle_surround_update_failure_ind(
  mph_surround_update_failure_ind_T *su_fail_ptr,
  const boolean                      restart,
  const boolean                      resume,
  const gas_id_t                     gas_id
);

/**
  @brief This is the main RR state machine.

  @param rr_event An input event or command for the state machine.
  @param new_message Optional pointer to a message for the state machine.
         This should only be non-NULL if rr_event == EV_INPUT_MESSAGE.
*/
extern void rr_control(rr_event_T rr_event, rr_cmd_bdy_type* new_message, const gas_id_t gas_id);

#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING

/**
  @brief Returns TRUE if the last cell selection attempt was unsuccessful,
         indicating that the UE is OOS, FALSE otherwise.
*/
extern boolean rr_get_ue_oos_flag(const gas_id_t gas_id);

/**
  @brief Sets the flag indicating whether the UE is OOS to the specified value.
*/
extern void rr_set_ue_oos_flag(boolean ue_oos, const gas_id_t gas_id);

#endif /* FEATURE_GSM_TX_DIV_ANTNA_SWITCHING */

#ifdef FEATURE_LTE

/**
  @brief Determine if LTE is supported by the UE hardware.

  @return TRUE if LTE is supported by the UE hardware, FALSE otherwise.
*/
extern boolean rr_is_lte_supported(const gas_id_t gas_id);

/**
  @brief Determine if G2L reselection is currently allowed by NAS.

  G2L reselection may be disallowed if the LTE network rejected UE registration
  via reject cause 14.

  @return TRUE if G2L reselection is allowed, FALSE otherwise.
*/
extern boolean rr_g2l_resel_allowed(const gas_id_t gas_id);

/**
  @brief Determine whether G2L blind redirection is enabled or not.

  @return TRUE if G2L blind redirection is enabled, FALSE otherwise.
*/
extern boolean rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(const gas_id_t gas_id);

/**
  @brief Determine whether G2L blind redirection should be enabled only after
         a CSFB call, or after any call.

  @return TRUE if G2L blind redirection should be enabled only after a CSFB call,
          FALSE if G2L blind redirection should be enabled after any voice call
                or periodic LAU.
*/
extern boolean rr_g2l_blind_redir_after_csfb_enabled(const gas_id_t gas_id);
extern boolean rr_g2l_blind_redir_after_srvcc_enabled(const gas_id_t gas_id);


/**
  @brief Determines whether RR is able to process an
         RR_CSFB_BLIND_REDIR_TO_LTE_REQ or whether it should be buffered until
         RR changes to a different state.

  @return TRUE if RR is able to process an RR_CSFB_BLIND_REDIR_TO_LTE_REQ,
          FALSE if it should be buffered in the RR pending command queue.
*/
extern boolean rr_can_process_csfb_blind_redir_to_lte_req(const gas_id_t gas_id);

#endif /* FEATURE_LTE */


extern boolean rr_can_process_change_mode_ind(const gas_id_t gas_id);


#ifdef FEATURE_SGLTE

extern boolean rr_is_pending_mode_change(const gas_id_t gas_id);
extern void rr_set_pending_mode_change(boolean mode_change_status, const gas_id_t gas_id);

/**
  @brief Determines whather RR is able to process an RR_STOP_GSM_MODE_REQ with
         stop reason RR_SGLTE_RAT_CHANGE.

  @return TRUE if RR is able to process the message, FALSE if it should be
          buffer in the RR pending command queue.
*/
extern boolean rr_can_process_stop_gsm_mode_req_sglte_rat_change(const gas_id_t gas_id);
extern boolean rr_can_process_sglte_l2g_ho_req(const gas_id_t gas_id);

#endif /* FEATURE_SGLTE */


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

/**
  @brief Determine if IRAT neighbour cell measurements are allowed.

  IRAT neighbour cell measurements are not allowed during X2G reselection,
  until RR has received RR_ACTIVATION_RSP.

  IRAT neighbour cell measurements are not allowed during X2G CCO procedures
  until the procedure has fully completed and RR has received a response
  from the originating RAT indicating that it has deactivated completed.

  @return TRUE if IRAT measurements are allowed, FALSE if not.
*/
extern boolean rr_g2x_measurements_allowed(const gas_id_t gas_id);

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

/**
  @brief Determines if G2W measurements are enabled in NV.

  @return TRUE if G2W measurements are enabled, FALSE if disabled.
*/
extern boolean rr_g2w_measurements_enabled_in_nv(gas_id_t gas_id);

/**
  @brief Determines if G2T measurements are enabled in NV.

  @return TRUE if G2T measurements are enabled, FALSE if disabled.
*/
extern boolean rr_g2t_measurements_enabled_in_nv(gas_id_t gas_id);

/**
  @brief Determines if G2L measurements are enabled in NV.

  @return TRUE if G2L measurements are enabled, FALSE if disabled.
*/
extern boolean rr_g2l_measurements_enabled_in_nv(const gas_id_t gas_id);

/** 
 @brief Indicates whether a MT CSFB call is in progress. 
 
 @return TRUE if a MT CSFB call is in progress, FALSE if not. 
*/   
extern boolean rr_is_mtcsfb_call_in_progress(const gas_id_t gas_id); 

/**
  @brief Indicates whether a CSFB call is in progress.

  @return TRUE if a CSFB call is in progress, FALSE if not.
*/
extern boolean rr_is_csfb_call_in_progress(
  const gas_id_t gas_id
);

/**
  @brief Determines whether fast SI acquisition should be enabled.

  Fast SI acquisition allows RR to start idle mode after receiving just
  SI1 and SI3, to reduce the time taken to establish the CSFB call.

  @param wait_for_si13 Output parameter, if provided will be set to TRUE
         if RR should wait for SI13 (if broadcast) before starting idle,
         or FALSE is SI13 is not required before starting idle mode.
  @return TRUE if fast SI acquisition is allowed, FALSE otherwise.
*/
extern boolean rr_is_fast_si_acq_enabled(
  boolean * wait_for_si13,
  const gas_id_t gas_id
);

extern byte rr_ms_get_bplmn_failed_id(const gas_id_t gas_id);

extern void rr_ms_set_bplmn_failed(const gas_id_t gas_id);

/**
  @brief Determines if all subssciber are in RR_INACTIVE state

  @param 
  @return TRUE all subscribers are in RR_INACTIVE, FALSE otherwise.
*/
extern boolean rr_is_inactive_for_all_subs(void);

extern boolean rr_allow_negative_c1(
  const sys_plmn_id_s_type plmn,
  const gas_id_t           gas_id
);

#ifdef FEATURE_GSM_TDS
/**
  @brief Determines if T2G reselection is in progress

  @param None
  @return TRUE T2G resel is in progress, FALSE otherwise.
*/
boolean rr_t2g_resel_in_progress(const gas_id_t gas_id);

/**
  @brief Determines if T2G CCO is in progress

  @param None
  @return TRUE T2G CCO is in progress, FALSE otherwise.
*/
boolean rr_t2g_cco_in_progress(const gas_id_t gas_id);

#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE_TO_GSM
/**
  @brief Determines if L2G reselection is in progress

  @param None
  @return TRUE L2G resel is in progress, FALSE otherwise.
*/
boolean rr_l2g_resel_in_progress(const gas_id_t gas_id);
/**
  @brief Determines if L2G CCO is in progress

  @param None
  @return TRUE L2G CCO is in progress, FALSE otherwise.
*/
boolean rr_l2g_cco_in_progress(const gas_id_t gas_id);
#endif /*FEATURE_LTE_TO_GSM*/

/**
  @brief Determines if GRR is operating in SGLTE mode.

  @return TRUE if GRR is operating in SGLTE mode, FALSE if not.
*/
extern boolean rr_is_sglte_mode_active(const gas_id_t gas_id);

#ifdef FEATURE_SGLTE

extern boolean rr_is_sglte_x2g_resel_in_progress(const gas_id_t gas_id);

extern boolean rr_sglte_is_full_service_available(const gas_id_t gas_id);

extern boolean rr_can_process_sglte_band_coex_omsg_supported_bands_updated_ind(const gas_id_t gas_id);

extern void rr_sglte_x2g_cco_failed_cb_fn(gas_id_t gas_id);

extern void rr_sglte_x2g_cco_aborted_cb_fn(gas_id_t gas_id);

extern void rr_sglte_x2g_resel_aborted_cb_fn(gas_id_t gas_id);

extern void rr_sglte_x2g_cco_nas_split_cb_fn(gas_id_t gas_id);

extern void rr_sglte_x2g_redir_aborted_cb_fn(gas_id_t gas_id);

boolean rr_is_lte_in_neighbour_list(const gas_id_t gas_id);

boolean rr_sglte_inhibit_tds_bplmn_search(const gas_id_t gas_id);

boolean rr_sglte_inhibit_lte_bplmn_search(const gas_id_t gas_id);

boolean rr_sglte_g2x_bplmn_search_active(const gas_id_t gas_id);

void rr_set_sglte_perform_g2l_blind_redir(boolean setting, const gas_id_t gas_id);

boolean rr_sglte_perform_g2l_blind_redir(const gas_id_t gas_id);

extern boolean rr_save_l2g_msg(rr_cmd_bdy_type * new_msg_ptr, const gas_id_t gas_id);
extern boolean rr_get_saved_l2g_msg(rr_cmd_bdy_type * * message_ptr_ptr, const gas_id_t gas_id);
extern boolean rr_saved_l2g_msg_present(const gas_id_t gas_id);
extern rr_event_T rr_process_saved_l2g_message(rr_cmd_bdy_type **new_message_ptr_ptr, const gas_id_t gas_id);

extern void rr_delete_saved_l2g_msg(const gas_id_t gas_id);
extern boolean rr_sglte_internal_stop_gsm_being_processed(const gas_id_t gas_id);
extern boolean rr_sglte_saved_l2g_ho_being_processed(const gas_id_t gas_id);
extern void rr_sglte_reinit_processing_l2g_ho_related_paramter(const gas_id_t gas_id);
extern void rr_reject_nas_msg_during_intrn_shutdown(rr_cmd_bdy_type* msg_ptr, const gas_id_t gas_id);
extern void rr_sglte_store_serving_cell_info(const gas_id_t gas_id);
extern boolean rr_sglte_get_stored_serv_gprs_ind(const gas_id_t gas_id);
extern ARFCN_T rr_sglte_get_stored_serv_ARFCN(const gas_id_t gas_id);

extern BSIC_T rr_sglte_get_stored_serv_BSIC(const gas_id_t gas_id);

extern int16 rr_sglte_get_stored_serv_RSSI(const gas_id_t gas_id);
#endif /* FEATURE_SGLTE */

/**
  @brief Returns the Source RAT type for current X2G reselection procedure
  @param GAS ID
  @return rr_rat_e
*/
extern rr_rat_e rr_get_x2g_resel_source_rat(const gas_id_t gas_id);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/**
  @brief Determines if last service request or status indicates Limited Service

  @return TRUE if GRR has been advised of limited service by NAS
*/
extern boolean rr_limited_service_in_effect(const gas_id_t gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#endif /* RR_CONTROL_H */

