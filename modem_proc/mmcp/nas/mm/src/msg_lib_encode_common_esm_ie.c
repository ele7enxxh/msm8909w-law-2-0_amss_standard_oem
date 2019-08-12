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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_encode_common_esm_ie.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when         who                          what                              
--------     --------  ------------------------------------------------
02/26/10     MNK       Fix a bug in EPS_QoS(SDF_QoS) encoding
05/17/09     hnam      Added the code to support security
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "bit.h"
#include "lte_nas.h"
#include "lte_nas_common_v.h"

#include "msg_lib_encode_esm.h"
#include "msg_lib_encode_common_esm_ie.h"
#include "msg_lib_declarations.h"

#include "lte_nas_esm_message_type.h"
#include "lte_nas_esm_message_description.h"
#include "msg.h"
#include "err.h"
#include "emm_database.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define PROTOCOL_CONF_OPTIONS_IEI 0x27
#define DEVICE_PROPERTIES_IEI 0xC

/*===========================================================================

                        FUNCTION DEFINITION

===========================================================================*/

/*===========================================================================

FUNCTION  ENCODE_EPS_QoS

DESCRIPTION
  This function encodes the EPS_QoS information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void encode_eps_qos(sdf_qos_T *eps_qos, byte *ota_buf, word *pos, word *esm_container_len)
{
  byte num_bits_to_write =0, length = 1;

  /*********** Length of EPS QoS **********/
  num_bits_to_write = 8;
  b_packb((byte)eps_qos->length,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write; 
  *esm_container_len += num_bits_to_write;

  /* Based on Length, write the values into OTA buffer */
  num_bits_to_write = 8;
  while(length <= eps_qos->length)
  {
    switch(length)
    {
      case 1:
        b_packb((byte)eps_qos->qci,ota_buf,*pos,num_bits_to_write);
        break;
      case 2:
        b_packb((byte)eps_qos->max_bit_rate_ul,ota_buf,*pos,num_bits_to_write);
        break;
      case 3:
        b_packb((byte)eps_qos->max_bit_rate_dl,ota_buf,*pos,num_bits_to_write);
        break;
      case 4:
        b_packb((byte)eps_qos->gbr_ul,ota_buf,*pos,num_bits_to_write);
        break;
      case 5:
        b_packb((byte)eps_qos->gbr_dl,ota_buf,*pos,num_bits_to_write);
        break;
      case 6:
        b_packb((byte)eps_qos->max_bit_rate_ul_ex,ota_buf,*pos,num_bits_to_write);
        break;
      case 7:
        b_packb((byte)eps_qos->max_bit_rate_dl_ex,ota_buf,*pos,num_bits_to_write);
        break;
      case 8:
        b_packb((byte)eps_qos->gbr_ul_ex,ota_buf,*pos,num_bits_to_write);
        break;
      case 9:
        b_packb((byte)eps_qos->gbr_dl_ex,ota_buf,*pos,num_bits_to_write);
        break;
#ifdef FEATURE_LTE_REL11
      case 10:
        b_packb((byte)eps_qos->max_bit_rate_ul_ex2,ota_buf,*pos,num_bits_to_write);
        break;
      case 11:
        b_packb((byte)eps_qos->max_bit_rate_dl_ex2,ota_buf,*pos,num_bits_to_write);
        break;
      case 12:
        b_packb((byte)eps_qos->gbr_ul_ex2,ota_buf,*pos,num_bits_to_write);
        break;
      case 13:
        b_packb((byte)eps_qos->gbr_dl_ex2,ota_buf,*pos,num_bits_to_write);
        break;
#endif /*FEATURE_LTE_REL11*/
	  default:
        MSG_ERROR_DS_1(MM_SUB, "Invalid octet %d", length);
      break;
    } /* end switch(length) */
    length++;
    *pos += num_bits_to_write; 
    *esm_container_len += num_bits_to_write;
  } /* end while(length <= eps_qos->length) */
}

