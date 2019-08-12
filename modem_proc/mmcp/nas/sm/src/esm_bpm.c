
/*!
  @file
  esm_bpm.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_bpm.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/17/12  cogolt   Added support for Rel-10 ESM backoff timer (T3396)
09/13/12   us      Rel 10 change: Added handling for collision of bearer resource allocation procedure & network initiated
                   bearer context deactivation for same linked bearer id. 
01/05/11   MNK     Handle race condition between bearer deactivation through active eps indication & any UE initiated procedures corresponding to that bearer.Terminate the procedure if the bearer got deactivated
06/03/10   zren    Updated BPM to handle EPS_BEARER_STATUS_IND gracefully
05/14/10   RI      Mainlined spec changes upto Dec 09.
02/17/10   zren    Added support to always deact bearer upon DS request in UE 
                   initiated modification procedure
02/03/10   zren    Added ESM new timer machanism
01/21/10   zren    Added EMM/ESM STATUS feature
12/15/09   zren    Updated PTI validation functionality
12/14/09   zren    Removed ds_eps_get_pdn_connectivity_ms stub code
12/10/09   zren    Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
11/18/09   zren    Added support for ESM Information procedure
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
10/14/09   MNK     Interface change for building and sending the detach cmd
10/01/09   zren    Set sdf id to invalid for the NW initiated procedures
10/01/09   zren    Reset APN name and PCo flaggs to FALSE
09/30/09   zren    Remove the hack setting the PCO and APN name to false
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/21/09   MNK     Changes due to IRAT design
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
09/14/09   zren    Added bearer ID and ESM cause check for PDN DISCONNECT 
                   REQ/REJ messages
09/14/09   hnam    Activated NAS events
09/01/09   MNK     Added handling of detach_ind from EMM, as we support bearer initiated procedures
08/18/09   MNK     CM-NAS interface changes
08/14/09   MNK     Add Support for PDN disconnect request
08/14/09   hnam    Added support NAS events
07/30/09   MNK     Changes for multiple PDN connectivity support
06/26/09   hnam    Added support for logging ESM NON-OTA message packets
06/05/09   zren    Updated code to use MSG_SPRINTF_3 for "%s" format specifier
06/02/09   MNK     Copy these keys so that BCM can use in case it needs to to respond back to CM for UE initiated procedure
05/21/09   MNK     Added support to reply back to CM/DS in case the NW sent bad messages as a response to UE initiated request
05/08/09   MNK     Added support for bearer resource allocation procedures
05/01/09   MNK     Added support for N/W initiated bearer context deactivation
04/16/09   MNK     More code added to support different scenarios for handling multiple bearers upon attach
04/06/09   MNK     Added code to generate based on new transaction id format
04/02/09   MNK     Added support for multiple bearer setup support at the attach 
03/06/09   MNK     Change esm_failure handling based on transaction id
02/24/09   MNK     Added code to handle invalid/mismtahc PTI
02/05/09   MNK     Changed ds_eps_get_pdn_connectivity_msg to function pointer ds_eps_get_pdn_connectivity_msg_fp
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "amssassert.h"
#include "esm_cm_msgr_enum_msg.h" 
#include "esm_emm_msgr_enum_msg.h" 
#include "esm_v.h"
#include "esm_bpm.h"
#include "esm_proc_state.h"
#include "cm_esm.h"
#include "esm_utils.h"
#include "timers_v.h"
#include "naslog_v.h"
#include "lte_nas_log.h"
#include "event.h"
#include "event_defs.h"
#include "lte_nas_msg_parse.h"
#include "stringl.h"
#ifdef FEATURE_DUAL_SIM
#include "sm_multimode_common.h"
#endif
extern void sm_unblock_all_apn(sys_modem_as_id_e_type as_id);
#ifdef TEST_FRAMEWORK
  #error code not present
#endif // TEST_FRAMEWORK

extern void esm_build_pdn_connectivity_msg
(
  const esm_pdn_connectivity_req_T* data_ptr,  /*!< REQUIRED short parameter description */
  lte_nas_esm_outgoing_msg_type*  ptr2esm_msg
);


extern void bpm_get_bp_status(const esm_bpm_T* mgr_ptr);

extern byte esm_get_module_from_trans_id
(
  dword transaction_id   /*!< REQUIRED short parameter description */
);

/*==============================================================================

                                FUNCTIONS

==============================================================================*/

/*==============================================================================

  FUNCTION:  bpm_generate_pti

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what bpm_generate_pti() does goes here.

    @detail
    Generates a PTI for UE initiated procedure 
*/
/*============================================================================*/
static byte  bpm_generate_pti
(
 esm_bpm_T* mgr_ptr    /*!< REQUIRED short parameter description */
)
{  
    //byte index = BPM_PTI_START; /* Zero is used by the N/W*/

  byte index = mgr_ptr->cur_pti_index;

  /* If this is the first time PTI is being allocated, this increment will make sure that 0 is not used. 0 is used by the NW. UE MUST start from 1*/
  /* After the 1st iteration cur_pti_index points to the last PTI that was allocated. Further iterations will use the next PTI*/
  if(index == 254)
  {
    /* Immediate increment below will make sure we start with BPM_PTI_START again*/
    index = 0;
  }

  index++;

  for(; index != mgr_ptr->cur_pti_index; index++)
  {
    if(index == 0)
    {
      /* SKIP 0 as it is used by the NW*/
      index = BPM_PTI_START;
    }
    if (mgr_ptr->pti_list[index] == FALSE)
    {
      mgr_ptr->pti_list[index] = TRUE;
      mgr_ptr->cur_pti_index = index;
      MSG_HIGH_DS_1(SM_SUB, "ESM: PTI %d is allocated", index);
      return mgr_ptr->cur_pti_index;
    }
  }

  MSG_ERROR_DS_0(SM_SUB, "ESM: ESM ran out of transaction ids. This should not happen, something is SERIOUSLY WRONG");

  return 0;
} /* bpm_generate_pti() */     

/*==============================================================================

  FUNCTION:  bpm_release_pti

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what bpm_release_pti() does goes here.

    @detail
    Releases PTI.

*/
/*============================================================================*/
static void bpm_release_pti
(
  esm_bpm_T* mgr_ptr, byte val 
)
{
  ASSERT(mgr_ptr != NULL);

  if (val > 0 && val < 255)
  {
    mgr_ptr->pti_list[val] = FALSE;
    MSG_HIGH_DS_1(SM_SUB, "ESM: PTI %d is released", val);
  }
  else
  {
    MSG_ERROR_DS_0(SM_SUB, "ESM: Received invalid PTI value");
  }
} /* bpm_release_pti() */



/*==============================================================================

  FUNCTION:  esm_get_free_proc_instance_and_pti


==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_get_free_proc_instance_and_pti() does goes here.

    @detail
    This function finds a free procedure instance and free protocol transaction id
*/
/*============================================================================*/
static esm_bearer_resource_proc_T* esm_get_free_proc_instance_and_pti
(
  esm_bpm_T* mgr_ptr
)
{
  byte index = 0;

  /* CRITICAL SECTION BEGIN*/

  ASSERT(mgr_ptr != NULL);

  /* Code goes here */
  for(index = 0; index < ESM_MAX_BP_PROCS ; index++)
  {
    //Try to find a match;
    if (mgr_ptr->esm_bp[index].inuse == FALSE)
    {
      //Mark the procedure to be inuse;
      mgr_ptr->esm_bp[index].inuse = TRUE;
      /*Get a PTI for this procedure*/
      mgr_ptr->esm_bp[index].pti = bpm_generate_pti(mgr_ptr);
      ASSERT(mgr_ptr->esm_bp[index].pti != 0);
      if(mgr_ptr->esm_bp[index].pending_msg_ptr != NULL)
      {
        MSG_ERROR_DS_1(SM_SUB, "ESM: Procedure instance %d - msg pionter not NULL", index);
        mgr_ptr->esm_bp[index].pending_msg_ptr = NULL;
      }
      mgr_ptr->esm_bp[index].pending_msg_ptr = 
                                   esm_modem_mem_alloc(sizeof(emm_data_req_T));
      /* Reset here also just to make sure the keys are set correctly*/
      mgr_ptr->esm_bp[index].lbi = ESM_INVALID_BEARER_ID;
      return &(mgr_ptr->esm_bp[index]);
    }
  }
  return NULL;
  /* CRITICAL SECTION END*/

} /* esm_get_free_proc_instance_and_pti() */

