/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state_manual_search.c_v   1.4   07 Jun 2002 14:23:56   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_state_manual_search.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/20/02   kwa     Added output_name function.
05/29/02   kwa     Changed mode when call routine to prioritize PLMN list.
06/06/02   kwa     Added logic to handle sending multiple mmr_plmn_search_req
                   messages when operating in dual mode.
06/27/02   kwa     Modified function that sends an mmr_plmn_search_req due to
                   interface change.
09/19/02   kwa     Added function to process the cm_act_req message.
11/04/02   kwa     Eliminated call to start plmn selection after receiving
                   the available PLMN list from MM.
12/30/02   kwa     Modified F3 message to fit within 40 characters.
05/21/03   kwa     Restructured code to eliminate lint warnings.
05/22/03   kwa     Modified process_mmr_plmn_search_cnf to pass a const
                   pointer to eliminate a lint warning.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added support for band preferences.
11/16/03   kwa     Modified process_mmr_plmn_search_cnf so the available PLMN
                   list stored in reg_mode.c is used during manual PLMN
                   selection. 
11/19/03   kwa     Removed obsolete function for retrieving the state name.
04/28/04   kwa     Added reg_state_manual_search_mode_pref so the mode
                   preference received in the cm_network_list_req is used to
                   prioritize the available PLMN list.
05/17/04   kwa     Added function to process the mmr_camped_ind message.
11/12/04   kwa     Added support for FEATURE_GTOW_BACKGROUND_PLMN_SEARCH in
                   process_mmr_plmn_search_cnf.
02/22/05   up      Updated for Cingular ENS feature.
                   Updated state table to add new function pointer for FCN IND.
                   Updated function process_mmr_plmn_search_cnf.
09/19/05   up      Updated state table for new primitive CM_HPLMN_SEARCH_REQ.
10/24/05   up      Ignore Auto PLMN Search while waiting for MANUAL Search CNF.
11/07/05   ajt     Added handling of MMR_EMERGENCY_NUM_LIST_IND in Manual Search state.
12/08/05   up      Send status of Manual Search in cm_network_list_cnf.
01/05/06   sn      Modified structure reg_state_manual_search. Added new
                   function process_hplmn_search_timer_expiration.
01/18/06   up      Updated process_mmr_plmn_search_cnf to ignore
                   PLMN_SEARCH_CNF of OLD request (old TI).
02/20/06   sn      EHPLMN Feature
04/18/06   sn      Added function process_mmr_service_ind to add PLMN to 
                   forbidden list when received respective reject cause.
