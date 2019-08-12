/*!
  @file
  esm_context_state.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_context_state.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/04/11   zren    Updated ESM to propagate SM group ID to CM for befault Bearer
                   context activation
06/02/10   zren    Updated to reset bearer ctxt data after updated SM when 
                   deactivating bearer ctxt
06/01/10   zren    Updated ESM to sync bearer contexts status with EMM when SM 
                   updated ESM
03/25/10   MNK     i-RAT updates
02/16/10   MNK     Update SM context when ESM context transitions to ACTIVE or INACTIVE
11/18/09   zren    Updated to fix bearer context state change logging and detach
                   trigger in case of no default bearer context existing
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
08/25/09   MNK     ESM-EMM interface changes for active_eps_ind 
07/31/09   MNK     Add handling of modify bearer context related events
06/05/09   zren    Updated code to use MSG_SPRINTF_3 for "%s" format specifier
05/20/09   MNK     Deactivate the context via the context itself for OTA deactivate request
04/28/09   MNK     Added support for EPS_BEARER_STATUS message generation
04/16/09   MNK     More code added to support different scenarios for handling multiple bearers upon attach
04/02/09   MNK     Added support for multiple bearer setup support at the attach 
03/11/09   MNK     Modified active eps indication processing to be generic
03/05/09   MNK     Add support for processing active_eps indication resulting from sending SERVICE REQ to EMM(Dormancy case)
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#ifdef FEATURE_LTE
#include "comdef.h"  
#include "lte_nas_common_v.h"
#include "esm_cm_msgr_enum_msg.h" 
#include "esm_emm_msgr_enum_msg.h" 
#include "esm_v.h"  
#include "esm_bcm.h"  
#include "esm_context_state.h"  
#include "naslog_v.h"
#include "lte_nas_log.h"
#include "sm_multimode_common.h"
#include "amssassert.h"
#include "esm_utils.h"

extern void esm_build_and_send_cm_drb_released_ind
(
  esm_eps_bearer_context_T* ctxt_ptr, /*!< REQUIRED short parameter description */
  //const esm_sig_connection_released_ind_T* msg_ptr
  lte_nas_local_cause_T emm_cause
);

extern void esm_build_and_send_cm_drb_setup_ind
(
  const esm_eps_bearer_context_T* ctxt_ptr   /*!< REQUIRED short parameter description */
);

extern void esm_build_and_send_deactivate_ind
(
  esm_eps_bearer_context_T* ctxt_ptr,   /*!< REQUIRED short parameter description */
  lte_nas_local_cause_T local_cause,
  esm_cause_T cause
);

/*==============================================================================

                                FUNCTIONS

==============================================================================*/


/*==============================================================================

  FUNCTION:  esm_ctxt_discard_act_default_bearer_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_discard_act_default_bearer_ctxt_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_discard_act_default_bearer_ctxt_req() goes here.

*/
/*============================================================================*/
void  esm_ctxt_discard_act_default_bearer_ctxt_req
(
   struct esm_eps_bearer_context* ctxt_ptr, 
   const lte_nas_esm_act_default_bearer_context_req* msg_ptr  /*!< REQUIRED short parameter description */
)
{
  MSG_SPRINTF_3(MSG_SSID_LTE, 
                MSG_LEGACY_HIGH, 
                "ESM: Message activate default bearer context req recived in the state %s received by ESM ",
                ctxt_ptr->eps_bearer_state_ptr->state_name, 0, 0);

} /* esm_ctxt_discard_act_default_bearer_ctxt_req() */

/*==============================================================================

  FUNCTION:  esm_ctxt_discard_act_default_bearer_ctxt_accept

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_discard_act_default_bearer_ctxt_accept() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_discard_act_default_bearer_ctxt_accept() goes here.

*/
/*============================================================================*/
void  esm_ctxt_discard_act_default_bearer_ctxt_accept
(
  struct esm_eps_bearer_context* ctxt_ptr, const esm_act_default_bearer_context_accept_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  MSG_SPRINTF_3(MSG_SSID_LTE,
                MSG_LEGACY_HIGH, 
                "ESM: DISCARDING act_default_bearer_ctxt_accept in %s state",
                ctxt_ptr->eps_bearer_state_ptr->get_state_name(),0,0);
} /* esm_ctxt_discard_act_default_bearer_ctxt_accept() */

