
/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_update_lib.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/06/15   us      Correction of Spec version check in handling of Attach Reject with cause LTE_NAS_CONGESTION.  
09/12=7/12 cogolt  Added support for Rel-10 ESM backoff timer
07/05/12   sar     T3412 ext timer
05/29/12   pahire  Added changes for UE Specific DRX Cycle for QChat/LTE 
27/10/11   rajesh Indicating LTE as active rat while calling "gs_read_coded_classmark_2" function.
22/09/11   sla     Send MMR_REG_REJECT_IND when network sends msc tempororily 
                   not reachable
08/01/11   Rajesh  T3442 timer related code changes
30/08/11   MNK     Add dual RX CSFB support 
08/22/11   MNK     CR: 301219 Handle CSFB call if TAU is rejected/failed  
08/18/11   abhi    Indicating GUTI type in Attach/TAU request
07/13/11   rajesh  Handling expiry of T3412 and T3423 when ISR is ON
01/31/11   rajesh  voice domain preference and UE usage setting related changes
06/16/11   PA      Added changes for Rel 9 Emergency Bearer services 
05/04/11   sar     clear the forbidden list on timer expiry and 
                   modified to have 40 tai 24.301 5.3.2
12/13/10   abhi    Populating additional update type in attach/tau based upon sms_only 
                              field in REG_REQ. Added sending of additional update results from attch/tau
                              accept to CM.
10/07/10   MNK     1XCSFB introduction 
09/30/10   MNK     pended_esr_msg changed to a pointer 
09/24/10   MNK     Handle ESR & TAU collision
08/30/10   MNK     CSFB changes
07/12/10   zren    Updated to only start TAU retry in ATTEMPTING_TO_UPDATE and 
                   ATTEMPTING_TO_UPDATE_MM substates upon T3411 expires in 
                   EMM_REGISTERED state
06/29/10   zren    Updated to not reset detach attempt counter when starting
                   MO detach 
06/29/10   zren    Updated EMM to handle service request procedure and unsent 
                   messages correctly for redirection and reselection 
06/21/10   zren    Updated EMM to include correct MME routing info in conn est 
                   req for TAU after IRAT  
06/21/10   zren    Updated to not include UE network capability field in TAU 
                   Request for periodic TAU
06/21/10   abhi    Hard coded the UMTS UE sec capabilities to uea0, uea1 and
                   uia1 in ATTACH request
06/10/10   RI      Fixed the gprs cksn information in TAU
                   Hard coded the UMTS UE sec capabilities to uea0, uea1 and 
                   uia1 in TAU req
06/04/10   zren    Updated to start TAU when RLF is recovered in a new TA
06/04/10   zren    Updated to set plmn_service_capability for DEC 09 feature 
                   when starting initial TAU  
05/29/10   zren    Updated to check updated active flag value to send RRC SIM 
                   update after sending TAU REQ message
05/27/10   zren    Updated to set ACTIVE flag in TAU REQ if DS has data 
                   available
05/26/10   zren    Updated to send LIMITED SERVICE status in REG CNF for 
                   LIMITED SERVICE REQ when get service available service ind 
                   from RRC
05/24/10   zren    Updated NONCE UE handling upon receiving TAU ACCEPT 
                   Updated TX failure causes handling for TAU COMPLETE message
05/19/10   zren    Added support for GSTK EMM REGISTERED EVT and NW REJ EVT
05/18/10   zren    Updated fun param to set only_for_reg flag in REG CNF and 
                   REG IND
05/18/10   zren    Added NONCE UE support in TAU request
05/13/10   RI      Mainlined spec changes upto Dec 09.
05/12/10   zren    Updated to ignore abnormal attach reject cause in function
                   emm_registration_failure_change_state
05/07/10   zren    Added temp FPLMN feature
                   Fixed and consolidated equivalent PLMN list handling
04/28/10   zren    Fixed the issue of not sending REG CNF for EPS only attach
04/22/10   zren    Updated to update service domain upon detach completion
04/22/10   zren    Updated to use global structure for NAS outgoing message
04/14/10   MNK     ISR implementation changes
04/14/10   zren    Updated handling of abnormal attach reject cause values
04/14/10   zren    Consolidated GMM/MM handling code for combined procedures to
                   mm_multimode_hanlder
                   Added support to set SMS only falg in combined Attach/TAU 
                   request message
04/13/10   zren    Fixed an integration issue on tau attempt counter handling
04/12/10   zren    Fixed GUTI handling upon receiving Attach Accept/TAU Accept
04/09/10   zren    Added combined procedure feature
04/08/10   zren    Updated Attach/TAU attempt counter handling to be spec 
                   compliant
04/02/10   zren    Added failure handling for RRC connection release causes of
                   CRE_FAILURE and OOS_DURING_CRE 
03/16/10   MNK     More i-RAT changes 
03/16/10   zren    Updated MO detach procedure upon receiving MMR_STOP_MODE
03/12/10   zren    Updated EMM state changes upon receiving attach reject to be
                   spec compliant
03/05/10   MNK     TIN related changes
02/24/10   zren    Added support for network selection modes of LIMITED MANUAL
                   ACQUISITION and LIMITED ROUTINE ACQUISITION
02/23/10   MNK     Set MM state if UE is multimode when attach accept is receivedreceived      
02/23/10   RI      Added saving of context for detach accept.
02/20/10   RI      Updated the saving of EPS context for  MO/MT DETACH
02/17/10   MNK     Re-org the reject cause code to be able to re-use for i-RAT
02/16/10   MNK     Set tin type 
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
02/03/10   MNK     Autonoumous service_req generation if STOP_MODE results in mo_detach or any kind of detach that 
                   needs to go over the air 
01/30/10   MNK     Fix issues with deregister state.
01/22/10   RI      Added support for handling multiple EPS contexts.
01/07/10   zren    Updated release client and pended message processing
12/28/09   zren    Updated to not process EMM cause of IMSI_UNKNOWN_IN_HSS in
                   MT detach since it is not fully supported yet
12/18/09   RI      Copy the UE security capabilities into Attach Request from
                   EFS data.
12/16/09   zren    Updated to support GUTI or IMSI as EPS mobile 
                   identification in TAU request message
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
12/10/09   zren    Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
11/10/09   RI      Moved the state transition before sending the 
                   Attach/Detach/TAU messages
10/20/09   RI      Security changes - send a SIM update to RRC if active flag is set
                   in TAU request.
10/06/09   RI      Fix compiler warnings.
09/21/09   MNK     Changes due to IRAT design
09/24/09   zren    Fixed compilation warnings due to missing default in switch
                   statement
09/17/09   zren    Updated to process timers after TAU request is sent for TAU
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
09/11/09   zren    Added debug message for NW Detach not handling the EMM cause
                   for type Re-attach Req
                   Added code to save the updated EPS status 
08/31/09   RI      Added  sim update to RRC with NAS count info if security is valid
08/19/09   vdr     Added TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN
                   as a possible failure cause during TAU procedure
08/14/09   zren    Modified procedure connection release handling
07/29/09   hnam    Changed the name of the MACRO (for MARCH/DEC spec compatibiltiy), 
                   used the standard macro commonly used by LTE_MODEM_SW
07/15/09   zren    Modified message list handling functions to use message 
                   pointer as parameter
07/10/09   zren    Added EMM state check before handling attach request failure
07/07/09   zren    Featurized reattach required MT Detach procedure to follow
                   either 2008 Dec or 2009 March spec 
07/01/09   tmai    Add support to trigger detach when TAU process complete during
                   detach abort
06/23/09   zren    Added support of processing EPLMN list, forbidden PLMN list,
                   forbidden TAI list for roaming and forbidden TAI list for
                   regional provision of service 
06/13/09   RI      Deleted the NAS security mode cmd functions.
06/11/09   zren    Added support to trigger TAU when UE returns back to 
                   REGISTERED_NORMAL_SERVICE state and EPS bearer context was
                   updated in earlier REGISTERED_NO_CELL_AVAILABLE state
06/03/09   zren    Updated code to send out Detach Accept message for the case
                   of reattach not required type detach request without any EMM 
                   cause
06/02/09   MNK     CR# 184007. These are the changes required to make sure at the time of attach, 
                   EMM receives (ATTACH ACCEPT + ACTIVATE DEFAULT BEARER CONTEXT REQUEST) and EMM sends
                    (ATTACH COMPLETE + ACTIVATE DEFAULT BEARER CONTEXT ACCEPT)
05/29/09   zren    Added support for TAU feature
05/27/09   zren    Added support for collision of attach and MT detach(CR183806)
05/19/09   hnam    Added support for transmission failure scenario - when 
                   connection is released by RRC during any UL procedure failures
05/11/09   zren    Added support for MT detach procedure
                   Added headers and debug messages for MO detach functions
                   Fixed some lint warnings  
04/21/09   hnam    Added T3440 timer support (required during attach and service reject)
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
04/15/09   RI      Added support for LTE NAS Authentication.
04/15/09   zren    Completed MO detach feature
03/04/09   vdr     Now updating plmn_service_state upon sending initial Attach Request
                   message and reception of the Attach Reject message
03/26/09   RI      Added support for GS Timers in LTE.
03/24/09   zren    Added support for service request and paging feature
03/12/09   RI      Added support for SMC.
03/12/09   MNK     Now sends detach indication to ESM whenever STOP mode is received from the CM
02/20/09   vdr     Now cleaning up structure representation of the outgoing ESM OTA messages
02/12/09   MNK     Removed the ue_id and even_odd_indication while passing the ue id information in the 
                   encoding of attach and detach messages
02/11/09   vdr     Registering emm_attach_release_indication(), emm_poweroff_detach_release_ind()
                   and emm_mo_detach_release_ind which will be called upon reception of the RRC 
                   Release indication
02/06/09   vdr     Modified failure delivery callbacks in order to delete message 
                   from the cache mem
02/05/09   MNK     Changed esm_get_pdn_connectivity_msg to esm_get_pdn_connectivity_msg_fp and encode_nas_msg to 
                   encode_nas_msg_fp
01/29/09   vdr     Now sending emm_send_rrc_system_update() when any of system info changes
                   Now sending emm_send_sim_update() when GUTI changes
02/03/09   vdr     Added processing for LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN cause
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "emm.h"
#include "rex.h"
#include "emm_update_lib.h"
#include "emm_database.h"
#include "emm_connection_handler.h"
#include "emm_utility.h"
#include "emm_timer_sigs.h"
#include "lte_nas_common_v.h"
#include "lte_nas.h"
#include "lte_nas_msg_parse.h"
#include "msgr.h"
#include "msgr_lte.h"
#include "sys.h"
#include "sys_plmn_selection_v.h"
#include "emm_rrc_if.h"
#include "emm_sim_handler.h"
#include "esm_emm_msg_if.h"
#include "esm_cm_msg_if.h"
#include "mm_v.h"
#include "libprot.h"
#include "emm_esm_handler.h"
#include "emm_security.h"
#include "emm_procedure_reject_handler.h"
#include "err.h"
#include "mm_multimode_common.h"
#include "mm_coord.h"
#include "gmm_emm_common.h"
#include "sys_stru_v.h"
#include "reg_sim_v.h"
#include "ds3gpp_api.h"
#include "emm_service_request_lib.h"
#include "emm_mem.h"
#include "emm_csfb_handler.h"
#include "stringl.h"
#include "reg_timers.h"
#include "fs_public.h"
#include "mcfg_fs.h"
#include "cm_v.h"
#include "reg_mode.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif

#ifdef TEST_FRAMEWORK
#error code not present
#endif

boolean is_nas_emm_attach_tau_timeout = FALSE;
emm_mo_detach_type nas_emm_power_off_detach_type = INVALID_DETACH_TYPE;
boolean nas_emm_tau_cs_rej = FALSE;
lte_nas_emm_eps_update_val_type nas_emm_tau_cs_rej_tau_type = TA_UPDATING;

extern boolean emm_disable_geran_cap;
extern emm_pended_msg_list_type *emm_find_pended_esr
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
);
extern void emm_registered_process_possible_tau_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
); 
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*===========================================================================
FUNCTION    emm_read_coded_classmark_2

DESCRIPTION
  This function calls GS classmark APIs and encodes the classmark 2 values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_read_coded_classmark_2(
  classmark_2_ie_T  *classmark_2_ptr,
  boolean           gsm_supported,
  boolean           wcdma_supported
)
{
  classmark_params_T classmark_params;
  classmark_params.gsm_supported   = gsm_supported;
  classmark_params.wcdma_supported = wcdma_supported;
  classmark_params.wcdma_in_use    = TRUE; // LTE is the active RAT
  classmark_params.MSCR99          = TRUE; // This is LTE NW, which is later revision than R99
  classmark_params.rat             = SYS_RAT_LTE_RADIO_ACCESS;
#ifdef FEATURE_DUAL_SIM
  classmark_params.as_id           = (sys_modem_as_id_e_type)mm_sub_id;
#endif
  gs_read_coded_classmark_2_with_rat(classmark_2_ptr, &classmark_params );
}


/*===========================================================================
FUNCTION    emm_read_coded_classmark_3

DESCRIPTION
  This function calls GS classmark APIs and encodes the classmark 3 values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_read_coded_classmark_3(
  classmark_3_ie_T  *classmark_3_ptr,
  boolean           gsm_supported,
  boolean           wcdma_supported
)
{
  classmark_params_T classmark_params;
  classmark_params.gsm_supported   = gsm_supported;
  classmark_params.wcdma_supported = wcdma_supported;
  classmark_params.wcdma_in_use    = TRUE; // LTE is the active RAT
  classmark_params.MSCR99          = TRUE; // This is LTE NW, which is later revision than R99
  classmark_params.rat             = SYS_RAT_LTE_RADIO_ACCESS;
#ifdef FEATURE_DUAL_SIM
  classmark_params.as_id           = (sys_modem_as_id_e_type)mm_sub_id;
#endif
  gs_read_coded_classmark_3_with_rat(classmark_3_ptr, &classmark_params );
}




/*===========================================================================
FUNCTION    EMM_REEVALUATE_T3402_VALUE

DESCRIPTION
  This function checks if UE needs to reset T3402 value based on current camped PLMN or registered TAI.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If T3402 value needs to be reset to default value
  FALSE: If UE should use current stored T3402 value.

SIDE EFFECTS
  None
===========================================================================*/

boolean emm_reevaluate_t3402_value
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  boolean set_t3402_to_default = TRUE;
  lte_nas_tai_lst1_type  current_tai;
  ASSERT(emm_ctrl_data_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->emm_tai_list_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);
  /*
    EMM needs to recover TAI info from latest RRC Service or 
    Connected Mode Cell Change indications
  */
  current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->
                                                                                 camped_svc_info.selected_plmn) ;
  current_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac ;

  /* If t3402 value is sent by the network in attach reject that apply the value to
     the PLMN where this value is received,  else apply the T3402 value to the last 
     registered TAI value or TAI list if present*/
  if(emm_ctrl_data_ptr->t3402_attach_rej_param.is_t3402_recv_attach_rej == TRUE)
  {
     if(sys_plmn_match( emm_ctrl_data_ptr->t3402_attach_rej_param.plmn,
                     emm_ctrl_data_ptr->plmn_service_state->plmn) == TRUE)
     {
       set_t3402_to_default = FALSE;
     }
  }
  else if(emm_ctrl_data_ptr->emm_tai_list_ptr->tai_lst_length != 0)
  {
    if(emm_search_tai_list(&current_tai,emm_ctrl_data_ptr->emm_tai_list_ptr)
                                                    == TRUE)
    {
      set_t3402_to_default = FALSE;
    }
  }
  else if((emm_ctrl_data_ptr->last_registered_tai_valid == TRUE)&&
          (sys_plmn_match(emm_ctrl_data_ptr->last_registered_tai.plmn,
           emm_ctrl_data_ptr->plmn_service_state->plmn) == TRUE)&&
          (emm_ctrl_data_ptr->last_registered_tai.tac == 
           emm_ctrl_data_ptr->plmn_service_state->tac))
  {
    set_t3402_to_default = FALSE;
  }
  return set_t3402_to_default;
}



/*===========================================================================
FUNCTION    EMM_START_ATTACH_RECOVERY

DESCRIPTION
  This function performs attach recovery procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/


void emm_start_attach_recovery
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  mmr_cause_e_type  mmr_status;
  boolean set_t3402_to_default = TRUE;
#ifdef FEATURE_LTE_REL10
  boolean ext_wait_time_processed = FALSE;
#endif
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  /* If T3410 is still running, stop it */
  mm_stop_timer(TIMER_T3410);

#ifdef FEATURE_LTE_REL10
  /* 5.5.1.2.6: abnormal case l */
  ext_wait_time_processed = emm_handle_extended_wait_time(emm_ctrl_data_ptr);
#endif

#ifdef FEATURE_LTE_REL9 
#ifdef FEATURE_LTE_REL10
  if(ext_wait_time_processed == TRUE)
  {
    emm_ctrl_data_ptr->emm_attach_attempt_counter = 0;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Ext wait time processed. Resetting attach attempt counter");
  }
  else
#endif
  if(((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_PENDING) &&
      (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == FALSE))||
      (emm_skip_emc_retry() == FALSE))
  {
#endif
    emm_ctrl_data_ptr->emm_attach_attempt_counter++;
#ifdef FEATURE_LTE_REL9 
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Not updating attempt counter - EMC SRV pending");
  }
#endif

  if (emm_ctrl_data_ptr->emm_attach_attempt_counter < MAX_NUMBER_ATTACH_OF_ATTEMPTS)
  {
#ifdef FEATURE_LTE_REL9 
    if((((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_PENDING) &&
      (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == FALSE))||
      (emm_skip_emc_retry() == FALSE))
#ifdef FEATURE_LTE_REL10
       && 
       (ext_wait_time_processed == FALSE)
#endif
      )
    {
#endif
      mm_stop_timer(TIMER_T3411) ;
      mm_start_timer(TIMER_T3411, EMM_T3411_TIMER_DURATION) ;
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Started timer T3411");
#ifdef FEATURE_LTE_REL9 
    }
    else
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Not starting T3411 - EMC SRV pending or Ext wait time received");
    }
#endif

    if((emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)
       && 
       (emm_ctrl_data_ptr->emm_location_area->location_update_status != 
                                                                      UPDATED)
#ifdef FEATURE_LTE_REL10
       && 
       (ext_wait_time_processed == FALSE)
#endif
      )
    {
      /* 
      ** if the attach attempt counter is less than 5 and, additionally, 
      ** the update status is different from U1 UPDATED, then the UE shall 
      ** delete any LAI, TMSI, ciphering key sequence number and list of 
      ** equivalent PLMNs and set the update status to U2 NOT UPDATED
      */
      mm_invalidate_lu_info(NOT_UPDATED);

      emm_ctrl_data_ptr->update_eplmn_list = TRUE;
      emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
    }
    
#ifdef FEATURE_LTE_REL9 
    if(((emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING) ||
       (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == TRUE))&&
       (emm_skip_emc_retry() == TRUE))
    {
      emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE);
      reg_service_report(MMR_CAUSE_ABORT_PLMN_SELECTION, 
                         FALSE, 
                         emm_ctrl_data_ptr);
      return;
    }
#endif
  }
  else
  {
#ifdef FEATURE_LTE_REL10
    /* This flag could be set if Attach is triggered due to paging.
       reset the flag */
    emm_ctrl_data_ptr->responding_to_page = FALSE;
#endif
    mmr_status = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;

    if(emm_ctrl_data_ptr->t3402_value != EMM_DEFAULT_T3402)
    {
      set_t3402_to_default = emm_reevaluate_t3402_value(emm_ctrl_data_ptr);
    }
    else
    {
      /* If T3402 is already set to default value then no need to send
         NAS_EMM_T3402_CHANGED_IND */
      set_t3402_to_default = FALSE;
    }

    if(set_t3402_to_default == TRUE)
    {
      emm_ctrl_data_ptr->t3402_value = EMM_DEFAULT_T3402;
      emm_send_t3402_changed_ind(emm_ctrl_data_ptr->t3402_value);
    }

    if (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_CS_PS)
    {
      /*
      ** if the attach attempt counter is equal to 5, then the UE shall delete
      ** any LAI, TMSI, ciphering key sequence number and set the update status 
      ** to U2 NOT UPDATED.
      */
      mm_invalidate_lu_info(NOT_UPDATED);
    }

    /* Always update GMM/MM before EMM processing to allow GMM/MM writes sim */
    if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,
                                       SYS_RAT_UMTS_RADIO_ACCESS)||
       mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,
                                       SYS_RAT_GSM_RADIO_ACCESS)|| 
       mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                       SYS_RAT_TDS_RADIO_ACCESS))
    {
      mm_set_gmm_param_attach_abnormal_failure();
    }

    emm_ctrl_data_ptr->manual_user_sel = FALSE;

    /* Delete GUTI */
    /* Delete last registered visited TAI */
    /* TAI List */
    /* Delete E-PLMN list */
    /* Invalidate NAS Security parameters */ 
    emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED,
                                 SYS_SIM_STATE_AVAILABLE,
                                 emm_ctrl_data_ptr, TRUE);
    /* Reset attach attempt conter */
    emm_ctrl_data_ptr->emm_attach_attempt_counter = 0;
    /* Reset the attach_rej with cause ESM_FAILURE counter*/
    emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter = 0;
    emm_ctrl_data_ptr->apn_switch = FALSE;
    emm_send_reset_apn_switch_ind();
    mm_stop_timer(TIMER_T3402) ;

    mm_start_timer(TIMER_T3402,emm_ctrl_data_ptr->t3402_value) ;

    if( emm_skip_emc_retry() == TRUE )
    {
      emm_handle_temporary_forbidden_plmn(emm_ctrl_data_ptr);
    }

#ifdef TEST_FRAMEWORK
    #error code not present
#endif
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Started timer T3402");

    /*Indicate to upper layer that there is LIMITED_SERVICE*/
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
   
    /* Add PLMN to temp forbidden if 
            i) device is voice centric and max attempt fails for combined registration
            ii) Aggression management is enabled*/
    if(((emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.is_voice_domain_ie_valid
                                                                                   == TRUE)&&
         (emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.voice_or_data_centric 
                                                           == SYS_UE_USAGE_SETTING_VOICE_CENTRIC) && 
       (emm_ctrl_data_ptr->eps_attach_type == COMBINED_EPS_IMSI_ATTACH)) ||
       (emm_ctrl_data_ptr->aggression_management == TRUE))
      {
        if(emm_ctrl_data_ptr->lte_disable_duration_timer_nv != 0)
        {
          emm_add_plmn_to_backoff_fplmn_list(emm_ctrl_data_ptr->plmn_service_state->plmn,
                    emm_ctrl_data_ptr->lte_disable_duration_timer_nv,SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE);
        }
        else
        {
          emm_add_plmn_to_backoff_fplmn_list(emm_ctrl_data_ptr->plmn_service_state->plmn,
                    emm_ctrl_data_ptr->t3402_value,SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE);
        }
      }

    emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_NO_SRV;
    emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;

#ifdef FEATURE_LTE_REL9 
    if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING)
    {
      emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);
    }
#endif

    reg_service_report(mmr_status, 
                       FALSE, 
                       emm_ctrl_data_ptr);
    emm_send_reg_attach_failed_ind();                              
                  
  }

  EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH) ;
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Attach attempt counter in attach recovery procedure = %d",
                emm_ctrl_data_ptr->emm_attach_attempt_counter);
} /* end of emm_start_attach_recovery */

/*===========================================================================
FUNCTION    EMM_ATTACH_REQUEST_FAILED

DESCRIPTION
  This function processes following failures in sending attach request message
  - RRC_UL_DATA_CNF with failure causes
  - RRC_CONN_EST_FAILURE
  - RRC_CONN_REL

DEPENDENCIES
  None

RETURN VALUE
  TRUE: TAU is required
  FALSE: TAU is not required

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_attach_request_failed
(
  lte_emm_connection_cause_type  cause,
  dword                          trans_id,
  emm_failure_type               emm_failure_cause,
  lte_nas_tai_lst1_type          tac,
  struct emm_pended_msg_list_tag *message_ptr,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  emm_pended_msg_list_type *pended_message_ptr = (struct emm_pended_msg_list_tag *)message_ptr ;
  emm_state_type            emm_state = (emm_state_type)EMM_UNKNOWN_STATE;
  emm_substate_type         emm_substate = EMM_UNKNOWN_SUBSTATE ;

  ASSERT(pended_message_ptr != (emm_pended_msg_list_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  emm_send_diag_event(EVENT_NAS_ATTACH, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_LOWER_LAYER_FAILURE, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);

  emm_state = EMM_GET_STATE();
  if(emm_state == EMM_REGISTERED_INITIATED)
  {
    emm_remove_release_client(emm_attach_release_indication);

    emm_substate = EMM_GET_SUBSTATE() ;

    switch (cause)
    {
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_RLF:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
        switch (emm_substate)
        {
          case EMM_WAITING_FOR_ESM_RESPONSE: 
            MSG_ERROR_DS_3(MM_SUB, "=EMM= ATTACH REQ tx failure %d - state %d, substate %d",
                           cause,
                           emm_state,
                           emm_substate);
            break ;
          case EMM_WAITING_FOR_NW_RESPONSE:
            if(emm_ctrl_data_ptr->aborting_conn == FALSE)
            {
              MSG_ERROR_DS_0(MM_SUB, "=EMM= Attach attempt failed. Start over.");
              /* Restart Attach procedure */
              emm_init_attach_procedure(EMM_UPDATE_RESTART,
                                        emm_ctrl_data_ptr);
            }
            else
            {
              MSG_HIGH_DS_0(MM_SUB, "=EMM= Aborting conn - Do not start re-attach");
              mm_stop_timer(TIMER_T3410);  
              EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
            }
            break ;
          default:
            MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM substate %u",emm_substate,0,0) ;
            break ;
        } 
        break ;
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
#ifdef FEATURE_NAS_EAB
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
#endif
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Attach request failed due to conn est failure %d",
                      cause);
        MSG_HIGH_DS_0(MM_SUB, "=EMM= T3410 stopped - Access barred/Conn Rej");
        mm_stop_timer(TIMER_T3410);
#ifdef FEATURE_LTE_REL9 
        /*skip re-attach procedure for emergency attach failure and release pdp context connection req*/
        if(emm_ctrl_data_ptr->pdn_conn_req_ptr->req == ESM_EMERGENCY && 
           emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING)
        {
           MSG_HIGH_DS_0(MM_SUB, "=EMM= Aborting EMC Attach ");
           EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);
           emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
           emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;
           emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_LIMITED;
           emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);
           reg_service_report(MMR_CAUSE_FAILURE_MAX_ATTEMPTED, FALSE, emm_ctrl_data_ptr);
        }
#ifdef FEATURE_LTE_REL10
        /* 5.5.1.2.6: abnormal case l
           Process extended wait time from lower layers only when connection establishment
           reject cause is CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT */
        else if((cause == CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT) &&
                emm_handle_extended_wait_time(emm_ctrl_data_ptr) == TRUE)
        {
          emm_ctrl_data_ptr->emm_attach_attempt_counter = 0;
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH);
        }
#endif
        else
        {
            EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED);
        }
#else
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED);
#endif
        /*Not sure if we need to send LIMITED_SERVICE event to GSTk*/
        break;
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ABORTED:
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Conn req aborted - Do not start attach recovery");
        mm_stop_timer(TIMER_T3410);  
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
        break;
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CELL_RESEL:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_SI_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TAI_CHANGED:
        switch (emm_substate)
        {
          case EMM_WAITING_FOR_ESM_RESPONSE: 
            MSG_FATAL_DS(MM_SUB,"=EMM= CAN NOT HAPPANED!!!",0,0,0) ;
            break ;
          case EMM_WAITING_FOR_NW_RESPONSE:
            if(emm_ctrl_data_ptr->reg_req_pending_ptr->rat != SYS_RAT_LTE_RADIO_ACCESS &&
               emm_get_reg_req_pending_status() == TRUE)
            {
              EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
              mm_stop_timer(TIMER_T3410);
            }
            else
            {
               /* 5.5.1.2.6 Abnormal cases in the UE: b, c, d */
              emm_start_attach_recovery(emm_ctrl_data_ptr) ;
            }
            break ;
          default:
            MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM substate %u",emm_substate,0,0) ;
            break ;
        } 
        break ;

#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Connection est failure due to TRM");
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH) ;
        mm_stop_timer(TIMER_T3410);
        mm_start_timer(TIMER_EMM_RADIO_RETRY,DEFAULT_TIMEOUT);
        break;
#endif 

      case CONN_REL_LTE_RRC_CONN_REL_NORMAL:
      case CONN_REL_LTE_RRC_CONN_REL_RLF:
      case CONN_REL_LTE_RRC_CONN_REL_CRE_FAILURE:
      case CONN_REL_LTE_RRC_CONN_REL_OOS_DURING_CRE:
      case CONN_REL_LTE_RRC_CONN_REL_ABORTED:
      case CONN_REL_LTE_RRC_CONN_REL_SIB_READ_ERROR:
      case CONN_REL_LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS:
        /* Connection release is handled by procedure release indication client 
           for this case. Do nothing here. */
        break;
      default :
        MSG_FATAL_DS(MM_SUB,"=EMM= Illegal EMM connection cause %u",(uint32)cause,0,0) ;
        break ;
    }
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Skip processing attach request failure in state %d",
                  emm_state);
  }

  pended_message_ptr->message_sent_indicator = TRUE ;

  /* Do not perform TAU procedure */
  return FALSE ;
} /* end of emm_attach_request_failed() */

#ifdef LTE_EMM_STANDALONE_TEST
#ifdef TEST_FRAMEWORK
 #error code not present
#endif // TEST_FRAMEWORK
#else
#ifdef TEST_FRAMEWORK
 #error code not present
#endif // TEST_FRAMEWORK
#endif

/*===========================================================================

FUNCTION  EMM_ATTACH_RELEASE_INDICATION

DESCRIPTION
  This function handles the case of connection is released before ATTACH complete

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_attach_release_indication
(
  lte_emm_connection_cause_type  release_cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr,
  lte_rrc_conn_rel_redir_info_s  redir_info,
  lte_nas_tau_cause_type tau_cause
)
{
  emm_state_type                 emm_state    = EMM_INVALID_STATE ;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  emm_send_diag_event(EVENT_NAS_ATTACH, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_LOWER_LAYER_FAILURE, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);

  emm_state = EMM_GET_STATE() ;

  if (emm_state == EMM_REGISTERED_INITIATED)
  {
    if(emm_ctrl_data_ptr->reg_req_pending_ptr->rat != SYS_RAT_LTE_RADIO_ACCESS &&
       emm_get_reg_req_pending_status() == TRUE)
    {
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
      mm_stop_timer(TIMER_T3410);
    }
    else
    {
      emm_start_attach_recovery(emm_ctrl_data_ptr) ;
    }
  }
} /* end of emm_attach_release_indication() */

/*===========================================================================
FUNCTION    EMM_INIT_ATTACH_PROCEDURE

DESCRIPTION
  This function initializes attach procedure.
  - If cached PDN_CONNECTIVITY_REQ message is valid/reusable, start attach 
    procedure directly
  - If cached PDN_CONNECTIVITY_REQ message is invalid/nonreusable, this function
    sends GET_PDN_CONNECTIVITY_REQ_IND to ESM and moves EMM into DEGEGISTERED 
    WAITING_PDN_CONN_REQ state. Once EMM receives new PDN_CONNECTIVITY_REQ in 
    this state later, EMM starts attach procedure with EMM_INITIAL_UPDATE type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_init_attach_procedure
(
  emm_update_attempt_type  attach_attemp_type,
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  if(attach_attemp_type == EMM_INITIAL_UPDATE)
  {
    emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter = 0;
    emm_ctrl_data_ptr->apn_switch = FALSE;
  }

  if(emm_ctrl_data_ptr->pdn_conn_req_trans_id == 
                                             EMM_INVALID_PDN_CONN_REQ_TRANS_ID)
  {
    /* Cached PDN_CONNECTIVITY_REQ is not reusable. Try to get a new one. */
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Send request to get PDN_CONNECTIVITY_REQ message");
    /* Send NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND */
    emm_build_and_send_get_pdn_connectivity_req_ind(emm_ctrl_data_ptr);
    /* Update EMM state */
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEGEGISTERED_WAITING_PDN_CONN_REQ);
  }
  else
  {
    /* Cached PDN_CONNECTIVITY_REQ is reusable. Start attach procedure. */
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Start attach procedure with cached PDN_CONNECTIVITY_REQ");
    emm_start_attach_procedure(attach_attemp_type, emm_ctrl_data_ptr);
  }
} /* end of emm_init_attach_procedure */

/*===========================================================================
FUNCTION    EMM_START_ATTACH_PROCEDURE

DESCRIPTION
  This function starts attach procedure by creating and sending
  EPS Attach Request message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  o This function stops (if any): TIMER_T3402 and TIMER_T3411
  o This function starts TIMER_T3410
  o This function moves EMM to 
    EMM_MOVES_TO_REGISTERED_INITIATED_STATE(EMM_WAITING_FOR_NW_RESPONSE) if
    EPS Attach Request has been sent successfully or to 
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED) if tx
    fails. In case of tx failure TIMER_T3410 is stopped
===========================================================================*/
void emm_start_attach_procedure
(
  emm_update_attempt_type  attach_attemp_type,
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  dword                        sequence_number = 0 ;
  lte_nas_emm_attach_req_type* emm_attach_request  = NULL;
  int i;

  boolean                      gsm_spt  = FALSE;
  boolean                      umts_spt = FALSE;
  boolean                      tds_spt  = FALSE;
  emm_security_context_info *context_ptr = NULL;
  lte_rrc_est_cause_e          conn_est_cause = LTE_RRC_EST_CAUSE_MO_SIGNALING;

  ASSERT(emm_ctrl_data_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  memset((void *)emm_ctrl_data_ptr->out_msg_ptr, 
         0x0, 
         sizeof(lte_nas_outgoing_msg_type));

  emm_attach_request = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.attach_req;

  /*Save the LAST attempted TAI before we send ATTACH_REQ*/
  emm_save_last_attempted_tai(emm_ctrl_data_ptr);

  /* Reset SG assoc flag in case it is set before UE is detached earlier */
  if(emm_ctrl_data_ptr->sg_association_to_be_established != NAS_EMM_SG_ASSOC_NOT_REQ)
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Reset SGs assoc flag cause (%d) - ATTACH",
                  emm_ctrl_data_ptr->sg_association_to_be_established);
    emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_NOT_REQ; 
  }

  emm_ctrl_data_ptr->imsi_attach = FALSE;

  if((attach_attemp_type == EMM_INITIAL_UPDATE) ||
     (attach_attemp_type == EMM_UPDATE_RESTART))
  {
    emm_stop_all_timers(emm_ctrl_data_ptr) ;

    if(attach_attemp_type == EMM_INITIAL_UPDATE)
    {
      emm_ctrl_data_ptr->emm_attach_attempt_counter = 0;
      emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter = 0;
      emm_ctrl_data_ptr->apn_switch = FALSE;
    }

    emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED ;

    emm_get_plmn_state_info(SYS_SRV_STATUS_SRV, emm_ctrl_data_ptr);
  }

  emm_attach_request->lte_nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES ;

  emm_attach_request->lte_nas_hdr.msg_id = ATTACH_REQUEST ;

  emm_attach_request->security_hdr = PLAIN_NAS_MSG ;

#ifdef FEATURE_LTE_REL9
  if(emm_ctrl_data_ptr->pdn_conn_req_ptr->req == ESM_EMERGENCY)
  {
    emm_attach_request->eps_attach_req = EPS_EMERGENCY_ATTACH;
  }
  else
