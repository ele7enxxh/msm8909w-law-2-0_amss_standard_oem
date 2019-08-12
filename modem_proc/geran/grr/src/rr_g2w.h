/*!
  @file rr_g2w.h

  @brief
  Exports all types and functions provided by the rr_g2w.c module.

  This module provides the following functionality:

  - Storage of WCDMA neighbour cell measurement information;
  - Management of databases containing details of WCDMA neighbour cells to
    which reselection has previously been attempted but failed for various
    reasons:
    - Cell too weak;
    - Cell barred;
    - Cell in non-equivalent PLMN;
    - Cell in Forbidden LAI;
*/

/*===========================================================================

  Copyright (c) 2005-2013 Qualcomm Technologies, Inc.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_g2w.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef RR_G2W_H
#define RR_G2W_H

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS)

#include "comdef.h"
#include "rr_defs.h"
#include "rr_log_types.h"
#include "geran_multi_sim.h"

#include "geran_grr_msg.h"
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

#define RR_MAX_WCDMA_FIVE_SECOND_TIMERS 0x06
#define RR_MAX_REJECT_WCDMA_TIMERS      0x06
#define RR_MAX_REJECT_WCDMA_THRESHOLDS  0x06

#define RR_DEFAULT_UTRAN_QXRLEVMIN  0
#define RR_DEFAULT_EUTRAN_QXRLEVMIN 0

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/

/**
  @brief Initialise the G2W module.
*/
extern void rr_g2w_init(
  const gas_id_t gas_id
);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
/**
  @brief Cancels all WCDMA reselection timers.
*/
extern void rr_g2wr_cancel_all_wcdma_five_second_timers(
  const gas_id_t gas_id
);
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS*/

#ifdef FEATURE_WCDMA
/**
  @brief Asserts that the timer indicated is a correct WCDMA reselection
         timer and that it is in expired state.

  @param resel_timer The timer id to validate.
*/
extern void rr_g2wr_update_wcdma_five_second_timer_state(
  rr_timer_id_T resel_timer,
  const gas_id_t gas_id
);
#endif /* FEATURE_WCDMA */

/**
  @brief Clears the list of WCDMA neighbour cell measurements.
*/
extern void rr_g2wr_reset_wcdma_surrounding_database(
  const gas_id_t gas_id
);

#ifdef FEATURE_WCDMA
/**
  @brief Stores WCDMA measurements sent by L1.
*/
extern void rr_g2wr_store_wcdma_surrounding_measurement(
  best_six_wcdma_info_T * p_best_six_info,
  const gas_id_t gas_id
);

/**
  @brief Stores the measurements received in MPH_SURROUND_WCDMA_XFER_MEAS_IND
         from L1.

  @param xfer_meas_ind Pointer to MPH_SURROUND_WCDMA_XFER_MEAS_IND sent by L1.
*/
extern void rr_gprs_store_wcdma_xfer_measurements(
  mph_surround_wcdma_xfer_meas_ind_T *xfer_meas_ind,
  const gas_id_t gas_id
);
#endif /* FEATURE_WCDMA */

/**
  @brief Stores the measurements received in MPH_SURROUND_IRAT_MEAS_IND sent by L1.

  @param surround_irat_meas_ind Pointer to MPH_SURROUND_IRAT_MEAS_IND sent
         by L1.
*/
extern void rr_irat_process_surround_irat_meas_ind(
  mph_surround_irat_meas_ind_t * surround_irat_meas_ind,
  const gas_id_t gas_id
);

/**
  @brief Determines whether rank-based IRAT reselection should be performed
         according to the rules specified in 3GPP 45.008 section 6.6.5.

  If rank-based IRAT reselection should be performed, the global variable
  returned by rr_irat_resel_target() will be set up
  with the target cell data.

  @return TRUE if rank-based IRAT reselection should be performed, FALSE
          otherwise.
*/
extern boolean rr_irat_check_for_rank_based_reselection(
  boolean recent_reselection_flag,
  boolean immediate_reselection,
  boolean ignore_serving_cell_rxlev,
  boolean inhibit_utran_reselection,
  boolean is_c2_based_resel,
  const gas_id_t gas_id
);

