/*!
  @file
  esm_proc_state.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_proc_state.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/17/12   cogolt  Modified input parameters esm_build_and_send_pdn_connectivity_reject_ind to 
                   support Rel-10 ESM backoff timer (T3396) requirement
02/17/10   zren    Added support to always deact bearer upon DS request in UE 
                   initiated modification procedure
02/03/10   zren    Added ESM new timer machanism
01/21/10   zren    Added EMM/ESM STATUS feature
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/14/09   zren    Added bearer ID and ESM cause check for PDN DISCONNECT 
                   REQ/REJ messages
09/01/09   MNK     Added handling of detach_ind from EMM, as we support bearer initiated procedures
08/18/09   MNK     CM-NAS interface changes
08/14/09   MNK     Add Support for PDN disconnect request
06/02/09   MNK     Added additional criteria of mathcing the LBI to match the UE initiated request  
05/21/09   MNK     Added support to reply back to CM/DS in case the NW sent bad messages as a response to UE initiated request
05/14/09   MNK     Added stop timer when max attempts expire
05/08/09   MNK     Added support for bearer resource allocation procedures
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#ifdef FEATURE_LTE
#include "comdef.h"
#include "esm_bpm.h"
#include "esm_proc_state.h"
#include "esm_utils.h"
#include "amssassert.h"

/*==============================================================================

                                FUNCTIONS

==============================================================================*/
/*==============================================================================

  FUNCTION:  esm_proc_discard_act_default_bearer_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_discard_act_default_bearer_ctxt_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_discard_act_default_bearer_ctxt_req() goes here.
*/
/*============================================================================*/
void  esm_proc_discard_act_default_bearer_ctxt_req
(
  struct esm_bearer_resource_proc* proc_ptr, 
  const byte* msg_ptr     /*!< REQUIRED short parameter description */
)
{
  MSG_ERROR_DS_0(SM_SUB, "ESM: Unexpected cmd act_default_bearer_ctxt_req received by ESM");
} /* esm_proc_discard_act_default_bearer_ctxt_req() */



/*==============================================================================

  FUNCTION:  esm_proc_discard_act_dedicated_bearer_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_discard_act_dedicated_bearer_ctxt_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_discard_act_dedicated_bearer_ctxt_req() goes here.
*/
/*============================================================================*/
void  esm_proc_discard_act_dedicated_bearer_ctxt_req
(
  const struct esm_bearer_resource_proc* proc_ptr, const byte* msg_ptr    /*!< REQUIRED short parameter description */
)
{
  MSG_ERROR_DS_0(SM_SUB, "ESM: Unexpected cmd act_dedicated_bearer_ctxt_req received by ESM");
} /* esm_proc_discard_act_dedicated_bearer_ctxt_req() */


/*==============================================================================

  FUNCTION:  esm_proc_discard_pdn_connectivity_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_discard_pdn_connectivity_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_discard_pdn_connectivity_req() goes here.
*/
/*============================================================================*/
void    esm_proc_discard_pdn_connect_req
(
  struct esm_bearer_resource_proc* proc_ptr, const esm_pdn_connectivity_req_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  MSG_HIGH_DS_0(SM_SUB, "ESM: Discarding cmd pdn Connectivity Req received by ESM");
  esm_build_and_send_pdn_connectivity_abort_rsp(proc_ptr->sdf_id,TRUE); 
} /* esm_proc_discard_pdn_connectivity_req() */


/*==============================================================================

  FUNCTION:  esm_proc_discard_bearer_resource_alloc_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_discard_bearer_resource_alloc_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_discard_bearer_resource_alloc_req() goes here.
*/
/*============================================================================*/
void    esm_proc_discard_bearer_resource_alloc_req
(
  struct esm_bearer_resource_proc* proc_ptr, 
  const esm_bearer_resource_alloc_req_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  MSG_ERROR_DS_0(SM_SUB, "ESM: Unexpected cmd bearer resource allocation request received by ESM");
}

/*==============================================================================

  FUNCTION:  esm_proc_discard_bearer_resource_abort_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_discard_bearer_resource_abort_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_discard_bearer_resource_abort_req() goes here.
*/
/*============================================================================*/
void    esm_proc_discard_bearer_resource_abort_req
(
  struct esm_bearer_resource_proc* proc_ptr 
)
{
  MSG_ERROR_DS_0(SM_SUB, "ESM: Unexpected cmd bearer resource abor request received by ESM");
}

/*==============================================================================

  FUNCTION:  esm_proc_discard_timer_expiry_or_lower_layer_failure

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_discard_timer_expiry_or_lower_layer_failure() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_discard_timer_expiry_or_lower_layer_failure() goes here.
*/
/*============================================================================*/
boolean esm_proc_discard_timer_expiry_or_lower_layer_failure
(
  struct esm_bearer_resource_proc   *proc_ptr,
  esm_failure_ind_T                 *failure_ind_ptr
)
{
  MSG_HIGH_DS_2(SM_SUB,"ESM: Discard timer expiry or lower layer failure - Proc %d, State %d",
                proc_ptr->instance_id, proc_ptr->esm_proc_state_ptr->state());
  return FALSE;
} /* esm_proc_discard_timer_expiry_or_lower_layer_failure */

