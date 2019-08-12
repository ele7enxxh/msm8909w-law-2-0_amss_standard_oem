/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state_registering.c_v   1.2   20 May 2002 11:48:12   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_state_registering.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/20/02   kwa     Added output_name function.
09/19/02   kwa     Added function to process the cm_act_req message.
05/21/03   kwa     Restructured code to eliminate lint warnings.
06/04/03   kwa     Removed one remaining lint warning.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added support for band preferences.
11/19/03   kwa     Removed obsolete function for retrieving the state name.
02/04/04   kwa     Added support for PS follow on request.
05/17/04   kwa     Added function to process the mmr_camped_ind message.
07/19/04   kwa     Removed service_state parameter from call to
                   reg_send_cm_camped_ind.
07/26/04   kwa     Added active_rat and active_band parameters to call to
                   reg_send_cm_camped_ind.
02/22/05   up      Updated for Cingular ENS feature.
                   Updated state table to add new function pointer for FCN IND.
09/19/05   up      Updated state table for new primitive CM_HPLMN_SEARCH_REQ.
11/07/05   ajt     Added handling of MMR_EMERGENCY_NUM_LIST_IND in registering state.
04/05/06   sn      Added function process_hplmn_search_timer_expiration to
                   restart 10 sec timer.
07/03/06   sn      Updated process_mmr_camped_ind to update active rat 
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

#include "reg_timers.h"
#include "reg_nv.h"
#include "err.h"
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  process_mmr_camped_ind

==============================================================================*/

reg_state_p_type process_mmr_camped_ind
(
  mmr_camped_ind_s_type* msg_p
)
{
  (void) msg_p;
#ifdef FEATURE_TDSCDMA
  if(msg_p->policy_change_pending == TRUE)
  {
    reg_nv_set_pm_chg_pending_plmn_rat(msg_p->plmn, msg_p->active_rat);
  }
#endif 
  reg_send_cm_camped_ind
  (
#ifndef FEATURE_GSM_BAND_PREF
    msg_p->active_rat
#else
    msg_p->active_rat,
    msg_p->active_band
#endif
    ,msg_p->plmn
    ,msg_p->lac_id
  );

  reg_state_active_rat_set(msg_p->active_rat);

  reg_state_camped_ind_received_set(TRUE);
  reg_mode_scan_scope_set(SYS_SCAN_SCOPE_FULL_BAND);   
  return (&reg_state_registering);
}


/*==============================================================================

FUNCTION NAME

  process_mmr_mm_information_ind

==============================================================================*/

static reg_state_p_type process_mmr_mm_information_ind
(
  const mmr_mm_information_ind_s_type* msg_p
)
{
  reg_send_cm_mm_information_ind(&(msg_p->info));

  return (&reg_state_registering);
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

  reg_mode_start_plmn_selection
  (
    reg_mode_network_selection_mode_get(),
    reg_mode_mode_pref_get(),
    reg_mode_cs_for_request_get(),
    reg_mode_ps_for_request_get(),
    reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
    ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
    ,reg_mode_rat_pri_list_get()
#endif
    ,REG_CM_SERVICE_REQ_NORMAL
#ifdef FEATURE_LTE
    ,reg_mode_sms_only_get(),
     0
#endif 
  );

  return (&reg_state_registering);
}

/*==============================================================================

FUNCTION NAME

  process_cm_network_list_req

==============================================================================*/
static reg_state_p_type process_cm_network_list_req
(
  const cm_network_list_req_s_type* msg_p
)
{
#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
    sys_found_plmn_list_u_type *available_list = NULL;
  if(msg_p->list_search_type == REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND)
  {
    /* Immediately respond CM_NETWORK_LIST_CNF with SEARCH FAILED */
#ifdef FEATURE_MODEM_HEAP
    available_list = (sys_found_plmn_list_u_type *)modem_mem_alloc(sizeof(sys_found_plmn_list_u_type), MODEM_MEM_CLIENT_NAS);
#else
    available_list = (sys_found_plmn_list_u_type *)gs_alloc(sizeof(sys_found_plmn_list_u_type));
#endif
    reg_check_for_null_ptr((void*)available_list );
    memset(available_list,0x00,sizeof(sys_found_plmn_list_u_type));
    reg_send_cm_network_list_cnf
    (
      reg_state_cm_transaction_id_get(),
      reg_sim_read_rplmn(reg_mode_req_service_domain_get()),
      available_list,
      FALSE /* Search completed FALSE */
    );
#ifndef FEATURE_MODEM_HEAP
   gs_free(available_list);
#else
   modem_mem_free(available_list, MODEM_MEM_CLIENT_NAS);
#endif
    available_list = NULL;
    return &reg_state_registering;
  }
  else
#endif
  {
    return reg_state_process_cm_network_list_req(msg_p);
  }
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

  return (&reg_state_registering);
}


/*==============================================================================

FUNCTION NAME

  number

==============================================================================*/

static reg_state_e_type number ( void )
{
  return REG_STATE_REGISTERING;
}


