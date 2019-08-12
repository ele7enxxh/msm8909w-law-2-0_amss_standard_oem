
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_procedure_reject_handler.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/10   zren    Fixed lint warnings
09/24/09   zren    Fixed compilation warning of  "no newline at end of file"
03/24/09   zren    Created for service request and paging feature
===========================================================================*/
#ifndef _EMM_PROCEDURE_REJECT_HANLDER_H_
#define _EMM_PROCEDURE_REJECT_HANLDER_H_

#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "lte_nas_emm_message_description.h"
#include "lte_nas_emm_message_type.h"


/*===========================================================================

                        TYPE DEFINITION

===========================================================================*/


/*===========================================================================

                        EXTERNAL DEFINITION

===========================================================================*/

/* Function to process service reject message */
extern
void emm_process_reject_message
(
  lte_nas_emm_incoming_msg_type  *incoming_message,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
) ;

/*===========================================================================

FUNCTION    EMM_PROCESS_REJECT_MESSAGE_CAUSE

DESCRIPTION
  This function processes EMM cause received as part of TAU Reject message and
  Service Reject message according to TS 24.301 5.5.3.2.5 & 5.6.1.5

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See below
===========================================================================*/
void emm_process_reject_message_cause
(
  lte_nas_message_id       msg_id,   /* Will be used when TAU feature is implemented */ 
  lte_nas_emm_cause_type   emm_cause,
#ifdef FEATURE_LTE_REL10
  dword                    t3346_val,
#endif
  emm_ctrl_data_type      *emm_ctrl_data_ptr
);

#endif /* _EMM_PROCEDURE_REJECT_HANLDER_H_ */

#endif /*FEATURE_LTE*/
