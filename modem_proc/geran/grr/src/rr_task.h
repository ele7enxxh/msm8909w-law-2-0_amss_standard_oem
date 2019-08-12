#ifndef RR_TASK_H
#define RR_TASK_H
/*===========================================================================
                          RR Interface

DESCRIPTION

 This module defines contains the defines, structures and function
 prototypes required to send a command to the rr task.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2014 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_task.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/03   sbs     Added support for Location Services related APDU handling
02/14/03   RG      Added support for G2W blind Handover
01/18/03   JAC     Added dl_data_ack_ind
11/08/02   Rao     Fixed a typo in rrc_interrat_reselect_rej_type.
10/03/02   Rao     Added InterRAT Reselection ( GSM to WCDMA ) Support.
09/20/02   JAC     Added sleep signal
09/17/02   Rao     Added InterRAT Reselection ( WCDMA to GSM ) Support.
08/22/02   Rao     Added Activation_rsp to rr_cmd_bdy_type.
07/20/02   Rao     Added InterRAT ( WCDMA To GSM )  Handover support
08/01/02   rmc     Removing featurizations
06/19/02   rmc     Featurizing RR_REG_STATUS_UPDATE_REQ message for new dual-mode interface
05/22/02   rmc     Changes to MM-RR interface for dual mode
03/18/02   SB      Featurized rr_act_req message
02/20/02   ATM     Merge GS_GSM and GS functions together
11/09/01   RMC     Added RR_GSDI_RESERVED_SIG as a reserved signal
12/11/00   TS      Initial Revision.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "gs.h"
#include "ms_timer.h"
#include "queue.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_l2.h"
#include "rr_l2_l1.h"
#include "mm_rr.h"
#include "cmd.h"
#include "rr_message_header.h"
#include "rr_if.h"
#include "rr_grr_if.h"
#include "rr_resel_if.h"
#include "rr_inter_ho_if.h"
#include "rr_cell_acq_if.h"
#include "rr_pscan_if.h"
#include "rr_decode_bcch_if.h"
#include "rr_select_bcch_if.h"
#include "rr_ce_if.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
#include "rr_resel_g2w_if.h"
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_CGPS_UMTS_CP_GSM
#include "rr_cgps_defs.h"
#endif

#include "mac_grr.h"
#ifdef FEATURE_GSM_DTM
#include "rlc_grr.h"
#endif /* FEATURE_GSM_DTM */

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
#include "rr_x2g_resel_if.h"
#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */

#include "geran_grr_msg.h"

#ifdef FEATURE_LTE
#include "lte_rrc_irat_msg.h"
#endif /* FEATURE_LTE */

#ifdef FEATURE_FEMTO_GSM_NL
#include "rr_femto_if.h"
#endif /* FEATURE_FEMTO_GSM_NL */

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS_G2T_REDIRECTION)
#include "rr_g2x_redir_if.h"
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION*/

#ifdef FEATURE_LTE_TO_GSM_CGI
#include "rr_x2g_cgi_if.h"
#endif /* FEATURE_LTE_TO_GSM_CGI */

#include "rr_l1_idle_mode_if.h"
#include "rr_ps_if.h"
#include "rr_dtm_if.h"

#ifdef FEATURE_GSM_TDS
#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"
#endif /*FEATURE_GSM_TDS*/

#ifdef FEATURE_SGLTE
#include "rr_plmn_list_if.h"
#include "rr_sglte_x2g_if.h"
#endif /* FEATURE_SGLTE */

#ifdef FEATURE_GSM_BAND_AVOIDANCE
#include "wwan_coex_mgr.h"
#include "rr_band_avoid_if.h"
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define RR_CMD_Q_SIG             0x0001
#define RR_RPT_TIMER_SIG         0x0002
#define RR_NV_SIG                0x0004   /* NV signal */
#define RR_SLEEP_SIG             0x0008
#define RR_RESET_SIG             0x0010
#define RR_MSGR_Q_SIG            0x0020

