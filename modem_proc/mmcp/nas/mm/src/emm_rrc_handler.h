
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_rrc_handler.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/06/09   vdr     Compiler warnings fixed
===========================================================================*/
#ifndef _EMM_RRC_HANDLER_H_
#define _EMM_RRC_HANDLER_H_

#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "lte_rrc_ext_msg.h"
#include "emm_database.h"
#include "emm_rrc_emm_if.h"

/*===========================================================================

                        EXTERNAL DEFINITIONS

===========================================================================*/
extern
void emm_process_rrc_ul_data_cnf
(
  lte_rrc_ul_data_cnf_s *rrc_ul_data_cnf,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
) ;

extern
boolean emm_process_rrc_primitive
(
  void                  *rrc_message_router_cmd, // Changed the i/p type to void *, to be compatible to function pointer
  emm_ctrl_data_type    *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
) ;

extern
void emm_registered_process_rrc_service_ind
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type   *emm_ctrl_data_ptr
);

/*===========================================================================
FUNCTION    EMM_RRC_PROCESS_SERVICE_INDICATION

DESCRIPTION
  This function processes RRC_SERVICE_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_rrc_process_service_indication
(
  lte_rrc_service_ind_s *rrc_service_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
);

extern
void emm_free_dsm_memory_in_rrc_data_ind
(
  lte_rrc_dl_data_ind_s *rrc_data_ind
);

#endif /* _EMM_RRC_HANDLER_H_ */

#endif /*FEATURE_LTE*/

