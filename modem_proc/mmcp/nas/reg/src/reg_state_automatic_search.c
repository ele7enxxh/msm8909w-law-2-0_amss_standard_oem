/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state_automatic_search.c_v   1.2   20 May 2002 11:47:10   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_state_automatic_search.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release.
05/20/02   kwa     Added output_name function.
08/22/02   kwa     Added logic to restart HPLMN Search Period Timer.
09/19/02   kwa     Added function to process the cm_act_req message.
04/04/03   kwa     Added call to reg_mode_start_plmn_selection if the search
                   for a higher priority PLMN is not initiated.
05/21/03   kwa     Restructured code to eliminate lint warnings.
09/07/03   kwa     Replaced call to reg_mode_start_plmn_selection with
                   reg_mode_reinitiate_plmn_selection to properly reset the
                   HPLMN search period timer.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added reg_mode_band_pref_get() in function call to
                   reg_mode_start_plmn_selection within
                   process_mmr_plmn_search_cnf.
11/19/03   kwa     Removed obsolete function for retrieving the state name.
11/20/03   kwa     Added functionality to search both RATs.
12/03/03   kwa     Added code that allows MM to abort the search for a higher
                   priority PLMN if the phone is in a call.
01/22/04   kwa     Added call to reg_state_service_status to support an
                   optimization to avoid performing a full search for service
                   when the user initiates PLMN selection and the phone is
                   already camped on the HPLMN.
02/04/04   kwa     Added support for PS follow on request.
03/03/04   kwa     Added calls to clear the attempted and available PLMN lists
                   only when the search for a higher priority PLMN is initiated.
04/28/04   kwa     Force the mode to SYS_NETWORK_SELECTION_MODE_AUTOMATIC in
                   process_mmr_plmn_search_cnf to handle the case when normal
                   service has been acquired while looking for limited service.
05/17/04   kwa     Added function to process the mmr_camped_ind message.
11/12/04   kwa     Added support for FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
                   in process_mmr_plmn_search_cnf.
02/22/05   up      Updated for Cingular ENS feature.
                   Updated state table to add new function pointer for FCN IND.
                   Updated function process_mmr_plmn_search_cnf.
06/28/05   up      Start HPLMN timer of 10 sec in connected mode.
                   i.e. PLMN serach is not completed or rejected by MM.
09/19/05   up      Updated state table for new primitive CM_HPLMN_SEARCH_REQ.
10/24/05   up      Ignore PLMN Search CNF of MANUAL Search in AUTO Search state.
11/07/05   ajt     Added handling of MMR_EMERGENCY_NUM_LIST_IND in Automatic Search state.
01/18/06   up      Updated process_mmr_plmn_search_cnf to ignore
                   PLMN_SEARCH_CNF of OLD request (old TI).
11/23/06   sn      Added feature FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
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
#include "reg_nv.h"
#include "err.h"
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/
#if !defined FEATURE_RAT_PRIORITY_LIST && !defined FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
#if defined(FEATURE_TRIPLE_SIM)  || defined(FEATURE_SGLTE_DUAL_SIM)
static boolean                       reg_state_automatic_search_complete_sim[MAX_NAS_STACKS] = { TRUE, TRUE, TRUE};
#define reg_state_automatic_search_complete reg_state_automatic_search_complete_sim[reg_as_id]
static sys_radio_access_tech_e_type  reg_state_automatic_search_rat_sim[MAX_NAS_STACKS];
#define reg_state_automatic_search_rat reg_state_automatic_search_rat_sim[reg_as_id]

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE/* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
static boolean                       reg_state_automatic_search_complete_sim[MAX_NAS_STACKS] = { TRUE, TRUE};
#define reg_state_automatic_search_complete reg_state_automatic_search_complete_sim[reg_as_id]
static sys_radio_access_tech_e_type  reg_state_automatic_search_rat_sim[MAX_NAS_STACKS];
#define reg_state_automatic_search_rat reg_state_automatic_search_rat_sim[reg_as_id]

#else

static boolean                       reg_state_automatic_search_complete = TRUE;
static sys_radio_access_tech_e_type  reg_state_automatic_search_rat;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/
#endif


/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  process_mmr_plmn_search_cnf

==============================================================================*/

