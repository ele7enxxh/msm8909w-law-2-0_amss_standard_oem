#ifndef CMDBG_H
#define CMDBG_H
/*===========================================================================

     C A L L   M A N A G E R   D E B U G   H E A D E R   F I L E

DESCRIPTION
  This header file contains debug macros and definitions necessary to
  interface with CMDBG.C


Copyright (c) 1991 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmdbg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/13   mh/jvo  Mainline the modem statistics code
07/19/13   jvo     Remove all MDM/MSM Fusion features
03/03/11   gm      Added separate queue processing for M2M indications
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
01/26/11   rm      CMI-4: Inclusion of cm_i.h
11/24/10   xs      Fusion featurization clean up
10/01/10   ak      Merged the Fusion code from Dev branch.
08/11/10   jqi     Remove API featurization between modems and CM/CNE modules.
10/22/09   rm      Adding Modem Statistics changes
04/10/06   pk      Lint fixes
03/15/06   sk      Fixed debug messages to print client type instead of
                   client id.
03/09/06   pk      Added support for generic report
01/06/06   ic      Lint cleanup
11/18/05   jqi     Moved CM_DEBUG to cmidbg.h file to reduce header file
                   dependency.
05/26/05   ic      Removed references to CM_STRING_MAPS_ACTIVE
11/15/04   ic      Lint cleanup with CM_DEBUG on
04/13/04   ic      Featurization changes
04/01/04   ic      Changed cmdbg_print_fav_mode_pref_for_origination() to take
                   5 parameters instead of a pointer to a structure that
                   contains those parameters.
                   cmdbg.c is shared across targets and the structure
                   cm_call_cmd_info_s_type became different for each target.
03/30/04   ws      Made print_fav_mode_pref mode independent.
03/24/04   ic      Changed definition of CM_VERIFY to not return a value
                   (compiler warning fix).
                   Made print_message() global.
02/06/04   ws      Initial jaguar/mainline merge.
11/03/03   ic      Added cmdbg_print_fav_mode_pref_for_origination()
10/23/03   ic      Added cm_ss_cmd_prt(), cm_ss_event_prt(),
                   cm_ss_cmd_err_prt(), cmdbg_ss_info_print()
08/27/03   ic      Changes to support HDR hybrid operation on Cougar.
07/11/03   ic      Baseline for HDR hybrid development for Jaguar.
                   Restored CMDBG_PRT_FUNC so it does not evaluate to no op
                   when CM_DEBUG is defined.
01/21/03   ws      Updated copyright information for 2003
05/05/02   vt      Fixed compile problem with new mm cm.h
03/25/02   ic      Added declaration of cm_call_orig_fail_e_type_string_map
03/06/02   ic      Replaced
                   extern const char* cm_call_cmd_e_string_map[];
                   with
                   extern const char* cm_call_cmd_e_type_string_map[];
                   Added
                   extern const char* cm_inband_cmd_e_type_string_map [];
                   extern const char* cm_inband_cmd_err_e_type_string_map [ ];
02/15/02   ic      Added declaration of cm_call_cmd_e_string_map
02/11/02   ic      Made print_message() global
02/07/02   ic      Under CM_STRING_MAPS_ACTIVE & FEATURE_IS2000_REL_A, added
                   declaration of cm_flash_type_e_type_string_map
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the
                   changes between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32
                   (CM VU in N*2165) that we needed to merge with and changes
                   between those two VUs were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
05/31/01   RC      Compile CM_ERR() to ERR() when CM_DEBUG is not
                   defined.
11/17/00   RC      Added support for System Determination 2.0
08/31/99   SH      Corrected some comments
06/21/99   SH      Enabled CM_DEBUG
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
12/08/98   RC      Changes per second week of I&T.
11/24/98   RC      Initial release.



===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */

#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"
#include "cm_i.h"
#include "cmcall.h"
#include "cmph.h"
#include "cminband.h"
#include "cmss.h"
#include "cmidbg.h"
#include "cmpmprx.h"
#include "mmoc_v.h"
#include "cmmsc.h"

#ifdef CM_DEBUG
#error code not present
#endif
/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/
#ifdef FEATURE_CM_DEBUG_BUFFER
#define CM_DEBUG_BUFFER_SIZE 50
#endif

#define CMDBG_MEM_ALLOC( VAR, TYPE) \
  TYPE* VAR = (TYPE *)cm_mem_malloc(sizeof(TYPE));

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_CM_DEBUG_BUFFER