10/12/06   sn      WCDMA_1X mode preference implementation.
11/23/06   sn      Added feature FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
11/23/06   sn      FEATURE_PRESERVE_MANUAL_PLMN_LIST `	
09/20/11   sla     Store the PLMn id of the cell where GSTK event is sent 
09/20/11   sla    Store last gstk service status sent.send IDLE event to GSK 
                  if current service status and last gstk sent service status
                  are different 
3/26/13   c_mg    Ported CR-447611
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
#include "sys_eplmn_list_v.h"
#ifdef FEATURE_GSTK
#include "reg_nv.h"
#endif
#include <stringl/stringl.h>
#include "err.h"
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern reg_cm_network_list_req_e_type reg_last_recieved_list_req_type_sim[MAX_NAS_STACKS];
#define reg_last_recieved_list_req_type reg_last_recieved_list_req_type_sim[reg_as_id]

#ifndef FEATURE_RAT_PRIORITY_LIST
#ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH

#ifdef FEATURE_TRIPLE_SIM /*FEATURE_TRIPLE_SIM */
static boolean                       reg_state_manual_search_complete_sim[MAX_NAS_STACKS] = { FALSE, FALSE,FALSE };
#define reg_state_manual_search_complete reg_state_manual_search_complete_sim[reg_as_id]
#else
static boolean                       reg_state_manual_search_complete_sim[MAX_NAS_STACKS] = { FALSE, FALSE};
#define reg_state_manual_search_complete reg_state_manual_search_complete_sim[reg_as_id]
#endif

static sys_radio_access_tech_e_type  reg_state_manual_search_rat_sim[MAX_NAS_STACKS];
#define reg_state_manual_search_rat reg_state_manual_search_rat_sim[reg_as_id]
#endif
static sys_mode_pref_e_type          reg_state_manual_search_mode_pref_sim[MAX_NAS_STACKS];
#define reg_state_manual_search_mode_pref   reg_state_manual_search_mode_pref_sim[reg_as_id]
#else
static sys_rat_pri_list_info_s_type  reg_state_manual_rat_pri_list_info_sim[MAX_NAS_STACKS];
#define reg_state_manual_rat_pri_list_info reg_state_manual_rat_pri_list_info_sim[reg_as_id]
#endif

#define sys_eplmn_list_validate(a) sys_eplmn_per_nas_stacks_list_validate(a, (sys_modem_as_id_e_type)reg_as_id)

#else
extern reg_cm_network_list_req_e_type reg_last_recieved_list_req_type;
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
static boolean                       reg_state_manual_search_complete = FALSE;
static sys_radio_access_tech_e_type  reg_state_manual_search_rat;
#endif
static sys_mode_pref_e_type          reg_state_manual_search_mode_pref;
#else
static sys_rat_pri_list_info_s_type  reg_state_manual_rat_pri_list_info;
#endif

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/


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
#ifdef FEATURE_EQUIVALENT_HPLMN
  uint32                             i,j;
#endif

  sys_detailed_plmn_list_s_type     *available_plmn_list =  NULL;
  sys_found_plmn_list_u_type        *available_plmn_u_list =  NULL;
  const sys_detailed_plmn_list_s_type  *available_list = (reg_mode_available_plmn_list_manual_get());
  uint32                             list_size = 0;

  reg_state_p_type next_state_p = &reg_state_manual_search;

#if defined (FEATURE_WTOG_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_WTOW_BACKGROUND_PLMN_SEARCH) \
    || defined (FEATURE_RAT_PRIORITY_LIST)
#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
  if (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC &&
      msg_p->service_search == SYS_SERVICE_SEARCH_NONE) 
#else
  if (msg_p->network_selection_mode != SYS_NETWORK_SELECTION_MODE_MANUAL)
#endif
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Ignore MMR_PLMN_SEARCH_CNF of Automatic Search");
    /*
    ** Start 30 sec timer to initiate the search ASAP.
    ** Starting the timer as the previous automatic search is ignored.
    */
    if(reg_state_roaming_ind_get()== SYS_ROAM_STATUS_ON)
    {
       reg_timers_start_hplmn_search_timer_10sec();
    }
    else
    { 
       /* Roaming is off if the IRAT HPLMN search is ongoing*/
      reg_state_start_irat_timers_with_10sec();

    }
    return next_state_p; 
  }
#endif
  if (msg_p->transaction_id != reg_state_plmn_search_transaction_id_get())
  {
    MSG_HIGH_DS_2(REG_SUB, "=REG= Ignore MMR_PLMN_SEARCH_CNF TI (%d) mismatch with TI (%d)", 
                  msg_p->transaction_id, reg_state_plmn_search_transaction_id_get());
    return next_state_p; 
  }
#ifdef FEATURE_MODEM_HEAP
  available_plmn_u_list = (sys_found_plmn_list_u_type *)modem_mem_alloc(sizeof(sys_found_plmn_list_u_type), MODEM_MEM_CLIENT_NAS);
#else
  available_plmn_u_list = (sys_found_plmn_list_u_type *)gs_alloc(sizeof(sys_found_plmn_list_u_type));
#endif
  reg_check_for_null_ptr((void*)available_plmn_u_list );
  available_plmn_list = &available_plmn_u_list->plmn_list;
  if(msg_p->service_search == SYS_SERVICE_SEARCH_PCI_PLMN)
  { 
     reg_send_cm_network_list_cnf
    (
      reg_state_cm_transaction_id_get(),
      reg_sim_read_rplmn(reg_mode_req_service_domain_get()),
      &(msg_p->found_plmn_list),
      msg_p->plmn_search_result
    );
    next_state_p = &reg_state_idle;
    reg_last_recieved_list_req_type = REG_CM_NETWORK_LIST_NONE;
  }
 #ifdef FEATURE_FEMTO_CSG
  else if(msg_p->service_search == SYS_SERVICE_SEARCH_CSG)
  {

    reg_mode_update_available_csg_list
    (
      &reg_state_manual_rat_pri_list_info,
      &(msg_p->found_plmn_list.plmn_list)
    );
 
    list_size = sizeof(uint32) + (available_list->length) * sizeof(sys_detailed_plmn_list_info_s_type);
    memscpy(available_plmn_list,list_size,available_list,list_size);

    reg_send_cm_network_list_cnf
    (
      reg_state_cm_transaction_id_get(),
      reg_sim_read_rplmn(reg_mode_req_service_domain_get()),
      available_plmn_u_list,
      msg_p->plmn_search_result
    );
    next_state_p = &reg_state_idle;
	reg_last_recieved_list_req_type = REG_CM_NETWORK_LIST_NONE;
  }
  else
#endif /*FEATURE_FEMTO_CSG*/
  {
    reg_mode_update_available_plmn_list_manual
    (
  #if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
      (msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC &&
       msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC)?
         REG_MODE_AUTOMATIC_MODE_NORMAL:REG_MODE_AUTOMATIC_MODE_MANUAL,
  #else    
      REG_MODE_AUTOMATIC_MODE_MANUAL,
  #endif
  #ifndef FEATURE_RAT_PRIORITY_LIST
      reg_state_manual_search_mode_pref,
  #else
      &reg_state_manual_rat_pri_list_info,
  #endif 
      reg_mode_req_service_domain_get(),
       &(msg_p->found_plmn_list.plmn_list),
        msg_p->service_search
    );

    
  #ifndef FEATURE_RAT_PRIORITY_LIST
  #ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
    /*
    ** Initiate the search on the second RAT.
    */
    if (!reg_state_manual_search_complete)
    {
      reg_state_manual_search_complete = TRUE;
  
      if (reg_state_manual_search_rat == SYS_RAT_GSM_RADIO_ACCESS)
      {
        reg_state_manual_search_rat = SYS_RAT_UMTS_RADIO_ACCESS;
      }
      else
      {
        reg_state_manual_search_rat = SYS_RAT_GSM_RADIO_ACCESS;
      }
  
      reg_send_mmr_plmn_search_req
      (
  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
        ((msg_p->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)&&
         (msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC ||
          msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL))?
           SYS_NETWORK_SELECTION_MODE_AUTOMATIC:SYS_NETWORK_SELECTION_MODE_MANUAL,
  #else
        SYS_NETWORK_SELECTION_MODE_MANUAL, /* network_selection_mode */
  #endif
        FALSE,                             /* dual_mode              */
        reg_state_manual_search_rat        /* rat                    */
  #ifdef FEATURE_GSM_BAND_PREF
        ,reg_mode_band_pref_get()          /* band_pref              */
  #endif
  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
        ,msg_p->service_search
  #endif
      );
    }
    /*
    ** Prioritize the complete PLMN list and send it to CM.
    */
    else
  #endif
  #endif /*FEATURE_RAT_PRIORITY_LIST*/
    {
       if( msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL ||
           msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC )
       {
         list_size = sizeof(uint32) + ((reg_mode_get_available_backgnd_list())->length) * sizeof(sys_detailed_plmn_list_info_s_type);
         memscpy(available_plmn_list,list_size,(reg_mode_get_available_backgnd_list()),list_size);
       }
       else
       {
         list_size = sizeof(uint32) + (available_list->length) * sizeof(sys_detailed_plmn_list_info_s_type);
         memscpy(available_plmn_list,list_size,available_list,list_size);
       }
  
  #ifdef FEATURE_EQUIVALENT_HPLMN
  
      if (reg_sim_disp_only_highest_priority_ehplmn() && 
          msg_p->service_search != SYS_SERVICE_SEARCH_BACKGROUND_MANUAL &&
          msg_p->service_search != SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC)
      {
        i=0;
  
        if (i<available_plmn_list->length && reg_sim_is_ehplmn(available_plmn_list->info[i].plmn))
        {
          i++;
          j = i;
          while (j<available_plmn_list->length && reg_sim_is_ehplmn(available_plmn_list->info[j].plmn))
          {
            /* If found another EHPLMN with other RAT of the first EHPLMN, insert
            ** highest priority EHPLMN with this RAT too. Keep other parameters
            ** same as jth this EHPLMN.
            */
            if (sys_plmn_match(available_plmn_list->info[0].plmn, available_plmn_list->info[j].plmn))
            {
              i++;
            }
            j++;
          }
  
          if (j!= i)
          {
            while (j<available_plmn_list->length)
            {
              available_plmn_list->info[i++]=available_plmn_list->info[j++];
            }
            available_plmn_list->length = i;
          } 
        }
      }
  #endif
#ifdef FEATURE_SGLTE
     if(REG_SGLTE_DUAL_STACK && 
        (msg_p->service_search == SYS_SERVICE_SEARCH_NONE|| 
         msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC ||
         msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL) &&
        (reg_as_id == reg_ps_stack_id))
     {
         reg_mode_copy_available_plmn_lists(msg_p->service_search);
     }
#endif
  
      /*
      ** Send the prioritized PLMN list to CM.
      */
      reg_send_cm_network_list_cnf
      (
        reg_state_cm_transaction_id_get(),
        reg_sim_read_rplmn(reg_mode_req_service_domain_get()),
        available_plmn_u_list,
        msg_p->plmn_search_result
      );
  
      if(msg_p->plmn_search_result == REG_MM_PLMN_SEARCH_PARTIAL)              
      {
        return (&reg_state_manual_search);
      }
  #ifdef FEATURE_ENHANCED_NW_SELECTION
      /*
      ** If FCN - Acting HPLMN is delayed, i.e. PLMN rescan should be done.
      ** Ignore MMR_REG_CNF if PLMN rescan is started.
      */
      if (reg_state_delay_fcn_act_hplmn_get()== TRUE) 
      {
        reg_state_delay_fcn_act_hplmn_set(FALSE);
        if (reg_mode_restart_plmn_selection() == TRUE)
        {
          MSG_HIGH_DS_0(REG_SUB, "=REG= PLMN rescan - ACT HPLMN Changed");
          reg_state_service_status_set(SYS_SRV_STATUS_NO_SRV);
#ifndef FEATURE_MODEM_HEAP
          gs_free(available_plmn_list);
#else
          modem_mem_free(available_plmn_list, MODEM_MEM_CLIENT_NAS);
#endif
          available_plmn_list =  NULL;
          return &reg_state_registering;
          reg_last_recieved_list_req_type = REG_CM_NETWORK_LIST_NONE;
        }
      }
  #endif
  #ifdef FEATURE_GSTK
      if ((reg_nv_is_imsi_switch_enabled()) &&
         (reg_state_is_gstk_event_to_be_send() ||
           (reg_state_service_status_get() != reg_state_get_last_gstk_sent_svc())) &&
           (!sys_plmn_id_is_undefined(reg_state_service_plmn_get())) &&
           (msg_p->plmn_search_result == REG_MM_PLMN_SEARCH_SUCCESS)&&
           (((msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_AUTOMATIC) &&
             (available_plmn_list->length  == 0))||
             (msg_p->service_search == SYS_SERVICE_SEARCH_BACKGROUND_MANUAL)))
      {
        reg_send_mmr_gstk_service( TRUE); 
        reg_state_update_last_gstk_sent_plmn(reg_state_service_plmn_get());
        reg_state_update_last_gstk_sent_svc(reg_state_service_status_get());
        reg_state_update_last_gstk_sent_rat(reg_state_active_rat_get());
        reg_state_set_gstk_event_flag(FALSE);
      }
  #endif
     }
      next_state_p = &reg_state_idle;
      reg_last_recieved_list_req_type = REG_CM_NETWORK_LIST_NONE;  
  }
#ifndef FEATURE_MODEM_HEAP
   gs_free(available_plmn_u_list);
#else
   modem_mem_free(available_plmn_u_list,MODEM_MEM_CLIENT_NAS);
#endif
  available_plmn_u_list =  NULL;
  return next_state_p;
}

/*==============================================================================

FUNCTION NAME

  process_mmr_reg_cnf

==============================================================================*/

static reg_state_p_type process_mmr_reg_cnf
(
  mmr_reg_cnf_s_type* msg_p
)
{
  /*No change of state in case of MMR_REG_CNF*/
  reg_state_p_type next_state_p = &reg_state_manual_search;

  /*Update the EPLMN List */
  if (msg_p->update_equivalent_plmn_list)
  {
    boolean list_matched = reg_mode_save_equivalent_plmn_list
    (
      msg_p->service_state.plmn,
      &(msg_p->equivalent_plmn_list)
    );
    boolean eplmn_list_validated = sys_eplmn_list_validate(TRUE);
    if ((!list_matched) || eplmn_list_validated)
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= EPLMN list is either validated or eplmn_list changed");
      reg_send_eq_plmn_change_ind();
    }
  }
  return next_state_p;
}


#ifdef FEATURE_SGLTE

/*==============================================================================

FUNCTION NAME

  process_sglte_ps_timer_expiry

==============================================================================*/

static reg_state_p_type process_sglte_ps_timer_expiry(void)
{
  reg_state_p_type next_state_p;
  reg_timers_start_sglte_ps_timer();
  next_state_p = &reg_state_manual_search;
  return next_state_p;
}
#endif
/*==============================================================================

FUNCTION NAME

  reg_state_manual_search_process_mmr_service_ind

==============================================================================*/

static reg_state_p_type reg_state_manual_search_process_mmr_service_ind
(
  mmr_service_ind_s_type* msg_p
)
{
  /*No change of state in case of MMR_SERVICE_IND*/
  reg_state_p_type next_state_p = &reg_state_manual_search;

  if((msg_p->service_state.active_rat == SYS_RAT_LTE_RADIO_ACCESS) && 
     (msg_p->cause == MMR_CAUSE_INITIATE_PLMN_SELECTION_ON_GW))
  {
     sys_found_plmn_list_u_type *available_list = NULL;
     MSG_HIGH_DS_0(REG_SUB, "=REG= abort PLMN search and process service ind");
     reg_send_mmr_plmn_search_abort_req();
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
       REG_MM_PLMN_SEARCH_ABORTED /* Search aborted */
     );
#ifndef FEATURE_MODEM_HEAP
     gs_free(available_list);
#else
     modem_mem_free(available_list, MODEM_MEM_CLIENT_NAS);
#endif
     available_list = NULL;
     return reg_state_process_mmr_service_ind(msg_p);     
  }

  /*Update the EPLMN List */
  if (msg_p->update_equivalent_plmn_list)
  {
    boolean list_matched = reg_mode_save_equivalent_plmn_list
    (
      msg_p->service_state.plmn,
      &(msg_p->equivalent_plmn_list)
    );
    boolean eplmn_list_validated = sys_eplmn_list_validate(TRUE);
    if ((!list_matched) || eplmn_list_validated)
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= EPLMN list is either validated or eplmn_list changed");
      reg_send_eq_plmn_change_ind();
    }
  }
  return next_state_p;
}

/*==============================================================================

FUNCTION NAME

  number

==============================================================================*/

static reg_state_e_type number ( void )
{
  return REG_STATE_MANUAL_SEARCH;
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
  reg_state_p_type next_state_p = &reg_state_manual_search;
  /*starting 10 second timer */
    /* Roaming is off if the IRAT HPLMN search is ongoing*/
    reg_timers_start_hplmn_irat_search_period_timer_10sec();
  
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
  reg_state_p_type next_state_p = &reg_state_manual_search;
  /*starting 10 second timer */
  reg_timers_start_lte_irat_search_period_timer_10sec();
  
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

  reg_state_p_type next_state_p  = &reg_state_manual_search; 

  boolean     irat_timer_allowed = FALSE;


  MSG_HIGH_DS_1(REG_SUB, "=REG= CM sent HPLMN IRAT timer  - %lu(milli-sec)", timer_val);   

  if (reg_nv_w_bplmn_pseudo_enabled()
#ifdef FEATURE_TDSCDMA
     || reg_nv_is_forced_irat_enabled()
#endif
      )
  {
     if((reg_state_roaming_ind_get()== SYS_ROAM_STATUS_OFF)&&
      (reg_state_service_status_get() != SYS_SRV_STATUS_NO_SRV)&&
      ((reg_state_active_rat_get() == SYS_RAT_TDS_RADIO_ACCESS) ||
        (reg_state_active_rat_get()  == SYS_RAT_GSM_RADIO_ACCESS) ||
        (reg_state_active_rat_get()  == SYS_RAT_UMTS_RADIO_ACCESS)) &&
#ifdef FEATURE_TDSCDMA
        (reg_mode_is_usim_compliant() || reg_nv_w_bplmn_pseudo_enabled())&&
#else 
        reg_nv_w_bplmn_pseudo_enabled()&&
#endif
      reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS, reg_mode_rat_pri_list_get(), FALSE) &&
      (reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED))
  {
    irat_timer_allowed = TRUE;
  }
  }



  if(timer_val == 0)
  {
    timer_val = REG_TIMERS_MAX_TIMER_VALUE;    
    if(irat_timer_allowed)
    {
      reg_timers_start_hplmn_irat_search_period_timer_10sec();
      reg_nv_hplmn_irat_search_timer_set(timer_val);
    }
  }
  else
  {

    reg_nv_hplmn_irat_search_timer_set(timer_val);
    reg_timers_stop_hplmn_irat_search_period_timer();
    if(irat_timer_allowed)
    {
      reg_timers_start_hplmn_irat_search_period_timer();
    }
  }
  return next_state_p;
}


/*==============================================================================

FUNCTION NAME

  output_name

==============================================================================*/

static void output_name ( void )
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_MANUAL_SEARCH");
}

/*==============================================================================

FUNCTION NAME

  process_hplmn_search_timer_expiration

==============================================================================*/

static reg_state_p_type process_hplmn_search_timer_expiration( void )
{
  reg_state_p_type next_state_p = &reg_state_manual_search;

  /* Starting 10 sec timer, so that the timer is running when manual search completes. */
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
  reg_state_p_type next_state_p = &reg_state_manual_search;
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
  return (&reg_state_manual_search);
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

  return (&reg_state_manual_search);
}


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

reg_state_s_type reg_state_manual_search = 
{
  reg_state_process_cm_network_list_req,
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
  reg_state_manual_search_process_mmr_service_ind,
  reg_state_unexpected_mmr_sim_available_cnf,
  reg_state_unexpected_mmr_sim_not_available_cnf,
  reg_state_unexpected_mmr_stop_mode_cnf,
  process_hplmn_search_timer_expiration,
  reg_state_process_mmr_emergency_num_list_ind,
  reg_state_process_sim_refresh_fcn_ind,
  reg_state_unexpected_ph_status_change_cnf,
  reg_state_handle_mmr_reg_reject_ind,
  process_hplmn_irat_search_timer_expiration,
  process_cm_set_hplmn_irat_timer,
  reg_state_process_t3245_expiry,
#ifdef FEATURE_LTE
  /*LTE Capabilities will be updated after manual search completion on 
     getting service indication from MM */
  reg_state_process_backoff_fplmn_timer_expiration,
  reg_state_process_cm_block_plmn_req,
  reg_state_ignore_update_lte_cap_timer_expiration,
#endif
#ifdef FEATURE_SGLTE
  process_sglte_ps_timer_expiry,
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

  reg_state_manual_search_initiated

==============================================================================*/

void reg_state_manual_search_initiated
(
#ifndef FEATURE_RAT_PRIORITY_LIST
  sys_mode_pref_e_type         mode_pref,
#else
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info,
#endif
  reg_cm_network_list_req_e_type    search_type,
  sys_radio_access_tech_e_type rat
)
{
#ifndef FEATURE_RAT_PRIORITY_LIST
  reg_state_manual_search_mode_pref = mode_pref;
#ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
  reg_state_manual_search_rat       = rat;

  if ( (mode_pref == SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY) ||
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM 
       #error code not present
#endif
       (mode_pref == SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY))

  {
    reg_state_manual_search_complete = TRUE;
  }
  else
  {
    reg_state_manual_search_complete = FALSE;
  }
#endif
#else  
   memscpy(&reg_state_manual_rat_pri_list_info, sizeof(sys_rat_pri_list_info_s_type), rat_pri_list_info, sizeof(sys_rat_pri_list_info_s_type));
#endif /*FEATURE_RAT_PRIORITY_LIST*/

 if(search_type != REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND)
 {
  reg_mode_clear_available_plmn_list_manual();
 }  
}
