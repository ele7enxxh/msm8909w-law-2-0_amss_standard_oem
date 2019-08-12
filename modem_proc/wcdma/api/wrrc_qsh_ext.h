
#ifndef WRRC_QSH_EXT_H
#define WRRC_QSH_EXT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            WRRC QSH EXT . H                 

GENERAL DESCRIPTION
   Contains declarations for function which are QUALCOMM proprietary 
   and may or may not be shipped as source code.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:$
  $Header: 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/09/16   sp     Made changes to log connection end information and multi rab status
06/09/15   sg      Made changes to add PSC to timers and counters metric
03/20/15   sp     Made changes to convert ota metric msg_id to enum and include t314 logging
03/10/15   ad      Made changes to log important parms from OTAs
2/06/14    sg     Initial version for wrrc_qsh_ext.h

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include <string.h>
#include "trm.h"
#include "qsh.h"
#include "sys.h"

#define WRRC_QSH_MAJOR_VER 0
#define WRRC_QSH_MINOR_VER 1

/* Below data structures are defined for mini dump */

#define RRC_CRASH_DEBUG_OPT_MM_CMD_CNT 10
#define RRC_CRASH_DEBUG_OPT_IRAT_RRC_CMD_CNT 3
#define RRC_CRASH_DEBUG_OPT_INTERNAL_CMD_CNT 25
#define RRC_CRASH_DEBUG_OPT_L1_RRC_CMD_CNT 10
#define RRC_CRASH_DEBUG_OPT_LLC_CMD_CNT 8
#define RRC_CRASH_DEBUG_OPT_OTA_SIZE 1000

typedef enum
{
  WRRC_QSH_DUMP_TAG_CONN_MODE_GLOBALS = 0, /* wrrc_qsh_dump_tag_conn_mode_globals*/
  WRRC_QSH_DUMP_TAG_CSP_CCM_SIB_RCE_DEBUG,/*wrrc_qsh_dump_tag_csp_ccm_sib_rce*/
  WRRC_QSH_DUMP_TAG_MCM_DEBUG,/*wrrc_qsh_dump_tag_mcm*/
  WRRC_QSH_DUMP_TAG_LLC_DEBUG,/*wrrc_qsh_dump_tag_llc_debug*/
  WRRC_QSH_DUMP_TAG_CMD_OTAS_DEBUG,/* */  
  WRRC_QSH_DUMP_TAG_NV_VALUES,/*wrrc_qsh_dump_tag_nv_values*/
  WRRC_QSH_DUMP_TAG_CU_DEBUG,/*wrrc_qsh_dump_tag_cu*/
  WRRC_QSH_DUMP_TAG_RB_PROC_DEBUG,/*wrrc_qsh_dump_tag_rb_proc*/
  WRRC_QSH_DUMP_TAG_MISC_PROC_DEBUG,/*wrrc_qsh_dump_tag_misc_proc*/
  WRRC_QSH_DUMP_TAG_CCTRCH_DEBUG,/*wrrc_qsh_dump_tag_cctrch_debug*/
  WRRC_QSH_DUMP_TAG_MAX
}wrrc_qsh_dump_tag_e;

typedef PACK(struct)
{
 uint16 timeticks; 
 uint16 config;
 uint32 cmd_id;
}rrc_debug_opt_cmd_type;

typedef PACK(struct)
{
  uint16 timeticks;
  uint16 config;
  uint32 cmd_id;/*rrc_to_mm_sink_e_type*/
}rrc_to_mm_debug_opt_cmd_type;

typedef PACK(struct)
{
  uint16 timeticks;
  uint16 config;
  uint32 cmd_id;/*rrc_rr_cmd_e_type*/
}rrc_rr_debug_opt_cmd_type;


typedef PACK(struct)
{
  uint16 timeticks;
  uint16 config;
  uint32 cmd_id;/*l1_cmd_enum_type*/
}rrc_to_l1_opt_cmd_type;


typedef PACK(struct)
{
  uint16 timeticks;
  uint16 config;
  uint32 cmd_id;
}lte_cmds_opt_type;


typedef PACK(struct)
{
  uint16 timeticks;
  uint16 config;
  uint32 l1_llc_cmd;
}rrc_llc_cmd_opt_type;

typedef PACK(struct)
{
  rrc_debug_opt_cmd_type mm_to_rrc_debug_cmds[RRC_CRASH_DEBUG_OPT_MM_CMD_CNT];
  rrc_to_mm_debug_opt_cmd_type rrc_to_mm_debug_cmds[RRC_CRASH_DEBUG_OPT_MM_CMD_CNT];
}rrc_crash_debug_opt_mm_cmd_type;

typedef PACK(struct)
{
  rrc_debug_opt_cmd_type rr_to_rrc_debug_cmds[RRC_CRASH_DEBUG_OPT_IRAT_RRC_CMD_CNT];
  rrc_rr_debug_opt_cmd_type rrc_to_rr_debug_cmds[RRC_CRASH_DEBUG_OPT_IRAT_RRC_CMD_CNT];
}rrc_crash_debug_opt_rr_cmd_type;

typedef PACK(struct)
{
  rrc_debug_opt_cmd_type int_cmd[RRC_CRASH_DEBUG_OPT_INTERNAL_CMD_CNT];
}rrc_crash_debug_opt_int_cmd_type;

typedef PACK(struct)
{
  rrc_debug_opt_cmd_type l1_to_rrc_debug_cmds[RRC_CRASH_DEBUG_OPT_L1_RRC_CMD_CNT];
  rrc_to_l1_opt_cmd_type rrc_to_l1_debug_cmds[RRC_CRASH_DEBUG_OPT_L1_RRC_CMD_CNT];
}rrc_crash_debug_opt_l1_cmd_type;

typedef PACK(struct)
{
  lte_cmds_opt_type lte_to_rrc_debug_cmds[RRC_CRASH_DEBUG_OPT_IRAT_RRC_CMD_CNT]; 
  lte_cmds_opt_type rrc_to_lte_debug_cmds[RRC_CRASH_DEBUG_OPT_IRAT_RRC_CMD_CNT];
}rrc_crash_debug_opt_lte_cmd_type;

