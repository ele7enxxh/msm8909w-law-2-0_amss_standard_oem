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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_decode_esm.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when         who                          what                              
--------     --------  ------------------------------------------------
05/26/15     cogolt   Added Rel-10 ESM backoff timer (T3396 timer) support
05/14/10      RI      Mainlined spec changes upto Dec 09.
01/21/10      zren    Added EMM/ESM STATUS feature
08/26/09      MNK      Fix a bug in decode activate default bearer request for MAR spec. Move report_esm_failure
08/04/09      hnam     Return TRUE whenever optional fields fails
                        (so that full message is not discarded)
07/29/09      hnam     Changed the name of the MACRO (for MARCH/DEC spec compatibiltiy), 
                        used the standard macro commonly used by LTE_MODEM_SW
06/08/09      hnam     (1) If ESM PD fails, set nas_esm_index to zero. Previously, this was not reset 
                           & this caused ESM failure (as ESM presumed to have a valid PD). 
                       (2) Fix for CR# 184007 
05/17/09      hnam     Added the code to support security
04/13/09      hnam     Added valid flags for all optional fields.
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "bit.h"
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "lte_nas_local_cause.h"
#include "amssassert.h"
#include "msg_lib_decode_esm.h"
#include "msg_lib_decode_common_esm_ie.h"

#include "lte_nas_emm_message_type.h"
#include "lte_nas_emm_message_description.h"
#include "msg_lib_declarations.h"
#include "msg_lib_validation.h"
#include "msg.h"
#include "err.h"
#include "esm_cm_msg_if.h"
#include "event_defs.h"

#ifdef FEATURE_LTE_REL9 
#include "emm_database.h"
#endif
#include "reg_sim.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define WORD_SIZE 8

#define PDN_ADDRESS_IEI 0x59
#define TRANSACTION_IDENTIFIER 0X5D
#define NEGOTIATED_QOS 0x30
#define NEGOGTIATED_LLC_SAPI 0x32
#define RADIO_PRIORITY_IEI 0x8
#define PACKET_FLOW_IDENTIFIER 0x34
#define APN_AGGRESSIVE_MAX_BIT_RATE 0X5E
#define ESM_CAUSE_IEI 0x58
#define PROT_CONF_OPT 0x27
#define EPS_QOS 0x5B
#define TRAFFIC_FLOW_TEMPLATE 0x36

#ifdef FEATURE_LTE_REL10
#define T3396_VALUE_IEI 0x37
#endif

#define LINKED_EPS_BEARER_RESERVED_VALUE 4

#define NAS_ESM_ACT_DED_REQ_MIN_LEN 8*WORD_SIZE
#define NAS_ESM_ACT_DEF_REQ_MIN_LEN  7*WORD_SIZE
#define NAS_ESM_PCR_MIN_LEN  4*WORD_SIZE
#define NAS_ESM_DEACT_BEARER_REQ_MIN_LEN 4*WORD_SIZE
#define NAS_ESM_BEARER_RES_ALLOC_REJ_MIN_LEN 4*WORD_SIZE
#define NAS_ESM_BEARER_RES_MODIFI_REJ_MIN_LEN 4*WORD_SIZE
#define NAS_ESM_PDN_DISCONNECT_REJ 4*WORD_SIZE
#define NAS_ESM_ESM_STATUS  4*WORD_SIZE

#ifdef FEATURE_LTE_REL9 
#define NAS_ESM_NOTIFICATION 5*WORD_SIZE
#endif

boolean esm_length_check_failed;

/*===========================================================================

                        FUNCTION DEFINITION

===========================================================================*/

#ifdef FEATURE_LTE_REL9 
/*===========================================================================

FUNCTION  DECODE_ESM_NOTIFICATION

DESCRIPTION
  This function decodes ESM_NOTIFICATION message. 

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_esm_notification
(
   byte                       *src,
   word                       *pos, 
   word                       *buffer_length,   
   lte_nas_esm_notification   *esm_notification,
   word                       *esm_len_in_bits
)
{
  byte num_bits_to_read =0, unpacked_bytes =0;  
  byte notification_ind_len = 1;

  if(*buffer_length < (NAS_ESM_NOTIFICATION - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "NAS_MSG_LIB: Buffer length = %d is insufficient to decode ESM NOTIFICATION",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE;
  }

  /*--------------------------------------------------
                NOTIFICATION_INDICATOR
  --------------------------------------------------*/  
  /*Length*/
  num_bits_to_read = 8; 
  unpacked_bytes = b_unpackb(src, *pos, num_bits_to_read);
  if(unpacked_bytes != notification_ind_len)
  {
    MSG_ERROR_DS_1(MM_SUB, "ESM: invalid length for Notification_indicator = %d",
                   unpacked_bytes);
    return FALSE;
  }
  (*pos) += num_bits_to_read;

  unpacked_bytes = b_unpackb(src, *pos, num_bits_to_read);
  if(unpacked_bytes != 0x01)
  {
    /* 9.9.4.7A.1 all other values are ignored */
    MSG_HIGH_DS_1(MM_SUB, "ESM: Notification_indicator %d unused ignored", unpacked_bytes);
    return FALSE;
  }

  esm_notification->notification_ind = unpacked_bytes;
  (*pos) += num_bits_to_read;
  return TRUE;
}
#endif

/*===========================================================================

FUNCTION  POPULATE_VALID_ESM_CAUSE

DESCRIPTION
  populates the esm_cause if it's a valid value

DEPENDENCIES
  None

RETURN VALUE
  TRUE : valid esm cause
  FALSE: invalid esm_cause

SIDE EFFECTS
  None

===========================================================================*/
boolean populate_valid_esm_cause
(
  esm_cause_enum_T *esm_cause,
  byte unpackedbytes
)
{
  if((unpackedbytes == (byte)NAS_ESM_OPTR_DETERMINED_BARRING)||
    ((unpackedbytes  >= (byte)NAS_ESM_INSUFFICIENT_RESOURCES)&&
     (unpackedbytes  <= (byte)NAS_ESM_PTI_MISMATCH)))
  {
    *esm_cause = (esm_cause_enum_T)unpackedbytes;  
  }
  else if((unpackedbytes >= (byte)NAS_ESM_LAST_PDN_DISCONNECTED_NOT_ALLOWED)&&
          (unpackedbytes <= (byte)NAS_ESM_COLLISION_WITH_NW_INIT_REQ ))
  {
    *esm_cause = (esm_cause_enum_T)unpackedbytes;  
  }
  else if((unpackedbytes == (byte)NAS_ESM_UNSUPPORTED_QCI_VALUE) ||
          (unpackedbytes == (byte)NAS_ESM_INVALID_PTI_VALUE) ||
		  (unpackedbytes == (byte)NAS_ESM_BEARER_HANDLING_NOT_SUPPORTED))
  {
    *esm_cause = (esm_cause_enum_T)unpackedbytes;  
  }
  else if((unpackedbytes >= (byte)NAS_ESM_SYMANTICALLY_INVALID_MSG) &&
          (unpackedbytes <= (byte)NAS_ESM_MSG_INCOMPATIBLE_WITH_PRO_STATE))
  {
    *esm_cause = (esm_cause_enum_T)unpackedbytes;  
  }
  else if((unpackedbytes == (byte)NAS_ESM_PROTOCOL_ERROR)||
          (unpackedbytes == (byte)NAS_ESM_APN_RESTRICT_VAL_INCOMPAT_WITH_ACT_CONTEXT))
  {
    *esm_cause = (esm_cause_enum_T)unpackedbytes;  
  }
  else if((unpackedbytes == (byte)NAS_ESM_MAX_PDP_CONTEXT_REACHED)||
          (unpackedbytes == (byte)NAS_ESM_APN_NOT_SUPPORTED_IN_PLMN_RAT_COMBINATION))
  {
    *esm_cause = (esm_cause_enum_T)unpackedbytes;  
  }
  else
  {
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_pdn_conn_req

DESCRIPTION
  This function decodes PDN_CONN_REQ message. 

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
void decode_pdn_conn_req
(
  byte                           *src,
  word                           buffer_length, 
  esm_pdn_connectivity_req_T     *pdn_conn_req
)
{
  word pos = 24; /*Start reading from the RPDN_TYPE IE (3rd octet*/
  word buf_len_in_bits = 0;
  byte num_bits_to_read = 0;
  byte unpacked_bytes = 0;
  byte esm_info_iei = 0x0d;
  byte apn_iei = 0x28;
#ifdef FEATURE_LTE_REL10
  byte device_prop_iei = 0xC;
#endif
  byte index = 0;
  word dummy_buf_len = 0;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  ASSERT(src != (void *)NULL);
  ASSERT(pdn_conn_req != (void *)NULL);
  ASSERT(buffer_length != NULL);

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  buffer_length -= 3; /*Start reading from PDN_TYPE, so skip the first three octets*/
  buf_len_in_bits = buffer_length * 8;  

  /*SKIP PDN_TYPE*/
  num_bits_to_read = 4; 
  pdn_conn_req->pdn_data.pdn_type = b_unpackb(src, pos, num_bits_to_read);  
  pos += num_bits_to_read;
  buf_len_in_bits -= num_bits_to_read;

  /*Request type*/              
  pdn_conn_req->request_type = b_unpackb(src, pos, num_bits_to_read);  
  pos += num_bits_to_read;
  buf_len_in_bits -= num_bits_to_read;

  /*ESM information transfer flag*/
  /*Get IEI*/
  unpacked_bytes = b_unpackb(src, pos, num_bits_to_read);  
  if(esm_info_iei == unpacked_bytes)
  {
    pos += num_bits_to_read;
    buf_len_in_bits -= num_bits_to_read;

    if(buf_len_in_bits)
    {
      /*Get the TRANSFER_FLAG value*/      
      pdn_conn_req->pdn_data.esm_info_transfer_flag = b_unpackb(src, pos, num_bits_to_read);  
      pos += num_bits_to_read;
      buf_len_in_bits -= num_bits_to_read;
    }
  }

  if(buf_len_in_bits)
  {
    /*Access point name*/  
    /*Get IEI*/
    num_bits_to_read = 8;
    unpacked_bytes = b_unpackb(src, pos, num_bits_to_read);  

    /*Get the APN_ADDRESS value*/
    if(apn_iei == unpacked_bytes)
    {
      pos += num_bits_to_read;
      buf_len_in_bits -= num_bits_to_read;
      pdn_conn_req->pdn_data.apn_name.valid = TRUE;
      /*Get the length of APN_ADDRESS_IEI*/
      pdn_conn_req->pdn_data.apn_name.apn_addr_len = b_unpackb(src, pos, num_bits_to_read);  
      pos += num_bits_to_read;
      buf_len_in_bits -= num_bits_to_read;

      for(index = 0;index < (pdn_conn_req->pdn_data.apn_name.apn_addr_len) &&
                    index < MAX_APN_ADR_LEN; index++)
      {
        pdn_conn_req->pdn_data.apn_name.address[index] = b_unpackb(src, pos, num_bits_to_read); 
        pos += num_bits_to_read;
        buf_len_in_bits -= num_bits_to_read;
      }
    }
  }

  num_bits_to_read = 8;

  /*Protocol configuration options*/ 
  if(buf_len_in_bits > num_bits_to_read)
  { 
    /*Get IEI*/
    unpacked_bytes = b_unpackb(src, pos, num_bits_to_read);
    if(unpacked_bytes == PROT_CONF_OPT)
    {
      pos += num_bits_to_read;
      buf_len_in_bits -= num_bits_to_read;

      dummy_buf_len = buf_len_in_bits;
      pdn_conn_req->pdn_data.protocol_config_options.valid =
         decode_prot_conf_opt(src,&pos,&buf_len_in_bits,&pdn_conn_req->pdn_data.protocol_config_options,&dummy_buf_len);
    }
  }

#ifdef FEATURE_LTE_REL10
  /*Device properties IE*/
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
     (reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE))
  {
    num_bits_to_read = 8;
    if(buf_len_in_bits)
    {
      /*Get IEI*/
      unpacked_bytes = b_unpackb(src, pos, num_bits_to_read);
      if(((unpacked_bytes & 0xF0)>>4) == device_prop_iei)
      {
        pos += num_bits_to_read;
        buf_len_in_bits -= num_bits_to_read;
        pdn_conn_req->pdn_data.low_priority_signaling = (boolean)(unpacked_bytes & 0xF);
      }
    }
  }
#endif
}

/*===========================================================================

FUNCTION  DECODE_PDN_DISCONNECT_REJ

DESCRIPTION
  This function decodes PDN_DISCONNECT_REJECT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_pdn_disconnect_rej
(
   byte                                    *src,
   word                                    *pos, 
   word                                    *buffer_length,   
   lte_nas_esm_pdn_disconnect_rej          *pdn_disconnect_rej,
   word                                    *esm_len_in_bits
)
{
  byte num_bits_to_read =0, unpackedbytes =0;

  if(*buffer_length < (NAS_ESM_PDN_DISCONNECT_REJ - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode PDN DISCONNECT REJECT",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE;
  }

  /*--------------------------------------------------
                Check for ESM Cause
  --------------------------------------------------*/    
  num_bits_to_read = 8;           
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

  if(populate_valid_esm_cause(&pdn_disconnect_rej->esm_cause.esm_cause,
                              unpackedbytes)  == FALSE)
  {                        
    MSG_ERROR_DS_1(MM_SUB, "ESM cause value is %d, unknown ESM Value received\n",
                   unpackedbytes);
    return FALSE;
  }
  MSG_LOW_DS_1(MM_SUB, "ESM Cause value is %d\n", unpackedbytes);
  ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
  pdn_disconnect_rej->esm_cause.valid = TRUE;
  /*--------------------------------------------------
         Protocol Configuration Options (Optional)
  --------------------------------------------------*/
  if(*buffer_length >= num_bits_to_read)
  {
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
    if(unpackedbytes == PROT_CONF_OPT)
    {
      pdn_disconnect_rej->protocol_conf_opt.valid = 
        decode_prot_conf_opt(src,pos,buffer_length,&pdn_disconnect_rej->protocol_conf_opt,esm_len_in_bits);
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB, "ESM: unknown iei_val %d", unpackedbytes);
    }
  }
  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_BEARER_MODIFI_CONTEXT_REJ