#ifdef FEATURE_LTE
/**
  @brief Determines whether priority-based IRAT reselection should be performed
         according to the rules specified in 3GPP 45.008 section 6.6.6.

  If priority-based IRAT reselection should be performed, the global variable
  returned by rr_irat_resel_target() will be set up
  accordingly.

  @return TRUE if priority-based IRAT reselection should be performed, FALSE
          otherwise.
*/
extern boolean rr_irat_check_for_priority_based_reselection(
  boolean priority_resel_to_lower_layers_permitted,
  boolean immediate_reselection,
  const gas_id_t gas_id
);

/**
  @brief Deletes any LTE neighbour cell measurement data.

  This function should be called when reselection to LTE becomes disallowed.
  Any running timers for LTE neighbour cells will be cancelled and measurement
  data will be discarded.
*/
extern void rr_irat_lte_measurements_delete(
  const gas_id_t gas_id
);
#endif /* FEATURE_LTE */

#ifdef FEATURE_WCDMA
/**
  @brief Populate the GPRS_RR_3G_RESELECTION_MEASUREMENTS log packet with WCDMA
         neighbour cell information.

  @param log_ptr Pointer to the log packet to fill in.
*/
extern void rr_irat_populate_gprs_3g_reselection_measurements_log_packet(
  LOG_GPRS_RR_3G_RESELECTION_MEASUREMENTS_C_type * log_ptr,
  const gas_id_t gas_id
);

extern void rr_irat_resel_target_set(
  uint16 uarfcn,
  uint16 scrambling_code,
  uint16 pn_offset,
  boolean diversity,
  const gas_id_t gas_id
);
#endif /* FEATURE_WCDMA */

extern rr_irat_cell_t rr_irat_resel_target_get(
  const gas_id_t gas_id
);

#ifdef FEATURE_WCDMA
/**
  @brief Iterates over each entry in the WCDMA Rejected Cell store, updating L1
         with the appropriate 'ignore' status for the entry.

  For each entry in the Rejected Cell store, if the reject timer has expired
  an MPH_IGNORE_WCDMA_NEIGHBORS_REQ is sent to L1 requesting it to stop
  ignoring the cell and the cell is also removed from the Rejected Cell store.
  If the reject timer is still running then L1 is send an
  MPH_IGNORE_WCDMA_NEIGHBOURS_REQ indicating that it should ignore the cell.
*/
extern void rr_send_rejected_wcdma_cells(
  const gas_id_t gas_id
);
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_WCDMA

/**
  @brief Delete any stored WCDMA measurements.
*/
extern void rr_g2w_wcdma_measurements_delete(
  const gas_id_t gas_id
);

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE

/**
  @brief Delete any stored LTE measurements.
*/
extern void rr_g2w_lte_measurements_delete(
  const gas_id_t gas_id
);

#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_TDS

/**
  @brief Delete any stored UTRAN TDD measurements.
*/
extern void rr_g2w_utran_tdd_measurements_delete(
  const gas_id_t gas_id
);

extern boolean rr_tds_is_measured (const gas_id_t gas_id);

#endif /* FEATURE_GSM_TDS */

#endif /* FEATURE_WCDMA || FEATURE_LTE || FEATURE_GSM_TDS */


#ifdef FEATURE_LTE_REL11 
extern void rr_g2w_update_lte_arfcn_deprioritized_list(geran_grr_lte_depri_freq_req_type *lte_depri_freq_ptr,
                                                       const gas_id_t gas_id);

extern void rr_g2w_update_lte_dedicated_pri_info_with_depri_list(lte_irat_dedicated_priority_list_s *lte_irat_ded_pri_list_ptr,
                                                                 const gas_id_t gas_id);



extern void rr_g2w_depri_update_lte_about_requested_plmn(PLMN_id_T *requested_PLMN_id,
  const gas_id_t gas_id);

#endif /*FEATURE_LTE_REL11*/

#endif /* RR_G2W_H */
