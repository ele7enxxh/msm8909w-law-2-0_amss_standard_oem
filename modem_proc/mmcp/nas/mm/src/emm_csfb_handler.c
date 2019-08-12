
/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_csfb_handler.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>

#ifdef FEATURE_LTE
#include "mm_v.h"
#include "emm_csfb_handler.h"
#include "emm_mem.h"
#include "emm_service_request_lib.h"
#include <dsm_item.h>
#include <dsm_init.h>
#include "msgr_nas.h"
#include "emm_rrc_if.h"
#include "emm_utility.h"
/*===========================================================================

FUNCTION  EMM_FIND_IRAT_TUNNEL_UL_MSG

DESCRIPTION
  This function finds the given IRAT tunnel uplink message in the pending
  message list and returns the pointer to it

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the given message

SIDE EFFECTS
  None

===========================================================================*/
emm_pending_irat_ul_msg_type *emm_find_irat_tunnel_ul_msg
(
  dword                trans_id,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_pending_irat_ul_msg_type  *msg_ptr = NULL;

  ASSERT(emm_ctrl_data_ptr != NULL); 

  msg_ptr = emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr;

  while(msg_ptr != NULL)
  {
    if(msg_ptr->trans_id == trans_id)
    {
      MSG_LOW_DS_1(MM_SUB, "=EMM= Found irat ul msg %d in pending list", trans_id);
      return msg_ptr;
    }
    msg_ptr = msg_ptr->next_msg_ptr;
  }

  MSG_LOW_DS_1(MM_SUB, "=EMM= Could not find irat ul msg %d in pending list", trans_id);
  return NULL ;

} /* emm_find_irat_tunnel_ul_msg */


/*===========================================================================

FUNCTION  EMM_DELETE_IRAT_TUNNEL_UL_MSG

DESCRIPTION
  This function deletes the given IRAT tunnel uplink message from the pending
  message list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_delete_irat_tunnel_ul_msg
(
  emm_pending_irat_ul_msg_type  *msg_ptr,
  emm_ctrl_data_type            *emm_ctrl_data_ptr
)
{
  emm_pending_irat_ul_msg_type   *curr_msg_ptr = NULL;
  emm_pending_irat_ul_msg_type  **prev_msg_ptr = NULL;

  ASSERT(msg_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 

  curr_msg_ptr = emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr;
  prev_msg_ptr = &emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr;

  while(curr_msg_ptr != NULL)
  {
    if(curr_msg_ptr == msg_ptr)
    {
      *prev_msg_ptr = curr_msg_ptr->next_msg_ptr;
      /* Free the memory block */
      emm_modem_mem_free(curr_msg_ptr, emm_ctrl_data_ptr);
      /* Decrease the message count */
      emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_cnt --;
      return;
    }
    else
    {
      /* Move to next message */
      prev_msg_ptr = &curr_msg_ptr->next_msg_ptr;
      curr_msg_ptr = curr_msg_ptr->next_msg_ptr;
    }
  }      
} /* emm_delete_irat_tunnel_ul_msg */

/*=========================================================================== 
 
FUNCTION  EMM_CONN_ABORT_MARK_PENDING_IRAT_MSG

DESCRIPTION
  This function marks all existing pending IRAT messages to be processed upon 
  receiving release indication with cause of aborted 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_conn_abort_mark_pending_irat_msg
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_pending_irat_ul_msg_type  *msg_ptr = NULL ;

  ASSERT(emm_ctrl_data_ptr != NULL) ; 

  MSG_LOW_DS_0(MM_SUB,"=EMM= Mark pending irat msg to be processed upon conn abort rel ind");

  /* Traverse the pending msg list and set the flag */
  if(emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr != NULL)
  {
    msg_ptr = emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr;

    while(msg_ptr != NULL)
    {
      msg_ptr->process_upon_conn_abort = TRUE;
      msg_ptr = msg_ptr->next_msg_ptr;
    }
  }
} /* emm_conn_abort_mark_pending_irat_msg */

/*===========================================================================

FUNCTION  EMM_CACHE_IRAT_TUNNEL_UL_MSG

DESCRIPTION
  This function caches the given IRAT tunnel uplink message to the end of
  pending message list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_cache_irat_tunnel_ul_msg
(
  lte_irat_tunnel_ul_msg_req_s  *msg_ptr,
  dword                          trans_id,
  rrc_irat_req_failure_cb_type   failure_cb,
  emm_ctrl_data_type            *emm_ctrl_data_ptr
)
{
  emm_pending_irat_ul_msg_type   *pending_msg_ptr = NULL; 
  emm_pending_irat_ul_msg_type   *curr_msg_ptr = NULL;  

  ASSERT(msg_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Caching UL_TUNNEL_MSG w/ Trans ID: 0x%x", trans_id);

  /* Allocate memory */
  pending_msg_ptr = emm_modem_mem_alloc(sizeof(emm_pending_irat_ul_msg_type),
                                        emm_ctrl_data_ptr);

  mm_check_for_null_ptr((void*)pending_msg_ptr);
  pending_msg_ptr->trans_id = trans_id;
  pending_msg_ptr->failure_cb = failure_cb;
  
  pending_msg_ptr->msg = *msg_ptr;

  pending_msg_ptr->next_msg_ptr = NULL;

  /* Tranverse to list end */
  if(emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr == NULL)
  {
    /* Add new msg at the end */
    emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr = pending_msg_ptr;
  }
  else
  {
    /* Tranverse to end of list */
    curr_msg_ptr = emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr;
    while(curr_msg_ptr->next_msg_ptr != NULL)
    {
      curr_msg_ptr = curr_msg_ptr->next_msg_ptr;
    }
    /* Add new msg at the end */
    curr_msg_ptr->next_msg_ptr = pending_msg_ptr;
  }

  /* Increase msg count */
  emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_cnt ++;

} /* emm_cache_irat_tunnel_ul_msg */