DESCRIPTION
  This function decodes BEARER_MODIFICATION_CONTEXT_REJECT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_bearer_modifi_context_rej
(
   byte                                    *src,
   word                                    *pos, 
   word                                    *buffer_length,   
   lte_nas_esm_bearer_resource_modifi_rej  *bearer_resource_modifi_rej,
   word                                    *esm_len_in_bits
 )
{
  byte num_bits_to_read =0, unpackedbytes =0;
  word original_buffer_len = 0;
  byte opt_val_iei, ie_length = 0;
  boolean pco_decoded = FALSE;
#ifdef FEATURE_LTE_REL10
  boolean t3396_decoded = FALSE;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);
#endif
  
  if(*buffer_length < (NAS_ESM_BEARER_RES_MODIFI_REJ_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode BEARER RESOURCE MODIFICATION REJECT",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE;
  }
  
  /*--------------------------------------------------
                Check for ESM Cause
  --------------------------------------------------*/    
  num_bits_to_read = 8;           
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

  if(populate_valid_esm_cause(&bearer_resource_modifi_rej->esm_cause.esm_cause,
                              unpackedbytes)  == FALSE)
  {                        
    MSG_ERROR_DS_1(MM_SUB, "ESM cause value is %d, unknown ESM Value received\n",
                   unpackedbytes);
    return FALSE;
  }

  ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
  bearer_resource_modifi_rej->esm_cause.valid = TRUE;

  if(bearer_resource_modifi_rej->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES)
  {
    bearer_resource_modifi_rej->backoff_timer.active = TRUE;
    bearer_resource_modifi_rej->backoff_timer.timer_count = 0;
  }

  /* Decode Optional IEs */
  original_buffer_len = *buffer_length;

  while(*buffer_length)
  {
    if(*buffer_length > original_buffer_len)
    {  
      MSG_ERROR_DS_0(MM_SUB, "*** Buffer overflow occured. Quit parsing further IEs for safety reasons ***");
      *buffer_length = 0;
      return TRUE;
    }

    num_bits_to_read = 8;
    if(*buffer_length >= num_bits_to_read)
    {
      opt_val_iei = b_unpackb(src, *pos, num_bits_to_read); /* Read the IEI value */
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_len_in_bits);

      switch(opt_val_iei)
      {
        case PROT_CONF_OPT: /* Protocol Configuration Options */
          if(pco_decoded == TRUE)
          {
            MSG_ERROR_DS_0(MM_SUB, "PROT_CONF_OPT IEI is repeated, skipping this IE");
            num_bits_to_read = 8;
            if(*buffer_length >= num_bits_to_read)
            {
              ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
              ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
              /* Skip the IE */
              ADJUST_OFFSET_AND_BUF_LEN(ie_length*8, pos, buffer_length, esm_len_in_bits);
            }
            else
            {
              MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
              *buffer_length = 0;
              return TRUE;
            }
            break;
          }

          bearer_resource_modifi_rej->protocol_conf_opt.valid = 
          decode_prot_conf_opt(src, pos, buffer_length,
                                &bearer_resource_modifi_rej->protocol_conf_opt,
                                   esm_len_in_bits);
          pco_decoded = TRUE;
          break;

#ifdef FEATURE_LTE_REL10
        case T3396_VALUE_IEI: /* T3396 value */
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
          {
            if(t3396_decoded == TRUE)
            {
              MSG_ERROR_DS_0(MM_SUB, "T3396 value IEI is repeated, skipping this IEI");
              num_bits_to_read = 8;
              if(*buffer_length >= num_bits_to_read)
              {
                ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
                ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
                /* Skip the IE */
                ADJUST_OFFSET_AND_BUF_LEN(ie_length*8, pos, buffer_length, esm_len_in_bits);
              }
              else
              {
                MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
                *buffer_length = 0;
                return TRUE;
              }
              break;
            }

            if(bearer_resource_modifi_rej->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES)
            {
              decode_t3396_value(src, pos, buffer_length,
                                  &bearer_resource_modifi_rej->backoff_timer,
                                      esm_len_in_bits);
            }
            else /* This IE shall not be included when ESM Cause is not NAS_ESM_INSUFFICIENT_RESOURCES(#26).
                    Skip decoding of this IE */
            {
              num_bits_to_read = 8;
              if(*buffer_length >= num_bits_to_read)
              {
                ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
                ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
                /* Skip the IE */
                ADJUST_OFFSET_AND_BUF_LEN(ie_length*8, pos, buffer_length, esm_len_in_bits);
              }
              else
              {
                MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
                *buffer_length = 0;
                return TRUE;
              }
            }
            t3396_decoded = TRUE;
            break;
          }
#endif

        default:
          MSG_ERROR_DS_1(MM_SUB, "NAS_MSG_LIB: Unknown IEI value %d", opt_val_iei);
          *buffer_length = 0;
          return TRUE;
      } /* END switch(opt_val_iei) */
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB, "Insufficient buffer length %d", *buffer_length);
      *buffer_length = 0;
      return TRUE;
    }

  }/* END while(*buffer_length) */
  return TRUE;
}

/*===========================================================================
FUNCTION  DECODE_MODIFI_EPS_CONTEXT_REQ

DESCRIPTION
  This function decodes MODIFY EPS BEARER CONTEXT REQUEST message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
boolean decode_modifi_eps_context_req
(
   byte                                        *src,
   word                                        *pos, 
   word                                        *buffer_length,   
   lte_nas_esm_modify_eps_bearer_ctxt_req      *modify_bearer_ctxt_req,
   word                                        *esm_len_in_bits
)
{
  byte num_bits_to_read =0,unpackedbytes =0,iei_val = 0;
  boolean valid_ie = TRUE;
  byte iei_len = 0;
  word iei_pos = 0,iei_buffer_len = 0;
  word temp_esm_rem_len = 0;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
  boolean tft_dec = FALSE, qos_dec = FALSE, sapi_dec = FALSE;
  boolean rp_dec  = FALSE, pfi_dec = FALSE, apn_dec  = FALSE;
  boolean pco_dec = FALSE, qos_new_dec = FALSE;
  original_buffer_len = *buffer_length;
#endif

  /********************************************************************
      MODIFY BEARER CONTEXT REQUEST shall contain 3 octets minimum, 
   reaching this stage implies that 3 octets have already been parsed.
  *********************************************************************/

                          /********** Decode Optional IEs **********/
  while(*esm_len_in_bits)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {  
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;     
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);

    /* IEI length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /*Store offset address & remaining buffer length, in case any octet 
    is faulty this info would be required to jump to next IEI*/
    iei_pos = *pos + 8; 
    iei_buffer_len = *buffer_length - 8;
    temp_esm_rem_len = *esm_len_in_bits - 8;

    switch(iei_val)
    {
      /***** EPS quality of service *****/
      case EPS_QOS:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(qos_dec == TRUE)
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip repeated EPS QoS IE. IEI = %d", EPS_QOS);
          valid_ie = FALSE;
          break;
        }
        qos_dec = TRUE;
