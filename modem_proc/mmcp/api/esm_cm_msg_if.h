/*!
  @file
  esm_cm_msg_if.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

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

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/esm_cm_msg_if.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/26/10   MNK     Add h_comp & d_comp for def accept & ded accept
02/17/10   zren    Added support to always deact bearer upon DS request in UE 
                   initiated modification procedure
09/21/09   MNK     Changes due to IRAT design
08/14/09   MNK     Minor changes to messages
07/31/09   MNK     Added local_cause to default bearer reject
06/02/09   MNK     Move lbi to esm_header as it is also one of the search key for ESM
05/28/09   MNK     Remove unneeded messages
05/11/09   MNK     Remove df_id from bearer resource allocation as it is part of the esm header
05/08/09   MNK     Added new internal timer expiration message/signal 
04/28/09   MNK     Remove pdn_data_T. Not needed
==============================================================================*/

#ifndef ESM_CM_MESSAGES_H
#define ESM_CM_MESSAGES_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include<customer.h>

#include "comdef.h"
#include <msgr.h>
#include <msgr_lte.h>
#include "sys.h"
#include "sys_gw_v.h"
#include "lte_nas_common.h"
#include "lte_nas_local_cause.h"
#include "lte_nas_esm_message_type.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
//typedef struct 
//{
//  msgr_hdr_s        msg_hdr; /*!< Message router header */
//  esm_header_T esm_hdr;

//  req_type_T request_type ;/*!< Brief description of this member */
//  byte pdn_type;/*!< Brief description of this member */
  /*This is just put as a reference, when reading the data from the messsage this will be passed 
  over as half octet before this will be read as a full byte and this half octet is anyway 
  upper 4 bits set to zero */ 
  //byte spare_half_octet; /*!< Brief description of this member */
//  apn_T apn_name;  /*!< Brief description of this member */
//  esm_info_transfer_flag_T esm_info_xfer_flag;   /*!< Brief description of this member */
//  protocol_cfg_options_T  protocol_config_options;/*!< pco will always be passed even if the pco transfer flag is true */
  /* pdn_address allocation member missing from the standard? */
//} pdn_data_T;


/*! @brief REQUIRED This struct represents the bearer resource allocation request
*/
typedef struct 
{
  msgr_hdr_s                msg_hdr; /*!< Message router header */
  esm_header_T              esm_hdr;

  sdf_qos_T                 sdf_qos; /*!< Brief description of this member */
  tft_type_T                ul_dl_tft; /*!< Brief description of this member */
  protocol_cfg_options_T    protocol_config_options; /*!< Brief description of this member */

  boolean                   low_priority_signaling; /*!< NAS signaling low priority indicator */  

} esm_bearer_resource_alloc_req_T ;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  msgr_hdr_s              msg_hdr; /*!< Message router header */
  esm_header_T            esm_hdr;
  req_type_T              request_type; /*!< Brief description of this member */
  pdn_connection_data_T   pdn_data;
  /* pdn_address allocation member missing from the standard? */
} esm_pdn_connectivity_req_T ;

#define PDN_CONN_REQ_SIZE 255 /*Size of PDN CONNECTIVITY REQ message should be restricted to 255*/

typedef enum
{
  ESM_MSG_ACCEPTED,
  ESM_MSG_REJECTED,
  ESM_MSG_MODIFIED,
  ESM_MSG_REJECTED_ACL_FAILURE
}pdn_conn_req_status_T;

