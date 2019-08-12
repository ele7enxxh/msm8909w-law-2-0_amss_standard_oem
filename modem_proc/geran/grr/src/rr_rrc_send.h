#ifndef RR_RRC_SEND_H
#define RR_RRC_SEND_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              rr_rrc_send.h

GENERAL DESCRIPTION
   This module contains functions that send messages from rr to rrc.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_rrc_send.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/03   RG      Added support for G2W blind Handover
10/04/02   Rao     Added InterRAT Reselection ( GSM to WCDMA ) Support.
09/16/02   Rao     Added InterRAT Reselection ( WCDMA to GSM ) Support.
07/22/02   Rao     Created the Module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)

#include "comdef.h"
#include "sys_cnst.h"
#include "rr_rrc_if.h"
#include "rr_candidate_cell_db.h"
#ifdef FEATURE_LTE
#include "lte_irat_types.h"
#endif /* FEATURE_LTE */
#include "geran_multi_sim.h"


/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*-----------------------------------------------------------------------------

  Signals related to G2W Cell Reselection/Packet Cell Change Order

-----------------------------------------------------------------------------*/

#ifdef FEATURE_WCDMA
/**
  @brief Sends RRC_INTERRAT_RESELECTION_REQ from RR to RRC.
*/
extern void rr_send_interrat_reselection_req(
  uint16                             uarfcn,
  uint16                             scrambling_code,
  uint16                             pn_offset,
  sys_network_selection_mode_e_type  network_selection_mode,
  PLMN_id_T                          serving_cell_plmn_id,
  uint16                             trk_lo_adj,
  sys_rat_pri_list_info_s_type      *rat_pri_list_info,
  const gas_id_t                     gas_id
);

/**
  @brief Sends RRC_INTERRAT_RESELECTION_ABORT_REQ from RR to RRC.
*/
extern void rr_send_interrat_reselection_abort_req(
  const gas_id_t gas_id
);

extern void rr_send_rrc_interrat_cc_order_abort_req(
  const gas_id_t gas_id
);


#ifdef FEATURE_INTERRAT_PCCO_GTOW
/**
  @brief Sends RRC_INTERRAT_CC_ORDER_REQ from RR to RRC.
*/
extern void rr_send_interrat_pcco_req(
  uint16                             uarfcn,
  uint16                             scrambling_code,
  sys_network_selection_mode_e_type  network_selection_mode,
  PLMN_id_T                          serving_cell_plmn_id,
  uint32                             t3174_value,
  sys_rat_pri_list_info_s_type      *rat_pri_list_info,
  const gas_id_t                     gas_id
);
#endif /* FEATURE_INTERRAT_PCCO_GTOW */



/*-----------------------------------------------------------------------------

  Signals related to W2G Cell Reselection/Cell Change Order

-----------------------------------------------------------------------------*/

extern void rr_send_interrat_reselection_rej(
  rr_interrat_reselect_fail_reason_type reselect_failure_cause,
  word                                  time_to_reselect,
  const gas_id_t                        gas_id
);

/**
 * Sends an RR_INTERRAT_RESELECTION_ABORT_CNF signal to RRC to confirm that
 * a W2G reselection has been aborted successfully.
 */
extern void rr_send_interrat_reselection_abort_cnf(
  const gas_id_t gas_id
);

extern void rr_send_interrat_cc_order_rej(
  rr_interrat_cc_order_fail_reason_type cc_order_failure_cause,
  const gas_id_t                        gas_id
);

extern void rr_send_interrat_cc_order_cnf(
  rr_interrat_cc_order_status_e_type    status,
  rr_interrat_cc_order_fail_reason_type failure_cause,
  const gas_id_t                        gas_id
);

extern void rr_send_interrat_cc_order_abort_cnf(
  rrc_rr_status_e_type status,
  const gas_id_t       gas_id
);



/*-----------------------------------------------------------------------------

  Signals related to G2W Handover

-----------------------------------------------------------------------------*/

extern void rr_send_interrat_handover_req(
  boolean                                cipherOn,
  byte                                  *utran_msg,
  rr_rrc_interrat_measurement_data_type *measurements,
  sys_rat_pri_list_info_s_type          *rat_pri_list_info,
  boolean                                csfb_call_status,
  const gas_id_t                         gas_id
);

extern void rr_send_interrat_handover_abort_req(
  const gas_id_t gas_id
);



/*-----------------------------------------------------------------------------

  Signals related to W2G Handover

-----------------------------------------------------------------------------*/

extern void rr_send_interrat_ho_abort_cnf(
  rrc_rr_status_e_type abort_status,
  const gas_id_t       gas_id
);

extern void rr_send_interrat_ho_cnf(
  rrc_rr_status_e_type          handover_status,
  rrc_rr_failure_cause_e_type   failure_cause,
  rrc_rr_gsm_command_list_type *gsm_command_list,
  const gas_id_t                gas_id
);



