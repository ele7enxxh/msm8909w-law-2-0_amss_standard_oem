
/*!
  @file
  lte_rrc_esmgr.h

  @brief
  ESMGR's exposed APIs.
*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_esmgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/11   da      Initial Version
===========================================================================*/

#ifndef LTE_RRC_ESMGR_H
#define LTE_RRC_ESMGR_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <lte_rrc_int_msg.h>
#include <__lte_rrc_esmgr.h> 

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/





/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* Allocates dynamic memory for dd_ptr of private data and initializes private 
   data */
extern void lte_rrc_esmgr_dd_init(void);

/* Deallocates dynamic memory part of private data */
extern void lte_rrc_esmgr_dd_deinit(void);

/* Returns the current state of ESMGR module */
extern stm_state_t lte_rrc_esmgr_get_state(void);

/* Returns TRUE if eMBMS has been enabled*/
extern boolean lte_rrc_esmgr_embms_is_enabled(void);

/* Returns the OOS warning reason */
extern lte_rrc_emp_warn_reason_e lte_rrc_esmgr_get_oos_warn_reason(void);

/* Determines if an EARFCN is part of CFL */
extern boolean lte_rrc_esmgr_earfcn_in_cfl
(
   lte_earfcn_t freq /* EARFCN */
);

/* Determines if eMBMS coverage is available or not */
extern boolean lte_rrc_esmgr_in_embms_coverage(void);

/* Returns whether the embms TMGIs match or not*/
extern boolean lte_rrc_esmgr_compare_tmgi
(
  lte_rrc_embms_session_info_s tmgi1,
  lte_rrc_embms_session_info_s tmgi2
);

/* Returns whether the embms is preferred of not */
extern boolean lte_rrc_esmgr_embms_is_preferred
(
  void
);

/* Returns whether the SAI list is active or not */
extern boolean lte_rrc_esmgr_active_sai_list_is_valid
(
  void
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#endif /* LTE_RRC_ESMGR_H */
