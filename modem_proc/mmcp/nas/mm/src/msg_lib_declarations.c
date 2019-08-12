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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_declarations.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when           who                          what                              
--------     --------  ------------------------------------------------
05/17/09      hnam    Added the code to support security
==============================================================================*/

/*==============================================================================
                         HEADER 
==============================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#ifdef FEATURE_LTE
#include "lte_nas_emm_message_type.h"
#include "lte_nas_esm_message_type.h"
#include "msg_lib_declarations.h"
#include "lte_nas.h"
#include "err.h"
#include "lte_nas_security_info.h"
#include "emm_security.h"

/*==============================================================================
                      DATA DECLARATIONS
==============================================================================*/

byte total_num_messages;
byte nas_emm_index;
byte nas_esm_index;
//lte_nas_dl_security_info_type *nas_dl_security_info;
#define MAX_BCD_VALUE 9

#ifdef FEATURE_LTE_NAS_DEEP_SEC

#define MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN 417
#define MAX_ATTACH_ACCEPT_LEN               MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN + 237 /*ESM + EMM*/
#define MAX_TRACKING_AREA_UPDATE_ACCEPT_LEN 243
#define MAX_ATTACH_REJECT_LEN               3 + 257 /*PDN_CONN_REJ = 257*/
#define MAX_SECURITY_MODE_COMMAND_LEN       21
#define MAX_EMM_AUTHENTICATION_REQUEST_LEN  36
#define MAX_GUTI_REALLOCATION_COMMAND_LEN   112
#define MAX_EMM_INFORMATION_LEN             15 +  251 /*NW_NAME*/ + 251 /*SHORT_NAME*/ 
#define MAX_DOWNLINK_NAS_TRANSPORT_LEN      254
#define MAX_CS_SERV_NOTIFICATION_LEN        278

#define MAX_MODIFY_EPS_BEARER_CONTEXT_REQUEST_LEN 538


