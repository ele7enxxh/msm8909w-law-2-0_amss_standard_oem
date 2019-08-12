
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_procedure_reject_handler.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/22/11   MNK     Fix CR 320588: Update the EMM state after service reject is received for cause codes not mentioned in the spec for a CSFB call. 
08/01/11   Rajesh  T3442 timer related code changes
08/22/11   MNK     CR: 301219 Handle CSFB call if TAU is rejected/failed  
04/01/2011 MNK     Enhance error handling for CSFB 
12/27/10   MNK     Fix CR: 268979 
10/07/10   MNK     1XCSFB introduction 
09/23/10   MNK     Handle Extended service reject for CSFB flows 
06/03/10   zren    Updated to send failure indication to DS before reattach 
                   when service reject message with specific causes is rcved
05/19/10   zren    Added support for GSTK EMM REGISTERED EVT and NW REJ EVT
05/18/10   zren    Updated fun param to set only_for_reg flag in REG CNF and 
                   REG IND
05/14/10   RI      Mainlined spec changes upto Dec 09.
05/07/10   zren    Added temp FPLMN feature
                   Fixed and consolidated equivalent PLMN list handling
04/28/10   zren    Updated to send detach indication to ESM when TAU/Service 
                   Request is rejected and EMM moves to DEREGISTERED state 
                   Updated to not reporting no service to REG if  attach is 
                   needed when TAU/Service Request is rejected 
04/14/10   zren    Consolidated GMM/MM handling code for combined procedures to
                   mm_multimode_hanlder
04/09/10   zren    Added combined procedure feature
03/16/10   MNK     More i-RAT changes 
03/12/10   zren    Updated the processing of SERVICE_REJECT message with 
                   abnormal causes 
03/10/10   zren    Updated SERVICE_REJECT message processing
02/23/10   RI      Added saving of security context for implicit detach
02/17/10   MNK     Re-org the reject cause code to be able to re-use for i-RAT
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
09/24/09   zren    Fixed compilation warnings due to missing default in switch
                   statement
09/17/09   zren    Updated to cache esm data req after service request procedure
                   is started in registered state w/o connection
                   Updated to only use ESM transaction ID in service request
                   procedure/send service request failure ind for MO Data type 
                   service request procedure
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/14/09   zren    Modified procedure connection release handling
06/31/09   hnam    Added support for RRC to handle camping in case of TAU 
                    REJECT (with a cause value LTE_NAS_TRACKING_AREA_NOT_ALLOWED)
06/23/09   zren    Added support of processing EPLMN list, forbidden PLMN list,
                   forbidden TAI list for roaming and forbidden TAI list for
                   regional provision of service 
06/22/09   zren    Updated to not release signaling connection upon receiving
                   service reject message with abnormal emm causes
05/29/09   zren    Added support for TAU feature
05/14/09   zren    Changed to include "emm_esm_handler.h" instead of using 
                   extern function prototype
04/21/09   hnam    Added T3440 timer support (required during attach and service reject)
03/04/09   vdr     Now updating plmn_service_state upon reception of the Service
                   Reject message
04/02/09   zren    Added support for new trnsaction ID format 
03/26/09   RI      Added support for GS Timers in LTE.
03/24/09   zren    Created for service request and paging feature
04/14/09   RI      Reset cached authentication values on SERVICE REJECT.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "emm_utility.h"
#include "emm_database.h"
#include "lte_nas_common_v.h"
#include "emm_sim_handler.h"
#include "lte_nas_local_cause.h"
#include "emm_update_lib.h"
#include "emm_procedure_reject_handler.h"
#include "emm_service_request_lib.h"
#include "mm_v.h"
#include "emm_timer_sigs.h"
#include "emm_esm_handler.h"
#include "mmauth.h"
#include "emm_security.h"
#include "err.h"
#include "mm_multimode_common.h"
#include "reg_sim_v.h"
#include "emm_csfb_handler.h"
#include "reg_timers.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===========================================================================

FUNCTION    EMM_TAU_RAU_SR_REJECT_INVALIDATE_MOBILITY

DESCRIPTION
  This function processes invalidate mobility con text according to EMM cause

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
void emm_tau_rau_sr_reject_invalidate_mobility
(
  lte_nas_message_id       msg_id,   /* Will be used when TAU feature is implemented */ 
  lte_nas_emm_cause_type   emm_cause,
  emm_ctrl_data_type       *emm_ctrl_data_ptr,
  boolean                  delete_EPLMN_list
)
{
  sys_plmn_id_s_type	current_lte_plmn;
 
  switch (emm_cause)
  {
    case LTE_NAS_ILLEGAL_UE:
    case LTE_NAS_ILLEGAL_ME:
      emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                   SYS_SIM_STATE_CS_PS_INVALID, 
                                   emm_ctrl_data_ptr, delete_EPLMN_list);
      break ;
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
      if(msg_id == TRACKING_AREA_REJECT)
      {
         if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE)            
         {
            current_lte_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                    emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
            MSG_HIGH_DS_2(MM_SUB, "=EMM= Is Accepted On LTE : %d Eps Storage Supported : %d",
                      emm_ctrl_data_ptr->is_accepted_on_lte,
                      emm_ctrl_data_ptr->eps_storage_supported);
            if((emm_ctrl_data_ptr->is_accepted_on_lte==FALSE) && 
               (emm_ctrl_data_ptr->eps_storage_supported == FALSE)&&
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
                                       emm_ctrl_data_ptr, FALSE) ;
           }
           else
           {
             emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED,
                                       SYS_SIM_STATE_CS_PS_INVALID,
                                       emm_ctrl_data_ptr, delete_EPLMN_list);
           }
        }
        else
        {
          emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED,
                                       SYS_SIM_STATE_CS_PS_INVALID,
                                       emm_ctrl_data_ptr, delete_EPLMN_list);
        }
      }
      break ;

    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
      emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                   SYS_SIM_STATE_PS_INVALID, 
                                   emm_ctrl_data_ptr, delete_EPLMN_list);
      break ;
    case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW:
      emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED, 
                                   SYS_SIM_STATE_AVAILABLE, 
                                   emm_ctrl_data_ptr, delete_EPLMN_list) ;
      break ;
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
      if(msg_id == TRACKING_AREA_REJECT)
      {
        emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                     SYS_SIM_STATE_AVAILABLE, 
                                     emm_ctrl_data_ptr, FALSE) ;
      }
      break ;
    case LTE_NAS_PLMN_NOT_ALLOWED:
      emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                   SYS_SIM_STATE_AVAILABLE, 
                                   emm_ctrl_data_ptr, delete_EPLMN_list) ;
      break ;
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
      emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                                   SYS_SIM_STATE_AVAILABLE, 
                                   emm_ctrl_data_ptr, FALSE) ;
      break ;
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
      /* Update EMM USIM with EPS update status of ROAMING_NOT_ALLOWED */
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_ROAMING_NOT_ALLOWED;
      emm_save_mobility_info(emm_ctrl_data_ptr) ;
      break;
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
    case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
      /* Update EMM USIM with EPS update status of ROAMING_NOT_ALLOWED */
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_ROAMING_NOT_ALLOWED;
      emm_save_mobility_info(emm_ctrl_data_ptr) ;
      break;
