#ifndef RR_RESEL_UTILS_H
#define RR_RESEL_UTILS_H

/** @file */

/*============================================================================
  @file rr_resel_utils.h

  This is the header file for the rr_gprs_reselection_utils.c module.

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_resel_utils.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "rr_gprs_defs.h"
#include "rr_candidate_cell_db.h"
#include "rr_resel_calcs.h"

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

extern void rr_gprs_init_best_six(const gas_id_t gas_id);

extern void rr_clear_all_best_six_snr_markers(const gas_id_t gas_id);

extern void rr_gprs_change_bcch_best_six_bsic(
  ARFCN_T arfcn,
  BSIC_T  new_bsic,
  const gas_id_t gas_id
);

extern gsm_ncell_list_member_t * rr_gprs_get_best_six_by_arfcn(
  ARFCN_T  arfcn,
  BSIC_T * bsic,
  const gas_id_t gas_id
);

extern rr_resel_candidate_t * rr_resel_best_six_find(
  ARFCN_T arfcn,
  BSIC_T * bsic,
  const gas_id_t gas_id
);

extern void rr_gprs_add_to_best_six(
  ARFCN_T arfcn,
  BSIC_T bsic,
  boolean start_ptimer,
  const gas_id_t gas_id
);

extern rr_timer_id_T * rr_gprs_return_penalty_timer_id(
  uint16 index,
  const gas_id_t gas_id
);

extern void rr_gprs_remove_from_best_six(
  ARFCN_T arfcn,
  BSIC_T  bsic,
  const gas_id_t gas_id
);

extern void rr_remove_all_best_six_ncells(const gas_id_t gas_id);

extern boolean rr_gprs_is_ncell_in_best_six(
  surrounding_channel_info_T * meas_chan_info_ptr,
  const gas_id_t gas_id
);

extern rr_timer_id_T rr_gprs_get_gprs_penalty_timer_id(const gas_id_t gas_id);

extern rr_timer_id_T rr_gprs_get_five_second_timer_id(const gas_id_t gas_id);

extern boolean rr_gprs_resel_suitable_cell(
  bcch_ncell_info_t * ptr_surround_database,
  const gas_id_t gas_id
);

extern void rr_gprs_update_penalty_timer_state(
  rr_timer_id_T penalty_timer_id,
  const gas_id_t gas_id
);

extern void rr_gprs_update_five_second_timer_state(
  rr_timer_id_T five_sec_timer_id,
  const gas_id_t gas_id
);

extern void rr_gprs_start_five_second_timer(
  reselection_info_T * rr_gprs_reselection_measurements,
  const gas_id_t gas_id
);

extern void rr_gprs_stop_five_second_timer(
  reselection_info_T * rr_gprs_reselection_measurements,
  const gas_id_t gas_id
);

extern void rr_gprs_cancel_all_five_second_timers(const gas_id_t gas_id);

extern void rr_gprs_create_expired_penalty_timer(
  rr_resel_candidate_t * resel_candidate,
  const gas_id_t gas_id
);

extern boolean rr_gprs_surrounding_cell_in_best_six(
  gsm_ncell_list_member_t * ncell_ptr,
  best_six_info_T         * p_best_six_info,
  boolean                 * bsic_changed_ptr,
  BSIC_T                  * newBSIC,
  const gas_id_t            gas_id
);

extern void rr_resel_clear_any_active_five_sec_timer(
   const gas_id_t gas_id );
   
extern void rr_gprs_remove_serving_cell_from_surrounding_measurement(
  ARFCN_T serving_BCCH_ARFCN,
  best_six_info_T *p_best_six_info
);

extern void rr_gprs_add_sync_cell(
  ARFCN_T arfcn,
  const gas_id_t gas_id
);

extern void rr_gprs_clear_sync_cell_list(const gas_id_t gas_id);

extern boolean rr_gprs_is_cell_in_sync_list(ARFCN_T arfcn, const gas_id_t gas_id);

extern boolean rr_gprs_is_scell_bsic_unchanged(
  mph_serving_idle_meas_ind_T * new_message,
  const gas_id_t gas_id
);

extern void rr_gprs_generate_rps_measurement_report(
  rr_candidate_cell_db_T *candidate_db_ptr,
  const gas_id_t gas_id
);

extern void grr_activate_reselection_if_allowed(const gas_id_t gas_id);

extern boolean rr_gprs_any_more_rps_cells_to_try(
  rr_reselection_pscan_cell_list_T * cell_list,
  const gas_id_t gas_id
);

extern void grr_reselection_pscan_cell_list_add_entry(
  rr_reselection_pscan_cell_list_T * pscan_cell_list_ptr,
  ARFCN_T                            ARFCN
);

extern void grr_reselection_pscan_cell_list_remove_entry(
  rr_reselection_pscan_cell_list_T * pscan_cell_list_ptr,
  ARFCN_T                            ARFCN
);

extern boolean grr_reselection_pscan_cell_list_check_entry(
  rr_reselection_pscan_cell_list_T *pscan_cell_list_ptr,
  ARFCN_T                           ARFCN
);

extern uint8 grr_get_ncell_priority_class(
  ARFCN_T bcch_arfcn,
  const gas_id_t gas_id
);

extern void rr_gprs_reselection_list_delete_index(
  gprs_reselection_structure_T * reselection_list,
  uint8 i
);

extern void remove_entry_from_reselection_list(
  ARFCN_T arfcn,
  const gas_id_t gas_id
);

extern void rr_clear_better_cell_list(const gas_id_t gas_id);

extern boolean grr_any_other_reselection_candidates(
  ARFCN_T ref_arfcn,
  const gas_id_t gas_id
);

extern gprs_reselection_values_T * ptr_ncell_reselection_list(
  ARFCN_T arfcn,
  const gas_id_t gas_id
);

extern boolean rr_gprs_check_if_any_g2x_resel_timer_is_running (
  const gas_id_t gas_id 
);

#endif /* RR_RESEL_UTILS_H */

/* EOF */

