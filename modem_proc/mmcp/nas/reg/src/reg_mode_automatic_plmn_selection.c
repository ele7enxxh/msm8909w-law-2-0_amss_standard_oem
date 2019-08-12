/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_mode_automatic_plmn_selection.c_v   1.2   06 Jun 2002 10:48:10   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_mode_automatic_plmn_selection.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/29/02   kwa     Changes to support sending a single RAT rather than a mode
                   preference in the mmr_reg_req message.
05/31/02   kwa     Added RPLMN in call to save equivalent PLMN list.
06/27/02   kwa     Modified function that sends an mmr_reg_req due to
                   interface change.
09/23/02   kwa     Added code to determine the allowed service domain.
10/18/02   kwa     Added FEATURE_REG_DEEP_SLEEP.
10/28/02   kwa     Added call to reg_send_mmr_stop_mode_req with reason
                   SYS_STOP_MODE_REASON_DEEP_SLEEP.
11/02/02   kwa     Added code to send cm_deep_sleep_ind only after the plmn
                   search period timer has reached its maximum value.
05/21/03   kwa     Restructured code to eliminate lint warnings.
06/04/03   kwa     Fixed one remaining lint warning.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/18/03   kwa     Added sys_band_mask_type to start_automatic_plmn_selection.
10/22/03   kwa     Removed the parameters update_equivalent_plmn_list and
                   equivalent_plmn_list_p from the function finished.
11/19/03   kwa     Removed obsolete function for retrieving the mode name.
01/20/04   kwa     Added unexpected function handler
                   reg_mode_unexpected_start_plmn_selection_on_specified_plmn.
02/04/04   kwa     Added support for PS follow on request.
05/16/04   kwa     Added support for CS follow on request.
05/20/04   kwa     Removed calls to reg_mode_req_plmn_set since this function
                   is now called in reg_mode_update_attempted_plmn_list.  Added
                   code to resend an MMR_REG_REQ if an MMR_REG_CNF message was
                   received for the wrong RAT.
07/13/04   kwa     Added code to only resend an MMR_REG_REQ if an MMR_REG_CNF
                   message was received for the wrong RAT when the phone is
                   operating in single rather than dual mode.
01/05/06   sn      Removed stopping of HPLMN Timer in start_automatic_plmn_
                   selection.
02/16/06   sn      EHPLMN Feature. 
10/12/06   sn      WCDMA_1X mode preference implementation.
11/15/11  abhi   For EFRPLMNSI Selecting RLPMN if HPLMN is not found
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "reg_mode.h"
#include "reg_send.h"
#include "reg_sim_v.h"
#include "reg_timers.h"
#include "reg_state.h"
#include "msg.h"
#include "reg_nv.h"
#include "cm_v.h"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 


static reg_mode_automatic_mode_e_type reg_mode_automatic_plmn_selection_mode_sim[MAX_NAS_STACKS];
#define reg_mode_automatic_plmn_selection_mode   reg_mode_automatic_plmn_selection_mode_sim[reg_as_id]

static boolean                        reg_mode_automatic_plmn_selection_first_attempt_sim[MAX_NAS_STACKS];
#define reg_mode_automatic_plmn_selection_first_attempt   reg_mode_automatic_plmn_selection_first_attempt_sim[reg_as_id]

#ifdef FEATURE_RAT_PRIORITY_LIST
static reg_mode_3gpp_capability_s_type   reg_mode_automatic_plmn_selection_foreground_rat_list_sim[MAX_NAS_STACKS];
#define reg_mode_automatic_plmn_selection_foreground_rat_list reg_mode_automatic_plmn_selection_foreground_rat_list_sim[reg_as_id]
#endif

#else

static reg_mode_automatic_mode_e_type reg_mode_automatic_plmn_selection_mode;

static boolean                        reg_mode_automatic_plmn_selection_first_attempt;

#ifdef FEATURE_RAT_PRIORITY_LIST
static reg_mode_3gpp_capability_s_type reg_mode_automatic_plmn_selection_foreground_rat_list;
#endif /*FEATURE_RAT_PRIORITY_LIST*/

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE */

#ifndef FEATURE_RAT_PRIORITY_LIST
/* FEATURE_TRIPLE_SIM*/
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
static sys_radio_access_tech_e_type reg_mode_automatic_plmn_selection_foreground_search_rat_sim[MAX_NAS_STACKS] = { SYS_RAT_NONE, SYS_RAT_NONE, SYS_RAT_NONE};
#define reg_mode_automatic_plmn_selection_foreground_search_rat   reg_mode_automatic_plmn_selection_foreground_search_rat_sim[reg_as_id]

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
static sys_radio_access_tech_e_type reg_mode_automatic_plmn_selection_foreground_search_rat_sim[MAX_NAS_STACKS] = { SYS_RAT_NONE, SYS_RAT_NONE};
#define reg_mode_automatic_plmn_selection_foreground_search_rat   reg_mode_automatic_plmn_selection_foreground_search_rat_sim[reg_as_id]
#else
static sys_radio_access_tech_e_type reg_mode_automatic_plmn_selection_foreground_search_rat = SYS_RAT_NONE;
#endif
#endif


/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  start_automatic_plmn_selection

==============================================================================*/

static void start_automatic_plmn_selection
(
  sys_network_selection_mode_e_type    network_selection_mode,
  reg_mode_automatic_mode_e_type       automatic_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain
#ifdef FEATURE_GSM_BAND_PREF
  ,sys_band_mask_type                   band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif
)
{
  sys_radio_access_tech_e_type initial_rat;
#ifndef FEATURE_RAT_PRIORITY_LIST
  sys_mode_pref_e_type         hplmn_rat_search_order;
#else
  reg_mode_3gpp_capability_s_type  hplmn_rat_search_order;
#endif

#ifdef FEATURE_EQUIVALENT_HPLMN
  reg_sim_plmn_list_s_type     ehplmn_list;
  sys_plmn_id_s_type           rplmn, plmn_initial;
#else
  sys_plmn_id_s_type           hplmn;
#endif

  reg_mode_automatic_plmn_selection_first_attempt = TRUE;

  reg_mode_automatic_plmn_selection_mode = automatic_mode;
#ifndef FEATURE_RAT_PRIORITY_LIST
  reg_mode_automatic_plmn_selection_foreground_search_rat = SYS_RAT_NONE;
#else
  reg_mode_automatic_plmn_selection_foreground_rat_list.num_items = 0;
#endif /*FEATURE_RAT_PRIORITY_LIST*/

  reg_mode_clear_attempted_plmn_list();

#ifdef FEATURE_EQUIVALENT_HPLMN
  ehplmn_list = reg_sim_ehplmn_list_get();
#else
  hplmn = reg_sim_read_hplmn();
#endif

 #ifdef FEATURE_EQUIVALENT_HPLMN
   plmn_initial = ehplmn_list.plmn[0];
 /* If RPLMN is one of the EHPLNM then start PLMN selection from RPLMN 
  ** else use EHPLMN[0]
  */
  if (reg_mode_automatic_plmn_selection_mode == REG_MODE_AUTOMATIC_MODE_MMSS_HPLMN)
  {
      rplmn = reg_sim_read_rplmn(reg_mode_req_service_domain_get());
      if (reg_sim_is_ehplmn(rplmn))
      {
          plmn_initial = rplmn;
      }
  }
#endif
#ifndef FEATURE_RAT_PRIORITY_LIST
  hplmn_rat_search_order = reg_sim_read_hplmn_rat_search_order
  (
    mode_pref
  );

  initial_rat = reg_mode_initial_rat(hplmn_rat_search_order);

  reg_mode_reset_available_plmn_list
  (
#ifdef FEATURE_EQUIVALENT_HPLMN
    plmn_initial,
#else
    hplmn,
#endif
    hplmn_rat_search_order
  );
#else
  reg_mode_get_3gpp_rat_capabilty(&hplmn_rat_search_order, rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );
  
  reg_sim_read_hplmn_rat_search_order(&hplmn_rat_search_order);
#ifdef FEATURE_LTE
  reg_mode_adjust_rat_order_plmn_backoff_forbidden
  (
#ifdef FEATURE_EQUIVALENT_HPLMN
    &(plmn_initial),
#else
    &hplmn,
#endif
    &hplmn_rat_search_order
  );
#endif
  
  initial_rat = hplmn_rat_search_order.rat_list[0];        
  
  reg_mode_reset_available_plmn_list
  (
#ifdef FEATURE_EQUIVALENT_HPLMN
    plmn_initial,
#else
    hplmn,
#endif
    &hplmn_rat_search_order
  );
#endif /*ifndef FEATURE_RAT_PRIORITY_LIST*/

  reg_mode_update_attempted_plmn_list
  (
#ifdef FEATURE_EQUIVALENT_HPLMN
    plmn_initial,
#else
    hplmn,
#endif
    initial_rat
  );

  reg_send_mmr_reg_req
  (
    network_selection_mode,              /* network_selection_mode */
#ifdef FEATURE_EQUIVALENT_HPLMN
    plmn_initial,
#else
    hplmn,                                /* plmn                   */
#endif
    req_service_domain,                   /* req_service_domain     */
    mode_pref,                            /* mode_pref              */
    cs_for_request,                       /* cs_for_request         */
    ps_for_request,                       /* ps_for_request         */
    initial_rat                            /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
    ,band_pref                             /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
    ,rat_pri_list_info
#endif
    , ((reg_mode_automatic_plmn_selection_mode == REG_MODE_AUTOMATIC_MODE_BETTER_SYSTEM_RESELECTION)?
          MMR_REG_REQ_BETTER_SYSTEM_RESELECTION : 
         ((reg_mode_automatic_plmn_selection_mode == REG_MODE_AUTOMATIC_MODE_EF_RAT_REFRESH)? 
            MMR_REG_REQ_EF_RAT_REFRESH : MMR_REG_REQ_NORMAL))
#ifdef FEATURE_LTE
   , sms_only,
     additional_info
#endif 
#ifdef FEATURE_FEMTO_CSG
  ,SYS_CSG_ID_INVALID
#endif 
  );

  reg_mode_automatic_plmn_selection_first_attempt = TRUE;
}