static reg_state_p_type process_mmr_plmn_search_cnf
(
  const mmr_plmn_search_cnf_s_type* msg_p
)
{
  reg_state_p_type next_state_p = &reg_state_idle;

  if (msg_p->transaction_id != reg_state_plmn_search_transaction_id_get())
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= Ignore MMR_PLMN_SEARCH_CNF TI (%d) mismatch with TI (%d)", 
                  msg_p->transaction_id, reg_state_plmn_search_transaction_id_get());
    return &reg_state_automatic_search; 
  }
  if (msg_p->service_search == SYS_SERVICE_SEARCH_IRAT_PLMN)
  {  
     /* Determine if the request for PLMN list search was
     ** for forced IRAT by checking if the current PLMN is EHPLMN
     */
     if (reg_state_roaming_ind_get() == SYS_ROAM_STATUS_OFF)
     {       
       if(msg_p->plmn_search_result != REG_MM_PLMN_SEARCH_SUCCESS)
       {
          reg_state_start_irat_timers_with_10sec();
         next_state_p = &reg_state_idle;
       }
       else
       {
        if (reg_state_get_lte_search_requested())
        {
          reg_state_set_force_irat_search_initiated(FALSE);
        } 
        if (reg_mode_start_higher_priority_irat_plmn_selection(&msg_p->found_plmn_list.plmn_list))  
        {    
             next_state_p = &reg_state_registering;  
        }
      }
    }
  }
  else 
  {
    
    /*
    ** Abort the higher priority search if MM failed to initiate the search
    ** because the phone was in a call.
    */
    if (msg_p->plmn_search_result != REG_MM_PLMN_SEARCH_SUCCESS)
    {
      /*
      ** Calling the stop function insures the timer is set to its minimum
      ** value (2 minutes) when it is started.
      */
      reg_timers_stop_hplmn_search_period_timer();
  
      /*
      ** Start 30 sec timer to initiate the search ASAP.
      reg_timers_start_hplmn_search_period_timer();
      */
      reg_timers_start_hplmn_search_timer_10sec();
  
      next_state_p = &reg_state_idle;
    }
    /*
    ** If a higher priority network was found then PLMN selection is started.
    */
    else if (reg_mode_start_higher_priority_plmn_selection(&(msg_p->found_plmn_list.plmn_list)))
    {
  #ifdef FEATURE_ENHANCED_NW_SELECTION
      reg_state_delay_fcn_act_hplmn_set(FALSE);
  #endif
      next_state_p = &reg_state_registering;
    }
  #ifndef FEATURE_RAT_PRIORITY_LIST
  #ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
    /*
    ** Initiate a search on the second RAT.
    */
    else if (!reg_state_automatic_search_complete)
    {
      if (reg_state_automatic_search_rat == SYS_RAT_GSM_RADIO_ACCESS)
      {
        reg_state_automatic_search_rat = SYS_RAT_UMTS_RADIO_ACCESS;
      }
      else
      {
        reg_state_automatic_search_rat = SYS_RAT_GSM_RADIO_ACCESS;
      }
  
      reg_send_mmr_plmn_search_req
      (
        SYS_NETWORK_SELECTION_MODE_AUTOMATIC, /* network_selection_mode */
        FALSE,                                /* dual_mode              */
        reg_state_automatic_search_rat        /* rat                    */
  #ifdef FEATURE_GSM_BAND_PREF
        ,reg_mode_band_pref_get()             /* band_pref              */
  #endif
  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
        ,SYS_SERVICE_SEARCH_NONE 
  #endif
      );
  
      next_state_p = &reg_state_automatic_search;
    }
  #endif
  #endif/*FEATURE_LTE*/
  
  
  #ifndef FEATURE_RAT_PRIORITY_LIST
  #ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
    reg_state_automatic_search_complete = TRUE;
  #endif
  #endif /*FEATURE_LTE*/
  }
  return next_state_p;
}