/*===========================================================================

FUNCTION  MSG_LIB_VALIDATE_UPDATE_INCOMING_NAS_MSG_LENGTH

DESCRIPTION
  This function checks the incoming NAS message length, if the length 
  is greater then it is truncated to the MAX length for that OTA message

DEPENDENCIES
  None

RETURN VALUE
  WORD: Updated length  (in bits) of the incoming NAS message

SIDE EFFECTS
  None

===========================================================================*/
word msg_lib_validate_update_incoming_nas_msg_length
( 
  word                            buffer_length,
  lte_nas_protocol_discriminator  pd,
  lte_nas_message_id              message_id
)
{
  switch(pd)
  {
    case EPS_MOBILITY_MANAGEMENT_MESSAGES:
      switch (message_id)
      {
      case ATTACH_ACCEPT:
        if(buffer_length > MAX_ATTACH_ACCEPT_LEN)
        {
          MSG_ERROR_DS_2(MM_SUB, "ATTACH_ACCEPT length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                         buffer_length,
                         MAX_ATTACH_ACCEPT_LEN);
          buffer_length = MAX_ATTACH_ACCEPT_LEN;
        }
        break ;
      case TRACKING_AREA_UPDATE_ACCEPT:
        if(buffer_length > MAX_TRACKING_AREA_UPDATE_ACCEPT_LEN)
        {
          MSG_ERROR_DS_2(MM_SUB, "TRACKING_AREA_UPDATE_ACCEPT length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                         buffer_length,
                         MAX_TRACKING_AREA_UPDATE_ACCEPT_LEN);
          buffer_length = MAX_TRACKING_AREA_UPDATE_ACCEPT_LEN;
        }
        break ; 
      case ATTACH_REJECT:  
        if(buffer_length > MAX_ATTACH_REJECT_LEN)
        {
          MSG_ERROR_DS_2(MM_SUB, "ATTACH_REJECT length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                         buffer_length,
                         MAX_ATTACH_REJECT_LEN);
          buffer_length = MAX_ATTACH_REJECT_LEN;
        }
        break ;
      case SECURITY_MODE_COMMAND:
        if(buffer_length > MAX_SECURITY_MODE_COMMAND_LEN)
        {
          MSG_ERROR_DS_2(MM_SUB, "SECURITY_MODE_COMMAND length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                         buffer_length,
                         MAX_SECURITY_MODE_COMMAND_LEN);
          buffer_length = MAX_SECURITY_MODE_COMMAND_LEN;
        }
        break;
      case EMM_AUTHENTICATION_REQUEST:
        if(buffer_length > MAX_EMM_AUTHENTICATION_REQUEST_LEN)
        {
          MSG_ERROR_DS_2(MM_SUB, "ACT_DEFAULT_BEARER_CTXT_REQ length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                         buffer_length,
                         MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN);
          buffer_length = MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN;
        }
        break;
      case GUTI_REALLOCATION_COMMAND:
        if(buffer_length > MAX_GUTI_REALLOCATION_COMMAND_LEN)
        {
          MSG_ERROR_DS_2(MM_SUB, "GUTI_REALLOCATION_COMMAND length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                         buffer_length,
                         MAX_GUTI_REALLOCATION_COMMAND_LEN);
          buffer_length = MAX_GUTI_REALLOCATION_COMMAND_LEN;
        }
        break;
      case EMM_INFORMATION:
        if(buffer_length > MAX_EMM_INFORMATION_LEN)
        {
          MSG_ERROR_DS_2(MM_SUB, "EMM_INFORMATION length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                         buffer_length,
                         MAX_EMM_INFORMATION_LEN);
          buffer_length = MAX_EMM_INFORMATION_LEN;
        }
        break;
      case DOWNLINK_NAS_TRANSPORT:
        if(buffer_length > MAX_DOWNLINK_NAS_TRANSPORT_LEN)
        {
          MSG_ERROR_DS_2(MM_SUB, "DOWNLINK_NAS_TRANSPORT length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                         buffer_length,
                         MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN);
          buffer_length = MAX_DOWNLINK_NAS_TRANSPORT_LEN;
        }
        break;
      case CS_SERV_NOTIFICATION:
        if(buffer_length > MAX_CS_SERV_NOTIFICATION_LEN)
        {
          MSG_ERROR_DS_2(MM_SUB, "CS_SERV_NOTIFICATION length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                         buffer_length,
                         MAX_CS_SERV_NOTIFICATION_LEN);
          buffer_length = MAX_CS_SERV_NOTIFICATION_LEN;
        }
        break;
      default:
        break;
      }
      break;
   
    case EPS_SESSION_MANAGEMENT_MESSAGES:  
      switch (message_id)
      {
      case ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST:
        if(buffer_length > MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN)
        {
          MSG_ERROR_DS_2(MM_SUB, "ACT_DEFAULT_BEARER_CTXT_REQ length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                         buffer_length,
                         MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN);
          buffer_length = MAX_ACT_DEFAULT_BEARER_CTXT_REQ_LEN;
        }
        break ;
      case MODIFY_EPS_BEARER_CONTEXT_REQUEST:
        if(buffer_length > MAX_MODIFY_EPS_BEARER_CONTEXT_REQUEST_LEN)
        {
          MSG_ERROR_DS_2(MM_SUB, "MODIFY_EPS_BEARER_CONTEXT_REQUEST length is %d MAX_LENGTH is %d, truncating it to the MAX length",
                         buffer_length,
                         MAX_MODIFY_EPS_BEARER_CONTEXT_REQUEST_LEN);
          buffer_length = MAX_MODIFY_EPS_BEARER_CONTEXT_REQUEST_LEN;
        }
        break ;
      default:
        break;
      }
      break;
    default:
      break;
  }
  MSG_HIGH_DS_1(MM_SUB, "Message_length is %d", buffer_length);
  /*Length was received in bytes, it needs to be returned in bits, so convert here*/
  /*Length was incremented and given to this function, 
   so subtract that length and give back the original length to the calling function...*/
  return (buffer_length - 1 )*8; 
}
#endif /*FEATURE_LTE_NAS_DEEP_SEC*/

void report_emm_success
(
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
)
{
  nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.valid = TRUE;
  nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.emm_decode_fail_reason = LTE_NAS_NO_FAILURE;
}

void report_emm_failure
(
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
)
{
  nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.valid = FALSE;
  nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.emm_decode_fail_reason = LTE_NAS_INVALID_MANDATORY_INFO;
}

void unknown_emm_msg_received
(
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
)
{
  nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.valid = FALSE;
  nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.emm_decode_fail_reason = LTE_NAS_NON_EXISTENT_MSG_TYPE;
}

void report_esm_failure
(
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
)
{
  nas_incoming_msg_ptr[nas_esm_index].esm_incoming_msg.nas_hdr.valid = FALSE; 
  nas_incoming_msg_ptr[nas_esm_index].esm_incoming_msg.nas_hdr.esm_decode_fail_reason = NAS_ESM_INVALID_MANDATORY_INFORMATION;
}

