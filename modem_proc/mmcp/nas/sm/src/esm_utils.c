

/*!
  @file
  esm_utils.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_utils.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/17/12   cogolt Added Rel-10 support for ESM Backoff timer (T3396) 
2/07/12   SAR     populate emm_failure_type to ESM->CM in failure ind  
08/04/11   zren    Updated ESM to propagate SM group ID to CM for befault Bearer
                   context activation
11/29/10   MNK     Additional check for neg_qos presence for deciding if the ctxt is transferbale to GW or not & More 1XCSFB changes including adversarial scenarios
10/07/10   MNK     1XCSFB introduction 
08/11/10   MNK     Changes to TI of UMTS 
05/25/10   zren    Updated to also send DEACT IND to CM when bearer context is 
                   in avtive pending state
05/14/10   RI      Mainlined spec changes upto Dec 09.
03/16/10   zren    Updated to fill CM modification request with saved data from
                   OTA modification request message 
03/11/10   MNK     Changes for transaction_id optional param in def & ded bearer context requests
02/17/10   zren    Added support to always deact bearer upon DS request in UE 
                   initiated modification procedure
02/10/10   zren    Updated to pass APN_AMBR info to CM/DS 
02/03/10   zren    Added ESM new timer machanism
01/21/10   zren    Added EMM/ESM STATUS feature
12/10/09   zren    Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
12/01/09   MNK     Changed apn_address to address in ESM_PRINT_DATA
11/18/09   zren    Added support for ESM Information procedure
11/03/09   zren    Updated to use modified cm_modify_bearer_context_request_ind_s
                   structure 
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
10/14/09   MNK     Interface change for building and sending the detach cmd
10/01/09   zren    reset APN name and PCo flaggs to FALSE
09/28/09   MNK     Remove unwanted fields in the CM-NAS interface 
09/28/09   MNK     LTE-eHRPD IRAT changes 
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/21/09   MNK     Changes due to IRAT design
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
09/14/09   zren    Added bearer ID and ESM cause check for PDN DISCONNECT 
                   REQ/REJ messages
09/14/09   hnam    Activated NAS events
09/14/09   zren    Updated to check pco_ciphering_flag in pdn_connectivity_req
09/12/09   MNK     Update rb_id in the CM_DRB_SETUP_IND
09/01/09   MNK     Added handling of detach_ind from EMM, as we support bearer initiated procedures
08/18/09   MNK     CM-NAS interface changes
08/14/09   MNK     Add Support for PDN disconnect request
08/14/09   hnam    Added support NAS events
08/10/09   MNK     Added local_cause handling for dedicated bearer reject
07/31/09   MNK     Added local_cause handling for default bearer reject
07/30/09   MNK     Changes for multiple PDN connectivity support
06/22/09   zren    Updated to use MM_CM_DRB_REESTABLISH_REJECT_IND for service
                   request procedure failures
06/02/09   MNK     Changed dedicated bearer req to use esm_hdr for lbi  
06/02/09   MNK     Added additional criteria of mathcing the LBI to match the UE initiated request  
05/21/09   MNK     Added support to reply back to CM/DS in case the NW sent bad messages as a response to UE initiated request
05/19/09   MNK     Include new header file esm_ext_constants.h
05/11/09   MNK     Store the transaction Id when doing retransmission
05/11/09   MNK     Recognize bearer reject and store sdf_id for correlation
05/08/09   MNK     Added support for bearer resource allocation procedures
05/01/09   MNK     Added support for N/W initiated bearer context deactivation
04/28/09   MNK     Added support for EPS_BEARER_STATUS message generation
04/28/09   MNK     Changed esm_information to esm_information_transfer_bit
04/24/09   MNK     Added code to build and send detach_cmd 
04/16/09   MNK     More code added to support different scenarios for handling multiple bearers upon attach
04/06/09   MNK     Added to use the new TRANSACTION_ID format
03/06/09   MNK     Change trans_id to begin from 1
03/06/09   MNK     Cause code handling in building of the drb reetablis failure indication
03/05/09   MNK     Add ASSERTION for esm_msgr_send failures
03/02/09   MNK     Add code to handle bad ESM messages received over the air
02/23/09   MNK     Added code to generate local default context reject towards the N/W
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
#include "bit.h"
#include "lte_nas_common_v.h"
#include "esm_cm_msgr_enum_msg.h" 
#include "esm_emm_msgr_enum_msg.h" 
#include "esm_v.h"
#include "esm_bcm.h"
#include "esm_bpm.h"
#include "esm_utils.h"
#include "emm_esm_if_msg.h"
#include "cm_esm.h"
#include "timers_v.h"
#include "gs_v.h"
#include "esm_ext_constants.h"
#include "naslog_v.h"
#include "event.h"
#include "event_defs.h"
#include "lte_nas.h"
#include "lte_nas_msg_parse.h"
#include "msg_lib_encode_esm.h"
#include "cm_msgr_msg.h"

#include "esm_onex_csfb.h"
#include "msgr_nas.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include "stringl.h"
/*==============================================================================

                         LOCAL VARIABLES

==============================================================================*/
#ifdef TEST_FRAMEWORK
  #error code not present
#endif // TEST_FRAMEWORK

/* trans_id 0 is considered invalid*/
static byte esm_msg_seq_num = 1;

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE)) 
#error code not present
#endif // TEST_FRAMEWORK


extern boolean sm_check_if_no_conn_exists(byte  connection_id);

extern bearer_ctrl_mode_T sm_get_bcm_from_pco(const protocol_cfg_options_T *pco);

/* required for LTE CIT */
#ifdef FEATURE_LTE 
void esm_reset_msg_seq_num(void)
{
  esm_msg_seq_num = 1;
}
#endif 

/*==============================================================================

  FUNCTION:  esm_build_and_send_cm_act_def_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_cm_act_def_ctxt_req() does goes here.

    @detail
    Builds and sends cm activate default context request.
*/
/*============================================================================*/
void esm_build_and_send_cm_act_def_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr   /*!< REQUIRED short parameter description */
)
{
  
  cm_act_default_bearer_context_request_ind_s_type *cm_def_req_cmd;
  errno_enum_type        e_retval;
  cm_def_req_cmd = (cm_act_default_bearer_context_request_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_act_default_bearer_context_request_ind_s_type));
  


  /* Fill the CM command directly later when the definition of cmd type available*/
  //cm_act_default_bearer_context_request_ind_s_type* cm_def_req_cmd = &(cm_cmd.cm_act_default_bearer_context_request_ind);
  //cm_act_default_bearer_context_request_ind_s_type* cm_def_req_cmd = &cm_cmd;
  /* Fill the msgr header*/
  msgr_init_hdr(&cm_def_req_cmd->msg_hdr,MSGR_NAS_ESM,MM_CM_ACT_DEFAULT_BEARER_CONTEXT_REQUEST_IND);
  /* Fill the ESM header*/
  cm_def_req_cmd->esm_hdr.connection_id = ctxt_ptr->connection_id;
  /*cm_def_req_cmd->esm_hdr.con_id_creator = ctxt_ptr->con_id_creator ;*/
  cm_def_req_cmd->esm_hdr.sdf_id = ctxt_ptr->sdf_id;

  /* LBI doesn't apply to DEFAULT BEARER*/
  cm_def_req_cmd->esm_hdr.lbi = ESM_INVALID_BEARER_ID;

  /* transaction identifier is not being passed to the DS as it is not needed 
  by the DS*/
  cm_def_req_cmd->eps_bearer_id = ctxt_ptr->eps_bearer_id;
  cm_def_req_cmd->sdf_qos = ctxt_ptr->sdf_qos;

  if(ctxt_ptr->apn_name.valid)
  {
    cm_def_req_cmd->apn_name.valid = TRUE;
    cm_def_req_cmd->apn_name  = ctxt_ptr->apn_name;
  }
  else
  {
    cm_def_req_cmd->apn_name.valid = FALSE;
  }
  if(ctxt_ptr->esm_cause.valid)
  {
    cm_def_req_cmd->esm_cause.valid = TRUE;
    cm_def_req_cmd->esm_cause = ctxt_ptr->esm_cause;
  }
  else
  {
    cm_def_req_cmd->esm_cause.valid = FALSE;
  }
  if(ctxt_ptr->neg_llc_sapi.valid)
  {
    cm_def_req_cmd->neg_llc_sapi.valid = TRUE;
    cm_def_req_cmd->neg_llc_sapi = ctxt_ptr->neg_llc_sapi;
  }
  else
  {
    cm_def_req_cmd->neg_llc_sapi.valid = FALSE;
  }
  if(ctxt_ptr->neg_qos.valid)
  {
    cm_def_req_cmd->neg_qos.valid = TRUE;
    cm_def_req_cmd->neg_qos = ctxt_ptr->neg_qos;
  }
  else
  {
    cm_def_req_cmd->neg_qos.valid = FALSE;
  }
  if(ctxt_ptr->pdn_address.valid)
  {
    cm_def_req_cmd->pdn_address.valid = TRUE;
    cm_def_req_cmd->pdn_address = ctxt_ptr->pdn_address;
  }
  else
  {
    cm_def_req_cmd->pdn_address.valid = FALSE;
  }
  if(ctxt_ptr->config_options.valid)
  {
    cm_def_req_cmd->protocol_config_options.valid = TRUE;
    cm_def_req_cmd->protocol_config_options = ctxt_ptr->config_options;
  }
  else
  {
    cm_def_req_cmd->protocol_config_options.valid = FALSE;
  }
  if(ctxt_ptr->apn_ambr.valid == TRUE)
  {
    cm_def_req_cmd->apn_ambr = ctxt_ptr->apn_ambr;
  }
  else
  {
    cm_def_req_cmd->apn_ambr.valid = FALSE;
  }

  if( (ctxt_ptr->ti.valid == TRUE) && (ctxt_ptr->neg_qos.valid == TRUE) )
  {
    cm_def_req_cmd->gsm_umts_connection_id.valid = TRUE;
    cm_def_req_cmd->gsm_umts_connection_id.connection_id = ctxt_ptr->ti.connection_id;
    cm_def_req_cmd->group_id = ctxt_ptr->sm_group_id;
    MSG_HIGH_DS_3(SM_SUB, "ESM: Def BC %d is transferable - UMTS CONN ID %d, Group ID %d",
                  cm_def_req_cmd->eps_bearer_id,
                  cm_def_req_cmd->gsm_umts_connection_id.connection_id,
                  cm_def_req_cmd->group_id);
  }
  else
  {
    cm_def_req_cmd->gsm_umts_connection_id.valid = FALSE;
  }


  e_retval = esm_msgr_send( (msgr_hdr_s*)cm_def_req_cmd, sizeof(cm_act_default_bearer_context_request_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent MM_CM_ACT_DEFAULT_BEARER_CONTEXT_REQUEST_IND");

  /*Free up memory allocated for message*/
  esm_modem_mem_free(cm_def_req_cmd);

} /* esm_build_and_send_cm_act_def_ctxt_req() */

/*==============================================================================

  FUNCTION:  esm_get_trans_id_for_data_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_get_trans_id_for_data_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_get_trans_id_for_data_req() goes here.
      +---------+---------+----------+------------------------------+
      |  8 bits      |8 bits      |  8 bits        | 8 bits         |
      |  TECHNOLOGY  |MODULE_ID   |  Instance Id   | Transaction Id |
      +-------------------------------------------------------------+
      - TECHNLOGY specifies LTE, eHRPD etc..
      - MODULE_ID specifies ESM-BCM, ESM-BPM, EMM etc..
      - Index is the index into either the BP(Bearer Procedure) or 
        BC(Bearer Context) instance
      - trans_id is ESM genrated part of transaction id
      - the 32 bits put together is the transaction id used between 
        the EMM and RRC
      - When EMM responds back with a failure indication it returns 
        the 32 bit transaction id , the index 
        part of 8 bits is used to index into the BP or BC array to find 
        the instance and trans_id part of 8 bits is not needed to be 
        used at this time for a failure indication

*/
/*============================================================================*/
static dword esm_get_trans_id_for_data_req
(
  byte index,  /*!< REQUIRED short parameter description */
  lte_nas_module_id_T mod_id 
)
{
  dword ret_val = 0;

  /* Populate the TECHNOLOGY */
  ret_val |= ((unsigned int)NAS_LTE << 24);
  ret_val |= (((unsigned int)mod_id) << 16);

  ret_val |= (index << 8) ;
  /* Populate the transaction part of the trnsaction id*/
  ret_val |= (esm_msg_seq_num);

  esm_msg_seq_num++;
  if(esm_msg_seq_num == 0)
  {
    /* trans_id 0 is invalid and should not be used*/
    esm_msg_seq_num = 1;
  }
  return ret_val;
} /* esm_get_trans_id_for_data_req() */


/*==============================================================================

  FUNCTION:  esm_build_and_send_act_def_ctxt_accept

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_act_def_ctxt_accept() does goes here.

    @detail
    Builds and sends cm activate default context accept.
*/
/*============================================================================*/
void esm_build_and_send_act_def_ctxt_accept
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_act_default_bearer_context_accept_T* msg_ptr/*!< REQUIRED short parameter description */
)
{
  
  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;

  
  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T)); 
  MSG_LOW_DS_0(SM_SUB, "ESM: ENTER esm_build_and_send_act_def_ctxt_accept\n");

  /* Init the MSGR header*/
  msgr_init_hdr(&out_msg_ptr->msg_hdr,MSGR_NAS_ESM,NAS_EMM_DATA_REQ);
  /*emm_cmd.emm_data_req.emm_transaction_id = emm_get_nas_transaction_id ();*/
  out_msg_ptr->emm_transaction_id = 0;
  out_msg_ptr->emm_transaction_id = esm_get_trans_id_for_data_req(ctxt_ptr->connection_id,LTE_ESM_BCM);

  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT;

  /* Fill the esm specific header*/
  out_msg_ptr->esm_out_msg.default_bearer_context_accept.lte_nas_esm_hdr.bearer_id = ctxt_ptr->eps_bearer_id;
  out_msg_ptr->esm_out_msg.default_bearer_context_accept.lte_nas_esm_hdr.pti = 0x0;

  if(msg_ptr->protocol_config_options.valid)
  {
    out_msg_ptr->esm_out_msg.default_bearer_context_accept.protocol_conf_opt.valid = TRUE;
    out_msg_ptr->esm_out_msg.default_bearer_context_accept.protocol_conf_opt = msg_ptr->protocol_config_options;
    /* DS is overwriting what N/W sent or what N/W accepted earlier when we received the request*/
    ctxt_ptr->config_options = msg_ptr->protocol_config_options;
  }
  else
  {
    out_msg_ptr->esm_out_msg.default_bearer_context_accept.protocol_conf_opt.valid = FALSE;
  }

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent NAS_EMM_DATA_REQ carrying ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT");

  
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(out_msg_ptr);
} /* esm_build_and_send_act_def_ctxt_accept() */

/*==============================================================================

  FUNCTION:  esm_build_and_send_act_def_ctxt_reject

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_act_def_ctxt_reject() does goes here.

    @detail
    Builds and sends cm activate default context reject.
*/
/*============================================================================*/
void esm_build_and_send_act_def_ctxt_reject 
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_act_default_bearer_context_rej_T* msg_ptr /*!< REQUIRED short parameter description */
)
{
  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;

  

  
  emm_data_req_T *out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T)); 

  msgr_init_hdr(&out_msg_ptr->msg_hdr,MSGR_NAS_ESM,NAS_EMM_DATA_REQ);

  out_msg_ptr->emm_transaction_id = 0;
  out_msg_ptr->emm_transaction_id = esm_get_trans_id_for_data_req(ctxt_ptr->connection_id,LTE_ESM_BCM);

  /*ESM header is not needed to be filled for EMM*/
  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT;

  /* Fill the esm specific header*/
  out_msg_ptr->esm_out_msg.default_bearer_context_rej.lte_nas_esm_hdr.bearer_id = ctxt_ptr->eps_bearer_id;
  out_msg_ptr->esm_out_msg.default_bearer_context_rej.lte_nas_esm_hdr.pti = 0x0;

  /* Is assertion the right thing to do or just drop the message?*/
  /*ASSERT(out_msg_ptr->default_bearer_context_reject.esm_cause.valid == TRUE);*/

  if(msg_ptr->esm_cause.valid == TRUE)
  {
   out_msg_ptr->esm_out_msg.default_bearer_context_rej.esm_cause = msg_ptr->esm_cause; 
  }
  else
  {
    out_msg_ptr->esm_out_msg.default_bearer_context_rej.esm_cause.esm_cause = NAS_ESM_PROTOCOL_ERROR ;
  }

  out_msg_ptr->esm_out_msg.default_bearer_context_rej.esm_cause.valid = TRUE;

  if(msg_ptr->protocol_config_options.valid)
  {
    out_msg_ptr->esm_out_msg.default_bearer_context_rej.protocol_conf_opt.valid = TRUE;
    out_msg_ptr->esm_out_msg.default_bearer_context_rej.protocol_conf_opt = msg_ptr->protocol_config_options;
    /* No need to overwrite in the ctxt as we are rejecting the request*/
  }
  else
  {
    out_msg_ptr->esm_out_msg.default_bearer_context_rej.protocol_conf_opt.valid = FALSE;
  }

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) ); 
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent NAS_EMM_DATA_REQ carrying ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT");
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(out_msg_ptr);

} /* esm_build_and_send_act_def_ctxt_reject */


/*==============================================================================

  FUNCTION:  esm_build_and_send_autonomous_act_def_ctxt_reject

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_autonoumous_act_def_ctxt_reject() does goes here.

    @detail
    Builds and sends cm activate default context reject.
*/
/*============================================================================*/
void esm_build_and_send_autonomous_act_def_ctxt_reject 
(
  const lte_nas_esm_act_default_bearer_context_req* ota_msg_ptr,
  const esm_act_default_bearer_context_rej_T* msg_ptr /*!< REQUIRED short parameter description */
)
{
  errno_enum_type        e_retval;

  
  log_lte_nas_esm_outgoing_msg_event_type event;


  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));
  msgr_init_hdr(&out_msg_ptr->msg_hdr,MSGR_NAS_ESM,NAS_EMM_DATA_REQ);

  /* Using ff as reserved trans Id as the number of contexts can't be more than 11. 
  Ideally this transaction id is derived in conjunction with the connection id. 
  In the case of autonomously sending the reject there is no context existing. Hence we are using the ff
  to build the transaction id*/
  out_msg_ptr->emm_transaction_id = esm_get_trans_id_for_data_req(ESM_RESERVED_CONNECTION_ID,LTE_ESM_BCM);

  /*ESM header is not needed to be filled for EMM*/
  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT;

  /* Fill the esm specific header*/
  //out_msg_ptr->default_bearer_context_rej.lte_nas_esm_hdr.bearer_id = 0;
  out_msg_ptr->esm_out_msg.default_bearer_context_rej.lte_nas_esm_hdr.bearer_id = ota_msg_ptr->lte_nas_esm_hdr.bearer_id;
  //out_msg_ptr->default_bearer_context_rej.lte_nas_esm_hdr.pti = ota_msg_ptr->lte_nas_esm_hdr.pti;
  out_msg_ptr->esm_out_msg.default_bearer_context_rej.lte_nas_esm_hdr.pti = 0;

  /* Is assertion the right thing to do or just drop the message?*/
  /*ASSERT(out_msg_ptr->dedicated_bearer_context_reject.esm_cause.valid == TRUE);*/

  out_msg_ptr->esm_out_msg.default_bearer_context_rej.esm_cause = msg_ptr->esm_cause;

  out_msg_ptr->esm_out_msg.default_bearer_context_rej.protocol_conf_opt.valid = FALSE;

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) ); 
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent NAS_EMM_DATA_REQ carrying ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT");
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(out_msg_ptr);

} /* esm_build_and_send_autonomous_act_def_ctxt_reject */


/*==============================================================================

  FUNCTION:  esm_build_and_send_autonomous_act_dedicated_ctxt_reject

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_autonoumous_act_dedicated_ctxt_reject() does goes here.

    @detail
    Builds and sends cm activate default context reject.
*/
/*============================================================================*/
void esm_build_and_send_autonomous_act_dedicated_ctxt_reject 
(
  const lte_nas_esm_act_dedicated_bearer_context_req* ota_msg_ptr,
  const esm_act_dedicated_bearer_context_rej_T* msg_ptr /*!< REQUIRED short parameter description */
)
{
  errno_enum_type        e_retval;

  
  log_lte_nas_esm_outgoing_msg_event_type event;

  
  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));
  msgr_init_hdr(&out_msg_ptr->msg_hdr,MSGR_NAS_ESM,NAS_EMM_DATA_REQ);

  /* Using ff as reserved trans Id as the number of contexts can't be more than 11. 
  Ideally this transaction id is derived in conjunction with the connection id. 
  In the case of autonomously sending the reject there is no context existing. Hence we are using the ff
  to build the transaction id*/
  out_msg_ptr->emm_transaction_id = esm_get_trans_id_for_data_req(ESM_RESERVED_CONNECTION_ID,LTE_ESM_BCM);

  /*ESM header is not needed to be filled for EMM*/
  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT;

  /* Fill the esm specific header*/
  //out_msg_ptr->dedicated_bearer_context_rej.lte_nas_esm_hdr.bearer_id = 0;
  out_msg_ptr->esm_out_msg.dedicated_bearer_context_rej.lte_nas_esm_hdr.bearer_id = ota_msg_ptr->lte_nas_esm_hdr.bearer_id;
  //out_msg_ptr->dedicated_bearer_context_rej.lte_nas_esm_hdr.pti = ota_msg_ptr->lte_nas_esm_hdr.pti;
  out_msg_ptr->esm_out_msg.dedicated_bearer_context_rej.lte_nas_esm_hdr.pti = 0x0;

  /* Is assertion the right thing to do or just drop the message?*/
  /*ASSERT(out_msg_ptr->default_bearer_context_reject.esm_cause.valid == TRUE);*/

  out_msg_ptr->esm_out_msg.dedicated_bearer_context_rej.esm_cause = msg_ptr->esm_cause;

  out_msg_ptr->esm_out_msg.dedicated_bearer_context_rej.protocol_conf_opt.valid = FALSE;

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) ); 
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent NAS_EMM_DATA_REQ carrying ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT");
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(out_msg_ptr);
} /* esm_build_and_send_autonomous_act_dedicated_ctxt_reject */

/*==============================================================================

  FUNCTION:  esm_build_and_send_autonomous_modify_ctxt_reject

==============================================================================*/
/*!
    @brief
    Builds and sends Modify ESP Bearer Context Reject

    @detail   
*/
/*============================================================================*/
void esm_build_and_send_autonomous_modify_ctxt_reject 
(
  const lte_nas_esm_modify_eps_bearer_ctxt_req      *modify_req_ptr,
  const esm_modify_bearer_context_rej_T             *modify_rej_ptr
)
{
  errno_enum_type        e_retval;

  
  log_lte_nas_esm_outgoing_msg_event_type    event;
  
  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));
  ASSERT(modify_req_ptr != NULL);
  ASSERT(modify_rej_ptr != NULL);

  

  msgr_init_hdr(&out_msg_ptr->msg_hdr, MSGR_NAS_ESM, NAS_EMM_DATA_REQ);

  /* Using ff as reserved trans Id as the number of contexts can't be more than 11. 
  Ideally this transaction id is derived in conjunction with the connection id. 
  In the case of autonomously sending the reject there is no context existing. Hence we are using the ff
  to build the transaction id*/
  out_msg_ptr->emm_transaction_id = esm_get_trans_id_for_data_req(ESM_RESERVED_CONNECTION_ID,
                                                                          LTE_ESM_BCM);
  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = MODIFY_EPS_BEARER_CONTEXT_REJECT;

  /* Fill the esm specific header*/
  out_msg_ptr->esm_out_msg.modify_eps_bearer_ctxt_rej.lte_nas_esm_hdr.bearer_id = 
                                     modify_req_ptr->lte_nas_esm_hdr.bearer_id;
  out_msg_ptr->esm_out_msg.modify_eps_bearer_ctxt_rej.lte_nas_esm_hdr.pti = 
                                           modify_req_ptr->lte_nas_esm_hdr.pti;

  out_msg_ptr->esm_out_msg.modify_eps_bearer_ctxt_rej.esm_cause = modify_rej_ptr->esm_cause;
  out_msg_ptr->esm_out_msg.modify_eps_bearer_ctxt_rej.protocol_conf_opt.valid = FALSE;

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) ); 
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: Send NAS_EMM_DATA_REQ carrying MODIFY_EPS_BEARER_CONTEXT_REJECT");
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(out_msg_ptr);

} /* esm_build_and_send_autonomous_modify_ctxt_reject */