typedef PACK(struct)
{
  rrc_llc_cmd_opt_type rrc_llc_cmds[RRC_CRASH_DEBUG_OPT_LLC_CMD_CNT];
}rrc_crash_debug_opt_llc_cmd_type;

typedef PACK(struct)
{
  rrc_crash_debug_opt_mm_cmd_type rrc_crash_debug_mm_cmds;
  rrc_crash_debug_opt_rr_cmd_type rrc_crash_debug_rr_cmds;
  rrc_crash_debug_opt_int_cmd_type rrc_crash_debug_int_cmds;
  rrc_crash_debug_opt_l1_cmd_type rrc_crash_debug_l1_cmds;
  rrc_crash_debug_opt_lte_cmd_type rrc_crash_debug_lte_cmds;
  rrc_crash_debug_opt_llc_cmd_type rrc_crash_debug_llc_cmds;  
}rrc_crash_debug_opt_info_type;

typedef PACK(struct)
{
  qsh_dump_tag_hdr_s hdr;
  uint8 rrc_mode;/*rrc_mode_e_type*/
  uint8 rrc_curr_state; /*rrc_state_e_type  */
  uint8 config_ptr_in_use; /* rrcllc_config_e_type */
  uint8 rrc_oc_state;  /*rrc_state_e_type  */
  uint8 rrc_cc_state;  /*rrc_state_e_type  */
  uint8 rrc_oc_set_status; /* rrcllc_oc_set_status_e_type */
  uint8 rrc_oc_set_proc; /* rrc_proc_e_type */
  uint8 rrc_oc_process_state; /* rrcllc_oc_process_state_e_type */
  uint8 toc_usage;/* rrcllc_toc_usage_e_type */
  uint8 hsdpa_action; /* hsdpa_action_enum_type */
  uint8 is_sib5_change_in_progress; /* rrcllc_sib5_change_type */
  uint8 oc_rnti_valid; /*mac_rnti_valid_e_type */
  uint8 oc_hrnti_status; /* rrc_hsdpa_hrnti_enum_type */
  uint8 oc_efach_hrnti_status; /*efach_hrnti_type_enum */
  uint8 oc_hs_action_in_e_fach;  /*hsdpa_action_enum_type */
  uint8 cc_rnti_valid; /*mac_rnti_valid_e_type */
  uint8 cc_hrnti_status; /* rrc_hsdpa_hrnti_enum_type */
  uint8 cc_efach_hrnti_status; /*efach_hrnti_type_enum */
  uint8 cc_hs_action_in_e_fach;  /*hsdpa_action_enum_type */
  uint8 dest_freq; /*rrc_freq_type*/  
  uint8 ctch_status; /* rrcllc_ctch_status_e_type */  
  uint8 oc_l1_hs_rach_req_mask;  
  uint8 cc_l1_hs_rach_req_mask;
  uint8 sec_eul_is_hho:1 ;
  uint8 dest_psc_present:1 ; 
  uint8 hs_rach_to_eul_trans:1 ;
  uint8 common_edch_transmission:1 ;
  uint8 hspa_rnti_stored_cell_pch:1 ;
  uint8 cpc_dtx_drx_status:1 ;
  uint8 cpc_drx_status:1 ;
  uint8 e_dch_transmission:1 ;
  uint8 sec_e_dch_transmission:1 ;
  uint8 hs_status_in_e_fach:1 ;
  uint8 hsdpa_status:1 ;
  uint8 revert_old_config:1 ;
}wrrc_qsh_dump_tag_conn_mode_globals; /* 25 bytes */

typedef PACK(struct)
{
  qsh_dump_tag_hdr_s hdr;
  uint16 wcdma_nv_hsupa_category;
  uint16 rrc_feature_nv_item_value;
  uint16 rrc_dsds_concurrency;  
  uint16 rrc_dsda_concurrency;
  uint8 rrc_nv_rel_indicator_debug;
  uint8 nv_hsdpa_category_debug;
  uint8 rrc_default_cfg_in_cell_fach_nv_debug;
  uint8 wcdma_look_for_concurrency_nv_debug :1 ;
  uint8 feature_dch_fach_opt_enabled_debug :1 ;
}wrrc_qsh_dump_tag_nv_values; /* 12 bytes */

typedef PACK(struct)
{
  qsh_dump_tag_hdr_s hdr;
  uint32 num_stored_cells_sib_db;
  uint32 active_cell_index;
  uint8 last_mm_primitive; /* rrc_csp_mm_primitive_e_type */
  uint8 curr_type_acq; /* rrc_csp_acq_e_type */
  uint8 is_valid_nw_sharing;
  uint8 csfb_extended_status; /* rrccsp_csfb_status_e_type */
  uint8 interrat_type; /* rrc_csp_interrat_rat_e_type */
  uint8 bplmn_interrat_type; /* rrc_csp_interrat_rat_e_type */
  uint8 bplmn_suspend_proc; /* rrc_proc_e_type */
  uint8 bplmn_suspend_cause; /* rrc_bplmn_suspend_cause_e_type */
  uint8 rrc_csp_substate; /* rrc_csp_substate_e_type */	
  uint8 curr_scan;/* rrc_csp_scan_e_type */
  uint8 pending_type;/* rrc_csp_pending_e_type */
  uint8 cell_camp_status ;/* rrc_new_cell_status_e_type */
  uint8 camping_status;/* rrc_ccm_cell_camping_status_valid_e_type */
  uint8 rrc_ccm_substate;/* rrc_ccm_substate_e_type */
  uint8 srv_bcch_action;/* sib_bcch_action_e_type */
  uint8 ngh_bcch_action;/* sib_bcch_action_e_type */
  uint8 rrcrce_substate;/* rrcrce_substate_e_type */
  uint8 rrc_establishment_cause; /* rrc_establish_cause_e_type */
  uint8 rrc_v_300;
  uint8 rrc_release_cause; /* rrc_rel_cause_e_type */
  uint8 rrcrce_mm_abort_cause;/* rrc_abort_cause_e_type */
  uint8 init_cell_sel_ind_during_wtointerrat:1 ;
  uint8 bplmn_guard_srch_tmr_expired:1 ;
  uint8 bplmn_srch_tmr_expired:1 ;
  uint8 rrc_csfb_call_status:1 ;
  uint8 rrcrce_send_abort_cnf :1 ;
  uint8 rrcrce_abort_ind_sent:1 ;
  uint8 update_sibs_event_pending:1 ;
  uint8 event0_active:1 ;
  uint8 event1_active:1 ;
  uint8 sib_db_empty :1;
  uint8 ue_in_deep_sleep :1;
  uint8 acq_db_empty :1;
  uint8 bplmn_new_srch :1;
  uint8 bplmn_srch_cnf :1;
  uint8 do_lfs_scan :1;
  uint8 stop_pending_for_wtog_abort :1;
  uint8 allow_lte_resel :1;
  uint8 bplmn_data_saved :1;
  uint8 ue_in_conn_oos :1;
  uint8 dch_to_fach_pch_tmr_expd :1;
}wrrc_qsh_dump_tag_csp_ccm_sib_rce;/* 32 bytes */


