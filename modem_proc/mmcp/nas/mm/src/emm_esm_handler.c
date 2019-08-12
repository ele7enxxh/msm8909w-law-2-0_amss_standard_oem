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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_esm_handler.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/11/13   us      Abort any ongoing service req procedure or TAU precedure going on before initiating MO detach 
09/17/12   cogolt  Rel-10, Added function emm_send_unblock_all_apns_ind 
12/01/11   SAR     populate emm_failure_type to ESM->CM in failure ind  
08/22/11   MNK     CR: 301219 Clear CSFB context and call if aborted by the upper layer  
04/07/2011 MNK     Add 1xCSFB abort handling 
10/21/10   MNK     Reject SR when ESR is in progress 
10/07/10   MNK     1XCSFB introduction 
05/18/10   zren    Updated fun param to set only_for_reg flag in REG CNF and 
                   REG IND
05/14/10   RI      Mainlined spec changes upto Dec 09.
04/28/10   zren    Updated to also send REG CNF when combined attach is 
                   successful for EPS service only
04/22/10   zren    Updated to use global structure for NAS outgoing message
04/09/10   zren    Added combined procedure feature
04/02/10   zren    Added failure handling for RRC connection release causes of
                   CRE_FAILURE and OOS_DURING_CRE 
03/05/10   MNK     TIN related changes
02/25/10   MNK     Temporray hack to bypass MM state initialization for 2.0 OOS/RLF, when we move to 4.0 OOS/RLF we need to remove this hack
02/24/10   zren    Added support for network selection modes of LIMITED MANUAL
                   ACQUISITION and LIMITED ROUTINE ACQUISITION
02/05/10   MNK     Remove sending of RRC connection rlease upon attach complete
                   failure. No need to send this to ESM
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
11/18/09   zren    Added support for ESM Information procedure

11/10/09   RI      Moved the state transition before sending the 
                   Attach complete message
11/04/09   MNK     Cleanup emm_ctrl_data as necessary upon detaching due to i-RAT
09/24/09   hnam    Reverted back the code, by uncommenting the event_payload function call
09/17/09   zren    Updated to cache esm data req after service request procedure
                   is started in registered state w/o connection
                   Updated to only use ESM transaction ID in service request
                   procedure/send service request failure ind for MO Data type 
                   service request procedure
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
09/14/09   hnam    Activated NAS events
08/25/09   MNK     RRC Interface changes 
08/20/09   hnam    Corrected emm_process_outgouing_esm_message() to support the EMM_INCOMING_EVENTS
08/14/09   zren    Modified procedure connection release handling
08/14/09   hnam    Added support NAS events
07/29/09   zren    Added support to always send SIG_CONN_RELEASED_IND to ESM 
                   when RRC_RELEASE_IND is received
07/15/09   zren    Modified message list handling functions to use message 
                   pointer as parameter
06/11/09   zren    Added support to trigger TAU when UE returns back to 
                   REGISTERED_NORMAL_SERVICE state and EPS bearer context was
                   updated in earlier REGISTERED_NO_CELL_AVAILABLE state
06/02/09   MNK     CR# 184007. These are the changes required to make sure at the time of attach, 
                   EMM receives (ATTACH ACCEPT + ACTIVATE DEFAULT BEARER CONTEXT REQUEST) and EMM sends
                    (ATTACH COMPLETE + ACTIVATE DEFAULT BEARER CONTEXT ACCEPT)
05/29/09   zren    Added support for TAU feature
05/28/09   zren    Added ESM data request failure call back functions which are
                   used when ESM sends Data_Req while connection is existing  
05/19/09   hnam    Added support for transmission failure scenario - when 
                   connection is released by RRC during any UL procedure failures
05/14/09   zren    Added EPS status and SIM state checks before starting Service
                   Request Procedure  
04/23/09   zren    Added detach_cmd processing sent by ESM
04/17/09   zren    Modified to support saving NAS messages in pended message 
                   list in the format of lte_nas_outgoing_msg_type
                   Added one parameter to function emm_send_rrc_connection_establishment_request() 
                   Sepearted RRC_DATA_CNF and RRC_EST_CNF for RRC_EST_REQ
                   Modified EMM connection state machines
04/16/09   MNK     Fixed REGISTERED state behaviour for processing EMM_DAT_REQ
04/02/09   zren    Added support for new trnsaction ID format 
03/24/09   zren    Added support for service request and paging feature
02/23/09   vdr     Fixed Lint errors & warnings
02/20/09   vdr     Now evaluating return value of the emm_msgr_send() and cleaning
                   up structure representation of the outgouing ESM OTA messages
02/11/09   vdr     Registering emm_esm_release_indication() which will be called
                   upon reception of the RRC Release indication
02/06/09   vdr     Modified failure delivery callbacks in order to delete message 
                   from the cache mem
02/15/09   MNK     Changed encode_nas_msg to encode_nas_msg_fp
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE

#include "emm.h"
#include "rex.h"
#include "emm_state.h"
#include "emm_database.h"
#include "msgr.h"
#include "msgr_lte.h"
#include "esm_emm_msg_if.h"
#include "esm_emm_msgr_enum_msg.h"
#include "lte_nas_common_v.h" 
#include "lte_nas_esm_message_type.h"
#include "lte_rrc_ext_msg.h"
#include "emm_esm_if_msg.h"
#include "lte_nas_local_cause.h"
#include "emm_update_lib.h"
#include "lte_nas.h"
#include "lte_nas_msg_parse.h"
#include "emm_service_request_lib.h"
#include "emm_esm_handler.h"
#include "err.h"
#include "naslog_v.h"
#include "event.h"
#include "event_defs.h"
#include "emm_utility.h"
#include "emm_connection_handler.h"
#include "msgr_nas.h"
#include "emm_security.h"
#include "emm_rrc_if.h"
#include "mm_multimode_common.h"
#include "emm_rrc_handler.h"
#include "ds3gpp_api.h"
#include "emm_mem.h"
#include "stringl.h"
#include "gmm_int_v.h"
#include "emm_csfb_handler.h"


void emm_esm_release_indication
(
  lte_emm_connection_cause_type  release_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr
)
{
  log_lte_nas_emm_outgoing_msg_event_type event;
  esm_sig_connection_released_ind_T sig_conn_rel_ind;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  sig_conn_rel_ind.emm_cause.local_cause = NAS_SIG_CONN_RELEASED; 
  sig_conn_rel_ind.emm_cause.valid = TRUE;
  msgr_init_hdr(&sig_conn_rel_ind.msg_hdr,MSGR_NAS_EMM,NAS_ESM_SIG_CONNECTION_RELEASED_IND); 

  if(emm_msgr_send( (msgr_hdr_s *)&sig_conn_rel_ind.msg_hdr, sizeof(sig_conn_rel_ind) ) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= emm_msgr_send failed!", 0,0,0 );
  } 
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent NAS_ESM_SIG_CONNECTION_RELEASED_IND");
    event.emm_out_msg = (byte)NAS_ESM_SIG_CONNECTION_RELEASED_IND;
    event_report_payload(EVENT_LTE_ESM_OUTGOING_MSG,sizeof(event), (void*)&event);
  }

} /* end of emm_esm_release_indication() */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

