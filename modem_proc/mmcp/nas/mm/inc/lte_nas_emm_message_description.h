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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/lte_nas_emm_message_description.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when         who                          what                              
--------     --------  ------------------------------------------------ 
10/21/11       c_cogolt  Added LTE_NAS_EMM_TMGI_MBMS to the enum mobile_id_type
08/18/11       abhi      Added supprot for GUTI type IE 
01/31/11       rajesh  voice domain preference and UE usage setting related changes
02/15/11       abhi      Deleted lte_nas_emm_add_update_result_type; used sys_lte_cs_capability_e_type 
04/09/10       zren     Added combined procedure feature
12/18/09       RI       Merged UE security capability to UE network capability.
10/06/09       hnam     Added a spare bit to support the security context
09/01/09       hnam     Added the support for EMM Information message (time stamp)
06/15/09       hnam     Equated the value of INTEGRITY_ALGO_128_EIA1 to 1 
05/18/09       hnam     Deleted "lte_nas_emm_identity_type"; used mobile_identity_type instead
04/29/09       hnam     Added "lte_nas_emm_eps_update_val_type" to support TAU request
04/29/09       hnam     Added "lte_nas_emm_eps_update_result_type" parameter -  used in TAU Accept
04/27/09       hnam     Removed "lte_nas_emm_security_hdr_type", as it is not necessary.
04/24/09       MNK      Removed lte_nas_eps_bearer_context_status_type and moved to lte_nas_common.h
04/14/09       hnam     changed eps context values array indexes, to match with that of the document.
04/14/09       hnam     Added length field to lte_nas_eps_bearer_context_status_type.
04/14/09       hnam     Added length to ue_nw_capability structure.
03/12/09       hnam     Added lte_nas_emm_imeisv_type, required for security mode complete.

03/04/2009     hnam     Added lte_nas_emm_drx_param_type into mobile_id_lst_type & added
                         lte_nas_emm_ciphering_alg_type,lte_nas_emm_integrity_protect_alg_type,
                         lte_nas_emm_security_alg_type,lte_nas_emm_imiesv_req_type,
                         lte_nas_emm_ksi_seqnum_type,lte_nas_msg_auth_code_type,
                         lte_nas_emm_ue_security_type,lte_nas_emm_umts_auth_challenge_type,
                         lte_nas_emm_auth_resp_type,lte_nas_emm_supported_codecs,
                         lte_nas_emm_supported_voice_codec_type 

12/15/2008     hnam     Added lte_nas_emm_ue_ntwk_capability_type,                        
                         lte_nas_emm_high_low_priority_classmark_type,                                                
                         lte_nas_emm_gen_ntrk_char_classmark_type


==============================================================================*/

/*Source:  //source/qcom/qct/modem/lte/NAS/MSG_LIB/main/latest/inc/lte_nas_emm_message_description.h */


/*==============================================================================*/
#ifndef _LTE_NAS_EMM_MESSAGE_DESCRIPTION_H_
#define _LTE_NAS_EMM_MESSAGE_DESCRIPTION_H_

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "sys.h"
#include "comdef.h"
#include "nas_exp.h"
#include "lte_rrc_ext_msg.h"
#include "emm_ext_msg.h"
#include "sys_cnst_v.h"

#define MAX_NO_OF_PLMNs 15 /*There can be a maximum of 15 PLMNs 
                             Refer Doc: 3GPP TS 24.008 [Version 6] : Section 10.5.1.13*/

#define MAX_NO_OF_IMSI 21 /* there can be a maximum of 10 octets containing identity digits,
                             Refer section: 9.9.3.10.2*/

#define MAX_NO_OF_IDENTITY_DIGITS 9 /* Maximum number of identity digits  8.5 octet, 
                                       Refer Doc: 3GPP TS 24.008 [Version 6] : Section 10.5.1.4*/

#define SIZE_OF_UMTS_AUTH_CHALLENGE_PARAM 16

#define SIZE_OF_AUTH_RESP 16

#define NUM_IDENTITY_DIGITS 17

#define NO_KEY_AVAIALABLE 7

#define MAX_EMERGENCY_NUM_DIGITS 22

#define SIZE_OF_EMERGENCY_SERVICE_TYPE 8

#define MAX_NUM_CODECS 5

#define MAX_NW_NAME_LEN 251 /*Maximum number of octets in a Layer 3 message (Ref: 44.006)*/