/*==============================================================================

  FUNCTION:  esm_proc_send_cm_autonomous_reject

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_send_cm_autonomous_reject() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_send_cm_autonomous_reject() goes here.
*/
/*============================================================================*/
void esm_proc_send_cm_autonomous_reject
(
  struct esm_bearer_resource_proc  *proc_ptr,
  lte_nas_local_cause_enum_T        nas_local_cause,
  emm_failure_type                 *emm_failure_ptr
)
{
  lte_nas_message_id  msg_id;
  word       sdf_id;
  byte       lbi;
  lte_nas_local_cause_T  local_cause;
  esm_cause_T  ota_cause;
  
  ASSERT(proc_ptr != NULL);
  ASSERT(proc_ptr->pending_msg_ptr != NULL);

  msg_id = proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id;
  sdf_id = proc_ptr->sdf_id;
  lbi = proc_ptr->lbi;

  local_cause.valid = FALSE;
  ota_cause.valid   = FALSE;
 
  switch(msg_id)
  {
    case BEARER_RESOURCE_ALLOCATION_REQUEST:
      esm_stop_timer(ESM_TIMER_T3480, proc_ptr);
      esm_build_and_send_cm_bearer_resource_allocation_reject(sdf_id, NULL,
                                                          nas_local_cause,lbi);
      break;
    case BEARER_RESOURCE_MODIFICATION_REQUEST:
      esm_stop_timer(ESM_TIMER_T3481, proc_ptr);
      esm_build_and_send_cm_bearer_resrc_mod_reject(sdf_id, 
                                                    NULL,
                                                    nas_local_cause, 
                                                    lbi);
      if(proc_ptr->retransmit_counter == ESM_BPM_MAX_ATTEMPTS)
      {
        /* 6.5.4.5 Abnormal case a */
        if(proc_ptr->pending_msg_ptr->esm_out_msg.bearer_resource_modifi_req.\
             tft.operation_code == TFT_OP_DEL_EXISTING_TFT)
        {
          /* Locally deactivate contexts */
          proc_ptr->request_esm_to_deactivate_ctxts_func_ptr(local_cause, 
                                                             ota_cause,
                                                             proc_ptr->lbi);
        }
      }
      else
      {
        /* Check if need to deact the bearer upon DS request */
        esm_chk_ds_explicit_deact(proc_ptr);
      }
      break;
    case PDN_CONNECTIVITY_REQUEST:
      esm_stop_timer(ESM_TIMER_T3482, proc_ptr);
#ifdef FEATURE_LTE_REL9 
      esm_pdn_conn_failure_update_emc_srv_status(proc_ptr);
#endif
      esm_build_and_send_pdn_connectivity_reject_ind(proc_ptr,
                                                     sdf_id, 
                                                     NULL,
                                                     nas_local_cause,
                                                     emm_failure_ptr);
      break;
    case PDN_DISCONNECT_REQUEST:
      esm_stop_timer(ESM_TIMER_T3492, proc_ptr);
      /* Deactivate all bearer contexts in this PDN connection locally */
      proc_ptr->request_esm_to_deactivate_ctxts_func_ptr(local_cause,
                                                         ota_cause,
                                                         proc_ptr->lbi);
      break;
    default:
      /* Other messages which are responses to the N/W requests like default bearer response, dedicated bearer response etc..
         are NOT needed to be retransmistted. What should be the UE behaviour for non-retransmission puproses is under
         disicussion with the STANDARDS folks.
      */
      MSG_ERROR_DS_1(SM_SUB, "ESM: Unexpected message %d received", msg_id);
      break;
  }
} /* esm_proc_send_cm_autonomous_reject */

/*==============================================================================

  FUNCTION:  esm_proc_discard_pdn_disconnect_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_discard_pdn_disconnect_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_discard_pdn_disconnect_req() goes here.
*/
/*============================================================================*/
void esm_proc_discard_pdn_disconnect_req
(
  struct esm_bearer_resource_proc* proc_ptr,
  esm_pdn_disconnect_req_T* dat_ptr
)
{
  MSG_ERROR_DS_0(SM_SUB, "ESM: Unexpected message pdn disconnect received");
}

/*==============================================================================

  FUNCTION:  esm_proc_discard_pdn_connectivity_abort_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_discard_pdn_connectivity_abort_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_discard_pdn_connectivity_abort_req() goes here.
*/
/*============================================================================*/
void    esm_proc_discard_pdn_connectivity_abort_req
(
  struct esm_bearer_resource_proc* proc_ptr 
)
{
  MSG_ERROR_DS_0(SM_SUB, "ESM: Unexpected esm_proc_discard_pdn_connectivity_abort_req received by ESM");
}

