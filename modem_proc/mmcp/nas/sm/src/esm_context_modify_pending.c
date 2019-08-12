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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_context_modify_pending.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
07/31/09   MNK     Initial Revision
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


extern bearer_ctrl_mode_T sm_get_bcm_from_pco(const protocol_cfg_options_T *pco);

extern void esm_update_bcm_info(esm_bcm_T* mgr_ptr,byte linked_bearer_id,bearer_ctrl_mode_T bcm);

/*==============================================================================

  FUNCTION:  esm_ctxt_process_modify_bearer_ctxt_accept

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_process_modify_bearer_ctxt_accept() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_process_modify_bearer_ctxt_accept() goes here.
*/
/*============================================================================*/
static void esm_ctxt_process_modify_bearer_ctxt_accept
(
  struct esm_eps_bearer_context                *ctxt_ptr,
  const  esm_modify_bearer_context_accept_T    *msg_ptr,
  struct esm_bcm* mgr_ptr
)
{
  /* Received modify accept from CM/DS, apply the modification data that was 
     saved previously */
  esm_apply_modify_ctxt_data(ctxt_ptr, msg_ptr);

  /* Update context state to active */
  ctxt_ptr->eps_bearer_state_ptr->ctxt_change_state(ctxt_ptr,
                                                    &esm_ctxt_state_active);
  /* Send accept message to NW */
  esm_build_and_send_modify_ctxt_accept(ctxt_ptr, msg_ptr);
  ctxt_ptr->last_msg_sent = MODIFY_EPS_BEARER_CONTEXT_ACCEPT;

  if(msg_ptr->protocol_config_options.valid == TRUE)
  {
    // If in case DS populates, update the BCM from PCO, in case DS modifies
    ctxt_ptr->bcm = sm_get_bcm_from_pco(&(msg_ptr->protocol_config_options));
  }
  else
  {
    // Else, update BCM from the requested PCO from NW
    ctxt_ptr->bcm = sm_get_bcm_from_pco(&(ctxt_ptr->config_options));
  }
	  
  // Update BCM for all the associated contexts 	  
  if (ctxt_ptr->default_bearer_ctxt == TRUE)
  {
    esm_update_bcm_info(mgr_ptr,ctxt_ptr->eps_bearer_id,ctxt_ptr->bcm); 
  }	
  else
  {
    esm_update_bcm_info(mgr_ptr,ctxt_ptr->lbi,ctxt_ptr->bcm); 
  }

  /* ESM LOG PACKET */
  send_esm_ctxt_info_log(ctxt_ptr);

} /* esm_ctxt_process_modify_bearer_ctxt_accept() */


/*==============================================================================

  FUNCTION:  esm_ctxt_process_modify_bearer_ctxt_rej

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_process_modify_bearer_ctxt_rej () does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_process_modify_bearer_ctxt_rej() goes here.

*/
/*============================================================================*/
static void esm_ctxt_process_modify_bearer_ctxt_rej
(
  struct esm_eps_bearer_context             *ctxt_ptr,
  const  esm_modify_bearer_context_rej_T    *msg_ptr
)
{
  /* Update context state back to active */
  ctxt_ptr->eps_bearer_state_ptr->ctxt_change_state(ctxt_ptr,&esm_ctxt_state_active);
  /* Send reject message to NW */
  esm_build_and_send_modify_ctxt_rej(ctxt_ptr, msg_ptr);
  ctxt_ptr->last_msg_sent = MODIFY_EPS_BEARER_CONTEXT_REJECT;

} /* esm_ctxt_process_modify_bearer_ctxt_rej() */

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
  return ESM_BC_MODIFY_PENDING; 
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
  MSG_LOW_DS_0(SM_SUB, "ESM: CONTEXT STATE is CONTEXT MODIFY PENDING \n"); 
} /* context_state_name() */

static char* context_get_state_name
(
  void  /*!< void */
)
{
  return "ESM_BC_MODIFY_PENDING";

} /* context_state_name() */


esm_ctxt_state_type esm_ctxt_state_modify_pending =
{
  esm_ctxt_discard_act_default_bearer_ctxt_req, 
  esm_ctxt_discard_act_dedicated_bearer_ctxt_req,
  esm_ctxt_discard_act_default_bearer_ctxt_accept,
  esm_ctxt_discard_default_bearer_ctxt_reject,
  esm_ctxt_discard_act_dedicated_bearer_ctxt_accept,
  esm_ctxt_discard_dedicated_bearer_ctxt_reject,
  esm_ctxt_discard_deact_bearer_ctxt_req, /* Does it makes sense to get deactivate even before sending 
                                             modify accept/reject to the NW, probably not*/
  esm_ctxt_process_sig_conn_released_ind,
  esm_ctxt_process_active_eps_ind,
  context_state,
  context_state_name,
  context_get_state_name,
  esm_ctxt_discard_esm_failure_ind,
  esm_ctxt_change_state,
  esm_ctxt_discard_modify_bearer_ctxt_req,
  esm_ctxt_process_modify_bearer_ctxt_rej,
  esm_ctxt_process_modify_bearer_ctxt_accept
};


/*==============================================================================

                                UNIT TEST

==============================================================================*/


#endif /*FEATURE_LTE*/