boolean emm_attach_complete_failed
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
  boolean    plmn_blocked_due_to_domain_sel = FALSE;

  ASSERT(pended_message_ptr != (emm_pended_msg_list_type *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  emm_ctrl_data_ptr->reg_complete_ack_pending = EMM_PENDING_REG_ACK_NONE;

  emm_send_diag_event(EVENT_NAS_ATTACH, (nas_event_end_result)PROC_END, (nas_event_end_reason)PROC_END_LOWER_LAYER_FAILURE, (lte_nas_emm_cause_type)LTE_NAS_CAUSE_NONE);

   /*If blocking of PLMN was kept pending due to attach complete ack then block the PLMN*/
  if(emm_ctrl_data_ptr->is_plmn_block_req_pending == TRUE)
  {
    emm_process_block_plmn_req(&emm_ctrl_data_ptr->pending_block_plmn_req, emm_ctrl_data_ptr);
    plmn_blocked_due_to_domain_sel = TRUE;
  }

  if((EMM_GET_STATE() == EMM_REGISTERED) ||
     (EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED) ||
     (EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED))
  {  
    switch (cause)
    {
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_RLF:    
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
      case CONN_REL_LTE_RRC_CONN_REL_NORMAL:
      case CONN_REL_LTE_RRC_CONN_REL_RLF:
      case CONN_REL_LTE_RRC_CONN_REL_CRE_FAILURE:
      case CONN_REL_LTE_RRC_CONN_REL_OOS_DURING_CRE:
      case CONN_REL_LTE_RRC_CONN_REL_ABORTED:
      case CONN_REL_LTE_RRC_CONN_REL_SIB_READ_ERROR:
      case CONN_REL_LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS:
      case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
      case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TAI_CHANGED:
        MSG_ERROR_DS_0(MM_SUB, "=EMM= Attach Complete failed to be transmitted to the NW");

        /*Start reattach only if UE is in service, LTE is active and 
               PLMN is not blocked due to domain selection*/
        if((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                      SYS_SRV_STATUS_NO_SRV) ||
           (emm_ctrl_data_ptr->rrc_active == FALSE) ||
           (plmn_blocked_due_to_domain_sel == TRUE))
        {
          MSG_HIGH_DS_2(MM_SUB, "=EMM= Reattach can not be started - Srv %d, rrc active %d",
                        emm_ctrl_data_ptr->plmn_service_state->service_status,
                        emm_ctrl_data_ptr->rrc_active);
          emm_build_and_send_detach_ind(EMM_DETACHED);
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
          if(!is_lte_mode())
          {
            gmm_set_state(GMM_DEREGISTERED);
          }
          /* Stop possible T3411 and T3402 for CS failure */
          mm_stop_timer(TIMER_T3411);
          mm_stop_timer(TIMER_T3402);
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Restarting attach procedure");
#ifdef FEATURE_LTE_REL9
          if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
          {
            /* If UE is performing EMC attach, notify upper layers and wait for 
               EMC PDN REQ to restart EMC attach. */
            emm_build_and_send_detach_ind(EMM_ATTACH_STARTED);
          }
          else
#endif
          {
            /* For non EMC attach, init re-attach. */
            // Reset the service domain before starting attach again.
            emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
            emm_init_attach_procedure(EMM_INITIAL_UPDATE,emm_ctrl_data_ptr);
          }
          if(EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE)
          {
            /* If reattach is started on existing connection, NW may just 
               resend attach accept without configuring radio bearers again. 
               In this case, EMM needs to resend the bearer update ind to 
               ESM to have DRB setup in upper layers. */
            emm_ctrl_data_ptr->sync_drb_status = TRUE;
          }
        }
        break;
      case EMM_SRV_REQ_FAILURE_NO_THROTTLE:
      case EMM_SRV_REQ_FAILURE_THROTTLE:
        MSG_ERROR_DS_1(MM_SUB, "=EMM= Unexpected ATTACH COMPLETE Tx failure - %d", cause);
        break;
      default :
        MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM connection cause",0,0,0) ;
        break ;
    }
  }

  pended_message_ptr->message_sent_indicator = TRUE ;

  return FALSE ;
} /* end of emm_attach_complete_failed() */

#ifdef TEST_FRAMEWORK
 #error code not present
#endif // TEST_FRAMEWORK

/*=========================================================================== 
 
FUNCTION  EMM_ESM_DATA_REQ_FAILED

DESCRIPTION
  This function sends data request failure indication to ESM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
boolean emm_esm_data_req_failed
(
  lte_emm_connection_cause_type    cause,
  dword                            trans_id,
  emm_failure_type                 emm_failure_cause,
  lte_nas_tai_lst1_type            tai,
  emm_pended_msg_list_type        *message_ptr,
  emm_ctrl_data_type              *emm_ctrl_data_ptr
)
{
  lte_nas_local_cause_enum_T  local_cause = LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE;

  ASSERT(message_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 

  switch(cause)
  {
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN:
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO:
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL:
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_RLF:
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
      if(EMM_GET_STATE() == EMM_DEREGISTERED)
      {
        emm_build_and_send_failure_ind(local_cause, 
                                       LTE_NAS_NO_FAILURE,
                                       trans_id,
                                       emm_failure_cause);
        message_ptr->message_sent_indicator = TRUE; 
      }
      else
      {   
        if(((cause != TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO) || 
           ((emm_search_tai_list(&tai,emm_ctrl_data_ptr->emm_tai_list_ptr) == TRUE) &&
            (EMM_GET_STATE() != EMM_TRACKING_AREA_UPDATING_INITIATED)))
            &&(EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE))
        {
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Retx msg 0x%x", trans_id);
          emm_send_rrc_data_request(&message_ptr->nas_out_message, 
                                    message_ptr->sequence_number,
                                    emm_ctrl_data_ptr);
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "=EMM= Msg 0x%x to be retxed after TAU", trans_id);
        }
        message_ptr->message_sent_indicator = FALSE;
      }
      break;
    default:
      if(((cause == CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED) &&
          ((emm_ctrl_data_ptr->lte_connection_control & 0x01) == FALSE)) 
         ||
         (cause == EMM_SRV_REQ_FAILURE_THROTTLE))
      {
        local_cause = LTE_NAS_SERVICE_REQ_FAILED;
      }
      else if(cause == CONN_CNF_LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE)
      {
        local_cause = LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE;
      }
      else if((emm_failure_cause.cause_type == LTE_NAS_IRAT_CONN_REL) &&
              (mm_timer_status[TIMER_T3440] != TIMER_STOPPED))
      {
        local_cause = ESM_REJ_DUE_TO_CONNECTED_STATE;
      }

      emm_build_and_send_failure_ind(local_cause, 
                                     LTE_NAS_NO_FAILURE,
                                     trans_id,
                                     emm_failure_cause);
      message_ptr->message_sent_indicator = TRUE;
      break;
  }

  return FALSE ;

} /* end of emm_esm_data_req_failed() */

/*=========================================================================== 
 
FUNCTION  EMM_SRV_REQ_DS_ALLOWANCE_CHK 
 
DESCRIPTION
  This function calls DS API with current PLMN info to see if Service Request
  should be triggered

DEPENDENCIES
  None

RETURN VALUE
  TRUE: SERVICE REQUEST is allowed
  FALSE: SERVICE REQUEST is disallowed

SIDE EFFECTS
  None
 
===========================================================================*/
boolean emm_srv_req_ds_allowance_chk
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  boolean             srv_req_allowed = FALSE;
  sys_plmn_id_s_type  sys_plmn;

  ASSERT(emm_ctrl_data_ptr != NULL);
  if(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL)
  {
    sys_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
         emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.selected_plmn);

    srv_req_allowed = ds3gpp_per_subs_is_service_req_allowed_on_plmn(sys_plmn, (sys_modem_as_id_e_type)mm_sub_id);

    MSG_HIGH_DS_1(MM_SUB, "=EMM= SRV REQ DS allowance check result - %d", srv_req_allowed);
  }

  return srv_req_allowed;

} /* emm_srv_req_ds_allowance_chk */

/*=========================================================================== 
 
FUNCTION  EMM_SYNC_DRB_STATUS 
 
DESCRIPTION
  This function sends last received RRC bearer ind to ESM after adding all 
  bearers in active list to added list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_sync_drb_status
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  lte_rrc_active_eps_bearer_update_ind_s  bearer_ind;

  ASSERT(emm_ctrl_data_ptr != NULL);

  /* Reset flag */
  emm_ctrl_data_ptr->sync_drb_status = FALSE;

  /* Create a local copy to modify */
  bearer_ind = emm_ctrl_data_ptr->last_rrc_bearer_ind;

  /* Add all bearers in active list to added list */
  memscpy((void *)&bearer_ind.added_eps_bearers[0],
          sizeof(bearer_ind.added_eps_bearers),  
          (void *)&bearer_ind.active_eps_bearers[0],
          sizeof(bearer_ind.added_eps_bearers));

  emm_process_rrc_eps_bearer_update(&bearer_ind, emm_ctrl_data_ptr);

}/* emm_sync_drb_status */

/*===========================================================================
FUNCTION    EMM_PROCESS_ESM_DATA_REQ

DESCRIPTION
  This function processes ESM DATA REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
void emm_process_esm_data_req
(
  emm_data_req_T     *emm_data_request,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{  
  emm_state_type     emm_state    = EMM_INVALID_STATE;
  emm_substate_type  emm_substate = EMM_UNKNOWN_STATE;
  emm_failure_type    emm_failure_cause;
  lte_rrc_est_cause_e  conn_est_cause = LTE_RRC_EST_CAUSE_MO_DATA;
#ifdef FEATURE_LTE_REL9
  boolean            emc_pdn_conn_req = FALSE;
#endif
  emm_csfb_rsp                     csfb_rsp;
  ASSERT(emm_ctrl_data_ptr != NULL); 
  ASSERT(emm_data_request  != NULL);

  /*initialise it*/
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));
  memset((void *)&csfb_rsp,0,sizeof(emm_csfb_rsp));

  if((emm_ctrl_data_ptr->out_msg_ptr == NULL) ||
     (emm_ctrl_data_ptr->rrc_active == FALSE) ||
     (emm_ctrl_data_ptr->reg_req_pending_ptr == NULL))
  {
    MSG_HIGH_DS_3(MM_SUB, "=EMM= Can not process ESM DATA REQ - Dyn mem 0x%x, rrc active %d, reg_req_pending_ptr 0x%x",
                  emm_ctrl_data_ptr->out_msg_ptr,
                  emm_ctrl_data_ptr->rrc_active,
                  emm_ctrl_data_ptr->reg_req_pending_ptr);
    emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
    emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE, 
                                   LTE_NAS_NO_FAILURE,
                                   emm_data_request->emm_transaction_id,
                                   emm_failure_cause);
    return;
  }

  if(emm_find_pended_message(emm_data_request->emm_transaction_id, 
                             emm_ctrl_data_ptr) != NULL)
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= ESM DATA REQ w/ ID 0x%x existing",
                  emm_data_request->emm_transaction_id);
    return;
  }

  emm_state    = EMM_GET_STATE() ;
  emm_substate = EMM_GET_SUBSTATE() ;

#ifdef FEATURE_LTE_REL9
/* Reject further PDN Connectivity requests for a normal PDN when deactivation of
   non-emergency bearers is in progress due to T3418/T3420 expiry. 
   Spec referenc: 5.4.2.7 in TS 24.301 */
  if((emm_ctrl_data_ptr->non_emc_bearer_deact_in_progress == TRUE)
     && (emm_data_request->esm_out_msg.nas_hdr.msg_id == PDN_CONNECTIVITY_REQUEST)
     && (emm_data_request->esm_out_msg.pdn_connectivity_req.req != ESM_EMERGENCY))
  {
    MSG_HIGH_DS_0(MM_SUB,"=EMM= Rejecting non emc PDN connection request as deact of non-emc bearers is in progress");
    emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
          
    emm_build_and_send_failure_ind(LTE_NAS_EMM_FAILURE_NO_RETRY, 
                                   LTE_NAS_NO_FAILURE, 
                                   emm_data_request->emm_transaction_id,
                                   emm_failure_cause);
    return;
  }

  if((emm_data_request->esm_out_msg.nas_hdr.msg_id == PDN_CONNECTIVITY_REQUEST)
     &&
     (emm_data_request->esm_out_msg.pdn_connectivity_req.req == ESM_EMERGENCY))
  {
    emc_pdn_conn_req = TRUE;
    conn_est_cause = LTE_RRC_EST_CAUSE_EMERGENCY;
  }
