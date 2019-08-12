/*!
  @file
  lte_rrc_controller.h

  @brief
  External API for RRC controller.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_controller.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/2009 sureshs Support for CDMA time info
08/20/2009 sureshs Added accessor for querying ML1 state
10/13/2008 sureshs Added accessors for SIM update info
07/28/2008 sureshs Initial version
===========================================================================*/

#ifndef LTE_RRC_CONTROLLER_H
#define LTE_RRC_CONTROLLER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <__lte_rrc_controller.h>
#include <lte_rrc_ext_msg.h>
#include <lte_cphy_msg.h>
#include <lte_rrc_int_msg.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/* Returns the state of RRC Controller */
extern stm_state_t lte_rrc_controller_get_state(void);

/* Returns if UE ID list is valid (from sim update) */
extern boolean lte_rrc_controller_ue_id_list_is_valid(void);

/* Returns pointer to UE ID list (from sim update), check if valid with 
lte_rrc_controller_ue_id_list_is_valid */
extern const lte_rrc_ue_id_list_s *lte_rrc_controller_get_ue_id_list(void);

/* Returns access classes (from sim update), 0xFFFF if not assigned */
extern uint16 lte_rrc_controller_get_access_class(void);

extern boolean lte_rrc_controller_get_eab_enabled(void);

/* Returns CDMA time info - valid after receiving Mode Change Confirm for a Mode
   Change Request with mode = LTE_RRC_MODE_CHANGE_SUSPEND and 
   scenario LTE_CPHY_IRAT_DO_RESEL */
extern const lte_stmr_cdma_rtc_pair *lte_rrc_controller_get_cdma_time_info
(
  void
);

/* Returns WCDMA Time info - valid after receiving Mode Change Cnf 
   for mode change SUSPEND and scenario L2W PS Handovers */
const lte_cphy_wcdma_time_info_s *lte_rrc_controller_get_wcdma_time_info
(
  void
);

/* Returns TDSCDMA Time info - valid after receiving Mode Change Cnf 
   for mode change SUSPEND and scenario L2T PS Handovers */
const lte_cphy_tdscdma_time_info_s *lte_rrc_controller_get_tdscdma_time_info
(
  void
);

/* Returns GSM Time info - valid after receiving Mode Change Cnf 
   for mode change SUSPEND and scenario L2G  Handovers */
const lte_cphy_gsm_time_info_s *lte_rrc_controller_get_gsm_time_info
(
  void
);
/* Allocates and initializes dynamic memory */
extern void lte_rrc_controller_dyn_mem_init(void);

/* Deallocates dynamic memory */
extern void lte_rrc_controller_dyn_mem_deinit(void);

/* Determines if IRAT CGI is in progress */
extern boolean lte_rrc_ctlr_irat_cgi_in_progress(void);

/* Determines if IRAT BPLMN search is in progress */
extern boolean lte_rrc_ctlr_irat_bplmn_srch_in_progress (void);

/* Determines if UE should handle tune aways*/
extern boolean lte_rrc_controller_tune_aways_enabled(void);

extern void lte_rrc_controller_get_cphy_rf_status_info
(
  lte_rrc_rf_status_info_s *info_ptr
);

extern errno_enum_type lte_rrc_ctlr_get_susp_rsme_time_info
(
  uint32 *elapsed_time,
  lte_l1_sfn_t *susp_sfn,
  lte_l1_sfn_t *rsme_sfn
);

extern msgr_umid_type lte_rrc_ctlr_get_deadlock_tmr_cphy_umid 
(
  void
);

extern sys_modem_device_mode_e_type lte_rrc_controller_get_device_mode(void);

extern sys_modem_dual_standby_pref_e_type lte_rrc_controller_get_standby_mode (void);

extern lte_trm_priority_e lte_rrc_controller_get_trm_priority(void);

extern lte_rrc_ctlr_cphy_stop_status lte_rrc_ctlr_ml1_stopped_status(void);

extern boolean lte_rrc_ctlr_get_single_rx_mode
(
  void
);

extern boolean lte_rrc_controller_is_non_dds_mode_enabled(void);

#endif /* LTE_RRC_CONTROLLER_H */

