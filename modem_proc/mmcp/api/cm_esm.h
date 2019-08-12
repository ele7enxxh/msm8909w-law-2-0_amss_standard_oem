/*!
  @file
  cm_esm.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cm_esm.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/19/12   skk     Added support for "ESM_SM_backoff timer" requirements
02/07/12   xs      VOLTE silent redial support
10/06/11   xs      Add SRVCC cancel notification definition between CM and NAS
02/28/11   gm      Added thermal emergency feature support
10/04/10   rk      Added new messages in cm_esm_cmd_u_type for 1xCSFB MO/MT call
09/21/10   rk      Added messages for 1xCSFB MO/MT calls
06/21/10   fj      Add support for LTE call control.
05/26/10   fj      Added group_id in cm_act_default_bearer_context_request_ind_s_type
                   and cm_act_dedicated_bearer_context_request_ind_s_type.
02/18/10   fj      Added gsm_umts_connection_id in act_default_bearer_context request
                   and act_dedicated_bearer_context request.
02/05/10   fj      Added apn_ambr in act_default_bearer_context request and 
                   modify_bearer_context_request.
10/15/09   np      Merge latest code from LTE off target
10/15/09   np      Added $header 
10/14/09   fj      Updating UMID def's to use MM tech area.
08/31/09   fj      In cm_modify_bearer_context_request_ind_s_type, updated ul_tft
                   to ul_dl_tft.
09/11/09   fj      Added rb_id in cm_drb_setup_ind_s_type.
08/18/09   fj      Added cm_get_pdn_connectivity_request_ind_s_type,
                         cm_bearer_resource_alloc_failure_ind_s_type,
                         cm_modify_bearer_context_request_ind_s_type,
                         cm_pdn_connectivity_failure_ind_s_type,
                         cm_bearer_context_modify_reject_ind_s_type.
08/06/09   fj      Added cm_pdn_disconnect_reject_ind to cm_esm_cmd_u.
06/19/09   np      Added cm_drb_reestablish_reject_ind to cm_esm_cmd_u
06/18/09   fj      Added cm_drb_reestablish_reject_ind_s_type.
06/01/09   fj      Removed lbi from cm_act_default_bearer_context_request_ind_s_type,
                   and from cm_act_dedicated_bearer_context_request_ind_s_type,
                   since ESM added lbi into esm_hdr.
05/28/09   fj      Added lbi into cm_act_default_bearer_context_request_ind_s_type.
05/08/09   fj      Added cm_bearer_resource_alloc_abort_rsp_s_type and 
                   cm_bearer_resource_alloc_reject_ind_s_type.
03/11/09   fj      Added data structure for act_dedicated_bearer_context_request,
                   bearer_context_modify_request, pdn_connectivity_reject, and 
                   pdn_connectivity_aborted.
11/11/08   fj      Initial version
==============================================================================*/

#ifndef CM_ESM_H
#define CM_ESM_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "comdef.h"
#include <msgr.h>
#include "lte_nas_esm_message_description.h"
#include "cm_msgr_msg.h"
#include "lte_nas_local_cause.h"
#include "cm_gw.h"    // for eps_bearer_id_T
#include "esm_cm_msg_if.h"
#include "emm_irat_if_msg.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief External type for my clients
*/


/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct cm_act_default_bearer_context_request_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id;    
      /*!< Brief description of this member */

  sdf_qos_T               sdf_qos; 
      /*!< Brief description of this member */

  pdn_address_T           pdn_address; 
      /*!< Brief description of this member */

  apn_T                   apn_name; 
      /*!< Brief escription of this member */

  tft_type_T              ul_tft; 
      /*!< Brief description of this member */

  qos_T                   neg_qos; 
      /*!< Brief description of this member */

  llc_sapi_T              neg_llc_sapi; 
      /*!< Brief description of this member */

  protocol_cfg_options_T  protocol_config_options; 
      /*!< Brief description of this member */

  esm_cause_T             esm_cause; 

  apn_ambr_T              apn_ambr;

  gsm_umts_connection_id_T gsm_umts_connection_id;

  byte                    group_id;
  
} cm_act_default_bearer_context_request_ind_s_type;



