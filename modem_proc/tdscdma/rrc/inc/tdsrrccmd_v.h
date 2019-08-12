
#ifndef TDSRRCCMD_V_H
#define TDSRRCCMD_V_H
/*===========================================================================
                      R R C    C O M M A N D S

DESCRIPTION

  This module contains external definitions for the
  command queues used by RRC task. This file should be
  included by any external module sending commands to RRC's
  command queues.


Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
Export of this technology or software is regulated by the U.S. Government. 
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rrc/inc/tdsrrccmd_v.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/12   hx      Merged to VU_MODEM_WCDMA_NIKEL_3_0_MHCI.00.00.00.00.08.04
03/10/11   hx      Merged with VU_MODEM_WCDMA_RRC_OSYS.01.93.53

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "tdscdma_variation.h"
#include "comdef.h"
#include "queue.h"
#include "tdsrrcmm_v.h"
#include "tdsrrccmd.h"




#include "tdsrrcintif.h"
#include "tdsl1rrcif.h"
#include "tdsmacrrcif.h"
#include "tdsrlcrrcif.h"

/*#ifdef FEATURE_INTERRAT_HANDOVER_WTOG*/
#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"
/*#endif*/


#if defined (FEATURE_CGPS_UMTS_CP_TDSCDMA)
#include "tdsrrclsmif.h"
#endif



#if defined(TEST_FRAMEWORK) && defined(TF_UNIT_TEST)
#error code not present
#endif

#include "tds_rrc_ext_if.h"

#if defined (FEATURE_TDSCDMA_TO_LTE) || defined (FEATURE_LTE_TO_TDSCDMA)
#include "tds_rrc_msg.h"
#include <lte_rrc_irat_msg.h>
#include <lte_irat_types.h>
#endif



/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */




/*--------------------------------------------------------*/
/*                 RRC COMMAND IDs                        */
/*                                                        */
/*  Command Ids for commands sent from various tasks to   */
/*  RRC are defined here. All command ids, regardless of  */
/*  which command queue is used to queue them are defined */
/*  in a single enum type given below.                    */
/*                                                        */
/*  NOTE! When you add a new command ID please make sure  */
/*  the corrosponding command structure is added to the   */
/*  structure tdsrrc_cmd_type that contains all the commands.*/
/*                                                        */
/*--------------------------------------------------------*/
/*lint -save -e793 */

/*Move RRC Command ID definitions to tdsrrccmd.h*/



/*lint -restore */


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/*
** RRC Command header. This header is a part of all commands sent
** to the RRC layer. This contains the command id for the command sent
** to RRC, and a Queue link in order to put the command in the
** appropriate queue
*/

typedef struct
{
  q_link_type   link;        /* Queue link     */
  tdsrrc_cmd_e_type cmd_id;     /* The command Id */
  /* Other items may be added here for debugging purposes.
  this is still TBD. */
  uint8         seq_num;
}tdsrrc_cmd_hdr_type;

/* Update tdsrrc_cmd_int_q_u_type along with tdsrrc_cmd_data_u_type 
   for new Internal commands
 */
