/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_state_idle.c_v   1.4   07 Jun 2002 14:23:46   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_state_idle.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/20/02   kwa     Added output_name function.
06/05/02   kwa     Filled roaming_ind when receiving an mmr_reg_cnf or
                   mmr_service_ind.
06/06/02   kwa     Replaced mode_pref with dual_mode and rat in
                   mmr_plmn_search_req.
06/27/02   kwa     Modified function that sends an mmr_plmn_search_req due to
                   interface change.
08/26/02   kwa     Removed limited service from check for loss of service in
                   process_mmr_service_ind.
08/26/02   kwa     Added call to reg_sim_write_rplmn_rat in 
                   process_mmr_service_ind to update the last used RAT when
                   service is reacquired on the RPLMN.
09/19/02   kwa     Added function to process the cm_act_req message.
10/04/02   kwa     Added code to properly handle reject causes in
                   process_mmr_service_ind.
10/10/02   kwa     Added code to handle expiration of plmn search period timer
                   in limited service network selection mode.
10/18/02   kwa     Added FEATURE_REG_DEEP_SLEEP.
11/05/02   kwa     Added process_mmr_stop_mode_cnf to handle an
                   mmr_stop_mode_cnf received in reponse to an mmr_stop_mode_req
                   sent to request deep sleep.
12/31/02   kwa     Added code to support equivalent PLMNs in
                   process_mmr_service_ind.
03/13/03   kwa     Modified process_mmr_service_ind so REG does not initiate
                   PLMN selection when the phone loss services and
                   FEATURE_REG_DEEP_SLEEP is not defined.  CM/MMoC are
                   responsible for initiating PLMN selection in this scenario.
03/27/03   kwa     Moved functionality of process_mmr_service_ind to
                   reg_state_process_mmr_service_ind.
04/03/03   kwa     Added code in process_hplmn_search_timer_expiration to
                   limit the search for a higher priority PLMN.  The search
                   is only initiated when a higher priority PLMN is contained
                   on the SIM in the same country as the RPLMN.
05/21/03   kwa     Restructured code to eliminate lint warnings.
06/04/03   kwa     Removed one remaining lint warning.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added support for band preferences.
09/29/03   kwa     Added code to handle the new enumerated value
                   SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION
                   in process_hplmn_search_timer_expiration.
11/19/03   kwa     Removed obsolete function for retrieving the state name.
01/12/04   kwa     Added call to reg_state_automatic_search_initiated to support
                   the search for a higher priority network on both RATs.
01/22/04   kwa     Added call to reg_state_service_status to support an
                   optimization to avoid performing a full search for service
                   when the user initiates PLMN selection and the phone is
                   already camped on the HPLMN. 
04/28/04   kwa     Modified process_hplmn_search_timer_expiration to initiate
                   a search for a higher priority network if normal service was
                   acquired while searching for limited service.
05/17/04   kwa     Added function to process the mmr_camped_ind message.
07/13/04   kwa     Replaced function process_mmr_reg_cnf with the more
                   general and correct reg_state_process_mmr_reg_cnf.
08/11/04   kwa     Added call to reg_send_cm_stop_mode_cnf in the function
                   process_mmr_reg_cnf to avoid a lock up scenario with CM.
11/12/04   kwa     Added support for FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
                   in process_hplmn_search_timer_expiration.
01/24/05   up      Modified process_hplmn_search_timer_expiration to allow
                   higher priority PLMN selection when the HPLMN timer
                   expires and MS is in LIMITED/LIMITED_REGIONAL service.
01/24/05   up      Updated function process_hplmn_search_timer_expiration to
                   use original preferred mode to determine the dual mode.
02/17/05   up      Modified process_hplmn_search_timer_expiration to handle
                   mode SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION.
02/22/05   up      Updated for Cingular ENS feature.
                   Updated state table to add new function pointer for FCN IND.
                   Added state function process_sim_refresh_fcn_ind.
03/01/05   up      ENS Fix - supported flags are replaced with hash defines.
07/18/05   up      Supporting for two new ENS EFs UIM_USIM_ACT_HPLMN and 
                   UIM_USIM_SPT_TABLE under USIM directory.
09/19/05   up      Added process_cm_hplmn_search_req and updated other
                   to support the on demand HPLMN search from user.
10/20/05   up      Updated for 3G to 2G BPLMN Search.
                   Kept under FEATURE_WTOG_BACKGROUND_PLMN_SEARCH.
11/07/05   ajt     Added handling of MMR_EMERGENCY_NUM_LIST_IND in idle state.                   
01/05/06   sn      Modified process_hplmn_search_timer_expiration. Added check
                   that timer will be started only if MS on VPLMN.
04/06/06   sn      Updated process_sim_refresh_fcn_ind for ehplmn
10/12/06   sn      WCDMA_1X mode preference implementation.
11/23/06   sn      Added feature FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
02/13/07   sn      Do not start HPLMN search if UE is in Limited Service.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "err.h"
#include "msg.h"
#include "reg_mode.h"
#include "reg_send.h"
#include "reg_sim_v.h"
#include "reg_state.h"
#include "ms.h"
#include "reg_timers.h"
#include "reg_log.h"
#include "reg_nv.h"
#include "sys_eplmn_list_v.h"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/
#if defined(FEATURE_TRIPLE_SIM)  || defined(FEATURE_SGLTE_DUAL_SIM)
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*
** Set this flag to TRUE if the HPLMN search is initiated in MANUAL mode.
*/
static sys_network_selection_mode_e_type reg_state_idle_prev_nw_sel_mode_sim[MAX_NAS_STACKS] = { SYS_NETWORK_SELECTION_MODE_NONE, SYS_NETWORK_SELECTION_MODE_NONE,SYS_NETWORK_SELECTION_MODE_NONE};
#define reg_state_idle_prev_nw_sel_mode reg_state_idle_prev_nw_sel_mode_sim[reg_as_id]
#endif

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*
** Set this flag to TRUE if the HPLMN search is initiated in MANUAL mode.
*/
static sys_network_selection_mode_e_type reg_state_idle_prev_nw_sel_mode_sim[MAX_NAS_STACKS] = { SYS_NETWORK_SELECTION_MODE_NONE, SYS_NETWORK_SELECTION_MODE_NONE};
#define reg_state_idle_prev_nw_sel_mode reg_state_idle_prev_nw_sel_mode_sim[reg_as_id]
#endif

#else
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*
** Set this flag to TRUE if the HPLMN search is initiated in MANUAL mode.
*/
static sys_network_selection_mode_e_type reg_state_idle_prev_nw_sel_mode = SYS_NETWORK_SELECTION_MODE_NONE;
#endif

#endif /* FEATURE_DUAL_SIM*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_validate(a) sys_eplmn_per_nas_stacks_list_validate(a, (sys_modem_as_id_e_type)reg_as_id)
#define sys_eplmn_list_rplmn() (sys_eplmn_per_nas_stacks_list_rplmn((sys_modem_as_id_e_type)reg_as_id))
#endif

#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_ENHANCED_NW_SELECTION
#define reg_sim_read_ens_flag() reg_sim_per_nas_stacks_read_ens_flag((sys_modem_as_id_e_type)reg_sub_id)
#endif

extern reg_as_id_e_type reg_multimode_sub;
#endif

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

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

  return (&reg_state_idle);
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

  process_hplmn_search_timer_expiration

==============================================================================*/

