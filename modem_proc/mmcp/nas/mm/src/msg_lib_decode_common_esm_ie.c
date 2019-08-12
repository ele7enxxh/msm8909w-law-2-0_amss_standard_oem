

/*==============================================================================

==============================================================================*/

/*Author: hnam
  Reference Document: 3GPP TS 24.301 V1.1.1 (2008-10)
  START DATE: 2/15/2008*/
/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_decode_common_esm_ie.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when         who                          what                              
--------     --------  ------------------------------------------------
09/17/12       cogolt  Rel-10, Added function decode_t3396_value
06/22/09       hnam    Changed the MAX_QOS_LEN from 14 to 16
05/18/09       hnam    Fixed the code to decode sdf qos
05/17/09       hnam    Added the code to support security
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "bit.h"
#include "lte_nas.h"
#include "lte_nas_common_v.h"

#include "msg_lib_encode_esm.h"
#include "msg_lib_decode_common_esm_ie.h"

#include "lte_nas_esm_message_type.h"
#include "lte_nas_esm_message_description.h"
#include "msg_lib_declarations.h"
#include "msg_lib_validation.h"
#include "msg.h"
#include "err.h"
#include "sm_v.h"
#include "gmm_int_v.h"
#include "emm_database.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define MAX_APN_ADDR_LEN 100
#define MIN_APN_ADDR_LEN 1
#define MIN_PROT_CONG_OPT_LEN 1
#define MAX_PROT_CONG_OPT_LEN 251
#define MIN_TRANSACTION_ID_LEN 1
#define MAX_TRANSACTION_ID_LEN 2

#define MAX_PROT_CONF_OPT_LEN 251
#define MIN_PROT_CONF_OPT_LEN 1

#define MIN_APN_AMBR_LEN 2
#define MAX_APN_AMBR_LEN 6

#define RESERVED_TFT_OPN_CODE 7

#define CREATE_NEW_TFT 1
#define DELETE_PKT_FLTR_IN_EXISTING_TFT 5
#define ADD_PKT_FLTR_IN_EXISTING_TFT 3
#define REPLACE_PKT_FLTR_IN_EXISTING_TFT 4


#define MAX_PDN_ADDR_LEN 13
#define MAX_PCO_LEN 253

#ifdef FEATURE_LTE_REL10
#define MAX_T3396_VALUE_LEN 1
#endif
/*===========================================================================

                        FUNCTION DEFINITION

===========================================================================*/
/*===========================================================================

FUNCTION  DECODE_TFT

DESCRIPTION
  This function decodes TFT Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_tft(byte *src,word *pos, word *buffer_length, tft_type_T *tft,word *esm_msg_len)
{
  byte num_bits_to_read = 0,unpackedbytes = 0, id = 0, pkt_content_index  = 0;
  word tft_len = 0; 
  byte param_content_index =0;

  /*=========================================================
           Length of TFT
  ==========================================================*/

  num_bits_to_read  = 8;
  tft->tft_total_length =  b_unpackb(src, *pos, num_bits_to_read);
  *pos = * pos + num_bits_to_read;
  *buffer_length  =  *buffer_length - num_bits_to_read;
  *esm_msg_len = *esm_msg_len - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  if((*buffer_length/8) < tft->tft_total_length)
  {
    esm_length_check_failed = TRUE;
    MSG_ERROR_DS_2(MM_SUB, "tft buffer_length %d is less than decoded tft len %d ",
                   *buffer_length/8, tft->tft_total_length);
    return FALSE;
  }