typedef PACK(struct)
{
  qsh_dump_tag_hdr_s hdr;
  uint64 cphy_setup_req_mask[5];	/* last 5 cphy setup req masks */
  uint32 rrc_current_cmd_id;/* Current cmd being processed *//*  rrc_cmd_e_type */
  uint8 current_cmd_engine; /* rrcllc_cmd_engine_e_type */
  uint8 chan_config_reason; /* rrcllc_chan_cfg_reason_e_type */
  uint8 int_cmd_q_cnt; 
  uint8 llc_cmd_q_cnt; 
  uint8 mm_cmd_q_cnt; 
  uint8 l1_cmd_q_cnt; 
  uint8 rrcllc_current_substate;
  uint8 procedure;  
  uint8 next_rrc_state; 
  uint8 cmd_under_process_valid :1 ;
}wrrc_qsh_dump_tag_llc_debug; /* 56 bytes */


typedef PACK(struct)
{
  qsh_dump_tag_hdr_s hdr;
  uint8 rrccu_substate; /* rrccu_substate_e_type */  
  uint8 rrccu_cell_update_cause; /* rrc_CellUpdateCause */
  uint8 rrccu_ura_update_cause; /* rrc_URA_UpdateCause */
  uint8 rrccu_state_indicator; /* rrc_RRC_StateIndicator */
  uint8 rrccu_resp_msg;/* rrccu_resp_msg_e_type */  
  uint8 rrccu_tran_to_discon_counter;  
  uint8 rrccu_buffered_cmd_mask;
  uint8 rrccu_reestablish_srb:1 ;
  uint8 rrccu_reestablish_rab:1 ;
}wrrc_qsh_dump_tag_cu; /* 8 bytes */


typedef PACK(struct)
{
  qsh_dump_tag_hdr_s hdr;
  uint8 rrc_rbe_substate; /* rrcrb_substate_e_type */
  uint8 rrc_rbrc_substate; /* rrcrb_substate_e_type */
  uint8 rrc_pcr_substate; /* rrcrb_substate_e_type*/
  uint8 rrc_tcr_substate; /* rrcrb_substate_e_type */
  uint8 rrc_rbr_substate; /* rrcrb_substate_e_type */
  uint8 num_of_ps_rabs;
  uint8 num_of_cs_rabs;
  uint8 call_type_mask;
  uint8 rrcrbe_send_initiate_cu_flag:1 ;
  uint8 rrcrbrc_send_initiate_cu_flag:1 ;
  uint8 rrcpcrc_send_initiate_cu_flag:1 ;
  uint8 rrctcrc_send_initiate_cu_flag:1 ;
  uint8 rrcrbr_send_initiate_cu_flag:1 ;  
}wrrc_qsh_dump_tag_rb_proc; /* 9 bytes */

typedef PACK(struct)
{
  qsh_dump_tag_hdr_s hdr;
  uint32 rrcmcm_cmd_store_mm_cmdid;/* rrc_cmd_e_type */
  uint32 rrcmcm_cmd_store_interat_resel_rr_cmdid;/* rrc_rr_cmd_e_type */
  uint32 rrcmcm_cmd_store_lte_irat_cmdid;/* rrc_cmd_e_type */
  uint8 rrcmcm_state;/* rrcmcm_state_e_type */  
  uint8 rrcmcm_cmd_store_abort_flag;/* rrcmcm_abort_flag_e_type */
  uint8 rrcmcm_procedure_id;/* rrcmcm_abort_flag_e_type */ /* Destination procedure for change mode confirmation */
  uint8 rrcmcm_stop_cause;/* rrc_stop_cause_e_type */
  uint8 rrcwrm_unlock_type; /* trm_unlock_event_enum_t */
  uint8 wcdma_device_mode; /* sys_modem_device_mode_e_type */
  uint8 rrc_dual_mode_enabled :1;
  uint8 rrc_lte_mode_enabled :1;
  uint8 rrc_is_ue_in_sglte_mode :1;
  uint8 stop_cause :1;
  uint8 rrcmcm_send_stop_cnf_to_mm_flag :1;
  uint8 rrcmcm_est_req_saved_flag :1;
  uint8 wcdma_is_ue_in_dual_standby :1;
  uint8 wcdma_is_ue_in_sxlte_mode :1;
}wrrc_qsh_dump_tag_mcm; /* 20 bytes */

