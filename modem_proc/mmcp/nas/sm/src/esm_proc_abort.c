
/*!
  @file
  esm_state_abort.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_proc_abort.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/03/10   zren    Added ESM new timer machanism
01/21/10   zren    Added EMM/ESM STATUS feature
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
09/25/09   MNK     Fix warnings
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/01/09   MNK     Added handling of detach_ind from EMM, as we support bearer initiated procedures
08/18/09   MNK     CM-NAS interface changes
08/14/09   MNK     Add Support for PDN disconnect request
05/11/09   MNK     Added stop timer when expected responses received in abort state
05/08/09   MNK     Added support for bearer resource allocation procedures
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
/*==============================================================================

                                FUNCTIONS

==============================================================================*/

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
static void  esm_proc_process_act_default_bearer_ctxt_req
(
   struct esm_bearer_resource_proc* proc_ptr, const byte* msg_ptr  /*!< REQUIRED short parameter description */
)
{
  proc_ptr->esm_proc_state_ptr = &esm_proc_state_inactive;
} /* esm_proc_process_act_default_bearer_ctxt_req() */


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*==============================================================================

  FUNCTION:  esm_proc_abort_process_OTA_message

==============================================================================*/
/*!
    @brief
    This function processes OTA ESM messages received in aborting procedure 
    state.

    Note: Only PDN Connectivity and Bearer Resource Allocation procedure can be
    aborted.
*/
/*============================================================================*/
static boolean esm_proc_abort_process_OTA_message
(
  struct esm_bearer_resource_proc   *proc_ptr,
  lte_nas_esm_incoming_msg_type     *data_ptr
)
{
  lte_nas_esm_hdr_type     *esm_hdr_ptr = NULL;
  boolean                   send_abort_rsp = FALSE;

  ASSERT(proc_ptr != NULL);
  ASSERT(proc_ptr->pending_msg_ptr != NULL);
  ASSERT(data_ptr != NULL);

  switch(data_ptr->nas_hdr.msg_id)
  {
    /* Case for PDN CONNECTIVITY */
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
      if(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                                      PDN_CONNECTIVITY_REQUEST)
      {
        esm_stop_timer(ESM_TIMER_T3482, proc_ptr);
        esm_bpm_build_and_send_autonomous_reject(data_ptr, 
                                                 NAS_ESM_ACTIVATION_REJECTED);
        send_abort_rsp = TRUE;
      }
      else
      {
        esm_send_esm_status(
                         &data_ptr->default_bearer_context_req.lte_nas_esm_hdr,
                          NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
      }
      break;
    /* Case for PDN CONNECTIVITY */
    case PDN_CONNECTIVITY_REJECT:
      if(data_ptr->pdn_connectivity_rej.lte_nas_esm_hdr.bearer_id == 
                                                          UNASSIGNED_BEARER_ID)
      {
        if(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                                      PDN_CONNECTIVITY_REQUEST)
        {
          esm_stop_timer(ESM_TIMER_T3482, proc_ptr);
          send_abort_rsp = TRUE;
        }
        else
        {
          esm_send_esm_status(&data_ptr->pdn_connectivity_rej.lte_nas_esm_hdr,
                               NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
        }
      }
      else
      {
        MSG_HIGH_DS_1(SM_SUB,"ESM: Ignoring PDN_CONNECTIVITY_REJECT w/ bearer id: %d", 
                      data_ptr->pdn_connectivity_rej.lte_nas_esm_hdr.bearer_id);
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
      }
      break;
    /* Case for BEARER RESOURCE ALLOCATION */
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST:
      if(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                            BEARER_RESOURCE_ALLOCATION_REQUEST)
      {
        esm_stop_timer(ESM_TIMER_T3480, proc_ptr);
        esm_bpm_build_and_send_autonomous_reject(data_ptr, 
                                                 NAS_ESM_ACTIVATION_REJECTED);
        send_abort_rsp = TRUE;
      }
      else
      {
        esm_send_esm_status(
                       &data_ptr->dedicated_bearer_context_req.lte_nas_esm_hdr,
                        NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
      }
      break;
    /* Case for BEARER RESOURCE ALLOCATION */
    case BEARER_RESOURCE_ALLOCATION_REJECT:
      if(data_ptr->pdn_connectivity_rej.lte_nas_esm_hdr.bearer_id == 
                                                          UNASSIGNED_BEARER_ID)
      { 
        if(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                            BEARER_RESOURCE_ALLOCATION_REQUEST)
        {
          esm_stop_timer(ESM_TIMER_T3480, proc_ptr);
          send_abort_rsp = TRUE;
        }
        else
        {
          esm_send_esm_status(
                          &data_ptr->bearer_resource_alloc_rej.lte_nas_esm_hdr,
                           NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
        }
      }
      else
      {
        MSG_HIGH_DS_1(SM_SUB,"ESM: Ignoring BEARER_RESRC_ALLOCATION_REJ w/ bearer id: %d", 
                      data_ptr->bearer_resource_alloc_rej.lte_nas_esm_hdr.bearer_id);
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
      }
      break;
    /* Case for BEARER RESOURCE ALLOCATION */
    case MODIFY_EPS_BEARER_CONTEXT_REQUEST:
      if(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                            BEARER_RESOURCE_ALLOCATION_REQUEST)
      {
        esm_stop_timer(ESM_TIMER_T3480, proc_ptr);
        esm_bpm_build_and_send_autonomous_reject(data_ptr, 
                                               NAS_ESM_INSUFFICIENT_RESOURCES);
        send_abort_rsp = TRUE;
      }
      else
      {
        esm_send_esm_status(
                         &data_ptr->modify_eps_bearer_ctxt_req.lte_nas_esm_hdr,
                          NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
      }
      break;
    default:
      
      esm_hdr_ptr = esm_get_esm_hdr_from_esm_incoming_msg(data_ptr);
      ASSERT(esm_hdr_ptr != NULL);
      /* Other messages shall not be received. Report protocol state error. */
      esm_send_esm_status(esm_hdr_ptr, 
                          NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE);
      break;
  }

  if(send_abort_rsp == TRUE)
  {    
    switch(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id)
    {
      case BEARER_RESOURCE_ALLOCATION_REQUEST:
        esm_build_and_send_bearer_resource_alloc_abort_rsp(proc_ptr->sdf_id, 
                                                           TRUE);
        break;
      case PDN_CONNECTIVITY_REQUEST:
#ifdef FEATURE_LTE_REL9 
        esm_pdn_conn_failure_update_emc_srv_status(proc_ptr);
#endif
        esm_build_and_send_pdn_connectivity_abort_rsp(proc_ptr->sdf_id,
                                                      TRUE);
        break;
      default:
        MSG_ERROR_DS_1(SM_SUB,"ESM: Invalid procedure to be aborted - %u",
                       proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id);
    }
  }

  /* Return always FALSE so that context manager doesn't process this message*/
  return FALSE;

} /* esm_proc_abort_process_OTA_message() */

/*==============================================================================

  FUNCTION:  esm_proc_abort_state

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_abort_state() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_abort_state() goes here.

*/
/*============================================================================*/
static esm_bp_state_T esm_proc_abort_state ( void )
{
  return ESM_BP_ABORT;
} /* esm_proc_abort_state() */

/*==============================================================================

  FUNCTION:  esm_proc_abort_state_name

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_abort_state_name() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_abort_state_name() goes here.
*/
/*============================================================================*/
static void esm_proc_abort_state_name ( void )
{

} /* esm_proc_abort_state_name() */

/*==============================================================================

  FUNCTION:  esm_proc_abort_state_name

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_abort_process_timer_expiry_or_lower_layer_failure() goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_abort_process_timer_expiry_or_lower_layer_failure()goes here.
*/
/*============================================================================*/
static boolean esm_proc_abort_process_timer_expiry_or_lower_layer_failure
(
  struct esm_bearer_resource_proc  *proc_ptr,
  esm_failure_ind_T                *failure_ind_ptr
)
{
  ASSERT(proc_ptr != NULL);

  switch(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id)
  {
    case BEARER_RESOURCE_ALLOCATION_REQUEST:
      esm_build_and_send_bearer_resource_alloc_abort_rsp(proc_ptr->sdf_id, 
                                                         TRUE);
      break;
    case PDN_CONNECTIVITY_REQUEST:
#ifdef FEATURE_LTE_REL9 
      esm_pdn_conn_failure_update_emc_srv_status(proc_ptr);
#endif
      esm_build_and_send_pdn_connectivity_abort_rsp(proc_ptr->sdf_id,
                                                    TRUE);
      break;
    default:
      MSG_ERROR_DS_1(SM_SUB,"ESM: Invalid proc w/ msg %d to be aborted upon TE or LLF",
                     proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id);
  }

  proc_ptr->esm_proc_state_ptr = &esm_proc_state_inactive;
  return FALSE;
} /* esm_proc_abort_process_timer_expiry_or_lower_layer_failure() */


static char* esm_proc_abort_get_state_name(void)
{
  return "ESM_BP_ABORT";
}


esm_proc_state_type esm_proc_state_abort =
{
  esm_proc_discard_pdn_connect_req,
  esm_proc_discard_bearer_resource_alloc_req,
  esm_proc_process_act_default_bearer_ctxt_req,
  esm_proc_abort_process_OTA_message,
  esm_proc_abort_state,
  esm_proc_abort_state_name,
  esm_proc_abort_get_state_name,
  esm_proc_discard_bearer_resource_abort_req,
  esm_proc_abort_process_timer_expiry_or_lower_layer_failure,
  esm_proc_discard_pdn_disconnect_req,
  esm_proc_discard_pdn_connectivity_abort_req,
  esm_proc_clean_up,
  esm_proc_discard_bearer_resrc_mod_req
};

#endif /*FEATURE_LTE*/