/* reserved signal for internal GSDI processing */
#define RR_GSDI_RESERVED_SIG     0x1000

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* Union of all messages that may be sent on the RR<->RR Signalling Interface.
*/
typedef union
{
  rr_message_header_t  header;

  /* Messages sent to/from rr_control. */
  rr_msg_u             rr;

  /* Messages sent to/from grr_control. */
  rr_grr_msg_u         grr;

  /* Messages sent to/from rr_resel_control. */
  rr_resel_msg_u       resel;

  /* Messages sent to/from rr_inter_cell_handover_control. */
  rr_inter_ho_msg_u    inter_ho;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  /* Messages sent to/from rr_resel_g2w_control. */
  rr_resel_g2w_msg_u   resel_g2w;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  rr_cell_acq_msg_u    cell_acq;

  rr_pscan_msg_u       pscan;

  rr_decode_bcch_msg_u decode_bcch;

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)
  rr_x2g_resel_msg_u   x2g_resel;
#endif /* FEATURE_WCDMA || FEATURE_LTE  || FEATURE_GSM_TDS */

  rr_select_bcch_msg_u select_bcch;

#ifdef FEATURE_FEMTO_GSM_NL
  rr_femto_msg_u       femto;
#endif /* FEATURE_FEMTO_GSM_NL */

  /* Messages sent to/from rr_connection_establishment_control. */
  rr_ce_msg_u          ce;

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS_G2T_REDIRECTION)
  /* Messages sent to/from rr_g2x_redir_control. */
  rr_g2x_redir_msg_u   g2x_redir;
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION */

#ifdef FEATURE_LTE_TO_GSM_CGI
  /* Messages sent to/from rr_x2g_cgi_control. */
  rr_x2g_cgi_msg_u     x2g_cgi;
#endif /* FEATURE_LTE_TO_GSM_CGI */

  /* Messages sent to/from rr_l1_idle_control */
  rr_l1_idle_msg_u     l1_idle;

  /* Messages sent to/from rr_ps_access_control */
  rr_ps_msg_u          ps;

  /* Messages sent to/from rr_dtm_control */
  rr_dtm_msg_u         dtm;

#ifdef FEATURE_SGLTE
    /* Messages sent to/from rr_plmn_list_control */
  rr_plmn_list_msg_u       plmn_list;

  rr_sglte_x2g_msg_u       sglte_x2g;
#endif /* FEATURE_SGLTE */
#ifdef FEATURE_GSM_BAND_AVOIDANCE
  rr_band_avoid_msg_u      band_avoid;
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
} rr_rr_msg_u;


/* Union of all messages that RR may receive via Message Router. */

typedef union
{
  msgr_hdr_s                     hdr;
  geran_grr_msg_req_u            geran_grr_msg_req;
  geran_grr_msg_ind_u            geran_grr_msg_ind;
#ifdef FEATURE_LTE
  lte_rrc_G_resel_failed_rsp_s   lte_rrc_G_resel_failed_rsp;
  lte_rrc_G_redir_failed_rsp_s   lte_rrc_G_redir_failed_rsp;
  lte_rrc_G_abort_resel_rsp_s    lte_rrc_G_abort_resel_rsp;
  lte_rrc_G_abort_redir_rsp_s    lte_rrc_G_abort_redir_rsp;
  lte_irat_plmn_srch_rsp_s       lte_irat_plmn_srch_rsp;
  lte_irat_abort_plmn_srch_rsp_s lte_irat_abort_plmn_srch_rsp;
  lte_rrc_get_dedicated_pri_rsp_s lte_rrc_get_dedicated_pri_rsp;
#endif /* FEATURE_LTE */
#ifdef FEATURE_GSM_BAND_AVOIDANCE
  cxm_coex_ba_ind_s               band_avoid_blacklist_ind;
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/
} geran_grr_msgr_messages_u;

typedef struct
{
  q_link_type               link;
  geran_grr_msgr_messages_u message;
} geran_grr_cmd_t;

typedef struct
{
  IMH_T                     message_header;
  geran_grr_msgr_messages_u message;
} rr_msgr_msg_t;