/*==============================================================================

FUNCTION NAME

  process_cm_network_list_req

==============================================================================*/
reg_state_p_type process_cm_network_list_req
(
  const cm_network_list_req_s_type* msg_p
)
{
#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
   sys_found_plmn_list_u_type  *available_list = NULL;
  if(msg_p->list_search_type == REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND)
  {
    /* Immediately respond CM_NETWORK_LIST_CNF with SEARCH FAILED */
#ifdef FEATURE_MODEM_HEAP
    available_list = ( sys_found_plmn_list_u_type *)modem_mem_alloc(sizeof( sys_found_plmn_list_u_type), MODEM_MEM_CLIENT_NAS);
#else
    available_list = ( sys_found_plmn_list_u_type *)gs_alloc(sizeof( sys_found_plmn_list_u_type));
#endif
    reg_check_for_null_ptr((void*)available_list );
    available_list->plmn_list.length = 0;
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
    return &reg_state_automatic_search;
  }
  else
#endif
  {
    return reg_state_process_cm_network_list_req(msg_p);
  }
}


/*==============================================================================

FUNCTION NAME

  process_mmr_service_ind

==============================================================================*/
static reg_state_p_type process_mmr_service_ind
(
  mmr_service_ind_s_type* msg_p
)
{

  if ((msg_p->service_state.active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
       (msg_p->cause == MMR_CAUSE_ABORT_PLMN_SELECTION))
  {
     MSG_HIGH_DS_0(REG_SUB, "=REG= abort PLMN search and process service ind");
     reg_send_mmr_plmn_search_abort_req();
     (void) reg_state_process_mmr_service_ind(msg_p);
     return &reg_state_idle;
  }
  else if((msg_p->service_state.active_rat == SYS_RAT_LTE_RADIO_ACCESS) && 
  	 (msg_p->cause == MMR_CAUSE_INITIATE_PLMN_SELECTION_ON_GW))
  {
     return reg_state_process_mmr_service_ind(msg_p);     
  }	
  else if((msg_p->service_state.service_status == SYS_SRV_STATUS_NO_SRV) &&
          (msg_p->cause == MMR_CAUSE_NOT_SPECIFIED ))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= abort PLMN search and Later on PLMN search CNF No service ind will be sent to CM");
    reg_send_mmr_plmn_search_abort_req();
  }
  else if((msg_p->service_state.service_status == SYS_SRV_STATUS_SRV) &&
          (msg_p->cause == MMR_CAUSE_NOT_SPECIFIED ))
  {
    (void) reg_state_process_mmr_service_ind(msg_p);
  }
  else
  {
     MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_SERVICE_IND - Unexpected");
  }

   return &reg_state_automatic_search;
}

/*==============================================================================

FUNCTION NAME

  process_mmr_service_cnf

==============================================================================*/
static reg_state_p_type process_mmr_reg_cnf
(
  mmr_reg_cnf_s_type* msg_p
)
{
   if((msg_p->service_state.active_rat == reg_mode_req_rat_get()) && 
      (msg_p->service_state.service_status == SYS_SRV_STATUS_SRV)&&
      (msg_p->cause == MMR_CAUSE_NOT_SPECIFIED ))
   {
      (void) reg_state_process_mmr_reg_cnf(msg_p);
   }
   else
   {
      MSG_ERROR_DS_0(REG_SUB, "=REG= MMR_REG_CNF - Unexpected");
   }
   return &reg_state_automatic_search;
}

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
  reg_state_p_type next_state_p = &reg_state_automatic_search;
  sys_csg_info_s_type  csg_info = reg_state_csg_info_get();
  reg_sim_set_csg_list_read_complete(TRUE);

  if((reg_state_power_on_csg_search == TRUE) && 
    (reg_state_service_status_get()== SYS_SRV_STATUS_SRV) )
  {
    if((csg_info.csg_id == SYS_CSG_ID_INVALID) || 
       (csg_info.hybrid_cell == TRUE))
    {
      reg_send_cm_csg_search_trigger_ind();
    }
    reg_state_power_on_csg_search = FALSE; 
  }
  return next_state_p;
}
#endif 
/*==============================================================================

FUNCTION NAME

  process_mmr_camped_ind

==============================================================================*/