/*==============================================================================

  FUNCTION:  esm_ctxt_discard_act_dedicated_bearer_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_discard_act_dedicated_bearer_ctxt_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_discard_act_dedicated_bearer_ctxt_req() goes here.
*/
/*============================================================================*/
void  esm_ctxt_discard_act_dedicated_bearer_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const lte_nas_esm_act_dedicated_bearer_context_req* msg_ptr  /*!< REQUIRED short parameter description */
)
{
  MSG_SPRINTF_3(MSG_SSID_LTE, 
                MSG_LEGACY_HIGH, 
                "ESM: DISCARDING act_dedicated_bearer_ctxt_req in %s state",
                ctxt_ptr->eps_bearer_state_ptr->get_state_name(),0,0);
} /* esm_ctxt_discard_act_dedicated_bearer_ctxt_req() */



/*==============================================================================

  FUNCTION:  esm_ctxt_discard_act_dedicated_bearer_ctxt_accept

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_discard_act_dedicated_bearer_ctxt_accept() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_discard_act_dedicated_bearer_ctxt_accept() goes here.

*/
/*============================================================================*/
void  esm_ctxt_discard_act_dedicated_bearer_ctxt_accept
(
  struct esm_eps_bearer_context* ctxt_ptr, const esm_act_dedicated_bearer_context_accept_T* msg_ptr, struct esm_bcm* mgr_ptr   /*!< REQUIRED short parameter description */
)
{
  MSG_SPRINTF_3(MSG_SSID_LTE, 
                MSG_LEGACY_HIGH, 
                "ESM: DISCARDING act_dedicated_bearer_ctxt_accept in %s state",
                ctxt_ptr->eps_bearer_state_ptr->get_state_name(),0,0);
} /* esm_ctxt_discard_act_dedicated_bearer_ctxt_accept() */

/*==============================================================================

  FUNCTION:  esm_ctxt_discard_sig_conn_released_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_discard_sig_conn_released_ind() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_discard_sig_conn_released_ind() goes here.

*/
/*============================================================================*/
void  esm_ctxt_discard_sig_conn_released_ind
(
  struct esm_eps_bearer_context* ctxt_ptr, 
  const esm_sig_connection_released_ind_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  MSG_SPRINTF_3(MSG_SSID_LTE, 
                MSG_LEGACY_HIGH, 
                "ESM: DISCARDING sig_conn_released_ind in %s state",
                ctxt_ptr->eps_bearer_state_ptr->get_state_name(),0,0);
} /* esm_ctxt_discard_sig_conn_released_ind() */

/*==============================================================================

  FUNCTION:  esm_ctxt_discard_active_eps_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_discard_active_eps_ind() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_discard_active_eps_ind() goes here.

*/
/*============================================================================*/
void  esm_ctxt_discard_active_eps_ind
(
  struct esm_eps_bearer_context  *ctxt_ptr,
  const esm_active_eps_ind_T     *msg_ptr,
  struct esm_bcm                 *mgr_ptr
)
{
  /*
  MSG_SPRINTF_3(MSG_SSID_LTE, 
                MSG_LEGACY_HIGH, 
                "ESM: DISCARDING esm_active_eps_ind in %s state",
                ctxt_ptr->eps_bearer_state_ptr->get_state_name(),0,0);
  */
} /* esm_ctxt_discard_active_eps_ind() */

 
/*==============================================================================

  FUNCTION:    esm_ctxt_discard_deact_bearer_ctxt_req  

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what   esm_ctxt_discard_deact_bearer_ctxt_req  () does goes here.

    @detail
    An OPTIONAL detailed description of   esm_ctxt_discard_deact_bearer_ctxt_req  () goes here.

*/
/*============================================================================*/
void   esm_ctxt_discard_deact_bearer_ctxt_req  
(
 struct esm_eps_bearer_context* ctxt_ptr,  
 const  lte_nas_esm_deact_eps_bearer_context_req* msg_ptr
)
{
  MSG_SPRINTF_3(MSG_SSID_LTE, 
                MSG_LEGACY_HIGH, 
                "ESM: DISCARDING deact_dedicated_bearer_ctxt_req in %s state",
                ctxt_ptr->eps_bearer_state_ptr->get_state_name(),0,0);
} /*   esm_ctxt_discard_deact_dedicated_bearer_ctxt_req  () */