/*==============================================================================

  FUNCTION:  esm_build_and_send_deactivate_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_deactivate_ind() does goes here.

    @detail
    Builds and sends cm deactivate ind.
*/
/*============================================================================*/
void esm_build_and_send_deactivate_ind
(
  esm_eps_bearer_context_T* ctxt_ptr,   /*!< REQUIRED short parameter description */
  lte_nas_local_cause_T local_cause,
  esm_cause_T cause
)
{
  
  errno_enum_type                         e_retval;
  esm_bc_state_T                          cur_state;
  log_lte_nas_esm_outgoing_msg_event_type event;

  
  cm_deact_bearer_context_request_ind_s_type* msg_ptr;
  msg_ptr = (cm_deact_bearer_context_request_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_deact_bearer_context_request_ind_s_type));

  ASSERT(ctxt_ptr != NULL);

  cur_state = ctxt_ptr->eps_bearer_state_ptr->state();

  if(cur_state != ESM_BC_INACTIVE)
  {
    msgr_init_hdr(&msg_ptr->msg_hdr,MSGR_NAS_ESM,MM_CM_DEACT_BEARER_CONTEXT_REQUEST_IND);

    /* Fill the ESM header*/
    msg_ptr->esm_hdr.connection_id = ctxt_ptr->connection_id;
    msg_ptr->esm_hdr.sdf_id = ctxt_ptr->sdf_id;

    msg_ptr->eps_bearer_id = ctxt_ptr->eps_bearer_id;
    if(local_cause.valid)
    {
      msg_ptr->esm_local_cause = local_cause;
      msg_ptr->esm_local_cause.valid = TRUE;
    }
    else
    {
      msg_ptr->esm_local_cause.valid = FALSE;
    }
    if(cause.valid)
    {
      msg_ptr->esm_cause = cause;
      msg_ptr->esm_cause.valid = TRUE;
      msg_ptr->protocol_config_options = ctxt_ptr->config_options;
    }
    else
    {
      msg_ptr->esm_cause.valid = FALSE;
    }

    msg_ptr->backoff_timer = ctxt_ptr->backoff_timer;
    MSG_HIGH_DS_2(SM_SUB, "ESM: Sending DEACT BC REQ - t3396 active value %d timer value %d",msg_ptr->backoff_timer.active,ctxt_ptr->backoff_timer.timer_count);

    e_retval = esm_msgr_send( (msgr_hdr_s*)msg_ptr, sizeof(cm_deact_bearer_context_request_ind_s_type) );
    ASSERT(e_retval == E_SUCCESS);
    MSG_HIGH_DS_3(SM_SUB, "ESM: Sending DEACT BC REQ - BID %d, Default %d, LBI %d",
                  ctxt_ptr->eps_bearer_id,
                  ctxt_ptr->default_bearer_ctxt,
                  ctxt_ptr->lbi);
    event.esm_out_msg = (byte)MM_CM_DEACT_BEARER_CONTEXT_REQUEST_IND;
    event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);
  }
  else
  {
    MSG_HIGH_DS_0(SM_SUB, "ESM: BC is already deactivated");
  }
  /*Free up memory allocated for message*/
  esm_modem_mem_free(msg_ptr);

} /* esm_build_and_send_deactivate_ind() */

/*==============================================================================

  FUNCTION:  esm_build_and_send_cm_drb_released_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_cm_drb_released_ind() does goes here.

    @detail
    Builds and sends cm drb released ind.
*/
/*============================================================================*/
void esm_build_and_send_cm_drb_released_ind
(
  esm_eps_bearer_context_T* ctxt_ptr, /*!< REQUIRED short parameter description */
  lte_nas_local_cause_T emm_cause 
)
{  
  
  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;
  
  cm_drb_released_ind_s_type* cm_msg_ptr;
  cm_msg_ptr = (cm_drb_released_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_drb_released_ind_s_type));

  msgr_init_hdr(&(cm_msg_ptr->msg_hdr),MSGR_NAS_ESM,MM_CM_ACT_DRB_RELEASED_IND);

  cm_msg_ptr->esm_hdr.connection_id = ctxt_ptr->connection_id;
  cm_msg_ptr->eps_bearer_id = ctxt_ptr->eps_bearer_id;
  if(emm_cause.valid)
  {
    cm_msg_ptr->esm_local_cause     = emm_cause;
    cm_msg_ptr->esm_local_cause.valid     = TRUE;
    cm_msg_ptr->esm_cause.valid = FALSE;
  }
  else
  {
    cm_msg_ptr->esm_local_cause.valid     = FALSE;
    /* esm cause is alwyas false in this scenario*/
    cm_msg_ptr->esm_cause.valid = FALSE;
  }

  /*
  1. get cm_cmd_type
  2. Initialize the message router header in the cm_cmd_type
  3. copy the message into the cm_cmd_type
  4. Call esm_msgr_send
  */

  e_retval = esm_msgr_send( (msgr_hdr_s*)cm_msg_ptr, sizeof(cm_drb_released_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent MM_CM_ACT_DRB_RELEASED_IND");
  event.esm_out_msg = (byte)MM_CM_ACT_DRB_RELEASED_IND;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);

  esm_modem_mem_free(cm_msg_ptr);
} /* esm_build_and_send_cm_drb_released_ind() */

/*==============================================================================

  FUNCTION:  esm_build_pdn_connectivity_msg

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_pdn_connectivity_msg() does goes here.

    @detail
    Builds the PDN Connectivity message.

    NOTE: DO NOT DO ANY RESETTING OF ptr2esm_msg as it carries some valid data when entering this function.

*/
/*============================================================================*/
void esm_build_pdn_connectivity_msg
(
  const esm_bearer_resource_proc_T    *proc_ptr, 
  const esm_pdn_connectivity_req_T    *data_ptr,  
  lte_nas_esm_outgoing_msg_type       *ptr2esm_msg
)
{
  ASSERT(ptr2esm_msg != NULL);
  ASSERT(data_ptr != NULL);

  ptr2esm_msg->nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES ;
  ptr2esm_msg->nas_hdr.msg_id = PDN_CONNECTIVITY_REQUEST;

  ptr2esm_msg->pdn_connectivity_req.lte_nas_esm_hdr.pti = proc_ptr->pti;
  ptr2esm_msg->pdn_connectivity_req.lte_nas_esm_hdr.bearer_id = 
                                                         ESM_INVALID_BEARER_ID;

  ptr2esm_msg->pdn_connectivity_req.req = data_ptr->request_type;
  ptr2esm_msg->pdn_connectivity_req.pdn_type = data_ptr->pdn_data.pdn_type;

  /* Set APN and PCO to FALSE to begin with*/
  ptr2esm_msg->pdn_connectivity_req.access_point_name.valid = FALSE;
  ptr2esm_msg->pdn_connectivity_req.protocol_conf_opt.valid = FALSE;

  ptr2esm_msg->pdn_connectivity_req.seq_num = data_ptr->pdn_data.seq_num;

  if(proc_ptr->chk_on_attach_def_bc_setup_ptr() == TRUE)
  {
    /* Additional PDN connectivity after attach */
    /* Always set esm_info_transfer_bit to false. No ESM Info proc should be 
       triggered. */ 
    ptr2esm_msg->pdn_connectivity_req.esm_info_transfer_bit = FALSE;

    /* Always pass APN and PCO from DS to NW */
    ptr2esm_msg->pdn_connectivity_req.access_point_name = 
                                                   data_ptr->pdn_data.apn_name;
    ptr2esm_msg->pdn_connectivity_req.protocol_conf_opt = 
                                    data_ptr->pdn_data.protocol_config_options;
  }
  else
  {
    /* PDN connectivity during attach */

    /* UE shall never send APN in the PDN connectivity message. If the UE has 
       APN to send during this process,  it must set the esm_info_transfer flag
       to TRUE. NW will come back with ESM info request, ESM sends APN in ESM 
       information response. */

    ptr2esm_msg->pdn_connectivity_req.esm_info_transfer_bit = 
                                     data_ptr->pdn_data.esm_info_transfer_flag;

    /* If pco_ciphering_flag is not set, send PCO. 
       Otherwise, do not send PCO now. ESM should send PCO in ESM information 
       response */
    if(data_ptr->pdn_data.pco_ciphering_flag == FALSE)
    {
      ptr2esm_msg->pdn_connectivity_req.protocol_conf_opt = 
                                    data_ptr->pdn_data.protocol_config_options;
    }
  }
#ifdef FEATURE_LTE_REL10
  if(emm_get_3gpp_rel_ver_per_sub((sys_modem_as_id_e_type)sm_as_id) >= LTE_3GPP_REL10)
  {
    ptr2esm_msg->pdn_connectivity_req.low_priority_indicator = 
                                       data_ptr->pdn_data.low_priority_signaling;
  }
#endif
 } /* esm_build_pdn_connectivity_msg() */

/*==============================================================================

  FUNCTION:  esm_build_and_send_pkd_pdn_connectivity_req_msg

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of 
    what esm_build_and_send_pkd_pdn_connectivity_req_msg() does goes here.

    @detail
    Builds, packs and sends the PDN Connectivity message to CM, 
    for verifying this PKD data.
*/
/*============================================================================*/

void esm_build_and_send_pkd_pdn_connectivity_req_msg
(
  esm_bearer_resource_proc_T          *proc_ptr, 
  const esm_pdn_connectivity_req_T    *msg_ptr
)
{

  
  errno_enum_type                  e_retval;
  word                             offset = 0;
  
  emm_data_req_T    *out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));
  byte                             *ota_buf = NULL;  

  cm_pdn_connectivity_packed_request_ind_s_type *cm_pkd_pdn_connectivity_req =  
                                                (cm_pdn_connectivity_packed_request_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_pdn_connectivity_packed_request_ind_s_type));

  ASSERT(msg_ptr != NULL);
  ASSERT(proc_ptr != NULL);

  proc_ptr->esm_proc_state_ptr = &esm_proc_state_active;

  /* Store the sdf id as this will be the key if ESm received abort request 
     from CM/DS */
  proc_ptr->sdf_id = msg_ptr->esm_hdr.sdf_id;

  /*Reset the PKD_PDN_CONNECTIVITY_REQ structure*/
  

  
  cm_pkd_pdn_connectivity_req->esm_hdr.connection_id = ESM_INVALID_CON_ID;
  
  esm_build_pdn_connectivity_msg(proc_ptr, msg_ptr, &(out_msg_ptr->esm_out_msg));

  /* Save info which will be used later after UICC verification */
  if(proc_ptr->chk_on_attach_def_bc_setup_ptr() == FALSE)
  {
    proc_ptr->pco_ciphering_flag = msg_ptr->pdn_data.pco_ciphering_flag;
  }
  proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id = 
                                                      PDN_CONNECTIVITY_REQUEST;
  proc_ptr->pending_msg_ptr->esm_out_msg.pdn_connectivity_req.seq_num
                                                   = msg_ptr->pdn_data.seq_num;

  /*============================================================================
    Fill in the APN / PCO IE in the structure (this may not be filled up in
   the above function esm_build_pdn_connectivity_msg, as including these IEs 
   depends upon couple of reason. For more info, please refer to this function.
  =============================================================================*/
  /* Always pass APN and PCO from DS to NW */
  out_msg_ptr->esm_out_msg.pdn_connectivity_req.access_point_name = 
                                                 msg_ptr->pdn_data.apn_name;
  out_msg_ptr->esm_out_msg.pdn_connectivity_req.protocol_conf_opt = 
                                    msg_ptr->pdn_data.protocol_config_options;  

  /*=======================================================================
                Build PKD_PDN_CONNECTIVITY_REQ structure
  =======================================================================*/
  /*Reset the PKD_PDN_CONNECTIVITY_REQ structure*/
  
  /* Init the MSGR header*/
  msgr_init_hdr(&(cm_pkd_pdn_connectivity_req->msg_hdr),
                MSGR_NAS_ESM,MM_CM_PDN_CONNECTIVITY_PACKED_REQUEST_IND);
  /*Call the MSG_LIB function to pack this message and 
    hence send it to CM for validation*/
#ifdef FEATURE_MODEM_HEAP
  ota_buf = (byte*) modem_mem_calloc( 1, sizeof(lte_nas_outgoing_msg_type), MODEM_MEM_CLIENT_NAS);
  sm_check_for_null_ptr((void *) ota_buf);
#endif
  /*Encode the PDN_CONNECTIVITY_REQ structure*/
  encode_esm_msg((lte_nas_outgoing_msg_type *)&(out_msg_ptr->esm_out_msg), ota_buf, 
                          sizeof(lte_nas_outgoing_msg_type), &offset,TRUE);
  /*Fill in the structure for cm_pkd_pdn_connectivity_req*/
  cm_pkd_pdn_connectivity_req->esm_hdr.sdf_id = proc_ptr->sdf_id;
  cm_pkd_pdn_connectivity_req->pti = proc_ptr->pti;
  cm_pkd_pdn_connectivity_req->esm_hdr.lbi = proc_ptr->lbi;
  cm_pkd_pdn_connectivity_req->pkd_msg_len = offset/8;/*Offset will be in bits, convert it to bytes*/
  /*Adding this field for now, to unblock the AST testing*/
  cm_pkd_pdn_connectivity_req->apn_name = msg_ptr->pdn_data.apn_name;
  memscpy((void*)&cm_pkd_pdn_connectivity_req->pkd_msg[0],
          sizeof(cm_pkd_pdn_connectivity_req->pkd_msg),
          (void*)ota_buf,
          cm_pkd_pdn_connectivity_req->pkd_msg_len);
  /*Free the used buffer*/
#ifdef FEATURE_MODEM_HEAP       
  modem_mem_free(ota_buf, MODEM_MEM_CLIENT_NAS);
#endif

  /*=====================================================================
             Send PKD_PDN_CONNECTIVITY_REQ structure to CM
  =====================================================================*/
  e_retval = esm_msgr_send( (msgr_hdr_s*)cm_pkd_pdn_connectivity_req, sizeof(cm_pdn_connectivity_packed_request_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent MM_CM_PDN_CONNECTIVITY_PACKED_REQUEST_IND");

  esm_modem_mem_free(cm_pkd_pdn_connectivity_req);
  esm_modem_mem_free(out_msg_ptr);
}

/*==============================================================================

  FUNCTION:  esm_build_and_send_pdn_connectivity_req_msg

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_pdn_connectivity_req_msg() does goes here.

    @detail
    Builds the PDN Connectivity message and sends it to EMM in ESM_DATA_REQ.
*/
/*============================================================================*/
void esm_build_and_send_pdn_connectivity_req_msg
(
  esm_bearer_resource_proc_T          *proc_ptr, 
  const esm_pdn_connectivity_req_T    *msg_ptr  
)
{
  
  errno_enum_type                            e_retval;
  log_lte_nas_esm_outgoing_msg_event_type    event;
  
  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));

  ASSERT(proc_ptr != NULL);
  ASSERT(proc_ptr->pending_msg_ptr != NULL);
  ASSERT(msg_ptr != NULL);

  

  /* Store the sdf id as this will be the key if ESm received abort request 
     from CM/DS */
  proc_ptr->sdf_id = msg_ptr->esm_hdr.sdf_id;

    /* Init the MSGR header*/
  msgr_init_hdr(&out_msg_ptr->msg_hdr, MSGR_NAS_ESM, NAS_EMM_DATA_REQ);
  out_msg_ptr->emm_transaction_id = 
                           esm_get_trans_id_for_data_req(proc_ptr->instance_id,
                                                         LTE_ESM_BPM);

  esm_build_pdn_connectivity_msg(proc_ptr, msg_ptr, &(out_msg_ptr->esm_out_msg));

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: Sends NAS_EMM_DATA_REQ carrying PDN_CONNECTIVITY_REQUEST");
   

  /* Case A: Additional PDN connetion after attach
     The PDN_CONNECTIVITY_REQUEST may be retrasmitted because of timer expiry
     or transmission failure. Also PDN data from CM/DS message is not useful 
     anymore since there will not be ESM information procedure. So for case A, 
     save the OTA PDN_CONNECTIVITY_REQUEST message in procedure pending message
     structure directly. 

     Case B: Default PDN connetion during attach
     The PDN_CONNECTIVITY_REQUEST will not be retransmitted because of there is
     no timer get started and the attach failure indcation will not trigger the
     retransmission. Therefore procedure pending message structure can be used 
     to save the PDN data from CM/DS message which may be used to compose ESM 
     INFO RSP message in the possible ESM information procedure later. So for
     case B, modify the OTA PDN_CONNECTIVITY_REQUEST message with the PDN data 
     from CM/DS message and save it in procedure pending message structure.*/
  if(proc_ptr->chk_on_attach_def_bc_setup_ptr() == FALSE)
  {
    /* Case B: Save the PDN data from CM/DS message */
    out_msg_ptr->esm_out_msg.pdn_connectivity_req.access_point_name 
                                                  = msg_ptr->pdn_data.apn_name;

    out_msg_ptr->esm_out_msg.pdn_connectivity_req.protocol_conf_opt
                                   = msg_ptr->pdn_data.protocol_config_options;

    proc_ptr->pco_ciphering_flag = msg_ptr->pdn_data.pco_ciphering_flag;
  }
  /* Case A and B, save to the procedure pending message */
  //*(proc_ptr->pending_msg_ptr) = emm_cmd.emm_data_req;
  memscpy((void *)(proc_ptr->pending_msg_ptr), sizeof(emm_data_req_T), (void *)out_msg_ptr, sizeof(emm_data_req_T));

  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG, sizeof(event), 
                       (void*)&event);
  esm_modem_mem_free(out_msg_ptr);

} /* esm_build_and_send_pdn_connectivity_req_msg() */

/*==============================================================================

  FUNCTION:  esm_get_esm_header

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_get_esm_header() does goes here.

    @detail
    returns the header

*/
/*============================================================================*/
esm_header_T* esm_get_esm_header
(
  esm_cmd_type* cmd_ptr   /*!< REQUIRED short parameter description */
)
{
  switch(cmd_ptr->cmd.msg_hdr.id)
  {
    case NAS_ESM_DATA_IND:
      return &(cmd_ptr->cmd.esm_data_ind.esm_hdr);
    case NAS_ESM_FAILURE_IND:
      return &(cmd_ptr->cmd.esm_failure_ind.esm_hdr);
    case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
      return &(cmd_ptr->cmd.esm_sig_connection_released_ind.esm_hdr);
    case NAS_ESM_ACTIVE_EPS_IND:
      return &(cmd_ptr->cmd.esm_active_eps_ind.esm_hdr);
    case NAS_ESM_BEARER_RESOURCE_ALLOC_REQ:
      return &(cmd_ptr->cmd.esm_bearer_resource_alloc_req.esm_hdr);
    case NAS_ESM_PDN_CONNECTIVTY_REQ:
      return &(cmd_ptr->cmd.esm_pdn_connectivity_req.esm_hdr);
    case NAS_ESM_PDN_DISCONNECT_REQ:
      return &(cmd_ptr->cmd.esm_pdn_disconnect_req.esm_hdr);
    case NAS_ESM_BEARER_RESOURCE_ALLOC_ABORT_REQ:
      return &(cmd_ptr->cmd.esm_bearer_resource_alloc_abort_req.esm_hdr);
    case NAS_ESM_PDN_CONNECTIVITY_ABORT_REQ:
      return &(cmd_ptr->cmd.esm_pdn_connectivity_abort_req.esm_hdr);
    case NAS_ESM_DRB_REESTABLISH_REQ:
      return &(cmd_ptr->cmd.esm_drb_reestablish_req.esm_hdr);
    case NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT_CNF:
      return &(cmd_ptr->cmd.esm_act_default_bearer_context_accept.esm_hdr);
    case NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_REJ_IND:
      return &(cmd_ptr->cmd.esm_act_default_bearer_context_rej.esm_hdr);
    case NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_ACCEPT_CNF:
      return &(cmd_ptr->cmd.esm_act_dedicated_bearer_context_accept.esm_hdr);
    case NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_REJ_IND:
      return &(cmd_ptr->cmd.esm_act_dedicated_bearer_context_rej.esm_hdr);
    case NAS_ESM_MODIFY_BEARER_CONTEXT_REJ_IND:
      return &(cmd_ptr->cmd.esm_modify_bearer_context_rej.esm_hdr);
    case NAS_ESM_MODIFY_BEARER_CONTEXT_ACCEPT_CNF:
      return &(cmd_ptr->cmd.esm_modify_bearer_context_accept.esm_hdr);
    case NAS_ESM_DETACH_IND:
      return &(cmd_ptr->cmd.esm_modify_bearer_context_accept.esm_hdr);
    case NAS_ESM_BEARER_RESOURCE_MODIFICATION_REQ:
      return &cmd_ptr->cmd.esm_bearer_resource_modification_req.esm_hdr;
    case NAS_ESM_PDN_CONNECT_PACKED_CNF:
      return &cmd_ptr->cmd.esm_pdn_connectivity_pkd_cnf.esm_hdr;
    default:
      MSG_ERROR_DS_0(SM_SUB, "ESM received UNKNOWN message");
      return NULL;
  }
} /* esm_get_esm_header() */

/*==============================================================================

  FUNCTION:  esm_get_esm_hdr_from_esm_incoming_msg

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_get_esm_hdr_from_esm_incoming_msg() does goes here.

    @detail
    returns esm header from the OTA message
*/
/*============================================================================*/
lte_nas_esm_hdr_type* esm_get_esm_hdr_from_esm_incoming_msg
(
  lte_nas_esm_incoming_msg_type* in_msg   /*!< REQUIRED short parameter description */
)
{
  switch(in_msg->nas_hdr.msg_id)
  {
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
      return &(in_msg->default_bearer_context_req.lte_nas_esm_hdr);
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST:
      return &(in_msg->dedicated_bearer_context_req.lte_nas_esm_hdr);
    case MODIFY_EPS_BEARER_CONTEXT_REQUEST:
      return &(in_msg->modify_eps_bearer_ctxt_req.lte_nas_esm_hdr);
    case DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST:
      return &(in_msg->deactivate_eps_bearer_context_req.lte_nas_esm_hdr);
    case PDN_CONNECTIVITY_REJECT:
      return &(in_msg->pdn_connectivity_rej.lte_nas_esm_hdr);
    case BEARER_RESOURCE_ALLOCATION_REJECT:
      return &(in_msg->bearer_resource_alloc_rej.lte_nas_esm_hdr);
    case PDN_DISCONNECT_REJECT:
      return &(in_msg->pdn_disconnect_rej.lte_nas_esm_hdr);
    case BEARER_RESOURCE_MODIFICATION_REJECT:
      return &(in_msg->bearer_resource_modifi_rej.lte_nas_esm_hdr);
    case ESM_INFORMATION_REQUEST:
      return &(in_msg->esm_info_req.lte_nas_esm_hdr);
    case ESM_STATUS:
      return &(in_msg->mt_esm_status.lte_nas_esm_hdr);
    default:
      return &in_msg->esm_unknown_msg.lte_nas_esm_hdr;
  }

} /* esm_get_esm_hdr_from_esm_incoming_msg() */

