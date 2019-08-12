
/*===========================================================================

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

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_tlb_handler.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $ 
 
when       who     what
--------   ---     ----------------------------------------------------------
08/05/15   c_cogolt    Added check for incoming tlb message length
04/22/10   zren    Updated to use global structure for NAS outgoing message
09/25/09   hnam    Added TLB support
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "emm.h"
#include "rex.h"
#include "emm_state.h"
#include "emm_database.h"
#include "msgr.h"
#include "msgr_lte.h"
#include "lte_nas_common_v.h" 
#include "lte_nas_local_cause.h"
#include "emm_update_lib.h"
#include "lte_nas.h"
#include "lte_nas_msg_parse.h"
#include "emm_service_request_lib.h"
#include "err.h"
#include "naslog_v.h"
#include "event.h"
#include "event_defs.h"
#include "emm_utility.h"
#include "emm_connection_handler.h"
#include "lte_nas_security_info.h"
#include "lte_tlb.h"
#include "lte_rrc_ext_msg.h"
#include "comdef.h"
#include "lte_nas_msg_parse.h"
#include "emm_security.h"
#include "msgr_lte.h"
#include "lte_nas_tlb_message_type.h"
#include "msgr_nas.h"
#include "lte_nas_tlb.h"
#include "stringl.h"
/*===========================================================================
FUNCTION    EMM_PROCESS_OUTGOING_TLB_MESSAGE

DESCRIPTION
  This function puts the incoming RAW buffer from TLB, into the correct 
  'C' structure that MSG_LIB identifies and then calls emm_Send_message, 
  to take care of encoding and hence routing this message to RRC

DEPENDENCIES
  None

RETURN VALUE
  NONE (Not required, as TLB messages does not require acknowledgement from RRC)

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_process_outgoing_tlb_message
(
  void                   *cmd_ptr, //Changed the i/p type to void*, to be compatible to function pointer
  emm_ctrl_data_type     *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp  **mm_emm_dispatcher_fn_ptr
)
{
  lte_nas_ul_security_info_type       ul_security_info;

  tlb_cmd_type *emm_cmd_ptr; //Initialize the lte_tlb_ul_info_req_s pointer 

  boolean      dummy_value = FALSE;

  emm_cmd_ptr = (tlb_cmd_type*)cmd_ptr; //typecast incoming cmd_ptr to lte_tlb_ul_info_req_s

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);

  if(emm_ctrl_data_ptr->rrc_active == FALSE)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= LTE is Inactive,Discarding outgoing tlb msg");
	return dummy_value;
  }

  ASSERT(emm_cmd_ptr != (tlb_cmd_type *)NULL) ; 
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 

  memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
         0x0,
         sizeof(lte_nas_outgoing_msg_type));
  memset((void *)&ul_security_info,
         0x0,
         sizeof(lte_nas_ul_security_info_type));

  /* Signaling connection exists -- Need to check for this and then send the message*/
  if(emm_cmd_ptr->cmd.hdr.id == LTE_TLB_UL_INFO_REQ) 
  {
    MSG_HIGH_DS_0(MM_SUB, "NAS_TLB: Received LTE_TLB_UL_INFO from TLB module");
    /*Add the procotocl discriminator into the TLB message, for MSG_LIB to encode this message*/
    emm_ctrl_data_ptr->out_msg_ptr->nas_hdr.pd = RESERVED_FOR_TESTS_PROCEDURES;
    /*Copy the raw buffer length into the message structure*/
    emm_ctrl_data_ptr->out_msg_ptr->tlb_outgoing_msg.tlb_msg.tlb_len = emm_cmd_ptr->cmd.nas_msg_container.msg_size;
    /*Copy the raw buffer into the message structure*/
    memscpy((void*)&emm_ctrl_data_ptr->out_msg_ptr->tlb_outgoing_msg.tlb_msg.msg_payload[0],
            sizeof(emm_ctrl_data_ptr->out_msg_ptr->tlb_outgoing_msg.tlb_msg.msg_payload),
            (void*)&emm_cmd_ptr->cmd.nas_msg_container.msg_payload[0],
            emm_cmd_ptr->cmd.nas_msg_container.msg_size);
    /*-------------------------------------------------------------------
       Call "emm_send_no_ack_msg", this function takes care of calling 
       the MSG_LIB encoding and hence routing it to RRC
    -------------------------------------------------------------------*/
    if( FALSE == emm_send_no_ack_msg(emm_ctrl_data_ptr->out_msg_ptr,
                                     emm_get_nas_transaction_id(), 
                                     emm_ctrl_data_ptr))                                       
    {
      MSG_ERROR_DS_0(MM_SUB, "NAS_TLB: NAS send message failed");
    }
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "NAS_TLB: Received invalid message from TLB Module, discarding it");
  }
  return dummy_value;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================
FUNCTION    EMM_PROCESS_INCOMING_TLB_MESSAGE

DESCRIPTION
  This function sends the incoming PLAIN TLB message to TLB through MSGR

DEPENDENCIES
  None

RETURN VALUE
  NONE 

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_incoming_tlb_msg
(
  byte *in_tlb_msg,
  word  tlb_msg_len
)
{
  lte_tlb_dl_info_ind_s dl_tlb_msg;

  memset(&dl_tlb_msg,0,sizeof(lte_tlb_dl_info_ind_s));

  msgr_init_hdr(&dl_tlb_msg.hdr,MSGR_NAS_EMM,LTE_TLB_DL_INFO_IND); 
  
  /*Fill the 'C' structure for TLB message*/

  if( tlb_msg_len > LTE_TLB_MAX_NAS_MSG_CONTAINER_SIZE )
  {
    MSG_ERROR_DS_1(MM_SUB, "NAS_TLB: incoming message length %d exceeds payload size, discarding this message",
                   tlb_msg_len);
#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */
    return;
  }

  dl_tlb_msg.nas_msg_container.msg_size = tlb_msg_len;
  memscpy((void*)&dl_tlb_msg.nas_msg_container.msg_payload[0],
          sizeof(dl_tlb_msg.nas_msg_container.msg_payload),
          (void*)in_tlb_msg,
          tlb_msg_len);

  /*Send this TLB message to TLB through MSGR*/
  if(emm_msgr_send(&dl_tlb_msg.hdr, sizeof(lte_tlb_dl_info_ind_s)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "TLB: emm_msgr_send failed!", 0,0,0 );
  }
  MSG_HIGH_DS_0(MM_SUB, "NAS_TLB: sent LTE_TLB_DL_INFO_IND");
}

#endif /*FEATURE_LTE*/
