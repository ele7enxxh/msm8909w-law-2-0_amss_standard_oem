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

This section contains comments describingchanges made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/lte_nas_emm_message_type.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when           who                          what                              
--------     --------  ------------------------------------------------
09/12/11       HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                       as a part of CMI activity 
08/01/11       Rajesh  Support to include EPS bearer context in extended service request
08/18/11       abhi    Added suport for GUTI type IE
12/13/10       abhi    Deleted lte_nas_emm_add_update_result_type; used sys_lte_cs_capability_e_type
05/14/10       RI      Mainlined spec changes upto Dec 09.
04/22/10       zren    Updated to use global structure for NAS outgoing message
01/22/10       RI      Added the NAS KSI field in MO DETACH msg.
01/21/10       zren    Added EMM/ESM STATUS feature
12/16/09       zren    Updated to support GUTI or IMSI as EPS mobile 
                       identification in TAU request message
09/02/09       hnam    Added NAS KASME in TAU ACCEPT
09/01/09       hnam    Added the support for EMM Information message & GUTI reallocation messages
08/13/09       hnam    Added ptmsi and additional guti for ATTACH REQUEST(March spec compliance)
06/09/09       hnam    Added additional_guti in TAU_Request
05/06/09       hnam    Changed "t3412_val" from word to dword (in Attach Accept and TAU Accept)
04/29/09       hnam    Added "lte_nas_emm_eps_update_val_type" to support TAU request
04/29/09       hnam    Added TAU complete
04/29/09       hnam    Added "lte_nas_emm_eps_update_result_type" parameter -  used in TAU Accept
04/27/09       hnam    Removed "lte_nas_emm_security_hdr_type", as it is not necessary.
04/10/09       hnam    Added a boolean flag for eps_bearer_context_status (in TAU accept).
04/10/09       hnam    Changed hdr to lte_nas_hdr, for lte_nas_emm_auth_fail & lte_nas_emm_auth_rej
04/10/09       hnam    Added authentication failure and changed qword to byte[4] (for security mode command)
03/12/09       hnam    Added lte_nas_emm_imeisv_type, required for security mode complete.
03/12/09       hnam    Changed nonce_ue & nonce_uue data type from array of 4 bytes to qword.
02/25/09       hnam    Added service req, service rej, auth req, auth rej, 
                        auth resp, security mode cmd, security mode complete, 
                         security mode reject
12/15/08       hnam    Added lte_nas_emm_ue_ntwk_capability_type,   
                         lte_nas_emm_hi_lo_priority_classmark_type,  
                          classmark_3_ie_T
                          required for Attach request (in the latest version of 3GPP)

==============================================================================*/

/*==============================================================================*/
#ifndef _LTE_NAS_EMM_MESSAGE_TYPE_H_ 
#define _LTE_NAS_EMM_MESSAGE_TYPE_H_ 

#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE

#define SIZE_OF_RAND_AUTH_PARAM 16 
#define MAX_SIZE_AUTH_FAILURE 16

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/


#include "comdef.h"
#include "lte_nas_common_v.h"
#include "lte_nas_emm_message_description.h"
#include "lte_nas_esm_message_type.h"
#include "emm_gps_if_msg.h"
#include "sys_stru_v.h"
#include "cm_gw.h"
#include "gmm_emm_common.h"
#include "reg_mm_v.h"
#include "sys_gw_v.h"
#include "sys.h"
#include "sys_v.h"

 /*Define a structure for Attach accept..*/