#endif
  if(emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)
  {  
    if((emm_is_combined_reg_allowed(emm_ctrl_data_ptr) == TRUE) &&
       (emm_ctrl_data_ptr->plmn_service_state->sim_state ==  
                                                   SYS_SIM_STATE_AVAILABLE))
    {
      emm_attach_request->eps_attach_req = COMBINED_EPS_IMSI_ATTACH;
    }
    else if((emm_ctrl_data_ptr->plmn_service_state->sim_state ==
                                                  SYS_SIM_STATE_AVAILABLE) ||
            (emm_ctrl_data_ptr->plmn_service_state->sim_state ==
                                                  SYS_SIM_STATE_CS_INVALID))
    {
      emm_attach_request->eps_attach_req = EPS_ONLY_ATTACH;
      
      if((emm_is_combined_reg_allowed(emm_ctrl_data_ptr) == TRUE) &&
         (emm_ctrl_data_ptr->plmn_service_state->sim_state ==
                                                  SYS_SIM_STATE_CS_INVALID))
      {
        MSG_HIGH_DS_0(MM_SUB, "EPS ONLY Attach done due to SIM state CS Invalid");
      }
    }
    else
    {
      MSG_FATAL_DS(MM_SUB,"=EMM= Incorrect SIM state %d",
             emm_ctrl_data_ptr->plmn_service_state->sim_state,0,0);
    }
  }
  else
  {
    emm_attach_request->eps_attach_req = EPS_ONLY_ATTACH ;
  }

#ifdef FEATURE_LTE_REL9
  if((emm_ctrl_data_ptr->pdn_conn_req_ptr->req == ESM_EMERGENCY) 
     ||
     (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == TRUE))
  {
    conn_est_cause = LTE_RRC_EST_CAUSE_EMERGENCY;
  }
#endif

  emm_ctrl_data_ptr->eps_attach_type = emm_attach_request->eps_attach_req;

  if (is_nas_emm_attach_tau_timeout == TRUE || emm_ctrl_data_ptr->emm_attach_attempt_counter == 0)
  {
    emm_send_diag_event(EVENT_NAS_ATTACH, (nas_event_end_result)PROC_START, (nas_event_end_reason)PROC_END_NO_REASON, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
    is_nas_emm_attach_tau_timeout = FALSE;
  } 
  else
  {
    emm_send_diag_event(EVENT_NAS_ATTACH, (nas_event_end_result)PROC_RESEND, (nas_event_end_reason)PROC_END_NO_REASON, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
  }

  /* Note: The KSI used here is that from the current context. If the
           EPS native context KSI is needed, update this info from EFS or USIM
  */
  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  if( context_ptr != NULL )
  {
    emm_attach_request->nas_key_set_identifier = context_ptr->nasKSI;

    MSG_HIGH_DS_0(MM_SUB, "=EMM= KASME - including index [0-31]");
    for(i=0; i+7< 32; i= i+8)
    {
      MSG_HIGH_DS_8(MM_SUB, "=EMM= 0x%x  0x%x 0x%x  0x%x  0x%x  0x%x  0x%x  0x%x", 
                    context_ptr->nas_kasme[i],context_ptr->nas_kasme[i+1],context_ptr->nas_kasme[i+2],context_ptr->nas_kasme[i+3],
                    context_ptr->nas_kasme[i+4],context_ptr->nas_kasme[i+5],context_ptr->nas_kasme[i+6],context_ptr->nas_kasme[i+7]);
    }
    
  } 
  else 
  {
    emm_attach_request->nas_key_set_identifier.keyset_id = NO_KEY_AVAIALABLE ;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= No KASME available to print ");
  }
  /* MNK; Spec section 5.5.1.2.2 */

  gsm_spt  = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_GSM_RADIO_ACCESS);
  umts_spt = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_UMTS_RADIO_ACCESS);
  tds_spt  = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_TDS_RADIO_ACCESS);

  if((gsm_spt == FALSE) && (umts_spt == FALSE) && (tds_spt == FALSE))
  {
    if(emm_ctrl_data_ptr->emm_guti_valid == TRUE)
    {
      MSG_LOW_DS_0(MM_SUB, "=EMM= Attach Req EPS Mobile Identity - GUTI");
      emm_attach_request->eps_mobile_id.ue_id = GUTI_ID ;
      emm_attach_request->eps_mobile_id.ue_id_info.guti = 
                                                     emm_ctrl_data_ptr->emm_guti;

      emm_attach_request->guti_type_exists = TRUE;
      emm_attach_request->guti_type = LTE_NAS_GUTI_TYPE_NATIVE;
    }
    else if((emm_ctrl_data_ptr->plmn_service_state->sim_state ==  
                                               SYS_SIM_STATE_AVAILABLE)
#ifdef FEATURE_LTE_REL9
           ||((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                               SYS_SIM_STATE_CS_INVALID) &&
              (emm_ctrl_data_ptr->pdn_conn_req_ptr->req != ESM_EMERGENCY))
#endif
           )
    {
      MSG_LOW_DS_0(MM_SUB, "=EMM= Attach Req EPS Mobile Identity - IMSI");
      emm_populate_imsi(&emm_attach_request->eps_mobile_id, emm_ctrl_data_ptr);
    }
#ifdef FEATURE_LTE_REL9
    else if(emm_ctrl_data_ptr->pdn_conn_req_ptr->req == ESM_EMERGENCY)
    {
      MSG_LOW_DS_0(MM_SUB, "=EMM= EMC Attach Req EPS Mobile Identity - IMEI");
      emm_attach_request->eps_mobile_id.ue_id = IMEI_ID;
      emm_populate_imei(&emm_attach_request->eps_mobile_id.ue_id_info.imei, 
                         TRUE);
    }
#endif
  }
  else
  {
    switch(emm_ctrl_data_ptr->TIN)
    {
      case NAS_MM_TIN_P_TMSI:
        /* Do we need to check fo rvalidity of RAI also?*/
        if( mm_is_ptmsi_valid() == TRUE)
        {
          /* Map P-TMSI & RAI to GUTI*/
          mm_map_umts_identity_to_lte(&emm_attach_request->eps_mobile_id, 
                                      emm_attach_request->ptmsi_signature, 
                                      &emm_attach_request->ptmsi_signature_exists);     
                 /* Copy GUTI to additional GUTI*/
          if(emm_ctrl_data_ptr->emm_guti_valid == TRUE)
          {
            emm_attach_request->additional_guti = emm_ctrl_data_ptr->emm_guti;
            emm_attach_request->additional_guti_exists = TRUE;
          }     
          emm_attach_request->guti_type_exists = TRUE;
          emm_attach_request->guti_type = LTE_NAS_GUTI_TYPE_MAPPED;   
        }
        else if((emm_ctrl_data_ptr->plmn_service_state->sim_state ==  
                                                   SYS_SIM_STATE_AVAILABLE)
#ifdef FEATURE_LTE_REL9
               ||((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                   SYS_SIM_STATE_CS_INVALID) &&
                  (emm_ctrl_data_ptr->pdn_conn_req_ptr->req != ESM_EMERGENCY))
#endif
               )
        {
          MSG_LOW_DS_0(MM_SUB, "=EMM= Attach Req EPS Mobile Identity - IMSI");
          emm_populate_imsi(&emm_attach_request->eps_mobile_id, emm_ctrl_data_ptr);
        }
#ifdef FEATURE_LTE_REL9
        else if(emm_ctrl_data_ptr->pdn_conn_req_ptr->req == ESM_EMERGENCY)
        {
          MSG_LOW_DS_0(MM_SUB, "=EMM= EMC Attach Req EPS Mobile Identity - IMEI");
          emm_attach_request->eps_mobile_id.ue_id = IMEI_ID;
          emm_populate_imei(&emm_attach_request->eps_mobile_id.ue_id_info.imei, 
                             TRUE);
        }
#endif
        break;
      case NAS_MM_TIN_GUTI:
      case NAS_MM_TIN_RAT_RELATED_TMSI:
        if(emm_ctrl_data_ptr->emm_guti_valid == TRUE)
        {
          emm_populate_guti(&emm_attach_request->eps_mobile_id, emm_ctrl_data_ptr);
          emm_attach_request->guti_type_exists = TRUE;
          emm_attach_request->guti_type = LTE_NAS_GUTI_TYPE_NATIVE;
        }
        else if((emm_ctrl_data_ptr->plmn_service_state->sim_state ==  
                                                   SYS_SIM_STATE_AVAILABLE)
#ifdef FEATURE_LTE_REL9
               ||((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                   SYS_SIM_STATE_CS_INVALID) &&
                  (emm_ctrl_data_ptr->pdn_conn_req_ptr->req != ESM_EMERGENCY))
#endif
               )
        {
          MSG_LOW_DS_0(MM_SUB, "=EMM= Attach Req EPS Mobile Identity - IMSI");
          emm_populate_imsi(&emm_attach_request->eps_mobile_id, emm_ctrl_data_ptr);
        }
#ifdef FEATURE_LTE_REL9
      else if(emm_ctrl_data_ptr->pdn_conn_req_ptr->req == ESM_EMERGENCY)
      {
        MSG_LOW_DS_0(MM_SUB, "=EMM= EMC Attach Req EPS Mobile Identity - IMEI");
        emm_attach_request->eps_mobile_id.ue_id = IMEI_ID;
        emm_populate_imei(&emm_attach_request->eps_mobile_id.ue_id_info.imei, 
                           TRUE);
      }
#endif
        break;
      case NAS_MM_TIN_NONE:
      default:
        if(emm_ctrl_data_ptr->emm_guti_valid == TRUE)
        {
          emm_populate_guti(&emm_attach_request->eps_mobile_id, emm_ctrl_data_ptr);
          emm_attach_request->guti_type_exists = TRUE;
          emm_attach_request->guti_type = LTE_NAS_GUTI_TYPE_NATIVE;
        }
        else if( mm_is_ptmsi_valid() == TRUE)
        {
          mm_map_umts_identity_to_lte(&emm_attach_request->eps_mobile_id,
                                      emm_attach_request->ptmsi_signature,
                                      &emm_attach_request->ptmsi_signature_exists);

          emm_attach_request->guti_type_exists = TRUE;
          emm_attach_request->guti_type = LTE_NAS_GUTI_TYPE_MAPPED; 
        }
        else if((emm_ctrl_data_ptr->plmn_service_state->sim_state ==  
                                                   SYS_SIM_STATE_AVAILABLE)
#ifdef FEATURE_LTE_REL9
               ||((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                   SYS_SIM_STATE_CS_INVALID) &&
                  (emm_ctrl_data_ptr->pdn_conn_req_ptr->req != ESM_EMERGENCY))
#endif
               )
        {
          MSG_LOW_DS_0(MM_SUB, "=EMM= Attach Req EPS Mobile Identity - IMSI");
          emm_populate_imsi(&emm_attach_request->eps_mobile_id, emm_ctrl_data_ptr);
        }
#ifdef FEATURE_LTE_REL9
        else if(emm_ctrl_data_ptr->pdn_conn_req_ptr->req == ESM_EMERGENCY)
        {
          MSG_LOW_DS_0(MM_SUB, "=EMM= EMC Attach Req EPS Mobile Identity - IMEI");
          emm_attach_request->eps_mobile_id.ue_id = IMEI_ID;
          emm_populate_imei(&emm_attach_request->eps_mobile_id.ue_id_info.imei,
                             TRUE);
        }
#endif
        break;
    }
  }

  /* UE Network capability is MANDATORY */
  /* Set everything to 0 for now until the encryption is supported*/
  memset(&emm_attach_request->ue_capability, 
         0x0, 
         sizeof(emm_attach_request->ue_capability));
  
  emm_attach_request->ue_capability.ue_sec_capability = 
                                          emm_ctrl_data_ptr->ue_security_capability;

#ifdef FEATURE_LTE_REL9 
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
  {
    emm_attach_request->ue_capability.lte_positioning_prot_supported = emm_ctrl_data_ptr->lpp_supported;
    emm_attach_request->ue_capability.lcs_notification_supported = emm_ctrl_data_ptr->lcs_supported;
    emm_attach_request->ue_capability.notification_proc_supported = FALSE;
#ifdef FEATURE_SGLTE
    if(MM_IS_IN_SGLTE_MODE)
    {
      if(emm_ctrl_data_ptr->srvcc_over_sglte_support == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= UE support Notification procedure ");
        emm_attach_request->ue_capability.notification_proc_supported = TRUE;     
      }
    }
    else
#endif
    if(emm_is_srvcc_supported(emm_ctrl_data_ptr) == TRUE)
    {
       MSG_HIGH_DS_0(MM_SUB, "=EMM= UE support Notification procedure ");
       emm_attach_request->ue_capability.notification_proc_supported = TRUE;
    }
    else
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= UE does not support Notification procedure ");
    }
  }
#endif

  if(emm_is_1xsrvcc_supported(emm_ctrl_data_ptr) == TRUE )
  {
    emm_attach_request->ue_capability.onexsrvcc_capability_supported = TRUE;
  }
  else
  {
    emm_attach_request->ue_capability.onexsrvcc_capability_supported = FALSE;
  }

  emm_update_gw_sec_capabilities(emm_ctrl_data_ptr,
                                 &emm_attach_request->ue_capability);

  /* Fill in the cached PDN Connectivity Request message */
  emm_attach_request->esm_msg_container.pdn_connectivity_req = 
                                 *(emm_ctrl_data_ptr->pdn_conn_req_ptr);

  /*Last visited registered TAI*/
  if (emm_ctrl_data_ptr->last_registered_tai_valid == TRUE)
  {
    emm_attach_request->last_visited_reg_tai_exists = TRUE ;
    emm_attach_request->last_visited_reg_tai = 
                                        emm_ctrl_data_ptr->last_registered_tai;
  }
  else
  {
    emm_attach_request->last_visited_reg_tai_exists = FALSE ;
  }

  /* If Combined Attach is required EMM populates the following fields */
  if(emm_ctrl_data_ptr->eps_attach_type == COMBINED_EPS_IMSI_ATTACH)
  {
    if(mm_has_valid_lai() == TRUE)
    {
      /* Populate LAI out of preserved MM mobility info */
      emm_attach_request->locn_area_id_exists = TRUE ;
      memscpy((void*)&emm_attach_request->locn_area_id,
              sizeof(emm_attach_request->locn_area_id),
              (void*)&emm_ctrl_data_ptr->emm_location_area->lai[0],
              sizeof(emm_ctrl_data_ptr->emm_location_area->lai));

      /* Populate TMSI status out of preserved MM mobility info */
      if(FALSE == mm_is_tmsi_valid(emm_ctrl_data_ptr->emm_location_area->tmsi,TMSI_SIZE))
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= TMSI does not exist, including TMSI_STATUS in ATTACH_REQ");
        emm_attach_request->tmsi_status_exists  = TRUE;
        emm_attach_request->tmsi_status = FALSE; /*TMSI not available*/
      }
#ifdef FEATURE_LTE_REL11
       else if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
       {
        emm_attach_request->tmsi_based_nri = emm_get_tmsi_based_nri(emm_ctrl_data_ptr);
        if(emm_attach_request->tmsi_based_nri.length > 0)
        {
          emm_attach_request->tmsi_based_nri_exists = TRUE; 
        }
    }
#endif
    }
    else
    {
      /* Do not populate LAi, but populate TMSI*/
      emm_attach_request->locn_area_id_exists = FALSE;
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TMSI does not exist, including TMSI_STATUS in ATTACH_REQ");
      emm_attach_request->tmsi_status_exists  = TRUE;
      emm_attach_request->tmsi_status = FALSE;
    }

    /* Populate Classmark 2 IE */
    emm_read_coded_classmark_2(&emm_attach_request->classmark_2, // Classmark 2 IE to be encoded here
                               gsm_spt,
                               (boolean)(umts_spt|tds_spt));
    if(emm_attach_request->classmark_2.length > 0)
    {
      emm_attach_request->classmark_2_exists = TRUE;
    }       

    if(emm_ctrl_data_ptr->reg_req_pending_ptr->sms_only == TRUE)
    { 
      emm_attach_request->additional_update_exists = TRUE;
      emm_attach_request->additional_update = TRUE; 
    }
    else
    {
      emm_attach_request->additional_update_exists = FALSE;
    }
  }
  else /* EPS only or EMC attach */
  {
    /* Do not populate LAI, TMSI, MS network capability & Classmark 2 IEs*/
    emm_attach_request->locn_area_id_exists = FALSE ;
    emm_attach_request->tmsi_status_exists  = FALSE ;
    emm_attach_request->classmark_2_exists  = FALSE;
#ifdef FEATURE_SGLTE
    if(MM_IS_IN_SGLTE_MODE)
    {
      if(emm_ctrl_data_ptr->srvcc_over_sglte_support == TRUE)
      {
        emm_attach_request->classmark_2_exists = TRUE;
        /* Populate Classmark 2 IE */
        emm_read_coded_classmark_2(&emm_attach_request->classmark_2, // Classmark 2 IE to be encoded here
                                   gsm_spt,
                                   (boolean)(umts_spt|tds_spt));  
      }
    }
    else
#endif
    if (emm_is_srvcc_supported(emm_ctrl_data_ptr) == TRUE) 
    {
      emm_attach_request->classmark_2_exists = TRUE;
      /* Populate Classmark 2 IE */
      emm_read_coded_classmark_2(&emm_attach_request->classmark_2, // Classmark 2 IE to be encoded here
                                 gsm_spt,
                                 (boolean)(umts_spt|tds_spt));
     }
  }
  if((emm_disable_geran_cap == FALSE) &&
     (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                      SYS_RAT_UMTS_RADIO_ACCESS)|| 
      mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                      SYS_RAT_GSM_RADIO_ACCESS) || 
      mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                      SYS_RAT_TDS_RADIO_ACCESS)))
  {
    /* Populate MS network capability */
    emm_attach_request->ms_nw_capability = get_ms_network_capability();
    if(emm_attach_request->ms_nw_capability.length > 0)
    {
      emm_attach_request->ms_nw_capability_exists = TRUE;
    }
  }

  /* DRX parameter */
  if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_UMTS_RADIO_ACCESS)|| 
     mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_GSM_RADIO_ACCESS) ||
     mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_TDS_RADIO_ACCESS) ||
     mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_LTE_RADIO_ACCESS))
  {
    emm_attach_request->drx_param_exists = TRUE;
    emm_attach_request->drx_param = get_drx_parameter();
    MSG_HIGH_DS_2(MM_SUB, "=EMM= DRX param in Attach Request, Split Pg Cycle Code = 0x%x, Non DRX Tmr Code/CN_COEFF_S1 = 0x%x",
                  emm_attach_request->drx_param.value[0],
                  emm_attach_request->drx_param.value[1]);
  }

#ifdef FEATURE_LTE_REL9
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9 &&
     emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.is_voice_domain_ie_valid == TRUE)
  {
    emm_attach_request->voice_domain_pref.ue_usage_settings = 
                            emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.voice_or_data_centric;
    emm_attach_request->voice_domain_pref.voice_pref_for_utran = 
                            emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.voice_domain_pref;
    emm_attach_request->voice_domain_pref_exists =  
                            emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.is_voice_domain_ie_valid;
  }
#endif
                      
#ifdef FEATURE_SGLTE
  if(MM_IS_IN_SGLTE_MODE)
  {
    if(emm_ctrl_data_ptr->srvcc_over_sglte_support == TRUE)
    {
      /* Populate Classmark 3 IE */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Attach Request - SRVCC/GERAN supported, populate GS Classmark 3 IE ");
      emm_attach_request->classmark_3_exists = TRUE ;
      emm_read_coded_classmark_3(&emm_attach_request->classmark_3, // Classmark 3 IE to be encoded here
                                 gsm_spt,
                                 umts_spt);

      /*  supported_voice_codec info */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Attach Request supports codecs list ");
      emm_attach_request->supported_voice_codec_exists = TRUE;
      emm_attach_request->supported_voice_codec = gmm_get_supported_codec_list();
    }
  }
  else
#endif                                
  if (emm_is_srvcc_supported(emm_ctrl_data_ptr) == TRUE)
  {
    if((gsm_spt == TRUE) &&
       (emm_ctrl_data_ptr->l2g_srvcc_capability_support == TRUE))
    {
      /* Populate Classmark 3 IE */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Attach Request - SRVCC/GERAN supported, populate GS Classmark 3 IE ");
      emm_attach_request->classmark_3_exists = TRUE ;
      emm_read_coded_classmark_3(&emm_attach_request->classmark_3, // Classmark 3 IE to be encoded here
                                 gsm_spt,
                                 umts_spt);
    }
    
    /*  supported_voice_codec info */
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Attach Request supports codecs list ");
    emm_attach_request->supported_voice_codec_exists = TRUE;
    emm_attach_request->supported_voice_codec = gmm_get_supported_codec_list();
  }
#ifdef FEATURE_LTE_REL10
  emm_attach_request->ms_nw_feature_support_exists = FALSE;
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
  {
    emm_attach_request->ms_nw_feature_support_exists = TRUE;
    emm_attach_request->ext_periodic_timer_support = TRUE;

    /* Set low priority indicator. */
    emm_attach_request->low_priority_indicator = 
             emm_evaluate_low_priority_indicator_value(emm_ctrl_data_ptr, conn_est_cause);

    /* Overide the connection est cause to Delay tolerant if the low priority indicator is set to TRUE.
       For emergency attach low priority indicator will never be set to TRUE, so conn_est_cause 
       LTE_RRC_EST_CAUSE_EMERGENCY will never get overridden */
    if(emm_attach_request->low_priority_indicator == TRUE)
    {
      conn_est_cause = LTE_RRC_EST_CAUSE_DELAY_TOLERANT_ACCESS;
      emm_ctrl_data_ptr->delay_tolerant_access = TRUE;
      emm_ctrl_data_ptr->mo_data_enabled_dt = FALSE;
    }
  }
#endif
  /* Obtain sequence number */
  sequence_number = emm_get_nas_transaction_id() ;

   /* EMM shall stop T3402 if running */
   mm_stop_timer(TIMER_T3402);

   /* EMM shall stop T3411 if running */
   mm_stop_timer(TIMER_T3411);

   /* EMM shall stop T3346 */
   mm_stop_timer(TIMER_T3346);

#ifdef FEATURE_NAS_REL10
     gmm_set_gmm_is_congestion_due_to_rau(FALSE);
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
   mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

   MSG_HIGH_DS_0(MM_SUB, "=EMM= Start timer T3410");

   /* Start T3410 */
   mm_stop_timer(TIMER_T3410) ;
   mm_start_timer(TIMER_T3410, EMM_T3410_TIMER_DURATION);

   /* 33.401, CR 0457 
      When the ME is transitioning away from EMM-DEREGISTERED state with the intent to 
      eventually transitioning to EMM-REGISTERED state, 
      if the USIM supports EMM parameters storage, 
      the ME shall mark the stored EPS NAS security context on the USIM as invalid.  
   */
   MSG_HIGH_DS_0(MM_SUB, "=EMM= Start Attach procedure, invalidate the security context");
   emm_update_stored_security_context(emm_ctrl_data_ptr, TRUE);

   EMM_MOVES_TO_REGISTERED_INITIATED_STATE((emm_substate_type)EMM_WAITING_FOR_NW_RESPONSE) ;

  if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number, 
                                MSGR_NAS_EMM,
                                NULL, 
                                emm_attach_request_failed, 
                                emm_ctrl_data_ptr))
  {
    if (FALSE == emm_connection_setup(conn_est_cause, 
                                      emm_ctrl_data_ptr->out_msg_ptr, 
                                      sequence_number, 
                                      MSGR_NAS_EMM, 
                                      NULL, 
                                      emm_attach_request_failed, 
                                      ROUTING_INFO_GUMMEI,
                                      NULL,
                                      emm_ctrl_data_ptr))
    {
        MSG_ERROR_DS_1(MM_SUB, "=EMM= %d Attach attempt failed due to Access Class barring, T3410 has been stopped",
                       emm_ctrl_data_ptr->emm_attach_attempt_counter+1);

        mm_stop_timer(TIMER_T3410);

#ifdef FEATURE_LTE_REL9
        /*skip re-attach procedure for emergency attach failure and release pdp context connection req*/
        if(emm_ctrl_data_ptr->pdn_conn_req_ptr->req == ESM_EMERGENCY && 
           emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING)
        {
           MSG_HIGH_DS_0(MM_SUB, "=EMM= Aborting EMC Attach ");
           EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);
           emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
           emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;
           emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_LIMITED;
           emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);
           reg_service_report(MMR_CAUSE_FAILURE_MAX_ATTEMPTED, FALSE, emm_ctrl_data_ptr);
        }
        else
        {
           EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED) ;
        }
#else
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED) ;
#endif
        /*Not sure if we need to send LIMITED_SERVICE event to GSTK*/
        return;
    }
  }

#ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
  mm_save_rat_pri_list_sent_to_nw();
#endif

  /* Register connection release client for attach procedure.
     The client needs to be removed upon attach request sending failure, 
     attach accept/reject reception and T3410 expiry */
  emm_add_new_release_client(emm_attach_release_indication);

 /* send a SIM update to RRC to start using the latest NAS COUNT
     in KENb calculation, 
     Note: do not move this section of code, since the NAS count is 
     incremented when the message is sent.
    Moved the emm_send_sim_update() to emm_send_msg and emm_connection_setup
  */

} /* end of emm_start_attach_procedure() */

/*===========================================================================
FUNCTION    EMM_REGISTRATION_FAILURE_INVALIDATE_MOBILITY

DESCRIPTION
  This function processes Attach & TAU reject causes

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
static void emm_registration_failure_invalidate_mobility
(  
  lte_nas_emm_cause_type  emm_cause,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type	  current_lte_plmn;

  ASSERT(emm_ctrl_data_ptr != NULL);

  switch (emm_cause)
  {
    case LTE_NAS_ILLEGAL_UE:
    case LTE_NAS_ILLEGAL_ME:
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
      current_lte_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                                       emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
                                                                                 selected_plmn);
      MSG_HIGH_DS_4(MM_SUB, "=EMM= Is Accepted On LTE %d Eps Storage Supported : %d emm_plmn_block_on_rej_15_8 : %d, emm_plmn_block_on_rej_15_roaming : %d",
                    emm_ctrl_data_ptr->is_accepted_on_lte,
                    emm_ctrl_data_ptr->eps_storage_supported,
                    emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8,
                    emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming);
      if((emm_cause==LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED) &&
         (emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE) &&
         (emm_ctrl_data_ptr->is_accepted_on_lte==FALSE) &&
         (emm_ctrl_data_ptr->eps_storage_supported == FALSE) &&
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
          (reg_sim_per_subs_is_ehplmn(current_lte_plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
          (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), current_lte_plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
          (reg_sim_is_ehplmn(current_lte_plmn) )
#else
          (sys_plmn_match(reg_sim_read_hplmn(), current_lte_plmn))
#endif
#endif
       )
       {
          emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED,
                                           SYS_SIM_STATE_AVAILABLE,
                                           emm_ctrl_data_ptr, TRUE) ;
       }
       else
       {
         /* Delete Equivalent PLMN List */
         emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                   SYS_SIM_STATE_CS_PS_INVALID, 
                                   emm_ctrl_data_ptr, TRUE) ;
       }
      break ;
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:

      /* Delete Equivalent PLMN List */
      emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                   SYS_SIM_STATE_PS_INVALID, 
                                   emm_ctrl_data_ptr, TRUE) ;
      break ;
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
      /* Preserve Equivalent PLMN List */
      emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                   SYS_SIM_STATE_AVAILABLE, 
                                   emm_ctrl_data_ptr, FALSE) ;
      break ;
#ifdef FEATURE_FEMTO_CSG
    case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
      /* Update EMM USIM with EPS update status of ROAMING_NOT_ALLOWED */
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_ROAMING_NOT_ALLOWED;
      emm_save_mobility_info(emm_ctrl_data_ptr);
      break;
#endif
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:

      /* Delete Equivalent PLMN List */
      emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                   SYS_SIM_STATE_AVAILABLE, 
                                   emm_ctrl_data_ptr, TRUE) ;
      break ;
#ifdef FEATURE_LTE_REL10
    case LTE_NAS_CONGESTION:
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
      {
        emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED ;
      }
      break;
#endif
     case LTE_NAS_SEVERE_NW_FAILURE:
       /*#42	(Severe network failure);
       The UE shall set the EPS update status to EU2 NOT UPDATED, 
       and shall delete any GUTI, last visited registered TAI, eKSI, 
       and list of equivalent PLMNs*/
       emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED,
                                   SYS_SIM_STATE_AVAILABLE,
                                   emm_ctrl_data_ptr, TRUE);
	   break;
    default:
      /* Do nothing here for other emm causes */
      break;
  } /* end of cause specific switch() */
}

static void emm_registration_failure_change_state(lte_nas_emm_cause_type emm_cause)
{
  emm_ctrl_data_type        *emm_ctrl_data_ptr = NULL;
  
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  /* EMM state change */
  switch (emm_cause)
  {
    case LTE_NAS_ILLEGAL_UE:
    case LTE_NAS_ILLEGAL_ME:
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
    case LTE_NAS_SEVERE_NW_FAILURE:
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH) ;
      emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);
      break ;
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
    case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
      emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);
      break;
#ifdef FEATURE_LTE_REL9
    case LTE_NAS_IMEI_NOT_ACCEPTED:
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_IMSI);
      emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);
      break;
#endif
#ifdef FEATURE_LTE_REL10
    case LTE_NAS_CONGESTION:
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
      {
         EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH);
      }
     break;
#endif   
    default:
      MSG_HIGH_DS_1(MM_SUB, "=EMM= Ignoring abnormal EMM cause %u", emm_cause);
      break ;
  }
}

/*===========================================================================

FUNCTION  EMM_PROCESS_ATTACH_FAILURE_MULTIMODE

DESCRIPTION
  This function updates EMM parameters upon GMM ATTACH REJECT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_attach_failure_multimode
(  
  lte_nas_emm_cause_type  emm_cause,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  /* Update update status, GUTI, last registered TAI, TAI list, KSI */
  emm_registration_failure_invalidate_mobility(emm_cause,emm_ctrl_data_ptr);

  /* Attach attempt counter */
  switch (emm_cause)
  {
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
#ifdef FEATURE_NAS_REL11
    case LTE_NAS_CONGESTION:
#endif
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Resetting attach attempt counter - Multimode");
      emm_ctrl_data_ptr->emm_attach_attempt_counter = 0 ;
      break;
    default:
      break;
  }

  /* Update EMM state */
/*24.008: 4.7.3.1.4,4.7.3.2.4
If the attach request is rejected with cause#22
If S1 mode is supported in the MS, the MS shall handle the EMM parameters EMM state, EPS update status, and attach attempt counter 
as specified in 3GPP TS 24.301 [120] for the case when the attach procedure is rejected with the EMM cause with the same value.*/
#ifdef FEATURE_NAS_REL11
  if(emm_cause == LTE_NAS_CONGESTION)
  {
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH);
  }
  else
#endif
  {
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
  }
}


/*===========================================================================
FUNCTION  EMM_CLEAN_UP_PENDING_ESR

DESCRIPTION
  This function cleans up pending ESR due to 3GPP CSFB

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
static void emm_cleanup_pending_ESR(emm_ctrl_data_type* emm_ctrl_data_ptr)
{
  emm_csfb_failure_cause_type failure_cause_type;
  failure_cause_type.failure_type = INVALID_REJECT_CAUSE;

  ASSERT(emm_ctrl_data_ptr != NULL); 

  if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED)
  {
    failure_cause_type.failure_type = OTA_REJECT_CAUSE;
    emm_process_extended_service_req_failure(failure_cause_type,emm_ctrl_data_ptr);
    emm_remove_release_client(emm_service_request_release_indication);
  }

} /* emm_cleanup_pending_ESR */

/*===========================================================================
FUNCTION    EMM_PROCESS_CS_REJECT_CAUSE

DESCRIPTION
  This function processes EMM cause for EPS only successful Attach Accept

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
void emm_process_cs_reject_cause
(
  lte_nas_emm_cause_type emm_cause,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{

  ASSERT(emm_ctrl_data_ptr != NULL); 

  emm_print_emm_cause(emm_cause);

  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                        SYS_SRV_DOMAIN_PS_ONLY;
  emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_PS_ONLY;

  switch (emm_cause)
  {
    case LTE_NAS_IMSI_UNKNOWN_IN_HSS:
      /* ----------------------------------------------------
      ** The UE shall set the update status to U3 ROAMING NOT
      ** ALLOWED and shall delete any TMSI, LAI and ciphering
      ** key sequence number. The USIM shall be considered as
      ** invalid for non-EPS services until switching off or
      ** the USIM is removed.
      ** ---------------------------------------------------- */
      mm_update_cs_reject_invalid_imsi_status();

      emm_ctrl_data_ptr->plmn_service_state->plmn_service_capability
                                          = SYS_SRV_CAPABILITY_PS_SERVICE_ONLY;

      emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;

      /* Add serving PLMN to forbidden PLMN list for combined registration */
      emm_add_combined_reg_forbidden_plmn(emm_ctrl_data_ptr);
      mm_send_mmr_reg_reject_ind(
                 SYS_SRV_DOMAIN_CS_ONLY, 
                 (byte)emm_cause,
                 emm_ctrl_data_ptr->plmn_service_state->plmn,
                 emm_ctrl_data_ptr->plmn_service_state->tac
  #ifdef FEATURE_FEMTO_CSG
                 ,SYS_CSG_ID_INVALID
  #endif
                ,emm_ctrl_data_ptr->plmn_service_state->active_rat
        );
      break ;
    case LTE_NAS_MSC_TEMPORARILY_NOT_REACHABLE:
    case LTE_NAS_NW_FAILURE:
    case LTE_NAS_CONGESTION:
#ifdef FEATURE_LTE_REL11
      if((emm_cause == LTE_NAS_CONGESTION)&&
        (emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11))
      {
        /*rev. of C1-122781/C1-123216 #22	(Congestion)
        The tracking area updating attempt counter shall be set to 5.
        */
        emm_ctrl_data_ptr->emm_tau_attempt_counter = MAX_NUMBER_TAU_OF_ATTEMPTS;
      }
#endif
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM);
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      emm_start_tau_recovery_procedure(emm_ctrl_data_ptr);
#ifdef FEATURE_LTE_REL11
      if((emm_cause == LTE_NAS_CONGESTION)&&
        (emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11))
      {
       emm_ctrl_data_ptr->plmn_service_state->cs_service_status = 
                                                        SYS_SRV_STATUS_LIMITED_REGIONAL;
      }
#endif
      /* TAU will be recovery type. Need to set ongoing EPS TA update type to 
         IMSI attach in case of TAU recovery later. */
      emm_ctrl_data_ptr->emm_ongoing_eps_ta_update = 
                                           COMBINED_TA_LA_UPDATING_IMSI_ATTACH;

      mm_send_mmr_reg_reject_ind(
                 SYS_SRV_DOMAIN_CS_ONLY, 
                 (byte)emm_cause,
                 emm_ctrl_data_ptr->plmn_service_state->plmn,
                 emm_ctrl_data_ptr->plmn_service_state->tac
  #ifdef FEATURE_FEMTO_CSG
                 ,SYS_CSG_ID_INVALID
  #endif
                ,emm_ctrl_data_ptr->plmn_service_state->active_rat
        );
      break ;
    case LTE_NAS_CS_DOMAIN_NOT_AVAILABLE:
      mm_invalidate_lu_info(NOT_UPDATED) ;

      emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;

      /* Prevent any other attempts to trigger Combined TAU */
      emm_ctrl_data_ptr->plmn_service_state->plmn_service_capability = 
                                            SYS_SRV_CAPABILITY_PS_SERVICE_ONLY;

      /* Add serving PLMN to forbidden PLMN list for combined registration */
      emm_add_combined_reg_forbidden_plmn(emm_ctrl_data_ptr);

      /* Reset SG assoc flag since combined registration is no longer allowed */
      if(emm_ctrl_data_ptr->sg_association_to_be_established != NAS_EMM_SG_ASSOC_NOT_REQ)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Reset SGs assoc flag - CS not available");
        emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_NOT_REQ; 
      }

      emm_ctrl_data_ptr->plmn_service_state->cs_service_status = 
                                                         SYS_SRV_STATUS_NO_SRV;

      /*For cause #18 send LTE CS capability as barred so that CM can disable LTE indefinitely. */

      emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability = SYS_LTE_CS_CAPABILITY_BARRED;
      mm_send_mmr_reg_reject_ind(
                     SYS_SRV_DOMAIN_CS_ONLY, 
                     (byte)emm_cause,
                     emm_ctrl_data_ptr->plmn_service_state->plmn,
                     emm_ctrl_data_ptr->plmn_service_state->tac
#ifdef FEATURE_FEMTO_CSG
                     ,SYS_CSG_ID_INVALID
#endif 
                     ,emm_ctrl_data_ptr->plmn_service_state->active_rat
                   );  
      break ;


    default:
      /*24.301 REL 11.5.0 section 5.5.1.3.6 - Abnormal CS failure is considered as PS registered 
        and TAU is triggered for CS registration*/
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM);
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      /* Start T3411 */
      emm_start_tau_recovery_procedure(emm_ctrl_data_ptr);
      /* TAU will be recovery type. Need to set ongoing EPS TA update type to 
         IMSI attach in case of TAU recovery later. */
      emm_ctrl_data_ptr->emm_ongoing_eps_ta_update = 
                                           COMBINED_TA_LA_UPDATING_IMSI_ATTACH;
      break ;
  }

  emm_cleanup_pending_ESR(emm_ctrl_data_ptr);

} /* end of emm_process_cs_reject_cause() */