typedef enum cm_buffer_type_e
{
  CM_BUFF_TYPE_NONE = -1,
  CM_BUFF_PH_CMD_TYPE,
  CM_BUFF_CALL_CMD_TYPE,
  CM_BUFF_SUPS_CMD_TYPE,
  CM_BUFF_SMS_CMD_TYPE,
  CM_BUFF_RPT_TYPE,
  CM_BUFF_MSG_RTR_TYPE,
  CM_BUFF_SIMC_ROUTING_ENTRY_TYPE,
  CM_BUFF_MSC_ROUTING_ENTRY_TYPE,
  CM_BUFF_MMOC_CMD_TYPE,
  CM_BUFF_OP_MODE_CHANGE,
  CM_BUFF_TYPE_MAX
}cm_buffer_type_e_type;

typedef enum cm_dbg_buff_msg_rtr_e
{
  CMDBG_BUFF_MSG_RTR_NONE = -1,
  CM_DBG_BUFF_MSGR_RTR_PM_CFG_UPDATE_IND,
  CMDBG_BUFF_MSG_RTR_MAX
}cm_dbg_buff_msg_rtr_e_type;

typedef struct cm_msg_debug_entry_s
{
  dword                     timestamp;
  cm_buffer_type_e_type     buff_type;
  union cmd_u_type_1
  {
    cm_ph_cmd_e_type                    ph_cmd;
    cm_call_cmd_e_type                  call_cmd;
    cm_sups_cmd_e_type                  sups_cmd;
    cm_sms_cmd_e_type                   sms_cmd;
    cm_name_type                        rpt;
    cm_dbg_buff_msg_rtr_e_type          msg;
    mmoc_cmd_e_type                     mmoc_cmd;
  }info;

  void*                   buff;
} cm_msg_debug_entry_s_type;

typedef struct cm_msg_debug_s
{
   uint8                      debug_buf_idx;
   cm_msg_debug_entry_s_type  cm_msg_debug_buf [CM_DEBUG_BUFFER_SIZE];
}cm_debug_buffer_s_type;

/**============================================================================
*                   CM PHONE COMMANDS                                         *
=============================================================================*/
// CM_PH_CMD_OPRT_MODE
typedef struct cm_ph_cmd_oprt_mode_debug_entry_s
{
  cm_client_id_type             client;
  sys_oprt_mode_e_type          oprt_mode;
}cm_ph_cmd_oprt_mode_debug_entry_s_type;

// CM_PH_CMD_SYS_SEL_PREF
typedef struct cm_ph_cmd_pref_change_debug_entry_s
{
  cm_client_id_type                 client;
  sys_modem_as_id_e_type            cmd_subs;
  cm_mode_pref_e_type               mode_pref;
  cm_pref_term_e_type               pref_term;
  cm_band_pref_e_type               band_pref;
  sys_lte_band_mask_e_type          lte_band_pref;
  cm_band_pref_e_type               tds_band_pref;
  cm_prl_pref_e_type                prl_pref;
  cm_roam_pref_e_type               roam_pref;
  cm_hybr_pref_e_type               hybr_pref;
  cm_srv_domain_pref_e_type         srv_domain_pref;
  cm_network_sel_mode_pref_e_type   network_sel_mode_pref;
  sys_ue_usage_setting_e_type       ue_usage_setting;
  sys_csg_id_type                   csg_id;
  sys_radio_access_tech_e_type      csg_rat;
  sys_voice_domain_pref_e_type      voice_domain_pref;
  cm_lte_disable_cause_e_type       lte_disable_cause;
  sys_plmn_id_s_type                plmn;
  cm_rat_acq_order_pref_s_type      rat_acq_order_pref;
}cm_ph_cmd_pref_change_debug_entry_s_type;

/*
  CM_PH_CMD_SUBSCRIPTION_AVAILABLE
  CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE
  CM_PH_CMD_PERSO_INFO_AVAILABLE
  CM_PH_CMD_ACTIVATE_SUBS
  CM_PH_CMD_DEACTIVATE_SUBS
*/
typedef struct cm_ph_cmd_subs_xx_entry_s
{
  cm_client_id_type                 client;
  uint8                             subs_xx_bitmask;
  /* Bitmask as follows
          0001   ==> CDMA Changed
          0010   ==> GW   Changed
          0100   ==> GW2  Changed
          1000   ==> GW3  Changed
    */
}cm_ph_cmd_subs_xx_entry_s_type;

// CM_PH_CMD_GET_NETWORKS
typedef struct cm_ph_cmd_get_net_entry_s
{
  cm_client_id_type                 client;
  sys_modem_as_id_e_type            cmd_subs;
  cm_network_list_type_e_type       network_list_type;
  cm_mode_pref_e_type               network_type;
  cm_band_pref_e_type               band_pref_lmt;
  sys_lte_band_mask_e_type          lte_band_pref_lmt;
  cm_band_pref_e_type               tds_band_pref_lmt;
}cm_ph_cmd_get_net_entry_s_type;

// CM_PH_CMD_TERMINATE_GET_NETWORKS
typedef struct cm_ph_cmd_term_get_net_entry_s
{
  cm_client_id_type                 client;
  sys_modem_as_id_e_type            cmd_subs;
}cm_ph_cmd_term_get_net_entry_s_type;


