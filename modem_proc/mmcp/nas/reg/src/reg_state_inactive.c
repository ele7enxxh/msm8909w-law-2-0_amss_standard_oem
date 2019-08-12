/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state_inactive.c_v   1.2   20 May 2002 11:47:42   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_state_inactive.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/20/02   kwa     Added output_name function.
08/06/02   kwa     Updated SIM state when cm_sim_available_req is received.
09/19/02   kwa     Added function to process the cm_act_req message.
05/21/03   kwa     Restructured code to eliminate lint warnings.
                   Modified reg_state_p_type process_cm_act_req to pass a
                   const pointer to eliminate a lint warning.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added support for band preferences.
11/19/03   kwa     Removed obsolete function for retrieving the state name.
03/17/04   kwa     Added call to reg_mode_initialize_equivalent_plmn_list in
                   process_cm_sim_available_req.
05/17/04   kwa     Added function to process the mmr_camped_ind message.
08/17/04   kwa     Removed call to reg_state_update_sim_state from
                   process_cm_sim_available_req.
02/22/05   up      Updated for Cingular ENS feature.
                   Updated state table to add new function pointer for FCN IND.
                   Updated function process_cm_sim_available_req.
09/19/05   up      Updated state table for new primitive CM_HPLMN_SEARCH_REQ.
10/27/05   up      Send CM_SIM_AVAILABLE_CNF even when SIM access failed.
11/07/05   ajt     Added handling of MMR_EMERGENCY_NUM_LIST_IND in inactive state.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "msg.h"
#include "reg_mode.h"
#include "reg_send.h"
#include "reg_sim_v.h"
#include "reg_state.h"
#include "reg_timers.h"
#include "mm_v.h"

#include "reg_nv.h"
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern sys_mode_pref_e_type  reg_sim_mode_pref_sim[MAX_NAS_STACKS];
#define  reg_sim_mode_pref reg_sim_mode_pref_sim[reg_as_id]

extern boolean reg_need_to_process_sim_avail_req_sim[MAX_NAS_STACKS];
#define reg_need_to_process_sim_avail_req reg_need_to_process_sim_avail_req_sim[reg_as_id]

extern boolean reg_waiting_for_open_session_cnf_sim[MAX_NAS_STACKS];
#define reg_waiting_for_open_session_cnf reg_waiting_for_open_session_cnf_sim[reg_as_id]

#else
extern boolean reg_need_to_process_sim_avail_req;
extern boolean reg_waiting_for_open_session_cnf;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define reg_state_is_pwr_on_manual_plmn_selection reg_state_is_pwr_on_manual_plmn_selection_sim[reg_as_id]
#endif


/*==============================================================================

FUNCTION NAME

  process_cm_sim_available_req

==============================================================================*/

static reg_state_p_type process_cm_sim_available_req
(
  cm_sim_available_req_s_type* msg_p
)
{
  (void) msg_p;

#ifdef FEATURE_DUAL_SIM
  reg_sim_mode_pref = msg_p->mode_pref;
  reg_nv_context_id = msg_p->nv_context_id;

  rex_enter_crit_sect(&reg_sim_crit_sect);
  if((reg_sim_mmgsdi_info_sim[reg_as_id].session_type != msg_p->session_type) &&
      (reg_sim_session_matching_with_other_sub(msg_p->session_type)))
      //(reg_sim_mmgsdi_info_sim[REG_OTHER_SUB(reg_as_id)].session_type == msg_p->session_type))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Swapping MMGSDI info");
    nas_swap_mmgsdi_info((mm_as_id_e_type)reg_as_id, reg_sim_mmgsdi_info_sim,reg_waiting_for_open_session_cnf_sim,msg_p->session_type);
  }
  /* Unexpected but try to recover */ 
  else if(reg_sim_mmgsdi_info_sim[reg_as_id].session_type != msg_p->session_type)
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= Unexpected session type", msg_p->session_type);
    reg_sim_mmgsdi_info.session_type = msg_p->session_type;
    reg_sim_mmgsdi_info.session_id = 0;
    reg_sim_reset_card();
  }
  rex_leave_crit_sect(&reg_sim_crit_sect);