/*===========================================================================
FUNCTION    EMM_PROCESS_ATTACH_REJECT_CAUSE

DESCRIPTION
  This function processes attach reject cause

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
void emm_process_attach_reject_cause
(
  lte_nas_emm_attach_rej_type *attach_reject,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
)
{
  mmr_cause_e_type    reg_cause = MMR_CAUSE_NONE ;
  sys_plmn_id_s_type  current_lte_plmn;
  emm_failure_type    emm_failure_cause;
  uint32              timer_value;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  /*initialise it*/
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  /* Always update GMM/MM before EMM processing to allow GMM/MM writes sim */
  if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_UMTS_RADIO_ACCESS)|| 
     mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_GSM_RADIO_ACCESS) ||
     mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_TDS_RADIO_ACCESS))
  {
    mm_update_attach_reject_status((byte)attach_reject->emm_cause);
  }

  switch (attach_reject->emm_cause)
  {
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
	case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA :
	  MSG_HIGH_DS_4(MM_SUB, "=EMM= Is Accepted On LTE %d Eps Storage Supported : %d emm_plmn_block_on_rej_15_8 : %d, emm_plmn_block_on_rej_15_roaming : %d",
                        emm_ctrl_data_ptr->is_accepted_on_lte,
                        emm_ctrl_data_ptr->eps_storage_supported,
                        emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8,
                        emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming);
	  break;
	default:
	  break;
  }
  switch (attach_reject->emm_cause)
  {
    case LTE_NAS_ILLEGAL_UE:
    case LTE_NAS_ILLEGAL_ME:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
    case LTE_NAS_ESM_FAILURE:
    case LTE_NAS_SEMANTICALLY_INCORRECT_MSG:
    case LTE_NAS_INVALID_MANDATORY_INFO:
    case LTE_NAS_NON_EXISTENT_MSG_TYPE:
    case LTE_NAS_NON_EXISTANT_INFO:
    case LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
    case LTE_NAS_SEVERE_NW_FAILURE:
     emm_print_emm_cause(attach_reject->emm_cause);
      break ;
#ifdef FEATURE_LTE_REL10
    case LTE_NAS_CONGESTION:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= EMM Cause - LTE_NAS_CONGESTION t3346 %d",
                     attach_reject->t3346_val);
     /* For Rel10 if t3346 value is 0 or if it is release this is abnormal*/
      if(((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
               (attach_reject->t3346_val == 0)) ||
               (emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL10))
      {
        /* Abnormal behavior */
        emm_send_reg_rej_ind(attach_reject->emm_cause, emm_ctrl_data_ptr);
        emm_start_attach_recovery(emm_ctrl_data_ptr);
        return;
      }
      break ;
#endif
#ifdef FEATURE_FEMTO_CSG
    case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= EMM Cause - LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG");
      if(emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id == SYS_CSG_ID_INVALID)
      {
        /* Abnormal behavior */
        emm_send_reg_rej_ind(attach_reject->emm_cause, emm_ctrl_data_ptr);
        emm_start_attach_recovery(emm_ctrl_data_ptr);
        return;
      }
      break;
#endif 
    default:
#ifdef FEATURE_LTE_REL9
      if((attach_reject->emm_cause == LTE_NAS_IMEI_NOT_ACCEPTED) &&
         (emm_ctrl_data_ptr->eps_attach_type == EPS_EMERGENCY_ATTACH) &&
         (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                  SYS_SIM_STATE_NOT_AVAILABLE))
      {
        MSG_ERROR_DS_0(MM_SUB, "=EMM= EMM Cause - IMEI_NOT_ACCEPTED");
        break;
      }
      else
      {
#endif
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Attach rejected with cause %u", attach_reject->emm_cause);
        emm_send_reg_rej_ind(attach_reject->emm_cause, emm_ctrl_data_ptr);
        emm_start_attach_recovery(emm_ctrl_data_ptr);
        return;
#ifdef FEATURE_LTE_REL9
      }
#endif
  }

  if(attach_reject->emm_cause != LTE_NAS_ESM_FAILURE)
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= reset rej_esm_fail_counter when it is %d",
                  emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter);
    emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter = 0;
  }

  switch (attach_reject->emm_cause)
  {
    case LTE_NAS_ESM_FAILURE:
      /* if attach_rej with cause #19,we need to attemp the attach procedure 
         atleast MAX_NUMBER_ESM_FAILURE_ATTEMPTS times before starting T3402 */

      emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter++;

      if(!(emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter < 
                                              MAX_NUMBER_ESM_FAILURE_ATTEMPTS))
      {
        emm_ctrl_data_ptr->emm_attach_attempt_counter = MAX_NUMBER_ATTACH_OF_ATTEMPTS ;  
        MSG_HIGH_DS_1(MM_SUB, "=EMM= rej_esm_fail_counter %d",
                      emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter);

      }

      /* Discard currently cached PDN_CONNECTIVITY_REQ since DS would give new PDN REQ */
      emm_ctrl_data_ptr->pdn_conn_req_trans_id =
                                             EMM_INVALID_PDN_CONN_REQ_TRANS_ID;

      memset((void *)(emm_ctrl_data_ptr->pdn_conn_req_ptr), 0x0, 
                  sizeof(lte_nas_esm_pdn_connectivity_req) ); 

      emm_start_attach_recovery(emm_ctrl_data_ptr) ;
      break ;
    case LTE_NAS_SEMANTICALLY_INCORRECT_MSG:
    case LTE_NAS_INVALID_MANDATORY_INFO:
    case LTE_NAS_NON_EXISTENT_MSG_TYPE:
    case LTE_NAS_NON_EXISTANT_INFO:
    case LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED:
#ifdef FEATURE_LTE_REL9 
      if((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_PENDING) &&
         (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == FALSE))
      {
#endif
        /* Finish UP Attach Recovery */
        emm_ctrl_data_ptr->emm_attach_attempt_counter = MAX_NUMBER_ATTACH_OF_ATTEMPTS ;
#ifdef FEATURE_LTE_REL9 
      }
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Not updating attempt counter - EMC SRV pending");
      }
#endif
      emm_start_attach_recovery(emm_ctrl_data_ptr) ;
      break ;
    default:
      /* Timer T3440 and T3346 */
      switch (attach_reject->emm_cause)
      {
        case LTE_NAS_PLMN_NOT_ALLOWED:
        case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
        case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
        case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
          mm_start_timer(TIMER_T3440,EMM_T3440_TIMER_DURATION);
          break ;
#ifdef FEATURE_LTE_REL10
       case LTE_NAS_CONGESTION:
         if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) 
         {
           mm_stop_timer(TIMER_T3346);
           mm_start_timer(TIMER_T3346,attach_reject->t3346_val);
           MSG_HIGH_DS_1(MM_SUB, "=EMM= T3346 timer started in Attach %d",
                         attach_reject->t3346_val);
/*Set LU initiated flag to TRUE to start LU in G in case of interesystem change.
24.008: 4.4.1
The normal location updating procedure shall also be started when the MS, configured to use CS fallback and SMS over SGs, 
or SMS over SGs only, enters a GERAN or UTRAN cell after intersystem change from S1 mode to Iu or A/Gb mode, if timer T3346 is running, 
and the location area of the current cell is the same as the stored location area.*/
           mm_is_lu_to_be_initiated_t3346_ltogw = TRUE;
           /*saves congestion plmn associated with the timer*/
           emm_save_t3346_plmn(emm_ctrl_data_ptr);
         }
         break;
#endif
         case LTE_NAS_SEVERE_NW_FAILURE:
           /*24.301 UE shall start an implementation specific timer, setting 
           its value to 2 times the value of T as defined in 3GPP TS23.122[6].
           While this timer is running, the UE shall not consider the PLMN+RAT
           combination that provided this reject cause, a candidate for PLMN selection.*/
           timer_value = reg_timers_get_hplmn_search_timer();
           emm_add_plmn_to_backoff_fplmn_list(emm_ctrl_data_ptr->plmn_service_state->plmn,
                                       timer_value*2*60000,SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE);
		   break;
        default:
          /* Do nothing here for other emm causes */
          break;
      }

      /* Attach attempt counter */
      switch (attach_reject->emm_cause)
      {
        case LTE_NAS_PLMN_NOT_ALLOWED:
        case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
        case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
        case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
#ifdef FEATURE_LTE_REL10
        case LTE_NAS_CONGESTION:
#endif
#ifdef FEATURE_FEMTO_CSG
        case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
#endif         
          /* Reset Attach Attempt Counter */
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Reset Attach attempt counter in attach procedure");
          emm_ctrl_data_ptr->emm_attach_attempt_counter = 0 ;
          break ;
        case LTE_NAS_SEVERE_NW_FAILURE:
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Reset Attach attempt counter in attach procedure to Max Number of Attach Attempts %d",
                       MAX_NUMBER_ATTACH_OF_ATTEMPTS);
          emm_ctrl_data_ptr->emm_attach_attempt_counter = MAX_NUMBER_ATTACH_OF_ATTEMPTS;
          break;
        default:
          /* Do nothing here for other emm causes */
          break;
      } /* reset attach attempt counter switch() */

      /* Delete the list of equivalent PLMNs */ 
      switch (attach_reject->emm_cause)
      {
        case LTE_NAS_ILLEGAL_UE:
        case LTE_NAS_ILLEGAL_ME:
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
        case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
        case LTE_NAS_PLMN_NOT_ALLOWED:
        case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Deleting equivalent PLMNs list - ATTACH REJ");
          emm_ctrl_data_ptr->update_eplmn_list = TRUE;
          emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
          break;
#ifdef FEATURE_LTE_REL11
         case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
            if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
            {
              MSG_HIGH_DS_0(MM_SUB, "=EMM= Deleting equivalent PLMNs list - ATTACH REJ");
              emm_ctrl_data_ptr->update_eplmn_list = TRUE;
              emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
            }
            break;
#endif
        default:
          /* Do nothing here for other emm causes */
          break;
      }

      /* Add current PLMN into forbidden PLMN list */ 
      switch (attach_reject->emm_cause)
      {
        case LTE_NAS_PLMN_NOT_ALLOWED:
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
          /* Create current PLMN ID based on latest RRC reported PLMN ID */
          current_lte_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                      emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
                                                                selected_plmn);
          if(attach_reject->emm_cause == LTE_NAS_PLMN_NOT_ALLOWED)
          {
            /* Add current PLMN into forbidden PLMN list */
            mm_add_plmn_to_fplmn_list(current_lte_plmn);
          }
          else
          {
            /* Add current PLMN into forbidden PLMNs for GPRS list */
            mm_add_plmn_to_gprs_fplmn_list(current_lte_plmn);
          }
          break;
        case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:		  
        case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
                 /* Create current PLMN ID based on latest RRC reported PLMN ID */
          current_lte_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                      emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
                                                                selected_plmn);

		  if(
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
              (reg_sim_per_subs_is_ehplmn(current_lte_plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
              (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), current_lte_plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
              (reg_sim_is_ehplmn(current_lte_plmn) )
#else
              (sys_plmn_match(reg_sim_read_hplmn(), current_lte_plmn))
#endif
#endif
              )
           {
			if((emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE) &&
			   (emm_ctrl_data_ptr->is_accepted_on_lte==FALSE) &&
			   (emm_ctrl_data_ptr->eps_storage_supported == FALSE))
		    {
                emm_add_plmn_to_backoff_fplmn_list(current_lte_plmn,mm_extend_lte_disable_duration,SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE);  
              }
           }
           else
		  {	  
              if((attach_reject->emm_cause == LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA) &&
		 (emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming == TRUE) &&
		 (!sys_plmn_match(current_lte_plmn,emm_ctrl_data_ptr->emm_lte_accepted_vplmn)) &&
                 (emm_ctrl_data_ptr->eps_storage_supported == FALSE))
             {
               emm_add_plmn_to_backoff_fplmn_list(current_lte_plmn,mm_extend_lte_disable_duration,SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE);  
             }
           }

          break;
        default:
          /* Do nothing here for other emm causes */
          break;
      }

      /* Add current TAI to forbidden TAI lists */
      switch (attach_reject->emm_cause)
      {
        case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Add TAI to list of forbidden TAIs for service");

          /* Add to frobidden tracking area list */
          emm_add_forbidden_tai(emm_ctrl_data_ptr->forbidden_for_service_list_ptr,
                                emm_get_camped_tai(emm_ctrl_data_ptr),
                                emm_ctrl_data_ptr) ;
          break ;
        case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
        case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
          current_lte_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                                          emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
                                                                                       selected_plmn);
          if(!((attach_reject->emm_cause==LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA) &&

	            (((emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE) &&
                   (emm_ctrl_data_ptr->is_accepted_on_lte==FALSE) &&
               (emm_ctrl_data_ptr->eps_storage_supported == FALSE) &&
#ifdef FEATURE_DUAL_SIM
 #ifdef FEATURE_EQUIVALENT_HPLMN
                (reg_sim_per_subs_is_ehplmn(current_lte_plmn,(sys_modem_as_id_e_type)mm_sub_id) )
 #else
                (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), current_lte_plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
                (reg_sim_is_ehplmn(current_lte_plmn) )
#else
                (sys_plmn_match(reg_sim_read_hplmn(), current_lte_plmn))
#endif
 #endif
                 ) ||  
                ((emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming == TRUE) &&
                 (!sys_plmn_match(current_lte_plmn,emm_ctrl_data_ptr->emm_lte_accepted_vplmn)) &&
                   (emm_ctrl_data_ptr->eps_storage_supported == FALSE) &&
#ifdef FEATURE_DUAL_SIM
 #ifdef FEATURE_EQUIVALENT_HPLMN
                   !(reg_sim_per_subs_is_ehplmn(current_lte_plmn,(sys_modem_as_id_e_type)mm_sub_id) )
 #else
                   !(sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), current_lte_plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
                   !(reg_sim_is_ehplmn(current_lte_plmn) )
#else
                   !(sys_plmn_match(reg_sim_read_hplmn(), current_lte_plmn))
#endif
 #endif
                  )
                 )))
            {
              MSG_HIGH_DS_0(MM_SUB, "=EMM= Add TAI to FTAI list for roaming - Attach Rej");
              emm_add_forbidden_tai(emm_ctrl_data_ptr->rrc_forbidden_list_ptr,
                                    emm_get_camped_tai(emm_ctrl_data_ptr),
                                 emm_ctrl_data_ptr);
          emm_add_manual_ftai(emm_ctrl_data_ptr);
            }
          break;
        default:
          /* Do nothing here for other emm causes */
          break;
      } /* forbidden area list switch() */

#ifdef FEATURE_FEMTO_CSG
      /* Remove current CSG from white list*/
      switch(attach_reject->emm_cause)
      {
        case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
          mm_delete_csg_from_white_list(emm_ctrl_data_ptr->plmn_service_state->plmn,
                                                                    &emm_ctrl_data_ptr->plmn_service_state->csg_info);
          break;
        default:
          /*Do nothing here for other emm cause*/
          break;
      }
#endif 
      emm_registration_failure_invalidate_mobility(attach_reject->emm_cause,
                                                   emm_ctrl_data_ptr);

      emm_registration_failure_change_state(attach_reject->emm_cause);

      emm_send_reg_rej_ind(attach_reject->emm_cause, emm_ctrl_data_ptr);

      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;

      /* should we handle for CS_PS */
      if((attach_reject->emm_cause == LTE_NAS_EPS_SERVICES_NOT_ALLOWED ||
          attach_reject->emm_cause == LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN)  &&
         (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                         SYS_SRV_DOMAIN_CS_ONLY   ||
          emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                          SYS_SRV_DOMAIN_CS_PS))
      {
        /* spec 24.301 5.5.1.3.5 v9.2.0 A UE in CS/PS mode 1 or mode2
           of operation is still IMSI attached for non-EPS services */
       
        /* UE CS only on GW, comes to L and starts combined attach. */
        emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                      SYS_SRV_DOMAIN_CS_ONLY;
        emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_CS_ONLY;
#ifdef FEATURE_LTE_REL11
       /*spec 24.301 5.5.1.3.5 A UE, which is already IMSI attached for non-EPS services, 
         is still IMSI attached for non-EPS services
         and shall set the update status to U2 NOT UPDATED.*/
        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
        {
          mm_location_information.location_update_status = NOT_UPDATED;
        }
#endif
      }
      else 
      {
        emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_NO_SRV;
        emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;
      }


     
      switch (attach_reject->emm_cause)
      {
        case LTE_NAS_ILLEGAL_UE:
        case LTE_NAS_ILLEGAL_ME:
        case LTE_NAS_IMEI_NOT_ACCEPTED:
          reg_cause = MMR_CAUSE_NOT_SPECIFIED ;
          if(attach_reject->emm_cause == LTE_NAS_ILLEGAL_UE ||
               attach_reject->emm_cause == LTE_NAS_ILLEGAL_ME)
          {
            emm_ctrl_data_ptr->manual_user_sel = FALSE;
            emm_set_mm_manual_selection_flag(FALSE);
          }
          break ;
        case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
#ifdef FEATURE_FEMTO_CSG
        case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
#endif
          current_lte_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                                    emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
                                                                             selected_plmn);
          if((attach_reject->emm_cause==LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA) &&

             (((emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE) &&
               (emm_ctrl_data_ptr->is_accepted_on_lte==FALSE) &&
             (emm_ctrl_data_ptr->eps_storage_supported == FALSE) &&
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
              (reg_sim_per_subs_is_ehplmn(current_lte_plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
              (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), current_lte_plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
              (reg_sim_is_ehplmn(current_lte_plmn) )
#else
              (sys_plmn_match(reg_sim_read_hplmn(), current_lte_plmn))
#endif
#endif
              ) ||  
              ((emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming == TRUE) &&
               (!(sys_plmn_match(current_lte_plmn,emm_ctrl_data_ptr->emm_lte_accepted_vplmn))) &&
               (emm_ctrl_data_ptr->eps_storage_supported == FALSE) &&
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
               !(reg_sim_per_subs_is_ehplmn(current_lte_plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
               !(sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), current_lte_plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
               !(reg_sim_is_ehplmn(current_lte_plmn) )
#else
               !(sys_plmn_match(reg_sim_read_hplmn(), current_lte_plmn))
#endif
#endif
              )
             )
            )
          {
            reg_cause = MMR_CAUSE_ABORT_PLMN_SELECTION;
          }
          else
          {
          reg_cause = MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED ;
          }
          break ;
        case LTE_NAS_SEVERE_NW_FAILURE:
          reg_cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED ;
          break ;
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
        case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
          current_lte_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                                        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
                                                                                 selected_plmn);
         if((attach_reject->emm_cause==LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED) &&
            (emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE) &&
            (emm_ctrl_data_ptr->is_accepted_on_lte==FALSE) &&
            (emm_ctrl_data_ptr->eps_storage_supported == FALSE) &&
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
            (reg_sim_per_subs_is_ehplmn(current_lte_plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
            (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), current_lte_plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
            (reg_sim_is_ehplmn(current_lte_plmn) )
#else
            (sys_plmn_match(reg_sim_read_hplmn(), current_lte_plmn))
#endif
#endif
           )
          {
            reg_cause = MMR_CAUSE_ABORT_PLMN_SELECTION;
            emm_ctrl_data_ptr->manual_user_sel = FALSE; 
            emm_set_mm_manual_selection_flag(FALSE);    
          }
          else
          {
            emm_ctrl_data_ptr->plmn_service_state->plmn_forbidden = TRUE ;
            reg_cause = MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES ;
            emm_ctrl_data_ptr->manual_user_sel = FALSE;
            if(attach_reject->emm_cause == LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN ||
               attach_reject->emm_cause == LTE_NAS_EPS_SERVICES_NOT_ALLOWED)
            { 
              emm_set_gmm_manual_selection_flag(FALSE);
            }
            else
            {
              emm_set_mm_manual_selection_flag(FALSE);
            }
          }
          break ;
        case LTE_NAS_PLMN_NOT_ALLOWED: 
          reg_cause = MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED ;
          emm_ctrl_data_ptr->plmn_service_state->plmn_forbidden = TRUE ;
          emm_set_mm_manual_selection_flag(FALSE);
          emm_ctrl_data_ptr->manual_user_sel = FALSE;
          break ;
        case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
          reg_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN ;
          emm_ctrl_data_ptr->manual_user_sel = FALSE;
          emm_set_mm_manual_selection_flag(FALSE);
          break ;
        case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
          reg_cause = MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION ;
          emm_set_mm_manual_selection_flag(FALSE);
          emm_ctrl_data_ptr->manual_user_sel = FALSE;
        break ;
#ifdef FEATURE_LTE_REL10
         case LTE_NAS_CONGESTION:          
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                SYS_SRV_STATUS_LIMITED;
          reg_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;           
          break;
#endif
        default:
          MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM cause",0,0,0) ;
          break ;
      } /* translation switch() */


#ifdef FEATURE_LTE_REL9
      if(emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr)== TRUE)
      {

        if(attach_reject->emm_cause == LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA
           ||
           attach_reject->emm_cause == LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA)
        {
          /* UE can not camp on cell which belongs to FTAI for roaming in automatic mode.
             Overriding SIB1 E911 support to FALSE to have CM swithes UE back to limited mode */
          emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
                                                 ims_emergency_support_enabled = FALSE;
        }        
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE);
        reg_cause = MMR_CAUSE_ABORT_PLMN_SELECTION;
      }
#endif

      reg_service_report(reg_cause, FALSE, emm_ctrl_data_ptr);
      emm_send_reg_attach_failed_ind();

      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      
      /* Notify ESM that attach procedure is rejected */
      emm_build_and_send_failure_ind(EMM_ATTACH_FAILED, 
                                     attach_reject->emm_cause,
                                     emm_ctrl_data_ptr->pdn_conn_req_trans_id,
                                     emm_failure_cause);
      /* ESM will release all resources allocated for currently cached  
         PDN_CONNECTIVITY_REQ upon receipt of failure ind. 
         Discard currently cached PDN_CONNECTIVITY_REQ since it is no longer 
         reusable */
      emm_ctrl_data_ptr->pdn_conn_req_trans_id = 
                                             EMM_INVALID_PDN_CONN_REQ_TRANS_ID;
      break ;
  }

} /* end of emm_process_attach_reject_cause() */

/*===========================================================================
FUNCTION    EMM_PROCESS_ATTACH_REJECT

DESCRIPTION
  This function processes attach reject message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_attach_reject
(
  lte_nas_emm_attach_rej_type *attach_reject,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
#ifdef FEATURE_LTE_REL10
  dword prev_t3402_value;
#endif

  ASSERT(attach_reject != (lte_nas_emm_attach_rej_type *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Received Attach Reject message");
  emm_send_diag_event(EVENT_NAS_ATTACH, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_REJECTED, (lte_nas_emm_cause_type)attach_reject->emm_cause);

  if(EMM_GET_SUBSTATE() == EMM_WAITING_FOR_ESM_RESPONSE)
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Rcevd Attach Rej in EMM Substate %d. Clean up bearer contexts",
                         EMM_GET_SUBSTATE());
    emm_build_and_send_detach_ind(EMM_DETACHED);
  }

  emm_cancel_authentication();

  MSG_HIGH_DS_0(MM_SUB, "=EMM= T3410 has been stoped");

#ifdef FEATURE_LTE_REL10
  /* R10 Spec 24.301 CR #1216 Correction of handling of ATTACH REJECT 
     together with SM back-off timer */
  prev_t3402_value = emm_ctrl_data_ptr->t3402_value;

  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
     (attach_reject->t3402_val_exists == TRUE))
  {
    emm_ctrl_data_ptr->t3402_value = attach_reject->t3402_val;
    emm_ctrl_data_ptr->t3402_attach_rej_param.is_t3402_recv_attach_rej = TRUE;
    emm_ctrl_data_ptr->t3402_attach_rej_param.plmn = 
                     emm_ctrl_data_ptr->plmn_service_state->plmn;

    /*Notify T3402 clients of the change in T3402 value*/
    if(prev_t3402_value != emm_ctrl_data_ptr->t3402_value)
    {
      emm_send_t3402_changed_ind(emm_ctrl_data_ptr->t3402_value);
    }
  }
  /*
  ** If T3402 value is not received in this current attach reject but UE has some stored value of
  ** t3402 from the previous attach reject in the same PLMN then continue to use that value.
  */
  else if((emm_ctrl_data_ptr->t3402_attach_rej_param.is_t3402_recv_attach_rej == TRUE) &&
          (sys_plmn_match( emm_ctrl_data_ptr->t3402_attach_rej_param.plmn,
                      emm_ctrl_data_ptr->plmn_service_state->plmn) == FALSE))
  {
    emm_ctrl_data_ptr->t3402_attach_rej_param.is_t3402_recv_attach_rej = FALSE;
  }
#endif

  /* 33.401, CR 0457 
     The ME shall store the EPS NAS security context on the USIM or non-volatile ME memory 
     only when a transition to EMM-DEREGISTERED occurs or 
     when an attempt to transition away from EMM-DEREGISTERED with the intent to 
     eventually transitioning to EMM-REGISTERED state state fails.
  */
  MSG_HIGH_DS_0(MM_SUB, "=EMM= processing Attach Reject, store the security context");
  emm_update_stored_security_context(emm_ctrl_data_ptr, FALSE);

  /* Stop ATTACH Guard timer (T3410) */
  mm_stop_timer(TIMER_T3410);

  emm_remove_release_client(emm_attach_release_indication);

#if defined  FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
#endif

#ifdef FEATURE_GSTK
  emm_send_nw_rej_evt_to_gstk(GSTK_EMM_ATTACH_REJ,
                              attach_reject->emm_cause,
                              emm_ctrl_data_ptr);
#endif

  emm_process_attach_reject_cause(attach_reject, emm_ctrl_data_ptr);
} /* end of emm_process_attach_reject() */

/*===========================================================================
FUNCTION    EMM_PROCESS_ATTACH_ACCEPT

DESCRIPTION
  This function processes attach accept message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_attach_accept
(
  lte_nas_emm_attach_accept_type *emm_attach_accept,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  dword    prev_t3402_value;

  ASSERT(emm_attach_accept != (lte_nas_emm_attach_accept_type *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->emm_tai_list_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->emergency_num_list_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Received Attach Accept message, T3410 has been stopped");

  /* Stop ATTACH Guard timer (T3410) */
  mm_stop_timer(TIMER_T3410);

#ifdef FEATURE_LTE_REL10
  /* This flag could be set if Attach is triggered due to paging.
     reset the flag */
  emm_ctrl_data_ptr->responding_to_page = FALSE;
#endif
  emm_remove_release_client(emm_attach_release_indication);

  /* Validate the ATTACH ACCEPT message */
  switch(emm_attach_accept->eps_attach_result)
  {
    case COMBINED_EPS_IMSI_ATTACH_RESULT:
      if(emm_attach_accept->emm_cause_exists == TRUE)
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Unexpected cause %d in combined ATTACH ACCEPT",
                      emm_attach_accept->emm_cause);
      }
	  if ( emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_PS_ONLY )
      {
        emm_attach_accept->eps_attach_result = EPS_ONLY_ATTACH_RESULT;
        MSG_HIGH("=EMM= change attach accept type from combined to PS only ",0,0,0);
      }
      break;
    case EPS_ONLY_ATTACH_RESULT:
      if(emm_ctrl_data_ptr->eps_attach_type == COMBINED_EPS_IMSI_ATTACH)
      {
        if(emm_attach_accept->emm_cause_exists == TRUE) 
        {   
          emm_ctrl_data_ptr->combined_attach_cs_rej_cause = 
                                                emm_attach_accept->emm_cause;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= CS attach failed - Cause %d ",
                        emm_attach_accept->emm_cause);
        }
        else 
        {
          emm_ctrl_data_ptr->combined_attach_cs_rej_cause = LTE_NAS_NO_FAILURE;
          MSG_HIGH_DS_0(MM_SUB, "=EMM= CS attached failed - No cause");
        }
      }
      break;
    default:
      /* Can not happen. MSG lib would have dropped the message. */
      MSG_FATAL_DS(MM_SUB,"=EMM= Invalid EPS attach result %d",
                emm_attach_accept->eps_attach_result,0,0);
      break;
  }

  prev_t3402_value = emm_ctrl_data_ptr->t3402_value;
  emm_ctrl_data_ptr->t3402_attach_rej_param.is_t3402_recv_attach_rej = FALSE;
  if(emm_attach_accept->t3402_val_exists == TRUE)
  {
    emm_ctrl_data_ptr->t3402_value = emm_attach_accept->t3402_val ;
  }
  else
  {
    emm_ctrl_data_ptr->t3402_value = EMM_DEFAULT_T3402 ;
  }

  /*Notify T3402 clients of the change in T3402 value*/
  if(prev_t3402_value != emm_ctrl_data_ptr->t3402_value)
  {
    emm_send_t3402_changed_ind(emm_ctrl_data_ptr->t3402_value);
  }

#ifdef FEATURE_LTE_REL9
    if(emm_ctrl_data_ptr->eps_attach_type == EPS_EMERGENCY_ATTACH)
    {
      emm_ctrl_data_ptr->emc_srv_status = LTE_NAS_EMC_SRV_ONLY;
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
    }
    else
#endif
    {
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                            SYS_SRV_STATUS_SRV;
    }

    /* Allow the DS to start sending data */
    emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend = FALSE ;

#ifdef FEATURE_LTE_REL10
    if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10 &&
       emm_attach_accept->t3412_ext_val_exists == TRUE)
    {
      emm_ctrl_data_ptr->t3412_value = emm_attach_accept->t3412_ext_val ;
    }
    else
#endif
    {
      emm_ctrl_data_ptr->t3412_value = emm_attach_accept->t3412_val ;
    }

    /* Reset Attach Attempt counter */
    emm_ctrl_data_ptr->emm_attach_attempt_counter = 0 ;

    /* Reset Attach_rej with cause ESM_FAILURE counter */
    emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter = 0 ;
    
     /* Reset Attach_rej for APN switch*/
    emm_ctrl_data_ptr->apn_switch = FALSE;

    emm_ctrl_data_ptr->manual_user_sel = FALSE;
    emm_set_mm_manual_selection_flag(FALSE);

    memscpy((void*)emm_ctrl_data_ptr->emm_tai_list_ptr, 
             sizeof(lte_nas_emm_tai_lst_info_type),
             (void*)&(emm_attach_accept->tai_lst),
             sizeof(lte_nas_emm_tai_lst_info_type));

#ifdef FEATURE_LTE_REL9 
    if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
    {
      emm_send_tai_list_ind(emm_ctrl_data_ptr);
    }

    /* Reset the Attach failed Ind type */
    emm_reset_attach_ind_status_type();
   if (emm_attach_accept->emergency_num_list_exists == TRUE)
    {
      memset(emm_ctrl_data_ptr->emergency_num_list_ptr,
             0,
             sizeof(mmr_emergency_num_list_s_type));

      memscpy(emm_ctrl_data_ptr->emergency_num_list_ptr,sizeof(mmr_emergency_num_list_s_type),
             &(emm_attach_accept->emergency_num_list),sizeof(mmr_emergency_num_list_s_type));

      mm_update_mmr_emergency_num_list_if_needed(
                                       &emm_attach_accept->emergency_num_list);
    }
	else
    {
       mm_nullify_mmr_emergency_num_list_if_needed
	   ( 
		 &(emm_ctrl_data_ptr->last_registered_tai.plmn.identity[0]),
		 &(emm_ctrl_data_ptr->plmn_service_state->plmn.identity[0])
	   );
	}

#ifdef FEATURE_LTE_REL9 
    if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
    {
      emm_cache_registration_ctxt(emm_ctrl_data_ptr);
    }
#endif

    emm_ctrl_data_ptr->last_registered_tai_valid = TRUE ;
    emm_ctrl_data_ptr->last_registered_tai.plmn = 
      emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
    emm_ctrl_data_ptr->last_registered_tai.tac = 
                   emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;

#ifdef FEATURE_LTE_REL9 
    if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
    {
       /* Remove the registered PLMN from FPLMN lists if necessary for non emc attach*/
       emm_remove_forbidden_plmn(emm_ctrl_data_ptr->last_registered_tai.plmn,
                              emm_ctrl_data_ptr);
    }
#else
    /* Remove the registered PLMN from FPLMN lists if necessary for non emc attach*/
    emm_remove_forbidden_plmn(emm_ctrl_data_ptr->last_registered_tai.plmn,
                            emm_ctrl_data_ptr);
#endif
#ifdef FEATURE_FEMTO_CSG
#ifdef FEATURE_LTE_REL9 
    if(emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)
#endif
    {
      mm_add_csg_to_white_list(emm_ctrl_data_ptr->last_registered_tai.plmn, 
                                        &emm_ctrl_data_ptr->plmn_service_state->csg_info);
      mm_remove_csg_from_rejected_list(
                         emm_ctrl_data_ptr->last_registered_tai.plmn,
                         emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id);
    }
#endif 
    if (emm_attach_accept->guti_type_exists == TRUE)
    {
      emm_ctrl_data_ptr->emm_guti_valid = TRUE;
      emm_ctrl_data_ptr->emm_guti = emm_attach_accept->guti ;
      emm_ctrl_data_ptr->plmn_service_state->rac_or_mme_code = emm_ctrl_data_ptr->emm_guti.mme_code;
      /*Set the TIN to point to GUTI*/
      emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
    }
    else
    {
      if(emm_ctrl_data_ptr->emm_guti_valid == FALSE)
      {
        emm_set_and_write_tin_type(NAS_MM_TIN_NONE);
        MSG_ERROR_DS_0(MM_SUB, "=EMM= GUTI inavlid after attach accept received.This shouldn't happen");
      }
      else
      {
        emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
      }
    }

    /* Set Update status */
    emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_UPDATED ;

    /* Update EMM USIM */
    emm_save_mobility_info(emm_ctrl_data_ptr);

    /* Need to update REG with EPLMN */
    emm_ctrl_data_ptr->update_eplmn_list = TRUE;

    emm_ctrl_data_ptr->rplmn_info.rplmn = emm_ctrl_data_ptr->last_registered_tai.plmn;
    emm_ctrl_data_ptr->rplmn_info.rplmn_type = MM_EPS_RPLMN_TYPE;

    /* Update current lte_cs_domain_param */
    emm_ctrl_data_ptr->reg_lte_cs_domain_param = 
           emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param;

    *(emm_ctrl_data_ptr->reg_lte_pended_cs_domain_param_ptr) = 
           emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param;
    /*We don't need to indicate REG & RRC, of EPLMN list changes here...
    at the end of ATTACH procedure, it gets notified*/
    if (emm_attach_accept->plmn_exists == TRUE)
    {
      emm_ctrl_data_ptr->emm_equivalent_PLMN_list = 
                                                   emm_attach_accept->plmn_lst;
    }
    else
    {
      /*Delete the existing EQ_PLMN list, if it is not present in the ATTACH_ACCEPT*/
      emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
    }  
    
    if(emm_attach_accept->eps_nw_feature_support_exists == TRUE)
    {
      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.ims_vops_supported = 
                                       emm_attach_accept->eps_nw_feature_support.ims_vops_supported;
#ifdef FEATURE_LTE_REL9
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
      {
        emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_last_eutran_voims_supported = 
                                       emm_attach_accept->eps_nw_feature_support.ims_vops_supported;
      }
#endif
#ifdef FEATURE_LTE_REL9
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
      {
        emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.lcs_supported = 
                                         emm_attach_accept->eps_nw_feature_support.lcs_supported;
        emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.emc_bs_supported = 
                                         emm_attach_accept->eps_nw_feature_support.emc_bs_supported;
        emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.cs_lcs_val = 
                                         emm_attach_accept->eps_nw_feature_support.cs_lcs_val;
#ifdef FEATURE_LTE_REL10
        emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.esr_ps_supported = 
                                          emm_attach_accept->eps_nw_feature_support.esr_ps_supported;
#endif
      }
#endif  
    }
    else
    {
      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.ims_vops_supported = FALSE;
#ifdef FEATURE_LTE_REL9
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
      {
        emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.lcs_supported = FALSE;
        emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.emc_bs_supported = FALSE;
        emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.cs_lcs_val = SYS_CS_LCS_INFO_NOT_AVAILBLE;
#ifdef FEATURE_LTE_REL10
        emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.esr_ps_supported = FALSE;
#endif
      }
#endif
#ifdef FEATURE_LTE_REL9
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
      {
        emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_last_eutran_voims_supported = FALSE;
      }
#endif
    }    