// CM_PH_CMD_AVOID_SYS
typedef struct cm_ph_cmd_avoid_sys_entry_s
{
  cm_client_id_type                 client;
  sys_modem_as_id_e_type            cmd_subs;
  cm_ss_avoid_sys_e_type            avoid_type;
  dword                             avoid_time;
}cm_ph_cmd_avoid_sys_entry_s_type;

// CM_PH_CMD_DUAL_STANDBY_PREF
typedef struct cm_ph_cmd_ds_pref_chgd_entry_s
{
  cm_client_id_type                    client;
  sys_modem_dual_standby_pref_e_type   standby_pref;
  sys_modem_as_id_e_type               active_subs;
  sys_modem_as_id_e_type               default_voice_subs;
  sys_modem_as_id_e_type               default_data_subs;
  sys_modem_as_id_e_type               priority_subs;
}cm_ph_cmd_ds_pref_chgd_entry_s_type;

// CM_PH_CMD_PLMN_BLOCKING
typedef struct cm_ph_cmd_plmn_blocking_entry_s
{
  cm_client_id_type                    client;
  sys_modem_as_id_e_type               cmd_subs;
  cm_plmn_blocking_pref_e_type         request_pref;
  sys_plmn_id_s_type                   plmn;
  dword                                blocking_interval;
}cm_ph_cmd_plmn_blocking_entry_s_type;

//CM_PH_CMD_SET_LTE_DSDS_TRM_PRIOIRTY
typedef struct cm_ph_cmd_trm_priority_entry_s
{
  cm_client_id_type                    client;
  sys_modem_as_id_e_type               cmd_subs;
  boolean                         tune_away_req;
}cm_ph_cmd_trm_priority_entry_s_type;

//CM_PH_CMD_RESET_CLNUP_ST_MC
typedef struct cm_ph_cmd_reset_clnup_mc_entry_s
{
  cm_client_id_type                    client;
}cm_ph_cmd_reset_clnup_mc_entry_s_type;

// CM_PH_CMD_SET_SRVCC_CAPABILITY
typedef struct cm_ph_cmd_set_srvcc_cap_entry_s
{
  cm_client_id_type                    client;
  sys_modem_as_id_e_type               asubs_id;
  boolean                              srvcc_capability;
}cm_ph_cmd_set_srvcc_cap_entry_s_type;

// CM_PH_CMD_DATA_PRIORITY
typedef struct cm_ph_cmd_data_pri_entry_s
{
  cm_client_id_type                    client;
  sys_modem_as_id_e_type               asubs_id;
  sys_data_priority_e_type             priority_type;
}cm_ph_cmd_data_pri_entry_s_type;

/**============================================================================
*                                                                  CM CALL  COMMANDS                                                                                   *
=============================================================================*/
// CM_CALL_CMD_ORIG
typedef struct cm_call_cmd_orig_entry_s
{
  cm_client_id_type                    client;
  sys_modem_as_id_e_type               cmd_subs;
  cm_call_id_type                      call_id;
  cm_call_type_e_type                  call_type;
  cm_srv_type_e_type                   srv_type;
}cm_call_cmd_orig_entry_s_type;

// CM_CALL_CMD_ANSWER
// CM_CALL_CMD_END
// CM_CALL_CMD_SETUP_RES
// CM_CALL_CMD_RAB_REESTAB
typedef struct cm_call_cmd_xx_entry_s
{
  cm_client_id_type                    client;
  cm_call_id_type                      call_id;
}cm_call_cmd_xx_entry_s_type;

// CM_CALL_CMD_SUPS
typedef struct cm_call_cmd_sups_entry_s
{
  cm_client_id_type                    client;
  sys_modem_as_id_e_type               cmd_subs;
  cm_call_sups_type_e_type             sups_type;
}cm_call_cmd_sups_entry_s_type;

// CM_CALL_CMD_PS_DATA_AVAILABLE
typedef struct cm_call_cmd_ps_data_avail_entry_s
{
  cm_client_id_type                    client;
}cm_call_cmd_ps_data_avail_entry_s_type;

// CM_CALL_CMD_DATA_SUSPEND
typedef struct cm_call_cmd_data_suspend_entry_s
{
  cm_client_id_type                    client;
  sys_modem_as_id_e_type               cmd_subs;
  boolean                              data_suspend;
}cm_call_cmd_data_suspend_entry_s_type;

// CM_CALL_CMD_LOCAL_HOLD
typedef struct cm_call_cmd_local_hold_entry_s
{
  cm_client_id_type                    client;
  sys_modem_as_id_e_type               cmd_subs;
  cm_call_hold_state_e_type            call_hold_state;
}cm_call_cmd_local_hold_entry_s_type;