#endif

  /* Init the global before use */
  memset((void *)emm_ctrl_data_ptr->out_msg_ptr, 
         0x0, 
         sizeof(lte_nas_outgoing_msg_type));

  switch (emm_state)
  {
    case EMM_REGISTERED_INITIATED:
      if (emm_substate == (emm_substate_type)EMM_WAITING_FOR_ESM_RESPONSE)
      {   
        if(emm_data_request->esm_out_msg.nas_hdr.msg_id == 
                                    ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT)
        {
          /* Detach to be performed. No need to sync up drb status anymore. */
          emm_ctrl_data_ptr->sync_drb_status = FALSE;

          MSG_HIGH_DS_0(MM_SUB, "=EMM= Act default BC rejected on attach. Initiating Detach");
          emm_start_mo_detach(emm_get_mo_detach_type(emm_ctrl_data_ptr), 
                              FALSE, 
                              NULL, 
                              emm_detach_failed_routine, 
                              emm_ctrl_data_ptr);
        }
        else if(emm_data_request->esm_out_msg.nas_hdr.msg_id != 
                                    ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT)
        {
          emm_failure_cause.cause_type      = LTE_NAS_IRAT_INVALID_STATE;
          
          emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE, 
                                         LTE_NAS_NO_FAILURE, 
                                         emm_data_request->emm_transaction_id,
                                         emm_failure_cause);
        }
        else
        {
          lte_nas_emm_attach_complete_type  *attach_complete_ptr = NULL;
          attach_complete_ptr = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.attach_complete;

          attach_complete_ptr->lte_nas_hdr.pd = 
                                              EPS_MOBILITY_MANAGEMENT_MESSAGES;
          attach_complete_ptr->lte_nas_hdr.msg_id  = ATTACH_COMPLETE ;
          if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_UMTS_RADIO_ACCESS) || 
              mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_GSM_RADIO_ACCESS)  || 
              mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_TDS_RADIO_ACCESS))
          {
            emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent = NAS_SEND_DRX_ON_LTE_RAT;
          }
          attach_complete_ptr->security_hdr = PLAIN_NAS_MSG ;
   
          attach_complete_ptr->esm_msg_container = emm_data_request->esm_out_msg;

          MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending ATTACH_COMPLETE");

          EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);   
          /* MNK: Move MM states*/
          mm_update_ps_registration_success_status();

          if((emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == 
                                                       SYS_SRV_DOMAIN_CS_PS) &&
               emm_ctrl_data_ptr->combined_attach_cs_rej_cause != LTE_NAS_CAUSE_NONE)
          {
            /* Combined attach successful for PS only. 
               Process CS reject cause. */
            emm_process_cs_reject_cause(
                               emm_ctrl_data_ptr->combined_attach_cs_rej_cause,
                               emm_ctrl_data_ptr);
            /* Reset the CS reject cause */
            emm_ctrl_data_ptr->combined_attach_cs_rej_cause = 
                                                            LTE_NAS_CAUSE_NONE;
           }

           emm_ctrl_data_ptr->reg_complete_ack_pending = EMM_PENDING_REG_ACK_ATTACH_COMPLETE;

           reg_service_report(MMR_CAUSE_NOT_SPECIFIED, 
                               FALSE, 
                               emm_ctrl_data_ptr);
          

          if(emm_ctrl_data_ptr->sync_drb_status == TRUE)
          {
            /* NW did not reconfigure radio bearers with attach accept. Need to
               sync up drb status.  */
            emm_sync_drb_status(emm_ctrl_data_ptr);
          }

          if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr,
                                        emm_data_request->emm_transaction_id, 
                                        MSGR_NAS_EMM, 
                                        emm_attach_complete_succeeded, 
                                        emm_attach_complete_failed, 
                                        emm_ctrl_data_ptr))
          {
            ASSERT(TRUE) ;  /* Can not happen */
          }


        }
      }
      else
      {
        if(emm_data_request->esm_out_msg.nas_hdr.msg_id == 
                                                      ESM_INFORMATION_RESPONSE)
        {
          if(EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE)
          {
             emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg = emm_data_request->esm_out_msg;

            /* Note: Set Tx failure CB to NULL since ESM does not need to know  
               the transmission failure of ESM_INFORMATION_RESPONSE message */
            (void)emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                   emm_data_request->emm_transaction_id, 
                                   MSGR_NAS_EMM, 
                                   NULL, 
                                   NULL, 
                                   emm_ctrl_data_ptr);
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent ESM_INFORMATION_RESPONSE ");
          }
          else
          {
            MSG_HIGH_DS_1(MM_SUB, "=EMM= Discarding ESM INFO RSP - Connection State %d",
                          EMM_GET_CONNECTION_STATE());
          }
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=EMM= Unexpected ESM msg rcved (%u) while waiting ESM INFO RSP",
                         emm_data_request->esm_out_msg.nas_hdr.msg_id);

          emm_failure_cause.cause_type      = LTE_NAS_IRAT_INVALID_STATE;
          
          emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE, 
                                         LTE_NAS_NO_FAILURE,
                                         emm_data_request->emm_transaction_id,
                                         emm_failure_cause);
        }
      }
      break ;
    case EMM_REGISTERED:
#ifdef FEATURE_SGLTE
      if (mm_timer_status[TIMER_DELAY_TAU] == TIMER_ACTIVE)
      {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Delay TAU timer running");
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
          emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE, 
                                     LTE_NAS_NO_FAILURE,
                                     emm_data_request->emm_transaction_id,
                                     emm_failure_cause);
      }
      else 
#endif
      if((emm_ctrl_data_ptr->rrc_active == TRUE)
         &&
         ((emm_substate == EMM_REGISTERED_NORMAL_SERVICE) ||
          (emm_substate == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM))
         &&
         (((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                            SYS_SRV_STATUS_SRV)
#ifdef FEATURE_LTE_REL10
         && ((mm_timer_status[TIMER_T3346] == TIMER_STOPPED)||
             (emm_check_high_priority(emm_ctrl_data_ptr)) ||
             (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_PENDING) ||
             (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_EXIST))
#endif
         )
#ifdef FEATURE_LTE_REL9
          ||
          ((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                     SYS_SRV_STATUS_LIMITED) &&
           (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY))
#endif
         )
         &&
         (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED) 
         &&
         ((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_AVAILABLE) ||
          (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                      SYS_SIM_STATE_CS_INVALID)
#ifdef FEATURE_LTE_REL9
          ||
          (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
#endif
         )
         && (emm_reg_container == NULL)
         && ((EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE) ||
             (emm_mmtel_status(emm_ctrl_data_ptr) == TRUE) ||
              (emm_wms_sms_status(emm_ctrl_data_ptr) == TRUE)||
             (emm_check_mplmn_in_progress(emm_ctrl_data_ptr) == FALSE)))
      {
        emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg = emm_data_request->esm_out_msg;

        if (EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE) 
        {
          if(mm_timer_status[TIMER_T3440] != TIMER_STOPPED)
          {
            (void)emm_cache_new_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                        emm_data_request->emm_transaction_id, 
                                        MSGR_NAS_EMM, 
                                        NULL, 
                                        emm_esm_data_req_failed, 
                                        emm_ctrl_data_ptr );
            if((emc_pdn_conn_req == TRUE) &&
               (emm_ctrl_data_ptr->reg_complete_ack_pending == EMM_PENDING_REG_ACK_NONE))
            {
              /* T3440 active, abort conection to handle emeregency PDN CONN REQ */
              emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,
                                       LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
            }
            
          }
          else
          {
            /* Signaling connection exists. Send the message. */          
            (void)emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                   emm_data_request->emm_transaction_id, 
                                   MSGR_NAS_EMM, 
                                   NULL, 
                                   emm_esm_data_req_failed, 
                                   emm_ctrl_data_ptr ) ;
          }
        }
        else
        {
          if((emm_srv_req_ds_allowance_chk(emm_ctrl_data_ptr) == TRUE)
#ifdef FEATURE_LTE_REL9 
             ||
             (emc_pdn_conn_req == TRUE)
#endif
            )
          {
            (void)emm_cache_new_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                        emm_data_request->emm_transaction_id, 
                                        MSGR_NAS_EMM, 
                                        NULL, 
                                        emm_esm_data_req_failed, 
                                        emm_ctrl_data_ptr );

            /* Signaling connection is not existing */
            /* Start service request procedure with the cause of MO_DATA */
            emm_ctrl_data_ptr->esm_srv_req_trans_id = 
                                                EMM_INVALID_ESM_SRV_REQ_TRANS_ID;
            if(emm_check_esr_for_ps(emm_ctrl_data_ptr) == TRUE)
            {
              /* Start extended service request procedure for PS */
              emm_start_nas_extended_service_request_procedure(conn_est_cause,LTE_PACKET_SERVICES_VIA_S1,
                                                         csfb_rsp, NAS_EMM_CSFB_NONE);
            }
            else
            {
              emm_start_nas_service_request_procedure(emm_ctrl_data_ptr, conn_est_cause);
            }
          }
          else
          {
            emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
            emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_DS_DISALLOW, 
                                           LTE_NAS_NO_FAILURE,
                                           emm_data_request->emm_transaction_id,
                                           emm_failure_cause);
          }
        }
      }
#ifdef FEATURE_LTE_REL9
      else if((emc_pdn_conn_req == TRUE) && 
              (emm_ctrl_data_ptr->plmn_service_state->service_status != 
                                                      SYS_SRV_STATUS_NO_SRV))
      {
        if((emm_substate == EMM_REGISTERED_UPDATE_NEEDED) ||
           (emm_substate == EMM_REGISTERED_ATTEMPTING_TO_UPDATE))
        {
          MSG_HIGH_DS_0(MM_SUB,"=EMM= EMC PDN REQ - Init TAU");
          emm_ctrl_data_ptr->cell_barred_tau_pending = FALSE;
          /* TAU to be started on a new connection with EMC conn est cause. 
             Abort current connection if applicable. */
          emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE, 
                                   LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
          /* Start TAU procedure with ACTIVE flag */
          emm_start_tau_procedure(EMM_INITIAL_UPDATE,
                                  emm_get_eps_update_type(emm_ctrl_data_ptr),
                                  TRUE,
                                  emm_ctrl_data_ptr);
          /* Cache the EMC PDN REQ to be processed after TAU completion */
          emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg = emm_data_request->esm_out_msg;
          (void)emm_cache_new_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                      emm_data_request->emm_transaction_id, 
                                      MSGR_NAS_EMM, 
                                      NULL, 
                                      emm_esm_data_req_failed, 
                                      emm_ctrl_data_ptr);
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB,"=EMM= EMC PDN REQ - Locally detach and EMC attach");
          /* EMC attach to be started on a new connection with EMC conn est cause. 
             Abort current connection if applicable. */
          emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE, 
                                   LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
          /* locally detach */
          emm_build_and_send_detach_ind(EMM_ATTACH_STARTED);
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
          emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_NO_SRV;
          emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED);
        
          /* Cache attach PDN */
          *(emm_ctrl_data_ptr->pdn_conn_req_ptr) = 
                                   emm_data_request->esm_out_msg.pdn_connectivity_req;
            
          /* Save the transaction ID */
          emm_ctrl_data_ptr->pdn_conn_req_trans_id = 
                                        emm_data_request->emm_transaction_id;
          emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
        }
      }