#ifdef FEATURE_LTE_REL9
    if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
    {
      /* Process new TAI list in order to update forbidden lists */
      emm_process_tai_list(emm_ctrl_data_ptr->emm_tai_list_ptr, emm_ctrl_data_ptr) ;
    }

    emm_send_rrc_system_update(emm_ctrl_data_ptr);     


    /*If T3423 exists in attach accept use it, otherwise set it to T3412*/
    if(emm_attach_accept->t3423_val_exists == TRUE)
    {
      emm_ctrl_data_ptr->t3423_value = emm_attach_accept->t3423_val;
    }
    else
    {
      emm_ctrl_data_ptr->t3423_value = emm_attach_accept->t3412_val;
    }

    if(emm_attach_accept->eps_attach_result == EPS_ONLY_ATTACH_RESULT)
    {
      /* EPS only attach */
      /* PLMN supports PS ONLY */
      emm_ctrl_data_ptr->plmn_service_state->plmn_service_capability = 
                                            SYS_SRV_CAPABILITY_PS_SERVICE_ONLY;
  
      /* UE is PS ONLY registered */
      emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                        SYS_SRV_DOMAIN_PS_ONLY;
      emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_PS_ONLY;
  
      if(
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
         (reg_sim_per_subs_is_ehplmn(emm_ctrl_data_ptr->plmn_service_state->plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
         (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), emm_ctrl_data_ptr->plmn_service_state->plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
         (reg_sim_is_ehplmn(emm_ctrl_data_ptr->plmn_service_state->plmn) )
#else
         (sys_plmn_match(reg_sim_read_hplmn(), emm_ctrl_data_ptr->plmn_service_state->plmn))
#endif
#endif
        )
      {
        if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE) 
        {
         emm_ctrl_data_ptr->is_accepted_on_lte = TRUE;
      }
      }
      else if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming== TRUE)
      {
        emm_ctrl_data_ptr->emm_lte_accepted_vplmn = emm_ctrl_data_ptr->plmn_service_state->plmn;
      }
      if(emm_ctrl_data_ptr->eps_attach_type == EPS_ONLY_ATTACH)
      {
        emm_ctrl_data_ptr->plmn_service_state->cs_service_status = SYS_SRV_STATUS_NO_SRV;
        emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
      }

      /* Reset Tracking Area Attempt counter */
      emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
    }
    else
    {
      /* Combined attach successful. Follow 5.5.1.3.4.2. to update MM */
      mm_update_cs_update_success_status(
                           (lte_nas_emm_incoming_msg_type *)emm_attach_accept);

      if(
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
         (reg_sim_per_subs_is_ehplmn(emm_ctrl_data_ptr->plmn_service_state->plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
         (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), emm_ctrl_data_ptr->plmn_service_state->plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
         (reg_sim_is_ehplmn(emm_ctrl_data_ptr->plmn_service_state->plmn) )
#else
         (sys_plmn_match(reg_sim_read_hplmn(), emm_ctrl_data_ptr->plmn_service_state->plmn))
#endif
#endif
        )
      {
        if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE) 
        {
        emm_ctrl_data_ptr->is_accepted_on_lte = TRUE;
      }
      }
      else if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming== TRUE)
      {
        emm_ctrl_data_ptr->emm_lte_accepted_vplmn = emm_ctrl_data_ptr->plmn_service_state->plmn;
      }
      /* PLMN supports CS & PS */
      emm_ctrl_data_ptr->plmn_service_state->plmn_service_capability = 
                                            SYS_SRV_CAPABILITY_CS_PS_SERVICE;

      /* UE is CS & PS registered */
      emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                          SYS_SRV_DOMAIN_CS_PS;
      emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_CS_PS;

      emm_ctrl_data_ptr->plmn_service_state->cs_service_status = 
                                                            SYS_SRV_STATUS_SRV;

      if(emm_attach_accept->additional_update_result_exists)
      {
        emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability = 
                                  emm_attach_accept->additional_update_result;
      }
      else if((emm_ctrl_data_ptr->eps_attach_type == COMBINED_EPS_IMSI_ATTACH)&&
              (emm_ctrl_data_ptr->reg_req_pending_ptr->sms_only == TRUE))
      {
        emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability = SYS_LTE_CS_CAPABILITY_SMS_ONLY;
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Overwriting CS capability with SMS only");
      }
      else
      {
        emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability = SYS_LTE_CS_CAPABILITY_NO_VALUE;           
      }
 
    }
    if(emm_ctrl_data_ptr->imsi_attach == TRUE)
    {
      /* Send attach ind to 1XCP */
      emm_send_irat_imsi_attach_ind();
    }
    if(emm_ctrl_data_ptr->emm_guti_valid == TRUE)
    {
      emm_send_sim_update(emm_ctrl_data_ptr) ;
    }


    /* Update GW flag(s) */          
    emm_attach_tau_success_update_gw(); 
    emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;

    /* Send DRX update to W-RRC */
    mm_send_rrc_set_drx_ind(); 
    
    /* Send DRX update to L-RRC  */
    emm_send_rrc_drx_update();

} /* end of emm_process_attach_accept() */

void emm_timer_t3440_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  MSG_HIGH_DS_0(MM_SUB, "=EMM= T3440 timer expired, sending abort connection to RRC");
  emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL) ;

} /* end of emm_timer_t3440_expire() */

/*===========================================================================
FUNCTION    EMM_TIMER_T3410_EXPIRE

DESCRIPTION
  This function processes timer 3410 expiry

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_timer_t3410_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  emm_send_diag_event(EVENT_NAS_ATTACH, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_TIME_OUT, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
  is_nas_emm_attach_tau_timeout = TRUE;

  emm_remove_release_client(emm_attach_release_indication);
  emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL) ;
  emm_ctrl_data_ptr->apn_switch = TRUE;
  emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);
  emm_start_attach_recovery(emm_ctrl_data_ptr) ;

} /* end of emm_timer_t3410_expire() */

/*===========================================================================
FUNCTION    EMM_ATTACH_TIMER_T3402_T3411_EXPIRE

DESCRIPTION
  This function processes timer 3402/3411 expiry

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_attach_timer_t3402_t3411_expire
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr,
  timer_id_T           timer_id  
)
{
  emm_state_type     emm_state = EMM_INVALID_STATE ;
  emm_substate_type  emm_substate;
  lte_nas_tai_lst1_type  serving_tai;

  ASSERT(emm_ctrl_data_ptr != NULL);
  if((emm_ctrl_data_ptr->last_rrc_service_ind_ptr == NULL) ||
     (emm_ctrl_data_ptr->reg_req_pending_ptr == NULL))
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function emm_attach_timer_t3402_t3411_expire");
    return;
  }

  if(emm_ctrl_data_ptr->rrc_active == FALSE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= T3402/3411 expiry while RRC is not active");
    return;
  }

  serving_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
  serving_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;

  if((timer_id == TIMER_T3402) &&
     ((emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode ==
                      SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
      (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode ==
                      SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)))
  {
    if(emm_ctrl_data_ptr->plmn_service_state->service_status != 
                                                 SYS_SRV_STATUS_NO_SRV)
    {
      switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
      {
        case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
         if(emm_is_plmn_reg_allowed(emm_ctrl_data_ptr->last_rrc_service_ind_ptr,
                                    emm_ctrl_data_ptr) == TRUE)            
          {
            emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                                   SYS_REG_DOMAIN_PS_SERVICE_ONLY;
          }
          else 
          {
            emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                                   SYS_REG_DOMAIN_CAMPED_ONLY;
          }
          break;
        case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
          if(emm_is_manual_reg_allowed(serving_tai, emm_ctrl_data_ptr) == TRUE)
          {
            emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                                   SYS_REG_DOMAIN_PS_SERVICE_ONLY;
          }
          else 
          {
            emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                                   SYS_REG_DOMAIN_CAMPED_ONLY;
          }
          break;
        default:
          break;
      }
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                 SYS_SRV_STATUS_LIMITED;
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
    }
    return;
  }

  if((emm_registration_allowance_chk(emm_ctrl_data_ptr->last_rrc_service_ind_ptr,
                                    emm_ctrl_data_ptr) == FALSE)
#ifdef FEATURE_LTE_REL9
     &&(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
    )
  {
    return;
  }

  if((emm_chk_ta_change(emm_ctrl_data_ptr->last_rrc_service_ind_ptr, 
                       emm_ctrl_data_ptr) == TRUE)||
      (reg_sim_plmn_backoff_forbidden(serving_tai.plmn,(sys_modem_as_id_e_type)mm_sub_id) == TRUE))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= T3402/3411 expiry on different TAI or PLMN backoff forbidden");
    return;  
  }

  emm_state = EMM_GET_STATE() ;
  emm_substate = EMM_GET_SUBSTATE() ;

  switch (emm_state)
  {
    case EMM_DEREGISTERED:
      if((emm_substate == EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH) || 
         ((emm_substate == EMM_DEREGISTERED_ATTACH_NEEDED) &&
          (emm_ctrl_data_ptr->reg_req_pended_indication == FALSE)))
      {
        if(timer_id == TIMER_T3402)
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN, 
                             FALSE,
                             emm_ctrl_data_ptr);
        }
        emm_init_attach_procedure(EMM_UPDATE_RECOVERY,emm_ctrl_data_ptr) ;
      }
      else
      {
        MSG_HIGH_DS_2(MM_SUB, "=EMM= Attach delayed - DEREG %d state, RRC camping %d",
                      emm_substate,
                      emm_ctrl_data_ptr->reg_req_pended_indication);
      }
      break ;
    case EMM_REGISTERED:
      if((emm_substate == EMM_REGISTERED_NORMAL_SERVICE) 
         ||
         (emm_substate == EMM_REGISTERED_ATTEMPTING_TO_UPDATE) 
         ||
         (emm_substate == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM) 
         ||
         ((emm_substate == EMM_REGISTERED_PLMN_SEARCH) &&
          (emm_ctrl_data_ptr->reg_req_pended_indication == FALSE)))
      {
        if(timer_id == TIMER_T3402)
        {
          if(emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                        SYS_SRV_STATUS_LIMITED)
          {
            /* Notify CM limited regional service to avoid CM service request 
               interrupts the TAU procedure */
            reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN, 
                               FALSE,
                               emm_ctrl_data_ptr);        
          }

          if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_PERIODIC)
          {
            /* No need to perform periodic TAU upon T3402 expiry any more */
            emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
          }

          /* Perform init TAU upon T3402 expiry */
          emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                                     emm_get_eps_update_type(emm_ctrl_data_ptr),
                                     FALSE,
                                     emm_ctrl_data_ptr);
        }
        else
        {          
          emm_initiate_tau_procedure(EMM_UPDATE_RECOVERY,
                                     emm_ctrl_data_ptr->emm_ongoing_eps_ta_update,
                                     FALSE,
                                     emm_ctrl_data_ptr);
        }
      } 
      else
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= TAU not started in REGISTERED %d substate",
                      emm_substate);
      }
      break;
    case EMM_SERVICE_REQUEST_INITIATED:
    case EMM_DEREGISTERED_INITIATED:
      /* Note: Delayed TAU or TAU retry. No need to set TAU cause again in emm
         data base. */
      emm_initiate_tau_procedure(EMM_UPDATE_RECOVERY,
                                 emm_ctrl_data_ptr->emm_ongoing_eps_ta_update,
                                 FALSE,
                                 emm_ctrl_data_ptr);
      break ;
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= Illegal EMM state. ignored");
      break ;
    case EMM_NULL:
    case EMM_REGISTERED_INITIATED:
    default :
      MSG_FATAL_DS(MM_SUB,"=EMM= Illegal EMM state",0,0,0) ;
      break ;
  }
} /* end of emm_attach_timer_t3402_t3411_texpire() */

#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
/*=========================================================================== 
 
FUNCTION  EMM_TIMER_RADIO_RETRY_EXPIRE

DESCRIPTION
  This function handles radio retry timer expiry
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_timer_radio_retry_expire
(
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  emm_state_type     emm_state = EMM_INVALID_STATE ;

  emm_state = EMM_GET_STATE();

  switch(emm_state)
  {
    case EMM_REGISTERED:
    case EMM_DEREGISTERED:
      emm_attach_timer_t3402_t3411_expire(emm_ctrl_data_ptr, TIMER_EMM_RADIO_RETRY);
      break;

    case EMM_DEREGISTERED_INITIATED:
      emm_start_mo_detach(emm_ctrl_data_ptr->ongoing_detach_type, FALSE, NULL, 
                        emm_detach_failed_routine, emm_ctrl_data_ptr) ;
      break;

    case EMM_TRACKING_AREA_UPDATING_INITIATED:;
    case EMM_NULL:
    case EMM_REGISTERED_INITIATED:
    case EMM_SERVICE_REQUEST_INITIATED:
    default :
      MSG_ERROR_DS_0(MM_SUB, "=EMM= Illegal EMM state. ignored");
      break;
  }

}

#endif 

#ifdef FEATURE_LTE_REL10
/*===========================================================================
FUNCTION    EMM_TIMER_T3346_EXPIRE

DESCRIPTION
  This function processes timer 3346 expiry
  - called on timer expiry
  - called on receiving page
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_timer_t3346_expire
(
  boolean             active_flag,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  emm_state_type     emm_state = EMM_INVALID_STATE ;

  boolean            is_reg_allowed = FALSE;

  sys_plmn_id_s_type     rrc_reported_plmn ;
  lte_nas_tai_lst1_type  tai;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  if((emm_ctrl_data_ptr->last_rrc_service_ind_ptr == NULL)||
     (emm_ctrl_data_ptr->reg_req_pending_ptr == NULL))
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function emm_timer_t3346_expire");
    return;
  }
  
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL10)
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= T3346 timer expired without REL10 NV %d",
                   emm_ctrl_data_ptr->emm_3gpp_spec_ver);
    return;
  }

  /* wait for new serv_ind if below conditions is true*/
  if((emm_ctrl_data_ptr->rrc_active == FALSE) 
     ||
     (emm_ctrl_data_ptr->last_rrc_service_ind_ptr->svc_status == 
                           LTE_RRC_SVC_STATUS_NOT_AVAILABLE) 
     ||
     (emm_ctrl_data_ptr->reg_req_pended_indication == TRUE))
  {
    MSG_HIGH_DS_3(MM_SUB, "=EMM= T3346 expiry. rrc active %d svc_status %d reg_pended %d",
                  emm_ctrl_data_ptr->rrc_active,
                  emm_ctrl_data_ptr->last_rrc_service_ind_ptr->svc_status,
                  emm_ctrl_data_ptr->reg_req_pended_indication);
    return;
  }

  gmm_set_gmm_is_congestion_due_to_rau(FALSE);
  sys_plmn_undefine_plmn_id(emm_ctrl_data_ptr->t3346_plmn_ptr);
  
  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr-> \
                       last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;

  emm_state    = EMM_GET_STATE() ;


  switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
  {
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
      if((emm_is_plmn_forbidden(rrc_reported_plmn,emm_ctrl_data_ptr) == FALSE) &&
         (emm_is_tai_forbidden(tai,emm_ctrl_data_ptr)== FALSE) &&
         (emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info. \
                        cell_access_status != LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY))
      {
        is_reg_allowed = TRUE;
      }          
      break;

    case SYS_NETWORK_SELECTION_MODE_MANUAL:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
      if(emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE)
      {
        is_reg_allowed = TRUE;
      }
      break;

    case SYS_NETWORK_SELECTION_MODE_LIMITED:
      /* do not start registraion wait for serv_req */
      is_reg_allowed = FALSE;
      break;

    default:
      MSG_HIGH_DS_1(MM_SUB, "=EMM= network_selection_mode not expected %d",
                    emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
      break; 
  }

  /* if reg is allowed start attach/tau*/
  if(is_reg_allowed == TRUE)
  {
    switch (emm_state)
    {
      case EMM_DEREGISTERED:
        emm_init_attach_procedure(EMM_INITIAL_UPDATE,emm_ctrl_data_ptr) ;
        break ;
      case EMM_REGISTERED:
        emm_registered_process_possible_tau_rrc_service_ind(
             emm_ctrl_data_ptr->last_rrc_service_ind_ptr,emm_ctrl_data_ptr);
        break;

      default :
        MSG_ERROR_DS_0(MM_SUB, "=EMM= Illegal EMM state");
        break ;
    }
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= T3346 timer expiry registraion not allowed");
     
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                      SYS_SRV_STATUS_LIMITED;
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, 
                       FALSE, emm_ctrl_data_ptr);
  }

  mm_send_mmr_t3346_timer_ind(SYS_NAS_T3346_STATUS_EXPIRED);
} /* end of emm_timer_t3346_expire() */

#endif

/*===========================================================================

FUNCTION  emm_poweroff_detach_release_ind

DESCRIPTION
  This function processes the case of receiving signaling connection release
  indication in a poweroff MO detach procedure. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_poweroff_detach_release_ind
(
  lte_emm_connection_cause_type  release_cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr,
  lte_rrc_conn_rel_redir_info_s  redir_info,
  lte_nas_tau_cause_type tau_cause
)
{
  emm_state_type                 emm_state    = EMM_INVALID_STATE ;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  emm_state = EMM_GET_STATE() ;

  if (emm_state != EMM_NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Poweroff dtach conn release CB - Need to deact RRC");
    emm_init_rrc_deact_req(emm_ctrl_data_ptr->rrc_deact_req.deact_reason, 
                           emm_ctrl_data_ptr);
  }
} /* end of emm_poweroff_detach_release_ind() */

/*===========================================================================

FUNCTION  EMM_COMPLETE_MO_DETACH

DESCRIPTION
  This function handles MO detach procedure completion.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_complete_mo_detach
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != NULL); 

  MSG_MED_DS_0(MM_SUB, "=EMM= Completing MO DETACH");

  mm_stop_timer(TIMER_T3421);

  emm_reset_poweroff_detach_params(emm_ctrl_data_ptr);

  /* Update EMM state */
  if(emm_ctrl_data_ptr->ongoing_detach_type == IMSI_DETACH_MO_DETACH)
  {
    EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);
    emm_send_diag_event(EVENT_NAS_MO_DETACH, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_ACCEPT, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
    /* Send REG REQ CNF */
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
  }
  else
  {
    /* Non-switchoff MO detach complete. Update service status to limited. */
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
  
    if(emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                   SYS_SIM_STATE_NOT_AVAILABLE)
    { 
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_IMSI) ;
      /*I guess there is no need to send LIMITED_SEVICE event to GSTK, as sim is not available*/
    }
    else
    {
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH) ;

      /* Send REG REQ CNF */
      reg_service_report(MMR_CAUSE_FAILURE_MAX_ATTEMPTED, FALSE, emm_ctrl_data_ptr) ;

      /*Need not send LIMITED_SERVICE event to GSTK*/
    }
    /* Update GMM state if necessary */
    gmm_multimode_set_state(GMM_DEREGISTERED, GMM_NO_CELL_AVAILABLE);
  }
    emm_send_rrc_system_update(emm_ctrl_data_ptr);

} /* end of emm_complete_mo_detach() */

/*===========================================================================

FUNCTION  emm_mo_detach_release_ind

DESCRIPTION
  This function processes the case of receiving signaling connection release
  indication in a non-poweroff MO detach procedure. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_mo_detach_release_ind
(
  lte_emm_connection_cause_type  release_cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr,
  lte_rrc_conn_rel_redir_info_s  redir_info,
  lte_nas_tau_cause_type tau_cause
)
{
  ASSERT(emm_ctrl_data_ptr != NULL); 
 
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Processing conn rel ind during MO detach procedure");

  if(EMM_GET_STATE() != EMM_DEREGISTERED)
  {
    emm_complete_mo_detach(emm_ctrl_data_ptr);
  }
} /* end of emm_mo_detach_release_ind() */

/*===========================================================================

FUNCTION  emm_detach_failed_routine

DESCRIPTION
  This function processes the transmission failures or lower layer failures in
  a non-poweroff MO detach procedure. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE   - Need to start Tracking Area Updating procedure
  FALSE  - No need to start Tracking Area Updating procedure

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_detach_failed_routine
(
  lte_emm_connection_cause_type  cause,
  dword                          trans_id,
  emm_failure_type               emm_failure_cause,
  lte_nas_tai_lst1_type          tai,
  struct emm_pended_msg_list_tag *message_ptr,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  emm_pended_msg_list_type *pended_message_ptr = (struct emm_pended_msg_list_tag *)message_ptr ;

  ASSERT(pended_message_ptr != (emm_pended_msg_list_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ;

  if( EMM_GET_STATE() == EMM_DEREGISTERED_INITIATED ) 
  {
    emm_remove_release_client(emm_mo_detach_release_ind);
    switch (cause)
    {
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO:
        /* If TAI is not part of the saved list abort Detach and re-initiate after TAU */
        /* emm_abort_rrc_connection() ; */
        /* 5.5.2.2.4 Abnormal cases in the UE: g */
        MSG_ERROR_DS_0(MM_SUB, "=EMM= Detach failed due to transmission failure w/ TAI change");
        emm_start_mo_detach(emm_ctrl_data_ptr->ongoing_detach_type, FALSE, NULL, emm_detach_failed_routine, emm_ctrl_data_ptr) ;
       
        break ;
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_RLF:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
        /* 5.5.2.2.4 Abnormal cases in the UE: h */
        MSG_ERROR_DS_0(MM_SUB, "=EMM= Detach failed due to transmission failure w/o TAI change");
        emm_start_mo_detach(emm_ctrl_data_ptr->ongoing_detach_type, 
                            FALSE, 
                            NULL,
                            emm_detach_failed_routine,
                            emm_ctrl_data_ptr);
        break;
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ABORTED:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CELL_RESEL:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_SI_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TAI_CHANGED:
        /* 5.5.2.2.4 Abnormal cases in the UE: b */
        MSG_HIGH_DS_0(MM_SUB, "=EMM= MO detach failed due to lower layer failure");
        emm_complete_mo_detach(emm_ctrl_data_ptr);
        break ;
      case CONN_REL_LTE_RRC_CONN_REL_NORMAL:
      case CONN_REL_LTE_RRC_CONN_REL_RLF:
      case CONN_REL_LTE_RRC_CONN_REL_CRE_FAILURE:
      case CONN_REL_LTE_RRC_CONN_REL_OOS_DURING_CRE:
      case CONN_REL_LTE_RRC_CONN_REL_ABORTED:
      case CONN_REL_LTE_RRC_CONN_REL_SIB_READ_ERROR:
      case CONN_REL_LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS:
        /* Connection release is handled by procedure release indication client 
           for this case. Do nothing here. */
        break;
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
#ifdef FEATURE_NAS_EAB
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
#endif
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
        /* 5.5.2.2.4 Abnormal cases in the UE: a */
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Detach request failed due to conn est failure,  %d,  T3421 stopped - Access barred/Conn Rej",
                      cause);
        /* Detach may be restarted on access barring timer expiry */
        mm_stop_timer(TIMER_T3421);
        break;

  #if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Detach request failed due to conn est failure due to TRM %d",
                      cause);
        mm_stop_timer(TIMER_T3421);
        mm_start_timer(TIMER_EMM_RADIO_RETRY, DEFAULT_TIMEOUT);
        break;
  #endif 

      default :
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM connection cause",0,0,0) ;
        break ;
    }
  }

  pended_message_ptr->message_sent_indicator = TRUE ;

  return FALSE ;
} /* end of emm_detach_failed_routine() */

/*===========================================================================

FUNCTION  emm_start_mo_detach

DESCRIPTION
  This function starts the MO detach procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_start_mo_detach
(
  emm_mo_detach_type          detach_type,
  boolean                     power_off_indicator,
  rrc_l2_callback_type        l2_ack,
  rrc_connection_failure_type tx_failed,
  emm_ctrl_data_type         *emm_ctrl_data_ptr
)
{
  lte_nas_emm_mo_detach_req_type*     mo_detach_request ;
  dword                               sequence_number = 0xFFFFFFFF;
  emm_release_cb_func_ptr             detach_release_cb_func_ptr = NULL;
  emm_state_type                      emm_state    = EMM_INVALID_STATE;
  emm_substate_type                   emm_substate = EMM_UNKNOWN_STATE;
  emm_security_context_info *context_ptr = NULL;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 
  
  emm_state    = EMM_GET_STATE();
  emm_substate = EMM_GET_SUBSTATE();
  
  if((detach_type != IMSI_DETACH_MO_DETACH) 
#ifdef FEATURE_LTE_REL9
     &&
     (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)
#endif
    )
  {
    /* Report to ESM */
    emm_build_and_send_detach_ind(EMM_DETACHED);
  }

  if (power_off_indicator == TRUE)
  {
    emm_ctrl_data_ptr->detach_reason = SWITCH_OFF;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Start MO switch-off detach procedure, Starting POWER_OFF_DETACH_TIMER");
    nas_emm_power_off_detach_type = detach_type;
    mm_start_timer(EMM_POWEROFF_DETACH_TIMER, 
                   EMM_POWEROFF_DETACH_TIMER_DURATION);
    mm_stop_timer(TIMER_T3411);
    mm_stop_timer(TIMER_T3402);

  }
  else
  {
    emm_ctrl_data_ptr->detach_reason = NORMAL_DETACH;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Start MO normal detach procedure");

    emm_ctrl_data_ptr->ongoing_detach_procedure = TRUE ;
    emm_ctrl_data_ptr->ongoing_detach_type = detach_type ;
  }

  /* Move to this state for all types of MO detach*/
  /* In future, check if we need to clear the pending message list/buffer when moving into this state*/
  if(detach_type == IMSI_DETACH_MO_DETACH)
  {
    EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_IMSI_DETACH_INITIATED);
  }
  else
  {
    EMM_MOVES_TO_DEREGISTERED_INITIATED_STATE();
  }
  emm_write_tin_to_efs(emm_ctrl_data_ptr);

  if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE)
  {
  emm_write_lte_accepted_status_to_efs(emm_ctrl_data_ptr);
  }
  if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming== TRUE) 
  {
    emm_write_lte_accepted_vplmn_status_to_efs(emm_ctrl_data_ptr);
  }

  memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
         0x0,
         sizeof(lte_nas_outgoing_msg_type)) ;

  mo_detach_request = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.mo_detach_req;

  mo_detach_request->lte_nas_hdr.pd     = EPS_MOBILITY_MANAGEMENT_MESSAGES ;

  mo_detach_request->lte_nas_hdr.msg_id = DETACH_REQUEST ;

  mo_detach_request->security_hdr = PLAIN_NAS_MSG ;

  mo_detach_request->detach_req.mo_detach = detach_type ;

  if (power_off_indicator == TRUE)
  {
    mo_detach_request->detach_req.detach_reason = SWITCH_OFF ;
    detach_release_cb_func_ptr = emm_poweroff_detach_release_ind ;
  }
  else
  {
    mo_detach_request->detach_req.detach_reason = NORMAL_DETACH ;
    detach_release_cb_func_ptr = emm_mo_detach_release_ind ;
  }

  if(emm_ctrl_data_ptr->emm_guti_valid == TRUE)
  {
    MSG_LOW_DS_0(MM_SUB, "=EMM= MO Detach Req EPS Mobile Identity - GUTI");
    mo_detach_request->mobile_id.ue_id = GUTI_ID ;
    mo_detach_request->mobile_id.ue_id_info.guti = emm_ctrl_data_ptr->emm_guti ;
  }
  else if(emm_ctrl_data_ptr->plmn_service_state->sim_state !=  
                                                 SYS_SIM_STATE_NOT_AVAILABLE)
  {
    MSG_LOW_DS_0(MM_SUB, "=EMM= MO Detach Req EPS Mobile Identity - IMSI");
    mo_detach_request->mobile_id.ue_id = IMSI_ID ;
    mo_detach_request->mobile_id.ue_id_info.imsi.num_id_digits_len = 
                                            emm_ctrl_data_ptr->imsi.num_digits;
    if(emm_ctrl_data_ptr->imsi.num_digits <= MAX_NO_OF_IMSI)
    {
      mo_detach_request->mobile_id.ue_id_info.imsi.num_id_digits_len = 
                                                                MAX_NO_OF_IMSI;
        memscpy((void*)&mo_detach_request->mobile_id.ue_id_info.imsi.num_id_digits[0],
                sizeof(mo_detach_request->mobile_id.ue_id_info.imsi.num_id_digits),
                (void*)&emm_ctrl_data_ptr->imsi.imsi_digits[0],
                emm_ctrl_data_ptr->imsi.num_digits) ;    
    }
    else
    {
      MSG_ERROR_DS_2(MM_SUB, "=EMM= num_digits %d exceeds imsi length, concatinating to %d",
                     emm_ctrl_data_ptr->imsi.num_digits,
                     MAX_NO_OF_IMSI);
      memscpy((void*)&mo_detach_request->mobile_id.ue_id_info.imsi.num_id_digits[0],
              sizeof(mo_detach_request->mobile_id.ue_id_info.imsi.num_id_digits),
              (void*)&emm_ctrl_data_ptr->imsi.imsi_digits[0],
              sizeof(emm_ctrl_data_ptr->imsi.imsi_digits));
    }
  }
#ifdef FEATURE_LTE_REL9
  else
  {
    mo_detach_request->mobile_id.ue_id = IMEI_ID;
    emm_populate_imei(&mo_detach_request->mobile_id.ue_id_info.imei, TRUE);
  }
#endif

  /* Set the security context type */
  /* Note: For march spec, since only one context is supported
     copy the info from emm_ctrl_data
    */

    context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

    if( context_ptr != NULL )
    {
      mo_detach_request->nas_key_set_identifier = context_ptr->nasKSI;
    } else 
    {
      mo_detach_request->nas_key_set_identifier.keyset_id= NO_KEY_AVAIALABLE;
    }
    /* Obtain sequence number */
    sequence_number = emm_get_nas_transaction_id() ;

  if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number, 
                                MSGR_NAS_EMM,
                                l2_ack, tx_failed, 
                                emm_ctrl_data_ptr))
  {
    if (FALSE == emm_connection_setup(LTE_RRC_EST_CAUSE_MO_SIGNALING,
                                      emm_ctrl_data_ptr->out_msg_ptr, 
                                      sequence_number, 
                                      MSGR_NAS_EMM, 
                                      l2_ack, 
                                      tx_failed,
                                      ROUTING_INFO_STMSI,
                                      NULL,
                                      emm_ctrl_data_ptr))
    {
      /* restore the state/substate back */
      emm_ctrl_data_ptr->emm_state = emm_state;
      emm_ctrl_data_ptr->emm_substate = emm_substate;

      MSG_ERROR_DS_0(MM_SUB, "=EMM= Initial Detach attempt failed due to Access Class barring");
        
      return;
    }

#ifdef FEATURE_LTE_REL11
    if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
       (emm_ctrl_data_ptr->detach_reason == NORMAL_DETACH) && 
       (emm_ctrl_data_ptr->ongoing_detach_type == IMSI_DETACH_MO_DETACH))
    {
      emm_ctrl_data_ptr->start_t3440_on_accept = TRUE;
    }
#endif

  }
  /* DETACH REQUEST message has been sent, start T3421 and update EMM state for
     MO detach procudure not due to power off */
  if (power_off_indicator != TRUE)
  {
    mm_start_timer(TIMER_T3421,EMM_T3421_TIMER_DURATION) ;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Started T3421 timer");
  }

  /* Register connection release client for MO detach procedure.
     The client needs to be removed upon
     - Switch off 
     -- Power down timer expiry
     - Other MO detach
     -- Detach request sending failure, detach accept/reject reception and 
        T3421 expiry */ 
  emm_add_new_release_client(detach_release_cb_func_ptr);

} /* end of emm_start_mo_detach */

/*===========================================================================

FUNCTION  emm_t3421_expire

DESCRIPTION
  This function processes the timer 3421 expiry for MO detach procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_t3421_expire
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
#ifdef FEATURE_DUAL_SIM
  ASSERT(emm_ctrl_data_ptr->dds_switch_pended_ptr != NULL); 
#endif 

  if(emm_ctrl_data_ptr->rrc_active == FALSE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= T3421 expiry while RRC is not active");
    return;
  }

  /* Increase T3421 expiry counter */
  emm_ctrl_data_ptr->emm_detach_attempt_counter++ ;

  emm_remove_release_client(emm_mo_detach_release_ind);

  /* EMM PS detach procedure will be aborted on T3421 expiry
     in case PS Detach is triggered due to DDS switch completion
   */

#ifdef FEATURE_DUAL_SIM
  if (*emm_ctrl_data_ptr->dds_switch_pended_ptr)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= T3421 counter set to max attempts for DDS switch");
    emm_ctrl_data_ptr->emm_detach_attempt_counter = MAX_NUMBER_DETACH_OF_ATTEMPTS;
  }
#endif

  if (emm_ctrl_data_ptr->emm_detach_attempt_counter < 
                                                 MAX_NUMBER_DETACH_OF_ATTEMPTS)  
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= T3421 expired, retry MO detach. Attempt No. %d",
                  emm_ctrl_data_ptr->emm_detach_attempt_counter);
    /* Retry detach on the first four expiries */
    emm_start_mo_detach(emm_ctrl_data_ptr->ongoing_detach_type, FALSE, NULL, 
                        emm_detach_failed_routine, emm_ctrl_data_ptr) ;
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= T3421 expired and max attempts reached");
    emm_complete_mo_detach(emm_ctrl_data_ptr);    
  }
} /* end of emm_t3421_expire() */

/*===========================================================================

FUNCTION  EMM_POWERDOWN_TIMER_EXPIRE

DESCRIPTION
  This function processes the power down timer expiry for switch off MO detach 
  procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_powerdown_timer_expire
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_remove_release_client(emm_poweroff_detach_release_ind);
  EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
  emm_reset_poweroff_detach_params(emm_ctrl_data_ptr);
  emm_init_rrc_deact_req(emm_ctrl_data_ptr->rrc_deact_req.deact_reason, 
                         emm_ctrl_data_ptr);

} /* end of emm_powerdown_timer_expire */

/*===========================================================================

FUNCTION  emm_process_detach_accept

DESCRIPTION
  This function processes the DETACH ACCEPT message from network and completes
  the MO detach procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_detach_accept
(
  lte_nas_emm_mt_detach_accept_type *emm_detach_accept,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
)
{
  emm_security_context_info *context_ptr = NULL;

  ASSERT(emm_detach_accept != (lte_nas_emm_mt_detach_accept_type *)NULL) ;
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  MSG_HIGH_DS_0(MM_SUB, "=EMM= DETACH ACCEPT received");

#ifdef FEATURE_LTE_REL11
  if((((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
     (emm_ctrl_data_ptr->start_t3440_on_accept == TRUE))||
     ((emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL11) &&
     (emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) && 
     (mm_timer_status[TIMER_T3346]==TIMER_ACTIVE)))&& 
     (emm_ctrl_data_ptr->detach_reason==NORMAL_DETACH) && 
     (emm_ctrl_data_ptr->ongoing_detach_type==IMSI_DETACH_MO_DETACH))
  {
    mm_start_timer( TIMER_T3440, EMM_T3440_TIMER_DURATION ); 
    MSG_HIGH_DS_0(MM_SUB, "=EMM= T3440 timer started");
    if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
       (emm_ctrl_data_ptr->start_t3440_on_accept == TRUE))
    {
      emm_ctrl_data_ptr->start_t3440_on_accept = FALSE;
    }
  }
#else
#ifdef FEATURE_LTE_REL10
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) && 
     (mm_timer_status[TIMER_T3346]==TIMER_ACTIVE) && 
     (emm_ctrl_data_ptr->detach_reason==NORMAL_DETACH) && 
     (emm_ctrl_data_ptr->ongoing_detach_type==IMSI_DETACH_MO_DETACH))
  {
    mm_start_timer( TIMER_T3440, EMM_T3440_TIMER_DURATION ); 
  }
#endif 
#endif



  emm_remove_release_client(emm_mo_detach_release_ind);

  emm_complete_mo_detach(emm_ctrl_data_ptr);

  /* update the EPS NSC file in USIM with the native EPS context */
  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  if(context_ptr != NULL)
  {
    if(context_ptr->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS) 
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= based on eps_storage_supported supported, update context in USIM(1)/ EFS(0) %d",emm_ctrl_data_ptr->eps_storage_supported);
      if (emm_ctrl_data_ptr->eps_storage_supported)
      {
        emm_update_usim_eps_security_context(context_ptr);
      }
      else
      {
        if( emm_write_efs_security_context(context_ptr) == FALSE )
        {
          MSG_FATAL_DS(MM_SUB, "=EMM= Security context EFS write Failed ", 0,0,0 );
		}
      }
    }
  }

} /* end of emm_process_detach_accept() */