typedef PACK(struct)
{
  qsh_dump_tag_hdr_s hdr;
  uint32 rrcmeas_meas_id_bitmask;
  uint32 config_id; /* default_config_id in handover message */
  uint8 misc_globals[150];
  uint8 rrcmeas_current_substate;/* rrcmeas_substate_e_type */
  uint8 rrcidt_substate;
  uint8 rrcudt_substate;
  uint8 rrcscrr_substate;
  uint8 rrciho_state; 
  uint8 rrc_ho_config_type;
  uint8 rrccho_substate; 
  uint8 status;
  uint8 failure_reason;
  uint8 rrcueci_substate;
  uint8 asu_substate;
  uint8 umi_substate;
  uint8 rrcsmc_substate;/* rrcsmc_substate_e_type */
  uint8 rrcrcr_substate; /* rrcrcr_substate_e_type */
  uint8 rrc_test_mode; /*byte*/
  uint8 lbt_substate;
  uint8 couec_substate; /*rrccouec.substate */
  uint8 rrc_dl_amr_mode;
  uint8 ho_from_gsm_eutra:1 ; /* TRUE- GSM, FALSE, EUTRA */
  uint8 rrcdt_int_data:1 ;
  uint8 umi_for_srns_relocation:1 ;
  uint8 rrciho_abort:1;
}wrrc_qsh_dump_tag_misc_proc; /*180 bytes */

typedef PACK(struct)
{
  qsh_dump_tag_hdr_s hdr;
  uint8 used_cctrch_bm;
  uint8 to_be_freed_cctrch_bm;
  uint8	pccpch_is_up:1 ;
  uint8	n_pccpch_is_up:1 ;
  uint8	sccpch_is_up:1 ;
  uint8	dl_dpch_is_up:1 ;
  uint8	fach_included:1 ;
  uint8	pch_included:1 ;
  uint8	pch_included_for_sccpch_with_ctch:1 ;
  uint8	sccpch_with_ctch_is_up:1 ;
}wrrc_qsh_dump_tag_cctrch_debug;/* 3 bytes */

typedef PACK(struct)
{
  qsh_dump_tag_hdr_s hdr;
  rrc_crash_debug_opt_info_type rrc_crash_debug_info_opt; /* 680 bytes */
  /* This OTA in crashdebugging with help in finding the configuration to all the layers in AS and is very helpful in finding the scenari of crash easily */
  uint8 ota[RRC_CRASH_DEBUG_OPT_OTA_SIZE]; /* 1000 */ 
}wrrc_qsh_dump_tag_cmds_otas_debug; /* 1680 bytes */


/* Below data structures are defined for metrics */
/* WRRC doesnt maintain any arrays. QSH takes care of maintaining the arrays for each of the metrics */
/* WRRC Just logs the events whenever they are triggered */
typedef enum{
  WRRC_NON_HHO,
  WRRC_TIMING_INIT,
  WRRC_TIMING_MAINTAINED,
  WRRC_REDIRECTION,
  WRRC_HHO_INVALID
}wrrc_hho_type_e;

typedef enum
{
  WRRC_QSH_METRIC_RESEL_CEL_SEL, /* wrrc_qsh_metric_resel_cel_sel_s */
  WRRC_QSH_METRIC_RLF_OOS, /* wrrc_qsh_metric_rlf_oos_s */
  WRRC_QSH_METRIC_HO, /* wrrc_qsh_metric_ho_s */
  WRRC_QSH_METRIC_ASET, /* wrrc_qsh_metric_aset_s */
  WRRC_QSH_METRIC_SERVING, /* wrrc_qsh_metric_serving_cell_s */
  WRRC_QSH_METRIC_TIMERS_AND_CONSTANTS,
  WRRC_QSH_METRIC_OTA,/* wrrc_qsh_metric_ota_s */
  WRRC_QSH_METRIC_MM_TO_RRC_CMD,/* wrrc_qsh_metric_mm_to_rrc_cmd_s */
  WRRC_QSH_METRIC_RRC_TO_MM_CMD,/* wrrc_qsh_metric_rrc_to_mm_cmd_s */
  WRRC_QSH_METRIC_RR_TO_RRC_CMD,/* wrrc_qsh_metric_rr_to_rrc_cmd_s */
  WRRC_QSH_METRIC_RRC_TO_RR_CMD,/* wrrc_qsh_metric_rrc_to_rr_cmd_s */
  WRRC_QSH_METRIC_RRC_TO_L1_CMD,/* wrrc_qsh_metric_rrc_to_l1_cmd_s */
  WRRC_QSH_METRIC_L1_TO_RRC_CMD,/* wrrc_qsh_metric_l1_to_rrc_cmd_s */
  WRRC_QSH_METRIC_RRC_TO_LTE_CMD,/* wrrc_qsh_metric_rrc_to_lte_cmd_s */
  WRRC_QSH_METRIC_LTE_TO_RRC_CMD,/* wrrc_qsh_metric_lte_to_rrc_cmd_s */
  WRRC_QSH_METRIC_INTERNAL_CMD,/* wrrc_qsh_metric_internal_cmd_s */
  WRRC_QSH_METRIC_LLC_CMD,/* wrrc_qsh_metric_llc_cmd_s */
  WRRC_QSH_METRIC_RRC_STATE,/* wrrc_qsh_metric_rrc_state_history */
  WRRC_QSH_METRIC_MEAS_ASU_OTA,/*wrrc_qsh_metric_meas_asu_ota_s */
  WRRC_QSH_METRIC_CONN_END_INFO,
  WRRC_QSH_METRIC_MULTI_RAB_STATUS,
  WRRC_QSH_METRIC_MAX
}wrrc_qsh_metric_e;
typedef enum
{
  WRRC_QSH_EVENT_MAX
}wrrc_qsh_event_e; /*WRRC EVENTS */

typedef struct{
  qsh_metric_hdr_s hdr; 
  uint8 num_u_to_l_resel;
  uint8 num_u_to_u_resel;
  uint8 num_u_to_g_resel;
  uint8 num_resel_fail;  
  uint8 num_cel_sel_fail;  
}wrrc_qsh_metric_resel_cel_sel_s;

typedef struct{
  qsh_metric_hdr_s hdr; 
  uint8 num_rlf;
  uint8 num_oos;
  uint8 num_con_reject;
}wrrc_qsh_metric_rlf_oos_s;