/**============================================================================
*                                                                  CM SMS  COMMANDS                                                                                   *
=============================================================================*/

// CM_SMS_CMD_MO_MSG
typedef struct cm_sms_cmd_mo_msg_entry_s
{
  cm_client_id_type                    client;
  boolean                              l2_ack_requested;
  cm_mode_pref_e_type                  mode_pref;
}cm_call_cmd_mo_msg_entry_s_type;

// CM_SMS_CMD_BC_ENABLE
typedef struct cm_sms_cmd_bc_enable_entry_s
{
  cm_client_id_type                    client;
  boolean                              bc_enable;
}cm_sms_cmd_bc_enable_entry_s_type;

/**============================================================================
*                                                                  NAS  REPORTS                                                                                   *
=============================================================================*/
// CM_NETWORK_LIST_CNF
typedef struct cm_rpt_net_list_cnf_entry_s
{
  sys_modem_as_id_e_type               as_id;
  sys_modem_stack_id_e_type            stack_id;
  reg_cm_network_list_status_e_type    status;
  uint32                               length;
}cm_rpt_net_list_cnf_entry_s_type;

// CM_SERVICE_CNF
typedef struct cm_rpt_srv_cnf_entry_s
{
  sys_modem_as_id_e_type                as_id;
  sys_modem_stack_id_e_type             stack_id;
  sys_plmn_service_state_s_type         service_state;
  reg_cm_scan_status_e_type             scan_status;
}cm_rpt_srv_cnf_entry_s_type;

// CM_SERVICE_IND
typedef struct cm_rpt_srv_ind_entry_s
{
  sys_modem_as_id_e_type                as_id;
  sys_modem_stack_id_e_type             stack_id;
  sys_plmn_service_state_s_type         service_state;
}cm_rpt_srv_ind_entry_s_type;

// CM_DATA_SUSPEND_CHGD_F
typedef struct cm_rpt_data_suspend_entry_s
{
  boolean               data_suspend;
  sd_ss_e_type          ss;
}cm_rpt_data_suspend_entry_s_type;

// CM_CAMPED_IND
typedef struct cm_rpt_camped_ind_entry_s
{
  sys_modem_as_id_e_type                 as_id;
  sys_modem_stack_id_e_type              stack_id;
  sys_radio_access_tech_e_type           active_rat;
  sys_plmn_id_s_type                     plmn;
}cm_rpt_camped_ind_entry_s_type;

// CM_MM_PS_DATA_SUSPEND_IND
typedef struct cm_rpt_data_susp_ind_entry_s
{
  sys_modem_as_id_e_type                as_id;
  sys_modem_stack_id_e_type             stack_id;
}cm_rpt_data_susp_ind_entry_s_type;

// CM_CELL_SERVICES_IND
typedef struct cm_rpt_cell_services_entry_s
{
  sys_modem_as_id_e_type                as_id;
  sys_modem_stack_id_e_type             stack_id;
  sys_hs_ind_e_type                     hsdpa_hsupa_support;
  sys_dtm_support_e_type                dtm_support;
  sys_egprs_support_e_type              egprs_support;
}cm_rpt_cell_services_entry_s_type;

// CM_CONN_MODE_SERVICE_STATUS_IND
typedef struct cm_rpt_conn_mode_ind_entry_s
{
  sys_modem_as_id_e_type                as_id;
  sys_modem_stack_id_e_type             stack_id;
  sys_conn_mode_ss_ind_s_type           conn_mode_ss_ind;
}cm_rpt_conn_mode_ind_entry_s_type;

// CM_CELL_SERVICES_IND
typedef struct cm_rpt_cell_services_ind_entry_s
{
  sys_modem_as_id_e_type                as_id;
  sys_modem_stack_id_e_type             stack_id;
  sys_hs_ind_e_type                     hsdpa_hsupa_support;
  sys_dtm_support_e_type                dtm_support;
  sys_egprs_support_e_type              egprs_support;
}cm_rpt_cell_services_ind_entry_s_type;

// CM_PS_DETACH_CNF
// CM_MULTIMODE_SUBS_CHGD_CNF ==> AS_ID & SS will be none
typedef struct cm_rpt_as_id_ss_entry_s
{
  sys_modem_as_id_e_type                as_id;
  sys_modem_stack_id_e_type             stack_id;
}cm_rpt_as_id_ss_entry_s_type;

// CM_STACK_ACT_IND
// CM_STACK_DEACT_IND
typedef struct cm_rpt_stack_act_deact_entry_s
{
  sys_modem_as_id_e_type                     as_id;
  sys_modem_stack_id_e_type                  stack_id;
  cmmsc_state_machine_op_mode_e_type         cmmsc_op_mode;
}cm_rpt_stack_act_deact_entry_s_type;

