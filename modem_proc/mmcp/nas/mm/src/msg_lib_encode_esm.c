/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_encode_esm.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when       who                          what                              
--------   ------  ------------------------------------------------
04/29/10   zren    Added support of encoding act def bearer context reject
                   message
01/21/10   zren    Added EMM/ESM STATUS feature
05/17/09   hnam    Added the code to support security
04/28/09   MNK     Changed esm_information to esm_information_transfer_bit
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "amssassert.h"
#include "bit.h"
#include "lte_nas.h"
#include "lte_nas_common_v.h"

#include "msg_lib_encode_esm.h"
#include "msg_lib_encode_common_esm_ie.h"

#include "lte_nas_esm_message_type.h"
#include "lte_nas_esm_message_description.h"
#include "msg.h"
#include "err.h"
#include "msg_lib_declarations.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include "stringl.h"
#include "emm_database.h"
#include "reg_sim.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define ESM_INFO_TRANSFER_FLAG_IEI 0xD
#define EPS_QOS_IEI 0x5B
#define ESM_CAUSE_IEI 0x58
#define ESM_APN_IEI 0x28
#define MAX_ALLOWABLE_PKT_SIZE_FOR_UICC_VALIDATION 255

/*===========================================================================

                        FUNCTION DEFINITION

===========================================================================*/
/*===========================================================================

FUNCTION  ENCODE_ESM_INFO_RESP

DESCRIPTION
  This function encodes the ESM_INFO_RESPONSE message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_esm_info_resp(lte_nas_esm_info_resp   *esm_info_resp, byte *ota_buf, word *pos)
{
  byte num_bits_to_write = 0;
  word esm_len;
  byte index = 0;

  /*-----------------------------------------------
              Access point Name (APN)
  ------------------------------------------------*/
  if(esm_info_resp->access_point_name.valid == TRUE)
  {
    /* ---- APN IEI ---- */     
    num_bits_to_write = 8;
    b_packb((byte)ESM_APN_IEI,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write; 
    /* ---- Length of APN ---- */
    num_bits_to_write = 8;
    b_packb((byte)esm_info_resp->access_point_name.apn_addr_len,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;     
    /* ---- APN Value ----*/
    num_bits_to_write = 8;
    for(index =0; index <esm_info_resp->access_point_name.apn_addr_len;index++)
    {
      b_packb((byte)esm_info_resp->access_point_name.address[index],ota_buf,*pos,num_bits_to_write);
      *pos += num_bits_to_write;        
    }
  }
  /*-------------------------------------------------------------
             Protocol configuration options (optional)
  -------------------------------------------------------------*/
  esm_len = *pos;
  encode_prot_conf_opt(&esm_info_resp->protocol_conf_opt,ota_buf,pos,&esm_len); 
}
/*===========================================================================

FUNCTION  ENCODE_PDN_DISCONNECT_REQ

DESCRIPTION
  This function encodes the PDN_DISCONNECT_REQUEST message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_pdn_disconnect_req(lte_nas_esm_pdn_disconnect_req   *pdn_disconnect_req, byte *ota_buf, word *pos)
{
  byte num_bits_to_write = 0;
  word esm_len;

  /*-------------------------------------------------------------
            Spare half octet + Linked EPS bearer identity
  -------------------------------------------------------------*/
  num_bits_to_write = 4;
  *pos += num_bits_to_write;
  b_packb((byte)pdn_disconnect_req->linked_eps_bearer_identity,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write; 
  /*-------------------------------------------------------------
             Protocol configuration options (optional)
  -------------------------------------------------------------*/
  esm_len = *pos;
  encode_prot_conf_opt(&pdn_disconnect_req->protocol_conf_opt,ota_buf,pos,&esm_len); 
}
/*===========================================================================

FUNCTION  ENCODE_MODIFY_BEARER_CONTEXT_ACCEPT

DESCRIPTION
  This function encodes the MODIFY_BEARER_CONTEXT_ACCEPT message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_modifi_bearer_ctxt_accept(lte_nas_esm_modify_eps_bearer_ctxt_accept *modifi_bearer_ctxt_accept, byte *ota_buf, word *pos)
{
  word esm_len;

  /*-------------------------------------------------------------
             Protocol configuration options (optional)
  -------------------------------------------------------------*/
  esm_len = *pos;
  encode_prot_conf_opt(&modifi_bearer_ctxt_accept->protocol_conf_opt,ota_buf,pos,&esm_len);  
}
/*===========================================================================

FUNCTION  ENCODE_MODIFY_BEARER_CONTEXT_REJ

DESCRIPTION
  This function encodes the MODIFY_BEARER_CONTEXT_REJECT message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_modifi_bearer_ctxt_rej(lte_nas_esm_modify_eps_bearer_ctxt_rej *modify_bearer_ctxt_rej, byte *ota_buf, word *pos)
{
  byte num_bits_to_write = 0;
  word esm_len;

  /*-------------------------------------------------------------
                          ESM CAUSE
  --------------------------------------------------------------*/
  if(modify_bearer_ctxt_rej->esm_cause.valid == TRUE)
  {
    /* ---- ESM CAUSE IEI ---- */
    num_bits_to_write = 8;
    b_packb((byte)modify_bearer_ctxt_rej->esm_cause.esm_cause,
            ota_buf, *pos,num_bits_to_write);  
    *pos += num_bits_to_write;     
  }
  /*-------------------------------------------------------------
             Protocol configuration options (optional)
  -------------------------------------------------------------*/
  esm_len = *pos;
  encode_prot_conf_opt(&modify_bearer_ctxt_rej->protocol_conf_opt,ota_buf,pos,&esm_len);  
}
/*===========================================================================

FUNCTION  ENCODE_BEARER_MODIFI_REQ

DESCRIPTION
  This function encodes the BEARER_MODIFICATION_REQUEST message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_bearer_modifi_req(lte_nas_esm_bearer_resource_modifi_req *bearer_modifi_req, byte *ota_buf, word *pos)
{
  byte num_bits_to_write = 0;
  word esm_len;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  /*-------------------------------------------------------------
            Spare half octet + Linked EPS bearer identity
  -------------------------------------------------------------*/

  num_bits_to_write = 4;
  *pos += num_bits_to_write;

  b_packb((byte)bearer_modifi_req->linked_eps_bearer_identity,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write; 
  esm_len = *pos;
  /*-------------------------------------------------------------
                 Traffic flow aggregate
  -------------------------------------------------------------*/
  encode_esm_tft(&bearer_modifi_req->tft,ota_buf,pos,&esm_len);  

  /*-------------------------------------------------------------
                 EPS quality of service
  -------------------------------------------------------------*/
  
  if(bearer_modifi_req->sdf_qos.valid == TRUE)
  {
    /* ---- EPS QoS IEI ---- */
    num_bits_to_write = 8;
    b_packb((byte)EPS_QOS_IEI,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write; 

    esm_len = *pos;
    encode_eps_qos(&bearer_modifi_req->sdf_qos,ota_buf,pos,&esm_len);
  }
  /*-------------------------------------------------------------
                          ESM CAUSE
  --------------------------------------------------------------*/
  if(bearer_modifi_req->esm_cause.valid == TRUE)
  {
    /* ---- ESM CAUSE IEI ---- */
    num_bits_to_write = 8;
    b_packb((byte)ESM_CAUSE_IEI,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write; 
    b_packb((byte)(bearer_modifi_req->esm_cause.esm_cause),ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write; 
    esm_len = *pos;
  }

  /*-------------------------------------------------------------
             Protocol configuration options (optional)
  -------------------------------------------------------------*/
  esm_len = *pos;
  encode_prot_conf_opt(&bearer_modifi_req->protocol_conf_opt,ota_buf,pos,&esm_len);  

#ifdef FEATURE_LTE_REL10
  /*  Device Properties IE */
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
     (reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE))
  {
    MSG_HIGH_DS_1(MM_SUB, "Low priority indicator value = %d in BEARER MOD REQ",
                  bearer_modifi_req->low_priority_indicator);
    encode_esm_device_properties_ie(&bearer_modifi_req->low_priority_indicator, ota_buf, pos, &esm_len);
  }
#endif
}
/*===========================================================================

FUNCTION  ENCODE_BEARER_ALLOC_REQ

DESCRIPTION
  This function encodes the BEARER_ALLOCATION_REQUEST message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_bearer_alloc_req(lte_nas_esm_bearer_resource_allocation_req *bearer_alloc_req, byte *ota_buf, word *pos)
{
  byte num_bits_to_write = 0;
  word esm_len;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  MSG_LOW_DS_0(MM_SUB, "Encoding bearer resource allocation request\n");

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  /*-------------------------------------------------------------
            Spare half octet + Linked EPS bearer identity
  -------------------------------------------------------------*/
  num_bits_to_write = 4;
  *pos += num_bits_to_write; 

  num_bits_to_write = 4;
  b_packb((byte)bearer_alloc_req->linked_eps_bearer_identity,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write; 
  
  /*-------------------------------------------------------------
                 Traffic flow aggregate
  -------------------------------------------------------------*/
  esm_len = *pos;
  encode_esm_tft(&bearer_alloc_req->tft,ota_buf,pos,&esm_len);  

  /*-------------------------------------------------------------
                 EPS quality of service
  -------------------------------------------------------------*/
  esm_len = *pos;
  encode_eps_qos(&bearer_alloc_req->sdf_qos,ota_buf,pos,&esm_len);

  /*-------------------------------------------------------------
             Protocol configuration options (optional)
  -------------------------------------------------------------*/
  esm_len = *pos;
  encode_prot_conf_opt(&bearer_alloc_req->protocol_conf_opt,ota_buf,pos,&esm_len);  

#ifdef FEATURE_LTE_REL10
  /*  Device Properties IE */
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
     (reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE))
  {
    MSG_HIGH_DS_1(MM_SUB, "Low priority indicator value = %d in BEARER ALLOC REQ",
                  bearer_alloc_req->low_priority_indicator);
    encode_esm_device_properties_ie(&bearer_alloc_req->low_priority_indicator, ota_buf, pos, &esm_len);
  }
#endif
}
/*===========================================================================

FUNCTION  ENCODE_DEACT_BEARER_CONTEXT_ACCEPT

DESCRIPTION
  This function encodes the DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_deact_bearer_context_accept(lte_nas_esm_deact_eps_bearer_context_accept *deact_bearer_context_acc, byte *ota_buf, word *pos)
{
  word esm_len;

  esm_len = *pos;
  /*------ WRITE THE LENGTH OF THE OFFSET ----*/ 
  encode_prot_conf_opt(&deact_bearer_context_acc->protocol_conf_opt,ota_buf,pos,&esm_len);  
}
/*===========================================================================

FUNCTION  ENCODE_EPS_BEARER_CONTEXT_ACCEPT

DESCRIPTION
  This function encodes the ACTIVATE_EPS_BEARER_CONTEXT_ACCEPT message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_eps_bearer_context_accept(lte_nas_esm_act_dedicated_bearer_context_accept *dedicated_bearer_context_acc, byte *ota_buf, word *pos)
{
  word esm_len;
  /*------ WRITE THE LENGTH OF THE OFFSET ----*/
  esm_len = *pos;
  encode_prot_conf_opt(&dedicated_bearer_context_acc->protocol_conf_opt,ota_buf,pos,&esm_len);  
}
/*===========================================================================

FUNCTION  ENCODE_ACT_DEF_BEARER_CONTEXT_ACCEPT

DESCRIPTION
  This function encodes the ACTIVATE_DEFAULT_BEARER_CONTEXT_ACCEPT message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_act_def_bearer_context_accept(lte_nas_esm_act_default_bearer_context_accept *act_def_bearer_context_acc, byte *ota_buf, word *pos, word *esm_container_len)
{  
  /*------ WRITE THE LENGTH OF THE OFFSET ----*/
  *esm_container_len  += 3;  
  encode_prot_conf_opt(&act_def_bearer_context_acc->protocol_conf_opt,ota_buf,pos,esm_container_len);
}

