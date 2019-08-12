
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_rrc_handler.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/26/15   us     If Timer 3346 is running and EMM substate is EMM_REGISTERED_ATT_TO_UPDATE 
                  then on receiving a page indication do TAU.   
09/17/12   cogolt Added support for Rel-10 ESM backoff timer
07/05/12   sar    Rel-10 change when no active bearers
05/29/12   pahire  Added changes for UE Specific DRX Cycle for QChat/LTE 
01/19/12   MNK   Fix for CR 331514 (1xcsfb: NAS needs to distnguish RLF vs OOS failure when indicating the failure to CM)  
08/09/11   abhi   reseting deact_cnf and ignore_gw_activation on receiving ph_staus_change_cnf
07/04/11   Abhi   Send ph status confirm to REG on receiving deactivate confirm from LRRC
06/16/11   PA      Added changes for Rel 9 Emergency Bearer services  
05/27/11   PA      Added changes for Security Key generation during W2L PS HO
02/01/10   PA      Added changes for Security Key generation during L2W PS HO
11/29/10   MNK     More 1XCSFB changes including adversarial scenarios
11/01/10   MNK     Introduce e1XCSFB 
10/07/10   MNK     1XCSFB introduction 
09/23/10   MNK     Handle Extended service reject for CSFB flows 
09/20/10   MNK     ESR validation for MT calls added 
09/17/10   MNK     CS_NOTIFICATION for 3GPPCSFB added
08/09/10   MNK     Make code compatible for 1xCSFB & 3GPPCSFB
08/30/10   MNK     CSFB changes
08/13/10   MNK     Reset reg_req_pended_indication upon deactivate_cnf 
08/07/10   MNK     Reset emm ctrl data when moving from DEREGISTERED to DEREGISTERED_NO_CELL_AVAILABLE to cover up the scenario 
                   where LTE attach failed and BSR took UE to 1X and then UE came back to LTE, but EMM uses the cached PDN connect request
                   as emm ctrl data is nort cleared when UE moved to DEREGISTERED_NO_CELL_AVAILABLE previously.
06/30/10   zren    Updated to only process unsent messages in idle state when 
                   receiving service ind w/ no service 
06/29/10   zren    Updated EMM to handle service request procedure and unsent 
                   messages correctly for redirection and reselection 
06/25/10   zren    Updated TAU type setting for TAU for IRAT 
06/23/10   zren    Updated EMM to send camped indication after receiving 
                   LTE_RRC_SERVICE_IND due to MMR_REG_REQ after IRAT 
05/26/10   zren    Updated to send LIMITED SERVICE status in REG CNF for 
                   LIMITED SERVICE REQ when get service available service ind 
                   from RRC
05/26/10   zren    Updated to only process pending REG REQ or message when EMM 
                   is in IDLE state after received a service available 
                   RRC SERVICE IND
05/18/10   zren    Updated fun param to set only_for_reg flag in REG CNF and 
                   REG IND
05/17/10   zren    Updated EMM to only process EMM timers
05/11/10   RI      Mainlined spec changes upto Dec 09.
05/11/10   zren    Updated to use trans ID rcved in PLMN SEARCH REQ
05/07/10   zren    Merged function emm_rrc_process_service_indication and 
                   function emm_rrc_process_normal_service_indication
                   Fixed to not process ACT DEF BC REQ when attach accept 
                   carries unexpected EMM cause and is treated as attach reject
                   Added code to handle found plmn list in RRC SERVICE IND
05/04/10   RI      Modified the decoding of NAS messages to use the DL cnt from 
                   the SQN in the message.
04/19/10   MNK     Minor Changes - F3 messages
04/13/10   MNK     ISR implementation
04/09/10   zren    Added combined procedure feature
03/16/10   RI      Handling of security header information.
03/16/10   zren    Updated MO detach procedure upon receiving MMR_STOP_MODE
02/24/10   zren    Added support for network selection modes of LIMITED MANUAL
                   ACQUISITION and LIMITED ROUTINE ACQUISITION
02/23/10   RI      Reset the secure exchange of NAS messages when in RRC
                   connection releasing state.
02/23/10   MNK     Added support for GW to LTE i-RAT
02/19/10   MNK     Remove the protocol state error checks for security and emm information
02/16/10   MNK     Add ignore_GW_activation_ind
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
02/05/10   zren    Updated processing of connection abort in waiting for RRC 
                   confirmation state
02/03/10   MNK     Fix emm state transitions for lte-eHRPD scenario
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
01/30/10   MNK     Fix issues with service_ind handling.
01/22/10   RI      Updated the handling of context during detach.
01/21/10   zren    Added EMM/ESM STATUS feature
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
11/16/09   RI      Added support for EMM INFORMATION request.
11/04/09   MNK     Cleanup emm_ctrl_data as necessary if RRC is deactivated due to non-i-RAT
10/14/09   MNK     Commented clearing the emm_ctrl_data when deactivate_cnf is received for now
10/05/09   hnam    Added support for TLB
09/26/09   RI      Added support for GUTI Reallocation command.
09/21/09   MNK     Changes due to IRAT design
09/25/09   hnam    Added TLB support
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/20/09   zren    Updated to stop barring timers if new cell is detected
08/04/09   hnam    Added support for taking care of ACT DEF EPS BEARER CONTEXT REQ
                    (on receipt of corrupted ATTACH ACCEPT message)
07/31/09   hnam    Replaced LTE_ERROR with ERR in switch case of ESM in 
                     emm_process_rrc_data_indication(), as this was asserting
07/29/09   zren    Added support to use global cell_identity
07/01/09   tmai    Add the code to handle detach USIM removal case for 
                   detach and rrc service indication collision
06/18/09   zren    Modified emm_registered_process_rrc_service_ind to process 
                   RRC_SERVICE_IND received in REGISTERED_PLMN_SEARCH state
                   to either move to REGISTERED_NORMAL_SERVICE or statr TAU
06/13/09   RI      Updated the NAS security processing.
                   Added Identity Request support.
06/11/09   zren    Added support to trigger TAU when UE returns back to 
                   REGISTERED_NORMAL_SERVICE state and EPS bearer context was
                   updated in earlier REGISTERED_NO_CELL_AVAILABLE state
06/02/09   MNK     CR# 184007. These are the changes required to make sure at the time of attach, 
                   EMM receives (ATTACH ACCEPT + ACTIVATE DEFAULT BEARER CONTEXT REQUEST) and EMM sends
                    (ATTACH COMPLETE + ACTIVATE DEFAULT BEARER CONTEXT ACCEPT)
05/29/09   zren    Added support for TAU feature
05/17/09   hnam    Added the code to support security
05/14/09   zren    Added EPS status and SIM state checks before starting Service
                   Request Procedure 
05/11/09   zren    Added support for MT detach procedure 
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
04/15/09   RI      Added support for LTE NAS Authentication procedure.
04/15/09   zren    Completed MO detach feature
04/02/09   zren    Added support for new transaction ID format 
03/26/09   RI      Added support for GS Timers in LTE.
03/24/09   zren    Added support for service request and paging feature
03/12/09   RI      Added support for SMC.
03/03/09   hnam    Added checking for the validity of the NAS messages parsed by the MSG_LIB after decoding
02/26/09   vdr     Fixed Lint errors and warnings
02/09/09   vdr     Fixed lte_rrc_conn_est_cnf_s processing according to lte_rrc_ext_msg.h#54
02/06/09   MNK     Fixed the bug in emm_process_rrc_ul_data_cnf where wrong trasn_id is being used to match the pending message
02/06/09   vdr     Modified lte_rrc_conn_est_cnf_s processing according to lte_rrc_ext_msg.h#54
02/05/09   MNK     Changed decode_nas_msg to decode_nas_msg_fp and nas_retrieve_msg to nas_retrieve_msg_fp
01/30/09   vdr     Added processing of new LTE_RRC_CONN_EST_TIMER_UPDATE_IND
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "emm.h"
#include "rex.h"
#include "emm_database.h"
#include "emm_connection_handler.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_irat_msg.h"
#include "emm_update_lib.h"
#include "lte_nas_common_v.h"
#include "emm_utility.h"
#include "emm_rrc_if.h"
#include "emm_reg_handler.h"
#include "emm_rrc_emm_if.h"
#include "emm_update_lib.h"
#include "lte_nas_emm_message_type.h"
#include "lte_nas_msg_parse.h"
#include "lte_nas.h"
#include "emm_esm_handler.h"
#include "emm_sim_handler.h"
#include "sys_stru_v.h"
#include "emm_service_request_lib.h"
#include "emm_procedure_reject_handler.h"
#include "emm_security.h"
#include "lte_nas_security_info.h"
#include "err.h"
#include "mm_v.h"
#include "lte_nas_tlb.h"
#include "emm_cnm_handler.h"
#include "gmm_int_v.h"
#include "emm_csfb_handler.h"
#include "mm_multimode_common.h"
#include "ds3gpp_api.h"
#include "emm_mem.h"
#include <dsm_item.h>
#include <dsm_init.h>
#include "msgr_types.h"
#include "policyman_nas.h"
#include "policyman_cm.h"
#ifdef FEATURE_LTE_REL9
#include "emm_gps_handler.h"
#endif
#include "sys_eplmn_list_v.h"
#include "stringl.h"
#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE)) 
#error code not present
#endif // TEST_FRAMEWORK

/* Global to keep track of the DL SQN for DL msgs */
byte  dl_sqn_for_current_msg;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_validate(a) sys_eplmn_per_nas_stacks_list_validate(a,(sys_modem_as_id_e_type)mm_as_id)
#endif

#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_ENHANCED_NW_SELECTION
#define reg_sim_read_ens_flag() reg_sim_per_nas_stacks_read_ens_flag((sys_modem_as_id_e_type)mm_as_id)
#endif
#endif

#ifdef FEATURE_SGLTE
  extern void mmcoord_process_pstimer_expiry_merging(void);
  extern boolean mm_is_sglteplmn_in_sglte_roam(sys_plmn_id_s_type  pPlmnId);
  extern boolean mm_process_if_both_stack_has_camped_non_sglte_plmn
                                                           (mm_as_id_e_type in_as_id);
  extern mm_as_id_e_type mm_get_ps_stack_id(void);
  extern boolean mm_get_is_limited_regional_sent(void);
  extern void mm_set_is_limited_regional_sent(boolean val);
  extern boolean mm_is_non_sglte_plmn_in_sglte_home(sys_plmn_id_s_type pPlmnId);
  extern  boolean mm_process_sglte_plmn_state4_gsm_oos(sys_plmn_id_s_type pPlmnId);
  extern void mm_set_is_reg_ps_split_reg (boolean val);
#endif
extern void emm_set_mm_msg_info_for_debug_buffer(byte msg_info);


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


boolean emm_policy_change_pending(lte_rrc_service_ind_s *rrc_service_ind)
{
  boolean pm_chng_pending = FALSE;
#ifdef FEATURE_TDSCDMA     
  emm_ctrl_data_type        *emm_ctrl_data_ptr;
  msgr_hdr_s   *pm_cfg_ptr = NULL;
 
  boolean      non_plmn_selection = TRUE;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  if((emm_ctrl_data_ptr->reg_req_pended_indication == TRUE) &&
     (emm_ctrl_data_ptr->reg_init_rrc_trans_id == rrc_service_ind->trans_id))

  {
    non_plmn_selection = FALSE;
  }
  if(reg_mode_is_usim_compliant_for_cm((sys_modem_as_id_e_type)mm_sub_id) == FALSE)
  {
    pm_chng_pending = FALSE;
  }
  else if(mm_timer_status[TIMER_POLICY_CHANGE_PENDING] == TIMER_ACTIVE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Policy Change Timer running");
    pm_chng_pending = TRUE;
  }
  else
  {
   
    /*If UE is not in IMS call then wait for PM to change mode if policy change 
      will happen on new PLMN*/ 
    if(emm_mmtel_status( emm_ctrl_data_ptr ) == FALSE && (emm_wms_sms_status(emm_ctrl_data_ptr) == FALSE))
    {
      pm_cfg_ptr = policyman_report_plmn_candidate((sys_modem_as_id_e_type)mm_as_id, 
                                         &emm_ctrl_data_ptr->plmn_service_state->plmn);
      if(pm_cfg_ptr != NULL)
      {
        MSG_HIGH_DS_0(MM_SUB, "EMM: Sent Limited regional As UE MODE change pending ");
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                           SYS_SRV_STATUS_LIMITED;
  
        emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                               rrc_service_ind->camped_svc_info, non_plmn_selection);
        reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,
                                    TRUE,
                                    emm_ctrl_data_ptr);
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Policy Change pending, send limited service");
        pm_chng_pending = TRUE;
        mm_start_timer(TIMER_POLICY_CHANGE_PENDING, DEFAULT_TIMEOUT);
      }
      policyman_report_preference_msg_free(pm_cfg_ptr);
    }
  }
#endif
  return pm_chng_pending;
}

/*===========================================================================

FUNCTION  EMM_ATTACH_NEEDED_RRC_SERVICE_IND

DESCRIPTION
  This function processes the RRC SERVICE IND recived in DEREGISTERED ATTACH 
  NEEDED state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_attach_needed_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type     rrc_reported_plmn;
  lte_nas_tai_lst1_type  tai;

#ifdef FEATURE_FEMTO_CSG
  sys_csg_info_s_type rrc_reported_csg; 
#endif 
#ifdef FEATURE_SGLTE
  boolean mm_is_sglte_plmn_in_roam;
#endif
  ASSERT(rrc_service_ind != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
    tai.plmn = rrc_reported_plmn;
    tai.tac = rrc_service_ind->camped_svc_info.tac;
#ifdef FEATURE_FEMTO_CSG
    rrc_reported_csg = rrc_service_ind->camped_svc_info.csg_info;
#endif 
    /* If the timer is running compare with prev PLMN and TAC */
    if(((mm_timer_status[TIMER_T3402] != TIMER_STOPPED) ||
        (mm_timer_status[TIMER_T3411] != TIMER_STOPPED)) 
       && 
       (emm_chk_ta_change(rrc_service_ind, emm_ctrl_data_ptr) == FALSE) 
       &&
       (emm_is_manual_req_pending(emm_ctrl_data_ptr) == FALSE))
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TA not changed, start attach after T3402/T3411 expiry ");
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
      if(mm_timer_status[TIMER_T3402] != TIMER_STOPPED)
      {
        /* T3402 is running, report limited service */
        reg_service_report(MMR_CAUSE_FAILURE_MAX_ATTEMPTED, 
                           FALSE, 
                           emm_ctrl_data_ptr);
      }
      else
      {
        /* T3411 is running, send camp ind to CM indicating UE is on LTE */
        emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                               rrc_service_ind->camped_svc_info, FALSE);
      }
    }
    else
    {       
      switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
      {
        case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
        case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
          if(emm_policy_change_pending(rrc_service_ind) == TRUE)
          {
            emm_send_reg_attach_failed_ind();
            return;
          }
#ifdef FEATURE_SGLTE
          mm_is_sglte_plmn_in_roam= 
			     mm_is_sglteplmn_in_sglte_roam(emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   rrc_service_ind->camped_svc_info.selected_plmn));
#endif
          if((((emm_is_plmn_forbidden(rrc_reported_plmn,emm_ctrl_data_ptr) == FALSE)&&
             (emm_is_tai_forbidden(tai,emm_ctrl_data_ptr)== FALSE)
#ifdef FEATURE_FEMTO_CSG
             && (mm_is_csg_in_white_list(rrc_reported_plmn, &rrc_reported_csg) == TRUE)
#endif 
             )
#ifdef FEATURE_FEMTO_CSG
            || (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
#endif
            )
            &&
             (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == TRUE) &&
             (rrc_service_ind->camped_svc_info.cell_access_status != 
                                           LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)
#ifdef FEATURE_SGLTE
              && (mm_is_sglte_plmn_in_roam == FALSE)
#endif
#ifdef FEATURE_DUAL_DATA
              && (mm_is_volte_registration_allowed(rrc_reported_plmn, FALSE) == TRUE)
#endif
            )
          {
            emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                                 rrc_service_ind->camped_svc_info, FALSE);                          

			
			if((emm_chk_ta_change(rrc_service_ind, emm_ctrl_data_ptr) == FALSE) &&
               ((emm_ctrl_data_ptr->emm_attach_attempt_counter >0)&&
               (emm_ctrl_data_ptr->emm_attach_attempt_counter < MAX_NUMBER_ATTACH_OF_ATTEMPTS)))
		    {
               emm_init_attach_procedure(EMM_UPDATE_RECOVERY, emm_ctrl_data_ptr);
			}
			else
			{
               emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
			}                  
            
          }
          else
          {
#ifdef FEATURE_SGLTE
            if (mm_is_sglte_plmn_in_roam == TRUE)
            {
              emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
              reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);
              MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited to REG, REG will send Limited Regional to CM ");
            }
            else
#endif
            {
              emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
              reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
            }
            emm_send_reg_attach_failed_ind();                             
          }
          break;
        case SYS_NETWORK_SELECTION_MODE_MANUAL:
        case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
          if(emm_policy_change_pending(rrc_service_ind) == TRUE)
          {
            emm_send_reg_attach_failed_ind();
            return;
          }
#ifdef FEATURE_SGLTE
          mm_is_sglte_plmn_in_roam= 
          mm_is_sglteplmn_in_sglte_roam(emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   rrc_service_ind->camped_svc_info.selected_plmn));
#endif
          if((emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE)
#ifdef FEATURE_SGLTE
              && (mm_is_sglte_plmn_in_roam == FALSE)
#endif
          )
          {
            emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                                 rrc_service_ind->camped_svc_info, FALSE);
            if((emm_chk_ta_change(rrc_service_ind, emm_ctrl_data_ptr) == FALSE) &&
               (emm_is_manual_req_pending(emm_ctrl_data_ptr) == FALSE) &&
               ((emm_ctrl_data_ptr->emm_attach_attempt_counter >0)&&
               (emm_ctrl_data_ptr->emm_attach_attempt_counter < MAX_NUMBER_ATTACH_OF_ATTEMPTS)))
		    {
               emm_init_attach_procedure(EMM_UPDATE_RECOVERY, emm_ctrl_data_ptr);
			}
			else
			{
               emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
			} 
          }
          else
          {
#ifdef FEATURE_SGLTE
            if (mm_is_sglte_plmn_in_roam == TRUE)
            {
              emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
              reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);              
              MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited TO REG, REG will send Limited Regional to CM ");
            }
            else
#endif
            {
              emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
              reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
            }
            

            emm_send_reg_attach_failed_ind();                             
          }
          break;
        case SYS_NETWORK_SELECTION_MODE_LIMITED:
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
          break;
#ifdef FEATURE_FEMTO_CSG
        case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
          if((emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
             && (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == TRUE) 
             && (rrc_service_ind->camped_svc_info.cell_access_status != 
                                           LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY))
          {
            emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                                rrc_service_ind->camped_svc_info, FALSE);
            emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
          }
          else
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }
          break;
#endif
        default:
          MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW Sel Mode - %u",
                         emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
          break;
      }
    }
  }
  else if (rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_NOT_AVAILABLE)
  {
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                          SYS_SRV_STATUS_NO_SRV;
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

    emm_send_reg_attach_failed_ind();                                  
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE); 
  }

} /* end of emm_attach_needed_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_WAITING_PDN_CONN_REQ_RRC_SERVICE_IND

DESCRIPTION
  This function processes the RRC SERVICE IND recived in DEREGISTERED WAITING 
  PDN CONN REQ state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_waiting_pdn_conn_req_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type     rrc_reported_plmn;
  lte_nas_tai_lst1_type  tai;
#ifdef FEATURE_FEMTO_CSG
  sys_csg_info_s_type rrc_reported_csg;
#endif 
  ASSERT(rrc_service_ind != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  tai.plmn = rrc_reported_plmn;
  tai.tac = rrc_service_ind->camped_svc_info.tac;

#ifdef FEATURE_FEMTO_CSG
  rrc_reported_csg = rrc_service_ind->camped_svc_info.csg_info;
#endif 

  switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
  {
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
      if((((emm_is_plmn_forbidden(rrc_reported_plmn,emm_ctrl_data_ptr) == TRUE) ||
            (emm_is_tai_forbidden(tai,emm_ctrl_data_ptr) == TRUE)
#ifdef FEATURE_FEMTO_CSG
         || (mm_is_csg_in_white_list(rrc_reported_plmn, &rrc_reported_csg) == FALSE) 
#endif 
         )
#ifdef FEATURE_FEMTO_CSG
        && (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == FALSE)
#endif 
         )
         ||
         (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == FALSE) 
         ||
         (rrc_service_ind->camped_svc_info.cell_access_status == 
                                           LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY) ||
         (emm_policy_change_pending(rrc_service_ind) == TRUE)
#ifdef FEATURE_DUAL_DATA
         ||
         (mm_is_volte_registration_allowed(rrc_reported_plmn, FALSE) == FALSE)
#endif

        )
      {
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
      }
      break;
    case SYS_NETWORK_SELECTION_MODE_MANUAL:
    case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
      if((emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == FALSE) ||
         (emm_policy_change_pending(rrc_service_ind) == TRUE))
      {
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
      }
      break;
    case SYS_NETWORK_SELECTION_MODE_LIMITED:
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
      break;
#ifdef FEATURE_FEMTO_CSG
    case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
      if((emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == FALSE)
         ||
         (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == FALSE) 
         ||
         (rrc_service_ind->camped_svc_info.cell_access_status == 
                                           LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY))
      {
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
      }  
      break;
#endif
    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW Sel Mode - %u",
                     emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
      break;
  }

} /* end of emm_waiting_pdn_conn_req_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_NO_SERVICE_RRC_SERVICE_IND

DESCRIPTION
  This function processes the RRC SERVICE IND recived in DEREGISTERED NO CELL
  AVAILABLE state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_no_service_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type  tai;
  sys_plmn_id_s_type rrc_reported_plmn ;
#ifdef FEATURE_SGLTE
  boolean mm_is_sglte_plmn_in_roam;
#endif
  ASSERT(rrc_service_ind != (lte_rrc_service_ind_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  tai.tac = rrc_service_ind->camped_svc_info.tac;

  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(rrc_service_ind->camped_svc_info.selected_plmn);

  if (rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
    {
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
        if(emm_policy_change_pending(rrc_service_ind) == TRUE)
        {
          emm_send_reg_attach_failed_ind();
          return;
        }
#ifdef FEATURE_SGLTE
        mm_is_sglte_plmn_in_roam= 
        mm_is_sglteplmn_in_sglte_roam(emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   rrc_service_ind->camped_svc_info.selected_plmn));
#endif
        if((emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE)
#ifdef FEATURE_SGLTE
              && (mm_is_sglte_plmn_in_roam == FALSE)
#endif
              &&(mm_timer_status[EMM_POWEROFF_DETACH_TIMER] != TIMER_ACTIVE)
        )
        {
          // Do not start an attach if power-off detach due to CM_STOP_MODE is pending.
          emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                               rrc_service_ind->camped_svc_info, FALSE);
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
#ifdef FEATURE_SGLTE
          if (mm_is_sglte_plmn_in_roam == TRUE)
          {
             emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
             reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);
             MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited TO REG, REG will send Limited Regional to CM ");
          }
          else
#endif
          { 
             emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
             reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }
          
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_MANUAL:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
        if(emm_policy_change_pending(rrc_service_ind) == TRUE)
        {
          emm_send_reg_attach_failed_ind();
          return;
        }
#ifdef FEATURE_SGLTE
        mm_is_sglte_plmn_in_roam= 
        mm_is_sglteplmn_in_sglte_roam(emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   rrc_service_ind->camped_svc_info.selected_plmn));
#endif
        if((emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE)
#ifdef FEATURE_SGLTE
            && (mm_is_sglte_plmn_in_roam == FALSE)
#endif
            &&(mm_timer_status[EMM_POWEROFF_DETACH_TIMER] != TIMER_ACTIVE) 
        )
        {
          // Do not start an attach if power-off detach due to CM_STOP_MODE is pending.
          emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                                  rrc_service_ind->camped_svc_info, FALSE);
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
#ifdef FEATURE_SGLTE
          if (mm_is_sglte_plmn_in_roam == TRUE)
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);
            MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited TO REG, REG will send Limited Regional to CM ");
          }
          else
#endif   
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }
          
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
        if(emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE)            
        {
          emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                                 SYS_REG_DOMAIN_PS_SERVICE_ONLY;
        }
        else 
        {
          emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                                 SYS_REG_DOMAIN_CAMPED_ONLY;
        }
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                 SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
        break;
      case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
        if(emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE)
        {
          emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                                 SYS_REG_DOMAIN_PS_SERVICE_ONLY;
        }
        else 
        {
          emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                                 SYS_REG_DOMAIN_CAMPED_ONLY;
        }
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                 SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
        break;
      case SYS_NETWORK_SELECTION_MODE_LIMITED:
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
        break;
#ifdef FEATURE_FEMTO_CSG
        case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
      if((emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == FALSE)
         && 
         (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == FALSE) 
         &&
         (rrc_service_ind->camped_svc_info.cell_access_status == 
                                           LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)
         &&(mm_timer_status[EMM_POWEROFF_DETACH_TIMER] != TIMER_ACTIVE)
         )
      {
        // Do not start an attach if power-off detach due to CM_STOP_MODE is pending.
        emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                               rrc_service_ind->camped_svc_info, FALSE);
        emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
      }
      else
      {
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
      }
      break;
#endif
      default:
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW Sel Mode - %u",
                       emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
        break;
    }
  }
  else if (rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_NOT_AVAILABLE)
  {
    sys_plmn_id_s_type null_plmn = {{0x00, 0x00, 0x00}} ;

    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                         SYS_SRV_STATUS_NO_SRV;

    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

    if (PLMN_MATCH(emm_ctrl_data_ptr->plmn_service_state->plmn.identity,rrc_reported_plmn.identity))
    {
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE) ; 
    }
    else
    {
      if (!PLMN_MATCH(emm_ctrl_data_ptr->plmn_service_state->plmn.identity,null_plmn.identity))
      {
        
        emm_send_rrc_service_req(SYS_NETWORK_SELECTION_MODE_AUTOMATIC,
                                 TRUE,
                                 &emm_ctrl_data_ptr->plmn_service_state->plmn,
                                 TRUE,
                                 TRUE,
                                 FALSE,
                                 FALSE,
                                 FALSE,
                                 0,
                                 emm_ctrl_data_ptr,
                                 &emm_ctrl_data_ptr->reg_req_pending_ptr->rat_pri_list_info,
                                 emm_ctrl_data_ptr->reg_req_pending_ptr->type
#ifdef FEATURE_LTE_REL9
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->additional_info
#endif                      
#ifdef FEATURE_FEMTO_CSG
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->csg_id
#endif 
                                ,SYS_SCAN_SCOPE_FULL_BAND 
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->trm_timeout
                                );
      }
    }
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Invalid Service Indication");
  }
} /* end of emm_no_service_rrc_service_ind()*/

