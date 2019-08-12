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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_decode_emm.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when         who                          what                              
--------     --------  ------------------------------------------------
08/01/11      Rajesh  T3442 timer related code changes
12/13/10       abhi      Deleted lte_nas_emm_add_update_result_type; used sys_lte_cs_capability_e_type
05/14/10      RI      Mainlined spec changes upto Dec 09.
01/22/10      hnam    Updated the KSI handling.
01/21/10      zren    Added EMM/ESM STATUS feature
10/05/09      hnam     spare bit parsed for security context
09/02/09      hnam     Featurized the TAU accept (based on MAR <->DEC spec)
07/30/09      hnam     Corrected EMM cause validation in MT detach
07/23/09      hnam     Called send_nas_ota_msg_log_packet() after decoding 
                       the length of ESM container
07/14/09      hnam     Moved calc_gprs_timer() to "msg_lib_declarations.c"
07/07/09      hnam     Fixed the decoding of TAI in TAU ACCEPT
06/22/09      hnam     Corrected security mode command parsing & EMM CAUSE parsing
06/16/09      hnam     Corrected the parsing in "decode_security_mode_cmd"
06/04/09      zren     Added code to parse GPRS timer
05/17/09      hnam     Added the code to support security
04/13/09      hnam     Added authentication request code.
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "bit.h"
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "msg_lib_decode_emm.h"
#include "msg_lib_decode_esm.h"
#include "msg_lib_decode_common_emm_ie.h"
#include "lte_nas_emm_message_type.h"
#include "lte_nas_emm_message_description.h"
#include "msg_lib_declarations.h"
#include "emm_gps_if_msg.h"
#include "msg.h"
#include "err.h"
#include "lte_nas_log.h"
#include "naslog_v.h"
#include "lte_log_codes.h"
#include "amssassert.h"
#include "mm_v.h"
#include "emm_cnm_handler.h"
#ifdef FEATURE_LTE_REL10
#include "gmm_int_v.h"
#endif
#include "stringl.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define T3402_VAL_ATTACH_REJECT_IEI 0x16
#define T3402_VAL_IEI 0x17
#define GUTI_IEI 0x50
#define MS_IDENTITY_IEI 0x23
#define LOC_AREA_ID_IEI 0x13
#define EMM_CAUSE_IEI 0x53
#define T3423_VAL_IEI 0x59
#define EQUIVALENT_PLMN_IEI 0X4A
#define ESM_IEI 0x78
#define T3442_VAL_IEI 0x5B

#define IMEISV_IEI 0xC
#define NONCE_UE_IEI 0x55
#define NONCE_MME_IEI 0x56
#define TAI_LIST_IEI  0x54
#define EPS_BEARER_CONTEXT_STATUS 0x57
#define EMERGENCY_NUM_LIST_IEI 0x34

#define NAS_EMM_ATTACH_REJECT_MIN_LEN 3*WORD_SIZE
#define NAS_EMM_ATTACH_ACCEPT_MIN_LEN 13*WORD_SIZE
#define NAS_EMM_MT_DETACH_REQ_MIN_LEN 3*WORD_SIZE
#define NAS_EMM_SECURITY_MODE_CMD_MIN_LEN 7*WORD_SIZE
#define NAS_EMM_SERV_REJ_MIN_LEN 3*WORD_SIZE
#define NAS_EMM_TAU_ACCEPT_MIN_LEN 3*WORD_SIZE

#define NAS_MSG_CONTAINER_MIN_LEN  3 * WORD_SIZE
#define NAS_MSG_CONTAINER_MAX_LEN  252 * WORD_SIZE

#define MIN_ESM_CONTAINER_LEN 3*WORD_SIZE /*This includes ESM IEI (1) + ESM Length (2) */
#define NAS_EMM_TAU_REJECT_MIN_LEN 3*WORD_SIZE

#define T3442_LEN 8
#define AUTH_REQ_LEN 34*WORD_SIZE
#define NAS_KEYSET_ID_UNAVAIALBLE 15
#define NAS_EMM_IDENTITY_REQUEST_LEN 3*WORD_SIZE
#define NAS_EMM_GUTI_REALLOC_CMD_LEN 14*WORD_SIZE
#define NAS_EMM_STATUS_LEN  3 * WORD_SIZE

#define NAS_EMM_INFO_LEN 2*WORD_SIZE

#ifdef FEATURE_LTE_REL9
#define NAS_EMM_DL_GENERIC_NAS_TRANSPORT_LEN 6*WORD_SIZE
#endif

#define NW_FULL_NAME_IEI 0x43
#define NW_PARTIAL_NAME_IEI 0x45
#define LOCAL_TIME_ZONE_IEI 0x46
#define UNIV_LOCAL_TIME_ZONE_IEI 0x47
#define NW_DAYLIGHT_SAVINGS_TIME_IEI 0x49
#define EPS_NW_FEATURE_SUPPORT_IEI 0x64
#define ADDITIONAL_UPDATE_RESULT_IEI 0xF0
#define T3412_TIMER_IEI 0x5A
#define NAS_KSI_IEI 0X80
#define NAS_EMM_CS_SERV_NOTIFICATION_LEN 0x03*WORD_SIZE

#define MAX_T3402_VALUE_LEN    1

#ifdef FEATURE_LTE_REL10
#define T3412_EXT_VAL_IEI   0x5E
#define T3346_VAL_IEI       0x5F 
#define T3346_LEN           0x1
#endif 

#define CALLING_LINE_INFO_IEI 0x60

#define SUPPLIMENTARY_SERV_CODE_IEI 0x61
      
#define LCS_IND_IEI 0x62

#define MAX_BCD_VAL 9

#define LOCATION_AREA_IDENTITY_LEN 5

byte lte_nas_maci[4];
boolean emm_len_check_failed;
extern boolean is_plmn_bcd_coded(sys_plmn_id_s_type plmn);

/*===========================================================================

                        FUNCTION DEFINITION

===========================================================================*/

#if defined (FEATURE_LTE_REL9) 
/*===========================================================================

FUNCTION  DECODE_DL_GENERIC_NAS_TRANSPORT

DESCRIPTION
  This function decodes DL_GENERIC_NAS_TRANSPORT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_dl_generic_nas_transport
(
  byte                                  *src,
  word                                  *pos, 
  word                                  *buffer_length, 
  lte_nas_emm_dl_generic_nas_transport  *dl_generic_nas_transport
)
{
  byte num_bits_to_read  = 0;
  byte unpacked_bytes = 0;
  word unpacked_word = 0;
  byte min_dl_nas_transport_msg_container_len = 1;
  byte opt_val_iei = 0;

  if(*buffer_length < (NAS_EMM_DL_GENERIC_NAS_TRANSPORT_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "Buffer length = %d is insufficient to decode DL_GENERIC_NAS_TRANSPORT\n", *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /*----------------------------------------
       Generic message container type
  ----------------------------------------*/
  num_bits_to_read = 8;
  unpacked_bytes= (lte_nas_generic_msg_container_type)b_unpackb(src, *pos, num_bits_to_read);
  if((unpacked_bytes < LPP_MSG_CONTAINER) || (unpacked_bytes > LOCATION_SERVICE_MSG_CONTAINER))
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Invalid Generic message container type = %d received. it must be in the range 1 & 2\n", unpacked_bytes);
    *buffer_length = 0;
    return FALSE;    
  }
  dl_generic_nas_transport->generic_msg_container_type = unpacked_bytes;
  *pos = *pos  + num_bits_to_read;
  *buffer_length = *buffer_length-num_bits_to_read; 
  /*----------------------------------------
         Generic message container
  ----------------------------------------*/
  /* Length*/
  num_bits_to_read = 16;  
  unpacked_word = b_unpackw(src, *pos, num_bits_to_read);
  if(unpacked_word < min_dl_nas_transport_msg_container_len)
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Generic message container len is %d invalid", unpacked_word);
    *buffer_length = 0;
    return FALSE;       
  }
  dl_generic_nas_transport->generic_msg_container.container_length = unpacked_word;
  *pos = *pos  + num_bits_to_read;
  *buffer_length = *buffer_length-num_bits_to_read; 
 
  dl_generic_nas_transport->generic_msg_container.container_value_ptr = src + (*pos/8);
  /*Skip the Container Value*/
  num_bits_to_read = 8;
  *pos = *pos  + ((num_bits_to_read/8) * dl_generic_nas_transport->generic_msg_container.container_length)*8;
  *buffer_length = *buffer_length - (((num_bits_to_read/8) * dl_generic_nas_transport->generic_msg_container.container_length)*8); 

  /*-----------------------------------------------------------------
                       Decode Optional fields
 --------------------------------------------------------------------*/
  while(*buffer_length)
  {
    /* ---- IEI ----*/
    num_bits_to_read = 8;
    opt_val_iei = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length-num_bits_to_read;
    switch(opt_val_iei)
      {
      case EMM_ADDITIONAL_INFORMATION_IEI:
      /* Length*/
      num_bits_to_read = 8;  
      unpacked_bytes = b_unpackb(src, *pos, num_bits_to_read);
      dl_generic_nas_transport->additional_info.additional_info_length = unpacked_bytes;
      *pos = *pos  + num_bits_to_read;
      *buffer_length = *buffer_length-num_bits_to_read; 
      /*Set the Additional information pointer*/
      dl_generic_nas_transport->additional_info.additional_info_value_ptr = src + (*pos/8);
      /*Skip the Additional_Information Value*/
      num_bits_to_read = 8;
      *pos = *pos  + ((num_bits_to_read/8) * dl_generic_nas_transport->additional_info.additional_info_length)*8;
      *buffer_length = *buffer_length - (((num_bits_to_read/8) * dl_generic_nas_transport->additional_info.additional_info_length)*8); 
      dl_generic_nas_transport->additional_info_exists =  TRUE;
      break;
    default:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= received unknown IEI");
      *buffer_length = 0;
      break;
    }
  }
  return TRUE;
}
#endif

/*===========================================================================

FUNCTION  DECODE_CS_SERV_NOTIFICATION

DESCRIPTION
  This function decodes CS_SERV_NOTIFICATION message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_cs_serv_notification
(
  byte                              *src,
  word                              *pos, 
  word                              *buffer_length, 
  lte_nas_emm_cs_serv_notification  *cs_serv_notification
)
{
  byte num_bits_to_read  = 0;
  byte opt_val_iei = 0;
  byte opt_iei_len = 0;
  byte index = 0;
  byte unpackedbytes = 0;
  byte min_cli_len = 1;
  byte max_cli_len = 12;

  if(*buffer_length < (NAS_EMM_CS_SERV_NOTIFICATION_LEN - MIN_EMM_LEN))
  {
    *buffer_length = 0;
    return FALSE;
  }

  /*----------------------------------------
             Paging Identity
  ----------------------------------------*/
  /*Skip 7 spare bits*/
  num_bits_to_read = 7;
  *pos = *pos  + num_bits_to_read;
  *buffer_length = *buffer_length-num_bits_to_read;   

  num_bits_to_read = 1;
  cs_serv_notification->paging_id = (lte_nas_paging_identity_type)b_unpackb(src, *pos, num_bits_to_read);
  *pos = *pos  + num_bits_to_read;
  *buffer_length = *buffer_length-num_bits_to_read; 

  /*-----------------------------------------------------------------
                       Decode Optional fields
    --------------------------------------------------------------------*/
  while(*buffer_length)
  {
     /* ---- IEI ----*/
      num_bits_to_read = 8;
      opt_val_iei = b_unpackb(src, *pos, num_bits_to_read);
      *pos = *pos + num_bits_to_read;
      *buffer_length = *buffer_length-num_bits_to_read;
      switch(opt_val_iei)
      {
      case CALLING_LINE_INFO_IEI:
      opt_iei_len = b_unpackb(src, *pos, num_bits_to_read);
      *pos = *pos + num_bits_to_read;
      *buffer_length = *buffer_length-num_bits_to_read;
      
      if( opt_iei_len > max_cli_len ) 
      {
        MSG_ERROR_DS_2(MM_SUB, "Received CLI length = %d bytes. Max allowable length = %d bytes, truncating it to this value.",
                       opt_iei_len, max_cli_len);
        opt_iei_len = max_cli_len;
      }
      else if( opt_iei_len < min_cli_len )
      {
        MSG_ERROR_DS_2(MM_SUB, "CLI length is %d, minimum allowable list length is %d.",
                       opt_iei_len, min_cli_len);
        *buffer_length = 0;
        cs_serv_notification->calling_line_info_exists = FALSE;
        return TRUE;
      }

      if( (opt_iei_len*WORD_SIZE) > *buffer_length )
      {
        MSG_ERROR_DS_2(MM_SUB, "CLI length = %d bits > available buffer size = %d bits.",
                       (opt_iei_len*WORD_SIZE), *buffer_length);
        *buffer_length = 0;
        cs_serv_notification->calling_line_info_exists = FALSE;
        return TRUE;
      }

         /*Spare bit*/  
        num_bits_to_read = 1;
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;

        /*Numbering Plan*/
        num_bits_to_read = 3;
        cs_serv_notification->calling_line_info.numbering_plan_identification = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;

        /*Type of number*/
        num_bits_to_read = 4;
        unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
        if(unpackedbytes > DEDICATED_ACCESS_SHORT_CODE)
        {
          MSG_ERROR_DS_3(MM_SUB, "Type of number = %d is not in the range %d - %d.",
                         unpackedbytes, UNKOWN_NUMBER, DEDICATED_ACCESS_SHORT_CODE);
          *buffer_length = 0;
          return TRUE;
        }
        cs_serv_notification->calling_line_info.number_type = unpackedbytes;
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;
        opt_iei_len --; /*Decrease the OPT_IE length (1 octet is read)*/
        
        if((*buffer_length) &&(opt_iei_len))
        {
          /*1 spare bit*/
          num_bits_to_read = 1;
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length-num_bits_to_read;
         
          /*Presentation Indicator*/
          num_bits_to_read = 2;
          cs_serv_notification->calling_line_info.presentation_ind = b_unpackb(src, *pos, num_bits_to_read);
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length-num_bits_to_read;
        
          /*3 spare bits*/
          num_bits_to_read = 3;
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length-num_bits_to_read;
 
          /*Screening Indicator*/
          num_bits_to_read = 2;
          cs_serv_notification->calling_line_info.screen_ind = b_unpackb(src, *pos, num_bits_to_read);
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length-num_bits_to_read;
          opt_iei_len --; /*Decrease the OPT_IE length (2 octets is read)*/

          /*Identity Digits*/
          num_bits_to_read = 4;         
          while(opt_iei_len)
          {
            if(opt_iei_len == 1)
            {
              /*Skip the MSB - 0XFF*/
              *pos = *pos + num_bits_to_read;
              *buffer_length = *buffer_length-num_bits_to_read;
              /*Read the LSB*/               
              cs_serv_notification->calling_line_info.digits[index] = b_unpackb(src, *pos, num_bits_to_read);
              *pos = *pos + num_bits_to_read;
              *buffer_length = *buffer_length-num_bits_to_read;
            }
            else
            {
              cs_serv_notification->calling_line_info.digits[index + 1] = b_unpackb(src, *pos, num_bits_to_read);              
              *pos = *pos + num_bits_to_read;
              *buffer_length = *buffer_length-num_bits_to_read;
              cs_serv_notification->calling_line_info.digits[index] = b_unpackb(src, *pos, num_bits_to_read);              
              *pos = *pos + num_bits_to_read;
              *buffer_length = *buffer_length-num_bits_to_read;
            }
            index += 2;
            opt_iei_len --;
            cs_serv_notification->calling_line_info.number_of_digits ++;
          }         
        }
      cs_serv_notification->calling_line_info_exists = TRUE;
      break;
      case SUPPLIMENTARY_SERV_CODE_IEI:
          num_bits_to_read = 8;
      unpackedbytes = (lte_nas_supl_serv_code_type)b_unpackb(src, *pos, num_bits_to_read);
      if( (unpackedbytes != CALLING_LINE_IDENTIFICATION_PRESENTATION) &&
          (unpackedbytes != CALLING_LINE_IDENTIFICATION_RESTRICTION) &&
          (unpackedbytes != CONNECTED_LINE_IDENTIFICATION_PRESENTATION) &&
          (unpackedbytes != CONNECTED_LINE_IDENTIFICATION_RESTRICTION) &&
          (unpackedbytes != ALL_NAME_IDENTIFICATION) &&
          (unpackedbytes != CALLING_NAME_PRESENTATION) &&
          (unpackedbytes != ALL_FORWARDING) &&
          (unpackedbytes != CALL_FORWARDING_UNCONDITIONAL) &&
          (unpackedbytes != ALL_CONDITIONAL_FORWARDING) &&
          (unpackedbytes != CALL_FWD_MOBILE_SUBSCRIBER_BUSY) &&
          (unpackedbytes != CALL_FWD_NO_REPLY) &&
          (unpackedbytes != CALL_FWD_MOBILE_SUBSCRIBER_UNREACHABLE) &&
          (unpackedbytes != CALL_DEFLECTION) &&
          (unpackedbytes != EXPLICIT_CALL_TRANSFER) &&
          (unpackedbytes != SS_CALL_WAITING) &&
          (unpackedbytes != CALL_HOLD) &&
          (unpackedbytes != CALL_COMPLETION_TO_BUSY_SUBS_ORIG_SIDE) &&
          (unpackedbytes != CALL_COMPLETION_TO_BUSY_SUBS_DEST_SIDE) &&
          (unpackedbytes != MULTI_CALL) &&
          (unpackedbytes != MULTI_PARTY) &&
          (unpackedbytes != CLOSED_USER_GROUP) &&
          (unpackedbytes != ADVICE_OF_CHARGE_INFORNMATION) &&
          (unpackedbytes != ADVICE_OF_CHARGE_CHARGING) &&
          (unpackedbytes != USER_TO_USER_SIGNALLING_1) &&
          (unpackedbytes != USER_TO_USER_SIGNALLING_2) &&
          (unpackedbytes != USER_TO_USER_SIGNALLING_3) &&
          (unpackedbytes != ALL_BARRING) &&
          (unpackedbytes != BARRING_OF_OUTGOING_CALLS) &&
          (unpackedbytes != BARRING_OF_ALL_OUTGOING_CALLS) &&
          (unpackedbytes != BARRING_OF_ALL_OUTGOING_INTERNATIONAL_CALLS) &&
          (unpackedbytes != BARRING_OF_ALL_OUTGOING_INT_CALLS_EXCEPT_DIRECTED_TO_HPLMN_COUNTRY) &&
          (unpackedbytes != BARRING_OF_INCOMING_CALLS) &&
          (unpackedbytes != BARRING_OF_ALL_INCOMING_CALLS) &&
          (unpackedbytes != BARRING_OF_ALL_INCOMING_CALL_EXCEPT_ROAMING_OUTSIDE_HPLMN_COUNTRY) &&
          (unpackedbytes != ENHANCED_MULTILEVEL_PRESEDENCE_PREEMPTION_SERVICE) &&
          (unpackedbytes != ALL_LCS_PRIVACY_EXCEMPTION) &&
          (unpackedbytes != UNIVERSAL) &&
          (unpackedbytes != CALL_SESSION_RELATED) &&
          (unpackedbytes != CALL_SESSION_UNRELATED) &&
          (unpackedbytes != PLMN_OPERATOR) &&
          (unpackedbytes != SERVICE_TYPE) &&
          (unpackedbytes != ALL_MO_LOCATION_REQUEST_CLASS) &&
          (unpackedbytes != BASIC_SELF_LOCATION) &&
          (unpackedbytes != AUTONOMOUS_SELF_LOCATION) &&
          (unpackedbytes != TRANSFER_TO_THIRD_PARTY) &&
          (unpackedbytes != ALL_PLMN_SPECIFIC_SS) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_1) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_2) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_3) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_4) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_5) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_6) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_7) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_8) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_9) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_A) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_B) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_C) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_D) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_E) &&
          (unpackedbytes != PLMN_SPECIFIC_SS_F))
      {
        MSG_ERROR_DS_1(MM_SUB, "SS Code value = %d is invalid.", unpackedbytes);
             cs_serv_notification->supl_serv_code_exists = FALSE;
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        break;
      }
      cs_serv_notification->supl_serv_code = unpackedbytes; 
      *pos = *pos + num_bits_to_read;
      *buffer_length = *buffer_length-num_bits_to_read;
      cs_serv_notification->supl_serv_code_exists = TRUE;
      break;
      case LCS_IND_IEI:
      num_bits_to_read = 8;
      cs_serv_notification->lcs_ind =  (lte_nas_lcs_ind_type)b_unpackb(src, *pos, num_bits_to_read);
      *pos = *pos + num_bits_to_read;
      *buffer_length = *buffer_length-num_bits_to_read;
      cs_serv_notification->lcs_ind_exists = TRUE;
      break;
    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= unkown IEI received = %d, skipping the message parser",
                     opt_val_iei);
      *buffer_length = 0;
      return TRUE;
      break;
    }
  }
  return TRUE;
}
/*===========================================================================

FUNCTION  DECODE_EMM_INFO

DESCRIPTION
  This function decodes EMM_INFO message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_emm_info
(
  byte                          *src,
  word                          *pos, 
  word                          *buffer_length, 
  lte_nas_emm_info              *emm_info
)
{
  byte num_bits_to_read  = 0;
  boolean opt_val_iei = FALSE;
#ifndef FEATURE_LTE_NAS_DEEP_SEC
  word temp_buf_len = 0;
  word temp_pos = 0;
#endif
  byte opt_iei_len = 0;
  byte day_light_savings = 0;
  /*set iei_exists to FALSE - even if this value is not updated, decoded IEI will not be 
    considered invalid (even though it is decoded successfully)*/
  boolean iei_exists = TRUE;