typedef struct {
  qsh_metric_hdr_s hdr; 
  uint8 num_u_to_l_ho;
  uint8 num_u_to_g_ho;
  uint8 num_inter_rat_ho_fail;/* only where W is originating RAT we log the failure, eg W->G*/ 
  uint8 num_srvcc;
  uint8 num_csfb;
  uint8 num_u_to_u_hho;
  uint8 num_hho_fail;
}wrrc_qsh_metric_ho_s; 

typedef struct {
  qsh_metric_hdr_s hdr; 
  uint8 num_rl;
}wrrc_qsh_metric_aset_s;
/* Non_HSUPA_Capable_Cell-0,HSUPA_Capable_Cell-1, HSUPA_Curently_Configured-2, DCHSUPA_Currently_Configured-3 */
typedef enum
{
  NON_HSDPA_CAPABLE,
  HSDPA_CAPABLE,
  HSDPA_UNKNOWN
}wrrc_hsdpa_cell_capability_enum_type;

typedef enum
{
  NON_HSUPA_CAPABLE,
  HSUPA_CAPABLE,
  HSUPA_UNKNOWN 
}wrrc_hsupa_cell_capability_enum_type;

/* Transition cnf and channel config cnf is the trigger to log these */
typedef struct {
  qsh_metric_hdr_s hdr;
  uint32 cell_id;
  boolean csg_indication;
  uint16 drx_cycle;/* OTA or sib configured */   
  uint8 rrc_state;
  uint16 uarfcn;
  uint16 psc;
  wrrc_hsdpa_cell_capability_enum_type hsdpa_cell_capability;/* Need to define an enum */
  boolean hsdpa_call_status; /* TRUE if HSDPA is configured */
  wrrc_hsupa_cell_capability_enum_type hsupa_cell_capability;/* Need to define an enum */
  boolean hsupa_call_status;
  boolean fdpch_status;  
  boolean comp_mode_status;
  int8 ul_interference;
  boolean cpc_dtx_status;
  boolean cpc_drx_status;
}wrrc_qsh_metric_serving_cell_s;

typedef struct {
  qsh_metric_hdr_s hdr;
  uint8 t313; 
  uint8 t300;
  uint8 t302;
  uint8 n313;
  uint8 n315;
  uint8 n300;
  uint8 n302;
  uint8 t314;
  uint16 psc;
}wrrc_qsh_metric_tmr_and_const_s;

typedef struct{
  uint8 num_rb_add;
  uint8 num_rb_rel;
  uint8 num_rb_recfg;
  uint8 oc_set_status; /* or state indicator */
  uint8 hho_type; /*non-hho, timing init, timing maintained, redirection *//* for non-dch otas, if freq info is included save this as redirection*/
  uint8 mapping_combo_srb; /* save it for SRB2 */
  uint8 mapping_combo_drb; /* save it only for 1st PS RB */
  uint8 srns_relocation_invloved:1;
  uint8 rlc_size_type :1; /* fixed or flexible for 1st PS RB	*/
  uint8 one_sided_re_est :1;
  uint8 cpc_dtx_drx_status :1;
  uint8 edch_transmission :1; /* even though mapping is provided, if some other edch info is missn this is false, so it may be important to save this */
  uint8 sec_edch_transmission :1;
  uint8 hsdpa_status :1; 
  uint8 dchsdpa_status :1;
}rrc_overall_ota_info_s;

typedef struct{
  rrc_overall_ota_info_s overall_ota;
  uint8 cap_requests;
}rrc_consetup_ota_parm_s;

typedef struct{
  rrc_overall_ota_info_s overall_ota;
  uint8 reserved;
}rrc_rb_recfg_ota_parm_s;

typedef struct{
  uint8 domain;
  uint8 int_algo;
  uint8 ciph_algo;
  uint8 sec_capability;
}rrc_smc_ota_parms_s;

typedef struct{
  rrc_overall_ota_info_s overall_ota;
  uint8 srb_re_est_ind :1;
  uint8 drb_re_est_ind :1;
  uint8 new_crnti_present :1;
  uint8 new_urnti_present :1;
}rrc_cuc_ota_param_s;

typedef struct{
  uint8 cell_update_cause;
  uint8 est_cause;
  uint8 rsi;
  uint8 rue_on_srb; /* RLC unrecoverable error */
  uint8 rue_on_drb; 
  uint8 security_revert_status;
  uint8 failure_cause;
  uint8 t314_expired:1;
  uint8 t315_expired:1;
}rrc_cu_ota_parms_s;

typedef struct{
  uint8 count_c_activation;
  uint8 failure_cause;/* only for failure messages */
  uint8 ul_integrity_info_present:1;
  uint8 ul_counter_sync_info:1;
}rrc_ul_ota_parms_s;

typedef struct{
  boolean new_crnti_valid; /*Indicates if a new crnti is recvd */
  boolean new_urnti_valid; /* Indicates if a valid U-RNTI was recd */ 
  boolean umi_for_srns_relocation;  
  boolean new_hrnti_valid;   
  uint8 timer_const_rcvd;
}rrc_umi_ota_parms_s;

typedef struct{
  rrc_overall_ota_info_s overall_ota;
  uint8 reserved;
}rrc_ho_to_utran_parm_s;

typedef struct
{
  uint32 fail_cause; /* DCS1800 or PCS 1900 */
}rrc_log_rrc_ho_from_utran_fail_ota;

typedef struct
{
  uint8 cn_id; /* Redirected RAT */
}rrc_log_rrc_scrr_ota;

typedef struct
{
  uint8 mcf_fail_cause; /* Measurement control failure cause */
}rrc_log_rrc_mcf_ota;

typedef struct
{
  uint8 cco_fail_cause; /* Cell change order failure cause */
}rrc_log_rrc_cco_fail_ota;

typedef struct
{
  ARFCN_T    BCCH_ARFCN;   /* indicates the Target GSM Cell BCCH ARFCN */
  uint8      BSIC_NCC;  /* indicates the Target GSM Cell BSIC */
  uint8      BSIC_BCC;
}rrc_log_rrc_cco_ota;