#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  MSG_HIGH_DS_1(REG_SUB, "=REG= Overall Feature: %d", reg_sub_config);
#endif

  if((reg_sim_mmgsdi_info.client_id == 0) || (reg_waiting_for_open_session_cnf))
  {
    MSG_HIGH_DS_3(REG_SUB, "=REG= Waiting for MMGSDI open session response %d/client reg response, client:H:0x%x L:0x%x",
                  reg_waiting_for_open_session_cnf, (dword)(reg_sim_mmgsdi_info.client_id >> 32), 
                  (dword)(reg_sim_mmgsdi_info.client_id & 0xFFFFFFFF));
    reg_need_to_process_sim_avail_req = TRUE;
    reg_send_mmr_sim_available_req
    (
#if defined FEATURE_DUAL_SIM
      msg_p->mode_pref,
      reg_sim_mmgsdi_info.client_id,
      msg_p->session_type,
      reg_nv_context_id
#else
      reg_sim_mmgsdi_info.client_id
#endif
    );

  }
  else if (reg_sim_mmgsdi_info.session_id == 0)
  {
    if (!reg_sim_open_session())
    {
      reg_send_cm_sim_available_cnf(reg_state_cm_transaction_id_get());
      MSG_ERROR_DS_0( REG_SUB, "=REG= MMGSDI OPEN SESSION failed!!!");
    }
    else
    {
      reg_need_to_process_sim_avail_req = TRUE;
      reg_send_mmr_sim_available_req
      (
#if defined FEATURE_DUAL_SIM
        msg_p->mode_pref,
        reg_sim_mmgsdi_info.client_id,
        msg_p->session_type,
        reg_nv_context_id
#else
        reg_sim_mmgsdi_info.client_id
#endif
      );
    }
  }
  else
  {
    if (reg_sim_load_card_mode())
    {
      /*
      ** Read the equivalent PLMN list from NVRAM.
      */
      reg_mode_initialize_equivalent_plmn_list
      (
  #ifdef FEATURE_ENHANCED_NW_SELECTION
        FALSE
  #endif
      );

#ifdef FEATURE_LTE
      reg_send_eq_plmn_change_ind();
      reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_FPLMN);
      reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_GPRS_FPLMN);
#endif
#ifdef FEATURE_FEMTO_CSG
      reg_state_power_on_csg_search = TRUE;
#endif 

      reg_mode_t3245_efnas_config_behavior();
#ifdef FEATURE_TDSCDMA
      reg_timers_stop_validate_sim_timer();
#endif
      /* Populate enhanced hplmn search tbl */
      reg_mode_enhanced_hplmn_srch_init();
      reg_send_mmr_sim_available_req
      (
#if defined FEATURE_DUAL_SIM
        msg_p->mode_pref,
        reg_sim_mmgsdi_info.client_id,
        msg_p->session_type,
        reg_nv_context_id
#else
        reg_sim_mmgsdi_info.client_id
#endif
      );
    }
    else
    {
      reg_send_cm_sim_available_cnf(reg_state_cm_transaction_id_get());
    }
  }

  if (mm_nv_nas_rel_compliance > NV_NAS_COMPLIANCE_REL6)
  {
    reg_state_is_pwr_on_manual_plmn_selection = TRUE;
  }

  return (&reg_state_inactive);
}


/*==============================================================================

FUNCTION NAME

  process_cm_sim_not_available_req

==============================================================================*/

static reg_state_p_type process_cm_sim_not_available_req
(
  cm_sim_not_available_req_s_type* msg_p
)
{
  (void) msg_p;

  reg_sim_reset_card();

  reg_send_mmr_sim_not_available_req();

  return (&reg_state_inactive);
}


/*==============================================================================

FUNCTION NAME

  process_mmr_stop_mode_cnf

==============================================================================*/

static reg_state_p_type process_mmr_stop_mode_cnf
(
  mmr_stop_mode_cnf_s_type* msg_p
)
{
  (void) msg_p;

  reg_send_cm_stop_mode_cnf
  (
    reg_state_cm_transaction_id_get()
  );

  return (&reg_state_inactive);
}


/*==============================================================================

FUNCTION NAME

  process_mmr_ph_status_change_cnf

==============================================================================*/

static reg_state_p_type process_mmr_ph_status_change_cnf
(
  mmr_ph_status_change_cnf_s_type* msg_p
)
{
  (void) msg_p;
  
  reg_send_cm_ph_status_chgd_cnf
  (
    reg_state_cm_transaction_id_get(),
    SYS_PH_STATUS_CHGD_SUCCESS
  );
  
  return (&reg_state_inactive);
}


/*==============================================================================

FUNCTION NAME

  process_mmr_sim_available_cnf

==============================================================================*/