#ifdef FEATURE_LTE_REL10
    case LTE_NAS_CONGESTION:
      if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
         (msg_id == TRACKING_AREA_REJECT))
      {
        emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
        emm_save_mobility_info(emm_ctrl_data_ptr) ;
       break;
      }
#endif
    case LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE:
#ifdef FEATURE_LTE_REL9
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL9)
#endif
      {
        /* Update EMM USIM with EPS update status of NOT_UPDATED */
        emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
        emm_save_mobility_info(emm_ctrl_data_ptr) ;
      }
      break;
     case LTE_NAS_SEVERE_NW_FAILURE:
       /*24.301 #42	(Severe network failure);
       The UE shall set the EPS update status to EU2 NOT UPDATED, 
       and shall delete any GUTI, last visited registered TAI, eKSI, 
       and list of equivalent PLMNs*/
       emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED, 
                                   SYS_SIM_STATE_AVAILABLE, 
                                   emm_ctrl_data_ptr, TRUE) ;
      break ;
    default:
      /* Do nothing here for other emm causes */
      break;
   } 
}

/*===========================================================================

FUNCTION  EMM_TAU_SR_REJECT_CHANGE_STATE

DESCRIPTION
  This function updates EMM states depending on EMM cause when TAU or Service
  Request reject is received

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_tau_sr_reject_change_state
(
  lte_nas_message_id      msg_id,
  lte_nas_emm_cause_type  emm_cause,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  emm_failure_type        emm_failure_cause;
  emm_security_context_info  *context_ptr = NULL;

  ASSERT(emm_ctrl_data_ptr != NULL);

  /*initialise it*/
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  /* EMM state change */
  switch (emm_cause)
  {
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
      if(msg_id == TRACKING_AREA_REJECT)
      {
        EMM_MOVES_TO_DEREGISTERED_STATE((emm_substate_type)EMM_DEREGISTERED_PLMN_SEARCH);
        emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);
      }
      break;
    case LTE_NAS_ILLEGAL_UE:
    case LTE_NAS_ILLEGAL_ME:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_SEVERE_NW_FAILURE:
      EMM_MOVES_TO_DEREGISTERED_STATE((emm_substate_type)EMM_DEREGISTERED_PLMN_SEARCH) ;
      emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);
      break ;
    case LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED:
#ifndef FEATURE_LTE_REL10       
      if(msg_id == TRACKING_AREA_REJECT)
      {
        EMM_MOVES_TO_DEREGISTERED_STATE((emm_substate_type)EMM_DEREGISTERED_NORMAL_SERVICE);
      }
      break;
#endif
    case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW:
    case LTE_NAS_IMPLICITLY_DETACHED : 
     /*if LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATE belong to service reject 
       it will fall to the else part */
#ifdef FEATURE_LTE_REL10 
      if((msg_id == TRACKING_AREA_REJECT) &&
         (emm_cause ==LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED))
      {
        EMM_MOVES_TO_DEREGISTERED_STATE((emm_substate_type)EMM_DEREGISTERED_NORMAL_SERVICE);
      }
      else
#endif
      {
      if(msg_id == SERVICE_REJECT)
      {
        emm_failure_cause.cause_type      = LTE_NAS_IRAT_SR_REJECT;
        emm_failure_cause.cause.emm_cause = emm_cause;
        /* If service request is initiated by DS, need to send failure ind to
           DS before starting reattach */
        emm_process_service_req_failure(LTE_NAS_SERVICE_REQ_FAILED,
                                        emm_cause,
                                        emm_failure_cause,
                                        emm_ctrl_data_ptr);
      }

      EMM_MOVES_TO_DEREGISTERED_STATE(
                           (emm_substate_type)EMM_DEREGISTERED_NORMAL_SERVICE);
      emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);

      context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr,
                                             CONTEXT_STATE_CURRENT);

#ifdef FEATURE_LTE_REL10 
         if(((emm_cause == LTE_NAS_IMPLICITLY_DETACHED)||
            ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
             (emm_cause ==LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED))) && 
#else 
      if((emm_cause == LTE_NAS_IMPLICITLY_DETACHED) && 
#endif
         (context_ptr != NULL))
      {
        if ( context_ptr->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS) 
        {
           if (emm_ctrl_data_ptr->eps_storage_supported)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= eps_storage_supported supported, update context in USIM");
            emm_update_usim_eps_security_context(context_ptr);
          }
          else
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= eps_storage_supported NOT supported, update context in EFS");
            emm_write_efs_security_context(context_ptr);
          }
        }
      }
      }
      break ;
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
      EMM_MOVES_TO_DEREGISTERED_STATE((emm_substate_type)EMM_DEREGISTERED_NORMAL_SERVICE);
      emm_send_limited_svc_to_gstk(emm_ctrl_data_ptr);
      break ;
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
      EMM_MOVES_TO_REGISTERED_STATE((emm_substate_type)EMM_REGISTERED_PLMN_SEARCH);
      break;
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
    case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
      EMM_MOVES_TO_REGISTERED_STATE((emm_substate_type)EMM_REGISTERED_LIMITED_SERVICE) ;
      break ;
#ifdef FEATURE_LTE_REL10
     case LTE_NAS_CONGESTION:
       if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
       {
         if(msg_id == SERVICE_REJECT)
         {
           EMM_MOVES_TO_REGISTERED_STATE((emm_substate_type)EMM_REGISTERED_NORMAL_SERVICE);
         }
         else
         {
           EMM_MOVES_TO_REGISTERED_STATE((emm_substate_type)EMM_REGISTERED_ATTEMPTING_TO_UPDATE) ;
         }
       }
       break;