/*==============================================================================

  FUNCTION:  esm_bpm_reset_bp

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bpm_reset_bp() does goes here.

    @detail
    Resets bearer procedure instance
*/
/*============================================================================*/
static void esm_bpm_reset_bp
(
  esm_bearer_resource_proc_T* proc_ptr  /*!< REQUIRED short parameter description */
)
{
  esm_timer_type  timer_id = ESM_TIMER_INVALID;

  ASSERT(proc_ptr != NULL);
  ASSERT(proc_ptr->pending_msg_ptr != NULL);

  /* Timer should has already been stopped. However do a double check here and 
     stop the missed timer */
  if(proc_ptr->timer_ptr != NULL)
  {
    switch(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id)
    {
      case PDN_CONNECTIVITY_REQUEST:
        timer_id = ESM_TIMER_T3482;
        break;
      case PDN_DISCONNECT_REQUEST:
        timer_id = ESM_TIMER_T3492;
        break;
      case BEARER_RESOURCE_ALLOCATION_REQUEST:
        timer_id = ESM_TIMER_T3480;
        break;
      case BEARER_RESOURCE_MODIFICATION_REQUEST:
        timer_id = ESM_TIMER_T3481;
        break;
      default:
        MSG_ERROR_DS_1(SM_SUB, "ESM: Invalid pending msg %u",
                       proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id);
        break;
    }
    esm_stop_timer(timer_id, proc_ptr);
  }

  proc_ptr->esm_proc_state_ptr = &esm_proc_state_inactive;
  proc_ptr->inuse = FALSE;
  proc_ptr->pti = ESM_INVALID_PTI;
  proc_ptr->sdf_id = ESM_INVALID_SDF_ID;
  proc_ptr->retransmit_counter = 0;
  proc_ptr->lbi = ESM_INVALID_BEARER_ID;
  proc_ptr->pco_ciphering_flag = FALSE;
  proc_ptr->esm_local_cause.valid = FALSE;

  /* Release the pending message resources */
  esm_modem_mem_free(proc_ptr->pending_msg_ptr);
  proc_ptr->pending_msg_ptr = NULL;

} /* esm_bpm_reset_bp() */


/*==============================================================================

  FUNCTION:  esm_bpm_release_proc_resources

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bpm_release_proc_resources() goes here.

    @detail
    Releases PTI and resets the procedure instance.

*/
/*============================================================================*/
static void esm_bpm_release_proc_resources
(
  esm_bpm_T* mgr_ptr,
  esm_bearer_resource_proc_T* proc_ptr
)
{
  lte_nas_esm_procedure_state_T  esm_proc_state_info_log;

  ASSERT(mgr_ptr != NULL);
  ASSERT(proc_ptr != NULL);

  /* Init */
  memset((void *)&esm_proc_state_info_log, 
         0x0, 
         sizeof(lte_nas_esm_procedure_state_T));

  esm_proc_state_info_log.instance_id = proc_ptr->instance_id;

  bpm_release_pti(mgr_ptr,proc_ptr->pti);
  
  esm_proc_state_info_log.proc_state = (byte)proc_ptr->esm_proc_state_ptr->state();
  esm_proc_state_info_log.pti = (byte)proc_ptr->pti;
  esm_proc_state_info_log.sdf_id = (byte)proc_ptr->sdf_id;
  if(proc_ptr->pending_msg_ptr != NULL)
  {
    esm_proc_state_info_log.pending_msg_id = (byte)proc_ptr->pending_msg_ptr->msg_hdr.id;
  }

  send_esm_proc_state_log_info(&esm_proc_state_info_log);

  esm_bpm_reset_bp(proc_ptr);

}

/*==============================================================================

  FUNCTION:  esm_bpm_validate_pti

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bpm_validate_pti() does goes here.

    @detail
    Validates the PTI
*/
/*============================================================================*/
boolean esm_bpm_validate_pti
(
  byte                 pti,
  lte_nas_message_id   msg_id
)
{
  boolean   pti_is_valid = TRUE;

  switch(pti)
  {
    case ESM_INVALID_PTI:
      switch(msg_id)
      { 
        case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
        case ESM_STATUS:
        case PDN_CONNECTIVITY_REJECT:
        case PDN_DISCONNECT_REJECT:
        case BEARER_RESOURCE_ALLOCATION_REJECT:
        case BEARER_RESOURCE_MODIFICATION_REJECT:
        case ESM_INFORMATION_REQUEST:
          MSG_HIGH_DS_1(SM_SUB, "ESM: Unassigned PTI w/ msg %u is invalid", msg_id);
          pti_is_valid = FALSE;
          break;
        default:
          MSG_HIGH_DS_1(SM_SUB, "ESM: Unassigned PTI w/ msg %u is valid", msg_id);
          break;
      }
      break;
    case ESM_RESERVED_PTI:
      MSG_HIGH_DS_2(SM_SUB, "ESM: Reserved PTI %d w/ msg %u is invalid", pti, msg_id);
      pti_is_valid = FALSE;
      break;
    default:
      MSG_HIGH_DS_2(SM_SUB, "ESM: Rcved assigned PTI %d w/ msg %u", pti, msg_id);
      break;
  }

  return pti_is_valid;
}
/*==============================================================================

  FUNCTION:  esm_bpm_find_proc_state_machine

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bpm_find_proc_state_machine() does goes here.

    @detail
    Validates the PTI
*/
/*============================================================================*/
esm_bearer_resource_proc_T* esm_bpm_find_proc_state_machine(esm_bpm_T* mgr_ptr, word sdf_id, byte lbi)
{
  byte index = 0;
  for(index = 0; index < ESM_MAX_BP_PROCS; index++)
  {
    if( (mgr_ptr->esm_bp[index].inuse == TRUE) && (mgr_ptr->esm_bp[index].sdf_id == sdf_id) && (mgr_ptr->esm_bp[index].lbi == lbi) )
    {
      return &mgr_ptr->esm_bp[index];
    }
  }
  return NULL;
}

/*==============================================================================

  FUNCTION:  esm_bpm_find_uicc_proc_state_machine

==============================================================================*/
/*!
    @brief
    During UICC validation, NAS_ESM_PDN_CONNECT_PACKED_CNF is received from 
    CM - this does not have LBI, so use the search criteria as SDF_ID

    @detail
    Validates the PTI
*/
/*============================================================================*/
esm_bearer_resource_proc_T*  esm_bpm_find_uicc_proc_state_machine(esm_bpm_T* mgr_ptr, byte pti)
{
  byte index = 0;
  for(index = 0; index < ESM_MAX_BP_PROCS; index++)
  {
    if((mgr_ptr->esm_bp[index].inuse == TRUE) && (mgr_ptr->esm_bp[index].pti == pti))
    {
      MSG_HIGH_DS_1(SM_SUB,"ESM: found the proc pointer, matching the PTI %d",pti);
      return &mgr_ptr->esm_bp[index];
    }
  }
  return NULL;
}
/*==============================================================================

  FUNCTION:  esm_bpm_validate_bearer_resource_alloc_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bpm_validate_bearer_resource_alloc_req() does goes here.

    @detail
    Validate the bearer resource allocation request
*/
/*============================================================================*/

static boolean esm_bpm_validate_bearer_resource_alloc_req
(
  struct esm_bpm* mgr_ptr,
  esm_bearer_resource_alloc_req_T* req_ptr
)
{
  byte index = 0;
#ifdef FEATURE_LTE_REL9
  esm_bearer_context_info_shared_T  bc_info_shared;
#endif

  ASSERT(req_ptr->esm_hdr.lbi >= MIN_NSAPI_NUM);

#ifdef FEATURE_LTE_REL9
  mgr_ptr->find_bearer_context_ptr(req_ptr->esm_hdr.lbi, &bc_info_shared);
  
  if((bc_info_shared.bearer_id_exist == FALSE) ||
     (bc_info_shared.emc_bc == TRUE))
  {
    MSG_HIGH_DS_2(SM_SUB, "ESM: Invalid BEARER RESRC ALLOC REQ - BID %d EMC SRV Status %d",
                  req_ptr->esm_hdr.lbi,
                  bc_info_shared.emc_bc);
    return FALSE;
  }
#endif

  for(index = 0; index < ESM_MAX_BP_PROCS; index++) 
  {
    if( (mgr_ptr->esm_bp[index].sdf_id == req_ptr->esm_hdr.sdf_id) && (mgr_ptr->esm_bp[index].lbi == req_ptr->esm_hdr.lbi) )
    {
      return FALSE;
    }
  }

  return TRUE;
}