#ifdef FEATURE_LTE_NAS_DEEP_SEC
  boolean full_name_decoded = FALSE, partial_name_decoded = FALSE;
  boolean local_time_decoded = FALSE,univ_time_decoded = FALSE;
  boolean daylight_savings_decoded = FALSE;
  word original_buffer_len = 0;

  original_buffer_len = *buffer_length;
#endif
 

  /*-------------------------------------------------
              Optional IEIs 
  -------------------------------------------------*/
  while(*buffer_length)
  {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
   if(*buffer_length > original_buffer_len)
   { 
     MSG_ERROR_DS_0(MM_SUB, "******* Buffer over flow is foreseen, will quit parsing further IEIs for safety reasons ******");
     *buffer_length = 0;
     return TRUE;
   }
#endif
    /* ---- IEI ----*/
   num_bits_to_read = 8;
   opt_val_iei = b_unpackb(src, *pos, num_bits_to_read);
   *pos = *pos + num_bits_to_read;
   *buffer_length = *buffer_length-num_bits_to_read;

    /*--------------------------------------------------------
     store the buffer length, offset and IEI length; these 
     values needs to be used while hopping to another optional 
     IE incase the current optional IE is incorrectly received.
     This switch is useful only for 3 cases.
    ---------------------------------------------------------*/

   switch(opt_val_iei)
   {
    case NW_FULL_NAME_IEI:
    case NW_PARTIAL_NAME_IEI: 
#ifndef FEATURE_LTE_NAS_DEEP_SEC
    case NW_DAYLIGHT_SAVINGS_TIME_IEI: 
#else
       if(*buffer_length < 2*num_bits_to_read)
       {
         MSG_ERROR_DS_0(MM_SUB, "Available buffer is insufficient to decode NW_FULL_NAME_IEI/NW_PARTIAL_NAME_IEI\n");
         *buffer_length = 0;
         return TRUE;
       }
#endif
#ifndef FEATURE_LTE_NAS_DEEP_SEC
       temp_buf_len = *buffer_length;
       temp_pos = *pos;
#endif
       opt_iei_len = b_unpackb(src, *pos, num_bits_to_read);
#ifdef FEATURE_LTE_NAS_DEEP_SEC
       if(opt_iei_len*num_bits_to_read >= *buffer_length)
       {
         MSG_ERROR_DS_0(MM_SUB, "Available buffer is insufficient to decode NW_FULL_NAME_IEI/NW_PARTIAL_NAME_IEI\n");
         *buffer_length = 0;
         return TRUE;
       }
#endif
       break;
     default:
      /*No need to remember the buffer length & offset*/
       break;
    }

    switch(opt_val_iei)
    {
      case NW_FULL_NAME_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(full_name_decoded == TRUE)
        {
          *pos += (opt_iei_len + 1)*num_bits_to_read;
          *buffer_length -= (opt_iei_len + 1)*num_bits_to_read;
          break;  
        }    
        full_name_decoded = TRUE;  
#endif
        iei_exists = decode_nw_name(src,pos,buffer_length, &emm_info->nw_full_name);
        if(iei_exists == TRUE)
        {
          /*If the length octet is correctly received, then set the IE existence flag to TRUE*/
          if(!emm_len_check_failed)
          {
            emm_info->nw_full_name_exists = TRUE;
          }
        }
        break;
      case NW_PARTIAL_NAME_IEI: 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(partial_name_decoded == TRUE)
        {
          *pos += (opt_iei_len + 1)*num_bits_to_read;
          *buffer_length -= (opt_iei_len + 1)*num_bits_to_read;
          break;  
        }    
        partial_name_decoded = TRUE;  
#endif
        iei_exists = decode_nw_name(src,pos,buffer_length, &emm_info->nw_partial_name);
        if(iei_exists == TRUE)
        {
          /*If the length octet is correctly received, then set the IE existence flag to TRUE*/
          if(!emm_len_check_failed)
          {
            emm_info->nw_partial_name_exists = TRUE;
          }
        }
        break;
      case LOCAL_TIME_ZONE_IEI: 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
       if(local_time_decoded == TRUE)
       {
        *pos += num_bits_to_read;
        *buffer_length -= num_bits_to_read;
        break;  
       }
       if(*buffer_length < num_bits_to_read)
       {
         MSG_ERROR_DS_0(MM_SUB, "Available buffer is insufficient to decode LOCAL_TIME_ZONE_IEI\n");
         *buffer_length = 0;
         return TRUE;
       }
       local_time_decoded = TRUE;
#endif
       emm_info->local_time_zone = mm_convert_time_zone(b_unpackb(src, *pos, num_bits_to_read));
       *pos = *pos + num_bits_to_read;
       *buffer_length = *buffer_length-num_bits_to_read;
       emm_info->local_time_zone_exists = TRUE;
       iei_exists = TRUE;
       break;
      case UNIV_LOCAL_TIME_ZONE_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(univ_time_decoded == TRUE)
        {
          *pos += 7*num_bits_to_read;
          *buffer_length -= 7*num_bits_to_read;
          break;  
        }
        if(*buffer_length < 7*num_bits_to_read)
        {
          MSG_ERROR_DS_0(MM_SUB, "Available buffer is insufficient to decode UNIV_LOCAL_TIME_ZONE_IEI\n");
          *buffer_length = 0;
          return TRUE;
        }
        univ_time_decoded = TRUE; 
#endif

        emm_info->tp_serv_center_time_stamp.year = mm_convert_semi_octets(b_unpackb(src, *pos, num_bits_to_read));
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;

        emm_info->tp_serv_center_time_stamp.month = mm_convert_semi_octets(b_unpackb(src, *pos, num_bits_to_read));
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;

        emm_info->tp_serv_center_time_stamp.day = mm_convert_semi_octets(b_unpackb(src, *pos, num_bits_to_read));
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;

        emm_info->tp_serv_center_time_stamp.hour = mm_convert_semi_octets(b_unpackb(src, *pos, num_bits_to_read));
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;

        emm_info->tp_serv_center_time_stamp.minute = mm_convert_semi_octets(b_unpackb(src, *pos, num_bits_to_read));
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;

        emm_info->tp_serv_center_time_stamp.second = mm_convert_semi_octets(b_unpackb(src, *pos, num_bits_to_read));
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;
    
        emm_info->tp_serv_center_time_stamp.time_zone = mm_convert_time_zone(b_unpackb(src, *pos, num_bits_to_read));
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;
      
        emm_info->tp_serv_center_time_stamp_exists = TRUE;
        iei_exists = TRUE;
       
       break;     
      case NW_DAYLIGHT_SAVINGS_TIME_IEI: 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(daylight_savings_decoded == TRUE)
        {
          *pos += 2*num_bits_to_read;
          *buffer_length -= 2*num_bits_to_read;
          break;  
        }    
        if(*buffer_length < 2*num_bits_to_read)
        {
         MSG_ERROR_DS_0(MM_SUB, "Available buffer is insufficient to decode NW_DAYLIGHT_SAVINGS_TIME_IEI\n");
         *buffer_length = 0;
         return TRUE;
        }
        daylight_savings_decoded = TRUE; 
#endif
        num_bits_to_read = 8 + 6;/*8 --> Length, 6 --> Reserved*/
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;
        num_bits_to_read = 2;

        day_light_savings = (lte_nas_day_light_savings_type)b_unpackb(src, *pos, num_bits_to_read);
        if( (day_light_savings != NO_ADJUSTMENTS)&&
            (day_light_savings != PLUS_ONE_HOUR)&&
            (day_light_savings != PLUS_TWO_HOUR) )
        {
          MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: Invalid daylight saving time value = %d",
                         day_light_savings);
          return TRUE;
        }

        emm_info->day_light_savings = day_light_savings;
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;

        emm_info->day_light_savings_exists = TRUE;
        iei_exists = TRUE;
       break;
      default:
        MSG_ERROR_DS_0(MM_SUB, "Received unknwon IE, probably there could be a mismatch in the length, discarding the rest of parsing");
#ifndef FEATURE_LTE_NAS_DEEP_SEC
        *buffer_length = 0;
        return TRUE;
#endif
    }/*switch*/
#ifndef FEATURE_LTE_NAS_DEEP_SEC
    if(iei_exists == FALSE)
    {
      *buffer_length = temp_buf_len - (opt_iei_len + sizeof(opt_iei_len));
      *pos = temp_pos - (opt_iei_len + sizeof(opt_iei_len));
    }
    else
    {
      /*If length is incorrectly received, discard the rest of EMM message*/
      if(emm_len_check_failed == TRUE)
      {
        *buffer_length = 0;
        return TRUE;
      }
    }
#else
    if(iei_exists == FALSE)
    {
      MSG_ERROR_DS_0(MM_SUB, "Received NW_FULL_NAME_IEI/NW_PARTIAL_NAME_IEI with invalid length\n");
      *buffer_length = 0;
      return TRUE;
    }
#endif
  }/*while*/
  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_GUTI_REALLOC_CMD

DESCRIPTION
  This function decodes GUTI_DEALLOCATION_COMMAND message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_guti_realloc_cmd
