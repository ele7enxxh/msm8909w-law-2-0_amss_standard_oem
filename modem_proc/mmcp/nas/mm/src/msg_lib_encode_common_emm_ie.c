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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_encode_common_emm_ie.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when          who                          what                              
--------     --------  ------------------------------------------------
08/01/11      rajesh   Added function to encode eps bearer context status.
05/17/09      hnam     Added the code to support security
04/29/09      hnam     Corrected the bit value for even/odd indicator in IMSI
04/10/09      hnam     Added encode_hi_lo_priority_classmark to encode hi priority classmark
04/07/09      hnam     Fix m_tmsi and guti's mme group id encoding and decoding
02/03/09      hnam     Remove the header file msg_lib_encode_emm.h temporarily
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "bit.h"
#include "lte_nas.h"
#include "lte_nas_common_v.h"

#include "msg_lib_encode_common_emm_ie.h"

#include "lte_nas_emm_message_type.h"
#include "lte_nas_emm_message_description.h"
#include "msg.h"
#include "err.h"
#include "msg_lib_declarations.h"
#include "emm_database.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define EPS_BEARER_IDENTITY_IEI 0x57

#ifdef FEATURE_LTE_REL10
#define MS_NW_FEATURE_SUPPORT 0xC
#define DEVICE_PROPERTIES_IEI 0xD
#endif


/*===========================================================================

                        FUNCTION DEFINITION

===========================================================================*/

/*===========================================================================

FUNCTION  ENCODE_UE_NW_CAPABILITY_IE

DESCRIPTION
  This function encodes the UE NW Capability IE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_ue_nw_capability_ie(lte_nas_emm_ue_capability_type *ue_capability,word *pos,byte *ota_buf)
{
  byte num_bits_to_write = 8;
  byte len = 0, octet = 0;
#ifdef FEATURE_LTE_REL9
  emm_ctrl_data_type        *emm_ctrl_data_ptr;
#endif

  ASSERT(ue_capability != NULL);

#ifdef FEATURE_LTE_REL9   
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif

  /*Calculating the length of this IEI*/
  len = MINIMUM_UE_NW_CAP_IE_LEN + 
          ue_capability->ue_sec_capability.umts_enc_exists + 
          ue_capability->ue_sec_capability.umts_int_exists;
   /*
   ** Set lenght to 5 if onexsrvcc capability bit is set or REL10 NV is set(ACC-CSFB is set when rel10 flag)
   ** or REL9 NV is set  with either notification or lpp or lcs supported bit set.
   */
  if((ue_capability->onexsrvcc_capability_supported == TRUE)
#ifdef FEATURE_LTE_REL10
     ||
     (emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
#endif
#ifdef FEATURE_LTE_REL9
     ||
     ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9) &&
      (TRUE == ue_capability->notification_proc_supported ||
       emm_ctrl_data_ptr->lpp_supported == TRUE || 
       emm_ctrl_data_ptr->lcs_supported == TRUE ))
