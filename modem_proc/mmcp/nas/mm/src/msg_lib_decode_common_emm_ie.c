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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_decode_common_emm_ie.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when         who                          what                              
--------    --------  ------------------------------------------------
10/16/14     RI        Mainlined spec changes upto Dec 09.
10/26/09     hnam      GUTI parsing is wrongly interpreted in teh spec,
                       corrected the working (based on June spec)                       
10/2/09      hnam      Fixed the GUTI validation check for MNC DIGIT 3
07/31/09     hnam      Added validation check for MNC/MCC in GUTI 
06/22/09     hnam      Fixed the "decode_tai_list" to parse tai list 0.
                       (Ref: CR 186713)
06/04/09     zren      Added code to save the type of tai list in Attach Accept
                       message
05/17/09     hnam      Added the code to support security
04/25/09     hnam      Removed Length parameter in EPS_BEARER_CONTEXT_STATUS
04/10/09     hnam      Added decode_ms_identity_ie function to encode ms identity
04/07/09     hnam      Fix m_tmsi and guti's mme group id encoding and decoding
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "bit.h"
#include "lte_nas.h"
#include "lte_nas_common_v.h"

#include "msg_lib_decode_common_emm_ie.h" 

#include "lte_nas_esm_message_type.h"
#include "lte_nas_esm_message_description.h"
#include "lte_nas_emm_message_description.h"
#include "msg_lib_declarations.h"
#include "msg.h"
#include "err.h"
#include "emm_utility.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
//#define MAX_TAI_ELEMENTS 16 /*There can only be a maximum of 16 TAI elemens*/

/*===========================================================================

                        FUNCTION DEFINITION

===========================================================================*/

/*===========================================================================

FUNCTION  IS_PLMN_BCD_CODED

DESCRIPTION
  This function checks BCD coding of a PLMN

DEPENDENCIES
  None

RETURN VALUE
  TRUE: PLMN BCD coded
  FALSE: PLMN not BCD coded

SIDE EFFECTS
  None

===========================================================================*/
boolean is_plmn_bcd_coded( sys_plmn_id_s_type plmn )
{
  /*If the operator decides to use 2 digit MNC, then mnc digit 3 will have value '1111' (binary)*/
  byte two_digit_mnc_val = 15; /*15 = 1111 (binary)*/

  /*if( (plmn.identity[0] & 0x0F) > MAX_BCD_VAL )*/
  if( LOWER_BCD(plmn.identity[0]) > MAX_BCD_VAL )
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: MCC digit1 = %X is not BCD coded.",
                   LOWER_BCD(plmn.identity[0]));
    return FALSE;
  }
  else if( UPPER_BCD(plmn.identity[0]) > MAX_BCD_VAL )
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: MCC digit2 = %X is not BCD coded.",
                   UPPER_BCD(plmn.identity[0]));
    return FALSE;
  }
  else if( LOWER_BCD(plmn.identity[1]) > MAX_BCD_VAL )
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: MCC digit3 = %X is not BCD coded.",
                   LOWER_BCD(plmn.identity[1]));
    return FALSE;
  }
  else if( (UPPER_BCD(plmn.identity[1]) > MAX_BCD_VAL)&& 
    (UPPER_BCD(plmn.identity[1]) != two_digit_mnc_val) )
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: MNC digit3 = %X is not BCD coded.",
                   UPPER_BCD(plmn.identity[1]));
    return FALSE;
  }
  else if( LOWER_BCD(plmn.identity[2]) > MAX_BCD_VAL )
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: MNC digit1 = %X is not BCD coded.",
                   LOWER_BCD(plmn.identity[2]));
    return FALSE;
  }
  else if( UPPER_BCD(plmn.identity[2]) > MAX_BCD_VAL )
  {
    MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: MNC digit2 = %X is not BCD coded.",
                   UPPER_BCD(plmn.identity[2]));
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* is_plmn_bcd_coded() END */


/*===========================================================================

FUNCTION  DECODE_EMERGENCY_NUM_LIST

DESCRIPTION
  This function decodes EMERGENCY_NUM_LIST Information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void decode_emergency_num_list
(
  byte                           *src,
  word                           *pos, 
  word                           *buffer_length,
  byte                            ie_length, 
  mmr_emergency_num_list_s_type  *emergency_num_list
)
{
  mm_parse_emergency_number(emergency_num_list, src + *pos/8);

  *pos = *pos + (ie_length + 1) * 8;
  *buffer_length = *buffer_length - (ie_length + 1) * 8;

}/* decode_emergency_num_list */