/*===========================================================================
 
FUNCTION  EMM_SEND_IRAT_TUNNEL_UL_MSG

DESCRIPTION
  This function sends IRAT tunnel uplink message in LTE_RRC_UL_TUNNEL_MSG_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_irat_tunnel_ul_msg
(
  lte_irat_tunnel_ul_msg_req_s  *msg_ptr,
  dword                          trans_id,
  emm_ctrl_data_type            *emm_ctrl_data_ptr
)
{
  lte_rrc_irat_tunnel_ul_msg_req_s  *ul_msg_req_ptr = NULL;

  ASSERT(msg_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS_1(MM_SUB,"=EMM= Sending LTE_RRC_UL_TUNNEL_MSG_REQ w/ Trans ID: 0x%x", trans_id);

  /* Allocate memory */
  ul_msg_req_ptr = 
                 emm_modem_mem_alloc(sizeof(lte_rrc_irat_tunnel_ul_msg_req_s),
                                     emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void*)ul_msg_req_ptr);

  msgr_init_hdr((msgr_hdr_s *)&ul_msg_req_ptr->msg_hdr, 
                MSGR_NAS_EMM, 
                LTE_RRC_IRAT_TUNNEL_UL_MSG_REQ);

  ul_msg_req_ptr->trans_id = trans_id;
  ul_msg_req_ptr->cnf_is_reqd = TRUE;

  ul_msg_req_ptr->msg = *msg_ptr;

  if(emm_msgr_send((msgr_hdr_s *)ul_msg_req_ptr, 
               sizeof(lte_rrc_irat_tunnel_ul_msg_req_s)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Failed to send LTE_RRC_UL_TUNNEL_MSG_REQ", 0,0,0 );
  } 

  /* Free momery */
  emm_modem_mem_free(ul_msg_req_ptr, emm_ctrl_data_ptr);

} /* emm_send_irat_tunnel_ul_msg */

/*=========================================================================== 
 
FUNCTION  EMM_SEND_IRAT_FAILURE_IND

DESCRIPTION
  This function sends NAS_EMM_IRAT_FAILURE_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_irat_failure_ind
(
  emm_irat_failure_cause_type  cause_type,
  byte                         cause,
  dword                        trans_id,
  uint64                       access_barring_rem_time
)
{
  emm_irat_failure_ind_type  failure_ind;

  MSG_HIGH_DS_3(MM_SUB,"=EMM= Sending IRAT_FAILURE_IND: Cause: %d, %d, Trans ID: 0x%x", 
                cause_type,
                cause,
                trans_id);

  /* Init */
  memset(&failure_ind, 0x0, sizeof(emm_irat_failure_ind_type));

  msgr_init_hdr((msgr_hdr_s *)&failure_ind.msg_hdr, 
                MSGR_NAS_EMM, 
                NAS_EMM_IRAT_FAILURE_IND);

  /* Populate the ind */
  failure_ind.trans_id = trans_id;
  failure_ind.cause_type = cause_type;

  switch(cause_type)
  {
    case LTE_NAS_IRAT_CONN_EST_FAILURE:
      failure_ind.cause.est_status = cause;
      failure_ind.access_barring_rem_time = access_barring_rem_time;
      break;
    case LTE_NAS_IRAT_CONN_REL:
      failure_ind.cause.rel_reason = cause;
      break;
    case LTE_NAS_IRAT_UL_MSG_CNF_FAILURE:
      failure_ind.cause.cnf_status = cause;
      break;
    case LTE_NAS_IRAT_SR_REJECT:
      failure_ind.cause.emm_cause = cause;
      break;
    case LTE_NAS_IRAT_ACCESS_BARRING:
      failure_ind.access_barring_rem_time = access_barring_rem_time;
      break;
    default:
      break;
  }

  if(emm_msgr_send((msgr_hdr_s *)&failure_ind, sizeof(failure_ind)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= Failed to send NAS_EMM_IRAT_FAILURE_IND", 0,0,0);
  } 

} /* emm_send_irat_failure_ind */