#endif
     )
  {
    len = 5;
  }

  b_packb((byte)len,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write;

  num_bits_to_write = 1;
  /*-----------------------------------------------------
       Write all the UE Capabilities bit by bit
  -----------------------------------------------------*/
  for(octet = 1; octet <= len; octet ++)
  {
    switch(octet)
    {
     case 1:
      b_packb(ue_capability->ue_sec_capability.eps_encrypt_alg_128_eea0_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_encrypt_alg_128_eea1_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_encrypt_alg_128_eea2_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_encrypt_alg_eea3_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_encrypt_alg_eea4_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_encrypt_alg_eea5_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_encrypt_alg_eea6_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_encrypt_alg_eea7_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
      
      break;

     case 2:
      b_packb(ue_capability->ue_sec_capability.spare_bit,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_integrity_alg_128_eia1_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_integrity_alg_128_eia2_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_integrity_alg_128_eia3_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_integrity_alg_128_eia4_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_integrity_alg_128_eia5_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_integrity_alg_128_eia6_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.eps_integrity_alg_128_eia7_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
      
        break;

      case 3:
      b_packb(ue_capability->ue_sec_capability.umts_encrypt_alg_uea0_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_encrypt_alg_uea1_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_encrypt_alg_uea2_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_encrypt_alg_uea3_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_encrypt_alg_uea4_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_encrypt_alg_uea5_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_encrypt_alg_uea6_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_encrypt_alg_uea7_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
      break;

      case 4:
      b_packb(ue_capability->ue_sec_capability.ucs2_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_integrity_alg_uia1_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_integrity_alg_uia2_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_integrity_alg_uia3_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_integrity_alg_uia4_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_integrity_alg_uia5_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_integrity_alg_uia6_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;

      b_packb(ue_capability->ue_sec_capability.umts_integrity_alg_uia7_supported,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
      break;

      case 5:
      num_bits_to_write = 3;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
#ifdef FEATURE_LTE_REL9   
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
      {   
        num_bits_to_write =1;
#ifdef FEATURE_LTE_REL10
         /*ACC-CSFB*/
        if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
        {
          b_packb(1,ota_buf,*pos,num_bits_to_write); 
        }
        else
#endif 
        {
          b_packb(0,ota_buf,*pos,num_bits_to_write); 
        }
        *pos += num_bits_to_write;
        /*LPP*/
        b_packb(ue_capability->lte_positioning_prot_supported,ota_buf,*pos,num_bits_to_write); 
        *pos += num_bits_to_write;
        /*LCS*/
        b_packb(ue_capability->lcs_notification_supported,ota_buf,*pos,num_bits_to_write); 
        *pos += num_bits_to_write;
        /*1xSRVCC*/
        b_packb(ue_capability->onexsrvcc_capability_supported,ota_buf,*pos,num_bits_to_write); 
        *pos += num_bits_to_write;
        /*NF*/
        b_packb(ue_capability->notification_proc_supported,ota_buf,*pos,num_bits_to_write); 
        *pos += num_bits_to_write;
      }
      else
#endif
      {
        num_bits_to_write =1;
        b_packb(0,ota_buf,*pos,num_bits_to_write);  /*ACC-CSFB*/
        *pos += num_bits_to_write;
  
        num_bits_to_write = 2;
        b_packb((byte)0,ota_buf,*pos,num_bits_to_write); /*LPP & LCS*/
        *pos += num_bits_to_write;
  
        num_bits_to_write =1;
  
        b_packb(ue_capability->onexsrvcc_capability_supported,ota_buf,*pos,num_bits_to_write); 
        *pos += num_bits_to_write;
  
        b_packb(ue_capability->isr_supported,ota_buf,*pos,num_bits_to_write); 
        *pos += num_bits_to_write;
      }
      break;

      case 6:
      num_bits_to_write = 8;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
      break;
    }
  }
}

/*===========================================================================

FUNCTION  ENCODE_UE_NW_CAP_IE

DESCRIPTION
  This function encodes the UE NW Capability IE, acts as a wrapper function for MM, 
  as MM expects offset increments in bytes

DEPENDENCIES
  None

RETURN VALUE
  UE_NW_CAPABILITY value part

SIDE EFFECTS
  None

===========================================================================*/
void encode_ue_nw_cap_ie(lte_nas_emm_ue_capability_type *ue_capability,byte *msg_index,byte *ota_buf)
{
  word offset_in_bits = 0;

  ASSERT(ue_capability != NULL);

  offset_in_bits = ((*msg_index) * 8);
  encode_ue_nw_capability_ie(ue_capability,&offset_in_bits,ota_buf);
  *msg_index = offset_in_bits/8;

}
/*===========================================================================

FUNCTION  ENCODE_IMSI_IE

DESCRIPTION
  This function encodes the IMSI information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_imsi_ie(lte_nas_emm_imsi_type *imsi,word *pos,byte *ota_buf)
{
  word imsi_offset = 0;
  byte num_bits_to_write = 0;
  byte imsi_len = 0;
  byte loop=0;

  /* ----- EPS Mobility Length (encoded below) ---- */
  imsi_offset = *pos;
  *pos += 8;

  /* ---- IMSI Length ---- */  

  if(imsi->num_id_digits_len)
  {  
    /* ---- Identity digit 0 ---- */
    num_bits_to_write = 4;
    b_packw(imsi->num_id_digits[0],ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;
    /* ---- Even or Odd Indicator ---- */
    num_bits_to_write = 1;

    if((imsi->num_id_digits_len)%2)
    {
      b_packb(1,ota_buf,*pos,num_bits_to_write); 
    }
    else
    {
      b_packb(0,ota_buf,*pos,num_bits_to_write); 
    }

    *pos += num_bits_to_write;

    /* ---- Type of Identity  ---- */
    num_bits_to_write = 3;
    b_packw((byte)IMSI_ID,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;    
    imsi_len++;
    /* ---- Identity digits except digit 0 ---- */
    if(imsi->num_id_digits_len <= MAX_NO_OF_IMSI)
    {
      for(loop =1;loop<(imsi->num_id_digits_len);)
      {
        num_bits_to_write = 4;
        b_packb(imsi->num_id_digits[loop],ota_buf,(word)(*pos+4),num_bits_to_write);
        loop++;
        if(loop == imsi->num_id_digits_len)
        { 
          break;  
        }
        b_packb(imsi->num_id_digits[loop],ota_buf,*pos,num_bits_to_write);
        *pos += 2*num_bits_to_write; 
        loop++;
        imsi_len++;
      }/*End of For*/
    }
    else
    {
      MSG_ERROR_DS_0(MM_SUB, "Number of digits is greater than 21, exiting encoding\n");
      msg_lib_encode_error = TRUE;
      return;
    }
  }/*End of IF*/
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "Number of Identity digits is less than minimum length (minimum number of identity digits is 1)");
    msg_lib_encode_error = TRUE;
    return;
  }

  if(!((imsi->num_id_digits_len)%2))
  {
    num_bits_to_write = 4;
    b_packw((byte)0xF,ota_buf,*pos,num_bits_to_write);
    *pos += 2*num_bits_to_write;
    imsi_len++;
  }
   
  num_bits_to_write = 8;
  b_packb(imsi_len,ota_buf,imsi_offset,num_bits_to_write); 

}

/*===========================================================================

FUNCTION  ENCODE_IMEI_IE

DESCRIPTION
  This function encodes the IMEI information element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_imei_ie
(
  lte_nas_emm_imei_type  *imei_ptr,
  word                   *pos,
  byte                   *ota_buf
)
{
  byte  index = 0;

  b_packb(imei_ptr->length, ota_buf, *pos, 8);  
  *pos += 8;    

  /* --- IMEI Value (array of 8 bytes) --- */
  for(index = 0; index < imei_ptr->length; index++)
  {
    b_packb(imei_ptr->value[index], ota_buf, *pos, 8);  
    *pos += 8;
  } 
}/* encode_imei_ie */

/*===========================================================================

FUNCTION  ENCODE_GUTI_IE

DESCRIPTION
  This function encodes the GUTI information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_guti_ie(lte_nas_emm_guti_type *guti,word *pos, byte *ota_buf)
{
  byte guti_len = 0, len =0, num_bits_to_write =0;
  byte plmn_len =3, loop =0, index=0;
  word eps_mobility_len_offset =0;

  /*------ Length of GUTI -----*/
  num_bits_to_write = 8; 
  eps_mobility_len_offset = *pos;
  *pos += num_bits_to_write;

  /* ----- IDENTITY DIGITS (1111) ---- */

  num_bits_to_write = 4;
  b_packb((byte)15,ota_buf,*pos,num_bits_to_write);
  *pos  += num_bits_to_write;
   
  /* ----  Even OR Odd Indicator ---- */
  num_bits_to_write = 1;
  b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 
   
  /* ---- Type of identity ----- */
  num_bits_to_write = 3;
  b_packb((byte)6,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write;

  guti_len = 1; /* Octet 3 is encoded*/

  /* ---- PLMN ---- */
  plmn_len =  sizeof(guti->plmn.identity);
  num_bits_to_write = 8;

  for(loop =0;loop<plmn_len;loop++)
  {
    b_packw(guti->plmn.identity[loop],ota_buf,*pos,num_bits_to_write);    
    *pos += num_bits_to_write; 
  }/*End of FOR*/

  guti_len += plmn_len;   

  /* ----- MME Group ID ----- */
  len = sizeof(guti->mme_group_id);
  
  num_bits_to_write =8;
  
  for(index =0;index< len;index++)
  {
    b_packb(guti->mme_group_id[len-index-1],ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write; 
  }
  guti_len += len;
   
  num_bits_to_write = 8;
  /* ----- MME CODE ----- */
  
  b_packb(guti->mme_code,ota_buf,*pos,num_bits_to_write);           
  *pos += num_bits_to_write;
  guti_len += 1;
              
  /* ----- M-TMSI -----*/
  len =  sizeof(guti->m_tmsi);

  num_bits_to_write = 8;               
  for(loop=0;loop< len;loop++)
  {
    b_packb(guti->m_tmsi[len-loop-1],ota_buf,*pos,num_bits_to_write);    
    *pos += num_bits_to_write; 
  }             
 
  guti_len = 11;  

  b_packb(guti_len,ota_buf,eps_mobility_len_offset,num_bits_to_write);  

}/* End of encode_guti_ie()*/
/*===========================================================================

FUNCTION  ENCODE_EMM_SUPPORTED_CODECS_IE

DESCRIPTION
  This function encodes the EMM_SUPPORTED_CODECS information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_emm_supported_codecs(cm_supported_codecs_list_T *emm_supported_codecs, byte *ota_buf, word *pos)
{
  byte index =0;
  byte num_bits_to_write = 8;
  
  /*Write the length of Supported codecs*/ 
  /*Length of IEI = number of Codecs * size of 1 codec*/
  b_packb((byte)((emm_supported_codecs->length)*sizeof(codec_bitmap_list_T)),ota_buf,*pos,num_bits_to_write);    
  *pos += num_bits_to_write;
  
  if(emm_supported_codecs->length <= MAX_NUM_CODECS)
  {
    for(index=0;index<(emm_supported_codecs->length);)
    {
      
      
        /*System Identification */
        b_packb((byte)emm_supported_codecs->codec_list[index].sysID,ota_buf,*pos,num_bits_to_write);    
        *pos += num_bits_to_write;
        /*System Identification Length*/
        b_packb((byte)emm_supported_codecs->codec_list[index].bitmap_length,ota_buf,*pos,num_bits_to_write);    
        *pos += num_bits_to_write;
        /*codec bitmap bit 0*/
        b_packb((byte)emm_supported_codecs->codec_list[index].bitmap_1_8,ota_buf,*pos,num_bits_to_write);    
        *pos += num_bits_to_write;
        /*codec bitmap bit 1*/
        b_packb((byte)emm_supported_codecs->codec_list[index].bitmap_9_16,ota_buf,*pos,num_bits_to_write);    
        *pos += num_bits_to_write;
        index += 1;
      
    }
  }
  else
  {
    MSG_ERROR_DS_0(MM_SUB, "Number of codecs is greater than 5\n");
    msg_lib_encode_error = TRUE;
    return;
  }
}
/*===========================================================================

FUNCTION  ENCODE_EPS_MOBILE_IDENTITY

DESCRIPTION
  This function encodes the EPS_MOBILE_IDENTITY information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_eps_mobile_identity(lte_nas_emm_eps_mobile_id_type *eps_mobile_id, byte *ota_buf, word *pos)
{ 
  /*--------------------------------------------------------
                EPS mobile identity
  ----------------------------------------------------------*/ 
  
  switch(eps_mobile_id->ue_id)
  {
    case GUTI_ID:/* ---- GUTI ---- */
      encode_guti_ie(&eps_mobile_id->ue_id_info.guti,pos,ota_buf);
      break;
    case IMSI_ID:/* ---- IMSI ---- */
      encode_imsi_ie(&eps_mobile_id->ue_id_info.imsi,pos,ota_buf);
      break;
    case IMEI_ID:
      encode_imei_ie(&eps_mobile_id->ue_id_info.imei,pos,ota_buf);
      break;
    default:
      MSG_FATAL_DS(MM_SUB,"NAS_MSG_LIB: Invalid mobile identity type",
                eps_mobile_id->ue_id,0,0);
      break;
  }/* End of "EPS mobile identity"*/
}/* End of Function for encoding EPS Mobile Identity*/


/*===========================================================================

FUNCTION  ENCODE_EPS_BEARER_CONTEXT_STATUS

DESCRIPTION
  This function encodes the EPS_BEARER_CONTEXT_STATUS information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_eps_bearer_context_status
(
  const lte_nas_eps_bearer_context_status_type *eps_bearer_context_status, 
  byte *ota_buf, 
  word *pos
)
{
  byte num_bits_to_write = 0;

  /*-------------------
           IEI
  -------------------*/
  num_bits_to_write = 8;
  b_packb((byte)EPS_BEARER_IDENTITY_IEI,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  /*-------------------
         LENGTH
  -------------------*/
  b_packb((byte)2,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  num_bits_to_write = 1;
  /*---------------------------------------
      Write the Active bearers bit by bit
  ----------------------------------------*/
  b_packb((byte)eps_bearer_context_status->eps_bearer_context_7_exists,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  b_packb((byte)eps_bearer_context_status->eps_bearer_context_6_exists,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  b_packb((byte)eps_bearer_context_status->eps_bearer_context_5_exists,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  /*---------------------------------------
             SPARE BITS
  ----------------------------------------*/
  num_bits_to_write = 5;
  b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  /*-------------------------------------------------
     Continue writing the Active bearers bit by bit
  --------------------------------------------------*/
  num_bits_to_write = 1;
  b_packb((byte)eps_bearer_context_status->eps_bearer_context_15_exists,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  b_packb((byte)eps_bearer_context_status->eps_bearer_context_14_exists,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  b_packb((byte)eps_bearer_context_status->eps_bearer_context_13_exists,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  b_packb((byte)eps_bearer_context_status->eps_bearer_context_12_exists,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  b_packb((byte)eps_bearer_context_status->eps_bearer_context_11_exists,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  b_packb((byte)eps_bearer_context_status->eps_bearer_context_10_exists,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  b_packb((byte)eps_bearer_context_status->eps_bearer_context_9_exists,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  b_packb((byte)eps_bearer_context_status->eps_bearer_context_8_exists,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;

}

/*===========================================================================

FUNCTION  ENCODE_MS_NW_FEATURE_SUPPORT

DESCRIPTION
  This function encodes the MS_NW_FEATURE_SUPPORT information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_LTE_REL10
void encode_ms_nw_feature_support(boolean *ext_periodic_timer_support, byte *ota_buf, word *pos)
{
  byte num_bits_to_write;
  num_bits_to_write = 4;
  b_packb(MS_NW_FEATURE_SUPPORT,ota_buf,*pos,num_bits_to_write); 
  *pos += num_bits_to_write;
  
  /*3 spare bits*/
  num_bits_to_write = 3;
  b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  
   /*ext_periodic_timer_support*/
  num_bits_to_write = 1;
  b_packb(*ext_periodic_timer_support,ota_buf,*pos,num_bits_to_write); 
  *pos += num_bits_to_write;
}

/*===========================================================================

FUNCTION  ENCODE_EMM_DEVICE_PROPERTIES_IE

DESCRIPTION
  This function encodes the DEVICE PROPERTIES information Element

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_emm_device_properties_ie(boolean *low_priority_indicator, byte *ota_buf, word *pos)
{
  byte num_bits_to_write = 4;

  ASSERT(low_priority_indicator != NULL);
  ASSERT(ota_buf != NULL);
  ASSERT(pos != NULL);

  /* IEI value */
  b_packb(DEVICE_PROPERTIES_IEI, ota_buf, *pos, num_bits_to_write); 
  *pos += num_bits_to_write;
  
  /* 3 spare bits */
  num_bits_to_write = 3;
  b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  
   /* low priority indicator */
  num_bits_to_write = 1;
  b_packb(*low_priority_indicator,ota_buf,*pos,num_bits_to_write); 
  *pos += num_bits_to_write;
}
#endif /* FEATURE_LTE_REL10 */

#endif /*FEATURE_LTE*/