#endif
      else
      {
        lte_nas_local_cause_enum_T local_cause;
        if (((emm_ctrl_data_ptr->rrc_active) == FALSE) 
#ifdef FEATURE_LTE_REL9
        || (emc_pdn_conn_req == TRUE)
#endif  
        )
        {
          local_cause = LTE_NAS_EMM_FAILURE_NO_RETRY;
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
          emm_build_and_send_failure_ind(local_cause, 
                                     LTE_NAS_NO_FAILURE,
                                     emm_data_request->emm_transaction_id,
                                     emm_failure_cause);
        }
        else if(emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                      SYS_SRV_STATUS_NO_SRV)
        {
          local_cause = LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE;
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
          emm_build_and_send_failure_ind(local_cause, 
                                     LTE_NAS_NO_FAILURE,
                                     emm_data_request->emm_transaction_id,
                                     emm_failure_cause);
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Skip sending failure indication,wait for ESM timer expriry ");
        }
      }
      break ;
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
    case EMM_SERVICE_REQUEST_INITIATED:
      if((emm_ctrl_data_ptr->aborting_conn == TRUE) && 
         (EMM_GET_CONNECTION_STATE() == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE))
      {
        emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
        emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE, 
                                       LTE_NAS_NO_FAILURE,
                                       emm_data_request->emm_transaction_id,
                                       emm_failure_cause);
      }
      /* This is to cover for scenario where user as initiated an MO call and then
         disconnected it before ESR is complete. We stay in service request 
         initiated state so that we can use the network response. If any new
         data request comes then send it to the network.
      */
      else if((emm_state == EMM_SERVICE_REQUEST_INITIATED) &&
              (EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE)&&
              (mm_timer_status[TIMER_T3417] == TIMER_STOPPED) &&
              (mm_timer_status[TIMER_T3417_EXT] == TIMER_STOPPED) &&
              (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE) &&
              (emm_ctrl_data_ptr->nas_secure_exchange == TRUE))
      {
         emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg = emm_data_request->esm_out_msg;
         /* Signaling connection exists. Send the message. */          
        (void)emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                               emm_data_request->emm_transaction_id, 
                               MSGR_NAS_EMM, 
                               NULL, 
                               emm_esm_data_req_failed, 
                               emm_ctrl_data_ptr ) ;
      }
      /* Sending failure cause we can revisit if it is not ok   */
      else if ((emm_ctrl_data_ptr->reg_req_pended_indication == TRUE) &&
               (emm_ctrl_data_ptr->aborting_conn == TRUE) &&
               (EMM_GET_CONNECTION_STATE() == EMM_RELEASING_RRC_CONNECTION_STATE ))
      {
        emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
        emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE, 
                                       LTE_NAS_NO_FAILURE,
                                       emm_data_request->emm_transaction_id,
                                       emm_failure_cause);
      }
      else
      {
        /* Signaling connection will be set up. Cache the message for now. */
        emm_ctrl_data_ptr->out_msg_ptr->esm_outgoing_msg = emm_data_request->esm_out_msg;

        (void)emm_cache_new_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                    emm_data_request->emm_transaction_id, 
                                    MSGR_NAS_EMM, 
                                    NULL, 
                                    emm_esm_data_req_failed, 
                                    emm_ctrl_data_ptr );
      }
      break ;
    case EMM_DEREGISTERED:
      MSG_HIGH_DS_3(MM_SUB, "=EMM= ESM DATA REQ - Msg %d, DEREG %d state, REG REQ pending %d",
                emm_data_request->esm_out_msg.nas_hdr.msg_id, 
                emm_substate,
                emm_ctrl_data_ptr->reg_req_pended_indication);

      /* Check if the data req is PDN CONNECTIVITY REQ message AND EMM is 
         waiting for it. If so, cache it and start initial type of attach 
         procedure */
      if((emm_substate == EMM_DEGEGISTERED_WAITING_PDN_CONN_REQ ||
          (emm_substate == EMM_DEREGISTERED_ATTACH_NEEDED && 
           emm_ctrl_data_ptr->reg_req_pended_indication == TRUE))
         &&
         (emm_data_request->esm_out_msg.nas_hdr.msg_id == 
                                                     PDN_CONNECTIVITY_REQUEST))
      {
        if(emm_data_request->esm_out_msg.pdn_connectivity_req.seq_num == 
                                         emm_ctrl_data_ptr->attach_pdn_seq_num)
        {
          /* Cache the PDN_CONNECTIVITY_REQ message */
          *(emm_ctrl_data_ptr->pdn_conn_req_ptr) = 
                          emm_data_request->esm_out_msg.pdn_connectivity_req;
  
          /* Save the transaction ID */
          emm_ctrl_data_ptr->pdn_conn_req_trans_id = 
                                          emm_data_request->emm_transaction_id;

          /* Start attach right away if substate is waiting pdn conn req and 
             there is no cached REG REQ to be processed. 
             Otherwise attach will be started when UE camps. */
          if((emm_substate == EMM_DEGEGISTERED_WAITING_PDN_CONN_REQ) &&
             (emm_reg_container == NULL))
          {
            if((emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter > 0)||
               (emm_ctrl_data_ptr->apn_switch == TRUE))
            {
              emm_ctrl_data_ptr->apn_switch = FALSE;
              /* Start recovery type attach procedure */
              emm_start_attach_procedure(EMM_UPDATE_RECOVERY,emm_ctrl_data_ptr);
            }
            else
            {
              /* Start initial type attach procedure */
              emm_start_attach_procedure(EMM_INITIAL_UPDATE,emm_ctrl_data_ptr);
            }
          }
        }
        else
        {
          MSG_HIGH_DS_2(MM_SUB, "=EMM= Attach PDN sequence number mismatching %d - %d",
                        emm_data_request->esm_out_msg.pdn_connectivity_req.seq_num,
                        emm_ctrl_data_ptr->attach_pdn_seq_num);
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
          emm_build_and_send_failure_ind(EMM_DETACHED, 
                                         LTE_NAS_NO_FAILURE,
                                         emm_data_request->emm_transaction_id,
                                         emm_failure_cause);
        }
      }
#ifdef FEATURE_LTE_REL9
      else if((emc_pdn_conn_req == TRUE) &&
              (emm_ctrl_data_ptr->plmn_service_state->service_status != 
                                                      SYS_SRV_STATUS_NO_SRV))
      {
        MSG_HIGH_DS_0(MM_SUB,"=EMM= EMC PDN REQ - EMC attach");
        /* EMC attach to be started on a new connection with EMC conn est cause. 
             Abort current connection if applicable. */
        emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE, 
                                 LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
        if(emm_ctrl_data_ptr->pdn_conn_req_trans_id != 
                                             EMM_INVALID_PDN_CONN_REQ_TRANS_ID)
        {
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
          emm_build_and_send_failure_ind(
            EMM_DETACHED, 
            LTE_NAS_NO_FAILURE,
            emm_ctrl_data_ptr->pdn_conn_req_trans_id,
            emm_failure_cause);
        }

        /* Cache attach PDN */
        *(emm_ctrl_data_ptr->pdn_conn_req_ptr) = 
                              emm_data_request->esm_out_msg.pdn_connectivity_req;
              
        /* Save the transaction ID */
        emm_ctrl_data_ptr->pdn_conn_req_trans_id = 
                                          emm_data_request->emm_transaction_id;
        emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
      }
#endif
      else
      {
        emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
        emm_build_and_send_failure_ind(EMM_DETACHED, 
                                       LTE_NAS_NO_FAILURE,
                                       emm_data_request->emm_transaction_id,
                                       emm_failure_cause);
      }
      break;
    case EMM_NULL:
    case EMM_DEREGISTERED_INITIATED:
      emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
      emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE, 
                                     LTE_NAS_NO_FAILURE,
                                     emm_data_request->emm_transaction_id,
                                     emm_failure_cause);
      break ;
    default:
      MSG_FATAL_DS(MM_SUB,"=EMM= Illigal EMM state",0,0,0) ;
      break ;

  }
} /* end of emm_process_esm_data_req() */

/*===========================================================================
FUNCTION    emm_process_esm_eps_bearer_status_cmd

DESCRIPTION
  This function processes esm_eps_bearer_status_cmd
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_esm_eps_bearer_status_cmd
(
  emm_eps_bearer_status_cmd_T     *emm_eps_bearer_status_cmd_ptr, 
  emm_ctrl_data_type              *emm_ctrl_data_ptr
)
{
  emm_registered_substate_type emm_substate ;

  ASSERT(emm_eps_bearer_status_cmd_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Received EPS_BEARER_STATUS_CMD from ESM in EMM state %d",
                emm_ctrl_data_ptr->emm_state);

  switch(emm_ctrl_data_ptr->emm_state)
  {
    case EMM_NULL:
    case EMM_DEREGISTERED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Discard unexpected EPS_BEARER_STATUS_CMD");
      break;
    case EMM_REGISTERED:
      emm_ctrl_data_ptr->eps_bearer_context_status = 
                                emm_eps_bearer_status_cmd_ptr->context_status;
      emm_substate = EMM_GET_SUBSTATE();

      if(emm_substate  == EMM_REGISTERED_WAITING_FOR_ESM_ISR_STATUS)
      {
        /* If EMM substate is waiting for ist status then this EPS_BEARER_STATUS_CMD
        ** is the response to the EMM request. If ISR status is returned as FALSE
        ** then a bearer context activated in GW after ISR exists so set the TIN to PTMSI.
        */
        if((emm_eps_bearer_status_cmd_ptr->isr_status.valid) && 
         (emm_eps_bearer_status_cmd_ptr->isr_status.isr_status == FALSE))
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= ISR deactivated TIN set to PTMSI");
          emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);
        }
        EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);
        emm_registered_process_rrc_service_ind(emm_ctrl_data_ptr->last_rrc_service_ind_ptr,emm_ctrl_data_ptr) ;
#ifdef FEATURE_DUAL_SIM
        if((emm_ctrl_data_ptr->tau_trm_priority == TRUE) &&
            ((EMM_GET_STATE() != EMM_TRACKING_AREA_UPDATING_INITIATED) &&
             ((EMM_GET_STATE() != EMM_REGISTERED) ||
              (emm_ctrl_data_ptr->reg_complete_ack_pending != EMM_PENDING_REG_ACK_TAU_COMPLETE))))
        {
          emm_set_tau_trm_priority(FALSE);
        }
#endif
      }
      else 
      {
        /* If EMM substate is not waiting for ISR status then this EPS_BEARER_STATUS_CMD
        ** might be due to modification of PDP in ESM. So set the TIN to GUTI so that we start
        ** RAU next time.
        */
        if((emm_eps_bearer_status_cmd_ptr->isr_status.valid) && 
           (emm_eps_bearer_status_cmd_ptr->isr_status.isr_status == FALSE))
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= ISR deactivated TIN set to GUTI");
          emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
        }
        // Dont set the eps bearer updated flag if UE is in GW and ISR is ON.
        if(((emm_substate == EMM_REGISTERED_NO_CELL_AVAILABLE) ||
            (emm_substate == EMM_REGISTERED_PLMN_SEARCH)) &&
            (mm_is_ue_camped_on_gw() == FALSE ||
             emm_ctrl_data_ptr->TIN != NAS_MM_TIN_RAT_RELATED_TMSI))
        {
          /* Set eps_bearer_context_status_updated to TRUE only for this substate
             for now */
          emm_ctrl_data_ptr->eps_bearer_context_status_updated = TRUE;
        }
        else if((emm_substate == EMM_REGISTERED_UPDATE_NEEDED) 
                 &&
                ((emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_INVALID) ||
                 (emm_ctrl_data_ptr->tau_cause == TAU_CAUSE_LOW_LAYER_FAILURE)))
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= BC updated in REGED_UPDATE_NEEDED - TAU needed later");
          emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
        }
      }
      break;
    case EMM_REGISTERED_INITIATED:
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
    case EMM_SERVICE_REQUEST_INITIATED:
    case EMM_DEREGISTERED_INITIATED:
      /* Do not set eps_bearer_context_status_updated to TRUE for now */
      emm_ctrl_data_ptr->eps_bearer_context_status = 
                                emm_eps_bearer_status_cmd_ptr->context_status; 
      break;
    default:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= Invalid EMM state");
      break;
  }
} /* emm_process_esm_eps_bearer_status_cmd */