#endif

  tft_len = tft->tft_total_length;

  if(!tft->tft_total_length)
  {
    MSG_ERROR_DS_0(MM_SUB, "Received TFT length  = %d, length varies between 1-255, truncating this optional IE and all IEs following this\n");
    esm_length_check_failed = TRUE;
    *buffer_length  = 0;
    *esm_msg_len = 0;
    return FALSE;
  }

  /*=======================================================================
      Based on the length of EPS QoS, decode the other fields (in TFT)
  ========================================================================*/  
  /*=========================================================
                    TFT operation code
  ==========================================================*/
  num_bits_to_read  = 3;
  unpackedbytes =  b_unpackb(src, *pos, num_bits_to_read);
  if(unpackedbytes == RESERVED_TFT_OPN_CODE)
  {
    MSG_ERROR_DS_0(MM_SUB, "received TFT operation code value 7 that is reserved, skipping message parser\n");
    return FALSE;
  }
  tft->operation_code  = unpackedbytes;
  *pos = * pos + num_bits_to_read;
  *buffer_length  =  *buffer_length - num_bits_to_read;
  *esm_msg_len = *esm_msg_len - num_bits_to_read;
  /*=========================================================
                           E bit
  ==========================================================*/
  num_bits_to_read  = 1;
  tft->e_bit  =  b_unpackb(src, *pos, num_bits_to_read);
  *pos = * pos + num_bits_to_read;
  *buffer_length  =  *buffer_length - num_bits_to_read;
  *esm_msg_len = *esm_msg_len - num_bits_to_read;
  /*=========================================================
                 Number of packet filters
  ==========================================================*/
  num_bits_to_read  = 4;
  tft->num_filters  =  b_unpackb(src, *pos, num_bits_to_read);
  *pos = * pos + num_bits_to_read;
  *buffer_length  =  *buffer_length - num_bits_to_read;
  *esm_msg_len = *esm_msg_len - num_bits_to_read;
  tft_len--;
  /*=========================================================
                      Packet filters
  ==========================================================*/
  num_bits_to_read  = 8;

  if((tft->num_filters == 0) || (tft->num_filters > MAX_PACKET_FILTERS))
  {
    /* Stop further decoding and skip the remaining bytes in the IE */
    *pos = *pos + (tft_len * 8);
    *buffer_length  =  *buffer_length - (tft_len * 8);
    *esm_msg_len = *esm_msg_len - (tft_len * 8);
    return TRUE; 
  }
  
  if(tft->operation_code == DELETE_PKT_FLTR_IN_EXISTING_TFT)
  {
    if(tft_len < tft->num_filters)
    {
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient TFT length = %d to decode %d packet filters",
                     tft_len, tft->num_filters);
      return FALSE;
    }
    for(id =0; id < tft->num_filters; id++)
    {
      tft->filter_list[id].identifier = b_unpackb(src, *pos, num_bits_to_read);
      *pos = * pos + num_bits_to_read;
      *buffer_length  =  *buffer_length - num_bits_to_read;
      *esm_msg_len = *esm_msg_len - num_bits_to_read;
      tft_len--;      
    }        
  }
  else if((tft->operation_code == CREATE_NEW_TFT)||
          (tft->operation_code == ADD_PKT_FLTR_IN_EXISTING_TFT)||
          (tft->operation_code == REPLACE_PKT_FLTR_IN_EXISTING_TFT))
  {
    for(id =0; id < tft->num_filters && 
                    id<MAX_PACKET_FILTERS; id++)
    {
      if(tft_len < 3)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient TFT length = %d to decode TFT packet filterlist [%d]",
                       tft_len, id);
        return FALSE;
      }
      tft->filter_list[id].identifier = b_unpackb(src, *pos, num_bits_to_read);
      *pos = * pos + num_bits_to_read;
      *buffer_length  =  *buffer_length - num_bits_to_read;
      *esm_msg_len = *esm_msg_len - num_bits_to_read;
      tft_len--;
      tft->filter_list[id].eval_precedence = b_unpackb(src, *pos, num_bits_to_read);
      *pos = * pos + num_bits_to_read;
      *buffer_length  =  *buffer_length - num_bits_to_read;
      *esm_msg_len = *esm_msg_len - num_bits_to_read;
      tft_len--;
      /*Content Length*/
      tft->filter_list[id].content_length = b_unpackb(src, *pos, num_bits_to_read);
      *pos = * pos + num_bits_to_read;
      *buffer_length  =  *buffer_length - num_bits_to_read;
      *esm_msg_len = *esm_msg_len - num_bits_to_read;
      tft_len--;
      /*=========================================================
                     Packet filter contents
      ==========================================================*/      
      if(tft_len < tft->filter_list[id].content_length)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient TFT length = %d to decode TFT packet filterlist [%d] contents",
                       tft_len, id);
        return FALSE;
      }
      for(pkt_content_index = 0;pkt_content_index < tft->filter_list[id].content_length &&
                                pkt_content_index < MAX_FILTER_CONTENT_LENGTH; pkt_content_index++)
      {
        tft->filter_list[id].filter_contents[pkt_content_index] = b_unpackb(src, *pos, num_bits_to_read);
        *pos = * pos + num_bits_to_read;
        *buffer_length  =  *buffer_length - num_bits_to_read;
        *esm_msg_len = *esm_msg_len - num_bits_to_read;
        tft_len--;
      }
    } 
  }
  /*=========================================================
                   Parameter filters
  ==========================================================*/
  if(tft_len > 0)
  {
    for(id =0; id < MAX_PACKET_FILTERS; id++)
    {
      if(tft_len < 2)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient TFT length = %d to decode TFT paramlist [%d]",
                       tft_len, id);
        return FALSE;
      }
      tft->param_list[id].identifier = b_unpackb(src, *pos, num_bits_to_read);
      *pos = * pos + num_bits_to_read;
      *buffer_length  =  *buffer_length - num_bits_to_read;
      *esm_msg_len = *esm_msg_len - num_bits_to_read;
      tft_len--;
      tft->param_list[id].content_length = b_unpackb(src, *pos, num_bits_to_read);
      *pos = * pos + num_bits_to_read;
      *buffer_length  =  *buffer_length - num_bits_to_read;
      *esm_msg_len = *esm_msg_len - num_bits_to_read;
      tft_len--;
      /*=========================================================
                      Parameter filter contents
      ==========================================================*/      
      if(tft_len < tft->param_list[id].content_length)
      {
        MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient TFT length = %d to decode TFT paramlist[%d] filter contents",
                       tft_len, id);
        return FALSE;
      }
      for(param_content_index = 0;param_content_index < tft->param_list[id].content_length &&
                                  param_content_index < MAX_FILTER_CONTENT_LENGTH; param_content_index++)
      {
        tft->param_list[id].filter_contents[param_content_index] = b_unpackb(src, *pos, num_bits_to_read);
        *pos = * pos + num_bits_to_read;
        *buffer_length  =  *buffer_length - num_bits_to_read;
        *esm_msg_len = *esm_msg_len - num_bits_to_read;
        tft_len--;
      }
    }
  }
  return TRUE;
}

/*===========================================================================
FUNCTION      DECODE_EPS_QoS

DESCRIPTION
  This function decodes EPS QoS Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
boolean decode_eps_qos
(
  byte      *src,
  word      *pos,
  word      *buffer_length,
  sdf_qos_T *eps_qos,
  word      *esm_msg_len
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0, octet = 0;
  byte ie_length = 0;
  boolean exit_for_loop = FALSE;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  word length_to_skip = 0;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(src != NULL);
  ASSERT(pos != NULL);
  ASSERT(buffer_length != NULL);
  ASSERT(eps_qos != NULL);
  ASSERT(esm_msg_len != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL);


  /***** Length of EPS QoS IE *****/
  num_bits_to_read = 8;
  ie_length = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of EPS QoS IE decode the other fields*/
  if(ie_length > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_length);
    esm_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_length < MIN_EPS_QOS_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received EPS QoS IE len %d < %d Min allowed",
                   ie_length, MIN_EPS_QOS_LENGTH);
    return FALSE;
  }

  /* Initialize EPS QoS length to 0 and increment the length upon decoding each octet */
  eps_qos->length = 0;
  num_bits_to_read = 8;

  for(octet = 1; octet <= ie_length; octet++)
  {
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

    switch(octet)
    {
      case 1: /***** QoS Class ID *****/
        if(((unpackedbytes >= 10) &&
            (unpackedbytes <= 127))
           ||
           (unpackedbytes == 255))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= QCI value = %d is reserved", unpackedbytes);
          return FALSE;
        }
        eps_qos->qci = unpackedbytes;
        break;

      case 2: /***** Max bit rate of uplink *****/
        if(unpackedbytes >= (byte)ONE_KBPS)
        {
          eps_qos->max_bit_rate_ul = unpackedbytes;
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Maximum bit rate for UL = %d. Valid range = 1-255",
                         unpackedbytes);
          return FALSE;
        }
        break;

      case 3: /***** Max bit rate of downlink *****/
        if(unpackedbytes >= (byte)ONE_KBPS)
        {
          eps_qos->max_bit_rate_dl = unpackedbytes;
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Maximum bit rate for DL = %d. Valid range = 1-255",
                         unpackedbytes);
          return FALSE;
        }
        break;

      case 4: /***** Guaranteed bit rate for uplink *****/
        if(unpackedbytes >= (byte)ONE_KBPS)
        {
          eps_qos->gbr_ul = unpackedbytes;
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Guaranteed bit rate for UL = %d. Valid range = 1-255",
                         unpackedbytes);
          return FALSE;
        }

      case 5: /***** Guaranteed bit rate for downlink *****/
        if(unpackedbytes >= (byte)ONE_KBPS)
        {
          eps_qos->gbr_dl = unpackedbytes;
        }
        else
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Guaranteed bit rate for DL = %d. valid range = 1-255",
                         unpackedbytes);
          return FALSE;
        }
        break;

      case 6: /***** Maximum bit rate for uplink (extended) *****/
        eps_qos->max_bit_rate_ul_ex = unpackedbytes;
        break;

      case 7: /***** Maximum bit rate for downlink (extended) *****/
        eps_qos->max_bit_rate_dl_ex = unpackedbytes;
        break;

      case 8: /***** Guaranteed bit rate for uplink (extended) *****/
        eps_qos->gbr_ul_ex = unpackedbytes;
        break;

      case 9: /***** Guaranteed bit rate for downlink (extended) *****/
        eps_qos->gbr_dl_ex = unpackedbytes;
        break;

