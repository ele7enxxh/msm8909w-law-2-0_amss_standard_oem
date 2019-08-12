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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_encode_emm.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when         who                          what                              
--------     --------  ------------------------------------------------
09/30/11      Rajesh  Added code to exclude GUTI type IE in TAU and Attach request if
                      "exclude_identity_type_ie" is set.
08/01/11      Rajesh  Support to include EPS bearer context in extended service request
08/18/11     abhi      Added support for GUTI type IE
10/07/10      MNK     1XCSFB introduction 
06/03/10      zren    Added URC UPD IEI support 
05/14/10      RI      Mainlined spec changes upto Dec 09.
-1/22/10      hnam     Updated the KSI handling.
01/21/10      zren    Added EMM/ESM STATUS feature
12/18/09      RI       Updated the UE security capability handling.
12/16/09      zren     Updated to support GUTI or IMSI as EPS mobile 
                       identification in TAU request message
08/13/09      hnam     Added ptmsi and additional guti for ATTACH REQUEST(March spec compliance)
08/11/09      hnam     Changed the UE NW capability value as per LSTI Doc.
07/29/09      hnam     Changed the name of the MACRO (for MARCH/DEC spec compatibiltiy), 
                        used the standard macro commonly used by LTE_MODEM_SW
07/07/09      zren     Moved define FEATURE_RELEASE_LTE_MARCH_SPEC to 
                       lte_nas_common.h
06/09/09      hnam     Featurized the TAU REQ code and added a fix to CR # 185494 
05/20/09      hnam     Fixed the encoding to write the first NAS octet correctly
05/17/09      hnam     Added the code to support security
04/27/09      hnam     Added authentication response code
04/14/09      hnam     Added backward compatibility to UE NW capability
04/13/09      hnam     Added code for authentication failure.
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE

#include "comdef.h"
#include "bit.h"
#include "lte_nas.h"
#include "lte_nas_common_v.h"
#include "msg_lib_encode_emm.h"
#include "msg_lib_encode_esm.h"
#include "msg_lib_encode_common_emm_ie.h"
#include "lte_nas_emm_message_type.h"
#include "lte_nas_emm_message_description.h"
#include "msg_lib_declarations.h"
#include "msg.h"
#include "err.h"
#include "lte_nas_log.h"
#include "naslog_v.h"
#include "lte_log_codes.h"
#include "mm_umts_v.h"

#ifdef FEATURE_LTE_REL9 
#include "mm_v.h"
#include "emm_database.h"
#endif
#include "stringl.h"
#include "reg_sim.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define LAST_VISITED_REGISTERED_TAI_IEI 0x52
#define DRX_PARAM_IEI 0x5C
#define LOCN_AREA_IDENTIFICATION_IEI 0x13
#define CLASS_MARK_2_IEI 0x11
#define CLASS_MARK_3_IEI 0x20
#define TMSI_STATUS_IEI 0x09
#define SUPPORTED_CODECS_IEI 0x40
#define MS_IDENTITY_IEI 0x23
#define AUTH_FAIL_PARAM 0X30
#define GUTI_IEI 0x50
#define PTMSI_IEI 0x19
#define NONCE_IEI 0x55
#define MS_NW_CAPABILITY_IEI 0x31
#define ADDITIONAL_UPDATE_IEI 0xF

#define SGSN_IEI 0x0B
#define CIPHERING_KEY_SEQ_NUM_IEI 0x08
#define UE_NW_CAPABILITY_IEI 0x58
#define LTE_NAS_URC_UPD_IEI  0xA
#define CSFB_RESP_IEI 0xB

#ifdef FEATURE_LTE_REL9 
#define VOICE_DOMAIN_PREF_IEI 0x5D
#endif

#ifdef FEATURE_LTE_REL11 
#define TMSI_BASED_NRI_IEI 0x10
#endif

#define GUTI_TYPE_IEI 0xE
boolean service_request;
boolean detach_reason;
/*===========================================================================

                        FUNCTION DEFINITION

===========================================================================*/

