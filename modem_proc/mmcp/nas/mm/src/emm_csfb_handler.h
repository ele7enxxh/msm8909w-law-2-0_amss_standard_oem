/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_csfb_handler.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef _EMM_CSFB_HANDLER_H_
#define _EMM_CSFB_HANDLER_H_

#include "mmcp_variation.h"
#include<customer.h>

#ifdef FEATURE_LTE

#include "emm_irat_if_msg.h"
#include "mm_v.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/

/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/

typedef void (*rrc_irat_req_failure_cb_type)
(
  emm_irat_failure_cause_type          cause_type,
  byte                                 cause,
  dword                                trans_id,
  uint64                               ac_rem_time
);
 
typedef struct emm_pending_irat_ul_msg_tag
{
  lte_irat_tunnel_ul_msg_req_s         msg;
#ifdef FEATURE_LTE_TO_HDR_OH 
  lte_irat_cdma_tunnel_ul_msg_s        irat_ul_msg;
  uint8                                num_dsm_ptr;
  dsm_item_type                       *dsm_ptr[LTE_RRC_CDMA_TUNNEL_MAX_DSM_ARRAY_SIZE];
#endif
  dword                                trans_id;
  rrc_irat_req_failure_cb_type         failure_cb;
  /* Flag indicating that the message should be processed upon receiving 
     release indication due to connection abortion */
  boolean                              process_upon_conn_abort;
  struct emm_pending_irat_ul_msg_tag  *next_msg_ptr;

}emm_pending_irat_ul_msg_type;

/*===========================================================================

                        FUNCTION PROTOTYPES 

===========================================================================*/

/*===========================================================================
FUNCTION  EMM_CONN_ABORT_MARK_PENDING_IRAT_MSG

DESCRIPTION
  This function marks all existing pending IRAT messages to be processed upon 
  receiving release indication with cause of aborted 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern
void emm_conn_abort_mark_pending_irat_msg
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION    EMM_PROCESS_RRC_IRAT_TUNNEL_DL_MSG_IND

DESCRIPTION
  This function processes LTE_RRC_IRAT_TUNNEL_DL_MSG_IND. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern
void emm_process_rrc_irat_tunnel_dl_msg_ind
(
  lte_rrc_irat_tunnel_dl_msg_ind_s  *rrc_ind_ptr,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
);

#ifdef FEATURE_LTE_TO_HDR_OH 
/*=========================================================================== 
 
FUNCTION    EMM_PROCESS_RRC_IRAT_3GPP2_TUNNEL_DL_MSG_IND

DESCRIPTION
  This function processes LTE_RRC_IRAT_HDR_TUNNEL_DL_MSG_IND. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern
void emm_process_rrc_irat_3gpp2_tunnel_dl_msg_ind
(
  lte_irat_cdma_tunnel_dl_msg_ind_s  *rrc_ind_ptr,
  emm_ctrl_data_type                 *emm_ctrl_data_ptr
);

#endif

/*=========================================================================== 
 
FUNCTION  EMM_SEND_IRAT_IMSI_ATTACH_IND

DESCRIPTION
  This function sends EMM_NAS_IRAT_IMSI_ATTACH_IND when combined attach is
  completed successfully

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern
void emm_send_irat_imsi_attach_ind(void);

/*=========================================================================== 
 
FUNCTION  EMM_SEND_IRAT_CTXT_LOST_IND

DESCRIPTION
  This function sends EMM_NAS_IRAT_CTXT_LOST_IND with given cause

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern
void emm_send_irat_ctxt_lost_ind
(
  emm_irat_ctxt_lost_cause_type  cause 
);

/*===========================================================================

FUNCTION  EMM_PROCESS_RRC_IRAT_TUNNEL_UL_MSG_CNF

DESCRIPTION
  This function processes LTE_RRC_IRAT_TUNNEL_UL_MSG_CNF
  - Failure: Calls registered failure call back funtion and delete the message
             from pending message list
  - Success: Delete the message from pending message liest 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern
void emm_process_rrc_irat_tunnel_ul_msg_cnf
(
  lte_rrc_irat_tunnel_ul_msg_cnf_s  *rrc_cnf_ptr,
  emm_ctrl_data_type                *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION  EMM_SEND_ALL_PENDING_IRAT_MSG

DESCRIPTION
  This function sends all messages in the pending IRAT message list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern
void emm_send_all_pending_irat_msg
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  EMM_PROCESS_PENDING_IRAT_MSG

DESCRIPTION
  This function traverse the pending IRAT message list and processes messages
  if needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern
void emm_process_pending_irat_msg
(
  byte                 cause_type,
  byte                 cause,
  uint64               ab_rem_time,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*=========================================================================== 
 
FUNCTION  EMM_PROCESS_CSFB_PRIMITIVE

DESCRIPTION
  This function processes CSFB primitives

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
extern
boolean emm_process_csfb_primitive
(
  void                   *cmd_ptr,
  emm_ctrl_data_type     *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp  **mm_emm_dispatcher_fn_ptr
);

#endif /*FEATURE_LTE*/
#endif /* _EMM_CSFB_HANDLER_H_ */



