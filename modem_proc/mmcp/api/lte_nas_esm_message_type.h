
/*==============================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*Author: hnam
  Reference Document: 3GPP TS 24.301 V1.1.1 (2008-10)
    START DATE: 12/01/2008*/
/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/lte_nas_esm_message_type.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/17/12   cogolt  Rel-10, Added backoff_timer to lte_nas_pdn_connectivity_rej, 
                   lte_nas_esm_bearer_resource_allocation_rej & lte_nas_esm_bearer_resource_modifi_rej
01/21/10   zren    Added EMM/ESM STATUS feature
07/21/09   hnam    Added modify bearer messages
07/15/09   hnam    Added esm_info_req/resp and pdn_disconnect_req/resp message types
04/29/09   hnam    Added Deactivate EPS Bearer context Accept
04/28/09   MNK     Changed esm_information to esm_information_transfer_bit
04/27/09   hnam    Added bearer resource allocation request, bearer resource modification request and reject
04/13/09   hnam    Added deactivate EPS bearer context request and bearer resource allocation reject
04/02/09   MNK     Added support for multiple bearer setup support at the attach 
03/20/09   hnam    Added Dedicated EPS Bearer context Accept, request and reject (release 2.0)

==============================================================================*/
#ifndef _LTE_NAS_ESM_MESSAGE_TYPE_H_
#define _LTE_NAS_ESM_MESSAGE_TYPE_H_ 

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include<customer.h>


#include "comdef.h"
#include "cm_gw.h"
#include "sm_common.h"
#include "lte_nas_common.h"
#include "lte_nas_esm_message_description.h"
#include "sm.h"

/*define a structure for header type*/
typedef struct 
{
  byte         pti;
  byte         bearer_id;

} lte_nas_esm_hdr_type;

typedef struct 
{
  lte_nas_hdr_type        lte_nas_hdr;
  lte_nas_esm_hdr_type    lte_nas_esm_hdr;
  esm_cause_T             esm_cause; 
  protocol_cfg_options_T  protocol_conf_opt;

} lte_nas_esm_act_default_bearer_context_rej;

typedef struct
{
  lte_nas_hdr_type        lte_nas_hdr;
  lte_nas_esm_hdr_type    lte_nas_esm_hdr;
  esm_cause_T             esm_cause; 
  protocol_cfg_options_T  protocol_conf_opt;
  
}lte_nas_esm_act_dedicated_bearer_context_rej;

typedef struct 
{
  lte_nas_hdr_type            lte_nas_hdr;
  lte_nas_esm_hdr_type        lte_nas_esm_hdr;
  byte                        linked_eps_bearer_identity;
  sdf_qos_T                   sdf_qos;
  tft_type_T                  tft;
  gsm_umts_transaction_id_T   ti;
  qos_T                       neg_qos;
  llc_sapi_T                  neg_llc_sapi;             
  rad_priority_T              radio_priority;            
  pfi_T                       pack_flo_id;        
  protocol_cfg_options_T      protocol_conf_opt;
  
} lte_nas_esm_act_dedicated_bearer_context_req;

typedef struct 
{
  lte_nas_hdr_type           lte_nas_hdr;
  lte_nas_esm_hdr_type       lte_nas_esm_hdr;
  sdf_qos_T                  sdf_qos;
  pdn_address_T              pdn_address;       
  apn_T                      access_point_name;
  gsm_umts_transaction_id_T  ti;
  qos_T                      neg_qos;
  llc_sapi_T                 neg_llc_sapi;             
  rad_priority_T             radio_priority;            
  pfi_T                      pack_flo_id;  
  apn_ambr_T                 apn_ambr;
  esm_cause_T                esm_cause;       
  protocol_cfg_options_T     protocol_conf_opt;
  
}lte_nas_esm_act_default_bearer_context_req;

typedef struct 
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  protocol_cfg_options_T   protocol_conf_opt;

} lte_nas_esm_act_default_bearer_context_accept;

typedef struct 
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  protocol_cfg_options_T   protocol_conf_opt;

} lte_nas_esm_act_dedicated_bearer_context_accept;