/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct cm_deact_bearer_context_request_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id; 
    /*!< Brief description of this member */

  esm_cause_T             esm_cause; 
    /*!< Brief description of this member */

  protocol_cfg_options_T  protocol_config_options; 

    /*!< Brief description of this member */
  lte_nas_local_cause_T   esm_local_cause ;

  timer_3gpp_type3        backoff_timer;
} cm_deact_bearer_context_request_ind_s_type;



/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct cm_act_dedicated_bearer_context_request_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id;    
      /*!< Brief description of this member */

  sdf_qos_T               sdf_qos; 
      /*!< Brief description of this member */

  tft_type_T              tft; 
      /*!< Brief description of this member */

  qos_T                   neg_qos; 
      /*!< Brief description of this member */

  llc_sapi_T              neg_llc_sapi; 
      /*!< Brief description of this member */

  protocol_cfg_options_T  protocol_config_options; 
      /*!< Brief description of this member */

  gsm_umts_connection_id_T gsm_umts_connection_id;

  byte                    group_id;
  
}cm_act_dedicated_bearer_context_request_ind_s_type;



/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct cm_modify_bearer_context_request_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id;    
      /*!< Brief description of this member */

  sdf_qos_T               sdf_qos; 
      /*!< Brief description of this member */

  tft_type_T              ul_dl_tft; 
      /*!< Brief description of this member */

  qos_T                   neg_qos; 
      /*!< Brief description of this member */

  llc_sapi_T              neg_llc_sapi; 
      /*!< Brief description of this member */

  protocol_cfg_options_T  protocol_config_options; 
      /*!< Brief description of this member */

  apn_ambr_T              apn_ambr;

}cm_modify_bearer_context_request_ind_s_type;



/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct cm_bearer_resource_alloc_failure_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  boolean                 result;
//esm_cause_T esm_cause;
} cm_bearer_resource_alloc_failure_ind_s_type;




/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct cm_bearer_resource_alloc_reject_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  esm_cause_T             esm_cause;
  lte_nas_local_cause_T   esm_local_cause;
  protocol_cfg_options_T  protocol_config_options;
  timer_3gpp_type3        backoff_timer;
} cm_bearer_resource_alloc_reject_ind_s_type;



/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef cm_bearer_resource_alloc_reject_ind_s_type cm_bearer_context_modify_reject_ind_s_type;



/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct cm_drb_released_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id ; 
    /*!< Brief description of this member */

  esm_cause_T             esm_cause ;
  lte_nas_local_cause_T   esm_local_cause ;

} cm_drb_released_ind_s_type;



/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct cm_drb_setup_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id ; 
    /*!< Brief description of this member */

  byte                    rb_id;

} cm_drb_setup_ind_s_type;


/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct cm_drb_reestablish_failed_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id ; 
    /*!< Brief description of this member */

  lte_nas_local_cause_T   esm_local_cause ;

  emm_failure_type        emm_failure_cause;
  
} cm_drb_reestablish_failed_ind_s_type;


typedef cm_drb_reestablish_failed_ind_s_type cm_drb_reestablish_reject_ind_s_type;


/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct cm_pdn_connectivity_reject_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  esm_cause_T             esm_cause;
  lte_nas_local_cause_T   esm_local_cause;
  protocol_cfg_options_T  protocol_config_options;
  timer_3gpp_type3               backoff_timer;
  pdn_reject_ind_params_s_type   pdn_reject_ind;

  emm_failure_type        emm_failure_cause;
  boolean                 emc_type;

} cm_pdn_connectivity_reject_ind_s_type;


/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct cm_pdn_connectivity_failure_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  boolean                 result;
    /*!< Brief description of this member */

} cm_pdn_connectivity_failure_ind_s_type;


/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct cm_pdn_disconnect_reject_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  eps_bearer_id_T         eps_bearer_id ;
  esm_cause_T             esm_cause ;
  lte_nas_local_cause_T   esm_local_cause ;
  protocol_cfg_options_T  protocol_config_options;

} cm_pdn_disconnect_reject_ind_s_type;


typedef struct cm_get_pdn_connectivity_request_ind_s
{
  msgr_hdr_s              msg_hdr;

  uint8                   seq_num;

  boolean                 responding_to_page;
  
} cm_get_pdn_connectivity_request_ind_s_type;


/*! @brief ESM sends packed data to CM for call control validation.
*/
typedef struct cm_pdn_connectivity_packed_request_ind_s
{
  msgr_hdr_s              msg_hdr; 
  esm_header_T            esm_hdr;

  byte                    pkd_msg_len;
  byte                    pkd_msg[PDN_CONN_REQ_SIZE];
  apn_T                   apn_name;
  byte                    pti;
} cm_pdn_connectivity_packed_request_ind_s_type;