// CM_SERVICE_STAT_IND
typedef struct cm_rpt_stat_ind_entry_s
{
  sys_modem_as_id_e_type                     as_id;
  sys_modem_stack_id_e_type                  stack_id;
  cmmsc_state_machine_op_mode_e_type         cmmsc_op_mode; // Store MSC op_mode when stack act/deact/stat is received
  sys_plmn_service_state_s_type              service_state;
}cm_rpt_stat_ind_entry_s_type;

/**============================================================================
*                                                                  SD  REPORTS                                                                                                 *
=============================================================================*/
// CM_SRV_IND_INFO_F
typedef struct cm_rpt_srv_ind_info_entry_s
{
  sd_ss_e_type                         ss;
  sys_srv_status_e_type                srv_status;
  sd_mode_e_type                       mode;
  sd_band_e_type                       band;
  sd_chan_type                         chan;
  sd_sid_u_type                        sid;
  sd_roam_e_type                       roam;
  sys_srv_capability_e_type            sys_srv_cap;
  sys_srv_domain_e_type                srv_domain;
  sd_sib8_status                       sib8_available;
  sys_lte_cs_capability_e_type         lte_cs_capability;
  boolean                              is_active;
  boolean                              lte_ims_voice_avail;
  sys_ims_lte_emerg_supp_e_type        lte_ims_emerg_avail;
  sys_extend_srv_info_e_type           extend_srv_info;
  sys_reg_domain_e_type                reg_domain;
}cm_rpt_srv_ind_info_entry_s_type;

// CM_ACQ_FAIL_F
// CM_FULL_SRV_LOST_F
typedef struct cm_rpt_ss_entry_s
{
  sd_ss_e_type                          ss;
}cm_rpt_ss_entry_s_type;

// CM_MMOC_IMS_DEREG_REQ
typedef struct cm_rpt_ims_dereg_entry_s
{
  cm_srv_domain_pref_e_type             srv_domain_pref;
}cm_rpt_ims_dereg_entry_s_type;

// CM_OK_TO_ORIG_F
typedef struct cm_rpt_ok_to_orig_entry_s
{
  sd_ss_e_type                          ss;
  sd_si_ok_to_orig_s_type               si_ok_to_orig;
}cm_rpt_ok_to_orig_entry_s_type;

// CM_LTE_DO_IRAT_F  ==> buff will be NULL for it
// CM_LTE_1X_IRAT_F  ==> buff will be NULL for it
// CM_IRAT_COMPLETE ==> buff will be NULL for it

// CM_SUSPEND_SS_F
typedef struct cm_rpt_suspend_ss_entry_s
{
  sd_ss_e_type                          ss;
  boolean                               is_suspend;
}cm_rpt_suspend_ss_entry_s_type;

// CM_HYBR_BSR_TO_HDR_F
typedef struct cm_rpt_hybr_bsr_hdr_entry_s
{
  sd_hybr_bsr_to_hdr_s_type     hybr_bsr_to_hdr_status;
}cm_rpt_hybr_bsr_hdr_entry_s_type;

// CM_BSR_STAT_CHGD_F
typedef struct cm_rpt_bsr_stat_chgd_entry_s
{
  boolean                       bsr_status;
}cm_rpt_bsr_stat_chgd_entry_s_type;

/**============================================================================
*                                                                  MMGSDI  REPORTS                                                                                          *
=============================================================================*/
// CM_MMGSDI_CARD_RPT
typedef struct cm_rpt_mmgsdi_card_rpt_entry_s
{
  mmgsdi_events_enum_type  evt;
  mmgsdi_client_id_type    client_id;
  mmgsdi_session_id_type   session_id;
  union {
    mmgsdi_refresh_evt_info_type                        refresh;
    mmgsdi_card_inserted_evt_info_type                  card_inserted;
    mmgsdi_session_changed_evt_info_type                session_changed;
    mmgsdi_subscription_ready_evt_info_type             subscription_ready;
    mmgsdi_card_err_evt_info_type                       card_error;
    mmgsdi_card_removed_evt_info_type                   card_removed;
    mmgsdi_perso_evt_info_type                          perso;
    mmgsdi_session_illegal_subscription_evt_info_type   illegal_session;
  } data;
}cm_rpt_mmgsdi_card_rpt_entry_s_type;

// CM_MMGSDI_SESSION_OPEN_RPT
typedef struct cm_rpt_mmgsdi_session_open_entry_s
{
  mmgsdi_session_id_type   session_id;
}cm_rpt_mmgsdi_session_open_entry_s_type;

/**============================================================================
*              CMMSC  ENTRY                                                   *
=============================================================================*/
/**
  * SIMCOORD Entry
*/
typedef struct cm_simc_routing_entry_s
{
  int                               idx;
}cm_simc_routing_entry_s_type;