/*===========================================================================

FUNCTION  DECODE_NW_NAME

DESCRIPTION
  This function decodes NETWORK_NAME Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_nw_name(byte *src,word *pos, word *buffer_length, lte_nas_nw_name_type *nw_name)
{
  byte num_bits_to_read = 0;
  byte len = 0;
  byte spare_bit = 0;
  byte octet = 0;
  byte coding_scheme = 0;

  num_bits_to_read = 8;
  len = b_unpackb(src, *pos, num_bits_to_read);   
  *pos = *pos + num_bits_to_read;
  *buffer_length = *buffer_length - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  if(len > MAX_NW_NAME_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "NW name length is %d, length must be in the range 1-%d, setting the MAX length",
                   MAX_NW_NAME_LEN);
    len = MAX_NW_NAME_LEN;
  } 
  
  if(len > *buffer_length/8)
  {
    MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to decode NW name\n");
    return FALSE;
  }
  else if(len < 1)
  {
    MSG_ERROR_DS_1(MM_SUB, "NW name length is %d, length must be in the range 1-%d",
                   MAX_NW_NAME_LEN);
    return FALSE;
  }
#else
  if(len > *buffer_length)
  {
    MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to decode NW name\n");
    return FALSE;
  }
  else if(len > MAX_NW_NAME_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "NW name length is %d, length must be in the range 1-%d",
                   MAX_NW_NAME_LEN);
    return FALSE;
  } 
#endif

  if(len)
  {
    /*---------------------
        skip EXT bit
    ---------------------*/
    num_bits_to_read = 1;
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length - num_bits_to_read;
    /*---------------------
       coding scheme
    ---------------------*/
    num_bits_to_read = 3;
    coding_scheme = (lte_nas_emm_coding_scheme_type)b_unpackb(src, *pos, num_bits_to_read);
    if( (coding_scheme != GSM_DEFAULT_ALPHABET)&&
    (coding_scheme != UCS_2) )
    {
      MSG_ERROR_DS_1(MM_SUB, "MSG_LIB: Invalid coding scheme = %d", coding_scheme);
      return FALSE;
    }
    nw_name->coding_scheme = coding_scheme;
    MSG_HIGH_DS_1(MM_SUB, "MSG_LIB: Coding scheme = %d", nw_name->coding_scheme);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length - num_bits_to_read;
    /*---------------------
       Country Initial
    ---------------------*/
    num_bits_to_read = 1;
    nw_name->country_initial_req = b_unpackb(src, *pos, num_bits_to_read);
    MSG_HIGH_DS_1(MM_SUB, "MSG_LIB: Country Initial = %d", nw_name->country_initial_req);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length - num_bits_to_read;
    /*-----------------------------------------
       Number of spare bits in the last octet
    -----------------------------------------*/
    num_bits_to_read = 3;
    spare_bit = b_unpackb(src, *pos, num_bits_to_read);
    MSG_HIGH_DS_1(MM_SUB, "MSG_LIB: Spare bits in last octet = %d", spare_bit);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length - num_bits_to_read;

    len--; /*decrement buffer by 1 octet*/
    num_bits_to_read = 8;
    /*Last octet may have spare bits, so check for last octet in the next step*/
    for(octet =0;octet < len-1; octet++)
    {
      /*Added the following check to resolve Klocwork error*/
      if(octet < MAX_NW_NAME_LEN)
      {
        nw_name->val[octet] = b_unpackb(src, *pos, num_bits_to_read);
        MSG_HIGH_DS_2(MM_SUB, "MSG_LIB: NW_Name[%d] = %d", octet, nw_name->val[octet]);
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read; 
        nw_name->length++;
      }
    }
   
    if(len)
    {
      num_bits_to_read = 8 - spare_bit;
      if(octet < MAX_NW_NAME_LEN)
      {
        nw_name->val[octet] = b_unpackb(src,(word)(*pos+spare_bit), num_bits_to_read);
        MSG_HIGH_DS_2(MM_SUB, "MSG_LIB: NW_Name[%d] = %d", octet, nw_name->val[octet]);
      }
      *pos = *pos + 8;
      *buffer_length = *buffer_length - 8;    
      nw_name->length++;
    }
  }
  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_EPS_BEARER_CONTEXT_STATUS

