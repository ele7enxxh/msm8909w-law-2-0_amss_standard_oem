
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_parse_nas_hdr.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when         who                          what                              
--------     --------  ------------------------------------------------
11/10/09      hnam      Added additional checks for UL/DL int and enc.
08/28/09      hnam     Added support for secure EMM and ESM messages.
08/04/09      hnam     Resolved some lint errors due to NULL_INTEGRITY featurization
06/16/09      hnam     Corrected the code to be compatible with security APIs
==============================================================================*/

/*==============================================================================
                         HEADER 
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "lte_nas_security_info.h"
#include "msg_lib_encode_emm.h"
#include "lte_nas_emm_message_description.h"
#include "lte_nas_common_v.h"
#include "lte_security.h"
#include "lte_nas.h"
#include "comdef.h"
#include "err.h"
#include "bit.h"
#include "msg_lib_declarations.h"
#include "lte_nas_security_info.h"


/*==============================================================================
                      DATA DECLARATIONS
==============================================================================*/
void convert_array_to_dword(byte *array_ptr,dword *var)
{                                  
  ///* byte size_of_var = 4/*sizeof(dword)*/;  
  //*var = *array_ptr;                
  //array_ptr++;                     
  //size_of_var--;                   
  //while(size_of_var)               
  //{                                
  //  *var = (*var << 8) + *array_ptr; 
  //  array_ptr++;                   
  //  size_of_var--;                 
  //}      
  //*/

}

void convert_dword_to_array(dword *var,byte *array_ptr)
{ 
  *array_ptr = (byte)((0xFF000000 & *var)>> 24);
  array_ptr++;
  *array_ptr = (byte)((0x00FF0000 & *var)>>16);
  array_ptr++;
  *array_ptr = (byte)((0x0000FF00 & *var)>>8);
  array_ptr++;
  *array_ptr = (byte)(0x000000FF & *var);
  array_ptr++;
}

lte_nas_sec_info_params_type  nas_sec_params;

boolean decipher_nas_msg
(
   byte                          *src,  
     word                          *rx_buffer_length,
   lte_nas_dl_security_info_type *nas_dl_security_info
)
{
  lte_sec_errno_e  nas_encrypt_errno;
  dword nas_count = 0;
  word plain_nas_msg_offset = 6; /*This includes PD + Sec (1 octet) + MAC (4) + SQN (1)*/
  byte first_byte_encoded = 1;
  byte *decipherable_ota_msg;
  byte add_maci_offset = 4; /*MACI values (4 bytes) were written in nas_msg_integrity_check(), increment the buffer length again*/
  word actual_buffer_length = (*rx_buffer_length  + (first_byte_encoded + add_maci_offset)*8 - (plain_nas_msg_offset)*8 )/8;

  decipherable_ota_msg = src + plain_nas_msg_offset;

  nas_count = (( dword) nas_dl_security_info->nas_count[2] << 16  ) | ( ( dword) nas_dl_security_info->nas_count[1] << 8 ) | ( dword) nas_dl_security_info->nas_count[0] ;
  nas_encrypt_errno = lte_security_stream_decipher(nas_dl_security_info->security_alg.ciphering_alg, 
                                                   nas_dl_security_info->security_alg.ciphering_alg_key, 
                                                   decipherable_ota_msg,/*Input buffer address*/
                                                   (uint16)actual_buffer_length, 
                                                   decipherable_ota_msg,/*output buffer address*/
                                                   nas_dl_security_info->bearer,
                                                   nas_count);
  if(nas_encrypt_errno != E_SUCCESS)
  {
    MSG_ERROR_DS_0(MM_SUB, "Deciphering failed, exiting MSG_LIB\n");
    return FALSE;
  }
  MSG_HIGH_DS_0(MM_SUB, "Message deciphered successfully\n");
  return TRUE;
}