/*=========================================================================== 
 
FUNCTION  EMM_PROCESS_IRAT_UL_MSG_REQ

DESCRIPTION
  This function processes EMM_NAS_IRAT_UL_MSG_REQ
  - When EMM in right state
  -- Sends the message to RRC when connected
  -- Set up connection and send message to RRC when not connected
  - When EMM not in right state 
  -- Sends failure indication 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  emm_process_irat_ul_msg_req
(
  emm_irat_ul_msg_req_type  *req_ptr,
  emm_ctrl_data_type        *emm_ctrl_data_ptr
)
{  
  emm_csfb_rsp                     csfb_rsp;
  ASSERT(req_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);

  memset((void *)&csfb_rsp,0,sizeof(emm_csfb_rsp));

  switch(EMM_GET_STATE())
  {
    case EMM_REGISTERED:
      if(((EMM_GET_SUBSTATE() == EMM_REGISTERED_NORMAL_SERVICE) ||
          (EMM_GET_SUBSTATE() == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM))
          &&
         (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED) 
          &&
         ((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_AVAILABLE) ||
          (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_CS_INVALID)) 
          &&
         (emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                    SYS_SRV_STATUS_SRV)
          &&
          (emm_ctrl_data_ptr->reg_complete_ack_pending ==
                                               EMM_PENDING_REG_ACK_NONE)
#ifdef FEATURE_LTE_REL10
        && ((mm_timer_status[TIMER_T3346] == TIMER_STOPPED)
#ifdef FEATURE_LTE_REL11
          || ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
              ((emm_check_high_priority(emm_ctrl_data_ptr)) ||
               (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE)))
#endif
           )
#endif
         && (mm_timer_status[TIMER_T3440] == TIMER_STOPPED)
          && (emm_reg_container == NULL))
      {
        /* Cache the message in pending ul irat msg list */
        emm_cache_irat_tunnel_ul_msg(&req_ptr->msg,
                                      req_ptr->trans_id,
                                      emm_send_irat_failure_ind,
                                      emm_ctrl_data_ptr);

        if(EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE) 
        {
          emm_send_irat_tunnel_ul_msg(&req_ptr->msg,
                                       req_ptr->trans_id,
                                       emm_ctrl_data_ptr);
        }
        else
        {
          if(emm_check_esr_for_ps(emm_ctrl_data_ptr) == TRUE)
          {
            /* Start extended service request procedure for PS */
            emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MO_DATA,LTE_PACKET_SERVICES_VIA_S1,
                                                         csfb_rsp, NAS_EMM_CSFB_NONE);
          }
          else
          {
          /* Signaling connection is not existing.
             Start service request procedure with the cause of MO_SIGNALING */
            emm_start_nas_service_request_procedure(emm_ctrl_data_ptr, LTE_RRC_EST_CAUSE_MO_DATA);
          }
        }
      }
      else
      {
        emm_send_irat_failure_ind(LTE_NAS_IRAT_INVALID_STATE, 
                                  LTE_NAS_IRAT_FAILURE_CAUSE_NONE, 
                                  req_ptr->trans_id,
                                  LTE_NAS_IRAT_ACCESS_BARRING_NONE);
      }
      break ;
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      /* Signaling connection will be set up. Cache the message for now. */
      emm_cache_irat_tunnel_ul_msg(&req_ptr->msg,
                                    req_ptr->trans_id,
                                    emm_send_irat_failure_ind,
                                    emm_ctrl_data_ptr);
      break;
    case EMM_SERVICE_REQUEST_INITIATED:
      /* Cache the message */
      emm_cache_irat_tunnel_ul_msg(&req_ptr->msg,
                                    req_ptr->trans_id,
                                    emm_send_irat_failure_ind,
                                    emm_ctrl_data_ptr);

      if((emm_ctrl_data_ptr->nas_secure_exchange == TRUE) && 
         (EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE))
      {
        emm_send_irat_tunnel_ul_msg(&req_ptr->msg,
                                    req_ptr->trans_id, 
                                    emm_ctrl_data_ptr);
      }
      break;
    default:
      emm_send_irat_failure_ind(LTE_NAS_IRAT_INVALID_STATE, 
                                LTE_NAS_IRAT_FAILURE_CAUSE_NONE, 
                                req_ptr->trans_id,
                                LTE_NAS_IRAT_ACCESS_BARRING_NONE);
      break;
  }
} /* emm_process_irat_ul_msg_req */

/*=========================================================================== 
 
FUNCTION    EMM_PROCESS_RRC_IRAT_TUNNEL_DL_MSG_IND

DESCRIPTION
  This function processes LTE_RRC_IRAT_TUNNEL_DL_MSG_IND. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_rrc_irat_tunnel_dl_msg_ind
(
  lte_rrc_irat_tunnel_dl_msg_ind_s  *rrc_ind_ptr,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
)
{
  emm_irat_dl_msg_ind_type  *dl_msg_ind_ptr = NULL;
  emm_state_type          emm_state = (emm_state_type)EMM_UNKNOWN_STATE;

  ASSERT(rrc_ind_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 


  emm_state = EMM_GET_STATE();

  if((emm_state == EMM_DEREGISTERED)||
    (emm_state == EMM_DEREGISTERED_INITIATED)||
    (emm_state == EMM_REGISTERED_INITIATED))
  {
    MSG_HIGH_DS_2(MM_SUB,"=EMM= Drop NAS_EMM_IRAT_DL_MSG_IND w/ msg - %d as EMM state is %d",
                  rrc_ind_ptr->msg.dl_msg_type,
                  emm_state);
    return;
  }

  MSG_HIGH_DS_1(MM_SUB,"=EMM= Sending NAS_EMM_IRAT_DL_MSG_IND w/ msg - %d",
                rrc_ind_ptr->msg.dl_msg_type);

  /* Allocate memory */
  dl_msg_ind_ptr = emm_modem_mem_alloc(sizeof(emm_irat_dl_msg_ind_type),
                                       emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void*)dl_msg_ind_ptr);

  msgr_init_hdr((msgr_hdr_s *)&dl_msg_ind_ptr->msg_hdr, 
                MSGR_NAS_EMM, 
                NAS_EMM_IRAT_DL_MSG_IND); 
  
  dl_msg_ind_ptr->msg = rrc_ind_ptr->msg;

  if(emm_msgr_send((msgr_hdr_s *)dl_msg_ind_ptr, 
               sizeof(emm_irat_dl_msg_ind_type)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Failed to send NAS_EMM_IRAT_DL_MSG_IND", 0,0,0);
  }
  
  emm_modem_mem_free(dl_msg_ind_ptr, emm_ctrl_data_ptr); 

} /* emm_process_rrc_irat_tunnel_dl_msg_ind */

/*=========================================================================== 
 
FUNCTION  EMM_SEND_IRAT_IMSI_ATTACH_IND

DESCRIPTION
  This function sends EMM_NAS_IRAT_IMSI_ATTACH_IND when combined attach is
  completed successfully

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_irat_imsi_attach_ind(void)
{
  emm_irat_imsi_attach_ind_type  imsi_attach_ind;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending NAS_EMM_IRAT_IMSI_ATTACH_IND");

  /* Init */
  memset((void *)&imsi_attach_ind, 0x0, sizeof(emm_irat_imsi_attach_ind_type));

  msgr_init_hdr((msgr_hdr_s *)&imsi_attach_ind.msg_hdr, 
                MSGR_NAS_EMM, 
                NAS_EMM_IRAT_IMSI_ATTACH_IND); 
  
  if(emm_msgr_send((msgr_hdr_s *)&imsi_attach_ind, 
               sizeof(emm_irat_imsi_attach_ind_type)) != E_SUCCESS)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Failed to send NAS_EMM_IRAT_IMSI_ATTACH_IND");
  } 

} /* emm_send_irat_imsi_attach_ind */

