/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2012 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_mode_manual_csg_selection.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_FEMTO_CSG

#include "reg_mode.h"
#include "reg_send.h"
#include "reg_sim_v.h"
#include "reg_timers.h"
#include "reg_nv.h"
#include "sys_eplmn_list_v.h"
#include "reg_state.h"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a,reg_sub_id,reg_as_id)
#endif
/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
#if defined(FEATURE_SGLTE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
static sys_radio_access_tech_e_type reg_mode_manual_csg_selection_next_rat_sim[MAX_NAS_STACKS] = {SYS_RAT_NONE, SYS_RAT_NONE, SYS_RAT_NONE};
#else
static sys_radio_access_tech_e_type reg_mode_manual_csg_selection_next_rat_sim[MAX_NAS_STACKS] = {SYS_RAT_NONE, SYS_RAT_NONE};
#endif 
#define reg_mode_manual_csg_selection_next_rat  reg_mode_manual_csg_selection_next_rat_sim[reg_as_id]

static sys_plmn_id_s_type          reg_mode_manual_csg_selection_plmn_sim[MAX_NAS_STACKS] ;
#define reg_mode_manual_csg_selection_plmn   reg_mode_manual_csg_selection_plmn_sim[reg_as_id]

static sys_csg_id_type                        reg_mode_manual_csg_selection_csg_id_sim[MAX_NAS_STACKS] ;
#define reg_mode_manual_csg_selection_csg_id  reg_mode_manual_csg_selection_csg_id_sim[reg_as_id]

static uint32           reg_mode_manual_csg_selection_next_index_sim[MAX_NAS_STACKS] = {0,0};
#define reg_mode_manual_csg_selection_next_index  reg_mode_manual_csg_selection_next_index_sim[reg_as_id]

static reg_cm_service_req_e_type reg_mode_manual_csg_selection_srv_req_type_sim[MAX_NAS_STACKS] = 
                                                     {REG_CM_SERVICE_REQ_NONE, REG_CM_SERVICE_REQ_NONE};
#define reg_mode_manual_csg_selection_srv_req_type  reg_mode_manual_csg_selection_srv_req_type_sim[reg_as_id]

static boolean                        reg_mode_manual_csg_selection_first_attempt_sim[MAX_NAS_STACKS];
#define reg_mode_manual_csg_selection_first_attempt   reg_mode_manual_csg_selection_first_attempt_sim[reg_as_id]
#else
static sys_radio_access_tech_e_type reg_mode_manual_csg_selection_next_rat = SYS_RAT_NONE;
static sys_plmn_id_s_type          reg_mode_manual_csg_selection_plmn;
static sys_csg_id_type                reg_mode_manual_csg_selection_csg_id;
static uint32                      reg_mode_manual_csg_selection_next_index = 0;
static reg_cm_service_req_e_type   reg_mode_manual_csg_selection_srv_req_type = REG_CM_SERVICE_REQ_NONE;

static boolean                        reg_mode_manual_csg_selection_first_attempt;
#endif 

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/



/*==============================================================================

FUNCTION NAME

  start_automatic_csg_selection

==============================================================================*/


static void start_manual_csg_selection
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_plmn_id_s_type          plmn,
  sys_radio_access_tech_e_type  rat,
  sys_csg_id_type                          csg_id,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info  