#ifdef FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE
typedef union
{


  /* Union of individual command structures */
  /*--------------------------------------------------------*/
  /* Command structures for Internal RRC commands           */
  /*--------------------------------------------------------*/
  tdsrrc_downlink_sdu_ind_type       downlink_sdu_ind;
  tdsrrc_downlink_l2ack_ind_type     downlink_l2ack_ind;
  tdsrrc_get_all_sibs_req_type       get_all_sibs_req;
  tdsrrc_sib_event_cnf_type          get_sibs_cnf;
  tdsrrc_get_specific_sibs_req_type  get_specific_sibs_req;
  tdsrrc_redirect_req_type           redirect_req;
  tdsrrc_redirect_cnf_type           redirect_cnf;
  tdsrrc_new_state_ind_type          new_state_ind;
  tdsrrc_state_change_ind_type       state_change_ind;
  tdsrrc_cell_change_req_type        cell_change_req;
  tdsrrc_cell_change_cnf_type        cell_change_cnf;
  tdsrrc_new_cell_ind_type           new_cell_ind;
  tdsrrc_llc_cmd_req_type            rrc_llc_req;
  tdsrrc_llc_cmd_cnf_type            rrc_llc_cnf;
  tdsrrc_channel_config_req_type     chan_config_req;
  tdsrrc_channel_config_cnf_type     chan_config_cnf;
  tdsrrc_update_sibs_ind_type        update_sibs_ind;
  tdsrrc_initiate_ue_cap_req_type    initiate_ue_cap;
  tdsrrc_initiate_cu_req_type        initiate_cell_update;
  tdsrrc_initiate_uu_req_type        initiate_ura_update;
  tdsrrc_cu_complete_cnf_type        cu_complete_cnf;
  tdsrrc_disable_prev_freq_type      disable_prev_freq;
  tdsrrc_cell_selected_meas_req_type cell_selected_meas_req;
  tdsrrc_initiate_cell_sel_ind_type  initiate_cell_sel_ind;
  tdsrrc_sib_change_ind_type         sib_change_ind;
  tdsrrc_rnti_update_req_type        rnti_update_req;
  tdsrrc_rnti_update_cnf_type        rnti_update_cnf;
  tdsrrc_cell_reselection_req_type   cell_resel_req;
  tdsrrc_cell_reselection_cnf_type   cell_resel_cnf;
  tdsrrc_ul_suspend_req_type         rrc_ul_suspend_req;
  tdsrrc_ul_resume_req_type          rrc_ul_resume_req;
  tdsrrc_sig_conn_release_req_type   rrc_sig_conn_req;
  tdsrrc_sig_conn_release_cnf_type   rrc_sig_conn_cnf;
  tdsrrc_ciph_config_req_type        rrc_ciph_config_req;

  tdsrrc_abort_ciph_config_req_type rrc_abort_ciph_config_req;

  tdsrrc_ho_ciph_req_type            ho_ciph_req;
  tdsrrc_stop_lower_layer_req_type   stop_lower_layer_req;
  tdsrrc_stop_lower_layer_cnf_type   stop_lower_layer_cnf;
  tdsrrc_mode_change_ind_type        mode_change_ind;

  tdsrrc_sib7_update_status_ind_type sib7_update_status;
  tdsrrc_change_mode_req_type        change_mode_req;
  tdsrrc_change_mode_cnf_type        change_mode_cnf;


#if defined(FEATURE_INTERRAT_HANDOVER_G2TD) || defined(FEATURE_TDSCDMA_L2TD_PS_HANDOVER)
  tdsrrc_csp_acq_req_type            rrc_csp_acq_req;
  tdsrrc_csp_acq_cnf_type            rrc_csp_acq_cnf;
  tdsrrc_ho_ciph_cnf_type            rrc_ho_ciph_cnf;
#endif

  tdsrrc_lower_layer_deact_req_type  lower_layer_deact_req;

  tdsrrc_re_est_rlc_srns_req_type    rrc_re_est_srns_req;
  tdsrrc_re_est_rlc_srns_cnf_type    rrc_re_est_srns_cnf;
  tdsrrc_llc_re_est_req_type         rrc_re_est_req;

  tdsrrc_csp_camp_req_type           rrc_csp_camp_req;
  tdsrrc_csp_camp_cnf_type           rrc_csp_camp_cnf;
  tdsrrc_conn_setup_ind_type         rrc_conn_setup_ind;
  tdsrrc_irat_abort_ind_type         rrc_irat_abort_ind;
  tdsrrc_idt_complete_cnf            idt_complete_cnf;

  tdsrrc_csp_bplmn_suspend_req_type    rrc_csp_bplmn_suspend_req;
  tdsrrc_csp_bplmn_suspend_cnf_type    rrc_csp_bplmn_suspend_cnf;

  /* added internal structure for dual mode handling will be set by MCM to CSP*/
  tdsrrc_mode_status_ind_type          rrc_mode_status_ind;

#ifdef FEATURE_TDSCDMA_PLI_NMR_UTRAN
  tdsrrc_gstk_utran_nmr_info_req_type   rrc_gstk_utran_nmr_info_req;
#endif


#ifdef FEATURE_TDSCDMA_REL7_SPECRS 
  /*Internal commands added for requesting & confirmation of
    "Change of UE Capability" */
  tdsrrc_couec_req_type rrc_couec_req;
  tdsrrc_couec_cnf_type rrc_couec_cnf;
#endif

  tdsrrc_cipher_config_cnf_type rrc_cipher_config_cnf;

#ifdef FEATURE_TDSCDMA_DSDS
  tdsrrc_reset_l1_req_type rrc_reset_l1_req;
#endif  

#if defined(TEST_FRAMEWORK) && defined(TF_UNIT_TEST)
  #error code not present
#endif

#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
/*types for internal FF command queue*/
  TDSRRC_SERVING_CELL_CHANGE_IND_CB_FUNC_TYPE *reg_cb_ptr;
  TDSRRC_SERVING_CELL_CHANGE_IND_CB_FUNC_TYPE *dereg_cb_ptr;
#endif

#ifdef FEATURE_MODEM_CONFIG_REFRESH
 /* RRC issues internal cmd TDSRRC_MCFG_REFRESH_IND when MCFG 
    task calls RRC API tdsrrc_mcfg_refresh_cb for MCFG refresh event */
  tds_rrc_mcfg_refresh_ind_type mcfg_refresh_ind;
#endif

 /* This dummy field has been added , so that both external and the 
 * RRC internal cmds have the same size for the cmd union. 
 * As the size of the command unoin is determined by the highest data type
 * present in the structure in ARM.*/
  /*Currently disabled for Q6 build*/
  /*uint64 dummyPadding;*/


}tdsrrc_cmd_int_q_u_type;

