
/*!
  @file
  esm_bcm.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_bcm.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/23/12   us      Removing unsused static buffer from esm_bcm_T.
09/19/11   rajesh  Reset ISR status in function "esm_bcm_deactivate_all_contexts"
09/14/11   rajesh  Made changes to set the ISR status correctly in ESM.
08/04/11   zren    Updated ESM to propagate SM group ID to CM for befault Bearer
                   context activation
01/05/11   MNK     Corrected handling of ESM_FAILURE_IND from EMM
10/21/10   MNK     Deactivation of bearer contexts not transferable to GW upon IRAT 
10/08/10   MNK     1XCSFB manager introduction 
10/07/10   MNK     1XCSFB introduction 
06/02/10   zren    Updated to reset bearer ctxt data after updated SM when 
                   deactivating bearer ctxt
06/01/10   zren    Updated ESM to sync bearer contexts status with EMM when SM 
                   updated ESM
05/14/10   RI      Mainlined spec changes upto Dec 09.
02/17/10   MNK     Add support for context updates when in UMTS
01/21/10   zren    Added EMM/ESM STATUS feature
12/10/09   zren    Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
11/18/09   zren    Updated to fix bearer context state change logging and detach
                   trigger in case of no default bearer context existing
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
06/05/09   zren    Updated code to use MSG_SPRINTF_3 for "%s" format specifier
06/02/09   MNK     Added additional criteria of mathcing the LBI to match the UE initiated request  
05/28/09   MNK     Add error checks for received CM messages
05/21/09   MNK     Added support to reply back to CM/DS in case the NW sent bad messages as a response to UE initiated request
05/20/09   MNK     Deactivate the context via the context itself for OTA deactivate request
05/08/09   MNK     Added support for bearer resource allocation procedures
05/05/09   MNK     Ignore failure indications from the EMM for the messages that use ESM_RESERVED_CONNECTION_ID
05/01/09   MNK     Added support for N/W initiated bearer context deactivation
04/29/08   MNK     Added processing of NAS_ESM_EPS_BEARER_STATUS_IND and changed the context allocation scheme
04/28/09   MNK     Added emm_eps_bearer_status message generation when a context moves from active to inactive and vice versa
04/24/09   MNK     Added emm_eps_bearer_status message and detach cmd gneration when all contexts removed
04/16/09   MNK     More code added to support different scenarios for handling multiple bearers upon attach
04/06/09   MNK     Added code to generate based on new transaction id format
04/02/09   MNK     Added support for multiple bearer setup support at the attach 
03/11/09   MNK     Removed service request specific active eps indication processing
03/06/09   MNK     Handling of esm_failure_ind modified
03/05/09   MNK     Service Request response (active_eps_ind) processing for dormancy support
02/23/09   MNK     Added check for validating the bearer id
02/22/09   MNK     Added support for Dormancy
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "amssassert.h"
#include "lte_nas_local_cause.h"
#include "lte_nas_esm_message_description.h"
#include "esm_cm_msgr_enum_msg.h" 
#include "esm_emm_msgr_enum_msg.h" 
#include "esm_v.h"
#include "esm_context_state.h"
#include "esm_bcm.h"
#include "esm_utils.h"
#include "naslog_v.h"
#include "lte_nas_log.h"
#include "sm_multimode_common.h"
#include "stringl.h"
#include "modem_mem.h"


extern byte esm_get_module_from_trans_id
(
  dword transaction_id   /*!< REQUIRED short parameter description */
);

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  esm_bcm_find_bearer_context

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_find_bearer_context() does goes here.

    @detail
    Finds a context given a bearer id
*/
/*============================================================================*/
struct esm_eps_bearer_context* esm_bcm_find_bearer_context
(
  esm_bcm_T         *mgr_ptr,  /*!< REQUIRED short parameter description */
  eps_bearer_id_T    bearer_id    /*!< REQUIRED short parameter description */
)
{
  byte index = 0;

  ASSERT(mgr_ptr != NULL);

  if(bearer_id >= MIN_NSAPI_NUM)
  {
    for(index = 0; index < ESM_MAX_EPS_BEARERS ; index++)
    {
      //Try to find a match;
      if((mgr_ptr->esm_bc[index] != NULL) && (mgr_ptr->esm_bc[index]->eps_bearer_id == bearer_id))
      {
        return mgr_ptr->esm_bc[index];
      }
    }
  }
  return NULL;
} /* esm_bcm_find_bearer_context() */

/*==============================================================================

  FUNCTION:  esm_bcm_allocate_context

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_allocate_context() does goes here.

    @detail
    Allocates a free context.

    IMPORTANT NOTE

    If the scheme in esm_bcm_allocate_context is changed for context allocation, then  
    esm_bcm_handle_eps_bearer_status_ind needs a change.
    This function assumes that there is a strict relation between the bearer id and connection id

    Givn a bearer id, the connection id can be mapped and vice versa. e.g
    connection_id 0 maps to bearer Id 5, connection id 1 maps to bearer id 6 etc..So, there is a fixed mapping 
    between the connection id and the bearer idi. This can't be violated. If it is violated it is an error
*/
/*============================================================================*/
static struct esm_eps_bearer_context* esm_bcm_allocate_context
(
  esm_bcm_T* mgr_ptr,  /*!< REQUIRED short parameter description */
  byte bearer_id
)
{
  byte index = 0;

  /* This is the new way of allocating the context. The idea is to create a direct mapping between 
  the bearer Id and the connection Id. Givn a bearer id, the connection id can be mapped and vice versa. e.g
  connection_id 0 maps to bearer Id 5, connection id 1 maps to bearer id 6 etc..So, there is a fixed mapping 
  between the connection id and the bearer idi. This can't be violated. If it is violated it is an error*/
  index = bearer_id - ESM_STARTING_BEARER_ID;
  if (mgr_ptr->esm_bc[index] == NULL)
  {
    mgr_ptr->esm_bc[index] = (esm_eps_bearer_context_T *)esm_modem_mem_alloc(sizeof(esm_eps_bearer_context_T));
    esm_bcm_reset_ctxt_data(mgr_ptr->esm_bc[index]);
    esm_init_bcm_context_fn_ptr(mgr_ptr->esm_bc[index]);

    mgr_ptr->esm_bc[index]->connection_id = index;

    if(mgr_ptr->isr_status == FALSE)
    {
      mgr_ptr->esm_bc[index]->isr_bearer_ctxt_act_status = CTXT_ACT_BEFORE_ISR;
    }
    else
    {
      /* ESM uses this information to decide whether ISR should be switched on or not after receiving a RAT change indication
         from EMM
      */
      mgr_ptr->esm_bc[index]->isr_bearer_ctxt_act_status = CTXT_ACT_AFTER_ISR_IN_LTE;
    }

    return mgr_ptr->esm_bc[index];
    
  }
  
  else
  {
    MSG_ERROR_DS_2(SM_SUB, "ESM: For a given beaer Idi = %d the connection Id =%d is already in use. This should never happen...Something is buggy...",
                   bearer_id, mgr_ptr->esm_bc[index]->connection_id);
    return NULL;
  }

} /* esm_bcm_allocate_context() */

/*==============================================================================

  FUNCTION:  esm_bcm_reset_ctxt_data

==============================================================================*/
/*!
    @brief
    This function resets all data from a given bearer context

    @detail
    Reset all data from a given context.
*/
/*============================================================================*/
void esm_bcm_reset_ctxt_data
(
  struct esm_eps_bearer_context  *ctxt_ptr
)
{
  ASSERT(ctxt_ptr != NULL);

  ctxt_ptr->connection_id = ESM_INVALID_CON_ID;
  ctxt_ptr->pti = ESM_INVALID_PTI;
  ctxt_ptr->con_id_creator = MSGR_NAS_ESM;
  ctxt_ptr->last_msg_sent = ESM_LAST_UNDEFINED;
  ctxt_ptr->apn_name.valid = FALSE;
  ctxt_ptr->config_options.valid = FALSE;
  ctxt_ptr->eps_bearer_id = 0;
  ctxt_ptr->esm_cause.valid = FALSE;
  ctxt_ptr->esm_local_cause.valid = FALSE;
  ctxt_ptr->lbi = 0;
  ctxt_ptr->neg_llc_sapi.valid = FALSE;
  ctxt_ptr->neg_qos.valid = FALSE;
  ctxt_ptr->packet_flow_id.valid = FALSE;
  ctxt_ptr->pdn_address.valid = FALSE;
  ctxt_ptr->radio_pri.valid = FALSE;
  ctxt_ptr->sdf_id = 0xff;
  ctxt_ptr->sdf_qos.valid = FALSE;
  ctxt_ptr->tft.valid = FALSE;
  ctxt_ptr->ti.valid = FALSE;
  ctxt_ptr->sm_group_id = 0;
  ctxt_ptr->on_attach_default_bearer = FALSE;
  ctxt_ptr->default_bearer_ctxt = FALSE;
  ctxt_ptr->rb_id = 0;
  ctxt_ptr->isr_bearer_ctxt_act_status = CTXT_ACT_BEFORE_ISR;
#ifdef FEATURE_LTE_REL9
  ctxt_ptr->emc_bc = FALSE;
#endif
  ctxt_ptr->unclaimed_rb_id = ESM_UNDEFINED_RB_ID;
  ctxt_ptr->backoff_timer.active = TRUE;
  ctxt_ptr->backoff_timer.timer_count = 0;
  ctxt_ptr->eps_bearer_state_ptr = &esm_ctxt_state_inactive;
}/* esm_bcm_reset_ctxt_data */