/*===========================================================================

FUNCTION  ESM_BPM_CLEAN_UP

DESCRIPTION
  This function aborts all pending ESM procedures and releases all resources

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void esm_bpm_clean_up
(
  struct esm_bpm* mgr_ptr,
  lte_nas_local_cause_T local_cause
)
{
  byte     index = 0;
#ifdef FEATURE_LTE_REL9
  boolean  keep_emc_pdn_proc = FALSE;
#endif

  for(index = 0; index < ESM_MAX_BP_PROCS; index++)
  {
    if((mgr_ptr->esm_bp[index].inuse == TRUE) && 
       (mgr_ptr->esm_bp[index].esm_proc_state_ptr->state() != ESM_BP_INACTIVE))
    {
#ifdef FEATURE_LTE_REL9
      if((mgr_ptr->esm_bp[index].pending_msg_ptr->esm_out_msg.nas_hdr.msg_id 
                                                   == PDN_CONNECTIVITY_REQUEST)
         &&
         (mgr_ptr->esm_bp[index].pending_msg_ptr->esm_out_msg.\
                                    pdn_connectivity_req.req == ESM_EMERGENCY)
         &&
         ((local_cause.valid == TRUE) && 
          (local_cause.local_cause == EMM_ATTACH_STARTED)))
      {
        esm_stop_timer(ESM_TIMER_T3482, &mgr_ptr->esm_bp[index]);
        keep_emc_pdn_proc = TRUE;
      }
#endif

#ifdef FEATURE_LTE_REL9
      if(keep_emc_pdn_proc == FALSE)
#endif
      {
        mgr_ptr->esm_bp[index].esm_proc_state_ptr->esm_proc_clean_up(
                                         &mgr_ptr->esm_bp[index], local_cause);

        bpm_release_pti(mgr_ptr, mgr_ptr->esm_bp[index].pti);

        esm_bpm_reset_bp(&mgr_ptr->esm_bp[index]);
      }
    }
  }

#ifdef FEATURE_LTE_REL9
  if(keep_emc_pdn_proc == FALSE)
  {
    if(esm_get_emc_srv_status() == LTE_NAS_EMC_SRV_PENDING)
    {
      esm_set_emc_srv_status(LTE_NAS_EMC_SRV_NONE);
    }
#endif

    /* Always reset PTI pool */
    memset((void *)mgr_ptr->pti_list, 0x0, sizeof(mgr_ptr->pti_list));
#ifdef FEATURE_LTE_REL9
  }
#endif

} /* esm_bpm_clean_up() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*==============================================================================

  FUNCTION:  esm_bpm_ignore_on_proc_collision

==============================================================================*/
/*!
    @brief
    This function checks if ESM needs to ignore coming in ESM OTA message due 
    to ESM procedure collisions

    @detail
*/
/*============================================================================*/
boolean esm_bpm_ignore_on_proc_collision
(
  lte_nas_esm_incoming_msg_type    *esm_incoming_msg_ptr, 
  esm_bpm_T                        *mgr_ptr
)
{
  int                                 index = 0;
  esm_bearer_context_info_shared_T    bc_info_shared;
  byte                                bearer_id = 0;

  ASSERT(esm_incoming_msg_ptr != NULL);
  ASSERT(mgr_ptr != NULL);

  switch(esm_incoming_msg_ptr->nas_hdr.msg_id)
  {
    /* 6.5.2.5 b - Collision of UE requested PDN disconnect procedure and 
       dedicated EPS bearer context activation procedure */
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST:
      for(index=0; index < ESM_MAX_BP_PROCS; index++)
      {
        if((mgr_ptr->esm_bp[index].inuse == TRUE) &&
           (mgr_ptr->esm_bp[index].pending_msg_ptr != NULL) &&
           (mgr_ptr->esm_bp[index].pending_msg_ptr->esm_out_msg.nas_hdr.msg_id
              == PDN_DISCONNECT_REQUEST) &&
           (mgr_ptr->esm_bp[index].lbi == 
                           esm_incoming_msg_ptr->dedicated_bearer_context_req.\
                             linked_eps_bearer_identity))
        {
          MSG_HIGH_DS_0(SM_SUB, "ESM: Proc collision - ignore ACT DEDICATED BC REQ");
          /* Proc collision found, need to ignore the message */

#ifdef TEST_FRAMEWORK
          #error code not present
#endif /* TEST_FRAMEWORK */
          return TRUE;
        }               
      }
      break;
    /* 6.5.2.5 c - Collision of UE requested PDN disconnect procedure and EPS
       bearer context modification */
    case MODIFY_EPS_BEARER_CONTEXT_REQUEST:
      bearer_id = esm_incoming_msg_ptr->modify_eps_bearer_ctxt_req.\
                                                     lte_nas_esm_hdr.bearer_id;
      /* Find the bearer context w/ the given bearer ID */
      mgr_ptr->find_bearer_context_ptr(bearer_id, &bc_info_shared);

      if(bc_info_shared.bearer_id_exist == TRUE)
      {
        /* Need to find our if the bearer context belongs to the PDN connection
           to be disconnected.
           - If the bearer context is not default, use its lbi.
           - If the bearer context is default, use the bearer ID itself */
        if(bc_info_shared.default_bearer_ctxt == FALSE)
        {
          bearer_id = bc_info_shared.lbi;
        }
        for(index=0; index < ESM_MAX_BP_PROCS; index++)
        {
          if((mgr_ptr->esm_bp[index].inuse == TRUE) &&
             (mgr_ptr->esm_bp[index].pending_msg_ptr != NULL) &&
             (mgr_ptr->esm_bp[index].pending_msg_ptr->esm_out_msg.nas_hdr.msg_id 
                == PDN_DISCONNECT_REQUEST) &&
             ((bc_info_shared.bearer_id_exist == TRUE) && 
              (mgr_ptr->esm_bp[index].lbi == bearer_id)))
          {
            MSG_HIGH_DS_0(SM_SUB, "ESM: Proc collision - ignore MODIFY EPS BC REQ");
            /* Proc collision found, need to ignore the message */
#ifdef TEST_FRAMEWORK
            #error code not present
#endif /* TEST_FRAMEWORK */
            return TRUE;
          }               
        }
      }
      break;
    default:
      /* Do nothing for other messages */
      break;
  }
  /* No proc collision, no need to ignore the message */
  return FALSE;

} /* esm_bpm_ignore_on_proc_collision */

/*==============================================================================

  FUNCTION:  esm_bpm_abort_on_proc_collision

==============================================================================*/
/*!
    @brief
    This function checks if ESM needs to abort an ongoing BPM procedure. 

    @detail
*/
/*============================================================================*/
void  esm_bpm_abort_on_proc_collision
(
  lte_nas_esm_incoming_msg_type    *esm_incoming_msg_ptr, 
  esm_bpm_T                        *mgr_ptr
)
{
  int index = 0;
  ASSERT(esm_incoming_msg_ptr != NULL);
  ASSERT(mgr_ptr != NULL);

  if(esm_incoming_msg_ptr->nas_hdr.msg_id ==
                                         DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST)
  {
    /* 6.5.4.5 c - Collision of UE requested bearer resource modification 
       procedure and EPS bearer context deactivation procedure */
    for(index=0; index < ESM_MAX_BP_PROCS; index++)
    {
      if((mgr_ptr->esm_bp[index].inuse == TRUE) &&
           (mgr_ptr->esm_bp[index].pending_msg_ptr != NULL) &&
           ((mgr_ptr->esm_bp[index].pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                         BEARER_RESOURCE_MODIFICATION_REQUEST)||
            (mgr_ptr->esm_bp[index].pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == 
                                          BEARER_RESOURCE_ALLOCATION_REQUEST)||
            (mgr_ptr->esm_bp[index].pending_msg_ptr->esm_out_msg.nas_hdr.msg_id ==
                                                         PDN_DISCONNECT_REQUEST))&&
           (mgr_ptr->esm_bp[index].lbi == 
              esm_incoming_msg_ptr->deactivate_eps_bearer_context_req.lte_nas_esm_hdr.bearer_id))
        {
          if(mgr_ptr->esm_bp[index].pending_msg_ptr->esm_out_msg.nas_hdr.msg_id ==
                                                         BEARER_RESOURCE_MODIFICATION_REQUEST)
          {
             /* Abort Bearer Resource Modification procedure */
             MSG_HIGH_DS_0(SM_SUB, "ESM: Proc collision - Abort Bearer Resrc MOD PROC");
          }

          else if(mgr_ptr->esm_bp[index].pending_msg_ptr->esm_out_msg.nas_hdr.msg_id ==
                                                         BEARER_RESOURCE_ALLOCATION_REQUEST)
          {
             /* Abort Bearer Resource Allocation procedure */
             MSG_HIGH_DS_0(SM_SUB, "ESM: Proc collision - Abort Bearer Resrc ALLOC PROC");
          }
           else
          {
             /* Abort PDN Disconnect procedure */
             MSG_HIGH_DS_0(SM_SUB, "ESM: Proc collision - Abort PDN DISCONNECT PROC");
          }

         if(mgr_ptr->esm_bp[index].pending_msg_ptr->esm_out_msg.nas_hdr.msg_id !=
                                                         PDN_DISCONNECT_REQUEST )
         {
            esm_proc_send_cm_autonomous_reject(&mgr_ptr->esm_bp[index], 
                                               ESM_LOWER_LAYER_FAILURE,
                                               NULL);
         }
          esm_bpm_release_proc_resources(mgr_ptr, &mgr_ptr->esm_bp[index]);
        }
      }
    }
  }/* esm_bpm_abort_on_proc_collision */


