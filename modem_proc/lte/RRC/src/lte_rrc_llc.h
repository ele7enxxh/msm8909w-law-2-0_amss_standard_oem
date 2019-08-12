/*!
  @file
  lte_rrc_llc.h

  @brief
  External API for RRC LLC.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_llc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/10   mm      Exported API for obtaining dedicated RLF timers, if configured
06/21/10   mm      Exported an API to determine if SRB2 and a DRB are active
06/15/10   mm      Exported the API to get source cell info during handover
04/05/10   da      Added dd_ptr in priv data for usage with dymically allocated mem.
03/12/10   mm      Removed API for external modules to check if HO is in progress
10/24/08   mm      Added API prototype that returns active EPS bearer list
10/09/08   da      Added support for processing LTE_RRC_CFG_REQI for Idle state.
08/12/08   da      Created initial file
===========================================================================*/

#ifndef LTE_RRC_LLC_H
#define LTE_RRC_LLC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <__lte_rrc_llc.h>
#include "lte.h"
#include "lte_as.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_ext_api.h"
#include "lte_rrc_int_msg.h"
#include "lte_rrc_config.h"
#include "lte_rrc_osys_asn1util.h"

/*===========================================================================

                              LOCAL VARIABLES

===========================================================================*/

/*! @brief Structure for state-machine per-instance extern variables
*/
typedef struct
{
  lte_eps_id_t eps_id;
  lte_rb_id_t rb_id;
} lte_rrc_eps_rb_mapping_s;


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern stm_state_t lte_rrc_llc_get_state
(
  void
);

extern void lte_rrc_llc_get_epsbearer_info
(
  lte_rrc_active_eps_bearer_update_ind_s *eps_list_msg_ptr
);

extern lte_rb_cfg_idx_t lte_rrc_llc_get_rb_cfg_idx_for_srb
(
  lte_rb_id_t rb_id  /*!< Radio bearer ID */
);

extern lte_rb_id_t lte_rrc_llc_get_srb_id_from_rb_cfg_idx
(
  lte_rb_cfg_idx_t rb_cfg_idx  /*!< rb configuration index */
);

extern boolean lte_rrc_llc_is_srb2_established
(
  void
);

extern void lte_rrc_llc_get_ho_source_cell_info
(
  lte_rrc_cell_info_s *cell_info_ptr /*!< Cell info pointer */
);

extern lte_errno_e lte_rrc_llc_get_target_cell_info
(
  lte_rrc_cell_id_s *cell_id_ptr /*!< Cell info pointer */
);

extern lte_rnti_val_t lte_rrc_llc_get_crnti
(
  void
);

extern lte_errno_e lte_rrc_llc_update_crnti_in_cfgdb
(
  lte_rnti_val_t crnti_val  /*!< CRNTI value to be updated to the Cfgdb */
);

extern lte_errno_e lte_rrc_llc_update_ul_earfcn
(
  lte_earfcn_t dl_earfcn, /*!< DL EARFCN */
  lte_earfcn_t *ul_earfcn_ptr /*!< UL EARFCN */
);

extern boolean lte_rrc_llc_srb2_and_drb_are_active
(
  void
);

extern void lte_rrc_llc_dd_init
(
  void
);

extern void lte_rrc_llc_dd_deinit
(
  void
);

extern boolean lte_rrc_llc_get_dedicated_rlf_timers
(
  lte_rrc_rlf_tmr_s *rlf_tmr_ptr /*!< the RLF timers maintained by RRC */
);

extern void lte_rrc_llc_get_pmch_datamcs_info
(
  lte_rrc_datamcs_info_s *datamcs_info_ptr /*!< Ptr to datamcs info */
);

extern boolean lte_rrc_llc_is_mib_changed_after_ho
(
  lte_l1_mib_info_param_s *rcvd_mib
);

extern void lte_rrc_llc_send_mbsfn_cfg_changed_indi
(
  boolean mbsfn_cfg_status
);

extern uint8 lte_rrc_llc_check_freq_match_with_scell_in_cfg_db
(
  lte_earfcn_t carrier_freq
);

extern lte_rrc_cell_type_e lte_rrc_llc_check_freq_match_with_pcell_scell_freq
(
  lte_earfcn_t carrier_freq , /*!< carrier freq which needs to be checked*/
  lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs* conn_recfg_ptr /*!< reconfig msg ptr*/
);

extern boolean lte_rrc_llc_check_freq_match_with_scell
(
  lte_earfcn_t carrier_freq,
  lte_rrc_osys_RRCConnectionReconfiguration_r8_IEs *conn_recfg_ptr /*!< reconfig msg ptr*/
);

extern void lte_rrc_llc_get_updated_scell_info
(
  lte_rrc_interfreq_list_update_indi_s *scell_info_ptr
);

extern boolean lte_rrc_llc_mbsfn_area_cfg_is_valid
(
  lte_mbsfn_area_id_t area_id
);

extern lte_errno_e lte_rrc_llc_process_r11_ssfp_tdd_cfg
(
lte_l1_tdd_special_subframe_pattern_e*  special_sub_fn_patterns,
lte_rrc_osys_TDD_Config_specialSubframePatterns* ota_specialsubframepatterns,
lte_rrc_osys_TDD_Config_v1130_specialSubframePatterns_v1130* ota_specialsubframepatterns_v1130
);
extern lte_errno_e lte_rrc_llc_process_ssfp_tdd_cfg
(
lte_l1_tdd_special_subframe_pattern_e*  special_sub_fn_patterns,
lte_rrc_osys_TDD_Config_specialSubframePatterns* ota_specialsubframepatterns
);
extern lte_errno_e lte_rrc_llc_process_sfa_tdd_cfg
(
lte_rrc_osys_TDD_Config_subframeAssignment* ota_subframeassignment,
lte_l1_tdd_ul_dl_cfg_index_e* subframe_assign
);

extern boolean lte_rrc_llc_get_sib2_mbsfn_cfg_is_valid
(
  void
);

extern lte_errno_e lte_rrc_llc_get_serving_cell_ul_earfcn
(
  lte_earfcn_t* ul_earfcn_ptr
);

extern boolean lte_rrc_llc_pmch_active_on_cell
(
  lte_rrc_cell_id_s cell_info
);

extern boolean lte_rrc_llc_eICIC_pattern_enabled
(
  void
);

extern uint16 lte_rrc_llc_get_t310
(
  void
);
extern lte_errno_e lte_rrc_llc_get_ca_scell_list
(
  lte_rrc_ca_scell_list_s* scell_list_info_ptr /*!< Scell List Info ptr*/
);
#ifdef FEATURE_LTE_ELS_ENABLED
extern boolean lte_rrc_llc_check_volte_call
(
  void
);
#endif

#endif /* LTE_RRC_LLC_H */