/* Refer Section: 8.3.20.1*/
typedef struct 
{
  lte_nas_hdr_type           lte_nas_hdr;
  lte_nas_esm_hdr_type       lte_nas_esm_hdr;
  req_type_T                 req;
  boolean                    esm_info_transfer_bit;
  pdn_type_T                 pdn_type;
  apn_T                      access_point_name;
  protocol_cfg_options_T     protocol_conf_opt;
  uint8                      seq_num;
  boolean                    low_priority_indicator;
} lte_nas_esm_pdn_connectivity_req;                    

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  esm_cause_T              esm_cause;       
  protocol_cfg_options_T   protocol_conf_opt;
  timer_3gpp_type3         backoff_timer;
}lte_nas_pdn_connectivity_rej;
/*define an union containing all incoming messge type*/

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  esm_cause_T              esm_cause;       
  protocol_cfg_options_T   protocol_conf_opt;
  timer_3gpp_type3         backoff_timer;               

}lte_nas_esm_deact_eps_bearer_context_req;

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  protocol_cfg_options_T   protocol_conf_opt;

}lte_nas_esm_deact_eps_bearer_context_accept;

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  esm_cause_T              esm_cause;       
  protocol_cfg_options_T   protocol_conf_opt;
  timer_3gpp_type3         backoff_timer;
}lte_nas_esm_bearer_resource_allocation_rej;

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  byte                     linked_eps_bearer_identity;
  tft_type_T               tft;
  sdf_qos_T                sdf_qos;
  protocol_cfg_options_T   protocol_conf_opt;
  boolean                  low_priority_indicator;
}lte_nas_esm_bearer_resource_allocation_req;

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  esm_cause_T              esm_cause;       
  protocol_cfg_options_T   protocol_conf_opt;
  timer_3gpp_type3         backoff_timer;
}lte_nas_esm_bearer_resource_modifi_rej;

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  byte                     linked_eps_bearer_identity;
  tft_type_T               tft;
  sdf_qos_T                sdf_qos;
  esm_cause_T              esm_cause; 
  protocol_cfg_options_T   protocol_conf_opt;
  boolean                  low_priority_indicator;
}lte_nas_esm_bearer_resource_modifi_req;

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  esm_cause_T              esm_cause; 
  protocol_cfg_options_T   protocol_conf_opt;

}lte_nas_esm_pdn_disconnect_rej;

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  byte                     linked_eps_bearer_identity;  
  protocol_cfg_options_T   protocol_conf_opt;

}lte_nas_esm_pdn_disconnect_req;

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  apn_T                    access_point_name;
  protocol_cfg_options_T   protocol_conf_opt;

}lte_nas_esm_info_resp;

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;

}lte_nas_esm_info_req;

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  sdf_qos_T                sdf_qos;
  tft_type_T               tft;
  qos_T                    neg_qos;
  llc_sapi_T               neg_llc_sapi;
  rad_priority_T           radio_priority;
  pfi_T                    pack_flo_id;  
  apn_ambr_T               apn_ambr;     
  protocol_cfg_options_T   protocol_conf_opt;
}lte_nas_esm_modify_eps_bearer_ctxt_req;

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  protocol_cfg_options_T   protocol_conf_opt;
}lte_nas_esm_modify_eps_bearer_ctxt_accept;

typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  esm_cause_T              esm_cause; 
  protocol_cfg_options_T   protocol_conf_opt;
}lte_nas_esm_modify_eps_bearer_ctxt_rej;

/* Data structure for ESM STATUS message */
typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;
  esm_cause_enum_T         esm_cause;

}lte_nas_esm_status;

/* For internal use - to hold bid/pti info from rcved unknown msg for ESM */
typedef struct
{
  lte_nas_hdr_type         lte_nas_hdr;
  lte_nas_esm_hdr_type     lte_nas_esm_hdr;

}lte_nas_esm_unknown_msg;


typedef struct
{
  lte_nas_hdr_type               lte_nas_hdr;
  lte_nas_esm_hdr_type           lte_nas_esm_hdr;
  byte                           notification_ind;
}lte_nas_esm_notification;


typedef union
{    
  lte_nas_hdr_type                                 nas_hdr;
  lte_nas_esm_act_default_bearer_context_req       default_bearer_context_req;
  lte_nas_pdn_connectivity_rej                     pdn_connectivity_rej;  
  lte_nas_esm_act_dedicated_bearer_context_req     dedicated_bearer_context_req;
  lte_nas_esm_deact_eps_bearer_context_req         deactivate_eps_bearer_context_req;
  lte_nas_esm_bearer_resource_allocation_rej       bearer_resource_alloc_rej;
  lte_nas_esm_bearer_resource_modifi_rej           bearer_resource_modifi_rej;
  lte_nas_esm_pdn_disconnect_rej                   pdn_disconnect_rej;
  lte_nas_esm_info_req                             esm_info_req;
  lte_nas_esm_modify_eps_bearer_ctxt_req           modify_eps_bearer_ctxt_req;
  /* Mobile terminated ESM STATUS message */
  lte_nas_esm_status                               mt_esm_status;
  lte_nas_esm_unknown_msg                          esm_unknown_msg;

  lte_nas_esm_notification                         esm_notification;

}lte_nas_esm_incoming_msg_type;

typedef union
{
  lte_nas_hdr_type                                 nas_hdr;
  lte_nas_esm_pdn_connectivity_req                 pdn_connectivity_req;
  lte_nas_esm_act_default_bearer_context_accept    default_bearer_context_accept;
  lte_nas_esm_act_default_bearer_context_rej       default_bearer_context_rej;
  lte_nas_esm_act_dedicated_bearer_context_accept  dedicated_bearer_context_accept;
  lte_nas_esm_act_dedicated_bearer_context_rej     dedicated_bearer_context_rej;
  lte_nas_esm_bearer_resource_allocation_req       bearer_alloc_req;
  lte_nas_esm_bearer_resource_modifi_req           bearer_resource_modifi_req;
  lte_nas_esm_deact_eps_bearer_context_accept      deactivate_eps_bearer_context_accept;
  lte_nas_esm_pdn_disconnect_req                   pdn_disconnect_req;
  lte_nas_esm_info_resp                            esm_info_resp;
  lte_nas_esm_modify_eps_bearer_ctxt_accept        modify_eps_bearer_ctxt_accept;
  lte_nas_esm_modify_eps_bearer_ctxt_rej           modify_eps_bearer_ctxt_rej;
  /* Mobile originated ESM STATUS message */
  lte_nas_esm_status                               mo_esm_status;

}lte_nas_esm_outgoing_msg_type;



#endif /*FEATURE_LTE*/