/*===========================================================================

FUNCTION  ENCODE_PDN_CONN_REQ

DESCRIPTION
  This function encodes the PDN_CONNECTIVITY_REQUEST message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_pdn_conn_request
(
  lte_nas_esm_pdn_connectivity_req *pdn_conn_req,
  byte *ota_buf,
  size_t ota_buf_len,
  word *pos,
  word *esm_container_len,
  boolean encoding_for_uicc
)
{
  byte num_bits_to_write;
  byte *temp_ota_buf = NULL; 
  byte index =0;    
  word temp_buf_len = 0; 
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  *esm_container_len += 3;
    
  /*------------------------------------------------
                  PDN TYPE
  --------------------------------------------------*/  
  num_bits_to_write = 4;
  b_packb((byte)pdn_conn_req->pdn_type,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write; 
    
  /*------------------------------------------------
                  REQUEST TYPE
  --------------------------------------------------*/
  
  num_bits_to_write = 4;
  b_packb((byte)pdn_conn_req->req,ota_buf,*pos,num_bits_to_write);
  
  *pos += num_bits_to_write; 
  *esm_container_len += 1;
  
  /*-----------------------------------------------
                  ESM Transfer Flag
  ------------------------------------------------*/
  if(pdn_conn_req->esm_info_transfer_bit == TRUE)
  {
    num_bits_to_write = 4;
    b_packb(ESM_INFO_TRANSFER_FLAG_IEI,ota_buf,*pos,num_bits_to_write);
    
    *pos += num_bits_to_write; 
    *esm_container_len += 1;
    
    b_packb(pdn_conn_req->esm_info_transfer_bit,ota_buf,*pos,num_bits_to_write);
    
    *pos += num_bits_to_write; 
    *esm_container_len += 1;   
  }
  
  /*-----------------------------------------------
              Access point Name (APN)
  ------------------------------------------------*/
  if(pdn_conn_req->access_point_name.valid == TRUE)
  {
    /* ---- APN IEI ---- */    
    num_bits_to_write = 8;
    b_packb((byte)ESM_APN_IEI,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write; 
    *esm_container_len += 1;
    
    /* ---- Length of APN ---- */
    num_bits_to_write = 8;
    b_packb((byte)pdn_conn_req->access_point_name.apn_addr_len,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write; 
    *esm_container_len += 1;
    
    /* ---- APN Value ----*/
    num_bits_to_write = 8;
    for(index =0; index <pdn_conn_req->access_point_name.apn_addr_len;index++)
    {
      b_packb((byte)pdn_conn_req->access_point_name.address[index],ota_buf,*pos,num_bits_to_write);
      *pos += num_bits_to_write; 
      *esm_container_len += 1;   
    }
  }
  /*------------------------------------------------
           PROTOCOL CONFIGURATION OPTIONS
  --------------------------------------------------*/
  if(pdn_conn_req->protocol_conf_opt.valid == TRUE)
  {
#ifndef FEATURE_LTE_UICC_CALL_CONTROL  
    encode_prot_conf_opt(&pdn_conn_req->protocol_conf_opt,ota_buf,pos,esm_container_len);
#else
    /*Store the offset address and use it while copying the PCO into actual OTA buffer*/
    temp_buf_len = *pos/8;
    temp_ota_buf = (byte*) modem_mem_calloc( 1, sizeof(pdn_conn_req->protocol_conf_opt),
                                               MODEM_MEM_CLIENT_NAS);
	mm_check_for_null_ptr((void *) temp_ota_buf);
    encode_prot_conf_opt(&pdn_conn_req->protocol_conf_opt,temp_ota_buf,pos,esm_container_len);   
    if (!encoding_for_uicc || (*pos)/8 <= MAX_ALLOWABLE_PKT_SIZE_FOR_UICC_VALIDATION)
    {
      memscpy((void*)(ota_buf + temp_buf_len),
              ota_buf_len - temp_buf_len,
              (void*)(temp_ota_buf + temp_buf_len),
              ((*pos)/8 - temp_buf_len));
    }
    else
    {
      MSG_HIGH_DS_1(MM_SUB, "MSG_LIB: discarding PCO as the size of PACKET_PDN_CONNECTIVITY_REQ is %d, max allowable size is 255",
                    ((*pos)/8));
    }
    modem_mem_free(temp_ota_buf, MODEM_MEM_CLIENT_NAS);
#endif
  }

#ifdef FEATURE_LTE_REL10
  /*  Device Properties IE */
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
     (reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE))
  {
    MSG_HIGH_DS_1(MM_SUB, "Low priority indicator value = %d in PDN CONN REQ",
                  pdn_conn_req->low_priority_indicator);
    encode_esm_device_properties_ie(&pdn_conn_req->low_priority_indicator, ota_buf, pos, esm_container_len);
  }
#endif
}

/*===========================================================================

FUNCTION  ENCODE_DEDICATED_EPS_BEARER_CONTEXT_REJ

DESCRIPTION
  This function encodes the DEDICATED_EPS_BEARER_CONTEXT_REJECT message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_dedicated_eps_bearer_context_rej(lte_nas_esm_act_dedicated_bearer_context_rej *dedicated_bearer_cont_rej, byte *ota_buf, word *pos)
{
  byte num_bits_to_write = 0;  
  
  /*------------------------------------------------
                  ESM CAUSE
  --------------------------------------------------*/
  
  num_bits_to_write = 8;
  b_packb((byte)dedicated_bearer_cont_rej->esm_cause.esm_cause,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write; 
  
  /*------------------------------------------------
           PROTOCOL CONFIGURATION OPTIONS
  --------------------------------------------------*/
  if(dedicated_bearer_cont_rej->protocol_conf_opt.valid == TRUE)  
  {
    encode_prot_conf_opt(&dedicated_bearer_cont_rej->protocol_conf_opt,ota_buf,pos,pos);
  }   
}

/*===========================================================================

FUNCTION  ENCODE_ACT_DEFAULT_EPS_BEARER_CONTEXT_REJ

DESCRIPTION
  This function encodes the ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  encode_act_default_eps_bearer_context_rej
(
  lte_nas_esm_act_default_bearer_context_rej  *act_default_bc_rej_ptr, 
  byte                                        *ota_buf, 
  word                                        *pos
)
{
  ASSERT(act_default_bc_rej_ptr != NULL);
  ASSERT(ota_buf != NULL);
  ASSERT(pos != NULL);

  /*------------------------------------------------
                  ESM CAUSE
  --------------------------------------------------*/
  b_packb((byte)act_default_bc_rej_ptr->esm_cause.esm_cause,
          ota_buf,
          *pos,
          8);  
  *pos += 8; 
  
  /*------------------------------------------------
           PROTOCOL CONFIGURATION OPTIONS
  --------------------------------------------------*/
  if(act_default_bc_rej_ptr->protocol_conf_opt.valid == TRUE)  
  {
    encode_prot_conf_opt(&act_default_bc_rej_ptr->protocol_conf_opt,
                         ota_buf,
                         pos,
                         pos);
  }
} /* encode_act_default_eps_bearer_context_rej */