#endif
    case LTE_NAS_CS_FALLBACK_CALL_EST_NOT_ALLOWED:
    case LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE:
    case LTE_NAS_CS_DOMAIN_NOT_AVAILABLE:
    default: /* 5.6.1.5 Abnormal cases in the UE: d - Other service reject values*/
      if (msg_id == SERVICE_REJECT ) 
      {
        EMM_MOVES_TO_REGISTERED_STATE((emm_substate_type)EMM_REGISTERED_NORMAL_SERVICE);
      }
      break;
  }

  /* If EMM state is moved to DEREGISTERED, send detach ind to ESM to clean up */
  if(EMM_GET_STATE() == EMM_DEREGISTERED)
  {
    emm_build_and_send_detach_ind(EMM_DETACHED);
  }
}/* emm_tau_sr_reject_change_state */

/*===========================================================================

FUNCTION    EMM_PROCESS_RAU_SR_REJECT

DESCRIPTION
  This function processes EMM cause received as part of TAU Reject message and
  Service Reject message according to TS 24.301 5.5.3.2.5 & 5.6.1.5

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_rau_sr_reject
(  
  lte_nas_message_id       msg_id,
  lte_nas_emm_cause_type   emm_cause,
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  boolean  delete_eplmn_list = FALSE;

  ASSERT(emm_ctrl_data_ptr != NULL);

  /* Update update status, GUTI, last registered TAI, TAI list, KSI */
  emm_tau_rau_sr_reject_invalidate_mobility(msg_id,
                                            emm_cause,
                                            emm_ctrl_data_ptr,
                                            delete_eplmn_list);

  /* Reset TAU attempt counter */
  if(msg_id == TRACKING_AREA_REJECT)
  {
    switch (emm_cause)
    {
      case LTE_NAS_PLMN_NOT_ALLOWED:
      case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
      case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
      case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
      case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
      case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
#ifdef FEATURE_NAS_REL11
      case LTE_NAS_CONGESTION:
#endif
        /* Reset TAU attempt counter */
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Resetting TAU attempt counter - Multimode");
        emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
        break ;
      default:
        break;
    }
  }

  /* EMM state change */
  switch (emm_cause)
  {
    case LTE_NAS_ILLEGAL_UE:
    case LTE_NAS_ILLEGAL_ME:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW:
    case LTE_NAS_IMPLICITLY_DETACHED:
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
#ifdef FEATURE_NAS_REL11
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
#endif
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
      break;
#ifndef FEATURE_NAS_REL11
/*24.008: 4.7.13.4  Service request procedure not accepted by the network cause #8*/
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
#endif
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
      if(msg_id == TRACKING_AREA_REJECT)
      {  
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
      }
      break;
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
    case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
      break;
/*24.008: 4.7.5.1.4,4.7.5.2.4
If the routing area update request is rejected with cause#22
If S1 mode is supported in the MS, the MS shall handle the EMM parameters EMM state, EPS update status, and tracking area updating attempt counter
as specified in 3GPP TS 24.301 [120] for the case when the tracking area update procedure is rejected with the EMM cause with the same value.*/
#if defined(FEATURE_LTE_REL10) && defined(FEATURE_NAS_REL11)
     case LTE_NAS_CONGESTION:
       if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
       {
         if(msg_id == SERVICE_REJECT)
         {
           EMM_MOVES_TO_REGISTERED_STATE((emm_substate_type)EMM_REGISTERED_NORMAL_SERVICE);
         }
         else
         {
           EMM_MOVES_TO_REGISTERED_STATE((emm_substate_type)EMM_REGISTERED_ATTEMPTING_TO_UPDATE) ;
         }
       }
       break;
#endif
    default:
      break;
  }
}/* emm_process_rau_sr_reject */

/*===========================================================================

FUNCTION    EMM_PROCESS_REJECT_MESSAGE_CAUSE

DESCRIPTION
  This function processes EMM cause received as part of TAU Reject message and
  Service Reject message according to TS 24.301 5.5.3.2.5 & 5.6.1.5

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
void emm_process_reject_message_cause
(
  lte_nas_message_id       msg_id,   /* Will be used when TAU feature is implemented */ 
  lte_nas_emm_cause_type   emm_cause,
#ifdef FEATURE_LTE_REL10
  dword                    t3346_val,
#endif
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  mmr_cause_e_type         reg_cause = MMR_CAUSE_NONE ;
  boolean                  delete_EPLMN_list = FALSE;
  lte_nas_tai_lst1_type    current_tai;
  emm_failure_type         emm_failure_cause;
  uint32                   timer_value;
  sys_plmn_id_s_type  current_lte_plmn;
#ifdef FEATURE_LTE_REL9
  lte_nas_emc_srv_status_type  prev_emc_srv_status;
#endif

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ;
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  emm_send_reg_rej_ind(emm_cause, emm_ctrl_data_ptr);

  /*printing the cause specific EFS value to keep F3 less */
  switch(emm_cause)
  {
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED :
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:	
	  MSG_HIGH_DS_4(MM_SUB, "=EMM= Is Accepted On LTE %d Eps Storage Supported : %d emm_plmn_block_on_rej_15_8 : %d emm_plmn_block_on_rej_15_roaming: %d",
				  emm_ctrl_data_ptr->is_accepted_on_lte,
				  emm_ctrl_data_ptr->eps_storage_supported,emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8, emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming);
	  break;
	default :
		break;
  }
  /* Following is covered by */
  /* 5.5.3.2.5 TAU not accepted by the network */
  /* 5.6.1.5 Service request procedure not accepted by the network */
  switch (emm_cause)
  {
    case LTE_NAS_ILLEGAL_UE:
    case LTE_NAS_ILLEGAL_ME:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
    case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW: 
    case LTE_NAS_IMPLICITLY_DETACHED : 
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
    case LTE_NAS_SEVERE_NW_FAILURE:
     emm_print_emm_cause( emm_cause);
      break ;
#ifdef FEATURE_LTE_REL10 
    case LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED:
      if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) || 
         (msg_id == TRACKING_AREA_REJECT))
       {
         MSG_HIGH_DS_0(MM_SUB, "=EMM= Cause - LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED");
       }
      else
       {
       /* 5.6.1.6 d - Abort service reject */
         MSG_HIGH_DS_0(MM_SUB, "=EMM= Abnormal service reject. Abort service request procedure.");
         emm_failure_cause.cause_type      = LTE_NAS_IRAT_SR_REJECT;
         emm_failure_cause.cause.emm_cause = emm_cause;
         if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)
          {
            /* Keep the connection. Do not send ESM failure indication here and
             let the caller of this function to send it.  */
             emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                       FALSE, ESM_LOCAL_CUASE_NONE,
                                       emm_failure_cause);
          }
         emm_process_unsent_messages(EMM_SRV_REQ_FAILURE_THROTTLE, 
                                      emm_failure_cause,
                                      emm_ctrl_data_ptr);
                /* No need to proceed */
         return;   
       }
      break ;