typedef struct
{
  uint8 lte_container_length; /* LTE capability container size */
}rrc_log_rrc_conn_setup_compl_ota;

typedef struct
{
  uint8    sys_info_mod:1; /* Sys info modification present */
  uint8    page_record_match:1; /* Page record match found */
  uint8    cn_id:1;  /* Domain id */
  uint8    paging_cause; /* Paging cause */
}rrc_log_pg_ota;


typedef struct
{
  uint8 conn_rej_cause; /* RRC connection reject cause */
  uint8 wait_time; /* RRC conn reject wait time */
  uint8 redir_info_present:1; /* Redirection info present */
  uint8 redir_rat; /* Redirected RAT */
}rrc_log_rrc_conn_rej_ota;

typedef struct
{
  uint8 conn_rel_cause; /* RRC connection release cause */
  uint8 redir_info_present:1; /* Redirection info present */
  uint8 redir_rat; /* Redirected RAT */
}rrc_log_rrc_conn_rel_ota;

typedef struct
{
  uint8 cn_id;
  uint8 est_cause;  /* Connection establishment cause */
  uint8 as_rel_indicator; /* UE release indicator */
}rrc_log_rrc_conn_req_ota;


typedef struct
{
  uint8 cn_id; /* Domain id */
  uint8 est_cause; /* IDT establishment cause */
}rrc_log_rrc_idt_ota;

typedef struct
{
  uint8 cn_id; /* Redirected RAT */
  uint8 scri_cause; /* SCRI cause */
}rrc_log_rrc_scri_ota;

typedef struct
{
  uint8 freq_band:1; /* DCS1800 or PCS 1900 */
  uint8 ho_msg_type; /* Single GSM message or Message list */
}rrc_log_rrc_ho_gsm_ota;


typedef union {
  rrc_consetup_ota_parm_s con_setup;
  rrc_rb_recfg_ota_parm_s rb_recfg;
  rrc_smc_ota_parms_s smc_params;
  rrc_cuc_ota_param_s cuc_params;
  rrc_cu_ota_parms_s cu_params;
  rrc_ul_ota_parms_s ul_ota_parms; /* recfg cplt and rbsetup complete */
  rrc_umi_ota_parms_s umi_ota_params;
  rrc_ho_to_utran_parm_s ho_to_utran_params;
  rrc_log_rrc_ho_gsm_ota ho_to_gsm;
  rrc_log_rrc_ho_from_utran_fail_ota ho_from_utran_fail_params;
  rrc_log_rrc_scrr_ota sig_conn_rel;
  rrc_log_rrc_scri_ota sig_conn_rel_ind;
  rrc_log_rrc_idt_ota idt_params;
  rrc_log_rrc_cco_fail_ota cco_fail_params;
  rrc_log_rrc_cco_ota cco_params;
  rrc_log_rrc_conn_setup_compl_ota conn_setup_compl_params;
  rrc_log_rrc_conn_req_ota conn_req_ota;
  rrc_log_rrc_conn_rel_ota conn_rel_ota;
  rrc_log_rrc_conn_rej_ota conn_rej_ota;
  rrc_log_pg_ota paging_type1;
  rrc_log_pg_ota paging_type2;
}rrc_ota_param_u;