/*==============================================================================

  FUNCTION:  esm_proc_discard_detach_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_discard_detach_ind() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_discard_detach_ind() goes here.
*/
/*============================================================================*/
void    esm_proc_discard_detach_ind
(
  struct esm_bearer_resource_proc  *proc_ptr,
  lte_nas_local_cause_T             local_cause
)
{
  MSG_ERROR_DS_0(SM_SUB, "ESM: Unexpected esm_proc_discard_detach_ind received by ESM");
}

/*==============================================================================

  FUNCTION:  esm_proc_discard_bearer_resrc_mod_req

==============================================================================*/
/*!
    @brief
    This function discard bearer resource modification request

    @detail

*/
/*============================================================================*/
void    esm_proc_discard_bearer_resrc_mod_req
(
  struct esm_bearer_resource_proc                 *proc_ptr, 
  const  esm_bearer_resource_modification_req_T   *msg_ptr
)
{
  MSG_ERROR_DS_0(SM_SUB, "ESM: Unexpected bearer resource modification request received");
} /* esm_proc_discard_bearer_resrc_mod_req */

/*==============================================================================

  FUNCTION:  esm_proc_clean_up

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_clean_up() goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_clean_up() goes here.

*/
/*============================================================================*/
void esm_proc_clean_up
(
  struct esm_bearer_resource_proc  *proc_ptr,
  lte_nas_local_cause_T             local_cause
)
{
  esm_cause_T                 ota_cause;
  boolean result = FALSE;

  ASSERT(proc_ptr != NULL);
  ASSERT(proc_ptr->pending_msg_ptr != NULL);

  MSG_HIGH_DS_0(SM_SUB,"ESM: Cleaning up ESM procs");

  switch(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id)
  {
    case BEARER_RESOURCE_ALLOCATION_REQUEST:
      esm_stop_timer(ESM_TIMER_T3480, proc_ptr);
      if(proc_ptr->esm_proc_state_ptr->state() == ESM_BP_ABORT)
      {
        result = TRUE;
      }
      esm_build_and_send_bearer_resource_alloc_abort_rsp(proc_ptr->sdf_id,result);
      break;
    case PDN_CONNECTIVITY_REQUEST:
      esm_stop_timer(ESM_TIMER_T3482, proc_ptr);
      if(proc_ptr->esm_proc_state_ptr->state() == ESM_BP_ABORT)
      {
        result = TRUE;
      }
#ifdef FEATURE_LTE_REL9 
      esm_pdn_conn_failure_update_emc_srv_status(proc_ptr);
#endif
      esm_build_and_send_pdn_connectivity_abort_rsp(proc_ptr->sdf_id,result);
      break;
    case PDN_DISCONNECT_REQUEST:
      esm_stop_timer(ESM_TIMER_T3492, proc_ptr);
      ota_cause.valid = FALSE;
      /* Locally deactivate contexts */
      proc_ptr->request_esm_to_deactivate_ctxts_func_ptr(local_cause,
                                                         ota_cause,
                                                         proc_ptr->lbi);
      break;
    case BEARER_RESOURCE_MODIFICATION_REQUEST:
      esm_stop_timer(ESM_TIMER_T3481, proc_ptr);
      if((proc_ptr->esm_local_cause.valid == TRUE) &&
         (proc_ptr->esm_local_cause.local_cause == DS_EXPLICIT_DEACT))
      {
        /* Deact the bearer upon DS request */
        esm_chk_ds_explicit_deact(proc_ptr);
      }
      else
      {
        esm_build_and_send_cm_bearer_resrc_mod_reject(proc_ptr->sdf_id, 
                                                      NULL,
                                                      local_cause.local_cause,
                                                      proc_ptr->lbi);
      }
      break;
    default:
      MSG_ERROR_DS_0(SM_SUB,"ESM: This shouldn't happen. Check out for a bug");
      break;
  }
} /* esm_proc_clean_up() */

/*==============================================================================

  FUNCTION:     esm_proc_process_pdn_connectivity_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_process_pdn_connect_req () does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_process_pdn_connect_req goes here.

*/
/*============================================================================*/
void  esm_proc_process_pdn_connect_req
(
  struct esm_bearer_resource_proc        *proc_ptr, 
  const esm_pdn_connectivity_req_T       *msg_ptr
)
{
  boolean status = FALSE;
  /*
       1. Check the message type to figure out what message is received
       2. Call the apprpriate static handler function for that message
       3. Return the proper state pointer back to the caller.
  */
  proc_ptr->sdf_id = msg_ptr->esm_hdr.sdf_id;
  esm_build_and_send_pdn_connectivity_req_msg(proc_ptr,msg_ptr);
  /* If not combined in attach procedure, start timer 3482 */
  if(proc_ptr->chk_on_attach_def_bc_setup_ptr() == TRUE)
  {
    status = esm_start_timer(ESM_TIMER_T3482,proc_ptr);
    ASSERT(status == TRUE);
  }
  proc_ptr->esm_proc_state_ptr = &esm_proc_state_active;

} /* esm_proc_process_pdn_connect_req  */

#endif /*FEATURE_LTE*/