#endif
    default:
      if (msg_id == SERVICE_REJECT)
      {
        /* EMM causes only for Service Reject message */
        switch(emm_cause)
        {
          case LTE_NAS_CS_FALLBACK_CALL_EST_NOT_ALLOWED:
          case LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE :
          case LTE_NAS_CS_DOMAIN_NOT_AVAILABLE :
            emm_print_emm_cause( emm_cause);
            break ;
#ifdef FEATURE_LTE_REL10
          case LTE_NAS_CONGESTION:
            MSG_HIGH_DS_1(MM_SUB, "=EMM= Cause - LTE_NAS_CONGESTION t3346 %d", t3346_val);
            /* if ue is configured for rel10 & t3346_val is 0 then it is treated as abnormal*/
            /*If UE is configured for Rel 9 then this event is abnormal*/
            if(((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
               (t3346_val == 0)) ||
               (emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL10))   
            {
              /* Abnormal scenario, 5.6.1.6 e - Abort service reject */
              MSG_HIGH_DS_0(MM_SUB, "=EMM= Abnormal service reject. Abort service request procedure.");
              emm_failure_cause.cause_type      = LTE_NAS_IRAT_SR_REJECT;
              emm_failure_cause.cause.emm_cause = emm_cause;
              if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)
              {
                /* Keep the connection. Do not send ESM failure indication here and
                 let the caller of this function to send it.  */
                  emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                            FALSE, ESM_LOCAL_CUASE_NONE,
                                            emm_failure_cause);
              }
              emm_process_unsent_messages(EMM_SRV_REQ_FAILURE_THROTTLE, 
                                          emm_failure_cause,
                                          emm_ctrl_data_ptr);
              /* No need to proceed */
              return;
            }
            break;
#endif
#ifdef FEATURE_FEMTO_CSG
          case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Cause - LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG");
            if(emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id == SYS_CSG_ID_INVALID)
            {
              /* 5.6.1.6 d - Abort service reject */
              MSG_HIGH_DS_0(MM_SUB, "=EMM= Abnormal service reject. Abort service request procedure.");
              emm_failure_cause.cause_type      = LTE_NAS_IRAT_SR_REJECT;
              emm_failure_cause.cause.emm_cause = emm_cause;
              if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)
              {
                /* Keep the connection. Do not send ESM failure indication here and
                 let the caller of this function to send it.  */
                  emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                            FALSE, ESM_LOCAL_CUASE_NONE,
                                            emm_failure_cause);
              }
              emm_process_unsent_messages(EMM_SRV_REQ_FAILURE_THROTTLE, 
                                          emm_failure_cause,
                                          emm_ctrl_data_ptr);
                  /* No need to proceed */
              return;
            }
            break;
#endif 
           default:
            /* 5.6.1.6 d - Abort service reject */
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Abnormal service reject. Abort service request procedure.");
            emm_failure_cause.cause_type      = LTE_NAS_IRAT_SR_REJECT;
            emm_failure_cause.cause.emm_cause = emm_cause;
            if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)
            {
              /* Keep the connection. Do not send ESM failure indication here and
               let the caller of this function to send it.  */
                emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                          FALSE, ESM_LOCAL_CUASE_NONE,
                                          emm_failure_cause);
            }
            emm_process_unsent_messages(EMM_SRV_REQ_FAILURE_THROTTLE, 
                                        emm_failure_cause,
                                        emm_ctrl_data_ptr);
                /* No need to proceed */
            return;
        }
      }
      else
      {
        /* EMM causes only for TAU Reject message */
        switch(emm_cause)
        {
          case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
#ifndef  FEATURE_LTE_REL10 
          case LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED:
#endif
           emm_print_emm_cause( emm_cause);
           break ;

#ifdef FEATURE_LTE_REL10
          case LTE_NAS_CONGESTION:
            MSG_HIGH_DS_1(MM_SUB, "=EMM= Cause - LTE_NAS_CONGESTION t3346 %d", t3346_val);
            /* if ue is configured for rel10 & t3346_val is 0 then it is treated as abnormal*/
            /*If UE is configured for Rel 9 then this event is abnormal*/
            if(((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
               (t3346_val == 0)) ||
               (emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL10))                              
            {
              /* Start TAU recovery. Do not reset tau cause */
              emm_start_tau_recovery_procedure(emm_ctrl_data_ptr);
              return;
            }
            break;
#endif
#ifdef FEATURE_FEMTO_CSG
          case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Cause - LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG");
            if(emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id == SYS_CSG_ID_INVALID)
            {
              /* Start TAU recovery. Do not reset tau cause */
              emm_start_tau_recovery_procedure(emm_ctrl_data_ptr);
              return;
            }
            break;
#endif 
          default:
            /* Check whether cause 8 is normal or abnomal */
            if((emm_cause == 
                         LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED) 
               &&
               ((emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == 
                                                    COMBINED_TA_LA_UPDATING) ||
                (emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == 
                                         COMBINED_TA_LA_UPDATING_IMSI_ATTACH)))
            {
              MSG_HIGH_DS_0(MM_SUB, "=EMM= Cause - LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED");
              break ;
            }
            else
            {  
              /* 5.5.3.2.6 d Abnormal causes */
              if((emm_cause == LTE_NAS_SEMANTICALLY_INCORRECT_MSG) ||
                 (emm_cause == LTE_NAS_INVALID_MANDATORY_INFO) ||
                 (emm_cause == LTE_NAS_NON_EXISTENT_MSG_TYPE) ||
                 (emm_cause == LTE_NAS_NON_EXISTANT_INFO) ||
                 (emm_cause == LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED))
              {
                if(emm_ctrl_data_ptr->ps_detach_info.status == 
                                                     EMM_PS_DETACH_CNF_PENDING)
                {
                  /* If PS DETACH is pending, do not proceed. Locally clean up 
                     and report to CM. */
                  emm_ps_detach_clean_up(emm_ctrl_data_ptr);
                  reg_service_report(MMR_CAUSE_NOT_SPECIFIED, 
                                     FALSE, 
                                     emm_ctrl_data_ptr);
                  return;
                }
#ifdef FEATURE_LTE_REL9 
                if((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_PENDING) &&
                   (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == FALSE))
                {
#endif
                  /* Set TAU attempt counter to 5 */
                  MSG_HIGH_DS_1(MM_SUB, "=EMM= Set TAU attempt counter to %d",
                                MAX_NUMBER_TAU_OF_ATTEMPTS);
                  emm_ctrl_data_ptr->emm_tau_attempt_counter = 
                                                    MAX_NUMBER_TAU_OF_ATTEMPTS;
#ifdef FEATURE_LTE_REL9 
                }
                else
                {
                  MSG_HIGH_DS_0(MM_SUB, "=EMM= Not updating TAU attempt counter - EMC SRV pending");
                }
#endif
              }            

              /* Start TAU recovery. Do not reset tau cause */
              emm_start_tau_recovery_procedure(emm_ctrl_data_ptr);
              return;
            }
        }        
      }
      break;
  }

  /* Always update GMM/MM before EMM processing to allow GMM/MM writes sim */
  if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_UMTS_RADIO_ACCESS) || 
     mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_GSM_RADIO_ACCESS)  || 
     mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_TDS_RADIO_ACCESS))
  {
#ifdef FEATURE_LTE_REL10
    if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
       (msg_id == TRACKING_AREA_REJECT) &&
       (emm_cause == LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED))
    {
      /* R10 Spec 24.301 CR #0905. EMM Cause #40 to be treated like EMM Cause #10 */
      mm_update_tau_sr_reject_status(msg_id, (byte)LTE_NAS_IMPLICITLY_DETACHED);
    }
    else