/*=========================================================================== 
 
FUNCTION  EMM_SEND_IRAT_CTXT_LOST_IND

DESCRIPTION
  This function sends EMM_NAS_IRAT_CTXT_LOST_IND with given cause

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_irat_ctxt_lost_ind
(
  emm_irat_ctxt_lost_cause_type  cause 
)
{
  emm_irat_ctxt_lost_ind_type  ctxt_lost_ind;

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sending NAS_EMM_IRAT_CTXT_LOST_IND w/ cause %d ", cause);

  /* Init */
  memset((void *)&ctxt_lost_ind, 0x0, sizeof(emm_irat_ctxt_lost_ind_type));

  msgr_init_hdr((msgr_hdr_s *)&ctxt_lost_ind.msg_hdr, 
                MSGR_NAS_EMM, 
                NAS_EMM_IRAT_CTXT_LOST_IND); 
  
  ctxt_lost_ind.cause = cause;

  if(emm_msgr_send((msgr_hdr_s *)&ctxt_lost_ind, 
               sizeof(emm_irat_ctxt_lost_ind_type)) != E_SUCCESS)
  {
    MSG_ERROR_DS_0(MM_SUB,"=EMM= Failed to send NAS_EMM_IRAT_CTXT_LOST_IND");
  } 

} /* emm_send_irat_ctxt_lost_ind */

/*=========================================================================== 
 
FUNCTION  EMM_SEND_IRAT_UL_MSG_CNF

DESCRIPTION
  This function sends NAS_EMM_IRAT_UL_MSG_CNF with transaction ID

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_send_irat_ul_msg_cnf
(
  uint32  trans_id
)
{
  emm_irat_ul_msg_cnf_type  irat_ul_msg_cnf;

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sending NAS_EMM_IRAT_UL_MSG_CNF w/ trans ID %x", trans_id);

  /* Init */
  memset((void *)&irat_ul_msg_cnf, 0x0, sizeof(emm_irat_ul_msg_cnf_type));

  msgr_init_hdr((msgr_hdr_s *)&irat_ul_msg_cnf.msg_hdr,
                MSGR_NAS_EMM, 
                NAS_EMM_IRAT_UL_MSG_CNF); 
  
  irat_ul_msg_cnf.trans_id = trans_id;

  if(emm_msgr_send((msgr_hdr_s *)&irat_ul_msg_cnf,sizeof(emm_irat_ul_msg_cnf_type))
     != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Failed to send NAS_EMM_IRAT_UL_MSG_CNF", 0,0,0);
  } 

} /* emm_send_irat_ul_msg_cnf */

#ifdef FEATURE_LTE_TO_HDR_OH 
/***************************************************************************/
/*                       LTE and HDR Interfaces                            */
/***************************************************************************/

/*===========================================================================
 
FUNCTION  EMM_IRAT_FREE_DSM_TUNNEL_UL_MSG

DESCRIPTION
  This function would free the dsm items of the failed UL MSG Req

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_irat_free_dsm_tunnel_ul_msg
(
  uint8              num_dsm_ptr,
  dsm_item_type      **dsm_ptr  
)
{
  uint8 i = 0;

  for(i = 0; i < num_dsm_ptr && 
             i < LTE_RRC_CDMA_TUNNEL_MAX_DSM_ARRAY_SIZE; i++)
  {
    /* free the dsm item */  
    dsm_free_packet(&dsm_ptr[i]);   

    /* Received dsm should have been freed */
    ASSERT(dsm_ptr[i] == NULL);

  }
}

/*===========================================================================

FUNCTION  EMM_CACHE_IRAT_3GPP2_TUNNEL_UL_MSG

DESCRIPTION
  This function caches the given IRAT tunnel uplink message to the end of
  pending message list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_cache_irat_3gpp2_tunnel_ul_msg
(
  lte_irat_cdma_tunnel_ul_msg_s     *irat_ul_msg,
  dword                              trans_id,
  uint8                              num_dsm_ptr,
  dsm_item_type                     **dsm_ptr,  
  rrc_irat_req_failure_cb_type       failure_cb,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
)
{
  emm_pending_irat_ul_msg_type   *pending_msg_ptr = NULL; 
  emm_pending_irat_ul_msg_type   *curr_msg_ptr = NULL;  
  
  uint8  i=0;
  
  ASSERT(irat_ul_msg != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Caching HDR_UL_TUNNEL_MSG w/ Trans ID: 0x%x", trans_id);

  /* Allocate memory */
  pending_msg_ptr = emm_modem_mem_alloc(sizeof(emm_pending_irat_ul_msg_type),
                                        emm_ctrl_data_ptr);

  mm_check_for_null_ptr((void*)pending_msg_ptr);

  pending_msg_ptr->trans_id    = trans_id;
  pending_msg_ptr->num_dsm_ptr = num_dsm_ptr;
  
  for(i=0;i < num_dsm_ptr &&
          i < LTE_RRC_CDMA_TUNNEL_MAX_DSM_ARRAY_SIZE; i++)
  {
    pending_msg_ptr->dsm_ptr[i] = dsm_ptr[i];
  }

  pending_msg_ptr->failure_cb = failure_cb;
  
  pending_msg_ptr->irat_ul_msg = *irat_ul_msg;

  pending_msg_ptr->next_msg_ptr = NULL;

  /* Tranverse to list end */
  if(emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr == NULL)
  {
    /* Add new msg at the end */
    emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr = pending_msg_ptr;
  }
  else
  {
    /* Tranverse to end of list */
    curr_msg_ptr = emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr;
    while(curr_msg_ptr->next_msg_ptr != NULL)
    {
      curr_msg_ptr = curr_msg_ptr->next_msg_ptr;
    }
    /* Add new msg at the end */
    curr_msg_ptr->next_msg_ptr = pending_msg_ptr;
  }

  /* Increase msg count */
  emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_cnt ++;

} /* emm_cache_irat_3gpp2_tunnel_ul_msg */