/*===========================================================================
FUNCTION    emm_process_esm_1xcsfb_esr_call_req

DESCRIPTION
               Triggers service_req for 1XCSFB
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_esm_1xcsfb_esr_call_req(emm_1xCSFB_esr_call_req_T* msg_ptr, emm_ctrl_data_type* emm_ctrl_data_ptr)
{
  lte_rrc_est_cause_e              service_req_cause; 
  emm_csfb_rsp                     csfb_response;
  reject_type_enum_T               cause_type;
  reject_cause_T cause_value = REJ_UNSPECIFIED;

  ASSERT(msg_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);

  csfb_response.valid = FALSE;

  switch(msg_ptr->csfb_service_type)
  {                                
    case LTE_MO_CSFB_1xCSFB:
    case LTE_MO_CSFB_S102_LESS_1XCSFB:
      service_req_cause = LTE_RRC_EST_CAUSE_MO_DATA;
      break;
    case LTE_MT_CSFB_1XCSFB:
      service_req_cause = LTE_RRC_EST_CAUSE_MT_ACCESS;
      csfb_response.csfb_rsp = CSFB_ACCEPT;
      csfb_response.valid = TRUE;
      break;
    case LTE_MO_CSFB_ECALL_1XCSFB_ECALL:
      service_req_cause = LTE_RRC_EST_CAUSE_EMERGENCY;
      break;
    default:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= unknown cause value received mapping serv_req to 'LTE_RRC_EST_CAUSE_MO_DATA'");
      service_req_cause = LTE_RRC_EST_CAUSE_MO_DATA;
      break;
  }
#ifdef FEATURE_1XSRLTE 
  emm_ctrl_data_ptr->partial_esr =msg_ptr->partial_esr;
#endif
  if(emm_ctrl_data_ptr->rrc_active == TRUE)
  {
    emm_start_nas_extended_service_request_procedure(service_req_cause,msg_ptr->csfb_service_type,csfb_response,NAS_EMM_1xCSFB);
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= L stack is not active and call is rejected");
    cause_type = AS_REJECT_CAUSE;
    cause_value = AS_REJ_LRRC_LOWER_LAYER_FAILURE;
    emm_build_and_send_esr_failure_ind(cause_type,cause_value,emm_ctrl_data_ptr->esm_srv_req_trans_id);
  }
}
/*===========================================================================
FUNCTION    emm_process_esm_1xcsfb_esr_call_abort_req

DESCRIPTION
       Autonomously clean up the pended ESR messages and reset the ESR type and STOP the TIMER
       It is a conscious decision to let the ESR run even after the call is borted by the upper lyaer/user. If UE is redirected it will go to GW.
       If ESR fails, it will not be retransmitted or failure is indicated to the upper layers 
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_esm_1xcsfb_esr_call_abort_req(emm_1xCSFB_esr_call_abort_req_T* msg_ptr,emm_ctrl_data_type* emm_ctrl_data_ptr)
{
  emm_1xCSFB_esr_call_abort_rsp_T      esr_abort_rsp;
  log_lte_nas_emm_outgoing_msg_event_type event;

  ASSERT(msg_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);
  if(emm_ctrl_data_ptr->last_rrc_service_ind_ptr == NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function mm_process_esm_1xcsfb_esr_call_abort_req");
    return;
  }

  mm_stop_timer(TIMER_T3417);    
  MSG_HIGH_DS_0(MM_SUB, "=EMM=ESR is aborted by CM. Stop the T3417 timer");


#ifdef FEATURE_1XSRLTE 
  mm_stop_timer(TIMER_SRLTE_ESR);    
  MSG_HIGH_DS_0(MM_SUB, "=EMM=ESR is aborted by CM. Stop the SRLTE_ESR_TIMER timer");
#endif

  memset(&esr_abort_rsp, 0x00, sizeof(emm_1xCSFB_esr_call_abort_rsp_T));

  msgr_init_hdr((msgr_hdr_s *)&esr_abort_rsp.msg_hdr, MSGR_NAS_EMM, NAS_ESM_1XCSFB_ESR_CALL_ABORT_RSP);

  //esr_abort_rsp.csfb_call_id

  esr_abort_rsp.trans_id = msg_ptr->trans_id;

  if(emm_msgr_send((msgr_hdr_s *)&esr_abort_rsp, sizeof(esr_abort_rsp)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= emm_msgr_send failed!", 0,0,0 );
  } 
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent NAS_ESM_1XCSFB_ESR_CALL_ABORT_RSP");
    event.emm_out_msg = (byte)NAS_ESM_1XCSFB_ESR_CALL_ABORT_RSP;
    event_report_payload(EVENT_LTE_ESM_OUTGOING_MSG,sizeof(event), (void*)&event);
  }

  /* Autonomously clean up the pended ESR messages and reset the ESR type and STOP the TIMER*/
  /* It is a conscious decision to let the ESR run even after the call is borted by the upper lyaer/user. If UE is redirected it will go to GW.
     If ESR fails, it will not be retransmitted or failure is indicated to the upper layers */

  emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_CSFB_NONE;

#ifdef FEATURE_1XSRLTE
  if(emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == TRUE)
  {
    /*Reset TRM to low priority after esr type reset to NONE in extended service req failure function*/
    emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
  }
#endif

  /* Reset service request cause and esm trans ID */
  emm_ctrl_data_ptr->service_req_cause = LTE_RRC_EST_CAUSE_MAX;
  
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

#ifdef FEATURE_LTE_REL9 
/*===========================================================================
FUNCTION  EMM_PROCESS_EMC_SRV_STATUS_CMD

DESCRIPTION
  This function processes EMC SRV STATUS CMD from ESM
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  emm_process_emc_srv_status_cmd
(
  emm_emc_srv_status_cmd_T  *cmd_ptr, 
  emm_ctrl_data_type        *emm_ctrl_data_ptr
)
{
  ASSERT(cmd_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL);

  if(emm_ctrl_data_ptr->reg_req_pending_ptr == NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function emm_process_emc_srv_status_cmd");
    return;
  }

  if(emm_ctrl_data_ptr->emc_srv_status == cmd_ptr->emc_srv_status)
  {
    return;
  }

  MSG_HIGH_DS_2(MM_SUB, "=EMM= Updating EMC SRV status from %d to %d",
                emm_ctrl_data_ptr->emc_srv_status,
                cmd_ptr->emc_srv_status);

  switch(cmd_ptr->emc_srv_status)
  {
    case LTE_NAS_EMC_SRV_NONE:
      if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_EXIST)
      {
        /* No more EMC BC - remove from the list of EPLMNs any PLMN present 
           in the list of FPLMNs and update RRC
           Note: No need to update REG as REG should have removed FPLMN 
                 already */
        emm_update_eq_plmn_list(emm_ctrl_data_ptr);
        emm_send_rrc_system_update(emm_ctrl_data_ptr);
      }
      break;
    case LTE_NAS_EMC_SRV_ONLY:
      if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_EXIST)
      {
        if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
        {
          /* Deactivate ISR */
          emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
        }

        /* Stop CS failure related T3411 or T3402 if running */
        if((emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED) &&
           (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == 
                                                       SYS_SRV_DOMAIN_CS_PS) &&
           (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                       SYS_SRV_DOMAIN_PS_ONLY))
        {
          mm_stop_timer(TIMER_T3411);
          mm_stop_timer(TIMER_T3402);
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
          mm_stop_timer(TIMER_EMM_RADIO_RETRY);
#endif 
        }

        emm_cache_registration_ctxt(emm_ctrl_data_ptr);

        if(emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                            SYS_SRV_STATUS_SRV)
        {
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
        }

        if(reg_service_report != (emm_reg_service_report_type)emm_send_reg_cnf)
        {
          /* Set emc srv status now to enable emc attached field in srv cnf/ind
             to REG */
          emm_ctrl_data_ptr->emc_srv_status = cmd_ptr->emc_srv_status;

          reg_service_report(MMR_CAUSE_NOT_SPECIFIED, 
                             FALSE, 
                             emm_ctrl_data_ptr);
        }
      }
      break;
    default:
      break;
  }

  emm_ctrl_data_ptr->emc_srv_status = cmd_ptr->emc_srv_status;

} /* emm_process_emc_srv_status_cmd */
#endif

/*===========================================================================
FUNCTION    emm_process_outgoing_esm_message

DESCRIPTION
  This function processes incoming esm messages
   
DEPENDENCIES
  None

RETURN VALUE
  TRUE

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_process_outgoing_esm_message
(
  void               *emm_message_router_cmd, // Changed the type to void*, to be compatible to function pointer.
  emm_ctrl_data_type *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
)
{
  emm_cmd_type  *emm_msgr_cmd;  // define esm_msgr_cmd type
  emm_csfb_rsp                     csfb_rsp;
  log_lte_nas_emm_incoming_event_type event;
  emm_failure_type  emm_failure_cause;

  lte_rrc_est_cause_e  conn_est_cause = LTE_RRC_EST_CAUSE_MO_DATA;
  
  emm_msgr_cmd = (emm_cmd_type*)emm_message_router_cmd; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  ASSERT(emm_msgr_cmd != (emm_cmd_type *)NULL) ; 

  /*initialise it*/
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));
  memset((void *)&csfb_rsp,0,sizeof(emm_csfb_rsp));
  switch (emm_msgr_cmd->emm_msgr_cmd.msg_hdr.id)
  {
    case NAS_EMM_SERVICE_REQ:
      /* Save the EMM_SERVICE_REQ transaction ID in EMM data base 
         Note: ESM can only send one EMM_SERVICE_REQ in one cycle */
      emm_ctrl_data_ptr->esm_srv_req_trans_id = 
                           emm_msgr_cmd->emm_msgr_cmd.emm_service_req.trans_id;

      if (EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED) 
      {
        if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB) || 
           (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB))
        {
            emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
            emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                           LTE_NAS_NO_FAILURE,
                                           emm_msgr_cmd->emm_msgr_cmd.emm_service_req.trans_id,
                                           emm_failure_cause);
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Ignore duplicate service request");
        }
      } 
      else 
      {
        if((emm_ctrl_data_ptr->rrc_active == TRUE) &&
           ((EMM_GET_STATE() == EMM_REGISTERED) && 
            ((EMM_GET_SUBSTATE() == EMM_REGISTERED_NORMAL_SERVICE) ||
             (EMM_GET_SUBSTATE() == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM)))
           &&
           (((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                         SYS_SRV_STATUS_SRV) 
#ifdef FEATURE_LTE_REL10
         && ((mm_timer_status[TIMER_T3346] == TIMER_STOPPED)||
             (emm_check_high_priority(emm_ctrl_data_ptr)) ||
             (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_EXIST))
#endif
           )
#ifdef FEATURE_LTE_REL9
            ||
            ((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                     SYS_SRV_STATUS_LIMITED) &&
             (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY))
#endif
           )
           &&
           (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED)
           &&
           ((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_AVAILABLE) ||
            (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                   SYS_SIM_STATE_CS_INVALID) 
#ifdef FEATURE_LTE_REL9
            ||
            (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
#endif
           )
           && (emm_reg_container == NULL)
         && ((emm_mmtel_status(emm_ctrl_data_ptr) == TRUE) ||
              (emm_wms_sms_status(emm_ctrl_data_ptr) == TRUE)||
             (emm_check_mplmn_in_progress(emm_ctrl_data_ptr) == FALSE)))
        {
         if(
#ifdef FEATURE_LTE_REL9
             (emm_msgr_cmd->emm_msgr_cmd.emm_service_req.emc_bc == TRUE) || 
#endif
             (emm_srv_req_ds_allowance_chk(emm_ctrl_data_ptr) == TRUE))
          {
            if(emm_ctrl_data_ptr->emm_connection_state != EMM_CONNECTED_STATE)
            {
#ifdef FEATURE_LTE_REL9
              if(emm_msgr_cmd->emm_msgr_cmd.emm_service_req.emc_bc == TRUE)
              {
                conn_est_cause = LTE_RRC_EST_CAUSE_EMERGENCY;
              }
#endif
              /* Save the EMM_SERVICE_REQ call type in EMM data base*/
              emm_ctrl_data_ptr->call_type = 
                         emm_msgr_cmd->emm_msgr_cmd.emm_service_req.call_type;
              MSG_HIGH_DS_1(MM_SUB, "=EMM= volte_opt call_type = %d",
                        emm_ctrl_data_ptr->call_type);

              if(emm_check_esr_for_ps(emm_ctrl_data_ptr)== TRUE)
              {
                /* Start extended service request procedure for PS */
                emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MO_DATA,LTE_PACKET_SERVICES_VIA_S1,
                                                         csfb_rsp, NAS_EMM_CSFB_NONE);
              }
              else
              {
                emm_start_nas_service_request_procedure(emm_ctrl_data_ptr,
                                                    conn_est_cause);
              }

              event.emm_in_msg = (byte)NAS_EMM_SERVICE_REQ;
              event_report_payload(EVENT_LTE_EMM_INCOMING_MSG,sizeof(event), (void*)&event);
            }
            else
            {

              emm_failure_cause.cause_type = LTE_NAS_IRAT_NONE;
              emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE, 
                                         LTE_NAS_NO_FAILURE,
                                         emm_msgr_cmd->emm_msgr_cmd.emm_service_req.trans_id,
                                         emm_failure_cause);
            }
          }
          else
          {
            emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
            emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_DS_DISALLOW, 
                                           LTE_NAS_NO_FAILURE,
                                           emm_msgr_cmd->emm_msgr_cmd.emm_service_req.trans_id,
                                           emm_failure_cause);
          }
        }