typedef struct cm_msc_routing_entry_s
{
  sys_modem_as_id_e_type            as_id;
  cmmsc_routing_table_e_type        tbl_id;
  int                               idx;
}cm_msc_routing_entry_s_type;

typedef struct cm_op_mode_change_entry_s
{
  sys_modem_as_id_e_type             as_id;
  cmmsc_state_machine_op_mode_e_type old_op_mode;
  cmmsc_state_machine_op_mode_e_type new_op_mode;
}cm_op_mode_change_entry_s_type;

/**============================================================================
*                  Message Router Messages                                    *
=============================================================================*/
// POLICYMAN_CFG_UPDATE_MSIM_IND
typedef struct cm_pm_policy_chg_entry_s
{
  cm_policy_config_s_type      policy;
}cm_pm_policy_chg_entry_s_type;

/**============================================================================
*                  CM => MMOC Msgs                                    *
=============================================================================*/
// MMOC_CMD_PROT_GEN_CMD
typedef struct cm_mmoc_prot_gen_cmd_entry_s
{
  sd_ss_e_type                         ss;
  sd_ss_pref_reas_e_type               pref_reas;
  sd_ss_orig_mode_e_type               orig_mode;
  sd_ss_mode_pref_e_type               mode_pref;
  sd_ss_band_pref_e_type               band_pref;
  sys_lte_band_mask_e_type             lte_band_pref;
  sd_ss_band_pref_e_type               tds_band_pref;
  sd_ss_roam_pref_e_type               roam_pref;
  sd_ss_hybr_pref_e_type               hybr_pref;
  sd_ss_avoid_sys_e_type               avoid_type;
  dword                                avoid_time;
  sd_ss_srv_domain_pref_e_type         domain_pref;
  sd_ss_pref_update_reason_e_type      pref_update_reas;
  sys_addtl_action_s_type              addl_action;
  sd_ss_mode_pref_e_type               user_mode_pref;
  sd_ss_pref_camp_mode_e_type          camp_mode_pref;
  sys_voice_domain_pref_e_type         voice_domain_pref;
}cm_mmoc_prot_gen_cmd_entry_s_type;

typedef struct cmdbg_subs_data_s
{
  sys_modem_as_id_e_type          as_id;
  boolean                         is_subs_avail;
  boolean                         is_perso_locked;
  mmgsdi_session_type_enum_type   session_type;
  boolean                         is_subsc_chg;
  sd_ss_e_type                    ss;
  sd_ss_orig_mode_e_type          orig_mode;
  sd_ss_mode_pref_e_type          mode_pref;
  sd_ss_band_pref_e_type          band_pref;
  sd_ss_roam_pref_e_type          roam_pref;
  sd_ss_hybr_pref_e_type          hybr_pref;
  sys_lte_band_mask_e_type        lte_band_pref;
  sd_ss_band_pref_e_type          tds_band_pref;
  subs_capability_e_type          subs_capability;
  sd_ss_srv_domain_pref_e_type    srv_domain_pref;
  sys_ue_mode_e_type              curr_ue_mode;
  boolean                         is_ue_mode_substate_srlte;
}cmdbg_subs_data_s_type;

// MMOC_CMD_SUBSCRIPTION_CHGD
typedef struct cm_mmoc_subsc_chgd_cmd_entry_s
{
  mmoc_subsc_chg_e_type                chg_type;
  prot_subsc_chg_e_type                prot_subsc_chg;
  boolean                              hybr_gw_subs_chg;
  boolean                              hybr3_gw_subs_chg;
  byte                                 nam;
  uint8                                active_ss;
  sys_modem_device_mode_e_type         device_mode;
  cmdbg_subs_data_s_type               cdma_subs;
  cmdbg_subs_data_s_type               gw_subs;
  cmdbg_subs_data_s_type               gw_hybr_subs;
  cmdbg_subs_data_s_type               gw3_hybr_subs;
}cm_mmoc_subsc_chgd_cmd_entry_s_type;