/* This message is used by CM to send ACCEPT/MODIFY/REJECT for the packed PDN_CONNECTIVITY_REQ message that ESM sends*/
typedef struct 
{
  msgr_hdr_s              msg_hdr; /*!< Message router header */
  esm_header_T            esm_hdr;
  pdn_conn_req_status_T   status;
  word                    pkd_msg_len;
  unsigned char           pkd_msg[PDN_CONN_REQ_SIZE];
  byte                    pti;
} esm_pdn_connectivity_packed_cnf_T ;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  msgr_hdr_s               msg_hdr; /*!< Message router header */
  esm_header_T             esm_hdr;
  eps_bearer_id_T          eps_bearer_id;  /*!< Brief description of this member */
  protocol_cfg_options_T   protocol_config_options; /*!< Brief description of this member */
  lte_nas_local_cause_T    esm_local_cause;
} esm_pdn_disconnect_req_T;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  msgr_hdr_s              msg_hdr; /*!< Message router header */
  esm_header_T            esm_hdr;
  esm_cause_T             esm_cause; /*!< ESM OTA cause value for the message */
  lte_nas_local_cause_T   esm_local_cause; /*!< ESM local cause value for the message */
} esm_bearer_resource_alloc_abort_req_T;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  msgr_hdr_s              msg_hdr; /*!< Message router header */
  esm_header_T            esm_hdr;
  esm_cause_T             esm_cause; /*!< ESM cause value for the message */
  lte_nas_local_cause_T   esm_local_cause; /*!< ESM local cause value for the message */
} esm_pdn_connectivity_abort_req_T;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  msgr_hdr_s         msg_hdr; /*!< Message router header */
  esm_header_T       esm_hdr;
  eps_bearer_id_T    eps_bearer_id; /*!< Brief description of this member */
  sys_lte_reestab_calltype_e call_type; /*!< Establishment call type */
} esm_drb_reestablish_req_T;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  msgr_hdr_s                msg_hdr; /*!< Message router header */
  esm_header_T              esm_hdr;
  eps_bearer_id_T           eps_bearer_id; /*!< Brief description of this member */
  /* Indicates if it is emergency bearer */
  boolean                   emc_bc;
  protocol_cfg_options_T    protocol_config_options; /*!< Brief description of this member */
  sys_pdp_header_comp_e_type      h_comp;
  sys_pdp_data_comp_e_type        d_comp;
} esm_act_default_bearer_context_accept_T;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  msgr_hdr_s                msg_hdr; /*!< Message router header */
  esm_header_T              esm_hdr;
  eps_bearer_id_T           eps_bearer_id; /*!< Brief description of this member */
  esm_cause_T               esm_cause; /*!< Brief description of this member */
  lte_nas_local_cause_T      esm_local_cause; /*!< ESM local cause value for the message */
  protocol_cfg_options_T    protocol_config_options; /*!< Brief description of this member */
  /* Indicates if it is emergency bearer */
  boolean                    emc_bc;
} esm_act_default_bearer_context_rej_T;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  msgr_hdr_s                  msg_hdr; /*!< Message router header */
  esm_header_T                esm_hdr;
  eps_bearer_id_T             eps_bearer_id; /*!< Brief description of this member */
  /* Indicates if it is emergency bearer */
  boolean                     emc_bc;
  protocol_cfg_options_T      protocol_config_options; /*!< Brief description of this member */
  sys_pdp_header_comp_e_type      h_comp;
  sys_pdp_data_comp_e_type        d_comp;
} esm_act_dedicated_bearer_context_accept_T; 

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  msgr_hdr_s                 msg_hdr; /*!< Message router header */
  esm_header_T               esm_hdr;
  eps_bearer_id_T            eps_bearer_id; /*!< Brief description of this member */
  esm_cause_T                esm_cause;
  lte_nas_local_cause_T      esm_local_cause; /*!< ESM local cause value for the message */
  protocol_cfg_options_T     protocol_config_options; /*!< Brief description of this member */
} esm_act_dedicated_bearer_context_rej_T;


/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  msgr_hdr_s                msg_hdr; /*!< Message router header */
  esm_header_T              esm_hdr;
  eps_bearer_id_T           eps_bearer_id; /*!< Brief description of this member */
  esm_cause_T               esm_cause; /*!< Brief description of this member */
  lte_nas_local_cause_T     esm_local_cause; /*!< ESM local cause value for the message */
  protocol_cfg_options_T    protocol_config_options; /*!< Brief description of this member */
} esm_modify_bearer_context_rej_T;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  msgr_hdr_s                msg_hdr; /*!< Message router header */
  esm_header_T              esm_hdr;
  eps_bearer_id_T           eps_bearer_id; /*!< Brief description of this member */
  protocol_cfg_options_T    protocol_config_options; /*!< Brief description of this member */
} esm_modify_bearer_context_accept_T;

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct 
{
  msgr_hdr_s        msg_hdr; /*!< Message router header */
  esm_header_T      esm_hdr;
  word              timer_id; /*!< Brief description of this member */
  word              instance_id;
} esm_self_signal_timer_expired_T;

/*! @brief REQUIRED This struct represents the bearer resource allocation request
*/
typedef struct 
{
  msgr_hdr_s                msg_hdr; /*!< Message router header */
  esm_header_T              esm_hdr;

  sdf_qos_T                 sdf_qos; /*!< Brief description of this member */
  tft_type_T                tad; /*!< Brief description of this member */
  esm_cause_T               esm_cause;
  protocol_cfg_options_T    protocol_config_options; /*!< Brief description of this member */
  lte_nas_local_cause_T     esm_local_cause;  

  boolean                   low_priority_signaling;  /*!< NAS signaling low priority indicator */

} esm_bearer_resource_modification_req_T ;



typedef struct
{
  msgr_hdr_s             msg_hdr;
  dword                  trans_id;
  uint8                  csfb_call_id;
  lte_csfb_service_type  csfb_service_type;
  boolean                partial_esr; //true when UE make silent redial on 1x after soft failure happened on VOLTE over SRLTE
}esm_1xCSFB_call_req_T; 

typedef struct
{
  msgr_hdr_s             msg_hdr;
  dword                  trans_id;
  uint8                  csfb_call_id;
}esm_1xCSFB_abort_req_T; 


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#endif /* ESM_CM_MESSAGES_H */