/* This structure is used for sending CSFB call abort response
** From NAS to CM
*/

typedef struct

{
  msgr_hdr_s             msg_hdr;             /* Message router header */
  uint8                  csfb_call_id;        /* Call ID */
} cm_1xCSFB_abort_rsp; 

 
/* This structure is used for sending CSFB call response 
** (only failures cases) from NAS to CM
*/
typedef struct

{
  msgr_hdr_s             msg_hdr;              /* Message router header */
  uint8                  csfb_call_id;         /* Call ID */

  ie_cm_cc_reject_T      cm_cc_reject;         /* Reject cause info */

} cm_1xCSFB_call_rsp;

/* This structure is used for sending ESM SRVCC cancel notification
** From NAS to CM
*/
typedef struct
{
  msgr_hdr_s                     msg_hdr;             /* Message router header */
  esm_notification_ind_e_type    notification_ind;    /* notificationID */

} cm_esm_notification_ind_s_type;



/*! @brief CM command structure type encompassing the incoming commands/messages in a union
*/

typedef  union cm_esm_cmd_u
  {  

  msgr_hdr_s        msg_hdr; /*!< Message router header */


    /*! @brief Messages sent by the ESM to CM. */ 

  cm_act_default_bearer_context_request_ind_s_type     cm_act_default_bearer_context_request_ind;
  cm_act_dedicated_bearer_context_request_ind_s_type   cm_act_dedicated_bearer_context_request_ind;
  cm_deact_bearer_context_request_ind_s_type           cm_deact_bearer_context_request_ind;
  cm_bearer_resource_alloc_failure_ind_s_type          cm_bearer_resource_alloc_failure_ind;
  cm_bearer_resource_alloc_reject_ind_s_type           cm_bearer_resource_alloc_reject_ind;
  //cm_bearer_resource_rel_reject_ind_s_type           cm_bearer_resource_rel_reject_ind;
  cm_modify_bearer_context_request_ind_s_type          cm_modify_bearer_context_request_ind;
  cm_pdn_connectivity_reject_ind_s_type                cm_pdn_connectivity_reject_ind;
  cm_pdn_connectivity_failure_ind_s_type               cm_pdn_connectivity_failure_ind;
  cm_pdn_disconnect_reject_ind_s_type                  cm_pdn_disconnect_reject_ind;
  cm_drb_released_ind_s_type                           cm_act_drb_released_ind;
  cm_drb_setup_ind_s_type                              cm_drb_setup_ind;
  cm_drb_reestablish_failed_ind_s_type                 cm_drb_reestablish_failed_ind;
  cm_drb_reestablish_reject_ind_s_type                 cm_drb_reestablish_reject_ind;
  cm_get_pdn_connectivity_request_ind_s_type           cm_get_pdn_connectivity_request_ind;
  cm_bearer_context_modify_reject_ind_s_type           cm_bearer_context_modify_reject_ind;
  cm_pdn_connectivity_packed_request_ind_s_type        cm_pdn_connectivity_packed_request_ind;
  cm_1xCSFB_call_rsp                                   cm_1xcsfb_call_rsp;
  cm_1xCSFB_abort_rsp                                  cm_1xcsfb_abort_rsp;
  cm_esm_notification_ind_s_type                       cm_esm_notif_ind;

} cm_esm_cmd_u_type;


/* This structure is used for receiving CFM indications
** It just has msg_hdr right now, as cfm_fc_cmd_msg_s (cfm.h) isn't
** available on all tagets.TODO: needs to be fixed
*/
typedef struct

{
  msgr_hdr_s             msg_hdr;              /* Message router header */
} cm_cfcm_fc_cmd_msg_s;


//typedef struct cm_cmd
//{
//  q_link_type        link; 
      /*!< Queue link. This must be the first element of the cmd_type. 
         MSGR expects q_link to be the first element and the msgr_hdr_s 
         as the second element.So, after link_type no element should be 
         added and the only element is the union below
      */

//  cm_esm_cmd_s_type  cm_esm_cmd;

  /*! @brief List all the incoming messages/commands*/ 
//} cm_cmd_type;


#endif /* CM_ESM_H */