DESCRIPTION
  This function decodes EPS_BEARER_CONTEXT_STATUS Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_eps_bearer_context_status(byte *src,word *pos, word *buffer_length, lte_nas_eps_bearer_context_status_type *bearer_context_status)
{
  byte num_bits_to_read = 0;
  byte len = 0;
  byte max_eps_bearer_status_len = 2;

  num_bits_to_read = 8;
  len = b_unpackb(src, *pos, num_bits_to_read);   
  *pos = *pos + num_bits_to_read;
  *buffer_length = *buffer_length - num_bits_to_read;

  if(len > *buffer_length)
  {
    MSG_ERROR_DS_0(MM_SUB, "Available buffer length is insufficient to decode EPS Bearer Context Status\n");
    return FALSE;
  }
  else if(len > max_eps_bearer_status_len)
  {
    MSG_ERROR_DS_1(MM_SUB, "Received EPS_BEARER_STATUS length is %d, maximum permissible length, truncating it to 2\n",
                   len);
    len = max_eps_bearer_status_len;
  }
#ifdef FEATURE_LTE_NAS_DEEP_SEC
  else if(len < max_eps_bearer_status_len)
  {
    MSG_ERROR_DS_1(MM_SUB, "BEARER_STATUS IEI is received with length = %d, discarding this IEI",
                   len);
    *buffer_length = 0;
    return FALSE;
  }
#endif

  num_bits_to_read =1;

  bearer_context_status->eps_bearer_context_7_exists =  b_unpackb(src, *pos, num_bits_to_read);  
  bearer_context_status->eps_bearer_context_6_exists =  b_unpackb(src,(word)(*pos+1), num_bits_to_read);
  bearer_context_status->eps_bearer_context_5_exists =  b_unpackb(src,(word)(*pos+2), num_bits_to_read);

  *pos = *pos + 8;
  *buffer_length = *buffer_length - 8; /*Move to next octet*/  
  
  bearer_context_status->eps_bearer_context_15_exists =  b_unpackb(src, *pos, num_bits_to_read);  
  bearer_context_status->eps_bearer_context_14_exists =  b_unpackb(src, (word)(*pos+1), num_bits_to_read);
  bearer_context_status->eps_bearer_context_13_exists =  b_unpackb(src, (word)(*pos+2), num_bits_to_read);
  bearer_context_status->eps_bearer_context_12_exists =  b_unpackb(src, (word)(*pos+3), num_bits_to_read);  
  bearer_context_status->eps_bearer_context_11_exists =  b_unpackb(src, (word)(*pos+4), num_bits_to_read);
  bearer_context_status->eps_bearer_context_10_exists =  b_unpackb(src, (word)(*pos+5), num_bits_to_read);
  bearer_context_status->eps_bearer_context_9_exists =  b_unpackb(src, (word)(*pos+6), num_bits_to_read);  
  bearer_context_status->eps_bearer_context_8_exists =  b_unpackb(src, (word)(*pos+7), num_bits_to_read);

  *pos = *pos + 8;
  *buffer_length = *buffer_length - 8;

  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_TAI_LIST_IE

DESCRIPTION
  This function decodes TAI_LIST Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_tai_list_ie(byte *src,word *pos, word *buffer_length, lte_nas_emm_tai_lst_info_type *msg_type)
{  
  byte tai_list_len = 0, max_tai_elements = 0, loop = 0;
  word max_tai_bits = 0;
  byte num_bits_to_read = 0, tai_lst_index = 0, plmn_len = 3, octet = 0;
  byte tac_len = 2; 
  byte tai_count = 0;
  byte partial_tai_list_count = 0;
  byte tai_list_type;
  boolean plmn_bcd_coded = FALSE;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  word temp_max_tai_bits = 0;
#endif

  /*----------------------- TAI List Length ----------------------------*/
  num_bits_to_read = 8;
  tai_list_len = b_unpackb(src, *pos, num_bits_to_read); 

  *pos = *pos + num_bits_to_read;
  *buffer_length = *buffer_length - num_bits_to_read;

  if(tai_list_len > MAX_TAI_LIST_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "Received TAI List length is %d, maximum permissible length, truncating it to 96\n",
                   tai_list_len);
    tai_list_len = MAX_TAI_LIST_LEN;
  }
  else if(tai_list_len < MIN_TAI_LIST_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "Received TAI List length is %d, minimum required tai list length (after tai list length octet) is 6\n",
                   tai_list_len);
    *buffer_length = 0;
    return FALSE;
  }

  MSG_MED_DS_1(MM_SUB, "TAI List length = %d\n", tai_list_len);
  max_tai_bits = (tai_list_len * 8); /*Keep track of the length in bits*/

  if(max_tai_bits > *buffer_length)
  {
    MSG_ERROR_DS_0(MM_SUB, "Received TAI with a length greater than buffer size, skipping message parser\n");
    *buffer_length = 0;
    return FALSE;
  }

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  /*Store the value of MAX_TAI length in this temporary variable, use it for comparison in the 
   "while" loop - this avoids crash - OTA messages can come as they want*/
  temp_max_tai_bits = max_tai_bits;