/*===========================================================================
 
FUNCTION  EMM_SEND_IRAT_3GPP2_TUNNEL_UL_MSG

DESCRIPTION
  This function sends IRAT tunnel uplink message in
  LTE_RRC_IRAT_HDR_TUNNEL_UL_MSG_REQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_irat_3gpp2_tunnel_ul_msg
(
  lte_irat_cdma_tunnel_ul_msg_s     *irat_ul_msg,
  dword                              trans_id,
  uint8                              num_dsm_ptr,
  dsm_item_type                     **dsm_ptr,  
  emm_ctrl_data_type                *emm_ctrl_data_ptr
)
{
  lte_irat_cdma_tunnel_ul_msg_req_s ul_msg_req;
  emm_pending_irat_ul_msg_type  *msg_ptr = NULL;
  uint8  i=0;
  msgr_attach_struct_type *att_ptr = NULL;         /* Message attachment pointer */

  ASSERT(irat_ul_msg != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL);


  /* Init */
  memset(&ul_msg_req, 0x0, sizeof(lte_irat_cdma_tunnel_ul_msg_req_s));

  msgr_init_hdr_attach((msgr_hdr_s *)&ul_msg_req.msg_hdr, 
                       MSGR_NAS_EMM, 
                       LTE_RRC_IRAT_HDR_UL_TUNNEL_MSG_REQ, 0, num_dsm_ptr);


  MSG_HIGH_DS_2(MM_SUB,"=EMM= Sending LTE_RRC_IRAT_HDR_TUNNEL_UL_MSG_REQ w/ Trans ID: 0x%x  num_dsm %d",
                trans_id, num_dsm_ptr);

  for(i = 0; (i < num_dsm_ptr) && (i < LTE_RRC_CDMA_TUNNEL_MAX_DSM_ARRAY_SIZE); i++)
  {
    /* Set DSM attach */
    att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&ul_msg_req, i);
    ASSERT(att_ptr != NULL);

    msgr_set_dsm_attach(att_ptr, dsm_ptr[i]);
    ASSERT(&dsm_ptr[i] != NULL);
  }

  ul_msg_req.trans_id    = trans_id;
  ul_msg_req.cnf_is_reqd = TRUE;

  ul_msg_req.msg = *irat_ul_msg;

  if(emm_msgr_send((msgr_hdr_s *)&ul_msg_req, 
               sizeof(lte_irat_cdma_tunnel_ul_msg_req_s)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Failed to send LTE_RRC_IRAT_HDR_UL_TUNNEL_MSG_REQ", 0,0,0 );
  } 

  if((msg_ptr = emm_find_irat_tunnel_ul_msg(trans_id, 
                                            emm_ctrl_data_ptr))!= NULL)
  {
    /*once the message is sent to RRC, dsm item will be freed by RRC
      if UL_MSG_CNF is not recvd for sent UL_MSG_REQ we do not free dsm item*/
    msg_ptr->num_dsm_ptr = 0;
  }
  else
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= could not find transaction_id 0x%x", trans_id);
  }
} /* emm_send_irat_3gpp2_tunnel_ul_msg */
  
/*=========================================================================== 
 
FUNCTION  EMM_SEND_IRAT_3GPP2_FAILURE_IND

DESCRIPTION
  This function sends NAS_EMM_IRAT_HDR_FAILURE_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_irat_3gpp2_failure_ind
(
  emm_irat_failure_cause_type  cause_type,
  byte                         cause,
  dword                        trans_id,
  uint64                       access_barring_rem_time
)
{
  emm_irat_failure_ind_type  failure_ind;

  /* Init */
  memset(&failure_ind, 0x0, sizeof(emm_irat_failure_ind_type));

  msgr_init_hdr((msgr_hdr_s *)&failure_ind.msg_hdr, 
                MSGR_NAS_EMM, 
                NAS_EMM_IRAT_HDR_FAILURE_IND);

  MSG_HIGH_DS_3(MM_SUB,"=EMM= Sending IRAT_HDR_FAILURE_IND: Cause: %d, %d, Trans ID: 0x%x", 
                cause_type, cause, trans_id);


  /* Populate the ind */
  failure_ind.trans_id   = trans_id;
  failure_ind.cause_type = cause_type;

  switch(cause_type)
  {
    case LTE_NAS_IRAT_CONN_EST_FAILURE:
      failure_ind.cause.est_status = cause;
      failure_ind.access_barring_rem_time = access_barring_rem_time;
      break;
    case LTE_NAS_IRAT_CONN_REL:
      failure_ind.cause.rel_reason = cause;
      break;
    case LTE_NAS_IRAT_UL_MSG_CNF_FAILURE:
      failure_ind.cause.cnf_status = cause;
      break;
    case LTE_NAS_IRAT_SR_REJECT:
      failure_ind.cause.emm_cause = cause;
      break;
    case LTE_NAS_IRAT_ACCESS_BARRING:
      failure_ind.access_barring_rem_time = access_barring_rem_time;
      break;
    default:
      break;
  }

  if(emm_msgr_send((msgr_hdr_s *)&failure_ind, sizeof(failure_ind)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= Failed to send NAS_EMM_IRAT_FAILURE_IND", 0,0,0);
  } 

} /* emm_send_irat_3gpp2_failure_ind */