/*===========================================================================

FUNCTION  ENCODE_EPS_BEARER_CONTEXT_REJ

DESCRIPTION
  This function encodes the EPS_BEARER_CONTEXT_REJECT message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_eps_bearer_context_rej(lte_nas_esm_act_default_bearer_context_rej *act_def_bearer_cont_rej, byte *ota_buf, word *pos)
{
  byte num_bits_to_write;   
  /*------------------------------------------------
                  ESM CAUSE
  --------------------------------------------------*/  
  num_bits_to_write = 8;
  b_packb((byte)act_def_bearer_cont_rej->esm_cause.esm_cause,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write; 
  
  /*------------------------------------------------
           PROTOCOL CONFIGURATION OPTIONS
  --------------------------------------------------*/
  if(act_def_bearer_cont_rej->protocol_conf_opt.valid == TRUE)  
  {
    encode_prot_conf_opt(&act_def_bearer_cont_rej->protocol_conf_opt,ota_buf,pos,pos);
  }   
}
/*===========================================================================

FUNCTION  ENCODE_NAS_ESM_HEADER

DESCRIPTION
  This function encodes the HEADER for all the ESM message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_nas_esm_header(byte *ota_buf, word *pos,byte pd, byte bearer_id, byte pti,byte msg_id)
{
  byte num_bits_to_write =0;  
  num_bits_to_write = 4;
  b_packb(bearer_id,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write;  
  b_packb(pd,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write;  
  num_bits_to_write = 8;
  b_packb(pti,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write;  
  b_packb(msg_id,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write;  
}
/*===========================================================================

FUNCTION GET_ESM_CONTAINER

DESCRIPTION
  ESM mesage can be a part of EMM OR can be an independent message, 
  this needs to be determined inorder to populate the ESM 'C' structure values.
  1. If it comes as an independent message, then the ESM message will be 
      populated in the ZEROETH ELEMENT of the ARRAY
  2. If it comes as an independent message, then the ESM message will be 
      populated in the FIRST ELEMENT of the ARRAY

DEPENDENCIES
  None

RETURN VALUE
  lte_nas_esm_outgoing_msg_type*

SIDE EFFECTS
  None

===========================================================================*/
lte_nas_esm_outgoing_msg_type *get_esm_container(lte_nas_outgoing_msg_type *out_buf)
{
  lte_nas_esm_outgoing_msg_type *out_msg = NULL;  
  
  if(out_buf->nas_hdr.pd == EPS_SESSION_MANAGEMENT_MESSAGES)
  {
    out_msg = (lte_nas_esm_outgoing_msg_type *)&out_buf->esm_outgoing_msg;   
  }
  else if(out_buf->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES)
  {
    switch(out_buf->nas_hdr.msg_id)
    {
    case ATTACH_REQUEST:      
      out_msg = (lte_nas_esm_outgoing_msg_type *)&out_buf->emm_outgoing_msg.attach_req.esm_msg_container;
      break;
     
    case ATTACH_COMPLETE:
      out_msg = (lte_nas_esm_outgoing_msg_type *)&out_buf->emm_outgoing_msg.attach_complete.esm_msg_container;
      break;
    default:
      MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: ESM encoding was called from the EMM message type with enum id = %d. Impossible case",
                    (byte)out_buf->nas_hdr.msg_id);
      out_msg = NULL;
      break;
    }/*End of switch(out_buf->nas_hdr.msg_id)*/    
  }/*End of else*/
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "Received unknown protocol discriminator from EMM, unable to continue encoding...\n");
  }
  return out_msg;
}/*End of get_esm_container()*/

