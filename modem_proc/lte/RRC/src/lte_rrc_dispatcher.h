/*!
  @file lte_rrc_dispatcher.h

  @brief
  Header definitions for LTE RRC Dispatcher.

*/

/*===========================================================================

  Copyright (c) 2008 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_dispatcher.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/12   sk      Support for L to G CCO/NACC
12/12/11   kp      Added code for W2L PSHO
02/08/11   mm      Added definitions for IRAT CGI 
12/10/10   mm      Added definition for LTE_RRC_SRB2_RESUMED_INDI
10/19/10   mm      Added support for LTE_RRC_IRAT_FROM_LTE_TO_W_MOBILITY_INDI
08/11/10   mm      Registered new UMIDs for transfering dedicated priorities to 
                   IRAT
06/21/10   mm      Added support for LTE_RRC_IRAT_FROM_LTE_TO_G_MOBILITY_INDI
06/09/10   mm      Added new internal UMID for updating serving cell in CSP
06/07/10   sk      Added new internal UMID for CCO request to GSM
06/03/10   mm      Added LTE_CPHY_SERV_CELL_MEAS_REQ and CNF
05/18/10   mm      Added LTE_MAC_RELEASE_RESOURCES_IND instead of 
                   LTE_CPHY_OUT_OF_SYNC_REQ
05/10/10   da      Added lte_rrc_nmr_info_req_s to ext_msg_u.
05/07/10   amit    Added LTE_RRC_DLM_PROCESSED_INDI 
05/06/10   da      Added support for LTE_FC_SHUTDOWN_IND
05/03/10   sureshs Added PDCP and RLC Start and Stop confirms
04/28/10   sureshs Added MAC Start and Stop confirms
02/20/10   sk      Moved to use of actual GERAN UMIDs for LTE --> G resel 
                   and redir
                   Added LTE --> G capabilities support
02/18/10   np      Integrate with real W/api Header
02/09/10   np      Interface for G/W -> L Redir 
01/26/10   sk      Added G/W --> LTE redirection - RRC internal msg support
01/24/10   sk      Added support for From-G RAT MGR and G to L Reselection
01/14/10   sk      Added support for L --> G reselection
01/13/10   sk      Moved to use of actual HDR structs for LTE --> resel and redir
12/15/09   sk      Updated abort reqi struct name from CSP
                   Added new member for To LTE resel abort
12/10/09   sk      Added G/W --> LTE reselection support
                   Renamed lte_rrc_irat_resel_reqi_s to 
                   lte_rrc_irat_from_lte_resel_reqi_s
                   Renamed lte_rrc_irat_redir_reqi_s to 
                   lte_rrc_irat_from_lte_redir_reqi_s
12/10/09   sureshs Added extern for lte_rrc_dispatcher_activate_sms
12/06/09   sk      Added LTE --> DO reselection support
12/04/09   np      LTE -> 1x IRAT Resel Support
11/18/09   fhu     replaced lte_cphy_sap.h by lte_cphy_msg.h
11/17/09   amit    Added LTE_RRC_UNSOLICITED_SIB_INDI
10/28/09   np      Added IRAT reselection failed struct to int union
10/28/09   sureshs Added IRAT reselection structs to int union
10/07/09   sureshs Added suspend and resume cnfs to ext union
09/02/09   sk      Added support for LTE to DO redirection
08/03/09   mm      Replaced LTE_RRC_HANDOVER_INDI with an external indication
                   LTE_RRC_HANDOVER_IND
05/15/09   sureshs Added lte_rrc_initiate_conn_rel_indi_s to internal union
05/11/09   sk      Added new meas pending message
03/23/09   sureshs Added lte_rrc_conn_mode_cell_change_ind_s to unions
03/02/09   amit    Removed sib_timeout_indi
03/02/09   amit    Added sib_read_error_indi & removed 
                   sib_modification_started_indi
02/06/09   da      Added CRE indications
01/07/09   ask     Added measurement related cnfs
12/17/08   sureshs Added cell reselection ind and OOS ind
12/08/08   sureshs Added cell reselection canceled msg to internal union
11/10/08   amit    Replaced initiate_cell_sel_reqi with sib_timeout_indi
11/09/08   sureshs Added lte_cphy_dl_weak_ind_s
10/31/08   ask     Ported RRC to POSIX APIs
07/14/08   ak      Initial Revision

===========================================================================*/

#ifndef LTE_RRC_DISPATCHER_H
#define LTE_RRC_DISPATCHER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte.h>
#include <msgr.h>
#include <msgr_lte.h>
#include <stm2.h>

#include <lte_cphy_msg.h>
#include <lte_mac_msg.h>
#include <lte_mac_ind.h>
#include <lte_mac_rrc.h>
#include <lte_rlc_msg.h>
#include <lte_rlc_rrc.h>
#include <lte_pdcp_msg.h>
#include <lte_fc.h>

#include <lte_rrc_ext_msg.h>
#include <lte_rrc_int_msg.h>