(
  byte                          *src,
  word                          *pos, 
  word                          *buffer_length, 
  lte_nas_emm_guti_realloc_cmd  *guti_realloc_cmd
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  /*set iei_exists to FALSE - even if this value is not updated, decoded IEI will not be 
    considered invalid (even though it is decoded successfully)*/
  boolean iei_exists = TRUE;

  if(*buffer_length < (NAS_EMM_GUTI_REALLOC_CMD_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode GUTI REALLOCATION COMMAND",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /*----------------------------------------
            EPS mobile identity
  ----------------------------------------*/
  iei_exists = decode_guti_ie(src,pos,buffer_length, &guti_realloc_cmd->guti);
  *pos = *pos + num_bits_to_read;
  *buffer_length = *buffer_length-num_bits_to_read;
  if(!iei_exists)
  {
    MSG_ERROR_DS_0(MM_SUB, "Incorrect GUTI received, skipping message parser");
    return FALSE;
  }  
  if(*buffer_length == 0)
  {
    return TRUE;
  }
  /*----------------------------------------
         Tracking area update(optional)
  ----------------------------------------*/
  if(((*buffer_length/8) <= MAX_TAI_LIST_LEN)&&((*buffer_length/8) >= MIN_TAI_LIST_LEN))
  {
    num_bits_to_read = 8;
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length-num_bits_to_read;
    if(unpackedbytes == TAI_LIST_IEI)
    {
      iei_exists = decode_tai_list_ie(src,pos,buffer_length,&guti_realloc_cmd->tai_lst);
      if(!iei_exists)
      {
        guti_realloc_cmd->tai_list_exists = FALSE;  
      }
      else
      {
        guti_realloc_cmd->tai_list_exists = TRUE;    
      }
    }
    else
    {
      MSG_ERROR_DS_0(MM_SUB, "Unknown IEI received in GUTI REALLOCATION CMD");
      guti_realloc_cmd->tai_list_exists = FALSE;
    }
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to decode TAI LIST IEI");
    guti_realloc_cmd->tai_list_exists = FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_IDENTITY_REQ

DESCRIPTION
  This function decodes IDENTITY_REQUEST message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_identity_req(byte *src,word *pos, word *buffer_length, lte_nas_emm_identity_req  *identity_req)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;

  if(*buffer_length < (NAS_EMM_IDENTITY_REQUEST_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode IDENTITY REQUEST",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /*-------------------------------------------------
          Spare half octet + Identity Type 
  --------------------------------------------------*/
  num_bits_to_read = 4;
  *pos += num_bits_to_read;
  *buffer_length += num_bits_to_read;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read); 
  if(unpackedbytes > (byte)LTE_NAS_EMM_TMSI)
  {
    MSG_ERROR_DS_1(MM_SUB, "Identity type value is %d, value ranges between 0 & 4\n",
                   unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }
  *pos += num_bits_to_read;
  *buffer_length += num_bits_to_read;
  identity_req->mobile_identity = (mobile_id_type) unpackedbytes;
  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_TAU_REJ

DESCRIPTION
  This function decodes TAU_REJECT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_tau_reject(byte *src,word *pos, word *buffer_length, lte_nas_emm_tau_rej *tau_rej)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  byte opt_val_iei = 0, iei_len = 0;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len  = 0;
#ifdef FEATURE_LTE_REL10
  boolean t3346_val_decoded = FALSE;
#endif
#endif


  if(*buffer_length < (NAS_EMM_TAU_REJECT_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode TRACKING AREA UPDATE REJECT",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /*-----------------------------
           EMM Cause
  -----------------------------*/
  num_bits_to_read = 8;    
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  if((unpackedbytes <(byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS)||
    (unpackedbytes > (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED))
  { 
    MSG_ERROR_DS_1(MM_SUB, "EMM cause value is %d, unknown EMM Cause received\n",
                   unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }/* End of FOR loop*/  
  tau_rej->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
  MSG_HIGH_DS_1(MM_SUB, "EMM cause value is %d\n", tau_rej->emm_cause);
  *pos = *pos  + num_bits_to_read;
  *buffer_length = *buffer_length-num_bits_to_read;
 
 /*------------------------------------------------------------------
                     Decode Optional IEs
  -------------------------------------------------------------------*/
#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
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
    /* ---- IEI ----*/
    num_bits_to_read = 8;
    opt_val_iei = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length - num_bits_to_read;
 
    switch(opt_val_iei)
    {
#ifdef FEATURE_LTE_REL10
      case T3346_VAL_IEI:
        num_bits_to_read = 8;
        if(*buffer_length >= num_bits_to_read)
        {
          iei_len = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length - num_bits_to_read;
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: Insufficient buffer len = %d bits", *buffer_length);
          *buffer_length = 0;
          return TRUE;
        }

        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
        {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
          if(t3346_val_decoded == TRUE)
          {
            MSG_ERROR_DS_0(MM_SUB, "MSG_LIB: T3346_VAL_IEI is repeated, skipping this IEI");
            /* Skip the IE */
            *pos = *pos + iei_len*8;
            *buffer_length = *buffer_length - iei_len*8;
            break;
          }
          t3346_val_decoded = TRUE;
#endif

          /* Read T3346 value */
          if((iei_len == 0) || (iei_len > T3346_LEN))
          {
            MSG_ERROR_DS_2(MM_SUB, "MSG_LIB: T3346 Timer length received = %d byte(s). Expected = %d byte.\n",
                           iei_len,
                           T3346_LEN);
          }
          num_bits_to_read = 8;
          if((iei_len >= T3346_LEN) && (*buffer_length >= num_bits_to_read))
          {
            unpackedbytes = b_unpackb(src, *pos, num_bits_to_read); 
            *pos = *pos + num_bits_to_read;
            *buffer_length = *buffer_length - num_bits_to_read;

            tau_rej->t3346_val = calc_gprs_timer(unpackedbytes);

            /* if t3346 value is not zero/deactivated and received message is not
               integrity protected use a random value b/w 15-30 mins */
            if((tau_rej->t3346_val != 0) &&
               (tau_rej->security_hdr != INTEGRITY_PROTECT_CIPHER) &&
               (tau_rej->security_hdr != INTEGRITY_PROTECT))
            {
              tau_rej->t3346_val =((15 + mm_get_random_value(15)) * 60 * 1000);
              MSG_HIGH_DS_2(MM_SUB, "MSG_LIB: Random T3346 value is %d ms. sec_hdr=%d",
                            tau_rej->t3346_val,
                            tau_rej->security_hdr);
            }
            else
            {
              MSG_HIGH_DS_2(MM_SUB, "MSG_LIB: T3346 value is %d ms. sec_hdr=%d",
                            tau_rej->t3346_val,
                            tau_rej->security_hdr);
            }

            /* Skip remaining bits in the IE if needed */
            if(iei_len > T3346_LEN)
            {
              *pos = *pos + (iei_len - T3346_LEN)*8;
              *buffer_length = *buffer_length - (iei_len - T3346_LEN)*8;
            }
          }
          else
          {
            MSG_ERROR_DS_2(MM_SUB, "MSG_LIB: Invalid iei_len = %d bits or buffer len = %d bits",
                           iei_len,
                           *buffer_length);
            *buffer_length = 0;
            return TRUE;
          }
          break;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "MSG_LIB: T3346 value IE skipped as spec_ver is %d",
                        emm_ctrl_data_ptr->emm_3gpp_spec_ver);
          /* Skip the IE */
          *pos = *pos + iei_len*8;
          *buffer_length = *buffer_length - iei_len*8;
          break;
        }
#endif /* end FEATURE_LTE_REL10 */
      default:
        MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: Unknown IEI %d in TAU Reject", opt_val_iei);
      break;
    } /* end of switch(opt_val_iei) */
  } /* end of while(*buffer_length) */
  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_TAU_ACCEPT

DESCRIPTION
  This function decodes TAU_ACCEPT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_tau_accept(byte *src,word *pos, word *buffer_length, lte_nas_emm_tau_accept_type *tau_accept)
{
  byte num_bits_to_read = 0,unpackedbytes = 0;
  byte plmn_len = 3,opt_val_iei = 0;
  byte min_equivalent_plmn_len = 3;
  byte equivalent_plmn_len = 0,eps_update_result = 0;
  /*set iei_exists to FALSE - even if this value is not updated, decoded IEI will not be 
    considered invalid (even though it is decoded successfully)*/
  boolean iei_exists = TRUE;
  byte plmn_index = 0, octet =0;
  byte eps_nw_support_len = 0,cs_lcs_val = 0;

  word original_buffer_len = 0;
  byte max_equivalent_plmn_len = 45;
  boolean plmn_bcd_coded = FALSE;
#ifdef FEATURE_LTE_NAS_DEEP_SEC  
  word iei_buffer_len = 0, iei_pos = 0;
  boolean guti_decoded = FALSE, loc_area_id_decoded = FALSE;
  boolean ms_id_decoded = FALSE,emm_cause_decoded = FALSE;
  boolean t3402_val_decoded = FALSE,t3423_val_decoded = FALSE;
  boolean eplmn_decoded = FALSE, emergency_num_list_decoded = FALSE;
  boolean eps_nw_support_decoded = FALSE,additional_update_result_decoded= FALSE;
  boolean t3412_val_decoded = FALSE,tai_list_decoded = FALSE, eps_bearer_status_decoded = FALSE;
  byte max_emergency_num_list_len = 48;
#ifdef FEATURE_LTE_REL10
  boolean t3412_ext_val_decoded = FALSE;
#endif
#endif
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  word iei_len = 0;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  if(*buffer_length < (NAS_EMM_TAU_ACCEPT_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Buffer length = %d is insufficient to decode TRACKING AREA UPDATE ACCEPT",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  while(*buffer_length)
  {
    /*----------------------------------------------------------------
                         SPARE HALF BYTE
    ----------------------------------------------------------------*/
    num_bits_to_read = 4;
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length- num_bits_to_read;
    /*---------------------------------------------------------------
                        EPS ATTACH RESULT
    ----------------------------------------------------------------*/
    num_bits_to_read = 4;    
    eps_update_result = b_unpackb(src, *pos, num_bits_to_read);  
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length- num_bits_to_read;
    /*Check for validation*/
    if((eps_update_result == (byte)TA_UPDATE)||(eps_update_result <= (byte)TA_LA_UPDATE)||
       (eps_update_result == (byte)TA_UPDATE_ISR_ATIVATE)||
       (eps_update_result == (byte)TA_LA_UPDATE_ISR_ACTIVATE))
    {
      tau_accept->eps_update_result = (lte_nas_emm_eps_update_result_type) eps_update_result;
    }    
    else
    {
      *buffer_length = 0;
      return FALSE;
    } /*End of ELSE*/
    /*Restore the original buffer length here, keep comparing if the buffer 
      length has incremented on every iteration of IEI decoding, it is greater 
      than original buffer length, then over flow has occured, so quit the parsing 
      otherwise memory corruption is going to happen*/
    original_buffer_len = *buffer_length;
    /*-----------------------------------------------------------------
                         Decode Optional fields
    --------------------------------------------------------------------*/
    while(*buffer_length)
    {
      if(*buffer_length > original_buffer_len)
      {  
        MSG_ERROR_DS_0(MM_SUB, "******* Buffer over flow is foreseen, will quit parsing further IEIs for safety reasons ******");
        *buffer_length = 0;
        return TRUE;
      }
      /* ---- IEI ----*/
      num_bits_to_read = 8;
      opt_val_iei = b_unpackb(src, *pos, num_bits_to_read);
      *pos = *pos + num_bits_to_read;
      *buffer_length = *buffer_length-num_bits_to_read;

      /*Get IEI Length*/
      iei_len = b_unpackb(src, *pos, num_bits_to_read);
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      /*Store offset address & remaining buffer length, in case any octet 
      is faulty this info would be required to jump to next IEI*/
      iei_pos = *pos + 8; 
      iei_buffer_len = *buffer_length - 8;
#endif

      switch(opt_val_iei)
      {
       case T3412_TIMER_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(t3412_val_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "T3412_VAL_IEI is repeated, skipping this IEI");
          *pos = iei_pos;
          *buffer_length = iei_buffer_len; 
          break;
        }
        t3412_val_decoded = TRUE;
#endif
        num_bits_to_read = 8;    
        if(*buffer_length >= num_bits_to_read)
        {
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          tau_accept->t3412_val = calc_gprs_timer(unpackedbytes);
          MSG_HIGH_DS_1(MM_SUB, "T3412 value is %d\n", tau_accept->t3412_val);
          *pos = *pos +num_bits_to_read;
          *buffer_length = *buffer_length-num_bits_to_read;          
          tau_accept->t3412_val_exists = TRUE;
        }
        else
        {          
          MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to decode T3402 Value, skipping messsage parser\n");
          tau_accept->t3402_val_exists = FALSE;
          return TRUE;
        }  
        break; 
      case GUTI_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(guti_decoded == TRUE)
        {
          if(iei_len > EMM_GUTI_LEN)
          {
            iei_len = EMM_GUTI_LEN;
          }
          MSG_ERROR_DS_0(MM_SUB, "GUTI IEI is repeated, skipping this IEI");
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
          break;
        }
        guti_decoded = TRUE;
#endif 
        iei_exists = decode_guti_ie(src,pos,buffer_length, &tau_accept->guti);
        if(!iei_exists)
        {
          MSG_ERROR_DS_0(MM_SUB, "GUTI decoding failed");
          tau_accept->guti_type_exists = FALSE;  
#ifdef FEATURE_LTE_NAS_DEEP_SEC
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
#endif
        }
        else
        {
          tau_accept->guti_type_exists = TRUE; 
        }
        break;  
        case LOC_AREA_ID_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(loc_area_id_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "LOCATION_AREA_IDENTITY IEI is repeated, skipping this IEI");
          /*offset will now be pointing to the first byte of this IEI, so incrementing the offset 
          by the length of this IEI will take the offset to the 2nd byte of next IEI (if present)..
          so we need to subtract that length from POS and BUFFER_LENGTH*/
          *pos = iei_pos + (LOCATION_AREA_IDENTITY_LEN - 1)* 8; 
          *buffer_length = iei_buffer_len - (LOCATION_AREA_IDENTITY_LEN -1) * 8;   
          break;  
        }    
        loc_area_id_decoded = TRUE;  
#endif
        /* ---- Decode PLMN ---- */
        num_bits_to_read  = 8;
        for(octet =0;octet<plmn_len;octet++)
        {
          if(*buffer_length >= num_bits_to_read )
          {
            /*If the operator decides to use 2 digit MNC, then mnc digit 3 will have value '1111' (binary)*/
            byte two_digit_mnc_val = 15; /*15 = 1111 (binary)*/
            unpackedbytes = b_unpackb(src, *pos, num_bits_to_read); 
            /*decode 1 byte in plmn (in each time)*/
            if(octet == 1)
            {
              if(((unpackedbytes & 0x0F)> MAX_BCD_VAL) || 
                 (((((unpackedbytes & 0xF0)>>4) > MAX_BCD_VAL)&&((unpackedbytes & 0xF0)>>4) < two_digit_mnc_val)))              
              {
                MSG_ERROR_DS_1(MM_SUB, "PLMN octet %d does not have a valid BCD value, discarding LAI & invalidating ATTACH",
                               octet+1);
                *buffer_length = 0;
                return FALSE;
              }
            }
            else
            {
              if(((unpackedbytes & 0x0F)> MAX_BCD_VAL) ||(((unpackedbytes & 0xF0)>>4) > MAX_BCD_VAL))
              {
                MSG_ERROR_DS_1(MM_SUB, "PLMN octet %d does not have a valid BCD value, discarding LAI & invalidating ATTACH",
                               octet+1);
                *buffer_length = 0;
                return FALSE;
              }
            }
            tau_accept->loc_area_id.PLMN_id.identity[octet] = unpackedbytes;
            MSG_HIGH_DS_2(MM_SUB, "PLMN value for octet %d = %d\n",
                         octet+1, tau_accept->loc_area_id.PLMN_id.identity[octet]);
            *pos = *pos + num_bits_to_read; /*move the pointer to point to next byte (in plmn)*/
            *buffer_length = *buffer_length-num_bits_to_read;
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB, "Available buffer length insufficient to decode Location Area identification\n");
            tau_accept->loc_area_id_exists = FALSE;
            return TRUE;
          }
        }
        /* ---- Decode Location Area Code ---- */
        num_bits_to_read = 8;
        if(*buffer_length >= (num_bits_to_read * sizeof(tau_accept->loc_area_id.location_area_code)))
        {
          for(octet=0 ;octet < sizeof(tau_accept->loc_area_id.location_area_code);octet++)
          {
            tau_accept->loc_area_id.location_area_code[octet] = b_unpackb(src, *pos, num_bits_to_read);
            *pos = *pos  + num_bits_to_read;
            *buffer_length = *buffer_length-num_bits_to_read;
          }
          tau_accept->loc_area_id_exists = TRUE;
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB, "Available buffer length insufficient to decode Location Area identification\n");
          tau_accept->loc_area_id_exists = FALSE;
          return TRUE;
        }
        break;
      case MS_IDENTITY_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(ms_id_decoded == TRUE)
        {  
          if(iei_len > MAX_MS_IDENTITY_LEN)
          {
            iei_len = MAX_MS_IDENTITY_LEN;
          }
          MSG_ERROR_DS_0(MM_SUB, "MS_IDENTITY_IEI is repeated, skipping this IEI");
#ifdef FEATURE_LTE_NAS_DEEP_SEC
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
#endif
          break; 
        }   
        ms_id_decoded = TRUE;
#endif
        iei_exists = decode_ms_identity_ie(src,pos,buffer_length, &tau_accept->ms_id);
        if(!iei_exists)
        {
          MSG_ERROR_DS_0(MM_SUB, "MS_IDENTITY decoding failed");
          tau_accept->ms_id_exists = FALSE; 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
#endif
        }
        else
        {
          tau_accept->ms_id_exists = TRUE;
        }
        break;
      case EMM_CAUSE_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(emm_cause_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "EMM_CAUSE_IEI is repeated, skipping this IEI");
          *pos = iei_pos;
          *buffer_length = iei_buffer_len; 
          break;
        }
        emm_cause_decoded = TRUE;
#endif
        num_bits_to_read = 8;
        if(*buffer_length >= num_bits_to_read)
        {
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          if((unpackedbytes < (byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS)||
            (unpackedbytes > (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED))
          { 
            MSG_ERROR_DS_1(MM_SUB, "EMM cause value is %d, unknown EMM Cause received\n",
                           unpackedbytes);
            tau_accept->emm_cause_exists = FALSE;
#ifdef FEATURE_LTE_NAS_DEEP_SEC
            *pos = iei_pos;
            *buffer_length = iei_buffer_len;            
#endif
          }/* End of if()*/  
          else
          {
            tau_accept->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
            MSG_HIGH_DS_1(MM_SUB, "EMM cause IE is decoded successfully. EMM cause value is %d\n", tau_accept->emm_cause);
            *pos = *pos  + num_bits_to_read;
            *buffer_length = *buffer_length-num_bits_to_read;
            tau_accept->emm_cause_exists = TRUE;
          }
        }
        else
        {          
          MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to decode EMM Cause, skipping messsage parser\n");
          tau_accept->emm_cause_exists = FALSE;
          return TRUE;
        }
        break;
      case T3402_VAL_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(t3402_val_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "T3402_VAL_IEI is repeated, skipping this IEI");
          *pos = iei_pos;
          *buffer_length = iei_buffer_len; 
          break;
        }
        t3402_val_decoded = TRUE;
#endif
        
        num_bits_to_read = 8;    
        if(*buffer_length >= num_bits_to_read)
        {
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          tau_accept->t3402_val = calc_gprs_timer(unpackedbytes);
          MSG_HIGH_DS_1(MM_SUB, "T3423 value is %d\n", tau_accept->t3402_val);
          *pos = *pos +num_bits_to_read;
          *buffer_length = *buffer_length-num_bits_to_read;          
          tau_accept->t3402_val_exists = TRUE;
        }
        else
        {          
          MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to decode T3402 Value, skipping messsage parser\n");
          tau_accept->t3402_val_exists = FALSE;
          return TRUE;
        }          
        break; 
      case T3423_VAL_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(t3423_val_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "T3423_VAL_IEI is repeated, skipping this IEI");
          *pos = iei_pos;
          *buffer_length = iei_buffer_len; 
          break;           
        }
        t3423_val_decoded = TRUE;
#endif        
        /* GRPS TIMER VALUE*/
        num_bits_to_read = 8; 
        if(*buffer_length >= num_bits_to_read)
        {
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          tau_accept->t3423_val = calc_gprs_timer(unpackedbytes);
          MSG_HIGH_DS_1(MM_SUB, "T3423 value is %d\n", tau_accept->t3423_val);
          *pos = *pos +num_bits_to_read;
          *buffer_length = *buffer_length-num_bits_to_read;          
          tau_accept->t3423_val_exists = TRUE;
        }
        else
        {          
          MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to decode T3402 Value, skipping messsage parser\n");
          tau_accept->t3423_val_exists = FALSE;
          return TRUE;
        }
        break;
      case EQUIVALENT_PLMN_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC 
        if(iei_len > max_equivalent_plmn_len)
        {
          iei_len = max_equivalent_plmn_len;
        } 
        if(eplmn_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "EQUIVALENT_PLMN_IEI is repeated, skipping this IEI");
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
          break;
        }   
        eplmn_decoded = TRUE;
#endif
        /* ---- Equivalent PLMNs length ---- */
        num_bits_to_read = 8;    
        equivalent_plmn_len = b_unpackb(src,*pos, num_bits_to_read);
        if(equivalent_plmn_len>max_equivalent_plmn_len) 
        {
          MSG_ERROR_DS_0(MM_SUB, "MAX allowable list length is 45 bytes, truncating it to this value");
          equivalent_plmn_len = max_equivalent_plmn_len;
        }
        else if(equivalent_plmn_len < min_equivalent_plmn_len)
        {
          MSG_ERROR_DS_2(MM_SUB, "Number of PLMNs is %d, minimum number of PLMNs is %d",
                         equivalent_plmn_len/plmn_len, min_equivalent_plmn_len/plmn_len);
          *buffer_length = 0;
          tau_accept->plmn_exists = FALSE;
          return TRUE;
        }
        else if((equivalent_plmn_len*WORD_SIZE) > *buffer_length)
        {
          MSG_ERROR_DS_0(MM_SUB, "Received EQUIVALENT PLMN with a length greater than buffer size, skipping message parser\n");
          *buffer_length = 0;
          tau_accept->plmn_exists = FALSE;
          return TRUE;
        }

        /* ----  Number of Equivalent PLMNs ---- */
        tau_accept->plmn_lst.length_of_plmn_lst_contents = equivalent_plmn_len/plmn_len;        
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;

        /*Read the Value part of PLMN*/
        num_bits_to_read  = 8;
        for(plmn_index = 0; plmn_index < equivalent_plmn_len/plmn_len; plmn_index++)
        {
          for(octet = 0; octet < plmn_len; octet++)
          {
            tau_accept->plmn_lst.plmn[plmn_index].identity[octet] = b_unpackb(src, *pos, num_bits_to_read); 
            *pos = *pos + num_bits_to_read; /*move the pointer to point next byte (in plmn)*/
            *buffer_length = *buffer_length - num_bits_to_read;
          }

          plmn_bcd_coded = is_plmn_bcd_coded(tau_accept->plmn_lst.plmn[plmn_index]); /* check BCD coding */
          if( plmn_bcd_coded == FALSE )
          {
           MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: PLMN %d not BCD coded.", (plmn_index + 1));
           *buffer_length = 0;
           tau_accept->plmn_exists = FALSE;
           return TRUE;
          }
        }
        tau_accept->plmn_exists = TRUE;
        break;
      case TAI_LIST_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC 
        if(tai_list_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "TAI_LIST_IEI is repeated, skipping this IEI");
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
          break;
        }       
        tai_list_decoded = TRUE;
#endif
        
        /*Skip the length bit and start decoding*/
        num_bits_to_read = 8;
        iei_exists = decode_tai_list_ie(src,pos,buffer_length,&tau_accept->tai_lst);
        if(!iei_exists)
        {
          MSG_ERROR_DS_0(MM_SUB, "TAI_LIST decoding failed");
          tau_accept->tai_lst_exists = FALSE;  
#ifdef FEATURE_LTE_NAS_DEEP_SEC
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
#endif
        }
        else
        {          
          tau_accept->tai_lst_exists = TRUE;  
        }
        break;
      case EPS_BEARER_CONTEXT_STATUS:
#ifdef FEATURE_LTE_NAS_DEEP_SEC   
        if(eps_bearer_status_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "EPS_BEARER_STATUS_IEI is repeated, skipping this IEI");
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
          break;
        }
        eps_bearer_status_decoded = TRUE;
#endif        
        iei_exists = decode_eps_bearer_context_status
                     (src,pos,buffer_length,&tau_accept->eps_bearer_context_status);
        if(!iei_exists)
        {
          MSG_ERROR_DS_0(MM_SUB, "EPS_BEARER_STATUS decoding failed");
          tau_accept->eps_bearer_context_status_exists = FALSE;
        }
        else
        {
          tau_accept->eps_bearer_context_status_exists = TRUE;
#ifdef FEATURE_LTE_NAS_DEEP_SEC
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8; 
#endif
        }
        break;
      case EMERGENCY_NUM_LIST_IEI:  
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(iei_len > max_emergency_num_list_len)
        {
          iei_len = max_emergency_num_list_len;
        } 
        if(emergency_num_list_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "EMERGENCY_NUM_LIST_IEI is repeated, skipping this IEI");
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
          break;
        }
        emergency_num_list_decoded = TRUE;
#endif
        if((iei_len * 8) > *buffer_length)
        {
          MSG_ERROR_DS_2(MM_SUB, "MSG_LIB: EMC NUM list len %d exceeds buf len %d - Stop decode",
                         iei_len * 8,
                         *buffer_length);
          *buffer_length = 0;
          tau_accept->emergency_num_list_exists = FALSE;
          return TRUE;
        }
        else
        {
          decode_emergency_num_list(src,pos,buffer_length,(byte)iei_len,
                                    &tau_accept->emergency_num_list);
        }
        iei_exists = TRUE;
        if(tau_accept->emergency_num_list.emergency_number_count > 0)
        {
          tau_accept->emergency_num_list_exists = TRUE;
        }
        else
        {
          tau_accept->emergency_num_list_exists = FALSE;
        }       
        break;
      case EPS_NW_FEATURE_SUPPORT_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC  
        if(eps_nw_support_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "EPS_NW_FEATURE_SUPPORT_IEI is repeated, skipping this IEI");
          *pos = iei_pos + 8;
          *buffer_length = iei_buffer_len - 8;  
          break;
        }
        eps_nw_support_decoded = TRUE;
#endif
        num_bits_to_read = 8;
        eps_nw_support_len = b_unpackb(src,*pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        if(eps_nw_support_len>1) /*Max length = 1*/
        {
          MSG_ERROR_DS_0(MM_SUB, "MAX allowable list length is 1, truncating it to this value");
          eps_nw_support_len = 1;
        }
        else if((eps_nw_support_len*WORD_SIZE) > *buffer_length)
        {
          MSG_ERROR_DS_0(MM_SUB, "Received EPS_NW_FEATURE_SUPPORT length with a length greater than buffer size, skipping message parser\n");
          *buffer_length = 0;
          tau_accept->eps_nw_feature_support_exists = FALSE;
          return TRUE;
        }
#ifdef FEATURE_LTE_REL9
#ifdef FEATURE_LTE_REL10
        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
        {
          num_bits_to_read = 1;
          /* read ESR PS bit directly */
          tau_accept->eps_nw_feature_support.esr_ps_supported = b_unpackb(src,(word)(*pos+2), num_bits_to_read);
        }
#endif
        /* Adjust buffer length for 3 bits*/
        num_bits_to_read = 3;
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        /*Read "Location services indicator in CS " */
        num_bits_to_read = 2; 
        cs_lcs_val = b_unpackb(src,*pos, num_bits_to_read);
        if(cs_lcs_val > SYS_CS_LCS_SUPPORTED)
        {
          MSG_ERROR_DS_1(MM_SUB, "Received a reserved value %d for CS_LCS_VAL, skipping this IEI",
                         cs_lcs_val);
          tau_accept->eps_nw_feature_support_exists = FALSE;
          iei_exists =  FALSE;
          break;
        }
        tau_accept->eps_nw_feature_support.cs_lcs_val = cs_lcs_val;
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        /*Read "Location services indicator in EPC" */
        num_bits_to_read = 1; 
        tau_accept->eps_nw_feature_support.lcs_supported = b_unpackb(src,*pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        /*Read "Emergency bearer services indicator " */
        num_bits_to_read = 1; 
        tau_accept->eps_nw_feature_support.emc_bs_supported = b_unpackb(src,*pos, num_bits_to_read);
#else
        /*Skip 7 bits*/
        num_bits_to_read = 7;    
#endif
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        /*Read "MS voice over PS session indicator" */
        num_bits_to_read = 1;
        tau_accept->eps_nw_feature_support.ims_vops_supported = b_unpackb(src,*pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        tau_accept->eps_nw_feature_support_exists = TRUE;
        break;
#ifdef FEATURE_LTE_REL10
      case T3412_EXT_VAL_IEI: 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        /*TLV field*/
        if(t3412_ext_val_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "T3412_EXT_VAL_IEI is repeated, skipping this IEI");
          *pos = iei_pos + iei_len;
          *buffer_length = iei_buffer_len - iei_len*8; 
          break;
        }
        t3412_ext_val_decoded = TRUE;
#endif
        if(*buffer_length >= num_bits_to_read*2)
        {
          num_bits_to_read = 8;   
          /*increment the pos as iei is TLV*/
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length- num_bits_to_read;
         
          /*read the value*/
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          tau_accept->t3412_ext_val = gmm_convert_timer3_value_ie_to_msecs(unpackedbytes);
          tau_accept->t3412_ext_val_exists = TRUE;
          MSG_HIGH_DS_1(MM_SUB, "T3412 EXT value is %d", tau_accept->t3412_ext_val);
          *pos = *pos +num_bits_to_read;
          *buffer_length = *buffer_length-num_bits_to_read;          
        }
        else
        {          
          MSG_ERROR_DS_1(MM_SUB, "Buffer len %d insufficient to decode T3412 EXT Value",
                         num_bits_to_read/8);
          tau_accept->t3412_ext_val_exists = FALSE;
          return TRUE;
        }  
        break; 
#endif
        default:
        /*Check for ADDITIONAL_UPDATE_RESULT*/
        /*IEI value will be the 1st 4 bits of the octet that is already read, 
        so just mask the bits to get the value*/

        /*If in DEC SPEC, check for Additional Update Result*/
        if(ADDITIONAL_UPDATE_RESULT_IEI == (byte)(0xF0 & opt_val_iei))
        {        
#ifdef FEATURE_LTE_NAS_DEEP_SEC
          if(additional_update_result_decoded == TRUE)
          {
            MSG_ERROR_DS_0(MM_SUB, "ADDITIONAL_UPDATE_RESULT_IEI is repeated, skipping this IEI");
            break;
          }
          additional_update_result_decoded = TRUE;
#endif         
         /*IEI value will be the last 2 bits of the octet 
          that is already read, so just mask the bits to get the value*/
          if(0x03 != (sys_lte_cs_capability_e_type)(0x03 & opt_val_iei))
          {
            tau_accept->additional_update_result = (sys_lte_cs_capability_e_type)(0x03 & opt_val_iei);
            tau_accept->additional_update_result_exists = TRUE;
            MSG_HIGH_DS_1(MM_SUB, "ADDITIONAL_UPDATE_RESULT is decoded %d",
                          tau_accept->additional_update_result);
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB, "ADDITIONAL_UPDATE_RESULT has reserved value, discarding this IE");
          }    
        }
        else
        {
          /*UNKNOWN MESSAGE TYPE RECEIVED*/
          MSG_ERROR_DS_0(MM_SUB, "Unknown IE received in TAU ACCEPT; message library discarding it\n");
        }
        break;
      }/*End of Switch case*/
    }/*End of decoding optional fields*/
  }
  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_DETACH_REQUEST

DESCRIPTION
  This function decodes DETACH_REQUEST message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_detach_request(byte *src,word *pos, word *buffer_length, lte_nas_emm_mt_detach_req_type *mt_detach)
{
  byte num_bits_to_read = 0,unpackedbytes = 0;

  if(*buffer_length < (NAS_EMM_MT_DETACH_REQ_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "Buffer length = %d is insufficient to decode DETACH REQUEST", 
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /*----------------------------
        Spare half octet
  ----------------------------*/
  num_bits_to_read = 4;
  *pos = *pos + num_bits_to_read;
  *buffer_length = *buffer_length- num_bits_to_read; 
 
  /*----------------------------
           Detach Type
  ----------------------------*/
  num_bits_to_read = 4;
  mt_detach->detach_req = (lte_nas_emm_mt_detach_type) b_unpackb(src, *pos, num_bits_to_read);

  switch(mt_detach->detach_req)
  {
    case REATTACH_REQUIRED:
    case REATTACH_NOT_REQUIURED:
    case IMSI_DETACH_MT_DETACH:
      break;
    case RESERVED_MT_DETACH_TYPE_ONE:
    case RESERVED_MT_DETACH_TYPE_TWO:
      MSG_ERROR_DS_0(MM_SUB, "=EMM= MT DETACH REQ carries reserved type");
      *buffer_length = 0;
      return FALSE;
    default:
      mt_detach->detach_req = REATTACH_NOT_REQUIURED;
      break;
  }

  *pos = *pos + num_bits_to_read;
  *buffer_length = *buffer_length - num_bits_to_read; 

  /*-----------------------------
        EMM Cause (optional)
  -----------------------------*/
  if(*buffer_length)
  {
    num_bits_to_read = 8;    
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);  
    *pos = *pos  + num_bits_to_read;
    *buffer_length = *buffer_length-num_bits_to_read;
    if(unpackedbytes != EMM_CAUSE_IEI)
    {
      MSG_ERROR_DS_1(MM_SUB, "Unknown Information received, with a IE Value %d\n",
                     unpackedbytes);
      *buffer_length = 0; /*since this is the last IE, it doesn't matter if the buffer length is set to zero*/
      mt_detach->emm_cause_exists = FALSE;
      return TRUE;
    }
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);  
    if(!(((unpackedbytes == (byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS)||
         (unpackedbytes == (byte)LTE_NAS_ILLEGAL_UE))||
        ((unpackedbytes >= (byte)LTE_NAS_ILLEGAL_ME)&&
         (unpackedbytes <= (byte)LTE_NON_EPS_AUTH_UNACCEPTABLE))||
        ((unpackedbytes >= (byte)LTE_NAS_CS_FALLBACK_CALL_EST_NOT_ALLOWED)&&
         (unpackedbytes <= (byte)LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED))||
        ((unpackedbytes >= (byte)LTE_NAS_SEMANTICALLY_INCORRECT_MSG)&&
        (unpackedbytes <= (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED))))
    { 
      MSG_ERROR_DS_1(MM_SUB, "EMM cause value is %d, unknown EMM Cause received\n",
                     unpackedbytes);
      *buffer_length = 0;  /*since this is the last IE, it doesn't matter if the buffer length is set to zero*/
      mt_detach->emm_cause_exists = FALSE;
      return TRUE;
    }/* End of FOR loop*/  
    mt_detach->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
    MSG_LOW_DS_1(MM_SUB, "EMM cause value is %d\n", mt_detach->emm_cause);
    *pos = *pos  + num_bits_to_read;
    *buffer_length = *buffer_length-num_bits_to_read;
    mt_detach->emm_cause_exists = TRUE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_ATTACH_REJECT

DESCRIPTION
  This function decodes ATTACH_REJECT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_attach_reject
(
  byte *src,
  word *pos,
  word *buffer_length,
  lte_nas_emm_attach_rej_type *attach_reject,
  lte_nas_incoming_msg_type  *nas_incoming_msg_ptr
)
{
  byte num_bits_to_read = 0, unpackedbytes =0;
  boolean valid_esm_ie_present =FALSE;
  word esm_len = 0;
  word iei_len = 0;
  byte opt_val_iei = 0;
    
#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len = 0;
  boolean esm_iei_decoded = FALSE;
#ifdef FEATURE_LTE_REL10
  boolean t3402_val_decoded = FALSE;
  boolean t3346_val_decoded = FALSE;
#endif
#endif

  if(*buffer_length < (NAS_EMM_ATTACH_REJECT_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "Buffer length = %d is insufficient to decode ATTACH REJECT",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /*-----------------------------
           EMM Cause
  -----------------------------*/
  num_bits_to_read = 8;    
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  if((unpackedbytes < (byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS)||(unpackedbytes > (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED))
  { 
    MSG_ERROR_DS_1(MM_SUB, "EMM cause value is %d, unknown EMM Cause received\n",
                   unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }
  attach_reject->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
  *pos = *pos + num_bits_to_read;
  *buffer_length = *buffer_length - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  /*------------------------------------------------------------------
                     Decode Optional IEs
  -------------------------------------------------------------------*/
  while(*buffer_length)
  { 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
      MSG_ERROR_DS_2(MM_SUB, "Buffer over flow is foreseen, will quit parsing further IEIs. buffer_length %d  original_buffer_len %d",
                     *buffer_length, original_buffer_len);
      *buffer_length = 0;
      return TRUE;
    }
#endif
    /* ---- IEI ----*/
    num_bits_to_read = 8;
    opt_val_iei = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos  + num_bits_to_read;
    *buffer_length = *buffer_length-num_bits_to_read;

    switch(opt_val_iei)
    {
      case ESM_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(esm_iei_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "MSG_LIB: ESM_IEI is repeated, skipping this IEI");
          num_bits_to_read = 16;
          if(*buffer_length >= num_bits_to_read)
          {
            iei_len = b_unpackw(src, *pos, num_bits_to_read); /* Read IE Length */
            /* Skip the IE */
            *pos = *pos + num_bits_to_read + iei_len*8;
            *buffer_length = *buffer_length - num_bits_to_read - iei_len*8;
            break;
          }
          else
          {
            MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: Insufficient buffer len = %d bits",
                           *buffer_length);
            *buffer_length = 0;
            return TRUE;
          }
        }
        esm_iei_decoded = TRUE;
#endif
        if(*buffer_length >= MIN_ESM_CONTAINER_LEN)
        {
          
          /* --- Calculate ESM Length --- */
          num_bits_to_read =16;
          esm_len = b_unpackw(src, *pos, num_bits_to_read);
          MSG_HIGH_DS_1(MM_SUB, "MSG_LIB: ESM Length = %d\n", esm_len);
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length- num_bits_to_read;
		  send_nas_ota_msg_log_packet(LOG_LTE_NAS_ESM_OTA_IN_MSG_LOG_C,*buffer_length,src + (*pos/8));
          nas_esm_index++;      
          valid_esm_ie_present = decode_esm_msg(src,pos,buffer_length,esm_len, &nas_incoming_msg_ptr[nas_esm_index]);
          /* Attach reject can only come with ESM container with PDN connectivity reject */
          if((valid_esm_ie_present == FALSE) ||
             (nas_incoming_msg_ptr[nas_esm_index].esm_incoming_msg.nas_hdr.msg_id != PDN_CONNECTIVITY_REJECT))
          {       
            valid_esm_ie_present = FALSE;
            report_esm_failure(nas_incoming_msg_ptr);  
            MSG_ERROR_DS_0(MM_SUB, "MSG_LIB: ESM decoding failed!");
            *buffer_length  = 0;
          }
          else
          {
            report_esm_success(nas_incoming_msg_ptr);
          }
        }
      break;
#ifdef FEATURE_LTE_REL10
      case T3346_VAL_IEI:
        num_bits_to_read = 8;
        if(*buffer_length >= num_bits_to_read)
        {
          iei_len = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length - num_bits_to_read;
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: Insufficient buffer len = %d bits", *buffer_length);
          *buffer_length = 0;
          return TRUE;
        }

        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
        {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
          if(t3346_val_decoded == TRUE)
          {
            MSG_ERROR_DS_0(MM_SUB, "MSG_LIB: T3346_VAL_IEI is repeated, skipping this IEI");
            /* Skip the IE */
            *pos = *pos + iei_len*8;
            *buffer_length = *buffer_length - iei_len*8;
            break;
          }
          t3346_val_decoded = TRUE;
#endif

          /* Read T3346 value */
          if((iei_len == 0) || (iei_len > T3346_LEN))
          {
            MSG_ERROR_DS_2(MM_SUB, "MSG_LIB: T3346 Timer length received = %d byte(s). Expected = %d byte.\n",
                           iei_len,
                           T3346_LEN);
          }
          num_bits_to_read = 8;
          if((iei_len >= T3346_LEN) && (*buffer_length >= num_bits_to_read))
          {
            unpackedbytes = b_unpackb(src, *pos, num_bits_to_read); 
            *pos = *pos + num_bits_to_read;
            *buffer_length = *buffer_length - num_bits_to_read;

            attach_reject->t3346_val = calc_gprs_timer(unpackedbytes);

            /* if t3346 value is not zero/deactivated and received message is not
               integrity protected use a random value b/w 15-30 mins */
            if((attach_reject->t3346_val != 0) &&
               (attach_reject->security_hdr != INTEGRITY_PROTECT_CIPHER) &&
               (attach_reject->security_hdr != INTEGRITY_PROTECT))
            {
              attach_reject->t3346_val =((15 + mm_get_random_value(15)) * 60 * 1000);
              MSG_HIGH_DS_2(MM_SUB, "MSG_LIB: Random T3346 value is %d ms. sec_hdr=%d",
                            attach_reject->t3346_val,
                            attach_reject->security_hdr);
            }
            else
            {
              MSG_HIGH_DS_2(MM_SUB, "MSG_LIB: T3346 value is %d ms. sec_hdr=%d",
                            attach_reject->t3346_val,
                            attach_reject->security_hdr);
            }

            /* Skip remaining bits in the IE if needed */
            if(iei_len > T3346_LEN)
            {
              *pos = *pos + (iei_len - T3346_LEN)*8;
              *buffer_length = *buffer_length - (iei_len - T3346_LEN)*8;
            }
          }
          else
          {
            MSG_ERROR_DS_2(MM_SUB, "MSG_LIB: Invalid iei_len = %d bits or buffer len = %d bits",
                           iei_len,
                           *buffer_length);
            *buffer_length = 0;
            return TRUE;
          }
          break;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "MSG_LIB: T3346 value IE skipped as spec_ver is %d",
                        emm_ctrl_data_ptr->emm_3gpp_spec_ver);
          /* Skip the IE */
          *pos = *pos + iei_len*8;
          *buffer_length = *buffer_length - iei_len*8;
          break;
        }

    case T3402_VAL_ATTACH_REJECT_IEI:
        num_bits_to_read = 8;
        if(*buffer_length >= num_bits_to_read)
        {
          iei_len = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length - num_bits_to_read;
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: Insufficient buffer len = %d bits", *buffer_length);
          *buffer_length = 0;
          return TRUE;
        }

        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
        {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
          if(t3402_val_decoded == TRUE)
          {
            MSG_ERROR_DS_0(MM_SUB, "MSG_LIB: T3402_VAL_ATTACH_REJECT_IEI is repeated, skipping this IEI");
            /* Skip the IE */
            *pos = *pos + iei_len*8;
            *buffer_length = *buffer_length - iei_len*8;
            break;
          }
          t3402_val_decoded = TRUE;
#endif

          /* Read T3402 value */
          if((iei_len == 0) || (iei_len > MAX_T3402_VALUE_LEN))
          {
            MSG_ERROR_DS_2(MM_SUB, "MSG_LIB: T3402 Timer length received = %d byte(s). Expected = %d byte.\n",
                           iei_len,
                           MAX_T3402_VALUE_LEN);
          }
          num_bits_to_read = 8;
          if((iei_len >= MAX_T3402_VALUE_LEN) && (*buffer_length >= num_bits_to_read))
          {
            unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
            *pos = *pos + num_bits_to_read;
            *buffer_length = *buffer_length - num_bits_to_read;
            /* if message is integrity protected use the value provided */
            if((attach_reject->security_hdr == INTEGRITY_PROTECT_CIPHER) || 
               (attach_reject->security_hdr == INTEGRITY_PROTECT))
            {
              attach_reject->t3402_val = calc_gprs_timer(unpackedbytes);
              MSG_HIGH_DS_1(MM_SUB, "MSG_LIB: T3402 value is %d ms", attach_reject->t3402_val);
              attach_reject->t3402_val_exists = TRUE;
            }
            else
            { 
              attach_reject->t3402_val_exists = FALSE;
            }
                /* Skip remaining bits in the IE if needed */
            if(iei_len > MAX_T3402_VALUE_LEN)
            {
              *pos = *pos + (iei_len - MAX_T3402_VALUE_LEN)*8;
              *buffer_length = *buffer_length - (iei_len - MAX_T3402_VALUE_LEN)*8;
            }
          }
          else
          {
            MSG_ERROR_DS_2(MM_SUB, "MSG_LIB: Invalid iei_len = %d bits or buffer len = %d bits",
                           iei_len,
                           *buffer_length);
            attach_reject->t3402_val_exists = FALSE;
            *buffer_length = 0;
            return TRUE;
          }
          break;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "MSG_LIB: T3402 value IE skipped as spec_ver is %d",
                        emm_ctrl_data_ptr->emm_3gpp_spec_ver);
          /* Skip the IE */
          *pos = *pos + iei_len*8;
          *buffer_length = *buffer_length - iei_len*8;
          break;
        }
#endif /* end FEATURE_LTE_REL10 */
      default:
        MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: unknown IEI %d in Attach Reject", opt_val_iei);
      break;
    } /* end of switch(opt_val_iei) */
  } /* end of while(*buffer_length) */

  /* If EMM reject cause indicates ESM failure then network must 
  ** include ESM IE with PDN connectivity reject
  */
  if((attach_reject->emm_cause == LTE_NAS_ESM_FAILURE) &&
     (valid_esm_ie_present == FALSE))
  {
    MSG_ERROR_DS_0(MM_SUB, "MSG_LIB: EMM reject cause #19 came without PDN conn reject, dropping attach reject message");
    return FALSE;
  }
  return TRUE;
}
/*===========================================================================

FUNCTION  DECODE_ATTACH_ACCEPT

DESCRIPTION
  This function decodes ATTACH_ACCEPT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_attach_accept
(
  byte  *src,
  word  *pos, 
  word  *buffer_length, 
  lte_nas_emm_attach_accept_type  *attach_accept,
  struct emm_ctrl_data_tag        *emm_ctrl_data_ptr,
  lte_nas_incoming_msg_type       *nas_incoming_msg_ptr
)
{
  byte octet; 
  boolean valid_ie = FALSE;
  byte num_bits_to_read,unpackedbytes;
  word esm_len =0;
  byte plmn_len = 3,opt_val_iei;
  byte min_equivalent_plmn_len =3;
  byte equivalent_plmn_len,eps_attach_result;
  byte max_equivalent_plmn_len = 45;
  boolean plmn_bcd_coded = FALSE;
  byte plmn_index =0; 
  /*set iei_exists to FALSE - even if this value is not updated, decoded IEI will not be 
    considered invalid (even though it is decoded successfully)*/
  boolean iei_exists = TRUE;
  word temp_buffer_len = 0;
  word temp_pos = 0 ;
  byte max_emergency_num_list_len = 48;  
  word iei_pos = 0,iei_buffer_len = 0;
  byte iei_len = 0;
  word original_buffer_len = 0;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  boolean guti_decoded = FALSE, loc_area_id_decoded = FALSE;
  boolean ms_id_decoded = FALSE,emm_cause_decoded = FALSE;
  boolean t3402_val_decoded = FALSE,t3423_val_decoded = FALSE;
  boolean eplmn_decoded = FALSE, emergency_num_list_decoded = FALSE;
  boolean eps_nw_support_decoded = FALSE,additional_update_result_decoded= FALSE;
#ifdef FEATURE_LTE_REL10
  boolean t3412_ext_val_decoded = FALSE;
#endif
#endif

  byte eps_nw_support_len = 0,cs_lcs_val = 0;

  ASSERT(emm_ctrl_data_ptr != NULL);

  if(*buffer_length < (NAS_EMM_ATTACH_ACCEPT_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "Buffer length = %d is insufficient to decode ATTACH ACCEPT",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  while(*buffer_length)
  {
    /*-------------------------------------------------------------
                        SPARE HALF BYTE
    --------------------------------------------------------------*/
     num_bits_to_read = 4;
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length- num_bits_to_read;
    /*---------------------------------------------------------------
                       EPS ATTACH RESULT
    ----------------------------------------------------------------*/
    num_bits_to_read = 4;    
    eps_attach_result = b_unpackb(src, *pos, num_bits_to_read);  
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length- num_bits_to_read;
    /*Check for validation*/
    if((eps_attach_result == (byte)EPS_ONLY_ATTACH_RESULT)||
     (eps_attach_result == (byte)COMBINED_EPS_IMSI_ATTACH_RESULT))
    {
      attach_accept->eps_attach_result = (lte_nas_emm_eps_attach_result_type) eps_attach_result;
    }    
    else
    {
      MSG_ERROR_DS_1(MM_SUB, "Received unknown EPS ATTACH RESULT: %d", eps_attach_result);
      *buffer_length = 0;
      return FALSE;
    } /*End of ELSE*/
    /*----------------------------------------------------------------
                            T3412 value
    -----------------------------------------------------------------*/
    num_bits_to_read = 8; 
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read); 
    attach_accept->t3412_val = calc_gprs_timer(unpackedbytes);
    MSG_HIGH_DS_1(MM_SUB, "T3412 Value -> %d ms\n", attach_accept->t3412_val);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length- num_bits_to_read;
    /*-----------------------------------------------------------------
                             TAI List
    -----------------------------------------------------------------*/
    /*------------------------ TAI IEI --------------------*/
    valid_ie = decode_tai_list_ie(src,pos,buffer_length,&attach_accept->tai_lst);
    if(valid_ie == FALSE)
    {
      *buffer_length = 0;
      return FALSE;
    }
    /*-----------------------------------------------------------------
                     ESM MESSAGE CONTAINTER
    ------------------------------------------------------------------*/
    /*-----------------------------------------------------
                Length of ESM contents
    -----------------------------------------------------*/
    num_bits_to_read = 16;   
    esm_len = b_unpackw(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length- num_bits_to_read;
    temp_buffer_len = *buffer_length;
    temp_pos = *pos;
    /*Increase the counter*/
    nas_esm_index++;
    send_nas_ota_msg_log_packet(LOG_LTE_NAS_ESM_OTA_IN_MSG_LOG_C,
                              (word)(*buffer_length/8),src + (*pos/8));
    valid_ie = decode_esm_msg(src,pos,buffer_length,esm_len,&nas_incoming_msg_ptr[nas_esm_index]); 
    if(valid_ie == FALSE)
    {        
      MSG_ERROR_DS_0(MM_SUB, "ESM decoding failed! Proceeding with decoding EMM");
      report_esm_failure(nas_incoming_msg_ptr);
      *buffer_length =0;
      return FALSE;
    }
    else
    {
      report_esm_success(nas_incoming_msg_ptr);
      if(esm_length_check_failed == TRUE)
      {
        MSG_ERROR_DS_0(MM_SUB, "Message library skipping parsing of further messages");
      }
      else
      {
        *buffer_length = temp_buffer_len - (esm_len*WORD_SIZE);
        *pos = temp_pos + (esm_len*WORD_SIZE);
      }
    }
    /*Restore the original buffer length here, keep comparing if the buffer 
      length has incremented on every iteration of IEI decoding, it is greater 
      than original buffer length, then over flow has occured, so quit the parsing 
      otherwise memory corruption is going to happen*/
    original_buffer_len = *buffer_length;
    /*-----------------------------------------------------------------
                      Decode Optional fields
    -----------------------------------------------------------------*/
    while(*buffer_length)
    {
      if(*buffer_length > original_buffer_len)
      {  
        MSG_ERROR_DS_0(MM_SUB, "******* Buffer over flow is foreseen, will quit parsing further IEIs for safety reasons ******");
        *buffer_length = 0;
        return TRUE;
      }
      /* ---- IEI ----*/
      num_bits_to_read = 8;
      opt_val_iei = b_unpackb(src, *pos, num_bits_to_read);
      *pos = *pos + num_bits_to_read;
      *buffer_length = *buffer_length-num_bits_to_read;
      /*Get IEI Length*/
      iei_len = b_unpackb(src, *pos, num_bits_to_read);
      /*Store offset address & remaining buffer length, in case any octet 
      is faulty this info would be required to jump to next IEI*/
      iei_pos = *pos + 8; 
      iei_buffer_len = *buffer_length - 8;
      switch(opt_val_iei)
      {
      case GUTI_IEI: 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(guti_decoded == TRUE)
        {
          if(iei_len > EMM_GUTI_LEN)
          {
            iei_len = EMM_GUTI_LEN;
          }
          MSG_ERROR_DS_0(MM_SUB, "GUTI IEI is repeated, skipping this IEI");
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
          break;
        }
        guti_decoded = TRUE;
#endif
        iei_exists = decode_guti_ie(src,pos,buffer_length, &attach_accept->guti);
        if(!iei_exists)
        {
          attach_accept->guti_type_exists = FALSE; 
        }
        else
        {
          attach_accept->guti_type_exists = TRUE; 
        }
        break;  
      case LOC_AREA_ID_IEI: 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(loc_area_id_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "LOCATION_AREA_IDENTITY IEI is repeated, skipping this IEI");
          if(iei_len > LOCATION_AREA_IDENTITY_LEN)
          {
            iei_len = LOCATION_AREA_IDENTITY_LEN;
          }
          /*offset will now be pointing to the first byte of this IEI, so incrementing the offset 
          by the length of this IEI will take the offset to the 2nd byte of next IEI (if present)..
          so we need to subtract that length from POS and BUFFER_LENGTH*/
          *pos = iei_pos + (iei_len - 1)* 8; 
          *buffer_length = iei_buffer_len - (iei_len -1) * 8;   
          break;  
        }    
        loc_area_id_decoded = TRUE;  
#endif
        /* ---- Decode PLMN ---- */
        num_bits_to_read  = 8;
        for(octet =0;octet<plmn_len;octet++)
        {
          if(*buffer_length >= num_bits_to_read )
          {
            /*If the operator decides to use 2 digit MNC, then mnc digit 3 will have value '1111' (binary)*/
            byte two_digit_mnc_val = 15; /*15 = 1111 (binary)*/
            unpackedbytes = b_unpackb(src, *pos, num_bits_to_read); 
            /*decode 1 byte in plmn (in each time)*/
            if(octet == 1)
            {
              if(((unpackedbytes & 0x0F)> MAX_BCD_VAL) || 
                 (((((unpackedbytes & 0xF0)>>4) > MAX_BCD_VAL)&&((unpackedbytes & 0xF0)>>4) < two_digit_mnc_val)))
              {
                MSG_ERROR_DS_1(MM_SUB, "PLMN octet %d does not have a valid BCD value, discarding LAI & invalidating ATTACH",
                               octet+1);
                *buffer_length = 0;
                return FALSE;
              }
            }
            else
            {
              if(((unpackedbytes & 0x0F)> MAX_BCD_VAL) ||(((unpackedbytes & 0xF0)>>4) > MAX_BCD_VAL))
              {
                MSG_ERROR_DS_1(MM_SUB, "PLMN octet %d does not have a valid BCD value, discarding LAI & invalidating ATTACH",
                               octet+1);
                *buffer_length = 0;
                return FALSE;
              }
            }
            attach_accept->loc_area_id.PLMN_id.identity[octet] = unpackedbytes;
            *pos = *pos + num_bits_to_read; /*move the pointer to point to next byte (in plmn)*/
            *buffer_length = *buffer_length-num_bits_to_read;
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB, "Available buffer length insufficient to decode Location Area identification\n");
            attach_accept->loc_area_id_exists = FALSE;
            *buffer_length = 0;
            return TRUE;
          }
        }
        /* ---- Decode Location Area Code ---- */
        num_bits_to_read = 8;
        if(*buffer_length >= num_bits_to_read)
        {
          for(octet = 0; octet < sizeof(attach_accept->loc_area_id.location_area_code); octet++)
          {
            attach_accept->loc_area_id.location_area_code[octet] = b_unpackb(src, *pos, num_bits_to_read);
            *pos = *pos  + num_bits_to_read;
            *buffer_length = *buffer_length-num_bits_to_read;
          }
          attach_accept->loc_area_id_exists = TRUE;
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB, "Available buffer length insufficient to decode Location Area identification\n");
          *buffer_length = 0;          
          attach_accept->loc_area_id_exists = FALSE;
          return TRUE;
        }        
        break;
      case MS_IDENTITY_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(ms_id_decoded == TRUE)
        {  
          if(iei_len > MAX_MS_IDENTITY_LEN)
          {
            iei_len = MAX_MS_IDENTITY_LEN;
          }
          MSG_ERROR_DS_0(MM_SUB, "MS_IDENTITY_IEI is repeated, skipping this IEI");
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
          break; 
        }   
        ms_id_decoded = TRUE;
#endif
        iei_exists = decode_ms_identity_ie(src,pos,buffer_length, &attach_accept->ms_id);
        if(!iei_exists)
        {
          attach_accept->ms_id_exists = FALSE;
          *buffer_length = 0;          
          return TRUE; 
        }
        attach_accept->ms_id_exists = TRUE;
        break;
      case EMM_CAUSE_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(emm_cause_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "EMM_CAUSE_IEI is repeated, skipping this IEI");
          *pos = iei_pos;
          *buffer_length = iei_buffer_len; 
          break;
        }
        emm_cause_decoded = TRUE;
#endif
        num_bits_to_read = 8;
        if(*buffer_length >= num_bits_to_read)
        {
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          if((unpackedbytes < (byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS)||
             (unpackedbytes > (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED))
          { 
            MSG_ERROR_DS_1(MM_SUB, "EMM cause value is %d, unknown EMM Cause received\n",
                           unpackedbytes);
            
            attach_accept->emm_cause_exists = FALSE;
            *pos = *pos  + num_bits_to_read;
            *buffer_length = *buffer_length-num_bits_to_read;
            return TRUE; 
          }/* End of if()*/  
          attach_accept->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
          MSG_HIGH_DS_1(MM_SUB, "EMM cause value is %d\n", attach_accept->emm_cause);
          *pos = *pos  + num_bits_to_read;
          *buffer_length = *buffer_length-num_bits_to_read;
          attach_accept->emm_cause_exists = TRUE;
        }
        else
        {          
          MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to decode EMM Cause, skipping messsage parser\n");
          attach_accept->emm_cause_exists = FALSE;
          *buffer_length = 0;
          return TRUE;
        }
        break;
      case T3402_VAL_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(t3402_val_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "T3402_VAL_IEI is repeated, skipping this IEI");
          *pos = iei_pos;
          *buffer_length = iei_buffer_len; 
        }
        t3402_val_decoded = TRUE;
#endif
        num_bits_to_read = 8;    
        if(*buffer_length >= num_bits_to_read)
        {
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          attach_accept->t3402_val = calc_gprs_timer(unpackedbytes);
          MSG_HIGH_DS_1(MM_SUB, "T3402 value is %d\n", attach_accept->t3402_val);
          *pos = *pos +num_bits_to_read;
          *buffer_length = *buffer_length-num_bits_to_read;          
          attach_accept->t3402_val_exists = TRUE;
        }
        else
        {          
          MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to decode T3402 Value, skipping messsage parser\n");
          *buffer_length = 0;
          attach_accept->t3402_val_exists = FALSE;
          return TRUE;
        }
        break; 
      case T3423_VAL_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(t3423_val_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "T3423_VAL_IEI is repeated, skipping this IEI");
          *pos = iei_pos;
          *buffer_length = iei_buffer_len; 
          break;           
        }
        t3423_val_decoded = TRUE;
#endif        /*GRPS TIMER VALUE*/
        num_bits_to_read = 8; 
        if(*buffer_length >= num_bits_to_read)
        {
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          attach_accept->t3423_val = calc_gprs_timer(unpackedbytes);
          MSG_HIGH_DS_1(MM_SUB, "T3423 value is %d\n", attach_accept->t3423_val);
          *pos = *pos +num_bits_to_read;
          *buffer_length = *buffer_length-num_bits_to_read;
          attach_accept->t3423_val_exists = TRUE;
        }
        else
        {          
          MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to decode T3402 Value, skipping messsage parser\n");
          attach_accept->t3423_val_exists = FALSE;
          *buffer_length = 0;
          return TRUE;
        }
        break;
      case EQUIVALENT_PLMN_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(iei_len > max_equivalent_plmn_len)
        {
          iei_len = max_equivalent_plmn_len;
        } 
        if(eplmn_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "EQUIVALENT_PLMN_IEI is repeated, skipping this IEI");
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
          break;
        }   
        eplmn_decoded = TRUE;
#endif
        /* ---- Equivalent PLMNs length ---- */
        num_bits_to_read = 8;    
        equivalent_plmn_len = b_unpackb(src,*pos, num_bits_to_read);
        MSG_HIGH_DS_1(MM_SUB, "EQUIVALENT_PLMN_IEI length is %d", equivalent_plmn_len);
        if(equivalent_plmn_len>max_equivalent_plmn_len) 
        {
          MSG_ERROR_DS_0(MM_SUB, "MAX allowable EQ_PLMN_LEN length is 45 bytes, truncating it to this value");
          equivalent_plmn_len = max_equivalent_plmn_len;
        }
        else if(equivalent_plmn_len < min_equivalent_plmn_len)
        {
          MSG_ERROR_DS_2(MM_SUB, "EQUIVALENT_PLMN_IEI is %d, minimum length is %d, skipping the message parser",
                         equivalent_plmn_len, min_equivalent_plmn_len);
          *buffer_length = 0;
          attach_accept->plmn_exists = FALSE;
          return TRUE;
        }
        else if((equivalent_plmn_len*WORD_SIZE) > *buffer_length)
        {
          MSG_ERROR_DS_0(MM_SUB, "Received EQUIVALENT PLMN with a length greater than buffer size, skipping message parser\n");
          *buffer_length = 0;
          attach_accept->plmn_exists = FALSE;
          return TRUE;
        }
        /* ----  Number of Equivalent PLMNs ---- */
        attach_accept->plmn_lst.length_of_plmn_lst_contents = equivalent_plmn_len/plmn_len;        
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length-num_bits_to_read;

        /*Read the Value part of PLMN*/
        num_bits_to_read  = 8;
        for(plmn_index = 0; plmn_index < equivalent_plmn_len/plmn_len; plmn_index++)
        {
          for(octet = 0; octet < plmn_len; octet++)
          {
            attach_accept->plmn_lst.plmn[plmn_index].identity[octet] = b_unpackb(src, *pos, num_bits_to_read); 
            *pos = *pos + num_bits_to_read; /*move the pointer to point next byte (in plmn)*/
            *buffer_length = *buffer_length - num_bits_to_read;
          }
          
          plmn_bcd_coded = is_plmn_bcd_coded(attach_accept->plmn_lst.plmn[plmn_index]); /* check BCD coding */
          if( plmn_bcd_coded == FALSE )
          {
           MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: PLMN %d not BCD coded.", (plmn_index + 1));
           *buffer_length = 0;
           attach_accept->plmn_exists = FALSE;
           return TRUE;
          }
        }
        attach_accept->plmn_exists = TRUE;
        break;
      case EMERGENCY_NUM_LIST_IEI: 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(iei_len > max_emergency_num_list_len)
        {
          iei_len = max_emergency_num_list_len;
        } 
        if(emergency_num_list_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "EMERGENCY_NUM_LIST_IEI is repeated, skipping this IEI");
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;  
          break;
        }
        emergency_num_list_decoded = TRUE;
#endif
        if((iei_len * 8) > *buffer_length)
        {
          MSG_ERROR_DS_2(MM_SUB, "MSG_LIB: EMC NUM list len %d exceeds buf len %d - Stop decode",
                         iei_len * 8,
                         *buffer_length);
          *buffer_length = 0;
          attach_accept->emergency_num_list_exists = FALSE;
          return TRUE;
        }
        else
        {
          decode_emergency_num_list(src,pos,buffer_length,iei_len,
                                    &attach_accept->emergency_num_list);
        }
        
        iei_exists = TRUE;
        if(attach_accept->emergency_num_list.emergency_number_count > 0)
        {
          attach_accept->emergency_num_list_exists = TRUE;
        }
        else
        {
          attach_accept->emergency_num_list_exists = FALSE;
        }
        break;
      case EPS_NW_FEATURE_SUPPORT_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(eps_nw_support_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "EPS_NW_FEATURE_SUPPORT_IEI is repeated, skipping this IEI");
          *pos = iei_pos + 8;
          *buffer_length = iei_buffer_len - 8;  
          break;
        }
        eps_nw_support_decoded = TRUE;
#endif
        num_bits_to_read = 8;
        eps_nw_support_len = b_unpackb(src,*pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        MSG_HIGH_DS_1(MM_SUB, "EPS_NW_FEATURE_SUPPORT length is %d", eps_nw_support_len);
        if(eps_nw_support_len>1) /*Max length = 1*/
        {
          MSG_ERROR_DS_0(MM_SUB, "MAX allowable list length is 1, truncating it to this value");
          eps_nw_support_len = 1;
        }
        else if((eps_nw_support_len*WORD_SIZE) > *buffer_length)
        {
          MSG_ERROR_DS_0(MM_SUB, "Received EPS_NW_FEATURE_SUPPORT length with a length greater than buffer size, skipping message parser\n");
          *buffer_length = 0;
          attach_accept->eps_nw_feature_support_exists = FALSE;
          return TRUE;
        }

#ifdef FEATURE_LTE_REL9
        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
        {
#ifdef FEATURE_LTE_REL10
           if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
           {
             num_bits_to_read = 1;
             /* read ESR PS bit directly */
             attach_accept->eps_nw_feature_support.esr_ps_supported = b_unpackb(src,(word)(*pos+2), num_bits_to_read);
           }
#endif
           /* Adjust buffer length for 3 bits*/
          num_bits_to_read = 3;
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length - num_bits_to_read;
          /*Read "Location services indicator in CS " */
          num_bits_to_read = 2; 
          cs_lcs_val = b_unpackb(src,*pos, num_bits_to_read);
          if(cs_lcs_val > SYS_CS_LCS_SUPPORTED)
          {
            MSG_ERROR_DS_1(MM_SUB, "Received a reserved value %d for CS_LCS_VAL, skipping this IEI",
                           cs_lcs_val);
            attach_accept->eps_nw_feature_support_exists = FALSE;
            iei_exists =  FALSE;
            break;
          }
          attach_accept->eps_nw_feature_support.cs_lcs_val = cs_lcs_val;
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length - num_bits_to_read;
          /*Read "Location services indicator in EPC" */
          num_bits_to_read = 1; 
          attach_accept->eps_nw_feature_support.lcs_supported = b_unpackb(src,*pos, num_bits_to_read);
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length - num_bits_to_read;
          /*Read "Emergency bearer services indicator " */
          num_bits_to_read = 1; 
          attach_accept->eps_nw_feature_support.emc_bs_supported = b_unpackb(src,*pos, num_bits_to_read);
        }
#else
        /*Skip 7 bits*/
        num_bits_to_read = 7;    
#endif
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        /*Read "MS voice over PS session indicator" */
        num_bits_to_read = 1;
        attach_accept->eps_nw_feature_support.ims_vops_supported = b_unpackb(src,*pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        attach_accept->eps_nw_feature_support_exists = TRUE;
        break;
#ifdef FEATURE_LTE_REL10
      case T3412_EXT_VAL_IEI: 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        /*TLV field*/
        if(t3412_ext_val_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "T3412_EXT_VAL_IEI is repeated, skipping this IEI");
          *pos = iei_pos + iei_len;
          *buffer_length = iei_buffer_len - iei_len*8;
          break;
        }
        t3412_ext_val_decoded = TRUE;
#endif
        if(*buffer_length >= num_bits_to_read*2)
        {
          num_bits_to_read = 8;   
          /*increment the pos as iei is TLV*/
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length- num_bits_to_read;

          /*read the timer value*/
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          attach_accept->t3412_ext_val = gmm_convert_timer3_value_ie_to_msecs(unpackedbytes);
          attach_accept->t3412_ext_val_exists=TRUE;
          MSG_HIGH_DS_1(MM_SUB, "T3412 EXT value is %d", attach_accept->t3412_ext_val);
          *pos = *pos +num_bits_to_read;
          *buffer_length = *buffer_length-num_bits_to_read;          
        }
        else
        {          
          MSG_ERROR_DS_1(MM_SUB, "Buffer len %d insufficient to decode T3412 EXT Value",
                         num_bits_to_read/8);
          attach_accept->t3412_ext_val_exists=FALSE;
          return TRUE;
        }  
        break; 
#endif
        default:
        /*Check for ADDITIONAL_UPDATE_RESULT*/
        /*IEI value will be the 1st 4 bits of the octet that is already read, 
        so just mask the bits to get the value*/
        if(ADDITIONAL_UPDATE_RESULT_IEI == (byte)(0xF0 & opt_val_iei))
        {
#ifdef FEATURE_LTE_NAS_DEEP_SEC 
          if(additional_update_result_decoded == TRUE)
          {
            MSG_ERROR_DS_0(MM_SUB, "ADDITIONAL_UPDATE_RESULT_IEI is repeated, skipping this IEI");
            break;
          }
          additional_update_result_decoded = TRUE;
#endif
          /*IEI value will be the last 2 bits of the octet that is already read, 
          so just mask the bits to get the value*/
          if(0x03 != (sys_lte_cs_capability_e_type)(0x03 & opt_val_iei))
          {
            attach_accept->additional_update_result = (sys_lte_cs_capability_e_type)(0x03 & opt_val_iei);
            attach_accept->additional_update_result_exists = TRUE;
            MSG_HIGH_DS_1(MM_SUB, "ADDITIONAL_UPDATE_RESULT is decoded %d",
                          attach_accept->additional_update_result);
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB, "ADDITIONAL_UPDATE_RESULT has reserved value, discarding this IE");
          }            
        }
        else
        {
          /*UNKNOWN MESSAGE TYPE RECEIVED*/
          MSG_ERROR_DS_0(MM_SUB, "Unknown IE received in ATTACH ACCEPT; message library discarding it\n");
        }
        break;
       }/*End of Switch case*/
      /*==============================================
        Check to see if the IEI is faulty, if so move 
        to next IEI by incremement offset address and 
        decrementing available buffer length
       ==============================================*/
      if(iei_exists == FALSE)
      {
        if(*buffer_length > 0)
        {
          *pos = iei_pos + iei_len*8;
          *buffer_length = iei_buffer_len - iei_len*8;
          MSG_ERROR_DS_1(MM_SUB, "This Optional IEI %d is faulty, will skip this IEI and continue decoding the next optional IEI",
                         opt_val_iei);
          iei_exists = TRUE;
        }
      }
    }/*End of decoding optional fields*/
  }/*End of WHILE(buffer_length)*/  

  if((emm_ctrl_data_ptr->emm_guti_valid == FALSE) &&
     (attach_accept->guti_type_exists == FALSE))
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= No GUTI in ATTACH ACCEPT while no valid GUTI on UE");
    attach_accept->lte_nas_hdr.valid = FALSE;
    attach_accept->lte_nas_hdr.emm_decode_fail_reason 
                                = LTE_NAS_MSG_INCOMPATIBLE_WITH_PROTOCOL_STATE;
    nas_esm_index = 0;
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}/*End of "decodingAttachAccept" function*/

/*===========================================================================

FUNCTION  DECODE_SECURITY_MODE_CMD

DESCRIPTION
  This function decodes SECURITY_MODE_COMMAND message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_security_mode_cmd
(
  byte                              *src,
  word                              *pos, 
  word                              *buffer_length, 
  lte_nas_emm_security_mode_command *security_mode_cmd
)
{
  byte num_bits_to_read =0;
  byte ue_security_len =0, min_security_len =2, max_security_len =5;
  byte temp_ue_len =1, iei_val =0; 
  byte unpackedbytes =0;
  byte index =0;
#ifdef FEATURE_LTE_NAS_DEEP_SEC
  boolean nonce_ue_decoded = FALSE, nonce_mme_decoded = FALSE;
  boolean imeisv_decoded = FALSE;
  word original_buffer_len = 0;
#endif

#ifdef FEATURE_LTE_REL9
  emm_ctrl_data_type  *emm_ctrl_data_ptr;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL); 
#endif

  if(*buffer_length < (NAS_EMM_SECURITY_MODE_CMD_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "Buffer length = %d is insufficient to decode SECURITY MODE COMMAND",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  while(*buffer_length)
  {
    /*Skip 1 bit*/
    num_bits_to_read = 1;
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length - num_bits_to_read;
    /* Ciphering Algorithm*/
    num_bits_to_read = 3;
    security_mode_cmd->nas_security_alg.ciphering_alg =  
                              (lte_nas_emm_ciphering_alg_type) b_unpackb(src, *pos, num_bits_to_read);  
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length - num_bits_to_read;
    /*Skip 1 bit*/
    num_bits_to_read = 1;
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length - num_bits_to_read;
    /* Integrity Algorithm*/
    num_bits_to_read = 3;
    security_mode_cmd->nas_security_alg.integrity_protect_alg = 
                      (lte_nas_emm_integrity_protect_alg_type) b_unpackb(src, *pos, num_bits_to_read);  
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length - num_bits_to_read;
    /*--------------------------------------------------
                       Spare 
    --------------------------------------------------*/
    num_bits_to_read = 4;
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length- num_bits_to_read;
    /*--------------------------------------------------
             NAS key set identifier ASME / SGSN
    --------------------------------------------------*/
    num_bits_to_read = 1; 
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length- num_bits_to_read;
    num_bits_to_read = 3; 
    if(unpackedbytes == 1)
    {
      security_mode_cmd->nas_key_set_identifier_sgsn.security_context = 1;
      security_mode_cmd->nas_key_set_identifier_sgsn.keyset_id = b_unpackb(src, *pos, num_bits_to_read);
    }
    else
    {
      security_mode_cmd->nas_key_set_identifier_asme.keyset_id = b_unpackb(src, *pos, num_bits_to_read);
    }
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length- num_bits_to_read;
    /*--------------------------------------------------
            Replayed UE security capabilities
    --------------------------------------------------*/
    num_bits_to_read = 8; 
    ue_security_len = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length- num_bits_to_read;
    if(ue_security_len < min_security_len)
    {
      MSG_ERROR_DS_1(MM_SUB, "Security Header length is %d, minimum required length is 2\n",
                     ue_security_len);
      *buffer_length =0;
      return FALSE;
    }
    else if(ue_security_len > max_security_len)
    {
      MSG_ERROR_DS_1(MM_SUB, "Security Header length is %d, Maximum allowable length is 5\n",
                     ue_security_len);
      *buffer_length = 0;
      return FALSE;    
    }
    else if((ue_security_len*WORD_SIZE)> *buffer_length)
    {
      MSG_ERROR_DS_0(MM_SUB, "received security capability with a length greater than buffer size, skipping message parser\n");
      *buffer_length = 0;
      return FALSE;
    }
    num_bits_to_read = 1; 
    while(temp_ue_len <= ue_security_len)
    {  
      switch(temp_ue_len)
      {
      case 1:
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_128_eea0_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_128_eea1_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_128_eea2_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_eea3_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_eea4_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_eea5_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_eea6_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_encrypt_alg_eea7_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_enc_exists = emm_ctrl_data_ptr->ue_security_capability.eps_enc_exists;
        break;
      case 2:
        /* -- Spare bit --*/
        //*pos = *pos + 1;
        security_mode_cmd->ue_security_capability.spare_bit = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia1_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia2_supported =b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia3_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia4_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia5_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia6_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_integrity_alg_128_eia7_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.eps_int_exists = emm_ctrl_data_ptr->ue_security_capability.eps_int_exists;
        break;
      case 3:
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea0_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea1_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea2_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea3_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea4_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea5_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea6_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read; 
        security_mode_cmd->ue_security_capability.umts_encrypt_alg_uea7_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_enc_exists = emm_ctrl_data_ptr->ue_security_capability.umts_enc_exists;
        break;
      case 4:
        /* -- Spare bit --*/
        *pos = *pos + 1;
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia1_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia2_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia3_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia4_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia5_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia6_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_integrity_alg_uia7_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.umts_int_exists = emm_ctrl_data_ptr->ue_security_capability.umts_int_exists;
        break;
      case 5:
        /* -- Spare bit --*/
        *pos = *pos + 1;
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_1_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_2_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_3_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_4_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_5_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_6_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.gprs_encrypt_alg_7_supported = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        security_mode_cmd->ue_security_capability.gprs_enc_exists = emm_ctrl_data_ptr->ue_security_capability.gprs_enc_exists;
        break;
      }/*End of switch()*/
        temp_ue_len++;
      *buffer_length = *buffer_length - 8; /*One octet would have been read by now, so subtract one octet length*/
    }/*End of while()*/
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(security_mode_cmd->ue_security_capability.umts_enc_exists == TRUE && security_mode_cmd->ue_security_capability.umts_int_exists == FALSE){
      MSG_ERROR_DS_0(MM_SUB, "Security capability includes octet 5 but not octet 6\n");
      *buffer_length = 0;
      return FALSE;
    }

    original_buffer_len = *buffer_length;
#endif
    while(*buffer_length)
    {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
      if(*buffer_length > original_buffer_len)
      { 
        MSG_ERROR_DS_0(MM_SUB, "******* Buffer over flow is foreseen, will quit parsing further IEIs for safety reasons ******");
        *buffer_length = 0;
        return TRUE;
      }
#endif
      /* ---- IEI (Optional) ----*/
      num_bits_to_read =8;
      iei_val = b_unpackb(src, *pos, num_bits_to_read);
      *pos += num_bits_to_read;
      *buffer_length -= num_bits_to_read;

      switch(iei_val)
        {
        case NONCE_UE_IEI: 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(nonce_ue_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "NONCE_UE_IEI IEI is repeated, skipping this IEI");
          *pos += 4*num_bits_to_read;
          *buffer_length -= 4*num_bits_to_read;
          break;  
        }
        nonce_ue_decoded = TRUE; 
#endif
        /*--------------------------------------------------
                    Replayed nonceUE
        --------------------------------------------------*/
        num_bits_to_read = 8;
        for(index=0;index<4;index++)
        {
          if(*buffer_length >= num_bits_to_read)
          {
            security_mode_cmd->nonce_ue[index] = b_unpackb(src, *pos, num_bits_to_read);
            *pos = *pos + num_bits_to_read;
            *buffer_length -= num_bits_to_read;
            security_mode_cmd->nonce_ue_exists = TRUE;
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB, "Available buffer is insufficient to decode NONCE_UE_IEI\n");
            *buffer_length = 0;
            security_mode_cmd->nonce_ue_exists = FALSE;
            return TRUE;
          }
        }
      break;
      case NONCE_MME_IEI:
        /*--------------------------------------------------
                    NonceMME
        --------------------------------------------------*/
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(nonce_mme_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "NONCE_MME_IEI IEI is repeated, skipping this IEI");
          *pos += 4*num_bits_to_read;
          *buffer_length -= 4*num_bits_to_read;
          break;  
        }
        nonce_mme_decoded = TRUE; 