#endif

/* LTE commands or other message router messages are only valid internally to RRC. Put here. */
typedef union
{
   msgr_hdr_s msg_hdr;
#if defined (FEATURE_TDSCDMA_TO_LTE) || defined (FEATURE_LTE_TO_TDSCDMA)
   /*L2TD requests*/
   tds_rrc_lte_resel_req_type tds_rrc_lte_resel_req;
   tds_rrc_lte_abort_resel_req_type tds_rrc_lte_abort_resel_req;
   tds_rrc_lte_redir_req_type tds_rrc_lte_redir_req;
   tds_rrc_lte_abort_redir_req_type tds_rrc_lte_abort_redir_req;
   lte_irat_plmn_srch_req_s tds_rrc_lte_plmn_srch_req;
   lte_irat_abort_plmn_srch_req_s tds_rrc_lte_abort_plmn_srch_req;
   tds_rrc_lte_utra_capabilities_req_type tds_rrc_lte_utra_capabilities_req;
   tds_rrc_lte_psho_req_type tds_rrc_lte_psho_req;
   tds_rrc_lte_abort_psho_req_type tds_rrc_lte_abort_psho_req;
   tds_rrc_lte_get_cgi_req_type  tds_rrc_lte_cgi_req;
   tds_rrc_lte_abort_cgi_req_type tds_rrc_lte_abort_cgi_req;
   tds_rrc_lte_depri_freq_req_type tds_rrc_lte_depri_freq_req;
   /*Responses for TD2L requests*/
   lte_rrc_tdscdma_resel_failed_rsp_s lte_rrc_tds_resel_failed_rsp;
   lte_rrc_tdscdma_abort_resel_rsp_s lte_rrc_tds_abort_resel_rsp;
   lte_rrc_tdscdma_redir_failed_rsp_s lte_rrc_tds_redir_failed_rsp;
   lte_rrc_tdscdma_abort_redir_rsp_s lte_rrc_tds_abort_redir_rsp;
   lte_irat_plmn_srch_rsp_s lte_rrc_tds_plmn_srch_rsp;
   lte_irat_abort_plmn_srch_rsp_s lte_rrc_tds_abort_plmn_srch_rsp;
   lte_rrc_eutra_capabilities_rsp_s lte_rrc_eutra_capabilites_rsp;
   lte_rrc_get_dedicated_pri_rsp_s lte_rrc_get_dedicated_pri_rsp;
   /*For PSHO RSP from LTE*/
   lte_rrc_tdscdma_psho_rsp_s lte_rrc_tdscdma_psho_rsp;
   lte_rrc_tdscdma_abort_psho_rsp_s lte_rrc_tdscdma_abort_psho_rsp;
#endif
   tds_rrc_gps_pos_cell_info_req_type tds_rrc_gps_pos_cell_info_req;
   tds_rrc_set_config_req_type tds_rrc_set_config_req;
}tds_rrc_msgr_cmd_u_type;