#ifdef FEATURE_LTE_REL9
        else if((emm_msgr_cmd->emm_msgr_cmd.emm_service_req.emc_bc == TRUE) && 
                (emm_ctrl_data_ptr->rrc_active == TRUE) &&
                (emm_ctrl_data_ptr->plmn_service_state->service_status != 
                                                        SYS_SRV_STATUS_NO_SRV) &&
                (EMM_GET_STATE() == EMM_REGISTERED) &&
                ((emm_chk_limited_service_state(emm_ctrl_data_ptr) == TRUE) ||
                 (EMM_GET_SUBSTATE() == EMM_REGISTERED_LIMITED_SERVICE)))
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= SRV REQ fro EMC DRB in LSM - Locally detach and EMC attach");

          /* locally detach */
          emm_build_and_send_detach_ind(EMM_ATTACH_STARTED);
          emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                        SYS_SRV_STATUS_LIMITED;
          emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = 
                                                         SYS_SRV_DOMAIN_NO_SRV;
          emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV;
          EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_ATTACH_NEEDED);
        }
#endif
        else if ((EMM_GET_STATE()==EMM_TRACKING_AREA_UPDATING_INITIATED)||
                 ((EMM_GET_STATE()==EMM_REGISTERED)&&
                  (EMM_GET_SUBSTATE()==EMM_REGISTERED_ATTEMPTING_TO_UPDATE)&&
                  (mm_timer_status[TIMER_T3411] == TIMER_ACTIVE))||
                 ((EMM_GET_CONNECTION_STATE()==EMM_CONNECTED_STATE) && 
                  (mm_timer_status[TIMER_T3440]== TIMER_ACTIVE)))
        {
          emm_failure_cause.cause_type =  LTE_NAS_IRAT_NONE;
          emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE, 
                                         LTE_NAS_NO_FAILURE,
                                         emm_msgr_cmd->emm_msgr_cmd.emm_service_req.trans_id,
                                         emm_failure_cause);
        }
        else
        {
          emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
          emm_build_and_send_failure_ind(LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE, 
                                         LTE_NAS_NO_FAILURE,
                                         emm_msgr_cmd->emm_msgr_cmd.emm_service_req.trans_id,
                                         emm_failure_cause);

          event.emm_in_msg = (byte)NAS_EMM_SERVICE_REQ;
          event_report_payload(EVENT_LTE_EMM_INCOMING_MSG,sizeof(event), (void*)&event);
        }
      }      
      break ;
    case NAS_EMM_DATA_REQ:
      emm_process_esm_data_req((emm_data_req_T *)&emm_msgr_cmd->emm_msgr_cmd,emm_ctrl_data_ptr) ;
      event.emm_in_msg = (byte)NAS_EMM_DATA_REQ;
      event_report_payload(EVENT_LTE_EMM_INCOMING_MSG,sizeof(event), (void*)&event);
      break ;
    case NAS_EMM_DETACH_CMD:
      if((EMM_GET_STATE() != EMM_DEREGISTERED_INITIATED) &&
         (EMM_GET_STATE() != EMM_DEREGISTERED))
      {   
        

        switch(emm_msgr_cmd->emm_msgr_cmd.emm_detach_cmd.detach_cause)
        {
          case ESM_NO_ACTIVE_BEARER:
            emm_ps_detach_clean_up(emm_ctrl_data_ptr);
            /* Cause is sent when SR/ESR/TAU by EMM resulting in no bearers at ESM */
            if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB) || 
             (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB))
            {
              emm_csfb_failure_cause_type failure_cause_type;
              /* Relesect to GW, if last procedure was ESR */
              failure_cause_type.failure_cause.lte_nas_failure_cause = ESM_LOCAL_CUASE_NONE;
              emm_process_extended_service_req_failure(failure_cause_type,emm_ctrl_data_ptr);

            }
            else
            {
              /* Initiate Attach again after checking rrc_active, service status & registration restrictions*/
              if((emm_ctrl_data_ptr->rrc_active == TRUE) &&
              (emm_ctrl_data_ptr->plmn_service_state->service_status != 
                                                      SYS_SRV_STATUS_NO_SRV) &&
              (emm_ctrl_data_ptr->reg_req_pended_indication == FALSE)&&
              (emm_is_plmn_reg_allowed(emm_ctrl_data_ptr->last_rrc_service_ind_ptr,emm_ctrl_data_ptr) == TRUE)&&
#ifdef FEATURE_LTE_REL10 
              ((mm_timer_status[TIMER_T3346] == TIMER_STOPPED)
#ifdef FEATURE_LTE_REL11
               ||
               ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
                ((emm_check_high_priority(emm_ctrl_data_ptr)) ||
                 (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_EXIST)))
#endif
               )
#endif
              )
              {
                if(reg_service_report ==(emm_reg_service_report_type)emm_send_reg_cnf)
                {
                  mm_send_mmr_reg_req_ignored_ind((sys_modem_as_id_e_type)mm_as_id);
                }
                else
                {
                  reg_service_report(MMR_CAUSE_FAILURE_REMAIN_ON_PLMN, 
                               FALSE, 
                               emm_ctrl_data_ptr);
                  emm_init_attach_procedure(EMM_INITIAL_UPDATE, emm_ctrl_data_ptr);
                }
              }
              //no service case
              else if((emm_ctrl_data_ptr->rrc_active == TRUE) && (emm_ctrl_data_ptr->reg_req_pended_indication == FALSE))
              {
                if(reg_service_report ==(emm_reg_service_report_type)emm_send_reg_cnf)
                {
                  mm_send_mmr_reg_req_ignored_ind((sys_modem_as_id_e_type)mm_as_id);
                }
                else
                {
                  /*If ATTACH cannot be started, send report to REG indicating maximum attempts have been made in cause.*/
                  reg_service_report(MMR_CAUSE_FAILURE_MAX_ATTEMPTED, 
                                      FALSE, 
                                      emm_ctrl_data_ptr);
                }
              }
            }
            break;

          case ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT:
            /* This cause is sent PDN is disconnected by DS due to IRAT to 3GPP2 */
            /* Do a local detach and send report to REG*/
            emm_ps_detach_clean_up(emm_ctrl_data_ptr);
            if((emm_ctrl_data_ptr->rrc_active == TRUE) && (emm_ctrl_data_ptr->reg_req_pended_indication == FALSE))
            {
              if(reg_service_report ==(emm_reg_service_report_type)emm_send_reg_cnf)
              {
                mm_send_mmr_reg_req_ignored_ind((sys_modem_as_id_e_type)mm_as_id);
              }
              else
              {
                reg_service_report(MMR_CAUSE_FAILURE_MAX_ATTEMPTED, 
                                   FALSE, 
                                   emm_ctrl_data_ptr);
              }
            }
            break;

          case ESM_LOCAL_CUASE_NONE:
            /* This casue is sent when Last PDN is Disconnected from DS */
          default:
            if((emm_ctrl_data_ptr->rrc_active == TRUE) &&
               ((emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                          SYS_SRV_STATUS_SRV &&
                 emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED) 
#ifdef FEATURE_LTE_REL9             
                ||
                (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY &&
                 emm_ctrl_data_ptr->plmn_service_state->service_status != 
                                                         SYS_SRV_STATUS_NO_SRV)
#endif
               ))
             {
                /*Abort any ongoing service req procedure or TAU precedure going on before initiating MO detach*/
                if(EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED)
                {
                  emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
                  emm_abort_service_request(emm_ctrl_data_ptr, FALSE, TRUE,
                                  LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                  emm_failure_cause);
                }
                else if (EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED)
                {
                  emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
                  mm_stop_timer(TIMER_T3430);
                  emm_remove_release_client(emm_tau_request_release_indication);
                  EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NORMAL_SERVICE);      
                }
                emm_start_mo_detach(emm_get_mo_detach_type(emm_ctrl_data_ptr), 
                                FALSE, 
                                NULL, 
                                emm_detach_failed_routine, 
                                emm_ctrl_data_ptr);
             }
             else
             {  
               emm_ps_detach_clean_up(emm_ctrl_data_ptr);
               if((emm_ctrl_data_ptr->rrc_active == TRUE) && (emm_ctrl_data_ptr->reg_req_pended_indication == FALSE))
               {
                 if(reg_service_report ==(emm_reg_service_report_type)emm_send_reg_cnf)
                 {
                   mm_send_mmr_reg_req_ignored_ind((sys_modem_as_id_e_type)mm_as_id);
                 }
                 else
                 {
                   reg_service_report(MMR_CAUSE_FAILURE_MAX_ATTEMPTED, 
                                      FALSE, 
                                      emm_ctrl_data_ptr);
                 }
               }
             }
             break;
        }
       
        event.emm_in_msg = NAS_EMM_DETACH_CMD;
        event_report_payload(EVENT_LTE_EMM_INCOMING_MSG,sizeof(event), (void*)&event);
      }
      else
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Ignoring DETACH CMD rcved in state %d",
                      EMM_GET_STATE());
      }
      break;
    case NAS_EMM_EPS_BEARER_STATUS_CMD:
      emm_process_esm_eps_bearer_status_cmd((emm_eps_bearer_status_cmd_T *)&emm_msgr_cmd->emm_msgr_cmd, 
                                            emm_ctrl_data_ptr);
      event.emm_in_msg = (byte)NAS_EMM_EPS_BEARER_STATUS_CMD;
      event_report_payload(EVENT_LTE_EMM_INCOMING_MSG,sizeof(event), (void*)&event);
      break;
    case NAS_EMM_1XCSFB_ESR_CALL_REQ:
      emm_process_esm_1xcsfb_esr_call_req(&emm_msgr_cmd->emm_msgr_cmd.emm_1xCSFB_esr_call_req,emm_ctrl_data_ptr);
      break;
    case NAS_EMM_1XCSFB_ESR_CALL_ABORT_REQ:
      emm_process_esm_1xcsfb_esr_call_abort_req(&emm_msgr_cmd->emm_msgr_cmd.emm_1xCSFB_esr_call_abort_req,emm_ctrl_data_ptr);
      break;