#endif      
      valid_ie = decode_eps_qos(src,pos,buffer_length,&modify_bearer_ctxt_req->sdf_qos,esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        modify_bearer_ctxt_req->sdf_qos.valid = FALSE;
      }
      else
      {
        modify_bearer_ctxt_req->sdf_qos.valid = TRUE;
      }
      break;

    /*==============================================================
                 Traffic flow template (Optional IEI)
    ==============================================================*/
    case TRAFFIC_FLOW_TEMPLATE:
     #ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(tft_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", TRAFFIC_FLOW_TEMPLATE);
        valid_ie = FALSE;
        break;
      }
      tft_dec = TRUE;
#endif  
      if(!iei_len)
      {
        MSG_ERROR_DS_0(MM_SUB, "Trafic Flow Template length is 0, discarding this IEI\n");
        *buffer_length = 0;
        *esm_len_in_bits  = 0;
        return TRUE;
      }
      else if(*buffer_length < iei_len)
      {
        MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to deocde TFT, truncating Message parser");
        *buffer_length = 0;
        *esm_len_in_bits  = 0;
        return TRUE;   
      }
      valid_ie = decode_tft(src,pos,buffer_length,&modify_bearer_ctxt_req->tft,esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        modify_bearer_ctxt_req->tft.valid = FALSE;
        *buffer_length = 0;
        *esm_len_in_bits  = 0;
        return TRUE; 
      }   
      modify_bearer_ctxt_req->tft.valid = TRUE;
      break;

       /***** Quality of service *****/
      case NEGOTIATED_QOS:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(qos_new_dec == TRUE)
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip repeated QoS IE. IEI = %d", NEGOTIATED_QOS);
          valid_ie = FALSE;
          break;
        }
        qos_new_dec = TRUE;
#endif     
        valid_ie = decode_qos(src,
                              pos,
                              buffer_length,
                              &modify_bearer_ctxt_req->neg_qos,
                              esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        modify_bearer_ctxt_req->neg_qos.valid  = FALSE;
      }
      else
      {
        modify_bearer_ctxt_req->neg_qos.valid = TRUE;     
      }
      break;

    /*==============================================================
            LLC service access point identifier (Optional IEI)
    ==============================================================*/
    case NEGOGTIATED_LLC_SAPI:
      /* ----- Skip 4 spare bits ---- */
      num_bits_to_read = 4;     
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
      modify_bearer_ctxt_req->neg_llc_sapi.sapi = b_unpackb(src, *pos, num_bits_to_read);
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(sapi_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", NEGOGTIATED_LLC_SAPI);
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
        break;
      }
      sapi_dec = TRUE;
#endif
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
      modify_bearer_ctxt_req->neg_llc_sapi.valid = TRUE;
      valid_ie = TRUE;
      break;
    /*==============================================================
                  Packet flow Identifier (Optional IEI)
    ==============================================================*/
    case PACKET_FLOW_IDENTIFIER:
      /* ------ Length of Packet Flow Identifier ----- */
      MSG_LOW_DS_1(MM_SUB, "Packet flow identifier length is %d\n", iei_len);
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(pfi_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", PACKET_FLOW_IDENTIFIER);
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
        break;
      }
      pfi_dec = TRUE;
#endif
      if(iei_len)
      {
        /* ----- Skip 1 reserved bit ---- */
        num_bits_to_read = 1;
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
        /* ----- Packet flow identifier value ---- */
        num_bits_to_read = 7;
        unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);        
        if(unpackedbytes > MAX_PACKET_FLOW_IDENTIFIER)
        {
          MSG_ERROR_DS_1(MM_SUB, "packet flow identifier value is %d, it must be within the range 0-127",
                         unpackedbytes);
          return TRUE;
        }
        modify_bearer_ctxt_req->pack_flo_id.packet_flow_id = unpackedbytes;
        MSG_LOW_DS_1(MM_SUB, "Packet flow identifier value is %d\n", unpackedbytes);
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
      }
      valid_ie = TRUE;
      modify_bearer_ctxt_req->pack_flo_id.valid = TRUE;
      break;
    /*==============================================================
             APN aggregate maximum bit rate (Optional IEI)
    ==============================================================*/
    case APN_AGGRESSIVE_MAX_BIT_RATE:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(apn_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now",
                       APN_AGGRESSIVE_MAX_BIT_RATE);
        valid_ie = FALSE;
        break;
      }
      apn_dec = TRUE;
#endif      
      valid_ie = decode_apn_ambr(src,pos,buffer_length,&modify_bearer_ctxt_req->apn_ambr,esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        if(esm_length_check_failed == TRUE)
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
          return TRUE; 
        }
      }
      else
      {
        modify_bearer_ctxt_req->apn_ambr.valid = TRUE;    
      }
      break;
    /*==============================================================
               Protocol configuration options(Optional IEI)
    ==============================================================*/
    case PROT_CONF_OPT:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(pco_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", PROT_CONF_OPT);
        valid_ie = FALSE;
        break;
      }
      pco_dec = TRUE;
#endif      
      valid_ie = decode_prot_conf_opt(src,pos,buffer_length,&modify_bearer_ctxt_req->protocol_conf_opt,esm_len_in_bits);      
      if(valid_ie == FALSE)
      {
        modify_bearer_ctxt_req->protocol_conf_opt.valid = FALSE; 
      }
      else
      {
        modify_bearer_ctxt_req->protocol_conf_opt.valid = TRUE;  
      }
      break;
    /*==============================================================
                     Radio priority (Optional IEI)
    ==============================================================*/
    default:
      num_bits_to_read = 4;
      iei_val = b_unpackb(src,(word)(*pos - 8), num_bits_to_read);

      if(iei_val == RADIO_PRIORITY_IEI)
      {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(rp_dec == TRUE)
        {
          MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", RADIO_PRIORITY_IEI);
          break;
        }
        rp_dec = TRUE;
#endif   
        num_bits_to_read = 3;  
        modify_bearer_ctxt_req->radio_priority.radio_priority = b_unpackb(src, (word)(*pos -3), num_bits_to_read);
        MSG_LOW_DS_1(MM_SUB, "Radio priority value is %d\n",
                     modify_bearer_ctxt_req->radio_priority.radio_priority);
        modify_bearer_ctxt_req->radio_priority.valid = TRUE;      
      }/*End of if(iei_val == RADIO_PRIORITY)*/
      else
      {
        MSG_ERROR_DS_1(MM_SUB, "Received unknown information element with an iei value %d",
                       iei_val);
        return TRUE;
      }
      break;
    }/*end of switch()*/
     /*==============================================
      Check to see if the IEI is faulty, if so move 
      to next IEI by incremement offset address and 
      decrementing available buffer length
     ==============================================*/
    if(valid_ie == FALSE)
    {
      if(*buffer_length < iei_len*8)
      {
        MSG_ERROR_DS_2(MM_SUB, "ESM: buffer_length %d insufficient than iei_len %d",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
        *esm_len_in_bits  = 0;
      }
      else
      {
        *pos = iei_pos + iei_len*8;
        *buffer_length   = iei_buffer_len - iei_len*8;
        *esm_len_in_bits = temp_esm_rem_len - iei_len*8;
        MSG_ERROR_DS_1(MM_SUB, "This Optional IEI %d is faulty, will skip this IEI and continue decoding the next optional IEI",
                       iei_val);
        valid_ie = TRUE;
      }
    }
  } /*end while(*esm_len_in_bits) */
  return TRUE;
} /* end decode_modifi_eps_context_req() */


/*===========================================================================

FUNCTION  DECODE_BEARER_RESOURCE_ALLOC_REJ

DESCRIPTION
  This function decodes BEARER_RESOURCE_ALLOC_REJECT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_bearer_resource_alloc_rej
(
   byte                                        *src,
   word                                        *pos, 
   word                                        *buffer_length,   
   lte_nas_esm_bearer_resource_allocation_rej  *bearer_resource_alloc_rej,
   word                                        *esm_len_in_bits
 )
{
  byte num_bits_to_read =0,unpackedbytes =0;
  word original_buffer_len = 0;
  byte opt_val_iei, ie_length = 0;
  boolean pco_decoded = FALSE;

#ifdef FEATURE_LTE_REL10
  boolean t3396_decoded = FALSE;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

  if(*buffer_length < (NAS_ESM_BEARER_RES_ALLOC_REJ_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode BEARER RESOURCE ALLOCATION REJECT",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE;
  }
  
  /*--------------------------------------------------
                Check for ESM Cause
  --------------------------------------------------*/
    
  num_bits_to_read = 8;           
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

  if(populate_valid_esm_cause(&bearer_resource_alloc_rej->esm_cause.esm_cause,
                              unpackedbytes)  == FALSE)
  {                        
    MSG_ERROR_DS_1(MM_SUB, "ESM cause value is %d, unknown ESM Value received\n",
                   unpackedbytes);
    return FALSE;
  }

  MSG_HIGH_DS_1(MM_SUB, "ESM Cause value is %d\n", unpackedbytes);
  ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
  bearer_resource_alloc_rej->esm_cause.valid = TRUE;

  if(bearer_resource_alloc_rej->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES)
  {
    bearer_resource_alloc_rej->backoff_timer.active = TRUE;
    bearer_resource_alloc_rej->backoff_timer.timer_count = 0;
  }

  /* Decode Optional IEs */
  original_buffer_len = *buffer_length;

  while(*buffer_length)
  {
    if(*buffer_length > original_buffer_len)
    {  
      MSG_ERROR_DS_0(MM_SUB, "*** Buffer overflow occured. Quit parsing further IEs for safety reasons ***");
      *buffer_length = 0;
      return TRUE;
    }

    num_bits_to_read = 8;
    if(*buffer_length >= num_bits_to_read)
    {
      opt_val_iei = b_unpackb(src, *pos, num_bits_to_read); /* Read the IEI value */
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_len_in_bits);

      switch(opt_val_iei)
      {
        case PROT_CONF_OPT: /* Protocol Configuration Options */
          if(pco_decoded == TRUE)
          {
            MSG_ERROR_DS_0(MM_SUB, "PROT_CONF_OPT IEI is repeated, skipping this IE");
            num_bits_to_read = 8;
            if(*buffer_length >= num_bits_to_read)
            {
              ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
              ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
              /* Skip the IE */
              ADJUST_OFFSET_AND_BUF_LEN(ie_length*8, pos, buffer_length, esm_len_in_bits);
            }
            else
            {
              MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
              *buffer_length = 0;
              return TRUE;
            }
            break;
          }

          MSG_LOW_DS_0(MM_SUB, "Decoding Protocol Configuration Options IE\n");
          bearer_resource_alloc_rej->protocol_conf_opt.valid = 
          decode_prot_conf_opt(src, pos, buffer_length,
                                 &bearer_resource_alloc_rej->protocol_conf_opt,
                                    esm_len_in_bits);
          pco_decoded = TRUE;
          break;

#ifdef FEATURE_LTE_REL10
        case T3396_VALUE_IEI: /* T3396 value */
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
          {
            if(t3396_decoded == TRUE)
            {
              MSG_ERROR_DS_0(MM_SUB, "T3396 value IEI is repeated, skipping this IEI");
              num_bits_to_read = 8;
              if(*buffer_length >= num_bits_to_read)
              {
                ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
                ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
                /* Skip the IE */
                ADJUST_OFFSET_AND_BUF_LEN(ie_length*8, pos, buffer_length, esm_len_in_bits);
              }
              else
              {
                MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
                *buffer_length = 0;
                return TRUE;
              }
              break;
            }

            MSG_LOW_DS_0(MM_SUB, "Decoding T3396 value IE\n");
            if(bearer_resource_alloc_rej->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES)
            {
              decode_t3396_value(src, pos, buffer_length,
                                  &bearer_resource_alloc_rej->backoff_timer,
                                      esm_len_in_bits);
            }
            else /* This IE shall not be included when ESM Cause is not NAS_ESM_INSUFFICIENT_RESOURCES(#26).
                    Skip decoding of this IE */
            {
              num_bits_to_read = 8;
              if(*buffer_length >= num_bits_to_read)
              {
                ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
                ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
                /* Skip the IE */
                ADJUST_OFFSET_AND_BUF_LEN(ie_length*8, pos, buffer_length, esm_len_in_bits);
              }
              else
              {
                MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
                *buffer_length = 0;
                return TRUE;
              }
            }
            t3396_decoded = TRUE;
            break;
          }
#endif
        default:
          MSG_ERROR_DS_1(MM_SUB, "NAS_MSG_LIB: Unknown IEI value %d", opt_val_iei);
          *buffer_length = 0;
          return TRUE;
      } /* END switch(opt_val_iei) */
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB, "Insufficient buffer length %d", *buffer_length);
      *buffer_length = 0;
      return TRUE;
    }

  }/* END while(*buffer_length) */
  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_DEACTIVATE_EPS_BEARER_REQ