/*==============================================================================

  FUNCTION:  esm_bpm_rel_proc_ota_msg

==============================================================================*/
/*!
    @brief
    This function release procedure resources if necessary

    @detail
    Releases PTI and resets the procedure instances if necessary.

*/
/*============================================================================*/
static void esm_bpm_rel_proc_ota_msg
(
  lte_nas_esm_incoming_msg_type   *ota_msg,
  esm_bpm_T                       *mgr_ptr,
  esm_bearer_resource_proc_T      *proc_ptr
)
{
  byte     index = 0;
  boolean  abort_on_pti = FALSE;
  boolean  abort_on_bid = FALSE;

  ASSERT(ota_msg != NULL);
  ASSERT(mgr_ptr != NULL);
  ASSERT(proc_ptr != NULL);

  switch(ota_msg->nas_hdr.msg_id)
  {
    case ESM_STATUS:
      switch(ota_msg->mt_esm_status.esm_cause)
      {
        case NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT:
          if(ota_msg->mt_esm_status.lte_nas_esm_hdr.bearer_id >= MIN_NSAPI_NUM)
          {
            abort_on_bid = TRUE;
            break;
          }
          else
          {
            return;
          }
        case NAS_ESM_INVALID_PTI_VALUE:
          abort_on_pti = TRUE;
          break;
        case NAS_ESM_MSG_TYPE_NON_EXISTANT:
          if(ota_msg->mt_esm_status.lte_nas_esm_hdr.bearer_id >= MIN_NSAPI_NUM)
          {
            abort_on_bid = TRUE;
          }
          abort_on_pti = TRUE;
          break;
        default:
          /* No need to abort any procedure */
          return;
      }

      for(index=0; index < ESM_MAX_BP_PROCS; index++)
      {
        if(mgr_ptr->esm_bp[index].inuse == TRUE) 
        {
          if(((abort_on_pti == TRUE) && (mgr_ptr->esm_bp[index].pti == 
                                   ota_msg->mt_esm_status.lte_nas_esm_hdr.pti))
             ||
             ((abort_on_bid == TRUE) && (mgr_ptr->esm_bp[index].lbi == 
                            ota_msg->mt_esm_status.lte_nas_esm_hdr.bearer_id)))
          {
            MSG_HIGH_DS_2(SM_SUB, "ESM: Abort proc w/ PTI %d, Bearer ID %d",
                          mgr_ptr->esm_bp[index].pti,
                          mgr_ptr->esm_bp[index].lbi);
            esm_proc_send_cm_autonomous_reject(&mgr_ptr->esm_bp[index],
                                               ESM_LOWER_LAYER_FAILURE,
                                               NULL);
            esm_bpm_release_proc_resources(mgr_ptr, &mgr_ptr->esm_bp[index]);
          }               
        }
      }
      break;
    case ESM_INFORMATION_REQUEST:
      /* No need to release the procedure resource since ESM INFO REQUEST 
         procedure is embedded in PDN CONNECTIVITY procedure during attach 
         which is still active */
      break;
    default:
      /* For other messages, related procedure is complete. Need to release 
         procedure resource */
      esm_bpm_release_proc_resources(mgr_ptr, proc_ptr);
      break;
  }

} /* esm_bpm_rel_proc_ota_msg */