#define MAX_EMERGENCY_INFO 16 /* 16 = (50 -2)3. where 2 -> Length and IEI and 3 is minimum length of one emergency list*/

#define MAX_CLI_DIGITS 19 /*Figure 10.5.93/3GPP TS 24.008 Calling party BCD number information element
                            "Number of Digits" starts from 5th octet, there could be MAX 14 octets, so there could be MAX 
                             9 octets (i.e. 9*2 digits) and 1 digit (last octet contains only 1 digit)*/

#define MAX_SIZE_OF_UE_NE_CAPABILITY_IE 14 /*1 byte for length and 13 bytes for value*/


//#define FEATURE_LTE_3GPP_REL8_SEP09


/*=========================================================================================================*/ 
/*                        Define all the Information elements for Attach Accept                            */
/*=========================================================================================================*/

/* define a message type for lte_emm_eps_attach_result_type*/

typedef enum
{
  TA_UPDATING = 0,
  COMBINED_TA_LA_UPDATING = 1,
  COMBINED_TA_LA_UPDATING_IMSI_ATTACH = 2,
  PERIODIC_UPDATING  = 3

}lte_nas_emm_eps_update_val_type;

typedef struct
{
  boolean                          active_flag;
  lte_nas_emm_eps_update_val_type  eps_update_val;

}lte_nas_emm_eps_update_type;

typedef enum
{
  TA_UPDATE = 0,
  TA_LA_UPDATE = 1,
  TA_UPDATE_ISR_ATIVATE = 4,
  TA_LA_UPDATE_ISR_ACTIVATE = 5

}lte_nas_emm_eps_update_result_type;

typedef enum {
  EPS_ONLY_ATTACH_RESULT=1,
  COMBINED_EPS_IMSI_ATTACH_RESULT=2
}lte_nas_emm_eps_attach_result_type;

/* define a message type for lte_emm_eps_attach_type*/
typedef enum {
  EPS_ONLY_ATTACH = 1,
  COMBINED_EPS_IMSI_ATTACH,
  EPS_EMERGENCY_ATTACH = 6
}lte_nas_emm_attach_type;

typedef enum{
  PLAIN_NAS_MSG                            = 0,
  INTEGRITY_PROTECT                        = 1,
  INTEGRITY_PROTECT_CIPHER                 = 2,
  INTEGRITY_PROTECT_SECURITY_MODE_CMD      = 3,
  INTEGRITY_PROTECT_SECURITY_MODE_COMPLETE = 4,
  SERV_REQ                                 = 12,
  RESERVED_SECURITY                        = 13

}lte_nas_security_hdr_type;

/*Refer section: 9.9.3.19*/

typedef struct 
{
  boolean        security_context;
  byte           keyset_id;
}lte_nas_emm_keyset_identifier_type;

/* Refer section: 9.9.3.6 (Network to UE)*/
typedef enum
{  
  INVALID_MT_DETACH_TYPE, 
  REATTACH_REQUIRED              = 1,
  REATTACH_NOT_REQUIURED         = 2,
  IMSI_DETACH_MT_DETACH          = 3,
  RESERVED_MT_DETACH_TYPE_ONE    = 6,
  RESERVED_MT_DETACH_TYPE_TWO    = 7

}lte_nas_emm_mt_detach_type;

/* Refer section: 9.9.3.6.1 (UE to Network); Required for lte_nas_emm_mo_detach_type*/
typedef enum
{
  INVALID_DETACH_TYPE,
  EPS_DETACH              = 1,
  IMSI_DETACH_MO_DETACH   = 2,
  COMBINED_EPS_DETACH     = 3

}emm_mo_detach_type;

/* Required for lte_nas_emm_mo_detach_type*/
typedef enum
{
  NORMAL_DETACH = 0,
  SWITCH_OFF    = 1,
  INVALID_DETACH

}emm_detach_reason_type;

/* Refer section: 9.9.3.6 : required for Detach type*/
typedef struct
{
    emm_mo_detach_type      mo_detach;
    emm_detach_reason_type  detach_reason;

}lte_nas_emm_mo_detach_type;

/*Define PLMN list, Refer Doc: 3GPP TS 24.008 [Version 6] : Section 10.5.1.13*/
typedef struct 
{
  byte                  length_of_plmn_lst_contents;
  sys_plmn_id_s_type    plmn[MAX_NO_OF_PLMNs];

} lte_nas_emm_plmn_lst_type;