/*===========================================================================

FUNCTION  EMM_ATTEMPTING_TO_ATTACH_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received in DEREGISTERED ATTEMPTING 
  TO ATTACH state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_attempting_to_attach_rrc_service_ind
(
  lte_rrc_service_ind_s  *rrc_service_ind,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type  srving_tai;
#ifdef FEATURE_FEMTO_CSG
  sys_csg_info_s_type rrc_reported_csg;
#endif 
#ifdef FEATURE_SGLTE
  boolean mm_is_sglte_plmn_in_roam;
#endif

  ASSERT(rrc_service_ind != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);
  /*
  ** Allow attach for an higher access UE even when T3346 is running
  */
  if((emm_chk_ta_change(rrc_service_ind, emm_ctrl_data_ptr) == FALSE) 
#ifdef FEATURE_LTE_REL11
     &&
     ((emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL11) ||
      (mm_timer_status[TIMER_T3346] == TIMER_STOPPED) ||
      (emm_check_high_priority(emm_ctrl_data_ptr) == FALSE))
#endif
    )
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= No TAI chg - Attach to be resumed upon T3411/3402 expiry");
  }
  else
  {       
    srving_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
    srving_tai.tac = rrc_service_ind->camped_svc_info.tac;
#ifdef FEATURE_FEMTO_CSG
  rrc_reported_csg = rrc_service_ind->camped_svc_info.csg_info;
#endif 

    switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
    {
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
        if(emm_policy_change_pending(rrc_service_ind) == TRUE)
        {
          emm_send_reg_attach_failed_ind();
          return;
        }
#ifdef FEATURE_SGLTE
        mm_is_sglte_plmn_in_roam= 
        mm_is_sglteplmn_in_sglte_roam(emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   rrc_service_ind->camped_svc_info.selected_plmn));
#endif
        if((((emm_is_plmn_forbidden(srving_tai.plmn, emm_ctrl_data_ptr) == FALSE)
           &&
           (emm_is_tai_forbidden(srving_tai, emm_ctrl_data_ptr)== FALSE)
#ifdef FEATURE_FEMTO_CSG
          && (mm_is_csg_in_white_list(srving_tai.plmn, &rrc_reported_csg) == TRUE)
#endif 
             )
#ifdef FEATURE_FEMTO_CSG
            || (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
#endif
            ) &&
           (emm_validate_attach_plmn(emm_ctrl_data_ptr, &srving_tai.plmn) == TRUE)
           &&
           (rrc_service_ind->camped_svc_info.cell_access_status != 
                                             LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)
#ifdef FEATURE_SGLTE
              && (mm_is_sglte_plmn_in_roam == FALSE)
#endif
#ifdef FEATURE_DUAL_DATA
              && (mm_is_volte_registration_allowed(srving_tai.plmn, FALSE) == TRUE)
#endif
           )
        {
          emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                               rrc_service_ind->camped_svc_info, FALSE);
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
#ifdef FEATURE_SGLTE
          if (mm_is_sglte_plmn_in_roam == TRUE)
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                      SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);
            MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited to REG, REG will send Limited Regional to CM ");
          }
          else
#endif
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }
          
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_MANUAL:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
        if(emm_policy_change_pending(rrc_service_ind) == TRUE)
        {
          emm_send_reg_attach_failed_ind();
          return;
        }
#ifdef FEATURE_SGLTE
        mm_is_sglte_plmn_in_roam= 
        mm_is_sglteplmn_in_sglte_roam(emm_convert_rrc_plmn_id_to_nas_plmn_id(
                   rrc_service_ind->camped_svc_info.selected_plmn));
#endif
        if((emm_is_manual_reg_allowed(srving_tai, emm_ctrl_data_ptr) == TRUE)
#ifdef FEATURE_SGLTE
              && (mm_is_sglte_plmn_in_roam == FALSE)
#endif
        )
        {
          emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                               rrc_service_ind->camped_svc_info, FALSE);
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
#ifdef FEATURE_SGLTE
          if (mm_is_sglte_plmn_in_roam == TRUE)
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                      SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);
            MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited to REG, REG will send Limited Regional to CM ");
          }
          else
#endif
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_LIMITED:
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
        break;
#ifdef FEATURE_FEMTO_CSG
      case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
        if((emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
           && (emm_validate_attach_plmn(emm_ctrl_data_ptr,&srving_tai.plmn) == TRUE) 
           && (rrc_service_ind->camped_svc_info.cell_access_status != 
                                         LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY))
        {
          emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                              rrc_service_ind->camped_svc_info, FALSE);
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                      SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
        }
        break;
#endif
      default:
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW Sel Mode - %u",
                       emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
        break;
    }
  }
} /* end of emm_attempting_to_attach_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_NORMAL_SERVICE_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received in DEREGISTERED NORMAL 
  SERVICE state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_normal_service_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type rrc_reported_plmn ;
  lte_nas_tai_lst1_type  tai;
#ifdef FEATURE_FEMTO_CSG
  sys_csg_info_s_type rrc_reported_csg;
#endif 
#ifdef FEATURE_SGLTE
  boolean mm_is_non_sglte_plmn_in_home;
#endif

  ASSERT(rrc_service_ind != (lte_rrc_service_ind_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  tai.tac = rrc_service_ind->camped_svc_info.tac;


  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(rrc_service_ind->camped_svc_info.selected_plmn);

#ifdef FEATURE_FEMTO_CSG
  rrc_reported_csg = rrc_service_ind->camped_svc_info.csg_info;
#endif 

  if (rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
    {
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
        if(emm_policy_change_pending(rrc_service_ind) == TRUE)
        {
          emm_send_reg_attach_failed_ind();
          return;
        }
#ifdef FEATURE_SGLTE
        mm_is_non_sglte_plmn_in_home = mm_is_non_sglte_plmn_in_sglte_home(mm_serving_plmn.info.plmn);
#endif
        if((((emm_is_plmn_forbidden(tai.plmn, emm_ctrl_data_ptr) == FALSE)
           &&
           (emm_is_tai_forbidden(tai, emm_ctrl_data_ptr)== FALSE)
#ifdef FEATURE_FEMTO_CSG
           && (mm_is_csg_in_white_list(rrc_reported_plmn, &rrc_reported_csg) == TRUE)
#endif 
             )
#ifdef FEATURE_FEMTO_CSG
            || (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
#endif
            ) &&
           (emm_validate_attach_plmn(emm_ctrl_data_ptr, &tai.plmn) == TRUE)
           &&
           (rrc_service_ind->camped_svc_info.cell_access_status != 
                                             LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)
           
#ifdef FEATURE_SGLTE
           &&(mm_is_non_sglte_plmn_in_home==FALSE) 
#endif
#ifdef FEATURE_DUAL_DATA
           && (mm_is_volte_registration_allowed(rrc_reported_plmn, FALSE) == TRUE)
#endif
       )
        {
          emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                               rrc_service_ind->camped_svc_info, FALSE);
#ifdef FEATURE_SGLTE
          if ( mm_process_sglte_plmn_state4_gsm_oos(mm_serving_plmn.info.plmn)==TRUE )
          {
             MSG_HIGH_DS_0(MM_SUB, "EMM:LTE got SGLTE PLMN in State4 While GSM OOS ");
          }
#endif
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
#ifdef FEATURE_SGLTE
          if(mm_is_non_sglte_plmn_in_home == TRUE)
          {
             emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
             MSG_HIGH_DS_0(MM_SUB, "EMM: Found NON SGLTE PLMN in SGLTE HOME sending Limited to REG, REG will send Limited Regional to CM ");
             reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);           
          }
          else
#endif
          { 
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }          
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_MANUAL:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
        if(emm_policy_change_pending(rrc_service_ind) == TRUE)
        {
          emm_send_reg_attach_failed_ind();
          return;
        }
#ifdef FEATURE_SGLTE
        mm_is_non_sglte_plmn_in_home = mm_is_non_sglte_plmn_in_sglte_home(mm_serving_plmn.info.plmn);
#endif
        if((emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE) 
#ifdef FEATURE_SGLTE
           &&(mm_is_non_sglte_plmn_in_home==FALSE) 
#endif
         )
        {
          emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                               rrc_service_ind->camped_svc_info, FALSE);
#ifdef FEATURE_SGLTE
          if ( mm_process_sglte_plmn_state4_gsm_oos(mm_serving_plmn.info.plmn)==TRUE )
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM=LTE got SGLTE PLMN in State4 While GSM OOS ");
          }
#endif
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
#ifdef FEATURE_SGLTE
          if(mm_is_non_sglte_plmn_in_home == TRUE)
          {
             emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
             reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,FALSE,emm_ctrl_data_ptr);
             MSG_HIGH_DS_0(MM_SUB, "EMM: Found SGLTE PLMN in SGLTE ROAM sending limited to REG, REG will send Limited Regional to CM ");
          }
          else
#endif    
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
          }
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_LIMITED:
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
        break;
#ifdef FEATURE_FEMTO_CSG
      case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
        if((emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
           && (emm_validate_attach_plmn(emm_ctrl_data_ptr,&tai.plmn) == TRUE) 
           && (rrc_service_ind->camped_svc_info.cell_access_status != 
                                         LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY))
        {
          emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                              rrc_service_ind->camped_svc_info, FALSE);
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
        else
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                      SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
        }
        break;
#endif
      default:
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW Sel Mode - %u",
                       emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
        break;
    }   
  }
  else if (rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_NOT_AVAILABLE)
  {
    sys_plmn_id_s_type null_plmn = {{0x00, 0x00, 0x00}} ;

    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_NO_SRV;

    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

    if (PLMN_MATCH(emm_ctrl_data_ptr->plmn_service_state->plmn.identity,rrc_reported_plmn.identity))
    {
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE) ; 
    }
    else
    {
      if (!PLMN_MATCH(emm_ctrl_data_ptr->plmn_service_state->plmn.identity,null_plmn.identity))
      {
        
        emm_send_rrc_service_req(SYS_NETWORK_SELECTION_MODE_AUTOMATIC,
                                 TRUE,
                                 &emm_ctrl_data_ptr->plmn_service_state->plmn,
                                 TRUE,
                                 TRUE,
                                 FALSE,
                                 FALSE,
                                 FALSE,
                                 0,
                                 emm_ctrl_data_ptr,
                                 &emm_ctrl_data_ptr->reg_req_pending_ptr->rat_pri_list_info,
                                 emm_ctrl_data_ptr->reg_req_pending_ptr->type
#ifdef FEATURE_LTE_REL9
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->additional_info
#endif         
#ifdef FEATURE_FEMTO_CSG
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->csg_id
#endif 
                                ,SYS_SCAN_SCOPE_FULL_BAND 
                                ,emm_ctrl_data_ptr->reg_req_pending_ptr->trm_timeout
                                );
      }
    }
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Invalid Service Indication");
  }
} /* end of emm_normal_service_rrc_service_ind() */

/*===========================================================================

FUNCTION   EMM_PLMN_SEARCH_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received in DEREGISTERED PLMN 
  SEARCH state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_plmn_search_rrc_service_ind
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != NULL); 

  emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
  reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

} /* end of emm_plmn_search_rrc_service_ind() */

/*===========================================================================

FUNCTION   EMM_LIMITED_SERVICE_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received in DEREGISTERED LIMITED 
  SERVICE state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_limited_service_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type  tai;

  ASSERT(rrc_service_ind != (lte_rrc_service_ind_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  tai.tac = rrc_service_ind->camped_svc_info.tac;

  emm_get_plmn_state_info(SYS_SRV_STATUS_LIMITED, emm_ctrl_data_ptr);

  switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
  {
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
      if(emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE)            
      {
        emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                               SYS_REG_DOMAIN_PS_SERVICE_ONLY;
      }
      else 
      {
        emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                               SYS_REG_DOMAIN_CAMPED_ONLY;
      }
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                               SYS_SRV_STATUS_LIMITED;
      break;
    case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
      if(emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE)
      {
        emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                               SYS_REG_DOMAIN_PS_SERVICE_ONLY;
      }
      else 
      {
        emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                               SYS_REG_DOMAIN_CAMPED_ONLY;
      }
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                               SYS_SRV_STATUS_LIMITED;
      break;
#ifdef FEATURE_LTE_REL9
    case SYS_NETWORK_SELECTION_MODE_LIMITED:
      if((emm_bit_mask_check(emm_ctrl_data_ptr->reg_req_pending_ptr->additional_info,
                                        (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE) &&
         (emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE))
      {
        emm_ctrl_data_ptr->plmn_service_state->extend_srv_info = 
                                         SYS_EXTEND_SRV_INFO_FULL_SRV_POSSIBLE;
      }
      emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS,     
                                        rrc_service_ind->camped_svc_info, FALSE); 
      break;
#endif
    default:
      break;
  }

  reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
  
} /* end of emm_limited_service_rrc_service_ind() */

/*===========================================================================

FUNCTION   EMM_DEREGISTERED_PROCESS_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received in DEREGISTERED state
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_deregistered_process_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  emm_deregistered_substate_type emm_substate ;

  ASSERT(rrc_service_ind != (lte_rrc_service_ind_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  if((emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == TRUE) ||
     (emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == TRUE))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Deacting RRC - Do not proc srv ind further");
    return;
  }

  emm_substate = (emm_deregistered_substate_type)EMM_GET_SUBSTATE() ;

  if((emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain != 
                                                        SYS_SRV_DOMAIN_PS_ONLY)
     &&
     (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain != 
                                                         SYS_SRV_DOMAIN_CS_PS))
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid Service Domain %d",
                   emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain);

    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);

    /* Report no service */
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                         SYS_SRV_STATUS_NO_SRV;
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
  }
  else
  {  
    switch(emm_substate)
    {

      case EMM_DEGEGISTERED_WAITING_PDN_CONN_REQ:
        emm_waiting_pdn_conn_req_rrc_service_ind(rrc_service_ind,
                                                 emm_ctrl_data_ptr);
        break ;
      case EMM_DEREGISTERED_PLMN_SEARCH:
        emm_plmn_search_rrc_service_ind(emm_ctrl_data_ptr);
        break;
      case EMM_DEREGISTERED_ATTACH_NEEDED:
        emm_attach_needed_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr);
        break ;
      case EMM_DEREGISTERED_NO_CELL_AVAILABLE:
        emm_no_service_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      case EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH:
        emm_attempting_to_attach_rrc_service_ind(rrc_service_ind,
                                                 emm_ctrl_data_ptr);
        break ;
      case EMM_DEREGISTERED_NORMAL_SERVICE:
        emm_normal_service_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      case EMM_DEREGISTERED_LIMITED_SERVICE:
      case EMM_DEREGISTERED_NO_IMSI:
        emm_limited_service_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      default:
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal Deregistered substate",0,0,0) ;
        break ;
    }
  }

} /* end of emm_deregistered_process_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_ATTACH_COLLISION_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND recived during ATTACH procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_attach_collision_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  sys_plmn_id_s_type  rrc_reported_plmn ;

  ASSERT(rrc_service_ind != (lte_rrc_service_ind_s *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);

  if(emm_policy_change_pending(rrc_service_ind) == TRUE)
  {
    emm_remove_release_client(emm_attach_release_indication);
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
    emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
    emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;
    emm_clean_msg_buffer(emm_ctrl_data_ptr);

    /* Report to ESM and invalidate pdn conn req*/
    emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);

    /* Notify ESM */
    emm_build_and_send_detach_ind(EMM_DETACHED);

    emm_send_reg_attach_failed_ind();
    return;
  }
  if((sys_plmn_match(rrc_reported_plmn, 
                     emm_ctrl_data_ptr->last_attempted_tai.plmn) != TRUE)
     ||
     (rrc_service_ind->camped_svc_info.tac != 
                                    emm_ctrl_data_ptr->last_attempted_tai.tac))
  {
    if((rrc_service_ind->camped_svc_info.cell_access_status != 
                                            LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)
#ifdef FEATURE_LTE_REL9
      ||(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING)
#endif
      )
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TAI changed - Reiniting ATTACH.");
      emm_init_attach_procedure(EMM_INITIAL_UPDATE,emm_ctrl_data_ptr);
    }
    else
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Attach init state, cell access emergency only");
      mm_stop_timer(TIMER_T3410);
      emm_remove_release_client(emm_attach_release_indication);
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
      emm_clean_msg_buffer(emm_ctrl_data_ptr);

      /* Report to ESM and invalidate pdn conn req*/
      emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);

      /* Notify ESM */
      emm_build_and_send_detach_ind(EMM_DETACHED);
    }
  }

} /* end of emm_attach_collision_rrc_service_ind() */

/*===========================================================================

FUNCTION    EMM_REGISTERED_PROCESS_POSSIBLE_TAU_RRC_SERVICE_IND

DESCRIPTION
  This function checks whether lte_rrc_service_ind_s primitive requires to 
  start TAU in REGISTERED state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If TAU is not required but TAI has been changed this function modifs the 
  following global control vars:
    o last_registered_tai_valid
    o last_registered_tai
    o plmn_service_state
  This function may initiate TAU procedure or change EMM substate to
  EMM_MOVES_TO_REGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE)
===========================================================================*/
void emm_registered_process_possible_tau_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type    current_tai;
  boolean                  ta_in_tai_list = FALSE;
  boolean                  start_tau = FALSE;
  emm_update_attempt_type update_type = EMM_INITIAL_UPDATE;
#ifdef FEATURE_FEMTO_CSG
  sys_csg_info_s_type  csg_info;
#endif 
  ASSERT(rrc_service_ind != (lte_rrc_service_ind_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  /* Create NAS current TAI */
  current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(rrc_service_ind->camped_svc_info.selected_plmn) ;
  current_tai.tac = rrc_service_ind->camped_svc_info.tac ;
#ifdef FEATURE_FEMTO_CSG
  csg_info = rrc_service_ind->camped_svc_info.csg_info;
#endif 

#ifdef FEATURE_SGLTE
  if(mm_timer_status[TIMER_DELAY_TAU] == TIMER_ACTIVE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Delay TAU running, start tau on timer expiry");
    return ;
  }
#endif

  /* Send camped ind if necessary */
  if(emm_ctrl_data_ptr->reg_req_pended_indication == TRUE)
  {
    switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
    {
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
        if(
#ifdef FEATURE_LTE_REL9
           (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) 
           ||
#endif
           ((((emm_is_plmn_forbidden(current_tai.plmn, emm_ctrl_data_ptr) 
                                                                   == FALSE) &&
            (emm_is_tai_forbidden(current_tai, emm_ctrl_data_ptr) == FALSE)
#ifdef FEATURE_FEMTO_CSG
           && (mm_is_csg_in_white_list(current_tai.plmn, &csg_info) == TRUE)
#endif 
           )
#ifdef FEATURE_FEMTO_CSG
          || (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)
#endif 
             )
#ifdef FEATURE_DUAL_DATA
           && (mm_is_volte_registration_allowed(current_tai.plmn, FALSE) == TRUE)
#endif
           && (rrc_service_ind->camped_svc_info.cell_access_status != 
                                          LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY)) 
           )
        {
          emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                           rrc_service_ind->camped_svc_info, FALSE);
        }
        else
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED,
                             FALSE,
                             emm_ctrl_data_ptr);
          return;
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_MANUAL:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
        if(
#ifdef FEATURE_LTE_REL9
           (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) 
           ||
#endif
           (emm_is_manual_reg_allowed(current_tai, emm_ctrl_data_ptr) == TRUE))
        {
          emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                               rrc_service_ind->camped_svc_info, FALSE);
          /* User manual PLMN selection. If a TAU is necessary and allowed, UE 
             should the TAU regardless T3402/T3411 status. Stop these timers 
             here if running. */
          mm_stop_timer(TIMER_T3402);
          mm_stop_timer(TIMER_T3411);
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
          mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

        }
        else
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED,
                             FALSE,
                             emm_ctrl_data_ptr);
          return;
        }
        break;
      case SYS_NETWORK_SELECTION_MODE_LIMITED:
#ifdef FEATURE_LTE_REL9
        if(emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)
#endif
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                          SYS_SRV_STATUS_LIMITED;
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED,
                             FALSE,
                             emm_ctrl_data_ptr);
          return;
        }
#ifdef FEATURE_LTE_REL9
        break;
#endif
#ifdef FEATURE_FEMTO_CSG
        case SYS_NETWORK_SELECTION_MODE_MANUAL_CSG:
          if((emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE)            
             && (rrc_service_ind->camped_svc_info.cell_access_status != 
                                           LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY))
          {
            emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                           rrc_service_ind->camped_svc_info, FALSE);
          }
          else
          {
            emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
            reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
            return;
          }
          break;