/*===========================================================================

FUNCTION  EMM_ABORT_MO_DETACH

DESCRIPTION
  This function aborts an ongoing MO detach procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_abort_mo_detach
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr,
  boolean              abort_connection
)
{
  ASSERT(emm_ctrl_data_ptr != NULL); 

  /* Stop timer 3421 */
  mm_stop_timer(TIMER_T3421);

  emm_ctrl_data_ptr->ongoing_detach_procedure = FALSE;
  emm_ctrl_data_ptr->emm_detach_attempt_counter = 0;

  emm_remove_release_client(emm_mo_detach_release_ind);

  if(abort_connection == TRUE)
  {
    /* Abort signaling connection if needed */
    emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
  }

  MSG_HIGH_DS_0(MM_SUB, "=EMM= MO detach procedure has been aborted");

} /* end of emm_abort_mo_detach() */

/*===========================================================================

FUNCTION  EMM_COMPLETE_MT_DETACH

DESCRIPTION
  This function sends MMR_SERVICE_IND to REG upon successful DETACH ACCPET
  transmission or max transmission attempt is reached for MT detach

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_complete_mt_detach
(
  dword trans_id
)
{
  mmr_cause_e_type       reg_cause = MMR_CAUSE_NONE;
  emm_ctrl_data_type    *emm_ctrl_data_ptr = NULL;
  lte_nas_emm_cause_type emm_cause;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);   

  emm_cause = emm_ctrl_data_ptr->mt_detach_info.emm_cause;

  emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
  if((emm_cause == LTE_NAS_EPS_SERVICES_NOT_ALLOWED ||
      emm_cause == LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN ||
      emm_ctrl_data_ptr->mt_detach_info.type == REATTACH_REQUIRED) &&
     (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                 SYS_SRV_DOMAIN_CS_PS))
  {
    /* spec 24.301 5.5.2.3.2 v9.2.0 A UE in CS/PS mode 1 or mode2
       of operation is still IMSI attached for non-EPS services */
    emm_ctrl_data_ptr->plmn_service_state->reg_service_domain =
                                                      SYS_SRV_DOMAIN_CS_ONLY;
    emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_CS_ONLY;
#ifdef FEATURE_LTE_REL11
     /*spec 24.301 5.5.2.3.2 A UE operating in CS/PS mode 1 or CS/PS mode 2 of operation 
       is still IMSI attached for non-EPS services in the network.
       The UE operating in CS/PS mode 1 or CS/PS mode 2 of operation
       shall set the update status to U2 NOT UPDATED*/
    if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
    {
      mm_location_information.location_update_status = NOT_UPDATED;
    }
#endif
  }
  else
  {
    emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                       SYS_SRV_DOMAIN_NO_SRV;
    emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;
  }

  if(emm_ctrl_data_ptr->mt_detach_info.type == REATTACH_NOT_REQUIURED)
  {
    /* Report to REG */
    switch(emm_ctrl_data_ptr->mt_detach_info.emm_cause)
    {
      case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
        reg_cause = MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED ;
        break ;
      case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
      case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
      case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
        emm_ctrl_data_ptr->plmn_service_state->plmn_forbidden = TRUE ;
        reg_cause = MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES ;
        break ;
      case LTE_NAS_PLMN_NOT_ALLOWED: 
        reg_cause = MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED ;
        emm_ctrl_data_ptr->plmn_service_state->plmn_forbidden = TRUE ;
        break ;
      case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
        reg_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN ;
        break ;
      case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
        reg_cause = MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION ;
        break ;
      case LTE_NAS_IMSI_UNKNOWN_IN_HSS:
      case LTE_NAS_ILLEGAL_UE:
      case LTE_NAS_ILLEGAL_ME:
        reg_cause = MMR_CAUSE_NOT_SPECIFIED;
        break;
      case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
#ifdef FEATURE_FEMTO_CSG
        if(emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id !=
                                                          SYS_CSG_ID_INVALID)
        {
          reg_cause = MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED;
        }
        else
        { 
        reg_cause = MMR_CAUSE_NOT_SPECIFIED ;
        }
#else
        reg_cause = MMR_CAUSE_NOT_SPECIFIED ;
#endif
        break ;
      default:
          reg_cause = MMR_CAUSE_NOT_SPECIFIED;
        break ;
    } 
  }
  else
  {
    reg_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
  }

  reg_service_report(reg_cause, FALSE, emm_ctrl_data_ptr);

  if(emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_CNF_PENDING)
  {
    emm_send_ps_detach_cnf(emm_ctrl_data_ptr);
  }

  /* Reset */
  emm_ctrl_data_ptr->mt_detach_info.type = INVALID_MT_DETACH_TYPE;
  emm_ctrl_data_ptr->mt_detach_info.emm_cause = LTE_NAS_NO_FAILURE;

}/* emm_complete_mt_detach */

/*===========================================================================

FUNCTION  emm_mt_detach_transmission_failure

DESCRIPTION
  This function handles the transmission failure of sending DETACH ACCEPT 
  message to network in MT detach procedure. (5.5.2.3.4 UE abnormal case a)
  - Resend DETACH ACCEPT message until the maximum attempt is reached. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_mt_detach_transmission_failure
(
  lte_emm_connection_cause_type    cause,
  dword                            trans_id,
  emm_failure_type                 emm_failure_cause,
  lte_nas_tai_lst1_type            tai,
  struct emm_pended_msg_list_tag  *message_ptr,
  emm_ctrl_data_type              *emm_ctrl_data_ptr
)
{
  emm_pended_msg_list_type   *msg_ptr = NULL;

  ASSERT(message_ptr != (emm_pended_msg_list_type *)NULL); 
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);

  msg_ptr = emm_find_pended_message(trans_id, emm_ctrl_data_ptr);

  if((msg_ptr != NULL) && 
     (msg_ptr->nas_out_message.nas_hdr.msg_id == DETACH_ACCEPT))
  {
    if((emm_ctrl_data_ptr->emm_detach_attempt_counter < 
                                          MAX_NUMBER_DETACH_ACCEPT_ATTEMPTS) &&
       (EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE))
    { 
      emm_ctrl_data_ptr->emm_detach_attempt_counter ++;
      /* Set indicator to FALSE to keep msg in list */
      msg_ptr->message_sent_indicator = FALSE;
      emm_send_rrc_data_request(&msg_ptr->nas_out_message, 
                                msg_ptr->sequence_number,
                                emm_ctrl_data_ptr);
    }
    else
    {
      MSG_HIGH_DS_2(MM_SUB, "=EMM= Complete MT DETACH - Failed to send DETACH ACCEPT, %d,%d",
                    emm_ctrl_data_ptr->emm_detach_attempt_counter,
                    EMM_GET_CONNECTION_STATE());

      emm_ctrl_data_ptr->emm_detach_attempt_counter = 0;
      /* Set indicator to TRUE to delete msg from list */
      msg_ptr->message_sent_indicator = TRUE;
      /* Report to REG */
      emm_complete_mt_detach(trans_id);
    }  
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Could not resend MO DETACH ACCEPT msg");
  }
  
  return FALSE ;
} /* emm_mt_detach_transmission_failure */

/*===========================================================================

FUNCTION  emm_process_mt_detach_abormal_case

DESCRIPTION
  This function processes the (5.5.2.3.4) UE abnormal case b in a MT detach 
  procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_mt_detach_abormal_case
(
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  boolean                set_t3402_to_default = TRUE;
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);

  if(emm_ctrl_data_ptr->t3402_value != EMM_DEFAULT_T3402)
  {
    set_t3402_to_default = emm_reevaluate_t3402_value(emm_ctrl_data_ptr);
  }
  else
  {
    /* If T3402 is already set to default value then no need to send
       NAS_EMM_T3402_CHANGED_IND */
    set_t3402_to_default = FALSE;
  }

  if(set_t3402_to_default == TRUE)
  {
    emm_ctrl_data_ptr->t3402_value = EMM_DEFAULT_T3402;
    emm_send_t3402_changed_ind(emm_ctrl_data_ptr->t3402_value);
  }
  mm_start_timer(TIMER_T3402,emm_ctrl_data_ptr->t3402_value);
  MSG_HIGH_DS_0(MM_SUB, "=EMM= T3402 started - MT deatch UE abnormal case b");
  emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED,  
                               SYS_SIM_STATE_AVAILABLE, 
                               emm_ctrl_data_ptr, TRUE);

  emm_handle_temporary_forbidden_plmn(emm_ctrl_data_ptr);

  /* MT detach abnormal case b */
  if(emm_ctrl_data_ptr->aggression_management == TRUE) 
  {
    emm_add_plmn_to_backoff_fplmn_list(emm_ctrl_data_ptr->plmn_service_state->plmn, 
                                        emm_ctrl_data_ptr->t3402_value, SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE);
  }

  EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH);

} /* emm_process_mt_detach_abormal_case */

/*===========================================================================

FUNCTION  EMM_MT_DETACH_INVALIDATE_MOBILITY

DESCRIPTION
  This function invalidates EMM mobility information depending on given emm
  cause upon reattach not required MT detach

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_mt_detach_invalidate_mobility
(
  lte_nas_emm_cause_type   emm_cause,
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  switch (emm_cause)
  {
    case LTE_NAS_ILLEGAL_UE:
    case LTE_NAS_ILLEGAL_ME:
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
      emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                   SYS_SIM_STATE_CS_PS_INVALID, 
                                   emm_ctrl_data_ptr, TRUE) ;
      break ;
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
      emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                   SYS_SIM_STATE_PS_INVALID, 
                                   emm_ctrl_data_ptr, TRUE) ;
      break ;
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
      emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                   SYS_SIM_STATE_AVAILABLE, 
                                   emm_ctrl_data_ptr, TRUE) ;
      break ;
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
      emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                   SYS_SIM_STATE_AVAILABLE, 
                                   emm_ctrl_data_ptr, FALSE) ;
      break ;
    case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
      /* Update EMM USIM with EPS update status of ROAMING_NOT_ALLOWED */
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_ROAMING_NOT_ALLOWED;
      emm_save_mobility_info(emm_ctrl_data_ptr) ;
      break;
    case LTE_NAS_IMSI_UNKNOWN_IN_HSS:
      /*The MS shall set the update status to U3 ROAMING NOT ALLOWED and
      shall delete any TMSI, LAI and ciphering key sequence number.
      The SIM shall be considered as invalid for non-GPRS services
      until switching off or the SIM is removed.*/
      mm_update_cs_reject_invalid_imsi_status();

      if((emm_ctrl_data_ptr->gw_reject_update == FALSE) && 
         (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                         SYS_SRV_DOMAIN_CS_PS))
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= setting reg_service_domain to SYS_SRV_DOMAIN_PS_ONLY");
        emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                        SYS_SRV_DOMAIN_PS_ONLY;
        emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_PS_ONLY;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
      }
      break;
    default:
      break;
  }

}/* emm_mt_detach_invalidate_mobility */

/*===========================================================================

FUNCTION  emm_process_mt_detach_request_emm_cause

DESCRIPTION
  This function processes the EMM cause of reattach not required type of MT 
  detach procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_mt_detach_request_emm_cause
(
  lte_nas_emm_cause_type   emm_cause,
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type  current_lte_plmn;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  /* Always update GMM/MM before EMM processing to allow GMM/MM writes sim */
  mm_update_detach_req_status((byte)emm_cause);

  switch (emm_cause)
  {
    case LTE_NAS_IMSI_UNKNOWN_IN_HSS:
    case LTE_NAS_ILLEGAL_UE:
    case LTE_NAS_ILLEGAL_ME:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED: 
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
    case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
      emm_print_emm_cause( emm_cause);
      break ;
    default:
      /* 5.5.2.3.4 Abnormal case b */
       MSG_HIGH_DS_0(MM_SUB, "=EMM= Abnormal Cause in MT Detach");  
      emm_process_mt_detach_abormal_case(emm_ctrl_data_ptr);
      return;
  }

  /* Mobility info and EPS status */
  emm_mt_detach_invalidate_mobility(emm_cause, emm_ctrl_data_ptr);

  /* Timer T3440 */
  switch (emm_cause)
  {
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
      mm_start_timer(TIMER_T3440,EMM_T3440_TIMER_DURATION);
      break ;
    default:
      /* Do nothing here for other emm causes */
      break;
  }

  /* Reset Attach Attempt Counter */
  switch (emm_cause)
  {
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
    case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Reset Attach attempt counter in detach Procedure");
      emm_ctrl_data_ptr->emm_attach_attempt_counter = 0 ;
      break ;
    default:
      /* Do nothing here for other emm causes */
      break;
  } 

  /* Delete the list of equivalent PLMNs */ 
  switch (emm_cause)
  {
    case LTE_NAS_ILLEGAL_UE:
    case LTE_NAS_ILLEGAL_ME:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Deleting equivalent PLMNs list - MT DETACH");
      emm_ctrl_data_ptr->update_eplmn_list = TRUE;
      emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
      break;
#ifdef FEATURE_LTE_REL11
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Deleting equivalent PLMNs list - MT DETACH");
        emm_ctrl_data_ptr->update_eplmn_list = TRUE;
        emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
      }
      break;
#endif
    default:
      /* Do nothing here for other emm causes */
      break;
  }

  /* Add current PLMN into forbidden PLMN list */
  switch (emm_cause)
  {
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
      /* Create current PLMN ID based on latest RRC reported PLMN ID */
      current_lte_plmn =  emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
      if(emm_cause == LTE_NAS_PLMN_NOT_ALLOWED)
      {
        /* Add current PLMN into forbidden PLMN list */
        mm_add_plmn_to_fplmn_list(current_lte_plmn);
      }
      else
      {
        /* Add current PLMN into forbidden PLMNs for GPRS list */
        mm_add_plmn_to_gprs_fplmn_list(current_lte_plmn);
      }
      break;
    default:
      /* Do nothing here for other emm causes */
      break;
  }

  /* Add current TAI to forbidden TAI lists */
  switch (emm_cause)
  {
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Add TAI to list of forbidden TAIs for service");

      /* Add to frobidden tracking area list */
      emm_add_forbidden_tai(emm_ctrl_data_ptr->forbidden_for_service_list_ptr,
                            emm_get_camped_tai(emm_ctrl_data_ptr),
                            emm_ctrl_data_ptr) ;
      break ;
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Add TAI to FTAI list for roaming - Detach Req");
      emm_add_forbidden_tai(emm_ctrl_data_ptr->rrc_forbidden_list_ptr,
                            emm_get_camped_tai(emm_ctrl_data_ptr),
                             emm_ctrl_data_ptr);
      emm_add_manual_ftai(emm_ctrl_data_ptr);
      break;
    default:
      /* Do nothing here for other emm causes */
      break;
  } /* forbidden area list switch() */

  /* EMM state change */
  switch (emm_cause)
  {
    case LTE_NAS_ILLEGAL_UE:
    case LTE_NAS_ILLEGAL_ME:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH) ;
      emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);
      break ;
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
      emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);
      break;
    case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
#ifdef FEATURE_FEMTO_CSG
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE);
      if(emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id !=
		                                              SYS_CSG_ID_INVALID)
      {
        mm_delete_csg_from_white_list
        (
          emm_ctrl_data_ptr->plmn_service_state->plmn,
          &(emm_ctrl_data_ptr->plmn_service_state->csg_info)
        );
      }
#else
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
#endif
      emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);
      break ;
    default:
      break ;
  }

  emm_send_reg_rej_ind(emm_cause, emm_ctrl_data_ptr);

} /* emm_process_mt_detach_request_emm_cause */


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================

FUNCTION  emm_start_mt_detach

DESCRIPTION
  This function handles the MT detach proedure which has following 3 types,
  - Re-attach required
  - Re-attach not required
  - IMSI detach

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_start_mt_detach
(
  lte_nas_emm_mt_detach_req_type     *mt_detach_request,
  emm_ctrl_data_type                 *emm_ctrl_data_ptr
)
{
  lte_nas_emm_mo_detach_accept_type   *mo_detach_accept;
  dword                                sequence_number = 0xFFFFFFFF;
  emm_failure_type                     emm_failure_cause;
  emm_security_context_info *context_ptr = NULL;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  switch(EMM_GET_STATE())
  {
    /* 5.5.1.2.6 UE abnormal case g */
    case EMM_REGISTERED_INITIATED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Received Detach Request in EMM_REGISTERED_INITIATED state");
      if(mt_detach_request->detach_req == REATTACH_NOT_REQUIURED
#ifdef FEATURE_LTE_REL11
         /*spec 24.301 5.5.1.2.6 abnormal case g)*/
           &&(( emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL11)||
              (( mt_detach_request->emm_cause_exists == FALSE )||
                (mt_detach_request->emm_cause != LTE_NAS_IMSI_UNKNOWN_IN_HSS)))
#endif
        )
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Aborting attach");
        mm_stop_timer(TIMER_T3410);
        emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);
        emm_remove_release_client(emm_attach_release_indication);
        break;
      }
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Detach Request discarded");
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
        return;
      }
    /* 5.5.3.2.6 UE abnormal case f */
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
#ifdef FEATURE_LTE_REL10
     /* R10 Spec 24.301 CR #0936. TAU and Detach procedure collision */
     if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
     {
       if(((mt_detach_request->detach_req == REATTACH_NOT_REQUIURED) &&
           (mt_detach_request->emm_cause == LTE_NAS_IMSI_UNKNOWN_IN_HSS)) ||
          (mt_detach_request->detach_req == IMSI_DETACH_MT_DETACH))
       {
         MSG_HIGH_DS_1(MM_SUB, "=EMM= Detach Request discarded in EMM STATE %d",
                       EMM_TRACKING_AREA_UPDATING_INITIATED);
         return;
       }
     }
#endif
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Rcved Detach Request - Aborting TAU");
      mm_stop_timer(TIMER_T3430);
      emm_remove_release_client(emm_tau_request_release_indication); 
      break;
    case EMM_SERVICE_REQUEST_INITIATED:
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      if(emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Rcved Detach Request - Aborting ESR (Extended Service Request)");
        if(mt_detach_request->detach_req == REATTACH_REQUIRED)
        {
          emm_abort_service_request(emm_ctrl_data_ptr, FALSE, TRUE,
                                    LTE_NAS_DETACH_WITH_REATTACH_LTE_NW_DETACH,
                                    emm_failure_cause);
        }
        else if(mt_detach_request->detach_req == REATTACH_NOT_REQUIURED)
        {
#ifdef FEATURE_LTE_REL10
          /* R10 Spec 24.301 CR #0936. Service request and Detach procedure collision */
          if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
             (mt_detach_request->emm_cause != LTE_NAS_IMSI_UNKNOWN_IN_HSS))
#endif
          {
            emm_abort_service_request(emm_ctrl_data_ptr, FALSE, TRUE,
                                      LTE_NAS_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH,
                                      emm_failure_cause);
          }
        }

        else
        {
#ifdef FEATURE_LTE_REL10
          /* R10 Spec 24.301 CR #0936. Service request and Detach procedure collision */
          if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
             (mt_detach_request->detach_req != IMSI_DETACH_MT_DETACH))
#endif
          {
            emm_abort_service_request(emm_ctrl_data_ptr, FALSE, TRUE,
                                      LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                      emm_failure_cause);
          }
        }
      }
      else
      {
#ifdef FEATURE_LTE_REL10
        /* R10 Spec 24.301 CR #0936. Service request and Detach procedure collision */
        if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
           (((mt_detach_request->detach_req == REATTACH_NOT_REQUIURED) &&
             (mt_detach_request->emm_cause != LTE_NAS_IMSI_UNKNOWN_IN_HSS)) ||
             (mt_detach_request->detach_req == REATTACH_REQUIRED)))
#endif
        {  
          emm_abort_service_request(emm_ctrl_data_ptr, FALSE, TRUE,
                                    LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                    emm_failure_cause);
        }
      }
      /* Clean up pending messages */
      emm_process_unsent_messages(CONN_CNF_LTE_RRC_CONN_EST_FAILURE,
                                  emm_failure_cause, 
                                  emm_ctrl_data_ptr);
      /* Process all pending irat uplink messages */
      emm_process_pending_irat_msg(LTE_NAS_IRAT_INVALID_STATE,
                                   LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                   LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                                   emm_ctrl_data_ptr);
      break;
    /* 5.5.2.2.3 UE abnormal case d */
    case EMM_DEREGISTERED_INITIATED:
#ifdef FEATURE_LTE_REL11
        /*Spec 24.301 5.2.2.4 abnormal case d)*/
        if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
           (mt_detach_request->detach_req == IMSI_DETACH_MT_DETACH))
       {
           MSG_HIGH_DS_2(MM_SUB, "=EMM= Proceed with MO detache %d and MT detach %d ",
                         emm_ctrl_data_ptr->ongoing_detach_type,
                         IMSI_DETACH_MT_DETACH);
       }
       else
#endif
       {
      emm_abort_mo_detach(emm_ctrl_data_ptr, FALSE);
       }
      break;
    default:
      break;
  }

  /* 5.3.1.2 Stop timer T3440 */
  mm_stop_timer(TIMER_T3440);

  emm_ctrl_data_ptr->emm_detach_attempt_counter = 0;

  switch(mt_detach_request->detach_req)
  {
    case REATTACH_REQUIRED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Received Detach Request - REATTACH_REQUIRED");

#ifdef FEATURE_LTE_REL10
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
      {
        emm_send_unblock_all_apns_ind();
      }
#endif
      break;
    case REATTACH_NOT_REQUIURED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Received Detach Request - REATTACH_NOT_REQUIURED");
      if(mt_detach_request->emm_cause_exists == TRUE)
      {
        if(mt_detach_request->emm_cause == LTE_NAS_IMSI_UNKNOWN_IN_HSS)
        {
          emm_process_mt_detach_request_emm_cause(mt_detach_request->emm_cause, 
                                                  emm_ctrl_data_ptr);
          return;
        }
      }
      break;
    case IMSI_DETACH_MT_DETACH:
        MSG_HIGH_DS_0(MM_SUB, "=EMM=Received Detach Request - IMSI_DETACH_MT_DETACH");
      /* 5.5.2.3.2 */
      mm_invalidate_lu_info(NOT_UPDATED);
#ifdef FEATURE_LTE_REL11
        /*Spec 24.301 5.2.2.4 abnormal case d)*/
        if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
           ((emm_ctrl_data_ptr->ongoing_detach_procedure == TRUE) &&
           (emm_ctrl_data_ptr->ongoing_detach_type != INVALID_DETACH_TYPE)&&
           (mt_detach_request->detach_req == IMSI_DETACH_MT_DETACH)))
        {
           MSG_HIGH_DS_2(MM_SUB, "=EMM= Skip re-attach to non-EPS sevicies for mt detach type %d collision with mo detach type %d ",
                         IMSI_DETACH_MT_DETACH,
                         emm_ctrl_data_ptr->ongoing_detach_type);
        }
        else
#endif  
        {
		  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_PS_ONLY;
          emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_PS_ONLY;  
		  emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
		  emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
          emm_initiate_tau_procedure(EMM_INITIAL_UPDATE, 
                                 COMBINED_TA_LA_UPDATING_IMSI_ATTACH, 
                                 FALSE, 
                                 emm_ctrl_data_ptr);
        }
      return;
    default:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Ignore detach attach msg - invalid detach type.");
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      return;
  }

  if ((mt_detach_request->detach_req == REATTACH_REQUIRED) && 
      (mt_detach_request->emm_cause_exists == TRUE) &&
      (mt_detach_request->emm_cause == LTE_NAS_EPS_SERVICES_NOT_ALLOWED))
  {
     emm_build_and_send_detach_ind(LTE_NAS_EPS_SERVICES_NOT_ALLOWED_LOCAL);      
  }
  else
  {
     /* 5.5.2.3.2 Deactivate the EPS bearer context(s) locally */
     emm_build_and_send_detach_ind(EMM_DETACHED);
  }
  

  /* 5.5.2.3.2 Send DETACH ACCEPT message to the network */
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Send Detach Accept msg to NW");
  memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
         0x0,
         sizeof(lte_nas_outgoing_msg_type)) ;

  mo_detach_accept                     = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.mo_detach_accept;
  mo_detach_accept->lte_nas_hdr.pd     = EPS_MOBILITY_MANAGEMENT_MESSAGES ;
  mo_detach_accept->lte_nas_hdr.msg_id = DETACH_ACCEPT ;
  mo_detach_accept->security_hdr = PLAIN_NAS_MSG ;
  /* Obtain sequence number */
  sequence_number = emm_get_nas_transaction_id() ;

  emm_ctrl_data_ptr->mt_detach_info.type = mt_detach_request->detach_req;
  emm_send_diag_event(EVENT_NAS_MT_DETACH, (nas_event_end_result)PROC_START, (nas_event_end_reason)PROC_END_NO_REASON, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
  if(mt_detach_request->emm_cause_exists == TRUE)
  {
    emm_ctrl_data_ptr->mt_detach_info.emm_cause = mt_detach_request->emm_cause;
  }
  else
  {
    emm_ctrl_data_ptr->mt_detach_info.emm_cause = LTE_NAS_NO_FAILURE;
  }
      
  if(FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number, 
                                MSGR_NAS_EMM,
                                emm_complete_mt_detach, 
                                emm_mt_detach_transmission_failure, 
                                emm_ctrl_data_ptr))
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Failed to send Detach Accept msg to NW");
  }

  switch(mt_detach_request->detach_req)
  {
    case REATTACH_REQUIRED:
      if(mt_detach_request->emm_cause_exists == TRUE)
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM=Ignore EMM cause %u in REATTACH_REQUIRED Detach Request",
                      mt_detach_request->emm_cause);
      }

      if(emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_CNF_PENDING)
      {
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);
      }
      else
      {
        /* 5.5.2.3.2 EMM enters state EMM_DEREGISTERED */
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
      }

      /* Update GMM state if necessary */
      gmm_multimode_set_state(GMM_DEREGISTERED, GMM_NO_CELL_AVAILABLE);
      /*I guess EMM should not send LIMITED_SEVICE event to GSTK here, as this is a REATTACH case*/

      /* section 7.2.5.1 of TS 33.401 */
      /* delete any mapped security context resulting from IRAT HO/ Idle mobility */
      /* update the EPS NSC file in USIM with the native EPS context */
      
      context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

      if( context_ptr != NULL )
      {
        if( context_ptr->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS) 
        {  
          MSG_HIGH_DS_1(MM_SUB, "=EMM= based on eps_storage_supported supported, update context in USIM(1)/ EFS(0) %d",emm_ctrl_data_ptr->eps_storage_supported);
          if (emm_ctrl_data_ptr->eps_storage_supported)
          {
            emm_update_usim_eps_security_context(context_ptr);
          }
          else
          {
            if( emm_write_efs_security_context(context_ptr) == FALSE )
            {
              MSG_FATAL_DS(MM_SUB, "=EMM= Security context EFS write Failed ", 0,0,0 );
			}
          }
        }

      }
      break;
    case REATTACH_NOT_REQUIURED:
      if(mt_detach_request->emm_cause_exists == TRUE)
      {
        /* 5.5.2.3.2 Process EMM cause */
        emm_process_mt_detach_request_emm_cause(mt_detach_request->emm_cause, 
                                                emm_ctrl_data_ptr);
      }
      else
      {
        /* Update GMM/MM if necessary */
        mm_update_detach_req_status((byte)LTE_NAS_NO_FAILURE);
        /* 5.5.2.3.4 Abnormal case b */
        emm_process_mt_detach_abormal_case(emm_ctrl_data_ptr);
        return;
      }
      break;
    default:
      /* Do nothing here */
      break;
  }

  emm_send_rrc_system_update(emm_ctrl_data_ptr);

  emm_write_tin_to_efs(emm_ctrl_data_ptr);
  if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE)
  {
  emm_write_lte_accepted_status_to_efs(emm_ctrl_data_ptr);
  }
  if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming== TRUE) 
  {
    emm_write_lte_accepted_vplmn_status_to_efs(emm_ctrl_data_ptr);
  }


} /* emm_start_mt_detach */

/*===========================================================================

FUNCTION    EMM_TAU_REQUEST_SUCCEEDED

DESCRIPTION
  This function is called when RRC indicates that TAU Request message has been
  successfully reseived by the network

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function changes value of global flag eps_bearer_context_status_updated
  to FALSE
===========================================================================*/
void emm_tau_request_succeeded
(
  dword trans_id
)
{
  emm_ctrl_data_type  *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  emm_ctrl_data_ptr->eps_bearer_context_status_updated = FALSE ;
  if(emm_ctrl_data_ptr->w2l_psho == TRUE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU Request sent successfully after W2L PS HO , establish nas_secure_exchange ");
    emm_ctrl_data_ptr->nas_secure_exchange = TRUE;
    emm_ctrl_data_ptr->w2l_psho = FALSE;
  }

} /* end of emm_tau_request_succeeded() */

/*===========================================================================

FUNCTION    EMM_START_TAU_RECOVERY_PROCEDURE

DESCRIPTION
  This function is called in order to process TAU procedure abnormal cases in
  UE according to TS 24.301 5.5.3.2.6

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function modifies TAU related vars which are allocated on 
  emm_ctrl_data_ptr:
    o emm_update_status,
    o emm_equivalent_PLMN_list
    o emm_tau_attempt_counter
===========================================================================*/
void emm_start_tau_recovery_procedure
(
  /* EMM control data */
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type current_tai;
  mmr_cause_e_type    mmr_status;
  boolean set_t3402_to_default = TRUE;
#ifdef FEATURE_LTE_REL10
  boolean ext_wait_time_processed = FALSE;
#endif

  emm_state_type    emm_state    = EMM_INVALID_STATE ;
  emm_substate_type emm_substate = EMM_UNKNOWN_SUBSTATE ;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->emm_tai_list_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  emm_state    = EMM_GET_STATE() ;
  emm_substate = EMM_GET_SUBSTATE() ;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Start TAU recovery");

  emm_set_tau_trm_priority(FALSE);

  /*
    EMM needs to recover TAI info from latest RRC Service or 
    Connected Mode Cell Change indications
  */
  current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->
                                                                                 camped_svc_info.selected_plmn) ;
  current_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac ;

  /*
  ** On failure of TAU procedure which was to update radio capabilities to network, set update status 
  ** to not updated so that service request procedure is not allowed.
  */
  if((emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED) &&
     (emm_ctrl_data_ptr->radio_cap_update != EMM_RADIO_CAP_UPDATE_NONE))
  {
    emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Set emm update status to not updated radio_cap_update %d",
                  emm_ctrl_data_ptr->radio_cap_update);
  }

#ifdef FEATURE_LTE_REL10
  /* 5.5.3.2.6: abnormal case k */
  ext_wait_time_processed = emm_handle_extended_wait_time(emm_ctrl_data_ptr);
#endif

#ifdef FEATURE_LTE_REL9 
#ifdef FEATURE_LTE_REL10
  if(ext_wait_time_processed == TRUE)
  {
    emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Ext wait time processed. Resetting TAU attempt counter");
  }
  else
#endif 
  if(((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_PENDING) &&
      (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == FALSE))||
      (emm_skip_emc_retry() == FALSE))
  {
#endif
    /* Increment TAU attempt counter */
    emm_ctrl_data_ptr->emm_tau_attempt_counter++;
#ifdef FEATURE_LTE_REL9 
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Not updating attempt counter - EMC SRV pending");
  }
#endif

  /* TS 24.301 5.5.3.2.6 */
  if (emm_ctrl_data_ptr->emm_tau_attempt_counter < MAX_NUMBER_TAU_OF_ATTEMPTS)
  {
    if((emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)
#ifdef FEATURE_LTE_REL10
       && 
       (ext_wait_time_processed == FALSE)
#endif
      )
    {
#ifdef FEATURE_LTE_REL11
       /*if the TAU attempt counter is less than 5,the UE shall 
        **set the update status to U2 NOT UPDATED but shall not
        **delete any LAI, TMSI, ciphering key sequence number and list of 
        ** equivalent PLMNs
        */
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
      {
        mm_location_information.location_update_status = NOT_UPDATED;
      }
#else
      if(emm_ctrl_data_ptr->emm_location_area->location_update_status 
                                                                    != UPDATED)
      {
        /* 
        ** if the TAU attempt counter is less than 5 and, additionally, 
        ** the update status is different from U1 UPDATED, then the UE shall 
        ** delete any LAI, TMSI, ciphering key sequence number and list of 
        ** equivalent PLMNs and set the update status to U2 NOT UPDATED
        */
        mm_invalidate_lu_info( NOT_UPDATED ) ;

        MSG_HIGH_DS_0(MM_SUB, "=EMM= Delete list of equivalent PLMNs");
        emm_ctrl_data_ptr->update_eplmn_list = TRUE;
        emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
      }
#endif
    }

#ifdef FEATURE_LTE_REL10
    if(ext_wait_time_processed == TRUE)
    {
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
    }
#endif

    if ((emm_state == EMM_REGISTERED) && 
        (emm_substate == (emm_substate_type)EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM) &&
        (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
        (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_CS_PS))
    {
      emm_ctrl_data_ptr->plmn_service_state->cs_service_status = 
                                               SYS_SRV_STATUS_LIMITED_REGIONAL;

      /* Start T3411 to repeat Combined TAU when T3411 expires */
      mm_stop_timer(TIMER_T3411) ;
      mm_start_timer(TIMER_T3411, EMM_T3411_TIMER_DURATION) ;
      MSG_HIGH_DS_0(MM_SUB, "=EMM= T3411 started - for Combined TAU recovery");
    }
    else
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM=Current EPS update status %d",
                    emm_ctrl_data_ptr->emm_update_status);

      /* Check newly reported TAI and EPS update status per TS 24.301 5.5.3.2.6*/
      if((emm_search_tai_list(&current_tai,emm_ctrl_data_ptr->emm_tai_list_ptr)) &&
         (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED)
         && (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_P_TMSI))
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Reported TAI IS belonging to the TAI list");
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);       
        if((emm_is_combined_reg_allowed(emm_ctrl_data_ptr) == TRUE) &&
           emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY &&
           emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_CS_PS)
        {
          emm_ctrl_data_ptr->plmn_service_state->cs_service_status = 
                                               SYS_SRV_STATUS_LIMITED_REGIONAL;
        }
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                               SYS_SRV_STATUS_SRV;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
      }
      else
      {
        emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
        emm_save_mobility_info(emm_ctrl_data_ptr);
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Reported TAI is NOT belonging to the TAI list");
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE);

#ifdef FEATURE_LTE_REL10
        if(ext_wait_time_processed == TRUE)
        {
          emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
        }
        else
#endif
        {
          emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;
          if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,
                                             SYS_RAT_UMTS_RADIO_ACCESS)||
             mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,
                                             SYS_RAT_GSM_RADIO_ACCESS)||
             mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_TDS_RADIO_ACCESS))
          {
            mm_set_update_status_tau_abnormal_failure(GMM_GU2_NOT_UPDATED);
          }
        }
        /* Suspend data */
        if(emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend == FALSE)
        {
          emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend = TRUE;
          mm_send_cm_ps_data_suspend_ind();
        }
      }