/*==============================================================================

  FUNCTION:  esm_extract_OTA_msg

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_extract_OTA_msg() does goes here.

    @detail
    An OPTIONAL detailed description of esm_extract_OTA_msg() goes here.

*/
/*============================================================================*/
boolean esm_extract_OTA_msg
(
  struct esm_eps_bearer_context* ctxt_ptr, byte* msg_ptr
)
{
  MSG_LOW_DS_0(SM_SUB, "+==========================================================================+\n");
  return TRUE;

}/*esm_extract_OTA_msg*/


/*==============================================================================

  FUNCTION:  esm_copy_default_ctxt_data

==============================================================================*/
/*!
    @brief
    Copies the context data received from the NW into the context instance

    @detail 
*/
/*============================================================================*/
void esm_copy_default_ctxt_data
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const lte_nas_esm_act_default_bearer_context_req* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  byte mask = 0x08;

  //Already extracted, so, just copy
  ctxt_ptr->eps_bearer_id = msg_ptr->lte_nas_esm_hdr.bearer_id;
  ctxt_ptr->pti = msg_ptr->lte_nas_esm_hdr.pti;

  ctxt_ptr->sdf_qos = msg_ptr->sdf_qos;
  if(msg_ptr->pdn_address.valid)
  {
    ctxt_ptr->pdn_address = msg_ptr->pdn_address;
  }

  if(msg_ptr->access_point_name.valid)
  {
    ctxt_ptr->apn_name = msg_ptr->access_point_name;
  }
  if(msg_ptr->neg_qos.valid)
  {
    ctxt_ptr->neg_qos = msg_ptr->neg_qos;
  }
  if(msg_ptr->neg_llc_sapi.valid)
  {
    ctxt_ptr->neg_llc_sapi = msg_ptr->neg_llc_sapi;
  }
  if(msg_ptr->radio_priority.valid)
  {
    ctxt_ptr->radio_pri = msg_ptr->radio_priority;
  }
  if(msg_ptr->pack_flo_id.valid)
  {
    ctxt_ptr->packet_flow_id = msg_ptr->pack_flo_id;
  }
  if(msg_ptr->apn_ambr.valid == TRUE)
  {
    ctxt_ptr->apn_ambr = msg_ptr->apn_ambr;
  }
  if(msg_ptr->esm_cause.valid)
  {
    ctxt_ptr->esm_cause = msg_ptr->esm_cause;
  }
  if(msg_ptr->protocol_conf_opt.valid)
  {
    ctxt_ptr->config_options = msg_ptr->protocol_conf_opt;
    ctxt_ptr->bcm = sm_get_bcm_from_pco(&msg_ptr->protocol_conf_opt);
  }
  if(msg_ptr->ti.valid == TRUE)
  {
    ctxt_ptr->ti.valid = TRUE;
    /* As per the spec 24.007 isection 11.2.3.1.3 (Transaction identifier) the octet 3 will have the transaction id. 
    24.008 specifies the Linked TI format in section 10.5.6.7 i
    Flip the TI flag which should have been received as 0 and then add it to the transaction id. 
    This is what SM will use to talk to NW on UMTS side. 
    Here we are assuming that the network stil going to use 1st octet of Linked TI. 
    So just set the 4th bit to 1(Fliiped TI flag) to create the transaction id that SM should use to talk to the UMTS NW */
    if(msg_ptr->ti.ti_flag == FALSE)
    {
      ctxt_ptr->ti.connection_id = mask | msg_ptr->ti.val_of_transaction_id[0];
    }
    else
    {
      mask = 0x0;
      ctxt_ptr->ti.connection_id = mask | msg_ptr->ti.val_of_transaction_id[0];
    }
  }
  else
  {
    ctxt_ptr->ti.valid = FALSE;
  }
} /* esm_copy_default_ctxt_data() */

/*==============================================================================

  FUNCTION:  esm_get_instance_id_from_trans_id

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_get_instance_id_from_trans_id() does goes here.

    @detail
    Extract the instance id from the transaction id
*/
/*============================================================================*/
byte esm_get_instance_id_from_trans_id
(
  dword transaction_id   /*!< REQUIRED short parameter description */
)
{
  byte ret_val = 0;
  dword tmp_val = 0;

  /* Assigning to abyte is ok here as we are extracting a byte value of the instance id*/
  /*tmp_val = (transaction_id & 0xff000000);
  tmp_val = tmp_val >> 24;*/

  tmp_val = (transaction_id & ESM_INSTANCE_ID_MASK);
  tmp_val = tmp_val >> 8;

  ret_val = (byte)tmp_val;

  return ret_val;
} /* esm_get_instance_id_from_trans_id() */

/*==============================================================================

  FUNCTION:  esm_get_seq_num_from_trans_id

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_get_seq_num_from_trans_id() does goes here.

    @detail
    Extract the sequence number from the transaction id
*/
/*============================================================================*/
byte esm_get_seq_num_from_trans_id
(
  dword transaction_id   /*!< REQUIRED short parameter description */
)
{
  byte ret_val = 0;
  dword tmp_val = 0;

  /* Assigning to abyte is ok here as we are extracting a byte value of the instance id*/
  /*tmp_val = (transaction_id & 0xff000000);
  tmp_val = tmp_val >> 24;*/

  tmp_val = (transaction_id & ESM_SEQ_NUM_MASK);
  ret_val = (byte)tmp_val;

  return ret_val;
} /* esm_get_seq_num_from_trans_id() */


/*==============================================================================

  FUNCTION:  esm_get_module_from_trans_id

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_get_module_from_trans_id() does goes here.

    @detail
    Extract the module from the transaction id
*/
/*============================================================================*/
byte esm_get_module_from_trans_id
(
  dword transaction_id   /*!< REQUIRED short parameter description */
)
{
  byte ret_val = 0;
  dword tmp_val = 0;

  /* Assigning to abyte is ok here as we are extracting a byte value of the instance id*/
  /*tmp_val = (transaction_id & 0xff000000);
  tmp_val = tmp_val >> 24;*/

  tmp_val = (transaction_id & ESM_MODULE_MASK);
  tmp_val = tmp_val >> 16;
  ret_val = (byte)tmp_val;

  return ret_val;
} /* esm_get_seq_num_from_trans_id() */


/*==============================================================================

  FUNCTION:  esm_build_and_send_cm_drb_setup_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_cm_drb_setup_ind() does goes here.

    @detail
    Builds and sends DRB setup indication to the CM

*/
/*============================================================================*/
void esm_build_and_send_cm_drb_setup_ind
(
  const esm_eps_bearer_context_T* ctxt_ptr   /*!< REQUIRED short parameter description */
)
{
  

  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;

  cm_drb_setup_ind_s_type* cm_msg_ptr = (cm_drb_setup_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_drb_setup_ind_s_type));

  msgr_init_hdr(&(cm_msg_ptr->msg_hdr),MSGR_NAS_ESM,MM_CM_DRB_SETUP_IND);

  cm_msg_ptr->esm_hdr.connection_id = ctxt_ptr->connection_id;
  cm_msg_ptr->eps_bearer_id = ctxt_ptr->eps_bearer_id;
  cm_msg_ptr->rb_id   = ctxt_ptr->rb_id;

  e_retval = esm_msgr_send( (msgr_hdr_s*)cm_msg_ptr, sizeof(cm_drb_setup_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_2(SM_SUB, "ESM: ESM sent MM_CM_DRB_SETUP_IND with Bearer ID=%d and Connection ID=%d  ",
                cm_msg_ptr->eps_bearer_id, cm_msg_ptr->esm_hdr.connection_id);
  event.esm_out_msg = (byte)MM_CM_DRB_SETUP_IND;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);  
 
  esm_modem_mem_free(cm_msg_ptr);

} /* esm_build_and_send_cm_drb_setup_ind() */


/*==============================================================================

  FUNCTION:  esm_build_and_send_pdn_connectivity_reject_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_pdn_connectivity_reject_ind() does goes here.

    @detail
    An OPTIONAL detailed description of esm_build_and_send_pdn_connectivity_reject_ind() goes here.

*/
/*============================================================================*/
void esm_build_and_send_pdn_connectivity_reject_ind
(
  //lte_nas_pdn_connectivity_rej* msg_ptr   /*!< REQUIRED short parameter description */
  esm_bearer_resource_proc_T *proc_ptr,
  word sdf_id,
  const lte_nas_pdn_connectivity_rej  *msg_ptr,
  lte_nas_local_cause_enum_T           local_cause,
  emm_failure_type                    *emm_failure_ptr
)
{
  
  errno_enum_type                          e_retval;
  log_lte_nas_esm_outgoing_msg_event_type  event;
  cm_pdn_connectivity_reject_ind_s_type   *cm_pdn_connectivity_rej = 
                                          (cm_pdn_connectivity_reject_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_pdn_connectivity_reject_ind_s_type));



  /* Fill the msgr header*/
  msgr_init_hdr(&cm_pdn_connectivity_rej->msg_hdr,
                MSGR_NAS_ESM,
                MM_CM_PDN_CONNECTIVITY_REJECT_IND);

  /* Fill the ESM header*/
  cm_pdn_connectivity_rej->esm_hdr.connection_id = ESM_INVALID_CON_ID;
  cm_pdn_connectivity_rej->esm_hdr.sdf_id = sdf_id;
  cm_pdn_connectivity_rej->esm_hdr.lbi = ESM_INVALID_BEARER_ID;
#ifdef FEATURE_LTE_REL9
 if(proc_ptr != NULL)
  {
     if( proc_ptr->pending_msg_ptr->esm_out_msg.pdn_connectivity_req.req == ESM_EMERGENCY )
	 {
	  cm_pdn_connectivity_rej->emc_type = TRUE;
	 }
	 else
	 {
	  cm_pdn_connectivity_rej->emc_type = FALSE;
	 }
  }