#endif 
)
{
  sys_radio_access_tech_e_type  initial_rat;
  uint8  index;
  reg_mode_3gpp_capability_s_type   rplmn_rat_search_order;
  sys_csg_support_e_type csg_support_nv = reg_nv_read_csg_support();
  
  reg_mode_manual_csg_selection_srv_req_type = 
                        REG_CM_SERVICE_REQ_USER_SELECTION_CSG_RPLMN;

  reg_mode_clear_attempted_plmn_list();
  
  /* Add RPLMN to avaialble list */
  reg_mode_get_3gpp_rat_capabilty(&rplmn_rat_search_order, rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );

  initial_rat = rat;
  if(csg_support_nv == SYS_CSG_SUPPORT_WCDMA_LTE)
  {
    if(initial_rat == SYS_RAT_LTE_RADIO_ACCESS || initial_rat == SYS_RAT_UMTS_RADIO_ACCESS)
    {
      reg_mode_manual_csg_selection_next_rat = SYS_RAT_NONE;
    }
    else
    {
      initial_rat = SYS_RAT_NONE;
      reg_mode_manual_csg_selection_next_rat = SYS_RAT_NONE;
      for(index = 0; index < rplmn_rat_search_order.num_items; index++)
      {
        if(((rplmn_rat_search_order.rat_list[index] == SYS_RAT_LTE_RADIO_ACCESS) &&
            (reg_nv_read_csg_support() == SYS_CSG_SUPPORT_WCDMA_LTE))|| 
            (rplmn_rat_search_order.rat_list[index] == SYS_RAT_UMTS_RADIO_ACCESS))
        {
          if(initial_rat == SYS_RAT_NONE)
          {
            initial_rat = rplmn_rat_search_order.rat_list[index];
          }
          else
          {
            reg_mode_manual_csg_selection_next_rat = rplmn_rat_search_order.rat_list[index];
            break;
          }
        }
      }
    }
  }
  else
  {
    initial_rat = SYS_RAT_NONE;
    reg_mode_manual_csg_selection_next_rat = SYS_RAT_NONE;
    if(csg_support_nv == SYS_CSG_SUPPORT_WCDMA &&
       reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_UMTS_RADIO_ACCESS,rat_pri_list_info,TRUE))
    {
      initial_rat = SYS_RAT_UMTS_RADIO_ACCESS;
    }
  }


  reg_sim_read_rplmn_rat_search_order(&rplmn_rat_search_order);
  
  reg_mode_reset_available_plmn_list
  (
    plmn,
    &rplmn_rat_search_order
  );

  reg_mode_manual_csg_selection_plmn = plmn;
  reg_mode_manual_csg_selection_csg_id = csg_id;

  reg_mode_req_rat_set(initial_rat);
  reg_mode_req_plmn_set(plmn);
  reg_send_mmr_reg_req
  (
    network_selection_mode,  /* network_selection_mode */
    plmn,                   /* plmn                   */
    req_service_domain,  /* req_service_domain     */
    reg_mode_req_mode_pref_get(),               /* mode_pref              */
    cs_for_request,          /* cs_for_request         */
    ps_for_request,          /* ps_for_request         */
    initial_rat,             /* rat                    */
 #ifdef FEATURE_GSM_BAND_PREF
    reg_mode_band_pref_get(),
#endif
    rat_pri_list_info,
    MMR_REG_REQ_NORMAL,
#ifdef FEATURE_LTE
    sms_only,
    additional_info,
#endif 
    csg_id
  );
  
  reg_mode_manual_csg_selection_first_attempt = TRUE;
  
}

/*==============================================================================
FUNCTION NAME:  start_manual_csg_selection_app
==============================================================================*/
static void start_manual_csg_selection_app
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_plmn_id_s_type                   plmn,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean                             sms_only,
   uint32                              additional_info  