#endif
        num_bits_to_read = 8;
        for(index=0;index<4;index++)
        {
          if(*buffer_length >= num_bits_to_read)
          {
            security_mode_cmd->nonce_mme[index] = b_unpackb(src, *pos, num_bits_to_read);
            *pos = *pos + num_bits_to_read;
            *buffer_length -= num_bits_to_read;
            security_mode_cmd->nonce_mme_exists = TRUE;
          }
          else
          {
            MSG_ERROR_DS_0(MM_SUB, "Available buffer is insufficient to decode NONCE_MME_IEI\n");
            *buffer_length = 0;
            security_mode_cmd->nonce_mme_exists = FALSE;
            return TRUE;
          }
        } 
      break;
      default:
        /*--------------------------------------------------
                    Check for IMEISV request
        --------------------------------------------------*/
        /*---- Go back 8 bits and check for the first 4 bits (for IMEISV IEI) ----*/      
        *pos = *pos - num_bits_to_read;
        *buffer_length += num_bits_to_read;
        num_bits_to_read = 4;
        iei_val = b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;
        *buffer_length -= num_bits_to_read;
        if(iei_val == IMEISV_IEI)
        {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
          if(imeisv_decoded == TRUE)
          {
            MSG_ERROR_DS_0(MM_SUB, "IMEISV_IEI IEI is repeated, skipping this IEI");
            *pos += num_bits_to_read;
            *buffer_length -= num_bits_to_read;
            break;  
          }
          imeisv_decoded = TRUE; 
#endif
          /* --- Skip the 5th bit and read the IMEISV value*/
          *pos = *pos + 1;
          *buffer_length -= 1;
          /* ---- Get the IMEISV value ---- */
          num_bits_to_read = 3;
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          if(unpackedbytes == 1)
          {
            security_mode_cmd->imiesv_req =  IMIESV_REQUESTED;
          }
          else
          {
            security_mode_cmd->imiesv_req =  IMIESV_NOT_REQUESTED;    
          }
          *pos = *pos + num_bits_to_read;
          *buffer_length -= num_bits_to_read;
          security_mode_cmd->imiesv_req_exists = TRUE;
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB, "Unknown Optional IE received in Security mode command\n");
#ifdef FEATURE_LTE_NAS_DEEP_SEC
          *pos += num_bits_to_read;
          *buffer_length -= num_bits_to_read;