/*==============================================================================

  FUNCTION:  handle_message

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what handle_message() does goes here.

    @detail
    Handles all the messages received from the CM and EMM
*/
/*============================================================================*/
static boolean handle_message
(
  struct esm_cmd* cmd_ptr, 
  struct esm_bpm* mgr_ptr
)
{
  byte index = 0, bearer_id = 0;
  esm_bearer_resource_proc_T* proc_ptr = NULL ;
  lte_nas_esm_hdr_type* esm_hdr = NULL;
  boolean result = TRUE ;
  lte_nas_esm_procedure_state_T esm_proc_state_info_log;
  log_lte_nas_esm_incoming_msg_event_type event;
  lte_nas_local_cause_T local_cause;
  esm_cause_T ota_cause;
  byte msg_pkt[PDN_CONN_REQ_SIZE] = {0};
  byte msg_len = 0;
  local_cause.valid = FALSE;
  ota_cause.valid   = FALSE;

  ASSERT(cmd_ptr != NULL);
  ASSERT(mgr_ptr != NULL);

  /* All this code should go to the state processing? Here we should only know whether 
   to invoke process_cm or process_nw or process_internal? Probably not and it is difficult 
   to do it that way */
  switch (cmd_ptr->cmd.msg_hdr.id)
  {
    case NAS_ESM_PDN_CONNECT_PACKED_CNF:
      MSG_HIGH_DS_0(SM_SUB, "ESM: MSG_ID == NAS_ESM_PDN_CONNECT_PACKED_CNF");
      proc_ptr = esm_bpm_find_uicc_proc_state_machine(mgr_ptr,
                                                      cmd_ptr->cmd.esm_pdn_connectivity_pkd_cnf.pti);

      if(proc_ptr == NULL)
      {
        MSG_HIGH_DS_0(SM_SUB, "ESM: No proc found - Do not process");
        return result;
      }

      /*Check this CM message to see if PACKED_PDN_CONNECTITY_REQ 
        status is ACCEPT/MODIFY/REJECT status and act based upon this status*/     
      MSG_HIGH_DS_1(SM_SUB, "ESM: PACKED_CNF status = %d",
                    cmd_ptr->cmd.esm_pdn_connectivity_pkd_cnf.status);
      if(proc_ptr->esm_proc_state_ptr->state() == ESM_BP_ABORT)          
      {            
        MSG_HIGH_DS_0(SM_SUB, "ESM: proc is in ABORT state, will send PDN_CONN_ABORT_CNF to DS");
        proc_ptr->esm_proc_state_ptr->esm_proc_process_pdn_connect_req(proc_ptr,&(cmd_ptr->cmd.esm_pdn_connectivity_req));     
        MSG_HIGH_DS_0(SM_SUB, "ESM: releasing the resources");
        esm_bpm_release_proc_resources(mgr_ptr, proc_ptr);           
        return result;
      }
      switch(cmd_ptr->cmd.esm_pdn_connectivity_pkd_cnf.status)
      {
        case ESM_MSG_ACCEPTED:
        case ESM_MSG_MODIFIED:
          /*Save the msg_len and msg_pkt that came in this message, because when we do 
            memset for pdn_conn_Req, all these data will also be vanished (as cmd_ptr.cmd is a union)*/
          msg_len = (byte)cmd_ptr->cmd.esm_pdn_connectivity_pkd_cnf.pkd_msg_len;
          MSG_HIGH_DS_1(SM_SUB, "ESM: PACKED_CNF status = ESM_MSG_MODIFIED/ESM_MSG_ACCEPTED msg_len = %d, shall proceed to decode this PDN_CONN_REQ ", msg_len);
          memscpy((void*)&msg_pkt[0],sizeof(msg_pkt),
                  (void*)&cmd_ptr->cmd.esm_pdn_connectivity_pkd_cnf.pkd_msg,
                  msg_len);
          /*Do reset and then give this structure to decode function, otherwise there are chances of junk values being present*/
          memset(&(cmd_ptr->cmd.esm_pdn_connectivity_req),0,sizeof(esm_pdn_connectivity_req_T));
          /*Decode the encoded message coming from CM*/
          decode_pdn_conn_req(&msg_pkt[0],/**/
                               msg_len,
                              &(cmd_ptr->cmd.esm_pdn_connectivity_req) /*Message structure*/);       
          
#ifdef FEATURE_LTE_REL10
          /* Send pdn connectivity reject ind to CM when APN is blocked */
          if(cmd_ptr->cmd.esm_pdn_connectivity_req.pdn_data.apn_name.valid == TRUE)
          {
            if(sm_is_apn_blocked(cmd_ptr->cmd.esm_pdn_connectivity_req.pdn_data.apn_name) == TRUE)
            {
              MSG_HIGH_DS_0(SM_SUB, "ESM: Reject PDN connectivity req since APN is Blocked");
              esm_build_and_send_pdn_connectivity_reject_ind(proc_ptr,
                                                             proc_ptr->sdf_id,
                                                             NULL, 
                                                             ESM_LOCAL_CUASE_NONE,
                                                             NULL);
              esm_bpm_release_proc_resources(mgr_ptr, proc_ptr);
              break;
            }
          }
#endif
          if(proc_ptr->chk_on_attach_def_bc_setup_ptr() == FALSE)
          {
            cmd_ptr->cmd.esm_pdn_connectivity_req.pdn_data.pco_ciphering_flag
              = proc_ptr->pco_ciphering_flag;
          }
          cmd_ptr->cmd.esm_pdn_connectivity_req.esm_hdr.sdf_id = proc_ptr->sdf_id;
          cmd_ptr->cmd.esm_pdn_connectivity_req.pdn_data.seq_num = 
           proc_ptr->pending_msg_ptr->esm_out_msg.pdn_connectivity_req.seq_num;
          /* Send PDN_CONNECTIVITY_REQ message to EMM, 
             to be encoded and sent Over The Air*/ 
          proc_ptr->esm_proc_state_ptr->esm_proc_process_pdn_connect_req(proc_ptr,&(cmd_ptr->cmd.esm_pdn_connectivity_req));    
          break;
        case ESM_MSG_REJECTED_ACL_FAILURE:
          MSG_HIGH_DS_0(SM_SUB, "ESM: PACKED_CNF status = ESM_MSG_REJECTED_ACL_FAILURE");
          /*Send PDN_CONNECTIVITY_REJECT for ACL_FAILURE */
          esm_build_and_send_pdn_connectivity_reject_ind(proc_ptr,
                                                         proc_ptr->sdf_id,
                                                         NULL, 
                                                         ACL_FAILURE, 
                                                         NULL);
          esm_bpm_release_proc_resources(mgr_ptr, proc_ptr);        
          break;
        case ESM_MSG_REJECTED:
          /*Send PDN_CONNECTIVITY_REJECT*/
          MSG_HIGH_DS_0(SM_SUB, "ESM: PACKED_CNF status = ESM_MSG_REJECTED");
          esm_build_and_send_pdn_connectivity_reject_ind(proc_ptr,
                                                         proc_ptr->sdf_id,
                                                         NULL, 
                                                         ESM_LOCAL_CUASE_NONE,
                                                         NULL);
          esm_bpm_release_proc_resources(mgr_ptr, proc_ptr);           
          break;
        default:
          MSG_FATAL_DS(SM_SUB, "ESM: Invalid PDN CONN PACKED CNF %d", 
                    cmd_ptr->cmd.esm_pdn_connectivity_pkd_cnf.status,0,0);
          break;
      }
      break;
    case NAS_ESM_DATA_IND:
      event.esm_in_msg = (byte)NAS_ESM_DATA_IND;
      event_report_payload(EVENT_LTE_EMM_INCOMING_MSG,
                           sizeof(event), (void*)&event);

      esm_hdr = esm_get_esm_hdr_from_esm_incoming_msg(
                                  &cmd_ptr->cmd.esm_data_ind.esm_incoming_msg);
      ASSERT(esm_hdr != NULL);

      /* 24.301 7.3.1 - Validate PTI */
      result = esm_bpm_validate_pti(esm_hdr->pti,
                    cmd_ptr->cmd.esm_data_ind.esm_incoming_msg.nas_hdr.msg_id);
      if(result != TRUE)
      {
        MSG_ERROR_DS_1(SM_SUB, "ESM: Invalid PTI %d", esm_hdr->pti);
        esm_bpm_build_and_send_autonomous_reject(
                                   &cmd_ptr->cmd.esm_data_ind.esm_incoming_msg,
                                    NAS_ESM_INVALID_PTI_VALUE);
        /* Return false so that BCM ignores it */
        return FALSE;
      }

      /* This is network initiated procedure */
      if (esm_hdr->pti == ESM_INVALID_PTI)
      {
        /* Check if need to ignore this msg due to procedure collision */
        if(esm_bpm_ignore_on_proc_collision(
                 &cmd_ptr->cmd.esm_data_ind.esm_incoming_msg, mgr_ptr) == TRUE)
        {
          /* Return false so that BCM ignores it */
          return FALSE;
        }
        else
        {
          /* Check if need to abort ongoing procedure */
          esm_bpm_abort_on_proc_collision(
                         &cmd_ptr->cmd.esm_data_ind.esm_incoming_msg, mgr_ptr);

          /* Key for DS to know that this is a NW initiated request*/
          cmd_ptr->cmd.esm_data_ind.esm_hdr.sdf_id = ESM_INVALID_SDF_ID;

          /* Return TRUE so that BCM handles it */
          return TRUE;
        }
      }

      for(index = 0; index < ESM_MAX_BP_PROCS ; index++)
      {
        proc_ptr = &mgr_ptr->esm_bp[index];
        //Try to find a match;
        if ( (proc_ptr->inuse == TRUE)&& (proc_ptr->pti == esm_hdr->pti))
        {
          /* For most part the OTA processing for the messages is none.The case of network 
          rejecting any request from the UE are the ones that need to be handled by the 
          procedure state machine*/
          result = proc_ptr->esm_proc_state_ptr->esm_proc_process_OTA_message(
            proc_ptr, &cmd_ptr->cmd.esm_data_ind.esm_incoming_msg);
          /* Copy these keys so that BCM can use in case it needs to to respond 
             back to CM for UE initiated procedure*/
          cmd_ptr->cmd.esm_data_ind.esm_hdr.sdf_id = proc_ptr->sdf_id;
          cmd_ptr->cmd.esm_data_ind.esm_hdr.lbi = proc_ptr->lbi;
          ASSERT(proc_ptr->pending_msg_ptr != NULL);
          cmd_ptr->cmd.esm_data_ind.esm_hdr.trigger_msg_id = 
                         proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id;
          /* Release procedure if needed */  
          esm_bpm_rel_proc_ota_msg(&cmd_ptr->cmd.esm_data_ind.esm_incoming_msg,
                                   mgr_ptr, 
                                   proc_ptr);
          return result;
        }
      }
      /* There is no matching PTI. The network sent a message which it says is 
         in response to the UE initiated procedure, but UE couldn't find a 
         macting PTI */
      if(index == ESM_MAX_BP_PROCS)
      {
        MSG_ERROR_DS_1(SM_SUB, "ESM: Unknown PTI %d received by ESM ", esm_hdr->pti);
        esm_bpm_build_and_send_autonomous_reject(
                                   &cmd_ptr->cmd.esm_data_ind.esm_incoming_msg,
                                    NAS_ESM_PTI_MISMATCH);
        result = FALSE;
      }
      break;
    case NAS_ESM_FAILURE_IND:
      event.esm_in_msg = (byte)NAS_ESM_FAILURE_IND;
      event_report_payload(EVENT_LTE_EMM_INCOMING_MSG,sizeof(event), (void*)&event);
      if(esm_get_module_from_trans_id(cmd_ptr->cmd.esm_failure_ind.trans_id) != (unsigned)LTE_ESM_BPM)
      {
        return TRUE;
      }
      for (index = 0; index < ESM_MAX_BP_PROCS; index++)
      {
        proc_ptr = &mgr_ptr->esm_bp[index];
        if((proc_ptr->inuse == TRUE) &&
           (proc_ptr->pending_msg_ptr != NULL) &&
           (cmd_ptr->cmd.esm_failure_ind.trans_id == 
                                 proc_ptr->pending_msg_ptr->emm_transaction_id))
        {
          if(proc_ptr->esm_proc_state_ptr->esm_proc_process_timer_expiry_or_lower_layer_failure(proc_ptr, &cmd_ptr->cmd.esm_failure_ind) == FALSE)
          {
            esm_bpm_release_proc_resources(mgr_ptr, proc_ptr);
          }
 #ifdef TEST_FRAMEWORK
          #error code not present
#endif
          break;/* break the for loop*/
        }
      }
      break;
    case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
      event.esm_in_msg = (byte)NAS_ESM_SIG_CONNECTION_RELEASED_IND;
      event_report_payload(EVENT_LTE_EMM_INCOMING_MSG,sizeof(event), (void*)&event);
      /* Ignore this in BPM for now and let any timers run. */ 
      break;
    case NAS_ESM_ACTIVE_EPS_IND:
      event.esm_in_msg = (byte)NAS_ESM_ACTIVE_EPS_IND;
      event_report_payload(EVENT_LTE_EMM_INCOMING_MSG,sizeof(event), (void*)&event);
      break;
    case NAS_ESM_BEARER_RESOURCE_ALLOC_REQ:
      event.esm_in_msg = (byte)NAS_ESM_BEARER_RESOURCE_ALLOC_REQ;
      event_report_payload(EVENT_LTE_CM_INCOMING_MSG,sizeof(event), (void*)&event);
      if(esm_bpm_validate_bearer_resource_alloc_req(mgr_ptr,&(cmd_ptr->cmd.esm_bearer_resource_alloc_req)) == FALSE)
      {
        esm_build_and_send_cm_bearer_resource_allocation_reject(cmd_ptr->cmd.esm_bearer_resource_alloc_req.esm_hdr.sdf_id,NULL,ESM_BAD_OTA_MESSAGE,cmd_ptr->cmd.esm_bearer_resource_alloc_req.esm_hdr.lbi);
        return result;
      }
      proc_ptr = esm_get_free_proc_instance_and_pti(mgr_ptr);
      ASSERT (proc_ptr != NULL);
      proc_ptr->esm_proc_state_ptr->esm_proc_process_bearer_resource_alloc_req(proc_ptr,&(cmd_ptr->cmd.esm_bearer_resource_alloc_req));
      /*hnam*/
      esm_proc_state_info_log.instance_id = proc_ptr->instance_id;  
      esm_proc_state_info_log.proc_state = (byte)proc_ptr->esm_proc_state_ptr->state();
      esm_proc_state_info_log.pti = (byte)proc_ptr->pti;
      esm_proc_state_info_log.sdf_id = (byte)proc_ptr->sdf_id;
      ASSERT(proc_ptr->pending_msg_ptr != NULL);
      esm_proc_state_info_log.pending_msg_id = (byte)proc_ptr->pending_msg_ptr->msg_hdr.id;

      send_esm_proc_state_log_info(&esm_proc_state_info_log);

      break;
    case NAS_ESM_PDN_CONNECTIVTY_REQ:
#ifdef FEATURE_LTE_REL9 
      if(esm_chk_pdn_conn_allowance(&cmd_ptr->cmd.esm_pdn_connectivity_req, 
                                     mgr_ptr) == FALSE)
      {
        esm_build_and_send_pdn_connectivity_reject_ind(
          NULL, cmd_ptr->cmd.esm_pdn_connectivity_req.esm_hdr.sdf_id,
          NULL, ESM_LOCAL_CUASE_NONE, NULL);
        return TRUE;      
      }
#endif
      event.esm_in_msg = (byte)NAS_ESM_PDN_CONNECTIVTY_REQ;
      event_report_payload(EVENT_LTE_CM_INCOMING_MSG,sizeof(event), (void*)&event);
      proc_ptr = esm_get_free_proc_instance_and_pti(mgr_ptr);
      ASSERT (proc_ptr != NULL);

      if (cmd_ptr->cmd.esm_get_pdn_connectivity_req.seq_num == mgr_ptr->esm_emm_attach_seq_num)
	  {
	    mgr_ptr->esm_emm_attach_seq_num = 0xFF;
	  }

      MSG_HIGH_DS_2(SM_SUB, "ESM: Rcved PDN CONN REQ - Seq num %d, Req type %d",
                    cmd_ptr->cmd.esm_pdn_connectivity_req.pdn_data.seq_num,
                    cmd_ptr->cmd.esm_pdn_connectivity_req.request_type);

#ifdef FEATURE_LTE_UICC_CALL_CONTROL
      if(cmd_ptr->cmd.esm_pdn_connectivity_req.request_type == ESM_EMERGENCY)
      {
        /* Skip call cotrol for emergency PDN CONN REQ */
        proc_ptr->esm_proc_state_ptr->esm_proc_process_pdn_connect_req
                           (proc_ptr,&(cmd_ptr->cmd.esm_pdn_connectivity_req));
      }
      else
      {
        /* Perform call control */
        esm_build_and_send_pkd_pdn_connectivity_req_msg(
                            proc_ptr,&(cmd_ptr->cmd.esm_pdn_connectivity_req));
      }
#else
      proc_ptr->esm_proc_state_ptr->esm_proc_process_pdn_connect_req(proc_ptr,&(cmd_ptr->cmd.esm_pdn_connectivity_req));
#endif

      /*hnam*/
      esm_proc_state_info_log.instance_id = proc_ptr->instance_id;  
      esm_proc_state_info_log.proc_state = (byte)proc_ptr->esm_proc_state_ptr->state();
      esm_proc_state_info_log.pti = (byte)proc_ptr->pti;
      esm_proc_state_info_log.sdf_id = (byte)proc_ptr->sdf_id;
      ASSERT(proc_ptr->pending_msg_ptr != NULL);
      esm_proc_state_info_log.pending_msg_id = (byte)proc_ptr->pending_msg_ptr->msg_hdr.id;

      send_esm_proc_state_log_info(&esm_proc_state_info_log);
      break;

    case NAS_ESM_PDN_DISCONNECT_REQ:
      event.esm_in_msg = (byte)NAS_ESM_PDN_DISCONNECT_REQ;
      event_report_payload(EVENT_LTE_CM_INCOMING_MSG,sizeof(event), (void*)&event);
    
      /* This will make sure we validate the pdn disconnect req and catch the problems during testing 
         itself*/
      /* Not validating whether DS is requesting disconnect with an existing bearer id or not
         It is assumed that DS does the right thing. If DS messes up the NW will reject the request 
         and DS would assert and the problem will be caught. So, these things shouldn't happen in the field as 
         these problems are expected to be caught much before during the integration if at all */
      bearer_id = cmd_ptr->cmd.esm_pdn_disconnect_req.eps_bearer_id;
      /* Validate bearer ID. If invalid, reject the PDN disconnect req locally. */
      if(esm_validate_pdn_disconnect_req_bearer_id(bearer_id, mgr_ptr) == TRUE)
      {
        if( (cmd_ptr->cmd.esm_pdn_disconnect_req.esm_local_cause.valid == FALSE) || 
            (cmd_ptr->cmd.esm_pdn_disconnect_req.esm_local_cause.local_cause != 
                                              ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT))
        {
          /* MNK IRAT Changes*/
          /* Check if detach needs to be triggered*/
          if(mgr_ptr->check_detach_trigger(bearer_id))
          {
            /* Need to detach. Send detach cmd to EMM to trigger detach. No need to
               further process the PDN_DISCONNECT_REQ message */
            esm_build_and_send_emm_detach_cmd(ESM_LOCAL_CUASE_NONE);
          }
          else
          {
            proc_ptr = esm_get_free_proc_instance_and_pti(mgr_ptr);
            ASSERT (proc_ptr != NULL);
            proc_ptr->esm_proc_state_ptr->esm_proc_process_pdn_disconnec_req(proc_ptr,&(cmd_ptr->cmd.esm_pdn_disconnect_req) );
          }
        }
        else
        {
          proc_ptr = esm_get_free_proc_instance_and_pti(mgr_ptr);
          ASSERT (proc_ptr != NULL);
          local_cause = cmd_ptr->cmd.esm_pdn_disconnect_req.esm_local_cause;
          MSG_HIGH_DS_0(SM_SUB, "ESM: DS Bearer contexts are transfered due to IRAT, locally deactivating the contexts");
          proc_ptr->request_esm_to_deactivate_ctxts_func_ptr(local_cause,
                                                             ota_cause,
                                                             bearer_id);
          /* Need to reset here itself as there is no OTA procedure happening to terminate the procedure*/
          bpm_release_pti(mgr_ptr, proc_ptr->pti);
          esm_bpm_reset_bp(proc_ptr);
        }
      }
      else
      {
        /* Invalid EPS bearer identity. Reject PDN_DISCONNECT_REQ right away */
        esm_build_and_send_cm_pdn_disconnect_reject(bearer_id, NULL, 
                                           LTE_NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT);
      }
      /* Always return TRUE, as in this case either we are trigering detach towards EMM or sending the 
         pdn disconnect over the air*/
      result = TRUE;
      break;
    case NAS_ESM_BEARER_RESOURCE_ALLOC_ABORT_REQ:
      event.esm_in_msg = (byte)NAS_ESM_BEARER_RESOURCE_ALLOC_ABORT_REQ;
      event_report_payload(EVENT_LTE_CM_INCOMING_MSG,sizeof(event), (void*)&event);
      proc_ptr = esm_bpm_find_proc_state_machine(mgr_ptr,
                                                 cmd_ptr->cmd.esm_bearer_resource_alloc_abort_req.esm_hdr.sdf_id,
                                                 cmd_ptr->cmd.esm_bearer_resource_alloc_abort_req.esm_hdr.lbi);
      if (proc_ptr != NULL)
      {
        proc_ptr->esm_proc_state_ptr->esm_proc_process_bearer_resource_alloc_abort(proc_ptr);
        /*hnam*/
        esm_proc_state_info_log.instance_id = proc_ptr->instance_id;  
        esm_proc_state_info_log.proc_state = (byte)proc_ptr->esm_proc_state_ptr->state();
        esm_proc_state_info_log.pti = (byte)proc_ptr->pti;
        esm_proc_state_info_log.sdf_id = (byte)proc_ptr->sdf_id;
        ASSERT(proc_ptr->pending_msg_ptr != NULL);
        esm_proc_state_info_log.pending_msg_id = (byte)proc_ptr->pending_msg_ptr->msg_hdr.id;
        send_esm_proc_state_log_info(&esm_proc_state_info_log);
      }
      else
      {
        MSG_ERROR_DS_1(SM_SUB, "ESM: UNKNOWN Bearer Resource Request is asked to be aborted with SDF_ID=%d",
                       cmd_ptr->cmd.esm_bearer_resource_alloc_abort_req.esm_hdr.sdf_id);
        esm_build_and_send_bearer_resource_alloc_abort_rsp(cmd_ptr->cmd.esm_bearer_resource_alloc_abort_req.esm_hdr.sdf_id,
                                                           FALSE);
      }
      break;
    case NAS_ESM_PDN_CONNECTIVITY_ABORT_REQ:
      event.esm_in_msg = (byte)NAS_ESM_PDN_CONNECTIVITY_ABORT_REQ;
      event_report_payload(EVENT_LTE_CM_INCOMING_MSG,sizeof(event), (void*)&event);
      /* In order to reuse the function for both the NAS_ESM_BEARER_RESOURCE_ALLOC_REQ and 
      NAS_ESM_PDN_CONNECTIVTY_REQ, we use ESM_INVALID_BEARER_ID in the search for the 
      NAS_ESM_PDN_CONNECTIVITY_ABORT_REQ case. So, BPM must always set the lbi to ESM_INVALID_BEARER_ID when
      the procedure state machine terminates */
      proc_ptr = esm_bpm_find_proc_state_machine(mgr_ptr,cmd_ptr->cmd.esm_pdn_connectivity_abort_req.esm_hdr.sdf_id,
                                                 ESM_INVALID_BEARER_ID);
      if (proc_ptr != NULL)
      {
        proc_ptr->esm_proc_state_ptr->esm_proc_process_pdn_connectivity_abort(proc_ptr);
        /*hnam*/
        esm_proc_state_info_log.instance_id = proc_ptr->instance_id;  
        esm_proc_state_info_log.proc_state = (byte)proc_ptr->esm_proc_state_ptr->state();
        esm_proc_state_info_log.pti = (byte)proc_ptr->pti;
        esm_proc_state_info_log.sdf_id = (byte)proc_ptr->sdf_id;
        ASSERT(proc_ptr->pending_msg_ptr != NULL);
        esm_proc_state_info_log.pending_msg_id = (byte)proc_ptr->pending_msg_ptr->msg_hdr.id;
        send_esm_proc_state_log_info(&esm_proc_state_info_log);
      }
      else
      {
        MSG_ERROR_DS_1(SM_SUB, "ESM: UNKNOWN Bearer Resource Request is asked to be aborted with SDF_ID=%d",
                       cmd_ptr->cmd.esm_bearer_resource_alloc_abort_req.esm_hdr.sdf_id);
        esm_build_and_send_pdn_connectivity_abort_rsp(cmd_ptr->cmd.esm_bearer_resource_alloc_abort_req.esm_hdr.sdf_id,
                                                      FALSE);
      }

      break;
    case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
      event.esm_in_msg = (byte)NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND;
      event_report_payload(EVENT_LTE_CM_INCOMING_MSG,sizeof(event), (void*)&event);
      /* No need of any procedure creation for this. This is an internal message to the UE*/
      /* To start attach procedure from scratch, need to clean up all ESM 
         procedures */
      local_cause.local_cause = EMM_DETACHED;
      local_cause.valid = TRUE;
      esm_bpm_clean_up(mgr_ptr,local_cause);
      mgr_ptr->esm_emm_attach_seq_num = cmd_ptr->cmd.esm_get_pdn_connectivity_req.seq_num;
      break;

    case NAS_ESM_DETACH_IND:
      /* Get detach indication, clean up all pending bearer procedures */
      local_cause.local_cause = cmd_ptr->cmd.esm_detach_ind.cause.local_cause;
      local_cause.valid = TRUE;
      esm_bpm_clean_up(mgr_ptr,local_cause);
      mgr_ptr->esm_emm_attach_seq_num = 0xFF;
      break;
    case NAS_ESM_BEARER_RESOURCE_MODIFICATION_REQ:
      event.esm_in_msg = (byte)NAS_ESM_BEARER_RESOURCE_MODIFICATION_REQ;
      event_report_payload(EVENT_LTE_CM_INCOMING_MSG, sizeof(event), 
                           (void*)&event);
      if(esm_validate_bearer_resrc_mod_req_bearer_id(
                 cmd_ptr->cmd.esm_bearer_resource_modification_req.esm_hdr.lbi,
                 mgr_ptr) == FALSE)
      {
        esm_build_and_send_cm_bearer_resrc_mod_reject(
              cmd_ptr->cmd.esm_bearer_resource_modification_req.esm_hdr.sdf_id,
              NULL,
              LTE_NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT,
              cmd_ptr->cmd.esm_bearer_resource_modification_req.esm_hdr.lbi);
        return result;
      }
      proc_ptr = esm_get_free_proc_instance_and_pti(mgr_ptr);
      ASSERT (proc_ptr != NULL);
      proc_ptr->esm_proc_state_ptr->esm_proc_process_bearer_resrc_mod_req(proc_ptr,
                             &(cmd_ptr->cmd.esm_bearer_resource_modification_req));
      esm_proc_state_info_log.instance_id = proc_ptr->instance_id;  
      esm_proc_state_info_log.proc_state = (byte)proc_ptr->esm_proc_state_ptr->state();
      esm_proc_state_info_log.pti = (byte)proc_ptr->pti;
      esm_proc_state_info_log.sdf_id = (byte)proc_ptr->sdf_id;
      ASSERT(proc_ptr->pending_msg_ptr != NULL);
      esm_proc_state_info_log.pending_msg_id = (byte)proc_ptr->pending_msg_ptr->msg_hdr.id;
      send_esm_proc_state_log_info(&esm_proc_state_info_log);
      break;
    case NAS_ESM_EPS_BEARER_STATUS_IND:
      /* Let BCM handle it. Do nothing here. */
      break;

#ifdef FEATURE_LTE_REL10
    case NAS_ESM_UNBLOCK_ALL_APNS_IND:
    /*call SM API to send Unblock All APNs message to DS */
#ifdef FEATURE_DUAL_SIM
    sm_unblock_all_apn((sys_modem_as_id_e_type)sm_as_id);
#else
    sm_unblock_all_apn(0);
#endif
    /* Return false so that BCM ignores it */
    return FALSE;
#endif

    default:
      MSG_ERROR_DS_1(SM_SUB, "ESM: Unknown message = %d received in BPM.",
                     cmd_ptr->cmd.msg_hdr.id);
      break;
  }
  return result;
} /* handle_message() */