/*==============================================================================

  FUNCTION:  esm_ctxt_discard_default_bearer_ctxt_reject        

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_discard_default_bearer_ctxt_reject        () does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_discard_default_bearer_ctxt_reject        () goes here.

*/
/*============================================================================*/
void      esm_ctxt_discard_default_bearer_ctxt_reject        
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_act_default_bearer_context_rej_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  MSG_SPRINTF_3(MSG_SSID_LTE, 
                MSG_LEGACY_HIGH, 
                "ESM: DISCARDING default_bearer_ctxt_reject  in %s state",
                ctxt_ptr->eps_bearer_state_ptr->get_state_name(),0,0);
} /* esm_ctxt_discard_default_bearer_ctxt_reject        () */

/*==============================================================================

  FUNCTION:  esm_ctxt_discard_dededicated_bearer_ctxt_reject        

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_discard_dedicated_bearer_ctxt_reject        () does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_discard_dedicated_bearer_ctxt_reject        () goes here.

*/
/*============================================================================*/
void      esm_ctxt_discard_dedicated_bearer_ctxt_reject        
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_act_dedicated_bearer_context_rej_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  MSG_SPRINTF_3(MSG_SSID_LTE, 
                MSG_LEGACY_HIGH, 
                "ESM: DISCARDING dededicated_bearer_ctxt_reject  in %s state",
                ctxt_ptr->eps_bearer_state_ptr->get_state_name(),0,0);
} /* esm_ctxt_discard_dedicated_bearer_ctxt_reject        () */


/*==============================================================================

  FUNCTION:  esm_ctxt_process_sig_conn_released_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_process_sig_conn_released_ind() does goes here.

    @detail
    Builds and sends DRB Released indication to the CM

*/
/*============================================================================*/
void esm_ctxt_process_sig_conn_released_ind
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_sig_connection_released_ind_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  esm_build_and_send_cm_drb_released_ind(ctxt_ptr,msg_ptr->emm_cause);
  ctxt_ptr->unclaimed_rb_id = ESM_UNDEFINED_RB_ID;
} /* esm_ctxt_process_sig_conn_released_ind() */

