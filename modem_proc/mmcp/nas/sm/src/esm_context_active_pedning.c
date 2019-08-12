/*!
  @file
  esm_context_active_pedning.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_context_active_pedning.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/26/10   MNK     Add more i-RAT(GSM) support
11/18/09   zren    Updated to fix bearer context state change logging and detach
                   trigger in case of no default bearer context existing 
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
08/14/09   MNK     Add sending of deactivate request to CM when DS sends reject for 
                   either default or dedicated bearer activation request
07/31/09   MNK     Add handling of modify bearer context related events
05/20/09   MNK     Deactivate the context via the context itself for OTA deactivate request
04/28/09   MNK     Added support for EPS_BEARER_STATUS message generation
04/02/09   MNK     Added support for multiple bearer setup support at the attach 
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#ifdef FEATURE_LTE
#include "comdef.h"
#include "esm_bcm.h"
#include "esm_context_state.h"
#include "esm_utils.h"

extern void esm_build_and_send_act_def_ctxt_reject 
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_act_default_bearer_context_rej_T* msg_ptr    /*!< REQUIRED short parameter description */
);

extern void esm_build_and_send_act_def_ctxt_accept
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_act_default_bearer_context_accept_T* msg_ptr    /*!< REQUIRED short parameter description */
);


extern void esm_build_and_send_cm_drb_setup_ind
(
  const esm_eps_bearer_context_T* ctxt_ptr   /*!< REQUIRED short parameter description */
);


/*==============================================================================

                                FUNCTIONS

==============================================================================*/

/*==============================================================================

  FUNCTION:  esm_ctxt_process_act_default_bearer_ctxt_accept

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_process_act_default_bearer_ctxt_accept() does goes here.

    @detail
    Builds and sends activate default bearer context accept
    Changes the context state to ACTIVE
*/
/*============================================================================*/
static void  esm_ctxt_process_act_default_bearer_ctxt_accept
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_act_default_bearer_context_accept_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  /* Update i-rat params to the context*/
  ctxt_ptr->h_comp = msg_ptr->h_comp;
  ctxt_ptr->d_comp = msg_ptr->d_comp;

#ifdef FEATURE_LTE_REL9
  if(emm_get_3gpp_rel_ver_per_sub((sys_modem_as_id_e_type)sm_as_id) >= LTE_3GPP_REL9)
  {
    ctxt_ptr->emc_bc = msg_ptr->emc_bc;
  }
#endif

  esm_build_and_send_act_def_ctxt_accept(ctxt_ptr,msg_ptr);
  ctxt_ptr->last_msg_sent = ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT;
  //ctxt_ptr->eps_bearer_state_ptr = &esm_ctxt_state_active;
  ctxt_ptr->eps_bearer_state_ptr->ctxt_change_state(ctxt_ptr,&esm_ctxt_state_active);
  ctxt_ptr->eps_bearer_state_ptr->state_name();

  if(ctxt_ptr->unclaimed_rb_id != ESM_UNDEFINED_RB_ID)
  {
    ctxt_ptr->rb_id = ctxt_ptr->unclaimed_rb_id;
    ctxt_ptr->unclaimed_rb_id = ESM_UNDEFINED_RB_ID;
    send_esm_ctxt_info_log(ctxt_ptr);
    esm_build_and_send_cm_drb_setup_ind(ctxt_ptr);
  }

} /* esm_ctxt_process_act_default_bearer_ctxt_accept() */


/*==============================================================================

  FUNCTION:  esm_ctxt_process_act_dedicated_bearer_ctxt_accept

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_process_act_dedicated_bearer_ctxt_accept() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_process_act_dedicated_bearer_ctxt_accept() goes here.

*/
/*============================================================================*/
static void  esm_ctxt_process_act_dedicated_bearer_ctxt_accept
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_act_dedicated_bearer_context_accept_T* msg_ptr,   /*!< REQUIRED short parameter description */
  struct esm_bcm* mgr_ptr
)
{
  byte index;
  /* Update i-rat params to the context*/
  ctxt_ptr->h_comp = msg_ptr->h_comp;
  ctxt_ptr->d_comp = msg_ptr->d_comp;

#ifdef FEATURE_LTE_REL9
  if(emm_get_3gpp_rel_ver_per_sub((sys_modem_as_id_e_type)sm_as_id) >= LTE_3GPP_REL9)
  {
    ctxt_ptr->emc_bc = msg_ptr->emc_bc;
  }
#endif

  esm_build_and_send_act_dedicated_ctxt_accept(ctxt_ptr,msg_ptr);
  ctxt_ptr->last_msg_sent = ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT;
  //ctxt_ptr->eps_bearer_state_ptr = &esm_ctxt_state_active;
  ctxt_ptr->eps_bearer_state_ptr->ctxt_change_state(ctxt_ptr,&esm_ctxt_state_active);
  ctxt_ptr->eps_bearer_state_ptr->state_name();

  for (index = 0; index < ESM_MAX_EPS_BEARERS ; index++)
  {
    if ((mgr_ptr->esm_bc[index] != NULL)&&
        (mgr_ptr->esm_bc[index]->default_bearer_ctxt == TRUE)&&
        (mgr_ptr->esm_bc[index]->eps_bearer_id == ctxt_ptr->lbi)&&
        (mgr_ptr->esm_bc[index]->bcm != BCM_MS_ONLY))
    {
      ctxt_ptr->bcm = mgr_ptr->esm_bc[index]->bcm;
	}
  }

  if(ctxt_ptr->unclaimed_rb_id != ESM_UNDEFINED_RB_ID)
  {
    ctxt_ptr->rb_id = ctxt_ptr->unclaimed_rb_id;
    ctxt_ptr->unclaimed_rb_id = ESM_UNDEFINED_RB_ID;
    send_esm_ctxt_info_log(ctxt_ptr);
    esm_build_and_send_cm_drb_setup_ind(ctxt_ptr);
  }

} /* esm_ctxt_process_act_dedicated_bearer_ctxt_accept() */