/*Required for emm_GUTI_type*/
typedef enum{
  IMSI_ID = 1,
  IMEI_ID = 3,
  GUTI_ID = 6
}ue_id_type;

/*Define GUTI (Mobile Identity), Ref section: 9.9.3.10.1*/
typedef struct
{
  ue_id_type          ue_id;
  sys_plmn_id_s_type  plmn;
  byte                mme_group_id[2]; /* Length of Mobile Identity Group ID = 2, Ref section: 9.9.3.10.1*/
  byte                mme_code;
  byte                m_tmsi[4];       /* Length of Mobile Identity M-TMSI = 4, Ref section: 9.9.3.10.1*/
} lte_nas_emm_guti_type;

/* Define IMS (Mobile Identity), Ref section: 9.9.3.10.2*/
typedef struct { 
    byte                num_id_digits_len;
    byte                num_id_digits[MAX_NO_OF_IMSI]; 
}lte_nas_emm_imsi_type;

typedef struct
{
  byte length;
  byte value[MAX_IMEI_LEN];
}lte_nas_emm_imei_type;

/*Define Union for IMSI and GUTI type; (Required for OLD_GUTI_type in Attach request*/
typedef union{
  lte_nas_emm_guti_type  guti;
  lte_nas_emm_imsi_type  imsi;
  lte_nas_emm_imei_type  imei;
}ue_id_info_type;

/*Define OLD_GUTI_type ; Refer secttion: Attach Request.*/
typedef struct{
    ue_id_type       ue_id;
    ue_id_info_type  ue_id_info;
}lte_nas_emm_eps_mobile_id_type;

typedef word location_area_code_type;

typedef struct
{
  sys_plmn_id_s_type plmn ;
  word               location_area_code ; /* Length of location area code = 3, 
                                             Refer Doc: 3GPP TS 24.008 [Version 6] : Section 10.5.1.3*/
} lte_nas_emm_loc_area_id_type ;


typedef struct {
  boolean    eps_enc_exists;
  boolean    eps_encrypt_alg_128_eea0_supported; 
  boolean    eps_encrypt_alg_128_eea1_supported; 
  boolean    eps_encrypt_alg_128_eea2_supported;
  boolean    eps_encrypt_alg_eea3_supported;
  boolean    eps_encrypt_alg_eea4_supported;
  boolean    eps_encrypt_alg_eea5_supported;
  boolean    eps_encrypt_alg_eea6_supported;
  boolean    eps_encrypt_alg_eea7_supported;
  boolean    eps_int_exists;
  boolean    spare_bit;
  boolean    eps_integrity_alg_128_eia1_supported; 
  boolean    eps_integrity_alg_128_eia2_supported; 
  boolean    eps_integrity_alg_128_eia3_supported; 
  boolean    eps_integrity_alg_128_eia4_supported; 
  boolean    eps_integrity_alg_128_eia5_supported; 
  boolean    eps_integrity_alg_128_eia6_supported; 
  boolean    eps_integrity_alg_128_eia7_supported; 
  boolean    umts_enc_exists;
  boolean    umts_encrypt_alg_uea0_supported; 
  boolean    umts_encrypt_alg_uea1_supported;
  boolean    umts_encrypt_alg_uea2_supported;
  boolean    umts_encrypt_alg_uea3_supported;
  boolean    umts_encrypt_alg_uea4_supported;
  boolean    umts_encrypt_alg_uea5_supported;
  boolean    umts_encrypt_alg_uea6_supported;
  boolean    umts_encrypt_alg_uea7_supported;
  boolean    umts_int_exists;
  boolean    ucs2_supported; 
  boolean    umts_integrity_alg_uia1_supported; 
  boolean    umts_integrity_alg_uia2_supported;
  boolean    umts_integrity_alg_uia3_supported;
  boolean    umts_integrity_alg_uia4_supported;
  boolean    umts_integrity_alg_uia5_supported;
  boolean    umts_integrity_alg_uia6_supported;
  boolean    umts_integrity_alg_uia7_supported;
  boolean    gprs_enc_exists;
  boolean    gprs_encrypt_alg_1_supported; 
  boolean    gprs_encrypt_alg_2_supported; 
  boolean    gprs_encrypt_alg_3_supported; 
  boolean    gprs_encrypt_alg_4_supported; 
  boolean    gprs_encrypt_alg_5_supported; 
  boolean    gprs_encrypt_alg_6_supported; 
  boolean    gprs_encrypt_alg_7_supported; 

}lte_nas_emm_ue_security_type;