#endif
    {
      mm_update_tau_sr_reject_status(msg_id, (byte)emm_cause);
    }
  }

  /* Mobility info and EPS status */
  /* For some TAU reject cases, need to delete EPLMN list */ 
  if(msg_id == TRACKING_AREA_REJECT)
  {
    delete_EPLMN_list = TRUE;
    if(emm_ctrl_data_ptr->tau_pending_gw_attach == TRUE)
    {
      emm_ctrl_data_ptr->tau_pending_gw_attach = FALSE;  
    }
  }

  /* Timer T3440 , T3346*/
  switch (emm_cause)
  {
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
    case LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED:
    case LTE_NAS_IMPLICITLY_DETACHED:
    case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW:
      mm_start_timer(TIMER_T3440,EMM_T3440_TIMER_DURATION);
      break ;
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
      if(msg_id != SERVICE_REJECT)
      {
        mm_start_timer(TIMER_T3440,EMM_T3440_TIMER_DURATION);
      }
      break ;
#ifdef FEATURE_LTE_REL10
    case LTE_NAS_CONGESTION: 
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) 
      {
        mm_stop_timer(TIMER_T3346);
        mm_start_timer(TIMER_T3346, t3346_val);
        mm_is_lu_to_be_initiated_t3346_ltogw = TRUE;
        /*saves congestion plmn associated with the timer*/
        if(msg_id == TRACKING_AREA_REJECT)
        {
            gmm_set_gmm_is_congestion_due_to_rau(TRUE);
        }
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

  /* Delete the list of equivalent PLMNs */ 
  if(msg_id == TRACKING_AREA_REJECT)
  {
    switch (emm_cause)
    {
      case LTE_NAS_ILLEGAL_UE:
      case LTE_NAS_ILLEGAL_ME:
      case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
      case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
      case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW:
      case LTE_NAS_IMPLICITLY_DETACHED:
      case LTE_NAS_PLMN_NOT_ALLOWED:
      case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
      case LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED:
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Deleting equivalent PLMNs list - TAU REJ");
        emm_ctrl_data_ptr->update_eplmn_list = TRUE;
        emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
        break;
#ifdef FEATURE_LTE_REL11
       case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
         if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)&&
            (emm_is_s1_mode_only(emm_ctrl_data_ptr) == TRUE))
         {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Deleting equivalent PLMNs list - TAU REJ");
          emm_ctrl_data_ptr->update_eplmn_list = TRUE;
          emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
         }
        break;
#endif
      default:
        /* Do nothing here for other emm causes */
        break;
    }
  }

  /* Create current PLMN ID and TAC based on latest RRC reported PLMN ID and TAC */
  current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
  current_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;

  /* Add current PLMN into forbidden PLMN list */
  switch (emm_cause)
  {
    case LTE_NAS_PLMN_NOT_ALLOWED:
      mm_add_plmn_to_fplmn_list(current_tai.plmn); 
      break;
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
      if(msg_id == TRACKING_AREA_REJECT)
      {
        mm_add_plmn_to_gprs_fplmn_list(current_tai.plmn);
      }
      break;
    case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:		  
	case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
	   current_lte_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
	                                       emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
	                                                                                   selected_plmn);
          if((
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
             ))
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
              if((emm_cause == LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA) &&
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
      current_lte_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
      if(!((emm_cause==LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA) &&

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
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Add TAI to FTAI list for roaming - TAU/SR Rej");
        emm_add_forbidden_tai(emm_ctrl_data_ptr->rrc_forbidden_list_ptr,
                              emm_get_camped_tai(emm_ctrl_data_ptr),
                             emm_ctrl_data_ptr);
        emm_add_manual_ftai(emm_ctrl_data_ptr);
      }
      break;
    case LTE_NAS_CS_DOMAIN_NOT_AVAILABLE :
      if ((msg_id == SERVICE_REJECT) &&
          (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB))
      {
        /* Add serving PLMN to forbidden PLMN list for combined registration */
        emm_add_combined_reg_forbidden_plmn(emm_ctrl_data_ptr);
      }
      break;
 
    default:
      /* Do nothing here for other emm causes */
      break;
  }

#ifdef FEATURE_FEMTO_CSG
  /* Remove current CSG from white list*/
  switch(emm_cause)
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

  if(msg_id == TRACKING_AREA_REJECT)
  {
    /* Remove current TAI from TAI list */
    switch (emm_cause)
    {
      case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
      case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
        emm_delete_tai_from_list(&current_tai, emm_ctrl_data_ptr->emm_tai_list_ptr);
        break;
      default:
        /* Do nothing here for other emm causes */
        break;
    }

    /* Reset TAU attempt counter */
    switch (emm_cause)
    {
      case LTE_NAS_PLMN_NOT_ALLOWED:
      case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
      case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
      case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
      case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
      case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
        /* Reset TAU attempt counter */
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Reset TAU attempt counter");
        emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
        break ;
#ifdef FEATURE_LTE_REL10
      case LTE_NAS_CONGESTION: 
        if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
           (msg_id == TRACKING_AREA_REJECT ))
        {
          /* Reset TAU attempt counter */
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Reset TAU attempt counter");
          emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
        }
#endif
        break; 
      case LTE_NAS_SEVERE_NW_FAILURE:
        /* Reset TAU attempt counter */
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Reset TAU attempt counter to MAX Number of Attempts %d",
                      MAX_NUMBER_TAU_OF_ATTEMPTS);
        emm_ctrl_data_ptr->emm_tau_attempt_counter = MAX_NUMBER_TAU_OF_ATTEMPTS;
        break ;
      default:
        /* Do nothing here for other emm causes */
        break;
    }

    /* Set flag indicating combined TA/LA updating with IMSI attch */
    if((emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == COMBINED_TA_LA_UPDATING)
       ||
       (emm_ctrl_data_ptr->emm_ongoing_eps_ta_update == 
                                          COMBINED_TA_LA_UPDATING_IMSI_ATTACH))
    {    
      switch (emm_cause)
      {
        case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
        case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
        case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Set SGs association flag - Combined TAU REJ %d",
                        emm_cause);
          emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
          break ;
        default:
          break;
      }
    }

    /* Reset TAU cause in emm data base */
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
  }

  emm_tau_rau_sr_reject_invalidate_mobility(msg_id,
                                            emm_cause,
                                            emm_ctrl_data_ptr,
                                            delete_EPLMN_list);