typedef struct
{
  lte_nas_hdr_type                     lte_nas_hdr;
  lte_nas_security_hdr_type            security_hdr;
  lte_nas_emm_eps_attach_result_type   eps_attach_result;
  dword                                t3412_val;
#ifdef FEATURE_LTE_REL10
  dword                                t3412_ext_val;
  boolean                              t3412_ext_val_exists;
#endif
  lte_nas_emm_tai_lst_info_type        tai_lst;
  lte_nas_esm_incoming_msg_type        esm_msg_lst;
  boolean                              guti_type_exists;
  lte_nas_emm_guti_type                guti;
  boolean                              loc_area_id_exists;
  inter_task_lai_T                     loc_area_id;  
  boolean                              ms_id_exists;
  gmm_ie_LV_mobile_id_T                ms_id;
  boolean                              emm_cause_exists;
  lte_nas_emm_cause_type               emm_cause;
  boolean                              t3402_val_exists;
  dword                                t3402_val;
  boolean                              t3423_val_exists;
  dword                                t3423_val;
  boolean                              plmn_exists;
  lte_nas_emm_plmn_lst_type            plmn_lst;
  boolean                              emergency_num_list_exists;
  mmr_emergency_num_list_s_type        emergency_num_list;
  boolean                              eps_nw_feature_support_exists;
  sys_nw_feature_support_s_type        eps_nw_feature_support;
  boolean                              additional_update_result_exists;
  sys_lte_cs_capability_e_type         additional_update_result;
} lte_nas_emm_attach_accept_type;

#define PTMSI_SIGNATURE_LEN 3

/*Define a structure for Attach request*/
typedef struct
{
  lte_nas_hdr_type                              lte_nas_hdr;
  lte_nas_security_hdr_type                     security_hdr;
  lte_nas_emm_attach_type                       eps_attach_req;
  lte_nas_emm_keyset_identifier_type            nas_key_set_identifier;
  lte_nas_emm_eps_mobile_id_type                eps_mobile_id; /*OLD GUTI_IMSI*/
  lte_nas_emm_ue_capability_type                ue_capability;
  lte_nas_esm_outgoing_msg_type                 esm_msg_container; 
  boolean                                       ptmsi_signature_exists;
  byte                                          ptmsi_signature[PTMSI_SIGNATURE_LEN];
  boolean                                       additional_guti_exists;
  lte_nas_emm_guti_type                         additional_guti; /*ADDITIONAL GUTI*/
  boolean                                       last_visited_reg_tai_exists;
  lte_nas_tai_lst1_type                         last_visited_reg_tai;
  boolean                                       drx_param_exists;                  
  gmm_ie_TV_drx_parameter_T                     drx_param;
  boolean                                       ms_nw_capability_exists;
  gmm_ie_LV_ms_network_caps_T                   ms_nw_capability;                
  boolean                                       locn_area_id_exists;
  inter_task_lai_T                              locn_area_id;
  boolean                                       tmsi_status_exists;
  boolean                                       tmsi_status;
  boolean                                       classmark_2_exists;
  classmark_2_ie_T                              classmark_2;
  boolean                                       classmark_3_exists; /* To be done*/ 
  classmark_3_ie_T                              classmark_3;
  boolean                                       supported_voice_codec_exists;
  cm_supported_codecs_list_T                    supported_voice_codec;
  boolean                                       additional_update_exists;
  boolean                                       additional_update;
#ifdef FEATURE_LTE_REL9
  boolean                                       voice_domain_pref_exists;
  lte_nas_emm_voice_domain_pref_type            voice_domain_pref;
#endif
  boolean                                       guti_type_exists;
  lte_nas_included_guti_type                    guti_type;
#ifdef FEATURE_LTE_REL10
  boolean                                       ms_nw_feature_support_exists;
  boolean                                       ext_periodic_timer_support;
#endif
#ifdef FEATURE_LTE_REL11
  boolean                                       tmsi_based_nri_exists;
  gmm_ie_tmsi_based_NRI_T                         tmsi_based_nri;
#endif
#ifdef FEATURE_LTE_REL10
  boolean                                       low_priority_indicator;
#endif
} lte_nas_emm_attach_req_type;