typedef struct {  
  lte_nas_emm_ue_security_type ue_sec_capability;
  boolean    onexsrvcc_capability_supported;
  boolean    isr_supported; 
#ifdef FEATURE_LTE_REL9 
  boolean    lte_positioning_prot_supported;
  boolean    lcs_notification_supported;
  boolean    notification_proc_supported;
#endif
}lte_nas_emm_ue_capability_type;

/*Define lte_nas_emm_rf_gsm_t_gsm_type 
Reqd for lte_nas_rf_capability_type*/
typedef enum
{
   RF_CLASS_1_GSM_TGSM,
   RF_CLASS_2_GSM_TGSM,
   RF_CLASS_3_GSM_TGSM,
   RF_CLASS_4_GSM_TGSM,
   RF_CLASS_5_GSM_TGSM

}lte_nas_emm_gsm_tgsm_rf_type;

/*Define lte_nas_emm_rf_gsm_t_gsm_type 
Reqd for lte_nas_rf_capability_type*/
typedef enum
{
   RF_CLASS_1_GSM,
   RF_CLASS_2_GSM,
   RF_CLASS_3_GSM

}lte_nas_emm_gsm_rf_type;

/*Define lte_nas_emm_umts_rf_type 
Reqd for lte_nas_rf_capability_type*/
typedef enum
{
   UMTS_RF_TYPE = 7

}lte_nas_emm_umts_rf_type;

/*Define lte_nas_rf_capability_type 
Reqd for lte_nas_emm_high_low_priority_classmark_type*/
typedef union
{
   lte_nas_emm_gsm_tgsm_rf_type   gsm_t_gsm_rf;
   lte_nas_emm_gsm_rf_type        gsm_rf;
   lte_nas_emm_umts_rf_type       umts_rf;

}lte_nas_rf_capability_type;

/*Define lte_nas_ntwrk_rev_level_type 
Reqd for lte_nas_emm_high_low_priority_classmark_type*/
typedef enum
{
   GSM_PHASE_1_MOBILE_STATIONS,
   GSM_PHASE_2_MOBILE_STATIONS,
   R99_MOBILE_STATIONS,
   RESERVED_FOR_FUTURE_VERSIONS

}lte_nas_nw_rev_level_type;

/*Define lte_nas_emm_suppl_serv_screening_type 
Reqd for lte_nas_emm_high_low_priority_classmark_type
Ref Doc: 24.080; Section: 3.7.1*/
typedef enum
{
   DEFAULT,
   ELLIPTIC,
   RESERVED_FOR_FUTURE_SUPPL_SERV_SCREENING

}lte_nas_emm_suppl_serv_screening_type;

/*Define lte_nas_emm_freq_compatibility_type 
Reqd for lte_nas_emm_high_low_priority_classmark_type
typedef union
{
}lte_nas_emm_freq_compatibility_type;*/

/*Define lte_nas_emm_high_low_priority_classmark_type*/ /*Classmark 2*/
/*Ref doc: 24.008 v8; section: 10.5.1.6 */
typedef struct {

    lte_nas_rf_capability_type                        rf_capability;
    boolean                                           a5_1_alg_supported;
    boolean                                           ctrl_early_classmark_supported;
    lte_nas_nw_rev_level_type                         ntwrk_revision_level;
    boolean                                           ps_capability_supported;
    lte_nas_emm_suppl_serv_screening_type             ssc_screening;
    boolean                                           sms_support;
    boolean                                           vbs_capability_notify; 
    boolean                                           vgcs_capability_notify; 
    boolean                                           freq_capability;      /*Frequency compatibility*/
    boolean                                           classmark_3_options_supported;
    boolean                                           loc_req_notification_supported; 
    boolean                                           default_alphbet_over_ucs2_preferred; /*UCS2 treatment */
    boolean                                           a_gb_mode_supported; /*SoLSA*/
    boolean                                           cm_serv_prompt;
    boolean                                           a5_3_alg_supported; 
    boolean                                           a5_2_alg_supported; 

}lte_nas_emm_hi_lo_pri_classmark_type;