/*=========================================================================== 
 
FUNCTION  EMM_PROCESS_IRAT_3GPP2_UL_MSG_REQ

DESCRIPTION
  This function processes EMM_NAS_IRAT_HDR_UL_MSG_REQ from HDR
  - When EMM in right state
  -- Sends the message to RRC when connected
  -- Set up connection and send message to RRC when not connected
  - When EMM not in right state 
  -- Sends failure indication 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  emm_process_irat_3gpp2_ul_msg_req
(
  emm_irat_3gpp2_ul_msg_req_type  *irat_ul_msg_req,
  emm_ctrl_data_type              *emm_ctrl_data_ptr
)
{  
  uint8 num_attach, i;
  dsm_item_type            *dsm_ptr[LTE_RRC_CDMA_TUNNEL_MAX_DSM_ARRAY_SIZE];
  msgr_attach_struct_type  *att_ptr;
  emm_csfb_rsp                     csfb_rsp;

  ASSERT(irat_ul_msg_req != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);

  memset((void *)&csfb_rsp,0,sizeof(emm_csfb_rsp));

  num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)irat_ul_msg_req);
  ASSERT(num_attach != 0);

  for(i=0; i< num_attach && i<LTE_RRC_CDMA_TUNNEL_MAX_DSM_ARRAY_SIZE ;i++)
  {
    att_ptr = msgr_get_attach((msgr_hdr_struct_type *)irat_ul_msg_req, i);
    ASSERT(att_ptr != NULL);

    msgr_get_dsm_attach(att_ptr, &dsm_ptr[i]);
    ASSERT(&dsm_ptr[i] != NULL);
  }

  switch(EMM_GET_STATE())
  {
    case EMM_REGISTERED:
      if(((EMM_GET_SUBSTATE() == EMM_REGISTERED_NORMAL_SERVICE) ||
          (EMM_GET_SUBSTATE() == EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM))
          &&
         (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED) 
          &&
         ((emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_AVAILABLE) ||
          (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                    SYS_SIM_STATE_CS_INVALID)) 
          &&
         (emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                    SYS_SRV_STATUS_SRV)
#ifdef FEATURE_LTE_REL10
        && ((mm_timer_status[TIMER_T3346] == TIMER_STOPPED)
#ifdef FEATURE_LTE_REL11
          || ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
              ((emm_check_high_priority(emm_ctrl_data_ptr)) ||
               (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE)))
#endif
           )
#endif
         && (mm_timer_status[TIMER_T3440] == TIMER_STOPPED)
          && (emm_reg_container == NULL))
      {
        /* Cache the message in pending ul irat msg list */
        emm_cache_irat_3gpp2_tunnel_ul_msg(&irat_ul_msg_req->msg,
                                         irat_ul_msg_req->trans_id,
                                         num_attach,
                                         dsm_ptr,
                                         emm_send_irat_3gpp2_failure_ind,
                                         emm_ctrl_data_ptr);

        if(EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE) 
        {
          emm_send_irat_3gpp2_tunnel_ul_msg(&irat_ul_msg_req->msg,
                                          irat_ul_msg_req->trans_id,
                                          num_attach,
                                          dsm_ptr,
                                          emm_ctrl_data_ptr);
        }
        else
        {
          if(emm_check_esr_for_ps(emm_ctrl_data_ptr) == TRUE)
          {
            /* Start extended service request procedure for PS */
            emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MO_DATA,LTE_PACKET_SERVICES_VIA_S1,
                                                         csfb_rsp, NAS_EMM_CSFB_NONE);
          }
          else
          {
            /* Signaling connection is not existing.
             Start service request procedure with the cause of MO_SIGNALING */
            emm_start_nas_service_request_procedure(emm_ctrl_data_ptr, LTE_RRC_EST_CAUSE_MO_DATA);
          }
        }
      }
      else
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= free the HDR UL MSG Req dsm items %d", num_attach);
        emm_irat_free_dsm_tunnel_ul_msg(num_attach, dsm_ptr);

        emm_send_irat_3gpp2_failure_ind(LTE_NAS_IRAT_INVALID_STATE,
                                      LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                      irat_ul_msg_req->trans_id,
                                      LTE_NAS_IRAT_ACCESS_BARRING_NONE);
      }
      break ;
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      /* Signaling connection will be set up. Cache the message for now. */
      emm_cache_irat_3gpp2_tunnel_ul_msg(&irat_ul_msg_req->msg,
                                       irat_ul_msg_req->trans_id,
                                       num_attach,
                                       dsm_ptr,
                                       emm_send_irat_3gpp2_failure_ind,
                                       emm_ctrl_data_ptr);
      break;
    case EMM_SERVICE_REQUEST_INITIATED:
      /* Cache the message */
      emm_cache_irat_3gpp2_tunnel_ul_msg(&irat_ul_msg_req->msg,
                                       irat_ul_msg_req->trans_id,
                                       num_attach,
                                       dsm_ptr,
                                       emm_send_irat_3gpp2_failure_ind,
                                       emm_ctrl_data_ptr);

      if((emm_ctrl_data_ptr->nas_secure_exchange == TRUE) && 
         (EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE))
      {
        emm_send_irat_3gpp2_tunnel_ul_msg(&irat_ul_msg_req->msg,
                                        irat_ul_msg_req->trans_id,
                                        num_attach,
                                        dsm_ptr,
                                        emm_ctrl_data_ptr);
      }
      break;
    default:
       MSG_HIGH_DS_1(MM_SUB, "=EMM= free the HDR UL MSG Req dsm items %d", num_attach);
       emm_irat_free_dsm_tunnel_ul_msg(num_attach, dsm_ptr);
       emm_send_irat_3gpp2_failure_ind(LTE_NAS_IRAT_INVALID_STATE,
                                      LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                      irat_ul_msg_req->trans_id,
                                      LTE_NAS_IRAT_ACCESS_BARRING_NONE);
      break;
  }
} /* emm_process_irat_3gpp2_ul_msg_req */