/*Define a structure for Attach reject*/
typedef struct
{
  lte_nas_hdr_type               lte_nas_hdr;
  lte_nas_security_hdr_type      security_hdr; 
  lte_nas_emm_cause_type         emm_cause;
  boolean                        esm_msg_exists;
  lte_nas_esm_incoming_msg_type  esm_msg_container;
#ifdef FEATURE_LTE_REL10
  boolean                        t3402_val_exists;
  dword                          t3402_val;
  dword                          t3346_val;
#endif
} lte_nas_emm_attach_rej_type;

/*Define a structure for Attach complete*/
typedef struct
{
  lte_nas_hdr_type              lte_nas_hdr;
  lte_nas_security_hdr_type     security_hdr;
  lte_nas_esm_outgoing_msg_type esm_msg_container;  

} lte_nas_emm_attach_complete_type;

/* Define a structure for Detach accept.*/
typedef struct
{
  lte_nas_hdr_type             lte_nas_hdr;
  lte_nas_security_hdr_type    security_hdr;

} lte_nas_emm_mo_detach_accept_type;

typedef struct
{
  lte_nas_hdr_type             lte_nas_hdr;
  lte_nas_security_hdr_type    security_hdr;
  
}lte_nas_emm_mt_detach_accept_type;

/* Define a structure for Detach request. UE terminated Detach*/ 
typedef struct
{
  lte_nas_hdr_type            lte_nas_hdr;
  lte_nas_security_hdr_type   security_hdr;
  lte_nas_emm_mt_detach_type  detach_req;
  boolean                     emm_cause_exists;
  lte_nas_emm_cause_type      emm_cause;

} lte_nas_emm_mt_detach_req_type;

/* Define a structure for Detach request. UE originating Detach*/
typedef struct
{
  lte_nas_hdr_type                      lte_nas_hdr;
  lte_nas_security_hdr_type             security_hdr;
  lte_nas_emm_mo_detach_type            detach_req;
  lte_nas_emm_keyset_identifier_type    nas_key_set_identifier;
  lte_nas_emm_eps_mobile_id_type        mobile_id; 

} lte_nas_emm_mo_detach_req_type;

typedef struct 
{
  lte_nas_hdr_type                    lte_nas_hdr;
  lte_nas_security_hdr_type           security_hdr;          
  lte_nas_emm_security_alg_type       nas_security_alg;
  lte_nas_emm_keyset_identifier_type  nas_key_set_identifier_asme;
  lte_nas_emm_keyset_identifier_type  nas_key_set_identifier_sgsn;
  lte_nas_emm_ue_security_type        ue_security_capability;
  boolean                             imiesv_req_exists;
  lte_nas_emm_imiesv_req_type         imiesv_req;
  boolean                             nonce_ue_exists;
  byte                                nonce_ue[4];
  boolean                             nonce_mme_exists;
  byte                                nonce_mme[4];

}lte_nas_emm_security_mode_command;

typedef struct
{
  lte_nas_hdr_type                 lte_nas_hdr;
  lte_nas_security_hdr_type        security_hdr;
  boolean                          imeisv_exists;
  lte_nas_emm_imeisv_type          imeisv_id;

}lte_nas_emm_security_mode_complete;

/* Define a structure for service request. Refer section: 8.2.25*/
typedef struct
{
  lte_nas_hdr_type                  lte_nas_hdr;
  lte_nas_security_hdr_type         security_hdr;
  lte_nas_emm_ksi_seqnum_type       ksi_seqnum;
  word                              short_mac_value;

}lte_nas_emm_service_req_type;

/*Define a structure for service reject*/
typedef struct
{
  lte_nas_hdr_type           lte_nas_hdr;
  lte_nas_security_hdr_type  security_hdr;
  lte_nas_emm_cause_type     emm_cause;
  boolean                    t3442_val_exists;
  dword                      t3442_val;
#ifdef FEATURE_LTE_REL10
  dword                      t3346_val;
#endif
} lte_nas_emm_service_rej_type;