#ifdef FEATURE_LTE_REL9
  /* EMM state change function reset emc srv status. Save it to local for 
     reattach condition checks. */
  prev_emc_srv_status = emm_ctrl_data_ptr->emc_srv_status;
#endif

  if((msg_id == TRACKING_AREA_REJECT) &&
     (emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_CNF_PENDING))
  {
    /* TAU REJ and PS DETACH pending. Do a clean up and report to CM. */
    emm_tau_sr_reject_change_state(msg_id,emm_cause,emm_ctrl_data_ptr);
    emm_send_rrc_system_update(emm_ctrl_data_ptr);
    
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
    emm_ps_detach_clean_up(emm_ctrl_data_ptr);
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
  }
  else
  {
    /* Update EMM state */
    emm_tau_sr_reject_change_state(msg_id,emm_cause,emm_ctrl_data_ptr);
  
    /*-------------------------------------------------------------------
    Now send RRC SYSTEM UPDATE - this is required to let RRC know 
    whether to camp or not, in case where NW sends TAU REJECT with a 
    CAUSE value LTE_NAS_TRACKING_AREA_NOT_ALLOWED
    --------------------------------------------------------------------*/
    emm_send_rrc_system_update(emm_ctrl_data_ptr);
  
    /* Start attach procedure or report no service to REG */
    switch (emm_cause) 
    {
      case LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED:
        if(msg_id == TRACKING_AREA_REJECT)
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
#ifdef FEATURE_LTE_REL10
        /* R10 Spec 24.301 CR #0905. A UE in CS/PS mode 1 or mode 2
           of operation is still IMSI attached for non-EPS services*/

          if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
             (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                              SYS_SRV_DOMAIN_CS_PS))
          {
            emm_ctrl_data_ptr->plmn_service_state->reg_service_domain =
                                                   SYS_SRV_DOMAIN_CS_ONLY;
            emm_ctrl_data_ptr->srv_domain_registered = 
                                                   SYS_SRV_DOMAIN_CS_ONLY;
          }
          else 
#endif
          {
            emm_ctrl_data_ptr->plmn_service_state->reg_service_domain =
                                                SYS_SRV_DOMAIN_NO_SRV; 
            emm_ctrl_data_ptr->srv_domain_registered = 
                                                SYS_SRV_DOMAIN_NO_SRV;
          }

#ifdef FEATURE_LTE_REL9
          if((prev_emc_srv_status != LTE_NAS_EMC_SRV_ONLY) &&
             (prev_emc_srv_status != LTE_NAS_EMC_SRV_PENDING)&&
             (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr)== FALSE))
          {
#endif 
            if(emm_ctrl_data_ptr->manual_user_sel == TRUE)
            {
              MSG_HIGH_DS_0(MM_SUB," =MM= Blocking service indication to REG during manual selection");
            }
            else    
            {
              reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN, FALSE, 
                               emm_ctrl_data_ptr);
            }
#ifdef FEATURE_LTE_REL9
          }
          else if(emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == TRUE)
          {
            EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE);
            reg_service_report(MMR_CAUSE_ABORT_PLMN_SELECTION, FALSE, emm_ctrl_data_ptr);
          }
          else
          {
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, 
                               emm_ctrl_data_ptr);
          }
#endif
        }
#ifdef FEATURE_LTE_REL10         
        else if ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) && 
                 (msg_id == SERVICE_REJECT))
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                               SYS_SRV_STATUS_LIMITED;
        /* spec 24.301 5.5.3.3.5 and 5.6.1.5. A UE in CS/PS mode 1 or mode2
               of operation is still IMSI attached for non-EPS services */
          if(emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)
          {
            emm_ctrl_data_ptr->plmn_service_state->reg_service_domain =
                                                 SYS_SRV_DOMAIN_CS_ONLY;
            emm_ctrl_data_ptr->srv_domain_registered = 
                                                 SYS_SRV_DOMAIN_CS_ONLY;
          }
          else 
          {
            emm_ctrl_data_ptr->plmn_service_state->reg_service_domain =
                                                 SYS_SRV_DOMAIN_NO_SRV;
            emm_ctrl_data_ptr->srv_domain_registered = 
                                                 SYS_SRV_DOMAIN_NO_SRV;
          }
 
          if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)

          {
          reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN, FALSE, emm_ctrl_data_ptr);
          }  
        }
