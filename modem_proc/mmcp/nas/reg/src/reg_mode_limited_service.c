/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_mode_limited_service.c_v   1.2   06 Jun 2002 10:48:42   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_mode_limited_service.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/31/02   kwa     Added update_equivalent_plmn_list to finished function.
06/27/02   kwa     Modified function that sends an mmr_reg_req due to
                   interface change.
10/01/02   kwa     Added reg_mode_limited_service_domain to store the requested
                   service domain for limited service independently from the
                   normal requested service domain.  This change was made to 
                   properly support limited service when the SIM/USIM is marked
                   as invalid by the network.
10/10/02   kwa     Killed timers upon processing request for limited service.
                   Fixed bug where REG did not send the cm_service_cnf message.
10/18/02   kwa     Changed behavior when the initial search for limited service
                   fails.  Instead of setting the PLMN Search Period Timer the
                   code now immediately reinitiates a search for service.
                   Added FEATURE_REG_DEEP_SLEEP.
04/04/03   kwa     Placed the code to repeatedly search for limited service
                   within FEATURE_REG_DEEP_SLEEP.
05/21/03   kwa     Restructured code to eliminate lint warnings.
06/04/03   kwa     Fixed one remaining lint warning.
09/08/03   kwa     Removed FEATURE_REG_DEEP_SLEEP.
09/16/03   kwa     Always set reg_mode_limited_service_domain to
                   SYS_SRV_DOMAIN_CS_ONLY when placing the phone in limited
                   service mode.
09/18/03   kwa     Added support for band preferences.
09/25/03   kwa     Revert to setting reg_mode_limited_service_domain to
                   the requested service domain when placing the phone in
                   limited service mode.
10/02/03   kwa     Added code to support the routine acquisition of normal
                   service when requesting limited service using
                   SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION.
10/22/03   kwa     Removed the parameters update_equivalent_plmn_list and
                   equivalent_plmn_list_p from the function finished.
11/19/03   kwa     Removed obsolete function for retrieving the mode name.
01/20/04   kwa     Added unexpected function handler
                   reg_mode_unexpected_start_plmn_selection_on_specified_plmn.
02/04/04   kwa     Added support for PS follow on request.
03/09/04   kwa     Added code in finished to set the HPLMN search period timer
                   if normal service was acquired during a routine search for
                   limited service.
05/04/04   kwa     Added code in start_limited_service to initiate the search
                   for limited service on the currently active rat.
05/16/04   kwa     Added support for CS follow on request.
06/08/04   kwa     Added code in start_limited_service to only initiate the
                   search for limited service on the currently active rat when
                   the active rat is consistent with the mode preference.
07/04/06   sn      Modified start_limited_service to fix incorrect brackets.
07/27/06   sn      Added Uniform OOS changes.
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING
10/12/06   sn      WCDMA_1X mode preference implementation.
12/15/09   abhi  Removing UOOS code
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "reg_mode.h"
#include "reg_nv.h"
#include "reg_send.h"
#include "reg_state.h"
#include "reg_timers.h"

#include "msg.h"
#include "sys.h"
#include "policyman.h"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                            STATIC DATA DECLARATIONS

==============================================================================*/
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 

#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
static sys_srv_domain_e_type        reg_mode_limited_service_domain_sim[MAX_NAS_STACKS] = { SYS_SRV_DOMAIN_CS_ONLY, SYS_SRV_DOMAIN_CS_ONLY, SYS_SRV_DOMAIN_CS_ONLY};
#else
static sys_srv_domain_e_type        reg_mode_limited_service_domain_sim[MAX_NAS_STACKS] = { SYS_SRV_DOMAIN_CS_ONLY, SYS_SRV_DOMAIN_CS_ONLY};
#endif 
#define reg_mode_limited_service_domain reg_mode_limited_service_domain_sim[reg_as_id]

#ifndef FEATURE_RAT_PRIORITY_LIST
static boolean                      reg_mode_limited_service_started_sim[MAX_NAS_STACKS] = { FALSE, FALSE};
#define reg_mode_limited_service_started   reg_mode_limited_service_started_sim[reg_as_id]


static sys_radio_access_tech_e_type reg_mode_limited_service_initial_rat_sim[MAX_NAS_STACKS] = { SYS_RAT_GSM_RADIO_ACCESS, SYS_RAT_GSM_RADIO_ACCESS};
#define reg_mode_limited_service_initial_rat   reg_mode_limited_service_initial_rat_sim[reg_as_id]
#else
static reg_mode_3gpp_capability_s_type reg_mode_limited_service_rat_order_sim[MAX_NAS_STACKS];
#define reg_mode_limited_service_rat_order reg_mode_limited_service_rat_order_sim[reg_as_id]
#endif /*FEATURE_RAT_PRIORITY_LIST*/

