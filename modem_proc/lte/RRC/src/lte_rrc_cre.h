/*!
  @file
  lte_rrc_cre.h

  @brief
  Header file for RRC CRE Module for use by other modules.

  @detail
  This header contains the constants definition and external function prototype 
  declarations used by the CRE module and its UTF test cases.

*/

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_cre.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/10   da      Added dd_ptr in priv data for usage with dymically allocated mem.
02/10/09   da      Initial Version
===========================================================================*/

#ifndef LTE_RRC_CRE_H
#define LTE_RRC_CRE_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <__lte_rrc_cre.h>



/*===========================================================================

                           EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
extern stm_state_t lte_rrc_cre_get_state
(
  void
);

extern boolean lte_rrc_cre_rlf_search_in_progress
(
  void
);

extern uint64 lte_rrc_cre_get_t311_time_remaining
(
  void
);
#ifdef FEATURE_LTE_ELS_ENABLED
extern boolean lte_rrc_cre_is_els_error_handle_tmr_running
(
    void
);

extern boolean lte_rrc_cre_check_tac_present_in_els_tac_list
(
    uint16
);
#endif
extern void lte_rrc_cre_dd_init
(
  void
);

extern void lte_rrc_cre_dd_deinit
(
  void
);
extern boolean lte_rrc_cre_was_conn_rel_during_suspend
(
  void
);

extern boolean lte_rrc_cre_was_conn_rel_during_closing
(
  void
);

extern lte_rrc_cre_cause_e lte_rrc_cre_get_rlf_cause
(
  void
);
extern uint8 lte_rrc_cre_get_rlf_count_since_lte_active
(
  void
);
extern uint8 lte_rrc_cre_get_rlf_count_since_rrc_connected
(
  void
);


extern lte_rrc_cre_failure_cause_e lte_rrc_cre_get_cre_failure_cause
(
  void
);

extern boolean lte_rrc_cre_get_t311_is_running
(
  void
);

#endif /* LTE_RRC_CRE_H */