#endif
        break;
      case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW :
      case LTE_NAS_IMPLICITLY_DETACHED :
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        /* spec 24.301 5.5.3.3.5 and 5.6.1.5. A UE in CS/PS mode 1 or mode2
               of operation is still IMSI attached for non-EPS services */
        if(emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)
        {
          emm_ctrl_data_ptr->plmn_service_state->reg_service_domain =
                                                 SYS_SRV_DOMAIN_CS_ONLY;
          emm_ctrl_data_ptr->srv_domain_registered = 
                                                 SYS_SRV_DOMAIN_CS_ONLY;
        }
        else 
        {
          emm_ctrl_data_ptr->plmn_service_state->reg_service_domain =
                                                 SYS_SRV_DOMAIN_NO_SRV;
          emm_ctrl_data_ptr->srv_domain_registered = 
                                                 SYS_SRV_DOMAIN_NO_SRV;
        }
#ifdef FEATURE_LTE_REL9
        if((prev_emc_srv_status != LTE_NAS_EMC_SRV_ONLY) &&
           (prev_emc_srv_status != LTE_NAS_EMC_SRV_PENDING)&&
           (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr)== FALSE))
        {
#endif 
          if(msg_id == TRACKING_AREA_REJECT && emm_ctrl_data_ptr->manual_user_sel == TRUE)
          {
            MSG_HIGH_DS_0(MM_SUB," =MM= Blocking service indication to REG during manual selection"); 
          }
          else
          {
            reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN, FALSE, 
                             emm_ctrl_data_ptr);
          }
#ifdef FEATURE_LTE_REL9
        }
        else if((msg_id == TRACKING_AREA_REJECT) &&
                (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr) == TRUE))
        {
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE);
          reg_service_report(MMR_CAUSE_ABORT_PLMN_SELECTION, FALSE, emm_ctrl_data_ptr);
        }
        else
        {
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, 
                             emm_ctrl_data_ptr);
        }
#endif
        break;
      case LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE:
#ifdef FEATURE_LTE_REL9
        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
        {
          mm_start_timer(TIMER_T3442, emm_ctrl_data_ptr->t3442_value);
          MSG_HIGH_DS_0(MM_SUB, "=EMM= T3442 timer started");
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);
        }
#endif
        break;
      case LTE_NAS_CS_DOMAIN_NOT_AVAILABLE:
        emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_PS_ONLY;
        emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_PS_ONLY;  
        emm_ctrl_data_ptr->plmn_service_state->cs_service_status = SYS_SRV_STATUS_NO_SRV;
        /*For cause #18 send LTE CS capability as barred so that CM can disable LTE indefinitely. */
        emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability = SYS_LTE_CS_CAPABILITY_BARRED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
        break;
      default:
         
        /* SERV_REJ with #22 we should not report LIMITED */
        if((msg_id != SERVICE_REJECT) ||
           (emm_cause != LTE_NAS_CONGESTION))
        {
          /* If reattach is not necessary, report no service to REG */
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        

          if((emm_cause == LTE_NAS_EPS_SERVICES_NOT_ALLOWED ||
             emm_cause == LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN) &&
             (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                              SYS_SRV_DOMAIN_CS_PS))
          {
            /* spec 24.301 5.5.3.3.5 and 5.6.1.5. A UE in CS/PS mode 1 or mode2
             of operation is still IMSI attached for non-EPS services */
            emm_ctrl_data_ptr->plmn_service_state->reg_service_domain =
                                                        SYS_SRV_DOMAIN_CS_ONLY;
            emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_CS_ONLY;
#ifdef FEATURE_LTE_REL11
             /*spec 24.301 5.5.3.2.5 The UE operating in CS/PS mode 1 or CS/PS mode 2 of operation 
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
        } 
  
        switch(emm_cause)
        {
          case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
#ifdef FEATURE_FEMTO_CSG
          case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
#endif 
            current_lte_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
            if((emm_cause==LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA) &&

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
          case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
          case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
            emm_ctrl_data_ptr->plmn_service_state->plmn_forbidden = TRUE ;
            reg_cause = MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES ;
            emm_set_gmm_manual_selection_flag(FALSE);
            emm_ctrl_data_ptr->manual_user_sel = FALSE;
            break ;
          case LTE_NAS_PLMN_NOT_ALLOWED: 
            emm_ctrl_data_ptr->plmn_service_state->plmn_forbidden = TRUE ;
            reg_cause = MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED ;
            emm_ctrl_data_ptr->manual_user_sel = FALSE;
            emm_set_mm_manual_selection_flag(FALSE);
            break ;
          case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
            emm_ctrl_data_ptr->plmn_service_state->plmn_forbidden = TRUE ;
            reg_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN ;
            emm_ctrl_data_ptr->manual_user_sel = FALSE;
            emm_set_mm_manual_selection_flag(FALSE);
            break ;
          case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
            reg_cause = MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION ;
            emm_set_mm_manual_selection_flag(FALSE);
            emm_ctrl_data_ptr->manual_user_sel = FALSE;

            break ;
          case LTE_NAS_SEVERE_NW_FAILURE:
            reg_cause = MMR_CAUSE_FAILURE_MAX_ATTEMPTED ;
            break ;
#ifdef FEATURE_LTE_REL10
          case LTE_NAS_CONGESTION:
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Cause - LTE_NAS_CONGESTION");
            if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
               (msg_id == TRACKING_AREA_REJECT))                               
            {
              reg_cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
            }
            else
            {
              reg_cause = MMR_CAUSE_NOT_SPECIFIED ;
            }
            break;
#endif
          default:
            current_lte_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
              emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);
            if((emm_cause==LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED)&&
               (emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE)&&
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
                if (emm_cause==LTE_NAS_ILLEGAL_UE ||
                    emm_cause==LTE_NAS_ILLEGAL_ME ||
                    emm_cause==LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED) 
                {
                  emm_ctrl_data_ptr->manual_user_sel = FALSE;
                  emm_set_mm_manual_selection_flag(FALSE);
                }  	  
              reg_cause = MMR_CAUSE_NOT_SPECIFIED ;
            }
            break ;
        }
#ifdef FEATURE_LTE_REL9
        if((msg_id == TRACKING_AREA_REJECT)&&
           (emm_is_registration_for_emc_pdn_conn(emm_ctrl_data_ptr)== TRUE))
        {
          if(emm_cause == LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA
             ||
             emm_cause == LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA)
          {
            /* UE can not camp on cell which belongs to FTAI for roaming in automatic mode.
               Overriding SIB1 E911 support to FALSE to have CM swithes UE back to limited mode */
            emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
                                                 ims_emergency_support_enabled = FALSE;
          }
          if(EMM_GET_STATE() == EMM_REGISTERED)
          {
            EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_LIMITED_SERVICE);
          }
          else
          {
            EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_LIMITED_SERVICE);
          }
          reg_cause = MMR_CAUSE_ABORT_PLMN_SELECTION;
        }