/*===========================================================================

FUNCTION  ENCODE_ESM_TFT

DESCRIPTION
  This function encodes the TFT information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void encode_esm_tft(tft_type_T *tft, byte *ota_buf, word *pos, word *esm_container_len)
{
  byte num_bits_to_write =0, loop =0, index = 0;
  word tft_length = 0;
  byte max_tft_len = 254; /*The maximum length for this IE is 254 octets*/
  byte min_pkt_filter_len = 24; /*identifier + evaluation precedence + length of contents*/
  byte min_parameter_filter_len = 16; /*identifier  + length of contents*/

  if(tft->valid == TRUE)
  {
    /*----------------------------------------
        Length of traffic flow template IE
    -----------------------------------------*/
    num_bits_to_write = 8;
    b_packb((byte)tft->tft_total_length,ota_buf,*pos,num_bits_to_write);
    if(tft->tft_total_length > max_tft_len)
    {
      MSG_ERROR_DS_1(MM_SUB, "=EMM= received TFT length = %d, truncating this to 254",
                     tft->tft_total_length);
      tft->tft_total_length = 254;
    }
    *pos += num_bits_to_write; 
    *esm_container_len += num_bits_to_write;
     
    tft_length = tft->tft_total_length * WORD_SIZE;
    /*----------------------------------------
               TFT operation code
    -----------------------------------------*/
    num_bits_to_write = 3;
    b_packb((byte)tft->operation_code,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write; 
    *esm_container_len += num_bits_to_write;
    tft_length -= num_bits_to_write;

    /*----------------------------------------
                     E bit  
    -----------------------------------------*/
    num_bits_to_write = 1;
    b_packb((byte)tft->e_bit,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write; 
    *esm_container_len += num_bits_to_write;
    tft_length -= num_bits_to_write;

    /*----------------------------------------
             Number of packet filters
    -----------------------------------------*/
    num_bits_to_write = 4;

     /*For the "delete existing TFT" operation and for the "no TFT operation",the 
      number of packet filters shall be coded as 0. For all other operations,the 
      number of packet filters shall be greater than 0 and less than or equal to 16*/

    if((tft->operation_code == 2) || (tft->operation_code == 6)) 
    {      
      if(tft->num_filters != 0)
      {
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= TFT OP Code = %d, set NUMBER_OF_PKT_FILTERS to 0",
                       tft->operation_code);
        tft->num_filters = 0;
      }
    }
    else if((tft->operation_code == 1)||(tft->operation_code == 3)|| 
            (tft->operation_code == 4)|| (tft->operation_code == 5))
    {
      if(tft->num_filters > 16)
      {        
        MSG_ERROR_DS_1(MM_SUB, "=NAS_MSG_LIB= Received TFT filters num = %d is greater than Max allowed 16. Truncating to max allowed",
                       tft->num_filters);
        tft->num_filters = 16;         
      }
      b_packb((byte)tft->num_filters,ota_buf,*pos,num_bits_to_write);
      *pos += num_bits_to_write; 
      *esm_container_len += num_bits_to_write;
      tft_length -= num_bits_to_write;
    }
    else
    {
      MSG_ERROR_DS_0(MM_SUB, "=EMM= received invalid Operation Code for encoding");
    }                        

    switch(tft->operation_code)
    {
    case 1: /*Create new TFT*/
    case 3: /*Add packet filters to existing TFT*/
    case 4: /*Replace packet filters in existing TFT*/    
      /*----------------------------------------
                 Packet filter list  
      -----------------------------------------*/
      num_bits_to_write = 8;
      for(loop = 0; loop < tft->num_filters &&
                    loop < MAX_PACKET_FILTERS; loop++)
      {
        if(tft_length < (min_pkt_filter_len + (tft->filter_list[loop].content_length * 8)))
        { 
          MSG_ERROR_DS_0(MM_SUB, "=EMM= received TFT length does not match with the parameters in TFT, skipping the rest of TFT encoding");
          return;
        }

        /*-- Packet filter identifier --*/
        b_packb((byte)tft->filter_list[loop].identifier,ota_buf,*pos,num_bits_to_write);
        *pos += num_bits_to_write; 
        *esm_container_len += num_bits_to_write;
        tft_length -= num_bits_to_write;
         
        /*-- Packet filter evaluation precedence --*/ 
        b_packb((byte)tft->filter_list[loop].eval_precedence,ota_buf,*pos,num_bits_to_write);
        *pos += num_bits_to_write; 
        *esm_container_len += num_bits_to_write;
        tft_length -= num_bits_to_write;

        /*-- Length of Packet filter contents --*/
        b_packb((byte)tft->filter_list[loop].content_length,ota_buf,*pos,num_bits_to_write);
        *pos += num_bits_to_write; 
        *esm_container_len += num_bits_to_write;
        tft_length -= num_bits_to_write;

        for(index=0;index < tft->filter_list[loop].content_length; index++)
        {
          /*-- Packet filter contents --*/
          b_packb((byte)tft->filter_list[loop].filter_contents[index],ota_buf,*pos,num_bits_to_write);
          *pos += num_bits_to_write; 
          *esm_container_len += num_bits_to_write;
          tft_length -= num_bits_to_write;
        }
      }
      break;
    case 5: /*Delete Packet filter from existing TFT*/
      num_bits_to_write = 8;
      if(tft->num_filters != 0)
      {
        MSG_ERROR_DS_0(MM_SUB, "=EMM= received DELETE_PKT_FILTER_FROM_EXISTING_TFT w/o pkt filter");
      }
      for(loop = 0; loop < tft->num_filters &&
                    loop < MAX_PACKET_FILTERS; loop++)
      {
        /*-- Packet filter identifier --*/
        b_packb((byte)tft->filter_list[loop].identifier,ota_buf,*pos,num_bits_to_write);
        *pos += num_bits_to_write; 
        *esm_container_len += num_bits_to_write;
        tft_length -= num_bits_to_write;
      }      
      break;
    case 6: /*NO_TFT*/    
      MSG_HIGH_DS_0(MM_SUB, "MSG_LIB: No TFT Packet Filter");
      break;
    default:
      MSG_FATAL_DS(MM_SUB,"=EMM= received invalid TFT Operation code",0,0,0);
      break;
    }

    if(tft->e_bit)
    {
      /*----------------------------------------
                   Parameters list  
      -----------------------------------------*/
      loop = 0;

      num_bits_to_write = 8;
      while(tft_length > (3*sizeof(byte))) /*If length is greater than 3 octets*/
      {
        if(tft_length < (min_parameter_filter_len + (tft->param_list[loop].content_length * 8)))
        {
          MSG_ERROR_DS_0(MM_SUB, "=EMM= TFT length does not match with the contents length, skipping TF encoding");
          return;
        }
        /* -- Parameter identifier --*/
        b_packb((byte)tft->param_list[loop].identifier,ota_buf,*pos,num_bits_to_write);
        *pos += num_bits_to_write; 
        *esm_container_len += num_bits_to_write;
        tft_length -= num_bits_to_write;

        /*-- Length of Parameter filter contents --*/
        b_packb((byte)tft->param_list[loop].content_length,ota_buf,*pos,num_bits_to_write);
        *pos += num_bits_to_write; 
        *esm_container_len += num_bits_to_write;
        tft_length -= num_bits_to_write;

        if(tft_length > tft->param_list[loop].content_length)
        {
          for(index=0;index < tft->param_list[loop].content_length; index++)
          {
            if(tft_length != 0)
            {
              /* -- Parameter contents --*/
              b_packb((byte)tft->param_list[loop].filter_contents[index],ota_buf,*pos,num_bits_to_write);
              *pos += num_bits_to_write; 
              *esm_container_len += num_bits_to_write;
              tft_length -= num_bits_to_write;
            }
            else
            { 
              MSG_ERROR_DS_0(MM_SUB, "=EMM= there was an error while encoding TFT PARAMETER LIST, skipping the rest of TFT encoding");
              return;
            }
          }
        }
        loop++;
      }  
    }
  }
}