static reg_state_p_type process_hplmn_search_timer_expiration
(
  void
)
{
  sys_radio_access_tech_e_type rat = SYS_RAT_GSM_RADIO_ACCESS;
  reg_state_p_type             next_state_p = &reg_state_idle;
  boolean                      dual_mode = FALSE;
#ifdef FEATURE_RAT_PRIORITY_LIST
  sys_rat_pri_list_info_s_type *rat_pri_list_info;
#endif

#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
  sys_plmn_rat_list_s_type *higher_priority_plmn_list = NULL;
#ifdef FEATURE_MODEM_HEAP
  higher_priority_plmn_list = (sys_plmn_rat_list_s_type *)modem_mem_alloc(sizeof(sys_plmn_rat_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
  higher_priority_plmn_list = (sys_plmn_rat_list_s_type *)gs_alloc(sizeof(sys_plmn_rat_list_s_type));
#endif
  reg_check_for_null_ptr((void*)higher_priority_plmn_list );
  higher_priority_plmn_list->length = 0;
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST
  rat_pri_list_info = reg_mode_rat_pri_list_get();
#endif

  switch (reg_mode_network_selection_mode_get())
  {
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
      if ((reg_state_service_status_get() == SYS_SRV_STATUS_SRV) ||
          (reg_state_service_status_get() == SYS_SRV_STATUS_LIMITED) ||
          (reg_state_service_status_get() == SYS_SRV_STATUS_LIMITED_REGIONAL) )
      {
        if (reg_mode_higher_priority_network_available
              (
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
                reg_state_active_rat_get(),
                higher_priority_plmn_list
#endif
              ))
        {
#ifndef FEATURE_RAT_PRIORITY_LIST
          dual_mode = (boolean)((reg_mode_req_mode_pref_get() == SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED) ||
                                (reg_mode_req_mode_pref_get() == SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED));
#else
          dual_mode = ((reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_GSM_RADIO_ACCESS, rat_pri_list_info, FALSE))&&
                             ((reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_UMTS_RADIO_ACCESS, rat_pri_list_info, FALSE) ||
                               (reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_TDS_RADIO_ACCESS, rat_pri_list_info, FALSE)))));
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST 
          rat = reg_state_active_rat_get();
#else 
#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
          /*
          ** Until WCDMA to GSM background PLMN searching is supported all
          ** dual-mode PLMN searches will be initiated on GSM.
          ** WCDMA to GSM background PLMN search is supported so
          ** search is initiated on surrent active RAT.
          */
          switch (reg_mode_mode_pref_get())
          {
            case SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY:
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM 
            #error code not present
#endif
              rat = SYS_RAT_UMTS_RADIO_ACCESS;
              break;

            case SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY:
              rat = SYS_RAT_GSM_RADIO_ACCESS;
              break;

            case SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED:
            case SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED:
            default:
#ifdef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
              rat = reg_state_active_rat_get();
#else
              rat = SYS_RAT_GSM_RADIO_ACCESS;
#endif
              break;
          }
#else // FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
          rat = reg_state_active_rat_get();
#endif //FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
#endif //FEATURE_RAT_PRIORITY_LIST

          reg_state_automatic_search_initiated
          (
            dual_mode,
            rat
          );

          reg_send_mmr_plmn_search_req
          (
            SYS_NETWORK_SELECTION_MODE_AUTOMATIC, /* network_selection_mode      */
            dual_mode,                            /* dual_mode                   */
            rat                                   /* rat                         */
#ifdef FEATURE_GSM_BAND_PREF
            ,reg_mode_band_pref_get()             /* band_pref                   */
#endif
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
            ,higher_priority_plmn_list           /* higher_priority_plmn_list_p */
#endif
#if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined(FEATURE_LTE)
            ,SYS_SERVICE_SEARCH_NONE 
#endif
#ifdef FEATURE_FEMTO_CSG
            ,SYS_CSG_SEARCH_TYPE_NONE
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
            ,rat_pri_list_info
            ,NULL 
#endif
            ,  SYS_SCAN_SCOPE_FULL_BAND                      
          );

          /*
          ** REG needs to remember that the phone has lost service after successfully
          ** initiating a hplmn search in order to support an optimization to
          ** avoid always searching for service upon receiving a cm_service_req.
          ** This code can be removed once background searching is supported.
          */
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifndef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
          reg_state_service_status_set(SYS_SRV_STATUS_NO_SRV);
#endif
#endif

          next_state_p = &reg_state_automatic_search;

#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
          if (reg_state_idle_prev_nw_sel_mode != SYS_NETWORK_SELECTION_MODE_NONE)
          {
            reg_send_cm_hplmn_search_cnf(TRUE);
          }
#endif
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
          /*
          ** send LOG packet for starting of higher priority PLMN selecton.
          */
          reg_log_umts_nas_hplmn_search_start(higher_priority_plmn_list);
#endif
        }
        else
        {
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
          if (reg_state_idle_prev_nw_sel_mode == SYS_NETWORK_SELECTION_MODE_NONE)
          {
#endif
            /* If MS on HPLMN do not start the timer but reset so that next time timer is 2 min*/ 
            if(reg_state_roaming_ind_get() == SYS_ROAM_STATUS_ON)
            {
              reg_timers_start_hplmn_search_period_timer();
            }

#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
          }
          else
          {
            MSG_HIGH_DS_0(REG_SUB, "=REG= Ignore: HPLMN Search - NO higher priority list found in PPLMN (SIM/USIM)");
            reg_send_cm_hplmn_search_cnf(FALSE);
          }
#endif
          next_state_p = &reg_state_idle;
        }
      }
#if defined (FEATURE_GW_ON_DEMAND_HPLMN_SRCH) || defined (FEATURE_REL7_STEERING_OF_ROAMING)
      else
      {
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
        if (reg_state_idle_prev_nw_sel_mode != SYS_NETWORK_SELECTION_MODE_NONE)
        {
          MSG_HIGH_DS_1(REG_SUB, "=REG= Ignore: HPLMN Search - Not allowed in NO SERVICE status %d", reg_state_service_status_get());
          reg_send_cm_hplmn_search_cnf(FALSE);
        }
#endif
      }
#endif
      break;

    case SYS_NETWORK_SELECTION_MODE_MANUAL:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
      MSG_ERROR_DS_0(REG_SUB, "=REG= HPLMN Search Timer expired - manual mode"); 
      break;

    case SYS_NETWORK_SELECTION_MODE_LIMITED:
      MSG_ERROR_DS_0(REG_SUB, "=REG= HPLMN Search Timer expired - limited mode"); 
      break;

    default:
      MSG_ERROR_DS_1(REG_SUB, "=REG= HPLMN Search Timer expired - mode (%d)", reg_mode_network_selection_mode_get()); 
      break;
  }

#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
  reg_state_idle_prev_nw_sel_mode = SYS_NETWORK_SELECTION_MODE_NONE;
#endif
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH ) || defined(FEATURE_RAT_PRIORITY_LIST)
#ifndef FEATURE_MODEM_HEAP
   gs_free(higher_priority_plmn_list);
#else
   modem_mem_free(higher_priority_plmn_list, MODEM_MEM_CLIENT_NAS);
#endif
   higher_priority_plmn_list = NULL;
#endif
  return next_state_p;
}

