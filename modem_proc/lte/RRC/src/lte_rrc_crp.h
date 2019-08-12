/*!
  @file
  lte_rrc_crp.h

  @brief
  External API for RRC CRP.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_crp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/2010 sureshs Initial version
===========================================================================*/

#ifndef LTE_RRC_CRP_H
#define LTE_RRC_CRP_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <__lte_rrc_crp.h>
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* Allocates and initializes dynamic memory */
extern void lte_rrc_crp_dyn_mem_init(void);

/* Deallocates dynamic memory */
extern void lte_rrc_crp_dyn_mem_deinit(void);

/* Returns the CRP state to other RRC modules */
extern stm_state_t lte_rrc_crp_get_state(void);

/* Returns the connection release reason, if applicable */
extern lte_rrc_conn_rel_reason_e lte_rrc_crp_get_conn_release_reason(void);

/* Returns if load balancing TAU is required */
extern boolean lte_rrc_crp_load_balancing_tau_is_required(void);

extern boolean lte_rrc_crp_redir_info_is_present(void);

#endif /* LTE_RRC_CRP_H */

