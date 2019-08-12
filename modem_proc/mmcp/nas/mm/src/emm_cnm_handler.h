
/*==============================================================================

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_cnm_handler.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity
==============================================================================*/

#ifndef EMM_CNM_HANDLER_H
#define EMM_CNM_HANDLER_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE

#include "comdef.h"
#include "sys.h"
#include "sys_plmn_selection_v.h"
#include "reg_mm_v.h"
#include "lte_rrc_ext_msg.h"
#include "emm_database.h"
#include "mmtask.h"

#define EMM_SMS_TIPD_OFFSET  0
#define EMM_SMS_TYPE_OFFSET  1
#define EMM_SMS_DATA_OFFSET  2

/*===========================================================================

FUNCTION EMM_SEND_MMCNM_EST_CNF

DESCRIPTION
  This function sends MMCNM_EST_CNF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmcnm_est_cnf
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

/*===========================================================================

FUNCTION EMM_SEND_MMCNM_EST_REJ

DESCRIPTION
  This function sends MMCNM_EST_REJ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmcnm_est_rej
( 
  reject_type_enum_T   cause_type,
  byte                 cause_value,
  emm_ctrl_data_type  *emm_ctrl_data_ptr 
);

/*===========================================================================

FUNCTION EMM_SEND_MMCNM_REL_IND

DESCRIPTION
  This function sends MMCNM_REL_IND to CNM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmcnm_rel_ind
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr,
  lower_layer_cause_T  rel_cause  
);

/*===========================================================================

FUNCTION EMM_PROCESS_CNM_COMMAND

DESCRIPTION
  This function processes command from CNM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_process_cnm_command
( 
  mm_cmd_type         *cnm_msg_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr 
);

/*===========================================================================

FUNCTION EMM_PROCESS_DL_NAS_TRANSPORT

DESCRIPTION
  This function passes SMS carried by downlink nas transport message to CNM 
  SMS handler

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_dl_nas_transport
(
  lte_nas_emm_dl_nas_transport  *dl_nas_transport_ptr
);

/*===========================================================================

FUNCTION EMM_PROCESS_UL_SMS

DESCRIPTION
  This function passes uplink SMS messages to NW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_ul_sms
(
  mm_cmd_type         *cnm_msg_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr
);

#endif /* FEATURE_LTE */
#endif /* EMM_CNM_HANDLER_H */