typedef enum {
	WRRC_UL_CCCH_MESSAGETYPE_CELLUPDATE,
	WRRC_UL_CCCH_MESSAGETYPE_RRCCONNECTIONREQUEST,
	WRRC_UL_CCCH_MESSAGETYPE_URAUPDATE,
	WRRC_DL_CCCH_MESSAGETYPE_CELLUPDATECONFIRM,
	WRRC_DL_CCCH_MESSAGETYPE_RRCCONNECTIONREJECT,
	WRRC_DL_CCCH_MESSAGETYPE_RRCCONNECTIONRELEASE,
	WRRC_DL_CCCH_MESSAGETYPE_RRCCONNECTIONSETUP,
	WRRC_DL_CCCH_MESSAGETYPE_URAUPDATECONFIRM,
	WRRC_UL_DCCH_MESSAGETYPE_ACTIVESETUPDATECOMPLETE,
	WRRC_UL_DCCH_MESSAGETYPE_ACTIVESETUPDATEFAILURE,
	WRRC_UL_DCCH_MESSAGETYPE_CELLCHANGEORDERFROMUTRANFAILURE,
	WRRC_UL_DCCH_MESSAGETYPE_COUNTERCHECKRESPONSE,
	WRRC_UL_DCCH_MESSAGETYPE_HANDOVERTOUTRANCOMPLETE,
	WRRC_UL_DCCH_MESSAGETYPE_INITIALDIRECTTRANSFER,
	WRRC_UL_DCCH_MESSAGETYPE_HANDOVERFROMUTRANFAILURE,
	WRRC_UL_DCCH_MESSAGETYPE_MEASUREMENTCONTROLFAILURE,
	WRRC_UL_DCCH_MESSAGETYPE_MEASUREMENTREPORT,
	WRRC_UL_DCCH_MESSAGETYPE_PHYSICALCHANNELRECONFIGURATIONCOMPLETE,
	WRRC_UL_DCCH_MESSAGETYPE_PHYSICALCHANNELRECONFIGURATIONFAILURE,
	WRRC_UL_DCCH_MESSAGETYPE_RADIOBEARERRECONFIGURATIONCOMPLETE,
	WRRC_UL_DCCH_MESSAGETYPE_RADIOBEARERRECONFIGURATIONFAILURE,
	WRRC_UL_DCCH_MESSAGETYPE_RADIOBEARERRELEASECOMPLETE,
	WRRC_UL_DCCH_MESSAGETYPE_RADIOBEARERRELEASEFAILURE,
	WRRC_UL_DCCH_MESSAGETYPE_RADIOBEARERSETUPCOMPLETE,
	WRRC_UL_DCCH_MESSAGETYPE_RADIOBEARERSETUPFAILURE,
	WRRC_UL_DCCH_MESSAGETYPE_RRCCONNECTIONRELEASECOMPLETE,
	WRRC_UL_DCCH_MESSAGETYPE_RRCCONNECTIONSETUPCOMPLETE,
	WRRC_UL_DCCH_MESSAGETYPE_RRCSTATUS,
	WRRC_UL_DCCH_MESSAGETYPE_SECURITYMODECOMPLETE,
	WRRC_UL_DCCH_MESSAGETYPE_SECURITYMODEFAILURE,
	WRRC_UL_DCCH_MESSAGETYPE_SIGNALLINGCONNECTIONRELEASEINDICATION,
	WRRC_UL_DCCH_MESSAGETYPE_TRANSPORTCHANNELRECONFIGURATIONCOMPLETE,
	WRRC_UL_DCCH_MESSAGETYPE_TRANSPORTCHANNELRECONFIGURATIONFAILURE,
	WRRC_UL_DCCH_MESSAGETYPE_TRANSPORTFORMATCOMBINATIONCONTROLFAILURE,
	WRRC_UL_DCCH_MESSAGETYPE_UECAPABILITYINFORMATION,
	WRRC_UL_DCCH_MESSAGETYPE_UPLINKDIRECTTRANSFER,
	WRRC_UL_DCCH_MESSAGETYPE_UTRANMOBILITYINFORMATIONCONFIRM,
	WRRC_UL_DCCH_MESSAGETYPE_UTRANMOBILITYINFORMATIONFAILURE,
	WRRC_UL_DCCH_MESSAGETYPE_MBMSMODIFICATIONREQUEST,
	WRRC_UL_DCCH_MESSAGETYPE_UL_DCCH_MESSAGETYPE_EXT,
	WRRC_DL_DCCH_MESSAGETYPE_ACTIVESETUPDATE,
	WRRC_DL_DCCH_MESSAGETYPE_ASSISTANCEDATADELIVERY,
	WRRC_DL_DCCH_MESSAGETYPE_CELLCHANGEORDERFROMUTRAN,
	WRRC_DL_DCCH_MESSAGETYPE_CELLUPDATECONFIRM,
	WRRC_DL_DCCH_MESSAGETYPE_COUNTERCHECK,
	WRRC_DL_DCCH_MESSAGETYPE_DOWNLINKDIRECTTRANSFER,
	WRRC_DL_DCCH_MESSAGETYPE_HANDOVERFROMUTRANCOMMAND_GSM,
	WRRC_DL_DCCH_MESSAGETYPE_HANDOVERFROMUTRANCOMMAND_CDMA2000,
	WRRC_DL_DCCH_MESSAGETYPE_MEASUREMENTCONTROL,
	WRRC_DL_DCCH_MESSAGETYPE_PAGINGTYPE2,
	WRRC_DL_DCCH_MESSAGETYPE_PHYSICALCHANNELRECONFIGURATION,
	WRRC_DL_DCCH_MESSAGETYPE_PHYSICALSHAREDCHANNELALLOCATION,
	WRRC_DL_DCCH_MESSAGETYPE_RADIOBEARERRECONFIGURATION,
	WRRC_DL_DCCH_MESSAGETYPE_RADIOBEARERRELEASE,
	WRRC_DL_DCCH_MESSAGETYPE_RADIOBEARERSETUP,
	WRRC_DL_DCCH_MESSAGETYPE_RRCCONNECTIONRELEASE,
	WRRC_DL_DCCH_MESSAGETYPE_SECURITYMODECOMMAND,
	WRRC_DL_DCCH_MESSAGETYPE_SIGNALLINGCONNECTIONRELEASE,
	WRRC_DL_DCCH_MESSAGETYPE_TRANSPORTCHANNELRECONFIGURATION,
	WRRC_DL_DCCH_MESSAGETYPE_TRANSPORTFORMATCOMBINATIONCONTROL,
	WRRC_DL_DCCH_MESSAGETYPE_UECAPABILITYENQUIRY,
	WRRC_DL_DCCH_MESSAGETYPE_UECAPABILITYINFORMATIONCONFIRM,
	WRRC_DL_DCCH_MESSAGETYPE_UPLINKPHYSICALCHANNELCONTROL,
	WRRC_DL_DCCH_MESSAGETYPE_URAUPDATECONFIRM,
	WRRC_DL_DCCH_MESSAGETYPE_UTRANMOBILITYINFORMATION,
	WRRC_DL_DCCH_MESSAGETYPE_HANDOVERFROMUTRANCOMMAND_GERANIU,
	WRRC_DL_DCCH_MESSAGETYPE_MBMSMODIFIEDSERVICESINFORMATION,
	WRRC_DL_DCCH_MESSAGETYPE_ETWSPRIMARYNOTIFICATIONWITHSECURITY,
	WRRC_DL_DCCH_MESSAGETYPE_HANDOVERFROMUTRANCOMMAND_EUTRA,
	WRRC_DL_DCCH_MESSAGETYPE_UEINFORMATIONREQUEST,
	WRRC_DL_DCCH_MESSAGETYPE_LOGGINGMEASUREMENTCONFIGURATION,
	WRRC_BCCH_FACH_MESSAGETYPE_SYSTEMINFORMATION,
	WRRC_BCCH_FACH_MESSAGETYPE_SYSTEMINFORMATIONCHANGEINDICATION,
	WRRC_PCCH_MESSAGETYPE_PAGINGTYPE1,
	WRRC_DL_DCCH_MESSAGETYPE_HANDOVERTOUTRAN,
	WRRC_MESSAGETYPE_INVALID
}wrrc_qsh_ota_msg_id;
typedef struct {
  qsh_metric_hdr_s hdr;
  wrrc_qsh_ota_msg_id msg_id;
  rrc_ota_param_u ota_info;
}wrrc_qsh_metric_ota_s;

typedef struct
{
  uint8 meas_id;  /* Measurement identity */
  uint8 meas_type; /* Intra/inter/irat/TVM/POS/Internal */ 
  uint8 meas_rpt_type; /* Event or periodic or additional */
  uint8 meas_event_id_bmask; /* Bitmask indicating the event */
}rrc_log_rrc_mcm_ota;