static void start_foreground_plmn_search
(
  sys_network_selection_mode_e_type    network_selection_mode,  
  sys_mode_pref_e_type                 mode_pref,               
  sys_srv_domain_e_type                req_service_domain
#ifdef FEATURE_GSM_BAND_PREF
  , sys_band_mask_type                   band_pref
#endif  
#ifdef FEATURE_RAT_PRIORITY_LIST
  , const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif
)
{
  sys_plmn_id_s_type plmn;
  sys_radio_access_tech_e_type initial_rat;
  reg_mode_automatic_mode_e_type plmn_sel_mode;
#ifdef FEATURE_RAT_PRIORITY_LIST
  uint8 i,j;
#endif
  initial_rat = reg_state_active_rat_get();
#ifndef FEATURE_RAT_PRIORITY_LIST
  if(mode_pref == SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY )
  {
     initial_rat = SYS_RAT_GSM_RADIO_ACCESS;
  }
  else if(mode_pref == SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY)
  {
     initial_rat = SYS_RAT_UMTS_RADIO_ACCESS;
  }
  reg_mode_automatic_plmn_selection_foreground_search_rat = initial_rat;
#else
  reg_mode_get_3gpp_rat_capabilty(&reg_mode_automatic_plmn_selection_foreground_rat_list,
                                   rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );  
  if(!reg_mode_is_rat_present_in_rat_pri_list(initial_rat, rat_pri_list_info, TRUE))
  {
     initial_rat = reg_mode_automatic_plmn_selection_foreground_rat_list.rat_list[0];
  }
#endif

  plmn_sel_mode = reg_mode_get_plmn_selection_mode();
  
  if(plmn_sel_mode == REG_MODE_AUTOMATIC_MODE_ROAMING_PLMN)
  {
    reg_mode_automatic_plmn_selection_mode = REG_MODE_AUTOMATIC_MODE_ROAMING_PLMN;
  }
  else
  {
    reg_mode_automatic_plmn_selection_mode = REG_MODE_AUTOMATIC_MODE_USER_RESELECTION;
  }

  reg_mode_foreground_search_status = REG_MODE_FOREGROUND_SEARCH_STARTED;
  
  sys_plmn_undefine_plmn_id(&plmn);

  reg_mode_clear_available_plmn_list();

  reg_mode_managed_roaming_plmn_reset();

  reg_send_mmr_reg_req
    (
      network_selection_mode,                        /* network_selection_mode */
      plmn,                                          /* plmn                   */
      req_service_domain,                            /* req_service_domain     */
      mode_pref,                                     /* mode_pref              */
      0,                                             /* cs_for_request         */
      0,                                             /* ps_for_request         */
      initial_rat                                    /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
      ,band_pref                                      /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
     ,rat_pri_list_info
#endif     
      ,MMR_REG_REQ_NORMAL
#ifdef FEATURE_LTE
      ,sms_only,
       additional_info
#endif
#ifdef FEATURE_FEMTO_CSG
      ,SYS_CSG_ID_INVALID
#endif 
    );
  reg_mode_automatic_plmn_selection_first_attempt = TRUE;
  reg_mode_req_rat_set(initial_rat);
  reg_mode_req_plmn_set(plmn);
#ifndef FEATURE_RAT_PRIORITY_LIST  
  if(mode_pref == SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED ||
     mode_pref == SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED)
  {
    reg_mode_automatic_plmn_selection_foreground_search_rat = (reg_mode_automatic_plmn_selection_foreground_search_rat == SYS_RAT_GSM_RADIO_ACCESS)?
      SYS_RAT_UMTS_RADIO_ACCESS:SYS_RAT_GSM_RADIO_ACCESS;
  }
  else
  {
    reg_mode_automatic_plmn_selection_foreground_search_rat = SYS_RAT_NONE;  
  }
#else
  //Remove initial rat from the rat_list. 
  for (i= 0; i < reg_mode_automatic_plmn_selection_foreground_rat_list.num_items; i++)
  {
    if(reg_mode_automatic_plmn_selection_foreground_rat_list.rat_list[i] == initial_rat)
    {
      for(j = i+1; j < reg_mode_automatic_plmn_selection_foreground_rat_list.num_items; j++)
      {
        reg_mode_automatic_plmn_selection_foreground_rat_list.rat_list[j-1] = 
            reg_mode_automatic_plmn_selection_foreground_rat_list.rat_list[j];
      }
      break;
    }
  }
  reg_mode_automatic_plmn_selection_foreground_rat_list.num_items--;  
#endif /*FEATURE_RAT_PRIORITY_LIST*/
}