DESCRIPTION
  This function decodes DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_deactivate_eps_bearer_req
(
   byte                                     *src,
   word                                     *pos, 
   word                                     *buffer_length,
   lte_nas_esm_deact_eps_bearer_context_req *deactivate_bearer_context_req,
   word                                     *esm_len_in_bits
 )
{
  byte num_bits_to_read =0,unpackedbytes =0;
  word original_buffer_len = 0;
  byte ie_length = 0;
  boolean pco_decoded = FALSE;

#ifdef FEATURE_LTE_REL11
    boolean t3396_decoded = FALSE;
    emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT(emm_ctrl_data_ptr != NULL);
#endif


  if(*buffer_length < (NAS_ESM_DEACT_BEARER_REQ_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode DEACTIVATE BEARER CONTEXT REQUEST",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE;
  }
  
  /*--------------------------------------------------
                Check for ESM Cause
  --------------------------------------------------*/
    
  num_bits_to_read = 8;           
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

  if(populate_valid_esm_cause(&deactivate_bearer_context_req->esm_cause.esm_cause,
                              unpackedbytes)  == FALSE)
  {                        
    MSG_ERROR_DS_1(MM_SUB, "ESM cause value is %d, unknown ESM Value received\n",
                   unpackedbytes);
    return FALSE;
  }

  MSG_HIGH_DS_1(MM_SUB, "ESM Cause value is %d\n", unpackedbytes);
  ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
  deactivate_bearer_context_req->esm_cause.valid = TRUE;


  if(deactivate_bearer_context_req->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES)
  {
#ifdef FEATURE_LTE_REL11
    deactivate_bearer_context_req->backoff_timer.active = TRUE;
#else
    deactivate_bearer_context_req->backoff_timer.active = FALSE;
#endif
    deactivate_bearer_context_req->backoff_timer.timer_count = 0;
  }


  /* Decode Optional IEs */
  original_buffer_len = *buffer_length;

  while(*buffer_length)
  {
    if(*buffer_length > original_buffer_len)
    {  
      MSG_ERROR_DS_0(MM_SUB, "*** Buffer overflow occured. Quit parsing further IEs for safety reasons ***");
      *buffer_length = 0;
      return TRUE;
    }
    num_bits_to_read = 8;
    if(*buffer_length >= num_bits_to_read)
    {
      unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
      switch(unpackedbytes)
      {
        case PROT_CONF_OPT:/* Protocol Configuration Options (Optional)*/
         if(pco_decoded == TRUE)
         {
           MSG_ERROR_DS_0(MM_SUB, "PROT_CONF_OPT IEI is repeated, skipping this IE");
           num_bits_to_read = 8;
           if(*buffer_length >= num_bits_to_read)
           {
             ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
             ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
             /* Skip the IE */
             ADJUST_OFFSET_AND_BUF_LEN(ie_length*8, pos, buffer_length, esm_len_in_bits);
           }
           else
           {
             MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
             *buffer_length = 0;
             return TRUE;
           }
           break;
         }
         deactivate_bearer_context_req->protocol_conf_opt.valid  = 
         decode_prot_conf_opt(src,pos,buffer_length,&deactivate_bearer_context_req->protocol_conf_opt,esm_len_in_bits);
         pco_decoded = TRUE;
         break;
#ifdef FEATURE_LTE_REL11
        case T3396_VALUE_IEI: /* T3396 value */
         if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
         {
           if(t3396_decoded == TRUE)
           {
              MSG_ERROR_DS_0(MM_SUB, "T3396 value IEI is repeated, skipping this IEI");
              num_bits_to_read = 8;
              if(*buffer_length >= num_bits_to_read)
              {
                ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
                ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
                /* Skip the IE */
                ADJUST_OFFSET_AND_BUF_LEN(ie_length*8, pos, buffer_length, esm_len_in_bits);
              }
              else
              {
                MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
                *buffer_length = 0;
                return TRUE;
              }
              break;
           }
           if(deactivate_bearer_context_req->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES)
           {
              decode_t3396_value(src, pos, buffer_length,
                                    &deactivate_bearer_context_req->backoff_timer,
                                          esm_len_in_bits);
           }
           else   /* This IE shall not be included when ESM Cause is not NAS_ESM_INSUFFICIENT_RESOURCES(#26).
                        Skip decoding of this IE */
           {
              num_bits_to_read = 8;
              if(*buffer_length >= num_bits_to_read)
              {
                ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
                ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
                /* Skip the IE */
                ADJUST_OFFSET_AND_BUF_LEN(ie_length*8, pos, buffer_length, esm_len_in_bits);
              }
              else
              {
                MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
                *buffer_length = 0;
                 return TRUE;
              }
           }
           t3396_decoded = TRUE;
           break;
         }
         break;
#endif
         default:
         {
           MSG_ERROR_DS_1(MM_SUB, "ESM: unknown iei_val %d", unpackedbytes);
         }
       }
     }
     else
     {
       MSG_ERROR_DS_1(MM_SUB, "Insufficient buffer length %d", *buffer_length);
       *buffer_length = 0;
       return TRUE;
     }
  }
  return TRUE;
}

/*===========================================================================
FUNCTION  DECODE_DEDICATED_EPS_BEARER_REQ

DESCRIPTION
  This function decodes DEDICATED EPS BEARER CONTEXT REQUEST message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
static boolean decode_dedicated_eps_bearer_req
(
  byte                                         *src,
  word                                         *pos,
  word                                         *buffer_length,
  lte_nas_esm_act_dedicated_bearer_context_req *dedicated_bearer_context_req,
  word                                         *esm_len_in_bits
)
{
  byte num_bits_to_read =0, iei_val=0,unpackedbytes =0,len=0;
  boolean valid_ie = TRUE; 
  byte iei_len = 0;
  word iei_pos = 0,iei_buffer_len = 0;
  word temp_esm_rem_len = 0;
#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
  boolean ti_dec  = FALSE, qos_dec = FALSE, sapi_dec = FALSE;
  boolean rp_dec  = FALSE, pfi_dec = FALSE;
  boolean pco_dec = FALSE;
#endif
   
  if(*buffer_length < (NAS_ESM_ACT_DED_REQ_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode ACTIVATE DEDICATED BEARER CONTEXT REQUEST",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE;
  }
  
  /*------------------------------------------------
     half octet(spare) + Linked EPS bearer identity
  -------------------------------------------------*/
  num_bits_to_read = 4;     
  ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);

  num_bits_to_read = 4;     
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  if(unpackedbytes <= LINKED_EPS_BEARER_RESERVED_VALUE)
  {
    MSG_ERROR_DS_0(MM_SUB, "NAS_MSG_LIB: Linked EPS Bearer is %d, values 0 to 4 are reserved");
    *buffer_length   = 0;
    *esm_len_in_bits = 0;
    return FALSE;
  }
  dedicated_bearer_context_req->linked_eps_bearer_identity = unpackedbytes;
  ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);

  /***** EPS quality of service *****/
  valid_ie = decode_eps_qos(src,pos,buffer_length,&dedicated_bearer_context_req->sdf_qos,esm_len_in_bits);
  if(valid_ie == FALSE)
  {
    dedicated_bearer_context_req->sdf_qos.valid = FALSE;
    return FALSE; 
  }  
  dedicated_bearer_context_req->sdf_qos.valid = TRUE;

  /*-------------------------------------
         Traffic flow template
  -------------------------------------*/
  /* Verify the length of TFT (for mandatory field)*/
  num_bits_to_read = 8;
  len = b_unpackb(src, *pos, num_bits_to_read);
  if(!len)
  {
    MSG_ERROR_DS_0(MM_SUB, "Trafic Flow Template length is 0, truncating Message parser\n");
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
     return FALSE;
  }
  else if(*buffer_length < len)
  {
    MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to deocde TFT, truncating Message parser\n");
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE;   
  }
  valid_ie = decode_tft(src,pos,buffer_length,&dedicated_bearer_context_req->tft,esm_len_in_bits);

  if(valid_ie == FALSE)
  {
    dedicated_bearer_context_req->tft.valid = FALSE;
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE; 
  }   
  dedicated_bearer_context_req->tft.valid = TRUE;


                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*esm_len_in_bits)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {  
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;     
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);

    /* IEI length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /*Store offset address & remaining buffer length, in case any octet 
    is faulty this info would be required to jump to next IEI*/
    iei_pos = *pos + 8; 
    iei_buffer_len = *buffer_length - 8;
    temp_esm_rem_len = *esm_len_in_bits - 8;

    switch(iei_val)
     {
      case TRANSACTION_IDENTIFIER:
      /*-------------------------------------
          Transaction IDENTIFIER
      --------------------------------------*/
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(ti_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", TRANSACTION_IDENTIFIER);
        valid_ie = FALSE;
        break;
      }
      ti_dec = TRUE;
#endif
      valid_ie = decode_transaction_id(src,pos,buffer_length,&dedicated_bearer_context_req->ti,esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        if(esm_length_check_failed == TRUE)
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
          return TRUE; 
        }
        else
        {
          /*If an IEI decoding doesn't fail due to length check criteria, 
          then decoding can continue safely - if length check fails, 
          then all IEIs following this IEI would be wrongly interpreted, 
          so it is better to skip parsing*/
          dedicated_bearer_context_req->ti.valid = FALSE;  
        }
      }
      else
      {
        dedicated_bearer_context_req->ti.valid = TRUE;    
      }
      break;

      /***** Negotiated QoS IE *****/
      case NEGOTIATED_QOS:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(qos_dec == TRUE)
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Skip repeated Negotiated QOS IE. IEI = %d",
                         NEGOTIATED_QOS);
          valid_ie = FALSE;
          break;
        }
        qos_dec = TRUE;
