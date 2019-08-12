/*!
  @file lte_rrc_irat_utils.h

  @brief
  Miscellaneous utilities for all Inter-RAT managers

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_irat_utils.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/12   sk      Added support for L to G CCO
01/24/10   np      lte_rrc_irat_mgr_utils_is_plmn_valid()
01/24/10   sk      Added support for From-G RAT MGR and G to L Reselection
01/08/10   np      Utility functions to trasnalte NAS & RRC PLMN definition
12/03/09   sk      Updated mode change interface with controller
12/02/09   sk      Changed dest_state to dest_scenario in mode change request
11/07/09   sk      Added suspend-resume cause field to mode change request
09/02/09   sk      Initial version
===========================================================================*/

#ifndef LTE_RRC_IRAT_UTILS_H
#define LTE_RRC_IRAT_UTILS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msgr_umid.h>
#include <msgr_types.h>
#include <sys.h>
#include <lte_as.h>
#include <lte_cphy_msg.h>
#include <lte_rrc_int_msg.h>
#include "lte_rrc_irat_mgr.h"
#include "lte_rrc_log.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define LTE_RRC_IRAT_MIN(A,B) ( (A) < (B) ? (A):(B)) 

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

typedef enum
{
  /*! Default value */
  LTE_RRC_IRAT_TYPE_INITIAL		= 0,
  LTE_RRC_IRAT_TYPE_FROM_1X		= 1,
  LTE_RRC_IRAT_TYPE_FROM_DO		= 2,
  LTE_RRC_IRAT_TYPE_FROM_G		= 3,
  LTE_RRC_IRAT_TYPE_FROM_TDS	= 4,
  LTE_RRC_IRAT_TYPE_FROM_W		= 5,
  LTE_RRC_IRAT_TYPE_TO_1X		= 6,
  LTE_RRC_IRAT_TYPE_TO_DO		= 7,
  LTE_RRC_IRAT_TYPE_TO_G		= 8,
  LTE_RRC_IRAT_TYPE_TO_TDS		= 9,
  LTE_RRC_IRAT_TYPE_TO_W		= 10,
} lte_rrc_irat_type_e;

typedef enum
{
  /*! Default value */
  STATE_DEFAULT		= 0,
  STATE_ENTER			= 1,
  STATE_LEAVE			= 2,
} lte_rrc_irat_state_transition_type_e;

typedef enum
{
	STATE_INITIAL,
  	STATE_WT_FOR_SUSPEND_CNF,
  	STATE_WT_FOR_RESUME_CNF,
	STATE_WT_FOR_ACTIVATE_CNF,
	STATE_WT_FOR_DEACTIVATE_CNF,
	STATE_WT_FOR_LTE_TO_COMPLETE,
	STATE_WT_FOR_ACTIVATION_RSP,
	STATE_WT_FOR_LTE_KEYS,
 	STATE_WT_FOR_UMTS_KEYS,
	STATE_WT_TO_ABORT_DURING_ACTIVATE,
  	STATE_WT_TO_ABORT_DURING_SUSPEND,
  	STATE_WT_TO_ABORT_DURING_RESUME,
  	STATE_WT_FOR_ABORT_CNF,
  	STATE_WT_FOR_ABORT_RSP,
  	STATE_WT_FOR_CONN_REL,
  	STATE_IRAT_TO_IN_PROGRESS,
	STATE_WT_TO_SUSPEND_DURING_SUSPEND,
} lte_rrc_irat_transition_state;

/*! @brief Enum to represent various redirection phases
*/
typedef enum
{
    /* Value none */
    LTE_RRC_TO_LTE_PROC_REDIR_NONE = 0,

    /*! Redirection - Initial Phase */
    LTE_RRC_TO_LTE_PROC_REDIR_LIST_SRCH = 1,

    /*! Redirection - Neighbor List / Full Search */
    LTE_RRC_TO_LTE_PROC_REDIR_FULL_SRCH = 2

}lte_rrc_to_lte_redirection_phase_type_e;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
extern void lte_rrc_irat_mgr_utils_send_mode_change_req
(
  lte_rrc_mode_change_e            mode,          /*!< Requested mode */
  lte_rrc_dest_scenario_e          dest_scenario,  /*!< Requested dest scenario */
  sys_lte_band_mask_e_type         lte_band_pref  /*!< LTE band pref, populated 
                                                      only for Activate, else its 0*/  
);

extern sys_plmn_id_s_type lte_rrc_irat_mgr_utils_to_nas_plmn_id
( 
  lte_rrc_plmn_s                    plmn_id 
);

extern boolean lte_rrc_irat_mgr_utils_is_plmn_valid 
( 
  sys_plmn_id_s_type                plmn 
);

extern boolean lte_rrc_irat_mgr_utils_compare_plmn_ids
(
  const sys_plmn_id_s_type *sys_plmn_ptr, /*!< PLMN ID 1 */
  const lte_rrc_plmn_s *plmn_ptr          /*!< PLMN ID 2 */
);

extern void lte_rrc_irat_mgr_utils_print_nwk_sel_mode 
( 
  sys_network_selection_mode_e_type mode 
);

extern void lte_rrc_irat_mgr_utils_print_irat_to_lte_fail_cause 
( 
  lte_rrc_irat_to_lte_failure_cause_type_e fail_cause
);

extern lte_errno_e lte_rrc_irat_validate_mobility_from_eutra_msg
(
  /*!< Pointer to the DL DCCH message received from Message Handler*/
  lte_rrc_osys_DL_DCCH_Message  *msg_ptr
);