#ifdef FEATURE_LTE_REL9
      if((((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_PENDING) &&
           (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == FALSE)) 
          ||
          (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED)
          ||
          (emm_skip_emc_retry() == FALSE))
#ifdef FEATURE_LTE_REL10
        && 
        (ext_wait_time_processed == FALSE)
#endif
        )
      {
#endif
        /* Start T3411 */
        mm_stop_timer(TIMER_T3411) ;
        mm_start_timer(TIMER_T3411, EMM_T3411_TIMER_DURATION) ;
        MSG_HIGH_DS_0(MM_SUB, "=EMM= T3411 started - TAU recovery");
#ifdef FEATURE_LTE_REL9 
      }
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Not starting T3411 - EMC SRV pending or Ext wait time received");
#ifdef FEATURE_LTE_REL10
        if(ext_wait_time_processed == FALSE)
#endif 
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                          SYS_SRV_STATUS_LIMITED;
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_LIMITED_SERVICE);
          reg_service_report(MMR_CAUSE_ABORT_PLMN_SELECTION, FALSE, emm_ctrl_data_ptr);
        }
      }
#endif
    }
  }
  else
  {
#ifdef FEATURE_LTE_REL10
    /* This flag could be set if TAU is triggered due to paging.
       reset the flag */
    emm_ctrl_data_ptr->responding_to_page = FALSE;
#endif
    /* Reset TAU attempt counter */
    emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Reset TAU attempt counter to %d",
                  emm_ctrl_data_ptr->emm_tau_attempt_counter);
    mmr_status = MMR_CAUSE_FAILURE_MAX_ATTEMPTED;

    if(emm_ctrl_data_ptr->t3402_value != EMM_DEFAULT_T3402)
    {
      set_t3402_to_default = emm_reevaluate_t3402_value(emm_ctrl_data_ptr);
    }
    else
    {
      /* If T3402 is already set to default value then no need to send
         NAS_EMM_T3402_CHANGED_IND */
      set_t3402_to_default = FALSE;
    }

    if(set_t3402_to_default == TRUE) 
    {
      emm_ctrl_data_ptr->t3402_value = EMM_DEFAULT_T3402;
      emm_send_t3402_changed_ind(emm_ctrl_data_ptr->t3402_value);
    }

    if ((emm_state == EMM_REGISTERED) && 
        (emm_substate == (emm_substate_type)EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM) &&
        (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
        (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_CS_PS))
    {
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_UPDATED;
      emm_save_mobility_info(emm_ctrl_data_ptr);

      /* Start T3402 */
      mm_start_timer(TIMER_T3402,emm_ctrl_data_ptr->t3402_value) ;

      emm_ctrl_data_ptr->plmn_service_state->cs_service_status = 
                                                         SYS_SRV_STATUS_NO_SRV;

      MSG_HIGH_DS_0(MM_SUB, "=EMM= T3402 started for CS registration");
    }
    else
    {
      /* Invalidate LU info if combined registration is performed */
      if((emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain  == 
                                                          SYS_SRV_DOMAIN_CS_PS)
         &&
         (emm_is_combined_reg_allowed(emm_ctrl_data_ptr) == TRUE))
      {
        mm_invalidate_lu_info(NOT_UPDATED);
      }

      /* Invalidate equivalent PLMN list */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Delete list of equivalent PLMNs");
      emm_ctrl_data_ptr->update_eplmn_list = TRUE;
      emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;

      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
      emm_save_mobility_info(emm_ctrl_data_ptr);
      if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,
                                         SYS_RAT_UMTS_RADIO_ACCESS)||
         mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,
                                         SYS_RAT_GSM_RADIO_ACCESS)||
         mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                         SYS_RAT_TDS_RADIO_ACCESS))
      {
        mm_set_update_status_tau_abnormal_failure(GMM_GU2_NOT_UPDATED);
      }

      /* Start T3402 */
      mm_stop_timer(TIMER_T3402) ;
      mm_start_timer(TIMER_T3402,emm_ctrl_data_ptr->t3402_value) ;
      if(emm_skip_emc_retry() == TRUE )
      {
        emm_handle_temporary_forbidden_plmn(emm_ctrl_data_ptr);
      }
      MSG_HIGH_DS_0(MM_SUB, "=EMM= T3402 has been started");


      /* Inform RRC that equivalent PLMN list is no longer valid */
      emm_send_rrc_system_update(emm_ctrl_data_ptr) ;

      /*Indicate to upper layer that there is LIMITED_SERVICE*/
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;


        /* Add PLMN to temp forbidden if 
            i) device is voice centric and max attempt fails for combined registration
            ii) Aggression management is enabled*/
      if(((emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.is_voice_domain_ie_valid
                                                                                   == TRUE)&&
           (emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.voice_or_data_centric 
                                                             == SYS_UE_USAGE_SETTING_VOICE_CENTRIC) &&
         ((emm_ctrl_data_ptr->emm_ongoing_eps_ta_update ==  COMBINED_TA_LA_UPDATING) ||
         (emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == COMBINED_TA_LA_UPDATING_IMSI_ATTACH)||
         ((emm_ctrl_data_ptr->emm_ongoing_eps_ta_update==PERIODIC_UPDATING) &&
          (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS) &&
          (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_CS_PS)))) ||
         (emm_ctrl_data_ptr->aggression_management == TRUE))
        {
          if(emm_ctrl_data_ptr->lte_disable_duration_timer_nv != 0)
          {
            emm_add_plmn_to_backoff_fplmn_list(emm_ctrl_data_ptr->plmn_service_state->plmn,
                      emm_ctrl_data_ptr->lte_disable_duration_timer_nv,SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE);
          }
          else
          {
            emm_add_plmn_to_backoff_fplmn_list(emm_ctrl_data_ptr->plmn_service_state->plmn,
                      emm_ctrl_data_ptr->t3402_value,SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE);
          }
        }

      emm_ctrl_data_ptr->manual_user_sel = FALSE;

      /* 
        Suspend user data by sending service report (MMR_REG_CNF or 
        MMR_SERVICE_IND) to REG and setting ps_data_suspend == TRUE 
      */      
      emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend = TRUE ;
      reg_service_report(mmr_status, 
                         FALSE,
                         emm_ctrl_data_ptr);

      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE);
    }
  }
} /* end of emm_start_tau_recovery_procedure() */




/*===========================================================================

FUNCTION    EMM_TAU_REQUEST_FAILED

DESCRIPTION
  This function is called when RRC indicates that TAU Request message 
  delivery has failed.

DEPENDENCIES
  This is a call back function which should not be called by name

RETURN VALUE
  TRUE  - indicates that TAU procedure is required before another EMM 
          or ESM procedures may be resumed
  FALSE - indicates that TAU is not required

SIDE EFFECTS
  This function modifies value of the corresponding boolean flag 
  message_sent_indicator:
    TRUE  - current message CAN be deleted from the cache memory
    FALSE - current message CANNOT be deleted from the cache memory
===========================================================================*/
boolean emm_tau_request_failed
(
  /* RRC failure cause */
  lte_emm_connection_cause_type   cause,

  /* Message ID that this callback is assosiated with */
  dword                           trans_id,

  /* Failure cause type and cause */
  emm_failure_type                emm_failure_cause,

  /* The following TAI is valid only with 
     TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO */
  lte_nas_tai_lst1_type           tai,

  /* Pointer to the massage which failed to be tx */
  struct emm_pended_msg_list_tag *message_ptr,

  /* EMM control data */
  emm_ctrl_data_type             *emm_ctrl_data_ptr
) 
{
  lte_nas_local_cause_enum_T local_cause;
  emm_csfb_failure_cause_type csfb_failure_cause;
  emm_pended_msg_list_type *pended_message_ptr = (struct emm_pended_msg_list_tag *)message_ptr ;
#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_1XSRLTE)
  lte_nas_tai_lst1_type current_tai;
#endif 
  ASSERT(pended_message_ptr != (emm_pended_msg_list_type *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_LOWER_LAYER_FAILURE, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);


#ifdef FEATURE_DUAL_SIM
  current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->
                                                                                 camped_svc_info.selected_plmn) ;
  current_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac ;
#endif 
  /* Set this flaf to FALSE unconditionally incase there was a W2L HO and TAU failed. */
  emm_ctrl_data_ptr->w2l_psho = FALSE;

  /* Putting this code outside as I am not sure why we are checking for EMM_TRACKING_AREA_UPDATING_INITIATED in this function*/
  if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED) || (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED))
  {
    local_cause = emm_convert_rrc_cause_to_local_cause(cause,emm_ctrl_data_ptr);
    if( (cause != TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO) && (cause != TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN) )
    {
      /* CSFB was buffered progress*/
      //failure_cause_type.failure_type = EMM_REJECT_CAUSE;
      csfb_failure_cause.failure_cause.lte_nas_failure_cause = local_cause;
      emm_process_extended_service_req_failure(csfb_failure_cause,emm_ctrl_data_ptr);
    }
  }

  if(EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED)
  {
    emm_remove_release_client(emm_tau_request_release_indication);

    /* Start TAU recovery based on the RRC cause */
    switch (cause)
    {
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN:
        emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED ;
        emm_save_mobility_info(emm_ctrl_data_ptr);
       /* Abort ongoing TAU attempt by re-starting TAU procedure */
        emm_start_tau_procedure(EMM_INITIAL_UPDATE, 
                                message_ptr->nas_out_message.emm_outgoing_msg.tau_req.eps_update.eps_update_val,
                                message_ptr->nas_out_message.emm_outgoing_msg.tau_req.eps_update.active_flag,
                                emm_ctrl_data_ptr) ;

        break ;
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_RLF:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ABORTED:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CELL_RESEL:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_SI_FAILURE:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TAI_CHANGED:
        mm_stop_timer(TIMER_T3430);

        if(emm_ctrl_data_ptr->reg_req_pending_ptr->rat != SYS_RAT_LTE_RADIO_ACCESS &&
           emm_get_reg_req_pending_status()== TRUE)
        { 
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
        }
        else
        {
          emm_start_tau_recovery_procedure(emm_ctrl_data_ptr) ;
        }
        break ;
      case CONN_REL_LTE_RRC_CONN_REL_NORMAL:
      case CONN_REL_LTE_RRC_CONN_REL_RLF:
      case CONN_REL_LTE_RRC_CONN_REL_CRE_FAILURE:
      case CONN_REL_LTE_RRC_CONN_REL_OOS_DURING_CRE:
      case CONN_REL_LTE_RRC_CONN_REL_ABORTED:
      case CONN_REL_LTE_RRC_CONN_REL_SIB_READ_ERROR:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
      case CONN_REL_LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS:
        /* Connection release is handled by procedure release indication client 
           for this case. Do nothing here. */
        break;
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
#ifdef FEATURE_NAS_EAB
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
#endif
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
        /* 5.5.3.2.6 Abnormal cases in the UE: a */
        MSG_HIGH_DS_1(MM_SUB, "=EMM= TAU request failed due to conn est failure %d T3430 stopped - Access barred/Conn rej",
                      cause);
        /* TAU may be restarted when signaling is available again */
        mm_stop_timer(TIMER_T3430);

        emm_ctrl_data_ptr->cell_barred_tau_pending = TRUE;

        /* Suspend data */
        if(emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend == FALSE)
        {
          emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend = TRUE ;
          mm_send_cm_ps_data_suspend_ind();
        }

#ifdef FEATURE_LTE_REL10
        /* 5.5.3.2.6: abnormal case k
           Process extended wait time from lower layers only when connection establishment
           reject cause is CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT */
        if((cause == CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT) &&
            emm_handle_extended_wait_time(emm_ctrl_data_ptr) == TRUE)
        {
          emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
          emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE);
        }
        else
#endif
        {
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_UPDATE_NEEDED);
        }
        emm_set_tau_trm_priority(FALSE);
        break;

#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
        MSG_HIGH_DS_1(MM_SUB, "=EMM= TAU request failed due to TRM failure %d",
                      cause);
        if((emm_search_tai_list(&current_tai,emm_ctrl_data_ptr->emm_tai_list_ptr)) &&
           (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED)
           && (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_P_TMSI))
        {
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);       
        }
        else
        {
          emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
          emm_save_mobility_info(emm_ctrl_data_ptr);
          emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE);
          if(emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend == FALSE)
          {
            emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend = TRUE;
            mm_send_cm_ps_data_suspend_ind();
          }
        }
        emm_set_tau_trm_priority(FALSE);
        mm_stop_timer(TIMER_T3430);
        mm_start_timer(TIMER_EMM_RADIO_RETRY, DEFAULT_TIMEOUT);
        break;
#endif 

      default :
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM connection cause",0,0,0) ;
        break ;
    }
  }

  /* 
    Mark the message as "SENT" letting connection 
    manager to delete this message from the cache 
    memory
  */
  pended_message_ptr->message_sent_indicator = TRUE ;

  /* Do not perform TAU procedure */
  return FALSE ;
}/* end of emm_tau_request_failed() */

/*===========================================================================

FUNCTION    EMM_TAU_REQUEST_RELEASE_INDICATION

DESCRIPTION
  This function is called when existing RRC connection which has been used for
  TAU procedure is release by RRC. See 24.301 5.5.3.2.6 b) for details.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function 

===========================================================================*/
void emm_tau_request_release_indication
(
  lte_emm_connection_cause_type  release_cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr,
  lte_rrc_conn_rel_redir_info_s  redir_info,
  lte_nas_tau_cause_type tau_cause
)
{
  emm_state_type        emm_state = EMM_INVALID_STATE ;

  lte_nas_local_cause_enum_T local_cause;
  emm_csfb_failure_cause_type csfb_failure_cause;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_LOWER_LAYER_FAILURE, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);

  /* Get EMM state */
  emm_state = EMM_GET_STATE() ;

  /* 
    Perform the following actions only if TAU Request has been 
    sent but no TAU Accept or Reject has been received
  */
  if (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED)
  {
    mm_stop_timer(TIMER_T3430);

    if( (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED) | (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) )
    {
      local_cause = emm_convert_rrc_cause_to_local_cause(release_cause,emm_ctrl_data_ptr);
      /* CSFB was buffered progress*/
      csfb_failure_cause.failure_type = INVALID_REJECT_CAUSE;
      csfb_failure_cause.failure_cause.lte_nas_failure_cause = local_cause;
      emm_process_extended_service_req_failure(csfb_failure_cause,emm_ctrl_data_ptr);
    }

    if(emm_ctrl_data_ptr->reg_req_pending_ptr->rat != SYS_RAT_LTE_RADIO_ACCESS &&
       emm_get_reg_req_pending_status() == TRUE)
    {
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
    }
    else
    {
      emm_start_tau_recovery_procedure(emm_ctrl_data_ptr) ;
    }
  }
} /* end of emm_tau_request_release_indication() */

/*===========================================================================

FUNCTION    EMM_TIMER_T3430_EXPIRE

DESCRIPTION
  This function is called when existing RRC connection which has been used for
  TAU procedure is release by RRC. See 24.301 5.5.3.2.6 b) for details.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function initiates abort of RRC connection

===========================================================================*/
void emm_timer_t3430_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  emm_csfb_failure_cause_type csfb_failure_cause;
  
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_TIME_OUT, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
  is_nas_emm_attach_tau_timeout = TRUE;

  emm_remove_release_client(emm_tau_request_release_indication);

  emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL) ;

  emm_start_tau_recovery_procedure(emm_ctrl_data_ptr) ;
  
  if(((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED) ||
      (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) ) &&
      !((emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_PERIODIC) &&
      (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)))
   {
         /* CSFB was buffered progress*/
     csfb_failure_cause.failure_type = INVALID_REJECT_CAUSE;
     csfb_failure_cause.failure_cause.lte_nas_failure_cause = LTE_NAS_LRRC_CONN_REL_ABORTED;
     emm_process_extended_service_req_failure(csfb_failure_cause,emm_ctrl_data_ptr);

   }
} /* end of emm_timer_t3430_expire() */


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================

FUNCTION    EMM_INITIATE_TAU_PROCEDURE

DESCRIPTION
  This function initiates TAU procedure attempt if procedure is allowed 
  according to 24.301 5.5.3

DEPENDENCIES
  This function shall be not called when UE is in DEREGISTERED state

RETURN VALUE
  None

SIDE EFFECTS
  Triggers TAU procedure

===========================================================================*/
void emm_initiate_tau_procedure
(
  /* TAU attempt type */
  emm_update_attempt_type  tau_attemp_type,

  /* TAU update type */
  lte_nas_emm_eps_update_val_type   emm_current_eps_ta_update,

  /* Flag to indicate enforce TAU */
  boolean  enforce_tau,

  /* EMM common control database */
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{

  emm_pended_msg_list_type *found_message = (emm_pended_msg_list_type *)NULL;
  emm_failure_type  emm_failure_cause;

  emm_state_type    emm_state    = EMM_INVALID_STATE ;
  emm_substate_type emm_substate = EMM_UNKNOWN_SUBSTATE ;

  boolean  fail_srv_req = TRUE;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ;
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  /* Get current EMM state and sub-state */
  emm_state    = EMM_GET_STATE() ;
  emm_substate = EMM_GET_SUBSTATE() ;

  /* Start EMM state dependent processing */
  switch (emm_state)
  {
    case EMM_NULL:
    case EMM_DEREGISTERED:
    case EMM_REGISTERED_INITIATED:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= EMM is not in REGISTERED state yet, TAU is not required");
      break ;
    case EMM_REGISTERED:
      /* No EMM procedure is avtive. Set "Active" flag to off for now. 
         emm_start_tau_procedure will check whether DS has data or not and may 
         update the flag before sending TAU REQ. */
      if((emm_substate != (emm_substate_type)EMM_REGISTERED_NO_CELL_AVAILABLE)
         || 
         (emm_ctrl_data_ptr->last_rrc_service_ind_ptr->svc_status == 
                                                 LTE_RRC_SVC_STATUS_AVAILABLE))
      {
        if((emm_ctrl_data_ptr->esr_type_in_progress == 
                                                      NAS_EMM_1X_CSFB_BUFFERED)
           ||
           (emm_ctrl_data_ptr->esr_type_in_progress ==
                                                   NAS_EMM_3GPP_CSFB_BUFFERED))
        {
          /* ESR is pending. Set "Active" flag to on in TAU REQ. */
          emm_start_tau_procedure(tau_attemp_type,
                                  emm_current_eps_ta_update,
                                  TRUE,
                                  emm_ctrl_data_ptr);
        }
        else
        {
          /* No ESR pending. Set "Active" flag to off here. 
             emm_start_tau_procedure will check whether DS has data or not and
             may update the flag before sending TAU REQ. */
          emm_start_tau_procedure(tau_attemp_type,
                                  emm_current_eps_ta_update,
                                  FALSE,
                                  emm_ctrl_data_ptr);
        }
      }
      else 
      {
        MSG_ERROR_DS_0(MM_SUB, "=EMM= TAU is not required when EMM is in EMM_REGISTERED_NO_CELL_AVAILABLE substate");
      }
      break ;
    case EMM_SERVICE_REQUEST_INITIATED:
      if(emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE)
      {
        /*If service request was initiated for CSFB call then store the esr type and dont fail the call.
         We will process the call after TAU accept.
         For cases where CSFB call was buffered, then fail the call.*/
        if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB)
        {
#ifdef FEATURE_1XSRLTE
          if(((emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == TRUE)
              &&(emm_ctrl_data_ptr->srlte_tau_needed == TRUE))
              ||(emm_ctrl_data_ptr->partial_esr == TRUE))
          {
            fail_srv_req = TRUE;
          }
          else
#endif
          {
            fail_srv_req = FALSE;
          }
        }
        else if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB)
        {
          fail_srv_req = FALSE;
        }
      }
       emm_failure_cause.cause_type = LTE_NAS_IRAT_LOWER_LAYER_FAILURE;
      /* Abort service request procedure. */
      if(EMM_GET_CONNECTION_STATE() == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE)
      {
        emm_abort_service_request(emm_ctrl_data_ptr, TRUE, fail_srv_req, 
                                  LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                  emm_failure_cause);
      }
      else
      {  
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, fail_srv_req, 
                                  LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                  emm_failure_cause);
      }
      if(emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE)
      {
        /* Set type to the correct type buffered*/
        if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB)
        {
          emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_3GPP_CSFB_BUFFERED;
        }
        else if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB)
        {
          emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_1X_CSFB_BUFFERED;
        }
        found_message = emm_find_pended_esr(emm_ctrl_data_ptr);
        if(found_message != NULL)
        {                    
          emm_delete_pended_message(found_message, emm_ctrl_data_ptr);
        }
      }
      /* Process unsent messages to abort service request procedure and notify
      ** DS and ESM the failure if necessary. For connection state waiting
      ** for connection confirmation state the pending messages will be 
      ** cleared on conn est failure with cause aborted.*/
      if(EMM_GET_CONNECTION_STATE() == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE)
      {
        emm_process_unsent_messages(EMM_SRV_REQ_FAILURE_NO_THROTTLE,
                                    emm_failure_cause, 
                                    emm_ctrl_data_ptr);
      }
      emm_process_pending_irat_msg(LTE_NAS_IRAT_INVALID_STATE,
                                   LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                   LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                                   emm_ctrl_data_ptr);

      /* EMM procedure is active. Set "Active" flag On */
      emm_start_tau_procedure(tau_attemp_type,
                              emm_current_eps_ta_update,
                              TRUE,
                              emm_ctrl_data_ptr);
      break;
    case EMM_DEREGISTERED_INITIATED:
      /* Abort MO detach. It should be resumed after TAU. */
      mm_stop_timer(TIMER_T3421);
      emm_remove_release_client(emm_mo_detach_release_ind);
      /* EMM procedure is active. Set "Active" flag On */
      emm_start_tau_procedure(tau_attemp_type,
                              emm_current_eps_ta_update,
                              TRUE,
                              emm_ctrl_data_ptr);
      break ;
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      if (enforce_tau == TRUE)
      {
        /* Active flag needs to be set if MO detach procedure was active 
           Note: If service request procedure was active, DS data check will be
           true later to set the active flag. */
        emm_start_tau_procedure(tau_attemp_type,
                                emm_current_eps_ta_update,
                                emm_ctrl_data_ptr->ongoing_detach_procedure,
                                emm_ctrl_data_ptr);
      }
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU has already been started");
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
      }
      break ;
    default:
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM state",0,0,0) ;
      break ;
  }
} /* end of emm_initiate_tau_procedure() */

/*===========================================================================

FUNCTION    EMM_TIMER_T3412_EXPIRE

DESCRIPTION
  This function is called upon expiry of Periodic TAU timer (T3412) and 
  tiggers Periodic TAU if TAU procedure has not been triggered by any
  other event described in TS 24.301 5.5.3.2.2

DEPENDENCIES
  This function shall be not called when UE is in DEREGISTERED state

RETURN VALUE
  None

SIDE EFFECTS
  Triggers TAU procedure

===========================================================================*/
void emm_timer_t3412_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

#ifdef FEATURE_LTE_REL9
  if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= EMC attached - Perform local detach");
    emm_ps_detach_clean_up(emm_ctrl_data_ptr);
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
    return;
  }
#endif

  if((EMM_GET_STATE() == EMM_REGISTERED) && 
     (EMM_GET_SUBSTATE() == (emm_substate_type)EMM_REGISTERED_NORMAL_SERVICE) &&
     (emm_ctrl_data_ptr->plmn_service_state->service_status == SYS_SRV_STATUS_SRV) &&
     (emm_ctrl_data_ptr->rrc_active == TRUE) 
#ifdef FEATURE_LTE_REL10
     &&
     ((mm_timer_status[TIMER_T3346] == TIMER_STOPPED)
#ifdef FEATURE_LTE_REL11
      ||
      ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
       ((emm_check_high_priority(emm_ctrl_data_ptr)) ||
        (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE)))
#endif
      )
#endif
     )
  {
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_PERIODIC;
    emm_initiate_tau_procedure(EMM_INITIAL_UPDATE, PERIODIC_UPDATING, FALSE,
                               emm_ctrl_data_ptr) ;
  }
  else
  {
    MSG_HIGH_DS_3(MM_SUB, "=EMM= TAU delayed - EMM state %d, substate %d, RRC status %d",
                  EMM_GET_STATE(),
                  EMM_GET_SUBSTATE(),
                  emm_ctrl_data_ptr->rrc_active);

    if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_INVALID)
    {
      /* Periodic TAU has the lower pri than normal TAU */
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_PERIODIC;
    }

#ifdef FEATURE_LTE_REL10
    MSG_HIGH_DS_2(MM_SUB, "=EMM= Delayed TAU cause %d, t3346 %d",
                  emm_ctrl_data_ptr->tau_cause,
                  mm_timer_status[TIMER_T3346]);
#else
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Delayed TAU cause %d", emm_ctrl_data_ptr->tau_cause);
#endif

    /* If ISR is ON, start ISR deactivate timer T3423. If UE is able to be perform TAU
        then this timer will be stopped immediately after UE moves to connected state */
    mm_handle_timer_t3412_expiry(emm_ctrl_data_ptr);
    if(EMM_GET_STATE() == EMM_REGISTERED)
    {
      if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
      {
        mm_start_timer(TIMER_T3423,emm_ctrl_data_ptr->t3423_value);
        MSG_HIGH_DS_0(MM_SUB, "=EMM= ISR deactivate timer T3423 started");
      }

      /* 24.301 5.3.5 Set the flag to start combined TAU IMSI attach later when
         UE goes back to normal service */
      if(emm_ctrl_data_ptr->srv_domain_registered == SYS_SRV_DOMAIN_CS_PS)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Set SGs association flag -T3412 expiry");
        emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
      }
    }
  }
} /* end of emm_timer_t3412_expire() */


/*===========================================================================

FUNCTION    EMM_PROCESS_IMS_VOICE_TAG_REG_STATUS_IND

DESCRIPTION
  This functions sets the IMS registration status parameter in emm ctrl data. 
  Also performs TAU if IMS registration status is changing from not registered to registered 
  and if ISR is ON, 

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/
#ifdef FEATURE_LTE_REL9
void emm_process_ims_voice_tag_reg_status_ind
(
  mm_cmd_type  *cmd_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  boolean is_ims_reg_status_changed = FALSE;
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  MSG_HIGH_DS_2(MM_SUB, "=EMM= Previous IMS reg status %d New IMS reg status %d",
                emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag,
                cmd_ptr->cmd.cm_mm_ims_registration_info.ims_registration_status);

  if(emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag != 
                                   cmd_ptr->cmd.cm_mm_ims_registration_info.ims_registration_status)
  {
    is_ims_reg_status_changed = TRUE;
    emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag =
                                   cmd_ptr->cmd.cm_mm_ims_registration_info.ims_registration_status;
  }

  /* If IMS registration status is changing from not registered to registered and if ISR is ON, then
  ** perform TAU (24.301 Section 5.5.3.3.2)
  */
  if((is_ims_reg_status_changed == TRUE) &&
     (mm_emm_is_mmit_enabled() == TRUE) &&
     (emm_ctrl_data_ptr->rrc_active == TRUE) &&
     (EMM_GET_STATE() == EMM_REGISTERED))
  {
    if(EMM_GET_SUBSTATE() == (emm_substate_type)EMM_REGISTERED_NORMAL_SERVICE)
    {
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      emm_initiate_tau_procedure(EMM_INITIAL_UPDATE, COMBINED_TA_LA_UPDATING, FALSE,
                                      emm_ctrl_data_ptr) ;
    }
    else
    {
      MSG_HIGH_DS_3(MM_SUB, "=EMM= TAU delayed - EMM state %d, substate %d, RRC status %d",
                    EMM_GET_STATE(),
                    EMM_GET_SUBSTATE(),
                    emm_ctrl_data_ptr->rrc_active);
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
    }
  }

}

#endif
/*===========================================================================

FUNCTION   emm_process_wms_sms_info

DESCRIPTION
  This functions sets the MO SMS status parameter in emm ctrl data. 

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/
void emm_process_wms_sms_info
(
  mm_cmd_type  *cmd_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != NULL);
  ASSERT(cmd_ptr != NULL);
  MSG_HIGH_DS_3(MM_SUB, "=EMM=sms_over_ip_status %d ,sms_over_nas_status %d ,sms_over_s102 %d ",
                cmd_ptr->cmd.wms_mm_sms_call_info.sms_over_ip_status,
                cmd_ptr->cmd.wms_mm_sms_call_info.sms_over_nas_status,
                cmd_ptr->cmd.wms_mm_sms_call_info.sms_over_s102);
  emm_ctrl_data_ptr->wms_sms_status.sms_over_nas_status = 
                  cmd_ptr->cmd.wms_mm_sms_call_info.sms_over_nas_status;
  emm_ctrl_data_ptr->wms_sms_status.sms_over_ims_status = 
                  cmd_ptr->cmd.wms_mm_sms_call_info.sms_over_ip_status;
  emm_ctrl_data_ptr->wms_sms_status.sms_over_s102_status = 
                  cmd_ptr->cmd.wms_mm_sms_call_info.sms_over_s102;
  if((emm_ctrl_data_ptr->wms_sms_status.sms_over_nas_status == TRUE)||
     (emm_ctrl_data_ptr->wms_sms_status.sms_over_ims_status == TRUE) ||
     (emm_ctrl_data_ptr->wms_sms_status.sms_over_s102_status == TRUE))
  {
    emm_check_possible_tau_for_mmtel_and_sms(emm_ctrl_data_ptr);
  }
}

/*===========================================================================

FUNCTION    EMM_PROCESS_MMTEL_CALL_STATUS_INFO

DESCRIPTION
  This functions sets the MMTEL status in emm ctrl data. 

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/
void emm_process_mmtel_call_status_info
(
  mm_cmd_type  *cmd_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != NULL);
  ASSERT(cmd_ptr != NULL);
  MSG_HIGH_DS_4(MM_SUB, "=EMM=Video %d ,Voice %d ,SMS %d sys_mode %d",
                cmd_ptr->cmd.cm_mm_mmtel_call_info.video_call_status,
                cmd_ptr->cmd.cm_mm_mmtel_call_info.voice_call_status,
                cmd_ptr->cmd.cm_mm_mmtel_call_info.sms_status,
                cmd_ptr->cmd.cm_mm_mmtel_call_info.sys_mode);
  emm_ctrl_data_ptr->mmtel_call_status.video_call_status = 
                  cmd_ptr->cmd.cm_mm_mmtel_call_info.video_call_status;
  emm_ctrl_data_ptr->mmtel_call_status.voice_call_status = 
                  cmd_ptr->cmd.cm_mm_mmtel_call_info.voice_call_status;
  emm_ctrl_data_ptr->mmtel_call_status.sms_status = 
                  cmd_ptr->cmd.cm_mm_mmtel_call_info.sms_status;
  emm_ctrl_data_ptr->mmtel_call_status.sys_mode = cmd_ptr->cmd.cm_mm_mmtel_call_info.sys_mode;
  if(((emm_ctrl_data_ptr->mmtel_call_status.video_call_status == TRUE )||
     (emm_ctrl_data_ptr->mmtel_call_status.voice_call_status == TRUE )||
     (emm_ctrl_data_ptr->mmtel_call_status.sms_status == TRUE )) && 
     (emm_ctrl_data_ptr->mmtel_call_status.sys_mode == SYS_SYS_MODE_LTE))
  {
    emm_check_possible_tau_for_mmtel_and_sms(emm_ctrl_data_ptr);
  }
}

/*===========================================================================

FUNCTION    emm_check_possible_tau_for_mmtel_and_sms

DESCRIPTION
  This function starts TAU procedure that was barred when MMTEL or SMS is becoming active and skip-able from SIB2

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void emm_check_possible_tau_for_mmtel_and_sms
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  emm_update_attempt_type update_type = EMM_INITIAL_UPDATE;

  /*5.5.3.2.6 Abnormal cases in the UE
  If access is barred for "originating signalling" (see 3GPP TS 36.331 [22]) and if:
  one of the MO MMTEL voice call is started, MO MMTEL video call is started or MO SMSoIP is started conditions is satisfied; or
  the upper layers request to send a mobile originated SMS over NAS or SMS over S102; 
  then the tracking area updating procedure shall be started according to subclause 5.5.3.2.2.*/

  if((emm_ctrl_data_ptr->cell_barred_tau_pending== TRUE)&&
     (emm_connection_establishment_barred(LTE_RRC_EST_CAUSE_MO_SIGNALING,NULL,emm_ctrl_data_ptr))&&
     (emm_ctrl_data_ptr->emm_connection_state == EMM_IDLE_STATE)&&
     (emm_ctrl_data_ptr->rrc_active == TRUE)&&
     (emm_ctrl_data_ptr->plmn_service_state->service_status !=  SYS_SRV_STATUS_NO_SRV )&& 
     (EMM_GET_STATE() == EMM_REGISTERED)&&
     (EMM_GET_SUBSTATE() == (emm_substate_type)EMM_REGISTERED_UPDATE_NEEDED))
  {
    /*reset the flag*/
    emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;

    if(emm_ctrl_data_ptr->emm_tau_attempt_counter != 0)
    {
      update_type = EMM_UPDATE_RECOVERY;
    }
    emm_initiate_tau_procedure(update_type, 
                               emm_get_eps_update_type(emm_ctrl_data_ptr), 
                               FALSE, 
                               emm_ctrl_data_ptr);
  }
}


/*===========================================================================

FUNCTION    EMM_START_TAU_PROCEDURE

DESCRIPTION
  This function starts TAU procedure as described in TS 24.301 5.5.3.2.2

DEPENDENCIES
  This function shall be not called when UE is in DEREGISTERED state

RETURN VALUE
  None

SIDE EFFECTS
  This function moves the EMM into EMM_TRACKING_AREA_UPDATING_INITIATED state

===========================================================================*/
void emm_start_tau_procedure
(
  /* TAU attempt type */
  emm_update_attempt_type  tau_attemp_type,

  /* TAU update type */
  lte_nas_emm_eps_update_val_type   emm_current_eps_ta_update,

  /* 
    Indicates that there are ongoing EPS activities
    either in DS or ESM
  */
  boolean eps_active_flag,

  /* EMM common control database */
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  dword                          sequence_number = 0 ;
  lte_nas_emm_tau_req           *nas_emm_tau_req = NULL ;
  boolean                        gsm_spt  = FALSE;
  boolean                        umts_spt = FALSE;
  boolean                        tds_spt  = FALSE;

  lte_nas_conn_req_routing_info  routing_info = ROUTING_INFO_NONE;
  byte index = 0;
  emm_security_context_info *context_ptr = NULL;
  lte_rrc_est_cause_e          conn_est_cause = LTE_RRC_EST_CAUSE_MO_SIGNALING;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
         0x0,
         sizeof(lte_nas_outgoing_msg_type)) ;
  
  /*Save the LAST attempted TAI before we send TAU_REQ*/
  emm_save_last_attempted_tai(emm_ctrl_data_ptr);

  nas_emm_tau_req = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.tau_req;

  if (tau_attemp_type == EMM_INITIAL_UPDATE)
  {
    SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band);
    emm_stop_all_timers(emm_ctrl_data_ptr) ;

    emm_ctrl_data_ptr->emm_tau_attempt_counter = 0 ;

    emm_ctrl_data_ptr->emm_ongoing_eps_ta_update = emm_current_eps_ta_update ;

    emm_ctrl_data_ptr->plmn_service_state->plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn) ;

    emm_ctrl_data_ptr->plmn_service_state->plmn_service_capability = 
                                            SYS_SRV_CAPABILITY_CS_PS_SERVICE;
    emm_ctrl_data_ptr->plmn_service_state->plmn_forbidden = FALSE ;

    emm_ctrl_data_ptr->plmn_service_state->active_rat = SYS_RAT_LTE_RADIO_ACCESS ;

    SYS_LTE_BAND_MASK_ADD_BAND(&emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band,emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_band); 

    emm_ctrl_data_ptr->plmn_service_state->tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac ;
  }

  if (is_nas_emm_attach_tau_timeout == TRUE || emm_ctrl_data_ptr->emm_tau_attempt_counter == 0)
  {
    emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_START,(nas_event_end_reason)PROC_END_NO_REASON, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
    is_nas_emm_attach_tau_timeout = FALSE;
  } 
  else
  {
    emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_RESEND, (nas_event_end_reason)PROC_END_NO_REASON, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
  }
  nas_emm_tau_req->lte_nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES ;

  nas_emm_tau_req->lte_nas_hdr.msg_id = TRACKING_AREA_UPADTE_REQUEST ;

  nas_emm_tau_req->security_hdr = PLAIN_NAS_MSG ;