typedef struct
{
  q_link_type           link;

  tds_rrc_msgr_cmd_u_type cmd;
} tds_rrc_msgr_cmd_type;


/*--------------------------------------------------------*/
/*                 RRC COMMAND BUFFER                     */
/*                                                        */
/*  The Command buffer used for all RRC command queues is */
/*  defined here. The command buffer contains a RRC       */
/*  command header and a union of all possible commands   */
/*  for any RRC command queue                             */
/*--------------------------------------------------------*/
/* Update tdsrrc_cmd_int_q_u_type along with tdsrrc_cmd_data_u_type 
   for new Internal commands
 */
typedef union
{


  /* Union of individual command structures */
  /*--------------------------------------------------------*/
  /* Command structures for Internal RRC commands           */
  /*--------------------------------------------------------*/
  tdsrrc_downlink_sdu_ind_type       downlink_sdu_ind;
  tdsrrc_downlink_l2ack_ind_type     downlink_l2ack_ind;
  tdsrrc_get_all_sibs_req_type       get_all_sibs_req;
  tdsrrc_sib_event_cnf_type          get_sibs_cnf;
  tdsrrc_get_specific_sibs_req_type  get_specific_sibs_req;
  tdsrrc_redirect_req_type           redirect_req;
  tdsrrc_redirect_cnf_type           redirect_cnf;
  tdsrrc_new_state_ind_type          new_state_ind;
  tdsrrc_state_change_ind_type       state_change_ind;
  tdsrrc_cell_change_req_type        cell_change_req;
  tdsrrc_cell_change_cnf_type        cell_change_cnf;
  tdsrrc_new_cell_ind_type           new_cell_ind;
  tdsrrc_llc_cmd_req_type            rrc_llc_req;
  tdsrrc_llc_cmd_cnf_type            rrc_llc_cnf;
  tdsrrc_channel_config_req_type     chan_config_req;
  tdsrrc_channel_config_cnf_type     chan_config_cnf;
  tdsrrc_update_sibs_ind_type        update_sibs_ind;
  tdsrrc_initiate_ue_cap_req_type    initiate_ue_cap;
  tdsrrc_initiate_cu_req_type        initiate_cell_update;
  tdsrrc_initiate_uu_req_type        initiate_ura_update;
  tdsrrc_cu_complete_cnf_type        cu_complete_cnf;
  tdsrrc_disable_prev_freq_type      disable_prev_freq;
  tdsrrc_cell_selected_meas_req_type cell_selected_meas_req;
  tdsrrc_initiate_cell_sel_ind_type  initiate_cell_sel_ind;
  tdsrrc_sib_change_ind_type         sib_change_ind;
  tdsrrc_rnti_update_req_type        rnti_update_req;
  tdsrrc_rnti_update_cnf_type        rnti_update_cnf;
  tdsrrc_cell_reselection_req_type   cell_resel_req;
  tdsrrc_cell_reselection_cnf_type   cell_resel_cnf;
  tdsrrc_ul_suspend_req_type         rrc_ul_suspend_req;
  tdsrrc_ul_resume_req_type          rrc_ul_resume_req;
  tdsrrc_sig_conn_release_req_type   rrc_sig_conn_req;
  tdsrrc_sig_conn_release_cnf_type   rrc_sig_conn_cnf;
  tdsrrc_ciph_config_req_type        rrc_ciph_config_req;

  tdsrrc_abort_ciph_config_req_type rrc_abort_ciph_config_req;

  tdsrrc_ho_ciph_req_type            ho_ciph_req;
  tdsrrc_stop_lower_layer_req_type   stop_lower_layer_req;
  tdsrrc_stop_lower_layer_cnf_type   stop_lower_layer_cnf;
  tdsrrc_mode_change_ind_type        mode_change_ind;

  tdsrrc_sib7_update_status_ind_type sib7_update_status;
  tdsrrc_change_mode_req_type        change_mode_req;
  tdsrrc_change_mode_cnf_type        change_mode_cnf;


#if defined(FEATURE_INTERRAT_HANDOVER_G2TD) || defined(FEATURE_TDSCDMA_L2TD_PS_HANDOVER)
  tdsrrc_csp_acq_req_type            rrc_csp_acq_req;
  tdsrrc_csp_acq_cnf_type            rrc_csp_acq_cnf;
  tdsrrc_ho_ciph_cnf_type            rrc_ho_ciph_cnf;
#endif

  tdsrrc_lower_layer_deact_req_type  lower_layer_deact_req;

  tdsrrc_re_est_rlc_srns_req_type    rrc_re_est_srns_req;
  tdsrrc_re_est_rlc_srns_cnf_type    rrc_re_est_srns_cnf;
  tdsrrc_llc_re_est_req_type         rrc_re_est_req;

  tdsrrc_csp_camp_req_type           rrc_csp_camp_req;
  tdsrrc_csp_camp_cnf_type           rrc_csp_camp_cnf;
  tdsrrc_conn_setup_ind_type         rrc_conn_setup_ind;
  tdsrrc_irat_abort_ind_type         rrc_irat_abort_ind;
  tdsrrc_idt_complete_cnf            idt_complete_cnf;

  tdsrrc_csp_bplmn_suspend_req_type    rrc_csp_bplmn_suspend_req;
  tdsrrc_csp_bplmn_suspend_cnf_type    rrc_csp_bplmn_suspend_cnf;

  /* added internal structure for dual mode handling will be set by MCM to CSP*/
  tdsrrc_mode_status_ind_type          rrc_mode_status_ind;

#ifdef FEATURE_TDSCDMA_PLI_NMR_UTRAN
  tdsrrc_gstk_utran_nmr_info_req_type   rrc_gstk_utran_nmr_info_req;
#endif


#ifdef FEATURE_TDSCDMA_REL7_SPECRS 
  /*Internal commands added for requesting & confirmation of
    "Change of UE Capability" */
  tdsrrc_couec_req_type rrc_couec_req;
  tdsrrc_couec_cnf_type rrc_couec_cnf;
#endif

  tdsrrc_cipher_config_cnf_type rrc_cipher_config_cnf;

#ifdef FEATURE_TDSCDMA_DSDS
  tdsrrc_reset_l1_req_type rrc_reset_l1_req;
#endif

  tdsrrc_open_domain_ind_type rrc_open_domain_ind;
  
#if defined(TEST_FRAMEWORK) && defined(TF_UNIT_TEST)
  #error code not present
#endif


#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
/*types for internal FF command queue*/
  TDSRRC_SERVING_CELL_CHANGE_IND_CB_FUNC_TYPE *reg_cb_ptr;
  TDSRRC_SERVING_CELL_CHANGE_IND_CB_FUNC_TYPE *dereg_cb_ptr;
#endif

#ifdef FEATURE_MODEM_CONFIG_REFRESH
 /* RRC issues internal cmd TDSRRC_MCFG_REFRESH_IND when MCFG 
    task calls RRC API tdsrrc_mcfg_refresh_cb for MCFG refresh event */
  tds_rrc_mcfg_refresh_ind_type mcfg_refresh_ind;
#endif

#ifdef FEATURE_QSH_EVENT_METRIC
  tdsrrc_qsh_cb_ind_type        qsh_cb_ind;
#endif

  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by L1          */
  /*--------------------------------------------------------*/
  tdsl1_idle_cnf_type                idle_cnf;
  tdsl1_acq_cnf_type                 acq_cnf;
  tdsl1_freq_scan_cnf_type           freq_scan_cnf;
  tdsl1_setup_cnf_type               setup_cnf;
  tdsl1_cell_select_cnf_type         cell_sel_cnf;
  tdsl1_error_ind_type               err_ind;
  tdsl1_cell_reselec_ind_type        cell_rsel_ind;
  tdsl1_measurement_ind_type         meas_ind;
  tdsl1_phychan_est_ind_type         phy_est_ind;
  tdsl1_cell_transition_cnf_type     cell_transition_cnf;
  tdsl1_drx_cnf_type                 drx_cnf;
  tdsl1_phychan_est_ind_type         l1_phychan_est_ind;
  tdsl1_meas_ctrl_cnf_type           meas_ctrl_cnf;
  tdsl1_act_time_cancel_cnf_type     act_cancel_cnf;


  tdsl1_deactivate_cnf_type          deactivate_cnf;
  tdsl1_stop_cnf_type                stop_cnf;
  tdsl1_start_cnf_type               start_cnf;
  tdsl1_suspend_cnf_type             suspend_cnf;
  tdsl1_resume_cnf_type              resume_cnf;

  tdsl1_dl_weak_ind_type             dl_weak_ind;

#ifdef FEATURE_TDSCDMA_DSDS
  tdsl1_out_of_service_type  oos_ind;
#endif
  tdsl1_activation_time_ind_type act_time_ind;

  /* Free Floating */
#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK

#ifdef FEATURE_TRITON_MODEM 
#else
#ifdef FEATURE_TDSCDMA_MODEM_STATISTICS
  tdsl1_modem_stat_rsp_cmd_type      modem_stat_rsp;
#endif
#endif /*FEATURE_TRITON_MODEM */
  tdsl1_s_criteria_cnf_type          s_criteria_cnf;
  tdsl1_rssi_info_struct_type     rssi_cnf;

#ifdef FEATURE_TDSCDMA_DATA_FOR_UI_BAR_DISPLAY
  tdsl1_ui_display_info_struct_type  data_for_ui_disp_cnf;
#endif

  tdsl1_sleep_req_type               l1_sleep_req;
  tdsl1_measured_results_struct_type add_meas_info_rsp;
  tdsl1_process_imm_meas_cnf_type    imm_meas_cnf;

#ifdef FEATURE_TDSCDMA_PLI_NMR_UTRAN
  tdsl1_utran_nmr_info_cnf_type      nmr_cnf;
#endif

#endif /*FEATURE_TDSCDMA_FREE_FLOATING_TASK*/

#ifdef FEATURE_TDSCDMA_DSDS_QTA
  tdsl1_start_qta_cmd_type           start_qta_cnf;
#endif

#ifdef FEATURE_TDSCDMA_G2T_TUNE_AWAY
  tdsl1_g2t_ta_status_type					 g2t_ta_mode_ind;
#endif

  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by MAC         */
  /*--------------------------------------------------------*/
  tdsmac_status_ind_e_type      cmac_status_ind;
  tdsmac_ul_traffic_meas_ind_type    cmac_ul_traffic_ind;
  tdsmac_meas_ctrl_cnf_type          mac_meas_ctrl_cnf;
  tdsmac_tfc_subset_cfg_cnf_type     mac_tfc_subset_cfg_cnf_info;


  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by RLC         */
  /*--------------------------------------------------------*/
  tdsrlc_status_ind_type        crlc_status_ind;
  tdsrlc_ul_tm_config_cnf_type  crlc_ul_tm_config_cnf;
  tdsrlc_dl_tm_config_cnf_type  crlc_dl_tm_config_cnf;
  tdsrlc_ul_um_config_cnf_type  crlc_ul_um_config_cnf;
  tdsrlc_dl_um_config_cnf_type  crlc_dl_um_config_cnf;
  tdsrlc_am_config_cnf_type     crlc_am_config_cnf;


  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by MM          */
  /*--------------------------------------------------------*/
  rrc_est_req_type           rrc_est_req;
  rrc_service_req_type       rrc_service_req;
  rrc_sim_inserted_req_type  rrc_sim_inserted_req;
  rrc_sim_update_req_type    rrc_sim_update_req;
  rrc_activation_rsp_type    activation_rsp;
  rrc_data_req_type          data_req;
  rrc_open_session_req_type  open_sess_req;
  rrc_close_session_req_type close_sess_req;
  rrc_plmn_list_req_type     plmn_list_req;
  rrc_forbid_lai_list_update_req_type forbid_lai_list_update_req;
  rrc_stop_wcdma_mode_req_type stop_wcdma_mode_req;

  rrc_change_mode_ind_type   rrc_change_mode_ind;
  rrc_abort_req_type         rrc_abort_req;
  rrc_deact_req              deact_req;
  rrc_nw_sel_mode_reset_ind_type    rrc_nw_sel_mode_reset_ind;
#ifdef FEATURE_CMCC_SECURE_CALL  
  tdsrrc_secure_call_ind_type  rrc_secure_call_ind;
#endif  
#ifdef FEATURE_DUAL_SIM
  tdsrrc_ds_status_change_ind_type  rrc_ds_status_change_ind;

  tdsrrc_subscription_id_type rrc_subscription_id;
  rrc_multimode_subs_chgd_ind_type rrc_multimode_subs_chgd_ind;
#endif
  tdsrrc_ds_tuneaway_status_change_ind_type rrc_ds_tuneaway_status_change_ind;
  rrc_fast_dormancy_req_type  rrc_fast_dormancy_req;
  rrc_csfb_call_status_ind_type rrc_csfb_call_status_ind;
  rrc_lte_reselection_allowed_status_ind_type  rrc_lte_resel_allowed_status_ind;
  rrc_lai_reject_list_ind_type      rrc_lai_reject_list_ind;
  rrc_sm_tuneaway_allowed_ind_type  rrc_sm_tuneaway_allowed_ind;

/*#ifdef FEATURE_SGLTE*/
  rrc_ue_mode_ind_type  rrc_ue_mode_ind;
/*#endif*/

  rrc_pseudo_lte_list_update_ind_type rrc_pseudo_lte_avail_update_ind;

  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by TC          */
  /*--------------------------------------------------------*/
#ifdef FEATURE_TC
  rrctc_mode_change_T        mode_change;
  rrctc_test_loopback_ind_T  test_loopback_ind;
#endif
  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by LSM         */
  /*--------------------------------------------------------*/
#if defined (FEATURE_CGPS_UMTS_CP_TDSCDMA)
  tdsrrc_meas_rpt_ind_type    meas_rpt_ind;
#endif

  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by CM          */
  /*--------------------------------------------------------*/

  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by MBMS Manager*/
  /*--------------------------------------------------------*/

  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent via MSGR       */
  /*--------------------------------------------------------*/
  tds_rrc_msgr_cmd_type tds_rrc_msgr_cmd;


 /* This dummy field has been added , so that both external and the 
 * RRC internal cmds have the same size for the cmd union. 
 * As the size of the command unoin is determined by the highest data type
 * present in the structure in ARM.*/
  /*Currently disabled for Q6 build*/
  /*uint64 dummyPadding;*/

}tdsrrc_cmd_data_u_type;


