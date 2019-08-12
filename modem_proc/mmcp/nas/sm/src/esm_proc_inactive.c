
/*!
  @file
  esm_proc_inactive.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_proc_inactive.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/03/10   zren    Added ESM new timer machanism
11/18/09   zren    Added support for ESM Information procedure
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/01/09   MNK     Added handling of detach_ind from EMM, as we support bearer initiated procedures
08/18/09   MNK     CM-NAS interface changes
08/14/09   MNK     Add Support for PDN disconnect request
07/30/09   MNK     Changes for multiple PDN connectivity support
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
#include "esm_proc_state.h"
#include "esm_bpm.h"
#include "timers_v.h"
#include "esm_utils.h"
#include "esm_bcm.h"
#include "sm_multimode_common.h"

/*==============================================================================

                                FUNCTIONS

==============================================================================*/
 
/*==============================================================================

  FUNCTION:  esm_proci_process_bearer_resource_alloc_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_process_bearer_resource_alloc_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_process_bearer_resource_alloc_req() goes here.
*/
/*============================================================================*/
static void    esm_proc_process_bearer_resource_alloc_req
(
  struct esm_bearer_resource_proc* proc_ptr, const esm_bearer_resource_alloc_req_T* msg_ptr  /*!< REQUIRED short parameter description */
)
{
  boolean status = FALSE;

  esm_build_and_send_bearer_resource_allocation_req(proc_ptr,msg_ptr);
  /* Start timer  TO DO MNK*/
  status = esm_start_timer(ESM_TIMER_T3480,proc_ptr);
  ASSERT(status == TRUE);
  proc_ptr->esm_proc_state_ptr =  &esm_proc_state_active;
} /* esm_proc_process_bearer_resource_alloc_req() */


/*==============================================================================

  FUNCTION:  esm_proc_inactive_process_OTA_message

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_inactive_process_OTA_message() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_inactive_process_OTA_message() goes here.

*/
/*============================================================================*/
static boolean esm_proc_inactive_process_OTA_message
(
  struct esm_bearer_resource_proc* proc_ptr, lte_nas_esm_incoming_msg_type* data_ptr  /*!< REQUIRED short parameter description */
)
{
  
  return TRUE;
} /* esm_proc_inactive_process_OTA_message() */



/*==============================================================================

  FUNCTION:  esm_proc_inactive_state

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_inactive_state() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_inactive_state() goes here.
*/
/*============================================================================*/
static esm_bp_state_T esm_proc_inactive_state
(
  void
)
{
  return ESM_BP_INACTIVE;
} /* esm_proc_inactive_state() */

/*==============================================================================

  FUNCTION:  esm_proc_inactive_state_name

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_inactive_state_name() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_inactive_state_name() goes here.
*/
/*============================================================================*/
static void esm_proc_inactive_state_name
(
  void
)
{

} /* esm_proc_inactive_state_name() */

/*==============================================================================

  FUNCTION:  esm_proc_inactive_process_pdn_disconnect_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_proc_inactive_process_pdn_disconnect_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_proc_inactive_process_pdn_disconnect_req() goes here.
*/
/*============================================================================*/
static void esm_proc_inactive_process_pdn_disconnect_req
(
  struct esm_bearer_resource_proc* proc_ptr,
  esm_pdn_disconnect_req_T* data_ptr
)
{
  boolean status = FALSE;

  /* All bearers associated with the PDN will be deactivated. 
     Update SM to deactivate corresponding PDPs.
     Note: In OOS/RLF case, MM would acquire service in GW and then sends 
           RAT_RESUME to SM followed by RAU without waiting for RAT_RESUME_CNF 
           from SM. If SM PDP contexts are not deactivated here then MM would 
           indicate them as active in RAU request while these PDP contexts would
           be deactivated upon SM processing on RAT_RESUME. Therefore SM and MM's 
           RAU will be out of sync. */
  esm_bc_deact_pending_update_sm(data_ptr->eps_bearer_id);

  esm_build_and_send_pdn_disconnect_req(proc_ptr,data_ptr);
  status = esm_start_timer(ESM_TIMER_T3492,proc_ptr);
  ASSERT(status == TRUE);
  proc_ptr->esm_proc_state_ptr =  &esm_proc_state_active;
} /* esm_proc_inactive_state_name() */


static char* esm_proc_inactive_get_state_name(void)
{
  return "ESM_BP_INACTIVE";
}

/*==============================================================================

  FUNCTION:     esm_proc_process_bearer_resrc_mod_req

==============================================================================*/
/*!
    @brief
    This function processed the Bearer Resource Modification Request from CM/DS

    @detail

*/
/*============================================================================*/
static void esm_proc_process_bearer_resrc_mod_req
(
  struct esm_bearer_resource_proc                 *proc_ptr,
  const  esm_bearer_resource_modification_req_T   *msg_ptr
)
{
  /* Save the sdf_id in procedure struture to be used later */
  proc_ptr->sdf_id = msg_ptr->esm_hdr.sdf_id;

  if((msg_ptr->esm_local_cause.valid == TRUE) &&
     (msg_ptr->esm_local_cause.local_cause == DS_EXPLICIT_DEACT))
  {
    /* Per DS request, the dedicated bearer will be deactivated regardless NW 
       response. Update SM to deactivate corresponding PDP.
       Note: In OOS/RLF case, MM would acquire service in GW and then sends 
             RAT_RESUME to SM followed by RAU without waiting for RAT_RESUME_CNF 
             from SM. If SM PDP contexts are not deactivated here then MM would 
             indicate them as active in RAU request while these PDP contexts would
             be deactivated upon SM processing on RAT_RESUME. Therefore SM and MM's 
             RAU will be out of sync. */
    esm_bc_deact_pending_update_sm(msg_ptr->esm_hdr.lbi);
  }
  /* Send req messge to NW */
  esm_build_and_send_bearer_resrc_mod_req(proc_ptr,msg_ptr);
  /* Start timer T3481 */
  (void)esm_start_timer(ESM_TIMER_T3481,proc_ptr);
  /* Update the proc state to active */
  proc_ptr->esm_proc_state_ptr = &esm_proc_state_active;

} /* esm_proc_process_bearer_resrc_mod_req  */

esm_proc_state_type esm_proc_state_inactive =
{
  esm_proc_process_pdn_connect_req,
  esm_proc_process_bearer_resource_alloc_req,
  esm_proc_discard_act_default_bearer_ctxt_req,
  esm_proc_inactive_process_OTA_message,
  esm_proc_inactive_state,
  esm_proc_inactive_state_name,
  esm_proc_inactive_get_state_name,
  esm_proc_discard_bearer_resource_abort_req,
  esm_proc_discard_timer_expiry_or_lower_layer_failure,
  esm_proc_inactive_process_pdn_disconnect_req,
  esm_proc_discard_pdn_connectivity_abort_req,
  esm_proc_discard_detach_ind,
  esm_proc_process_bearer_resrc_mod_req
};

#endif /*FEATURE_LTE*/
