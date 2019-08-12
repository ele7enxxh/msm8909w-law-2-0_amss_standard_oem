/*!
  @file
  lte_rrc_misc.h

  @brief
  MISC module's exposed APIs.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_misc.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/

#ifndef LTE_RRC_MISC_H
#define LTE_RRC_MISC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "lte_rrc_int_msg.h"
#include <lte_as.h>
#include <__lte_rrc_misc.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define LTE_RRC_MISC_NUM_CNF_DUE_0 0
#define LTE_RRC_MISC_NUM_CNF_DUE_1 1

#define LTE_RRC_DEFAULT_SUB_ID -1

typedef enum
{
  /*!< Default not reserved */
  NOT_RESERVED                       = 0,

  LTE_RRC_REDIRECTION                = 1,

  LTE_RRC_RESELECTION                = 2,

  LTE_RRC_HANDOVER                   = 3

}lte_rrc_misc_trm_reserve_reason_e;

typedef enum
{
  /*!< Default not released */
  NOT_RELEASED                       = 0,

  LTE_RRC_CSFB_CALL_END              = 1,

  LTE_RRC_REDIRECTION_FAILURE        = 2,

  LTE_RRC_IRAT_ABORT                 = 3,

}lte_rrc_misc_trm_release_reason_e;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/* Allocates dynamic memory for dd_ptr of private data and initializes private 
   data */
extern void lte_rrc_misc_dd_init(void);

/* Deallocates dynamic memory part of private data */
extern void lte_rrc_misc_dd_deinit(void);

/*Updates procedure status based on ack/nack mh is waiting on */
extern void lte_rrc_misc_update_procedure_status(uint32);

extern boolean lte_rrc_misc_get_csfb_call_status(void);

extern boolean lte_rrc_misc_get_trm_status(void);

extern lte_rat_e lte_rrc_misc_get_trm_reserve_rat(void);

extern void lte_rrc_misc_set_trm_release_status(lte_rrc_misc_trm_release_reason_e reason);

extern void lte_rrc_misc_set_trm_reservation_status(lte_rrc_misc_trm_reserve_reason_e reason, lte_rat_e rat);

extern void lte_rrc_misc_reset_trm_status(void);

extern lte_cphy_ue_mode_type_e lte_rrc_misc_get_ue_mode(void);

extern void lte_rrc_misc_update_ue_mode(sys_ue_mode_e_type ue_mode, boolean is_ue_mode_substate_srlte);

extern boolean lte_rrc_misc_get_1xsrlte_mode(void);

extern void lte_rrc_misc_start_trm_high_priority_timer(lte_rrc_trm_client_id_e client,uint64 tmr_val);

extern void lte_rrc_misc_stop_trm_high_priority_timer(lte_rrc_trm_client_id_e client);

extern boolean lte_rrc_misc_pfl_is_applicable(void);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
void lte_rrc_misc_register_nv_refresh_cb
(
  void
);
#endif

extern boolean lte_rrc_misc_is_sglte_mode(void);
#ifdef FEATURE_LTE_ELS_ENABLED
extern void lte_rrc_misc_send_els_ue_ind(void);
#endif
extern void lte_rrc_misc_set_msim_crat_feature_cnf_pending(boolean value);

extern boolean lte_rrc_misc_get_msim_crat_feature_cnf_pending(void);

extern boolean lte_rrc_misc_pend_till_cphy_stop_cnf(byte *msg_ptr, uint32 msg_len);
extern boolean lte_rrc_misc_pend_till_msim_crat_feature_cnf(byte *msg_ptr, uint32 msg_len);
#endif /* LTE_RRC_MISC_H */