typedef struct
{
  /* Command header common for all commands */
  tdsrrc_cmd_hdr_type    cmd_hdr;

  tdsrrc_cmd_data_u_type cmd;

}tdsrrc_cmd_type;

#ifdef FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE
typedef struct
{
  /* Command header common for all commands */
  tdsrrc_cmd_hdr_type    cmd_hdr;
  tdsrrc_cmd_int_q_u_type cmd;
}tdsrrc_cmd_int_type;
#endif


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */


/* =======================================================================
**                        Function Declarations
** ======================================================================= */


  /*===========================================================================
  
  FUNCTION tdsrrc_get_pointer_l1_req_value
  
  DESCRIPTION
  
    This function get pointer for L1 command
  
  DEPENDENCIES
  
    None.
  
  RETURN VALUE
  
    None.
  
  SIDE EFFECTS
  
    None.
  
  ===========================================================================*/
#ifdef FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE
  void tdsrrc_get_pointer_l1_req_value
  (
    tdsl1_req_cmd **rrc_out_cmd_ptr,
    tdsl1_req_cmd **rrc_cmd_l1_req
  );
#else /*FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE*/
void tdsrrc_get_pointer_l1_req_value
(
  tdsl1_req_cmd *rrc_out_cmd_ptr,
  tdsl1_req_cmd **rrc_cmd_l1_req
);
#endif  /*FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE*/      