typedef struct
{
  uint8 meas_id;  /* Measurement identity */
  uint8 meas_type; /* Intra/inter/irat/TVM/quality */ 
  uint8 meas_rpt_type; /* Event or periodic or additional */
  uint8 event_id; /* Bitmask indicating event */
  uint8 num_cells_rpt; /* Number of cell reported */
}rrc_log_rrc_mrm_ota;

typedef struct{
  rrc_overall_ota_info_s overall_ota;
  uint8 num_rls_added;
  uint8 num_rls_removed;	
}rrc_dl_asu_ota_parms_s;

typedef struct{
  uint32 trans_id;
  uint8 failure_cause;/* only for failure messages */
  uint8 ul_integrity_info_present:1;
}rrc_ul_asu_ota_parms_s;
	
typedef union{
  rrc_dl_asu_ota_parms_s asu_info;
  rrc_ul_asu_ota_parms_s ul_asu_info;
  rrc_log_rrc_mcf_ota mcf_params;
  rrc_log_rrc_mrm_ota mrm_ota;
  rrc_log_rrc_mcm_ota mcm_ota;
}rrc_asu_meas_ota_param_u;

typedef struct {
  qsh_metric_hdr_s hdr;
  wrrc_qsh_ota_msg_id msg_id;
  rrc_asu_meas_ota_param_u asu_meas_info;
}wrrc_qsh_metric_meas_asu_ota_s;

typedef union
{
  /* Connection req to be added */
  rrc_cu_ota_parms_s cu_params;
  rrc_log_rrc_mcf_ota mcf_params;
  rrc_ul_asu_ota_parms_s ul_asu_info;
  rrc_log_rrc_cco_fail_ota cco_fail_params;
  rrc_ul_ota_parms_s ul_ota_parms; 
  rrc_log_rrc_ho_from_utran_fail_ota ho_from_utran_fail_params;
}rrc_qsh_ul_failure_ota_u;

typedef enum {
  RRC_OTA_INVALID,
  RRC_OTA_UL_FAILURE,
  RRC_OTA_UL_CPLT,
  RRC_OTA_CU,
  RRC_OTA_MEAS,
  RRC_OTA_ASU,
  RRC_OTA_CHO,
  RRC_OTA_IHO,
  RRC_OTA_MAX
}rrc_qsh_ul_ota_type_e;

typedef struct {
  rrc_qsh_ul_ota_type_e ul_ota_type;
  uint8 msg_id;  
  rrc_qsh_ul_failure_ota_u ul_fail_ota;
}rrc_qsh_ul_failure_ota_param_s;

typedef struct {
  qsh_metric_hdr_s hdr;
  uint16 msg_id;       
  uint32 config; 
}wrrc_qsh_metric_mm_to_rrc_cmd_s;

typedef struct {
  qsh_metric_hdr_s hdr;
  uint16 msg_id;
  uint32 config; 
}wrrc_qsh_metric_rrc_to_mm_cmd_s;

typedef struct {
  qsh_metric_hdr_s hdr;
  uint16 msg_id;
  uint32 config; 
}wrrc_qsh_metric_rr_to_rrc_cmd_s;

typedef struct {
  qsh_metric_hdr_s hdr;
  uint16 msg_id;
  uint32 config; 
}wrrc_qsh_metric_rrc_to_rr_cmd_s;

typedef struct {
  qsh_metric_hdr_s hdr;
  uint16 msg_id;
  uint32 config; 
}wrrc_qsh_metric_rrc_to_l1_cmd_s;

typedef struct {
  qsh_metric_hdr_s hdr;
  uint16 msg_id;
  uint32 config; 
}wrrc_qsh_metric_l1_to_rrc_cmd_s;

typedef struct {
  qsh_metric_hdr_s hdr;
  uint16 msg_id;
  uint32 config; 
}wrrc_qsh_metric_rrc_to_lte_cmd_s;

typedef struct {
  qsh_metric_hdr_s hdr;
  uint16 msg_id;
  uint32 config;
}wrrc_qsh_metric_lte_to_rrc_cmd_s;

typedef struct {
  qsh_metric_hdr_s hdr;
  uint16 msg_id;
  uint32 config; 
}wrrc_qsh_metric_internal_cmd_s;

typedef struct {
  qsh_metric_hdr_s hdr;
  uint16 msg_id;
  uint32 config; 
}wrrc_qsh_metric_llc_cmd_s;

typedef struct {
  qsh_metric_hdr_s hdr;
  uint8 rrc_state;
}wrrc_qsh_metric_rrc_state_history;

typedef enum
{
  WRRC_CONN_REL_CAUSE_NORMAL_EVENT, 
  WRRC_CONN_REL_CAUSE_UNSPECIFIED,
  WRRC_CONN_REL_CAUSE_PRE_EMPTIVE_RELEASE,
  WRRC_CONN_REL_CAUSE_CONGESTION,
  WRRC_CONN_REL_CAUSE_REESTABLISHMENT_REJECT,
  WRRC_CONN_REL_CAUSE_DRCTD_SIGNALING_CONN_REEST,
  WRRC_CONN_REL_CAUSE_USER_INACTIVITY,
  WRRC_CONN_REL_CAUSE_OTHER
}wrrc_conn_rel_cause_e_type;

typedef union
{
  wrrc_conn_rel_cause_e_type      conn_rel_cause;
  sys_rlf_weak_signal_e_type      rlf_cause;
  sys_handover_failure_e_type     ho_fail_type;
}wrrc_call_end_cause_u_type;

typedef struct{
  qsh_metric_hdr_s       hdr;
  sys_call_end_event_e_type   type;
  wrrc_call_end_cause_u_type   cause;
}wrrc_qsh_metric_call_end_info_s_type;

typedef struct{
  qsh_metric_hdr_s       hdr;
  sys_rab_status_e_type mrab_status;
}wrrc_qsh_metric_multi_rab_status_s_type;
#endif