/*==============================================================================

FUNCTION NAME

  output_name

==============================================================================*/

static void output_name ( void )
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_REGISTERING");
}


/*==============================================================================

FUNCTION NAME

  process_hplmn_search_timer_expiration

==============================================================================*/

static reg_state_p_type process_hplmn_search_timer_expiration( void )
{
  reg_state_p_type next_state_p = &reg_state_registering;

  /* Starting 10 sec timer, so that the timer is running when registration completes. */
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

#ifdef FEATURE_SGLTE
static reg_state_p_type process_sglte_ps_timer_expiry(void)
{
  sys_modem_as_id_e_type reg_other_as_id;
  reg_state_p_type next_state_p;
  reg_other_as_id = (sys_modem_as_id_e_type)((reg_as_id == reg_sub_id) ? REG_SGLTE_STACK_2 : reg_sub_id);
  if( REG_SGLTE_DUAL_STACK &&
      reg_state_get_service_status_per_subs(reg_other_as_id) == SYS_SRV_STATUS_SRV && 
      reg_state_get_service_domain_per_subs(reg_other_as_id) == SYS_SRV_DOMAIN_CS_ONLY &&
      reg_state_get_plmn_service_capability(reg_other_as_id) == SYS_SRV_CAPABILITY_CS_PS_SERVICE &&
      reg_state_get_active_rat_per_subs(reg_other_as_id) == SYS_RAT_GSM_RADIO_ACCESS &&
      /*Adding CS stack id to get PLMN , becuase during split process finished or dual stack mode
      CS stack PLMN is to check for forbidden PLMN */
#ifdef FEATURE_DUAL_SIM
      (reg_sim_plmn_gprs_forbidden(reg_state_service_plmn_get_per_subs(reg_other_as_id), (sys_modem_as_id_e_type)reg_sub_id) == FALSE )
#else
      (reg_sim_plmn_gprs_forbidden(reg_state_service_plmn_get_per_subs(reg_other_as_id))==FALSE )
#endif
    )
  {
#ifdef FEATURE_SGLTE_DUAL_SIM
      if (!mm_get_verify_ps_transfer_conditions(reg_other_as_id))
      {
          /* Call must be pending, start the timer instead*/
          MSG_HIGH_DS_0(REG_SUB, "=REG= Call pending, start PS timer");
          reg_timers_start_sglte_ps_timer_val (5000);
          return &reg_state_registering;
      }
#endif     

    next_state_p = &reg_state_idle;
    reg_send_ps_timer_expiry();
  }
  else
  {
    next_state_p = &reg_state_registering;
  }
  return next_state_p;
}

#endif

#ifdef FEATURE_FEMTO_CSG
/*==============================================================================

FUNCTION NAME

  process_mmr_csg_list_read_complete
==============================================================================*/
static reg_state_p_type process_mmr_csg_list_read_complete
(
 void
)
{
  reg_state_p_type next_state_p = &reg_state_registering;

  reg_sim_set_csg_list_read_complete(TRUE);
  
  return next_state_p;
}
#endif 

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

reg_state_s_type reg_state_registering = 
{
  process_cm_network_list_req,
  reg_state_process_cm_service_req,
  reg_state_process_cm_sim_available_req,
  reg_state_process_cm_sim_not_available_req,
  reg_state_process_cm_stop_mode_req,
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
  reg_state_ignore_hplmn_search_request,
#endif
  reg_state_unexpected_cm_ph_status_chgd_req,
#ifdef FEATURE_LTE
  reg_state_process_cm_set_drx_req,
#endif 
  process_mmr_camped_ind,
  reg_state_process_mmr_attach_failed_ind,
  process_mmr_mm_information_ind,
  reg_state_unexpected_mmr_plmn_search_cnf,
  reg_state_process_mmr_reg_cnf,
  reg_state_unexpected_mmr_service_ind,
  process_mmr_sim_available_cnf,
  reg_state_unexpected_mmr_sim_not_available_cnf,
  reg_state_unexpected_mmr_stop_mode_cnf,
  process_hplmn_search_timer_expiration,
  reg_state_process_mmr_emergency_num_list_ind,
  reg_state_process_sim_refresh_fcn_ind,
  reg_state_unexpected_ph_status_change_cnf,
  reg_state_process_mmr_reg_reject_ind,
  reg_state_unexpected_hplmn_irat_search_timer_expiration,  
  process_cm_set_hplmn_irat_timer,
  reg_state_t3245_exp_clean_up,
#ifdef FEATURE_LTE
  reg_state_process_backoff_fplmn_timer_expiration,
  reg_state_process_cm_block_plmn_req,
  reg_state_ignore_update_lte_cap_timer_expiration,
#endif
#ifdef FEATURE_SGLTE
  process_sglte_ps_timer_expiry,
#endif
  reg_state_unexpected_lte_irat_search_timer_expiration,
#ifdef FEATURE_FEMTO_CSG
  process_mmr_csg_list_read_complete,
#endif 
  reg_state_process_mmr_reg_req_ignored_ind,
  number,
  output_name
};

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/