/*===========================================================================

FUNCTION tdsrrc_get_pointer_l2_req_value

DESCRIPTION

  This function get pointer for L2 command

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE
void tdsrrc_get_pointer_l2_req_value
(
  tdsl2_ul_req_cmd **rrc_out_cmd_ptr,
  tdsl2_ul_req_cmd **rrc_cmd_l2_req
);
#else /*FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE*/
void tdsrrc_get_pointer_l2_req_value
(
  tdsl2_ul_req_cmd *rrc_out_cmd_ptr,
  tdsl2_ul_req_cmd **rrc_cmd_l2_req
);
#endif  /*FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE*/

/*===========================================================================

FUNCTION RRC_GET_INT_CMD_BUF

DESCRIPTION

  This function returns a internal command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling tdsrrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type tdsrrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrc_cmd_type *tdsrrc_get_int_cmd_buf( void );

/*===========================================================================

FUNCTION RRC_GET_CMD_BUF

DESCRIPTION

  This function returns a internal command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling tdsrrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type tdsrrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrc_cmd_type *tdsrrc_get_cmd_buf( void );




/*===========================================================================

FUNCTION RRC_PUT_INT_CMD

DESCRIPTION

  This function puts a command buffer into RRC's internal
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_int_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void tdsrrc_put_int_cmd
(
  tdsrrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);


/*===========================================================================

FUNCTION RRC_PUT_INT_FF_CMD

DESCRIPTION

  This function puts a command buffer into RRC's l1 FF
  command queue, which is a sharing queue for all FF commands. 
  Note that the command buffer must have been requested through 
  a call to tdsrrc_get_int_cmd_buf(). 
  Also, the calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void tdsrrc_put_int_ff_cmd
(
  tdsrrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);



/*===========================================================================

FUNCTION RRC_FREE_CMD_BUF

DESCRIPTION

  This function frees a command buffer removed from any RRC
  command queue. Only commands removed from RRC's command queues
  can be passed into this function.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void tdsrrc_free_cmd_buf
(
  tdsrrc_cmd_type *cmd_buf    
);



/*===========================================================================

FUNCTION RRC_FREE_RR_CMD_BUF

DESCRIPTION

  This function frees a RR command buffer removed from RRC's RR
  command queue. Only commands removed from RRC's RR command queue
  can be passed into this function.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_free_rr_cmd_buf
(
  tdsrrc_rr_cmd_type *cmd_buf    /* Command Buffer */
);