/*==============================================================================

FUNCTION NAME

  process_sim_refresh_fcn_ind

==============================================================================*/
static reg_state_p_type process_sim_refresh_fcn_ind
(
  sim_refresh_fcn_ind_s_type* msg_p
)
{
  reg_state_p_type next_state_p = &reg_state_idle;
  sys_plmn_list_s_type eq_plmn_list;
  boolean              list_matched = FALSE;
  boolean eplmn_list_validated = FALSE;
#ifdef FEATURE_RAT_PRIORITY_LIST
  sys_rat_pri_list_info_s_type *rat_pri_list_info = reg_mode_rat_pri_list_get();
  reg_mode_3gpp_capability_s_type rat_priority_list;
#endif 
#ifdef FEATURE_ENHANCED_NW_SELECTION
  uint8            ens_flag;

  ens_flag = reg_sim_read_ens_flag();
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST  
  reg_mode_get_3gpp_rat_capabilty(&rat_priority_list,rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );
#endif
    switch(msg_p->file_id)
    {
#ifdef FEATURE_ENHANCED_NW_SELECTION
      case MMGSDI_SIM_7F66_PROP1_ACT_HPLMN:
      case MMGSDI_USIM_7F66_PROP1_ACT_HPLMN:
        if (ens_flag & GSDI_SUPPORT_ACTING_HPLMN)
        {
#ifdef FEATURE_EQUIVALENT_HPLMN
          (void)reg_sim_read_ehplmn_list();
#endif
          reg_state_process_fcn_act_hplmn(FALSE);
          if (reg_mode_restart_plmn_selection() == TRUE)
          {
            reg_state_service_status_set(SYS_SRV_STATUS_NO_SRV);
            next_state_p = &reg_state_registering;
          }
        }
        break;
#endif
      case MMGSDI_USIM_HPLMN:
      case MMGSDI_GSM_HPLMN:
         /*
        This may be needed to satisfy ENS test cases along with general SIM refresh implementation. 
#ifdef FEATURE_ENHANCED_NW_SELECTION
         if (ens_flag & GSDI_SUPPORT_HPLMN_TIMER)
        {
#endif
        */
        reg_state_process_fcn_hplmn();
        break;

      case MMGSDI_GSM_FPLMN:
      case MMGSDI_USIM_FPLMN:
        
        MSG_HIGH_DS_0(REG_SUB, "=REG= FPLMN List Updated");

        reg_nv_sys_equivalent_plmn_list_get(&eq_plmn_list);
        list_matched = reg_mode_save_equivalent_plmn_list(sys_eplmn_list_rplmn(), &eq_plmn_list);
        eplmn_list_validated = sys_eplmn_list_validate(TRUE);

        if((!list_matched) || eplmn_list_validated)
        {
          MSG_HIGH_DS_0(REG_SUB, "=REG= EPLMN list is either validated or eplmn_list changed");
          reg_send_eq_plmn_change_ind();
        }
        break;

      case MMGSDI_GSM_HPLMNACT:
      case MMGSDI_USIM_HPLMNWACT:

        if((sys_plmn_match(reg_sim_read_hplmn(), reg_sim_read_rplmn(reg_mode_req_service_domain_get()))) &&
           (reg_state_service_status_get() == SYS_SRV_STATUS_SRV))
        {
#ifndef FEATURE_RAT_PRIORITY_LIST        
          if(((reg_state_active_rat_get() == SYS_RAT_GSM_RADIO_ACCESS) &&
              (reg_sim_read_hplmn_rat_search_order(reg_mode_req_mode_pref_get()) == 
               SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED)) ||
             ((reg_state_active_rat_get() == SYS_RAT_UMTS_RADIO_ACCESS) &&
              (reg_sim_read_hplmn_rat_search_order(reg_mode_req_mode_pref_get()) == 
               SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED)))
#else
          reg_sim_read_hplmn_rat_search_order(&rat_priority_list);
          if(rat_priority_list.rat_list[0] != reg_state_active_rat_get())
#endif /*FEATURE_RAT_PRIORITY_LIST*/
          {
            reg_mode_start_automatic_plmn_selection
            (
              SYS_NETWORK_SELECTION_MODE_AUTOMATIC,
              REG_MODE_AUTOMATIC_MODE_NORMAL,
              reg_mode_req_mode_pref_get(),
              reg_mode_cs_for_request_get(),
              reg_mode_ps_for_request_get(),
              reg_mode_req_service_domain_get()
#ifdef FEATURE_GSM_BAND_PREF
              ,reg_mode_band_pref_get()
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
              ,reg_mode_rat_pri_list_get()
#endif
#ifdef FEATURE_LTE
              ,reg_mode_sms_only_get(),
               0
#endif 
            );
            next_state_p = &reg_state_registering;
          }
        }
        break;
        
      case MMGSDI_USIM_RPLMNACT:

        if(reg_state_service_status_get() == SYS_SRV_STATUS_SRV)
        {
#ifndef FEATURE_RAT_PRIORITY_LIST        
          if(((reg_state_active_rat_get() == SYS_RAT_GSM_RADIO_ACCESS) &&
              (reg_sim_read_rplmn_rat_search_order(reg_mode_req_mode_pref_get()) == 
               SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED)) ||
             ((reg_state_active_rat_get() == SYS_RAT_UMTS_RADIO_ACCESS) &&
              (reg_sim_read_rplmn_rat_search_order(reg_mode_req_mode_pref_get()) == 
               SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED)))
#else
          reg_sim_read_rplmn_rat_search_order(&rat_priority_list);
          if(rat_priority_list.rat_list[0] != reg_state_active_rat_get())
#endif /*FEATURE_RAT_PRIORITY_LIST*/
          {
            reg_mode_start_plmn_selection
            (
              reg_mode_network_selection_mode_get(),
              reg_mode_req_mode_pref_get(),
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
            next_state_p = &reg_state_registering;
          }
        }
        break;
        
        case MMGSDI_USIM_LOCI:
        case MMGSDI_USIM_PSLOCI:
#ifdef FEATURE_GSTK
          reg_state_set_gstk_event_flag (TRUE);
          MSG_HIGH_DS_0(REG_SUB, "=REG= Setting reg need to send gstk event flag, due to RPLMN Refresh");
#endif
          break;
      default:
        MSG_HIGH_DS_1(REG_SUB, "=REG= Wrong FCN Received file - %d", msg_p->file_id);
        break;
    }

  return next_state_p;
}

#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
/*==============================================================================

FUNCTION NAME

  process_cm_hplmn_search_req

==============================================================================*/

static reg_state_p_type process_cm_hplmn_search_req
(
  const cm_hplmn_search_req_s_type* msg_p
)
{
  reg_cmd_type* reg_cmd_p;

  (void) msg_p;
  reg_state_idle_prev_nw_sel_mode = SYS_NETWORK_SELECTION_MODE_NONE;
  
  /*
  ** Ignore the HPLMN search if initiated in LIMITED mode.
  */
  if ((reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
      (reg_mode_network_selection_mode_get() != SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Ignore: HPLMN Search in LIMITED/MANUAL MODE");

    reg_send_cm_hplmn_search_cnf(FALSE);
    return (&reg_state_idle);
  }
  
  /*
  ** N/W selection mode will be changed to AUTOMATIC
  ** if HPLMN search is initiated in MANUAL mode.
  ** This requirement is droped. Saved to indicate the hplmn search initiated by user.
  */
  reg_state_idle_prev_nw_sel_mode = reg_mode_network_selection_mode_get();
  
  /*
  ** Enqueue HPLMN Search timer to initiate the HPLMN search.
  ** Get a command buffer for the REG task.
  */
  reg_cmd_p = reg_get_cmd_buf();

  if(reg_cmd_p == NULL)
  {
    MSG_FATAL_DS(REG_SUB, "=REG= ERR:Unable to allocate the meomory", 0, 0, 0);
  }

  /*
  ** Construct the timer command.
  */
  reg_cmd_p->cmd.timer_expiry.IMH.message_set = MS_TIMER;
  reg_cmd_p->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

  reg_cmd_p->cmd.timer_expiry.IMH.message_len_lsb =
    (sizeof(timer_expiry_T) - sizeof(IMH_T)) % 256;
  reg_cmd_p->cmd.timer_expiry.IMH.message_len_msb =
    (sizeof(timer_expiry_T) - sizeof(IMH_T)) / 256;

  reg_cmd_p->cmd.timer_expiry.data     = 0;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if(reg_as_id == SYS_MODEM_AS_ID_1)
  {
    reg_cmd_p->cmd.timer_expiry.timer_id = REG_HPLMN_TIMER;
  }
  else if(reg_as_id == SYS_MODEM_AS_ID_2)
  {
    reg_cmd_p->cmd.timer_expiry.timer_id = REG_HPLMN_TIMER_SIM_ID_2;
  }
#if defined FEATURE_TRIPLE_SIM || defined FEATURE_SGLTE_DUAL_SIM
  else if(reg_as_id == SYS_MODEM_AS_ID_3)
  {
    reg_cmd_p->cmd.timer_expiry.timer_id = REG_HPLMN_TIMER_SIM_ID_3; 
  }
#endif
#else
  reg_cmd_p->cmd.timer_expiry.timer_id = REG_HPLMN_TIMER;
#endif

  /*
  ** Put on REG command queue
  */
  reg_put_cmd(reg_cmd_p);

  return (&reg_state_idle);
}
#endif

/*==============================================================================

FUNCTION NAME

  process_hplmn_irat_search_timer_expiration

==============================================================================*/
static reg_state_p_type process_hplmn_irat_search_timer_expiration
(
  void
)
{
  sys_radio_access_tech_e_type rat = SYS_RAT_GSM_RADIO_ACCESS;
  reg_state_p_type             next_state_p = &reg_state_idle;
  sys_rat_pri_list_info_s_type *rat_pri_list_info_ptr, rat_pri_list_info;

  sys_plmn_rat_list_s_type*     list = NULL;
  sys_scan_scope_e_type scan_scope = SYS_SCAN_SCOPE_FULL_BAND;
  boolean efs_or_ehplmn_list_from_nv = FALSE;

  reg_sim_plmn_list_s_type*     efs_or_ehplmn_list = NULL;
#ifndef FEATURE_EQUIVALENT_HPLMN
  sys_plmn_id_s_type            home_plmn ;
#endif
  uint32 i, j;
   
  boolean cell_not_present_in_lists;

  cell_not_present_in_lists = reg_state_cell_not_present_in_lte_lists();

  reg_state_set_lte_search_requested(FALSE);


  if ( ((reg_nv_w_bplmn_pseudo_enabled()
#ifdef FEATURE_TDSCDMA
      || reg_nv_is_forced_irat_enabled()
#endif
       )
#ifdef FEATURE_SGLTE
      &&
      (! reg_sglte_mode))
       ||
       ((reg_sglte_mode && (reg_state_active_rat_get() == SYS_RAT_TDS_RADIO_ACCESS))) /* SGLTE state 2 condition */
       ||
       ((IS_UE_IN_SGLTE_STATE_4 && (reg_state_get_plmn_service_capability ((sys_modem_as_id_e_type)reg_cs_stack_id) == SYS_SRV_CAPABILITY_CS_PS_SERVICE)) /* SGLTE state 4 condition */
#endif
     ))  
  {
      /*The below condition checkes the following, be careful before editing it
             1. Return immediately if LTE or TDS is not present in rat priority list(no rat is there to be searched)
             2. Return if cell is present in list ( avaiable or not avaiable) and active rat is TDS(this means that LTE irat timer is running for LTE search)
             3. If cell is present in avaiable list and active rat is GSM then return if mode is CSFB
      */
      rat_pri_list_info_ptr = reg_mode_rat_pri_list_get();

      if((!( reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS, rat_pri_list_info_ptr, FALSE)) &&
           (!reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_TDS_RADIO_ACCESS, rat_pri_list_info_ptr, FALSE))) ||
             ( cell_not_present_in_lists == FALSE  
#ifdef FEATURE_SGLTE
                && (reg_state_active_rat_get() == SYS_RAT_TDS_RADIO_ACCESS || (!reg_sglte_mode))
#endif
        ))
      {
          reg_state_hplmn_irat_timer_count = 0;
          return next_state_p;
      }

      switch (reg_mode_network_selection_mode_get())
      {

      case SYS_NETWORK_SELECTION_MODE_LIMITED:
          return next_state_p;
          break;

      default:

          if (reg_state_service_status_get() == SYS_SRV_STATUS_SRV)
          {
              /* Check if UE is camped on a RAT for which further processing
              ** should proceed
              */
              rat = reg_state_active_rat_get();
              if((cell_not_present_in_lists && rat != SYS_RAT_TDS_RADIO_ACCESS) && ( rat != SYS_RAT_GSM_RADIO_ACCESS) && ( rat != SYS_RAT_UMTS_RADIO_ACCESS))
              {
                  return next_state_p;
              } 
              /* At this stage, UE is in full service on TDSCDMA or GSM.
              ** Determine if the PLMN matches one of the PLMNs stored in 
              ** the EFS EHPLMN by checking the roaming ind
              */

              if (reg_state_roaming_ind_get() == SYS_ROAM_STATUS_ON)
              {
                  /* Not camped on EHPLMN, return*/
                  return next_state_p;
              }
              /* All conditions are met. Prepare the rat priority list accordingly */
              memset(&rat_pri_list_info, 0, sizeof(rat_pri_list_info));
              /* Search for LTE is allowed for legacy case and for SGLTE state 2*/
              if ( ((reg_nv_w_bplmn_pseudo_enabled()
#ifdef FEATURE_TDSCDMA
                   || reg_nv_is_forced_irat_enabled()
#endif
                    )
#ifdef FEATURE_SGLTE
                   &&
                   ((!REG_SUB_IS_SGLTE || ! reg_sglte_mode))) 
                  ||
                  (REG_SUB_IS_SGLTE && reg_sglte_mode 
                    && (cell_not_present_in_lists && reg_state_active_rat_get() == SYS_RAT_TDS_RADIO_ACCESS) /* SGLTE state 2 condition */
#endif
              ))              
              {
                /* Copy the LTE related information from the rat priority list*/
                for(i = 0; i < rat_pri_list_info_ptr->num_items; i++)
                {
                    if (rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
                    {
                        rat_pri_list_info.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_LTE;
                        rat_pri_list_info.priority_list_info[0].band_cap = 
                            rat_pri_list_info_ptr->priority_list_info[i].band_cap;
                        rat_pri_list_info.priority_list_info[0].bst_band_cap = 
                            rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap;
                        rat_pri_list_info.num_items = 1;
                        break;
                    }
                }
              }
              else
              {
                 /* This must be SGLTE state 4*/
                  /* Copy the LTE and TD related information from the rat priority list*/
                  for(i = 0; i < rat_pri_list_info_ptr->num_items; i++)
                  {
                      if (rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE )
                      {
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].acq_sys_mode = SYS_SYS_MODE_LTE;
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].band_cap = rat_pri_list_info_ptr->priority_list_info[i].band_cap;
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].bst_band_cap = rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap;
                          rat_pri_list_info.num_items++;
                      }
                      else if (rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_TDS)
                      {
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].acq_sys_mode = SYS_SYS_MODE_TDS;
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].band_cap = rat_pri_list_info_ptr->priority_list_info[i].band_cap;
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].bst_band_cap = rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap;
                          rat_pri_list_info.num_items++;
                      }
                  }
              }
              