/*===========================================================================

FUNCTION  ESM_GEN_SM_GROUP_ID

DESCRIPTION
  This function generates SM group ID for default bearer context/Primary PDP
  
DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
byte  emm_gen_sm_group_id
(
  esm_bcm_T  *mgr_ptr
)
{
  byte index, group_id;

  ASSERT(mgr_ptr != NULL);

  for(group_id = 1; group_id < MAX_PDP_NUM; group_id++)
  {
    for(index = 0; index < ESM_MAX_EPS_BEARERS; index++)
    {
      if((mgr_ptr->esm_bc[index] != NULL && mgr_ptr->esm_bc[index]->eps_bearer_state_ptr->state() != 
                                                            ESM_BC_INACTIVE) &&
         (mgr_ptr->esm_bc[index]->default_bearer_ctxt == TRUE) &&
         (mgr_ptr->esm_bc[index]->sm_group_id == group_id))
      {
        /* This group ID is used by an existing default bearer context. Break
           out the inner loop to try next group ID */
        break;
      }
    }
    if(index == ESM_MAX_EPS_BEARERS)
    {
      /* Return this group ID since it is not used by any existing default 
         bearer context. */
      return group_id;
    }
  }

  MSG_FATAL_DS(SM_SUB, "ESM: Could not generate SM group ID",0,0,0);
  return 0;

}/* emm_gen_sm_group_id */

/*==============================================================================

  FUNCTION:  esm_bcm_deactivate_associated_ded_ctxts()

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_deactivate_associated_ded_ctxts().

    @detail
    Deactivate the associated dedicated beares of a default bearer

*/
/*============================================================================*/
void esm_bcm_deactivate_associated_ded_ctxts
(
  esm_bcm_T* mgr_ptr,   
  lte_nas_local_cause_T local_cause,
  esm_cause_T cause,
  byte linked_bearer_id
)
{
  byte index = 0;
  esm_bc_state_T   cur_state;

  for(index=0; index < ESM_MAX_EPS_BEARERS; index++)
  {
    if(mgr_ptr->esm_bc[index] != NULL && mgr_ptr->esm_bc[index]->lbi == linked_bearer_id)
    {
      cur_state = mgr_ptr->esm_bc[index]->eps_bearer_state_ptr->state();
      if(cur_state != ESM_BC_INACTIVE)
      {
        esm_build_and_send_deactivate_ind(mgr_ptr->esm_bc[index],local_cause,cause);
      }
      esm_bcm_reset_ctxt(mgr_ptr->esm_bc[index]);

    }
  }
}

void send_esm_ctxt_info_log(esm_eps_bearer_context_T *ctxt_ptr)
{
  lte_nas_esm_bearer_context_info_T esm_bearer_ctxt_info_log;

  if(ctxt_ptr != NULL)
  {
    esm_bearer_ctxt_info_log.context_type = (!ctxt_ptr->default_bearer_ctxt);
    esm_bearer_ctxt_info_log.bearer_id = ctxt_ptr->eps_bearer_id;
    esm_bearer_ctxt_info_log.bearer_state = (byte)ctxt_ptr->eps_bearer_state_ptr->state();
    esm_bearer_ctxt_info_log.connection_id = ctxt_ptr->connection_id;
    esm_bearer_ctxt_info_log.sdf_id = ctxt_ptr->sdf_id;  
    esm_bearer_ctxt_info_log.lbi_valid = FALSE;
    if(ctxt_ptr->lbi != 0)
    {
      esm_bearer_ctxt_info_log.lbi_valid = TRUE;
      esm_bearer_ctxt_info_log.lbi = ctxt_ptr->lbi;
    }
    esm_bearer_ctxt_info_log.rb_id = ctxt_ptr->rb_id;  

    if (ctxt_ptr->eps_bearer_state_ptr->state() == ESM_BC_MODIFY_PENDING)
    {
      esm_bearer_ctxt_info_log.eps_qos[0] = ctxt_ptr->bc_modify_data.sdf_qos.length; 
      esm_bearer_ctxt_info_log.eps_qos[1] = ctxt_ptr->bc_modify_data.sdf_qos.qci; 
      esm_bearer_ctxt_info_log.eps_qos[2] = ctxt_ptr->bc_modify_data.sdf_qos.max_bit_rate_ul; 
      esm_bearer_ctxt_info_log.eps_qos[3] = ctxt_ptr->bc_modify_data.sdf_qos.max_bit_rate_dl; 
      esm_bearer_ctxt_info_log.eps_qos[4] = ctxt_ptr->bc_modify_data.sdf_qos.gbr_ul; 
      esm_bearer_ctxt_info_log.eps_qos[5] = ctxt_ptr->bc_modify_data.sdf_qos.gbr_dl; 
      esm_bearer_ctxt_info_log.eps_qos[6] = ctxt_ptr->bc_modify_data.sdf_qos.max_bit_rate_ul_ex; 
      esm_bearer_ctxt_info_log.eps_qos[7] = ctxt_ptr->bc_modify_data.sdf_qos.max_bit_rate_dl_ex; 
      esm_bearer_ctxt_info_log.eps_qos[8] = ctxt_ptr->bc_modify_data.sdf_qos.gbr_ul_ex; 
      esm_bearer_ctxt_info_log.eps_qos[9] = ctxt_ptr->bc_modify_data.sdf_qos.gbr_dl_ex;
	}
	else
	{
      esm_bearer_ctxt_info_log.eps_qos[0] = ctxt_ptr->sdf_qos.length; 
      esm_bearer_ctxt_info_log.eps_qos[1] = ctxt_ptr->sdf_qos.qci; 
      esm_bearer_ctxt_info_log.eps_qos[2] = ctxt_ptr->sdf_qos.max_bit_rate_ul; 
      esm_bearer_ctxt_info_log.eps_qos[3] = ctxt_ptr->sdf_qos.max_bit_rate_dl; 
      esm_bearer_ctxt_info_log.eps_qos[4] = ctxt_ptr->sdf_qos.gbr_ul; 
      esm_bearer_ctxt_info_log.eps_qos[5] = ctxt_ptr->sdf_qos.gbr_dl; 
      esm_bearer_ctxt_info_log.eps_qos[6] = ctxt_ptr->sdf_qos.max_bit_rate_ul_ex; 
      esm_bearer_ctxt_info_log.eps_qos[7] = ctxt_ptr->sdf_qos.max_bit_rate_dl_ex; 
      esm_bearer_ctxt_info_log.eps_qos[8] = ctxt_ptr->sdf_qos.gbr_ul_ex; 
      esm_bearer_ctxt_info_log.eps_qos[9] = ctxt_ptr->sdf_qos.gbr_dl_ex; 
	}    

    //memcpy(&esm_bearer_ctxt_info_log.ota_context_data, &data_ptr->nas_hdr.ota_msg_ptr,data_ptr->nas_hdr.ota_msg_len);
  }
  else
  {
    esm_bearer_ctxt_info_log.context_type = 0xff;
    esm_bearer_ctxt_info_log.bearer_id = 0;
    esm_bearer_ctxt_info_log.bearer_state = (byte)ESM_BC_INACTIVE;
    esm_bearer_ctxt_info_log.connection_id = ESM_INVALID_CON_ID;;
    esm_bearer_ctxt_info_log.sdf_id = 0xff;  
    esm_bearer_ctxt_info_log.lbi_valid = FALSE;
    esm_bearer_ctxt_info_log.lbi = 0;
    esm_bearer_ctxt_info_log.rb_id = 0;  
    esm_bearer_ctxt_info_log.eps_qos[0] = 0; /* This should make the packet to be discarded by the tools*/

    //memcpy(&esm_bearer_ctxt_info_log.ota_context_data, &data_ptr->nas_hdr.ota_msg_ptr,data_ptr->nas_hdr.ota_msg_len);
  }
  send_esm_bearer_ctxt_log_info(&esm_bearer_ctxt_info_log);

}