extern void lte_rrc_irat_send_plmn_srch_resume_failed_indi
(
  void
);

extern boolean lte_rrc_irat_mgr_utils_dedicated_pri_plmn_match
(
  sys_plmn_id_s_type                plmn_id
);

extern boolean lte_rrc_irat_to_DO_mgr_get_pre_reg_status_info (void);

extern lte_rrc_irat_proc_e lte_rrc_irat_to_g_mgr_get_current_proc(void);

extern void lte_rrc_irat_to_G_mgr_update_high_speed_flag(boolean);

extern void lte_rrc_irat_mgr_utils_get_lte_cell_info_for_ho 
( 
  /*! Ptr to LTE cell info*/
  lte_irat_target_cell_info_s *lte_cell_info_ptr 
);

extern lte_errno_e lte_rrc_irat_validate_mobility_to_eutra_msg
(
  /*!< Pointer to the DL DCCH message */
  lte_rrc_osys_DL_DCCH_Message  *msg_ptr
);

extern void lte_rrc_irat_log_psho_to_eutra_msg
(
  lte_rrc_osys_DL_DCCH_Message *decoded_msg_ptr, /*!< Ptr to decoded OTA msg */
  unsigned char *encoded_msg_ptr,  /*!< Ptr to encoded OTA msg */
  uint32 msg_len  /*!< Encoded msg length */
);

extern void lte_rrc_irat_send_conn_mode_failure_indi 
(
  lte_rrc_conn_mode_failure_cause_e conn_mode_failure_cause,
  lte_rrc_ho_failure_cause_e        ho_failure_cause
);

extern void lte_rrc_to_irat_utils_send_abort_cnfi 
( 
  lte_rrc_ml1_status_type_e        ml1_status, 
  lte_rat_e                        rat,
  lte_rrc_irat_proc_e              irat_proc_in_progress,
  lte_rrc_irat_abort_cause_type_e  irat_abort_cause,
  lte_rrc_proc_e                   abort_proc
);

extern void lte_rrc_irat_mgr_utils_send_act_ind_to_NAS (lte_rrc_act_reason_e act_reason);

extern void lte_rrc_irat_mgr_utils_send_act_ind_to_CM 
(
  lte_rrc_act_reason_e act_reason,
  lte_rrc_irat_rat_type_e src_rat
);

extern void lte_rrc_to_irat_utils_send_redir_failed_ind ( lte_rat_e rat );

extern void lte_rrc_to_irat_utils_send_resel_failed_indi_to_CSP( void );

extern void lte_rrc_irat_mgr_utils_log_redir_start_event
( 
  lte_rrc_log_rat_type_e rat 
);

extern void lte_rrc_irat_mgr_utils_log_redir_end_event
(
  lte_rrc_log_rat_type_e rat,
  lte_rrc_log_from_eutran_redir_end_cause_e cause
);

extern void lte_rrc_irat_mgr_utils_log_resel_end_event
(
  lte_rrc_log_rat_type_e rat,
  lte_rrc_log_from_eutran_resel_end_cause_e cause
);

extern void lte_rrc_irat_mgr_utils_send_irat_resel_completed_indi 
(
  void 
);

extern void lte_rrc_irat_mgr_utils_send_irat_redir_completed_indi 
(
  void 
);

extern void lte_rrc_irat_mgr_utils_send_irat_resel_abort_reqi 
(
  void 
);

extern void lte_rrc_irat_mgr_output
(
  lte_rrc_irat_type_e type,
  lte_rrc_irat_state_transition_type_e transition,
  lte_rrc_irat_transition_state state
);

extern void lte_rrc_irat_mgr_utils_send_dedicated_pri_list
(
  lte_irat_dedicated_priority_info_s *dedicated_priority_info,
  sys_plmn_id_s_type                  plmn
);

extern void lte_rrc_irat_mgr_utils_send_trm_release_req
(
  void
);

extern uint32 lte_rrc_irat_mgr_get_t304 
( 
  lte_rrc_irat_type_e type,
  lte_rrc_osys_RRCConnectionReconfiguration *recfg_ptr
);

extern boolean lte_rrc_irat_mgr_trm_exchange_needed 
(
  lte_rrc_dest_scenario_e dest_scenario
);

extern lte_earfcn_t lte_rrc_irat_mgr_get_target_cell_earfcn_for_x2l_irat
( 
  void
);

extern sys_lte_band_mask_e_type lte_rrc_irat_mgr_get_lte_band_pref_from_irat_nas_info
( 
  sys_rat_pri_list_info_s_type        *rat_pri_list_ptr
);

extern lte_earfcn_t lte_rrc_irat_mgr_get_target_cell_earfcn_for_x2l_ue_initiated_redir 
(
  sys_rat_pri_list_info_s_type        *rat_pri_list_ptr
);

extern boolean lte_rrc_irat_utils_fast_redirection_supported
(
  uint8 num_g_internal_redir_freq
);

extern boolean lte_rrc_from_irat_proc_deact_is_in_progress
(
void
);

extern boolean lte_rrc_from_irat_proc_is_in_progress 
(
  void
);

extern void lte_rrc_to_irat_utils_is_releasing_rat_valid
(
  lte_rat_e rat
);

extern void lte_rrc_irat_send_redir_abort_req_to_csp
(
  void 
);
#endif /* LTE_RRC_IRAT_UTILS_H */