#endif
      default:
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid NW Sel Mode in REG REQ - %d",
                       emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
        return;
    }
  }
  /* 
  ** If UE camps on a different PLMN from the manually requested PLMN in limited routine
  ** manual then return limited service to REG/CM.
  */
  else if((emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == 
                          SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION) &&
          (emm_is_manual_reg_allowed(current_tai, emm_ctrl_data_ptr) == FALSE))
  {
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                          SYS_SRV_STATUS_LIMITED;

    if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) || 
       (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED))
    {
       emm_process_buffered_csfb_hard_failure();
    }

    reg_service_report(MMR_CAUSE_NOT_SPECIFIED,
                       FALSE,
                       emm_ctrl_data_ptr);
    return;
  }
  else if(
#ifdef FEATURE_LTE_REL9
          (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
          &&
#endif
          ((((emm_is_plmn_forbidden(current_tai.plmn, emm_ctrl_data_ptr) == TRUE)
           ||(emm_is_tai_forbidden(current_tai, emm_ctrl_data_ptr) == TRUE) 
#ifdef FEATURE_FEMTO_CSG
           || (mm_is_csg_in_white_list(current_tai.plmn, &csg_info) == FALSE)            
#endif
           ) &&
          ((emm_is_manual_req_pending(emm_ctrl_data_ptr) == FALSE) ||
           (emm_is_manual_reg_allowed(current_tai, emm_ctrl_data_ptr) == FALSE)
#ifdef FEATURE_FEMTO_CSG
           || (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == FALSE)
#endif 
#ifdef FEATURE_DUAL_DATA
           || (mm_is_volte_registration_allowed(current_tai.plmn, FALSE) == FALSE)
#endif
         ))))
  {
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                          SYS_SRV_STATUS_LIMITED;

    if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) || 
       (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED))
    {
       emm_process_buffered_csfb_hard_failure();
    }

    reg_service_report(MMR_CAUSE_NOT_SPECIFIED,
                       FALSE,
                       emm_ctrl_data_ptr);
    return;
  }
  else if (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_LIMITED
#ifdef FEATURE_LTE_REL9
      && emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY
#endif
     )
  {
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                       SYS_SRV_STATUS_LIMITED;
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
    return;
  }
#ifdef FEATURE_SGLTE
  if ((mm_process_if_both_stack_has_camped_non_sglte_plmn(mm_get_ps_stack_id())== TRUE)||
     ((mm_is_sglteplmn_in_sglte_roam(current_tai.plmn ))==TRUE))
  {
    MSG_HIGH_DS_0(MM_SUB, "EMM: Sent Limited regional As UE MODE change pending ");
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
    
    reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN,
                       FALSE,
                       emm_ctrl_data_ptr);
    return ;
  }
  else if ( mm_process_sglte_plmn_state4_gsm_oos(mm_serving_plmn.info.plmn)==TRUE )
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM=LTE got SGLTE PLMN in State4 While GSM OOS ");
  }
#endif
  if(emm_policy_change_pending(rrc_service_ind) == TRUE)
  {
    emm_send_reg_attach_failed_ind();
    return;
  }
  ta_in_tai_list = emm_search_tai_list(&current_tai,
                                       emm_ctrl_data_ptr->emm_tai_list_ptr);

  /* UE comes out of no service. Set service status. */
  if(emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                         SYS_SRV_STATUS_NO_SRV)
  {
    if((ta_in_tai_list == TRUE) && 
       (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED)
#ifdef FEATURE_LTE_REL9
       &&
       (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)
#endif 
#ifdef FEATURE_FEMTO_CSG
       && (mm_is_csg_in_white_list(current_tai.plmn, &csg_info) == TRUE)
#endif 
#ifdef FEATURE_DUAL_DATA
       && (mm_is_volte_registration_allowed(current_tai.plmn, FALSE) == TRUE)
#endif
      )
    {
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                            SYS_SRV_STATUS_SRV;
    }
    else
    {
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
    }
  }

#ifdef FEATURE_LTE_REL10
    if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) 
    {
      /* if there is not active bearer & there is no PS detach pending then start Attach procedure */ 
      if((emm_convert_eps_bc_status_to_bit_mask(
              emm_ctrl_data_ptr->eps_bearer_context_status) == 0) &&
         (emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_NONE))
      {
        /* for Tau rej#40 we move it to DE-REG NORMAL service*/
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE);
        MSG_HIGH_DS_0(MM_SUB, "=EMM= no active bearers, start attach procedure");
        /* Start initial type attach procedure */
        emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr); 
        return;
      }
    }
#endif

  /* Check whether current RRC reported TAI belongs to the current TAI list */
  if(ta_in_tai_list == TRUE)
  {
    /* Current TAI IS part of the current TAI list */
    MSG_HIGH_DS_3(MM_SUB, "=EMM= TA in TAI list - US %d, T3402 %d, T3411 %d",
                  emm_ctrl_data_ptr->emm_update_status,
                  mm_timer_status[TIMER_T3402],
                  mm_timer_status[TIMER_T3411]);

    if((emm_ctrl_data_ptr->reg_complete_ack_pending != EMM_PENDING_REG_ACK_NONE) &&
       ((emm_ctrl_data_ptr->ps_resumption_status == EMM_PS_RESUMPTION_PENDING) ||
        (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC)||
        (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_DUE_TO_SRVCC)))
    {
      /* Do not start TAU here. Let ATTACH/TAU COMPLETE NAK to trigger reattach
         or TA change TAU. */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= ATTACH/TAU COMPLETE ACK pending, restart proc on NAK");
      return;
    }

    if(emm_ctrl_data_ptr->emm_update_status != EMM_STATUS_UPDATED)
    {
#ifdef FEATURE_1XSRLTE
      if(emm_ctrl_data_ptr->srlte_tau_needed == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU delayed in 1X SRLTE mode until call end on other stack");
      }
      else
#endif
      if((mm_timer_status[TIMER_T3402] == TIMER_ACTIVE) ||
         (mm_timer_status[TIMER_T3411] == TIMER_ACTIVE))
      {
        if(emm_search_tai_list(&emm_ctrl_data_ptr->last_attempted_tai,
                               emm_ctrl_data_ptr->emm_tai_list_ptr) == TRUE)
        {
          /* T3402/T3411 was started on a TA which belongs to TA list. The
             current serving TA is also in the TA list. Do not start TAU
             until T3402/T3411 expiry. */
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE);
        }
        else
        {
          /* T3402/T3411 was started on a TA which does not belong to TA list.
             The current serving TA is different since it belongs to the TA list.
             Start TAU right away. */
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - Srving TA in TAL w/ T3402/T3411 running");
          emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;
          start_tau = TRUE;
        }
      }
      else
      {  
        /* EU was updated to not updated by GW for RAU failure. Start TAU. */
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Initing TAU - EU STATUS %d",
                      emm_ctrl_data_ptr->emm_update_status);
        
        if((emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_INVALID)||
           (emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_PERIODIC))
        {
            emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
        }
        start_tau = TRUE;
      }      
    }
    else if((emm_ctrl_data_ptr->ps_resumption_status == EMM_PS_RESUMPTION_PENDING)&&
            (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE) &&
            (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE))
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - GW PS session suspended");
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      start_tau = TRUE;
    }
    else if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI)
    {
      MSG_HIGH_DS_2(MM_SUB, "=EMM= Initing TAU - T3423 timer status %d, TIN type %d",
                    mm_timer_status[TIMER_T3423],
                    emm_ctrl_data_ptr->TIN);
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      start_tau = TRUE;
    }
#ifdef FEATURE_FEMTO_CSG    
    else if ((mm_is_csg_in_white_list(current_tai.plmn, &csg_info) == FALSE) &&
            (emm_is_manual_csg_selection_success(emm_ctrl_data_ptr) == TRUE) &&
            (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE) &&
            (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE))
    {
      MSG_HIGH_DS_2(MM_SUB, "=EMM= Initing TAU - Manual CSG SELECTION success",
                    mm_timer_status[TIMER_T3423],
                    emm_ctrl_data_ptr->TIN);
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      start_tau = TRUE;
    }
#endif 
    else if(((emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC)||            
             (emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_DUE_TO_SRVCC)) &&
             (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE) &&
             (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE))
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= Initing TAU - SGs association to be established cause %d",
                    emm_ctrl_data_ptr->sg_association_to_be_established);
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      start_tau = TRUE;
    }
    else if( (*emm_ctrl_data_ptr->set_drx_req_pending_ptr == TRUE)&&
        (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE) &&
        (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE)) 
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= SET_DRX_REQ pending, Start TAU procedure ");
      start_tau = TRUE;
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
    }
    else if((emm_ctrl_data_ptr->radio_cap_update != EMM_RADIO_CAP_UPDATE_NONE) &&
            (emm_ctrl_data_ptr->tau_cause != TAU_CAUSE_LOAD_BALANCING) &&
            (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE) &&
            (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE))
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Initiate TAU - Update radio capabilities pending");
      start_tau = TRUE;
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;
    }
    else if((emm_lte_cs_domain_param_match(
                      emm_ctrl_data_ptr->reg_req_pending_ptr->lte_cs_domain_param,
                      emm_ctrl_data_ptr->reg_lte_cs_domain_param) == FALSE) &&
            (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE) &&
            (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE))
    {
       MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - change in lte_cs_domain_param");
       emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
       start_tau = TRUE;
    }
    else if(EMM_GET_SUBSTATE() == EMM_REGISTERED_UPDATE_NEEDED)
    {  
      if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_LOW_LAYER_FAILURE)
      {  
        if(ds3gpp_per_subs_is_ps_data_available((sys_modem_as_id_e_type)mm_sub_id) == FALSE)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - Low layer failure and no data pending");
          start_tau = TRUE;
        }
        else if(emm_ctrl_data_ptr->eps_bearer_context_status_updated == TRUE)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - BC updated in REGED_NO_CELL_AVAILABLE");
          emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
          start_tau = TRUE;
        }
        else
        {
          emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
        }
      }
      else if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_TA_CHANGE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= TA change to orig cell");
        emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
        emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;
      }
      else if(emm_ctrl_data_ptr->tau_cause != TAU_CAUSE_INVALID)
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Initing TAU %d", emm_ctrl_data_ptr->tau_cause);
        start_tau = TRUE;
      }
    }
    else if((EMM_GET_STATE() == EMM_REGISTERED) &&
               ((EMM_GET_SUBSTATE() == EMM_REGISTERED_NO_CELL_AVAILABLE) ||
               (EMM_GET_SUBSTATE() == EMM_REGISTERED_PLMN_SEARCH) ||
               (EMM_GET_SUBSTATE() == EMM_REGISTERED_NORMAL_SERVICE)))
    {  
      /* 5.5.3.2.2 f - Start TAU when EPS bearer context is updated while in 
         NO_CELL_AVAILABLE substate and UE returns to NORMAL_SERVICE substate  */
      if(emm_ctrl_data_ptr->eps_bearer_context_status_updated == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - BC updated in REGED_NO_CELL_AVAILABLE");
        emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
        start_tau = TRUE;
      }
      else if((emm_ctrl_data_ptr->tau_cause != TAU_CAUSE_INVALID) &&
              (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE) &&
              (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE))
      {  
        if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_LOW_LAYER_FAILURE)
        {  
          if(ds3gpp_per_subs_is_ps_data_available((sys_modem_as_id_e_type)mm_sub_id) == FALSE)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - Delayed lower layer failure");
            start_tau = TRUE;
          }
          else
          {
            emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
          }
        }
        else if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_TA_CHANGE)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= TA change to orig cell");
          emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
          emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;
        }
        else
        {
          MSG_HIGH_DS_2(MM_SUB, "=EMM= Initing TAU %d - REGISTERED %d substate",
                        emm_ctrl_data_ptr->tau_cause,
                        EMM_GET_SUBSTATE());
          start_tau = TRUE;
        }
      }
    }
  }
  else
  {
    /* Current TAI is NOT part of the current TAI list */
    MSG_HIGH_DS_3(MM_SUB, "=EMM= TA not in TAI list - US %d, T3402 %d, T3411 %d",
                  emm_ctrl_data_ptr->emm_update_status,
                  mm_timer_status[TIMER_T3402],
                  mm_timer_status[TIMER_T3411]);
 
    if(emm_ctrl_data_ptr->reg_complete_ack_pending != EMM_PENDING_REG_ACK_NONE)
    {
      /* Do not start TAU here. Let ATTACH/TAU COMPLETE NAK to trigger reattach
         or TA change TAU. */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= ATTACH/TAU COMPLETE ACK pending, restart proc on NAK");
      return;
    }

    if((emm_ctrl_data_ptr->emm_update_status != EMM_STATUS_UPDATED)
       &&
       ((mm_timer_status[TIMER_T3402] == TIMER_ACTIVE) ||
        (mm_timer_status[TIMER_T3411] == TIMER_ACTIVE))
       &&
       (sys_plmn_match(emm_ctrl_data_ptr->last_attempted_tai.plmn, 
                       current_tai.plmn) == TRUE) 
       &&
       (emm_ctrl_data_ptr->last_attempted_tai.tac == current_tai.tac))
    {
      /* EU is not updated. Current serving TA is the same TA on which T3402 was
         started. Do not start TAU untill T3402 expiry. */
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE);
    }
    else
    {
      /* For all other cases, serving TA is not in TA list, start TAU. */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - Srving TA not in TAL");
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;
      if((sys_plmn_match(emm_ctrl_data_ptr->last_attempted_tai.plmn, 
                       current_tai.plmn) == TRUE)&&
         (emm_ctrl_data_ptr->last_attempted_tai.tac == current_tai.tac)&&
         (emm_ctrl_data_ptr->emm_tau_attempt_counter != 0))
      {
        update_type = EMM_UPDATE_RECOVERY;
      }
      start_tau = TRUE;
    }

    /* Is this a good way to determine if timer is running? Can there be a race condition? when just before doing this 
    what happens if timer expires*/
    if(mm_timer_status[ TIMER_T3423 ] == TIMER_ACTIVE )
    {
      mm_stop_timer(TIMER_T3423);
    }
  }

#ifdef FEATURE_ENHANCED_NW_SELECTION
  if((start_tau != TRUE) && 
     (EMM_GET_SUBSTATE() != EMM_REGISTERED_ATTEMPTING_TO_UPDATE) &&
     (mm_timer_status[TIMER_T3402] != TIMER_ACTIVE) &&
     (mm_timer_status[TIMER_T3411] != TIMER_ACTIVE) &&
     (emm_ctrl_data_ptr->reg_req_pending_ptr->type == MMR_REG_REQ_EF_RAT_REFRESH) &&
     (emm_ctrl_data_ptr->reg_req_pended_indication == TRUE) &&
     (emm_ctrl_data_ptr->refresh_pending == TRUE) && 
     (reg_sim_read_ens_flag() == TRUE))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Initing TAU - EF_RAT_REFRESH");
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
    start_tau = TRUE;
  }
  MSG_HIGH_DS_0(MM_SUB, "=EMM= reset refresh_pending flag");
  emm_ctrl_data_ptr->refresh_pending = FALSE;
#endif 

  if(start_tau == TRUE)
  {
    emm_ctrl_data_ptr->restart_mt_service_req = FALSE;
    if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_PERIODIC)
    {
      emm_initiate_tau_procedure(update_type,
                                 PERIODIC_UPDATING,
                                 FALSE, 
                                 emm_ctrl_data_ptr);
    }
    else
    {
      emm_initiate_tau_procedure(update_type,
                                 emm_get_eps_update_type(emm_ctrl_data_ptr),
                                 FALSE, 
                                 emm_ctrl_data_ptr);
    }
  }
  else
  {  
    if(EMM_GET_SUBSTATE() != EMM_REGISTERED_ATTEMPTING_TO_UPDATE)
    {
      if(sys_eplmn_list_validate(TRUE) == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= EPLMN list validated - TAU not needed");
        emm_send_rrc_system_update(emm_ctrl_data_ptr);
        /* Update G RR and W RRC */
        mm_send_rrc_eq_plmn_list_change_ind();
        mm_send_rr_eq_plmn_list_change_ind(); 
      }

      /* EMM moves into NORMAL SERVICE substate */
      emm_enforce_normal_service(&current_tai, emm_ctrl_data_ptr);
	  /* Set manual flag false for the case same registered TAI for Manual*/
	  emm_set_mm_manual_selection_flag(FALSE);
      emm_ctrl_data_ptr->manual_user_sel = FALSE;

      if(((mm_timer_status[TIMER_T3411] == TIMER_ACTIVE) &&
          (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == 
                                                       SYS_SRV_DOMAIN_CS_PS) &&
          (emm_is_combined_reg_allowed(emm_ctrl_data_ptr) == TRUE) &&
          (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_AVAILABLE) &&
          (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                       SYS_SRV_DOMAIN_PS_ONLY))
         ||
         (mm_timer_status[TIMER_T3402] == TIMER_ACTIVE))
      {
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM);
      }
      if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED)
      {
         if(emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS)
         {
            emm_send_esr_if_buffered(emm_ctrl_data_ptr);
         }
         else if((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain != SYS_SRV_DOMAIN_CS_PS))
         {
            emm_process_buffered_csfb_hard_failure();
         }
      }
      else if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED)
      {
         emm_send_esr_if_buffered(emm_ctrl_data_ptr);
      }
      if(emm_ctrl_data_ptr->restart_mt_service_req == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= starting pending MT page response - TAU not needed");
        /* Start service request procedure with the cause of MT_ACCESS. */
        emm_start_nas_service_request_procedure(emm_ctrl_data_ptr,LTE_RRC_EST_CAUSE_MT_ACCESS);
        emm_ctrl_data_ptr->restart_mt_service_req = FALSE;
      }
    }
    else if(mm_timer_status[TIMER_T3402] == TIMER_ACTIVE )
    {
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
      reg_service_report(MMR_CAUSE_FAILURE_MAX_ATTEMPTED,
                         FALSE,
                         emm_ctrl_data_ptr);

    }
  }

} /* end of emm_registered_process_possible_tau_rrc_service_ind() */

/*===========================================================================

FUNCTION   EMM_REGISTERED_LIMITED_PROCESS_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received in REGISTERED LIMITED 
  SERVICE state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_registered_limited_process_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type  tai;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  tai.tac = rrc_service_ind->camped_svc_info.tac;

  emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;

  switch(emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode)
  {
    case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
      if(emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE)            
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
      if(emm_is_manual_reg_allowed(tai, emm_ctrl_data_ptr) == TRUE)
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
#ifdef FEATURE_LTE_REL9
    case SYS_NETWORK_SELECTION_MODE_LIMITED:
      if((emm_bit_mask_check(emm_ctrl_data_ptr->reg_req_pending_ptr->additional_info,
                                       (byte)SYS_ADDTNL_INFO_EMERG_CALL) == TRUE) &&
         (emm_is_plmn_reg_allowed(rrc_service_ind,emm_ctrl_data_ptr) == TRUE))
      {
        emm_ctrl_data_ptr->plmn_service_state->extend_srv_info = 
                                         SYS_EXTEND_SRV_INFO_FULL_SRV_POSSIBLE;
      }
      emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS,     
                                        rrc_service_ind->camped_svc_info, FALSE);             
      break;
#endif
    default:
      break;
  }

  reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

} /* emm_registered_limited_process_rrc_service_ind */

/*===========================================================================

FUNCTION    EMM_REGISTERED_PROCESS_RRC_SERVICE_IND

DESCRIPTION
  This function checks whether lte_rrc_service_ind_s primitive requires to 
  start TAU when EMM in REGISTERED state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May stop timers:
    o
===========================================================================*/
void emm_registered_process_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{


  ASSERT(rrc_service_ind != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL); 


  switch (EMM_GET_SUBSTATE())
  {
    case EMM_REGISTERED_UPDATE_NEEDED:
    case EMM_REGISTERED_PLMN_SEARCH:
    case EMM_REGISTERED_NO_CELL_AVAILABLE:
    case EMM_REGISTERED_NORMAL_SERVICE:
    case EMM_REGISTERED_ATTEMPTING_TO_UPDATE:
    case EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM:
    case EMM_REGISTERED_IMSI_DETACH_INITIATED:
      /* Check whether TAU is no longer required, if it is start TAU procedure */
      emm_registered_process_possible_tau_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
      break ; 
    case EMM_REGISTERED_LIMITED_SERVICE:
      emm_registered_limited_process_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr);
      break ;
    case EMM_REGISTERED_WAITING_FOR_ESM_ISR_STATUS:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Rcved SRV IND in WAITING_FOR_ESM_ISR_STATUS state");
      /* Do nothing for now */
      break;
    default:
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM REGISTERED substate",0,0,0) ;
      break ;
  }
} /* end of emm_registered_process_rrc_service_ind() */

/*===========================================================================

FUNCTION    EMM_TAU_COLLISION_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received during TAU procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_tau_collision_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type  current_tai;
  boolean                start_tau = FALSE;
  emm_failure_type               emm_failure_cause;
  boolean policy_change_pend = emm_policy_change_pending(rrc_service_ind);

  ASSERT(rrc_service_ind != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  current_tai = emm_get_current_tai(rrc_service_ind);

  if(emm_search_tai_list(&current_tai, emm_ctrl_data_ptr->emm_tai_list_ptr)
       == TRUE)
  {
    if(emm_search_tai_list(&emm_ctrl_data_ptr->last_attempted_tai,
                           emm_ctrl_data_ptr->emm_tai_list_ptr) == FALSE)
    {
      /* Current TA is in TA list. The TA that TAU was started on is not in TA
         list. TAU need to be restarted. */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TA (In TAL) changed from TAU started on");
      start_tau = TRUE;
    }
  }
  else
  {
    if((sys_plmn_match(emm_ctrl_data_ptr->last_attempted_tai.plmn, 
                       current_tai.plmn) == FALSE) 
       ||
       (emm_ctrl_data_ptr->last_attempted_tai.tac != current_tai.tac))
    {
      /* Current TA is not in TA list and is different frm the TA that TAU was
         started on. TAU need to be restarted. */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= TA (Not in TAL) changed from TAU started on");
     start_tau = TRUE;
    }
  }
  
  if((rrc_service_ind->camped_svc_info.cell_access_status == 
                                       LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY) || 
      (policy_change_pend == TRUE))
  {
    mm_stop_timer(TIMER_T3430);
    emm_remove_release_client(emm_tau_request_release_indication);
    EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_PLMN_SEARCH);

    emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
    /* Process any pending messages */
    emm_process_unsent_messages(CONN_CNF_LTE_RRC_CONN_EST_FAILURE, 
                                emm_failure_cause,
                                emm_ctrl_data_ptr);      
    /* Process all pending irat uplink messages */
    emm_process_pending_irat_msg(LTE_NAS_IRAT_INVALID_STATE,
                                 LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                 LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                                 emm_ctrl_data_ptr);
    if(start_tau == TRUE)
    {       
      start_tau = FALSE;
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED ;
      emm_save_mobility_info(emm_ctrl_data_ptr);
    }

    if(policy_change_pend == FALSE)
    {
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
    }
  }

  if(start_tau == TRUE)
  {
    /* Delete pended TAU message if any before starting TAU to avoid processing 
        any buffered TAU Request if it was not ACKed */
    emm_delete_pended_message_with_msg_id(TRACKING_AREA_UPADTE_REQUEST,emm_ctrl_data_ptr);
	
	
    /* TAU is being restarted due to TA change. Set EPS update status to EU2 
       NOT UPDATED. (24.301 5.5.3.2.6 e) */
    emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED ;
    emm_save_mobility_info(emm_ctrl_data_ptr);
    emm_cancel_authentication();

    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;

    emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                               emm_get_eps_update_type(emm_ctrl_data_ptr),
                               TRUE, 
                               emm_ctrl_data_ptr);
  }
} /* end of emm_tau_collision_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_SERVICE_REQUEST_COLLISION_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received during SERVICE REQUEST
  procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_service_request_collision_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type current_tai ;

  ASSERT(rrc_service_ind != (lte_rrc_service_ind_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  /* Create NAS current TAI */
  current_tai = emm_get_current_tai(rrc_service_ind);

  if(emm_policy_change_pending(rrc_service_ind))
  {
    EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_LIMITED_SERVICE) ;
    return;
  }
  /* 
    Check whether current RRC reported TAI is belonging to the 
    current TAI list
  */
  if (emm_search_tai_list(&current_tai,emm_ctrl_data_ptr->emm_tai_list_ptr))
  {
    /* Current TAI IS part of the current TAI list */

    MSG_HIGH_DS_0(MM_SUB, "=EMM= TAI is part of the existing TAI list, no TAU is required");

    MSG_HIGH_DS_0(MM_SUB, "=EMM= Keep running Service Request procedure");
  }
  else
  {
    /* Current TAI is NOT part of the current TAI list */

    /* Start TAU procedure */
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;
    emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                               emm_get_eps_update_type(emm_ctrl_data_ptr),
                               FALSE, 
                               emm_ctrl_data_ptr);
  }
} /* end of emm_service_request_collision_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_DETACH_COLLISION_RRC_SERVICE_IND

DESCRIPTION
  This function processes RRC SERVICE IND received during DETACH procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_detach_collision_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type current_tai ;

  ASSERT(rrc_service_ind != (lte_rrc_service_ind_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ;
#ifdef FEATURE_DUAL_SIM
  ASSERT(emm_ctrl_data_ptr->dds_switch_pended_ptr != NULL); 
#endif

  /* Create NAS current TAI */
  current_tai = emm_get_current_tai(rrc_service_ind);

  if(emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY ||
     emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS)
  {
    /* 
      Check whether current RRC reported TAI is belonging to the 
      current TAI list
    */
    if (emm_search_tai_list(&current_tai,emm_ctrl_data_ptr->emm_tai_list_ptr))
    {
      /* Current TAI IS part of the current TAI list */

      MSG_HIGH_DS_0(MM_SUB, "=EMM= TAI is part of the existing TAI list, no TAU is required, Keep running DETACH Procedure");

      //emm_misc_actions_on_service_indication(rrc_service_ind,emm_ctrl_data_ptr);
    }
    else
    {

      /* EMM PS detach procedure will be aborted on TAI Change
         in case PS Detach is triggered due to DDS switch completion
      */

#ifdef FEATURE_DUAL_SIM
      if (*emm_ctrl_data_ptr->dds_switch_pended_ptr)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Perform local detach due to high priority user action");
        emm_complete_mo_detach(emm_ctrl_data_ptr);    

      }
      else
#endif
      {
        /* Current TAI is NOT part of the current TAI list */
        /* Start TAU procedure */
        emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;
        emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                                   emm_get_eps_update_type(emm_ctrl_data_ptr),
                                   FALSE, 
                                   emm_ctrl_data_ptr);
      }
    }
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= not starting tau. service domain is %d",
                  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain);
  }
} /* end of emm_detach_collision_rrc_service_ind() */