/*-----------------------------------------------------------------------------

  Miscellaneous signals sent to from RR to RRC.

-----------------------------------------------------------------------------*/


extern void rr_send_nas_data_ind(
  SAPI_T          SAPI,
  byte            l3_message_length,
  byte           *l3_message,
  const gas_id_t  gas_id
);

#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH

#ifdef FEATURE_LTE

extern void rr_send_interrat_plmn_search_req(
  sys_plmn_rat_list_s_type          *plmn_list, 
  sys_network_selection_mode_e_type  network_select_mode,
  uint32                             timer_val,
  boolean                            camped_suitably,
  detected_frequency_list_type      *detected_frequency_list,
  const gas_id_t                     gas_id
);

#else

extern void rr_send_interrat_plmn_search_req(
  sys_plmn_rat_list_s_type          *plmn_list, 
  sys_network_selection_mode_e_type  network_select_mode,
  uint32                             timer_val,
  boolean                            camped_suitably,
  const gas_id_t                     gas_id
);

#endif /* FEATURE_LTE */

extern void rr_send_interrat_plmn_search_abort_req(
  const gas_id_t gas_id
);

#endif /* FEATURE_GTOW_BACKGROUND_PLMN_SEARCH */

#ifdef FEATURE_INTERRAT_REDIRECTION_WTOG
extern void rr_send_interrat_redirect_rej(
  rr_interrat_redirect_fail_reason_type  failure_reason,
  rr_available_plmn_db_T                *available_plmn_db,
  const gas_id_t                         gas_id
);
#ifdef FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION
extern void rr_send_interrat_redirect_rej_service_search_success(
  const gas_id_t gas_id
);
extern void rr_send_interrat_redirect_rej_service_search_failure(
  const gas_id_t gas_id
);
#endif /* FEATURE_PCS_850_FREQ_SCAN_OPTIMIZATION */

extern void rr_send_interrat_redirect_abort_cnf(
  rrc_rr_status_e_type  abort_status,
  const gas_id_t        gas_id
);
#endif /* FEATURE_INTERRAT_REDIRECTION_WTOG */

extern void rr_send_interrat_plmn_search_cnf(
  boolean                        srch_done,
  boolean                        hplmn_found,
  sys_detailed_plmn_list_s_type *plmn_list,
  const gas_id_t                 gas_id
);

extern void rr_send_interrat_plmn_search_abort_cnf(
  const gas_id_t gas_id
);

extern void rr_send_cipher_sync_ind(
  const gas_id_t gas_id
);

extern void rr_send_rrc_stop_wcdma_mode_req(
  rrc_rr_stop_wcdma_cause_e_type cause,
  const gas_id_t gas_id
);

extern void rr_send_rrc_stop_gsm_mode_cnf(
  const gas_id_t gas_id
);

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE

extern void rr_send_interrat_gtow_redirection_req(
  rr_rrc_fdd_target_info_type  *fdd_target,
  sys_rat_pri_list_info_s_type *rat_pri_list_info,
  const gas_id_t                gas_id
);

extern void rr_send_interrat_gtow_redirection_abort_req(
  const gas_id_t gas_id
);

#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_FEMTO_GSM_NL
extern void rr_send_rrc_nbr_disc_cnf(
  uint8 num_cells,
  sys_detailed_gsm_list_info_s_type *cell_list
);
extern void rr_send_rrc_start_cosc_cnf(
  boolean camped,
  uint16 cell_id,
  BSIC_T bsic
);
extern void rr_send_rrc_stop_cosc_cnf(
  void
);
extern void rr_send_rrc_service_lost_ind(
  void
);
#endif /* FEATURE_FEMTO_GSM_NL */
#endif /* FEATURE_WCDMA */

extern void rr_get_plmn_list_for_redirection_rej(
  rr_available_plmn_db_T    *available_plmn_db,
  byte                      *num_avail_plmns,
  byte                      *num_available_fplmns,
  PLMN_id_T                 *plmn,
  sys_signal_quality_e_type *signal_quality,
  sys_signal_strength_type  *signal_strength,
  const gas_id_t             gas_id
);


/**
  @brief Sends RRC_CLEAR_DEDICATED_PRIORITIES_IND to WCDMA RRC.
*/
extern void rr_send_rrc_clear_dedicated_priorities_ind(
  const gas_id_t gas_id
);

/**
  @brief Sends RRC_DEDICATED_PRIORITIES_REQ to WCDMA RRC.
*/
extern void rr_send_rrc_dedicated_priorities_req(
  const gas_id_t gas_id
);

/**
  @brief Sends RR_DEDICATED_PRIORITIES_RSP to WCDMA RRC.
*/
extern void rr_send_rrc_dedicated_priorities_rsp(
  const gas_id_t gas_id
);


#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/
#endif /* RR_RRC_SEND_H */

/* EOF */