#endif
    
  while(max_tai_bits)
  { 
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    if(( max_tai_bits > temp_max_tai_bits) || (*buffer_length < max_tai_bits ))
    {  
      MSG_ERROR_DS_0(MM_SUB, "******* Buffer over flow is foreseen, will quit parsing further IEIs for safety reasons ******");
      *buffer_length = 0;
      return FALSE;
    }
#endif

    if(tai_count == MAX_NO_OF_TAI)
    {
      MSG_HIGH_DS_1(MM_SUB, "Max permissible TAIs = %d already decoded. Ignore remaining bits in TAI list IE.",
                    MAX_NO_OF_TAI);
      *pos = *pos + max_tai_bits;
      *buffer_length = *buffer_length - max_tai_bits;
      break;
    }

    /*------- spare bit --------*/
    num_bits_to_read = 1;

    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length - num_bits_to_read;
    max_tai_bits = max_tai_bits - num_bits_to_read;

    /*Type of list*/
    num_bits_to_read = 2;

    tai_list_type = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length - num_bits_to_read;
    max_tai_bits = max_tai_bits - num_bits_to_read;
    
    /*TAI_LIST type 0 & 1 are similar, so we use TYPELIST_0 structure for Typelist 0 & 1*/
    if((tai_list_type == 0) || (tai_list_type == 1))
    {
      msg_type->tai_lst_elem[tai_lst_index].tai_lst = NAS_EMM_TAI_LST0;
    }
#ifdef FEATURE_LTE_NAS_DEEP_SEC
    else if(tai_list_type == 2)
    {
      msg_type->tai_lst_elem[tai_lst_index].tai_lst = NAS_EMM_TAI_LST2;
    }
    else
    {
      /*skip remaining TAI bits*/
      MSG_ERROR_DS_1(MM_SUB, "Invalid partial TAI list type %d.", tai_list_type);
      *pos = *pos + max_tai_bits;
      *buffer_length = *buffer_length - max_tai_bits;
      return FALSE;
    }
#else
    else
    {
      msg_type->tai_lst_elem[tai_lst_index].tai_lst = (lte_nas_tai_lst_type)tai_list_type;
    }
#endif

    switch(tai_list_type)
    {
      case NAS_EMM_TAI_LST0:/*----- TAI list type 0 ----*/
        partial_tai_list_count++;
        /* ----- Decode tac_list_length ---- */
        num_bits_to_read = 5;  
        max_tai_elements = b_unpackb(src, *pos, num_bits_to_read) + 1;

        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        max_tai_bits = max_tai_bits - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(max_tai_elements > (MAX_NO_OF_TAI - tai_count))
        {
          MSG_HIGH_DS_2(MM_SUB,"TAIs received = %d exceed max available = %d. Restricting to max.",
                        max_tai_elements,(MAX_NO_OF_TAI - tai_count));
          max_tai_elements = (MAX_NO_OF_TAI - tai_count);
        }
#endif
        if(max_tai_bits < ((plmn_len + (max_tai_elements * tac_len)) * 8))
        {
          MSG_ERROR_DS_0(MM_SUB, " Available TAI List length is insufficient to completely decode typelist 0.");
          /*skip remaining TAI bits*/
          *pos  = *pos + max_tai_bits;
          *buffer_length = *buffer_length - max_tai_bits;
          return FALSE;
        }

        msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.tac_lst_length = max_tai_elements;
        MSG_HIGH_DS_2(MM_SUB, "Total elements in partial TAI list type %d = %d\n",
                     tai_list_type, max_tai_elements);
 
      /* ---- decode plmn ---- */
      for(octet =0;octet<plmn_len;octet++)
      {
        num_bits_to_read  = 8; /*decode 1 byte in plmn (in each time)*/
        msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.plmn.identity[octet] = b_unpackb(src, *pos, num_bits_to_read);
        
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length- num_bits_to_read;
        max_tai_bits = max_tai_bits-num_bits_to_read;
      }

      plmn_bcd_coded = is_plmn_bcd_coded(msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.plmn); /* check BCD coding */
      if( plmn_bcd_coded == FALSE )
      {
        MSG_ERROR_DS_0(MM_SUB, "MSG_LIB: PLMN not BCD coded.");
        return FALSE;
      }

        /*Decode the bytes representing Tracking Area Code*/
        for(octet = 0;octet < max_tai_elements; octet++)
        {
          num_bits_to_read =  16; /*Each TAC takes 2 bytes, so decode 16 bits of data*/
          msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.tac_lst[octet] = b_unpackw(src, *pos, num_bits_to_read);
   
          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length- num_bits_to_read;
          max_tai_bits = max_tai_bits-num_bits_to_read;
          tai_count++;
      }
      break;

    case NAS_EMM_TAI_LST1: /*----- TAI list type 1 ----*/
        partial_tai_list_count++;
        /* ----- Decode tac_list_length ---- */
        num_bits_to_read = 5;
        max_tai_elements = b_unpackb(src, *pos, num_bits_to_read) + 1;
        
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        max_tai_bits = max_tai_bits - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(max_tai_elements > (MAX_NO_OF_TAI - tai_count))
        {
          MSG_HIGH_DS_2(MM_SUB,"TAIs received = %d exceed max available = %d. Restricting to max.",
                        max_tai_elements,(MAX_NO_OF_TAI - tai_count));
          max_tai_elements = (MAX_NO_OF_TAI - tai_count);
        }
#endif
        if(max_tai_bits < ((plmn_len + tac_len) * 8))
        {
          MSG_ERROR_DS_0(MM_SUB, " Available TAI List length is insufficient to completely decode typelist 1.");
          /*skip remaining TAI bits*/
          *pos  = *pos + max_tai_bits;
          *buffer_length = *buffer_length - max_tai_bits;
          return FALSE;
        }

      /* ---- decode plmn ---- */
        for(octet = 0;octet < plmn_len; octet++)
        {
          num_bits_to_read = 8; /*decode 1 byte in plmn (in each time)*/
          msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.plmn.identity[octet] = b_unpackb(src, *pos, num_bits_to_read);

          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length - num_bits_to_read;
          max_tai_bits = max_tai_bits - num_bits_to_read;
        }

        plmn_bcd_coded = is_plmn_bcd_coded(msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.plmn); /* check BCD coding */
        if( plmn_bcd_coded == FALSE )
        {
          MSG_ERROR_DS_0(MM_SUB, "MSG_LIB: PLMN not BCD coded.");
          return FALSE;
        }

      /* -----  Decode TAC Value ---- */
        num_bits_to_read =  16; /*Each TAC takes 2 bytes, so decode 16 bits of data*/
        msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.tac_lst_length = max_tai_elements;
        msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.tac_lst[0] = b_unpackw(src, *pos, num_bits_to_read);

        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        max_tai_bits = max_tai_bits - num_bits_to_read;        
        tai_count++;

        /* 1 TAC decoded. Compute remaining TACs */
        for(octet = 1;octet < max_tai_elements; octet++)
        {
          msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.tac_lst[octet] = 
                                              msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst0.tac_lst[octet - 1] + 1;
          tai_count++;
        }
        break;

      case NAS_EMM_TAI_LST2: /*----- TAI list type 2 ----*/                             
        partial_tai_list_count++;
        /* ----- Decode tac_list_length ---- */
        num_bits_to_read = 5;  
        max_tai_elements = b_unpackb(src, *pos, num_bits_to_read) + 1;

        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        max_tai_bits = max_tai_bits - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
        if(max_tai_elements > (MAX_NO_OF_TAI - tai_count))
        {
          MSG_HIGH_DS_2(MM_SUB,"TAIs received = %d exceed max available = %d. Restricting to max.",
                        max_tai_elements,(MAX_NO_OF_TAI - tai_count));
          max_tai_elements = (MAX_NO_OF_TAI - tai_count);
        }
#endif
        if(max_tai_bits < ((plmn_len + tac_len) * max_tai_elements * 8))
        {
          MSG_ERROR_DS_0(MM_SUB, " Available TAI List length is insufficient to completely decode typelist 2.");
          /*skip remaining TAI bits*/
          *pos  = *pos + max_tai_bits;
          *buffer_length = *buffer_length - max_tai_bits;
          return FALSE;
        }

        msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst2.tai_lst_length = max_tai_elements;

        for(loop = 0;loop < max_tai_elements; loop++)
        {
          /* ---- decode plmn ---- */
          for(octet = 0;octet < plmn_len; octet++)
          {
            num_bits_to_read  = 8; /*decode 1 byte in plmn (in each time)*/
            msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst2.diff_plmn[loop].plmn.identity[octet] = b_unpackb(src, *pos, num_bits_to_read); 

            *pos = *pos + num_bits_to_read;
            *buffer_length = *buffer_length - num_bits_to_read;
            max_tai_bits = max_tai_bits - num_bits_to_read;
          }

          plmn_bcd_coded = is_plmn_bcd_coded(msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst2.diff_plmn[loop].plmn); /* check BCD coding */
          if( plmn_bcd_coded == FALSE )
          {
            MSG_ERROR_DS_0(MM_SUB, "MSG_LIB: PLMN not BCD coded.");
            return FALSE;
          }

          /* -----  Decode TAC Value ---- */
          num_bits_to_read =  16; /*Each TAC takes 2 bytes, so decode 16 bits of data*/
          msg_type->tai_lst_elem[tai_lst_index].tai.tai_lst2.diff_plmn[loop].tac = b_unpackw(src, *pos, num_bits_to_read);

          *pos = *pos + num_bits_to_read;
          *buffer_length = *buffer_length - num_bits_to_read;
          max_tai_bits = max_tai_bits - num_bits_to_read;
          tai_count++;
        }
        break;
    
      default:
        break;
    }/*End of switch(tai_list_type)*/

    /*----- Inrease the TAI List index -----*/
    tai_lst_index = tai_lst_index + 1; 
  }/* End of WHILE(TAI LIST LENGTH)*/
  msg_type->tai_lst_length = partial_tai_list_count;
  return TRUE;
}/*End of decode_tai_list_ie*/