/*===========================================================================

FUNCTION   ESM_DEACT_CTXT_UPON_SYNC_UP

DESCRIPTION
  This function deactivates the given bearer context and its assiciated dedicated
  bearer contexts if it is a default one. It also aborts related ESM procedure
  before the deactivation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void esm_deact_ctxt_upon_sync_up
(
  struct esm_eps_bearer_context  *ctxt_ptr,
  lte_nas_local_cause_T           local_cause,
  esm_cause_T                     ota_cause,
  struct esm_bcm                 *mgr_ptr
)
{
  byte  index;
  struct esm_eps_bearer_context* bctxt_ptr = NULL;
  eps_bearer_id_T eps_bearer_id;

  ASSERT(ctxt_ptr != NULL);
  ASSERT(mgr_ptr != NULL);

  if(ctxt_ptr->default_bearer_ctxt == TRUE)
  {
    /* The bearer to be deactivated is default. Need to abort all ongoing ESM
       procedures which are related to the associated dedicated bearers. */
    for(index=0; index < ESM_MAX_EPS_BEARERS; index++)
    {
      if((mgr_ptr->esm_bc[index] != NULL) && (mgr_ptr->esm_bc[index]->lbi == ctxt_ptr->eps_bearer_id) &&
         (mgr_ptr->esm_bc[index]->eps_bearer_state_ptr->state() != ESM_BC_INACTIVE))
      {
        mgr_ptr->abort_proc_ptr(mgr_ptr->esm_bc[index]->eps_bearer_id);
      }
    }

    /* Deactivate all associated dedicated bearers */
    ctxt_ptr->deactivate_associated_dedicated_ctxts_func_ptr(
                              local_cause, ota_cause, ctxt_ptr->eps_bearer_id);
  }

  /* Get the eps_bearer_id to local variable as ctxt_ptr contents might be invalid after Abort Procedure Call */
  eps_bearer_id = ctxt_ptr->eps_bearer_id;
  
  /* Abort the ongoing ESM procedures which is related to bearer */
  mgr_ptr->abort_proc_ptr(ctxt_ptr->eps_bearer_id);

  bctxt_ptr = esm_bcm_find_bearer_context(mgr_ptr,eps_bearer_id);
  if(bctxt_ptr != NULL)
  {
    esm_build_and_send_deactivate_ind(bctxt_ptr, local_cause, ota_cause);
    esm_bcm_reset_ctxt(bctxt_ptr);
    bctxt_ptr = NULL;
  }
  ctxt_ptr = NULL;
}/* esm_deact_ctxt_upon_sync_up */