/*==============================================================================

FUNCTION NAME

  finished

==============================================================================*/

static boolean finished
(
  sys_plmn_service_state_s_type  service_state,
  const sys_detailed_plmn_list_s_type* available_plmn_list_p
)
{
  sys_radio_access_tech_e_type rat;
  sys_srv_domain_e_type        allowed_service_domain;
  sys_plmn_id_s_type           plmn;
  boolean                      success = FALSE;
  boolean                      finished_automatic_plmn_selection = FALSE;

#ifdef FEATURE_RAT_PRIORITY_LIST
  uint8 index;
#endif /*FEATURE_RAT_PRIORITY_LIST*/
  sys_detailed_plmn_list_info_s_type managed_roaming_plmn;

   sys_radio_access_tech_e_type   rplmn_rat;
   sys_plmn_id_s_type           rplmn;
  success = reg_mode_successful_registration
  (
    service_state.plmn,
    service_state.service_status,
    service_state.active_rat,
    service_state.plmn_reg_type
  );

  /*
  ** The MS is registered.
  */
  if (success)
  {
    if (service_state.roaming_ind == SYS_ROAM_STATUS_ON)
    {
      reg_timers_start_hplmn_search_period_timer();
    }
    else if (reg_nv_w_bplmn_pseudo_enabled()
#ifdef FEATURE_TDSCDMA
            ||reg_nv_is_forced_irat_enabled()
#endif
            )
    {    
        /* Start the timer if roaming is off and PLMN meets criteria
        */                   
       if ((service_state.roaming_ind == SYS_ROAM_STATUS_OFF)&&
            ((service_state.active_rat == SYS_RAT_TDS_RADIO_ACCESS) ||
            (service_state.active_rat == SYS_RAT_GSM_RADIO_ACCESS)||
               (service_state.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)) &&
#ifdef FEATURE_TDSCDMA
             (reg_mode_is_usim_compliant() || reg_nv_w_bplmn_pseudo_enabled())&&
#else 
             reg_nv_w_bplmn_pseudo_enabled() &&
#endif 
            reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS, reg_mode_rat_pri_list_get(), FALSE)
          )
        {
          reg_mode_start_appropriate_irat_timers();
        }
    }

    finished_automatic_plmn_selection = TRUE;
    reg_mode_automatic_plmn_selection_first_attempt = FALSE;
   }
  /*
  ** The MS is not registered.
  */
  else
  {
  #ifndef FEATURE_RAT_PRIORITY_LIST
    /*
    ** If the MS is not on the requested RAT then retry one more time.
    */
    if ( (service_state.active_rat != reg_mode_req_rat_get()) &&
         ( (reg_mode_mode_pref_get() == SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY) ||
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
           #error code not present
#endif
           (reg_mode_mode_pref_get() == SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY) 
#ifdef FEATURE_LTE
           || (reg_mode_mode_pref_get() == SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY)
#endif
         ) &&
         (reg_mode_automatic_plmn_selection_first_attempt == TRUE) )
    {
      reg_send_last_mmr_reg_req();

      reg_mode_automatic_plmn_selection_first_attempt = FALSE;
    }
    else
#else /*FEATURE_RAT_PRIORITY_LIST*/
    if((reg_mode_automatic_plmn_selection_first_attempt == TRUE) &&
       (service_state.active_rat != reg_mode_req_rat_get()) &&
       !(reg_mode_is_rat_present_in_rat_pri_list(service_state.active_rat,
                                            reg_mode_rat_pri_list_get(),TRUE)))
    {
      reg_send_last_mmr_reg_req();
      reg_mode_automatic_plmn_selection_first_attempt = FALSE;
    }
    else
#endif /*FEATURE_RAT_PRIORITY_LIST*/
    {
      reg_mode_update_available_plmn_list
      (
        reg_mode_automatic_plmn_selection_mode,													
#ifndef FEATURE_RAT_PRIORITY_LIST        
        reg_mode_mode_pref_get(),
#else
        reg_mode_rat_pri_list_get(),
#endif
        reg_mode_req_service_domain_get(),
        available_plmn_list_p
      );

#ifndef FEATURE_RAT_PRIORITY_LIST
      if(reg_mode_automatic_plmn_selection_foreground_search_rat != SYS_RAT_NONE)
      {
        rat = reg_mode_automatic_plmn_selection_foreground_search_rat; 
#else       
#ifdef FEATURE_LTE
      /*remove LTE from foreground RAT list if SIM is invalid for PS domain */
      if((reg_state_sim_status_get() == SYS_SIM_STATE_PS_INVALID) &&
          (reg_mode_automatic_plmn_selection_foreground_rat_list.num_items > 0))
      {
        reg_mode_remove_rat_from_3gpp_rat_list(&reg_mode_automatic_plmn_selection_foreground_rat_list, 
                                                                               SYS_RAT_LTE_RADIO_ACCESS);
      }
#endif 
      if(reg_mode_automatic_plmn_selection_foreground_rat_list.num_items > 0)
      {
        rat = reg_mode_automatic_plmn_selection_foreground_rat_list.rat_list[0];
#endif /*FEATURE_RAT_PRIORITY_LIST*/
      
        sys_plmn_undefine_plmn_id(&plmn);

        reg_send_mmr_reg_req
        (
          reg_mode_network_selection_mode_get(),         /* network_selection_mode */
          plmn,                                          /* plmn                   */
          reg_mode_req_service_domain_get(),             /* req_service_domain     */
          reg_mode_mode_pref_get(),                           /* mode_pref              */
          0,                                             /* cs_for_request         */
          0,                                             /* ps_for_request         */
          rat                                           /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
          ,reg_mode_band_pref_get()                       /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
          ,reg_mode_rat_pri_list_get()                
#endif          
          ,MMR_REG_REQ_NORMAL
#ifdef FEATURE_LTE
         ,reg_mode_sms_only_get(),
          0
#endif
#ifdef FEATURE_FEMTO_CSG
         ,SYS_CSG_ID_INVALID
#endif 
        );   

        reg_mode_req_rat_set(rat);
        reg_mode_req_plmn_set(plmn);
#ifndef FEATURE_RAT_PRIORITY_LIST
        reg_mode_automatic_plmn_selection_foreground_search_rat = SYS_RAT_NONE;
#else 
        //remove rat from rat_list
        for (index = 1; index < reg_mode_automatic_plmn_selection_foreground_rat_list.num_items; index++)
        {
          reg_mode_automatic_plmn_selection_foreground_rat_list.rat_list[index-1] = 
            reg_mode_automatic_plmn_selection_foreground_rat_list.rat_list[index];
        }
        reg_mode_automatic_plmn_selection_foreground_rat_list.num_items--;  
#endif /*FEATURE_RAT_PRIORITY_LIST*/
      }
      else
      {
        /*
        ** Get the next prioritized PLMN.
        */
        reg_mode_next_available_plmn
        (
          &plmn,
          &rat
        );

        if(reg_nv_efrplmnsi_select_rplmn() == TRUE &&
           (reg_mode_get_hplmn_to_selected(
 #if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
                                (sys_modem_as_id_e_type)reg_as_id
 #endif
                                           ) == TRUE))
        {         
 #ifdef FEATURE_EQUIVALENT_HPLMN  
            if(!reg_sim_is_ehplmn( plmn))
 #else
            if(!sys_plmn_match(reg_sim_read_hplmn(), plmn) )
 #endif  
           {
             reg_mode_next_equivalent_rplmn(&rplmn, &rplmn_rat);
             if(!sys_plmn_id_is_undefined(rplmn))
             {
               plmn = rplmn;
               rat = rplmn_rat;
               MSG_HIGH_DS_0(REG_SUB,"=REG= RPLMN to be selected after HPLMN");
             }
          }
        }      
        /*
        ** Attempt to acquire service on the PLMN.
        */
        if (!sys_plmn_id_is_undefined(plmn))
        {
            if((reg_mode_automatic_plmn_selection_mode == 
                    REG_MODE_AUTOMATIC_MODE_BETTER_SYSTEM_RESELECTION ||
                 reg_mode_automatic_plmn_selection_mode == REG_MODE_AUTOMATIC_MODE_MMSS_HPLMN)&&
#ifdef FEATURE_EQUIVALENT_HPLMN  
            (!reg_sim_is_ehplmn( plmn))
#else
            (!sys_plmn_match(reg_sim_read_hplmn(), plmn) )
#endif
             &&(!cm_per_subs_is_plmn_rat_allowed(plmn,rat,(sys_modem_as_id_e_type)reg_sub_id,reg_set_cm_stack_id())))
          {
            MSG_HIGH_DS_0(REG_SUB,"=REG= CM API for PLMN/RAT allowed query returned FALSE, send no service to CM");
            finished_automatic_plmn_selection = TRUE;
          }
          else
          {
            reg_mode_update_attempted_plmn_list
            (
              plmn,
              rat
            );
  
            allowed_service_domain = reg_mode_allowed_service_domain
            (
              plmn,                                 /* plmn                   */
              reg_mode_req_service_domain_get()     /* req_service_domain     */
            );
  
            reg_send_mmr_reg_req
            (
              reg_mode_network_selection_mode_get(), /* network_selection_mode */
              plmn,                                 /* plmn                   */
              allowed_service_domain,               /* req_service_domain     */
              reg_mode_mode_pref_get(),             /* mode_pref              */
              reg_mode_cs_for_request_get(),        /* cs_for_request         */
              reg_mode_ps_for_request_get(),        /* ps_for_request         */
              rat                                  /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
             ,reg_mode_band_pref_get()              /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
             ,reg_mode_rat_pri_list_get()                 /*rat_pri_list*/
#endif              
             , ((reg_mode_automatic_plmn_selection_mode == REG_MODE_AUTOMATIC_MODE_BETTER_SYSTEM_RESELECTION)?
                   MMR_REG_REQ_BETTER_SYSTEM_RESELECTION : 
                   ((reg_mode_automatic_plmn_selection_mode == REG_MODE_AUTOMATIC_MODE_EF_RAT_REFRESH)? 
                      MMR_REG_REQ_EF_RAT_REFRESH : MMR_REG_REQ_NORMAL))
#ifdef FEATURE_LTE
             ,reg_mode_sms_only_get(),
              0
#endif
#ifdef FEATURE_FEMTO_CSG
             ,SYS_CSG_ID_INVALID
#endif 
            );
  
            reg_mode_automatic_plmn_selection_first_attempt = TRUE;
          }
        }
        else
        {
          managed_roaming_plmn = reg_mode_managed_roaming_plmn_get();
         if(mm_managed_roaming_enabled && !sys_plmn_id_is_undefined(managed_roaming_plmn.plmn))
         {
           allowed_service_domain = reg_mode_allowed_service_domain
           (
             managed_roaming_plmn.plmn,                                 /* plmn                   */
             reg_mode_req_service_domain_get()     /* req_service_domain     */
           );
           
           reg_send_mmr_reg_req
           (
             reg_mode_network_selection_mode_get(), /* network_selection_mode */
             managed_roaming_plmn.plmn,                                 /* plmn                   */
             allowed_service_domain,               /* req_service_domain     */
             reg_mode_mode_pref_get(),             /* mode_pref              */
             reg_mode_cs_for_request_get(),        /* cs_for_request         */
             reg_mode_ps_for_request_get(),        /* ps_for_request         */
             managed_roaming_plmn.rat                                  /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
             ,reg_mode_band_pref_get()              /* band_pref              */
#endif    
#ifdef FEATURE_RAT_PRIORITY_LIST
             ,reg_mode_rat_pri_list_get()                 /*rat_pri_list*/
#endif   
             ,MMR_REG_REQ_NORMAL
#ifdef FEATURE_LTE
             ,reg_mode_sms_only_get(),
              0
#endif
#ifdef FEATURE_FEMTO_CSG
             ,SYS_CSG_ID_INVALID
#endif 
            );

            reg_mode_req_rat_set(managed_roaming_plmn.rat);
            reg_mode_req_plmn_set(managed_roaming_plmn.plmn);
            reg_mode_managed_roaming_plmn_reset();

            reg_mode_automatic_plmn_selection_first_attempt = TRUE; 
         } 
         else
         {
            finished_automatic_plmn_selection = TRUE;

            reg_mode_automatic_plmn_selection_first_attempt = FALSE;
         }
        }
      }
    }
  }

  return finished_automatic_plmn_selection;
}