#if defined (FEATURE_CGPS_UMTS_CP_TDSCDMA)

/*===========================================================================

FUNCTION RRC_GET_LSM_CMD_BUF

DESCRIPTION

  This function returns a LSM command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling tdsrrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type tdsrrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrc_cmd_type *tdsrrc_get_lsm_cmd_buf( void );

/*===========================================================================

FUNCTION RRC_PUT_LSM_CMD

DESCRIPTION

  This function puts a command buffer into RRC's LSM
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_lsm_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void tdsrrc_put_lsm_cmd
(
  tdsrrc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);

#endif

/*====================================================================
FUNCTION: rrcllc_free_embedded_buf()

DESCRIPTION:
  This function frees an embedded pointer (if any) from the command,
  queue.  Embedded memory allocations are not freed by tdsrrc_free_cmd_buf.
  Only the memory allocated for tdsrrc_cmd_type is freed.  This is needed
  to free memory allocations in TDSL1_CPHY_MEASUREMENT_REQ and TVM_REQ from
  RRC to L1 and RRC to MAC, when they are not routed through LLC.  If
  these commands are routed to LLC, LLC takes care of sending them to
  L1/MAC and deleting them if they are not routed and discarded..
  This function also frees memory allocated for the NAS Payload and
  measurement ind

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

=====================================================================*/
void tdsrrc_free_embedded_buf
(
  tdsrrc_cmd_type *cmd_buf,    /* Command Buffer */
  void         *ptr         /* ptr to the payload that needs to be freed */
);

/*===========================================================================
FUNCTION tdsrrc_get_int_cmd_buf_from_heap_only


DESCRIPTION

  This function returns a internal command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling tdsrrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.
 The function allocates memory from heap only and not from static buffer
DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type tdsrrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrc_cmd_type *tdsrrc_get_int_cmd_buf_from_heap_only
(
  void 
);

#ifdef FEATURE_TDSCDMA_RRC_REDUCE_CMD_SIZE
/*===========================================================================

FUNCTION tdsrrccmd_free_internal_req_ptr

DESCRIPTION

  This function frees the internal pointers present in the internal commands

DEPENDENCIES


RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrccmd_free_internal_req_ptr
(
  tdsrrc_cmd_type *cmd_buf 
);
#endif

/*===========================================================================

FUNCTION tdsrrc_free_embedd_buf_llc_req

DESCRIPTION

  This function frees the internal pointers present in the internal LLC commands

DEPENDENCIES


RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_free_embedd_buf_llc_req
(
  tdsrrc_cmd_type *cmd_buf
);


#endif /* RRCMD.H */