/*=========================================================================== 
 
FUNCTION    EMM_PROCESS_RRC_IRAT_3GPP2_TUNNEL_DL_MSG_IND

DESCRIPTION
  This function processes LTE_RRC_IRAT_HDR_TUNNEL_DL_MSG_IND. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_rrc_irat_3gpp2_tunnel_dl_msg_ind
(
  lte_irat_cdma_tunnel_dl_msg_ind_s  *rrc_ind_ptr,
  emm_ctrl_data_type                 *emm_ctrl_data_ptr
)
{
  emm_irat_3gpp2_dl_msg_ind_type  dl_msg_ind;
 
  uint8 num_attach = 0;
  dsm_item_type            *dsm_ptr = NULL;
  msgr_attach_struct_type  *att_ptr;
  
  ASSERT(rrc_ind_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 
  
  /* reset the memory */
  memset(&dl_msg_ind, 0x0, sizeof(emm_irat_3gpp2_dl_msg_ind_type));

  MSG_HIGH_DS_1(MM_SUB,"=EMM= Sending NAS_EMM_IRAT_HDR_DL_MSG_IND w/ msg - %d",
                rrc_ind_ptr->dl_msg_info.dl_msg_type);

  if(rrc_ind_ptr->dl_msg_info.dl_msg_type 
                              == LTE_IRAT_TUNNEL_DL_MSG_DL_INFO_TRANSFER)
  {
    num_attach = msgr_get_num_attach((msgr_hdr_struct_type *)rrc_ind_ptr);
    ASSERT(num_attach != 0);
  
    att_ptr = msgr_get_attach((msgr_hdr_struct_type *)&rrc_ind_ptr->msg_hdr, 0);
    ASSERT(att_ptr != NULL);
  
    msgr_get_dsm_attach(att_ptr, &dsm_ptr);
    ASSERT(dsm_ptr != NULL);

    msgr_init_hdr_attach((msgr_hdr_s *)&dl_msg_ind.msg_hdr, 
                         MSGR_NAS_EMM, 
                         NAS_EMM_IRAT_HDR_DL_MSG_IND,0,1); 
  
    /* get DSM attach */
    att_ptr = msgr_get_attach(&dl_msg_ind.msg_hdr, 0);

    /* set DSM ptr */
    msgr_set_dsm_attach(att_ptr, dsm_ptr);
    ASSERT(dsm_ptr != NULL);
  }
  else
  {
    msgr_init_hdr((msgr_hdr_s *)&dl_msg_ind.msg_hdr, 
                   MSGR_NAS_EMM, 
                   NAS_EMM_IRAT_HDR_DL_MSG_IND); 
  }

  dl_msg_ind.dl_msg_info = rrc_ind_ptr->dl_msg_info;

  if(emm_msgr_send((msgr_hdr_s *)&dl_msg_ind, 
               sizeof(emm_irat_3gpp2_dl_msg_ind_type)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Failed to send NAS_EMM_IRAT_DL_MSG_IND", 0,0,0);
  }
  
} /* emm_process_rrc_irat_3gpp2_tunnel_dl_msg_ind */


/*=========================================================================== 
 
FUNCTION  EMM_SEND_IRAT_3GPP2_UL_MSG_CNF

DESCRIPTION
  This function sends NAS_EMM_IRAT_HDR_UL_MSG_CNF with transaction ID

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_send_irat_3gpp2_ul_msg_cnf
(
  uint32  trans_id
)
{
  emm_irat_ul_msg_cnf_type  irat_ul_msg_cnf;

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Sending NAS_EMM_IRAT_UL_MSG_CNF w/ trans ID %x", trans_id);

  /* Init */
  memset((void *)&irat_ul_msg_cnf, 0x0, sizeof(emm_irat_ul_msg_cnf_type));

  msgr_init_hdr((msgr_hdr_s *)&irat_ul_msg_cnf.msg_hdr,
                MSGR_NAS_EMM, 
                NAS_EMM_IRAT_HDR_UL_MSG_CNF); 
  
  irat_ul_msg_cnf.trans_id = trans_id;

  if(emm_msgr_send((msgr_hdr_s *)&irat_ul_msg_cnf,sizeof(emm_irat_ul_msg_cnf_type))
     != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Failed to send NAS_EMM_IRAT_HDR_UL_MSG_CNF", 0,0,0);
  } 

} /* emm_send_irat_3gpp2_ul_msg_cnf */

#endif

/*===========================================================================

FUNCTION  EMM_PROCESS_RRC_IRAT_TUNNEL_UL_MSG_CNF

DESCRIPTION
  This function processes LTE_RRC_IRAT_TUNNEL_UL_MSG_CNF
  - Failure: Calls registered failure call back funtion and delete the message
             from pending message list
  - Success: Delete the message from pending message liest 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_rrc_irat_tunnel_ul_msg_cnf
(
  lte_rrc_irat_tunnel_ul_msg_cnf_s  *rrc_cnf_ptr,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
)
{
  emm_pending_irat_ul_msg_type  *msg_ptr = NULL;
  uint8                          index  = 0 ;
  uint8                          tech_id=0;

  ASSERT(rrc_cnf_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL); 

  tech_id = (uint8)(rrc_cnf_ptr->trans_id[index] >> 24);

  for(index=0; index < rrc_cnf_ptr->num_trans_ids; index++)
  {
    MSG_LOW_DS_2(MM_SUB,"=EMM= Processing irat tunnel ul msg cnf w/ trans id %d, status %d",
                 rrc_cnf_ptr->trans_id[index], rrc_cnf_ptr->status);
        
    msg_ptr = emm_find_irat_tunnel_ul_msg(rrc_cnf_ptr->trans_id[index], 
                                          emm_ctrl_data_ptr);

    if(msg_ptr != NULL)
    {
      if(rrc_cnf_ptr->status == LTE_RRC_TUNNEL_UL_MSG_CNF_SUCCESS)
      {  
        switch(tech_id)
        {
          case NAS_1XCP: 
            emm_send_irat_ul_msg_cnf(rrc_cnf_ptr->trans_id[index]);
            break;
#ifdef FEATURE_LTE_TO_HDR_OH 
          case NAS_HDR:
            emm_send_irat_3gpp2_ul_msg_cnf(rrc_cnf_ptr->trans_id[index]);
            break;
#endif
          default:
            MSG_ERROR_DS_1(MM_SUB,"Unknown Techonology group trans_id",
                           rrc_cnf_ptr->trans_id[index]);
            break;
        }
      }
      else if(msg_ptr->failure_cb != NULL)
      {
        /* Appropriate cb for 1x or HDR is called and the message
           is sent accordingly */
        msg_ptr->failure_cb((byte)LTE_NAS_IRAT_UL_MSG_CNF_FAILURE,
                            (byte)rrc_cnf_ptr->status,
                            msg_ptr->trans_id,
                            LTE_NAS_IRAT_ACCESS_BARRING_NONE);     
      }
      /* Remove the message from pending list */
      emm_delete_irat_tunnel_ul_msg(msg_ptr, emm_ctrl_data_ptr);
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB,"=EMM= Could not find irat tunnel ul msg w/ trans id 0x%x",
                     rrc_cnf_ptr->trans_id[index]);
    }
  }
} /* emm_process_rrc_irat_tunnel_ul_msg_cnf */