/*==============================================================================

  FUNCTION:  esm_ctxt_process_eps_active_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_process_active_eps_ind() does goes here.

    @detail
    If the eps indication suggests the RB correspoing to the bearer id is removed
    Sends Bearer Deactivation Indication to the CM
*/
/*============================================================================*/
void esm_ctxt_process_active_eps_ind
(
  struct esm_eps_bearer_context  *ctxt_ptr,
  const esm_active_eps_ind_T     *msg_ptr,
  struct esm_bcm                 *mgr_ptr
)
{
  byte index = 0;
  lte_nas_local_cause_T local_cause;
  esm_cause_T ota_cause;
  boolean need_deactivation = TRUE;/* This flag should be checked only if esm_bcm_srv_req_pending is TRUE*/
  local_cause.valid = FALSE;
  ota_cause.valid = FALSE;

  ASSERT(ctxt_ptr != NULL);
  ASSERT(msg_ptr != NULL);
  ASSERT(mgr_ptr != NULL);

  /* ESM will always send the drb setup indication. DS will discard it in the case where bearer context setup request is received 
  from the N/W for example like attach case and use it in all other cases when the DRBs are setup 
  like service request or tracking area update*/
  for(index = 0; index < msg_ptr->num_added_bearers; index++)
  {
    if((msg_ptr->added_eps_bearers[index].eps_id < ESM_STARTING_BEARER_ID)
       ||
       (msg_ptr->added_eps_bearers[index].eps_id > ESM_ENDING_BEARER_ID))
    {
      MSG_ERROR_DS_2(SM_SUB,"=ESM= Rcved invalid Added EPS bearer ID %d, index %d.\
					 Ignoring this Bearer ID",
		             msg_ptr->added_eps_bearers[index].eps_id, index);
    }
    else if(ctxt_ptr->eps_bearer_id == msg_ptr->added_eps_bearers[index].eps_id)
    {
      need_deactivation = FALSE;
      /* Store the rb_id to pass it on to CM/DS for TLB/LBT (test loop back/loop back test)*/
      ctxt_ptr->rb_id   = msg_ptr->added_eps_bearers[index].rb_id;
      /* Reset unclaimed rb id */
      ctxt_ptr->unclaimed_rb_id = ESM_UNDEFINED_RB_ID;
      MSG_HIGH_DS_2(SM_SUB,"ESM: Added BC matching active EPS ind - BID %d, RBID %d",
                    ctxt_ptr->eps_bearer_id, ctxt_ptr->rb_id);
      /* ESM LOG PACKET */
      send_esm_ctxt_info_log(ctxt_ptr);
      esm_build_and_send_cm_drb_setup_ind(ctxt_ptr);
      break;
    }
  }

  for(index = 0; index < msg_ptr->num_removed_bearers; index++)
  {
    if((msg_ptr->removed_eps_bearers[index].eps_id < ESM_STARTING_BEARER_ID)
       ||
       (msg_ptr->removed_eps_bearers[index].eps_id > ESM_ENDING_BEARER_ID))
    {
      MSG_ERROR_DS_2(SM_SUB,"=ESM= Rcved invalid Removed EPS bearer ID %d, index %d.\
					 Ignoring this Bearer ID",
		             msg_ptr->removed_eps_bearers[index].eps_id, index);
    }
    else if(ctxt_ptr->eps_bearer_id == msg_ptr->removed_eps_bearers[index].eps_id)
    {
      /* The DRB is gone, so inform the CM/DS the same. The DRB could be noted a gone for 
      many reasons. a) The DRB is released by RRC for internal reasons b) There is a mimacth
      scenario where UE has more DRBs than the N/W. For now we don't knwo why we see that
      the DRB is gone. So, we just set the cause value to DRB_RELEASED_AT_RRC*/
      local_cause.local_cause = DRB_RELEASED_AT_RRC;
      local_cause.valid = TRUE;     
      ota_cause.valid = FALSE;
      MSG_HIGH_DS_1(SM_SUB,"ESM: Removed BC matching active EPS ind - BID %d",
                    ctxt_ptr->eps_bearer_id);
      esm_deact_ctxt_upon_sync_up(ctxt_ptr, local_cause, ota_cause, mgr_ptr);
      need_deactivation = FALSE;
      break;
    }
  }

  /* Now walk through the active eps bearer list to see if we have a mismatch. This is needed especially in the cases of 
  service request and tracking area update reactivating the bearers*/
  for(index = 0; index < msg_ptr->num_active_bearers; index++)
  {
    if((msg_ptr->active_eps_bearers[index].eps_id < ESM_STARTING_BEARER_ID)
       ||
       (msg_ptr->active_eps_bearers[index].eps_id > ESM_ENDING_BEARER_ID))
    {
      MSG_ERROR_DS_2(SM_SUB,"=ESM= Rcved invalid Active EPS bearer ID %d, index %d.\
					 Ignoring this Bearer ID",
		             msg_ptr->active_eps_bearers[index].eps_id, index);
    }
    else if(ctxt_ptr->eps_bearer_id == msg_ptr->active_eps_bearers[index].eps_id)
    {
      MSG_HIGH_DS_1(SM_SUB,"ESM: Active BC matching active EPS ind - BID %d",
                    ctxt_ptr->eps_bearer_id);
      need_deactivation = FALSE;
      break;
    }
  }

  /* This needs to be done only if this flag is true. This is now syncing up the context mismatch between the
  n/w and UE*/
  if(need_deactivation == TRUE)
  {
    MSG_HIGH_DS_1(SM_SUB,"ESM: Bearer %d to be deactivated - Not matching active EPS ind",
                  ctxt_ptr->eps_bearer_id);
    local_cause.valid = TRUE;
    local_cause.local_cause = ESM_SYNC_UP_WITH_NW;
    esm_deact_ctxt_upon_sync_up(ctxt_ptr, local_cause, ota_cause, mgr_ptr);
  }
} /* esm_ctxt_process_active_eps_ind() */

void esm_ctxt_discard_esm_failure_ind
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_failure_ind_T* msg_ptr    /*!< REQUIRED short parameter description */
)
{
  /*
  MSG_SPRINTF_3(MSG_SSID_LTE, 
                MSG_LEGACY_HIGH, 
                "ESM: DISCARDING esm_failure_ind in %s state",
                ctxt_ptr->eps_bearer_state_ptr->get_state_name(),0,0);
  */
}