#include "onex_cp_msg.h"
#include "wcdma_rrc_msg.h"
#include "tds_rrc_msg.h"
#include "hdrcp_msg.h"
#include "geran_grr_msg.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Union of external message types
    @detail This is a dummy union only declared to get the max size of an ext msg
*/
typedef union
{
  /* Requests */
  lte_rrc_conn_est_req_s            lte_rrc_conn_est_req;            
  lte_rrc_ul_data_req_s             lte_rrc_ul_data_req;
  lte_rrc_irat_tunnel_ul_msg_req_s  lte_rrc_irat_tunnel_ul_msg_req;
  lte_irat_cdma_tunnel_ul_msg_req_s lte_irat_cdma_tunnel_ul_msg_req;
  lte_rrc_deactivate_req_s          lte_rrc_deactivate_req;
  lte_rrc_conn_abort_req_s          lte_rrc_conn_abort_req;
  lte_rrc_service_req_s             lte_rrc_service_req;
  lte_rrc_system_update_req_s       lte_rrc_system_update_req;
  lte_rrc_sim_update_req_s          lte_rrc_sim_update_req;
  lte_rrc_forbidden_ta_list_reset_req_s lte_rrc_forbidden_ta_list_reset_req;
  lte_rrc_drx_info_req_s            lte_rrc_drx_info_req;
  lte_rrc_plmn_search_req_s         lte_rrc_plmn_search_req;
  lte_rrc_plmn_search_abort_req_s   lte_rrc_plmn_search_abort_req;
  lte_rrc_G_resel_req_s             lte_rrc_G_resel_req;
  lte_rrc_G_abort_resel_req_s       lte_rrc_G_abort_resel_req;
  lte_rrc_G_redir_req_s             lte_rrc_G_redir_req;
  lte_rrc_G_abort_redir_req_s       lte_rrc_G_abort_redir_req;
  lte_rrc_wcdma_resel_req_s         lte_rrc_wcdma_resel_req;
  lte_rrc_wcdma_abort_resel_req_s   lte_rrc_wcdma_abort_resel_req;
  lte_rrc_wcdma_redir_req_s         lte_rrc_wcdma_redir_req;
  lte_rrc_wcdma_abort_redir_req_s   lte_rrc_wcdma_abort_redir_req;
  lte_rrc_wcdma_psho_req_s          lte_rrc_wcdma_psho_req;
  lte_rrc_wcdma_abort_psho_req_s    lte_rrc_wcdma_abort_psho_req;
  lte_rrc_eHRPD_resel_req_s         lte_rrc_eHRPD_resel_req;
  lte_rrc_eHRPD_abort_resel_req_s   lte_rrc_eHRPD_abort_resel_req;
  lte_rrc_eHRPD_redir_req_s         lte_rrc_eHRPD_redir_req;
  lte_rrc_eHRPD_abort_redir_req_s   lte_rrc_eHRPD_abort_redir_req;
  lte_rrc_1x_resel_req_s            lte_rrc_1x_resel_req;
  lte_rrc_1x_abort_resel_req_s      lte_rrc_1x_abort_resel_req;
  /* TO DO: Remove after WMS removes reference to the old UMID */
  lte_rrc_etws_msg_id_list_req_s    lte_rrc_etws_msg_id_list_req;
  lte_rrc_msg_id_list_req_s         lte_rrc_msg_id_list_req;

  lte_rrc_nw_sel_mode_reset_req_s   lte_rrc_nw_sel_mode_reset_req;
  lte_rrc_eutra_capabilities_req_s  lte_rrc_eutra_capabilities_freq;
  lte_irat_plmn_srch_req_s          lte_irat_plmn_srch_req;
  lte_irat_abort_plmn_srch_req_s    lte_irat_abort_plmn_srch_req;
  lte_irat_suspend_plmn_srch_ind_s  lte_irat_suspend_plmn_srch_req;
  lte_rrc_get_dedicated_pri_req_s   lte_rrc_get_dedicated_pri_req;
  lte_rrc_im3_backoff_applied_req_s	lte_rrc_im3_backoff_applied_req;

  lte_rrc_embms_act_tmgi_req_s      lte_rrc_embms_act_tmgi_req;
  lte_rrc_embms_deact_tmgi_req_s    lte_rrc_embms_deact_tmgi_req;
  lte_rrc_embms_act_deact_tmgi_req_s lte_rrc_embms_act_deact_tmgi_req;
  lte_rrc_embms_avail_tmgi_list_req_s lte_rrc_embms_avail_tmgi_list_req;
  lte_rrc_embms_act_tmgi_list_req_s lte_rrc_embms_act_tmgi_list_req;
  lte_rrc_embms_enable_req_s        lte_rrc_embms_enable_req;
  lte_rrc_embms_disable_req_s       lte_rrc_embms_disable_req;
  lte_tlb_rrc_cfg_req_s             lte_tlb_rrc_cfg_req_s;
  lte_rrc_band_pri_change_req_s     lte_rrc_band_pri_change_req;
  lte_rrc_get_band_pri_list_req_s   lte_rrc_get_band_pri_list_req;
  lte_rrc_cmapi_embms_cov_status_req_s lte_rrc_cmapi_embms_cov_status_req;
  lte_rrc_cmapi_embms_datamcs_req_s lte_rrc_cmapi_embms_datamcs_req;
  lte_rrc_cell_lock_req_s           lte_rrc_cell_lock_req;
  lte_rrc_get_serv_cell_sib_req_s   lte_rrc_get_serv_cell_sib_req;
  lte_rrc_utra_serving_cell_info_req_s     lte_rrc_utra_serving_cell_info_req;
  lte_rrc_mtc_cfg_req_s             lte_rrc_mtc_cfg_req;
  lte_rrc_irat_capability_req_s      lte_rrc_irat_capability_req;
  lte_rrc_acq_db_earfcn_req_s       lte_rrc_acq_db_earfcn_req;

  /* Responses */
  lte_rrc_act_rsp_s                 lte_rrc_act_rsp;
  hdrcp_msg_rsp_u                   lte_irat_to_do_redir_reject_rsp;
  hdrcp_msg_rsp_u                   lte_irat_to_do_resel_reject_rsp;
  geran_grr_lte_redir_failed_rsp_t  lte_irat_to_G_redir_reject_rsp;
  geran_grr_lte_resel_failed_rsp_t  lte_irat_to_G_resel_reject_rsp;
  geran_grr_lte_cco_rsp_t           lte_irat_to_G_cco_rsp;

  wcdma_rrc_lte_resel_failed_rsp_type lte_irat_to_wcdma_resel_reject_rsp;
  wcdma_rrc_lte_abort_resel_rsp_type  lte_irat_to_wcdma_abort_resel_rsp;
  wcdma_rrc_lte_redir_failed_rsp_type lte_irat_to_wcdma_redir_reject_rsp;
  wcdma_rrc_lte_abort_redir_rsp_type  lte_irat_to_wcdma_abort_redir_rsp;
  wcdma_rrc_lte_utra_capabilities_rsp_type lte_irat_utra_capabilities_rsp;

  onex_cp_lte_redir_failed_rsp_msg_t lte_irat_to_1x_redir_reject_rsp;
  onex_cp_lte_resel_failed_rsp_msg_t lte_irat_to_1x_resel_reject_rsp;
  onex_cp_lte_abort_redir_rsp_msg_t  lte_irat_to_1x_abort_redir_reject_rsp;
  onex_cp_lte_abort_resel_rsp_msg_t  lte_irat_to_1x_abort_resel_reject_rsp;
  onex_cp_lte_1xcsfb_call_rel_ind_t  lte_irat_to_1x_call_rel_ind;
  onex_cp_lte_1xcsfb_ho_fail_rsp_t   lte_irat_to_1x_ho_fail_rsp;

  lte_irat_plmn_srch_rsp_s          lte_irat_plmn_srch_rsp;
  lte_irat_abort_plmn_srch_rsp_s    lte_irat_abort_plmn_srch_rsp;

  lte_cphy_ueinfo_rpt_rsp_s         lte_cphy_ueinfo_rpt_rsp;

  /* Indications */
  lte_cphy_mod_prd_boundary_ind_s   lte_cphy_mod_prd_boundary_ind;
  lte_mac_random_access_problem_ind_msg_s lte_mac_random_access_problem_ind_msg;
  lte_cphy_dl_weak_ind_s            lte_cphy_dl_weak_ind;
  lte_cphy_cell_resel_ind_s         lte_cphy_cell_resel_ind;
  lte_cphy_oos_ind_s                lte_cphy_oos_ind;
  lte_cphy_irat_asf_needed_ind_s    lte_cphy_irat_asf_needed_ind;
  lte_cphy_rl_failure_ind_s         lte_cphy_rl_failure_ind;
  lte_cphy_mcch_change_notification_ind_s lte_cphy_mcch_change_notification_ind;
  lte_rlcul_max_retx_ind_s          lte_rlcul_max_retx_ind;
  lte_l2_rlf_ind_msg_s              lte_l2_rlf_ind;
  lte_pdcpul_rlf_ind_msg_s          lte_pdcpul_rl_fail_ind;
  lte_rrc_handover_completed_ind_s  lte_rrc_handover_completed_ind;
  lte_rrc_service_ind_s             lte_rrc_service_ind;
  lte_rrc_clear_dedicated_pri_ind_s lte_rrc_clear_dedicated_pri_ind;
  lte_rrc_ue_mode_ind_s             lte_rrc_ue_mode_ind;
  lte_rrc_depri_requested_plmn_ind_s lte_rrc_depri_req_plmn_ind;
  lte_rrc_wcdma_resel_rsp_s         lte_rrc_wcdma_resel_rsp;
  lte_cphy_ifreq_otdoa_ind_s        lte_cphy_ifreq_otdoa_ind;
  lte_rrc_eab_update_ind_s      lte_rrc_eab_update_ind;
  cm_ims_emerg_cap_ind_s_type       cm_ims_emerg_cap_ind;
  lte_rrc_barring_update_ind_s      lte_rrc_barring_update_ind;
  lte_rrc_utra_serving_cell_info_rsp_s     lte_rrc_utra_serving_cell_info_rsp;
  lte_tlb_ctrl_update_loc_info_s    lte_tlb_ctrl_update_loc_info;

  /* Confirmations */
  lte_cphy_start_cnf_s              lte_cphy_start_cnf;
  lte_cphy_stop_cnf_s               lte_cphy_stop_cnf;
  lte_cphy_common_cfg_cnf_s         lte_cphy_common_cfg_cnf;
  lte_cphy_dedicated_cfg_cnf_s      lte_cphy_dedicated_cfg_cnf;
  lte_cphy_handover_cnf_s           lte_cphy_handover_cnf;
  lte_cphy_abort_cnf_s              lte_cphy_abort_cnf;
  lte_cphy_con_release_cnf_s        lte_cphy_con_release_cnf;
  lte_mac_cfg_cnf_msg_s             lte_mac_cfg_cnf_msg;
  lte_mac_access_cnf_msg_s          lte_mac_access_cnf_msg;    
  lte_mac_access_abort_cnf_msg_s    lte_mac_access_abort_cnf_msg;
  lte_mac_rach_rpt_cnf_msg_s        lte_mac_rach_rpt_cnf;
  lte_cphy_ueinfo_conn_failure_report_cnf_s lte_cphy_ueinfo_conn_failure_report_cnf;
  lte_rlc_cfg_cnf_s                 lte_rlc_cfg_cnf;
  lte_pdcpdl_cfg_cnf_msg_s          lte_pdcpdl_cfg_cnf_msg;
  lte_pdcpul_cfg_cnf_msg_s          lte_pdcpul_cfg_cnf_msg;
  lte_pdcpul_sdu_cnf_msg_s          lte_pdcpul_sdu_cnf_msg;
  lte_pdcpdl_recfg_prep_cnf_msg_s   lte_pdcpdl_recfg_prep_cnf_msg;
  lte_pdcpul_recfg_prep_cnf_msg_s   lte_pdcpul_recfg_prep_cnf_msg;
  lte_cphy_acq_cnf_s                lte_cphy_acq_cnf;
  lte_cphy_system_scan_cnf_s        lte_cphy_system_scan_cnf;
  lte_cphy_band_scan_cnf_s          lte_cphy_band_scan_cnf;
  lte_cphy_cell_select_cnf_s        lte_cphy_cell_select_cnf;
  lte_cphy_idle_meas_cfg_cnf_s      lte_cphy_idle_meas_cfg_cnf;
  lte_cphy_conn_meas_cfg_cnf_s      lte_cphy_conn_meas_cfg_cnf;
  lte_cphy_serv_cell_meas_cnf_s     lte_cphy_serv_cell_meas_cnf;
  lte_cphy_conn_meas_report_ind_s   lte_cphy_conn_meas_report_ind;
  lte_mac_release_resources_ind_s   lte_mac_release_resources_ind;
  hdrcp_msg_rsp_u                   lte_irat_to_do_redir_abort_cnf;
  hdrcp_msg_rsp_u                   lte_irat_to_do_resel_abort_cnf;
  geran_grr_msg_rsp_u               lte_irat_to_G_redir_abort_cnf;
  geran_grr_msg_rsp_u               lte_irat_to_G_resel_abort_cnf;
  lte_cphy_suspend_cnf_s            lte_cphy_suspend_cnf;
  lte_cphy_resume_cnf_s             lte_cphy_resume_cnf;
  lte_cphy_msg_bplmn_cell_cnf_s     lte_cphy_bplmn_cell_cnf;
  lte_cphy_msg_bplmn_complete_ind_s lte_cphy_bplmn_complete_ind;
  lte_cphy_msg_bplmn_stop_cnf_s     lte_cphy_bplmn_stop_cnf;
  lte_cphy_msg_bplmn_time_avail_ind_s lte_cphy_bplmn_time_avail_ind;
  lte_cphy_msg_bplmn_suspend_cnf_s     lte_cphy_bplmn_suspend_cnf;
  geran_grr_lte_cs_capabilities_rsp_t geran_grr_lte_cs_capabilities_rsp;
  geran_grr_lte_ps_capabilities_rsp_t geran_grr_lte_ps_capabilities_rsp;
  geran_grr_lte_dedicated_priorities_rsp_t geran_grr_lte_get_dedicated_pri_rsp;
  lte_mac_start_cnf_msg_s           lte_mac_start_cnf_msg;
  lte_mac_stop_cnf_msg_s            lte_mac_stop_cnf_msg;
  lte_pdcp_start_cnf_msg_s          lte_pdcp_start_cnf_msg;
  lte_pdcp_stop_cnf_msg_s           lte_pdcp_stop_cnf_msg;
  lte_rlc_start_cnf_s               lte_rlc_start_cnf;
  lte_rlc_stop_cnf_s                lte_rlc_stop_cnf;
  lte_fc_shutdown_ind_s             lte_fc_shutdown_ind;
  lte_pdcpul_conn_rel_ind_msg_s         lte_pdcpul_conn_rel_ind;
  lte_rrc_nmr_info_req_s            lte_rrc_nmr_info_req;
  lte_cphy_nmr_info_cnf_s           lte_cphy_nmr_info_cnf;
  lte_cphy_best_mbms_neigh_freq_cnf_s lte_cphy_best_mbms_neigh_freq_cnf;
  lte_cphy_release_trm_cnf_s        lte_cphy_release_trm_cnf;
  lte_common_msim_crat_feature_cnf_s  lte_rrc_common_msim_crat_feature_cnf;
  lte_cphy_harq_ack_tx_sts_cnf_s    lte_cphy_harq_ack_tx_sts_cnf;
  /* Supervisory */
  msgr_spr_loopback_struct_type     msgr_spr_loopback;
  lte_rrc_nas_lte_key_rsp_s			lte_rrc_nas_lte_key_rsp;
  lte_tlb_rrc_cfg_cnf_s             lte_tlb_rrc_cfg_cnf;
  lte_dsds_trm_priority_req_s          lte_rrc_dsds_trm_priority_req;
  lte_cphy_rf_available_ind_s                 lte_cphy_rf_available_ind;
  lte_cphy_rf_unavailable_ind_s                 lte_cphy_rf_unavailable_ind;
} lte_rrc_ext_msg_u;