#endif

  if((msg_ptr != NULL) && (msg_ptr->lte_nas_hdr.valid == FALSE))
  {
    esm_send_esm_status(&msg_ptr->lte_nas_esm_hdr, 
                         msg_ptr->lte_nas_hdr.esm_decode_fail_reason);
  }

  if((msg_ptr != NULL) && (msg_ptr->lte_nas_hdr.valid == TRUE)) 
  {
    cm_pdn_connectivity_rej->esm_cause = msg_ptr->esm_cause;
    cm_pdn_connectivity_rej->esm_local_cause.valid = FALSE;

    /* This is whatever MSG_LIB says*/
    cm_pdn_connectivity_rej->protocol_config_options = msg_ptr->protocol_conf_opt;
#ifdef FEATURE_LTE_REL10
    cm_pdn_connectivity_rej->backoff_timer = msg_ptr->backoff_timer;

    if(proc_ptr != NULL)
    {
      /* Pass APN name and IP type from stored PDN Connectivity Request msg to DS*/
      cm_pdn_connectivity_rej->pdn_reject_ind.pdn_type = 
        proc_ptr->pending_msg_ptr->esm_out_msg.pdn_connectivity_req.pdn_type;
      cm_pdn_connectivity_rej->pdn_reject_ind.access_point_name = 
        proc_ptr->pending_msg_ptr->esm_out_msg.pdn_connectivity_req.access_point_name;
    }
#endif
  }
  else
  {
    cm_pdn_connectivity_rej->esm_cause.valid = FALSE;
    cm_pdn_connectivity_rej->esm_local_cause.valid = TRUE;
    cm_pdn_connectivity_rej->esm_local_cause.local_cause = local_cause;
    cm_pdn_connectivity_rej->protocol_config_options.valid = FALSE;
  }

  if(emm_failure_ptr != NULL)
  {
    cm_pdn_connectivity_rej->emm_failure_cause = *emm_failure_ptr;
  }
  else
  {
    cm_pdn_connectivity_rej->emm_failure_cause.cause_type = LTE_NAS_IRAT_NONE;
  }

  e_retval = esm_msgr_send( (msgr_hdr_s *)cm_pdn_connectivity_rej, sizeof(cm_pdn_connectivity_reject_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent MM_CM_PDN_CONNECTIVITY_REJECT_IND");
  event.esm_out_msg = (byte)MM_CM_PDN_CONNECTIVITY_REJECT_IND;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(cm_pdn_connectivity_rej);

} /* esm_build_and_send_pdn_connectivity_reject_ind() */

/*==============================================================================

  FUNCTION:  esm_build_and_send_service_request

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_service_request() does goes here.

    @detail
    An OPTIONAL detailed description of esm_build_and_send_service_request() goes here.

*/
/*============================================================================*/
void esm_build_and_send_service_request
( 
  struct esm_eps_bearer_context* ctxt_ptr,
  sys_lte_reestab_calltype_e call_type
)
{
  
  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;

  
  emm_service_req_T* service_req = (emm_service_req_T *)esm_modem_mem_alloc(sizeof(emm_service_req_T));

  msgr_init_hdr(&(service_req->msg_hdr),MSGR_NAS_ESM,NAS_EMM_SERVICE_REQ);

  service_req->trans_id = esm_get_trans_id_for_data_req(ctxt_ptr->connection_id,LTE_ESM_BCM);

#ifdef FEATURE_LTE_REL9 
 service_req->emc_bc = ctxt_ptr->emc_bc;
#endif

  service_req->call_type = call_type;
  e_retval = esm_msgr_send( (msgr_hdr_s*)service_req, sizeof(emm_service_req_T) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_1(SM_SUB, "ESM: ESM sent NAS_EMM_SERVICE_REQ and Call type is %d", call_type);
  event.esm_out_msg = (byte)NAS_EMM_SERVICE_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(service_req);

}

/*==============================================================================

  FUNCTION:  esm_build_and_send_cm_drb_reestablish_reject_ind

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_cm_drb_reestablish_reject_ind() does goes here.

    @detail
    An OPTIONAL detailed description of esm_build_and_send_cm_drb_reestablish_reject_ind() goes here.

*/
/*============================================================================*/
void esm_build_and_send_cm_drb_reestablish_reject_ind
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_failure_ind_T* msg_ptr,   /*!< REQUIRED short parameter description */
  byte con_id   /*!< Ideally to be used only for case where existing con_id does not exist with ESM */
)
{
 
  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;

  cm_drb_reestablish_reject_ind_s_type* cm_msg_ptr = (cm_drb_reestablish_reject_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_drb_reestablish_reject_ind_s_type));

  msgr_init_hdr(&(cm_msg_ptr->msg_hdr),MSGR_NAS_ESM,MM_CM_DRB_REESTABLISH_REJECT_IND);

  if (ctxt_ptr != NULL)
  {
    cm_msg_ptr->esm_hdr.connection_id = ctxt_ptr->connection_id;
    cm_msg_ptr->eps_bearer_id = ctxt_ptr->eps_bearer_id;
  }
  else
  {
    cm_msg_ptr->esm_hdr.connection_id = con_id;
    cm_msg_ptr->eps_bearer_id = ESM_INVALID_BEARER_ID;
  }

  cm_msg_ptr->esm_local_cause.valid = TRUE;
  if(msg_ptr->emm_ota_cause.valid)
  {
    cm_msg_ptr->esm_local_cause.local_cause = msg_ptr->emm_ota_cause.local_cause;
  }
  else
  {
    cm_msg_ptr->esm_local_cause.local_cause = msg_ptr->emm_cause.local_cause;
  }

  cm_msg_ptr->emm_failure_cause = msg_ptr->emm_failure_cause;

  e_retval = esm_msgr_send( (msgr_hdr_s*)cm_msg_ptr, sizeof(cm_drb_reestablish_reject_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent MM_CM_DRB_REESTABLISH_REJECT_IND");
  event.esm_out_msg = (byte)MM_CM_DRB_REESTABLISH_REJECT_IND;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);

  esm_modem_mem_free(cm_msg_ptr);
}

/*==============================================================================

  FUNCTION: esm_validate_activate_default_bearer_ctxt_req 

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_validate_activate_default_bearer_ctxt_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_validate_activate_default_bearer_ctxt_req() goes here.

*/
/*============================================================================*/

static boolean esm_validate_activate_default_bearer_ctxt_req
(
  lte_nas_esm_act_default_bearer_context_req   *def_bearer_req_ptr
)
{
  esm_act_default_bearer_context_rej_T* out_msg;
  out_msg = (esm_act_default_bearer_context_rej_T *)esm_modem_mem_alloc(sizeof(esm_act_default_bearer_context_rej_T));
  memset((void *)out_msg,0,sizeof(sizeof(esm_act_default_bearer_context_rej_T)));

  if(def_bearer_req_ptr->lte_nas_esm_hdr.bearer_id >= MIN_NSAPI_NUM)
  {
    if(def_bearer_req_ptr->lte_nas_hdr.valid == TRUE)
    {
      byte conn_id = (byte)(((def_bearer_req_ptr->ti.ti_flag == FALSE) ? 0x08 : 0x00) | 
	                          def_bearer_req_ptr->ti.val_of_transaction_id[0]);

      if((def_bearer_req_ptr->ti.valid == TRUE) && (sm_check_if_no_conn_exists(conn_id) == FALSE))
      {
        // Check if there's already an existing bearer/ PDP with same TID.
        // If yes, then let's mark the TID and QoS as invalid, but return TRUE.
        // This will ensure SM PDP contexts are not updated, but PS call will be unhindered in LTE   
        def_bearer_req_ptr->ti.valid = FALSE;
        def_bearer_req_ptr->neg_qos.valid = FALSE;
        MSG_HIGH_DS_1(SM_SUB, "ESM: PDP context for Connection ID %d exists, not updating SM context any further, invalidating TI/ QoS",
                      conn_id);
      }
      esm_modem_mem_free(out_msg);
      return TRUE;
    }
    else
    {
      out_msg->esm_cause.esm_cause = 
                        def_bearer_req_ptr->lte_nas_hdr.esm_decode_fail_reason;
      out_msg->esm_cause.valid = TRUE;
      MSG_ERROR_DS_1(SM_SUB, "ESM: Invalid Act Default BC Req msg w/ cause - %d",
                     def_bearer_req_ptr->lte_nas_hdr.esm_decode_fail_reason);
    }
  }
  else
  {
    out_msg->esm_cause.esm_cause = NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT;
    out_msg->esm_cause.valid = TRUE;
    MSG_ERROR_DS_1(SM_SUB, "ESM: Unknown EPS BC %d in Act Default BC Req msg",
                   def_bearer_req_ptr->lte_nas_esm_hdr.bearer_id);
  }
  esm_build_and_send_autonomous_act_def_ctxt_reject(def_bearer_req_ptr,
                                                    out_msg);
  esm_modem_mem_free(out_msg);
  return FALSE;
}

/*==============================================================================

  FUNCTION: esm_link_bearer_identity_and_update_pdp_params 

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_link_bearer_identity_and_update_pdp_params() does goes here.

    @detail
    An OPTIONAL detailed description of esm_link_bearer_identity_and_update_pdp_params() goes here.

*/
/*============================================================================*/
static boolean esm_link_bearer_identity_and_update_pdp_params
(
  lte_nas_esm_act_dedicated_bearer_context_req* dedicated_bearer_req_ptr,
  const esm_bcm_T* mgr_ptr
)
{
  byte index = 0;

  for(index=0; index < ESM_MAX_EPS_BEARERS; index++)
  {
    if((mgr_ptr->esm_bc[index] != NULL) && (mgr_ptr->esm_bc[index]->eps_bearer_state_ptr->state() != ESM_BC_INACTIVE) && 
        (mgr_ptr->esm_bc[index]->connection_id != ESM_INVALID_CON_ID) )
    {
      if(dedicated_bearer_req_ptr->linked_eps_bearer_identity == mgr_ptr->esm_bc[index]->eps_bearer_id)
      {
        MSG_HIGH_DS_3(SM_SUB, "ESM: Default bearer =%d for the dedicated bearer =%d with linked bearer identity =%d  is found",
                      mgr_ptr->esm_bc[index]->eps_bearer_id,
                      dedicated_bearer_req_ptr->lte_nas_esm_hdr.bearer_id,
                      dedicated_bearer_req_ptr->linked_eps_bearer_identity);
        if((dedicated_bearer_req_ptr->ti.valid == TRUE) && (mgr_ptr->esm_bc[index]->ti.valid == FALSE))
        {
          MSG_ERROR_DS_0(SM_SUB, "ESM: dedicated bearer has valid TI, while default_bearer has invalid QoS/TI,shall invalidate the TI in dedicated bearer");
          dedicated_bearer_req_ptr->ti.valid = FALSE;
        }
        if((dedicated_bearer_req_ptr->neg_qos.valid == TRUE) && (mgr_ptr->esm_bc[index]->neg_qos.valid == FALSE))
        {
          ERR("ESM: dedicated bearer has valid QoS, while default_bearer has invalid QoS,shall invalidate the QoS in dedicated bearer",0,0,0);
          dedicated_bearer_req_ptr->neg_qos.valid = FALSE;
        }
        return TRUE;
      }
    }
  }
  MSG_ERROR_DS_2(SM_SUB, "ESM: Default bearer for the dedicated bearer =%d with linked bearer identity =%d  is NOT found",
                 dedicated_bearer_req_ptr->lte_nas_esm_hdr.bearer_id,
                 dedicated_bearer_req_ptr->linked_eps_bearer_identity);

  return FALSE;
} /*esm_link_bearer_identity_and_update_pdp_params*/

/*==============================================================================

  FUNCTION: esm_validate_activate_dedicated_bearer_ctxt_req 

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_validate_activate_dedicated_bearer_ctxt_req() does goes here.

    @detail
    An OPTIONAL detailed description of esm_validate_activate_dedicated_bearer_ctxt_req() goes here.

*/
/*============================================================================*/
static boolean esm_validate_activate_dedicated_bearer_ctxt_req
(
  lte_nas_esm_act_dedicated_bearer_context_req* dedicated_bearer_req_ptr,
  const esm_bcm_T* mgr_ptr
)
{
  esm_act_dedicated_bearer_context_rej_T* out_msg;
  out_msg = (esm_act_dedicated_bearer_context_rej_T *)esm_modem_mem_alloc(sizeof(esm_act_dedicated_bearer_context_rej_T));
  memset((void *)out_msg,0,sizeof(sizeof(esm_act_dedicated_bearer_context_rej_T)));

  if((dedicated_bearer_req_ptr->lte_nas_esm_hdr.bearer_id >= MIN_NSAPI_NUM) &&
     (esm_link_bearer_identity_and_update_pdp_params(dedicated_bearer_req_ptr,
                                                     mgr_ptr) == TRUE))
  {
    if(dedicated_bearer_req_ptr->lte_nas_hdr.valid == TRUE)
    {
       byte conn_id = (byte)(((dedicated_bearer_req_ptr->ti.ti_flag == FALSE) ? 0x08 : 0x00) | 
	                          dedicated_bearer_req_ptr->ti.val_of_transaction_id[0]);

      
       if((dedicated_bearer_req_ptr->ti.valid == TRUE) && (sm_check_if_no_conn_exists(conn_id) == FALSE))
       {
         // Check if there's already an existing bearer/ PDP with same TID.
         // If yes, then let's mark the TID and QoS as invalid, but return TRUE.
         // This will ensure SM PDP contexts are not updated, but PS call will be unhindered in LTE
         dedicated_bearer_req_ptr->ti.valid = FALSE;
         dedicated_bearer_req_ptr->neg_qos.valid = FALSE;
         MSG_HIGH_DS_1(SM_SUB, "ESM: PDP context for Connection ID %d exists, not updating SM context any further, invalidating TI/ QoS",
                  conn_id);
       }
       esm_modem_mem_free(out_msg);
       return TRUE;
    }
    else
    {
      out_msg->esm_cause.esm_cause = NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT;
      out_msg->esm_cause.valid = TRUE;
      MSG_ERROR_DS_1(SM_SUB, "ESM: Invalid Ded BC Req msg w/ cause - %d",
                     dedicated_bearer_req_ptr->lte_nas_hdr.esm_decode_fail_reason);
    }
  }
  else
  {
    out_msg->esm_cause.esm_cause = NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT;
    out_msg->esm_cause.valid = TRUE;
    MSG_ERROR_DS_2(SM_SUB, "ESM: Unknown EPS BI %d or LBI d% in Act Ded BC Req msg",
                   dedicated_bearer_req_ptr->lte_nas_esm_hdr.bearer_id,
                   dedicated_bearer_req_ptr->linked_eps_bearer_identity);
  }

  esm_build_and_send_autonomous_act_dedicated_ctxt_reject(dedicated_bearer_req_ptr,
                                                          out_msg);
  esm_modem_mem_free(out_msg);
  return FALSE;
}

/*==============================================================================

  FUNCTION: esm_validate_modify_bearer_ctxt_req

==============================================================================*/
/*!
    @brief
    This function validates MODIFY EPS BEARER CONTEXT REQUEST message


    @detail
*/
/*============================================================================*/
static boolean esm_validate_modify_bearer_ctxt_req
(
  const lte_nas_esm_modify_eps_bearer_ctxt_req  *modify_bearer_req_ptr,
  struct esm_eps_bearer_context                 *ctxt_ptr
)
{
  esm_modify_bearer_context_rej_T*   out_msg;
  out_msg = (esm_modify_bearer_context_rej_T *)esm_modem_mem_alloc(sizeof(esm_modify_bearer_context_rej_T));
  memset((void *)out_msg,0,sizeof(sizeof(esm_modify_bearer_context_rej_T)));

  if(ctxt_ptr != NULL)
  {
    if(modify_bearer_req_ptr->lte_nas_hdr.valid == TRUE)
    {
      esm_modem_mem_free(out_msg);
      return TRUE;
    }
    else
    {
      MSG_ERROR_DS_1(SM_SUB, "ESM: Invalid Modify BC Req msg w/ cause - %d",
                     modify_bearer_req_ptr->lte_nas_hdr.esm_decode_fail_reason);
      out_msg->esm_cause.esm_cause = 
                       modify_bearer_req_ptr->lte_nas_hdr.esm_decode_fail_reason;
      out_msg->esm_cause.valid = TRUE;
    }
  }
  else
  {
    MSG_HIGH_DS_1(SM_SUB, "ESM: UNKNOWN bearer id %u in MODIFY BC REQ",
                  modify_bearer_req_ptr->lte_nas_esm_hdr.bearer_id);
    out_msg->esm_cause.esm_cause = NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT;
    out_msg->esm_cause.valid = TRUE;
  }

  esm_build_and_send_autonomous_modify_ctxt_reject(modify_bearer_req_ptr,
                                                   out_msg);
  esm_modem_mem_free(out_msg);
  return FALSE;

} /* esm_validate_modify_bearer_ctxt_req*/

/*==============================================================================

  FUNCTION: esm_validate_OTA_message 

==============================================================================*/
/*!
    @brief
    This function validates the given OTA message

    @detail
*/
/*============================================================================*/
boolean esm_validate_OTA_message
(
  lte_nas_esm_incoming_msg_type         *msg_ptr, 
  esm_bcm_T                             *mgr_ptr,
  word                                   sdf_id,
  byte                                   lbi,
  lte_nas_message_id                     trigger_msg_id,
  struct esm_eps_bearer_context         *ctxt_ptr,
  lte_nas_esm_hdr_type                  *esm_hdr_ptr
)
{
  boolean                       result = FALSE;
  lte_nas_local_cause_T         local_cause;

  ASSERT(msg_ptr != NULL);
  ASSERT(mgr_ptr != NULL);

  switch (msg_ptr->nas_hdr.msg_id)
  {
    /* For this case, bearer ID is considered valid if it falls in the right 
       range of [5, 15]. */
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
      result = esm_validate_activate_default_bearer_ctxt_req(
                                         &msg_ptr->default_bearer_context_req);
      /* Since the default bearer context failed, no point processing any 
      messages received after this. Examples of messages that can be received 
      are NAS_ESM_ACTIVE_EPS_IND and ACTIVATE_DEDICATED_BEARER_CONTEXT_REQUESTs
      Since we switch off this flag the handle_emm_message function will try to 
      process these messages even when the default bearer context setup failed. 
      For the NAS_ESM_ACTIVE_EPS_IND processing since the contexts are INACTIVE, 
      they will throw away the EPS_IND. For the dedicated bearer processing it 
      should fail as lbi check will fail.But, for this case we shouldn't send
      a DEDICATED BEARER REJECT to the N/W as the on attach default bearer 
      doesn't exist */
      if((result == FALSE) &&
         (mgr_ptr->on_attach_def_bearer_pending == 
                                          ESM_BC_ON_ATTACH_DEF_BEARER_PENDING))
      {
        /* Only reset the flag during attch */
        mgr_ptr->on_attach_def_bearer_pending = 
                                            ESM_BC_ON_ATTACH_DEF_BEARER_FAILED;
      }
      break;
    /* For this case, bearer ID is considered valid if it falls in the right
       range of [5, 15] and the linked bearer ID belongs to an existing default 
       bearer context */
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST:
      result = esm_validate_activate_dedicated_bearer_ctxt_req(
                              &msg_ptr->dedicated_bearer_context_req, mgr_ptr);
      /* If PTI in received msg is assigned value, a procedure had been active.
         If validation fails, need to send proc reject indication to CM */
      if((result == FALSE) && (esm_hdr_ptr->pti != ESM_INVALID_PTI))
      {
        switch(trigger_msg_id) 
        {
          case BEARER_RESOURCE_ALLOCATION_REQUEST:
            esm_build_and_send_cm_bearer_resource_allocation_reject(sdf_id, 
                                            NULL, ESM_BAD_OTA_MESSAGE, lbi);
            break;
          case BEARER_RESOURCE_MODIFICATION_REQUEST:
            esm_build_and_send_cm_bearer_resrc_mod_reject(sdf_id, 
                                                          NULL, 
                                                          ESM_BAD_OTA_MESSAGE, 
                                                          lbi);
            break;
          default:
            MSG_ERROR_DS_1(SM_SUB, "ESM: Invalid trigger msg %u for ACT DEDICATED BC REQ",
                           trigger_msg_id);
            break;
        }
      }
      break;
    /* For this case, bearer ID is considered valid if it belongs to an existing
       bearer context */
    case DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST:
      if((ctxt_ptr == NULL) || (msg_ptr->nas_hdr.valid == FALSE))
      {
        /* 7.3.2 UE proc d: Bearer ID is not existing, send DEACT BC ACCEPT */
        esm_build_and_send_deactivate_bearer_context_accept(
                                                 esm_hdr_ptr->bearer_id, NULL);
        /* If PTI in rcved msg is not invalid, a procedure had been active */
        if(esm_hdr_ptr->pti != ESM_INVALID_PTI)
        {
          switch(trigger_msg_id)
          {
            case PDN_DISCONNECT_REQUEST:
              /* Locally deactivate */
              local_cause.valid = FALSE;
              mgr_ptr->deact_ctxts_ptr(local_cause,
                    msg_ptr->deactivate_eps_bearer_context_req.esm_cause, lbi);
			  ctxt_ptr = NULL;
              break;
            case BEARER_RESOURCE_MODIFICATION_REQUEST:
              /* Send reject indication to CM */
              esm_build_and_send_cm_bearer_resrc_mod_reject(sdf_id, 
                                                            NULL,
                                                            ESM_BAD_OTA_MESSAGE, 
                                                            lbi);
              break;
            default:
              MSG_ERROR_DS_1(SM_SUB, "ESM: Invalid trigger msg %u for DEACT BC REQ",
                             trigger_msg_id);
              break;
          }
        }            
      }
      else
      {
        /* Bearer ID is valid since it matchs to an existing bearer context */
        result = TRUE;
      }
      break;
    /* For this case, bearer ID is considered valid if it belongs to an existing
       bearer context */
    case MODIFY_EPS_BEARER_CONTEXT_REQUEST:
      result = esm_validate_modify_bearer_ctxt_req(
                                          &msg_ptr->modify_eps_bearer_ctxt_req,
                                           ctxt_ptr);
      /* If PTI in rcved msg is not invalid, a procedure had been active
         If validation fails, need to send proc reject indication to CM */
      if((result == FALSE) && (esm_hdr_ptr->pti != ESM_INVALID_PTI))
      {
        switch(trigger_msg_id)
        {
          case BEARER_RESOURCE_ALLOCATION_REQUEST:
            esm_build_and_send_cm_bearer_resource_allocation_reject(sdf_id, 
                                             NULL, ESM_BAD_OTA_MESSAGE,lbi);
            break;
          case BEARER_RESOURCE_MODIFICATION_REQUEST:
            esm_build_and_send_cm_bearer_resrc_mod_reject(sdf_id, 
                                                          NULL,
                                                          ESM_BAD_OTA_MESSAGE,
                                                          lbi);
            break;
          default:
            MSG_ERROR_DS_1(SM_SUB, "ESM: Invalid trigger msg %u for MODIFY BC REQ %u",
                           trigger_msg_id);
            break;
        }
      }
      break;
    case ESM_STATUS:
      if(ctxt_ptr != NULL)
      {
        result = TRUE;
      }
      break;
    case PDN_CONNECTIVITY_REJECT:
    case PDN_DISCONNECT_REJECT:
    case BEARER_RESOURCE_ALLOCATION_REJECT:
    case BEARER_RESOURCE_MODIFICATION_REJECT:
    case ESM_INFORMATION_REQUEST:
      /* These messages should be only handled by BPM. */
      MSG_HIGH_DS_1(SM_SUB, "ESM: Unexpected OTA msg %u to be handled by BCM",
                    msg_ptr->nas_hdr.msg_id);
      break;
    case ESM_NOTIFICATION: 
      /*Refer to 9.9.4.7A of 24.301 where spec mandates to use value 1 only, and ignore all other reserved values*/
      if(msg_ptr->nas_hdr.valid == FALSE)
      {
        MSG_HIGH_DS_1(SM_SUB, "ESM:ESM notification ignored as valid field is %d",
                      msg_ptr->nas_hdr.valid);
      }
      else
      {
        result = TRUE;
      }
      break;
    default:
    /* Validate the bearer id in unknown messages. Message is considered
       invalid if it includes an unassigned or reserved EPS bearer identity
       value or a value that does not match an existing one. */
    if(ctxt_ptr != NULL)
    {
      result = TRUE;
    }
    else
    {
      /* 24.301 7.3.2 UE case j */
      MSG_HIGH_DS_1(SM_SUB, "ESM: Ignoring unknown OTA msg - %u",
                    msg_ptr->nas_hdr.msg_id);
#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */
    }
    break;
}

return result;

} /* esm_validate_OTA_message */

void ESM_PRINT_ESM_DATA_IND(esm_data_ind_T* data_ptr)
{
lte_nas_esm_act_default_bearer_context_req* msg_ptr = NULL;
byte index = 0;

switch(data_ptr->esm_incoming_msg.nas_hdr.msg_id)
{
  case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
  msg_ptr = &data_ptr->esm_incoming_msg.default_bearer_context_req;
  MSG_HIGH_DS_3(SM_SUB, "ESM: Bearer Id=%d, pti = %d Sdf Qos length=%d",
               msg_ptr->lte_nas_esm_hdr.bearer_id, msg_ptr->lte_nas_esm_hdr.pti,msg_ptr->sdf_qos.length);
  MSG_HIGH_DS_8(SM_SUB, "ESM: QCI = %d, MaxBitRate-UL = %d MaxBitRate-DL = %d GuaranteedbitRate-UL = %d,  GuaranteedbitRate-DL= %d MaxBitRate-UL-EX = %d, MaxBitRate-DL-EX = %d,GuaranteedbitRate-UL-EX= %d",
               msg_ptr->sdf_qos.qci,
               msg_ptr->sdf_qos.max_bit_rate_ul,
               msg_ptr->sdf_qos.max_bit_rate_dl,
               msg_ptr->sdf_qos.gbr_ul,
               msg_ptr->sdf_qos.gbr_dl,
               msg_ptr->sdf_qos.max_bit_rate_ul_ex,
               msg_ptr->sdf_qos.max_bit_rate_dl_ex,
               msg_ptr->sdf_qos.gbr_ul_ex);

  if(msg_ptr->access_point_name.valid)
  {
    MSG_HIGH_DS_1(SM_SUB, "ESM: APN address length = %d APN adBCMs = ",msg_ptr->access_point_name.apn_addr_len);
    /*memcpy(tmp_arr,&msg_ptr->access_point_name.apn_address[0],msg_ptr->access_point_name.apn_addr_len);*/

    for(index = 0; index < msg_ptr->access_point_name.apn_addr_len;index++)
    {
      MSG_HIGH_DS_1(SM_SUB, "ESM: %d",(unsigned)(msg_ptr->access_point_name.address[index]));
    }
  }
  if(msg_ptr->pdn_address.valid)
  {
    MSG_HIGH_DS_2(SM_SUB, "ESM: PDN address length = %d,PDN type value=%d PDN address = ",
                 msg_ptr->pdn_address.pdn_addr_len,
                 msg_ptr->pdn_address.pdn_type_val);

  /* memcpy(tmp_arr,&msg_ptr->pdn_address.address[0],msg_ptr->pdn_address.pdn_addr_len);*/
    for(index = 0; index < msg_ptr->pdn_address.pdn_addr_len ;index++)
    {
      MSG_HIGH_DS_1(SM_SUB, "ESM: %d",(unsigned)(msg_ptr->pdn_address.address[index]));
    }

  }
  break;
case PDN_CONNECTIVITY_REJECT:
  break;
default:
  MSG_ERROR_DS_0(SM_SUB, "ESM:UNKNWN OTA message received");
  break;
}
}

void ESM_PRINT_ESM_FAILURE_IND(esm_failure_ind_T* msg_ptr)
{
}
void ESM_PRINT_SIG_CONNECTION_RELEASED_IND(esm_sig_connection_released_ind_T* msg_ptr)
{
}
void ESM_PRINT_ACTIVE_EPS_IND(esm_active_eps_ind_T* msg_ptr)
{
}

void ESM_PRINT_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT_CNF
(
esm_act_default_bearer_context_accept_T* msg_ptr
)
{
}

void ESM_PRINT_ACT_DEFAULT_BEARER_CONTEXT_REJ_IND(esm_act_default_bearer_context_rej_T* msg_ptr)
{
}
void ESM_PRINT_DETACH_IND(esm_detach_ind_T* msg_ptr)
{
}


void ESM_PRINT_MSG
(
esm_cmd_type* cmd_ptr   
)
{
switch(cmd_ptr->cmd.msg_hdr.id)
{
  case NAS_ESM_DATA_IND:
    ESM_PRINT_ESM_DATA_IND(&cmd_ptr->cmd.esm_data_ind);
    break;
    case NAS_ESM_FAILURE_IND:
      ESM_PRINT_ESM_FAILURE_IND(&cmd_ptr->cmd.esm_failure_ind);
      break;
    case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
      ESM_PRINT_SIG_CONNECTION_RELEASED_IND(&cmd_ptr->cmd.esm_sig_connection_released_ind);
      break;
    case NAS_ESM_ACTIVE_EPS_IND:
      ESM_PRINT_ACTIVE_EPS_IND(&cmd_ptr->cmd.esm_active_eps_ind);
      break;
    case NAS_ESM_BEARER_RESOURCE_ALLOC_REQ:
    case NAS_ESM_PDN_CONNECTIVTY_REQ:
    case NAS_ESM_PDN_DISCONNECT_REQ:
    case NAS_ESM_BEARER_RESOURCE_ALLOC_ABORT_REQ:
    case NAS_ESM_PDN_CONNECTIVITY_ABORT_REQ:
    case NAS_ESM_DRB_REESTABLISH_REQ:
    case NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT_CNF:
      ESM_PRINT_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT_CNF(&cmd_ptr->cmd.esm_act_default_bearer_context_accept);
      break;
    case NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_REJ_IND:
      ESM_PRINT_ACT_DEFAULT_BEARER_CONTEXT_REJ_IND(&cmd_ptr->cmd.esm_act_default_bearer_context_rej);
      break;
    case NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_ACCEPT_CNF:
    case NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_REJ_IND:
    case NAS_ESM_MODIFY_BEARER_CONTEXT_REJ_IND:
    case NAS_ESM_MODIFY_BEARER_CONTEXT_ACCEPT_CNF:
    case NAS_ESM_DETACH_IND:
      ESM_PRINT_DETACH_IND(&cmd_ptr->cmd.esm_detach_ind);
      break;
    default:
      MSG_ERROR_DS_0(SM_SUB, "ESM: ESM received UNKNOWN message");
      break ;
  }
} /* ESM_PRINT_MSG() */

/*==============================================================================

  FUNCTION:  esm_build_and_send_cm_act_dedicated_ctxt_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_cm_act_dedicated_ctxt_req() does goes here.

    @detail
    Builds and sends cm activate dedicated context request.
*/
/*============================================================================*/
void esm_build_and_send_cm_act_dedicated_ctxt_req
(
  struct esm_eps_bearer_context* ctxt_ptr   /*!< REQUIRED short parameter description */
)
{
  
  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;


  /* Fill the CM command directly later when the definition of cmd type available*/
  cm_act_dedicated_bearer_context_request_ind_s_type* cm_dedicated_req_cmd = (cm_act_dedicated_bearer_context_request_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_act_dedicated_bearer_context_request_ind_s_type));
  /* Fill the msgr header*/
  msgr_init_hdr(&cm_dedicated_req_cmd->msg_hdr,MSGR_NAS_ESM,MM_CM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND);
  /* Fill the ESM header*/
  cm_dedicated_req_cmd->esm_hdr.connection_id = ctxt_ptr->connection_id;
  /*cm_def_req_cmd->esm_hdr.con_id_creator = ctxt_ptr->con_id_creator ;*/
  cm_dedicated_req_cmd->esm_hdr.sdf_id = ctxt_ptr->sdf_id;

  /* transaction identifier is not being passed to the DS as it is not needed 
  by the DS*/
  cm_dedicated_req_cmd->eps_bearer_id = ctxt_ptr->eps_bearer_id;
  cm_dedicated_req_cmd->esm_hdr.lbi = ctxt_ptr->lbi;
  cm_dedicated_req_cmd->sdf_qos = ctxt_ptr->sdf_qos;
  cm_dedicated_req_cmd->tft = ctxt_ptr->tft;

  if(ctxt_ptr->neg_qos.valid)
  {
    cm_dedicated_req_cmd->neg_qos.valid = TRUE;
    cm_dedicated_req_cmd->neg_qos = ctxt_ptr->neg_qos;
  }
  else
  {
    cm_dedicated_req_cmd->neg_qos.valid = FALSE;
  }

  if(ctxt_ptr->neg_llc_sapi.valid)
  {
    cm_dedicated_req_cmd->neg_llc_sapi.valid = TRUE;
    cm_dedicated_req_cmd->neg_llc_sapi = ctxt_ptr->neg_llc_sapi;
  }
  else
  {
    cm_dedicated_req_cmd->neg_llc_sapi.valid = FALSE;
  }

  if(ctxt_ptr->config_options.valid)
  {
    cm_dedicated_req_cmd->protocol_config_options.valid = TRUE;
    cm_dedicated_req_cmd->protocol_config_options = ctxt_ptr->config_options;
  }
  else
  {
    cm_dedicated_req_cmd->protocol_config_options.valid = FALSE;
  }
  if( (ctxt_ptr->ti.valid == TRUE) && (ctxt_ptr->neg_qos.valid == TRUE) )
  {
    cm_dedicated_req_cmd->gsm_umts_connection_id.valid = TRUE;
    cm_dedicated_req_cmd->gsm_umts_connection_id.connection_id = ctxt_ptr->ti.connection_id;
    MSG_HIGH_DS_2(SM_SUB, "ESM: CTXT =%d with gsm_umts_connection_id.connection_id = %d is transferable to W & minimum requirement for G is satisfied",
                  cm_dedicated_req_cmd->eps_bearer_id,
                  cm_dedicated_req_cmd->gsm_umts_connection_id.connection_id);
  }
  else
  {
    cm_dedicated_req_cmd->gsm_umts_connection_id.valid = FALSE;
  }

  e_retval = esm_msgr_send( (msgr_hdr_s*)cm_dedicated_req_cmd, sizeof(cm_act_dedicated_bearer_context_request_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent MM_CM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND");
  event.esm_out_msg = (byte)MM_CM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);

  esm_modem_mem_free(cm_dedicated_req_cmd);
} /* esm_build_and_send_cm_act_def_ctxt_req() */

/*==============================================================================

  FUNCTION:  esm_copy_dedicated_ctxt_data

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_copy_dedicated_ctxt_data() does goes here.

    @detail 
    Copies the context data received from the NW into the context instance
*/
/*============================================================================*/
void esm_copy_dedicated_ctxt_data
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const lte_nas_esm_act_dedicated_bearer_context_req* msg_ptr   /*!< REQUIRED short parameter description */
)
{
  byte mask = 0x08;
  //Already extracted, so, just copy
  ctxt_ptr->eps_bearer_id = msg_ptr->lte_nas_esm_hdr.bearer_id;
  ctxt_ptr->pti = msg_ptr->lte_nas_esm_hdr.pti;   

  /* Mandatory parameters*/
  ctxt_ptr->lbi = msg_ptr->linked_eps_bearer_identity;
  ctxt_ptr->sdf_qos = msg_ptr->sdf_qos;
  ctxt_ptr->tft = msg_ptr->tft;

  if(msg_ptr->neg_qos.valid)
  {
    ctxt_ptr->neg_qos = msg_ptr->neg_qos;
  }
  if(msg_ptr->neg_llc_sapi.valid)
  {
    ctxt_ptr->neg_llc_sapi = msg_ptr->neg_llc_sapi;
  }
  if(msg_ptr->radio_priority.valid)
  {
    ctxt_ptr->radio_pri = msg_ptr->radio_priority;
  }
  if(msg_ptr->pack_flo_id.valid)
  {
    ctxt_ptr->packet_flow_id = msg_ptr->pack_flo_id;
  }
  if(msg_ptr->protocol_conf_opt.valid)
  {
    ctxt_ptr->config_options = msg_ptr->protocol_conf_opt;
  }
  if(msg_ptr->ti.valid == TRUE)
  {
    ctxt_ptr->ti.valid = TRUE;
    /* As per the spec 24.007 isection 11.2.3.1.3 (Transaction identifier) the octet 4 will have the transaction id. 
    24.008 specifies the Linked TI format in section 10.5.6.7 i
    Flip the TI flag which should have been received as 0 and then add it to the transaction id. 
    This is what SM will use to talk to NW on UMTS side. 
    NW uses the octet 3 to fill the transaction id */
    if(msg_ptr->ti.ti_flag == FALSE)
    {
      ctxt_ptr->ti.connection_id = mask | msg_ptr->ti.val_of_transaction_id[0];
    }
    else
    {
      mask = 0x0;
      ctxt_ptr->ti.connection_id = mask | msg_ptr->ti.val_of_transaction_id[0];
    }
  }
  else
  {
    ctxt_ptr->ti.valid = FALSE;
  }
} /* esm_copy_dedicated_ctxt_data() */