typedef enum
{
  ENCRYPTION_ALGO_128_EEA0,
  ENCRYPTION_ALGO_128_EEA1,
  ENCRYPTION_ALGO_128_EEA2,
  ENCRYPTION_ALG_EEA3,
  ENCRYPTION_ALG_EEA4,
  ENCRYPTION_ALG_EEA5,
  ENCRYPTION_ALG_EEA6,
  ENCRYPTION_ALG_EEA7

}lte_nas_emm_ciphering_alg_type;

typedef enum
{
  INTEGRITY_ALGO_128_EIA0,
  INTEGRITY_ALGO_128_EIA1,
  INTEGRITY_ALGO_128_EIA2,
  INTEGRITY_ALGO_EIA3,
  INTEGRITY_ALGO_EIA4,
  INTEGRITY_ALGO_EIA5,
  INTEGRITY_ALGO_EIA6,
  INTEGRITY_ALGO_EIA7
}lte_nas_emm_integrity_protect_alg_type;

typedef struct
{
  lte_nas_emm_ciphering_alg_type         ciphering_alg;
  lte_nas_emm_integrity_protect_alg_type integrity_protect_alg;

}lte_nas_emm_security_alg_type;

typedef enum 
{  
  IMIESV_NOT_REQUESTED,
  IMIESV_REQUESTED
}lte_nas_emm_imiesv_req_type;

typedef struct 
{
  byte             keyset_id;
  byte             sequence_number;

}lte_nas_emm_ksi_seqnum_type;

typedef struct
{
  byte            short_mac_value[2];
}lte_nas_msg_auth_code_type;


typedef struct 
{
  byte length;
  byte val[SIZE_OF_UMTS_AUTH_CHALLENGE_PARAM];

}lte_nas_emm_umts_auth_challenge_type;

typedef struct
{
  byte length;
  byte val[SIZE_OF_AUTH_RESP];

}lte_nas_emm_auth_resp_type;

typedef struct 
{
  byte sys_id;
  byte sys_id_len;
  byte codec_bitmap[2];
}lte_nas_emm_supported_codecs;

typedef struct
{
  byte                         num_codecs;
  lte_nas_emm_supported_codecs supported_codecs[MAX_NUM_CODECS];

}lte_nas_emm_supported_voice_codec_type;

/* Refer Doc: 3GPP TS 24.008 V8.9.0 (2010-03), subsection 10.5.1.4, table 10.5.4/3GPP TS 24.008...Required for emm_ms_identity_type */
/*
Type of identity (octet 3)
Bits
3    2    1    
0    0    1    IMSI
0    1    0    IMEI
0    1    1    IMEISV
1    0    0    TMSI/P-TMSI
1    0    1    TMGI and optional MBMS Session Identity
0    0    0    No Identity note 1)
*/

typedef enum
{
  LTE_NAS_EMM_NO_ID=0,
  LTE_NAS_EMM_IMSI=1,
  LTE_NAS_EMM_IMEI=2,
  LTE_NAS_EMM_IMEISV=3,
  LTE_NAS_EMM_TMSI=4,
  LTE_NAS_EMM_TMGI_MBMS=5
}mobile_id_type;

typedef struct
{
  byte length;
  byte value[9];
}lte_nas_emm_imeisv_type;

typedef struct
{
  byte  length;
  byte  value[4];
}lte_nas_emm_tmsi_type;

/*Required for lte_nas_emm_ms_identity_type*/
typedef union
{
  lte_nas_emm_imsi_type      imsi;
  lte_nas_emm_tmsi_type      tmsi;
  lte_nas_emm_imeisv_type    imeisv;
  lte_nas_emm_imei_type      imei;
  
}mobile_id_lst_type;

/*Define MS Identity , Refer Doc: 3GPP TS 24.008 [Version 6] : Section 10.5.1.4*/
typedef struct 
{
  mobile_id_type      mobile_id;
  mobile_id_lst_type  mobile_id_lst;
   
} lte_nas_emm_ms_id_type;  

typedef enum
{
  GSM_DEFAULT_ALPHABET,
  UCS_2,
  RESERVED_CODING_SCHEME
}lte_nas_emm_coding_scheme_type;

typedef struct
{
  byte                            length;
  byte                            val[MAX_NW_NAME_LEN];
  boolean                         country_initial_req;
  lte_nas_emm_coding_scheme_type  coding_scheme;
}lte_nas_nw_name_type;