#ifdef FEATURE_LTE_REL11
      case 10: /***** Maximum bit rate for uplink (extended-2) *****/
        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
        {
          eps_qos->max_bit_rate_ul_ex2 = unpackedbytes;
        }
        else
        {
          /* Octets from Max bit rate for uplink (extended-2) onwards are only supported in spec
             versions from rel-11 onwards. Therefore stop decoding further and skip the remaining
             octets in EPS QoS IE */
          length_to_skip = ie_length - eps_qos->length;
          exit_for_loop = TRUE;
        }
        break;

      case 11: /***** Maximum bit rate for downlink (extended-2) *****/
        eps_qos->max_bit_rate_dl_ex2 = unpackedbytes;
        break;

      case 12: /***** Guaranteed bit rate for uplink (extended-2) *****/
        eps_qos->gbr_ul_ex2 = unpackedbytes;
        break;

      case 13: /***** Guaranteed bit rate for downlink (extended-2) *****/
        eps_qos->gbr_dl_ex2 = unpackedbytes;
        break;
#endif /*FEATURE_LTE_REL11*/

      default:
        length_to_skip = ie_length - eps_qos->length;
        exit_for_loop = TRUE;
        break;
    } /* end switch(octet) */

    if(exit_for_loop == TRUE)
    {
      break;
    }

    eps_qos->length++;
    ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_msg_len);
  } /* end for loop */

  /* Discard additional octets in EPS QoS IE */  
  if(length_to_skip > 0)
  {
    MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in EPS QoS IE. 3gpp_spec_ver = %d",
                  length_to_skip, emm_ctrl_data_ptr->emm_3gpp_spec_ver);
    ADJUST_OFFSET_AND_BUF_LEN(length_to_skip*8, pos, buffer_length, esm_msg_len);
  }
  
  return TRUE;
} /* end decode_eps_qos() */


/*===========================================================================

FUNCTION  DECODE_APN_AMBR

DESCRIPTION
  This function decodes APN_AMBR Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_apn_addr(byte *src,word *pos, word *buffer_length, apn_T *apn_addr,word *esm_len_in_bits)
{
  byte num_bits_to_read = 0, octet =0;

  /*=========================================================
                   Length of APN Address
  ==========================================================*/
  num_bits_to_read  = 8;
  apn_addr->apn_addr_len =  b_unpackb(src, *pos, num_bits_to_read);
  *pos = *pos + num_bits_to_read; 
  *buffer_length = *buffer_length-num_bits_to_read;
  *esm_len_in_bits = *esm_len_in_bits - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  if((*buffer_length/8) < apn_addr->apn_addr_len)
  {
    esm_length_check_failed = TRUE;
	MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), apn_addr->apn_addr_len);
    return FALSE;
  }
#endif

  /*===================================================================================
     Based on the length of APN Address, decode the other fields (in APN Address)
  ====================================================================================*/
  if((apn_addr->apn_addr_len)> MAX_APN_ADDR_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "APN Address length is %d, maximum APN address length (after length octet)is 100",
                   apn_addr->apn_addr_len);
    esm_length_check_failed = TRUE;
    return FALSE;
  }
  else if ((apn_addr->apn_addr_len)< MIN_APN_ADDR_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "APN Address length is %d, minimum APN address length (after length octet) is 1",
                   apn_addr->apn_addr_len);
    esm_length_check_failed = TRUE;
    return FALSE;
  }
  /*Decode access point address*/
  for(octet = 0;octet<(apn_addr->apn_addr_len); octet++)
  {                
    apn_addr->address[octet] =  b_unpackb(src, *pos, num_bits_to_read);
    MSG_HIGH_DS_2(MM_SUB, "APN Address value[%d] = %d\n", octet, apn_addr->address[octet]);
    *pos = *pos + num_bits_to_read;
    *buffer_length = *buffer_length-num_bits_to_read;
    *esm_len_in_bits = *esm_len_in_bits - num_bits_to_read;
  }/*End of for()*/
  apn_addr->valid = TRUE;
  return TRUE;
}/*End of decode_apn_address()*/

/*===========================================================================

FUNCTION  DECODE_TRANSACTION_ID

DESCRIPTION
  This function decodes TRANSACTION_ID Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_transaction_id(byte *src,word *pos, word *buffer_length, gsm_umts_transaction_id_T *ti,word *esm_len_in_bits)
{ 
  byte num_bits_to_read,len =0;

  /*-------------------------------------
    Transaction identifier
  --------------------------------------*/
  /*--------------- Length of transaction ID ------------*/
  num_bits_to_read = 8;     
  ti->length_of_transaction_id = b_unpackb(src, *pos, num_bits_to_read);
  len = ti->length_of_transaction_id;
   MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ti->length_of_transaction_id);
  *pos = *pos + num_bits_to_read; 
  *buffer_length = *buffer_length-num_bits_to_read; 
  *esm_len_in_bits  = *esm_len_in_bits - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  if((*buffer_length/8) < len)
  {
    esm_length_check_failed = TRUE;
    MSG_ERROR_DS_2(MM_SUB, "trans_id buffer_length %d is less than decoded len %d ",
                   *buffer_length/8, len);
    return FALSE;
  }