#endif      
        valid_ie = decode_qos(src,
                              pos,
                              buffer_length,
                              &dedicated_bearer_context_req->neg_qos,
                              esm_len_in_bits);
        if(valid_ie == FALSE)
        {
           dedicated_bearer_context_req->neg_qos.valid  = FALSE;
        }
        else
        {
          dedicated_bearer_context_req->neg_qos.valid = TRUE;     
        }
        break;

      case NEGOGTIATED_LLC_SAPI:
      /*-------------------------------------
               Negotiated LLC SAPI
      --------------------------------------*/
      
      /* ----- Skip 4 spare bits ---- */
      num_bits_to_read = 4;     
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(sapi_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", NEGOGTIATED_LLC_SAPI);
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
        break;
      }
      sapi_dec = TRUE;
#endif

      dedicated_bearer_context_req->neg_llc_sapi.sapi = b_unpackb(src, *pos, num_bits_to_read);
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);

      dedicated_bearer_context_req->neg_llc_sapi.valid = TRUE;
      valid_ie = TRUE;
      break;

      case PACKET_FLOW_IDENTIFIER:
      /*-------------------------------------
             Packet flow Identifier
      --------------------------------------*/
        
      /* ------ Length of Packet Flow Identifier ----- */
      num_bits_to_read = 8;
      len = b_unpackb(src, *pos, num_bits_to_read);
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(pfi_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", PACKET_FLOW_IDENTIFIER);
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
      }
      pfi_dec = TRUE;
      valid_ie = TRUE;  
      break;
#endif

      if(len)
      {
        /* ----- Skip 1 reserved bit ---- */
        num_bits_to_read = 1;
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);

        /* ----- Packet flow identifier value ---- */
        num_bits_to_read = 7;
        unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
        
        if(unpackedbytes > MAX_PACKET_FLOW_IDENTIFIER)
        {
          MSG_ERROR_DS_1(MM_SUB, "packet flow identifier value is %d, it must be within the range 0-127", unpackedbytes);
          return TRUE;
        }
        dedicated_bearer_context_req->pack_flo_id.packet_flow_id = unpackedbytes;
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
      }
      dedicated_bearer_context_req->pack_flo_id.valid = TRUE;

      break;

      case PROT_CONF_OPT:
      /*-------------------------------------
           Protocol configuration options
      --------------------------------------*/ 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(pco_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", PROT_CONF_OPT);
        valid_ie = FALSE;
        break;
      }
      pco_dec = TRUE;
#endif      

      valid_ie = decode_prot_conf_opt(src,pos,buffer_length,&dedicated_bearer_context_req->protocol_conf_opt,esm_len_in_bits);
      
      if(valid_ie == FALSE)
      {
        if(esm_length_check_failed == TRUE)
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
          return TRUE; 
        }
        else
        {
          /*If an IEI decoding doesn't fail due to length check criteria, 
          then decoding can continue safely - if length check fails, 
          then all IEIs following this IEI would be wrongly interpreted, 
          so it is better to skip parsing*/
          dedicated_bearer_context_req->protocol_conf_opt.valid = FALSE; 
        }
      }
      else
      {
        dedicated_bearer_context_req->protocol_conf_opt.valid = TRUE;  
      }

      break;

      default:
      /*pos is already incremented (before calling switch(iei_val), 
        so decrease pos by 8(so that it points to the correct octet)*/
      num_bits_to_read = 4;
      iei_val = b_unpackb(src,(word)(*pos - 8), num_bits_to_read);

      if(iei_val == RADIO_PRIORITY_IEI)
      {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(rp_dec == TRUE)
        {
          MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", RADIO_PRIORITY_IEI);
          break;
        }
        rp_dec = TRUE;
#endif  
        num_bits_to_read = 3;     
        dedicated_bearer_context_req->radio_priority.radio_priority = b_unpackb(src,(word)(*pos-3), num_bits_to_read);
        MSG_HIGH_DS_1(MM_SUB, "Radio priority value is %d\n",
                     dedicated_bearer_context_req->radio_priority.radio_priority);
        dedicated_bearer_context_req->radio_priority.valid = TRUE;     
      }/*End of if(iei_val == RADIO_PRIORITY)*/
      else
      {
        MSG_ERROR_DS_1(MM_SUB, "Received unknown information element with an iei value %d",
                       iei_val);
        return TRUE;
      }
      break;

     }/*End of switch(iei_val)*/
    /*==============================================
      Check to see if the IEI is faulty, if so move 
      to next IEI by incremement offset address and 
      decrementing available buffer length
     ==============================================*/
    if(valid_ie == FALSE)
    {
      if(*buffer_length < iei_len*8)
      {
        MSG_ERROR_DS_2(MM_SUB, "ESM: buffer_length %d insufficient than iei_len %d",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
        *esm_len_in_bits  = 0;
      }
      else
      {
        *pos = iei_pos + iei_len*8;
        *buffer_length   = iei_buffer_len - iei_len*8;
        *esm_len_in_bits = temp_esm_rem_len - iei_len*8;
        MSG_ERROR_DS_1(MM_SUB, "This Optional IEI %d is faulty, will skip this IEI and continue decoding the next optional IEI",
                       iei_val);
        valid_ie = TRUE;
      }
    }
  } /* end while(*esm_len_in_bits) */  
  return TRUE;
} /* end decode_dedicated_eps_bearer_req() */


/*===========================================================================
FUNCTION  DECODE_ACT_DEF_CONTEXT_REQ

DESCRIPTION
  This function decodes ACTIVATE DEFFAULT EPS BEARER CONTEXT REQ message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
static boolean decode_act_def_context_req
(
  byte                                       *src,
  word                                       *pos,
  word                                       *buffer_length,
  lte_nas_esm_act_default_bearer_context_req *def_bearer_context_req,
  word                                       *esm_len_in_bits
)
{
  byte num_bits_to_read =0, iei_val=0,unpackedbytes =0,len=0;
  boolean valid_ie = TRUE; 
  byte min_apn_len = 1, max_apn_len = 100;
  byte iei_len = 0;
  word iei_pos = 0,iei_buffer_len = 0;
  word temp_esm_rem_len = 0;
#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
  boolean ti_dec = FALSE, qos_dec = FALSE, sapi_dec = FALSE;
  boolean rp_dec = FALSE, pfi_dec = FALSE, esm_dec  = FALSE;
  boolean pco_dec = FALSE, apn_dec = FALSE;
#endif
 
  if(*buffer_length < (NAS_ESM_ACT_DEF_REQ_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "Buffer length = %d is insufficient to decode ACTIVATE DEFAULT BEARER CONTEXT REQUEST",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE;
  }
  
  /***** EPS quality of service *****/
  valid_ie = decode_eps_qos(src,
                            pos,
                            buffer_length,
                            &def_bearer_context_req->sdf_qos,
                            esm_len_in_bits);

  if(valid_ie == FALSE)
  {
    def_bearer_context_req->sdf_qos.valid = FALSE;
    return FALSE; 
  }
    def_bearer_context_req->sdf_qos.valid = TRUE;
  
  /*-------------------------------------
            Access point name
  --------------------------------------*/
  num_bits_to_read = 8;
  len = b_unpackb(src, *pos, num_bits_to_read);
  if((len < min_apn_len)||(len > max_apn_len))
  {
    MSG_ERROR_DS_0(MM_SUB, "Expecting Access Point Name, received unknown mandatory field\n");
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE;
  }
  else if(len > (*esm_len_in_bits/8))
  {
    MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to decode Access point name, truncating message parser\n");
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE;
  }

  valid_ie = decode_apn_addr(src,pos,buffer_length,&def_bearer_context_req->access_point_name,esm_len_in_bits);
  if(valid_ie == FALSE)
  {
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE; 
  }

  /*-------------------------------------
                PDN Address
  --------------------------------------*/      
  valid_ie = decode_pdn_address(src,pos,buffer_length,&def_bearer_context_req->pdn_address,esm_len_in_bits);
  if(valid_ie == FALSE)
  {
    return FALSE; 
  }
  else
  {
    def_bearer_context_req->pdn_address.valid = TRUE;  
  }


                          /********** Decode Optional IEs **********/

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*esm_len_in_bits)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow, buff_len %d, original_buff_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif

    num_bits_to_read = 8;     
    iei_val = b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);

    /* IEI length */
    iei_len = b_unpackb(src, *pos, num_bits_to_read);

    /*Store offset address & remaining buffer length, in case any octet 
    is faulty this info would be required to jump to next IEI*/
    iei_pos = *pos + 8; 
    iei_buffer_len = *buffer_length - 8;
    temp_esm_rem_len = *esm_len_in_bits - 8;

    switch(iei_val)
    {
     case TRANSACTION_IDENTIFIER:
      /*-------------------------------------
             Transaction IDENTIFIER
      --------------------------------------*/
      iei_len = b_unpackb(src, *pos, num_bits_to_read);
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(ti_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", TRANSACTION_IDENTIFIER);
        valid_ie = FALSE;
        break;
      }
      ti_dec = TRUE;
#endif
      valid_ie = decode_transaction_id(src,pos,buffer_length,&def_bearer_context_req->ti,esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        if(esm_length_check_failed == TRUE)
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
          return TRUE; 
        }
        else
        {
          /*If an IEI decoding doesn't fail due to length check criteria, 
          then decoding can continue safely - if length check fails, 
          then all IEIs following this IEI would be wrongly interpreted, 
          so it is better to skip parsing*/
          def_bearer_context_req->ti.valid = FALSE;  
        }
      } 
      else
      {
        def_bearer_context_req->ti.valid = TRUE;  
      }
      break;

      /***** Negotiated QOS *****/
      case NEGOTIATED_QOS:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(qos_dec == TRUE)
        {
          MSG_ERROR_DS_1(MM_SUB, "Skip repeated Negotiated QoS IE. IEI = %d", NEGOTIATED_QOS);
          valid_ie = FALSE;
          break;
        }
        qos_dec = TRUE;
