/*!
  @file
  esm_proc_active.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*==============================================================================

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_proc_active.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/17/12   cogolt  Modified input parameters esm_build_and_send_pdn_connectivity_reject_ind to 
                   support Rel-10 ESM backoff timer (T3396) requirement
05/14/10   RI      Mainlined spec changes upto Dec 09.
02/17/10   zren    Added support to always deact bearer upon DS request in UE 
                   initiated modification procedure
02/03/10   zren    Added ESM new timer machanism
01/21/10   zren    Added EMM/ESM STATUS feature
12/10/09   zren    Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
11/18/09   zren    Added support for ESM Information procedure
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
09/14/09   zren    Added bearer ID and ESM cause check for PDN DISCONNECT 
                   REQ/REJ messages
09/01/09   MNK     Added handling of detach_ind from EMM, as we support bearer initiated procedures
08/18/09   MNK     CM-NAS interface changes
08/14/09   MNK     Add Support for PDN disconnect request
06/02/09   MNK     Added additional criteria of mathcing the LBI to match the UE initiated request  
05/21/09   MNK     Added support to reply back to CM/DS in case the NW sent bad messages as a response to UE initiated request
05/11/09   MNK     Added stop timer when bearer resource allocation reject is received
05/08/09   MNK     Added support for bearer resource allocation procedures
05/01/09   MNK     Added support for N/W initiated bearer context deactivation
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#ifdef FEATURE_LTE
#include "comdef.h"
#include "amssassert.h"
#include "esm_bpm.h"
#include "esm_proc_state.h"
#include "esm_utils.h"
#include "sm_multimode_common.h"

/*==============================================================================

  FUNCTION:  esm_proc_process_act_default_bearer_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_process_act_default_bearer_ctxt_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_process_act_default_bearer_ctxt_req() goes here.

*/
/*============================================================================*/
static void    esm_proc_process_act_default_bearer_ctxt_req
(
  struct esm_bearer_resource_proc* proc_ptr, const byte* msg_ptr  /*!< REQUIRED short parameter description */
)
{
  proc_ptr->esm_proc_state_ptr = &esm_proc_state_inactive;
} /* esm_proc_process_act_default_bearer_ctxt_req() */


extern boolean emm_skip_emc_retry( void );
#ifdef FEATURE_DUAL_SIM
extern boolean emm_skip_emc_retry_per_subs( sys_modem_as_id_e_type sub_id );
#endif
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*==============================================================================

  FUNCTION:  esm_proc_active_process_OTA_message

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_active_process_OTA_message() does goes here.

    @detail
    Processes the OTA messages received