/*===========================================================================

FUNCTION  EMM_PROCESS_DIV_DUPLEX_CHG

DESCRIPTION
  This function performs local detach and reattach (if UE is camped) upon 
  division duplex change

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_div_duplex_chg
(
  emm_ctrl_data_type     *emm_ctrl_data_ptr,
  lte_rrc_service_ind_s  *rrc_srv_ind_ptr
)
{
  emm_state_type  emm_state = EMM_INVALID_STATE;
  emm_state_type  emm_substate = EMM_INVALID_STATE;
  emm_failure_type        emm_failure_cause;
  boolean policy_change_pending = FALSE; 
  ASSERT(emm_ctrl_data_ptr != NULL); 
  ASSERT(rrc_srv_ind_ptr != NULL); 

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  emm_state = EMM_GET_STATE();
  emm_substate = EMM_GET_SUBSTATE();

  MSG_HIGH_DS_3(MM_SUB, "=EMM= DIV DUPLEX change - RRC active %d, Srv status %d, State %d",
                emm_ctrl_data_ptr->rrc_active,
                emm_ctrl_data_ptr->plmn_service_state->service_status,
                emm_state);

  if(rrc_srv_ind_ptr->svc_status != LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Unexpected div duplex chg while UE is not camped");
    return;
  }

  policy_change_pending = emm_policy_change_pending(rrc_srv_ind_ptr);

  /* Update service status if REG REQ is pending */
  if((emm_ctrl_data_ptr->reg_req_pended_indication == TRUE) &&
     (emm_ctrl_data_ptr->reg_init_rrc_trans_id == rrc_srv_ind_ptr->trans_id))
  {
    emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;
    emm_send_reg_camped_ind(SYS_RAT_LTE_RADIO_ACCESS, 
                            rrc_srv_ind_ptr->camped_svc_info, FALSE);
  }

  /* Abort ongoing spedific procedures and stop related timers */
  switch(emm_state)
  {
    case EMM_REGISTERED_INITIATED:
      mm_stop_timer(TIMER_T3410);
      emm_remove_release_client(emm_attach_release_indication);
      if (emm_substate == (emm_state_type)EMM_WAITING_FOR_NW_RESPONSE)
      {  
        emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);
      }
      emm_clean_msg_buffer(emm_ctrl_data_ptr);
      break;  
    case EMM_REGISTERED:
      mm_stop_timer(TIMER_T3411);
      mm_stop_timer(TIMER_T3402);
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
      mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 

      break;
    case EMM_SERVICE_REQUEST_INITIATED:
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      if(emm_state == EMM_SERVICE_REQUEST_INITIATED)
      {
        /* 5.6.1.6 Abnormal case g in UE. Abort service request and start detach
           Keep the connection and notify ESM if necessary */
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                  TRUE, LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                  emm_failure_cause);
      }
      else
      {
        mm_stop_timer(TIMER_T3430);
        emm_remove_release_client(emm_tau_request_release_indication);
      }
      /* Process any pending messages */
      emm_process_unsent_messages(CONN_CNF_LTE_RRC_CONN_EST_FAILURE, 
                                  emm_failure_cause,
                                  emm_ctrl_data_ptr);      
      /* Process all pending irat uplink messages */
      emm_process_pending_irat_msg(LTE_NAS_IRAT_INVALID_STATE,
                                   LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                   LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                                   emm_ctrl_data_ptr);
      break;
    case EMM_DEREGISTERED_INITIATED:
      if(emm_ctrl_data_ptr->detach_reason == SWITCH_OFF)
      {
        /* Let switch off detach proceed. Do nothing here. */
        return;
      }
      else
      {
        /* Abort non switch off detach and do a local detach */
        emm_abort_mo_detach(emm_ctrl_data_ptr, TRUE);
        emm_clean_msg_buffer(emm_ctrl_data_ptr);
      }
      break;
    default:
      return;
  }

  if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) || 
     (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED))
  {
     emm_process_buffered_csfb_hard_failure();
  }

  /* Local detach */
  emm_build_and_send_detach_ind(EMM_DETACHED);

  emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_NO_SRV;
  emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;


  if(emm_state == EMM_DEREGISTERED_INITIATED)
  {
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);    
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr); 
  }
  else
  {
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED);
    reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN, FALSE, emm_ctrl_data_ptr); 
    if(emm_ctrl_data_ptr->reg_req_pended_indication == FALSE)
    { 
      emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
      if(policy_change_pending == FALSE)
      {
        emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);      
      }
    }  
  }

} /* emm_process_div_duplex_chgdiv_duplex_chg */

#ifdef FEATURE_LTE_REL10
/*===========================================================================
FUNCTION    EMM_CHK_BACKOFF_CONGESTION_ALLOWED

DESCRIPTION
  checks if registraion is allowed on plmn due to congestion while t3346
  is running. puts emm in appropriate state due to new serv ind

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if reg is allowed on plmn
  FALSE - if reg is not allowed on plmn

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_chk_backoff_congestion_allowed
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  emm_state_type emm_state   = EMM_INVALID_STATE;
  boolean        reg_allowed = TRUE;
  emm_state = EMM_GET_STATE();

#ifdef FEATURE_LTE_REL11 
   if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
      (emm_state == EMM_REGISTERED) &&
      (EMM_GET_SUBSTATE() == EMM_REGISTERED_NORMAL_SERVICE)&&
      (EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE))
   {
     MSG_HIGH_DS_0(MM_SUB, "=EMM= In Registered-Normal service and Connected state");
     return reg_allowed;
   }
  /* If UE is higher access allowed UE or if the an emergency 
  ** PDN exists then TAU is allowed(T3346 should not be considered)
  */
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
     ((emm_check_high_priority(emm_ctrl_data_ptr) == TRUE) ||
      (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_EXIST)))
  {
    return reg_allowed;
  }
#endif

  /* check T3346 and eplmn list for plmn registration */
  if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
     (emm_plmn_equivalent_t3346(rrc_service_ind,
                                emm_ctrl_data_ptr) == TRUE))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= registration not allowed on plmn T3346 %d",
                  mm_timer_status[TIMER_T3346]);

    /* need to do registration upon t3346 timer expiry */
    if(emm_chk_ta_change(rrc_service_ind, emm_ctrl_data_ptr) == TRUE)    
    {
      emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
      emm_save_mobility_info(emm_ctrl_data_ptr);
    }

    /* move to approptiate based update status */
    switch(emm_state)
    {
      case EMM_DEREGISTERED:
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH);
        emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                             SYS_SRV_STATUS_LIMITED_REGIONAL;
        break;

     case EMM_REGISTERED:
        if(emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED)
        {
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                          SYS_SRV_STATUS_SRV;
        } 
        else
        {
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_ATTEMPTING_TO_UPDATE);
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                             SYS_SRV_STATUS_LIMITED_REGIONAL;
        }
        break;

      default:
         MSG_ERROR_DS_1(MM_SUB, "=EMM= rrc_service_ind unhandled state %d", emm_state);
         break;
    }  

    reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN, FALSE, emm_ctrl_data_ptr);
    
    if(emm_ctrl_data_ptr->reg_init_rrc_trans_id == rrc_service_ind->trans_id)
    {
      /* Post RAT change event */
      mmsend_rat_change_event();
    }

    reg_allowed = FALSE;
  }
  

  if(!reg_allowed &&
     ((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) || 
      (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED)))
  {
     emm_process_buffered_csfb_hard_failure();
   }

  return reg_allowed;
}

#endif /* end FEATURE_LTE_REL10*/

/*===========================================================================
FUNCTION    EMM_RRC_PROCESS_SERVICE_INDICATION

DESCRIPTION
  This function processes RRC_SERVICE_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_rrc_process_service_indication
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  emm_state_type        emm_state       = EMM_INVALID_STATE ;
  sys_plmn_id_s_type    rrc_reported_plmn;
  sys_plmn_id_s_type    last_saved_plmn;
  lte_rrc_plmn_s        plmn;
  boolean               is_plmn_match = FALSE;
  lte_nas_local_cause_enum_T    local_cause = AS_REJ_LRRC_LOWER_LAYER_FAILURE;
  emm_failure_type      emm_failure_cause;

  ASSERT(rrc_service_ind != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL);

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  plmn = rrc_service_ind->camped_svc_info.selected_plmn;

  MSG_HIGH_DS_6(MM_SUB, "=EMM= RRC_SERVICE_IND - MCC: D1 %u, D2 %u, D3 %u MNC: D1 %u, D2 %u, D3 %u",
                plmn.mcc[0],
                plmn.mcc[1],
                plmn.mcc[2],
                plmn.mnc[0],
                plmn.mnc[1],
                plmn.mnc[2]);

  MSG_HIGH_DS_5(MM_SUB, "=EMM= RRC_SERVICE_IND - TAC %u, Cell ID %u, IMS EMC spt %d Div Duplex %d, Detach Reqd %d",
                rrc_service_ind->camped_svc_info.tac,
                rrc_service_ind->camped_svc_info.cell_identity,
                rrc_service_ind->camped_svc_info.ims_emergency_support_enabled,
                rrc_service_ind->div_duplex,
                rrc_service_ind->detach_reqd);

#ifdef  FEATURE_FEMTO_CSG
  MSG_HIGH_DS_2(MM_SUB, "=EMM= RRC_SERVICE_IND - CSG Id = %d,  Hybrid cell = %d",
                rrc_service_ind->camped_svc_info.csg_info.csg_id,
                rrc_service_ind->camped_svc_info.csg_info.hybrid_cell);
#endif 
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  MSG_HIGH_DS_1(MM_SUB, "=EMM= RRC_SERVICE_IND - ACQ Status = %d",
                rrc_service_ind->acq_status);
#endif 
  /* Reset the MM band related variables */
  mm_band_indicator = SYS_BAND_CLASS_NONE;
#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
  mm_active_band = SYS_BAND_MASK_EMPTY;
#endif

  if((emm_ctrl_data_ptr->rrc_active == FALSE) && 
     (emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == FALSE) &&
     (emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == FALSE))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= RRC is activated");
    emm_ctrl_data_ptr->rrc_active = TRUE;
  }

  emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = FALSE;
  if((EMM_GET_CONNECTION_STATE() == EMM_RELEASING_RRC_CONNECTION_STATE) &&
     (emm_ctrl_data_ptr->aborting_conn == FALSE))
  {
    EMM_MOVES_TO_IDLE_STATE() ;

    /* reset the secure exchange messages */
    emm_ctrl_data_ptr->nas_secure_exchange = FALSE;         
  }

  rrc_reported_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                               rrc_service_ind->camped_svc_info.selected_plmn);
  last_saved_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

  is_plmn_match = sys_plmn_match(last_saved_plmn, rrc_reported_plmn);

  if(rrc_service_ind->no_svc_cause == SYS_NO_SVC_CAUSE_RLF)
  {
    memscpy(&emm_ctrl_data_ptr->rlf_plmn, sizeof(sys_plmn_id_s_type),
           &emm_ctrl_data_ptr->plmn_service_state->plmn, sizeof(sys_plmn_id_s_type));
  }
  /* The tuple of plmn_identity, cell_identity identifies a cell uniquely. If 
     either of plmn_identity and cell_identity is different, that means that 
     UE has reselected to a new cell. UE needs to stop barring timers when cell
     is changed. */
  if((is_plmn_match == FALSE) ||
     (emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.cell_identity !=
       rrc_service_ind->camped_svc_info.cell_identity))
  {
      emm_stop_cell_barring_timers(emm_ctrl_data_ptr
#ifdef FEATURE_NAS_EAB
                                   ,TRUE
#endif      
                                  );
  }

  /* At this point, EMM can be only in IDLE or CONNECTED state 
     Only process pending REG_REQ and pending message in IDLE state 
     Note: If this service ind comes when a RLF is recovered, the connection
     state will be CONNECTED and RRC will send DATA_CNF for the pending 
     message before the RLF. */
  if((EMM_GET_CONNECTION_STATE() == EMM_IDLE_STATE) &&
     (emm_reg_container != NULL))
  {
    if(emm_ctrl_data_ptr->client_list != NULL)
    {
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      emm_process_unsent_messages(CONN_REL_LTE_RRC_CONN_REL_NORMAL,
                                  emm_failure_cause, 
                                  emm_ctrl_data_ptr);
      /* Process all pending irat uplink messages */
      emm_process_pending_irat_msg(LTE_NAS_IRAT_INVALID_STATE,
                                   LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                   LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                                   emm_ctrl_data_ptr);
    }
    if(emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == FALSE)
    {
      emm_reg_container(emm_ctrl_data_ptr);
    }
  }

  MSG_HIGH_DS_3(MM_SUB, "=EMM= SRV REQ pending %d, SRV REQ 0x%x, RRC SRV IND 0x%x",
                emm_ctrl_data_ptr->reg_req_pended_indication,
                emm_ctrl_data_ptr->reg_init_rrc_trans_id,
                rrc_service_ind->trans_id);

  emm_state = EMM_GET_STATE();

  if(emm_ctrl_data_ptr->tau_pending_gw_attach == TRUE &&
	 rrc_service_ind->detach_reqd == TRUE)
  {
     rrc_service_ind->detach_reqd = FALSE;
  }

  if((emm_ctrl_data_ptr->reg_req_pended_indication == TRUE) &&
     (emm_ctrl_data_ptr->reg_init_rrc_trans_id != rrc_service_ind->trans_id))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= SRV IND is not rsp for pending SRV REQ");
    if((rrc_service_ind->detach_reqd == TRUE) && 
    (emm_state != EMM_DEREGISTERED)) 
    {
           /*To delay process div duplex and also reset pending div duplex if it happened again since UE is back to the same mode now */
          emm_ctrl_data_ptr->emm_lte_mode_change_pending = (emm_ctrl_data_ptr->emm_lte_mode_change_pending == FALSE) ? TRUE : FALSE ;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Div Duplex Chg handling pending - %d",
                        emm_ctrl_data_ptr->emm_lte_mode_change_pending);
    }
    return;
  }
  
  if((emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_REQ_PENDING)||
     ((emm_ctrl_data_ptr->ps_detach_info.status== EMM_PS_DETACH_WAITING_FOR_REG_REQ) &&
      (emm_ctrl_data_ptr->reg_req_pended_indication == TRUE)))
  {
    emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;

    /* save the cell ID, cell frequency and global cell ID for cell barring */
    emm_ctrl_data_ptr->phy_cell_id = 
                                  rrc_service_ind->camped_svc_info.phy_cell_id;
    emm_ctrl_data_ptr->freq = rrc_service_ind->camped_svc_info.earfcn;
    emm_ctrl_data_ptr->cell_identity = 
                                rrc_service_ind->camped_svc_info.cell_identity;

    if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
    {
      emm_send_plmn_change_ind(rrc_reported_plmn);
    }

    memscpy(emm_ctrl_data_ptr->last_rrc_service_ind_ptr, sizeof(lte_rrc_service_ind_s),
            rrc_service_ind, sizeof(lte_rrc_service_ind_s));

    if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_NOT_AVAILABLE)
    {
      if(rrc_service_ind->no_svc_cause != SYS_NO_SVC_CAUSE_RLF)
      {
        memset((void *)emm_ctrl_data_ptr->plmn_service_state->plmn.identity,
               0xFF, sizeof(sys_plmn_id_s_type));
        emm_ctrl_data_ptr->plmn_service_state->tac = 0xFFFE;
      }
       
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                         SYS_SRV_STATUS_NO_SRV;
#ifdef FEATURE_FEMTO_CSG
      emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id = SYS_CSG_ID_INVALID;
#endif 
    }
    else
    {
      emm_ctrl_data_ptr->plmn_service_state->plmn = 
       emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

      emm_ctrl_data_ptr->plmn_service_state->tac = 
                   emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;
				   
      SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band);
      SYS_LTE_BAND_MASK_ADD_BAND(&emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band,
                                 emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_band);

      /* Set the MSCR to 1 in case of LTE operation */
      mm_system_information.mscr = 1;  
#ifdef FEATURE_FEMTO_CSG
       emm_ctrl_data_ptr->plmn_service_state->csg_info = rrc_service_ind->camped_svc_info.csg_info;
#endif 
    }

#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
    emm_ctrl_data_ptr->plmn_service_state->acq_status = rrc_service_ind->acq_status;
#endif 
    emm_set_tau_trm_priority(FALSE);
    if(emm_ctrl_data_ptr->suitable_search_end_pending == TRUE)
    {
      emm_send_reg_t311_timer_expiry_ind();
      emm_ctrl_data_ptr->suitable_search_end_pending = FALSE;
    }
    if(emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_WAITING_FOR_REG_REQ)
    {
      emm_ctrl_data_ptr->ps_detach_info.status = EMM_PS_DETACH_NONE;
    }
    else
    {
      emm_process_ps_detach_req(emm_ctrl_data_ptr->ps_detach_info.ps_detach_req, 
                              emm_ctrl_data_ptr);
    }

    return;
  }

  /* rrc_active can be FALSE only when RRC is being deactivated */ 
  if(emm_ctrl_data_ptr->rrc_active == FALSE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= RRC is being deactivated - Do not further process");
    emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;
    reg_service_report = (emm_reg_service_report_type)emm_send_reg_ind;
    return;
  }

#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  emm_ctrl_data_ptr->plmn_service_state->acq_status = rrc_service_ind->acq_status;
#endif 
  if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    /* Set t311_rem_time = 0 & no_svc_cause = SYS_NO_SVC_CAUSE_NORMAL
       to prevent Service request to be sent in connected state during
       w2l handover*/
    emm_ctrl_data_ptr->last_rrc_service_ind_ptr->t311_rem_time = 0;
    emm_ctrl_data_ptr->last_rrc_service_ind_ptr->no_svc_cause = 
                                                SYS_NO_SVC_CAUSE_NORMAL;
    
/*Set lu initiated flag to TRUE if Attach Rejected in GW with congestion and moved to LTE such that in case of going back to GW, LU should be performed
As per 24.008: 4.4.1
The normal location updating procedure shall also be started when the MS, configured to use CS fallback and SMS over SGs, 
or SMS over SGs only, enters a GERAN or UTRAN cell after intersystem change from S1 mode to Iu or A/Gb mode, if timer T3346 is running, 
and the location area of the current cell is the same as the stored location area.*/
#ifdef FEATURE_NAS_REL10
#ifdef FEATURE_LTE
    if(mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
    {
      mm_is_lu_to_be_initiated_t3346_ltogw = TRUE;
    }
#endif
#endif
#ifdef FEATURE_NAS_REL11
/*Reset flag when moved from LTE to G to LTE*/
      gmm_is_lw_to_g_interrat_in_progress = FALSE;
#endif
    /* This function sends rat change request to SM and 
       resets the loss on service global variables of LTE and GW */

    /* If GW exists then update MM/GMM */
/*    if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                       SYS_RAT_UMTS_RADIO_ACCESS) || 
       mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                       SYS_RAT_GSM_RADIO_ACCESS)  || 
       mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                       SYS_RAT_TDS_RADIO_ACCESS))*/
    {
      emm_service_acquired_on_lte(emm_ctrl_data_ptr);
    }
#ifdef FEATURE_WRLF_SYSTEM_SEL
    if((mm_wcdma_rlf_state == WCDMA_RLF_STARTED)&&
       (emm_registration_allowance_chk(rrc_service_ind,emm_ctrl_data_ptr) == TRUE))
    {
      if(pmm_mode == PMM_CONNECTED)
      {
        if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
        {
          emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);
        }

        MSG_HIGH_DS_0(MM_SUB, "=EMM= TAU to be performed - PMM CONNECTED MODE");
        emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      }
      MSG_HIGH_DS_0(MM_SUB, "=EMM= send RRC ABORT REQ TO WCDMA TO COME OUT OF RLF STATE");
      mm_send_rrc_abort_req_during_wrlf();
    }