typedef union
{
  IMH_T                                   message_header;

  /* Union of all messages received from GERAN L1. */
  l1_rr_messages_u                        mph;

  dl_data_ind_T                           dl_data_ind;
  dl_establish_cnf_T                      dl_establish_cnf;
  dl_establish_ind_T                      dl_establish_ind;
  dl_random_access_cnf_T                  dl_random_access_cnf;
  dl_reconnect_cnf_T                      dl_reconnect_cnf;
  dl_release_cnf_T                        dl_release_cnf;
  dl_release_ind_T                        dl_release_ind;
  dl_resume_cnf_T                         dl_resume_cnf;
  dl_suspend_cnf_T                        dl_suspend_cnf;
  dl_unit_data_ind_T                      dl_unit_data_ind;
  dl_data_ack_ind_T                       dl_data_ack_ind;
  mdl_error_ind_T                         mdl_error_ind;
  mph_block_quality_ind_T                 mph_block_quality_ind;
  mph_channel_assign_cnf_T                mph_channel_assign_cnf;
  mph_channel_mode_modify_cnf_T           mph_channel_mode_modify_cnf;
  mph_channel_release_cnf_T               mph_channel_release_cnf;
  mph_frequency_redefinition_ind_T        mph_frequency_redefinition_ind;
  mph_handover_cnf_T                      mph_handover_cnf;
  mph_handover_ind_T                      mph_handover_ind;
  mph_immed_assign_cnf_T                  mph_immed_assign_cnf;
  mph_power_scan_cnf_T                    mph_power_scan_cnf;
  mph_reconnect_channel_cnf_T             mph_reconnect_channel_cnf;
  mph_reselection_cnf_T                   mph_reselection_cnf;
  mph_select_specific_bcch_cnf_T          mph_select_specific_bcch_cnf;
  mph_serving_ded_meas_ind_T              mph_serving_ded_meas_ind;
  mph_serving_idle_meas_ind_T             mph_serving_idle_meas_ind;
  mph_set_ciphering_cnf_T                 mph_set_ciphering_cnf;
  mph_stop_handover_access_cnf_T          mph_stop_handover_access_cnf;
  mph_surround_meas_ind_T                 mph_surround_meas_ind;
  mph_surround_update_failure_ind_T       mph_surround_update_failure_ind;
  mph_decode_bcch_list_failure_ind_T      mph_decode_bcch_list_failure_ind;
  mph_decode_bcch_list_cnf_T              mph_decode_bcch_list_cnf;
  mph_fcch_sch_decode_ind_T               mph_fcch_sch_decode_ind;
  mph_read_sys_info_failure_ind_T         mph_read_sys_info_failure_ind;
  mph_start_gsm_mode_cnf_T                mph_start_gsm_mode_cnf;
  mph_stop_gsm_mode_cnf_T                 mph_stop_gsm_mode_cnf;
  mph_serving_aux_meas_ind_T              mph_serving_aux_meas_ind;
#ifdef FEATURE_WCDMA
  mph_surround_wcdma_xfer_meas_ind_T      mph_surround_wcdma_xfer_meas_ind;
  mph_surround_wcdma_idle_meas_ind_T      mph_surround_wcdma_idle_meas_ind;
  mph_surround_wcdma_dedicated_meas_ind_T mph_surround_wcdma_dedicated_meas_ind;
#endif /* FEATURE_WCDMA */
  mph_start_idle_mode_cnf_T               mph_start_idle_mode_cnf;
#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
  mph_suspend_gsm_mode_cnf_T              mph_suspend_gsm_mode_cnf;
  mph_resume_gsm_mode_cnf_T               mph_resume_gsm_mode_cnf;
  mph_interrat_list_srch_time_ind_T       mph_interrat_list_srch_time_ind;
#endif /* FEATURE_GTOW_BACKGROUND_PLMN_SEARCH */
  mph_suspend_gsm_mode_ind_T              mph_suspend_gsm_mode_ind;
  mph_start_gsm_mode_for_bplmn_cnf_T      mph_start_gsm_mode_for_bplmn_cnf;
  mph_resume_gsm_mode_for_bplmn_cnf_T     mph_resume_gsm_mode_for_bplmn_cnf;
#ifdef FEATURE_GSM_DTM
  mph_dtm_channel_release_ind_t           mph_dtm_channel_release_ind;
  mph_dtm_channel_assign_cnf_t            mph_dtm_channel_assign_cnf;
#endif /* FEATURE_GSM_DTM */
  mph_l1_test_mode_start_ind_T            mph_l1_test_mode_start_ind;
  mph_l1_test_mode_stop_ind_T             mph_l1_test_mode_stop_ind;
  mph_reset_ind_T                         mph_reset_ind;
  mph_reset_cnf_T                         mph_reset_cnf;
  mph_abort_ra_cnf_T                      mph_abort_ra_cnf;
  mph_ue_mode_change_cnf_T                mph_ue_mode_change_cnf;

  rr_abort_req_T                          rr_abort_req;
  rr_act_req_T                            rr_act_req;
  rr_data_req_T                           rr_data_req;
  rr_deact_req_T                          rr_deact_req;
  rr_est_req_T                            rr_est_req;
  rr_plmn_list_req_T                      rr_plmn_list_req;
  rr_plmn_select_req_T                    rr_plmn_select_req;
  rr_forbidden_lai_update_req_T           rr_forbidden_lai_update_req;
  rr_sim_inserted_req_T                   rr_sim_inserted_req;
  rr_eq_plmn_list_change_ind_T            rr_eq_plmn_list_change_ind;
  rr_invalidate_sim_data_req_T            rr_invalidate_sim_data_req;
  rr_bplmn_search_abort_req_T             rr_bplmn_search_abort_req;

  timer_expiry_T                          mid_timer_expiry;

  rr_stop_gsm_mode_req_T                  rr_stop_gsm_mode_req;
  rr_sim_update_req_T                     rr_sim_update_req;
  rr_nw_sel_mode_reset_ind_T              rr_nw_sel_mode_reset_ind;
  rr_lai_reject_list_ind_T                rr_lai_reject_list_ind;

  mac_grr_sig_t                           mac_grr_sig;   /* union of all signals from MAC */

  mph_nc_measurement_ind_t                mph_nc_measurement_ind;
  mph_serving_meas_ind_t                  mph_serving_meas_ind;
  rr_gmm_ready_timer_state_req_T          rr_gmm_ready_timer_state_req;
  rr_gmm_gprs_tlli_assignment_req_T       rr_gmm_gprs_tlli_assignment_req;
  rr_gmm_gprs_state_change_req_T          rr_gmm_gprs_state_change_req;
  rr_gmm_gprs_ms_ra_cap_ie_ind_T          rr_gmm_gprs_ms_ra_cap_ie_ind;
  rr_gmm_start_mm_non_drx_ind_T           rr_gmm_start_mm_non_drx_ind;
  rr_gmm_stop_mm_non_drx_ind_T            rr_gmm_stop_mm_non_drx_ind;
  rr_gmm_gprs_service_cnf_T               rr_gmm_gprs_service_cnf;
  rr_gmm_gprs_nv_params_ind_t             rr_gmm_gprs_nv_params_ind;
  rr_gmm_gprs_service_req_T               rr_gmm_gprs_service_req;
  rr_gmm_gprs_auth_fail_ind_T             rr_gmm_gprs_auth_fail_ind;
  rr_psho_cnf_T                           rr_psho_cnf;

#ifdef FEATURE_GSM_DTM
  rlc_grr_ul_dcch_req_t                   rlc_grr_ul_dcch_req;
#endif /* FEATURE_GSM_DTM */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  rr_activation_rsp_T 					rr_activation_rsp;
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

#ifdef FEATURE_WCDMA
  rr_interrat_handover_abort_req_type     rr_interrat_handover_abort_req;
  rr_interrat_handover_req_type           rr_interrat_handover_req;
  rr_nas_data_ind_type                    rr_nas_data_ind;
  rrc_cipher_sync_ind_type                rrc_cipher_sync_ind;

  rr_interrat_reselect_req_type           rr_interrat_reselect_req;
  rr_interrat_reselection_abort_req_type  rr_interrat_reselection_abort_req;
  rr_interrat_cc_order_req_type           rr_interrat_cc_order_req;
  rr_interrat_cc_order_abort_req_type     rr_interrat_cc_order_abort_req;
  rrc_interrat_cc_order_cnf_type          rrc_interrat_cc_order_cnf;
  rrc_interrat_reselect_rej_type          rrc_interrat_reselect_rej;
  rrc_interrat_handover_cnf_type          rrc_interrat_handover_cnf;
  rrc_interrat_handover_abort_cnf_type    rrc_interrat_handover_abort_cnf;

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
  rr_interrat_redirect_req_type           rr_interrat_redirect_req;
  rr_interrat_redirect_abort_req_type     rr_interrat_redirect_abort_req;
  rrc_interrat_reselection_abort_cnf_type rrc_interrat_reselection_abort_cnf;
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

  rr_interrat_plmn_srch_req_type          rr_interrat_plmn_srch_req;
  rrc_interrat_plmn_srch_cnf_type         rrc_interrat_plmn_srch_cnf;
  rrc_interrat_plmn_srch_abort_cnf_type   rrc_interrat_plmn_srch_abort_cnf;
  rr_interrat_plmn_srch_abort_req_type    rr_interrat_plmn_srch_abort_req;
  rr_interrat_plmn_srch_suspend_ind_type  rr_interrat_plmn_srch_suspend_ind;
  rr_rrc_stop_gsm_req_type                rr_rrc_stop_gsm_req;

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
  rrc_interrat_redirect_rej_type          rrc_interrat_redirect_rej;
  rrc_interrat_redirect_abort_cnf_type    rrc_interrat_redirect_abort_cnf;
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

  rrc_dedicated_priorities_rsp_type       rrc_dedicated_priorities_rsp;
  rr_dedicated_priorities_req_type        rr_dedicated_priorities_req;
  rr_clear_dedicated_priorities_ind_type  rr_clear_dedicated_priorities_ind;

  rrc_rr_stop_wcdma_cnf_type              rrc_rr_stop_wcdma_cnf;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_FEMTO_GSM_NL
  rrc_rr_nbr_disc_req_type                rr_nbr_disc_req;
  rrc_rr_start_cosc_req_type              rr_start_cosc_req;
#endif /* FEATURE_FEMTO_GSM_NL */

#ifdef FEATURE_CGPS_UMTS_CP_GSM
  rr_apdu_req_T                           rr_apdu_req;
  rr_cgps_event_req_T                     rr_cgps_event_req;
#endif

  rr_rr_msg_u                             rr;

  rr_msgr_msg_t                           rr_msgr_msg;

#ifdef FEATURE_GSM_TDS
  tdsrrc_interrat_reselect_rej_type       tdsrrc_resel_rej;
  tdsrr_interrat_reselect_req_type        tdsrr_resel_req;
  tdsrr_interrat_cc_order_req_type        tdsrr_cco_req;
  tdsrr_interrat_handover_req_type        tdsrr_ho_req;
  tdsrr_nas_data_ind_type                 tdsrr_nas_data_ind;
  tdsrr_interrat_plmn_srch_req_type       tdsrr_plmn_srch_req;
  tdsrr_stop_gsm_req_type                 tdsrr_stop_gsm_req;
  tdsrr_interrat_tl1_stop_ind_type        tdsrr_tl1_stop_ind;
#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
  tdsrrc_dedicated_priorities_rsp_type    tdsrrc_dedicated_priorities_rsp;
  tdsrr_dedicated_priorities_req_type     tdsrr_dedicated_priorities_req;
  tdsrr_clear_dedicated_priorities_ind_type  tdsrr_clear_dedicated_priorities_ind;
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/
#ifdef FEATURE_TDS_SUB_ID_INTERFACE
  tdsrr_interrat_handover_abort_req_type  tdsrr_ho_abort_req;
  tdsrr_interrat_reselection_abort_req_type  tdsrr_resel_abort_req;
  tdsrr_interrat_plmn_srch_abort_req_type tdsrr_plmn_srch_abort_req;
  tdsrr_interrat_plmn_srch_suspend_ind_type  tdsrr_plmn_srch_suspend_ind;
  tdsrr_interrat_cc_order_abort_req_type  tdsrr_cco_abort_req;
  tdsrr_interrat_redirect_req_type        tdsrr_redir_req;
  tdsrr_interrat_redirect_abort_req_type  tdsrr_redir_abort_req;
  tdsrrc_interrat_handover_cnf_type       tdsrr_ho_cnf;
  tdsrrc_interrat_handover_abort_cnf_type tdsrr_ho_abort_cnf;
  tdsrrc_interrat_reselect_rej_type       tdsrr_resel_rej;
  tdsrrc_interrat_reselection_abort_cnf_type  tdsrr_resel_abort_cnf;
  tdsrrc_interrat_plmn_srch_cnf_type      tdsrr_plmn_srch_cnf;
  tdsrrc_interrat_plmn_srch_abort_cnf_type  tdsrr_plmn_srch_abort_cnf;
  tdsrrc_interrat_redirect_rej_type       tdsrr_redir_rej;
  tdsrrc_interrat_redirect_abort_cnf_type tdsrr_redir_abort_cnf;
  tdsrrc_stop_tds_cnf_type                tdsrr_stop_tds_cnf;
#endif // FEATURE_TDS_SUB_ID_INTERFACE
#endif /*FEATURE_GSM_TDS*/

  /* Union of messages that may be received from MM. */
  mm_rr_msgs_u                           mm;

} rr_cmd_bdy_type;

typedef struct
{
  q_link_type      link;
  rr_cmd_bdy_type  msg;

  /* Record how many times each message is held in on the pending message
  queue. */
  uint32           pended_count;
} rr_cmd_type;

#endif /* #ifndef RR_TASK_H */

/* EOF */