*/
/*============================================================================*/
static boolean esm_proc_active_process_OTA_message
(
  struct esm_bearer_resource_proc   *proc_ptr, 
  lte_nas_esm_incoming_msg_type     *data_ptr
)
{ 
  boolean                  result = FALSE;
  lte_nas_local_cause_T    local_cause;
  esm_cause_T              ota_cause;
  boolean                  send_reject_ind = FALSE;
  
  ASSERT(proc_ptr != NULL);
  ASSERT(proc_ptr->pending_msg_ptr != NULL);
  ASSERT(data_ptr != NULL);

  switch(data_ptr->nas_hdr.msg_id)
  {
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
      if(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                                      PDN_CONNECTIVITY_REQUEST)
      {
        if(proc_ptr->chk_on_attach_def_bc_setup_ptr() == TRUE)
        {
          /* Stop timer 3482 if the proc is not during attach */
          esm_stop_timer(ESM_TIMER_T3482, proc_ptr);
        }
#ifdef FEATURE_LTE_REL9
        /* This logic needs to be revisited once IMS emergency bearer service 
           is supported on UMTS at the same time */
        if((proc_ptr->pending_msg_ptr->esm_out_msg.pdn_connectivity_req.req
                                                              == ESM_EMERGENCY) &&
           (data_ptr->default_bearer_context_req.ti.valid == TRUE))
        {
          MSG_HIGH_DS_0(SM_SUB,"ESM: Invalidate TI for bearer ctxt for emergency service");
          data_ptr->default_bearer_context_req.ti.valid = FALSE;
        }
#endif
        result = TRUE;
      }
      else
      {
        esm_send_esm_status(
                         &data_ptr->default_bearer_context_req.lte_nas_esm_hdr,
                          NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
        send_reject_ind = TRUE;
      }
      break;
    case PDN_CONNECTIVITY_REJECT:
      if(data_ptr->pdn_connectivity_rej.lte_nas_esm_hdr.bearer_id == 
                                                          UNASSIGNED_BEARER_ID)
      {
        MSG_HIGH_DS_0(SM_SUB,"ESM: Rcved PDN_CONNECTIVITY_REJECT");
        if(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                                      PDN_CONNECTIVITY_REQUEST)
        {
          if(proc_ptr->chk_on_attach_def_bc_setup_ptr() == TRUE)
          {
            /* Stop timer 3482 */
            esm_stop_timer(ESM_TIMER_T3482, proc_ptr);
          }
#ifdef FEATURE_LTE_REL9 
          esm_pdn_conn_failure_update_emc_srv_status(proc_ptr);
#endif
          esm_build_and_send_pdn_connectivity_reject_ind(proc_ptr, proc_ptr->sdf_id,
                      &(data_ptr->pdn_connectivity_rej), ESM_LOCAL_CUASE_NONE, NULL);
        }
        else
        {
          esm_send_esm_status(&data_ptr->pdn_connectivity_rej.lte_nas_esm_hdr,
                               NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
          send_reject_ind = TRUE;
        }
      }
      else
      {
        MSG_HIGH_DS_1(SM_SUB,"ESM: Ignoring PDN_CONNECTIVITY_REJECT w/ bearer id: %d", 
                      data_ptr->pdn_connectivity_rej.lte_nas_esm_hdr.bearer_id);
        send_reject_ind = TRUE;
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
      }
      break;
    case DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST:
      switch(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id) 
      {
        case PDN_DISCONNECT_REQUEST:
          esm_stop_timer(ESM_TIMER_T3492, proc_ptr);
          result = TRUE;
          break;
        case BEARER_RESOURCE_MODIFICATION_REQUEST:
          esm_stop_timer(ESM_TIMER_T3481, proc_ptr);
          result = TRUE;
          break;
        default:
          esm_send_esm_status(
                  &data_ptr->deactivate_eps_bearer_context_req.lte_nas_esm_hdr,
                   NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
          send_reject_ind = TRUE;
          break;
      }
      break;
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST:
      switch(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id) 
      {
        case BEARER_RESOURCE_ALLOCATION_REQUEST:
          esm_stop_timer(ESM_TIMER_T3480, proc_ptr);
          result = TRUE;
          break;
        case BEARER_RESOURCE_MODIFICATION_REQUEST:
          esm_stop_timer(ESM_TIMER_T3481, proc_ptr);
          /* Check if need to deact the bearer upon DS request */
          esm_chk_ds_explicit_deact(proc_ptr);
          result = TRUE;
          break;
        default:
          esm_send_esm_status(
                       &data_ptr->dedicated_bearer_context_req.lte_nas_esm_hdr,
                        NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
          send_reject_ind = TRUE;
          break;
      }
      break;
    case BEARER_RESOURCE_ALLOCATION_REJECT:
      if(data_ptr->bearer_resource_alloc_rej.lte_nas_esm_hdr.bearer_id == 
                                                         UNASSIGNED_BEARER_ID )
      {
        if(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                            BEARER_RESOURCE_ALLOCATION_REQUEST)
        {
          MSG_HIGH_DS_0(SM_SUB,"ESM: Reved BEARER_RESOURCE_ALLOCATION_REJECT ");
          esm_stop_timer(ESM_TIMER_T3480, proc_ptr);
          esm_build_and_send_cm_bearer_resource_allocation_reject(
                                           proc_ptr->sdf_id,
                                          &data_ptr->bearer_resource_alloc_rej,
                                           ESM_LOCAL_CUASE_NONE,
                                           proc_ptr->lbi);
          if((data_ptr->nas_hdr.valid == TRUE) &&
             (data_ptr->bearer_resource_alloc_rej.esm_cause.esm_cause == 
                                             NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT))
          {
            local_cause.valid = TRUE;
            local_cause.local_cause = LTE_NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT;
            proc_ptr->request_esm_to_deactivate_ctxts_func_ptr(local_cause,
                 data_ptr->bearer_resource_alloc_rej.esm_cause, proc_ptr->lbi);
          }
        }
        else
        {
          esm_send_esm_status(
                          &data_ptr->bearer_resource_alloc_rej.lte_nas_esm_hdr,
                           NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
          send_reject_ind = TRUE;
        }
      }
      else
      {
        MSG_HIGH_DS_1(SM_SUB,"ESM: Ignoring BEARER_RESRC_ALLOCATION_REJ w/ bearer id: %d", 
                      data_ptr->bearer_resource_alloc_rej.lte_nas_esm_hdr.bearer_id);
        send_reject_ind = TRUE;
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
      }
      break;
    case MODIFY_EPS_BEARER_CONTEXT_REQUEST:
      switch(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id) 
      {
        case BEARER_RESOURCE_ALLOCATION_REQUEST:
          esm_stop_timer(ESM_TIMER_T3480, proc_ptr);
          result = TRUE;
          break;
        case BEARER_RESOURCE_MODIFICATION_REQUEST:
          esm_stop_timer(ESM_TIMER_T3481, proc_ptr);
          /* Check if need to deact the bearer upon DS request */
          esm_chk_ds_explicit_deact(proc_ptr);
          result = TRUE;
          break;
        default:
          esm_send_esm_status(
                         &data_ptr->modify_eps_bearer_ctxt_req.lte_nas_esm_hdr,
                          NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
          send_reject_ind = TRUE;
          break;
      }
      break;
    case PDN_DISCONNECT_REJECT:
      if(data_ptr->pdn_disconnect_rej.lte_nas_esm_hdr.bearer_id == 
                                                          UNASSIGNED_BEARER_ID)
      {
        MSG_HIGH_DS_0(SM_SUB,"ESM: Rcved PDN_DISCONNECT_REJECT");
        if(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                                        PDN_DISCONNECT_REQUEST)
        {
          esm_stop_timer(ESM_TIMER_T3492, proc_ptr);
          local_cause.valid = FALSE;
          if(data_ptr->nas_hdr.valid == FALSE)
          {
            esm_send_esm_status(&data_ptr->pdn_disconnect_rej.lte_nas_esm_hdr,
                                 data_ptr->nas_hdr.esm_decode_fail_reason);
            ota_cause.valid = FALSE;
          }
          else
          {
            ota_cause.valid = TRUE;
            ota_cause.esm_cause = 
                              data_ptr->pdn_disconnect_rej.esm_cause.esm_cause;
          }
          /* For all other causes, locally deactivate assoicated contexts */
          proc_ptr->request_esm_to_deactivate_ctxts_func_ptr(local_cause,
                                                             ota_cause,
                                                             proc_ptr->lbi);
        }
        else
        {
          esm_send_esm_status(&data_ptr->pdn_disconnect_rej.lte_nas_esm_hdr,
                               NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
          send_reject_ind = TRUE;
        }
      }
      else
      {
        MSG_HIGH_DS_1(SM_SUB,"ESM: Ignoring PDN_DISCONNECT_REJECT w/ bearer id: %d", 
                      data_ptr->pdn_disconnect_rej.lte_nas_esm_hdr.bearer_id);
        send_reject_ind = TRUE;
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
      }
      break;
    case BEARER_RESOURCE_MODIFICATION_REJECT:
      if(data_ptr->bearer_resource_modifi_rej.lte_nas_esm_hdr.bearer_id == 
                                                          UNASSIGNED_BEARER_ID)
      {
        MSG_HIGH_DS_0(SM_SUB,"ESM: Rcved BEARER_RESOURCE_MODIFICATION_REJECT");
        if(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                          BEARER_RESOURCE_MODIFICATION_REQUEST)
        {
          esm_stop_timer(ESM_TIMER_T3481, proc_ptr);
          esm_build_and_send_cm_bearer_resrc_mod_reject(
                                          proc_ptr->sdf_id,
                                         &data_ptr->bearer_resource_modifi_rej,
                                          ESM_LOCAL_CUASE_NONE, 
                                          proc_ptr->lbi);
          /* 6.5.4.5 Abnormal case b */
          if((data_ptr->nas_hdr.valid == TRUE) &&
             (data_ptr->bearer_resource_modifi_rej.esm_cause.esm_cause == 
                                           NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT))
          {
            local_cause.valid = TRUE;
            local_cause.local_cause = LTE_NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT;
            proc_ptr->request_esm_to_deactivate_ctxts_func_ptr(local_cause,
                data_ptr->bearer_resource_modifi_rej.esm_cause, proc_ptr->lbi);
          } 
          else
          {
            /* Check if need to deact the bearer upon DS request */
            esm_chk_ds_explicit_deact(proc_ptr);
          }
        }
        else
        {
          esm_send_esm_status(
                         &data_ptr->bearer_resource_modifi_rej.lte_nas_esm_hdr,
                          NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
          send_reject_ind = TRUE;
        }
      }
      else
      {
        MSG_HIGH_DS_1(SM_SUB,"ESM: Ignoring BEARER_RESOURCE_MOD_REJECT w/ bearer id: %d", 
                      data_ptr->bearer_resource_modifi_rej.lte_nas_esm_hdr.bearer_id);
        send_reject_ind = TRUE;
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
      }
      break;
    case ESM_INFORMATION_REQUEST:
      if(data_ptr->esm_info_req.lte_nas_esm_hdr.bearer_id == 
                                                          UNASSIGNED_BEARER_ID)
      {
        if(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                                      PDN_CONNECTIVITY_REQUEST)
        {
          esm_build_and_send_esm_info_rsp(proc_ptr);
        }
        else
        {
          esm_send_esm_status(&data_ptr->esm_info_req.lte_nas_esm_hdr,
                               NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
          send_reject_ind = TRUE;
        }
      }
      else
      {
        esm_send_esm_status(&data_ptr->pdn_connectivity_rej.lte_nas_esm_hdr,
                             NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT);
        send_reject_ind = TRUE;
      }
      break;
    case ESM_STATUS:
      MSG_HIGH_DS_3(SM_SUB,"ESM: Rcved ESM STATUS w/ PTI %d, Bearer ID %d, ESM cause %u",
                    data_ptr->mt_esm_status.lte_nas_esm_hdr.pti,
                    data_ptr->mt_esm_status.lte_nas_esm_hdr.bearer_id,
                    data_ptr->mt_esm_status.esm_cause);
      if(data_ptr->mt_esm_status.esm_cause == 
                                            NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT)
      {
        /* Return TRUE so that BCM can locally deact the bearer */
        result = TRUE;
      }
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      break;
    default:
      /* Unknown message */
      send_reject_ind = TRUE;
      /* Return TRUE so that BCM can check bearer ID and may send ESM_STATUS 
         message to the network */
      result = TRUE;
      break;
  }

  if(send_reject_ind == TRUE)
  {
    esm_proc_send_cm_autonomous_reject(proc_ptr, ESM_LOWER_LAYER_FAILURE, NULL);
  }

  return result;
} /* esm_proc_active_process_OTA_message() */


/*==============================================================================

  FUNCTION:  esm_proc_active_state

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_active_state() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_active_state() goes here.
*/
/*============================================================================*/
static esm_bp_state_T esm_proc_active_state
(
  void
)
{
  return ESM_BP_ACTIVE;
} /* esm_proc_active_state() */

/*==============================================================================

  FUNCTION:  esm_proc_active_state_name

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_active_state_name() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_active_state_name() goes here.

*/
/*============================================================================*/
static void esm_proc_active_state_name
(     
  void
)
{

} /* esm_proc_active_state_name() */

static char* esm_proc_active_get_state_name(void)
{
  return "ESM_BP_ACTIVE";
}

/*==============================================================================

  FUNCTION:  esm_proc_active_process_bearer_resource_alloc_abort

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_active_process_bearer_resource_alloc_abort() goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_active_process_bearer_resource_alloc_abort() goes here.

*/
/*============================================================================*/
static void esm_proc_active_process_bearer_resource_alloc_abort
(
  struct esm_bearer_resource_proc* proc_ptr
)
{
  MSG_HIGH_DS_0(SM_SUB,"ESM: Received bearer_resource_alloc_abort,moving to abort state");
  proc_ptr->esm_proc_state_ptr = &esm_proc_state_abort;  
} /* esm_proc_active_process_bearer_resource_alloc_abort() */


/*==============================================================================

  FUNCTION:  esm_proc_active_process_timer_expiry_or_lower_layer_failure

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_active_process_timer_expiry_or_lower_layer_failure() goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_active_process_timer_expiry_or_lower_layer_failure() goes here.

    NOTE:
     Since this function is called for either lower layer failure or timer expiry, the treatment is the same
     Hence the timers are stopped before starting the timers as we don't know which caused us to be here.
     So, lower layer failure could result in timer being restarted much earlier than what it would had the
     timer were to run and expire.
     The knowledge of which(lower layer failure or timer expiration) caused us to be here is there in
     BPM. If different treatment needed to be given then, one could feed that into this function and process
     accordingly 
*/
/*============================================================================*/
static boolean esm_proc_active_process_timer_expiry_or_lower_layer_failure
(
  struct esm_bearer_resource_proc  *proc_ptr,
  esm_failure_ind_T                *failure_ind_ptr
)
{
  lte_nas_local_cause_enum_T  local_cause;

  ASSERT(proc_ptr != NULL);
  
  if(failure_ind_ptr != NULL)
  { 
    MSG_LOW_DS_1(SM_SUB,"ESM: Enter esm_proc_active_process_timer_expiry_or_lower_layer_failure - %d ",failure_ind_ptr->emm_cause.local_cause);
  }

  /* No need to retransmit PDN_CONNECTIVITY_REQ for attach failures. Set the 
     attempts counter to MAX */  
  if(((failure_ind_ptr != NULL) && 
     (failure_ind_ptr->emm_cause.valid == TRUE) &&
     ((failure_ind_ptr->emm_cause.local_cause == EMM_ATTACH_FAILED) ||
      (failure_ind_ptr->emm_cause.local_cause == EMM_DETACHED) ||
      (failure_ind_ptr->emm_cause.local_cause == LTE_NAS_EMM_FAILURE_NO_RETRY)
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
      || (failure_ind_ptr->emm_cause.local_cause == LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE)
#endif 
     ))
#ifdef FEATURE_LTE_REL9
     ||
      /* Do not retry for EMC PDN CONN REQ */
     ((esm_get_emc_srv_status() == LTE_NAS_EMC_SRV_PENDING) &&
      (proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                                   PDN_CONNECTIVITY_REQUEST) &&
      (proc_ptr->pending_msg_ptr->esm_out_msg.pdn_connectivity_req.req 
      
                                                     == ESM_EMERGENCY) &&
      
#ifdef FEATURE_DUAL_SIM
      (emm_skip_emc_retry_per_subs(esm_get_sm_as_id()) == TRUE) &&
#else
      (emm_skip_emc_retry() == TRUE) &&
#endif
      ((failure_ind_ptr == NULL) ||
       (failure_ind_ptr->emm_cause.valid == FALSE) ||
       (failure_ind_ptr->emm_cause.local_cause != ESM_REJ_DUE_TO_CONNECTED_STATE)))
#endif
    )
  {
    proc_ptr->retransmit_counter = ESM_BPM_MAX_ATTEMPTS;
  }

  /* Since this function is called for either lower layer failure or timer expiry, the treatment is the same
     Hence the timers are stopped before starting the timers as we don't know which caused us to be here.
     So, lower layer failure could result in timer being restarted much earlier than what it would had the
     timer were to run and expire.
     The knowledge of which(lower layer failure or timer expiration) caused us to be here is there in
     BPM. If different treatment needed to be given then, one could feed that into this function and process
     accordingly */
  if(proc_ptr->retransmit_counter < ESM_BPM_MAX_ATTEMPTS)
  {
    esm_send_retransmission(proc_ptr);
    if((failure_ind_ptr == NULL) || 
       (failure_ind_ptr->emm_cause.valid == FALSE) ||
       (failure_ind_ptr->emm_cause.local_cause != ESM_REJ_DUE_TO_CONNECTED_STATE))
    {
      proc_ptr->retransmit_counter++;
    }
    /* Stay in the same state*/
    proc_ptr->esm_proc_state_ptr = &esm_proc_state_active;  
    return TRUE;
  }
  else
  {
    if(failure_ind_ptr != NULL)
    {
      local_cause = failure_ind_ptr->emm_cause.local_cause;
      esm_proc_send_cm_autonomous_reject(proc_ptr, local_cause,
                                        &failure_ind_ptr->emm_failure_cause);
    }
    else
    {
      /* ESM timer expiry. Notify DS to throttle. */
      local_cause = LTE_NAS_SERVICE_REQ_FAILED;
      esm_proc_send_cm_autonomous_reject(proc_ptr, local_cause, NULL);
    }

    proc_ptr->esm_proc_state_ptr = &esm_proc_state_inactive;  
    return FALSE;
  }
} /* esm_proc_active_process_timer_expiry_or_lower_layer_failure() */

/*==============================================================================

  FUNCTION:  esm_proc_active_process_pdn_connectivity_abort_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_active_process_pdn_connectivity_abort_req() goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_active_process_pdn_connectivity_abort_req() goes here.

*/
/*============================================================================*/
static void esm_proc_active_process_pdn_connectivity_abort_req
(
  struct esm_bearer_resource_proc* proc_ptr
)
{
  MSG_HIGH_DS_0(SM_SUB,"ESM: Received pdn_connectivity_abort_req, moving to abort state");
  proc_ptr->esm_proc_state_ptr = &esm_proc_state_abort;  
} /* esm_proc_active_process_pdn_connectivity_abort_req() */



esm_proc_state_type esm_proc_state_active =
{
  esm_proc_process_pdn_connect_req,
  esm_proc_discard_bearer_resource_alloc_req,
  esm_proc_process_act_default_bearer_ctxt_req,
  esm_proc_active_process_OTA_message,
  esm_proc_active_state,
  esm_proc_active_state_name,
  esm_proc_active_get_state_name,
  esm_proc_active_process_bearer_resource_alloc_abort,
  esm_proc_active_process_timer_expiry_or_lower_layer_failure,
  esm_proc_discard_pdn_disconnect_req,
  esm_proc_active_process_pdn_connectivity_abort_req,
  esm_proc_clean_up,
  esm_proc_discard_bearer_resrc_mod_req
};

#endif /*FEATURE_LTE*/