/*==============================================================================

  FUNCTION:  esm_ctxt_change_state        

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_change_state() does goes here.

    @detail
    An OPTIONAL detailed description of  esm_ctxt_change_state() goes here.

*/
/*============================================================================*/
void esm_ctxt_change_state         
(
  struct esm_eps_bearer_context  *ctxt_ptr,
  esm_ctxt_state_ptr_type         new_state_ptr
)
{
  lte_nas_esm_bearer_context_state_T  esm_bearer_ctxt_state_log;

  ASSERT(ctxt_ptr != NULL);
  ASSERT(new_state_ptr != NULL);

  if (ctxt_ptr->eps_bearer_state_ptr != new_state_ptr)
  {
    /* Change state */
    ctxt_ptr->eps_bearer_state_ptr = new_state_ptr;

    /* Logging */
    esm_bearer_ctxt_state_log.bearer_state = (byte)new_state_ptr->state();
    esm_bearer_ctxt_state_log.bearer_id = ctxt_ptr->eps_bearer_id;
    if(new_state_ptr == &esm_ctxt_state_inactive)
    {
      esm_bearer_ctxt_state_log.connection_id = ESM_INVALID_CON_ID;
    }
    else
    {
      esm_bearer_ctxt_state_log.connection_id = ctxt_ptr->connection_id;
    }
    send_esm_bearer_ctxt_state_log_info(&esm_bearer_ctxt_state_log);

    if((new_state_ptr->state() == ESM_BC_ACTIVE) || 
       (new_state_ptr->state() == ESM_BC_INACTIVE) )
    {
      /* Update EMM */
      ctxt_ptr->context_status_update_func_ptr();
      /* Update SM context also*/
      (void)sm_update_sm_context_from_esm(ctxt_ptr, new_state_ptr->state());
    }
  }	
} /*esm_ctxt_change_state() */

/*==============================================================================

  FUNCTION: esm_ctxt_discard_modify_bearer_ctxt_req         

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what         esm_ctxt_discard_modify_bearer_ctxt_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_discard_modify_bearer_ctxt_req() goes here.

*/
/*============================================================================*/
void esm_ctxt_discard_modify_bearer_ctxt_req             
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const lte_nas_esm_modify_eps_bearer_ctxt_req* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  MSG_SPRINTF_3(MSG_SSID_LTE, 
                MSG_LEGACY_HIGH, 
                "ESM: DISCARDING Modify Bearer Context Request in %s state",
                ctxt_ptr->eps_bearer_state_ptr->get_state_name(),0,0);
} /* esm_ctxt_discard_modify_bearer_ctxt_req        () */

/*==============================================================================

  FUNCTION: esm_ctxt_discard_modify_bearer_ctxt_rej         

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_ctxt_discard_modify_bearer_ctxt_rej() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_discard_modify_bearer_ctxt_rej() goes here.

*/
/*============================================================================*/
void esm_ctxt_discard_modify_bearer_ctxt_rej             
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_modify_bearer_context_rej_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  MSG_SPRINTF_3(MSG_SSID_LTE, 
                MSG_LEGACY_HIGH, 
                "ESM: DISCARDING Modify Bearer Context Reject in %s state",
                ctxt_ptr->eps_bearer_state_ptr->get_state_name(),0,0);
} /* esm_ctxt_discard_modify_bearer_ctxt_rej        () */

/*==============================================================================

  FUNCTION: esm_ctxt_discard_modify_bearer_ctxt_accept         

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what         esm_ctxt_discard_modify_bearer_ctxt_accept() does goes here.

    @detail
    An OPTIONAL detailed description of esm_ctxt_discard_modify_bearer_ctxt_accept() goes here.

*/
/*============================================================================*/
void  esm_ctxt_discard_modify_bearer_ctxt_accept            
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_modify_bearer_context_accept_T* msg_ptr,   /*!< REQUIRED short parameter description */
  struct esm_bcm* mgr_ptr
)
{
  MSG_SPRINTF_3(MSG_SSID_LTE, 
                MSG_LEGACY_HIGH, 
                "ESM: DISCARDING Modify Bearer Context Accept in %s state",
                ctxt_ptr->eps_bearer_state_ptr->get_state_name(),0,0);
} /* esm_ctxt_discard_modify_bearer_ctxt_accept        () */

#endif /*FEATURE_LTE*/