#endif

  if(len<MIN_TRANSACTION_ID_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "Transaction ID length is %d, minimum required length (after length octet)is 1\n",
                   len);
    esm_length_check_failed = TRUE;
    return FALSE;
  }
  if(len>MAX_TRANSACTION_ID_LEN)
  { 
    MSG_ERROR_DS_1(MM_SUB, "Transaction ID length is %d, maximum required length(after length octet)is 2\n",
                   len);
    esm_length_check_failed = TRUE;
    return FALSE;
  }
  else
  {
    /*-------------- Transaction ID Flag -------------*/
    num_bits_to_read = 1;
    ti->ti_flag = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read; 
    *buffer_length = *buffer_length-num_bits_to_read; 
    *esm_len_in_bits  = *esm_len_in_bits - num_bits_to_read;
    /*-------------- Transaction ID Value 0 -------------*/
    num_bits_to_read = 3; 
    ti->val_of_transaction_id[0] = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read; 
    *buffer_length = *buffer_length-num_bits_to_read; 
    *esm_len_in_bits  = *esm_len_in_bits - num_bits_to_read;
    /*-----------Skip 4 bits (0000) +  1 extended bit -----------*/
    num_bits_to_read = 4;
    *pos = *pos + num_bits_to_read; 
    *buffer_length = *buffer_length-num_bits_to_read; 
    *esm_len_in_bits  = *esm_len_in_bits - num_bits_to_read;
    len--;
  }
  if(len)
  {
    /* ------------- 1 extended bit ---------- */
    num_bits_to_read = 1;
    *pos = *pos + num_bits_to_read; 
    *buffer_length = *buffer_length-num_bits_to_read; 
    *esm_len_in_bits  = *esm_len_in_bits - num_bits_to_read;
    /*-------------- Transaction ID Value 1 -------------*/
    num_bits_to_read = 7; 
    ti->val_of_transaction_id[1] = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read; 
    *buffer_length = *buffer_length-num_bits_to_read; 
    *esm_len_in_bits  = *esm_len_in_bits - num_bits_to_read;
    len--;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION  DECODE_PROT_CONF_OPT

DESCRIPTION
  This function decodes PROTOCOL_CONFIGURATION_OPTIONS Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_prot_conf_opt(byte *src,word *pos, word *buffer_length, protocol_cfg_options_T *prot_conf_opt,word *esm_msg_len)
{
  byte num_bits_to_read, prot_conf_loop;
  word prot_conf_opt_len =0, index =0;
  word original_buffer_length = 0, extra_length = 0;

  /*--------------------------- Length of Protocol Conf Options----------------*/
  num_bits_to_read  = 8;
  prot_conf_opt_len = b_unpackb(src, *pos, num_bits_to_read);   
  *pos = *pos + num_bits_to_read;              
  *buffer_length = *buffer_length-num_bits_to_read;
  *esm_msg_len = *esm_msg_len - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  if((*buffer_length/8) < prot_conf_opt_len)
  {
    esm_length_check_failed = TRUE;
	 MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), prot_conf_opt_len);
    return FALSE;
  }
#endif

  if(prot_conf_opt_len<MIN_PROT_CONG_OPT_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "Protocol Configuration options Length is %d, minimum Protocol Length required is 3",
                   prot_conf_opt_len);
    esm_length_check_failed = TRUE;
    return FALSE;
  }
  if(prot_conf_opt_len>MAX_PROT_CONF_OPT_LEN)
  {
    extra_length = prot_conf_opt_len - MAX_PROT_CONF_OPT_LEN;
    MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received %d extra octets in PCO IE", extra_length);
    prot_conf_opt_len = MAX_PROT_CONF_OPT_LEN;
  }

  /*--------------------------- Configuration Protocol----------------*/
  /*------ Skip spare bit and 4 reserved bits -------*/
  num_bits_to_read  = 5;
  *pos = *pos + num_bits_to_read;              
  *buffer_length = *buffer_length-num_bits_to_read;
  *esm_msg_len = *esm_msg_len - num_bits_to_read;
  num_bits_to_read  = 3;
  prot_conf_opt->config_protocol = b_unpackb(src, *pos, num_bits_to_read); 
  *pos = *pos + num_bits_to_read;              
  *buffer_length = *buffer_length-num_bits_to_read;
  *esm_msg_len = *esm_msg_len - num_bits_to_read;
  prot_conf_opt_len--; /*Subtract 1 byte from the total number of bytes*/

  original_buffer_length = *buffer_length;

  while((prot_conf_opt_len > 0) && (index < NUM_PROTOCOL_ID) )
  {

    if(*buffer_length > original_buffer_length)
    {
      esm_length_check_failed = TRUE;
      MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Buffer overflow:  original_buffer_length = %d, buffer_length = %d",
                             original_buffer_length/8, (*buffer_length/8));
      return FALSE;
    }

    /***** Protocol ID *****/
    num_bits_to_read = 8;
    prot_conf_opt->procol_info[index].protocol_id[0] = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;              
    *buffer_length = *buffer_length - num_bits_to_read;
    *esm_msg_len = *esm_msg_len - num_bits_to_read;
    prot_conf_opt_len--;
     
    prot_conf_opt->procol_info[index].protocol_id[1] = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;              
    *buffer_length = *buffer_length - num_bits_to_read;
    *esm_msg_len = *esm_msg_len - num_bits_to_read;
    prot_conf_opt_len--;

    /***** Protocol ID length *****/
    prot_conf_opt->procol_info[index].protocol_len = b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;              
    *buffer_length = *buffer_length - num_bits_to_read;
    *esm_msg_len = *esm_msg_len - num_bits_to_read;
    prot_conf_opt_len--;

    /***** Protocol ID contents *****/
    for(prot_conf_loop = 0;prot_conf_loop < (prot_conf_opt->procol_info[index].protocol_len) &&
                         (prot_conf_loop < MAX_PROTOCOL_ID_CONTENT); prot_conf_loop++)
    {
      if(prot_conf_opt_len <= (*buffer_length/8))
      {
        prot_conf_opt->procol_info[index].protocol_contents[prot_conf_loop] =
                                            b_unpackb(src, *pos, num_bits_to_read);
        *pos = *pos + num_bits_to_read;              
        *buffer_length = *buffer_length - num_bits_to_read;
        *esm_msg_len = *esm_msg_len - num_bits_to_read;
        prot_conf_opt_len--;
      }
      else
      {
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Insufficient PCO Length to decode Protocol ID Contents for Protocol ID %d",
                       index+1);
        esm_length_check_failed = TRUE;
        return FALSE;
      }
    } /* end protocol ID contents for loop */
    index++;
    prot_conf_opt->num_protocol_ids++;
  } /* end while(prot_conf_opt_len) */

  /* Skip remaining PCO length if any plus any extra octets in PCO IE */
  if((prot_conf_opt_len + extra_length) > 0)
  {
    MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in PCO IE",
                  (prot_conf_opt_len + extra_length));
    *pos = *pos + (prot_conf_opt_len + extra_length)*8;              
    *buffer_length = *buffer_length - (prot_conf_opt_len + extra_length)*8;
    *esm_msg_len = *esm_msg_len - (prot_conf_opt_len + extra_length)*8;
  }
  return TRUE;  
}/*End of decode*/