#endif
        valid_ie = decode_qos(src,
                              pos,
                              buffer_length,
                              &def_bearer_context_req->neg_qos,
                              esm_len_in_bits);
        if(valid_ie == FALSE)
        {
          def_bearer_context_req->neg_qos.valid = FALSE;  
        }
        else
        {
          def_bearer_context_req->neg_qos.valid = TRUE;  
        }
        break;

      case NEGOGTIATED_LLC_SAPI:
      /*-------------------------------------
             Negotiated LLC SAPI
      --------------------------------------*/
      
      /* ----- Skip 4 spare bits ---- */
      num_bits_to_read = 4;     
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(sapi_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", NEGOGTIATED_LLC_SAPI);
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
        break;
      }
      sapi_dec = TRUE;
#endif
      def_bearer_context_req->neg_llc_sapi.sapi = b_unpackb(src, *pos, num_bits_to_read);
      def_bearer_context_req->neg_llc_sapi.valid = TRUE;
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
      valid_ie = TRUE;
      break;

      case PACKET_FLOW_IDENTIFIER:
      /*-------------------------------------
            Packet flow Identifier
      --------------------------------------*/
        
      /* ------ Length of Packet Flow Identifier ----- */
      num_bits_to_read = 8;
      len = b_unpackb(src, *pos, num_bits_to_read);
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);

#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(pfi_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", PACKET_FLOW_IDENTIFIER);
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
        break;
      }
      pfi_dec = TRUE;
#endif
      if(len)
      {
        /* ----- Skip 1 reserved bit ---- */
        num_bits_to_read = 1;
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
        /* ----- Packet flow identifier value ---- */
        num_bits_to_read = 7;
        unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);        
        if(unpackedbytes > MAX_PACKET_FLOW_IDENTIFIER)
        {
          MSG_ERROR_DS_1(MM_SUB, "packet flow identifier value is %d, it must be within the range 0-127",
                         unpackedbytes);
          esm_length_check_failed = TRUE;
          *buffer_length = 0;
          *esm_len_in_bits = 0;         
          return TRUE;
        }
        def_bearer_context_req->pack_flo_id.packet_flow_id = unpackedbytes;
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
        def_bearer_context_req->pack_flo_id.valid = TRUE;
      }
      valid_ie = TRUE;
      break;

      case APN_AGGRESSIVE_MAX_BIT_RATE:
      /*-------------------------------------
            APN_AGGRESSIVE_MAX_BIT_RATE
      --------------------------------------*/  
      iei_len = b_unpackb(src, *pos, num_bits_to_read);
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(apn_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now",
                       APN_AGGRESSIVE_MAX_BIT_RATE);
        valid_ie = FALSE;
        break;
      }
      apn_dec = TRUE;
#endif
      valid_ie = decode_apn_ambr(src,pos,buffer_length,&def_bearer_context_req->apn_ambr,esm_len_in_bits);
      if(valid_ie == FALSE)
      {
        if(esm_length_check_failed == TRUE)
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
          return TRUE; 
        }
        else
        {
          /*If an IEI decoding doesn't fail due to length check criteria, 
          then decoding can continue safely - if length check fails, 
          then all IEIs following this IEI would be wrongly interpreted, 
          so it is better to skip parsing*/
          def_bearer_context_req->apn_ambr.valid = FALSE;   
        }
      }
      else
      {
        def_bearer_context_req->apn_ambr.valid = TRUE;    
      }
      break;
      case ESM_CAUSE_IEI:
      /*-------------------------------------
               ESM cause
      --------------------------------------*/
      
      num_bits_to_read = 8;           
      unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(esm_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", ESM_CAUSE_IEI);
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
        break;
      }
      esm_dec = TRUE;
#endif

      if(populate_valid_esm_cause(&def_bearer_context_req->esm_cause.esm_cause,
                                   unpackedbytes)  == FALSE)
      {                        
        MSG_ERROR_DS_1(MM_SUB, "ESM cause value is %d, unknown ESM Value received\n",
                       unpackedbytes);
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
        def_bearer_context_req->esm_cause.valid = FALSE;
        break;
      }      

      def_bearer_context_req->esm_cause.valid = TRUE;
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);
      valid_ie = TRUE;
      break;

      case PROT_CONF_OPT:
      iei_len = b_unpackb(src, *pos, num_bits_to_read);
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(pco_dec == TRUE)
      {
        MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", PROT_CONF_OPT);
        valid_ie = FALSE;
        break;
      }
      pco_dec = TRUE;
#endif
      /*-------------------------------------
           Protocol configuration options
      --------------------------------------*/       
      valid_ie = decode_prot_conf_opt(src,pos,buffer_length,&def_bearer_context_req->protocol_conf_opt,esm_len_in_bits);      
      if(valid_ie == FALSE)
      {
        if(esm_length_check_failed == TRUE)
        {
          *buffer_length = 0;
          *esm_len_in_bits = 0;
          return TRUE; 
        }
        else
        {
          /*If an IEI decoding doesn't fail due to length check criteria, 
          then decoding can continue safely - if length check fails, 
          then all IEIs following this IEI would be wrongly interpreted, 
          so it is better to skip parsing*/
          def_bearer_context_req->protocol_conf_opt.valid = FALSE;
        }
      }
      else
        {
          def_bearer_context_req->protocol_conf_opt.valid = TRUE;
        }
      break;

      default:
        /*-------------------------------------
                Check for RADIO PRIORITY
              -------------------------------------*/ 
      /*pos is already incremented (before calling switch(iei_val), so decrease pos by 8(so that it points to the correct octet)*/
      num_bits_to_read = 4;
      iei_val = b_unpackb(src, (word)(*pos - 8), num_bits_to_read);

      if(iei_val == RADIO_PRIORITY_IEI)
      {
        /* ------ Get 3 bit value of radio priority value ----- */
#ifdef FEATURE_LTE_NAS_DEEP_SEC
       if(rp_dec == TRUE)
       {
         MSG_ERROR_DS_1(MM_SUB, "iei %d was already decoded skip now", RADIO_PRIORITY_IEI);
         break;
       }
       rp_dec = TRUE;
#endif
       num_bits_to_read = 3;
       def_bearer_context_req->radio_priority.radio_priority = b_unpackb(src, (word)(*pos - 3), num_bits_to_read);
       def_bearer_context_req->radio_priority.valid = TRUE;
      }/*End of if(iei_val == RADIO_PRIORITY)*/
      else
      {
        MSG_ERROR_DS_3(MM_SUB, "Received unknown information element with an iei value %d pos %d len %d",
                       iei_val, *pos, iei_len);
      }
      break;
    }/*End of switch(iei_val)*/
    /*==============================================
      Check to see if the IEI is faulty, if so move 
      to next IEI by incremement offset address and 
      decrementing available buffer length 
     ==============================================*/
    if(valid_ie == FALSE)
    {
      if(esm_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_2(MM_SUB, "ESM: buffer_length %d insufficient than iei_len %d",
                       (*buffer_length/8), iei_len);
        *buffer_length = 0;
        *esm_len_in_bits  = 0;
      }
      else
      {
        *pos = iei_pos + iei_len*8;
        *buffer_length   = iei_buffer_len - iei_len*8;
        *esm_len_in_bits = temp_esm_rem_len - iei_len*8;
        MSG_ERROR_DS_1(MM_SUB, "This Optional IEI %d is faulty, will skip this IEI and continue decoding the next optional IEI",
                       iei_val);
        valid_ie = TRUE;
      }
    }
  } /* end while(*esm_len_in_bits) */
  return TRUE;
} /* end decode_def_bearer_context_request() */