#ifdef FEATURE_LTE_REL9
    case NAS_EMM_EMC_SRV_STATUS_CMD:
      emm_process_emc_srv_status_cmd(
          &emm_msgr_cmd->emm_msgr_cmd.emm_emc_srv_status_cmd, emm_ctrl_data_ptr);
      event.emm_in_msg = (byte)NAS_EMM_EMC_SRV_STATUS_CMD;
      event_report_payload(EVENT_LTE_EMM_INCOMING_MSG,sizeof(event), (void*)&event);
      break;
#endif
    default:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= Unknown ESM message");
      break ;
  }
  return TRUE;
} /* end of emm_process_outgoing_esm_message() */

/*===========================================================================
FUNCTION  EMM_PROCESS_RRC_EPS_BEARER_UPDATE

DESCRIPTION
  This function processes RRC_ACTIVE_EPS_BEARER_UPDATE_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_rrc_eps_bearer_update
(
  lte_rrc_active_eps_bearer_update_ind_s  *eps_bearer_update_ind,
  emm_ctrl_data_type                      *emm_ctrl_data_ptr
)
{
  log_lte_nas_emm_outgoing_msg_event_type event;
  esm_active_eps_ind_T esm_active_eps_ind ;
  uint8  index = 0;                   

  ASSERT(eps_bearer_update_ind != (lte_rrc_active_eps_bearer_update_ind_s *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  MSG_HIGH_DS_3(MM_SUB, "=EMM= Active EPS bearer update ind - Added %d, rmved %d, active %d",
           eps_bearer_update_ind->num_added_bearers,
           eps_bearer_update_ind->num_removed_bearers,
           eps_bearer_update_ind->num_active_bearers);

  if(mm_timer_status[TIMER_T3440] != TIMER_STOPPED)
  {
    /* Send out all pended messages */
    emm_send_all_pended_messages(emm_ctrl_data_ptr); 

    /* Send out all pending IRAT uplink messages */
    emm_send_all_pending_irat_msg(emm_ctrl_data_ptr);

    if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED) || 
       (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED))
    {
      emm_send_esr_if_buffered(emm_ctrl_data_ptr);
    }
  }

  MSG_HIGH_DS_0(MM_SUB, "=EMM= stop T3440 timer");
  mm_stop_timer(TIMER_T3440);   
  emm_ctrl_data_ptr->start_t3440_on_accept = FALSE;
  msgr_init_hdr((msgr_hdr_s *)&esm_active_eps_ind.msg_hdr, MSGR_NAS_EMM, NAS_ESM_ACTIVE_EPS_IND);

  esm_active_eps_ind.num_active_bearers = eps_bearer_update_ind->num_active_bearers ;
  esm_active_eps_ind.num_added_bearers = eps_bearer_update_ind->num_added_bearers ;
  esm_active_eps_ind.num_removed_bearers = eps_bearer_update_ind->num_removed_bearers ;

  if (esm_active_eps_ind.num_active_bearers > EPS_MAX_NUM_ACTIVE_EPS_BEARERS)
  {
    esm_active_eps_ind.num_active_bearers = EPS_MAX_NUM_ACTIVE_EPS_BEARERS ;
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Active EPS bearer list has been truncated");
  }

  if (esm_active_eps_ind.num_added_bearers > EPS_MAX_NUM_ACTIVE_EPS_BEARERS)
  {
    esm_active_eps_ind.num_added_bearers = EPS_MAX_NUM_ACTIVE_EPS_BEARERS ;
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Added EPS bearer list has been truncated");
  }

  if (esm_active_eps_ind.num_removed_bearers > EPS_MAX_NUM_ACTIVE_EPS_BEARERS)
  {
    esm_active_eps_ind.num_removed_bearers = EPS_MAX_NUM_ACTIVE_EPS_BEARERS ;
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Removed EPS bearer list has been truncated");
  }

  ASSERT(esm_active_eps_ind.num_active_bearers <= LTE_RRC_MAX_NUM_ACTIVE_EPS_BEARERS);
  ASSERT(esm_active_eps_ind.num_added_bearers  <= LTE_RRC_MAX_NUM_ACTIVE_EPS_BEARERS);
  ASSERT(esm_active_eps_ind.num_removed_bearers <= LTE_RRC_MAX_NUM_ACTIVE_EPS_BEARERS);

  emm_ctrl_data_ptr->last_rrc_bearer_ind = *eps_bearer_update_ind; 

  for(index =0; index < eps_bearer_update_ind->num_active_bearers; index++)
  {
    MSG_HIGH_DS_2(MM_SUB, "=EMM= Active EPS bearer ID %d, RB_ID %d",
                  eps_bearer_update_ind->active_eps_bearers[index].eps_id,
                  eps_bearer_update_ind->active_eps_bearers[index].rb_id);
  }
  memscpy((void *)&esm_active_eps_ind.active_eps_bearers[0],
          sizeof(esm_active_eps_ind.active_eps_bearers),
          (void *)&eps_bearer_update_ind->active_eps_bearers[0],
         sizeof(esm_active_eps_ind.active_eps_bearers) ) ;

  for(index =0; index < eps_bearer_update_ind->num_added_bearers; index++)
  {
    MSG_HIGH_DS_2(MM_SUB, "=EMM= Added EPS bearer ID %d, RB_ID %d",
                  eps_bearer_update_ind->added_eps_bearers[index].eps_id,
                  eps_bearer_update_ind->added_eps_bearers[index].rb_id);
  }
  memscpy((void *)&esm_active_eps_ind.added_eps_bearers[0],
          sizeof(esm_active_eps_ind.added_eps_bearers),
          (void *)&eps_bearer_update_ind->added_eps_bearers[0],
         sizeof(esm_active_eps_ind.added_eps_bearers)  ) ;

  for(index =0; index < eps_bearer_update_ind->num_removed_bearers; index++)
  {
    MSG_HIGH_DS_2(MM_SUB, "=EMM= Removed EPS bearer ID %d, RB_ID %d",
                  eps_bearer_update_ind->removed_eps_bearers[index].eps_id,
                  eps_bearer_update_ind->removed_eps_bearers[index].rb_id);
  }
  memscpy((void *)&esm_active_eps_ind.removed_eps_bearers[0],
          sizeof(esm_active_eps_ind.removed_eps_bearers),
          (void *)&eps_bearer_update_ind->removed_eps_bearers[0],
         sizeof(esm_active_eps_ind.removed_eps_bearers) ) ;
  

  if(emm_msgr_send((msgr_hdr_s *)&esm_active_eps_ind, sizeof(esm_active_eps_ind_T)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= emm_msgr_send failed!", 0,0,0 );
  } 
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent NAS_ESM_ACTIVE_EPS_IND");
    event.emm_out_msg = (byte)NAS_ESM_ACTIVE_EPS_IND;
    event_report_payload(EVENT_LTE_ESM_OUTGOING_MSG,sizeof(event), (void*)&event);
  }
} /* end of emm_process_rrc_eps_bearer_update() */