typedef struct 
{
  lte_nas_hdr_type                      lte_nas_hdr;
  lte_nas_security_hdr_type             security_hdr; 
  lte_nas_emm_keyset_identifier_type    nas_key_set_identifier_asme;
  byte                                  nas_auth_rand_param[SIZE_OF_RAND_AUTH_PARAM];
  lte_nas_emm_umts_auth_challenge_type  nas_umts_auth_challenge;
  
}lte_nas_emm_auth_req;

typedef struct 
{
  lte_nas_hdr_type            lte_nas_hdr;
  lte_nas_security_hdr_type   security_hdr; 
  lte_nas_emm_auth_resp_type  nas_auth_resp;
}lte_nas_emm_auth_resp;

typedef struct 
{
  lte_nas_hdr_type              lte_nas_hdr;
  lte_nas_security_hdr_type     security_hdr;
  lte_nas_emm_cause_type        emm_cause;

}lte_nas_emm_security_mode_rej;

typedef struct 
{
  lte_nas_hdr_type                lte_nas_hdr;
  lte_nas_security_hdr_type       security_hdr;  
}lte_nas_emm_auth_rej;

typedef struct 
{
  lte_nas_hdr_type              lte_nas_hdr;
  lte_nas_security_hdr_type     security_hdr;  
  lte_nas_emm_cause_type        emm_cause;
  boolean                       auth_fail_param_exists;
  byte                          auth_fail_param[MAX_SIZE_AUTH_FAILURE];
}lte_nas_emm_auth_fail;

typedef struct 
{
  lte_nas_hdr_type                              lte_nas_hdr;
  lte_nas_security_hdr_type                     security_hdr;
  lte_nas_emm_eps_update_type                   eps_update; 
  lte_nas_emm_keyset_identifier_type            nas_key_set_identifier_asme;
  lte_nas_emm_eps_mobile_id_type                eps_mobile_id;
  boolean                                       nas_keyset_identifier_sgsn_exists;
  lte_nas_emm_keyset_identifier_type            nas_key_set_identifier_sgsn;
  boolean                                       ciphering_key_seq_num_exists;
  byte                                          ciphering_key_seq_num;
  boolean                                       ptmsi_signature_exists;
  byte                                          ptmsi_signature[PTMSI_SIGNATURE_LEN];
  boolean                                       additional_guti_exists;
  lte_nas_emm_guti_type                         additional_guti;
  boolean                                       nonce_ue_exists;
  byte                                          nonce_ue[4];
  boolean                                       ue_capability_exists;
  lte_nas_emm_ue_capability_type                ue_capability;
  boolean                                       last_visited_reg_tai_exists;
  lte_nas_tai_lst1_type                         last_visited_reg_tai;
  boolean                                       drx_param_exists;                  
  gmm_ie_TV_drx_parameter_T                     drx_param;
  boolean                                       ue_radio_capability_info_update;
  boolean                                       eps_bearer_context_status_exists;
  lte_nas_eps_bearer_context_status_type        eps_bearer_context_status;
  boolean                                       ms_nw_capability_exists;
  gmm_ie_LV_ms_network_caps_T                   ms_nw_capability;              
  boolean                                       locn_area_id_exists;
  inter_task_lai_T                              locn_area_id;
  boolean                                       tmsi_status_exists;
  boolean                                       tmsi_status;
  boolean                                       classmark_2_exists;
  classmark_2_ie_T                              classmark_2;
  boolean                                       classmark_3_exists; /* To be done*/ 
  classmark_3_ie_T                              classmark_3;
  boolean                                       supported_voice_codec_exists;
  cm_supported_codecs_list_T                    supported_voice_codec;
  boolean                                       additional_update_exists;
  boolean                                       additional_update;
#ifdef FEATURE_LTE_REL9
  boolean                                       voice_domain_pref_exists;
  lte_nas_emm_voice_domain_pref_type            voice_domain_pref;
#endif
  boolean                                       guti_type_exists;
  lte_nas_included_guti_type                    guti_type;
#ifdef FEATURE_LTE_REL10
  boolean                                       ms_nw_feature_support_exists;
  boolean                                       ext_periodic_timer_support;
#endif
#ifdef FEATURE_LTE_REL11
  boolean                                       tmsi_based_nri_exists;
  gmm_ie_tmsi_based_NRI_T                          tmsi_based_nri;
#endif
#ifdef FEATURE_LTE_REL10
  boolean                                       low_priority_indicator;
#endif
}lte_nas_emm_tau_req;