/*==============================================================================

  FUNCTION:  ctxt_state

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what ctxt_state() does goes here.

    @detail
    An OPTIONAL detailed description of ctxt_state() goes here.
*/
/*============================================================================*/
static esm_bc_state_T context_state
(
  void   /*!< void */
)
{
  return ESM_BC_ACTIVE_PENDING;
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
  MSG_LOW_DS_0(SM_SUB, "ESM: CONTEXT STATE is CONTEXT ACTIVE PENDING\n"); 
} /* context_state_name() */

 /*==============================================================================
 
   FUNCTION:  esm_ctxt_act_default_bearer_ctxt_reject 
 
 ==============================================================================*/
 /*!
     @brief
     A REQUIRED brief one-sentence description of what esm_ctxt_act_default_bearer_ctxt_reject () does goes here.
 
     @detail
     Builds and sends default bearer context reject
     Changes state to INACTIVE
 
 */
 /*============================================================================*/
void  esm_ctxt_process_act_default_bearer_ctxt_reject 
(
 struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
 const esm_act_default_bearer_context_rej_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  lte_nas_local_cause_T local_cause;
  esm_cause_T cause;

  esm_build_and_send_act_def_ctxt_reject (ctxt_ptr,msg_ptr);
  ctxt_ptr->last_msg_sent = ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT;
  local_cause.valid = TRUE;
  local_cause.local_cause = ESM_DS_REJECTED_THE_CALL;
  cause.valid = FALSE;
  esm_build_and_send_deactivate_ind (ctxt_ptr,local_cause,cause);
  esm_bearer_setup_failure_update_emc_srv_status(msg_ptr);
  esm_bcm_reset_ctxt(ctxt_ptr);
} /* esm_ctxt_act_default_bearer_ctxt_reject () */


 /*==============================================================================
 
   FUNCTION:  esm_ctxt_act_dedicated_bearer_ctxt_reject 
 
 ==============================================================================*/
 /*!
     @brief
     A REQUIRED brief one-sentence description of what esm_ctxt_act_dedicated_bearer_ctxt_reject () does goes here.
 
     @detail
     Builds and sends dededicated bearer context reject
     Changes state to INACTIV
 
 */
 /*============================================================================*/
void  esm_ctxt_process_act_dedicated_bearer_ctxt_reject 
(
 struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
 const esm_act_dedicated_bearer_context_rej_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  lte_nas_local_cause_T local_cause;
  esm_cause_T cause;

  esm_build_and_send_act_dedicated_ctxt_reject (ctxt_ptr,msg_ptr);
  ctxt_ptr->last_msg_sent = ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT;

  local_cause.valid = TRUE;
  local_cause.local_cause = ESM_DS_REJECTED_THE_CALL;
  cause.valid = FALSE;
  esm_build_and_send_deactivate_ind (ctxt_ptr,local_cause,cause);
  esm_bcm_reset_ctxt(ctxt_ptr);

} /* esm_ctxt_act_dedicated_bearer_ctxt_reject () */


static char* context_get_state_name
(
  void  /*!< void */
)
{
  return "ESM_BC_ACTIVE_PENDING";
} /* context_state_name() */

/*==============================================================================

  FUNCTION:  esm_ctxt_active_pending_change_state        

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_active_pending_change_state() does goes here.

    @detail
    An OPTIONAL detailed description of  esm_ctxt_active_pending_change_state() goes here.

*/
/*============================================================================*/
/*void esm_ctxt_active_pending_change_state         
(
  struct esm_eps_bearer_context* ctxt_ptr,
  esm_ctxt_state_ptr_type state_ptr
)
{
  ctxt_ptr->eps_bearer_state_ptr = state;
  if( (state_ptr->state() == ESM_BC_ACTIVE) || (state_ptr->state() == ESM_BC_INACTIVE) )
  {
    ctxt_ptr->context_status_update_func_ptr();
  }
}*/ /*esm_ctxt_active_pending_change_state() */
 
esm_ctxt_state_type esm_ctxt_state_active_pending =
{
  esm_ctxt_discard_act_default_bearer_ctxt_req,
  esm_ctxt_discard_act_dedicated_bearer_ctxt_req,
  esm_ctxt_process_act_default_bearer_ctxt_accept,
  esm_ctxt_process_act_default_bearer_ctxt_reject,
  esm_ctxt_process_act_dedicated_bearer_ctxt_accept,
  esm_ctxt_process_act_dedicated_bearer_ctxt_reject,
  esm_ctxt_discard_deact_bearer_ctxt_req,  
  esm_ctxt_process_sig_conn_released_ind,
  esm_ctxt_process_active_eps_ind,
  context_state,
  context_state_name,
  context_get_state_name,
  esm_ctxt_discard_esm_failure_ind,
  esm_ctxt_change_state,
  esm_ctxt_discard_modify_bearer_ctxt_req,
  esm_ctxt_discard_modify_bearer_ctxt_rej,
  esm_ctxt_discard_modify_bearer_ctxt_accept
};
 
#endif /*FEATURE_LTE*/