/*==============================================================================

FUNCTION NAME

  number

==============================================================================*/

static reg_mode_e_type number ( void )
{
  return REG_MODE_AUTOMATIC_PLMN_SELECTION;
}


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

reg_mode_s_type reg_mode_automatic_plmn_selection = 
{
  start_automatic_plmn_selection,
  reg_mode_unexpected_start_higher_priority_plmn_selection,
  reg_mode_unexpected_start_limited_service,
  reg_mode_unexpected_start_manual_plmn_selection,
  reg_mode_unexpected_start_plmn_selection,
  reg_mode_unexpected_start_plmn_selection_roaming_plmn,
  reg_mode_unexpected_start_plmn_selection_on_specified_plmn,
  start_foreground_plmn_search,
#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
  reg_mode_unexpected_start_suitable_cell_selection,
#endif
  reg_mode_unexpected_start_higher_priority_irat_plmn_selection,
#ifdef FEATURE_FEMTO_CSG
  reg_mode_unexpected_start_automatic_csg_selection,
  reg_mode_unexpected_start_automatic_csg_selection_app, 
  reg_mode_unexpected_start_manual_csg_selection,
  reg_mode_unexpected_start_manual_csg_selection_app,
#endif
  reg_mode_unexpected_update_plmn_selection_mode,
  finished,
  number
};

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