/*===========================================================================

FUNCTION  DECODE_PDN_CONN_REJ

DESCRIPTION
  This function decodes PDN_CONNECTIVITY_REJECT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
static boolean decode_pdn_conn_rej(byte *src,word *pos, word *buffer_length,lte_nas_pdn_connectivity_rej *pdn_conn_rej, word * esm_len_in_bits)
{
  byte unpackedbytes =0, num_bits_to_read=0;    
  word original_buffer_len = 0;
  byte opt_val_iei, ie_length = 0;
  boolean pco_decoded = FALSE;

#ifdef FEATURE_LTE_REL10
  byte security_hdr_type = 0, security_hdr_offset = 0;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  lte_nas_message_id emm_msg_id;
  boolean t3396_decoded = FALSE;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);
#endif
  
  if(*buffer_length < (NAS_ESM_PCR_MIN_LEN - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "Buffer length = %d is insufficient to decode PDN CONNECTIVITY REJECT",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE;
  }
  /*----------------------------------------------------------------
                            ESM cause
  -----------------------------------------------------------------*/
  num_bits_to_read = 8;           
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);  

  if(populate_valid_esm_cause(&pdn_conn_rej->esm_cause.esm_cause,
                               unpackedbytes)  == FALSE)
  {                        
    MSG_ERROR_DS_1(MM_SUB, "ESM cause value is %d, unknown ESM Value received\n",
                   unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }
  pdn_conn_rej->esm_cause.valid = TRUE;

  ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);

  if((pdn_conn_rej->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES) ||
     (pdn_conn_rej->esm_cause.esm_cause == NAS_ESM_UNKNOWN_MISSING_APN))
  {
    pdn_conn_rej->backoff_timer.active = TRUE;
    pdn_conn_rej->backoff_timer.timer_count = 0;
  }

  /* Decode Optional IEs */
  original_buffer_len = *buffer_length;

  while(*esm_len_in_bits)
  {
    if(*buffer_length > original_buffer_len)
    {  
      MSG_ERROR_DS_0(MM_SUB, "*** Buffer overflow occured. Quit parsing further IEs for safety reasons ***");
      *buffer_length = 0;
      return TRUE;
    }

    num_bits_to_read = 8;  
    if(*buffer_length >= num_bits_to_read)
    {
      opt_val_iei = b_unpackb(src, *pos, num_bits_to_read); /* Read the IEI value */
      ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_len_in_bits);

      switch(opt_val_iei)
      {
        case PROT_CONF_OPT: /* Protocol Configuration Options */
          if(pco_decoded == TRUE)
          {
            MSG_ERROR_DS_0(MM_SUB, "PROT_CONF_OPT IEI is repeated, skipping this IE");
            num_bits_to_read = 8;
            if(*buffer_length >= num_bits_to_read)
            {
              ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
              ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
              /* Skip the IE */
              ADJUST_OFFSET_AND_BUF_LEN(ie_length*8, pos, buffer_length, esm_len_in_bits);
            }
            else
            {
              MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
              *buffer_length = 0;
              return TRUE;
            }
            break;
          }

          MSG_LOW_DS_0(MM_SUB, "Decoding Protocol Configuration Options IE\n");
          pdn_conn_rej->protocol_conf_opt.valid = 
          decode_prot_conf_opt(src, pos, buffer_length,
                                &pdn_conn_rej->protocol_conf_opt,
                                   esm_len_in_bits);
          pco_decoded = TRUE;
          break;
     
#ifdef FEATURE_LTE_REL10
        case T3396_VALUE_IEI: /* T3396 value */
          if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
          {
            if(t3396_decoded == TRUE)
            {
              MSG_ERROR_DS_0(MM_SUB, "T3396 value IEI is repeated, skipping this IEI");
              num_bits_to_read = 8;
              if(*buffer_length >= num_bits_to_read)
              {
                ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
                ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);   
                /* Skip the IE */
                ADJUST_OFFSET_AND_BUF_LEN(ie_length*8, pos, buffer_length, esm_len_in_bits);
              }
              else
              {
                MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
                *buffer_length = 0;
                return TRUE;
              }
              break;
            }

            MSG_LOW_DS_0(MM_SUB, "Decoding T3396 value IE\n");
            if((pdn_conn_rej->esm_cause.esm_cause == NAS_ESM_INSUFFICIENT_RESOURCES) ||
               (pdn_conn_rej->esm_cause.esm_cause == NAS_ESM_UNKNOWN_MISSING_APN))
            {
              decode_t3396_value(src, pos, buffer_length,
                                  &pdn_conn_rej->backoff_timer,
                                     esm_len_in_bits);

              num_bits_to_read = 4;
              security_hdr_type = (byte)b_unpackb(emm_ctrl_data_ptr->emm_ota_message_ptr,
                                                   security_hdr_offset,
                                                   num_bits_to_read);
    
              emm_msg_id = emm_ctrl_data_ptr->nas_incoming_msg_ptr->emm_incoming_msg.hdr.msg_id;
              MSG_HIGH_DS_2(MM_SUB, "MSG_LIB: emm_msg_id=%d, sec_hdr=%d",
                            emm_msg_id,
                            security_hdr_type);
   
              if((emm_msg_id == ATTACH_REJECT) &&
                 (security_hdr_type != INTEGRITY_PROTECT) &&
                 (security_hdr_type != INTEGRITY_PROTECT_CIPHER))
              {
                /* Set timer to active and assign a random timer value between 15min to 30min */
                pdn_conn_rej->backoff_timer.active = TRUE;
                pdn_conn_rej->backoff_timer.timer_count = ((15 + mm_get_random_value(15)) * 60 * 1000);
                MSG_HIGH_DS_3(MM_SUB, "MSG_LIB: msg_id=%d: sec_hdr=%d: random backoff_timer value = %d milliseconds",
                              emm_msg_id,
                              security_hdr_type,
                              pdn_conn_rej->backoff_timer.timer_count);
              }
            }
            else /* This IE shall not be included when ESM Cause is not NAS_ESM_INSUFFICIENT_RESOURCES(#26) or
                    NAS_ESM_UNKNOWN_MISSING_APN(#27). Skip decoding of this IE */
            {
              num_bits_to_read = 8;
              if(*buffer_length >= num_bits_to_read)
              {
                ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
                ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_len_in_bits);
                /* Skip the IE */
                ADJUST_OFFSET_AND_BUF_LEN(ie_length*8, pos, buffer_length, esm_len_in_bits);
              }
              else
              {
                MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
                *buffer_length = 0;
                return TRUE;
              }
            }  
            t3396_decoded = TRUE;
            break;
          }
#endif
        default:
          MSG_ERROR_DS_1(MM_SUB, "NAS_MSG_LIB: Unknown IEI value %d", opt_val_iei);
          *buffer_length = 0;
          return TRUE;
      } /* END switch(opt_val_iei) */
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB, "Insufficient buffer length %d", *buffer_length);
      *buffer_length = 0;
      return TRUE;
    }
  }/* END while(*esm_len_in_bits) */
  return TRUE; 
}

/*===========================================================================

FUNCTION  DECODE_ESM_STATUS

DESCRIPTION
  This function decodes ESM STATUS message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_esm_status
(
   byte                                    *src,
   word                                    *pos, 
   word                                    *buffer_length,   
   lte_nas_esm_status                      *esm_status,
   word                                    *esm_len_in_bits
)
{
  byte    num_bits_to_read =0, unpackedbytes =0;

  if(*buffer_length < (NAS_ESM_ESM_STATUS - MIN_ESM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode ESM STATUS",
                   *buffer_length);
    *buffer_length = 0;
    *esm_len_in_bits  = 0;
    return FALSE;
  }
  
  /*--------------------------------------------------
                Check for ESM Cause
  --------------------------------------------------*/    
  num_bits_to_read = 8;           
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
 
  if(populate_valid_esm_cause(&esm_status->esm_cause,
                              unpackedbytes)  == FALSE)
  {                        
    MSG_ERROR_DS_1(MM_SUB, "NAS_MSG_LIB: Unknown ESM cause %d", unpackedbytes);
    return FALSE;
  }
  ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);

  if(*buffer_length > 0)
  {
    MSG_ERROR_DS_1(MM_SUB, "NAS_MSG_LIB: ESM STATUS is %d bits longer than expected",
                   *buffer_length);
  }
  return TRUE;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================

FUNCTION  DECODE_ESM_MSG

DESCRIPTION
  This function decodes ESM messages. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_esm_msg