#if defined (FEATURE_LTE_REL9) 
/*===========================================================================

FUNCTION  ENCODE_UL_GENERIC_NAS_TRANSPORT

DESCRIPTION
  This function encodes the UL_GENERIC_NAS_TRANSPORT message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_ul_generic_nas_transport
(
  byte *ota_buf,
  size_t ota_buf_len,
  lte_nas_outgoing_msg_type *out_buf,
  word *pos
)
{
  byte num_bits_to_write;
  lte_nas_emm_ul_generic_nas_transport  *ul_generic_nas_transport;

  ul_generic_nas_transport = &out_buf->emm_outgoing_msg.ul_generic_nas_transport_msg;

  /*--------------------------------
    Generic message container type
  ---------------------------------*/  
  num_bits_to_write = 8;
  b_packb((byte)ul_generic_nas_transport->generic_msg_container_type,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 
 
  /*--------------------------------
      Generic message container
  ---------------------------------*/  
  /*-----Length-----*/
  num_bits_to_write = 16;
  b_packw(ul_generic_nas_transport->generic_msg_container.container_length,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 
  /*Value*/
  num_bits_to_write = 8;
  memscpy((void*)(ota_buf + (*pos/8)), //Skip the 'already encoded bytes'
          ota_buf_len - (*pos/8),         
          (void*)ul_generic_nas_transport->generic_msg_container.container_value_ptr,
          ul_generic_nas_transport->generic_msg_container.container_length);
  *pos += num_bits_to_write * ul_generic_nas_transport->generic_msg_container.container_length;
  
  /*--------------------------------
    EMM_ADDITIONAL_INFORMATION_IEI
  --------------------------------*/
  if(ul_generic_nas_transport->additional_info_exists)
  {
    /*IEI*/
    b_packb(EMM_ADDITIONAL_INFORMATION_IEI,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write; 
    /*IEI Length*/
    b_packb(ul_generic_nas_transport->additional_info.additional_info_length,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write; 
    /*IEI Value*/
    memscpy((void*)(ota_buf + (*pos/8)),//Skip the 'already encoded bytes'
            ota_buf_len - (*pos/8),
            (void*)ul_generic_nas_transport->additional_info.additional_info_value_ptr,
            ul_generic_nas_transport->additional_info.additional_info_length);
    *pos += num_bits_to_write * ul_generic_nas_transport->additional_info.additional_info_length;    
  }
}
#endif

/*===========================================================================

FUNCTION  ENCODE_EXT_SERV_REQ

DESCRIPTION
  This function encodes the EXTENDED_SERVICE_REQUEST message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_ext_serv_req
(
  byte *ota_buf, 
  lte_nas_outgoing_msg_type *out_buf, 
  word *pos
#ifdef FEATURE_LTE_REL9
  ,emm_ctrl_data_type *emm_ctrl_data_ptr
#endif
)
{
  byte num_bits_to_write;
  lte_nas_emm_extended_service_req   *ext_service_req;
  byte m_tmsi_len = 5;/*M_TMSI has a total length of 5 bytes -- refer 24.301, EXT_SERV OTA message*/
  byte m_tmsi_identity = 4; /*1 0 0   --> TMSI/P-TMSI/M-TMSI*/

  ext_service_req = &out_buf->emm_outgoing_msg.ext_service_req;

  /*----------------------
        NAS KEYSET ID
  -----------------------*/
  num_bits_to_write = 1;
  b_packb((byte)ext_service_req->nas_key_set_identifier.security_context,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 
  num_bits_to_write = 3;
  b_packb((byte)ext_service_req->nas_key_set_identifier.keyset_id,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;

  /*Service type*/  
  num_bits_to_write = 4;
  b_packb((byte)ext_service_req->service_type,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 

  /*Mobile identity*/
  num_bits_to_write = 8;
  b_packb((byte)m_tmsi_len,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 

  /*Identity Digit 0*/
  num_bits_to_write = 4;
  b_packb((byte)0xF,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 

  /*Even or Odd Indicator*/
  num_bits_to_write = 1;
  b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 

  /*Type of Identity*/
  num_bits_to_write =3;
  b_packb((byte)m_tmsi_identity,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 

  num_bits_to_write = 8;
  b_packb(ext_service_req->m_tmsi[3],ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 
  b_packb(ext_service_req->m_tmsi[2],ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 
  b_packb(ext_service_req->m_tmsi[1],ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 
  b_packb(ext_service_req->m_tmsi[0],ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 

  /*CSFB response*/
  if(ext_service_req->service_type == LTE_MT_CSFB_1XCSFB)
  {
    num_bits_to_write = 4;
    b_packb((byte)CSFB_RESP_IEI,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;    
    num_bits_to_write = 4;
    b_packb((byte)ext_service_req->csfb_resp,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;  
  }

  /*--------------------------------------------------------
                 EPS BEARER CONTEXT STATUS       
  ----------------------------------------------------------*/ 
#ifdef FEATURE_LTE_REL9
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9) &&
     (ext_service_req->eps_bearer_context_status_exists))
  {
    encode_eps_bearer_context_status(&(ext_service_req->eps_bearer_context_status), ota_buf, pos);
  }
#endif

#ifdef FEATURE_LTE_REL10
    /*  Device Properties IE */
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
     (reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE))
  {
    MSG_HIGH_DS_1(MM_SUB, "Low priority indicator value = %d in EXT_SERVICE_REQ",
                  ext_service_req->low_priority_indicator);
    encode_emm_device_properties_ie(&(ext_service_req->low_priority_indicator), ota_buf, pos);
  }
#endif
}

/*===========================================================================

FUNCTION  ENCODE_IDENTITY_RESP

DESCRIPTION
  This function encodes the IDENTITY_RESPONSE message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_identity_resp(byte *ota_buf, lte_nas_outgoing_msg_type *out_buf, word *pos)
{
  byte num_bits_to_write;
  lte_nas_emm_identity_resp  *identity_resp;
  byte index = 0;
  byte no_identity_len = 0;
  boolean odd_or_even_indicator = FALSE;
  byte tmsi_ie[TMSI_SIZE + 2] = {0};
  uint32 tmsi_len = 0;

  identity_resp = &out_buf->emm_outgoing_msg.identity_resp;

  /*-----------------------------------------------------------------------
     Mobile Identity (Refer Spec: 24.008.830 --> Figure 10.5.4/3GPP )
  -------------------------------------------------------------------------*/
  switch(identity_resp->ms_id.mobile_id)
  {
  case LTE_NAS_EMM_IMSI:
    if(identity_resp->ms_id.mobile_id_lst.imsi.num_id_digits_len <= MAX_NO_OF_IMSI)
    {
      encode_imsi_ie(&identity_resp->ms_id.mobile_id_lst.imsi,pos,ota_buf);
    }
    else
    {
      msg_lib_encode_error = TRUE;
    }
    break;
  case LTE_NAS_EMM_IMEI:
    if(identity_resp->ms_id.mobile_id_lst.imei.length == MAX_IMEI_LEN)
    {
      encode_imei_ie(&identity_resp->ms_id.mobile_id_lst.imei, pos, ota_buf);
    }
    else
    {
      msg_lib_encode_error = TRUE;
    }
    break;
  case LTE_NAS_EMM_IMEISV:
    /* -- Length -- */
    num_bits_to_write = 8;
    b_packb(identity_resp->ms_id.mobile_id_lst.imeisv.length,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write;    

    /* --- IMEISV Value (array of 9 bytes) --- */
    for(index =0;index <(identity_resp->ms_id.mobile_id_lst.imeisv.length); index++)
    {
      b_packb(identity_resp->ms_id.mobile_id_lst.imeisv.value[index],ota_buf,*pos,num_bits_to_write);  
      *pos += num_bits_to_write;
    }/*End of for()*/ 
    break;
  case LTE_NAS_EMM_TMSI:
    num_bits_to_write = 8;
    tmsi_len = mm_pack_tmsi(tmsi_ie);
    for(index = 0; index < tmsi_len; index++)
    {
      b_packb(tmsi_ie[index],ota_buf,*pos,num_bits_to_write);  
      *pos += num_bits_to_write;
    }
    break;
  default:
    MSG_HIGH_DS_0(MM_SUB, "Encoding 'LTE_NAS_EMM_NO_ID' as the default Mobile Identity");
    /* -- Length -- */
    /* 24.008 table 10.5.4 
       For Type of identity "No Identity", the Identity digit bits shall be 
       encoded with all 0s and the Length of mobile identity contents parameter 
       shall be set to "3" if the EMM identification procedure is used */
    num_bits_to_write = 8;
    no_identity_len = 3;
    b_packb(no_identity_len,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write; 
    /* Identity digit 1 (4 bits) */
    num_bits_to_write = 4;
    b_packb((byte)0,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write; 
    /* Set odd/even indicator to 1 (odd) */
    num_bits_to_write = 1;
    odd_or_even_indicator = 1;
    b_packb(odd_or_even_indicator,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write; 
    /* Mobile Identity Type */
    num_bits_to_write = 3;    
    b_packb((byte)LTE_NAS_EMM_NO_ID,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write;    
    /* Identity digit 2 - 5 (16 bits)*/ 
    num_bits_to_write = 16;    
    b_packb((byte)0,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write;     
    break;
  }  
}
/*===========================================================================

FUNCTION  ENCODE_SECURITY_REJECT

DESCRIPTION
  This function encodes the SECURITY_REJECT message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_security_reject(byte *ota_buf, lte_nas_outgoing_msg_type *out_buf, word *pos)
{
  byte num_bits_to_write;
  lte_nas_emm_security_mode_reject    *security_reject;  

  security_reject = &out_buf->emm_outgoing_msg.security_rej;
  
  num_bits_to_write = 8;
  b_packb((byte)security_reject->emm_cause,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;

}

/*===========================================================================

FUNCTION  ENCODE_TAU_REQUEST

DESCRIPTION
  This function encodes OTA TRACKING AREA UPDATE REQUEST message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_tau_request(byte *ota_buf, lte_nas_outgoing_msg_type *out_buf, word *pos)
{
  byte num_bits_to_write;
  lte_nas_emm_tau_req *tau_req;
  emm_ctrl_data_type        *emm_ctrl_data_ptr;
  byte skip_first_two_octets = 2*WORD_SIZE;
  byte plmn_len = 3;
  byte octet = 0;

  *pos = *pos + skip_first_two_octets;
  tau_req = &out_buf->emm_outgoing_msg.tau_req;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  /*--------------------------------------------------------
                 NAS key set identifier ASME       
  ----------------------------------------------------------*/   
  num_bits_to_write = 1;
  b_packb((byte)tau_req->nas_key_set_identifier_asme.security_context,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 
  num_bits_to_write = 3;
  b_packb((byte)tau_req->nas_key_set_identifier_asme.keyset_id,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 
  

  /*--------------------------------------------------------
                       EPS Update type
  ----------------------------------------------------------*/ 
  num_bits_to_write = 1;
  b_packb((byte)tau_req->eps_update.active_flag,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  num_bits_to_write = 3;
  b_packb((byte)tau_req->eps_update.eps_update_val,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;

  /*--------------------------------------------------------
                       EPS mobile identity
  ----------------------------------------------------------*/ 
  encode_eps_mobile_identity(&tau_req->eps_mobile_id, ota_buf, pos);

  /*--------------------------------------------------------
                 NAS key set identifier SGSN       
  ----------------------------------------------------------*/ 
  if(tau_req->nas_keyset_identifier_sgsn_exists)
  {
    /*----------------------
              IEI
    -----------------------*/
    num_bits_to_write = 4;
    b_packb((byte)SGSN_IEI,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;
    /*----------------------
        SECURITY CONTEXT
    -----------------------*/
    num_bits_to_write = 1;
    b_packb((byte)tau_req->nas_key_set_identifier_sgsn.security_context,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;
    num_bits_to_write = 3;
    /*----------------------
          KEYSET ID
    -----------------------*/
    b_packb((byte)tau_req->nas_key_set_identifier_sgsn.keyset_id,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;
  }

  /*--------------------------------------------------------
              GPRS Ciphering Key Sequence Number       
  ----------------------------------------------------------*/
  if(tau_req->ciphering_key_seq_num_exists)
  {
    /*-------------------
             IEI
    -------------------*/
    num_bits_to_write = 4;
    b_packb((byte)CIPHERING_KEY_SEQ_NUM_IEI,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;
    /*-------------------
           SPARE
    -------------------*/
    num_bits_to_write = 1;
    *pos += num_bits_to_write;
    /*-------------------
           VALUE
    -------------------*/
    num_bits_to_write = 3;
    b_packb((byte)tau_req->ciphering_key_seq_num,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;
  }

  /*------------------------------------------------------------------
                         Old P-TMSI signature (Optional)
  -------------------------------------------------------------------*/

  if(tau_req->ptmsi_signature_exists == TRUE)
  {
    /*-------------------
             IEI
    -------------------*/
    num_bits_to_write = 8;  
    b_packb(PTMSI_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*-------------------
           VALUE
    -------------------*/
    for(octet = 0;octet< sizeof(tau_req->ptmsi_signature);octet++)
    {
      b_packb(tau_req->ptmsi_signature[octet],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
  }
  /*------------------------------------------------------------------
                         Additional GUTI (Optional)
  -------------------------------------------------------------------*/  
  if(tau_req->additional_guti_exists == TRUE)
  {
    /*-------------------
             IEI
    -------------------*/
    num_bits_to_write = 8;  
    b_packb(GUTI_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*-------------------
           VALUE
    -------------------*/
    encode_guti_ie(&tau_req->additional_guti,pos,ota_buf);
  }
  /*------------------------------------------------------------------
                             NonceUE (Optional)
  -------------------------------------------------------------------*/
  if(tau_req->nonce_ue_exists == TRUE)
  {
    /*-------------------
             IEI
    -------------------*/
    num_bits_to_write = 8; 
    b_packb(NONCE_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*-------------------
           VALUE
    -------------------*/
    for(octet = 0;octet< sizeof(tau_req->nonce_ue);octet++)
    {
      b_packb(tau_req->nonce_ue[octet],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
  }
  /*------------------------------------------------------------------
                     UE network capability (Optional)
  -------------------------------------------------------------------*/
  if(tau_req->ue_capability_exists)
  {
    num_bits_to_write = 8; 
    b_packb(UE_NW_CAPABILITY_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    encode_ue_nw_capability_ie(&(tau_req->ue_capability),pos,ota_buf);
  }
  /*------------------------------------------------------------------
                  Last visited registered TAI (Optional)
  -------------------------------------------------------------------*/
  if(tau_req->last_visited_reg_tai_exists == TRUE)
  {
    /*----------------------
             IEI
    -----------------------*/
    num_bits_to_write = 8;  
    b_packb(LAST_VISITED_REGISTERED_TAI_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*----------------------
             PLMN
    -----------------------*/
    for(octet =0; octet <plmn_len;octet++)
    {
      b_packw(tau_req->last_visited_reg_tai.plmn.identity[octet],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
    /*----------------------
          TAC VALUE
    -----------------------*/
    num_bits_to_write = 16;
    b_packw(tau_req->last_visited_reg_tai.tac,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
  }
  /*------------------------------------------------------------------
                        DRX parameter (Optional)
  -------------------------------------------------------------------*/
  if(tau_req->drx_param_exists == TRUE)
  {
    /*----------------------
             IEI
    -----------------------*/
    num_bits_to_write = 8;  
    b_packb(DRX_PARAM_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;

    /*----------------------
             VALUE
    -----------------------*/
     b_packb(tau_req->drx_param.value[0],ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;

     b_packb(tau_req->drx_param.value[1],ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;
  }

  /*------------------------------------------------------------------
      UE radio capability information update needed (Optional)
  -------------------------------------------------------------------*/
  if(tau_req->ue_radio_capability_info_update == TRUE)
  {
    /*----------------------
             IEI
    -----------------------*/
    num_bits_to_write = 4;
    b_packb(LTE_NAS_URC_UPD_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*----------------------
         3 SPARE bits
    -----------------------*/
    num_bits_to_write = 3;
    b_packb(0,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;
    /*----------------------
          URC upd FLAG
    -----------------------*/
    num_bits_to_write = 1;
    b_packb(tau_req->ue_radio_capability_info_update,
            ota_buf,
            *pos,
            num_bits_to_write); 
    *pos += num_bits_to_write;
  }
  
  /*--------------------------------------------------------
                 EPS BEARER CONTEXT STATUS       
  ----------------------------------------------------------*/ 
  if(tau_req->eps_bearer_context_status_exists)
  {
    encode_eps_bearer_context_status(&(tau_req->eps_bearer_context_status), ota_buf, pos);
  }
  /*------------------------------------------------------------------
                     MS network capability (Optional)
  -------------------------------------------------------------------*/
  /* -------------------------------
  ** Set MS network capability field
  ** ------------------------------- */
  if(tau_req->ms_nw_capability_exists == TRUE)
  {
     /*----------------------
             IEI
     -----------------------*/
     num_bits_to_write = 8;
     b_packb((byte)MS_NW_CAPABILITY_IEI,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;

    /*----------------------
             LENGTH
    -----------------------*/
    b_packb((byte)tau_req->ms_nw_capability.length,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;

    /*----------------------
             VALUE
    -----------------------*/
    for (octet = 0; octet < tau_req->ms_nw_capability.length; octet++)
    {
      b_packb((byte)tau_req->ms_nw_capability.value[octet],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }    
  }

  /*------------------------------------------------------------------
                  Location area identification (Optional)
  -------------------------------------------------------------------*/
  if(tau_req->locn_area_id_exists == TRUE)
  {
    /*----------------------
             IEI
    -----------------------*/
    num_bits_to_write = 8;  
    b_packb(LOCN_AREA_IDENTIFICATION_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*----------------------
             PLMN
    -----------------------*/
    for(octet =0; octet <plmn_len;octet++)
    {
      b_packb(tau_req->locn_area_id.PLMN_id.identity[octet],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
    /*----------------------
             TAC VALUE
    -----------------------*/
    num_bits_to_write = 8;
    b_packb(tau_req->locn_area_id.location_area_code[0],ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    b_packb(tau_req->locn_area_id.location_area_code[1],ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
  }

  /*------------------------------------------------------------------
                         TMSI status (Optional)
  -------------------------------------------------------------------*/
  if(tau_req->tmsi_status_exists == TRUE)
  {
    /*----------------------
             IEI
    -----------------------*/
    num_bits_to_write = 4;
    b_packb(TMSI_STATUS_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*----------------------
         3 SPARE bits
    -----------------------*/
    num_bits_to_write = 3;
    b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;
    /*----------------------
          TMSI FLAG
    -----------------------*/
    num_bits_to_write = 1;
    b_packb(tau_req->tmsi_status,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
  }

  /*------------------------------------------------------------------
                    Mobile station classmark 2(Optional)
  -------------------------------------------------------------------*/
  if(tau_req->classmark_2_exists == TRUE)
  {
    /*----------------------
              IEI
    -----------------------*/
    num_bits_to_write = 8;
    b_packb(CLASS_MARK_2_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*----------------------
           LENGTH
    -----------------------*/
    num_bits_to_write = 8;
    b_packb(tau_req->classmark_2.length,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*----------------------
             VALUE
    -----------------------*/
    for(octet=0;octet < sizeof(tau_req->classmark_2.data); octet++)
    {
      b_packb(tau_req->classmark_2.data[octet],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
  }
  
  /*------------------------------------------------------------------
                    Mobile station classmark 3(Optional)
  -------------------------------------------------------------------*/
  if(tau_req->classmark_3_exists == TRUE)
  {
    /*----------------------
              IEI
    -----------------------*/
    num_bits_to_write = 8;
    b_packb(CLASS_MARK_3_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*----------------------
           LENGTH
    -----------------------*/
    num_bits_to_write = 8;
    b_packb(tau_req->classmark_3.length,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*----------------------
             VALUE
    -----------------------*/
    for(octet=0;octet < tau_req->classmark_3.length; octet++)
    {
      b_packb(tau_req->classmark_3.data[octet],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
  }

  /*------------------------------------------------------------------
          Supported Codecs (Optional) --- NOT SUPPORTED (as of now)
  -------------------------------------------------------------------*/
  if(tau_req->supported_voice_codec_exists == TRUE)
  {
    /*----------------------
               IEI
    -----------------------*/
    num_bits_to_write = 8;
    b_packb(SUPPORTED_CODECS_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*----------------------
              VALUE
    -----------------------*/
    if(tau_req->supported_voice_codec.length <=  MAX_NUM_CODECS)
    {
      encode_emm_supported_codecs(&tau_req->supported_voice_codec,ota_buf,pos);
    }
    else
    {
      msg_lib_encode_error = TRUE;
    }
  }
  /*------------------------------------------------------------------
                     ADDITIONAL_UPDATE (Optional) 
  -------------------------------------------------------------------*/
  if(tau_req->additional_update_exists == TRUE)
  {
    /*-------- IEI ---------*/
    num_bits_to_write = 4;
    b_packb(ADDITIONAL_UPDATE_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*-------- Skip 3 bits ---------*/
    num_bits_to_write = 3;
    *pos += num_bits_to_write;
    /*-------- VALUE ---------*/
    num_bits_to_write = 1;
    b_packb(tau_req->additional_update,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
  }

#ifdef FEATURE_LTE_REL9
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
  {
    if(tau_req->voice_domain_pref_exists == TRUE)
    {
      /*-------- IEI ---------*/
      num_bits_to_write = 8;
      b_packb(VOICE_DOMAIN_PREF_IEI,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;   

      /*-------- Length ---------*/      
      b_packb((byte)1,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;   

      /*-------- skip 5 bytes---------*/      
      num_bits_to_write = 5;
      *pos += num_bits_to_write;   

      /*-------- UE usage settings ---------*/      
      num_bits_to_write = 1;
      b_packb((byte)tau_req->voice_domain_pref.ue_usage_settings,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;   
      /*-------- Voice domain Preference ---------*/      
      num_bits_to_write = 2;
      b_packb((byte)tau_req->voice_domain_pref.voice_pref_for_utran,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;      
    }  
  }
#endif

  /*----------------------------------------------------
                    GUTI TYPE IEI (Optional)
   ----------------------------------------------------*/
  if(emm_ctrl_data_ptr->exclude_identity_type_ie != TRUE)
  {
    if(tau_req->guti_type_exists == TRUE)
    {     
      /*-------- IEI ---------*/
      num_bits_to_write = 4;
      b_packb(GUTI_TYPE_IEI,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
      
      /*3 spare bits*/
      num_bits_to_write = 3;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += num_bits_to_write;
  
      /*GUTI type*/
      num_bits_to_write = 1;
      b_packb((byte)tau_req->guti_type,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
  }

#ifdef FEATURE_LTE_REL10
    /*	Device Properties IE */
    if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
       (reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE))
    {
      MSG_HIGH_DS_1(MM_SUB, "Low priority indicator value = %d in TAU REQ",
                    tau_req->low_priority_indicator);
      encode_emm_device_properties_ie(&(tau_req->low_priority_indicator), ota_buf, pos);
    }
#endif

  /*----------------------------------------------------
                    MS network feature support (Optional)
   ----------------------------------------------------*/
#ifdef FEATURE_LTE_REL10
  if(tau_req->ms_nw_feature_support_exists == TRUE)
  {
    encode_ms_nw_feature_support(&(tau_req->ext_periodic_timer_support),ota_buf,pos);
  }
#endif



#ifdef FEATURE_LTE_REL11
   /*------------------------------------------------------------------
                         TMSI NRI (Optional)
  -------------------------------------------------------------------*/
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)&&
     (tau_req->tmsi_based_nri_exists == TRUE))
  {
    /*-------- IEI ---------*/
    num_bits_to_write = 8;  
    b_packb(TMSI_BASED_NRI_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;

    /*length of NRI container contents*/
    num_bits_to_write = 8;
    b_packb(tau_req->tmsi_based_nri.length,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;

    /*NRI container value-TMSI bits 23-16*/
    num_bits_to_write = 8;
    b_packb(tau_req->tmsi_based_nri.tmsi_based_nri_value[0],ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;

    /*NRI container value-TMSI bits 15-14*/
    num_bits_to_write = 8;
    b_packb(tau_req->tmsi_based_nri.tmsi_based_nri_value[1],ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
  }
#endif
}
/*===========================================================================

FUNCTION  ENCODE_AUTHENTICATION_RESPONSE

DESCRIPTION
  This function encodes the AUTHENTICATION_RESPONSE message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_authentication_response(byte *ota_buf, lte_nas_outgoing_msg_type *out_buf, word *pos)
{
  byte num_bits_to_write = 4,index =0;
  lte_nas_emm_auth_resp  *auth_resp;

  auth_resp = &out_buf->emm_outgoing_msg.auth_resp;

  num_bits_to_write = 8;
  b_packb((byte)auth_resp->nas_auth_resp.length,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;

  for(index =0; index < auth_resp->nas_auth_resp.length; index++)
  {
    b_packb((byte)auth_resp->nas_auth_resp.val[index],ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;    
  }
}
/*===========================================================================

FUNCTION  ENCODE_AUTHENTICATION_FAILURE

DESCRIPTION
  This function encodes the AUTHENTICATION_FAILURE message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_authentication_failure(byte *ota_buf, lte_nas_outgoing_msg_type *out_buf, word *pos)
{
  byte num_bits_to_write = 4, index =0;
  lte_nas_emm_auth_fail  *auth_fail; 

  auth_fail = &out_buf->emm_outgoing_msg.auth_fail;
  b_packb((byte)auth_fail->security_hdr,ota_buf,*pos,num_bits_to_write);

  /* ----- EMM cause ----- */
  num_bits_to_write = 8;
  b_packb((byte)auth_fail->emm_cause,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;

  if(auth_fail->auth_fail_param_exists == TRUE)
  {
    /* ----- Authentication failure parameter ----- */
    /* -- IEI -- */
    num_bits_to_write = 8;
    b_packb((byte)AUTH_FAIL_PARAM,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;

    /* -- Length -- */
    num_bits_to_write = 8;
    b_packb((byte)14,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;

    /* --- Value --- */
    for(index = 0; index < 14; index++)
    {
      b_packb((byte)auth_fail->auth_fail_param[index],ota_buf,*pos,num_bits_to_write);
      *pos += num_bits_to_write;
    }
  }  
}
/*===========================================================================

FUNCTION  ENCODE_SERVICE_REQ

DESCRIPTION
  This function encodes the SERVICE_REQ message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_service_request(byte *ota_buf, lte_nas_outgoing_msg_type *out_buf, word *pos)
{
  byte num_bits_to_write = 4;
  lte_nas_emm_service_req_type *serv_req;

  serv_req = &out_buf->emm_outgoing_msg.service_req;

  /* ---- KSI and sequence number ---- */
  num_bits_to_write = 3;
  b_packb((byte)serv_req->ksi_seqnum.keyset_id,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;

  /* ---- Sequence number (short) ---- */
  num_bits_to_write = 5;
  b_packb((byte)serv_req->ksi_seqnum.sequence_number,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;

}
/*===========================================================================

FUNCTION  ENCODE_SECURITY_MODE_COMPLETE

DESCRIPTION
  This function encodes the SECURITY_MODE_COMPLETE message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_security_mode_complete(byte *ota_buf, lte_nas_outgoing_msg_type *out_buf, word *pos)
{
  byte num_bits_to_write = 4;
  byte index =0;
  lte_nas_emm_security_mode_complete *security_mode_complete;

  security_mode_complete = &out_buf->emm_outgoing_msg.security_mode_complete;
  b_packb((byte)security_mode_complete->security_hdr,ota_buf,*pos,num_bits_to_write);

  /* ---- Encode IMEISV Mobile Identity ---- */

  if(security_mode_complete->imeisv_exists)
  {   
    /* ---- Encode IMEISV Mobile Identity IEI ---- */
    num_bits_to_write = 8;
    b_packb((byte)MS_IDENTITY_IEI,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write;    

    /* -- Length -- */
    num_bits_to_write = 8;
    b_packb(security_mode_complete->imeisv_id.length,ota_buf,*pos,num_bits_to_write);  
    *pos += num_bits_to_write;    

    /* --- IMEISV Value (array of 9 bytes) --- */
    for(index =0;index <(security_mode_complete->imeisv_id.length); index++)
    {
      b_packb(security_mode_complete->imeisv_id.value[index],ota_buf,*pos,num_bits_to_write);  
      *pos += num_bits_to_write;
    }/*End of for()*/
  }/*End of if()*/
}

/*===========================================================================

FUNCTION  ENCODE_ATTACH_COMPLETE

DESCRIPTION
  This function encodes the ATTACH_COMPLETE message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_attach_complete
(
  byte *ota_buf,
  size_t ota_buf_len,
  lte_nas_outgoing_msg_type *out_buf,
  word *pos
)
{
  send_nas_ota_msg_log_packet(LOG_LTE_NAS_EMM_SEC_OTA_OUT_MSG_LOG_C,(word)(*pos/8),ota_buf + *pos/8);
  encode_esm_msg(out_buf,ota_buf,ota_buf_len,pos,FALSE);
}

/*===========================================================================

FUNCTION  ENCODE_MO_DETACH_REQUEST

DESCRIPTION
  This function encodes the MO_DETACH_REQUEST message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_mo_detach_request(byte *ota_buf, lte_nas_outgoing_msg_type *out_buf, word *pos)
{
  byte num_bits_to_write = 4;
  lte_nas_emm_mo_detach_req_type *detach_req;
  detach_reason = 0;

  detach_req = &out_buf->emm_outgoing_msg.mo_detach_req;

  /*--------------------------------------------------------
                 NAS key set identifier
  ----------------------------------------------------------*/ 
  num_bits_to_write = 1;
  b_packb((byte)detach_req->nas_key_set_identifier.security_context,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 
  num_bits_to_write = 3;
  b_packb((byte)detach_req->nas_key_set_identifier.keyset_id,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;

  /*--------------------------
          Detach Type 
  ---------------------------*/

  /*---------Switch off ------*/
  num_bits_to_write = 1;
  b_packb((byte)detach_req->detach_req.detach_reason,ota_buf,*pos,num_bits_to_write);  
  detach_reason = (byte)detach_req->detach_req.detach_reason;
  *pos += num_bits_to_write;

  /*-------Type of detach-------*/
  num_bits_to_write = 3;
  b_packb((byte)detach_req->detach_req.mo_detach,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write;


  /*--------------------------------
          GUTI/IMSI
  ---------------------------------*/

  encode_eps_mobile_identity(&detach_req->mobile_id,ota_buf,pos);
} 
/*===========================================================================

FUNCTION  ENCODE_ATTACH_REQ

DESCRIPTION
  This function encodes the ATTACH_REQ message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_attach_req
(
  byte *ota_buf,
  size_t ota_buf_len,
  lte_nas_outgoing_msg_type *out_buf,
  word *pos
)
{
  byte plmn_len =3,num_bits_to_write;
  lte_nas_emm_attach_req_type *attach_req;
  emm_ctrl_data_type        *emm_ctrl_data_ptr;
  byte index =0;
  byte loop =0;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  attach_req = &out_buf->emm_outgoing_msg.attach_req;

  /*--------------------------------------------------------
                 NAS key set identifier
  ----------------------------------------------------------*/ 
  num_bits_to_write = 1;
  b_packb((byte)attach_req->nas_key_set_identifier.security_context,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write; 
  num_bits_to_write = 3;
  b_packb((byte)attach_req->nas_key_set_identifier.keyset_id,ota_buf,*pos,num_bits_to_write);
  *pos += num_bits_to_write;
  /*--------------------------------------------------------
                EPS attach result
  ----------------------------------------------------------*/ 
  num_bits_to_write = 4;
  b_packb((byte)attach_req->eps_attach_req,ota_buf,*pos,num_bits_to_write);  
  *pos += num_bits_to_write;
  /*--------------------------------------------------------
                EPS mobile identity
  ----------------------------------------------------------*/ 
  encode_eps_mobile_identity(&attach_req->eps_mobile_id, ota_buf,pos);
  /*------------------------------------------------------------------
                      UE network capability 
  -------------------------------------------------------------------*/  
  num_bits_to_write  = 8;
  encode_ue_nw_capability_ie(&(attach_req->ue_capability),pos,ota_buf);

  /*------------------------------------------------------------------
                      ESM message container
  -------------------------------------------------------------------*/
  encode_esm_msg(out_buf,ota_buf,ota_buf_len,pos,FALSE);

  /*------------------------------------------------------------------
                 P_TMSI_SIGNATURE (Optional)
  -------------------------------------------------------------------*/
  if(attach_req->ptmsi_signature_exists == TRUE)
  {
    /*---- IEI ---*/
    num_bits_to_write = 8;  
    b_packb(PTMSI_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*--- Value ---*/
    num_bits_to_write = 8;
    for(loop = 0;loop< sizeof(attach_req->ptmsi_signature);loop++)
    {
      b_packb(attach_req->ptmsi_signature[loop],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
  }
  /*------------------------------------------------------------------
                  ADDITIONAL GUTI (Optional)
  -------------------------------------------------------------------*/
  if(attach_req->additional_guti_exists == TRUE)
  {
    /*---- IEI ---*/
    num_bits_to_write = 8;  
    b_packb(GUTI_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*--- Value ---*/
    encode_guti_ie(&attach_req->additional_guti,pos,ota_buf);
  }


  /*------------------------------------------------------------------
                  Last visited registered TAI (Optional)
  -------------------------------------------------------------------*/
  if(attach_req->last_visited_reg_tai_exists == TRUE)
  {
    /*-------- IEI ---------*/
    num_bits_to_write = 8;  
    b_packb(LAST_VISITED_REGISTERED_TAI_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    for(index =0; index <plmn_len;index++)
    {
      b_packw(attach_req->last_visited_reg_tai.plmn.identity[index],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
    /* ---- TAC Value ---- */
    num_bits_to_write = 16;
    b_packw(attach_req->last_visited_reg_tai.tac,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
  }
  /*------------------------------------------------------------------
                        DRX parameter (Optional)
  -------------------------------------------------------------------*/
  if(attach_req->drx_param_exists == TRUE)
  {
    /*-------- IEI ---------*/
    num_bits_to_write = 8;  
    b_packb(DRX_PARAM_IEI,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;
    /*-------- VALUE ---------*/
     b_packb(attach_req->drx_param.value[0],ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;

     b_packb(attach_req->drx_param.value[1],ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;
  }  
  
  /*------------------------------------------------------------------
                     MS network capability (Optional)
  -------------------------------------------------------------------*/
  /* -------------------------------
  ** Set MS network capability field
  ** ------------------------------- */
  if(attach_req->ms_nw_capability_exists == TRUE)
  {
    /*-------- IEI ---------*/
    num_bits_to_write = 8;
    b_packb((byte)MS_NW_CAPABILITY_IEI,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;
    /*-------- LENGTH ---------*/
    b_packb((byte)attach_req->ms_nw_capability.length,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*-------- VALUE ---------*/
    for (loop = 0; loop < attach_req->ms_nw_capability.length; loop++)
    {
      b_packb((byte)attach_req->ms_nw_capability.value[loop],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }    
  }
  /*------------------------------------------------------------------
                  Location area identification (Optional)
  -------------------------------------------------------------------*/
  if(attach_req->locn_area_id_exists == TRUE)
  {
    /*-------- IEI ---------*/
    num_bits_to_write = 8;  
    b_packb(LOCN_AREA_IDENTIFICATION_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*-------- PLMN ---------*/
    for(index =0; index <plmn_len;index++)
    {
      b_packb(attach_req->locn_area_id.PLMN_id.identity[index],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
    /* ---- TAC Value ---- */
    num_bits_to_write = 8;
    b_packb(attach_req->locn_area_id.location_area_code[0],ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    b_packb(attach_req->locn_area_id.location_area_code[1],ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
  }

  /*------------------------------------------------------------------
                         TMSI status (Optional)
  -------------------------------------------------------------------*/
  if(attach_req->tmsi_status_exists == TRUE)
  {
    /*-------- IEI ---------*/
    num_bits_to_write = 4;
    b_packb(TMSI_STATUS_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;

    /*3 spare bits*/
    num_bits_to_write = 3;
    b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
    *pos += num_bits_to_write;

    /*TMSI Flag*/
    num_bits_to_write = 1;
    b_packb(attach_req->tmsi_status,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
  }

  /*------------------------------------------------------------------
                    Mobile station classmark 2(Optional)
  -------------------------------------------------------------------*/
  if(attach_req->classmark_2_exists == TRUE)
  {
    /*-------- IEI ---------*/
    num_bits_to_write = 8;
    b_packb(CLASS_MARK_2_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*-------- LENGTH ---------*/
    num_bits_to_write = 8;
    b_packb(attach_req->classmark_2.length,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*-------- VALUE ---------*/
    for(index=0;index < sizeof(attach_req->classmark_2.data); index++)
    {
      b_packb(attach_req->classmark_2.data[index],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
  }
  
  /*------------------------------------------------------------------
                    Mobile station classmark 3(Optional)
  -------------------------------------------------------------------*/
  if(attach_req->classmark_3_exists == TRUE)
  {
    /*-------- IEI ---------*/
    num_bits_to_write = 8;
    b_packb(CLASS_MARK_3_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*-------- LENGTH ---------*/
    num_bits_to_write = 8;
    b_packb(attach_req->classmark_3.length,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*-------- VALUE ---------*/
    for(index=0;index < attach_req->classmark_3.length; index++)
    {
      b_packb(attach_req->classmark_3.data[index],ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
  }

  /*------------------------------------------------------------------
                    Supported Codecs (Optional)  
  -------------------------------------------------------------------*/
  if(attach_req->supported_voice_codec_exists == TRUE)
  {
    /*-------- IEI ---------*/
    num_bits_to_write = 8;
    b_packb(SUPPORTED_CODECS_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*-------- VALUE ---------*/
    if(attach_req->supported_voice_codec.length <=  MAX_NUM_CODECS)
    {
      encode_emm_supported_codecs(&attach_req->supported_voice_codec,ota_buf,pos);
    }
    else
    {
      msg_lib_encode_error = TRUE;
    }
  }
  /*------------------------------------------------------------------
                     ADDITIONAL_UPDATE (Optional) 
  -------------------------------------------------------------------*/
  if(attach_req->additional_update_exists == TRUE)
  {
    /*-------- IEI ---------*/
    num_bits_to_write = 4;
    b_packb(ADDITIONAL_UPDATE_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
    /*-------- Skip 3 bits ---------*/
    num_bits_to_write = 3;
    *pos += num_bits_to_write;
    /*-------- VALUE ---------*/
    num_bits_to_write = 1;
    b_packb(attach_req->additional_update,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
  }
#ifdef FEATURE_LTE_REL9
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
  {
    if(attach_req->voice_domain_pref_exists == TRUE)
    {
      /*-------- IEI ---------*/
      num_bits_to_write = 8;
      b_packb(VOICE_DOMAIN_PREF_IEI,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;   

      /*-------- Length ---------*/      
      b_packb((byte)1,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;   

      /*-------- skip 5 bytes---------*/  
      num_bits_to_write = 5;    
      *pos += num_bits_to_write;   

      /*-------- UE usage settings ---------*/      
      num_bits_to_write = 1;
      b_packb((byte)attach_req->voice_domain_pref.ue_usage_settings,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;   
      /*-------- Voice domain Preference ---------*/      
      num_bits_to_write = 2;
      b_packb((byte)attach_req->voice_domain_pref.voice_pref_for_utran,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;      
    }  
  }
#endif

#ifdef FEATURE_LTE_REL10
    /* Device Properties IE */
    if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
       (reg_sim_per_subs_chk_low_pri_cfg((sys_modem_as_id_e_type)mm_sub_id) == TRUE))
    {
      MSG_HIGH_DS_1(MM_SUB, "Low priority indicator value = %d in ATTACH REQ",
                    attach_req->low_priority_indicator);
      encode_emm_device_properties_ie(&(attach_req->low_priority_indicator), ota_buf, pos);
    }
#endif

  /*----------------------------------------------------
                    GUTI TYPE IEI (Optional)
   ----------------------------------------------------*/
  if(emm_ctrl_data_ptr->exclude_identity_type_ie != TRUE)
  {
    if(attach_req->guti_type_exists == TRUE)
    {     
      /*-------- IEI ---------*/
      num_bits_to_write = 4;
      b_packb(GUTI_TYPE_IEI,ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
      
      /*3 spare bits*/
      num_bits_to_write = 3;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += num_bits_to_write;
  
      /*GUTI type*/
      num_bits_to_write = 1;
      b_packb((byte)(attach_req->guti_type),ota_buf,*pos,num_bits_to_write); 
      *pos += num_bits_to_write;
    }
  }

  /*----------------------------------------------------
                    MS network feature support (Optional)
   ----------------------------------------------------*/
#ifdef FEATURE_LTE_REL10
  if(attach_req->ms_nw_feature_support_exists == TRUE)
  {
    encode_ms_nw_feature_support(&(attach_req->ext_periodic_timer_support),ota_buf,pos);
  }
#endif

#ifdef FEATURE_LTE_REL11
   /*------------------------------------------------------------------
                         TMSI NRI (Optional)
  -------------------------------------------------------------------*/
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)&&
     (attach_req->tmsi_based_nri_exists == TRUE))
  {
    /*-------- IEI ---------*/
    num_bits_to_write = 8;  
    b_packb(TMSI_BASED_NRI_IEI,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;

    /*length of NRI container contents*/
    num_bits_to_write = 8;
    b_packb(attach_req->tmsi_based_nri.length,ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;

    /*NRI container value-TMSI bits 23-16*/
    num_bits_to_write = 8;
    b_packb(attach_req->tmsi_based_nri.tmsi_based_nri_value[0],ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;

    /*NRI container value-TMSI bits 15-14*/
    num_bits_to_write = 8;
    b_packb(attach_req->tmsi_based_nri.tmsi_based_nri_value[1],ota_buf,*pos,num_bits_to_write); 
    *pos += num_bits_to_write;
  }
#endif
} /*End of encode_attach_req()*/

/*===========================================================================

FUNCTION  ENCODE_EMM_MSG

DESCRIPTION
  This function calls the appropriate encoder to encode the EMM message 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void encode_emm_msg
(
  lte_nas_outgoing_msg_type *out_buf,
  byte *ota_buf,
  size_t ota_buf_len,
  word *pos
)
{
  byte num_bits_to_write = 4,pd_offset = 4;
  byte plain_nas_first_two_octets = 16;
  byte plain_nas_first_octet = 8;
  byte emm_msg_id_offset = 8; /*Messag ID is 2nd octet, so skip the first Octet 
                                containing security header (4 bits) + protocol discriminator (4 bits)*/

#ifdef FEATURE_LTE_REL9
  emm_ctrl_data_type        *emm_ctrl_data_ptr;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif

  service_request = FALSE;
  /*--------------------------------------------------------
                  PROTOCOL DISCRIMINATOR
  ----------------------------------------------------------*/
  b_packb((byte)out_buf->nas_hdr.pd,ota_buf,(word)(*pos+ pd_offset),num_bits_to_write);  
  
  /*--------------------------------------------------------
                     SECURITY HEADER
  ----------------------------------------------------------*/
  num_bits_to_write = 4;

  switch(out_buf->nas_hdr.msg_id)
  {
    case ATTACH_REQUEST:
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)ATTACH_REQUEST,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += plain_nas_first_two_octets;
      encode_attach_req(ota_buf, ota_buf_len, out_buf, pos);
      break;

    case ATTACH_COMPLETE:
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)ATTACH_COMPLETE,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += plain_nas_first_two_octets;
      encode_attach_complete(ota_buf, ota_buf_len, out_buf, pos);
      break;

    case DETACH_REQUEST:                
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)DETACH_REQUEST,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += plain_nas_first_two_octets;
      encode_mo_detach_request(ota_buf,out_buf,pos);
      break;

    case DETACH_ACCEPT:
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)DETACH_ACCEPT,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += plain_nas_first_two_octets;
      break;

    case SECURITY_MODE_COMPLETE:
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)SECURITY_MODE_COMPLETE,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += plain_nas_first_two_octets;
      encode_security_mode_complete(ota_buf,out_buf,pos);
      break;

    case SECURITY_MODE_REJECT:
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)SECURITY_MODE_REJECT,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += plain_nas_first_two_octets;
      encode_security_reject(ota_buf,out_buf,pos);
      break;

    case EMM_AUTHENTICATION_FAILURE:                 
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)EMM_AUTHENTICATION_FAILURE,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += plain_nas_first_two_octets;
      encode_authentication_failure(ota_buf,out_buf,pos);
      break;

    case EMM_AUTHENTICATION_RESPONSE:
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)EMM_AUTHENTICATION_RESPONSE,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += plain_nas_first_two_octets;
      encode_authentication_response(ota_buf,out_buf,pos);
      break;

    case TRACKING_AREA_UPADTE_REQUEST:                 
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)TRACKING_AREA_UPADTE_REQUEST,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      encode_tau_request(ota_buf,out_buf,pos);
      break;

    case TRACKING_AREA_UPDATE_COMPLETE:                 
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)TRACKING_AREA_UPDATE_COMPLETE,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += plain_nas_first_two_octets;
      break;

    case EMM_IDENTITY_RESPONSE:    
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)EMM_IDENTITY_RESPONSE,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += plain_nas_first_two_octets;
      encode_identity_resp(ota_buf,out_buf,pos);
      break;

    case GUTI_REALLOCATION_COMPLETE:    
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)GUTI_REALLOCATION_COMPLETE,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += plain_nas_first_two_octets;
      break;

    case EMM_STATUS:
      /* Security header type */
      b_packb((byte)0,ota_buf,*pos,4);
      /* Message type */
      b_packb((byte)EMM_STATUS, ota_buf,(word)(*pos + emm_msg_id_offset),8);
      *pos += plain_nas_first_two_octets;
      /* EMM cause */
      b_packb((byte)out_buf->emm_outgoing_msg.mo_emm_status.emm_cause, 
              ota_buf, *pos, 8);
      *pos += 8;
      break;
    case UPLINK_NAS_TRANSPORT:
      /* Security header type */
      b_packb(0, ota_buf, *pos, 4);
      /* Message type */
      b_packw((byte)UPLINK_NAS_TRANSPORT,
              ota_buf,
              (word)(*pos + emm_msg_id_offset),
              8);
      *pos += plain_nas_first_two_octets;
      /* Msg container */
      b_packw(out_buf->emm_outgoing_msg.ul_nas_transport.nas_msg_container.msg_len,
              ota_buf,
              *pos,
              8);
      *pos += 8;
      memscpy((void*)(ota_buf + *pos/8),
              ota_buf_len - (*pos/8),
              (void*)&out_buf->emm_outgoing_msg.ul_nas_transport.nas_msg_container.msg_payload[0],
              out_buf->emm_outgoing_msg.ul_nas_transport.nas_msg_container.msg_len);
      *pos += (out_buf->emm_outgoing_msg.ul_nas_transport.nas_msg_container.msg_len * 8);
      break;
    case EXT_SERVICE_REQUEST:
      /*--------------------------------------------------------
                         MESSAGE ID
      ----------------------------------------------------------*/
      num_bits_to_write = 8;
      b_packw((byte)EXT_SERVICE_REQUEST,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
      num_bits_to_write = 4;
      b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
      *pos += plain_nas_first_two_octets;
      encode_ext_serv_req(ota_buf,
                                           out_buf,
                                           pos
#ifdef FEATURE_LTE_REL9
                                           ,emm_ctrl_data_ptr
#endif
                                          );
      break;
#if defined (FEATURE_LTE_REL9)
    case UL_GENERIC_NAS_TRANSPORT:
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
      {
        /*--------------------------------------------------------
                           MESSAGE ID
        ----------------------------------------------------------*/
        num_bits_to_write = 8;
        b_packw((byte)UL_GENERIC_NAS_TRANSPORT,ota_buf,(word)(*pos + emm_msg_id_offset),num_bits_to_write);
        num_bits_to_write = 4;
        b_packb((byte)0,ota_buf,*pos,num_bits_to_write);
        *pos += plain_nas_first_two_octets;
        encode_ul_generic_nas_transport(ota_buf, ota_buf_len, out_buf, pos);
      }
      break;
#endif

    default:  
      /*Check for Service request (with security header masked with xF0)*/
      if(out_buf->emm_outgoing_msg.service_req.security_hdr == SERV_REQ)
      {
        num_bits_to_write = 4;
        b_packb((byte)out_buf->emm_outgoing_msg.service_req.security_hdr,ota_buf,*pos,num_bits_to_write);
        *pos += plain_nas_first_octet;
        encode_service_request(ota_buf,out_buf,pos);
        service_request = TRUE;
      }
      break;
  }/*End of switch(out_buf->nas_hdr.msg_id)*/
}/*End of encode_emm_msg()*/

#endif /*FEATURE_LTE*/