/*! @brief Union of externalota message types
    @detail This is a dummy union only declared to get the max size of an extota msg
*/
typedef union
{
  /* Indications */
  lte_cphy_mib_ind_s                lte_cphy_mib_ind;
  lte_mac_rrc_dl_data_ind_s         lte_mac_rrc_dl_data_ind;
  lte_pdcpdl_sdu_ind_msg_s          lte_pdcpdl_sdu_ind_msg;
  lte_rlcdl_mcch_pdu_ind_s          lte_rlcdl_mcch_pdu_ind;
  
} lte_rrc_extota_msg_u;


/*! @brief Union of internal message types
    @detail This is a dummy union only declared to get the max size of an int msg
*/
typedef union
{
  /* Requests */
  lte_rrc_send_ul_msg_reqi_s lte_rrc_send_ulmsg_reqi;
  lte_rrc_mode_change_reqi_s lte_rrc_mode_change_reqi;
  lte_rrc_get_sibs_reqi_s lte_rrc_get_sibs_reqi;
  lte_rrc_sib_abort_reqi_s lte_rrc_sibs_abort_reqi;
  lte_rrc_cfg_reqi_s lte_rrc_cfg_reqi;
  lte_rrc_pended_cfg_reqi_s lte_rrc_pended_cfg_reqi;
  lte_rrc_meas_cfg_reqi_s lte_rrc_meas_cfg_reqi;
  lte_rrc_initiate_cell_sel_reqi_s lte_rrc_initiate_cell_sel_reqi;
  lte_rrc_irat_from_lte_redir_reqi_s  lte_rrc_irat_redir_reqi;
  lte_rrc_irat_from_lte_internal_redir_reqi_s  lte_rrc_irat_internal_redir_reqi;
  lte_rrc_irat_from_lte_abort_reqi_s  lte_rrc_irat_from_lte_abort_reqi;
  lte_rrc_irat_to_lte_resel_abort_reqi_s  lte_rrc_irat_to_lte_resel_abort_reqi;
  lte_rrc_irat_from_lte_resel_reqi_s  lte_rrc_irat_resel_reqi;
  lte_rrc_irat_to_lte_resel_reqi_s lte_rrc_irat_to_lte_resel_reqi;
  lte_rrc_irat_to_lte_psho_reqi_s  lte_rrc_irat_psho_reqi;
  lte_rrc_irat_to_lte_psho_abort_reqi_s lte_rrc_irat_to_lte_psho_abort_reqi;
  lte_rrc_irat_from_lte_to_do_capabilities_reqi_s lte_rrc_irat_do_capabilities_reqi;
  lte_rrc_irat_from_lte_to_w_capabilities_reqi_s lte_rrc_irat_w_capabilities_reqi;
  lte_rrc_irat_from_lte_to_tds_capabilities_reqi_s lte_rrc_irat_tds_capabilities_reqi;
  lte_rrc_irat_from_lte_to_g_cs_capabilities_reqi_s lte_rrc_irat_g_cs_capabilities_reqi;
  lte_rrc_irat_from_lte_to_g_ps_capabilities_reqi_s lte_rrc_irat_g_ps_capabilities_reqi;
  lte_rrc_csp_cell_select_abort_reqi_s lte_rrc_csp_cell_select_abort_reqi;
  lte_rrc_plmn_search_abort_reqi_s lte_rrc_plmn_search_abort_reqi;
  lte_rrc_irat_to_lte_redir_list_reqi_s lte_rrc_irat_to_lte_redir_list_reqi;
  lte_rrc_irat_to_lte_redir_full_reqi_s lte_rrc_irat_to_lte_redir_full_reqi;
  lte_rrc_irat_to_lte_redir_abort_reqi_s  lte_rrc_irat_to_lte_redir_abort_reqi;
  lte_irat_plmn_srch_req_s lte_irat_plmn_srch_reqi;
  lte_irat_abort_plmn_srch_req_s lte_irat_abort_plmn_srch_reqi;
  lte_irat_suspend_plmn_srch_ind_s lte_irat_suspend_plmn_srch_indi;
  lte_rrc_ml1_revoke_reqi_s lte_rrc_ml1_revoke_reqi;
  lte_rrc_irat_from_lte_to_g_cco_reqi_s lte_rrc_irat_from_lte_to_g_cco_reqi;
  lte_rrc_irat_from_lte_to_w_get_cgi_reqi_s lte_rrc_irat_from_lte_to_w_get_cgi_reqi;
  lte_rrc_irat_from_lte_to_w_abort_cgi_reqi_s lte_rrc_irat_from_lte_to_w_abort_cgi_reqi;
  lte_rrc_irat_from_lte_to_g_get_cgi_reqi_s lte_rrc_irat_from_lte_to_g_get_cgi_reqi;
  lte_rrc_irat_from_lte_to_g_abort_cgi_reqi_s lte_rrc_irat_from_lte_to_g_abort_cgi_reqi;
  lte_rrc_irat_from_lte_to_do_get_cgi_reqi_s lte_rrc_irat_from_lte_to_do_get_cgi_reqi;
  lte_rrc_irat_from_lte_to_do_abort_cgi_reqi_s lte_rrc_irat_from_lte_to_do_abort_cgi_reqi;
  lte_rrc_irat_from_lte_to_1x_get_cgi_reqi_s lte_rrc_irat_from_lte_to_1x_get_cgi_reqi;
  lte_rrc_esmgr_act_tmgi_reqi_s lte_rrc_esmgr_act_tmgi_reqi;
  lte_rrc_esmgr_deact_tmgi_reqi_s lte_rrc_esmgr_deact_tmgi_reqi;
  lte_rrc_esmgr_act_deact_tmgi_reqi_s lte_rrc_esmgr_act_deact_tmgi_reqi;
  lte_rrc_esmgr_avail_tmgi_list_reqi_s lte_rrc_esmgr_avail_tmgi_list_reqi;
  lte_rrc_start_internal_cell_selection_reqi_s lte_rrc_start_internal_cell_selection_reqi;

  /* Confirmations */
  lte_rrc_ul_msg_cnfi_s lte_rrc_ul_msg_cnfi;
  lte_rrc_mode_change_cnfi_s lte_rrc_mode_change_cnfi;
  lte_rrc_get_sibs_cnfi_s lte_rrc_get_sibs_cnfi;
  lte_rrc_cfg_cnfi_s lte_rrc_cfg_cnfi;
  lte_rrc_meas_cfg_cnfi_s lte_rrc_meas_cfg_cnfi;
  lte_rrc_irat_to_lte_resel_cnfi_s lte_rrc_irat_to_lte_resel_cnfi;
  lte_rrc_irat_to_lte_resel_abort_cnfi_s lte_rrc_irat_to_lte_resel_abort_cnfi;
  lte_rrc_irat_to_lte_psho_cnfi_s lte_rrc_irat_to_lte_psho_cnfi;
  lte_rrc_irat_to_lte_psho_abort_cnfi_s lte_rrc_irat_to_lte_psho_abort_cnfi;
  lte_rrc_irat_from_do_to_lte_capabilities_cnfi_s lte_rrc_irat_do_capabilities_cnfi;
  lte_rrc_irat_from_w_to_lte_capabilities_cnfi_s lte_rrc_irat_w_capabilities_cnfi;
  lte_rrc_irat_from_g_cs_to_lte_capabilities_cnfi_s lte_rrc_irat_g_cs_capabilities_cnfi;
  lte_rrc_irat_from_g_ps_to_lte_capabilities_cnfi_s lte_rrc_irat_g_ps_capabilities_cnfi;
  lte_rrc_csp_cell_select_abort_cnfi_s lte_rrc_cell_select_abort_cnfi;
  lte_rrc_irat_to_lte_redir_cnfi_s lte_rrc_irat_to_lte_redir_cnfi;
  lte_rrc_irat_to_lte_redir_abort_cnfi_s lte_rrc_irat_to_lte_redir_abort_cnfi;
  lte_irat_plmn_srch_cnfi_s lte_irat_plmn_srch_cnfi;
  lte_irat_abort_plmn_srch_rsp_s lte_irat_abort_plmn_srch_cnfi;
  lte_rrc_irat_from_lte_abort_cnfi_s  lte_rrc_irat_from_lte_abort_cnfi;
  lte_rrc_ml1_revoke_cnfi_s lte_rrc_ml1_revoke_cnfi;
  lte_rrc_irat_from_lte_to_w_get_cgi_cnfi_s lte_rrc_irat_from_lte_to_w_get_cgi_cnfi;
  lte_rrc_irat_from_lte_to_g_get_cgi_cnfi_s lte_rrc_irat_from_lte_to_g_get_cgi_cnfi;
  lte_rrc_irat_from_lte_to_g_abort_cgi_cnfi_s lte_rrc_irat_from_lte_to_g_abort_cgi_cnfi;
  lte_rrc_irat_from_lte_to_do_get_cgi_cnfi_s lte_rrc_irat_from_lte_to_do_get_cgi_cnfi;
  lte_rrc_irat_from_lte_to_1x_get_cgi_cnfi_s lte_rrc_irat_from_lte_to_1x_get_cgi_cnfi;
  lte_rrc_esmgr_act_tmgi_cnfi_s lte_rrc_esmgr_act_tmgi_cnfi;
  lte_rrc_esmgr_deact_tmgi_cnfi_s lte_rrc_esmgr_deact_tmgi_cnfi;
  lte_rrc_esmgr_act_deact_tmgi_cnfi_s lte_rrc_esmgr_act_deact_tmgi_cnfi;
  lte_rrc_csg_asf_search_cnfi_s lte_rrc_csg_asf_search_cnfi;
  lte_rrc_plmn_search_cnfi_s lte_rrc_plmn_search_cnfi;
  lte_rrc_csg_cfg_indi_s lte_rrc_csg_cfg_indi;

  /* Indications */
  lte_rrc_dlm_processed_indi_s lte_rrc_dlm_processed_indi;
  lte_rrc_stopped_indi_s lte_rrc_stopped_indi;
  lte_rrc_unsolicited_sib_indi_s lte_rrc_unsolicited_sib_indi;
  lte_rrc_sib_updated_indi_s lte_rrc_sib_updated_indi;
  lte_rrc_sib_updated_indi_s lte_rrc_meas_pend_sib_updated_indi;
  lte_rrc_sib_read_error_indi_s lte_rrc_sib_read_error_indi;
  lte_rrc_conn_establishment_started_indi_s lte_rrc_conn_establishment_started_indi;
  lte_rrc_connected_indi_s lte_rrc_connected_indi;
  lte_rrc_conn_establishment_failure_indi_s lte_rrc_conn_establishment_failure_indi;
  lte_rrc_stop_cell_resel_indi_s lte_rrc_stop_cell_resel_indi;
  lte_rrc_proceed_with_resel_indi_s lte_rrc_proceed_with_resel_indi;
  lte_rrc_conn_rel_started_indi_s lte_rrc_conn_rel_started_indi;
  lte_rrc_conn_released_indi_s lte_rrc_conn_released_indi;
  lte_rrc_not_camped_indi_s lte_rrc_not_camped_indi;
  lte_rrc_camped_indi_s lte_rrc_camped_indi;
  lte_rrc_cell_resel_started_indi_s lte_rrc_cell_resel_started_indi;
  lte_rrc_cell_resel_canceled_indi_s lte_rrc_cell_resel_canceled_indi;
  lte_rrc_handover_started_indi_s lte_rrc_handover_started_indi;
  lte_rrc_cre_started_indi_s lte_rrc_cre_started_indi;
  lte_rrc_cre_completed_indi_s lte_rrc_cre_completed_indi;
  lte_rrc_conn_mode_failure_indi_s lte_rrc_conn_mode_failure_indi;
  lte_rrc_initiate_conn_rel_indi_s lte_rrc_initiate_conn_rel_indi;
  lte_rrc_serving_cell_changed_indi_s lte_rrc_serving_cell_changed_indi;
  lte_rrc_irat_from_lte_redir_failed_indi_s lte_rrc_irat_from_lte_redir_failed_indi;
  lte_rrc_irat_from_lte_resel_failed_indi_s lte_rrc_irat_from_lte_resel_failed_indi;
  lte_rrc_irat_from_lte_plmn_srch_resume_failed_indi_s lte_rrc_irat_from_lte_plmn_srch_resume_failed_indi;
  lte_rrc_ded_priority_list_indi_s lte_rrc_ded_priority_list_indi;
  lte_rrc_irat_to_lte_resel_completed_indi_s lte_rrc_irat_to_lte_resel_completed_indi;
  lte_rrc_irat_to_lte_psho_completed_indi_s lte_rrc_irat_to_lte_psho_completed_indi;
  lte_rrc_irat_to_lte_redir_completed_indi_s lte_rrc_irat_to_lte_redir_completed_indi;
  lte_rrc_irat_utra_resel_wait_time_indi_s lte_rrc_irat_utra_resel_wait_time_indi;
  lte_rrc_irat_from_lte_to_g_mobility_indi_s lte_rrc_irat_from_lte_to_g_mobility_indi;
  lte_rrc_irat_from_lte_to_w_mobility_indi_s lte_rrc_irat_from_lte_to_w_mobility_indi;
  lte_rrc_irat_from_lte_to_1x_mobility_indi_s lte_rrc_irat_from_lte_to_1x_mobility_indi;
  lte_rrc_irat_from_lte_mobility_started_indi_s lte_rrc_irat_from_lte_mobility_started_indi;
  lte_rrc_srb2_resumed_indi_s lte_rrc_srb2_resumed_indi;
  lte_rrc_irat_hrpd_pre_reg_info_indi_s lte_rrc_irat_hrpd_pre_reg_info_indi;
  lte_rrc_irat_to_lte_psho_started_indi_s lte_rrc_irat_to_lte_psho_started_indi;
  lte_rrc_irat_to_lte_psho_key_gen_indi_s lte_rrc_irat_to_lte_psho_key_gen_indi;
  lte_rrc_irat_lte_depri_indi_s lte_rrc_irat_lte_depri_indi;
  lte_rrc_esmgr_resel_prio_change_indi_s lte_rrc_esmgr_resel_prio_change_indi;
  lte_rrc_trm_priority_change_indi_s lte_rrc_trm_priority_change_indi;
  lte_rrc_irat_from_lte_resel_success_indi_s lte_rrc_irat_from_lte_resel_success_indi;
  lte_rrc_prox_cfg_indi_s lte_rrc_prox_cfg_indi;
  lte_rrc_loc_services_info_indi_s lte_rrc_loc_services_info_indi;
  lte_rrc_sib1_avail_indi_s lte_rrc_sib1_avail_indi;
  lte_rrc_sec_active_indi_s lte_rrc_sec_active_indi;
  lte_rrc_qsh_cb_ind_s lte_rrc_qsh_cb_indi;

  //CEP opt
  lte_rrc_oos_indi_s lte_rrc_oos_indi;
  lte_rrc_emp_deact_tmgi_indi_s lte_rrc_emp_deact_tmgi_indi;
  lte_rrc_emp_oos_warn_indi_s lte_rrc_emp_oos_warn_indi;
  lte_rrc_emp_pended_service_indi_s lte_rrc_emp_pended_service_indi;
  lte_rrc_esmgr_avail_tmgi_list_indi_s lte_rrc_esmgr_avail_tmgi_list_indi;
  lte_rrc_depri_freq_indi_s lte_rrc_depri_freq_indi;
  lte_rrc_clear_depri_freq_indi_s lte_rrc_clear_depri_freq_indi;

  /* DLMs */
  lte_rrc_mh_dlm_s lte_rrc_mh_dlm;

  /* Timers */
  lte_rrc_tmri_s lte_rrc_tmri;

  lte_rrc_irat_to_lte_psho_success_indi_s lte_rrc_irat_to_lte_psho_success_indi;

  #ifdef FEATURE_LTE_ELS_ENABLED
  lte_rrc_auto_els_ue_cap_info_indi_s lte_rrc_auto_els_ue_cap_info_indi;
  #endif
  
} lte_rrc_int_msg_u;


