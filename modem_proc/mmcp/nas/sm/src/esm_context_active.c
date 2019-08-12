/*!
  @file
  esm_context_active.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_context_active.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/05/11   MNK     Corrected handling of ESM_FAILURE_IND from EMM
12/10/09   zren    Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
11/18/09   zren    Updated to fix bearer context state change logging and detach
                   trigger in case of no default bearer context existing 
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
09/25/09   MNK     Fix warnings
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
07/31/09   MNK     Add handling of modify bearer context related events
06/22/09   zren    Updated to use LTE_CM_DRB_REESTABLISH_REJECT_IND for service
                   request procedure failures
05/20/09   MNK     Deactivate the context via the context itself for OTA deactivate request
05/08/09   MNK     Added support for bearer resource allocation procedures
04/28/09   MNK     Added support for EPS_BEARER_STATUS message generation
04/16/09   MNK     More code added to support different scenarios for handling multiple bearers upon attach
04/02/09   MNK     Added support for multiple bearer setup support at the attach 
03/06/09   MNK     Name change of failure reason
03/05/09   MNK     Add service request(comes as ESM_FAILURE_IND) failure handling
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#ifdef FEATURE_LTE
#include "comdef.h"      
#include "esm_bcm.h"
#include "esm_utils.h"

/*==============================================================================

                                FUNCTIONS

==============================================================================*/


/*==============================================================================

  FUNCTION:  esm_ctxt_process_act_default_bearer_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_process_act_default_bearer_ctxt_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_process_act_default_bearer_ctxt_req() goes here.
*/
/*============================================================================*/
static void esm_ctxt_process_act_default_bearer_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */ 
  const lte_nas_esm_act_default_bearer_context_req* msg_ptr   /*!< REQUIRED short parameter description */ 
)
{
  MSG_ERROR_DS_0(SM_SUB, "ESM: Activate Default Bearer context request is not supported currently in CONTEXt_ACTIVE state received by ESM %d %d %d");

  //ctxt_ptr->eps_bearer_state_ptr = &esm_ctxt_state_active;/* Remove when this is supported in the active state*/
  ctxt_ptr->eps_bearer_state_ptr->ctxt_change_state(ctxt_ptr,&esm_ctxt_state_active);/* Remove when this is supported in the active state*/
} /* esm_ctxt_process_act_default_bearer_ctxt_req() */


/*==============================================================================

  FUNCTION:  esm_ctxt_process_act_dedicate_bearer_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_process_act_dedicated_bearer_ctxt_req () does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_process_act_dedicated_bearer_ctxt_req() goes here.

*/
/*============================================================================*/
static void esm_ctxt_process_act_dedicated_bearer_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const lte_nas_esm_act_dedicated_bearer_context_req* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  lte_nas_local_cause_T local_cause;
  esm_cause_T  ota_cause;

  /* Got an activate for an already active context, need to deactivate the context and setup the context again
  TS 24.301, Section 6.4.2.5 (b)*/

  local_cause.local_cause = ESM_ACTIVE_DEDICATED_BEARER_REACTIVATED_BY_NW;
  local_cause.valid = TRUE;

  ota_cause.valid = FALSE;
  esm_build_and_send_deactivate_ind(ctxt_ptr,local_cause,ota_cause);

   
  esm_copy_dedicated_ctxt_data(ctxt_ptr,msg_ptr);
  esm_build_and_send_cm_act_dedicated_ctxt_req (ctxt_ptr);

  //ctxt_ptr->eps_bearer_state_ptr = &esm_ctxt_state_active_pending;
  ctxt_ptr->eps_bearer_state_ptr->ctxt_change_state(ctxt_ptr,&esm_ctxt_state_active_pending);
} /* esm_ctxt_process_act_dedicated_bearer_ctxt_req() */