/*==============================================================================

  FUNCTION:  init_esm_bpm

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what init_esm_bpm() does goes here.

    @detail
    Initializes the ESM BPM including the bearer procedure instances
 */
/*============================================================================*/
void init_esm_bpm
(
  esm_bpm_T                                *mgr_ptr, 
  esm_check_if_detach_to_be_triggered_fp   *chk_trigger_ptr,
  esm_bpm_deactivate_pdn_ctxts_fp          *deact_func_ptr,
  esm_find_bearer_context_fp               *find_bearer_context_ptr,
  esm_bpm_chk_on_attach_def_bc_setup_fp    *chk_on_attach_def_bc_setup_ptr
)
{
  byte index = 0;

  ASSERT(mgr_ptr != NULL);
  ASSERT(chk_trigger_ptr != NULL);
  ASSERT(deact_func_ptr != NULL);
  ASSERT(find_bearer_context_ptr != NULL);
  ASSERT(chk_on_attach_def_bc_setup_ptr != NULL);

  for ( index = 0; index < ESM_MAX_BP_PROCS; index++ )
  {
    mgr_ptr->esm_bp[index].inuse = FALSE;
    mgr_ptr->esm_bp[index].pti = ESM_INVALID_PTI;
    mgr_ptr->esm_bp[index].pending_msg_ptr = NULL;
    mgr_ptr->esm_bp[index].sdf_id = ESM_INVALID_SDF_ID;
    mgr_ptr->esm_bp[index].esm_proc_state_ptr =  &esm_proc_state_inactive;
    mgr_ptr->esm_bp[index].instance_id = index;
    mgr_ptr->esm_bp[index].retransmit_counter = 0; ;
    mgr_ptr->esm_bp[index].request_esm_to_deactivate_ctxts_func_ptr = 
                                                                deact_func_ptr;
    mgr_ptr->esm_bp[index].chk_on_attach_def_bc_setup_ptr = 
                                                chk_on_attach_def_bc_setup_ptr;
    mgr_ptr->esm_bp[index].pco_ciphering_flag = FALSE;
    mgr_ptr->esm_bp[index].esm_local_cause.valid = FALSE;
    mgr_ptr->esm_bp[index].timer_ptr = NULL;
  }

  /*Mark PTIs to be free*/
  for ( index = 0; index < BPM_PTI_START; index++ )
  {
    mgr_ptr->pti_list[index] = FALSE;
  }

  /* Initialize to 0. PTI allocation function takes care of starting with BPM_PTI_START*/
  mgr_ptr->cur_pti_index = 0;

  mgr_ptr->handle_message_ptr = handle_message;
  mgr_ptr->check_detach_trigger = chk_trigger_ptr;
  mgr_ptr->find_bearer_context_ptr = find_bearer_context_ptr;
  mgr_ptr->esm_emm_attach_seq_num = 0xFF;

} /* init_esm_bpm() */

