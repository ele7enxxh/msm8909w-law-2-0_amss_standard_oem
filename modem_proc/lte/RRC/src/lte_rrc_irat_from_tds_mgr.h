/*!
  @file
  lte_rrc_irat_from_tds_mgr.h

  @brief
  External API for RRC TDS IRAT Mgr.

*/

/*===========================================================================

  Copyright (c) 2011: Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_irat_from_tds_mgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/11   np      Initial version
===========================================================================*/

#ifndef LTE_RRC_IRAT_FROM_TDS_H
#define LTE_RRC_IRAT_FROM_TDS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <__lte_rrc_irat_from_tds_mgr.h>
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* Returns the target lte cell frequency during T2L psho */
extern lte_earfcn_t lte_rrc_irat_from_tds_mgr_get_target_earfcn_for_t2l_irat(void);

/* Returns if T2L redirection is UE initiated */
extern boolean lte_rrc_irat_from_tds_mgr_is_redir_ue_initiated (void);

/* Allocates and initializes dynamic memory */
extern void lte_rrc_irat_from_tds_mgr_init(void);

/* Deallocates dynamic memory */
extern void lte_rrc_irat_from_tds_mgr_deinit(void);

extern stm_state_t lte_rrc_irat_from_tds_mgr_get_state(void);

/* Returns the stored LTE cell info */
extern void lte_rrc_irat_from_tds_mgr_get_lte_cell_info 
( 
  lte_irat_target_cell_info_s *lte_cell_info_ptr  /*! Ptr to LTE cell info*/
);

/* Returns the requested PLMN in PSHO req */
extern lte_errno_e lte_rrc_irat_from_tds_mgr_get_requested_plmn 
( 
  sys_plmn_id_s_type *req_plmn_ptr
);
#endif /* LTE_RRC_IRAT_FROM_TDS_H */