/*===========================================================================

FUNCTION  DECODE_MS_IDENTITY_IE

DESCRIPTION
  This function decodes MS_IDENTITY Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_ms_identity_ie(byte *src,word *pos, word *buffer_length, gmm_ie_LV_mobile_id_T *ms_id)
{
  byte original_ms_identity_len = 0, adjusted_ms_identity_len = 0;
  byte loop = 0, num_bits_to_read = 0;
  byte ms_id_type = 0;

 /* Read MS Identity length */
  num_bits_to_read = 8;
  original_ms_identity_len = b_unpackb(src, *pos, num_bits_to_read);
  adjusted_ms_identity_len = original_ms_identity_len;
  *pos = *pos + num_bits_to_read;
  *buffer_length = *buffer_length - num_bits_to_read;

  if(adjusted_ms_identity_len > MAX_MS_IDENTITY_LEN)
    adjusted_ms_identity_len = MAX_MS_IDENTITY_LEN;
  
  if((adjusted_ms_identity_len < MIN_MS_IDENTITY_LEN) ||
     (adjusted_ms_identity_len*8 > *buffer_length))
  {
    MSG_ERROR_DS_3(MM_SUB,"MSG_LIB: MS ID len=%d, Min len=%d, Buffer len=%d in bits",
                   adjusted_ms_identity_len*8, MIN_MS_IDENTITY_LEN*8, *buffer_length);
    if(adjusted_ms_identity_len*8 > *buffer_length)
      *buffer_length = 0;
    return FALSE;
  }

  /* Skip identity digit 1 and odd/even indicator bits and read Type of Identity 
  This IE is sent from network to UE in Attach accept or TAU accept message 3GPP TS 24.301 V8.4.0 (2009-12)
  3GPP TS 24.301 V8.4.0 (2009-12) 8.2.26 Tracking area update accept, 8.2.1.4 MS identity
  This IE may be included to assign or unassign a new TMSI to a UE during a combined attach
  3GPP TS 24.301 V8.4.0 (2009-12) 8.2.26 Tracking area update accept, 8.2.26.7 MS identity
  This IE may be included to assign or unassign a new TMSI to a UE during a combined TA/LA update
  Check is restricted to MS Identity type IMSI(for unassigning) & TMSI(for assigning) for above reasons 
  Also refer to subsections 5.5.1.3.4.2 Combined attach successful & 5.5.3.3.4.2 Combined tracking area
  updating successful in 3GPP TS 24.301 V8.4.0 (2009-12)*/
  num_bits_to_read = 3;
  ms_id_type = b_unpackb(src, (word)(*pos+5), num_bits_to_read);

  num_bits_to_read = 8;
  *pos = *pos + num_bits_to_read;
  *buffer_length = *buffer_length - num_bits_to_read;
  adjusted_ms_identity_len--;
  original_ms_identity_len--;

  if(ms_id_type == LTE_NAS_EMM_IMSI)
  {
    ms_id->id_type = ms_id_type;
  }
  else if(ms_id_type == LTE_NAS_EMM_TMSI)
  {
    ms_id->id_type = ms_id_type;
  
    /* 3GPP TS 24.008 V8.9.0 (2010-03) 10.5.1.4 Mobile Identity the TMSI/P-TMSI/M-TMSI
    is 4 octets long. Populate TMSI from next 4 bytes (TMSI_SIZE) */
    num_bits_to_read = 8;
    for(loop = 0; loop < TMSI_SIZE; loop++) 
    {
      if(loop < sizeof(ms_id->value))
      {
        ms_id->value[loop] = b_unpackb(src, *pos, num_bits_to_read);
        ms_id->length++;
        *pos = *pos + num_bits_to_read;
        *buffer_length = *buffer_length - num_bits_to_read;
        adjusted_ms_identity_len--;
        original_ms_identity_len--;
      }
    }
  }
  else
  {
    MSG_ERROR_DS_1(MM_SUB,"MSG_LIB: Invalid/Unexpected MS IDENTITY type = %d",
                   ms_id_type);
    *buffer_length = 0;
    return FALSE;
  }

  /*Skip remaining bytes in the IE if any*/
  if(adjusted_ms_identity_len == original_ms_identity_len)
  {
    *pos = *pos + adjusted_ms_identity_len*8;
    *buffer_length = *buffer_length - adjusted_ms_identity_len*8;
  }
  else if(original_ms_identity_len*8 <= *buffer_length)
  {
    *pos = *pos + original_ms_identity_len*8;
    *buffer_length = *buffer_length - original_ms_identity_len*8;
  }
  else
  {
    *buffer_length = 0;
  }
  return TRUE;
}/*End of decode_ms_identity_ie*/