void bpm_get_bp_status(const esm_bpm_T* mgr_ptr)
{
  byte index = 0;

  for(index = 0; index < ESM_MAX_BP_PROCS; index++)
  {
    MSG_SPRINTF_3(MSG_SSID_LTE, 
                  MSG_LEGACY_HIGH, 
                  "ESM: BP id = %d status is %d, STATUS=%s\n",
                  index,
                  mgr_ptr->esm_bp[index].inuse,
                  mgr_ptr->esm_bp[index].esm_proc_state_ptr->get_state_name());

    if( mgr_ptr->esm_bp[index].inuse)
    {
      MSG_HIGH_DS_3(SM_SUB, "ESM: BP id = %d status is %d and pti=%d\n",
                    index, mgr_ptr->esm_bp[index].inuse, mgr_ptr->esm_bp[index].pti);
    }
  }
}

/*===========================================================================

FUNCTION  ESM_BPM_PROCESS_TIMER_EXPIRY

DESCRIPTION
  This function processes ESM timer expiry primitive

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void esm_bpm_process_timer_expiry
(
  struct esm_cmd* cmd_ptr, 
  struct esm_bpm* mgr_ptr
)
{
  log_lte_nas_esm_timer_event_type event;
  struct esm_bearer_resource_proc* proc_ptr = NULL;
  byte instance_id = (byte)(cmd_ptr->cmd.esm_timer_expired_ind.instance_id);

  ASSERT(instance_id < ESM_MAX_BP_PROCS);

  if(instance_id < ESM_MAX_BP_PROCS)
  {
    proc_ptr = &mgr_ptr->esm_bp[instance_id];
  }
  
  switch(cmd_ptr->cmd.esm_timer_expired_ind.timer_id)
  {
    case ESM_TIMER_T3480:
    case ESM_TIMER_T3481:
    case ESM_TIMER_T3482:
    case ESM_TIMER_T3492:
      MSG_HIGH_DS_1(SM_SUB, "ESM: Timer %d is expired", cmd_ptr->cmd.esm_timer_expired_ind.timer_id);
      break;
    default:
      MSG_ERROR_DS_1(SM_SUB, "ESM: Invalid timer ID on expiry - %u",
                     cmd_ptr->cmd.esm_timer_expired_ind.timer_id);
      return;
  }

  /* Release the resources allocated for this timer */
  esm_clear_timer(proc_ptr);

  if((proc_ptr->inuse == TRUE) &&
     (proc_ptr->esm_proc_state_ptr->state() != ESM_BP_INACTIVE))
  {
    esm_failure_ind_T failure_ind;

    memset((void *)&failure_ind,0,sizeof(esm_failure_ind_T));

    failure_ind.emm_cause.valid = TRUE;
    failure_ind.emm_cause.local_cause = ESM_PROC_TIME_OUT;

    if(proc_ptr->esm_proc_state_ptr->esm_proc_process_timer_expiry_or_lower_layer_failure(proc_ptr, &failure_ind) == FALSE)
    {
      esm_bpm_release_proc_resources(mgr_ptr, proc_ptr);
    }
  }
  else
  {
    MSG_HIGH_DS_1(SM_SUB, "ESM: Proc is already released on timer %d expiry",
                  cmd_ptr->cmd.esm_timer_expired_ind.timer_id);
  }

  event.esm_timer_event = cmd_ptr->cmd.esm_timer_expired_ind.timer_id;
  event_report_payload(EVENT_LTE_ESM_TIMER_EXPIRY, sizeof(event), &event);
}