#endif
        reg_service_report(reg_cause, FALSE, emm_ctrl_data_ptr);
        break;
    }
  }
  if (msg_id == SERVICE_REJECT)
  {
    emm_failure_cause.cause_type      = LTE_NAS_IRAT_SR_REJECT;
    emm_failure_cause.cause.emm_cause = emm_cause;
  }
  else
  {
    emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
  }
  emm_process_unsent_messages(EMM_SRV_REQ_FAILURE_NO_THROTTLE, 
                              emm_failure_cause,
                              emm_ctrl_data_ptr);

} /* end of emm_process_reject_message_cause() */

/*===========================================================================

FUNCTION    EMM_PROCESS_REJECT_MESSAGE

DESCRIPTION
  This function processes TAU Reject and Service Reject messages

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
void emm_process_reject_message
(
  lte_nas_emm_incoming_msg_type  *incoming_message,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  emm_failure_type        emm_failure_cause;
  emm_csfb_failure_cause_type failure_cause_type;
  failure_cause_type.failure_type = INVALID_REJECT_CAUSE;

  ASSERT(incoming_message != (lte_nas_emm_incoming_msg_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  /*initialise it*/
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  switch(incoming_message->hdr.msg_id)
  {
    case SERVICE_REJECT:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Received service Reject message");
      if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB)
      {
        mm_stop_timer(TIMER_T3417_EXT);
      }
      else
      {
        mm_stop_timer(TIMER_T3417);
#ifdef FEATURE_LTE_REL10
        emm_ctrl_data_ptr->responding_to_page = FALSE;
#endif

#ifdef FEATURE_1XSRLTE 
        mm_stop_timer(TIMER_SRLTE_ESR);
        MSG_HIGH_DS_0(MM_SUB, "=EMM= TIMER_SRLTE_ESR has been stopped");
#endif
      }
#ifdef FEATURE_LTE_REL9
      if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9) &&
         (incoming_message->service_rej.t3442_val_exists))
      {
        emm_ctrl_data_ptr->t3442_value = incoming_message->service_rej.t3442_val;
      }
#endif

      emm_remove_release_client(emm_service_request_release_indication);

      emm_process_reject_message_cause(incoming_message->hdr.msg_id, 
                                       incoming_message->service_rej.emm_cause,
#ifdef FEATURE_LTE_REL10
                                       incoming_message->service_rej.t3346_val,
#endif
                                       emm_ctrl_data_ptr) ;

      /* NAS security requirement: RAND and RES stored are to be deleted and timer T3416 if running
         must be stopped if there is a SERVICE REJECT 
      */
      auth_reset_cache_memory(FALSE, NO_TIMER_EXPIRED, RRC_PS_DOMAIN_CN_ID);

      /* If service request is initiated by DS, need to send failure ind to DS
         Note: For service reject (DS intiated) with cause 9 or 10, a failure 
         ind is sent in emm_process_reject_message_cause function and srv req 
         trans id in data base is reset. Therefore no duplicate failure ind 
         will be sent here */
      if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE ||
         emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED || 
         emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED)
      {
        emm_failure_cause.cause_type      = LTE_NAS_IRAT_SR_REJECT;
        emm_failure_cause.cause.emm_cause = incoming_message->service_rej.emm_cause;
        emm_process_service_req_failure(LTE_NAS_SERVICE_REQ_FAILED,
                                        incoming_message->service_rej.emm_cause,
                                        emm_failure_cause, 
                                        emm_ctrl_data_ptr);
      }
      if(emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE)
      {
        /* CSFB was in progress*/
        failure_cause_type.failure_type = OTA_REJECT_CAUSE;
        failure_cause_type.failure_cause.lte_nas_ota_emm_cause = incoming_message->service_rej.emm_cause;
        emm_process_extended_service_req_failure(failure_cause_type,emm_ctrl_data_ptr);
#if defined  FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
        /*Reset TRM to low priority after esr type reset to NONE in extended service req failure function*/
        emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
#endif
        /* This state check is needed as in case the state is already changed to something other than REGISTERED when reject casue is processed*/
        if(EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED)
        {
          if(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->svc_status == 
                                                  LTE_RRC_SVC_STATUS_AVAILABLE)
          {
            /* Enetr EMM_REGISTERED_NORMAL_SERVICE state */
            EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);
          }
          else
          {
            EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
          }
        }
      }

      /* Process all pending irat uplink messages */
      emm_process_pending_irat_msg(LTE_NAS_IRAT_SR_REJECT,
                                   (byte)incoming_message->service_rej.emm_cause,
                                   LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                                   emm_ctrl_data_ptr);
      break;
    case TRACKING_AREA_REJECT:
      emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_REJECTED, (lte_nas_emm_cause_type)incoming_message->tau_rej.emm_cause);
#ifdef FEATURE_GSTK
      emm_send_nw_rej_evt_to_gstk(GSTK_TRACKING_AREA_UPDATE_REJ,
                                  incoming_message->tau_rej.emm_cause,
                                  emm_ctrl_data_ptr);
#endif
      emm_process_reject_message_cause(incoming_message->hdr.msg_id, 
                                       incoming_message->tau_rej.emm_cause, 
#ifdef FEATURE_LTE_REL10
                                       incoming_message->tau_rej.t3346_val,
#endif
                                       emm_ctrl_data_ptr) ;

      if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED) || 
         (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED))
      {
        /* CSFB was buffered progress*/
        failure_cause_type.failure_type = OTA_REJECT_CAUSE;
        failure_cause_type.failure_cause.lte_nas_ota_emm_cause = incoming_message->tau_rej.emm_cause;
        emm_process_extended_service_req_failure(failure_cause_type,emm_ctrl_data_ptr);
      }
      break;
    default:
      /* This function only processes reject messages for service request or TAU */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Invliad reject message");
      break;
  } 
} /* end of emm_process_reject_message() */

#endif /*FEATURE_LTE*/
