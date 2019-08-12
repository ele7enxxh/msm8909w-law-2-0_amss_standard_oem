
/*!
  @file
  esm_context_inactive.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_context_inactive.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/31/09   MNK     Add handling of modify bearer context related events
05/20/09   MNK     Deactivate the context via the context itself for OTA deactivate request
04/28/09   MNK     Added support for EPS_BEARER_STATUS message generation
04/16/09   MNK     More code added to support different scenarios for handling multiple bearers upon attach
04/02/09   MNK     Added support for multiple bearer setup support at the attach 
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "esm_bcm.h"
#include "esm_context_state.h"
#include "esm_utils.h"

extern void esm_build_and_send_cm_act_def_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr   /*!< REQUIRED short parameter description */
);

extern boolean esm_extract_OTA_msg
(
  struct esm_eps_bearer_context* ctxt_ptr, byte* msg_ptr
);

extern void esm_copy_default_ctxt_data
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const lte_nas_esm_act_default_bearer_context_req* msg_ptr   /*!< REQUIRED short parameter description */
);


/*==============================================================================

                                FUNCTIONS

==============================================================================*/


/*==============================================================================

  FUNCTION:  esm_ctxt_process_act_defaut_bearer_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_process_act_defaut_bearer_ctxt_req() does goes here.

    @detail
    Builds and sends default bearer context activate request to the CM
    Copies the context data received from the NW
    Changes state to ACTIVE PENDING
*/
/*============================================================================*/
static void esm_ctxt_process_act_default_bearer_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const lte_nas_esm_act_default_bearer_context_req* msg_ptr
)
{
  MSG_LOW_DS_0(SM_SUB, "ESM: ENTER ESM_INACTIVE:esm_ctxt_process_act_default_bearer_ctxt_req\n");
  ctxt_ptr->eps_bearer_state_ptr->state_name();

  esm_copy_default_ctxt_data(ctxt_ptr,msg_ptr);

  esm_build_and_send_cm_act_def_ctxt_req (ctxt_ptr);
  //ctxt_ptr->eps_bearer_state_ptr = &esm_ctxt_state_active_pending;

  ctxt_ptr->eps_bearer_state_ptr->ctxt_change_state(ctxt_ptr,&esm_ctxt_state_active_pending);

  ctxt_ptr->eps_bearer_state_ptr->state_name();
} /* esm_ctxt_process_act_defaut_bearer_ctxt_req() */

/*==============================================================================

  FUNCTION:  esm_ctxt_process_act_dedicated_bearer_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_process_act_dedicated_bearer_ctxt_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_process_act_dedicated_bearer_ctxt_req() goes here.

*/
/*============================================================================*/
static void  esm_ctxt_process_act_dedicated_bearer_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const lte_nas_esm_act_dedicated_bearer_context_req* msg_ptr  /*!< REQUIRED short parameter description */
)
{
  MSG_LOW_DS_0(SM_SUB, "ESM: ENTER ESM_INACTIVE:esm_ctxt_process_act_default_bearer_ctxt_req\n");
  ctxt_ptr->eps_bearer_state_ptr->state_name();

  esm_copy_dedicated_ctxt_data(ctxt_ptr,msg_ptr);

  esm_build_and_send_cm_act_dedicated_ctxt_req (ctxt_ptr);

  //ctxt_ptr->eps_bearer_state_ptr = &esm_ctxt_state_active_pending;
  ctxt_ptr->eps_bearer_state_ptr->ctxt_change_state(ctxt_ptr,&esm_ctxt_state_active_pending);
  
} /* esm_ctxt_process_act_dedicated_bearer_ctxt_req() */

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
  return ESM_BC_INACTIVE; 
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
  MSG_LOW_DS_0(SM_SUB, "ESM: Context State Name : ESM_BC_INACTIVE\n");
} /* context_state_name() */

static char* context_get_state_name
(
  void  /*!< void */
)
{
  return "ESM_BC_INACTIVE";
} /* context_state_name() */

/*==============================================================================

  FUNCTION:  esm_ctxt_inactive_change_state        

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_inactive_change_state() does goes here.

    @detail
    An OPTIONAL detailed description of  esm_ctxt_inactive_change_state() goes here.

*/
/*============================================================================*/
/*void esm_ctxt_inactive_change_state         
(
  struct esm_eps_bearer_context* ctxt_ptr,
  esm_ctxt_state_ptr_type state_ptr
)
{
  //ctxt_ptr->eps_bearer_state_ptr = state_ptr;
}*/ /*esm_ctxt_inactive_change_state() */


esm_ctxt_state_type esm_ctxt_state_inactive =
{
  esm_ctxt_process_act_default_bearer_ctxt_req,
  esm_ctxt_process_act_dedicated_bearer_ctxt_req,
  esm_ctxt_discard_act_default_bearer_ctxt_accept,
  esm_ctxt_discard_default_bearer_ctxt_reject,
  esm_ctxt_discard_act_dedicated_bearer_ctxt_accept,
  esm_ctxt_discard_dedicated_bearer_ctxt_reject,
  esm_ctxt_discard_deact_bearer_ctxt_req, 
  esm_ctxt_discard_sig_conn_released_ind,
  esm_ctxt_discard_active_eps_ind,
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
