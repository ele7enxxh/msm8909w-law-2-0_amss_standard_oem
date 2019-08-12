/*!
  @file
  esm.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/esm.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/17/12   cogolt  Rel-10, Added esm_unblock_all_apns_ind to union esm_msg
12/10/09   zren    Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/03/09   MNK     Added esm_get_pdn_connectivity_req_T message
05/28/09   MNK     Remove unneeded messages
05/08/09   MNK     Added new internal timer expiration message/signal 
04/24/09   MNK     Added emm_eps_bearer_status message
==============================================================================*/

#ifndef ESM_H
#define ESM_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include<customer.h>

#include "comdef.h"
#include "queue.h"
#include <msgr.h>
#include <msgr_lte.h>
#include "esm_cm_msg_if.h"
#include "esm_emm_msg_if.h"



/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

typedef union
{  

    msgr_hdr_s        msg_hdr; /*!< Message router header */

    /*! @brief Messages sent by the CM to ESM*/ 
    esm_bearer_resource_alloc_req_T             esm_bearer_resource_alloc_req;
    esm_pdn_connectivity_req_T                 esm_pdn_connectivity_req;
    esm_pdn_disconnect_req_T                    esm_pdn_disconnect_req;
    esm_bearer_resource_alloc_abort_req_T       esm_bearer_resource_alloc_abort_req;
    esm_pdn_connectivity_abort_req_T            esm_pdn_connectivity_abort_req;
    esm_drb_reestablish_req_T                   esm_drb_reestablish_req;
    esm_act_default_bearer_context_accept_T     esm_act_default_bearer_context_accept;
    esm_act_default_bearer_context_rej_T        esm_act_default_bearer_context_rej;
    esm_act_dedicated_bearer_context_accept_T   esm_act_dedicated_bearer_context_accept; 
    esm_act_dedicated_bearer_context_rej_T      esm_act_dedicated_bearer_context_rej;
    //esm_act_deact_bearer_context_accept_T       esm_act_deact_bearer_context_accept;
    esm_modify_bearer_context_rej_T             esm_modify_bearer_context_rej;
    esm_modify_bearer_context_accept_T          esm_modify_bearer_context_accept;
    esm_bearer_resource_modification_req_T      esm_bearer_resource_modification_req;

    /*! @brief Messages sent by the ESM to CM. These will be part of the CM cmd type structure in the union part */ 

    /*cm_act_default_bearer_context_request_ind_T cm_act_default_bearer_context_request_ind;
    cm_ act_dedicated_bearer_context_request_ind_T cm_ act_dedicated_bearer_context_request_ind;
    cm_ deact_bearer_context_request_ind_T cm_ deact_bearer_context_request_ind;
    cm_ bearer_resource_alloc_reject_ind _T cm_ bearer_resource_alloc_reject_ind;
    cm_ cm_bearer_resource_rel_reject_ind_T cm_ cm_bearer_resource_rel_reject_ind;
    cm_ bearer_context_modify_requestt_ind_T cm_ bearer_context_modify_requestt_indT;
    cm_ pdn_connectivity_reject_ind_T cm_ pdn_connectivity_reject_ind;
    cm_ pdn_disconnect_rejectt_ind_T cm_ pdn_disconnect_rejectt_ind;
    cm_ act_drb_released_ind_T cm_ act_drb_released_ind;
    cm_ act_drb_released_ind_T cm_ act_drb_released_ind;
    cm_drb_setup_ind_T cm_drb_setup_ind;*/

    /*! @brief Messages sent by the EMM to ESM*/ 
    esm_data_ind_T                      esm_data_ind;
    //esm_reject_ind_T esm_reject_ind;
    esm_failure_ind_T                   esm_failure_ind;
    esm_sig_connection_released_ind_T   esm_sig_connection_released_ind ;
    esm_active_eps_ind_T                esm_active_eps_ind ;
    esm_detach_ind_T                    esm_detach_ind ;
    esm_eps_bearer_status_ind_T         esm_eps_bearer_status_ind;
    esm_get_pdn_connectivity_req_T      esm_get_pdn_connectivity_req;

    /* ESM to ESM signal. Shouldn't be sent/used by any other module other than ESM*/
    esm_self_signal_timer_expired_T     esm_timer_expired_ind;

    esm_get_isr_status_ind_T  esm_isr_status_ind;
    esm_isr_status_change_ind_T  esm_isr_status_change_ind;


    esm_pdn_connectivity_packed_cnf_T   esm_pdn_connectivity_pkd_cnf;

    esm_1xCSFB_call_req_T               esm_1xCSFB_call_req;
    esm_1xCSFB_abort_req_T              esm_1xCSFB_abort_req;
    emm_1xCSFB_esr_call_rsp_T           emm_1xCSFB_esr_call_rsp;
    emm_1xCSFB_esr_call_abort_rsp_T     emm_1xCSFB_esr_call_abort_rsp;

    esm_unblock_all_apns_ind_T          esm_unblock_all_apns_ind;

}esm_msg;

/*! @brief ESM command structure type encompassing the incmoing commands/messages in a union
*/
typedef struct esm_cmd
{
  q_link_type         link; /*!< Queue link. This must be the first element of the cmd_type. 
                            MSGR expects q_link to be the first element and the msgr_hdr_s 
                            as the second element.So, after link_type no element should be 
                            added and the only element is the union below*/
  esm_msg cmd;
  /*req_direction_T request_direction;*/

} esm_cmd_type;


#endif /* ESM_H */