/*===========================================================================

FUNCTION  DECODE_APN_AMBR

DESCRIPTION
  This function decodes APN_AMB Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/        
boolean decode_apn_ambr(byte *src,word *pos, word *buffer_length, apn_ambr_T *apn_ambr,word *esm_msg_len)
{
  byte num_bits_to_read,len=0, unpackedbytes =0;
  
  /*--------------------------------------------------
                 Length of APN-AMBR
  ---------------------------------------------------*/
  num_bits_to_read  = 8;
  len = b_unpackb(src, *pos, num_bits_to_read);   
  *pos = *pos + num_bits_to_read;              
  *buffer_length = *buffer_length-num_bits_to_read;
  *esm_msg_len = *esm_msg_len - num_bits_to_read;

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  if((*buffer_length/8) < len)
  {
    esm_length_check_failed = TRUE;
    MSG_ERROR_DS_2(MM_SUB, "apn_ambr buffer_length %d is less than decoded len %d ",
                   *buffer_length/8, len);
    return FALSE;
  }
#endif

  if(len<MIN_APN_AMBR_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "APN-AMBR length is %d, minimum Length required (after Length octet)is 2",
                   len);
    esm_length_check_failed = TRUE;
    return FALSE;
  }
  if(len>MAX_APN_AMBR_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "APN-AMBR length is %d, maximum permissible Length (after Length octet) is 6\n",
                   len);
    esm_length_check_failed = TRUE;
    return FALSE;
  }

  /*----------------------------------------------
                Max Bit Rate Downlink
  ----------------------------------------------*/
  num_bits_to_read  = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);     
  apn_ambr->apn_ambr_dl =  unpackedbytes;    
  *pos = *pos + num_bits_to_read;              
  *buffer_length = *buffer_length-num_bits_to_read;
  *esm_msg_len = *esm_msg_len - num_bits_to_read;
  len--;  
  /*----------------------------------------------
                Max Bit Rate Uplink
  ----------------------------------------------*/
  num_bits_to_read  = 8;
  unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);   
  apn_ambr->apn_ambr_ul =  unpackedbytes;  
  *pos = *pos + num_bits_to_read;              
  *buffer_length = *buffer_length-num_bits_to_read;
  *esm_msg_len = *esm_msg_len - num_bits_to_read;
  len--;  
  if(len)
  {
    /*----------------------------------------------
                Max Bit Rate Down link (extended)
    ----------------------------------------------*/
    num_bits_to_read  = 8;
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);
    if(unpackedbytes > MAX_APN_AMBR_EXT_UL_DL)
    {
      MSG_ERROR_DS_1(MM_SUB, "Ext downlink max bit rate is %d, value should be in the range 0x00 to 0xFA, setting the value to 0xFA",
                     unpackedbytes);
      unpackedbytes = MAX_APN_AMBR_EXT_UL_DL; /*Section 9.9.4.2.1 - APN-AMBR for downlink (extended)  - 
                                              All other values shall be interpreted as '1 1 1 1 1 0 1 0'*/
    }
    apn_ambr->apn_ambr_dl_ext =  unpackedbytes;    
    *pos = *pos + num_bits_to_read;              
    *buffer_length = *buffer_length-num_bits_to_read;
    *esm_msg_len = *esm_msg_len - num_bits_to_read;
    len--;
  }    
  if(len)
  {
    /*----------------------------------------------
            Max Bit Rate Up link (extended)
    ----------------------------------------------*/
    num_bits_to_read  = 8;
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read); 
    if(unpackedbytes > MAX_APN_AMBR_EXT_UL_DL)
    {
      MSG_ERROR_DS_1(MM_SUB, "Ext uplink max bit rate is %d, value should be in the range 0x00 to 0xFA, setting the value to 0xFA",
                     unpackedbytes);
      unpackedbytes = MAX_APN_AMBR_EXT_UL_DL; /*Section 9.9.4.2.1 - APN-AMBR for uplink (extended)  - 
                                               The coding is identical to that of the APN-AMBR for downlink (extended)*/
    }
    apn_ambr->apn_ambr_ul_ext =  unpackedbytes;    
    *pos = *pos + num_bits_to_read;              
    *buffer_length = *buffer_length-num_bits_to_read;
    *esm_msg_len = *esm_msg_len - num_bits_to_read;
    len--;
  }/*End of Max bit rate down link (extended)*/  
  if(len)
  {
    /*----------------------------------------------
          Max Bit Rate Downlink (extended-2)
    ----------------------------------------------*/
    num_bits_to_read  = 8;
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);     
    if(unpackedbytes > MAX_APN_AMBR_EXT2_UL_DL)
    {
      MSG_ERROR_DS_1(MM_SUB, "Ext2 downlink max bit rate is %d, value should be in the range 0-254, this value is set to '0'",
                     unpackedbytes);
      unpackedbytes = 0x00; /*Section 9.9.4.2.1 - APN-AMBR for downlink (extended-2)  - 
                            "1 1 1 1 1 1 1 1" This value shall be interpreted as '0 0 0 0 0 0 0 0' 
                             in this version of the specification.*/
    }
    apn_ambr->apn_ambr_dl_ext2 =  unpackedbytes;    
    *pos = *pos + num_bits_to_read;              
    *buffer_length = *buffer_length-num_bits_to_read;
    *esm_msg_len = *esm_msg_len - num_bits_to_read;
    len--;
  }/*End of Max bit rate downlink (extended 2)*/  
  if(len)
  {
    /*----------------------------------------------
            Max Bit Rate up link (extended 2)
    ----------------------------------------------*/
    num_bits_to_read  = 8;
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);     
    if(unpackedbytes > MAX_APN_AMBR_EXT2_UL_DL)
    {
      MSG_ERROR_DS_1(MM_SUB, "Ext downlink max bit rate is %d, value should be in the range 0-254, this value is set to '0'",
                     unpackedbytes);
      unpackedbytes = 0x00; /*APN-AMBR for uplink (extended-2)
                            The coding is identical to that of the APN-AMBR for downlink (extended-2)*/
    }
    apn_ambr->apn_ambr_ul_ext2 =  unpackedbytes;    
    *pos = *pos + num_bits_to_read;              
    *buffer_length = *buffer_length-num_bits_to_read;
    *esm_msg_len = *esm_msg_len - num_bits_to_read;
    len--;
  }/*End of Max bit rate down link (extended)*/  
  return TRUE;
}