#endif
)
{
  sys_radio_access_tech_e_type         rat = SYS_RAT_NONE;
  sys_csg_id_type                      csg_id = SYS_CSG_ID_INVALID;
  const sys_detailed_plmn_list_s_type  *available_csg_list_manual;
  reg_mode_3gpp_capability_s_type      rat_priority_list;

  available_csg_list_manual = reg_mode_available_plmn_list_manual_get();
  reg_mode_manual_csg_selection_next_rat = SYS_RAT_NONE;

  if(available_csg_list_manual->length > 0)
  {
    reg_mode_manual_csg_selection_srv_req_type = 
                        REG_CM_SERVICE_REQ_CSG_SELECTION_APP;

    reg_mode_clear_attempted_plmn_list();

    reg_mode_get_3gpp_rat_capabilty(&rat_priority_list, 
                                    rat_pri_list_info
#ifdef FEATURE_SGLTE
                                    ,TRUE
#endif                                  
     );

    reg_mode_reset_available_plmn_list
    (
      plmn,
      &rat_priority_list
    );

    /* Attempt CSG registration */
    sys_plmn_undefine_plmn_id(&plmn);
    plmn = available_csg_list_manual->info[0].plmn;
    rat = available_csg_list_manual->info[0].rat;
    csg_id = available_csg_list_manual->info[0].csg_info.csg_id;
    reg_mode_manual_csg_selection_next_index = 1;

    reg_mode_req_plmn_set(plmn);
    reg_mode_req_rat_set(rat);

    if(!sys_plmn_match(plmn, reg_sim_read_rplmn(req_service_domain)))
    {
      network_selection_mode  = SYS_NETWORK_SELECTION_MODE_MANUAL_CSG;
    }

    reg_send_mmr_reg_req
    (
      network_selection_mode,
      plmn,
      req_service_domain,
      reg_mode_req_mode_pref_get(),
      cs_for_request,
      ps_for_request,
      rat,
 #ifdef FEATURE_GSM_BAND_PREF
      reg_mode_band_pref_get(),
#endif
      rat_pri_list_info,
      MMR_REG_REQ_CSG_SELECTION_APP,
#ifdef FEATURE_LTE
      sms_only,
      additional_info,
#endif 
      csg_id
    );
    reg_mode_manual_csg_selection_first_attempt = TRUE;
  }
  else
  {
    reg_state_update_last_cm_service_req_type(REG_CM_SERVICE_REQ_NORMAL);
    reg_mode_start_manual_plmn_selection
    (
      network_selection_mode,
      plmn,
      reg_mode_req_mode_pref_get(),
      cs_for_request,
      ps_for_request,
      req_service_domain,
#ifdef FEATURE_GSM_BAND_PREF
      reg_mode_band_pref_get(),
#endif
      rat_pri_list_info,
      REG_CM_SERVICE_REQ_NORMAL
#ifdef FEATURE_LTE
      ,sms_only
      ,additional_info
#endif
    );
  }
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

  sys_radio_access_tech_e_type rat = SYS_RAT_GSM_RADIO_ACCESS;
  sys_srv_domain_e_type        allowed_service_domain;
  sys_plmn_id_s_type           plmn, rplmn;
  boolean                      success                 = FALSE;
  boolean                      finished_plmn_selection = FALSE;
  const sys_detailed_plmn_list_s_type  *available_csg_list_manual;
  sys_csg_id_type csg_id = SYS_CSG_ID_INVALID;
  sys_network_selection_mode_e_type network_selection_mode = reg_mode_network_selection_mode_get();
  mmr_reg_req_e_type   reg_req_type = MMR_REG_REQ_NORMAL;

  success = reg_mode_successful_registration
  (
    service_state.plmn,
    service_state.service_status,
    service_state.active_rat,
    service_state.plmn_reg_type
  );

  if (success)
  {
    if(reg_send_nw_sel_mode_sent_to_mm_get() == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
    {
      reg_mode_network_selection_mode_set(SYS_NETWORK_SELECTION_MODE_MANUAL_CSG);
      reg_send_cm_csg_nw_sel_mode_update_ind(SYS_NETWORK_SELECTION_MODE_MANUAL_CSG);
    }
    finished_plmn_selection = TRUE;
    reg_mode_manual_csg_selection_first_attempt = FALSE;
  }
  /*
  ** For MODE MANUAL_CSG no need to continue PLMN selection*/
  else if((reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
               && (reg_mode_manual_csg_selection_next_rat == SYS_RAT_NONE))
  {
    finished_plmn_selection = TRUE;
  }
    /*
  ** If service cnf is received on a rat different from requested rat and service cnf rat
  ** is not present in rat priority list then resend the last reg request.
  */
  else if((reg_mode_manual_csg_selection_first_attempt == TRUE) &&
            (service_state.active_rat != reg_mode_req_rat_get()) &&
           !(reg_mode_is_rat_present_in_rat_pri_list(service_state.active_rat,
                                            reg_mode_rat_pri_list_get(),TRUE)))
  {
    reg_send_last_mmr_reg_req();
    reg_mode_manual_csg_selection_first_attempt = FALSE;
  }
  else
  {
    sys_plmn_undefine_plmn_id(&plmn);

    rplmn = reg_sim_read_rplmn(reg_mode_req_service_domain_get());

    if(reg_mode_manual_csg_selection_srv_req_type == REG_CM_SERVICE_REQ_CSG_SELECTION_APP)
    {
      available_csg_list_manual = reg_mode_available_plmn_list_manual_get();
      if(reg_mode_manual_csg_selection_next_index < available_csg_list_manual->length)
      {
        plmn = available_csg_list_manual->info[reg_mode_manual_csg_selection_next_index].plmn;
        csg_id = available_csg_list_manual->info[reg_mode_manual_csg_selection_next_index].csg_info.csg_id;
        rat = available_csg_list_manual->info[reg_mode_manual_csg_selection_next_index].rat;
        if(!sys_plmn_match(plmn, rplmn))
        {
          network_selection_mode = SYS_NETWORK_SELECTION_MODE_MANUAL_CSG;
        }
        reg_mode_manual_csg_selection_next_index++;
        reg_req_type = MMR_REG_REQ_CSG_SELECTION_APP;
      }
      else
      {
        reg_state_update_last_cm_service_req_type(REG_CM_SERVICE_REQ_NORMAL);
      }
    }
    else if(reg_mode_manual_csg_selection_next_rat != SYS_RAT_NONE)
    {
      plmn = reg_mode_manual_csg_selection_plmn;
      csg_id = reg_mode_manual_csg_selection_csg_id;
      rat = reg_mode_manual_csg_selection_next_rat;
      reg_mode_manual_csg_selection_next_rat = SYS_RAT_NONE;
    }

    /*pick PLMN from avaialble PLMN list*/
    if(sys_plmn_id_is_undefined(plmn))
    {
      reg_mode_next_available_plmn
      (
        &plmn,
        &rat
      );
      csg_id = SYS_CSG_ID_INVALID;
    }
    if(!sys_plmn_id_is_undefined(plmn))
    {
      allowed_service_domain = reg_mode_allowed_service_domain
      (
        plmn,                            
        reg_mode_req_service_domain_get()
      );
      if(csg_id == SYS_CSG_ID_INVALID)
      {
        reg_mode_update_attempted_plmn_list
        (
          plmn,
          rat
        );
      }
      else
      {
        reg_mode_req_rat_set(rat);
        reg_mode_req_plmn_set(plmn);
      }
      reg_send_mmr_reg_req
      (
        network_selection_mode,
        plmn,
        allowed_service_domain,
        reg_mode_mode_pref_get(),
        reg_mode_cs_for_request_get(),
        reg_mode_ps_for_request_get(),
        rat
#ifdef FEATURE_GSM_BAND_PREF
        ,reg_mode_band_pref_get()
#endif
        ,reg_mode_rat_pri_list_get()
        ,reg_req_type
#ifdef FEATURE_LTE
        ,reg_mode_sms_only_get(),
         0
#endif 
        ,csg_id
      );
      reg_mode_manual_csg_selection_first_attempt = TRUE;
    }
    else
    {
      finished_plmn_selection = TRUE;
      reg_mode_manual_csg_selection_first_attempt = FALSE;
    }  
  }
  return finished_plmn_selection;
}




/*==============================================================================

FUNCTION NAME

  number

==============================================================================*/

static reg_mode_e_type number ( void )
{
  return REG_MODE_MANUAL_CSG_SELECTION;
}


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

reg_mode_s_type reg_mode_manual_csg_selection = 
{
  reg_mode_unexpected_start_automatic_plmn_selection,
  reg_mode_unexpected_start_higher_priority_plmn_selection,
  reg_mode_unexpected_start_limited_service,
  reg_mode_unexpected_start_manual_plmn_selection,
  reg_mode_unexpected_start_plmn_selection,
  reg_mode_unexpected_start_plmn_selection_roaming_plmn,
  reg_mode_unexpected_start_plmn_selection_on_specified_plmn,
  reg_mode_unexpected_start_foreground_plmn_search,
#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
  reg_mode_unexpected_start_suitable_cell_selection,
#endif 
  reg_mode_unexpected_start_higher_priority_irat_plmn_selection,
  reg_mode_unexpected_start_automatic_csg_selection,
  reg_mode_unexpected_start_automatic_csg_selection_app,
  start_manual_csg_selection,
  start_manual_csg_selection_app,
  reg_mode_unexpected_update_plmn_selection_mode,
  finished,
  number
};


#endif
/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/
