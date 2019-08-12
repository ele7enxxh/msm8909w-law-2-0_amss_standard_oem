
/*!
  @file
  esm_onex_csfb.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/esm_onex_csfb.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/10/11   HC      Removed AEEstd.h inclusion as aeestdlib is obsoleted 
11/29/10   MNK     More 1XCSFB changes including adversarial scenarios
10/08/10   MNK     1XCSFB introduction 

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "amssassert.h"
#include "rex.h"
#include "msgr_rex.h"
#include "esm_onex_csfb.h" 
#include "esm_cm_msgr_enum_msg.h" 
#include "esm_emm_msgr_enum_msg.h" 
#include "esm_v.h"
#include "esm_utils.h"

/*===========================================================================

FUNCTION  check_and_handle_handle_csfb_cm_message

DESCRIPTION: This function handles 1X CSFB messages from CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean check_and_handle_handle_csfb_cm_message(struct esm_cmd* cmd_ptr, struct esm_1xcsfbm* mgr_ptr)
{
  boolean is_csfb = FALSE;

  ASSERT(cmd_ptr != NULL);
  ASSERT(mgr_ptr != NULL);

  switch(cmd_ptr->cmd.msg_hdr.id)
  {
    case NAS_ESM_1XCSFB_CALL_REQ:
      esm_build_and_send_1xcsfb_call_req(&cmd_ptr->cmd.esm_1xCSFB_call_req,mgr_ptr);
      is_csfb = TRUE;
      break;
    case NAS_ESM_1XCSFB_ABORT_REQ:
      esm_build_and_send_1xcsfb_abort_req(&cmd_ptr->cmd.esm_1xCSFB_abort_req);
      is_csfb = TRUE;
      break;
      default:
        /* Do not do anything as we fall down to the regular LTE processing*/
        break;
  }

  return is_csfb;
}
/*===========================================================================

FUNCTION  check_and_handle_handle_csfb_emm_message

DESCRIPTION: This function handles 1X CSFB messages from EMM 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean check_and_handle_handle_csfb_emm_message(struct esm_cmd* cmd_ptr, struct esm_1xcsfbm* mgr_ptr)
{
  boolean is_csfb = FALSE;

  ASSERT(cmd_ptr != NULL);
  ASSERT(mgr_ptr != NULL);

  switch(cmd_ptr->cmd.msg_hdr.id)
  {
    case NAS_ESM_1XCSFB_ESR_CALL_RSP:
      esm_build_and_send_1xcsfb_call_rsp(&cmd_ptr->cmd.emm_1xCSFB_esr_call_rsp,mgr_ptr);
      is_csfb = TRUE;
      break;
    case NAS_ESM_1XCSFB_ESR_CALL_ABORT_RSP:
      esm_build_and_send_1xcsfb_call_abort_rsp(&cmd_ptr->cmd.emm_1xCSFB_esr_call_abort_rsp,mgr_ptr);
      is_csfb = TRUE;
      break;
      default:
        /* Do not do anything as we fall down to the regular LTE processing*/
        break;
  }

  return is_csfb;
}
/*===========================================================================

FUNCTION  init_esm_1xcsfbm

DESCRIPTION: This function is used to initialize 1X CSFB manager within ESM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void init_esm_1xcsfbm(esm_1xcsfbm_T* mgr_ptr)
{
  ASSERT(mgr_ptr != NULL);
  
  mgr_ptr->check_and_handle_cm_message_ptr = check_and_handle_handle_csfb_cm_message;
  mgr_ptr->check_and_handle_emm_message_ptr = check_and_handle_handle_csfb_emm_message;
  /* As suggested by CM team. This is actually defined as CM_CALL_ID_UNASSIGNED in cm.h. Since we can't include cm.h,
  we are not using that macro, instaed hard coding the value for now      */
  mgr_ptr->csfb_call_id = 0xFE;
  mgr_ptr->trans_id = 0;
}
#endif