#endif
/*lint -save -e656 -e641
** Turn off enum to int / operationuses compatible enums
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

extern void cm_call_state_prt( dword enum_val );
#define CM_CALL_STATE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_call_state_prt, enum_val )


extern void cm_call_type_prt( dword  enum_val );
#define CM_CALL_TYPE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_call_type_prt, enum_val )


extern void cm_call_end_prt( cm_call_end_e_type  enum_val );
#define CM_CALL_END_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_call_end_prt, enum_val )


extern void cm_digit_mode_prt( dword enum_val );
#define CM_DIGIT_MODE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_digit_mode_prt, enum_val )


extern void cm_call_cmd_prt( dword enum_val );
#define CM_CALL_CMD_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_call_cmd_prt, enum_val )


extern void cm_call_event_prt( dword enum_val );
#define CM_CALL_EVENT_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_call_event_prt, enum_val )


extern void cm_call_cmd_err_prt( dword enum_val );
#define CM_CALL_CMD_ERR_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_call_cmd_err_prt, enum_val )


extern void cm_srv_state_prt( dword enum_val );
#define CM_SRV_STATE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_srv_state_prt, enum_val )


extern void cm_oprt_mode_prt( dword enum_val );
#define CM_OPRT_MODE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_oprt_mode_prt, enum_val )


extern void cm_mode_pref_prt( dword enum_val );
#define CM_MODE_PREF_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_mode_pref_prt, enum_val )


extern void cm_mode_term_prt( dword enum_val );
#define CM_MODE_TERM_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_mode_term_prt, enum_val )

extern void cm_band_pref_prt( dword enum_val );
#define CM_BAND_PREF_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_band_pref_prt, enum_val )

extern void cm_roam_pref_prt( dword enum_val );
#define CM_ROAM_PREF_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_roam_pref_prt, enum_val )

extern void cm_answer_voice_prt( dword enum_val );
#define CM_ANSWER_VOICE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_answer_voice_prt, enum_val )


extern void cm_nam_prt( dword enum_val );
#define CM_NAM_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_nam_prt, enum_val )


extern void cm_cdma_lock_mode_prt( dword enum_val );
#define CM_CDMA_LOCK_MODE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_cdma_lock_mode_prt, enum_val )


extern void cm_ph_cmd_prt( dword enum_val );
#define CM_PH_CMD_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_ph_cmd_prt, enum_val )

extern void cm_stats_cmd_prt( dword enum_val );
#define CM_STATS_CMD_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_stats_cmd_prt, enum_val )

extern void cm_ss_cmd_prt( dword enum_val );
#define CM_SS_CMD_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_ss_cmd_prt, enum_val );

extern void cm_ph_event_prt( dword enum_val );
#define CM_PH_EVENT_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_ph_event_prt, enum_val )

extern void cm_ss_event_prt( dword enum_val );
#define CM_SS_EVENT_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_ss_event_prt, enum_val )

extern void cm_ph_cmd_err_prt( dword enum_val );
#define CM_PH_CMD_ERR_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_ph_cmd_err_prt, enum_val )

extern void cm_stats_cmd_err_prt( dword enum_val );
#define CM_STATS_CMD_ERR_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_stats_cmd_err_prt, enum_val )

extern void cm_ss_cmd_err_prt( dword enum_val );
#define CM_SS_CMD_ERR_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_ss_cmd_err_prt, enum_val )

extern void cm_in_use_prt( dword enum_val );
#define CM_IN_USE_PRT( enum_val )  \
    CMDBG_PRT_FUNC( cm_in_use_prt, enum_val )


extern void cm_client_type_prt( cm_client_type_e_type  client_id );
#define CM_CLIENT_TYPE_PRT( client )  \
    CMDBG_PRT_FUNC( cm_client_type_prt, client )

extern void  mc_cmd_prt( dword enum_val );
#define CM_MC_CMD_PRT( enum_val )  \
    CMDBG_PRT_FUNC( mc_cmd_prt, enum_val )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to create a bit array of arbitrary size. The first argument
** (bit_arr) is the name of the array. The second argument (bit_size)
** is the bit size of the array.
*/
#define BIT_ARR_T( bit_arr, bit_size )             \
    byte _##bit_arr[1 + (bit_size)/sizeof(byte)];  \
    bit_arr_s_type bit_arr = { _##bit_arr, bit_size }


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/**--------------------------------------------------------------------------
** Externs
** --------------------------------------------------------------------------
*/



/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

extern void cmdbg_print_message( const char *string, ... );
extern void cmdbg_print_call(const cmcall_s_type *call_ptr);
extern void cmdbg_print_cm_cmd(const cm_cmd_type  *cmd_ptr );
extern void cmdbg_stringmap_verify( void );

extern void cmdbg_call_info_print(const cm_mm_call_info_s_type *info_ptr);
extern void cmdbg_ph_info_print(const cm_ph_info_s_type  *ph_info_ptr);
extern void cmdbg_ss_info_print( const cm_mm_ss_info_s_type   *ss_info_ptr);
extern void cmdbg_print_cm_event( cm_cmd_type_e_type  cmd_type, word event,
  const void *info_ptr);
extern void cmdbg_print_cm_cmderr( cm_cmd_type_e_type  cmd_type,
  word cmd_err, const void *cmd_ptr);
extern void cmdbg_print_sd_rpt(const cm_sd_rpt_u_type *sd_rpt_ptr );
extern void cmdbg_print_generic_rpt(const cm_generic_rpt_s_type *generic_rpt_ptr );


#define CMDBG_PRINT_CM_CMD_FUNC( cmd_ptr ) \
  cmdbg_print_cm_cmd( cmd_ptr )
#define CMDBG_PRINT_CM_EVENT_FUNC( cmd_type, event, info_ptr ) \
  cmdbg_print_cm_event( cmd_type, event, info_ptr)
#define CMDBG_PRINT_CM_CMDERR_FUNC( cmd_type, cmd_err, cmd_ptr ) \
  cmdbg_print_cm_cmderr( cmd_type, cmd_err, cmd_ptr )
#define CMDBG_PRINT_SD_RPT_FUNC( sd_rpt_ptr ) \
  cmdbg_print_sd_rpt( sd_rpt_ptr )
#define CMDBG_PRINT_GENERIC_RPT_FUNC( generic_rpt_ptr ) \
  cmdbg_print_generic_rpt( generic_rpt_ptr )

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

extern void cmdbg_print_mc_cmd(const mc_msg_type *mc_cmd_ptr );
extern void cmdbg_print_mc_rpt(const cm_mc_rpt_type *mc_rpt_ptr );

#define CMDBG_PRINT_MC_CMD_FUNC( mc_cmd_ptr ) \
  cmdbg_print_mc_cmd( mc_cmd_ptr )
#define CMDBG_PRINT_MC_RPT_FUNC( mc_rpt_ptr ) \
  cmdbg_print_mc_rpt( mc_rpt_ptr )

#endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */

#ifdef CM_GW_SUPPORTED

extern void cmdbg_print_gw_rpt( const cm_rpt_type *gw_rpt_ptr );

#define CMDBG_PRINT_GW_RPT_FUNC( gw_rpt_ptr ) \
  cmdbg_print_gw_rpt( gw_rpt_ptr )

#endif /* FEATURE_WCDMA || FEATURE_GSM */

extern void cmdbg_print_fav_mode_pref_for_origination(

  sys_modem_as_id_e_type         asubs_id,
  /* requested asubs_id
  */
  
  cm_call_type_e_type            call_type,
    /* requested call type
    */

  cm_srv_type_e_type             srv_type,
    /* requested service type
    */

  cm_mode_pref_e_type            mode_pref,
    /* mode preference at the time call origination request was made
    */

  cm_mode_pref_e_type            favored_mode_pref,
    /* computed favored mode preference for the call
    */

  cm_hybr_pref_e_type            favored_hybr_pref
    /* computed favored hybrid preference for the call
    */

);

#ifdef FEATURE_CM_DEBUG_BUFFER

/*===========================================================================

FUNCTION cmdbg_ptr

DESCRIPTION
  Return a pointer to the one and only CM debug buffer

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_debug_buffer_s_type  *cmdbg_ptr( void );


/*===========================================================================

FUNCTION cmdbg_init

DESCRIPTION
  Initialize the CM Debug Buffer

  This function must be called before any CM submodule init, 
  in order to capture the commands

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_init( void );

/*===========================================================================

FUNCTION cmdbg_welcome_new_entry

DESCRIPTION
  Adjust debug buffer for new entry of debug buffer

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_welcome_new_entry();


/*===========================================================================

FUNCTION cmdbg_add_to_dbg_buffer

DESCRIPTION
  Add command/report/routing entry to Debug Buffer 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_to_dbg_buffer(
   cm_buffer_type_e_type buff_type,
   /* Type of Buffer to be used */

  uint16                event_type,
  /* Specific cmd/report inside buffer mentioned above */
   
   void                  *buff
  /* Actual payload */
);