#else
          security_mode_cmd->imiesv_req =  IMIESV_NOT_REQUESTED; 
          *buffer_length = 0;
          return TRUE; 
#endif
        }
      break;
        }/*End of Switch*/
    }/*End of while(*buffer_length) - Mandatory + Optional*/
  }/*End of while(*buffer_length) - Optional*/
  return TRUE;  
}

/*===========================================================================

FUNCTION  DECODE_SERVICE_REJ

DESCRIPTION
  This function decodes SERVICE_REJECT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_service_rej(byte *src,word *pos, word *buffer_length, lte_nas_emm_service_rej_type *serv_rej)
{
  byte num_bits_to_read =0, unpackedbytes =0;  
  byte opt_val_iei = 0, iei_len = 0;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word original_buffer_len  = 0;
  boolean t3442_val_decoded = FALSE;
#ifdef FEATURE_LTE_REL10
  boolean t3346_val_decoded = FALSE;
#endif
#endif

  if(*buffer_length < (NAS_EMM_SERV_REJ_MIN_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "Buffer length = %d is insufficient to decode SERVICE REJECT",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /* ----- EMM CAUSE ----- */
  num_bits_to_read = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);  
  if(((unpackedbytes == (byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS)|| 
      (unpackedbytes == (byte)LTE_NAS_ILLEGAL_UE))           ||
     ((unpackedbytes >= (byte)LTE_NAS_ILLEGAL_ME) &&
      (unpackedbytes <= (byte)LTE_NON_EPS_AUTH_UNACCEPTABLE))||
     ((unpackedbytes >= (byte)LTE_NAS_CS_FALLBACK_CALL_EST_NOT_ALLOWED) &&
      (unpackedbytes <= (byte)LTE_NAS_SEVERE_NW_FAILURE)) ||
     ((unpackedbytes >= (byte)LTE_NAS_SEMANTICALLY_INCORRECT_MSG) &&
      (unpackedbytes <= (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED)))
  {  
    serv_rej->emm_cause = (lte_nas_emm_cause_type) unpackedbytes;
  } 
  else
  {
    MSG_ERROR_DS_1(MM_SUB, "EMM cause value is %d, unknown EMM Cause received\n",
                   unpackedbytes);
    *buffer_length = 0;
    return TRUE;
  }

  *pos = *pos  + num_bits_to_read;
  *buffer_length = *buffer_length-num_bits_to_read;
 /*------------------------------------------------------------------
                     Decode Optional IEs
  -------------------------------------------------------------------*/
#ifdef FEATURE_LTE_NAS_DEEP_SEC
  original_buffer_len = *buffer_length;
#endif

  while(*buffer_length)
  { 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(*buffer_length > original_buffer_len)
    {
     MSG_ERROR_DS_2(MM_SUB, "Buffer over flow, will quit parsing further IEIs. buffer_length %d  original_buffer_len %d",
                    *buffer_length, original_buffer_len);
     *buffer_length = 0;
     return TRUE;
    }
#endif
    /* ---- IEI ----*/
    num_bits_to_read = 8;
    opt_val_iei = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length - num_bits_to_read;

    switch(opt_val_iei)
    {
      case T3442_VAL_IEI:
#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(t3442_val_decoded == TRUE)
        {
          MSG_ERROR_DS_0(MM_SUB, "MSG_LIB: T3342_VAL_IEI is repeated, skipping this IEI");
          num_bits_to_read = 8;
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length - num_bits_to_read; 
          break;
        }
        t3442_val_decoded = TRUE;
#endif
        if(*buffer_length < T3442_LEN)
        {
          MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: Insufficient buffer len = %d bits", *buffer_length);
          *buffer_length = 0;
          return TRUE;
        }
        serv_rej->t3442_val_exists = FALSE;
        if(serv_rej->emm_cause == LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE)
        {
          /*----------------  T3442 Value ---------- */
          num_bits_to_read = 8;
          unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
          serv_rej->t3442_val = calc_gprs_timer(unpackedbytes);
          MSG_HIGH_DS_1(MM_SUB, "T3442 value is %d\n", serv_rej->t3442_val);
          serv_rej->t3442_val_exists = TRUE;
          *pos = *pos + num_bits_to_read;
          *buffer_length -= num_bits_to_read;  
        }
        else
        {
          MSG_ERROR_DS_0(MM_SUB, "MSG_LIB: Unknown IE received in Service Reject");
          *buffer_length = 0;
          return TRUE;
        }     
      break;
#ifdef FEATURE_LTE_REL10
      case T3346_VAL_IEI:
        num_bits_to_read = 8;
        if(*buffer_length >= num_bits_to_read)
        {
          iei_len = b_unpackb(src, *pos, num_bits_to_read); /* Read IE Length */
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length - num_bits_to_read;
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: Insufficient buffer len = %d bits", *buffer_length);
          *buffer_length = 0;
          return TRUE;
        }

        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
        {
#ifdef FEATURE_LTE_NAS_DEEP_SEC
          if(t3346_val_decoded == TRUE)
          {
            MSG_ERROR_DS_0(MM_SUB, "MSG_LIB: T3346_VAL_IEI is repeated, skipping this IEI");
            /* Skip the IE */
            *pos = *pos + iei_len*8;
            *buffer_length = *buffer_length - iei_len*8;
            break;
          }
          t3346_val_decoded = TRUE;
#endif

          /* Read T3346 value */
          if((iei_len == 0) || (iei_len > T3346_LEN))
          {
            MSG_ERROR_DS_2(MM_SUB, "MSG_LIB: T3346 Timer length received = %d byte(s). Expected = %d byte.\n",
                           iei_len,
                           T3346_LEN);
          }
          num_bits_to_read = 8;
          if((iei_len >= T3346_LEN) && (*buffer_length >= num_bits_to_read))
          {
            unpackedbytes = b_unpackb(src, *pos, num_bits_to_read); 
            *pos = *pos + num_bits_to_read;
            *buffer_length = *buffer_length - num_bits_to_read;

            serv_rej->t3346_val = calc_gprs_timer(unpackedbytes);

            /* if t3346 value is not zero/deactivated and received message is not
               integrity protected use a random value b/w 15-30 mins */
            if((serv_rej->t3346_val != 0) &&
               (serv_rej->security_hdr != INTEGRITY_PROTECT_CIPHER) &&
               (serv_rej->security_hdr != INTEGRITY_PROTECT))
            {
              serv_rej->t3346_val =((15 + mm_get_random_value(15)) * 60 * 1000);
              MSG_HIGH_DS_2(MM_SUB, "MSG_LIB: Random T3346 value is %d ms. sec_hdr=%d",
                            serv_rej->t3346_val,
                            serv_rej->security_hdr);
            }
            else
            {
              MSG_HIGH_DS_2(MM_SUB, "MSG_LIB: T3346 value is %d ms. sec_hdr=%d",
                            serv_rej->t3346_val,
                            serv_rej->security_hdr);
            }

            /* Skip remaining bits in the IE if needed */
            if(iei_len > T3346_LEN)
            {
              *pos = *pos + (iei_len - T3346_LEN)*8;
              *buffer_length = *buffer_length - (iei_len - T3346_LEN)*8;
            }
          }
          else
          {
            MSG_ERROR_DS_2(MM_SUB, "MSG_LIB: Invalid iei_len = %d bits or buffer len = %d bits",
                           iei_len,
                           *buffer_length);
            *buffer_length = 0;
            return TRUE;
          }
          break;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "MSG_LIB: T3346 value IE skipped as spec_ver is %d",
                        emm_ctrl_data_ptr->emm_3gpp_spec_ver);
          /* Skip the IE */
          *pos = *pos + iei_len*8;
          *buffer_length = *buffer_length - iei_len*8;
          break;
        }
#endif /* end FEATURE_LTE_REL10 */
      default:
        MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: Unknown IEI %d in Service Reject", opt_val_iei);
      break;
    } /* end of switch(opt_val_iei) */
  } /* end of while(*buffer_length) */
  return TRUE;  
}