#endif

    emm_ctrl_data_ptr->suitable_search_end_pending = FALSE;
  }

  emm_ctrl_data_ptr->ignore_GW_activation_ind = FALSE;

  /* save the cell ID ,cell frequency and global cell ID for cell barring */
  emm_ctrl_data_ptr->phy_cell_id = rrc_service_ind->camped_svc_info.phy_cell_id;
  emm_ctrl_data_ptr->freq = rrc_service_ind->camped_svc_info.earfcn;
  emm_ctrl_data_ptr->cell_identity = 
                                rrc_service_ind->camped_svc_info.cell_identity;

  memscpy(emm_ctrl_data_ptr->last_rrc_service_ind_ptr, sizeof(lte_rrc_service_ind_s),
            rrc_service_ind, sizeof(lte_rrc_service_ind_s));

  emm_ctrl_data_ptr->plmn_service_state->sib8_available = 
                                               rrc_service_ind->sib8_available;

  emm_state = EMM_GET_STATE();

  /* RRC reports that LTE service IS avaliable */
  if(rrc_service_ind->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    emm_ctrl_data_ptr->plmn_service_state->plmn  =
      emm_convert_rrc_plmn_id_to_nas_plmn_id(
        emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

    emm_ctrl_data_ptr->plmn_service_state->tac = 
                   emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac;
				   
    SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band);
    SYS_LTE_BAND_MASK_ADD_BAND(&emm_ctrl_data_ptr->plmn_service_state->active_band.lte_active_band,
                               emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_band);

#ifdef FEATURE_FEMTO_CSG
     emm_ctrl_data_ptr->plmn_service_state->csg_info = rrc_service_ind->camped_svc_info.csg_info;
#endif 

    emm_send_plmn_change_ind(rrc_reported_plmn);

    /* Set the MSCR to 1 in case of LTE operation */
    mm_system_information.mscr = 1;

    if(((rrc_service_ind->detach_reqd == TRUE)|| 
    (emm_ctrl_data_ptr->emm_lte_mode_change_pending == TRUE)) && 
    (emm_state != EMM_DEREGISTERED)) 
    {
     if(emm_registration_allowance_chk(rrc_service_ind,emm_ctrl_data_ptr) == FALSE)
      {
        if(rrc_service_ind->detach_reqd == TRUE)
        {
          /* reset pending div duplex if it happened again since UE is back to the same mode now */
          emm_ctrl_data_ptr->emm_lte_mode_change_pending = (emm_ctrl_data_ptr->emm_lte_mode_change_pending == FALSE) ? TRUE : FALSE ;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Pending DETACH/ATTACH due to div_duplex_chg, emm_lte_mode_change_pending set to %d\n",
                        emm_ctrl_data_ptr->emm_lte_mode_change_pending);
        }
      }
     else
      {
        /* process div duplex if div duplex happened only once - but not twice in which case there is no mode change effectively */
        if(((emm_ctrl_data_ptr->emm_lte_mode_change_pending == FALSE) && (rrc_service_ind->detach_reqd == TRUE)) ||
        ((emm_ctrl_data_ptr->emm_lte_mode_change_pending == TRUE) && (rrc_service_ind->detach_reqd == FALSE)))
        {
          emm_set_tau_trm_priority(FALSE);
          emm_process_div_duplex_chg(emm_ctrl_data_ptr, rrc_service_ind);
          return;
        }
        /* If there is no mode change effectively, reset the flag, as we do not need to ATTACH / DETACH */
        emm_ctrl_data_ptr->emm_lte_mode_change_pending = FALSE;
      }
    }

#ifdef FEATURE_LTE_REL10
    if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) 
    {
      /* if registration is not allowe wait for t3346 timer expiry */
      if(emm_chk_backoff_congestion_allowed(rrc_service_ind, 
                                            emm_ctrl_data_ptr) == FALSE)
      {
        emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;
        emm_set_tau_trm_priority(FALSE);
        return;
      }
    }
#endif
    if((EMM_GET_CONNECTION_STATE() == EMM_IDLE_STATE) &&
       (emm_ctrl_data_ptr->client_list != NULL))
    {
      emm_autonomous_connection_setup(emm_ctrl_data_ptr) ;
    }

    if((emm_ctrl_data_ptr->plmn_service_state->sim_state != 
                                                    SYS_SIM_STATE_AVAILABLE) &&
       (emm_ctrl_data_ptr->plmn_service_state->sim_state != 
                                                   SYS_SIM_STATE_CS_INVALID) &&
       (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode != 
                                          SYS_NETWORK_SELECTION_MODE_LIMITED)
#ifdef FEATURE_LTE_REL9
        &&(emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)&&
           !(((emm_state == EMM_DEREGISTERED)||(emm_state == EMM_REGISTERED_INITIATED))&&
            (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING))
#endif
        )
    {
      MSG_HIGH_DS_3(MM_SUB, "=EMM= Invalid SIM_STATE %d, EMM state %d, NW_SEL_MODE %d",
                    emm_ctrl_data_ptr->plmn_service_state->sim_state,
                    emm_state,
                    emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode);
      if(emm_state == EMM_DEREGISTERED)
      {        
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH) ;        
      }
      else
      {
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_PLMN_SEARCH) ;      
      } 
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                  SYS_SRV_STATUS_LIMITED ;
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
      return;  
    }

    switch(emm_state)
    {
      case EMM_DEREGISTERED:      
        emm_deregistered_process_rrc_service_ind(rrc_service_ind,
                                                 emm_ctrl_data_ptr);
        break ;
      case EMM_REGISTERED_INITIATED:
        emm_attach_collision_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      case EMM_REGISTERED:
        if(emm_ctrl_data_ptr->TIN != NAS_MM_TIN_RAT_RELATED_TMSI)
        {
          emm_registered_process_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        }
        else if(EMM_GET_SUBSTATE() != EMM_REGISTERED_WAITING_FOR_ESM_ISR_STATUS)
        {
          emm_build_and_send_get_isr_status_ind();
          EMM_REGISTERED_CHANGE_SUB_STATE(EMM_REGISTERED_WAITING_FOR_ESM_ISR_STATUS);
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Received service indication in waiting for esm isr status substate");
        }
        break ;
      case EMM_TRACKING_AREA_UPDATING_INITIATED:
        emm_tau_collision_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      case EMM_SERVICE_REQUEST_INITIATED:
        emm_service_request_collision_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      case EMM_DEREGISTERED_INITIATED:
        emm_detach_collision_rrc_service_ind(rrc_service_ind,emm_ctrl_data_ptr) ;
        break ;
      default:
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM state",0,0,0) ;
        break ;
    }
    if(emm_ctrl_data_ptr->reg_init_rrc_trans_id == rrc_service_ind->trans_id)
    {
      mmsend_rat_change_event();//Post RAT change event
#ifdef FEATURE_ENHANCED_NW_SELECTION
      mm_inform_updated_foreground_search_list();
#endif
    }
  }
  else /* RRC reports that LTE service is NOT avaliable */
  {
    switch(emm_state)
    {
      case EMM_DEREGISTERED:
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
        break;
      case EMM_REGISTERED_INITIATED:
        mm_stop_timer(TIMER_T3410);
        emm_delete_pended_message_with_msg_id(ATTACH_REQUEST,emm_ctrl_data_ptr);
        MSG_LOW_DS_0(MM_SUB, "Deleted Attach Request from pended message queue\n");
#ifdef FEATURE_LTE_REL9 
        if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING)
        {
          emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);
        }
#endif
        if(EMM_GET_SUBSTATE() == EMM_WAITING_FOR_ESM_RESPONSE)
        {
          emm_build_and_send_detach_ind(EMM_DETACHED);
        }
        emm_send_diag_event(EVENT_NAS_ATTACH, (nas_event_end_result)PROC_END,
			    (nas_event_end_reason)PROC_END_LOWER_LAYER_FAILURE, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
        emm_remove_release_client(emm_attach_release_indication);
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
        break;
      case EMM_DEREGISTERED_INITIATED:
        if(emm_ctrl_data_ptr->detach_reason == NORMAL_DETACH)
        {
          if(emm_ctrl_data_ptr->ps_detach_info.status == 
                                                     EMM_PS_DETACH_CNF_PENDING)
          {
            emm_abort_mo_detach(emm_ctrl_data_ptr, FALSE);
            /* If PS DETACH pending, do clean up later.*/
            EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
          }
          else
          {          
            if(emm_ctrl_data_ptr->ongoing_detach_type == IMSI_DETACH_MO_DETACH)
            {
              emm_abort_mo_detach(emm_ctrl_data_ptr, FALSE);
              EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
            }
            else if(EMM_GET_CONNECTION_STATE() != EMM_CONNECTED_STATE)
            { 
              emm_abort_mo_detach(emm_ctrl_data_ptr, FALSE);         
              EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
            }
          }
        }
        else if((mm_timer_status[EMM_POWEROFF_DETACH_TIMER] == TIMER_ACTIVE)&&
          (emm_ctrl_data_ptr->detach_reason == SWITCH_OFF))
        {
          emm_remove_release_client(emm_poweroff_detach_release_ind);
          mm_stop_timer(EMM_POWEROFF_DETACH_TIMER); 
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
          emm_reset_poweroff_detach_params(emm_ctrl_data_ptr);
          emm_init_rrc_deact_req(emm_ctrl_data_ptr->rrc_deact_req.deact_reason, 
                         emm_ctrl_data_ptr);
        }
        else
        {
          emm_abort_mo_detach(emm_ctrl_data_ptr, FALSE);
        }
        break ;
      case EMM_REGISTERED:
        mm_stop_timer(TIMER_DELAY_TAU);
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
        break;
      case EMM_TRACKING_AREA_UPDATING_INITIATED:
        mm_stop_timer(TIMER_T3430);
        emm_delete_pended_message_with_msg_id(TRACKING_AREA_UPADTE_REQUEST,emm_ctrl_data_ptr);
        MSG_LOW_DS_0(MM_SUB, "Deleted TRACKING_AREA_UPADTE_REQUEST from pended message queue\n");
        emm_remove_release_client(emm_tau_request_release_indication);
        emm_send_diag_event(EVENT_NAS_TAU, (nas_event_end_result)PROC_END,
					(nas_event_end_reason)PROC_END_LOWER_LAYER_FAILURE, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);
        if(( emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_TA_CHANGE )
           && (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED))
        {
          emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
          MSG_HIGH_DS_2(MM_SUB, "=EMM= Change EMM status to %d as TAU cause is %d",
                        EMM_STATUS_NOT_UPDATED, emm_ctrl_data_ptr->tau_cause);
        }
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
        if(emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED)
        {
          emm_csfb_failure_cause_type failure_cause_type;
          failure_cause_type.failure_type = AS_REJECT_CAUSE;
          failure_cause_type.failure_cause.lte_nas_failure_cause = LTE_NAS_LRRC_CONN_EST_FAILURE_LINK_FAILURE;//skip tau later
          emm_process_extended_service_req_failure(failure_cause_type,emm_ctrl_data_ptr);
        }
        break;      
      case EMM_SERVICE_REQUEST_INITIATED:
        /* EMM will be moved to REGISTERED_NO_CELL_AVAILABLE substate in
           emm_abort_service_request */
         emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
        //MNK:CSFB_FAILURE
        if(emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE)
        {
            //emm_convert_rrc_failure_to_local_cause(rrc_service_ind->no_svc_cause);
          if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB) &&
             (rrc_service_ind->no_svc_cause == SYS_NO_SVC_CAUSE_RLF))
          {
            local_cause = LTE_NAS_REJ_LRRC_RADIO_LINK_FAILURE;  
          }
#ifdef FEATURE_DUAL_SIM
          if(((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB) ||
              (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED)) &&
             (rrc_service_ind->acq_status == SYS_ACQ_STATUS_NO_RESOURCE))
          {
            local_cause = LTE_NAS_REJ_NO_SERVICE_TRM_UNAVAILABLE;
          }
#endif
          emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                    TRUE, local_cause,
                                    emm_failure_cause);
        }
        else
        {
          emm_abort_service_request(emm_ctrl_data_ptr, FALSE, 
                                    TRUE, LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                    emm_failure_cause);

        }
        break ;
      default:
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM state",0,0,0) ;
        break ;
    }

    if(EMM_GET_CONNECTION_STATE() == EMM_IDLE_STATE) 
    {
      emm_failure_cause.cause_type = LTE_NAS_IRAT_LOWER_LAYER_FAILURE;
      /* Process all unsent messages in buffer */
      emm_process_unsent_messages(CONN_CNF_LTE_RRC_CONN_EST_FAILURE,
                                  emm_failure_cause, 
                                  emm_ctrl_data_ptr);
      /* Process all pending irat uplink messages */
      emm_process_pending_irat_msg(LTE_NAS_IRAT_LOWER_LAYER_FAILURE,
                                   LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                   LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                                   emm_ctrl_data_ptr);
    }

    /* Indicate the Upper layers that Service is no longer avaliable */
    emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                         SYS_SRV_STATUS_NO_SRV;

#ifdef FEATURE_FEMTO_CSG
    emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id = SYS_CSG_ID_INVALID;
#endif 

    if((emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode ==
                        SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY) ||
       (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode ==
                        SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY))
    {
      emm_ctrl_data_ptr->plmn_service_state->plmn_reg_type = 
                                            SYS_REG_DOMAIN_NO_SERVICE;
    }

    if(rrc_service_ind->no_svc_cause != SYS_NO_SVC_CAUSE_RLF)
    {
      memset((void *)emm_ctrl_data_ptr->plmn_service_state->plmn.identity, 
             0xFF,
             sizeof(sys_plmn_id_s_type));
      emm_ctrl_data_ptr->plmn_service_state->tac = 0xFFFE;
    }

    if(emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_CNF_PENDING)
    {
      emm_ps_detach_clean_up(emm_ctrl_data_ptr);
    }
    /* If registration ack and PLMN blocking is pending then block the PLMN
       emm_process_block_plmn_req will not trigger service ind/cnf as service status is NO service */
    if(emm_ctrl_data_ptr->reg_complete_ack_pending != EMM_PENDING_REG_ACK_NONE)
    {
      if(emm_ctrl_data_ptr->is_plmn_block_req_pending == TRUE)
      {
        emm_process_block_plmn_req(&emm_ctrl_data_ptr->pending_block_plmn_req, emm_ctrl_data_ptr);
      }
    }

    /* Inform REG regarding current level of Service */
    reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
    emm_send_reg_attach_failed_ind();
    if(emm_ctrl_data_ptr->suitable_search_end_pending == TRUE)
    {
      emm_send_reg_t311_timer_expiry_ind();
      emm_ctrl_data_ptr->suitable_search_end_pending = FALSE;
    }
  }
  emm_state = EMM_GET_STATE();
#ifdef FEATURE_DUAL_SIM
  /*
  ** Do not reset TAU TRM priority if
  **  a) if EMM state is TAU initiated or
  **  b) if EMM state is REGISTERED and TAU complete is pending or 
  **  c) if EMM state is REGISTERED and EMM is waiting for ISR status response from ESM.
  */
  if((emm_ctrl_data_ptr->tau_trm_priority == TRUE) &&
     !((emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED) ||
       ((emm_state == EMM_REGISTERED) &&
        ((emm_ctrl_data_ptr->reg_complete_ack_pending == EMM_PENDING_REG_ACK_TAU_COMPLETE) ||
         (EMM_GET_SUBSTATE() == EMM_REGISTERED_WAITING_FOR_ESM_ISR_STATUS)))))
  {
    emm_set_tau_trm_priority(FALSE);
  }
#endif
  emm_ctrl_data_ptr->reg_req_pended_indication = FALSE ;
} /* end of emm_rrc_process_service_indication() */


/*===========================================================================
FUNCTION    EMM_RRC_PROCESS_BARRING_UPDATE_INDICATION

DESCRIPTION
  This function processes LTE_RRC_BARRING_UPDATE_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_rrc_process_barring_update_indication
(
  lte_rrc_barring_update_ind_s *rrc_barring_update_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  ASSERT(rrc_barring_update_ind != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 
  MSG_HIGH_DS_2(MM_SUB, "=EMM= LTE_RRC_BARRING_UPDATE_IND - emergency_access_barred %u, skip_acb_for_volte_call %u",
                rrc_barring_update_ind->emergency_access_barred,
                rrc_barring_update_ind->skip_acb_for_volte_call);
 MSG_HIGH_DS_4(MM_SUB, "=EMM= LTE_RRC_BARRING_UPDATE_IND - ac_Barring_r12_present %d,ac barring skip for voice %d,ac barring skip for video %d,ac barring for SMS %d ",
                         rrc_barring_update_ind->ac_barring_info.ac_Barring_r12_present,
                         rrc_barring_update_ind->ac_barring_info.ac_BarringSkipForMMTELVoice_r12,
                         rrc_barring_update_ind->ac_barring_info.ac_BarringSkipForMMTELVideo_r12,
                         rrc_barring_update_ind->ac_barring_info.ac_BarringSkipForSMS_r12);

  emm_ctrl_data_ptr->sib2_ac_barring_info.ac_Barring_r12_present = rrc_barring_update_ind->ac_barring_info.ac_Barring_r12_present;

  if(rrc_barring_update_ind->ac_barring_info.ac_Barring_r12_present)/*use sib2 info for skipping ACB*/
  {
    emm_ctrl_data_ptr->sib2_ac_barring_info.skip_ac_barring_for_volte_call = rrc_barring_update_ind->ac_barring_info.ac_BarringSkipForMMTELVoice_r12;
    emm_ctrl_data_ptr->sib2_ac_barring_info.skip_ac_barring_for_video_call = rrc_barring_update_ind->ac_barring_info.ac_BarringSkipForMMTELVideo_r12;
    emm_ctrl_data_ptr->sib2_ac_barring_info.skip_ac_barring_for_for_sms = rrc_barring_update_ind->ac_barring_info.ac_BarringSkipForSMS_r12;
  }
  else/*falls back to legacy -NV*/
  {
    emm_ctrl_data_ptr->skip_acb_for_volte_call = rrc_barring_update_ind->skip_acb_for_volte_call;
  }
#ifdef FEATURE_LTE_REL9
  emm_send_mmr_lte_cell_info_ind(rrc_barring_update_ind->emergency_access_barred);
#endif
  return;
}

void emm_rrc_process_pseudo_resel_low_priority_indication
(
  lte_rrc_pseudo_resel_low_priority_ind_s *rrc_pseudo_resel_low_priority_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  ASSERT(rrc_pseudo_resel_low_priority_ind != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL); 
  MSG_HIGH_DS_1(MM_SUB, "=EMM= LTE_RRC_PESUDO_RESEL_LOW_PRIORITY_IND - earfcn %u",
                rrc_pseudo_resel_low_priority_ind->earfcn);
  emm_send_mmr_lte_pseudo_resel_low_priority_ind(rrc_pseudo_resel_low_priority_ind->earfcn);
}


