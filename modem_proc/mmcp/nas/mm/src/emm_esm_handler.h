
/*!
  @file
  emm_database.h

  @brief
  EMM internal data structure definitions.

*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_esm_handler.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/17/12   cogolt  Rel-10, Added function declaration for emm_send_unblock_all_apns_ind 
12/01/11   SAR     populate emm_failure_type to ESM->CM in failure ind   
10/06/09   vdr     Compiler warnings fixed
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
07/29/09   zren    Added support to always send SIG_CONN_RELEASED_IND to ESM 
                   when RRC_RELEASE_IND is received
05/29/09   zren    Added support for TAU feature
05/14/09   zren    Added function prototype for emm_build_and_send_failure_ind
03/24/09   zren    Added support for service request and paging feature
02/26/09   vdr     Modified prototype of emm_process_incoming_esm_message() in
                   order to fix Lint error
===========================================================================*/

#ifndef _EMM_ESM_HANDLER_H_
#define _EMM_ESM_HANDLER_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE

#include "comdef.h"
#include "lte_nas_common_v.h" 
#include "lte_nas_esm_message_type.h"
#include "emm_database.h"
#include "lte_rrc_ext_msg.h"
#include "emm_esm_if_msg.h"
#include "lte_nas_local_cause.h"
#include "emm_irat_if_msg.h"
/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/


/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/

/*===========================================================================

                        EXTERNAL FUNCTION DEFINITION

===========================================================================*/

extern
boolean emm_process_outgoing_esm_message
(
  void                  *emm_message_router_cmd,//changed the i/p parameter to void* to be compatible to function pointer
  emm_ctrl_data_type    *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
) ;

extern
void emm_process_rrc_eps_bearer_update
(
  lte_rrc_active_eps_bearer_update_ind_s  *eps_bearer_update_ind,
  emm_ctrl_data_type                      *emm_ctrl_data_ptr
) ;

extern
void emm_process_incoming_esm_message
(
  lte_nas_esm_incoming_msg_type *esm_incoming_msg,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
) ;

extern
void emm_build_and_send_detach_ind(lte_nas_local_cause_enum_T local_cause);

void emm_build_and_send_failure_ind (lte_nas_local_cause_enum_T  local_cause, 
                                     lte_nas_emm_cause_type      emm_ota_cause,
                                     dword                       trans_id,
                                     emm_failure_type            emm_failure_cause);
/*===========================================================================

FUNCTION    EMM_BUILD_AND_SEND_BEARER_STATUS

DESCRIPTION
  This function builds and sends NAS_ESM_EPS_BEARER_STATUS_IND to ESM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern
void emm_build_and_send_eps_bearer_status
(
  lte_nas_eps_bearer_context_status_type *eps_bearer_context_status_ptr
) ;

void emm_esm_release_indication
(
  lte_emm_connection_cause_type  release_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr
);

void emm_build_and_send_get_pdn_connectivity_req_ind 
(
  emm_ctrl_data_type     *emm_ctrl_data_ptr
);

void emm_build_and_send_get_isr_status_ind(void);
void emm_build_and_send_isr_status_change_ind(boolean status);

/*===========================================================================
FUNCTION    emm_process_esm_eps_bearer_status_cmd

DESCRIPTION
  This function processes esm_eps_bearer_status_cmd
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_esm_eps_bearer_status_cmd
(
  emm_eps_bearer_status_cmd_T     *emm_eps_bearer_status_cmd_ptr, 
  emm_ctrl_data_type              *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  EMM_ESM_DATA_REQ_FAILED

DESCRIPTION
  This function sends data request failure indication to ESM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
boolean emm_esm_data_req_failed
(
  lte_emm_connection_cause_type    cause,
  dword                            trans_id,
  emm_failure_type                 emm_failure_cause,
  lte_nas_tai_lst1_type            tai,
  emm_pended_msg_list_type        *message_ptr,
  emm_ctrl_data_type              *emm_ctrl_data_ptr
);

#ifdef FEATURE_LTE_REL10
void emm_send_unblock_all_apns_ind(void);
#endif

#ifdef FEATURE_LTE_REL9 
/*===========================================================================
FUNCTION  EMM_PROCESS_EMC_SRV_STATUS_CMD

DESCRIPTION
  This function processes EMC SRV STATUS CMD from ESM
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  emm_process_emc_srv_status_cmd
(
  emm_emc_srv_status_cmd_T  *cmd_ptr, 
  emm_ctrl_data_type        *emm_ctrl_data_ptr
);
#endif

#endif /* _EMM_ESM_HANDLER_H_ */

#endif /*FEATURE_LTE*/