/*==============================================================================

  FUNCTION:  esm_bcm_process_OTA_message

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_process_OTA_message() does goes here.

    @detail
    Processes the OTA messages
*/
/*============================================================================*/
static void esm_bcm_process_OTA_message
(
  esm_bcm_T                         *mgr_ptr,
  lte_nas_esm_incoming_msg_type     *data_ptr,
  word                               sdf_id,
  byte                               lbi,
  lte_nas_message_id                 trigger_msg_id
)
{
  esm_ctxt_state_ptr_type state_ptr = NULL;
  struct esm_eps_bearer_context* ctxt_ptr = NULL;
  lte_nas_esm_hdr_type* esm_hdr = NULL;
  
  eps_bearer_id_T bearer_id = 0;
  lte_nas_local_cause_T local_cause;
  esm_cause_T ota_cause;
  lte_nas_message_id msg_type = data_ptr->nas_hdr.msg_id;



  esm_hdr = esm_get_esm_hdr_from_esm_incoming_msg(data_ptr);
  if(esm_hdr == NULL)
  {
    MSG_ERROR_DS_0(SM_SUB, "ESM: ESM header is NULL");
    return;
  }
  bearer_id = esm_hdr->bearer_id;

  /* Find the activated bearer context using the given bearer ID. 
     If none found, ctxt_ptr is set to NULL. */
	   

    ctxt_ptr = esm_bcm_find_bearer_context(mgr_ptr,bearer_id);


  if (ctxt_ptr != NULL) 
  {
    state_ptr = ctxt_ptr->eps_bearer_state_ptr;
  }

  /* Validate the given bearer ID */
  if(esm_validate_OTA_message(data_ptr, 
                              mgr_ptr, 
                              sdf_id,
                              lbi, 
                              trigger_msg_id, 
                              ctxt_ptr,
                              esm_hdr)
     == FALSE)
  {
    return;
  }

  switch (msg_type)
  {               
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
      if((ctxt_ptr != NULL) && (state_ptr->state() != ESM_BC_INACTIVE))
      {
        /* TS 24.301  6.4.1.5 Abnormal case in UE 
           a) Default bc act request for an already activated default bc
              - UE shall locally deactivate the existing default bc and all the 
                associated dedicated bc, if any, and proceed with the requested 
                default EPS bearer context activation.
           b) Default bc act request for an already activated dedicated bc
              - UE shall locally deactivate the existing dedicated EPS bearer 
                context and proceed with the requested default EPS bearer 
                context activation */
        
        MSG_HIGH_DS_0(SM_SUB, "ESM: ACT DEFAULT BC REQ rcved for an existing context");
        ota_cause.valid = FALSE;
        local_cause.valid = TRUE;
        local_cause.local_cause = 
                             ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER;
        /* Deactivate bc following rule of a or b listed above */
        mgr_ptr->deact_ctxts_ptr(local_cause, ota_cause, bearer_id);
        ctxt_ptr = NULL;
       
      }
      /* bearer context is not existing or is deactivated now. Allocate it. */
      if (ctxt_ptr == NULL)
	  {
        // For cases where NW has already set up RAB corresponding to a specific EPS ID before activating the context, we would have allocated memory for the context already.
        // In that case, do not allocate memory again, use the allocated memory for activation of this context now, as we will need the RID which we have remembered. 
        ctxt_ptr = esm_bcm_allocate_context(mgr_ptr, bearer_id);
	  }
      ASSERT(ctxt_ptr != NULL);
	  
      /* Save the sdf_id to be used in the message sent to CM/DS */
      ctxt_ptr->sdf_id = sdf_id;
      state_ptr = ctxt_ptr->eps_bearer_state_ptr;

      if((data_ptr->default_bearer_context_req.ti.valid == TRUE) &&
         (data_ptr->default_bearer_context_req.neg_qos.valid == TRUE))
      {
        ctxt_ptr->sm_group_id = emm_gen_sm_group_id(mgr_ptr);
      }
      /* Proceed with the default EPS bearer context activation */
      state_ptr->ctxt_process_act_default_bearer_ctxt_req(ctxt_ptr, 
                                      &(data_ptr->default_bearer_context_req));
      mgr_ptr->first_def_bearer_ctxt_received = TRUE;
       /* Set this flag to TRUE, so that while logging context_info log pkt, 
         bearer_ctxt is set to "TRUE" */
      ctxt_ptr->default_bearer_ctxt = TRUE; 
      break;
    case DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST:
      /* The case of NULL ctxt_ptr should have been already taken care of in 
         previous function call of esm_validate_OTA_message. To reach here, 
         ctxt_ptr must not be NULL. */
      
      /* Save the sdf_id to be used in the message sent to CM/DS */
	  
	  ASSERT(ctxt_ptr != NULL);
      /* Save the sdf_id to be used in the message sent to CM/DS */
      ctxt_ptr->sdf_id = sdf_id;
      ctxt_ptr->eps_bearer_state_ptr->ctxt_process_deact_bearer_ctxt_req(
                     ctxt_ptr, &(data_ptr->deactivate_eps_bearer_context_req));
	  ctxt_ptr = NULL;

	  
	  
      break;
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST:
      if((ctxt_ptr != NULL) && (state_ptr->state() != ESM_BC_INACTIVE))
      {
        /* TS 24.301  6.4.2.5 Abnormal case in UE 
           a) Dedicated bc act request for an already activated default bc
              - UE shall locally deactivate the existing default bc and all the 
                associated dedicated bc, if any, and proceed with the requested 
                dedicated EPS bearer context activation.
           b) Dedicated bc act request for an already activated dedicated bc
              - UE shall locally deactivate the existing dedicated EPS bearer 
                context and proceed with the requested sedicated EPS bearer 
                context activation */
         
        MSG_HIGH_DS_0(SM_SUB, "ESM: ACT DEDICATED BC REQ rcved for an existing context");
        ota_cause.valid = FALSE;
        local_cause.valid = TRUE;
        local_cause.local_cause = 
                             ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER;
        /* Deactivate bc following rule of a or b listed above */
        mgr_ptr->deact_ctxts_ptr(local_cause, ota_cause, bearer_id);
        ctxt_ptr = NULL;

      }
      /* bearer context is not existing or is deactivated now. Allocate it. */
      if (ctxt_ptr == NULL)
	  {
        // For cases where NW has already set up RAB corresponding to a specific EPS ID before activating the context, we would have allocated memory for the context already.
        // In that case, do not allocate memory again, use the allocated memory for activation of this context now, as we will need the RID which we have remembered. 
        ctxt_ptr = esm_bcm_allocate_context(mgr_ptr, bearer_id);
	  }
      ASSERT(ctxt_ptr != NULL);
	  
      /* Save the sdf_id to be used in the message sent to CM/DS */
      ctxt_ptr->sdf_id = sdf_id;
      state_ptr = ctxt_ptr->eps_bearer_state_ptr;
      /* Proceed with the default EPS bearer context activation */
      state_ptr->ctxt_process_act_dedicated_bearer_ctxt_req(ctxt_ptr, 
                                    &(data_ptr->dedicated_bearer_context_req));
      break;
    case MODIFY_EPS_BEARER_CONTEXT_REQUEST:
      /* The case of NULL ctxt_ptr should have been already taken care of in 
         previous function call of esm_validate_OTA_message. To reach here, 
         ctxt_ptr must not be NULL. */
      ASSERT(ctxt_ptr != NULL);
	  
      /* Save the sdf_id to be used in the message sent to CM/DS */
      ctxt_ptr->sdf_id = sdf_id;
      ctxt_ptr->eps_bearer_state_ptr->ctxt_process_modify_bearer_ctxt_req(
                              ctxt_ptr, &data_ptr->modify_eps_bearer_ctxt_req);
      break;
    case ESM_STATUS:
      /* Locally deactivate */
      local_cause.valid = FALSE;
      ota_cause.valid = TRUE;
      ota_cause.esm_cause = data_ptr->mt_esm_status.esm_cause;

      mgr_ptr->deact_ctxts_ptr(local_cause, ota_cause,
                            data_ptr->mt_esm_status.lte_nas_esm_hdr.bearer_id);
      break;

    case ESM_NOTIFICATION:
      esm_build_and_send_esm_notification();
      break;

    default:
      MSG_HIGH_DS_1(SM_SUB, "ESM: Unknown ESM msg type - %u", msg_type);
      esm_send_esm_status(&data_ptr->esm_unknown_msg.lte_nas_esm_hdr, 
                           NAS_ESM_MSG_TYPE_NON_EXISTANT);
      break;
  }

  /* ESM LOG PACKETS */
  send_esm_ctxt_info_log(ctxt_ptr);

  ctxt_ptr = NULL;

} /* esm_bcm_process_OTA_message() */

/*==============================================================================

  FUNCTION:  esm_bcm_process_active_eps_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_process_active_eps_ind() does goes here.

    @detail
    Processes the active EPS indication
*/
/*============================================================================*/
void esm_bcm_process_active_eps_ind
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_active_eps_ind_T* msg_ptr/*!< REQUIRED short parameter description */
)
{
} /* esm_bcm_process_active_eps_ind() */