void emm_process_incoming_esm_message
(
  lte_nas_esm_incoming_msg_type  *esm_incoming_msg,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  esm_data_ind_T *esm_data_ind_ptr ;

  ASSERT(esm_incoming_msg != (lte_nas_esm_incoming_msg_type *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  /* Allocate memory and initialize it */
  esm_data_ind_ptr = emm_modem_mem_alloc(sizeof(esm_data_ind_T),
                                             emm_ctrl_data_ptr);
                                             
  mm_check_for_null_ptr((void*)esm_data_ind_ptr);


  msgr_init_hdr((msgr_hdr_s *)&esm_data_ind_ptr->msg_hdr, MSGR_NAS_EMM, NAS_ESM_DATA_IND);

  esm_data_ind_ptr->esm_incoming_msg = *esm_incoming_msg;

  esm_data_ind_ptr->length = sizeof(lte_nas_esm_incoming_msg_type) ;

  if(emm_msgr_send((msgr_hdr_s *)esm_data_ind_ptr, sizeof(esm_data_ind_T)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= emm_msgr_send failed!", 0,0,0 );
  } 
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent NAS_ESM_DATA_IND");
  }

  /* Free the memory block */
  emm_modem_mem_free(esm_data_ind_ptr, emm_ctrl_data_ptr);

  emm_chk_eps_bearer_sync_recovery(esm_incoming_msg, emm_ctrl_data_ptr);

} /* end of emm_process_incoming_esm_message() */

void emm_build_and_send_failure_ind (lte_nas_local_cause_enum_T  local_cause, 
                                     lte_nas_emm_cause_type      emm_ota_cause,
                                     dword                       trans_id,
                                     emm_failure_type            emm_failure_cause)
{
  esm_failure_ind_T      failure_ind;
  log_lte_nas_emm_outgoing_msg_event_type event;

  memset(&failure_ind, 0x00, sizeof(esm_failure_ind_T));

  msgr_init_hdr((msgr_hdr_s *)&failure_ind.msg_hdr, MSGR_NAS_EMM, NAS_ESM_FAILURE_IND);
  failure_ind.trans_id = trans_id;

  if(local_cause == ESM_LOCAL_CUASE_NONE)
  {
    failure_ind.emm_cause.valid = FALSE;
  }
  else
  {
    failure_ind.emm_cause.valid = TRUE;
  }
  failure_ind.emm_cause.local_cause = local_cause;

  /* For service request failure, ESM needs the cause in service reject message if existing. */
  if((local_cause == LTE_NAS_SERVICE_REQ_FAILED)&&(emm_ota_cause != LTE_NAS_NO_FAILURE))
  {
    /* Only set emm_ota_cause to valid if it is a service request failure AND a cause in service reject message is received. */
    failure_ind.emm_ota_cause.valid = TRUE;
  }
  else
  {
    failure_ind.emm_ota_cause.valid = FALSE;
  }
  failure_ind.emm_ota_cause.local_cause = (lte_nas_local_cause_enum_T)emm_ota_cause;
  
  /*send the emm failure cause to esm->cm*/
  failure_ind.emm_failure_cause = emm_failure_cause;

  if(emm_msgr_send((msgr_hdr_s *)&failure_ind, sizeof(failure_ind)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= emm_msgr_send failed!", 0,0,0 );
  } 
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent NAS_ESM_FAILURE_IND");
    event.emm_out_msg = (byte)NAS_ESM_FAILURE_IND;
    event_report_payload(EVENT_LTE_ESM_OUTGOING_MSG,sizeof(event), (void*)&event);
  }
} /* end of emm_build_and_send_failure_ind() */

void emm_build_and_send_detach_ind (lte_nas_local_cause_enum_T local_cause)
{
  esm_detach_ind_T detach_ind;
  log_lte_nas_emm_outgoing_msg_event_type event;

  msgr_init_hdr((msgr_hdr_s *)&detach_ind.msg_hdr, MSGR_NAS_EMM, NAS_ESM_DETACH_IND);

  detach_ind.cause.local_cause = local_cause;
  detach_ind.cause.valid = TRUE;
  if(emm_msgr_send((msgr_hdr_s *)&detach_ind, sizeof(detach_ind)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= emm_msgr_send failed!", 0,0,0 );
  } 
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent NAS_ESM_DETACH_IND");
    event.emm_out_msg = (byte)NAS_ESM_DETACH_IND;
    event_report_payload(EVENT_LTE_ESM_OUTGOING_MSG,sizeof(event), (void*)&event);
  }
}

/*===========================================================================

FUNCTION    EMM_BUILD_END_SEND_EPS_BEARER_STATUS

DESCRIPTION
  This function builds and sends NAS_ESM_EPS_BEARER_STATUS_IND to ESM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_build_and_send_eps_bearer_status
(
  lte_nas_eps_bearer_context_status_type *eps_bearer_context_status_ptr
)
{
  log_lte_nas_emm_outgoing_msg_event_type event;
  esm_eps_bearer_status_ind_T eps_bearer_status_ind;

  ASSERT(eps_bearer_context_status_ptr != (lte_nas_eps_bearer_context_status_type *)NULL) ; 

  msgr_init_hdr((msgr_hdr_s *)&eps_bearer_status_ind.msg_hdr, 
                               MSGR_NAS_EMM, 
                               NAS_ESM_EPS_BEARER_STATUS_IND);

  eps_bearer_status_ind.context_status = *eps_bearer_context_status_ptr ;

  if(emm_msgr_send((msgr_hdr_s *)&eps_bearer_status_ind, sizeof(esm_eps_bearer_status_ind_T)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= emm_msgr_send failed!", 0,0,0 );
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent NAS_ESM_EPS_BEARER_STATUS_IND");
    event.emm_out_msg = (byte)NAS_ESM_EPS_BEARER_STATUS_IND;
    event_report_payload(EVENT_LTE_ESM_OUTGOING_MSG,sizeof(event), (void*)&event);
  }
} /* end of emm_build_and_send_eps_bearer_status() */

/*===========================================================================

FUNCTION    EMM_BUILD_END_SEND_GET_PDN_CONNECTIVITY_REQ_IND

DESCRIPTION
  This function builds/sends NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND to ESM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_build_and_send_get_pdn_connectivity_req_ind 
(
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
  esm_get_pdn_connectivity_req_T               get_pdn_connectivity_req_ind;
  log_lte_nas_emm_outgoing_msg_event_type      event;

  ASSERT(emm_ctrl_data_ptr->pdn_conn_req_trans_id == 
                                           EMM_INVALID_PDN_CONN_REQ_TRANS_ID); 

  msgr_init_hdr((msgr_hdr_s *)&get_pdn_connectivity_req_ind.msg_hdr, 
                MSGR_NAS_EMM, 
                NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND);

  /* Get sequence number */
  if(emm_ctrl_data_ptr->attach_pdn_seq_num == 0xFF)
  {
    /* Wrap around */
    emm_ctrl_data_ptr->attach_pdn_seq_num = 1;
  }
  else
  {
    emm_ctrl_data_ptr->attach_pdn_seq_num = 
                                     emm_ctrl_data_ptr->attach_pdn_seq_num + 1;
  }

  get_pdn_connectivity_req_ind.seq_num = emm_ctrl_data_ptr->attach_pdn_seq_num;
#ifdef FEATURE_LTE_REL10
  get_pdn_connectivity_req_ind.responding_to_page = emm_ctrl_data_ptr->responding_to_page;
#endif

  if(emm_msgr_send((msgr_hdr_s *)&get_pdn_connectivity_req_ind,
               sizeof(get_pdn_connectivity_req_ind)) 
     != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= Failed to send NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND!", 
               0,0,0 );
  } 
  else
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Sent NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND w/ Seq Num %d",
                  get_pdn_connectivity_req_ind.seq_num);
    event.emm_out_msg = NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND;
    event_report_payload(EVENT_LTE_ESM_OUTGOING_MSG,sizeof(event), (void*)&event);
  }
} /* end of emm_build_and_send_get_pdn_connectivity_req_ind */

void emm_build_and_send_get_isr_status_ind(void)
{
  esm_get_isr_status_ind_T   isr_status_ind;
  log_lte_nas_emm_outgoing_msg_event_type event;

  msgr_init_hdr((msgr_hdr_s *)&isr_status_ind.msg_hdr, MSGR_NAS_EMM, NAS_ESM_GET_ISR_STATUS_IND);

  if(emm_msgr_send((msgr_hdr_s *)&isr_status_ind, sizeof(esm_get_isr_status_ind_T)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= emm_msgr_send failed!", 0,0,0 );
  } 
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent NAS_ESM_GET_ISR_STATUS_IND");
    event.emm_out_msg = (byte)NAS_ESM_GET_ISR_STATUS_IND;
    event_report_payload(EVENT_LTE_ESM_OUTGOING_MSG,sizeof(event), (void*)&event);
  }
}

/*===========================================================================

FUNCTION    EMM_BUILD_AND_SEND_ISR_STATUS_CHANGE_IND

DESCRIPTION
  This function sends ISR status change indication to ESM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_build_and_send_isr_status_change_ind(boolean status)
{
  esm_isr_status_change_ind_T   isr_status_ind;
  log_lte_nas_emm_outgoing_msg_event_type event;

  msgr_init_hdr((msgr_hdr_s *)&isr_status_ind.msg_hdr, MSGR_NAS_EMM, NAS_ESM_ISR_STATUS_CHANGE_IND);

  isr_status_ind.isr_status = status;

  if(emm_msgr_send((msgr_hdr_s *)&isr_status_ind, sizeof(esm_isr_status_change_ind_T)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= emm_msgr_send failed!", 0,0,0 );
  } 
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent NAS_ESM_ISR_STATUS_CHANGE_IND");
    event.emm_out_msg = (byte)NAS_ESM_ISR_STATUS_CHANGE_IND;
    event_report_payload(EVENT_LTE_ESM_OUTGOING_MSG,sizeof(event), (void*)&event);
  }
}

/*===========================================================================

FUNCTION    EMM_BUILD_AND_SEND_ESR_FAILURE_IND

DESCRIPTION
  This function sends ISR status change indication to ESM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_build_and_send_esr_failure_ind (reject_type_enum_T  reject_type, 
                                         reject_cause_T      reject_cause,
                                         dword               trans_id)
{
  emm_1xCSFB_esr_call_rsp_T      esr_call_rsp;
  log_lte_nas_emm_outgoing_msg_event_type event;

  memset(&esr_call_rsp, 0x00, sizeof(emm_1xCSFB_esr_call_rsp_T));

  msgr_init_hdr((msgr_hdr_s *)&esr_call_rsp.msg_hdr, MSGR_NAS_EMM, NAS_ESM_1XCSFB_ESR_CALL_RSP);

  esr_call_rsp.reject.rej_type = reject_type;
  esr_call_rsp.reject.rej_value = reject_cause;
  esr_call_rsp.reject.present = TRUE;
  esr_call_rsp.trans_id = trans_id;
  
  if(emm_msgr_send((msgr_hdr_s *)&esr_call_rsp, sizeof(esr_call_rsp)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= emm_msgr_send failed!", 0,0,0 );
  } 
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent NAS_ESM_1XCSFB_ESR_CALL_RSP");
    event.emm_out_msg = (byte)NAS_ESM_1XCSFB_ESR_CALL_RSP;
    event_report_payload(EVENT_LTE_ESM_OUTGOING_MSG,sizeof(event), (void*)&event);
  }
} /* end of emm_build_and_send_failure_ind() */

#ifdef FEATURE_LTE_REL10
/*===========================================================================

FUNCTION    EMM_SEND_UNBLOCK_ALL_APNS_IND

DESCRIPTION
  This function sends Unblock all APNs indication to ESM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_unblock_all_apns_ind(void)
{
  esm_unblock_all_apns_ind_T  unblock_all_apns_ind;
  log_lte_nas_emm_outgoing_msg_event_type  event;
  errno_enum_type error_code;

  memset(&unblock_all_apns_ind, 0x00, sizeof(esm_unblock_all_apns_ind_T));

  msgr_init_hdr((msgr_hdr_s *)&unblock_all_apns_ind.msg_hdr,
                        MSGR_NAS_EMM, NAS_ESM_UNBLOCK_ALL_APNS_IND);

  error_code = emm_msgr_send((msgr_hdr_s *)&unblock_all_apns_ind,
                    sizeof(unblock_all_apns_ind));

  if(error_code != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= msgr_send failed with error code %d!", error_code,0,0 );
  } 
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent NAS_ESM_UNBLOCK_ALL_APNS_IND");
    event.emm_out_msg = (byte)NAS_ESM_UNBLOCK_ALL_APNS_IND;
    event_report_payload(EVENT_LTE_ESM_OUTGOING_MSG,sizeof(event), (void*)&event);
  }
} /* end of emm_send_unblock_all_apns_ind() */
#endif

#endif  /*FEATURE_LTE*/