/*==============================================================================

  FUNCTION:  esm_build_and_send_act_dedicated_ctxt_accept

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_act_dedicated_ctxt_accept() does goes here.

    @detail
    Builds and sends activate dedicated context accept.
*/
/*============================================================================*/
void esm_build_and_send_act_dedicated_ctxt_accept
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_act_dedicated_bearer_context_accept_T* msg_ptr/*!< REQUIRED short parameter description */
)
{
  
  errno_enum_type        e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;

  
  emm_data_req_T * out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));
  MSG_LOW_DS_0(SM_SUB, "ESM: ENTER esm_build_and_send_act_def_ctxt_accept\n");

  /* Init the MSGR header*/
  msgr_init_hdr(&out_msg_ptr->msg_hdr,MSGR_NAS_ESM,NAS_EMM_DATA_REQ);
  /*emm_cmd.emm_data_req.emm_transaction_id = emm_get_nas_transaction_id ();*/
  out_msg_ptr->emm_transaction_id = 0;
  out_msg_ptr->emm_transaction_id = esm_get_trans_id_for_data_req(ctxt_ptr->connection_id,LTE_ESM_BCM);

  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT;

  /* Fill the esm specific header*/
  out_msg_ptr->esm_out_msg.dedicated_bearer_context_accept.lte_nas_esm_hdr.bearer_id = ctxt_ptr->eps_bearer_id;
  //out_msg_ptr->dedicated_bearer_context_accept.lte_nas_esm_hdr.pti = ctxt_ptr->pti;
  out_msg_ptr->esm_out_msg.dedicated_bearer_context_accept.lte_nas_esm_hdr.pti = 0x0;

  if(msg_ptr->protocol_config_options.valid)
  {
    out_msg_ptr->esm_out_msg.default_bearer_context_accept.protocol_conf_opt.valid = TRUE;
    out_msg_ptr->esm_out_msg.default_bearer_context_accept.protocol_conf_opt = msg_ptr->protocol_config_options;
    /* DS is overwriting what N/W sent or what N/W accepted earlier when we received the request*/
    ctxt_ptr->config_options = msg_ptr->protocol_config_options;
  }
  else
  {
    out_msg_ptr->esm_out_msg.default_bearer_context_accept.protocol_conf_opt.valid = FALSE;
  }

  /* TEMPORARY FIX . NEED TO REMOVE THIS */
  //emm_cmd.emm_data_req.esm_buffer_length = sizeof(out_msg_ptr->default_bearer_context_accept);

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent NAS_EMM_DATA_REQ carrying ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT");
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(out_msg_ptr);
} /* esm_build_and_send_act_dedicated_ctxt_accept() */

/*==============================================================================

  FUNCTION:  esm_build_and_send_act_dedicated_ctxt_reject

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_act_dedicated_ctxt_reject() does goes here.

    @detail
    Builds and sends activate dedicated context reject.
*/
/*============================================================================*/
void esm_build_and_send_act_dedicated_ctxt_reject 
(
  struct esm_eps_bearer_context* ctxt_ptr,  /*!< REQUIRED short parameter description */
  const esm_act_dedicated_bearer_context_rej_T* msg_ptr /*!< REQUIRED short parameter description */
)
{
  errno_enum_type        e_retval;
  
  log_lte_nas_esm_outgoing_msg_event_type event;

  
  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));
  msgr_init_hdr(&out_msg_ptr->msg_hdr,MSGR_NAS_ESM,NAS_EMM_DATA_REQ);

  out_msg_ptr->emm_transaction_id = 0;
  out_msg_ptr->emm_transaction_id = esm_get_trans_id_for_data_req(ctxt_ptr->connection_id,LTE_ESM_BCM);

  /*ESM header is not needed to be filled for EMM*/
  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT;

  /* Fill the esm specific header*/
  out_msg_ptr->esm_out_msg.dedicated_bearer_context_rej.lte_nas_esm_hdr.bearer_id = ctxt_ptr->eps_bearer_id;
  //out_msg_ptr->dedicated_bearer_context_rej.lte_nas_esm_hdr.pti = ctxt_ptr->pti;
  out_msg_ptr->esm_out_msg.dedicated_bearer_context_rej.lte_nas_esm_hdr.pti = 0x0;

  /* Is assertion the right thing to do or just drop the message?*/
  /*ASSERT(out_msg_ptr->default_bearer_context_reject.esm_cause.valid == TRUE);*/

  if(msg_ptr->esm_cause.valid == TRUE)
  {
    out_msg_ptr->esm_out_msg.dedicated_bearer_context_rej.esm_cause = msg_ptr->esm_cause;
  }
  else
  {
    out_msg_ptr->esm_out_msg.dedicated_bearer_context_rej.esm_cause.esm_cause = NAS_ESM_PROTOCOL_ERROR;
    if(msg_ptr->esm_local_cause.valid == TRUE)
    {
      MSG_HIGH_DS_1(SM_SUB, "ESM: DEDICATED BEARER REJECT: Local Cause = %d ",
                    msg_ptr->esm_local_cause.local_cause);
    }
    else
    {
      MSG_ERROR_DS_0(SM_SUB, "ESM: DEDICATED BEARER REJECT: Local Cause and esm cause both are invalid. This shouldn't happen");
    }
  }


  if(msg_ptr->protocol_config_options.valid)
  {
    out_msg_ptr->esm_out_msg.dedicated_bearer_context_rej.protocol_conf_opt.valid = TRUE;
    out_msg_ptr->esm_out_msg.dedicated_bearer_context_rej.protocol_conf_opt = msg_ptr->protocol_config_options;
    /* No need to overwrite in the ctxt as we are rejecting the request*/
  }
  else
  {
    out_msg_ptr->esm_out_msg.dedicated_bearer_context_rej.protocol_conf_opt.valid = FALSE;
  }

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) ); 
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent NAS_EMM_DATA_REQ carrying ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT");
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(out_msg_ptr);
} /* esm_build_and_send_act_dedicated_ctxt_reject */

/*==============================================================================

  FUNCTION:  esm_build_and_send_emm_detach_cmd

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_act_dedicated_ctxt_reject() does goes here.

    @detail
    Builds and sends emm_detach_cmd.
*/
/*============================================================================*/
void esm_build_and_send_emm_detach_cmd(lte_nas_local_cause_enum_T local_cause)
{
  
  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;
  
  emm_detach_cmd_T* detach_cmd = (emm_detach_cmd_T *)esm_modem_mem_alloc(sizeof(emm_detach_cmd_T));
  msgr_init_hdr(&(detach_cmd->msg_hdr),MSGR_NAS_ESM,NAS_EMM_DETACH_CMD);

  detach_cmd->detach_cause = local_cause;

  e_retval = esm_msgr_send( (msgr_hdr_s*)detach_cmd, sizeof(emm_detach_cmd_T) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent NAS_EMM_DETACH_CMD ");
  event.esm_out_msg = (byte)NAS_EMM_DETACH_CMD;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(detach_cmd);
}

void esm_build_and_send_eps_bearer_status_update_ind
(
  const esm_bcm_T* mgr_ptr,
  lte_nas_isr_status_type isr_status
)
{
  
  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;
  byte     index = 0;
#ifdef FEATURE_LTE_REL9
  boolean  emc_bc_exist = FALSE;
  boolean  non_emc_bc_exist = FALSE;
#endif

  
  emm_eps_bearer_status_cmd_T* bearer_status_cmd = (emm_eps_bearer_status_cmd_T *)esm_modem_mem_alloc(sizeof(emm_eps_bearer_status_cmd_T));
  msgr_init_hdr(&(bearer_status_cmd->msg_hdr),MSGR_NAS_ESM,NAS_EMM_EPS_BEARER_STATUS_CMD);

  

  bearer_status_cmd->isr_status = isr_status;

  for(index = 0; index < ESM_MAX_EPS_BEARERS; index++)
  { 
    if((mgr_ptr->esm_bc[index] != NULL) && ((mgr_ptr->esm_bc[index]->eps_bearer_state_ptr->state() == ESM_BC_ACTIVE) ||
       (mgr_ptr->esm_bc[index]->eps_bearer_state_ptr->state() == ESM_BC_MODIFY_PENDING)))
    {
#ifdef FEATURE_LTE_REL9      
      if(mgr_ptr->esm_bc[index]->default_bearer_ctxt == TRUE) 
      { 
        if(mgr_ptr->esm_bc[index]->emc_bc == TRUE)
        {
          emc_bc_exist = TRUE;
        }
        else
        {
          non_emc_bc_exist = TRUE;
        }
      }
      else if((mgr_ptr->esm_bc[index]->emc_bc == TRUE) &&
              (mgr_ptr->esm_bc[index]->sdf_qos.qci == 1))
      {
        /* Voice bearer for emergency service exist */
        MSG_HIGH_DS_1(SM_SUB, "ESM: Dedicated bearer %d for emergency voice srv exists",
                      mgr_ptr->esm_bc[index]->eps_bearer_id);
        bearer_status_cmd->context_status.emc_voice_bearer_exist = TRUE;
      }
#endif

      switch(mgr_ptr->esm_bc[index]->eps_bearer_id)
      {
        case 5:
          bearer_status_cmd->context_status.eps_bearer_context_5_exists = TRUE;
          break;
        case 6:
          bearer_status_cmd->context_status.eps_bearer_context_6_exists = TRUE;
          break;
        case 7:
          bearer_status_cmd->context_status.eps_bearer_context_7_exists = TRUE;
          break;
        case 8:
          bearer_status_cmd->context_status.eps_bearer_context_8_exists = TRUE;
          break;
        case 9:
          bearer_status_cmd->context_status.eps_bearer_context_9_exists = TRUE;
          break;
        case 10:
          bearer_status_cmd->context_status.eps_bearer_context_10_exists = TRUE;
          break;
        case 11:
          bearer_status_cmd->context_status.eps_bearer_context_11_exists = TRUE;
          break;
        case 12:
          bearer_status_cmd->context_status.eps_bearer_context_12_exists = TRUE;
          break;
        case 13:
          bearer_status_cmd->context_status.eps_bearer_context_13_exists = TRUE;
          break;
        case 14:
          bearer_status_cmd->context_status.eps_bearer_context_14_exists = TRUE;
          break;
        case 15:
          bearer_status_cmd->context_status.eps_bearer_context_15_exists = TRUE;
          break;
        default:
          MSG_ERROR_DS_1(SM_SUB, "ESM: Invalid bearer ID %d",
                         mgr_ptr->esm_bc[index]->eps_bearer_id);
          break;
      }
    }
  }

#ifdef FEATURE_LTE_REL9
  if(emc_bc_exist == TRUE) 
  {
    if(non_emc_bc_exist == TRUE)
    {
      esm_set_emc_srv_status(LTE_NAS_EMC_SRV_EXIST);
    }
    else
    {
      esm_set_emc_srv_status(LTE_NAS_EMC_SRV_ONLY);
    }
  }
  else
  {
     if(esm_get_emc_srv_status() != LTE_NAS_EMC_SRV_PENDING)
     {
       esm_set_emc_srv_status(LTE_NAS_EMC_SRV_NONE);
     }
  }
#endif

  e_retval = esm_msgr_send( (msgr_hdr_s*)bearer_status_cmd, sizeof(emm_eps_bearer_status_cmd_T) );  
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent LTE_EPS_BEARER_STATUS_CMD ");
  event.esm_out_msg = (byte)NAS_EMM_EPS_BEARER_STATUS_CMD;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(bearer_status_cmd);
}

void esm_build_and_send_deactivate_bearer_context_accept
(
  byte bearer_id,
  protocol_cfg_options_T* prot_ptr
)
{
  
  errno_enum_type        e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;

  
  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));
  MSG_LOW_DS_0(SM_SUB, "ESM: ENTER esm_build_and_send_deactivate_bearer_context_accept\n");

  /* Init the MSGR header*/
  msgr_init_hdr(&out_msg_ptr->msg_hdr,MSGR_NAS_ESM,NAS_EMM_DATA_REQ);
  /* We don't have a connection id at the time of sending the deactivate accept so use 0xff*/
  out_msg_ptr->emm_transaction_id = esm_get_trans_id_for_data_req(0xff,LTE_ESM_BCM);

  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT;

  /* Fill the esm specific header*/
  out_msg_ptr->esm_out_msg.deactivate_eps_bearer_context_accept.lte_nas_esm_hdr.bearer_id = bearer_id;
  out_msg_ptr->esm_out_msg.deactivate_eps_bearer_context_accept.lte_nas_esm_hdr.pti = 0x0;

  if(prot_ptr != NULL)
  {
    out_msg_ptr->esm_out_msg.deactivate_eps_bearer_context_accept.protocol_conf_opt = *prot_ptr;
  }
  else
  {
    out_msg_ptr->esm_out_msg.deactivate_eps_bearer_context_accept.protocol_conf_opt.valid = FALSE;
  }

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent NAS_EMM_DATA_REQ carrying DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT");
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(out_msg_ptr);

}

void esm_build_and_send_bearer_resource_allocation_req
(
  esm_bearer_resource_proc_T* proc_ptr, 
  const esm_bearer_resource_alloc_req_T* msg_ptr 
)
{
  
  errno_enum_type        e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;
 
  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));
  ASSERT(proc_ptr != NULL);
  ASSERT(proc_ptr->pending_msg_ptr != NULL);
  ASSERT(msg_ptr != NULL);

  MSG_LOW_DS_0(SM_SUB, "ESM: ENTER esm_build_and_send_bearer_resource_allocation_req\n");

  /* Store the sdf id as this will be the key if ESm received abort request from CM/DS */
  proc_ptr->sdf_id = msg_ptr->esm_hdr.sdf_id;
  proc_ptr->lbi    = msg_ptr->esm_hdr.lbi;

  /* Init the MSGR header*/
  msgr_init_hdr(&out_msg_ptr->msg_hdr,MSGR_NAS_ESM,NAS_EMM_DATA_REQ);
  out_msg_ptr->emm_transaction_id = esm_get_trans_id_for_data_req(proc_ptr->instance_id,LTE_ESM_BPM);

  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = BEARER_RESOURCE_ALLOCATION_REQUEST;

  /* Fill the esm specific header*/
  out_msg_ptr->esm_out_msg.bearer_alloc_req.lte_nas_esm_hdr.bearer_id = ESM_INVALID_BEARER_ID;
  out_msg_ptr->esm_out_msg.bearer_alloc_req.lte_nas_esm_hdr.pti = proc_ptr->pti;
  out_msg_ptr->esm_out_msg.bearer_alloc_req.linked_eps_bearer_identity = msg_ptr->esm_hdr.lbi;
  out_msg_ptr->esm_out_msg.bearer_alloc_req.tft = msg_ptr->ul_dl_tft;
  out_msg_ptr->esm_out_msg.bearer_alloc_req.sdf_qos = msg_ptr->sdf_qos;
  /* Optional Filed, assuming that the DS/CM would have the valid flag set to TRUE or FALSE*/
  out_msg_ptr->esm_out_msg.bearer_alloc_req.protocol_conf_opt  = msg_ptr->protocol_config_options;
#ifdef FEATURE_LTE_REL10
  if(emm_get_3gpp_rel_ver_per_sub((sys_modem_as_id_e_type)sm_as_id) >= LTE_3GPP_REL10)
  {
    out_msg_ptr->esm_out_msg.bearer_alloc_req.low_priority_indicator = 
                                               msg_ptr->low_priority_signaling;
  }
#endif  


  memscpy((void *)(proc_ptr->pending_msg_ptr), sizeof(emm_data_req_T),(void *)out_msg_ptr, sizeof(emm_data_req_T));

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T));
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent NAS_EMM_DATA_REQ carrying BEARER_RESOURCE_ALLOCATION_REQUEST");
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(out_msg_ptr);
}