#ifdef FEATURE_LTE_REL9
    if((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) 
       ||
       (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == TRUE))
    {
      conn_est_cause = LTE_RRC_EST_CAUSE_EMERGENCY;
    }
#endif

  /* Print the KASME */
  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  if( context_ptr != NULL )
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= KASME - including index [0-31]");
    for(index=0; index+7< 32; index = index+8)
    {
      MSG_HIGH_DS_8(MM_SUB, "=EMM= 0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x", 
                  context_ptr->nas_kasme[index],context_ptr->nas_kasme[index+1],context_ptr->nas_kasme[index+2],context_ptr->nas_kasme[index+3],
                  context_ptr->nas_kasme[index+4],context_ptr->nas_kasme[index+5],context_ptr->nas_kasme[index+6],context_ptr->nas_kasme[index+7]);
    }
  }
  else 
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Security not valid, no KASME");
  }

  gsm_spt  = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_GSM_RADIO_ACCESS);

  umts_spt = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_UMTS_RADIO_ACCESS);
   
  tds_spt  = mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                             SYS_RAT_TDS_RADIO_ACCESS);

  MSG_HIGH_DS_3(MM_SUB, "=EMM= gsm_spt = %d, umts_spt = %d tds_spt = %d",
                gsm_spt,
                umts_spt,
                tds_spt);
  
  /* 24 301 - 8.2.29.7 The UE shall include this IE, unless the UE performs a
     periodic tracking area updating procedure. */
  if(emm_ctrl_data_ptr->tau_cause != TAU_CAUSE_PERIODIC)
  {  
    nas_emm_tau_req->ue_capability.ue_sec_capability =
                             emm_ctrl_data_ptr->ue_security_capability;

    nas_emm_tau_req->ue_capability_exists = TRUE;
#ifdef FEATURE_LTE_REL9     
    if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >=  LTE_3GPP_REL9)
    {
      nas_emm_tau_req->ue_capability.lte_positioning_prot_supported = emm_ctrl_data_ptr->lpp_supported;
      nas_emm_tau_req->ue_capability.lcs_notification_supported = emm_ctrl_data_ptr->lcs_supported;
      nas_emm_tau_req->ue_capability.notification_proc_supported = FALSE;
#ifdef FEATURE_SGLTE
      if(MM_IS_IN_SGLTE_MODE)
      {
        if(emm_ctrl_data_ptr->srvcc_over_sglte_support == TRUE)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= UE support Notification procedure ");
          nas_emm_tau_req->ue_capability.notification_proc_supported = TRUE;
        }
      }
      else
#endif
      if(emm_is_srvcc_supported(emm_ctrl_data_ptr) == TRUE)
      {
        nas_emm_tau_req->ue_capability.notification_proc_supported = TRUE;
      }
      MSG_HIGH_DS_1(MM_SUB, "=EMM= Does UE support Notification procedure (1/0)? = %d ",nas_emm_tau_req->ue_capability.notification_proc_supported);
    }
#endif
  }

  if(emm_is_1xsrvcc_supported(emm_ctrl_data_ptr) == TRUE )
  {
    nas_emm_tau_req->ue_capability.onexsrvcc_capability_supported = TRUE;
  }
  else
  {
    nas_emm_tau_req->ue_capability.onexsrvcc_capability_supported = FALSE;
  }

  /* Checking with DS on data availability here in case DS may have data 
     available after initial TAU attempt and before TAU retry attempt. */
  if((eps_active_flag == TRUE) || (ds3gpp_per_subs_is_ps_data_available((sys_modem_as_id_e_type)mm_sub_id) == TRUE) 
    ||((emm_ctrl_data_ptr->volte_persistent_active_flag == TRUE)&&((emm_mmtel_status(emm_ctrl_data_ptr) == TRUE) || (emm_ctrl_data_ptr->wms_sms_status.sms_over_ims_status == TRUE)))
#ifdef FEATURE_LTE_REL9
     ||
     (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) ||
     (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == TRUE)
#endif
     )
  {
    /* Set "ACTIVE" flag to TRUE indicating EMM or ESM or DS are active */
    MSG_MED_DS_0(MM_SUB, "=EMM= Setting ACTIVE flag to TRUE in TAU REQ");
    nas_emm_tau_req->eps_update.active_flag = TRUE;
  }

  nas_emm_tau_req->eps_update.eps_update_val = emm_current_eps_ta_update ;

  mm_update_start_tau_procedure(emm_current_eps_ta_update);

  if((gsm_spt == FALSE) && (umts_spt == FALSE) && (tds_spt == FALSE))
  {
    if(emm_ctrl_data_ptr->emm_guti_valid == TRUE)
    {
      MSG_LOW_DS_0(MM_SUB, "=EMM= TAU Req EPS Mobile Identity - GUTI");
      nas_emm_tau_req->eps_mobile_id.ue_id = GUTI_ID ;
      nas_emm_tau_req->eps_mobile_id.ue_id_info.guti = 
                                                     emm_ctrl_data_ptr->emm_guti;
      nas_emm_tau_req->guti_type_exists = TRUE;
      nas_emm_tau_req->guti_type = LTE_NAS_GUTI_TYPE_NATIVE;
    }
    else if(emm_ctrl_data_ptr->plmn_service_state->sim_state !=  
                                                   SYS_SIM_STATE_NOT_AVAILABLE)
    {
      MSG_LOW_DS_0(MM_SUB, "=EMM= TAU Req EPS Mobile Identity - IMSI");
      nas_emm_tau_req->eps_mobile_id.ue_id = IMSI_ID;
      nas_emm_tau_req->eps_mobile_id.ue_id_info.imsi.num_id_digits_len = 
                                              emm_ctrl_data_ptr->imsi.num_digits;
      if(emm_ctrl_data_ptr->imsi.num_digits <= MAX_NO_OF_IMSI)
      {
        memscpy((void*)&nas_emm_tau_req->eps_mobile_id.ue_id_info.imsi.num_id_digits[0],
                sizeof(nas_emm_tau_req->eps_mobile_id.ue_id_info.imsi.num_id_digits),
                (void*)&emm_ctrl_data_ptr->imsi.imsi_digits[0], 
                emm_ctrl_data_ptr->imsi.num_digits) ; 
      }
      else
      {
        MSG_ERROR_DS_2(MM_SUB, "=EMM= num_digits %d exceeds imsi length, concatinating to %d",
                       emm_ctrl_data_ptr->imsi.num_digits,
                       MAX_NO_OF_IMSI);
        nas_emm_tau_req->eps_mobile_id.ue_id_info.imsi.num_id_digits_len = 
                                                                MAX_NO_OF_IMSI;
        memscpy((void*)&nas_emm_tau_req->eps_mobile_id.ue_id_info.imsi.num_id_digits[0],
                sizeof(nas_emm_tau_req->eps_mobile_id.ue_id_info.imsi.num_id_digits),
                (void*)&emm_ctrl_data_ptr->imsi.imsi_digits[0],
                sizeof(emm_ctrl_data_ptr->imsi.imsi_digits));
      }   
    }
#ifdef FEATURE_LTE_REL9
    else
    {
      nas_emm_tau_req->eps_mobile_id.ue_id = IMEI_ID;
      emm_populate_imei(&nas_emm_tau_req->eps_mobile_id.ue_id_info.imei, TRUE);
    }
#endif
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= TIN = %d ", emm_ctrl_data_ptr->TIN);

    switch(emm_ctrl_data_ptr->TIN)
    {
    case NAS_MM_TIN_P_TMSI:
      /* Do we need to check fo rvalidity of RAI also?*/
      if( mm_is_ptmsi_valid() == TRUE)
      {
        /* Map P-TMSI & RAI to GUTI*/
        mm_map_umts_identity_to_lte(&nas_emm_tau_req->eps_mobile_id,
                                    nas_emm_tau_req->ptmsi_signature,
                                    &nas_emm_tau_req->ptmsi_signature_exists);
        if(EMM_GET_CONNECTION_STATE() == EMM_IDLE_STATE)
        {
          /* Fill nonce optional field */
          nas_emm_tau_req->nonce_ue_exists = TRUE;
          emm_set_nonce_ue(nas_emm_tau_req->nonce_ue, emm_ctrl_data_ptr);
          nas_emm_tau_req->ciphering_key_seq_num_exists =  TRUE;
          nas_emm_tau_req->ciphering_key_seq_num = 
                   auth_get_ciphering_key_sequence_number(mm_get_current_security_context( RRC_PS_DOMAIN_CN_ID ));

        }
        
        nas_emm_tau_req->guti_type_exists = TRUE;
        nas_emm_tau_req->guti_type = LTE_NAS_GUTI_TYPE_MAPPED;
      }
      else
      {
        MSG_FATAL_DS(MM_SUB,"=EMM= TIN points to P-TMSI, but P-TMSI is invalid",0,0,0) ; 
      }
      /* Copy GUTI to additional GUTI*/
      if(emm_ctrl_data_ptr->emm_guti_valid == TRUE)
      {
        nas_emm_tau_req->additional_guti = emm_ctrl_data_ptr->emm_guti;
        nas_emm_tau_req->additional_guti_exists = TRUE;
      }
      break;
    case NAS_MM_TIN_GUTI:
    case NAS_MM_TIN_RAT_RELATED_TMSI:
      emm_populate_guti(&nas_emm_tau_req->eps_mobile_id, emm_ctrl_data_ptr);

      nas_emm_tau_req->guti_type_exists = TRUE;
      nas_emm_tau_req->guti_type = LTE_NAS_GUTI_TYPE_NATIVE;
      break;
    case NAS_MM_TIN_NONE:
    default:
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal TIN value %u",emm_ctrl_data_ptr->TIN,0,0) ;
        break;
    }
  }

  emm_update_gw_sec_capabilities(emm_ctrl_data_ptr, &nas_emm_tau_req->ue_capability);
									   
  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  if( context_ptr != NULL )
  {
    emm_security_context_info *non_current_native_context_ptr;

    MSG_HIGH_DS_2(MM_SUB, "=EMM= Current context of type %d , KSI %d ",
                  context_ptr->context_type,
                  context_ptr->nasKSI.keyset_id);

    /* nas_key_set_identifier_asme will be populated with mapped eKSI if current context is mapped */
    nas_emm_tau_req->nas_key_set_identifier_asme = context_ptr->nasKSI;

    /* Print the KASME */
    MSG_HIGH_DS_0(MM_SUB, "=EMM= KASME - including index [0-31]");
    for(index=0; index+7< 32; index = index+8)
    {
      MSG_HIGH_DS_8(MM_SUB, "=EMM= 0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x", 
                  context_ptr->nas_kasme[index],context_ptr->nas_kasme[index+1],context_ptr->nas_kasme[index+2],context_ptr->nas_kasme[index+3],
                  context_ptr->nas_kasme[index+4],context_ptr->nas_kasme[index+5],context_ptr->nas_kasme[index+6],context_ptr->nas_kasme[index+7]);
    }

    if(context_ptr->context_type == CONTEXT_TYPE_MAPPED_EPS) 
    { 
      non_current_native_context_ptr = 
        emm_get_context_by_type (emm_ctrl_data_ptr, CONTEXT_TYPE_FULL_NATIVE_EPS );
      if(non_current_native_context_ptr != NULL )
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= populate non-current native context with eKSI %d in TAU Req ",
                      non_current_native_context_ptr->nasKSI.keyset_id);
        nas_emm_tau_req->nas_keyset_identifier_sgsn_exists = TRUE;
        nas_emm_tau_req->nas_key_set_identifier_sgsn = 
                                   non_current_native_context_ptr->nasKSI;
      }
    }
  } 
  else 
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM=  No Current Context ");
    nas_emm_tau_req->nas_key_set_identifier_asme.security_context = FALSE ;
    nas_emm_tau_req->nas_key_set_identifier_asme.keyset_id = NO_KEY_AVAIALABLE ; 
    nas_emm_tau_req->nas_key_set_identifier_sgsn.security_context = FALSE ;
    nas_emm_tau_req->nas_key_set_identifier_sgsn.keyset_id = NO_KEY_AVAIALABLE ; 
  }

  if (emm_ctrl_data_ptr->last_registered_tai_valid == TRUE)
  {
    nas_emm_tau_req->last_visited_reg_tai_exists = TRUE ;
    nas_emm_tau_req->last_visited_reg_tai = emm_ctrl_data_ptr->last_registered_tai ;
  }

  nas_emm_tau_req->eps_bearer_context_status_exists = TRUE ;
  nas_emm_tau_req->eps_bearer_context_status = emm_ctrl_data_ptr->eps_bearer_context_status ;

    if((emm_current_eps_ta_update == COMBINED_TA_LA_UPDATING_IMSI_ATTACH) ||
       (emm_current_eps_ta_update == COMBINED_TA_LA_UPDATING))
    {
      if(mm_has_valid_lai() == TRUE)
      {
        /* Populate LAI out of preserved MM mobility info */
        nas_emm_tau_req->locn_area_id_exists = TRUE ;

        memscpy((void*)&nas_emm_tau_req->locn_area_id,
                sizeof(nas_emm_tau_req->locn_area_id),
                (void*)&emm_ctrl_data_ptr->emm_location_area->lai[0],
                sizeof(emm_ctrl_data_ptr->emm_location_area->lai)) ;
        if(FALSE == mm_is_tmsi_valid(emm_ctrl_data_ptr->emm_location_area->tmsi,TMSI_SIZE))
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= TMSI does not exist, including TMSI_STATUS in TAU_REQ");
          nas_emm_tau_req->tmsi_status_exists  = TRUE;
          nas_emm_tau_req->tmsi_status = FALSE; /*TMSI not available*/
        }        
#ifdef FEATURE_LTE_REL11
       else if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
       {
          nas_emm_tau_req->tmsi_based_nri = emm_get_tmsi_based_nri(emm_ctrl_data_ptr);
          if(nas_emm_tau_req->tmsi_based_nri.length > 0)
          {
            nas_emm_tau_req->tmsi_based_nri_exists = TRUE; 
          }
      }
#endif
      }
      else
      { 
        /* Do not populate LAI & TMSI */
        nas_emm_tau_req->locn_area_id_exists = FALSE ;        
        MSG_HIGH_DS_0(MM_SUB, "=EMM= TMSI does not exist, including TMSI_STATUS in TAU_REQ");
        nas_emm_tau_req->tmsi_status_exists  = TRUE;
        nas_emm_tau_req->tmsi_status = FALSE; /*TMSI not available*/
      }

      /* Populate Classmark 2 IE */
      nas_emm_tau_req->classmark_2_exists = TRUE ;
      emm_read_coded_classmark_2(&nas_emm_tau_req->classmark_2, // Classmark 2 IE to be encoded here
                                 gsm_spt,
                                 (boolean)(umts_spt|tds_spt));

      if(emm_ctrl_data_ptr->reg_req_pending_ptr->sms_only == TRUE)
      { 
        nas_emm_tau_req->additional_update_exists = TRUE;
        nas_emm_tau_req->additional_update = TRUE; 
      }
      else
      {
        nas_emm_tau_req->additional_update_exists = FALSE;
      }

    }
    else
    {
      /* Do not populate LAI, TMSI, MS network capability & Classmark 2 IEs*/
      nas_emm_tau_req->locn_area_id_exists = FALSE ;
      nas_emm_tau_req->tmsi_status_exists  = FALSE ;
      nas_emm_tau_req->classmark_2_exists  = FALSE;
#ifdef FEATURE_SGLTE
      if(MM_IS_IN_SGLTE_MODE)
      {
        if(emm_ctrl_data_ptr->srvcc_over_sglte_support == TRUE)
        {
          /* Populate Classmark 2 IE */
          nas_emm_tau_req->classmark_2_exists = TRUE;
          emm_read_coded_classmark_2(&nas_emm_tau_req->classmark_2, // Classmark 2 IE to be encoded here
                                     gsm_spt,
                                     (boolean)(umts_spt|tds_spt));
        } 
      }
      else
#endif
      if(emm_is_srvcc_supported(emm_ctrl_data_ptr) == TRUE)      
      {
        /* Populate Classmark 2 IE */
        nas_emm_tau_req->classmark_2_exists = TRUE;
        emm_read_coded_classmark_2(&nas_emm_tau_req->classmark_2, // Classmark 2 IE to be encoded here
                                   gsm_spt,
                                   (boolean)(umts_spt|tds_spt));
      }
    }
    if((emm_disable_geran_cap == FALSE) &&
       (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                        SYS_RAT_UMTS_RADIO_ACCESS)|| 
        mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                        SYS_RAT_GSM_RADIO_ACCESS)||
        mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                        SYS_RAT_TDS_RADIO_ACCESS)))

    {
      /* Populate MS network capability */
      nas_emm_tau_req->ms_nw_capability_exists = TRUE;
      nas_emm_tau_req->ms_nw_capability = get_ms_network_capability() ;
    }
    /* UE radio capability information update needed flag */
    if((emm_ctrl_data_ptr->tau_pending_gw_attach == TRUE) ||
       (emm_ctrl_data_ptr->radio_cap_update != EMM_RADIO_CAP_UPDATE_NONE))
    {
      MSG_MED_DS_2(MM_SUB, "=EMM= Setting radio cap info update flag tau_pending_gw_attach %d radio_cap_update %d",
                   emm_ctrl_data_ptr->tau_pending_gw_attach,
                   emm_ctrl_data_ptr->radio_cap_update);
      nas_emm_tau_req->ue_radio_capability_info_update = TRUE;
      if(emm_ctrl_data_ptr->radio_cap_update == EMM_RADIO_CAP_UPDATE_PENDING)
      {
        emm_ctrl_data_ptr->radio_cap_update = EMM_RADIO_CAP_UPDATE_IN_PROGRESS;
      }
    }

    if((emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent == NAS_SEND_DRX_ON_LTE_RAT) ||
        (emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent == NAS_SEND_DRX_ON_ALL_RATS))
    {
      /* DRX parameter */
      nas_emm_tau_req->drx_param_exists = TRUE;
      nas_emm_tau_req->drx_param = get_drx_parameter();
    }

#ifdef FEATURE_LTE_REL9
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9 &&
     emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.is_voice_domain_ie_valid == TRUE)
  {
    nas_emm_tau_req->voice_domain_pref.ue_usage_settings = 
                            emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.voice_or_data_centric;
    nas_emm_tau_req->voice_domain_pref.voice_pref_for_utran = 
                            emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.voice_domain_pref;
    nas_emm_tau_req->voice_domain_pref_exists = 
                            emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param.is_voice_domain_ie_valid;
  }
#endif

#ifdef FEATURE_SGLTE
  if(MM_IS_IN_SGLTE_MODE)
  {
    if(emm_ctrl_data_ptr->srvcc_over_sglte_support == TRUE)
    {
      /* Populate Classmark 3 IE */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU Request - SRVCC/GERAN supported, populate GS Classmark 3 IE ");
      nas_emm_tau_req->classmark_3_exists = TRUE ;
      emm_read_coded_classmark_3(&nas_emm_tau_req->classmark_3, // Classmark 3 IE to be encoded here
                                 gsm_spt,
                                 umts_spt);

      /*  supported_voice_codec info */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU Request supports codecs list ");
      nas_emm_tau_req->supported_voice_codec = gmm_get_supported_codec_list();
      nas_emm_tau_req->supported_voice_codec_exists = TRUE;
    }
  }
  else
#endif
  if (emm_is_srvcc_supported(emm_ctrl_data_ptr) == TRUE)
  {
    if((gsm_spt == TRUE) &&
       (emm_ctrl_data_ptr->l2g_srvcc_capability_support == TRUE))
    {
      /* Populate Classmark 3 IE */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU Request - SRVCC/GERAN supported, populate GS Classmark 3 IE ");
      nas_emm_tau_req->classmark_3_exists = TRUE ;
      emm_read_coded_classmark_3(&nas_emm_tau_req->classmark_3, // Classmark 3 IE to be encoded here
                                 gsm_spt,
                                 umts_spt);
    }
      
    /*  supported_voice_codec info */
    MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU Request supports codecs list ");
    nas_emm_tau_req->supported_voice_codec = gmm_get_supported_codec_list();
    nas_emm_tau_req->supported_voice_codec_exists = TRUE;
  }

#ifdef FEATURE_LTE_REL10
  nas_emm_tau_req->ms_nw_feature_support_exists = FALSE;
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
  {
    nas_emm_tau_req->ms_nw_feature_support_exists = TRUE;
    nas_emm_tau_req->ext_periodic_timer_support = TRUE;

    /* Set low priority indicator. */
    nas_emm_tau_req->low_priority_indicator = 
             emm_evaluate_low_priority_indicator_value(emm_ctrl_data_ptr, conn_est_cause);

    /* Override the connection est cause to Delay tolerant if the low priority indicator is set to TRUE */
    if(nas_emm_tau_req->low_priority_indicator == TRUE)
    {
      conn_est_cause = LTE_RRC_EST_CAUSE_DELAY_TOLERANT_ACCESS;
      emm_ctrl_data_ptr->delay_tolerant_access = TRUE;
      emm_ctrl_data_ptr->mo_data_enabled_dt = FALSE;
    }
  }
#endif

  /* Obtain sequence number */
  sequence_number = emm_get_nas_transaction_id() ;

  EMM_MOVES_TO_TRACKING_AREA_UPDATING_INITIATED_STATE() ;

  /* If connection is setup.. TAU REQ will go through, do not start
     the T3440 on accept as TAU REQ was not sent in IDLE */
  emm_ctrl_data_ptr->start_t3440_on_accept = FALSE;

#ifdef FEATURE_TDSCDMA
  if((reg_mode_is_usim_compliant_for_cm((sys_modem_as_id_e_type)mm_sub_id) == TRUE )&& 
     (emm_ctrl_data_ptr->emm_tau_attempt_counter == 0))
  {
     emm_set_tau_trm_priority(TRUE);
  }
#endif

  
#ifdef FEATURE_DUAL_DATA
  if(*emm_ctrl_data_ptr->is_volte_only_ps == TRUE && (emm_ctrl_data_ptr->emm_tau_attempt_counter == 0))
  {
    emm_set_tau_trm_priority(TRUE);
  }
#endif

  if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number, 
                                MSGR_NAS_EMM,
                                emm_tau_request_succeeded, 
                                emm_tau_request_failed, 
                                emm_ctrl_data_ptr))
  {
    /* Get MME routing info type */
    routing_info = emm_get_tau_req_mme_routing_info(emm_ctrl_data_ptr);

    /* TAU REQ was sent in IDLE mode start the timer on TAU Accept */
    emm_ctrl_data_ptr->start_t3440_on_accept = TRUE;

    if (FALSE == emm_connection_setup(conn_est_cause, 
                                      emm_ctrl_data_ptr->out_msg_ptr, 
                                      sequence_number, 
                                      MSGR_NAS_EMM, 
                                      emm_tau_request_succeeded, 
                                      emm_tau_request_failed, 
                                      routing_info,
                                      NULL,
                                      emm_ctrl_data_ptr))
    {
        MSG_ERROR_DS_1(MM_SUB, "=EMM= TAU attempt failed due to Access Class barring %d, T3430 has been stopped",
                       emm_ctrl_data_ptr->emm_attach_attempt_counter+1);
        mm_stop_timer(TIMER_T3430);
        
        emm_ctrl_data_ptr->cell_barred_tau_pending = TRUE;
        /* TAU REQ could not be sent, reset the variable */
        emm_ctrl_data_ptr->start_t3440_on_accept   = FALSE;
        emm_set_tau_trm_priority(FALSE);
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_UPDATE_NEEDED) ;
        return;
    }
  }

#ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
  mm_save_rat_pri_list_sent_to_nw();
#endif

  /* EMM shall stop T3402 if running */
  mm_stop_timer(TIMER_T3402);

  /* EMM shall stop T3411 if running */
  mm_stop_timer(TIMER_T3411);

  /* EMM shall stop T3346 */
  mm_stop_timer(TIMER_T3346);

#ifdef FEATURE_NAS_REL10
    gmm_set_gmm_is_congestion_due_to_rau(FALSE);
#endif
  
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

  mm_stop_timer(TIMER_T3430);
  mm_start_timer(TIMER_T3430, EMM_T3430_TIMER_DURATION) ;

#ifdef FEATURE_LTE_REL9
  mm_stop_timer(TIMER_T3442);
#endif

  /* Register connection release client for TAU procedure.
     The client needs to be removed upon TAU request sending failure, 
     TAU accept/reject reception and T3430 expiry */
  emm_add_new_release_client(emm_tau_request_release_indication);

  /* send a SIM update to RRC to start using the latest NAS COUNT
     in KENb calculation, 
     Note: do not move this section of code, since the NAS count is 
     incremented when the message is sent.
    Moved the emm_send_sim_update() to emm_send_msg and emm_connection_setup
  */

} /* end of emm_start_tau_procedure() */

/*===========================================================================

FUNCTION    EMM_TAU_COMPLETE_FAILED

DESCRIPTION
  This function is called when RRC indicates that TAU Complete message 
  delivery has failed.

DEPENDENCIES
  This is a call back function which should not be called by name

RETURN VALUE
  TRUE  - indicates that TAU procedure is required before another EMM 
          or ESM procedures may be resumed
  FALSE - indicates that TAU is not required

SIDE EFFECTS
  This function modifies value of the corresponding boolean flag 
  message_sent_indicator:
    TRUE  - current message CAN be deleted from the cache memory
    FALSE - current message CANNOT be deleted from the cache memory
===========================================================================*/
boolean emm_tau_complete_failed
(
  /* RRC failure cause */
  lte_emm_connection_cause_type   cause,

  /* Message ID that this callback is assosiated with */
  dword                           trans_id,

  /* Failure cause type and cause */
  emm_failure_type                emm_failure_cause,

  /* The following TAI is valid only with 
     TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO */
  lte_nas_tai_lst1_type           tai,

  /* Pointer to the massage which failed to be tx */
  struct emm_pended_msg_list_tag *message_ptr,

  /* EMM control data */
  emm_ctrl_data_type             *emm_ctrl_data_ptr
) 
{
  emm_pended_msg_list_type *pended_message_ptr = (struct emm_pended_msg_list_tag *)message_ptr ;

  ASSERT(pended_message_ptr != (emm_pended_msg_list_type *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  if (!(cause == TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO &&
       cause == TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO &&
       cause == TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO &&
       cause == TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO &&
       cause == TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO)) 
  {
    emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_LOWER_LAYER_FAILURE, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
  }
  /*If blocking of PLMN was kept pending due to TAU complete ack then block the PLMN*/
  emm_ctrl_data_ptr->reg_complete_ack_pending = EMM_PENDING_REG_ACK_NONE;
  if(emm_ctrl_data_ptr->is_plmn_block_req_pending == TRUE)
  {
    emm_set_tau_trm_priority(FALSE);
    emm_process_block_plmn_req(&emm_ctrl_data_ptr->pending_block_plmn_req, emm_ctrl_data_ptr);
    /* Need to stop possible T3402 or T3411*/
    mm_stop_timer(TIMER_T3411);
    mm_stop_timer(TIMER_T3402);      
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
    return FALSE;
  }

  /* Start TAU recovery based on the RRC cause */
  switch (cause)
  {
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO:
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN:
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL:
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_RLF:
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
      /* Check newly reported TAI */
      if (emm_search_tai_list(&tai,emm_ctrl_data_ptr->emm_tai_list_ptr))
      {
        /* 
          Current TAI IS belonging to the stored TAI list.

          Keep update status as EU1 UPDATED as per TS 24.301 5.5.3.2.6
        */
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Reported TAI IS belonging to the TAI list, reTX TAU Complete");

        if (FALSE == emm_send_message(&message_ptr->nas_out_message, 
                                      message_ptr->sequence_number,
                                      MSGR_NAS_EMM,
                                      emm_tau_complete_succeeded, 
                                      emm_tau_complete_failed, 
                                      emm_ctrl_data_ptr))
        {
          /* We should never get about here */
          MSG_FATAL_DS(MM_SUB,"=EMM= TAU COMPLETE has not been delivered due to lack of RRC connection",0,0,0) ;
        }
        emm_ctrl_data_ptr->reg_complete_ack_pending = EMM_PENDING_REG_ACK_TAU_COMPLETE;
      }
      else
      {
        /* 
          Current TAI is NOT belonging to the stored TAI list.

          As per TS 24.301 5.5.3.2.6 i) Restart TAU procedure and change 
          EPS Update Status to EU1 NOT UPDATED
        */
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Reported TAI is NOT belonging to the TAI list, EU1 NOT UPDATED");
        emm_set_tau_trm_priority(FALSE);

        emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED ;
        emm_save_mobility_info(emm_ctrl_data_ptr);
        emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;
        emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_LOWER_LAYER_FAILURE, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
        emm_initiate_tau_procedure(EMM_INITIAL_UPDATE, 
                                   emm_get_eps_update_type(emm_ctrl_data_ptr), 
                                   FALSE, 
                                   emm_ctrl_data_ptr);
      }
      break ;
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ABORTED:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
#ifdef FEATURE_NAS_EAB
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
#endif
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CELL_RESEL:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_SI_FAILURE:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TAI_CHANGED:
      /* Should not happen. RRC problem. 
         Print an error message and do nothing */
      MSG_ERROR_DS_1(MM_SUB, "=EMM= TAU COMPLETE Tx CONN failure - %d", cause);
      break ;
    case CONN_REL_LTE_RRC_CONN_REL_NORMAL:
    case CONN_REL_LTE_RRC_CONN_REL_RLF:
    case CONN_REL_LTE_RRC_CONN_REL_CRE_FAILURE:
    case CONN_REL_LTE_RRC_CONN_REL_OOS_DURING_CRE:
    case CONN_REL_LTE_RRC_CONN_REL_ABORTED:
    case CONN_REL_LTE_RRC_CONN_REL_SIB_READ_ERROR:
    case CONN_REL_LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU COMPLETE tx failure due to CONN REL");
      /* Need to stop possible T3402 or T3411 for combined TAU CS failure */
      mm_stop_timer(TIMER_T3411);
      mm_stop_timer(TIMER_T3402);      
      emm_set_tau_trm_priority(FALSE);
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      break;
    case EMM_SRV_REQ_FAILURE_NO_THROTTLE:
    case EMM_SRV_REQ_FAILURE_THROTTLE:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Unexpected TAU COMPLETE Tx failure - %d", cause);
      break;
    default :
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM connection cause",0,0,0) ;
      break ;
  }

  /* 
    Mark the message as "SENT" letting connection 
    manager to delete this message from the cache 
    memory
  */
  pended_message_ptr->message_sent_indicator = TRUE ;

  /* Do not perform TAU procedure */
  return FALSE ;
}/* end of emm_tau_complete_failed() */


/*===========================================================================

FUNCTION    EMM_PROCESS_TAU_ACCEPT

DESCRIPTION
  This function processes TAU Accept messageas described in TS 24.301 5.5.3.2.4

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function may modify the following global control vars:
    o emm_tau_attempt_counter
    o emm_update_status
    o emm_guti_valid
    o emm_guti
    o emm_equivalent_PLMN_list
    o t3412_val_exists
    o t3412_val
    o loc_area_id
    o tmsi_present
    o tmsi
    o t3402_value
    o t3423_value
===========================================================================*/
void emm_process_tau_accept
(
  lte_nas_emm_tau_accept_type *emm_tau_accept_ptr,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
)
{
  lte_nas_emm_tau_complete  *emm_tau_complete = NULL;
  dword                      prev_t3402_value;

  ASSERT(emm_tau_accept_ptr != (lte_nas_emm_tau_accept_type *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ;
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr->emm_tai_list_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->emergency_num_list_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Received TAU Accept message, T3430 is being stopped");

  mm_stop_timer(TIMER_T3430);
#ifdef FEATURE_LTE_REL10
  /* This flag could be set if TAU is triggered due to paging.
     reset the flag */
  emm_ctrl_data_ptr->responding_to_page = FALSE;
#endif
  emm_remove_release_client(emm_tau_request_release_indication);

  /* NAS security requirement: RAND and RES stored are to be deleted and timer T3416 if running
     must be stopped if there is a TAU ACCEPT
  */
  auth_reset_cache_memory(FALSE, NO_TIMER_EXPIRED, RRC_PS_DOMAIN_CN_ID);
  
  if(emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent == NAS_SEND_DRX_ON_GW_RAT) 
  {
    emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent = NAS_SEND_DRX_ON_ALL_RATS;
  }
  else if(emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent == NAS_SEND_DRX_ON_ALL_RATS)
  {
    emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent = NAS_SEND_DRX_ON_LTE_RAT;
  }

  switch(emm_tau_accept_ptr->eps_update_result)
  {
    case TA_LA_UPDATE:
    case TA_LA_UPDATE_ISR_ACTIVATE:
      if(emm_tau_accept_ptr->emm_cause_exists == TRUE)
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Unexpected cause %d in combined TAU ACCEPT",
                      emm_tau_accept_ptr->emm_cause);
      }
	  /* changing as this point so that ;logging will be taken correctly and also it 
              will fall into PS only attach mode.*/
      if (((emm_tau_accept_ptr->eps_update_result == TA_LA_UPDATE)
          ||(emm_tau_accept_ptr->eps_update_result == TA_LA_UPDATE_ISR_ACTIVATE)) &&
         ((emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == TA_UPDATING)||
          (emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == PERIODIC_UPDATING)) && 
         (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_PS_ONLY ))
      {
        if (emm_tau_accept_ptr->eps_update_result == TA_LA_UPDATE_ISR_ACTIVATE )
        {
          emm_tau_accept_ptr->eps_update_result = TA_UPDATE_ISR_ATIVATE;
        }
        else
        {
          emm_tau_accept_ptr->eps_update_result = TA_UPDATE;
        }
        MSG_HIGH("=EMM= Ongoing TAU Cause = %d TAU EPS update result change to = %d ",
                              emm_ctrl_data_ptr->emm_ongoing_eps_ta_update,
                                      emm_tau_accept_ptr->eps_update_result,0);
      }
      break;
    case TA_UPDATE:
    case TA_UPDATE_ISR_ATIVATE:
      if(emm_tau_accept_ptr->emm_cause_exists == TRUE) 
      {   
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Cause %d in combined TAU ACCEPT",
                      emm_tau_accept_ptr->emm_cause);
      }
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU ACCEPT for EPS only w/o EMM cause");
      }
      break;
    default:
      /* Can not happen. MSG lib would have dropped the message. */
      MSG_FATAL_DS(MM_SUB,"=EMM= Invalid EPS update result %d",
                emm_tau_accept_ptr->eps_update_result,0,0);
      break;
  }

  prev_t3402_value = emm_ctrl_data_ptr->t3402_value;
  emm_ctrl_data_ptr->t3402_attach_rej_param.is_t3402_recv_attach_rej = FALSE;

  /* Obtain T3402 value if avaliable */
  if (emm_tau_accept_ptr->t3402_val_exists == TRUE)
  {
    emm_ctrl_data_ptr->t3402_value = emm_tau_accept_ptr->t3402_val ;
  }
  else
  {
    emm_ctrl_data_ptr->t3402_value = EMM_DEFAULT_T3402;
  }

  /*Notify T3402 clients of the change in T3402 value*/
  if(prev_t3402_value != emm_ctrl_data_ptr->t3402_value)
  {
    emm_send_t3402_changed_ind(emm_ctrl_data_ptr->t3402_value);
  }


#ifdef FEATURE_LTE_REL9
    if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
    {
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
    }
    else
#endif
    {
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                            SYS_SRV_STATUS_SRV;
    }

	if (emm_tau_accept_ptr->emergency_num_list_exists == TRUE)
	 {
	   memset(emm_ctrl_data_ptr->emergency_num_list_ptr,0,sizeof(mmr_emergency_num_list_s_type));
	   memscpy(emm_ctrl_data_ptr->emergency_num_list_ptr,sizeof(mmr_emergency_num_list_s_type),
			  &emm_tau_accept_ptr->emergency_num_list,sizeof(mmr_emergency_num_list_s_type));
	
	   mm_update_mmr_emergency_num_list_if_needed(
									   &emm_tau_accept_ptr->emergency_num_list);
	 }
	 else
	 {
	   mm_nullify_mmr_emergency_num_list_if_needed
	   (
		   &(emm_ctrl_data_ptr->last_registered_tai.plmn.identity[0]),
		   &(emm_ctrl_data_ptr->plmn_service_state->plmn.identity[0])
	   );
	 }

    /* Set Update status to EU1 UPDATED */
    emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_UPDATED ;

    emm_ctrl_data_ptr->manual_user_sel = FALSE;
    emm_set_mm_manual_selection_flag(FALSE);

    /* Allow the DS to start sending data */
    emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend = FALSE ;

    emm_ctrl_data_ptr->last_registered_tai_valid = TRUE ;
    emm_ctrl_data_ptr->last_registered_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn) ;
    emm_ctrl_data_ptr->last_registered_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac ;
    emm_ctrl_data_ptr->rplmn_info.rplmn = emm_ctrl_data_ptr->last_registered_tai.plmn;
    emm_ctrl_data_ptr->rplmn_info.rplmn_type = MM_EPS_RPLMN_TYPE;