void report_esm_success
(
  lte_nas_incoming_msg_type      *nas_incoming_msg_ptr
)
{
  nas_incoming_msg_ptr[nas_esm_index].esm_incoming_msg.nas_hdr.valid = TRUE; 
  nas_incoming_msg_ptr[nas_esm_index].esm_incoming_msg.nas_hdr.esm_decode_fail_reason = NAS_ESM_NO_FAILURE;
}

boolean is_bcd_valid
(
  byte bcd_value
)
{
  if((((bcd_value)& 0x0F) > MAX_BCD_VALUE)||(((bcd_value)& 0xF0) > MAX_BCD_VALUE))
  {
    MSG_ERROR_DS_1(MM_SUB, "MCC/MNC value is %d, it ahould always be less than 9", bcd_value);
    return FALSE;
  }
  return TRUE;  
}

dword calc_gprs_timer
(
  byte  gprs_timer_octet
)
{
  byte    timer_value_unit = 0;
  dword   timer_value = 0;

  /* Bits 6 to 8 defines the timer value unit for the GPRS timer as follows:
     Bits
     8 7 6
     0 0 0 value is incremented in multiples of 2 seconds
     0 0 1 value is incremented in multiples of 1 minute
     0 1 0 value is incremented in multiples of decihours
     1 1 1 value indicates that the timer is deactivated.   */
  timer_value_unit = gprs_timer_octet >> 5;

  /* Bits 5 to 1 represent the binary coded timer value. */
  timer_value = (dword)(gprs_timer_octet & 0x1F);

  /* Convert timer value to ms */
  switch(timer_value_unit)
  {
    case 0:
      timer_value = ( timer_value * 2) * 1000;
      break;
    case 1:
      timer_value = ( timer_value * 60) * 1000;
      break;
    case 2:
      timer_value = ( timer_value * 6 * 60) * 1000;
      break;
    case 7:
      timer_value = 0;
      break;
    default:
      timer_value = ( timer_value * 60) * 1000;
      break;
  }

  return timer_value;
} /* calc_gprs_timer */

/*===========================================================================

FUNCTION    : REPORT_INVALID_GUTI_OR_NAS_SECURITY

DESCRIPTION:  In order for INTER-RAT to happen, NAS_SECURITY & GUTI must be valid. 
This function reports if NAS_SECURITY OR GUTI are not in sync 
(meaning valid GUTI & invalid SECURITY or vice-versa)
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
boolean report_invalid_guti_or_nas_security
(
  boolean                      guti_present,
  lte_nas_incoming_msg_type   *nas_incoming_msg_ptr,
  emm_ctrl_data_type          *emm_ctrl_data_ptr
)
{
  boolean valid_guti_sec_ctxt = TRUE;

  if(emm_ctrl_data_ptr->avoid_guti_nas_security_check == TRUE)
  {
    MSG_HIGH_DS_0(MM_SUB, "EFS flag is present to avoid GUTI & NAS SECURITY validation check, shall skip this step");
    return valid_guti_sec_ctxt;
  }  

  if(guti_present == TRUE)
  {
    if(NULL == emm_get_context_by_state(emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT))
    {
      MSG_ERROR_DS_0(MM_SUB, "Received GUTI is valid but CURRENT_SECURITY context is invalid, shall send EMM status message");
      valid_guti_sec_ctxt = FALSE;
    }
  }
  else
  {
    if(emm_ctrl_data_ptr->emm_guti_valid == FALSE)
    {
      MSG_ERROR_DS_0(MM_SUB, "Neither stored nor received GUTI are valid, shall send EMM status message");
      valid_guti_sec_ctxt = FALSE;
    }
    else
    {
      if(NULL == emm_get_context_by_state(emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT))
      {
        MSG_ERROR_DS_0(MM_SUB, "Stored GUTI is valid but CURRENT_SECURITY context is invalid, shall send EMM status message");
        valid_guti_sec_ctxt = FALSE;
      }
    }
  }
  if(valid_guti_sec_ctxt == FALSE)
  {
    nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.valid = FALSE;
    nas_incoming_msg_ptr[nas_emm_index].emm_incoming_msg.hdr.emm_decode_fail_reason = 
                                                   LTE_NAS_MSG_INCOMPATIBLE_WITH_PROTOCOL_STATE;         
  }

  return valid_guti_sec_ctxt;
}
#endif /*FEATURE_LTE*/