/*==============================================================================

  FUNCTION:  esm_ctxt_process_sig_conn_released_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_process_sig_conn_released_ind() does goes here.

    @detail
    Processes the signaling connection released indication
*/
/*============================================================================*/
void esm_bcm_process_sig_conn_released_ind
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_sig_connection_released_ind_T* msg_ptr   /*!< REQUIRED short parameter description */
)
{


} /* esm_bcm_process_sig_conn_released_ind() */


/*==============================================================================

  FUNCTION:  esm_bcm_deactivate_all_contexts()

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_deactivate_all_contexts() does goes here.

    @detail
    Sends deactivate indications for all the active contexts

*/
/*============================================================================*/
void esm_bcm_deactivate_all_contexts
(
  struct esm_bcm* mgr_ptr,   
  lte_nas_local_cause_T local_cause,
  esm_cause_T cause
)
{
  byte index = 0;

  ASSERT(mgr_ptr != NULL);

  for(index=0; index < ESM_MAX_EPS_BEARERS; index++)
  {
    esm_eps_bearer_context_T* ctxt_ptr =  mgr_ptr->esm_bc[index];
	if (ctxt_ptr != NULL)
    {
      esm_ctxt_state_ptr_type state_ptr  = ctxt_ptr->eps_bearer_state_ptr;

      if( state_ptr->state() != ESM_BC_INACTIVE )
      {
        /* First deactivate the associated dedicated bearers and then the default bearer*/
        if(ctxt_ptr->default_bearer_ctxt == TRUE)
        {
          esm_bcm_deactivate_associated_ded_ctxts(mgr_ptr,local_cause,cause,ctxt_ptr->eps_bearer_id);
        }
        /* Since we don't know in which order the contexts are, we will deactivate whichever bearer we encounter. 
             If we encountered default bearer we would have deactivated the associated dedicated bearers if they are not 
             already deactivated*/
        esm_build_and_send_deactivate_ind(ctxt_ptr,local_cause,cause);
        esm_bcm_reset_ctxt(ctxt_ptr);

		ctxt_ptr = NULL;
      }
      else
	  {
        // For cases where RAB has been established but bearer not activated yet, clear up memory
        esm_modem_mem_free(mgr_ptr->esm_bc[index]);
		mgr_ptr->esm_bc[index] = NULL;
		ctxt_ptr = NULL;
	  }
	}
                    
  }

#ifdef FEATURE_LTE_REL9
  if((esm_get_emc_srv_status() == LTE_NAS_EMC_SRV_PENDING) &&
     ((local_cause.valid == TRUE) && 
      (local_cause.local_cause == EMM_ATTACH_STARTED)))
  {
    mgr_ptr->on_attach_def_bearer_pending = ESM_BC_ON_ATTACH_DEF_BEARER_PENDING;
  }
  else
#endif
  {
    /* Reset on_attach_def_bearer_pending flag */
    mgr_ptr->on_attach_def_bearer_pending = ESM_BC_ON_ATTACH_DEF_BEARER_NONE;
  }
  mgr_ptr->first_def_bearer_ctxt_received = FALSE;
  sm_esm_isr_status_set(FALSE);  

} /* esm_bcm_deactivate_all_contexts() */

/*==============================================================================

  FUNCTION:  esm_bcm_process_deatch_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_clean_up() does goes here.

    @detail
    Sends deactivate indications for all the active contexts

*/
/*============================================================================*/
void esm_bcm_clean_up
(
  struct esm_bcm* mgr_ptr,   /*!< REQUIRED short parameter description */
  lte_nas_local_cause_T local_cause
)
{
  esm_cause_T cause;

  /* For the detach case it must be a local cause, so the OTa cause is always false*/
  cause.valid = FALSE;
  esm_bcm_deactivate_all_contexts(mgr_ptr,local_cause,cause);
} /* esm_bcm_clean_up() */

/*==============================================================================

  FUNCTION:  esm_bcm_handle_esm_failure_Ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_handle_esm_failure_Ind() does goes here.

    @detail
    Handles the esm_failure indication from the EMM.If the cause of failure is SERVICE REQ FAILURE, then 
    BCM calls all the contexts blindly to process the event.
    In all other cases it extracts the instance id (context connection id) from the transaction id 
    to identify the context that should handle the failure
*/
/*============================================================================*/

static void esm_bcm_handle_esm_failure_Ind 
(
  struct esm_bcm* mgr_ptr,
  esm_failure_ind_T* msg_ptr
)
{
  byte instance_id = 0;
  esm_ctxt_state_ptr_type state_ptr = NULL; 

  /* Don't check for validity as EMM currently sets this flag only for service request failures and for service request
  failure we catch it anyway despite the valid flag*/
  switch(msg_ptr->emm_cause.local_cause)
  {
      /* For the attach failure case there is nothing to do*/
    case EMM_ATTACH_FAILED:
      break;

      /* In the case of service request ESM needs to respond with individual drb reestablish failure irrespective of which 
      context requested the drb reestablishment. So, in this case BCM doesn't use the connection id retrieved from the the 
      trnsaction id to identify the context to be called*/
    case LTE_NAS_SERVICE_REQ_FAILED:
    case LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE:
    default:
      /* Need to call the context to process based on trnsaction id*/
      instance_id = esm_get_instance_id_from_trans_id(msg_ptr->trans_id);
      /* ESM uses ESM_RESERVED_CONNECTION_ID when sending autonoumous reject messages to the network as there is no context exists
         if EMM failed to send such messages ESM should ignore this failure and stay silent*/
      if(instance_id != ESM_RESERVED_CONNECTION_ID)
      {
        ASSERT(instance_id < ESM_MAX_EPS_BEARERS);
        /*Added the following check just to avoid Klocworks defect*/
        if((instance_id < ESM_MAX_EPS_BEARERS) && (mgr_ptr->esm_bc[instance_id])!= NULL)
        {
          state_ptr = mgr_ptr->esm_bc[instance_id]->eps_bearer_state_ptr;
          state_ptr->ctxt_process_esm_failure_ind((mgr_ptr->esm_bc[instance_id]),msg_ptr);
        }        
      }
      break;
    }
} /*esm_bcm_handle_esm_failure_Ind*/



/*==============================================================================

  FUNCTION:  esm_bcm_store_pending_messages

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_store_pending_messages() does goes here.

    @detail


    IMPORTANT NOTE:
    
*/
/*============================================================================*/
/*static boolean esm_bcm_check_and_store_pending_messages
(
  struct esm_cmd* cmd_ptr,
  struct esm_bcm* mgr_ptr
)
{
  lte_nas_esm_incoming_msg_type* msg_ptr = NULL;
  byte index = 0;
  boolean continue_processing = TRUE;

  ASSERT(cmd_ptr != NULL);

  msg_ptr =  &cmd_ptr->cmd.esm_data_ind.esm_incoming_msg;
*/

  /* DO NOT CHECK FOR THE nas_hdr valid flag here, as we need to process this irrespective of that, as we need 
  to send a recject even if the message is bad*/

/*  if(mgr_ptr->on_attach_def_bearer_pending == ESM_BC_ON_ATTACH_DEF_BEARER_PENDING)
  {
    if (msg_ptr->nas_hdr.msg_id != ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST)
    {
      for(index = 0; index < ESM_MAX_PENDING_MSGS; index++)
      {
        if(mgr_ptr->pending_msg_list[index].valid == FALSE)
        {
          mgr_ptr->pending_msg_list[index].pending_cmd.cmd = cmd_ptr->cmd;
          mgr_ptr->pending_msg_list[index].valid = TRUE;
 
          continue_processing = FALSE;
          break;
        }
      }
    }
  }*/ /* This is handling the scenario where default 
    bearer setup failed at the time of attach and N/W sent dedicated bearer setups and those messages are being processed now. 
    Since the default bearer failed we should also assume that dedicated bearers also failed, so just dorp the messages. 
    The EMM would initiate another round of attach*/
  /*else if(mgr_ptr->on_attach_def_bearer_pending == ESM_BC_ON_ATTACH_DEF_BEARER_FAILED)
  {
    MSG_ERROR_DS_0(SM_SUB, "ESM: dropping messages as the default bearer setup FAILED. This situation happens when multiple bearers are setup at the time of attach. Default bearer setup failed while dedicated bearer setups are queued");
    continue_processing = FALSE;
  }
  if(index == ESM_MAX_PENDING_MSGS)
  {
    MSG_ERROR_DS_0(SM_SUB, "ESM: ERROR pending message list FULL, dropping messages");
    continue_processing = FALSE;
  }
  return continue_processing;
}*/ /*esm_bcm_store_pending_messages*/