/*===========================================================================
FUNCTION    EMM_ESTIMATE_DL_COUNT

DESCRIPTION
  This function derives a 32 bit DL count from the given 4 bits of DL NAS count from MME.
  This derivation is explained in Section 4.8 of Multimode Inter-RAT Security FDD 
  (Doc# 80-N1321-1 A)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_estimate_dl_count
(
  lte_rrc_nas_umts_key_ind_s *rrc_nas_umts_key_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr,
  byte *nas_dl_count,
  size_t nas_dl_count_buf_len
)
{
  emm_security_context_info *context_ptr = NULL;
  byte  out_count[4]; 
  uint8 y1, y2, y_hash;
  word over_flow_cnt = 0;
  word under_flow_cnt = 0;

  ASSERT(rrc_nas_umts_key_ind != (lte_rrc_nas_umts_key_ind_s *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  
  memset(out_count,0, sizeof(byte)* 4);

  /* get the current context */
  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  if( context_ptr != NULL )
  {
    /* Ignore the spare bits in MSB*/
    rrc_nas_umts_key_ind->dl_nas_count &= 0xF;

    MSG_HIGH_DS_4(MM_SUB, "=EMM= [Current UE DL NAS Count] context_ptr->nas_dl_cnt[0] = %d context_ptr->nas_dl_cnt[1] = %d context_ptr->nas_dl_cnt[2] = %d context_ptr->nas_dl_cnt[3] = %d",
                           context_ptr->nas_dl_cnt[0],context_ptr->nas_dl_cnt[1],context_ptr->nas_dl_cnt[2],context_ptr->nas_dl_cnt[3]);

    y1 = ((context_ptr->nas_dl_cnt[0] & 0xF0) >> 4);
    y2 = context_ptr->nas_dl_cnt[0] & 0x0F;
    y_hash = (y2 - 8) & 0xF;

    MSG_HIGH_DS_4(MM_SUB, "=EMM= y1 = %d, y2 = %d, y_hash = %d [DEBUG RX] rrc_nas_umts_key_ind->dl_nas_count %d ", y1, y2, y_hash,rrc_nas_umts_key_ind->dl_nas_count);

    if(y2 > y_hash) /* y_hash does not wrap around */
    {
      if(((y2 <= rrc_nas_umts_key_ind->dl_nas_count) && (rrc_nas_umts_key_ind->dl_nas_count <= 15)) ||
         ((y_hash < rrc_nas_umts_key_ind->dl_nas_count) && (rrc_nas_umts_key_ind->dl_nas_count < y2)))
      {
        out_count[0] = (y1 << 4) | rrc_nas_umts_key_ind->dl_nas_count;
        out_count[1] = context_ptr->nas_dl_cnt[1];
        out_count[2] = context_ptr->nas_dl_cnt[2];
        out_count[3] = 0;
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Debug out_count[0] = %d", out_count[0]);
      }
      else if(rrc_nas_umts_key_ind->dl_nas_count <= y_hash)
      {
        if (y1 >= 15) /* y1+1 will overflow */
        {
          over_flow_cnt = ( (word )context_ptr->nas_dl_cnt[2] << 8) | context_ptr->nas_dl_cnt[1];
          over_flow_cnt++ ;

          MSG_HIGH_DS_0(MM_SUB, "=EMM= overflow detected, Increment overflow count");
          out_count[0] = ((y1-15) << 4) | rrc_nas_umts_key_ind->dl_nas_count;
          /* Increment Overflow count */
          out_count[1] = (byte )(over_flow_cnt & 0xFF);
          out_count[2] = (byte )((over_flow_cnt & 0xFF00) >> 8);
          out_count[3] = 0;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Debug out_count[0] = %d", out_count[0]);
        }
        else /* y1+1 in not overflow */
        {
          out_count[0] = ((y1+1) << 4) | rrc_nas_umts_key_ind->dl_nas_count;
          out_count[1] = context_ptr->nas_dl_cnt[1];
          out_count[2] = context_ptr->nas_dl_cnt[2];
          out_count[3] = 0;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Debug out_count[0] = %d", out_count[0]);
         }
      }
    }
    else /* y_hash does wrap around */
    {
      if(((y2 <= rrc_nas_umts_key_ind->dl_nas_count) && (rrc_nas_umts_key_ind->dl_nas_count <= y_hash)) ||
         (rrc_nas_umts_key_ind->dl_nas_count < y2))
      {
        out_count[0] = (y1 << 4) | rrc_nas_umts_key_ind->dl_nas_count;
        out_count[1] = context_ptr->nas_dl_cnt[1];
        out_count[2] = context_ptr->nas_dl_cnt[2];
        out_count[3] = 0;
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Debug out_count[0] = %d", out_count[0]);
      }
      else if((y_hash < rrc_nas_umts_key_ind->dl_nas_count) && (rrc_nas_umts_key_ind->dl_nas_count <= 15))
      {
        if (y1 == 0) /* y1-1 will underflow */
        {
          under_flow_cnt = ( (word )context_ptr->nas_dl_cnt[2] << 8) | context_ptr->nas_dl_cnt[1];
          under_flow_cnt--;
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Underflow detected, Decrement underflow count");
          out_count[0] = ((y1+15) << 4) | rrc_nas_umts_key_ind->dl_nas_count;
          /* Increment Overflow count */
          out_count[1] = (byte )(under_flow_cnt & 0xFF);
          out_count[2] = (byte )((under_flow_cnt & 0xFF00) >> 8);
          out_count[3] = 0;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Debug out_count[0] = %d", out_count[0]);
        }
        else /* y1-1 will not underflow */
        {
          out_count[0] = ((y1-1) << 4) | rrc_nas_umts_key_ind->dl_nas_count;
          out_count[1] = context_ptr->nas_dl_cnt[1];
          out_count[2] = context_ptr->nas_dl_cnt[2];
          out_count[3] = 0;
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Debug out_count[0] = %d", out_count[0]);
        }
      }
    }
    context_ptr->nas_dl_cnt[0] = out_count[0];
    context_ptr->nas_dl_cnt[1] = out_count[1];
    context_ptr->nas_dl_cnt[2] = out_count[2];
    context_ptr->nas_dl_cnt[3] = out_count[3];
  }

  memscpy((void*)nas_dl_count,
           nas_dl_count_buf_len,
           (void*)&out_count[0],
           sizeof(out_count));

  MSG_HIGH_DS_6(MM_SUB, "=EMM= [emm_estimate_dl_coun] out_count[0] = %d out_count[1] = %d out_count[2] out_count[3] = %d over_flow_cnt = %d, under_flow_cnt = %d", 
                        out_count[0],
                        out_count[1],
                        out_count[2],
                        out_count[3],
                        over_flow_cnt,
                        under_flow_cnt);
  
} /* end of emm_estimate_dl_count() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================
FUNCTION    EMM_PROCESS_EMM_MESSAGE

DESCRIPTION
  This function processes the received EMM messages from NW if there is no 
  protocol state error. Otherwise, this function ignores the message and sends
  EMM STATUS message to NW. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_emm_message
(
  lte_nas_emm_incoming_msg_type     *incloming_message,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
)
{
  emm_state_type  emm_state = EMM_INVALID_STATE;
  boolean         protocol_state_error = FALSE;

  ASSERT(incloming_message != (lte_nas_emm_incoming_msg_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  emm_state = EMM_GET_STATE();

  if((emm_state == EMM_NULL) || (emm_state == EMM_INVALID_STATE))
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Ignoring emm message in EMM NULL/INVALID state");
    return;
  }
  /* Adding DL data ind debugging here */
  emm_set_mm_msg_info_for_debug_buffer((byte)incloming_message->hdr.msg_id);
  emm_add_message_to_debug_buffer(LTE_RRC_DL_DATA_IND,TO_EMM,(sys_modem_as_id_e_type)mm_as_id);

  switch (incloming_message->hdr.msg_id)
  {
    case ATTACH_ACCEPT:
      if(emm_state == EMM_REGISTERED_INITIATED)
      {
        if(EMM_GET_SUBSTATE() == EMM_WAITING_FOR_NW_RESPONSE)
        {
          emm_process_attach_accept(&incloming_message->attach_accept, 
                                     emm_ctrl_data_ptr);        
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Ignoring dup ATTACH ACCEPT");
        }
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break ;
    case ATTACH_REJECT:
      if(emm_state == EMM_REGISTERED_INITIATED)
      {
        emm_process_attach_reject(&incloming_message->attach_rej, 
                                  emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break ;
     case DETACH_REQUEST:
#ifdef FEATURE_LTE_REL11
        /*Spec 24.301 5.2.2.4 abnormal case d)*/
        if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
           (emm_state == EMM_DEREGISTERED_INITIATED) &&
           (emm_ctrl_data_ptr->detach_reason == SWITCH_OFF) &&
           (nas_emm_power_off_detach_type  != INVALID_DETACH_TYPE))
       {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Detach Request discarded while switch off type MO detach ongoing");
       }
       else
#endif
      if(emm_state != EMM_DEREGISTERED)
      {
        emm_start_mt_detach(&incloming_message->mt_detach_req,
                            emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break;
    case DETACH_ACCEPT:
      if((emm_state == EMM_DEREGISTERED_INITIATED) ||
         ((emm_state == EMM_REGISTERED) && 
          (EMM_GET_SUBSTATE() == (emm_substate_type)EMM_REGISTERED_IMSI_DETACH_INITIATED)))
      {
        emm_process_detach_accept(&incloming_message->mt_detach_accept, 
                                  emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break;
    case TRACKING_AREA_UPDATE_ACCEPT:
      if(emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED)
      {
        /* Received TAU accept message. Process the accept message */
        emm_process_tau_accept(&incloming_message->tau_accept,
                               emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break ;
    case TRACKING_AREA_REJECT:
      if(emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED)
      {
        /* Received TAU reject message. Process the reject message */
        emm_process_tau_reject(&incloming_message->tau_rej, 
                               emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break ;
    case SERVICE_REJECT:
      if(emm_state == EMM_SERVICE_REQUEST_INITIATED)
      {
        /* Received service reject message. Process the reject message */
        emm_process_reject_message(incloming_message, emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break;  
    case SECURITY_MODE_COMMAND:
        emm_process_security_mode_cmd(&incloming_message->security_mode_cmd, 
                                      emm_ctrl_data_ptr);
      break;
    case EMM_AUTHENTICATION_REQUEST:
        emm_process_auth_request(&incloming_message->auth_req, 
                                 emm_ctrl_data_ptr); 
      break;
    case EMM_AUTHENTICATION_REJECT:
        emm_process_authentication_reject(emm_ctrl_data_ptr);
      break;
    case EMM_IDENTITY_REQUEST:
        emm_process_identity_request(&incloming_message->identity_req, 
                                     emm_ctrl_data_ptr);
      break;
    case GUTI_REALLOCATION_COMMAND:
        emm_process_guti_realloc_cmd(&incloming_message->guti_realloc_cmd, 
                                     emm_ctrl_data_ptr); 
      break;
    case EMM_INFORMATION:
        emm_process_emm_info(&incloming_message->emm_info, 
                             emm_ctrl_data_ptr);
      break;
    case EMM_STATUS:
      /* No need to do anything */
      MSG_HIGH_DS_1(MM_SUB, "=EMM= Rcved EMM_STATUS w/ cause %u",
                    incloming_message->mt_emm_status.emm_cause);
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      break;
    case DOWNLINK_NAS_TRANSPORT:
      if((emm_state == EMM_REGISTERED || 
         (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED && (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED))) 
         && (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
      {
        emm_ctrl_data_ptr->emm_sms_active = TRUE;
#ifdef FEATURE_DUAL_SIM
        mm_check_other_mm_cnm_connections();
#endif  
        emm_process_dl_nas_transport(&incloming_message->dl_nas_transport);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break;
    case CS_SERV_NOTIFICATION:
      emm_process_cs_service_notification(&incloming_message->cs_serv_notification,emm_ctrl_data_ptr);
      break;
#if defined (FEATURE_LTE_REL9) 
    case DL_GENERIC_NAS_TRANSPORT:
     if((emm_state == EMM_REGISTERED)&& 
     ((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY)||
      (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == SYS_SRV_DOMAIN_CS_PS)))
      {
        emm_process_dl_generic_nas_transport_msg(&incloming_message->dl_generic_nas_transport_msg,emm_ctrl_data_ptr);
      }
      else
      {
        protocol_state_error = TRUE;
      }
      break;
#endif
    default:
      /* Should never happen - msg type check is done at caller function */
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM Message ID - %d", 
                (byte)incloming_message->hdr.msg_id, 0,0) ;
      break;
  } /* switch (incloming_message->message_id) */

  if(protocol_state_error == TRUE)
  {
    MSG_HIGH_DS_2(MM_SUB, "=EMM= Protocol state error - Rcved msg %u in EMM state %u",
                  incloming_message->hdr.msg_id,
                  emm_state);
    emm_send_emm_status(emm_ctrl_data_ptr,
                        LTE_NAS_MSG_TYPE_INCOMPATIBLE_WITH_PROTOCOL_STATE);
  }

} /* end of emm_process_emm_message() */


/*===========================================================================
FUNCTION    EMM_FREE_DSM_MEMORY_IN_RRC_DATA_IND

DESCRIPTION
  This function frees the DSM memory in the received RRC data indication when
  EMM is not in EMM_CONNECTED_STATE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_free_dsm_memory_in_rrc_data_ind
(
  lte_rrc_dl_data_ind_s *rrc_data_ind
)
{
  uint8  num_attach = 0;
  uint32 dsm_packet_length = 0;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */

  ASSERT(rrc_data_ind != (lte_rrc_dl_data_ind_s *)NULL) ; 

  num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)rrc_data_ind);

  if ( num_attach == 1 )
  {
    att_ptr = msgr_get_attach((msgr_hdr_struct_type *)rrc_data_ind, 0);
    msgr_get_dsm_attach(att_ptr, &dsm_ptr);
    ASSERT(dsm_ptr != NULL);

    dsm_packet_length               = dsm_length_packet(dsm_ptr);
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Freeing DSM memory! item len %d", dsm_packet_length);
    
    /* free the dsm memory */
    dsm_free_packet(&dsm_ptr);
    ASSERT(dsm_ptr == NULL);
  }
  else
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= DSM DL not found. RRC using old interface",0,0,0);
  }
}

#ifdef TEST_FRAMEWORK
 #error code not present
#endif // TEST_FRAMEWORK

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
FUNCTION    EMM_PROCESS_RRC_DATA_INDICATION

DESCRIPTION
  This function processes RRC data indication. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -save -e550 Symbol 'sec_params' not accessed */
void emm_process_rrc_data_indication
(
  lte_rrc_dl_data_ind_s *rrc_data_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  // jalokia lte_nas_incoming_msg_type incloming_message_ptr ;
  lte_nas_incoming_msg_type *incloming_message_ptr = NULL;
  lte_nas_dl_security_info_type dl_security_info;
  byte message_counter = 0 ;
  /*lte_nas_protocol_discriminator protocol_descriminator = RESERVED ; */
  lte_nas_protocol_discriminator protocol_descriminator  ; 
  byte message_index = 0 ;
  boolean continue_processing = TRUE;
  boolean attach_accept_successful = TRUE;

  uint8  num_attach = 0;
  uint32 dsm_packet_length;
  dsm_item_type           *dsm_ptr = NULL;  /* DSM pointer */
  msgr_attach_struct_type *att_ptr;         /* Message attachment pointer */
   

  byte  sec_hdr;

  emm_security_context_info *context_ptr = NULL;
  byte  prev_dl_cnt[4]; 
  boolean is_smc_msg = FALSE;  

  ASSERT(rrc_data_ind != (lte_rrc_dl_data_ind_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->emm_ota_message_ptr != NULL) ; 

  ASSERT(decode_nas_msg_fp != NULL) ; 
  ASSERT(nas_retrieve_msg_fp != NULL) ;

  if(EMM_GET_CONNECTION_STATE() != EMM_CONNECTED_STATE)
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Ignoring RRC DL DATA IND rcved in conn state %d",
                   EMM_GET_CONNECTION_STATE());
    emm_free_dsm_memory_in_rrc_data_ind(rrc_data_ind);
    return;
  }

  memset(&dl_security_info,0,sizeof(lte_nas_dl_security_info_type));
  memset((void *)prev_dl_cnt, 0, sizeof(byte)*4); 
  memset(emm_ctrl_data_ptr->emm_ota_message_ptr, 0, EMM_MAX_MESSAGE_LENGTH);
  
    /* See if NAS has sent the DT message as a DSM item by checking for attachments */
  num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)rrc_data_ind);

  if ( num_attach == 1 )
  {
    att_ptr = msgr_get_attach((msgr_hdr_struct_type *)rrc_data_ind, 0);
    msgr_get_dsm_attach(att_ptr, &dsm_ptr);
    ASSERT(dsm_ptr != NULL);

    dsm_packet_length               = dsm_length_packet(dsm_ptr);
    MSG_HIGH_DS_1(MM_SUB, "=EMM= DSM DL Data found! item len %d", dsm_packet_length);
    
    if( dsm_packet_length <= EMM_MAX_MESSAGE_LENGTH )
    {
    
      /* store the RRC message payload, for SMC processing */
      emm_ctrl_data_ptr->ota_msg_size = (word)dsm_packet_length;

      emm_ctrl_data_ptr->ota_msg_size = dsm_pullup(&dsm_ptr, 
                                                   emm_ctrl_data_ptr->emm_ota_message_ptr, 
                                                   emm_ctrl_data_ptr->ota_msg_size);
    }
    else
    {
      emm_ctrl_data_ptr->ota_msg_size = EMM_MAX_MESSAGE_LENGTH;
      emm_ctrl_data_ptr->ota_msg_size = dsm_pullup(&dsm_ptr, 
                                                   emm_ctrl_data_ptr->emm_ota_message_ptr, 
                                                   emm_ctrl_data_ptr->ota_msg_size);

      MSG_ERROR_DS_3(MM_SUB, "=EMM= DSM item length %d greater than %d bytes, truncating to max read %d bytes",
                     dsm_packet_length,
                     EMM_MAX_MESSAGE_LENGTH,
                     emm_ctrl_data_ptr->ota_msg_size);

      /* free the additional memory greater than EMM_MAX_MSG_LEN */
      dsm_free_packet(&dsm_ptr);
    }
   
    /* Received dsm should have been freed */
    ASSERT(dsm_ptr == NULL);
  }
  else
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= DSM DL not found. RRC using old interface",0,0,0);
  }

  sec_hdr = (byte)((emm_ctrl_data_ptr->emm_ota_message_ptr[0] & 0xF0) >> 4);
  if(sec_hdr == INTEGRITY_PROTECT_SECURITY_MODE_CMD)
  { 
    is_smc_msg = TRUE;
  }

  dl_sqn_for_current_msg = 0;

  /* get the DL SQN from the OTA message */
  get_dl_nas_sqn((byte *)emm_ctrl_data_ptr->emm_ota_message_ptr,
                   &dl_sqn_for_current_msg);
           
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Rcved DL MSG w/ SQN %d", dl_sqn_for_current_msg);
  
  /* get the current context */
  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  if( context_ptr != NULL )
  {
    /* Save the current nas CL connt in case the rcved msg is invalid. */
    memscpy((void*)&prev_dl_cnt[0], 
            sizeof(prev_dl_cnt),
            (void*)&context_ptr->nas_dl_cnt[0], 
            sizeof(context_ptr->nas_dl_cnt)); 

    MSG_HIGH_DS_2(MM_SUB, "=EMM= Current context of type %d, KSI %d ",
                  context_ptr->context_type,
                  context_ptr->nasKSI.keyset_id);
  }

  message_counter = decode_nas_msg_fp((byte *)emm_ctrl_data_ptr->emm_ota_message_ptr, 
                                      (word)emm_ctrl_data_ptr->ota_msg_size,
                                      emm_get_dl_security_info(emm_ctrl_data_ptr, 
                                                               &dl_security_info,
                                                               is_smc_msg,
                                                               emm_ctrl_data_ptr->emm_ota_message_ptr),
                                                               emm_ctrl_data_ptr,
                                                               emm_ctrl_data_ptr->nas_incoming_msg_ptr);

#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */

  if(message_counter == 0)
  {
    if( context_ptr != NULL )
    {
       MSG_HIGH_DS_0(MM_SUB, "=EMM= DL MSG dropped, rolling back NAS DL CNT");
       memscpy((void*)&context_ptr->nas_dl_cnt[0],
               sizeof(context_ptr->nas_dl_cnt),
               (void*)&prev_dl_cnt[0],
               sizeof(prev_dl_cnt));
       emm_ctrl_data_ptr->last_incoming_sqn = prev_dl_cnt[0];
    }
    return;
  }

  incloming_message_ptr =(lte_nas_incoming_msg_type*) emm_modem_mem_alloc(
                         sizeof(lte_nas_incoming_msg_type),
                         emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void *) incloming_message_ptr);

  for (message_index=0; message_index<message_counter; message_index++)
  {
    protocol_descriminator = nas_retrieve_msg_fp(message_index, incloming_message_ptr) ;

    /* save the security context information */
    if( context_ptr != NULL )
    {
      /*
      MME may re-establish the secure exchange of NAS messages:
      - by replying with a NAS message that is integrity protected and ciphered 
        using the current EPS security context.
      */
      (void)get_nas_sec_info(&sec_hdr);
      if( sec_hdr == INTEGRITY_PROTECT_CIPHER ||
          sec_hdr == INTEGRITY_PROTECT )
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Rx INTEGRITY_PROTECT msg %d, set nas_secure_exchange to TRUE",
                      sec_hdr);
        emm_ctrl_data_ptr->nas_secure_exchange = TRUE;
      }
    }

    switch (protocol_descriminator)
    {
      case EPS_MOBILITY_MANAGEMENT_MESSAGES:
        /* Check for valid flag only for EMM messages*/
        if(incloming_message_ptr->nas_hdr.valid)
        {
          emm_process_emm_message(&incloming_message_ptr->emm_incoming_msg, emm_ctrl_data_ptr) ;
        }
        else
        {
          attach_accept_successful = FALSE;
          MSG_HIGH_DS_2(MM_SUB, "=EMM= Invalid EMM msg %u - Cause %u",
                        incloming_message_ptr->nas_hdr.msg_id,
                        incloming_message_ptr->nas_hdr.emm_decode_fail_reason);
          emm_send_emm_status(emm_ctrl_data_ptr, 
                              incloming_message_ptr->nas_hdr.emm_decode_fail_reason);
        }
        break ;
      case EPS_SESSION_MANAGEMENT_MESSAGES:
        if(EMM_GET_STATE() == EMM_REGISTERED_INITIATED)
        {
          /* Here the assumption is the MSG_LIB would always say there are 2 messages when processing ATTACH ACCEPT.
             So, in this case if attach accept came with any message other than ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST,
             EMM considers the attach accept as invalid and starts mo detach. 
          */
          if(incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id != ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST)
          {
            if((EMM_GET_SUBSTATE() != (emm_substate_type)EMM_WAITING_FOR_ESM_RESPONSE))
            {
              if(incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id != ESM_INFORMATION_REQUEST)
              {
                MSG_ERROR_DS_1(MM_SUB, "=EMM= ATTACH ACCEPT must carry ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST. Received msg id=%d,discarding this message",
                               incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id);
                continue_processing = FALSE;
#ifdef TEST_FRAMEWORK
                #error code not present
#endif /* TEST_FRAMEWORK */
              }            
              else if(emm_ctrl_data_ptr->nas_secure_exchange == FALSE)
              {
                MSG_ERROR_DS_0(MM_SUB, "=EMM= ESM_INFORMATION_REQUEST received before secure exchange of NAS messages is established, discarding this message");
                continue_processing = FALSE;
              }
            }
          }
          else
          {
            if(message_counter != 2)
            {
              MSG_ERROR_DS_0(MM_SUB, "=EMM= ESM message received without ATTACH ACCEPT, discarding this message\n");
              continue_processing = FALSE;
#ifdef TEST_FRAMEWORK
              #error code not present
#endif /* TEST_FRAMEWORK */
            }
            else
            {
              if((attach_accept_successful == TRUE) && 
                 (EMM_GET_STATE() == EMM_REGISTERED_INITIATED) &&
                 (EMM_GET_SUBSTATE() == (emm_substate_type)EMM_WAITING_FOR_NW_RESPONSE))
              {
                /* DO NOT Check for valid flag. Let ESM handle that*/
                emm_process_incoming_esm_message(&incloming_message_ptr->esm_incoming_msg, 
                                                 emm_ctrl_data_ptr) ;
#ifdef TEST_FRAMEWORK
                #error code not present
#endif /* TEST_FRAMEWORK */
                /* MOVING to EMM_WAITING_FOR_ESM_RESPONSE before sending the message to ESM*/
                EMM_MOVES_TO_REGISTERED_INITIATED_STATE((emm_substate_type)EMM_WAITING_FOR_ESM_RESPONSE) ;

                /* ESM will release all resources allocated for currently cached 
                   PDN_CONNECTIVITY_REQ upon receipt of ACTIVATE DEFAULT EPS 
                   BEARER CONTEXT REQUEST. 
                   Discard currently cached PDN_CONNECTIVITY_REQ since it is no 
                   longer reusable */
                emm_ctrl_data_ptr->pdn_conn_req_trans_id = 
                                               EMM_INVALID_PDN_CONN_REQ_TRANS_ID;
                continue_processing = FALSE;
              }
              else
              {
                MSG_ERROR_DS_0(MM_SUB, "=EMM= Dropping ACT DEF BC REQ - Invalid ATTACH ACCEPT");
                continue_processing = FALSE; 
              }
            }
          }
        }
        else if(EMM_GET_STATE() == EMM_DEREGISTERED)
        {
          /*For only Attach reject with #19 we delete the cached PDN..need to send 
            PDN Rej to ESM for other attach reject causes no need to process ESM mesage*/
          if(incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id != PDN_CONNECTIVITY_REJECT || 
             emm_ctrl_data_ptr->pdn_conn_req_trans_id != EMM_INVALID_PDN_CONN_REQ_TRANS_ID)
          {
            /*Block unnecessary ESM messages being sent to ESM*/
            MSG_HIGH_DS_1(MM_SUB, "=EMM= do not send message to ESM msg_id %d",
                          incloming_message_ptr->esm_incoming_msg.nas_hdr.msg_id);
            continue_processing = FALSE;  
#ifdef TEST_FRAMEWORK
            #error code not present
#endif /* TEST_FRAMEWORK */
          }
        }
        if((EMM_GET_STATE() != EMM_DEREGISTERED_INITIATED) &&
           (continue_processing == TRUE))
        {
          /* DO NOT Check for valid flag. Let ESM handle that*/
          emm_process_incoming_esm_message(&incloming_message_ptr->esm_incoming_msg, emm_ctrl_data_ptr) ;
#ifdef TEST_FRAMEWORK
          #error code not present
#endif /* TEST_FRAMEWORK */
        }
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
        break ;
      case RESERVED_FOR_TESTS_PROCEDURES:    
        if(incloming_message_ptr->nas_hdr.valid)
        {
          /*Pass the PLAIN TLB message pointer and length of the message to emm_process_incoming_tlb_msg block*/
          emm_process_incoming_tlb_msg((byte *)incloming_message_ptr->nas_hdr.ota_msg_ptr, 
                                    (word)incloming_message_ptr->nas_hdr.ota_msg_len);
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB, "=EMM= security procedure failed for 'RESERVED_FOR_TESTS_PROCEDURES', discarding this message");
#ifdef TEST_FRAMEWORK
          #error code not present
#endif /* TEST_FRAMEWORK */
        }
        break;
      default:      
        /* Here the assumption is the MSG_LIB would always say there are 2 messages when processing ATTACH ACCEPT.
           So, in this case if attach accept did not come with any ESM message,
           EMM considers the attach accept as invalid and starts mo detach.
        */        
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Received message with unknown protocol discriminator %d, discarding this message",
                       incloming_message_ptr->emm_incoming_msg.hdr.pd);
        break ;
    } /* switch (protocol_descriminator) */
  } /* for (message_index=0; message_index<message_counter; message_counter++) */
//jalokia
  emm_modem_mem_free(incloming_message_ptr, emm_ctrl_data_ptr);

} /* end of emm_rrc_process_data_indication() */