/*Check the functionality of this function*/
boolean nas_msg_integrity_check
( 
   byte                          *src, 
     word                          *pos, 
     word                          *buffer_length,
   lte_nas_dl_security_info_type *nas_dl_sec_info, 
   boolean                       nas_msg_lib,
   boolean                       smc
)
{
  byte rx_maci[4] = {0};
  word plain_nas_msg_offset = 5; /*This includes PD + Sec (1 octet) + MAC (4)*/  
  lte_sec_errno_e nas_encrypt_errno;
  dword nas_count = 0;
  dword maci = 0;
  boolean dl_direction = 1;
  //word actual_buffer_length = (*buffer_length - plain_nas_msg_offset*8 + first_octet_decoded*8)/8;
  //byte *integrity_src = {NULL};

  if(nas_dl_sec_info != NULL)
  { 
    //integrity_src = src + plain_nas_msg_offset;
    /*====================================================================
             Computes MAC-I on the input data stream
    =====================================================================*/  

  nas_count = (( dword) nas_dl_sec_info->nas_count[2] << 16  ) | ( ( dword) nas_dl_sec_info->nas_count[1] << 8 ) | ( dword) nas_dl_sec_info->nas_count[0] ;

  nas_encrypt_errno = lte_security_stream_compute_integrity_maci(nas_dl_sec_info->security_alg.integrity_alg, 
                                                                 nas_dl_sec_info->security_alg.integrity_alg_key, 
                                                                 src + plain_nas_msg_offset,
                                                                 (uint16)((*buffer_length)/WORD_SIZE - plain_nas_msg_offset), 
                                                                 (uint32*)&maci,
                                                                 nas_dl_sec_info->bearer,
                                                                 (uint32)nas_count,
                                                                 dl_direction);
  if(nas_encrypt_errno != E_SUCCESS)
  {
    MSG_ERROR_DS_0(MM_SUB, "lte_security_stream_compute_integrity_maci returned FAILURE");
    return FALSE;
  }
  convert_dword_to_array(&maci,&rx_maci[0]);
 
  if(memcmp(&nas_dl_sec_info->maci[0],&rx_maci[0],sizeof(rx_maci)))
  {
    MSG_ERROR_DS_0(MM_SUB, "Error in receiving the MAC-I algorithm\n");
    return FALSE;
  }      
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "Integrity check passed\n");
  }   
  return TRUE;
  }
  MSG_ERROR_DS_0(MM_SUB, "Integrity check failed\n");
  return FALSE;
}

void integrity_protect_nas_msg
(
  byte                          *ota_buf,
  lte_nas_ul_security_info_type *security_info,
  word                          *msg_len,
  lte_nas_outgoing_msg_type     *out_msg
)
{
  byte              num_bits_to_write;
  errno_enum_type   nas_encrypt_errno;
  dword             maci;  
  byte              dl_direction        = 0;
  byte              maci_offset         = 8;   

  word              temp_msg_len        = 0;
  byte              *temp_ota_buf;
  word              seq_num_offset      = 11;/*SEQ Number starts from bit 12 (starting from 0)*/ 
  dword nas_count = 0;

  nas_count = (( dword) security_info->nas_count[2] << 16  ) | ( ( dword) security_info->nas_count[1] << 8 ) | ( dword) security_info->nas_count[0] ;

  if((out_msg->emm_outgoing_msg.service_req.security_hdr == SERV_REQ) &&    
     (out_msg->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES))
  {
    maci_offset = 16;
    temp_msg_len = *msg_len;
    temp_ota_buf = ota_buf;

      /*Write the bits 0-4 of 2nd octet (i.e.NAS_COUNT)*/
    num_bits_to_write = 5;/*SEQ NUMBER*/
    b_packb(security_info->nas_count[0],ota_buf,seq_num_offset,num_bits_to_write); 
  }
  else
  {
    temp_msg_len = *msg_len - 5;
    temp_ota_buf = ota_buf + 5;
  }
  //word plain_nas_msg_offset = 5; /*This includes PD + Sec (1 octet) + MAC (4)*/
  //word actual_buffer_length = *msg_len - plain_nas_msg_offset;
  /*===========================================================================
           Computes MAC-I on the input data stream
  =========================================================================*/
  
  MSG_HIGH_DS_4(MM_SUB, "NAS_COUNT[0] = %d, NAS_COUNT[1] = %d, NAS_COUNT[2] = %d, NAS COUNT = %u",
                security_info->nas_count[0], security_info->nas_count[1],
                security_info->nas_count[2], nas_count);

  nas_encrypt_errno = lte_security_stream_compute_integrity_maci(security_info->security_alg.integrity_alg, 
                                                                 security_info->security_alg.integrity_alg_key, 
                                                                 temp_ota_buf, 
                                                                 (uint16)temp_msg_len, 
                                                                 (uint32*)&maci,
                                                                 security_info->bearer,
                                                                 nas_count,
                                                                 dl_direction);
  if(nas_encrypt_errno)
  {    
    MSG_ERROR_DS_0(MM_SUB, "MSG_LIB:There was an error in executing ciphering algorithm for the outgoing message\n");
    msg_len = 0;
  }
  /*------------ Add MAC-I -------------*/
  if((out_msg->emm_outgoing_msg.service_req.security_hdr == SERV_REQ)&&
    (out_msg->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES))
  {
    MSG_HIGH_DS_1(MM_SUB, "SERV REQ MACI = %u", maci);
    num_bits_to_write = 16;/*Short MACI*/
    b_packw((word)maci,ota_buf,maci_offset,num_bits_to_write); 
  }
  else
  {
    num_bits_to_write = 32;
    b_packd(maci,ota_buf,maci_offset,num_bits_to_write); 
  }
}