/*static void discard_all_pending_messages(struct esm_bcm* mgr_ptr)
{
  byte index = 0;

  for(index = 0; index < ESM_MAX_PENDING_MSGS; index++)
  {
    mgr_ptr->pending_msg_list[index].valid = FALSE;
  }
}*/

/*==============================================================================

  FUNCTION:  esm_bcm_handle_eps_bearer_status_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what iesm_bcm_handle_eps_bearer_status_ind() does goes here.

    @detail
    This function process the eps_bearer_update_status messsage received from the EMM. The bearer status update message
    indicates the contexts that are active at the MME. Any contexts that are atcive at ESM but inactive at MME, those contexts
    will be deactivated locally by the ESM.

    IMPORTANT NOTE:
    If the scheme in esm_bcm_allocate_context is changed for context allocation, then this function needs a change.
    This function assumes that there is a strict relation between the bearer id and connection id
    
*/
/*============================================================================*/
static void esm_bcm_handle_eps_bearer_status_ind
(
  struct esm_bcm* mgr_ptr,esm_eps_bearer_status_ind_T* msg_ptr
)
{
  byte index = 0;
  lte_nas_local_cause_T local_cause;
  esm_cause_T  ota_cause;
  esm_eps_bearer_context_T* ctxt_ptr = NULL;


  ota_cause.valid = FALSE;
  local_cause.valid = TRUE;

  local_cause.local_cause = ESM_SYNC_UP_WITH_NW;

  for(index = 0; index < ESM_MAX_EPS_BEARERS; index++)
  {
    ctxt_ptr = NULL;
    /* Context activei, it is also possible to check the status, but this may be fine, this way if a context is pending
       we will be sending deactivate for that also, will this cause problems? Is that even prossible ?*/
    if (mgr_ptr->esm_bc[index] != NULL && mgr_ptr->esm_bc[index]->connection_id != ESM_INVALID_CON_ID)
    {
      switch(mgr_ptr->esm_bc[index]->connection_id)
      {
      case 0:
        /* Does N/W say the context is inactive on it's end?*/
        if(msg_ptr->context_status.eps_bearer_context_5_exists == FALSE)
        {
          ctxt_ptr = mgr_ptr->esm_bc[index];
        }
        break;
      case 1:
        /* Does N/W say the context is inactive on it's end?*/
        if(msg_ptr->context_status.eps_bearer_context_6_exists == FALSE)
        {
          ctxt_ptr = mgr_ptr->esm_bc[index];
        }
        break;
      case 2:
        /* Does N/W say the context is inactive on it's end?*/
        if(msg_ptr->context_status.eps_bearer_context_7_exists == FALSE)
        {
          ctxt_ptr = mgr_ptr->esm_bc[index];
        }
        break;
      case 3:
        /* Does N/W say the context is inactive on it's end?*/
        if(msg_ptr->context_status.eps_bearer_context_8_exists == FALSE)
        {
          ctxt_ptr = mgr_ptr->esm_bc[index];
        }
        break;
      case 4:
        /* Does N/W say the context is inactive on it's end?*/
        if(msg_ptr->context_status.eps_bearer_context_9_exists == FALSE)
        {
          ctxt_ptr = mgr_ptr->esm_bc[index];
        }
        break;
      case 5:
        /* Does N/W say the context is inactive on it's end?*/
        if(msg_ptr->context_status.eps_bearer_context_10_exists == FALSE)
        {
          ctxt_ptr = mgr_ptr->esm_bc[index];
        }
        break;
      case 6:
        /* Does N/W say the context is inactive on it's end?*/
        if(msg_ptr->context_status.eps_bearer_context_11_exists == FALSE)
        {
          ctxt_ptr = mgr_ptr->esm_bc[index];
        }
        break;
      case 7:
        /* Does N/W say the context is inactive on it's end?*/
        if(msg_ptr->context_status.eps_bearer_context_12_exists == FALSE)
        {
          ctxt_ptr = mgr_ptr->esm_bc[index];
        }
        break;
      case 8:
        /* Does N/W say the context is inactive on it's end?*/
        if(msg_ptr->context_status.eps_bearer_context_13_exists == FALSE)
        {
          ctxt_ptr = mgr_ptr->esm_bc[index];
        }
        break;
      case 9:
        /* Does N/W say the context is inactive on it's end?*/
        if(msg_ptr->context_status.eps_bearer_context_14_exists == FALSE)
        {
          ctxt_ptr = mgr_ptr->esm_bc[index];
        }
        break;
      case 10:
        /* Does N/W say the context is inactive on it's end?*/
        if(msg_ptr->context_status.eps_bearer_context_15_exists == FALSE)
        {
          ctxt_ptr = mgr_ptr->esm_bc[index];
        }
        break;
      default:
        MSG_ERROR_DS_0(SM_SUB, "ESM: We should never be here... There exists only 0 to 10 connection ids. If we are here something is broken....");
        break;
      }
      if(ctxt_ptr != NULL)
      {
        MSG_HIGH_DS_1(SM_SUB, "ESM: Bearer %d to be deactivated - Not matching EMM bearer status",
                      ctxt_ptr->eps_bearer_id);
        esm_deact_ctxt_upon_sync_up(ctxt_ptr, local_cause, ota_cause, mgr_ptr);
        ctxt_ptr = NULL;
      }
    }
  }
}/*esm_bcm_handle_eps_bearer_status_ind*/

/*==============================================================================

  FUNCTION:  esm_bcm_process_rat_change

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_process_rat_changedoes goes here.

    @detail
    Handles isr_status_or_rat_change_ind

*/
/*============================================================================*/
void esm_bcm_process_rat_change
(
  struct esm_bcm* mgr_ptr   /*!< REQUIRED short parameter description */
)
{
  byte index = 0;
  lte_nas_isr_status_type isr;

  isr.valid = FALSE;

  for(index = 0; index < ESM_MAX_EPS_BEARERS; index++)
  {
    /* Check if any PDP activated in UMTS after ISR is ON exists.  Or check if a PDP
    ** got modified or last PDP got deactivated while in GW. If YES send 
    ** LTE_EPS_BEARER_STATUS_CMD with ISR OFF to EMM. Also reset the ISR
    ** context activation status in both ESM and SM.
    */
    if((mgr_ptr->esm_bc[index] != NULL) && ((mgr_ptr->esm_bc[index]->isr_bearer_ctxt_act_status == CTXT_ACT_AFTER_ISR_IN_GW) ||
       (sm_deact_isr_on_interrat_status_get() == SM_DEACT_ISR_ON_INTERRAT_TO_LTE)))
    {
      isr.valid = TRUE;
      isr.isr_status = FALSE;
      break;
    }
  }
  if(isr.valid == TRUE && isr.isr_status == FALSE)
  {
    sm_esm_isr_status_set(FALSE);
  }
  esm_build_and_send_eps_bearer_status_update_ind(mgr_ptr,isr);
} /* esm_bcm_process_rat_change() */


/*==============================================================================

  FUNCTION:  esm_bcm_handle_isr_status_or_rat_change_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_handle_isr_status_or_rat_change_ind does goes here.

    @detail
    Handles isr_status_or_rat_change_ind

*/
/*============================================================================*/
void esm_bcm_handle_isr_status_or_rat_change_ind
(
  struct esm_bcm* mgr_ptr,   /*!< REQUIRED short parameter description */
  esm_get_isr_status_ind_T* msg_ptr
)
{
  MSG_HIGH_DS_0(SM_SUB, "ESM: EMM sent ISR_STATUS Query");

  esm_bcm_process_rat_change(mgr_ptr);
} /* esm_bcm_handle_isr_status_or_rat_change_ind() */