/*lint -restore */
/*===========================================================================

FUNCTION    EMM_PROCESS_RRC_UL_DATA_CNF

DESCRIPTION
  This function processes RRC_UL_DATA_CNF primitive


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_rrc_ul_data_cnf
(
  lte_rrc_ul_data_cnf_s *rrc_ul_data_cnf,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  emm_pended_msg_list_type *found_message = (emm_pended_msg_list_type *)NULL;
  boolean                   perform_tau   = FALSE ;
  word                      status_index  = 0 ;
  emm_failure_type          emm_failure_cause;

  ASSERT(rrc_ul_data_cnf != (lte_rrc_ul_data_cnf_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  for (status_index=0; status_index<rrc_ul_data_cnf->num_trans_ids; status_index++)
  {
    MSG_HIGH_DS_2(MM_SUB, "=EMM= UL DATA CNF - Msg 0x%x, status %d",
                  rrc_ul_data_cnf->trans_id[status_index],
                  rrc_ul_data_cnf->status);

    found_message = emm_find_pended_message(rrc_ul_data_cnf->trans_id[status_index], emm_ctrl_data_ptr) ;

    if (found_message != (emm_pended_msg_list_type *)NULL)
    {
      if (rrc_ul_data_cnf->status != LTE_RRC_UL_DATA_CNF_SUCCESS)
      {
        if (found_message->msg_tx_failure_callback != NULL)
        {  
            emm_failure_cause.cause_type       = LTE_NAS_IRAT_UL_MSG_CNF_FAILURE;
            emm_failure_cause.cause.cnf_status = rrc_ul_data_cnf->status;
            if (TRUE == found_message->msg_tx_failure_callback(emm_translate_ul_data_cnf_status(rrc_ul_data_cnf->status), 
                                                               rrc_ul_data_cnf->trans_id[status_index],
                                                               emm_failure_cause,
                                                               emm_convert_ta(rrc_ul_data_cnf->ta), 
                                                               found_message,
                                                               emm_ctrl_data_ptr))
            {
              perform_tau = TRUE ;
            }
        }
      }
      else
      {
        if (status_index > 1)
        {
          MSG_ERROR_DS_0(MM_SUB, "=EMM= Violation of the protocol, cancel lte_rrc_ul_data_cnf processing");
          break ;
        }

        found_message->message_sent_indicator = TRUE ;

        if (found_message->msg_delivery_callback != NULL)
        {
          found_message->msg_delivery_callback(rrc_ul_data_cnf->trans_id[status_index]) ;
        }
      }

      if (found_message->message_sent_indicator == TRUE)
      {
        emm_delete_pended_message(found_message, emm_ctrl_data_ptr) ;
      }
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Transaction ID (%u) has not been found",
                     rrc_ul_data_cnf->trans_id[status_index]);
    }
  }
  
#if defined  FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
  emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
#endif 

  if (perform_tau)
  {
    /* Initiate TAU */
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
    emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                               emm_get_eps_update_type(emm_ctrl_data_ptr),
                               FALSE, 
                               emm_ctrl_data_ptr);
  }
} /* end of emm_process_rrc_ul_data_cnf() */

/*===========================================================================

FUNCTION  EMM_PROCESS_RRC_DEACTIVATE_CNF

DESCRIPTION
  This function processes RRC_UL_DEACTIVATE_CNF primitive

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_rrc_deactivate_cnf
(
  lte_rrc_deactivate_cnf_s *rrc_deactivate_cnf,
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{
  emm_security_context_info  *context_ptr = NULL;
  emm_security_context_info  *current_context_ptr = NULL;
  sys_stop_mode_reason_e_type save_temp_stop_mode_reason;

  ASSERT(rrc_deactivate_cnf != (lte_rrc_deactivate_cnf_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Rcved RRC DEACT CNF - %d", rrc_deactivate_cnf->status);

  /* Set Ignore to FALSE. GW will check this flag before proceeding to act on the activation_ind */
  emm_ctrl_data_ptr->ignore_GW_activation_ind = FALSE;
  save_temp_stop_mode_reason = emm_ctrl_data_ptr->rrc_deact_req.deact_reason;

#ifdef FEATURE_LTE_REL10
  /* this flag could be set if Attach/TAU/SR/ESR is triggered due to paging.
     reset the flag */
  emm_ctrl_data_ptr->responding_to_page = FALSE;
#endif

  if(emm_ctrl_data_ptr->emm_ph_status_change_cnf_pending == TRUE)
  {
    emm_ctrl_data_ptr->emm_ph_status_change_cnf_pending = FALSE;

    emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending = FALSE;
    emm_ctrl_data_ptr->ignore_GW_activation_ind = TRUE;

    mm_send_mmr_ph_status_change_cnf();
    return;
  }

  if((emm_ctrl_data_ptr->rrc_deact_req.deact_reason != 
                                                 SYS_STOP_MODE_REASON_ALT_SCAN)
#ifdef FEATURE_DUAL_SIM
     && (emm_ctrl_data_ptr->rrc_deact_req.deact_reason != 
                                              SYS_STOP_MODE_REASON_DSDS_SUSPEND)
#endif
#ifdef FEATURE_SGLTE
     && (emm_ctrl_data_ptr->rrc_deact_req.deact_reason != 
                                                 SYS_STOP_MODE_REASON_SUSPEND_STACK) 
#endif
    && (emm_ctrl_data_ptr->rrc_deact_req.deact_reason != 
                                                 SYS_STOP_MODE_REASON_DUAL_SWITCH)
     )
  {
    /* save the NAS security context info in emm ctrl data to the USIM  or EFS */
    current_context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr,
                                               CONTEXT_STATE_CURRENT);
    if( current_context_ptr != NULL )
    {
        context_ptr = emm_get_context_by_type (emm_ctrl_data_ptr, 
                                               CONTEXT_TYPE_FULL_NATIVE_EPS);

        /* if there are only mapped contexts at this time, do not save it to USIM or EFS */
        if( context_ptr != NULL )
        {
          if (emm_ctrl_data_ptr->eps_storage_supported)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= eps_storage_supported supported, update context in USIM");
            emm_update_usim_eps_security_context(context_ptr);
          }
          else
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= eps_storage_supported NOT supported, update context in EFS");
            if ( emm_write_efs_security_context(context_ptr) == FALSE )
            {
                MSG_FATAL_DS(MM_SUB, "=EMM= write efs security context return FALSE", 0,0,0 );
			}
          }
        }  
      }
    else
    {
      /* If there is no current EPS context.
         Any native EPS NAS security context stored in UICC or EFS is marked as invalid at power off 
      */
      if( emm_ctrl_data_ptr->detach_reason == SWITCH_OFF )
        emm_update_stored_security_context(emm_ctrl_data_ptr, TRUE);

    }

    if(emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                   SYS_SIM_STATE_NOT_AVAILABLE)
    {
      /* Invalidate GUTI */
      emm_ctrl_data_ptr->emm_guti_valid = FALSE;
      memset((void *)&emm_ctrl_data_ptr->emm_guti, 
             0xFF, 
             sizeof(lte_nas_emm_guti_type));

      /* Invalidate IMSI */
      memset((void *)&emm_ctrl_data_ptr->imsi, 0xFF, sizeof(lte_rrc_imsi_s));
    }

    switch(emm_ctrl_data_ptr->rrc_deact_req.deact_reason)
    {
      case SYS_STOP_MODE_REASON_DEEP_SLEEP:
#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE
      case SYS_STOP_MODE_REASON_UE_MODE_CHANGE:
#endif
		if(emm_ctrl_data_ptr->rrc_deact_req.deact_reason == 
		                     SYS_STOP_MODE_REASON_DEEP_SLEEP)
		{
		  /*Send NO_SRV to GSTK*/
          emm_send_no_svc_to_gstk();
        }
        /* Move EMM connection state to IDLE*/
        EMM_MOVES_TO_IDLE_STATE();
        /* Move EMM mobility state */
        if(EMM_GET_STATE() == EMM_DEREGISTERED)
        {
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
        }
        else
        {
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
        }   
        break;
      case SYS_STOP_MODE_REASON_POWER_OFF:
        /* Reset as needed. Only Some parameters will need to be reset and other 
           retained to be used on coming back to LTE upon inter-RAT procedures */
        emm_reset_ctrl_data(emm_ctrl_data_ptr, TRUE);
        mm_reset_global_variables();
        gmm_reset_global_variables();
        EMM_MOVES_TO_NULL_STATE();
        mm_last_active_rat_stopped = SYS_RAT_LTE_RADIO_ACCESS;
        /*Reset the ACTIVE_RAT to NONE, as REG/CM will come up with a new REG_REQ once 
          stop_mode_cnf is sent (above) - then MM/EMM will behave similar to UE power up scenario*/
        emm_ctrl_data_ptr->plmn_service_state->active_rat = SYS_RAT_NONE;
        break;
      case SYS_STOP_MODE_REASON_IRAT:
        /* Move EMM connection state to IDLE*/
        EMM_MOVES_TO_IDLE_STATE();
        /* Move EMM mobility state */
        if(EMM_GET_STATE() == EMM_DEREGISTERED)
        {
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
          emm_reset_ctrl_data(emm_ctrl_data_ptr, FALSE);
          mm_reset_global_variables();
          gmm_reset_global_variables();
        }
        else
        {
          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
        }
        mm_last_active_rat_stopped = SYS_RAT_LTE_RADIO_ACCESS;
        emm_ctrl_data_ptr->last_reg_cause = MMR_CAUSE_NONE;

        break;
      case SYS_STOP_MODE_REASON_MODE_CHANGE: 
      case SYS_STOP_MODE_REASON_LPM:
      case SYS_STOP_MODE_REASON_OFFLINE:    

        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
        /* Update GMM state if necessary */
        gmm_multimode_set_state(GMM_DEREGISTERED, GMM_NO_CELL_AVAILABLE);
        mm_reset_global_variables();
        gmm_reset_global_variables();
        emm_reset_attempt_counter();
        mm_last_active_rat_stopped = SYS_RAT_LTE_RADIO_ACCESS;
        /*Reset the ACTIVE_RAT to NONE, as REG/CM will come up with a new REG_REQ once 
          stop_mode_cnf is sent (above) - then MM/EMM will behave similar to UE power up scenario*/
        emm_ctrl_data_ptr->plmn_service_state->active_rat = SYS_RAT_NONE;
        emm_ctrl_data_ptr->last_reg_cause = MMR_CAUSE_NONE;

        break;
      default:
        EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
        /* Update GMM state if necessary */
        gmm_multimode_set_state(GMM_DEREGISTERED, GMM_NO_CELL_AVAILABLE);
        break;
    }
  }
  else
  {
    if(EMM_GET_STATE() == EMM_DEREGISTERED)
    {
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
    }
    else
    {
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
    }   
  }

  /* If UE is in LTE RLF and stop mode reqason is alt scan then donot clean the message buffer */
  if((emm_ctrl_data_ptr->rrc_deact_req.deact_reason != SYS_STOP_MODE_REASON_ALT_SCAN) ||
     (EMM_GET_CONNECTION_STATE() != EMM_CONNECTED_STATE))
  {
    /* LTE stack is deactivated. Clean up message buffer. */
    MSG_HIGH_DS_1(MM_SUB, "=EMM= LTE stack is deactivated, Clean up message buffer stop_mode_reason %d",
                  emm_ctrl_data_ptr->rrc_deact_req.deact_reason);
    emm_clean_msg_buffer(emm_ctrl_data_ptr);
  }

  /* LTE stack is deactivated. Clean up PDN CONN REQ for ATTACH if necessary */
  emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);

  /* RRC is deactivated and it needs new rrc service req or plmn search req to go active again.
     Set service status to no service. */
  emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_NO_SRV;

  emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending = FALSE; 
  
  if(emm_ctrl_data_ptr->mm_send_mmr_stop_mode_cnf_fp != NULL)
  {
    if(emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending == TRUE)
    {
      /* Stop mode collided with L to GW RAT change deactivation. Need to deact 
         RRC with stop mode cause one more time */
      emm_init_rrc_deact_req(emm_ctrl_data_ptr->rrc_deact_req.deact_reason, 
                             emm_ctrl_data_ptr);
    }
    else
    {
      /* Reset deact reason */
      emm_ctrl_data_ptr->rrc_deact_req.deact_reason = SYS_STOP_MODE_REASON_NONE;

      /* Report Stop Mode confirmation to upper layers */
      emm_ctrl_data_ptr->mm_send_mmr_stop_mode_cnf_fp();
    }
  }
  else
  {
    emm_ctrl_data_ptr->mm_send_mmr_stop_mode_cnf_fp = mm_send_mmr_stop_mode_cnf;
  }

  if(emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_REQ_PENDING)
  {
    emm_process_ps_detach_req(emm_ctrl_data_ptr->ps_detach_info.ps_detach_req, 
                              emm_ctrl_data_ptr);
  }
  else if(emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_CNF_PENDING)
  {
    emm_ps_detach_clean_up(emm_ctrl_data_ptr);  
  }

  /* Process any pending service request */
  if(emm_reg_container != NULL)
  {
    emm_reg_container(emm_ctrl_data_ptr);
  }
  else if(emm_ctrl_data_ptr->reg_req_pended_indication == TRUE)
  {
    emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;
    reg_service_report = (emm_reg_service_report_type)emm_send_reg_ind;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Reset reg_req_pended_indication to FALSE");
  }

  if(emm_ctrl_data_ptr->pending_reg_cmd_ptr != NULL)
  {
    if ((save_temp_stop_mode_reason == SYS_STOP_MODE_REASON_ALT_SCAN)||
        (save_temp_stop_mode_reason== SYS_STOP_MODE_REASON_DEEP_SLEEP))
     {
       switch(emm_ctrl_data_ptr->pending_reg_cmd_ptr->cmd.hdr.message_id)
       {
         case MMR_PLMN_SEARCH_REQ:
           emm_process_reg_command(emm_ctrl_data_ptr->pending_reg_cmd_ptr,
                                  emm_ctrl_data_ptr);
           break;
         case MMR_PLMN_SEARCH_ABORT_REQ:
           emm_send_empty_plmnsearch_cnf(
             &emm_ctrl_data_ptr->pending_reg_cmd_ptr->cmd.mmr_plmn_search_req);
           reg_service_report(MMR_CAUSE_NOT_SPECIFIED,FALSE,emm_ctrl_data_ptr);
           break;
         default:
           MSG_HIGH_DS_1(MM_SUB ,"=MM= Discarding unexpected pending REG cmd 0x%x", 
               emm_ctrl_data_ptr->pending_reg_cmd_ptr->cmd.hdr.message_id);
           break;
       }
       
     }
    /* In case UE comes back to normal handler before receiving DEACT CNF in 
       OOS/RLF hander, free the memory for pending reg cmd. */

    emm_modem_mem_free(emm_ctrl_data_ptr->pending_reg_cmd_ptr,
                       emm_ctrl_data_ptr);
    emm_ctrl_data_ptr->pending_reg_cmd_ptr = NULL;
  }
  
  /* Do not deallocate memory since UE is trying to camp on LTE */
  if(emm_ctrl_data_ptr->reg_req_pended_indication != TRUE)
  {
    /* Free the memory allocated dynamically */
    emm_procedure_message_mem_free(emm_ctrl_data_ptr);
  }
} /* end of emm_process_rrc_deactivate_cnf() */

/*===========================================================================

FUNCTION    EMM_ABORT_ONGOING_SPECIFIC_PROCEDURE

DESCRIPTION
  This function aborts EMM ongoing specific procedures

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_abort_ongoing_specific_procedure
(
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ;

  switch(EMM_GET_STATE())
  {
    case EMM_DEREGISTERED_INITIATED:
      /* Abort detach procedure */
      emm_abort_mo_detach(emm_ctrl_data_ptr, TRUE);
      break;
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      /* Abort TAU procedure */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Aborting ongoing specific procedure - TAU");
      mm_stop_timer(TIMER_T3430);
      emm_remove_release_client(emm_tau_request_release_indication);
      break;
    default:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= No EMM specific procedure ongoing");
      /* No EMM specific procedure is aborted, return FALSE */
      return FALSE;
  }
  /* A EMM specific procedure is aborted, return TRUE */
  return TRUE;
} /* emm_abort_ongoing_specific_procedure */

/*===========================================================================

FUNCTION    EMM_PROCESS_RRC_PAGE_INDICATION

DESCRIPTION
  This function processes the RRC PAGE INDICATION.Refer to 5.6.2 Paging procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void emm_process_rrc_page_indication
(
  lte_rrc_page_ind_s      *rrc_page_ind,
  emm_ctrl_data_type      *emm_ctrl_data_ptr
)
{
  emm_state_type     emm_state = EMM_INVALID_STATE;
  emm_substate_type  emm_substate;
  boolean            emm_specific_procedure_aborted = FALSE;

#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type local_sub_id = SYS_MODEM_AS_ID_1;
  boolean is_multisim_device = TRUE;
#endif

  emm_csfb_rsp  csfb_response;
  boolean buffer_esr = FALSE;
  ASSERT(rrc_page_ind != (lte_rrc_page_ind_s *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  emm_state = EMM_GET_STATE();
  emm_substate = EMM_GET_SUBSTATE();

  /*initialize it*/
  memset((void *)&csfb_response,0,sizeof(emm_csfb_rsp));
  MSG_HIGH_DS_6(MM_SUB, "=EMM= RRC_PAGE_IND - UE ID type %d, EMM State %d, Substate %d Srv status %d, Update status %d, Sim state %d",
                rrc_page_ind->ue_id.type,
                emm_state,
                emm_substate,
                emm_ctrl_data_ptr->plmn_service_state->service_status,
                emm_ctrl_data_ptr->emm_update_status,
                emm_ctrl_data_ptr->plmn_service_state->sim_state);

  if((rrc_page_ind->cn_domain == LTE_RRC_PAGING_CN_DOMAIN_CS)&&
     ((emm_ignore_cs_page_cs_service_notification(emm_ctrl_data_ptr) == TRUE)||
      (emm_reject_cs_service_notification(emm_ctrl_data_ptr) == TRUE)))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Ignoring MT CS Paging during VOLTE call");
    return;
  }

  switch (rrc_page_ind->ue_id.type)
  {
    case LTE_RRC_UE_ID_STMSI:
      if(rrc_page_ind->cn_domain == LTE_RRC_PAGING_CN_DOMAIN_CS)
      {
#ifdef FEATURE_LTE_REL10
        if (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
        {
          mm_stop_timer(TIMER_T3346);
          gmm_set_gmm_is_congestion_due_to_rau(FALSE);
          if(EMM_GET_SUBSTATE() == EMM_REGISTERED_ATTEMPTING_TO_UPDATE)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Received CS page ind. initiating TAU\n");
           /* set the flag to remember TAU is triggered due to paging.
              this information will be used to encode device properties IE and setting
              rrc_est_cause to delay tolerant */
            emm_ctrl_data_ptr->responding_to_page = TRUE;
            emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                                       emm_get_eps_update_type(emm_ctrl_data_ptr),
                                       FALSE, 
                                       emm_ctrl_data_ptr);
          }
        }
#endif
        MSG_HIGH_DS_2(MM_SUB, "=EMM= Received Page with S-TMSI = M-TMSI(%x) + MMEC(%x) and CN domain set to CS",
                      rrc_page_ind->ue_id.value.s_tmsi.m_tmsi,
                      rrc_page_ind->ue_id.value.s_tmsi.mmec);
#ifdef FEATURE_DUAL_SIM
        for(local_sub_id = SYS_MODEM_AS_ID_1;local_sub_id < MAX_AS_IDS; local_sub_id++)
        {
          if (mm_sub_config_sim[local_sub_id] != SYS_OVERALL_FEATURE_MODE_MULTISIM)
          {
            is_multisim_device = FALSE;
          }
        }
        if(MMCNM_IS_MODE_DUAL_STANDBY && is_multisim_device && (cm_call_is_there_call_on_other_sub((sys_modem_as_id_e_type)mm_as_id,CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_EMERG_MASK ) == TRUE))
        {
          MSG_HIGH_DS_0(MM_SUB,"=MM= ignoring page on this sub as CM is starting a voice call on other sub");
          return;
        }
#endif
        //emm_validate_page_req();/* In this check for the validity of PAGE for the current EMM state */
        if(emm_check_if_csfb_allowed(LTE_MT_CSFB_1XCSFB,NAS_EMM_3GPP_CSFB,emm_ctrl_data_ptr,&buffer_esr) == TRUE )
        {
          /* Now, validate the PAGE with MM*/
          if(mm_csfb_process_mt_page (EMM_CSFB_PAGINGI_TYPE_TMSI) == TRUE)
          {
            csfb_response.valid = TRUE;
            csfb_response.csfb_rsp =  CSFB_ACCEPT;
                /**-------------------------------------------------------------
           **At reception of an indication of paging for CS services from EMM, 
           **the MS shall stop timer T3246, if it is running. The MM sublayer in the MS requests 
           **EMM to perform the service request procedure for CS fallback.
           **(24.008 section 4.5.1.3.4.) ------------------------------------------------*/
#ifdef FEATURE_NAS_REL10
            mm_stop_timer(TIMER_T3246);
#endif
            emm_ctrl_data_ptr->responding_to_page = TRUE;
            emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MT_ACCESS,LTE_MT_CSFB_1XCSFB,csfb_response,NAS_EMM_3GPP_CSFB);
          }
          else
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= MM rejected the PAGE message");
          }
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB, "=EMM= CSFB not allowed");
        }
        return;
      }

      if( EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE )
      {
        MSG_ERROR_DS_0(MM_SUB, "=EMM= Unexpected RRC_PAGE_IND");
        return;
      }

      /* Only proceed if there is no EMM specific procedure or service request
         procedure ongoing and EMM is not in IDLE or DEREGISTERED state */
      if(((emm_state == EMM_REGISTERED) && 
          (emm_substate == EMM_REGISTERED_NORMAL_SERVICE ||
           emm_substate == EMM_REGISTERED_UPDATE_NEEDED ||
           emm_substate == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM ||
           emm_substate == EMM_REGISTERED_WAITING_FOR_ESM_ISR_STATUS))
         &&
         (emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                            SYS_SRV_STATUS_SRV)
         &&
         (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED) 
         &&
         ((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_AVAILABLE) ||
          (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_CS_INVALID))
         &&((emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag == TRUE)||
             emm_check_mplmn_in_progress(emm_ctrl_data_ptr)== FALSE))
      {
#ifdef FEATURE_LTE_REL10
        if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
           (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE))
        {
          /* Stop the Timer */
          mm_stop_timer(TIMER_T3346);
	  gmm_set_gmm_is_congestion_due_to_rau(FALSE);
          emm_timer_t3346_expire(TRUE, emm_ctrl_data_ptr) ;
        }  
#endif        
        if(emm_state != EMM_TRACKING_AREA_UPDATING_INITIATED)
        {
          emm_ctrl_data_ptr->responding_to_page = TRUE;
          if(emm_check_esr_for_ps(emm_ctrl_data_ptr) == TRUE)
          {
            /* Start extended service request procedure for PS */
            emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MT_ACCESS,LTE_PACKET_SERVICES_VIA_S1,
                                                         csfb_response, NAS_EMM_CSFB_NONE);
          }
          else
          {
            /* Start service request procedure with the cause of MT_ACCESS. */
            emm_start_nas_service_request_procedure(emm_ctrl_data_ptr, 
                                                  LTE_RRC_EST_CAUSE_MT_ACCESS);
          }
        }
      }
#ifdef FEATURE_LTE_REL10
      else if (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
      {
        mm_stop_timer(TIMER_T3346);
	gmm_set_gmm_is_congestion_due_to_rau(FALSE);
        if(EMM_GET_SUBSTATE() == EMM_REGISTERED_ATTEMPTING_TO_UPDATE)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Received page ind. initiating TAU\n");
         /* set the flag to remember TAU is triggered due to paging.
            this information will be used to encode device properties IE and setting
            rrc_est_cause to delay tolerant */
          emm_ctrl_data_ptr->responding_to_page = TRUE;
          emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                            emm_get_eps_update_type(emm_ctrl_data_ptr),
                            FALSE, 
                            emm_ctrl_data_ptr);
        }
      }
#endif
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Ignoring RRC_PAGE_IND - SR not allowed");
      }
      break;
    case LTE_RRC_UE_ID_IMSI:
      if(rrc_page_ind->cn_domain == LTE_RRC_PAGING_CN_DOMAIN_CS)
      {
#ifdef FEATURE_LTE_REL10
        if (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
        {
          mm_stop_timer(TIMER_T3346);
	  gmm_set_gmm_is_congestion_due_to_rau(FALSE);
          if(EMM_GET_SUBSTATE() == EMM_REGISTERED_ATTEMPTING_TO_UPDATE)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Received page ind. initiating TAU\n");
            /* set the flag to remember TAU is triggered due to paging.
               this information will be used to encode device properties IE and setting
               rrc_est_cause to delay tolerant */
            emm_ctrl_data_ptr->responding_to_page = TRUE;
            emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                               emm_get_eps_update_type(emm_ctrl_data_ptr),
                               FALSE, 
                               emm_ctrl_data_ptr);
          }
        }
