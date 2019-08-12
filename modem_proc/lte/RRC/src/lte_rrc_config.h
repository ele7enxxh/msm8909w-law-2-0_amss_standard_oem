/*!
  @file
  lte_rrc_config.h

  @brief
  Header file for RRC Config Module for use by other modules.

  @detail
  This header contains the constants definition and external function prototype 
  declarations used by the Config module and its UTF test cases.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_config.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/10   mm      Added APIs to determine if DRB or Meas reconfig is active
04/05/10   da      Added dd_ptr in priv data for usage with dymically allocated mem.
03/12/10   mm      Added API for external modules to check if HO is in progress
08/06/08   da      Initial Version
===========================================================================*/

#ifndef LTE_RRC_CONFIG_H
#define LTE_RRC_CONFIG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <__lte_rrc_config.h> 

/*===========================================================================

                           EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
extern stm_state_t lte_rrc_config_get_state
(
  void
);

extern lte_errno_e lte_rrc_config_get_last_ho_target_cell_info
(
  lte_rrc_cell_info_s *cell_info_ptr,
  boolean* freq_present_ptr
);

extern boolean lte_rrc_config_ho_in_progress
(
  void
);

#ifdef FEATURE_LTE_ELS_ENABLED
extern boolean lte_rrc_config_els_in_progress
(
  void
);
#endif

extern boolean lte_rrc_config_check_hof_trm_unavail
(
);

extern uint32 lte_rrc_config_get_t304_rem_time
(
  void
);
extern uint32 lte_rrc_config_get_t304_value
(
  void
);
extern lte_rrc_osys_RRCConnectionReconfiguration* lte_rrc_config_get_recfg_ota
(
  void
);
extern boolean lte_rrc_config_get_ho_failure_in_closing_state
(
  void
);
extern boolean lte_rrc_config_ho_to_eutra_in_progress
(
  void
);

extern boolean lte_rrc_config_is_first_reconfig_after_cre
(
  void
);

extern boolean lte_rrc_config_is_cqi_ri_periodicity_check_disabled
(
  void
);

extern void lte_rrc_config_dd_init
(
  void
);

extern void lte_rrc_config_dd_deinit
(
  void
);

extern boolean lte_rrc_config_rb_reconfig_in_progress
(
  void
);

extern boolean lte_rrc_config_meas_reconfig_in_progress
(
  void
);

extern boolean lte_rrc_config_mobility_from_eutra_in_progress
(
  void
);

extern boolean lte_rrc_allow_cqi_ant_rel_change_without_full_cfg
(
  void
);

#ifdef FEATURE_LTE_REL11
extern boolean lte_rrc_config_feicic_supported
(
  void
);

#endif

extern boolean lte_rrc_config_rel_scell_cqi
(
  void
);

extern boolean lte_rrc_config_is_intra_cell_ho
(
  void
);

#ifdef FEATURE_LTE_ELS_ENABLED
extern void lte_rrc_config_set_rlf_due_to_els_ies
(
  boolean val
);
#endif

#endif /* LTE_RRC_CONFIG_H */