typedef struct
{
  byte year;
  byte month;
  byte day;
  byte hour;
  byte minute;
  byte second;
  int time_zone;
}lte_nas_tp_serv_center_time_stamp_type;

typedef enum
{
  NO_ADJUSTMENTS,
  PLUS_ONE_HOUR,
  PLUS_TWO_HOUR,
  RESERVED_VAL
}lte_nas_day_light_savings_type;


typedef enum
{
  MO_CSFB_1xCSFB,
  MT_CSFB_1XCSFB,
  MO_CSFB_ECALL_1XCSFB_ECALL
}lte_nas_service_type;

typedef enum
{
  CSFB_REJECT,
  CSFB_ACCEPT
}lte_nas_csfb_rsp_type;

typedef enum
{
 EMM_CSFB_PAGING_TYPE_IMSI = 0,
 EMM_CSFB_PAGINGI_TYPE_TMSI = 1
}lte_nas_paging_identity_type;

typedef enum
{
  USER_PROVIDED_UNSCREENED,
  USER_PROVIDED_SCREENED,
  USER_PROVIDED_VERIFIED_AND_FAILED,
  NW_PROVIDED
}lte_nas_screen_ind_type;

typedef enum
{
  PRESENTATION_ALLOWED,
  PRESENTATION_RESTRICTED,
  NUMBER_UNAVAILABLE_DUE_TO_INTERWORKING  
}lte_nas_presentation_ind_type;

typedef enum
{
  UNKOWN_NUMBER = 0,
  INTERNATIONAL_NUMBER,
  NATIONAL_NUMBER,
  NW_SPECIFIC_NUMBER,
  DEDICATED_ACCESS_SHORT_CODE
}lte_nas_number_type;

typedef enum
{
  UNKWONN_NUMBERING_PLAN = 0,
  ISDN_TELEPHONY_NUMBERING_PLAN = 1,
  DATA_NUMBERING_PLAN = 3,
  TELEX_NUMBERING_PLAN = 4,
  NATIONAL_NUMBERING_PLAN = 8,
  PRIVATE_NUMBERING_PLAN = 9,
  RESERVED_FOR_CTS = 11  
}lte_nas_numbering_plan_identification_type;

typedef struct
{
  lte_nas_numbering_plan_identification_type numbering_plan_identification;
  lte_nas_number_type                        number_type;
  lte_nas_screen_ind_type                    screen_ind;
  lte_nas_presentation_ind_type              presentation_ind;
  byte                                       number_of_digits;
  byte                                       digits[MAX_CLI_DIGITS];
}lte_nas_calling_line_info_type;