#endif
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Received Page with IMSI and CN domain set to CS");
#ifdef FEATURE_DUAL_SIM
        for(local_sub_id = SYS_MODEM_AS_ID_1;local_sub_id < MAX_AS_IDS; local_sub_id++)
        {
          if (mm_sub_config_sim[local_sub_id] != SYS_OVERALL_FEATURE_MODE_MULTISIM)
          {
            is_multisim_device = FALSE;
          }
        }
        if(MMCNM_IS_MODE_DUAL_STANDBY && is_multisim_device && (cm_call_is_there_call_on_other_sub((sys_modem_as_id_e_type)mm_as_id,CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_EMERG_MASK ) == TRUE))
        {
          MSG_HIGH_DS_0(MM_SUB,"=MM= ignoring page on this sub as CM is starting a voice call on other sub");
          return;
        }
#endif
        //emm_validate_page_req();/* In this check for the validity of PAGE for the current EMM state */
        if(emm_check_if_csfb_allowed(LTE_MT_CSFB_1XCSFB,NAS_EMM_3GPP_CSFB,emm_ctrl_data_ptr,&buffer_esr) == TRUE )
        {
          /* Now, validate the PAGE with MM*/
          if(mm_csfb_process_mt_page (EMM_CSFB_PAGING_TYPE_IMSI) == TRUE)
          {
            csfb_response.valid = TRUE;
            csfb_response.csfb_rsp =  CSFB_ACCEPT;
              /**-------------------------------------------------------------
           **At reception of an indication of paging for CS services from EMM, 
           **the MS shall stop timer T3246, if it is running. The MM sublayer in the MS requests 
           **EMM to perform the service request procedure for CS fallback.
           **(24.008 section 4.5.1.3.4.) ------------------------------------------------*/
#ifdef FEATURE_NAS_REL10
            mm_stop_timer(TIMER_T3246);
#endif
            emm_ctrl_data_ptr->responding_to_page = TRUE;
            emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MT_ACCESS,LTE_MT_CSFB_1XCSFB,csfb_response,NAS_EMM_3GPP_CSFB);
          }
          else
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= MM rejected the PAGE message");
          }
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= MM rejected the PAGE message");
        }
        return;
      }

#ifdef FEATURE_LTE_REL10
      /* Stop the Timer */
      mm_stop_timer(TIMER_T3346);
      gmm_set_gmm_is_congestion_due_to_rau(FALSE);
#endif

      if((emm_state == EMM_REGISTERED) || 
         (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED) ||
         (emm_state == EMM_DEREGISTERED_INITIATED))
      {      
        /* 5.6.2.2.2 - Abort ongoing EMM specific procedure */
        /* Note: Attach does not apply here since EMM only gets page ind in the REGISTERED state */
        emm_specific_procedure_aborted = emm_abort_ongoing_specific_procedure(emm_ctrl_data_ptr);

        /* Send detach ind to ESM to release bearers */
        emm_build_and_send_detach_ind(EMM_DETACHED);
        emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED, 
                                     SYS_SIM_STATE_AVAILABLE, 
                                     emm_ctrl_data_ptr, FALSE) ;
    if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_UMTS_RADIO_ACCESS)|| 
       mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_GSM_RADIO_ACCESS) ||
       mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_TDS_RADIO_ACCESS))
    {
      gmm_invalidate_parameters();
    }

        if((emm_specific_procedure_aborted == TRUE) &&
           (emm_ctrl_data_ptr->ps_detach_info.status == 
                                                    EMM_PS_DETACH_CNF_PENDING))
        {
          emm_ps_detach_clean_up(emm_ctrl_data_ptr);
          reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
        }
        else
        {
          /* Move to EMM_DEREGISTERED_NORMAL_SERVICE state to be ready to re-attach */
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NORMAL_SERVICE) ;
        }

#ifdef FEATURE_LTE_REL10
        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
        {
          emm_send_unblock_all_apns_ind();
          /* set the flag to remember Attach is triggered due to paging.
            this information will be used to encode device properties IE and setting
            rrc_est_cause to delay tolerant */
          emm_ctrl_data_ptr->responding_to_page = TRUE;
        }
#endif

        /* Notify context lost */
        emm_send_irat_ctxt_lost_ind(LTE_NAS_CTXT_LOST_IMSI_PAGING);
        /* If a EMM specific procedure is aborted, attach procedure will be 
           triggered by receiving a RRC service ind after RRC goes to IDLE 
           and RRC connection is released */
        if (emm_specific_procedure_aborted == FALSE)
        {
          /* If no EMM specific procedure is aborted, no RRC service ind will 
             come. EMM should start attach procedure here. */
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
      }
      else
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Invalid EMM state %d, ignore RRC_PAGE_IND with IMSI",
                     (int)emm_state);
      }
      break;
    default:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= Unknown RRC_PAGE_IND UE ID type");
      break;
  }
} /* emm_process_rrc_page_indication */

/*=============================================================================

FUNCTION    EMM_PROCESS_RRC_PLMN_SEARCH_CNF

DESCRIPTION
  This function processes LTE_RRC_PLMN_SEARCH_CNF primitive

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void  emm_process_rrc_plmn_search_cnf
(
  lte_rrc_plmn_search_cnf_s  *rrc_plmn_search_cnf_ptr,
  emm_ctrl_data_type         *emm_ctrl_data_ptr
)
{
  lte_nas_tai_lst1_type current_tai;
  emm_state_type            emm_state = (emm_state_type)EMM_UNKNOWN_STATE;
  int length               =0;
  ASSERT(rrc_plmn_search_cnf_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);

  if(rrc_plmn_search_cnf_ptr->search_type != SYS_SERVICE_SEARCH_PCI_PLMN)
  {
    length = rrc_plmn_search_cnf_ptr->found_plmn_list.plmn_list.length;
  }
  else
  {
    length = rrc_plmn_search_cnf_ptr->found_plmn_list.pci_plmn_list.num_cell;
  }

  MSG_HIGH_DS_3(MM_SUB, "=EMM= Rcved RRC PLMN SEARCH CNF - Trans ID %d, Status %d No of PLMNs or No of Cells found %d",
                rrc_plmn_search_cnf_ptr->trans_id,
                rrc_plmn_search_cnf_ptr->status,
                length);
  
#ifdef FEATURE_DUAL_DATA
  if (rrc_plmn_search_cnf_ptr->status != LTE_RRC_PLMN_SEARCH_PARTIAL &&
      rrc_plmn_search_cnf_ptr->network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL)
  {
    emm_send_rr_ps_resume_ind(emm_ctrl_data_ptr);
  }
#endif

  emm_state = EMM_GET_STATE();
  current_tai.plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(emm_ctrl_data_ptr->last_rrc_service_ind_ptr->
                                                                                 camped_svc_info.selected_plmn) ;
  current_tai.tac = emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.tac ;

  /* Dont send Plmn to mmode if the number of PLMNs found are 0*/
  if( mm_emm_is_qrd_device == TRUE && 
	  rrc_plmn_search_cnf_ptr->search_type != SYS_SERVICE_SEARCH_PCI_PLMN &&
  	  rrc_plmn_search_cnf_ptr->status == LTE_RRC_PLMN_SEARCH_PARTIAL &&
  	  rrc_plmn_search_cnf_ptr->found_plmn_list.plmn_list.length == 0)
  {
     return ;
  }
  if(emm_ctrl_data_ptr->ps_detach_info.status != EMM_PS_DETACH_NONE)
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= PS Detach pending %d - Discarding PLMN search result",
                  emm_ctrl_data_ptr->ps_detach_info.status);
    rrc_plmn_search_cnf_ptr->status = LTE_RRC_PLMN_SEARCH_ABORTED;
  }

  emm_send_mmr_plmn_search_cnf(rrc_plmn_search_cnf_ptr);

  /*EMM just like MM will always send the service indication to REG after PLMN search cnf, for updating the service status*/
  if( rrc_plmn_search_cnf_ptr->status != LTE_RRC_PLMN_SEARCH_PARTIAL)						  
  {
    /*Accept ACTIVEATION_INDICATION from RRC*/
    emm_ctrl_data_ptr->ignore_GW_activation_ind    = FALSE;

    mm_set_plmn_search_in_progress_flag(FALSE);

    if(!(((emm_state == EMM_REGISTERED_INITIATED )||
          (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED))||
          ((mm_timer_status[TIMER_T3411] == TIMER_ACTIVE)&&
           ((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI)||
            (emm_search_tai_list(&current_tai,emm_ctrl_data_ptr->emm_tai_list_ptr)== FALSE))&&
           ((sys_plmn_match(current_tai.plmn,emm_ctrl_data_ptr->last_attempted_tai.plmn) == TRUE)&&
            (current_tai.tac == emm_ctrl_data_ptr->last_attempted_tai.tac)))))
    {
        if(emm_ctrl_data_ptr->last_reg_cause != MMR_CAUSE_NONE)
        {
          emm_send_reg_ind(emm_ctrl_data_ptr->last_reg_cause, FALSE, emm_ctrl_data_ptr);
        }
        else
       {
          emm_send_reg_ind(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
       }
    }
  }
} /* emm_process_rrc_plmn_search_cnf */


/*=============================================================================

FUNCTION    EMM_PROCESS_LTE_RRC_CCO_NACC_COMPLETED_IND

DESCRIPTION
  This function processes LTE_RRC_CCO_NACC_COMPLETED_IND primitive

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/

void emm_process_lte_rrc_cco_nacc_completed_ind
(
  emm_ctrl_data_type         *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Rcved LTE_RRC_CCO_NACC_COMPLETED_IND");
  EMM_MOVES_TO_IDLE_STATE();
  mm_start_timer(TIMER_T3412,emm_ctrl_data_ptr->t3412_value);
  /* Send ESM connection release indication */
  emm_esm_release_indication(CONN_REL_LTE_RRC_CONN_REL_NORMAL, emm_ctrl_data_ptr);
  mm_process_cco_complete_ind(emm_ctrl_data_ptr);
  /* This is for scenario where when service req on LTE is pending and we receive stop cnf 
     from GSM before CCO Complete indication from LTE. In this case we will pend the reg request.
     Please check emm_process_reg_req for reg request pending.
     The pending reg request is processed now.*/
  if (emm_reg_container != (emm_reg_container_type)NULL)
  {
    emm_reg_container(emm_ctrl_data_ptr) ;
  }
}


#ifdef FEATURE_FEMTO_CSG
/*=============================================================================

FUNCTION    emm_process_lte_rrc_csg_info_update_ind

DESCRIPTION
  This function processes LTE_RRC CSG info update

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void emm_process_lte_rrc_csg_info_update_ind
(
  lte_rrc_csg_info_update_ind_s  *csg_info_update,
  emm_ctrl_data_type                  *emm_ctrl_data_ptr
)
{

  sys_plmn_id_s_type  rrc_plmn_id;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  MSG_HIGH_DS_0(MM_SUB, "=EMM= LTE_RRC_CSG_INFO_UPDATE_IND");

  rrc_plmn_id = emm_convert_rrc_plmn_id_to_nas_plmn_id(csg_info_update->selected_plmn);

  if((emm_ctrl_data_ptr->plmn_service_state->active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
      (emm_ctrl_data_ptr->plmn_service_state->csg_info.csg_id == csg_info_update->csg_info.csg_id) &&
      (sys_plmn_match(emm_ctrl_data_ptr->plmn_service_state->plmn, rrc_plmn_id)))
  {
    emm_ctrl_data_ptr->plmn_service_state->csg_info.hnb_name = csg_info_update->csg_info.hnb_name;
    emm_send_reg_home_nodeb_name_ind(csg_info_update);
  }
}

#endif 

/*=============================================================================
FUNCTION    emm_process_lte_rrc_sib_info_ind

DESCRIPTION
  This function processes SIB INFO IND from LTE_RRC for SIB6 and SIB7

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void emm_process_lte_rrc_sib_info_ind
(
  emm_rrc_cmd_type         *rrc_cmd_ptr,
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{
  ASSERT(rrc_cmd_ptr != (emm_rrc_cmd_type *)NULL);
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);

  switch(rrc_cmd_ptr->rrc_msgr_cmd.msg_hdr.id)
  {
    case LTE_RRC_SIB6_INFO_IND:
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Received LTE_RRC_SIB6_INFO_IND");
    emm_send_mmr_rrc_sib6_info_ind(&rrc_cmd_ptr->rrc_msgr_cmd.rrc_sib6_info_ind);
    break;

    case LTE_RRC_SIB7_INFO_IND:
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Received LTE_RRC_SIB7_INFO_IND");
    emm_send_mmr_rrc_sib7_info_ind(&rrc_cmd_ptr->rrc_msgr_cmd.rrc_sib7_info_ind);
    break;

    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Unexpected SIB INFO IND primitive %d", 
                                   rrc_cmd_ptr->rrc_msgr_cmd.msg_hdr.id);
    break;
  }
}


#ifdef FEATURE_NAS_EAB
/*=============================================================================
FUNCTION    emm_process_lte_rrc_eab_update_ind

DESCRIPTION
  This function processes EAB UPDATE IND from LTE_RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void emm_process_lte_rrc_eab_update_ind
(
  lte_rrc_eab_update_ind_s *eab_update_ind,
  emm_ctrl_data_type       *emm_ctrl_data_ptr
)
{
  ASSERT(eab_update_ind != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS_2(MM_SUB, "=EMM= Received LTE_RRC_EAB_UPDATE_IND, eab_valid %d, eab_barring %d",
                eab_update_ind->eab_valid, emm_ctrl_data_ptr->eab_barring);

  if(eab_update_ind->eab_valid == FALSE)
  {
    emm_stop_eab_barring(emm_ctrl_data_ptr);
  }
}
#endif

boolean emm_process_rrc_primitive
(
  void                  *rrc_message_router_cmd,// Changed the i/p parameter to void*, to be compatible to function pointer
  emm_ctrl_data_type    *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
)
{
  emm_rrc_cmd_type   *rrc_msgr_cmd; //Initialize emm_rrc_cmd_type pointer
  emm_security_context_info  *context_ptr = NULL;
  boolean             start_tau = FALSE;

  rrc_msgr_cmd = (emm_rrc_cmd_type*)rrc_message_router_cmd; //Typecast rrc_message_router_cmd to emm_rrc_cmd_type

  ASSERT(rrc_msgr_cmd != (emm_rrc_cmd_type *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  switch (rrc_msgr_cmd->rrc_msgr_cmd.msg_hdr.id)
  {
    case LTE_RRC_CONN_EST_CNF:
      emm_process_connection_confirmation(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_conn_est_cnf,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_UL_DATA_CNF:
      emm_process_rrc_ul_data_cnf(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_ul_data_cnf,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_DEACTIVATE_CNF:
      emm_process_rrc_deactivate_cnf(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_deactivate_cnf,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_ACTIVATION_IND:
      if((emm_ctrl_data_ptr->plmn_service_state->active_rat == 
                                                   SYS_RAT_GSM_RADIO_ACCESS) ||
         (emm_ctrl_data_ptr->plmn_service_state->active_rat == 
                                                   SYS_RAT_UMTS_RADIO_ACCESS)||
         (emm_ctrl_data_ptr->plmn_service_state->active_rat == 
                                                   SYS_RAT_TDS_RADIO_ACCESS))
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= LTE_RRC_ACTIVATION_IND received, current rat =%d",
                      emm_ctrl_data_ptr->plmn_service_state->active_rat);
        mm_emm_process_GW_to_LTE_rat_change(
                                       &rrc_msgr_cmd->rrc_msgr_cmd.rrc_act_ind,
                                        mm_emm_dispatcher_fn_ptr,
                                        emm_ctrl_data_ptr);
      }
      else
      {
        MSG_ERROR_DS_1(MM_SUB, "=EMM= LTE_RRC_ACTIVATION_IND shouldn't be received when active rat is LTE or none, discard the message, current rat=%d",
                       emm_ctrl_data_ptr->plmn_service_state->active_rat);
      }
      break ;

    case LTE_RRC_NAS_UMTS_KEY_IND:
      emm_rrc_process_GW_key_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_umts_key_ind,emm_ctrl_data_ptr) ;
      break;

    case LTE_RRC_NAS_GSM_KEY_IND:
      emm_rrc_process_GW_key_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_umts_key_ind,emm_ctrl_data_ptr) ;
      break;

   case LTE_RRC_NAS_LTE_KEY_IND:
      emm_rrc_process_lte_key_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_lte_key_ind,emm_ctrl_data_ptr) ;
      break;

    case LTE_RRC_SERVICE_IND:
      emm_rrc_process_service_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_service_ind,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_BARRING_UPDATE_IND:
      emm_rrc_process_barring_update_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_barring_update_ind,emm_ctrl_data_ptr) ;
      break;
    case LTE_RRC_PESUDO_RESEL_LOW_PRIORITY_IND:
      emm_rrc_process_pseudo_resel_low_priority_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_pseudo_resel_low_priority_ind,emm_ctrl_data_ptr) ;
      break;
    case LTE_RRC_CONN_REL_IND:
      emm_process_rrc_release_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_conn_rel_ind,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_DL_DATA_IND:
      emm_process_rrc_data_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_dl_data_ind,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_PAGE_IND:
      emm_process_rrc_page_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_page_ind, emm_ctrl_data_ptr);
      break ;
    case LTE_RRC_ACTIVE_EPS_BEARER_UPDATE_IND:
      context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr,
                                               CONTEXT_STATE_CURRENT);
      if( context_ptr != NULL )
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= LTE_RRC_ACTIVE_EPS_BEARER_UPDATE_IND, set nas_secure_exchange to TRUE");
        emm_ctrl_data_ptr->nas_secure_exchange = TRUE;
      }

      /* Received RRC_ACTIVE_ESP_IND which indicates the completion of service request procedure if there is one ongoing */
      /* If needed, complete service request procedure */
      if ( EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED )
      {
        if((emm_ctrl_data_ptr->service_req_cause == 
                                                LTE_RRC_EST_CAUSE_MT_ACCESS) &&
           (emm_ctrl_data_ptr->cell_barred_tau_pending == TRUE))
        {
          /* TAU was delayed because of access class barring. Since UE get paged
             and conn is established, start TAU. */
          if(emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_PERIODIC)
          {
            /* Connection is up by NW paging. No need to start delayed
               periodic TAU. Reset the TAU cause.*/
            emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID;
          }
          else
          {
            emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;
            start_tau = TRUE;
          }
        }

        if( (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE) || 
            (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) || 
            (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED))
        {
          /* Process the service request procedure completion */
          emm_complete_service_request(emm_ctrl_data_ptr);
          if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) || (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED))
          {
            emm_send_esr_if_buffered(emm_ctrl_data_ptr);
          }
        }
        /* Send out all pended messages */
        emm_send_all_pended_messages(emm_ctrl_data_ptr);
        emm_send_all_pending_irat_msg(emm_ctrl_data_ptr);
      }

      /* NW configured radio bearers. No need to sync drb staus anymore. */
      if((EMM_GET_STATE() == EMM_REGISTERED_INITIATED) &&
         (EMM_GET_SUBSTATE() == EMM_WAITING_FOR_ESM_RESPONSE))
      {
        emm_ctrl_data_ptr->sync_drb_status = FALSE;
      }

      emm_process_rrc_eps_bearer_update(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_active_eps_bearer_update_ind,emm_ctrl_data_ptr) ;
      /* Start delayed TAU because of access class barring */
      if(start_tau == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Init delayed TAU upon completion of MT SR");
        emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                                   emm_get_eps_update_type(emm_ctrl_data_ptr),
                                   FALSE, 
                                   emm_ctrl_data_ptr);      
      }
      break ;
    case LTE_RRC_CONN_EST_TIMER_UPDATE_IND:
      emm_start_connection_delay_timers(rrc_msgr_cmd->rrc_msgr_cmd.rrc_conn_est_timer_update_ind.call_type_bitmask,
                                       (dword)rrc_msgr_cmd->rrc_msgr_cmd.rrc_conn_est_timer_update_ind.barring_timer_value,
                                        emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_PLMN_SEARCH_CNF:
      emm_process_rrc_plmn_search_cnf(
                               &rrc_msgr_cmd->rrc_msgr_cmd.rrc_plmn_search_cnf,
                                emm_ctrl_data_ptr);
      if(rrc_msgr_cmd->rrc_msgr_cmd.rrc_plmn_search_cnf.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL 
	      && rrc_msgr_cmd->rrc_msgr_cmd.rrc_plmn_search_cnf.status != LTE_RRC_PLMN_SEARCH_PARTIAL)
      {
        emm_ctrl_data_ptr->plmn_search_in_progress = FALSE;
      }
      break;
    case LTE_RRC_IRAT_TUNNEL_UL_MSG_CNF:
#ifdef FEATURE_LTE_TO_HDR_OH
    case LTE_RRC_IRAT_HDR_TUNNEL_UL_MSG_CNF:
#endif
      emm_process_rrc_irat_tunnel_ul_msg_cnf(
                        &rrc_msgr_cmd->rrc_msgr_cmd.rrc_irat_tunnel_ul_msg_cnf,
                         emm_ctrl_data_ptr);
      break;
    case LTE_RRC_IRAT_TUNNEL_DL_MSG_IND:
      emm_process_rrc_irat_tunnel_dl_msg_ind(
                        &rrc_msgr_cmd->rrc_msgr_cmd.rrc_irat_tunnel_dl_msg_ind,
                         emm_ctrl_data_ptr);
      break;
#ifdef FEATURE_LTE_TO_HDR_OH
    case LTE_RRC_IRAT_HDR_TUNNEL_DL_MSG_IND:
      emm_process_rrc_irat_3gpp2_tunnel_dl_msg_ind(
                        &rrc_msgr_cmd->rrc_msgr_cmd.rrc_irat_cdma_tunnel_dl_msg_ind,
                         emm_ctrl_data_ptr);
      break;
#endif
    case LTE_RRC_1XCSFB_HO_STARTED_IND:
      emm_process_rrc_1xcsfb_ho_started_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_1xcsfb_ho_started_ind,emm_ctrl_data_ptr);
      break;
    case LTE_RRC_1XCSFB_HO_FAILED_IND:
      emm_process_rrc_1xcsfb_ho_failed_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_1xcsfb_ho_failed_ind,emm_ctrl_data_ptr);
      break;

    case LTE_RRC_1XSRVCC_HO_STARTED_IND:
      emm_process_rrc_1xsrvcc_ho_started_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_1xsrvcc_ho_started_ind,emm_ctrl_data_ptr);
      break;
    case LTE_RRC_1XSRVCC_HO_FAILED_IND:
      emm_process_rrc_1xsrvcc_ho_failed_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_1xsrvcc_ho_failed_ind,emm_ctrl_data_ptr);
      break;

    case LTE_RRC_CCO_NACC_COMPLETED_IND:
      emm_process_lte_rrc_cco_nacc_completed_ind(emm_ctrl_data_ptr);
      break;
#ifdef FEATURE_FEMTO_CSG
    case LTE_RRC_CSG_INFO_UPDATE_IND:
      emm_process_lte_rrc_csg_info_update_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_csg_info_update_ind, emm_ctrl_data_ptr);
      break;  
#endif 

    case LTE_RRC_SIB6_INFO_IND:
    case LTE_RRC_SIB7_INFO_IND:
      emm_process_lte_rrc_sib_info_ind(rrc_msgr_cmd, emm_ctrl_data_ptr);
      break;

#ifdef FEATURE_NAS_EAB
    case LTE_RRC_EAB_UPDATE_IND:
      emm_process_lte_rrc_eab_update_ind(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_eab_update_ind, emm_ctrl_data_ptr);
      break;
#endif

    default:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= Unknown RRC primitive");
      break ;
  }
  return TRUE;
} /* end of emm_process_rrc_primitive() */

#endif /*FEATURE_LTE*/
