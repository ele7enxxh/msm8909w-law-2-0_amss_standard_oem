#ifndef RR_TDS_SEND_H
#define RR_TDS_SEND_H

/*! \file rr_tds_send.h 
 
  This is the header file for rr_tds_send.c.
  This module contains functions that send messages from RR to TDS-RRC.
 
                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_tds_send.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_TDS

#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "sys.h"
#include "rr_defs.h"
#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"
#include "rrc_rr_types.h"
#include "rr_candidate_cell_db.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/*!
 * \brief Sends TDSRRC_INTERRAT_RESELECTION_REQ from RR to TDS-RRC.
 * 
 * \param cpid (in)                   - The Cell Parameter ID of the target TDS cell.
 * \param network_selection_mode (in) - The network selection mode that the UE is currently operating in.
 * \param rat_pri_list_info (in)      - The RAT priority list.
 * \param serving_cell_plmn_id (in)   - The ID of the PLMN that the current serving cell belongs to.
 * \param trk_lo_adj (in)             - The last trk_lo_adj value received from GSM L1.
 * \param uarfcn (in)                 - The UARFCN of the target TDS cell.
 * \param gas_id (in)
 */
extern void rr_send_tds_interrat_reselection_req(
  uint16                              cpid,
  sys_network_selection_mode_e_type   network_selection_mode,
  sys_rat_pri_list_info_s_type       *rat_pri_list_info,
  PLMN_id_T                           serving_cell_plmn_id,
  uint16                              trk_lo_adj,
  uint16                              uarfcn,
  const gas_id_t                      gas_id
);

/*!
 * \brief Sends TDSRRC_INTERRAT_RESELECTION_ABORT_REQ from RR to TDS-RRC.
 * 
 * \param gas_id (in)
 */
extern void rr_send_tds_interrat_reselection_abort_req(
  const gas_id_t gas_id
);

/*!
 * \brief Sends TDSRR_INTERRAT_RESELECTION_ABORT_CNF from RR to TDS-RRC.
 * 
 * \param gas_id (in)
 */
extern void rr_send_tds_interrat_reselection_abort_cnf(
  const gas_id_t gas_id
);

/*!
 * \brief Sends TDSRRC_STOP_TDS_REQ from RR to TDS-RRC.
 * 
 * \param cause (in)
 * \param gas_id (in)
 */
extern void rr_send_rrc_stop_tds_mode_req(
  tdsrrc_rr_stop_tds_cause_e_type cause,
  const gas_id_t                  gas_id
);

/*!
 * \brief Sends TDSRR_INTERRAT_RESELECT_REJ from RR to TDS-RRC.
 * 
 * \param failure_cause (in)    - Cause of InterRAT Reselection Failure.
 * \param time_to_reselect (in) - Time (secs) when the same cell should be considered
 * \param gas_id (in) 
 */
extern void rr_send_tds_interrat_reselection_rej(
  tdsrr_interrat_reselect_fail_reason_type failure_cause,
  word                                     time_to_reselect,
  const gas_id_t                           gas_id
);

/*!
 * \brief Send TDSRR_INTERRAT_CC_ORDER_CNF from RR to TDS-RRC.
 * 
 * \param status (in)        - Status of CCO.
 * \param failure_cause (in) - Cause of InterRAT CC Order Failure.
 * \param gas_id (in) 
 */
extern void rr_send_tds_interrat_cc_order_cnf(
  tdsrr_interrat_cc_order_status_e_type    status,
  tdsrr_interrat_cc_order_fail_reason_type failure_cause,
  const gas_id_t                           gas_id
);

/*!
 * \brief Sends TDSRR_INTERRAT_HANDOVER_CNF to TDS-RRC.
 * 
 * \param handover_status (in) - handover status.
 * \param failure_cause (in)   - failure cause, valid if handover_status is FALSE.
 * \param gsm_command_list (in) - GSM commands, only populated when handover fails.
 * \param gas_id (in) 
 */
extern void rr_send_tds_interrat_ho_cnf(
  tdsrrc_rr_status_e_type          handover_status,
  tdsrrc_rr_failure_cause_e_type   failure_cause,
  tdsrrc_rr_gsm_command_list_type *gsm_command_list,
  const gas_id_t                   gas_id
);

/*!
 * \brief Send TDSRR_INTERRAT_HANDOVER_ABORT_CNF to TDS-RRC.
 * 
 * \param abort_status (in) - abort status.
 * \param gas_id (in)
 */
extern void rr_send_tds_interrat_ho_abort_cnf(
  tdsrrc_rr_status_e_type abort_status,
  const gas_id_t          gas_id
);

/*!
 * \brief Sends TDSRR_INTERRAT_HANDOVER_ABORT_CNF to TDS-RRC.
 * 
 * \param gas_id (in)
 */
extern void rr_send_tds_interrat_plmn_search_abort_cnf(
  const gas_id_t gas_id
);

/*!
 * \brief Sends TDSRR_INTERRAT_HANDOVER_ABORT_CNF to TDS-RRC.
 * 
 * \param srch_done (in)
 * \param hplmn_found (in)
 * \param plmn_list (in)
 * \param gas_id (in)
 */