/*===========================================================================

FUNCTION  EMM_SEND_ALL_PENDING_IRAT_MSG

DESCRIPTION
  This function sends all messages in the pending IRAT message list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_all_pending_irat_msg
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_pending_irat_ul_msg_type  *msg_ptr = NULL;

  ASSERT(emm_ctrl_data_ptr != NULL); 

  msg_ptr = emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr;

  while(msg_ptr != NULL)
  {
#ifdef FEATURE_LTE_TO_HDR_OH 
    if(msg_ptr->trans_id >> 24 ==  NAS_HDR )
    {
       emm_send_irat_3gpp2_tunnel_ul_msg(&msg_ptr->irat_ul_msg,
                                       msg_ptr->trans_id,
                                       msg_ptr->num_dsm_ptr,
                                       msg_ptr->dsm_ptr,
                                       emm_ctrl_data_ptr);
    }
    else
#endif
    {
      /* Tranverse the whole pending list and send all message */
      emm_send_irat_tunnel_ul_msg(&msg_ptr->msg,
                                   msg_ptr->trans_id,
                                   emm_ctrl_data_ptr);
    }
    msg_ptr = msg_ptr->next_msg_ptr;
  }

} /* emm_send_all_pending_irat_msg */

/*=========================================================================== 
 
FUNCTION  EMM_PROCESS_PENDING_IRAT_MSG

DESCRIPTION
  This function traverse the pending IRAT message list and processes messages
  if needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_pending_irat_msg
(
  byte                 cause_type,
  byte                 cause,
  uint64               ab_rem_time,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_pending_irat_ul_msg_type  **prev_msg_ptr = NULL;
  emm_pending_irat_ul_msg_type   *curr_msg_ptr = NULL;
  boolean                         msg_processed = FALSE;

  ASSERT(emm_ctrl_data_ptr != NULL); 

  curr_msg_ptr = emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr;
  prev_msg_ptr = &emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_list_ptr;       

  while(curr_msg_ptr != NULL)
  {          
    msg_processed = FALSE;

    /* Process the message if connection release or est failure 
       - IS NOT due to connection abortion 
       or 
       - IS due to connection abortion and processing flag is set  
    */
    if(((cause_type != LTE_NAS_IRAT_CONN_EST_FAILURE || 
         emm_ctrl_data_ptr->aborting_conn == FALSE)
        &&
        (cause_type != LTE_NAS_IRAT_CONN_REL || 
         emm_ctrl_data_ptr->aborting_conn == FALSE)) 
       ||
       (curr_msg_ptr->process_upon_conn_abort == TRUE))
    {
      /* Call the tx failure call back function if there is one */
      if(curr_msg_ptr->failure_cb != NULL)
      {
#ifdef FEATURE_LTE_TO_HDR_OH 
        /* If UL MSG Req is from HDR free the dsm item and send
           failure ind */
         if(curr_msg_ptr->trans_id >> 24 ==  NAS_HDR)
         {
           MSG_HIGH_DS_1(MM_SUB,"=EMM= free pending HDR UL MSG Req dsm items %d", 
                         curr_msg_ptr->num_dsm_ptr);
           emm_irat_free_dsm_tunnel_ul_msg(curr_msg_ptr->num_dsm_ptr, 
                                           curr_msg_ptr->dsm_ptr);
         }
#endif
        /* Appropriate cb are registered for 1x and HDR. Messages are
           sent accordingly */
        curr_msg_ptr->failure_cb(cause_type,
                                 cause,
                                 curr_msg_ptr->trans_id,
                                 ab_rem_time);
      }
      msg_processed = TRUE;
    }            

    if(msg_processed == TRUE)
    {
      /* Remove the processed message */
      *prev_msg_ptr = curr_msg_ptr->next_msg_ptr;
      emm_modem_mem_free(curr_msg_ptr, emm_ctrl_data_ptr);
      emm_ctrl_data_ptr->pending_irat_ul_msg_list.msg_cnt --;
      curr_msg_ptr = *prev_msg_ptr;            
    }
    else
    {
      /* Message is not processed. Find next message. */
      prev_msg_ptr = &curr_msg_ptr->next_msg_ptr;
      curr_msg_ptr = curr_msg_ptr->next_msg_ptr;
    }
  }
} /* emm_process_pending_irat_msg */

/*=========================================================================== 
 
FUNCTION  EMM_PROCESS_CSFB_PRIMITIVE

DESCRIPTION
  This function processes CSFB primitives

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_process_csfb_primitive
(
  void                   *cmd_ptr,
  emm_ctrl_data_type     *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp  **mm_emm_dispatcher_fn_ptr
)
{
  emm_csfb_cmd_type  *csfb_cmd_ptr = NULL;

  ASSERT(cmd_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);
  
  csfb_cmd_ptr = (emm_csfb_cmd_type *)cmd_ptr; 

  switch(csfb_cmd_ptr->cmd.msg_hdr.id)
  {
    case NAS_EMM_IRAT_UL_MSG_REQ:
      emm_process_irat_ul_msg_req(&csfb_cmd_ptr->cmd.irat_ul_msg_req,
                                   emm_ctrl_data_ptr);
      break;
#ifdef FEATURE_LTE_TO_HDR_OH 
    case NAS_EMM_IRAT_HDR_UL_MSG_REQ:
      emm_process_irat_3gpp2_ul_msg_req(&csfb_cmd_ptr->cmd.irat_3gpp2_ul_msg_req,
                                      emm_ctrl_data_ptr);
      break;
#endif
    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Unknown IRAT primitive %d", csfb_cmd_ptr->cmd.msg_hdr.id);
      break;
  }

  return TRUE;

}/* emm_process_irat_primitive */


#endif  /*FEATURE_LTE*/