#ifdef FEATURE_LTE_REL9 
    if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
    {
       /* Remove the registered PLMN from FPLMN lists if necessary for non emc attach*/
       emm_remove_forbidden_plmn(emm_ctrl_data_ptr->last_registered_tai.plmn,
                              emm_ctrl_data_ptr);
    }
#else
    /* Remove the registered PLMN from FPLMN lists if necessary for non emc attach*/
    emm_remove_forbidden_plmn(emm_ctrl_data_ptr->last_registered_tai.plmn,
                            emm_ctrl_data_ptr);
#endif
#ifdef FEATURE_FEMTO_CSG
    mm_add_csg_to_white_list(emm_ctrl_data_ptr->last_registered_tai.plmn, 
                                      &emm_ctrl_data_ptr->plmn_service_state->csg_info);
    mm_remove_csg_from_rejected_list(
                       emm_ctrl_data_ptr->last_registered_tai.plmn,
                       emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id);
#endif 

    /* Obtain T3412 value if avaliable */
#ifdef FEATURE_LTE_REL10
    if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10 &&
       emm_tau_accept_ptr->t3412_ext_val_exists == TRUE)
    {
      emm_ctrl_data_ptr->t3412_value = emm_tau_accept_ptr->t3412_ext_val ;
    }
    else
#endif
    if(emm_tau_accept_ptr->t3412_val_exists)
    {
      emm_ctrl_data_ptr->t3412_value = emm_tau_accept_ptr->t3412_val ;
    }

    if (emm_tau_accept_ptr->guti_type_exists == TRUE)
    {
      /* Process new GUTI if provided */
      emm_ctrl_data_ptr->emm_guti_valid = TRUE;
      emm_ctrl_data_ptr->emm_guti = emm_tau_accept_ptr->guti ;
      emm_ctrl_data_ptr->plmn_service_state->rac_or_mme_code = emm_ctrl_data_ptr->emm_guti.mme_code;
    }

    /* Process new TAI list if provided */
    if (emm_tau_accept_ptr->tai_lst_exists == TRUE)
    {
      memscpy((void*)emm_ctrl_data_ptr->emm_tai_list_ptr, 
               sizeof(lte_nas_emm_tai_lst_info_type),
               (void*)&(emm_tau_accept_ptr->tai_lst),
               sizeof(lte_nas_emm_tai_lst_info_type));
#ifdef FEATURE_LTE_REL9 
      if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
      {
        emm_send_tai_list_ind(emm_ctrl_data_ptr);
      }
    }

    /* TAU was started IDLE mode. If BEARER_UPDATE_IND is receive
       stop the timer..else release the connection on timer expiry */
    if(emm_ctrl_data_ptr->start_t3440_on_accept == TRUE)
    {
      mm_start_timer(TIMER_T3440,EMM_T3440_TIMER_DURATION);
      MSG_HIGH_DS_0(MM_SUB, "=EMM= T3440 timer started");
      emm_ctrl_data_ptr->start_t3440_on_accept = FALSE;
    }

    /* 
      If EPS Bearer Status is included into TAU Accept message
      EMM sends NAS_ESM_EPS_BEARER_STATUS_IND to ESM
    */
    if (emm_tau_accept_ptr->eps_bearer_context_status_exists == TRUE)
    {
      emm_build_and_send_eps_bearer_status(&emm_tau_accept_ptr->eps_bearer_context_status) ;
    }


    /*If T3423 exists in tau accept use it, otherwise set it to T3412*/
    if (emm_tau_accept_ptr->t3423_val_exists == TRUE)
    {
      emm_ctrl_data_ptr->t3423_value = emm_tau_accept_ptr->t3423_val ;
    }
    else 
    {
      emm_ctrl_data_ptr->t3423_value = emm_ctrl_data_ptr->t3412_value;
    }

    /* REVISIT VITALY
       FOR INTER-RAT cases WE NEED TO SATISFIED THE FOLLOWING :
       "If the UE had initiated TAU due to a change in UE network 
       capability or change in DRX parameters or both, the UE shall
       locally deactivate ISR by setting TIN value to "GUTI"."
    */

    /* Send DRX updates to RRC */
    if(*emm_ctrl_data_ptr->set_drx_req_pending_ptr == TRUE)
    {
      mm_send_mmr_set_drx_cnf(TRUE);
      /* Send DRX update to W-RRC */
      mm_send_rrc_set_drx_ind(); 
      emm_send_rrc_drx_update();
      emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
    }

    /*
      If Equivalent PLMN List is included into TAU Accept 
      message EMM EMM processes new list according to 
      TS 24.301 5.5.3.2.4
    */
    emm_ctrl_data_ptr->update_eplmn_list = TRUE;

    /*We don't need to indicate REG & RRC, of EPLMN list changes here...
    at the end of ATTACH procedure, it gets notified*/
    if (emm_tau_accept_ptr->plmn_exists == TRUE)
    {
      emm_ctrl_data_ptr->emm_equivalent_PLMN_list = 
                                                  emm_tau_accept_ptr->plmn_lst;
    }
    else
    {      
      /*Delete the existing EQ_PLMN list, if it is not present in the TAU_ACCEPT*/
      emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0 ;
    }    

    /* REVISIT
      The network may also indicate in the EPS update result IE in the 
      TRACKING AREA UPDATE ACCEPT message that ISR is active. If the 
      TRACKING AREA UPDATE ACCEPT message contains:
        i)  no indication that ISR is activated, the UE shall set the TIN to "GUTI";
        ii) an indication that ISR is activated, the UE shall regard a previously 
            assigned P-TMSI and RAI as valid and registered with the network. 
            If the TIN currently indicates "P-TMSI", the UE shall set the TIN to 
            "RAT-related TMSI".
    */

    if (emm_tau_accept_ptr->tai_lst_exists)
    {
#ifdef FEATURE_LTE_REL9
      if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
      {
        /* Process new TAI list in order to update forbidden lists */
        emm_process_tai_list(emm_ctrl_data_ptr->emm_tai_list_ptr, emm_ctrl_data_ptr) ;
      }
    }



    /* Reset TAU cause in emm data base */
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;

     EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);

    if(
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
       (reg_sim_per_subs_is_ehplmn(emm_ctrl_data_ptr->plmn_service_state->plmn,(sys_modem_as_id_e_type)mm_sub_id) )
#else
       (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id), emm_ctrl_data_ptr->plmn_service_state->plmn) )
#endif
#else
#ifdef FEATURE_EQUIVALENT_HPLMN
       (reg_sim_is_ehplmn(emm_ctrl_data_ptr->plmn_service_state->plmn) )
#else
       (sys_plmn_match(reg_sim_read_hplmn(), emm_ctrl_data_ptr->plmn_service_state->plmn))
#endif
#endif
      )
    {
      if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE) 
      {
        emm_ctrl_data_ptr->is_accepted_on_lte = TRUE;
      }
    }
    else if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming== TRUE)
    {
      emm_ctrl_data_ptr->emm_lte_accepted_vplmn = emm_ctrl_data_ptr->plmn_service_state->plmn;
    }

     /* From spec 24.301 section 5.5.3.2.4
     ** If the TRACKING AREA UPDATE ACCEPT message contains:
     **    i) no indication that ISR is activated, the UE shall set the TIN to "GUTI";
     **    ii) an indication that ISR is activated, the UE shall regard a previously assigned P-TMSI and RAI as valid and 
     **       registered with the network. If the TIN currently indicates "P-TMSI" and the periodic routing area update timer 
     **       T3312 is running, the UE shall set the TIN to "RAT-related TMSI". If the TIN currently indicates "P-TMSI" 
     **       the periodic routing area update timer T3312 has already expired, the UE shall set the TIN to "GUTI"."
     */
#ifdef FEATURE_LTE_REL9
    MSG_HIGH_DS_3(MM_SUB, "=EMM= Update result %d, EMC srv status %d, TIN type %d",
                  emm_tau_accept_ptr->eps_update_result,
                  emm_ctrl_data_ptr->emc_srv_status,
                  emm_ctrl_data_ptr->TIN);
#endif
#ifdef FEATURE_LTE_REL11
     /*
     ** If LTE CS parameter updated in GMM is different from the current capabilities then
     ** do not enable ISR so that UE performs RAU in GW.
     */
     if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
        (emm_lte_cs_domain_param_match(emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param,
                                      emm_get_gmm_reg_lte_cs_param()) == FALSE) &&
        (emm_tau_accept_ptr->eps_update_result == TA_UPDATE_ISR_ATIVATE || 
         emm_tau_accept_ptr->eps_update_result == TA_LA_UPDATE_ISR_ACTIVATE) &&
        (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY))
     {
       MSG_HIGH_DS_0(MM_SUB, "=EMM= Set TIN to GUTI to update LTE CS param change");
       emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
       mm_stop_timer(TIMER_T3312);
       mm_stop_timer(TIMER_T3323); 
     }
     else
#endif
     if((emm_tau_accept_ptr->eps_update_result == TA_UPDATE_ISR_ATIVATE
        || emm_tau_accept_ptr->eps_update_result == TA_LA_UPDATE_ISR_ACTIVATE) 
       &&
#ifdef FEATURE_LTE_REL9
       (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)
       &&
#endif
        (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI))
     {
#ifdef FEATURE_LTE_REL10
        if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
           (mm_timer_status[TIMER_T3312] == TIMER_STOPPED))
        {
          emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
        }
        else
#endif
        {
          emm_set_and_write_tin_type(NAS_MM_TIN_RAT_RELATED_TMSI);
        }
     }                
     else if(emm_tau_accept_ptr->eps_update_result == TA_UPDATE
          || emm_tau_accept_ptr->eps_update_result == TA_LA_UPDATE)
     {
       /* We may be writing GUTI twice in this. Later on see if writing twice can be eliminated*/
        emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
        mm_stop_timer(TIMER_T3312);
        mm_stop_timer(TIMER_T3323); 
     }

    if ((emm_tau_accept_ptr->eps_update_result == TA_UPDATE) || 
        (emm_tau_accept_ptr->eps_update_result == TA_UPDATE_ISR_ATIVATE))
    {
      /* Combined TAU CS rejected */
      if (emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == COMBINED_TA_LA_UPDATING ||
          emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == COMBINED_TA_LA_UPDATING_IMSI_ATTACH)
      {

        nas_emm_tau_cs_rej = TRUE;
        nas_emm_tau_cs_rej_tau_type = emm_ctrl_data_ptr->emm_ongoing_eps_ta_update;
        MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU for CS failed");
        if(emm_tau_accept_ptr->emm_cause_exists == FALSE)
        {
          emm_tau_accept_ptr->emm_cause = LTE_NAS_NO_FAILURE;
        }
        emm_process_cs_reject_cause(emm_tau_accept_ptr->emm_cause, 
                                    emm_ctrl_data_ptr);
      }
      /* PS only TAU */
      else
      {
        emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
        /* No need to update registered service domain for periodic TAU. Keep
           the existing registered service domain since periodic will only have
           TA_UPDATING no matter UE is PS only or combined attached. */
        if(emm_ctrl_data_ptr->emm_ongoing_eps_ta_update != PERIODIC_UPDATING)
        {
          emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                        SYS_SRV_DOMAIN_PS_ONLY;
          emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_PS_ONLY;
          emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
        }
        if(emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS)
        {
           emm_ctrl_data_ptr->plmn_service_state->cs_service_status = SYS_SRV_STATUS_SRV;
        }
        else if(emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY)
        {
           emm_ctrl_data_ptr->plmn_service_state->cs_service_status = SYS_SRV_STATUS_NO_SRV;
        }
      }
    }
    else
    {
      mm_update_cs_update_success_status(
                          (lte_nas_emm_incoming_msg_type *)emm_tau_accept_ptr);

      /* Reset TAU attempt counter */
      emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;

      emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                          SYS_SRV_DOMAIN_CS_PS;
      emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_CS_PS;

      emm_ctrl_data_ptr->plmn_service_state->cs_service_status = 
                                                            SYS_SRV_STATUS_SRV;

      if(emm_tau_accept_ptr->additional_update_result_exists)
      {
        emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability = 
                            emm_tau_accept_ptr->additional_update_result;
      }
      else if(((emm_tau_accept_ptr->eps_update_result == TA_LA_UPDATE)
              ||(emm_tau_accept_ptr->eps_update_result == TA_LA_UPDATE_ISR_ACTIVATE))&&
               (emm_ctrl_data_ptr->reg_req_pending_ptr->sms_only == TRUE))
      {
        emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability = SYS_LTE_CS_CAPABILITY_SMS_ONLY;
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Overwriting CS capability with SMS only");
      }
      else
      {
        emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability = SYS_LTE_CS_CAPABILITY_NO_VALUE;
      }
    }


    if(emm_tau_accept_ptr->eps_nw_feature_support_exists == TRUE)
    {
      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.ims_vops_supported = 
                                       emm_tau_accept_ptr->eps_nw_feature_support.ims_vops_supported;
#ifdef FEATURE_LTE_REL9
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
      {
        emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_last_eutran_voims_supported = 
                                           emm_tau_accept_ptr->eps_nw_feature_support.ims_vops_supported;
      }
#endif
#ifdef FEATURE_LTE_REL9
    if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
    {
      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.lcs_supported = 
                                       emm_tau_accept_ptr->eps_nw_feature_support.lcs_supported;
      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.emc_bs_supported = 
                                       emm_tau_accept_ptr->eps_nw_feature_support.emc_bs_supported;
      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.cs_lcs_val = 
                                       emm_tau_accept_ptr->eps_nw_feature_support.cs_lcs_val;
#ifdef FEATURE_LTE_REL10
      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.esr_ps_supported = 
                                     emm_tau_accept_ptr->eps_nw_feature_support.esr_ps_supported;
#endif
     }
#endif      
    }
    else
    {
      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.ims_vops_supported = FALSE;
#ifdef FEATURE_LTE_REL9
    if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
    {
      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.lcs_supported = FALSE;
      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.emc_bs_supported = FALSE;
      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.cs_lcs_val = SYS_CS_LCS_INFO_NOT_AVAILBLE;
#ifdef FEATURE_LTE_REL10
      emm_ctrl_data_ptr->plmn_service_state->eps_nw_feature_support.esr_ps_supported = FALSE;
#endif
     }
#endif      
#ifdef FEATURE_LTE_REL9
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
      {
        emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_last_eutran_voims_supported = FALSE;
      }
#endif
    }

    /* Update EMM USIM */
    emm_save_mobility_info(emm_ctrl_data_ptr) ;

    /* Inform RRC regarding change of registration info */
    emm_send_rrc_system_update(emm_ctrl_data_ptr) ;

    /* Update current lte_cs_domain_param */
    emm_ctrl_data_ptr->reg_lte_cs_domain_param = 
           emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param;
 
    *(emm_ctrl_data_ptr->reg_lte_pended_cs_domain_param_ptr) = 
	       emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param;
 
    /* 
      If the TRACKING AREA UPDATE ACCEPT message contained a GUTI, 
      the UE shall return a TRACKING AREA UPDATE COMPLETE message
    */
    if((emm_tau_accept_ptr->guti_type_exists == TRUE) ||
       (emm_tau_accept_ptr->ms_id_exists == TRUE))
    {
      dword sequence_number = 0 ;

      emm_ctrl_data_ptr->reg_complete_ack_pending = EMM_PENDING_REG_ACK_TAU_COMPLETE;

      /* Update REG that current TAU has been finished successfully */
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

      /* Inform RRC that GUTI has been changed */
      emm_send_sim_update(emm_ctrl_data_ptr) ;

      memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
             0x0,
             sizeof(lte_nas_outgoing_msg_type)) ;

      emm_tau_complete = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.tau_complete;

      emm_tau_complete->lte_nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES ;

      emm_tau_complete->lte_nas_hdr.msg_id = TRACKING_AREA_UPDATE_COMPLETE ;

      emm_tau_complete->security_hdr = PLAIN_NAS_MSG ;

      /* Obtain sequence number */
      sequence_number = emm_get_nas_transaction_id() ;

      MSG_HIGH_DS_0(MM_SUB, "=EMM= Send TAU COMPLETE message to NW");

      if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                    sequence_number,
                                    MSGR_NAS_EMM,
                                    emm_tau_complete_succeeded, 
                                    emm_tau_complete_failed, 
                                    emm_ctrl_data_ptr))
      {
        MSG_ERROR_DS_0(MM_SUB, "=EMM= TAU COMPLETE has not been delivered due to lack of RRC connection");
      }
    }
    else
    {
          /* Update REG that current TAU has been finished successfully */
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
      if (((emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == COMBINED_TA_LA_UPDATING ||
          emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == COMBINED_TA_LA_UPDATING_IMSI_ATTACH)&&
         (emm_tau_accept_ptr->eps_update_result == TA_LA_UPDATE || 
          emm_tau_accept_ptr->eps_update_result == TA_LA_UPDATE_ISR_ACTIVATE))||
         (emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == TA_UPDATING &&
         (emm_tau_accept_ptr->eps_update_result == TA_UPDATE ||
          emm_tau_accept_ptr->eps_update_result == TA_UPDATE_ISR_ATIVATE)) ||
         (emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == PERIODIC_UPDATING))
      {
        emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_ACCEPT, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
      }
      else 
      {
        emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_ACCEPT_PS_ONLY, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
      }
      /* No need to send TAU Complete message. TAU is completed. */
      /* Check if need to update the network selction mode */
      emm_update_nw_selection_mode(emm_ctrl_data_ptr);
      if(emm_ctrl_data_ptr->tau_pending_gw_attach == TRUE)
      {
        emm_ctrl_data_ptr->tau_pending_gw_attach = FALSE;  
      }
      if(emm_ctrl_data_ptr->radio_cap_update != EMM_RADIO_CAP_UPDATE_NONE)
      {
        emm_ctrl_data_ptr->radio_cap_update = EMM_RADIO_CAP_UPDATE_NONE;  
      }
      
      if((emm_ctrl_data_ptr->sg_association_to_be_established != NAS_EMM_SG_ASSOC_NOT_REQ)
         &&
         (emm_ctrl_data_ptr->srv_domain_registered == SYS_SRV_DOMAIN_CS_PS))
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Reset SGs assoc flag - Combined TAU ACCEPT");
        emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_NOT_REQ; 
      }             
      
      /* Update GW flag(s) */          
      emm_attach_tau_success_update_gw();                   
      emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;
      emm_set_tau_trm_priority(FALSE);

      if(mm_timer_status[TIMER_T3440] == TIMER_STOPPED)
      {
        /* Send out all pended messages */
        emm_send_all_pended_messages(emm_ctrl_data_ptr); 

        /* Send out all pending IRAT uplink messages */
        emm_send_all_pending_irat_msg(emm_ctrl_data_ptr);
      }
      else if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING)
      {
        emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,
                                 LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
      }
      emm_send_tau_complete_ind();
    }

    /* NONCE UE is no longer needed after getting TAU ACCEPT. 
       Reset nonce ue in EMM data base */
    memset((void *)emm_ctrl_data_ptr->nonce_ue, 
           0x0, 
           sizeof(emm_ctrl_data_ptr->nonce_ue));

    if(emm_ctrl_data_ptr->ongoing_detach_procedure == TRUE)
    {
      /* 5.5.2.2.4 a: Restart detach when detach abort for cell change*/
      emm_start_mo_detach(emm_ctrl_data_ptr->ongoing_detach_type, 
                          FALSE, 
                          NULL, 
                          emm_detach_failed_routine, 
                          emm_ctrl_data_ptr);
    }
    else if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED) || 
            (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED))
    {
      if(mm_timer_status[TIMER_T3440] == TIMER_STOPPED)
      {
        emm_send_esr_if_buffered(emm_ctrl_data_ptr);
      }
      else if((emm_ctrl_data_ptr->pended_esr_msg->emm_outgoing_msg.ext_service_req.service_type 
               == LTE_MO_CSFB_ECALL_1XCSFB_ECALL) &&
              (emm_ctrl_data_ptr->reg_complete_ack_pending == EMM_PENDING_REG_ACK_NONE))

      {
        emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,
                                 LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
      }
      else
      {
        MSG_HIGH_DS_0(MM_SUB,"=EMM= Keep caching ESR - T3440 active or TAU COMPLETE pending");
      }
    }
} /* end of emm_process_tau_accept() */

/*===========================================================================

FUNCTION    EMM_PROCESS_TAU_REJECT

DESCRIPTION
  This function processes TAU Reject messageas described in TS 24.301 5.5.3.2.5

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  EMM changes state according to the specified cause (see TS 24.301 5.5.3.2.5)
===========================================================================*/
void emm_process_tau_reject
(
  lte_nas_emm_tau_rej         *tau_rej_ptr,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
)
{
  ASSERT(tau_rej_ptr != (lte_nas_emm_tau_rej *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Received TAU Reject message, T3430 is being stopped");

  emm_cancel_authentication();

  auth_reset_cache_memory(FALSE, NO_TIMER_EXPIRED, RRC_PS_DOMAIN_CN_ID);

  mm_stop_timer(TIMER_T3430);

  emm_remove_release_client(emm_tau_request_release_indication);

  emm_process_reject_message((lte_nas_emm_incoming_msg_type *)tau_rej_ptr,emm_ctrl_data_ptr) ;

  emm_set_tau_trm_priority(FALSE);
} /* end of emm_process_tau_reject() */

/*===========================================================================

FUNCTION    EMM_ENFORCE_NORMAL_SERVICE

DESCRIPTION
  This function enforces transition into NORMAL SERVICE substate of the 
  REGISTERED state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  o EMM moves into EMM_MOVES_TO_REGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE)
  The following Global Control Vars are updated:
    o last_registered_tai_valid
    o last_registered_tai
    o plmn_service_state
===========================================================================*/
void emm_enforce_normal_service
(
  lte_nas_tai_lst1_type *current_tai,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  ASSERT(current_tai != (lte_nas_tai_lst1_type *)NULL) ;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ;

  /* Update Last Registered TAI */
  emm_ctrl_data_ptr->last_registered_tai_valid = TRUE ;
  emm_ctrl_data_ptr->last_registered_tai       = *current_tai ;

  /* Update current PLMN Service state */
  emm_ctrl_data_ptr->plmn_service_state->plmn = current_tai->plmn ;

#ifdef FEATURE_LTE_REL9
  if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
  {
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
  }
  else
#endif
  {
    emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_SRV;
  }

  /* reg service domain may be changed at GW side during IRAT. Set it back to
     the last registered service domain on LTE. */
  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                      emm_ctrl_data_ptr->srv_domain_registered;

  if((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS)&&
      (emm_ctrl_data_ptr->plmn_service_state->cs_service_status == SYS_SRV_STATUS_LIMITED_REGIONAL)&&
      (emm_ctrl_data_ptr->plmn_service_state->service_status == SYS_SRV_STATUS_SRV))
  {
    emm_ctrl_data_ptr->plmn_service_state->cs_service_status = SYS_SRV_STATUS_SRV;
  }
  else if ((mm_timer_status[TIMER_T3411] != TIMER_ACTIVE) &&
      (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == 
                                                       SYS_SRV_DOMAIN_CS_PS) &&
	  (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                       SYS_SRV_DOMAIN_PS_ONLY) &&
      (emm_ctrl_data_ptr->plmn_service_state->cs_service_status == SYS_SRV_STATUS_LIMITED_REGIONAL)&&
      (emm_ctrl_data_ptr->plmn_service_state->service_status == SYS_SRV_STATUS_SRV))
  {
    emm_ctrl_data_ptr->plmn_service_state->cs_service_status = SYS_SRV_STATUS_NO_SRV;
  }

#ifdef FEATURE_1XSRLTE
    if((emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == FALSE)
        ||(emm_ctrl_data_ptr->srlte_tau_needed == FALSE))

#endif
    {
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_UPDATED ;
    }

    /* Allow the DS to start sending data */
    emm_ctrl_data_ptr->plmn_service_state->ps_data_suspend = FALSE ;

    /* Update REG that current TAU has been finished successfully */
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

    /* Inform RRC regarding change of registration info */
    emm_send_rrc_system_update(emm_ctrl_data_ptr) ;
  
    /* Update EMM USIM */
    emm_save_mobility_info(emm_ctrl_data_ptr) ;

    EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE) ;

    /* Check if need to update the network selction mode */
    emm_update_nw_selection_mode(emm_ctrl_data_ptr);

} /* end of emm_enforce_normal_service() */

/*===========================================================================

FUNCTION    EMM_TIMER_T3423_EXPIRE

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void emm_timer_t3423_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  MSG_HIGH_DS_0(MM_SUB, "=EMM= T3423 timer expired, Disabling or locally deactivating ISR");

  if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
  {
    emm_set_lte_deact_isr_timer_expired(TRUE);
  }
  /* Set TIN to P-TMSI so that TAU is performed when service is acquired in LTE */
  emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);

  /* 24.301 5.3.5 Set the flag to start combined TAU IMSI attach later when
     UE goes back to normal service */
  if(emm_ctrl_data_ptr->srv_domain_registered == SYS_SRV_DOMAIN_CS_PS)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Set SGs association flag - T3423 expiry");
    emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
  }
}

/*===========================================================================

FUNCTION    EMM_FORBIDDEN_TAI_CLEAR_TIMER_EXPIRY

DESCRIPTION
  This function is called upon Forbidden LAI TIMER. which is started
  when the first forbidden plmn is added.
  On expiry clears the forbidden TAIs for service adn roaming list. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void emm_forbidden_tai_clear_timer_expiry
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  if((emm_ctrl_data_ptr->forbidden_for_service_list_ptr == NULL) ||
     (emm_ctrl_data_ptr->rrc_forbidden_list_ptr == NULL))
  {
    MSG_ERROR_DS_0(MM_SUB,"=EMM= Wrong sub this should not happen, return from function emm_forbidden_tai_clear_timer_expiry");
    return;
  }
  MSG_HIGH_DS_0(MM_SUB, "=EMM= FORBIDDEN TAI timer expired, clear roaming, serving and manual ftai list");

  if(emm_ctrl_data_ptr->rrc_active == TRUE)
  {
    lte_nas_tai_lst1_type  serving_tai;
    serving_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
    serving_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;
    if((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                       SYS_SRV_STATUS_LIMITED ||
        emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                SYS_SRV_STATUS_LIMITED_REGIONAL) &&
       (emm_search_tai_list2(serving_tai,emm_ctrl_data_ptr->forbidden_for_service_list_ptr) == TRUE) &&
       ((EMM_GET_STATE() == EMM_REGISTERED)||
        (EMM_GET_STATE() == EMM_DEREGISTERED)) &&
       (reg_service_report == (emm_reg_service_report_type)emm_send_reg_ind))
    {
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                   SYS_SRV_STATUS_LIMITED;
      reg_service_report(MMR_CAUSE_FAILURE_MAX_ATTEMPTED, FALSE, emm_ctrl_data_ptr);
    }
  }
  /* Clears the entire list and set the length to 0 */
  memset((void *)emm_ctrl_data_ptr->forbidden_for_service_list_ptr,
                           0x0, sizeof(lte_nas_tai_lst2_type ));

  memset((void *)emm_ctrl_data_ptr->rrc_forbidden_list_ptr,
                           0x0, sizeof(lte_nas_tai_lst2_type ));

  emm_clear_manual_ftai_list();

  /*send updated information to RRC */
  emm_send_rrc_system_update(emm_ctrl_data_ptr);
}

/*===========================================================================

FUNCTION    EMM_PROCESS_UPDATE_FTAI_LIST_CMD

DESCRIPTION
  This function add PLMNs read from EFS to FTAI list 

DEPENDENCIES
  This function will be called when NAS recevies MMCM_RPM_UPDATE_FTAI_LIST
  This message will be sent before SIM available request with SYS_RPM_FTAI_LIST_READ when one of tai 
  counters reaches maximum or with SYS_RPM_FTAI_LIST_CLEAR when max log timer expires in CM.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void emm_process_update_ftai_list_cmd
(
  cm_mm_update_ftai_list_T  *update_ftai_list_ptr,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  MSG_HIGH_DS_1(MM_SUB,"=EMM= MMCM_RPM_UPDATE_FTAI_LIST ftai_list_action %d",update_ftai_list_ptr->ftai_list_action);
  if(update_ftai_list_ptr->ftai_list_action == SYS_RPM_FTAI_LIST_CLEAR)
  {
    emm_clear_rpm_ftai_list(TRUE,emm_ctrl_data_ptr);
    /*send updated information to RRC */
    emm_send_rrc_system_update(emm_ctrl_data_ptr);
  }
  else if(update_ftai_list_ptr->ftai_list_action == SYS_RPM_FTAI_LIST_READ)
  {
    mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
    cmrpm_lte_efs_rpm_info_s_type rpm_info;
    uint8 index, count;
    lte_nas_tai_lst1_type tai;
    count = 0;
    status = mcfg_fs_read("/nv/item_files/modem/lte/cmrpm_lte_efs_rpm_info", 
                        &rpm_info, 
                        sizeof(cmrpm_lte_efs_rpm_info_s_type),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);
    if(status == MCFG_FS_STATUS_OK)
    {
      for(index = 0; index < CM_MAX_NUM_OF_TAC; index++)
      {
        tai.plmn = rpm_info.emm_15_reject[index].lte_tai.plmn;
        tai.tac = rpm_info.emm_15_reject[index].lte_tai.tac;
        if((rpm_info.emm_15_reject[index].counter != 0) &&
           (is_tai_invalid(tai) == FALSE))
        {
          emm_add_to_rpm_ftai_list(tai,emm_ctrl_data_ptr);
          count++;
        }
      }
    }
    MSG_HIGH_DS_2(MM_SUB,"=EMM= RPM UPDATE FTAI LIST EFS Success (0)/ Failure(non zero)= %d. COUNT of FTAI added = %u",
          status,count);
  }
}


/*===========================================================================

FUNCTION    EMM_PROCESS_UE_CAPABILITIES_INFO

DESCRIPTION
  This function set NV items related with srvcc 

DEPENDENCIES
  CM should send this command only if air plane mode on

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void emm_process_ue_capabilities_info
(
  mm_cmd_type  *cmd_ptr,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;
  ASSERT(emm_ctrl_data_ptr != NULL);
  ASSERT(cmd_ptr != NULL);

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Received cmd from CM about UE capabilities info srvcc %d",
                cmd_ptr->cmd.cm_mm_ue_capabilities_info.srvcc_on_fly_value);

  emm_ctrl_data_ptr->l2g_srvcc_capability_support = 
            cmd_ptr->cmd.cm_mm_ue_capabilities_info.srvcc_on_fly_value;

  emm_ctrl_data_ptr->srvcc_capability_support = 
            cmd_ptr->cmd.cm_mm_ue_capabilities_info.srvcc_on_fly_value;

  emm_send_rrc_ue_cap_change_info( cmd_ptr,emm_ctrl_data_ptr);

  efs_status = mcfg_fs_write("/nv/item_files/modem/nas/nas_l2g_srvcc_support" , 
                             &cmd_ptr->cmd.cm_mm_ue_capabilities_info.srvcc_on_fly_value, 
                             sizeof(boolean),
                             MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR,
                             MCFG_FS_ALLPERMS,
                             MCFG_FS_TYPE_EFS,
                            (mcfg_fs_sub_id_e_type)mm_sub_id);

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Write to EFS /nv/item_files/modem/nas/nas_l2g_srvcc_support file SUCCESS(0)/FAIL(non zero) = %d",efs_status);

  efs_status = mcfg_fs_write("/nv/item_files/modem/nas/nas_srvcc_support" , 
                             &cmd_ptr->cmd.cm_mm_ue_capabilities_info.srvcc_on_fly_value, 
                             sizeof(boolean),
                             MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR,
                             MCFG_FS_ALLPERMS,
                             MCFG_FS_TYPE_EFS,
                            (mcfg_fs_sub_id_e_type)mm_sub_id);

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Write to EFS /nv/item_files/modem/nas/nas_srvcc_support file SUCCESS(0)/FAIL(non zero) = %d",efs_status);

}

#ifdef FEATURE_1XSRLTE
/*===========================================================================

FUNCTION    EMM_PROCESS_CM_DUAL_RX_TAU_CMD

DESCRIPTION
  This function initiates TAU procedure

DEPENDENCIES
  CM should send this command only if DUAL RX CSFB is in operation.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void emm_process_cm_dual_rx_tau_cmd(emm_ctrl_data_type   *emm_ctrl_data_ptr)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Received cmd from CM to trigger TAU for dual RX CSFB");

  if(emm_ctrl_data_ptr->srlte_tau_needed == TRUE)
  {
    if((EMM_GET_STATE() == EMM_REGISTERED) && 
    (EMM_GET_SUBSTATE() == (emm_substate_type)EMM_REGISTERED_NORMAL_SERVICE) &&
    (emm_ctrl_data_ptr->plmn_service_state->service_status == SYS_SRV_STATUS_SRV) &&
    (emm_registration_allowance_chk(emm_ctrl_data_ptr->last_rrc_service_ind_ptr,emm_ctrl_data_ptr) == TRUE) &&
    (emm_ctrl_data_ptr->rrc_active == TRUE) &&
    (mm_timer_status[TIMER_T3346] != TIMER_ACTIVE))
    {
      emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                                 emm_get_eps_update_type(emm_ctrl_data_ptr),
                                 FALSE, 
                                 emm_ctrl_data_ptr);
    }
    else
    {
       MSG_HIGH_DS_3(MM_SUB,"=EMM= TAU delayed - EMM state %d, substate %d, RRC status %d", 
                     EMM_GET_STATE(), EMM_GET_SUBSTATE(), emm_ctrl_data_ptr->rrc_active);
       if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_INVALID)
       {
         emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
       }
    }
    /*reset it back to FALSE*/
    emm_ctrl_data_ptr->srlte_tau_needed = FALSE;
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= SRLTE TAU is not needed as ESR Flag is set as %d",emm_ctrl_data_ptr->srlte_tau_needed);
  }
}
#endif /*FEATURE_1XSRLTE*/

#ifdef FEATURE_SGLTE
/*===========================================================================
FUNCTION    EMM_TIMER_DELAY_TAU_EXPIRE

DESCRIPTION
  start TAU on timer expiry. Started on SRVCC HO in SGLTE config

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
void emm_timer_delay_tau_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
   if((EMM_GET_STATE() == EMM_REGISTERED && 
      (EMM_GET_SUBSTATE() == EMM_REGISTERED_NORMAL_SERVICE || 
       EMM_GET_SUBSTATE() == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM))
     && 
     (emm_ctrl_data_ptr->rrc_active == TRUE && 
      emm_ctrl_data_ptr->reg_req_pended_indication == FALSE) && 
     (emm_registration_allowance_chk(emm_ctrl_data_ptr->last_rrc_service_ind_ptr,
                                    emm_ctrl_data_ptr) == TRUE))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU ");
    emm_initiate_tau_procedure(EMM_INITIAL_UPDATE, 
                               emm_get_eps_update_type(emm_ctrl_data_ptr), 
                               FALSE, 
                               emm_ctrl_data_ptr);
  }
}
#endif 

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION    emm_timer_policy_change_pending_expire

DESCRIPTION
 Send service indication to REG to start PLMN seleciton

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
void emm_timer_policy_change_pending_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  if(emm_ctrl_data_ptr->plmn_service_state->service_status != SYS_SRV_STATUS_NO_SRV)
  {
    reg_service_report(MMR_CAUSE_PLMN_SEL_CALL_END,
                                 FALSE,
                                 emm_ctrl_data_ptr);
  }

}

#endif 
#endif /*FEATURE_LTE*/