/*===========================================================================
FUNCTION      DECODE_QoS

DESCRIPTION
  This function decodes QoS Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/ 
boolean decode_qos
(
  byte  *src,
  word  *pos,
  word  *buffer_length,
  qos_T *neg_qos,
  word  *esm_msg_len
)
{
  byte ie_length = 0,num_bits_to_read = 0, unpackedbytes = 0;
  byte octet = 0, neg_qos_length = 0, value = 0;
  boolean exit_for_loop = FALSE;
  word length_to_skip = 0;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
 
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(src != NULL);
  ASSERT(pos != NULL);
  ASSERT(buffer_length != NULL);
  ASSERT(neg_qos != NULL);
  ASSERT(esm_msg_len != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL);

  
  /************ Length of QoS IE ************/
  num_bits_to_read = 8;
  ie_length = b_unpackb(src, *pos, num_bits_to_read);
  ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_msg_len);

  /* Based on the length of QoS IE decode the other fields */
  if(ie_length > (*buffer_length/8))
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), ie_length);
    esm_length_check_failed = TRUE;
    return FALSE;
  }

  if(ie_length < MIN_QOS_LENGTH)
  {
    MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Received QoS IE len %d < %d Min allowed",
                   ie_length, MIN_QOS_LENGTH);
    return FALSE;
  }

  /* Increment Negotiated QoS length upon decoding each octet */
  for(octet = 1; octet <= ie_length; octet++)
  {
    unpackedbytes = b_unpackb(src, *pos, num_bits_to_read);

    switch(octet)
    {
      case 1: /***** 2 Spare bits + 3 Delay class bits + 3 Reliability class bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |    0         0    |<------- Delay class ------->|<--- Reliability class ---->|
                |<----- spare ----->|                             |                            | */

        /* Extract Delay class value */
        value = ((unpackedbytes & 0x38) >> 3);
        if((value == 0) || (value == 7))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Delay class = %d is reserved. Valid range = 1-6", value);
          return FALSE;
        }
        neg_qos->delay_class = value;

        /* Extract Reliability class value */
        value = (unpackedbytes & 0x7);
        if((value == 0) || (value == 7))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Reliability class = %d is reserved. Valid range = 1-6", value);
          return FALSE;
        }
        neg_qos->reliability_class = value;
        break;

      case 2: /***** 4 Peak throughput bits + 1 Spare bit + 3 Precedence class bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |<---------- Peak throughput ---------->|    0    |<---- Precedence class ---->|
                |                                       |<-spare->|                            | */
  
        /* Extract Peak throughput value */
        value = ((unpackedbytes & 0xF0) >> 4);
        if((value == 0) || (value == 15))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Peak throughput = %d is reserved. Valid range = 1-14",
                         value);
          return FALSE;
        }
        neg_qos->peak_throughput = value;
  
        /* Extract Precedence class value */
        value = (unpackedbytes & 0x7);
        if((value == 0) || (value == 7))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Precedence class = %d is reserved. Valid range = 1-6", value);
          return FALSE;
        }
        neg_qos->precedence_class = value;
        break;

      case 3: /***** 3 Spare bits + 5 Mean throughput bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |    0    |    0    |    0    |<--------------  Mean throughput -------------->|
                |<---------- spare ---------->|                                                | */
  
        /* Extract Mean throughput value */
        value = (unpackedbytes & 0x1F);
        if((value == 0) || (value == 30))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Mean throughput = %d is reserved. Valid range = 1-29 or 31",
                         value);
          return FALSE;
        }
        neg_qos->mean_throughput = value;
        break;
  
      case 4: /***** 3 Traffic class bits + 2 Delivery order bits + 3 Delivery of erroneous SDU bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |<------ Traffic class ------>|<- Delivery order->|<-Delivery of erroneous SDU->| */
  
        /* Extract Traffic class value */
        value = ((unpackedbytes & 0xE0) >> 5);
        if((value == 0) || ((value >= 5) && (value <= 7)))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Traffic class = %d is reserved. Valid range = 1-4",
                         value);
          return FALSE;
        }
        neg_qos->traffic_class = value;
        /* Extract Delivery order value */
        value = ((unpackedbytes & 0x18) >> 3);
        if((value == 0) || (value == 3))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Delivery order = %d is reserved. Valid range = 1-2",
                         value);
          return FALSE;
        }
        neg_qos->deliver_order = value;

        /* Extract Delivery of erroneous SDU value */
        value = (unpackedbytes & 0x7);
        if((value == 0) || ((value >= 4) && (value <= 7)))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Delivery of erroneous SDU = %d is reserved. Valid range = 1-3",
                         value);
          return FALSE;
        }
        neg_qos->deliver_err_sdu = value;
        break;
  
      case 5: /***** Maximum SDU size *****/
        if((unpackedbytes == 0) || (unpackedbytes > 153))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Maximum SDU size = %d is reserved. Valid range = 1-153",
                         unpackedbytes);
          return FALSE;
        }
        neg_qos->max_sdu_size = unpackedbytes;
        break;

      case 6: /***** Maximum bit rate for uplink *****/
        if(unpackedbytes == 0)
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Maximum bit rate for uplink = %d is reserved. Valid range = 1-255",
                         unpackedbytes);
          return FALSE;
        }
        neg_qos->max_bit_rare_uplink = unpackedbytes;
        break;
  
      case 7: /***** Maximum bit rate for downlink *****/
        if(unpackedbytes == 0)
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Maximum bit rate for downlink = %d is reserved. Valid range = 1-255",
                         unpackedbytes);
          return FALSE;
        }
        neg_qos->max_bit_rare_downlink = unpackedbytes;
        break;

      case 8: /***** 4 Residual BER bits + 4 SDU error ratio bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |<----------- Residual BER ------------>|<---------- SDU error ratio --------->| */
  
        /* Extract Residual BER value */
        value = ((unpackedbytes & 0xF0) >> 4);
        if((value == 0) || ((value >= 10) && (value <= 15)))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Residual BER = %d is reserved. Valid range = 1-9",
                         value);
          return FALSE;
        }
        neg_qos->residual_ber = value;

        /* Extract SDU error ratio value */
        value = (unpackedbytes & 0xF);
        if((value == 0) || ((value >= 8) && (value <= 15)))
        {
          MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= SDU error ratio = %d is reserved. Valid range = 1-7",
                         value);
          return FALSE;
        }
        neg_qos->sdu_err_ratio = value;
        break;

      case 9: /***** 6 Transfer delay bits + 2 Traffic handling priority bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |<-------------------- Transfer delay --------------------->|<---- Traffic --->
                |                                                            |      handling   | 
                |                                                            |      priority   | */
  
        /* Extract Transfer delay value.
           The Transfer delay value is ignored if the Traffic Class is Interactive class
           or Background class. */
        if((neg_qos->traffic_class != INTERACTIVE_CLASS) &&
           (neg_qos->traffic_class != BACKGROUND_CLASS))
        {
          value = ((unpackedbytes & 0xFC) >> 2);
          if((value == 0) || (value == 63))
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Transfer delay = %d is reserved. Valid range = 1-62",
                           value);
            return FALSE;
          }
          neg_qos->tranfer_delay = value;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Transfer delay is ignored because traffic class is %d",
                       neg_qos->traffic_class);
        }

        /* Extract Traffic handling priority value.
           The Traffic handling priority value is ignored if the Traffic Class is
           Conversational class, Streaming class or Background class. */
        if((neg_qos->traffic_class != CONVERSATION_CLASS) &&
           (neg_qos->traffic_class != STREAMING_CLASS) &&
           (neg_qos->traffic_class != BACKGROUND_CLASS))
        {
          value = (unpackedbytes & 0x3);
          if(value == 0)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Traffic handling priority = %d is reserved. Valid range = 1-3",
                           value);
            return FALSE;
          }
          neg_qos->traffic_priority = value;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Traffic handling priority is ignored because traffic class is %d",
                       neg_qos->traffic_class);
        }
        break;

      case 10: /***** Guaranteed bit rate for uplink *****/
        /* The Guaranteed bit rate for uplink value is ignored if the Traffic Class
           is Interactive class or Background class */
        if((neg_qos->traffic_class != INTERACTIVE_CLASS) &&
           (neg_qos->traffic_class != BACKGROUND_CLASS))
        {
          if(unpackedbytes == 0)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Guaranteed bit rate for uplink = %d is reserved. Valid range = 1-255",
                           unpackedbytes);
            return FALSE;
          }
          neg_qos->guarantee_ber_uplink = unpackedbytes;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Guaranteed bit rate for uplink is ignored because traffic class is %d",
                       neg_qos->traffic_class);
        }
        break;

      case 11: /***** Guaranteed bit rate for downlink *****/
        /* The Guaranteed bit rate for downlink value is ignored if the Traffic Class
           is Interactive class or Background class */
        if((neg_qos->traffic_class != INTERACTIVE_CLASS) &&
           (neg_qos->traffic_class != BACKGROUND_CLASS))
        {
          if(unpackedbytes == 0)
          {
            MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Guaranteed bit rate for downlink = %d is reserved. Valid range = 1-255",
                           unpackedbytes);
            return FALSE;
          }
          neg_qos->guarantee_ber_dwnlink = unpackedbytes;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Guaranteed bit rate for downlink is ignored because traffic class is %d",
                       neg_qos->traffic_class);
        }
        break;

      case 12: /***** 3 Spare bits + 1 Signalling indication bit + 4 Source statistics descriptor bits *****/
        /* bits:|    8    |    7    |    6    |    5    |    4    |    3    |    2    |   1    |
                |    0    |    0    |    0    |<-Signa->|<--- Source statistics descriptor --->|
                |<---------- spare ---------->|lling in |                                      |
                |                             |dication |                                      | */

        /* Extract Signalling indication value.
           The Signalling Indication value is ignored if the Traffic Class is Conversational class,
           Streaming class or Background class */
        if((neg_qos->traffic_class != CONVERSATION_CLASS) &&
           (neg_qos->traffic_class != STREAMING_CLASS) &&
           (neg_qos->traffic_class != BACKGROUND_CLASS))
        {
          neg_qos->sig_ind = ((unpackedbytes & 0x10) >> 4);
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB, "=NAS_MSG_LIB= Signalling indication is ignored because traffic class is %d",
                       neg_qos->traffic_class);
        }
  
        /***** Source statistics descriptor *****/
        /* In network to MS direction Source statistics descriptor bits are spare so skip it*/
        break;

      case 13: /***** Maximum bit rate for downlink(extended) *****/
        neg_qos->ext_max_bit_rate_downlink = unpackedbytes;
        break;

      case 14: /* Guaranteed bit rate for downlink(extended) */
        neg_qos->ext_gtd_bit_rate_downlink = unpackedbytes;
        break;

      case 15: /***** Maximum bit rate for uplink(extended) *****/
        neg_qos->ext_max_bit_rate_uplink = unpackedbytes;
        break;

      case 16: /***** Guaranteed bit rate for uplink(extended) *****/
        neg_qos->ext_gtd_bit_rate_uplink = unpackedbytes;
        break;