/*===========================================================================

FUNCTION cmdbg_add_new_entry

DESCRIPTION
  Adjust debug buffer for new entry of debug buffer

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_new_entry(
    cm_buffer_type_e_type buff_type,
     /* Type of Buffer to be used */
  
    uint16                event_type,
    /* Specific cmd/report inside buffer mentioned above */
     
     void                *buff
    /* Actual payload */
);

/*===========================================================================

FUNCTION cmdbg_add_subs_cmd_to_buffer

DESCRIPTION
  Add subs command to buffer

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void                                 cmdbg_add_subs_cmd_to_buffer
(
        mmoc_subsc_chg_e_type                chg_type,
             /* Subscription change type.
             */
        prot_subsc_chg_e_type                prot_subsc_chg,

        boolean                              hybr_gw_subs_chg,

        boolean                              hybr3_gw_subs_chg,

        byte                                 nam,

        const mmoc_subs_data_s_type          *cdma_subs_ptr,
        /* CDMA subscription info */

        const mmoc_subs_data_s_type          *gw_subs_ptr,
           /* GW subscription info */

        const mmoc_subs_data_s_type          *gw_hybr_subs_ptr,
           /* GW HYBR subscription info */

        const mmoc_subs_data_s_type          *gw3_hybr_subs_ptr ,
           /* GW HYBR subscription info */
        uint8                                active_ss,

        sys_modem_device_mode_e_type         device_mode
);

#endif
/*lint -restore */
#endif /* CMDBG_H */