void add_nas_sec_hdr_first_octet
(
  lte_nas_outgoing_msg_type     *out_msg,
  boolean                       encryption_rqd,
  byte                          *ota_buf
)
{
  byte num_bits_to_write = 4;
  byte pd_offset = 4;
  byte first_bit = 0;  

  byte security_hdr_type_val = 0;
  /*--------------------------------------------------------
                       SECURITY HEADER
  ----------------------------------------------------------*/  
  security_hdr_type_val = (byte)INTEGRITY_PROTECT;
  if(encryption_rqd)
  {
    security_hdr_type_val = (byte)INTEGRITY_PROTECT_CIPHER;     
  } 
  if(service_request)
  {
    security_hdr_type_val = (byte)SERV_REQ;
  }
  if(out_msg->nas_hdr.msg_id == SECURITY_MODE_COMPLETE)
  {
    security_hdr_type_val = (byte)INTEGRITY_PROTECT_SECURITY_MODE_COMPLETE; 
  }
  b_packb((byte)security_hdr_type_val,ota_buf,first_bit,num_bits_to_write);
  /*--------------------------------------------------------
                  PROTOCOL DISCRIMINATOR
  ----------------------------------------------------------*/
  b_packb((byte)EPS_MOBILITY_MANAGEMENT_MESSAGES,ota_buf,pd_offset,num_bits_to_write);  
}

void encode_security_protected_nas_hdr
(
  lte_nas_outgoing_msg_type     *out_msg, 
  byte                          *ota_buf,
  lte_nas_ul_security_info_type *security_info,
  word                          *msg_len
)
{
  errno_enum_type  nas_encrypt_errno = E_FAILURE;
  byte             maci[4];  
  boolean          encryption = FALSE;  
  dword            nas_count = 0;

  word             plain_nas_msg_offset = 0;
  word             actual_buffer_length = 0;

  if((out_msg->emm_outgoing_msg.service_req.security_hdr == SERV_REQ) &&
     (out_msg->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES))
  {
    actual_buffer_length = *msg_len; 
  }
  else
  {
    plain_nas_msg_offset = 6; /*This includes PD + Sec (1 octet) + MAC (4) + SQN (1)*/
    actual_buffer_length = *msg_len - plain_nas_msg_offset;
  }

  if((out_msg->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES)&&
     ((out_msg->nas_hdr.msg_id == ATTACH_REQUEST)||(out_msg->nas_hdr.msg_id == TRACKING_AREA_UPADTE_REQUEST)))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= set Cipher_Flag to FALSE for encoding the MSG_ID %d",
                  out_msg->nas_hdr.msg_id);
    security_info->cipher_valid = FALSE;
  }

  memset(&maci[0],0,sizeof(maci)); 

  if((out_msg->emm_outgoing_msg.service_req.security_hdr == SERV_REQ) &&
     (out_msg->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= not ciphering the SERV_REQ");
  }
  else
  {       
    /*===========================================================================
                     Does ciphering on the input data stream
    ============================================================================*/
    if(security_info->cipher_valid)
    {
      encryption = TRUE;
      //convert_array_to_dword(security_info->nas_count,&nas_count);
      nas_count = (( dword) security_info->nas_count[2] << 16  ) | ( ( dword) security_info->nas_count[1] << 8 ) | ( dword) security_info->nas_count[0] ;
      nas_encrypt_errno = lte_security_stream_cipher(
                                                     security_info->security_alg.ciphering_alg, 
                                                     security_info->security_alg.ciphering_alg_key, 
                                                     ota_buf + plain_nas_msg_offset,/*Input buffer address*/
                                                     (uint16)actual_buffer_length, 
                                                     ota_buf + plain_nas_msg_offset,/*output buffer address*/
                                                     security_info->bearer,
                                                     ((uint32)nas_count)
                                                      );
  
     if(nas_encrypt_errno != E_SUCCESS)
     {    
       MSG_ERROR_DS_0(MM_SUB, "MSG_LIB:There was an error in executing ciphering algorithm for the outgoing message\n");
       msg_len = 0;
       msg_lib_encode_error = TRUE;
       return;
     }
    }
  }
   /*========================================================================
                  Add Integrity on the input data stream
   =========================================================================*/
  if(security_info->int_valid)
   {
     integrity_protect_nas_msg(ota_buf,security_info,msg_len, out_msg);
   }
  if((out_msg->emm_outgoing_msg.service_req.security_hdr == SERV_REQ) &&
     (out_msg->nas_hdr.pd == EPS_MOBILITY_MANAGEMENT_MESSAGES))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= there is no security header for SERV_REQ");
  }      
  else
  {
   /*------------ Add Security header + PD -------------*/
   add_nas_sec_hdr_first_octet(out_msg, encryption,ota_buf);
  }
}

#endif /*FEATURE_LTE*/