/*! @brief Union of all msgs that RRC can receive
*/
typedef union
{
  msgr_hdr_s        msgr_hdr;

  lte_rrc_int_msg_u int_msg;
  lte_rrc_ext_msg_u ext_msg;
  lte_rrc_extota_msg_u extdlm_msg;

} lte_rrc_msg_u;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

lte_errno_e lte_rrc_dispatcher_init
(
  void
);

lte_errno_e lte_rrc_dispatcher_deinit
(
  void
);

lte_errno_e lte_rrc_dispatcher_test_init
(
  stm_state_machine_t *sm_ut,    /*!< RRC state machine under test */
  stm_state_machine_t *test_sm   /*!< State machine of the unit test framework */
);

lte_errno_e lte_rrc_dispatcher_add_sm
(
  stm_state_machine_t *sm,    /*!< State machine to add */
  uint32 umid                 /*!< UMID to register this sm for */
);

void* lte_rrc_dispatcher_get_msg_buf
(
  void *msg_ptr,        /*!< Msg payload received by stm handler */
  int *buf_len          /*!< <OUT> Length of the returned buffer */ 
);

lte_errno_e lte_rrc_dispatcher_send_synch_msg
(
  msgr_hdr_s *msg_ptr,      /*!< Ptr to the message buffer */
  uint32 msg_len            /*!< Length of the message buffer */
);

lte_errno_e lte_rrc_dispatcher_dispatch
(
  msgr_hdr_s *msg_ptr,      /*!< Ptr to the message buffer */
  uint32    msg_len         /*!< Length of the message buffer */
);

void lte_rrc_dispatcher_loop
(
  void
);

extern boolean lte_rrc_dispatcher_activate_sms
(
  void
);

#endif /* LTE_RRC_DISPATCHER_H */