/*===========================================================================

FUNCTION  DECODE_GUTI_IE

DESCRIPTION
  This function decodes GUTI Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_guti_ie(byte *src,word *pos, word *buffer_length, lte_nas_emm_guti_type *guti)
{
  byte num_bits_to_read, guti_len=0,plmn_len=3, index =0, octet=0;
  byte unpackedbytes =0,mme_group_id_len = 0;
 
  /* ---- Length of GUTI ----*/
  num_bits_to_read = 8;
  guti_len = b_unpackb(src, *pos, num_bits_to_read);
  *pos = *pos + num_bits_to_read;
  *buffer_length = *buffer_length-num_bits_to_read;

  if(guti_len > EMM_GUTI_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "GUTI Length = %d is greater than 11, truncating it to 11 bytes (following length octet)",
                   guti_len);
    guti_len = EMM_GUTI_LEN;
  }
  else if(guti_len != EMM_GUTI_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "GUTI Length is %d bytes. Expected length is 11 bytes (following length octet)",
                   guti_len);
    *buffer_length = 0;
    return FALSE;
  }
  else if((guti_len*WORD_SIZE) > *buffer_length)
  {
    MSG_ERROR_DS_1(MM_SUB, "GUTI Length is %d bytes. Received GUTI length greater than buffer size, skipping message parser\n",
                   guti_len);
    *buffer_length = 0;
    return FALSE;
  }

  /* ---- Skip the first 4 bits of 3rd octet ---- */  
  num_bits_to_read = 4;
  *pos  = *pos + num_bits_to_read; /* Skip the first 4 bits of the 2nd octet*/ 
  *buffer_length = *buffer_length-num_bits_to_read;
  /* ---- Even / Odd Indication ----*/
  num_bits_to_read = 1;   
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  *pos = *pos + num_bits_to_read;
  *buffer_length = *buffer_length-num_bits_to_read;
  if(unpackedbytes !=0)
  {
    MSG_ERROR_DS_0(MM_SUB, "Even indication not received in GUTI\n");
    *pos  = *pos + guti_len;
    *buffer_length = *buffer_length - guti_len;
    return FALSE;
  }
  /* ---- Verify Type of identity ----*/
  num_bits_to_read = 3;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
  *pos = *pos + num_bits_to_read;
  *buffer_length = *buffer_length-num_bits_to_read;
  guti_len--;
  if(unpackedbytes!=(byte)GUTI_ID)
  {
    MSG_ERROR_DS_0(MM_SUB, "Type of Identity is not equal to GUTI\n");
    *pos  = *pos + guti_len;
    *buffer_length = *buffer_length - guti_len;
    return FALSE;
  }
  guti->ue_id = (ue_id_type)unpackedbytes;
  /* ----- decode plmn ----- */
  num_bits_to_read  = 8; /*decode 1 byte in plmn (in each time)*/
  for(octet =0;octet<plmn_len;octet++)
  {
    /*If the operator decides to use 2 digit MNC, then mnc digit 3 will have value '1111' (binary)*/
    byte two_digit_mnc_val = 15; /*15 = 1111 (binary)*/
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
    /*NW sets MNC digit 3 to 1111 (in case it wants to use 2 digit MNC) in June spec (this is correct)
    NW sets MNC digits 2 to 1111 (in case it wants to use 2 digit MNC) in March/ Dec spec (this is wrong)
    Have corrected the code to be compliant with the June spec (to be functionally correct)
    - though June spec is not tested now*/
    if(octet == 1)
    {
      if(((unpackedbytes & 0x0F)> MAX_BCD_VAL) || 
         (((((unpackedbytes & 0xF0)>>4) > MAX_BCD_VAL)&&((unpackedbytes & 0xF0)>>4) < two_digit_mnc_val)))
      {
        MSG_ERROR_DS_1(MM_SUB, "PLMN octet %d does not have a valid BCD value, discarding GUTI",
                       octet+1);
        return FALSE;
      }
    }
    else
    {
      if(((unpackedbytes & 0x0F)> MAX_BCD_VAL) ||(((unpackedbytes & 0xF0)>>4) > MAX_BCD_VAL))
      {
        MSG_ERROR_DS_1(MM_SUB, "PLMN octet %d does not have a valid BCD value, discarding GUTI",
                       octet+1);
        return FALSE;
      }
    }
    guti->plmn.identity[octet] = unpackedbytes;
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length-num_bits_to_read;
     guti_len--;
  }
  /* ---- MME GROUPID ---- */
  num_bits_to_read =  8;
  mme_group_id_len = 2;
  for(index = 0;index<mme_group_id_len;index++)
  {
    if(guti_len)
    {
      guti->mme_group_id[mme_group_id_len-index-1] = b_unpackb(src, *pos, num_bits_to_read);
      *pos = *pos + num_bits_to_read;
      *buffer_length = *buffer_length-num_bits_to_read;
      guti_len--;
    }
  }                            

  /* ---- MME CODE ---- */
  num_bits_to_read =  8;
  if(guti_len)
  {      
    guti->mme_code = b_unpackb(src, *pos, num_bits_to_read); 
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length-num_bits_to_read;
    guti_len--;
  }
  /* ---- M-TMSI ---- */
  octet = 0;
  num_bits_to_read =  8;
  if(guti_len != 4)
  {
    MSG_ERROR_DS_0(MM_SUB, "GUTI parsing error, skipping this IEI");
    return FALSE;     
  }
  while(guti_len)
  {
    guti->m_tmsi[guti_len-1] = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length-num_bits_to_read;
    guti_len--;
  }/*Until the remaining GUTI length, decode M-TMSI*/
  return TRUE;
}/*End of decode_guti_ie()*/
#endif /*FEATURE_LTE*/