/*==============================================================================

  FUNCTION:  esm_build_and_send_cm_bearer_resource_allocation_reject

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_cm_send_bearer_resource_allocation_reject() here.

    @detail
    Builds and sends cm bearer resource allocation reject.
*/
/*============================================================================*/
void esm_build_and_send_cm_bearer_resource_allocation_reject
(
  word sdf_id,
  const lte_nas_esm_bearer_resource_allocation_rej* msg_ptr,
  lte_nas_local_cause_enum_T  local_cause,
  byte lbi
)
{
  
  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;

  /* Fill the CM command directly later when the definition of cmd type available*/
  cm_bearer_resource_alloc_reject_ind_s_type* cm_bearer_rsrc_rej_ind = 
                                              (cm_bearer_resource_alloc_reject_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_bearer_resource_alloc_reject_ind_s_type)); 
  /* Fill the msgr header*/
  msgr_init_hdr(&cm_bearer_rsrc_rej_ind->msg_hdr,MSGR_NAS_ESM,MM_CM_BEARER_RESOURCE_ALLOC_REJECT_IND);
  /* Fill the ESM header*/
  cm_bearer_rsrc_rej_ind->esm_hdr.connection_id = ESM_INVALID_CON_ID;
  cm_bearer_rsrc_rej_ind->esm_hdr.sdf_id = sdf_id;
  cm_bearer_rsrc_rej_ind->esm_hdr.lbi = lbi;

  if((msg_ptr != NULL) && (msg_ptr->lte_nas_hdr.valid == FALSE))
  {
    esm_send_esm_status(&msg_ptr->lte_nas_esm_hdr, 
                         msg_ptr->lte_nas_hdr.esm_decode_fail_reason);
  }

  if((msg_ptr != NULL) && (msg_ptr->lte_nas_hdr.valid == TRUE))
  {
    cm_bearer_rsrc_rej_ind->esm_cause = msg_ptr->esm_cause;
    cm_bearer_rsrc_rej_ind->esm_local_cause.valid = FALSE;
    cm_bearer_rsrc_rej_ind->protocol_config_options = msg_ptr->protocol_conf_opt;
#ifdef FEATURE_LTE_REL10
    cm_bearer_rsrc_rej_ind->backoff_timer = msg_ptr->backoff_timer;
#endif
  }
  else
  {
    cm_bearer_rsrc_rej_ind->esm_cause.valid = FALSE;
    cm_bearer_rsrc_rej_ind->esm_local_cause.valid = TRUE;
    cm_bearer_rsrc_rej_ind->esm_local_cause.local_cause = local_cause;
    cm_bearer_rsrc_rej_ind->protocol_config_options.valid = FALSE;
  }

  e_retval = esm_msgr_send( (msgr_hdr_s*)cm_bearer_rsrc_rej_ind, sizeof(cm_bearer_resource_alloc_reject_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent MM_CM_BEARER_RESOURCE_ALLOC_REJECT_IND");
  event.esm_out_msg = (byte)MM_CM_BEARER_RESOURCE_ALLOC_REJECT_IND;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(cm_bearer_rsrc_rej_ind);

} /* esm_build_and_send_cm_bearer_resource_allocation_reject() */

/*==============================================================================

  FUNCTION:  esm_build_and_send_bearer_resource_alloc_abort_rsp

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_bearer_resource_alloc_abort_rsp() here.

    @detail
*/
/*============================================================================*/
void esm_build_and_send_bearer_resource_alloc_abort_rsp(word sdf_id,boolean result)
{
  
  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;

  /* Fill the CM command directly later when the definition of cmd type available*/
  cm_bearer_resource_alloc_failure_ind_s_type* cm_bearer_rsrc_alloc_failure = (cm_bearer_resource_alloc_failure_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_bearer_resource_alloc_failure_ind_s_type));
  /* Fill the msgr header*/
  msgr_init_hdr(&cm_bearer_rsrc_alloc_failure->msg_hdr,MSGR_NAS_ESM,MM_CM_BEARER_RESOURCE_ALLOC_FAILURE_IND);
  /* Fill the ESM header*/
  cm_bearer_rsrc_alloc_failure->esm_hdr.connection_id = ESM_INVALID_CON_ID;
  cm_bearer_rsrc_alloc_failure->esm_hdr.sdf_id = sdf_id;

  cm_bearer_rsrc_alloc_failure->result = result;

  e_retval = esm_msgr_send( (msgr_hdr_s*)cm_bearer_rsrc_alloc_failure, sizeof(cm_bearer_resource_alloc_failure_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_1(SM_SUB, "ESM: ESM sent  MM_CM_BEARER_RESOURCE_ALLOC_FAILURE_IND for SDF_ID=%d",
                sdf_id);
  event.esm_out_msg = (byte)MM_CM_BEARER_RESOURCE_ALLOC_FAILURE_IND;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);

  esm_modem_mem_free(cm_bearer_rsrc_alloc_failure);
}

/*==============================================================================

  FUNCTION:  esm_start_timer

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_start_timer() here.

    @detail
    timer utility function.
*/
/*============================================================================*/
boolean esm_start_timer
(
  esm_timer_type                    timer_id,
  struct esm_bearer_resource_proc  *proc_ptr 
)
{
  dword                             timer_val = 0;
  log_lte_nas_esm_timer_event_type  event;
  unsigned long                     param;

  ASSERT(proc_ptr != NULL);

  switch(timer_id)
  {
    case ESM_TIMER_T3480:
      timer_val = ESM_BEARER_RSRC_ALLOC_TIMER_VAL;
      MSG_HIGH_DS_2(SM_SUB, "ESM: Start timer 3480 - %u(ms) for proc %d",
                    timer_val,
                    proc_ptr->instance_id);
      break;
    case ESM_TIMER_T3481:
      timer_val = ESM_BEARER_RSRC_MODIFY_TIMER_VAL;
      MSG_HIGH_DS_2(SM_SUB, "ESM: Start timer 3481 - %u(ms) for proc %d",
                    timer_val,
                    proc_ptr->instance_id);
      break;
    case ESM_TIMER_T3482:
      timer_val = ESM_PDN_CONNECTIVITY_TIMER_VAL;
      MSG_HIGH_DS_2(SM_SUB, "ESM: Start timer 3482 - %u(ms) for proc %d",
                    timer_val,
                    proc_ptr->instance_id);
      break;
    case ESM_TIMER_T3492:
      timer_val = ESM_PDN_DISCONNECT_TIMER_VAL;
      MSG_HIGH_DS_2(SM_SUB, "ESM: Start timer 3492 - %u(ms) for proc %d",
                    timer_val,
                    proc_ptr->instance_id);
      break;
    default:
      MSG_ERROR_DS_2(SM_SUB, "ESM: Invalid timer to start - %u for proc %d",
                     timer_id,
                     proc_ptr->instance_id);
      return FALSE;
  }

  /* Check to see if timer already exists */
  if(proc_ptr->timer_ptr != NULL)
  {
    /* Stop timer */
    (void)rex_clr_timer(proc_ptr->timer_ptr);

    /* Deallocate timer */
    rex_delete_timer_ex(proc_ptr->timer_ptr);
    
    /* Make sure to mark this timer as inactive */
    proc_ptr->timer_ptr = NULL;
  }
   
  /* Copy data to param */
  param = proc_ptr->instance_id;
    
  /* Copy timer id to param */
  param |= (byte)timer_id << 16;
    
  /* Allocate timer */
 
#ifdef FEATURE_DUAL_SIM
  if((sys_modem_as_id_e_type)sm_as_id == SYS_MODEM_AS_ID_2)
  {
    proc_ptr->timer_ptr = (rex_timer_type*)rex_create_timer_ex(esm_post_timer_expired_sub2, param);
  }
  else
#endif
  {
    proc_ptr->timer_ptr = (rex_timer_type*)rex_create_timer_ex(esm_post_timer_expired, param);
  }
    
  /* If memory was available */  
  if(proc_ptr->timer_ptr != NULL)
  {  
    /* Start Timer */
    (void)rex_set_timer(proc_ptr->timer_ptr, timer_val);
  }
  else
  {
    /* Print error message for now. If there is no response from NW for the 
       procedure, the message may sit for ever without the timer. */
    MSG_ERROR_DS_2(SM_SUB, "ESM: Failed to start timer %u for porc %d",
                   timer_id,
                   proc_ptr->instance_id);
  }
    
  event.esm_timer_event = (byte)timer_id;
  /* log the event for ConnectionRequest being sent */
  event_report_payload(EVENT_LTE_ESM_TIMER_START, sizeof(event), &event);

  return TRUE;
}

/*==============================================================================

  FUNCTION:  esm_stop_timer

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_stop_timer() here.

    @detail
    timer utility function.
*/
/*============================================================================*/
void esm_stop_timer
( 
  esm_timer_type                    timer_id,
  struct esm_bearer_resource_proc  *proc_ptr
)
{
  log_lte_nas_esm_timer_event_type event;

  ASSERT(proc_ptr != NULL);

  /* Check to see if timer exists */
  if(proc_ptr->timer_ptr != NULL)
  {
    /* Stop timer */
    (void)rex_clr_timer(proc_ptr->timer_ptr);

    /* Deallocate timer */
    rex_delete_timer_ex(proc_ptr->timer_ptr);

    /* Make sure to mark this timer as inactive */
    proc_ptr->timer_ptr = NULL;

    switch(timer_id)
    {
      case ESM_TIMER_T3480:
      case ESM_TIMER_T3481:
      case ESM_TIMER_T3482:
      case ESM_TIMER_T3492:
        MSG_HIGH_DS_2(SM_SUB, "ESM: TIMER %d is stopped for PROC %d",
                      timer_id,proc_ptr->instance_id);
        break;
      default:
        MSG_ERROR_DS_2(SM_SUB, "ESM: Invalid timer to stop - %u for proc %d",
                       timer_id,
                       proc_ptr->instance_id);
        break;
    }
    event.esm_timer_event = (byte)timer_id;
    /* log the event for ConnectionRequest being sent */
    event_report_payload(EVENT_LTE_ESM_TIMER_STOP, sizeof(event), &event);
  }
}

/*==============================================================================

  FUNCTION:  esm_clear_timer

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_stop_timer() here.

    @detail
    timer utility function.
*/
/*============================================================================*/
void esm_clear_timer
( 
  struct esm_bearer_resource_proc  *proc_ptr
)
{
  ASSERT(proc_ptr != NULL);

  /* Check to see if timer exists */
  if(proc_ptr->timer_ptr != NULL)
  {
    /* Deallocate timer */
    rex_delete_timer_ex(proc_ptr->timer_ptr);

     /* Make sure to mark this timer as inactive */
     proc_ptr->timer_ptr = NULL;
  }
}

/*==============================================================================

  FUNCTION:  esm_send_retransmission

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_send_retransmission() here.

    @detail
    Sends retransmission.
*/
/*============================================================================*/
void esm_send_retransmission(esm_bearer_resource_proc_T* proc_ptr)
{
  emm_msgr_cmd_type      *emm_cmd_ptr = NULL;
  errno_enum_type        e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;
  boolean status = FALSE;

  ASSERT(proc_ptr != NULL);
  ASSERT(proc_ptr->pending_msg_ptr != NULL);

  /*allocate memory on heap*/
  emm_cmd_ptr = (emm_msgr_cmd_type*)esm_modem_mem_alloc(sizeof(emm_msgr_cmd_type));


  /* Init the MSGR header*/
  msgr_init_hdr(&emm_cmd_ptr->msg_hdr,MSGR_NAS_ESM,NAS_EMM_DATA_REQ);

  /* Reuse ESM transaction ID */
  emm_cmd_ptr->emm_data_req.emm_transaction_id = 
                                 proc_ptr->pending_msg_ptr->emm_transaction_id;

  emm_cmd_ptr->emm_data_req.esm_out_msg = proc_ptr->pending_msg_ptr->esm_out_msg;

  e_retval = esm_msgr_send( &emm_cmd_ptr->msg_hdr, sizeof(emm_msgr_cmd_type) );
  ASSERT(e_retval == E_SUCCESS);

  MSG_HIGH_DS_1(SM_SUB, "ESM: Retransmission. ESM sent NAS_EMM_DATA_REQ carrying message id=%d",
                emm_cmd_ptr->emm_data_req.esm_out_msg.nas_hdr.msg_id);

  /*free heap memory*/
  esm_modem_mem_free(emm_cmd_ptr);

  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  switch(proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id)
  {
  case BEARER_RESOURCE_ALLOCATION_REQUEST:
    esm_stop_timer(ESM_TIMER_T3480, proc_ptr);
    status = esm_start_timer(ESM_TIMER_T3480,proc_ptr);
    ASSERT(status == TRUE);
    break;
  case BEARER_RESOURCE_MODIFICATION_REQUEST:
    esm_stop_timer(ESM_TIMER_T3481, proc_ptr);
    status = esm_start_timer(ESM_TIMER_T3481,proc_ptr);
    ASSERT(status == TRUE);
    break;

    case PDN_CONNECTIVITY_REQUEST:
      esm_stop_timer(ESM_TIMER_T3482, proc_ptr);
    status = esm_start_timer(ESM_TIMER_T3482,proc_ptr);
    ASSERT(status == TRUE);
    break;
  case PDN_DISCONNECT_REQUEST:
    esm_stop_timer(ESM_TIMER_T3492, proc_ptr);
    status = esm_start_timer(ESM_TIMER_T3492,proc_ptr);
    ASSERT(status == TRUE);
    break;
  default:
    MSG_ERROR_DS_1(SM_SUB, "ESM: Unknown message sent over the air, no timer to start",
                   proc_ptr->pending_msg_ptr->esm_out_msg.nas_hdr.msg_id);
  }
}

/*==============================================================================

  FUNCTION:  esm_is_detach_required

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_is_detach_required() here.

    @detail
    Checks to see if this is the last PDN we are disconnecting from. If so, trigger a detach_cmd towards
    EMM and when EMM sends detach_ind back to ESM, then clean up locally which will be handled in the
    function that handled the detach_ind from EMM

    See Spec TS 24.301 section 5.5.2.1
*/
/*============================================================================*/
boolean esm_is_detach_required(byte bearer_id, esm_bcm_T* mgr_ptr)
{

  byte index = 0, count = 0;

  ASSERT(mgr_ptr != NULL);

  for(index = 0; index < ESM_MAX_EPS_BEARERS; index++)
  {
    /* Is this a default bearer?  */
    if(mgr_ptr->esm_bc[index] != NULL && mgr_ptr->esm_bc[index]->default_bearer_ctxt == TRUE)
    {
      count++;
    }
  }
  /* We are connected to more than one PDN*/
  if(count > 1)
  {
    return FALSE;
  }
  else 
  /* We are disconnecting from the last PDN. So, need to tell EMM to start
    the DETACH process */
  {
    return TRUE;
  }
}

/*==============================================================================

  FUNCTION:  esm_build_and_send_pdn_disconnect_req

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_pdn_disconnect_req() here.

    @detail
    Build and send pdn disconnect request 
*/
/*============================================================================*/
void esm_build_and_send_pdn_disconnect_req
(
  struct esm_bearer_resource_proc* proc_ptr,
  esm_pdn_disconnect_req_T* msg_ptr
)
{
  
  errno_enum_type        e_retval;
  
  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));

  ASSERT(proc_ptr != NULL);
  ASSERT(proc_ptr->pending_msg_ptr != NULL);
  ASSERT(msg_ptr != NULL);

  MSG_LOW_DS_0(SM_SUB, "ESM: ENTER esm_build_and_send_pdn_disconnect_req\n");

  /* Init the MSGR header*/
  msgr_init_hdr(&out_msg_ptr->msg_hdr,MSGR_NAS_ESM,NAS_EMM_DATA_REQ);
  out_msg_ptr->emm_transaction_id = esm_get_trans_id_for_data_req(proc_ptr->instance_id,LTE_ESM_BPM);

  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = PDN_DISCONNECT_REQUEST;

  /* Fill the esm specific header*/
  out_msg_ptr->esm_out_msg.pdn_disconnect_req.lte_nas_esm_hdr.bearer_id = ESM_INVALID_BEARER_ID;
  out_msg_ptr->esm_out_msg.pdn_disconnect_req.lte_nas_esm_hdr.pti = proc_ptr->pti;
  out_msg_ptr->esm_out_msg.pdn_disconnect_req.linked_eps_bearer_identity = msg_ptr->eps_bearer_id;
  /* Optional Filed, assuming that the DS/CM would have the valid flag set to TRUE or FALSE*/
  out_msg_ptr->esm_out_msg.pdn_disconnect_req.protocol_conf_opt  = msg_ptr->protocol_config_options;

  
  memscpy((void *)(proc_ptr->pending_msg_ptr),sizeof(emm_data_req_T),(void *)out_msg_ptr, sizeof(emm_data_req_T));

  proc_ptr->lbi = msg_ptr->eps_bearer_id;

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent NAS_EMM_DATA_REQ carrying PDN_DISCONNECT_REQUEST");
  esm_modem_mem_free(out_msg_ptr);
}

/*==============================================================================

  FUNCTION: esm_build_and_send_get_pdn_connectivity_req_ind 

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_get_pdn_connectivity_req_ind() here.

    @detail
    Builds and sends esm_build_and_send_get_pdn_connectivity_req_ind towards CM.
*/
/*============================================================================*/
void esm_build_and_send_get_pdn_connectivity_req_ind
(
  uint8 seq_num
)
{
  
  errno_enum_type                          e_retval;
  log_lte_nas_esm_outgoing_msg_event_type  event;

  cm_get_pdn_connectivity_request_ind_s_type  *cm_msg_ptr = 
                                   (cm_get_pdn_connectivity_request_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_get_pdn_connectivity_request_ind_s_type));

  msgr_init_hdr(&(cm_msg_ptr->msg_hdr), 
                MSGR_NAS_ESM,
                MM_CM_GET_PDN_CONNECTIVITY_REQUEST_IND);

  cm_msg_ptr->seq_num = seq_num;

  e_retval = esm_msgr_send( (msgr_hdr_s*)cm_msg_ptr, sizeof(cm_get_pdn_connectivity_request_ind_s_type) );

  ASSERT(e_retval == E_SUCCESS);

  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent MM_CM_GET_PDN_CONNECTIVITY_REQUEST_IND");

  event.esm_out_msg = MM_CM_GET_PDN_CONNECTIVITY_REQUEST_IND;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);

  esm_modem_mem_free(cm_msg_ptr);
} /* esm_build_and_send_get_pdn_connectivity_req_ind() */

/*==============================================================================

  FUNCTION:  esm_build_and_send_cm_pdn_disconnect_reject

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_cm_pdn_disconnect_reject() here.

    @detail
*/
/*============================================================================*/
void esm_build_and_send_cm_pdn_disconnect_reject
(
  byte   lbi,
  const  lte_nas_esm_pdn_disconnect_rej* msg_ptr,
  lte_nas_local_cause_enum_T  local_cause
)
{
  
  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;

  cm_pdn_disconnect_reject_ind_s_type* cm_pdn_disconnect_rej = 
                                       (cm_pdn_disconnect_reject_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_pdn_disconnect_reject_ind_s_type));      
  msgr_init_hdr(&cm_pdn_disconnect_rej->msg_hdr,MSGR_NAS_ESM,MM_CM_PDN_DISCONNECT_REJECT_IND);
  cm_pdn_disconnect_rej->esm_hdr.connection_id = ESM_INVALID_CON_ID;

  if(msg_ptr != NULL)
  {
    cm_pdn_disconnect_rej->eps_bearer_id = msg_ptr->lte_nas_esm_hdr.bearer_id;
    cm_pdn_disconnect_rej->esm_cause = msg_ptr->esm_cause;
    cm_pdn_disconnect_rej->esm_local_cause.valid = FALSE;
  }
  else
  {
    cm_pdn_disconnect_rej->eps_bearer_id = lbi;
    cm_pdn_disconnect_rej->esm_cause.valid = FALSE;
    cm_pdn_disconnect_rej->esm_local_cause.valid = TRUE;
    cm_pdn_disconnect_rej->esm_local_cause.local_cause = local_cause;
  }

  e_retval = esm_msgr_send( (msgr_hdr_s*)cm_pdn_disconnect_rej, sizeof(cm_pdn_disconnect_reject_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_1(SM_SUB, "ESM: ESM sent MM_CM_PDN_DISCONNECT_REJECT_IND for BEARER_ID=%d", lbi);
  event.esm_out_msg = (byte)MM_CM_PDN_DISCONNECT_REJECT_IND;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(cm_pdn_disconnect_rej);

}/* esm_build_and_send_cm_pdn_disconnect_reject */

/*==============================================================================

  FUNCTION:  esm_build_and_send_pdn_connectivity_abort_rsp

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_build_and_send_pdn_connectivity_abort_rsp() here.

    @detail
*/
/*============================================================================*/
void esm_build_and_send_pdn_connectivity_abort_rsp(word sdf_id,boolean result)
{
  
  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;

  /* Fill the CM command directly later when the definition of cmd type available*/
  cm_pdn_connectivity_failure_ind_s_type* cm_pdn_connectivity_failure = (cm_pdn_connectivity_failure_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_pdn_connectivity_failure_ind_s_type));
  /* Fill the msgr header*/
  msgr_init_hdr(&cm_pdn_connectivity_failure->msg_hdr,MSGR_NAS_ESM,MM_CM_PDN_CONNECTIVITY_FAILURE_IND);
  /* Fill the ESM header*/
  cm_pdn_connectivity_failure->esm_hdr.connection_id = ESM_INVALID_CON_ID;
  cm_pdn_connectivity_failure->esm_hdr.sdf_id = sdf_id;

  cm_pdn_connectivity_failure->result = result;

  e_retval = esm_msgr_send( (msgr_hdr_s*)cm_pdn_connectivity_failure, sizeof(cm_pdn_connectivity_failure_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_1(SM_SUB, "ESM: ESM sent MM_CM_PDN_CONNECTIVITY_FAILURE_IND  for SDF_ID=%d", sdf_id);
  event.esm_out_msg = (byte)MM_CM_PDN_CONNECTIVITY_FAILURE_IND;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);

  esm_modem_mem_free(cm_pdn_connectivity_failure);
}/* esm_build_and_send_pdn_connectivity_abort_rsp */

/*==============================================================================

  FUNCTION:  esm_build_and_send_bearer_resrc_mod_req

==============================================================================*/
/*!
    @brief
    Build and send bearer resource modification request 

    @detail
*/
/*============================================================================*/
void esm_build_and_send_bearer_resrc_mod_req
(
  struct esm_bearer_resource_proc                 *proc_ptr,
  const  esm_bearer_resource_modification_req_T   *msg_ptr
)
{
  
  errno_enum_type                   e_retval;
 
  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));
  ASSERT(proc_ptr != NULL);
  ASSERT(msg_ptr != NULL);



  proc_ptr->sdf_id = msg_ptr->esm_hdr.sdf_id;
  proc_ptr->lbi    = msg_ptr->esm_hdr.lbi;
  proc_ptr->esm_local_cause = msg_ptr->esm_local_cause;

  /* Init the MSGR header*/
  msgr_init_hdr(&out_msg_ptr->msg_hdr,MSGR_NAS_ESM,NAS_EMM_DATA_REQ);
  out_msg_ptr->emm_transaction_id = 
              esm_get_trans_id_for_data_req(proc_ptr->instance_id,LTE_ESM_BPM);

  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = BEARER_RESOURCE_MODIFICATION_REQUEST;

  /* Fill the esm specific header */
  out_msg_ptr->esm_out_msg.bearer_resource_modifi_req.lte_nas_esm_hdr.bearer_id = 
                                                         ESM_INVALID_BEARER_ID;
  out_msg_ptr->esm_out_msg.bearer_resource_modifi_req.lte_nas_esm_hdr.pti = proc_ptr->pti;

  /* Mandatory fields */
  out_msg_ptr->esm_out_msg.bearer_resource_modifi_req.linked_eps_bearer_identity = 
                                                          msg_ptr->esm_hdr.lbi;
  out_msg_ptr->esm_out_msg.bearer_resource_modifi_req.tft = msg_ptr->tad;

  /* Optional fields */
  /* Assuming that the DS/CM would have the valid flag set to TRUE or FALSE */
  out_msg_ptr->esm_out_msg.bearer_resource_modifi_req.sdf_qos = msg_ptr->sdf_qos;
  out_msg_ptr->esm_out_msg.bearer_resource_modifi_req.esm_cause = msg_ptr->esm_cause;
  out_msg_ptr->esm_out_msg.bearer_resource_modifi_req.protocol_conf_opt = 
                                              msg_ptr->protocol_config_options;
#ifdef FEATURE_LTE_REL10
  if(emm_get_3gpp_rel_ver_per_sub((sys_modem_as_id_e_type)sm_as_id) >= LTE_3GPP_REL10)
  {
    out_msg_ptr->esm_out_msg.bearer_resource_modifi_req.low_priority_indicator = 
                                                msg_ptr->low_priority_signaling;
  }
#endif
  /* Cache the message */
  


  memscpy((void *)(proc_ptr->pending_msg_ptr), sizeof(emm_data_req_T), (void *)out_msg_ptr, sizeof(emm_data_req_T));

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent LTE_EMM_DATA_REQ carrying BEARER_RESOURCE_MODIFICATION_REQUEST");
  esm_modem_mem_free(out_msg_ptr);

}/* esm_build_and_send_bearer_resrc_mod_req */

/*==============================================================================

  FUNCTION:  esm_build_and_send_cm_modify_ctxt_req

==============================================================================*/
/*!
    @brief
    Builds and sends cm modify eps bearer context request.

    @detail    
*/
/*============================================================================*/
void esm_build_and_send_cm_modify_ctxt_req
(
  struct esm_eps_bearer_context   *ctxt_ptr
)
{
  
  errno_enum_type                                e_retval;
  log_lte_nas_esm_outgoing_msg_event_type        event;
  cm_modify_bearer_context_request_ind_s_type   *cm_modify_ctxt_req_ptr =(cm_modify_bearer_context_request_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_modify_bearer_context_request_ind_s_type));



  /* Fill the msgr header*/
  msgr_init_hdr(&cm_modify_ctxt_req_ptr->msg_hdr,
                MSGR_NAS_ESM,
                MM_CM_MODIFY_BEARER_CONTEXT_REQUEST_IND);

  /* Fill the ESM header*/
  cm_modify_ctxt_req_ptr->esm_hdr.connection_id = ctxt_ptr->connection_id;
  cm_modify_ctxt_req_ptr->esm_hdr.sdf_id = ctxt_ptr->sdf_id;

  cm_modify_ctxt_req_ptr->eps_bearer_id = ctxt_ptr->eps_bearer_id;

  cm_modify_ctxt_req_ptr->sdf_qos = ctxt_ptr->bc_modify_data.sdf_qos;
  cm_modify_ctxt_req_ptr->ul_dl_tft = ctxt_ptr->bc_modify_data.tft;
  cm_modify_ctxt_req_ptr->neg_qos = ctxt_ptr->bc_modify_data.neg_qos;
  cm_modify_ctxt_req_ptr->neg_llc_sapi = ctxt_ptr->bc_modify_data.neg_llc_sapi;
  cm_modify_ctxt_req_ptr->apn_ambr = ctxt_ptr->bc_modify_data.apn_ambr;
  cm_modify_ctxt_req_ptr->protocol_config_options = 
                                       ctxt_ptr->bc_modify_data.config_options;

  e_retval = esm_msgr_send( (msgr_hdr_s *)cm_modify_ctxt_req_ptr, sizeof(cm_modify_bearer_context_request_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: Send MM_CM_MODIFY_BEARER_CONTEXT_REQUEST_IND");
  event.esm_out_msg = (byte)MM_CM_MODIFY_BEARER_CONTEXT_REQUEST_IND;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(cm_modify_ctxt_req_ptr);

} /* esm_build_and_send_cm_modify_ctxt_req() */

/*==============================================================================

  FUNCTION:  esm_save_modify_ctxt_data

==============================================================================*/
/*!
    @brief
    Save the context data received from the NW into the context instance

    @detail 
*/
/*============================================================================*/
void esm_save_modify_ctxt_data
(
  struct esm_eps_bearer_context                     *ctxt_ptr,
  const  lte_nas_esm_modify_eps_bearer_ctxt_req     *msg_ptr
)
{
  ASSERT(ctxt_ptr!= NULL);
  ASSERT(msg_ptr!= NULL);

  if(ctxt_ptr->eps_bearer_id != msg_ptr->lte_nas_esm_hdr.bearer_id)
  {
    MSG_ERROR_DS_2(SM_SUB, "ESM: Could not save modify ctxt data duto bearer ID mismatch %d - %d",
                   ctxt_ptr->eps_bearer_id,
                   msg_ptr->lte_nas_esm_hdr.bearer_id);
    return;
  }

  ctxt_ptr->pti = msg_ptr->lte_nas_esm_hdr.pti;   

  /* Init */
  memset((void *)&ctxt_ptr->bc_modify_data, 0x0, 
         sizeof(esm_bearer_context_modify_data_T));

  if(msg_ptr->sdf_qos.valid == TRUE)
  {
    ctxt_ptr->bc_modify_data.sdf_qos = msg_ptr->sdf_qos;
  }
  if(msg_ptr->tft.valid == TRUE)
  {
    ctxt_ptr->bc_modify_data.tft = msg_ptr->tft;
  }
  if(msg_ptr->neg_qos.valid == TRUE)
  {
    ctxt_ptr->bc_modify_data.neg_qos = msg_ptr->neg_qos;
  }
  if(msg_ptr->neg_llc_sapi.valid == TRUE)
  {
    ctxt_ptr->bc_modify_data.neg_llc_sapi = msg_ptr->neg_llc_sapi;
  }
  if(msg_ptr->radio_priority.valid == TRUE)
  {
    ctxt_ptr->bc_modify_data.radio_pri = msg_ptr->radio_priority;
  }
  if(msg_ptr->pack_flo_id.valid == TRUE)
  {
    ctxt_ptr->bc_modify_data.packet_flow_id = msg_ptr->pack_flo_id;
  }
  if(msg_ptr->apn_ambr.valid == TRUE)
  {
    ctxt_ptr->bc_modify_data.apn_ambr = msg_ptr->apn_ambr;
  }
  if(msg_ptr->protocol_conf_opt.valid == TRUE)
  {
    ctxt_ptr->bc_modify_data.config_options = msg_ptr->protocol_conf_opt;
  }
} /* esm_save_modify_ctxt_data() */

/*==============================================================================

  FUNCTION:  esm_apply_modify_ctxt_data

==============================================================================*/
/*!
    @brief
    Apply the context modification data upon receipt of MODIFY EPS BEARER 
    CONTEXT ACCEPT message from CM

    @detail 
*/
/*============================================================================*/
void esm_apply_modify_ctxt_data
(
  struct esm_eps_bearer_context                     *ctxt_ptr,
  const  esm_modify_bearer_context_accept_T         *msg_ptr
)
{
  ASSERT(ctxt_ptr!= NULL);
  ASSERT(msg_ptr!= NULL);

  if(ctxt_ptr->eps_bearer_id != msg_ptr->eps_bearer_id)
  {
    MSG_ERROR_DS_2(SM_SUB, "ESM: Could not apply modify ctxt data duto bearer ID mismatch %d - %d",
                   ctxt_ptr->eps_bearer_id,
                   msg_ptr->eps_bearer_id);
    return;
  }

  if(ctxt_ptr->bc_modify_data.sdf_qos.valid == TRUE)
  {
    ctxt_ptr->sdf_qos = ctxt_ptr->bc_modify_data.sdf_qos;
  }

  if(ctxt_ptr->bc_modify_data.tft.valid == TRUE)
  {
    ctxt_ptr->tft = ctxt_ptr->bc_modify_data.tft; 
  }

  if(ctxt_ptr->bc_modify_data.neg_qos.valid == TRUE)
  {
    ctxt_ptr->neg_qos = ctxt_ptr->bc_modify_data.neg_qos;
  }

  if(ctxt_ptr->bc_modify_data.neg_llc_sapi.valid == TRUE)
  {
    ctxt_ptr->neg_llc_sapi = ctxt_ptr->bc_modify_data.neg_llc_sapi;
  }

  if(ctxt_ptr->bc_modify_data.radio_pri.valid == TRUE)
  {
    ctxt_ptr->radio_pri = ctxt_ptr->bc_modify_data.radio_pri;
  }

  if(ctxt_ptr->bc_modify_data.packet_flow_id.valid == TRUE)
  {
    ctxt_ptr->packet_flow_id = ctxt_ptr->bc_modify_data.packet_flow_id;
  }

  if(ctxt_ptr->bc_modify_data.apn_ambr.valid == TRUE)
  {
    ctxt_ptr->apn_ambr = ctxt_ptr->bc_modify_data.apn_ambr;
  }

  if(msg_ptr->protocol_config_options.valid == TRUE)
  {
    ctxt_ptr->config_options = msg_ptr->protocol_config_options;
  }
  else if(ctxt_ptr->bc_modify_data.config_options.valid == TRUE)
  {
    ctxt_ptr->config_options = ctxt_ptr->bc_modify_data.config_options;
  }

} /* esm_apply_modify_ctxt_data() */

/*==============================================================================

  FUNCTION:  esm_build_and_send_modify_ctxt_accept

==============================================================================*/
/*!
    @brief
    Builds and sends MODIFY EPS BEARER CONTEXT ACCEPT in LTE_EMM_DATA_REQ

    @detail
*/
/*============================================================================*/
void esm_build_and_send_modify_ctxt_accept
(
  struct esm_eps_bearer_context              *ctxt_ptr,  
  const esm_modify_bearer_context_accept_T   *msg_ptr
)
{
  
  errno_enum_type                          e_retval;
  log_lte_nas_esm_outgoing_msg_event_type  event;
  
  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));

  ASSERT(ctxt_ptr != NULL);
  ASSERT(msg_ptr != NULL);



  /* Init the MSGR header*/
  msgr_init_hdr(&out_msg_ptr->msg_hdr, MSGR_NAS_ESM, NAS_EMM_DATA_REQ);

  out_msg_ptr->emm_transaction_id = 
            esm_get_trans_id_for_data_req(ctxt_ptr->connection_id,LTE_ESM_BCM);

  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = MODIFY_EPS_BEARER_CONTEXT_ACCEPT;

  /* Fill the esm specific header*/
  out_msg_ptr->esm_out_msg.modify_eps_bearer_ctxt_accept.lte_nas_esm_hdr.bearer_id = 
                                                        msg_ptr->eps_bearer_id;

  if(msg_ptr->protocol_config_options.valid == TRUE)
  {
    out_msg_ptr->esm_out_msg.modify_eps_bearer_ctxt_accept.protocol_conf_opt = 
                                              msg_ptr->protocol_config_options;
    ctxt_ptr->config_options = msg_ptr->protocol_config_options;
  }

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent LTE_EMM_DATA_REQ carrying MODIFY_EPS_BEARER_CONTEXT_ACCEPT");
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(out_msg_ptr);

} /* esm_build_and_send_modify_ctxt_accept() */