typedef struct
{
  lte_nas_hdr_type                        lte_nas_hdr;
  lte_nas_security_hdr_type               security_hdr;
  lte_nas_emm_eps_update_result_type      eps_update_result;
  boolean                                 t3412_val_exists;
  dword                                   t3412_val;
#ifdef FEATURE_LTE_REL10
  dword                                   t3412_ext_val;
  boolean                                 t3412_ext_val_exists;
#endif
  boolean                                 guti_type_exists;
  lte_nas_emm_guti_type                   guti;
  boolean                                 tai_lst_exists;
  lte_nas_emm_tai_lst_info_type           tai_lst;
  boolean                                 eps_bearer_context_status_exists;
  lte_nas_eps_bearer_context_status_type  eps_bearer_context_status;
  boolean                                 loc_area_id_exists;
  inter_task_lai_T                        loc_area_id;  
  boolean                                 ms_id_exists;
  gmm_ie_LV_mobile_id_T                   ms_id;
  boolean                                 emm_cause_exists;
  lte_nas_emm_cause_type                  emm_cause;
  boolean                                 t3402_val_exists;
  dword                                   t3402_val;
  boolean                                 t3423_val_exists;
  dword                                   t3423_val;
  boolean                                 plmn_exists;
  lte_nas_emm_plmn_lst_type               plmn_lst;
  boolean                                 nas_key_set_identifier_asme_exists;
  lte_nas_emm_keyset_identifier_type      nas_key_set_identifier_asme;
  boolean                                 emergency_num_list_exists;
  mmr_emergency_num_list_s_type           emergency_num_list;
  boolean                                 eps_nw_feature_support_exists;
  sys_nw_feature_support_s_type           eps_nw_feature_support;
  boolean                                 additional_update_result_exists;
  sys_lte_cs_capability_e_type            additional_update_result;
} lte_nas_emm_tau_accept_type;

typedef struct
{
  lte_nas_hdr_type                lte_nas_hdr;
  lte_nas_security_hdr_type       security_hdr;
  lte_nas_emm_cause_type          emm_cause;
#ifdef FEATURE_LTE_REL10
  dword                t3346_val;
#endif
}lte_nas_emm_tau_rej;

typedef struct
{
  lte_nas_hdr_type                lte_nas_hdr;
  lte_nas_security_hdr_type        security_hdr;

}lte_nas_emm_tau_complete;

typedef struct
{
  lte_nas_hdr_type           lte_nas_hdr;
  lte_nas_security_hdr_type  security_hdr;
  mobile_id_type             mobile_identity;

}lte_nas_emm_identity_req;

typedef struct
{
  lte_nas_hdr_type           lte_nas_hdr;
  lte_nas_security_hdr_type  security_hdr;
  lte_nas_emm_ms_id_type     ms_id;

}lte_nas_emm_identity_resp;

typedef struct
{
  lte_nas_hdr_type           lte_nas_hdr;
  lte_nas_security_hdr_type  security_hdr;
  lte_nas_emm_cause_type     emm_cause;

}lte_nas_emm_security_mode_reject;