#ifdef FEATURE_LTE_REL11
      case 17: /***** Maximum bit rate for downlink(extended-2) *****/
        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
        {
          neg_qos->ext2_max_bit_rate_downlink = b_unpackb(src, *pos, num_bits_to_read);
        }
        else
        {
          /* Octets from Max bit rate for downlink(extended-2) onwards are only supported in spec
             versions from rel-11 onwards. Therefore stop decoding further and skip the remaining
             octets in QoS IE */
          length_to_skip = ie_length - neg_qos_length;
          exit_for_loop = TRUE;
        }
        break;

      case 18: /***** Guaranteed bit rate for downlink(extended-2) *****/
        neg_qos->ext2_gtd_bit_rate_downlink = b_unpackb(src, *pos, num_bits_to_read);
        break;

      case 19: /***** Maximum bit rate for uplink(extended-2) *****/
          neg_qos->ext2_max_bit_rate_uplink = b_unpackb(src, *pos, num_bits_to_read);
        break;

      case 20: /***** Guaranteed bit rate for uplink(extended-2) *****/
        neg_qos->ext2_gtd_bit_rate_uplink = b_unpackb(src, *pos, num_bits_to_read);
        break;
#endif /*FEATURE_LTE_REL11*/
      default:
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Unsupported Octet #%d (after length field)", octet);
        length_to_skip = ie_length - neg_qos_length;
        exit_for_loop = TRUE;
        break;
    } /* end switch(octet) */

    if(exit_for_loop == TRUE)
    {
      break;
    }

    neg_qos_length++;
    ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read, pos, buffer_length, esm_msg_len);
  } /* end for loop */

  /* Discard additional octets in QoS IE */
  if(length_to_skip > 0)
  {
    MSG_HIGH_DS_2(MM_SUB, "=NAS_MSG_LIB= Discarding additional %d octets in QoS IE. 3gpp_spec_ver = %d",
                  length_to_skip, emm_ctrl_data_ptr->emm_3gpp_spec_ver);
    ADJUST_OFFSET_AND_BUF_LEN(length_to_skip*8, pos, buffer_length, esm_msg_len);
  }

  return TRUE;
} /* end decode_qos()*/