/*==============================================================================

  FUNCTION:  esm_build_and_send_modify_ctxt_rej

==============================================================================*/
/*!
    @brief
    Builds and sends MODIFY EPS BEARER CONTEXT REJECT in LTE_EMM_DATA_REQ

    @detail
*/
/*============================================================================*/
void esm_build_and_send_modify_ctxt_rej
(
  struct esm_eps_bearer_context              *ctxt_ptr,  
  const  esm_modify_bearer_context_rej_T     *msg_ptr
)
{
  
  errno_enum_type                          e_retval;
  log_lte_nas_esm_outgoing_msg_event_type  event;
  
  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));
  ASSERT(ctxt_ptr != NULL);
  ASSERT(msg_ptr != NULL);



  /* Init the MSGR header*/
  msgr_init_hdr(&out_msg_ptr->msg_hdr, MSGR_NAS_ESM, NAS_EMM_DATA_REQ);

  out_msg_ptr->emm_transaction_id = 
            esm_get_trans_id_for_data_req(ctxt_ptr->connection_id,LTE_ESM_BCM);

  /* Fill the NAS header*/
  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = MODIFY_EPS_BEARER_CONTEXT_REJECT;

  /* Fill the esm specific header*/
  out_msg_ptr->esm_out_msg.modify_eps_bearer_ctxt_rej.lte_nas_esm_hdr.bearer_id = 
                                                        msg_ptr->eps_bearer_id;
  out_msg_ptr->esm_out_msg.modify_eps_bearer_ctxt_rej.esm_cause = msg_ptr->esm_cause;

  if(msg_ptr->protocol_config_options.valid == TRUE)
  {
    out_msg_ptr->esm_out_msg.modify_eps_bearer_ctxt_rej.protocol_conf_opt = 
                                              msg_ptr->protocol_config_options;
    ctxt_ptr->config_options = msg_ptr->protocol_config_options;
  }

  e_retval = esm_msgr_send( &out_msg_ptr->msg_hdr, sizeof(emm_data_req_T) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent LTE_EMM_DATA_REQ carrying MODIFY_EPS_BEARER_CONTEXT_REJECT");
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(out_msg_ptr);

} /* esm_build_and_send_modify_ctxt_rej() */

/*==============================================================================

  FUNCTION:  esm_bpm_build_and_send_autonomous_reject

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bpm_build_and_send_autonomous_reject() does goes here.

    @detail
    builds and sends a reject message depening on the message received
*/
/*============================================================================*/
void esm_bpm_build_and_send_autonomous_reject
(
  const lte_nas_esm_incoming_msg_type   *data_ptr,
  esm_cause_enum_T                       esm_cause
)
{
  esm_act_default_bearer_context_rej_T*     def_out_msg;
  esm_act_dedicated_bearer_context_rej_T*   ded_out_msg;
  esm_modify_bearer_context_rej_T*          modify_out_msg;

  ASSERT(data_ptr != NULL);

  switch(data_ptr->nas_hdr.msg_id)
  {
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
      def_out_msg = (esm_act_default_bearer_context_rej_T *)esm_modem_mem_alloc(sizeof(esm_act_default_bearer_context_rej_T));
	  memset((void *)def_out_msg, 0x0, 
             sizeof(esm_act_default_bearer_context_rej_T));
      def_out_msg->esm_cause.esm_cause = esm_cause;
      def_out_msg->esm_cause.valid = TRUE;
      esm_build_and_send_autonomous_act_def_ctxt_reject (&data_ptr->default_bearer_context_req,
                                                         def_out_msg); 
	  esm_modem_mem_free(def_out_msg);
      break;
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST:
	  ded_out_msg = (esm_act_dedicated_bearer_context_rej_T *)esm_modem_mem_alloc(sizeof(esm_act_dedicated_bearer_context_rej_T));	
      memset((void *)ded_out_msg, 0x0, 
             sizeof(esm_act_dedicated_bearer_context_rej_T));
      ded_out_msg->esm_cause.valid = TRUE;
      ded_out_msg->esm_cause.esm_cause = esm_cause;
      esm_build_and_send_autonomous_act_dedicated_ctxt_reject(&data_ptr->dedicated_bearer_context_req,
                                                              ded_out_msg);
	  esm_modem_mem_free(ded_out_msg);
      break;
    case PDN_CONNECTIVITY_REJECT:
      /* TS 24.301 7.3.1 UE case a - Ignore the message */
      MSG_HIGH_DS_1(SM_SUB, "ESM: Ignoring PDN_CONNECTIVITY_REJECT w/ PTI %d",
                    data_ptr->pdn_connectivity_rej.lte_nas_esm_hdr.pti);
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      break;
    case PDN_DISCONNECT_REJECT:
      /* TS 24.301 7.3.1 UE case b - Ignore the message */
      MSG_HIGH_DS_1(SM_SUB, "ESM: Ignoring PDN_DISCONNECT_REJECT w/ PTI %d",
                    data_ptr->pdn_disconnect_rej.lte_nas_esm_hdr.pti);
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      break;
    case DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST:
      /* TS 24.301 7.3.1 UE case L - Ignore the message */
      MSG_HIGH_DS_1(SM_SUB, "ESM: Ignoring DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST w/ PTI %d",
                    data_ptr->deactivate_eps_bearer_context_req.lte_nas_esm_hdr.pti);
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      break;
    case BEARER_RESOURCE_ALLOCATION_REJECT:
      /* TS 24.301 7.3.1 UE case c - Ignore the message */
      MSG_HIGH_DS_1(SM_SUB, "ESM: Ignoring BEARER_RESOURCE_ALLOCATION_REJECT w/ PTI %d",
                    data_ptr->bearer_resource_alloc_rej.lte_nas_esm_hdr.pti);
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      break;
    case BEARER_RESOURCE_MODIFICATION_REJECT:
      /* TS 24.301 7.3.1 UE case d - Ignore the message */
      MSG_HIGH_DS_1(SM_SUB, "ESM: Ignoring BEARER_RESOURCE_MODIFICATION_REJECT w/ PTI %d",
                    data_ptr->bearer_resource_modifi_rej.lte_nas_esm_hdr.pti);
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      break;
    case MODIFY_EPS_BEARER_CONTEXT_REQUEST:
      modify_out_msg = (esm_modify_bearer_context_rej_T *)esm_modem_mem_alloc(sizeof(esm_modify_bearer_context_rej_T));
      memset((void *)modify_out_msg, 0x0, 
      sizeof(esm_modify_bearer_context_rej_T));
      modify_out_msg->esm_cause.valid = TRUE;
      modify_out_msg->esm_cause.esm_cause = esm_cause;
      esm_build_and_send_autonomous_modify_ctxt_reject(&data_ptr->modify_eps_bearer_ctxt_req,
                                                       modify_out_msg);
	  esm_modem_mem_free(modify_out_msg);
      break;
    case ESM_INFORMATION_REQUEST:
      /* TS 24.301 7.3.1 UE case e */
      if((data_ptr->esm_info_req.lte_nas_esm_hdr.pti == ESM_INVALID_PTI) ||
         (data_ptr->esm_info_req.lte_nas_esm_hdr.pti == ESM_RESERVED_PTI))
      {
        MSG_HIGH_DS_1(SM_SUB, "ESM: Ignoring ESM_INFORMATION_REQUEST w/ PTI %d",
                      data_ptr->esm_info_req.lte_nas_esm_hdr.pti);
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
      }
      else
      {
        esm_send_esm_status(&data_ptr->esm_info_req.lte_nas_esm_hdr,
                             esm_cause);
      }
      break;
    case ESM_STATUS:
      /* TS 24.301 7.3.1 UE case m - Ignore the message */
      MSG_HIGH_DS_1(SM_SUB, "ESM: Ignoring ESM_STATUS w/ PTI %d",
                    data_ptr->mt_esm_status.lte_nas_esm_hdr.pti);
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      break;
    default:
      /* TS 24.301 7.3.1 UE case m - Ignore the message */
      MSG_ERROR_DS_1(SM_SUB, "ESM: Ignoring unknown message - %d", data_ptr->nas_hdr.msg_id);
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      break;
  }
} /* esm_bpm_build_and_send_autonomous_reject */

/*==============================================================================

  FUNCTION:  esm_build_and_send_cm_bearer_resrc_mod_reject

==============================================================================*/
/*!
    @brief
    Build and send 

    @detail
*/
/*============================================================================*/
void esm_build_and_send_cm_bearer_resrc_mod_reject
(
  word                                              sdf_id,
  const  lte_nas_esm_bearer_resource_modifi_rej    *msg_ptr,
  lte_nas_local_cause_enum_T                        local_cause,
  byte                                              lbi
)
{
  
  errno_enum_type                               e_retval;
  log_lte_nas_esm_outgoing_msg_event_type       event;
  cm_bearer_context_modify_reject_ind_s_type   *cm_bc_mod_rej_ptr =
                                               (cm_bearer_context_modify_reject_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_bearer_context_modify_reject_ind_s_type));



  msgr_init_hdr(&cm_bc_mod_rej_ptr->msg_hdr, MSGR_NAS_ESM, 
                MM_CM_BEARER_CONTEXT_MODIFY_REJECT_IND);

  cm_bc_mod_rej_ptr->esm_hdr.connection_id = ESM_INVALID_CON_ID;
  cm_bc_mod_rej_ptr->esm_hdr.sdf_id = sdf_id;
  cm_bc_mod_rej_ptr->esm_hdr.lbi = lbi;

  if((msg_ptr != NULL) && (msg_ptr->lte_nas_hdr.valid == FALSE))
  {
    esm_send_esm_status(&msg_ptr->lte_nas_esm_hdr, 
                         msg_ptr->lte_nas_hdr.esm_decode_fail_reason);
  }

  if((msg_ptr != NULL) && (msg_ptr->lte_nas_hdr.valid == TRUE))
  {  
    cm_bc_mod_rej_ptr->esm_cause = msg_ptr->esm_cause;
    cm_bc_mod_rej_ptr->protocol_config_options = msg_ptr->protocol_conf_opt;
#ifdef FEATURE_LTE_REL10
    cm_bc_mod_rej_ptr->backoff_timer = msg_ptr->backoff_timer;
#endif
  }
  else
  {
    cm_bc_mod_rej_ptr->esm_local_cause.valid = TRUE;
    cm_bc_mod_rej_ptr->esm_local_cause.local_cause = local_cause;
  }

  e_retval = esm_msgr_send( (msgr_hdr_s *)cm_bc_mod_rej_ptr, sizeof(cm_bearer_context_modify_reject_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: Send MM_CM_BEARER_CONTEXT_MODIFY_REJECT_IND");
  event.esm_out_msg = (byte)MM_CM_BEARER_CONTEXT_MODIFY_REJECT_IND;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(cm_bc_mod_rej_ptr);

}/* esm_build_and_send_cm_bearer_resrc_mod_reject */

/*==============================================================================

  FUNCTION:  esm_build_and_send_esm_info_rsp

==============================================================================*/
/*!
    @brief
    This function builds and sends ESM INFO RSP messge in EMM DATA REQ

    @detail    
*/
/*============================================================================*/
void esm_build_and_send_esm_info_rsp 
(
  struct esm_bearer_resource_proc   *proc_ptr
)
{
  errno_enum_type                            e_retval;
  
  log_lte_nas_esm_outgoing_msg_event_type    event;
  
  emm_data_req_T* out_msg_ptr = (emm_data_req_T *)esm_modem_mem_alloc(sizeof(emm_data_req_T));

  ASSERT(proc_ptr != NULL);
  ASSERT(proc_ptr->pending_msg_ptr != NULL);



  msgr_init_hdr(&out_msg_ptr->msg_hdr, MSGR_NAS_ESM, NAS_EMM_DATA_REQ);

  out_msg_ptr->emm_transaction_id = 
        esm_get_trans_id_for_data_req(ESM_RESERVED_CONNECTION_ID, LTE_ESM_BCM);

  out_msg_ptr->esm_out_msg.nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->esm_out_msg.nas_hdr.msg_id = ESM_INFORMATION_RESPONSE;

  out_msg_ptr->esm_out_msg.esm_info_resp.lte_nas_esm_hdr.bearer_id = ESM_INVALID_BEARER_ID;
  out_msg_ptr->esm_out_msg.esm_info_resp.lte_nas_esm_hdr.pti = proc_ptr->pti;

  /* Always send APN in the ESM INFO RSP */
  out_msg_ptr->esm_out_msg.esm_info_resp.access_point_name = 
    proc_ptr->pending_msg_ptr->esm_out_msg.pdn_connectivity_req.access_point_name;

  /* Send PCO in the ESM INFO RSP only when PCO cipher flag is set */
  if(proc_ptr->pco_ciphering_flag == TRUE)
  {
    out_msg_ptr->esm_out_msg.esm_info_resp.protocol_conf_opt = 
      proc_ptr->pending_msg_ptr->esm_out_msg.pdn_connectivity_req.protocol_conf_opt;
  }

  e_retval = esm_msgr_send((msgr_hdr_s *)&out_msg_ptr->msg_hdr, sizeof(emm_data_req_T)); 
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: Send NAS_EMM_DATA_REQ carrying ESM_INFORMATION_RESPONSE");
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);

  esm_modem_mem_free(out_msg_ptr);

} /* esm_build_and_send_esm_info_rsp */

/*==============================================================================

  FUNCTION:  esm_send_esm_status

==============================================================================*/
/*!
    @brief
    This function builds and sends ESM STATUS messge in EMM DATA REQ

    @detail    
*/
/*============================================================================*/
void esm_send_esm_status 
(
  const lte_nas_esm_hdr_type        *esm_hdr_ptr,
  esm_cause_enum_T                   esm_cause
)
{
  errno_enum_type                            e_retval;
  emm_msgr_cmd_type                          *emm_cmd_ptr = NULL;
  log_lte_nas_esm_outgoing_msg_event_type    event;
  lte_nas_esm_outgoing_msg_type             *out_msg_ptr = NULL;

  ASSERT(esm_hdr_ptr != NULL);

  /*allocate memory on heap*/
  emm_cmd_ptr = (emm_msgr_cmd_type*)esm_modem_mem_alloc(sizeof(emm_msgr_cmd_type));

  /* Init */
  out_msg_ptr = &emm_cmd_ptr->emm_data_req.esm_out_msg;

  msgr_init_hdr(&emm_cmd_ptr->msg_hdr, MSGR_NAS_ESM, NAS_EMM_DATA_REQ);

  emm_cmd_ptr->emm_data_req.emm_transaction_id = 
        esm_get_trans_id_for_data_req(ESM_RESERVED_CONNECTION_ID, LTE_ESM_BCM);

  out_msg_ptr->nas_hdr.pd = EPS_SESSION_MANAGEMENT_MESSAGES;
  out_msg_ptr->nas_hdr.msg_id = ESM_STATUS;
  out_msg_ptr->mo_esm_status.lte_nas_esm_hdr = *esm_hdr_ptr;
  out_msg_ptr->mo_esm_status.esm_cause = esm_cause;

  e_retval = esm_msgr_send( &emm_cmd_ptr->msg_hdr, sizeof(emm_msgr_cmd_type) ); 

  ASSERT(e_retval == E_SUCCESS);
  
  MSG_HIGH_DS_3(SM_SUB, "ESM: Sending ESM_STATUS w/ PTI - %u, BID - %u, Cause %u",
                out_msg_ptr->mo_esm_status.lte_nas_esm_hdr.pti,
                out_msg_ptr->mo_esm_status.lte_nas_esm_hdr.bearer_id,
                esm_cause);
  event.esm_out_msg = (byte)NAS_EMM_DATA_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);

  /*free heap memory*/
  esm_modem_mem_free(emm_cmd_ptr);
} /* esm_send_esm_status */

/*==============================================================================

  FUNCTION:  esm_validate_bearer_resrc_mod_req_bearer_id

==============================================================================*/
/*!
    @brief
    This function validates the bearer ID in Bearer Resource Modification Req 
    message.

    @detail
    
*/
/*============================================================================*/
boolean esm_validate_bearer_resrc_mod_req_bearer_id
(
  byte                   bearer_id,
  const struct esm_bpm  *mgr_ptr
)
{
  esm_bearer_context_info_shared_T    bc_info_shared;

  ASSERT(mgr_ptr != NULL);

  memset((void *)&bc_info_shared,0x0,sizeof(esm_bearer_context_info_shared_T));

  mgr_ptr->find_bearer_context_ptr(bearer_id, &bc_info_shared);

#ifdef FEATURE_LTE_REL9
  /* Check if bearer context is existing */
  if((bc_info_shared.bearer_id_exist == TRUE) && 
     (bc_info_shared.emc_bc == FALSE))
  {
    return TRUE;
  }
  else
  {
    MSG_HIGH_DS_2(SM_SUB, "ESM: Invalid BEARER RESRC MOD REQ - BID %d EMC SRV Status %d",
                  bearer_id,
                  bc_info_shared.emc_bc);
    return FALSE;
  }
#else
  if(bc_info_shared.bearer_id_exist == TRUE)
  {
    return TRUE;
  }
  else
  {
    MSG_HIGH_DS_1(SM_SUB, "ESM: Bearer ID in BEARER RESRC MOD REQ is unknown - %u",
                  bearer_id);
    return FALSE;
  }
#endif
} /* esm_validate_bearer_resrc_mod_req_bearer_id */

/*==============================================================================*/
/*!
    @brief
    This function deactivates the bearer context if needed 

    @detail
*/
/*============================================================================*/
void esm_chk_ds_explicit_deact
(
  struct esm_bearer_resource_proc  *proc_ptr
)
{
  ASSERT(proc_ptr != NULL);

  MSG_LOW_DS_0(SM_SUB, "ESM: Chk if DS requests explicit deact");

  if((proc_ptr->esm_local_cause.valid == TRUE) &&
     (proc_ptr->esm_local_cause.local_cause == DS_EXPLICIT_DEACT))
  {
    esm_cause_T             esm_cause;
    lte_nas_local_cause_T   local_cause;

    esm_cause.valid = FALSE;
    esm_cause.esm_cause = NAS_ESM_NO_FAILURE;
    local_cause.valid = TRUE;
    local_cause.local_cause = DS_EXPLICIT_DEACT;

    MSG_HIGH_DS_1(SM_SUB, "ESM: Deact bearer context %d uopn DS request", proc_ptr->lbi);

    proc_ptr->request_esm_to_deactivate_ctxts_func_ptr(local_cause, 
                                                       esm_cause,
                                                       proc_ptr->lbi);
  }

}/* esm_chk_ds_explicit_deact */


