
/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_service_request_lib.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------   
08/22/11   MNK     CR: 301219 Handle CSFB call if TAU is rejected/failed 
07/10/10   MNK     Fix CR  295108 - 1xCSFB:  Load balance connection release received after sending ESR not treated as soft failure 
11/29/10   MNK     More 1XCSFB changes including adversarial scenarios
10/07/10   MNK     1XCSFB introduction 
09/17/10   MNK     CS_NOTIFICATION for 3GPPCSFB added
08/09/10   MNK     Make code compatible for 1xCSFB & 3GPPCSFB
08/30/10   MNK     CSFB changes
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
10/06/09   vdr     Compiler warnings fixed
04/02/09   zren    Added support for new trnsaction ID format 
03/24/09   zren    Created for service request and paging feature
===========================================================================*/
#ifndef _EMM_SERVICE_REQUEST_LIB_H_
#define _EMM_SERVICE_REQUEST_LIB_H_

#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE

#include "emm_database.h"
#include "emm_irat_if_msg.h"
/*===========================================================================

                        TYPE DEFINITION

===========================================================================*/

typedef struct
{
  boolean valid;
  lte_nas_csfb_rsp_type csfb_rsp;
}emm_csfb_rsp;

typedef struct
{
  /*! This will indicate the UL Message type */
  reject_type_enum_T failure_type;

  union 
  {
    lte_irat_1xcsfb_ho_failure_cause_e    onex_failure_cause;

    lte_nas_local_cause_enum_T            lte_nas_failure_cause;

    /* This is used in conjunction with OTA_REJECT_CAUSE of  reject_type_enum_T failure_type*/
    lte_nas_emm_cause_type                lte_nas_ota_emm_cause;
  } failure_cause;
} emm_csfb_failure_cause_type;


/*===========================================================================

                        EXTERNAL DEFINITION

===========================================================================*/
/*===========================================================================
FUNCTION    EMM_START_NAS_SERVICE_REQUEST_PROCEDURE

DESCRIPTION
  This function starts service request procedure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void emm_start_nas_service_request_procedure
(
  emm_ctrl_data_type    *emm_ctrl_data_ptr,
  lte_rrc_est_cause_e    service_req_cause
) ;

/*===========================================================================
FUNCTION    EMM_ABORT_SERVICE_REQUEST

DESCRIPTION
  This function aborts service request procedure. It may also abort the RRC 
  connection and may send ESM failure indication upon the value of given 
  flags.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_abort_service_request
(
  emm_ctrl_data_type             *emm_ctrl_data_ptr,
  boolean                         abort_connection,
  boolean                         service_req_failed,
  lte_nas_local_cause_enum_T      local_cause,
  emm_failure_type                emm_failure_cause
);

/* Function to complete successful service request procedure */
extern void emm_complete_service_request
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
) ;

/* Function to handle service request procedure when timer 3417 expires */
extern void emm_timer_t3417_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
) ;

/* Function to handle the case of service request message sending failure */
extern boolean emm_service_request_failed
(
  lte_emm_connection_cause_type    cause,
  dword                            trans_id,
  emm_failure_type                 emm_failure_cause,
  lte_nas_tai_lst1_type            tac,
  struct emm_pended_msg_list_tag   *message_ptr,
  emm_ctrl_data_type               *emm_ctrl_data_ptr
) ;

/* Function to handle the case of receiving RRC release ind during the service request procedure */
extern void emm_service_request_release_indication
(
  lte_emm_connection_cause_type  release_cause,
  emm_failure_type               emm_failure_cause,
  emm_ctrl_data_type            *emm_ctrl_data_ptr,
  lte_rrc_conn_rel_redir_info_s  redir_info,
  lte_nas_tau_cause_type tau_cause
);

extern void emm_process_buffered_csfb_hard_failure
( 
   void
);
/*===========================================================================
FUNCTION    EMM_PROCESS_SERVICE_REQ_FAILURE

DESCRIPTION
  This function sends failure indication to ESM upon service request procedure
  failure if necessary

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  emm_process_service_req_failure
(
  lte_nas_local_cause_enum_T  local_cause,
  lte_nas_emm_cause_type      emm_cause,
  emm_failure_type            emm_failure_cause,
  emm_ctrl_data_type         *emm_ctrl_data_ptr
);

void emm_start_nas_extended_service_request_procedure
(
  lte_rrc_est_cause_e              service_req_cause, 
  lte_csfb_service_type             svc_type, 
  emm_csfb_rsp            csfb_response,
  emm_esr_type                     esr_type
);

void emm_process_esr_complete(emm_ctrl_data_type* emm_ctrl_data_ptr);

void emm_send_esr_if_buffered(emm_ctrl_data_type* emm_ctrl_data_ptr);
void  emm_process_extended_service_req_failure
(
  emm_csfb_failure_cause_type failure_cause_type,
  emm_ctrl_data_type         *emm_ctrl_data_ptr
);

boolean emm_check_if_csfb_allowed(lte_csfb_service_type   svc_type, emm_esr_type esr_type, emm_ctrl_data_type  *emm_ctrl_data_ptr,boolean *buffer_esr);

void emm_timer_t3417_ext_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);

#ifdef FEATURE_1XSRLTE
extern
void emm_srlte_esr_timer_expire
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
) ;
#endif

void emm_abort_and_clean_csfb_call
(
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);   

lte_nas_local_cause_enum_T emm_convert_rrc_cause_to_local_cause(lte_emm_connection_cause_type cause, emm_ctrl_data_type *emm_ctrl_data_ptr);

void emm_process_cs_service_notification
(
  lte_nas_emm_cs_serv_notification* cs_service_notification,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
); 

void emm_process_rrc_1xcsfb_ho_started_ind(lte_rrc_1xcsfb_ho_started_ind_s* msg_ptr,emm_ctrl_data_type   *emm_ctrl_data_ptr);
void emm_process_rrc_1xcsfb_ho_failed_ind(lte_rrc_1xcsfb_ho_failed_ind_s* msg_ptr,emm_ctrl_data_type   *emm_ctrl_data_ptr);
void emm_process_rrc_1xsrvcc_ho_started_ind(lte_rrc_1xsrvcc_ho_started_ind_s* msg_ptr,emm_ctrl_data_type   *emm_ctrl_data_ptr);
void emm_process_rrc_1xsrvcc_ho_failed_ind(lte_rrc_1xsrvcc_ho_failed_ind_s* msg_ptr,emm_ctrl_data_type   *emm_ctrl_data_ptr);
//boolean mt_csfb_received_on_lte(lte_rrc_ue_id_type_e page_type);

#endif /* _EMM_SERVICE_REQUEST_LIB_H_ */

#endif /*FEATURE_LTE*/