#ifdef FEATURE_MODEM_HEAP
              list = (sys_plmn_rat_list_s_type *)modem_mem_alloc(sizeof(sys_plmn_rat_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
              list = (sys_plmn_rat_list_s_type *)gs_alloc(sizeof(sys_plmn_rat_list_s_type));
#endif
              reg_check_for_null_ptr((void*)list );
              list->length = 0;
              /* The TDSCDMA operators are expected to populate the specific list of PLMNs
              ** in the EFS.
              */       
#ifdef FEATURE_TDSCDMA
              if(reg_mode_is_usim_compliant() && (reg_state_active_rat_get() != SYS_RAT_UMTS_RADIO_ACCESS))
              {
                efs_or_ehplmn_list = reg_nv_tdscdma_plmn_list_get();
                ASSERT(efs_or_ehplmn_list);
                efs_or_ehplmn_list->length = MIN (efs_or_ehplmn_list->length, REG_SIM_PLMN_LIST_LENGTH);
                efs_or_ehplmn_list_from_nv = TRUE;
              }
              else if(reg_nv_w_bplmn_pseudo_enabled() && (reg_state_active_rat_get() != SYS_RAT_TDS_RADIO_ACCESS))
              {
#ifdef FEATURE_MODEM_HEAP
              efs_or_ehplmn_list = (reg_sim_plmn_list_s_type *)modem_mem_alloc(sizeof(reg_sim_plmn_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
              efs_or_ehplmn_list = (reg_sim_plmn_list_s_type *)gs_alloc(sizeof(reg_sim_plmn_list_s_type));
#endif
              reg_check_for_null_ptr((void*)efs_or_ehplmn_list );
#ifdef FEATURE_EQUIVALENT_HPLMN
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                 reg_sim_per_nas_stacks_ehplmn_list_get_ptr(efs_or_ehplmn_list, (sys_modem_as_id_e_type)reg_as_id);
#else
                 reg_sim_ehplmn_list_get_ptr(efs_or_ehplmn_list);
#endif 
                 if(efs_or_ehplmn_list->length > SYS_EHPLMN_LIST_MAX_LENGTH)
                 {
                   efs_or_ehplmn_list->length = SYS_EHPLMN_LIST_MAX_LENGTH;
                 }
                 efs_or_ehplmn_list->length = MIN (efs_or_ehplmn_list->length, REG_SIM_PLMN_LIST_LENGTH);
#else
   /* HPLMN */
#if defined FEATURE_DUAL_SIM|| defined FEATURE_SGLTE
                 home_plmn = reg_sim_per_subs_hplmn_get(reg_as_id);
#else
                 home_plmn = reg_sim_hplmn_get(); 
#endif
#endif
              }
              else
              {
                 MSG_ERROR_DS_2(REG_SUB, "=REG= IRAT Search unexpected reg_mode_is_usim_compliant = %d LTE reg_nv_w_bplmn_pseudo_enabled = %d",
                 reg_mode_is_usim_compliant(),
                 reg_nv_w_bplmn_pseudo_enabled());
#ifndef FEATURE_MODEM_HEAP
                 gs_free(list);
#else
                 modem_mem_free(list, MODEM_MEM_CLIENT_NAS);
#endif
                 list = NULL;
                 return next_state_p;
              }
#else
#ifdef FEATURE_MODEM_HEAP
              efs_or_ehplmn_list = (reg_sim_plmn_list_s_type *)modem_mem_alloc(sizeof(reg_sim_plmn_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
              efs_or_ehplmn_list = (reg_sim_plmn_list_s_type *)gs_alloc(sizeof(reg_sim_plmn_list_s_type));
#endif
              reg_check_for_null_ptr((void*)efs_or_ehplmn_list );
#ifdef FEATURE_EQUIVALENT_HPLMN
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
              reg_sim_per_nas_stacks_ehplmn_list_get_ptr(efs_or_ehplmn_list, (sys_modem_as_id_e_type)reg_as_id);
#else
              reg_sim_ehplmn_list_get_ptr(efs_or_ehplmn_list);
#endif
              if(efs_or_ehplmn_list->length > SYS_EHPLMN_LIST_MAX_LENGTH)
              {
                efs_or_ehplmn_list->length = SYS_EHPLMN_LIST_MAX_LENGTH;
              }
              efs_or_ehplmn_list->length = MIN (efs_or_ehplmn_list->length, REG_SIM_PLMN_LIST_LENGTH);

#else
    /* HPLMN */
#if defined FEATURE_DUAL_SIM|| defined FEATURE_SGLTE
              home_plmn = reg_sim_per_subs_hplmn_get(reg_as_id);
#else
              home_plmn = reg_sim_hplmn_get(); 
#endif

#endif

#endif
#if defined FEATURE_EQUIVALENT_HPLMN || defined FEATURE_TDSCDMA
             for(i = 0; (i < efs_or_ehplmn_list->length && list->length < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH); i++)
              {
                for (j = 0; j < rat_pri_list_info.num_items; j++)
                {
                   list->info[list->length].plmn = efs_or_ehplmn_list->plmn[i];
                  if (rat_pri_list_info.priority_list_info[j].acq_sys_mode == SYS_SYS_MODE_TDS)
                  {
                    list->info[list->length].rat = SYS_RAT_TDS_RADIO_ACCESS;
                  }
                  else if( rat_pri_list_info.priority_list_info[j].acq_sys_mode == SYS_SYS_MODE_LTE &&
                            (cell_not_present_in_lists
#ifdef FEATURE_SGLTE
                             || (IS_UE_IN_SGLTE_STATE_4 && reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS, rat_pri_list_info_ptr, FALSE))
#endif
                             )
                           )
                  {
                    reg_state_set_lte_search_requested(TRUE);
                    list->info[list->length].rat = SYS_RAT_LTE_RADIO_ACCESS;
                  }
                  list->length++;
                }
              }
#else
              for (j = 0; j < rat_pri_list_info.num_items; j++)
              {
                list->info[list->length].plmn = home_plmn;
                if( rat_pri_list_info.priority_list_info[j].acq_sys_mode == SYS_SYS_MODE_LTE )
                {
                   reg_state_set_lte_search_requested(TRUE);
                   list->info[list->length].rat = SYS_RAT_LTE_RADIO_ACCESS;
                }
              }
#endif
              for(i = 0; i < rat_pri_list_info.num_items; i++)
              {
                if(rat_pri_list_info.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
                {
#if (LTE_BAND_NUM == 256)
                 MSG_HIGH_DS_5(REG_SUB, "=REG= sys_mode = %d, LTE bands 1_64 : %u,LTE bands 65_128 : %u,LTE bands 129_192 : %u,LTE bands 193_256 : %u",
                   rat_pri_list_info.priority_list_info[i].acq_sys_mode, 
                   rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_1_64,
                   rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_65_128,
                   rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_129_192,
                   rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_193_256);
#else
                 MSG_HIGH_DS_2(REG_SUB, "=REG= sys_mode = %d LTE band_cap = %u",
                     rat_pri_list_info.priority_list_info[i].acq_sys_mode,
                     rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap);
#endif

                }
                else
                {
                  MSG_HIGH_DS_2(REG_SUB, "=REG= sys_mode = %d band_cap = %u",
                     rat_pri_list_info.priority_list_info[i].acq_sys_mode,
                     rat_pri_list_info.priority_list_info[i].band_cap.chgwt_band_cap);
                }
              }
              if(reg_state_hplmn_irat_timer_count == 0 || reg_state_hplmn_irat_timer_count == MAX_HPLMN_IRAT_ACQ_DB_SCAN)
              {
                scan_scope = SYS_SCAN_SCOPE_FULL_BAND;
                reg_state_hplmn_irat_timer_count = 0;
              }
              else
              {
                scan_scope = SYS_SCAN_SCOPE_ACQ_DB;
              }
              reg_state_hplmn_irat_timer_count++;  
              MSG_HIGH_DS_4(REG_SUB, "=REG= HPLMN Search started with RAT length - %d, hpplmn list length - %d reg_state_hplmn_irat_timer_count = %d scan_scope = %d", 
                            rat_pri_list_info.num_items, list->length, reg_state_hplmn_irat_timer_count, scan_scope);

              reg_send_mmr_plmn_search_req
              (
                SYS_NETWORK_SELECTION_MODE_AUTOMATIC, /* Background PLMN search      */
                TRUE,                            /* dual_mode                   */
                rat                                   /* rat                         */
  #ifdef FEATURE_GSM_BAND_PREF
                ,reg_mode_band_pref_get()             /* band_pref                   */
  #endif
                ,list
                ,SYS_SERVICE_SEARCH_IRAT_PLMN
#ifdef FEATURE_FEMTO_CSG
                ,SYS_CSG_SEARCH_TYPE_NONE
#endif              
                ,&rat_pri_list_info
                ,NULL                                 
                ,scan_scope                             
              );
#ifndef FEATURE_MODEM_HEAP
              gs_free(list);
#else
              modem_mem_free(list, MODEM_MEM_CLIENT_NAS);
#endif
              list = NULL;
              if(reg_nv_w_bplmn_pseudo_enabled() && (reg_state_active_rat_get() != SYS_RAT_TDS_RADIO_ACCESS) && !efs_or_ehplmn_list_from_nv)
              {
#ifndef FEATURE_MODEM_HEAP
                gs_free(efs_or_ehplmn_list);
#else
                modem_mem_free(efs_or_ehplmn_list, MODEM_MEM_CLIENT_NAS);
#endif
                efs_or_ehplmn_list = NULL;
              }

              /* Move to another REG state*/
              next_state_p = &reg_state_automatic_search;
          } /* SYS_SRV_STATUS_SRV */
          break;
      }
  }
  return next_state_p;
}
/*==============================================================================

FUNCTION NAME

  process_lte_irat_search_timer_expiration

==============================================================================*/
static reg_state_p_type process_lte_irat_search_timer_expiration
(
  void
)
{
  sys_radio_access_tech_e_type rat = SYS_RAT_GSM_RADIO_ACCESS;
  reg_state_p_type             next_state_p = &reg_state_idle;
  sys_rat_pri_list_info_s_type *rat_pri_list_info_ptr, rat_pri_list_info;

  sys_plmn_rat_list_s_type*     list = NULL;
  reg_sim_plmn_list_s_type*     efs_or_ehplmn_list = NULL;
  boolean efs_or_ehplmn_list_from_nv = FALSE;
  
#ifndef FEATURE_EQUIVALENT_HPLMN
  sys_plmn_id_s_type			  home_plmn ;
#endif
  uint32 i, j;
  boolean cell_not_present_in_lists = FALSE;

  if(!reg_state_get_force_bplmn_search_start())
  {
    cell_not_present_in_lists = reg_state_cell_not_present_in_lte_lists();
  }
  if(cell_not_present_in_lists && !(reg_state_get_force_irat_search_initiated()))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= LTE IRAT BPLMN : cell not present in any of the LTE lists, ignoring expiry");
    return next_state_p;
  }
  else
  {
    if (((reg_nv_w_bplmn_pseudo_enabled()
#ifdef FEATURE_TDSCDMA
      || reg_nv_is_forced_irat_enabled()
#endif
       )
#ifdef FEATURE_SGLTE
      &&
      (! reg_sglte_mode))
       ||
       ((reg_sglte_mode && (reg_state_active_rat_get() == SYS_RAT_TDS_RADIO_ACCESS))) /* SGLTE state 2 condition */
       ||
       ((IS_UE_IN_SGLTE_STATE_4 && (reg_state_get_plmn_service_capability ((sys_modem_as_id_e_type)reg_cs_stack_id) == SYS_SRV_CAPABILITY_CS_PS_SERVICE)) /* SGLTE state 4 condition */
#endif
     ))  
  {
      reg_state_set_lte_search_requested(FALSE);
      /* Return immediately if there is no LTE or TDS in rat priority list */
      rat_pri_list_info_ptr = reg_mode_rat_pri_list_get();

      if (!reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS, rat_pri_list_info_ptr, FALSE))
      {
          return next_state_p;
      }

      switch (reg_mode_network_selection_mode_get())
      {

      case SYS_NETWORK_SELECTION_MODE_LIMITED:
          return next_state_p;
          break;

      default:

          if (reg_state_service_status_get() == SYS_SRV_STATUS_SRV)
          {
              /* Check if UE is camped on a RAT for which further processing
              ** should proceed
              */
              rat = reg_state_active_rat_get();
              if((rat != SYS_RAT_TDS_RADIO_ACCESS) && (rat != SYS_RAT_GSM_RADIO_ACCESS) && (rat != SYS_RAT_UMTS_RADIO_ACCESS))
              {
                  return next_state_p;
              } 
              /* At this stage, UE is in full service on TDSCDMA or GSM.
              ** Determine if the PLMN matches one of the PLMNs stored in 
              ** the EFS EHPLMN by checking the roaming ind
              */

              if (reg_state_roaming_ind_get() == SYS_ROAM_STATUS_ON)
              {
                  /* Not camped on EHPLMN, return*/
                  return next_state_p;
              }
              /* All conditions are met. Prepare the rat priority list accordingly */
              memset(&rat_pri_list_info, 0, sizeof(rat_pri_list_info));
              /* Search for LTE is allowed for legacy case and for SGLTE state 2*/
              if (((reg_nv_w_bplmn_pseudo_enabled()
#ifdef FEATURE_TDSCDMA
                   || reg_nv_is_forced_irat_enabled()
#endif
                    )
#ifdef FEATURE_SGLTE
                   &&
                   ((!REG_SUB_IS_SGLTE || ! reg_sglte_mode))) 
                  ||
                  (REG_SUB_IS_SGLTE && reg_sglte_mode 
                    && (reg_state_active_rat_get() == SYS_RAT_TDS_RADIO_ACCESS) /* SGLTE state 2 condition */
#endif
              ))              
              {
                /* Copy the LTE related information from the rat priority list*/
                for(i = 0; i < rat_pri_list_info_ptr->num_items; i++)
                {
                    if (rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
                    {
                        rat_pri_list_info.priority_list_info[0].acq_sys_mode = SYS_SYS_MODE_LTE;
                        rat_pri_list_info.priority_list_info[0].band_cap = 
                            rat_pri_list_info_ptr->priority_list_info[i].band_cap;
                        rat_pri_list_info.priority_list_info[0].bst_band_cap = 
                            rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap;
                        rat_pri_list_info.num_items = 1;
                        break;
                    }
                }
              }
              else
              {
                 /* This must be SGLTE state 4*/
                  /* Copy the LTE and TD related information from the rat priority list*/
                  for(i = 0; i < rat_pri_list_info_ptr->num_items; i++)
                  {
                      if (rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
                      {
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].acq_sys_mode = SYS_SYS_MODE_LTE;
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].band_cap = rat_pri_list_info_ptr->priority_list_info[i].band_cap;
                          rat_pri_list_info.priority_list_info[rat_pri_list_info.num_items].bst_band_cap = rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap;
                          rat_pri_list_info.num_items++;
                      }
                  }
              }
              
           
#ifdef FEATURE_MODEM_HEAP
			  list = (sys_plmn_rat_list_s_type *)modem_mem_alloc(sizeof(sys_plmn_rat_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
			  list = (sys_plmn_rat_list_s_type *)gs_alloc(sizeof(sys_plmn_rat_list_s_type));
#endif
			  if(list == NULL)
			  {
			    MSG_FATAL_DS(REG_SUB, "=REG= Error: Not able to Allocate list Buffer", 0, 0, 0);
			  }
			  list->length = 0;          
              /* The TDSCDMA operators are expected to populate the specific list of PLMNs
              ** in the EFS.
              */       
#ifdef FEATURE_TDSCDMA
             if(reg_mode_is_usim_compliant() && (reg_state_active_rat_get() != SYS_RAT_UMTS_RADIO_ACCESS))
             {
               efs_or_ehplmn_list = reg_nv_tdscdma_plmn_list_get();
               ASSERT(efs_or_ehplmn_list);
               efs_or_ehplmn_list->length = MIN (efs_or_ehplmn_list->length, REG_SIM_PLMN_LIST_LENGTH);
               efs_or_ehplmn_list_from_nv = TRUE;
             }
             else if(reg_nv_w_bplmn_pseudo_enabled() && (reg_state_active_rat_get() != SYS_RAT_TDS_RADIO_ACCESS))
             {
#ifdef FEATURE_MODEM_HEAP
                efs_or_ehplmn_list = (reg_sim_plmn_list_s_type *)modem_mem_alloc(sizeof(reg_sim_plmn_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
                efs_or_ehplmn_list = (reg_sim_plmn_list_s_type *)gs_alloc(sizeof(reg_sim_plmn_list_s_type));
#endif
                reg_check_for_null_ptr((void*)efs_or_ehplmn_list );

#ifdef FEATURE_EQUIVALENT_HPLMN
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                reg_sim_per_nas_stacks_ehplmn_list_get_ptr(efs_or_ehplmn_list, (sys_modem_as_id_e_type)reg_as_id);
#else
                reg_sim_ehplmn_list_get_ptr(efs_or_ehplmn_list);
#endif 
                if(efs_or_ehplmn_list->length > SYS_EHPLMN_LIST_MAX_LENGTH)
                {
                  efs_or_ehplmn_list->length = SYS_EHPLMN_LIST_MAX_LENGTH;
                }
                efs_or_ehplmn_list->length = MIN (efs_or_ehplmn_list->length, REG_SIM_PLMN_LIST_LENGTH);
#else
       /* HPLMN */
#if defined FEATURE_DUAL_SIM|| defined FEATURE_SGLTE
                home_plmn = reg_sim_per_subs_hplmn_get(reg_as_id);
#else
                home_plmn = reg_sim_hplmn_get(); 
#endif
#endif
              }
              else
              {
                 MSG_ERROR_DS_2(REG_SUB, "=REG= IRAT Search unexpected reg_mode_is_usim_compliant = %d LTE reg_nv_w_bplmn_pseudo_enabled = %d",
                                          reg_mode_is_usim_compliant(),
                                          reg_nv_w_bplmn_pseudo_enabled());
#ifndef FEATURE_MODEM_HEAP
                 gs_free(list);
#else
                 modem_mem_free(list, MODEM_MEM_CLIENT_NAS);
#endif
                 list = NULL;
                 return next_state_p;
              }
#else
#ifdef FEATURE_MODEM_HEAP
              efs_or_ehplmn_list = (reg_sim_plmn_list_s_type *)modem_mem_alloc(sizeof(reg_sim_plmn_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
              efs_or_ehplmn_list = (reg_sim_plmn_list_s_type *)gs_alloc(sizeof(reg_sim_plmn_list_s_type));
#endif
              ASSERT(efs_or_ehplmn_list);

#ifdef FEATURE_EQUIVALENT_HPLMN
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
              reg_sim_per_nas_stacks_ehplmn_list_get_ptr(efs_or_ehplmn_list, (sys_modem_as_id_e_type)reg_as_id);
#else
              reg_sim_ehplmn_list_get_ptr(efs_or_ehplmn_list);
#endif
              ASSERT(efs_or_ehplmn_list);
              if(efs_or_ehplmn_list->length > SYS_EHPLMN_LIST_MAX_LENGTH)
              {
                 efs_or_ehplmn_list->length = SYS_EHPLMN_LIST_MAX_LENGTH;
              }
              efs_or_ehplmn_list->length = MIN (efs_or_ehplmn_list->length, REG_SIM_PLMN_LIST_LENGTH);
#else
  /* HPLMN */
#if defined FEATURE_DUAL_SIM|| defined FEATURE_SGLTE
              home_plmn = reg_sim_per_subs_hplmn_get(reg_as_id);
#else
              home_plmn = reg_sim_hplmn_get(); 
#endif
#endif
#endif
#if defined FEATURE_EQUIVALENT_HPLMN || defined FEATURE_TDSCDMA
              for(i = 0; (i < efs_or_ehplmn_list->length && list->length < SYS_PRIORITY_PLMN_LIST_MAX_LENGTH); i++)
              {
                for (j = 0; j < rat_pri_list_info.num_items; j++)
              {
                  list->info[list->length].plmn = efs_or_ehplmn_list->plmn[i];
                  if (rat_pri_list_info.priority_list_info[j].acq_sys_mode == SYS_SYS_MODE_LTE)
                  {
                    reg_state_set_lte_search_requested(TRUE);
                    list->info[list->length].rat = SYS_RAT_LTE_RADIO_ACCESS;
                  }
                  list->length++;
                }
              }
#else
              for (j = 0; j < rat_pri_list_info.num_items; j++)
              {
                 list->info[list->length].plmn = home_plmn;
                 if( rat_pri_list_info.priority_list_info[j].acq_sys_mode == SYS_SYS_MODE_LTE )
                 {
              reg_state_set_lte_search_requested(TRUE);
                    list->info[list->length].rat = SYS_RAT_LTE_RADIO_ACCESS;
                 }
              }
#endif

              MSG_HIGH_DS_2(REG_SUB, "=REG= LTE IRAT BPLMN: LTE IRAT Search started with RAT length - %d, hpplmn list length - %d", 
                            rat_pri_list_info.num_items, list->length);
              for(i = 0; i < rat_pri_list_info.num_items; i++)
              {
                if(rat_pri_list_info.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
                {
#if (LTE_BAND_NUM == 256)
                  MSG_HIGH_DS_5(REG_SUB, "=REG= sys_mode = %d, LTE bands 1_64: 0x%08x %08x,  LTE bands 65_128: 0x%08x %08x",
                           rat_pri_list_info.priority_list_info[i].acq_sys_mode,
                           QWORD_HIGH(rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_1_64),
                           QWORD_LOW(rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_1_64),
                           QWORD_HIGH(rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_65_128),
                           QWORD_LOW(rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_65_128));
                  MSG_HIGH_DS_4(REG_SUB, "=REG= LTE bands 129_192: 0x%08x %08x,LTE bands 193_256: 0x%08x %08x",
                           QWORD_HIGH(rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_129_192),
                           QWORD_LOW(rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_129_192),
                           QWORD_HIGH(rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_193_256),
                           QWORD_LOW(rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap.bits_193_256));
#else
                  MSG_HIGH_DS_2(REG_SUB, "=REG= sys_mode = %d LTE band_cap = %u",
                     rat_pri_list_info.priority_list_info[i].acq_sys_mode,
                     rat_pri_list_info.priority_list_info[i].band_cap.lte_band_cap);
#endif

                }
                else
                {
                  MSG_HIGH_DS_2(REG_SUB, "=REG= sys_mode = %d band_cap = 0x%lx",
                     rat_pri_list_info.priority_list_info[i].acq_sys_mode,
                     rat_pri_list_info.priority_list_info[i].band_cap.chgwt_band_cap);
                }
              }
 
              reg_send_mmr_plmn_search_req
              (
                SYS_NETWORK_SELECTION_MODE_AUTOMATIC, /* Background PLMN search      */
                TRUE,                            /* dual_mode                   */
                rat                                   /* rat                         */
  #ifdef FEATURE_GSM_BAND_PREF
                ,reg_mode_band_pref_get()             /* band_pref                   */
  #endif
                ,list
                ,SYS_SERVICE_SEARCH_IRAT_PLMN
#ifdef FEATURE_FEMTO_CSG
                ,SYS_CSG_SEARCH_TYPE_NONE
#endif              
                ,&rat_pri_list_info
                ,NULL
                ,  SYS_SCAN_SCOPE_FULL_BAND                             
              );

              /* Move to another REG state*/
              next_state_p = &reg_state_automatic_search;
          } /* SYS_SRV_STATUS_SRV */
          break;
      }
  }
}

#ifndef FEATURE_MODEM_HEAP
	gs_free(list);
#else
	modem_mem_free(list, MODEM_MEM_CLIENT_NAS);
#endif
	list = NULL;
  if(reg_nv_w_bplmn_pseudo_enabled() && (reg_state_active_rat_get() != SYS_RAT_TDS_RADIO_ACCESS) && !efs_or_ehplmn_list_from_nv)
    {
#ifndef FEATURE_MODEM_HEAP
      gs_free(efs_or_ehplmn_list);
#else
      modem_mem_free(efs_or_ehplmn_list, MODEM_MEM_CLIENT_NAS);
#endif
      efs_or_ehplmn_list = NULL;
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

  reg_state_p_type             next_state_p = &reg_state_idle;

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

  /* Stop the timer */
  reg_timers_stop_hplmn_irat_search_period_timer();  
  if(timer_val == 0)
  {
      /* Note that setting the timer to this value will prevent starting it */
    timer_val = REG_TIMERS_MAX_TIMER_VALUE;    
      /* Update the timer NV */
      reg_nv_hplmn_irat_search_timer_set(timer_val);
    if(irat_timer_allowed)
    {
        /* Issue an immediate search request.Note that in the case of falling 
        ** back to T or G rat, the timer will not be restarted
        */
        next_state_p = process_hplmn_irat_search_timer_expiration();
    }
  }
  else
  {
    /* Update the timer NV */
    reg_nv_hplmn_irat_search_timer_set(timer_val);
    if(irat_timer_allowed)
  {
        /* Issue an immediate search request. Note that in the case of falling 
        ** back to T or G rat, the timer will be restarted with the new value
        */
        next_state_p = process_hplmn_irat_search_timer_expiration();
      }
  }

  return next_state_p;
}




#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  process_backoff_fplmn_timer_expiration

==============================================================================*/
static reg_state_p_type process_backoff_fplmn_timer_expiration
(
  void
)
{
  reg_sim_handle_backoff_fplmn_search_timer_expiry();
  if(reg_mode_reenable_lte())
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Initiate PLMN selection to enable LTE RAT capability");
    reg_mode_start_plmn_selection_on_specified_plmn
    (
      reg_state_service_plmn_get(),
      reg_mode_network_selection_mode_get(),
      reg_mode_mode_pref_get(),
      FALSE,
      FALSE,
      reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
      reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
      reg_mode_curr_rat_pri_list_get(),
#endif
      reg_state_active_rat_get(),
#ifdef FEATURE_LTE
      reg_mode_sms_only_get(),
       0,
#endif 
      MMR_REG_REQ_NORMAL
    );
    return &reg_state_registering;
  }
  return &reg_state_idle;
}


/*==============================================================================

FUNCTION NAME

  process_cm_block_plmn_req

==============================================================================*/
static reg_state_p_type process_cm_block_plmn_req
(
  const cm_block_plmn_req_s_type *msg_p
)
{
  reg_state_p_type    next_state_p = &reg_state_idle;
  boolean            plmn_id_is_undefined;
  boolean            mnc_includes_pcs_digit;
  uint32             mcc;
  uint32             mnc;  
  sys_plmn_get_mcc_mnc
  (
    msg_p->plmn,
    &plmn_id_is_undefined,
    &mnc_includes_pcs_digit,
    &mcc,
    &mnc
  );
#ifdef FEATURE_DUAL_SIM
  if (reg_multimode_sub != msg_p->as_id)
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= PLMN blocking ignored on non mutimode SUB, multimode SUB %d", reg_multimode_sub);
    return next_state_p;
  }
#endif
  MSG_HIGH_DS_2(REG_SUB, "=REG= PLMN ID(%d-%d)", mcc, mnc);
  MSG_HIGH_DS_2(REG_SUB, "=REG= type = %d timer count = %d",msg_p->type, msg_p->timer_count);

  switch(msg_p->type)
  {
    case SYS_BLOCK_PLMN_BLOCK:
      reg_send_mmr_block_plmn_req(msg_p->plmn, msg_p->timer_count, FALSE,msg_p->cause); 
      break;

    case SYS_BLOCK_PLMN_UNBLOCK:
      if (reg_sim_remove_plmn_from_backoff_fplmn_list(msg_p->plmn,msg_p->cause,(sys_modem_as_id_e_type)reg_sub_id))
      {
        reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
        if(reg_mode_reenable_lte())
        {
          MSG_HIGH_DS_0(REG_SUB, "=REG= Initiate PLMN selection to enable LTE RAT capability");
          reg_mode_start_plmn_selection_on_specified_plmn
          (
            reg_state_service_plmn_get(),
            reg_mode_network_selection_mode_get(),
            reg_mode_mode_pref_get(),
            FALSE,
            FALSE,
            reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
            reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
            reg_mode_curr_rat_pri_list_get(),
#endif
            reg_state_active_rat_get(),
#ifdef FEATURE_LTE
            reg_mode_sms_only_get(),
             0,
#endif 
            MMR_REG_REQ_NORMAL
          );
          next_state_p = &reg_state_registering;
        }
      }
      break;

    case SYS_BLOCK_PLMN_UNBLOCK_FORCE_PREF:
      if (reg_sim_remove_plmn_from_backoff_fplmn_list(msg_p->plmn,SYS_BLOCK_PLMN_CAUSE_PRIORITY,
                                                      (sys_modem_as_id_e_type)reg_sub_id))
      {
        rex_timer_cnt_type timer_count = 0;
        sys_block_plmn_cause_e_type cause = SYS_BLOCK_PLMN_CAUSE_NONE;
        if(reg_mode_check_eplmn_backoff_forbidden(&msg_p->plmn,&timer_count,&cause,(sys_modem_as_id_e_type)reg_sub_id))
        {
          reg_mode_remove_eplmn_rplmn_backoff_forbidden(&msg_p->plmn,cause);
        }
        reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
        if(((reg_mode_is_only_req_rat_present_in_rat_pri_list(reg_mode_curr_rat_pri_list_get(),SYS_SYS_MODE_LTE) == TRUE )&&
            (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION || 
             reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)) ||
             reg_mode_reenable_lte())
        {
          reg_sim_write_rplmn_rat(SYS_RAT_NONE);
          MSG_HIGH_DS_0(REG_SUB, "=REG= Initiate PLMN selection to start plmn selection all over again from LTE");
          if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_AUTOMATIC  ||
             reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)
          {
             reg_mode_start_plmn_selection
             (
                reg_mode_network_selection_mode_get(),
                reg_mode_req_mode_pref_get(),
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
             reg_state_force_pref_pending = FALSE;
             next_state_p = &reg_state_registering;
          }
          else if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL || 
                  reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
          {
            reg_mode_start_manual_plmn_selection
            (
              reg_mode_network_selection_mode_get(),
              reg_send_last_reg_req_plmn_get(),
              reg_mode_mode_pref_get(),
              FALSE,
              FALSE,
              reg_mode_req_service_domain_get(),
#ifdef FEATURE_GSM_BAND_PREF
              reg_mode_band_pref_get(),
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
              reg_mode_rat_pri_list_get(),
#endif
              REG_CM_SERVICE_REQ_NORMAL
#ifdef FEATURE_LTE 
              ,reg_mode_sms_only_get(),
              0
#endif                 
             );
             reg_state_force_pref_pending = FALSE;
             next_state_p = &reg_state_registering;
           } 
           reg_state_update_service_state_if_required(next_state_p);
         }
         else
         { 
            reg_state_force_pref_pending = TRUE;
         }

      }
      break;

    case SYS_BLOCK_PLMN_RESET:
      if(reg_sim_backoff_fplmn_list.length != 0)
      {
        reg_sim_clear_backoff_fplmn_list();
        reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
      }
      break;

    default:
      break;
  }
  return next_state_p;
}

#endif


#ifdef FEATURE_SGLTE
static reg_state_p_type process_sglte_ps_timer_expiry(void)
{
  reg_as_id_e_type reg_other_as_id;
  reg_other_as_id = ((reg_as_id == reg_sub_id) ? REG_SGLTE_STACK_2: reg_sub_id);
  if( REG_SGLTE_DUAL_STACK &&
      reg_state_get_service_status_per_subs((sys_modem_as_id_e_type)reg_other_as_id) == SYS_SRV_STATUS_SRV && 
      reg_state_get_service_status_per_subs((sys_modem_as_id_e_type)reg_as_id) != SYS_SRV_STATUS_SRV  && 
      reg_state_get_service_domain_per_subs((sys_modem_as_id_e_type)reg_other_as_id) == SYS_SRV_DOMAIN_CS_ONLY &&
      reg_state_get_plmn_service_capability((sys_modem_as_id_e_type)reg_other_as_id) == SYS_SRV_CAPABILITY_CS_PS_SERVICE &&
      reg_state_get_active_rat_per_subs((sys_modem_as_id_e_type)reg_other_as_id) == SYS_RAT_GSM_RADIO_ACCESS &&
      /*Adding CS stack id to get PLMN , becuase during split process finished or dual stack mode
       CS stack PLMN is to check for forbidden PLMN */
#ifdef FEATURE_DUAL_SIM
      (reg_sim_plmn_gprs_forbidden(reg_state_service_plmn_get_per_subs((sys_modem_as_id_e_type)reg_other_as_id), (sys_modem_as_id_e_type)reg_sub_id) == FALSE )
#else
      (reg_sim_plmn_gprs_forbidden(reg_state_service_plmn_get_per_subs(reg_other_as_id))==FALSE )
#endif
    )
  {

#ifdef FEATURE_SGLTE_DUAL_SIM
      if (! mm_get_verify_ps_transfer_conditions((sys_modem_as_id_e_type)reg_other_as_id))
      {
          /* Call must be pending, start the timer instead*/
          MSG_HIGH_DS_0(REG_SUB, "=REG= Call pending, start PS timer"); 
          reg_timers_start_sglte_ps_timer_val (5000);
          return &reg_state_idle;
      }
#endif           

    reg_send_ps_timer_expiry();
  }
  else
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SGLTE PS Timer - Unexpected");
  }
  return &reg_state_idle;
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
  reg_state_p_type next_state_p = &reg_state_idle;
  sys_csg_info_s_type  csg_info = reg_state_csg_info_get();
  reg_sim_set_csg_list_read_complete(TRUE);

  if((reg_state_power_on_csg_search == TRUE) && 
     (reg_state_service_status_get() == SYS_SRV_STATUS_SRV))
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
  return (&reg_state_idle);
}
/*==============================================================================

FUNCTION NAME

  number

==============================================================================*/

static reg_state_e_type number ( void )
{
  return REG_STATE_IDLE;
}


/*==============================================================================

FUNCTION NAME

  output_name

==============================================================================*/

static void output_name ( void )
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= REG_STATE_IDLE");
}


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

reg_state_s_type reg_state_idle = 
{
  reg_state_process_cm_network_list_req,
  reg_state_process_cm_service_req,
  reg_state_process_cm_sim_available_req,
  reg_state_process_cm_sim_not_available_req,
  reg_state_process_cm_stop_mode_req,
#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
  process_cm_hplmn_search_req,
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
  reg_state_process_mmr_service_ind,
  reg_state_unexpected_mmr_sim_available_cnf,
  reg_state_unexpected_mmr_sim_not_available_cnf,
  process_mmr_stop_mode_cnf,
  process_hplmn_search_timer_expiration,
  reg_state_process_mmr_emergency_num_list_ind,
  process_sim_refresh_fcn_ind,
  reg_state_unexpected_ph_status_change_cnf,
  reg_state_process_mmr_reg_reject_ind,
  process_hplmn_irat_search_timer_expiration,
  process_cm_set_hplmn_irat_timer,
  reg_state_process_t3245_expiry,
#ifdef FEATURE_LTE
  process_backoff_fplmn_timer_expiration,
  process_cm_block_plmn_req,
  reg_state_process_update_lte_cap_timer_expiration,
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