/*==============================================================================

  FUNCTION:  handle_emm_message

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what handle_emm_message() does goes here.

    @detail
    Processes the emm commands.
*/
/*============================================================================*/
static void handle_emm_message
(
  struct esm_cmd* cmd_ptr, 
  struct esm_bcm* mgr_ptr
)
{
  lte_nas_esm_incoming_msg_type* msg_data =  NULL;
  esm_ctxt_state_ptr_type state_ptr = NULL;   
  byte index = 0;
  lte_nas_local_cause_T local_cause;

  MSG_LOW_DS_0(SM_SUB, "ESM: Enter BCM: handle_emm_message\n");

  /* All this code should go to the state processing? Here we should only know 
  whether to invoke process_cm or process_nw or process_internal? Probably not and 
  it is difficult to do it that way */
  switch (cmd_ptr->cmd.msg_hdr.id)
  {
    case NAS_ESM_DATA_IND:
      msg_data =  &cmd_ptr->cmd.esm_data_ind.esm_incoming_msg;
      esm_bcm_process_OTA_message(mgr_ptr, 
                                  msg_data,
                                  cmd_ptr->cmd.esm_data_ind.esm_hdr.sdf_id,
                                  cmd_ptr->cmd.esm_data_ind.esm_hdr.lbi,
                                  cmd_ptr->cmd.esm_data_ind.esm_hdr.trigger_msg_id);
      break;
    case NAS_ESM_FAILURE_IND:
      /* Reset on_attach_def_bearer_pending flag if ongoing attach procedure 
         is failed. This is a special case with transaction ID of LTE_ESM_BPM */
      if((cmd_ptr->cmd.esm_failure_ind.emm_cause.valid == TRUE) &&
         (cmd_ptr->cmd.esm_failure_ind.emm_cause.local_cause == EMM_ATTACH_FAILED))
      {
        mgr_ptr->on_attach_def_bearer_pending = ESM_BC_ON_ATTACH_DEF_BEARER_NONE;
        mgr_ptr->first_def_bearer_ctxt_received = FALSE;
      }
      if(esm_get_module_from_trans_id(cmd_ptr->cmd.esm_failure_ind.trans_id) != (unsigned)LTE_ESM_BCM)
      {
        return;
      }
      esm_bcm_handle_esm_failure_Ind(mgr_ptr, &(cmd_ptr->cmd.esm_failure_ind) );
      break;
    case NAS_ESM_SIG_CONNECTION_RELEASED_IND:  
      for(index = 0; index < ESM_MAX_EPS_BEARERS; index++)
      {
        if (mgr_ptr->esm_bc[index] != NULL)
        {
          if (mgr_ptr->esm_bc[index]->eps_bearer_state_ptr->state() != ESM_BC_INACTIVE)
          {
            state_ptr = mgr_ptr->esm_bc[index]->eps_bearer_state_ptr;
            state_ptr->ctxt_process_sig_conn_released_ind((mgr_ptr->esm_bc[index]),&(cmd_ptr->cmd.esm_sig_connection_released_ind));
          }
		  else
		  {
            // This is the case where we have allocated memory for a context not activeted by NW yet, but RAB has already been established
            // corresponding to a specific EPS ID.
            esm_modem_mem_free(mgr_ptr->esm_bc[index]);
		    mgr_ptr->esm_bc[index] = NULL;
		  }
        }
      }
      break;              
    case NAS_ESM_ACTIVE_EPS_IND:
      /* print out the contents */
      MSG_HIGH_DS_3(SM_SUB, "ESM: Active EPS IND - Added %d, rmved %d, active %d",
                    cmd_ptr->cmd.esm_active_eps_ind.num_added_bearers,
                    cmd_ptr->cmd.esm_active_eps_ind.num_removed_bearers,
                    cmd_ptr->cmd.esm_active_eps_ind.num_active_bearers);

      for(index =0; index < cmd_ptr->cmd.esm_active_eps_ind.num_active_bearers; index++)
      {
        MSG_HIGH_DS_2(SM_SUB, "ESM: Active EPS bearer ID %d, RB_ID %d",
                      cmd_ptr->cmd.esm_active_eps_ind.active_eps_bearers[index].eps_id,
                      cmd_ptr->cmd.esm_active_eps_ind.active_eps_bearers[index].rb_id);
      }

      for(index =0; index < cmd_ptr->cmd.esm_active_eps_ind.num_added_bearers; index++)
      {
        byte conn_id = 0;

        if((cmd_ptr->cmd.esm_active_eps_ind.added_eps_bearers[index].eps_id
                                                    < ESM_STARTING_BEARER_ID)
           ||
           (cmd_ptr->cmd.esm_active_eps_ind.added_eps_bearers[index].eps_id
                                                    > ESM_ENDING_BEARER_ID))
        {

            MSG_ERROR_DS_2(SM_SUB,"=ESM= Rcved invalid Added EPS bearer ID %d, index %d.\
					 Ignoring this Bearer ID",
		             cmd_ptr->cmd.esm_active_eps_ind.added_eps_bearers[index].eps_id, index);
        }
        else
        {
          MSG_HIGH_DS_2(SM_SUB, "ESM: Added EPS bearer ID %d, RB_ID %d",
                        cmd_ptr->cmd.esm_active_eps_ind.added_eps_bearers[index].eps_id,
                        cmd_ptr->cmd.esm_active_eps_ind.added_eps_bearers[index].rb_id);
        
          /* In case NW set radio bearer for inactive EPS bearer, save the radio beaer ID
             to be used later in case NW does not send reconfiguration message when EPS 
             bearer is established */
          conn_id = cmd_ptr->cmd.esm_active_eps_ind.added_eps_bearers[index].eps_id - ESM_STARTING_BEARER_ID;
          if(mgr_ptr->esm_bc[conn_id] == NULL)
          {
            mgr_ptr->esm_bc[conn_id] = esm_bcm_allocate_context(mgr_ptr, cmd_ptr->cmd.esm_active_eps_ind.added_eps_bearers[index].eps_id);
            mgr_ptr->esm_bc[conn_id]->unclaimed_rb_id = 
                  cmd_ptr->cmd.esm_active_eps_ind.added_eps_bearers[index].rb_id;
            mgr_ptr->esm_bc[conn_id]->eps_bearer_id = 
                cmd_ptr->cmd.esm_active_eps_ind.added_eps_bearers[index].eps_id;
          }
        }
      }

      for(index =0; index < cmd_ptr->cmd.esm_active_eps_ind.num_removed_bearers; index++)
      {
        MSG_HIGH_DS_2(SM_SUB, "ESM: Romved EPS bearer ID %d, RB_ID %d",
                      cmd_ptr->cmd.esm_active_eps_ind.removed_eps_bearers[index].eps_id,
                      cmd_ptr->cmd.esm_active_eps_ind.removed_eps_bearers[index].rb_id);
      }

      /* Process the ind for each bearer */
      for(index = 0; index < ESM_MAX_EPS_BEARERS; index++)
      {
        if (mgr_ptr->esm_bc[index] != NULL)
        {
          if (mgr_ptr->esm_bc[index]->eps_bearer_state_ptr->state() != ESM_BC_INACTIVE)
          {
            state_ptr = mgr_ptr->esm_bc[index]->eps_bearer_state_ptr;
            state_ptr->ctxt_process_active_eps_ind((mgr_ptr->esm_bc[index]),
                                               &(cmd_ptr->cmd.esm_active_eps_ind),
                                               mgr_ptr);
          }
        }  
      }
      break;              
    case NAS_ESM_DETACH_IND:
      /* Get detach indication, clean up all contexts*/
      local_cause.local_cause = cmd_ptr->cmd.esm_detach_ind.cause.local_cause;
      local_cause.valid = TRUE;
      esm_bcm_clean_up(mgr_ptr,local_cause);
      break;

    case NAS_ESM_EPS_BEARER_STATUS_IND:
      esm_bcm_handle_eps_bearer_status_ind(mgr_ptr,&(cmd_ptr->cmd.esm_eps_bearer_status_ind));
      break;

    case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
      /* To start attach procedure from scratch, need to clean up all contexts */
      local_cause.local_cause = EMM_DETACHED;
      local_cause.valid = TRUE;
      esm_bcm_clean_up(mgr_ptr,local_cause); 
      esm_build_and_send_get_pdn_connectivity_req_ind(
                            cmd_ptr->cmd.esm_get_pdn_connectivity_req.seq_num);
      break;
  case NAS_ESM_GET_ISR_STATUS_IND:
      esm_bcm_handle_isr_status_or_rat_change_ind(mgr_ptr, &(cmd_ptr->cmd.esm_isr_status_ind) );
      break;
  case NAS_ESM_ISR_STATUS_CHANGE_IND:
      MSG_HIGH_DS_1(SM_SUB, "ESM: EMM sent ISR_STATUS change indication status(%d)",
                    cmd_ptr->cmd.esm_isr_status_change_ind.isr_status);
      sm_esm_isr_status_set(cmd_ptr->cmd.esm_isr_status_change_ind.isr_status);
      break;

//  case NAS_ESM_1XCSFB_ESR_CALL_RSP:
//    esm_build_and_send_1xcsfb_call_rsp(&cmd_ptr->cmd.emm_1xCSFB_esr_call_rsp,);
//    break;
//  case NAS_ESM_1XCSFB_ESR_CALL_ABORT_RSP:
//    esm_build_and_send_1xcsfb_call_abort_rsp(&cmd_ptr->cmd.emm_1xCSFB_esr_call_abort_rsp);
//    break;
    default:
      MSG_ERROR_DS_1(SM_SUB, "ESM: ESM received UNKNOWN message from EMM with id %d", cmd_ptr->cmd.msg_hdr.id);
      break;
  }
} /* handle_emm_message() */

/*==============================================================================

  FUNCTION:  handle_cm_message

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what handle_cm_message() does goes here.

    @detail
    Processes the CM commands
*/
/*============================================================================*/