/*Reference: 17.7.5 Supplementary service codes (SPEC 29.002 V9.6.0 (2011-09))*/
typedef enum
{
  ALL_SS_RESERVED = 0,
  ALL_LINE_IDENTIFICATION_RESERVED = 16,
  CALLING_LINE_IDENTIFICATION_PRESENTATION = 17,
  CALLING_LINE_IDENTIFICATION_RESTRICTION = 18,
  CONNECTED_LINE_IDENTIFICATION_PRESENTATION = 19,
  CONNECTED_LINE_IDENTIFICATION_RESTRICTION = 20,
  MALICIOUS_CALL_IDENTIFICATION_RESERVED = 21,
  ALL_NAME_IDENTIFICATION = 24,
  CALLING_NAME_PRESENTATION = 25,
  ALL_FORWARDING = 32,
  CALL_FORWARDING_UNCONDITIONAL = 33,
  ALL_CONDITIONAL_FORWARDING = 40,
  CALL_FWD_MOBILE_SUBSCRIBER_BUSY = 41,
  CALL_FWD_NO_REPLY = 42,
  CALL_FWD_MOBILE_SUBSCRIBER_UNREACHABLE = 43,
  CALL_DEFLECTION = 36,
  ALL_CALL_FORWARDING_RESERVED = 48,
  EXPLICIT_CALL_TRANSFER = 49,
  MOBILE_ACCESS_HUNTING_RESERVED = 50,
  ALL_CALL_COMPLETION_RESERVED = 64,
  SS_CALL_WAITING = 65,
  CALL_HOLD = 66,
  CALL_COMPLETION_TO_BUSY_SUBS_ORIG_SIDE = 67,
  CALL_COMPLETION_TO_BUSY_SUBS_DEST_SIDE = 68,
  MULTI_CALL = 69,
  ALL_MULTY_PARTY_RESERVED = 80,
  MULTI_PARTY = 81,
  ALL_COMMUNITY_INTEREST_RESERVED = 96,
  CLOSED_USER_GROUP = 97,
  ALL_CHARGING_RESERVED = 112,
  ADVICE_OF_CHARGE_INFORNMATION = 113,
  ADVICE_OF_CHARGE_CHARGING = 114,
  ALL_ADDITIONAL_INFORMATION_TRANSFER_RESERVED = 128,
  USER_TO_USER_SIGNALLING_1 = 129,
  USER_TO_USER_SIGNALLING_2 = 130,
  USER_TO_USER_SIGNALLING_3 = 131,
  ALL_BARRING = 144,
  BARRING_OF_OUTGOING_CALLS = 145,
  BARRING_OF_ALL_OUTGOING_CALLS = 146,
  BARRING_OF_ALL_OUTGOING_INTERNATIONAL_CALLS = 147,
  BARRING_OF_ALL_OUTGOING_INT_CALLS_EXCEPT_DIRECTED_TO_HPLMN_COUNTRY = 148,
  BARRING_OF_INCOMING_CALLS = 153,
  BARRING_OF_ALL_INCOMING_CALLS = 154,
  BARRING_OF_ALL_INCOMING_CALL_EXCEPT_ROAMING_OUTSIDE_HPLMN_COUNTRY = 155,
  ALL_CALL_PRIORITY_SS_RESERVED = 160,
  ENHANCED_MULTILEVEL_PRESEDENCE_PREEMPTION_SERVICE = 161,
  ALL_LCS_PRIVACY_EXCEMPTION = 176,
  UNIVERSAL = 177,
  CALL_SESSION_RELATED = 178,
  CALL_SESSION_UNRELATED = 179,
  PLMN_OPERATOR = 180,
  SERVICE_TYPE = 181,
  ALL_MO_LOCATION_REQUEST_CLASS = 192,
  BASIC_SELF_LOCATION = 193,
  AUTONOMOUS_SELF_LOCATION = 194,
  TRANSFER_TO_THIRD_PARTY = 195,
  ALL_PLMN_SPECIFIC_SS = 240,
  PLMN_SPECIFIC_SS_1 = 241,
  PLMN_SPECIFIC_SS_2 = 242,
  PLMN_SPECIFIC_SS_3 = 243,
  PLMN_SPECIFIC_SS_4 = 244,
  PLMN_SPECIFIC_SS_5 = 245,
  PLMN_SPECIFIC_SS_6 = 246,
  PLMN_SPECIFIC_SS_7 = 247,
  PLMN_SPECIFIC_SS_8 = 248,
  PLMN_SPECIFIC_SS_9 = 249,
  PLMN_SPECIFIC_SS_A = 250,
  PLMN_SPECIFIC_SS_B = 251,
  PLMN_SPECIFIC_SS_C = 252,
  PLMN_SPECIFIC_SS_D = 253,
  PLMN_SPECIFIC_SS_E = 254,
  PLMN_SPECIFIC_SS_F = 255  
}lte_nas_supl_serv_code_type;
typedef enum
{
  NORMAL_OR_UNSPECIFIED,
  MT_LR
}lte_nas_lcs_ind_type;

typedef struct
{
  byte  msg_payload[MAX_OCTETS_L3_MESSAGE];
  word  msg_len;

}lte_nas_msg_container_type;

#ifdef FEATURE_LTE_REL9
typedef struct
{
  sys_ue_usage_setting_e_type           ue_usage_settings;
  sys_voice_domain_pref_e_type         voice_pref_for_utran;
}lte_nas_emm_voice_domain_pref_type;

typedef struct
{
  word  container_length;
  byte  *container_value_ptr;
}lte_nas_generic_msg_container;

typedef struct
{
  byte  additional_info_length;
  byte  *additional_info_value_ptr;
}lte_nas_generic_additional_info_type;
#endif

typedef enum
{
  LTE_NAS_GUTI_TYPE_NATIVE = 0,
  LTE_NAS_GUTI_TYPE_MAPPED = 1  
}lte_nas_included_guti_type;

#endif


#endif /*FEATURE_LTE*/