/*==============================================================================*/
/*!
    @brief
    This function sends ESM NOTIFICATION to CM 

    @detail
*/
/*============================================================================*/
void esm_build_and_send_esm_notification(void)
{
  
  errno_enum_type                               e_retval;
  cm_esm_notification_ind_s_type* cm_esm_notification_msg_ptr = 
                                  (cm_esm_notification_ind_s_type *)esm_modem_mem_alloc(sizeof(cm_esm_notification_ind_s_type));



  msgr_init_hdr(&cm_esm_notification_msg_ptr->msg_hdr, MSGR_NAS_ESM, 
                MM_CM_ESM_NOTIFICATION_IND); /* CM dependancy */

  cm_esm_notification_msg_ptr->notification_ind = SRVCC_HO_CANCELLED;

  e_retval = esm_msgr_send( (msgr_hdr_s *)cm_esm_notification_msg_ptr, sizeof(cm_esm_notification_ind_s_type) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: Send MM_CM_ESM_NOTIFICATION ");
  esm_modem_mem_free(cm_esm_notification_msg_ptr);
}

/*===========================================================================

FUNCTION   ESM_MODEM_MEM_ALLOC_DEBUG

DESCRIPTION
  This function allocates dynamic memory using modem heap. This function simply
  calls modem_mem_alloc and returns pointer to the allocated memory.  
  The memory allocated is initialized with zeros.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void *esm_modem_mem_alloc_debug
(
  size_t               size,
  char                *objname
)
{
  void *mem_ptr = NULL;

  mem_ptr = modem_mem_alloc(size, MODEM_MEM_CLIENT_NAS);
  sm_check_for_null_ptr((void*)mem_ptr);

  /* Init the allocated memory */
  memset(mem_ptr, 0x0, size);

  return mem_ptr;

} /* esm_modem_mem_alloc() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION   ESM_MODEM_MEM_FREE_DEBUG

DESCRIPTION
   This function returuns the specified memory back to the Heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void esm_modem_mem_free_debug
(
  void                *ptr,
  char                *objname
)
{
  if(ptr != NULL)
  {
    /* Deallocate the memory to heap */
    modem_mem_free(ptr, MODEM_MEM_CLIENT_NAS);

  }
  else
  {
    MSG_ERROR_DS_0(SM_SUB, "ESM: Modem mem free - NULL pointer");
  }

} /* esm_modem_mem_free_debug */

/*==============================================================================*/
/*!
    @brief
    This function sends 1xcsfb call req to EMM 

    @detail
*/
/*============================================================================*/
void esm_build_and_send_1xcsfb_call_req(esm_1xCSFB_call_req_T* msg_ptr,struct esm_1xcsfbm* mgr_ptr)
{

  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;
  
  emm_1xCSFB_esr_call_req_T* esr_call_req = (emm_1xCSFB_esr_call_req_T *)esm_modem_mem_alloc(sizeof(emm_1xCSFB_esr_call_req_T));

  ASSERT(msg_ptr != NULL); 

  msgr_init_hdr(&(esr_call_req->msg_hdr),MSGR_NAS_ESM,NAS_EMM_1XCSFB_ESR_CALL_REQ);

  /* Purposefully using ESM_INVALID_CON_ID for 1XCSFB calls as they are different from calls originated from DS*/
  esr_call_req->trans_id = esm_get_trans_id_for_data_req(ESM_INVALID_CON_ID,LTE_ESM_BCM);
  mgr_ptr->trans_id = esr_call_req->trans_id;
  mgr_ptr->csfb_call_id = msg_ptr->csfb_call_id;
  esr_call_req->csfb_service_type = msg_ptr->csfb_service_type;
  esr_call_req->partial_esr = msg_ptr->partial_esr;
  e_retval = esm_msgr_send( (msgr_hdr_s*)esr_call_req, sizeof(emm_1xCSFB_esr_call_req_T) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent  NAS_EMM_1XCSFB_ESR_CALL_REQ");
  event.esm_out_msg = (byte)NAS_EMM_1XCSFB_ESR_CALL_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(esr_call_req);
}


/*==============================================================================*/
/*!
    @brief
    This function sends 1xcsfb call rsp to CM 

    @detail
*/
/*============================================================================*/
void esm_build_and_send_1xcsfb_call_rsp(emm_1xCSFB_esr_call_rsp_T* msg_ptr,struct esm_1xcsfbm* mgr_ptr)
{
  
  errno_enum_type                               e_retval;
  log_lte_nas_esm_outgoing_msg_event_type       event;
  cm_1xCSFB_call_rsp* cm_1xcsfb_call_rsp_ptr = (cm_1xCSFB_call_rsp *)esm_modem_mem_alloc(sizeof(cm_1xCSFB_call_rsp));

  ASSERT(msg_ptr != NULL); 



  msgr_init_hdr(&cm_1xcsfb_call_rsp_ptr->msg_hdr, MSGR_NAS_ESM, 
                MM_CM_1XCSFB_CALL_RSP);

  cm_1xcsfb_call_rsp_ptr->csfb_call_id = mgr_ptr->csfb_call_id;
  cm_1xcsfb_call_rsp_ptr->cm_cc_reject = msg_ptr->reject;

  e_retval = esm_msgr_send( (msgr_hdr_s *)cm_1xcsfb_call_rsp_ptr, sizeof(cm_1xCSFB_call_rsp) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: Send MM_CM_1XCSFB_CALL_RSP ");
  event.esm_out_msg = (byte)MM_CM_1XCSFB_CALL_RSP;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(cm_1xcsfb_call_rsp_ptr);
}

/*==============================================================================*/
/*!
    @brief
    This function sends 1xcsfb call abort req to EMM 

    @detail
*/
/*============================================================================*/
void esm_build_and_send_1xcsfb_abort_req(esm_1xCSFB_abort_req_T* msg_ptr)
{

  errno_enum_type                         e_retval;
  log_lte_nas_esm_outgoing_msg_event_type event;
  
  emm_1xCSFB_esr_call_abort_req_T* esr_call_abort_req_ptr = (emm_1xCSFB_esr_call_abort_req_T *)esm_modem_mem_alloc(sizeof(emm_1xCSFB_esr_call_abort_req_T));

  ASSERT(msg_ptr != NULL); 

  
  msgr_init_hdr(&(esr_call_abort_req_ptr->msg_hdr),MSGR_NAS_ESM,NAS_EMM_1XCSFB_ESR_CALL_ABORT_REQ);

  esr_call_abort_req_ptr->trans_id = esm_get_trans_id_for_data_req(ESM_INVALID_CON_ID,LTE_ESM_BCM);
  //esr_call_abort_req_ptr->csfb_call_id = msg_ptr->csfb_call_id;

  e_retval = esm_msgr_send( (msgr_hdr_s*)esr_call_abort_req_ptr, sizeof(emm_1xCSFB_esr_call_abort_req_T) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM sent  NAS_EMM_1XCSFB_ESR_CALL_ABORT_REQ");
  event.esm_out_msg = (byte)NAS_EMM_1XCSFB_ESR_CALL_ABORT_REQ;
  event_report_payload(EVENT_LTE_EMM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(esr_call_abort_req_ptr);
}
/*==============================================================================*/
/*!
    @brief
    This function sends 1xcsfb call abort rsp to CM 

    @detail
*/
/*============================================================================*/
void esm_build_and_send_1xcsfb_call_abort_rsp(emm_1xCSFB_esr_call_abort_rsp_T* msg_ptr,struct esm_1xcsfbm* mgr_ptr)
{
  
  errno_enum_type                               e_retval;
  log_lte_nas_esm_outgoing_msg_event_type       event;
  cm_1xCSFB_abort_rsp* cm_1xcsfb_call_abort_rsp_ptr = 
                       (cm_1xCSFB_abort_rsp *)esm_modem_mem_alloc(sizeof(cm_1xCSFB_abort_rsp));

  ASSERT(msg_ptr != NULL); 



  msgr_init_hdr(&cm_1xcsfb_call_abort_rsp_ptr->msg_hdr, MSGR_NAS_ESM, 
                MM_CM_1XCSFB_ABORT_RSP);

  cm_1xcsfb_call_abort_rsp_ptr->csfb_call_id = mgr_ptr->csfb_call_id;

  e_retval = esm_msgr_send( (msgr_hdr_s *)cm_1xcsfb_call_abort_rsp_ptr, sizeof(cm_1xCSFB_abort_rsp) );
  ASSERT(e_retval == E_SUCCESS);
  MSG_HIGH_DS_0(SM_SUB, "ESM: Send MM_CM_1XCSFB_ABORT_RSP ");
  event.esm_out_msg = (byte)MM_CM_1XCSFB_ABORT_RSP;
  event_report_payload(EVENT_LTE_CM_OUTGOING_MSG,sizeof(event), (void*)&event);
  esm_modem_mem_free(cm_1xcsfb_call_abort_rsp_ptr);
}

#ifdef FEATURE_LTE_REL9
/*===========================================================================

FUNCTION  ESM_CHK_PDN_CONN_ALLOWANCE

DESCRIPTION
  This function checks if PDN CONN procedure is allowed

DEPENDENCIES
  None

RETURN VALUE
  TRUE   PDN CONN procedure is allowed
  FALSE  PDN CONN procedure is not allowed

SIDE EFFECTS
  None

===========================================================================*/
boolean  esm_chk_pdn_conn_allowance
(
  esm_pdn_connectivity_req_T  *msg_ptr,
  const struct esm_bpm        *mgr_ptr
)
{
  lte_nas_emc_srv_status_type  emc_srv_status;  
  boolean  pdn_conn_allowed = TRUE;  

  ASSERT(msg_ptr != NULL);
  ASSERT(mgr_ptr != NULL);

  if((msg_ptr->request_type == ESM_EMERGENCY) &&
     (emm_get_3gpp_rel_ver_per_sub((sys_modem_as_id_e_type)sm_as_id) < LTE_3GPP_REL9))
  {
    MSG_HIGH_DS_1(SM_SUB, "ESM: Emergency bearer service not supported on Rel %d",
                  emm_get_3gpp_rel_ver_per_sub((sys_modem_as_id_e_type)sm_as_id));
    return FALSE;
  }

  emc_srv_status = esm_get_emc_srv_status();

  switch(emc_srv_status)
  {
    case LTE_NAS_EMC_SRV_ONLY:
      pdn_conn_allowed = FALSE;
      break;
    case LTE_NAS_EMC_SRV_PENDING:
    case LTE_NAS_EMC_SRV_EXIST:
      if(msg_ptr->request_type == ESM_EMERGENCY)
      {
        pdn_conn_allowed = FALSE;
      }
      break;
    default:
      break;
  }

  MSG_HIGH_DS_3(SM_SUB, "ESM: PDN CONN ALLOWANCE %d - REQ Type %d, EMC SRV Status %d",
                pdn_conn_allowed,
                msg_ptr->request_type,
                emc_srv_status);

  /* Update EMC service status. Do not update EMM yet. */
  if((pdn_conn_allowed == TRUE) && (msg_ptr->request_type == ESM_EMERGENCY))
  {
    esm_set_emc_srv_status(LTE_NAS_EMC_SRV_PENDING);
  }

  return pdn_conn_allowed;

} /* esm_chk_pdn_conn_allowance */

/*===========================================================================

FUNCTION  ESM_PDN_CONN_FAILURE_UPDATE_EMC_SRV_STATUS

DESCRIPTION
  This function updates emergency service status upon PDN CONN failure

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void  esm_pdn_conn_failure_update_emc_srv_status
(
  struct esm_bearer_resource_proc  *proc_ptr
)
{
  ASSERT(proc_ptr != NULL);

  if(proc_ptr->pending_msg_ptr->esm_out_msg.pdn_connectivity_req.req 
     == ESM_EMERGENCY)
  {
    esm_set_emc_srv_status(LTE_NAS_EMC_SRV_NONE);
  }
}/* esm_pdn_conn_failure_update_emc_srv_status */


/*===========================================================================

FUNCTION  ESM_BEARER_SETUP_FAILURE_UPDATE_EMC_SRV_STATUS

DESCRIPTION
  This function updates emergency service status upon emergency bearer setup failure

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void  esm_bearer_setup_failure_update_emc_srv_status
(
  const esm_act_default_bearer_context_rej_T* msg_ptr 
)
{
  ASSERT(msg_ptr != NULL);

  if((esm_get_emc_srv_status() == LTE_NAS_EMC_SRV_PENDING) &&
     (msg_ptr->emc_bc == TRUE))
  {
    esm_set_emc_srv_status(LTE_NAS_EMC_SRV_NONE);
  }
}

#endif

/*===========================================================================

FUNCTION  ESM_GET_CONNECTION_ID

DESCRIPTION
  This function calcuates the connection ID using bearer ID passed as param

DEPENDENCIES
  None

RETURN VALUE
  byte
 
SIDE EFFECTS
  None

===========================================================================*/
byte esm_get_connection_id
(
  byte bearer_id
)
{
  /* This is the new way of allocating the context. The idea is to create a direct mapping between 
  the bearer Id and the connection Id. Givn a bearer id, the connection id can be mapped and vice versa. e.g
  connection_id 0 maps to bearer Id 5, connection id 1 maps to bearer id 6 etc..So, there is a fixed mapping 
  between the connection id and the bearer idi. This can't be violated. If it is violated it is an error*/
  return (bearer_id - ESM_STARTING_BEARER_ID);
}


#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION  ESM_TAG_AS_ID

DESCRIPTION
  This function tags as_ids to the primitive before calling msgr_send.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  esm_tag_as_id
(
  msgr_hdr_struct_type* msg_ptr
)
{
  switch(msg_ptr->id)
  {
    /* Messages to CM */
    case MM_CM_ACT_DEFAULT_BEARER_CONTEXT_REQUEST_IND:
    case MM_CM_DEACT_BEARER_CONTEXT_REQUEST_IND:
    case MM_CM_ACT_DRB_RELEASED_IND:
    case MM_CM_PDN_CONNECTIVITY_PACKED_REQUEST_IND:
    case MM_CM_DRB_SETUP_IND:
    case MM_CM_PDN_CONNECTIVITY_REJECT_IND:
    case MM_CM_DRB_REESTABLISH_REJECT_IND :
    case MM_CM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND:
    case MM_CM_BEARER_RESOURCE_ALLOC_REJECT_IND:
    case MM_CM_BEARER_RESOURCE_ALLOC_FAILURE_IND:
    case MM_CM_GET_PDN_CONNECTIVITY_REQUEST_IND:
    case MM_CM_PDN_DISCONNECT_REJECT_IND:
    case MM_CM_PDN_CONNECTIVITY_FAILURE_IND:
    case MM_CM_MODIFY_BEARER_CONTEXT_REQUEST_IND:
    case MM_CM_BEARER_CONTEXT_MODIFY_REJECT_IND:
    case MM_CM_ESM_NOTIFICATION_IND:
    case MM_CM_1XCSFB_CALL_RSP:
    case MM_CM_1XCSFB_ABORT_RSP:
     /* Log to SM buffer */
      esm_add_message_to_debug_buffer(msg_ptr->id, FROM_ESM , (sys_modem_as_id_e_type)sm_as_id);
      msgr_set_hdr_inst(msg_ptr,(uint8)SYS_AS_ID_TO_INST_ID((sys_modem_as_id_e_type)sm_as_id));
      break;

    /* Messages to EMM */
    case NAS_EMM_DATA_REQ:
    case NAS_EMM_SERVICE_REQ:
    case NAS_EMM_DETACH_CMD:
    case NAS_EMM_EPS_BEARER_STATUS_CMD:
    case NAS_EMM_1XCSFB_ESR_CALL_REQ:
    case NAS_EMM_1XCSFB_ESR_CALL_ABORT_REQ:
    case NAS_EMM_EMC_SRV_STATUS_CMD:
      /* Log to SM buffer */
      esm_add_message_to_debug_buffer(msg_ptr->id, FROM_ESM , (sys_modem_as_id_e_type)sm_as_id);
      msgr_set_hdr_inst(msg_ptr,(uint8)SYS_AS_ID_TO_INST_ID((sys_modem_as_id_e_type)sm_as_id));
      break;

    default:
      msgr_set_hdr_inst(msg_ptr,(uint8)SYS_AS_ID_TO_INST_ID((sys_modem_as_id_e_type)sm_as_id));
      MSG_ERROR_DS_1(SM_SUB, "ESM: Unknown message to tag UMID %d", msg_ptr->id);
      break;
  }
}
#endif

/*===========================================================================

FUNCTION  ESM_MSGR_SEND

DESCRIPTION
  This function logs the message before calling msgr_send.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

errno_enum_type esm_msgr_send
(
  msgr_hdr_struct_type*     msg_ptr,
  uint32                    msg_len
)
{
  ASSERT( msg_ptr != NULL );
#ifdef FEATURE_DUAL_SIM
  esm_tag_as_id(msg_ptr);
#endif

  return(msgr_send(msg_ptr,msg_len)); 
}


/*===========================================================================

FUNCTION  ESM_COPY_ESM_CMD_MSG

DESCRIPTION
  This function copies the individual struct member in the source esm_msg union
  pointer to the destination pointer by checking the message id in the header.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void esm_copy_esm_cmd_msg
(
  esm_msg*     srcPtr,
  esm_msg*     destPtr
)
{
  ASSERT(srcPtr != NULL);
  ASSERT(destPtr != NULL);

  /* All messages available in the esm_msg union @ api/esm.h should be present below */
  switch(srcPtr->msg_hdr.id)
  {
    case NAS_ESM_BEARER_RESOURCE_ALLOC_REQ:
      destPtr->esm_bearer_resource_alloc_req = 
                                         srcPtr->esm_bearer_resource_alloc_req;
      break;
    case NAS_ESM_PDN_CONNECTIVTY_REQ:
      destPtr->esm_pdn_connectivity_req = srcPtr->esm_pdn_connectivity_req;
      break;
    case NAS_ESM_PDN_DISCONNECT_REQ:
      destPtr->esm_pdn_disconnect_req = srcPtr->esm_pdn_disconnect_req;
      break;
    case NAS_ESM_BEARER_RESOURCE_ALLOC_ABORT_REQ:
      destPtr->esm_bearer_resource_alloc_abort_req = 
                                   srcPtr->esm_bearer_resource_alloc_abort_req;
      break;
    case NAS_ESM_PDN_CONNECTIVITY_ABORT_REQ:
      destPtr->esm_pdn_connectivity_abort_req = 
                                        srcPtr->esm_pdn_connectivity_abort_req;
      break;
    case NAS_ESM_DRB_REESTABLISH_REQ:
      destPtr->esm_drb_reestablish_req = 
                                         srcPtr->esm_drb_reestablish_req;
      break;
    case NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT_CNF:
      destPtr->esm_act_default_bearer_context_accept = 
                                 srcPtr->esm_act_default_bearer_context_accept;
      break;
    case NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_REJ_IND:
      destPtr->esm_act_default_bearer_context_rej = 
                                    srcPtr->esm_act_default_bearer_context_rej;
      break;
    case NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_ACCEPT_CNF:
      destPtr->esm_act_dedicated_bearer_context_accept = 
                               srcPtr->esm_act_dedicated_bearer_context_accept;
      break;
    case NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_REJ_IND:
      destPtr->esm_act_dedicated_bearer_context_rej = 
                                  srcPtr->esm_act_dedicated_bearer_context_rej;
      break;
    case NAS_ESM_MODIFY_BEARER_CONTEXT_REJ_IND:
      destPtr->esm_modify_bearer_context_rej = 
                                         srcPtr->esm_modify_bearer_context_rej;
      break;
    case NAS_ESM_MODIFY_BEARER_CONTEXT_ACCEPT_CNF:
      destPtr->esm_modify_bearer_context_accept = 
                                      srcPtr->esm_modify_bearer_context_accept;
      break;
    case NAS_ESM_BEARER_RESOURCE_MODIFICATION_REQ:
      destPtr->esm_bearer_resource_modification_req = 
                                  srcPtr->esm_bearer_resource_modification_req;
      break;
    case NAS_ESM_DATA_IND:
      destPtr->esm_data_ind = srcPtr->esm_data_ind;
      break;
    case NAS_ESM_FAILURE_IND:
      destPtr->esm_failure_ind = srcPtr->esm_failure_ind;
      break;
    case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
      destPtr->esm_sig_connection_released_ind = 
                                       srcPtr->esm_sig_connection_released_ind;
      break;
    case NAS_ESM_ACTIVE_EPS_IND:
      destPtr->esm_active_eps_ind = srcPtr->esm_active_eps_ind;
      break;
    case NAS_ESM_DETACH_IND:
      destPtr->esm_detach_ind = srcPtr->esm_detach_ind;
      break;
    case NAS_ESM_EPS_BEARER_STATUS_IND:
      destPtr->esm_eps_bearer_status_ind = srcPtr->esm_eps_bearer_status_ind;
      break;
    case NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND:
      destPtr->esm_get_pdn_connectivity_req = 
                                          srcPtr->esm_get_pdn_connectivity_req;
      break;
    case NAS_ESM_TIMER_EXPIRED_TMRI:
      destPtr->esm_timer_expired_ind = srcPtr->esm_timer_expired_ind;
      break;
    case NAS_ESM_GET_ISR_STATUS_IND:
      destPtr->esm_isr_status_ind = srcPtr->esm_isr_status_ind;
      break;
    case NAS_ESM_ISR_STATUS_CHANGE_IND:
      destPtr->esm_isr_status_change_ind = srcPtr->esm_isr_status_change_ind;
      break;
    case NAS_ESM_PDN_CONNECT_PACKED_CNF:
      destPtr->esm_pdn_connectivity_pkd_cnf = 
                                          srcPtr->esm_pdn_connectivity_pkd_cnf;
      break;
    case NAS_ESM_1XCSFB_CALL_REQ:
      destPtr->esm_1xCSFB_call_req = srcPtr->esm_1xCSFB_call_req;
      break;
    case NAS_ESM_1XCSFB_ABORT_REQ:
      destPtr->esm_1xCSFB_abort_req = srcPtr->esm_1xCSFB_abort_req;
      break;
    case NAS_ESM_1XCSFB_ESR_CALL_RSP:
      destPtr->emm_1xCSFB_esr_call_rsp = srcPtr->emm_1xCSFB_esr_call_rsp;
      break;
    case NAS_ESM_1XCSFB_ESR_CALL_ABORT_RSP:
      destPtr->emm_1xCSFB_esr_call_abort_rsp = 
                                         srcPtr->emm_1xCSFB_esr_call_abort_rsp;
      break;
    case NAS_ESM_UNBLOCK_ALL_APNS_IND:
      destPtr->esm_unblock_all_apns_ind = srcPtr->esm_unblock_all_apns_ind;
      break;
    default:
      MSG_ERROR_DS_1(SM_SUB, "ESM: Unknown message - %d", srcPtr->msg_hdr.id);
      break;
  }
}

/*===========================================================================

FUNCTION  esm_update_bcm_info

DESCRIPTION
  This function updates the BCM info for one group of bearer contexts.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void esm_update_bcm_info(esm_bcm_T* mgr_ptr,byte linked_bearer_id,bearer_ctrl_mode_T bcm)
{

  byte index = 0;
  esm_bc_state_T   cur_state;

  MSG_HIGH_DS_2(SM_SUB,"BCM info for EPS ID %d is set to %d",linked_bearer_id,bcm);

  for(index=0; index < ESM_MAX_EPS_BEARERS; index++)
  {
    // BCM info should be updated for the complete group
    // TS 24.008: Section 9.5.2.2 Protocol configuration options 
    // This IE is also included to indicate the selected Bearer Control Mode to be applied as well as the network support for 
    // Local IP address in TFTs for all active PDP contexts sharing the same PDP Address and APN.
    if((mgr_ptr->esm_bc[index] != NULL)&&((mgr_ptr->esm_bc[index]->default_bearer_ctxt == FALSE && mgr_ptr->esm_bc[index]->lbi == linked_bearer_id) ||
		(mgr_ptr->esm_bc[index]->default_bearer_ctxt == TRUE && mgr_ptr->esm_bc[index]->eps_bearer_id == linked_bearer_id)))
    {
      cur_state = mgr_ptr->esm_bc[index]->eps_bearer_state_ptr->state();
      if(cur_state != ESM_BC_INACTIVE)
      {
        mgr_ptr->esm_bc[index]->bcm = bcm;
      }
      
    }
  }
}
#endif /*FEATURE_LTE*/