(
  byte *src,
  word *pos,
  word *buffer_length,
  word esm_len, 
  lte_nas_incoming_msg_type  *nas_incoming_msg_ptr
)
{ 
  byte num_bits_to_read,pd,bearer_id,pt_id, message_id; 
  log_lte_nas_ota_in_msg_event_type event;
  boolean valid_ie = FALSE;
  word esm_len_in_bits = 0;
  lte_nas_esm_incoming_msg_type *msg_type;
  boolean esm_msg_failed = FALSE;
  byte first_three_esm_bytes = 3; /*PD + Bearer ID (1) + PTI (1) + MSG_TYPE (1)*/


  emm_ctrl_data_type        *emm_ctrl_data_ptr;


  esm_length_check_failed = FALSE;

  esm_len_in_bits = esm_len * WORD_SIZE;
  
  if(*buffer_length < MIN_ESM_LEN || esm_len_in_bits < MIN_ESM_LEN )
  {
    MSG_ERROR_DS_3(MM_SUB, "NAS_MSG_LIB: Either Buffer length = %d or ESM length = %d less than minimum ESM length = %d.",
                   *buffer_length, esm_len_in_bits, MIN_ESM_LEN);
    return FALSE;
  }

  while(esm_len_in_bits)
  {
    msg_type = &nas_incoming_msg_ptr->esm_incoming_msg;
    /*------------------------------------------------------------
               BEARER ID (reading the MSB 4 bits)
    ------------------------------------------------------------*/
    num_bits_to_read = 4;     
    bearer_id = b_unpackb(src, *pos, num_bits_to_read);
    MSG_MED_DS_1(MM_SUB, "EPS Bearer ID = %d\n", bearer_id);
    ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,&esm_len_in_bits);
    /*------------------------------------------------------------
     ESM PROTOCOL DISCRIMINATOR (reading the LSB 4 bits)
    ------------------------------------------------------------*/
    num_bits_to_read = 4;     
    pd = b_unpackb(src, *pos, num_bits_to_read);      
    /*Check for validation*/
    if(pd!= (byte)EPS_SESSION_MANAGEMENT_MESSAGES)
    {
      MSG_ERROR_DS_0(MM_SUB, "Received Protocol Discriminator other than ESM\n");
      SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits);
      return FALSE;
    }   
    msg_type->nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
    ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,&esm_len_in_bits);
    /*--------------------------------------------------
                Protocol Transaction ID
    -------------------------------------------------*/ 
    num_bits_to_read  = 8;
    pt_id= b_unpackb(src, *pos, num_bits_to_read);
    ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,&esm_len_in_bits);
    /*------------------------------------------------------------
                   ESM MESSAGE ID
    ------------------------------------------------------------*/
    num_bits_to_read = 8;     
    message_id = b_unpackb(src, *pos, num_bits_to_read);    

    ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,&esm_len_in_bits);    
    /* Write these parameters into the corresponding structures*/
    msg_type->nas_hdr.msg_id = (lte_nas_message_id)message_id;
    /*Populate OTA message info*/
    msg_type->nas_hdr.ota_msg_ptr = src;
    msg_type->nas_hdr.ota_msg_len = (*buffer_length + (first_three_esm_bytes)*8)/8;
    event.in_msg = message_id;
    event_report_payload(EVENT_LTE_ESM_OTA_INCOMING_MSG,sizeof(event), (void*)&event);
    switch(message_id)
    {
      case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
        msg_type->default_bearer_context_req.lte_nas_hdr.pd  = (lte_nas_protocol_discriminator)pd;
        msg_type->default_bearer_context_req.lte_nas_hdr.msg_id  = (lte_nas_message_id)message_id;
        msg_type->default_bearer_context_req.lte_nas_esm_hdr.bearer_id = bearer_id;          
        msg_type->default_bearer_context_req.lte_nas_esm_hdr.pti = pt_id; 
        valid_ie = decode_act_def_context_req(src,pos,buffer_length,&msg_type->default_bearer_context_req, &esm_len_in_bits);
        if(valid_ie  == FALSE)
        {
          SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits);
          MSG_HIGH_DS_0(MM_SUB, "Default Bearer context request decoding unsuccessfull\n");
          return FALSE;
        }                           
        total_num_messages++;
        break;
      case PDN_CONNECTIVITY_REJECT:
        msg_type->pdn_connectivity_rej.lte_nas_hdr.pd  = (lte_nas_protocol_discriminator)pd;
        msg_type->pdn_connectivity_rej.lte_nas_hdr.msg_id  = (lte_nas_message_id)message_id;
        msg_type->pdn_connectivity_rej.lte_nas_esm_hdr.bearer_id = bearer_id;          
        msg_type->pdn_connectivity_rej.lte_nas_esm_hdr.pti = pt_id; 
        /*If PDN_CONNECTIVITY_REJ has different PTI than the one allocated in PDN_CON_REQ, 
          then stop decoding PDN_CON_REJ message*/
        emm_ctrl_data_ptr = emm_db_get_ctrl_data();
		ASSERT(emm_ctrl_data_ptr != NULL);
        if ((emm_ctrl_data_ptr->emm_state == EMM_REGISTERED_INITIATED) &&
            (emm_ctrl_data_ptr->emm_substate == EMM_WAITING_FOR_NW_RESPONSE) &&
            (emm_ctrl_data_ptr != NULL) && (emm_ctrl_data_ptr->pdn_conn_req_ptr != NULL) &&
            (pt_id != emm_ctrl_data_ptr->pdn_conn_req_ptr->lte_nas_esm_hdr.pti))
        {
          SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits);
          MSG_HIGH_DS_0(MM_SUB, "PDN Connectivity Reject decoding unsuccessfull due to PTI mismatch during Attach, stop decoding\n");
          return FALSE; 
        }
        valid_ie = decode_pdn_conn_rej(src,pos,buffer_length,&msg_type->pdn_connectivity_rej,&esm_len_in_bits);
        if(valid_ie == FALSE)
        { 
          SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits);
          MSG_HIGH_DS_0(MM_SUB, "PDN Connectivity Reject decoding unsuccessfull\n");
          return FALSE; 
        }
        total_num_messages ++; 
        break;
      case ACTIVATE_DEDICATED_EPS_BEARER_CONTEXT_REQUEST:
        msg_type->dedicated_bearer_context_req.lte_nas_hdr.pd  = (lte_nas_protocol_discriminator)pd;
        msg_type->dedicated_bearer_context_req.lte_nas_hdr.msg_id  = (lte_nas_message_id)message_id;
        msg_type->dedicated_bearer_context_req.lte_nas_esm_hdr.bearer_id = bearer_id;          
        msg_type->dedicated_bearer_context_req.lte_nas_esm_hdr.pti = pt_id; 
        valid_ie = decode_dedicated_eps_bearer_req(src,pos,buffer_length,&msg_type->dedicated_bearer_context_req,&esm_len_in_bits);
        if(valid_ie == FALSE)
        { 
          SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits); 
          MSG_HIGH_DS_0(MM_SUB, "DEDICATED EPS BEARER REQUEST decoding unsuccessfull\n");
          return FALSE; 
        }
        total_num_messages ++; 
        break;
      case DEACTIVATE_EPS_BEARER_CONTEXT_REQUEST:
        msg_type->deactivate_eps_bearer_context_req.lte_nas_hdr.pd  = (lte_nas_protocol_discriminator)pd;
        msg_type->deactivate_eps_bearer_context_req.lte_nas_hdr.msg_id  = (lte_nas_message_id)message_id;
        msg_type->deactivate_eps_bearer_context_req.lte_nas_esm_hdr.bearer_id = bearer_id;          
        msg_type->deactivate_eps_bearer_context_req.lte_nas_esm_hdr.pti = pt_id;       
        valid_ie = decode_deactivate_eps_bearer_req(src,pos,buffer_length,&msg_type->deactivate_eps_bearer_context_req,&esm_len_in_bits);      
        if(valid_ie == FALSE)
        { 
          SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits); 
          MSG_HIGH_DS_0(MM_SUB, "DEACTIVATE EPS BEARER REQUEST decoding unsuccessfull\n");
          return FALSE; 
        }
        total_num_messages ++; 
        MSG_HIGH_DS_0(MM_SUB, "DEACTIVATE EPS BEARER REQUEST is decoded successfully\n");
        break;
      case BEARER_RESOURCE_ALLOCATION_REJECT:
        msg_type->bearer_resource_alloc_rej.lte_nas_hdr.pd  = (lte_nas_protocol_discriminator)pd;
        msg_type->bearer_resource_alloc_rej.lte_nas_hdr.msg_id  = (lte_nas_message_id)message_id;
        msg_type->bearer_resource_alloc_rej.lte_nas_esm_hdr.bearer_id = bearer_id;          
        msg_type->bearer_resource_alloc_rej.lte_nas_esm_hdr.pti = pt_id;      
        valid_ie = decode_bearer_resource_alloc_rej(src,pos,buffer_length,&msg_type->bearer_resource_alloc_rej,&esm_len_in_bits);      
        if(valid_ie == FALSE)
        { 
          SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits); 
          MSG_HIGH_DS_0(MM_SUB, "BEARER RESOURCE ALLOCATION REJECT decoding unsuccessfull\n");
          return FALSE; 
        }
        total_num_messages ++; 
        break;
      case BEARER_RESOURCE_MODIFICATION_REJECT:
        msg_type->bearer_resource_modifi_rej.lte_nas_hdr.pd  = (lte_nas_protocol_discriminator)pd;
        msg_type->bearer_resource_modifi_rej.lte_nas_hdr.msg_id  = (lte_nas_message_id)message_id;
        msg_type->bearer_resource_modifi_rej.lte_nas_esm_hdr.bearer_id = bearer_id;          
        msg_type->bearer_resource_modifi_rej.lte_nas_esm_hdr.pti = pt_id;       
        valid_ie = decode_bearer_modifi_context_rej(src,pos,buffer_length,&msg_type->bearer_resource_modifi_rej,&esm_len_in_bits);      
        if(valid_ie == FALSE)
        { 
          SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits); 
          MSG_HIGH_DS_0(MM_SUB, "BEARER MODIFICATION REJECT decoding unsuccessfull\n");
          return FALSE; 
        }
        total_num_messages ++; 
        break;
      case MODIFY_EPS_BEARER_CONTEXT_REQUEST:
        msg_type->modify_eps_bearer_ctxt_req.lte_nas_hdr.pd  = (lte_nas_protocol_discriminator)pd;
        msg_type->modify_eps_bearer_ctxt_req.lte_nas_hdr.msg_id  = (lte_nas_message_id)message_id;
        msg_type->modify_eps_bearer_ctxt_req.lte_nas_esm_hdr.bearer_id = bearer_id;          
        msg_type->modify_eps_bearer_ctxt_req.lte_nas_esm_hdr.pti = pt_id;       
        valid_ie = decode_modifi_eps_context_req(src,pos,buffer_length,&msg_type->modify_eps_bearer_ctxt_req,&esm_len_in_bits);      
        if(valid_ie == FALSE)
        { 
          SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits); 
          MSG_HIGH_DS_0(MM_SUB, "MODIFY EPS BEARER CONTEXT REQUEST decoding unsuccessfull\n");
          buffer_length = 0;
          return FALSE; 
        }
        total_num_messages ++;
        break;
      case PDN_DISCONNECT_REJECT: 
        msg_type->pdn_disconnect_rej.lte_nas_hdr.pd  = (lte_nas_protocol_discriminator)pd;
        msg_type->pdn_disconnect_rej.lte_nas_hdr.msg_id  = (lte_nas_message_id)message_id;
        msg_type->pdn_disconnect_rej.lte_nas_esm_hdr.bearer_id = bearer_id;          
        msg_type->pdn_disconnect_rej.lte_nas_esm_hdr.pti = pt_id;       
        valid_ie = decode_pdn_disconnect_rej(src,pos,buffer_length,&msg_type->pdn_disconnect_rej,&esm_len_in_bits);      
        if(valid_ie == FALSE)
        { 
          SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits); 
          MSG_HIGH_DS_0(MM_SUB, "PDN DISCONNECT REJECT decoding unsuccessfull\n");
          return FALSE; 
        }
        total_num_messages ++;
        break;
      case ESM_INFORMATION_REQUEST:
        msg_type->esm_info_req.lte_nas_hdr.pd  = (lte_nas_protocol_discriminator)pd;
        msg_type->esm_info_req.lte_nas_hdr.msg_id  = (lte_nas_message_id)message_id;
        msg_type->esm_info_req.lte_nas_esm_hdr.bearer_id = bearer_id;          
        msg_type->esm_info_req.lte_nas_esm_hdr.pti = pt_id;       
        total_num_messages ++;
        break;
#ifdef FEATURE_LTE_REL9 
      case ESM_NOTIFICATION: 
        emm_ctrl_data_ptr = emm_db_get_ctrl_data();
		ASSERT(emm_ctrl_data_ptr != NULL);
        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
        {
          msg_type->esm_notification.lte_nas_hdr.pd  = (lte_nas_protocol_discriminator)pd;
          msg_type->esm_notification.lte_nas_hdr.msg_id  = (lte_nas_message_id)message_id;
          msg_type->esm_notification.lte_nas_esm_hdr.bearer_id = bearer_id;          
          msg_type->esm_notification.lte_nas_esm_hdr.pti = pt_id;   
          valid_ie = decode_esm_notification(src,pos,buffer_length,&msg_type->esm_notification,&esm_len_in_bits);      
          if(valid_ie == FALSE)
          { 
            SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits); 
            MSG_HIGH_DS_0(MM_SUB, "ESM_NOTIFICATION decoding unsuccessfull\n");
            return FALSE; 
          }
          total_num_messages ++;
        }
        break;
#endif
      case ESM_STATUS:
        msg_type->mt_esm_status.lte_nas_hdr.pd  = 
                                            (lte_nas_protocol_discriminator)pd;
        msg_type->mt_esm_status.lte_nas_hdr.msg_id  = ESM_STATUS;
        msg_type->mt_esm_status.lte_nas_esm_hdr.bearer_id = bearer_id;          
        msg_type->mt_esm_status.lte_nas_esm_hdr.pti = pt_id; 
        valid_ie = decode_esm_status(src,pos, 
                                     buffer_length,
                                     &msg_type->mt_esm_status,
                                     &esm_len_in_bits);      
        if(valid_ie == FALSE)
        { 
          SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits); 
          MSG_HIGH_DS_0(MM_SUB, "ESM STATUS decoding unsuccessfull\n");
          return FALSE; 
        }
        total_num_messages ++; 
        break;
      default:
        MSG_ERROR_DS_0(MM_SUB, "LTE_NAS_MSG_LIB: Unknown ESM msg");
        msg_type->nas_hdr.pd = (lte_nas_protocol_discriminator)pd;
        msg_type->nas_hdr.msg_id = (lte_nas_message_id)message_id;
        msg_type->nas_hdr.valid = FALSE;
        msg_type->nas_hdr.esm_decode_fail_reason = 
                               (esm_cause_enum_T)LTE_NAS_NON_EXISTENT_MSG_TYPE;
        msg_type->esm_unknown_msg.lte_nas_esm_hdr.bearer_id = bearer_id;
        msg_type->esm_unknown_msg.lte_nas_esm_hdr.pti = pt_id;
        SKIP_DECODING_ESM(pos,buffer_length,esm_len_in_bits);
        esm_msg_failed = TRUE;
        total_num_messages ++; 
        break;
    }/*End of switch(message_id)*/ 
    /*Discard the extra bytes present in the OTA buffer*/
    esm_len_in_bits = 0;
  }/*End of WHILE(esm_len)*/  
#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */
  if(esm_msg_failed)
  {
    return FALSE;     
  }
  return TRUE;
}/*End of decode_esm_container*/

#endif /*FEATURE_LTE*/