/*==============================================================================

  FUNCTION:  esm_ctxt_process_deact_bearer_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_process_deact_bearer_ctxt_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_process_deact_bearer_ctxt_req() goes here.
*/
/*============================================================================*/
static void esm_ctxt_process_deact_bearer_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const  lte_nas_esm_deact_eps_bearer_context_req* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  lte_nas_local_cause_T local_cause;
  esm_cause_T ota_cause;
  eps_bearer_id_T bearer_id = 0;

  bearer_id = msg_ptr->lte_nas_esm_hdr.bearer_id;

  local_cause.valid = FALSE;
  /* Cause is mandatory and if we are herei(MSG_LIB validates the message) then, cause should be valid*/
  ota_cause = msg_ptr->esm_cause;
  if(msg_ptr->protocol_conf_opt.valid)
  {
    /* This should set the valid flag as well*/
    ctxt_ptr->config_options = msg_ptr->protocol_conf_opt;
  }
  else
  {
    /* It is ok to overwrite in the context as this context is getting deactivated anyway*/
    ctxt_ptr->config_options.valid = FALSE;
  }
  if(ctxt_ptr->default_bearer_ctxt == TRUE)
  {
    ctxt_ptr->deactivate_associated_dedicated_ctxts_func_ptr(local_cause,ota_cause,bearer_id);
  }
  ctxt_ptr->backoff_timer = msg_ptr->backoff_timer;
  /* Send OTA message first */
  esm_build_and_send_deactivate_bearer_context_accept(bearer_id,NULL);
  /* Send message to CM/DS */
  esm_build_and_send_deactivate_ind(ctxt_ptr,local_cause,ota_cause);
  esm_bcm_reset_ctxt(ctxt_ptr);

} /* esm_ctxt_process_deact_dedicated_bearer_ctxt_req() */

/*==============================================================================

  FUNCTION:  context_state

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what context_state() does goes here.

    @detail
    An OPTIONAL detailed description of context_state() goes here.
*/
/*============================================================================*/
static esm_bc_state_T context_state
(
  void   /*!< void */
)
{
  return ESM_BC_ACTIVE; 
} /* context_state() */

/*==============================================================================

  FUNCTION:  context_state_name

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what context_state_name() does goes here.

    @detail
    An OPTIONAL detailed description of context_state_name() goes here.
*/
/*============================================================================*/
static void context_state_name
(
  void  /*!< void */
)
{
  MSG_LOW_DS_0(SM_SUB, "ESM: CONTEXT STATE is CONTEXT ACTIVE \n"); 
} /* context_state_name() */

static char* context_get_state_name
(
  void  /*!< void */
)
{
  return "ESM_BC_ACTIVE";

} /* context_state_name() */
/*==============================================================================

  FUNCTION:  esm_ctxt_active_process_esm_failure_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_active_process_esm_failure_ind() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_active_process_esm_failure_ind() goes here.

*/
/*============================================================================*/
static void esm_ctxt_active_process_esm_failure_ind
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_failure_ind_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{

  /* Only in the case of dormancy does ESM need to respond back to CM/DS. Otherwise silently swallow 
  the failure*/
  if(msg_ptr->emm_cause.valid)
  {
    if((msg_ptr->emm_cause.local_cause == LTE_NAS_SERVICE_REQ_FAILED) ||
       (msg_ptr->emm_cause.local_cause == 
                                     LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE) ||
       (msg_ptr->emm_cause.local_cause == 
                                       LTE_NAS_SERVICE_REQ_FAILED_DS_DISALLOW)
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
       || (msg_ptr->emm_cause.local_cause == LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE)
#endif 
       )
    {
      if(ctxt_ptr->last_msg_sent == ESM_LAST_UNDEFINED)
      {
        esm_build_and_send_cm_drb_reestablish_reject_ind(ctxt_ptr,msg_ptr,ESM_INVALID_CON_ID);
      }
      else
      {
        MSG_HIGH_DS_1(SM_SUB, "ESM:Failed to send message with ID = %x No Action is taken and the bearer is kept",
                      ctxt_ptr->last_msg_sent);
      }
    }
  }
  else
  {
    switch (ctxt_ptr->last_msg_sent)
    {
      /* Not resetting last_msg_sent here as we don't do it in the case of success also*/
      /* Nothing to do for REJECT*/
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT:
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT:
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT:
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT:
      MSG_HIGH_DS_1(SM_SUB, "ESM:FAILED to send message %u. No Action is taken and the bearer is kept",ctxt_ptr->last_msg_sent);
      break;
    default:
      MSG_ERROR_DS_0(SM_SUB, "ESM: Error, received ESM_FAILURE_IND from EMM for the ctxt that didn't send any valid message");
      break;
    }
  }
} /* esm_ctxt_active_process_deact_default_bearer_ctxt_req() */