/*===========================================================================

FUNCTION  DECODE_AUTHENTICATION_REQ

DESCRIPTION
  This function decodes AUTHENTICATION_REQUEST message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_authentication_req(byte *src,word *pos, word *buffer_length, lte_nas_emm_auth_req *auth_req)
{
  byte num_bits_to_read = 4, index = 0, unpackedbytes =0;
  byte keyset_id_reserved = 7;

  if(*buffer_length < AUTH_REQ_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "Buffer length = %d is insufficient to decode AUTHENTICATION REQUEST",
                   *buffer_length);
    *buffer_length  = 0;
    return FALSE;
  }

  /*-------------------------------------------------------
      Spare half octet +  NAS key set identifier(ASME) 
  --------------------------------------------------------*/
  *pos = *pos + num_bits_to_read;
  *buffer_length -= num_bits_to_read;

  num_bits_to_read = 1;
  *pos = *pos + num_bits_to_read;
  *buffer_length -= num_bits_to_read; 
  num_bits_to_read = 3;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  if( unpackedbytes == keyset_id_reserved )
  {
    MSG_ERROR_DS_1(MM_SUB, "Key set identifier value = %d is a reserved value.", unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }
  auth_req->nas_key_set_identifier_asme.keyset_id = unpackedbytes; 
  *pos = *pos + num_bits_to_read;
  *buffer_length -= num_bits_to_read;
  /*-------------------------------------------------------
       Authentication parameter RAND (EPS challenge) 
  --------------------------------------------------------*/
  num_bits_to_read = 8;
  for(index = 0; index < 16; index++)
  {
    auth_req->nas_auth_rand_param[index] = b_unpackb(src,*pos, num_bits_to_read);  
    *pos = *pos + num_bits_to_read;
    *buffer_length -= num_bits_to_read;
  }
  /*-------------------------------------------------------
        Authentication parameter AUTN (EPS challenge)
  --------------------------------------------------------*/
  unpackedbytes =  b_unpackb(src,*pos, num_bits_to_read); 
  *pos = *pos + num_bits_to_read;
  *buffer_length -= num_bits_to_read; 
  if(unpackedbytes != SIZE_OF_UMTS_AUTH_CHALLENGE_PARAM)
  {
    MSG_ERROR_DS_1(MM_SUB, "Received AUTH challenge with length  = %d, Expected length is 16",
                   unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }
  auth_req->nas_umts_auth_challenge.length  = unpackedbytes;
  for(index = 0; index < auth_req->nas_umts_auth_challenge.length; index++)
  {
    auth_req->nas_umts_auth_challenge.val[index] = b_unpackb(src,*pos, num_bits_to_read);    
    *pos = *pos + num_bits_to_read;
    *buffer_length -= num_bits_to_read;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_EMM_STATUS

DESCRIPTION
  This function decodes EMM_STATUS message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_emm_status
(
  byte                  *src,
  word                  *pos, 
  word                  *buffer_length,
  lte_nas_emm_status    *emm_status
)
{
  byte enum_val = 0, unpackedbytes = 0;  

  if(*buffer_length != (NAS_EMM_STATUS_LEN - MIN_EMM_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "Buffer length = %d is insufficient to decode EMM STATUS",
                   *buffer_length);
    *buffer_length = 0;
    return FALSE;
  }

  /* EMM cause */
  unpackedbytes = b_unpackb(src, *pos, *buffer_length);
/*lint -e774 */
/*lint -e831 */
  /*enum_val is initialized to ZERO & this value is not one of the values of EMM_CAUSE, so compiler thinks " 
  by default control would not go inside this for loop" - apparently this value will be changed when 
  EMM_CAUSE value is read, this was causing the lint error - this lint error is redundant*/
  if(((enum_val == (byte)LTE_NAS_IMSI_UNKNOWN_IN_HSS) ||
      (enum_val == (byte)LTE_NAS_ILLEGAL_UE)) ||
      ((enum_val >= (byte)LTE_NAS_ILLEGAL_ME) 
       && 
       (enum_val <= (byte)LTE_NON_EPS_AUTH_UNACCEPTABLE)) ||
      ((enum_val >= (byte)LTE_NAS_CS_FALLBACK_CALL_EST_NOT_ALLOWED)
       &&(enum_val <= (byte)LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED)) ||
      ((enum_val >= (byte)LTE_NAS_SEMANTICALLY_INCORRECT_MSG)
       &&(enum_val <= (byte)LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED)))
  {  
    MSG_ERROR_DS_1(MM_SUB, "LTE_NAS_MSG_LIB: Invalid EMM cause %d in EMM STATUS",
                   unpackedbytes);
    *buffer_length = 0;
    return FALSE;
  }
/*lint +e831 */
/*lint +e774 */

  emm_status->emm_cause = (lte_nas_emm_cause_type)unpackedbytes;      
  *pos = *pos  + *buffer_length;
  *buffer_length = 0; 
  return TRUE;  

} /* decode_emm_status */

/*===========================================================================

FUNCTION  DECODE_DOWNLINK_NAS_TRANSPORT

DESCRIPTION
  This function decodes DOWNLINK_NAS_TRANSPORT message. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean  decode_dl_nas_transport
(
  byte                          *src_ptr,
  word                          *pos_ptr, 
  word                          *buffer_length_ptr, 
  lte_nas_emm_dl_nas_transport  *dl_nas_transport_ptr
)
{
  word  msg_len = 0;
  ASSERT(src_ptr != NULL);
  ASSERT(pos_ptr != NULL);
  ASSERT(buffer_length_ptr != NULL);
  ASSERT(dl_nas_transport_ptr != NULL);

  if((*buffer_length_ptr < NAS_MSG_CONTAINER_MIN_LEN) ||
     (*buffer_length_ptr> NAS_MSG_CONTAINER_MAX_LEN))
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: Invalid msg container length in DL NAS TRANSPORT - %d",
                   *buffer_length_ptr);
    return FALSE;
  }

  msg_len = b_unpackb(src_ptr, *pos_ptr, 8);
  if((msg_len < EMM_SMS_DATA_OFFSET) ||
     (msg_len > MAX_OCTETS_L3_MESSAGE)) 
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: Invalid msg length in DL NAS TRANSPORT - %d",
                   msg_len);
    return FALSE;
  }
  /* Msg length */
  dl_nas_transport_ptr->nas_msg_container.msg_len = msg_len;
  *buffer_length_ptr -= 8;
  *pos_ptr += 8;

  memscpy((void*)&dl_nas_transport_ptr->nas_msg_container.msg_payload[0],
          sizeof(dl_nas_transport_ptr->nas_msg_container.msg_payload),
          (void *)(src_ptr + *pos_ptr/8),
          *buffer_length_ptr/8);

  *buffer_length_ptr = 0;

  return TRUE;
}/* decode_dl_nas_transport */

#endif /*FEATURE_LTE*/