typedef struct
{
  lte_nas_hdr_type               lte_nas_hdr;
  lte_nas_security_hdr_type      security_hdr;
  lte_nas_emm_guti_type          guti;
  boolean                        tai_list_exists;
  lte_nas_emm_tai_lst_info_type  tai_lst;
}lte_nas_emm_guti_realloc_cmd;

typedef struct
{
  lte_nas_hdr_type               lte_nas_hdr;
  lte_nas_security_hdr_type      security_hdr;
}lte_nas_emm_guti_realloc_complete;

typedef struct
{
  lte_nas_hdr_type                       lte_nas_hdr;
  lte_nas_security_hdr_type              security_hdr;
  boolean                                nw_full_name_exists;
  lte_nas_nw_name_type                   nw_full_name;
  boolean                                nw_partial_name_exists;
  lte_nas_nw_name_type                   nw_partial_name;
  boolean                                local_time_zone_exists;
  int                                   local_time_zone;/*BCD: Ref(23.040) -> TP Service Centre Time Stamp (TP SCTS)*/
  boolean                                tp_serv_center_time_stamp_exists;
  lte_nas_tp_serv_center_time_stamp_type tp_serv_center_time_stamp;
  boolean                                day_light_savings_exists;
  lte_nas_day_light_savings_type         day_light_savings;
}lte_nas_emm_info;

/* Data structure for EMM STATUS message */
typedef struct
{
  lte_nas_hdr_type                       lte_nas_hdr;
  lte_nas_security_hdr_type              security_hdr;
  lte_nas_emm_cause_type                 emm_cause;

}lte_nas_emm_status;

/* Data structure for downlink NAS transport message */
typedef struct
{
  lte_nas_hdr_type            lte_nas_hdr;
  lte_nas_security_hdr_type   security_hdr;
  lte_nas_msg_container_type  nas_msg_container;
} lte_nas_emm_dl_nas_transport;

/* Data structure for uplink NAS transport message */
typedef struct
{
  lte_nas_hdr_type            lte_nas_hdr;
  lte_nas_security_hdr_type   security_hdr;
  lte_nas_msg_container_type  nas_msg_container;
} lte_nas_emm_ul_nas_transport;

/* Data structure for Extended Service Request message */
typedef struct
{
  lte_nas_hdr_type                      lte_nas_hdr;
  lte_nas_security_hdr_type             security_hdr;
  lte_nas_emm_keyset_identifier_type    nas_key_set_identifier;
  lte_csfb_service_type                 service_type;
  byte                                  m_tmsi[4];
  lte_nas_csfb_rsp_type                 csfb_resp;
#ifdef FEATURE_LTE_REL9
  boolean                                   eps_bearer_context_status_exists;
  lte_nas_eps_bearer_context_status_type    eps_bearer_context_status;
#endif
#ifdef FEATURE_LTE_REL10
  boolean                               low_priority_indicator;
#endif
} lte_nas_emm_extended_service_req;

/* Data structure for CS SERVICE NOTIFICATION */
typedef struct
{
  lte_nas_hdr_type                      lte_nas_hdr;
  lte_nas_security_hdr_type             security_hdr;
  lte_nas_paging_identity_type          paging_id;
  boolean                               calling_line_info_exists;
  lte_nas_calling_line_info_type        calling_line_info;
  boolean                               supl_serv_code_exists;
  lte_nas_supl_serv_code_type           supl_serv_code; /*Refer spec 29.002, section 17.7.5*/
  boolean                               lcs_ind_exists;
  lte_nas_lcs_ind_type                  lcs_ind;
  //lte_nas_lcs_client_identity_type      lcs_client_identity; /*Refer spec 29.002, section 17.7.5*/
} lte_nas_emm_cs_serv_notification;

#if defined (FEATURE_LTE_REL9)
typedef struct
{
  lte_nas_hdr_type                      lte_nas_hdr;
  lte_nas_security_hdr_type             security_hdr;
  lte_nas_generic_msg_container_type    generic_msg_container_type;
  lte_nas_generic_msg_container         generic_msg_container;
  boolean                               additional_info_exists;
  lte_nas_generic_additional_info_type  additional_info;
}lte_nas_emm_ul_generic_nas_transport;