/*------------------------------------------------
       PROTOCOL CONFIGURATION OPTIONS
--------------------------------------------------*/
/*===========================================================================

FUNCTION  ENCODE_PROT_CONF_OPT

DESCRIPTION
  This function encodes the PROTOCOL_CONFIGURATION_OPTIONS information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_prot_conf_opt(protocol_cfg_options_T *prot_conf_opt, byte *ota_buf, word *pos, word *esm_container_len)
{
  byte num_bits_to_write =0, loop =0;
  word protocol_index = 0, prot_len =0;
  word prot_len_offset = 0;
  
  /* ----- Protocol Conf Opt IEI ---- */
  if(prot_conf_opt->valid == TRUE)
  {
    if( prot_conf_opt->num_protocol_ids > NUM_PROTOCOL_ID )
    {
      MSG_ERROR_DS_2(MM_SUB,"MSG_LIB: Number of protocol ids = %d. Max allowed = %d. Skipping PCO encoding.",
          prot_conf_opt->num_protocol_ids, NUM_PROTOCOL_ID);
      prot_conf_opt->valid = FALSE;
      return;
    }

    for(loop=0;loop<(prot_conf_opt->num_protocol_ids);loop++)
    {
      if( prot_conf_opt->procol_info[loop].protocol_len > MAX_PROTOCOL_ID_CONTENT )
      {
        MSG_ERROR_DS_3(MM_SUB,"MSG_LIB: Number of protocol contents bytes in protocol id #%d = %d. Max allowed = %d. Skipping PCO encoding.",
                       (loop + 1),
                       prot_conf_opt->procol_info[loop].protocol_len,
                       MAX_PROTOCOL_ID_CONTENT);
        prot_conf_opt->valid = FALSE;
        return;
      }
    }

    num_bits_to_write = 8;
    b_packb((byte)PROTOCOL_CONF_OPTIONS_IEI,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write; 
    *esm_container_len += 1;

    /* ---- Length of protocol Configuration options ---- */
    prot_len_offset = *pos;
    *pos += num_bits_to_write; 
    *esm_container_len += 1;
 
    /* ---- 1 bit (one) for padding ---- */
    num_bits_to_write = 1;
    b_packb((byte)1,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write; 
  
    /* ---- 4 bits (zeroes) for padding ---- */
    num_bits_to_write = 4;
    b_packb((byte)0,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write; 

    /* ---- Configuration protocol ---- */
    num_bits_to_write = 3;
    b_packb(prot_conf_opt->config_protocol,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write; 

     prot_len++;

    *esm_container_len += 1;

    /* ---- Protocol IDs ---- */
    num_bits_to_write = 8;

    for(loop=0;loop<(prot_conf_opt->num_protocol_ids);loop++)
    { 
      /* ---- protocol ID[0] ---- */
      b_packb(prot_conf_opt->procol_info[loop].protocol_id[0],ota_buf,*pos,num_bits_to_write);  
      *pos += num_bits_to_write; 
      *esm_container_len += 1;
      prot_len++;

      /* ---- protocol ID[1] ---- */
      b_packb(prot_conf_opt->procol_info[loop].protocol_id[1],ota_buf,*pos,num_bits_to_write);  
      *pos += num_bits_to_write; 
      *esm_container_len += 1;
      prot_len++;

      /* ---- protocol ID Length ---- */
      b_packb(prot_conf_opt->procol_info[loop].protocol_len,ota_buf,*pos,num_bits_to_write);  
      *pos += num_bits_to_write; 
      *esm_container_len += 1;
       prot_len++;
      /* ---- Protocol ID Contents ---- */
      for(protocol_index = 0;protocol_index <(prot_conf_opt->procol_info[loop].protocol_len);protocol_index++)
      {
        b_packb(prot_conf_opt->procol_info[loop].protocol_contents[protocol_index],ota_buf,*pos,num_bits_to_write);  
        *pos += num_bits_to_write; 
        *esm_container_len += 1;
        prot_len++;
      }/*End of FOR Loop() ....writing protocol ID Contents*/
    }/*End of FOR Loop() ....writing protocol IDs*/
    /*------ WRITE THE LENGTH OF THE OFFSET ----*/
    num_bits_to_write = 8;
    b_packw(prot_len,ota_buf,prot_len_offset,num_bits_to_write);  
  }
}

#ifdef FEATURE_LTE_REL10
/*===========================================================================

FUNCTION  ENCODE_ESM_DEVICE_PROPERTIES_IE

DESCRIPTION
  This function encodes the DEVICE PROPERTIES information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_esm_device_properties_ie
(
  boolean *low_priority_indicator,
  byte *ota_buf,
  word *pos,
  word *esm_container_len
)
{
  byte num_bits_to_write = 4;

  ASSERT(low_priority_indicator != NULL);
  ASSERT(ota_buf != NULL);
  ASSERT(pos != NULL);
  ASSERT(esm_container_len != NULL);

  /* IEI value */
  b_packb(DEVICE_PROPERTIES_IEI, ota_buf, *pos, num_bits_to_write);
  *pos += num_bits_to_write;

  /* 3 spare bits */
  num_bits_to_write = 3;
  b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;

  /* low priority indicator */
  num_bits_to_write = 1;
  b_packb(*low_priority_indicator, ota_buf, *pos, num_bits_to_write);
  *pos += num_bits_to_write;

  *esm_container_len += 1;
} /* end encode_esm_device_properties_ie() */
#endif

#endif /*FEATURE_LTE*/