/*==============================================================================

  FUNCTION:  esm_ctxt_active_change_state        

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_active_change_state() does goes here.

    @detail
    An OPTIONAL detailed description of  esm_ctxt_active_change_state() goes here.

*/
/*============================================================================*/
/*void esm_ctxt_active_change_state         
(
  struct esm_eps_bearer_context* ctxt_ptr,
  esm_ctxt_state_ptr_type state_ptr
)
{
  ctxt_ptr->eps_bearer_state_ptr = state;
  if( (state_ptr->state() == ESM_BC_ACTIVE) || (state_ptr-_state() == ESM_BC_INACTIVE) )
  {
    ctxt_ptr->context_status_update_func_ptr();
  }
}*/ /*esm_ctxt_active_change_state() */

/*==============================================================================

  FUNCTION:  esm_ctxt_process_modify_bearer_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_process_modify_bearer_ctxt_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_process_modify_bearer_ctxt_req() goes here.

*/
/*============================================================================*/
static void esm_ctxt_process_modify_bearer_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr,
  const lte_nas_esm_modify_eps_bearer_ctxt_req* msg_ptr
)
{
  ctxt_ptr->eps_bearer_state_ptr->state_name();

  /* Save the modification data first w/o applying */
  esm_save_modify_ctxt_data(ctxt_ptr, msg_ptr);

  /* Send MODIFY EPS BEARER CONTEXT REQUEST to CM */
  esm_build_and_send_cm_modify_ctxt_req(ctxt_ptr);

  /* Update context state to modify pending */
  ctxt_ptr->eps_bearer_state_ptr->ctxt_change_state(ctxt_ptr,
                                               &esm_ctxt_state_modify_pending);

} /* esm_ctxt_process_modify_bearer_ctxt_req */

esm_ctxt_state_type esm_ctxt_state_active =
{
  esm_ctxt_process_act_default_bearer_ctxt_req,/* if the context is already active and activate is received again. 
                                                  Deactivate and restart the context activation as per the spec*/
  esm_ctxt_process_act_dedicated_bearer_ctxt_req,
  esm_ctxt_discard_act_default_bearer_ctxt_accept,
  esm_ctxt_discard_default_bearer_ctxt_reject,
  esm_ctxt_discard_act_dedicated_bearer_ctxt_accept,
  esm_ctxt_discard_dedicated_bearer_ctxt_reject,
  esm_ctxt_process_deact_bearer_ctxt_req, 
  esm_ctxt_process_sig_conn_released_ind,
  esm_ctxt_process_active_eps_ind,
  context_state,
  context_state_name,
  context_get_state_name,
  esm_ctxt_active_process_esm_failure_ind,
  esm_ctxt_change_state,
  esm_ctxt_process_modify_bearer_ctxt_req,
  esm_ctxt_discard_modify_bearer_ctxt_rej,
  esm_ctxt_discard_modify_bearer_ctxt_accept
};


/*==============================================================================

                                UNIT TEST

==============================================================================*/


#endif /*FEATURE_LTE*/