static reg_state_p_type process_mmr_sim_available_cnf
(
  mmr_sim_available_cnf_s_type* msg_p
)
{
  (void) msg_p;

  reg_send_cm_sim_available_cnf
  (
    reg_state_cm_transaction_id_get()
  );

  return (&reg_state_inactive);
}


/*==============================================================================

FUNCTION NAME

  process_mmr_sim_not_available_cnf

==============================================================================*/

static reg_state_p_type process_mmr_sim_not_available_cnf
(
  mmr_sim_not_available_cnf_s_type* msg_p
)
{
  (void) msg_p;

  reg_send_cm_sim_not_available_cnf
  (
    reg_state_cm_transaction_id_get()
  );

  return (&reg_state_inactive);
}

/*==============================================================================

FUNCTION NAME

  process_hplmn_search_timer_expiration

==============================================================================*/

static reg_state_p_type process_hplmn_search_timer_expiration( void )
{ 
  reg_state_p_type next_state_p = &reg_state_inactive;

  /* Timer running in Inactive state means Deep Sleep Alt Scan. Restart */
  switch(reg_mode_network_selection_mode_get())
  {
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
      reg_timers_start_hplmn_search_timer_10sec();
      break;

    default:
      break;
  }
  return next_state_p;
}


/*==============================================================================

FUNCTION NAME

  process_cm_set_hplmn_irat_timer

==============================================================================*/
static reg_state_p_type process_cm_set_hplmn_irat_timer
(
  const cm_set_hplmn_irat_search_timer_req_s_type *msg_p
)
{
  
  uint32 timer_val = msg_p->timer_value;

  MSG_HIGH_DS_1(REG_SUB, "=REG= CM sent HPLMN IRAT timer  - %lu(milli-sec)", timer_val);   
  if(timer_val == 0)
  {
    timer_val = REG_TIMERS_MAX_TIMER_VALUE;    
  }

  reg_nv_hplmn_irat_search_timer_set(timer_val);

  return (&reg_state_inactive);
}


/*==============================================================================

FUNCTION NAME

  number

==============================================================================*/

static reg_state_e_type number ( void )
{
  return REG_STATE_INACTIVE;
}


/*==============================================================================

FUNCTION NAME

  output_name

==============================================================================*/

static void output_name ( void )
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_INACTIVE");
}


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

reg_state_s_type reg_state_inactive = 
{
  reg_state_process_cm_network_list_req,
  reg_state_process_cm_service_req,
  process_cm_sim_available_req,
  process_cm_sim_not_available_req,
  reg_state_process_cm_stop_mode_req,
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
  reg_state_ignore_hplmn_search_request,
#endif
  reg_state_process_cm_ph_status_chgd_req,
#ifdef FEATURE_LTE
  reg_state_unexpected_cm_set_drx_req,
#endif 
  reg_state_unexpected_mmr_camped_ind,
  reg_state_process_mmr_attach_failed_ind,
  reg_state_unexpected_mmr_mm_information_ind,
  reg_state_unexpected_mmr_plmn_search_cnf,
  reg_state_unexpected_mmr_reg_cnf,
  reg_state_unexpected_mmr_service_ind,
  process_mmr_sim_available_cnf,
  process_mmr_sim_not_available_cnf,
  process_mmr_stop_mode_cnf,
  process_hplmn_search_timer_expiration,
  reg_state_process_mmr_emergency_num_list_ind,
  reg_state_unexpected_sim_refresh_fcn_ind,
  process_mmr_ph_status_change_cnf,
  reg_state_unexpected_mmr_reg_reject_ind,
  reg_state_unexpected_hplmn_irat_search_timer_expiration,
  process_cm_set_hplmn_irat_timer,
  reg_state_t3245_exp_clean_up,
#ifdef FEATURE_LTE
  reg_state_process_backoff_fplmn_timer_expiration,
  reg_state_process_cm_block_plmn_req,
  reg_state_ignore_update_lte_cap_timer_expiration,
#endif
#ifdef FEATURE_SGLTE
  reg_state_unexpected_sglte_ps_timer_expiry,
#endif
  reg_state_unexpected_lte_irat_search_timer_expiration,
#ifdef FEATURE_FEMTO_CSG
  reg_state_process_unexpected_mmr_csg_list_read_complete,
#endif 
  reg_state_unexpected_mmr_reg_req_ignored_ind,
  number,
  output_name
};

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/