/*===========================================================================

FUNCTION  ENCODE_ESM_MSG

DESCRIPTION
  This function encodes ESM message. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_esm_msg
(
  lte_nas_outgoing_msg_type *out_buf,
  byte *ota_buf,
  size_t ota_buf_len,
  word *pos,
  boolean encoding_for_uicc
)
{
  word esm_container_len_offset,esm_container_len = 0;
  byte num_bits_to_write =0;
  byte esm_len_octet =16;
  
  lte_nas_esm_outgoing_msg_type *esm_outgoing_msg = {NULL};  
  
  esm_outgoing_msg = get_esm_container(out_buf);     
  
  ASSERT(esm_outgoing_msg != NULL);
  
  /*--------------------------------------------------------------------
       Based on the message type, select the right encoder function
  --------------------------------------------------------------------*/
  switch(esm_outgoing_msg->nas_hdr.msg_id)
  {
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_ACCEPT:
      /*--------------------------------------------------------
                  LENGTH OF ESM CONTAINER
      ---------------------------------------------------------*/
      esm_container_len_offset = *pos;
      if(out_buf->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES)
      {
        *pos += esm_len_octet;/*Skip the octet to which ESM Container Length is to be written                           
                            (ESM Container Length is written at later point of time) */    
      }
      encode_nas_esm_header(ota_buf,
                            pos,
                            (byte)esm_outgoing_msg->default_bearer_context_accept.lte_nas_hdr.pd,
                            esm_outgoing_msg->default_bearer_context_accept.lte_nas_esm_hdr.bearer_id,
                            esm_outgoing_msg->default_bearer_context_accept.lte_nas_esm_hdr.pti,
                            (byte)esm_outgoing_msg->default_bearer_context_accept.lte_nas_hdr.msg_id);
      encode_act_def_bearer_context_accept(&esm_outgoing_msg->default_bearer_context_accept,
                                           ota_buf,
                                           pos,
                                           &esm_container_len);    
      if(out_buf->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES)
      {
        /* ----- Length of the ESM Container ----- */
        num_bits_to_write = 16;
        b_packb((byte)(((*pos - esm_len_octet)- esm_container_len_offset)/8),ota_buf,esm_container_len_offset,num_bits_to_write);
      }
      break;
    case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REJECT:
      /* Encode ESM message header */
      encode_nas_esm_header(
        ota_buf,
        pos,
        (byte)esm_outgoing_msg->default_bearer_context_rej.lte_nas_hdr.pd,
        esm_outgoing_msg->default_bearer_context_rej.lte_nas_esm_hdr.bearer_id,
        esm_outgoing_msg->default_bearer_context_rej.lte_nas_esm_hdr.pti,
        (byte)esm_outgoing_msg->default_bearer_context_rej.lte_nas_hdr.msg_id);
      /* Encode remaining part */
      encode_act_default_eps_bearer_context_rej(
                                 &esm_outgoing_msg->default_bearer_context_rej,
                                  ota_buf, 
                                  pos);
      break;
    case PDN_CONNECTIVITY_REQUEST:
      /*--------------------------------------------------------
                  LENGTH OF ESM CONTAINER
      ---------------------------------------------------------*/
      esm_container_len_offset = *pos;
      if(out_buf->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES)
      {
        *pos += esm_len_octet;/*Skip the octet to which ESM Container Length is to be 
                              written (ESM Container Length is written at later point of time) */
      }
      encode_nas_esm_header(ota_buf,
                            pos,
                            (byte)esm_outgoing_msg->pdn_connectivity_req.lte_nas_hdr.pd,
                            esm_outgoing_msg->pdn_connectivity_req.lte_nas_esm_hdr.bearer_id,
                            esm_outgoing_msg->pdn_connectivity_req.lte_nas_esm_hdr.pti,
                            (byte)esm_outgoing_msg->pdn_connectivity_req.lte_nas_hdr.msg_id);
      encode_pdn_conn_request(&esm_outgoing_msg->pdn_connectivity_req,
                              ota_buf, 
                              ota_buf_len, 
                              pos, 
                              &esm_container_len,
                              encoding_for_uicc);
      /* ----- Length of the ESM Container ----- */
      if(out_buf->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES)
      {
        num_bits_to_write = 16;
        b_packb((byte)(((*pos - esm_len_octet)- esm_container_len_offset)/8),ota_buf,esm_container_len_offset,num_bits_to_write);
      }
      break;
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_ACCEPT:
      encode_nas_esm_header(ota_buf,
                            pos,
                            (byte)esm_outgoing_msg->dedicated_bearer_context_accept.lte_nas_hdr.pd,
                            esm_outgoing_msg->dedicated_bearer_context_accept.lte_nas_esm_hdr.bearer_id,
                            esm_outgoing_msg->dedicated_bearer_context_accept.lte_nas_esm_hdr.pti,
                            (byte)esm_outgoing_msg->dedicated_bearer_context_accept.lte_nas_hdr.msg_id);
      encode_eps_bearer_context_accept(&esm_outgoing_msg->dedicated_bearer_context_accept,
                                       ota_buf, 
                                       pos);
      break;
    case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REJECT: 
      encode_nas_esm_header(ota_buf,
                            pos,
                            (byte)esm_outgoing_msg->dedicated_bearer_context_rej.lte_nas_hdr.pd,
                            esm_outgoing_msg->dedicated_bearer_context_rej.lte_nas_esm_hdr.bearer_id,
                            esm_outgoing_msg->dedicated_bearer_context_rej.lte_nas_esm_hdr.pti,
                            (byte)esm_outgoing_msg->dedicated_bearer_context_rej.lte_nas_hdr.msg_id);
      encode_dedicated_eps_bearer_context_rej(&esm_outgoing_msg->dedicated_bearer_context_rej,
                                              ota_buf, 
                                              pos);   
      break;
    case BEARER_RESOURCE_ALLOCATION_REQUEST:
      encode_nas_esm_header(ota_buf,
                            pos,
                            (byte)esm_outgoing_msg->bearer_alloc_req.lte_nas_hdr.pd,
                            esm_outgoing_msg->bearer_alloc_req.lte_nas_esm_hdr.bearer_id,
                            esm_outgoing_msg->bearer_alloc_req.lte_nas_esm_hdr.pti,
                            (byte)esm_outgoing_msg->bearer_alloc_req.lte_nas_hdr.msg_id);
      encode_bearer_alloc_req(&esm_outgoing_msg->bearer_alloc_req,ota_buf, pos);
      break;
    case BEARER_RESOURCE_MODIFICATION_REQUEST:
      esm_container_len += 16;
      encode_nas_esm_header(ota_buf,
                            pos,
                            (byte)esm_outgoing_msg->bearer_resource_modifi_req.lte_nas_hdr.pd,
                            esm_outgoing_msg->bearer_resource_modifi_req.lte_nas_esm_hdr.bearer_id,
                            esm_outgoing_msg->bearer_resource_modifi_req.lte_nas_esm_hdr.pti,
                            (byte)esm_outgoing_msg->bearer_resource_modifi_req.lte_nas_hdr.msg_id);
      encode_bearer_modifi_req(&esm_outgoing_msg->bearer_resource_modifi_req,ota_buf, pos);
      break;  
    case MODIFY_EPS_BEARER_CONTEXT_ACCEPT:
      esm_container_len += 16;
      encode_nas_esm_header(ota_buf,
                            pos,
                            (byte)esm_outgoing_msg->modify_eps_bearer_ctxt_accept.lte_nas_hdr.pd,
                            esm_outgoing_msg->modify_eps_bearer_ctxt_accept.lte_nas_esm_hdr.bearer_id,
                            esm_outgoing_msg->modify_eps_bearer_ctxt_accept.lte_nas_esm_hdr.pti,
                            (byte)esm_outgoing_msg->modify_eps_bearer_ctxt_accept.lte_nas_hdr.msg_id);
      encode_modifi_bearer_ctxt_accept(&esm_outgoing_msg->modify_eps_bearer_ctxt_accept,ota_buf, pos);
      break;
    case MODIFY_EPS_BEARER_CONTEXT_REJECT:
      esm_container_len += 16;
      encode_nas_esm_header(ota_buf,
                            pos,
                            (byte)esm_outgoing_msg->modify_eps_bearer_ctxt_rej.lte_nas_hdr.pd,
                            esm_outgoing_msg->modify_eps_bearer_ctxt_rej.lte_nas_esm_hdr.bearer_id,
                            esm_outgoing_msg->modify_eps_bearer_ctxt_rej.lte_nas_esm_hdr.pti,
                            (byte)esm_outgoing_msg->modify_eps_bearer_ctxt_rej.lte_nas_hdr.msg_id);
      encode_modifi_bearer_ctxt_rej(&esm_outgoing_msg->modify_eps_bearer_ctxt_rej,ota_buf, pos);
      break;
    case DEACTIVATE_EPS_BEARER_CONTEXT_ACCEPT:   
      esm_container_len += 16;
      encode_nas_esm_header(ota_buf,
                            pos,
                            (byte)esm_outgoing_msg->deactivate_eps_bearer_context_accept.lte_nas_hdr.pd,
                            esm_outgoing_msg->deactivate_eps_bearer_context_accept.lte_nas_esm_hdr.bearer_id,
                            esm_outgoing_msg->deactivate_eps_bearer_context_accept.lte_nas_esm_hdr.pti,
                            (byte)esm_outgoing_msg->deactivate_eps_bearer_context_accept.lte_nas_hdr.msg_id);
      encode_deact_bearer_context_accept(&esm_outgoing_msg->deactivate_eps_bearer_context_accept,ota_buf, pos);
      break;   
    case PDN_DISCONNECT_REQUEST:   
      esm_container_len += 16;
      encode_nas_esm_header(ota_buf,
                            pos,
                            (byte)esm_outgoing_msg->pdn_disconnect_req.lte_nas_hdr.pd,
                            esm_outgoing_msg->pdn_disconnect_req.lte_nas_esm_hdr.bearer_id,
                            esm_outgoing_msg->pdn_disconnect_req.lte_nas_esm_hdr.pti,
                            (byte)esm_outgoing_msg->pdn_disconnect_req.lte_nas_hdr.msg_id);
      encode_pdn_disconnect_req(&esm_outgoing_msg->pdn_disconnect_req,ota_buf, pos);
      break;
    case ESM_INFORMATION_RESPONSE:   
      esm_container_len += 16;
      encode_nas_esm_header(ota_buf,
                            pos,
                            (byte)esm_outgoing_msg->esm_info_resp.lte_nas_hdr.pd,
                            esm_outgoing_msg->esm_info_resp.lte_nas_esm_hdr.bearer_id,
                            esm_outgoing_msg->esm_info_resp.lte_nas_esm_hdr.pti,
                            (byte)esm_outgoing_msg->esm_info_resp.lte_nas_hdr.msg_id);
      encode_esm_info_resp(&esm_outgoing_msg->esm_info_resp,ota_buf, pos);
      break;
    case ESM_STATUS:
      encode_nas_esm_header(ota_buf,
                            pos,
                            (byte)esm_outgoing_msg->mo_esm_status.lte_nas_hdr.pd,
                            esm_outgoing_msg->mo_esm_status.lte_nas_esm_hdr.bearer_id,
                            esm_outgoing_msg->mo_esm_status.lte_nas_esm_hdr.pti,
                            (byte)esm_outgoing_msg->mo_esm_status.lte_nas_hdr.msg_id);
      /* ESM cause */
      b_packb((byte)out_buf->esm_outgoing_msg.mo_esm_status.esm_cause, 
              ota_buf, *pos, 8);
      *pos += 8; 
      break;
    default:
      MSG_ERROR_DS_1(MM_SUB, "Received unknown Message I D = %d",
                     (byte)out_buf->nas_hdr.msg_id);
      *pos = 0;
      msg_lib_encode_error =  FALSE;
      break;
  }/*End of switch(out_buf->nas_hdr.msg_id)*/
  
  MSG_HIGH_DS_0(MM_SUB, "=NAS_MSG_LIB= ESM encoding complete");
  
} /*End of encode_esm_msg()*/

#endif /*FEATURE_LTE*/