static reg_state_p_type process_mmr_camped_ind
(
  mmr_camped_ind_s_type* msg_p
)
{
  (void) msg_p;
  if(msg_p->policy_change_pending == TRUE)
  {
#ifdef FEATURE_TDSCDMA
    reg_nv_set_pm_chg_pending_plmn_rat(msg_p->plmn, msg_p->active_rat);
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
  }
  return (&reg_state_automatic_search);
}
/*==============================================================================

FUNCTION NAME

  number

==============================================================================*/

static reg_state_e_type number ( void )
{
  return REG_STATE_AUTOMATIC_SEARCH;
}


/*==============================================================================

FUNCTION NAME

  output_name

==============================================================================*/

static void output_name ( void )
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_AUTOMATIC_SEARCH");
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

  return (&reg_state_automatic_search);
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
  return (&reg_state_automatic_search);
}


/*==============================================================================

FUNCTION NAME

  process_hplmn_irat_search_timer_expiration

==============================================================================*/
static reg_state_p_type process_hplmn_irat_search_timer_expiration
(
  void
)
{
  reg_state_p_type next_state_p = &reg_state_automatic_search;
  if(reg_state_get_irat_lte_timer_search_ongoing() == TRUE)
  {
    /*starting 10 second timer */
    reg_timers_start_hplmn_irat_search_period_timer_10sec();
  }
  return next_state_p;
}


/*==============================================================================

FUNCTION NAME

  process_hplmn_irat_search_timer_expiration

==============================================================================*/
static reg_state_p_type process_lte_irat_search_timer_expiration
(
  void
)
{
  reg_state_p_type next_state_p = &reg_state_automatic_search;

  if(reg_state_get_irat_hplmn_timer_search_ongoing() == TRUE)
  {
    /*starting 10 second timer */
    reg_timers_start_lte_irat_search_period_timer_10sec();
  }
  
  return next_state_p;
}

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

reg_state_s_type reg_state_automatic_search = 
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
  process_mmr_plmn_search_cnf,
  process_mmr_reg_cnf,
  process_mmr_service_ind,
  reg_state_unexpected_mmr_sim_available_cnf,
  reg_state_unexpected_mmr_sim_not_available_cnf,
  reg_state_unexpected_mmr_stop_mode_cnf,
  reg_state_unexpected_hplmn_search_timer_expiration,
  reg_state_process_mmr_emergency_num_list_ind,
  reg_state_process_sim_refresh_fcn_ind,
  reg_state_unexpected_ph_status_change_cnf,
  reg_state_process_mmr_reg_reject_ind,
  process_hplmn_irat_search_timer_expiration,
  process_cm_set_hplmn_irat_timer,
  reg_state_process_t3245_expiry,
#ifdef FEATURE_LTE
  /*LTE capabilities will be updated after automatic search completion on 
     getting service indication from MM */
  reg_state_process_backoff_fplmn_timer_expiration,
  reg_state_process_cm_block_plmn_req,
  reg_state_ignore_update_lte_cap_timer_expiration,
#endif
#ifdef FEATURE_SGLTE
  reg_state_unexpected_sglte_ps_timer_expiry,
#endif
  process_lte_irat_search_timer_expiration,
#ifdef FEATURE_FEMTO_CSG
  process_mmr_csg_list_read_complete,
#endif 
  reg_state_unexpected_mmr_reg_req_ignored_ind,
  number,
  output_name
};

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_state_automatic_search_initiated

==============================================================================*/

void reg_state_automatic_search_initiated
(
  boolean                      dual_mode,
  sys_radio_access_tech_e_type rat
)
{
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
  reg_state_automatic_search_rat = rat;
  if (dual_mode)
  {
    reg_state_automatic_search_complete = FALSE;
  }
  else
  {
    reg_state_automatic_search_complete = TRUE;
  }
#endif
#endif
}