static void handle_cm_message
(
  struct esm_cmd* cmd_ptr, 
  struct esm_bcm* mgr_ptr,
  byte con_id
)
{
  esm_eps_bearer_context_T* ctxt_ptr = NULL;
  esm_ctxt_state_ptr_type state_ptr = NULL;   
  lte_nas_local_cause_T local_cause; 
  esm_cause_T cause;

  MSG_LOW_DS_1(SM_SUB, "ESM: Enter BCM: handle_cm_message with CONNECTION ID =%d\n",con_id);

  local_cause.valid = FALSE;
  cause.valid = FALSE;

  /* Connection id is the index into the bearer context array. 
  Using that retrieve the bearer context*/
  /* Should we need to validate the con_id for bound check before using it.
  The value should be between 0 to 10*/
  if (con_id >= ESM_MAX_EPS_BEARERS) 
  {
    MSG_ERROR_DS_1(SM_SUB, "ESM: Invalid Connection Id %d received by ESM %d %d", con_id);
    return;
  }
  ctxt_ptr = mgr_ptr->esm_bc[con_id];
  /* Retrieve the state ptr */
  if (ctxt_ptr != NULL)
  {
    state_ptr = ctxt_ptr->eps_bearer_state_ptr;
  }	
  /* All this code should go to the state processing? Here we should only know 
  whether to invoke process_cm or process_nw or process_internal? Probably not and 
  it is difficult to do it that way */
  switch (cmd_ptr->cmd.msg_hdr.id)
  {
    case NAS_ESM_BEARER_RESOURCE_ALLOC_REQ:
      break;
    case NAS_ESM_PDN_CONNECTIVTY_REQ:
      break;
    case NAS_ESM_PDN_DISCONNECT_REQ:
      break;
    case NAS_ESM_BEARER_RESOURCE_ALLOC_ABORT_REQ:
      break;
    case NAS_ESM_PDN_CONNECTIVITY_ABORT_REQ:
      break;
    case NAS_ESM_DRB_REESTABLISH_REQ:
      MSG_HIGH_DS_1(SM_SUB, "ESM: received NAS_ESM_DRB_REESTABLISH_REQ, Bearer ID =%d \n",
                    cmd_ptr->cmd.esm_drb_reestablish_req.eps_bearer_id);
      ASSERT(cmd_ptr->cmd.esm_drb_reestablish_req.eps_bearer_id >= MIN_NSAPI_NUM);
	  if (ctxt_ptr != NULL)
	  {
        ctxt_ptr->last_msg_sent = ESM_LAST_UNDEFINED;
        esm_build_and_send_service_request(ctxt_ptr,cmd_ptr->cmd.esm_drb_reestablish_req.call_type); 
	  }	
	  else
	  {
        esm_failure_ind_T failure_ind;
		memset((void *)&failure_ind,0,sizeof(esm_failure_ind_T));
		failure_ind.emm_cause.valid = TRUE;
		failure_ind.emm_cause.local_cause = LTE_NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT;
        MSG_ERROR_DS_1(SM_SUB,"ESM: No context for con id %d exists at NAS",con_id); 
	    esm_build_and_send_cm_drb_reestablish_reject_ind(ctxt_ptr,&failure_ind,con_id);
      }
      break;
    case NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT_CNF:
      MSG_HIGH_DS_1(SM_SUB, "ESM: received Default Bearer CTXT ACCEPT, Bearer ID =%d \n",
                    cmd_ptr->cmd.esm_act_default_bearer_context_accept.eps_bearer_id);
      ASSERT(cmd_ptr->cmd.esm_act_default_bearer_context_accept.eps_bearer_id >= MIN_NSAPI_NUM);
	  if (ctxt_ptr != NULL)
	  {
	    state_ptr->state_name();
        state_ptr->ctxt_process_act_default_bearer_ctxt_accept(ctxt_ptr,&(cmd_ptr->cmd.esm_act_default_bearer_context_accept));
        if(mgr_ptr->on_attach_def_bearer_pending == ESM_BC_ON_ATTACH_DEF_BEARER_PENDING)
        {
          mgr_ptr->on_attach_def_bearer_pending = ESM_BC_ON_ATTACH_DEF_BEARER_SUCCESS;
          ctxt_ptr->on_attach_default_bearer = TRUE;
      
      
        }
	  }
	  else
	  {
        
	    MSG_ERROR_DS_1(SM_SUB,"ESM: No context for con id %d exists at NAS",con_id);
	  }
      break;
    case NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_REJ_IND:
      MSG_LOW_DS_2(SM_SUB, "ESM: Enter BCM: handle_CM_message, received Default Bearer CTXT REJECT, Bearer Id=%d, Cause=%d\n",
                   cmd_ptr->cmd.esm_act_default_bearer_context_rej.eps_bearer_id,
                   cmd_ptr->cmd.esm_act_default_bearer_context_rej.esm_cause.esm_cause);
      ASSERT(cmd_ptr->cmd.esm_act_default_bearer_context_rej.eps_bearer_id >= MIN_NSAPI_NUM);
	  if (ctxt_ptr != NULL)
	  {
        state_ptr->ctxt_process_act_default_bearer_ctxt_reject(ctxt_ptr,&(cmd_ptr->cmd.esm_act_default_bearer_context_rej));
        esm_modem_mem_free(mgr_ptr->esm_bc[con_id]);
		mgr_ptr->esm_bc[con_id] = NULL;
        ctxt_ptr = NULL;
	  }	
	  else
	  {
        
	    MSG_ERROR_DS_1(SM_SUB,"ESM: No context for con id %d exists at NAS",con_id);
	  }
      if(mgr_ptr->on_attach_def_bearer_pending == 
                                           ESM_BC_ON_ATTACH_DEF_BEARER_PENDING)
      {
        /* Only reset the flag during attch */
        mgr_ptr->on_attach_def_bearer_pending = 
                                            ESM_BC_ON_ATTACH_DEF_BEARER_FAILED;
      }
      break;
    case NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_ACCEPT_CNF:
      MSG_HIGH_DS_1(SM_SUB, "ESM: received Dedicated Bearer CTXT ACCEPT, Bearer ID =%d \n",
                    cmd_ptr->cmd.esm_act_dedicated_bearer_context_accept.eps_bearer_id);
      ASSERT(cmd_ptr->cmd.esm_act_dedicated_bearer_context_accept.eps_bearer_id >= MIN_NSAPI_NUM);
	  if (ctxt_ptr != NULL)
      {
        if(mgr_ptr->on_attach_def_bearer_pending == ESM_BC_ON_ATTACH_DEF_BEARER_SUCCESS)
        {
          state_ptr->ctxt_process_act_dedicated_bearer_ctxt_accept(ctxt_ptr,&(cmd_ptr->cmd.esm_act_dedicated_bearer_context_accept),mgr_ptr);
        }
        else /* If the state is not SUCCESS, then it must have FAILED, so we need to deactivate this context*/
        {
          local_cause.valid = TRUE;
          local_cause.local_cause = EMM_ATTACH_FAILED;
          esm_build_and_send_deactivate_ind(ctxt_ptr,local_cause,cause);
        
		  esm_bcm_reset_ctxt(ctxt_ptr);
		  ctxt_ptr = NULL;
		  
        }
	  }
	  else
	  {
        
	    MSG_ERROR_DS_1(SM_SUB,"ESM: No context for con id %d exists at NAS",con_id);
	  }

      break;
    case NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_REJ_IND:
      MSG_HIGH_DS_2(SM_SUB, "ESM: received Dedicated Bearer CTXT REJECT, Bearer ID =%d , Caus=%d\n",
                    cmd_ptr->cmd.esm_act_dedicated_bearer_context_rej.eps_bearer_id,
                    cmd_ptr->cmd.esm_act_dedicated_bearer_context_rej.esm_cause.esm_cause);
      ASSERT(cmd_ptr->cmd.esm_act_dedicated_bearer_context_rej.eps_bearer_id >= MIN_NSAPI_NUM);
      if (ctxt_ptr != NULL)
      {
        state_ptr->ctxt_process_act_dedicated_bearer_ctxt_reject(ctxt_ptr,&(cmd_ptr->cmd.esm_act_dedicated_bearer_context_rej));
        esm_modem_mem_free(mgr_ptr->esm_bc[con_id]);
		mgr_ptr->esm_bc[con_id] = NULL;
        ctxt_ptr = NULL;
	  }
	  else
	  {
        
	    MSG_ERROR_DS_1(SM_SUB,"ESM: No context for con id %d exists at NAS",con_id);
	  }
      break;  
    case NAS_ESM_PDN_CONNECT_PACKED_CNF:
      MSG_HIGH_DS_0(SM_SUB, "ESM: received NAS_ESM_PDN_CONNECT_PACKED_CNF from CM");
      break;
    case NAS_ESM_MODIFY_BEARER_CONTEXT_REJ_IND:
      MSG_HIGH_DS_1(SM_SUB, "ESM: Received MODIFY BEARER CTXT REJ, Bearer ID =%d",
                    cmd_ptr->cmd.esm_modify_bearer_context_rej.eps_bearer_id);
      ASSERT(cmd_ptr->cmd.esm_modify_bearer_context_rej.eps_bearer_id 
                 >= MIN_NSAPI_NUM);
	  if (ctxt_ptr != NULL)
	  {
      state_ptr->ctxt_process_modify_bearer_ctxt_rej(ctxt_ptr,
                                  &cmd_ptr->cmd.esm_modify_bearer_context_rej);
	  }
	  else
	  {
	    MSG_ERROR_DS_1(SM_SUB,"ESM: No context for con id %d exists at NAS",con_id);
	  }
      break;
    case NAS_ESM_MODIFY_BEARER_CONTEXT_ACCEPT_CNF:
      MSG_HIGH_DS_1(SM_SUB, "ESM: Received MODIFY BEARER CTXT ACCEPT, Bearer ID =%d",
                    cmd_ptr->cmd.esm_modify_bearer_context_accept.eps_bearer_id);
      ASSERT(cmd_ptr->cmd.esm_modify_bearer_context_accept.eps_bearer_id 
                 >= MIN_NSAPI_NUM);

	  if (ctxt_ptr != NULL)
	  {
        state_ptr->ctxt_process_modify_bearer_ctxt_accept(ctxt_ptr,
                               &cmd_ptr->cmd.esm_modify_bearer_context_accept,mgr_ptr);
        /* if ISR is ON and the BEARER context which is modified is not created after ISR on in LTE then
        ** set ISR status to OFF and send this status to EMM. Also reset ISR context activation status in 
        ** both ESM and SM.
        */
        if((mgr_ptr->isr_status == TRUE) && (ctxt_ptr->isr_bearer_ctxt_act_status == CTXT_ACT_BEFORE_ISR))
        {
          sm_deact_isr_on_interrat_status_set(SM_DEACT_ISR_ON_INTERRAT_TO_GW);
      
        }
	  }
	  else
	  {
        
	    MSG_ERROR_DS_1(SM_SUB,"ESM: No context for con id %d exists at NAS",con_id);
	  }
      break;

    default:
      MSG_ERROR_DS_1(SM_SUB, "ESM: Received UNKNOWN message from CM with id %d",
                     cmd_ptr->cmd.msg_hdr.src);
      break;
  }
} /* handle_cm_message() */



