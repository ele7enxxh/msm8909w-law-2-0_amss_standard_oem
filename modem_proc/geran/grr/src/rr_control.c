/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Control Module

GENERAL DESCRIPTION
   This module contains the main state machine for Radio Resources (RR)

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2010-2014 Qualcomm Technologies, Incorporated. All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_control.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
Nov/07/12   SJW     CR419063: store trans_id of BPLMN failed due to guard timer
Oct/10/12   SJW     Do not reset the candidate DB if BPLMN times out in Ltd Svc
Sep/27/12   SJW     CR404279: Prioritise PLMN search to foreground during Ltd Svc
Jul/11/12   SJW     CR377942: do not prioritise automatic mode BPLMN searches
Jun/28/12   SJW     CR374381: further BPLMN search enhancement for DSDS mode
May/16/12   SJW     Force PLMN search to foreground if both stacks in forbidden LAI
Apr/13/12   SJW     Prioritise PLMN search to foreground if camped on forbidden LAI
Mar/29/12   SJW     Reject automatic PLMN searches whilst early camping
Jan/27/12   SJW     Treat MDL_ERROR_IND as DL_RELEASE_IND in W2G handover state
Jun/21/11   SJW     Declare OOS if PLMN search triggered after early camp failure
Apr/19/11   SJW     Clear pending establishment if early camping times out
Apr/08/11   SJW     Correction to AUTH_FAIL_IND timer unit conversion
Mar/28/11   SJW     New DSC handler for BPLMN power scan in Dual SIM mode
Jun/24/04    VT     Added support for FEATURE_GERAN_STORED_LIST_CELL_SELECTION
Jun/07/04   NJT     Added code for optimisation to force RPS after call release.
May/26/04   DMB     Added MPH_START_IDLE_MODE_CNF handling
May/20/04   DMB     Amend background PLMN list handling
05/07/04    gc      Enhanced trace buffer code
04/29/04    gc      CR42445: Process Serving Idle Meas. in EarlyCampWaitforSI
03/24/04    DMB     Replace GRR51 and GRR52 states with single RR_GRR state
03/22/04    gc      Added Trace Buffer Code
03/05/04    gc      CR31210: Fixes to W2G HO Failure
02/17/04    gc      Fixes for HO to Barred Cell
Feb/12/04   VT      QuadBand: In rr_control(), in RR_NO_CHANNELS state,
                      on receiving RR_ACT_REQ, added call to rr_set_supported_bands()
02/10/04    gc      Added stop_camping_req to L1 for Cell not suitable case
Jan/19/04   VT      QuadBand: In rr_control(), in RR_NO_CHANNELS state,
                      on receiving RR_ACT_REQ, added call to rr_set_allowed_bands()
22/10/03    guru    Added cell suitability check for the new cell after a handover.
08/03/03    he      Added detection of L1 service loss.
06/30/03    sbs     Added support for Location Services related APDU handling.
05/08/03    he      Featurized debug string logging (RR_DIAG_STR)
05/02/03    He      Removed clear pscan results from process stop request function.
02/19/03    Rao     Added GSM Deep Sleep support.
02/18/03    JAC     lint cleanup
02/07/03    he      Added RR_CELL_REEST state to support cell re-establishment.
02/14/03    RG      Added support for G2W blind Handover
01/22/03    he      Fixed lint errors.
01/14/03    NJT     Fixed reselection problem when going from GSM to GPRS.
01/10/02    rmc     Add dummy handler for WCDMA measurements in cell reselection state
01/09/02    JAC     Added rr_in_dedicated function
12/12/02    JAC     Added additional C2 timers
12/03/02    Rao     Added changes for camping on GPRS cell after W2G Reselection.
12/02/02    JAC     Transfer control to GRR on connection release if on GPRS serv cell
11/21/02    Rao     Incorporated G2W Reselection Code Review Comments.
11/18/02    RG      Added "case MPH_WCDMA_CELL_UPDATE_LIST_REQ" to the rr_message_name() routine.
11/13/02    Rao     Fix for deactivating L1 when DEACT_REQ is received.
11/08/02    he      Addional support for GPRS 2.0.
10/03/02    Rao     Added InterRAT Reselection ( GSM to WCDMA ) Support.
09/25/02    rmc     Fix compilation warnings
09/19/02    JAC     Added early power scan
08/30/02    Rao     Added InterRAT Reselection ( WCDMA to GSM ) Support.
08/26/02    rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
08/15/02    Rao     Fixed Inter RAT HO issues found in Unit test.
08/01/02    rmc     Cell Selection/Reselection Optimization Changes for Phase II
07/20/02    Rao     Added InterRAT ( WCDMA To GSM )  Handover support
07/31/02    rmc     Generate page received event when page addressing mobile is received
07/31/02    rmc     Genetare OOS event when DSF is detected
07/29/02    rmc     Adding reselection trigger storage for DSF/OOS event if it occurs
07/11/02    JAC     Added early camp support when collecting full SI for reselection
07/01/02    rmc     Handling of timers while in Inactive state (no message should be sent out for
                    signal strength timeout)
06/25/02    rmc     RR_ABORT_REQ should always result in an RR_ABORT_CNF (even for failure cases)
06/21/02    JAC     Moved save page support to rr_cell_reselection
06/19/02    JAC     Added save pages support, removed reselection reason
06/18/02    rmc     Adding support for RR_PLMN_LIST_REQ message (always in non-camped mode)
05/30/02    Rao     Getting rid of Path Loss timer & Added 5 Sec Surr & Serv timers.
06/03/02    rmc     Adding EV_NO_BCCH_FOUND event to WAIT_FOR_DATA state in case of
                    low priority cells not having sufficient SI information. Fixed lint warnings
05/23/02    rmc     Removed use of EV_GSM_MODE_ACT_FAIL and EV_GSM_MODE_DEACT_FAIL
05/22/02    rmc     Changes to MM-RR interface for dual mode
05/14/02    Rao     Disabling Cell Reestablishment after RLF.
05/09/02    ATM     Added calls to rr_log_x_counters
04/11/02    JAC     added saving of stored plmn db
03/12/02    rmc     Dual Mode Cell Selection changes
03/07/02    SB      Dual Mode Activation / Deactivation changes
02/28/02    ATM     Removed gs_gsm.h
02/07/02    JAC     Added handling of mph_select_specific_bcch_cnf during reselection
                     Added EV_GO_TO_IDLE_ABORT for when a reselection is aborted
02/01/02    RMC     XDM logging and state log packets
01/30/02    RMC     Cell Reselection Changes and general cleanup
12/18/01    RMC     Fixes for RR connection establishment handling
12/07/01    JAC     Modified rr_control to handle LAI_LIST_REQ and PLMN_REQ in
                     all states
11/14/01    RMC     Process RR_FORBIDDEN_LAI_UPDATE_REQ in state independent manner
12/11/00    TS      Initial Revision.

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"
#include "rr_seg_load.h"
#include "rr_control.h"
#include "rr_l2.h"
#include "gs.h"
#include "ms_timer.h"
#include "ms.h"
#include "rr_general.h"
#include "rr_l2_send.h"
#include "rr_mm_send.h"
#include "rr_conn_establish.h"
#include "rr_cell_selection.h"
#include "rr_shared_cell_selection.h"
#include "rr_sys_info.h"
#include "rr_conn_release.h"
#include "rr_procedure.h"
#include "cause.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "rr_task.h"
#include "rr_log.h"
#include "rr_mm_send.h"
#include "timers.h"
#include "rr_plmn_list.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "rr_mode.h"
#include "rr_rrc_send.h"
#include "rr_ded_meas_reporting.h"
#include "rr_inter_ho.h"
#include "rr_intra_ho.h"
#include "rr_g2w_handover.h"
#include "rr_w2g_service_redirection.h"
#include "rr_gprs_defs.h"
#include "mac_grr.h"
#include "rr_mac_send.h"
#include "rr_resel_calcs.h"
#include "rr_candidate_cell_db.h"
#include "rr_acq_db.h"
#include "rr_ded_meas_reporting.h"
#include "rr_grr.h"
#include "rr_gprs_debug.h"
#include "rr_gprs_main.h"
#include "rr_nv.h"
#include "rr_process_pmo_pcco.h"
#include "rr_resel_utils.h"
#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
#include "rr_gprs_edtm.h"
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
#ifdef FEATURE_CGPS_UMTS_CP_GSM
#include "rr_lsm_if.h"
#endif /* FEATURE_CGPS_UMTS_CP_GSM */
#include "rr_cell_reestablish.h"
#include "rr_pscan.h"
#include "rr_main.h"
#include "rr_resel.h"
#include "geran_eng_mode_info.h"
#ifdef FEATURE_GPRS_PS_HANDOVER
#include "rr_psho.h"
#endif /* FEATURE_GPRS_PS_HANDOVER */
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
#include "rr_callback.h"
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
#include "rr_sys_info_pcell.h"
#include "rr_sys_info_ncell.h"
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
#include "rr_x2g_resel.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */
#ifdef FEATURE_FEMTO_GSM_NL
#include "rr_femto.h"
#include "rr_femto_if.h"
#endif // FEATURE_FEMTO_GSM_NL
#include "rr_decode_ie.h"

#ifdef FEATURE_LTE
#include "rr_classmark.h"
#endif /* FEATURE_LTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
#include "rr_g2x.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/
#ifdef FEATURE_WCDMA
#include "rr_g2w.h"
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
#include "lte_rrc_ext_api.h"
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
#include "rr_tds_send.h"
#endif /*FEATURE_GSM_TDS*/
#include "rr_multi_sim.h"
#include "rr_gprs_dsc.h"
#include "rr_gprs_conn_establish.h"
#include "rr_init.h"
#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS_G2T_REDIRECTION)
#include "rr_g2x_redir.h"
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION*/
#include "rr_ded_config.h"
#include "rr_l1_send.h"
#include "rr_sim.h"
#ifdef FEATURE_LTE_TO_GSM_CGI
#include "rr_x2g_cgi.h"
#endif /* FEATURE_LTE_TO_GSM_CGI */
#include "rr_classmark.h"
#include "rr_cell_acq.h"
#include "rr_l1_idle_mode.h"
#include "rr_ps_access.h"
#include "rr_ps_domain.h"

#include "fs_lib.h"
#include "fs_sys_types.h"
#ifdef FEATURE_LTE
#include "lte_rrc_ext_msg.h"
#endif /* FEATURE_LTE */

#include "rr_msgr.h"
#include "rr_msc.h"
#include "trm.h"
#include "rr_gprs_meas_rpt_control.h"
#include "gprs_mem.h"
#include "stringl/stringl.h"
#include "rr_channel_mode.h"

#ifdef FEATURE_SGLTE
#include "rr_sglte.h"

#ifdef  FEATURE_SGLTE_G2X_CONN_MODE_RESEL
#include "rr_sglte_g2x_conn_resel.h"
#endif
#endif /* FEATURE_SGLTE */

#include "rr_x2g.h"
#include "rr_select_bcch.h"

#ifdef FEATURE_GSM_BAND_AVOIDANCE
#include "rr_band_avoid.h"
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
#ifdef FEATURE_CGPS_UMTS_CP_GSM
#include "rr_cgps.h"
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

#include "geran_nv.h"
#include "rr_sys_info_cache.h"

#ifdef FEATURE_IDLE_DRX_SCALING
#include "rr_idle_drx.h"
#endif /* FEATURE_IDLE_DRX_SCALING */

#ifdef FEATURE_QSH_EVENT_METRIC
#include "rr_qsh_metric.h"
#endif // FEATURE_QSH_EVENT_METRIC

#define dBm_x16_VALUE_FOR_RXLEV_0 ((dBx16_T)(-111*16))
#define dBm_x16_VALUE_FOR_RXLEV_63 ((dBx16_T)(-48*16)) 


/*****************************************************************************
***
***     External Functions & Variables
***
*****************************************************************************/
extern void rr_mm_send_message( IMH_T * p_message, gs_queue_id_T queue,
                                const gas_id_t gas_id);

#ifdef FEATURE_FAKE_BTS_CELL_BARRING
extern rr_event_T rr_handle_fake_gcell_action_req(rr_cmd_bdy_type* new_message_ptr, const gas_id_t gas_id);
#endif /*FEATURE_FAKE_BTS_CELL_BARRING*/


/*****************************************************************************
***
***     Local Typedefs, #defines & Variables
***
*****************************************************************************/
#ifdef FEATURE_LTE
/* Minimum duration for T304 during L2G CCO is set here as 2 seconds.  If
the network configures a timer for a value less than this, RR will modify
increase it to this value. */
#define RR_T304_MINIMUM_DURATION (2000)
#endif /* FEATURE_LTE */



typedef enum
{
  RR_DEACT_REASON_STOP_GSM_MODE_REQ,
  RR_DEACT_REASON_DEACT_REQ,
  RR_DEACT_REASON_LOWER_LAYER_FAILURE,
  RR_DEACT_REASON_STOP_CAMPING,
  RR_DEACT_REASON_STOP_CAMPING_THEN_RESTART,
  RR_DEACT_REASON_SWITCH_TO_OTHER_RAT
} rr_deact_reason_t;

/*data structure for saving messages*/
typedef struct
{
  rr_cmd_bdy_type msg;
  boolean valid;
} rr_saved_msg_type_T;


typedef struct
{
  boolean                      valid;
  sys_rat_pri_list_info_s_type rat_pri_list_info;
  sys_srv_domain_e_type        service_domain;
} rr_pending_mode_change_data_t;


typedef struct
{
  boolean              l1_reset;
  boolean              process_saved_msg;
  rr_saved_msg_type_T  saved_msg;

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  /* Indicates whether an X2G CCO is in progress.  Even though the X2G CCO
  request is saved in saved_msg in this structure, this is only valid until
  RR camps on the target cell.  At this point, the X2G CCO request saved in
  this structure is not reliable, as a subsequent RR_EST_REQ or other NAS
  command may overwrite it.  Instead, when camping is completed, we set this
  flag and check it explicitly on handling a panic reset in camped state. */
  rr_x2g_resel_type_e  x2g_cco_in_progress;

  /* This flag is used to determine the correct status to send WCDMA RRC if
  a panic reset occurs during a W2G CCO procedure. */
  boolean              cco_activation_ind_sent;

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
} rr_panic_reset_info_t;

#ifdef FEATURE_SGLTE
typedef enum
{
  RR_SGLTE_PLMN_SEARCH_ABORT_NONE,
  RR_SGLTE_PLMN_SEARCH_ABORT_MM_DEATIVATION,
  RR_SGLTE_PLMN_SEARCH_ABORT_EV_GSM_MODE_DEACT,
  RR_SGLTE_PLMN_SEARCH_ABORT_EV_SWITCH_TO_CS_ONLY,
  RR_SGLTE_PLMN_SEARCH_ABORT_EV_SWITCH_TO_USER_CS_ONLY,
  RR_SGLTE_PLMN_SEARCH_ABORT_EV_GUARD_TIMER_EXPIRY
} rr_sglte_g2x_plmn_search_abort_cause_e;

/*data structure for saving L2G messages*/
typedef struct
{
  rr_cmd_bdy_type msg;
  boolean valid;
} rr_saved_l2g_msg_type_T;


#endif /*FEATURE_SGLTE*/

typedef struct
{
  sys_band_mask_type    rr_invalid_band_pref; /* This is used to store an invalid band_pref as received in RR_PLMN_SELECT_REQ
                                                 so that it can be reported (MSG_ERROR) periodically. */
  rr_state_T            old_rr_state;
  rr_deact_reason_t     rr_deact_reason;
  rr_panic_reset_info_t rr_panic_reset_info;
  rr_saved_msg_type_T   rr_saved_msg;
  rr_state_T            rr_state;
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  rr_x2g_resel_type_e        x2g_resel_type;
  rr_x2g_resel_abort_cause_e x2g_resel_abort_cause;
  rr_cco_status_e            x2g_cco_status;
  boolean                    is_cco_for_csfb;
  boolean                    x2g_cco_pending_network_control_order_valid;
  uint8                      x2g_cco_pending_network_control_order;
  boolean                    x2g_cco_act_rsp_rcvd;
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

#ifdef FEATURE_GSM_TDS
  boolean                    waiting_for_tdsrr_interrat_tl1_stop_ind;
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
  boolean                    waiting_for_lte_rrc_cco_nacc_completed_ind;
#endif /* FEATURE_LTE */

  rr_pending_mode_change_data_t pending_mode_change_data;

  inter_task_BCCH_allocation_T  ba_sacch_copy;
  int                   bplmn_dsc_max;
  int                   bplmn_dsc_now;
  byte                  bplmn_fail_id;
#ifdef FEATURE_LTE
  /* TRUE if LTE is supported by the UE hardware, FALSE otherwise. */
  boolean lte_supported;

  /* TRUE if reselection to LTE is allowed, FALSE otherwise.  This may be set
  to FALSE if registration on LTE is rejected with reject cause 14 and is
  indicated in RR_LTE_RESELECTION_ALLOWED_STATUS_IND. */
  boolean g2l_resel_allowed;
#endif /* FEATURE_LTE */

  boolean wcdma_stop_mode_cnf_rxd;
#ifdef FEATURE_GSM_TDS
  boolean tds_stop_mode_cnf_rxd;
#endif /*FEATURE_GSM_TDS*/

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
  /* Used during W2G CCO */
  lte_irat_dedicated_priority_info_s dedicated_priority_info;
#endif /* FEATURE_WCDMA || FEATURE_LTE */

  /* Set from RR_CSFB_CALL_STATUS_IND from NAS. */
  boolean is_csfb_call_in_progress;
  boolean csmt;

#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING
  /* TRUE if the last cell selection procedure that RR attempted was not
  successful in finding service. */
  boolean ue_oos;
#endif /* FEATURE_GSM_TX_DIV_ANTNA_SWITCHING */

  struct
  {
    sys_srv_domain_e_type service_domain;
    sys_srv_status_e_type service_status;
  } nas_service_status;

#ifdef FEATURE_SGLTE
  /* Set from RR_UE_MODE_IND, indicates if GRR is in SGLTE mode. */
  sys_ue_mode_e_type ue_mode;

  /* Flags whether RR is waiting for MPH_UE_MODE_CHANGE_CNF during processing
  of RR_STOP_GSM_MODE_REQ(RR_SGLTE_RAT_CHANGE). */
  boolean            switch_to_cs_only_gsm_only_pending;

  /* Pending RAT Priority List received in X2G SGLTE IRAT request. */
  boolean                      sglte_pending_rat_pri_list_valid;
  sys_rat_pri_list_info_s_type sglte_pending_rat_pri_list;

  boolean            sglte_pending_gl1_mode_change;
  boolean            sglte_x2g_resel_in_progress;
  rr_rat_e           sglte_x2g_plmn_list_search_originator;
  boolean            sglte_x2g_plmn_list_search_in_progress;
  boolean            sglte_x2g_plmn_list_search_is_foreground;
  boolean            sglte_x2g_plmn_list_search_aborted_by_rrc;
  struct
  {
    boolean                         srch_done;
    sys_detailed_plmn_list_s_type * available_plmn_list;
    boolean                         hplmn_found;
    detected_frequency_list_type  * detected_frequency_list;
  } sglte_x2g_plmn_list_search_results;

  struct
  {
    ARFCN_T  serving_ARFCN;
    BSIC_T   serving_BSIC;
    int16  serving_rssi_dBm;
    boolean  serving_gprs_indicator;
  } sglte_store_serv_cell;

  boolean                                sglte_g2x_plmn_list_search_in_progress;
  rr_sglte_g2x_plmn_search_abort_cause_e sglte_g2x_plmn_list_search_abort_cause;
  boolean                                sglte_g2x_plmn_list_search_inhibit_tds_search;
  boolean                                sglte_g2x_plmn_list_search_inhibit_lte_search;
  sglte_l2g_resel_req_ded_pri_info_t     sglte_ded_pri_info;
  boolean                                sglte_perform_g2l_blind_redir;
  rr_saved_l2g_msg_type_T                rr_saved_l2g_msg;
  boolean                                sglte_processng_intrnl_stop_gsm;
  boolean                                sglte_processng_saved_l2g_ho_msg;
#endif /* FEATURE_SGLTE */

  boolean purge_gmm_signalling;
} rr_control_data_t;

typedef enum
{
  RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_UNDEFINED,
  RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_GO_TO_INACTIVE,
  RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_AUTO_RESTART,
  RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_RECOVER_FROM_PANIC_RESET,
#ifdef FEATURE_SGLTE
  RR_GSM_AS_SGLTE_INTERNAL_DEACTIVATION_COMPLETE_ACTION_PROCESS_L2G_HO_REQ,
#endif /*FEATURE_SGLTE*/
  RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_MAX
} rr_gsm_as_deactivation_complete_action_e;

#define _RR_CONTROL_DATA_INIT { SYS_BAND_MASK_EMPTY, /*rr_invalid_band_pref*/ \
                                RR_INACTIVE          /*old_rr_state*/ \
                              }


static rr_control_data_t  rr_control_data[NUM_GERAN_DATA_SPACES] =
                                        { INITIAL_VALUE(_RR_CONTROL_DATA_INIT) };
#undef _RR_CONTROL_DATA_INIT
/*****************************************************************************
***
***     Function Prototypes
***
*****************************************************************************/

static void rr_retry_msg_after_panic_reset_restart(const gas_id_t gas_id);
static rr_event_T rr_process_panic_reset_saved_msg(rr_cmd_bdy_type **message_ptr_ptr, const gas_id_t gas_id);
static rr_gsm_as_deactivation_complete_action_e rr_gsm_as_deactivation_complete(const gas_id_t gas_id);
static void set_rr_deact_reason(rr_deact_reason_t reason, const gas_id_t gas_id);
static rr_deact_reason_t get_rr_deact_reason(const gas_id_t gas_id);
static rr_event_T rr_process_rr_stop_gsm_mode_req(boolean rr_deact_gsm, const gas_id_t gas_id);
static void rr_handle_nw_sel_mode_reset_ind(const rr_nw_sel_mode_reset_ind_T * p_nw_sel_mode, const gas_id_t gas_id);

#ifdef FEATURE_SGLTE
static boolean rr_start_sglte_plmn_list_search(rr_plmn_list_req_T * plmn_list_req_ptr, const gas_id_t gas_id);
static void rr_sglte_set_lte_blind_redir_in_data_transfer(const gas_id_t gas_id);
#endif /* FEATURE_SGLTE */

/*!
 * \brief Returns a pointer to the module data.
 *
 * \param gas_id
 *
 * \return rr_control_data_t*
 */
static rr_control_data_t *rr_control_get_data_ptr(
  const gas_id_t gas_id
)
{
  uint8 as_index = GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id);

  return &rr_control_data[as_index];
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/**
 * Initialise Downlink Signalling Counter used for BPLMN in DSDS mode
 *
 * @param  VOID
 *
 * @return VOID
 *
 */
static void rr_ms_bplmn_initialise_dsc(
  const boolean for_new_search,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (for_new_search == TRUE)
  {
    /* obtain the BPLMN-specific DSC counters from the serving cell info */
    rr_control_data_ptr->bplmn_dsc_max = rr_gprs_get_max_dsc_value(FALSE, gas_id);
    rr_control_data_ptr->bplmn_dsc_now = rr_control_data_ptr->bplmn_dsc_max;

    /* print state information and the current idle mode DSC value */
    MSG_GERAN_HIGH_3_G(
      "BPLMN SHM=%d camped=%d IDLE DSC=%d",
      rr_control_data_ptr->bplmn_dsc_now,
      (int)rr_is_camped(gas_id),
      rr_gprs_return_gprs_dsc_counter_value(gas_id)
    );
  }
  else
  {
    /* set the BPLMN-specific counters back to a default value */
    rr_control_data_ptr->bplmn_dsc_max = (GPRS_DSC_DIVIDEND >> 1);
    rr_control_data_ptr->bplmn_dsc_now = (GPRS_DSC_DIVIDEND >> 1);
  }

  return;
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */


#ifdef FEATURE_SGLTE


boolean rr_is_pending_mode_change(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);

  MSG_GERAN_HIGH_1_G("rr_is_pending_mode_change Is Mode change pending = %d", 
    rr_control_data_ptr->sglte_pending_gl1_mode_change  );

  return( rr_control_data_ptr->sglte_pending_gl1_mode_change );

}


void rr_set_pending_mode_change(boolean mode_change_status, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);

  MSG_GERAN_HIGH_2_G("rr_set_pending_mode_change Current Status= %d, New status=: %d", 
    rr_control_data_ptr->sglte_pending_gl1_mode_change, mode_change_status  );

  rr_control_data_ptr->sglte_pending_gl1_mode_change = mode_change_status;
  
}

#endif /* FEATURE_SGLTE */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

static rr_rat_e rr_map_x2g_resel_type_to_rat(
  rr_x2g_resel_type_e x2g_resel_type
)
{
  switch (x2g_resel_type)
  {
#ifdef FEATURE_WCDMA
    case RR_X2G_RESEL_TYPE_W2G_RESELECTION:
    case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
    {
      return RR_RAT_WCDMA;
    } /* RR_X2G_RESEL_TYPE_W2G_RESELECTION ... */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
    case RR_X2G_RESEL_TYPE_L2G_RESELECTION:
    case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
    case RR_X2G_RESEL_TYPE_L2G_SGLTE_REDIRECTION:
    {
      return RR_RAT_LTE;
    } /* RR_X2G_RESEL_TYPE_L2G_RESELECTION ... */
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
    case RR_X2G_RESEL_TYPE_T2G_RESELECTION:
    case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
    case RR_X2G_RESEL_TYPE_T2G_SGLTE_REDIRECTION:
    {
      return RR_RAT_UTRAN_TDD;
    } /* RR_X2G_RESEL_TYPE_T2G_RESELECTION ... */
#endif /*FEATURE_GSM_TDS*/

    default:
    {
      MSG_GERAN_ERROR_1("Unexpected x2g_resel_type: %d", x2g_resel_type);
      return RR_RAT_UNDEFINED;
    }
  }
}

static rr_x2g_resel_type_e rr_get_panic_reset_x2g_cco_flag(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_HIGH_1_G(
    "rr_get_panic_reset_x2g_cco_flag: %d",
    rr_control_data_ptr->rr_panic_reset_info.x2g_cco_in_progress
  );

  return rr_control_data_ptr->rr_panic_reset_info.x2g_cco_in_progress;
}

static void rr_set_panic_reset_x2g_cco_flag(
  rr_x2g_resel_type_e x2g_cco_in_progress,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_HIGH_1_G(
    "rr_set_panic_reset_x2g_cco_flag: %d",
    x2g_cco_in_progress
  );

  rr_control_data_ptr->rr_panic_reset_info.x2g_cco_in_progress = x2g_cco_in_progress;
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
static tdsrr_interrat_reselect_fail_reason_type rr_t2g_reselect_failure_reason_map(
  rr_x2g_resel_status_e resel_status
)
{
  switch (resel_status)
  {
    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BARRED:
      return TDSRR_GSM_CELL_BARRED;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_LOW_C1:
      return TDSRR_GSM_CELL_LOW_C1;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_PLMN_MISMATCH:
      return TDSRR_GSM_PLMN_MISMATCH;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_FORBIDDEN:
      return TDSRR_GSM_CELL_FORBIDDEN;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BSIC_MISMATCH:
      return TDSRR_GSM_BSIC_NOT_FOUND;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_NOT_FOUND:
      return TDSRR_GSM_BCCH_NOT_FOUND;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_DECODE_FAILED:
      return TDSRR_GSM_BCCH_DECODE_FAIL;

    default:
      MSG_GERAN_ERROR_1("Unexpected resel_status: %d", resel_status);
      return TDSRR_GSM_OTHER_REASON;
  }
}

static uint16 rr_t2g_reselect_failure_time_to_reselect_map(
  rr_x2g_resel_status_e resel_status
)
{
  switch (resel_status)
  {
    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BARRED:
      return RR_T2GR_GSM_CELL_BARRED_TIMEOUT;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_LOW_C1:
      return RR_T2GR_GSM_CELL_LOW_C1_TIMEOUT;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_PLMN_MISMATCH:
      return RR_T2GR_GSM_CELL_PLMN_MISMATCH_TIMEOUT;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_FORBIDDEN:
      return RR_T2GR_GSM_CELL_FORBIDDEN_TIMEOUT;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BSIC_MISMATCH:
      return RR_T2GR_GSM_BSIC_NOTFOUND_TIMEOUT;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_NOT_FOUND:
      return RR_T2GR_BCCH_NOTFOUND_TIMEOUT;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_DECODE_FAILED:
      return RR_T2GR_BCCH_DECODE_FAIL_TIMEOUT;

    default:
      MSG_GERAN_ERROR_1("Unexpected resel_status: %d", resel_status);
      return 0xffff;
  }
}

#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA

static rr_interrat_reselect_fail_reason_type rr_w2g_reselect_failure_reason_map(
  rr_x2g_resel_status_e resel_status
)
{
  switch (resel_status)
  {
    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BARRED:
      return RR_GSM_CELL_BARRED;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_LOW_C1:
      return RR_GSM_CELL_LOW_C1;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_PLMN_MISMATCH:
      return RR_GSM_PLMN_MISMATCH;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_FORBIDDEN:
      return RR_GSM_CELL_FORBIDDEN;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BSIC_MISMATCH:
      return RR_GSM_BSIC_NOT_FOUND;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_NOT_FOUND:
      return RR_GSM_BCCH_NOT_FOUND;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_DECODE_FAILED:
      return RR_GSM_BCCH_DECODE_FAIL;

    default:
      MSG_GERAN_ERROR_1("Unexpected resel_status: %d", resel_status);
      return RR_GSM_OTHER_REASON;
  }
}

static uint16 rr_w2g_reselect_failure_time_to_reselect_map(
  rr_x2g_resel_status_e resel_status
)
{
  switch (resel_status)
  {
    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BARRED:
      return RR_W2GR_GSM_CELL_BARRED_TIMEOUT;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_LOW_C1:
      return RR_W2GR_GSM_CELL_LOW_C1_TIMEOUT;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_PLMN_MISMATCH:
      return RR_W2GR_GSM_CELL_PLMN_MISMATCH_TIMEOUT;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_FORBIDDEN:
      return RR_W2GR_GSM_CELL_FORBIDDEN_TIMEOUT;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BSIC_MISMATCH:
      return RR_W2GR_GSM_BSIC_NOTFOUND_TIMEOUT;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_NOT_FOUND:
      return RR_W2GR_BCCH_NOTFOUND_TIMEOUT;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_DECODE_FAILED:
      return RR_W2GR_BCCH_DECODE_FAIL_TIMEOUT;

    default:
      MSG_GERAN_ERROR_1("Unexpected resel_status: %d", resel_status);
      return 0xffff;
  }
}

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE

static geran_grr_lte_resel_failure_cause_type_e rr_l2g_resel_failure_cause_map(
  rr_x2g_resel_status_e resel_status
)
{
  switch (resel_status)
  {
    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BARRED:
      return GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_CELL_BARRED;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_LOW_C1:
      return GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_LOW_C1;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_PLMN_MISMATCH:
      return GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_PLMN_MISMATCH;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_FORBIDDEN:
      return GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_FORBIDDEN_LAI;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BSIC_MISMATCH:
      return GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_BSIC_MISMATCH;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_NOT_FOUND:
      return GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_BCCH_NOT_FOUND;

    case RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_DECODE_FAILED:
      return GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_BCCH_DECODE_FAILED;

    default:
      return GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_UNDEFINED;
  }
}

#endif /* FEATURE_LTE */


/**
  @brief Stores the contents of RR_CHANGE_MODE_IND to be used on return to
         idle mode.

  @param change_mode_ind Pointer to RR_CHANGE_MODE_IND signal from MM.
*/

void rr_pending_mode_change_data_store(
  rr_change_mode_ind_T * change_mode_ind,
  const gas_id_t gas_id
)
{
  rr_control_data_t *rr_control_data_ptr;

  RR_NULL_CHECK_RETURN_VOID(change_mode_ind);

  // Obtain a pointer to the module data
  rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_HIGH_2_G("Storing pending GSM-only / band pref changes (%d, 0x%08x)",
    rr_rat_pri_list_info_gsm_only(&change_mode_ind->rat_pri_list_info),
    RR_GET_BAND_CAP_CHGWT(rr_rat_pri_list_info_band_pref(
      &change_mode_ind->rat_pri_list_info,
      SYS_SYS_MODE_GSM))
  );

  rr_control_data_ptr->pending_mode_change_data.valid = TRUE;
  rr_control_data_ptr->pending_mode_change_data.rat_pri_list_info = change_mode_ind->rat_pri_list_info;
  rr_control_data_ptr->pending_mode_change_data.service_domain = change_mode_ind->service_domain;
}

/**
  @brief Deletes any stored pending mode change data.
*/
void rr_pending_mode_change_data_clear(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_HIGH_0_G("Clearing pending GSM-only / band pref changes");
  memset(&rr_control_data_ptr->pending_mode_change_data, 0, sizeof(rr_pending_mode_change_data_t));
}

/**
  @brief Copies pending change mode data to PLMN Request Details store.
*/
void rr_pending_mode_change_data_action(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_control_data_ptr->pending_mode_change_data.valid)
  {
    rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
#ifdef FEATURE_WCDMA
    boolean update_wcdma_rrc_band_pref = FALSE;
#endif /* FEATURE_WCDMA */
    boolean update_geran_l1_mode = FALSE;
    boolean gsm_only = rr_rat_pri_list_info_gsm_only(&rr_control_data_ptr->pending_mode_change_data.rat_pri_list_info);

    MSG_GERAN_HIGH_2_G("Copying pending GSM-only / band pref changes (%d, 0x%08x)",
      gsm_only, 
      RR_GET_BAND_CAP_CHGWT(rr_rat_pri_list_info_band_pref(
        &rr_control_data_ptr->pending_mode_change_data.rat_pri_list_info,
        SYS_SYS_MODE_GSM))
    );

#ifdef FEATURE_WCDMA
    /* Update RRC with band pref regardless of whether it has changed or not. */
    update_wcdma_rrc_band_pref = TRUE;
#endif /* FEATURE_WCDMA */

    if (rr_gsm_only(gas_id) != gsm_only)
    {
      update_geran_l1_mode = TRUE;
    }
#ifdef FEATURE_SGLTE
    if (rr_is_sglte_mode_active(gas_id))
    {
      rr_store_service_domain(rr_control_data_ptr->pending_mode_change_data.service_domain, gas_id );
    }
#endif /*FEATURE_SGLTE*/

    rr_PLMN_request_details_ptr->rat_pri_list_info =
      rr_control_data_ptr->pending_mode_change_data.rat_pri_list_info;

#ifdef FEATURE_WCDMA
    if (update_wcdma_rrc_band_pref)
    {
      rr_update_band_preference_for_rrc(rr_band_pref_wcdma(gas_id), gas_id);
#ifdef FEATURE_SGLTE
      if (rr_is_sglte_mode_active(gas_id) && rr_gsm_only(gas_id))
      {
        MSG_GERAN_HIGH_0_G("SGLTE: SGLTE active, GSM-only, inhibit TDS band pref update");
      }
      else
#endif /* FEATURE_SGLTE */
      {
        rr_update_band_preference_for_tdsrrc(rr_band_pref_utran_tdd(gas_id), gas_id);
      }
    }
#endif /* FEATURE_WCDMA */

    if (update_geran_l1_mode)
    {
#ifdef FEATURE_SGLTE
      if (rr_is_sglte_mode_active(gas_id))
      {
        rr_set_pending_mode_change(TRUE, gas_id);    
      }
#endif /* FEATURE_SGLTE */
      rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id), gas_id);
      rr_g2wr_reset_wcdma_surrounding_database(gas_id);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
      rr_g2wr_cancel_all_wcdma_five_second_timers(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
    }
  }
  else
  {
    MSG_GERAN_HIGH_0_G("No pending GSM-only / band pref changes");
  }
}


/**
  @brief Checks whether a band_pref is valid or not.

  This function checks whether the lower layers (RR/L1/RF drivers) support
  the bands specified in the given band pref.

  @param band_pref Bitmask of bands.
  @return TRUE if all GSM bands set in the bitmask are supported, FALSE otherwise.
*/
static boolean rr_validate_band_pref(
  sys_band_mask_type band_pref
)
{
  /* Modify band_pref such that it only contains GSM bands. */
  band_pref &= RR_SYS_BAND_MASK_GSM_ALL_BANDS;

  /* Remove all of the bands that RR actually supports from band_pref */
  CLEAR_BIT(RR_SYS_BAND_MASK_GSM_SUPPORTED_BANDS, band_pref);

  /* Now if band_pref has any bits set (i.e. band_pref != 0) then it must
  originally have contained some unsupported bands, so report this to our
  caller. */
  if (band_pref)
  {
    MSG_GERAN_HIGH_1(
      "band_pref includes invalid/unsupported GSM bands (invalid band bitmask: 0x%08x)",
      band_pref
    );

    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/**
  @brief Print out some errors messages indicating that an invalid band_pref
         has been received.

  This function is intended to be called on receipt of an RR_PLMN_SELECT_REQ
  that contains an invalid band_pref (i.e. that includes bands not supported
  by the lower layers.  It will print some error messages to diag indicating
  appropriate action to be taken, and additionally will start the
  RR_INVALID_BAND_PREF_TIMER.

  This function should also be called on expiry of that timer, such that the
  error messages will be displayed repeatedly.

  The global static variable rr_invalid_band_pref is accessed to allow the
  invalid band pref to be displayed in the diag messages.
*/
static void rr_report_invalid_band_pref_received(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_ERROR_1_G(
    "RR band_pref (0x%08x) includes unsupported (RR/L1/RF) bands",
    rr_control_data_ptr->rr_invalid_band_pref
  );

  MSG_GERAN_ERROR_0_G(
    "Check NV item 1877 - update of automation script may be required"
  );

  (void)rr_timer_start(RR_INVALID_BAND_PREF_TIMER, RR_INVALID_BAND_PREF_TIMEOUT, gas_id);
}

/* Initialise this module - called when RR task first runs */
void rr_control_init(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  rr_control_data_ptr->rr_state = RR_INACTIVE;
  rr_control_data_ptr->rr_deact_reason = RR_DEACT_REASON_STOP_GSM_MODE_REQ;
  rr_control_data_ptr->rr_saved_msg.valid = FALSE;

  /* Reset the lower layer failure information */
  rr_control_data_ptr->rr_panic_reset_info.l1_reset = FALSE;
  rr_control_data_ptr->rr_panic_reset_info.process_saved_msg = FALSE;
  rr_control_data_ptr->rr_panic_reset_info.saved_msg.valid = FALSE;

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  rr_control_data_ptr->rr_panic_reset_info.x2g_cco_in_progress = RR_X2G_RESEL_TYPE_UNDEFINED;
  rr_control_data_ptr->rr_panic_reset_info.cco_activation_ind_sent = FALSE;
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

  rr_pending_mode_change_data_clear(gas_id);

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
  rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
  rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
  rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
  rr_control_data_ptr->is_cco_for_csfb = FALSE;
  rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
  rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;
  rr_control_data_ptr->x2g_cco_act_rsp_rcvd = FALSE;
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

#ifdef FEATURE_GSM_TDS
  rr_control_data_ptr->waiting_for_tdsrr_interrat_tl1_stop_ind = FALSE;
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
  rr_control_data_ptr->waiting_for_lte_rrc_cco_nacc_completed_ind = FALSE;
#endif /* FEATURE_LTE */

  memset(&rr_control_data_ptr->ba_sacch_copy, 0, sizeof(inter_task_BCCH_allocation_T));

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  /* set the BPLMN DSC variables to defaults */
  rr_ms_bplmn_initialise_dsc(FALSE, gas_id);
#else
  rr_control_data_ptr->bplmn_dsc_max = 0;
  rr_control_data_ptr->bplmn_dsc_now = 0;
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  rr_control_data_ptr->bplmn_fail_id = BPLMN_INVALID_TRANS_ID;

  /* explicitly initialise the default service status variables */
  rr_control_data_ptr->nas_service_status.service_domain = SYS_SRV_DOMAIN_NO_SRV;
  rr_control_data_ptr->nas_service_status.service_status = SYS_SRV_STATUS_NO_SRV;

#ifdef FEATURE_LTE
  if (mcs_modem_has_capability(MCS_MODEM_CAPABILITY_FEATURE_LTE) == MCS_MODEM_CAP_AVAILABLE)
  {
    rr_control_data_ptr->lte_supported = TRUE;
  }
  else
  {
    rr_control_data_ptr->lte_supported = FALSE;
  }
  rr_control_data_ptr->g2l_resel_allowed = TRUE;
#endif /* FEATURE_LTE */

  return;
} /* end rr_control_init() */

static void rr_control_recovery_init(
  const gas_id_t gas_id
)
{
  // TODO: The following sequence is taken from rr_leave_idle_mode{NOT_CAMPED} which was
  // previously called following Panic Reset
  // Need to establish what actually needs to be reset following Recovery Restart

  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  RR_NULL_CHECK_FATAL(scell_info_ptr);

  scell_info_ptr->gsm.camped_on_flag = FALSE;

  /* reset the surrounding database */
  rr_reset_surrounding_database(gas_id);

#ifdef FEATURE_WCDMA
  rr_g2wr_reset_wcdma_surrounding_database(gas_id);
#endif /* FEATURE_WCDMA */

  rr_cancel_all_five_second_timers(gas_id);

  (void) rr_selection_control(EV_RESET_SOFTWARE, NULL, gas_id);
  (void) rr_handle_procedures(EV_RESET_SOFTWARE, NULL, gas_id);
  (void) rr_inter_cell_handover_control(EV_RESET_SOFTWARE, NULL, gas_id);

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
  rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
  rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
  rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
  rr_control_data_ptr->is_cco_for_csfb = FALSE;
  rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
  rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;

  /* Reset the IRAT state machines. */
  (void) rr_x2g_resel_deinit(gas_id);
  (void) rr_w2g_service_redirection_control(EV_RESET_SOFTWARE, NULL, gas_id);
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

#ifdef FEATURE_GSM_TDS
  rr_control_data_ptr->waiting_for_tdsrr_interrat_tl1_stop_ind = FALSE;
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
  rr_control_data_ptr->waiting_for_lte_rrc_cco_nacc_completed_ind = FALSE;
#endif /* FEATURE_LTE */

  rr_gprs_main_init(gas_id);

  rr_mac_send_reset_last_cell_info(gas_id);

  rr_cell_acq_control(EV_RESET_SOFTWARE, NULL, gas_id);
#ifdef FEATURE_IDLE_DRX_SCALING
  rr_idle_drx_control(EV_RESET_SOFTWARE, NULL, gas_id);
#endif /* FEATURE_IDLE_DRX_SCALING */

  if (rr_control_data_ptr->rr_saved_msg.valid)
  {
    MSG_GERAN_HIGH_2_G("Discarding saved message (%d, %d)",
      rr_control_data_ptr->rr_saved_msg.msg.message_header.message_set,
      rr_control_data_ptr->rr_saved_msg.msg.message_header.message_id
    );

    rr_control_data_ptr->rr_saved_msg.valid = FALSE;
  }

} /* end rr_control_recovery_init() */


void rr_panic_reset_triggered_by_l1(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  rr_control_data_ptr->rr_panic_reset_info.l1_reset = TRUE;

} /* end rr_panic_reset_triggered_by_l1() */


static rr_event_T rr_trigger_gsm_as_deactivation(
  rr_deact_reason_t deact_reason,
  const gas_id_t gas_id
)
{
  set_rr_deact_reason(deact_reason, gas_id);

  return rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);

} /* end rr_trigger_gsm_as_deactivation() */

static boolean rr_wcdma_stop_cnf_required(const gas_id_t gas_id)
{
#ifdef FEATURE_WCDMA
  if (!geran_is_multi_mode_gas_id(gas_id))
  {
    /* this subscription has no multi-mode support */
    return(FALSE);
  }

  /* default is based on whether the other RAT is loaded */
  return(rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA));
#else
  return(FALSE);
#endif /* FEATURE_WCDMA */
}

static boolean rr_tds_stop_cnf_required(const gas_id_t gas_id)
{
#ifdef FEATURE_GSM_TDS
  if (!geran_is_multi_mode_gas_id(gas_id))
  {
    /* this subscription has no multi-mode support */
    return(FALSE);
  }

  /* default is based on whether the other RAT is loaded */
  return(rr_is_rat_segment_loaded(RR_SEG_LOAD_TDSCDMA));
#else
  return(FALSE);
#endif /* FEATURE_GSM_TDS */
}

/**
  @brief Determine whether another RAT needs to be signalled for deactivation
*/
static boolean rr_other_rat_deactivation_required(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  boolean            wcdma_stop_required;
  boolean            tds_stop_required;

  /* validate pointer to the module data */
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  /* determine whether each of the other RATs needs to be signalled */
  wcdma_stop_required = rr_wcdma_stop_cnf_required(gas_id);
  tds_stop_required   = rr_tds_stop_cnf_required(gas_id);

#ifdef FEATURE_GSM_TDS
  /* perform explicit actions for TDSCDMA */
  if (tds_stop_required)
  {
    MSG_GERAN_HIGH_0_G("Stopping TDS-RRC before sending RR_DEACT_CNF");
    rr_control_data_ptr->tds_stop_mode_cnf_rxd = FALSE;

    rr_send_rrc_stop_tds_mode_req(TDSRRC_POWER_OFF, gas_id);
  }
  else
  {
    MSG_GERAN_HIGH_0_G("TDS-RRC stop NOT required");
    rr_control_data_ptr->tds_stop_mode_cnf_rxd = TRUE;
  }
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
  /* perform explicit actions for WCDMA */
  if (wcdma_stop_required)
  {
    MSG_GERAN_HIGH_0_G("Stopping W-RRC before sending RR_DEACT_CNF");
    rr_control_data_ptr->wcdma_stop_mode_cnf_rxd = FALSE;

    rr_send_rrc_stop_wcdma_mode_req(POWER_OFF, gas_id);
  }
  else
  {
    MSG_GERAN_HIGH_0_G("WCDMA-RRC stop NOT required");
    rr_control_data_ptr->wcdma_stop_mode_cnf_rxd = TRUE;
  }
#endif /* FEATURE_WCDMA */

  return(wcdma_stop_required || tds_stop_required);
}

static rr_gsm_as_deactivation_complete_action_e rr_gsm_as_deactivation_complete(
  const gas_id_t gas_id
)
{
  rr_gsm_as_deactivation_complete_action_e   action = RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_GO_TO_INACTIVE;
  rr_deact_reason_t                          deact_reason = get_rr_deact_reason(gas_id);
  gprs_scell_info_t                        * scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_HIGH_1_G("RR - GSM Mode Deactivated, reason=%d", deact_reason);

  /* Inform CB task that CBCH is no longer valid */
  rr_send_cell_change_ind(
    scell_info_ptr->gsm.location_area_identification,
    scell_info_ptr->gsm.cell_identity,
    FALSE,
    gas_id
  );

  rr_send_cb_info(FALSE, gas_id);

#ifdef FEATURE_SGLTE
  /*ensure flag is reset when going INACTIVE*/
  rr_set_sglte_perform_g2l_blind_redir(FALSE, gas_id);
  rr_set_pending_mode_change(FALSE, gas_id);
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
  rr_set_rr_using_pseudo_si2q_data(FALSE, gas_id);
#endif
  /*Calling rr_is_camped() will alloc some heap if not already allocated. 
    So this needs to be called before rr_init_modem_offline() is called,
    which frees all the allocated memory*/
  if (rr_is_camped(gas_id))
  {
    MSG_GERAN_HIGH_0_G("RR camped-on flag is still set, but L1 is deactivated, unsetting!");
    scell_info_ptr->gsm.camped_on_flag = FALSE;
  }
  switch ( deact_reason )
  {
    case RR_DEACT_REASON_DEACT_REQ:
    {
      /* Perform initialisation */
      rr_ps_no_ps_access_del_all(gas_id);
      rr_init_modem_offline(gas_id);

      rr_acq_db_save(gas_id);

      if (FALSE == rr_other_rat_deactivation_required(gas_id))
      {
        /* Confirm to MM immediately */
        rr_send_deact_cnf(gas_id);
      }
      break;
    }

    case RR_DEACT_REASON_STOP_GSM_MODE_REQ:
    {
      rr_init_geran_stopped(gas_id);
#ifdef FEATURE_SGLTE
      if (rr_sglte_internal_stop_gsm_being_processed(gas_id) &&
          rr_saved_l2g_msg_present(gas_id))
      {
        /* If we have a saved L2G HO message then RR would have internally shut down so do not send confirm to NAS */
        action = RR_GSM_AS_SGLTE_INTERNAL_DEACTIVATION_COMPLETE_ACTION_PROCESS_L2G_HO_REQ;
        MSG_GERAN_HIGH_0_G("SGLTE: SRVCC Internal deactivation completed");
        rr_control_data_ptr->sglte_processng_intrnl_stop_gsm = FALSE;
      }
      else
#endif /* FEATURE_SGLTE */
      {
        /* Confirm to MM */
      rr_send_stop_gsm_mode_cnf(MM_AS_SUCCESS, gas_id);
      }
      break;
    }

    case RR_DEACT_REASON_LOWER_LAYER_FAILURE:
    {
      // Free any allocated memory and re-initialise modules [ready for cell selection]
      rr_init_recovery_init(gas_id);

      // Any message received but not yet processed before the Recovery Restart event occur is loaded
      // for proessing
      rr_retry_msg_after_panic_reset_restart(gas_id);
      action = RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_RECOVER_FROM_PANIC_RESET;

      break;
    }

    case RR_DEACT_REASON_STOP_CAMPING_THEN_RESTART:
    {
      // TODO: Need to work out what needs to be reset and how this should be done
      rr_grr_control_init(gas_id);
      (void) rr_resel_control(EV_RESET_SOFTWARE, NULL, gas_id);
      rr_cell_acq_control(EV_RESET_SOFTWARE, NULL, gas_id);
      (void) rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, NULL, gas_id);
      (void) rr_gprs_sys_info_acquisition_control( EV_RESET_SOFTWARE, NULL, gas_id);
      (void) rr_plmn_list_control( EV_RESET_SOFTWARE, NULL, gas_id);
      action = RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_AUTO_RESTART;
      break;
    }

    case RR_DEACT_REASON_STOP_CAMPING:
    {
      // TODO: Need to work out what needs to be reset and how this should be done
      rr_grr_control_init(gas_id);
      (void) rr_resel_control(EV_RESET_SOFTWARE, NULL, gas_id);
      rr_cell_acq_control(EV_RESET_SOFTWARE, NULL, gas_id);
      (void) rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, NULL, gas_id);
      (void) rr_gprs_sys_info_acquisition_control( EV_RESET_SOFTWARE, NULL, gas_id);
      (void) rr_plmn_list_control( EV_RESET_SOFTWARE, NULL, gas_id);
      break;
    }

    case RR_DEACT_REASON_SWITCH_TO_OTHER_RAT:
    {
      /* have to reset the GPRS SI aquisition control state */
      rr_gprs_reset_software(gas_id);

      /* No longer camped */
      scell_info_ptr->gsm.camped_on_flag = FALSE;
      scell_info_ptr->gsm.BCCH_ARFCN.num = INVALID_ARFCN;
      scell_info_ptr->gsm.valid_data_flag = FALSE;
      scell_info_ptr->gsm.camped_on_flag = FALSE;

      rr_initialize_SI2q_instances(gas_id);
      rr_initialize_SI2ter_instances(gas_id);
    
      break;
    }

    default:
      MSG_GERAN_ERROR_1_G("Unknown deactivation reason %d",deact_reason);
  }

  /*As GRR will start from INACTIVE state so stop all the sys infor timers.*/
  rr_gprs_deactivate_timed_sys_info_refresh(gas_id);
  rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);
#ifdef FEATURE_LTE
 /*Clear the call_aborted flag if set to TRUE*/
  rr_connection_release_reset_call_aborted_flag_during_csfb(gas_id);
#endif /* FEATURE_LTE */
  rr_ce_init_access_reason(gas_id);

  if (action != RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_AUTO_RESTART &&
      action != RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_RECOVER_FROM_PANIC_RESET)
  {
    /* clear the saved recovery message if doesn't need auto restart */
    MSG_GERAN_HIGH_3_G("clean panic reset recovery message. valid:%d, msg_set:%d, msg_id:%d",
      rr_control_data_ptr->rr_panic_reset_info.saved_msg.valid,
      rr_control_data_ptr->rr_panic_reset_info.saved_msg.msg.message_header.message_set,
      rr_control_data_ptr->rr_panic_reset_info.saved_msg.msg.message_header.message_id
    );

    rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);
  }

#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  rr_init_cell_change_cb_data(gas_id);
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */

  {
    eng_mode_scell_info_t scell_info;
    eng_mode_nmr_info_t eng_mode_nmr;
    eng_mode_scell_measurements_t scell_meas;
    eng_mode_nw_params_t nw_params;
    eng_mode_scell_config_t scell_config;

    RR_ZERO_STRUCTURE(scell_info);
    RR_ZERO_STRUCTURE(eng_mode_nmr);
    RR_ZERO_STRUCTURE(scell_meas);
    RR_ZERO_STRUCTURE(nw_params);
    RR_ZERO_STRUCTURE(scell_config);

    geran_eng_mode_data_write(
      ENG_MODE_SCELL_INFO,   // geran_eng_mode_info_type
      (void*)&scell_info,    // param_copy
      FALSE,                 // valid_flag
      gas_id                 // gas_id
    );
    geran_eng_mode_data_write(
      ENG_MODE_NMR_INFO,      // geran_eng_mode_info_type
      (void*)&eng_mode_nmr,   // param_copy
      FALSE,                  // valid_flag
      gas_id                  // gas_id
    );
    geran_eng_mode_data_write(
      ENG_MODE_SCELL_MEASUREMENTS,   // geran_eng_mode_info_type
      (void*)&scell_meas,            // param_copy
      FALSE,                         // valid_flag
      gas_id                         // gas_id
    );
    geran_eng_mode_data_write(
      ENG_MODE_NW_PARAMS,   // geran_eng_mode_info_type
      (void*)&nw_params,    // param_copy
      FALSE,                // valid_flag
      gas_id                // gas_id
    );
    geran_eng_mode_data_write(
      ENG_MODE_SCELL_CONFIG,   // geran_eng_mode_info_type
      (void*)&scell_config,    // param_copy
      FALSE,                   // valid_flag
      gas_id                   // gas_id
    );
  }

#ifdef FEATURE_SGLTE
#ifdef FEATURE_GSM_TDS
  rr_sglte_set_l1_tds_measurements_configured(FALSE, gas_id);
#endif
#endif

#ifdef FEATURE_LTE
  rr_set_lte_l1_measurements_configured(FALSE, gas_id);
#endif

  return action;

} /* rr_gsm_as_deactivation_complete() */


/**
 *
 * @return TRUE if RR is in a dedicated state, FALSE otherwise
 */
boolean rr_in_dedicated(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return (rr_control_data_ptr->rr_state == RR_DATA_TRANSFER);

} /* end rr_in_dedicated() */

boolean rr_in_connection_pending(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return (rr_control_data_ptr->rr_state == RR_CONNECTION_PENDING);
} /* rr_in_connection_pending */

boolean rr_background_plmn_search_in_progress(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return (rr_control_data_ptr->rr_state == RR_PLMN_LIST_SEARCH);
}

boolean rr_is_inactive(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return (rr_control_data_ptr->rr_state == RR_INACTIVE);
}

boolean rr_is_acquiring_si_after_conn_rel(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return (rr_control_data_ptr->rr_state == RR_EARLY_CAMPED_WAIT_FOR_SI);
}

rr_state_T get_rr_state(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return rr_control_data_ptr->rr_state;
}

#ifdef PERLUTF
#error code not present
#endif // PERLUTF

boolean rr_is_control_state(const rr_state_T check_state, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return (rr_control_data_ptr->rr_state == check_state);
}

static void set_rr_deact_reason(rr_deact_reason_t reason, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  rr_control_data_ptr->rr_deact_reason = reason;

} /* end set_rr_deact_reason() */

static rr_deact_reason_t get_rr_deact_reason(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return rr_control_data_ptr->rr_deact_reason;

} /* end get_rr_deact_reason() */

/* reject the message */
void rr_reject_msg(rr_cmd_bdy_type* msg_ptr, const gas_id_t gas_id)
{
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (msg_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("msg_ptr=NULL");
    return;
  }

  if (msg_ptr->message_header.message_set == MS_MM_RR)
  {
    switch (msg_ptr->message_header.message_id)
    {
      case RR_DEACT_REQ:
        MSG_GERAN_ERROR_0_G("can't reject RR_DEACT_REQ!!");
        break;

      case RR_EST_REQ:
        rr_send_abort_ind(RR_CONNECTION_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
        break;

      case RR_PLMN_SELECT_REQ:
        rr_send_plmn_select_cnf(
          MM_AS_CONNECTED,
          MM_AS_NO_SERVICE,
          NULL,
          NULL,
          NULL,
          NULL,
          gas_id
        );
        break;

      case RR_BPLMN_SEARCH_ABORT_REQ:
        /* If 1) the RR_PLMN_LIST_REQ was saved but haven't been processed yet,
         *       delete the saved PLMN_LIST_REQ, and sends CNF based on the PLMN_LIST_REQ
         *    2) the RR_PLMN_LIST_REQ was processed in RR, sends CNF based on
         *        rr_plmn_list_req_info
         */
        if (grr_verify_saved_mm_message(RR_PLMN_LIST_REQ, gas_id))
        {
          rr_send_plmn_list_cnf(
            NULL,
            MM_AS_ABORTED,
            (rr_PLMN_list_req_info_T *)&rr_control_data_ptr->rr_saved_msg.msg,
            gas_id
          );
          rr_control_data_ptr->rr_saved_msg.valid = FALSE;
        }
        else
        {
          rr_send_plmn_list_cnf(
            NULL,
            MM_AS_ABORTED,
            rr_plmn_list_req_info_ptr,
            gas_id
          );
        }
        break;

      case RR_PLMN_LIST_REQ:
        rr_send_plmn_list_cnf(
          NULL,
          MM_AS_ABORTED,
          (rr_plmn_list_req_T *)msg_ptr,
          gas_id
        );
        break;

      default:
        MSG_GERAN_ERROR_1_G(
          "can't reject an unknown message %d!!",
          msg_ptr->message_header.message_id
        );
        break;
    }
  }
  else if (msg_ptr->message_header.message_set == MS_MAC_RR)
  {
    if ((mac_grr_sig_id_t) msg_ptr->message_header.message_id == MAC_GRR_UL_TBF_REQ)
    {
      /* MAC_GRR_UL_TBF_REQ does not need to be explicitly rejected if MAC
      suspension state machine is not in camped state, as MAC will already
      have been sent a GRR_MAC_NO_PS_ACCESS_IND via some other route.  Receiving
      MAC_GRR_UL_TBF_REQ in this case is likely to be due to some race condition
      that allows GRR_MAC_NO_PS_ACCESS_IND and MAC_GRR_UL_TBF_REQ to cross over
      while in transit. */
      if (rr_mac_ps_access_granted(gas_id))
      {
        /* send delete one to GMAC because we cannot handle tbf req and send access ind */
        rr_send_grr_mac_no_ps_access_ind(DEL_ONE, gas_id);
        rr_send_grr_mac_ps_access_req(FALSE, gas_id);
      }
    }
    else
    {
      MSG_GERAN_ERROR_1_G(
        "can't reject an unknown message %d!!",
        msg_ptr->message_header.message_id
      );
    }
  }
  else if (msg_ptr->message_header.message_set == MS_RR_L2)
  {
    if ((rr_l2_message_id_enum_T) msg_ptr->message_header.message_id == DL_UNIT_DATA_IND)
    {
      /* no need to send any reject/cnf to L2 */
    }
    else
    {
      MSG_GERAN_ERROR_1_G(
        "can't reject an unknown message %d!!",
        msg_ptr->message_header.message_id
      );
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G(
      "can't reject an unknown message %d!!",
      msg_ptr->message_header.message_id
    );
  }

  MSG_GERAN_HIGH_2_G(
    "reject message (%d, %d)",
    msg_ptr->message_header.message_set,
    msg_ptr->message_header.message_id
  );

}/* rr_reject_msg */

/**
 * Removes a message from queued message buffer
 * and copies it to message pointer
 *
 * @param message_ptr_ptr Pointer to memory space where message should be copied to
 *
 * @return FALSE if the buffer is empty or the message was not copied, TRUE if success
 */
static void rr_delete_saved_msg(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_control_data_ptr->rr_saved_msg.valid == TRUE)
  {
    rr_reject_msg(&rr_control_data_ptr->rr_saved_msg.msg, gas_id);
  }

  rr_control_data_ptr->rr_saved_msg.valid = FALSE;

} /* rr_delete_saved_msg */


static rr_procedure_priority_e rr_get_message_priority(
  rr_cmd_bdy_type *msg_ptr
)
{
  rr_procedure_priority_e priority = PRIO_LOWEST;

  if (msg_ptr->message_header.message_set == MS_MM_RR)
  {
    switch (msg_ptr->message_header.message_id)
    {
      case RR_DEACT_REQ:
        priority = PRIO_SHUTDOWN;
        break;

      case RR_STOP_GSM_MODE_REQ:
        priority = PRIO_GSM_MODE_DEACTIVATION;
        break;

      case RR_EST_REQ:
        if (msg_ptr->rr_est_req.establishment_cause == (uint8) MM_RR_LOCATION_UPDATE)
        {
          priority = PRIO_CS_CALL_LAU;
        }
        else if (msg_ptr->rr_est_req.establishment_cause == (uint8) MM_RR_ANSWER_TO_PAGING)
        {
          priority = PRIO_CS_CALL_PAGE_RES;
        }
        else
        {
          priority = PRIO_CS_CALL_NORMAL;
        }
        break;

      case RR_PLMN_SELECT_REQ:
        priority = PRIO_PLMN_SELECT;
        break;

      case RR_PLMN_LIST_REQ:
        if (msg_ptr->rr_plmn_list_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
        {
          priority = PRIO_PLMN_SEARCH_AUTO;
        }
        else
        {
          priority = PRIO_PLMN_SEARCH_MANUAL;
        }
        break;

      default:
        priority = PRIO_LOWEST;
        break;
    }
  }
  else if (msg_ptr->message_header.message_set == MS_MAC_RR)
  {
    mac_grr_ul_tbf_req_t * mac_grr_ul_tbf_req = (mac_grr_ul_tbf_req_t *)msg_ptr;
    if ((mac_grr_sig_id_t) msg_ptr->message_header.message_id == MAC_GRR_UL_TBF_REQ)
    {
      if(mac_grr_ul_tbf_req->channel_req_info.pkt_est_cause == RR_PKT_EST_CAUSE_PAGE_RESP)
      {
        priority = PRIO_PS_CALL_PAGE_RES;
      }
      else
      {
        priority = PRIO_PS_CALL;
      }
    }
    else
    {
      MSG_GERAN_ERROR_2(
        "Other message (%d, %d) not need to be saved/check priority!",
        msg_ptr->message_header.message_set,
        msg_ptr->message_header.message_id
      );
    }
  }
  else if (msg_ptr->message_header.message_set == MS_RR_L2)
  {
    if ((rr_l2_message_id_enum_T) msg_ptr->message_header.message_id == DL_UNIT_DATA_IND)
    {
      priority = PRIO_DL_UNIT_DATA_IND;
    }
    else
    {
      MSG_GERAN_ERROR_2("Other message (%d, %d) not need to be saved/check priority!",
        msg_ptr->message_header.message_set,
        msg_ptr->message_header.message_id
      );
    }
  }
  else
  {
    MSG_GERAN_ERROR_2("Other message (%d, %d) not need to be saved/check priority!",
      msg_ptr->message_header.message_set,
      msg_ptr->message_header.message_id
    );
  }

  return priority;
}

boolean rr_save_msg(
  rr_cmd_bdy_type *new_msg_ptr,
  const gas_id_t gas_id
)
{
  rr_procedure_priority_e new_msg_priority = PRIO_LOWEST;
  rr_procedure_priority_e saved_msg_priority = PRIO_LOWEST;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (new_msg_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("new_msg_ptr=NULL");
    return FALSE;
  }

  /* if there is already a message saved, check the new and old messages'
   * priority, then save/leave the higher priority one
   */
  if (rr_control_data_ptr->rr_saved_msg.valid == TRUE)
  {
    /* get messages' prioirty */
    new_msg_priority = rr_get_message_priority(new_msg_ptr);
    saved_msg_priority = rr_get_message_priority(&rr_control_data_ptr->rr_saved_msg.msg);

    /* EDTM specific handling */
#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
    if (saved_msg_priority == PRIO_DL_UNIT_DATA_IND)
    {
      if(rr_gprs_edtm_prioritise_saved_msg(new_msg_ptr, gas_id) == TRUE)
      {
        /* copy the message to save into the buffer */
        rr_control_data_ptr->rr_saved_msg.msg   = *new_msg_ptr;
        rr_control_data_ptr->rr_saved_msg.valid = TRUE;

        MSG_GERAN_HIGH_1_G("Save IMsg(%s) for later processing",
          rr_gprs_message_name(
            new_msg_ptr,
            new_msg_ptr->message_header.message_set,
            new_msg_ptr->message_header.message_id,
            NULL
          )
        );

        if (new_msg_ptr->message_header.message_set == MS_RR_RR)
        {
          MSG_GERAN_HIGH_3_G(
            "Save IMsg: (%d,%d,%d) for later processing",
            new_msg_ptr->message_header.message_set,
            new_msg_ptr->rr.header.rr_message_set,
            new_msg_ptr->message_header.message_id
          );
        }
        else
        {
          MSG_GERAN_HIGH_2_G(
            "Save IMsg: (%d,%d) for later processing",
            new_msg_ptr->message_header.message_set,
            new_msg_ptr->message_header.message_id
          );
        }

        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

    /* if new message has higher priority than the old one, reject the old one
     * then save the new arrival, or vice versa
     */
    if (new_msg_priority >= saved_msg_priority)
    {
      rr_delete_saved_msg(gas_id);
    }
    else
    {
      rr_reject_msg(new_msg_ptr, gas_id);
      return FALSE;
    }
  }


  MSG_GERAN_HIGH_1_G("Save IMsg(%s) for later processing", \
    rr_gprs_message_name(
      new_msg_ptr,
      new_msg_ptr->message_header.message_set,
      new_msg_ptr->message_header.message_id,
      NULL
    )
  );

  if (new_msg_ptr->message_header.message_set == MS_RR_RR)
  {
    MSG_GERAN_HIGH_3_G(
      "Save IMsg(%d,%d,%d) for later processing",
      new_msg_ptr->message_header.message_set,
      new_msg_ptr->rr.header.rr_message_set,
      new_msg_ptr->message_header.message_id
    );
  }
  else
  {
    MSG_GERAN_HIGH_2_G(
      "Save IMsg(%d,%d) for later processing",
      new_msg_ptr->message_header.message_set,
      new_msg_ptr->message_header.message_id
    );
  }

  /* copy the message to save into the buffer */
  rr_control_data_ptr->rr_saved_msg.msg   = *new_msg_ptr;
  rr_control_data_ptr->rr_saved_msg.valid = TRUE;
  return TRUE;

}

#ifdef FEATURE_SGLTE

boolean rr_save_l2g_msg(rr_cmd_bdy_type *new_msg_ptr, const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (new_msg_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("new_msg_ptr=NULL");
    return FALSE;
  }

  /* if there is already a message saved, check the new and old messages'
   * priority, then save/leave the higher priority one
   */
  MSG_GERAN_HIGH_2_G("Save L2G IMsg(%d,%d) for later processing",
             new_msg_ptr->message_header.message_set,
             new_msg_ptr->message_header.message_id);

  /* copy the message to save into the buffer */
  rr_control_data_ptr->rr_saved_l2g_msg.msg   = *new_msg_ptr;
  rr_control_data_ptr->rr_saved_l2g_msg.valid = TRUE;

  return TRUE;
}

/**
 * Get and remove a message from saved  message buffer
 * and copies it to message pointer
 *
 * @param message_ptr_ptr Pointer to memory space where message should be copied to
 *
 * @return FALSE if the buffer is empty or the message was not copied, TRUE if success
 */
boolean rr_get_saved_l2g_msg(rr_cmd_bdy_type ** message_ptr_ptr, const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  /* check if empty */
  if (rr_control_data_ptr->rr_saved_l2g_msg.valid == FALSE)
  {
    return FALSE;
  }

  /* The saved message flag should always be set to FALSE so that the message */
  /* is no longer considered saved (in case caller doesn't want to see it)    */
  rr_control_data_ptr->rr_saved_l2g_msg.valid = FALSE;

  /* saves message into parameter if not null */
  if (message_ptr_ptr != NULL)
  {
    *message_ptr_ptr = &(rr_control_data_ptr->rr_saved_l2g_msg.msg);
  }
  else
  {
    return FALSE;
  }
  return TRUE;
} /* rr_get_saved_msg */

boolean rr_saved_l2g_msg_present(const gas_id_t gas_id)
{
  boolean message_present = FALSE;
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  /* check if empty */
  if (rr_control_data_ptr->rr_saved_l2g_msg.valid == TRUE)
  {
    message_present = TRUE;
  }
  else if (rr_control_data_ptr->rr_saved_l2g_msg.valid == FALSE)
  {
    message_present = FALSE;
  }
  MSG_GERAN_HIGH_1_G("L2G message presence is %d",message_present);
  return message_present;
}

rr_event_T rr_process_saved_l2g_message(rr_cmd_bdy_type **new_message_ptr_ptr, const gas_id_t gas_id)
{
  /* if there is a saved L2G message */
  if (rr_get_saved_l2g_msg(new_message_ptr_ptr, gas_id))
  {
    /* return EV_INPUT_MESSAGE to process the saved message */
    return EV_INPUT_MESSAGE;
  }

  /* no saved message was found */
  return EV_NO_EVENT;
} /* rr_process_saved_message */


void rr_delete_saved_l2g_msg(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  rr_control_data_ptr->rr_saved_l2g_msg.valid = FALSE;
  rr_control_data_ptr->sglte_processng_saved_l2g_ho_msg = FALSE;
  MSG_GERAN_HIGH_0_G("SGLTE SRVCC Saved L2G HO deleted");

} /* rr_delete_saved_l2_msg */

boolean rr_sglte_internal_stop_gsm_being_processed(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_control_data_ptr->sglte_processng_intrnl_stop_gsm)
  {
    /* internal flag is set: this function only returns TRUE in SGLTE mode */
    MSG_GERAN_HIGH_0_G("SGLTE SRVCC internal shutdown being processed");
    return(rr_is_sglte_mode_active(gas_id));
  }

  return(FALSE);
}

boolean rr_sglte_saved_l2g_ho_being_processed(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_HIGH_1_G("SGLTE SRVCC Saved L2G handover being processed: %d", rr_control_data_ptr->sglte_processng_saved_l2g_ho_msg);
  return rr_control_data_ptr->sglte_processng_saved_l2g_ho_msg;
}

void rr_sglte_reinit_processing_l2g_ho_related_parameter(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_HIGH_0_G("SGLTE SRVCC resetting sglte_processng_saved_l2g_ho_msg");
  rr_control_data_ptr->sglte_processng_saved_l2g_ho_msg = FALSE;
}

void rr_reject_nas_msg_during_intrn_shutdown(rr_cmd_bdy_type* msg_ptr, const gas_id_t gas_id)
{
  if (msg_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("msg_ptr=NULL");
    return;
  }

  switch (msg_ptr->message_header.message_id)
  {
    case RR_DEACT_REQ:
      MSG_GERAN_HIGH_0_G("Reject RR_DEACT_REQ during Internal shutdown");
      rr_send_deact_cnf(gas_id);
      break;

    case RR_EST_REQ:
      MSG_GERAN_HIGH_0_G("Reject RR_EST_REQ during Internal shutdown");
      rr_sglte_srvcc_send_abort_ind(gas_id);
      break;

    case RR_PLMN_SELECT_REQ:
      MSG_GERAN_HIGH_0_G("Reject RR_PLMN_SELECT_REQ during Internal shutdown");
      rr_send_plmn_select_cnf_no_service(MM_AS_INTERRAT_HANDOVER, gas_id);
      break;

    case RR_PLMN_LIST_REQ:
      MSG_GERAN_HIGH_0_G("Reject RR_PLMN_LIST_REQ during Internal shutdown");
      rr_reject_msg(msg_ptr, gas_id);
      break;

    case RR_STOP_GSM_MODE_REQ:
      MSG_GERAN_HIGH_0_G("Reject RR_STOP_GSM_MODE_REQ during Internal shutdown");
      rr_send_stop_gsm_mode_cnf(MM_AS_CONNECTED, gas_id);
      break;

    default:
      MSG_GERAN_ERROR_1_G("can't reject an unknown message %d!!",msg_ptr->message_header.message_id);
  }
}

static void rr_sglte_send_lte_ho_failed_rsp(const gas_id_t gas_id)
{
  geran_grr_lte_ho_failed_rsp_t rsp;
  errno_enum_type               result;

  memset(&rsp, 0, sizeof(geran_grr_lte_ho_failed_rsp_t));

  msgr_init_hdr(
    &rsp.hdr,
    MSGR_GERAN_GRR,
    GERAN_GRR_LTE_HO_FAILED_RSP
  );

  rsp.ho_fail_cause = GERAN_GRR_LTE_HO_FAILURE_CAUSE_INVALID_STATE;

  result = rr_msgr_send(&rsp.hdr, sizeof(rsp), gas_id);

  if (result != E_SUCCESS)
  {
    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
  }
}


#endif /*FEATURE_SGLTE*/


/**
 * A version of rr_save_msg specifically for saving a RR_EST_REQ.
 */
void rr_save_rr_est_req(
  rr_est_req_T *rr_est_req_ptr,
  const gas_id_t gas_id
)
{
  if (rr_est_req_ptr != NULL)
  {
    rr_cmd_bdy_type *rr_cmd_bdy_ptr = GPRS_MEM_MALLOC(sizeof(rr_cmd_bdy_type));

    if (rr_cmd_bdy_ptr != NULL)
    {

      rr_cmd_bdy_ptr->mm.rr_est_req = *rr_est_req_ptr;

      rr_save_msg(rr_cmd_bdy_ptr, gas_id);

      GPRS_MEM_FREE(rr_cmd_bdy_ptr);
    }
  }
}

/**
 * Get and remove a message from queued message buffer
 * and copies it to message pointer
 *
 * @param message_ptr_ptr Pointer to memory space where message should be copied to
 *
 * @return FALSE if the buffer is empty or the message was not copied, TRUE if success
 */
boolean rr_get_saved_msg(
  rr_cmd_bdy_type ** message_ptr_ptr,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  /* check if empty */
  if (rr_control_data_ptr->rr_saved_msg.valid == FALSE)
  {
    return FALSE;
  }

  /* The saved message flag should always be set to FALSE so that the message */
  /* is no longer considered saved (in case caller doesn't want to see it)    */
  rr_control_data_ptr->rr_saved_msg.valid = FALSE;

  /* saves message into parameter if not null */
  if (message_ptr_ptr != NULL)
  {
    *message_ptr_ptr = &(rr_control_data_ptr->rr_saved_msg.msg);
  }
  else
  {
    return FALSE;
  }

  return TRUE;
} /* rr_get_saved_msg */

/**
 * Returns TRUE if rr_saved_msg is valid and is a RR_EST_REQ (any type), FALSE otherwise
 */
boolean rr_saved_msg_is_rr_est_req(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_control_data_ptr->rr_saved_msg.valid)
  {
    if (rr_control_data_ptr->rr_saved_msg.msg.message_header.message_set == MS_MM_RR &&
        rr_control_data_ptr->rr_saved_msg.msg.message_header.message_id == (byte) RR_EST_REQ)
    {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Returns TRUE if rr_saved_msg is valid and is an RR_EST_REQ for an emergency
 * call, FALSE otherwise.
 */
boolean rr_saved_msg_is_rr_est_req_emergency_call(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_control_data_ptr->rr_saved_msg.valid)
  {
    if (rr_control_data_ptr->rr_saved_msg.msg.message_header.message_set == MS_MM_RR &&
        rr_control_data_ptr->rr_saved_msg.msg.message_header.message_id == (byte) RR_EST_REQ)
    {
      if (rr_control_data_ptr->rr_saved_msg.msg.rr_est_req.establishment_cause == (byte) MM_RR_EMERGENCY_CALL)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
/**
 * Saves a message from the EDTM module, but avoids overwriting existing data
 *
 * @param message pointer to new message to be saved
 *
 * @return TRUE if the new message was saved successfully
 */
boolean rr_save_msg_for_edtm(rr_cmd_bdy_type *message_ptr, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  /* N.B. Eventually, it might be worth calling prioritise_saved_msg() instead */
  if ((rr_control_data_ptr->rr_saved_msg.valid == TRUE) || (message_ptr == NULL))
  {
    /* return a failure */
    return(FALSE);
  }

  /* save the message */
  (void)rr_save_msg(message_ptr, gas_id);

  return(TRUE);
}
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

/**
 * Deletes a saved MAC_GRR_UL_TBF_REQ message (if there is one)
 */
void rr_delete_saved_mac_ul_tbf_req(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if ((rr_control_data_ptr->rr_saved_msg.valid) &&
      (rr_control_data_ptr->rr_saved_msg.msg.message_header.message_set == MS_MAC_RR) &&
      (rr_control_data_ptr->rr_saved_msg.msg.message_header.message_id  == (byte)MAC_GRR_UL_TBF_REQ))
  {
    MSG_GERAN_ERROR_0_G("Saved MAC_GRR_UL_TBF_REQ deleted");
    rr_control_data_ptr->rr_saved_msg.valid = FALSE;
  }

  return;
}

/**
 * Determines if a saved message exists (either a saved MM message
 * or a saved page) and if it does, sets the given message_pointer
 * to point to it and returns EV_INPUT_MESSAGE. Otherwise, returns
 * EV_NO_EVENT.
 *
 * @param new_message_ptr_ptr [out] Set to point to the message buffer containing the
 *                            saved message that should be processed
 *
 * @return EV_INPUT_MESSAGE if there is a saved message
 *         EV_NO_EVENT if no message is saved
 */
rr_event_T rr_process_saved_message(
  rr_cmd_bdy_type **new_message_ptr_ptr,
  const gas_id_t gas_id
)
{
  /* if there is a saved MM message */
  if (rr_get_saved_msg(new_message_ptr_ptr, gas_id))
  {
    /* and MM message was found, clear any saved pages. MM messages take precedence */
    rr_ce_discard_saved_pages(gas_id);

    /* return EV_INPUT_MESSAGE to process the saved message */
    return EV_INPUT_MESSAGE;
  }

  /* no saved message was found */
  return EV_NO_EVENT;
} /* rr_process_saved_message */

/**
 * Returns a saved message, if there is one.
 * Sets the given message_pointer to point to the saved message, and returns EV_INPUT_MESSAGE.
 * Otherwise, returns EV_NO_EVENT.
 *
 * Note: The saved message IS NOT removed, and should be either processed or removed using
 * rr_process_saved_message()
 *
 * @param new_message_ptr_ptr [out] Set to point to the message buffer containing the
 *                            saved message that should be processed
 *
 * @return EV_INPUT_MESSAGE if there is a saved message
 *         EV_NO_EVENT if no message is saved
 */
rr_event_T rr_examine_saved_message(
  rr_cmd_bdy_type **new_message_ptr_ptr,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if ( rr_control_data_ptr->rr_saved_msg.valid )
  {
    if ( new_message_ptr_ptr != NULL )
    {
      (* new_message_ptr_ptr) = &(rr_control_data_ptr->rr_saved_msg.msg);
    }

    return EV_INPUT_MESSAGE;
  }
  else
  {
    return EV_NO_EVENT;
  }

} /* end rr_examine_saved_message() */


/**
 * Saves a message for reporting following lower layer failure (panic reset)
 *
 * @param message pointer to message to be saved
 */
void rr_save_msg_for_recovery_after_panic_reset(
  rr_cmd_bdy_type* message_ptr,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  rr_control_data_ptr->rr_panic_reset_info.process_saved_msg = FALSE;

  if (message_ptr != NULL)
  {
    rr_control_data_ptr->rr_panic_reset_info.saved_msg.msg = *message_ptr;
    rr_control_data_ptr->rr_panic_reset_info.saved_msg.valid = TRUE;
  }
  else
  {
    rr_control_data_ptr->rr_panic_reset_info.saved_msg.valid = FALSE;
  }

} /* end rr_save_msg_for_recovery_after_panic_reset() */


static void rr_retry_msg_after_panic_reset_restart(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if ( rr_control_data_ptr->rr_panic_reset_info.saved_msg.valid )
  {
    rr_control_data_ptr->rr_panic_reset_info.process_saved_msg = TRUE;
  }

} /* rr_retry_msg_after_panic_reset_restart() */


static rr_event_T rr_process_panic_reset_saved_msg(
  rr_cmd_bdy_type **message_ptr_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T  rr_event = EV_NO_EVENT;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_control_data_ptr->rr_panic_reset_info.process_saved_msg)
  {
    if (message_ptr_ptr && rr_control_data_ptr->rr_panic_reset_info.saved_msg.valid)
    {
      *message_ptr_ptr = &(rr_control_data_ptr->rr_panic_reset_info.saved_msg.msg);
      rr_event = EV_INPUT_MESSAGE;
    }

    rr_control_data_ptr->rr_panic_reset_info.process_saved_msg = FALSE;
  }

  return rr_event;
}

boolean rr_verify_panic_reset_saved_message_set(
  uint8 message_set,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_control_data_ptr->rr_panic_reset_info.saved_msg.valid)
  {
    IMH_T * message_header = (IMH_T *) &rr_control_data_ptr->rr_panic_reset_info.saved_msg.msg;

    if (message_header->message_set == message_set)
    {
      return TRUE;
    }
  }

  return FALSE;
}

boolean rr_verify_panic_reset_saved_msg(
  uint8 message_set,
  uint8 message_id,
  const gas_id_t gas_id
)
{
  boolean match = FALSE;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_control_data_ptr->rr_panic_reset_info.saved_msg.valid)
  {
    IMH_T * message_header = (IMH_T *) &rr_control_data_ptr->rr_panic_reset_info.saved_msg.msg;

    if (message_header->message_set == message_set &&
        message_header->message_id == message_id)
    {
      match = TRUE;
    }
  }

  return match;

} /* end rr_verify_panic_reset_saved_msg() */

#if defined(FEATURE_WCDMA)
static sys_band_cap_u_type rr_get_plmn_list_req_band_pref(
  rr_plmn_list_req_T *plmn_list_req,
  sys_sys_mode_e_type sys_mode
)
{
  sys_band_cap_u_type band_cap_u = {0};

  band_cap_u = rr_rat_pri_list_info_band_pref(
    &plmn_list_req->mcc_mode_band_filter,
    sys_mode
  );

  if (rr_is_band_mask_empty(band_cap_u, sys_mode))
  {
    band_cap_u = rr_rat_pri_list_info_band_pref(
      &plmn_list_req->rat_pri_list_info,
      sys_mode
    );
  }

  return band_cap_u;
}
#endif /* FEATURE_WCDMA */

static void rr_validate_rat_priority_for_plmn_search(sys_rat_pri_list_info_s_type *rat_pri_list_ptr, const gas_id_t gas_id)
{
  if (NULL == rat_pri_list_ptr)
  {
    MSG_GERAN_ERROR_0_G("rat_pri_list_ptr=NULL");
  }
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  else
  {
    sys_rat_pri_list_info_s_type valid_list;
    boolean                      rat_allowed;
    uint32                 i;
  
    valid_list.num_items = 0;
    for (i = 0; (i < rat_pri_list_ptr->num_items) && (i < SYS_MAX_ACQ_PREF_RAT_LIST_NUM); i++)
    {
      /* determine whether each entry is an allowable RAT mode */
      switch(rat_pri_list_ptr->priority_list_info[i].acq_sys_mode)
      {
        case SYS_SYS_MODE_GSM:
         rat_allowed = TRUE;
         break;

#ifdef FEATURE_WCDMA
        case SYS_SYS_MODE_WCDMA:
         if (rr_g2x_rat_supported(RR_RAT_WCDMA, gas_id))
         {
           rat_allowed = rr_g2x_allowed_by_gprs_activity(RR_RAT_WCDMA, gas_id);
         }
         else
         {
           rat_allowed = FALSE;
         }
         break;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
        case SYS_SYS_MODE_TDS:
         if (rr_g2x_rat_supported(RR_RAT_UTRAN_TDD, gas_id))
         {
           rat_allowed = rr_g2x_allowed_by_gprs_activity(RR_RAT_UTRAN_TDD, gas_id);
         }
         else
         {
           rat_allowed = FALSE;
         }
         break;
#endif /* FEATURE_GSM_TDS */
  
#ifdef FEATURE_LTE
        case SYS_SYS_MODE_LTE:
         if (rr_g2x_rat_supported(RR_RAT_LTE, gas_id))
         {
           rat_allowed = rr_g2x_allowed_by_gprs_activity(RR_RAT_LTE, gas_id);
         }
         else
         {
           rat_allowed = FALSE;
         }
         break;
#endif /* FEATURE_LTE */
        default:
         rat_allowed = FALSE;
         break;
      }
      if (rat_allowed)
      {
        valid_list.priority_list_info[valid_list.num_items] = rat_pri_list_ptr->priority_list_info[i];
        valid_list.num_items++;
      }
    }
  
   /* copy the validated list back over the input list */
   rat_pri_list_ptr->num_items = valid_list.num_items;
   for (i = 0; (i < valid_list.num_items) && (i < SYS_MAX_ACQ_PREF_RAT_LIST_NUM); i++)
   {
     rat_pri_list_ptr->priority_list_info[i] = valid_list.priority_list_info[i];
   }
  }

#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return;
}

/**
 * save contents of plmn_list_req
 *
 * @param  plmn_list_req
 *
 * @return TRUE  if PLMN_LIST_REQ has higher priority and should be processed
 *         FALSE if PLMN_LIST_REQ has lower prirority and is rejected
 */
static void rr_start_plmn_list_search(
  rr_plmn_list_req_T * plmn_list_req,
  boolean foreground,
  const gas_id_t gas_id
)
{
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  sys_band_cap_u_type      band_cap_u;

  rr_validate_rat_priority_for_plmn_search(&(plmn_list_req->rat_pri_list_info), gas_id);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  MSG_GERAN_HIGH_4_G("PLMN search can start (network_selection_mode:%d trans_id:%d gsm_only:%d service_search:%d)",
    plmn_list_req->network_selection_mode,
    plmn_list_req->trans_id,
    rr_rat_pri_list_info_gsm_only(&plmn_list_req->rat_pri_list_info),
    plmn_list_req->service_search
  );
#else
  MSG_GERAN_HIGH_2_G("PLMN search can start (network_selection_mode:%d trans_id:%d)",
    plmn_list_req->network_selection_mode,
    plmn_list_req->trans_id
  );
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  /*
   * Save the message contents in case there is a failure / panic reset
   */
  rr_save_msg_for_recovery_after_panic_reset((rr_cmd_bdy_type *)plmn_list_req, gas_id);

  /*
   * save rr_plmn_list_req_info
   */
  rr_plmn_list_req_info_ptr->scan_scope = plmn_list_req->scan_scope;
  rr_plmn_list_req_info_ptr->network_selection_mode = plmn_list_req->network_selection_mode;
  rr_plmn_list_req_info_ptr->trans_id = plmn_list_req->trans_id;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
  rr_plmn_list_req_info_ptr->plmn_list = plmn_list_req->plmn_list;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/
  rr_plmn_list_req_info_ptr->rat_pri_list_info = plmn_list_req->rat_pri_list_info;
  rr_plmn_list_req_info_ptr->mcc_mode_band_filter = plmn_list_req->mcc_mode_band_filter;

  band_cap_u = rr_rat_pri_list_info_band_pref(rr_plc_get_rat_pri_list_info(gas_id), SYS_SYS_MODE_GSM);
  rr_set_preferred_bands(band_cap_u.chgwt_band_cap, gas_id);

#ifdef FEATURE_WCDMA
  band_cap_u = rr_get_plmn_list_req_band_pref(plmn_list_req, SYS_SYS_MODE_WCDMA);
  rr_update_band_preference_for_rrc(band_cap_u.chgwt_band_cap, gas_id);
#endif /* FEATURE_WCDMA */

  rr_plmn_list_req_info_ptr->service_search = plmn_list_req->service_search;

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
  RR_ZERO_STRUCTURE(rr_plmn_list_req_info_ptr->search_timer_value);

  if (plmn_list_req->search_timer_value.MaxManualPLMNTimer > 0)
  {
    rr_plmn_list_req_info_ptr->search_timer_value = plmn_list_req->search_timer_value;
  }
#endif /* FEATURE_MPLMN_PARTIAL_UPDATING */

#ifdef FEATURE_WCDMA

  /* Switch L1 to GSM-only mode if necessary (if currently in non-GSM-only and
  search is going to be performed in the background. */
  if (!foreground && !rr_gsm_only(gas_id))
  {
    rr_send_mph_ue_mode_change_req(TRUE, gas_id);
  }
#endif /* FEATURE_WCDMA */

  if (!foreground)
  {
    // Ensure L1 is in NORMAL PAGING and stop MM non-DRX mode (if applicable)
    if (rr_ce_compare_and_update_l1_page_mode(PAGING_REORGANIZATION, NORMAL_PAGING, gas_id))
    {
      MSG_GERAN_HIGH_0_G("Use Normal Paging for BPLMN search");
    }
    rr_gprs_clear_l1_in_mm_non_drx(gas_id);
  }

  /*
   * setup rr_plmn_list_control()
   */
  rr_plc_setup(foreground?RR_PLC_FOREGROUND_SEARCH:RR_PLC_BACKGROUND_SEARCH, 0, gas_id);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (foreground)
  {
    /* ensure that priority inversion is re-engaged now */
    rr_ms_service_recovery_started(gas_id);
  }
  else
  {
    rr_ms_bplmn_initialise_dsc(TRUE, gas_id);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return;
}

#ifdef FEATURE_SGLTE

#if defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
static void rr_sglte_x2g_plmn_list_search_results_clear(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);

  RR_NULL_CHECK_RETURN_VOID(rr_control_data_ptr);

  rr_control_data_ptr->sglte_x2g_plmn_list_search_results.srch_done = FALSE;
  rr_control_data_ptr->sglte_x2g_plmn_list_search_results.hplmn_found = FALSE;
  if (rr_control_data_ptr->sglte_x2g_plmn_list_search_results.available_plmn_list != NULL)
  {
    GPRS_MEM_FREE(rr_control_data_ptr->sglte_x2g_plmn_list_search_results.available_plmn_list);
    rr_control_data_ptr->sglte_x2g_plmn_list_search_results.available_plmn_list = NULL;
  }
  if (rr_control_data_ptr->sglte_x2g_plmn_list_search_results.detected_frequency_list != NULL)
  {
    GPRS_MEM_FREE(rr_control_data_ptr->sglte_x2g_plmn_list_search_results.detected_frequency_list);
    rr_control_data_ptr->sglte_x2g_plmn_list_search_results.detected_frequency_list = NULL;
  }
}
#endif /* FEATURE_LTE  || FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
/**
  @brief Starts an SGLTE L2G PLMN list search.

  Contents of the GERAN_GRR_LTE_PLMN_SRCH_REQ are copied in to the RR global
  PLMN list request details store, and the search is started in the foreground
  or background appropriately.

  This type of search is only ever triggered when GSM is already operating in
  GSM-only mode, so there is no need to switch GL1 to this mode explicitly.
 */
static void rr_start_sglte_l2g_plmn_list_search(
  lte_irat_plmn_srch_req_s * plmn_srch_req,
  boolean foreground,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  rr_PLMN_list_req_info_T * rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  sys_band_cap_u_type band_cap_u;

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);
  RR_NULL_CHECK_RETURN_VOID(plmn_srch_req);

  MSG_GERAN_HIGH_3_G("SGLTE L2G PLMN list search (network_select_mode:%d search_type:%d num_plmns:%d)",
                     plmn_srch_req->network_select_mode,
                     plmn_srch_req->search_type,
                     plmn_srch_req->num_plmns);

  if (plmn_srch_req->num_plmns > 0)
  {
    boolean            plmn_is_undefined;
    boolean            mnc_includes_pcs_digit;
    sys_mcc_type       mcc;
    sys_mnc_type       mnc;

    sys_plmn_get_mcc_mnc(plmn_srch_req->plmn[0],
                         &plmn_is_undefined,
                         &mnc_includes_pcs_digit,
                         &mcc,
                         &mnc);
    if (plmn_is_undefined)
    {
      MSG_GERAN_HIGH_0_G("HPLMN: undefined");
    }
    else
    {
      MSG_GERAN_HIGH_3_G("HPLMN: %d-%d (%d)", mcc, mnc, mnc_includes_pcs_digit);
    }
  }

  MSG_GERAN_HIGH_2_G("band_pref: %08x, timer_val: %dms",
                     plmn_srch_req->band_pref.chgwt_band_cap,
                     plmn_srch_req->timer_val);

    /*
   * Save the message contents in case there is a failure / panic reset
   */
  rr_save_msg_for_recovery_after_panic_reset((rr_cmd_bdy_type *) plmn_srch_req, gas_id);

  /*
   * save rr_plmn_list_req_info
   */
  rr_plmn_list_req_info_ptr->network_selection_mode = plmn_srch_req->network_select_mode;
  rr_plmn_list_req_info_ptr->trans_id = 0; /* No transaction id. for L2G search. */
  rr_plmn_list_req_info_ptr->service_search = plmn_srch_req->search_type;

  /* LTE does not pass a sys_rat_pri_list_info_s_type structure in the L2G
  PLMN search request, so spoof one here with appropriate info - the band_pref is however included
  in the request message so we use that.
  is updated by RRC whenever it receives a new one anyway. */
  rr_plmn_list_req_info_ptr->rat_pri_list_info.num_items = 1;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.next_acq_sys_index = 0;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].band_cap = plmn_srch_req->band_pref;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_GSM;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_time_interval = 0;

  /* set BST band filter as invalid */
  rr_plmn_list_req_info_ptr->mcc_mode_band_filter.num_items = 0;

  MSG_GERAN_HIGH_1_G("SGLTE L2G PLMN list search, Is search triggered in Foreground= %d",
                      (int)plmn_srch_req->foreground_search);

  if ( plmn_srch_req->foreground_search )
  { /* Set the time to remember to start it in PLMN List */
    rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_time_interval = plmn_srch_req->timer_val;
  }

  band_cap_u = rr_rat_pri_list_info_band_pref(&rr_plmn_list_req_info_ptr->rat_pri_list_info, SYS_SYS_MODE_GSM);
  rr_set_preferred_bands(band_cap_u.chgwt_band_cap, gas_id);

  if (plmn_srch_req->num_plmns != 0)
  {
    uint32 i;

    for (i = 0; i < plmn_srch_req->num_plmns && i < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH; ++i)
    {
      rr_plmn_list_req_info_ptr->plmn_list.length++;
      rr_plmn_list_req_info_ptr->plmn_list.info[i].rat = SYS_RAT_GSM_RADIO_ACCESS;
      rr_plmn_list_req_info_ptr->plmn_list.info[i].plmn = plmn_srch_req->plmn[i];
    }
  }

  if (!foreground)
  {
    // Ensure L1 is in NORMAL PAGING and stop MM non-DRX mode (if applicable)
    if (rr_ce_compare_and_update_l1_page_mode(PAGING_REORGANIZATION, NORMAL_PAGING, gas_id))
    {
      MSG_GERAN_HIGH_0_G("Use Normal Paging for BPLMN search");
    }
    rr_gprs_clear_l1_in_mm_non_drx(gas_id);
  }

  /*
   * setup rr_plmn_list_control()
   */
  rr_plc_setup(foreground ? RR_PLC_SGLTE_X2G_FOREGROUND_SEARCH : RR_PLC_SGLTE_X2G_BACKGROUND_SEARCH, 0, gas_id);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (FALSE == foreground)
  {
    rr_ms_bplmn_initialise_dsc(TRUE, gas_id);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_control_data_ptr->sglte_x2g_plmn_list_search_in_progress = TRUE;
  rr_control_data_ptr->sglte_x2g_plmn_list_search_is_foreground = plmn_srch_req->foreground_search;
  rr_control_data_ptr->sglte_x2g_plmn_list_search_originator = RR_RAT_LTE;
  rr_control_data_ptr->sglte_x2g_plmn_list_search_aborted_by_rrc = FALSE;

  rr_sglte_x2g_plmn_list_search_results_clear(gas_id);

  /* As this is an SGLTE search, we send a response back to LTE immediately
  to allow them to carry on with normal operations. This is the case ONLY when the requested search from LTE is a Background Search
  The GSM search will be performed in the background on GSM on the dedicated radio. */
  if ( plmn_srch_req->foreground_search == FALSE )
  {
    lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
    errno_enum_type          result;

    plmn_srch_rsp_ptr = (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));

    RR_NULL_CHECK_RETURN_VOID(plmn_srch_rsp_ptr );
    msgr_init_hdr(
      &plmn_srch_rsp_ptr->msg_hdr,
      MSGR_GERAN_GRR,
      GERAN_GRR_LTE_PLMN_SRCH_RSP
    );
    MSG_GERAN_HIGH_0_G("Sending L2G Response Immediately, as the Search is in BackGround");

    plmn_srch_rsp_ptr->srch_done = FALSE;

    rr_invalidate_csg_info(&plmn_srch_rsp_ptr->available_plmn_list);

    result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);
    if (result != E_SUCCESS)
    {
      MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
    }
    GPRS_MEM_FREE(plmn_srch_rsp_ptr);
  }
  else
  {
      MSG_GERAN_HIGH_0_G("L2G Response After search completion as the Search is in Foreground");
  }
}
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
/**
  @brief Starts an SGLTE L2G PLMN list search.

  Contents of the GERAN_GRR_LTE_PLMN_SRCH_REQ are copied in to the RR global
  PLMN list request details store, and the search is started in the foreground
  or background appropriately.

  This type of search is only ever triggered when GSM is already operating in
  GSM-only mode, so there is no need to switch GL1 to this mode explicitly.
 */
static void rr_start_sglte_t2g_plmn_list_search(
  tdsrr_interrat_plmn_srch_req_type * plmn_srch_req,
  boolean foreground, const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  rr_PLMN_list_req_info_T * rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);

  RR_NULL_CHECK_RETURN_VOID(plmn_srch_req);

  MSG_GERAN_HIGH_3_G("Starting SGLTE T2G PLMN list search, network_select_mode: %d, service_search: %d, num_plmns: %d",
                     plmn_srch_req->network_select_mode,
                     plmn_srch_req->service_search,
                     plmn_srch_req->num_plmns);

  if (plmn_srch_req->num_plmns > 0)
  {
    boolean            plmn_is_undefined;
    boolean            mnc_includes_pcs_digit;
    sys_mcc_type       mcc;
    sys_mnc_type       mnc;

    sys_plmn_get_mcc_mnc(plmn_srch_req->plmn[0],
                         &plmn_is_undefined,
                         &mnc_includes_pcs_digit,
                         &mcc,
                         &mnc);
    if (plmn_is_undefined)
    {
      MSG_GERAN_HIGH_0_G("HPLMN: undefined");
    }
    else
    {
      MSG_GERAN_HIGH_3_G("HPLMN: %d-%d (%d)", mcc, mnc, mnc_includes_pcs_digit);
    }
  }

  MSG_GERAN_HIGH_1_G("timer_val: %dms", plmn_srch_req->timer_val);

    /*
   * Save the message contents in case there is a failure / panic reset
   */
  rr_save_msg_for_recovery_after_panic_reset((rr_cmd_bdy_type *) plmn_srch_req, gas_id);

  /*
   * save rr_plmn_list_req_info
   */
  rr_plmn_list_req_info_ptr->network_selection_mode = plmn_srch_req->network_select_mode;
  rr_plmn_list_req_info_ptr->trans_id = 0; /* No transaction id. for L2G search. */
  rr_plmn_list_req_info_ptr->service_search = plmn_srch_req->service_search;

  /* LTE does not pass a sys_rat_pri_list_info_s_type structure in the L2G
  PLMN search request, so spoof one here with appropriate info - the band_pref is however included
  in the request message so we use that.
  is updated by RRC whenever it receives a new one anyway. */
  rr_plmn_list_req_info_ptr->rat_pri_list_info.num_items = 1;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.next_acq_sys_index = 0;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].band_cap.chgwt_band_cap = rr_band_pref(gas_id);
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_GSM;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_time_interval = 0;

  /* set BST band filter as invalid */
  rr_plmn_list_req_info_ptr->mcc_mode_band_filter.num_items = 0;

  if ( plmn_srch_req->is_foreground_search )
  {
    rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_time_interval = plmn_srch_req->timer_val;
  }

  if (plmn_srch_req->num_plmns != 0)
  {
    uint32 i;

    for (i = 0; i < plmn_srch_req->num_plmns && i < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH; ++i)
    {
      rr_plmn_list_req_info_ptr->plmn_list.length++;
      rr_plmn_list_req_info_ptr->plmn_list.info[i].rat = SYS_RAT_GSM_RADIO_ACCESS;
      rr_plmn_list_req_info_ptr->plmn_list.info[i].plmn = plmn_srch_req->plmn[i];
    }
  }

  if (!foreground)
  {
    // Ensure L1 is in NORMAL PAGING and stop MM non-DRX mode (if applicable)
    if (rr_ce_compare_and_update_l1_page_mode(PAGING_REORGANIZATION, NORMAL_PAGING, gas_id))
    {
      MSG_GERAN_HIGH_0_G("Use Normal Paging for BPLMN search");
    }
    rr_gprs_clear_l1_in_mm_non_drx(gas_id);
  }

  /*
   * setup rr_plmn_list_control()
   */
  rr_plc_setup(foreground ? RR_PLC_SGLTE_X2G_FOREGROUND_SEARCH : RR_PLC_SGLTE_X2G_BACKGROUND_SEARCH, 0, gas_id);

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (FALSE == foreground)
  {
    rr_ms_bplmn_initialise_dsc(TRUE, gas_id);
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_control_data_ptr->sglte_x2g_plmn_list_search_in_progress = TRUE;
  rr_control_data_ptr->sglte_x2g_plmn_list_search_originator = RR_RAT_UTRAN_TDD;
  rr_control_data_ptr->sglte_x2g_plmn_list_search_aborted_by_rrc = FALSE;
  rr_control_data_ptr->sglte_x2g_plmn_list_search_is_foreground = plmn_srch_req->is_foreground_search;


  MSG_GERAN_HIGH_1_G("SGLTE T2G PLMN list search, Is search triggered in Foreground= %d",
                     (int)plmn_srch_req->is_foreground_search);


  /* Do not clear the results if the request is to be done in Foreground, it will be done when Search is finished */
  if (plmn_srch_req->is_foreground_search == FALSE)
  {
    MSG_GERAN_HIGH_0_G("Sending T2G Response Immediately, as the Search is in BackGround");

    rr_sglte_x2g_plmn_list_search_results_clear(gas_id);

    /* As this is an SGLTE search, we send a response back to LTE immediately
    to allow them to carry on with normal operations.  The GSM search will be
    performed in the background on the dedicated radio. */
    rr_send_tds_interrat_plmn_search_cnf(FALSE, FALSE, NULL, gas_id);
  }
  else
  {
    MSG_GERAN_HIGH_0_G("T2G Response After search completion as the Search is in Foreground");
  }
}
#endif /*FEATURE_GSM_TDS*/


#endif /* FEATURE_SGLTE */

#ifdef FEATURE_GSM_TDS
/**
 * save contents of TDSRR_INTERRAT_PLMN_SRCH_REQ
 *
 * @param  tdsrr_interrat_plmn_srch_req_type
 *
 * @return None
 */
static void rr_start_ttog_plmn_search(
  tdsrr_interrat_plmn_srch_req_type * msg_ptr,
  const gas_id_t gas_id
)
{
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  sys_band_mask_type       plc_band_filter;

  memset(rr_plmn_list_req_info_ptr, 0, sizeof(rr_PLMN_list_req_info_T));

  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  MSG_GERAN_HIGH_3_G(
    "TTOG PLMN search (network_selection_mode:%d timer_val:%d service_search:%d)",
    msg_ptr->network_select_mode,
    msg_ptr->timer_val,
    msg_ptr->service_search
  );
  #else
  MSG_GERAN_HIGH_3_G(
    "TTOG PLMN search (network_selection_mode:%d timer_val:%d)",
    msg_ptr->network_select_mode,
    msg_ptr->timer_val,
    0
  );
  #endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

  /* save rr_plmn_list_req_info */
  rr_plmn_list_req_info_ptr->network_selection_mode = msg_ptr->network_select_mode;

  /* TDSCDMA does not pass a sys_rat_pri_list_info_s_type structure in the T2G
  PLMN search request, so spoof one here with appropriate info - the band_pref
  is updated by TDSRRC whenever it receives a new one anyway. */
  rr_plmn_list_req_info_ptr->rat_pri_list_info.num_items = 1;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.next_acq_sys_index = 0;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].band_cap.chgwt_band_cap = rr_band_pref(gas_id);
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_GSM;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_time_interval = 0;

#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  rr_plmn_list_req_info_ptr->service_search = msg_ptr->service_search;
#endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

  /* install the correct RAT priority list if a band filter has been set */
  plc_band_filter = rr_plc_get_band_filter(gas_id);
  if (SYS_BAND_MASK_EMPTY == plc_band_filter)
  {
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.num_items = 0;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.next_acq_sys_index = 0;
  }
  else
  {
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.num_items = 1;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.next_acq_sys_index = 0;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.priority_list_info[0].band_cap.chgwt_band_cap = plc_band_filter;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_GSM;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.priority_list_info[0].acq_sys_time_interval = 0;
  }

  if (msg_ptr->num_plmns != 0)
  {
    uint32 i;

    MSG_GERAN_HIGH_3_G(
      "TDSRR_INTERRAT_PLMN_SRCH_REQ (search_mode=%d num_plmns=%d hplmn=%6x)",
      msg_ptr->network_select_mode,
      msg_ptr->num_plmns,
      (msg_ptr->plmn[0].identity[0] << 16) +
      (msg_ptr->plmn[0].identity[1] << 8)  +
      msg_ptr->plmn[0].identity[2]);

    for (i=0; (i < msg_ptr->num_plmns) && (i < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH); i++)
    {
      rr_plmn_list_req_info_ptr->plmn_list.length ++;
      rr_plmn_list_req_info_ptr->plmn_list.info[i].rat = SYS_RAT_GSM_RADIO_ACCESS;
      rr_plmn_list_req_info_ptr->plmn_list.info[i].plmn = msg_ptr->plmn[i];
    }
  }

  /*
   * setup rr_plmn_list_control()
   */
  rr_plc_setup(RR_PLC_TTOG_SEARCH, msg_ptr->timer_val, gas_id);
}
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
/**
 * save contents of RR_INTERRAT_PLMN_SRCH_REQ
 *
 * @param  rr_interrat_plmn_srch_req_type
 *
 * @return TRUE  if RR_INTERRAT_PLMN_SRCH_REQ should be processed
 */
static void rr_start_wtog_plmn_search(
  rr_interrat_plmn_srch_req_type * msg_ptr,
  const gas_id_t gas_id
)
{
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  sys_band_mask_type       plc_band_filter;

  memset(rr_plmn_list_req_info_ptr, 0, sizeof(rr_PLMN_list_req_info_T));

#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  MSG_GERAN_HIGH_3_G(
    "WTOG PLMN search (network_selection_mode:%d timer_val:%d service_search:%d)",
    msg_ptr->network_select_mode,
    msg_ptr->timer_val,
    msg_ptr->service_search
  );
#else
  MSG_GERAN_HIGH_3_G(
    "WTOG PLMN search (network_selection_mode:%d timer_val:%d)",
    msg_ptr->network_select_mode,
    msg_ptr->timer_val,
    0
  );
#endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

  /* save rr_plmn_list_req_info */
  rr_plmn_list_req_info_ptr->scan_scope = msg_ptr->scan_scope;
  rr_plmn_list_req_info_ptr->network_selection_mode = msg_ptr->network_select_mode;

  /* WCDMA does not pass a sys_rat_pri_list_info_s_type structure in the W2G
  PLMN search request, so spoof one here with appropriate info - the band_pref
  is updated by RRC whenever it receives a new one anyway. */
  rr_plmn_list_req_info_ptr->rat_pri_list_info.num_items = 1;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.next_acq_sys_index = 0;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].band_cap.chgwt_band_cap = rr_band_pref(gas_id);
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_GSM;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_time_interval = 0;

#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  rr_plmn_list_req_info_ptr->service_search = msg_ptr->service_search;
#endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

  /* install the correct RAT priority list if a band filter has been set */
  plc_band_filter = rr_plc_get_band_filter(gas_id);
  if (SYS_BAND_MASK_EMPTY == plc_band_filter)
  {
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.num_items = 0;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.next_acq_sys_index = 0;
  }
  else
  {
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.num_items = 1;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.next_acq_sys_index = 0;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.priority_list_info[0].band_cap.chgwt_band_cap = plc_band_filter;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_GSM;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.priority_list_info[0].acq_sys_time_interval = 0;
  }

  if (msg_ptr->num_plmns != 0)
  {
    uint32 i;

    MSG_GERAN_HIGH_3_G(
      "RR_INTERRAT_PLMN_SRCH_REQ (search_mode=%d num_plmns=%d hplmn=%6x)",
      msg_ptr->network_select_mode,
      msg_ptr->num_plmns,
      (msg_ptr->plmn[0].identity[0] << 16) +
      (msg_ptr->plmn[0].identity[1] << 8)  +
      msg_ptr->plmn[0].identity[2]
    );

    for (i=0; (i < msg_ptr->num_plmns) && (i < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH); i++)
    {
      rr_plmn_list_req_info_ptr->plmn_list.length ++;
      rr_plmn_list_req_info_ptr->plmn_list.info[i].rat = SYS_RAT_GSM_RADIO_ACCESS;
      rr_plmn_list_req_info_ptr->plmn_list.info[i].plmn = msg_ptr->plmn[i];
    }
  }


  /*
   * setup rr_plmn_list_control()
   */
  rr_plc_setup(RR_PLC_WTOG_SEARCH, msg_ptr->timer_val, gas_id);
}


#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
/**
 * Process content of GERAN_GRR_LTE_GERAN_ACTIVITY_IND
 *
 * @param  ind_ptr Pointer to GERAN_GRR_LTE_GERAN_ACTIVITY_IND message.
 *
 * @return new RR event to be processed
 */
static rr_event_T rr_process_lte_geran_activity_ind(geran_grr_lte_geran_activity_ind_t *ind_ptr, const gas_id_t gas_id)
{
  if (ind_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("Ignored invalid GERAN_GRR_LTE_GERAN_ACTIVITY_IND"); 
  }
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  else
  {
    gas_id_t other_gas_id;
    uint32   activity_mask;

    /* update all other stacks, since the message is always received for the LTE-capable subscription */
    for (other_gas_id = GERAN_ACCESS_STRATUM_ID_1; other_gas_id <= GERAN_ACCESS_STRATUM_ID_MAX; other_gas_id++)
    {
      if (other_gas_id != gas_id)
      {
        /* retrieve the current activity information for the other subscription */
        activity_mask = geran_get_lte_activity_status(other_gas_id);

        if (ind_ptr->embms_active)
        {
          /* enable the EMBMS bit in the mask */
          activity_mask |= LTE_ACTIVITY_EMBMS;
        }
        else
        {
          /* disable the EMBMS bit in the mask */
          activity_mask &= (~LTE_ACTIVITY_EMBMS);
        }

        /* write the updated mask back to GCOMMON storage */
        geran_set_lte_activity_status(activity_mask, other_gas_id);
        MSG_GERAN_HIGH_2("gs%1d: LTE activity mask updated to 0x%08X", 1+(int)other_gas_id, activity_mask);

        if (rr_mode_gsm_active_or_starting(other_gas_id))
        {
          /* if GL1 is active, trigger it to resubmit any acquisition requests */
          rr_send_mph_set_priority_ind(GERAN_PRIORITY_NOT_CHANGED, FALSE, other_gas_id);
        }
      }
    }
  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  return(EV_NO_EVENT);
}

/**
 * Save contents of GERAN_GRR_LTE_PLMN_SRCH_REQ
 *
 * @param  msg_ptr Pointer to GERAN_GRR_LTE_PLMN_SRCH_REQ message.
 *
 */
static void rr_start_ltog_plmn_search(
  lte_irat_plmn_srch_req_s * msg_ptr,
  const gas_id_t gas_id
)
{
  rr_PLMN_list_req_info_T *rr_plmn_list_req_info_ptr = rr_get_plmn_list_req_info_ptr(gas_id);
  sys_band_mask_type       plc_band_filter;

  memset(rr_plmn_list_req_info_ptr, 0, sizeof(rr_PLMN_list_req_info_T));

  MSG_GERAN_HIGH_3_G(
    "LTOG PLMN search (network_selection_mode:%d timer_val:%d service_search:%d)",
    msg_ptr->network_select_mode,
    msg_ptr->timer_val,
    msg_ptr->search_type
  );

  /* save rr_plmn_list_req_info */
  rr_plmn_list_req_info_ptr->scan_scope = msg_ptr->scan_scope;
  rr_plmn_list_req_info_ptr->network_selection_mode = msg_ptr->network_select_mode;

  /* LTE does not pass a sys_rat_pri_list_info_s_type structure in the L2G
  PLMN search request, so spoof one here with appropriate info - the band_pref is however included
  in the request message so we use that.
  is updated by RRC whenever it receives a new one anyway. */
  rr_plmn_list_req_info_ptr->rat_pri_list_info.num_items = 1;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.next_acq_sys_index = 0;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].band_cap = msg_ptr->band_pref;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_GSM;
  rr_plmn_list_req_info_ptr->rat_pri_list_info.priority_list_info[0].acq_sys_time_interval = 0;

  rr_plmn_list_req_info_ptr->service_search = msg_ptr->search_type;

  /* install the correct RAT priority list if a band filter has been set */
  plc_band_filter = msg_ptr->bst_band_pref.chgwt_band_cap;

  rr_plc_set_band_filter(plc_band_filter, gas_id);

  if (SYS_BAND_MASK_EMPTY == plc_band_filter)
  {
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.num_items = 0;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.next_acq_sys_index = 0;
  }
  else
  {
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.num_items = 1;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.next_acq_sys_index = 0;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.priority_list_info[0].band_cap.chgwt_band_cap = plc_band_filter;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_GSM;
    rr_plmn_list_req_info_ptr->mcc_mode_band_filter.priority_list_info[0].acq_sys_time_interval = 0;
  }

  if (msg_ptr->num_plmns != 0)
  {
    uint32 i;

    MSG_GERAN_HIGH_3_G(
      "GERAN_GRR_LTE_PLMN_SRCH_REQ (search_mode=%d num_plmns=%d hplmn=%6x)",
      msg_ptr->network_select_mode,
      msg_ptr->num_plmns,
      (msg_ptr->plmn[0].identity[0] << 16) +
      (msg_ptr->plmn[0].identity[1] << 8)  +
      msg_ptr->plmn[0].identity[2]
    );

    for (i=0; (i < msg_ptr->num_plmns) && (i < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH); i++)
    {
      rr_plmn_list_req_info_ptr->plmn_list.length ++;
      rr_plmn_list_req_info_ptr->plmn_list.info[i].rat = SYS_RAT_GSM_RADIO_ACCESS;
      rr_plmn_list_req_info_ptr->plmn_list.info[i].plmn = msg_ptr->plmn[i];
    }
  }

  /*
   * setup rr_plmn_list_control()
   */
  rr_plc_setup(RR_PLC_LTOG_SEARCH, msg_ptr->timer_val, gas_id);
}

#endif /* FEATURE_LTE */

/**
 * Adjust the S counter value in dedicated and determine if
 * a radio link failure has occurred or not
 *
 * @param block_quality quality of the block just received, TRUE if good, FALSE if bad
 *
 * @return new RR event to be processed
 */
static rr_event_T rr_check_rlf_in_dedicated(
  gl1_block_quality_t block_quality,
  const gas_id_t gas_id
)
{
  rr_event_T rr_event = EV_NO_EVENT;
  uint8      S_counter;

  if (block_quality == GL1_BLOCK_QUALITY_GOOD)
  {
    S_counter = rr_increment_S_counter(gas_id);
  }
  else if (block_quality == GL1_BLOCK_QUALITY_BAD)
  {
    S_counter = rr_decrement_S_counter(gas_id);
  }
  else
  {
    S_counter = 0;
  }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  if (block_quality == GL1_BLOCK_QUALITY_UNKNOWN)
  {
    /* this should never happen, since L1 in dedicated always has a result */
    MSG_GERAN_ERROR_0_G("GL1 failed to determine block quality during dedicated");
  }
  else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
  {
    rr_log_s_counter(S_counter, gas_id);

    geran_eng_mode_data_write(
      ENG_MODE_RADIO_LINK_TIMEOUT_VALUE,    // geran_eng_mode_info_type
      &S_counter,                           // param_copy
      TRUE,                                 // valid_flag
      gas_id                                // gas_id
    );

    if (S_counter == 0)
    {
      rr_event = EV_RADIO_LINK_FAILURE;
    }
  }

  return(rr_event);
} /* rr_check_rlf_in_dedicated */

/**
 * Prepares an OTA message received in a DL_UNIT_DATA_IND message
 * (unacknowledged mode) for parsing and determines if the
 * message can be decoded or not
 *
 * @param message_ptr Pointer to DL_UNIT_DATA_IND carrying the OTA message
 *
 * @return TRUE if the message is decoded successfully, FALSE otherwise
 */
boolean rr_prepare_ota_msg_for_parsing(
  dl_unit_data_ind_T *message_ptr,
  const gas_id_t gas_id
)
{
  boolean decoded_ok = TRUE;
  RR_cause_T error_type;

  /* use the check for protocol errors function to prepare the message and determine if it's OK */
  if (message_ptr->l3_message_length <= N201_MAX)
  {
    rr_check_for_protocol_errors(
      message_ptr->layer3_message,
      message_ptr->l3_message_length,
      message_ptr->l2_channel_type,
      message_ptr->short_header,
      &decoded_ok,
      &error_type,
      gas_id
    );
  }
  else
  {
    error_type = RR_SEMANTICALLY_INCORRECT_MESSAGE;
    MSG_GERAN_MED_3_G("Message length exceeds message payload size:Received message length: %d,expected:%d, cause=%d",
      message_ptr->l3_message_length,
      N201_MAX,
      (int) error_type
    );
    decoded_ok = FALSE;
  }

  /* if the message was not decoded successfully */
  if (!decoded_ok)
  {
    /* generate an error message indicating the problem */
    MSG_GERAN_ERROR_1_G("Bad OTA Message, cause=%d",(int) error_type);
  }

  return decoded_ok;
} /* rr_prepare_ota_msg_for_parsing */

/**
 * Prepares the OTA message from a DL_DATA_IND message (contains
 * acknowledged message)
 *
 * @param message_ptr    pointer to DL_DATA_IND containing OTA message
 *
 * @param error_type_ptr [out] set to the error type value if the message is not OK
 *
 * @return TRUE if the message is OK, FALSE otherwise
 */
boolean rr_prepare_ota_msg_for_parsing_in_dedicated(
  dl_data_ind_T *message_ptr,
  RR_cause_T    *error_type_ptr,
  const gas_id_t gas_id
)
{
  boolean decoded_ok = TRUE;

  /* use the check for protocol errors function to prepare the message and determine if it's OK */
  rr_check_for_protocol_errors(
    message_ptr->layer3_message,
    message_ptr->l3_message_length,
    message_ptr->l2_channel_type,
    FALSE,
    &decoded_ok,
    error_type_ptr,
    gas_id
  );

  return decoded_ok;
} /* rr_prepare_ota_msg_for_parsing_in_dedicated */

typedef enum
{
  RR_NOT_CAMPED,
  RR_CONNECTION_ESTABLISHMENT,
  RR_DEDICATED
} rr_leave_idle_mode_next_state_T;

/**
 * Leave the IDLE mode
 *
 * @param future_state Indicates the next "state" of the RR when it leaves idle mode
 */
static void rr_leave_idle_mode(
  rr_leave_idle_mode_next_state_T next_state,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
#ifdef FEATURE_WCDMA
  surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
  // TODO DSDA
  rr_ded_irat_measurements_t   *tds_ded_meas_ptr = rr_get_tds_ded_meas_ptr(gas_id);
#endif /*FEATURE_GSM_TDS*/

  if (next_state == RR_NOT_CAMPED)
  {
    MSG_GERAN_HIGH_0_G("Leaving idle mode to NOT_CAMPED");
    scell_info_ptr->gsm.camped_on_flag = FALSE;

    /* reset the surrounding database */
    rr_reset_surrounding_database(gas_id);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    /*************************************************************/
    /* Initialise wcdma surrounding cell data base               */
    /*************************************************************/
    rr_g2wr_reset_wcdma_surrounding_database(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  }

  /**************************************************************/
  /* NOTE: penalty timers not cancelled for going to connection */
  /* establishment phase because RR could still return to idle  */
  /* on the same cell during this time                          */
  /**************************************************************/

  rr_cancel_all_five_second_timers(gas_id);

  if (next_state == RR_DEDICATED)
  {
    /* Reset the S_counter to RADIO_LINK_TIMEOUT */
    rr_reset_S_counter(gas_id);

    /* no need to reinitialize the S counter again */
    scell_info_ptr->gsm.RLT_valid = TRUE;

    /* reset the surrounding database, it may not be valid after RR connection is released */
    rr_reset_surrounding_database(gas_id);

    rr_initialize_MI_instances(gas_id);

#ifdef FEATURE_GSM_TDS
    rr_initialize_tds_ded_measurements(tds_ded_meas_ptr);
#endif /*FEATURE_GSM_TDS*/

    rr_dedicated_mode_process_initial_params(
      scell_info_ptr,
      rr_get_instances_ptr(SI2Q_INSTANCE,gas_id),
      gas_id
    );

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    /*************************************************************/
    /* Initialise the wcdma surrounding cell data base           */
    /*************************************************************/
    rr_g2wr_reset_wcdma_surrounding_database(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_WCDMA
    /* Reinitialise the WCDMA measurements store.  This ensures that we do not
    include measurements taken in a previous call during this call. */
    rr_initialize_wcdma_ded_measurements(wcdma_ded_meas_ptr);

    /* Reinitialise the WCDMA dedicated neighbor cell list */
    rr_initialize_wcdma_ded_neighbor_list(&scell_info_ptr->gsm.wcdma_ded_neighbor_list);
#endif /* FEATURE_WCDMA */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    if (!rr_gsm_only(gas_id))
    {
      boolean lte_nbr_list_valid = FALSE;

#ifdef FEATURE_LTE
      lte_nbr_list_valid = (scell_info_ptr->gsm.dedicated_mode_params != NULL && 
                            scell_info_ptr->gsm.dedicated_mode_params->eutran_params.neighbour_list.count > 0);
#endif /* FEATURE_LTE */
      
      /* When entering into dedicated mode, update L1 with the 3g neighbor cell list
      received from SI2quater; This list will be used by L1 until RR updates L1 again
      after receiving 3G_WAIT instances of Measurement Information message */

      if (scell_info_ptr->gsm.utran_neighbor_list.valid_data == TRUE ||
          lte_nbr_list_valid == TRUE)
      {
        /* Send the mph_dedicated_wcdma_cell_update_list_req to L1 which has the 3g neighbor cells received in MI */
        /* L1 will start sending the mph_surround_dedicated_meas_ind after processing this message */
        rr_send_mph_dedicated_wcdma_cell_update_list_req(
          &scell_info_ptr->gsm.utran_neighbor_list,
          NULL,
          scell_info_ptr->gsm.dedicated_mode_params,
          gas_id
        );
      }
    }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  } /* going to dedicated mode */
} /* rr_leave_idle_mode */

static rr_event_T rr_process_rr_deact_req(
  rr_cmd_bdy_type *new_message_ptr,
  const gas_id_t gas_id
)
{
  rr_control_data_t *rr_control_data_ptr;

  boolean pass_to_rr_control = TRUE;
  rr_event_T new_event =  EV_NO_EVENT;

  RR_NULL_CHECK_RETURN_PARAM(new_message_ptr, EV_NO_EVENT);

  // Obtain a pointer to the module data
  rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  /***********************************************************/
  /* RR_DEACT_REQ indicates that the MS is being deactivated */
  /***********************************************************/

  /* Save this message in case of failure */
  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

  // RR_DEACT_REQ contains deact_reason, which can be
  //  RR_MM_MODE_CHANGE - (e.g. change to 1X)
  //  RR_MM_POWER_OFF - power-off or Game Mode
  //  RR_MM_OFF_LINE - (e.g. change to FTM)
  //  RR_MM_LPM_POWER_OFF - power-off (Android)

  switch (new_message_ptr->rr_deact_req.deact_reason)
  {
    case RR_MM_MODE_CHANGE:
    {

      rr_l1_gsm_deact_reason_set(RR_L1_MODE_CHANGE, gas_id);
      rr_l1_gsm_stop_reason_set(RR_L1_DEEP_SLEEP, gas_id);
      break;
    }

    case RR_MM_POWER_OFF:
    case RR_MM_LPM_POWER_OFF:
    {
      rr_l1_gsm_deact_reason_set(RR_L1_POWER_OFF, gas_id);
      rr_l1_gsm_stop_reason_set(RR_L1_DEEP_SLEEP, gas_id);
      break;
    }

    case RR_MM_OFF_LINE:
    default:
    {
      rr_l1_gsm_deact_reason_set(RR_L1_OFF_LINE, gas_id);
      rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);
    }
  }

  MSG_GERAN_HIGH_2_G(
    "Deactivating RR software in state %d for reason %d",
    (int)(rr_control_data_ptr->rr_state),
    (int)(new_message_ptr->rr_deact_req.deact_reason)
  );

  /* store any information in NV that is needed */


  /**
   * decide how to proceed based on RR/GRR states
   */
  if (rr_gprs_grr_is_active(gas_id))
  {
    /* To enable GPRS to be gracefully shut-down, the RR_DEACT_REQ is handled just like any */
    /* signal. Once GPRS is deactivated, processing will return to here and the RR_DEACT_REQ */
    /* will be processed again, but this time, GRR will not be active */
    pass_to_rr_control = TRUE;
  }
  else if (rr_control_data_ptr->rr_state == RR_CELL_SELECTION)
  {
    /* process in rr_selection_control() */
    pass_to_rr_control = TRUE;
  }
  else if (rr_control_data_ptr->rr_state == RR_PLMN_LIST_SEARCH)
  {
    /**
     * If RR_DEACT_REQ is received during GTOW PLMN search, RR needs
     * CNF from RRC before sending DEACT_CNF to NAS.
     */
    pass_to_rr_control = TRUE;
  }

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
  else if (rr_w2g_redirection_search_in_progress(gas_id))
  {
    /* let this signal be processed in rr_w2g_service_redirection_control() */
    pass_to_rr_control = TRUE;
  }
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS_G2T_REDIRECTION)
  else if (rr_control_data_ptr->rr_state == RR_G2X_REDIR)
  {
    pass_to_rr_control = TRUE;
  }
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION */

  else if (rr_control_data_ptr->rr_state == RR_GOING_ACTIVE)
  {
    pass_to_rr_control = TRUE;
  }
  else if (rr_control_data_ptr->rr_state == RR_CELL_REESTABLISH)
  {
    /* process in rr_cell_reest_control() */
    pass_to_rr_control = TRUE;
  }
#ifdef FEATURE_SGLTE
  else if (rr_control_data_ptr->rr_state == RR_W2G_INTERRAT_HANDOVER_PROGRESS)
  {
    pass_to_rr_control = TRUE;
  }
#endif
  /* If GL1 is inactive or deactivating and the new stop reason is for deep
  sleeep and this does not match the stop reason that has already been sent to
  GL1 then allow rr_control to handle the RR_DEACT_REQ - it will be saved and
  processed later. */
  else if ((rr_control_data_ptr->rr_state == RR_GOING_INACTIVE ||
            rr_control_data_ptr->rr_state == RR_INACTIVE) &&
            rr_mode_get_l1_stop_reason(gas_id) != rr_l1_gsm_stop_reason_get(gas_id) &&
            rr_l1_gsm_stop_reason_get(gas_id) == RR_L1_DEEP_SLEEP)
  {
    MSG_GERAN_HIGH_3_G(
      "rr_control() state:%d last L1 stop reason:%d new L1 stop reason:%d",
      rr_control_data_ptr->rr_state,
      rr_mode_get_l1_stop_reason(gas_id),
      rr_l1_gsm_stop_reason_get(gas_id)
    );

    pass_to_rr_control = TRUE;
  }
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  else if (rr_control_data_ptr->rr_state == RR_X2G_RESEL)
  {
    pass_to_rr_control = TRUE;
  }
  else if (rr_control_data_ptr->rr_state == RR_CONNECTION_PENDING &&
           rr_x2g_cco_in_progress(gas_id))
  {
    pass_to_rr_control = TRUE;
  }
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */
  else if (rr_control_data_ptr->rr_state == RR_CELL_SELECTED_ACQUIRE_SI)
  {
    pass_to_rr_control = TRUE;
  }
  else
  {
    /**
     * There is a possibility of Receiving DEACT_REQ when RR
     * is Inactive, in that case no need to send STOP_GSM_MODE Req
     * To L1 just reset the State machines
     */
    pass_to_rr_control = FALSE;
  }

#ifdef FEATURE_LTE
  rr_control_data_ptr->g2l_resel_allowed = TRUE;
#endif /* FEATURE_LTE */

  if (pass_to_rr_control == FALSE)
  {
    if ( rr_control_data_ptr->rr_state == RR_INACTIVE  )
    {
      rr_ps_no_ps_access_del_all(gas_id);
      rr_init_modem_offline(gas_id);

      if ((new_message_ptr->rr_deact_req.deact_reason == RR_MM_POWER_OFF) ||
          (new_message_ptr->rr_deact_req.deact_reason == RR_MM_LPM_POWER_OFF))
      {
        /* only write NV if deactivate reason is RR_MM_POWER_OFF */
        rr_acq_db_save(gas_id);

        if (FALSE == rr_other_rat_deactivation_required(gas_id))
        {
          rr_send_deact_cnf(gas_id);
        }
      }
      else
      {
        rr_send_deact_cnf(gas_id);
      }
    }
    else if ( rr_control_data_ptr->rr_state != RR_RESET )
    {
      set_rr_deact_reason(RR_DEACT_REASON_DEACT_REQ, gas_id);
      rr_delete_saved_msg(gas_id);

      /**
       * This will send STOP_MODE_REQ to L1, or,
       * if L1 is already inactive (RR_INACTIVE or in IRAT process),
       * immediately return EV_GSM_MODE_DEACTIVATED,
       */
      new_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
      rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
    }
    else
    {
      new_event = EV_NO_EVENT;
    }
  }
  else
  {
    new_event = EV_INPUT_MESSAGE;
  }

  return new_event;
}

/**
 * Cancel all timers relevant to RR_IDLE
 */
static void rr_cancel_idle_timers(
  const gas_id_t gas_id
)
{
  (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);
  (void)rr_timer_cancel(SIGNAL_STRENGTH_TIMER, gas_id);
  rr_cancel_all_five_second_timers(gas_id);
  rr_cancel_all_forbidden_cell_timers(gas_id);
  (void)rr_timer_cancel(RR_CELL_SELECTION_ABORT_TIMER, gas_id);
} /* rr_cancel_idle_timers */

/**
 * Takes all the necessary actions when a
 * RR_STOP_GSM_MODE_REQ message is received
 *
 * @param rr_deact_gsm indicates if GSM Need to be deactivated.
 *        (STOP_GSM_MODE_REQ to be sent to L1 )
 * @return event to be processed by RR control state machine
 */
static rr_event_T rr_process_rr_stop_gsm_mode_req(
  boolean rr_deact_gsm,
  const gas_id_t gas_id
)
{
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  /* Note the cause of the shut-down, so the correct reponse is sent to NAS */
  /* when the shut-down is complete */
  set_rr_deact_reason(RR_DEACT_REASON_STOP_GSM_MODE_REQ, gas_id);

  /* reset any saved messages (MM and pages) */
  rr_delete_saved_msg(gas_id);
  rr_ce_discard_saved_pages(gas_id);

  /* cancel all the idle timers */
  rr_cancel_idle_timers(gas_id);

#ifdef FEATURE_SGLTE
  if (rr_sglte_internal_stop_gsm_being_processed(gas_id))
  {
    MSG_GERAN_HIGH_0_G("Deactivation due to SRVCC HO hence presenrve Serving cell System Information");
  }
  else
#endif /* FEATURE_SGLTE */
  {
    /* RR is no longer camped, reset the system information database */
    rr_clear_system_information(&scell_info_ptr->gsm);
  }
  scell_info_ptr->gsm.camped_on_flag = FALSE;

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
    rr_set_rr_using_pseudo_si2q_data(FALSE, gas_id);
#endif

  /* reset the surrounding database as well */
  rr_reset_surrounding_database(gas_id);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
  /*************************************************************/
  /* Initialise the wcdma surrounding cell data bse            */
  /* Also Reset the Rejected wcdma Cell data base,             */
  /* rejected wcdma threshold store.                           */
  /*************************************************************/
  rr_g2wr_reset_wcdma_surrounding_database(gas_id);
  rr_g2wr_cancel_all_wcdma_five_second_timers(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
  rr_g2x_barred_frequency_db_clear(gas_id);
  rr_g2x_barred_cell_db_clear(gas_id);
  rr_g2x_cell_quality_data_clear(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS*/

  /**********************************************************************************/
  /* reset all the RR state machines since RR_STOP_GSM_MODE_REQ aborts any ongoing  */
  /* processing without reseting the state machines                                 */
  /**********************************************************************************/
  (void) rr_selection_control(EV_RESET_SOFTWARE, NULL, gas_id);
  (void) rr_plmn_list_control(EV_RESET_SOFTWARE, NULL, gas_id);
  rr_initialise_forbidden_cell_list(gas_id);

  (void) rr_connection_establishment_control(EV_RESET_SOFTWARE, NULL, gas_id);

  rr_reset_current_band(gas_id);

  /* Take L1 out of MM Non-drx mode if necessary.  This must be done before
     rr_gprs_initialise() is called as this clears the status flag. */
  rr_gprs_clear_l1_in_mm_non_drx(gas_id);

  /* PS access is already blocked, but ensure any remaining PDUs are deleted */
  rr_ps_no_ps_access_del_all(gas_id);

  rr_stop_t3122(gas_id);

#ifdef FEATURE_LTE
  rr_control_data_ptr->g2l_resel_allowed = TRUE;
#endif /* FEATURE_LTE */

  // If a Stop Mode req. arrives while trying to Activate mode,
  // start deactivation procedure and goto RR_GOING_INACTIVE
  if(rr_deact_gsm)
  {
    return rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
  }
  else
  {
    return EV_NO_EVENT;
  }
} /* rr_process_rr_stop_gsm_mode_req */

/**
 * Gets bplmn_failed_id from the rr_control_data db.
 *
 * @param  gas_id : access stratum identifier
 *
 * @return void
 *
 */
byte rr_ms_get_bplmn_failed_id(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr;
  rr_control_data_ptr = rr_control_get_data_ptr(gas_id);

  if(rr_control_data_ptr != NULL)
  {
    return (rr_control_data_ptr->bplmn_fail_id);
  }
  else
  {
    return(BPLMN_INVALID_TRANS_ID);
  }
}

/**
 * Notifies this module that a BPLMN search has failed
 *
 * @param  gas_id : access stratum identifier
 *
 * @return void
 *
 */
void rr_ms_set_bplmn_failed(const gas_id_t gas_id)
{
  rr_PLMN_list_req_info_T *list_req_ptr;
  rr_control_data_t       *rr_control_data_ptr;

  rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  list_req_ptr        = rr_get_plmn_list_req_info_ptr(gas_id);

  if ((rr_control_data_ptr != NULL) && (list_req_ptr != NULL))
  {
    if (list_req_ptr->network_selection_mode != SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
    {
      /* store the transaction ID of the failed manual search */
      rr_control_data_ptr->bplmn_fail_id = list_req_ptr->trans_id;

      MSG_GERAN_ERROR_1_G("Manual BPLMN transaction ID %d failed",
                           (int)(list_req_ptr->trans_id));
    }
    else
    {
      MSG_GERAN_HIGH_1_G("Automatic BPLMN transaction ID %d failed, expect NAS retry",
                          (int)(list_req_ptr->trans_id));
    }
  }

  return;
}

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/**
 * Update Downlink Signalling Counter during BPLMN in DSDS mode
 *
 * @param  block_quality: GOOD/BAD/UNKNOWN sent by Layer 1
 *
 * @return rr_event_T for main state machine
 *
 */
static rr_event_T rr_ms_bplmn_dsc_check(
  const gl1_block_quality_t block_quality,
  const gas_id_t gas_id
)
{
  rr_event_T bplmn_event = EV_NO_EVENT;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (block_quality == GL1_BLOCK_QUALITY_GOOD)
  {
    /* increment the BPLMN-specific DSC and bound it to the max */
    rr_control_data_ptr->bplmn_dsc_now += 1;
    if (rr_control_data_ptr->bplmn_dsc_now > rr_control_data_ptr->bplmn_dsc_max)
    {
      rr_control_data_ptr->bplmn_dsc_now = rr_control_data_ptr->bplmn_dsc_max;
    }
  }
  else if (block_quality == GL1_BLOCK_QUALITY_BAD)
  {
    /* decrement the BPLMN-specific DSC and check for zero */
    rr_control_data_ptr->bplmn_dsc_now -= 4;
  }
  else
  {
    /* unknown block quality, decrement the counter by a smaller amount */
    rr_control_data_ptr->bplmn_dsc_now -= 1;
  }

  MSG_GERAN_HIGH_2_G("BPLMN SHM=%d LBQ=%d", rr_control_data_ptr->bplmn_dsc_now, (int)block_quality);

  /* generate the abort event if the counter falls to zero or below */
  if (rr_control_data_ptr->bplmn_dsc_now <= 0)
  {
    rr_ms_set_bplmn_failed(gas_id);
    bplmn_event = EV_PLMN_SEARCH_ABORT;
    rr_ms_bplmn_initialise_dsc(FALSE, gas_id);
  }

  return(bplmn_event);
}

/**
 * Function: This function determines whether the RR_PLMN_LIST_REQ with
 *           previous transaction ID failed due to a BPLMN DSDS abort
 *
 * @Params:  Incoming inter-task message (new RR_PLMN_LIST_REQ)
 *
 * @Outputs: TRUE if the previous BPLMN search failed
 */
static boolean rr_ms_last_bplmn_failed(rr_plmn_list_req_T *new_req_ptr, const gas_id_t gas_id)
{
  boolean last_failed = FALSE;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  /* check whether any previous attempt failed */
  if (rr_control_data_ptr->bplmn_fail_id != BPLMN_INVALID_TRANS_ID)
  {
    byte next_trans_id;

    /* determine whether the failed search's trans ID */
    /* was the one prior to this new request from NAS */
    next_trans_id = (1 + rr_control_data_ptr->bplmn_fail_id) % 16;

    if (new_req_ptr->trans_id == next_trans_id)
    {
      last_failed = geran_in_multi_standby_mode() ||
                    geran_device_in_svlte_dsda_mode();
    }

    /* reset the failed transaction ID for next time */
    rr_control_data_ptr->bplmn_fail_id = BPLMN_INVALID_TRANS_ID;
  }

  return(last_failed);
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

static rr_event_T rr_plc_block_quality_report(
  rr_cmd_bdy_type *msg_ptr,
  const boolean fg_search,
  const gas_id_t gas_id
)
{
  mph_block_quality_ind_T *bq_ptr    = (mph_block_quality_ind_T *)msg_ptr;
  rr_event_T               plc_event = EV_NO_EVENT;

  if (bq_ptr != NULL)
  {
    gl1_block_quality_t adjusted_bq = bq_ptr->block_quality;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
    if (rr_plc_bplmn_pscan_active(gas_id))
    {
      /* if the BPLMN failure count is non-zero, treat this as a block missed, */
      /* such that searches can gracefully time-out if not accessing the radio */
      if ((adjusted_bq == GL1_BLOCK_QUALITY_GOOD) && (bq_ptr->bplmn_fail_count != 0))
      {
        adjusted_bq = GL1_BLOCK_QUALITY_UNKNOWN;
      }
      plc_event = rr_ms_bplmn_dsc_check(adjusted_bq, gas_id);
    }
    else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
    {
      MSG_GERAN_HIGH_2_G(
        "GL1 reports block quality %d during PLMN search (FG=%1d)",
        (int)adjusted_bq,
        (int)fg_search
      );
    }
  }

  return(plc_event);
}

/**
 * Function: This function pre-processes incoming messsages
 *           while RR is in RR_PLMN_LIST_SEARCH state
 *           - filters out the messages should ignored
 *           - passes the required messages to rr_plmn_list_control()
 *           - generates EV_PLMN_SEARCH_ABORT if the incoming message
 *             should result in abort the PLMN search
 *
 * @Params: the new message
 *
 * @Outputs: EV_NO_EVENT, EV_INPUT_MESSAGE or EV_PLMN_SEARCH_ABORT
 */
static rr_event_T rr_control_plmn_search_state_msg_handler(
  byte msg_set,
  byte msg_id,
  rr_cmd_bdy_type *new_message_ptr,
  const gas_id_t gas_id
)
{
  rr_event_T new_rr_event = EV_NO_EVENT;

  switch(msg_set)
  {
    case MS_RR_L1:
      switch (msg_id)
      {
        case MPH_BLOCK_QUALITY_IND:
          new_rr_event = rr_plc_block_quality_report(
            new_message_ptr,
            rr_plc_search_in_foreground(gas_id),
            gas_id
          );
          break;

        /* Ignore these messages */
        case MPH_SURROUND_MEAS_IND:
        case MPH_SURROUND_UPDATE_FAILURE_IND:
        case MPH_SERVING_IDLE_MEAS_IND:
        case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:

        case MPH_SERVING_AUX_MEAS_IND:

        case MPH_FCCH_SCH_DECODE_IND:
          new_rr_event = EV_NO_EVENT;
          break;

        case MPH_POWER_SCAN_CNF:
        case MPH_DECODE_BCCH_LIST_FAILURE_IND:
        case MPH_DECODE_BCCH_LIST_CNF:
        #ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
        case MPH_INTERRAT_LIST_SRCH_TIME_IND:
        case MPH_SUSPEND_GSM_MODE_CNF:
        case MPH_RESUME_GSM_MODE_CNF:
        case MPH_SUSPEND_GSM_MODE_IND:
        case MPH_RESUME_GSM_MODE_FOR_BPLMN_CNF:
        #endif /* FEATURE_GTOW_BACKGROUND_PLMN_SEARCH */
          new_rr_event = EV_INPUT_MESSAGE;
          break;

        default:
          MSG_GERAN_ERROR_2_G(
            "Unexpected message=(%d,%d) in PLMN_LIST_SEARCH",
            (int)msg_set,
            (int)msg_id
          );
          break;
      }
      break;

    case MS_RR_L2:
      switch (msg_id)
      {
        case DL_UNIT_DATA_IND:
          /* prepare the OTA message from the network for parsing */
          if (rr_prepare_ota_msg_for_parsing(&(new_message_ptr->dl_unit_data_ind), gas_id))
          {
            /* forward the message to connection establishment control for processing */
            new_rr_event = rr_connection_establishment_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
          }
          break;

        default:
          MSG_GERAN_ERROR_2_G(
            "Unexpected message=(%d,%d) in PLMN_LIST_SEARCH",
            (int)msg_set,
            (int)msg_id
          );
          break;
      }
      break;

    case MS_MM_RR:
      switch (msg_id)
      {
        case RR_EST_REQ:
        {
          boolean proceed_with_rr_est_req;
          rr_est_req_T * rr_est_req = (rr_est_req_T *) new_message_ptr;
          if (rr_plc_search_in_foreground(gas_id))
          {
            MSG_GERAN_ERROR_0_G("received RR_EST_REQ while RR is not in service");
            rr_reject_msg(new_message_ptr, gas_id);
          }
          else
          {
            /*
            If the CS establishment is for a Location Update, then abort the ongoing
            PLMN search only if an automatic HPLMN search. If the PLMN search was
            user-initiated then the Location Update is not allowed to interrupt.
            CS establishment for other reasons is allowed to abort the PLMN search,
            regardless of the reason for starting it
            */
            if ( rr_est_req->establishment_cause == (byte)MM_RR_LOCATION_UPDATE )
            {
              proceed_with_rr_est_req = rr_plc_is_automatic_search(gas_id);
            }
            else
            {
              proceed_with_rr_est_req = TRUE;
            }

            if (proceed_with_rr_est_req)
            {
              /* Save the MM message, which will be processed in GRR CAMPED */
              (void)rr_save_msg(new_message_ptr, gas_id);
              new_rr_event = EV_PLMN_SEARCH_ABORT;
            }
            else
            {
              rr_reject_msg(new_message_ptr, gas_id);
            }
          }
          break;
        }

        case RR_PLMN_SELECT_REQ:
        case RR_STOP_GSM_MODE_REQ:
        case RR_PLMN_LIST_REQ:
        case RR_DEACT_REQ:
          if (rr_save_msg(new_message_ptr, gas_id))
          {
            new_rr_event = EV_PLMN_SEARCH_ABORT;
          }
          break;

        case RR_BPLMN_SEARCH_ABORT_REQ:
          /* no need to save this message */
          new_rr_event = EV_PLMN_SEARCH_ABORT;
          break;

        case RR_ABORT_REQ:
          /* It is impossible to have a CS connection during PLMN search */
          rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);
          rr_delete_saved_msg(gas_id);
          break;

        case RR_GMM_GPRS_STATE_CHANGE_REQ:
        {
          break;  // no action required
        }

        default:
          MSG_GERAN_ERROR_2_G(
            "Unexpected message=(%d,%d) in PLMN_LIST_SEARCH",
            (int)msg_set,
            (int)msg_id
          );
          break;
      }
      break;

    case MS_MAC_RR:
      switch(msg_id)
      {
        case MAC_GRR_UL_TBF_REQ:
          if (rr_save_msg(new_message_ptr, gas_id))
          {
            if (rr_plc_get_ps_service_priority(gas_id))
            {
              MSG_GERAN_HIGH_0_G("PS service has higher priority, abort BPLMN search");
              new_rr_event = EV_PLMN_SEARCH_ABORT;
            }
            else
            {
              MSG_GERAN_HIGH_0_G("PS service is disabled during PLMN search");
              rr_delete_saved_msg(gas_id);
            }
          }
          break;

        case MAC_GRR_MAC_STATUS:
        case MAC_GRR_PS_ACCESS_CNF:
        {
          // Use the RR-PS-ACCESS module to control PS access
          rr_ps_access_control(new_message_ptr, gas_id);
          break;
        }

        default:
          break;
      }
      break;


#ifdef FEATURE_WCDMA
    case MS_RRC_RR:
      switch(msg_id)
      {
        case RR_INTERRAT_PLMN_SRCH_REQ:
          rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);
          new_rr_event = EV_INPUT_MESSAGE;
          break;

        case RRC_INTERRAT_PLMN_SRCH_CNF:
        case RRC_INTERRAT_PLMN_SRCH_ABORT_CNF:
          new_rr_event = EV_INPUT_MESSAGE;
          break;

        case RR_INTERRAT_PLMN_SRCH_ABORT_REQ:
          rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);
          new_rr_event = EV_PLMN_SEARCH_ABORT;
          break;

        case RR_INTERRAT_PLMN_SRCH_SUSPEND_IND:
          new_rr_event = EV_PLMN_SEARCH_SUSPEND;
          break;

        default:
          MSG_GERAN_ERROR_2_G(
            "Unexpected message=(%d,%d) in PLMN_LIST_SEARCH",
            (int)msg_set,
            (int)msg_id
          );
          break;
      }
      break;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case MS_TDSRRC_RR:
      switch(msg_id)
      {
        case TDSRR_INTERRAT_PLMN_SRCH_REQ:
          rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);
          new_rr_event = EV_INPUT_MESSAGE;
        break;

        case TDSRRC_INTERRAT_PLMN_SRCH_CNF:
        case TDSRRC_INTERRAT_PLMN_SRCH_ABORT_CNF:
          new_rr_event = EV_INPUT_MESSAGE;
          break;

        case TDSRR_INTERRAT_PLMN_SRCH_ABORT_REQ:
          rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);
          new_rr_event = EV_PLMN_SEARCH_ABORT;
          break;

        case TDSRR_INTERRAT_PLMN_SRCH_SUSPEND_IND:
          new_rr_event = EV_PLMN_SEARCH_SUSPEND;
          break;

        default:
          MSG_GERAN_ERROR_2_G(
            "Unexpected message=(%d,%d) in PLMN_LIST_SEARCH",
            (int)msg_set,
            (int)msg_id
          );
          break;
      }
      break;
#endif /* FEATURE_GSM_TDS */

    case MS_MSGR_RR:
    {
      if (new_message_ptr != NULL)
      {
        switch (new_message_ptr->rr_msgr_msg.message.hdr.id)
        {
#ifdef FEATURE_LTE
          case GERAN_GRR_LTE_PLMN_SRCH_REQ:
            rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);
            new_rr_event = EV_INPUT_MESSAGE;
            break;

          case LTE_RRC_G_PLMN_SRCH_RSP:
          case LTE_RRC_G_ABORT_PLMN_SRCH_RSP:
            new_rr_event = EV_INPUT_MESSAGE;
            break;

          case GERAN_GRR_LTE_ABORT_PLMN_SRCH_REQ:
            rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);
            new_rr_event = EV_PLMN_SEARCH_ABORT;
            break;

          case GERAN_GRR_LTE_SUSPEND_PLMN_SRCH_IND:
          {
            new_rr_event = EV_PLMN_SEARCH_SUSPEND;
            break;
          } /* GERAN_GRR_LTE_SUSPEND_PLMN_SRCH_IND */

          case GERAN_GRR_LTE_GET_PLMN_PRTL_RESULTS_REQ:
          {
            new_rr_event = EV_INPUT_MESSAGE;
            break;
          } /* GERAN_GRR_LTE_GET_PLMN_PRTL_RESULTS_REQ */
#endif /* FEATURE_LTE */

          default:
            MSG_GERAN_ERROR_2_G(
              "Unexpected message=(%d,%d) in PLMN_LIST_SEARCH",
              msg_set,
              msg_id
            );
        }
      }
      break;
    }

    case MS_TIMER:
      new_rr_event = EV_INPUT_MESSAGE;
      break;

    case MS_RR_RR:
    {
      switch (new_message_ptr->rr.header.rr_message_set)
      {
        case RR_IMSG:
        {
          switch (new_message_ptr->rr.header.imh.message_id)
          {
            case RR_IMSG_PS_ACCESS_IND:
            {
              MSG_GERAN_MED_0_G("Ignoring RR_IMSG_PS_ACCESS_IND in state RR_PLMN_LIST_SEARCH");
              break;
            } /* RR_IMSG_PS_ACCESS_IND */

            case RR_IMSG_PS_ACCESS_REQ:
            {
              // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

              // TODO - how to handle this during PLMN search ???

              rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

              // Call the call-back function (if one was supplied)
              CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
              break;
            }

            case RR_IMSG_NO_PS_ACCESS_REQ:
            {
              // No PS access request (could be TLLI unassignment, CS-only)

              // TODO - how to handle this during PLMN search ???

              rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

              // Call the call-back function (if one was supplied)
              CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
              break;
            }

            case RR_IMSG_MSC_CHANGE_IND:
            {
              // TODO - check this
              /*
              Update NAS as required
              RR is camped (from NAS perspective), so a RR_SERVICE_IND can be sent to MM if required
              */
              rr_msc_update(
                TRUE,    // can_send_service_ind
                gas_id   // gas_id
              );
              break;
            }

            case RR_IMSG_GPRS_IA_IND:
            {
              if (rr_plc_get_ps_service_priority(gas_id))
              {
                rr_imsg_gprs_ia_ind_t *imsg = &new_message_ptr->rr.rr.imsg.gprs_ia_ind;

                // Re-post this as RR_GRR_IMSG_GPRS_IA_IND.
                // This is blocked until GRR is in Packet Idle
                rr_grr_send_imsg_gprs_ia_ind(
                  &imsg->packet_ch_desc,
                  &imsg->ia_rest_octets,
                  &imsg->frequency_list,
                  imsg->timing_advance,
                  gas_id
                );

                MSG_GERAN_HIGH_0_G("PS service has higher priority, abort BPLMN search");
                new_rr_event = EV_PLMN_SEARCH_ABORT;
              }
              else
              {
                MSG_GERAN_HIGH_0_G("PS service is disabled during PLMN search!");
              }
              break;
            }

            case RR_IMSG_PAGE_IND:
            {
              rr_imsg_page_ind_t *imsg = &new_message_ptr->rr.rr.imsg.page_ind;

              // Forward the CS/PS page to NAS
              rr_ce_send_page_to_nas(&imsg->page_info, gas_id);
              break;
            }

            case RR_IMSG_PAGE_MODE_IND:
            {
              // Changes in OTA page mode are ignored during BPLMN search
              MSG_GERAN_MED_0_G("Page Mode change ignored");
              break;
            }

            default:
            {
              new_rr_event = EV_INPUT_MESSAGE;
            }
          }
          break;
        } /* RR_IMSG */

        default:
        {
          new_rr_event = EV_INPUT_MESSAGE;
        }
      }
      break;
    } /* MS_RR_RR */

    default:
      MSG_GERAN_ERROR_2_G(
        "Unexpected message=(%d,%d) in PLMN_LIST_SEARCH",
        (int)msg_set,
        (int)msg_id
      );
      break;
  }

  return new_rr_event;
}

boolean rr_x2g_cco_in_progress(
  const gas_id_t gas_id
)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->x2g_resel_type)
  {
#ifdef FEATURE_WCDMA
    case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER: return TRUE;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
    case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER: return TRUE;
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
    case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER: return TRUE;
#endif /* FEATURE_LTE */

    default: return FALSE;
  }
#else /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
  return FALSE;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)

boolean rr_x2g_cco_with_nacc_in_progress(
  const gas_id_t gas_id
)
{
  if (rr_x2g_cco_in_progress(gas_id))
  {
    return rr_x2g_resel_with_nacc_in_progress(gas_id);
  }
  else
  {
    return FALSE;
  }
}

boolean rr_get_x2g_cco_act_rsp_rcvd(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_x2g_cco_in_progress(gas_id))
  {
    MSG_GERAN_HIGH_1_G("X2G CCO: act_rsp_rcvd=%d", rr_control_data_ptr->x2g_cco_act_rsp_rcvd);
    return rr_control_data_ptr->x2g_cco_act_rsp_rcvd;
  }
  else
  {
    return FALSE;
  }
}

void rr_set_x2g_cco_act_rsp_rcvd(
  boolean        act_rsp_rcvd,
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  rr_control_data_ptr->x2g_cco_act_rsp_rcvd = act_rsp_rcvd;
}

boolean rr_x2g_cco_for_csfb_in_progress(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_x2g_cco_in_progress(gas_id))
  {
    return rr_control_data_ptr->is_cco_for_csfb;
  }
  else
  {
    return FALSE;
  }
}

boolean rr_g2x_measurements_allowed(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_x2g_cco_in_progress(gas_id))
  {
    MSG_GERAN_HIGH_0_G("No G2X measurements allowed during X2G CCO");
    return FALSE;
  }
#ifdef FEATURE_GSM_TDS
  else if (rr_control_data_ptr->waiting_for_tdsrr_interrat_tl1_stop_ind)
  {
    MSG_GERAN_HIGH_0_G("No G2X measurements allowed, waiting for TDSRR_INTERRAT_TL1_STOP_IND");
    return FALSE;
  }
#endif /* FEATURE_GSM_TDS */
#ifdef FEATURE_LTE
  else if (rr_control_data_ptr->waiting_for_lte_rrc_cco_nacc_completed_ind)
  {
    MSG_GERAN_HIGH_0_G("No G2X measurements allowed, waiting for LTE_RRC_CCO_NACC_COMPLETED_IND");
    return FALSE;
  }
#endif /* FEATURE_LTE */

#ifdef FEATURE_SGLTE
  else if ((rr_is_sglte_plmn_srch_active(gas_id) == TRUE) &&
           (rr_is_sglte_mode_active(gas_id) == TRUE))
  {
    MSG_GERAN_HIGH_0_G("No G2X measurements allowed, SGLTE BPLMN search active");
    return FALSE;
  }
  else if (rr_sglte_g2l_blind_redirection_active(gas_id))
  {
    MSG_GERAN_HIGH_0_G("No G2X measurements allowed, SGLTE G2L blind redir ongoing");
    return FALSE;
  }
#endif /*FEATURE_SGLTE*/

  else
  {
    return TRUE;
  }
}

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_WCDMA

static void rr_respond_to_wcdma_rrc_request_after_panic_reset(
  rr_cmd_bdy_type * request,
  boolean rr_activation_ind_sent,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(request);

  switch (request->message_header.message_set)
  {
    case MS_RRC_RR:
    {
      switch (request->message_header.message_id)
      {
        case RR_INTERRAT_RESELECTION_REQ:
        {
          rr_send_interrat_reselection_rej(
            RR_GSM_OTHER_REASON,
            0,
            gas_id
          );
          break;
        } /* RR_INTERRAT_RESELECTION_REQ */

        case RR_INTERRAT_RESELECTION_ABORT_REQ:
        {
          rr_send_interrat_reselection_abort_cnf(gas_id);
          break;
        } /* RR_INTERRAT_RESELECTION_ABORT_REQ */

        case RR_INTERRAT_REDIRECT_REQ:
        {
          rr_send_interrat_redirect_rej(
            RR_RRC_REDIRECT_FAIL_OTHER_REASON,
            NULL,
            gas_id
          );
          break;
        } /* RR_INTERRAT_REDIRECT_REQ */

        case RR_INTERRAT_REDIRECT_ABORT_REQ:
        {
          rr_send_interrat_redirect_abort_cnf(RRC_RR_SUCCESS, gas_id);
          break;
        } /* RR_INTERRAT_REDIRECT_ABORT_REQ */

        case RR_INTERRAT_HANDOVER_REQ:
        {
          rr_send_interrat_ho_cnf(RRC_RR_FAILURE, RRC_RR_INVALID_STATE, NULL, gas_id);
          break;
        } /* RR_INTERRAT_HANDOVER_REQ */

        case RR_INTERRAT_HANDOVER_ABORT_REQ:
        {
          rr_send_interrat_ho_abort_cnf(RRC_RR_SUCCESS, gas_id);
          break;
        } /* RR_INTERRAT_HANDOVER_ABORT_REQ */

        case RR_INTERRAT_CC_ORDER_REQ:
        {
          if (rr_get_x2g_cco_act_rsp_rcvd(gas_id))
          {
            rr_send_interrat_cc_order_cnf(
              CCO_FAILURE_AFTER_ACT_IND,
              RR_GSM_CCO_OTHER_REASON,
              gas_id
            );
          }
          else
          {
            rr_send_interrat_cc_order_cnf(
              CCO_FAILURE_BEFORE_ACT_IND,
              RR_GSM_CCO_OTHER_REASON,
              gas_id
            );
          }
          rr_set_x2g_cco_act_rsp_rcvd(FALSE, gas_id);

#ifdef FEATURE_LTE
          rr_individual_priorities_clear(gas_id);
#endif /* FEATURE_LTE */
          break;
        } /* RR_INTERRAT_CC_ORDER_REQ */

        case RR_INTERRAT_CC_ORDER_ABORT_REQ:
        {
          rr_send_interrat_cc_order_abort_cnf(RRC_RR_SUCCESS, gas_id);
          break;
        } /* RR_INTERRAT_CC_ORDER_ABORT_REQ */

        case RR_INTERRAT_PLMN_SRCH_REQ:
        {
          rr_send_interrat_plmn_search_cnf(TRUE, FALSE, NULL, gas_id);
          break;
        } /* RR_INTERRAT_PLMN_SRCH_REQ */

        case RR_INTERRAT_PLMN_SRCH_ABORT_REQ:
        {
          rr_send_interrat_plmn_search_abort_cnf(gas_id);
          break;
        } /* RR_INTERRAT_PLMN_SRCH_ABORT_REQ */

        default:
        {
          MSG_GERAN_ERROR_1_G(
            "Unexpected message: %d",
            request->message_header.message_id
          );
        }
      }
      break;
    } /* MS_RRC_RR */

    default:
    {
      MSG_GERAN_ERROR_1_G(
        "Unexpected message set: %d",
        request->message_header.message_set
      );
    }
  }
}

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE

static void rr_respond_to_lte_rrc_request_after_panic_reset(
  rr_cmd_bdy_type * request,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(request);

  switch (request->message_header.message_set)
  {
    case MS_MSGR_RR:
    {
      switch (request->rr_msgr_msg.message.hdr.id)
      {
        case GERAN_GRR_LTE_RESEL_REQ:
        {
          geran_grr_lte_resel_failed_rsp_t lte_resel_failed_rsp;
          errno_enum_type                  result;

          memset(&lte_resel_failed_rsp, 0, sizeof(lte_resel_failed_rsp));

          msgr_init_hdr(
            &lte_resel_failed_rsp.hdr,
            MSGR_GERAN_GRR,
            GERAN_GRR_LTE_RESEL_FAILED_RSP
          );
          lte_resel_failed_rsp.resel_fail_cause = GERAN_GRR_LTE_RESEL_FAILURE_CAUSE_UNDEFINED;
          result = rr_msgr_send(&lte_resel_failed_rsp.hdr, sizeof(lte_resel_failed_rsp), gas_id);
          if (result != E_SUCCESS)
          {
            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
          }
          break;
        } /* GERAN_GRR_LTE_RESEL_REQ */

        case GERAN_GRR_LTE_ABORT_RESEL_REQ:
        {
          msgr_hdr_struct_type lte_abort_resel_rsp;
          errno_enum_type      result;

          memset(&lte_abort_resel_rsp, 0, sizeof(lte_abort_resel_rsp));

          msgr_init_hdr(
            &lte_abort_resel_rsp,
            MSGR_GERAN_GRR,
            GERAN_GRR_LTE_ABORT_RESEL_RSP
          );
          result = rr_msgr_send(&lte_abort_resel_rsp, sizeof(lte_abort_resel_rsp), gas_id);
          if (result != E_SUCCESS)
          {
            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
          }
          break;
        } /* GERAN_GRR_LTE_ABORT_RESEL_REQ */

        case GERAN_GRR_LTE_REDIR_REQ:
        {
          rr_x2g_send_lte_redir_failure(gas_id);
          break;
        } /* GERAN_GRR_LTE_REDIR_REQ */

        case GERAN_GRR_LTE_ABORT_REDIR_REQ:
        {
          msgr_hdr_struct_type lte_abort_redir_rsp;
          errno_enum_type      result;

          memset(&lte_abort_redir_rsp, 0, sizeof(lte_abort_redir_rsp));

          msgr_init_hdr(
            &lte_abort_redir_rsp,
            MSGR_GERAN_GRR,
            GERAN_GRR_LTE_ABORT_REDIR_RSP
          );

          result = rr_msgr_send(&lte_abort_redir_rsp, sizeof(lte_abort_redir_rsp), gas_id);
          if (result != E_SUCCESS)
          {
            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
          }
          break;
        } /* GERAN_GRR_LTE_ABORT_REDIR_REQ */

        case GERAN_GRR_LTE_PLMN_SRCH_REQ:
        {
          lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
          errno_enum_type          result;

          plmn_srch_rsp_ptr = (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));
          RR_NULL_CHECK_RETURN_VOID(plmn_srch_rsp_ptr );

          msgr_init_hdr(
            &plmn_srch_rsp_ptr->msg_hdr,
            MSGR_GERAN_GRR,
            GERAN_GRR_LTE_PLMN_SRCH_RSP
          );

          plmn_srch_rsp_ptr->srch_done = TRUE;
          plmn_srch_rsp_ptr->hplmn_found = FALSE;

          rr_invalidate_csg_info(&plmn_srch_rsp_ptr->available_plmn_list);

          result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);
          if (result != E_SUCCESS)
          {
            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
          }
          GPRS_MEM_FREE(plmn_srch_rsp_ptr);
          break;
        } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */

        case GERAN_GRR_LTE_ABORT_PLMN_SRCH_REQ:
        {
          lte_irat_abort_plmn_srch_rsp_s abort_plmn_srch_rsp;
          errno_enum_type                result;

          memset(&abort_plmn_srch_rsp, 0, sizeof(abort_plmn_srch_rsp));

          msgr_init_hdr(
            &abort_plmn_srch_rsp.msg_hdr,
            MSGR_GERAN_GRR,
            GERAN_GRR_LTE_ABORT_PLMN_SRCH_RSP
          );

          result = rr_msgr_send(&abort_plmn_srch_rsp.msg_hdr, sizeof(abort_plmn_srch_rsp), gas_id);
          if (result != E_SUCCESS)
          {
            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
          }
          break;
        } /* GERAN_GRR_LTE_ABORT_PLMN_SRCH_REQ */

        case GERAN_GRR_LTE_GET_CGI_REQ:
        {
          geran_grr_lte_get_cgi_rsp_t lte_get_cgi_rsp;
          errno_enum_type             result;

          memset(&lte_get_cgi_rsp, 0, sizeof(lte_get_cgi_rsp));

          msgr_init_hdr(
            &lte_get_cgi_rsp.msg_hdr,
            MSGR_GERAN_GRR,
            GERAN_GRR_LTE_GET_CGI_RSP
          );

          result = rr_msgr_send(&lte_get_cgi_rsp.msg_hdr, sizeof(lte_get_cgi_rsp), gas_id);
          if (result != E_SUCCESS)
          {
            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
          }
          break;
        } /* GERAN_GRR_LTE_GET_CGI_REQ */

        case GERAN_GRR_LTE_ABORT_CGI_REQ:
        {
          geran_grr_lte_abort_cgi_rsp_t lte_abort_cgi_rsp;
          errno_enum_type             result;

          memset(&lte_abort_cgi_rsp, 0, sizeof(lte_abort_cgi_rsp));

          msgr_init_hdr(
            &lte_abort_cgi_rsp.msg_hdr,
            MSGR_GERAN_GRR,
            GERAN_GRR_LTE_ABORT_CGI_RSP
          );

          result = rr_msgr_send(&lte_abort_cgi_rsp.msg_hdr, sizeof(lte_abort_cgi_rsp), gas_id);
          if (result != E_SUCCESS)
          {
            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
          }
          break;
        } /* GERAN_GRR_LTE_ABORT_CGI_REQ */

        case GERAN_GRR_LTE_CCO_REQ:
        {
          geran_grr_lte_cco_rsp_t lte_cco_rsp;
          errno_enum_type         result;

          memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

          msgr_init_hdr(
            &lte_cco_rsp.hdr,
            MSGR_GERAN_GRR,
            GERAN_GRR_LTE_CCO_RSP
          );
          if (rr_get_x2g_cco_act_rsp_rcvd(gas_id))
          {
            lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_RACH;
          }
          else
          {
            lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_ACQUIRE;
          }

          rr_set_x2g_cco_act_rsp_rcvd(FALSE, gas_id);
          result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
          if (result != E_SUCCESS)
          {
            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
          }
          break;
        } /* GERAN_GRR_LTE_CCO_REQ */

        case GERAN_GRR_LTE_ABORT_CCO_REQ :
        {
          msgr_hdr_struct_type lte_abort_cco_rsp;
          errno_enum_type      result;

          memset(&lte_abort_cco_rsp, 0, sizeof(lte_abort_cco_rsp));

          msgr_init_hdr(
            &lte_abort_cco_rsp,
            MSGR_GERAN_GRR,
            GERAN_GRR_LTE_ABORT_CCO_RSP
          );
          result = rr_msgr_send(&lte_abort_cco_rsp, sizeof(lte_abort_cco_rsp), gas_id);
          if (result != E_SUCCESS)
          {
            MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
          }
          break;
        } /* GERAN_GRR_LTE_ABORT_CCO_REQ */

        default:
        {
          MSG_GERAN_ERROR_1_G(
            "Unexpected message: %d",
            request->rr_msgr_msg.message.hdr.id
          );
        }
      }
      break;
    } /* RR_MSGR_RR */

    default:
    {
      MSG_GERAN_ERROR_1_G(
        "Unexpected message set: %d",
        request->message_header.message_set
      );
    }
  }
}

#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS

static void rr_respond_to_tdscdma_rrc_request_after_panic_reset(
  rr_cmd_bdy_type * request,
  boolean rr_activation_ind_sent,
  const gas_id_t gas_id
)
{
  RR_NULL_CHECK_RETURN_VOID(request);

  switch (request->message_header.message_set)
  {
    case MS_TDSRRC_RR:
    {
      switch (request->message_header.message_id)
      {
        case TDSRR_INTERRAT_RESELECTION_REQ:
        {
          rr_send_tds_interrat_reselection_rej(
            TDSRR_GSM_OTHER_REASON,
            0,
            gas_id
          );
          break;
        } /* TDSRR_INTERRAT_RESELECTION_REQ */

        case TDSRR_INTERRAT_RESELECTION_ABORT_REQ:
        {
          rr_send_tds_interrat_reselection_abort_cnf(gas_id);
          break;
        } /* TDSRR_INTERRAT_RESELECTION_ABORT_REQ */

        case TDSRR_INTERRAT_REDIRECT_REQ:
        {
          rr_send_tds_interrat_redirect_rej(
            TDSRR_REDIRECT_FAIL_OTHER_REASON,
            NULL,
            gas_id
          );
          break;
        } /* TDSRR_INTERRAT_REDIRECT_REQ */

        case TDSRR_INTERRAT_REDIRECT_ABORT_REQ:
        {
          rr_send_tds_interrat_redirect_abort_cnf(TDSRRC_RR_SUCCESS, gas_id);
          break;
        } /* TDSRR_INTERRAT_REDIRECT_ABORT_REQ */

        case TDSRR_INTERRAT_HANDOVER_REQ:
        {
          rr_send_tds_interrat_ho_cnf(TDSRRC_RR_FAILURE, TDSRRC_RR_INVALID_STATE, NULL, gas_id);
          break;
        } /* TDSRR_INTERRAT_HANDOVER_REQ */

        case TDSRR_INTERRAT_HANDOVER_ABORT_REQ:
        {
          rr_send_tds_interrat_ho_abort_cnf(TDSRRC_RR_SUCCESS, gas_id);
          break;
        } /* TDSRR_INTERRAT_HANDOVER_ABORT_REQ */

        case TDSRR_INTERRAT_CC_ORDER_REQ:
        {
          if (rr_get_x2g_cco_act_rsp_rcvd(gas_id))
          {
            rr_send_tds_interrat_cc_order_cnf(
              TDSCCO_FAILURE_AFTER_ACT_IND,
              TDSRR_GSM_CCO_OTHER_REASON,
              gas_id
            );
          }
          else
          {
            rr_send_tds_interrat_cc_order_cnf(
              TDSCCO_FAILURE_BEFORE_ACT_IND,
              TDSRR_GSM_CCO_OTHER_REASON,
              gas_id
            );
          }
          rr_set_x2g_cco_act_rsp_rcvd(FALSE, gas_id);
          break;
        } /* TDSRR_INTERRAT_CC_ORDER_REQ */

        case TDSRR_INTERRAT_CC_ORDER_ABORT_REQ:
        {
          rr_send_tds_interrat_cc_order_abort_cnf(TDSRRC_RR_SUCCESS, gas_id);
          break;
        } /* TDSRR_INTERRAT_CC_ORDER_ABORT_REQ */

        case TDSRR_INTERRAT_PLMN_SRCH_REQ:
        {
          rr_send_tds_interrat_plmn_search_cnf(TRUE, FALSE, NULL, gas_id);
          break;
        } /* TDSRR_INTERRAT_PLMN_SRCH_REQ */

        case TDSRR_INTERRAT_PLMN_SRCH_ABORT_REQ:
        {
          rr_send_tds_interrat_plmn_search_abort_cnf(gas_id);
          break;
        } /* TDSRR_INTERRAT_PLMN_SRCH_ABORT_REQ */

        default:
        {
          MSG_GERAN_ERROR_1_G(
            "Unexpected message: %d",
            request->message_header.message_id
          );
        }
      }
      break;
    } /* MS_TDSRRC_RR */

    default:
    {
      MSG_GERAN_ERROR_1_G(
        "Unexpected message set: %d",
        request->message_header.message_set
      );
    }
  }
}

#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE

boolean rr_is_lte_supported(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return rr_control_data_ptr->lte_supported;
}

/**
  @brief Check whether G2L reselection is allowed by NAS or not.

  @return TRUE if G2L reselection is allowed, FALSE otherwise.
*/
boolean rr_g2l_resel_allowed(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return rr_control_data_ptr->g2l_resel_allowed;
}

#endif /* FEATURE_LTE */

/**
  @brief Determines if GRR is operating in SGLTE mode.

  @return TRUE if GRR is operating in SGLTE mode, FALSE if not.
*/
boolean rr_is_sglte_mode_active(const gas_id_t gas_id)
{
  boolean is_sglte_mode = FALSE;
#ifdef FEATURE_SGLTE
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->ue_mode)
  {
    case SYS_UE_MODE_SGLTE_HOME:
    case SYS_UE_MODE_SGLTE_TEST_ONLY:
    {
      is_sglte_mode = TRUE;
      break;
    }

    case SYS_UE_MODE_SGLTE_ROAM:
    case SYS_UE_MODE_CSFB_ONLY:
    case SYS_UE_MODE_NORMAL:
    {
      is_sglte_mode = FALSE;
      break;
    }

    default:
    {
      MSG_GERAN_ERROR_1_G("ue_mode:%d invalid, SGLTE mode disabled", rr_control_data_ptr->ue_mode);
      is_sglte_mode = FALSE;
      break;
    }
  }

  /* output debug where appropriate */
  if (geran_is_multi_mode_gas_id(gas_id))
  {
    if (is_sglte_mode)
    {
      MSG_GERAN_HIGH_1_G("ue_mode: %d, SGLTE mode enabled", rr_control_data_ptr->ue_mode);
    }
    else
    {
      MSG_GERAN_HIGH_1_G("ue_mode: %d, SGLTE mode disabled", rr_control_data_ptr->ue_mode);
    }
  }
  else if (is_sglte_mode)
  {
    MSG_GERAN_ERROR_0_G("Unexpected SGLTE mode setting for non-multimode sub");
  }
#endif /* FEATURE_SGLTE */

  return(is_sglte_mode);
}

#ifdef FEATURE_SGLTE

static void rr_data_transfer_handle_rr_change_mode_ind(
  sys_rat_pri_list_info_s_type    *rat_pri_list_info,
  sys_srv_domain_e_type            service_domain,
  const gas_id_t                   gas_id
)
{
  rr_PLMN_request_details_T * rr_plmn_request_details;
  boolean                     old_gsm_only;
  sys_srv_domain_e_type       old_service_domain;

  RR_NULL_CHECK_RETURN_VOID(rat_pri_list_info);

  rr_plmn_request_details = rr_get_PLMN_request_details_ptr(gas_id);

  old_gsm_only = rr_gsm_only(gas_id);
  old_service_domain = rr_sel_get_service_domain(gas_id);

  rr_store_service_domain(service_domain,gas_id);
  rr_plmn_request_details->rat_pri_list_info = *rat_pri_list_info;

  if (old_gsm_only != rr_gsm_only(gas_id))
  {
    if (rr_gsm_only(gas_id))
    {
      MSG_GERAN_MED_0_G("Switching GL1 to GSM-only mode");
      rr_send_mph_ue_mode_change_req(TRUE, gas_id);

      /* Cancel any IRAT reselection timers that are running. */
      rr_g2wr_reset_wcdma_surrounding_database(gas_id);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
      rr_g2wr_cancel_all_wcdma_five_second_timers(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
    }
    else
    {
      gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

      rr_update_band_preference_for_tdsrrc(rr_band_pref_utran_tdd(gas_id), gas_id);

      MSG_GERAN_MED_0_G("Switching GL1 to multi-RAT mode");
      /* When switching to Multi_RAT mode, track the request, only done for Multi Mode enabling,
      to avoid sending back to back Mode change, caused by User SGLTE split */
      rr_set_pending_mode_change(TRUE, gas_id);

      rr_send_mph_ue_mode_change_req(FALSE, gas_id);

      /* Send MPH_DEDICATED_WCDMA_CELL_UPDATE_LIST_REQ to GL1. */
      if (scell->gsm.wcdma_ded_neighbor_list.valid_data)
      {
        rr_send_mph_dedicated_wcdma_cell_update_list_req(
          NULL,
          &scell->gsm.wcdma_ded_neighbor_list,
          scell->gsm.dedicated_mode_params,
          gas_id
        );
      }
      else if (scell->gsm.utran_neighbor_list.valid_data == TRUE)
      {
        rr_send_mph_dedicated_wcdma_cell_update_list_req(
          &scell->gsm.utran_neighbor_list,
          NULL,
          scell->gsm.dedicated_mode_params,
          gas_id
        );
      }
      else
      {
        rr_send_mph_dedicated_wcdma_cell_update_list_req(
          NULL,
          NULL,
          scell->gsm.dedicated_mode_params,
          gas_id
        );
      }
    }
  }

  if (!rr_cell_supports_dtm(gas_id))
  {
    rr_gmm_suspend_gprs(gas_id);
  }
  else
  { /* Cell Suppports DTM, Resume GPRS, if it was suspended  */
    rr_gmm_resume_gprs(gas_id);
  }

  rr_ps_handle_rr_change_mode_ind_service_domain_changed(
    old_service_domain,
    rr_sel_get_service_domain(gas_id),
    gas_id
   );

 
}

/**
  @brief Check whether serving measurements are currently available.

  This function checks the RR state to determine whether serving cell
  measurements are currently available.  If the state indicates that
  RR is in service, it is assumed that serving cell measurements are valid,
  otherwise they are assumed to be invalid.

  @return TRUE if serving cell measurements are available, FALSE if not.
*/
static boolean rr_sglte_serving_cell_meas_valid(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  boolean            serving_cell_meas_valid;

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_CONNECTION_PENDING:
    case RR_DATA_TRANSFER:
    case RR_CONNECTION_RELEASE:
    case RR_EARLY_CAMPED_WAIT_FOR_SI:
    case RR_GRR:
    {
      serving_cell_meas_valid = TRUE;
      break;
    } /* RR_CONNECTION_PENDING ... */

    default:
    {
      serving_cell_meas_valid = FALSE;
    }
  }
  return serving_cell_meas_valid;
}

boolean rr_is_sglte_x2g_resel_in_progress(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if ((rr_control_data_ptr->rr_state != RR_GRR) &&
      (rr_control_data_ptr->rr_state != RR_DATA_TRANSFER) &&
      (rr_control_data_ptr->x2g_resel_type != RR_X2G_RESEL_TYPE_UNDEFINED))
  {
    return(rr_is_sglte_mode_active(gas_id));
  }

  return(FALSE);
}

boolean rr_sglte_is_full_service_available(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (sys_srv_status_is_full_srv(rr_control_data_ptr->nas_service_status.service_status))
  {
    MSG_GERAN_HIGH_1_G("service_status:%d, full service available",
                       rr_control_data_ptr->nas_service_status.service_status);
    return(TRUE);
  }
  else
  {
    MSG_GERAN_HIGH_1_G("service_status:%d, full service not available",
                       rr_control_data_ptr->nas_service_status.service_status);
  }

  return(FALSE);
}

void rr_sglte_store_serving_cell_info(const gas_id_t gas_id)
{
  uint8 rxlev ;
  rr_measured_signal_data_t * measured_signal_data = rr_get_measured_signal_data_ptr(gas_id);
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);
  RR_NULL_CHECK_FATAL(scell_info_ptr);

  rxlev = measured_signal_data->current_signal_strength;

  rr_control_data_ptr->sglte_store_serv_cell.serving_ARFCN = rr_gprs_get_scell_arfcn(gas_id);

  rr_control_data_ptr->sglte_store_serv_cell.serving_BSIC = rr_gprs_get_scell_bsic(gas_id);

  rr_control_data_ptr->sglte_store_serv_cell.serving_rssi_dBm = rxlev_to_dbm(rxlev) ;

  rr_control_data_ptr->sglte_store_serv_cell.serving_gprs_indicator  = scell_info_ptr->gsm.gprs_indicator;

  MSG_GERAN_HIGH_1_G("SGLTE Serving cell ARFCN:%d",
                     rr_control_data_ptr->sglte_store_serv_cell.serving_ARFCN.num);

  MSG_GERAN_HIGH_2_G("SGLTE Serving cell RSSI:%ddBm and Serving GPRS indicator = %d",
                     rr_control_data_ptr->sglte_store_serv_cell.serving_rssi_dBm,
                     rr_control_data_ptr->sglte_store_serv_cell.serving_gprs_indicator);
}

static void rr_sglte_delete_store_serving_cell_info(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  rr_control_data_ptr->sglte_store_serv_cell.serving_ARFCN.num = 0xFF;
  rr_control_data_ptr->sglte_store_serv_cell.serving_ARFCN.band= SYS_BAND_NONE;
  rr_control_data_ptr->sglte_store_serv_cell.serving_BSIC.PLMN_colour_code = INVALID_BSIC_CC;
  rr_control_data_ptr->sglte_store_serv_cell.serving_BSIC.BS_colour_code =INVALID_BSIC_CC;
  rr_control_data_ptr->sglte_store_serv_cell.serving_rssi_dBm = 0xFFFF ;
  rr_control_data_ptr->sglte_store_serv_cell.serving_gprs_indicator = FALSE;
}

/*===========================================================================

FUNCTION       rr_sglte_get_stored_serv_gprs_ind

DESCRIPTION    Determines Serving cell GPRS capbaility

DEPENDENCIES   None

RETURN VALUE   TRUE if GPRS indicated else FALSE

SIDE EFFECTS   None

===========================================================================*/

boolean rr_sglte_get_stored_serv_gprs_ind(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return rr_control_data_ptr->sglte_store_serv_cell.serving_gprs_indicator;
}


ARFCN_T rr_sglte_get_stored_serv_ARFCN(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return rr_control_data_ptr->sglte_store_serv_cell.serving_ARFCN;
}

BSIC_T rr_sglte_get_stored_serv_BSIC(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return rr_control_data_ptr->sglte_store_serv_cell.serving_BSIC;
}

int16 rr_sglte_get_stored_serv_RSSI(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return rr_control_data_ptr->sglte_store_serv_cell.serving_rssi_dBm;
}

#endif /* FEATURE_SGLTE */

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
/*===========================================================================

FUNCTION       rr_limited_service_in_effect

DESCRIPTION    Determines if NAS service request or status indicates Ltd Svc

DEPENDENCIES   None

RETURN VALUE   TRUE if GRR has been advised of limited service by NAS
SIDE EFFECTS   None

===========================================================================*/
boolean rr_limited_service_in_effect(const gas_id_t gas_id)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  rr_control_data_t         *rr_control_data_ptr         = rr_control_get_data_ptr(gas_id);

  if ((rr_PLMN_request_details_ptr != NULL) &&
      (rr_control_data_ptr         != NULL))
  {
    if ((SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION == rr_PLMN_request_details_ptr->network_select_mode) ||
        (SYS_NETWORK_SELECTION_MODE_LIMITED                     == rr_PLMN_request_details_ptr->network_select_mode))
    {
      MSG_GERAN_HIGH_0_G("Limited Service in last RR_PLMN_SELECT_REQ");
      return(TRUE);
    }
    else if ((SYS_SRV_STATUS_LIMITED_REGIONAL == rr_control_data_ptr->nas_service_status.service_status) ||
             (SYS_SRV_STATUS_LIMITED          == rr_control_data_ptr->nas_service_status.service_status))
    {
      MSG_GERAN_HIGH_0_G("Limited Service in last RR_SERVICE_STATUS_IND");
      return(TRUE);
    }
  }

  return(FALSE);
}
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

/*!
 * \brief Update the module data to reflect the new state, and performs any generic state exit/entry function.
 * 
 * \param new_state 
 */
static void rr_control_transition_to_state(rr_state_T new_state, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);

  // Perform any generic functionality required when exiting a state

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_CONNECTION_RELEASE:
    {
      rr_control_data_ptr->purge_gmm_signalling = FALSE;
      break;
    }

    default: ;
  }

  // Update the module data with the new state
  rr_control_data_ptr->rr_state = new_state;

  // Perform any generic functionality required when entering a state

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_CONNECTION_RELEASE:
    {
      rr_control_data_ptr->purge_gmm_signalling = FALSE;
      break;
    }

    default: ;
  }

  return;
}

/**
 * Handles MPH_SURROUND_UPDATE_FAILURE_IND
 *
 * @Param: MPH_SURROUND_UPDATE_FAILURE_IND message
 *
 * @Param: restart - to restart timer count when SU failure is not due to state change.
 *
 * @Param: resume - to resume sys info handling when SU failure is not due to state change.
 *
 * @param: gas_id
 */
void rr_handle_surround_update_failure_ind(
  mph_surround_update_failure_ind_T *su_fail_ptr,
  const boolean                      restart,
  const boolean                      resume,
  const gas_id_t                     gas_id
)
{
  RR_NULL_CHECK_FATAL(su_fail_ptr);
  
  MSG_GERAN_MED_2_G("Surround Failure for ARFCN=%d, state_changed=%d",
    su_fail_ptr->BCCH_ARFCN.num, su_fail_ptr->state_changed);
  
  if (get_waiting_for_surround_response(gas_id) && 
      match_waiting_for_surround_response(su_fail_ptr->BCCH_ARFCN, gas_id))
  {
    bcch_ncell_info_t *bcch_ncell = rr_gprs_get_surround_ncell_sys_info(su_fail_ptr->BCCH_ARFCN, gas_id);

    set_waiting_for_surround_response_metrics(FALSE, gas_id);
    clear_waiting_for_surround_response(gas_id);
    
    if (su_fail_ptr->state_changed == TRUE)
    {
      if (bcch_ncell != NULL)
      {
        /* Stop the system info timer so a new surround request will be immediate for this cell */
        MSG_GERAN_MED_0_G("system_info_timer_count reset");
        bcch_ncell->system_info_timer_count = UNINITIALISED_SYS_INFO_COUNT;
      }
    }
    else
    {
      if (restart && (bcch_ncell != NULL))
      {
        MSG_GERAN_MED_0_G("system_info_timer_count set to 20 sec");
        bcch_ncell->system_info_timer_count = SYSTEM_INFO_20_SEC_RETRY;
      }
      
      if (resume)
      {
        rr_gprs_resume_camped_sys_info_handling(gas_id);
      }
    }

    rr_log_gprs_surround_search_failure(RR_LOG_GPRS_NCELL_NOT_FOUND_BY_L1, gas_id);
  }
}

/**
 * This is the control of the RR Main State Machine
 *
 * @param rr_event    An event to the state machine
 *
 * @param new_message Pointer to an input message (only valid if event is EV_INPUT_MESSAGE)
 */
void rr_control(rr_event_T rr_event, rr_cmd_bdy_type* new_message_ptr, const gas_id_t gas_id)
{
  byte                        message_id;
  byte                        message_set;
  rr_abort_req_T              rr_rr_abort_req;
 #ifdef FEATURE_SGLTE
  rr_stop_gsm_mode_req_T      rr_rr_stop_gsm_mode_req;
 #endif /*FEATURE_SGLTE*/
  rr_timer_id_T               timer_id;
  rr_event_T                  new_rr_event = EV_NO_EVENT;
  rr_PLMN_request_details_T * rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);


  //gprs_scell_info_t         * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  //gprs_scell_info_t         * pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
  /*These are moved into the individual states as needed, as these calls can cause heap allocation 
    This happens during going inactive sequence due to final STOP_WCDMA/TDS/etc handshake, which
    is after rr_init_offline()has been called to free any allocated heap */

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  /* If EV_INPUT_MESSAGE is passed in, message_set & message_id will be setup from */
  /* new_message_ptr and EV_INPUT_MESSAGE returned. If new_message_ptr, message_set */
  /* or message_id are NULL, then EV_NO_EVENT will be returned */
  /* If any other event is passed in, this is simply returned */
  rr_event = rr_extract_message_header_details(
    rr_event,
    new_message_ptr,
    &message_set,
    &message_id
  );

  /* loop on the current input message and all generated events until no more events remain */
  while (rr_event != EV_NO_EVENT)
  {
    /* If EV_INPUT_MESSAGE is passed in, message_set & message_id will be setup from */
    /* new_message_ptr and EV_INPUT_MESSAGE returned. If new_message_ptr, message_set */
    /* or message_id are NULL, then EV_NO_EVENT will be returned */
    /* If any other event is passed in, this is simply returned */
    rr_event = rr_extract_message_header_details(
      rr_event,
      new_message_ptr,
      &message_set,
      &message_id
    );

    /*****************************************************************/
    /* Handle some input messages in a state independent manner      */
    /* If the rr_event is changed to EV_NO_EVENT, the state handlers */
    /* will not be invoked                                           */
    /*****************************************************************/
#ifdef FEATURE_SGLTE
    /*Rejecting NAS messages during internal shutdown.*/
    if (rr_sglte_internal_stop_gsm_being_processed(gas_id))
    {
       if (rr_event == EV_INPUT_MESSAGE &&
           message_set == MS_MM_RR)
       {
         switch (message_id)
         {
           case RR_PLMN_LIST_REQ:
           case RR_PLMN_SELECT_REQ:
           case RR_EST_REQ:
           {
             MSG_GERAN_HIGH_0_G("Reject NAS message rcvd during intrnl shutdown");
             rr_reject_nas_msg_during_intrn_shutdown(new_message_ptr,gas_id);
             rr_event = EV_NO_EVENT;
           }
         }/*End of Switch*/
       }/*End of rr_event == EV_INPUT_MESSAGE*/
    } /*End of stop_gsm_being_processed*/

    /* SGLTE PLMN List Search occurs in parallel with normal RR operation so
       it is handled in a stateless manner*/
    if (rr_control_data_ptr->sglte_g2x_plmn_list_search_in_progress == TRUE)
    {
      if (rr_event == EV_INPUT_MESSAGE &&
          message_set == MS_MM_RR)
      {
        switch (message_id)
        {
          case RR_DEACT_REQ:
          case RR_STOP_GSM_MODE_REQ:
          case RR_PLMN_LIST_REQ:
          {
            MSG_GERAN_HIGH_1_G("RR SGLTE BPLMN MM deact rxd msg_id:%d, abort search",message_id);
            rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);
            (void)rr_save_msg(new_message_ptr, gas_id);
            rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause = RR_SGLTE_PLMN_SEARCH_ABORT_MM_DEATIVATION;
            rr_event = rr_plc_sglte_g2x_control(EV_PLMN_SEARCH_ABORT,new_message_ptr, gas_id);
            break;
          }
          case RR_BPLMN_SEARCH_ABORT_REQ:
          {
            MSG_GERAN_HIGH_1_G("RR SGLTE BPLMN MM deactivation rxd msg_id:%d, abort search",message_id);
            if(rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause == RR_SGLTE_PLMN_SEARCH_ABORT_EV_GSM_MODE_DEACT)
            {
              MSG_GERAN_HIGH_0_G("RR SGLTE GSM deactivated and abort search already sent to target RAT");
            }
            else
            {
              MSG_GERAN_HIGH_1_G("RR SGLTE BPLMN MM deactivation rxd msg_id:%d, abort search",message_id);
              rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);
              (void)rr_save_msg(new_message_ptr, gas_id);
              rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause = RR_SGLTE_PLMN_SEARCH_ABORT_MM_DEATIVATION;
              rr_event = rr_plc_sglte_g2x_control(EV_PLMN_SEARCH_ABORT,new_message_ptr, gas_id);
            }
            break;
          }
        }
      }
      else if ((rr_event == EV_INPUT_MESSAGE) && (message_set == MS_TIMER) )
      {
          if ( (new_message_ptr->mid_timer_expiry.timer_id) == RR_PLMN_LIST_SEARCH_GUARD_TIMER )
          {
            MSG_GERAN_HIGH_0_G("RR_PLMN_LIST_SEARCH_GUARD_TIMER expired, abort search");
            rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause = RR_SGLTE_PLMN_SEARCH_ABORT_EV_GUARD_TIMER_EXPIRY;
            rr_event = rr_plc_sglte_g2x_control(EV_PLMN_SEARCH_ABORT, new_message_ptr, gas_id);
          }
      }
      else if ((rr_event == EV_GSM_MODE_DEACTIVATED) &&
               (rr_control_data_ptr->rr_state == RR_INACTIVE ||rr_control_data_ptr->rr_state == RR_GOING_INACTIVE))
      {
        MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN EV_GSM_MODE_DEACT rxd, abort search");
        rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause = RR_SGLTE_PLMN_SEARCH_ABORT_EV_GSM_MODE_DEACT;
        rr_event = rr_plc_sglte_g2x_control(EV_PLMN_SEARCH_ABORT_NO_MULTIRAT_ACT,new_message_ptr, gas_id);
      }
      else if (rr_event == EV_INPUT_MESSAGE &&
               message_set == MS_RR_RR &&
               message_id == RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ)
      {
        MSG_GERAN_HIGH_0_G("RR SGLTE SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ rxd, abort search");
        rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause = RR_SGLTE_PLMN_SEARCH_ABORT_EV_SWITCH_TO_CS_ONLY;
        rr_event = rr_plc_sglte_g2x_control(EV_PLMN_SEARCH_ABORT,new_message_ptr, gas_id);
      }
      else if (rr_event == EV_INPUT_MESSAGE &&
               message_set == MS_RR_RR &&
               message_id == RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ)
      {
        MSG_HIGH("RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ rxd, abort search",0,0,0);
        rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause = RR_SGLTE_PLMN_SEARCH_ABORT_EV_SWITCH_TO_USER_CS_ONLY;
        rr_event = rr_plc_sglte_g2x_control(EV_PLMN_SEARCH_ABORT,new_message_ptr, gas_id);
      }
      else
      {
        rr_event = rr_plc_sglte_g2x_control(rr_event,new_message_ptr, gas_id);
      }

      if (rr_event == EV_PLMN_SEARCH_COMPLETE)
      {
        MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN search complete sglte_g2x_plmn_list_search_in_progress=0");
        rr_control_data_ptr->sglte_g2x_plmn_list_search_in_progress = FALSE;

        if (rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause == RR_SGLTE_PLMN_SEARCH_ABORT_MM_DEATIVATION)
        {
          rr_event_T  saved_event = rr_process_saved_message(&new_message_ptr, gas_id);

          rr_event = rr_extract_message_header_details(  saved_event,
                                                         new_message_ptr,
                                                        &message_set,
                                                        &message_id );
          MSG_GERAN_HIGH_1_G("RR SGLTE BPLMN send saved MM msg id:%d",message_id);

          if (rr_event == EV_INPUT_MESSAGE &&
              message_set == MS_MM_RR &&
              message_id == RR_PLMN_LIST_REQ)
          {
            /*handle subsequent request for SGLTE BPLMN search*/
            if (rr_start_sglte_plmn_list_search(((rr_plmn_list_req_T *)new_message_ptr), gas_id))
            {
              MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN sglte search restarted");
              rr_control_data_ptr->sglte_g2x_plmn_list_search_in_progress = TRUE;
              rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause = RR_SGLTE_PLMN_SEARCH_ABORT_NONE;
              rr_start_plmn_list_search((rr_plmn_list_req_T *)new_message_ptr, TRUE, gas_id);
              rr_event = rr_plc_sglte_g2x_control(EV_PLMN_SEARCH_START,new_message_ptr, gas_id);

              MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN sglte_g2x_plmn_list_search_in_progress=1");
              rr_control_data_ptr->sglte_g2x_plmn_list_search_in_progress = TRUE;
            }
            else
            {
              rr_plmn_list_req_T *plmn_list_req_ptr = &new_message_ptr->rr_plmn_list_req;
              if (plmn_list_req_ptr->service_search == SYS_SERVICE_SEARCH_IRAT_PLMN)
              {
                MSG_GERAN_HIGH_0_G("RR_PLMN_LIST_REQ rxd in SGLTE mode does not meet Acceptance Criterion, reject RR_PLMN_LIST_REQ");
                rr_send_plmn_list_cnf(
                          NULL,
                          MM_AS_ABORTED,
                          &new_message_ptr->rr_plmn_list_req,
                          gas_id
                        );
                rr_event = EV_NO_EVENT;
              }
            }
          }

          if (rr_event == EV_INPUT_MESSAGE &&
              message_set == MS_MM_RR &&
              message_id == RR_BPLMN_SEARCH_ABORT_REQ)
          {
            rr_event = EV_NO_EVENT; /*Abort is handled by the SGLTE BPLMN state machine*/
          }
        }
        else
        {
          rr_cmd_bdy_type *new_message_ptr = NULL;
          rr_event_T rr_saved_event;
          
          rr_saved_event = rr_examine_saved_message( &new_message_ptr, gas_id );
          
          /* Look for the expected saved RR_BPLMN_SEARCH_ABORT_REQ that may have arrived prior to reception of these events */
          if ( (rr_saved_event == EV_INPUT_MESSAGE) &&
                (new_message_ptr != NULL) &&
               (new_message_ptr->message_header.message_set == MS_MM_RR) &&
               (new_message_ptr->message_header.message_id == (byte) RR_BPLMN_SEARCH_ABORT_REQ) )
          {
            MSG_HIGH("RR SGLTE BPLMN THROW AWAY SAVED BPLMN SEARCH ABORT",0,0,0);
            grr_throwaway_saved_message(gas_id);
          }
            
          if (rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause == RR_SGLTE_PLMN_SEARCH_ABORT_EV_GSM_MODE_DEACT)
        {
          MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN send saved EV_GSM_MODE_DEACTIVATED");
          rr_event = EV_GSM_MODE_DEACTIVATED;
        }
        else if (rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause == RR_SGLTE_PLMN_SEARCH_ABORT_EV_SWITCH_TO_CS_ONLY)
        {
          MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN deactivated send RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ");
          rr_event = EV_NO_EVENT;
          rr_send_imsg(RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ, gas_id);
        }
        else if (rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause == RR_SGLTE_PLMN_SEARCH_ABORT_EV_SWITCH_TO_USER_CS_ONLY)
        {
          MSG_HIGH("RR SGLTE BPLMN deactivated pass on saved RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ",0,0,0);
          rr_event = EV_NO_EVENT;
          rr_send_imsg(RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ, gas_id);
        }
      }
    }
    }
    else
    {
      if (rr_is_sglte_mode_active(gas_id))
      {
        if (rr_event == EV_INPUT_MESSAGE &&
            message_set == MS_MM_RR &&
            message_id == RR_PLMN_LIST_REQ)
        {
          if (rr_start_sglte_plmn_list_search(((rr_plmn_list_req_T *)new_message_ptr), gas_id))
          {
             MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN sglte_g2x_plmn_list_search_in_progress=1");
             rr_control_data_ptr->sglte_g2x_plmn_list_search_in_progress = TRUE;
             rr_control_data_ptr->sglte_g2x_plmn_list_search_abort_cause = RR_SGLTE_PLMN_SEARCH_ABORT_NONE;
             rr_start_plmn_list_search((rr_plmn_list_req_T *)new_message_ptr, TRUE, gas_id);
             rr_event = rr_plc_sglte_g2x_control(EV_PLMN_SEARCH_START,new_message_ptr, gas_id);
          }
          else
          {
            rr_plmn_list_req_T *plmn_list_req_ptr = &new_message_ptr->rr_plmn_list_req;
            if (plmn_list_req_ptr->service_search == SYS_SERVICE_SEARCH_IRAT_PLMN)
            {
              MSG_GERAN_HIGH_0_G("RR_PLMN_LIST_REQ rxd in SGLTE mode does not meet Acceptance Criterion, reject RR_PLMN_LIST_REQ");
              rr_send_plmn_list_cnf(
                        NULL,
                        MM_AS_ABORTED,
                        &new_message_ptr->rr_plmn_list_req,
                        gas_id
                      );
              rr_event = EV_NO_EVENT;
            }
          }
        }
      }
    }
#endif /*FEATURE_SGLTE*/

    if (rr_event == EV_INPUT_MESSAGE)
    {
      if (message_set == MS_MM_RR)
      {
        switch ((mm_rr_message_id_enum_T) message_id)
        {
          case RR_DEACT_REQ:
          {
            /* Always clear pending mode change data on RR_DEACT_REQ so we
            do not re-use it next time GERAN stack is activated. */
            rr_pending_mode_change_data_clear(gas_id);

            rr_event = rr_process_rr_deact_req(new_message_ptr, gas_id);
            break;
          } /* RR_DEACT_REQ */

          case RR_STOP_GSM_MODE_REQ:
          {
            /* Always clear pending mode change data on RR_STOP_GSM_MODE_REQ so
            we do not re-use it next time GERAN stack is activated. */
            rr_pending_mode_change_data_clear(gas_id);

            /*********************************************************************/
            /* RR_STOP_GSM_MODE_REQ indicates that GSM mode is being deactivated */
            /* Remember the GSM Stop Mode reason.                                */
            /*********************************************************************/
            if ((new_message_ptr->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_MM_DEEP_SLEEP) ||
                (new_message_ptr->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_MM_DUAL_SWITCH))
            {
              rr_l1_gsm_stop_reason_set(RR_L1_DEEP_SLEEP, gas_id);
              rr_notify_deep_sleep_to_other_rats(TRUE, gas_id);
            }
            else
            {
              rr_l1_gsm_stop_reason_set(RR_L1_RAT_CHANGE, gas_id);
            }

#ifdef FEATURE_SGLTE
            if ( (new_message_ptr->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_LTOG_CCO_FAILURE) &&
                  rr_is_sglte_mode_active(gas_id) &&
                 (rr_PLMN_request_details_ptr->service_domain == SYS_SRV_DOMAIN_CS_ONLY)
               )
            {
              MSG_GERAN_HIGH_0_G("RR_STOP_GSM_MODE_REQ(RR_LTOG_CCO_FAILURE) - already in SYS_SRV_DOMAIN_CS");
              rr_send_stop_gsm_mode_cnf(MM_AS_SUCCESS, gas_id);
              grr_update_mm_with_rr_service_ind(gas_id);
              rr_event = EV_NO_EVENT;
            }
#endif /* FEATURE_SGLTE */

            MSG_GERAN_HIGH_0_G("Deactivating GSM mode");
            break;
          } /* RR_STOP_GSM_MODE_REQ */

          case RR_FORBIDDEN_LAI_UPDATE_REQ:
          {
            /***************************************************************************/
            /* handle the RR_FORBIDDEN_LAI_UPDATE_REQ message in a state independent   */
            /* manner. Just replace the existing forbidden LAI list with the new value */
            /* provided by the MM                                                      */
            /***************************************************************************/
            MSG_GERAN_HIGH_0_G("Updating forbidden LAI list from MM");
            rr_store_forbidden_lai_list(
              (rr_forbidden_lai_update_req_T *) new_message_ptr,
              gas_id
            );

            /* since the message was already handled, there is no further event to process */
            rr_event = EV_NO_EVENT;
            break;
          } /* RR_FORBIDDEN_LAI_UPDATE_REQ */

          case RR_EQ_PLMN_LIST_CHANGE_IND:
          {
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) | defined(FEATURE_GSM_TDS)
            rr_g2x_equivalent_plmn_list_updated(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

            rr_event = EV_NO_EVENT;
            break;
          } /* RR_EQ_PLMN_LIST_CHANGE_IND */

          case RR_NW_SEL_MODE_RESET_IND:
          {
            rr_handle_nw_sel_mode_reset_ind(
              &(new_message_ptr->rr_nw_sel_mode_reset_ind),
              gas_id
            );
            rr_event = EV_NO_EVENT;
            break;
          } /* RR_NW_SEL_MODE_RESET_IND */

#ifdef FEATURE_LTE
          case RR_LTE_RESELECTION_ALLOWED_STATUS_IND:
          {
            boolean g2l_resel_allowed = new_message_ptr->mm.rr_lte_reselection_allowed_status_ind.lte_reselection_allowed;

            if (rr_control_data_ptr->g2l_resel_allowed != g2l_resel_allowed)
            {
              rr_control_data_ptr->g2l_resel_allowed = g2l_resel_allowed;
              MSG_GERAN_HIGH_1_G("Reselection to LTE allowed: %d", g2l_resel_allowed);
              /* Send an indication to ourselves that the g2l_resel_allowed
              flag has been updated.  This will trigger an update of GRR if
              necessary. */
              rr_send_imsg(RR_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND, gas_id);
            }

            rr_event = EV_NO_EVENT;
            break;
          } /* RR_LTE_RESELECTION_ALLOWED_STATUS_IND */

          case RR_CSFB_CALL_STATUS_IND:
          {
            boolean is_csfb_call_in_progress = new_message_ptr->mm.rr_csfb_call_status_ind.csfb_call_status;
            boolean csmt = new_message_ptr->mm.rr_csfb_call_status_ind.csmt;


            MSG_GERAN_HIGH_3_G("RR_CSFB_CALL_STATUS_IND changed: %d -> %d, MT call status= %d",
                               rr_control_data_ptr->is_csfb_call_in_progress,
                               is_csfb_call_in_progress,
                               csmt);

            if (is_csfb_call_in_progress)
            {
              /**
               * In the case of CSFB MT connection establishment, the network page will be received by LTE-RRC
               * rather than RR. Therefore channel_needed, which is included in the paging message and needs
               * to be reflected in the RACH content, is not known. 'ANY CHANNEL' should be used in this case.
               */
              rr_ce_store_cs_page_info(
                NULL,          // identity
                ANY_CHANNEL,   // channel_needed
                gas_id
              );

#ifdef FEATURE_QSH_EVENT_METRIC
              rr_qsh_metric_log_csfb(gas_id);
#endif // FEATURE_QSH_EVENT_METRIC
            }

            rr_control_data_ptr->is_csfb_call_in_progress = is_csfb_call_in_progress;
            rr_control_data_ptr->csmt = csmt;
            rr_event = EV_NO_EVENT;

            if (rr_g2l_blind_redir_after_srvcc_enabled(gas_id))
            {
               rr_connection_release_reset_g2l_blind_redir_flag(gas_id);
               MSG_GERAN_HIGH_0_G("SRVCC enabled but no G2L blind as Ultraflash in progress.");
            }

            /* Ultra Flash CSFB from GERAN prospective is the reception of RR_CSFB_CALL_STATUS_IND after SRVCC*/
            /* HO, so if this meesage is received in connected mode we should check the flag for FRLTE so that FRLTE occurs */
            /* in case of UltraFlash CSFB case*/			
            if ((get_rr_state(gas_id)== RR_DATA_TRANSFER) && (rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(gas_id)))
            {
              MSG_GERAN_HIGH_0_G("UltraFlasg CSFB in progress set the flag for FRLTE");            
              rr_connection_release_set_g2l_blind_redir_flag(gas_id);
            }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
            /* notify the Priority Inversion Manager of the new status */
            rr_ms_csfb_status_changed(is_csfb_call_in_progress, gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
            break;
          } /* RR_CSFB_CALL_STATUS_IND */
#endif /* FEATURE_LTE */

          case RR_PLMN_SELECT_REQ:
          {
            MSG_GERAN_HIGH_1_G("RR_PLMN_SELECT_REQ rxd reset csfb_call_status, previous status %d",
                                rr_control_data_ptr->is_csfb_call_in_progress);

            rr_control_data_ptr->is_csfb_call_in_progress = FALSE;
            break;
          }

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) | defined(FEATURE_GSM_TDS)
          case RR_CSG_WHITE_LIST_UPDATE_IND:
          {
            rr_g2x_csg_white_list_updated(gas_id);
            rr_event = EV_NO_EVENT;
            break;
          } /* RR_CSG_WHITE_LIST_UPDATE_IND */
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

#ifdef FEATURE_SGLTE

          case RR_UE_MODE_IND:
          {
            MSG_GERAN_HIGH_2_G("ue_mode = %d, old ue_mode = %d",
                               new_message_ptr->mm.rr_ue_mode_ind.ue_mode,
                               rr_control_data_ptr->ue_mode);

            rr_control_data_ptr->ue_mode = new_message_ptr->mm.rr_ue_mode_ind.ue_mode;

            rr_event = EV_NO_EVENT;
            break;
          } /* RR_UE_MODE_IND */

          case RR_PLMN_LIST_REQ:
          {
            if (rr_is_sglte_mode_active(gas_id))
            {
              if (rr_control_data_ptr->rr_state == RR_PLMN_LIST_SEARCH &&
                  rr_x2g_bplmn_search_active(gas_id))
              {
                MSG_GERAN_HIGH_0_G("Reject RR_PLMN_LIST_REQ rxd in SGLTE mode during X2G BPLMN");
                rr_send_plmn_list_cnf(
                          NULL,
                          MM_AS_ABORTED,
                          &new_message_ptr->rr_plmn_list_req,
                          gas_id
                        );
                rr_event = EV_NO_EVENT;
              }
            }
            break;
          }

#endif /* FEATURE_SGLTE */

          case RR_SERVICE_STATUS_IND:
          {
            MSG_GERAN_HIGH_2_G("Service Status: service_domain %d, service_status %d",
                               new_message_ptr->mm.rr_service_status_ind.service_domain,
                               new_message_ptr->mm.rr_service_status_ind.service_state);

            rr_control_data_ptr->nas_service_status.service_domain = new_message_ptr->mm.rr_service_status_ind.service_domain;
            rr_control_data_ptr->nas_service_status.service_status = new_message_ptr->mm.rr_service_status_ind.service_state;
            rr_event = EV_NO_EVENT;
            break;
          } /* RR_SERVICE_STATUS_IND */

#ifdef FEATURE_LTE  
          case RR_MT_CSFB_FAILURE_IND:
          {
            MSG_GERAN_HIGH_1_G("MT-CSFB failure ind rcvd deprioritze cell = %d",
                               new_message_ptr->mm.rr_mt_csfb_failure_ind.csmt_fail_depriotize_cells);

            if (new_message_ptr->mm.rr_mt_csfb_failure_ind.csmt_fail_depriotize_cells)
            {
              rr_add_gsm_cell_to_depriotized_list(rr_gprs_get_scell_arfcn(gas_id), gas_id);
            }

            rr_event = EV_NO_EVENT;
            break;
          } /* RR_MT_CSFB_FAILURE_IND */
#endif /*FEATURE_LTE*/
          default:
          {
            ;
          }
        }
      }

#ifdef FEATURE_GSM_TDS
      // TODO DSDA
      else if (message_set == MS_TDSRRC_RR)
      {
        if (message_id == (byte) TDSRR_INTERRAT_RESELECTION_REQ)
        {
#ifdef FEATURE_SGLTE
          if (rr_is_sglte_mode_active(gas_id) == FALSE)
#endif /* FEATURE_SGLTE */
          {
            // Check if we received this in suitable RR state.
            if(rr_control_data_ptr->rr_state != RR_INACTIVE)
            {
              // if we received this in a state other than INACTIVE, no need to process
              rr_event = EV_NO_EVENT;

              // Send back the failure response to RRC
              rr_send_tds_interrat_reselection_rej(
                TDSRR_GSM_OTHER_REASON,
                0,
                gas_id
              );

              MSG_GERAN_ERROR_3_G(
                "Unexpected message=(%d,%d) in %d State",
                (int) message_set,
                (int) message_id,
                (int) rr_control_data_ptr->rr_state
              );
            }
          }
        }
        else if (message_id == (byte) TDSRR_INTERRAT_HANDOVER_REQ)
        {
          // Check if we received this in suitable RR state.
          if(rr_control_data_ptr->rr_state != RR_INACTIVE)
          {
            // if we received this in a state other than INACTIVE, no need to process
            rr_event = EV_NO_EVENT;

            // Send back the failure response to TDS-RRC
            rr_send_tds_interrat_ho_cnf(
              TDSRRC_RR_FAILURE,TDSRRC_RR_INVALID_STATE,
              &(new_message_ptr->tdsrr_ho_req.gsm_command_list),
              gas_id
            );

            MSG_GERAN_ERROR_3_G(
              "Unexpected message=(%d,%d) in %d State",
              (int) message_set,
              (int) message_id,
              (int) rr_control_data_ptr->rr_state
            );
          }
        }
        else if (message_id == (byte) TDSRR_INTERRAT_PLMN_SRCH_ABORT_REQ)
        {
          if (rr_control_data_ptr->rr_state != RR_PLMN_LIST_SEARCH)
          {
            rr_event = EV_NO_EVENT;
            rr_send_tds_interrat_plmn_search_abort_cnf(gas_id);

            MSG_GERAN_ERROR_3_G(
              "Unexpected message=(%d,%d) in %d State",
              (int) message_set,
              (int) message_id,
              (int) rr_control_data_ptr->rr_state
            );
          }
        }
        else if (message_id == (byte) TDSRR_STOP_GSM_REQ)
        {
          if (rr_control_data_ptr->rr_state != RR_INACTIVE)
          {
            MSG_GERAN_HIGH_1_G(
              "Unexpected TDSRR_STOP_GSM_REQ received in state %d, send _CNF",
              rr_control_data_ptr->rr_state
            );

            rr_send_tds_stop_gsm_mode_cnf(gas_id);
            rr_event = EV_NO_EVENT;
          }
        }
        else if (message_id == (byte) TDSRR_INTERRAT_TL1_STOP_IND)
        {
          MSG_GERAN_MED_1_G(
            "RR rxd TDSRR_INTERRAT_TL1_STOP_IND cause:%d",
            new_message_ptr->tdsrr_tl1_stop_ind.cause
          );

          if (new_message_ptr->tdsrr_tl1_stop_ind.cause == TDSRR_TL1_STOP_IND_T2G_CCO)
          {
            rr_control_data_ptr->waiting_for_tdsrr_interrat_tl1_stop_ind = FALSE;
            rr_send_imsg(RR_IMSG_X2G_CCO_COMPLETE_IND, gas_id);
            rr_event = EV_NO_EVENT;
          }
        }
        else if (message_id == (byte) TDSRR_INTERRAT_REDIRECT_REQ)
        {
          tdsrr_interrat_redirect_req_type *req = (tdsrr_interrat_redirect_req_type *) new_message_ptr;

          if (req->cell_srch_reason == TDSRR_SERVICE_SEARCH) /*not supported in TDS this is for FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION only*/
          {
            MSG_GERAN_ERROR_0_G(
              "TDSRR_INTERRAT_REDIRECT_REQ search_reason=TDSRR_SERVICE_SEARCH not supported"
            );

            rr_send_tds_interrat_redirect_rej(
              TDSRR_REDIRECT_FAIL_OTHER_REASON,
              NULL,
              gas_id
            );

            rr_event = EV_NO_EVENT;
          }
        }

        else if (message_id == (byte) TDSRR_CIPHER_SYNC_IND)
        {
          if (rr_control_data_ptr->rr_state == RR_INACTIVE)
          {
            rr_siminfo_activate_new_cipher_keys(gas_id);
          }
          else
          {
            MSG_GERAN_HIGH_0_G("Ignoring TDSRR_CIPHER_SYNC_IND while RR is active");
          }
          rr_event = EV_NO_EVENT;
        }

#ifdef FEATURE_SGLTE
        else if (message_id == (byte) TDSRR_INTERRAT_PLMN_SRCH_REQ)
        {
          if (rr_is_sglte_mode_active(gas_id))
          {
            if (rr_control_data_ptr->sglte_x2g_plmn_list_search_in_progress)
            {
              if (rr_control_data_ptr->sglte_x2g_plmn_list_search_results.srch_done)
              {
                rr_send_tds_interrat_plmn_search_cnf(
                  TRUE,
                  rr_control_data_ptr->sglte_x2g_plmn_list_search_results.hplmn_found,
                  rr_control_data_ptr->sglte_x2g_plmn_list_search_results.available_plmn_list,
                  gas_id
                );

                rr_control_data_ptr->sglte_x2g_plmn_list_search_in_progress = FALSE;
                rr_control_data_ptr->sglte_x2g_plmn_list_search_is_foreground = FALSE;
                rr_control_data_ptr->sglte_x2g_plmn_list_search_aborted_by_rrc = FALSE;

                rr_sglte_x2g_plmn_list_search_results_clear(gas_id);
              }
              else
              {

                tdsrr_interrat_plmn_srch_req_type* req = (tdsrr_interrat_plmn_srch_req_type *) &new_message_ptr->tdsrr_plmn_srch_req;
                
                if ( req->is_foreground_search == FALSE )
                { /* Search is happening in the Background, send Immediate Confirm */
                  rr_send_tds_interrat_plmn_search_cnf(
                  FALSE,
                  FALSE,
                  NULL,
                  gas_id
                  );
                }
                else
                { /* Await for search to Finish for foreground search */
                  rr_control_data_ptr->sglte_x2g_plmn_list_search_is_foreground = req->is_foreground_search ;
                }
              }
              /* Message has been processed in stateless handler, so no need to
              pass it through to the state handlers. */
              rr_event = EV_NO_EVENT;
            }
            else if (rr_control_data_ptr->rr_state == RR_PLMN_LIST_SEARCH)
            {
              MSG_GERAN_HIGH_0_G("Reject TDSRR_INTERRAT_PLMN_SRCH_REQ during PLMN search");
              rr_sglte_reject_tds_plmn_srch_req(gas_id);

              rr_event = EV_NO_EVENT;
            }
          }
        }
#endif /* FEATURE_SGLTE */
#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
        else if (message_id == (byte) TDSRRC_DEDICATED_PRIORITIES_RSP)
        {
          tdsrrc_dedicated_priorities_rsp_type * dedicated_priorities_rsp =
                                          &new_message_ptr->tdsrrc_dedicated_priorities_rsp;

          rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

          sys_plmn_id_s_type requested_PLMN_id = rr_PLMN_request_details_ptr->requested_PLMN_id;

          boolean plmn_ok = sys_plmn_match(requested_PLMN_id, dedicated_priorities_rsp->plmn);

          if (!plmn_ok)
          {
            plmn_ok = rr_eplmn_list_equivalent_plmn(requested_PLMN_id, gas_id) &&
                      rr_eplmn_list_equivalent_plmn(dedicated_priorities_rsp->plmn, gas_id);
          }

          if (plmn_ok)
          {
            if (dedicated_priorities_rsp->dedicated_pri_valid)
            {
              ARFCN_T arfcn = rr_gprs_get_scell_arfcn(gas_id);

              rr_dedicated_priority_info_store(
                &dedicated_priorities_rsp->dedicated_pri_info,
                &arfcn,
                requested_PLMN_id,
                gas_id
              );

              rr_dedicated_priorities_take_ownership(gas_id);

              rr_send_imsg(RR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND, gas_id);
            }
            else
            {
              /* No action necessary, dedicated priorities are no longer valid.  Likely
              to be due to timer expiry between request and response. */
              MSG_GERAN_HIGH_0_G("Dedicated priorities not valid");
            }
          }
          else
          {
            if (geran_is_multi_mode_gas_id(gas_id))
            {
              lte_rrc_dedicated_pri_owner_e owner;

              MSG_GERAN_HIGH_0_G("Dedicated priorities not valid for current PLMN, discarding");

              owner = lte_rrc_dedicated_pri_get_owner();

              lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_NONE);

              rr_dedicated_priorities_request_owner_clears(owner, gas_id);
            }
            else
            {
              MSG_GERAN_HIGH_0_G("GERAN AS not multimode: request for Dedicated priorities N/A");
            }
          }

          /* Message is processed, no need to pass it through to RR state machines. */
          rr_event = EV_NO_EVENT;
        }
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/
      }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
      else if (message_set == MS_RRC_RR)
      {
        if (message_id == (byte) RR_INTERRAT_HANDOVER_REQ)
        {
          // Check if we received this in suitable RR state.
          if(rr_control_data_ptr->rr_state != RR_INACTIVE)
          {
            // if we received this in a state other than INACTIVE, no need to process
            rr_event = EV_NO_EVENT;

            // Send back the failure response to RRC
            rr_send_interrat_ho_cnf(
              RRC_RR_FAILURE,RRC_RR_INVALID_STATE,
              &(new_message_ptr->rr_interrat_handover_req.gsm_command_list),
              gas_id
            );

            MSG_GERAN_ERROR_3_G(
              "Unexpected message=(%d,%d) in %d State",
              (int) message_set,
              (int) message_id,
              (int) rr_control_data_ptr->rr_state
            );
          }
        }

        else if (message_id == (byte) RR_INTERRAT_RESELECTION_REQ)
        {
          // Check if we received this in suitable RR state.
          if(rr_control_data_ptr->rr_state != RR_INACTIVE)
          {
            // if we received this in a state other than INACTIVE, no need to process
            rr_event = EV_NO_EVENT;

            // Send back the failure response to RRC
            rr_send_interrat_reselection_rej(
              RR_GSM_OTHER_REASON,
              0,
              gas_id
            );

            MSG_GERAN_ERROR_3_G(
              "Unexpected message=(%d,%d) in %d State",
              (int) message_set,
              (int) message_id,
              (int) rr_control_data_ptr->rr_state
            );
          }
        }

        else if (message_id == (byte) RR_INTERRAT_CC_ORDER_REQ)
        {
          // Check if we received this in suitable RR state.
          if (rr_control_data_ptr->rr_state != RR_INACTIVE)
          {
            // if we received this in a state other than INACTIVE, no need to process
            rr_event = EV_NO_EVENT;

            // Send back the failure response to RRC
            rr_send_interrat_cc_order_rej(
              RR_GSM_CCO_OTHER_REASON,
              gas_id
            );

            MSG_GERAN_ERROR_3_G(
              "Unexpected message=(%d,%d) in %d State",
              (int) message_set,
              (int) message_id,
              (int) rr_control_data_ptr->rr_state
            );
          }
        }

        else if (message_id == (byte) RR_INTERRAT_PLMN_SRCH_ABORT_REQ)
        {
          if (rr_control_data_ptr->rr_state != RR_PLMN_LIST_SEARCH)
          {
            rr_event = EV_NO_EVENT;
            rr_send_interrat_plmn_search_abort_cnf(gas_id);

            MSG_GERAN_ERROR_3_G(
              "Unexpected message=(%d,%d) in %d State",
              (int) message_set,
              (int) message_id,
              (int) rr_control_data_ptr->rr_state
            );
          }
        }

        else if (message_id == (byte) RR_RRC_STOP_GSM_REQ)
        {
          if (rr_control_data_ptr->rr_state != RR_INACTIVE)
          {
            MSG_GERAN_HIGH_1_G(
              "Unexpected RR_RRC_STOP_GSM_MODE_REQ received in state %d, send _CNF",
              rr_control_data_ptr->rr_state
            );

            rr_send_rrc_stop_gsm_mode_cnf(gas_id);
            rr_event = EV_NO_EVENT;
          }
        }


#ifdef FEATURE_LTE
        else if (message_id == (byte) RRC_DEDICATED_PRIORITIES_RSP)
        {
          rrc_dedicated_priorities_rsp_type * dedicated_priorities_rsp =
            &new_message_ptr->rrc_dedicated_priorities_rsp;

          rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

          sys_plmn_id_s_type requested_PLMN_id = rr_PLMN_request_details_ptr->requested_PLMN_id;

          boolean plmn_ok = sys_plmn_match(requested_PLMN_id, dedicated_priorities_rsp->plmn);

          if (!plmn_ok)
          {
            plmn_ok = rr_eplmn_list_equivalent_plmn(requested_PLMN_id, gas_id) &&
                      rr_eplmn_list_equivalent_plmn(dedicated_priorities_rsp->plmn, gas_id);
          }

          if (plmn_ok)
          {
            if (dedicated_priorities_rsp->dedicated_pri_valid)
            {
              ARFCN_T arfcn = rr_gprs_get_scell_arfcn(gas_id);

              rr_dedicated_priority_info_store(
                &dedicated_priorities_rsp->dedicated_pri_info,
                &arfcn,
                requested_PLMN_id,
                gas_id
              );

              rr_dedicated_priorities_take_ownership(gas_id);

              rr_send_imsg(RR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND, gas_id);
            }
            else
            {
              /* No action necessary, dedicated priorities are no longer valid.  Likely
              to be due to timer expiry between request and response. */
              MSG_GERAN_HIGH_0_G("Dedicated priorities not valid");
            }
          }
          else
          {
            if (geran_is_multi_mode_gas_id(gas_id))
            {
              lte_rrc_dedicated_pri_owner_e owner;

              MSG_GERAN_HIGH_0_G("Dedicated priorities not valid for current PLMN, discarding");

              owner = lte_rrc_dedicated_pri_get_owner();

              lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_NONE);

              rr_dedicated_priorities_request_owner_clears(owner, gas_id);
            }
            else
            {
              MSG_GERAN_HIGH_0_G("GERAN AS not multimode: request for Dedicated priorities N/A");
            }
          }

          /* Message is processed, no need to pass it through to RR state machines. */
          rr_event = EV_NO_EVENT;
        }
#endif /* FEATURE_LTE */


        else if (message_id == (byte) RRC_CIPHER_SYNC_IND)
        {
          if (rr_control_data_ptr->rr_state == RR_INACTIVE)
          {
            rr_siminfo_activate_new_cipher_keys(gas_id);
          }
          else
          {
            MSG_GERAN_HIGH_0_G("Ignoring RRC_CIPHER_SYNC_IND while RR is active");
          }
          rr_event = EV_NO_EVENT;
        }

      } /* (message_set == MS_RRC_RR) */
#endif /* FEATURE_WCDMA */

      /***********************************************************/
      /* Handle some of the timers in a state independent manner */
      /***********************************************************/
      else if ((message_set == MS_TIMER) && (message_id == MID_TIMER_EXPIRY))
      {
        /* assume the timer will be handled by the state independent handler */
        boolean timer_handled = TRUE;
        timer_id = (rr_timer_id_T) new_message_ptr->mid_timer_expiry.timer_id;

        new_rr_event = EV_NO_EVENT;

        /***********************************************************************/
        /* NOTE: Any new events generated by timer processing should be placed */
        /* in new_rr_event variable                                            */
        /***********************************************************************/

        /***************************************************************/
        /* Forbidden cell timers are used during cell reselection to   */
        /* discourage RR from returning to a previous serving cell for */
        /* a period of 5 seconds. The timer has expired indicating     */
        /* that a previously forbidden cell is now allowed             */
        /***************************************************************/
        if (timer_id == RR_RESET_FORBIDDEN_CELL_TIMER_1 ||
            timer_id == RR_RESET_FORBIDDEN_CELL_TIMER_2 ||
            timer_id == RR_RESET_FORBIDDEN_CELL_TIMER_3 ||
            timer_id == RR_RESET_FORBIDDEN_CELL_TIMER_4 ||
            timer_id == RR_RESET_FORBIDDEN_CELL_TIMER_5 ||
            timer_id == RR_RESET_FORBIDDEN_CELL_TIMER_6 ||
            timer_id == RR_RESET_FORBIDDEN_CELL_TIMER_7)
        {
          /*********************************************/
          /* one of the forbidden cells is now allowed */
          /* clear it from the forbidden cell list     */
          /*********************************************/
          rr_clear_forbidden_cell_timer(timer_id, gas_id);

          /****************************************************************/
          /* Nothing else needs to be done. The previously forbidden cell */
          /* will now be allowed if a reselection occurs.                 */
          /****************************************************************/
        } /* forbidden cell timer */

        /************************************************************************/
        /* Five second timers are used as part of cell reselection to decide if */
        /* a surrounding cell has been better than the serving cell long enough */
        /************************************************************************/

        else if ((rr_control_data_ptr->rr_state != RR_GRR) &&
                 ((timer_id == RR_FIVE_SEC_C2_TIMER_1) ||
                 (timer_id == RR_FIVE_SEC_C2_TIMER_2) ||
                 (timer_id == RR_FIVE_SEC_C2_TIMER_3) ||
                 (timer_id == RR_FIVE_SEC_C2_TIMER_4) ||
                 (timer_id == RR_FIVE_SEC_C2_TIMER_5) ||
                 (timer_id == RR_FIVE_SEC_C2_TIMER_6))
                )
        {
          // Update the Surrounding gsm Cell 5 Sec timer state
          rr_gprs_update_five_second_timer_state(timer_id, gas_id);
        }

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
        /************************************************************************/
        /* Five second timers are used as part of InterRAT WCDMA to GSM cell    */
        /* reselection to decide if a surrounding wcdma cell has been better    */
        /* than the serving cell long enough                                    */
        /************************************************************************/
        else if ((timer_id == RR_G2WR_FIVE_SEC_TIMER_1) ||
                 (timer_id == RR_G2WR_FIVE_SEC_TIMER_2) ||
                 (timer_id == RR_G2WR_FIVE_SEC_TIMER_3) ||
                 (timer_id == RR_G2WR_FIVE_SEC_TIMER_4) ||
                 (timer_id == RR_G2WR_FIVE_SEC_TIMER_5) ||
                 (timer_id == RR_G2WR_FIVE_SEC_TIMER_6))
        {
#ifdef FEATURE_WCDMA
          // Update the Surrounding wcdma Cell 5 Sec timer state
          rr_g2wr_update_wcdma_five_second_timer_state(timer_id, gas_id);
#endif /* FEATURE_WCDMA */

          new_rr_event = EV_CR_G2W_TIMER_EXPIRED;
        }

#ifdef FEATURE_LTE

        else if (timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_1 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_2 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_3 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_4 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_5 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_6 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_7 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_8 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_9 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_10 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_11 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_HIGH_TIMER_12 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_1 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_2 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_3 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_4 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_5 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_6 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_7 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_8 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_9 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_10 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_11 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_THRESH_LOW_TIMER_12 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_1 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_2 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_3 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_4 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_5 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_6 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_7 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_8 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_9 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_10 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_11 ||
                 timer_id == RR_IRAT_PRIORITY_RESEL_H_PRIO_TIMER_12)
        {
          new_rr_event = EV_CR_G2W_TIMER_EXPIRED;
        }

#endif /* FEATURE_LTE */

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
        else if (timer_id == RR_W2G_SERVICE_REDIRECTION_WAIT_TIMER)
        {
          (void)rr_w2g_service_redirection_control(
            RR_EV_SERVICE_REDIRECTION_WAIT_TIMER_EXPIRY,
            NULL,
            gas_id
          );
        }
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

        else if (timer_id == RR_G2X_BARRED_FREQUENCY_TIMER_1 ||
                 timer_id == RR_G2X_BARRED_FREQUENCY_TIMER_2 ||
                 timer_id == RR_G2X_BARRED_FREQUENCY_TIMER_3 ||
                 timer_id == RR_G2X_BARRED_FREQUENCY_TIMER_4 ||
                 timer_id == RR_G2X_BARRED_FREQUENCY_TIMER_5 ||
                 timer_id == RR_G2X_BARRED_FREQUENCY_TIMER_6)
        {
          rr_g2x_barred_frequency_timer_expired(timer_id, gas_id);
        }

        else if (timer_id == RR_G2X_BARRED_CELL_TIMER_1 ||
                 timer_id == RR_G2X_BARRED_CELL_TIMER_2 ||
                 timer_id == RR_G2X_BARRED_CELL_TIMER_3 ||
                 timer_id == RR_G2X_BARRED_CELL_TIMER_4 ||
                 timer_id == RR_G2X_BARRED_CELL_TIMER_5 ||
                 timer_id == RR_G2X_BARRED_CELL_TIMER_6)
        {
          rr_g2x_barred_cell_timer_expired(timer_id, gas_id);
        }

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

        /*********************************************************/
        /* Serving cell 5 second timer used in cell reselection  */
        /* to decide if serving C1 falls below 0 for long enough */
        /*********************************************************/
        else if (timer_id == RR_SERV_FIVE_SEC_C1_TIMER)
        {
          // Update the Serving Cell 5 Sec timer state
          rr_update_serving_five_second_timer(gas_id);
        }

        /****************************************************************/
        /* Signal strength timer is used to periodically send updated   */
        /* signal strength information to the CM (User Interface) task. */
        /* The timer has expired indicating that the status needs to    */
        /* be sent.                                                     */
        /****************************************************************/
        else if (timer_id == SIGNAL_STRENGTH_TIMER)
        {
          MSG_GERAN_LOW_0_G("Signal strength timer expiration");

          /* only send the message if NOT in the Inactive state */
          if (rr_control_data_ptr->rr_state != RR_INACTIVE)
          {
            rr_send_cm_status_ind(gas_id);
          }

          if (rr_is_camped(gas_id))
          {
            (void)rr_timer_start(SIGNAL_STRENGTH_TIMER, SIGNAL_STRENGTH_TIMEOUT, gas_id);
          }

        } /* SIGNAL_STRENGTH_TIMER */

        /*********************************************************************/
        /* Timer is used to invalidate the available PLMN database collected */
        /* during a cell selection or PLMN list construction operation       */
        /* when the information gets too old                                 */
        /*********************************************************************/
        else if (timer_id == RR_INVALIDATE_AVAILABLE_PLMN_DB_TIMER)
        {
          /* invalidate the database */
          rr_invalidate_available_plmn_database(gas_id);
        }

        /* Handle barred cell timer expiry */
        else if (timer_id == RR_CELL_BARRED_TIMER)
        {
          rr_process_barred_cell_timer_expiry(gas_id);
        }

#ifdef FEATURE_LTE
        else if (timer_id == T3230)
        {
          MSG_GERAN_HIGH_0_G("T3230 expired");
          rr_individual_priorities_clear(gas_id);
          rr_send_imsg(RR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND, gas_id);
        }
#endif /* FEATURE_LTE */
        else if (timer_id == RR_L1_GUARD_TIMER ||
                 timer_id == RR_GRR_CONTROL_STATE_TRANSITION_GUARD_TIMER ||
                 timer_id == RR_RESEL_CONTROL_STATE_TRANSITION_GUARD_TIMER ||
                 timer_id == RR_CELL_ACQ_CONTROL_STATE_TRANSITION_GUARD_TIMER ||
                 timer_id == RR_MM_ACTIVATION_RSP_TIMER ||
                 timer_id == RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMER)
        {
          switch (timer_id)
          {
            case RR_L1_GUARD_TIMER:
            {
              MSG_GERAN_ERROR_0_G("No response from L1 to start/stop request");
              break;
            } /* RR_L1_GUARD_TIMER */

            case RR_GRR_CONTROL_STATE_TRANSITION_GUARD_TIMER:
            {
              MSG_GERAN_ERROR_0_G("grr_control() stuck waiting for response from L1 / MAC");
              break;
            } /* RR_GRR_CONTROL_STATE_TRANSITION_GUARD_TIMER */

            case RR_RESEL_CONTROL_STATE_TRANSITION_GUARD_TIMER:
            {
              MSG_GERAN_ERROR_0_G("rr_resel_control() stuck waiting for response from L1 / MAC");
              break;
            } /* RR_RESEL_CONTROL_STATE_TRANSITION_GUARD_TIMER */

            case RR_CELL_ACQ_CONTROL_STATE_TRANSITION_GUARD_TIMER:
            {
              MSG_GERAN_ERROR_0_G("rr_cell_acq_control() stuck waiting for response from L1 / MAC");
              break;
            } /* RR_CELL_ACQ_CONTROL_STATE_TRANSITION_GUARD_TIMER */

			case RR_MM_ACTIVATION_RSP_TIMER:
            {
              MSG_GERAN_ERROR_0_G("rr_w2g_redirection_control () stuck waiting for response from MM in the form of RR_ACTIVATION_RSP");
			  
              if (geran_get_nv_recovery_restart_enabled(gas_id))
              {
                geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
              }
              else
              {
                ERR_GERAN_FATAL_1_G("GRR state transition timer %d expired, recovery disabled", (int)timer_id);
              }			  
              break;				
            }			

            case RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMER:
            {
              MSG_GERAN_ERROR_0_G("RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMER expired GRR is  stuck waiting for response from L1 ");
              if (geran_get_nv_recovery_restart_enabled(gas_id))
              {
                geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
              }
              else
              {
                ERR_GERAN_FATAL_1_G("GRR state transition timer %d expired, recovery disabled", (int)timer_id);
              }
              break;
            }  /*RR_CONN_REL_CONTROL_WAIT_FOR_REL_CNF_TIMER*/

            default:
            {
              MSG_GERAN_ERROR_1_G(
                "Unhandled timer expiry (ID=%d) will cause recovery restart",
                (int) timer_id
              );
              break;
            }
          }

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
          /* not all of these timers are applicable in multi-SIM modes */
          if (rr_multi_sim_mode_enabled() && (RR_GRR_CONTROL_STATE_TRANSITION_GUARD_TIMER != timer_id))
          {
            MSG_GERAN_ERROR_1_G("GRR state transition timer %d expiry ignored", (int)timer_id);
          }
          else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
          {
            if (geran_get_nv_recovery_restart_enabled(gas_id))
            {
              geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
            }
            else
            {
              ERR_GERAN_FATAL_1_G("GRR state transition timer %d expired, recovery disabled", (int)timer_id);
            }
          }
        }
        else
        {
          timer_handled = rr_gprs_general_timer_expiry_handler(timer_id, &new_rr_event, gas_id);
        }

        /*************************************************************/
        /* If the timer was handled above, convert any new_rr_events */
        /* into the main rr_event to be handled below                */
        /*************************************************************/
        if (timer_handled == TRUE)
        {
          rr_event = new_rr_event;
          new_rr_event = EV_NO_EVENT;

          /* redo the loop with the new RR event */
          continue;
        }
      } /* MID_TIMER_EXPIRY */

      // TODO DSDA
      else if (message_set == MS_MSGR_RR)
      {
        switch (new_message_ptr->rr_msgr_msg.message.hdr.id)
        {
#ifdef FEATURE_FAKE_BTS_CELL_INFO
          case GERAN_GRR_QMI_CELL_INFO_REQ:
          {
            if (rr_get_fake_bs_detection_enabled(gas_id) == TRUE)
            {
              rr_handle_qmi_cell_info_req(gas_id);
              rr_event = EV_NO_EVENT;
            }
            break;
          }
#endif /*FEATURE_FAKE_BTS_CELL_INFO*/

#ifdef FEATURE_FAKE_BTS_CELL_BARRING
          case GERAN_GRR_FAKE_GCELL_ACTION_REQ:
          {
            if (rr_get_fake_bs_detection_enabled(gas_id) == TRUE)
            {
              if (new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.fake_gcell_action_req.barred_time != 0)
              {
                rr_barred_cell_info_T fake_cell;
                geran_grr_band_ind_e band_ind;
                csi_entry_t *entry_ptr;
                csi_entry_t csi_entry;
                gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
                band_ind = ((new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.fake_gcell_action_req.arfcn >> 15)& 0x01);
                new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.fake_gcell_action_req.arfcn &= 0x7FFF;
                fake_cell.arfcn = geran_grr_convert_to_arfcn_type(new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.fake_gcell_action_req.arfcn, band_ind);
                fake_cell.bsic.PLMN_colour_code = new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.fake_gcell_action_req.bsic>>3 & 0x07;
                fake_cell.bsic.BS_colour_code = new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.fake_gcell_action_req.bsic & 0x07;
                fake_cell.remaining_time = new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.fake_gcell_action_req.barred_time;
                fake_cell.cgi.lai.plmn_id = new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.fake_gcell_action_req.plmn_id;
                fake_cell.cgi.lai.location_area_code = new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.fake_gcell_action_req.location_area_code;
                fake_cell.cgi.ci = new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.fake_gcell_action_req.cell_id;
                fake_cell.cgi_valid = TRUE;

                rr_add_to_barred_cell_list(&fake_cell, gas_id); 
                csi_entry.cgi = fake_cell.cgi;
                csi_entry.si_arfcn = fake_cell.arfcn;

                entry_ptr = rr_csi_find_entry(&csi_entry);

                if (entry_ptr != NULL && ARFCNS_EQUAL(fake_cell.arfcn,entry_ptr->si_arfcn) == TRUE)
                {    
                  rr_csi_flush_data(&fake_cell.cgi.lai, &fake_cell.cgi.ci, CSI_FLUSH_FAKE_CELL_DETECTED, gas_id);
                }
                
                if ((scell_info_ptr->gsm.BCCH_ARFCN.num == fake_cell.arfcn.num) && (scell_info_ptr->gsm.BCCH_ARFCN.band == fake_cell.arfcn.band) &&
                    (scell_info_ptr->gsm.BSIC.PLMN_colour_code == fake_cell.bsic.PLMN_colour_code) && (scell_info_ptr->gsm.BSIC.BS_colour_code== fake_cell.bsic.BS_colour_code))
                {
                  rr_event = rr_handle_fake_gcell_action_req(new_message_ptr, gas_id);
                }
                else
                {
                  rr_event = EV_NO_EVENT;
                }
              }
              else
              {
                MSG_GERAN_HIGH_0_G("Invalid bar time, no action taken");
                rr_event = EV_NO_EVENT;
              }
            }
            break;
          }

#endif /*FEATURE_FAKE_BTS_CELL_BARRING*/

#ifdef FEATURE_LTE
          case GERAN_GRR_LTE_CS_CAPABILITIES_REQ:
          {
            rr_handle_lte_cs_capabilities_req(
              &new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_cs_capabilities_req,
              gas_id
            );
            rr_event = EV_NO_EVENT;
            break;
          } /* GERAN_GRR_LTE_CS_CAPABILITIES_REQ */

          case GERAN_GRR_LTE_PS_CAPABILITIES_REQ:
          {
            rr_handle_lte_ps_capabilities_req(
              &new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_ps_capabilities_req,
              gas_id
            );
            rr_event = EV_NO_EVENT;
            break;
          } /* GERAN_GRR_LTE_PS_CAPABILITIES_REQ */

#ifdef FEATURE_SGLTE
          case GERAN_GRR_LTE_PLMN_SRCH_REQ:
          {
            if (rr_is_sglte_mode_active(gas_id))
            {
              if (rr_control_data_ptr->sglte_x2g_plmn_list_search_in_progress)
              {
                MSG_GERAN_HIGH_0_G("GERAN_GRR_LTE_PLMN_SRCH_REQ: sglte_x2g_plmn_list_search_in_progress=1");

                if (rr_control_data_ptr->sglte_x2g_plmn_list_search_results.srch_done)
                {
                  lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
                  errno_enum_type          result;

                  plmn_srch_rsp_ptr = (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));

                  RR_NULL_CHECK_RETURN_VOID(plmn_srch_rsp_ptr);
                  msgr_init_hdr(
                    &plmn_srch_rsp_ptr->msg_hdr,
                    MSGR_GERAN_GRR,
                    GERAN_GRR_LTE_PLMN_SRCH_RSP
                  );

                  plmn_srch_rsp_ptr->srch_done = TRUE;
                  if (rr_control_data_ptr->sglte_x2g_plmn_list_search_results.available_plmn_list != NULL)
                  {
                    plmn_srch_rsp_ptr->available_plmn_list = *rr_control_data_ptr->sglte_x2g_plmn_list_search_results.available_plmn_list;
                  }
                  rr_invalidate_csg_info(&plmn_srch_rsp_ptr->available_plmn_list);

                  plmn_srch_rsp_ptr->hplmn_found = rr_control_data_ptr->sglte_x2g_plmn_list_search_results.hplmn_found;
                  if (rr_control_data_ptr->sglte_x2g_plmn_list_search_results.detected_frequency_list != NULL)
                  {
                    plmn_srch_rsp_ptr->detected_frequency_list = *rr_control_data_ptr->sglte_x2g_plmn_list_search_results.detected_frequency_list;
                  }

                  result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);
                  if (result != E_SUCCESS)
                  {
                    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                  }
                  GPRS_MEM_FREE(plmn_srch_rsp_ptr);

                  rr_control_data_ptr->sglte_x2g_plmn_list_search_in_progress = FALSE;

                  rr_control_data_ptr->sglte_x2g_plmn_list_search_is_foreground = FALSE;
                  rr_control_data_ptr->sglte_x2g_plmn_list_search_aborted_by_rrc = FALSE;

                  rr_sglte_x2g_plmn_list_search_results_clear(gas_id);
                }
                else
                {
                  
                  lte_irat_plmn_srch_req_s* req = (lte_irat_plmn_srch_req_s *) &new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.plmn_srch_req;

                  if (req->foreground_search == FALSE )
                  {
                    lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
                    errno_enum_type          result;

                    plmn_srch_rsp_ptr =
                      (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));

                    RR_NULL_CHECK_RETURN_VOID(plmn_srch_rsp_ptr);

                    msgr_init_hdr(
                      &plmn_srch_rsp_ptr->msg_hdr,
                      MSGR_GERAN_GRR,
                      GERAN_GRR_LTE_PLMN_SRCH_RSP
                    );

                    plmn_srch_rsp_ptr->srch_done = FALSE;

                    rr_invalidate_csg_info(&plmn_srch_rsp_ptr->available_plmn_list);

                    result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);
                    if (result != E_SUCCESS)
                    {
                      MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                    }
                    GPRS_MEM_FREE(plmn_srch_rsp_ptr);
                  }
                  else
                  {
                   /* New request is a Background search, wait for Results to Finish */
                   rr_control_data_ptr->sglte_x2g_plmn_list_search_is_foreground = req->foreground_search;
                  }
                }
                /* Message has been processed in stateless handler, so no need to
                pass it through to the state handlers. */
                rr_event = EV_NO_EVENT;
              }
              else if (rr_control_data_ptr->rr_state == RR_PLMN_LIST_SEARCH)
              {
                MSG_GERAN_HIGH_0_G("Reject GERAN_GRR_LTE_PLMN_SRCH_REQ during PLMN search");
                rr_sglte_reject_lte_plmn_srch_req(gas_id);

                rr_event = EV_NO_EVENT;
              }
            }
            break;
          } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */
#endif /* FEATURE_SGLTE */

          case GERAN_GRR_LTE_ABORT_PLMN_SRCH_REQ:
          {
            if (rr_control_data_ptr->rr_state != RR_PLMN_LIST_SEARCH)
            {
              lte_irat_abort_plmn_srch_rsp_s abort_plmn_srch_rsp;
              errno_enum_type                result;

              memset(&abort_plmn_srch_rsp, 0, sizeof(abort_plmn_srch_rsp));

              msgr_init_hdr(
                &abort_plmn_srch_rsp.msg_hdr,
                MSGR_GERAN_GRR,
                GERAN_GRR_LTE_ABORT_PLMN_SRCH_RSP
              );

              result = rr_msgr_send(&abort_plmn_srch_rsp.msg_hdr, sizeof(abort_plmn_srch_rsp), gas_id);
              if (result != E_SUCCESS)
              {
                MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
              }

              rr_event = EV_NO_EVENT;
            }
            break;
          } /* GERAN_GRR_LTE_ABORT_PLMN_SRCH_REQ */

          case LTE_RRC_CCO_NACC_COMPLETED_IND:
          {
            rr_control_data_ptr->waiting_for_lte_rrc_cco_nacc_completed_ind = FALSE;
            rr_send_imsg(RR_IMSG_X2G_CCO_COMPLETE_IND, gas_id);
            rr_event = EV_NO_EVENT;
            break;
          } /* LTE_RRC_CCO_NACC_COMPLETED_IND */

          case LTE_RRC_GET_DEDICATED_PRI_RSP:
          {
            lte_rrc_get_dedicated_pri_rsp_s * get_dedicated_pri_rsp =
              &new_message_ptr->rr_msgr_msg.message.lte_rrc_get_dedicated_pri_rsp;

            rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

            sys_plmn_id_s_type requested_PLMN_id = rr_PLMN_request_details_ptr->requested_PLMN_id;

            boolean plmn_ok = sys_plmn_match(requested_PLMN_id, get_dedicated_pri_rsp->plmn);

            if (!plmn_ok)
            {
              plmn_ok = rr_eplmn_list_equivalent_plmn(requested_PLMN_id, gas_id) &&
                        rr_eplmn_list_equivalent_plmn(get_dedicated_pri_rsp->plmn, gas_id);
            }

            if (plmn_ok)
            {
              if (get_dedicated_pri_rsp->dedicated_priorities_valid)
              {
                ARFCN_T arfcn = rr_gprs_get_scell_arfcn(gas_id);

                rr_dedicated_priority_info_store(
                  &get_dedicated_pri_rsp->dedicated_priority_info,
                  &arfcn,
                  requested_PLMN_id,
                  gas_id
                );

                rr_dedicated_priorities_take_ownership(gas_id);

                rr_send_imsg(RR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND, gas_id);
              }
              else
              {
                /* No action necessary, dedicated priorities are no longer valid.  Likely
                to be due to timer expiry between request and response. */
                MSG_GERAN_HIGH_0_G("Dedicated priorities not valid");
              }
            }
            else
            {
              if (geran_is_multi_mode_gas_id(gas_id))
              {
                lte_rrc_dedicated_pri_owner_e owner;

                MSG_GERAN_HIGH_0_G("Dedicated priorities not valid for current PLMN, discarding");

                owner = lte_rrc_dedicated_pri_get_owner();

                lte_rrc_dedicated_pri_set_owner(LTE_RRC_DEDICATED_PRI_OWNER_NONE);

                rr_dedicated_priorities_request_owner_clears(owner, gas_id);
              }
              else
              {
                MSG_GERAN_HIGH_0_G("GERAN AS not multimode: request for Dedicated priorities N/A");
              }
            }

            /* Message is processed, no need to pass it through to RR state machines. */
            rr_event = EV_NO_EVENT;

            break;
          } /* LTE_RRC_GET_DEDICATED_PRIORITIES_RSP */

#ifdef FEATURE_LTE_REL11
          case GERAN_GRR_LTE_DEPRI_FREQ_REQ:
          {
            /* update de-pri list.*/
            rr_g2w_update_lte_arfcn_deprioritized_list(&new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_depri_freq_req,
                                                       gas_id);

            /* Message is processed, no need to pass it through to RR state machines. */
            rr_event = EV_NO_EVENT;
            break;
          }
#endif /*FEATURE_LTE_REL11*/

#ifdef FEATURE_SGLTE
          case GERAN_GRR_LTE_SGLTE_SERVING_MEAS_REQ:
          {
            boolean serving_cell_meas_valid = TRUE;

            if (rr_is_sglte_mode_active(gas_id))
            {
              if (rr_sglte_is_full_service_available(gas_id) && rr_sglte_serving_cell_meas_valid(gas_id) && rr_cell_supports_gprs(gas_id))
              {
                if ( (rr_cell_supports_dtm(gas_id) == FALSE) && (rr_is_voice_call_active(gas_id)) )
                {
                  MSG_GERAN_HIGH_0_G("GERAN_GRR_LTE_SGLTE_SERVING_MEAS_REQ: Report NO SERVICE when in Active CS Call on a NON DTM Cell");
                  serving_cell_meas_valid = FALSE;
                }
              }
              else
              {
                serving_cell_meas_valid = FALSE;
              }

              if ( serving_cell_meas_valid )
              {
                rr_measured_signal_data_t * measured_signal_data = rr_get_measured_signal_data_ptr(gas_id);

                /* This request should only be received when GERAN is operating
                in CS-only / GSM-only mode. */
                RR_ASSERT_WARN(rr_sel_get_service_domain(gas_id) == SYS_SRV_DOMAIN_CS_ONLY);
                RR_ASSERT_WARN(rr_gsm_only(gas_id) == TRUE);

                rr_send_lte_sglte_serving_meas_rsp(
                  rr_gprs_get_scell_arfcn(gas_id),
                  TRUE,
                  rr_gprs_get_scell_bsic(gas_id),
                  measured_signal_data->current_signal_strength,
                  gas_id
                );
              }
              else
              {
                rr_send_lte_sglte_serving_meas_rsp_no_service(gas_id);
              }
            }
            else
            {
              MSG_GERAN_ERROR_0_G("Ignoring GERAN_GRR_LTE_SGLTE_SERVING_MEAS_REQ, SGLTE mode not active");
            }

            /* Message is processed, no need to pass it through to RR state machines. */
            rr_event = EV_NO_EVENT;
            break;
          } /* GERAN_GRR_LTE_SGLTE_SERVING_MEAS_REQ */
#endif /* FEATURE_SGLTE */
#endif /* FEATURE_LTE */

#if defined(FEATURE_GSM_TDS) && defined(FEATURE_SGLTE)
          case GERAN_GRR_TDS_SGLTE_SERVING_MEAS_REQ:
          {
            boolean serving_cell_meas_valid = TRUE;

            if (rr_is_sglte_mode_active(gas_id))
            {
              if (rr_sglte_is_full_service_available(gas_id) && rr_sglte_serving_cell_meas_valid(gas_id) && rr_cell_supports_gprs(gas_id))
              {
                if ( (rr_cell_supports_dtm(gas_id) == FALSE) && (rr_is_voice_call_active(gas_id)) )
                {

                  MSG_GERAN_HIGH_0_G("GERAN_GRR_TDS_SGLTE_SERVING_MEAS_REQ: Report NO SERVICE when in Active CS Call on a NON DTM Cell");
                  serving_cell_meas_valid = FALSE;
                }
              }
              else
              {
                serving_cell_meas_valid = FALSE;
              }

              if ( serving_cell_meas_valid)
              {
                rr_measured_signal_data_t * measured_signal_data = rr_get_measured_signal_data_ptr(gas_id);

                /* This request should only be received when GERAN is operating
                in CS-only / GSM-only mode. */
                RR_ASSERT_WARN(rr_sel_get_service_domain(gas_id) == SYS_SRV_DOMAIN_CS_ONLY);
                RR_ASSERT_WARN(rr_gsm_only(gas_id) == TRUE);

                rr_send_tds_sglte_serving_meas_rsp(
                  rr_gprs_get_scell_arfcn(gas_id),
                  TRUE,
                  rr_gprs_get_scell_bsic(gas_id),
                  measured_signal_data->current_signal_strength,
                  gas_id
                );
              }
              else
              {
                rr_send_tds_sglte_serving_meas_rsp_no_service(gas_id);
              }
            }
            else
            {
              MSG_GERAN_ERROR_0_G("Ignoring GERAN_GRR_TDS_SGLTE_SERVING_MEAS_REQ, SGLTE mode not active");
            }

            /* Message is processed, no need to pass it through to RR state machines. */
            rr_event = EV_NO_EVENT;
            break;
          } /* GERAN_GRR_TDS_SGLTE_SERVING_MEAS_REQ */
#endif /* FEATURE_GSM_TDS && FEATURE_SGLTE */

#ifdef FEATURE_LTE
#ifdef FEATURE_SGLTE
          case GERAN_GRR_LTE_SGLTE_NEIGHBOR_MEAS_REQ:
          {
            if (rr_is_sglte_mode_active(gas_id) &&
                rr_sglte_serving_cell_meas_valid(gas_id) &&
                rr_sglte_is_full_service_available(gas_id))
            {
              rr_measured_signal_data_t * measured_signal_data = rr_get_measured_signal_data_ptr(gas_id);
              MSG_GERAN_HIGH_0_G("Sending valid N + S cell measurement");

              rr_send_lte_sglte_neighbor_meas_rsp(
                rr_gprs_get_scell_arfcn(gas_id),
                TRUE,
                rr_gprs_get_scell_bsic(gas_id),
                measured_signal_data->current_signal_strength,
                gas_id
                );
             }
             else
             {
               MSG_GERAN_HIGH_0_G("Sending emnpty N cell measurement");
               rr_send_lte_sglte_neighbor_meas_rsp_no_service(gas_id);
             }
             rr_event = EV_NO_EVENT;
             break;
          } /*GERAN_GRR_LTE_SGLTE_NEIGHBOR_MEAS_REQ*/

          case GERAN_GRR_LTE_HO_REQ:
          {
            if (rr_is_sglte_mode_active(gas_id))
            {
              switch (get_rr_state(gas_id))
              {
                case RR_INACTIVE:
                {
                  if((!rr_sglte_saved_l2g_ho_being_processed(gas_id)) && (!rr_saved_l2g_msg_present(gas_id)))
                  {
                    MSG_GERAN_HIGH_0_G("Handling GERAN_GRR_LTE_HO_REQ in SGLTE mode, RR_INACTIVE for VOLTE+G");
                    rr_control_data_ptr->sglte_processng_saved_l2g_ho_msg = TRUE; /*set to behave as for SRVCC in SGLTE mode */
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("GERAN_GRR_LTE_HO_REQ after deact - process now");
                  }
                  break;
                }

                case RR_CELL_SELECTION:
                {
                  sys_codec_ho_info ho_codec_info = {0};
                  ho_codec_info.cmd = SYS_HO_START;
                  ho_codec_info.ho_type = SYS_HO_TYPE_LTE_GSM_SRVCC;
                  ho_codec_info.speech_codec = SYS_SPEECH_CODEC_NONE;
                  ho_codec_info.speech_enc_samp_freq = 0;
                  MSG_GERAN_HIGH_0_G("GERAN_GRR_LTE_HO_REQ while not in service - send failure in SGLTE mode");

                  geran_eng_mode_data_write(ENG_MODE_CODEC_HO_INFO,&ho_codec_info,TRUE,gas_id);

                  rr_sglte_send_lte_ho_failed_rsp(gas_id);
                  rr_send_srvcc_handover_failure_ind(gas_id);
                  rr_event = EV_NO_EVENT;
                  break;
                }

                default:
                {
                  if(rr_is_limited_plmn_selection_mode(gas_id))
                  {
                    sys_codec_ho_info ho_codec_info = {0};
                    ho_codec_info.cmd = SYS_HO_START;
                    ho_codec_info.ho_type = SYS_HO_TYPE_LTE_GSM_SRVCC;
                    ho_codec_info.speech_codec = SYS_SPEECH_CODEC_NONE;
                    ho_codec_info.speech_enc_samp_freq = 0;
                    
                    geran_eng_mode_data_write(ENG_MODE_CODEC_HO_INFO,&ho_codec_info,TRUE,gas_id);                    
                    MSG_GERAN_HIGH_0_G(
                      "GERAN_GRR_LTE_HO_REQ while in limited service - send failure in SGLTE mode"
                    );
                    rr_sglte_send_lte_ho_failed_rsp(gas_id);
                    rr_send_srvcc_handover_failure_ind(gas_id);
                    rr_event = EV_NO_EVENT;
                  }
                  else if(rr_save_l2g_msg(new_message_ptr,gas_id))
                  {
                    MSG_GERAN_HIGH_0_G(
                      "GERAN_GRR_LTE_HO_REQ saved - store cell info, trigger internal shut down in SGLTE mode"
                    );
                    rr_sglte_store_serving_cell_info(gas_id);

                    rr_control_data_ptr->sglte_processng_intrnl_stop_gsm = TRUE;
                    MSG_GERAN_HIGH_0_G("Send RR_STOP_GSM_MODE_REQ RR->RR");
                    rr_rr_stop_gsm_mode_req.message_header.message_set = MS_MM_RR;
                    rr_rr_stop_gsm_mode_req.message_header.message_id  = RR_STOP_GSM_MODE_REQ;
                    rr_rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason = RR_MM_RAT_CHANGE;
                    rr_rr_stop_gsm_mode_req.as_id = geran_map_gas_id_to_nas_id(gas_id);
                    PUT_IMH_LEN( sizeof(rr_stop_gsm_mode_req_T) - sizeof(IMH_T), &rr_rr_stop_gsm_mode_req.message_header );
                    rr_mm_send_message( &rr_rr_stop_gsm_mode_req.message_header, GS_QUEUE_RR, gas_id );
                    rr_event = EV_NO_EVENT;
                  }
              else
                  {
                    MSG_GERAN_HIGH_0_G("GERAN_GRR_LTE_HO_REQ not saved - send failure in SGLTE mode");
                    rr_sglte_send_lte_ho_failed_rsp(gas_id);
                    rr_event = EV_NO_EVENT;
                  }
                }
              }
            }
            else
            {
              MSG_GERAN_HIGH_0_G("GERAN_GRR_LTE_HO_REQ, SGLTE mode not active");
            }
            break;
          } /* GERAN_GRR_LTE_HO_REQ */
#endif /* FEATURE_SGLTE */

          case GERAN_GRR_LTE_GERAN_ACTIVITY_IND:
          {
            rr_event = rr_process_lte_geran_activity_ind(&(new_message_ptr->rr_msgr_msg.message.geran_grr_msg_ind.lte_geran_activity_ind), gas_id);
            break;
          }
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_BAND_AVOIDANCE
          case MCS_CXM_BAND_AVOID_BLIST_GSM1_IND:
          case MCS_CXM_BAND_AVOID_BLIST_GSM2_IND:
          {
            rr_band_avoidance_control(new_message_ptr, gas_id);
            rr_event = EV_NO_EVENT;
            break;
          }
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

#ifdef FEATURE_IDLE_DRX_SCALING
          case GERAN_GRR_MTC_CFG_REQ:
          {
            rr_idle_drx_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            rr_event = EV_NO_EVENT;
            break;
          }
#endif /* FEATURE_IDLE_DRX_SCALING */

          default:
          {
            break;
          }
        }
      }

      else if (message_set == MS_RR_RR)
      {
        switch (new_message_ptr->rr.header.rr_message_set)
        {
          case RR_IMSG:
          {
            rr_imsg_u *imsg;
            imsg = &new_message_ptr->rr.rr.imsg;

            switch (imsg->header.imh.message_id)
            {
              case RR_IMSG_RR_EVENT_IND:
              {
                rr_event = imsg->rr_event_ind.rr_event;

                MSG_SPRINTF_1(MSG_SSID_DFLT,
                              MSG_LEGACY_HIGH,
                              "RR_IMSG_RR_EVENT_IND contains %s",
                              rr_event_name(rr_event));
                break;
              }
              case RR_IMSG_CSI_MULTIPLE_INSTANCES_SIS_RETRIEVED_IND:
              {
                rr_event = EV_NO_EVENT;

                if (rr_is_camped(gas_id) && !rr_gsm_only(gas_id))
                {
                  MSG_GERAN_HIGH_0_G("All SIs retrieved from cache. Sending WCDMA neighbor list to L1");
                  rr_l1_update_wcdma_neighbor_list(gas_id);
                }
                break;
              }
              case RR_IMSG_SAVED_MSG_IND:
              {
                rr_event = EV_INPUT_MESSAGE;

                new_message_ptr = (rr_cmd_bdy_type *)&imsg->saved_msg_ind.msg.header;

                rr_extract_message_header_details(
                  EV_INPUT_MESSAGE,
                  new_message_ptr,
                  &message_set,
                  &message_id);

                MSG_SPRINTF_1(MSG_SSID_DFLT,
                              MSG_LEGACY_HIGH,
                              "RR_IMSG_SAVED_MSG_IND contains %s",
                              rr_gprs_message_name(
                                new_message_ptr,
                                new_message_ptr->message_header.message_set,
                                new_message_ptr->message_header.message_id,
                                NULL
                              )
                             );
                break;
              }

#ifdef FEATURE_MODEM_CONFIG_REFRESH
              case RR_IMSG_GERAN_NV_REFRESH_IND:
              {
                /* NV refresh is allowed only when GRR is INACTIVE*/
                if (rr_control_data_ptr->rr_state != RR_INACTIVE)
                {
                  MSG_GERAN_HIGH_0_G("Received RR_IMSG_GERAN_NV_REFRESH_IND while ACTIVE");
                  geran_nv_refresh_done(FALSE, gas_id);
                  rr_event = EV_NO_EVENT;
                }
                break;
              } /* RR_IMSG_GERAN_NV_REFRESH_IND */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

              default: ;
            }
            break;
          } /* RR_IMSG */

#ifdef FEATURE_SGLTE
          case RR_PLMN_LIST_OMSG:
          {
            switch (new_message_ptr->message_header.message_id)
            {
              /*
              There are three possible cases for receiving this signal:

              SGLTE X2G PLMN List Search has completed successfully
                Action: store the results to be sent back to originating RAT on next request.

              SGLTE X2G PLMN List Search was aborted by NAS
                Action: store the results to be sent back to originating RAT on next request.

              SGLTE X2G PLMN List Search was aborted by originating RRC
                Action: discard results and send abort confirmation to originating RRC.
              */

#if defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
              case RR_PLMN_LIST_OMSG_SGLTE_X2G_PLMN_LIST_CNF:
              {
                rr_plmn_list_omsg_sglte_x2g_plmn_list_cnf_t * sglte_x2g_plmn_list_cnf;

                RR_ASSERT_WARN(rr_control_data_ptr->sglte_x2g_plmn_list_search_in_progress == TRUE);

                sglte_x2g_plmn_list_cnf = &new_message_ptr->rr.plmn_list.omsg.sglte_x2g_plmn_list_cnf;

                if (rr_control_data_ptr->sglte_x2g_plmn_list_search_aborted_by_rrc)
                {
                  MSG_GERAN_HIGH_0_G("SGLTE X2G PLMN List Search aborted by RRC, responding");

                  rr_sglte_send_irat_plmn_srch_abort_cnf(
                    rr_control_data_ptr->sglte_x2g_plmn_list_search_originator,
                    gas_id
                  );

                  rr_control_data_ptr->sglte_x2g_plmn_list_search_in_progress = FALSE;
                  rr_control_data_ptr->sglte_x2g_plmn_list_search_aborted_by_rrc = FALSE;
                  rr_control_data_ptr->sglte_x2g_plmn_list_search_is_foreground = FALSE;

                  rr_sglte_x2g_plmn_list_search_results_clear(gas_id);
                }
                else
                {
                  MSG_GERAN_HIGH_0_G("SGLTE X2G PLMN List Search completed, storing results");

                  rr_control_data_ptr->sglte_x2g_plmn_list_search_results.srch_done = TRUE;
                  rr_control_data_ptr->sglte_x2g_plmn_list_search_results.hplmn_found = sglte_x2g_plmn_list_cnf->hplmn_found;
                  rr_control_data_ptr->sglte_x2g_plmn_list_search_results.available_plmn_list = GPRS_MEM_CALLOC(sizeof(sys_detailed_plmn_list_s_type));
                  if (rr_control_data_ptr->sglte_x2g_plmn_list_search_results.available_plmn_list != NULL)
                  {
                    *rr_control_data_ptr->sglte_x2g_plmn_list_search_results.available_plmn_list = sglte_x2g_plmn_list_cnf->available_plmn_list;
                  }
                  rr_control_data_ptr->sglte_x2g_plmn_list_search_results.detected_frequency_list = GPRS_MEM_CALLOC(sizeof(detected_frequency_list_type));
                  if (rr_control_data_ptr->sglte_x2g_plmn_list_search_results.detected_frequency_list != NULL)
                  {
                    *rr_control_data_ptr->sglte_x2g_plmn_list_search_results.detected_frequency_list = sglte_x2g_plmn_list_cnf->detected_frequency_list;
                  }

                  /* Now based on the Originator send the Response */
                  if ( rr_control_data_ptr->sglte_x2g_plmn_list_search_is_foreground )
                  {
#ifdef FEATURE_LTE
                    if ( rr_control_data_ptr->sglte_x2g_plmn_list_search_originator  == RR_RAT_LTE )
                    {
                        lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
                        errno_enum_type          result;


                        plmn_srch_rsp_ptr = (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));

                        RR_NULL_CHECK_RETURN_VOID(plmn_srch_rsp_ptr );
                        msgr_init_hdr(
                          &plmn_srch_rsp_ptr->msg_hdr,
                          MSGR_GERAN_GRR,
                          GERAN_GRR_LTE_PLMN_SRCH_RSP
                        );

                        plmn_srch_rsp_ptr->srch_done = TRUE;
                        if (rr_control_data_ptr->sglte_x2g_plmn_list_search_results.available_plmn_list != NULL)
                        {
                          plmn_srch_rsp_ptr->available_plmn_list = *rr_control_data_ptr->sglte_x2g_plmn_list_search_results.available_plmn_list;
                        }
                        rr_invalidate_csg_info(&plmn_srch_rsp_ptr->available_plmn_list);

                        plmn_srch_rsp_ptr->hplmn_found = rr_control_data_ptr->sglte_x2g_plmn_list_search_results.hplmn_found;
                        if (rr_control_data_ptr->sglte_x2g_plmn_list_search_results.detected_frequency_list != NULL)
                        {
                          plmn_srch_rsp_ptr->detected_frequency_list = *rr_control_data_ptr->sglte_x2g_plmn_list_search_results.detected_frequency_list;
                        }

                        result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);
                        if (result != E_SUCCESS)
                        {
                          MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                        }
                        GPRS_MEM_FREE(plmn_srch_rsp_ptr);

                        rr_control_data_ptr->sglte_x2g_plmn_list_search_in_progress = FALSE;

                        rr_control_data_ptr->sglte_x2g_plmn_list_search_is_foreground = FALSE;
                        rr_control_data_ptr->sglte_x2g_plmn_list_search_aborted_by_rrc = FALSE;

                        rr_sglte_x2g_plmn_list_search_results_clear(gas_id);
                    }
                    else 
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
                    if ( rr_control_data_ptr->sglte_x2g_plmn_list_search_originator  == RR_RAT_UTRAN_TDD )
                    {
                        rr_send_tds_interrat_plmn_search_cnf(
                          TRUE,
                          rr_control_data_ptr->sglte_x2g_plmn_list_search_results.hplmn_found,
                          rr_control_data_ptr->sglte_x2g_plmn_list_search_results.available_plmn_list,
                          gas_id
                        );

                        rr_control_data_ptr->sglte_x2g_plmn_list_search_in_progress = FALSE;
                        rr_control_data_ptr->sglte_x2g_plmn_list_search_is_foreground = FALSE;
                        rr_control_data_ptr->sglte_x2g_plmn_list_search_aborted_by_rrc = FALSE;

                        rr_sglte_x2g_plmn_list_search_results_clear(gas_id);
                    }
                    else
#endif /*FEATURE_GSM_TDS*/
                    {
                        MSG_GERAN_HIGH_0_G("SGLTE X2G PLMN List INVALID RAT to send Result");
                    }
                  }

                }
                rr_event = EV_NO_EVENT;
                break;
              } /* RR_PLMN_LIST_OMSG_SGLTE_X2G_PLMN_LIST_CNF */
#endif /* FEATURE_LTE  || FEATURE_GSM_TDS */

              default:
              {
                ; /* No action necessary. */
              }
            }
            break;
          } /* RR_PLMN_LIST_OMSG */
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_GSM_BAND_AVOIDANCE
          case RR_BAND_AVOID_IMSG:
          {
            rr_band_avoidance_control(new_message_ptr, gas_id);
            rr_event = EV_NO_EVENT;
            break;
          }
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

          default: ;
        }
      }
    } /* EV_INPUT_MESSAGE */

    /* if the event is EV_NO_EVENT, skip the state handlers */
    if (rr_event == EV_NO_EVENT)
    {
      continue;
    }
    else if (rr_event == EV_MAC_PS_ACCESS_GUARD_TIMER_EXPIRED)
    {
      if (geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_HIGH_0_G("MAC failed to respond to PS_ACCESS_REQ");
        geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
        rr_event = EV_NO_EVENT;
        continue;
      }
      else
      {
        MSG_GERAN_FATAL_0_G("MAC failed to respond to PS_ACCESS_REQ");
      }
      break;
    }

    /* handle the event in a state dependent manner */
    switch (rr_control_data_ptr->rr_state)
    {
      /***********************************************************************/
      /* RR_INACTIVE                                                         */
      /*                                                                     */
      /* Initial RR state. The GSM AS is deactivated, and L1 is INACTIVE     */
      /***********************************************************************/
      case RR_INACTIVE:
      {
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          switch (message_set)
          {
            case MS_MM_RR:
            {
              switch ((mm_rr_message_id_enum_T) message_id)
              {
                case RR_PLMN_SELECT_REQ:
                {
                  rr_plmn_select_req_T *rr_plmn_select_req;
                  sys_band_cap_u_type   band_cap_u;
                  sys_band_mask_type band_pref;
                  rr_plmn_select_req = &new_message_ptr->rr_plmn_select_req;

                  band_cap_u = rr_rat_pri_list_info_band_pref(
                    &rr_plmn_select_req->rat_pri_list_info,
                    SYS_SYS_MODE_GSM
                  );
                  band_pref = band_cap_u.chgwt_band_cap;

                  if (rr_validate_band_pref(band_pref))
                  {
                    rr_PLMN_request_details_ptr->rat_pri_list_info = rr_plmn_select_req->rat_pri_list_info;

                    /* Queue the request and handle it after GSM mode is active. */
                    if (rr_save_msg(new_message_ptr, gas_id))
                    {
                      /* Save the message in case there is a panic reset before the procedure completes */
                      rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);
                    }

                    new_rr_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                    rr_control_data_ptr->rr_state = RR_GOING_ACTIVE;
                  }
                  else
                  {
                    rr_control_data_ptr->rr_invalid_band_pref = band_pref;
                    rr_report_invalid_band_pref_received(gas_id);
                  }
                  break;
                }

                case RR_PLMN_LIST_REQ:
                {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                  /* check that NAS isn't asking an inactive subscription */
                  /* to perform a PLMN list search in single standby mode */
                  if (rr_ms_plmn_list_search_valid(RR_INACTIVE, gas_id) == FALSE)
                  {
                    rr_plmn_list_req_T *req_ptr = &new_message_ptr->rr_plmn_list_req;

                    /* reject the request immediately */
                    rr_send_plmn_list_cnf(NULL, MM_AS_CONNECTED, req_ptr, gas_id);
                    new_rr_event = EV_NO_EVENT;
                    break;
                  }
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

                  if (rr_save_msg(new_message_ptr, gas_id))
                  {
                    rr_plmn_list_req_T *plmn_list_req = &new_message_ptr->rr_plmn_list_req;

                    rr_PLMN_request_details_ptr->rat_pri_list_info = plmn_list_req->rat_pri_list_info;

                    /* Save the message in case there is a panic reset before the procedure completes */
                    rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                    new_rr_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                    rr_control_data_ptr->rr_state = RR_GOING_ACTIVE;
                  }
                  else
                  {
                    MSG_GERAN_ERROR_0_G("must be able to start PLMN search in INACTIVE");
                  }
                  break;
                }

                case RR_STOP_GSM_MODE_REQ:
                {
                  /* Handle deep sleep request, may need to switch GSM L1 to deep sleep mode */
                  if ((new_message_ptr->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_MM_DEEP_SLEEP) ||
                      (new_message_ptr->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_MM_DUAL_SWITCH))
                  {
                    if (rr_mode_get_l1_stop_reason(gas_id) != RR_L1_DEEP_SLEEP)
                        {
                      rr_mode_set_l1_stop_reason(RR_L1_DEEP_SLEEP, gas_id);
                      rr_l1_send_mph_deep_sleep_ind(gas_id);
                    }
                  }
                    (void)rr_process_rr_stop_gsm_mode_req(FALSE, gas_id);
                    (void)rr_gsm_as_deactivation_complete(gas_id);
                  break;
                }

                case RR_DEACT_REQ:
                {
                  /* perform checks to ensure GL1 is rightly put in deep sleep mode */
                  if ((rr_mode_get_l1_stop_reason(gas_id) != rr_l1_gsm_stop_reason_get(gas_id)) &&
                      (rr_l1_gsm_stop_reason_get(gas_id) == RR_L1_DEEP_SLEEP))
                  {
                    
                    MSG_GERAN_HIGH_1_G("RR_DEACT_REQ(reason=%d) received, GL1 is now requested to deep sleep",
                                       new_message_ptr->rr_deact_req.deact_reason );

                    /* Inform GL1 to move to deep sleep mode */
                    rr_mode_set_l1_stop_reason(RR_L1_DEEP_SLEEP, gas_id);
                    rr_l1_send_mph_deep_sleep_ind(gas_id);
    
                    /* Complete deactivation in normal way */
                    rr_ps_no_ps_access_del_all(gas_id);
                    rr_init_modem_offline(gas_id);
                  
                    /* only write NV if deactivate reason is RR_MM_POWER_OFF or RR_MM_LPM_POWER_OFF */
                    rr_acq_db_save(gas_id);

                    /* Stop other RATs, if needed, before sending RR_DEACT_CNF */
                    if (FALSE == rr_other_rat_deactivation_required(gas_id))
                    {
                      rr_send_deact_cnf(gas_id);
                    }
                  }
                  else
                  {
                    rr_send_deact_cnf(gas_id);
                  }
                  break;
                }

                /* The following MM->RR messages aren't expected here, but
                could be received following a lower layer failure if MM had
                sent them before it received the NO SERVICE indication. */

                case RR_EST_REQ:
                {
                  rr_send_abort_ind(RR_CONNECTION_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
                  break;
                }

                case RR_ABORT_REQ:
                {
                  rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);
                  break;
                }

#ifdef FEATURE_LTE

                case RR_LTE_RESELECTION_ALLOWED_STATUS_IND:
                {
                  MSG_GERAN_MED_0_G("Ignoring RR_LTE_RESELECTION_ALLOWED_STATUS_IND in RR_INACTIVE");
                  break;
                } /* RR_LTE_RESELECTION_ALLOWED_STATUS_IND */

#endif /* FEATURE_LTE */

                case RR_GMM_GPRS_STATE_CHANGE_REQ:
                {
                  break;  // no action required
                }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
               
                case RR_PSEUDO_LTE_LIST_UPDATE_IND:
                {
                  break;  // no action required
                }
#endif

#ifdef FEATURE_SGLTE
                case RR_CHANGE_MODE_IND:
                {
                  MSG_GERAN_HIGH_0_G("Sending no-service indication to NAS");
                  rr_send_no_service_ind(gas_id);
                  break;
                } /* RR_CHANGE_MODE_IND */
#endif /* FEATURE_SGLTE */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_MM_RR */

            case MS_MAC_RR:
            {
              switch ((mac_grr_sig_id_t) message_id)
              {
                case MAC_GRR_MAC_STATUS:
                {
                  MSG_GERAN_MED_1_G(
                    "MAC_GRR_MAC_STATUS(%d) received",
                    new_message_ptr->mac_grr_sig.mac_grr_mac_status.mac_status
                  );
                  break;
                }


                case MAC_GRR_RESET_CNF:
                {
                  MSG_GERAN_HIGH_0_G("MAC/RLC reset confirmed");
                  break;
                }


                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_MAC_RR */

            case MS_RR_L2:
            {
              switch ((rr_l2_message_id_enum_T) message_id)
              {
                case DL_RELEASE_IND:
                case DL_RELEASE_CNF:
                {
                  /* No action necessary - it is possible to receive these
                  messages in this state during a successful G2W Handover
                  procedure. */
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_RR_L2 */

            case MS_RR_L1:
            {
              switch ((rr_l1_message_id_enum_T) message_id)
              {
#ifdef FEATURE_MODEM_CONFIG_REFRESH
                case MPH_NV_REFRESH_CNF:
                {
                  /* GL1 has completed NV refresh. */
                  MSG_GERAN_HIGH_0_G("NV Refresh CNF rcvd from GL1");
                  geran_nv_refresh_done(TRUE, gas_id);
                  break;
                }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr, 
                    rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_RR_L1 */
            
            case MS_TIMER:
            {
              switch (new_message_ptr->mid_timer_expiry.timer_id)
              {
                case RR_INVALID_BAND_PREF_TIMER:
                {
                  rr_report_invalid_band_pref_received(gas_id);
                  break;
                } /* RR_INVALID_BAND_PREF_TIMER */

                case T3122:
                {
                  // no action required - NAS-MM will get notified when the next RR_SERVICE_IND is sent
                  MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_TIMER_EXPIRY(
                    new_message_ptr->mid_timer_expiry.timer_id,
                    rr_main_state_name(rr_control_data_ptr->rr_state)
                  );
                }
              }
              break;
            }

            case MS_RR_RR:
            {
              switch (new_message_ptr->rr.header.rr_message_set)
              {
                case RR_IMSG:
                {
                  switch (new_message_ptr->rr.header.imh.message_id)
                  {
                    case RR_IMSG_RR_EVENT_IND:
                    {
                      MSG_GERAN_HIGH_0_G("RR_IMSG_RR_EVENT_IND");
                      break;
                    }

                    case RR_IMSG_PS_ACCESS_REQ:
                    {
                      // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                      rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                      // Call the call-back function (if one was supplied)
                      CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                      break;
                    }

                    case RR_IMSG_NO_PS_ACCESS_REQ:
                    {
                      // No PS access request (could be TLLI unassignment, CS-only)

                      rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                      // Call the call-back function (if one was supplied)
                      CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                      break;
                    }

                    case RR_IMSG_MSC_CHANGE_IND:
                    {
                      /*
                      Update NAS as required
                      RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                      This will be sent when G is next active
                      */
                      rr_msc_update(
                        FALSE,   // can_send_service_ind
                        gas_id   // gas_id
                      );
                      break;
                    }

#ifdef FEATURE_SGLTE
                    case RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ:
                    case RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ:
                    {
                      MSG_GERAN_HIGH_0_G("RR_STOP_GSM_MODE_REQ(RR_SGLTE_RAT_CHANGE/RR_SGLTE_USER_RAT_CHANGE) in RR_INACTIVE");
                      rr_send_stop_gsm_mode_cnf(MM_AS_SUCCESS, gas_id);
                      break;
                    } /* RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ, RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ */
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
                    case RR_IMSG_GERAN_NV_REFRESH_IND:
                    {
                      MSG_GERAN_HIGH_0_G("Received RR_IMSG_GERAN_NV_REFRESH_IND in RR_INACTIVE");

                      /* Refresh GRR EFS/NV items */
                      rr_nv_process_nv_refresh_ind(gas_id);

                      /* Send message to GL1 to refresh GL1 EFS/NV items */
                      rr_l1_send_mph_nv_refresh_req(gas_id);
                      break;
                    } /* RR_IMSG_GERAN_NV_REFRESH_IND */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_INACTIVE");
                    }
                  }
                  break;
                } /* RR_IMSG */

#ifdef FEATURE_FEMTO_GSM_NL
                case RR_FEMTO_OMSG:
                {
                  switch (new_message_ptr->rr.header.imh.message_id)
                  {
                    case RR_FEMTO_OMSG_ACTIVE_IND:
                    {
                      rr_control_data_ptr->rr_state = RR_FEMTO;
                      break;
                    }

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message_ptr,
                        rr_main_state_name(rr_control_data_ptr->rr_state)
                      );
                    }
                  }
                  break;
                }
#endif // FEATURE_FEMTO_GSM_NL

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message_ptr,
                    rr_main_state_name(rr_control_data_ptr->rr_state)
                  );
                }
              }
              break;
            } /* MS_RR_RR */

#ifdef FEATURE_GSM_TDS
            // TODO DSDA
            case MS_TDSRRC_RR:
            {
              switch ((tdsrr_rrc_cmd_e_type) message_id)
              {
                case TDSRRC_STOP_TDS_CNF:
                {
                  rr_control_data_ptr->tds_stop_mode_cnf_rxd = TRUE;

                  if (rr_control_data_ptr->tds_stop_mode_cnf_rxd &&
                      rr_control_data_ptr->wcdma_stop_mode_cnf_rxd)
                  {
                    rr_send_deact_cnf(gas_id);
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("RR waiting for RRC_RR_STOP_WCDMA_CNF");
                  }
                  break;
                } /*TDSRRC_STOP_TDS_CNF*/

                case TDSRR_INTERRAT_RESELECTION_REQ:
                {

#ifdef FEATURE_SGLTE
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                     rr_sglte_reject_tds_resel_req(gas_id);
                  }
                  else
#endif /* FEATURE_SGLTE */
                  {

                    tdsrr_interrat_reselect_req_type * rr_interrat_reselect_req = &new_message_ptr->tdsrr_resel_req;
                    BSIC_T bsic;

                    rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                    rr_PLMN_request_details_ptr->rat_pri_list_info = rr_interrat_reselect_req->rat_pri_list_info;
                    rr_PLMN_request_details_ptr->network_select_mode = rr_interrat_reselect_req->network_select_mode;
                    rr_store_plmn_id(&rr_interrat_reselect_req->requested_PLMN, gas_id);

                    bsic.PLMN_colour_code = rr_interrat_reselect_req->BSIC_NCC;
                    bsic.BS_colour_code = rr_interrat_reselect_req->BSIC_BCC;

#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
                    /* Store Dedicated Priority information. */
                    rr_dedicated_priority_info_store(
                      &rr_interrat_reselect_req->dedicated_priority_info,
                      &rr_interrat_reselect_req->BCCH_ARFCN,
                      rr_interrat_reselect_req->requested_PLMN,
                      gas_id
                    );
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/

                    rr_x2g_resel_init(gas_id);

                    rr_x2g_resel_send_imsg_resel_req(
                      rr_interrat_reselect_req->BCCH_ARFCN,
                      rr_interrat_reselect_req->RXLEV_average,
                      bsic,
                      MM_AS_INTERRAT_RESELECTION,
                      rr_is_any_cell_reselection_mode(gas_id),
                      &rr_interrat_reselect_req->requested_PLMN,
                      NULL,
                      gas_id
                    );

                    rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_T2G_RESELECTION;
                    rr_control_data_ptr->rr_state = RR_X2G_RESEL;
                  }
                  break;
                } /* TDSRR_INTERRAT_RESELECTION_REQ */

                /* No T2G reselection is in progress, so send back a
                confirmation immediately. */
                case TDSRR_INTERRAT_RESELECTION_ABORT_REQ:
                {
                  rr_send_tds_interrat_reselection_abort_cnf(gas_id);
                  break;
                } /* TDSRR_INTERRAT_RESELECTION_ABORT_REQ */


                case TDSRR_INTERRAT_CC_ORDER_REQ:
                {

#ifdef FEATURE_SGLTE
                   if (rr_is_sglte_mode_active(gas_id))
                   {
                      rr_x2g_send_x2g_cco_rsp_rejection(RR_RAT_UTRAN_TDD, gas_id);
                   }
                   else
#endif /* FEATURE_SGLTE */
                   {
                     tdsrr_interrat_cc_order_req_type * req = &new_message_ptr->tdsrr_cco_req;
                     BSIC_T bsic;

                     rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                     rr_PLMN_request_details_ptr->rat_pri_list_info = req->rat_pri_list_info;
                     rr_PLMN_request_details_ptr->network_select_mode = req->network_select_mode;

                     bsic.PLMN_colour_code = req->BSIC_NCC;
                     bsic.BS_colour_code = req->BSIC_BCC;

#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
                     if (rr_dedicated_priority_info_valid(&req->dedicated_priority_info))
                     {
                       rr_control_data_ptr->dedicated_priority_info = req->dedicated_priority_info;
                     }
                     else
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/
                     {
                       memset(&rr_control_data_ptr->dedicated_priority_info,0, sizeof(rr_control_data_ptr->dedicated_priority_info));
                     }

                     MSG_GERAN_HIGH_1_G("Starting T2G CCO procedure (T309: %d ms)", req->T309_value * 1000);
                     (void)rr_timer_start(T3174, req->T309_value * 1000, gas_id);

                     rr_x2g_resel_init(gas_id);

                     rr_x2g_resel_send_imsg_resel_req(
                     req->BCCH_ARFCN,
                     /* TDSCDMA RRC does not set the RXLEV_average field, so we use
                     a hard-coded value here. */
                     /* req->RXLEV_average, */
                     0xff,
                     bsic,
                     MM_AS_WTOG_INTERRAT_CC_ORDER,
                     rr_is_any_cell_reselection_mode(gas_id),
                     NULL,
                     NULL,
                     gas_id
                     );

                     rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER;

                     rr_control_data_ptr->rr_state = RR_X2G_RESEL;
                   }
                  break;
                } /* TDSRR_INTERRAT_CC_ORDER_REQ */

                case TDSRR_INTERRAT_HANDOVER_REQ:
                {
                  /* MUST change state before calling the state machine, as
                  rr_get_band() will result in the RR state being checked to
                  determine which band indicator to use. */
                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);
                  rr_control_data_ptr->rr_state = RR_W2G_INTERRAT_HANDOVER_PROGRESS;
                  new_rr_event = rr_inter_cell_handover_control(
                    EV_INPUT_MESSAGE,
                    new_message_ptr,
                    gas_id
                  );
                  break;
                } /*TDSRR_INTERRAT_HANDOVER_REQ*/

                case TDSRR_INTERRAT_PLMN_SRCH_REQ:
                {
                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                  rr_start_ttog_plmn_search((tdsrr_interrat_plmn_srch_req_type *)new_message_ptr, gas_id);
                  new_rr_event = rr_plmn_list_control(EV_PLMN_SEARCH_START_TTOG, NULL, gas_id);
                  rr_control_data_ptr->rr_state = RR_PLMN_LIST_SEARCH;
                  break;
                } /*TDSRR_INTERRAT_PLMN_SRCH_REQ*/

                case TDSRR_INTERRAT_REDIRECT_REQ:
                {

#ifdef FEATURE_SGLTE
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    rr_sglte_reject_tds_redir_req(gas_id);
                  }
                  else
#endif /* FEATURE_SGLTE */
                  {
                    rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                    new_rr_event = rr_w2g_service_redirection_control(
                      EV_INPUT_MESSAGE,
                      new_message_ptr,
                      gas_id
                    );
                    rr_control_data_ptr->rr_state = RR_W2G_SERVICE_REDIRECTION_IN_PROGRESS;
                  }
                  break;
                }

                case TDSRR_INTERRAT_REDIRECT_ABORT_REQ:
                {
                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr,gas_id);

                  /* Don't change state here as there is no service redirection
                  in progress so the service redirection state machine should just
                  send an abort confirm. */
                  new_rr_event = rr_w2g_service_redirection_control(
                    EV_INPUT_MESSAGE,
                    new_message_ptr,
                    gas_id
                  );
                  break;
                }

                /* This signal is an indication from TDSRRC that it is being
                powered down.  RR should write it's ACQ-DB to NV then respond
                to TDSRRC to allow it to continue with it's deactivation and
                confirm it's completion to NAS. */
                case TDSRR_STOP_GSM_REQ:
                {
                  tdsrr_stop_gsm_req_type *stop_gsm_req = &new_message_ptr->tdsrr_stop_gsm_req;

                  switch (stop_gsm_req->stop_req_cause)
                  {
                    case TDSRRC_POWER_OFF:
                    {
                      MSG_GERAN_MED_0_G("TDSRR_STOP_GSM_REQ (POWER_OFF) - save RR ACQ-DB");

                      // Write to NV - RR task is blocked while this happens
                      rr_acq_db_save(gas_id);

                      // Confirm to TDS-RRC
                      rr_send_tds_stop_gsm_mode_cnf(gas_id);

                      break;
                    }

                    case TDSRRC_MODE_LPM:
                    {
                      MSG_GERAN_HIGH_0_G("TDSRR_STOP_GSM_REQ (MODE_LPM) - save RR ACQ-DB, put GL1 in LPM");

                      // Write to NV - RR task is blocked while this happens
                      rr_acq_db_save(gas_id);

                      // Remember L1 is put in DEEP SLEEP 
                      rr_l1_gsm_stop_reason_set(RR_L1_DEEP_SLEEP, gas_id);
                      rr_mode_set_l1_stop_reason(RR_L1_DEEP_SLEEP, gas_id);

                      /* Put GL1 in LPM and send CNF back to T */
                      rr_l1_send_mph_deep_sleep_ind(gas_id);
                      rr_send_tds_stop_gsm_mode_cnf(gas_id);

                      break;
                    }

                    default:
                    {
                      MSG_GERAN_MED_1_G("TDSRR_STOP_GSM_REQ (%d) - no action", stop_gsm_req->stop_req_cause);

                      // Confirm to TDS-RRC
                      rr_send_tds_stop_gsm_mode_cnf(gas_id);
                    }
                  }

                  break;
                } /* RR_RRC_STOP_GSM_REQ */

#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
                case TDSRR_CLEAR_DEDICATED_PRIORITIES_IND:
                {
                  rr_individual_priorities_clear(gas_id);
                  break;
                } /* TDSRR_CLEAR_DEDICATED_PRIORITIES_IND */

                case TDSRR_DEDICATED_PRIORITIES_REQ:
                {
                  rr_send_tds_dedicated_priorities_rsp(gas_id);
                  break;
                } /* TDSRR_DEDICATED_PRIORITIES_REQ */
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message_ptr,
                    rr_main_state_name(rr_control_data_ptr->rr_state)
                  );
                  break;
                }
              }
              break;
            }
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA

            case MS_RRC_RR:
            {
              switch ((rrc_rr_cmd_e_type) message_id)
              {
                case RR_INTERRAT_HANDOVER_REQ:
                {
                  /* MUST change state before calling the state machine, as
                  rr_get_band() will result in the RR state being checked to
                  determine which band indicator to use. */
                  rr_control_data_ptr->rr_state = RR_W2G_INTERRAT_HANDOVER_PROGRESS;

                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                  new_rr_event = rr_inter_cell_handover_control(
                    EV_INPUT_MESSAGE,
                    new_message_ptr,
                    gas_id
                  );
                  break;
                }

                /* W2G reselection is triggered. */
                case RR_INTERRAT_RESELECTION_REQ:
                {
                  rr_interrat_reselect_req_type * rr_interrat_reselect_req = &new_message_ptr->rr_interrat_reselect_req;
                  BSIC_T                          bsic;

                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                  rr_PLMN_request_details_ptr->rat_pri_list_info = rr_interrat_reselect_req->rat_pri_list_info;
                  rr_PLMN_request_details_ptr->network_select_mode = rr_interrat_reselect_req->network_select_mode;
                  rr_store_plmn_id(&rr_interrat_reselect_req->requested_PLMN, gas_id);

                  bsic.PLMN_colour_code = rr_interrat_reselect_req->BSIC_NCC;
                  bsic.BS_colour_code = rr_interrat_reselect_req->BSIC_BCC;

#ifdef FEATURE_LTE
                  /* Store Dedicated Priority information. */
                  rr_dedicated_priority_info_store(
                    &rr_interrat_reselect_req->dedicated_priority_info,
                    &rr_interrat_reselect_req->BCCH_ARFCN,
                    rr_interrat_reselect_req->requested_PLMN,
                    gas_id
                  );
#endif /* FEATURE_LTE */

                  rr_x2g_resel_init(gas_id);

                  rr_x2g_resel_send_imsg_resel_req(
                    rr_interrat_reselect_req->BCCH_ARFCN,
                    rr_interrat_reselect_req->RXLEV_average,
                    bsic,
                    MM_AS_INTERRAT_RESELECTION,
                    rr_is_any_cell_reselection_mode(gas_id),
                    &rr_interrat_reselect_req->requested_PLMN,
                    NULL,
                    gas_id
                  );

                  rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_W2G_RESELECTION;
                  rr_control_data_ptr->rr_state = RR_X2G_RESEL;
                  break;
                } /* RR_INTERRAT_RESELECTION_REQ */

                /* No W2G reselection is in progress, so send back a
                confirmation immediately. */
                case RR_INTERRAT_RESELECTION_ABORT_REQ:
                {
                  rr_send_interrat_reselection_abort_cnf(gas_id);
                  break;
                } /* RR_INTERRAT_RESELECTION_ABORT_REQ */

                case RR_INTERRAT_CC_ORDER_REQ:
                {
                  rr_interrat_cc_order_req_type * req = &new_message_ptr->rr_interrat_cc_order_req;
                  BSIC_T                          bsic;
                  rr_x2g_system_information_t     system_information;

                  memset(&system_information, 0, sizeof(rr_x2g_system_information_t));

                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                  rr_PLMN_request_details_ptr->rat_pri_list_info = req->rat_pri_list_info;
                  rr_PLMN_request_details_ptr->network_select_mode = req->network_select_mode;
//                  rr_store_plmn_id(&req->requested_PLMN);

                  bsic.PLMN_colour_code = req->BSIC_NCC;
                  bsic.BS_colour_code = req->BSIC_BCC;

                  if (req->geran_sysinfo_choice == GERAN_INFO_TYPE_SI &&
                      req->num_blocks > 0)
                  {
                    uint32 i;

                    for (i = 0;
                         i < req->num_blocks &&
                         i < RRC_RR_MAX_SI_BLOCKS &&
                         i < RR_X2G_SYSTEM_INFORMATION_MESSAGES_MAX;
                         ++i)
                    {
                      const uint8 max_si_message_length = MIN(GERAN_GRR_SI_MESSAGE_MAX_LENGTH, RRC_RR_MAX_SI_BLOCK_LENGTH);

                      system_information.messages[i].length = MIN(req->geran_SI[i].msg_length, max_si_message_length);

                      memscpy(
                        system_information.messages[i].data,
                        sizeof(system_information.messages[i].data),
                        req->geran_SI[i].msg_string,
                        system_information.messages[i].length
                      );
                    }
                    system_information.count = i;

                    /* Tunnelled system information messages received from WCDMA
                    RRC do not usually include the first two octets of each
                    message.  Before blindly adding the headers we do check for
                    their presence first though, to allow for flexibility in
                    network implementations. */
                    if (!rr_x2g_system_information_headers_present(
                           &system_information
                         )
                       )
                    {
                      rr_x2g_system_information_prepend_headers(
                        &system_information,
                        gas_id  //tlx
                      );
                    }
                  }

#ifdef FEATURE_LTE
                  if (rr_dedicated_priority_info_valid(&req->dedicated_priority_info))
                  {
                    rr_control_data_ptr->dedicated_priority_info = req->dedicated_priority_info;
                  }
                  else
                  {
                    memset(&rr_control_data_ptr->dedicated_priority_info,
                           0, sizeof(rr_control_data_ptr->dedicated_priority_info));
                  }
#endif /* FEATURE_LTE */

                  MSG_GERAN_HIGH_1_G("Starting W2G CCO procedure (T309: %d ms)", req->T309_value * 1000);
                  (void)rr_timer_start(T3174, req->T309_value * 1000, gas_id);

                  rr_x2g_resel_init(gas_id);

                  rr_x2g_resel_send_imsg_resel_req(
                    req->BCCH_ARFCN,
                    /* WCDMA RRC does not set the RXLEV_average field, so we use
                    a hard-coded value here. */
                    /* req->RXLEV_average, */
                    0xff,
                    bsic,
                    MM_AS_WTOG_INTERRAT_CC_ORDER,
                    rr_is_any_cell_reselection_mode(gas_id),
                    NULL,
                    &system_information,
                    gas_id
                  );

                  rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER;

                  rr_control_data_ptr->rr_state = RR_X2G_RESEL;
                  break;
                } /* RR_INTERRAT_CC_ORDER_REQ */

                case RR_INTERRAT_PLMN_SRCH_REQ:
                {
                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                  rr_start_wtog_plmn_search((rr_interrat_plmn_srch_req_type *)new_message_ptr, gas_id);
                  new_rr_event = rr_plmn_list_control(EV_PLMN_SEARCH_START_WTOG, NULL, gas_id);
                  rr_control_data_ptr->rr_state = RR_PLMN_LIST_SEARCH;
                  break;
                }

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG

                case RR_INTERRAT_REDIRECT_REQ:
                {
                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                  new_rr_event = rr_w2g_service_redirection_control(
                    EV_INPUT_MESSAGE,
                    new_message_ptr,
                    gas_id
                  );
                  rr_control_data_ptr->rr_state = RR_W2G_SERVICE_REDIRECTION_IN_PROGRESS;
                  break;
                }

                case RR_INTERRAT_REDIRECT_ABORT_REQ:
                {
                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                  /* Don't change state here as there is no service redirection
                  in progress so the service redirection state machine should just
                  send an abort confirm. */
                  new_rr_event = rr_w2g_service_redirection_control(
                    EV_INPUT_MESSAGE,
                    new_message_ptr,
                    gas_id
                  );
                  break;
                }

#endif /* #ifdef FEATURE_INTERRAT_REDIRECTION_WTOG */

                /* This signal is an indication from RRC that it is being
                powered down.  RR should write it's ACQ-DB to NV then respond
                to RRC to allow it to continue with it's deactivation and
                confirm it's completion to NAS. */
                case RR_RRC_STOP_GSM_REQ:
                {
                  rr_rrc_stop_gsm_req_type *stop_gsm_req = &new_message_ptr->rr_rrc_stop_gsm_req;

                  switch (stop_gsm_req->stop_req_cause)
                  {
                    case POWER_OFF:
                    {
                      MSG_GERAN_MED_0_G("RR_RRC_STOP_GSM_MODE_REQ (POWER_OFF) - save RR ACQ-DB");

                      // Write to NV - RR task is blocked while this happens
                      rr_acq_db_save(gas_id);

                      // Confirm to RRC
                      rr_send_rrc_stop_gsm_mode_cnf(gas_id);

                      break;
                    }

                    case MODE_LPM:
                    {
                      MSG_GERAN_HIGH_0_G("RR_RRC_STOP_GSM_MODE_REQ (MODE_LPM) - save RR ACQ-DB, put GL1 in LPM");

                      // Write to NV - RR task is blocked while this happens
                      rr_acq_db_save(gas_id);

                      // Remember L1 is put in DEEP SLEEP 
                      rr_l1_gsm_stop_reason_set(RR_L1_DEEP_SLEEP, gas_id);
                      rr_mode_set_l1_stop_reason(RR_L1_DEEP_SLEEP, gas_id);

                      /* Put GL1 in LPM and send CNF back to W */
                      rr_l1_send_mph_deep_sleep_ind(gas_id);
                      rr_send_rrc_stop_gsm_mode_cnf(gas_id);

                      break;
                    }

                    default:
                    {
                      MSG_GERAN_MED_1_G("RR_RRC_STOP_GSM_MODE_REQ (%d) - no action", stop_gsm_req->stop_req_cause);

                      // Confirm to RRC
                      rr_send_rrc_stop_gsm_mode_cnf(gas_id);
                    }
                  }

                  break;
                } /* RR_RRC_STOP_GSM_REQ */

                /* This signal is an indication that RRC has completed writing
                it's ACQ-DB to NV (in response to an earlier RRC_RR_STOP_WCDMA_REQ.
                RR only sends RRC_RR_STOP_WCDMA_REQ after receiving an RR_DEACT_REQ
                from NAS, and now on receipt of this confirmation from RRC RR can
                finally send RR_DEACT_CNF to NAS.  At this point, shut-down of the
                access stratum is complete. */
                case RRC_RR_STOP_WCDMA_CNF:
                {
                  rr_control_data_ptr->wcdma_stop_mode_cnf_rxd = TRUE;

                  if ( rr_control_data_ptr->wcdma_stop_mode_cnf_rxd
#ifdef FEATURE_GSM_TDS
                      && rr_control_data_ptr->tds_stop_mode_cnf_rxd
#endif /*FEATURE_GSM_TDS*/
                     )
                  {
                    rr_send_deact_cnf(gas_id);
                  }
#ifdef FEATURE_GSM_TDS
                  else
                  {
                    MSG_GERAN_HIGH_0_G("RR waiting for TDSRRC_STOP_TDS_REQ");
                  }
#endif /*FEATURE_GSM_TDS*/
                  break;
                } /* RRC_RR_STOP_WCDMA_CNF */

#ifdef FEATURE_LTE
                case RR_CLEAR_DEDICATED_PRIORITIES_IND:
                {
                  rr_individual_priorities_clear(gas_id);
                  break;
                } /* RR_CLEAR_DEDICATED_PRIORITIES_IND */

                case RR_DEDICATED_PRIORITIES_REQ:
                {
                  rr_send_rrc_dedicated_priorities_rsp(gas_id);
                  break;
                } /* RR_DEDICATED_PRIORITIES_REQ */
#endif /* FEATURE_LTE */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_RRC_RR */

#endif /* FEATURE_WCDMA */

            case MS_MSGR_RR:
            {
              switch (new_message_ptr->rr_msgr_msg.message.hdr.id)
              {
#ifdef FEATURE_LTE
                case GERAN_GRR_LTE_RESEL_REQ:
                {
                  geran_grr_lte_resel_req_t * req = &new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_resel_req;
                  BSIC_T bsic;
                  ARFCN_T arfcn;
                  uint8 rssi_new;

                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                  rr_PLMN_request_details_ptr->rat_pri_list_info = req->rat_pri_list;
                  rr_PLMN_request_details_ptr->network_select_mode = req->network_select_mode;
                  rr_store_plmn_id(&req->requested_PLMN, gas_id);
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                  rr_ms_x2g_acquisition_started(gas_id);
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

                  arfcn.num = req->arfcn;
                  if (req->band_ind == GERAN_GRR_BAND_IND_PCS_1900)
                  {
                    arfcn.band = (sys_band_T) rr_get_band_using_band_indicator(
                      arfcn.num,
                      SI_PSI_BAND_IND_PCS1900,
                      gas_id
                    );
                  }
                  else
                  {
                    arfcn.band = (sys_band_T) rr_get_band_using_band_indicator(
                      arfcn.num,
                      SI_PSI_BAND_IND_DCS1800,
                      gas_id
                    );
                  }
                  bsic.PLMN_colour_code = req->cell_id.ncc;
                  bsic.BS_colour_code = req->cell_id.bcc;

                  /* Store Dedicated Priority information. */
                  rr_dedicated_priority_info_store(
                    &req->dedicated_priority_info,
                    &arfcn,
                    req->requested_PLMN,
                    gas_id
                  );

                  rr_x2g_resel_init(gas_id);

                  MSG_HIGH("RSSI received in GERAN_GRR_LTE_RESEL_REQ is %d", req->rssi, 0, 0);                    
                  if (req->rssi <= dBm_x16_VALUE_FOR_RXLEV_0)
                  {
                    req->rssi = dBm_x16_VALUE_FOR_RXLEV_0; /* floor */
                  } 
                  else if (req->rssi > dBm_x16_VALUE_FOR_RXLEV_63)
                  {
                    req->rssi = dBm_x16_VALUE_FOR_RXLEV_63; /* saturate */
                  }
                                       
                  rssi_new = (uint8)(((dBx16_T)(req->rssi - dBm_x16_VALUE_FOR_RXLEV_0))/16); 

                  MSG_HIGH("rxlev after RSSI to RXLEV conversion is %d", rssi_new, 0, 0);

                  rr_x2g_resel_send_imsg_resel_req(
                    arfcn,
                    rssi_new,
                    bsic,
                    MM_AS_INTERRAT_RESELECTION,
                    rr_is_any_cell_reselection_mode(gas_id),
                    &req->requested_PLMN,
                    NULL,
                    gas_id
                  );

                  rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_L2G_RESELECTION;

                  rr_control_data_ptr->rr_state = RR_X2G_RESEL;
                  break;
                } /* GERAN_GRR_LTE_RESEL_REQ */

                // TODO DSDA
                case GERAN_GRR_LTE_CCO_REQ:
                {
                  geran_grr_lte_cco_req_t     * req = &new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_cco_req;
                  BSIC_T                        bsic;
                  ARFCN_T                       arfcn;
                  PLMN_id_T                   * plmn_id;
                  rr_x2g_system_information_t   system_information;

                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                  memset(&system_information, 0, sizeof(rr_x2g_system_information_t));

                  rr_PLMN_request_details_ptr->rat_pri_list_info = req->rat_pri_list;

                  rr_PLMN_request_details_ptr->network_select_mode = req->network_select_mode;
                  rr_store_plmn_id(&req->requested_PLMN, gas_id);

                  arfcn.num = req->arfcn;
                  if (req->band_ind == GERAN_GRR_BAND_IND_PCS_1900)
                  {
                    arfcn.band = (sys_band_T) rr_get_band_using_band_indicator(
                      arfcn.num,
                      SI_PSI_BAND_IND_PCS1900,
                      gas_id
                    );
                  }
                  else
                  {
                    arfcn.band = (sys_band_T) rr_get_band_using_band_indicator(
                      arfcn.num,
                      SI_PSI_BAND_IND_DCS1800,
                      gas_id
                    );
                  }
                  bsic.PLMN_colour_code = req->cell_id.ncc;
                  bsic.BS_colour_code = req->cell_id.bcc;

                  if (req->network_control_order_present)
                  {
                    MSG_GERAN_HIGH_3_G(
                      "Starting L2G CCO procedure (T304: %d ms, NCO: %d, CSFB: %d)",
                      req->t304,
                      req->network_control_order,
                      req->is_cco_for_csfb
                    );
                  }
                  else
                  {
                    MSG_GERAN_HIGH_2_G(
                      "Starting L2G CCO procedure (T304: %d ms, NCO: Not Present, CSFB: %d)",
                      req->t304,
                      req->is_cco_for_csfb
                    );
                  }

                  if (req->t304 < RR_T304_MINIMUM_DURATION)
                  {
                    MSG_GERAN_ERROR_2_G(
                      "T304 %dms too low, increasing to %dms",
                      req->t304,
                      RR_T304_MINIMUM_DURATION
                    );

                    (void) rr_timer_start(T3174, RR_T304_MINIMUM_DURATION, gas_id);
                  }
                  else
                  {
                    (void) rr_timer_start(T3174, req->t304, gas_id);
                  }

                  if (req->no_of_si_messages > 0)
                  {
                    uint32 i;

                    for (i = 0;
                         i < req->no_of_si_messages &&
                         i < GERAN_GRR_LTE_MAX_SI_MESSAGES &&
                         i < RR_X2G_SYSTEM_INFORMATION_MESSAGES_MAX;
                         ++i)
                    {
                      system_information.messages[i] = req->si_messages[i];
                    }
                    system_information.count = i;

                    /* Tunnelled system information messages received from LTE
                    RRC do not include the first two octets of each message.
                    These must be added here to allow RR to process the
                    messages normally. */
                    rr_x2g_system_information_prepend_headers(
                      &system_information,
                      gas_id
                    );
                  }

                  rr_x2g_resel_init(gas_id);

                  if (sys_plmn_id_is_undefined(req->requested_PLMN))
                  {
                    plmn_id = NULL;
                  }
                  else
                  {
                    plmn_id = &req->requested_PLMN;
                  }

                  rr_x2g_resel_send_imsg_resel_req(
                    arfcn,
                    63,
                    bsic,
                    MM_AS_LTOG_CCO,
                    rr_is_any_cell_reselection_mode(gas_id),
                    plmn_id,
                    &system_information,
                    gas_id
                  );

                  rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER;
                  rr_control_data_ptr->is_cco_for_csfb = req->is_cco_for_csfb;
                  if (req->network_control_order_present)
                  {
                    rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = TRUE;
                    rr_control_data_ptr->x2g_cco_pending_network_control_order = req->network_control_order;
                  }
                  else
                  {
                    rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
                  }

                  rr_control_data_ptr->rr_state = RR_X2G_RESEL;
                  break;
                } /* GERAN_GRR_LTE_CCO_REQ */

                case GERAN_GRR_LTE_REDIR_REQ:
                {
                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                  new_rr_event = rr_w2g_service_redirection_control(
                                   EV_INPUT_MESSAGE,
                                   new_message_ptr,
                                   gas_id
                                   );
                  rr_control_data_ptr->rr_state = RR_X2G_REDIR;
                  break;
                } /* GERAN_GRR_LTE_REDIR_REQ */

                case GERAN_GRR_LTE_PLMN_SRCH_REQ:
                {
                  /* Save the message in case there is a panic reset before the procedure completes */
                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                  rr_start_ltog_plmn_search(
                    &new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.plmn_srch_req,
                    gas_id
                  );
                  new_rr_event = rr_plmn_list_control(EV_PLMN_SEARCH_START_LTOG, NULL, gas_id);
                  rr_control_data_ptr->rr_state = RR_PLMN_LIST_SEARCH;
                  break;
                } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */

                case GERAN_GRR_LTE_GET_PLMN_PRTL_RESULTS_REQ:
                {
                  lte_irat_plmn_srch_rsp_s *plmn_srch_rsp_ptr;
                  errno_enum_type           result;

                  plmn_srch_rsp_ptr = (lte_irat_plmn_srch_rsp_s *)GPRS_MEM_CALLOC(sizeof(lte_irat_plmn_srch_rsp_s));
                  RR_NULL_CHECK_FATAL(plmn_srch_rsp_ptr);

                  msgr_init_hdr(
                    &plmn_srch_rsp_ptr->msg_hdr,
                    MSGR_GERAN_GRR,
                    GERAN_GRR_LTE_PLMN_SRCH_RSP
                  );

                  plmn_srch_rsp_ptr->srch_done = FALSE;
                  plmn_srch_rsp_ptr->hplmn_found = FALSE;
                  plmn_srch_rsp_ptr->partial_srch = TRUE;

                  result = rr_msgr_send(&plmn_srch_rsp_ptr->msg_hdr, sizeof(lte_irat_plmn_srch_rsp_s), gas_id);

                  if (result != E_SUCCESS)
                  {
                    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                  }

                  GPRS_MEM_FREE(plmn_srch_rsp_ptr);
                  break;
                } /* GERAN_GRR_LTE_GET_PLMN_PRTL_RESULTS_REQ */

#ifdef FEATURE_LTE_TO_GSM_CGI

                // TODO DSDA
                case GERAN_GRR_LTE_GET_CGI_REQ:
                {
                  geran_grr_lte_get_cgi_req_t * req = &new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_get_cgi_req;

                  if (req->num_arfcns == 0)
                  {
                    geran_grr_lte_get_cgi_rsp_t lte_get_cgi_rsp;
                    errno_enum_type             result;

                    MSG_GERAN_ERROR_0_G("GERAN_GRR_LTE_GET_CGI_REQ: ARFCN list is empty");

                    memset(&lte_get_cgi_rsp, 0, sizeof(lte_get_cgi_rsp));

                    msgr_init_hdr(
                      &lte_get_cgi_rsp.msg_hdr,
                      MSGR_GERAN_GRR,
                      GERAN_GRR_LTE_GET_CGI_RSP
                    );

                    result = rr_msgr_send(&lte_get_cgi_rsp.msg_hdr, sizeof(lte_get_cgi_rsp), gas_id);
                    if (result != E_SUCCESS)
                    {
                      MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                    }
                  }
                  else
                  {
                    rr_x2g_cgi_arfcn_list_t arfcn_list;
                    BSIC_T                  bsic;
                    uint32                  i;
                    si_psi_band_ind_T       band_ind;
                    rr_x2g_cgi_info_t       info;

                    /* Save the message in case there is a panic reset before the procedure completes */
                    rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                    memset(&arfcn_list, 0, sizeof(arfcn_list));
                    memset(&info, 0, sizeof(info));

                    if (req->band_ind == GERAN_GRR_BAND_IND_PCS_1900)
                    {
                      band_ind = SI_PSI_BAND_IND_PCS1900;
                    }
                    else
                    {
                      band_ind = SI_PSI_BAND_IND_DCS1800;
                    }

                    for (i = 0;
                         i < req->num_arfcns &&
                         i < GERAN_GRR_LTE_CGI_MAX_NUM_ARFCN &&
                         i < RR_X2G_CGI_ARFCN_LIST_SIZE;
                         ++i)
                    {
                      arfcn_list.entries[i].num = req->arfcns[i];
                      arfcn_list.entries[i].band = (sys_band_T) rr_get_band_using_band_indicator(
                        arfcn_list.entries[i].num,
                        band_ind,
                        gas_id
                      );
                    }
                    arfcn_list.count = i;

                    bsic.PLMN_colour_code = req->cell_id.ncc;
                    bsic.BS_colour_code = req->cell_id.bcc;

                    if (req->last_cgi_rsp_info.cgi_rsp_info_bitmask & GERAN_GRR_LTE_CGI_RSP_RSSI)
                    {
                      info.mask |= RR_X2G_CGI_INFO_MASK_RXLEV;
                      info.rxlev = req->last_cgi_rsp_info.rssi;
                    }

                    if (req->last_cgi_rsp_info.cgi_rsp_info_bitmask & GERAN_GRR_LTE_CGI_RSP_CELL_ID)
                    {
                      info.mask |= RR_X2G_CGI_INFO_MASK_CELL_ID;
                      info.cell_id = (uint16) req->last_cgi_rsp_info.cgi_info.cell_id;
                    }

                    if ((req->last_cgi_rsp_info.cgi_rsp_info_bitmask & GERAN_GRR_LTE_CGI_RSP_PLMN_ID) &&
                        (req->last_cgi_rsp_info.cgi_rsp_info_bitmask & GERAN_GRR_LTE_CGI_RSP_LAC))
                    {
                      info.mask |= RR_X2G_CGI_INFO_MASK_LAI;
                      info.lai.plmn_id = req->last_cgi_rsp_info.cgi_info.plmn_id;

                      info.lai.location_area_code = ((req->last_cgi_rsp_info.cgi_info.lac[0] & 0xff) << 8) |
                                                     (req->last_cgi_rsp_info.cgi_info.lac[1] & 0xff);

                    }

                    if (req->last_cgi_rsp_info.cgi_rsp_info_bitmask & GERAN_GRR_LTE_CGI_RSP_RAC)
                    {
                      info.mask |= RR_X2G_CGI_INFO_MASK_RAC;
                      info.rac_present = TRUE;
                      info.rac = req->last_cgi_rsp_info.cgi_info.rac;
                    }

                    rr_x2g_cgi_init(gas_id);
                    rr_x2g_cgi_send_imsg_cgi_req(
                      &arfcn_list,
                      bsic,
                      req->time_available_for_cgi,
                      &info,
                      gas_id
                    );
                    rr_control_data_ptr->rr_state = RR_X2G_CGI;
                  }
                  break;
                } /* GERAN_GRR_LTE_GET_CGI_REQ */

                /**
                 * GERAN_GRR_LTE_ABORT_CGI_REQ would not normally be expected outside of the CGI procedure,
                 * but can be received due to a race when GERAN_GRR_LTE_GET_CGI_RSP and
                 * GERAN_GRR_LTE_ABORT_CGI_REQ cross.
                 */
                case GERAN_GRR_LTE_ABORT_CGI_REQ:
                {
                  geran_grr_lte_abort_cgi_rsp_t lte_abort_cgi_rsp;
                  errno_enum_type             result;

                  memset(&lte_abort_cgi_rsp, 0, sizeof(lte_abort_cgi_rsp));

                  msgr_init_hdr(
                    &lte_abort_cgi_rsp.msg_hdr,
                    MSGR_GERAN_GRR,
                    GERAN_GRR_LTE_ABORT_CGI_RSP
                  );

                  result = rr_msgr_send(&lte_abort_cgi_rsp.msg_hdr, sizeof(lte_abort_cgi_rsp), gas_id);
                  if (result != E_SUCCESS)
                  {
                    MSG_GERAN_ERROR_1_G("msgr_send failed: %d", result);
                  }
                  break;
                } /* GERAN_GRR_LTE_ABORT_CGI_REQ */

#endif /* FEATURE_LTE_TO_GSM_CGI */

                case GERAN_GRR_CLEAR_DEDICATED_PRIORITIES_IND:
                {
                  rr_individual_priorities_clear(gas_id);
                  break;
                } /* GERAN_GRR_CLEAR_DEDICATED_PRIORITIES_IND */

                case GERAN_GRR_LTE_DEDICATED_PRIORITIES_REQ:
                {
                  geran_grr_lte_dedicated_priorities_rsp_t *lte_dedicated_priorities_ptr;
                  errno_enum_type                          result;

                  lte_dedicated_priorities_ptr = (geran_grr_lte_dedicated_priorities_rsp_t *)GPRS_MEM_CALLOC(sizeof(geran_grr_lte_dedicated_priorities_rsp_t));
                  RR_NULL_CHECK_FATAL(lte_dedicated_priorities_ptr);

                  msgr_init_hdr(
                    &(lte_dedicated_priorities_ptr->msg_hdr),
                    MSGR_GERAN_GRR,
                    GERAN_GRR_LTE_DEDICATED_PRIORITIES_RSP
                  );

                  if (rr_individual_priorities_valid(gas_id))
                  {
                    lte_dedicated_priorities_ptr->dedicated_pri_valid = TRUE;
                    lte_dedicated_priorities_ptr->plmn = rr_dedicated_priority_info_get_plmn(gas_id);
                    rr_dedicated_priority_info_get(
                      &(lte_dedicated_priorities_ptr->dedicated_pri_info), gas_id
                    );
#ifdef FEATURE_LTE_REL11
                    rr_g2w_update_lte_dedicated_pri_info_with_depri_list(
                            &(lte_dedicated_priorities_ptr->dedicated_pri_info.priority_list),
                            gas_id );
#endif /*FEATURE_LTE_REL11*/
                  }
                  else
                  {
                    lte_dedicated_priorities_ptr->dedicated_pri_valid = FALSE;
                  }

                  result = rr_msgr_send(
                    &(lte_dedicated_priorities_ptr->msg_hdr),
                    sizeof(geran_grr_lte_dedicated_priorities_rsp_t),
                    gas_id
                  );

                  if (result != E_SUCCESS)
                  {
                    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                  }

                  GPRS_MEM_FREE(lte_dedicated_priorities_ptr);
                  break;
                } /* GERAN_GRR_LTE_DEDICATED_PRIORITIES_REQ */

                case GERAN_GRR_LTE_ABORT_REDIR_REQ:
                {
                  msgr_hdr_struct_type lte_abort_redir_rsp;
                  errno_enum_type      result;

                  memset(&lte_abort_redir_rsp, 0, sizeof(lte_abort_redir_rsp));

                  msgr_init_hdr(
                    &lte_abort_redir_rsp,
                    MSGR_GERAN_GRR,
                    GERAN_GRR_LTE_ABORT_REDIR_RSP
                  );

                  result = rr_msgr_send(&lte_abort_redir_rsp, sizeof(lte_abort_redir_rsp), gas_id);
                  if (result != E_SUCCESS)
                  {
                    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                  }
                  break;
                } /* GERAN_GRR_LTE_ABORT_REDIR_REQ */

                case GERAN_GRR_LTE_ABORT_RESEL_REQ:
                {
                  msgr_hdr_struct_type lte_abort_resel_rsp;
                  errno_enum_type      result;

                  memset(&lte_abort_resel_rsp, 0, sizeof(lte_abort_resel_rsp));

                  msgr_init_hdr(
                    &lte_abort_resel_rsp,
                    MSGR_GERAN_GRR,
                    GERAN_GRR_LTE_ABORT_RESEL_RSP
                  );
                  result = rr_msgr_send(&lte_abort_resel_rsp, sizeof(lte_abort_resel_rsp), gas_id);
                  if (result != E_SUCCESS)
                  {
                    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                  }
                  break;
                } /* GERAN_GRR_LTE_ABORT_RESEL_REQ */

                case GERAN_GRR_LTE_ABORT_CCO_REQ:
                {
                  msgr_hdr_struct_type lte_abort_cco_rsp;
                  errno_enum_type      result;

                  memset(&lte_abort_cco_rsp, 0, sizeof(lte_abort_cco_rsp));

                  msgr_init_hdr(
                    &lte_abort_cco_rsp,
                    MSGR_GERAN_GRR,
                    GERAN_GRR_LTE_ABORT_CCO_RSP
                  );
                  result = rr_msgr_send(&lte_abort_cco_rsp, sizeof(lte_abort_cco_rsp), gas_id);
                  if (result != E_SUCCESS)
                  {
                    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                  }
                  break;
                } /* GERAN_GRR_LTE_ABORT_CCO_REQ */

                case GERAN_GRR_LTE_HO_REQ:
                {
                  /* MUST change state before calling the state machine, as
                  rr_get_band() will result in the RR state being checked to
                  determine which band indicator to use. */
                  rr_control_data_ptr->rr_state = RR_W2G_INTERRAT_HANDOVER_PROGRESS;

                  rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                  new_rr_event = rr_inter_cell_handover_control(
                    EV_INPUT_MESSAGE,
                    new_message_ptr,
                    gas_id
                  );
                  break;
                } /* GERAN_GRR_LTE_HO_REQ */
#endif /* FEATURE_LTE */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_MSGR_RR */


            default:
            {
              LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                     rr_main_state_name(rr_control_data_ptr->rr_state));
            }
          }
        } /* EV_INPUT_MESSAGE */

        else if (rr_event == EV_PANIC_RESET)
        {
          /* Assume RLC triggered the panic reset - just reset MAC & RLC */
          (void)rr_send_grr_mac_reset_req(gas_id);

          /* No other failure processing is required */
          rr_event = EV_NO_EVENT;
        }


        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(rr_event),
                               rr_main_state_name(rr_control_data_ptr->rr_state));
        }
      } /* RR_INACTIVE state */
      break;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
      /***********************************************************************/
      /* RR_W2G_INTERRAT_HANDOVER_PROGRESS                                   */
      /*                                                                     */
      /* W->G or T->G handover in progress                                   */
      /* (triggered by a RR_INTERRAT_HANDOVER_REQ from RRC,                  */
      /*  TDSRR_INTERRAT_HANDOVER_REQ from TDS-RRC                           */
      /***********************************************************************/
      case RR_W2G_INTERRAT_HANDOVER_PROGRESS:
      {
        // A new message has arrived
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (new_message_ptr->rr.header.rr_message_set)
            {
              case RR_IMSG:
              {
                switch (new_message_ptr->rr.header.imh.message_id)
                {
                  case RR_IMSG_MSC_CHANGE_IND:
                  {
                    /*
                    Update NAS as required
                    RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                    This will be sent when the current procedure completes or G is next active
                    */
                    rr_msc_update(
                      FALSE,   // can_send_service_ind
                      gas_id   // gas_id
                    );
                    break;
                  }

                  case RR_IMSG_PS_ACCESS_REQ:
                  {
                    // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                    rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  case RR_IMSG_NO_PS_ACCESS_REQ:
                  {
                    // No PS access request (could be TLLI unassignment, CS-only)

                    rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  default:
                  {
                    ;  // other RR-IMSG messages can be ignored
                  }
                }
                break;
              } /* RR_IMSG */

              default:
              {
                ;  // other RR-RR messages can be ignored
              }
            }
            break;
          }

          else
          if (message_set == MS_RR_L2)
          {
            // All the following messages need to be forwarded to
            // WCDMA to GSM Handover SM
            if ((message_id == (byte) DL_SUSPEND_CNF)        ||
                 (message_id == (byte) DL_RESUME_CNF)        ||
                 (message_id == (byte) DL_RELEASE_IND)       ||
                 (message_id == (byte) DL_RANDOM_ACCESS_CNF)
               )
            {
              new_rr_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }
            else if (message_id == (byte) MDL_ERROR_IND)
            {
              mdl_error_ind_T *mdl_error_ptr = (mdl_error_ind_T *)new_message_ptr;

              /* for specific failure causes, treat as DL_RELEASE_IND instead */
              if (mdl_error_ptr->error_code == UNSOLICITED_DM_RESPONSE_PAR)
              {
                dl_release_ind_T release_ind;

                release_ind.message_header.message_set = (byte)MS_RR_L2;
                release_ind.message_header.message_id  = (byte)DL_RELEASE_IND;

                release_ind.SAPI            = mdl_error_ptr->SAPI;
                release_ind.l2_channel_type = mdl_error_ptr->l2_channel_type;
                release_ind.release_mode    = LOCAL_END_RELEASE;
                release_ind.gas_id          = gas_id;

                new_rr_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, (rr_cmd_bdy_type *)&release_ind,gas_id);
              }
              else
              {
                MSG_GERAN_ERROR_1_G("Ignored MDL_ERROR_IND with cause %d", (int)(mdl_error_ptr->error_code));
              }
            }

            else if (message_id == (byte) DL_UNIT_DATA_IND)
            {
              if (rr_prepare_ota_msg_for_parsing(&(new_message_ptr->dl_unit_data_ind), gas_id))
              {
                byte message_type = new_message_ptr->dl_unit_data_ind.layer3_message[1];

                // Only PHYSICAL INFORMATION is handled while W2G handover in progress
                if (message_type == PHYSICAL_INFORMATION)
                {
                  new_rr_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                }
                else
                {
                  MSG_GERAN_ERROR_0_G("DL_UNIT_DATA_IND ignored in RR_W2G_INTERRAT_HANDOVER_PROGRESS");
                }
              }
              else
              {
                MSG_GERAN_ERROR_0_G("DL_UNIT_DATA_IND protocol error");
              }
            } /* DL_UNIT_DATA_IND */

            else if (message_id == (byte) DL_DATA_IND)
            {
              RR_cause_T rr_cause;

              if (rr_prepare_ota_msg_for_parsing_in_dedicated(
                    &new_message_ptr->dl_data_ind,
                    &rr_cause,
                    gas_id
                  )
                 )
              {
                byte message_type = new_message_ptr->dl_data_ind.layer3_message[1];

                // Only CLASSMARK ENQUIRY is handled while W2G handover in progress
                if (message_type == CLASSMARK_ENQUIRY)
                {
                  rr_perform_classmark_sending(RR_CLASSMARK_CLIENT_NETW, gas_id);
                }
                else
                {
                  MSG_GERAN_ERROR_0_G("DL_DATA_IND ignored in RR_W2G_INTERRAT_HANDOVER_PROGRESS");

                  MSG_GERAN_HIGH_0_G("OMsg(NW,Status(NotCompatibleWithProtState)");
                  rr_send_status(RR_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE, gas_id);
                }
              }
              else
              {
                /* Send a RR STATUS message indicating the protocol error, if applicable */
                if (rr_cause == RR_MESSAGE_TYPE_NON_EXISTENT)
                {
                  MSG_GERAN_HIGH_0_G("OMsg(NW,Status(MessageTypeNonExistent)");
                  rr_send_status(RR_MESSAGE_TYPE_NON_EXISTENT, gas_id);
                }
                else
                if (rr_cause == RR_INVALID_MANDATORY_INFORMATION)
                {
                  MSG_GERAN_HIGH_0_G("OMsg(NW,Status(InvalidMandatoryInfo)");
                  rr_send_status(RR_INVALID_MANDATORY_INFORMATION, gas_id);
                }
              }
            } /* DL_DATA_IND */

            else
            {
              MSG_GERAN_ERROR_1_G(
                "Unexpected message %d from L2 in INTERRAT_HO_PROGRESS state",
                (int) message_id
              );
            }

          } /* Message_set == MS_RR_L2 */
          else if (message_set == MS_RR_L1)
          {
            if ((message_id == (byte) MPH_HANDOVER_IND) ||
                (message_id == (byte) MPH_HANDOVER_CNF) ||
                (message_id == (byte) MPH_STOP_HANDOVER_ACCESS_CNF))
            {
              new_rr_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }
            else if ((message_id == (byte) MPH_SERVING_DED_MEAS_IND) ||
                     (message_id == (byte) MPH_SURROUND_MEAS_IND)    ||
                     (message_id == (byte) MPH_BLOCK_QUALITY_IND))
            {
              /* no need to process these until the InterRAT Handover is complete */
            }
            else
            {
              MSG_GERAN_ERROR_1_G(
                "Unexpected message %d from L1 in INTERRAT_HO_PROGRESS state",
                (int) message_id
              );
            }
          }

          else if ( message_set == MS_MM_RR )
          {
            if (message_id == (byte) RR_PLMN_SELECT_REQ)
            {
              // We cannot process this in this state
              // send back the Failure to MM.
              rr_send_plmn_select_cnf_no_service(MM_AS_INTERRAT_HANDOVER, gas_id);
            }
            else if (message_id == (byte) RR_PLMN_LIST_REQ)
            {
              MSG_GERAN_HIGH_0_G("Reject RR_PLMN_LIST_REQ during W2G handover");
              rr_reject_msg(new_message_ptr, gas_id);
            }
            else if ( message_id == (byte) RR_ACTIVATION_RSP )
            {
              // Forward this message to WCDMA to GSM Handover SM
              new_rr_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }
            else if ( message_id == (byte) RR_STOP_GSM_MODE_REQ )
            {
              // If a Stop Mode req. arrives in this state
              // send CNF to MM that RR is connected.
              rr_send_stop_gsm_mode_cnf(MM_AS_CONNECTED, gas_id);
            }
            else if ( message_id == (byte) RR_DATA_REQ )
            {
              // in this state all NAS messages need to routed to WCDMA to GSM Handover SM
              new_rr_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }
            /******************************************************/
            /* MM is requesting the abortion of the RR connection */
            /******************************************************/
            else if (message_id == (byte) RR_ABORT_REQ)
            {
              // Forward this message to WCDMA to GSM Handover SM
              new_rr_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }
#ifdef FEATURE_SGLTE
            else if (message_id == (byte) RR_EST_REQ)
            {
              if((rr_is_sglte_mode_active(gas_id)) && (rr_sglte_saved_l2g_ho_being_processed(gas_id)))
              {
                MSG_GERAN_HIGH_0_G("Reject RR_EST_REQ during W2G HO in SGLTE mode");
                rr_sglte_srvcc_send_abort_ind(gas_id);
              }
            }
            else if (message_id == (byte) RR_DEACT_REQ)
            {
              (void)rr_save_msg(new_message_ptr,gas_id);
              MSG_GERAN_HIGH_0_G("Save RR_DEACT_REQ during W2G HO in SGLTE mode");
              new_rr_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message_ptr,gas_id);
            }
#endif /*FEATURE_SGLTE*/
            else if (message_id == (byte) RR_GMM_GPRS_STATE_CHANGE_REQ)
            {
              ;  // no action required
            }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q            
            else if (message_id == (byte) RR_PSEUDO_LTE_LIST_UPDATE_IND)
            {
              ;  // no action required
            }            
#endif            
            else
            {
              MSG_GERAN_ERROR_1_G(
                "Unexpected message %d from MM in INTERRAT_HO_PROGRESS State",
                (int) message_id
              );
            }
          }
#ifdef FEATURE_WCDMA
          // To handle the RRC messages received in this state
          else if (message_set == MS_RRC_RR)
          {
            if ((message_id == (byte) RR_NAS_DATA_IND) || (message_id == (byte) RR_INTERRAT_HANDOVER_ABORT_REQ))
            {
              // Forward this message to WCDMA to GSM Handover SM
              new_rr_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }
            else
            {
              MSG_GERAN_ERROR_1_G(
                "Unexpected message %d from RRC in INTERRAT_HO_PROGRESS State",
                (int) message_id
              );
            }
          }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_GSM_TDS
          else if (message_set == MS_TDSRRC_RR)
          {
            if ((message_id == (byte) TDSRR_NAS_DATA_IND) || (message_id == (byte) TDSRR_INTERRAT_HANDOVER_ABORT_REQ))
            {
              new_rr_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }
            else
            {
              MSG_GERAN_ERROR_1_G(
                "Unexpected message %d from TDSRRC in INTERRAT_HO_PROGRESS State",
                (int) message_id
              );
            }
          }
#endif /*FEATURE_GSM_TDS*/
          else if (message_set == MS_MSGR_RR)
          {
            new_rr_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
          }
          else if ((message_set == MS_GHDI_EXPORT) &&
                   (message_id == (byte) GHDI_CLASSMARK_CHANGED_IND))
          {
            // Forward this message to WCDMA to GSM Handover SM
            new_rr_event = rr_inter_cell_handover_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
          }
          else if ((message_set == MS_TIMER) && (message_id == (byte) MID_TIMER_EXPIRY))
          {
            switch (new_message_ptr->mid_timer_expiry.timer_id)
            {
              case T3124:
              {
                // Forward this event to WCDMA to GSM Handover SM
                new_rr_event = rr_inter_cell_handover_control(EV_T3124_TIMEOUT, NULL, gas_id);
                break;
              }

              case T3122:
              {
                // Unexpected T3122 expiry during W2G HO.
                // T3122 is stopped when G-RAT is stopped
                MSG_GERAN_ERROR_0_G("Unexpected T3122 expiry during W2G HO");
                break;
              }

              default:
              {
                LOG_UNEXPECTED_TIMER_EXPIRY(
                  new_message_ptr->mid_timer_expiry.timer_id,
                  rr_main_state_name(rr_control_data_ptr->rr_state)
                );
              }
            }
          }
          else
          {
            MSG_GERAN_ERROR_2_G(
              "Unexpected message=(%d,%d) in INTERRAT_HO_PROGRESS State",
              (int) message_set,
              (int) message_id
            );
          }
        } /* EV_INPUT_MESSAGE */
        else if (rr_event == EV_GSM_MODE_ACTIVATED)
        {
          new_rr_event = rr_inter_cell_handover_control(rr_event, NULL, gas_id);
        }
        else if (rr_event == EV_GSM_MODE_DEACTIVATED)
        {
          new_rr_event = rr_inter_cell_handover_control(rr_event, NULL, gas_id);
        }
        else if (rr_event == RR_EV_W2G_HO_COMPLETED)
        {
#ifdef FEATURE_LTE
          boolean g2l_blind_redir_after_csfb_required = 
            rr_connection_release_get_g2l_blind_redir_after_csfb_flag(gas_id);
          boolean g2l_blind_redir_required = 
            rr_connection_release_get_g2l_blind_redir_flag(gas_id);
#endif /* FEATURE_LTE */
          gprs_scell_info_t         * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
          surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);
#ifdef FEATURE_GSM_TDS
          rr_ded_irat_measurements_t   *tds_ded_meas_ptr = rr_get_tds_ded_meas_ptr(gas_id);
#endif /*FEATURE_GSM_TDS*/

          rr_control_data_ptr->rr_state = RR_DATA_TRANSFER;
          scell_info_ptr->gsm.camped_on_flag = TRUE;

#ifdef FEATURE_GSM_TDS
          // TODO DSDA
          rr_initialize_tds_ded_measurements(tds_ded_meas_ptr);
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_SGLTE
         rr_sglte_delete_store_serving_cell_info(gas_id);
         rr_sglte_reinit_processing_l2g_ho_related_parameter(gas_id);
#endif /*FEATURE_SGLTE*/

          /* Reinitialise the WCDMA measurements store.  This ensures that we do not
          include measurements taken in a previous call during this call. */
          rr_initialize_wcdma_ded_measurements(wcdma_ded_meas_ptr);

          /* Inform connection modules that RR connection is present now */
          (void)rr_connection_establishment_control(EV_CONNECTION_EXISTS, NULL, gas_id);
          (void)rr_connection_release(EV_CONNECTION_EXISTS, NULL,gas_id);

#ifdef FEATURE_LTE
          /* During the handover, it is possible for G2L blind redirection
          to have been enabled.  Informing connection release of
          EV_CONNECTION_RELEASE will clear the flag, so we reset it after
          informing rr_connection_relase here. */
          if (g2l_blind_redir_after_csfb_required)
          {
            rr_connection_release_set_g2l_blind_redir_after_csfb_flag(gas_id);
          }

          if (g2l_blind_redir_required)
          {
            rr_connection_release_set_g2l_blind_redir_flag(gas_id);
          }
#endif /* FEATURE_LTE */
        }
        else if (rr_event == RR_EV_W2G_GO_TO_INACTIVE)
        {
          rr_control_data_ptr->rr_state = RR_INACTIVE;
          new_rr_event = rr_inter_cell_handover_control(EV_RESET_SOFTWARE, NULL, gas_id);

#ifdef FEATURE_SGLTE

          new_rr_event = rr_process_saved_message( &new_message_ptr, gas_id );

          if((rr_is_sglte_mode_active(gas_id)) && (rr_sglte_saved_l2g_ho_being_processed(gas_id)) && (new_rr_event != EV_INPUT_MESSAGE) )
          {
             rr_sglte_srvcc_send_no_service_ind(MM_AS_NO_SERVICE,gas_id);
          }
          rr_sglte_reinit_processing_l2g_ho_related_parameter(gas_id);
#endif /*FEATURE_SGLTE*/

          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);
        }
        else if (rr_event == RR_EV_W2G_HO_ABORTED)
        {
          // Move to DATA_TRANSFER state.
          rr_control_data_ptr->rr_state = RR_DATA_TRANSFER;
#ifdef FEATURE_SGLTE
          rr_sglte_reinit_processing_l2g_ho_related_parameter(gas_id);
#endif /*FEATURE_SGLTE*/
          // also Inform Connection Release module that RR connection is present now.
          (void) rr_connection_release(EV_CONNECTION_EXISTS, NULL, gas_id);
          rr_set_link_status(SAPI0_LINK, gas_id);
          // create RR_ABORT_REQ, so that it will be processed in DATA_TRANSFER State.
          memset(&rr_rr_abort_req, 0, sizeof(rr_rr_abort_req));
          rr_rr_abort_req.message_header.message_set = MS_MM_RR;
          rr_rr_abort_req.message_header.message_id  = (byte) RR_ABORT_REQ;
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
          rr_rr_abort_req.as_id = RR_GAS_ID_TO_AS_ID;
#endif // FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM
          new_message_ptr = (rr_cmd_bdy_type *) &rr_rr_abort_req;
          new_rr_event = EV_INPUT_MESSAGE;
        }
        else if (rr_event == EV_PANIC_RESET)
        {
          /* Panic Reset can not be handled immediately, as RR may be in the
          process of activating NAS - if this is the case, the  */
#ifdef FEATURE_SGLTE
          rr_sglte_reinit_processing_l2g_ho_related_parameter(gas_id);
#endif /*FEATURE_SGLTE*/
          rr_event = EV_NO_EVENT;
          new_rr_event = rr_inter_cell_handover_control(EV_PANIC_RESET_REQ, NULL, gas_id);
        }
        else if (rr_event == EV_PANIC_RESET_CNF)
        {
          if (rr_verify_panic_reset_saved_message_set(MS_RRC_RR, gas_id)
#ifdef FEATURE_GSM_TDS
              // TODO DSDA
              || rr_verify_panic_reset_saved_message_set(MS_TDSRRC_RR, gas_id)
#endif /*FEATURE_GSM_TDS*/
              )
          {
            /* Change the rr_event back to EV_PANIC_RESET.  Note - this *must*
            be rr_event, not new_rr_event as rr_event is handled explicitly
            at the bottom of this state machine, before the next loop commences. */
            rr_event = EV_PANIC_RESET;
          }
          else
          {
            /* No saved message indicates that we have sent RR_ACTIVATION_IND
            to NAS and received RR_ACTIVATION_RSP - NAS has switch to GERAN
            being active, so we should drop the call and perform cell selection
            to recover service. */
            gprs_scell_info_t         * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
            surr_wcdma_ded_measurements_T *wcdma_ded_meas_ptr = rr_get_wcdma_ded_meas_ptr(gas_id);

            rr_control_data_ptr->rr_state = RR_DATA_TRANSFER;
            scell_info_ptr->gsm.camped_on_flag = TRUE;

            /* Reinitialise the WCDMA measurements store.  This ensures that we do not
            include measurements taken in a previous call during this call. */
            rr_initialize_wcdma_ded_measurements(wcdma_ded_meas_ptr);

            /* Inform connection modules that RR connection is present now */
            (void)rr_connection_establishment_control(EV_CONNECTION_EXISTS, NULL, gas_id);
            (void)rr_connection_release(EV_CONNECTION_EXISTS, NULL, gas_id);

            /* Panic reset is handled in RR_DATA_TRANSFER state as we have effectively
            finished the W2G Handover and are now in dedicated mode. */
            new_rr_event = EV_PANIC_RESET;
          }
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(rr_event),
                               rr_main_state_name(rr_control_data_ptr->rr_state));
        }
      }  /* RR_W2G_INTERRAT_HANDOVER_PROGRESS state */
      break;

#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

      /***********************************************************************/
      /* RR_GOING_ACTIVE                                                     */
      /*                                                                     */
      /* The GSM AS is being activated.                                      */
      /* MPH_START_GSM_MODE_REQ has been sent to L1, which will move it from */
      /* INACTIVE to NULL. RR is waiting for MPH_START_GSM_MODE_CNF.         */
      /***********************************************************************/
      case RR_GOING_ACTIVE:
      {
        // A new message has arrived
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (new_message_ptr->rr.header.rr_message_set)
            {
              case RR_IMSG:
              {
                switch (new_message_ptr->rr.header.imh.message_id)
                {
                  case RR_IMSG_MSC_CHANGE_IND:
                  {
                    /*
                    Update NAS as required
                    RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                    This will be sent after RR/L1 enters Idle
                    */
                    rr_msc_update(
                      FALSE,   // can_send_service_ind
                      gas_id   // gas_id
                    );
                    break;
                  }

                  case RR_IMSG_PS_ACCESS_REQ:
                  {
                    // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                    rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  case RR_IMSG_NO_PS_ACCESS_REQ:
                  {
                    // No PS access request (could be TLLI unassignment, CS-only)

                    rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  default:
                  {
                    ;  // other RR-IMSG messages can be ignored
                  }
                }
                break;
              } /* RR_IMSG */

              default:
              {
                ;  // other RR-RR messages can be ignored
              }
            }
            break;
          }

          else
          if (message_set == MS_MM_RR)
          {
            if ((message_id == (byte) RR_PLMN_SELECT_REQ) ||
                (message_id == (byte) RR_PLMN_LIST_REQ)   ||
                (message_id == (byte) RR_BPLMN_SEARCH_ABORT_REQ) ||
                (message_id == (byte) RR_DEACT_REQ))
            {
              /* Queue the request and handle it after GSM mode is active */
              if (rr_save_msg(new_message_ptr, gas_id))
              {
                /* Save the message in case there is a panic reset before the procedure completes */
                rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);
              }
            }
            else if (message_id == (byte) RR_EST_REQ)
            {
              /* GSM mode not active. Cannot establish RR connection... */
              MSG_GERAN_HIGH_0_G("Cannot establish RR connection");
              rr_send_abort_ind(RR_CONNECTION_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
            }
            else if (message_id == (byte) RR_STOP_GSM_MODE_REQ)
            {
              new_rr_event = rr_process_rr_stop_gsm_mode_req(TRUE, gas_id);
              rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
            }
            /******************************************************/
            /* MM is requesting the abortion of the RR connection */
            /******************************************************/
            else if (message_id == (byte) RR_ABORT_REQ)
            {
              /* send back a CNF message right away since no RR connection exists */
              rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);
            }
            else if (message_id == (byte) RR_GMM_GPRS_STATE_CHANGE_REQ)
            {
              ;  // no action required
            }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q            
            else if (message_id == (byte) RR_PSEUDO_LTE_LIST_UPDATE_IND)
            {
              ;  // no action required
            }  
#endif            
            else
            {
              MSG_GERAN_ERROR_3_G(
                "Unexpected message=(%d,%d), State=%d",
                (int) message_set,
                (int) message_id,
                (int) rr_control_data_ptr->rr_state
              );
            }
          }

          else if (message_set == MS_TIMER)
          {
            switch (new_message_ptr->mid_timer_expiry.timer_id)
            {
              case T3122:
              {
                // no action required - NAS-MM will get notified when the next RR_SERVICE_IND is sent
                MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
                break;
              }

              default:
              {
                LOG_UNEXPECTED_TIMER_EXPIRY(
                  new_message_ptr->mid_timer_expiry.timer_id,
                  rr_main_state_name(rr_control_data_ptr->rr_state)
                );
              }
            }
          } /* (message_set == MS_TIMER) */

          else
          {
            MSG_GERAN_ERROR_3_G(
              "Unexpected message=(%d,%d), State=%d",
              (int) message_set,
              (int) message_id,
              (int) rr_control_data_ptr->rr_state
            );
          }
        } /* EV_INPUT_MESSAGE */
        /* GSM AS has been activated */
        else if (rr_event == EV_GSM_MODE_ACTIVATED)
        {
          MSG_GERAN_HIGH_0_G("RR - GSM Mode Activated");

          if (rr_process_panic_reset_saved_msg(&new_message_ptr, gas_id) == EV_INPUT_MESSAGE)
          {
            rr_control_data_ptr->rr_state = RR_NO_CHANNELS;
            new_rr_event = EV_INPUT_MESSAGE;
          }

          else
          /* In most situations, a message from MM prompted RR to activate. */
          /* But there are cases where RR/L1 have deactivated to re-synchronise */
          /* so a cell selection should be started autonously to recamp. */
          /* Therefore, if there is no MM message pending here, trigger a */
          /* cell selection */
          if (rr_process_saved_message(&new_message_ptr, gas_id) == EV_INPUT_MESSAGE)
          {
            rr_control_data_ptr->rr_state = RR_NO_CHANNELS;
            new_rr_event = EV_INPUT_MESSAGE;
          }
          else
          {
            rr_control_data_ptr->rr_state = RR_CELL_SELECTION;
            new_rr_event = rr_selection_control(EV_TRIGGER_CELL_SELECTION, NULL, gas_id);
          }
        }

        /* GSM AS has been deactivated */
        else if (rr_event == EV_GSM_MODE_DEACTIVATED)
        {
          MSG_GERAN_ERROR_0_G("EV_GSM_MODE_DEACTIVATED in RR_GOING_ACTIVE");
          rr_control_data_ptr->rr_state = RR_INACTIVE;
        }

        /* Panic Reset triggered */
        else if (rr_event == EV_PANIC_RESET)
        {
          ;  /* Panic reset triggered - no specific action required */
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(rr_event),
                               rr_main_state_name(rr_control_data_ptr->rr_state));
        }
      }  /* RR_GOING_ACTIVE state */
      break;


      /***********************************************************************/
      /* RR_GOING_INACTIVE                                                   */
      /*                                                                     */
      /* The GSM AS is being deactivated.                                    */
      /* Awaiting confirmation from L1 (MPH_STOP_GSM_MODE_CNF) that L1 has   */
      /* transitioned to INACTIVE                                            */
      /***********************************************************************/
      case RR_GOING_INACTIVE:
      {
        // A new message has arrived
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (new_message_ptr->rr.header.rr_message_set)
            {
              case RR_IMSG:
              {
                switch (new_message_ptr->rr.header.imh.message_id)
                {
                  case RR_IMSG_MSC_CHANGE_IND:
                  {
                    /*
                    Update NAS as required
                    RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                    This will be sent when G is next active
                    */
                    rr_msc_update(
                      FALSE,   // can_send_service_ind
                      gas_id   // gas_id
                    );
                    break;
                  }

                  case RR_IMSG_PS_ACCESS_REQ:
                  {
                    // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                    rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  case RR_IMSG_NO_PS_ACCESS_REQ:
                  {
                    // No PS access request (could be TLLI unassignment, CS-only)

                    rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  default:
                  {
                    ;  // other RR-IMSG messages can be ignored
                  }
                }
                break;
              } /* RR_IMSG */

              default:
              {
                ;  // other RR-RR messages can be ignored
              }
            }
          }
          else if (message_set == MS_MM_RR)
          {

            if ( (message_id == (byte) RR_PLMN_SELECT_REQ)   ||
                 (message_id == (byte) RR_PLMN_LIST_REQ)     ||
                 (message_id == (byte) RR_STOP_GSM_MODE_REQ) ||
                 (message_id == (byte) RR_DEACT_REQ))
            {
              /* Save the message, and process once L1 has confirmed deactivation. */
              /* Note: In the case of RR_STOP_GSM_MODE_REQ, the only action will be */
              /* a _CNF sent from INACTIVE when this is processed */
              if (rr_save_msg(new_message_ptr, gas_id))
              {
                /* Save the message in case there is a panic reset before the procedure completes */
                rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);
              }
            }
            else if ((mm_rr_message_id_enum_T) message_id == RR_BPLMN_SEARCH_ABORT_REQ)
            {
              if (rr_control_data_ptr->rr_saved_msg.valid)
              {
                if (grr_verify_saved_mm_message(RR_PLMN_LIST_REQ, gas_id))
                {
                  rr_delete_saved_msg(gas_id);
                  /* TODO: Clear saved panic reset message here? */
                }
              }

              new_rr_event = EV_NO_EVENT;
            }
            else if (message_id == (byte) RR_EST_REQ)
            {
              // GSM mode not active. Cannot establish RR connection...
              MSG_GERAN_HIGH_0_G("Cannot establish RR connection");
              rr_send_abort_ind(RR_CONNECTION_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
            }
            /******************************************************/
            /* MM is requesting the abortion of the RR connection */
            /******************************************************/
            else if (message_id == (byte) RR_ABORT_REQ)
            {
              /* send back a CNF message right away since no RR connection exists */
              rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);
            }
            else if (message_id == (byte) RR_GMM_GPRS_STATE_CHANGE_REQ)
            {
              ;  // no action required
            }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q            
            else if (message_id == (byte) RR_PSEUDO_LTE_LIST_UPDATE_IND)
            {
              ;  // no action required
            }
#endif            
            else
            {
              MSG_GERAN_ERROR_3_G(
                "Unexpected message=(%d,%d), State=%d",
                (int) message_set,
                (int) message_id,
                (int) rr_control_data_ptr->rr_state
              );
            }
          }
          else if (message_set == MS_RR_L1)
          {
            switch (message_id)
            {
              case MPH_BLOCK_QUALITY_IND:
              case MPH_SERVING_IDLE_MEAS_IND:
              case MPH_SERVING_AUX_MEAS_IND:
              case MPH_SERVING_MEAS_IND:
              case MPH_NC_MEASUREMENT_IND:
              case MPH_SURROUND_MEAS_IND:
              case MPH_SURROUND_UPDATE_FAILURE_IND:
#ifdef FEATURE_WCDMA
              case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
              case MPH_SURROUND_WCDMA_XFER_MEAS_IND:
#endif
#if defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
              case MPH_SURROUND_IRAT_MEAS_IND:
#endif /* FEATURE_LTE || FEATURE_GSM_TDS */
              case MPH_POWER_SCAN_CNF:
              case MPH_DECODE_BCCH_LIST_FAILURE_IND:
              case MPH_DECODE_BCCH_LIST_CNF:
              case MPH_SELECT_SPECIFIC_BCCH_CNF:
              {
                break;  // ignore these messages (can be sent by L1 before RR_STOP_GSM_MODE_REQ processed)
              }

              default:
              {
                LOG_UNEXPECTED_MESSAGE(new_message_ptr, rr_main_state_name(rr_control_data_ptr->rr_state));
              }
            }
          }
          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_UNIT_DATA_IND))
          {
            /*****************************************************************/
            /* These messages can be received as a result of race conditions */
            /* when RR receives RR_STOP_GSM_MODE_REQ while in Idle           */
            /* they should be ignored...                                     */
            /*****************************************************************/
          }
          else if ((message_set == MS_MAC_RR) && (message_id == (byte)MAC_GRR_MAC_STATUS))
          {
            ;
          }
          else if (message_set == MS_TIMER)
          {
            switch (new_message_ptr->mid_timer_expiry.timer_id)
            {
              case T3122:
              {
                // no action required - NAS-MM will get notified when the next RR_SERVICE_IND is sent
                MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
                break;
              }

              default:
              {
                LOG_UNEXPECTED_TIMER_EXPIRY(
                  new_message_ptr->mid_timer_expiry.timer_id,
                  rr_main_state_name(rr_control_data_ptr->rr_state)
                );
              }
            }
          } /* (message_set == MS_TIMER) */
          else
          {
            LOG_UNEXPECTED_MESSAGE(new_message_ptr, rr_main_state_name(rr_control_data_ptr->rr_state));
          }
        } /* EV_INPUT_MESSAGE */
        else if (rr_event == EV_GSM_MODE_DEACTIVATED)
        {
          /* Inform CB of loss of CBCH, re-initialise RR data and determine
          next action. */
          rr_gsm_as_deactivation_complete_action_e action = rr_gsm_as_deactivation_complete(gas_id);

          rr_l1_idle_init(gas_id);//Clear l1 idle init state
          rr_psi_clear_pmo_data_going_inactive(gas_id);

          switch (action)
          {
            case RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_AUTO_RESTART:
            {
              rr_control_data_ptr->rr_state = RR_GOING_ACTIVE;
              new_rr_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
              break;
            } /* RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_AUTO_RESTART */

            case RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_RECOVER_FROM_PANIC_RESET:
            {
#ifdef FEATURE_WCDMA
              if (rr_verify_panic_reset_saved_message_set(MS_RRC_RR, gas_id))
              {
                /* If a panic reset occurred during a W2G procedure then RR
                needs to send an appropriate failure message back to WCMDA RRC
                before returning to RR_INACTIVE state. */
                MSG_GERAN_HIGH_0_G("Handling saved panic reset message from WCDMA RRC");
                if (rr_process_panic_reset_saved_msg(&new_message_ptr, gas_id) == EV_INPUT_MESSAGE)
                {
                  boolean rr_activation_ind_sent = FALSE;

                  if (rr_verify_panic_reset_saved_msg(MS_RRC_RR, RR_INTERRAT_CC_ORDER_REQ, gas_id))
                  {
                    rr_activation_ind_sent = rr_control_data_ptr->rr_panic_reset_info.cco_activation_ind_sent;
                  }

                  rr_control_data_ptr->rr_panic_reset_info.cco_activation_ind_sent = FALSE;

                  rr_respond_to_wcdma_rrc_request_after_panic_reset(
                    new_message_ptr,
                    rr_activation_ind_sent,
                    gas_id
                  );
                  rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);
                  rr_control_data_ptr->rr_state = RR_INACTIVE;
                  new_rr_event = rr_process_saved_message(&new_message_ptr, gas_id);

                  rr_gprs_reset_software(gas_id);
                }
                else
                {
                  MSG_GERAN_ERROR_0_G("Saved message invalid, attempting further recovery");
                  rr_control_data_ptr->rr_state = RR_GOING_ACTIVE;
                  new_rr_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                }
              }
              else
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
              if (rr_verify_panic_reset_saved_message_set(MS_MSGR_RR, gas_id))
              {
                /* If a panic reset occurred during a L2G procedure then RR
                needs to send an appropriate failure message back to LTE RRC
                before returning to RR_INACTIVE state. */
                MSG_GERAN_HIGH_0_G("Handling saved panic reset message from LTE RRC");
                if (rr_process_panic_reset_saved_msg(&new_message_ptr, gas_id) == EV_INPUT_MESSAGE)
                {
                  rr_control_data_ptr->rr_panic_reset_info.cco_activation_ind_sent = FALSE;

                  rr_respond_to_lte_rrc_request_after_panic_reset(
                    new_message_ptr,
                    gas_id
                  );
                  rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);
                  rr_control_data_ptr->rr_state = RR_INACTIVE;
                  new_rr_event = rr_process_saved_message(&new_message_ptr, gas_id);

                  rr_gprs_reset_software(gas_id);
                }
                else
                {
                  MSG_GERAN_ERROR_0_G("Saved message invalid, attempting further recovery");
                  rr_control_data_ptr->rr_state = RR_GOING_ACTIVE;
                  new_rr_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                }
              }
              else
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_TDS
              // TODO DSDA
              if (rr_verify_panic_reset_saved_message_set(MS_TDSRRC_RR, gas_id))
              {
                /* If a panic reset occurred during a T2G procedure then RR
                needs to send an appropriate failure message back to WCMDA RRC
                before returning to RR_INACTIVE state. */
                MSG_GERAN_HIGH_0_G("Handling saved panic reset message from RRC");
                if (rr_process_panic_reset_saved_msg(&new_message_ptr, gas_id) == EV_INPUT_MESSAGE)
                {
                  boolean rr_activation_ind_sent = FALSE;

                  if (rr_verify_panic_reset_saved_msg(
                        MS_TDSRRC_RR,
                        TDSRR_INTERRAT_CC_ORDER_REQ,
                        gas_id
                      ))
                  {
                    rr_activation_ind_sent = rr_control_data_ptr->rr_panic_reset_info.cco_activation_ind_sent;
                  }

                  rr_control_data_ptr->rr_panic_reset_info.cco_activation_ind_sent = FALSE;

                  rr_respond_to_tdscdma_rrc_request_after_panic_reset(
                    new_message_ptr,
                    rr_activation_ind_sent,
                    gas_id
                  );
                  rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);
                  rr_control_data_ptr->rr_state = RR_INACTIVE;
                  new_rr_event = rr_process_saved_message(&new_message_ptr, gas_id);

                  rr_gprs_reset_software(gas_id);
                }
                else
                {
                  MSG_GERAN_ERROR_0_G("Saved message invalid, attempting further recovery");
                  rr_control_data_ptr->rr_state = RR_GOING_ACTIVE;
                  new_rr_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                }
              }
              else
#endif /* FEATURE_GSM_TDS */

              /* Special case - W2G CCO
              The W2G CCO procedure extends beyond just acquiring service on a
              cell - connection establishment (either CS or PS) must be
              successful before the procedure is terminated.  The
              RR_INTERRAT_CC_ORDER_REQ is removed from the panic reset structure
              on receipt of RR_ACTIVATION_RSP but a flag is then set to
              indicate that the W2G CCO is ongoing.  The reason this is done is
              because RR will automatically and necessarily store any received
              RR_EST_REQ in the panic reset structure, which would result in the
              RR_INTERRAT_CC_ORDER_REQ being overwritten anyway. */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
              if (rr_get_panic_reset_x2g_cco_flag(gas_id) != RR_X2G_RESEL_TYPE_UNDEFINED)
              {
                rr_cmd_bdy_type *x2g_cco_req_ptr;

                MSG_GERAN_HIGH_0_G(
                  "panic reset during W2G CCO - return control to WCDMA AS"
                );

                x2g_cco_req_ptr = GPRS_MEM_MALLOC(sizeof(rr_cmd_bdy_type));

                if (x2g_cco_req_ptr != NULL)
                {
                  switch (rr_get_panic_reset_x2g_cco_flag(gas_id))
                  {
#ifdef FEATURE_WCDMA
                    case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
                    {
                      x2g_cco_req_ptr->message_header.message_set = MS_RRC_RR;
                      x2g_cco_req_ptr->message_header.message_id = RR_INTERRAT_CC_ORDER_REQ;

                      rr_respond_to_wcdma_rrc_request_after_panic_reset(
                        x2g_cco_req_ptr,
                        TRUE,
                        gas_id
                      );
                      break;
                    } /* RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                    // TODO DSDA
                    case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                    {
                      x2g_cco_req_ptr->message_header.message_set = MS_TDSRRC_RR;
                      x2g_cco_req_ptr->message_header.message_id = TDSRR_INTERRAT_CC_ORDER_REQ;

                      rr_respond_to_tdscdma_rrc_request_after_panic_reset(
                        x2g_cco_req_ptr,
                        TRUE,
                        gas_id
                      );
                      break;
                    } /* RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
                    case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                    {
                      x2g_cco_req_ptr->message_header.message_set = MS_MSGR_RR;
                      x2g_cco_req_ptr->rr_msgr_msg.message.hdr.id = GERAN_GRR_LTE_CCO_REQ;

                      rr_respond_to_lte_rrc_request_after_panic_reset(
                        x2g_cco_req_ptr,
                        gas_id
                      );
                      break;
                    } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_LTE */

                    default:
                    {
                      MSG_GERAN_ERROR_1_G(
                        "Unexpected X2G CCO type: %d",
                        rr_get_panic_reset_x2g_cco_flag(gas_id)
                      );
                    }
                  }

                  GPRS_MEM_FREE(x2g_cco_req_ptr);
                }

                rr_control_data_ptr->rr_panic_reset_info.cco_activation_ind_sent = FALSE;
                rr_set_panic_reset_x2g_cco_flag(RR_X2G_RESEL_TYPE_UNDEFINED, gas_id);

                rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);
                rr_control_data_ptr->rr_state = RR_INACTIVE;
                new_rr_event = rr_process_saved_message(&new_message_ptr, gas_id);

                rr_gprs_reset_software(gas_id);
              }
              else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */
              {
                /* Continue with normal recovery - cell selection or process an
                existing saved message. */
                rr_control_data_ptr->rr_state = RR_GOING_ACTIVE;
                new_rr_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
              }
              break;
            } /* RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_RECOVER_FROM_PANIC_RESET */

            case RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_GO_TO_INACTIVE:
            {
              /* Move to INACTIVE, and process and saved MM messages there */
              rr_control_data_ptr->rr_state = RR_INACTIVE;
              new_rr_event = rr_process_saved_message(&new_message_ptr, gas_id);
              break;
            } /* RR_GSM_AS_DEACTIVATION_COMPLETE_ACTION_GO_TO_INACTIVE */
#ifdef FEATURE_SGLTE
            case RR_GSM_AS_SGLTE_INTERNAL_DEACTIVATION_COMPLETE_ACTION_PROCESS_L2G_HO_REQ:
            {
              rr_cmd_bdy_type *message_ptr;
              rr_event_T rr_saved_event;
              rr_control_data_ptr->rr_state = RR_INACTIVE;

              rr_saved_event = rr_examine_saved_message( &message_ptr , gas_id);

              /* Look for the  saved RR_DEACT_REQ rcvd during itrnl shutdown */
              if ( (rr_saved_event == EV_INPUT_MESSAGE) &&
                   (message_ptr != NULL) &&
                   (message_ptr->message_header.message_set == MS_MM_RR) &&
                   (message_ptr->message_header.message_id == (byte) RR_DEACT_REQ)
                  )
              {
                new_rr_event = rr_process_saved_message( &new_message_ptr, gas_id );

                if(rr_saved_l2g_msg_present(gas_id))
                {
                  rr_delete_saved_l2g_msg(gas_id);
                  MSG_GERAN_HIGH_0_G("Sending L2H HO failed due to deact being rcvd");
                  rr_sglte_send_lte_ho_failed_rsp(gas_id);
                }
              }
              else
              {
                new_rr_event = rr_process_saved_l2g_message( &new_message_ptr, gas_id );
                if(EV_INPUT_MESSAGE == new_rr_event)
                {
                  MSG_GERAN_HIGH_0_G("GRR processng now saved L2G HO after internal shutdown");
                  rr_control_data_ptr->sglte_processng_saved_l2g_ho_msg = TRUE;
                }
                else
                {
                  MSG_GERAN_ERROR_0_G("Not able to proces saved message");
                }
              }
              break;
            }

#endif /*FEATURE_SGLTE*/
            default:
            {
              MSG_GERAN_ERROR_1_G("Unexpected action: %d", action);
            }
          }
        }

        else
        if (rr_event == EV_CR_G2W_TIMER_EXPIRED)
        {
          ;  // ignore this event (can be caused by blocked timer expiry)
        }

        else
        /* Panic Reset occurred */
        if (rr_event == EV_PANIC_RESET)
        {
          ;  /* Panic reset triggered - no specific action required */
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(rr_event),
                               rr_main_state_name(rr_control_data_ptr->rr_state));
        }
      }  /* RR_GOING_INACTIVE state */
      break;


      /***********************************************************************/
      /* RR_NO_CHANNELS                                                      */
      /*                                                                     */
      /* RR is not camped on a cell, but is still active. No independant     */
      /* actions are taken by RR to move out of this state - it waits for    */
      /* a message from MM                                                   */
      /***********************************************************************/
      case RR_NO_CHANNELS:
      {
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          if (message_set == MS_MM_RR)
          {
            /****************************************************************/
            /* MM->RR: RR_PLMN_SELECT_REQ                                   */
            /*                                                              */
            /* Start a cell selection                                       */
            /****************************************************************/
            if (message_id == (byte) RR_PLMN_SELECT_REQ)
            {
              /* Save the message so it can be retried in the case of panic reset */
              /* during cell selection */
              rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

              /* This will return EV_CELL_SELECTION_TRIGGERED */
              new_rr_event = rr_selection_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }

            /****************************************************************/
            /* MM->RR: RR_PLMN_LIST_REQ                                     */
            /*                                                              */
            /* Start a PLMN search. This will be a foreground search because*/
            /* RR is not camped. (L1 should be NULL)                        */
            /* Note: It is not possible to perform a background search      */
            /****************************************************************/
            else if (message_id == (byte) RR_PLMN_LIST_REQ)
            {
              // Ensure AS and NAS are in-sync with respect to service status
              if (rr_mm_get_service_state(gas_id) != MM_AS_NO_SERVICE)
              {
                rr_send_no_service_ind(gas_id);
              }

              rr_start_plmn_list_search((rr_plmn_list_req_T *)new_message_ptr, TRUE, gas_id);
              new_rr_event = rr_plmn_list_control(EV_PLMN_SEARCH_START, NULL, gas_id);
              rr_control_data_ptr->rr_state = RR_PLMN_LIST_SEARCH;
            }
            /****************************************************************/
            /* MM->RR: RR_BPLMN_SEARCH_ABORT_REQ                            */
            /* the message was saved in RR_GOING_ACTIVE                     */
            /* RR should go back to INACTIVE and waiting for further        */
            /* PLMN_SELECTION_REQ or PLMN_LIST_REQ                          */
            /****************************************************************/
            else if (message_id == (byte) RR_BPLMN_SEARCH_ABORT_REQ)
            {
              set_rr_deact_reason(RR_DEACT_REASON_STOP_CAMPING, gas_id);
              new_rr_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
              rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
            }

            /****************************************************************/
            /* MM->RR: RR_DEACT_REQ                                         */
            /* this message may have been saved and restored by GRR after a */
            /* panic reset was handled, so shut down Layer 1 and transition */
            /* to RR_GOING_INACTIVE now                                     */
            /****************************************************************/
            else if (message_id == (byte) RR_DEACT_REQ)
            {
              set_rr_deact_reason(RR_DEACT_REASON_DEACT_REQ, gas_id);
              new_rr_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
              rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;

              MSG_GERAN_HIGH_1_G(
                "RR_DEACT_REQ caused event %d in state RR_NO_CHANNELS",
                (int) new_rr_event
              );
            }

            else if (message_id == (byte) RR_STOP_GSM_MODE_REQ)
            {
              if ( rr_x2g_cco_in_progress(gas_id ) )
              {
                /* terminate CCO and then proceed with processing STOP MODE Request */
                (void)rr_timer_cancel(T3174, gas_id);

                switch (rr_control_data_ptr->x2g_resel_type)
                {
#ifdef FEATURE_WCDMA
                  case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
                  {
                    rr_send_interrat_cc_order_cnf(CCO_SUCCESS, RR_GSM_CCO_OTHER_REASON, gas_id);
                    break;
                  }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                  case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                  {
                    rr_send_tds_interrat_cc_order_cnf(
                    TDSCCO_SUCCESS,
                    TDSRR_GSM_CCO_OTHER_REASON,
                    gas_id
                    );

                    rr_control_data_ptr->waiting_for_tdsrr_interrat_tl1_stop_ind = FALSE;
                    break;
                  }
#endif /*FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
                  case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                  {
                    geran_grr_lte_cco_rsp_t lte_cco_rsp;
                    errno_enum_type         result;

                    memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

                    msgr_init_hdr(
                     &lte_cco_rsp.hdr,
                     MSGR_GERAN_GRR,
                     GERAN_GRR_LTE_CCO_RSP
                    );
                    lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_SUCCESS;
                    result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
                    if (result != E_SUCCESS)
                    {
                      MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                    }
                    rr_control_data_ptr->waiting_for_lte_rrc_cco_nacc_completed_ind = FALSE;
                    break;
                  }
#endif /* FEATURE_LTE */

                  default:
                  {
                    MSG_GERAN_ERROR_1_G(
                    "Unexpected x2g_resel_type: %d",
                     rr_control_data_ptr->x2g_resel_type
                    );
                  }
                }
                MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
                rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
              }
                
              new_rr_event = rr_process_rr_stop_gsm_mode_req(TRUE, gas_id);
              rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
            }

            /* The following MM requests cannot be serviced, but may have been stored */
            /* for later processing later during a panic reset. Respond accordingly, */
            /* and then regain service (auto restart) */

            else if (message_id == (byte) RR_EST_REQ)
            {
              /* Re-save the RR_EST_REQ, which will be processed once cell selection */
              /* completes */
              (void)rr_save_msg(new_message_ptr, gas_id);

              rr_control_data_ptr->rr_state = RR_CELL_SELECTION;
              new_rr_event = rr_selection_control(EV_TRIGGER_CELL_SELECTION, NULL, gas_id);
            }
            else if (message_id == (byte) RR_ABORT_REQ)
            {
              /* Respond to the RR_ABORT_REQ, and trigger cell selection */
              rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);

              rr_control_data_ptr->rr_state = RR_CELL_SELECTION;
              new_rr_event = rr_selection_control(EV_TRIGGER_CELL_SELECTION, NULL, gas_id);
            }

            else
            {
              LOG_UNEXPECTED_MESSAGE(new_message_ptr, rr_main_state_name(rr_control_data_ptr->rr_state));
            }
          } /* message_set == MS_MM_RR */

          else if (message_set == MS_TIMER)
          {
            LOG_UNEXPECTED_TIMER_EXPIRY(
              new_message_ptr->mid_timer_expiry.timer_id,
              rr_main_state_name(rr_control_data_ptr->rr_state)
            );
          } /* (message_set == MS_TIMER) */

          else
          {
            LOG_UNEXPECTED_MESSAGE(new_message_ptr, rr_main_state_name(rr_control_data_ptr->rr_state));
          }
        } /* EV_INPUT_MESSAGE */
        /****************************************************************/
        /* EV_NO_BCCH_FOUND                                             */
        /*                                                              */
        /* A PLMN search was triggered, but cached information was still*/
        /* valid, and indicated no power-scan was required.             */
        /* No action required (L1 is still in NULL)                     */
        /****************************************************************/
        else if (rr_event == EV_NO_BCCH_FOUND)
        {
          /* Remove panic reset saved message */
          rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);
          rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
          new_rr_event = rr_trigger_gsm_as_deactivation( RR_DEACT_REASON_STOP_CAMPING, gas_id);
        }


        /* A cell selection was triggered, either by a PLMN SELECT REQ, or upon finishing a */
        /* PLMN search due to a PLMN LIST REQ */
        else if (rr_event == EV_CELL_SELECTION_TRIGGERED)
        {
          rr_control_data_ptr->rr_state = RR_CELL_SELECTION;
        }

        /* Panic Reset triggered - restart */
        else if (rr_event == EV_PANIC_RESET)
        {
          /* Normally RR_NO_CHANNELS is a transitory state - the exception to this */
          /* is when RR was activated via a RR_ACT_REQ. In that case, it will stay */
          /* here after completing the early power scan. */
          /* No specific action to take here */
          ;
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(rr_event),
                               rr_main_state_name(rr_control_data_ptr->rr_state));
        }
      } /* RR_NO_CHANNELS state */
      break;

      /***********************************************************************/
      /* RR_CELL_SELECTION                                                   */
      /*                                                                     */
      /* Cell selection in progress.                                         */
      /***********************************************************************/
      case RR_CELL_SELECTION:
      {
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          switch (message_set)
          {
            case MS_RR_L1:
            {
              switch ((rr_l1_message_id_enum_T) message_id)
              {
                case MPH_POWER_SCAN_CNF:
                case MPH_DECODE_BCCH_LIST_FAILURE_IND:
                case MPH_DECODE_BCCH_LIST_CNF:
                case MPH_SELECT_SPECIFIC_BCCH_CNF:
                case MPH_FCCH_SCH_DECODE_IND:
                {
                  new_rr_event = rr_selection_control(
                    EV_INPUT_MESSAGE,
                    new_message_ptr,
                    gas_id
                  );
                  break;
                }

                case MPH_BLOCK_QUALITY_IND:
                case MPH_SERVING_IDLE_MEAS_IND:
                case MPH_SURROUND_MEAS_IND:
                case MPH_SURROUND_WCDMA_IDLE_MEAS_IND:
                case MPH_SERVING_AUX_MEAS_IND:
                {
                  /* These messages may be sent by L1 after early camping but
                  before starting idle mode properly.  They should be ignored. */
                  break;
                }

                case MPH_SURROUND_UPDATE_FAILURE_IND:
                {
                  /* This message may be received in certain race conditions.  No
                  action is necessary. */
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_RR_L1 */

            case MS_MM_RR:
            {
              switch ((mm_rr_message_id_enum_T) message_id)
              {
                case RR_PLMN_SELECT_REQ:
                {
                  rr_plmn_select_req_T  *rr_plmn_select_req = &new_message_ptr->rr_plmn_select_req;

                  /* If the current cell selection in progress was initiated via a PLMN SELECT REQ */
                  /* and the contents of the PLMN SELECT REQ match those currently stored, don't abort */
                  if ((rr_PLMN_request_details_ptr->search_reason == RR_PLMN_SELECTION) &&
                      (rr_compare_plmn_select_req_info(rr_plmn_select_req, gas_id)))
                  {
                    MSG_GERAN_HIGH_0_G("Cell selection not aborted, duplicate PLMN SELECT REQ");
                  }
                  else
                  {
                    /* save the message, abort the cell selection, then process the message */
                    (void)rr_save_msg(new_message_ptr, gas_id);
                    new_rr_event = rr_selection_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);
                  }
                  break;
                }

                case RR_PLMN_LIST_REQ:
                {
                  rr_plmn_list_req_T  * rr_plmn_list_req = &new_message_ptr->rr_plmn_list_req;

                  if (rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
                  {
                    MSG_GERAN_HIGH_0_G("Rejecting Automatic PLMN search during cell selection");
                    rr_send_plmn_list_cnf(NULL, MM_AS_ABORTED, rr_plmn_list_req, gas_id);
                  }
                  else
                  {
                    /* save the message, abort the cell selection, then process the message */
                    (void)rr_save_msg(new_message_ptr, gas_id);
                    new_rr_event = rr_selection_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);
                  }
                  break;
                }

                case RR_EST_REQ:
                {
                  /* save the message for later processing */
                  (void)rr_save_msg(new_message_ptr, gas_id);

                  /**********************************************************/
                  /* don't abort the cell selection since RR cannot process */
                  /* the RR_EST_REQ without a cell. Must be handled after   */
                  /* the cell selection completes                           */
                  /**********************************************************/

                  if (new_message_ptr->rr_est_req.establishment_cause ==
                      (byte)MM_RR_EMERGENCY_CALL)
                  {
                    /**
                     * If the RR_EST_REQ is for an emergency call, RR should camp on
                     * a the strongest cell even if it is in the wrong PLMN.  Reset
                     * the 'processed' flags in the candidate cell database so that
                     * RR will re-examine any cells whose BCCHs have been decoded so
                     * far, and send an even in that will cause RR to re-examine all
                     * valid entries.
                     */
                    new_rr_event = rr_selection_control(RR_EV_REPROCESS_CANDIDATE_CELL_DB, NULL, gas_id);
                  }
                  else
                  if (new_message_ptr->rr_est_req.establishment_cause ==
                      (byte)MM_RR_CALL_REEST)
                  {
                    MSG_GERAN_HIGH_0_G("Reestablishment not possible - no candidates");

                    rr_send_abort_ind(RR_CONNECTION_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
                    grr_throwaway_saved_message(gas_id);
                  }
                  break;
                }

                case RR_ABORT_REQ:
                {
                  if ( grr_verify_saved_mm_message( RR_EST_REQ, gas_id ) )
                  {
                    grr_throwaway_saved_message(gas_id);
                    /* no connection exists, respond with success */
                    rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);                    
                  }
                  else
                  {                  
                    MSG_GERAN_ERROR_1_G("RR_ABORT_REQ: Unexpected when No connection Exists in RR State %d", rr_control_data_ptr->rr_state );
                    /* no connection exists, respond with success */
                    rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);

                    /* clear any saved MM messages since the abort overrides them */
                    rr_delete_saved_msg(gas_id);
                  }                 
                  break;
                }
                case RR_DEACT_REQ:
                case RR_STOP_GSM_MODE_REQ:
                {
                  (void)rr_save_msg(new_message_ptr, gas_id);
                  new_rr_event = rr_selection_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  break;
                }

                case RR_BPLMN_SEARCH_ABORT_REQ:
                {
                  if (grr_verify_saved_mm_message(RR_PLMN_LIST_REQ, gas_id))
                  {
                    rr_delete_saved_msg(gas_id);
                  }
                  break;
                }

                case RR_GMM_GPRS_STATE_CHANGE_REQ:
                {
                  break;  // no action required
                }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q                
                case RR_PSEUDO_LTE_LIST_UPDATE_IND:
                {
                  break;  // no action required
                }
#endif
                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_MM_RR */

            case MS_TIMER:
            {
              switch (new_message_ptr->mid_timer_expiry.timer_id)
              {
                case T3122:
                {
                  // no action required - NAS-MM will get notified when the next RR_SERVICE_IND is sent
                  MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
                  break;
                }

                default:
                {
                  new_rr_event = rr_selection_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                }
              }
              break;
            } /* MS_TIMER */

            case MS_MAC_RR:
            {
              switch ((mac_grr_sig_id_t) message_id)
              {
                case MAC_GRR_MAC_STATUS:
                {
                  /* Ignore this here */
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_MAC_RR */

            case MS_RR_RR:
            {
              switch (new_message_ptr->rr.header.rr_message_set)
              {
                /* Cell Selection uses legacy events rather than
                the rr_cell_acq_control() state machine, so ignore
                these messages here. */
                case RR_CELL_ACQ_IMSG:
                {
                  break;
                }

                case RR_L1_IDLE_OMSG:
                case RR_SELECT_BCCH_OMSG:
                {
                  new_rr_event = rr_selection_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  break;
                }

                case RR_IMSG:
                {
                  switch (new_message_ptr->rr.header.imh.message_id)
                  {
                    case RR_IMSG_MSC_CHANGE_IND:
                    {
                      /*
                      Update NAS as required
                      RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                      This will be sent after RR/L1 enters Idle
                      */
                      rr_msc_update(
                        FALSE,   // can_send_service_ind
                        gas_id   // gas_id
                      );
                      break;
                    }

                    case RR_IMSG_PS_ACCESS_REQ:
                    {
                      // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                      rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                      // Call the call-back function (if one was supplied)
                      CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                      break;
                    }

                    case RR_IMSG_NO_PS_ACCESS_REQ:
                    {
                      // No PS access request (could be TLLI unassignment, CS-only)

                      rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                      // Call the call-back function (if one was supplied)
                      CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                      break;
                    }

                    case RR_IMSG_PAGE_IND:
                    {
                      rr_imsg_page_ind_t *imsg = &new_message_ptr->rr.rr.imsg.page_ind;

                      // A page received during cell selection is saved. It is forwarded to NAS when RR/L1
                      // are in Packet Idle
                      rr_ce_save_page(&imsg->page_info, gas_id);
                      break;
                    }

                    case RR_IMSG_PAGE_MODE_IND:
                    {
                      // Changes in OTA page mode are ignored during cell selection
                      // (these may be received after Early Camping)
                      MSG_GERAN_MED_0_G("Page Mode change ignored");
                      break;
                    }

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message_ptr,
                        rr_main_state_name(rr_control_data_ptr->rr_state)
                      );
                    }
                  }
                  break;
                } /* RR_IMSG */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message_ptr,
                    rr_main_state_name(rr_control_data_ptr->rr_state)
                  );
                }
              }
              break;
            } /* MS_RR_RR */

#ifdef FEATURE_SGLTE
            case MS_MSGR_RR:
            {
              switch (new_message_ptr->rr_msgr_msg.message.hdr.id)
              {
#ifdef FEATURE_LTE
                case GERAN_GRR_LTE_PLMN_SRCH_REQ:
                {
                  MSG_GERAN_HIGH_0_G("GERAN_GRR_LTE_PLMN_SRCH_REQ rejected during cell selection");
                  rr_sglte_reject_lte_plmn_srch_req(gas_id);

                  rr_event = EV_NO_EVENT;
                  break;
                } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */
#endif /* FEATURE_LTE */

                default:
                {
                  MSG_GERAN_ERROR_1_G("Unexpected MS_MSGR_RR message, UMID %08x",
                                      new_message_ptr->rr_msgr_msg.message.hdr.id);
                  break;
                }
              }
              break;
            } /* MS_MSGR_RR */

#ifdef FEATURE_GSM_TDS
            case MS_TDSRRC_RR:
            {
              switch (message_id)
              {
                case TDSRR_INTERRAT_PLMN_SRCH_REQ:
                {
                  MSG_GERAN_HIGH_0_G("TDSRR_INTERRAT_PLMN_SRCH_REQ rejected during cell selection");
                  rr_sglte_reject_tds_plmn_srch_req(gas_id);

                  rr_event = EV_NO_EVENT;
                  break;
                } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */

                default:
                {
                  MSG_GERAN_ERROR_1_G("Unexpected MS_TDSRRC_RR message, message_id %08x",
                                      message_id);
                  break;
                }
              }
              break;
            } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_SGLTE */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                     rr_main_state_name(rr_control_data_ptr->rr_state));
            }
          } /* switch (message_set) */
        }

        else if (rr_event == EV_CELL_SELECTION_TRIGGERED)
        {
          /* already in the appropriate state, can be ignored */
        }
        else if (rr_event == EV_TRIGGER_CELL_SELECTION)
        {
          /* requesting a cell selection, since already performing one can be ignored */
        }

        /* Cell selection returns EV_NO_BCCH_FOUND for two reasons:
            a) No suitable cells were found (MM will have been informed of no service)
            b) The cell selection was interrupted by a pending message from MM
           To ensure L1 is in a state to perform the next action, deactivate it here
           and then process any message after that */
        else if (rr_event == EV_NO_BCCH_FOUND)
        {
#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING
          rr_set_ue_oos_flag(TRUE, gas_id);
#endif /* FEATURE_GSM_TX_DIV_ANTNA_SWITCHING */

          /* clear any saved pages since they cannot be processed with no cell */
          rr_ce_discard_saved_pages(gas_id);

          /* Remove any message saved in case of panic reset */
          rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);

#ifdef FEATURE_LTE
          rr_connection_release_reset_g2l_blind_redir_flag(gas_id);
#endif /* FEATURE_LTE */
          /* Deactivate to keep RR/L1 in sync - setting the reason to STOP CAMPING will */
          /* mean no messages are sent to MM */
          rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
          new_rr_event = rr_trigger_gsm_as_deactivation(RR_DEACT_REASON_STOP_CAMPING, gas_id);
        }
        /**
         * EV_CELL_SELECTION_ABORTED is generated when an RR_DEACT_REQ
         * is received during a cell selection power scan.  If the deact
         * reason is RR_MM_MODE_CHANGE the power scan will be allowed to
         * finish before this event is generated.  Otherwise, this event
         * will be generated immediately.
         */
        else if (rr_event == EV_CELL_SELECTION_ABORTED)
        {
#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING
          /* A previous cell selection attempt may have resulted in
          the ue_oos flag being left TRUE.  We need to reset it here
          as we no longer consider the UE to be OOS. */
          rr_set_ue_oos_flag(FALSE, gas_id);
#endif /* FEATURE_GSM_TX_DIV_ANTNA_SWITCHING */

          /* Remove any message saved in case of panic reset */
          rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);

          /* Deactivate to keep RR/L1 in sync - setting the reason to STOP CAMPING will */
          /* mean no messages are sent to MM */
          rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
          new_rr_event = rr_trigger_gsm_as_deactivation(RR_DEACT_REASON_STOP_CAMPING, gas_id);
        }

        else if ((rr_event == RR_EV_GSM_SI_RECEIVED)            ||
                 (rr_event == RR_EV_EARLY_CAMP_SI_RECEIVED)     ||
                 (rr_event == RR_EV_GPRS_SI13_RECEIVED)         ||
                 (rr_event == RR_EV_CANDIDATE_CELL_SI_RECEIVED)
                )
        {
          /* forward to cell selection control */
          new_rr_event = rr_selection_control(rr_event, NULL, gas_id);

          /* remain in the same state to process any generated events */
        }
        else if ((rr_event == EV_GO_TO_IDLE) ||
                 (rr_event == EV_GO_TO_IDLE_PLMN_SELECTION))
        {
          boolean notify_mm_plmn_select_cnf;

#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING
          /* A previous cell selection attempt may have resulted in
          the ue_oos flag being left TRUE.  We need to reset it here
          as we no longer consider the UE to be OOS. */
          rr_set_ue_oos_flag(FALSE, gas_id);
#endif /* FEATURE_GSM_TX_DIV_ANTNA_SWITCHING */

#ifdef FEATURE_LTE
          rr_dedicated_priorities_request_from_owner(gas_id);
#endif /* FEATURE_LTE */

          if (rr_event == EV_GO_TO_IDLE_PLMN_SELECTION)
          {
            notify_mm_plmn_select_cnf = TRUE;
          }
          else
          {
            notify_mm_plmn_select_cnf = FALSE;
          }

          /* Remove any message saved in case of panic reset */
          rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);

          rr_grr_send_imsg_camp_on_cell_req(
            TRUE,                             // notify_mm_service_ind
            notify_mm_plmn_select_cnf,        // notify_mm_plmn_select_cnf
            FALSE,                            // purge_gmm_signalling
            gas_id                            // gas_id
          );

#ifdef FEATURE_IDLE_DRX_SCALING
          if (rr_idle_drx_start_scaling_suspend_timer(gas_id))
          {
            MSG_GERAN_HIGH_0_G("Started the IDLE_DRX_SCALING_SUSPEND TIMER");
          }
#endif /* FEATURE_IDLE_DRX_SCALING */

          rr_control_data_ptr->rr_state = RR_GRR;
        }

        /* Panic Reset triggered */
        else if (rr_event == EV_PANIC_RESET)
        {
#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING
          /* A previous cell selection attempt may have resulted in
          the ue_oos flag being left TRUE.  We need to reset it here
          as we no longer consider the UE to be OOS. */
          rr_set_ue_oos_flag(FALSE, gas_id);
#endif /* FEATURE_GSM_TX_DIV_ANTNA_SWITCHING */

          /* Reset cell selection ready for restart */
          (void) rr_selection_control(EV_RESET_SOFTWARE, NULL, gas_id);
        }

        else if (rr_event == RR_EV_MEASUREMENT_PARAMS_UPDATED)
        {
          /* No action necessary */
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(rr_event),
                               rr_main_state_name(rr_control_data_ptr->rr_state));
        }
      } /* RR_CELL_SELECTION state */
      break;


      /***********************************************************************/
      /* RR_PLMN_LIST_SEARCH                                                 */
      /*                                                                     */
      /* This state is entered when performing a PLMN list search.           */
      /* RR is previously inactive, or in service.                           */
      /* RR should transition to INACTIVE or RR_GRR when the search is       */
      /* finished, process message from MM/MAC if there is any               */
      /***********************************************************************/
      case RR_PLMN_LIST_SEARCH:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_TIMER:
              {
                switch (new_message_ptr->mid_timer_expiry.timer_id)
                {
                  case T3122:
                  {
                    if (rr_plc_get_action_after_plmn_search_complete(gas_id) == RR_PLC_COMPLETE_START_IDLE)
                    {
                      // Update NAS-MM with T3122 status
                      grr_update_mm_with_rr_service_ind(gas_id);
                    }
                    else
                    {
                      // no action required - NAS-MM will get notified when the next RR_SERVICE_IND is sent
                      MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
                    }
                    break;
                  } /* T3122 */

#ifdef FEATURE_LTE
                  case RR_PLMN_LIST_SEARCH_GUARD_TIMER:
                  {
                    new_rr_event = EV_PLMN_LIST_SEARCH_GUARD_TIMER_EXPIRED;
                    break;
                  } /* RR_PLMN_LIST_SEARCH_GUARD_TIMER */
#endif /* FEATURE_LTE */

#ifdef FEATURE_MPLMN_PARTIAL_UPDATING
                  case RR_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER:
                  {
                    new_rr_event = EV_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER_EXPIRED;
                    break;
                  } /* RR_PLMN_LIST_SEARCH_PARTIAL_RESULTS_TIMER */
#endif /* FEATURE_MPLMN_PARTIAL_UPDATING */

                  default:
                  {
                    new_rr_event = rr_control_plmn_search_state_msg_handler(
                      message_set,
                      message_id,
                      new_message_ptr,
                      gas_id
                    );
                  }
                }
                break;
              }

#ifdef FEATURE_SGLTE
              case MS_MSGR_RR:
              {
                switch (new_message_ptr->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case GERAN_GRR_LTE_ABORT_PLMN_SRCH_REQ:
                  {
                    rr_control_data_ptr->sglte_x2g_plmn_list_search_aborted_by_rrc = TRUE;
                    new_rr_event = rr_control_plmn_search_state_msg_handler(message_set, message_id, new_message_ptr, gas_id);
                    break;
                  } /* GERAN_GRR_LTE_ABORT_PLMN_SRCH_REQ */

                  case GERAN_GRR_LTE_GET_CGI_REQ:
                  {
                    geran_grr_lte_get_cgi_rsp_t lte_get_cgi_rsp;
                    errno_enum_type             result;

                    MSG_GERAN_HIGH_0_G("GERAN_GRR_LTE_GET_CGI_REQ is not handled in PLMN list search state");

                    memset(&lte_get_cgi_rsp, 0, sizeof(lte_get_cgi_rsp));

                    msgr_init_hdr(
                      &lte_get_cgi_rsp.msg_hdr,
                      MSGR_GERAN_GRR,
                      GERAN_GRR_LTE_GET_CGI_RSP
                    );

                    result = rr_msgr_send(&lte_get_cgi_rsp.msg_hdr, sizeof(lte_get_cgi_rsp), gas_id);
                    if (result != E_SUCCESS)
                    {
                      MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                    }
                    break;
                  }
#endif /* FEATURE_LTE */

                  default:
                  {
                    new_rr_event = rr_control_plmn_search_state_msg_handler(message_set, message_id, new_message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_MSGR_RR */

#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case TDSRR_INTERRAT_PLMN_SRCH_ABORT_REQ:
                  {
                    rr_control_data_ptr->sglte_x2g_plmn_list_search_aborted_by_rrc = TRUE;
                    new_rr_event = rr_control_plmn_search_state_msg_handler(message_set, message_id, new_message_ptr, gas_id);
                    break;
                  } /* GERAN_GRR_LTE_ABORT_PLMN_SRCH_REQ */

                  default:
                  {
                    new_rr_event = rr_control_plmn_search_state_msg_handler(message_set, message_id, new_message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_SGLTE */

              default:
              {
                new_rr_event = rr_control_plmn_search_state_msg_handler(
                  message_set,
                  message_id,
                  new_message_ptr,
                  gas_id
                );
              }
            }

            if (new_rr_event == EV_INPUT_MESSAGE)
            {
              new_rr_event = rr_plmn_list_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }
            break;
          }

          case EV_PLMN_SEARCH_COMPLETE:
            {
              rr_action_after_plc_e next_action;

              // RR_PLMN_LIST_CNF has been sent, so remove panic reset saved message
              rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);

              next_action = rr_plc_get_action_after_plmn_search_complete(gas_id);
              switch (next_action)
              {
                case RR_PLC_COMPLETE_START_IDLE:
                  /*
                   * restore parameters before the PLMN search
                   */
                  rr_set_preferred_bands(rr_band_pref(gas_id), gas_id);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined (FEATURE_LTE)
                  rr_send_mph_ue_mode_change_req(rr_gsm_only(gas_id), gas_id);
                  if (!rr_gsm_only(gas_id))
                  {
                    rr_l1_update_wcdma_neighbor_list(gas_id);
                  }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

                  rr_grr_send_imsg_camped_on_cell_ind(gas_id);
                  rr_control_data_ptr->rr_state = RR_GRR;
                  break;

                case RR_PLC_COMPLETE_GOTO_INACTIVE:
                  /* deactivate, declares no service */
#ifdef FEATURE_SGLTE
                  if (rr_sglte_internal_stop_gsm_being_processed(gas_id))
                  {
                    MSG_GERAN_HIGH_0_G("Search getting aborted due to internal shutdown ");
                    new_rr_event = rr_trigger_gsm_as_deactivation(RR_DEACT_REASON_STOP_GSM_MODE_REQ, gas_id);
                  }
                  else
#endif /* FEATURE_SGLTE */
                  {
                  new_rr_event = rr_trigger_gsm_as_deactivation(RR_DEACT_REASON_STOP_CAMPING, gas_id);
                  rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
                  }
                  break;

                default:
                  MSG_GERAN_ERROR_1_G("unknown action after plmn search %d", next_action);
#ifdef FEATURE_SGLTE
                  if (rr_sglte_internal_stop_gsm_being_processed(gas_id))
                  {
                    MSG_GERAN_HIGH_0_G("Search getting aborted due to internal shutdown ");
                    new_rr_event = rr_trigger_gsm_as_deactivation(RR_DEACT_REASON_STOP_GSM_MODE_REQ, gas_id);
                  }
                  else
#endif /* FEATURE_SGLTE */
                  {
                  new_rr_event = rr_trigger_gsm_as_deactivation(RR_DEACT_REASON_STOP_CAMPING, gas_id);
                  rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
                  }
                  break;
              }

              rr_plc_init(gas_id);
            }
            break;

          case EV_PLMN_SEARCH_ABORT:
            new_rr_event = rr_plmn_list_control(EV_PLMN_SEARCH_ABORT, new_message_ptr, gas_id);
            break;

          case RR_EV_MEASUREMENT_PARAMS_UPDATED:
          {
            break;
          }

          default:
            new_rr_event = rr_plmn_list_control(rr_event, new_message_ptr, gas_id);
            break;
        }
      } /* RR_PLMN_LIST_SEARCH state */
      break;

      /**********************************************************************/
      /* RR_GRR                                                             */
      /*                                                                    */
      /* RR is camped on a cell, and GRR is performed idle procedures       */
      /**********************************************************************/
      case RR_GRR:
      {
        /*******************************************************************/
        /* Some messages and events require specific action in this state, */
        /* so must be checked for here. All others are passed through to   */
        /* grr51_control()                                                 */
        /*******************************************************************/
        if ( rr_event == EV_INPUT_MESSAGE )
        {
          switch (message_set)
          {
            case MS_RR_RR:
            {
              switch (new_message_ptr->rr.header.rr_message_set)
              {
                case RR_IMSG:
                {
                  switch (message_id)
                  {
                    case RR_IMSG_PS_ACCESS_IND:
                    {
                      rr_grr_send_imsg(RR_GRR_IMSG_PS_ACCESS_IND, gas_id);
                      break;
                    } /* RR_IMSG_PS_ACCESS_IND */

#ifdef FEATURE_LTE
                    case RR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND:
                    {
                      rr_grr_send_imsg(RR_GRR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND, gas_id);
                      break;
                    } /* RR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND */

                    case RR_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND:
                    {
                      rr_grr_send_imsg(RR_GRR_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND, gas_id);
                      break;
                    } /* RR_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND */

                    case RR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND:
                    {
                      rr_grr_send_imsg(RR_GRR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND, gas_id);
                      break;
                    } /* RR_GRR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND */
#endif /* FEATURE_LTE */

                    case RR_IMSG_PS_ACCESS_REQ:
                    {
                      rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                      rr_grr_send_imsg_ps_access_req(
                        imsg->purge_gmm_signalling,   // purge_gmm_signalling
                        imsg->confirm_cb_fn,          // confirm_cb_fn
                        gas_id                        // gas_id
                      );
                      break;
                    }

                    case RR_IMSG_NO_PS_ACCESS_REQ:
                    {
                      rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                      rr_grr_send_imsg_no_ps_access_req(
                        imsg->del_all_pdus,    // del_all_pdus
                        imsg->confirm_cb_fn,   // confirm_cb_fn
                        gas_id                 // gas_id
                      );
                      break;
                    }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                    case RR_IMSG_X2G_CCO_COMPLETE_IND:
                    {
                      MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                      rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                      rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
                      rr_control_data_ptr->is_cco_for_csfb = FALSE;
                      rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
                      rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;

                      rr_set_panic_reset_x2g_cco_flag(RR_X2G_RESEL_TYPE_UNDEFINED, gas_id);

                      /* Now that the other RAT is shut down fully, it is time to
                      enable sleep in GL1. */
                      rr_l1_send_mph_enable_sleep_ind(gas_id);

                      /* Inform GRR of CCO completion so it can take
                      appropriate action based on it's state. */
                      rr_grr_send_imsg(RR_GRR_IMSG_X2G_CCO_COMPLETE_IND, gas_id);
                      break;
                    } /* RR_IMSG_X2G_CCO_COMPLETE_IND */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

                    case RR_IMSG_MSC_CHANGE_IND:
                    {
                      rr_grr_send_imsg_msc_change_ind(gas_id);
                      break;
                    }

                    case RR_IMSG_PAGE_IND:
                    {
                      rr_imsg_page_ind_t *imsg = &new_message_ptr->rr.rr.imsg.page_ind;

                      rr_grr_send_imsg_page_ind(&imsg->page_info, gas_id);
                      break;
                    }

                    case RR_IMSG_PAGE_MODE_IND:
                    {
                      rr_imsg_page_mode_ind_t *imsg = &new_message_ptr->rr.rr.imsg.page_mode_ind;

                      rr_grr_send_imsg_page_mode_ind(
                        imsg->old_page_mode,
                        imsg->new_page_mode,
                        gas_id
                      );
                      break;
                    }

                    case RR_IMSG_L1_IDLE_MODE_CHANGE_REQUIRED:
                    {
                      rr_grr_send_imsg(RR_GRR_IMSG_L1_IDLE_MODE_CHANGE_REQUIRED, gas_id);
                      break;
                    } /* RR_IMSG_L1_IDLE_MODE_CHANGE_REQUIRED */

#ifdef FEATURE_SGLTE
                    case RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ:
                    {
                      MSG_GERAN_HIGH_0_G("RR_STOP_GSM_MODE_REQ(RR_SGLTE_RAT_CHANGE) in RR_GRR");

                      /* First, switch L1 to GSM-only mode.  Once this is done
                      then remove PS support from service domain. */

                      rr_sglte_set_rat_priority_list_gsm_only(gas_id);

                      if(rr_x2g_cco_in_progress(gas_id))
                      {
                        rr_grr_send_imsg_switch_to_cs_only_gsm_only_req(
                          rr_sglte_x2g_cco_nas_split_cb_fn, gas_id
                        );
                      }
                      else
                      {
                        rr_grr_send_imsg_switch_to_cs_only_gsm_only_req(
                          rr_sglte_confirm_rat_change_cb_fn, gas_id
                        );
                      }
                      break;
                    } /* RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ */
                    case RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ:
                    {
                      MSG_GERAN_HIGH_0_G("Received RR_STOP_GSM_MODE_REQ(RR_SGLTE_USER_RAT_CHANGE) in RR_GRR");

                      /* First, switch L1 to GSM-only mode.  Once this is done
                      then remove PS support from service domain. */

                      rr_sglte_set_rat_priority_list_gsm_only(gas_id);

                      rr_grr_send_imsg_switch_to_cs_only_user_gsm_only_req(
                        rr_sglte_confirm_rat_change_cb_fn, gas_id
                      );
                      break;
                    } /* RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ */

#endif /* FEATURE_SGLTE */

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_GRR");
                    }
                  }
                  break;
                } /* RR_IMSG */

                case RR_GRR_OMSG:
                {
                  switch (message_id)
                  {
#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                    case RR_GRR_OMSG_X2G_CCO_STATUS_IND:
                    {
                      MSG_GERAN_HIGH_1_G(
                        "X2G CCO status: %d",
                        new_message_ptr->rr.grr.omsg.x2g_cco_status_ind.status
                      );

                      (void)rr_timer_cancel(T3174, gas_id);

                      switch (new_message_ptr->rr.grr.omsg.x2g_cco_status_ind.status)
                      {
                        case RR_CCO_STATUS_SUCCESS:
                        {
                          switch (rr_control_data_ptr->x2g_resel_type)
                          {
#ifdef FEATURE_WCDMA
                            case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
                            {

#ifdef FEATURE_SGLTE
                              rr_x2g_send_x2g_cco_rsp_success(RR_RAT_WCDMA, gas_id);
#else
                              rr_send_interrat_cc_order_cnf(CCO_SUCCESS, RR_GSM_CCO_OTHER_REASON, gas_id);
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_LTE
                              rr_dedicated_priorities_take_ownership(gas_id);
#endif /* FEATURE_LTE */

                              /* WCDMA RRC does not confirm completion of W2G CCO,
                              so we just send completion indication to ourselves here. */
                              rr_send_imsg(RR_IMSG_X2G_CCO_COMPLETE_IND, gas_id);
                              break;
                            } /* RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                            // TODO DSDA
                            case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                            {

#ifdef FEATURE_SGLTE
                              rr_x2g_send_x2g_cco_rsp_success(RR_RAT_UTRAN_TDD, gas_id);
#else
                              rr_send_tds_interrat_cc_order_cnf(
                                TDSCCO_SUCCESS,
                                TDSRR_GSM_CCO_OTHER_REASON,
                                gas_id
                              );
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
                              rr_dedicated_priorities_take_ownership(gas_id);
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/

                              rr_control_data_ptr->waiting_for_tdsrr_interrat_tl1_stop_ind = TRUE;
                              break;
                            } /* RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
                            // TODO DSDA
                            case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                            {

#ifdef FEATURE_SGLTE
                              rr_x2g_send_x2g_cco_rsp_success(RR_RAT_LTE, gas_id);
#else
                              geran_grr_lte_cco_rsp_t lte_cco_rsp;
                              errno_enum_type         result;

                              memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

                              msgr_init_hdr(
                                &lte_cco_rsp.hdr,
                                MSGR_GERAN_GRR,
                                GERAN_GRR_LTE_CCO_RSP
                              );
                              lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_SUCCESS;
                              result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
                              if (result != E_SUCCESS)
                              {
                                MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                              }
#endif /* FEATURE_SGLTE */

                              rr_control_data_ptr->waiting_for_lte_rrc_cco_nacc_completed_ind = TRUE;
                              break;
                            } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_LTE */

                            default:
                            {
                              MSG_GERAN_ERROR_1_G(
                                "Unexpected x2g_resel_type: %d",
                                rr_control_data_ptr->x2g_resel_type
                              );
                            }
                          }

                          rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                          rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                          break;
                        } /* RR_GRR_CCO_STATUS_SUCCESS */

                        case RR_CCO_STATUS_ABORTED_HIGH_PRIORITY_REQUEST:
                        {

                          /* Send a signal to the originating RRC indicating
                          that the CCO procedure has been completed
                          successfully.  LTE is told specifically that the CCO
                          procedure was aborted due to a high priority request
                          from NAS, but WCDMA and TDSCDMA are just told that
                          the procedure completed successfully.  Both LTE and
                          TDSCDMA send a response to the 'CCO complete'
                          indication but WCDMA does not, so if the CCO was
                          from WCDMA then RR will immediately send an internal
                          signal RR_IMSG_X2G_CCO_COMPLETE_IND to finish the
                          procedure. */

#ifdef FEATURE_SGLTE
                          rr_x2g_send_x2g_cco_rsp_aborted_by_nas(
                            rr_map_x2g_resel_type_to_rat(rr_control_data_ptr->x2g_resel_type),
                            gas_id
                          );

                          if (rr_control_data_ptr->x2g_resel_type == RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER)
                          {
                            rr_send_imsg(RR_IMSG_X2G_CCO_COMPLETE_IND, gas_id);
                          }
#else
                          switch (rr_control_data_ptr->x2g_resel_type)
                          {
#ifdef FEATURE_WCDMA
                            case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
                            {
                              rr_send_interrat_cc_order_cnf(CCO_SUCCESS, RR_GSM_CCO_OTHER_REASON, gas_id);
                              rr_send_imsg(RR_IMSG_X2G_CCO_COMPLETE_IND, gas_id);
                              break;
                            } /* RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                            // TODO DSDA
                            case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                            {
                              rr_send_tds_interrat_cc_order_cnf(
                                TDSCCO_SUCCESS,
                                TDSRR_GSM_CCO_OTHER_REASON,
                                gas_id
                              );
                              break;
                            } /* RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
                            // TODO DSDA
                            case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                            {
                              geran_grr_lte_cco_rsp_t lte_cco_rsp;
                              errno_enum_type         result;

                              memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

                              msgr_init_hdr(
                                &lte_cco_rsp.hdr,
                                MSGR_GERAN_GRR,
                                GERAN_GRR_LTE_CCO_RSP
                              );
                              lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_ABORTED_DUE_TO_HIGH_PRI_REQ;
                              result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
                              if (result != E_SUCCESS)
                              {
                                MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                              }
                              rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                              break;
                            } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_LTE */

                            default:
                            {
                              MSG_GERAN_ERROR_1_G(
                                "Unexpected x2g_resel_type: %d",
                                rr_control_data_ptr->x2g_resel_type
                              );
                            }
                          }
#endif /* FEATURE_SGLTE */

                          rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                          rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                          break;
                        } /* RR_CCO_STATUS_ABORTED_HIGH_PRIORITY_REQ */
#ifdef FEATURE_SGLTE
                        case RR_CCO_STATUS_REJECTED:
                        {
                          RR_ASSERT_WARN(rr_is_sglte_mode_active(gas_id));

                          MSG_GERAN_HIGH_0_G("SGLTE X2G CCO rejected by GRR");

                          /* Send reject to originating RAT. */
                          rr_x2g_send_x2g_cco_rsp_failure_before_act_ind(
                            rr_map_x2g_resel_type_to_rat(rr_control_data_ptr->x2g_resel_type),
                            RR_X2G_RESEL_STATUS_FAILURE_CELL_BCCH_NOT_FOUND,
                            gas_id
                          );

                          rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                          rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;

                          (void)rr_timer_cancel(T3174, gas_id);
                          break;
                        } /* RR_CCO_STATUS_REJECTED */
#endif /* FEATURE_SGLTE */

                        default:
                        {
                          /* RR is no longer camped, so clear the camped on flag. */
                          gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);
                          scell->gsm.camped_on_flag = FALSE;

                          rr_control_data_ptr->x2g_cco_status = new_message_ptr->rr.grr.omsg.x2g_cco_status_ind.status;
                          set_rr_deact_reason(RR_DEACT_REASON_SWITCH_TO_OTHER_RAT, gas_id);
                          new_rr_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
                          rr_control_data_ptr->rr_state = RR_X2G_CCO_FAILED;
                          break;
                        }
                      }
                      break;
                    } /* RR_GRR_OMSG_X2G_CCO_STATUS_IND */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_SGLTE
                    case RR_GRR_OMSG_SGLTE_X2G_CNF:
                    {
                      /* Activation of packet domain and switch to multi-RAT
                      mode has been completed. */

                      switch (rr_control_data_ptr->x2g_resel_type)
                      {
                        case RR_X2G_RESEL_TYPE_L2G_RESELECTION:
                        case RR_X2G_RESEL_TYPE_L2G_SGLTE_REDIRECTION:
                        case RR_X2G_RESEL_TYPE_T2G_RESELECTION:
                        case RR_X2G_RESEL_TYPE_T2G_SGLTE_REDIRECTION:
                        {
                          MSG_GERAN_HIGH_1_G("SGLTE X2G procedure (x2g_resel_type: %d) completed successfully",
                                             rr_control_data_ptr->x2g_resel_type);

                          rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                          rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                          break;
                        } /* RR_X2G_RESEL_TYPE_L2G_RESELECTION ... */

                        case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                        case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                        {
                          MSG_GERAN_HIGH_1_G("SGLTE_X2G CCO (x2g_resel_type: %d) ongoing",
                                             rr_control_data_ptr->x2g_resel_type);
                          break;
                        } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER ... */

                        default:
                        {
                          MSG_GERAN_ERROR_1_G("Unexpected x2g_resel_type: %d", rr_control_data_ptr->x2g_resel_type);
                        }
                      }
                      break;
                    } /* RR_GRR_OMSG_SGLTE_X2G_CNF */

                    case RR_GRR_OMSG_SGLTE_X2G_ABORT_CNF:
                    {
                      /* SGLTE X2G procedure has been aborted, need to respond with
                      and abort confirmation to the originating RAT. */

                      RR_ASSERT_WARN(rr_gsm_only(gas_id));
                      RR_ASSERT_WARN(rr_sel_get_service_domain(gas_id) == SYS_SRV_DOMAIN_CS_ONLY);

                      switch (rr_control_data_ptr->x2g_resel_type)
                      {
                        case RR_X2G_RESEL_TYPE_L2G_RESELECTION:
                        case RR_X2G_RESEL_TYPE_T2G_RESELECTION:
                        {
                          rr_x2g_send_x2g_resel_abort_rsp(
                            rr_map_x2g_resel_type_to_rat(rr_control_data_ptr->x2g_resel_type),
                            gas_id
                          );
                          break;
                        } /* RR_X2G_RESEL_TYPE_L2G_RESELECTION ... */

                        case RR_X2G_RESEL_TYPE_L2G_SGLTE_REDIRECTION:
                        case RR_X2G_RESEL_TYPE_T2G_SGLTE_REDIRECTION:
                        {
                          rr_x2g_send_x2g_redir_abort_rsp(
                            rr_map_x2g_resel_type_to_rat(rr_control_data_ptr->x2g_resel_type),
                            gas_id
                          );
                          break;
                        } /* RR_X2G_RESEL_TYPE_L2G_SGLTE_REDIRECTION ... */

                        case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                        case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                        {
                          (void)rr_timer_cancel(T3174, gas_id);

                          rr_x2g_send_x2g_cco_abort_rsp(
                            rr_map_x2g_resel_type_to_rat(rr_control_data_ptr->x2g_resel_type),
                            gas_id
                          );
                          break;
                        } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER ... */

                        default:
                        {
                          MSG_GERAN_ERROR_1_G("Unexpected x2g_resel_type: %d", rr_control_data_ptr->x2g_resel_type);
                        }
                      }

                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                      rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;

                      (void)rr_timer_cancel(T3174, gas_id);
                      break;
                    } /* RR_GRR_OMSG_SGLTE_X2G_CCO_ABORT_CNF */

                    case RR_GRR_OMSG_SGLTE_X2G_REJECT_IND:
                    {
                      rr_x2g_send_x2g_IRAT_rej(rr_control_data_ptr->x2g_resel_type,gas_id);
                      MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                      rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                      rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
                      rr_control_data_ptr->is_cco_for_csfb = FALSE;
                      rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
                      rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;
                      break;
                    } /* RR_GRR_OMSG_SGLTE_X2G_REJECT_IND */

#endif /* FEATURE_SGLTE */

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_GRR");
                    }
                  }
                  break;
                } /* RR_GRR_OMSG */

                default:
                {
                  new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                }
              }
              break;
            } /* MS_RR_RR */

            case MS_MM_RR:
            {
              switch (message_id)
              {
                case RR_PLMN_SELECT_REQ:
                case RR_DEACT_REQ:
                case RR_EST_REQ:
                case RR_PLMN_LIST_REQ:
                {
                  if (rr_save_msg(new_message_ptr, gas_id))
                  {
                    new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  }
                  else
                  {
                    MSG_GERAN_HIGH_2_G(
                      "New message (%d,%d) has been rejected",
                      (int) message_set,
                      (int) message_id
                    );
                  }
                  break;
                } /* RR_PLMN_SELECT_REQ ... */

                case RR_STOP_GSM_MODE_REQ:
                {
#ifdef FEATURE_INTERRAT_PCCO_GTOW
                  if (new_message_ptr->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_GTOW_CC_ORDER)
                  {
                    /* don't save this message, it is no use any more ! */
                    /* if it's a PCCO G2W, do nothing except send a confirm */
                    rr_send_stop_gsm_mode_cnf(MM_AS_SUCCESS, gas_id);
                  }
                  else
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
                  if (rr_save_msg(new_message_ptr, gas_id))
                  {
                    new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  }
                  else
                  {
                    MSG_GERAN_HIGH_2_G(
                      "New message (%d,%d) has been rejected",
                      (int) message_set,
                      (int) message_id
                    );
                  }
                  break;
                } /* RR_STOP_GSM_MODE_REQ */

                default:
                {
                  new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                }
              }
              break;
            } /* MS_MM_RR */

            case MS_RR_L2:
            {
              switch (message_id)
              {
                case DL_UNIT_DATA_IND:
                {
                  /* This message is syntactically checked before being passed to GRR. */
                  if (rr_prepare_ota_msg_for_parsing(&new_message_ptr->dl_unit_data_ind, gas_id))
                  {
                    new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  }
                  break;
                } /* DL_UNIT_DATA_IND */

                default:
                {
                  new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                }
              }
              break;
            } /* MS_RR_L2 */

            case MS_MAC_RR:
            {
              switch (message_id)
              {
                case MAC_GRR_MAC_STATUS:
                {
                  new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);

                  // GRR procedures all make use of the RR-PS-ACCESS module to control PS access
                  rr_ps_access_control(new_message_ptr, gas_id);
                  break;
                } /* MAC_GRR_MAC_STATUS */

                case MAC_GRR_PS_ACCESS_CNF:
                {
                  // GRR procedures all make use of the RR-PS-ACCESS module to control PS access
                  rr_ps_access_control(new_message_ptr, gas_id);
                  break;
                }

                default:
                {
                  new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                }
              }
              break;
            } /* MS_MAC_RR */

#ifdef FEATURE_SGLTE
            case MS_MSGR_RR:
            {
              switch (new_message_ptr->rr_msgr_msg.message.hdr.id)
              {
#ifdef FEATURE_LTE
                case GERAN_GRR_LTE_PLMN_SRCH_REQ:
                {
                  if (rr_save_msg(new_message_ptr, gas_id))
                  {
                    new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  }
                  else
                  {
                    MSG_GERAN_HIGH_2_G("New message (%d,%d) has been rejected", message_set, message_id);
                  }
                  break;
                } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */

                case GERAN_GRR_LTE_RESEL_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    if (rr_sglte_is_full_service_available(gas_id) && rr_cell_supports_gprs(gas_id))
                    {
                      geran_grr_lte_resel_req_t * lte_resel_req = &new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_resel_req;
                      sglte_l2g_resel_req_ded_pri_info_t *lte_req_ded_ptr;

                      lte_req_ded_ptr = (sglte_l2g_resel_req_ded_pri_info_t *)GPRS_MEM_CALLOC(sizeof(sglte_l2g_resel_req_ded_pri_info_t));
                      RR_NULL_CHECK_FATAL(lte_req_ded_ptr);

                      lte_req_ded_ptr->dedicated_priority_info =  lte_resel_req->dedicated_priority_info;
                      lte_req_ded_ptr->requested_PLMN = lte_resel_req->requested_PLMN; 
                      lte_req_ded_ptr->dedicated_priorities_valid = TRUE;

                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_L2G_RESELECTION;

                      rr_grr_send_imsg_sglte_x2g_req(
                        MM_AS_SGLTE_INTER_RAT_RESELECTION,
                        &lte_resel_req->rat_pri_list,
                        lte_req_ded_ptr, gas_id);

                      GPRS_MEM_FREE(lte_req_ded_ptr);
                    }
                    else
                    {
                      MSG_GERAN_HIGH_2_G("is_full_service_available: %d, cell_supports_gprs: %d",
                                         rr_sglte_is_full_service_available(gas_id),
                                         rr_cell_supports_gprs(gas_id));

                      rr_sglte_reject_lte_resel_req(gas_id);
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_GRR");
                  }
                  break;
                } /* GERAN_GRR_LTE_RESEL_REQ */

                case GERAN_GRR_LTE_REDIR_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    if (rr_sglte_is_full_service_available(gas_id) && rr_cell_supports_gprs(gas_id))
                    {
                      geran_grr_lte_redir_req_t * lte_redir_req = &new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_redir_req;

                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_L2G_SGLTE_REDIRECTION;

                      rr_grr_send_imsg_sglte_x2g_req(
                        MM_AS_SGLTE_INTER_RAT_REDIRECTION,
                        &lte_redir_req->rat_pri_list,
                        NULL, gas_id);
                    }
                    else
                    {
                      MSG_GERAN_HIGH_2_G("is_full_service_available: %d, cell_supports_gprs: %d",
                                         rr_sglte_is_full_service_available(gas_id),
                                         rr_cell_supports_gprs(gas_id));

                      rr_sglte_reject_lte_redir_req(gas_id);
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_GRR");
                  }
                  break;
                } /* GERAN_GRR_LTE_REDIR_REQ */

                case GERAN_GRR_LTE_CCO_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    if (rr_sglte_is_full_service_available(gas_id) && rr_cell_supports_gprs(gas_id))
                    {
                      geran_grr_lte_cco_req_t * req = &new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_cco_req;

                      if (req->network_control_order_present)
                      {
                        MSG_GERAN_HIGH_3_G("Starting L2G CCO (T304: %d ms, NCO: %d, CSFB: %d)",
                                           req->t304, req->network_control_order, req->is_cco_for_csfb);
                      }
                      else
                      {
                        MSG_GERAN_HIGH_2_G("Starting L2G CCO (T304: %d ms, NCO: Not Present, CSFB: %d)",
                                           req->t304, req->is_cco_for_csfb);
                      }

                      if (req->t304 < RR_T304_MINIMUM_DURATION)
                      {
                        MSG_GERAN_ERROR_2_G("T304 %dms too low, increasing to %dms",
                                            req->t304, RR_T304_MINIMUM_DURATION);
                        (void)rr_timer_start(T3174, RR_T304_MINIMUM_DURATION, gas_id);
                      }
                      else
                      {
                        (void)rr_timer_start(T3174, req->t304, gas_id);
                      }

                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER;
                      rr_control_data_ptr->is_cco_for_csfb = req->is_cco_for_csfb;
                      if (req->network_control_order_present)
                      {
                        rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = TRUE;
                        rr_control_data_ptr->x2g_cco_pending_network_control_order = req->network_control_order;
                      }
                      else
                      {
                        rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
                      }

                      rr_grr_send_imsg_sglte_x2g_req(MM_AS_LTOG_CCO,
                                                     &req->rat_pri_list,
                                                     NULL, gas_id);
                    }
                    else
                    {
                      MSG_GERAN_HIGH_2_G("is_full_service_available: %d, cell_supports_gprs: %d",
                                         rr_sglte_is_full_service_available(gas_id),
                                         rr_cell_supports_gprs(gas_id));

                      rr_x2g_send_x2g_cco_rsp_rejection(RR_RAT_LTE, gas_id);
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_GRR");
                  }
                  break;
                } /* GERAN_GRR_LTE_CCO_REQ */

                case GERAN_GRR_LTE_ABORT_RESEL_REQ:
                case GERAN_GRR_LTE_ABORT_REDIR_REQ:
                case GERAN_GRR_LTE_ABORT_CCO_REQ:
                {
                  rr_grr_send_imsg(RR_GRR_IMSG_SGLTE_X2G_ABORT_REQ, gas_id);
                  break;
                } /* GERAN_GRR_LTE_ABORT_CCO_REQ */
#endif /* FEATURE_LTE */

                default:
                {
                  new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                }
              }
              break;
            } /* MS_MSGR_RR */

#ifdef FEATURE_GSM_TDS
            case MS_TDSRRC_RR:
            {
              switch (new_message_ptr->message_header.message_id)
              {
                case TDSRR_INTERRAT_PLMN_SRCH_REQ:
                {
                  if (rr_save_msg(new_message_ptr, gas_id))
                  {
                    new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  }
                  else
                  {
                    MSG_GERAN_HIGH_2_G("New message (%d,%d) has been rejected", message_set, message_id);
                  }
                  break;
                } /* TDSRR_INTERRAT_PLMN_SRCH_REQ */

                case TDSRR_INTERRAT_RESELECTION_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    if (rr_sglte_is_full_service_available(gas_id) && rr_cell_supports_gprs(gas_id))
                    {
                      tdsrr_interrat_reselect_req_type * resel_req = &new_message_ptr->tdsrr_resel_req;

                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_T2G_RESELECTION;

                      rr_grr_send_imsg_sglte_x2g_req(
                        MM_AS_SGLTE_INTER_RAT_RESELECTION,
                        &resel_req->rat_pri_list_info,
                        NULL, gas_id);
                    }
                    else
                    {
                      MSG_GERAN_HIGH_2_G("is_full_service_available: %d, cell_supports_gprs: %d",
                                         rr_sglte_is_full_service_available(gas_id),
                                         rr_cell_supports_gprs(gas_id));

                      rr_sglte_reject_tds_resel_req(gas_id);
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_GRR");
                  }
                  break;
                } /* TDSRR_INTERRAT_RESELECTION_REQ */

                case TDSRR_INTERRAT_REDIRECT_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    if (rr_sglte_is_full_service_available(gas_id) && rr_cell_supports_gprs(gas_id))
                    {
                      tdsrr_interrat_redirect_req_type * redir_req = (tdsrr_interrat_redirect_req_type *) new_message_ptr;

                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_T2G_SGLTE_REDIRECTION;

                      rr_grr_send_imsg_sglte_x2g_req(
                        MM_AS_SGLTE_INTER_RAT_REDIRECTION,
                        &redir_req->rat_pri_list_info,
                        NULL, gas_id);
                    }
                    else
                    {
                      MSG_GERAN_HIGH_2_G("is_full_service_available: %d, cell_supports_gprs: %d",
                                         rr_sglte_is_full_service_available(gas_id),
                                         rr_cell_supports_gprs(gas_id));

                      rr_sglte_reject_tds_redir_req(gas_id);
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_GRR");
                  }
                  break;
                } /* GERAN_GRR_LTE_RESEL_REQ */

                case TDSRR_INTERRAT_CC_ORDER_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    if (rr_sglte_is_full_service_available(gas_id) && rr_cell_supports_gprs(gas_id))
                    {
                      tdsrr_interrat_cc_order_req_type * req = (tdsrr_interrat_cc_order_req_type *) new_message_ptr;

                      MSG_GERAN_HIGH_1_G("Starting T2G CCO (T309: %d ms)", req->T309_value * 1000);

                      if ((req->T309_value * 1000) < RR_T304_MINIMUM_DURATION)
                      {
                        MSG_GERAN_ERROR_2_G("T309 %dms too low, increasing to %dms",
                                            req->T309_value * 1000, RR_T304_MINIMUM_DURATION);
                        (void) rr_timer_start(T3174, RR_T304_MINIMUM_DURATION, gas_id);
                      }
                      else
                      {
                        (void) rr_timer_start(T3174, req->T309_value * 1000, gas_id);
                      }

                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER;
                      rr_control_data_ptr->is_cco_for_csfb = FALSE;

                      rr_grr_send_imsg_sglte_x2g_req(MM_AS_WTOG_INTERRAT_CC_ORDER,
                                                     &req->rat_pri_list_info,
                                                     NULL, gas_id);

                    }
                    else
                    {
                      MSG_GERAN_HIGH_2_G("is_full_service_available: %d, cell_supports_gprs: %d",
                                         rr_sglte_is_full_service_available(gas_id),
                                         rr_cell_supports_gprs(gas_id));

                      rr_x2g_send_x2g_cco_rsp_rejection(RR_RAT_UTRAN_TDD, gas_id);
                    }
                  }
                  break;
                } /* TDSRR_INTERRAT_CC_ORDER_REQ */

                case TDSRR_INTERRAT_RESELECTION_ABORT_REQ:
                case TDSRR_INTERRAT_REDIRECT_ABORT_REQ:
                case TDSRR_INTERRAT_CC_ORDER_ABORT_REQ:
                {
                  rr_grr_send_imsg(RR_GRR_IMSG_SGLTE_X2G_ABORT_REQ, gas_id);
                  break;
                } /* GERAN_GRR_LTE_ABORT_CCO_REQ */

                default:
                {
                  new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                }
              }
              break;
            } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_SGLTE */

            default:
            {
              new_rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }
          }
          break;
        } /* EV_INPUT_MESSAGE */


        /*******************************************************************/
        /* RR_EV_PROCESS_SAVED_MESSAGE                                     */
        /* Process a saved message in RR - this could be                   */
        /*  RR_EST_REQ                                                     */
        /*  RR_PLMN_LIST_REQ                                               */
        /*  RR_STOP_GSM_MODE_REQ                                           */
        /*  RR_DEACT_REQ                                                   */
        /* Note: This does not include RR_PLMN_SELECT_REQ - the event      */
        /* RR_EV_PROCESS_PLMN_SELECT_REQ is returned in this case          */
        /*******************************************************************/
        else if (rr_event == RR_EV_PROCESS_SAVED_MESSAGE)
        {
          rr_event_T  saved_event = rr_process_saved_message(&new_message_ptr, gas_id);

          /* Passing in EV_INPUT_MESSAGE will setup message_set & message_id using */
          /* the contents of new_message_ptr. If new_message_ptr, message_set or */
          /* message_id are NULL, then EV_NO_EVENT will be returned, otherwise */
          /* EV_INPUT_MESSAGE is returned */
          rr_event = rr_extract_message_header_details(
            saved_event,
            new_message_ptr,
            &message_set,
            &message_id
          );

          if ( rr_event == EV_INPUT_MESSAGE )
          {
            /******************************************************************/
            /* MM->RR: RR_EST_REQ                                             */
            /******************************************************************/
            if ( (message_set == MS_MM_RR) && (message_id == (byte) RR_EST_REQ) )
            {
              /* If a panic reset occurs after this point, then there is specific */
              /* handling in the case of panic reset, which sends RR_ABORT_IND to MM */
              /* before restarting RR. Therefore, there is no need to keep the */
              /* RR_EST_REQ saved past this point */
              rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);

              /* This will return EV_RR_EST_REQ */
              new_rr_event = rr_connection_establishment_control(
                EV_INPUT_MESSAGE,
                new_message_ptr,
                gas_id
              );
            }

            else
            /******************************************************************/
            /* MM->RR: RR_PLMN_LIST_REQ                                       */
            /******************************************************************/
            if ( (message_set == MS_MM_RR) && (message_id == (byte) RR_PLMN_LIST_REQ) )
            {
              rr_plmn_list_req_T *req_ptr   = (rr_plmn_list_req_T *)new_message_ptr;
              gprs_scell_info_t  *scell_ptr = rr_gprs_get_scell_info(gas_id);
              boolean             foreground_search;

              /* determine whether a background search can take place */
              if (rr_is_camped(gas_id) && (scell_ptr->l1_idle_mode != GRR_L1_IDLE_MODE_NONE))
              {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
                if (geran_in_multi_standby_mode() && rr_limited_service_in_effect(gas_id))
                {
                  /* UE is in limited service, prioritise the search for recovery */
                  foreground_search = TRUE;
                  MSG_GERAN_HIGH_0_G("Limited service in multi-standby mode");
                }
                else if (geran_in_multi_standby_mode() && rr_ms_other_stack_in_dedicated(gas_id))
                {
                  /* if another stack has a CS voice call and is not pre-emptable */
                  /* move this search to the foreground to effect faster recovery */
                  foreground_search = TRUE;
                  MSG_GERAN_HIGH_0_G("Other stack in dedicated in multi-standby mode");
                }
                else if (rr_is_forbidden_LAI(&(scell_ptr->gsm.location_area_identification), gas_id))
                {
                  /* UE is camped on a forbidden LAI, unlikely to be paged */
                  foreground_search = TRUE;
                  MSG_GERAN_HIGH_0_G("Camped on forbidden LAI");
                }
#if defined(FEATURE_SGLTE_DUAL_SIM) || defined(FEATURE_SVLTE_DUAL_SIM)
                else if ((SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE == geran_get_multi_sim_mode()) &&
                         ((SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC   == req_ptr->service_search) ||
                          (SYS_SERVICE_SEARCH_BACKGROUND_MANUAL      == req_ptr->service_search)))
                {
                  /* According to NAS, these service_search types imply limited service */
                  foreground_search = TRUE; /* this is deliberate, despite the "BACKGROUND" NAS indication */
                  MSG_GERAN_HIGH_0_G("NAS service_search implies limited service in SxLTE+G mode");
                }
#endif /* FEATURE_SGLTE_DUAL_SIM || FEATURE_SVLTE_DUAL_SIM */
                else if (rr_ms_last_bplmn_failed(req_ptr, gas_id))
                {
                  /* if last attempt at a search failed, bump this one into the foreground now */
                  foreground_search = TRUE;
                  MSG_GERAN_HIGH_0_G("Last background search failed due to lack of resource");
                }
                else if (rr_ms_active_stacks_in_forbidden_LAI())
                {
                  /* unusual error condition not captured above, try to recover */
                  foreground_search = TRUE;
                  MSG_GERAN_ERROR_0_G("All active stacks in forbidden LAIs");
                }
                else
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
                {
                  /* background search can take place */
                  foreground_search = FALSE;
                }
              }
              else
              {
                /* not camped or L1 idle mode not suitable for background search */
                foreground_search = TRUE;
              }

              /* if the search cannot take place in the background, declare loss of service */
              if (foreground_search)
              {
                MSG_GERAN_HIGH_0_G("PLMN list search prioritised to foreground");
                rr_send_no_service_for_plmn_list(gas_id);
              }

              rr_start_plmn_list_search(req_ptr, foreground_search, gas_id);
              new_rr_event = rr_plmn_list_control(EV_PLMN_SEARCH_START, NULL, gas_id);
              rr_control_data_ptr->rr_state = RR_PLMN_LIST_SEARCH;
            }

            else
            /******************************************************************/
            /* MM->RR: RR_STOP_GSM_MODE_REQ                                   */
            /******************************************************************/
            if ( (message_set == MS_MM_RR) && (message_id == (byte) RR_STOP_GSM_MODE_REQ) )
            {
              rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
              new_rr_event = rr_process_rr_stop_gsm_mode_req(TRUE, gas_id);
            }

            else
            /******************************************************************/
            /* MM->RR: RR_DEACT_REQ                                           */
            /* This is handled at the top of rr_control() independently of    */
            /* state                                                          */
            /******************************************************************/
            if ( (message_set == MS_MM_RR) && (message_id == (byte) RR_DEACT_REQ) )
            {
              /* Cause processing to loop around again */
              new_rr_event = EV_INPUT_MESSAGE;
            }

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
            else
            /******************************************************************/
            /* L2->RR: DL_UNIT_DATA_IND                                       */
            /* This message is generated by the EDTM module if an IA or IAREJ */
            /* is received on the PACCH                                       */
            /******************************************************************/
            if ((message_set == MS_RR_L2) && (message_id == (byte) DL_UNIT_DATA_IND))
            {
              rr_control_data_ptr->rr_state = RR_CONNECTION_PENDING;
              new_rr_event = EV_INPUT_MESSAGE;
            }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */


#ifdef FEATURE_SGLTE
            else if (message_set == MS_MSGR_RR)
            {
              switch (new_message_ptr->rr_msgr_msg.message.hdr.id)
              {
#ifdef FEATURE_LTE
                case GERAN_GRR_LTE_PLMN_SRCH_REQ:
                {
                  gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

                  if (rr_is_camped(gas_id) &&
                      cell_info->l1_idle_mode != GRR_L1_IDLE_MODE_NONE)
                  {
                    rr_start_sglte_l2g_plmn_list_search(
                      &new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.plmn_srch_req,
                      FALSE,
                      gas_id
                    );
                  }
                  else
                  {
                    rr_send_no_service_ind(gas_id);
                    rr_start_sglte_l2g_plmn_list_search(
                      &new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.plmn_srch_req,
                      TRUE, gas_id
                    );
                  }

                  new_rr_event = rr_plmn_list_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                  rr_control_data_ptr->rr_state = RR_PLMN_LIST_SEARCH;
                  break;
                } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */
#endif /* FEATURE_LTE */

                default:
                {
                  MSG_GERAN_ERROR_1_G("Unexpected saved message, UMID %08x",
                                      new_message_ptr->rr_msgr_msg.message.hdr.id);
                }
              }
            } /* MS_MSGR_RR */

#ifdef FEATURE_GSM_TDS
            else if (message_set == MS_TDSRRC_RR)
            {
              switch (message_id)
              {
                case TDSRR_INTERRAT_PLMN_SRCH_REQ:
                {
                  gprs_scell_info_t * cell_info = rr_gprs_get_scell_info(gas_id);

                  if (rr_is_camped(gas_id) &&
                      cell_info->l1_idle_mode != GRR_L1_IDLE_MODE_NONE)
                  {
                    rr_start_sglte_t2g_plmn_list_search(
                      &new_message_ptr->tdsrr_plmn_srch_req,
                      FALSE,
                      gas_id
                    );
                  }
                  else
                  {
                    rr_send_no_service_ind(gas_id);
                    rr_start_sglte_t2g_plmn_list_search(
                      &new_message_ptr->tdsrr_plmn_srch_req,
                      TRUE,
                      gas_id
                    );
                  }

                  new_rr_event = rr_plmn_list_control(EV_PLMN_SEARCH_START, NULL, gas_id);
                  rr_control_data_ptr->rr_state = RR_PLMN_LIST_SEARCH;

                  break;
                } /* TDSRR_INTERRAT_PLMN_SRCH_REQ */

                default:
                {
                  MSG_GERAN_ERROR_1_G("Unexpected saved message, message_id %02x", message_id);
                }
              }
            }
#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_SGLTE */
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Saved message found to be invalid");
          }
        }

        /*********************************************************/
        /* RR_EV_G2WR_GO_TO_INACTIVE                             */
        /* InterRAT reselection is complete. Finish the shutdown */
        /* of GPRS.  This is also where an RR_STOP_GSM_MODE_REQ  */
        /* or RR_DEACT_REQ is handled eventually.                */
        /*********************************************************/
        else if (rr_event == RR_EV_G2WR_GO_TO_INACTIVE)
        {
          gprs_scell_info_t         * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
          /* Inform CB task that CBCH is no longer present */
          rr_send_cell_change_ind(
            scell_info_ptr->gsm.location_area_identification,
            scell_info_ptr->gsm.cell_identity,
            FALSE,
            gas_id
          );

          rr_send_cb_info(FALSE, gas_id);

          // TODO - need to work out what needs to be initialised here and how best to do it
          rr_grr_control_init(gas_id);
          (void) rr_resel_control(EV_RESET_SOFTWARE, NULL, gas_id);
          rr_cell_acq_control(EV_RESET_SOFTWARE, NULL, gas_id);
          (void) rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, NULL, gas_id);
          (void) rr_gprs_sys_info_acquisition_control( EV_RESET_SOFTWARE, NULL, gas_id);
          (void) rr_plmn_list_control( EV_RESET_SOFTWARE, NULL, gas_id);

#ifdef FEATURE_LTE
          /* Reset g2l_resel_allowed to default value in preparation for return
          to GSM service later. */
          rr_control_data_ptr->g2l_resel_allowed = TRUE;
#endif /* FEATURE_LTE */

          /* As RR is being deactivated after a successful reselection we
          clear the store of cell quality data associated with this PLMN.  This
          will be re-populated if GERAN regains service later. */
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS)
          rr_g2x_cell_quality_data_clear(gas_id);
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

          new_rr_event = rr_process_saved_message(&new_message_ptr, gas_id);
          rr_control_data_ptr->rr_state = RR_INACTIVE;
        }

        /**********************************************************/
        /* RR_EV_TRIGGER_CELL_SELECTION                           */
        /* A cell selection needs to be triggered                 */
        /**********************************************************/
        else if (rr_event == RR_EV_TRIGGER_CELL_SELECTION)
        {
          rr_control_data_ptr->rr_state = RR_CELL_SELECTION;
          new_rr_event = rr_selection_control(EV_TRIGGER_CELL_SELECTION, NULL, gas_id);
        }

        /**********************************************************/
        /* RR_EV_PROCESS_PLMN_SELECT_REQ                          */
        /* A RR_PLMN_SELECT_REQ has been partially processed in   */
        /* in GRR, and the details stored. But a cell selection   */
        /* is now required.                                       */
        /**********************************************************/
        else if (rr_event == RR_EV_PROCESS_PLMN_SELECT_REQ)
        {
          rr_control_data_ptr->rr_state = RR_CELL_SELECTION;
          new_rr_event = rr_selection_control(RR_EV_PROCESS_PLMN_SELECT_REQ, NULL, gas_id);
        }

        else
        /*****************************************************************/
        /* EV_RR_EST_REQ                                                 */
        /*                                                               */
        /* Connection establishment as begun, due to the reception of a  */
        /* RR_EST_REQ                                                    */
        /*****************************************************************/
        if ( rr_event == EV_RR_EST_REQ )
        {
          rr_control_data_ptr->rr_state = RR_CONNECTION_PENDING;

          /* Initialise data ready for a dedicated connection */
          rr_leave_idle_mode(RR_CONNECTION_ESTABLISHMENT, gas_id);
        }

        else
        /*****************************************************************/
        /* EV_PLMN_SEARCH_START                                          */
        /*                                                               */
        /* A foreground PLMN list search has been started, as RR is not  */
        /* camped (e.g. PLMN LIST REQ interrupted reselection).          */
        /* Cell selection must be performed afterwards to get back into  */
        /* service                                                       */
        /*****************************************************************/
        if ( rr_event == EV_PLMN_SEARCH_START )
        {

          rr_control_data_ptr->rr_state = RR_PLMN_LIST_SEARCH;
        }
        else
        /*****************************************************************/
        /* EV_NO_BCCH_FOUND                                              */
        /*                                                               */
        /* A foreground PLMN search has been completed without having    */
        /* to perform a power scan.                                      */
        /* Deactivate to keep RR/L1 in sync (this is necessary, because  */
        /* it is not known what state L1 was left in by GRR), and ensure */
        /* RR/L1 will then be reactivated and cell selection prformed    */
        /*****************************************************************/
        if ( rr_event == EV_NO_BCCH_FOUND )
        {
          /* Remove the saved RR_PLMN_LIST_REQ stored in case of panic reset */
          rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);

          rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
          new_rr_event = rr_trigger_gsm_as_deactivation(
            RR_DEACT_REASON_STOP_CAMPING_THEN_RESTART,
            gas_id
          );
        }

        else if (rr_event == RR_EV_W2G_GO_TO_INACTIVE)
        {
          set_rr_deact_reason(RR_DEACT_REASON_SWITCH_TO_OTHER_RAT, gas_id);
          rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
          new_rr_event = EV_GSM_MODE_DEACTIVATED;
        }
        else if (rr_event == EV_GSM_MODE_DEACTIVATED)
        {
          new_rr_event = grr_control(rr_event, NULL, gas_id);
        }

#ifdef FEATURE_INTERRAT_PCCO_GTOW
        else if (rr_event == RR_EV_G2WR_GO_TO_INACTIVE)
        {
          set_rr_deact_reason(RR_DEACT_REASON_SWITCH_TO_OTHER_RAT, gas_id);
          /* remove any buffered message because the GTOW PCCO succeeded */
          rr_delete_saved_msg(gas_id);
          rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
          new_rr_event = EV_GSM_MODE_DEACTIVATED;
        }
#endif /* FEATURE_INTERRAT_PCCO_GTOW */

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
        else if (rr_event == EV_EDTM_ESTABLISHMENT_COMPLETED)
        {
          rr_control_data_ptr->rr_state = RR_CONNECTION_PENDING;
          new_rr_event = EV_RR_EST_L1_IN_DED;
        }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */

        else if(rr_event == EV_ACCESS_CLASS_NOT_ALLOWED)
        {
          /*GRR is in GRR_NULL state, needs to move GRR state to GRR CAMPED*/
          /* pass CAMPED ON CELL signal to GRR CONTROL, this event will move*/
          /* GRR to GRR_CAMPED*/
          new_rr_event = grr_control(RR_EV_CAMPED_ON_CELL, NULL, gas_id);
        }

        else if (rr_event == RR_EV_CAMPED_ON_CELL)
        {
          rr_grr_send_imsg_camped_on_cell_ind(gas_id);
        }
#ifdef FEATURE_SGLTE
        else if (rr_event == EV_SGLTE_G2L_BLIND_REDIR_REQUIRED)
        {
          new_rr_event = EV_SGLTE_G2L_BLIND_REDIR_REQUIRED;
          rr_control_data_ptr->rr_state = RR_G2X_REDIR;
        }
#endif /*FEATURE_SGLTE*/

        /* Pass all other events into GRR51 state machine for processing */
        else
        {
          new_rr_event = grr_control(rr_event, NULL, gas_id);
        }

        break;
      }


      /***********************************************************************/
      /* RR_CONNECTION_PENDING                                               */
      /*                                                                     */
      /* A connection has been requested by MM (RR_EST_REQ).                 */
      /***********************************************************************/
      case RR_CONNECTION_PENDING:
      {
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (new_message_ptr->rr.header.rr_message_set)
            {
              case RR_CE_OMSG:
              {
                switch (new_message_ptr->rr.header.imh.message_id)
                {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                  case RR_CE_OMSG_X2G_CCO_STATUS_IND:
                  {
                    switch (new_message_ptr->rr.ce.omsg.x2g_cco_status_ind.status)
                    {
                      case RR_CCO_STATUS_SUCCESS:
                      {
                        (void)rr_timer_cancel(T3174, gas_id);

                        switch (rr_control_data_ptr->x2g_resel_type)
                        {
#ifdef FEATURE_WCDMA
                          case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
                          {
                            rr_send_interrat_cc_order_cnf(
                              CCO_SUCCESS,
                              RR_GSM_CCO_OTHER_REASON,
                              gas_id
                            );

                            /* WCDMA RRC does not confirm completion of W2G CCO, so we
                            just confirm here to allow IRAT measurements to procede. */
                            rr_send_imsg(RR_IMSG_X2G_CCO_COMPLETE_IND, gas_id);
                            break;
                          }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                          // TODO DSDA
                          case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                          {
                            rr_send_tds_interrat_cc_order_cnf(
                              TDSCCO_SUCCESS,
                              TDSRR_GSM_CCO_OTHER_REASON,
                              gas_id
                            );

                            /* Wait for TDSRR_INTERRAT_TL1_STOP_IND before resetting x2g_resel_type*/
                            rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER;
                            break;
                          }
#endif /*FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
                          // TODO DSDA
                          case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                          {
                            geran_grr_lte_cco_rsp_t lte_cco_rsp;
                            errno_enum_type         result;

                            memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

                            msgr_init_hdr(
                              &lte_cco_rsp.hdr,
                              MSGR_GERAN_GRR,
                              GERAN_GRR_LTE_CCO_RSP
                            );
                            lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_SUCCESS;
                            result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
                            if (result != E_SUCCESS)
                            {
                              MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                            }
                            rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                            break;
                          }
#endif /* FEATURE_LTE */

                          default:
                          {
                            MSG_GERAN_ERROR_1_G(
                              "Unexpected x2g_resel_type: %d",
                              rr_control_data_ptr->x2g_resel_type
                            );
                          }
                        }
                        break;
                      } /* RR_CCO_STATUS_SUCCESS */

                      default:
                      {
                        /* RR is no longer camped, so clear the camped on flag. */
                        gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);
                        scell->gsm.camped_on_flag = FALSE;

                        rr_control_data_ptr->x2g_cco_status = new_message_ptr->rr.ce.omsg.x2g_cco_status_ind.status;
                        set_rr_deact_reason(RR_DEACT_REASON_SWITCH_TO_OTHER_RAT, gas_id);
                        new_rr_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
                        rr_control_data_ptr->rr_state = RR_X2G_CCO_FAILED;
                      }
                    }
                    break;
                  } /* RR_CE_OMSG_X2G_CCO_FAILURE_IND */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_CONNECTION_PENDING");
                  }
                }
                break;
              } /* RR_CE_OMSG */

              case RR_IMSG:
              {
                switch (new_message_ptr->rr.header.imh.message_id)
                {
                  case RR_IMSG_PS_ACCESS_REQ:
                  {
                    rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

#ifdef FEATURE_GSM_DTM
                    if (imsg->purge_gmm_signalling)
                    {
                      (void)rr_dtm_control(
                        EV_DTM_PS_ACCESS_REQ_PURGE,
                        (void *)imsg->confirm_cb_fn,
                        gas_id
                      );
                    }
                    else
                    {
                      (void)rr_dtm_control(
                        EV_DTM_PS_ACCESS_REQ,
                        (void *)imsg->confirm_cb_fn,
                        gas_id
                      );
                    }
#else
                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
#endif // FEATURE_GSM_DTM
                    break;
                  }

                  case RR_IMSG_NO_PS_ACCESS_REQ:
                  {
                    rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

#ifdef FEATURE_GSM_DTM
                    if (imsg->del_all_pdus)
                    {
                      (void)rr_dtm_control(
                        EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL,
                        (void *)imsg->confirm_cb_fn,
                        gas_id
                      );
                    }
                    else
                    {
                    (void)rr_dtm_control(
                      EV_DTM_NO_PS_ACCESS_REQ,
                      (void *)imsg->confirm_cb_fn,
                      gas_id
                    );
                    }
#else
                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
#endif // FEATURE_GSM_DTM
                    break;
                  }

                  case RR_IMSG_MSC_CHANGE_IND:
                  {
                    /*
                    Update NAS as required
                    RR is camped (from NAS perspective), so a RR_SERVICE_IND can be sent to MM if required
                    */
                    rr_msc_update(
                      TRUE,    // can_send_service_ind
                      gas_id   // gas_id
                    );
                    break;
                  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
                  case RR_IMSG_X2G_CCO_COMPLETE_IND:
                  {
                    MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
                    rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                    rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                    rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
                    rr_control_data_ptr->is_cco_for_csfb = FALSE;
                    rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
                    rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;

                    rr_set_panic_reset_x2g_cco_flag(RR_X2G_RESEL_TYPE_UNDEFINED, gas_id);

                    /* Now that the other RAT is shut down fully, it is time to
                    enable sleep in GL1. */
                    rr_l1_send_mph_enable_sleep_ind(gas_id);

                    rr_l1_update_wcdma_neighbor_list(gas_id);
                    break;
                  } /* RR_IMSG_X2G_CCO_COMPLETE_IND */
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

                  case RR_IMSG_PAGE_IND:
                  {
                    // A CS or PS page has been received for the MS
                    // Note: RR_CONNECTION_PENDING state only relates to RACHing for CS establishment

                    /*
                    3GPP 44.018 3.3.1.1.2 Initiation of the immediate assignment procedure
                    "The RR entity of the mobile station initiates the immediate assignment procedure by
                     scheduling the sending on the RACH and leaving idle mode (in particular, the mobile
                     station shall ignore PAGING REQUEST messages)"
                    */
                    break;
                  }

                  case RR_IMSG_PAGE_MODE_IND:
                  {
                    // A OTA message has been received which has changed the Page Mode
                    // e.g. PAGE REQUEST, IMMEDIATE ASSIGNMENT, IMMEDIATE ASSIGNMENT REJECT, etc

                    /*
                    3GPP 3.3.2.1.1 Paging initiation using paging subchannel on CCCH
                    "The mobile station in idle mode is required to receive and analyse the paging messages
                     and immediate assignment messages sent on the paging subchannel corresponding to its
                     paging subgroup, as specified in 3GPP TS 45.002"
                    */

                    // so changes in Page Mode are ignored while RACHing
                    MSG_GERAN_MED_0_G("Page Mode change ignored");
                    break;
                  }

                  case RR_IMSG_L1_IDLE_MODE_CHANGE_REQUIRED:
                  {
                    MSG_GERAN_HIGH_0_G("L1 idle mode will be changed after CS release");
                    break;
                  } /* RR_IMSG_L1_IDLE_MODE_CHANGE_REQUIRED */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_CONNECTION_PENDING");
                  }
                }
                break;
              }

              default:
              {
                new_rr_event = rr_connection_establishment_control(
                  EV_INPUT_MESSAGE,
                  new_message_ptr,
                  gas_id
                );
              }
            }
          }
          else if (message_set == MS_RR_L1)
          {
            if ((message_id == (byte) MPH_SURROUND_MEAS_IND)     ||
                (message_id == (byte) MPH_SERVING_IDLE_MEAS_IND) ||
                (message_id == (byte) MPH_SERVING_MEAS_IND)      ||
                (message_id == (byte) MPH_SERVING_DED_MEAS_IND)
               )
            {
              /* ignore these three messages while performing connection establishment */
            }

            else if (message_id == (byte) MPH_SURROUND_UPDATE_FAILURE_IND)
            {
              mph_surround_update_failure_ind_T *su_fail_ptr = &(new_message_ptr->mph_surround_update_failure_ind);
              rr_handle_surround_update_failure_ind(su_fail_ptr, FALSE, FALSE, gas_id);
            }

            else if ((message_id == (byte) MPH_IMMED_ASSIGN_CNF)     ||
                     (message_id == (byte) MPH_CHANNEL_RELEASE_CNF))
            {
              /* forward the message to the connection establishment module */
              new_rr_event = rr_connection_establishment_control(
                EV_INPUT_MESSAGE,
                new_message_ptr,
                gas_id
              );
            }

            else if (message_id == (byte) MPH_BLOCK_QUALITY_IND)
            {
              /* forward the message to the connection establishment module */
              new_rr_event = rr_connection_establishment_control(
                EV_INPUT_MESSAGE,
                new_message_ptr,
                gas_id
              );
            }

#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
            else if (message_id == (byte) MPH_DTM_CHANNEL_RELEASE_IND)
            {
              new_rr_event = rr_gprs_edtm_control(
                EV_INPUT_MESSAGE,
                new_message_ptr,
                gas_id
              );
            }
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
            else
            {
              MSG_GERAN_ERROR_3_G(
                "Unexpected message=(%d,%d), State=%d",
                (int) message_set,
                (int) message_id,
                (int) rr_control_data_ptr->rr_state
              );
            }
          } /* MS_RR_L1 */
          else if ((message_set == MS_RR_L2) && (message_id == (byte) DL_UNIT_DATA_IND))
          {
            /* prepare the OTA message for parsing */
            if (rr_prepare_ota_msg_for_parsing(&(new_message_ptr->dl_unit_data_ind), gas_id))
            {
              /* forward it to the connection establishment control */
              new_rr_event = rr_connection_establishment_control(
                EV_INPUT_MESSAGE,
                new_message_ptr,
                gas_id
              );
            }
            else
            {
              /* message is bad, ignore it */
              MSG_GERAN_ERROR_0_G("Bad RR OTA Message");
            }
          }
          else if ((message_set == MS_RR_L2) &&
                   ((message_id == (byte) DL_RANDOM_ACCESS_CNF)  ||
                    (message_id == (byte) DL_ESTABLISH_CNF)      ||
                    (message_id == (byte) DL_RELEASE_IND)        ||
                    (message_id == (byte) MDL_ERROR_IND)
                   )
                  )
          {
            new_rr_event = rr_connection_establishment_control(
              EV_INPUT_MESSAGE,
              new_message_ptr,
              gas_id
            );
          }
          else if (message_set == MS_MM_RR)
          {
            if (message_id == (byte) RR_ABORT_REQ)
            {
              /*****************************************************************************/
              /* MM is requesting RR to abort the currect connection establishment request */
              /*****************************************************************************/
              new_rr_event = rr_connection_establishment_control(
                EV_INPUT_MESSAGE,
                new_message_ptr,
                gas_id
              );

              /* Set the release cause to RELEASE_CAUSE_NAS_REQUEST so RR_ABORT_CNF is sent */
              /* Note: This must be done after RR_EV_MM_REQ_RECEIVED_ABORT is posted into */
              /* rr_connection_establishment_control() */
              rr_conn_set_release_cause(RELEASE_CAUSE_NAS_REQUEST, gas_id);
            }
            else if (message_id == (byte) RR_PLMN_SELECT_REQ)
            {
              /* Queue the request and handle it after conn est has been aborted */
              (void)rr_save_msg(new_message_ptr, gas_id);

              /* Abort the conn est */
              new_rr_event = rr_connection_establishment_control(
                RR_EV_MM_REQ_RECEIVED_ABORT,
                NULL,
                gas_id
              );
            }
            else if (message_id == (byte) RR_PLMN_LIST_REQ)
            {
              rr_plmn_list_req_T * rr_plmn_list_req = (rr_plmn_list_req_T *)new_message_ptr;

              /* Only abort the ongoing connection establishment if this a MANUAL PLMN search */
              /* and the connection establishment is for a Location Update */
              if ( ( rr_plmn_list_req->network_selection_mode != SYS_NETWORK_SELECTION_MODE_AUTOMATIC ) &&
                   ( rr_ce_access_reason(gas_id) == (byte)MM_RR_LOCATION_UPDATE ) )
              {
                /* Queue the request and handle it after conn est has been aborted */
                if (rr_save_msg(new_message_ptr, gas_id))
                {
                  /* Abort the conn est */
                  new_rr_event = rr_connection_establishment_control(
                    RR_EV_MM_REQ_RECEIVED_ABORT,
                    NULL,
                    gas_id
                  );
                }
              }
              else
              {
                MSG_GERAN_HIGH_0_G("Reject MM request while RR connected");
                rr_reject_msg(new_message_ptr, gas_id);
              }
            }
            else if (message_id == (byte) RR_STOP_GSM_MODE_REQ)
            {
              /* Queue the request and handle it after conn est has been aborted */
              (void)rr_save_msg(new_message_ptr, gas_id);

              /* Abort the conn est */
              new_rr_event = rr_connection_establishment_control(
                RR_EV_MM_REQ_RECEIVED_ABORT,
                NULL,
                gas_id
              );
            }
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
            else if (message_id == (byte) RR_DEACT_REQ)
            {
              /* RR_DEACT_REQ should only be passed through to this state if
              rr_x2g_cco_in_progress() returns TRUE when RR_DEACT_REQ is
              processed at the top of rr_control(). */

              /* Queue the request and handle it after conn est has been aborted */
              (void)rr_save_msg(new_message_ptr, gas_id);

              /* Abort the conn est */
              new_rr_event = rr_connection_establishment_control(
                RR_EV_MM_REQ_RECEIVED_ABORT,
                NULL,
                gas_id
              );
            }
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

            else if (message_id == (byte) RR_CHANGE_MODE_IND)
            {
              rr_pending_mode_change_data_store(
                &new_message_ptr->mm.rr_change_mode_ind,
                gas_id
              );
            }

            else if (message_id == (byte) RR_GMM_GPRS_STATE_CHANGE_REQ)
            {
              ;  // no action required
            }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q            
            else if (  message_id == (byte) RR_PSEUDO_LTE_LIST_UPDATE_IND)
            {
              ;  // no action required as going back to Idle GRR would cater for this
            }
#endif            
            else if (message_id == (byte) RR_EST_REQ)
            {
              if (rr_ce_conn_is_requested_by_nas(gas_id) == FALSE)
              {
#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
                (void) rr_gprs_edtm_prioritise_saved_msg(new_message_ptr, gas_id);
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
                /* Queue the request and handle it after conn est has been aborted */
                (void)rr_save_msg(new_message_ptr, gas_id);

                /* Abort the conn est */
                new_rr_event = rr_connection_establishment_control(
                  RR_EV_MM_REQ_RECEIVED_ABORT,
                  NULL,
                  gas_id
                );
              }
              else
              {
                MSG_GERAN_ERROR_0_G("RR_EST_REQ received - already establishing");
              }
            }

#ifdef FEATURE_LTE
            else if (message_id == (byte) RR_CSFB_BLIND_REDIR_TO_LTE_REQ)
            {

              boolean call_aborted  = new_message_ptr->mm.rr_csfb_blind_redir_to_lte_req.call_aborted;	

              boolean redir = rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(gas_id);
              boolean redir_after_csfb = rr_g2l_blind_redir_after_csfb_enabled(gas_id);

              MSG_GERAN_HIGH_2_G("G2L blind redir NV: after_voice_ss_or_periodic_lau=%d, after_CSFB=%d",
			  	redir, redir_after_csfb);
			  /*Set the call aborted flag received from NAS*/
              rr_connection_release_set_call_aborted_flag_during_csfb(call_aborted,gas_id);

              if (redir || redir_after_csfb)
              {
                rr_connection_release_set_g2l_blind_redir_after_csfb_flag(gas_id);
              }
            } /* RR_CSFB_BLIND_REDIR_TO_LTE_REQ */
#endif /* FEATURE_LTE */

            else
            {
              MSG_GERAN_ERROR_3_G(
                "Unexpected message=(%d,%d), State=%d",
                (int) message_set,
                (int) message_id,
                (int) rr_control_data_ptr->rr_state
              );
            }
          }
#ifdef FEATURE_CGPS_UMTS_CP_GSM
          else if ( message_set == MS_CGPS_RR )
          {
            switch ( message_id )
            {
              case RR_CGPS_EVENT_REQ:
              {
                rr_cgps_process_cgps_event_req(new_message_ptr, gas_id);
                break;
              }

              default:
              {
                MSG_GERAN_ERROR_0_G("Unknown CGPS->RR Inter-Task Message Received");
                break;
              }
            }
          }
#endif /* FEATURE_CGPS_UMTS_CP_GSM */
          else if ((message_set == MS_TIMER) && (message_id == MID_TIMER_EXPIRY))
          {
            switch (new_message_ptr->mid_timer_expiry.timer_id)
            {
              case T3126:
              {
#ifdef FEATURE_SGLTE
            /*Optimisation: When SGLTE mode is active and T3126 expires
              reject any pending X2G reselection.  This prevents PS domain being stuck on G whilst G
              tries to reselect*/
                (void)rr_sglte_reject_pending_x2g_req(FALSE, gas_id);
#endif /*FEATURE_SGLTE*/
                new_rr_event = rr_connection_establishment_control(EV_T3126_EXPIRY, NULL, gas_id);
                break;
              }

              case T3174:
              {
                new_rr_event = rr_connection_establishment_control(EV_T3174_EXPIRY, NULL, gas_id);
                break;
              }

              case T3122:
              {
                break;    // nothing to do
              }

              default:
              {
                LOG_UNEXPECTED_TIMER_EXPIRY(
                  new_message_ptr->mid_timer_expiry.timer_id,
                  grr_control_current_state_name(gas_id)
                );
              }
            }
          }

          else if (message_set == MS_MAC_RR)
          {
            switch (message_id)
            {
              case MAC_GRR_MAC_STATUS:
              {
#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
                new_rr_event = rr_gprs_edtm_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
                break;
              } /* MAC_GRR_MAC_STATUS */

              case MAC_GRR_MEAS_RPT_CNF:
              {
                /* MAC_GRR_MEAS_RPT_CNF may be received in this state if MAC
                was requested to send a meausurement report but it's
                transmission was interrupted by, for example, a CS
                establishment.  It is therefore appropriate to ignore this
                message. */
                break;
              } /* MAC_GRR_MEAS_RPT_CNF */

#ifdef FEATURE_GSM_DTM
              case MAC_GRR_UL_TBF_REQ:
              {
                /* No action necessary, GMAC has already been sent NO_PS_ACCESS */
                break;
              }
#endif /* FEATURE_GSM_DTM */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  new_message_ptr,
                  grr_control_current_state_name(gas_id)
                );
              }
            }
          } /* MS_MAC_RR */

#ifdef FEATURE_SGLTE
          else if (message_set == MS_MSGR_RR)
          {
            switch (new_message_ptr->rr_msgr_msg.message.hdr.id)
            {
#ifdef FEATURE_LTE
              case GERAN_GRR_LTE_PLMN_SRCH_REQ:
              {
                if (rr_is_sglte_mode_active(gas_id) &&
                    new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.plmn_srch_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
                {
                  rr_measured_signal_data_t * measured_signal_data = rr_get_measured_signal_data_ptr(gas_id);

                  rr_sglte_send_lte_plmn_srch_rsp_in_dedicated(
                    rr_scell_plmn_id(gas_id),
                    measured_signal_data->current_signal_strength,
                    rr_cell_supports_gprs(gas_id) ? SYS_SRV_CAPABILITY_CS_PS_SERVICE : SYS_SRV_CAPABILITY_CS_SERVICE_ONLY,
                    gas_id
                  );
                }
                else
                {
                  rr_sglte_reject_lte_plmn_srch_req(gas_id);
                }
                break;
              } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */
#endif /* FEATURE_LTE */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_CONNECTION_PENDING");
              }
            }
          } /* message_set == MS_MSGR_RR */

#ifdef FEATURE_GSM_TDS
          else if (message_set == MS_TDSRRC_RR)
          {
            switch (new_message_ptr->message_header.message_id)
            {
              case TDSRR_INTERRAT_PLMN_SRCH_REQ:
              {
                if (rr_is_sglte_mode_active(gas_id) &&
                    new_message_ptr->tdsrr_plmn_srch_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
                {
                  rr_measured_signal_data_t * measured_signal_data = rr_get_measured_signal_data_ptr(gas_id);

                  rr_sglte_send_tds_plmn_srch_rsp_in_dedicated(
                    rr_scell_plmn_id(gas_id),
                    measured_signal_data->current_signal_strength,
                    rr_cell_supports_gprs(gas_id) ? SYS_SRV_CAPABILITY_CS_PS_SERVICE : SYS_SRV_CAPABILITY_CS_SERVICE_ONLY,
                    gas_id
                  );
                }
                else
                {
                  rr_sglte_reject_tds_plmn_srch_req(gas_id);
                }
                break;
              } /* TDSRR_INTERRAT_PLMN_SRCH_REQ */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_CONNECTION_PENDING");
              }
            }
          } /* message_set == MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_SGLTE */

          else
          {
            LOG_UNEXPECTED_MESSAGE(
              new_message_ptr,
              grr_control_current_state_name(gas_id)
            );
          }
        } // EV_INPUT_MESSAGE
        else if ((rr_event == EV_FORCE_CELL_RESELECTION) ||
                 (rr_event == RR_EV_START_RESELECTION_POWER_SCAN))
        {

          rr_pending_mode_change_data_action(gas_id);
          rr_pending_mode_change_data_clear(gas_id);

          /* Ensure sys info updating is allowed again - this may have been */
          /* previously blocked during RACHing (e.g. for re-establishment) */
          rr_gprs_sys_info_allow_all_updates(gas_id);

          /* clear any existing connection establishment state */
          (void) rr_connection_establishment_control(EV_RESET_SOFTWARE, NULL, gas_id);

          if (rr_log_reselection_trigger == RR_RESELECTION_TRIGGER_SERVING_BARRED)
          {
            /* Reselection triggered due to serving cell becoming barred */
            /* MM should be notified of connection establishment abort */
            rr_send_abort_ind(RANDOM_ACCESS_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
          }

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_DTM_RR_EST_FAIL, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */

          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);

#ifdef FEATURE_LTE
          if ((rr_connection_release_get_g2l_blind_redir_flag(gas_id) || 
               rr_connection_release_get_g2l_blind_redir_after_csfb_flag(gas_id)) && 
                 (rr_connection_release_get_call_aborted_flag_during_csfb(gas_id)  &&
                   rr_is_lte_enabled(gas_id)))
          {
            rr_chan_rel_cell_selection_indicator_t *csi = rr_get_cell_select_indicator_ptr(gas_id);

            csi->valid = TRUE;
            csi->type = LTE_TARGET;
            csi->target.lte_target.blind_redir = TRUE;

            MSG_GERAN_HIGH_0_G("G2L blind redir (resetting the flag)");
            rr_connection_release_reset_g2l_blind_redir_flag(gas_id);
            rr_connection_release_reset_g2l_blind_redir_after_csfb_flag(gas_id);
            rr_connection_release_reset_call_aborted_flag_during_csfb(gas_id);

            rr_ps_no_ps_access_del_all(gas_id);

            rr_g2x_redir_init(gas_id);
            rr_g2x_redir_send_imsg_redir_req(
              csi,
              gas_id
            );

            /* CSI has been sent to redirection state machine so is no
            longer required. */
            rr_init_cell_selection_indicator(gas_id);

            rr_control_data_ptr->rr_state = RR_G2X_REDIR;
          }
          else
#endif /* FEATURE_LTE */
          {
            /* trigger an immediate reselection, possibly including a power scan */
            new_rr_event = rr_event;

            rr_control_data_ptr->rr_state = RR_GRR;
          }
        }

        else if ((rr_event == EV_DSF_RESELECTION) || (rr_event == EV_DSF_FOR_DENIALS))
        {

          rr_pending_mode_change_data_action(gas_id);
          rr_pending_mode_change_data_clear(gas_id);

          /* clear any existing connection establishment state */
          (void) rr_connection_establishment_control(EV_RESET_SOFTWARE, NULL, gas_id);

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_DTM_RR_EST_FAIL, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */

          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);

#ifdef FEATURE_LTE          
          if (rr_connection_release_get_g2l_blind_redir_flag(gas_id) || 
              ((rr_connection_release_get_g2l_blind_redir_after_csfb_flag(gas_id)) && 
               (rr_connection_release_get_call_aborted_flag_during_csfb(gas_id))))
          {
            rr_chan_rel_cell_selection_indicator_t *csi = rr_get_cell_select_indicator_ptr(gas_id);
                    
            csi->valid = TRUE;
            csi->type = LTE_TARGET;
            csi->target.lte_target.blind_redir = TRUE;
                    
            MSG_GERAN_HIGH_0_G("G2L blind redir (resetting the flag)");
            rr_connection_release_reset_g2l_blind_redir_flag(gas_id);
            rr_connection_release_reset_g2l_blind_redir_after_csfb_flag(gas_id);
            rr_connection_release_reset_call_aborted_flag_during_csfb(gas_id);
                    
            rr_ps_no_ps_access_del_all(gas_id);
            rr_g2x_redir_init(gas_id);
            rr_g2x_redir_send_imsg_redir_req(
              csi,
              gas_id
            );
            rr_init_cell_selection_indicator(gas_id);
            rr_control_data_ptr->rr_state = RR_G2X_REDIR;
          }
          else
#endif /* FEATURE_LTE */
          {
           /* trigger an immediate reselection, possibly including a power scan */
            new_rr_event = rr_event;
            rr_control_data_ptr->rr_state = RR_GRR;
          }          
        }


        // L1 has entered Dedicated during Connection Establishment
        else if (rr_event == EV_RR_EST_L1_IN_DED)
        {
          gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);

          /* prepare for dedicated SIs */
          rr_ded_meas_reporting_l1_in_dedicated(gas_id);
          rr_clear_si_sacch(gas_id);
          rr_clear_dedicated_sys_info_cache(gas_id);

          /* Copy the existing BCCH BA list(s) to the old SACCH store.  These
          BCCH BA lists will be combined with the new BA lists received on
          the SACCH, e.g. if SI5ter is received first, this will be combined
          with SI2 and SI2bis before the BA list is sent to L1.  This allows
          L1 to remain synced to all neighbour cells during acquisition of the
          SACCH system information. */
          rr_update_old_si5_ba_lists(
            &scell->gsm.SI2_BA_list,
            &scell->gsm.SI2bis_BA_list,
            &scell->gsm.SI2ter_BA_list,
            gas_id
          );

          rr_set_status_of_old_cell_si5_ba_lists(TRUE, gas_id);

          /* reset NC measurement state machine - may result in MPH_STOP_NC_MEASUREMENTS_REQ */
          (void)rr_gprs_nc_measurement_control(EV_RESET_SOFTWARE, NULL, gas_id);
        }

        // L2 link setup has failed due to Contention Resolution failure
        // The connection establishment will be retried
        else if (rr_event == EV_RR_EST_L2_CONT_RES_FAIL)
        {
          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);
        }

        /* Connection Establishment has been successful */
        else if (rr_event == EV_RR_EST_CNF)
        {
#ifdef FEATURE_LTE
          boolean g2l_blind_redir_after_csfb_required = 
            rr_connection_release_get_g2l_blind_redir_after_csfb_flag(gas_id);
#endif /* FEATURE_LTE */

          /* Ensure sys info updating is allowed again - this may have been */
          /* previously blocked during RACHing (e.g. for re-establishment) */
          rr_gprs_sys_info_allow_all_updates(gas_id);

          rr_leave_idle_mode(RR_DEDICATED, gas_id);

          (void) rr_connection_release(EV_CONNECTION_EXISTS, NULL, gas_id);
          (void) rr_cell_reest_control(EV_CONNECTION_EXISTS, NULL, gas_id);
          (void) rr_resel_control(EV_RR_EST_IND, NULL, gas_id);

#ifdef FEATURE_LTE
          /* It is possible that RR_CSFB_BLIND_REDIR_TO_LTE_REQ is received before RR_EST_CNF,
           * in that case set the varaible to again TRUE */
          if (g2l_blind_redir_after_csfb_required)
          {
            rr_connection_release_set_g2l_blind_redir_after_csfb_flag(gas_id);
          }

          if (rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(gas_id))
          {
            boolean cs_is_for_voice_or_periodic_lau = FALSE;

            if (rr_ce_access_reason(gas_id) == MM_RR_MO_FULL_SPEECH ||
                rr_ce_access_reason(gas_id) == MM_RR_MO_HALF_SPEECH ||
                rr_ce_access_reason(gas_id) == MM_RR_SUPPLEMENTARY_SERVICE ||
                rr_ce_access_reason(gas_id) == MM_RR_CALL_REEST)
            {
              cs_is_for_voice_or_periodic_lau = TRUE;
            }
            else if (rr_ce_access_reason(gas_id) == MM_RR_LOCATION_UPDATE)
            {
              if (rr_ce_is_periodic_lau(gas_id))
              {
                cs_is_for_voice_or_periodic_lau = TRUE;
              }
            }
            MSG_GERAN_MED_1_G(
              "CS for Voice / Periodic LAU: %d, G2L blind redir after Voice / SS / Periodic LAU enabled: 1",
              cs_is_for_voice_or_periodic_lau
            );

            if (cs_is_for_voice_or_periodic_lau)
            {
              rr_connection_release_set_g2l_blind_redir_flag(gas_id);
            }
          }
          else
          {
            MSG_GERAN_MED_0_G("G2L blind redir after Voice / SS / Periodic LAU not enabled");
          }
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
          // TODO DSDA
          rr_tds_pseudo_fr_reset_dedicated_info(gas_id);
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

          rr_control_data_ptr->rr_state = RR_DATA_TRANSFER;

#ifdef FEATURE_GSM_DTM
          rr_dtm_send_imsg_cs_conn_ind(gas_id);
#endif /* FEATURE_GSM_DTM */

#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM_TDS) || defined(FEATURE_LTE)
          if (rr_x2g_cco_in_progress(gas_id))
          {
            (void)rr_timer_cancel(T3174, gas_id);

            switch (rr_control_data_ptr->x2g_resel_type)
            {
#ifdef FEATURE_WCDMA
              case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
              {
                rr_send_interrat_cc_order_cnf(CCO_SUCCESS, RR_GSM_CCO_OTHER_REASON, gas_id);

#ifdef FEATURE_LTE
                rr_dedicated_priorities_take_ownership(gas_id);
#endif /* FEATURE_LTE */

                /* WCDMA RRC does not confirm completion of W2G CCO, so we
                just confirm here to allow IRAT measurements to procede. */
                rr_send_imsg(RR_IMSG_X2G_CCO_COMPLETE_IND, gas_id);
                break;
              }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
              // TODO DSDA
              case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
              {
                rr_send_tds_interrat_cc_order_cnf(
                  TDSCCO_SUCCESS,
                  TDSRR_GSM_CCO_OTHER_REASON,
                  gas_id
                );

#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
                rr_dedicated_priorities_take_ownership(gas_id);
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/

                rr_control_data_ptr->waiting_for_tdsrr_interrat_tl1_stop_ind = TRUE;
                break;
              }
#endif /*FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
              // TODO DSDA
              case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
              {
                geran_grr_lte_cco_rsp_t lte_cco_rsp;
                errno_enum_type         result;

                memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

                msgr_init_hdr(
                  &lte_cco_rsp.hdr,
                  MSGR_GERAN_GRR,
                  GERAN_GRR_LTE_CCO_RSP
                );
                lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_SUCCESS;
                result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
                if (result != E_SUCCESS)
                {
                  MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                }
                rr_control_data_ptr->waiting_for_lte_rrc_cco_nacc_completed_ind = TRUE;
                break;
              }
#endif /* FEATURE_LTE */

              default:
              {
                MSG_GERAN_ERROR_1_G(
                  "Unexpected x2g_resel_type: %d",
                  rr_control_data_ptr->x2g_resel_type
                );
              }
            }

            MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
            rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
            rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
          }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_SGLTE
          if (rr_is_sglte_mode_active(gas_id))
          {
            if (rr_control_data_ptr->pending_mode_change_data.valid)
            {
              /* CS is connected.  RR_CHANGE_MODE_IND may change service
              domain and/or RAT priority list. */
              MSG_GERAN_HIGH_0_G("Processing pending Mode Change Ind when entering data transfer state");
              rr_data_transfer_handle_rr_change_mode_ind(
                &rr_control_data_ptr->pending_mode_change_data.rat_pri_list_info,
                 rr_control_data_ptr->pending_mode_change_data.service_domain,
                 gas_id
              );

              rr_pending_mode_change_data_clear(gas_id);
            }
          }
#endif /* FEATURE_SGLTE */
        }
        else if (rr_event == EV_GO_TO_IDLE)
        {
          /* Ensure sys info updating is allowed again - this may have been */ /*
          previously blocked during RACHing (e.g. for re-establishment) */
          rr_gprs_sys_info_allow_all_updates(gas_id);

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_DTM_RR_EST_FAIL, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */

          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);

          rr_control_data_ptr->rr_state = RR_GRR;
          new_rr_event = RR_EV_CAMPED_ON_CELL;
        }

#ifdef FEATURE_SGLTE
        else if (rr_event == EV_SGLTE_X2G_CCO_FAILED_GO_TO_IDLE)
        {
          /* Ensure sys info updating is allowed again - this may have been */ /*
          previously blocked during RACHing (e.g. for re-establishment) */
          rr_gprs_sys_info_allow_all_updates(gas_id);

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control( EV_DTM_RR_EST_FAIL, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */

          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);

          rr_control_data_ptr->rr_state = RR_GRR;
          new_rr_event = EV_SGLTE_X2G_CCO_FAILED_GO_TO_IDLE;
        }
#endif /* FEATURE_SGLTE */
        else if (rr_event == EV_NO_BCCH_FOUND)
        {
          gprs_scell_info_t         * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
          rr_control_data_ptr->rr_state = RR_NO_CHANNELS;
          /* Inform CB task that CBCH is no longer valid */
          rr_send_cell_change_ind(
            scell_info_ptr->gsm.location_area_identification,
            scell_info_ptr->gsm.cell_identity,
            FALSE,
            gas_id
          );

          rr_send_cb_info(FALSE, gas_id);

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_DTM_RR_EST_FAIL, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */

          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);
        }

        else if ( rr_event == EV_CONNECTION_ABORTION)
        {
          /* Ensure sys info updating is allowed again - this may have been */
          /* previously blocked during RACHing (e.g. for re-establishment) */
          rr_gprs_sys_info_allow_all_updates(gas_id);

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_DTM_RR_EST_FAIL, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */

          /* simulate an abort and move to release state */
          memset(&rr_rr_abort_req, 0, sizeof(rr_rr_abort_req));
          rr_rr_abort_req.message_header.message_set = MS_MM_RR;
          rr_rr_abort_req.message_header.message_id  = (byte) RR_ABORT_REQ;
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
          rr_rr_abort_req.as_id = RR_GAS_ID_TO_AS_ID;
#endif // FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM
          new_message_ptr = (rr_cmd_bdy_type *) &rr_rr_abort_req;
          new_rr_event = EV_INPUT_MESSAGE;

          geran_eng_mode_data_write(ENG_MODE_GSM_CIPHER_INFO,NULL, FALSE, gas_id); /*cipher mode invalid, connection released*/

          rr_control_transition_to_state(RR_CONNECTION_RELEASE, gas_id);
        }

        else if ( rr_event == RR_EV_PROCESS_SAVED_MESSAGE )
        {
          /* Ensure sys info updating is allowed again - this may have been */
          /* previously blocked during RACHing (e.g. for re-establishment) */
          rr_gprs_sys_info_allow_all_updates(gas_id);

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_DTM_RR_EST_FAIL, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */

          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);

          if (grr_verify_saved_mm_message(RR_PLMN_LIST_REQ, gas_id))
          {
            MSG_GERAN_HIGH_0_G(
              "Reporting no service to NAS, expect RR_PLMN_SELECT_REQ after PLMN list search."
            );

            rr_send_no_service_ind(gas_id);
          }

          rr_control_data_ptr->rr_state = RR_NO_CHANNELS;
          new_rr_event = rr_process_saved_message(&new_message_ptr, gas_id);
        }
        else if ( rr_event == RR_EV_MEASUREMENT_PARAMS_UPDATED )
        {
          /* can be caused by Si2ter, Si2q or Si13 arriving on AGCH */
          MSG_GERAN_LOW_0_G("Measurement parameters updated during conn. est.");
        }
        /* RACH failure occurred - trigger panic reset */
        else if ( rr_event == EV_RACH_FOR_CS_FAILURE )
        {
          if (geran_get_nv_recovery_restart_enabled(gas_id))
          {
            MSG_GERAN_HIGH_0_G("RACH failure occured");
            geran_initiate_recovery_restart(GERAN_CLIENT_GRR, gas_id);
          }
          else
          {
            ERR_GERAN_FATAL_0_G("RACH Failure occured");
          }
        }

        /* Panic Reset triggered - cleanup conn est */
        else if ( rr_event == EV_PANIC_RESET )
        {
          /* Set up the release cause to something fairly general. */
          rr_conn_set_release_cause(RELEASE_CAUSE_CONN_EST_FAILURE, gas_id);

          /* Send RR_ABORT_IND to MM; No RR_SERVICE_IND */
          rr_conn_inform_mm_of_channel_release(FALSE, gas_id);

          (void) rr_connection_establishment_control(EV_RESET_SOFTWARE, NULL, gas_id);

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_RESET_SOFTWARE, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */
        }

        else if ( rr_event == EV_PCCO_FAILED )
        {
          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_DTM_RR_EST_FAIL, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */

          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);

          rr_control_data_ptr->rr_state = RR_GRR;
          new_rr_event = EV_PCCO_FAILED;
        }

        else if (rr_event == RR_EV_CANDIDATE_CELL_SI_RECEIVED)
        {
          /* ignore this event */
        }

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(rr_event),
                               grr_control_current_state_name(gas_id));
        }

      } /* CONNECTION_PENDING */
      break;


      /***********************************************************************/
      /* RR_CELL_REESTABLISH                                                 */
      /*                                                                     */
      /* Re-establishment after Radio Link Failure in progress.              */
      /***********************************************************************/
      case RR_CELL_REESTABLISH:
      {
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (new_message_ptr->rr.header.rr_message_set)
            {
              case RR_IMSG:
              {
                switch (new_message_ptr->rr.header.imh.message_id)
                {
                  case RR_IMSG_MSC_CHANGE_IND:
                  {
                    /*
                    Update NAS as required
                    RR may not be camped, so a RR_SERVICE_IND should not be sent to MM.
                    Either a RR_SERVICE_IND_FROM_OTA_MSG will be sent during the CS connection, or a
                    RR_SERVICE_IND will be sent when RR/L1 enter Idle
                    */
                    rr_msc_update(
                      FALSE,   // can_send_service_ind
                      gas_id   // gas_id
                    );
                    break;
                  }

                  case RR_IMSG_PS_ACCESS_REQ:
                  {
                    // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                    rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  case RR_IMSG_NO_PS_ACCESS_REQ:
                  {
                    // No PS access request (could be TLLI unassignment, CS-only)

                    rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  default:
                  {
                    new_rr_event = rr_cell_reest_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  }
                }
                break;
              }

              default:
              {
                new_rr_event = rr_cell_reest_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
              }
            }
          }
          else if (message_set == MS_RR_L1)
          {
            if ((message_id == (byte) MPH_DECODE_BCCH_LIST_FAILURE_IND)     ||
                (message_id == (byte) MPH_DECODE_BCCH_LIST_CNF)             ||
                (message_id == (byte) MPH_SELECT_SPECIFIC_BCCH_CNF)         ||
                (message_id == (byte) MPH_POWER_SCAN_CNF)
               )
            {
              new_rr_event = rr_cell_reest_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }

            else if ((message_id == (byte) MPH_BLOCK_QUALITY_IND)          ||
                     (message_id == (byte) MPH_SERVING_IDLE_MEAS_IND)      ||
                     (message_id == (byte) MPH_SURROUND_MEAS_IND)          ||
                     (message_id == (byte) MPH_SURROUND_WCDMA_IDLE_MEAS_IND)
                  || (message_id == (byte) MPH_SERVING_AUX_MEAS_IND)
                    )
            {
              ;  /* Received in Early Camping - ignore here */
            }

            else if (message_id == (byte) MPH_SURROUND_UPDATE_FAILURE_IND)
            {
              /* can happen under certain race conditions, just ignore it */
            }

            else if (message_id == (byte) MPH_FCCH_SCH_DECODE_IND)
            {
              /* No action necessary. */
            }

            else
            {
              MSG_GERAN_ERROR_3_G(
                "Unexpected message=(%d,%d), State=%d",
                (int) message_set,
                (int) message_id,
                (int) rr_control_data_ptr->rr_state
              );
            }
          }
          else if (message_set == MS_MM_RR)
          {
            if ((message_id == (byte) RR_EST_REQ) || (message_id == (byte) RR_ABORT_REQ))
            {
              (void) rr_cell_reest_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }

            /*
            RR_PLMN_LIST_REQ
             A HPLMN search request could be received durig re-establishment.
             A manual PLMN LIST REQ is not expected during re-establishment.
             Reject the request whatever the mode.
            */
            else if (message_id == (byte) RR_PLMN_LIST_REQ)
            {
              rr_plmn_list_req_T  * rr_plmn_list_req = &new_message_ptr->rr_plmn_list_req;

              if ( rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC )
              {
                MSG_GERAN_HIGH_0_G("Rejecting HPLMN search during reest");
                rr_reject_msg(new_message_ptr, gas_id);
              }
              else
              {
                MSG_GERAN_HIGH_0_G("Reest aborted for RR_PLMN_LIST_REQ");

                new_rr_event = rr_cell_reest_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);

                /* save the message for later processing */
                (void)rr_save_msg(new_message_ptr, gas_id);
              }
            }

            /*
            RR_PLMN_SELECT_REQ
             Not expected during re-establishment - but process anyway
            */
            else if (message_id == (byte) RR_PLMN_SELECT_REQ)
            {
              MSG_GERAN_HIGH_0_G("Reest aborted for RR_PLMN_SELECT_REQ");

              new_rr_event = rr_cell_reest_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);

              /* save the message for later processing */
              (void)rr_save_msg(new_message_ptr, gas_id);
            }

            /*
            RR_STOP_GSM_MODE_REQ
            */
            else if (message_id == (byte) RR_STOP_GSM_MODE_REQ)
            {
              MSG_GERAN_HIGH_0_G("Reest aborted for RR_STOP_GSM_MODE_REQ");

              new_rr_event = rr_cell_reest_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);

              /* save the message for later processing */
              (void)rr_save_msg(new_message_ptr, gas_id);
            }

            /*
            RR_DEACT_REQ
            */
            else if (message_id == (byte) RR_DEACT_REQ)
            {
              MSG_GERAN_HIGH_0_G("Reest aborted for RR_DEACT_REQ");

              new_rr_event = rr_cell_reest_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);

              /* save the message for later processing */
              (void)rr_save_msg(new_message_ptr, gas_id);
              set_rr_deact_reason(RR_DEACT_REASON_DEACT_REQ, gas_id);
            }

            /*
            RR_GMM_GPRS_STATE_CHANGE_REQ
            */
            else if (message_id == (byte) RR_GMM_GPRS_STATE_CHANGE_REQ)
            {
              ;  // no action required
            }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q            
            else if (message_id == (byte) RR_PSEUDO_LTE_LIST_UPDATE_IND)
            {
              ;  // no action required, GRR would query NAS DB going to IDLE
            }
#endif
            else
            {
              MSG_GERAN_ERROR_3_G(
                "Unexpected message=(%d,%d), State=%d",
                (int) message_set,
                (int) message_id,
                (int) rr_control_data_ptr->rr_state
              );
            }
          } /* MS_MM_RR */

          else if ((message_set == MS_TIMER) && (message_id == (byte) MID_TIMER_EXPIRY))
          {
            switch (new_message_ptr->mid_timer_expiry.timer_id)
            {
              case RR_CELL_SELECTION_ABORT_TIMER:
              case RR_GSM_SYS_INFO_TIMER:
              case RR_BCCH_DECODE_COMPLETE_TIMER:
              {
                (void) rr_cell_reest_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                break;
              }

              case T3122:
              {
                // This is unexpected - normally no CS connection would be attempted if T3122 is running.
                // The exception is an Emergency Call, which could lead to a re-establishment attempt.
                // No need to notify NAS-MM at this point - it will be notified when the next RR_SERVICE_IND
                // is sent when returning to Idle when the CS connection is released
                MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
                break;
              }

              default:
              {
                LOG_UNEXPECTED_TIMER_EXPIRY(
                  new_message_ptr->mid_timer_expiry.timer_id,
                  rr_main_state_name(rr_control_data_ptr->rr_state)
                );
              }
            }
          }
          else
          {
            MSG_GERAN_ERROR_3_G(
              "Unexpected message=(%d,%d), State=%d",
              (int) message_set,
              (int) message_id,
              (int) rr_control_data_ptr->rr_state
            );
          }
        } /* EV_INPUT_MESSAGE */

        else if ( (rr_event == RR_EV_CANDIDATE_CELL_SI_RECEIVED) ||
                  (rr_event == RR_EV_GSM_SI_RECEIVED))
        {
          new_rr_event = rr_cell_reest_control(rr_event, NULL, gas_id);
        }

        /*
        EV_NO_BCCH_FOUND
          Reestablishment has failed to find a suitable cell, or the re-establishment was
        aborted.
        */
        else if (rr_event == EV_NO_BCCH_FOUND)
        {
          /* If there is a saved MM message, process it now appropriately for a */
          /* condition of no service */
          if (grr_examine_saved_mm_message(NULL, gas_id))
          {
            new_rr_event = RR_EV_PROCESS_SAVED_MESSAGE;
          }
          else
          {
            /* Perform normal cell selection to regain normal service */
            (void) rr_selection_control(EV_TRIGGER_CELL_SELECTION, NULL, gas_id);
            rr_control_data_ptr->rr_state = RR_CELL_SELECTION;
          }
        }

        /*
        RR_EV_CAMPED_ON_CELL
          A suitable cell has been camped onto. Now CS establishment can be attempted
        */
        else if (rr_event == RR_EV_CAMPED_ON_CELL)
        {
          rr_cmd_bdy_type *message_ptr;

          rr_event = rr_examine_saved_message(&message_ptr, gas_id);

          /* Look for the expected saved RR_EST_REQ for re-establishment */
          if ( (rr_event == EV_INPUT_MESSAGE) &&
               (message_ptr != NULL) &&
               (message_ptr->message_header.message_set == MS_MM_RR) &&
               (message_ptr->message_header.message_id == (byte) RR_EST_REQ) &&
               (message_ptr->rr_est_req.establishment_cause == (byte) MM_RR_CALL_REEST) )
          {
            MSG_GERAN_MED_0_G("Proceeding with CS establishment");

            /* This returns EV_RR_EST_REQ (can be ignored) */
            (void) rr_connection_establishment_control( EV_INPUT_MESSAGE, message_ptr, gas_id);

            /* The RR_EST_REQ has been processed, so we must clear it from the
            saved message store. */
            grr_throwaway_saved_message(gas_id);

            /* Block sys info updating during RACHing for re-establishment */
            /* (because there is incomplete sys info at this point) */
            rr_gprs_sys_info_block_all_updates(gas_id);

            rr_control_data_ptr->rr_state = RR_CONNECTION_PENDING;
          }
        }

        else if ((rr_event == EV_CELL_CHOSEN) ||
                 (rr_event == EV_CELL_CHOSEN_EARLY_CAMPED))
        {
          // RR-CELL-REESTABLISH has abandoned re-establishment, but has found a cell suitable
          // for camping onto. This cell has been selected

          gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

          RR_NULL_CHECK_FATAL(pcell_info_ptr);

          // Reset the SI event generation flag as SI acquistion will continue in RR-CONTROL
          pcell_info_ptr->gsm.generate_si_received_event = TRUE;

          if (rr_event == EV_CELL_CHOSEN_EARLY_CAMPED)
          {
            /* Start RR_GSM_SYS_INFO_TIMER to guard the sys info acquisition */
           (void)rr_timer_start(RR_GSM_SYS_INFO_TIMER, RR_GSM_SYS_INFO_TIMEOUT, gas_id);
          }
          else
          {
            rr_l1_idle_send_imsg_start_early_camp_req(
              pcell_info_ptr->gsm.BCCH_ARFCN,
              pcell_info_ptr->gsm.BSIC,
              pcell_info_ptr->gsm.location_area_identification,
              pcell_info_ptr->gsm.cell_identity,
              pcell_info_ptr->gsm.control_channel_description,
              pcell_info_ptr->gsm.control_channel_description_valid,
              gas_id
            );
          }

          rr_control_data_ptr->rr_state = RR_EARLY_CAMPED_WAIT_FOR_SI;
        }

#ifdef FEATURE_LTE
        else if (rr_event == EV_G2L_BLIND_REDIR_REQUIRED)
        {
          /* If there is a saved MM message, process it now */
          if (grr_examine_saved_mm_message(NULL, gas_id))
          {
            MSG_GERAN_HIGH_0_G("Process saved message instead of G2L blind redir.)");
            new_rr_event = RR_EV_PROCESS_SAVED_MESSAGE;
          }
          else
          {
            /* Perform G2L blind redirection */
            rr_chan_rel_cell_selection_indicator_t *csi = rr_get_cell_select_indicator_ptr(gas_id);
  
            csi->valid = TRUE;
            csi->type = LTE_TARGET;
            csi->target.lte_target.blind_redir = TRUE;
  
            rr_ps_no_ps_access_del_all(gas_id);
            rr_g2x_redir_init(gas_id);
            rr_g2x_redir_send_imsg_redir_req(csi, gas_id);  
            rr_init_cell_selection_indicator(gas_id);
            rr_control_data_ptr->rr_state = RR_G2X_REDIR;
          }    
          
          /* Clear blind redir flags */
          rr_connection_release_reset_g2l_blind_redir_flag(gas_id);
          rr_connection_release_reset_g2l_blind_redir_after_csfb_flag(gas_id);
       }
#endif

        else if (rr_event == RR_EV_PROCESS_SAVED_MESSAGE)
        {
          rr_event_T  saved_event = rr_process_saved_message(&new_message_ptr, gas_id);

          /* Passing in EV_INPUT_MESSAGE will setup message_set & message_id using */
          /* the contents of new_message_ptr. If new_message_ptr, message_set or */
          /* message_id are NULL, then EV_NO_EVENT will be returned, otherwise */
          /* EV_INPUT_MESSAGE is returned */
          rr_event = rr_extract_message_header_details(
            saved_event,
            new_message_ptr,
            &message_set,
            &message_id
          );

          if ( (rr_event == EV_INPUT_MESSAGE) &&
               (new_message_ptr != NULL) )
          {
            /******************************************************************/
            /* MM->RR: RR_EST_REQ                                             */
            /******************************************************************/
            if ( (message_set == MS_MM_RR) && (message_id == (byte) RR_EST_REQ) )
            {
              /* Inform MM that (re-)establishment is not possible */
              rr_send_abort_ind(RR_CONNECTION_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
              new_rr_event = EV_NO_BCCH_FOUND;
            }

            else
            /******************************************************************/
            /* MM->RR: RR_PLMN_LIST_REQ                                       */
            /******************************************************************/
            if ( (message_set == MS_MM_RR) && (message_id == (byte) RR_PLMN_LIST_REQ) )
            {
              rr_event_T plc_event;

              rr_start_plmn_list_search((rr_plmn_list_req_T *)new_message_ptr, TRUE, gas_id);

              plc_event = rr_plmn_list_control(EV_PLMN_SEARCH_START, NULL, gas_id);

              if (EV_PLMN_SEARCH_COMPLETE == plc_event)
              {
                // No search required - perform normal cell selection to regain normal service
                (void) rr_selection_control(EV_TRIGGER_CELL_SELECTION, NULL, gas_id);
                rr_control_data_ptr->rr_state = RR_CELL_SELECTION;
              }
              else
              {
                // Ensure NAS brings RR back into service after the search completes - indicate NO SERVICE now
                rr_send_no_service_ind(gas_id);
                rr_control_data_ptr->rr_state = RR_PLMN_LIST_SEARCH;
              }
            }

            else
            /******************************************************************/
            /* MM->RR: RR_PLMN_SELECT_REQ                                     */
            /******************************************************************/
            if ( (message_set == MS_MM_RR) && (message_id == (byte) RR_PLMN_SELECT_REQ) )
            {
              MSG_GERAN_ERROR_0_G("TODO: Handle RR_PLMN_SELECT_REQ properly");

              rr_send_plmn_select_cnf(
                MM_AS_SUCCESS,
                MM_AS_NO_SERVICE,
                NULL,                 /* LAI not valid    */
                NULL,
                NULL,                 /*BA list not valid */
                NULL,                 /*SI DB             */
                gas_id
              );

              /* Perform normal cell selection to regain normal service */
              (void) rr_selection_control(EV_TRIGGER_CELL_SELECTION, NULL, gas_id);
              rr_control_data_ptr->rr_state = RR_CELL_SELECTION;
            }

            else
            /******************************************************************/
            /* MM->RR: RR_STOP_GSM_MODE_REQ                                   */
            /******************************************************************/
            if ( (message_set == MS_MM_RR) && (message_id == (byte) RR_STOP_GSM_MODE_REQ) )
            {
              rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
              new_rr_event = rr_process_rr_stop_gsm_mode_req(TRUE, gas_id);
            }

            else
            /******************************************************************/
            /* MM->RR: RR_DEACT_REQ                                           */
            /******************************************************************/
            if ( (message_set == MS_MM_RR) && (message_id == (byte) RR_DEACT_REQ) )
            {
              rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
              new_rr_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
            }
          }

          if (!grr_examine_saved_mm_message(NULL, gas_id))
          {
            /*
            If a panic reset occurs after this point, then there is specific handling
            in the case of panic reset, which sends an appropriate message to MM
            before restarting RR. Therefore, there is no need to keep the MM message
            saved past this point
            */
            rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);
          }
        }

        /* Panic Reset triggered */
        else if (rr_event == EV_PANIC_RESET)
        {
          /* Reset cell selection ready for restart */
          (void) rr_cell_reest_control(EV_RESET_SOFTWARE, NULL, gas_id);
        }
        else if (rr_event == RR_EV_EARLY_CAMP_SI_RECEIVED)
        {
          /* This can be ignored because we already given early camp ind*/
        }
        else
        {
          MSG_GERAN_ERROR_2_G(
            "UnexEV. Ev(%d), State(%d)",
            (int) rr_event,
            (int) rr_control_data_ptr->rr_state
          );
        }

        break;
      } /* RR_CELL_REESTABLISH state */


      /***********************************************************************/
      /* RR_DATA_TRANSFER                                                    */
      /*                                                                     */
      /* A CS connection has been setup. L1 is in DEDICATED.                 */
      /***********************************************************************/
      case RR_DATA_TRANSFER:
      {
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          switch (message_set)
          {
            case MS_RR_RR:
            {
              switch (new_message_ptr->rr.header.rr_message_set)
              {
                case RR_IMSG:
                {
                  switch (new_message_ptr->rr.rr.imsg.header.imh.message_id)
                  {
                    case RR_IMSG_PS_ACCESS_REQ:
                    {
                     /* FR - 19574  Pass this message to Our Reselection Control,where, it will send a request to MAC, via PS ACCESS FSM*/
#ifdef FEATURE_SGLTE
#ifdef  FEATURE_SGLTE_G2X_CONN_MODE_RESEL

                       /* FR - 19574 , first check our feature is active or NOT ?, if active, pass the message to connected mode Resel FSM */
                      if ( rr_is_sglte_mode_active(gas_id) && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
                      {
                        /* Inject it to Connected Mode Reselection FSM */
                        rr_event = rr_sglte_rr_conn_g2x_resel_control(rr_event, new_message_ptr, gas_id );
                      }
#endif
#endif
                      if (rr_event == EV_INPUT_MESSAGE)
                      {
                        rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

#ifdef FEATURE_GSM_DTM
                        if (imsg->purge_gmm_signalling)
                        {
                          (void) rr_dtm_control(
                            EV_DTM_PS_ACCESS_REQ_PURGE,
                            (void *)imsg->confirm_cb_fn,
                            gas_id
                          );
                        }
                        else
                        {
                          (void) rr_dtm_control(
                            EV_DTM_PS_ACCESS_REQ,
                            (void *)imsg->confirm_cb_fn,
                            gas_id
                          );
                        }
#else
                          // Call the call-back function (if one was supplied)
                          CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
#endif // FEATURE_GSM_DTM
                      }
                      break;
                    } /* RR_IMSG_PS_ACCESS_REQ */

                    case RR_IMSG_NO_PS_ACCESS_REQ:
                    {
                      rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

#ifdef FEATURE_GSM_DTM
                      if (imsg->del_all_pdus)
                      {
                        (void) rr_dtm_control(
                          EV_DTM_NO_PS_ACCESS_REQ_DEL_ALL,
                          (void *)imsg->confirm_cb_fn,
                          gas_id
                        );
                      }
                      else
                      {
                      (void) rr_dtm_control(
                        EV_DTM_NO_PS_ACCESS_REQ,
                        (void *)imsg->confirm_cb_fn,
                        gas_id
                      );
                      }
#else
                      // Call the call-back function (if one was supplied)
                      CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
#endif // FEATURE_GSM_DTM
                      break;
                    } /* RR_IMSG_NO_PS_ACCESS_REQ */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
                    case RR_IMSG_X2G_CCO_COMPLETE_IND:
                    {
                      gprs_scell_info_t * scell_info_ptr = rr_gprs_get_scell_info(gas_id);

                      if (rr_control_data_ptr->x2g_resel_type != RR_X2G_RESEL_TYPE_UNDEFINED)
                      {
                        MSG_GERAN_ERROR_1_G(
                          "Clearing x2g_resel_type (%d)",
                          rr_control_data_ptr->x2g_resel_type
                        );
                        rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                      }
                      if (rr_control_data_ptr->x2g_resel_abort_cause != RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED)
                      {
                        MSG_GERAN_ERROR_1_G(
                          "Clearing x2g_resel_abort_cause (%d)",
                          rr_control_data_ptr->x2g_resel_abort_cause
                        );
                        rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                      }
                      if (rr_control_data_ptr->x2g_cco_status != RR_CCO_STATUS_UNDEFINED)
                      {
                        MSG_GERAN_ERROR_1_G(
                          "Clearing x2g_cco_status (%d)",
                          rr_control_data_ptr->x2g_cco_status
                        );
                        rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
                      }
                      if (rr_control_data_ptr->is_cco_for_csfb != FALSE)
                      {
                        MSG_GERAN_ERROR_1_G(
                          "Clearing is_cco_for_csfb (%d)",
                          rr_control_data_ptr->is_cco_for_csfb
                        );
                        rr_control_data_ptr->is_cco_for_csfb = FALSE;
                      }
                      if (rr_control_data_ptr->x2g_cco_pending_network_control_order_valid != FALSE)
                      {
                        rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
                        rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;
                      }

                      rr_set_panic_reset_x2g_cco_flag(RR_X2G_RESEL_TYPE_UNDEFINED, gas_id);

#ifdef FEATURE_SGLTE
                      /* Activate the pending RAT priority list. */
                      if (rr_is_sglte_mode_active(gas_id) && rr_control_data_ptr->sglte_pending_rat_pri_list_valid)
                      {
                         /* CS is connected.  RR_CHANGE_MODE_IND may change service
                        domain and/or RAT priority list. */
                        rr_data_transfer_handle_rr_change_mode_ind(
                          &rr_control_data_ptr->sglte_pending_rat_pri_list,
                           SYS_SRV_DOMAIN_CS_PS, gas_id
                        );
                        rr_sglte_set_lte_blind_redir_in_data_transfer(gas_id);
                        rr_control_data_ptr->sglte_pending_rat_pri_list_valid = FALSE;
                      }
                      else
#endif /* FEATURE_SGLTE */
                      {
                      /* Now that the other RAT is shut down fully, it is time to
                      enable sleep in GL1. */
                      rr_l1_send_mph_enable_sleep_ind(gas_id);

                      if (scell_info_ptr->gsm.wcdma_ded_neighbor_list.valid_data)
                      {
                        rr_send_mph_dedicated_wcdma_cell_update_list_req(
                          NULL,                                           // wcdma_idle_neighbor_list_ptr
                          &scell_info_ptr->gsm.wcdma_ded_neighbor_list,   // wcdma_ded_neighbor_list_ptr
                          scell_info_ptr->gsm.dedicated_mode_params,
                          gas_id
                        );
                      }
                      else if (scell_info_ptr->gsm.utran_neighbor_list.valid_data == TRUE)
                      {
                        rr_send_mph_dedicated_wcdma_cell_update_list_req(
                          &scell_info_ptr->gsm.utran_neighbor_list,   // wcdma_idle_neighbor_list_ptr
                          NULL,                                       // wcdma_ded_neighbor_list_ptr
                          scell_info_ptr->gsm.dedicated_mode_params,
                          gas_id
                        );
                      }
                      else
                      {
                        rr_send_mph_dedicated_wcdma_cell_update_list_req(
                          NULL,     // wcdma_idle_neighbor_list_ptr
                          NULL,     // wcdma_ded_neighbor_list_ptr
                          scell_info_ptr->gsm.dedicated_mode_params,
                          gas_id
                        );
                      }
             }
                      break;
                    } /* RR_IMSG_X2G_CCO_COMPLETE_IND */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

                    case RR_IMSG_MSC_CHANGE_IND:
                    {
                      (void) rr_dtm_control(EV_DTM_MSC_CHANGE_IND, NULL, gas_id);
                      break;
                    }

                    case RR_IMSG_PAGE_IND:
                    {
                      rr_imsg_page_ind_t *imsg = &new_message_ptr->rr.rr.imsg.page_ind;

                      // A PS page could be received via a Paging Notification message (see RR-DTM)
                      if (imsg->page_info.domain == RR_PAGE_DOMAIN_PS)
                      {
                        rr_ce_send_page_to_nas(
                          &imsg->page_info,   // page_info_ptr
                          gas_id
                        );
                      }
                      break;
                    } /* RR_IMSG_PAGE_IND */

#ifdef FEATURE_SGLTE
                    case RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ:
                    {
                      MSG_GERAN_HIGH_0_G("Received RR_STOP_GSM_MODE_REQ(RR_SGLTE_RAT_CHANGE) in RR_DATA_TRANSFER");

#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
                      /* FR - 19574 , first check our feature is active or NOT ?, if active, pass the message to connected mode Resel FSM */
                      if ( rr_is_sglte_mode_active(gas_id) && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
                      {
                        MSG_GERAN_HIGH_0_G("RR_STOP_GSM_MODE_REQ(RR_SGLTE_RAT_CHANGE) in RR_DATA_TRANSFER");
                        /* Inject it to Connected Mode Reselection FSM */
                         rr_event = rr_sglte_rr_conn_g2x_resel_control(rr_event, new_message_ptr, gas_id );
                      }
                      else
                      {
#endif
                        LOG_UNEXPECTED_MESSAGE(
                          new_message_ptr,
                          rr_main_state_name(rr_control_data_ptr->rr_state)
                          );

#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
                      }
#endif
                      break;
                    } /* RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ */

                    case RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_USER_GSM_ONLY_REQ:
                    {
                      boolean rr_event_saved = FALSE;

#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
                      /* FR - 19574, first check feature is active or NOT ?, if active, pass the message to connected mode Resel FSM */
                      if ( rr_is_sglte_mode_active(gas_id) && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
                      {
                        MSG_GERAN_HIGH_0_G("RR_STOP_GSM_MODE_REQ(RR_SGLTE_USER_RAT_CHANGE) in RR_DATA_TRANSFER");
                        /* Inject it to Connected Mode Reselection FSM */
                        new_rr_event = rr_sglte_rr_conn_g2x_resel_control(EV_G2X_CONN_RESEL_ABORT_REQ, new_message_ptr, gas_id );
                        if ( new_rr_event != EV_G2X_CONN_RESEL_ABORT_CNF )
                        {  /* Wait for Reselection to Abort */
                          rr_event_saved = TRUE;
                          (void)rr_save_msg(new_message_ptr, gas_id);
                        }
                      }
#endif
                      if ( rr_event_saved == FALSE )
                      {
                        /* First, switch L1 to GSM-only mode.  Once this is done
                        then remove PS support from service domain. */
                        rr_sglte_set_rat_priority_list_gsm_only(gas_id);
                        rr_send_mph_ue_mode_change_req(TRUE, gas_id);
                        rr_control_data_ptr->switch_to_cs_only_gsm_only_pending = TRUE;
                        new_rr_event = EV_NO_EVENT;
                      }

                      break;
                    }

#endif /* FEATURE_SGLTE */

                    default:
                    {
                      LOG_UNEXPECTED_MESSAGE(
                        new_message_ptr,
                        rr_main_state_name(rr_control_data_ptr->rr_state)
                      );
                    }
                  }
                  break;
                } /* RR_IMSG */

                case RR_CE_IMSG:
                {
                  rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  break;
                }

#ifdef FEATURE_GSM_DTM
                case RR_PS_IMSG:
                case RR_PS_OMSG:
                {
                 /* FR - 19574  */
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
                   /* FR - 19574 , first check our feature is active or NOT ?, if active, pass the message to connected mode Resel FSM */
                   if ( rr_is_sglte_mode_active(gas_id) && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
                   {
                     /* Inject it to Connected Mode Reselection FSM */
                       rr_event = rr_sglte_rr_conn_g2x_resel_control(rr_event, new_message_ptr, gas_id );
                   }
 #endif
                  (void)rr_dtm_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  break;
                }

                case RR_DTM_OMSG:
                {
                  new_rr_event = rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  break;
                }
#endif // FEATURE_GSM_DTM

#ifdef FEATURE_SGLTE
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
               case RR_SGLTE_G2X_RESEL_CONN_OMSG:
               {
                 switch ( new_message_ptr->rr.rr.imsg.header.imh.message_id)
                 {
                   case RR_SGLTE_G2X_RESEL_CONN_OMSG_ABORT_CNF:
                   {
                     new_rr_event = rr_process_saved_message(&new_message_ptr, gas_id);
                     break;
                   }

                default:
                   {
                   LOG_UNEXPECTED_MESSAGE(
                    new_message_ptr,
                    rr_main_state_name(rr_control_data_ptr->rr_state)
                   );
                   break;
                 }
               }
               }
               break;
#endif
#endif
                default:
                {
                  LOG_UNEXPECTED_MESSAGE(
                    new_message_ptr,
                    rr_main_state_name(rr_control_data_ptr->rr_state)
                  );
                }
              }
              break;
            }

            case MS_RR_L1:
            {
              switch ((rr_l1_message_id_enum_T) message_id)
              {
                case MPH_CHANNEL_ASSIGN_CNF:
                case MPH_HANDOVER_IND:
                case MPH_HANDOVER_CNF:
                case MPH_STOP_HANDOVER_ACCESS_CNF:
                case MPH_CHANNEL_MODE_MODIFY_CNF:
                case MPH_RECONNECT_CHANNEL_CNF:
                case MPH_SET_CIPHERING_CNF:
                case MPH_FREQUENCY_REDEFINITION_IND:
#ifdef FEATURE_GSM_DTM
                case MPH_DTM_CHANNEL_ASSIGN_CNF:
                case MPH_DTM_CHANNEL_RELEASE_IND:
#endif /* #ifdef FEATURE_GSM_DTM */
                {
                  new_rr_event = rr_handle_procedures(
                    EV_INPUT_MESSAGE,
                    new_message_ptr,
                    gas_id
                  );
                  break;
                }

                case MPH_SERVING_DED_MEAS_IND:
                {
                  rr_process_mph_serving_ded_meas_ind(
                    &new_message_ptr->mph_serving_ded_meas_ind,
                    gas_id
                  );
                  break;
                }

                case MPH_SURROUND_MEAS_IND:
                {
                  rr_store_surround_meas(
                    &new_message_ptr->mph_surround_meas_ind.best_six_info,
                    TRUE,
                    gas_id
                  );
                  break;
                }

#ifdef FEATURE_WCDMA

                case MPH_SURROUND_WCDMA_DEDICATED_MEAS_IND:
                {
                  rr_process_mph_surround_wcdma_dedicated_meas_ind(
                    &new_message_ptr->mph_surround_wcdma_dedicated_meas_ind,
                    gas_id
                  );
                  break;
                }

#endif /* #ifdef FEATURE_WCDMA */

#if defined (FEATURE_LTE) || defined (FEATURE_GSM_TDS)
                case MPH_SURROUND_IRAT_MEAS_IND:
                {
                  rr_irat_process_surround_irat_meas_ind_in_dedicated(
                    &new_message_ptr->mph.surround_irat_meas_ind,
                    gas_id
                  );

#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
                 if ( rr_is_sglte_mode_active(gas_id) && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
                 {
                     if ( rr_sel_get_service_domain(gas_id) == SYS_SRV_DOMAIN_CS_PS )
                     {
                       /* First invoke G2W function to save the measurement results */
                       rr_irat_process_surround_irat_meas_ind(&new_message_ptr->mph.surround_irat_meas_ind, gas_id);
                       rr_sglte_rr_conn_g2x_resel_control(EV_IRAT_SURROUND_MEAS_IND, new_message_ptr, gas_id );
                     }
                 }
#endif

                  break;
                } /* MPH_SURROUND_IRAT_MEAS_IND */
#endif /* FEATURE_LTE || FEATURE_GSM_TDS */

                case MPH_BLOCK_QUALITY_IND:
                {
                  new_rr_event = rr_check_rlf_in_dedicated(
                    new_message_ptr->mph_block_quality_ind.block_quality,
                    gas_id
                  );
                  break;
                }

#ifdef FEATURE_SGLTE
                case MPH_UE_MODE_CHANGE_CNF:
                {
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
                  /* FR - 19574 , first check our feature is active or NOT ?, if active, pass the message to connected mode Resel FSM */
                  if ( rr_is_sglte_mode_active(gas_id) && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
                  {
                     /* Inject MODE CHANGE CONFIRM to Connected Mode Reselection FSM */
                     rr_event = rr_sglte_rr_conn_g2x_resel_control(rr_event, new_message_ptr, gas_id );
                  }
#endif /* FEATURE_SGLTE_G2X_CONN_MODE_RESEL */

                   if ( rr_event != EV_NO_EVENT )
                   {
                     if (rr_control_data_ptr->switch_to_cs_only_gsm_only_pending)
                     {
                       rr_control_data_ptr->switch_to_cs_only_gsm_only_pending = FALSE;

                       MSG_GERAN_HIGH_0_G("L1 confirmed GSM-only, deactivating PS");
                      rr_ps_sglte_handle_ps_removal(rr_sglte_confirm_rat_change_cb_fn, gas_id);
                     }
                   }
                  break;
                } /* MPH_UE_MODE_CHANGE_CNF */
#endif /* FEATURE_SGLTE */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_RR_L1 */

            case MS_RR_L2:
            {
              switch ((rr_l2_message_id_enum_T) message_id)
              {
                case DL_SUSPEND_CNF:
                case DL_RESUME_CNF:
                case DL_RECONNECT_CNF:
                case MDL_ERROR_IND:
                case DL_RANDOM_ACCESS_CNF:
                {
                  new_rr_event = rr_handle_procedures(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  break;
                }

                case DL_RELEASE_IND:
                {
                  /* FR - 19574  */
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
                  if ( rr_is_sglte_mode_active(gas_id) && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
                  {
                    new_rr_event = rr_sglte_rr_conn_g2x_resel_control(EV_G2X_CONN_RESEL_ABORT_REQ, new_message_ptr, gas_id );
                    if ( new_rr_event != EV_G2X_CONN_RESEL_ABORT_CNF )
                    {  /* Wait for Reselection to Abort */
                      (void)rr_save_msg(new_message_ptr, gas_id);
                    }
                    else
                    {
                      new_rr_event = rr_handle_procedures(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                    }
                  }
                  else
                  {
                    new_rr_event = rr_handle_procedures(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  }
#else
                  new_rr_event = rr_handle_procedures(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
#endif

                  break;
                }

                case DL_DATA_IND:
                {
                  RR_cause_T error_type;
                  /* No need to rr_prepare_ota_msg_for_parsing_in_dedicated for the
                  INTERSYSTEM_TO_UTRAN_HANDOVER_COMMAND message, since this
                  message is not decoded by RR */
                  if (
                      (new_message_ptr->dl_data_ind.layer3_message[1] == INTERSYSTEM_TO_UTRAN_HANDOVER_COMMAND) &&
                      (
                       (new_message_ptr->dl_data_ind.l2_channel_type == DCCH) ||
                       (new_message_ptr->dl_data_ind.l2_channel_type == SDCCH)
                      )
                     )
                  {
                    rr_log_gsm_message_received(
                      new_message_ptr->dl_data_ind.layer3_message[1],
                      RADIO_RESSOURCES,
                      new_message_ptr->dl_data_ind.l2_channel_type,
                      gas_id
                    );

                    if (rr_gsm_only(gas_id))
                    {
                      MSG_GERAN_MED_0_G("Rx UTRAN_HO_CMD: UE in GSM only mode");

                      new_rr_event = EV_NO_EVENT;
                    }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS)
                    else if ((FALSE == rr_is_rat_segment_loaded(RR_SEG_LOAD_WCDMA)) ||
                             (FALSE == rr_is_wcdma_enabled(gas_id)))
                    {
                      MSG_GERAN_MED_0_G("Rx UTRAN_HO_CMD: WCDMA not supported");
                      send_handover_failure_ota(RR_PROTOCOL_ERROR_UNSPECIFIED, gas_id);
                      new_rr_event = EV_NO_EVENT;
                    }
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */
                    else
                    {
                      /* Forward this message to rr_handle_procedures */
                      new_rr_event = rr_handle_procedures(EV_INPUT_MESSAGE, new_message_ptr, gas_id);

                      rr_control_data_ptr->rr_state = RR_G2W_INTERRAT_HANDOVER_PROGRESS;
                    }
                  }
                  /* prepare the OTA message for parsing */
                  else
                  {
                    if (rr_prepare_ota_msg_for_parsing_in_dedicated(
                          &new_message_ptr->dl_data_ind,
                          &error_type,
                          gas_id
                        )
                       )
                    {
                      byte message_type = new_message_ptr->dl_data_ind.layer3_message[1];

                      switch (message_type)
                      {
                        case CHANNEL_RELEASE:
                        case PARTIAL_RELEASE:
                        {
                          /* FR - 19574  */
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
                          if ( rr_is_sglte_mode_active(gas_id) && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
                          {
                             new_rr_event = rr_sglte_rr_conn_g2x_resel_control(EV_G2X_CONN_RESEL_ABORT_REQ, new_message_ptr, gas_id );
                             if ( new_rr_event != EV_G2X_CONN_RESEL_ABORT_CNF )
                             {  /* Wait for Reselection to Abort */
                               (void)rr_save_msg(new_message_ptr, gas_id);
                             }
                             else
                             {
                               new_rr_event = rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                             }
                          }
                          else
                          {
                            new_rr_event = rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                          }
#else
                          new_rr_event = rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
#endif
                          break;
                        }

#ifdef FEATURE_CGPS_UMTS_CP_GSM

                        case APPLICATION_INFORMATION:
                        {
                          byte layer3_message[2];

                          rr_cgps_process_apdu_data_seg(new_message_ptr, gas_id);

                          layer3_message[0] = RADIO_RESSOURCES;
                          layer3_message[1] = APPLICATION_INFORMATION;

                          // Send an indication to MM that APPLICATION INFORMATION data has been received
                          rr_send_data_ind(
                             layer3_message,          // layer3_message
                             sizeof(layer3_message),  // message_length
                             gas_id
                           );

                          new_rr_event = EV_NO_EVENT;
                          break;
                        }

#endif /* FEATURE_CGPS_UMTS_CP_GSM */

                        default:
                        {
                          new_rr_event = rr_handle_procedures(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                        }
                      } /* switch (message_type) */
                    }
                    else /* the OTA message is not correct */
                    {
                      MSG_GERAN_ERROR_0_G("Bad RR OTA Message");
                      /* send a RR_STATUS message depending on the error */
                      if (error_type == RR_MESSAGE_TYPE_NON_EXISTENT)
                      {
                        MSG_GERAN_HIGH_0_G("OMsg(NW,Status(MessageNotExists)");
                        rr_send_status(RR_MESSAGE_TYPE_NON_EXISTENT, gas_id);
                      }
                      else if (error_type == RR_INVALID_MANDATORY_INFORMATION)
                      {
                        MSG_GERAN_HIGH_0_G("OMsg(NW,Status(InvalidMandatoryInfo)");
                        rr_send_status(RR_INVALID_MANDATORY_INFORMATION, gas_id);
                      }
                    }
                  }
                  break;
                }

                case DL_UNIT_DATA_IND:
                {
                  dl_unit_data_ind_T *dl_unit_data_ind;
                  dl_unit_data_ind = &new_message_ptr->dl_unit_data_ind;

                  if (rr_prepare_ota_msg_for_parsing(dl_unit_data_ind, gas_id))
                  {
                    uint8 message_type = dl_unit_data_ind->layer3_message[1];

                    switch (message_type)
                    {
                      case PHYSICAL_INFORMATION:
                      {
                        new_rr_event = rr_handle_procedures(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                        break;
                      }

#ifdef FEATURE_GSM_DTM
                      case PACKET_NOTIFICATION:
                      {
                        new_rr_event = rr_handle_procedures(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                        break;
                      }
#endif // FEATURE_GSM_DTM

                      default:
                      {
                        MSG_GERAN_ERROR_0_G("Ignoring OTA DL_UNIT_DATA_IND in dedicated");
                      }
                    }
                  }
                  else
                  {
                    MSG_GERAN_ERROR_0_G("ProtocolError for UnitData");
                  }
                  break;
                }

                case DL_DATA_ACK_IND:
                {
                  rr_process_dl_data_ack_ind(&new_message_ptr->dl_data_ack_ind, gas_id);
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_RR_L2 */

            case MS_MM_RR:
            {
              switch ((mm_rr_message_id_enum_T) message_id)
              {
                case RR_ABORT_REQ:
                {
                  boolean handle_abort = TRUE;

                  /* FR - 19574  */
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
                  if ( rr_is_sglte_mode_active(gas_id) && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
                  {
                    new_rr_event = rr_sglte_rr_conn_g2x_resel_control(
                      EV_G2X_CONN_RESEL_ABORT_REQ, new_message_ptr, gas_id
                    );

                    if ( new_rr_event != EV_G2X_CONN_RESEL_ABORT_CNF )
                    {  /* Wait for Reselection to Abort */
                      handle_abort = FALSE;
                      (void)rr_save_msg(new_message_ptr, gas_id);
                     }
                   }
#endif
                  if ( handle_abort )
                  {
                    rr_abort_req_T *rr_abort_req = &new_message_ptr->rr_abort_req;

                    if (rr_abort_req->auth_failed_bar_cells)
                    {
                      uint32 barred_time_secs, T3212_secs;                     
                      gprs_scell_info_t         * scell_info_ptr = rr_gprs_get_scell_info(gas_id);

                      T3212_secs =
                        CONVERT_DECIHOURS_TO_SECONDS(
                          scell_info_ptr->gsm.control_channel_description.T3212_timeout );

                      if (T3212_secs == 0)
                      {
                        /* Default is 12 hours if T3212 is zero */
                        barred_time_secs = CONVERT_HOURS_TO_SECONDS(12);
                      }
                      else
                      if (T3212_secs > 60)
                      {
                        /* otherwise bar cell for T3302 minus 60 secs */
                        barred_time_secs = T3212_secs - 60;
                      }
                      else
                      {
                        /*
                        T3212 is less than 60 secs, so can't bar cell for T3212 minus 60 secs
                        Note: According to the coding of the T3212 value, it's minimum value
                        is 6 minutes, so this shouldn't be possible
                        */
                        barred_time_secs = 0;
                      }

                      if (barred_time_secs > 0)
                      {
                        MSG_GERAN_HIGH_2_G(
                        "Barring cell for %d secs (T3212=%d secs)",
                        barred_time_secs,
                        T3212_secs
                        );

                        rr_authentication_failed_bar_cell(
                        CONVERT_SECONDS_TO_MILLISECONDS(barred_time_secs),
                        gas_id
                      );
                      }
                      else
                      {
                        MSG_GERAN_HIGH_1_G(
                        "T3212 only %d secs - not long enough to bar cell",
                        T3212_secs
                      );
                      }
                  }
#ifdef FEATURE_LTE   
                  if (rr_abort_req->csmt_fail_depriotize_cells) 
                  { 

                    rr_add_gsm_cell_to_depriotized_list(rr_gprs_get_scell_arfcn(gas_id), gas_id); 
                  } 
#endif /*FEATURE_LTE*/

                  rr_log_gsm_call_drop(
                    RR_LOG_CALL_DROPPED_DUE_TO_UPPER_LAYER_TRIGGER,
                    rr_ce_access_reason(gas_id),
                    gas_id
                  );

                  /*
                  Now release CS connection. If the cell was barred (see above), then a new
                  cell will be chosen - this is handled in rr_conn_release.c
                  */
                    new_rr_event = rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                 } /* End of handle_abort */
                  break;
                }

#ifdef FEATURE_GSM_DTM

                case RR_GMM_GPRS_AUTH_FAIL_IND:
                {
                  rr_gmm_gprs_auth_fail_ind_T *rr_gmm_gprs_auth_fail_ind;
                  uint32                       barred_time_secs;
                  BSIC_T                      local_bsic;
                  gprs_scell_info_t         * scell_info_ptr = rr_gprs_get_scell_info(gas_id);
                  /*
                  Authentication and ciphering has failed. The serving cell should be
                  barred for the period indicated by T3302 minus 60 seconds.
                  If no T3302 value was included in previous ATTACH ACCEPT / RAU ACCEPT
                  messages, then a default of 12 hours is used (see 3GPP 43.022 3.5.5)
                  */
                  rr_gmm_gprs_auth_fail_ind = (rr_gmm_gprs_auth_fail_ind_T *)new_message_ptr;
                  CONVERT_TO_BSIC_T(local_bsic, rr_gmm_gprs_auth_fail_ind->bsic);

                  /* NAS sends the T3302 timer value as a number of MILLISECONDS, see mm_rr.h */
                  if (rr_gmm_gprs_auth_fail_ind->t3302_timer_value == 0)
                  {
                    /* Default is 12 hours if T3302 not supplied */
                    barred_time_secs = CONVERT_HOURS_TO_SECONDS(12);
                  }
                  else if (rr_gmm_gprs_auth_fail_ind->t3302_timer_value > CONVERT_SECONDS_TO_MILLISECONDS(60))
                  {
                    dword t3302_secs;

                    /* otherwise bar cell for T3302 minus 60 secs */
                    t3302_secs = (rr_gmm_gprs_auth_fail_ind->t3302_timer_value / 1000);

                    barred_time_secs = (uint32)(t3302_secs - 60);
                  }
                  else
                  {
                    /*
                    T3302 is less than 60 secs, so can't bar cell for T3302 minus 60 secs
                    Note: According to the coding of the T3302 value, it's minimum value
                    could be 2 secs
                    */
                    barred_time_secs = 0;
                  }

                  if (barred_time_secs > 0)
                  {
                    MSG_GERAN_HIGH_2_G(
                      "Barring cell for %d secs (T3302=%dms)",
                      barred_time_secs,
                      rr_gmm_gprs_auth_fail_ind->t3302_timer_value
                    );

                    if ((ARFCNS_EQUAL(rr_gmm_gprs_auth_fail_ind->arfcn, scell_info_ptr->gsm.BCCH_ARFCN)) &&
                        (SAME_BSIC(local_bsic, scell_info_ptr->gsm.BSIC)))
                    {
                      rr_authentication_failed_bar_cell(
                        CONVERT_SECONDS_TO_MILLISECONDS(barred_time_secs),
                        gas_id);

                      rr_log_gsm_call_drop(
                        RR_LOG_CALL_DROPPED_DUE_TO_UPPER_LAYER_TRIGGER,
                        rr_ce_access_reason(gas_id),
                        gas_id);

                      /*
                      Now release CS connection, after which a new cell should be chosen.
                      This is handled in rr_conn_release.c
                      */
                      new_rr_event = rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                    }
                    else
                    {
                      MSG_GERAN_HIGH_2_G("ARFCN mismatch Received ARFCN num,band=(%d %d)",
                        rr_gmm_gprs_auth_fail_ind->arfcn.num,
                        rr_gmm_gprs_auth_fail_ind->arfcn.band);

                      MSG_GERAN_HIGH_2_G("ARFCN mismatch Service ARFCN num,band=(%d %d)",
                        scell_info_ptr->gsm.BCCH_ARFCN.num,
                        scell_info_ptr->gsm.BCCH_ARFCN.band);

                      MSG_GERAN_HIGH_2_G("BSIC mismatch Received BSIC= %d Serving BSIC= %d ",
                        rr_gmm_gprs_auth_fail_ind->bsic,
                        CONVERT_BSIC_TO_BYTE(scell_info_ptr->gsm.BSIC));

                      rr_authn_fail_bar_cell_basedon_ARFCN_BSIC(
                        CONVERT_SECONDS_TO_MILLISECONDS(barred_time_secs),
                        rr_gmm_gprs_auth_fail_ind->arfcn,
                        rr_gmm_gprs_auth_fail_ind->bsic,
                        gas_id);
                    }
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("Cell barring timeout is zero, ignored");
                  }
                  break;
                }

                case RR_GMM_GPRS_STATE_CHANGE_REQ:
                {
                  break;  // no action required - could have performed GPRS Detach in DTM
                }

#endif /* #ifdef FEATURE_GSM_DTM */

                case RR_DATA_REQ:
                {
                  rr_process_rr_data_req(&(new_message_ptr->rr_data_req), gas_id);
                  break;
                }

                /* RR_PLMN_SELECT_REQ or RR_STOP_GSM_MODE_REQ will always abort
                a connection. */
                case RR_PLMN_SELECT_REQ:
                case RR_STOP_GSM_MODE_REQ:
                {
                  /* Queue the request and handle it after connection is
                  released. */
                  (void)rr_save_msg(new_message_ptr, gas_id);

                  rr_log_gsm_call_drop(
                    RR_LOG_CALL_DROPPED_DUE_TO_UPPER_LAYER_TRIGGER,
                    rr_ce_access_reason(gas_id),
                    gas_id
                  );

                  /* Abort the connection. */
                  /* This will return RR_EV_CONNECTION_RELEASE_IN_PROGRESS to
                  transition RR to RR_CONNECTION_RELEASE state, and the
                  connection will be released.  Then RR_EV_PROCESS_SAVED_MESSAGE
                  will be generated. */
                  new_rr_event = rr_connection_release(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);
                  break;
                }

                case RR_PLMN_LIST_REQ:
                {
                  rr_plmn_list_req_T * rr_plmn_list_req = (rr_plmn_list_req_T *)new_message_ptr;

                  /* Only abort the ongoing connection establishment if this a
                  MANUAL PLMN search and the connection establishment is for a
                  Location Update. */
                  if ((rr_plmn_list_req->network_selection_mode != SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
                      (rr_ce_access_reason(gas_id) == (byte)MM_RR_LOCATION_UPDATE))
                  {
                    /* Queue the request and handle it after connection is
                    released. */
                    (void)rr_save_msg(new_message_ptr, gas_id);

                    rr_log_gsm_call_drop(
                      RR_LOG_CALL_DROPPED_DUE_TO_UPPER_LAYER_TRIGGER,
                      rr_ce_access_reason(gas_id),
                      gas_id
                    );

                    /* Abort the connection. */
                    /* This will return RR_EV_CONNECTION_RELEASE_IN_PROGRESS to
                    transition RR to RR_CONNECTION_RELEASE state, and the
                    connection will be released.  Then RR_EV_PROCESS_SAVED_MESSAGE
                    will be generated. */
                    new_rr_event = rr_connection_release(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);
                  }
                  else
                  {
                    MSG_GERAN_HIGH_0_G("Reject MM request while RR connected");

                    /* MM expects a MM_AS_ABORTED status in all cases when
                    PLMN LIST REQ rejected/aborted */
                    rr_send_plmn_list_cnf(NULL, MM_AS_ABORTED, rr_plmn_list_req, gas_id);
                  }
                  break;
                }

                case RR_CHANGE_MODE_IND:
                {

#ifdef FEATURE_SGLTE
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    /* CS is connected.  RR_CHANGE_MODE_IND may change service
                    domain and/or RAT priority list. */
                    rr_data_transfer_handle_rr_change_mode_ind(
                      &new_message_ptr->mm.rr_change_mode_ind.rat_pri_list_info,
                       new_message_ptr->mm.rr_change_mode_ind.service_domain, gas_id
                    );
                  }
                  else
#endif /* FEATURE_SGLTE */
                  {
                    rr_pending_mode_change_data_store(&new_message_ptr->mm.rr_change_mode_ind, gas_id);
                  }
                  break;
                } /* RR_CHANGE_MODE_IND */

#ifdef FEATURE_LTE
                case RR_CSFB_BLIND_REDIR_TO_LTE_REQ:
                {
                  boolean call_aborted  = new_message_ptr->mm.rr_csfb_blind_redir_to_lte_req.call_aborted;
                  boolean redir = rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(gas_id);
                  boolean redir_after_csfb = rr_g2l_blind_redir_after_csfb_enabled(gas_id);

                  MSG_GERAN_HIGH_2_G("G2L blind redir NV: after_voice_ss_or_periodic_lau=%d, after_CSFB=%d",
                    redir, redir_after_csfb);

                  /*Set the call aborted flag received from NAS*/
                  rr_connection_release_set_call_aborted_flag_during_csfb(call_aborted,gas_id);
				  
                  if (redir || redir_after_csfb)
                  {
                    rr_connection_release_set_g2l_blind_redir_after_csfb_flag(gas_id);
                  }
                  /*Check if the connection was established for LAU */
                  if (MM_RR_LOCATION_UPDATE == rr_ce_access_reason(gas_id))
                  {
                    MSG_GERAN_HIGH_0_G("Connection etsbalished for LAU , now modified for speech");
                    rr_ce_set_access_reason(MM_RR_MO_FULL_SPEECH,gas_id);
                  }
                  break;
                } /* RR_CSFB_BLIND_REDIR_TO_LTE_REQ */
#endif /* FEATURE_LTE */

#ifdef FEATURE_SGLTE
                case RR_ACTIVATION_RSP:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    RR_ASSERT_WARN(rr_control_data_ptr->x2g_resel_type != RR_X2G_RESEL_TYPE_UNDEFINED);

                    switch (rr_control_data_ptr->x2g_resel_type)
                    {
#ifdef FEATURE_LTE
                      case RR_X2G_RESEL_TYPE_L2G_RESELECTION:
                      {
                        MSG_GERAN_HIGH_0_G("SGLTE L2G resel successful, enabling PS domain / Multi-RAT");
                        rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;                        
                        break;
                      } /* RR_X2G_RESEL_TYPE_L2G_RESELECTION */

                      case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                      {
                        geran_grr_lte_cco_rsp_t lte_cco_rsp;
                        errno_enum_type         result;

                        MSG_GERAN_HIGH_0_G("SGLTE L2G CCO successful, enabling PS domain / Multi-RAT");

                        grr_update_mm_with_rr_service_ind(gas_id);

                        memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

                        msgr_init_hdr(
                          &lte_cco_rsp.hdr,
                          MSGR_GERAN_GRR,
                          GERAN_GRR_LTE_CCO_RSP
                        );
                        lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_SUCCESS;
                        result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
                        if (result != E_SUCCESS)
                        {
                          MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                        }

                        rr_control_data_ptr->waiting_for_lte_rrc_cco_nacc_completed_ind = TRUE;
                        break;
                      } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER */

                      case RR_X2G_RESEL_TYPE_L2G_SGLTE_REDIRECTION:
                      {
                        MSG_GERAN_HIGH_0_G("SGLTE L2G redir successful, enabling PS domain / Multi-RAT");
                        rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                        break;
                      } /* RR_X2G_RESEL_TYPE_L2G_SGLTE_REDIRECTION */
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
                      case RR_X2G_RESEL_TYPE_T2G_RESELECTION:
                      {
                        MSG_GERAN_HIGH_0_G("SGLTE T2G resel successful, enabling PS domain / Multi-RAT");
                        rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                        break;
                      } /* RR_X2G_RESEL_TYPE_T2G_RESELECTION */

                      case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                      {
                        MSG_GERAN_HIGH_0_G("SGLTE T2G CCO successful, enabling PS domain / Multi-RAT");

                        grr_update_mm_with_rr_service_ind(gas_id);

                        rr_send_tds_interrat_cc_order_cnf(TDSCCO_SUCCESS, TDSRR_GSM_CCO_OTHER_REASON, gas_id);
                        rr_control_data_ptr->waiting_for_tdsrr_interrat_tl1_stop_ind = TRUE;
                        break;
                      } /* RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER */

                      case RR_X2G_RESEL_TYPE_T2G_SGLTE_REDIRECTION:
                      {
                        MSG_GERAN_HIGH_0_G("SGLTE T2G redir successful, enabling PS domain / Multi-RAT");
                        rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                        break;
                      } /* RR_X2G_RESEL_TYPE_T2G_RESELECTION */
#endif /*FEATURE_GSM_TDS*/


                      default:
                      {
                        MSG_GERAN_ERROR_1_G("Unexpected x2g_resel_type: %d", rr_control_data_ptr->x2g_resel_type);
                      }
                    }

                    /* If the procedure was not a CCO, x2g_resel_type should now be 'undefined'
                    and RR should modify service domain / etc. */
                    if (rr_control_data_ptr->x2g_resel_type == RR_X2G_RESEL_TYPE_UNDEFINED)
                    {

                      /*update sglte for dedicated pri info when G becomes acive rat */
                      rr_sglte_dedicated_priority_info_store(&rr_control_data_ptr->sglte_ded_pri_info, gas_id);

                      /* CS is connected.  RR_CHANGE_MODE_IND may change service
                      domain and/or RAT priority list. */
                      rr_data_transfer_handle_rr_change_mode_ind(
                        &rr_control_data_ptr->sglte_pending_rat_pri_list,
                         SYS_SRV_DOMAIN_CS_PS, gas_id
                      );
                      rr_sglte_set_lte_blind_redir_in_data_transfer(gas_id);

                    }
                  }
                  else
                  {
                    MSG_GERAN_ERROR_0_G("Unexpected RR_ACTIVATION_RSP in RR_DATA_TRANSFER, no SGLTE X2G in progress");
                  }
                  break;
                } /* RR_ACTIVATION_RSP */
#endif /* FEATURE_SGLTE */
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q
                case RR_PSEUDO_LTE_LIST_UPDATE_IND:
                {
                  // No action required as NAS DB queried after call release
                  break;
                }
#endif                
                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_MM_RR */

#ifdef FEATURE_CGPS_UMTS_CP_GSM

            case MS_CGPS_RR:
            {
              switch (message_id)
              {
                case RR_APDU_REQ:
                {
                  rr_cgps_process_rr_apdu_req(RR_APDU_ID_RRLP, new_message_ptr, gas_id);
                  break;
                }

                case RR_CGPS_EVENT_REQ:
                {
                  rr_cgps_process_cgps_event_req(new_message_ptr, gas_id);
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                  break;
                }
              }
              break;
            } /* MS_CGPS_RR */

#endif /* FEATURE_CGPS_UMTS_CP_GSM */

            case MS_TIMER:
            {
              switch (new_message_ptr->mid_timer_expiry.timer_id)
              {
                case T3124:
                {
                  new_rr_event = rr_handle_procedures(EV_T3124_TIMEOUT, NULL, gas_id);
                  break;
                }

                case T3122:
                {
                  // This is unexpected - normally no CS connection would be attempted if T3122 is running.
                  // The exception is an Emergency Call, which could lead to a CS connection established when
                  // T3122 expires.
                  // No need to notify NAS-MM at this point - it will be notified when the next RR_SERVICE_IND
                  // is sent when returning to Idle when the CS connection is released
                  MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_TIMER_EXPIRY(
                    new_message_ptr->mid_timer_expiry.timer_id,
                    rr_main_state_name(rr_control_data_ptr->rr_state)
                  );
                }
              }
              break;
            } /* MS_TIMER */

            case MS_GHDI_EXPORT:
            {
              switch (message_id)
              {
                case GHDI_CLASSMARK_CHANGED_IND:
                {
                  new_rr_event = rr_handle_procedures(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                         rr_main_state_name(rr_control_data_ptr->rr_state));
                }
              }
              break;
            } /* MS_GHDI_EXPORT */

#ifdef FEATURE_GSM_DTM

            case MS_RLC_RR:
            {
              /* All messages from RLC are passed through to DTM to be
              handled. */
              (void) rr_dtm_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
              break;
            } /* MS_RLC_RR */

            case MS_MAC_RR:
            {
              /* All messages from MAC are passed through to DTM via RR
              Procedure to be handled (because a MAC_GRR_DTM_ASSIGN_REJ will
              cancel the assignment process. */
              (void) rr_handle_procedures(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
              break;
            } /* MS_MAC_RR */

#endif /* #ifdef FEATURE_GSM_DTM */

#ifdef FEATURE_SGLTE
            case MS_MSGR_RR:
            {
              switch (new_message_ptr->rr_msgr_msg.message.hdr.id)
              {
#ifdef FEATURE_LTE
                case GERAN_GRR_LTE_RESEL_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    if (rr_sglte_is_full_service_available(gas_id))
                    {
                      rr_control_data_ptr->sglte_pending_rat_pri_list_valid = TRUE;
                      rr_control_data_ptr->sglte_pending_rat_pri_list =
                        new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_resel_req.rat_pri_list;

                      /*temporary  store SGLTE dedicated priority info rcvd in GERAN_GRR_LTE_RESEL_REQ*/
                      /*updated current active when RR becomes the active RAT                         */

                      rr_control_data_ptr->sglte_ded_pri_info.dedicated_priorities_valid = TRUE;

                      rr_control_data_ptr->sglte_ded_pri_info.dedicated_priority_info =
                        new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_resel_req.dedicated_priority_info;

                      rr_control_data_ptr->sglte_ded_pri_info.requested_PLMN =
                       new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_resel_req.requested_PLMN;

                      rr_send_activation_ind(MM_AS_SGLTE_INTER_RAT_RESELECTION, gas_id);
                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_L2G_RESELECTION;
                    }
                    else
                    {
                      rr_sglte_reject_lte_resel_req(gas_id);
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_DATA_TRANSFER");
                  }
                  break;
                } /* GERAN_GRR_LTE_RESEL_REQ */

                case GERAN_GRR_LTE_CCO_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    if (rr_sglte_is_full_service_available(gas_id))
                    {
                      rr_control_data_ptr->sglte_pending_rat_pri_list_valid = TRUE;
                      rr_control_data_ptr->sglte_pending_rat_pri_list =
                        new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_cco_req.rat_pri_list;

                      rr_send_activation_ind(MM_AS_LTOG_CCO, gas_id);
                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER;
                    }
                    else
                    {
                      rr_x2g_send_x2g_cco_rsp_rejection(RR_RAT_LTE, gas_id);
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_DATA_TRANSFER");
                  }
                  break;
                } /* GERAN_GRR_LTE_CCO_REQ */

                case GERAN_GRR_LTE_ABORT_CCO_REQ:
                {
                  rr_sglte_x2g_cco_aborted_cb_fn(gas_id);
                  break;
                } /* GERAN_GRR_LTE_ABORT_CCO_REQ */

                case GERAN_GRR_LTE_ABORT_RESEL_REQ:
                {
                  rr_sglte_x2g_resel_aborted_cb_fn(gas_id);
                  break;
                } /* GERAN_GRR_LTE_ABORT_RESEL_REQ */

                case GERAN_GRR_LTE_ABORT_REDIR_REQ:
                {
                  rr_sglte_x2g_redir_aborted_cb_fn(gas_id);
                  break;
                } /* GERAN_GRR_LTE_ABORT_REDIR_REQ */

                case GERAN_GRR_LTE_REDIR_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    if (rr_sglte_is_full_service_available(gas_id))
                    {
                      rr_control_data_ptr->sglte_pending_rat_pri_list_valid = TRUE;
                      rr_control_data_ptr->sglte_pending_rat_pri_list =
                        new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.lte_redir_req.rat_pri_list;

                      rr_send_activation_ind(MM_AS_SGLTE_INTER_RAT_REDIRECTION, gas_id);
                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_L2G_SGLTE_REDIRECTION;
                    }
                    else
                    {
                      rr_sglte_reject_lte_redir_req(gas_id);
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_DATA_TRANSFER");
                  }
                  break;
                } /* GERAN_GRR_LTE_REDIR_REQ */

                case GERAN_GRR_LTE_PLMN_SRCH_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id) &&
                      new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.plmn_srch_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
                  {
                    rr_measured_signal_data_t * measured_signal_data = rr_get_measured_signal_data_ptr(gas_id);

                    rr_sglte_send_lte_plmn_srch_rsp_in_dedicated(
                      rr_scell_plmn_id(gas_id),
                      measured_signal_data->current_signal_strength,
                      rr_cell_supports_gprs(gas_id) ? SYS_SRV_CAPABILITY_CS_PS_SERVICE : SYS_SRV_CAPABILITY_CS_SERVICE_ONLY,
                      gas_id
                    );
                  }
                  else
                  {
                    rr_sglte_reject_lte_plmn_srch_req(gas_id);
                  }
                  break;
                } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */

 #ifdef FEATURE_SGLTE
 #ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
                case LTE_RRC_G_RESEL_FAILED_RSP:
                case LTE_RRC_G_ABORT_RESEL_RSP:
                {
                  /* FR - 19574 , first check our feature is active or NOT ?, if active, pass the message to connected mode Resel FSM */
                  if ( rr_is_sglte_mode_active(gas_id) && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
                  {
                    /* Inject it to Connected Mode Reselection FSM */
                    rr_event = rr_sglte_rr_conn_g2x_resel_control(rr_event, new_message_ptr, gas_id );
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_DATA_TRANSFER");
                  }
                }
                break;
 #endif
 #endif
#endif /* FEATURE_LTE */

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_DATA_TRANSFER");
                }
              }
              break;
            } /* MS_MSGR_RR */

#ifdef FEATURE_GSM_TDS
            case MS_TDSRRC_RR:
            {
              switch (new_message_ptr->message_header.message_id)
              {
                case TDSRR_INTERRAT_RESELECTION_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    if (rr_sglte_is_full_service_available(gas_id))
                    {
                      rr_control_data_ptr->sglte_pending_rat_pri_list_valid = TRUE;
                      rr_control_data_ptr->sglte_pending_rat_pri_list =
                        ((tdsrr_interrat_reselect_req_type *) new_message_ptr)->rat_pri_list_info;

                      rr_send_activation_ind(MM_AS_SGLTE_INTER_RAT_RESELECTION, gas_id);
                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_T2G_RESELECTION;
                    }
                    else
                    {
                      rr_sglte_reject_tds_resel_req(gas_id);
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_DATA_TRANSFER");
                  }
                  break;
                } /* TDSRR_INTERRAT_RESELECTION_REQ */

                case TDSRR_INTERRAT_CC_ORDER_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    if (rr_sglte_is_full_service_available(gas_id))
                    {
                      rr_control_data_ptr->sglte_pending_rat_pri_list_valid = TRUE;
                      rr_control_data_ptr->sglte_pending_rat_pri_list =
                        ((tdsrr_interrat_cc_order_req_type *) new_message_ptr)->rat_pri_list_info;

                      rr_send_activation_ind(MM_AS_WTOG_INTERRAT_CC_ORDER, gas_id);
                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER;
                    }
                    else
                    {
                      rr_x2g_send_x2g_cco_rsp_rejection(RR_RAT_UTRAN_TDD, gas_id);
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_DATA_TRANSFER");
                  }
                  break;
                } /* TDSRR_INTERRAT_CC_ORDER_REQ */

                case TDSRR_INTERRAT_REDIRECT_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id))
                  {
                    if (rr_sglte_is_full_service_available(gas_id))
                    {
                      rr_control_data_ptr->sglte_pending_rat_pri_list_valid = TRUE;
                      rr_control_data_ptr->sglte_pending_rat_pri_list =
                        ((tdsrr_interrat_redirect_req_type *) new_message_ptr)->rat_pri_list_info;

                      rr_send_activation_ind(MM_AS_SGLTE_INTER_RAT_REDIRECTION, gas_id);
                      rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_T2G_SGLTE_REDIRECTION;
                    }
                    else
                    {
                      rr_sglte_reject_tds_redir_req(gas_id);
                    }
                  }
                  else
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_DATA_TRANSFER");
                  }
                  break;
                } /* TDSRR_INTERRAT_REDIRECT_REQ */

                case TDSRR_INTERRAT_PLMN_SRCH_REQ:
                {
                  if (rr_is_sglte_mode_active(gas_id) &&
                      new_message_ptr->tdsrr_plmn_srch_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
                  {
                    rr_measured_signal_data_t * measured_signal_data = rr_get_measured_signal_data_ptr(gas_id);

                    rr_sglte_send_tds_plmn_srch_rsp_in_dedicated(
                      rr_scell_plmn_id(gas_id),
                      measured_signal_data->current_signal_strength,
                      rr_cell_supports_gprs(gas_id) ? SYS_SRV_CAPABILITY_CS_PS_SERVICE : SYS_SRV_CAPABILITY_CS_SERVICE_ONLY,
                      gas_id
                    );
                  }
                  else
                  {
                    rr_sglte_reject_tds_plmn_srch_req(gas_id);
                  }
                  break;
                } /* TDSRR_INTERRAT_PLMN_SRCH_REQ */

 #ifdef FEATURE_SGLTE
 #ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
                case TDSRRC_INTERRAT_RESELECTION_REJ:
                case TDSRRC_INTERRAT_RESELECTION_ABORT_CNF:
                {
                    /* FR - 19574 , first check our feature is active or NOT ?, if active, pass the message to connected mode Resel FSM */
                    if ( rr_is_sglte_mode_active(gas_id) && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
                    {
                      /* Inject it to Connected Mode Reselection FSM */
                      rr_event = rr_sglte_rr_conn_g2x_resel_control(rr_event, new_message_ptr, gas_id );
                    }
                    else
                    {
                      LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_DATA_TRANSFER");
                    }
                }
                break;
 #endif
 #endif

                case TDSRR_INTERRAT_RESELECTION_ABORT_REQ:
                {
                  rr_sglte_x2g_resel_aborted_cb_fn(gas_id);
                  break;
                }

                case TDSRR_INTERRAT_CC_ORDER_ABORT_REQ:
                {
                  rr_sglte_x2g_cco_aborted_cb_fn(gas_id);
                  break;
                }
                
                case TDSRR_INTERRAT_REDIRECT_ABORT_REQ:
                {
                  rr_sglte_x2g_redir_aborted_cb_fn(gas_id);
                  break;
                }                   

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_DATA_TRANSFER");
                }
              }
              break;
            } /* MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_SGLTE */


            default:
            {
              LOG_UNEXPECTED_MESSAGE(new_message_ptr,
                                     rr_main_state_name(rr_control_data_ptr->rr_state));
            }
          }
        }
        else if (rr_event == EV_RADIO_LINK_FAILURE)
        {
          // Radio link failure detected, log it
          MSG_GERAN_HIGH_0_G("Radio Link Failure");

          /* FR - 19574 , Handle RLF, in SGLTE in speical way, i.e. Return Immediate Abort Confirm  */
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
          if ( rr_is_sglte_mode_active(gas_id) && rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
          {
            new_rr_event = rr_sglte_rr_conn_g2x_resel_control(EV_G2X_CONN_RESEL_ABORT_REQ, new_message_ptr, gas_id );
            if ( new_rr_event != EV_G2X_CONN_RESEL_ABORT_CNF )
            {  /* DO NOT Wait for Reselection to Abort */
               MSG_GERAN_ERROR_0_G("SGLTE Connected Mode resel could not be aborted");
            }
          }
#endif

          rr_log_link_failure(gas_id);

          rr_log_gsm_call_drop(
            RR_LOG_CALL_DROPPED_DUE_TO_RADIO_LINK_FAILURE,
            rr_ce_access_reason(gas_id),
            gas_id
          );

          new_rr_event = rr_connection_release(EV_RADIO_LINK_FAILURE, NULL, gas_id);
        }
        else if (rr_event == EV_PROCEDURE_COMPLETE)
        {
          /* If the connection was aborted during an RR procedure then connection
          release should now actually go ahead with the abort. */
          new_rr_event = rr_connection_release(EV_PROCEDURE_COMPLETE, NULL, gas_id);
        }
        else if (rr_event == RR_EV_GSM_SI_RECEIVED)
        {
          /* ignored in this state */
        }
        else if (rr_event == RR_EV_CONNECTION_RELEASE_IN_PROGRESS)
        {
          /* Reset the state machines that may be active after connection established */
          (void) rr_inter_cell_handover_control(EV_RESET_SOFTWARE, NULL, gas_id);
          (void) rr_intra_cell_handover_control(EV_RESET_SOFTWARE, NULL, gas_id);
          (void) rr_handle_procedures(EV_RESET_SOFTWARE, NULL, gas_id);
          rr_eng_mode_update_dtx_ul_status(FALSE, gas_id); /*Connection is being released default to DTX FALSE*/
          geran_eng_mode_data_write(ENG_MODE_GSM_CIPHER_INFO,NULL, FALSE, gas_id); /*cipher mode invalid, connection released*/

          rr_control_transition_to_state(RR_CONNECTION_RELEASE, gas_id);
        }

        /* Failure / Panic Reset occurred */
        else if (rr_event == EV_PANIC_RESET)
        {
          /* Send RR_ABORT_IND to MM; No RR_SERVICE_IND */
          rr_conn_inform_mm_of_channel_release(FALSE, gas_id);

          /* Reset the state machines that may be active during CS connection */
          (void) rr_connection_establishment_control(EV_RESET_SOFTWARE, NULL, gas_id);
          (void) rr_connection_release(EV_RESET_SOFTWARE, NULL, gas_id);
          (void) rr_inter_cell_handover_control(EV_RESET_SOFTWARE, NULL, gas_id);
          (void) rr_intra_cell_handover_control(EV_RESET_SOFTWARE, NULL, gas_id);
          (void) rr_handle_procedures(EV_RESET_SOFTWARE, NULL, gas_id);
          rr_reset_sapis_due_to_failure(gas_id);

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_RESET_SOFTWARE, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */
        }

#ifdef FEATURE_GSM_EDTM
        else if (rr_event == RR_EV_MEASUREMENT_PARAMS_UPDATED)
        {
          /* can be caused by Si13 arriving on PACCH during EDTM release */
          MSG_GERAN_LOW_0_G("Measurement parameters updated during DTM");
        }
#endif /* FEATURE_GSM_EDTM */

        else if (rr_event == EV_MAC_IDLE_MODE)
        {
          (void)rr_handle_procedures(EV_MAC_IDLE_MODE, NULL, gas_id);
        }

        else if (rr_event == EV_GPRS_SUSPENDED)
        {
          MSG_GERAN_MED_0_G("GPRS suspended in dedicated mode");
        }

#ifdef  FEATURE_SGLTE
#ifdef  FEATURE_SGLTE_G2X_CONN_MODE_RESEL
        else if ( rr_event == EV_CR_G2W_TIMER_EXPIRED)
        {
          rr_event = rr_sglte_rr_conn_g2x_resel_control(rr_event, new_message_ptr, gas_id );
        }
#endif
#endif


        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(rr_event),
                               rr_main_state_name(rr_control_data_ptr->rr_state));
        }
      } /* RR_DATA_TRANSFER */
      break;


      /***********************************************************************/
      /* RR_CONNECTION_RELEASE                                               */
      /*                                                                     */
      /* The CS connection is in the process of being released               */
      /***********************************************************************/
      case RR_CONNECTION_RELEASE:
      {
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (new_message_ptr->rr.header.rr_message_set)
            {
              case RR_IMSG:
              {
                switch (new_message_ptr->rr.rr.imsg.header.imh.message_id)
                {
                  case RR_IMSG_PS_ACCESS_REQ:
                  {
                    // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                    rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                    // Note if GMM signalling PDUs need to be deleted
                    rr_control_data_ptr->purge_gmm_signalling = imsg->purge_gmm_signalling;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  case RR_IMSG_NO_PS_ACCESS_REQ:
                  {
                    // No PS access request (could be TLLI unassignment, CS-only)

                    rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  case RR_IMSG_MSC_CHANGE_IND:
                  {
                    /*
                    Update NAS as required
                    RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                    This will be sent when RR/L1 enters Idle
                    */
                    rr_msc_update(
                      FALSE,   // can_send_service_ind
                      gas_id   // gas_id
                    );
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message_ptr,
                      rr_main_state_name(rr_control_data_ptr->rr_state)
                    );
                  }
                }
                break;
              } /* RR_IMSG */

              case RR_CE_IMSG:
              {
                rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                break;
              }

              case RR_CE_OMSG:
              {
                switch (new_message_ptr->rr.rr.imsg.header.imh.message_id)
                {
                  case RR_CE_OMSG_CS_RELEASE_IND:
                  {
                    rr_ce_omsg_cs_release_ind_t *imsg_ptr = &new_message_ptr->rr.ce.omsg.cs_release_ind;
                    gprs_scell_info_t         * scell_info_ptr = rr_gprs_get_scell_info(gas_id);

                    // Any measurement reporting data can be thrown away
                    rr_ded_meas_reporting_discard_data(gas_id);

                    switch (imsg_ptr->status)
                    {
                      case RR_CE_REL_STATUS_CAMP_ON_CELL:
                      {
                        boolean purge_gmm_signalling =
                          rr_control_data_ptr->purge_gmm_signalling || imsg_ptr->purge_gmm_signalling;

                        // Request GRR to camp back on the serving cell
                        rr_grr_send_imsg_camp_on_cell_req(
                          TRUE,                   // notify_mm_service_ind
                          FALSE,                  // notify_mm_plmn_select_cnf
                          purge_gmm_signalling,   // purge_gmm_signalling
                          gas_id                  // gas_id
                        );

                        rr_control_transition_to_state(RR_GRR, gas_id);
                        break;
                      }

                      case RR_CE_REL_STATUS_ACQUIRE_SI:
                      {
                        scell_info_ptr->gsm.camped_on_flag = FALSE;
                        rr_gprs_deactivate_timed_sys_info_refresh(gas_id);

                        switch (rr_gprs_get_scell_info(gas_id)->gsm.invalidate_si_reason)
                        {
                          case RR_SI_HANDOVER:
                            MSG_GERAN_HIGH_0_G("SI not valid: HANDOVER");
                            break;

                          case RR_SI_OUT_OF_DATE:
                            MSG_GERAN_HIGH_0_G("SI not valid: OUT OF DATE");
                            break;

                          case RR_SI_INCONSISTENT:
                            MSG_GERAN_HIGH_0_G("SI not valid: INCONSISTENT");
                            break;

                          case RR_SI_LAC_CHANGED:
                            MSG_GERAN_HIGH_0_G("SI not valid: LAC CHANGED");
                            break;

                          default:
                            MSG_GERAN_HIGH_2_G("SI not valid: (reason=%d)",
                                     (int)scell_info_ptr->gsm.invalidate_si_reason,
                                     0);
                        }

                        // Acquire SI3 and check that the cell is suitable for camping
                        rr_select_bcch_send_imsg_acquire_si_req(
                          imsg_ptr->cell_info.arfcn,             // arfcn
                          imsg_ptr->cell_info.rxlev_average,     // rxlev_average
                          imsg_ptr->cell_info.bsic,              // bsic
                          NULL,                                  // cell_suitable_cb_fn
                          RR_SELECT_BCCH_EARLY_CAMP_SI3,         // early_camp_required
                          MASK_SI_NONE,                          // notify_si_mask
                          MASK_SI_3,                             // wait_for_si_mask
                          NULL,                                  // init_params_ptr
                          0,                                     // max_allowed_time_ms
                          gas_id                                 // gas_id
                        );

                        // Take a copy of a BA-SACCH which may be used later
                        rr_control_data_ptr->ba_sacch_copy = imsg_ptr->cell_info.ba_sacch;

                        // Wait for RR-SELECT-BCCH module response
                        rr_control_transition_to_state(RR_CELL_SELECTED_ACQUIRE_SI, gas_id);

                        // Start RR_GSM_SYS_INFO_TIMER to guard the SI acquisition
                        (void)rr_timer_start(RR_GSM_SYS_INFO_TIMER, RR_GSM_SYS_INFO_TIMEOUT, gas_id);
                        break;
                      }

                      case RR_CE_REL_STATUS_RPS_REQUIRED:
                      {
                        scell_info_ptr->gsm.camped_on_flag = FALSE;
                        rr_gprs_deactivate_timed_sys_info_refresh(gas_id);

                        rr_resel_send_start_rps_ind(
                           imsg_ptr->cell_info.arfcn,       // serving_cell
                           &imsg_ptr->cell_info.ba_sacch,   // ba_list
                           gas_id                           // gas_id
                         );

                        new_rr_event = EV_GRR_RPS_STARTED;

                        rr_control_transition_to_state(RR_GRR, gas_id);
                        break;
                      }

                      case RR_CE_REL_STATUS_CAMPED:
                      {
                        // There is no need to put L1 is Packet Idle, so transition to GRR_CAMPED
                        rr_grr_send_imsg_camped_on_cell_ind(gas_id);

                        // Inform NAS of the return to Packet Idle (this is not done in GRR-CONTROL)
                        grr_update_mm_with_rr_service_ind(gas_id);

                        rr_control_transition_to_state(RR_GRR, gas_id);
                        break;
                      }

                      default:
                      {
                        ERR_GERAN_FATAL_1_G("status=%d",(int)imsg_ptr->status);
                      }
                    }
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message_ptr,
                      rr_main_state_name(rr_control_data_ptr->rr_state)
                    );
                  }
                }
                break;
              }

#ifdef FEATURE_GSM_DTM
              case RR_DTM_OMSG:
              {
                rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                break;
              }
#endif // FEATURE_GSM_DTM

              default:
              {
                LOG_UNEXPECTED_MESSAGE(
                  new_message_ptr,
                  rr_main_state_name(rr_control_data_ptr->rr_state)
                );
              }
            }
          }

          else
          if ((message_set == MS_RR_L1) &&
              (message_id == (byte) MPH_CHANNEL_RELEASE_CNF))
          {
            /* forward the message to connection release module */
            new_rr_event = rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
          }

          /********************************************************/
          /* Receive measurement of the serving channel, while in */
          /* dedicated mode waiting for L2 to release the conn.   */
          /********************************************************/
          else if ((message_set == MS_RR_L1) &&
                   (message_id == (byte) MPH_SERVING_DED_MEAS_IND))
          {
            rr_store_ded_meas_while_releasing_l2(
              (mph_serving_ded_meas_ind_T *) new_message_ptr,
              gas_id
            );
          }
          /****************************************************/
          /* Receive measurement of the surrounding channels, */
          /* while in dedicated mode waiting for L2 to release*/
          /* the connection.                                  */
          /****************************************************/
          else if ((message_set == MS_RR_L1) &&
                   (message_id == (byte) MPH_SURROUND_MEAS_IND))
          {
            mph_surround_meas_ind_T *mph_surround_meas_ind =
                                      (mph_surround_meas_ind_T *) new_message_ptr;

            rr_store_surround_meas(&mph_surround_meas_ind->best_six_info, TRUE, gas_id);
          }

          #ifdef FEATURE_GSM_DTM
          else if ((message_set == MS_RR_L1) &&
                   (message_id == (byte) MPH_DTM_CHANNEL_RELEASE_IND))
          {
            new_rr_event = rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
          }

          else if ((message_set == MS_MAC_RR) &&
                   (message_id == (byte)MAC_GRR_UL_TBF_REQ))
          {
            /* this is intended to handle the situation where a request from GMAC */
            /* overlaps the suspension of PS services due to a CS conn release    */
            MSG_GERAN_MED_0_G("MAC_GRR_UL_TBF_REQ during connection release");
            /*
            IPA: TODO
            rr_send_grr_mac_ul_tbf_est_rej(DTM_ACCESS_FAILURE);
            */
          }
          #endif /* FEATURE_GSM_DTM */

          else if (message_set == MS_RR_L2)
          {
            if ((message_id == (byte) DL_RELEASE_CNF)    ||
                (message_id == (byte) DL_RELEASE_IND)
               )
            {
              /* forward the message to connection release module */
              new_rr_event = rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }
            else if (message_id == (byte) DL_DATA_ACK_IND)
            {
              /* just absorb this message */
              rr_process_dl_data_ack_ind((dl_data_ack_ind_T *)new_message_ptr, gas_id);
            }
            else
            {
              MSG_GERAN_ERROR_3_G(
                "Unexpected message=(%d,%d), State=%d",
                (int) message_set,
                (int) message_id,
                (int) rr_control_data_ptr->rr_state
              );
            }
          }
          else if (message_set == MS_MM_RR)
          {
            if (message_id == (byte) RR_ABORT_REQ)
            {
              new_rr_event = rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
            }
            else if (message_id == (byte) RR_DATA_REQ)
            {
              MSG_GERAN_HIGH_0_G("Ignoring data req during conn. release");
            }

            /* Queue up other MM messages, and handle them after the connection is released. */
            /* At this point, if there is a MM message pending, RR_EV_PROCESS_SAVED_MESSAGE */
            /* will be returned */
            else if (message_id == (byte) RR_PLMN_LIST_REQ)
            {
              rr_plmn_list_req_T  * rr_plmn_list_req = &new_message_ptr->rr_plmn_list_req;

              if ( rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC )
              {
                MSG_GERAN_HIGH_0_G("Reject automatic PLMN search during conn rel");
                rr_reject_msg(new_message_ptr, gas_id);
              }
              else
              {
                /* Queue the request and handle it after connection is released */
                (void)rr_save_msg(new_message_ptr, gas_id);

                /* Inform connection release that MM requires the connection to be aborted, */
                /* although it is already in the process of being released. This will affect */
                /* how MM is later informed of the connection release. */
                new_rr_event = rr_connection_release(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);
              }
            }

            /* Queue up other MM messages, and handle them after the connection is released. */
            /* At this point, if there is a MM message pending, RR_EV_PROCESS_SAVED_MESSAGE */
            /* will be returned */
            else if ( (message_id == (byte) RR_PLMN_SELECT_REQ) ||
                      (message_id == (byte) RR_STOP_GSM_MODE_REQ) )
            {
              /* Queue the request and handle it after connection is released */
              (void)rr_save_msg(new_message_ptr, gas_id);

              /* Inform connection release that MM requires the connection to be aborted, */
              /* although it is already in the process of being released. This will affect */
              /* how MM is later informed of the connection release. */
              new_rr_event = rr_connection_release(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);
            }

            else if (message_id == (byte) RR_CHANGE_MODE_IND)
            {
              rr_pending_mode_change_data_store(
                &new_message_ptr->mm.rr_change_mode_ind,
                gas_id
              );
            }

#ifdef FEATURE_LTE
            else if (message_id == (byte) RR_CSFB_BLIND_REDIR_TO_LTE_REQ)
            {
              boolean call_aborted  = new_message_ptr->mm.rr_csfb_blind_redir_to_lte_req.call_aborted;
              boolean redir = rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(gas_id);
              boolean redir_after_csfb = rr_g2l_blind_redir_after_csfb_enabled(gas_id);

              MSG_GERAN_HIGH_2_G("G2L blind redir NV: after_voice_ss_or_periodic_lau=%d, after_CSFB=%d",
			  	redir, redir_after_csfb);
	     /*Set the call aborted flag received from NAS*/
              rr_connection_release_set_call_aborted_flag_during_csfb(call_aborted,gas_id);

              if (redir || redir_after_csfb)
              {
                rr_connection_release_set_g2l_blind_redir_after_csfb_flag(gas_id);
              }
            } /* RR_CSFB_BLIND_REDIR_TO_LTE_REQ */
#endif /* FEATURE_LTE */

            else if (message_id == (byte) RR_GMM_GPRS_STATE_CHANGE_REQ)
            {
              ;  // no action required
            }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q            
            else if (message_id == (byte) RR_PSEUDO_LTE_LIST_UPDATE_IND )
            {
              // No action required as NAS DB queried after call release
              ;
            }
#endif
            else
            {
              MSG_GERAN_ERROR_3_G(
                "Unexpected message=(%d,%d), State=%d",
                (int) message_set,
                (int) message_id,
                (int) rr_control_data_ptr->rr_state
              );
            }
          }

#ifdef FEATURE_CGPS_UMTS_CP_GSM
          else if ( message_set == MS_CGPS_RR )
          {
            switch ( message_id )
            {
              case RR_CGPS_EVENT_REQ:
              {
                rr_cgps_process_cgps_event_req(new_message_ptr, gas_id);
                break;
              }
              default:
              {
                MSG_GERAN_ERROR_0_G("Unknown CGPS->RR message");
                break;
              }
            }
          }
#endif /* FEATURE_CGPS_UMTS_CP_GSM */

          else if ((message_set == MS_TIMER) && (message_id == (byte) MID_TIMER_EXPIRY))
          {
            switch (new_message_ptr->mid_timer_expiry.timer_id)
            {
              case T3110:
              {
                new_rr_event = rr_connection_release(EV_T3110_TIMEOUT, NULL, gas_id);
                break;
              }

              case T3122:
              {
                // This is unexpected - normally no CS connection would be attempted if T3122 is running.
                // The exception is an Emergency Call, which could lead to a CS release after which T3122 expires.
                // No need to notify NAS-MM at this point - it will be notified when the next RR_SERVICE_IND
                // is sent when returning to Idle when the CS connection is released
                MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
                break;
              }

              default:
              {
                LOG_UNEXPECTED_TIMER_EXPIRY(
                  new_message_ptr->mid_timer_expiry.timer_id,
                  rr_main_state_name(rr_control_data_ptr->rr_state)
                );
              }
            }
          }

#ifdef FEATURE_SGLTE
          else if (message_set == MS_MSGR_RR)
          {
            switch (new_message_ptr->rr_msgr_msg.message.hdr.id)
            {
#ifdef FEATURE_LTE
              case GERAN_GRR_LTE_PLMN_SRCH_REQ:
              {
                if (rr_is_sglte_mode_active(gas_id) &&
                    new_message_ptr->rr_msgr_msg.message.geran_grr_msg_req.plmn_srch_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
                {
                  rr_measured_signal_data_t * measured_signal_data = rr_get_measured_signal_data_ptr(gas_id);

                  rr_sglte_send_lte_plmn_srch_rsp_in_dedicated(
                    rr_scell_plmn_id(gas_id),
                    measured_signal_data->current_signal_strength,
                    rr_cell_supports_gprs(gas_id) ? SYS_SRV_CAPABILITY_CS_PS_SERVICE : SYS_SRV_CAPABILITY_CS_SERVICE_ONLY,
                    gas_id
                  );
                }
                else
                {
                  rr_sglte_reject_lte_plmn_srch_req(gas_id);
                }
                break;
              } /* GERAN_GRR_LTE_PLMN_SRCH_REQ */
#endif /* FEATURE_LTE */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_DATA_TRANSFER");
              }
            }
          } /* message_set == MS_MSGR_RR */

#ifdef FEATURE_GSM_TDS
          else if (message_set == MS_TDSRRC_RR)
          {
            switch (new_message_ptr->message_header.message_id)
            {
              case TDSRR_INTERRAT_PLMN_SRCH_REQ:
              {
                if (rr_is_sglte_mode_active(gas_id) &&
                    new_message_ptr->tdsrr_plmn_srch_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
                {
                  rr_measured_signal_data_t * measured_signal_data = rr_get_measured_signal_data_ptr(gas_id);

                  rr_sglte_send_tds_plmn_srch_rsp_in_dedicated(
                    rr_scell_plmn_id(gas_id),
                    measured_signal_data->current_signal_strength,
                    rr_cell_supports_gprs(gas_id) ? SYS_SRV_CAPABILITY_CS_PS_SERVICE : SYS_SRV_CAPABILITY_CS_SERVICE_ONLY,
                    gas_id
                  );
                }
                else
                {
                  rr_sglte_reject_tds_plmn_srch_req(gas_id);
                }
                break;
              } /* TDSRR_INTERRAT_PLMN_SRCH_REQ */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_DATA_TRANSFER");
              }
            }
          } /* message_set == MS_TDSRRC_RR */
#endif /*FEATURE_GSM_TDS*/
#endif /* FEATURE_SGLTE */
        } /* EV_INPUT_MESSAGE */

        else if (rr_event == EV_CHOOSE_CELL)
        {
          /* notify connection establishment module that the RR connection is released */
          (void) rr_connection_establishment_control(EV_CHANNEL_DEACTIVATED, NULL, gas_id);

          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);

          /* Perform cell selection to find a new suitable cell */
          new_rr_event = rr_selection_control(EV_TRIGGER_CELL_SELECTION, NULL, gas_id);
        }
        else if (rr_event == EV_CHOOSE_CELL_BY_CELL_SELECTION_INDICATOR)
        {
          rr_chan_rel_cell_selection_indicator_t *rr_cell_select_indicator_ptr;

          rr_cell_select_indicator_ptr = rr_get_cell_select_indicator_ptr(gas_id);

          MSG_GERAN_HIGH_1_G(
            "choose cell by cell selection indicator, target %d",
            rr_cell_select_indicator_ptr->type
          );
          (void) rr_connection_establishment_control(EV_CHANNEL_DEACTIVATED, NULL, gas_id);

          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);

          if (rr_cell_select_indicator_ptr->valid)
          {
            if (rr_cell_select_indicator_ptr->type == GSM_TARGET)
            {
              /* since the cell selection list is provided by network,
               * assuming those cell are unbarred
               */
              bcch_ncell_info_t *ptr_bcch_surround_cell;
              byte i;
              for (i=0; i<rr_cell_select_indicator_ptr->target.gsm_target.num_of_cells; i++)
              {
                ptr_bcch_surround_cell = rr_gprs_get_surround_ncell_sys_info(
                  rr_cell_select_indicator_ptr->target.gsm_target.arfcn[i],
                  gas_id
                );

                if (ptr_bcch_surround_cell != NULL)
                {
                  ptr_bcch_surround_cell->gsm.RACH_control_parameters.CELL_BAR_ACCESS = 0;
                }
              }

              new_rr_event = RR_EV_START_RESELECTION_POWER_SCAN;

              rr_control_transition_to_state(RR_GRR, gas_id);
            }

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
            else if (rr_cell_select_indicator_ptr->type == FDD_TARGET)
            {
              if (rr_is_wcdma_enabled(gas_id))
              {
                uint32 i;

                /* If the network provides one or more UARFCNs in the CSI
                IE and none of these UARFCNs belong to bands that the UE
                currently supports then redirection to WCDMA should not
                be triggered. */
                boolean bands_ok = TRUE;
                gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);

                if (rr_cell_select_indicator_ptr->target.fdd_target.num_of_frequencies > 0)
                {
                  /* This will be set to TRUE if at least one of the indicated
                  UARFCNs is in a supported band. */
                  bands_ok = FALSE;
                }

                /*
                3GPP 45.008 section 6.6.5 states:

                "In case the highest ranked UTRAN cell is not suitable (see 3GPP
                TS 25.304) due to being part of the "list of forbidden LAs for
                roaming" or belonging to a PLMN which is not indicated as being
                equivalent to the registered PLMN, the MS may abandon further
                reselection attempts towards this UTRAN cell and all other cells
                on the same frequency, for a period of up to 20 min. If the MS
                has to perform cell selection, this limitation shall be removed.
                If the MS is redirected under GERAN control to a frequency for
                which the timer is running, any limitation on that frequency
                shall be removed."
                */
                for (i = 0;
                     i < rr_cell_select_indicator_ptr->target.fdd_target.num_of_frequencies &&
                     i < MAX_UTRAN_FDD_FREQUNECIES;
                     ++i)
                {
                  uint16 uarfcn = (uint16) rr_cell_select_indicator_ptr->target.fdd_target.fdd_cell_list[i].uarfcn;
                  rr_irat_frequency_t frequency;

                  frequency.rat = RR_RAT_WCDMA;
                  frequency.data.wcdma = uarfcn;

                  rr_g2x_redir_triggered(frequency, gas_id);

                  if (rr_g2x_is_uarfcn_valid(uarfcn, scell->gsm.location_area_identification.plmn_id, gas_id))
                  {
                    bands_ok = TRUE;
                  }
                }

                if (bands_ok)
                {
                  MSG_GERAN_MED_0_G("CSI indicates WCDMA");
                  rr_ps_no_ps_access_del_all(gas_id);

                  rr_g2x_redir_init(gas_id);
                  rr_g2x_redir_send_imsg_redir_req(
                    rr_cell_select_indicator_ptr,
                    gas_id
                  );

                  /* CSI has been sent to redirection state machine so is no
                  longer required. */
                  rr_init_cell_selection_indicator(gas_id);

                  rr_control_transition_to_state(RR_G2X_REDIR, gas_id);
                }
                else
                {
                  MSG_GERAN_MED_0_G(
                    "CSI indicates WCDMA but UARFCNs in unsupported band, perform cell selection"
                  );
                  new_rr_event = EV_CHOOSE_CELL;
                }
              }
              else
              {
                MSG_GERAN_MED_0_G("CSI indicates WCDMA but not enabled, perform cell selection");
                new_rr_event = EV_CHOOSE_CELL;
              }
            }
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
            else if (rr_cell_select_indicator_ptr->type == TDD_TARGET)
            {
              if (rr_is_utran_tdd_enabled(gas_id))
              {
                uint32 i;
                /* If the network provides one or more UARFCNs in the CSI
                IE and none of these UARFCNs belong to bands that the UE
                currently supports then redirection to TD-SCDMA should not
                be triggered. */
                boolean bands_ok = TRUE;

                if (rr_cell_select_indicator_ptr->target.tdd_target.target.num_of_frequencies > 0)
                {
                  /* This will be set to TRUE if at least one of the indicated
                  UARFCNs is in a supported band. */
                  bands_ok = FALSE;
                }

                /*
                3GPP 45.008 section 6.6.5 states:

                "In case the highest ranked UTRAN cell is not suitable (see 3GPP
                TS 25.304) due to being part of the "list of forbidden LAs for
                roaming" or belonging to a PLMN which is not indicated as being
                equivalent to the registered PLMN, the MS may abandon further
                reselection attempts towards this UTRAN cell and all other cells
                on the same frequency, for a period of up to 20 min. If the MS
                has to perform cell selection, this limitation shall be removed.
                If the MS is redirected under GERAN control to a frequency for
                which the timer is running, any limitation on that frequency
                shall be removed."
                */
                for (i = 0;
                     i < rr_cell_select_indicator_ptr->target.tdd_target.target.num_of_frequencies &&
                     i < MAX_UTRAN_TDS_FREQUNECIES;
                     ++i)
                {
                  uint16 uarfcn = (uint16) rr_cell_select_indicator_ptr->target.tdd_target.target.fdd_cell_list[i].uarfcn;

                  rr_irat_frequency_t frequency;
                  frequency.rat = RR_RAT_UTRAN_TDD;
                  frequency.data.tdscdma = uarfcn;

                  rr_g2x_redir_triggered(frequency, gas_id);

                  if (tdsrrc_is_arfcn_valid(uarfcn))
                  {
                    bands_ok = TRUE;
                  }
                }

                if (bands_ok)
                {
                  MSG_GERAN_MED_0_G("CSI indicates TDSCDMA");
                  rr_ps_no_ps_access_del_all(gas_id);

                  rr_g2x_redir_init(gas_id);
                  rr_g2x_redir_send_imsg_redir_req(
                    rr_cell_select_indicator_ptr,
                    gas_id
                  );

                  /* CSI has been sent to redirection state machine so is no
                  longer required. */
                  rr_init_cell_selection_indicator(gas_id);

                  rr_control_transition_to_state(RR_G2X_REDIR, gas_id);
                }
                else
                {
                  MSG_GERAN_MED_0_G(
                    "CSI indicates TDSCDMA but UARFCNs in unsupported band, perform cell selection"
                  );
                  new_rr_event = EV_CHOOSE_CELL;
                }
              }
              else
              {
                MSG_GERAN_MED_0_G("CSI indicates TDSCDMA but not enabled, perform cell selection");
                new_rr_event = EV_CHOOSE_CELL;
              }
            }
#endif /* FEATURE_GSM_TDS_G2T_REDIRECTION */

#ifdef FEATURE_LTE
            else if (rr_cell_select_indicator_ptr->type == LTE_TARGET)
            {
              uint16 i;

              /* If the network provides one or more EARFCNs in the CSI
              IE and none of these EARFCNs belong to bands that the UE
              currently supports then redirection to LTE should not
              be triggered. */
              boolean bands_ok = TRUE;

              if (rr_cell_select_indicator_ptr->target.lte_target.eutran_descriptions_count > 0)
              {
                /* This will be set to TRUE if at least one of the indicated
                EARFCNs is in a supported band. */
                bands_ok = FALSE;
              }

              for (i = 0;
                   i < rr_cell_select_indicator_ptr->target.lte_target.eutran_descriptions_count &&
                   i < MAX_CSI_EUTRAN_DESCRIPTIONS_COUNT;
                   ++i)
              {
                rr_irat_frequency_t frequency;
                frequency.rat = RR_RAT_LTE;
                frequency.data.lte = rr_cell_select_indicator_ptr->target.lte_target.eutran_descriptions[i].earfcn;

                rr_g2x_redir_triggered(frequency, gas_id);

                if (lte_rrc_earfcn_is_supported(frequency.data.lte, rr_band_pref_lte(gas_id)))
                {
                  bands_ok = TRUE;
                }
              }

              if (rr_is_lte_enabled(gas_id))
              {
                if (bands_ok)
                {
                  MSG_GERAN_MED_0_G("CSI indicates LTE");
                  rr_ps_no_ps_access_del_all(gas_id);

                  rr_g2x_redir_init(gas_id);
                  rr_g2x_redir_send_imsg_redir_req(
                    rr_cell_select_indicator_ptr,
                    gas_id
                  );

                  /* CSI has been sent to redirection state machine so is no
                  longer required. */
                  rr_init_cell_selection_indicator(gas_id);

                  rr_control_transition_to_state(RR_G2X_REDIR, gas_id);
                }
                else
                {
                  MSG_GERAN_MED_0_G(
                    "CSI indicates LTE but EARFCNs in unsupported band, perform cell selection"
                  );
                  new_rr_event = EV_CHOOSE_CELL;
                }
              }
              else
              {
                MSG_GERAN_MED_0_G("LTE indicated but not enabled, perform cell selection");
                new_rr_event = EV_CHOOSE_CELL;
              }
            }
#endif /* FEATURE_LTE */

            else
            {
              new_rr_event = EV_CHOOSE_CELL;
            }
          }
          else
          {
            new_rr_event = EV_CHOOSE_CELL;
          }
        }

#ifdef FEATURE_LTE
        else if (rr_event == EV_G2L_BLIND_REDIR_REQUIRED)
        {
          /* Any measurement reporting data can be thrown away */
          rr_ded_meas_reporting_discard_data(gas_id);

          /* notify connection establishment module that the RR connection is released */
          (void) rr_connection_establishment_control(EV_CHANNEL_DEACTIVATED, NULL, gas_id);

          if (rr_is_lte_enabled(gas_id))
          {
            rr_chan_rel_cell_selection_indicator_t *csi = rr_get_cell_select_indicator_ptr(gas_id);

            csi->valid = TRUE;
            csi->type = LTE_TARGET;
            csi->target.lte_target.blind_redir = TRUE;

            MSG_GERAN_MED_0_G("Starting G2L blind redir");
            rr_ps_no_ps_access_del_all(gas_id);

            rr_g2x_redir_init(gas_id);
            rr_g2x_redir_send_imsg_redir_req(
              csi,
              gas_id
            );

            /* CSI has been sent to redirection state machine so is no
            longer required. */
            rr_init_cell_selection_indicator(gas_id);

            rr_control_transition_to_state(RR_G2X_REDIR, gas_id);
          }
          else
          {
            MSG_GERAN_MED_0_G("G2L blind redir required but LTE not supported, perform cell selection");
            new_rr_event = EV_CHOOSE_CELL;
          }
        }
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
        else if (rr_event == EV_G2T_PSEUDO_FR_REQUIRED)
        {
          rr_chan_rel_cell_selection_indicator_t *csi = rr_get_cell_select_indicator_ptr(gas_id);

          /* notify connection establishment module that the RR connection is released */
          (void) rr_connection_establishment_control(EV_CHANNEL_DEACTIVATED, NULL, gas_id);

          rr_ded_meas_reporting_discard_data(gas_id);

          csi->valid = TRUE;
          csi->type = TDD_TARGET;
          csi->target.tdd_target.pseudo_fr_active = TRUE;

          rr_tds_pseudo_fr_populate_freq_list(&(csi->target.tdd_target.target), gas_id);

          /* Stored Pseudo FR dedicated data can now be reset as it has been formatted for G->T Redir*/
          rr_tds_pseudo_fr_reset_dedicated_info(gas_id);

          MSG_GERAN_MED_0_G("RR Initiating G2T Pseudo FR");
          rr_ps_no_ps_access_del_all(gas_id);

          rr_g2x_redir_init(gas_id);
          rr_g2x_redir_send_imsg_redir_req(
            csi,
            gas_id
          );

          /* CSI has been sent to redirection state machine so is no
          longer required. */
          rr_init_cell_selection_indicator(gas_id);

          rr_control_transition_to_state(RR_G2X_REDIR, gas_id);
        }
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

        else if ((rr_event == EV_CHOOSE_CELL_FOR_REEST) ||
                 (rr_event == EV_CHOOSE_CELL_FOR_CAMP))
        {
          /* notify connection establishment module that the RR connection is released */
          (void) rr_connection_establishment_control(EV_CHANNEL_DEACTIVATED, NULL, gas_id);

          // Any measurement reporting data can be thrown away
          // Note: RR-CELL-REEST uses the dedicated best-six data to perform a RPS; It does not reference
          // BA-SACCH lists
          // TODO: This could do with tidying, as it is easy to see the the validity of these items as linked
          rr_ded_meas_reporting_discard_data(gas_id);

          /*
          Returns EV_CELL_REEST_TRIGGERED if there are suitable candidates, or
          EV_CHOOSE_CELL if cell selection is required
          */
          new_rr_event = rr_cell_reest_control(rr_event, NULL, gas_id);
        }
        else if (rr_event == EV_CELL_SELECTION_TRIGGERED)
        {
          rr_control_transition_to_state(RR_CELL_SELECTION, gas_id);
        }
        else if (rr_event == RR_EV_CELL_REEST_TRIGGERED)
        {
          rr_control_transition_to_state(RR_CELL_REESTABLISH, gas_id);
        }
        else if (rr_event == RR_EV_ACQUISITION_FAILURE)
        {
           new_rr_event = EV_CHOOSE_CELL;
        }

        else if ( rr_event == RR_EV_CONNECTION_RELEASE_IN_PROGRESS)
        {
          ;
        }

        else if ( rr_event == RR_EV_PROCESS_SAVED_MESSAGE )
        {
          (void) rr_connection_establishment_control(EV_CHANNEL_DEACTIVATED, NULL, gas_id);

          // Any measurement reporting data can be thrown away
          rr_ded_meas_reporting_discard_data(gas_id);

          rr_control_transition_to_state(RR_NO_CHANNELS, gas_id);

          new_rr_event = rr_process_saved_message(&new_message_ptr, gas_id);

          if ((new_rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
          {
            // If about to start a PLMN LIST REQ, this will be a foreground search, so inform
            // NAS that RR is currently not camped to ensure service is requested once the search
            // has been completed
            if ((new_message_ptr->message_header.message_set == MS_MM_RR) &&
                (new_message_ptr->message_header.message_id == (byte) RR_PLMN_LIST_REQ))
            {
              rr_send_no_service_ind(gas_id);
            }
          }
        }
        else
        /*****************************************************************/
        /* EV_NO_BCCH_FOUND                                              */
        /*                                                               */
        /* A foreground PLMN search has been completed without having    */
        /* to perform a power scan.                                      */
        /* Deactivate to keep RR/L1 in sync (this is necessary, because  */
        /* it is not known what state L1 was left in by GRR), and ensure */
        /* RR/L1 will then be reactivated and cell selection prformed    */
        /*****************************************************************/
        if ( rr_event == EV_NO_BCCH_FOUND )
        {
          /* Remove the saved RR_PLMN_LIST_REQ as no search is going to be performed */
          rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);

          rr_control_transition_to_state(RR_GOING_INACTIVE, gas_id);

          new_rr_event = rr_trigger_gsm_as_deactivation(RR_DEACT_REASON_STOP_CAMPING_THEN_RESTART, gas_id);
        }

        else
        if ( rr_event == EV_PANIC_RESET )
        {
          /* Send RR_ABORT_IND / RR_ABORT_CNF to MM; No RR_SERVICE_IND */
          rr_conn_inform_mm_of_channel_release(FALSE, gas_id);

          (void) rr_connection_establishment_control(EV_CHANNEL_DEACTIVATED, NULL, gas_id);
          (void) rr_connection_release(EV_RESET_SOFTWARE, NULL, gas_id);

          #ifdef FEATURE_GSM_DTM
          (void) rr_dtm_control(EV_RESET_SOFTWARE, NULL, gas_id);
          #endif /* FEATURE_GSM_DTM */
        }

        else if ( rr_event == EV_MAC_IDLE_MODE )
        {
          MSG_GERAN_MED_0_G("MAC is IDLE");
        }

        else if ( rr_event == EV_GPRS_SUSPENDED )
        {
          MSG_GERAN_MED_0_G("GPRS suspended");
        }

        else if (rr_event == EV_PCCO_FAILURE_CCO_TIMER_EXPIRY)
        {
          rr_control_transition_to_state(RR_GRR, gas_id);

          new_rr_event = EV_PCCO_FAILED;
        }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
        else if ( rr_event == EV_X2G_CCO_FAILURE_CCO_TIMER_EXPIRY )
        {
          /* RR is no longer camped, so clear the camped on flag. */
          gprs_scell_info_t * scell = rr_gprs_get_scell_info(gas_id);
          scell->gsm.camped_on_flag = FALSE;

          rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_FAILURE_CCO_TIMER_EXPIRY;
          set_rr_deact_reason(RR_DEACT_REASON_SWITCH_TO_OTHER_RAT, gas_id);
          new_rr_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);

          rr_control_transition_to_state(RR_X2G_CCO_FAILED, gas_id);
        } /* EV_X2G_CCO_FAILURE_CCO_TIMER_EXPIRY */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(rr_event),
                               rr_main_state_name(rr_control_data_ptr->rr_state));
        }
      } /* RR_CONNECTION_RELEASE */
      break;


      /***********************************************************************/
      /* RR_EARLY_CAMPED_WAIT_FOR_SI                                         */
      /*                                                                     */
      /* The CS connection has been released, but RR does not have complete  */
      /* sys info, so is not yet able to yet. Wait here until all the sys    */
      /* info is received.                                                   */
      /***********************************************************************/
      case RR_EARLY_CAMPED_WAIT_FOR_SI:
      {
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (new_message_ptr->rr.header.rr_message_set)
            {
              case RR_L1_IDLE_IMSG:
              {
                rr_l1_idle_control(new_message_ptr, gas_id);
                break;
              }

              case RR_L1_IDLE_OMSG:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF:
                  {
                    /* Start RR_GSM_SYS_INFO_TIMER to guard the sys info acquisition */
                    (void)rr_timer_start(RR_GSM_SYS_INFO_TIMER, RR_GSM_SYS_INFO_TIMEOUT, gas_id);
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message_ptr,
                      rr_main_state_name(rr_control_data_ptr->rr_state)
                    );
                  }
                }
                break;
              }

              case RR_IMSG:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case RR_IMSG_MSC_CHANGE_IND:
                  {
                    /*
                    Update NAS as required
                    RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                    This will be sent when RR/L1 enters Idle
                    */
                    rr_msc_update(
                      FALSE,   // can_send_service_ind
                      gas_id   // gas_id
                    );
                    break;
                  }

                  case RR_IMSG_PS_ACCESS_REQ:
                  {
                    // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                    rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  case RR_IMSG_NO_PS_ACCESS_REQ:
                  {
                    // No PS access request (could be TLLI unassignment, CS-only)

                    rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  case RR_IMSG_PAGE_IND:
                  {
                    rr_imsg_page_ind_t *imsg = &new_message_ptr->rr.rr.imsg.page_ind;

                    rr_ce_save_page(&imsg->page_info, gas_id);
                    break;
                  }

                  case RR_IMSG_PAGE_MODE_IND:
                  {
                    // Changes in OTA page mode are ignored during Early Camping
                    MSG_GERAN_MED_0_G("Page Mode change ignored");
                    break;
                  }

                  default: ; // ignore other RR-IMSG messages
                }
                break;
              }

              default: ; // ignore other RR-RR messages
            }
          }

          else if (message_set == MS_RR_L1)
          {
            if ((message_id == (byte) MPH_SURROUND_MEAS_IND)       ||
                (message_id == (byte) MPH_BLOCK_QUALITY_IND)       ||
                (message_id == (byte) MPH_SURROUND_WCDMA_IDLE_MEAS_IND)
               )
            {
              /* message can be ignored */
            }
            /* CR42445: Handle Serving Idle Meas in this state */
            /* We cannot compute C1 & C2 as we may not have SIs to do so    */
            else if (message_id == (byte) MPH_SERVING_IDLE_MEAS_IND)
            {
              gprs_scell_info_t         * pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

              pcell_info_ptr->gsm.reselection_info.RXLEV_average = new_message_ptr->mph_serving_idle_meas_ind.RXLEV_average;
              pcell_info_ptr->gsm.trk_lo_adj = new_message_ptr->mph_serving_idle_meas_ind.trk_lo_adj;

              /* Set measurements_valid to FALSE to prevent any Reselection calculations   */
              /* We will set it to true once we camp and receive MPH_SERVING_IDLE_MEAS_IND */
              pcell_info_ptr->gsm.reselection_info.measurements_valid = FALSE;
            }

            else if (message_id == (byte) MPH_SURROUND_UPDATE_FAILURE_IND)
            {
              /* since no reselection is taking place, just ignore it */
            }
            else if (message_id == (byte) MPH_SERVING_AUX_MEAS_IND)
            {
               /* Ignore */
            }

            else
            {
              MSG_GERAN_ERROR_3_G(
                "Unexpected message=(%d,%d), State=%d",
                (int) message_set,
                (int) message_id,
                (int) rr_control_data_ptr->rr_state
              );
            }
          }
          else if (message_set == MS_MM_RR)
          {
            if (message_id == (byte) RR_EST_REQ)
            {
              (void)rr_save_msg(new_message_ptr, gas_id);
              MSG_GERAN_HIGH_0_G("Saved RR_EST_REQ in EARLY_CAMP_WAIT_FOR_SI");
            } /* RR_EST_REQ */

            else if (message_id == (byte) RR_PLMN_LIST_REQ)
            {
              /*
              If this is an automatic HPLMN search, reject it (it may be retried later).
              If this is a manual PLMN search, save the message.
              */
              if (new_message_ptr->
                    mm.rr_plmn_list_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
              {
                rr_reject_msg(new_message_ptr, gas_id);
              }
              else
              {
                (void)rr_save_msg(new_message_ptr, gas_id);
              MSG_GERAN_HIGH_0_G("Saved RR_PLMN_SELECT_REQ in EARLY_CAMP_WAIT_FOR_SI");
              }
            }
            else if (message_id == (byte) RR_PLMN_SELECT_REQ)
            {
              /* Save the RR_PLMN_SELECT_REQ
               * - a cell selection will be performed when RR camps on the cell with different PLMN-id
               * - or a PLMN_SELECT_CNF will be send back if RR camps on a suitable cell
               * - if acquisition fails, a cell selection will be performed
               */
              (void)rr_save_msg(new_message_ptr, gas_id);
              MSG_GERAN_HIGH_0_G("Saved RR_PLMN_SELECT_REQ during EARLY_CAMP_WAIT_FOR_SI");
            }
            else if (message_id == (byte) RR_STOP_GSM_MODE_REQ)
            {
              /* Save this in case there is a panic reset during shut-down */
              rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

              /* The acquisition is aborted, so cancel it's guard timer */
              (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);
              (void)rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id);

              /* If on going CCO Operation then send CCO success to the source RAT */
              if ( new_message_ptr->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_MM_USER_RAT_CHANGE )
              {
                if ( rr_x2g_cco_in_progress(gas_id ) )
                {
                  /* terminate CCO and then proceed with processing STOP MODE Request */
                  (void)rr_timer_cancel(T3174, gas_id);

                  switch (rr_control_data_ptr->x2g_resel_type)
                  {
#ifdef FEATURE_WCDMA
                    case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
                    {
                      rr_send_interrat_cc_order_cnf(CCO_SUCCESS, RR_GSM_CCO_OTHER_REASON, gas_id);
                      break;
                    }
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                    case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                    {
                      rr_send_tds_interrat_cc_order_cnf(
                      TDSCCO_SUCCESS,
                      TDSRR_GSM_CCO_OTHER_REASON,
                      gas_id
                      );

                     rr_control_data_ptr->waiting_for_tdsrr_interrat_tl1_stop_ind = FALSE;
                     break;
                    }
#endif /*FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
                  case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                  {
                    geran_grr_lte_cco_rsp_t lte_cco_rsp;
                    errno_enum_type         result;

                    memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

                    msgr_init_hdr(
                     &lte_cco_rsp.hdr,
                     MSGR_GERAN_GRR,
                     GERAN_GRR_LTE_CCO_RSP
                    );
                   lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_SUCCESS;
                   result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
                   if (result != E_SUCCESS)
                   {
                     MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                   }
                   rr_control_data_ptr->waiting_for_lte_rrc_cco_nacc_completed_ind = FALSE;
                 break;
                }
#endif /* FEATURE_LTE */

                  default:
                  {
                    MSG_GERAN_ERROR_1_G(
                    "Unexpected x2g_resel_type: %d",
                     rr_control_data_ptr->x2g_resel_type
                    );
                  }
                }

                  MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
                  rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                  rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                }
              }
              
              /* stop will take care of aborting ongoing operations */
              new_rr_event = rr_process_rr_stop_gsm_mode_req(TRUE, gas_id);
              rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
            }
            else if (message_id == (byte) RR_ABORT_REQ)
            {
              if ( grr_verify_saved_mm_message( RR_EST_REQ, gas_id ))
              {
                grr_throwaway_saved_message(gas_id);

                /* no connection exists, respond with success */
                rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);                   
              }
              else
              {                  
                MSG_GERAN_ERROR_1_G(" RR_ABORT_REQ: Unexpected when No connection Exists in RR State %d", rr_control_data_ptr->rr_state );
                /* no connection exists, respond with success */
                rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);                   
                /* clear any saved MM messages since the abort overrides them */
                rr_delete_saved_msg(gas_id);
              }
            }
            else if (message_id == (byte) RR_GMM_GPRS_STATE_CHANGE_REQ)
            {
              ;  // no action required
            }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q            
            else if (message_id == (byte) RR_PSEUDO_LTE_LIST_UPDATE_IND)
            {
              ;  // no action required
            }
#endif            
#ifdef FEATURE_SGLTE
            else if (message_id == (byte) RR_CHANGE_MODE_IND)
            {
              if (rr_is_sglte_mode_active(gas_id))
              {
                 /* RR_CHANGE_MODE_IND may change service domain and/or RAT priority list. hence store it.*/
                 rr_pending_mode_change_data_store(&new_message_ptr->mm.rr_change_mode_ind, gas_id);
              }
              break;
            }
#endif /* FEATURE_SGLTE */
            else
            {
              MSG_GERAN_ERROR_3_G(
                "Unexpected message=(%d,%d), State=%d",
                (int) message_set,
                (int) message_id,
                (int) rr_control_data_ptr->rr_state
              );
            }
          } /* MS_MM_RR */
          else if ((message_set == MS_TIMER) && (message_id == (byte) MID_TIMER_EXPIRY))
          {
            switch (new_message_ptr->mid_timer_expiry.timer_id)
            {
              case RR_GSM_SYS_INFO_TIMER:
              {
                /* give up trying to camp on this cell, aborting any pending establishment first */
                if (rr_abort_pending_rr_est_req(RR_ABORT_CON_EST_NOT_ATTEMPTED, gas_id))
                {
                  MSG_GERAN_HIGH_0_G("Aborted pending establishment - SI timeout during early camp");
                }
                else
                {
                  MSG_GERAN_HIGH_0_G("Giving up on the early camping");
                }

                /* perform an autonomous cell selection to attempt to recover */
                new_rr_event = EV_TRIGGER_CELL_SELECTION;
                break;
              }

              case RR_GPRS_SYS_INFO_TIMER:
              {
                /* GPRS SI13 failed to arrive */
                MSG_GERAN_HIGH_0_G("GPRS SI timeout");
                new_rr_event = RR_EV_ACQUISITION_FAILURE;
                break;
              }

              case T3122:
              {
                // This is unexpected - normally no CS connection would be attempted if T3122 is running.
                // The exception is an Emergency Call, which could lead to a CS release after which T3122 expires.
                // No need to notify NAS-MM at this point - it will be notified when the next RR_SERVICE_IND
                // is sent when returning to Idle when the CS connection is released
                MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
                break;
              }

              default:
              {
                LOG_UNEXPECTED_TIMER_EXPIRY(
                  new_message_ptr->mid_timer_expiry.timer_id,
                  rr_main_state_name(rr_control_data_ptr->rr_state)
                );
              }
            }
          } /* MID_TIMER_EXPIRY */
          else
          {
            LOG_UNEXPECTED_MESSAGE(
              new_message_ptr,
              rr_main_state_name(rr_control_data_ptr->rr_state)
            );
          }
        } /* EV_INPUT_MESSAGE */

        else if (rr_event == EV_CHECK_GPRS)
        {
          gprs_scell_info_t         * pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
          
          if (pcell_info_ptr->gsm.gprs_indicator)
          {
            MSG_GERAN_HIGH_0_G("Cell supports GPRS");

            if (pcell_info_ptr->bcch_sys_info.si13_data_valid_flag)
            {
              new_rr_event = RR_EV_GPRS_SI13_RECEIVED;
            }
            else
            {
              MSG_GERAN_LOW_0_G("Waiting for SI13...");

              /* Start guard timer for SI13 */
              (void)rr_timer_start(RR_GPRS_SYS_INFO_TIMER, RR_GPRS_SYS_INFO_TIMEOUT, gas_id);
            }
          }
          else
          {
            MSG_GERAN_HIGH_0_G("Cell is GSM-only");

            new_rr_event = EV_GO_TO_IDLE;
          }
        }

        else if (rr_event == RR_EV_GPRS_SI13_RECEIVED)
        {
          gprs_scell_info_t         * pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

          if ( ARE_SET( MASK_SI_2_TER, pcell_info_ptr->gsm.update_message_received ) )
          {
            new_rr_event = EV_GO_TO_IDLE;
          }
          else
          {
            if (rr_is_fast_si_acq_enabled(NULL,gas_id))
            {
              MSG_GERAN_HIGH_0_G("Fast SI Acq enabled, need not wait for Si2ter");
              new_rr_event = EV_GO_TO_IDLE;
            }
            else
            {
              MSG_GERAN_MED_0_G("Wait for SI2ter...");
            }
          }
        }

        else if ( (rr_event == RR_EV_ACQUISITION_FAILURE) ||
                  (rr_event == EV_TRIGGER_CELL_SELECTION) )
        {
          /* Cancel the timer guarding this sys info acquisition, and prepare for */
          /* cell selection */
          (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);
          (void)rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id);

          rr_leave_idle_mode(RR_NOT_CAMPED, gas_id);

          if (grr_verify_saved_mm_message(RR_PLMN_LIST_REQ, gas_id))
          {
            /**
             * Layer 1 will be stopped and restarted - RR will check for saved
             * messages and process the saved RR_PLMN_LIST_REQ when L1 is
             * restarted.
             */
            rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
            new_rr_event = rr_trigger_gsm_as_deactivation(
              RR_DEACT_REASON_STOP_CAMPING_THEN_RESTART,
              gas_id
            );

            /**
             * Ensure NAS brings RR back into service after the search completes
             * by indicating NO SERVICE now
             */
            rr_send_no_service_ind(gas_id);
          }
          else if (grr_verify_saved_mm_message(RR_PLMN_SELECT_REQ, gas_id))
          {
            /**
             * Layer 1 will be stopped and restarted - RR will check for saved
             * messages and process the saved RR_PLMN_SELECT_REQ when L1 is
             * restarted.
             */
            rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
            new_rr_event = rr_trigger_gsm_as_deactivation(
              RR_DEACT_REASON_STOP_CAMPING_THEN_RESTART,
              gas_id
            );
          }
          else
          {
            /* Trigger a cell selection - no need to process the return event (which */
            /* will be EV_CELL_SELECTION_TRIGGERED) */
            rr_control_data_ptr->rr_state = RR_CELL_SELECTION;
            (void) rr_selection_control(EV_TRIGGER_CELL_SELECTION, NULL, gas_id);
          }
        }

        else if (rr_event == EV_GO_TO_IDLE)
        {
          (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);
          (void)rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id);

          rr_grr_send_imsg_camp_on_cell_req(
            TRUE,     // notify_mm_service_ind
            FALSE,    // notify_mm_plmn_select_cnf
            TRUE,     // purge_gmm_signalling
            gas_id    // gas_id
          );

          rr_control_data_ptr->rr_state = RR_GRR;

          // TODO [free-float] - what to do here
#if 0
          if (grr_verify_saved_mm_message(RR_PLMN_LIST_REQ))
          {
            /* If there is a saved RR_PLMN_LIST_REQ, we should process it
            immediately rather than resuming GPRS.  This resolves a problem
            where RESUMPTION_IND is sent to GMM, followed immediately by
            a SUSPENSION. */

            new_rr_event = RR_EV_PROCESS_SAVED_MESSAGE;
          }
          else
          {
            /* transition to idle and handle any saved messages that persist */
            new_rr_event = RR_EV_CAMPED_ON_CELL;
          }
#endif
        }
        else if (rr_event == RR_EV_CAMPED_ON_CELL)
        {
          MSG_GERAN_ERROR_0_G("Unhandled RR_EV_CAMPED_ON_CELL in EARLY_CAMPED_WAIT_FOR_SI");
        }

        else if (rr_event == RR_EV_GSM_SI_RECEIVED)
        {
          gprs_scell_info_t         * pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);

          /* all necessary information for camping has been saved in pending channel data */
          if (pcell_info_ptr->gsm.valid_data_flag == TRUE)
          {
            (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);

            /* check for pending establishment requests that should be discarded */
            rr_abort_pending_rr_est_req_if_lai_changed(
              &(pcell_info_ptr->gsm.location_area_identification),
              gas_id
             );

            /* Check if the serving cell is suitable             */
            /* After a hand over procedure we could potentially  */
            /* end up in a BARRED CELL                           */
            if (rr_check_cell_suitable(
                  &pcell_info_ptr->gsm.cell_selection_parameters,
                  &pcell_info_ptr->gsm.RACH_control_parameters,
                  &pcell_info_ptr->gsm.location_area_identification,
                  pcell_info_ptr->gsm.reselection_info.RXLEV_average,
                  pcell_info_ptr->gsm.BCCH_ARFCN,
                  &pcell_info_ptr->gsm.BSIC,
                  rr_is_any_cell_selection_mode(gas_id),
                  rr_is_automatic_plmn_selection_mode(gas_id),
                  TRUE,
                  gas_id
                ))
            {
              new_rr_event = EV_CHECK_GPRS;
            }
            else
            {
               /* Check for Barred Cell or C1 <=0, and if TRUE */
               /* trigger Cell Selection ELSE inform NAS & L1  */
               if (rr_cell_barred_or_c1_unsuitable(
                     &pcell_info_ptr->gsm.cell_selection_parameters,
                     &pcell_info_ptr->gsm.RACH_control_parameters,
                     &pcell_info_ptr->gsm.location_area_identification,
                     pcell_info_ptr->gsm.reselection_info.RXLEV_average,
                     pcell_info_ptr->gsm.BCCH_ARFCN,
                     &pcell_info_ptr->gsm.BSIC,
                     gas_id
                   ))
               {
                 /*
                 Because all system info has been received the BAlist is populated
                 so a RPS can be started as the cell is no good.
                 */

                 // TODO: Actually can use BA as it is available, but instead the current
                 // implementation is to use BA-SACCH.
                 // This should be changed to check what is available

                 MSG_GERAN_HIGH_0_G("Cell Barred or C1<=0: Triggering RPS");

                 // Perform RPS using the serving cell + BA(SACCH)
                 rr_resel_send_start_rps_ind(
                   rr_gprs_get_scell_arfcn(gas_id),
                   &rr_control_data_ptr->ba_sacch_copy,
                   gas_id
                 );

                 new_rr_event = EV_GRR_RPS_STARTED;
                 rr_control_data_ptr->rr_state = RR_GRR;
               }
               else
               /* The suitability check has failed due at least one of: */
               /* a) Forbidden LAI (most likely)                        */
               /* b) PLMN not suitable                                  */
               /* c) Band not supported                                 */
               {
                  MSG_GERAN_HIGH_3_G(
                    "Cell not suitable (LAI/PLMN/Band %d %d %d)",
                    (int) rr_is_any_cell_selection_mode(gas_id),
                    (int) rr_is_any_cell_reselection_mode(gas_id),
                    (int) GET_ARFCN_BAND(pcell_info_ptr->gsm.BCCH_ARFCN)
                  );

                  rr_ce_discard_saved_pages(gas_id);

                  rr_leave_idle_mode(RR_NOT_CAMPED, gas_id);

                  /* Inform MM of NO SERVICE. The upper layers will then instruct RR */
                  /* to re-camp (PLMN SELECT REQ) */
                  rr_send_no_service_ind(gas_id);

                  /* Deactivate L1, and go to INACTIVE state */
                  rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
                  new_rr_event = rr_trigger_gsm_as_deactivation(
                    RR_DEACT_REASON_STOP_CAMPING,
                    gas_id
                  );
               } /* rr_check_cell_for_selection */
            } /* rr_check_cell_suitable */
          } /* valid_data_flag */
        } /* RR_EV_GSM_SI_RECEIVED */

        else if (rr_event == RR_EV_EARLY_CAMP_SI_RECEIVED)
        {
          // TODO: Ideally early camping should be started when SI3 is received
          // However at the moment RR-CONN-RELEASE and RR-CELL-REESTABLISH start
          // early camping [no ccch] when they exit
        } /* RR_EV_EARLY_CAMP_SI_RECEIVED */

        /* EV_NO_BCCH_FOUND - A PLMN LIST REQ interrupted SI acquisition and was processed, but */
        /* cached results were used and no power-scan was performed. */
        /* L1 may be in IDLE due to early camping so it is not possible to go to NO CHANNELS, so */
        /* RR/L1 need to be deactivated to keep them in sync. Then perform cell selection to */
        /* regain service */
        else if (rr_event == EV_NO_BCCH_FOUND)
        {
          // DMB  I don't think this is a possible execution path - need to check

          /* Remove the RR_PLMN_LIST_REQ from the panic reset saved message */
          rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);

          rr_ce_discard_saved_pages(gas_id);

          rr_leave_idle_mode(RR_NOT_CAMPED, gas_id);

          /* Deactivate L1, and go to INACTIVE state, then re-camp */
          rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
          new_rr_event = rr_trigger_gsm_as_deactivation(
            RR_DEACT_REASON_STOP_CAMPING_THEN_RESTART,
            gas_id
          );
        }

        /* EV_CELL_SELECTION_TRIGGERED - a cell selection has begun */
        // TODO: Not sure if it's possible to get here ?
        else if ( rr_event == EV_CELL_SELECTION_TRIGGERED )
        {
          /* The acquisition is aborted, so cancel it's guard timer */
          (void)rr_timer_cancel(RR_GSM_SYS_INFO_TIMER, gas_id);
          (void)rr_timer_cancel(RR_GPRS_SYS_INFO_TIMER, gas_id);

          rr_control_data_ptr->rr_state = RR_CELL_SELECTION;
        }

        /* RR_EV_NO_CELL_SELECTION_REQUIRED - current cell is suitable */
        // TODO: Not sure if it's possible to get here ?
        else if ( rr_event == RR_EV_NO_CELL_SELECTION_REQUIRED )
        {
          ;
        }

        /* Failure / Panic Reset occurred */
        else if (rr_event == EV_PANIC_RESET)
        {
          /* As we have not arrived in this state due to any MM request, there is no */
          /* specific action required here */
          ;
        }

        else if (rr_event == RR_EV_CANDIDATE_CELL_SI_RECEIVED)
        {
          /* No action necessary */
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(rr_event),
                               rr_main_state_name(rr_control_data_ptr->rr_state));
        }
      } /* RR_EARLY_CAMPED_WAIT_FOR_SI */
      break;

#ifdef FEATURE_WCDMA
      /***********************************************************************/
      /* RR_G2W_INTERRAT_HANDOVER_PROGRESS                                   */
      /*                                                                     */
      /* G->W Handover in progress                                           */
      /***********************************************************************/
      case RR_G2W_INTERRAT_HANDOVER_PROGRESS:
      {
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          if (message_set == MS_RR_RR)
          {
            switch (new_message_ptr->rr.header.rr_message_set)
            {
              case RR_IMSG:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case RR_IMSG_MSC_CHANGE_IND:
                  {
                    /*
                    Update NAS as required
                    RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                    This will be sent when G is next active
                    */
                    rr_msc_update(
                      FALSE,   // can_send_service_ind
                      gas_id   // gas_id
                    );
                    break;
                  }

                  case RR_IMSG_PS_ACCESS_REQ:
                  {
                    // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                    rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  case RR_IMSG_NO_PS_ACCESS_REQ:
                  {
                    // No PS access request (could be TLLI unassignment, CS-only)

                    rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  default: ; // ignore other RR-IMSG messages
                }
                break;
              }

              default: ; // ignore other RR-RR messages
            }
          }

          else
          if ((message_set == MS_RR_L2) && (message_id == (byte) DL_DATA_ACK_IND))
          {
            rr_process_dl_data_ack_ind((dl_data_ack_ind_T *)new_message_ptr, gas_id);
          }
          else if (
                    (
                     (message_set == MS_RR_L2) &&
                     (
                       (message_id == (byte) DL_SUSPEND_CNF) ||
                       (message_id == (byte) DL_RELEASE_IND) ||
                       (message_id == (byte) MDL_ERROR_IND)
                     )
                    )
                    ||
                    (
                     (message_set == MS_MM_RR) &&
                     (
                       (message_id == (byte) RR_ABORT_REQ) ||
                       (message_id == (byte) RR_DATA_REQ) ||
                       (message_id == (byte) RR_STOP_GSM_MODE_REQ)
                      )
                    )
                    ||
                    (
                      (message_set == MS_RRC_RR) &&
                      (
                        (message_id == (byte) RRC_INTERRAT_HANDOVER_CNF) ||
                        (message_id == (byte) RRC_INTERRAT_HANDOVER_ABORT_CNF)
                      )
                    )
                  )
          {
            new_rr_event = rr_g2w_handover_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
          }
          else if ((message_set == MS_RR_L1) &&
                   ((message_id == (byte) MPH_SERVING_DED_MEAS_IND) ||
                    (message_id == (byte) MPH_SURROUND_MEAS_IND)))
          {
            /* May receive these due to race conditions. Ignore these messages */
          }
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_PLMN_SELECT_REQ))
          {
            /* Invalid message while connected; Send CNF with MM_AS_CONNECTED */
            LAI_T temp_lai;

            /* fill this temporary variable with invalid LAC, but valid PLMN ID */
            rr_LAI_with_PLMN_only(
              &temp_lai,
              &(new_message_ptr->rr_plmn_select_req.PLMN_id)
            );

            MSG_GERAN_ERROR_0_G("RR is already connected");
            rr_send_plmn_select_cnf(
              MM_AS_CONNECTED,
              MM_AS_SERVICE_AVAILABLE,
              &temp_lai,
              NULL,
              NULL,
              NULL,
              gas_id
            );
          }
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_PLMN_LIST_REQ))
          {
            MSG_GERAN_HIGH_0_G("Reject RR_PLMN_LIST_REQ during G2W HO");
            rr_reject_msg(new_message_ptr, gas_id);
          }
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_GMM_GPRS_STATE_CHANGE_REQ))
          {
            ;  // no action required
          }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q          
          else if ((message_set == MS_MM_RR) && (message_id == (byte) RR_PSEUDO_LTE_LIST_UPDATE_IND ))
          {
            rr_set_rr_using_pseudo_si2q_data(FALSE, gas_id);
          }  
#endif
          else if (message_set == MS_MAC_RR)
          {
            (void) rr_handle_procedures(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
          }
          else if (message_set == MS_TIMER)
          {
            switch (new_message_ptr->mid_timer_expiry.timer_id)
            {
              case T3122:
              {
                // This is unexpected - normally no CS connection would be attempted if T3122 is running.
                // The exception is an Emergency Call, which could lead to a CS connection established when
                // T3122 expires.
                // No need to notify NAS-MM at this point - it will be notified when the next RR_SERVICE_IND
                // is sent when returning to Idle when the CS connection is released
                MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
                break;
              }

              default:
              {
                LOG_UNEXPECTED_TIMER_EXPIRY(
                  new_message_ptr->mid_timer_expiry.timer_id,
                  rr_main_state_name(rr_control_data_ptr->rr_state)
                );
              }
            }
          } /* MID_TIMER_EXPIRY */
        } /* EV_INPUT_MESSAGE */
        else if ((rr_event == EV_GSM_MODE_ACTIVATED) || (rr_event == EV_GSM_MODE_DEACTIVATED))
        {
          /* Forward this event to GSM to WCDMA Handover SM */
          new_rr_event = rr_g2w_handover_control(rr_event, NULL, gas_id);
        }
        else if (rr_event == EV_RADIO_LINK_FAILURE)
        {
          new_rr_event = EV_RADIO_LINK_FAILURE;
          /* Move to DATA_TRANSFER state and handle the event there */
          rr_control_data_ptr->rr_state = RR_DATA_TRANSFER;
          /* This is to log the transition from RR_G2W_INTERRAT_HANDOVER_PROGRESS
             (through RR_DATA_TRANSFER) to RR_CONNECTION_RELEASE properly */
          rr_control_data_ptr->old_rr_state = RR_DATA_TRANSFER;
        }
        else if (rr_event == RR_EV_G2W_HO_ABORTED)
        {
          rr_ce_rel_ded_proc_details_t ded_proc_details;

          /*Before moving to this state make sure that handle_procedure has been cleared refer to CR554022*/
          (void) rr_handle_procedures(RR_EV_G2W_HO_ABORTED, NULL,gas_id);

          // L1 is left in NULL after aborting a G2X HO, so indicate this when requesting the CS abort
          ded_proc_details.l1_in_null = TRUE;

          rr_ce_send_imsg_cs_abort_req(
            TRUE,                // ded_proc_in_progress
            &ded_proc_details,   // ded_proc_details_ptr
            gas_id               // gas_id
          );

          // Subsequent actions are the same as would be done in dedicated
          rr_control_data_ptr->rr_state = RR_DATA_TRANSFER;
        }
        else if (rr_event == RR_EV_G2W_HO_FAIL)
        {
          new_rr_event = rr_handle_procedures(RR_EV_G2W_HO_FAIL, NULL, gas_id);

          /* Move to DATA_TRANSFER state */
          rr_control_data_ptr->rr_state = RR_DATA_TRANSFER;
        }
        else if (rr_event == RR_EV_G2W_HO_SUCCESS)
        {
          gprs_scell_info_t         * scell_info_ptr = rr_gprs_get_scell_info(gas_id);

          (void) rr_handle_procedures(RR_EV_G2W_HO_SUCCESS, NULL, gas_id);

          /* Inform CB task that CBCH is no longer valid */
          rr_send_cell_change_ind(
            scell_info_ptr->gsm.location_area_identification,
            scell_info_ptr->gsm.cell_identity,
            FALSE,
            gas_id
          );

          rr_send_cb_info(FALSE, gas_id);

          /********************************/
          /* Reset all the state machines */
          /********************************/
          rr_reset_software(gas_id);
          rr_init_geran_stopped(gas_id);

          rr_ps_no_ps_access_del_all(gas_id);

          /* Send back RR_STOP_GSM_MODE_CNF to MM */
          rr_send_stop_gsm_mode_cnf(MM_AS_SUCCESS, gas_id);
          /* Transition to RR_INACTIVE */
          rr_control_data_ptr->rr_state = RR_INACTIVE;
        }
        else
        {
          LOG_UNEXPECTED_EVENT(rr_event_name(rr_event),
                               rr_main_state_name(rr_control_data_ptr->rr_state));
        }
      } /* RR_G2W_INTERRAT_HANDOVER_PROGRESS */
      break;
#endif /* FEATURE_WCDMA */


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
      /***********************************************************************/
      /* RR_W2G_SERVICE_REDIRECTION_IN_PROGRESS                              */
      /*                                                                     */
      /* W->G Service Redirection is in progress                             */
      /***********************************************************************/

      /**
       * RR W2G Service Redirection is in progress.  All messages and events
       * should be forwarded to the Service Redirection state machine.  Exit
       * events for this state are:
       *
       * RR_EV_W2G_GO_TO_INACTIVE
       *
       * This is an indication that RR has failed to find a suitable cell to
       * camp on.  A service redirection reject has been sent to RRC, which
       * will regain control.
       *
       * RR_EV_CAMPED_ON_CELL
       *
       * This is an indication that RR has camped on a suitable cell.  Move to
       * the camped state where there should be an RR_EST_REQ waiting to be
       * processed.
       *
       * EV_PROCESS_SAVED_MESSAGE
       *
       * This is an indication that the procedure has been aborted in some way,
       * e.g. by an RR_STOP_MODE_REQ or RR_DEACT_REQ from MM.  The message will
       * have been saved using rr_save_msg(), so should be processed now (in the
       * RR_INACTIVE state).
       */
      case RR_W2G_SERVICE_REDIRECTION_IN_PROGRESS:
      {
        if (rr_event == EV_INPUT_MESSAGE)
        {
          if (message_set == MS_RRC_RR)
          {
#ifdef FEATURE_WCDMA
            switch (message_id)
            {
              case RR_INTERRAT_REDIRECT_ABORT_REQ:
              {
                rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                new_rr_event = rr_w2g_service_redirection_control(rr_event, new_message_ptr, gas_id);
                break;
              } /* RR_INTERRAT_REDIRECT_ABORT_REQ */

              default:
              {
                new_rr_event = rr_w2g_service_redirection_control(rr_event, new_message_ptr, gas_id);
              }
            }
#endif /* FEATURE_WCDMA */
          }
          else if (message_set == MS_MM_RR)
          {
            if ((mm_rr_message_id_enum_T) message_id == RR_DEACT_REQ ||
                (mm_rr_message_id_enum_T) message_id == RR_STOP_GSM_MODE_REQ ||
                (mm_rr_message_id_enum_T) message_id == RR_PLMN_SELECT_REQ )
            {
              (void)rr_save_msg(new_message_ptr, gas_id);
              new_rr_event = RR_EV_MM_REQ_RECEIVED_ABORT;
            }
            else if ((mm_rr_message_id_enum_T) message_id == RR_PLMN_LIST_REQ)
            {
              if (rr_save_msg(new_message_ptr, gas_id))
              {
                new_rr_event = rr_w2g_service_redirection_control(RR_EV_MM_REQ_RECEIVED_ABORT, NULL, gas_id);
              }
              else
              {
                MSG_GERAN_HIGH_0_G("can't process PLMN search request");
              }
            }
            else if ((mm_rr_message_id_enum_T) message_id == RR_EST_REQ)
            {
              MSG_GERAN_ERROR_0_G("RR_EST_REQ received in RR_W2G_SERVICE_REDIRECTION_IN_PROGRESS, aborting request");
              rr_send_abort_ind(REASON_NOT_AVAILABLE, MM_RR_OTHER_FAILURE, gas_id);
            }
            else if ((mm_rr_message_id_enum_T) message_id == RR_GMM_GPRS_STATE_CHANGE_REQ)
            {
              ;  // no action required
            }
            else
            {
              new_rr_event = rr_w2g_service_redirection_control(rr_event, new_message_ptr, gas_id);
            }
          }
          else if (message_set == MS_RR_RR)
          {
            switch (new_message_ptr->rr.header.rr_message_set)
            {
              case RR_L1_IDLE_IMSG:
              {
                rr_l1_idle_control(new_message_ptr, gas_id);
                break;
              }

              case RR_L1_IDLE_OMSG:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF:
                  {
                    rr_w2g_service_redirection_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(
                      new_message_ptr,
                      rr_main_state_name(rr_control_data_ptr->rr_state)
                    );
                  }
                }
                break;
              }

              case RR_IMSG:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case RR_IMSG_MSC_CHANGE_IND:
                  {
                    /*
                    Update NAS as required
                    RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                    This will be sent when the current procedure completes or G is next active
                    */
                    rr_msc_update(
                      FALSE,   // can_send_service_ind
                      gas_id   // gas_id
                    );
                    break;
                  }

                  case RR_IMSG_PS_ACCESS_REQ:
                  {
                    // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                    rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  case RR_IMSG_NO_PS_ACCESS_REQ:
                  {
                    // No PS access request (could be TLLI unassignment, CS-only)

                    rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                    // Call the call-back function (if one was supplied)
                    CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                    break;
                  }

                  case RR_IMSG_PAGE_IND:
                  {
                    rr_imsg_page_ind_t *imsg = &new_message_ptr->rr.rr.imsg.page_ind;

                    // A page received during cell selection is saved. It is forwarded to NAS when RR/L1
                    // are in Packet Idle
                    rr_ce_save_page(&imsg->page_info, gas_id);
                    break;
                  }

                  case RR_IMSG_PAGE_MODE_IND:
                  {
                    // Changes in OTA page mode are ignored during cell selection
                    // (these may be received after Early Camping)
                    MSG_GERAN_MED_0_G("Page Mode change ignored");
                    break;
                  }

                  default: ; // ignore other RR-IMSG messages
                }
                break;
              }

              default: ; // ignore other RR-RR messages
            }
          }
          else if (message_set == MS_TIMER)
          {
            switch (new_message_ptr->mid_timer_expiry.timer_id)
            {
              case T3122:
              {
                // This is unexpected, but should't cause a malfunction
                // NAS-MM will be notified when the next RR_SERVICE_IND is sent
                MSG_GERAN_ERROR_0_G("Unexpected T3122 expiry during W2G service redirection");
                break;
              }

              default:
              {
                new_rr_event = rr_w2g_service_redirection_control(rr_event, new_message_ptr, gas_id);
              }
            }
          } /* MID_TIMER_EXPIRY */
          else
          {
            new_rr_event = rr_w2g_service_redirection_control(rr_event, new_message_ptr, gas_id);
          }
        }
        else if (rr_event == RR_EV_W2G_GO_TO_INACTIVE)
        {
#ifdef FEATURE_LTE
          /* If RR received Dedicated Priorities from the
          originating RAT then they should be cleared now. */
          rr_individual_priorities_clear(gas_id);
#endif /* FEATURE_LTE */
          rr_control_data_ptr->rr_state = RR_INACTIVE;
        }
        else if (rr_event == EV_GO_TO_IDLE)
        {
#ifdef FEATURE_LTE
          rr_dedicated_priorities_take_ownership(gas_id);
#endif /* FEATURE_LTE */

          rr_grr_send_imsg_camp_on_cell_req(
            TRUE,       // notify_mm_service_ind
            FALSE,      // notify_mm_plmn_select_cnf
            FALSE,      // purge_gmm_signalling
            gas_id      // gas_id
          );

#ifdef FEATURE_IDLE_DRX_SCALING
          if (rr_idle_drx_start_scaling_suspend_timer(gas_id))
          {
            MSG_GERAN_HIGH_0_G("Started the IDLE_DRX_SCALING_SUSPEND TIMER");
          }
#endif /* FEATURE_IDLE_DRX_SCALING */

          rr_control_data_ptr->rr_state = RR_GRR;
        }
        else if (rr_event == EV_PROCESS_SAVED_MESSAGE)
        {
#ifdef FEATURE_LTE
          rr_dedicated_priorities_take_ownership(gas_id);
#endif /* FEATURE_LTE */
          rr_control_data_ptr->rr_state = RR_INACTIVE;
          new_rr_event = rr_process_saved_message(&new_message_ptr, gas_id);
        }
        else if (rr_event == RR_EV_ACQUISITION_FAILURE)
        {
#ifdef FEATURE_LTE
          /* As NAS is made aware of the RAT change, RR should take ownership
          of any dedicated priorities that are configured. */
          rr_dedicated_priorities_take_ownership(gas_id);
#endif /* FEATURE_LTE */

          /* This event is generated if a cell is chosen and RR attempts to
          camp on it (and sends RR_ACTIVATION_IND to MM) but subsequently fails
          to camp on the cell.  MM must be informed of RR being out of service
          so that it generates an RR_PLMN_SELECT_REQ. */
          rr_send_no_service_ind(gas_id);

          /* Deactivate to keep RR/L1 in sync - setting the reason to STOP CAMPING will */
          /* mean no messages are sent to MM */
          rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
          new_rr_event = rr_trigger_gsm_as_deactivation(
            RR_DEACT_REASON_STOP_CAMPING,
            gas_id
          );
        }
        else if (rr_event == EV_PANIC_RESET)
        {
          /* Clear rr_event here as at the end of this state machine EV_PANIC_RESET is
          handled in a stateless manner - we do not want this to happen until
          X2G Redir STM has confirmed it has handled the panic reset event correctly. */
          rr_event = EV_NO_EVENT;
          new_rr_event = rr_w2g_service_redirection_control(EV_PANIC_RESET_REQ, NULL, gas_id);
        }
        else if (rr_event == EV_PANIC_RESET_CNF)
        {
          /* Now that X2G Redir STM has confirmed that it has handled the
          panic reset we can set rr_event back to EV_PANIC_RESET to be
          handled at the end of this state machine. */
          rr_event = EV_PANIC_RESET;
        }
        else
        {
          new_rr_event = rr_w2g_service_redirection_control(rr_event, new_message_ptr, gas_id);
        }
      }
      break;

#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

      case RR_RESET:
      {
        if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
        {
          // Note: MM messages are blocked in RR_RESET state - see rr_can_process_mm_message()

          if (message_set == MS_RR_RR)
          {
            switch (new_message_ptr->rr.header.rr_message_set)
            {
              case RR_IMSG:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case RR_IMSG_MSC_CHANGE_IND:
                  {
                    /*
                    Update NAS as required
                    RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                    This will be sent when RR/L1 enters Idle
                    */
                    rr_msc_update(
                      FALSE,   // can_send_service_ind
                      gas_id   // gas_id
                    );
                    break;
                  }

                  default: ; // ignore other RR-IMSG messages
                }
                break;
              }

              default: ; // ignore other RR-RR messages
            }
            break;
          }
          else
          if ( (message_set == MS_RR_L1) && (message_id == (byte) MPH_RESET_CNF) )
          {
            new_rr_event = rr_mode_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
          }
          else
          if ( (message_set == MS_RR_L1) && (message_id == (byte) MPH_RESET_IND) )
          {
            new_rr_event = rr_mode_control(EV_GSM_MODE_DEACTIVATED, NULL, gas_id);
          }
          else
          if ( (message_set == MS_MAC_RR) && (message_id == (byte) MAC_GRR_RESET_CNF) )
          {
            MSG_GERAN_HIGH_0_G("MAC/RLC reset confirmed");
#ifdef FEATURE_WCDMA
            if (rr_verify_panic_reset_saved_message_set(MS_RRC_RR, gas_id) ||
                rr_verify_panic_reset_saved_message_set(MS_TDSRRC_RR, gas_id) ||
                rr_verify_panic_reset_saved_message_set(MS_MSGR_RR, gas_id) ||
                (rr_get_panic_reset_x2g_cco_flag(gas_id) != RR_X2G_RESEL_TYPE_UNDEFINED))
            {
              MSG_GERAN_HIGH_0_G("Panic reset during X2G, restarting GL1 to stop it for RAT change");
              (void) rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
              new_rr_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
            }
            else
#endif /* FEATURE_WCDMA */
            {
              new_rr_event = EV_GSM_MODE_DEACTIVATED;
            }
            rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
          }
          else
          {
            MSG_GERAN_HIGH_2_G(
              "Message (%d,%d) ignored during lower layer recovery",
              (int) message_set,
              (int) message_id
            );
          }
        }
        else
        if (rr_event == EV_GSM_MODE_DEACTIVATED)
        {
          if (rr_send_grr_mac_reset_req(gas_id) == FALSE)
          {
            rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
            new_rr_event = EV_GSM_MODE_DEACTIVATED;
          }
        }
        else
        {
          MSG_GERAN_HIGH_1_G(
            "Event (%d) ignored during lower layer recovery",
            (int) rr_event
          );
        }
      }  /* RR_RESET state */
      break;


#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
      /**
      State: RR_X2G_RESEL

      This state is entered when a reselection to GERAN from another RAT is
      being performed.

      The rr_x2g_resel_control() state machine is responsible for most actions
      during this procedure.

      If the procedure is aborted by the other RAT or by NAS then the state
      is changed to RR_X2G_RESEL_ABORTED.
      */
      case RR_X2G_RESEL:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (new_message_ptr->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (new_message_ptr->rr.header.rr_message_set)
                {
                  case RR_IMSG:
                  {
                    switch (new_message_ptr->message_header.message_id)
                    {
                      case RR_IMSG_MSC_CHANGE_IND:
                      {
                        /*
                        Update NAS as required
                        RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                        This will be sent when the current procedure completes or G is next active
                        */
                        rr_msc_update(
                          FALSE,   // can_send_service_ind
                          gas_id   // gas_id
                        );
                        break;
                      }

                      case RR_IMSG_PS_ACCESS_REQ:
                      {
                        // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                        rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      case RR_IMSG_NO_PS_ACCESS_REQ:
                      {
                        // No PS access request (could be TLLI unassignment, CS-only)

                        rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      case RR_IMSG_PAGE_IND:
                      {
                        rr_imsg_page_ind_t *imsg = &new_message_ptr->rr.rr.imsg.page_ind;

                        // A page received during cell reselection is saved. It is forwarded to NAS when RR/L1
                        // are in Packet Idle
                        rr_ce_save_page(&imsg->page_info, gas_id);
                        break;
                      }

                      case RR_IMSG_PAGE_MODE_IND:
                      {
                        // Changes in OTA page mode are ignored during cell reselection
                        // (these may be received after Early Camping)
                        MSG_GERAN_MED_0_G("Page Mode change ignored");
                        break;
                      }

                      default:
                      {
                        rr_x2g_resel_control(new_message_ptr, gas_id);
                      }
                    }
                    break;
                  }

                  case RR_X2G_RESEL_OMSG:
                  {
                    switch (new_message_ptr->message_header.message_id)
                    {
                      case RR_X2G_RESEL_OMSG_RESEL_CNF:
                      {
                        rr_x2g_resel_status_e resel_status;

                        resel_status = new_message_ptr->rr.x2g_resel.omsg.resel_cnf.status;

                        if (resel_status == RR_X2G_RESEL_STATUS_SUCCESS)
                        {
                          if (rr_x2g_cco_in_progress(gas_id))
                          {
                            /* Set the flag in RR panic reset data storage that indicates
                            that a W2G CCO procedure is ongoing. */
                            rr_set_panic_reset_x2g_cco_flag(rr_control_data_ptr->x2g_resel_type, gas_id);

                            /* Also flag that an RR_ACTIVATION_IND has been sent, so that
                            we can inform X-RRC of this if need be. */
                            rr_control_data_ptr->rr_panic_reset_info.cco_activation_ind_sent = TRUE;

                            /* Propagate the X2G CCO Network Control Order to PCCO params if required. */
                            if (rr_control_data_ptr->x2g_cco_pending_network_control_order_valid)
                            {
                              rr_gprs_meas_rpt_control_handle_l2g_cco_network_control_order(
                                rr_control_data_ptr->x2g_cco_pending_network_control_order,
                                gas_id
                              );
                              rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
                            }

#ifdef FEATURE_LTE
                            if (rr_dedicated_priority_info_valid(&rr_control_data_ptr->dedicated_priority_info))
                            {
                              /* Need to use the pending cell ARFCN here as
                              idle mode has not been started yet.  Dedicated
                              priority information is stored at this time, but
                              ownership of the store is not taken until the CCO
                              completes successfully. */
                              gprs_scell_info_t * pcell = rr_gprs_get_pcell_info(gas_id);
                              ARFCN_T arfcn = pcell->gsm.BCCH_ARFCN;
                              rr_PLMN_request_details_T * rr_PLMN_request_details_ptr =
                                rr_get_PLMN_request_details_ptr(gas_id);

                              rr_dedicated_priority_info_store(
                                &rr_control_data_ptr->dedicated_priority_info,
                                &arfcn,
                                rr_PLMN_request_details_ptr->requested_PLMN_id,
                                gas_id
                              );
                            }
#endif /* FEATURE_LTE */
                          }
                          else
                          {
#ifdef FEATURE_LTE
                            rr_dedicated_priorities_take_ownership(gas_id);
#endif /* FEATURE_LTE */

                            /* These variables need to remain set if an X2G CCO is
                            in progress, but if the current procedure is a Reselection
                            then they should be reset here. */
                            MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
                            rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                            rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                          }

                          rr_grr_send_imsg_camp_on_cell_req(
                            TRUE,       // notify_mm_service_ind
                            FALSE,      // notify_mm_plmn_select_cnf
                            FALSE,      // purge_gmm_signalling
                            gas_id      // gas_id
                          );

#ifdef FEATURE_IDLE_DRX_SCALING
                          if (rr_idle_drx_start_scaling_suspend_timer(gas_id))
                          {
                            MSG_GERAN_HIGH_0_G("Started the IDLE_DRX_SCALING_SUSPEND TIMER");
                          }
#endif /* FEATURE_IDLE_DRX_SCALING */

                          rr_control_data_ptr->rr_state = RR_GRR;
                        }
                        else
                        {
                          /* The procedure failed.  Send response to the
                          appropriate calling RAT and return to INACTIVE state. */
                          switch (rr_control_data_ptr->x2g_resel_type)
                          {
#ifdef FEATURE_WCDMA
                            case RR_X2G_RESEL_TYPE_W2G_RESELECTION:
                            {
                              rr_send_interrat_reselection_rej(
                                rr_w2g_reselect_failure_reason_map(resel_status),
                                rr_w2g_reselect_failure_time_to_reselect_map(resel_status),
                                gas_id
                              );
                              break;
                            } /* RR_X2G_RESEL_TYPE_W2G_RESELECTION */

                            case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
                            {
                              (void)rr_timer_cancel(T3174, gas_id);

                              if (new_message_ptr->rr.x2g_resel.omsg.resel_cnf.status == RR_X2G_RESEL_STATUS_FAILURE_CELL_PLMN_MISMATCH)
                              {
                                rr_send_interrat_cc_order_cnf(
                                  CCO_FAILURE_AFTER_ACT_IND,
                                  rr_x2g_map_resel_status_to_w2g_cco_status(resel_status),
                                  gas_id
                                );
                              }
                              else
                              {
                                rr_send_interrat_cc_order_cnf(
                                  CCO_FAILURE_BEFORE_ACT_IND,
                                  rr_x2g_map_resel_status_to_w2g_cco_status(resel_status),
                                  gas_id
                                );
                              }
                              break;
                            } /* RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                            case RR_X2G_RESEL_TYPE_T2G_RESELECTION:
                            {
                              rr_send_tds_interrat_reselection_rej(
                                rr_t2g_reselect_failure_reason_map(resel_status),
                                rr_t2g_reselect_failure_time_to_reselect_map(resel_status),
                                gas_id
                              );
                              break;
                            }

                            case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                            {
                              (void)rr_timer_cancel(T3174, gas_id);

                              if (new_message_ptr->rr.x2g_resel.omsg.resel_cnf.status == RR_X2G_RESEL_STATUS_FAILURE_CELL_PLMN_MISMATCH)
                              {
                                rr_send_tds_interrat_cc_order_cnf(
                                  TDSCCO_FAILURE_AFTER_ACT_IND,
                                  rr_x2g_map_resel_status_to_t2g_cco_status(resel_status),
                                  gas_id
                                );
                              }
                              else
                              {
                                rr_send_tds_interrat_cc_order_cnf(
                                  TDSCCO_FAILURE_BEFORE_ACT_IND,
                                  rr_x2g_map_resel_status_to_t2g_cco_status(resel_status),
                                  gas_id
                                );
                              }
                              break;
                            }
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
                            case RR_X2G_RESEL_TYPE_L2G_RESELECTION:
                            {
                              geran_grr_lte_resel_failed_rsp_t lte_resel_failed_rsp;
                              errno_enum_type                  result;

                              memset(&lte_resel_failed_rsp, 0, sizeof(lte_resel_failed_rsp));

                              msgr_init_hdr(
                                &lte_resel_failed_rsp.hdr,
                                MSGR_GERAN_GRR,
                                GERAN_GRR_LTE_RESEL_FAILED_RSP
                              );
                              lte_resel_failed_rsp.resel_fail_cause = rr_l2g_resel_failure_cause_map(resel_status);
                              result = rr_msgr_send(&lte_resel_failed_rsp.hdr, sizeof(lte_resel_failed_rsp), gas_id);
                              if (result != E_SUCCESS)
                              {
                                MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                              }
                              break;
                            }

                            case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                            {
                              geran_grr_lte_cco_rsp_t lte_cco_rsp;
                              errno_enum_type         result;

                              memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

                              (void)rr_timer_cancel(T3174, gas_id);

                              msgr_init_hdr(
                                &lte_cco_rsp.hdr,
                                MSGR_GERAN_GRR,
                                GERAN_GRR_LTE_CCO_RSP
                              );
                              lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_ACQUIRE;
                              result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
                              if (result != E_SUCCESS)
                              {
                                MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                              }
                              break;
                            }
#endif /* FEATURE_LTE */

                            default:
                            {
                              MSG_GERAN_ERROR_1_G(
                                "Unexpected x2g_resel_type: %d",
                                rr_control_data_ptr->x2g_resel_type
                              );
                            }
                          }

#ifdef FEATURE_LTE
                          /* If RR received Dedicated Priorities from the
                          originating RAT then they should be cleared now. */
                          rr_individual_priorities_clear(gas_id);
#endif /* FEATURE_LTE */

                          MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
                          rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                          rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                          rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
                          rr_control_data_ptr->is_cco_for_csfb = FALSE;
                          rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
                          rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;
                          rr_l1_idle_init(gas_id);
                          rr_control_data_ptr->rr_state = RR_INACTIVE;
                        }
                        rr_x2g_resel_deinit(gas_id);
                        break;
                      } /* RR_X2G_RESEL_OMSG_RESEL_CNF */

                      case RR_X2G_RESEL_OMSG_PANIC_RESET_CNF:
                      {
                        if (rr_x2g_cco_in_progress(gas_id))
                        {
                          /* Set the flag in RR panic reset data storage that indicates
                          that a W2G CCO procedure is ongoing. */
                          rr_set_panic_reset_x2g_cco_flag(rr_control_data_ptr->x2g_resel_type, gas_id);

                          if (new_message_ptr->rr.x2g_resel.omsg.panic_reset_cnf.activation_ind_sent)
                          {
                            rr_control_data_ptr->rr_panic_reset_info.cco_activation_ind_sent = TRUE;
                          }
                          else
                          {
                            rr_control_data_ptr->rr_panic_reset_info.cco_activation_ind_sent = FALSE;
                          }
                        }
                        rr_x2g_resel_deinit(gas_id);
                        rr_event = EV_PANIC_RESET;
                        break;
                      } /* RR_X2G_RESEL_OMSG_PANIC_RESET_CNF */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_RESEL");
                      }
                    }
                    break;
                  } /* RR_X2G_RESEL_OMSG */

                  default:
                  {
                    rr_x2g_resel_control(new_message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_RR_RR */

#ifdef FEATURE_GSM_TDS
              case MS_TDSRRC_RR:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case TDSRR_INTERRAT_RESELECTION_ABORT_REQ:
                  {
                    rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                    rr_x2g_resel_send_imsg_abort_req(RR_L1_RAT_CHANGE, gas_id);
                    rr_control_data_ptr->rr_state = RR_X2G_RESEL_ABORTED;
                    rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_IRAT;
                    break;
                  }
                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_RESEL");
                  }
                }
                break;
              }
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_WCDMA
              case MS_RRC_RR:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case RR_INTERRAT_RESELECTION_ABORT_REQ:
                  {
                    rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                    rr_x2g_resel_send_imsg_abort_req(RR_L1_RAT_CHANGE, gas_id);
                    rr_control_data_ptr->rr_state = RR_X2G_RESEL_ABORTED;
                    rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_IRAT;
                    break;
                  } /* RR_INTERRAT_RESELECTION_ABORT_REQ */

		          case RR_INTERRAT_CC_ORDER_ABORT_REQ:
                  {
                    rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                    rr_x2g_resel_send_imsg_abort_req(RR_L1_RAT_CHANGE, gas_id);
                    rr_control_data_ptr->rr_state = RR_X2G_RESEL_ABORTED;
                    rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_CCO_ABORT;
                    break;
                  } /* RR_INTERRAT_CC_ORDER_ABORT_REQ */


                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_RESEL");
                  }
                }
                break;
              } /* MS_RRC_RR */
#endif /* FEATURE_WCDMA */

              case MS_MSGR_RR:
              {
                switch (new_message_ptr->rr_msgr_msg.message.hdr.id)
                {
#ifdef FEATURE_LTE
                  case GERAN_GRR_LTE_ABORT_RESEL_REQ:
                  {
                    rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                    ASSERT(rr_control_data_ptr->x2g_resel_type == RR_X2G_RESEL_TYPE_L2G_RESELECTION);
                    rr_x2g_resel_send_imsg_abort_req(RR_L1_RAT_CHANGE, gas_id);
                    rr_control_data_ptr->rr_state = RR_X2G_RESEL_ABORTED;
                    rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_IRAT;
                    break;
                  } /* GERAN_GRR_RESEL_ABORT_REQ */

                  case GERAN_GRR_LTE_ABORT_CCO_REQ:
                  {
                    rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                    ASSERT(rr_control_data_ptr->x2g_resel_type == RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER);
                    (void)rr_timer_cancel(T3174, gas_id);
                    rr_x2g_resel_send_imsg_abort_req(RR_L1_RAT_CHANGE, gas_id);
                    rr_control_data_ptr->rr_state = RR_X2G_RESEL_ABORTED;
                    rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_IRAT;
                    break;
                  } /* GERAN_GRR_LTE_ABORT_CCO_REQ */
#endif /* FEATURE_LTE */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_RESEL");
                  }
                }
                break;
              }

              case MS_MM_RR:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case RR_DEACT_REQ:
                  case RR_STOP_GSM_MODE_REQ:
                  case RR_PLMN_SELECT_REQ:
                  {
                    /* Abort the reselection to handle the request */
                    (void) rr_save_msg(new_message_ptr, gas_id);

                    if (rr_x2g_cco_in_progress(gas_id))
                    {
                      (void) rr_timer_cancel(T3174, gas_id);
                    }

                    /* RR_DEACT_REQ / RR_STOP_GSM_MODE_REQ are pre-processed
                    statelessly before reaching this state handler.  This
                    will have caused the L1 stop reason to have been set up
                    already, so we pass that on to x2g_resel so that it can
                    send the appropriate value to L1. */
                    rr_x2g_resel_send_imsg_abort_req(rr_l1_gsm_stop_reason_get(gas_id), gas_id);
                    rr_control_data_ptr->rr_state = RR_X2G_RESEL_ABORTED;
                    rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_NAS;
                    break;
                  }

                  case RR_PLMN_LIST_REQ:
                  {
                    rr_plmn_list_req_T * rr_plmn_list_req = &new_message_ptr->rr_plmn_list_req;

                    if (rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
                    {
                      /* This is an automatic (background HPLMN) search, so abort the
                      request - it will be retried by NAS after 10 seconds. */
                      rr_reject_msg(new_message_ptr, gas_id);
                    }
                    else
                    {
                      /* This is a manual PLMN list request, so abort the reselection and
                      process it in the foreground. */
                      if (rr_save_msg(new_message_ptr, gas_id))
                      {
                        if (rr_x2g_cco_in_progress(gas_id))
                        {
                          (void) rr_timer_cancel(T3174, gas_id);
                        }

                        rr_x2g_resel_send_imsg_abort_req(RR_L1_RAT_CHANGE, gas_id);
                        rr_control_data_ptr->rr_state = RR_X2G_RESEL_ABORTED;
                        rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_NAS;
                      }
                    }
                    break;
                  }

                  case RR_ABORT_REQ:
                  {
                    /* No RR connection exists, send confirm back to keep NAS
                    happy. */
                    rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);
                    break;
                  }

                  case RR_EST_REQ:
                  {
                    MSG_GERAN_ERROR_0_G("RR_EST_REQ received in RR_X2G_RESEL, aborting request");
                    rr_send_abort_ind(REASON_NOT_AVAILABLE, MM_RR_OTHER_FAILURE, gas_id);
                    break;
                  }

                  case RR_ACTIVATION_RSP:
                  {
                    rr_x2g_resel_control(new_message_ptr, gas_id);
                    break;
                  }

                  case RR_GMM_GPRS_STATE_CHANGE_REQ:
                  {
                    break;  // no action required
                  }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q                  
                  case RR_PSEUDO_LTE_LIST_UPDATE_IND:
                  {
                    break;  // Ignore this message while X2G Resel is ongoing
                  }
#endif
                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_RESEL");
                  }
                }
                break;
              } /* MS_MM_RR */

              case MS_TIMER:
              {
                switch (new_message_ptr->mid_timer_expiry.timer_id)
                {
                  case T3122:
                  {
                    // This is unexpected as the T3122 is stopped when the G-RAT is deactivated
                    MSG_GERAN_ERROR_0_G("Unexpected T3122 expiry during X2G resel");
                    break;
                  } /* T3122 */

                  case T3174:
                  {
                    ASSERT(rr_x2g_cco_in_progress(gas_id));

                    rr_x2g_resel_send_imsg_abort_req(RR_L1_RAT_CHANGE, gas_id);
                    rr_control_data_ptr->rr_state = RR_X2G_RESEL_ABORTED;
                    rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_CCO_TIMER_EXPIRY;
                    break;
                  } /* T3174 */

                  default:
                  {
                    rr_x2g_resel_control(new_message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_TIMER */

              default:
              {
                rr_x2g_resel_control(new_message_ptr, gas_id);
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_GSM_MODE_ACTIVATED:
          {
            rr_x2g_resel_send_imsg(RR_X2G_RESEL_IMSG_L1_ACTIVATED_IND, gas_id);
            break;
          } /* EV_GSM_MODE_ACTIVATED */

          case EV_GSM_MODE_DEACTIVATED:
          {
            rr_x2g_resel_send_imsg(RR_X2G_RESEL_IMSG_L1_DEACTIVATED_IND, gas_id);
            break;
          } /* EV_GSM_MODE_DEACTIVATED */

          case EV_PANIC_RESET:
          {
            rr_x2g_resel_send_imsg(RR_X2G_RESEL_IMSG_PANIC_RESET_REQ, gas_id);
            /* rr_event will be re-set to EV_PANIC_RESET when rr_x2g_resel_control()
            indicates that it is done. */
            rr_event = EV_NO_EVENT;
            break;
          }

          case RR_EV_EARLY_CAMP_SI_RECEIVED:
          case RR_EV_GSM_SI_RECEIVED:
          case RR_EV_GPRS_SI13_RECEIVED:
          case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
          case RR_EV_MEASUREMENT_PARAMS_UPDATED:
          {
            /* Legacy events, no action necessary. */
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(rr_event), "RR_X2G_RESEL");
          }
        }
        break;
      } /* RR_X2G_RESEL */

      /**
      State: RR_X2G_RESEL_ABORTED
      */
      case RR_X2G_RESEL_ABORTED:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (new_message_ptr->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (new_message_ptr->rr.header.rr_message_set)
                {
                  case RR_IMSG:
                  {
                    switch (new_message_ptr->message_header.message_id)
                    {
                      case RR_IMSG_MSC_CHANGE_IND:
                      {
                        /*
                        Update NAS as required
                        RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                        This will be sent when G is next active
                        */
                        rr_msc_update(
                          FALSE,   // can_send_service_ind
                          gas_id   // gas_id
                        );
                        break;
                      }

                      case RR_IMSG_PS_ACCESS_REQ:
                      {
                        // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                        rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      case RR_IMSG_NO_PS_ACCESS_REQ:
                      {
                        // No PS access request (could be TLLI unassignment, CS-only)

                        rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      default:
                      {
                        rr_x2g_resel_control(new_message_ptr, gas_id);
                      }
                    }
                    break;
                  }

                  case RR_X2G_RESEL_OMSG:
                  {
                    switch (new_message_ptr->message_header.message_id)
                    {
                      case RR_X2G_RESEL_OMSG_ABORT_CNF:
                      {
                        switch (rr_control_data_ptr->x2g_resel_abort_cause)
                        {
                          case RR_X2G_RESEL_ABORT_CAUSE_IRAT:
                          {
                            /* X2G Reselection / CCO procedure was aborted
                            after receiving an abort request from the
                            initiating RAT.  L1 is shut down by
                            rr_x2g_resel_control() so all that happens now
                            is rr_control() state is reset to INACTIVE, any
                            X2G state variables are reset and RR sends an
                            abort response to the initiating RAT. */
                            switch (rr_control_data_ptr->x2g_resel_type)
                            {
#ifdef FEATURE_WCDMA
                              case RR_X2G_RESEL_TYPE_W2G_RESELECTION:
                              {
                                rr_send_interrat_reselection_abort_cnf(gas_id);
                                break;
                              } /* RR_X2G_RESEL_TYPE_W2G_RESELECTION */

                              case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
                              {
                                rr_send_interrat_cc_order_abort_cnf(RRC_RR_SUCCESS, gas_id);
                                break;
                              } /* RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                              case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                              {
                                rr_send_tds_interrat_cc_order_abort_cnf(TDSRRC_RR_SUCCESS, gas_id);
                                break;
                              } /* RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER */

                              case RR_X2G_RESEL_TYPE_T2G_RESELECTION:
                              {
                                rr_send_tds_interrat_reselection_abort_cnf(gas_id);
                                break;
                              } /* RR_X2G_RESEL_TYPE_W2G_RESELECTION */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
                              case RR_X2G_RESEL_TYPE_L2G_RESELECTION:
                              {
                                msgr_hdr_struct_type lte_abort_resel_rsp;
                                errno_enum_type      result;

                                memset(&lte_abort_resel_rsp, 0, sizeof(lte_abort_resel_rsp));

                                msgr_init_hdr(
                                  &lte_abort_resel_rsp,
                                  MSGR_GERAN_GRR,
                                  GERAN_GRR_LTE_ABORT_RESEL_RSP
                                );
                                result = rr_msgr_send(&lte_abort_resel_rsp, sizeof(lte_abort_resel_rsp), gas_id);
                                if (result != E_SUCCESS)
                                {
                                  MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                                }
                                break;
                              } /* RR_X2G_RESEL_TYPE_L2G_RESELECTION */

                              case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                              {
                                msgr_hdr_struct_type lte_abort_cco_rsp;
                                errno_enum_type      result;

                                memset(&lte_abort_cco_rsp, 0, sizeof(lte_abort_cco_rsp));

                                msgr_init_hdr(
                                  &lte_abort_cco_rsp,
                                  MSGR_GERAN_GRR,
                                  GERAN_GRR_LTE_ABORT_CCO_RSP
                                );
                                result = rr_msgr_send(&lte_abort_cco_rsp, sizeof(lte_abort_cco_rsp), gas_id);
                                if (result != E_SUCCESS)
                                {
                                  MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                                }
                                break;
                              } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_LTE */

                              default:
                              {
                                MSG_GERAN_ERROR_1_G(
                                  "Unexpected x2g_resel_type: %d",
                                  rr_control_data_ptr->x2g_resel_type
                                );
                              }
                            }
                            rr_l1_idle_init(gas_id);
                            rr_control_data_ptr->rr_state = RR_INACTIVE;
                            break;
                          } /* RR_X2G_RESEL_ABORT_CAUSE_IRAT */

                          case RR_X2G_RESEL_ABORT_CAUSE_NAS:
                          {
                            if (rr_x2g_cco_in_progress(gas_id))
                            {
                              switch (rr_control_data_ptr->x2g_resel_type)
                              {
#ifdef FEATURE_WCDMA
                                case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
                                {
                                  rr_send_interrat_cc_order_cnf(
                                    CCO_SUCCESS,
                                    RR_GSM_CCO_OTHER_REASON,
                                    gas_id
                                  );

                                  /* WCDMA does not send an indication when
                                  shutdown is completed, so we just send one
                                  to ourselves here to allow things to move
                                  on in RR. */
                                  rr_send_imsg(RR_IMSG_X2G_CCO_COMPLETE_IND, gas_id);
                                  break;
                                } /* RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                                case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                                {
                                  rr_send_tds_interrat_cc_order_cnf(
                                    TDSCCO_SUCCESS,
                                    TDSRR_GSM_CCO_OTHER_REASON,
                                    gas_id
                                  );

                                  rr_control_data_ptr->waiting_for_tdsrr_interrat_tl1_stop_ind = TRUE;
                                  break;
                                } /* RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
                                case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                                {
                                  geran_grr_lte_cco_rsp_t lte_cco_rsp;
                                  errno_enum_type         result;

                                  memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

                                  msgr_init_hdr(
                                    &lte_cco_rsp.hdr,
                                    MSGR_GERAN_GRR,
                                    GERAN_GRR_LTE_CCO_RSP
                                  );

                                  lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_ABORTED_DUE_TO_HIGH_PRI_REQ;

                                  result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
                                  if (result != E_SUCCESS)
                                  {
                                    MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                                  }
                                  rr_control_data_ptr->waiting_for_lte_rrc_cco_nacc_completed_ind = TRUE;
                                  break;
                                } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_LTE */

                                default:
                                {
                                  MSG_GERAN_ERROR_1_G(
                                    "Unexpected x2g_resel_type: %d",
                                    rr_control_data_ptr->x2g_resel_type
                                  );
                                }
                              }

                              /* Switch state and await CCO complete indication. */
                              rr_control_data_ptr->rr_state = RR_X2G_CCO_ABORTED;
                            }
                            else
                            {
                              new_rr_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                              rr_control_data_ptr->rr_state = RR_GOING_ACTIVE;
                            }
                            break;
                          } /* RR_X2G_RESEL_ABORT_CAUSE_NAS */

                          /* In the case of CCO timer expiry, it is important
                          for the initiating RAT to be informed of whether RR
                          has sent an RR_ACTIVATION_IND to NAS or not. */
                          case RR_X2G_RESEL_ABORT_CAUSE_CCO_TIMER_EXPIRY:
                          {
                            ASSERT(rr_x2g_cco_in_progress(gas_id));

                            switch (rr_control_data_ptr->x2g_resel_type)
                            {
#ifdef FEATURE_WCDMA
                              case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
                              {
                                if (rr_get_x2g_cco_act_rsp_rcvd(gas_id))
                                {
                                  rr_send_interrat_cc_order_cnf(
                                    CCO_FAILURE_AFTER_ACT_IND,
                                    RR_GSM_CCO_T309_EXPIRED,
                                    gas_id
                                  );
                                }
                                else
                                {
                                  rr_send_interrat_cc_order_cnf(
                                    CCO_FAILURE_BEFORE_ACT_IND,
                                    RR_GSM_CCO_T309_EXPIRED,
                                    gas_id
                                  );
                                }
                                rr_set_x2g_cco_act_rsp_rcvd(FALSE, gas_id);
                                break;
                              } /* RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                              case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                              {
                                if (rr_get_x2g_cco_act_rsp_rcvd(gas_id))
                                {
                                  rr_send_tds_interrat_cc_order_cnf(
                                    TDSCCO_FAILURE_AFTER_ACT_IND,
                                    TDSRR_GSM_CCO_T309_EXPIRED,
                                    gas_id
                                  );
                                }
                                else
                                {
                                  rr_send_tds_interrat_cc_order_cnf(
                                    TDSCCO_FAILURE_BEFORE_ACT_IND,
                                    TDSRR_GSM_CCO_T309_EXPIRED,
                                    gas_id
                                  );
                                }
                                rr_set_x2g_cco_act_rsp_rcvd(FALSE, gas_id);
                                break;
                              } /* RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
                              case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                              {
                                geran_grr_lte_cco_rsp_t lte_cco_rsp;
                                errno_enum_type         result;

                                memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

                                msgr_init_hdr(
                                  &lte_cco_rsp.hdr,
                                  MSGR_GERAN_GRR,
                                  GERAN_GRR_LTE_CCO_RSP
                                );

                                if (rr_get_x2g_cco_act_rsp_rcvd(gas_id))
                                {
                                  lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_RACH;
                                }
                                else
                                {
                                  lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_ACQUIRE;
                                }

                                rr_set_x2g_cco_act_rsp_rcvd(FALSE, gas_id);
                                result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
                                if (result != E_SUCCESS)
                                {
                                  MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                                }
                                break;
                              } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_LTE */

                              default:
                              {
                                MSG_GERAN_ERROR_1_G(
                                  "Unexpected x2g_resel_type: %d",
                                  rr_control_data_ptr->x2g_resel_type
                                );
                              }
                            }

#ifdef FEATURE_LTE
                            rr_individual_priorities_clear(gas_id);
#endif /* FEATURE_LTE */
                            rr_l1_idle_init(gas_id);
                            rr_control_data_ptr->rr_state = RR_INACTIVE;
                            break;
                          } /* RR_X2G_RESEL_ABORT_CAUSE_CCO_TIMER_EXPIRY */


                          case  RR_X2G_RESEL_ABORT_CAUSE_CCO_ABORT:
                          {
#ifdef FEATURE_WCDMA
                            if (new_message_ptr->rr.x2g_resel.omsg.abort_cnf.activation_ind_sent)
                            {
                               MSG_HIGH("CCO ABORT CNF sent ", 0, 0, 0);
                               rr_send_interrat_cc_order_cnf(CCO_FAILURE_AFTER_ACT_IND, RR_GSM_CCO_OTHER_REASON,gas_id);
                            }
                            else
                            {
                               MSG_HIGH("CCO ABORT CNF before act ind is sent ", 0, 0, 0);  
                               rr_send_interrat_cc_order_cnf(CCO_FAILURE_BEFORE_ACT_IND, RR_GSM_CCO_OTHER_REASON,gas_id);                         
                            }
                            rr_control_data_ptr->rr_state = RR_INACTIVE;
#endif /* FEATURE_WCDMA */
                            break; 
                          }

                          default:
                          {
                            MSG_GERAN_ERROR_1_G(
                              "Unexpected X2G abort cause: %d",
                              rr_control_data_ptr->x2g_resel_abort_cause
                            );
                          }
                        }

                        MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
                        rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                        rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                        rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
                        rr_control_data_ptr->is_cco_for_csfb = FALSE;
                        rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
                        rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;
                        rr_l1_idle_init(gas_id);
                        rr_x2g_resel_deinit(gas_id);
                        break;
                      } /* RR_X2G_RESEL_OMSG_ABORT_CNF */

                      case RR_X2G_RESEL_OMSG_PANIC_RESET_CNF:
                      {
                        if (rr_x2g_cco_in_progress(gas_id))
                        {
                          /* Set the flag in RR panic reset data storage that indicates
                          that a W2G CCO procedure is ongoing. */
                          rr_set_panic_reset_x2g_cco_flag(rr_control_data_ptr->x2g_resel_type,gas_id);

                          if (new_message_ptr->rr.x2g_resel.omsg.panic_reset_cnf.activation_ind_sent)
                          {
                            rr_control_data_ptr->rr_panic_reset_info.cco_activation_ind_sent = TRUE;
                          }
                          else
                          {
                            rr_control_data_ptr->rr_panic_reset_info.cco_activation_ind_sent = FALSE;
                          }
                        }
                        rr_x2g_resel_deinit(gas_id);
                        rr_event = EV_PANIC_RESET;
                        break;
                      } /* RR_X2G_RESEL_OMSG_PANIC_RESET_CNF */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_RESEL_ABORTED");
                      }
                    }
                    break;
                  } /* RR_X2G_RESEL_OMSG */

                  default:
                  {
                    rr_x2g_resel_control(new_message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_TIMER:
              {
                switch (new_message_ptr->mid_timer_expiry.timer_id)
                {
                  case T3122:
                  {
                    // This is unexpected as the T3122 is stopped when the G-RAT is deactivated
                    MSG_GERAN_ERROR_0_G("Unexpected T3122 expiry during X2G resel");
                    break;
                  }

                  default:
                  {
                    rr_x2g_resel_control(new_message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_TIMER */

              case MS_MM_RR:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case RR_ACTIVATION_RSP:
                  {
                    /* May be received if the X2G Reselection / CCO procedure
                    is aborted due to T3174 expiry after sending
                    RR_ACTIVATION_IND to NAS.  No action necessary as control
                    will shortly be returned to the initiating RAT. */
                    break;
                  } /* RR_ACTIVATION_RSP */

                  case RR_GMM_GPRS_STATE_CHANGE_REQ:
                  {
                    break;  // no action required
                  }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q                  
                  case RR_PSEUDO_LTE_LIST_UPDATE_IND:
                  {
                    break;  // Ignore this message while X2G Resel ABortion is ongoing
                  }                
#endif                  
                }
                break;
              } /* MS_MM_RR */

              default:
              {
                rr_x2g_resel_control(new_message_ptr, gas_id);
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_GSM_MODE_ACTIVATED:
          {
            rr_x2g_resel_send_imsg(RR_X2G_RESEL_IMSG_L1_ACTIVATED_IND, gas_id);
            break;
          } /* EV_GSM_MODE_ACTIVATED */

          case EV_GSM_MODE_DEACTIVATED:
          {
            rr_x2g_resel_send_imsg(RR_X2G_RESEL_IMSG_L1_DEACTIVATED_IND, gas_id);
            break;
          } /* EV_GSM_MODE_DEACTIVATED */

          case EV_PANIC_RESET:
          {
            rr_x2g_resel_send_imsg(RR_X2G_RESEL_IMSG_PANIC_RESET_REQ,gas_id);
            /* rr_event will be re-set to EV_PANIC_RESET when rr_x2g_resel_control()
            indicates that it is done. */
            rr_event = EV_NO_EVENT;
            break;
          } /* EV_PANIC_RESET */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(rr_event), "RR_X2G_RESEL_ABORTED");
          }
        }
        break;
      } /* RR_X2G_RESEL_ABORTED */

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

      case RR_X2G_REDIR:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_MM_RR:
              {
                switch (message_id)
                {
                  case RR_DEACT_REQ:
                  case RR_STOP_GSM_MODE_REQ:
                  case RR_PLMN_SELECT_REQ:
                  {
                    (void) rr_save_msg(new_message_ptr, gas_id);
                    new_rr_event = RR_EV_MM_REQ_RECEIVED_ABORT;
                    break;
                  } /* RR_DEACT_REQ ... */

                  case RR_PLMN_LIST_REQ:
                  {
                    if (rr_save_msg(new_message_ptr, gas_id))
                    {
                      new_rr_event = rr_w2g_service_redirection_control(
                        RR_EV_MM_REQ_RECEIVED_ABORT,
                        NULL,
                        gas_id
                      );
                    }
                    else
                    {
                      MSG_GERAN_HIGH_0_G("can't process PLMN search request");
                    }
                    break;
                  } /* RR_PLMN_LIST_REQ */

                  case RR_EST_REQ:
                  {
                    MSG_GERAN_MED_0_G("RR_EST_REQ received, save for later");
                    (void) rr_save_msg(new_message_ptr, gas_id);
                    break;
                  } /* RR_EST_REQ */

                  case RR_GMM_GPRS_STATE_CHANGE_REQ:
                  {
                    break;  // no action required
                  }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q                  
                  case RR_PSEUDO_LTE_LIST_UPDATE_IND:
                  {
                    break;  // Ignore this message while X2G Redir is ongoing
                  }
#endif
                  default:
                  {
                    new_rr_event = rr_w2g_service_redirection_control(rr_event, new_message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_MM_RR */

              case MS_RR_RR:
              {
                switch (new_message_ptr->rr.header.rr_message_set)
                {
                  case RR_IMSG:
                  {
                    switch (new_message_ptr->message_header.message_id)
                    {
                      case RR_IMSG_MSC_CHANGE_IND:
                      {
                        /*
                        Update NAS as required
                        RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                        This will be sent when the current procedure completes or G is next active
                        */
                        rr_msc_update(
                          FALSE,   // can_send_service_ind
                          gas_id   // gas_id
                        );
                        break;
                      }

                      case RR_IMSG_PS_ACCESS_REQ:
                      {
                        // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                        rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      case RR_IMSG_NO_PS_ACCESS_REQ:
                      {
                        // No PS access request (could be TLLI unassignment, CS-only)

                        rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      case RR_IMSG_PAGE_IND:
                      {
                        rr_imsg_page_ind_t *imsg = &new_message_ptr->rr.rr.imsg.page_ind;

                        // A page received during cell selection is saved. It is forwarded to NAS when RR/L1
                        // are in Packet Idle
                        rr_ce_save_page(&imsg->page_info, gas_id);
                        break;
                      }

                      case RR_IMSG_PAGE_MODE_IND:
                      {
                        // Changes in OTA page mode are ignored during cell selection
                        // (these may be received after Early Camping)
                        MSG_GERAN_MED_0_G("Page Mode change ignored");
                        break;
                      }

                      default: ; // ignore other RR-IMSG messages
                    }
                    break;
                  }

                  case RR_L1_IDLE_IMSG:
                  {
                    rr_l1_idle_control(new_message_ptr, gas_id);
                    break;
                  }

                  case RR_L1_IDLE_OMSG:
                  {
                    switch (new_message_ptr->message_header.message_id)
                    {
                      case RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF:
                      {
                        rr_w2g_service_redirection_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id);
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(
                          new_message_ptr,
                          rr_main_state_name(rr_control_data_ptr->rr_state)
                        );
                      }
                    }
                    break;
                  }

                  default: ; // ignore other RR-RR messages
                }
                break;
              } /* MS_RR_RR */

              case MS_TIMER:
              {
                switch (new_message_ptr->mid_timer_expiry.timer_id)
                {
                  case T3122:
                  {
                    // This is unexpected as the T3122 is stopped when the G-RAT is deactivated
                    MSG_GERAN_ERROR_0_G("Unexpected T3122 expiry during X2G service redir");
                    break;
                  }

                  default:
                  {
                    new_rr_event = rr_w2g_service_redirection_control(rr_event, new_message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_TIMER */

              default:
              {
                new_rr_event = rr_w2g_service_redirection_control(rr_event, new_message_ptr, gas_id);
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case RR_EV_W2G_GO_TO_INACTIVE:
          {
#ifdef FEATURE_LTE
            /* If RR received Dedicated Priorities from the
            originating RAT then they should be cleared now. */
            rr_individual_priorities_clear(gas_id);
#endif /* FEATURE_LTE */

            rr_control_data_ptr->rr_state = RR_INACTIVE;
            break;
          } /* RR_EV_W2G_GO_TO_INACTIVE */

          case EV_GO_TO_IDLE:
          {
#ifdef FEATURE_LTE
            rr_dedicated_priorities_take_ownership(gas_id);
#endif /* FEATURE_LTE */

            rr_grr_send_imsg_camp_on_cell_req(
              TRUE,       // notify_mm_service_ind
              FALSE,      // notify_mm_plmn_select_cnf
              FALSE,      // purge_gmm_signalling
              gas_id      // gas_id
            );

#ifdef FEATURE_IDLE_DRX_SCALING
            if (rr_idle_drx_start_scaling_suspend_timer(gas_id))
            {
              MSG_GERAN_HIGH_0_G("Started the IDLE_DRX_SCALING_SUSPEND TIMER");
            }
#endif /* FEATURE_IDLE_DRX_SCALING */

            rr_control_data_ptr->rr_state = RR_GRR;
            break;
          } /* EV_GO_TO_IDLE */

          case EV_PROCESS_SAVED_MESSAGE:
          {
#ifdef FEATURE_LTE
            rr_dedicated_priorities_take_ownership(gas_id);
#endif /* FEATURE_LTE */

            rr_control_data_ptr->rr_state = RR_INACTIVE;
            new_rr_event = rr_process_saved_message(&new_message_ptr, gas_id);
            break;
          } /* EV_PROCESS_SAVED_MESSAGE */

          case RR_EV_ACQUISITION_FAILURE:
          {
#ifdef FEATURE_LTE
            /* As NAS is made aware of the RAT change, RR should take ownership
            of any dedicated priorities that are configured. */
            rr_dedicated_priorities_take_ownership(gas_id);
#endif /* FEATURE_LTE */

            /* This event is generated if a cell is chosen and RR attempts to
            camp on it (and sends RR_ACTIVATION_IND to MM) but subsequently fails
            to camp on the cell.  MM must be informed of RR being out of service
            so that it generates an RR_PLMN_SELECT_REQ. */
            rr_send_no_service_ind(gas_id);

            /* Deactivate to keep RR/L1 in sync - setting the reason to STOP CAMPING will */
            /* mean no messages are sent to MM */
            rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
            new_rr_event = rr_trigger_gsm_as_deactivation(
              RR_DEACT_REASON_STOP_CAMPING,
              gas_id
            );
            break;
          } /* RR_EV_ACQUISITION_FAILURE */

          case EV_PANIC_RESET:
          {
            /* Clear rr_event here as at the end of this state machine
            EV_PANIC_RESET is handled in a stateless manner - we do not want
            this to happen until the X2G Redirection STM has confirmed it has
            handled the panic reset event correctly. */
            rr_event = EV_NO_EVENT;
            new_rr_event = rr_w2g_service_redirection_control(EV_PANIC_RESET_REQ, NULL, gas_id);
            break;
          } /* EV_PANIC_RESET */

          case EV_PANIC_RESET_CNF:
          {
            /* Now that X2G Redirection STM has confirmed that it has handled
            the panic reset we can set rr_event back to EV_PANIC_RESET to be
            handled at the end of this state machine. */
            rr_event = EV_PANIC_RESET;
            break;
          } /* EV_PANIC_RESET_CNF */

          default:
          {
            new_rr_event = rr_w2g_service_redirection_control(rr_event, new_message_ptr, gas_id);
          }
        }
        break;
      } /* RR_X2G_REDIR */

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS_G2T_REDIRECTION)

      case RR_G2X_REDIR:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (new_message_ptr->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (new_message_ptr->rr.header.rr_message_set)
                {
                  case RR_IMSG:
                  {
                    switch (new_message_ptr->message_header.message_id)
                    {
                      case RR_IMSG_MSC_CHANGE_IND:
                      {
                        /*
                        Update NAS as required
                        RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                        This will be sent when G is next active
                        */
                        rr_msc_update(
                          FALSE,   // can_send_service_ind
                          gas_id   // gas_id
                        );
                        break;
                      }

                      case RR_IMSG_PS_ACCESS_REQ:
                      {
                        // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                        rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      case RR_IMSG_NO_PS_ACCESS_REQ:
                      {
                        // No PS access request (could be TLLI unassignment, CS-only)

                        rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      default:
                      {
                        rr_g2x_redir_control(new_message_ptr, gas_id);
                      }
                    }
                    break;
                  }

                  case RR_G2X_REDIR_OMSG:
                  {
                    switch (new_message_ptr->message_header.message_id)
                    {
                      case RR_G2X_REDIR_OMSG_REDIR_CNF:
                      {
                        rr_g2x_redir_deinit(gas_id);

                        switch (new_message_ptr->rr.g2x_redir.omsg.redir_cnf.status)
                        {
                          case RR_G2X_REDIR_STATUS_SUCCESS:
                          {
                            new_rr_event = rr_process_rr_stop_gsm_mode_req(TRUE, gas_id);
                            rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
                            break;
                          } /* RR_G2X_REDIR_STATUS_SUCCESS */

                          case RR_G2X_REDIR_STATUS_FAILURE:
                          {
                            switch (new_message_ptr->rr.g2x_redir.omsg.redir_cnf.failure_action)
                            {
                              case RR_G2X_REDIR_FAILURE_ACTION_OOS:
                              {
                                rr_send_no_service_ind(gas_id);
                                rr_control_data_ptr->rr_state = RR_INACTIVE;
                                break;
                              } /* RR_G2X_REDIR_FAILURE_ACTION_OOS */

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined (FEATURE_GSM_TDS_PSEUDO_FAST_RETURN)
                              case RR_G2X_REDIR_FAILURE_ACTION_CELL_SELECTION:
                              {
                                rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                                rr_control_data_ptr->rr_state = RR_GOING_ACTIVE;
                                break;
                              } /* RR_G2X_REDIR_FAILURE_ACTION_CELL_SELECTION */
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_PSEUDO_FAST_RETURN */

#ifdef FEATURE_SGLTE
                              case RR_G2X_REDIR_FAILURE_ACTION_SGLTE_RETURN_TO_CAMPED:
                              {
                                /*The PS domain was not moved to T/L. GERAN
                                  is still camped so move back to GRR CAMPED*/

                                new_rr_event = EV_SGLTE_G2X_REDIR_COMPLETE;

                                rr_control_data_ptr->rr_state = RR_GRR;
                                break;
                              }
#endif /*FEATURE_SGLTE*/

                              default:
                              {
                                MSG_GERAN_ERROR_1_G(
                                  "Unexpected failure action: %d",
                                  new_message_ptr->rr.g2x_redir.omsg.redir_cnf.failure_action
                                );
                                rr_send_no_service_ind(gas_id);
                                rr_control_data_ptr->rr_state = RR_INACTIVE;
                                break;
                              }
                            }
                            break;
                          } /* RR_G2X_REDIR_STATUS_FAILURE */

#ifdef FEATURE_SGLTE
                          case RR_G2X_REDIR_STATUS_SGLTE_SUCCESS:
                          {
                            /*The PS domain has now been moved to T/L. GERAN
                              is still camped so move back to GRR CAMPED*/
                            new_rr_event = EV_SGLTE_G2X_REDIR_COMPLETE;
                            rr_control_data_ptr->rr_state = RR_GRR;
                          }
                          break;
#endif /*FEATURE_SGLTE*/

                          default:
                          {
                            MSG_GERAN_ERROR_1_G(
                              "Unexpected status: %d",
                              new_message_ptr->rr.g2x_redir.omsg.redir_cnf.status
                            );
                            rr_send_no_service_ind(gas_id);
                            rr_control_data_ptr->rr_state = RR_INACTIVE;
                          }
                        }
                        break;
                      } /* RR_G2X_REDIR_OMSG_REDIR_CNF */

                      case RR_G2X_REDIR_OMSG_ABORT_CNF:
                      {
                        rr_g2x_redir_deinit(gas_id);

                        /* If the redirection was aborted due to an
                        RR_PLMN_LIST_REQ then we need to inform NAS at this
                        point that we are out of service.  This is because
                        the PLMN list search will be performed in the
                        foreground, but this leads RR to assume after the
                        search that NAS is aware that we are out of service. */
                        if (grr_verify_saved_mm_message(RR_PLMN_LIST_REQ, gas_id))
                        {
                          rr_send_no_service_ind(gas_id);
                        }

                        /* L1 is already stopped at this point, but we trigger
                        a new deactivation request to rr_mode_control() in case
                        the stop reason given to L1 needs to be changed.  Once
                        RR_GOING_INACTIVE state received EV_GSM_MODE_DEACTIVATED
                        any saved message will be processed and appropriate next
                        action taken. */

                        rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
                        new_rr_event = rr_mode_control(EV_DEACTIVATE_GSM_MODE, NULL, gas_id);
                        break;
                      } /* RR_G2X_REDIR_OMSG_ABORT_CNF */

#ifdef FEATURE_SGLTE
                      case RR_G2X_REDIR_OMSG_SGLTE_ABORT_CNF:
                      {
                        rr_g2x_redir_deinit(gas_id);

                        new_rr_event = EV_SGLTE_G2X_REDIR_COMPLETE;
                        rr_control_data_ptr->rr_state = RR_GRR;
                        break;
                      }
#endif /*FEATURE_SGLTE*/

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_G2X_REDIR_NG");
                      }
                    }
                    break;
                  } /* RR_G2X_REDIR_OMSG */

#ifdef FEATURE_SGLTE
                  case RR_PS_IMSG:
                  {
                    rr_ps_access_control(new_message_ptr, gas_id);
                    break;
                  }

                  case RR_PS_OMSG:
                  {
                    /*ignore messages from PS*/
                  }
                  break;
#endif /*FEATURE_SGLTE*/

                  default:
                  {
                    rr_g2x_redir_control(new_message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_MM_RR:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case RR_STOP_GSM_MODE_REQ:
                  {
                    /* Make sure that deact reason is set correctly so that
                    RR responds to MM appropriately after stopping L1. */
                    set_rr_deact_reason(RR_DEACT_REASON_STOP_GSM_MODE_REQ, gas_id);

                    /* RR_STOP_GSM_MODE_REQ must be sent in to
                    rr_g2x_redir_control() as it's meaning depends on
                    the stage of the procedure that it is in. */
                    rr_g2x_redir_control(new_message_ptr, gas_id);
                    break;
                  } /* RR_STOP_GSM_MODE_REQ */

                  case RR_EST_REQ:
                  {
                    switch (new_message_ptr->rr_est_req.establishment_cause)
                    {
                      case MM_RR_LOCATION_UPDATE:
                      {
                        rr_send_abort_ind(RR_ABORT_GTOW_REDIRECTION_IN_PROGRESS, MM_RR_OTHER_FAILURE, gas_id);
                        break;
                      } /* MM_RR_LOCATION_UPDATE */

                      default:
                      {
                        set_rr_deact_reason(RR_DEACT_REASON_STOP_CAMPING_THEN_RESTART, gas_id);
                        (void) rr_save_msg(new_message_ptr, gas_id);
                        rr_g2x_redir_send_imsg(RR_G2X_REDIR_IMSG_ABORT_REQ, gas_id);
                      }
                    }
                    break;
                  } /* RR_EST_REQ */

                  case RR_PLMN_LIST_REQ:
                  {
                    switch (new_message_ptr->rr_plmn_list_req.network_selection_mode)
                    {
                      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
                      {
                        rr_send_plmn_list_cnf(
                          NULL,
                          MM_AS_ABORTED,
                          &new_message_ptr->rr_plmn_list_req,
                          gas_id
                        );
                        break;
                      } /* SYS_NETWORK_SELECTION_MODE_AUTOMATIC */

                      default:
                      {
                        set_rr_deact_reason(RR_DEACT_REASON_STOP_CAMPING_THEN_RESTART, gas_id);
                        (void) rr_save_msg(new_message_ptr, gas_id);
                        rr_g2x_redir_send_imsg(RR_G2X_REDIR_IMSG_ABORT_REQ, gas_id);
                      }
                    }
                    break;
                  } /* RR_PLMN_LIST_REQ */

                  case RR_DEACT_REQ:
                  {
                    set_rr_deact_reason(RR_DEACT_REASON_DEACT_REQ, gas_id);
                    /* No need to save the message when it is an
                    RR_DEACT_REQ as will be shut down when the abort is
                    done anyhow. */
                    rr_g2x_redir_send_imsg(RR_G2X_REDIR_IMSG_ABORT_REQ, gas_id);
                    break;
                  } /* RR_DEACT_REQ */

                  case RR_PLMN_SELECT_REQ:
                  {
                    set_rr_deact_reason(RR_DEACT_REASON_STOP_CAMPING_THEN_RESTART, gas_id);
                    (void) rr_save_msg(new_message_ptr, gas_id);
                    rr_g2x_redir_send_imsg(RR_G2X_REDIR_IMSG_ABORT_REQ, gas_id);
                    break;
                  } /* RR_PLMN_SELECT_REQ */

                  case RR_GMM_GPRS_STATE_CHANGE_REQ:
                  {
                    break;  // no action required
                  }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q                  
                  case RR_PSEUDO_LTE_LIST_UPDATE_IND:
                  {
                    break;  // Ignore this message while G2X is ongoing
                  }
#endif
                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_G2X_REDIR_NG");
                  }
                }
                break;
              }

              case MS_MAC_RR:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case MAC_GRR_MAC_STATUS:
                  {
                    rr_ps_access_control(new_message_ptr, gas_id);
                    break;
                  }

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_G2X_REDIR_NG");
                  }
                }
                break;
              }

              case MS_TIMER:
              {
                switch (new_message_ptr->mid_timer_expiry.timer_id)
                {
                  case T3122:
                  {
                    /* Highly unusual situation. Normally a CS connection would
                    not be established if T3122 is running. The exception to
                    this is for a Emergency Call, so it is possible that a CS
                    connection could have been established, released and
                    redirected, when T3122 expires. */
                    MSG_GERAN_MED_0_G("T3122 expired - notify NAS at next service status update");
                    break;
                  } /* T3122 */

                  default:
                  {
                    LOG_UNEXPECTED_TIMER_EXPIRY(
                      new_message_ptr->mid_timer_expiry.timer_id,
                      rr_main_state_name(rr_control_data_ptr->rr_state)
                    );
                  }
                }
                break;
              } /* MS_TIMER */

              default:
              {
                rr_g2x_redir_control(new_message_ptr, gas_id);
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_GSM_MODE_DEACTIVATED:
          {
            rr_g2x_redir_send_imsg(RR_G2X_REDIR_IMSG_L1_DEACTIVATED_IND, gas_id);
            break;
          } /* EV_GSM_MODE_DEACTIVATED */

#ifdef FEATURE_SGLTE
          case EV_SGLTE_G2L_BLIND_REDIR_REQUIRED:
          {
            rr_chan_rel_cell_selection_indicator_t csi = {0};

            csi.valid = TRUE;
            csi.type = LTE_TARGET;
            csi.target.lte_target.blind_redir = TRUE;
            csi.target.lte_target.redirection_for_sglte = TRUE;

            MSG_GERAN_MED_0_G("Starting SGLTE G2L blind redir");
            rr_g2x_redir_init(gas_id);
            rr_g2x_redir_send_imsg_redir_req(
              &csi,
              gas_id
            );
            break;
          }
#endif /*FEATURE_SGLTE*/
          case RR_EV_MEASUREMENT_PARAMS_UPDATED:
          {
            break;
          }
          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(rr_event), "RR_G2X_REDIR_NG");
          }
        }
        break;
      } /* RR_G2X_REDIR */
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION */

#ifdef FEATURE_FEMTO_GSM_NL

      /**
      State: RR_FEMTO
      */
      case RR_FEMTO:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (new_message_ptr->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (new_message_ptr->rr.header.rr_message_set)
                {
                  case RR_FEMTO_OMSG:
                  {
                    switch (new_message_ptr->message_header.message_id)
                    {
                      case RR_FEMTO_OMSG_INACTIVE_IND:
                      {
                        rr_control_data_ptr->rr_state = RR_INACTIVE;
                        break;
                      } /* RR_FEMTO_OMSG_INACTIVE_IND */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_FEMTO");
                      }
                    }
                    break;
                  } /* RR_FEMTO_OMSG */

                  default:
                  {
                    rr_femto_control(new_message_ptr);
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                rr_femto_control(new_message_ptr);
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_GSM_MODE_ACTIVATED:
          {
            rr_femto_send_imsg_l1_activated_ind();
            break;
          } /* EV_GSM_MODE_ACTIVATED */

          case EV_GSM_MODE_DEACTIVATED:
          {
            rr_femto_send_imsg_l1_deactivated_ind();
            break;
          } /* EV_GSM_MODE_DEACTIVATED */

          case RR_EV_EARLY_CAMP_SI_RECEIVED:
          {
            break;   // Ignore - processing not required
          }

          case EV_PANIC_RESET:
          {
            // TODO
            break;
          }

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(rr_event), "RR_FEMTO");
          }
        }
        break;
      } /* RR_FEMTO */

#endif // FEATURE_FEMTO_GSM_NL

#ifdef FEATURE_LTE_TO_GSM_CGI

      case RR_X2G_CGI:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (message_set)
            {
              case MS_RR_RR:
              {
                switch (new_message_ptr->rr.header.rr_message_set)
                {
                  case RR_IMSG:
                  {
                    switch (message_id)
                    {
                      case RR_IMSG_MSC_CHANGE_IND:
                      {
                        /*
                        Update NAS as required
                        RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                        This will be sent when the current procedure completes or G is next active
                        */
                        rr_msc_update(
                          FALSE,   // can_send_service_ind
                          gas_id   // gas_id
                        );
                        break;
                      }

                      case RR_IMSG_PS_ACCESS_REQ:
                      {
                        // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                        rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      case RR_IMSG_NO_PS_ACCESS_REQ:
                      {
                        // No PS access request (could be TLLI unassignment, CS-only)

                        rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      default:
                      {
                        rr_x2g_cgi_control(new_message_ptr, gas_id);
                      }
                    }
                    break;
                  }

                  case RR_X2G_CGI_OMSG:
                  {
                    switch (message_id)
                    {
                      case RR_X2G_CGI_OMSG_CGI_CNF:
                      {
                        geran_grr_lte_get_cgi_rsp_t   lte_get_cgi_rsp;
                        rr_x2g_cgi_omsg_cgi_cnf_t   * cgi_cnf = &new_message_ptr->rr.x2g_cgi.omsg.cgi_cnf;
                        errno_enum_type               result;

                        rr_x2g_cgi_deinit(gas_id);
                        rr_control_data_ptr->rr_state = RR_INACTIVE;

                        memset(&lte_get_cgi_rsp, 0, sizeof(lte_get_cgi_rsp));

                        msgr_init_hdr(
                          &lte_get_cgi_rsp.msg_hdr,
                          MSGR_GERAN_GRR,
                          GERAN_GRR_LTE_GET_CGI_RSP
                        );

                        lte_get_cgi_rsp.arfcn = cgi_cnf->arfcn.num;
                        if (cgi_cnf->arfcn.band == SYS_BAND_PCS_1900)
                        {
                          lte_get_cgi_rsp.band_ind = GERAN_GRR_BAND_IND_PCS_1900;
                        }
                        else
                        {
                          lte_get_cgi_rsp.band_ind = GERAN_GRR_BAND_IND_DCS_1800;
                        }
                        lte_get_cgi_rsp.cell_id.ncc = cgi_cnf->bsic.PLMN_colour_code;
                        lte_get_cgi_rsp.cell_id.bcc = cgi_cnf->bsic.BS_colour_code;

                        if (cgi_cnf->info.mask & RR_X2G_CGI_INFO_MASK_RXLEV)
                        {
                          lte_get_cgi_rsp.cgi_rsp_info.cgi_rsp_info_bitmask |= GERAN_GRR_LTE_CGI_RSP_RSSI;
                          lte_get_cgi_rsp.cgi_rsp_info.rssi = cgi_cnf->info.rxlev;
                        }

                        if (cgi_cnf->info.mask & RR_X2G_CGI_INFO_MASK_CELL_ID)
                        {
                          lte_get_cgi_rsp.cgi_rsp_info.cgi_rsp_info_bitmask |= GERAN_GRR_LTE_CGI_RSP_CELL_ID;
                          lte_get_cgi_rsp.cgi_rsp_info.cgi_info.cell_id = cgi_cnf->info.cell_id;
                        }

                        if (cgi_cnf->info.mask & RR_X2G_CGI_INFO_MASK_LAI)
                        {
                          lte_get_cgi_rsp.cgi_rsp_info.cgi_rsp_info_bitmask |= GERAN_GRR_LTE_CGI_RSP_PLMN_ID;
                          lte_get_cgi_rsp.cgi_rsp_info.cgi_info.plmn_id = cgi_cnf->info.lai.plmn_id;

                          lte_get_cgi_rsp.cgi_rsp_info.cgi_rsp_info_bitmask |= GERAN_GRR_LTE_CGI_RSP_LAC;
                          lte_get_cgi_rsp.cgi_rsp_info.cgi_info.lac[0] = (cgi_cnf->info.lai.location_area_code >> 8) & 0xff;
                          lte_get_cgi_rsp.cgi_rsp_info.cgi_info.lac[1] = cgi_cnf->info.lai.location_area_code & 0xff;
                        }

                        if (cgi_cnf->info.mask & RR_X2G_CGI_INFO_MASK_RAC)
                        {
                          lte_get_cgi_rsp.cgi_rsp_info.cgi_rsp_info_bitmask |= GERAN_GRR_LTE_CGI_RSP_RAC;
                          if (cgi_cnf->info.rac_present)
                          {
                            lte_get_cgi_rsp.cgi_rsp_info.cgi_info.rac_present = TRUE;
                            lte_get_cgi_rsp.cgi_rsp_info.cgi_info.rac = cgi_cnf->info.rac;
                          }
                        }

                        result = rr_msgr_send(&lte_get_cgi_rsp.msg_hdr, sizeof(lte_get_cgi_rsp), gas_id);
                        if (result != E_SUCCESS)
                        {
                          MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                        }
                        break;
                      } /* RR_X2G_CGI_OMSG_CGI_CNF */

                      case RR_X2G_CGI_OMSG_ABORT_CNF:
                      {
                        geran_grr_lte_abort_cgi_rsp_t   lte_abort_cgi_rsp;
                        rr_x2g_cgi_omsg_abort_cnf_t   * abort_cnf = &new_message_ptr->rr.x2g_cgi.omsg.abort_cnf;
                        errno_enum_type                 result;

                        rr_x2g_cgi_deinit(gas_id);
                        rr_control_data_ptr->rr_state = RR_INACTIVE;

                        memset(&lte_abort_cgi_rsp, 0, sizeof(lte_abort_cgi_rsp));

                        msgr_init_hdr(
                          &lte_abort_cgi_rsp.msg_hdr,
                          MSGR_GERAN_GRR,
                          GERAN_GRR_LTE_ABORT_CGI_RSP
                        );

                        lte_abort_cgi_rsp.arfcn = abort_cnf->arfcn.num;
                        if (abort_cnf->arfcn.band == SYS_BAND_PCS_1900)
                        {
                          lte_abort_cgi_rsp.band_ind = GERAN_GRR_BAND_IND_PCS_1900;
                        }
                        else
                        {
                          lte_abort_cgi_rsp.band_ind = GERAN_GRR_BAND_IND_DCS_1800;
                        }
                        lte_abort_cgi_rsp.cell_id.ncc = abort_cnf->bsic.PLMN_colour_code;
                        lte_abort_cgi_rsp.cell_id.bcc = abort_cnf->bsic.BS_colour_code;

                        if (abort_cnf->info.mask & RR_X2G_CGI_INFO_MASK_RXLEV)
                        {
                          lte_abort_cgi_rsp.cgi_rsp_info.cgi_rsp_info_bitmask |= GERAN_GRR_LTE_CGI_RSP_RSSI;
                          lte_abort_cgi_rsp.cgi_rsp_info.rssi = abort_cnf->info.rxlev;
                        }

                        if (abort_cnf->info.mask & RR_X2G_CGI_INFO_MASK_CELL_ID)
                        {
                          lte_abort_cgi_rsp.cgi_rsp_info.cgi_rsp_info_bitmask |= GERAN_GRR_LTE_CGI_RSP_CELL_ID;
                          lte_abort_cgi_rsp.cgi_rsp_info.cgi_info.cell_id = abort_cnf->info.cell_id;
                        }

                        if (abort_cnf->info.mask & RR_X2G_CGI_INFO_MASK_LAI)
                        {
                          lte_abort_cgi_rsp.cgi_rsp_info.cgi_rsp_info_bitmask |= GERAN_GRR_LTE_CGI_RSP_PLMN_ID;
                          lte_abort_cgi_rsp.cgi_rsp_info.cgi_info.plmn_id = abort_cnf->info.lai.plmn_id;

                          lte_abort_cgi_rsp.cgi_rsp_info.cgi_rsp_info_bitmask |= GERAN_GRR_LTE_CGI_RSP_LAC;
                          lte_abort_cgi_rsp.cgi_rsp_info.cgi_info.lac[0] = (abort_cnf->info.lai.location_area_code >> 8) & 0xff;
                          lte_abort_cgi_rsp.cgi_rsp_info.cgi_info.lac[1] = abort_cnf->info.lai.location_area_code & 0xff;
                        }

                        if (abort_cnf->info.mask & RR_X2G_CGI_INFO_MASK_RAC)
                        {
                          lte_abort_cgi_rsp.cgi_rsp_info.cgi_rsp_info_bitmask |= GERAN_GRR_LTE_CGI_RSP_RAC;
                          if (abort_cnf->info.rac_present)
                          {
                            lte_abort_cgi_rsp.cgi_rsp_info.cgi_info.rac_present = TRUE;
                            lte_abort_cgi_rsp.cgi_rsp_info.cgi_info.rac = abort_cnf->info.rac;
                          }
                        }

                        result = rr_msgr_send(&lte_abort_cgi_rsp.msg_hdr, sizeof(lte_abort_cgi_rsp), gas_id);
                        if (result != E_SUCCESS)
                        {
                          MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                        }
                        break;
                      } /* RR_X2G_CGI_OMSG_ABORT_CNF */

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_CGI");
                      }
                    }
                    break;
                  } /* RR_X2G_CGI_OMSG */

                  case RR_CELL_ACQ_IMSG:
                  {
                    /* Filter out these messages as rr_x2g_cgi_control() uses
                    rr_select_bcch_control() rather than rr_cell_acq_control(). */
                    break;
                  } /* RR_CELL_ACQ_IMSG */

                  default:
                  {
                    rr_x2g_cgi_control(new_message_ptr, gas_id);
                  }
                }
                break;
              } /* MS_RR_RR */

              case MS_MSGR_RR:
              {
                switch (new_message_ptr->rr_msgr_msg.message.hdr.id)
                {
                  case GERAN_GRR_LTE_ABORT_CGI_REQ:
                  {
                    /* Save the message in case there is a panic reset before the procedure completes */
                    rr_save_msg_for_recovery_after_panic_reset(new_message_ptr, gas_id);

                    rr_x2g_cgi_send_imsg(RR_X2G_CGI_IMSG_ABORT_REQ, gas_id);
                    break;
                  } /* GERAN_GRR_LTE_ABORT_CGI_REQ */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_CGI");
                  }
                }
                break;
              } /* MS_MSGR_RR */

              case MS_MM_RR:
              {
                switch (message_id)
                {
                  case RR_GMM_GPRS_STATE_CHANGE_REQ:
                  {
                    break;  // no action required
                  }
#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q                  
                  case RR_PSEUDO_LTE_LIST_UPDATE_IND:
                  {
                    break;  //  Ignore Pseudo List in X2G_CGI as RR is not the current Active RAT
                  }
#endif
                  default:
                  {
                    rr_x2g_cgi_control(new_message_ptr, gas_id);
                  }
                }
              }
              default:
              {
                rr_x2g_cgi_control(new_message_ptr, gas_id);
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_GSM_MODE_ACTIVATED:
          {
            rr_x2g_cgi_send_imsg(RR_X2G_CGI_IMSG_L1_ACTIVATED_IND, gas_id);
            break;
          } /* EV_GSM_MODE_ACTIVATED */

          case EV_GSM_MODE_DEACTIVATED:
          {
            rr_x2g_cgi_send_imsg(RR_X2G_CGI_IMSG_L1_DEACTIVATED_IND, gas_id);
            break;
          } /* EV_GSM_MODE_DEACTIVATED */

          case EV_PANIC_RESET:
          {
            MSG_GERAN_HIGH_0_G("Panic reset during X2G CGI, reset CGI module");
            rr_timer_cancel(RR_X2G_CGI_TIMER, gas_id);
            rr_x2g_cgi_deinit(gas_id);
            break;
          } /* EV_PANIC_RESET */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(rr_event), "RR_X2G_CGI");
          }
        }
        break;
      } /* RR_X2G_CGI */

#endif /* FEATURE_LTE_TO_GSM_CGI */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
      /**
        @brief RR_X2G_CCO_FAILED state is entered if X2G CCO failure occurs
        after RR receives RR_ACTIVATION_RSP from NAS.

        The main point of this state is to maintain CCO context until RR
        has send CCO failure indication to the originating RAT and received
        RR_STOP_GSM_MODE_REQ from NAS, indicating that the CCO procedure
        really is terminated.
      */
      case RR_X2G_CCO_FAILED:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (new_message_ptr->message_header.message_set)
            {
              case MS_MM_RR:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  /*
                  RR_STOP_GSM_MODE_REQ is treated as an indication of
                  successful reactivation of the originating RAT (i.e.
                  procedure is completed).
                  */
                  case RR_STOP_GSM_MODE_REQ:
                  {
                    switch (rr_control_data_ptr->x2g_resel_type)
                    {
#ifdef FEATURE_WCDMA
                      case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
                      {
                        /* RR_STOP_GSM_MODE_REQ is always treated as successful
                        completion of the CCO failure procedure. */
                        rr_send_imsg(RR_IMSG_X2G_CCO_COMPLETE_IND, gas_id);
                        break;
                      } /* RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
                      case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
                      {
                        rr_send_imsg(RR_IMSG_X2G_CCO_COMPLETE_IND, gas_id);
                        break;
                      } /* RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
                      case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
                      {
                        /* L2G CCO failure procedure completion is indicated by
                        RR_LTOG_CCO_FAILURE stop reason.  Any other stop reason
                        is treated as an failure abort by RR. */
                        switch (new_message_ptr->rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason)
                        {
                          case RR_LTOG_CCO_FAILURE:
                          {
                            rr_send_imsg(RR_IMSG_X2G_CCO_COMPLETE_IND, gas_id);
                            break;
                          } /* RR_LTOG_CCO_FAILURE */

                          default:
                          {
                            /* CCO failure procedure is aborted by NAS command, send appropriate
                            response to originating RAT. */
                            msgr_hdr_struct_type hdr;
                            errno_enum_type      result;

                            rr_save_msg(new_message_ptr, gas_id);

                            memset(&hdr, 0, sizeof(hdr));

                            msgr_init_hdr(
                              &hdr,
                              MSGR_GERAN_GRR,
                              GERAN_GRR_LTE_ABORT_CCO_FAILURE_PROCEDURE_IND
                            );

                            result = rr_msgr_send(&hdr, sizeof(hdr), gas_id);
                            if (result != E_SUCCESS)
                            {
                              MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                            }

                            rr_control_data_ptr->rr_state = RR_X2G_CCO_FAILED_ABORTED;
                            break;
                          }
                        }
                        break;
                      } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_LTE */

                      default:
                      {
                        MSG_GERAN_ERROR_1_G(
                          "Unexpected x2g_resel_type: %d",
                          rr_control_data_ptr->x2g_resel_type
                        );
                      }
                    }
                    break;
                  } /* RR_STOP_GSM_MODE_REQ */

                  case RR_EST_REQ:
                  {
                    /* RR_EST_REQ is part of CCO procedure. If CCO is failed then send ABORT_IND to NAS */
                    rr_send_abort_ind(RR_CONNECTION_FAILURE, MM_RR_OTHER_FAILURE, gas_id);
                    break;
                  } /* RR_EST_REQ */

                  default:
                  {
#ifdef FEATURE_LTE
                    if (rr_control_data_ptr->x2g_resel_type == RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER)
                    {
                      /* CCO failure procedure is aborted by NAS command, send appropriate
                      response to originating RAT. */
                      msgr_hdr_struct_type hdr;
                      errno_enum_type      result;

                      rr_save_msg(new_message_ptr, gas_id);

                      memset(&hdr, 0, sizeof(hdr));

                      msgr_init_hdr(
                        &hdr,
                        MSGR_GERAN_GRR,
                        GERAN_GRR_LTE_ABORT_CCO_FAILURE_PROCEDURE_IND
                      );

                      result = rr_msgr_send(&hdr, sizeof(hdr), gas_id);
                      if (result != E_SUCCESS)
                      {
                        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                      }
                      rr_control_data_ptr->rr_state = RR_X2G_CCO_FAILED_ABORTED;
                    }
                    else
#endif /* FEATURE_LTE */
                    {
                      /* If anything else is received from NAS at this point,
                      it will not be handled. */
                      MSG_GERAN_ERROR_0_G("Unable to process NAS command");
                    }
                  }
                }
                break;
              } /* MS_MM_RR */

              case MS_MAC_RR:
              {
                switch (new_message_ptr->message_header.message_id)
                {
                  case MAC_GRR_MAC_STATUS:
                  {
                    break;
                  } /* MAC_GRR_MAC_STATUS */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_CCO_FAILED");
                  }
                }
                break;
              } /* MS_MAC_RR */

              case MS_RR_RR:
              {
                switch (new_message_ptr->rr.header.rr_message_set)
                {
                  case RR_IMSG:
                  {
                    switch (new_message_ptr->rr.header.imh.message_id)
                    {
                      case RR_IMSG_X2G_CCO_COMPLETE_IND:
                      {
                        MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
                        rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                        rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                        rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
                        rr_control_data_ptr->is_cco_for_csfb = FALSE;
                        rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
                        rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;

                        rr_set_panic_reset_x2g_cco_flag(RR_X2G_RESEL_TYPE_UNDEFINED, gas_id);

#ifdef FEATURE_LTE
                        /* If RR received Dedicated Priorities from the
                        originating RAT then they should be cleared now. */
                        rr_individual_priorities_clear(gas_id);
#endif /* FEATURE_LTE */

                        new_rr_event = rr_process_rr_stop_gsm_mode_req(TRUE, gas_id);
                        rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
                        break;
                      } /* RR_IMSG_X2G_CCO_COMPLETE_IND */

                      case RR_IMSG_MSC_CHANGE_IND:
                      {
                        /*
                        Update NAS as required
                        RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                        This will be sent when the current procedure completes or G is next active
                        */
                        rr_msc_update(
                          FALSE,   // can_send_service_ind
                          gas_id   // gas_id
                        );
                        break;
                      }

                      case RR_IMSG_PS_ACCESS_REQ:
                      {
                        // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                        rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      case RR_IMSG_NO_PS_ACCESS_REQ:
                      {
                        // No PS access request (could be TLLI unassignment, CS-only)

                        rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_CCO_ABORTED");
                      }
                    } /* RR_IMSG_X2G_CCO_COMPLETE_IND */
                    break;
                  } /* RR_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_CCO_ABORTED");
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_CCO_FAILED");
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          case EV_GSM_MODE_DEACTIVATED:
          {
            switch (rr_control_data_ptr->x2g_resel_type)
            {
#ifdef FEATURE_WCDMA
              case RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER:
              {
                if (rr_get_x2g_cco_act_rsp_rcvd(gas_id))
                {
                  rr_send_interrat_cc_order_cnf(
                  CCO_FAILURE_AFTER_ACT_IND,
                  rr_x2g_map_cco_status_to_w2g_cco_status(
                    rr_control_data_ptr->x2g_cco_status
                  ),
                  gas_id
                );
                }
                else
                {
                  rr_send_interrat_cc_order_cnf(
                  CCO_FAILURE_BEFORE_ACT_IND,
                  rr_x2g_map_cco_status_to_w2g_cco_status(
                    rr_control_data_ptr->x2g_cco_status
                  ),
                  gas_id
                );
                }
                rr_set_x2g_cco_act_rsp_rcvd(FALSE, gas_id);
                break;
              } /* RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS
              case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
              {
                if (rr_get_x2g_cco_act_rsp_rcvd(gas_id))
                {
                  rr_send_tds_interrat_cc_order_cnf(
                  TDSCCO_FAILURE_AFTER_ACT_IND,
                  rr_x2g_map_cco_status_to_t2g_cco_status(
                    rr_control_data_ptr->x2g_cco_status
                  ),
                  gas_id
                );
                }
                else
                {
                  rr_send_tds_interrat_cc_order_cnf(
                  TDSCCO_FAILURE_BEFORE_ACT_IND,
                  rr_x2g_map_cco_status_to_t2g_cco_status(
                    rr_control_data_ptr->x2g_cco_status
                  ),
                  gas_id
                );
                }
                rr_set_x2g_cco_act_rsp_rcvd(FALSE, gas_id);
                break;
              } /* RR_X2G_RESEL_TYPE_W2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
              case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
              {
                geran_grr_lte_cco_rsp_t lte_cco_rsp;
                errno_enum_type         result;

                memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

                msgr_init_hdr(
                  &lte_cco_rsp.hdr,
                  MSGR_GERAN_GRR,
                  GERAN_GRR_LTE_CCO_RSP
                );

                if (rr_get_x2g_cco_act_rsp_rcvd(gas_id))
                {
                  lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_RACH;
                }
                else
                {
                  lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_ACQUIRE;
                }

                rr_set_x2g_cco_act_rsp_rcvd(FALSE, gas_id);
                result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
                if (result != E_SUCCESS)
                {
                  MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
                }

                break;
              } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_LTE */

              default:
              {
                MSG_GERAN_ERROR_1_G(
                  "Unexpected x2g_resel_type: %d",
                  rr_control_data_ptr->x2g_resel_type
                );
              }
            }
            break;
          } /* EV_GSM_MODE_DEACTIVATED */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(rr_event), "RR_X2G_CCO_FAILED");
          }
        }
        break;
      } /* RR_X2G_CCO_FAILED */

      /**
        @brief RR_X2G_CCO_ABORTED state is entered when an X2G CCO procedure is
               aborted by NAS.  This may occur any time after sending
               RR_ACTIVATION_IND after acquisition has been completed.
      */
      case RR_X2G_CCO_ABORTED:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (new_message_ptr->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (new_message_ptr->rr.header.rr_message_set)
                {
                  case RR_IMSG:
                  {
                    switch (new_message_ptr->rr.header.imh.message_id)
                    {
                      case RR_IMSG_X2G_CCO_COMPLETE_IND:
                      {
                        /* Remove any message saved in case of panic reset */
                        rr_save_msg_for_recovery_after_panic_reset(NULL, gas_id);
                        rr_set_panic_reset_x2g_cco_flag(RR_X2G_RESEL_TYPE_UNDEFINED, gas_id);

                        /* Deactivate to keep RR/L1 in sync - setting the reason to STOP CAMPING will */
                        /* mean no messages are sent to MM */
                        rr_control_data_ptr->rr_state = RR_GOING_INACTIVE;
                        new_rr_event = rr_trigger_gsm_as_deactivation(
                          RR_DEACT_REASON_STOP_CAMPING,
                          gas_id
                        );
                        break;
                      } /* RR_IMSG_X2G_CCO_COMPLETE_IND */

                      case RR_IMSG_MSC_CHANGE_IND:
                      {
                        /*
                        Update NAS as required
                        RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                        This will be sent when the current procedure completes or G is next active
                        */
                        rr_msc_update(
                          FALSE,   // can_send_service_ind
                          gas_id   // gas_id
                        );
                        break;
                      }

                      case RR_IMSG_PS_ACCESS_REQ:
                      {
                        // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                        rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      case RR_IMSG_NO_PS_ACCESS_REQ:
                      {
                        // No PS access request (could be TLLI unassignment, CS-only)

                        rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_CCO_ABORTED");
                      }
                    } /* RR_IMSG_X2G_CCO_COMPLETE_IND */
                    break;
                  } /* RR_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_CCO_ABORTED");
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_CCO_ABORTED");
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(rr_event), "RR_X2G_CCO_ABORTED");
          }
        }
        break;
      } /* RR_X2G_CCO_ABORTED */

      /**
        @brief RR_X2G_CCO_FAILED_ABORTED state is entered when a NAS request is
               received between RR sending a CCO failure indication to the
               originating RAT after CCO failure (after RR_ACTIVATION_RSP) and
               RR receiving RR_STOP_GSM_MODE_REQ indicating that the originating
               RAT has reactivated successfully.  RR will have sent a failure-abort
               request to the originating RAT, and is now waiting for a completion
               indication, indicating that RR can now reactivate and continue
               processing the saved NAS request. */
      case RR_X2G_CCO_FAILED_ABORTED:
      {
        switch (rr_event)
        {
          case EV_INPUT_MESSAGE:
          {
            switch (new_message_ptr->message_header.message_set)
            {
              case MS_RR_RR:
              {
                switch (new_message_ptr->rr.header.rr_message_set)
                {
                  case RR_IMSG:
                  {
                    switch (new_message_ptr->rr.header.imh.message_id)
                    {
                      case RR_IMSG_X2G_CCO_COMPLETE_IND:
                      {
                        MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
                        rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
                        rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
                        rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
                        rr_control_data_ptr->is_cco_for_csfb = FALSE;
                        rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
                        rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;

                        rr_set_panic_reset_x2g_cco_flag(RR_X2G_RESEL_TYPE_UNDEFINED, gas_id);

                        rr_control_data_ptr->rr_state = RR_GOING_ACTIVE;
                        new_rr_event = rr_mode_control(EV_ACTIVATE_GSM_MODE, NULL, gas_id);
                        break;
                      } /* RR_IMSG_X2G_CCO_COMPLETE_IND */

                      case RR_IMSG_MSC_CHANGE_IND:
                      {
                        /*
                        Update NAS as required
                        RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                        This will be sent when the current procedure completes or G is next active
                        */
                        rr_msc_update(
                          FALSE,   // can_send_service_ind
                          gas_id   // gas_id
                        );
                        break;
                      }

                      case RR_IMSG_PS_ACCESS_REQ:
                      {
                        // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                        rr_imsg_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      case RR_IMSG_NO_PS_ACCESS_REQ:
                      {
                        // No PS access request (could be TLLI unassignment, CS-only)

                        rr_imsg_no_ps_access_req_t *imsg = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                        // Call the call-back function (if one was supplied)
                        CALL_RR_CB_FN(imsg->confirm_cb_fn, gas_id);
                        break;
                      }

                      default:
                      {
                        LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_CCO_FAILED_ABORTED");
                      }
                    } /* RR_IMSG_X2G_CCO_COMPLETE_IND */
                    break;
                  } /* RR_IMSG */

                  default:
                  {
                    LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_CCO_FAILED_ABORTED");
                  }
                }
                break;
              } /* MS_RR_RR */

              default:
              {
                LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_X2G_CCO_FAILED_ABORTED");
              }
            }
            break;
          } /* EV_INPUT_MESSAGE */

          default:
          {
            LOG_UNEXPECTED_EVENT(rr_event_name(rr_event), "RR_X2G_CCO_FAILED_ABORTED");
          }
        }
        break;
      } /* RR_X2G_CCO_ABORTED */
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

       case RR_CELL_SELECTED_ACQUIRE_SI:
       {
         switch (rr_event)
         {
           case EV_INPUT_MESSAGE:
           {
             switch (new_message_ptr->message_header.message_set)
             {
               case MS_RR_RR:
               {
                 switch (new_message_ptr->rr.header.rr_message_set)
                 {
                   case RR_IMSG:
                   {
                     switch (new_message_ptr->rr.header.imh.message_id)
                     {
                       case RR_IMSG_MSC_CHANGE_IND:
                       {
                         /*
                         Update NAS as required
                         RR is not camped, so a RR_SERVICE_IND should not be sent to MM.
                         This will be sent when the current procedure completes or G is next active
                         */
                         rr_msc_update(
                           FALSE,   // can_send_service_ind
                           gas_id   // gas_id
                         );
                         break;
                       }

                       case RR_IMSG_PS_ACCESS_REQ:
                       {
                         // PS access request (could be RR_GMM_GPRS_SERVICE_REQ, T3142 expiry)

                         rr_imsg_ps_access_req_t *imsg_ptr = &new_message_ptr->rr.rr.imsg.ps_access_req;

                         // Call the call-back function (if one was supplied)
                         CALL_RR_CB_FN(imsg_ptr->confirm_cb_fn, gas_id);
                         break;
                       }

                       case RR_IMSG_NO_PS_ACCESS_REQ:
                       {
                         // No PS access request (could be TLLI unassignment, CS-only)

                         rr_imsg_no_ps_access_req_t *imsg_ptr = &new_message_ptr->rr.rr.imsg.no_ps_access_req;

                         // Call the call-back function (if one was supplied)
                         CALL_RR_CB_FN(imsg_ptr->confirm_cb_fn, gas_id);
                         break;
                       }

                       case RR_IMSG_PAGE_IND:
                       {
                         rr_imsg_page_ind_t *imsg = &new_message_ptr->rr.rr.imsg.page_ind;

                         rr_ce_save_page(&imsg->page_info, gas_id);
                         break;
                       }

                       case RR_IMSG_PAGE_MODE_IND:
                       {
                         // Changes in OTA page mode are ignored
                         MSG_GERAN_MED_0_G("Page Mode change ignored");
                         break;
                       }

                       default:
                       {
                         LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_CELL_SELECTED_ACQUIRE_SI");
                       }
                     }
                     break;
                   } /* RR_IMSG */

                   case RR_SELECT_BCCH_OMSG:
                   {
                     switch (new_message_ptr->rr.select_bcch.omsg.header.imh.message_id)
                     {
                       case RR_SELECT_BCCH_OMSG_SELECT_CNF:
                       {
                         gprs_scell_info_t *pcell_info_ptr = rr_gprs_get_pcell_info(gas_id);
                         RR_NULL_CHECK_FATAL(pcell_info_ptr);

                         // SI event generation flags are used for remaining SI acquisition
                         pcell_info_ptr->gsm.generate_si_received_event = TRUE;

                         rr_control_data_ptr->rr_state = RR_EARLY_CAMPED_WAIT_FOR_SI;
                         break;
                       } /* RR_SELECT_BCCH_OMSG_SELECT_CNF */

                       case RR_SELECT_BCCH_OMSG_SELECT_FAIL_IND:
                       {
                         rr_select_bcch_omsg_select_fail_ind_t *omsg_ptr =
                           &new_message_ptr->rr.select_bcch.omsg.rr_select_bcch_omsg_select_fail_ind;

                         if (omsg_ptr->reason == RR_SELECT_BCCH_FAIL_REASON_ABORTED)
                         {
                           // Handle the saved message in RR_NO_CHANNELS state
                           // Note: This should only be RR_STOP_GSM_MODE_REQ or RR_DEACT_REQ
                           rr_control_data_ptr->rr_state = RR_NO_CHANNELS;
                           new_rr_event = rr_process_saved_message(&new_message_ptr, gas_id);
                         }
                         else
                         {
                           // SI acqusition failed - use cell selection to regain service
                           rr_control_data_ptr->rr_state = RR_EARLY_CAMPED_WAIT_FOR_SI;
                           new_rr_event = EV_TRIGGER_CELL_SELECTION;
                         }
                         break;
                       }

                       default:
                       {
                         LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_CELL_SELECTED_ACQUIRE_SI");
                       }
                     }
                     break;
                   } /* RR_SELECT_BCCH_OMSG */

                   case RR_L1_IDLE_OMSG:
                   {
                     rr_select_bcch_control(new_message_ptr, gas_id);
                     break;
                   } /* RR_L1_IDLE_IMSG/OMSG */

                   default:
                   {
                     LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_CELL_SELECTED_ACQUIRE_SI");
                   }
                 }
                 break;
               } /* MS_RR_RR */

               case MS_MM_RR:
               {
                 switch (new_message_ptr->mm.header.message_id)
                 {
                   case RR_EST_REQ:
                   {
                     MSG_GERAN_HIGH_0_G("Save RR_EST_REQ");

                     (void)rr_save_msg(new_message_ptr, gas_id);
                     break;
                   }

                   case RR_PLMN_LIST_REQ:
                   {
                     rr_plmn_list_req_T *rr_plmn_list_req_ptr = &new_message_ptr->mm.rr_plmn_list_req;

                     if (rr_plmn_list_req_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
                     {
                       MSG_GERAN_HIGH_0_G("Reject RR_PLMN_LIST_REQ (Auto)");

                       rr_send_plmn_list_cnf(
                         NULL,                   // available_plmn_list_ptr
                         MM_AS_ABORTED,          // status
                         rr_plmn_list_req_ptr,   // plmn_list_req
                         gas_id                  // gas_id
                       );
                     }
                     else
                     {
                       MSG_GERAN_HIGH_0_G("Save RR_PLMN_LIST_REQ (Manual)");

                       (void)rr_save_msg(new_message_ptr, gas_id);
                     }
                     break;
                   }

                   case RR_PLMN_SELECT_REQ:
                   {
                     MSG_GERAN_HIGH_0_G("Save RR_PLMN_SELECT_REQ");

                     (void)rr_save_msg(new_message_ptr, gas_id);
                     break;
                   }

                   case RR_STOP_GSM_MODE_REQ:
                   case RR_DEACT_REQ:
                   {
                     MSG_GERAN_HIGH_0_G("Save RR_STOP_GSM_MODE_REQ/RR_DEACT_REQ");
                     (void)rr_save_msg(new_message_ptr, gas_id);

                     // Abort the RR-SELECT-BCCH procedure in progress
                     rr_select_bcch_send_imsg_abort_req(gas_id);
                     break;
                   }

                   case RR_ABORT_REQ:
                   {
                     // No CS connection established - reply with confirmation
                     rr_send_abort_cnf(MM_AS_SUCCESS, gas_id);
                     break;

                     /**
                      * Note: They won't be any saved RR_EST_REQ at this point because the RR_ABORT_REQ
                      * would have been blocked on the queue if there was a RR_EST_REQ saved
                      */
                   }

                   case RR_GMM_GPRS_STATE_CHANGE_REQ:
                   {
                     break;  // action action required
                   }

#ifdef FEATURE_FAST_G2L_PSEUDO_SI2Q                   
                   case RR_PSEUDO_LTE_LIST_UPDATE_IND:
                   {
                     break;  // Ignore, as Pseudo List Evaluated upon completion of SI Acq
                   }
#endif

#ifdef FEATURE_SGLTE
                   case RR_CHANGE_MODE_IND:
                   {
                     if (rr_is_sglte_mode_active(gas_id))
                     {
                        /* RR_CHANGE_MODE_IND may change service domain and/or RAT priority list */
                        rr_pending_mode_change_data_store(&new_message_ptr->mm.rr_change_mode_ind, gas_id);
                     }
                     break;
                   }
#endif /* FEATURE_SGLTE */

                   default:
                   {
                     LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_CELL_SELECTED_ACQUIRE_SI");
                   }
                 }
                 break;
               }

               case MS_TIMER:
               {
                 rr_select_bcch_control(new_message_ptr, gas_id);
                 break;
               }

               default:
               {
                 LOG_UNEXPECTED_MESSAGE(new_message_ptr, "RR_CELL_SELECTED_ACQUIRE_SI");
               }
             }
             break;
           } /* EV_INPUT_MESSAGE */

           case RR_EV_CANDIDATE_CELL_SI_RECEIVED:
           {
             break;  // not used
           }

           default:
           {
             LOG_UNEXPECTED_EVENT(rr_event_name(rr_event), "RR_CELL_SELECTED_ACQUIRE_SI");
           }
         }
         break;
       } /* RR_CELL_SELECTED_ACQUIRE_SI */

      default:
      {
        MSG_GERAN_ERROR_1_G(
          "Bad RR State(%d)",
          rr_control_data_ptr->rr_state
        );
      }
      break;

    } /* End of the switch (rr_control_data_ptr->rr_state) statement */

#ifdef DEBUG_RR_TRACE_MSG
    if (((rr_event != new_rr_event) && (new_rr_event != EV_NO_EVENT)) || (rr_control_data_ptr->old_rr_state != rr_control_data_ptr->rr_state))
    {
       rr_store_trace_msg_buf(
         RR_CONTROL_SM,
         new_rr_event,
         (byte) rr_control_data_ptr->rr_state,
         new_message_ptr,
         gas_id
       );
    }
#endif

    if ( rr_event == EV_PANIC_RESET )
    {
#ifndef PERLUTF
      MSG_GERAN_ERROR_0_G("Panic Reset - attempting to recover");
#else
      #error code not present
#endif

      if(grr_verify_saved_mm_message(RR_DEACT_REQ,gas_id))
      {
        MSG_GERAN_HIGH_0_G("Panic Reset while RR_DEACT_REQ saved - setting deact reason to RR_DEACT_REASON_DEACT_REQ");
        set_rr_deact_reason( RR_DEACT_REASON_DEACT_REQ,gas_id );
      }
      else if(grr_verify_saved_mm_message(RR_STOP_GSM_MODE_REQ,gas_id))
      {
        MSG_GERAN_HIGH_0_G("Panic Reset while RR_STOP_GSM_MODE_REQ saved - setting deact reason to RR_DEACT_REASON_STOP_GSM_MODE_REQ");
        set_rr_deact_reason( RR_DEACT_REASON_STOP_GSM_MODE_REQ,gas_id );
      }
      else
      {
        set_rr_deact_reason(RR_DEACT_REASON_LOWER_LAYER_FAILURE, gas_id);
      }

      if (rr_control_data_ptr->rr_panic_reset_info.l1_reset)
      {
        rr_event = rr_mode_control(EV_GSM_MODE_DEACTIVATED, NULL, gas_id);
        rr_control_data_ptr->rr_panic_reset_info.l1_reset = FALSE;
      }
      else
      {
        rr_event = rr_mode_control(EV_PANIC_RESET, NULL, gas_id);
      }

      rr_control_recovery_init(gas_id);
      rr_control_data_ptr->rr_state = RR_RESET;
    }
    else
    if (new_rr_event != EV_NO_EVENT)
    {
      rr_event = new_rr_event;
      new_rr_event = EV_NO_EVENT;
    }
    else
    {
      rr_event = EV_NO_EVENT;
    }

    /* if processing a previously saved input message */
    if ((rr_event == EV_INPUT_MESSAGE) && (new_message_ptr != NULL))
    {
      message_set = new_message_ptr->message_header.message_set;
      message_id  = new_message_ptr->message_header.message_id;

      /* log the processing of the input message */
      rr_show_input_message(
        RR_MESSAGE_TYPE_SAVED,
        new_message_ptr,
        message_set,
        message_id,
        rr_main_state_name(rr_control_data_ptr->rr_state),
        rr_control_data_ptr->rr_state,
        gas_id
      );
    }

    // Check for RR-CONTROL state change
    if (rr_control_data_ptr->rr_state != rr_control_data_ptr->old_rr_state)
    {
      // Check for transition to RR_INACTIVE
      if (rr_control_data_ptr->rr_state == RR_INACTIVE)
      {
        if (rr_mode_get_l1_stop_reason(gas_id) == RR_L1_RAT_CHANGE)
        {
#ifdef FEATURE_SGLTE
          if(rr_sglte_saved_l2g_ho_being_processed(gas_id))
          {
            MSG_GERAN_HIGH_0_G("SGLTE SRVCC Saved L2G HO being processed: RAT_CHANGE won't be sent");
          }
          else
#endif /*FEATURE_SGLTE*/
          {
          rr_l1_send_mph_rat_change_ind(gas_id);
        }

        }
      }

      RR_LOG_STATE_TRANSITION(
        "rr_control",
        rr_control_data_ptr->old_rr_state,
        rr_control_data_ptr->rr_state,
        rr_main_state_name,
        gas_id
      );

      // Log the change of state (log packet)
      (void) rr_log_state(
        (byte)rr_control_data_ptr->rr_state,   // state
        0,                                     // substate - ?
        0,                                     // status - ?
        0,                                     // mode - ?
        gas_id                                 // gas_id
      );

      rr_control_data_ptr->old_rr_state = rr_control_data_ptr->rr_state;
    }

  } /* End of the while statement */

} /* rr_control */

/**
 * Returns the name of the main RR state
 *
 * @param state  state of the RR
 *
 * @return text of the state name
 */
const char *rr_main_state_name(rr_state_T state)
{
  const char *s;
  switch (state)
  {
    case RR_INACTIVE:
      s = "RR_INACTIVE";
      break;

    case RR_GOING_ACTIVE:
      s = "RR_GOING_ACTIVE";
      break;

    case RR_GOING_INACTIVE:
      s = "RR_GOING_INACTIVE";
      break;

    case RR_CELL_SELECTION:
      s = "RR_CELL_SELECTION";
      break;

    case RR_PLMN_LIST_SEARCH:
      s = "RR_PLMN_LIST_SEARCH";
      break;

    case RR_IDLE:
      s = "RR_IDLE (not used)";
      break;

    case RR_CELL_RESELECTION:
      s = "RR_CELL_RESELECTION (not used)";
      break;

    case RR_CONNECTION_PENDING:
      s = "RR_CONNECTION_PENDING";
      break;

    case RR_CELL_REESTABLISH:
      s ="RR_CELL_REESTABLISH";
      break;

    case RR_DATA_TRANSFER:
      s = "RR_DATA_TRANSFER";
      break;

    case RR_NO_CHANNELS:
      s = "RR_NO_CHANNELS";
      break;

    case RR_CONNECTION_RELEASE:
      s = "RR_CONN_RELEASE";
      break;

    case RR_EARLY_CAMPED_WAIT_FOR_SI:
      s = "RR_EARLY_CAMPED_WAIT_FOR_SI";
      break;

    case RR_W2G_INTERRAT_HANDOVER_PROGRESS:
      s = "RR_W2G_INTERRAT_HANDOVER_PROG";
      break;

    case RR_W2G_INTERRAT_CC_ORDER_PROGRESS:
      s = "RR_W2G_INTERRAT_CC_ORDER_PROG";
      break;

    case RR_GRR:
      s = "RR_GRR";
      break;

    case RR_G2W_INTERRAT_HANDOVER_PROGRESS:
      s = "RR_G2W_INTERRAT_HANDOVER_PROG";
      break;

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
    case RR_W2G_SERVICE_REDIRECTION_IN_PROGRESS:
      s = "RR_W2G_SERVICE_REDIRECTION";
      break;
#endif

    case RR_RESET:
      s = "RR_RESET";
      break;


#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
    case RR_X2G_RESEL:
      s = "RR_X2G_RESEL";
      break;

    case RR_X2G_RESEL_ABORTED:
      s = "RR_X2G_RESEL_ABORTED";
      break;
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

    case RR_X2G_REDIR:
      s = "RR_X2G_REDIR";
      break;

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS_G2T_REDIRECTION)
    case RR_G2X_REDIR:
      s = "RR_G2X_REDIR";
      break;
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION */

#ifdef FEATURE_FEMTO_GSM_NL
    case RR_FEMTO:
      s = "RR_FEMTO";
      break;
#endif /* FEATURE_FEMTO_GSM_NL */

#ifdef FEATURE_LTE_TO_GSM_CGI
    case RR_X2G_CGI:
      s = "RR_X2G_CGI";
      break;
#endif /* FEATURE_LTE_TO_GSM_CGI */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
    case RR_X2G_CCO_FAILED:
      s = "RR_X2G_CCO_FAILED";
      break;

    case RR_X2G_CCO_ABORTED:
      s = "RR_X2G_CCO_ABORTED";
      break;

    case RR_X2G_CCO_FAILED_ABORTED:
      s = "RR_X2G_CCO_FAILED_ABORTED";
      break;
#endif /* FEATURE_WCDMA || FEATURE_GMS_TDS || FEATURE_LTE */

    case RR_CELL_SELECTED_ACQUIRE_SI:
      s = "RR_CELL_SELECTED_ACQUIRE_SI";
      break;

    default:
      MSG_GERAN_ERROR_1("Unknown state: %d", state);
      s = "Unknown";
      break;
  } /* switch */

  return s;
} /* rr_main_state_name */

const char *rr_main_current_state_name(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_control_data_ptr->rr_state == RR_GRR)
  {
    return grr_control_current_state_name(gas_id);
  }
  else
  {
    return rr_main_state_name(rr_control_data_ptr->rr_state);
  }
}

/**
 * Handles RR_NW_SEL_MODE_RESET_IND
 *
 * @param p_nw_sel_mode
 *
 * @param gas_id
 */
static void rr_handle_nw_sel_mode_reset_ind(
  const rr_nw_sel_mode_reset_ind_T *p_nw_sel_mode,
  const gas_id_t gas_id
)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);

  if (p_nw_sel_mode == NULL)
  {
    MSG_GERAN_ERROR_0_G("p_nw_sel_mode=NULL");
    return;
  }

  MSG_GERAN_HIGH_2_G(
    "Received RR_NW_SEL_MODE_RESET_IND, network_select_mode new: %d, old: %d",
    p_nw_sel_mode->network_select_mode,
    rr_PLMN_request_details_ptr->network_select_mode
  );


  /* if network_select_mode was LIMITED or LIMITED_ROUTINE_ACQUISITION
   * MM will give an invalid PLMN-id to RR (FF) in PLMN_SELECT_REQ.
   * If RR receive RR_NW_SEL_MODE_RESET_IND indicates mode change to full service,
   * RR will need to copy current serving cell's PLMN id to rr_plmn_request_details
   */
  if ((rr_PLMN_request_details_ptr->network_select_mode ==
         SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) ||
      (rr_PLMN_request_details_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_LIMITED))
  {
    gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

    if (scell_info_ptr->gsm.valid_data_flag)
    {
      MSG_GERAN_HIGH_3_G("overwrite PLMN-id old:%6x, new:%6x", \
        (rr_PLMN_request_details_ptr->requested_PLMN_id.identity[0]<<16)+ \
        (rr_PLMN_request_details_ptr->requested_PLMN_id.identity[1]<< 8)+ \
         rr_PLMN_request_details_ptr->requested_PLMN_id.identity[2],
        (scell_info_ptr->gsm.location_area_identification.plmn_id.identity[0]<<16)+ \
        (scell_info_ptr->gsm.location_area_identification.plmn_id.identity[1]<< 8)+ \
         scell_info_ptr->gsm.location_area_identification.plmn_id.identity[2],
        0
      );

      rr_PLMN_request_details_ptr->requested_PLMN_id =
        scell_info_ptr->gsm.location_area_identification.plmn_id;
    }
  }

  rr_PLMN_request_details_ptr->network_select_mode = p_nw_sel_mode->network_select_mode;

  return;
}

/**
 * Checks if the rr state machine is in correct state to handle
 * RR_RESEL_ABORT_REQ signal.
 *
 * @return - boolean
 *
 */
boolean rr_can_process_resel_abort_req(
  const gas_id_t gas_id
)
{
  boolean process = FALSE;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_GRR:
    {
      process = grr_can_process_rr_resel_abort_req(gas_id);
    }
    break;

    default:
    {
      process = TRUE;
    }
    break;
  }

  return process;
}

#ifdef FEATURE_LTE
boolean rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(const gas_id_t gas_id)
{
  if (rr_is_lte_enabled(gas_id))
  {
    return rr_nv_get_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(RR_GAS_ID_TO_AS_ID);
  }
  else
  {
    return FALSE;
  }
}

boolean rr_g2l_blind_redir_after_csfb_enabled(const gas_id_t gas_id)
{
  if (rr_is_lte_enabled(gas_id))
  {
    return rr_nv_get_g2l_blind_redir_after_csfb_enabled(RR_GAS_ID_TO_AS_ID);
  }
  else
  {
    return FALSE;
  }
}

boolean rr_g2l_blind_redir_after_srvcc_enabled(const gas_id_t gas_id)
{
  if (rr_is_lte_enabled(gas_id))
  {
    return rr_nv_get_g2l_blind_redir_after_srvcc_enabled(RR_GAS_ID_TO_AS_ID);
  }
  else
  {
    return FALSE;
  }
}
#endif /* FEATURE_LTE */

/**
 * Checks if the rr state machine is in a suitable state to handle RR_IMSG_SAVED_MSG_IND
 *
 * @return boolean TRUE if the message can be processed
 */
boolean rr_can_process_saved_msg_ind(
  const gas_id_t gas_id
)
{
  boolean process = TRUE;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    default: ;
  }

  return process;
}

/**
 * Checks if the rr state machine is in suitable state to handle RR_IMSG_RR_EVENT_IND
 *
 * @return boolean TRUE if the message can be processed
 */
boolean rr_can_process_rr_event_ind(
  const gas_id_t gas_id
)
{
  boolean process = TRUE;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    default: ;
  }

  return process;
}

/**
 * Checks if RR-CONTROL is in a suitable state to handle a MM message
 *
 * @return boolean TRUE if the message can be processed
 */
boolean rr_can_process_mm_message(
  const gas_id_t gas_id
)
{
  boolean process = TRUE;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (rr_control_data_ptr->rr_saved_msg.valid &&
      (rr_control_data_ptr->rr_saved_msg.msg.message_header.message_set == MS_MM_RR))
  {
    process = FALSE;
  }
  else
  {
    switch (rr_control_data_ptr->rr_state)
    {
      case RR_RESET:
      {
        process = FALSE;
        break;
      }

      case RR_GRR:
      {
        process = grr_can_process_mm_message(gas_id);
        break;
      }

      case RR_CONNECTION_PENDING:
      {
        process = rr_ce_can_process_mm_message(gas_id);
        break;
      } /* RR_CONNECTION_PENDING */

      default: ;
    }
  }

  return process;
}

/**
 * Checks if RR-CONTROL is in a suitable state to handle a timer expiry
 *
 * @return boolean TRUE if the message can be processed
 */
boolean rr_can_process_timer_expiry(
  rr_timer_id_e timer_id,
  const gas_id_t gas_id
)
{
  boolean process = TRUE;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (timer_id)
  {
    // The following timers must ALWAYS be handled
    case GRR_GUARD_TIMER:
    case RR_L1_GUARD_TIMER:
    case RR_GRR_CONTROL_STATE_TRANSITION_GUARD_TIMER:
    case RR_RESEL_CONTROL_STATE_TRANSITION_GUARD_TIMER:
    case RR_CELL_ACQ_CONTROL_STATE_TRANSITION_GUARD_TIMER:
    {
      break;
    }

    // Block timer handling according to state
    default:
    {
      switch (rr_control_data_ptr->rr_state)
      {
        case RR_GRR:
        {
          process = grr_can_process_timer_expiry(gas_id);
          break;
        }

        default: ;
      }
    }
  }

  return process;
}

/**
 * Checks if RR-CONTROL is in a suitable state to handle a serving cell measurement indication
 *
 * @return boolean TRUE if the message can be processed
 */
boolean rr_can_process_serving_meas_ind(
  const gas_id_t gas_id
)
{
  boolean process = TRUE;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_GRR:
    {
      process = grr_can_process_serving_meas_ind(gas_id);
      break;
    }

    default: ;
  }

  return process;
}

/**
 * Checks if RR-CONTROL is in a suitable state to handle a surround cell measurement indication
 *
 * @return boolean TRUE if the message can be processed
 */
boolean rr_can_process_surround_meas_ind(
  const gas_id_t gas_id
)
{
  boolean process = TRUE;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_GRR:
    {
      process = grr_can_process_surround_meas_ind(gas_id);
      break;
    }

    default: ;
  }

  return process;
}

#ifdef FEATURE_LTE

boolean rr_can_process_csfb_blind_redir_to_lte_req(
  const gas_id_t gas_id
)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_DATA_TRANSFER:
    {
      /* This is the main state that this signal is expected to be received
      in, so processing is possible. */
      return TRUE;
    } /* RR_DATA_TRANSFER */

    case RR_CONNECTION_RELEASE:
    {
      /*  RR should process the blind redirection request in RR_CONNECTION_PENDING 
        as if it were received in RR_DATA_TRANSFER state. This is because RR needs to perform
        blind redirection if Raching fails,instead of going to Cell Reselection state*/

      return TRUE;
    } /* RR_CONNECTION_RELEASE */

        case RR_CONNECTION_PENDING:
    {
      /* To amake sure that RR performs a blind redirection is performed if T3126 expires,
             or RACH failure occurs for some other reason,this message needs to be processed in
              RR_CONNECTION_PENDING state. If not processed, then RR would perform reselection
              as in legacy implementation */
              
      return TRUE;
    } /* RR_CONNECTION_PENDING */

    case RR_INACTIVE:
    {
      /* Processing of RR_CSFB_BLIND_REDIR_TO_LTE_REQ is allowed in these
      states so RR does not get one of these signals 'stuck' in it's queue if
      it missed somehow in other states. */
      return TRUE;
    } /* RR_INACTIVE / RR_GRR */

    default:
    {
      /* In all other states, RR_CSFB_BLIND_REDIR_TO_LTE_REQ is buffered, the
      assumption being that it should be processed when RR_DATA_TRANSFER is
      reached. */
      return FALSE;
    }
  }
}

#endif /* FEATURE_LTE */


boolean rr_can_process_change_mode_ind(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_GRR:
    {
      /* Processing of RR_CSFB_BLIND_REDIR_TO_LTE_REQ is allowed in these
      states so RR does not get one of these signals 'stuck' in it's queue if
      it missed somehow in other states. */
      return grr_can_process_change_mode_ind(gas_id);
    } /* RR_GRR */

    default:
    {
      /* In all other states, RR_CSFB_BLIND_REDIR_TO_LTE_REQ is buffered, the
      assumption being that it should be processed when RR_DATA_TRANSFER is
      reached. */
      return TRUE;
    }
  }

}

boolean rr_can_process_rr_abort_req(const gas_id_t gas_id)
{
  boolean process_message;
  
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  process_message = TRUE;
  
  switch (rr_control_data_ptr->rr_state)
  {
    case RR_CELL_SELECTION:
    case RR_EARLY_CAMPED_WAIT_FOR_SI:
    {
      /* If RR_ABORT_REQ() is received from NAS, while RR has saved RR_EST_REQ()
         and is in middle of Camping Procedure, allow processing */
      if (rr_control_data_ptr->rr_saved_msg.valid &&
        (rr_control_data_ptr->rr_saved_msg.msg.message_header.message_set == MS_MM_RR))
      {
         
         MSG_GERAN_HIGH_1_G("rr_can_process_rr_abort_req() RR_ABORT_REQ: in RR State %d", rr_control_data_ptr->rr_state );
         process_message = rr_saved_msg_is_rr_est_req(gas_id);
      }        
      break;
    }
            
    case RR_GRR:
    {
      /* Rely on GRR FSM to handle RR_ABORT_REQ() */      
      if (rr_control_data_ptr->rr_saved_msg.valid &&
        (rr_control_data_ptr->rr_saved_msg.msg.message_header.message_set == MS_MM_RR))
      {
         process_message = rr_saved_msg_is_rr_est_req(gas_id);
      }  

      if ( process_message) 
      {
        process_message = grr_can_process_rr_abort_req(gas_id );
      }
      
      break;
    } /* RR_GRR */

    default:
    {
      process_message = rr_can_process_mm_message(gas_id);
    }
  }

  return(process_message);
}

/**
 * Checks if the rr state machine is in suitable state to handle RR_IMSG_MSC_CHANGE_IND
 *
 * @return boolean TRUE if the message can be processed
 */
boolean rr_can_process_msc_change_ind(
  const gas_id_t gas_id
)
{
  boolean process = TRUE;

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
#ifdef FEATURE_GSM_DTM
    case RR_DATA_TRANSFER:
    {
      process = rr_dtm_can_process_msc_change_ind(gas_id);
      break;
    }
#endif

    default: 
    {
      if (rr_resel_reselection_is_pending(gas_id))
      {
        process = FALSE;
      }
    }
  }

  return process;
}


#ifdef FEATURE_SGLTE

boolean rr_can_process_stop_gsm_mode_req_sglte_rat_change(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  boolean            process;

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_INACTIVE:
    case RR_DATA_TRANSFER:
    {
      if (rr_is_pending_mode_change(gas_id) )
      {
        MSG_GERAN_HIGH_1_G(
        "Processing of STOP GSM MODE Blocked due to Pending Mode Change in state: %d",
         rr_control_data_ptr->rr_state
        );
        process = FALSE;
      }
      else
      {
      process = TRUE;
      }
      break;
    } /* RR_INACTIVE ... */

    case RR_G2X_REDIR:
    {
      process = TRUE;
      break;
    } /* RR_G2X_REDIR */

    case RR_GRR:
    {
      process = grr_can_process_stop_gsm_mode_req_sglte_rat_change(gas_id);
      break;
    } /* RR_GRR */

    default:
    {
      process = FALSE;
    }
  }

  return process;
}

boolean rr_can_process_sglte_x2g_req(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  boolean            process;

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_INACTIVE:
    case RR_DATA_TRANSFER:
    {
      process = TRUE;
      break;
    } /* RR_INACTIVE ... */

    case RR_GRR:
    {
      process = grr_can_process_sglte_x2g_req(gas_id);
      break;
    } /* RR_GRR */

    default:
    {
      process = FALSE;
    }
  }

  return process;
}

boolean rr_can_process_sglte_l2g_ho_req(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  boolean process= TRUE;

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_GOING_INACTIVE:
    {
      process = FALSE;
      break;
    } /* RR_INACTIVE ... */
    default:
    {
      process = TRUE;
    }
  }

  return process;
}

boolean rr_can_process_sglte_band_coex_omsg_supported_bands_updated_ind(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  boolean            process;

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_INACTIVE:
    case RR_DATA_TRANSFER:
    {
      process = TRUE;
      break;
    } /* RR_INACTIVE ... */

    case RR_GRR:
    {
      process = grr_can_process_sglte_band_coex_omsg_supported_bands_updated_ind(gas_id);
      break;
    } /* RR_GRR */

    default:
    {
      process = FALSE;
    }
  }

  return process;
}

#endif /* FEATURE_SGLTE */

#ifdef FEATURE_LTE
static boolean rr_is_fast_si_acq_during_x2g_allowed(gas_id)
{
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;

  if (rr_nv_x2g_fast_si_acq_feature_is_enabled(as_id) &&
      rr_nv_x2g_fast_si_acq_is_plmn_match(rr_PLMN_request_details_ptr->requested_PLMN_id, as_id))
  {
    MSG_HIGH("PLMN in white list SKIPPING Enabled",0,0,0);    
    return TRUE;
  }
  else
  {
    MSG_HIGH("PLMN not in white list SKIPPING disabled",0,0,0);  
    return FALSE;
  }
}
#endif //#ifdef FEATURE_LTE

/**
 * Checks if the rr state machine is in suitable state to handle RR_IMSG_NO_PS_ACCESS_REQ
 *
 * @return boolean TRUE if the message can be processed
 */
boolean rr_can_process_no_ps_access_req(const gas_id_t gas_id)
{
  boolean process = TRUE;

  switch (rr_control_get_data_ptr(gas_id)->rr_state)
  {
#ifdef FEATURE_GSM_DTM
    case RR_DATA_TRANSFER:
    {
      process = rr_dtm_can_process_no_ps_access_req(gas_id);
      break;
    }
#endif

    default: ;
  }

  return process;
}

/**
 * Checks if the rr state machine is in suitable state to handle RR_IMSG_PS_ACCESS_REQ
 *
 * @return boolean TRUE if the message can be processed
 */
boolean rr_can_process_ps_access_req(const gas_id_t gas_id)
{
  boolean process = TRUE;

  switch (rr_control_get_data_ptr(gas_id)->rr_state)
  {
#ifdef FEATURE_GSM_DTM
    case RR_DATA_TRANSFER:
    {
      process = rr_dtm_can_process_ps_access_req(gas_id);
      break;
    }
#endif

    default: ;
  }

  return process;
}

#ifdef FEATURE_GSM_TX_DIV_ANTNA_SWITCHING

boolean rr_get_ue_oos_flag(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_MED_1_G("ue_oos: %d", rr_control_data_ptr->ue_oos);
  return rr_control_data_ptr->ue_oos;
}

void rr_set_ue_oos_flag(boolean ue_oos, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_MED_1_G("ue_oos = %d", ue_oos);
  rr_control_data_ptr->ue_oos = ue_oos;
}

#endif /* FEATURE_GSM_TX_DIV_ANTNA_SWITCHING */

boolean rr_is_mtcsfb_call_in_progress(const gas_id_t gas_id) 
{ 
  // Obtain a pointer to the module data 
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id); 
  RR_NULL_CHECK_FATAL(rr_control_data_ptr); 
  
  return rr_control_data_ptr->csmt; 
} 

boolean rr_is_csfb_call_in_progress(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return rr_control_data_ptr->is_csfb_call_in_progress;
}

boolean rr_is_fast_si_acq_enabled(
  boolean * wait_for_si13,
  const gas_id_t gas_id
)
{
  boolean nv_dtm_enabled;
  sys_modem_as_id_e_type as_id = RR_GAS_ID_TO_AS_ID;
  boolean anite_gcf_flag = rr_get_nv_anite_gcf_flag(as_id);

  /* Wait for SI13 by default. */
  if (wait_for_si13 != NULL)
  {
    *wait_for_si13 = TRUE;
  }

  #ifdef FEATURE_GSM_DTM
  nv_dtm_enabled = rr_get_nv_dtm_enabled(as_id);
#else
  nv_dtm_enabled = FALSE;
#endif // FEATURE_GSM_DTM

MSG_GERAN_HIGH_5_G(
    "Fast SI Acq : anite_gcf_flag=%d service_domain=%d is_csfb_call_in_progress=%d, nv_dtm_enabled=%d, nv_fast_si_acq_during_csfb=%d",
	(int)anite_gcf_flag,
	rr_sel_get_service_domain(gas_id),
    rr_is_csfb_call_in_progress(gas_id),
    nv_dtm_enabled,
    rr_get_nv_fast_si_acq_during_csfb_enabled(as_id)
  );

#ifdef FEATURE_GSM_TDS
  MSG_GERAN_HIGH_2_G(
    "t2g_resel_in_progress= %d, t2g_cco_in_progress=%d",
    rr_t2g_resel_in_progress(gas_id),
    rr_t2g_cco_in_progress(gas_id)
  );
#endif

#ifdef FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT
  if (rr_x2g_redir_with_si_in_progress(gas_id))
  {
    if (rr_is_csfb_call_in_progress(gas_id))
    {
     if (rr_get_nv_fast_si_acq_during_csfb_enabled(as_id) && !anite_gcf_flag)
     {
      /* For CSFB calls, we do not wait for SI13. */
      if (wait_for_si13 != NULL)
      {
        *wait_for_si13 = FALSE;
         MSG_GERAN_HIGH_0_G("SI13 skipped for tunnelled CSFB case");         
      }
     }
    }  
    return TRUE;
  }
  else
#endif /* FEATURE_LTE_TO_GSM_REDIR_WITH_SI_SUPPORT */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  if (rr_x2g_cco_with_nacc_in_progress(gas_id))
  {
    return TRUE;
  }
  else
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
  if ((rr_t2g_resel_in_progress(gas_id) || rr_t2g_cco_in_progress(gas_id) || rr_t2g_redirection_in_progress(gas_id) ) && !nv_dtm_enabled && !anite_gcf_flag)
  {
    MSG_GERAN_HIGH_0_G("Fast SI acq for TDS");

    /* For T2G resel, we do not wait for SI13. */
    if (wait_for_si13 != NULL)
    {
      *wait_for_si13 = FALSE;
    }
    return TRUE;
  }
  else
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_LTE
	if ( (rr_l2g_resel_in_progress(gas_id) || rr_l2g_cco_in_progress(gas_id)|| rr_l2g_redirection_in_progress(gas_id)) && 
		 ((!rr_get_nv_dtm_enabled(as_id)) && (!rr_get_nv_anite_gcf_flag(as_id)) && (rr_is_fast_si_acq_during_x2g_allowed(gas_id))) )
	{
	  MSG_HIGH("Fast SI acq for L2G Redire/Resel/CCO",0,0,0);
  
	  /* For T2G resel, we do not wait for SI13. */
	  if (wait_for_si13 != NULL)
	  {
		*wait_for_si13 = FALSE;
	  }
	  return TRUE;
	}
	else
#endif /*FEATURE_LTE*/

  /* The check for Fast SI Acq during CSFB call without network SI provided
  should always come after the check for X2G redirection with tunnelled SIs,
  as otherwise during an X2G redirection with tunnelled SIs but with DTM
  enabled, we will not perform Fast SI Acq. */
  if (rr_is_csfb_call_in_progress(gas_id))
  {
    if (rr_get_nv_fast_si_acq_during_csfb_enabled(as_id) && !anite_gcf_flag)
    {
      /* For CSFB calls, we do not wait for SI13. */
      if (wait_for_si13 != NULL)
      {
        *wait_for_si13 = FALSE;
      }
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  if ((SYS_SRV_DOMAIN_CS_ONLY == rr_sel_get_service_domain(gas_id)) &&
      rr_nv_fast_si_acq_when_cs_only_is_enabled(as_id))
  {
    MSG_GERAN_HIGH_0_G("No need to wait for SI13 when domain is set to CS only");
    if (wait_for_si13 != NULL)
    {
      *wait_for_si13 = FALSE;
    }
    return TRUE;
  }

  // Default - feature disabled
  return FALSE;
}

/*!
 * \brief Indicates if C1 relaxation is enabled for the supplied PLMN.
 *
 * \param plmn (in)
 * \param gas_id (in)
 *
 * \return boolean - TRUE if C1 relaxation is enabled, FALSE otherwise
 */
boolean rr_allow_negative_c1(
  const sys_plmn_id_s_type plmn,
  const gas_id_t           gas_id
)
{
  if (rr_nv_c1_relaxation_is_plmn_match(plmn,gas_id))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

boolean rr_g2w_measurements_enabled_in_nv(gas_id_t gas_id)
{
  return rr_nv_get_g2w_measurements_enabled(RR_GAS_ID_TO_AS_ID);
}

boolean rr_g2t_measurements_enabled_in_nv(gas_id_t gas_id)
{
  return rr_nv_get_g2t_measurements_enabled(RR_GAS_ID_TO_AS_ID);
}

boolean rr_g2l_measurements_enabled_in_nv(gas_id_t gas_id)
{
  return rr_nv_get_g2l_measurements_enabled(RR_GAS_ID_TO_AS_ID);
}

#ifdef FEATURE_GSM_TDS
/**
  @brief Determines if T2G reselection is in progress

  @param None
  @return TRUE T2G resel is in progress, FALSE otherwise.
*/
boolean rr_t2g_resel_in_progress(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->x2g_resel_type)
  {
    case RR_X2G_RESEL_TYPE_T2G_RESELECTION:
      return TRUE;

    default:
      return FALSE;
  }
}

/**
  @brief Determines if T2G CCO is in progress

  @param None
  @return TRUE T2G CCO is in progress, FALSE otherwise.
*/
boolean rr_t2g_cco_in_progress(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->x2g_resel_type)
  {
    case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
      return TRUE;

    default:
      return FALSE;
  }
}

#endif /* FEATURE_GSM_TDS */

#ifdef FEATURE_LTE

/**
  @brief Determines if L2G reselection is in progress

  @param None
  @return TRUE L2G resel is in progress, FALSE otherwise.
*/
boolean rr_l2g_resel_in_progress(const gas_id_t gas_id)
{

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->x2g_resel_type)
  {
    case RR_X2G_RESEL_TYPE_L2G_RESELECTION: return TRUE;

    default: return FALSE;
  }
}

/**
  @brief Determines if L2G CCO is in progress

  @param None
  @return TRUE L2G CCO is in progress, FALSE otherwise.
*/
boolean rr_l2g_cco_in_progress(const gas_id_t gas_id)
{

  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->x2g_resel_type)
  {
    case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER: return TRUE;

    default: return FALSE;
  }
} 




#endif /*FEATURE_LTE*/

#ifdef FEATURE_SGLTE
/**
  @brief During a SGLTE X2G CCO a higher layer procedure moves
         PS domain back to X.  GERAN should give a CCO success
         indication to X in this case and complete the CCO
         procedure.

*/
void rr_sglte_x2g_cco_nas_split_cb_fn(gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);

  MSG_GERAN_HIGH_1_G("SGLTE: X2G CCO (x2g_resel_type:%d) interrupted by NAS split", rr_control_data_ptr->x2g_resel_type);

  switch (rr_control_data_ptr->x2g_resel_type)
  {
#ifdef FEATURE_LTE
    case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
    {
      geran_grr_lte_cco_rsp_t lte_cco_rsp;
      errno_enum_type         result;

      memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

      msgr_init_hdr(
        &lte_cco_rsp.hdr,
        MSGR_GERAN_GRR,
        GERAN_GRR_LTE_CCO_RSP
      );

      lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_SUCCESS;

      result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      break;
    } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
    case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
    {
      rr_send_tds_interrat_cc_order_cnf(
        TDSCCO_SUCCESS,
        TDSRR_GSM_CCO_OTHER_REASON,
        gas_id
      );
      break;
    } /* RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER */
#endif /*FEATURE_GSM_TDS*/

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected x2g_resel_type: %d", rr_control_data_ptr->x2g_resel_type);
    }
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
  rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
  rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
  rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
  rr_control_data_ptr->is_cco_for_csfb = FALSE;
  rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
  rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;

  rr_set_panic_reset_x2g_cco_flag(RR_X2G_RESEL_TYPE_UNDEFINED, gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  rr_timer_cancel(T3174, gas_id);

  rr_control_data_ptr->sglte_pending_rat_pri_list_valid = FALSE;

  rr_send_stop_gsm_mode_cnf(MM_AS_SUCCESS, gas_id);
  grr_update_mm_with_rr_service_ind(gas_id);
}

/**
  @brief Sends a failure indication to the originating RAT after an SGLTE
         X2G CCO failure.

  The failure phase should always indicate that an activation ind has been
  sent, as this will always be the case.  There is no acquisition phase for
  SGLTE X2G CCO, as it can only occur when GERAN is in service.
*/
void rr_sglte_x2g_cco_failed_cb_fn(gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_HIGH_1_G("SGLTE: X2G CCO failure (x2g_resel_type:%d)", rr_control_data_ptr->x2g_resel_type);

  switch (rr_control_data_ptr->x2g_resel_type)
  {
#ifdef FEATURE_LTE
    case RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER:
    {
      geran_grr_lte_cco_rsp_t lte_cco_rsp;
      errno_enum_type         result;

      memset(&lte_cco_rsp, 0, sizeof(lte_cco_rsp));

      msgr_init_hdr(
        &lte_cco_rsp.hdr,
        MSGR_GERAN_GRR,
        GERAN_GRR_LTE_CCO_RSP
      );

      if (rr_get_x2g_cco_act_rsp_rcvd(gas_id))
      {
        lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_RACH;
      }
      else
      {
        lte_cco_rsp.status = GERAN_GRR_LTE_CCO_STATUS_FAILURE_TO_ACQUIRE;
      }

      rr_set_x2g_cco_act_rsp_rcvd(FALSE, gas_id);
      result = rr_msgr_send(&lte_cco_rsp.hdr, sizeof(lte_cco_rsp), gas_id);
      if (result != E_SUCCESS)
      {
        MSG_GERAN_ERROR_1_G("rr_msgr_send failed: %d", result);
      }
      break;
    } /* RR_X2G_RESEL_TYPE_L2G_CELL_CHANGE_ORDER */
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
    case RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER:
    {
      if (rr_get_x2g_cco_act_rsp_rcvd(gas_id))
      {
        rr_send_tds_interrat_cc_order_cnf(
        TDSCCO_FAILURE_AFTER_ACT_IND,
        TDSRR_GSM_CCO_T309_EXPIRED,
        gas_id
      );
      }
      {
        rr_send_tds_interrat_cc_order_cnf(
        TDSCCO_FAILURE_BEFORE_ACT_IND,
        TDSRR_GSM_CCO_T309_EXPIRED,
        gas_id
      );
      }
      rr_set_x2g_cco_act_rsp_rcvd(FALSE, gas_id);
      break;
    } /* RR_X2G_RESEL_TYPE_T2G_CELL_CHANGE_ORDER */
#endif /*FEATURE_GSM_TDS*/

    default:
    {
      MSG_GERAN_ERROR_1_G("Unexpected x2g_resel_type: %d", rr_control_data_ptr->x2g_resel_type);
    }
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
  rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
  rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
  rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
  rr_control_data_ptr->is_cco_for_csfb = FALSE;
  rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
  rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;

  rr_set_panic_reset_x2g_cco_flag(RR_X2G_RESEL_TYPE_UNDEFINED, gas_id);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  rr_control_data_ptr->sglte_pending_rat_pri_list_valid = FALSE;
}

/**
  @brief Sends an abort confirmation to the originating RAT after an SGLTE
         X2G CCO is aborted by the calling RAT.
*/
void rr_sglte_x2g_cco_aborted_cb_fn(gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_HIGH_1_G("SGLTE: X2G CCO aborted (x2g_resel_type:%d)", rr_control_data_ptr->x2g_resel_type);

  rr_x2g_send_x2g_cco_abort_rsp(
    rr_map_x2g_resel_type_to_rat(rr_control_data_ptr->x2g_resel_type),
    gas_id
  );

  MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
  rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
  rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
  rr_control_data_ptr->x2g_cco_status = RR_CCO_STATUS_UNDEFINED;
  rr_control_data_ptr->is_cco_for_csfb = FALSE;
  rr_control_data_ptr->x2g_cco_pending_network_control_order_valid = FALSE;
  rr_control_data_ptr->x2g_cco_pending_network_control_order = 0;

  rr_set_panic_reset_x2g_cco_flag(RR_X2G_RESEL_TYPE_UNDEFINED, gas_id);

  rr_control_data_ptr->sglte_pending_rat_pri_list_valid = FALSE;
}

/**
  @brief Sends an abort confirmation to the originating RAT after an SGLTE
         X2G Redirection is aborted by the calling RAT.
*/
void rr_sglte_x2g_redir_aborted_cb_fn(gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_HIGH_1_G("SGLTE: X2G Redirection aborted (x2g_resel_type:%d)", rr_control_data_ptr->x2g_resel_type);

  rr_x2g_send_x2g_redir_abort_rsp(
    rr_map_x2g_resel_type_to_rat(rr_control_data_ptr->x2g_resel_type),
    gas_id
  );

  MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
  rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
  rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
  rr_control_data_ptr->sglte_pending_rat_pri_list_valid = FALSE;
}

/**
  @brief Sends an abort confirmation to the originating RAT after an SGLTE
         X2G Reselection is aborted by the calling RAT.
*/
void rr_sglte_x2g_resel_aborted_cb_fn(gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_HIGH_1_G("SGLTE: X2G Reselection aborted (x2g_resel_type:%d)", rr_control_data_ptr->x2g_resel_type);

  rr_x2g_send_x2g_resel_abort_rsp(
    rr_map_x2g_resel_type_to_rat(rr_control_data_ptr->x2g_resel_type),
    gas_id
  );

  MSG_GERAN_MED_0_G("Clearing x2g_resel_type / x2g_resel_abort_cause");
  rr_control_data_ptr->x2g_resel_type = RR_X2G_RESEL_TYPE_UNDEFINED;
  rr_control_data_ptr->x2g_resel_abort_cause = RR_X2G_RESEL_ABORT_CAUSE_UNDEFINED;
  rr_control_data_ptr->sglte_pending_rat_pri_list_valid = FALSE;
}


boolean rr_sglte_g2x_bplmn_search_active(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return rr_control_data_ptr->sglte_g2x_plmn_list_search_in_progress;
}

boolean rr_sglte_inhibit_tds_bplmn_search(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_tds_search;
}

boolean rr_sglte_inhibit_lte_bplmn_search(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_lte_search;
}

#ifdef FEATURE_SGLTE
static boolean rr_control_can_process_sglte_plmn_search(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  boolean            search_handled = FALSE;

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  switch (rr_control_data_ptr->rr_state)
  {
    case RR_CONNECTION_PENDING:
    case RR_CONNECTION_RELEASE:
      search_handled = TRUE;
    break;

    case RR_GRR:
      search_handled = grr_can_process_sglte_plmn_search(gas_id);
    break;

   case RR_DATA_TRANSFER:
 #ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
      search_handled = rr_sglte_g2x_conn_mode_resel_can_process_plmn_search(gas_id);
 #else
     search_handled  = TRUE;
 #endif /*FEATURE_SGLTE_G2X_CONN_MODE_RESEL*/
    break;   

    default:
      search_handled = FALSE;
    break;
  }

  return search_handled;
}

static boolean rr_is_rat_in_plmn_search_list(rr_plmn_list_req_T * plmn_list_req_ptr,
                                             sys_radio_access_tech_e_type rat)
{
  boolean rat_in_list = FALSE;
  uint8 i = 0;

  for (i=0; i<plmn_list_req_ptr->plmn_list.length; i++)
  {
    if (plmn_list_req_ptr->plmn_list.info[i].rat == rat)
    {
      rat_in_list = TRUE;
      break;
    }
  }

  return rat_in_list;
}

/**
 * @brief
 * Override decisions for SGLTE PLMN List search inhibit.
 * Default return of TRUE if no decision is made.
 *
 * @return boolean TRUE if the search should be inhibited, FALSE
 *         if not.
 */
static boolean rr_sglte_irat_neighbour_inhibit_plmn_list(boolean g2x_resel_in_ded_mode, const gas_id_t gas_id)
{
  boolean inhibit_plmn_list_search = TRUE; /*default to TRUE*/

#if defined(FEATURE_SGLTE) && defined(FEATURE_SGLTE_G2X_CONN_MODE_RESEL)
  /*If we are performing SGLTE G2X reselection in dedicated mode then we do not want to inhibit the
    PLMN search if we are in a call and IRAT measurements are not possible (when multimode sub is on WTR2605,
    IRAT measurements are not possible to multimode WTR16x5)*/
  if (g2x_resel_in_ded_mode)
  {
    inhibit_plmn_list_search = rr_sglte_g2x_resel_conn_inhibit_plmn_search(gas_id);
  }
#endif /*FEATURE_SGLTE && FEATURE_SGLTE_G2X_CONN_MODE_RESEL*/

  MSG_GERAN_HIGH_1_G("RR SGLTE PLMN List search inhibit search: %d",inhibit_plmn_list_search);

  return inhibit_plmn_list_search;
}

static boolean rr_start_sglte_plmn_list_search(rr_plmn_list_req_T * plmn_list_req_ptr, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  gprs_scell_info_t  *scell_info_ptr = rr_gprs_get_scell_info(gas_id);
  rr_nw_supp_utran_rats_t nw_supp_rats = rr_nw_supported_utran_rats(scell_info_ptr);
  rr_PLMN_request_details_T *rr_PLMN_request_details_ptr = rr_get_PLMN_request_details_ptr(gas_id);
  boolean start_sglte_search = FALSE;
  boolean is_sglte_irat_ded_meas_possible = FALSE;

  rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_lte_search = FALSE;
  rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_tds_search = FALSE;

  if (rr_is_sglte_mode_active(gas_id) &&
      plmn_list_req_ptr->service_search == SYS_SERVICE_SEARCH_IRAT_PLMN)
  {
    MSG_GERAN_HIGH_3_G("RR SGLTE BPLMN (sel_mode:%d serv_dom:%d gsm in list:%d)",
                        plmn_list_req_ptr->network_selection_mode,
                        rr_PLMN_request_details_ptr->service_domain,
                        rr_is_rat_in_plmn_search_list(plmn_list_req_ptr,SYS_RAT_GSM_RADIO_ACCESS));

    if ( rr_control_data_ptr->rr_state == RR_DATA_TRANSFER )
    {
#ifdef FEATURE_SGLTE_G2X_CONN_MODE_RESEL
      if (rr_get_nv_sglte_g2x_conn_mode_resel_enabled(RR_GAS_ID_TO_AS_ID) )
      {
        is_sglte_irat_ded_meas_possible = TRUE;
      }
#endif
    }


    /*only check if the requested RAT is a neighbour if we are not in dedicated mode or sglte connected mode reselections are not allowed? */
    if (rr_control_data_ptr->rr_state != RR_CONNECTION_PENDING &&
        ( (rr_control_data_ptr->rr_state != RR_DATA_TRANSFER) || is_sglte_irat_ded_meas_possible ) &&
        rr_control_data_ptr->rr_state != RR_CONNECTION_RELEASE)
    {
#ifdef FEATURE_LTE
      if (rr_is_rat_in_plmn_search_list(plmn_list_req_ptr,SYS_RAT_LTE_RADIO_ACCESS) == TRUE)
      {
        if (rr_is_lte_in_neighbour_list(gas_id) == TRUE)
        {
          rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_lte_search = rr_sglte_irat_neighbour_inhibit_plmn_list(is_sglte_irat_ded_meas_possible, gas_id);
        }

        MSG_GERAN_HIGH_1_G("RR SGLTE BPLMN LTE in PLMN list, LTE is neighbour:%d",rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_lte_search);
      }
      else
      {
        MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN LTE NOT in PLMN list");
        rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_lte_search = TRUE;
      }
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS
      if (rr_is_rat_in_plmn_search_list(plmn_list_req_ptr,SYS_RAT_TDS_RADIO_ACCESS) == TRUE)
      {
        if ((nw_supp_rats == RR_NW_SUPP_UTRAN_TDD ||
            nw_supp_rats == RR_NW_SUPP_UTRAN_TDD_FDD) && rr_sglte_is_l1_tds_measurements_configured(gas_id))
        {
          rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_tds_search = rr_sglte_irat_neighbour_inhibit_plmn_list(is_sglte_irat_ded_meas_possible, gas_id);
        }

        MSG_GERAN_HIGH_1_G("RR SGLTE BPLMN TDS in PLMN list, TDS is neighbour:%d",rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_tds_search);
      }
      else
      {
        MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN TDSCDMA NOT in PLMN list");
        rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_tds_search = TRUE;
      }
#endif /* FEATURE_GSM_TDS */
    }
    if ( rr_gprs_check_if_any_g2x_resel_timer_is_running( gas_id ) )
    { /* Block SGLTE IRAT BPLMN List Search, if Ongoing reselection criterion is met for any other RAT */
      MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN Inhibit both LTE and TDS search in case IRAT G2X Reselection Timer is running");
      rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_tds_search = TRUE;
      rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_lte_search = TRUE;
    }

#ifdef FEATURE_WCDMA
    if (rr_is_rat_in_plmn_search_list(plmn_list_req_ptr,SYS_RAT_UMTS_RADIO_ACCESS) == TRUE)
    {
      MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN UMTS in plmn list");
    }
#endif /* FEATURE_WCDMA */

    if (rr_control_can_process_sglte_plmn_search(gas_id) &&
        plmn_list_req_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC &&
        rr_is_rat_in_plmn_search_list(plmn_list_req_ptr,SYS_RAT_GSM_RADIO_ACCESS) == FALSE &&
        rr_is_rat_in_plmn_search_list(plmn_list_req_ptr,SYS_RAT_UMTS_RADIO_ACCESS) == FALSE &&
        rr_PLMN_request_details_ptr->service_domain == SYS_SRV_DOMAIN_CS_PS &&
          !(rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_lte_search &&
            rr_control_data_ptr->sglte_g2x_plmn_list_search_inhibit_tds_search)
       )
    {
      start_sglte_search = TRUE;
      MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN Starting search");
    }
    else
    {
      MSG_GERAN_HIGH_0_G("RR SGLTE BPLMN does not match acceptance criteria");
    }
  }

  return start_sglte_search;
}
#endif /* FEATURE_SGLTE */

boolean rr_is_lte_in_neighbour_list(const gas_id_t gas_id)
{
  gprs_scell_info_t *scell = rr_gprs_get_scell_info(gas_id);
  boolean rat_is_neighbour = FALSE;

  if (  rr_priority_and_threshold_information_available(rr_get_instances_ptr(SI2Q_INSTANCE, gas_id), gas_id) &&
       (scell->gsm.lte_neighbour_list.count > 0) )
  {
    rat_is_neighbour = TRUE;
  }

  return rat_is_neighbour;
}

void rr_set_sglte_perform_g2l_blind_redir(boolean setting, const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  if (geran_is_multi_mode_gas_id(gas_id))
  {
    MSG_GERAN_MED_1_G("Set sglte_perform_g2l_blind_redir to: %d",setting);
  }
  else if (TRUE == setting)
  {
    MSG_GERAN_ERROR_0_G("Unexpected use of G2L blind redir on non-multimode sub");
  }

  rr_control_data_ptr->sglte_perform_g2l_blind_redir = setting;

  return;
}

boolean rr_sglte_perform_g2l_blind_redir(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  MSG_GERAN_MED_1_G("rr_sglte_perform_g2l_blind_redir() returns %d",
                    rr_control_data_ptr->sglte_perform_g2l_blind_redir);

  return rr_control_data_ptr->sglte_perform_g2l_blind_redir;
}

static void rr_sglte_set_lte_blind_redir_in_data_transfer(const gas_id_t gas_id)
{
  // Obtain a pointer to the module data
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);

  RR_NULL_CHECK_FATAL(rr_control_data_ptr);
#ifdef FEATURE_LTE
  if (rr_g2l_blind_redir_after_voice_ss_or_periodic_lau_enabled(gas_id))
  {
    boolean cs_is_for_voice_or_periodic_lau = FALSE;

    if (rr_ce_access_reason(gas_id) == MM_RR_MO_FULL_SPEECH ||
        rr_ce_access_reason(gas_id) == MM_RR_MO_HALF_SPEECH ||
        rr_ce_access_reason(gas_id) == MM_RR_SUPPLEMENTARY_SERVICE ||
        rr_ce_access_reason(gas_id) == MM_RR_CALL_REEST)
    {
      cs_is_for_voice_or_periodic_lau = TRUE;
    }
    else if (rr_ce_access_reason(gas_id) == MM_RR_LOCATION_UPDATE)
    {
      if (rr_ce_is_periodic_lau(gas_id))
      {
        cs_is_for_voice_or_periodic_lau = TRUE;
      }
    }

    MSG_GERAN_MED_1_G(
      "SGLTE CS for voice / periodic LAU: %d, G2L blind redir after Voice / SS / Periodic LAU enabled: 1",
      cs_is_for_voice_or_periodic_lau
    );

    if (cs_is_for_voice_or_periodic_lau)
    {
      rr_connection_release_set_g2l_blind_redir_flag(gas_id);
    }
  }
  else
  {
    MSG_GERAN_MED_0_G("SGLTE G2L blind redir after Voice / SS / Periodic LAU not enabled");
  }
#endif /* FEATURE_LTE */

  return;
}
#endif /* FEATURE_SGLTE */

/**
  @brief Determines if all subsscriber are in RR_INACTIVE state
  @param 
  @return TRUE all subscribers are in RR_INACTIVE, FALSE otherwise.
*/
boolean rr_is_inactive_for_all_subs(void)
{
  int i;
  for (i = 0; i < NUM_GERAN_DATA_SPACES; i++)
  {
    if (rr_control_data[i].rr_state != RR_INACTIVE)
    {
      return FALSE;
    }
  }
  return TRUE;    
}

/**
  @brief Returns the Source RAT type for current X2G reselection procedure
  @param GAS ID
  @return rr_rat_e
*/
rr_rat_e rr_get_x2g_resel_source_rat(const gas_id_t gas_id)
{
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  RR_NULL_CHECK_FATAL(rr_control_data_ptr);

  return(rr_map_x2g_resel_type_to_rat(rr_control_data_ptr->x2g_resel_type));
}


#ifdef FEATURE_FAKE_BTS_CELL_BARRING
rr_event_T rr_handle_fake_gcell_action_req(rr_cmd_bdy_type* new_message_ptr, const gas_id_t gas_id)
{
  rr_event_T rr_event;
  rr_control_data_t *rr_control_data_ptr = rr_control_get_data_ptr(gas_id);
  
  switch (rr_control_data_ptr->rr_state)
  {
    case RR_DATA_TRANSFER:
    {
      rr_log_gsm_call_drop(
        RR_LOG_CALL_DROPPED_DUE_TO_UPPER_LAYER_TRIGGER,
        rr_ce_access_reason(gas_id),
        gas_id);
       rr_event = rr_connection_release(EV_INPUT_MESSAGE, new_message_ptr, gas_id);   
    }
    break;    
    case RR_GRR:
    {
      rr_event = grr_control(EV_INPUT_MESSAGE, new_message_ptr, gas_id); /* Needs to be discussed */
      break;
    }
    default:  
      rr_event = EV_NO_EVENT;
      MSG_GERAN_HIGH_1_G("No action taken in %d state apart from barring",rr_control_data_ptr->rr_state );
      break;
  }
  return rr_event;
}
#endif /*FEATURE_FAKE_BTS_CELL_BARRING*/

/* EOF */