typedef struct
{
  lte_nas_hdr_type                      lte_nas_hdr;
  lte_nas_security_hdr_type             security_hdr;
  lte_nas_generic_msg_container_type    generic_msg_container_type;
  lte_nas_generic_msg_container         generic_msg_container;
  boolean                               additional_info_exists;
  lte_nas_generic_additional_info_type  additional_info;
}lte_nas_emm_dl_generic_nas_transport;

#endif

/*define an union containing all incoming messge type*/
typedef union
{
  lte_nas_hdr_type                      hdr;
  lte_nas_emm_attach_accept_type        attach_accept ;    /*attach accept*/  
  lte_nas_emm_attach_rej_type           attach_rej;     /*attach reject*/ 
  lte_nas_emm_mt_detach_req_type        mt_detach_req; /*detach request*/
  lte_nas_emm_mt_detach_accept_type     mt_detach_accept;

  lte_nas_emm_security_mode_command     security_mode_cmd;
  lte_nas_emm_service_rej_type          service_rej; /*service reject*/
  lte_nas_emm_auth_req                  auth_req;/*Authentication Request*/
  lte_nas_emm_auth_rej                  auth_rej;
  lte_nas_emm_tau_accept_type           tau_accept;
  lte_nas_emm_tau_rej                   tau_rej;
  lte_nas_emm_identity_req              identity_req;
  lte_nas_emm_guti_realloc_cmd          guti_realloc_cmd;
  lte_nas_emm_info                      emm_info;
  /* Mobile terminated EMM STATUS message */
  lte_nas_emm_status                    mt_emm_status;
  /* Downlink NAS transport message */
  lte_nas_emm_dl_nas_transport          dl_nas_transport;
  lte_nas_emm_cs_serv_notification      cs_serv_notification;
#ifdef FEATURE_LTE_REL9
  lte_nas_emm_dl_generic_nas_transport  dl_generic_nas_transport_msg;
#endif

}lte_nas_emm_incoming_msg_type ;


/*define an union containing all ougoing messge type*/

typedef union
{
  lte_nas_hdr_type                        hdr;
  lte_nas_emm_attach_complete_type        attach_complete;   /*attach complete*/ 
  lte_nas_emm_attach_req_type             attach_req;    /*attach request*/
  lte_nas_emm_mo_detach_accept_type       mo_detach_accept;  /*detach accept*/   
  lte_nas_emm_mo_detach_req_type          mo_detach_req; /*detach request*/ 

  lte_nas_emm_security_mode_complete      security_mode_complete;
  lte_nas_emm_security_mode_reject        security_rej;
  lte_nas_emm_service_req_type            service_req;   /*service request*/
  lte_nas_emm_auth_resp                   auth_resp; /*Authentication Response*/
  lte_nas_emm_security_mode_rej           security_mode_rej;
  lte_nas_emm_tau_req                     tau_req;  
  lte_nas_emm_tau_complete                tau_complete;
  lte_nas_emm_auth_fail                   auth_fail;
  lte_nas_emm_identity_resp               identity_resp;
  lte_nas_emm_guti_realloc_complete       guti_realloc_complete;
  /* Mobile originated EMM STATUS message */
  lte_nas_emm_status                      mo_emm_status;
  /* Uplink NAS transport message */
  lte_nas_emm_ul_nas_transport            ul_nas_transport;
  lte_nas_emm_extended_service_req        ext_service_req;
#ifdef FEATURE_LTE_REL9
  lte_nas_emm_ul_generic_nas_transport    ul_generic_nas_transport_msg;
#endif
} lte_nas_emm_outgoing_msg_type ;
#endif


#endif /*FEATURE_LTE*/
 

 

 