extern void rr_send_tds_interrat_plmn_search_cnf(
  boolean                        srch_done,
  boolean                        hplmn_found,
  sys_detailed_plmn_list_s_type *plmn_list,
  const gas_id_t                 gas_id
);

/*!
 * \brief Send TDSRRC_INTERRAT_PLMN_SRCH_ABORT_REQ to TDS-RRC.
 * 
 * \param gas_id (in)
 */
extern void rr_send_tds_interrat_plmn_search_abort_req(
  const gas_id_t gas_id
);

/*!
 * \brief Sends TDSRRC_INTERRAT_PLMN_SRCH_REQ tp TDS-RRC.
 * 
 * \param plmn_list (in)
 * \param network_select_mode (in)
 * \param timer_val (in)
 * \param camped_suitably (in)
 * \param gas_id (in)
 */
extern void rr_send_tds_interrat_plmn_search_req(
  sys_plmn_rat_list_s_type          *plmn_list,
  sys_network_selection_mode_e_type  network_select_mode,
  uint32                             timer_val,
  boolean                            camped_suitably,
  const gas_id_t                     gas_id
);

/*!
 * \brief Sends TDSRR_INTERRAT_REDIRECT_ABORT_CNF to TDS-RRC.
 * 
 * \param abort_status (in)
 * \param gas_id (in)
 */
extern void rr_send_tds_interrat_redirect_abort_cnf(
  tdsrrc_rr_status_e_type abort_status,
  const gas_id_t          gas_id
);

/*!
 * \brief Send TDSRR_INTERRAT_REDIRECT_REJ to TDS-RRC.
 * 
 * \param failure_reason (in)
 * \param available_plmn_db (in)
 * \param gas_id (in)
 */
extern void rr_send_tds_interrat_redirect_rej(
  tdsrr_interrat_redirect_fail_reason_type  failure_reason,
  rr_available_plmn_db_T                   *available_plmn_db,
  const gas_id_t                            gas_id
);

/*!
 * \brief Send TDSRR_STOP_GSM_CNF to TDS-RRC.
 * 
 * \param gas_id (in)
 */
extern void rr_send_tds_stop_gsm_mode_cnf(
  const gas_id_t gas_id
);

/*!
 * \brief Send TDSRR_INTERRAT_CC_ORDER_ABORT_CNF to TDS-RRC
 * 
 * \param status (in)
 * \param gas_id (in)
 */
extern void rr_send_tds_interrat_cc_order_abort_cnf(
  tdsrrc_rr_status_e_type status,
  const gas_id_t          gas_id
);

/*!
 * \brief Converts TDS RRC Resel Rej to internal format (same as WCDMA).
 * 
 * \param tds_resel_rej (in)  - TDS Resel Rej message 
 * \param rrc_resel_rej (out) - Converted RRC Resel Rej message 
 */
extern void rr_convert_tds_to_rrc_resel_rej(
  tdsrrc_interrat_reselect_rej_type *tds_resel_rej,
  rrc_interrat_reselect_rej_type *rrc_resel_rej
);

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
/*!
 * \brief Sends TDSRRC_INTERRAT_REDIRECT_REQ to TDS-RRC.
 * 
 * \param tdd_target (in)
 * \param rat_pri_list_info (in)
 * \param pseudo_fr_active (in)
 * \param gas_id (in) 
 */
extern void rr_send_tds_interrat_redirection_req(
  tdsrrc_target_info_type      *tdd_target,
  sys_rat_pri_list_info_s_type *rat_pri_list_info,
  boolean                       pseudo_fr_active,
  const gas_id_t                gas_id
);

/**
 * Send the TDSRRC_INTERRAT_REDIRECT_ABORT_REQ primitive to TDSRRC 
 *
 * @param None 
 */
/*!
 * \brief Sends TDSRRC_INTERRAT_REDIRECT_ABORT_REQ to TDS-RRC 
 * 
 * \param gas_id (in)
 */
extern void rr_send_tds_interrat_redirection_abort_req(
  const gas_id_t gas_id
);
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

#ifdef FEATURE_TDSCDMA_DEDICATED_PRI_INFO
/**
  @brief Sends TDSRRC_CLEAR_DEDICATED_PRIORITIES_IND to TDSRRC.
*/
extern void rr_send_tds_clear_dedicated_priorities_ind(
  const gas_id_t gas_id
);

/**
  @brief Sends TDSRRC_DEDICATED_PRIORITIES_REQ to TDSRRC.
*/
extern void rr_send_tds_dedicated_priorities_req(
  const gas_id_t gas_id
);

/**
  @brief Sends TDSRR_DEDICATED_PRIORITIES_RSP to TDSRRC.
*/
extern void rr_send_tds_dedicated_priorities_rsp(
  const gas_id_t gas_id
);
#endif /*FEATURE_TDSCDMA_DEDICATED_PRI_INFO*/

#endif /*FEATURE_GSM_TDS*/
#endif /* RR_TDS_SEND_H */

/* EOF */