#else

static sys_srv_domain_e_type        reg_mode_limited_service_domain = SYS_SRV_DOMAIN_CS_ONLY;
#ifndef FEATURE_RAT_PRIORITY_LIST
static boolean                      reg_mode_limited_service_started = FALSE;
static sys_radio_access_tech_e_type reg_mode_limited_service_initial_rat = SYS_RAT_GSM_RADIO_ACCESS;
#else
static reg_mode_3gpp_capability_s_type reg_mode_limited_service_rat_order;
#endif /*FEATURE_RAT_PRIORITY_LIST*/

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE */
/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  start_limited_service

==============================================================================*/

static void start_limited_service
(
  boolean                              cs_for_request,
  boolean                              ps_for_request,
#ifdef FEATURE_GSM_BAND_PREF
  sys_srv_domain_e_type                req_service_domain,
  sys_band_mask_type                   band_pref
#else
  sys_srv_domain_e_type                req_service_domain
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
  ,reg_cm_service_req_e_type           type
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif
)
{
  sys_plmn_id_s_type           plmn;
  sys_specialization_t  specialization = SYS_SPECIAL_NONE;
  policyman_status_t  status = POLICYMAN_STATUS_ERR;
  policyman_item_t    *pItem;
  mmr_reg_req_e_type  serv_req_type = MMR_REG_REQ_NORMAL;    

 #ifdef FEATURE_RAT_PRIORITY_LIST
  sys_radio_access_tech_e_type initial_rat;
  uint8 i;
  uint8 j;
 #endif /*FEATURE_RAT_PRIORITY_LIST*/

  {
    policyman_item_id_t ids[] = {POLICYMAN_ITEM_DEVICE_CONFIGURATION};
    status = policyman_get_items(ids, ARR_SIZE(ids), (policyman_item_t const **) &pItem);    
  }
  if (status == POLICYMAN_STATUS_SUCCESS)
  {
     policyman_device_config_get_specialization(pItem, &specialization);
     policyman_item_release(pItem);
  }

  if(type == REG_CM_SERVICE_REQ_FULL_RAT_MODE)
  {
    serv_req_type = MMR_REG_REQ_FULL_RAT_MODE;
  }

  if(reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED)
  {
    reg_timers_stop_hplmn_search_period_timer();
  }

  reg_mode_limited_service_domain = req_service_domain;

  sys_plmn_undefine_plmn_id(&plmn);
#ifndef FEATURE_RAT_PRIORITY_LIST
  if (((reg_state_service_status_get() == SYS_SRV_STATUS_SRV) ||
       (reg_state_service_status_get() == SYS_SRV_STATUS_LIMITED) ||
       (reg_state_service_status_get() == SYS_SRV_STATUS_LIMITED_REGIONAL)) &&
      (((reg_state_active_rat_get() == SYS_RAT_GSM_RADIO_ACCESS) &&
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
        #error code not present
#endif
        (reg_mode_mode_pref_get() != SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY)) ||
       ((reg_state_active_rat_get() == SYS_RAT_UMTS_RADIO_ACCESS) &&
        (reg_mode_mode_pref_get() != SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY))
      ))
  {
    reg_mode_limited_service_initial_rat = reg_state_active_rat_get();
  }
  else
  {
    reg_mode_limited_service_initial_rat = reg_mode_initial_rat(reg_mode_mode_pref_get());
    if((reg_mode_service_available_rat_get() != reg_mode_limited_service_initial_rat) &&
       (reg_mode_service_available_rat_get() != SYS_RAT_NONE) &&
       ((reg_mode_mode_pref_get() == SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED) ||
        (reg_mode_mode_pref_get() == SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED)))
    {
      reg_mode_limited_service_initial_rat = reg_mode_service_available_rat_get();  
    }
  }
  if(reg_nv_is_wcdma_freq_lock_enabled())
  {
     reg_mode_limited_service_started = FALSE;
  }
  else
  {
    reg_mode_limited_service_started = TRUE;
  }
#else
  reg_mode_get_3gpp_rat_capabilty(&reg_mode_limited_service_rat_order, rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );

  if(status == POLICYMAN_STATUS_SUCCESS)
  {
      if ((specialization != SYS_SPECIAL_SBM_RAT_SEARCH_ORDER) && 
         (type != REG_CM_SERVICE_REQ_FULL_RAT_MODE))
      {           
#ifdef FEATURE_LTE
          /* Move LTE to end if there is no emergency call pending */
#ifdef FEATURE_LTE_REL9
          if(reg_bit_mask_check(additional_info,
              (byte)SYS_ADDTNL_INFO_EMERG_CALL) == FALSE)
#endif
          {
              for (i= 0; i < reg_mode_limited_service_rat_order.num_items; i++)
              {
                  if(reg_mode_limited_service_rat_order.rat_list[i] == SYS_RAT_LTE_RADIO_ACCESS)
                  {
                      for(j = i+1; j < reg_mode_limited_service_rat_order.num_items; j++)
                      {
                          reg_mode_limited_service_rat_order.rat_list[j-1] = 
                              reg_mode_limited_service_rat_order.rat_list[j];
                      }
                      reg_mode_limited_service_rat_order.rat_list[j-1] = SYS_RAT_LTE_RADIO_ACCESS;
                      break;
                  }
              }
          }
#endif
      }
  }
#ifdef FEATURE_LTE
  if((reg_bit_mask_check(additional_info,
                         (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE) &&
     (reg_bit_mask_check(additional_info,
                         (byte)SYS_ADDTNL_INFO_RESTART_PLMN_SELECTION) == TRUE))
  {
    initial_rat = reg_mode_limited_service_rat_order.rat_list[0];
  }
  else
#endif
  if (((reg_state_service_status_get() == SYS_SRV_STATUS_SRV) ||
       (reg_state_service_status_get() == SYS_SRV_STATUS_LIMITED) ||
       (reg_state_service_status_get() == SYS_SRV_STATUS_LIMITED_REGIONAL)) &&
       (reg_mode_is_rat_present_in_rat_pri_list(
              reg_state_active_rat_get(), rat_pri_list_info, TRUE))
#ifdef FEATURE_LTE
       && ((reg_state_active_rat_get() != SYS_RAT_LTE_RADIO_ACCESS) 
#ifdef FEATURE_LTE_REL9
           ||
           (reg_bit_mask_check(additional_info,
                         (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE)
#endif
          )
#endif
     )
  {
    initial_rat = reg_state_active_rat_get();
  } 
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
  else if((type == REG_CM_SERVICE_REQ_SUITABLE_SEARCH) &&
          (reg_state_rlf_rat_get() == SYS_RAT_LTE_RADIO_ACCESS) &&
          (reg_mode_is_rat_present_in_rat_pri_list(reg_state_rlf_rat_get(), 
                                                   rat_pri_list_info, TRUE) == TRUE))
  {
    initial_rat = reg_state_rlf_rat_get();
    plmn = reg_state_rlf_plmn_get();
  }
#endif
  else if (reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_UMTS_RADIO_ACCESS, rat_pri_list_info, TRUE) && (reg_nv_is_wcdma_freq_lock_enabled() == TRUE))
  {
    initial_rat = SYS_RAT_UMTS_RADIO_ACCESS;
    reg_mode_limited_service_rat_order.num_items = 0;
  }
  else
  {
      initial_rat = reg_mode_limited_service_rat_order.rat_list[0];
      if(status == POLICYMAN_STATUS_SUCCESS)
      {
          if (specialization != SYS_SPECIAL_SBM_RAT_SEARCH_ORDER)
          {
              if((reg_mode_service_available_rat_get() != initial_rat) &&
                  (reg_mode_service_available_rat_get() != SYS_RAT_NONE) && 
                  (reg_mode_is_rat_present_in_rat_pri_list(
                  reg_mode_service_available_rat_get(), rat_pri_list_info, TRUE))
#ifdef FEATURE_SGLTE
                  && !(REG_SGLTE_DUAL_STACK && reg_mode_service_available_rat_get() == SYS_RAT_GSM_RADIO_ACCESS)
#endif
#ifdef FEATURE_LTE
                 && (reg_mode_service_available_rat_get() != SYS_RAT_LTE_RADIO_ACCESS)
#endif
                  )
              {
                  initial_rat = reg_mode_service_available_rat_get();
              }
          }
      }
  } 
  //Remove initial rat from rat_list. So that it in not tried next time
  for (i= 0; i < reg_mode_limited_service_rat_order.num_items; i++)
  {
    if(reg_mode_limited_service_rat_order.rat_list[i] == initial_rat)
    {
      for(j = i+1; j < reg_mode_limited_service_rat_order.num_items; j++)
      {
        reg_mode_limited_service_rat_order.rat_list[j-1] = 
           reg_mode_limited_service_rat_order.rat_list[j];
      }
      break;
    }
  }
  if(reg_mode_limited_service_rat_order.num_items>0)
  {
    reg_mode_limited_service_rat_order.num_items--;  
  }
#endif /*FEATURE_RAT_PRIORITY_LIST*/
  

  reg_send_mmr_reg_req
  (
    (reg_mode_network_selection_mode_get() ==
    SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION ?
    SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION :
    SYS_NETWORK_SELECTION_MODE_LIMITED),  /* network_selection_mode */
    plmn,                                 /* plmn                   */
    reg_mode_limited_service_domain,      /* req_service_domain     */
    reg_mode_mode_pref_get(),             /* mode_pref              */
    cs_for_request,                       /* cs_for_request         */
    ps_for_request,                       /* ps_for_request         */
#ifndef FEATURE_RAT_PRIORITY_LIST    
    reg_mode_limited_service_initial_rat  /* rat                    */
#else
    initial_rat
#endif
#ifdef FEATURE_GSM_BAND_PREF
    ,band_pref                             /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
    ,rat_pri_list_info
#endif
    ,serv_req_type
#ifdef FEATURE_LTE
    ,sms_only,
     additional_info
#endif
#ifdef FEATURE_FEMTO_CSG
    ,SYS_CSG_ID_INVALID
#endif 
  );
}


/*==============================================================================

FUNCTION NAME

  finished

==============================================================================*/
#ifndef FEATURE_RAT_PRIORITY_LIST
static boolean finished
(
        sys_plmn_service_state_s_type  service_state,
  const sys_detailed_plmn_list_s_type* available_plmn_list_p
)
{
  sys_radio_access_tech_e_type rat = SYS_RAT_GSM_RADIO_ACCESS;
  sys_plmn_id_s_type           plmn;
  boolean                      finished_plmn_selection = TRUE;
  mmr_reg_req_e_type  serv_req_type = MMR_REG_REQ_NORMAL;

  (void) available_plmn_list_p;

  
  if(reg_state_get_last_cm_service_req_type() == REG_CM_SERVICE_REQ_FULL_RAT_MODE)
  {
    serv_req_type = MMR_REG_REQ_FULL_RAT_MODE;
  }

  /*
  ** The MS found service on the initial RAT. 
  */
  if (service_state.service_status != SYS_SRV_STATUS_NO_SRV)
  {
    if ( (service_state.service_status == SYS_SRV_STATUS_SRV) &&
         (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) &&
         (service_state.roaming_ind == SYS_ROAM_STATUS_ON) )
    {
      reg_timers_start_hplmn_search_period_timer();
    }

    reg_mode_limited_service_started = FALSE;
  }
  /*
  ** The MS did not find service on the initial RAT.
  */
  else if ( (reg_mode_limited_service_started) &&
            (reg_mode_mode_pref_get() != SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY) &&
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
            #error code not present
#endif
            (reg_mode_mode_pref_get() != SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY) 
#ifdef FEATURE_LTE
            && (reg_mode_mode_pref_get() != SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY)
#endif
           )
  {

    if (reg_mode_limited_service_initial_rat == SYS_RAT_UMTS_RADIO_ACCESS)
    {
      rat = SYS_RAT_GSM_RADIO_ACCESS;
    }
    else if (reg_mode_limited_service_initial_rat == SYS_RAT_GSM_RADIO_ACCESS)
    {
      rat = SYS_RAT_UMTS_RADIO_ACCESS;
    }

    sys_plmn_undefine_plmn_id(&plmn);

    reg_send_mmr_reg_req
    (
      (reg_mode_network_selection_mode_get() ==
      SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION ?
      SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION :
      SYS_NETWORK_SELECTION_MODE_LIMITED),  /* network_selection_mode */
      plmn,                                 /* plmn                   */
      reg_mode_limited_service_domain,      /* req_service_domain     */
      reg_mode_mode_pref_get(),             /* mode_pref              */
      reg_mode_cs_for_request_get(),        /* cs_for_request         */
      reg_mode_ps_for_request_get(),        /* ps_for_request         */
      rat                                  /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
      ,reg_mode_band_pref_get()              /* band_pref              */
#endif
      ,serv_req_type
#ifdef FEATURE_LTE
     ,reg_mode_sms_only_get(),
      0
#endif
#ifdef FEATURE_FEMTO_CSG
     ,SYS_CSG_ID_INVALID
#endif 
    );

    finished_plmn_selection = FALSE;
    reg_mode_limited_service_started = FALSE;
  }
  return finished_plmn_selection;
}
#else
static boolean finished
(
  sys_plmn_service_state_s_type  service_state,
  const sys_detailed_plmn_list_s_type* available_plmn_list_p
)
{
  sys_radio_access_tech_e_type rat = SYS_RAT_GSM_RADIO_ACCESS;
  sys_plmn_id_s_type           plmn;
  boolean                      finished_plmn_selection = TRUE;
  uint8                        index;
  mmr_reg_req_e_type  serv_req_type = MMR_REG_REQ_NORMAL;  
  (void) available_plmn_list_p;

  if(reg_state_get_last_cm_service_req_type() == REG_CM_SERVICE_REQ_FULL_RAT_MODE)
  {
    serv_req_type = MMR_REG_REQ_FULL_RAT_MODE;
  } 
  

  /* filter out GSM from  main stack, on hybrid stack anyway after split, CM has to send another
  service req to activate hybrid stack, so no need to copy on hybrid.*/
#ifdef FEATURE_SGLTE
  if (REG_SGLTE_DUAL_STACK && (reg_as_id == reg_ps_stack_id))
  {
    reg_mode_remove_rat_from_3gpp_rat_list(&reg_mode_limited_service_rat_order,SYS_RAT_GSM_RADIO_ACCESS);
  }
#endif

  /*
  ** The MS found service on the initial RAT. 
  */
  if (service_state.service_status != SYS_SRV_STATUS_NO_SRV)
  {
    if ( (service_state.service_status == SYS_SRV_STATUS_SRV) &&
         (reg_mode_network_selection_mode_get() == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) &&
         (service_state.roaming_ind == SYS_ROAM_STATUS_ON) )
    {
      reg_timers_start_hplmn_search_period_timer();
    }
    reg_mode_limited_service_rat_order.num_items = 0;
  }
  /*
  ** The MS did not find service on the initial RAT.
  */  
  else if (reg_mode_limited_service_rat_order.num_items > 0)
  {

    rat = reg_mode_limited_service_rat_order.rat_list[0];

    //remove rat from rat_search_order
    for (index = 1; index < reg_mode_limited_service_rat_order.num_items; index++)
    {
       reg_mode_limited_service_rat_order.rat_list[index-1] = 
              reg_mode_limited_service_rat_order.rat_list[index];
    }
    reg_mode_limited_service_rat_order.num_items--;


      sys_plmn_undefine_plmn_id(&plmn);

      reg_send_mmr_reg_req
      (
        (reg_mode_network_selection_mode_get() ==
        SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION ?
        SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION :
        SYS_NETWORK_SELECTION_MODE_LIMITED),  /* network_selection_mode */
        plmn,                                 /* plmn                   */
        reg_mode_limited_service_domain,      /* req_service_domain     */
        reg_mode_mode_pref_get(),             /* mode_pref              */
        reg_mode_cs_for_request_get(),        /* cs_for_request         */
        reg_mode_ps_for_request_get(),        /* ps_for_request         */
        rat                                  /* rat                    */
#ifdef FEATURE_GSM_BAND_PREF
        ,reg_mode_band_pref_get()              /* band_pref              */
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
        ,reg_mode_rat_pri_list_get()      /*ue_capabilty*/
#endif 
        ,serv_req_type
#ifdef FEATURE_LTE
       ,reg_mode_sms_only_get()
       ,reg_mode_additional_info_get()
#endif
#ifdef FEATURE_FEMTO_CSG
       ,SYS_CSG_ID_INVALID
#endif 
      );

      finished_plmn_selection = FALSE;
    
  }
  return finished_plmn_selection;
}


#endif /*FEATURE_RAT_PRIORITY_LIST*/

/*==============================================================================

FUNCTION NAME

  number

==============================================================================*/

static reg_mode_e_type number ( void )
{
  return REG_MODE_LIMITED_SERVICE;
}


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

reg_mode_s_type reg_mode_limited_service = 
{
  reg_mode_unexpected_start_automatic_plmn_selection,
  reg_mode_unexpected_start_higher_priority_plmn_selection,
  start_limited_service,
  reg_mode_unexpected_start_manual_plmn_selection,
  reg_mode_unexpected_start_plmn_selection,
  reg_mode_unexpected_start_plmn_selection_roaming_plmn,
  reg_mode_unexpected_start_plmn_selection_on_specified_plmn,
  reg_mode_unexpected_start_foreground_plmn_search,
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