/*===========================================================================

FUNCTION  DECODE_PDN_ADRESS

DESCRIPTION
  This function decodes PDN_ADDRESS Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
boolean decode_pdn_address(byte *src, word *pos, word *buffer_length,pdn_address_T *pdn_address, word *esm_msg_len)
{
  byte num_bits_to_read =0;
  byte loop =0, pdn_addr_len =0;  

  num_bits_to_read = 8; 
  pdn_addr_len = b_unpackb(src, *pos, num_bits_to_read); 
  pdn_address->pdn_addr_len = pdn_addr_len;
  *pos = *pos + num_bits_to_read;              
  *buffer_length = *buffer_length-num_bits_to_read;
  *esm_msg_len = *esm_msg_len - num_bits_to_read; 

#ifdef FEATURE_LTE_NAS_DEEP_SEC
  if((*buffer_length/8) < pdn_addr_len)
  {
    esm_length_check_failed = TRUE;
	MSG_ERROR_DS_2(MM_SUB, "=NAS_MSG_LIB= Insufficient buffer length = %d, ie_len = %d",
                   (*buffer_length/8), pdn_address->pdn_addr_len);
    return FALSE;
  }
#endif

  if(pdn_addr_len>MAX_PDN_ADDR_LEN)
  {
    MSG_ERROR_DS_1(MM_SUB, "PDN address length is %d, max length is %d (following the length octet)",
                   MAX_PDN_ADDR_LEN);
    esm_length_check_failed = TRUE;
    return FALSE;    
  }
  if(pdn_addr_len != 0 && 
     pdn_addr_len <= (*esm_msg_len/8))
  {
    /*--- Skip 5 spare bits ---*/
    num_bits_to_read = 5;
    *pos = *pos + num_bits_to_read;              
    *buffer_length = *buffer_length-num_bits_to_read;
    *esm_msg_len = *esm_msg_len - num_bits_to_read; 
    
    /* ---- PDN Type --- */
    num_bits_to_read = 3;
    pdn_address->pdn_type_val =(pdn_type_T)b_unpackb(src, *pos, num_bits_to_read);
    *pos = *pos + num_bits_to_read;              
    *buffer_length = *buffer_length-num_bits_to_read;
    *esm_msg_len = *esm_msg_len - num_bits_to_read;     
    pdn_addr_len--; /*Subtract one byte (5 spare bits + 3 bits for PDN Type)*/    
    /* ---- PDN Value --- */
    num_bits_to_read = 8;  
    for(loop = 0;loop <pdn_addr_len;loop++)
    {
      pdn_address->address[loop] = b_unpackb(src, *pos, num_bits_to_read); 
      *pos = *pos + num_bits_to_read;              
      *buffer_length = *buffer_length-num_bits_to_read;
      *esm_msg_len   = *esm_msg_len - num_bits_to_read;     
    }
  }
  else
  {
    MSG_ERROR_DS_2(MM_SUB,"Len of PDN Address %d, available esm buffer %d",
                   pdn_addr_len,(*esm_msg_len/8));
    esm_length_check_failed = TRUE;
    return FALSE;
  }
  return TRUE; 
}

#ifdef FEATURE_LTE_REL10
/*===========================================================================
FUNCTION  DECODE_T3396_VALUE

DESCRIPTION
  This function decodes T3396 value Information Element

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Decoding succeeded
  FALSE: Decoding failed

SIDE EFFECTS
  None
===========================================================================*/
boolean decode_t3396_value
(
  byte *src,
  word *pos,
  word *buffer_length,
  timer_3gpp_type3 *backoff_timer,
  word *esm_len_in_bits
)
{
  byte num_bits_to_read = 0, unpackedbytes = 0;
  byte ie_length = 0;
  
  num_bits_to_read = 8;
          
  if(*buffer_length >= num_bits_to_read)
  {
    ie_length = b_unpackb(src, *pos, num_bits_to_read); /* Read IE length */
    ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);

    if((ie_length == 0) || (ie_length > MAX_T3396_VALUE_LEN))
    {
      MSG_ERROR_DS_1(MM_SUB, "NAS_MSG_LIB: T3396 Timer length received = %d byte(s). Expected = 1 byte.\n",
                     ie_length);
    }

    num_bits_to_read = 8;
    if(ie_length >= MAX_T3396_VALUE_LEN) /* Read only 1 byte to decode T3396 timer unit and value */
    {
      if(*buffer_length >= num_bits_to_read)
      {
        unpackedbytes = b_unpackb(src, *pos, num_bits_to_read); /* Read the Timer octet */
        ADJUST_OFFSET_AND_BUF_LEN(num_bits_to_read,pos,buffer_length,esm_len_in_bits);

        if((unpackedbytes >> 5) == 7) /* Timer is deactivated */
        {
          backoff_timer->active = FALSE;
        }
        else /* Timer is active */
        {
          backoff_timer->active = TRUE;
        }

        backoff_timer->timer_count = gmm_convert_timer3_value_ie_to_msecs(unpackedbytes);
        /* Skip remaining bits in the IE if needed */
        if(ie_length > MAX_T3396_VALUE_LEN)
        {
          ADJUST_OFFSET_AND_BUF_LEN((ie_length - MAX_T3396_VALUE_LEN)*8,
                                       pos,buffer_length,esm_len_in_bits);
        }
      }
      else
      {
        MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
        *buffer_length = 0;
        return FALSE;
      }
    }
    else
    {
      *buffer_length = 0;
      return FALSE;
    }
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "Insufficient buffer length");
    *buffer_length = 0;
    return FALSE;
  }
  return TRUE;
} /*decode_t3396_value()*/
#endif

#endif /*FEATURE_LTE*/