/*==============================================================================

  FUNCTION:  esm_validate_pdn_disconnect_req_bearer_id

==============================================================================*/
/*!
    @brief
    This function validates the bearer ID in PDN DISCONNECT REQ message.

    @detail
    
*/
/*============================================================================*/
boolean esm_validate_pdn_disconnect_req_bearer_id
(
  byte             bearer_id,
  esm_bpm_T       *mgr_ptr
)
{
  esm_bearer_context_info_shared_T    bc_info_shared;

  ASSERT(mgr_ptr != NULL);

  memset((void *)&bc_info_shared,0x0,sizeof(esm_bearer_context_info_shared_T));

  mgr_ptr->find_bearer_context_ptr(bearer_id, &bc_info_shared);

  /* Check if bearer context is existing */
  if(bc_info_shared.bearer_id_exist == TRUE)
  {
    /* Check if the bearer context is default */
    if(bc_info_shared.default_bearer_ctxt == TRUE)
    {
      return TRUE;
    }
    else
    {
      MSG_HIGH_DS_1(SM_SUB, "ESM: Bearer ID in PDN_DISCONNECT_REQUEST is not default - %u",
                    bearer_id);
      return FALSE;
    }
  }
  else
  {
    MSG_HIGH_DS_1(SM_SUB, "ESM: Bearer ID in PDN_DISCONNECT_REQUEST is unknown - %u",
                  bearer_id);
    return FALSE;
  }
} /* esm_validate_pdn_disconnect_req_bearer_id */

/*===========================================================================

FUNCTION  ESM_BPM_ABORT_PROC_BY_BID

DESCRIPTION
  This function aborts the ongoing ESM procedure which is associated to the
  given bearer context 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  esm_bpm_abort_proc_by_bid
(
  byte        bearer_id,
  esm_bpm_T  *mgr_ptr
)
{
  byte  index = 0;

  ASSERT(mgr_ptr != NULL);

  for(index=0; index < ESM_MAX_BP_PROCS; index++)
  {
    if((mgr_ptr->esm_bp[index].inuse == TRUE) &&
       (mgr_ptr->esm_bp[index].lbi == bearer_id)) 
    {
      MSG_HIGH_DS_2(SM_SUB, "ESM: Aborting proc %d associated w/ BID %d",
                    mgr_ptr->esm_bp[index].instance_id,
                    bearer_id);
      esm_proc_send_cm_autonomous_reject(&mgr_ptr->esm_bp[index],
                                         ESM_LOWER_LAYER_FAILURE,
                                         NULL);
      esm_bpm_release_proc_resources(mgr_ptr, &mgr_ptr->esm_bp[index]);
      break;
    }               
  }

  MSG_HIGH_DS_1(SM_SUB, "ESM: No proc associated w/ BID %d to abort", bearer_id);

}/* esm_bpm_abort_proc_by_bid */

#endif /*FEATURE_LTE*/