/*==============================================================================

  FUNCTION:  init_esm_bcm

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what init_esm_bcm() does goes here.

    @detail
    Initializes the bearer contexts.
*/
/*============================================================================*/
void init_esm_bcm
(
  esm_bcm_T                                      *mgr_ptr,
  esm_emm_context_status_update_func_fp          *update_func_ptr,
  esm_bcm_deactivate_associated_ded_ctxts_fp     *deact_func_ptr, 
  esm_bcm_deactivate_ctxts_fp                    *deact_ctxts_ptr,
  esm_bcm_allocate_ctxt_fp_type                 **bcm_allocate_ctxt_for_sm,
  esm_bcm_abort_proc_fp                          *abort_proc_ptr
)
{
  byte index = 0;

  /* Code goes here */
  ASSERT(mgr_ptr != NULL);
  ASSERT(update_func_ptr != NULL);
  ASSERT(deact_func_ptr != NULL);

  mgr_ptr->on_attach_def_bearer_pending = ESM_BC_ON_ATTACH_DEF_BEARER_NONE;
  mgr_ptr->first_def_bearer_ctxt_received = FALSE;

  /* I am surprised why we are not calling reste_ctxt function here?*/
  for (index = 0; index < ESM_MAX_EPS_BEARERS; index++ )
  {
    // Initializing the pointers to NULL here.
    // Function pointers for corresponding contexts will be initialized
    // while allocating memory for context in esm_bcm_allocate_context()
    // by calling esm_init_bcm_context_fn_ptr
    mgr_ptr->esm_bc[index] = NULL;
  }
  mgr_ptr->handle_cm_message_ptr = handle_cm_message;
  mgr_ptr->handle_emm_message_ptr = handle_emm_message;
  mgr_ptr->deact_ctxts_ptr = deact_ctxts_ptr;
  mgr_ptr->abort_proc_ptr = abort_proc_ptr;

  *bcm_allocate_ctxt_for_sm = esm_bcm_allocate_context;
  mgr_ptr->isr_status = FALSE;

} /* init_esm_bcm() */

/*==============================================================================

  FUNCTION:  esm_bcm_check_context_status

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_check_context_status() does goes here.

    @detail
    Check to see if we have atleast one context
*/
/*============================================================================*/
boolean esm_bcm_check_context_status(const esm_bcm_T* mgr_ptr)
{
  byte index = 0;
  boolean trigger_detach = TRUE;
  esm_bc_state_T cur_state;

  for(index=0; index < ESM_MAX_EPS_BEARERS; index++)
  {
    /* Only check for default bearer contexts */
    if(mgr_ptr->esm_bc[index] != NULL && mgr_ptr->esm_bc[index]->default_bearer_ctxt == TRUE)
    {
      cur_state = mgr_ptr->esm_bc[index]->eps_bearer_state_ptr->state();
      if( (cur_state == ESM_BC_ACTIVE) || (cur_state == ESM_BC_MODIFY_PENDING) )
      { 
        /* At least one default bearer context found, no need to trigger 
           detach */
        trigger_detach = FALSE;
        break;
      }
    }
  }

  return trigger_detach;
}

void bcm_get_bc_status(const esm_bcm_T* mgr_ptr)
{
  byte index = 0;
  for(index=0; index < ESM_MAX_EPS_BEARERS; index++)
  {
    if(mgr_ptr->esm_bc[index] != NULL && mgr_ptr->esm_bc[index]->connection_id != ESM_INVALID_CON_ID)
    { 
      MSG_SPRINTF_3(MSG_SSID_LTE, 
                    MSG_LEGACY_HIGH, 
                    "ESM: BC id = %d status is %d(inuse), STATUS=%s\n",
                    index,
                    1,
                    mgr_ptr->esm_bc[index]->eps_bearer_state_ptr->get_state_name());
    }
    else
    {
      MSG_HIGH_DS_2(SM_SUB, "ESM: BC id = %d status is %d(Free)\n", index, 1);
    }
  }
}
/*==============================================================================

  FUNCTION:  esm_bcm_deactivate_non_transferable_ctxts_to_GW
 
  This function is called by SM to deactivate any bearer contexts that
  are not transferbale to GW upon IRAT to GW

==============================================================================*/
void esm_bcm_deactivate_non_transferable_ctxts_to_GW(esm_bcm_T* mgr_ptr)
{
  byte index = 0;
  esm_cause_T cause;
  lte_nas_local_cause_T local_cause;
  boolean reset_flag = TRUE;

  ASSERT(mgr_ptr != NULL);
  cause.valid = FALSE;
  local_cause.valid = TRUE;

  for(index=0; index < ESM_MAX_EPS_BEARERS; index++)
  {
    esm_eps_bearer_context_T* ctxt_ptr =  mgr_ptr->esm_bc[index];
	if (ctxt_ptr != NULL)
	{
      esm_ctxt_state_ptr_type state_ptr  = ctxt_ptr->eps_bearer_state_ptr;

      if( state_ptr->state() != ESM_BC_INACTIVE )
      {
        /* First deactivate the associated dedicated bearers and then the default bearer*/
        if(ctxt_ptr->ti.valid == FALSE || ctxt_ptr->neg_qos.valid == FALSE)
        {
          if(ctxt_ptr->default_bearer_ctxt == TRUE)
          {
            /* IN this function we will NOT check for TI validity again as it is assumed that if default doesn't have a valid TI, then associated
                    dedicated bearer contexts will also not have a valid TI.
                    Similarly if default has a valid TI, then it is assumed that the associated dedicated bearers will also have a valid TI*/
            esm_bcm_deactivate_associated_ded_ctxts(mgr_ptr,local_cause,cause,ctxt_ptr->eps_bearer_id);
          }
          esm_build_and_send_deactivate_ind(ctxt_ptr,local_cause,cause);
          esm_bcm_reset_ctxt(ctxt_ptr);
          ctxt_ptr = NULL;
        }
        else
        {
          reset_flag = FALSE;
        }
      }
      else
      {
        esm_modem_mem_free(mgr_ptr->esm_bc[index]);
        mgr_ptr->esm_bc[index] = NULL;
      }
    }                
  }

  /* Reset on_attach_def_bearer_pending flag */
  if(reset_flag == TRUE)
  {
    mgr_ptr->on_attach_def_bearer_pending = ESM_BC_ON_ATTACH_DEF_BEARER_NONE;
    mgr_ptr->first_def_bearer_ctxt_received = FALSE;
  }
}

#endif /*FEATURE_LTE*/
  
